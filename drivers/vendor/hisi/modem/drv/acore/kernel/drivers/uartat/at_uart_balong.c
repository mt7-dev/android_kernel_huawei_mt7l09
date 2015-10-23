/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  uartdrv_balong.c
*
*   作    者 :  wangxiandong
*
*   描    述 :  本文件命名为"uartdrv_balong.c"
*
*   修改记录 :
*************************************************************************/
/*lint --e{537}*/
#include <linux/semaphore.h>    /*创建mutex所需的头文件*/
#include <linux/kthread.h>      /*创建线程*/
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <bsp_memmap.h>
#include <osl_types.h>
#include <bsp_om.h>
#include <bsp_shared_ddr.h>
#include <soc_interrupts.h>
#include <bsp_pm.h>
#include <osl_spinlock.h>
#include "at_uart_balong.h"

/*lint --e{63, 527, 550, 681, 958} */
struct uart_at_mng g_uart_at_infor = {1, 0, {0}, {0}, NULL};

/*****************************************************************************
* 函 数 名  : uart_at_open
* 功能描述  : 打开uart设备
* 输入参数  : dev_id  设备管理结构指针
* 输出参数  : flag
            : mode
* 返 回 值  : 成功/失败
*****************************************************************************/
static s32 uart_at_open(s32 dev_id, s32 flag, s32 mode)
{
	UART_CTX_S* uart_dev_ctx = (UART_CTX_S*)dev_id;

	if (NULL == uart_dev_ctx)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AT_UART, "[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; dev_id is null\n\n", (int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	if (uart_dev_ctx->bOpen)
	{
		bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_AT_UART, "[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; UartDevCtx is opened\n\n", (int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	uart_dev_ctx->bOpen = TRUE;
	/*因为在初始化的时候已经做好了，just 获得phy层的CTX就行了*/
	if(BSP_OK != get_uart_port((u32)uart_dev_ctx))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AT_UART, "[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; uart_at_open is failed\n\n", (int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	return ((s32)uart_dev_ctx);
}

/*****************************************************************************
* 函 数 名  : udi_uart_at_open
* 功能描述  : 打开uart设备
* 输入参数  : UDI_OPEN_PARAM *param, UDI_HANDLE handle
* 输出参数  :
* 返 回 值  : 返回文件句柄
*
*****************************************************************************/
static s32 bsp_uart_at_open(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	s32 fd = 0;

	fd = uart_at_open((s32)(&g_uart_at_infor.UartCtx), 0, 0);
	if (fd == -1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AT_UART, "[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; BSP_MODU_UDI open fail\n\n", (int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	(void)BSP_UDI_SetPrivate(param->devid, (void*)fd);
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_uart_at_write
*
* 功能描述  : uart发送接口
*
* 输入参数  : s32 s32UartDev, u8* pBuf, u32 u32Size
* 输出参数  : 实际写入的字数

*
* 返 回 值  : 失败/实际的数字
*
*****************************************************************************/
s32 bsp_uart_at_write(s32 uart_dev_ctx, unsigned char * buffer, u32 size)
{
	UART_CTX_S* p_uart_dev_ctx = (UART_CTX_S*) uart_dev_ctx;
	s32 ret;

	/*回环测试*/
#ifdef MSP_IF_IOCTL_TEST
	int i = 0;
	(void)printk("\n#########DL DATA ##########\n");
	for(i = 0; i< (s32)size;i++)
	{
		(void)printk("%x ",(u8)buffer[i]);
	}
	(void)printk("\n");
#endif

	if(TRUE != p_uart_dev_ctx->bOpen )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AT_UART,
			"[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; uart at is not opend\n\n",
			(int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	down(&g_uart_at_infor.UartCtx.MSP_Tx_Sem);
	/*设置UartCtx*/
	ret = uart_send((s32)p_uart_dev_ctx,buffer,size);
	if (ret == (s32)size)
	{
		/*对于UART不需要DMA内存，这样的话，发送成功也不需要DRV释放*/
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AT_UART, "[AT UART ERROR]:\n\t\t\t <func>: %s;  <line>: %d; uart_send failed\n\n", (int)__FUNCTION__, (int)__LINE__);
		return BSP_ERROR;
	}
	up(&g_uart_at_infor.UartCtx.MSP_Tx_Sem);
	return (ret);
}/*lint !e529*/
/*****************************************************************************
* 函 数 名  : bsp_uart_at_close
* 功能描述  : 关闭uart设备
* 输入参数  : handle
* 输出参数  :
* 返 回 值  :失败/成功
*****************************************************************************/
s32 bsp_uart_at_close(s32 uart_dev_ctx)
{
	UART_CTX_S *p_uart_dev_ctx = (UART_CTX_S *)uart_dev_ctx;
	p_uart_dev_ctx->bOpen = FALSE;
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_uart_at_ioctrl
* 功能描述  : UART 业务相关 ioctl 设置
* 输入参数  : uart_dev_id: 设备管理结构指针
*             s32Cmd: 命令码
*             s32Arg: 命令参数
* 输出参数  :
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_uart_at_ioctrl(s32 uart_dev_id, s32 cmd, s32 arg)
{
	UART_CTX_S* p_uart_ctx = (UART_CTX_S*)uart_dev_id;
	s32 ret = BSP_OK;

	if (NULL == p_uart_ctx || !p_uart_ctx->bOpen)
	{
		(void)printk("THE PARAM IS ERROR\n");
		return BSP_ERROR;
	}
	switch(cmd)
	{
		/* 1注册下行的buf释放函数OK */
		case UART_IOCTL_SET_WRITE_CB:
		p_uart_ctx->MSP_WriteCb = (UART_WRITE_DONE_CB_T)arg;
		break;
		/*3MSP调用获得上行buf的地址，这是在我们的read回调函数中来调用的*/
		case UART_IOCTL_GET_RD_BUFF:
		{
			if (0 == arg)
			{
				(void)printk("write UART_IOCTL_WRITE_ASYNC invalid args\n");
				return BSP_ERROR;
			}
			ret = uart_at_get_read_buffer(uart_dev_id, (UART_WR_ASYNC_INFO*)arg);
		}
		break;
		/*4我们将上行buf地址传给MSP后，有MSP来调用此接口进行释放*/
		case UART_IOCTL_RETURN_BUFF:
		{
			if (0 == arg)
			{
				(void)printk("write UART_IOCTL_RETURN_BUFF invalid args\n");
				return BSP_ERROR;
			}
			ret = uart_at_return_read_buffer(uart_dev_id, (UART_WR_ASYNC_INFO*)arg);
		}
		break;
		/*6MSP的接收发送函数，我们收到数据后，回来调用此接口OK*/
		case UART_IOCTL_SET_READ_CB:
		{
			p_uart_ctx->MSP_ReadCb = (UART_READ_DONE_CB_T)arg;
			if(g_uart_at_infor.printkflag == 1)
			{
				(void)printk("p_uart_ctx->MSP_ReadCb ==  0X%x\n",(u32)p_uart_ctx->MSP_ReadCb);
			}
		}
		break;
		default:
		(void)printk("error cmd \n");
		break;
	}
	return ret;
}

/*****************************************************************************
* 函 数 名  : uart_at_init
* 功能描述  : UART 调用的初始化接口
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
s32  uart_at_init(void)
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	s32 ret = 0;

 	if(AT_UART_USE_FLAG != (*(u32 *)SHM_MEM_AT_FLAG_ADDR))
		return 0;

	pm_enable_wake_src(DRV_WAKE_SRC_UART0);
	spin_lock_init(&g_uart_at_infor.uart_port0.lock_irq);
	g_uart_at_infor.uart_port0.irq    = INT_LVL_UART0;
	g_uart_at_infor.uart_port0.vir_addr_base= (unsigned char *)HI_UART0_REGBASE_ADDR_VIRT;
	/*初始化互斥信号量*/
	sema_init(&(g_uart_at_infor.uart_port0.uart_rx_sem), SEM_EMPTY);
	/*创建接收线程*/
	if (!g_uart_at_infor.pstUartReadThread)
	{
		g_uart_at_infor.pstUartReadThread = kthread_run(uart_recv_thread, NULL, "UART_RX_KTHREAD");
		ret = IS_ERR(g_uart_at_infor.pstUartReadThread) ? PTR_ERR(g_uart_at_infor.pstUartReadThread) : 0;
		if (ret)
		{
			(void)printk("kthread_run is failed!\n");
			g_uart_at_infor.pstUartReadThread = NULL;
			return BSP_ERROR;
		}
	}
	if(uart_at_reg_init() != OK)
	{
		(void)printk("uart_at_reg_init is error\n");
		return BSP_ERROR;
	}
	if(uart_rx_buf_init() != OK)
	{
		(void)printk("uart_rx_buf_init is error\n");
		return BSP_ERROR;
	}
	sema_init(&(g_uart_at_infor.UartCtx.MSP_Tx_Sem), SEM_FULL);
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		(void)printk("BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return BSP_ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));
	/* 只设置需要实现的几个接口 */
	pDrvInterface->udi_open_cb = bsp_uart_at_open;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)bsp_uart_at_close;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)bsp_uart_at_write;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)bsp_uart_at_ioctrl;
	(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, 0), 0);
	(void)printk("UDI_BUILD_DEV_ID is 0x%x\n",UDI_BUILD_DEV_ID(UDI_DEV_UART, 0));
	(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, 0), pDrvInterface);
	g_uart_at_infor.UartCtx.bOpen = FALSE;
	return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : uart_at_uninit
* 功能描述  : UART 调用的初始化接口
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
void uart_at_uninit(void)
{
	uart_rx_buf_uinit();
	uart_at_reg_uninit();
}

/*****************************************************************************
* 函 数 名  : uart_find_mem_from_queue
* 功能描述  : 找到push到MSP得接收节点
* 输入参数  : s32 uart_dev_id, u8 *buf
* 输出参数  :
* 返 回 值  :失败/成功
*****************************************************************************/
s32 uart_realloc_read_buf(UART_CTX_S* p_uart_ctx, UART_READ_BUFF_INFO* pstReadBuffInfo)
{
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : creat_queue
* 功能描述  :创建 接收缓存队列
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
s32 creat_queue(UART_RX_BUF_S *p_head,unsigned char *p_data_buf)
{
	int i = 1;
	int j = 0;
	int k = 0;
	UART_RX_BUF_S *p_temp = p_head;

	(void)printk("p_temp is 0x%x\n",(u32)p_temp);
	/*创建接收循环缓冲*/
	for(k = 0; k < (int)RX_NUM; k++)
	{
		/*分配data区域*/
		p_temp->buf = (unsigned char*)&(p_data_buf[RX_SIZE*(u32)(j++)]);
		p_temp->ulSize = 0;
		/*连接信息头*/
		p_temp->pstNext = (struct tagUART_RX_BUF_S *)(&p_head[(i++)%((int)RX_NUM)]);
		p_temp = p_temp->pstNext;
	}
	p_temp = NULL;
	if(g_uart_at_infor.printkflag == 1)
	{
		(void)printk("i is : %d\n",i);
		(void)printk("pTemp is 0x%x ?= pHead 0x%x\n",(u32)p_temp, (u32)p_head);
		(void)printk("out func creat_queue line is %d\n",__LINE__);
	}
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : uart_rx_buf_init
* 功能描述  :初始化接收缓存队列
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
s32 uart_rx_buf_init(void)
{
	/*malloc信息头空间*/
	g_uart_at_infor.UartCtx.pHead_RxQueue = (UART_RX_BUF_S*)kmalloc(sizeof(UART_RX_BUF_S)*RX_NUM, GFP_KERNEL);
	if(NULL == g_uart_at_infor.UartCtx.pHead_RxQueue)
	{
		(void)printk("kmalloc is failed\n");
		return BSP_ERROR;
	}
	/*分配数据的buf*/
	g_uart_at_infor.UartCtx.pDataBuf = (unsigned char *)kmalloc(RX_SIZE * RX_NUM ,GFP_KERNEL);
	if(NULL == g_uart_at_infor.UartCtx.pDataBuf)
	{
		(void)printk("kmalloc is failed\n");
		(void)kfree(g_uart_at_infor.UartCtx.pHead_RxQueue);
		g_uart_at_infor.UartCtx.pHead_RxQueue = NULL;
		return BSP_ERROR;
    }
	/*创建缓冲buf*/
	if (creat_queue(g_uart_at_infor.UartCtx.pHead_RxQueue,g_uart_at_infor.UartCtx.pDataBuf) != BSP_OK)
	{
		(void)printk("craet_queue is failed\n");
		(void)kfree(g_uart_at_infor.UartCtx.pHead_RxQueue);
		(void)kfree(g_uart_at_infor.UartCtx.pDataBuf);
		g_uart_at_infor.UartCtx.pHead_RxQueue = NULL;
		g_uart_at_infor.UartCtx.pDataBuf      = NULL;
		return BSP_ERROR;
	}
	/*设置当前读写指针指向*/
	g_uart_at_infor.UartCtx.pstCurrRead  = g_uart_at_infor.UartCtx.pHead_RxQueue;
	g_uart_at_infor.UartCtx.pstCurrWrite = g_uart_at_infor.UartCtx.pHead_RxQueue;
	return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : get_uart_port0
* 功能描述  : UART 调用的初始化接口
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR/OK
*****************************************************************************/
void uart_rx_buf_uinit(void)
{
	/*释放内存*/
	(void)kfree(g_uart_at_infor.UartCtx.pHead_RxQueue);
	(void)kfree(g_uart_at_infor.UartCtx.pDataBuf);
	g_uart_at_infor.UartCtx.pHead_RxQueue = NULL;
	g_uart_at_infor.UartCtx.pDataBuf      = NULL;
	g_uart_at_infor.UartCtx.pstCurrRead   = NULL;
	g_uart_at_infor.UartCtx.pstCurrWrite  = NULL;
	return;
}

/*****************************************************************************
* 函 数 名  : get_uart_port0
* 功能描述  : UART 调用的初始化接口
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR/OK
*****************************************************************************/
s32 get_uart_port(u32 uart_dev_id)
{
	UART_CTX_S *p_uart_dev_ctx = (UART_CTX_S *) uart_dev_id;

	if(NULL == p_uart_dev_ctx)
	{
		(void)printk("the p_uart_dev_ctx is NULL line is %d\n",__LINE__);
		 return BSP_ERROR;
	}
	p_uart_dev_ctx->port = (UART_PORT_S*)(&g_uart_at_infor.uart_port0);
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : uart_at_return_read_buffer
* 功能描述  : MSP释放RX buf的接口
* 输入参数  : s32 uart_dev_id, ACM_WR_ASYNC_INFO* pWRInfo
* 输出参数  :
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 uart_at_return_read_buffer(s32 uart_dev_id, ACM_WR_ASYNC_INFO* p_wr_info)
{
	UART_CTX_S* p_uart_ctx = (UART_CTX_S*)uart_dev_id;
	UART_RX_BUF_S* pMemInfo = NULL;

	if (NULL == p_uart_ctx || !p_uart_ctx->bOpen)
	{
		(void)printk("p_uart_ctx & p_uart_ctx->bOpen is error\n");
		return BSP_ERROR;
	}
	pMemInfo = p_wr_info->pDrvPriv;
	pMemInfo->ulSize = 0;/*这里算是归还内存*/
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : uart_at_get_read_buffer
* 功能描述  : MSP 获取接收buf的信息
* 输入参数  : s32 uart_dev_id, ACM_WR_ASYNC_INFO* pWRInfo
* 输出参数  :
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 uart_at_get_read_buffer(s32 uart_dev_id, ACM_WR_ASYNC_INFO* p_wr_info)
{
	UART_CTX_S* p_uart_ctx = (UART_CTX_S*)uart_dev_id;
	UART_RX_BUF_S* pMemInfo = NULL;
	if (NULL == p_uart_ctx || !p_uart_ctx->bOpen)
	{
		(void)printk("p_uart_ctx & p_uart_ctx->bOpen is error\n");
		return BSP_ERROR;
	}
	/* 获取完成的 buffer节点 */
	pMemInfo = p_uart_ctx->pstCurrRead;
	p_wr_info->pBuffer = (BSP_CHAR *)pMemInfo->buf; /*数据所指向的数据*/
	p_wr_info->u32Size = (u32)pMemInfo->ulSize; /*数据的大小*/
	p_wr_info->pDrvPriv = (void*)pMemInfo;          /*当前信息头指针*/
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : uart_at_rx_chars
* 功能描述  : UART底层接收函数
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
static void uart_at_rx_chars(void)
{
	u32 status;
	u8 ch;
	s8 max_count = RX_SIZE;/*一个数据块的大小*/
	u8 *writebuf = g_uart_at_infor.UartCtx.pstCurrWrite->buf;

	status = readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LSR);
	while ((max_count > 0) && (status & 0x01))
	{
		ch = (u8)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_RBR);
		*writebuf++ = ch;
		/*UartCtx.pstCurrWrite->ulSize初始值为0*/
		g_uart_at_infor.UartCtx.pstCurrWrite->ulSize++;
		status = readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LSR);
		max_count--;
	}
	if((g_uart_at_infor.UartCtx.pstCurrWrite->pstNext == g_uart_at_infor.UartCtx.pstCurrRead) ||
		(g_uart_at_infor.UartCtx.pstCurrWrite->pstNext->ulSize !=0))
	{
		(void)printk("buf is full\n");
	}
	else
	{
		g_uart_at_infor.UartCtx.pstCurrWrite = g_uart_at_infor.UartCtx.pstCurrWrite->pstNext;
	}
	up(&g_uart_at_infor.uart_port0.uart_rx_sem);
	return;
}

/*****************************************************************************
* 函 数 名  : uart_recv_thread
* 功能描述  : UART底层接收线程
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
/*递交给上层之后置buf为空这是MSP的事情*/
static int uart_recv_thread(void *data)
{
    UNUSED(data);
	/* coverity[no_escape] */
	for(;;)
	{
		down(&g_uart_at_infor.uart_port0.uart_rx_sem);
		while(g_uart_at_infor.UartCtx.pstCurrRead->ulSize != 0)
		{
			if(g_uart_at_infor.UartCtx.MSP_ReadCb)
			{
#ifdef MSP_IF_IOCTL_TEST
				if(g_uart_at_infor.printkflag == 1)
				{
					ACM_WR_ASYNC_INFO pWRinfo;
					int i = 0;
					(void)printk("UartCtx.MSP_ReadCb =0X%x",g_uart_at_infor.UartCtx.MSP_ReadCb);
					(void)printk("UartCtx.pstCurrRead:%x\n",g_uart_at_infor.UartCtx.pstCurrRead);
					if(BSP_OK == uart_at_get_read_buffer((s32)(&g_uart_at_infor.UartCtx),&pWRinfo))
					{
						(void)printk("#####pWRinfo->u32Size==%d\n",pWRinfo.u32Size);
						for(i = 0;i < (s32)pWRinfo.u32Size; i++)
						{
							(void)printk("%x ",(u8)(pWRinfo.pBuffer)[i]);
						}
					}
				}
#endif
				g_uart_at_infor.UartCtx.MSP_ReadCb();
				g_uart_at_infor.UartCtx.pstCurrRead->ulSize = 0;//本来是由MSP来"释放"上行内存，现在DRV将其置0
			}
			else /*一旦上面已经注册好之后，就不会走这个分支了*/
			{
				if(g_uart_at_infor.buf_look == 0)
				{
					(void)uart_send((s32)(&g_uart_at_infor.UartCtx),g_uart_at_infor.UartCtx.pstCurrRead->buf,g_uart_at_infor.UartCtx.pstCurrRead->ulSize);
				}
				g_uart_at_infor.UartCtx.pstCurrRead->ulSize = 0;
			}
			/*一个buf发送完成之后,指针移到下一个buf*/
			if(g_uart_at_infor.UartCtx.pstCurrRead != g_uart_at_infor.UartCtx.pstCurrWrite)
			{
				g_uart_at_infor.UartCtx.pstCurrRead = g_uart_at_infor.UartCtx.pstCurrRead->pstNext;
			}
			else/*就是没有数据了，那么就会跳出本层循环，等待信号量的到来*/
			{
				(void)printk("data is empty\n");
				break;
			}
		}
	}
    return 0;
}/*lint !e529*/

/*****************************************************************************
* 函 数 名  : uart_int
* 功能描述  : uart 中断服务程序
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
static irqreturn_t uart_int(void)
{
	u32 interrupt_id;
	u32 status;
	s32 handled = 0;
	unsigned long irq_flag = 0;

	spin_lock_irqsave(&(g_uart_at_infor.uart_port0.lock_irq), irq_flag);
    status = readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_IIR);
	interrupt_id = 0x0F&status;
	while ((interrupt_id != DW_NOINTERRUPT))
	{
		/*接收数据有效*/
		if ((DW_RECEIVEAVA == interrupt_id) ||
			(DW_RECTIMEOUT == interrupt_id))    /*timeout interrupt*/
		{
			uart_at_rx_chars();
		}
		else
		{
			(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LSR);
			(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_USR);
			handled = 1;
			break;
		}
		status = readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_IIR);
		interrupt_id = 0x0F&status;
		handled = 1;
	}
	spin_unlock_irqrestore(&(g_uart_at_infor.uart_port0.lock_irq), irq_flag);
	return IRQ_RETVAL(handled);/*lint !e64*/
}
/*****************************************************************************
* 函 数 名  : uart_at_tx_chars
* 功能描述  : uart 底层发送函数
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
static int uart_at_tx_chars(u8 *p_uart_tx_buf,u32 size)
{
	s32 Cnt = (s32)size;
	u32 UsrValue = 0;

	/*这样做足可以保证能够将MSP的数据发送到FIFO里面去*/
	do
	{
		UsrValue = readl(g_uart_at_infor.uart_port0.vir_addr_base + DW_UART_TFL);
		if (UsrValue < FIFO_TX_SIZE)
		{
			writel(*p_uart_tx_buf, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_THR);
			p_uart_tx_buf++;
#ifdef MSP_IF_IOCTL_TEST
			if(g_uart_at_infor.printkflag == 1)
			{
				(void)printk("%x ",(u8)(*(p_uart_tx_buf-1)));
			}
#endif
			Cnt--;
		}
	}while (Cnt > 0);
	return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : uart_send
* 功能描述  : uart 底层发送函数
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
s32 uart_send(s32 s32uart_dev_id, u8 *pbuf, u32 size)
{
	UART_CTX_S *p_uart_ctx = (UART_CTX_S *)s32uart_dev_id;
	u8 *p_uart_tx_buf  = (u8 *)pbuf;

	if (NULL == p_uart_tx_buf || NULL == p_uart_ctx)
	{
		(void)printk("the parm is error\n");
		return BSP_ERROR;
	}
	if(uart_at_tx_chars(p_uart_tx_buf,size)!= BSP_OK)
	{
		(void)printk("uart_at_tx_chars is FAILED,line is %d\n",__LINE__);
		return BSP_ERROR;
	}
	return ((s32)size);
}
/*****************************************************************************
* 函 数 名  : uart_at_reg_init
* 功能描述  : uart 底层初始化函数
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
s32 uart_at_reg_init(void)
{
	u32 lcr_reg   = 0;
	u32 lcr_value = 0;
	u32 enbaud    = 0;
	s32 ret       = 0;

	/* 使能 DLL 和 DLH 的写入口*/
	enbaud = enbaud | DW_UART_DLAB;
	(void)writel(enbaud,g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LCR);
	/* 设置波特率 */
	(void)writel( DLL_BUAD_PARA, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_DLL);
	(void)writel( DLH_BUAD_PARA, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_DLH);
	/*关闭DLL和DLH的入口*/
	enbaud &= ~DW_UART_DLAB;/* [false alarm]:误报 */
	(void)writel(enbaud, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LCR);
	/*LCR设置****数据长度8位+   停止2位  +  校验使能 + 偶校验*/
	lcr_reg = lcr_reg | DW_UART_8bit;/* [false alarm]:误报 */
	(void)writel(lcr_reg, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LCR);
	lcr_value = readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LCR);
	(void)printk("the lcr_reg is %x\n",lcr_value);
	/* 禁止 FIFO和中断 */
	(void)writel(0,g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_IER);
	/* enable FIFO */
	(void)writel(0x1, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_FCR);
	/*set fifo trigger level*/
	(void)writel(0xb1,g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_FCR);
	/*clear and reset fifo*/
	(void)writel(0xb7,g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_FCR);
	/* clear line interrupt status */
	(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LSR);
	/*clear rcv interrupt*/
	(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_RBR);
	/*clear iir reg*/
	(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_IIR);
	/*clear line busy interrupt*/
	(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_USR);
	/*申请使用中断处理*/
	ret = request_irq((unsigned int)g_uart_at_infor.uart_port0.irq, (irq_handler_t)uart_int, 0, "BalongV?R? ISR", NULL);
	if (ret)
	{
		(void)printk("request_irq is failed\n");
		return (s32)BSP_ERROR;
	}
	/*使能 FIFO */
	/*这里无需设置超时中断，芯片本身就有*/
	/*(void)writel(DW_FCR_PARA, HI_UART0_REGBASE_ADDR_VIRT+ DW_UART_FCR);*/
	(void)readl(g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_LSR);
	(void)writel(ABLE_RX_IER, g_uart_at_infor.uart_port0.vir_addr_base  + UART_REGOFF_IER);
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : uart_write
* 功能描述  : uart 底层初始化函数
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_ERROR
*****************************************************************************/
void uart_at_reg_uninit(void)
{
	(void)writel(0x0, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_IER);
	(void)writel(0x0, g_uart_at_infor.uart_port0.vir_addr_base + UART_REGOFF_FCR);
	free_irq((unsigned int)(g_uart_at_infor.uart_port0.irq), NULL);
}

subsys_initcall(uart_at_init);