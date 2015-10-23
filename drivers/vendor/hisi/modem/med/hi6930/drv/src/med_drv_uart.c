

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_uart.h"
#include "ucom_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID DRV_UART_Init(VOS_VOID)
{
    /*该寄存器只写，使能FIFO,设置Rx水线为1/4 FIFO，Tx水线为1/4 FIFO,清除FIFO*/
    UCOM_RegWr(DRV_UART_IIR_FCR, 0x67);

    /*配置数据长度为8bit, bit0/bit1, 2b'11*/
    UCOM_RegBitWr(DRV_UART_LCR, UCOM_BIT0, UCOM_BIT1, 0x3);

    /*配置STOP长度为1, bit2, 1b'0*/
    UCOM_RegBitWr(DRV_UART_LCR, UCOM_BIT2, UCOM_BIT2, 0x1);

    /*配置校验功能关闭, bit3/bit4,1b'0*/
    UCOM_RegBitWr(DRV_UART_LCR, UCOM_BIT3, UCOM_BIT4, 0x0);

    /*打开分频配置寄存器DLL DLH的使能(完成DLL DLH的配置后,此比特需清0), bit7,1b'1*/
    UCOM_RegBitWr(DRV_UART_LCR, UCOM_BIT7, UCOM_BIT7, 0x1);

    UCOM_RegBitWr(DRV_UART_RBR_THR_DLL, UCOM_BIT0, UCOM_BIT7, 0xA);

    UCOM_RegBitWr(DRV_UART_DLH_IER, UCOM_BIT0, UCOM_BIT7, 0x0);

    /*完成DLL DLH的配置后,清0 LCR bit7,使能寄存器读写*/
    UCOM_RegBitWr(DRV_UART_LCR, UCOM_BIT7, UCOM_BIT7, 0x0);

    /*禁止中断*/
    UCOM_RegBitWr(DRV_UART_DLH_IER, UCOM_BIT0, UCOM_BIT7, 0x0);
}
VOS_VOID DRV_UART_SendData( VOS_UCHAR *pucData, VOS_UINT32 uwDataLen )
{
    VOS_UINT32                          i;
    VOS_UINT32                          uwThreValue;
    VOS_UINT32                          uwFifoNotFullFlag;
    VOS_UINT32                          uwCnt;

    if (VOS_NULL_PTR == pucData)
    {
        return;
    }

    /*逐byte依次写入发送数据寄存器*/
    for (i = 0; i < uwDataLen; i++)
    {
        uwCnt = DRV_UART_WAIT_CNT;

        do
        {
            uwCnt--;

            /*读取THRE标志，0表示非空，1表示空*/
            uwThreValue         = UCOM_RegBitRd(DRV_UART_LSR, UCOM_BIT5, UCOM_BIT5);

            /*读取TFNF标志，0表示FIFO满，1表示FIFO非满*/
            uwFifoNotFullFlag   = UCOM_RegBitRd(DRV_UART_USR, UCOM_BIT1, UCOM_BIT1);

        /*若THR非空且FIFO满,则无法发送新数据,轮循等待*/
        }while ((0 == uwThreValue) && (0 == uwFifoNotFullFlag) && (uwCnt > 0));

        /*写入一个byte至发送数据寄存器*/
        UCOM_RegWr(DRV_UART_RBR_THR_DLL, (VOS_UINT32)(*(pucData + i)));
    }
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

