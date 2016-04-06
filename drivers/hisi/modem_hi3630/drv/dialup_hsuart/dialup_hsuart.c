
#include <linux/kernel.h>       /*kmalloc,printk*/
#include <linux/kthread.h>      
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/irq.h>
//#include <linux/amba/serial.h>
#include "osl_cache.h"
#include "osl_malloc.h"
#include "osl_thread.h"
#include "soc_baseaddr_interface.h"
#include "soc_interrupts_comm.h"
#include <bsp_edma.h>
#include "drv_udi.h"
#include "drv_acm.h"
#include "drv_udi.h"
#include "hi_uart.h"
#include "dialup_hsuart.h"
#include "dialup_hsuart_test.h"

m2m_recv_str hsuart_recv_str;

hsuart_ctx  m2m_hsuart_ctx = {
    .dev = {
        .irq = INT_LVL_HSUART,
        .clock = 64000000,
        .phy_addr_base = HI_HSUART_REGBASE_ADDR,
        .dma_req_rx = EDMA_CH_HIFI_SIO_TX,
        .dma_req_tx = EDMA_CH_HIFI_SIO_RX,
    },
    .udi_device_id = UDI_HSUART_0_ID,
    .transfer_mode = DMA_TRANSFER,
};

static int typical_baud[] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 2900000, 3200000, 3686400, 4000000};
static const char switch_cmd[] = "+++";

struct hsuart_ctrl g_hsuart_ctrl ={{0},{0},{0},{0},{0},{0},0};


#if 0
STATIC INLINE void hsuart_hex_dump(bsp_log_level_e level, unsigned char *buf, unsigned int len)
{
    int idx;
    
    if (level < g_dbg_lvl)
    {
        return;
    }

    printk("hsuart_hex_dump addr: %p, len = %d\n", buf, len);
    for (idx = 0; idx < len; idx++) {
        if (idx % 16 == 0)
            printk("0x%04x : ", idx);
        printk("%02x ", ((unsigned char)buf[idx]));
        if (idx % 16 == 15)
            printk("\n");
    }
    printk("\n");
}
#endif

/*****************************************************************************
* 函 数 名  : hsuart_get_dma_info
*
* 功能描述  : uart 获取DMA信息
*
* 输入参数  : uart_ctx:uart上下文
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_get_dma_info(hsuart_ctx *uart_ctx)
{

	hsuart_error("hsuart_get_dma_info enter...!\n");

    if (115200 <= uart_ctx->feature.baud)
    {
        uart_ctx->dma_info.burst_delay = 5;
        uart_ctx->dma_info.single_delay = 2;
        uart_ctx->dma_info.single_loop = 1;
    }
    else if (9600 <= uart_ctx->feature.baud)
    {
        uart_ctx->dma_info.burst_delay = 20;
        uart_ctx->dma_info.single_delay = 10;
        uart_ctx->dma_info.single_loop = 1;
    }
    else
    {
        uart_ctx->dma_info.burst_delay = 20;
        uart_ctx->dma_info.single_delay = 10;
        uart_ctx->dma_info.single_loop = 1;
    }
	
    hsuart_error("burst_delay:%d   single_delay:%d  single_loop:%d!\n",\
		uart_ctx->dma_info.burst_delay, uart_ctx->dma_info.single_delay, uart_ctx->dma_info.single_loop);
}
/*******************************************************************************
* 函 数 名  : hsuart_enable
*
* 功能描述  : 使能UART
*
* 输入参数  : uart_dev 设备指针

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
static inline void hsuart_enable(hsuart_dev *uart_dev)
{
    u32 val = 0;
    
    /*enable uart*/
    val = readl(uart_dev->vir_addr_base + UART_REGOFF_FCR);
    val |= UART_FCR_FIFO_ENABLE;
    (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_FCR);
}

/*******************************************************************************
* 函 数 名  : hsuart_disable
*
* 功能描述  : 禁能UART
*
* 输入参数  : uart_dev 设备指针

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
static inline void hsuart_disable(hsuart_dev *uart_dev)
{
    u32 val = 0;
    
    val = readl(uart_dev->vir_addr_base + UART_REGOFF_FCR);
    val &= ~(UART_FCR_FIFO_ENABLE );
    (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_FCR);
}

/*******************************************************************************
* 函 数 名  : hsuart_set_rts
*
* 功能描述  : 设置流控
*
* 输入参数  : uart_dev 设备指针

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
#if 0
static inline void hsuart_set_rts(hsuart_dev *uart_dev)
{
    u32 val = 0;
    hsuart_ctx *uart_ctx = NULL;
    
    uart_ctx = container_of(uart_dev, hsuart_ctx, dev);

    if (CPU_TRANSFER == uart_ctx->transfer_mode)
    {
        /*mask rx interrupts to enable rts port*/
        val = readl(uart_dev->vir_addr_base + UART011_IMSC);
        val &= (~(UART011_RXIM | UART011_RTIM));
        (void)writel(val, uart_dev->vir_addr_base + UART011_IMSC); 
    }
}
#endif
/*******************************************************************************
* 函 数 名  : hsuart_disable
*
* 功能描述  : 禁能UART
*
* 输入参数  : uart_dev 设备指针

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
#if 0
static inline void hsuart_clear_rts(hsuart_dev *uart_dev)
{
    u32 val = 0;
    hsuart_ctx *uart_ctx = NULL;
    
    uart_ctx = container_of(uart_dev, hsuart_ctx, dev);

    if (CPU_TRANSFER == uart_ctx->transfer_mode)
    {
        /*unmask rx interrupts to disable rts port*/
        val = readl(uart_dev->vir_addr_base + UART011_IMSC);
        val |= (UART011_RXIM | UART011_RTIM);
        (void)writel(val, uart_dev->vir_addr_base + UART011_IMSC);
    }
}
#endif

/*****************************************************************************
* 函 数 名  : hsuart_set_baud
*
* 功能描述  : UART 设置波特率函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             baud: 波特率值
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_set_baud(hsuart_dev *uart_dev, int baud)
{
	u32 divisor = 0;
	u32 divisor_dll = 0;
    u32 divisor_dlh = 0;
//   	u32 val = 0;
	
    hsuart_ctx *uart_ctx = NULL;
    hsuart_error("hsuart_set_baud enter...!\n");

    if (NULL == uart_dev || baud < 0 || (unsigned int)baud > (uart_dev->clock >> 4))
    {
        hsuart_error("uart_dev:0x%x   baud:%d\n", (unsigned int)uart_dev, baud);
        return ERROR;
    }
    
    uart_ctx = container_of(uart_dev, hsuart_ctx, dev);
    if (0 == baud)
    {
        uart_ctx->baud_adapt = HSUART_TRUE;
        uart_ctx->baud_idx = (uart_ctx->baud_idx + 1) % (sizeof(typical_baud) / sizeof(int));
        baud = typical_baud[uart_ctx->baud_idx];
        hsuart_error("baud adapt start!   idx:%d baud:%d\n", uart_ctx->baud_idx, baud);
    }
    else
    {
        uart_ctx->baud_idx = 0;
    }
    uart_ctx->feature.baud = baud;
	
    hsuart_error("set baud: %d\n", baud);
    
	divisor =  uart_dev->clock/ (u32)(16 * baud);
	divisor_dll = divisor & 0xFF;
	divisor_dlh = (divisor & 0xFF00) >> 8;
	
	/* 打开 DLL/DHL */
    writel(UART_LCR_DLAB, uart_dev->vir_addr_base + UART_REGOFF_LCR);

    /* 配置DLH 和DLL 寄存器，设置串行通讯的波特率*/
    writel(divisor_dll, uart_dev->vir_addr_base + UART_REGOFF_DLL);
    writel(divisor_dlh, uart_dev->vir_addr_base + UART_REGOFF_DLH);
 
    /*禁止 DLL&DLH */
//	val &= ~UART_LCR_DLAB;
//    writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
 
    hsuart_get_dma_info(uart_ctx);

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_set_wlen
*
* 功能描述  : UART 设置数据位长函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             w_len: 数据位长
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_set_wlen(hsuart_dev *uart_dev, int w_len)
{

	unsigned int val = 0;
    unsigned int wlen = 0;
    
	if(NULL == uart_dev)
	{
       hsuart_error("uart_dev is NULL\n");
	   return ERROR;
	}
    hsuart_error("set wlen: %d\n", w_len);
    
    switch(w_len)
    {
        /*set wlen as 5 bit*/
        case WLEN_5_BITS:
            wlen = UART_LCR_DLS_5BITS;
            break;
        /*set wlen as 6 bit*/
        case WLEN_6_BITS:
            wlen = UART_LCR_DLS_6BITS;
            break;
        /*set wlen as 7 bit*/
        case WLEN_7_BITS:
            wlen = UART_LCR_DLS_7BITS;
            break;
        /*set wlen as 8 bit*/
        case WLEN_8_BITS:
            wlen = UART_LCR_DLS_8BITS;
            break;
        default:
            hsuart_error("unsupport w_len:%d\n", w_len);
            return ERROR;
    }
 
    /*clear wlen bits*/
    val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
    val &= (~UART_LCR_DLS_MASK);
    
    /*set new wlen*/
    val |= (unsigned int)wlen;
    (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_set_stp2
*
* 功能描述  : UART 设置停止位长函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             stp2: 两个停止位开关
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_set_stp2(hsuart_dev *uart_dev, int stp2)
{
	int ret = OK;

    unsigned int val = 0;  
    
	if(NULL == uart_dev)
	{
        hsuart_error("uart_dev is NULL\n");
		return ERROR;	
	}
    hsuart_error("set stp2: %d\n", stp2);
   
    switch(stp2)
    {
        case STP2_OFF:
            val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
            val &= ~UART_LCR_STOP_2BITS;
            (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
            break;
        case STP2_ON:
            val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
            val |= UART_LCR_STOP_2BITS;
            (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
            break;
        default:
            hsuart_error("unsupport stp2:%d\n", stp2);
            ret = ERROR;
			break;
    }
    return ret;
}

/*****************************************************************************
* 函 数 名  : hsuart_set_eps
*
* 功能描述  : UART 设置校验方式函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             eps: 校验方式
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_set_eps(hsuart_dev *uart_dev, int eps)
{
    int ret = OK;
    unsigned int val = 0;

     if(NULL == uart_dev)
	{
        hsuart_error("uart_dev is NULL\n");
		return ERROR;	
	}    
	 hsuart_error("set eps: %d\n", eps);

    switch(eps)
    {
		/*无校验位*/
		case PARITY_NO_CHECK:
            val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
            val &= ~UART_LCR_PEN;
            (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
            break;
		/*奇校验*/
        case PARITY_CHECK_ODD:
            val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
            val |= UART_LCR_PEN;
			val &= ~UART_LCR_EPS ;
            (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
            break;
		/*偶校验*/
        case PARITY_CHECK_EVEN:
            val = readl(uart_dev->vir_addr_base + UART_REGOFF_LCR);
            val |= (UART_LCR_PEN | UART_LCR_EPS);
            (void)writel(val, uart_dev->vir_addr_base + UART_REGOFF_LCR);
            break;
			
        default:
            hsuart_error("unsupport eps:%d\n", eps);
            ret = ERROR;
    }
    return ret;
}

/*****************************************************************************
* 函 数 名  : hsuart_alloc_skb
*
* 功能描述  :分配SKB
*
* 输入参数  : pMemNode 数据节点size 缓存大小
* 输出参数  : NA
*
* 返 回 值  : 失败/成功
*
*****************************************************************************/
s32 hsuart_alloc_skb(uart_mem_node* pMemNode, u32 size)
{
   	hsuart_error("hsuart_alloc_skb enter...!\n");
	if(NULL == pMemNode)
	{
       hsuart_error("pMemNode is NULL\n");
	   return ERROR;
	}
    pMemNode->pSkb = alloc_skb((unsigned int)size, GFP_ATOMIC);;   
	if (NULL == pMemNode->pSkb)
    {
        hsuart_error("pMemNode buff alloc fail, size:%d\n", size);
        return ERROR;
    }
    pMemNode->mem.vaddr = (void*)pMemNode->pSkb->data;
  	pMemNode->mem.paddr = dma_map_single(NULL, pMemNode->mem.vaddr, size, DMA_FROM_DEVICE);
    //(void)memset(pMemNode->mem.vaddr, 0, size);

    return OK;
}

/*****************************************************************************
* 函 数 名  : add_mem_to_freelist
*
* 功能描述  : UART UDI 添加节点到 FreeList 中
*
* 输入参数  : trans_info: 读写结构体
*             pMemNode: 待添加的节点
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void add_mem_to_freelist(hsuart_trans_info *trans_info, uart_mem_node* pMemNode)
{
    unsigned long flags;
	hsuart_error("add_mem_to_freelist enter...!\n");
    
    spin_lock_irqsave(&trans_info->list_lock, flags);
    
    list_add_tail(&pMemNode->list_node, &(trans_info->free_list));
    /*去初始化pMemNode成员*/
    pMemNode->state = UART_MEM_IN_FREELIST;
    pMemNode->valid_size = 0;
    pMemNode->pSkb = NULL;
    pMemNode->mem.vaddr = (void*)NULL;
    pMemNode->mem.paddr = (phys_addr)NULL;
    
    spin_unlock_irqrestore(&trans_info->list_lock, flags);
}

/*****************************************************************************
* 函 数 名  : get_mem_from_freelist
*
* 功能描述  : UART UDI 从FreeList 头取出一个节点
*
* 输入参数  : trans_info: 读写结构体
* 输出参数  : NA
*
* 返 回 值  : 返回的节点指针
*
*****************************************************************************/
uart_mem_node* get_mem_from_freelist(hsuart_trans_info *trans_info)
{
    struct list_head *pListNode;
    uart_mem_node* pMemNode;
    unsigned long flags = 0;
	hsuart_error("get_mem_from_freelistenter...!\n");
    
    spin_lock_irqsave(&trans_info->list_lock, flags);
    
    if (list_empty(&(trans_info->free_list)))
    {
        pMemNode = NULL;
    }
    else
    {
        pListNode = trans_info->free_list.next;
        pMemNode = list_entry(pListNode, uart_mem_node, list_node);
        
        list_del_init(pListNode);
        pMemNode->state = UART_MEM_NOT_IN_LIST;
    }
    
    spin_unlock_irqrestore(&trans_info->list_lock, flags);
    
    return pMemNode;
}

/*****************************************************************************
* 函 数 名  : add_mem_to_donelist
*
* 功能描述  : UART UDI 添加节点到 完成链表 中
*
* 输入参数  : trans_info: uart 读写结构体指针
*             pMemNode: 待添加的节点
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void add_mem_to_donelist(hsuart_trans_info *trans_info, uart_mem_node* pMemNode)
{

	unsigned long flags;

    /*入参合法性检查*/
    spin_lock_irqsave(&trans_info->list_lock, flags);

    list_add_tail(&pMemNode->list_node, &(trans_info->done_list));
    trans_info->done_cnt++;
    pMemNode->state = UART_MEM_IN_DONELIST;
    
    spin_unlock_irqrestore(&trans_info->list_lock, flags);
}

/*****************************************************************************
* 函 数 名  : get_mem_from_donelist
*
* 功能描述  : UART UDI 从DoneList 头取出一个节点
*
* 输入参数  : trans_info: 读写结构体
* 输出参数  : NA
* 返 回 值  : 返回的节点指针
*
*****************************************************************************/
uart_mem_node* get_mem_from_donelist(hsuart_trans_info *trans_info)
{
    struct list_head *pListNode;
    uart_mem_node* pMemNode;
    unsigned long flags;

	hsuart_error("get_mem_from_donelist enter...!\n");

    spin_lock_irqsave(&trans_info->list_lock, flags);
    
    if (list_empty(&(trans_info->done_list)))
    {
		hsuart_error("pMemNode is null...!\n");
		pMemNode = NULL;
    }
    else
    {    
        pListNode = trans_info->done_list.next;
        pMemNode = list_entry(pListNode, uart_mem_node, list_node);
        list_del_init(pListNode);
        trans_info->done_cnt--;
        pMemNode->state = UART_MEM_NOT_IN_LIST;
    }
    
    spin_unlock_irqrestore(&trans_info->list_lock, flags);
    
    return pMemNode;
}

/*****************************************************************************
* 函 数 名  : find_mem_from_list
*
* 功能描述  : 从链表中查找内存对应节点
*
* 输入参数  : trans_info 读写结构体 pBuffer 内存地址
* 输出参数  : NA
*
* 返 回 值  : 节点指针
*
*****************************************************************************/
uart_mem_node* find_mem_from_list(hsuart_trans_info *trans_info, BSP_U8* pBuffer)
{
    u32 idx;
    uart_mem_node* pMemNode = NULL;

    for (idx = 0; idx < trans_info->node_num; idx++)
    {        
        if ((u8*)trans_info->pmem_start[idx].pSkb == pBuffer)
        {
            pMemNode = &trans_info->pmem_start[idx];
            break;
        }
    }   
    return pMemNode;
}

/*****************************************************************************
* 函 数 名  : get_list_node_num
*
* 功能描述  : 获取链表中的节点个数
*
* 输入参数  : pFreeHeader: Free List头节点
* 输出参数  : 返回的节点个数
*
* 返 回 值  : 节点个数
*
*****************************************************************************/
BSP_S32 get_list_node_num(struct list_head *pHeader)
{
    struct list_head* pCurPos = NULL;
    struct list_head* pNextPos = NULL;
    BSP_S32 s32Num = 0;

    list_for_each_safe(pCurPos, pNextPos, pHeader)
    {
        s32Num++;
    }
    
    return s32Num;
}

/*****************************************************************************
* 函 数 名  : hsuart_uninit_list
*
* 功能描述  : 去初始化读写链表
*
* 输入参数  : trans_info: 读写结构体

* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_uninit_list(hsuart_trans_info *trans_info)
{
    u32 idx = 0;
    unsigned long flags;

    if(NULL == trans_info)
	{
       hsuart_error("trans_info is NULL\n");
	   return;
	}
    spin_lock_irqsave(&trans_info->list_lock, flags);
    /* 去初始化链表 */
    INIT_LIST_HEAD(&(trans_info->free_list));
    INIT_LIST_HEAD(&(trans_info->done_list));
  
    if (trans_info->pmem_start)
    {
        /* 去初始化所有节点 */
        for (idx = 0; idx < trans_info->node_num; idx++)
        {
            INIT_LIST_HEAD(&(trans_info->pmem_start[idx].list_node));
            trans_info->pmem_start[idx].state = UART_MEM_NOT_IN_LIST;
            trans_info->pmem_start[idx].mem.vaddr = (void*)NULL;
            trans_info->pmem_start[idx].mem.paddr = (phys_addr)NULL;
            trans_info->pmem_start[idx].valid_size = 0;
            /* 读节点无需释放SKB?*/
            if (trans_info->pmem_start[idx].pSkb)
            {
                kfree_skb(trans_info->pmem_start[idx].pSkb);
                trans_info->pmem_start[idx].pSkb = NULL;
            }
            /*其他成员去初始化*/
        }

        kfree(trans_info->pmem_start);
        trans_info->pmem_start = NULL;
    }
    
    trans_info->pmem_end = NULL;
    trans_info->pcur_pos = NULL;
    trans_info->node_num = 0;
    trans_info->node_size = 0;
    trans_info->done_cnt = 0;
    trans_info->total_bytes = 0;
    spin_unlock_irqrestore(&trans_info->list_lock, flags);
}

/*****************************************************************************
* 函 数 名  : hsuart_init_list
*
* 功能描述  : 初始化读写链表
*
* 输入参数  : trans_info: 读写结构体
*             
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_init_list(hsuart_trans_info *trans_info)
{
    BSP_U8* pMemHead = NULL;
    struct list_head *pListNode = NULL;
    u32 idx = 0;
    BSP_S32 ret = OK;
    unsigned long flags;
	
	hsuart_error("hsuart_init_list enter...!\n");

    if(NULL == trans_info)
	{
       hsuart_error("trans_info is NULL\n");
	   return ERROR;
	}
    spin_lock_irqsave(&trans_info->list_lock, flags);//阻塞信号量
    /* 初始化链表 */
    INIT_LIST_HEAD(&(trans_info->free_list));
    INIT_LIST_HEAD(&(trans_info->done_list));
    
    /* 初始化读信息 */
    pMemHead = osl_malloc(trans_info->node_num*sizeof(uart_mem_node));
    if (pMemHead)
    {
        (void)memset(pMemHead, 0, trans_info->node_num*sizeof(uart_mem_node));
        trans_info->pmem_start = (uart_mem_node*)pMemHead;
        trans_info->pmem_end = (uart_mem_node*)(pMemHead + (trans_info->node_num*sizeof(uart_mem_node)));
        trans_info->pcur_pos = NULL;
        trans_info->done_cnt = 0;
        trans_info->total_bytes = 0;

        /* 初始化时,所有节点都在FreeList */
        for (idx = 0; idx < trans_info->node_num; idx++)
        {
            pListNode = &(trans_info->pmem_start[idx].list_node);
            list_add_tail(pListNode, &(trans_info->free_list));
            trans_info->pmem_start[idx].state = UART_MEM_IN_FREELIST;
            trans_info->pmem_start[idx].valid_size = 0;
            (void)memset(&(trans_info->pmem_start[idx].mem), 0, sizeof(mem_desc_t));
            trans_info->pmem_start[idx].pSkb = NULL;
            /*其他成员初始化*/
        }
    }
    else
    {
        ret = ERROR;
    }
    spin_unlock_irqrestore(&trans_info->list_lock, flags);

    return ret;
}

/*****************************************************************************
* 函 数 名  : hsuart_buf_init
*
* 功能描述  :初始化读写缓存队列
*
* 输入参数  : uart_ctx 上下文
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_buf_init(hsuart_ctx *uart_ctx)
{
    s32 ret = OK;
	hsuart_error("hsuart_buf_init enter...!\n");
	
    if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    /* 初始化读写初始值 */
    memset(&(uart_ctx->read_info), 0, sizeof(hsuart_trans_info));
    memset(&(uart_ctx->write_info), 0, sizeof(hsuart_trans_info));
    
    spin_lock_init(&(uart_ctx->read_info.list_lock));
    spin_lock_init(&(uart_ctx->write_info.list_lock));

    uart_ctx->read_info.node_num = UART_READ_BUF_NUM;   //读节点数量16
    uart_ctx->read_info.node_size = UART_READ_BUF_SIZE; //每个节点大小1536字节
    uart_ctx->write_info.node_num = UART_WRITE_BUF_NUM; //写节点数量64
    uart_ctx->write_info.node_size = 0;					//每个写节点大小0
	/*初始化读写链表，将节点挂到freelist*/
	if (hsuart_init_list(&(uart_ctx->read_info)) || hsuart_init_list(&(uart_ctx->write_info)))
    {
        hsuart_error("hsuart_init_list error.\n");
        ret = ERROR;
    }
   
    return ret;
}

/*****************************************************************************
* 函 数 名  : gpio_dtr_irq
*
* 功能描述  : GPIO 中断处理
*
* 输入参数  : irq 中断号 dev_id 传参
* 输出参数  : NA
*
* 返 回 值  : 成功/失败bcmsdh_register_oob_intr
*
*****************************************************************************/
static irqreturn_t gpio_dtr_irq(int irq, void *dev_id)
{
   // hsuart_ctx* uart_ctx = (hsuart_ctx*)dev_id;
    MODEM_MSC_STRU modem_msc;

    gpio_int_mask_set(HSUART_DTR);
	
    memset(&modem_msc, 0, sizeof(MODEM_MSC_STRU));
    modem_msc.OP_Dtr = SIGNALCH;
    modem_msc.ucDtr = !(BSP_U8)gpio_get_value(HSUART_DTR);
    hsuart_error("gpio_dtr_irq:%d\n", modem_msc.ucDtr);
//    if (uart_ctx->cbs.msc_read_cb)
 //   {
 //       uart_ctx->cbs.msc_read_cb(&modem_msc);
 //   }
 //   else
    {
        hsuart_error("value:%d   msc_read_cb is NULL.\n", modem_msc.ucDtr);
    }
    
    gpio_int_state_clear(HSUART_DTR);
    gpio_int_unmask_set(HSUART_DTR);  
    
    return IRQ_HANDLED;
}

/*****************************************************************************
* 函 数 名  : hsuart_gpio_init
*
* 功能描述  : uart gpio 模拟管脚初始化函数
*
* 输入参数  : uart_ctx 上下文
* 输出参数  : NA
*
* 返 回 值  : 成功/失败bcmsdh_register_oob_intr
*
*****************************************************************************/
int hsuart_gpio_init(hsuart_ctx *uart_ctx)
{
    int ret;
    int irq_id = 0;
	hsuart_error("hsuart_gpio_init enter...!\n");

    if (gpio_request(HSUART_RING, "GPIO_RING"))
    {
        hsuart_error("gpio=%d is busy.\n", HSUART_RING);
        return ERROR;
    }

    if (gpio_request(HSUART_DSR, "GPIO_DSR"))
    {
        hsuart_error("gpio=%d is busy.\n", HSUART_DSR);
        return ERROR;
    }

    if (gpio_request(HSUART_DCD, "GPIO_DCD"))
    {
        hsuart_error("gpio=%d is busy.\n", HSUART_DCD);
        return ERROR;
    }
  
    gpio_direction_output(HSUART_RING, HIGHLEVEL); 
    gpio_direction_output(HSUART_DSR, HIGHLEVEL); 
    gpio_direction_output(HSUART_DCD, HIGHLEVEL); 
    
    if (gpio_request(HSUART_DTR, "GPIO_DTR"))
    {
        hsuart_error("gpio=%d is busy.\n", HSUART_DTR);
        return ERROR;
    }
    
    gpio_direction_input(HSUART_DTR);     
    gpio_int_mask_set(HSUART_DTR);    
    gpio_int_state_clear(HSUART_DTR);    
//    gpio_set_function(HSUART_DTR, GPIO_INTERRUPT);
    hsuart_error("GPIO: %d gpio_set_function done.\n", HSUART_DTR);
    
	irq_id = gpio_to_irq(HSUART_DTR);
    ret = request_irq(irq_id, gpio_dtr_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"HSUART_DTR", (void*)uart_ctx);
    if (ret != 0) 
	{
        hsuart_error("failed at register gpio-oob irq.\n");
        return ERROR;
    }

    hsuart_error("GPIO: %d oob-unmask.\n", HSUART_DTR);
    
    gpio_int_state_clear(HSUART_DTR);
    gpio_int_unmask_set(HSUART_DTR);
    
    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_tx_poll
*
* 功能描述  : UART CPU 模式发送数据函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             uart_tx_buf: 发送数据缓存地址
*             length: 发送数据长度
* 输出参数  :
*
* 返 回 值  : 发送数据长度
*
*****************************************************************************/
int hsuart_tx_poll(hsuart_dev *uart_dev, unsigned char *uart_tx_buf,unsigned int length)
{
    unsigned char *buf = uart_tx_buf;
    unsigned int cnt = length;
    unsigned int value = 0;

    value = readl(uart_dev->vir_addr_base + UART_REGOFF_FCR);
    value |= UART_FCR_FIFO_ENABLE;
    (void)writel(value, uart_dev->vir_addr_base + UART_REGOFF_FCR);
    do
    {
        value = readl(uart_dev->vir_addr_base + UART_REGOFF_USR);
        if ((value & UART_USR_FIFO_NOT_FULL) == 0x02)
        {
             (void)writel(*(buf++), uart_dev->vir_addr_base + UART_REGOFF_THR);
             --cnt;
        }
        else
        {
#ifndef _DRV_LLT_
            /* TX FIFO FULL */
            hsuart_error("UART01x_FR:0x%x  cnt:%u\n", value, cnt);
            msleep(1);
#endif
        }
    }while (cnt > 0); 

    return length;
}

/*****************************************************************************
* 函 数 名  : hsuart_dma_tx_complete
*
* 功能描述  : UART DMA 模式发送完成函数
*
* 输入参数  : arg: 设备管理结构指针
*             int_status: 中断状态
* 输出参数  :
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_dma_tx_complete(u32 arg, u32 int_status)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    
    hsuart_error("UART TX DMA transfer complete.\n");
    up(&uart_ctx->dma_sema);
}

/*****************************************************************************
* 函 数 名  : hsuart_tx_dma
*
* 功能描述  : UART DMA 模式发送数据函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             dma_handle: 发送数据缓存地址
*             length: 发送数据长度
* 输出参数  :
*
* 返 回 值  : 发送数据长度
*
*****************************************************************************/
int hsuart_tx_dma(hsuart_dev *uart_dev, u32 dma_handle, u32 length)
{
    BSP_S32 ret_id = 0;
    hsuart_ctx *uart_ctx = NULL;

    uart_ctx = container_of(uart_dev, hsuart_ctx, dev);
    
	ret_id = bsp_edma_channel_init(uart_dev->dma_req_tx, hsuart_dma_tx_complete, (u32)uart_ctx, BALONG_DMA_INT_DONE);
	if (ret_id < 0)
    {
        hsuart_error("balong_dma_channel_init error ret_id = %d\n", ret_id);
        return ERROR;
    }

    /* 配置config 寄存器 */
    if (bsp_edma_channel_set_config(ret_id, BALONG_DMA_M2P, EDMA_TRANS_WIDTH_32, EDMA_BUR_LEN_16))
    {
        hsuart_error("balong_dma_channel_set_config failed! ret_id = %d\n", ret_id);
        return ERROR;
    }

    /* 启动一次EDMA 异步数据搬运*/
    if (bsp_edma_channel_async_start(ret_id, dma_handle, (u32)(uart_dev->phy_addr_base + UART_REGOFF_THR), length))
    {
        hsuart_error("balong_dma_channel_async_start transfer failed! ret_id = %d\n", ret_id);
        return ERROR;
    }
    down(&uart_ctx->dma_sema);
    
    return length;
}

u32 hsuart_dma_rx_length(u32 channel_id, u32 start_addr)
{
    s32 dst_addr = 0;
    u32 length = 0;
    unsigned long flags;

    /*Get Data Buffer Length*/
    local_irq_save(flags);
    dst_addr = bsp_edma_current_transfer_address(channel_id);
    if (dst_addr >0)
    {
		length = (u32)((dst_addr - start_addr) / sizeof(u8));
    	//*pcur_addr = dst_addr;
    }

    local_irq_restore(flags);

    return length;
}

/*****************************************************************************
* 函 数 名  : hsuart_dma_tx_complete
*
* 功能描述  : UART DMA 模式发送完成函数
*
* 输入参数  : arg: 设备管理结构指针
*             int_status: 中断状态
* 输出参数  :
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_dma_rx_complete(u32 arg, u32 int_status)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    
    hsuart_error("UART RX DMA transfer complete.\n");

    uart_ctx->dma_info.rx_dma_done = HSUART_TRUE;

    bsp_softtimer_delete(&g_hsuart_ctrl.sleep_timer);
    up(&(uart_ctx->sleep_sema));
}

/*****************************************************************************
* 函 数 名  : hsuart_rx_dma
*
* 功能描述  : UART DMA 模式接收数据函数
*
* 输入参数  : uart_dev: 设备管理结构指针
*             dma_handle: 接收数据缓存地址
*             length: 接收数据长度
* 输出参数  :
*
* 返 回 值  : DMA channel id
*
*****************************************************************************/
BSP_S32 hsuart_rx_dma(hsuart_dev *uart_dev, u32 dma_handle, u32 length)
{
    BSP_S32 ret_id = 0;
    hsuart_ctx *uart_ctx = NULL;

    uart_ctx = container_of(uart_dev, hsuart_ctx, dev);
    
    ret_id = bsp_edma_channel_init(uart_dev->dma_req_rx, hsuart_dma_rx_complete, (u32)uart_ctx, BALONG_DMA_INT_DONE);
    if (ret_id < 0)
    {
        hsuart_error("balong_dma_channel_init error ret_id = %d\n", ret_id);
        return ERROR;
    }

    /* 配置config 寄存器 */
    if (bsp_edma_channel_set_config(ret_id, BALONG_DMA_P2M, EDMA_TRANS_WIDTH_32, EDMA_BUR_LEN_16))
    {
        hsuart_error("balong_dma_channel_set_config failed! ret_id = %d\n", ret_id);
        return ERROR;
    }
    
    /* 启动一次EDMA 异步数据搬运*/
    if (bsp_edma_channel_async_start(ret_id, (u32)(uart_dev->phy_addr_base + UART_REGOFF_RBR), dma_handle, length))
    {
        hsuart_error("balong_dma_channel_async_start transfer failed! ret_id = %d\n", ret_id);
        return ERROR;
    }
    
    return ret_id;
}

/*****************************************************************************
* 函 数 名  : hsuart_send
*
* 功能描述  : uart 发送函数
*
* 输入参数  : uart_ctx: 上下文
*             buffer: 发送数据缓存地址
*             length: 发送数据长度
* 输出参数  :
*
* 返 回 值  : 发送数据长度
*
*****************************************************************************/
int hsuart_send(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
    int ret = OK;
    hsuart_dev *uart_dev = &uart_ctx->dev;
    
    hsuart_error("enter. pSkb:0x%x, valid_size:0x%x\n", (unsigned int)pMemNode->pSkb, (unsigned int)pMemNode->valid_size);
    if (CPU_TRANSFER == uart_ctx->transfer_mode)
    {
        ret = hsuart_tx_poll(uart_dev, (unsigned char *)pMemNode->mem.vaddr, pMemNode->valid_size);
        msleep(1);
    }
    else
    {
        ret = hsuart_tx_dma(uart_dev, (u32)pMemNode->mem.paddr, pMemNode->valid_size);
    }
    
    return ret;
}

/*******************************************************************************
* 函 数 名  : switch_stage_two
*
* 功能描述  : 模式切换第二阶段
*
* 输入参数  : arg 传参

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
void switch_stage_two(u32 arg)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    
    uart_ctx->stage = DETECT_STAGE_2;
    uart_ctx->switch_timer.flag = HSUART_FALSE;
    hsuart_error("enter switch stage two...\n");
}

void switch_stage_one(u32 arg)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    
    uart_ctx->stage = DETECT_STAGE_2;
    uart_ctx->switch_timer.flag = HSUART_FALSE;
    hsuart_error("enter switch stage two...\n");
}
/*******************************************************************************
* 函 数 名  : switch_stage_three
*
* 功能描述  : 模式切换第三阶段
*
* 输入参数  : arg 传参

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
void switch_stage_three(u32 arg)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    uart_mem_node* pMemNode;
    
    pMemNode = uart_ctx->read_info.pcur_pos;
    pMemNode->valid_size = 0;
    uart_ctx->stage = DETECT_STAGE_1;
    
//    if (uart_ctx->cbs.switch_mode_cb)
   // {
   //     uart_ctx->cbs.switch_mode_cb();
  //      hsuart_error("switch cmd mode.\n");
  //  }
//    else
    {
        hsuart_error("switch_mode_cb is NULL.\n");
    }

    uart_ctx->switch_timer.flag = HSUART_FALSE;
    //check_create_softtimer(&uart_ctx->switch_timer, SWITCH_TIMER_LENGTH, SOFTTIMER_NO_LOOP,
    //               (SOFTTIMER_FUNC)switch_stage_two, (UINT32)uart_ctx);
}

/*******************************************************************************
* 函 数 名  : set_next_adapt
*
* 功能描述  : 波特率自适应超时
*
* 输入参数  : arg 传参

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
void set_next_adapt(u32 arg)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)arg;
    
    /*重新设置波特率*/
    hsuart_disable(&uart_ctx->dev);
    hsuart_set_baud(&uart_ctx->dev, 0);
    hsuart_enable(&uart_ctx->dev);
    bsp_softtimer_add(&g_hsuart_ctrl.adapt_timer);
}

/*****************************************************************************
* 函 数 名  : switch_mode_cmd_detect
*
* 功能描述  : 模式切换命令检测函数
*
* 输入参数  : uart_ctx 上下文 pMemNode 数据节点指针
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int switch_mode_cmd_detect(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
    int result = 0;
    
    /*入参合法性检查*/
    if (DETECT_STAGE_2 == uart_ctx->stage)
    {
        /*该数据节点内数据为"+++"*/
        if (pMemNode)
        {
            result = !memcmp((void*)switch_cmd, (void*)pMemNode->mem.vaddr, strlen(switch_cmd));
            result = result && (pMemNode->valid_size == strlen(switch_cmd));
        }
        
        if (result)
        {
            /*起1S钟的超时SoftTimer，（在UART接收数据中断中，判断删除该SoftTimer，若该SoftTimer能触发超时中断处理，则表示后1S无数据下发）*/
            /*设置当前处于命令模式切换第三阶段*/
			uart_ctx->stage = DETECT_STAGE_3;
            bsp_softtimer_add(&g_hsuart_ctrl.switch_state3_timer);
        }
        else
        {
			/*设置当前处于命令模式切换第一阶段，重新等1S空闲*/
			hsuart_error("DETECT_STAGE_2 to DETECT_STAGE_1.\n");           
            uart_ctx->stage = DETECT_STAGE_1;
            bsp_softtimer_add(&g_hsuart_ctrl.switch_state1_timer);
        }
    }
    
    return result;
}

/*******************************************************************************
* 函 数 名  : discard_read_mem_node
*
* 功能描述  : 丢弃读数据节点
*
* 输入参数  : uart_ctx 上下文 pMemNode 内存节点

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
void discard_read_mem_node(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
    kfree_skb(pMemNode->pSkb);
    /*将该数据节点放回Freelist，即丢弃该数据*/
    add_mem_to_freelist(&uart_ctx->read_info, pMemNode);
}

/*******************************************************************************
* 函 数 名  : check_mem_to_donelist
*
* 功能描述  : 接收数据检查
*
* 输入参数  : uart_ctx 上下文 pMemNode 内存节点

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
int check_mem_to_donelist(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
    s32 ret = ERROR;
    hsuart_error("check_mem_to_donelist enter...!\n");

    if (uart_ctx->baud_adapt)
    {
        /*数据适配AT 是否成功*/
        if (memcmp("AT", pMemNode->mem.vaddr, 2) && memcmp("at", pMemNode->mem.vaddr, 2))
        {
            hsuart_error("baud adapt fail!   idx:%d baud:%d\n", uart_ctx->baud_idx, typical_baud[uart_ctx->baud_idx]);
            /* 丢弃该数据*/
            pMemNode->valid_size = 0;
            /*重新设置波特率*/
            hsuart_disable(&uart_ctx->dev);
            hsuart_set_baud(&uart_ctx->dev, 0);
            hsuart_enable(&uart_ctx->dev);
            bsp_softtimer_add(&g_hsuart_ctrl.adapt_timer);
        }
        else
        {
            hsuart_error("baud adapt success!   idx:%d baud:%d\n", uart_ctx->baud_idx, typical_baud[uart_ctx->baud_idx]);
            uart_ctx->baud_adapt = HSUART_FALSE;
            add_mem_to_donelist(&uart_ctx->read_info, pMemNode);
			bsp_softtimer_delete(&g_hsuart_ctrl.adapt_timer);
            ret = OK;
        }
    }
    else
    {
        if (!switch_mode_cmd_detect(uart_ctx, pMemNode))
        {
            add_mem_to_donelist(&uart_ctx->read_info, pMemNode);
            ret = OK;
        }
    }

    return ret;
}

/*******************************************************************************
* 函 数 名  : hsuart_rx_complete
*
* 功能描述  : 接收完成
*
* 输入参数  : uart_ctx 上下文

* 输出参数  : NA
*
* 返 回 值  : NA
*
*******************************************************************************/
void hsuart_rx_complete(hsuart_ctx *uart_ctx)
{
    uart_mem_node* pMemNode;
    
    pMemNode = uart_ctx->read_info.pcur_pos;

    if (!pMemNode)
    {
		hsuart_error("pMemNode is null...!\n");

		return;
    }

    if (pMemNode->pSkb)
    {
        /*使用标准的API来修改len */
        skb_put(pMemNode->pSkb, pMemNode->valid_size);
    }
   
  // if (uart_ctx->cbs.read_cb)
   {
        if (OK == check_mem_to_donelist(uart_ctx, pMemNode))
        {
            uart_ctx->read_info.total_bytes += pMemNode->valid_size;
            //uart_ctx->cbs.read_cb();
            uart_ctx->read_info.pcur_pos = NULL;
        }
    }

    //else
    //{
        /* 读回调未注册 */
     //   hsuart_error("ReadCb is null.\n");
        /* 丢弃该数据*/
    //    pMemNode->valid_size = 0;
    //}
}



/*******************************************************************************
* 函 数 名  : get_read_node
*
* 功能描述  : UART 获取当前写地址
*
* 输入参数  : uart_ctx 上下文

* 输出参数  : NA
*
* 返 回 值  : 写地址
*
*******************************************************************************/
uart_mem_node* get_read_node(hsuart_ctx *uart_ctx)
{
    uart_mem_node* pMemNode;
	
	hsuart_error("get_read_node enter...!\n");
    
    if (NULL == uart_ctx->read_info.pcur_pos)
    {
        pMemNode = get_mem_from_freelist(&(uart_ctx->read_info));
        if (pMemNode)
        {
            if (!pMemNode->pSkb)
            {
                if (OK != hsuart_alloc_skb(pMemNode, uart_ctx->read_info.node_size))
                {
                    add_mem_to_freelist(&(uart_ctx->read_info), pMemNode);
                    pMemNode = NULL;
                    uart_ctx->stat.alloc_skb_fail++;
                }
            }
            uart_ctx->read_info.pcur_pos = pMemNode;
        }
    }
    else
    {
        pMemNode = uart_ctx->read_info.pcur_pos;        
    }
    
    return pMemNode;
}


void hsuart_sleep_cb(u32 arg)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx*)arg;

	//hsuart_error("hsuart_sleep_cb enter...!\n");

    up(&(uart_ctx->sleep_sema));
}

void hsuart_sleep(hsuart_ctx *uart_ctx, u32 length)
{

    if (0 == length)
    {
	    hsuart_error("hsuart_sleep length =0 ...!\n");
		return;
    }
	bsp_softtimer_delete(&g_hsuart_ctrl.sleep_timer);
	if(OK == bsp_softtimer_modify(&g_hsuart_ctrl.sleep_timer, length))
   	{
	    bsp_softtimer_add(&g_hsuart_ctrl.sleep_timer);
		down(&(uart_ctx->sleep_sema));
	    if (uart_ctx->sleep_out)
	    {
	        msleep(1000);
	        uart_ctx->sleep_out = HSUART_FALSE;
	        uart_ctx->stat.sleep_delay_cnt++;
	    }
		//hsuart_error("get sleep_sema ...!\n");
    }
    else
    {
		hsuart_error("softtimer_modify fail...!\n");
		msleep(length);
    }
}
void hsuart_single_dma_transfer(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
    u32 channel_id = 0;
    u32 start_addr = 0;
    u32 cur_addr = 0;
    u32 to_transfer = 0;
    volatile u32 total_transfered = 0;
    u32 cnt = 0;
	u32 ulCharNum = 0;
	u32 Data = 0;
	u8 *recvTemp = 0;
	u32 i = 0;
	hsuart_dev *uart_dev;
	
	uart_dev = &uart_ctx->dev;
	
    uart_ctx->dma_info.rx_dma_done = HSUART_FALSE;

    to_transfer = uart_ctx->read_info.node_size - pMemNode->valid_size;
    to_transfer = (to_transfer > 32) ? 32 : to_transfer;
	start_addr = pMemNode->mem.paddr + pMemNode->valid_size;

    channel_id = hsuart_rx_dma(&uart_ctx->dev, start_addr, to_transfer);
    cnt = 0;

    while (!uart_ctx->dma_info.rx_dma_done)
	{
        hsuart_sleep(uart_ctx, uart_ctx->dma_info.single_delay);
		
	    total_transfered = hsuart_dma_rx_length(channel_id, start_addr);

		if (total_transfered)
	    {
			if(uart_ctx->dma_info.single_loop <= cnt++)
			{
				hsuart_error("single rx time out!\n");
				hsuart_error("total_transfered: 0x%x\n",total_transfered);
	            break;
			}
	    }
		else 
		{
			ulCharNum = readl(uart_dev->vir_addr_base + 0x84);
			if(ulCharNum)
			{
				//hsuart_error("ulCharNum:0x%x\n",ulCharNum);
				break;
			}
		}

        if (DETECT_STAGE_1 == uart_ctx->stage)
        {
           if (!uart_ctx->switch_timer.flag)
            {
                   // check_create_softtimer(&uart_ctx->switch_timer, SWITCH_TIMER_LENGTH, SOFTTIMER_NO_LOOP,
                  //             (SOFTTIMER_FUNC)switch_stage_two, (UINT32)uart_ctx);
            }
        }
		
    }
      
	total_transfered = bsp_edma_channel_stop(channel_id);
	if (uart_ctx->dma_info.rx_dma_done)
    {
		pMemNode->valid_size += to_transfer;
        uart_ctx->stat.single_dma_done++;
		return;
    }
	total_transfered = (total_transfered - start_addr)/sizeof(u8);
	hsuart_error("total_transfered: 0x%x\n",total_transfered);

	
	cur_addr = (u32)pMemNode->mem.vaddr + (pMemNode->valid_size + total_transfered);

	ulCharNum = readl(uart_dev->vir_addr_base + 0x84);
	pMemNode->valid_size += (total_transfered + ulCharNum);

	while(ulCharNum)
	{
		if(ulCharNum >=4)
		{
			Data = readl(uart_dev->vir_addr_base+ UART_REGOFF_RBR);
			recvTemp = (u8 *)&Data;
			for(i =0;i<4;i++)
			{
				writel(recvTemp[i],cur_addr);
				cur_addr++;
			}
			ulCharNum = ulCharNum -4;
		}
		else
		{	
			Data = readl(uart_dev->vir_addr_base+ UART_REGOFF_RBR);
			recvTemp = (u8 *)&Data;
			for(i = 0; i <ulCharNum; i++)
			{		
				writel(recvTemp[i],cur_addr);
				cur_addr++;	
			}
			break;
		}	
	}

    hsuart_error("channel_id:%d   start_addr:0x%x   total_transfered:0x%x\n", channel_id, start_addr, total_transfered);
	uart_ctx->stat.single_time_out++;
	ulCharNum = readl(uart_dev->vir_addr_base + 0x84);
	hsuart_error("rx_dma_done ulCharNum:0x%x\n",ulCharNum);	
}


void hsuart_burst_dma_transfer(hsuart_ctx *uart_ctx, uart_mem_node* pMemNode)
{
 	u32 channel_id = 0;
    u32 start_addr = 0;
    u32 cur_addr = 0;
    u32 cur_transfered = 0;
    u32 total_transfered = 0;
//    u32 ulCharNum = 0;
	u32 startaddr = 0;
	hsuart_dev *uart_dev;
	
	uart_dev = &uart_ctx->dev;
    uart_ctx->dma_info.rx_dma_done = HSUART_FALSE;
    start_addr = (u32)pMemNode->mem.paddr;
	channel_id = hsuart_rx_dma(&uart_ctx->dev, start_addr, uart_ctx->read_info.node_size);
	
	while (!uart_ctx->dma_info.rx_dma_done)
    {	
    		
        hsuart_sleep(uart_ctx, uart_ctx->dma_info.burst_delay);
		 //hsuart_sleep(uart_ctx, 500);
		cur_transfered = hsuart_dma_rx_length(channel_id, start_addr);
		
		//hsuart_error("hsuart_burst_dma_transfer cur_transfered :0x%x.....\n",cur_transfered);
		
		if((startaddr == cur_transfered)||(0 == cur_transfered))
		{
			
			cur_addr = bsp_edma_channel_stop(channel_id);
			hsuart_error("hsuart_burst_dma_transfer time out\n");
			break;	
		}	
		startaddr = cur_transfered;
			
		
		//hsuart_sleep(uart_ctx, uart_ctx->dma_info.burst_delay);
   }
    
    if (DMA_FAIL != cur_addr)
    {
        if (uart_ctx->dma_info.rx_dma_done)
        {
			hsuart_error("hsuart_burst_dma_transfer rx_dma_done!\n");
			pMemNode->valid_size = uart_ctx->read_info.node_size;
            uart_ctx->stat.burst_dma_done++;
        }
        else
        {
            total_transfered = (cur_addr - start_addr)/sizeof(u8);
            pMemNode->valid_size = total_transfered;
			hsuart_error("channel_id:%d   start_addr:0x%x   cur_transfered:0x%x   total_transfered:0x%x\n", channel_id, start_addr, cur_transfered, total_transfered);

           hsuart_single_dma_transfer(uart_ctx, pMemNode);            
       }
   }
    else
    {
        hsuart_error("disable dma channel fail!\n");
    }

}

/*****************************************************************************
* 函 数 名  : hsuart_rx_thread
*
* 功能描述  : UART 底层接收线程
*
* 输入参数  : ctx 传参
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
s32 hsuart_rx_thread(void *ctx)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)ctx;
    uart_mem_node* pMemNode = NULL;
    
    if(NULL == ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    while (1)
    {
		if (!uart_ctx->rts)
        {
            pMemNode = get_read_node(uart_ctx);
            if (pMemNode)
            {
                while (uart_ctx->read_info.pcur_pos)
                {
                    hsuart_burst_dma_transfer(uart_ctx, pMemNode);
					//hsuart_error("hsuart_rx_thread enter...!\n");
										

#if 0
                    /*第一和第三阶段起的Softtimer 都删除*/  
                    if (OK == check_delete_softtimer(&uart_ctx->switch_timer))
                    {
                        /*若删除第三阶段Softtimer，需重新进入第一阶段，且往上发送+++*/
                        if (DETECT_STAGE_3 == uart_ctx->stage)
                        {
                            uart_ctx->stage = DETECT_STAGE_1;
                        }
                    }
#endif
                    if (pMemNode->valid_size)
                    {
                        hsuart_error("pMemNode->valid_size:0x%x\n", (u32)pMemNode->valid_size);
                        hsuart_rx_complete(uart_ctx);
                    }
                }
            }

            else
            {
                msleep(1);
            }
        }
        else
        {
            msleep(1);
        }   
  }
   
    return 0;
}

/*****************************************************************************
* 函 数 名  : wait_tx_fifo_empty
*
* 功能描述  : UART 等待发送完成
*
* 输入参数  : ctx 传参
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void wait_tx_fifo_empty(hsuart_ctx *uart_ctx)
{
    u32 cnt = 0;
    u32 status;
    
    status = readl(uart_ctx->dev.vir_addr_base + UART_REGOFF_USR);
    while (0 == (status & UART_USR_FIFO_EMP))
    {
        msleep(1);
        if (10000 < cnt++)
        {
            hsuart_error("wait time out.\n");
            break;
        }
        status = readl(uart_ctx->dev.vir_addr_base + UART_REGOFF_USR);
    }
}

/*****************************************************************************
* 函 数 名  : check_set_feature
*
* 功能描述  : uart 检查设置属性
*
* 输入参数  : uart_ctx:uart上下文
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void check_set_feature(hsuart_ctx *uart_ctx)
{
    unsigned long flags = 0;
    
    if (uart_ctx->cbs.set_baud_cb || uart_ctx->cbs.set_wlen_cb || uart_ctx->cbs.set_stp2_cb || uart_ctx->cbs.set_eps_cb)
    {
        wait_tx_fifo_empty(uart_ctx);
        spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
        hsuart_disable(&uart_ctx->dev);
        if (uart_ctx->cbs.set_baud_cb)
        {
            uart_ctx->cbs.set_baud_cb(&uart_ctx->dev, uart_ctx->feature.baud);
            uart_ctx->cbs.set_baud_cb = NULL;
        }
        if (uart_ctx->cbs.set_wlen_cb)
        {
            uart_ctx->cbs.set_wlen_cb(&uart_ctx->dev, uart_ctx->feature.wlen);
            uart_ctx->cbs.set_wlen_cb = NULL;
        }
        if (uart_ctx->cbs.set_stp2_cb)
        {
            uart_ctx->cbs.set_stp2_cb(&uart_ctx->dev, uart_ctx->feature.stp2);
            uart_ctx->cbs.set_stp2_cb = NULL;
        }
        if (uart_ctx->cbs.set_eps_cb)
        {
            uart_ctx->cbs.set_eps_cb(&uart_ctx->dev, uart_ctx->feature.eps);
            uart_ctx->cbs.set_eps_cb = NULL;
        }
        hsuart_enable(&uart_ctx->dev);
        spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
    }
}
/*****************************************************************************
* 函 数 名  : hsuart_irq
*
* 功能描述  : uart 中断服务程序
*
* 输入参数  : irq:中断号
*                           dev_id:传参
* 输出参数  : NA
*
* 返 回 值  : 中断返回值
*
*****************************************************************************/
irqreturn_t hsuart_irq(int irq, void *ctx)
{
   	u32 ulCharNum;
   	u32 ulInt = 0;
	u32 i = 0;
	u32 Data = 0;
	u8 *recvTemp = 0;
	hsuart_dev *uart_dev = (hsuart_dev *)ctx;
	
    ulInt = readl(uart_dev->vir_addr_base+ UART_REGOFF_IIR);
    ulInt &= UART_FIFO_MASK;

    if(ulInt == UART_IIR_REV_TIMEOUT) //接收超时中断
    {	
		hsuart_error("UART_IIR_REV_TIMEOUT ......!\n");
        ulCharNum = readl(uart_dev->vir_addr_base + 0x84);
       	while(ulCharNum)
		{
			if(ulCharNum >=4)
			{
				Data = readl(uart_dev->vir_addr_base+ UART_REGOFF_RBR);
				recvTemp = (u8 *)&Data;
				for(i =0;i<4;i++)
				{
					M2m_hsuart_InQue(&hsuart_recv_str, recvTemp[i]);
				}
				ulCharNum = ulCharNum -4;
			}
			else
			{
				Data = readl(uart_dev->vir_addr_base+ UART_REGOFF_RBR);
				recvTemp = (u8 *)&Data;
				for(i = 0; i <ulCharNum; i++)
				{
					M2m_hsuart_InQue(&hsuart_recv_str, recvTemp[i]);
				}
				break;
			}
        }
    }

    else if(ulInt == UART_IIR_REV_VALID)   //接收数据有效中断
    {

		hsuart_error("UART_IIR_REV_VALID ......!\n");

		ulCharNum = readl(uart_dev->vir_addr_base + UART_REGOFF_USR);
        while((ulCharNum & 0x8) == UART_USR_FIFO_NOT_EMP)
        {
            Data = readl(uart_dev->vir_addr_base+ UART_REGOFF_RBR);
			recvTemp = (u8 *)&Data;
			for(i =0;i<4;i++)
			{
				M2m_hsuart_InQue(&hsuart_recv_str, recvTemp[i]);
			}
			ulCharNum = readl(uart_dev->vir_addr_base + UART_REGOFF_USR);

        }
    }
	return (irqreturn_t)IRQ_HANDLED;
}
/*****************************************************************************
* 函 数 名  : hsuart_tx_thread
*
* 功能描述  : UART 底层发送线程
*
* 输入参数  : ctx 传参
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
s32 hsuart_tx_thread(void *ctx)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)ctx;
    uart_mem_node* pMemNode = NULL;
    int ret = OK;
    
	if(NULL == ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    while (1)
    {
		 hsuart_error("hsuart_tx_thread enter...!\n");

		/* 波特率自适应期间不上报消息*/
        if (!uart_ctx->baud_adapt)
        {
            uart_ctx->tx_running = HSUART_FALSE;
            down(&(uart_ctx->tx_sema));
            uart_ctx->tx_running = HSUART_TRUE;
   
            pMemNode = get_mem_from_donelist(&(uart_ctx->write_info));
            while (pMemNode)
            {
                ret = hsuart_send(uart_ctx, pMemNode);
                /* pMemNode成员去初始化*/
			
                if (uart_ctx->cbs.free_cb)
                {
                    uart_ctx->cbs.free_cb((char*)pMemNode->pSkb);
                }
				
                else
                {
                    uart_ctx->stat.write_not_free_cnt++;
                    hsuart_error("no write free func register.\n");
                    kfree_skb(pMemNode->pSkb);
                }
                add_mem_to_freelist(&(uart_ctx->write_info), pMemNode);
                pMemNode = get_mem_from_donelist(&(uart_ctx->write_info));
            }
            //check_set_feature(uart_ctx);
        }
        else
        {
            msleep(10);
        }
    }
    return 0;
}

/*****************************************************************************
* 函 数 名  : hsuart_deepsleep_in
*
* 功能描述  : uart 睡眠函数
*
* 输入参数  : NA
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_deepsleep_in(void)
{
  //  hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;

    //SOFTTIMER_DEL_TIMER(&uart_ctx->sleep_timer.id);

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_deepsleep_out
*
* 功能描述  : uart 睡眠函数
*
* 输入参数  : NA
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_deepsleep_out(void)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;

    uart_ctx->sleep_out = HSUART_TRUE;
    up(&(uart_ctx->sleep_sema));

    return OK;
}



/*****************************************************************************
* 函 数 名  : hsuart_drv_uninit
*
* 功能描述  : uart 去初始化函数
*
* 输入参数  : uart_dev 设备指针
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_drv_uninit(hsuart_ctx *uart_ctx)
{
//    u32 val = 0;

    if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return;
	}
    /*disable rx and tx fifo, mask all interrupt*/
    (void)writel(UART_FCR_FIFO_DISABLE, uart_ctx->dev.vir_addr_base + UART_REGOFF_FCR);
	(void)writel(UART_IER_IRQ_DISABLE, uart_ctx->dev.vir_addr_base + UART_REGOFF_IER);
    /* 禁止 FIFO和中断 */
    free_irq((unsigned int)uart_ctx->dev.irq, NULL);
}

/*****************************************************************************
* 函 数 名  : hsuart_write_async
*
* 功能描述  : UART UDI 设备异步写实现
*
* 输入参数  : uart_ctx: 上下文
*             pWRInfo: 内存信息
* 输出参数  : NA
*
* 返 回 值  : 请求的 成功/失败
*
*****************************************************************************/
BSP_S32 hsuart_write_async(hsuart_ctx* uart_ctx, hsuart_wr_async_info* pWRInfo)
{
    struct sk_buff* tx_skb;
    uart_mem_node* pMemNode;
    
    if (NULL == uart_ctx || NULL == pWRInfo)
    {
        hsuart_error("uart_ctx:0x%x   pWRInfo:0x%x\n", (unsigned int)uart_ctx, (unsigned int)pWRInfo);
        return ERROR;
    }

    tx_skb = (struct sk_buff*)pWRInfo->pBuffer;
#if(FEATURE_ON == FEATURE_TTFMEM_CACHE)       
    __dma_single_cpu_to_dev_nocheck(tx_skb->data, tx_skb->len, DMA_TO_DEVICE);
#endif

    pMemNode = get_mem_from_freelist(&(uart_ctx->write_info));
    if (pMemNode)
    {
        pMemNode->pSkb = tx_skb;
        pMemNode->valid_size = pMemNode->pSkb->len;
        pMemNode->mem.vaddr = (void*)pMemNode->pSkb->data;

        pMemNode->mem.paddr = dma_map_single(NULL, pMemNode->mem.vaddr, pMemNode->valid_size, DMA_TO_DEVICE);
	   	hsuart_error("pMemNode_mem.vaddr:0x%x \n", pMemNode->mem.vaddr);

        add_mem_to_donelist(&(uart_ctx->write_info), pMemNode);
    }
    else
    {
        hsuart_error("no free node,   tx_skb:0x%x.\n", (unsigned int)tx_skb);
        return ERROR;
    }
    
    up(&(uart_ctx->tx_sema));
    
    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_get_read_buffer
*
* 功能描述  : 上层获取接收buf 的信息
*
* 输入参数  :  uart_ctx 上下文, 
* 输出参数  :  pWRInfo 读内存信息
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_get_read_buffer(hsuart_ctx* uart_ctx, hsuart_wr_async_info* pWRInfo)
{
    uart_mem_node* pMemNode = NULL;
#ifdef HSUART_2_LINE
#else
    unsigned long flags = 0;
#endif
    
    if (NULL == uart_ctx || NULL == pWRInfo)
    {
        hsuart_error("uart_ctx:0x%x   pWRInfo:0x%x.\n", (unsigned int)uart_ctx, (unsigned int)pWRInfo);
        return ERROR;
    }
    
    /* 获取完成的 buffer节点 */
    pMemNode = get_mem_from_donelist(&(uart_ctx->read_info));
    if (pMemNode)
    {
        /*数据所指向的数据*/
        pWRInfo->pBuffer = (char *)pMemNode->pSkb;
        /*数据的大小*/
        pWRInfo->u32Size = (u32)pMemNode->pSkb->len;
        /*当前信息头指针*/
        pWRInfo->pDrvPriv = (void*)pMemNode->pSkb;
        
        add_mem_to_freelist(&(uart_ctx->read_info), pMemNode);

        (void)hsuart_alloc_skb(pMemNode, uart_ctx->read_info.node_size);
#ifdef HSUART_2_LINE
#else
        /*非上层起流控*/
        if (CPU_TRANSFER == uart_ctx->transfer_mode)
        {
            if (uart_ctx->feature.rtsen && (!uart_ctx->rts))
            {
                spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
//                hsuart_clear_rts(&uart_ctx->dev);
                spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
                hsuart_error("rts invalued\n");
            }
        }
#endif
    }
    else
    {
        hsuart_error("no done mem node in list\n");
        return ERROR;
    }

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_return_read_buffer
*
* 功能描述  : 上层释放接收Buffer 的接口
*
* 输入参数  : uart_ctx 上下文, pWRInfo 内存信息
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_return_read_buffer(hsuart_ctx* uart_ctx, hsuart_wr_async_info* pWRInfo)
{
	if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    if(NULL == pWRInfo)
	{
       hsuart_error("pWRInfo is NULL\n");
	   return ERROR;
	}
    kfree_skb((struct sk_buff *)pWRInfo->pBuffer);

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_modem_write_signal
*
* 功能描述  :写modem 管脚信号
*
* 输入参数  :uart_dev 设备结构体pModemMsc 管脚信息
* 输出参数  :
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_modem_write_signal(hsuart_ctx *uart_ctx, MODEM_MSC_STRU* pModemMsc)
{    
	if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
#ifdef HSUART_2_LINE
#else
    /*处理DCE rts 管脚*/
    if (uart_ctx->feature.rtsen)
    {
        if (SIGNALCH == pModemMsc->OP_Cts)
        {
            if (LOWLEVEL == pModemMsc->ucCts)
            {
                /*拉低rts 管脚*/
                uart_ctx->rts = HSUART_TRUE;
                //hsuart_set_rts(&uart_ctx->dev);
                hsuart_error("rts valued:%d\n", pModemMsc->ucCts);
            }
            else
            {
                /*拉高rts 管脚*/
                uart_ctx->rts = HSUART_FALSE;
//                hsuart_clear_rts(&uart_ctx->dev);
                hsuart_error("rts invalued:%d\n", pModemMsc->ucCts);
            }
        }
    }

#ifdef HSUART_4_LINE
#else
    if (SIGNALCH == pModemMsc->OP_Ri)
    {
        gpio_direction_output(HSUART_RING, !pModemMsc->ucRi); 
        hsuart_error("Ring change:%d\n", !pModemMsc->ucRi);
    }

    /*DSR SIGNAL CHANGE*/
    if (SIGNALCH == pModemMsc->OP_Dsr)
    {
        gpio_direction_output(HSUART_DSR, !pModemMsc->ucDsr); 
        hsuart_error("dsr change:%d\n", !pModemMsc->ucDsr);
    }

    /* DCD SIGNAL CHANGE*/
    if (SIGNALCH == pModemMsc->OP_Dcd)
    {
        gpio_direction_output(HSUART_DCD, !pModemMsc->ucDcd); 
        hsuart_error("dcd change:%d\n", !pModemMsc->ucDcd);
    }
#endif
#endif

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_realloc_read_buf
*
* 功能描述  : 重新分配读缓存
*
* 输入参数  : uart_ctx 上下文pReadBuffInfo 内存信息
* 输出参数  :

*
* 返 回 值  :失败/成功
*
*****************************************************************************/
s32 hsuart_realloc_read_buf(hsuart_ctx* uart_ctx, hsuart_read_buff_info* pReadBuffInfo)
{
    s32 ret = OK;
    unsigned long flags;

    if (NULL == uart_ctx || NULL == pReadBuffInfo)
    {
        hsuart_error("uart_ctx:0x%x   pReadBuffInfo:0x%x.\n", (unsigned int)uart_ctx, (unsigned int)pReadBuffInfo);
        return ERROR;
    }
    
    if ((uart_ctx->read_info.node_num != pReadBuffInfo->u32BuffNum) || (uart_ctx->read_info.node_size != pReadBuffInfo->u32BuffSize))
    {
        spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
        hsuart_uninit_list(&(uart_ctx->read_info));
        uart_ctx->read_info.node_num = pReadBuffInfo->u32BuffNum;
        uart_ctx->read_info.node_size = pReadBuffInfo->u32BuffSize;
        if (hsuart_init_list(&(uart_ctx->read_info)))
        {
            hsuart_error("hsuart_init_rw_list error.\n");
            ret = ERROR;
        }
        spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
    }

    return ret;
}

/*****************************************************************************
* 函 数 名  : hsuart_mode_switch_config
*
* 功能描述  : 模式切换设置
*
* 输入参数  : uart_ctx 上下文s32Arg 配置参数
* 输出参数  :

*
* 返 回 值  :NA
*
*****************************************************************************/
void hsuart_mode_switch_config(hsuart_ctx* uart_ctx, void* s32Arg)
{
    unsigned long flags;
    
    spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
    if (NULL == s32Arg)
    {
        /*第一和第三阶段起的Softtimer 都删除*/
//        check_delete_softtimer(&uart_ctx->switch_timer);
        /* 检测阶段为0）*/
        uart_ctx->stage = NO_DETECT_STAGE;
//        uart_ctx->cbs.switch_mode_cb= NULL;
        hsuart_error("detect mode switch end.\n");
    }
    else
    {
        hsuart_error("detect mode switch start.\n");
        /*当前已为数据模式，打开检测（检测阶段为1）*/
        uart_ctx->stage = DETECT_STAGE_1;
//        uart_ctx->cbs.switch_mode_cb = (uart_switch_mode_cb_t)s32Arg;
       // check_create_softtimer(&uart_ctx->switch_timer, SWITCH_TIMER_LENGTH, SOFTTIMER_NO_LOOP,
        //               (SOFTTIMER_FUNC)switch_stage_two, (UINT32)uart_ctx);
    }
    spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
}

/*****************************************************************************
* 函 数 名  : hsuart_write_uart_cr
*
* 功能描述  : 写uart_cr寄存器
*
* 输入参数  : uart_ctx 上下文s32Arg 配置参数
* 输出参数  :

*
* 返 回 值  :NA
*
*****************************************************************************/
void hsuart_write_uart_cr(hsuart_ctx* uart_ctx, u32 value)
{
    hsuart_disable(&uart_ctx->dev);
#if 0
    /*等待当前数据发送或接收结束*/
    val = readl(uart_ctx->dev.vir_addr_base + UART011_LCRH);
    val &= (~UART01x_LCRH_FEN);
    (void)writel(val, uart_ctx->dev.vir_addr_base + UART011_LCRH);
#endif
    (void)writel(value, uart_ctx->dev.vir_addr_base + UART_REGOFF_FCR);
    hsuart_enable(&uart_ctx->dev);
}

/*****************************************************************************
* 函 数 名  : hsuart_set_flow_ctrl
*
* 功能描述  : uart流控控制
*
* 输入参数  : uart_ctx 上下文s32Arg 配置参数
* 输出参数  :

*
* 返 回 值  :NA
*
*****************************************************************************/
void hsuart_set_flow_ctrl(hsuart_ctx* uart_ctx, hsuart_flow_ctrl_union* pParam)
{
//    u32 val = 0;
    //u32 mask = UART011_CR_CTSEN | UART011_CR_RTSEN;
    unsigned long flags;
    
    spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);

   // val = readl(uart_ctx->dev.vir_addr_base + UART011_CR);
    //val &= pParam->value | (~mask);
    //val |= pParam->value & mask;
    //hsuart_write_uart_cr(uart_ctx, val);

    uart_ctx->feature.rtsen = pParam->reg.rtsen;
    uart_ctx->feature.ctsen = pParam->reg.ctsen;

    spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
}

/*****************************************************************************
* 函 数 名  : hsuart_udi_open
*
* 功能描述  : 打开uart udi设备
*
* 输入参数  : param 参数, handle 句柄
* 输出参数  :
*
* 返 回 值  : 打开结果
*
*****************************************************************************/
BSP_S32 hsuart_udi_open(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;

    uart_ctx->open = HSUART_TRUE;

    (BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)uart_ctx);

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_udi_close
*
* 功能描述  : 关闭uart udi设备
*
* 输入参数  : handle
* 输出参数  :
*
* 返 回 值  :失败/成功
*
*****************************************************************************/
BSP_S32 hsuart_udi_close(BSP_S32 s32UartDevCtx)
{
    hsuart_ctx *pUartDevCtx = (hsuart_ctx *)s32UartDevCtx;
    
    pUartDevCtx->open = HSUART_FALSE;

    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_udi_write
*
* 功能描述  : uart校准同步发送接口
*
* 输入参数  : BSP_S32 s32UartDev, BSP_U8* pBuf, u32 u32Size
* 输出参数  : 实际写入的字数

*
* 返 回 值  :失败/实际的数字
*
*****************************************************************************/
BSP_S32 hsuart_udi_write(VOID* pPrivate, void* pMemObj, u32 u32Size)
{
    hsuart_ctx *uart_ctx = (hsuart_ctx *)pPrivate;
    BSP_S32 ret = 0;
    BSP_S32 num = 0;

    if (NULL == uart_ctx || NULL == pMemObj || 0 == u32Size)
    {
        hsuart_error("Invalid params.\n");
        return ERROR;
    }
    if (!uart_ctx->open)
    {
        hsuart_error("uart is not open.\n");
        return ERROR;
    }
    
    uart_ctx->stat.syn_write_cnt++;

    num = get_list_node_num(&uart_ctx->write_info.done_list);
    hsuart_error("start addr:0x%x  len:%d  done list num:%d.\n", (unsigned int)pMemObj, u32Size, num);

    while (num)
    {
        msleep(1);
        num = get_list_node_num(&uart_ctx->write_info.done_list);
    }

    ret = hsuart_tx_poll(&uart_ctx->dev, (unsigned char *)pMemObj, u32Size);
   
    return OK;
}

/*****************************************************************************
* 函 数 名  : hsuart_udi_ioctl
*
* 功能描述  : UART 拨号业务相关 ioctl 设置
*
* 输入参数  : s32UartDevId: 设备管理结构指针
*             s32Cmd: 命令码
*             pParam: 命令参数
* 输出参数  :
*
* 返 回 值  : 成功/失败错误码
*
*****************************************************************************/
BSP_S32 hsuart_udi_ioctl(VOID* pPrivate, u32 u32Cmd, VOID* pParam)
{
    hsuart_ctx* uart_ctx = (hsuart_ctx*)pPrivate;
    hsuart_dev *uart_dev = NULL;
    BSP_S32 ret = OK;
    unsigned long flags = 0;

	if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    hsuart_error("enter. cmd:0x%x  pParam:0x%x\n", u32Cmd, (unsigned int)pParam);
    
    if (!uart_ctx->open)
    {
        hsuart_error("uart is not open.\n");
        return ERROR;
    }
    
    uart_dev = &uart_ctx->dev;
    switch(u32Cmd)
    {
        /* 下行异步写接口*/
        case UART_IOCTL_WRITE_ASYNC:
            {
                ret = hsuart_write_async(uart_ctx, (hsuart_wr_async_info *)pParam);
            }
            break;
        /* NAS注册下行数据buf 释放函数*/
        case UART_IOCTL_SET_FREE_CB:
            {
                uart_ctx->cbs.free_cb = (hsuart_free_cb_t)pParam;
            }
            break;
        /* 收到数据后调用此注册接口通知上层接收*/
        case UART_IOCTL_SET_READ_CB:
            {
                uart_ctx->cbs.read_cb = (hsuart_read_cb_t)pParam;
            }
            break;
         /* 上层调用此接口获得上行buf 地址，上层在我们的read回调函数中来实现的*/
        case UART_IOCTL_GET_RD_BUFF:
            {               
                ret = hsuart_get_read_buffer(uart_ctx, (hsuart_wr_async_info*)pParam);
            }
            break;
        /* 上层调用此接口释放上行buf */
        case UART_IOCTL_RETURN_BUFF:
            {
                ret = hsuart_return_read_buffer(uart_ctx, (hsuart_wr_async_info*)pParam);
            }
            break;
        /* 重新分配单个buffer大小命令*/
        case UART_IOCTL_RELLOC_READ_BUFF:
            {
                ret = hsuart_realloc_read_buf(uart_ctx, (hsuart_read_buff_info*)pParam);
            }
            break;
        /*注册DTR管脚变化通知回调参数为pMODEM_MSC_STRU 指向本地全局*/
        case UART_IOCTL_SET_MSC_READ_CB:
            {
                uart_ctx->cbs.msc_read_cb = (hsuart_msc_read_cb_t)pParam;
            }
            break;
        /*写modem管脚信号，参数为pMODEM_MSC_STRU*/
        case UART_IOCTL_MSC_WRITE_CMD:
            {
                ret = hsuart_modem_write_signal(uart_ctx, (MODEM_MSC_STRU*)(pParam));
            }
            break;
        /* +++切换命令模式回调，设置NULL关闭该功能 */
        case UART_IOCTL_SWITCH_MODE_CB:
            {
                hsuart_mode_switch_config(uart_ctx, pParam);
            }
            break;            
        /*setting uart baud rate*/
        case UART_IOCTL_SET_BAUD:
            {
                spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
                uart_ctx->cbs.set_baud_cb = hsuart_set_baud;
                uart_ctx->feature.baud = *(int*)pParam;
                up(&(uart_ctx->tx_sema));
                hsuart_error("set baud cb: %d\n", *(int*)pParam);
                spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
            }
            break;
        /*setting uart word length*/
        case UART_IOCTL_SET_WLEN:
            {
                spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
                uart_ctx->cbs.set_wlen_cb = hsuart_set_wlen;
                uart_ctx->feature.wlen = *(int*)pParam;
                up(&(uart_ctx->tx_sema));
                hsuart_error("set wlen cb: %d\n", *(int*)pParam);
                spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
            }
            break;
        /*turn on/off uart 2bit stop bits*/
        case UART_IOCTL_SET_STP2:
            {
                spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);           
                uart_ctx->cbs.set_stp2_cb = hsuart_set_stp2;
                uart_ctx->feature.stp2 = *(int*)pParam;
                up(&(uart_ctx->tx_sema));
                hsuart_error("set stp2 cb: %d\n", *(int*)pParam);             
                spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
            }
            break;
        /*setting uart parity check type*/
        case UART_IOCTL_SET_EPS:
            {
                spin_lock_irqsave(&(uart_ctx->dev.lock_irq), flags);
                uart_ctx->cbs.set_eps_cb = hsuart_set_eps;
                uart_ctx->feature.eps = *(int*)pParam;
                up(&(uart_ctx->tx_sema));
                hsuart_error("set eps cb: %d\n", *(int*)pParam);
                spin_unlock_irqrestore(&(uart_ctx->dev.lock_irq), flags);
            }
            break;
        /*set flow control */
        case UART_IOCTL_SET_FLOW_CONTROL:
            {
                hsuart_set_flow_ctrl(uart_ctx, (hsuart_flow_ctrl_union*)pParam);
            }
            break;
        /*set A/C shell */
        case UART_IOCTL_SET_AC_SHELL:
            {
#ifndef _DRV_LLT_
//                *(volatile unsigned int*)MEMORY_AXI_HSUART_INOUT_ADDR = *(volatile unsigned int*)pParam;
#endif
            }
            break;
        default:
            hsuart_error("unknow s32Cmd: 0x%x.\n", u32Cmd);
            ret = ERROR;
            break;
    }

    return ret;
}

/*****************************************************************************
* 函 数 名  : hsuart_timer_init
*
* 功能描述  : 
*
* 输入参数  : uart_ctx 上下文
* 输出参数  : NA
*
* 返 回 值  : 成功/失败bcmsdh_register_oob_intr
*
*****************************************************************************/
int hsuart_timer_init(hsuart_ctx *uart_ctx)
{
	struct softtimer_list *dma_sleep_timer = &g_hsuart_ctrl.sleep_timer;
#if 1
	struct softtimer_list *stateone_timer  = &g_hsuart_ctrl.switch_state1_timer;
	struct softtimer_list *statetwo_timer  = &g_hsuart_ctrl.switch_state2_timer;
	struct softtimer_list *statethree_timer = &g_hsuart_ctrl.switch_state3_timer;
	struct softtimer_list *baud_adapt_timer = &g_hsuart_ctrl.adapt_timer;
#endif	
	dma_sleep_timer->func = hsuart_sleep_cb;
	dma_sleep_timer->para = (u32)&m2m_hsuart_ctx;
	dma_sleep_timer->timeout = uart_ctx->dma_info.burst_delay;
	dma_sleep_timer->wake_type = SOFTTIMER_NOWAKE;
#if 1
	stateone_timer->func = switch_stage_two;
	stateone_timer->para = (u32)&m2m_hsuart_ctx;
	stateone_timer->timeout = 500;
	stateone_timer->wake_type = SOFTTIMER_NOWAKE;

	statetwo_timer->func = switch_stage_three;
	statetwo_timer->para = (u32)&m2m_hsuart_ctx;
	statetwo_timer->timeout = 1000;
	statetwo_timer->wake_type = SOFTTIMER_NOWAKE;

	statethree_timer->func = switch_stage_one;
	statethree_timer->para = (u32)&m2m_hsuart_ctx;
	statethree_timer->timeout = 1000;
	statethree_timer->wake_type = SOFTTIMER_NOWAKE;

	baud_adapt_timer->func = set_next_adapt;
	baud_adapt_timer->para = (u32)&m2m_hsuart_ctx;
	baud_adapt_timer->timeout = 1000;
	baud_adapt_timer->wake_type = SOFTTIMER_NOWAKE;


	if(OK != bsp_softtimer_create(dma_sleep_timer))
	{
		hsuart_error("sleep_timer create fail...\n");
		goto free_SleepTimer;
	}

	
	if(OK != bsp_softtimer_create(stateone_timer))
	{
		hsuart_error("stateone_timer create fail...\n");
		goto fail_onetimer;
	}

	if(OK != bsp_softtimer_create(statetwo_timer))
	{
		hsuart_error("statetwo_timer create fail...\n");
		goto fail_twotimer;
	}

	if(OK != bsp_softtimer_create(statethree_timer))
	{
		hsuart_error("statethree_timer create fail...\n");
		goto fail_threetimer;
	}

	if(OK != bsp_softtimer_create(baud_adapt_timer))
	{
		hsuart_error("baud_adapt_timer create fail...\n");
		goto fail_adapt_timer;
	}
return 0;
fail_adapt_timer:
	bsp_softtimer_free(baud_adapt_timer);
fail_threetimer:
	bsp_softtimer_free(statethree_timer);
fail_twotimer:
	bsp_softtimer_free(statetwo_timer);
fail_onetimer:	
	bsp_softtimer_free(stateone_timer);
free_SleepTimer:
	bsp_softtimer_free(dma_sleep_timer);
#endif
	return -1;	
}
/*****************************************************************************
* 函 数 名  : hsuart_drv_init
*
* 功能描述  : uart 底层初始化函数
*
* 输入参数  : uart_ctx:uart上下文
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
s32 hsuart_drv_init(hsuart_ctx *uart_ctx)
{
    s32 ret = ERROR;
    hsuart_dev *uart_dev = NULL;

  	hsuart_error("hsuart_drv_init enter...!\n");

    if(NULL == uart_ctx)
	{
       hsuart_error("uart_ctx is NULL\n");
	   return ERROR;
	}
    uart_dev = &uart_ctx->dev;
    spin_lock_init(&uart_dev->lock_irq);
    uart_dev->vir_addr_base = ioremap(uart_dev->phy_addr_base,SIZE_4K);
    hsuart_error("uart_dev irq is %d, phy_addr_base is %x, vir_addr_base is %x.\n", uart_dev->irq, uart_dev->phy_addr_base, (unsigned int)uart_dev->vir_addr_base);

	memset(&uart_ctx->cbs, 0, sizeof(hsuart_udi_cbs));
    memset(&uart_ctx->stat, 0, sizeof(hsuart_stat));
	
	uart_ctx->open = HSUART_TRUE;
    uart_ctx->baud_adapt = HSUART_FALSE;
    uart_ctx->baud_idx = 0;
    uart_ctx->stage = NO_DETECT_STAGE;	
    uart_ctx->rts = HSUART_FALSE;
    uart_ctx->tx_running = HSUART_FALSE;
    uart_ctx->sleep_out = HSUART_FALSE;
    uart_ctx->feature.baud = 115200;
    uart_ctx->dma_info.rx_dma_done = HSUART_FALSE;

    /* 根据波特率获取dma延时时间 */
	hsuart_get_dma_info(uart_ctx);
	
	/* 创建信号量 */
    osl_sem_init(SEM_EMPTY,&(uart_ctx->rx_sema));
    osl_sem_init(SEM_EMPTY,&(uart_ctx->tx_sema));
    osl_sem_init(SEM_EMPTY,&(uart_ctx->dma_sema));
    osl_sem_init(SEM_EMPTY,&(uart_ctx->sleep_sema));
    
 
	/* 初始化缓存链表 */
    if (hsuart_buf_init(uart_ctx) != OK)
    {
        hsuart_error("hsuart_buf_init is error.\n");
        return ERROR;
    }
	
	if (DMA_TRANSFER == uart_ctx->transfer_mode)
	{
		writel(UART_DEF_RT_ONEFOUR|UART_DEF_DMA_MODE|UART_TX_FIFO_RESET|UART_RX_FIFO_RESET|UART_FCR_FIFO_ENABLE, uart_dev->vir_addr_base+ UART_REGOFF_FCR);
		writel(UART_IER_IRQ_DISABLE, uart_dev->vir_addr_base + UART_REGOFF_IER);
	}

	else 
	{
		/* 申请使用中断处理 */
	    ret = request_irq((unsigned int)uart_dev->irq, (irq_handler_t)hsuart_irq,0,"HS UART ISR", &uart_ctx->dev);
	    if (ret)
	    {
	        hsuart_error("request_irq is failed!\n");
	        return ERROR;
	    }
		/* 高速串口底层初始化 */
	    writel(UART_DEF_RT_ONETWO|UART_DEF_NO_DMA_MODE|UART_TX_FIFO_RESET|UART_RX_FIFO_RESET|UART_FCR_FIFO_ENABLE, uart_dev->vir_addr_base+ UART_REGOFF_FCR);

		/* 去使能所有中断 */
	    writel(UART_IER_IRQ_DISABLE, uart_dev->vir_addr_base + UART_REGOFF_IER);
		
		readl(uart_dev->vir_addr_base + UART_REGOFF_LSR);
	    /* 清除接收中断 */
	    readl(uart_dev->vir_addr_base + UART_REGOFF_RBR);
	    /* 清除发送空中断 */
	    readl(uart_dev->vir_addr_base + UART_REGOFF_IIR);
	    /* 清除线忙中断 */
	    readl(uart_dev->vir_addr_base + UART_REGOFF_USR);
	}

	/* 设置波特率 */
    hsuart_set_baud(uart_dev, uart_ctx->feature.baud);
	
	/*设置数据位长8，1个停止位，无校验*/
    writel(UART_LCR_DLS_8BITS | UART_LCR_STOP_1BITS |UART_LCR_PEN_NONE, uart_dev->vir_addr_base + UART_REGOFF_LCR);
	
	/* 配置发送深度 */
    writel(0x10, uart_dev->vir_addr_base +  0x2c);
	
	if(CPU_TRANSFER == uart_ctx->transfer_mode)
	{
		/* 使能接收和超时中断 */
		(void)writel(UART_IER_RX_IRQ_ENABLE | 0x10,  uart_dev->vir_addr_base+ UART_REGOFF_IER);
	}
	
    hsuart_gpio_init(uart_ctx);
	hsuart_timer_init(uart_ctx);
	
    return OK;
}

static UDI_DRV_INTEFACE_TABLE hsuart_udi = {
    .udi_open_cb = (UDI_OPEN_CB_T)hsuart_udi_open,
    .udi_close_cb = (UDI_CLOSE_CB_T)hsuart_udi_close,
    .udi_write_cb = (UDI_WRITE_CB_T)hsuart_udi_write,
    .udi_read_cb = (UDI_READ_CB_T)NULL,
    .udi_ioctl_cb = (UDI_IOCTL_CB_T)hsuart_udi_ioctl,
};
/*****************************************************************************
* 函 数 名  : hsuart_udi_init
*
* 功能描述  : UART UDI 初始化接口
*
* 输入参数  : NA
* 输出参数  : NA
*
* 返 回 值  : 成功/失败
*
*****************************************************************************/
int hsuart_udi_init(void)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;//获取串口属性表述信息
    
    hsuart_error("hsuart_udi_init enter...!\n");

    uart_ctx->init = HSUART_FALSE;

    /* UDI层的初始化 */
    (void)BSP_UDI_SetCapability(uart_ctx->udi_device_id, 0);
    (void)BSP_UDI_SetInterfaceTable(uart_ctx->udi_device_id, &hsuart_udi);

    /* 芯片驱动层的初始化*/
    if (OK != hsuart_drv_init(uart_ctx))
    {
        hsuart_error("hsuart_drv_init create failed!\n");
        return ERROR;
    }
	
	if(DMA_TRANSFER == uart_ctx->transfer_mode)
	{
		/*创建接收线程*/
	    if (ERROR == osl_task_init("uart_recv_thread",HSUART_RX_TASK_PRI,HSUART_RX_TASK_SIZE,(void *)hsuart_rx_thread,(void *)uart_ctx,
									&g_hsuart_ctrl.rx_task_id))
	    {
	         hsuart_error("uart rx thread create failed!\n");
	         return ERROR;
	    }
	    
	    /*创建发送线程*/
	    if (ERROR == osl_task_init("uart_send_thread",HSUART_TX_TASK_PRI,HSUART_TX_TASK_SIZE,(void *)hsuart_tx_thread,(void *)uart_ctx,
									&g_hsuart_ctrl.tx_task_id))
	    {
	        hsuart_error("uart tx thread create failed!\n");
	        return ERROR;
	    }
	}
	uart_ctx->init = HSUART_TRUE;
    return OK;
}

module_init(hsuart_udi_init);
/*****************************************************************************
* 函 数 名  : show_hsuart_info
*
* 功能描述  : UART 可维可测接口
*
* 输入参数  : NA
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void show_hsuart_info(void)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    u32 idx = 0;
    unsigned long flags = 0;
    
    hsuart_error(" enter.\n");
    
    if (uart_ctx->read_info.pcur_pos)
    {
        printk("current memnode valid size: %d \n", uart_ctx->read_info.pcur_pos->valid_size);
    }
	printk("pcur_pos: %x \n", (u32)uart_ctx->read_info.pcur_pos);
    printk("baud adapt status: %d \n", uart_ctx->baud_adapt);
    printk("tx_running: %d \n", uart_ctx->tx_running);
    printk("total_bytes: %d \n", uart_ctx->read_info.total_bytes);
    
    printk("current detect stage: %d \n", uart_ctx->stage);
    printk("current baud: %d \n", uart_ctx->feature.baud);
    printk("current wlen: %d \n", uart_ctx->feature.wlen);
    printk("current stp2: %d \n", uart_ctx->feature.stp2);
    printk("current eps: %d \n", uart_ctx->feature.eps);
    
    printk("not free write data cnt: %d \n", uart_ctx->stat.write_not_free_cnt);
    printk("lost data cnt: %d \n", uart_ctx->stat.lost_data_cnt);
    printk("synchronization write cnt: %d \n", uart_ctx->stat.syn_write_cnt);
    printk("alloc_skb_fail: %d \n", uart_ctx->stat.alloc_skb_fail);
    printk("sleep_delay_cnt: %d \n", uart_ctx->stat.sleep_delay_cnt);
#ifndef _DRV_LLT_
 //   printk("MEMORY_AXI_HSUART_INOUT_ADDR: 0x%x \n", *( volatile unsigned int* )MEMORY_AXI_HSUART_INOUT_ADDR);
#endif

    /* 模块未初始化，将导致空指针*/
    if (uart_ctx->init)
    {
        spin_lock_irqsave(&uart_ctx->read_info.list_lock, flags);
        printk("read free node num: %d \n", get_list_node_num(&uart_ctx->read_info.free_list));
        printk("read done node num: %d \n", get_list_node_num(&uart_ctx->read_info.done_list));
        printk("read node num: %d \n", uart_ctx->read_info.node_num);
        printk("read node size: %d \n", uart_ctx->read_info.node_size);
        printk("read done cnt: %d \n", uart_ctx->read_info.done_cnt);
        for (idx = 0; idx < uart_ctx->read_info.node_num; idx++)
        {
            printk("node state: %d \n", uart_ctx->read_info.pmem_start[idx].state);
            printk("node size: %d \n", uart_ctx->read_info.pmem_start[idx].valid_size);
        }
        spin_unlock_irqrestore(&uart_ctx->read_info.list_lock, flags); 
        
        spin_lock_irqsave(&uart_ctx->write_info.list_lock, flags);
        printk("write free node num: %d \n", get_list_node_num(&uart_ctx->write_info.free_list));
        printk("write done node num: %d \n", get_list_node_num(&uart_ctx->write_info.done_list));
        printk("write node num: %d \n", uart_ctx->write_info.node_num);
        printk("write node size: %d \n", uart_ctx->write_info.node_size);
        printk("write done cnt: %d \n", uart_ctx->write_info.done_cnt);
        for (idx = 0; idx < uart_ctx->write_info.node_num; idx++)
        {
            printk("node state: %d \n", uart_ctx->write_info.pmem_start[idx].state);
            printk("node size: %d \n", uart_ctx->write_info.pmem_start[idx].valid_size);
        }
        spin_unlock_irqrestore(&uart_ctx->write_info.list_lock, flags);
    }
}

void hsuart_read_reg(u32 offset)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    u32 status;
    
    status = readl(uart_ctx->dev.vir_addr_base + offset);
    hsuart_error("offset:0x%x   value:0x%x !\n", offset, status);
}

void hsuart_write_reg(u32 offset, u32 value)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;

    (void)writel(value, uart_ctx->dev.vir_addr_base + offset);
    
    hsuart_error("write offset:0x%x   value:0x%x !\n", offset, value);
}

/*****************************************************************************
* 函 数 名  : hsuart_read_cb
*
* 功能描述  : UART 回环测试回调
*
* 输入参数  : ctx 传参
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
void hsuart_read_cb(void)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    hsuart_wr_async_info WRInfo;

    hsuart_udi_ioctl(uart_ctx, UART_IOCTL_GET_RD_BUFF, &WRInfo);
    hsuart_udi_ioctl(uart_ctx, UART_IOCTL_WRITE_ASYNC, &WRInfo);
}


/*****************************************************************************
* 函 数 名  : hsuart_loop_test
*
* 功能描述  : UART 回环测试
*
* 输入参数  : ctx 传参
* 输出参数  : NA
*
* 返 回 值  : NA
*
*****************************************************************************/
#if 0
void hsuart_loop_test(BSP_BOOL val)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    static uart_read_cb_t bak;

    if (val)
    {
        bak = uart_ctx->cbs.read_cb;
        uart_ctx->cbs.read_cb = hsuart_read_cb;
    }
    else
    {
        uart_ctx->cbs.read_cb = bak;
    }
}
#endif
void show_dma_info(void)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    
    printk("burst_dma_done: %d \n", uart_ctx->stat.burst_dma_done);
    printk("single_dma_done: %d \n", uart_ctx->stat.single_dma_done);
    printk("single_time_out: %d \n", uart_ctx->stat.single_time_out);
}

void set_dma_info(u32 burst_delay, u32 single_delay, u32 single_loop)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    
    hsuart_error("burst_delay:%d ->%d   single_delay:%d ->%d   single_loop:%d -> %d !\n", uart_ctx->dma_info.burst_delay, burst_delay, uart_ctx->dma_info.single_delay, single_delay, uart_ctx->dma_info.single_loop, single_loop);

    uart_ctx->dma_info.burst_delay = burst_delay;
    uart_ctx->dma_info.single_delay = single_delay;
    uart_ctx->dma_info.single_loop = single_loop;
}
#if 0
void set_flow_ctrl(u32 rtsen, u32 ctsen)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    hsuart_flow_ctrl_union Param;

    Param.reg.rtsen = rtsen;
    Param.reg.ctsen = ctsen;
   // hsuart_udi_ioctl(uart_ctx, UART_IOCTL_SET_FLOW_CONTROL, &Param);
}
#endif

void set_baud(u32 baud)
{
    hsuart_ctx *uart_ctx = &m2m_hsuart_ctx;
    int Param;

    Param = baud;
    hsuart_udi_ioctl(uart_ctx, UART_IOCTL_SET_BAUD, &Param);
}

/*****************************************************************************
 函 数 名  : DualModem_InQue
 功能描述  : 高速串口接收缓冲区入队函数
 输入参数  : pstQue 缓冲区队列指针
             sucData 接收到的字符
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
int M2m_hsuart_InQue(m2m_recv_str *pstQue, UINT8 ucData)
{
    u32 ulTail = 0;

    if(NULL == pstQue)
    {
        return ERROR;
    }
    ulTail = pstQue->WritePtr;
    pstQue->buffer[ulTail] = ucData;
    ulTail = ((ulTail+1) ==  M2M_RECV_SIZE) ? 0 : (ulTail+1);
    pstQue->WritePtr = ulTail;
    return OK;
}

/*****************************************************************************
 函 数 名  : DualModem_InQue
 功能描述  : 高速串口接收缓冲区入队函数
 输入参数  : pstQue 缓冲区队列指针
             sucData 接收到的字符
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void m2m_hsuart(void)
{
	u32 i = 0;
	UINT8 temp = 0;
	u32 regval = 0;
	hsuart_recv_str.ReadPtr = hsuart_recv_str.WritePtr;
	printk("ReadPtr: 0x%x \n", hsuart_recv_str.ReadPtr);
	printk("WritePtr: 0x%x \n", hsuart_recv_str.WritePtr);

	for(i = 0; i < hsuart_recv_str.ReadPtr; i++)
	{
		temp = hsuart_recv_str.buffer[i];
		regval = readl(0xc2000000 + UART_REGOFF_USR);
        if (0 != (regval & 0x02))
        {
			*(UINT8 *)(0xc2000000 + UART_REGOFF_THR) = temp;
        }
	}
}

void hsuart_clear(void)
{
	hsuart_recv_str.ReadPtr = 0;
	hsuart_recv_str.WritePtr = 0;
	memset(&hsuart_recv_str.buffer,0,sizeof(hsuart_recv_str.buffer));
}

void hsuart_send_test(void)
{
	u32 temp;
	temp = 0xaaaaaaaa;
	
	printk("temp: %x \n", temp);

	//*(UINT8 *)(0xc2000000 + UART_REGOFF_THR) = temp;
	writel(temp, (0xc2000000 + UART_REGOFF_THR));

}

