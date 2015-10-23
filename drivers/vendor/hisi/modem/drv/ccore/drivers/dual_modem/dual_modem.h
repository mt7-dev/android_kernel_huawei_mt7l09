

#ifndef _DUAL_MODEM_H_
#define _DUAL_MODEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vxWorks.h>
#include <taskLib.h>
#include <sioLib.h>

#include "bsp_om.h"
#include "bsp_softtimer.h"
#include "drv_uart_if.h"
#include "SysNvId.h"
#include "drv_comm.h"
#include "hi_uart.h"
#include "hi_armip_uart.h"
#include "soc_memmap_comm.h"
#include "osl_types.h"
#include "osl_sem.h"
#include "osl_thread.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"
#include "drv_wakelock.h"
#include "bsp_wakelock.h"
#include "bsp_nvim.h"
#include "bsp_icc.h"
#include "bsp_dpm.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define PBXA9_UART_CLK          	(19200000)       //高速时钟频率为60MHz
#define UART_DEFAULT_BAUDRATE  		(57600)			
#define UART_RECV_BUF_SIZE     		(1024)
#define RCVR_FIFO_LEVEL 			(0X08)
#define LPM3_UART5_IPC_INTID		199		
#define HI_CRG_CLKEN4_OFFSET 		(0x24)
#define TX_FIFO_DEPTH			    (0x10)
#define UART_FIFO_DEF_SET			(0x41)
#define UART_REGOFF_REFL			(0x84)			//接收fifo个数寄存器
#define UART_REGOFF_TXDPTH			(0x2c)			//发送fifo深度设置寄存器
#define UART_IER_TIMEOUT_ENABLE     (0x10)

#define DUAL_MODEM_TASK_PRO			53
#define DUAL_MODEM_TASK_STK			3072

#define WAKEUP_TASK_PRO				60
#define WAKEUP_TASK_STK				3072

#define UART_RECV_POLL_TASK_PRO		14
#define UART_RECV_POLL_TASK_STK		3072
#define IPC_MBX_ICLR_REG_SETTOFF	(0x4d8)
#define IPC_MBX19_INT_CLEAR			(1<<5)
//#define DualModemPoll
#define CBP_WAKEUP_STRING_SIZE		3
#define LPm3_UART5_IQR_ENABLE		(0xaa)
#define VIA_WAKEUP_BALONG			(0x01)

#define CBP_WAKEUP_DELAY_TIME_MS    (20000)
#define dm_print_err(fmt, ...)    	(bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DUAL_MODEM, "[dual_modem]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define DualModem_LowPower


#define GPIO_HIGH		            1
#define GPIO_LOW		            0
#define GPIO_WAKEUP_PIN             54
#define WAKEUP_STATE_PIN            211

/*------------------------PERI_CRG-------------------------------*/
#define PERI_CRG_PEREN2_REG			0x20
#define PERI_CRG_PERDIS2_REG		0x24
#define PERI_CRG_PERRSTDIS2_REG		0x7c
#define PERI_CRG_CLKDIV19_REG		0xf4

#define GT_CLK_UARTL_OPEN			((1<<12)|(1<<28))
#define GT_CLK_UART5_ENABLE			(1<<15)
#define GT_CLK_UART5_DISABLE		(1<<15)
#define GT_RST_UART5_ENABLE	 		(1<<15)
#define GT_CLK_UARTL_CLOSS			(~((1<<12)|(1<<28)))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum  _UART_PHY_PORT_
{
    MUART0_ID = 0,
    MUART1_ID = 1,
    MUART2_ID = 2,
    UART_PHY_BUTT
}UART_PHY_PORT;

enum UART_SWITCH_ENUM
{
    UART_SWITCH_DISABLE = 0,
    UART_SWITCH_ENABLE  = 1,
    UART_SWITCH_BUTT
};

enum WAKEUP_ENUM
{
	DO_WAKEUP_3RD =0,
	NOT_WAKEUP_V3,	
};


typedef struct _UART_HW_DESC_
{
    unsigned int base_addr;
    unsigned int irq_num;
    unsigned int clk;
    unsigned int baudrate;
}UART_HW_DESC;


typedef struct hsuart_recv_str
{
    u32 ulRead;
    u32 ulWrite;
    u32 ulTotalNum;
	u32 recv_task_id;
    u32 uart_recv_poll_id;
	u32 max_size;
	osl_sem_id    recv_mutex;
    BSP_U8 ucData[UART_RECV_BUF_SIZE];
}DUAL_MODEM_RECV_STR;

typedef struct _S_UART_PORT_
{
    UART_PHY_PORT   port_id;
    UART_HW_DESC*   hw_desc;
    pUARTRecv       recv_callback;
    osl_sem_id     	send_mutex;
    struct  _UART_HW_OPS_* ops;
    unsigned int    irq_cnt;
    unsigned int    irq_SemGive_cnt;
    unsigned int    rtask_SemTake_cnt;  //信号量获取计数	
    unsigned int 	cbpa_send_count;
    unsigned int 	tx_total_size;
	unsigned int 	tx_cur_size;
	unsigned int 	rx_total_size;
	unsigned int 	suspend_cnt;
	unsigned int 	resume_cnt;
	unsigned int 	callback_fail_cnt;
	unsigned int 	send_mutex_cnt;
	unsigned int    recv_register_cnt;
}UART_PORT;


typedef struct  _UART_HW_OPS_
{
    int(*send)(UART_PORT *uart_port, unsigned char *pbuf ,unsigned int size);
}UART_HW_OPS;



typedef struct
{
    UART_CONSUMER_ID   consumer_id;
    UART_PHY_PORT      phy_id;
}UART_MAP;



struct dual_modem_control
{
	struct softtimer_list hold_wake_timer;   //240ms内双方modem通信不用唤醒
	struct softtimer_list sleep_timer;       //300ms内v7r2 modem不投睡眠票
	struct wake_lock wakelock;				 //睡眠投票
	u32 send_wakeup_count;					 //唤醒重发次数
	u32 wakeup_3rdmodem_flag;				 //发送前是否需要唤醒对方标志
	u32 active_wakeup_v3modem_flag;			 //主动发起唤醒
	u32 via_wakeup_balong_cnt;
	u32 modem_sleeptimer_cnt;
	u32 wakeup_task_id;
	u8 via_wakeup_balong_flag;
	osl_sem_id wait_reply_mutex;			 //等待对方应答信号量
	osl_sem_id wakeup_gpio_mutex;			
	UART_PORT uart_port[UART_PHY_BUTT];
	DUAL_MODEM_RECV_STR HSUART_RECV;
	UART_HW_OPS muart2_ops;
};

struct dual_modem_info
{
	UART_HW_DESC uart_port_hw_desc;
	UART_MAP uart_map[3];
};

extern void *memset(void *pdst, int c, unsigned int plen);
void t_resq_via(void);
void dual_modem_uart_channel_init(UART_HW_DESC* uart_hw_desc);
s32 dual_modem_uart_port_init(UART_PORT *uart_port);
int bsp_dual_modem_init(void);
int uart_core_recv_handler_register(UART_CONSUMER_ID uPortNo, pUARTRecv pCallback);
int dual_modem_send_bytes(UART_PORT* uart_port,BSP_U8* pbuf,BSP_U32 size);
int uart_core_send(UART_CONSUMER_ID uPortNo, unsigned char * pDataBuffer, unsigned int uslength);
void modem_sleeptimer_overhandler(u32 temp);
void dual_modem_uart_irq_handler(u32 uart_port_add);
void modem_sleeptimer_overhandler(u32 temp);
void modem_awaketimer_overhandler(u32 temp);
int dual_modem_wakeup_init(DRV_DUAL_MODEM_STR DualModemNv);
void dual_modem_uart_recv_task(UART_PORT * uart_port_addr);
int DualModem_InQue(DUAL_MODEM_RECV_STR *pstQue, UINT8 ucData);
s32 wakeup_via_modem(void);
s32 dual_uart_suspend(struct dpm_device *dev);
s32 dual_uart_resume(struct dpm_device *dev);
void t_read_reg(void);
void t_send_via(void);
void t_send_to_via(void);

void ut_send(void);
void t_send(void);
int t_recv_handler(UART_CONSUMER_ID uPortNo,unsigned char *pData, unsigned int ulLength);
void t_recv_reg(void);
void clear_recv_num(void);
void t_write_reg(void);


#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif /* end of _DUAL_MODEM_H_ */
