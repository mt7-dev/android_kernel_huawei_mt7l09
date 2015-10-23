
#include <osl_sem.h>
#include <bsp_ipc.h>
#include <spi_balong.h>

#if defined(__VXWORKS__)
#define SPI_SEM_FULL    (1)
#define SPI_SEM_EMPTY   (0)
osl_sem_id    spi0_lock;

#define spi0_core_lock()        \
    do {                            \
        osl_sem_down(&spi0_lock);       \
        (void)bsp_ipc_spin_lock(IPC_SEM_SPI0);   \
    } while (0)
#define spi0_core_unlock()      \
    do {                            \
        (void)bsp_ipc_spin_unlock(IPC_SEM_SPI0);   \
        osl_sem_up(&spi0_lock);     \
    } while (0)
#elif defined(__M3_OS__)
#define spi0_core_lock()        \
    do {                                \
        (void)bsp_ipc_spin_lock(IPC_SEM_SPI0);   \
    } while (0)
#define spi0_core_unlock()      \
    do {                            \
        (void)bsp_ipc_spin_unlock(IPC_SEM_SPI0);    \
    } while (0)

#endif

const u32 spiBase[2] = {ECS_SPI0_BASE, ECS_SPI1_BASE};

void balong_spi_init(void)
{
#if defined(__VXWORKS__)
    osl_sem_init(SPI_SEM_FULL, &spi0_lock);
#endif
}

/*****************************************************************************
* 函 数 名  : spiRecv
*
* 功能描述  : SPI数据轮询接收数据
*
* 输入参数  : spiNo:     SPI控制器号
*                       cs:      片选号
*                   prevData:   指向要读取数据地址的指针
*                   recvSize：要读取的数据的大小
*                   psendData: 指向要发送的命令和地址的指针
*                   sendSize:   要发送命令和地址的长度(3 或者4)
* 输出参数  :
*
* 返 回 值  : OK or ERROR
*
* 其它说明  :
*
*****************************************************************************/
s32 balong_spi_recv (u32 spiNo, u32 cs, u16* prevData, u32 recvSize,u16* psendData,u32 sendSize )
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

    spi0_core_lock();
    /* 禁止SPI Slave */
    writel(0,SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master */
    writel(0, SPI_EN(spiNo));

    /* 设置成EEPROM读模式 */
    writel(((readl(SPI_CTRL0(spiNo)) & ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND_RECV),SPI_CTRL0(spiNo));

    /* 设置接收数据的数目 */
    writel((recvSize-1),SPI_CTRL1(spiNo));

    /* 使能SPI Master */
    writel(1, SPI_EN(spiNo));

    /* 使能SPI Slave */
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

        *pRh++ = (u16)readl(SPI_DR(spiNo));

    }
    spi0_core_unlock();

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
* 输入参数  : spiNo: SPI控制器号
*                       cs: 片选号
*                    pData: 指向要发送数据地址的指针
*                     ulLen: 要发送的数据的大小
* 输出参数  :
*
* 返 回 值  : OK or ERROR
*
* 其它说明  :
*
*****************************************************************************/
s32 balong_spi_send (u32 spiNo, u32 cs, u16* pData, u32 ulLen)
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

    spi0_core_lock();
    /* 禁止SPI Slave*/
    writel(0, SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0, SPI_EN(spiNo));

    /* 设置成发送模式 */
    writel(((readl(SPI_CTRL0(spiNo) )& ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND), SPI_CTRL0(spiNo));

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
    spi0_core_unlock();

    if(0 == ulLoop)
    {
        return ERROR;
    }

    return OK;
}


