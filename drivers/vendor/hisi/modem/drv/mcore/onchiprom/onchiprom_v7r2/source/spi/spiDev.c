/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  spiDev.c
*
*   作    者 :  wuzechun
*
*   描    述 :  spi设备驱动程序，主要功能包括区分
*                 SPI挂接的设备是EEPROM还是SFlash，以及根据
*                 设备类型读取的功能。
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "sys.h"
#include "OnChipRom.h"
#include "secBoot.h"
#include "spi.h"
#include "spiDev.h"

/*****************************************************************************
* 函 数 名  : spiDevDetect
*
* 功能描述  : 动态探测SPI总线上接入的设备类型
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
spiReadStatus spiDevDetect( eSpiDevType *devType )
{
    UINT8 ucCmdRDID = SPI_DEV_CMD_RDID;
    UINT32 ulRecv = 0;

    if(NULL == devType)
    {
        return SPI_READ_ERR_PARA;
    }

    /* EEPROM对该命令不支持，会收不到数据超时退出 */
    if(OK != spiRecv(SPI_NO_DEV, SPI_CS_DEV, (UINT8*)&ulRecv, 3, &ucCmdRDID, 1))
    {
        print_info("\r\nread ID err");
        *devType = E_SPI_DEV_TYPE_EEPROM;

        return SPI_READ_OK;
    }

    print_info_with_u32("\r\nID:", ulRecv);

    /* EEPROM不支持该命令，会返回全0或全1 */
    if(0 != ulRecv && 0xFFFFFF != ulRecv)
    {
        *devType = E_SPI_DEV_TYPE_SFLASH;
    }
    else
    {
        *devType = E_SPI_DEV_TYPE_EEPROM;
    }

    return SPI_READ_OK;

}

/*****************************************************************************
* 函 数 名  : spiDevGetStat
*
* 功能描述  : 读EEPROM状态寄存器
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  : EEPROM状态寄存器的值
*
* 其它说明  :
*
*****************************************************************************/
UINT8 spiDevGetStat(void)
{
    UINT8 tx_cmd;
    UINT8 ucSpiDevStat;              /*spiDev状态寄存器的值*/

    tx_cmd = SPI_DEV_CMD_RDSR;

    /*读取SPI设备的状态寄存器*/
    (void)spiRecv(SPI_NO_DEV, SPI_CS_DEV, &ucSpiDevStat,1,&tx_cmd,1);

    //print_info_with_u32("\r\nsta:", ucSpiDevStat);
    return ucSpiDevStat;
}

/*****************************************************************************
* 函 数 名  : spiDevRead
*
* 功能描述  : SPI设备读取接口。
*
* 输入参数  : pReadBuf: 指向数据目的地址的指针
*                 devType: 设备类型,EEPROM或SFlash
* 输出参数  :
*
* 返 回 值  : OK/ERROR
*                SEC_IMAGE_LEN_ERROR: 读取的BootLoader长度错误
*
* 其它说明  :
*
*****************************************************************************/
spiReadStatus spiDevRead(eSpiDevType devType, UINT32 addr, UINT8 *pReadBuf, UINT32 len)
{
    UINT32 u16SendLen = 0;
    UINT32 u16RcveLen = 0;
    UINT32 ulLoop;
    INT32 ret;
    UINT8 ucCmd[4];
    UINT8 ucStatus;

    /*判断输入的合法性，如果合法则继续，不合法则返回。*/
    if(NULL == pReadBuf)
    {
        return SPI_READ_ERR_PARA;
    }

    /* 等待空闲 */
    ulLoop = SPI_MAX_DELAY_TIMES;
    while(((ucStatus = spiDevGetStat()) & SPI_DEV_STATUS_WIP)
        && (0 != --ulLoop))
    {
    }

    if(0 == ulLoop)
    {
        print_info_with_u32("\r\nstatus:", ucStatus);
        return SPI_READ_ERR_GETSTATUS;
    }

    /* 构造命令,从0地址开始读 ，若大于64k，则需要分多次循环读*/
    while(len > 0)
    {
        u16SendLen = 0;
        ucCmd[u16SendLen++] = SPI_DEV_CMD_READ;
        
        if(len > (0x01 << 16))
            u16RcveLen = 0x01 << 16;
        else 
            u16RcveLen = len;

        if(E_SPI_DEV_TYPE_EEPROM == devType)
        {
            /* EEPROM地址为2字节 */
            ucCmd[u16SendLen++] = (UINT8)(addr>>8);
            ucCmd[u16SendLen++] = (UINT8)addr;
        }
        else
        {
            /* 串行Flash地址为3个字节 */
            ucCmd[u16SendLen++] = (UINT8)(addr>>16);
            ucCmd[u16SendLen++] = (UINT8)(addr>>8);
            ucCmd[u16SendLen++] = (UINT8)addr;
        }

        ret = spiRecv(SPI_NO_DEV, SPI_CS_DEV, pReadBuf, u16RcveLen, ucCmd, u16SendLen);
        if(OK != ret)
        {
            return SPI_READ_ERR_READ_BL;
        }
        addr += u16RcveLen;
        pReadBuf += u16RcveLen;
        len -= u16RcveLen;
        
    }

    return SPI_READ_OK;
}

/*****************************************************************************
* 函 数 名  : spiDevReadBl
*
* 功能描述  : SPI设备读取接口。
*
* 输入参数  : pReadBuf: 指向数据目的地址的指针,必须保证字对齐
*
* 输出参数  :
*
* 返 回 值  : OK/ERROR
*                SEC_IMAGE_LEN_ERROR: 读取的BootLoader长度错误
*
* 其它说明  :
*
*****************************************************************************/
spiReadStatus spiDevReadBl(UINT8 *pReadBuf)
{
    UINT32 u16RecvLen;
    eSpiDevType edevType;

    /*判断输入的合法性，如果合法则继续，不合法则返回。*/
    if((NULL == pReadBuf)
        || ((UINT32)pReadBuf % 4))
    {
        return SPI_READ_ERR_PARA;
    }

    (void)spiInit(SPI_NO_DEV);
    (void)spiDevDetect(&edevType);
    if(E_SPI_DEV_TYPE_EEPROM == edevType)
    {
        print_info("\r\nE2PROM_boot!" );
    }
    else
    {
        print_info("\r\nSF_boot!" );
    }

    /* 先获取长度字节 */
    u16RecvLen = BL_LEN_INDEX + 4;
    if(SPI_READ_OK != spiDevRead(edevType, 0, pReadBuf, u16RecvLen))
    {
        return SPI_READ_ERR_READ_LENBYTE;
    }

    u16RecvLen = *(volatile UINT32*)(pReadBuf+BL_LEN_INDEX); /*lint !e826*/  /* 保证pReadBuf是4字节对齐 */

    /* 判断长度是否合法:不为零/字对齐/不翻转/不过大 */
    if((0 == u16RecvLen)
        || (u16RecvLen % 4)
        || (u16RecvLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < u16RecvLen)
        || (u16RecvLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX))
    {
        print_info_with_u32("\r\nBL len err:", u16RecvLen);

        return SPI_READ_ERR_BL_LEN;
    }

    /* 加上镜像签名、OEM CA和OEM CA签名的长度 */
    u16RecvLen += IDIO_LEN + OEM_CA_LEN + IDIO_LEN;

    /* 读取整个BootLoader镜像 */
    if(SPI_READ_OK != spiDevRead(edevType, 0, pReadBuf, u16RecvLen))
    {
        return SPI_READ_ERR_READ_BL;
    }

    return SPI_READ_OK;
}

#ifdef SPI_TEST

#include "string.h"

#define SPI_FLASH_WR_DATA_SIZE 256    /* 分次写入，每次256字节 */
#define SPI_EEPROM_WR_DATA_SIZE 64    /* 分次写入，每次64字节 */
#define SPI_CMD_WR_SIZE_MAX 4

INT32 spiWrtStat(UINT8 val)
{
    UINT32 u16SendLen;
    UINT32 ulLoop;
    UINT8 ucCmd[2];
    UINT8 ucSpiDevStat;              /*spiDev状态寄存器的值*/

    ucCmd[0] = SPI_DEV_CMD_WRSR;
    ucCmd[1] = val;

    /* 构造命令,使能写 */
    ucCmd[0] = SPI_DEV_CMD_WREN;
    u16SendLen = 1;

    /* 发送 */
    if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
    {
        return ERROR;
    }

    /*读取SPI设备的状态寄存器*/
    (void)spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd,2);

    /* 等待编程完成 */
    ulLoop = SPI_MAX_DELAY_TIMES;
    while((spiDevGetStat() & SPI_DEV_STATUS_WIP)
        && (0 != --ulLoop))
    {
    }

    if(0 == ulLoop)
    {
        print_info("\r\nwrite timeout");
        return ERROR;
    }

    /* 构造命令,禁止写 */
    ucCmd[0] = SPI_DEV_CMD_WRDI;
    u16SendLen = 1;

    /* 发送 */
    if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
    {
        return ERROR;
    }

    if(val != (ucSpiDevStat = spiDevGetStat()))
    {
        print_info_with_u32("\r\nsta:", ucSpiDevStat);
    }

    return ERROR;

}
void spiWrtStop(void)
{
    return;
}
/*****************************************************************************
* 函 数 名  : spiDevWrite
*
* 功能描述  : SPI设备写接口。
*
* 输入参数  : pReadBuf: 指向数据目的地址的指针
*                 devType: 设备类型,EEPROM或SFlash
* 输出参数  :
*
* 返 回 值  : OK/ERROR
*                SEC_IMAGE_LEN_ERROR: 读取的BootLoader长度错误
*
* 其它说明  :
*
*****************************************************************************/
int spiDevWrite(eSpiDevType devType, UINT32 destAddr, UINT8 *pBuf, UINT32 ulLen)
{
#define E2PROM_MAX_SIZE (16*0x400)

    UINT32 ulCmdLen = 0;
    UINT32 ulDataLen;
    UINT32 u16SendLen;
    UINT32 ulLenRemain = ulLen;
    UINT32 ulAddrToWrt = destAddr;
    UINT32 ulWrtMaxBytesPerTime;
    UINT8 *pWrtBuf = pBuf;
    UINT32 ulLoop;
    UINT8 ucCmd[SPI_FLASH_WR_DATA_SIZE + SPI_CMD_WR_SIZE_MAX];

    /*判断输入的合法性，如果合法则继续，不合法则返回。*/
    if(NULL == pBuf)
    {
        return ERROR;
    }

    if((E_SPI_DEV_TYPE_EEPROM == devType)
        && (destAddr + ulLen > E2PROM_MAX_SIZE))
    {
        print_info("EEPRPM write out of range\r\n");
        return -1;
    }

    (void)spiInit(SPI_NO_DEV);

    /* 串行Flash和EEPROM每次写入的长度不同 */
    if(E_SPI_DEV_TYPE_SFLASH == devType)
    {
        ulWrtMaxBytesPerTime = SPI_FLASH_WR_DATA_SIZE;
    }
    else
    {
        ulWrtMaxBytesPerTime = SPI_EEPROM_WR_DATA_SIZE;
    }

    /* 串行Flash写之前先擦除 */
    if(E_SPI_DEV_TYPE_SFLASH == devType)
    {
        /* 构造命令,使能擦除和写 */
        ucCmd[0] = SPI_DEV_CMD_WREN;
        u16SendLen = 1;

        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
        {
            return ERROR;
        }

        /* 构造命令,擦除一个sector:1024*256Bytes */
        ucCmd[0] = SPI_DEV_CMD_SE;

        ucCmd[1] = ulAddrToWrt>>16;
        ucCmd[2] = ulAddrToWrt>>8;
        ucCmd[3] = ulAddrToWrt;
        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, 4))
        {
            return ERROR;
        }

        /* 等待擦除完成 */
        print_info("\r\n erasing...");
        ulLoop = SPI_MAX_DELAY_TIMES;
        while((spiDevGetStat() & SPI_DEV_STATUS_WIP)
            && (0 != --ulLoop))
        {
            if(ulLoop % 100 == (100-1))
            {
                print_info(".");
            }
        }

        if(0 == ulLoop)
        {
            print_info("\r\nerase timeout");
            return ERROR;
        }
        print_info_with_u32("\r\nulLoop for erase:", ulLoop);

        /* 构造命令,禁止写 */
        ucCmd[0] = SPI_DEV_CMD_WRDI;
        u16SendLen = 1;

        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
        {
            return ERROR;
        }

    }

    while(ulLenRemain>0)
    {

        /* 构造命令,使能写 */
        ucCmd[0] = SPI_DEV_CMD_WREN;
        u16SendLen = 1;

        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
        {
            return ERROR;
        }

        /* 构造写命令 */
        if(E_SPI_DEV_TYPE_EEPROM == devType)
        {
            ucCmd[0] = SPI_DEV_CMD_WRITE;
        }
        else
        {
            ucCmd[0] = SPI_DEV_CMD_PP;
        }
        //print_info_with_u32("\r\n\r\npragram addr: 0x", ulAddrToWrt);
        /* 构造写命令+地址 */
        if(E_SPI_DEV_TYPE_EEPROM == devType)
        {
            /* EEPROM地址为2字节 */
            ucCmd[1] = ulAddrToWrt >> 8;
            ucCmd[2] = ulAddrToWrt;
            ulCmdLen = 3;
        }
        else
        {
            /* 串行Flash地址为3个字节 */
            ucCmd[1] = ulAddrToWrt >> 16;
            ucCmd[2] = ulAddrToWrt >> 8;
            ucCmd[3] = ulAddrToWrt;
            ulCmdLen = 4;
        }

        ulDataLen = ulLenRemain > ulWrtMaxBytesPerTime ? ulWrtMaxBytesPerTime : ulLenRemain;

        //print_info_with_u32("\r\nbuf addr: 0x", (UINT32)pWrtBuf);
        /* 填充数据 */
        memcpy(ucCmd+ulCmdLen, pWrtBuf, ulDataLen);
        u16SendLen = ulCmdLen + ulDataLen;

        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
        {
            return ERROR;
        }

        /* 等待编程完成 */
        ulLoop = SPI_MAX_DELAY_TIMES;
        while((spiDevGetStat() & SPI_DEV_STATUS_WIP)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            print_info("\r\nwrite timeout");
            return ERROR;
        }
        print_info_with_u32("\r\nulLoop:", ulLoop);

        print_info("#");
        /* 更新以便下次写入 */
        ulAddrToWrt += ulDataLen;
        pWrtBuf += ulDataLen;
        ulLenRemain -= ulDataLen;

        /* 构造命令,禁止写 */
        ucCmd[0] = SPI_DEV_CMD_WRDI;
        u16SendLen = 1;

        /* 发送 */
        if(OK != spiSend(SPI_NO_DEV, SPI_CS_DEV, ucCmd, u16SendLen))
        {
            return ERROR;
        }

    }

    print_info("\r\nspi write success");

    //for trace32 to Stop
    spiWrtStop();

    return OK;
}
#endif

