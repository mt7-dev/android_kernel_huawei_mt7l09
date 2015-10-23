/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  hsuart.c
*
*   作    者 :  wuzechun
*
*   描    述 :  高速串口驱动，主要完成高速串口的初始化、收、发操作。
*
*   修改记录 :  2011年6月13日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "types.h"
#include "sys.h"
#include "OnChipRom.h"
#include "string.h"
#include "hsuart.h"
#include "secBoot.h"
#include "ioinit.h"


/*****************************************************************************
* 函 数 名  : IsQueNotFull
*
* 功能描述  : 高速串口接收缓冲区队列未满
*
* 输入参数  : pstQue 缓冲区队列指针
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int IsQueNotFull(RECV_STR *pstQue)
{
    if(NULL == pstQue)
    {
        return ERROR;
    }

    if(pstQue->ulTotalNum < HSUART_BUFFER_SIZE)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

/*****************************************************************************
* 函 数 名  : InQue
*
* 功能描述  : 高速串口接收缓冲区入队函数
*
* 输入参数  : pstQue 缓冲区队列指针
*                 ucData 接收到的字符
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int InQue(RECV_STR *pstQue, UINT8 ucData)
{
    UINT32 ulTail = 0;

    if(NULL == pstQue)
    {
        return ERROR;
    }

    if(ERROR == IsQueNotFull(pstQue))
    {
        return ERROR;
    }

    ulTail = pstQue->ulWrite;
    pstQue->ucData[ulTail] = ucData;
    ulTail = ((ulTail+1) ==  HSUART_BUFFER_SIZE) ? 0 : (ulTail+1);
    pstQue->ulTotalNum++;
    pstQue->ulWrite = ulTail;

    return OK;
}

/*****************************************************************************
* 函 数 名  : InQue
*
* 功能描述  : 高速串口接收缓冲区出队函数
*
* 输入参数  : pstQue 缓冲区队列指针
*                 pBuf 从接收缓冲区读取的数据存放位置
*                 ulLong 欲读取的长度
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int OutQue(RECV_STR *pstQue, UINT8 *pBuf,UINT32 ulLong)
{
    UINT32 ulRead = 0;

    if((NULL == pstQue) || (NULL == pBuf) || (0 == ulLong))
    {
        return ERROR;
    }

    if (pstQue->ulTotalNum < ulLong)
    {
        return  ERROR;
    }

    ulRead = pstQue->ulRead;

    while(ulLong)
    {
        *pBuf = pstQue->ucData[ulRead];
        ulRead = ((ulRead+1) == HSUART_BUFFER_SIZE)?0:(ulRead+1);
        pstQue->ulTotalNum--;
        ulLong--;
        pBuf++;
    }

    //print_info_with_u32("\r\n ulRead = ", ulRead);
    pstQue->ulRead = ulRead;

    return OK;
}



/*****************************************************************************
* 函 数 名  : hsUartInit
*
* 功能描述  : HS UART的基本参数的初始化
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : HSUART_STATUS_OK 设置成功
*                HSUART_STATUS_BAUTRATE_ERR 设置波特率超时失败
*
* 修改记录  :2010年12月16日   鲁婷  创建
*****************************************************************************/
HSUART_STATUS hsUartInit(RECV_STR *pstQue)
{
    UINT32 u32Divisor = 0;
    UINT32 u32DivisorLow = 0;
    UINT32 u32DivisorHigh = 0;
    UINT32 u32Discard = 0;
    UINT32 u32Fcr = 0;
    UINT32 u32Times = 0;

    if(NULL == pstQue)
    {
        return HSUART_STATUS_PARA_ERR;
    }

    /* 管脚复用为高速串口 */
    HSUART_IOS_SYS_CONFIG();

    /* 计算分频比 */
    u32Divisor = HSUART_CLOCK_FREQ / (16 * HSUART_BAUT_RATE_DEFAULT);
    u32DivisorHigh = (u32Divisor&0xFF00) >> 8;
    u32DivisorLow = u32Divisor&0xFF;

    /*写FCR，把接收FIFO 和发送FIFO 全部复位清空,设置FIFO使能，非DMA Model */
    u32Fcr = UART_DEF_RT_ONEFOUR|UART_DEF_TET_NULL|UART_DEF_NO_DMA_MODE|UART_RESET_TX_FIFO|UART_RESET_RCV_FIFO|UART_DEF_FIFOE_ENABLE;

    OUTREG32(HSUART_BASE_ADDR + UART_FCR, u32Fcr);

    /*去使能所有中断*/
    OUTREG32(HSUART_BASE_ADDR + UART_IER, 0x00);

    /*clear line status interrupt*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_LSR);
    /*clear rcv interrupt*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_RBR);
    /*clear send empty interrupt*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_IIR);
    /*clear line busy interrupt*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_USR);

    /* 读取UART 状态寄存器，看UART是否处于idle状态*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_USR);
    while((u32Discard & 0x01)&&(u32Times<1000))/*uart is busy*/
    {
    	u32Discard = INREG32(HSUART_BASE_ADDR + UART_USR);
        u32Times++;
    }
    if(1000 == u32Times)
    {
        return HSUART_STATUS_BAUTRATE_ERR;
    }

    /* USR bit[0]=0 即UART 处在idle 态时，才能对LCR bit[7]该位进行写操作*/
    OUTREG32(HSUART_BASE_ADDR + UART_LCR, UART_DEF_LCR_DIVENABLE);

    /* 配置DLH 和DLL 寄存器，设置分频比*/
    OUTREG32(HSUART_BASE_ADDR +  UART_DLL, u32DivisorLow);
    OUTREG32(HSUART_BASE_ADDR +  UART_DLH, u32DivisorHigh);

    /* 配置外围发送和接收数据的数目为8bit,1停止位,无校验位,disable DLL&DLH */
    OUTREG32(HSUART_BASE_ADDR +  UART_LCR, UART_DEF_PARITY_NONE|UART_DEF_LCR_STP1|UART_DEF_LCR_CS8);

    /* 配置MCR ,rts,打开流控*/
    OUTREG32(HSUART_BASE_ADDR +  UART_MCR, 0x22);

    /* 配置发送FIFO */
    OUTREG32(HSUART_BASE_ADDR +  UART_TXDEPTH, 10);

    /* 使用接收中断 */
    OUTREG32(HSUART_BASE_ADDR +  UART_IER, 0x11);

    pstQue->ulRead = 0;
    pstQue->ulWrite = 0;
    pstQue->ulTotalNum = 0;

    /* 初始化超时定时器 */
    TIMER0_INIT();

    return HSUART_STATUS_OK;
}

void send4(UINT32 * pucSrcAddr ,UINT32 ulTimes)
{
    UINT32 regval;
    UINT32 * pu32Buffer;

    pu32Buffer = pucSrcAddr;

    while(ulTimes)
    {
        regval = INREG32(HSUART_BASE_ADDR + UART_USR);
        if (0 != (regval & 0x02))
        {
            /* 将缓冲区中的数据放到数据寄存器中*/
            *(UINT32 *)(HSUART_BASE_ADDR + UART_THR) = *pu32Buffer;
            pu32Buffer++;
        	ulTimes--;
        }
        else
        {
            //print_info("Tx FIFO full\r\n");
        }
    }
    return;
}

void send2(UINT16 * pucSrcAddr ,UINT32 ulTimes)
{
    UINT32 regval;
    UINT16 * pu16Buffer;

    pu16Buffer = pucSrcAddr;

    while(ulTimes)
    {
        regval = INREG32(HSUART_BASE_ADDR + UART_USR);
        if (0 != (regval & 0x02))
        {
            /* 将缓冲区中的数据放到数据寄存器中*/
            *(UINT16 *)(HSUART_BASE_ADDR + UART_THR) = *pu16Buffer;
            pu16Buffer++;
        	ulTimes--;
        }
        else
        {
            //print_info("Tx FIFO full\r\n");
        }
    }
    return;
}

void send1(UINT8 * pucSrcAddr ,UINT32 ulTimes)
{
    UINT32 regval;
    UINT8 * pu8Buffer;

    pu8Buffer = pucSrcAddr;

    while(ulTimes)
    {
        regval = INREG32(HSUART_BASE_ADDR + UART_USR);
        if (0 != (regval & 0x02))
        {
            /* 将缓冲区中的数据放到数据寄存器中*/
            *(UINT8 *)(HSUART_BASE_ADDR + UART_THR) = *pu8Buffer;
            pu8Buffer++;
        	ulTimes--;
        }
        else
        {
            //print_info("Tx FIFO full\r\n");
        }
    }
    return;
}

/*****************************************************************************
* 函 数 名  : hsUartSend
*
* 功能描述  : HS UART发送数据接口函数
*
* 输入参数  : UINT32  u32SrcAddr       需发送的数据的首地址
*             UINT32  u32TransLength   需发送的数据的长度
*
* 输出参数  : 无
* 返 回 值  : OK       成功
*             ERROR    失败
*
* 修改记录  :2010年12月16日   鲁婷  创建
*****************************************************************************/
HSUART_STATUS hsUartSend(UINT8 * pucSrcAddr, UINT32 u32TransLength)
{
//    UINT32 regval;
    UINT8 * pu8Buffer;
    UINT32 ulTimes;
    UINT32 ulLeft;

    /* 参数的有效性检查 */
    if((NULL == pucSrcAddr) || (0 == u32TransLength))
    {
    	return HSUART_STATUS_PARA_ERR;
    }

    pu8Buffer = pucSrcAddr;

    ulTimes = u32TransLength / 4;
    ulLeft = u32TransLength % 4;

    send4((UINT32*)pu8Buffer, ulTimes); /*lint !e826*/
    pu8Buffer = pu8Buffer + ulTimes*4;

    if(ulLeft == 1)
    {
        send1(pu8Buffer, 1);
    }
    else if(ulLeft == 2)
    {
        send2((UINT16*)pu8Buffer, 1); /*lint !e826*/
    }
    else if(ulLeft == 3)
    {
        send2((UINT16*)pu8Buffer, 1); /*lint !e826*/
        pu8Buffer = pu8Buffer + 1*2;
        send1(pu8Buffer, 1);
    }

    return HSUART_STATUS_OK;
}

/*****************************************************************************
* 函 数 名  : HSUART_Recv
*
* 功能描述  : HS UART接收数据接口函数，读取有效数据时需要
*                 4字节对齐，否则会多读到后面的有效数据并丢弃,
*                 下次读取会失败。
*
* 输入参数  : UINT8 *  pu8DstAddr       接收到的数据存放的的首地址
*             UINT32  u32TransLength   接收的数据的长度
* 输出参数  : 无
* 返 回 值  : OK       成功
*             ERROR    失败
*
* 修改记录  :2010年12月16日   鲁婷  创建
*****************************************************************************/
HSUART_STATUS hsUartRecv(RECV_STR *pstHsUartRecvData)
{
    UINT32 ulCharNum;
    UINT32 ulData;
    UINT8* ucTemp;
    UINT32 i;
    UINT32 ulInt = 0;

    ulInt = INREG32(HSUART_BASE_ADDR + UART_IIR);
    ulInt &= 0xF;

    if(ulInt == 0xC)
    {
        ulCharNum = INREG32(HSUART_BASE_ADDR + UART_RFL);
        while(ulCharNum)
        {
            if(ulCharNum >= 4)
            {
                ulData = INREG32(HSUART_BASE_ADDR + UART_RBR);
                ucTemp = (UINT8*)&ulData;
                for(i=0;i<4;i++)
                {
                    if(InQue(pstHsUartRecvData, ucTemp[i]) != OK)
                    {
                        print_info("\r\nbuffer full\r\n");
                        return HSUART_STATUS_BUFFER_FULL_ERR;
                    }
                }
                ulCharNum = ulCharNum - 4;
            }
            else
            {
                ulData = INREG32(HSUART_BASE_ADDR + UART_RBR);
                ucTemp = (UINT8*)&ulData;
                for(i=0;i<ulCharNum;i++)
                {
                    if(InQue(pstHsUartRecvData, ucTemp[i]) != OK)
                    {
                        print_info("\r\nbuffer full\r\n");
                        return HSUART_STATUS_BUFFER_FULL_ERR;
                    }
                }
                return HSUART_STATUS_OK;
            }
            //ulCharNum = INREG32(HSUART_BASE_ADDR + UART_RFL);
        }

    }

    else if(ulInt == 0x4)
    {
        ulCharNum = INREG32(HSUART_BASE_ADDR + UART_USR);
        while(ulCharNum & 0x8)
        {
            ulData = INREG32(HSUART_BASE_ADDR + UART_RBR);
            ucTemp = (UINT8*)&ulData;
            for(i=0;i<4;i++)
            {
                if(InQue(pstHsUartRecvData, ucTemp[i]) != OK)
                {
                    print_info("\r\nbuffer full\r\n");
                    return HSUART_STATUS_BUFFER_FULL_ERR;
                }
            }
            ulCharNum = INREG32(HSUART_BASE_ADDR + UART_USR);
        }

    }

    return HSUART_STATUS_OK;
}


INT32 hsUartRecvData(RECV_STR *pstQue, UINT8 *pBuf, UINT32 ulLong)
{
    TIMER0_VAR_DECLAR;

    if((NULL == pstQue) || (NULL == pBuf) || (0 == ulLong))
    {
        return ERROR;
    }

    TIMER0_INIT_TIMEOUT(HS_UART_RECV_TIMEOUT_MS);

    while(pstQue->ulTotalNum < ulLong)
    {
        if(hsUartRecv(pstQue) != HSUART_STATUS_OK)
        {
            return ERROR;
        }
        if(TIMER0_IS_TIMEOUT())
        {
            return HSUART_RECV_TIMEOUT;
        }
    }
    if(OutQue(pstQue, pBuf, ulLong) != OK)
    {
        return ERROR;
    }

    return OK;
}

/*****************************************************************************
* 函 数 名  : isHsSptBaud
*
* 功能描述  : 检查是否是支持的波特率
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BOOL isHsSptBaud(UINT32 u32BaudRate)
{
    if(BAUDRATE_9600 == u32BaudRate
        || BAUDRATE_115200 == u32BaudRate
        || BAUDRATE_1M == u32BaudRate
        || BAUDRATE_2M == u32BaudRate
        || BAUDRATE_3M == u32BaudRate
        || BAUDRATE_4M == u32BaudRate)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
* 函 数 名  : hsUartSetBaud
*
* 功能描述  : HS UART设置波特率接口函数
*
* 输入参数  : UINT32  u32BaudRate      需要设置的波特率
*
* 输出参数  : 无
* 返 回 值  : OK       成功
*             ERROR    失败
*
* 修改记录  :2011年7月15日   鲁婷  创建
*****************************************************************************/
HSUART_STATUS hsUartSetBaud(UINT32 u32BaudRate)
{
    UINT32 u32Divisor = 0;
    UINT32 u32DivisorLow = 0;
    UINT32 u32DivisorHigh = 0;
    UINT32 u32Discard = 0;
    UINT32 u32Times = 0;

    /* 计算分频比 */
    switch(u32BaudRate)
    {
        case BAUDRATE_115200:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_115200);
            break;

        case BAUDRATE_9600:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_9600);
            break;

        case BAUDRATE_1M:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_1M);
            break;

        case BAUDRATE_2M:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_2M);
            break;

        case BAUDRATE_3M:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_3M);
            break;

        case BAUDRATE_4M:
            u32Divisor = HSUART_CLOCK_FREQ / (16 * BAUDRATE_4M);
            break;

        default:
            print_info_with_u32("\r\nbaudrate error baud :",u32BaudRate);
            return HSUART_STATUS_BAUTRATE_ERR;
            break;  /*lint !e527*/

    }
#if 0   /* 不会走到该分支 */
    /* 判断是否支持该分频比 */
    if(u32Divisor == 0)
    {
        print_info("\r\not support baudrate\r\n");
        return HSUART_STATUS_BAUTRATE_ERR;
    }
#endif
    u32DivisorHigh = (u32Divisor&0xFF00) >> 8;
    u32DivisorLow = u32Divisor&0xFF;

    /* 读取UART 状态寄存器，看UART是否处于idle状态*/
    u32Discard = INREG32(HSUART_BASE_ADDR + UART_USR);
    while((u32Discard & 0x01)&&(u32Times<HSUUART_DELAY_TIMES))/*uart is busy*/
    {
    	u32Discard = INREG32(HSUART_BASE_ADDR + UART_USR);
        u32Times++;
        delay(100);
    }
    if(HSUUART_DELAY_TIMES == u32Times)
    {
        print_info("\r\nhsuart busy\r\n");
        return HSUART_STATUS_BAUTRATE_ERR;
    }

    /* USR bit[0]=0 即UART 处在idle 态时，才能对LCR bit[7]该位进行写操作*/
    OUTREG32(HSUART_BASE_ADDR + UART_LCR, UART_DEF_LCR_DIVENABLE);

    /* 配置DLH 和DLL 寄存器，设置分频比*/
    OUTREG32(HSUART_BASE_ADDR +  UART_DLL, u32DivisorLow);
    OUTREG32(HSUART_BASE_ADDR +  UART_DLH, u32DivisorHigh);

    /* 配置外围发送和接收数据的数目为8bit,1停止位,无校验位,disable DLL&DLH */
    OUTREG32(HSUART_BASE_ADDR +  UART_LCR, UART_DEF_PARITY_NONE|UART_DEF_LCR_STP1|UART_DEF_LCR_CS8);

    return HSUART_STATUS_OK;
}


