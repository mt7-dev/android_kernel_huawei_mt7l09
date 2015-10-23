/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  emmcBoot.c
*
*   作    者 :  wangzhongshun
*
*   描    述 :  boot工作模式
*
*   修改记录 :  2011年6月15日  v1.00  wangzhongshun  创建
*
*************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "secBoot.h"
#include "OnChipRom.h"
#include "emmcConfig.h"
#include "emmcErrcode.h"
#include "emmcIp.h"
#include "emmcIdmac.h"
#include "emmcNormal.h"
#include "emmcMain.h"

extern UINT32 emmc_poll_cmd_accept(VOID);
extern UINT32  emmc_send_command(EMMC_GLOBAL_VAR_S *pstGlbVar, COMMAND_INFO_S *pstCmdInfo);
extern UINT32 emmc_set_preidle(UINT32 slot);

/*****************************************************************************
* 函 数 名  : emmc_start_boot
*
* 功能描述  : 启动boot模式
*
* 输入参数  : UINT32 ulAddress  :数据地址
*             UINT32 ulSize     :数据长度
* 输出参数  : 无
*
* 返 回 值  : 0   :成功
*             其它:失败
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_start_boot(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddress, UINT32 ulSize)
{
    UINT32 ulRetVal = 0;
    COMMAND_INFO_S stCmdInfo = {{0}};/*lint !e708*/
    volatile UINT32  ulData = 0;

    ulData = INREG32(EMMC_REG_RINTSTS);
    OUTREG32(EMMC_REG_RINTSTS,ulData);

    /* 清除所有的中断状态 */
    EMMC_DMA_INTERRUPT_CLEAR(DmaStatusAbnormal|DmaStatusNormal|DmaStatusCardErrSum|DmaStatusNoDesc|
                                DmaStatusBusError|DmaStatusRxCompleted|DmaStatusTxCompleted);

    stCmdInfo.BlkSize = EMMC_SEC_SIZE;
    stCmdInfo.ByteCnt = ((ulSize + EMMC_SEC_SIZE - 1) / EMMC_SEC_SIZE) * EMMC_SEC_SIZE;
    stCmdInfo.CmdArg = 0;
    stCmdInfo.CmdReg.reg = EMMC_CMD_BIT_START
                            | EMMC_CMD_BIT_ENABLE_BOOT
                            | EMMC_CMD_BIT_DATA_EXPECTED;

    if(pstGlbVar->stCardInfo[slot].extcsd_partitioncfg&EXTCSD_BOOT_ACK)
    {
        stCmdInfo.CmdReg.reg |= EMMC_CMD_BIT_EXPECT_BOOT_ACK;
    }

    emmc_idmac_nocopyreceive(&pstGlbVar->stIdmacHandle, (UINT32)ulAddress, (UINT32)stCmdInfo.ByteCnt, EMMC_IDMAC_GET_DESC_COUNT(stCmdInfo.ByteCnt));

    /*
        Config completed, try send command and let emmc enter boot mode.
        This is the first command for card.
    */
    ulRetVal = emmc_send_command(pstGlbVar, &stCmdInfo);
    return ulRetVal;
}

/*****************************************************************************
* 函 数 名  : emmc_exit_boot
*
* 功能描述  : 退出boot模式
*
* 输入参数  : VOID
* 输出参数  : 无
*
* 返 回 值  : 0   :成功
*             其它:失败
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_exit_boot(EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal = 0;
    COMMAND_INFO_S stCmdInfo = {{0}};/*lint !e708*/

    /*
        Last step, stop the boot mode. needn't check the result,
        because we don't known how to handle it.
    */
    stCmdInfo.CmdReg.reg = EMMC_CMD_BIT_START
                            | EMMC_CMD_BIT_DISABLE_BOOT
                            | EMMC_CMD_BIT_STOP_ABORT;
    stCmdInfo.CmdArg = 0x0;
    stCmdInfo.BlkSize = 0;
    stCmdInfo.ByteCnt = 0;

    ulRetVal = emmc_send_command(pstGlbVar, &stCmdInfo);
    return ulRetVal;
}

/*****************************************************************************
* 函 数 名  : emmc_wait_boot_ack
*
* 功能描述  : 等待boot ack
*
* 输入参数  : void
* 输出参数  : 无
*
* 返 回 值  : 0   :成功
*             其它:失败
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_wait_boot_ack(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    TIMER0_VAR_DECLAR;
    volatile UINT32  ulStatus  = 0;

    if(!(pstGlbVar->stCardInfo[slot].extcsd_partitioncfg&EXTCSD_BOOT_ACK))
    {
        return ERRNOERROR;
    }

    TIMER0_INIT_TIMEOUT(EMMC_BOOT_ACK_TIMEOUT); /* Timeout is 50ms */
    for(;;)
    {
        ulStatus = INREG32( EMMC_REG_RINTSTS );
        if(ulStatus & EMMC_INT_BIT_BAR)
        {
            return ERRNOERROR;
        }

        if(TIMER0_IS_TIMEOUT())
        {
            return ERRHARDWARE;
        }
    }
}

/*****************************************************************************
* 函 数 名  : emmc_wait_data_start
*
* 功能描述  : 等待botot数据开始
*
* 输入参数  : void
* 输出参数  : 无
*
* 返 回 值  : 0   :成功
*             其它:失败
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_wait_data_start(VOID)
{
    TIMER0_VAR_DECLAR;
    volatile UINT32  ulStatus = 0;

    TIMER0_INIT_TIMEOUT(EMMC_BOOT_DATA_START_TIMEOUT); /* timeout is 1000 ms */
    for(;;)
    {
        ulStatus = INREG32(EMMC_REG_RINTSTS);
        if(ulStatus & EMMC_INT_BIT_BDS) /* Is data start ? */
        {
            return ERRNOERROR;
        }

        if(TIMER0_IS_TIMEOUT())
        {
            return ERRHARDWARE;
        }
    }
}

UINT32 emmc_read_data_boot_mode(VOID)
{
    TIMER0_VAR_DECLAR;
    UINT32 ulRetVal = ERRNOERROR;
    volatile UINT32 ulStatus = 0;
    volatile UINT32 ulRecord = 0;

    TIMER0_INIT_TIMEOUT(EMMC_BOOT_DATA_READ_TIMEOUT);
    for(;;)
    {
        ulStatus = INREG32(EMMC_REG_RINTSTS);
#ifdef EMMC_DEBUG
        print_info_with_u32("\r\nRINTSTS: ", ulStatus);
#endif
        ulRecord |= ulStatus & (EMMC_INT_BIT_CD|EMMC_INT_BIT_DTO);
        if((EMMC_INT_BIT_CD|EMMC_INT_BIT_DTO) == ulRecord)
        {
            ulRetVal = ERRNOERROR;
            break;
        }

        if(TIMER0_IS_TIMEOUT())
        {
            ulRetVal = ERRHARDWARE;
            goto read_data_boot_exit;
        }
    }

read_data_boot_exit:
    ulStatus = INREG32(EMMC_REG_RINTSTS);
    OUTREG32(EMMC_REG_RINTSTS,ulStatus);

    return ulRetVal;
}

/*****************************************************************************
* 函 数 名  : emmc_read_bootloader_boot_mode_without_stop
*
* 功能描述  : boot模式读取bootloader
*
* 输入参数  : UINT32 ulAddress
* 输出参数  : 无
*
* 返 回 值  : 0   :成功
*             其它:失败
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_read_bootloader_boot_mode_without_stop
    (UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddress, UINT32 *len)
{
    UINT32 ulRetVal = 0;
    UINT32 ulBootLen = 0;

    emmc_config_od(FALSE);

    if((ulRetVal = emmc_set_preidle(slot)))
    {
        return ulRetVal;
    }

    /*
        start boot
    */
    if((ulRetVal = emmc_start_boot(slot, pstGlbVar,ulAddress,*len)))
    {
        return ulRetVal;
    }

    /* emmc_print_current_tick(); */
    if((ulRetVal = emmc_wait_boot_ack(slot, pstGlbVar)))
    {
        /* emmc_print_current_tick(); */
        print_info( "\r\nwait boot ack fail" );
        return ulRetVal;
    }

    /* emmc_print_current_tick(); */
    ulRetVal = emmc_wait_data_start();
    if(ulRetVal)
    {
        /* emmc_print_current_tick(); */
        print_info((UINT8 *)"\r\nwait data start fail");
        return ulRetVal;
    }

    /* emmc_print_current_tick(); */
    ulRetVal = emmc_read_data_boot_mode();
    /* after read boot data, exit boot opration */
    (VOID)emmc_exit_boot(pstGlbVar);
    if(ERRNOERROR == ulRetVal)
    {
        /* 获取BootLoader长度 */
        ulBootLen = *(volatile UINT32 *)(ulAddress + BL_LEN_INDEX);

        /* 判断长度是否越界，防止翻转 */
        if((0 == ulBootLen)
            ||(ulBootLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < ulBootLen)
            ||(ulBootLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX))
        {
            print_info_with_u32("\r\nBL len err:0x", ulBootLen);
            return ERRBOOTLEN;
        }

        /* 加上镜像签名、OEM CA和OEM CA签名的长度 */
        ulBootLen += IDIO_LEN + OEM_CA_LEN + IDIO_LEN;
    }
    *len = ulBootLen;
    return ulRetVal;
}
UINT32 emmc_read_bootloader_boot_mode(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddress)
{
    UINT32 ulRetVal = 0;
    UINT32 len = BL_LEN_INDEX + 4;
    UINT32 retry_time = 2;
    while (retry_time-- && !ulRetVal) {

        ulRetVal = emmc_read_bootloader_boot_mode_without_stop(slot, pstGlbVar, ulAddress, &len);
    }

    return ulRetVal;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
