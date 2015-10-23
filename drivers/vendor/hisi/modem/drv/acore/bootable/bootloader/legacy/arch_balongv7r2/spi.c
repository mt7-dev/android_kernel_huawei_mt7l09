
#include <product_config.h>
#include <boot/boot.h>
#include <balongv7r2/spi.h>
#include <balongv7r2/parameters.h>
#include <hi_syscrg_interface.h>
#include <soc_memmap.h>

const u32 spiBase[2] = {ECS_SPI0_BASE, ECS_SPI1_BASE};

void spi_clk_enable(u32 spiNo)
{
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ;
    }

    if(spiNo)
        hi_syscrg_spi1_clk_enable();
    else
        hi_syscrg_spi0_clk_enable();
}

s32 spi_init(u32 spiNo)
{
    u32 reg;
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ERROR;
    }

    /*打开clk*/
    spi_clk_enable(spiNo);
    /*三线模式*/
    reg = readl(HI_SYSCTRL_BASE_ADDR+0x040C);
    reg &= ~0x100;
    writel(reg,HI_SYSCTRL_BASE_ADDR+0x040C);
    mdelay(10);//10ms

    /* 禁止SPI Slave*/
    writel(0,SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0,SPI_EN(spiNo));

    if(spiNo)
    {   /*spi1*/
        writel((0x08<<SPI_CTRL0_DFS_BITPOS)    \
                | (0<<SPI_CTRL0_SCPH_BITPOS)    \
                | (0<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));
         /* 配置SPI波特率为SSI CLK的1/24，即48/6=8MHz */
        writel(6,SPI_BAUD(spiNo));
    }
    else
    {
        /*spi0*/
        /* 配置ctrl0寄存器，命令字长度为1，数据宽度为8,上升沿触发,低电平有效 */
        writel((0x0f<<SPI_CTRL0_DFS_BITPOS)    \
                | (1<<SPI_CTRL0_SCPH_BITPOS)    \
                | (1<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));

    /* 配置SPI波特率为SSI CLK的1/24，即48/24=2MHz */
    writel(24,SPI_BAUD(spiNo));
    }

    /*禁止所有中断信号*/
    writel(0,SPI_IMR(spiNo));

    /*屏蔽DMA传输*/
    writel(0, SPI_DMAC(spiNo));

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
s32 spi_recv (u32 spiNo, u32 cs, u16* prevData, u32 recvSize,u16* psendData,u32 sendSize )
{
    u16 *pRh;
    u16 *pSh;
    u32  i;
    u32 ulLoop = SPI_MAX_DELAY_TIMES;

    if (((0 != spiNo) && (1 != spiNo))
        || (SPI_CS_PMU != cs && SPI_CS_DEV != cs)
        || (NULL == psendData) || (NULL == prevData) || (0 == recvSize) || (0 == sendSize))
    {
        return ERROR;
    }

    pRh = prevData;
    pSh = psendData;

    /* 禁止SPI Slave*/
    writel(0,SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0, SPI_EN(spiNo));

    /* 设置成EEPROM读模式 */
    /*writel((readl(SPI_CTRL0(spiNo)) | SPI_CTRL0_TMOD_EEPROM_READ),SPI_CTRL0(spiNo));*/
    writel(((readl(SPI_CTRL0(spiNo)) & ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND_RECV),SPI_CTRL0(spiNo));

    /* 设置接收数据的数目*/
    writel((recvSize-1),SPI_CTRL1(spiNo));

    /*使能SPI Master*/
    writel(1, SPI_EN(spiNo));

    /*使能SPI Slave*/
    writel(cs, SPI_SLAVE_EN(spiNo));

    /* 发送命令 */
    for(i = 0; i < sendSize; i++)
    {
        /* 等待发送FIFO非满 */
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        writel(*pSh++, SPI_DR(spiNo));
    }

    /*将发送FIFO中的数据全部发出*/
    while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXEMPTY)
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
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_RXNOTEMPTY)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        *pRh++ = readl(SPI_DR(spiNo));

    }

/* SPI控制器会自动禁使能,这里无需软件操作 */
#if 0
    /* 禁止SPI Slave*/
    writel(0, SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0, SPI_EN(spiNo));
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
s32 spi_send (u32 spiNo, u32 cs, u16* pData, u32 ulLen)
{
    u16 *pSh;
    u32  i;
    u32 ulLoop = SPI_MAX_DELAY_TIMES;
    u32 ulVal;

    if (((0 != spiNo) && (1 != spiNo))
        || (SPI_CS_PMU != cs && SPI_CS_DEV != cs)
        || (NULL == pData) || (0 == ulLen))
    {
        return ERROR;
    }

    pSh = (u16*)pData;

    /* 禁止SPI Slave*/
    writel(0, SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0, SPI_EN(spiNo));

    /* 设置成发送模式 */
    writel(((readl(SPI_CTRL0(spiNo)) & ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND),SPI_CTRL0(spiNo));

    /*使能SPI Master*/
    writel(1, SPI_EN(spiNo));

    /*使能SPI Slave*/
    writel(cs, SPI_SLAVE_EN(spiNo));

    /* 发送命令 */
    for(i = 0; i < ulLen; i++)
    {
        /* 等待发送FIFO非满 */
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }
        writel(*pSh++, SPI_DR(spiNo));
    }

    /*将发送FIFO中的数据全部发出,且不BUSY*/
    ulLoop = SPI_MAX_DELAY_TIMES;
    ulVal = readl(SPI_STATUS(spiNo));
    while(((!(ulVal & SPI_STATUS_TXEMPTY)) || (ulVal & SPI_STATUS_BUSY))
        && (0 != --ulLoop))
    {
        ulVal = readl(SPI_STATUS(spiNo));
    }

    if(0 == ulLoop)
    {
        return ERROR;
    }

    return OK;
}

/*****************************************************************************
* 函 数 名  : spiDevDetect
*
* 功能描述  : 动态探测SPI总线上接入的设备类型，验证SPI总线驱动
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 spi_dev_detect( eSpiDevType *devType )
{
    u16 ucCmdRDID = SPI_DEV_CMD_RDID;
    u32 ulRecv = 0;

    /*cprintf("\r\nspiDevDetect begin!" );*/
    if(NULL == devType)
    {
        cprintf("\r\nPARA *devType is NULL!");
        return ERROR;
    }

    /* EEPROM对该命令不支持，会收不到数据超时退出 */
    if(OK != spi_recv(SPI_NO_DEV, SPI_CS_DEV, (u16*)&ulRecv, 3, &ucCmdRDID, 1))
    {
        cprintf("\r\nread ID err");
        *devType = E_SPI_DEV_TYPE_EEPROM;

        return OK;
    }

    cprintf("\r\nID: %x", ulRecv);

    /* EEPROM不支持该命令，会返回全0或全1 */
    if(0 != ulRecv && 0xFFFFFF != ulRecv)
    {
        *devType = E_SPI_DEV_TYPE_SFLASH;
    }
    else
    {
        *devType = E_SPI_DEV_TYPE_EEPROM;
    }
    cprintf("\r\ndevType is :  @ %x", *devType);

    return OK;

}
s32 spitest()
{
    eSpiDevType edevType;

    (void)spi_init(SPI_NO_DEV);

    (void)spi_dev_detect(&edevType);

    if(E_SPI_DEV_TYPE_EEPROM == edevType)
    {
        cprintf("\r\nE2PROM_boot!" );
    }
    else
    {
        cprintf("\r\nSF_boot!" );
    }
    return 0;
}