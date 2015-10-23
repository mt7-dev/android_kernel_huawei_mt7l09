

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "med_drv_interface.h"

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define DRV_UART_RBR_THR_DLL            (DRV_UART_BASE_ADDR + 0x000)            /* UART接收发送数据和波特率低8位 */
#define DRV_UART_DLH_IER                (DRV_UART_BASE_ADDR + 0x004)            /* UART中断使能和波特率高8位     */
#define DRV_UART_IIR_FCR                (DRV_UART_BASE_ADDR + 0x008)            /* UART中断ID和fifo控制寄存器    */
#define DRV_UART_LCR                    (DRV_UART_BASE_ADDR + 0x00C)            /* UART线性控制寄存器            */
#define DRV_UART_MCR                    (DRV_UART_BASE_ADDR + 0x010)            /* UART Modem控制寄存器          */
#define DRV_UART_LSR                    (DRV_UART_BASE_ADDR + 0x014)            /* UART线性状态寄存器            */
#define DRV_UART_MSR                    (DRV_UART_BASE_ADDR + 0x018)            /* Modem状态寄存器               */
#define DRV_UART_SCR                    (DRV_UART_BASE_ADDR + 0x01C)            /* 中间结果暂存寄存器            */
#define DRV_UART_FAR                    (DRV_UART_BASE_ADDR + 0x070)            /* FIFO访问控制寄存器            */
#define DRV_UART_TFR                    (DRV_UART_BASE_ADDR + 0x074)            /* 发送FIFO读寄存器              */
#define DRV_UART_RFW                    (DRV_UART_BASE_ADDR + 0x078)            /* 接收FIFO写寄存器              */
#define DRV_UART_USR                    (DRV_UART_BASE_ADDR + 0x07C)            /* UART状态寄存器                */
#define DRV_UART_TFL                    (DRV_UART_BASE_ADDR + 0x080)            /* 发送FIFO数据个数寄存器        */
#define DRV_UART_RFL                    (DRV_UART_BASE_ADDR + 0x084)            /* 接收FIFO数据个数寄存器        */
#define DRV_UART_HTX                    (DRV_UART_BASE_ADDR + 0x0A4)            /* 暂停发送寄存器                */
#define DRV_UART_DMASA                  (DRV_UART_BASE_ADDR + 0x0A8)            /* DMA软件响应寄存器             */
#define DRV_UART_CPR                    (DRV_UART_BASE_ADDR + 0x0F4)            /* 配置参数寄存器                */
#define DRV_UART_UCV                    (DRV_UART_BASE_ADDR + 0x0F8)            /* UART版本寄存器                */
#define DRV_UART_CTR                    (DRV_UART_BASE_ADDR + 0x0FC)            /* 外设ID寄存器                  */

#define DRV_UART_WAIT_CNT               (0x100)                                 /* UART冲突时等待次数 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID DRV_UART_Init(VOS_VOID);
extern VOS_VOID DRV_UART_SendData(VOS_UCHAR *pucData, VOS_UINT32 uwDataLen);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_drv_uart.h */
