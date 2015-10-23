#include "sys.h"
#include "string.h"
#include "config.h"
#include "OnChipRom.h"
#include "secBoot.h"
#include "emmcErrcode.h"
#include "emmcConfig.h"
#include "emmcIdmac.h"
#include "emmcNormal.h"

#ifdef EMMC_DEBUG
extern UINT32 emmc_normal_mode_init(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar);
extern UINT32 emmc_write_extcsd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT8 ucIndex, UINT8 ucValue, UINT8 ucAccess);
extern VOID emmc_packet_dump(UINT8 *pcAddr);
extern UINT32 emmc_byte2ulong(UINT8 *pucByte);
extern VOID emmc_ulong2byte(UINT8 *pucByte, UINT32 ulLong);

VOID emmc_download_ok(VOID)
{
    print_info("\r\nemmc download bootloader ok");
}

/*****************************************************************************
* 函 数 名  : emmc_download_bootloader
*
* 功能描述  : 下载bootloader到eMMC/MMC/SD卡中
*
* 输入参数  : UINT8 ucBootPartiCfg  :configurate of partitions
*               ----------------------------------------------------------
*               B7  |B6      | B5 B4 B3            | B2 B1 B0
*               ----------------------------------------------------------
*               RESV|BOOT_ACK|BOOT_PARTITION_ENABLE|PARTITION_ACCESS
*               ----------------------------------------------------------
*               Bit 6:BOOT_ACK (R/W/E)
*                   0x0 : No boot acknowledge sent (default)
*                   0x1 : Boot acknowledge sent during boot operation
*               Bit[5:3] :BOOT_PARTITION_ENABLE (R/W/E)
*               User selects boot data that will be sent to master
*                   0x0 : Device not boot enabled (default)
*                   0x1 : Boot partition 1 enabled for boot
*                   0x2 : Boot partition 2 enabled for boot
*                   0x3-0x6 : Reserved
*                   0x7 : User area enabled for boot
*               Bit[2:0] : PARTITION_ACCESS (before BOOT_PARTITION_ACCESS, R/W/E_P)
*               User selects partitions to access
*                   0x0 : No access to boot partition (default)
*                   0x1 : R/W boot partition 1
*                   0x2 : R/W boot partition 2
*                   0x3 : R/W Replay Protected Memory Block (RPMB)
*                   0x4 : Access to General Purpose partition 1
*                   0x5 : Access to General Purpose partition 2
*                   0x6 : Access to General Purpose partition 3
*                   0x7 : Access to General Purpose partition 4
*
*             UINT8 ucBootBusWidth  :bus width of boot operation
*               ----------------------------------------------------------
*               B7 B6 B5| B4 B3    | B2                  | B1 B0
*               ----------------------------------------------------------
*               RESV    | BOOT_MODE| RESET_BOOT_BUS_WIDTH| BOOT_BUS_WIDTH
*               ----------------------------------------------------------
*               Bit [4:3] : BOOT_MODE (non-volatile)
*                   0:Use single data rate + backward compatible timings in boot
*                     operation (default)
*                   1:Use single data rate + high speed timings in boot operation
*                     mode
*                   2:Use dual data rate in boot operation
*                   3:Reserved
*               Bit [2]: RESET_BOOT_BUS_WIDTH (non-volatile)
*                   0:Reset bus width to x1, single data rate and backward
*                     compatible timings after boot operation
*                   1:Retain BOOT_BUS_WIDTH and BOOT_MODE values after boot
*                     operation.This is relevant to Push-pull mode operation only
*               Bit[1:0] : BOOT_BUS_WIDTH (non-volatile)
*                   0x0 : x1 (sdr) or x4 (ddr) bus width in boot operation mode (default)
*                   0x1 : x4 (sdr/ddr) bus width in boot operation mode
*                   0x2 : x8 (sdr/ddr) bus width in boot operation mode
*                   0x3 : Reserved
*             UINT8 *pcAddr    :bootloader地址
*             UINT32 ulLen     :bootloader长度
*             BOOL bPmuEnable  :是否打开PMU
* 输出参数  : 无
*
* 返 回 值  : 0     :成功
*             其它  :失败
*
* 其它说明  : ucBootPartiCfg和ucBootBusWidth只对4.3以上的协议版本有效
*
*****************************************************************************/
UINT32 emmc_download_bootloader
(
    UINT8 ucBootPartiCfg,
    UINT8 ucBootBusWidth,
    UINT8 *pcAddr,
    UINT32 ulLen
)
{
    UINT8 *pcDdr = pcAddr;
    UINT8 *pcAxi = (UINT8 *)M3_TCM_BL_ADDR;
    UINT32 ulStartBlk;
    UINT32 ulSecCnt = (ulLen+EMMC_SEC_SIZE-1)/EMMC_SEC_SIZE;
    UINT32 ulBootBlk = 0;
    UINT32 ulRetVal = 0;
    UINT8 ucExtData[512];
    UINT32 slot = EMMC_CURRENT_SLOT;
    BOOL bBootSupport = FALSE;
    EMMC_GLOBAL_VAR_S stGlbVar;
    EMMC_GLOBAL_VAR_S *pstGlbVar = &stGlbVar;

   // (VOID)emmc_card_power(FALSE);
    memset((VOID *)ucExtData,0,512);

    print_info("\r\nemmc download bootloader");
    print_info("\r\n--------------------------");
    print_info_with_u32("\r\nucBootPartiCfg =0x",(UINT32)ucBootPartiCfg);
    print_info_with_u32("\r\nucBootBusWidth =0x",(UINT32)ucBootBusWidth);
    print_info_with_u32("\r\npcAddr         =0x",(UINT32)pcAddr);
    print_info_with_u32("\r\nulLen          =0x",ulLen);
    print_info("\r\n--------------------------");

    /* eMMC/MMC/SD初始化 */
    ulRetVal = emmc_normal_mode_init(slot,pstGlbVar);
    if(ulRetVal)
    {
        print_info("\r\nemmc_normal_mode_init failed");
        goto errExit;
    }

    /* 检查是否为4.3以上协议版本的EMMC/MMC卡 */
    if(pstGlbVar->stCardInfo[slot].CSD.Fields.spec_vers >= CSD_SPEC_VER_4)
    {
        if(pstGlbVar->stCardInfo[slot].extcsd_rev>=3)
        {
            print_info("\r\nemmc/mmc card with spec >=4.3");
            bBootSupport = TRUE;
        }
    }

    /* 如果不支持boot操作，则向高地址域写入分区表 */
    if(!bBootSupport)
    {
        /*
         * 向分区表扇区[card_size-1]写入分区表信息
         * -----------------------------------------------
         * 0x0      0x4     0x8(boot len)   0xc(boot addr)
         * -----------------------------------------------
         */
        ulStartBlk = pstGlbVar->stCardInfo[slot].card_size - 1;
        memset((void *)pcAxi, 0, EMMC_SEC_SIZE);
        emmc_ulong2byte(&pcAxi[BL_LEN_INDEX], ulLen);
        ulBootBlk = ulStartBlk - ulSecCnt;
        emmc_ulong2byte(&pcAxi[BL_SEC_INDEX], ulBootBlk);
        print_info("\r\nstart to write partition info");
        print_info("\r\n-----------------------------");
        print_info_with_u32("\r\nulLen    :0x",ulLen);
        print_info_with_u32("\r\nulBootBlk:0x",ulBootBlk);
        print_info_with_u32("\r\nBL_LEN   :0x",emmc_byte2ulong(&pcAxi[BL_LEN_INDEX]));
        print_info_with_u32("\r\nBL_SEC   :0x",emmc_byte2ulong(&pcAxi[BL_SEC_INDEX]));
        print_info("\r\n-----------------------------");

        ulRetVal = emmc_block_write(slot, pstGlbVar, ulStartBlk, pcAxi, EMMC_SEC_SIZE);
        if(ulRetVal)
        {
            print_info_with_u32("\r\nwrite partition info failed,retval=0x",ulRetVal);
            goto errExit;
        }

        /*
         * 读取分区表信息
         */
        print_info("\r\nstart to read partition info");
        memset((void *)pcAxi, 0, EMMC_SEC_SIZE);
        ulRetVal = emmc_block_read(slot, pstGlbVar, ulStartBlk, pcAxi, EMMC_SEC_SIZE);
        if(ulRetVal)
        {
            print_info_with_u32("\r\nread partition info failed,retval=0x",ulRetVal);
            goto errExit;
        }

        /*
         * 比较分区表信息
         */
        print_info("\r\nstart to compare partition info");
        if((ulLen!=emmc_byte2ulong(&pcAxi[BL_LEN_INDEX]))
            ||(ulBootBlk!=emmc_byte2ulong(&pcAxi[BL_SEC_INDEX])))
        {
            print_info("\r\npartition info data unconsistent!");
            print_info_with_u32("\r\nulLen    :0x",ulLen);
            print_info_with_u32("\r\nulBootBlk:0x",ulBootBlk);
            print_info_with_u32("\r\nBL_LEN   :0x",emmc_byte2ulong(&pcAxi[BL_LEN_INDEX]));
            print_info_with_u32("\r\nBL_SEC   :0x",emmc_byte2ulong(&pcAxi[BL_SEC_INDEX]));
            ulRetVal = ERRDATAUNCONSISTENT;
            goto errExit;
        }

        emmc_packet_dump(pcAxi);
    }

    /*
     * 写入PARITION CONFIG、BOOT BUS WIDTH
     */
    if(bBootSupport)
    {
        /* set boot bus width */
        print_info_with_u32("\r\nstart to set boot bus width:0x",ucBootBusWidth);
        if(ucBootBusWidth != pstGlbVar->stCardInfo[slot].extcsd_bootbuswidth)
        {
            ulRetVal = emmc_write_extcsd(slot, pstGlbVar, EMMC_EXT_CSD_BOOT_BUS_WIDTH, ucBootBusWidth, EXTCSD_ACCESS_WRBYTE);
            if(ulRetVal)
            {
                print_info_with_u32("\r\nwrite extcsd boot bus width failed,retval=0x",ulRetVal);
                goto errExit;
            }
        }

        /* set boot partition config */
        print_info_with_u32("\r\nstart to set boot partition config:0x",ucBootPartiCfg);
        if(ucBootPartiCfg != pstGlbVar->stCardInfo[slot].extcsd_partitioncfg)
        {
            ulRetVal = emmc_write_extcsd(slot, pstGlbVar, EMMC_EXT_CSD_BOOTCONFIG, ucBootPartiCfg, EXTCSD_ACCESS_WRBYTE);
            if(ulRetVal)
            {
                print_info_with_u32("\r\nwrite extcsd boot partition config failed,retval=0x",ulRetVal);
                goto errExit;
            }
        }

        ulRetVal = emmc_read_extcsd(slot, pstGlbVar, ucExtData);
        if(ulRetVal)
        {
            print_info_with_u32("\r\nread extcsd failed,retval=0x",ulRetVal);
            goto errExit;
        }

        if((ucBootBusWidth != ucExtData[EMMC_EXT_CSD_BOOT_BUS_WIDTH])
            ||(ucBootPartiCfg != ucExtData[EMMC_EXT_CSD_BOOTCONFIG]))
        {
            print_info("\r\nextcsd compare failed");
            print_info_with_u32("expect ucBootBusWidth:0x",ucBootBusWidth);
            print_info_with_u32("read ucBootBusWidth  :0x",ucExtData[EMMC_EXT_CSD_BOOT_BUS_WIDTH]);
            print_info_with_u32("expect ucBootPartiCfg:0x",ucBootPartiCfg);
            print_info_with_u32("read ucBootPartiCfg  :0x",ucExtData[EMMC_EXT_CSD_BOOTCONFIG]);
        }
    }

    /*
     * 写入bootloader数据
     * -----------------------------------------------
     * 0x0      0x4     0x8(boot len)   0xc
     * -----------------------------------------------
     */
    print_info("\r\nstart to write bootloader");
    memset((void *)pcAxi, 0, ulSecCnt*EMMC_SEC_SIZE);
    memcpy((void *)pcAxi, (const void *)pcDdr, ulLen);
    ulRetVal = emmc_block_write(slot, pstGlbVar, ulBootBlk, pcAxi, ulSecCnt*EMMC_SEC_SIZE);
    if(ulRetVal)
    {
        print_info_with_u32("\r\nwrite boot data failed,retval=0x",ulRetVal);
        goto errExit;
    }

    print_info("\r\nstart to read bootloader");
    memset((void *)pcAxi, 0, ulSecCnt*EMMC_SEC_SIZE);
    ulRetVal = emmc_block_read(slot, pstGlbVar, ulBootBlk, pcAxi, ulSecCnt*EMMC_SEC_SIZE);
    if(ulRetVal)
    {
        print_info_with_u32("\r\nread bootloader data failed,retval=0x",ulRetVal);
        goto errExit;
    }

    if(memcmp((const void *)pcAxi, (const void *)pcDdr, ulLen))
    {
        UINT32 i;
        print_info_with_u32("\r\nboot data unconsistent!0x",ulLen);
        for(i=0;i<ulLen;i++)
        {
            if(pcAxi[i]!=pcDdr[i])
            {
                print_info_with_u32("\r\n",i);
                print_info_with_u32(":",pcAxi[i]);
                print_info_with_u32(",",pcDdr[i]);
            }
        }

        ulRetVal = ERRDATAUNCONSISTENT;
        goto errExit;
    }

    print_info_with_u32("\r\nboot data len:0x",ulLen);

    emmc_packet_dump(pcAxi);

    TIMER0_DEINIT();
   // (VOID)emmc_card_power(FALSE);
    print_info("\r\nemmc download bootloader ok");
    emmc_download_ok();

errExit:
    TIMER0_DEINIT();
//    (VOID)emmc_card_power(FALSE);
    return ulRetVal;
}

#if 0
UINT32 emmc_download_bootloader_pmupoweron
(
    UINT8 ucBootPartiCfg,
    UINT8 ucBootBusWidth,
    UINT8 *pcAddr,
    UINT32 ulLen
)
{
    return emmc_download_bootloader(ucBootPartiCfg,ucBootBusWidth,pcAddr,ulLen,TRUE);
}

UINT32 emmc_download_bootloader_pmupoweroff
(
    UINT8 ucBootPartiCfg,
    UINT8 ucBootBusWidth,
    UINT8 *pcAddr,
    UINT32 ulLen
)
{
    return emmc_download_bootloader(ucBootPartiCfg,ucBootBusWidth,pcAddr,ulLen,FALSE);
}
#endif
#endif
