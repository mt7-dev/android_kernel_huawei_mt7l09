/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  spi.c
*
*   作    者 :  wuzechun
*
*   描    述 :  SPI读取驱动
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "sys.h"
#include "OnChipRom.h"
#include "spi.h"
#include "ioinit.h"

const UINT32 spiBase[2] = {ECS_SPI0_BASE, ECS_SPI1_BASE};
/*****************************************************************************
* 函 数 名  : spiInit
*
* 功能描述  : 初始化SPI驱动
*
* 输入参数  : spiNo SPI控制器号
*
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
INT32 spiInit(UINT32 spiNo)
{
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ERROR;
    }

    SPI_IOS_SYS_CONFIG();

    /* 禁止SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), 0);

    /* 禁止SPI Master*/
    OUTREG32(SPI_EN(spiNo), 0);

    /* 配置ctrl0寄存器，命令字长度为1，数据宽度为8,上升沿触发,低电平有效 */
    OUTREG32(SPI_CTRL0(spiNo), (0x07<<SPI_CTRL0_DFS_BITPOS)    \
            | (1<<SPI_CTRL0_SCPH_BITPOS)    \
            | (1<<SPI_CTRL0_SCPOL_BITPOS)   \
            | (0 << SPI_CTRL0_FRF_BITPOS)   \
            | (0x00<<SPI_CFS_BITPOS));

    /* 配置SPI波特率为SSI CLK的1/24，即48/24=2MHz */
    OUTREG32(SPI_BAUD(spiNo), 24);

    /*禁止所有中断信号*/
    OUTREG32(SPI_IMR(spiNo), 0);

    /*屏蔽DMA传输*/
    OUTREG32(SPI_DMAC(spiNo), 0);

    return OK;
}

/*****************************************************************************
* 函 数 名  : spiRecv
*
* 功能描述  : SPI数据轮询接收数据
*
* 输入参数  : spiNo SPI控制器号
*                  cs:   片选号
*                  prevData:   指向要读取数据地址的指针
*                  recvSize：要读取的数据的大小
*                  psendData: 指向要发送的命令和地址的指针
*                  sendSize:  要发送命令和地址的长度(3 或者4)
* 输出参数  :
*
* 返 回 值  : OK or ERROR
*
* 其它说明  :
*
*****************************************************************************/
INT32 spiRecv (UINT32 spiNo, UINT32 cs, UINT8* prevData, UINT32 recvSize,UINT8* psendData,UINT32 sendSize )
{
    UINT8 *pRh;
    UINT8 *pSh;
    UINT32  i;
    UINT32 ulLoop = SPI_MAX_DELAY_TIMES;

    if (((0 != spiNo) && (1 != spiNo))
        || (SPI_CS_PMU != cs && SPI_CS_DEV != cs)
        || (NULL == psendData) || (NULL == prevData) || (0 == recvSize) || (0 == sendSize) ||(((0x1<<16) < recvSize)) )
    {
        return ERROR;
    }

    pRh = prevData;
    pSh = psendData;

    /* 禁止SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), 0);

    /* 禁止SPI Master*/
    OUTREG32(SPI_EN(spiNo), 0);

    /* 设置成EEPROM读模式 */
    SETBITVALUE32(SPI_CTRL0(spiNo), SPI_CTRL0_TMOD_BITMASK, SPI_CTRL0_TMOD_EEPROM_READ);

    /* 设置接收数据的数目*/
    OUTREG32(SPI_CTRL1(spiNo),(recvSize-1));

    //print_info_with_u32("\r\nctrl1:", INREG32(SPI_CTRL1));

    /*使能SPI Master*/
    OUTREG32(SPI_EN(spiNo), 1);

    /*使能SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), cs);

    /* 发送命令 */
    for(i = 0; i < sendSize; i++)
    {
        /* 等待发送FIFO非满 */
        while(!(INREG32(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        OUTREG32(SPI_DR(spiNo), *pSh++);
    }

    /*将发送FIFO中的数据全部发出*/
    while(!(INREG32(SPI_STATUS(spiNo)) & SPI_STATUS_TXEMPTY)
        && (0 != --ulLoop))
    {
    }

    if(0 == ulLoop)
    {
        return ERROR;
    }

    /* 接收数据 */
    for(i = 0; i < recvSize; i++)
    {
        ulLoop = SPI_MAX_DELAY_TIMES;
        /* 等待读取到数据 */
        while(!(INREG32(SPI_STATUS(spiNo)) & SPI_STATUS_RXNOTEMPTY)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        *pRh++ = (UINT8)INREG32(SPI_DR(spiNo));
    }

#if 0 /* SPI控制器会自动禁使能,这里无需软件操作 */
    /* 禁止SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), 0);

    /* 禁止SPI Master*/
    OUTREG32(SPI_EN(spiNo), 0);
#endif
    return OK;
}

/*****************************************************************************
* 函 数 名  : spiSend
*
* 功能描述  : SPI数据轮询发送
*
* 输入参数  : spiNo SPI控制器号
*                  cs:   片选号
*                  pData:   指向要发送数据地址的指针
*                  ulLen：要发送的数据的大小
* 输出参数  :
*
* 返 回 值  : OK or ERROR
*
* 其它说明  :
*
*****************************************************************************/
INT32 spiSend (UINT32 spiNo, UINT32 cs, UINT8* pData, UINT32 ulLen)
{
    UINT8 *pSh;
    UINT32  i;
    UINT32 ulLoop = SPI_MAX_DELAY_TIMES;
    UINT32 ulVal;

    if (((0 != spiNo) && (1 != spiNo))
        || (SPI_CS_PMU != cs && SPI_CS_DEV != cs)
        || (NULL == pData) || (0 == ulLen))
    {
        return ERROR;
    }

    pSh = (UINT8*)pData;

    /* 禁止SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), 0);

    /* 禁止SPI Master*/
    OUTREG32(SPI_EN(spiNo), 0);

    /* 设置成发送模式 */
    SETBITVALUE32(SPI_CTRL0(spiNo), SPI_CTRL0_TMOD_BITMASK, SPI_CTRL0_TMOD_SEND);

    /*使能SPI Master*/
    OUTREG32(SPI_EN(spiNo), 1);

    /*使能SPI Slave*/
    OUTREG32(SPI_SLAVE_EN(spiNo), cs);

    /* 发送命令 */
    for(i = 0; i < ulLen; i++)
    {
        /* 等待发送FIFO非满 */
        while(!(INREG32(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }
        OUTREG32(SPI_DR(spiNo), *pSh++);
    }

    /*将发送FIFO中的数据全部发出,且不BUSY*/
    ulLoop = SPI_MAX_DELAY_TIMES;
    ulVal = INREG32(SPI_STATUS(spiNo));
    while(((!(ulVal & SPI_STATUS_TXEMPTY)) || (ulVal & SPI_STATUS_BUSY))
        && (0 != --ulLoop))
    {
        ulVal = INREG32(SPI_STATUS(spiNo));
    }

    if(0 == ulLoop)
    {
        return ERROR;
    }

    return OK;
}

