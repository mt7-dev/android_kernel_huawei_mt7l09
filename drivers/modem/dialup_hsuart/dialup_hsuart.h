
#ifndef    _HSUART_UDI_H_
#define    _HSUART_UDI_H_

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************
  头文件包含                            
**************************************************************************/
#include <linux/semaphore.h>
#include <mach/gpio.h>
#include <osl_sem.h>
#include "bsp_softtimer.h"
#include "drv_dpm.h"
#include "bsp_om.h"
#include "drv_usb.h"
#include "drv_edma.h"
#include "drv_hsuart.h"
#include "bsp_softtimer.h"


#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif


#ifdef _DRV_LLT_
#define memset
#endif

#define HSUART_TRUE 	1
#define HSUART_FALSE 	0
#define hsuart_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HSUART, "[HSUART]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define SWITCH_TIMER_LENGTH 	1000
#define ADAPT_TIMER_LENGTH 		15000
#define RX_TIMER_LENGTH 		10

/* buffer define */
#define UART_READ_BUF_SIZE 		1536
#define UART_READ_BUF_NUM 		16
#define UART_WRITE_BUF_NUM 		64

#define SIZE_4K 				(4096)

#define HSUART_DCD    			GPIO_2_4
#define HSUART_DSR        		GPIO_2_5
#define HSUART_DTR        		GPIO_2_16
#define HSUART_RING        		GPIO_2_26

#define HSUART_RX_TASK_PRI		81
#define HSUART_TX_TASK_PRI		81
#define HSUART_RX_TASK_SIZE		0x1000
#define HSUART_TX_TASK_SIZE		0x1000

/* DMA Mode */
#define UART_DEF_NO_DMA_MODE  	0x00   /* Mode 0 */
#define UART_DEF_DMA_MODE     	0x08   /* Mode 1 */
#define UART_LCR_DLAB			0x80
#define UART_TX_FIFO_RESET		0x04
#define UART_RX_FIFO_RESET		0x02
/* 设置接收FIFO水线 */
#define UART_DEF_RT_1CHAR     0x00 /* FIFO 中有1 个字符*/
#define UART_DEF_RT_2CHAR     0xc0 /* FIFO 差2 个字符就满*/
#define UART_DEF_RT_ONEFOUR   0x40 /* FIFO 四分之一满*/
#define UART_DEF_RT_ONETWO    0x80 /* FIFO 二分之一满*/

typedef u32    	HSUART_TASK_ID;
typedef int* 	HTIMER;
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* +++检测阶段*/
typedef enum
{
    NO_DETECT_STAGE,
    DETECT_STAGE_1,
    DETECT_STAGE_2,
    DETECT_STAGE_3,
    DETECT_STAGE_MAX,
}detect_stage;


/* UART TIMER 结构 */
typedef struct tagUART_TIMER
{
    HTIMER id;
    BSP_BOOL flag;
}uart_timer;

typedef enum tagUART_TRANSFER_MODE
{
    CPU_TRANSFER,
    DMA_TRANSFER,
    MODE_MAX,
}uart_transfer_mode;

typedef enum tagUART_MEM_STATE
{
    UART_MEM_NOT_IN_LIST = 0,
    UART_MEM_IN_FREELIST = 1,
    UART_MEM_IN_DONELIST = 2
}uart_mem_state;

/*****************************************************************************
  3 结构体定义
*****************************************************************************/
typedef uint32_t  phys_addr;

/* Transfer block descriptor */
typedef struct mem_desc_s {
    void           *vaddr;
    phys_addr      paddr;
} mem_desc_t;

typedef mem_desc_t *mem_desc_h;

/* uart stat info */
typedef struct tagUART_STAT {
    BSP_U32 write_not_free_cnt;
    BSP_U32 lost_data_cnt;
    BSP_U32 syn_write_cnt;
    BSP_U32 burst_dma_done;
    BSP_U32 single_dma_done;
    BSP_U32 single_time_out;
    BSP_U32 alloc_skb_fail;
    BSP_U32 sleep_delay_cnt;
} hsuart_stat;

typedef struct tagHSUART_DEV_S
{
    int irq;
    unsigned int clock;
    struct spinlock lock_irq;
    unsigned int phy_addr_base;
    volatile void __iomem * vir_addr_base;
    BALONG_DMA_REQ dma_req_rx;
    BALONG_DMA_REQ dma_req_tx;
}hsuart_dev;

typedef int (*hsuart_set_baud_cb)(hsuart_dev *uart_dev, int baud);
typedef int (*hsuart_set_wlen_cb)(hsuart_dev *uart_dev, int w_len);
typedef int (*hsuart_set_stp2_cb)(hsuart_dev *uart_dev, int stp2);
typedef int (*hsuart_set_eps_cb)(hsuart_dev *uart_dev, int eps);

typedef struct tagUART_UDI_CALLBACKS_S
{
    /* General Handlers */
    hsuart_free_cb_t free_cb;
    hsuart_read_cb_t read_cb;
    hsuart_msc_read_cb_t msc_read_cb;
    hsuart_switch_mode_cb_t switch_mode_cb;
    
    hsuart_set_baud_cb set_baud_cb;
    hsuart_set_wlen_cb set_wlen_cb;
    hsuart_set_stp2_cb set_stp2_cb;
    hsuart_set_eps_cb set_eps_cb;
}hsuart_udi_cbs;

typedef struct tagUART_WR_ASYNC_INFO
{
    char* pBuffer;
    unsigned int u32Size;
    void* pDrvPriv;
}hsuart_wr_async_info;

typedef struct tagUART_FEATURE
{
    int baud;
    int wlen;
    int stp2;
    int eps;
    volatile BSP_BOOL rtsen;
    volatile BSP_BOOL ctsen;
}hsuart_feature;

/* UART 内存信息 */
typedef struct tagUART_MEM_NODE
{
    struct list_head list_node;
    uart_mem_state state;
    mem_desc_t mem;
    BSP_S32 valid_size;

    struct sk_buff *pSkb;
}uart_mem_node;

/* UART数据队列结构体*/
typedef struct tagUART_TRANS_INFO_S
{
    struct spinlock list_lock;
    struct list_head free_list;
    struct list_head done_list;
    uart_mem_node *pmem_start;                   /* MemNode数组首地址 */
    uart_mem_node *pmem_end;
    uart_mem_node *pcur_pos;                   /* 当前使用节点*/
    BSP_U32  node_size;
    BSP_U32  node_num;
    volatile BSP_U32  done_cnt;

    /* statistics section */
    BSP_U32 total_bytes;                      /* 总计数 */
}hsuart_trans_info;

/* dma 信息结构体*/
typedef struct tagUART_DMA_INFO_S
{
    volatile BSP_BOOL rx_dma_done;
    volatile BSP_U32  burst_delay;
    volatile BSP_U32  single_delay;
    volatile BSP_U32  single_loop;
}hsuart_dma_info;

typedef struct tagHSUART_CTX_S
{  
    hsuart_dev dev;
    UDI_DEVICE_ID udi_device_id;
    osl_sem_id rx_sema;
    osl_sem_id tx_sema;
    osl_sem_id dma_sema;
    osl_sem_id sleep_sema;
    BSP_BOOL init;
    BSP_BOOL open;
    BSP_BOOL baud_adapt;
    BSP_U32 baud_idx;
    volatile detect_stage stage;
    uart_timer switch_timer;
    uart_timer adapt_timer;
    uart_timer rx_timer;
    uart_timer sleep_timer;
    volatile BSP_BOOL rts;
    volatile BSP_BOOL tx_running;
    volatile BSP_BOOL sleep_out;
    uart_transfer_mode transfer_mode;
    hsuart_trans_info read_info;                     /* 读信息*/
    hsuart_trans_info write_info;                    /* 写信息*/
    hsuart_feature feature;
    hsuart_dma_info dma_info;
    hsuart_udi_cbs cbs;
    hsuart_stat stat;
	
}hsuart_ctx;

struct hsuart_ctrl
{
	struct softtimer_list switch_state1_timer;
	struct softtimer_list switch_state2_timer;
	struct softtimer_list switch_state3_timer;
    struct softtimer_list adapt_timer;
    struct softtimer_list rx_timer;
    struct softtimer_list sleep_timer; 
	HSUART_TASK_ID rx_task_id;
	HSUART_TASK_ID tx_task_id;
};

void hsuart_sleep_cb(u32 arg);
void switch_stage_one(u32 arg);
void switch_stage_two(u32 arg);
void switch_stage_three(u32 arg);
void switch_stage_one(u32 arg);
//extern int BSP_PWRCTRL_SleepInUartCB(PWC_DS_SOCP_CB_STRU stFunc);
extern int BSP_UDI_SetPrivate(UDI_DEVICE_ID devId, VOID* pPrivate);
extern int BSP_UDI_SetCapability(UDI_DEVICE_ID devId, BSP_U32 u32Capability);
extern int BSP_UDI_SetInterfaceTable(UDI_DEVICE_ID devId, UDI_DRV_INTEFACE_TABLE *pDrvInterface);
extern unsigned int TTF_VIRT_TO_PHY(unsigned int ulVAddr);
#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */
#endif

