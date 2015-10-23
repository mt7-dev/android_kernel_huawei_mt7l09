/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  uartdrv.H
*
*   作    者 :  wangdi
*
*   描    述 :  本文件命名为"uartdrv.H"
*
*   修改记录 :  2011年9月19日  v1.00  wangdi创建
*************************************************************************/

#ifndef    _UART_DRV_H_
#define    _UART_DRV_H_


#include <osl_types.h>
#include <bsp_om.h>
#include <drv_comm.h>
#include <drv_acm.h>
#include <hi_uart.h>


#define DW_UART_TFL          0x80
#define DW_UART_RFL          0x84

/*
*
* DW_UART_LCR bit field
*
*/
#define FIFO_TX_SIZE   (31)
#define FIFO_MAX_SIZE  (32)
#define SIZE_4K (4096)

#define DLL_BUAD_PARA 0x1A
#define DLH_BUAD_PARA 0x0

#define DW_FCR_PARA 0xb1

#define ABLE_RX_IER (ERBFI|ELSI)

#define DW_UART_DLAB       (1 << 7)
#define DW_UART_BREAK      (1 << 6)
#define DW_UART_EPS        (1 << 4)
#define DW_UART_PEN        (1 << 3)
#define DW_UART_STOP       (1 << 2)
#define DW_UART_8bit       0x3
#define DW_UART_7bit       0x2
#define DW_UART_6bit       0x1
#define DW_UART_5bit       0x0
#define DW_UART_DATALEN_MASK 0x03

/*
*
* DW_UART_IER bit field
*
*/
#define PTIME       (1<<7)
#define EDSSI       (1<<3)
#define ELSI        (1<<2)
#define ETBEI       (1<<1)
#define ERBFI       (1)

/*
*
* DW_UART_LSR bit field
*
*/
#define DW_RFE        (1<<7)
#define DW_TEMT       (1<<6)
#define DW_THRE       (1<<5)
#define DW_BI         (1<<4)
#define DW_FE         (1<<3)
#define DW_PE         (1<<2)
#define DW_OE         (1<<1)
#define DW_DR         (1)

#define DW_RSR_ANY (DW_OE|DW_PE|DW_FE|DW_BI)
#define DW_DUMMY_RSR_RX
/*
*
* DW_UART_MCR bit field
*
*/
#define DW_MC_AFCE      (1<<5)
#define DW_MC_LOOP      (1<<4)
#define DW_MC_OUT2      (1<<3)
#define DW_MC_OUT1      (1<<2)
#define DW_MC_RTS       (1<<1)
#define DW_MC_DTR       (1 )

/*
*
* DW_UART_MSR bit field
*
*/
#define DW_DCD          (1<<7)
#define DW_RI           (1<<6)
#define DW_DSR          (1<<5)
#define DW_CTS          (1<<4)

#define DW_MSR_ANY (DW_DCD|DW_DSR|DW_CTS)

/*
*
* DW_UART_IIR bit field
*
*/
#define DW_RECEIVERR        (0x06)    /* HIGHEST   */
#define DW_RECEIVEAVA       (0x04)    /* SECOND  RECEIVE date */
#define DW_RECTIMEOUT       (0x0C)    /* SECOND    */
#define DW_TRANSEMP         (0x02)    /* THIRD transmit hold reg empty */
#define DW_NOINTERRUPT      (0x01)    /* NO interrupt pending */
#define DW_MODEMSTA         (0)       /* FOURTH modem int    */
#define DW_BUSY             (0x7)     /* Fifth write while line control busy*/

/*
*
* DW_UART_FCR bit field
*
*/
#define RECFIFO1_2          (0x02<<6)    /* */
#define TXFIFO1_2           (0x03<<4)    /*  */
#define FIFOENA             (1)

/*
*
* DW_UART_USR bit field
*
*/
#define DW_UART_BUSY        0x01
#define DW_XFIFO_NOT_FULL   0x02
#define DW_XFIFO_EMP        0x04
#define DW_RFIFO_NOT_EMP    0x08
#define DW_RFIFO_FULL       0x10

#define RX_NUM 3096
#define RX_SIZE 16

#define UDI_GET_MAIN_DEV_ID(id)	(((id) & 0xff00) >> 8 )
#define UDI_GET_DEV_TYPE(id)	(((id) & 0x00ff))

/* 驱动适配函数指针表 */
typedef BSP_S32 (*UDI_ADP_INIT_CB_T) (VOID);
typedef BSP_U32 (*UDI_GET_CAPABILITY_CB_T)(UDI_DEVICE_ID devId);
typedef BSP_S32 (*UDI_OPEN_CB_T)(UDI_OPEN_PARAM *param, UDI_HANDLE handle);
typedef BSP_S32 (*UDI_CLOSE_CB_T)(VOID* pPrivate);
typedef BSP_S32 (*UDI_WRITE_CB_T)(VOID* pPrivate, void* pMemObj, BSP_U32 u32Size);
typedef BSP_S32 (*UDI_READ_CB_T)(VOID* pPrivate, void* pMemObj, BSP_U32 u32Size);
typedef BSP_S32 (*UDI_IOCTL_CB_T) (VOID* pPrivate, BSP_U32 u32Cmd, VOID* pParam);

typedef struct tagUART_RX_BUF_S
{
    BSP_U8 * buf;
    BSP_U32 ulSize;
    struct tagUART_RX_BUF_S * pstNext;
}UART_RX_BUF_S;

typedef struct tagUDI_DRV_INTEFACE_TABLE
{
	/* capability */
	UDI_ADP_INIT_CB_T		udi_get_capability_cb;

	/* data interfaces */
	UDI_OPEN_CB_T			udi_open_cb;
	UDI_CLOSE_CB_T			udi_close_cb;
	UDI_WRITE_CB_T			udi_write_cb;
	UDI_READ_CB_T			udi_read_cb;
	UDI_IOCTL_CB_T			udi_ioctl_cb;
}UDI_DRV_INTEFACE_TABLE;

typedef struct tagUART_PORT_S
{
	int irq;
	struct spinlock lock_irq;
	struct semaphore uart_rx_sem;
	unsigned int phy_addr_base;
	unsigned char __iomem * vir_addr_base;
}UART_PORT_S;

#define UART_READ_BUFF_INFO ACM_READ_BUFF_INFO
typedef BSP_VOID (*UART_WRITE_DONE_CB_T)(char* pDoneBuff, int s32DoneSize);
typedef BSP_VOID (*UART_READ_DONE_CB_T)(BSP_VOID);

typedef struct tagUART_CTX_S
{
    UART_PORT_S* port;

    u8  bOpen;
    u8  bReverse;
    u16 ulRxBufNum;
    struct semaphore  MSP_Tx_Sem;

    u8 * pDataBuf;/*为了释放方便*/
    UART_RX_BUF_S *pHead_RxQueue;
    UART_RX_BUF_S *pstCurrRead;
    UART_RX_BUF_S *pstCurrWrite;

    UART_READ_DONE_CB_T  MSP_ReadCb;
    UART_WRITE_DONE_CB_T MSP_WriteCb;

}UART_CTX_S;

struct uart_at_mng
{
	u32 printkflag;
	u32 buf_look;
	UART_PORT_S uart_port0;
	UART_CTX_S  UartCtx;
	struct task_struct * pstUartReadThread;
};

#define UART_WR_ASYNC_INFO ACM_WR_ASYNC_INFO

#define UART_READ_BUFF_INFO ACM_READ_BUFF_INFO
/*外部函数extern区*/
extern BSP_S32 BSP_UDI_SetPrivate(UDI_DEVICE_ID devId, void* pPrivate);
extern BSP_S32 BSP_UDI_SetCapability(UDI_DEVICE_ID devId, BSP_U32 u32Capability);
extern BSP_S32 BSP_UDI_SetInterfaceTable(UDI_DEVICE_ID devId, UDI_DRV_INTEFACE_TABLE *pDrvInterface);
extern unsigned int bsp_get_amba_ports(void);


s32 get_uart_port(u32 uart_dev_id);
s32 uart_send(s32 s32uart_dev_id, u8 *pbuf, u32 size);
s32 uart_at_get_read_buffer(s32 uart_dev_id, ACM_WR_ASYNC_INFO* p_wr_info);
s32 uart_at_return_read_buffer(s32 uart_dev_id, ACM_WR_ASYNC_INFO* p_wr_info);
s32 uart_at_reg_init(void);
s32 uart_rx_buf_init(void);
void uart_rx_buf_uinit(void);
void uart_at_reg_uninit(void);
static int uart_recv_thread(void *data);

#endif
