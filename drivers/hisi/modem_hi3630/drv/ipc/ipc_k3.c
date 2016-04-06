/*lint --e{537,701,713,718,732,746}*/
#ifdef __KERNEL__
#include <linux/platform_device.h>
#include <soc_interrupts_app.h>
#define DRIVER_NAME "v7r2_ipc_device"
#ifdef CONFIG_HISI_RPROC
#include <linux/huawei/hisi_rproc.h>
static struct notifier_block k3_ipc_block;
#endif
#elif defined(__VXWORKS__)
#include <soc_interrupts_mdm.h>
#endif
#include <osl_bio.h>
#include <osl_types.h>
#include <osl_module.h>
#include <bsp_memmap.h>
#include <bsp_om.h>
#include <bsp_ipc.h>
#include <bsp_hardtimer.h>
#include <soc_clk.h>
#include "ipc_balong.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#define IPC_INT       INT_LVL_IPCM2APP0
#define IPC_SEM       INT_LVL_IPCM2APP1
#elif defined(__VXWORKS__)
#define IPC_INT       INT_LVL_IPCM2MDM0
#define IPC_SEM       INT_LVL_IPCM2MDM1
#endif
/*lint --e{129, 63, 64, 409, 49, 52, 502} */
static struct ipc_control		ipc_ctrl = {0};
static struct ipc_debug_s		ipc_debug = {0};
#define IPC_CHECK_PARA(para,max) \
    do {\
        if (para >= max)\
        {\
            bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]Wrong para , line:%d,para = %d\n",__FUNCTION__, __LINE__,para); \
            return ERROR; \
        } \
    } while (0)
#define LPM3_TO_AP_IPC_FLAG (8<<24|9<<16|3<<8|14)
void ipc_modem_reset(void);
/*lint -save -e550*/
s32 bsp_ipc_int_enable (IPC_INT_LEV_E ulLvl)
{
    u32 u32IntMask = 0;
    unsigned long flags=0;
    IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
    /*写中断屏蔽寄存器*/
    spin_lock_irqsave(&ipc_ctrl.lock,flags);
    u32IntMask = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num)));
    u32IntMask |= (u32)1 << ulLvl;/* [false alarm]:误报 */
    writel(u32IntMask,(volatile void *)(ipc_ctrl.ipc_base+BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num)));
    spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
    return OK;
}

s32 bsp_ipc_int_disable(IPC_INT_LEV_E ulLvl)
{
	u32 u32IntMask = 0;
	unsigned long flags=0;
	IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
	/*写中断屏蔽寄存器*/
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	u32IntMask = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num)));
    u32IntMask = u32IntMask & (~((u32)1 << ulLvl));/* [false alarm]:误报 */
	writel(u32IntMask,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num)));
	spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
	return OK;
}


s32 bsp_ipc_int_connect(IPC_INT_LEV_E ulLvl, voidfuncptr routine, u32 parameter)
{
	 unsigned long flags=0;
	 IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
	 spin_lock_irqsave(&ipc_ctrl.lock,flags);
	 ipc_ctrl.ipc_int_table[ulLvl].routine = routine;
	 ipc_ctrl.ipc_int_table[ulLvl].arg = parameter;
	 spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
	 return OK;
}

 
s32 bsp_ipc_int_disconnect(IPC_INT_LEV_E ulLvl,voidfuncptr routine, u32 parameter)
{
	unsigned long flags = 0;
	IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	ipc_ctrl.ipc_int_table[ulLvl].routine = NULL;
	ipc_ctrl.ipc_int_table[ulLvl].arg = 0;
	spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
	return OK;
 }

OSL_IRQ_FUNC(irqreturn_t,ipc_int_handler,irq,dev_id)
{
	u32 i = 0;
	u32 u32IntStat = 0,begin = 0,end = 0;
	u32 u32Date = 0x1;
	u32 u32BitValue = 0;
	u32IntStat=readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_STAT(ipc_ctrl.core_num)));
	/*清中断*/
	writel(u32IntStat,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_CLR(ipc_ctrl.core_num)));
	/* 遍历32个中断 */
	for (i = 0; i < INTSRC_NUM; i++)
	{
		if(0!=i)
		{
			u32Date <<= 1;   
		} 
		u32BitValue = u32IntStat & u32Date;
		/* 如果有中断 ,则调用对应中断处理函数 */
		if (0 != u32BitValue)
		{  
			/*调用注册的中断处理函数*/
			if (NULL !=  ipc_ctrl.ipc_int_table[i].routine)
			{
				begin = bsp_get_slice_value();
				ipc_ctrl.ipc_int_table[i].routine(ipc_ctrl.ipc_int_table[i].arg);
				end = bsp_get_slice_value();
				ipc_debug.u32IntTimeDelta[i] = get_timer_slice_delta(begin,end);
			}
			else
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"BSP_DRV_IpcIntHandler:No IntConnect,ERROR!.int num =%d\n",i);
			}
			ipc_debug.u32IntHandleTimes[i]++;
		}
	}
	return IRQ_HANDLED;
}



s32 bsp_ipc_int_send(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
	unsigned long flags = 0,ret = 0;
	/*如果是modem处于复位状态，则调用AP侧IPC*/
	/*ap receive mabx 0,send mbx 13*/
	 rproc_msg_t msg[2] ;
	/*发送标志，用于LPM3上接收时解析*/
	msg[0]=(0<<24|9<<16|3<<8|14);
	if(!bsp_reset_ccpu_status_get())
	{
		if(enDstCore!=IPC_CORE_MCORE)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC," only can send to mcore\n");
			return ERROR;
		}
		msg[1]=(rproc_msg_t)(1<<(u32)ulLvl);
		ret = RPROC_ASYNC_SEND(HISI_RPROC_LPM3 , msg ,2,ASYNC_MSG, NULL, NULL);
	}
	 else
	{
	 
		IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
		IPC_CHECK_PARA(enDstCore,IPC_CORE_BUTTOM);
		/*写原始中断寄存器,产生中断*/
		spin_lock_irqsave(&ipc_ctrl.lock,flags);
		writel((u32)1 << ulLvl,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_RAW_INT(enDstCore)));
		spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
		ipc_debug.u32RecvIntCore = enDstCore;
		ipc_debug.u32IntSendTimes[enDstCore][ulLvl]++;
	}
	return OK;
}

static void  mask_int(u32 u32SignalNum)
{
	u32 u32IntMask = 0;
	unsigned long flags=0;
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	u32IntMask = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num)));
	u32IntMask = u32IntMask & (~((u32)1 << u32SignalNum)); /* [false alarm]:误报 */
	writel(u32IntMask,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num)));
	spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
}

 s32 bsp_ipc_sem_create(u32 u32SignalNum)
 {
	 IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	 if(true != ipc_ctrl.sem_exist[u32SignalNum])/*避免同一个信号量在没有删除的情况下创建多次*/
	 {
	 	osl_sem_init(SEM_EMPTY,&(ipc_ctrl.sem_ipc_task[u32SignalNum]));
		ipc_ctrl.sem_exist[u32SignalNum] = true;
	 	return OK;
	 }
	 else
	 {
	 	return OK;
	 }
	
 }
 
 s32 bsp_ipc_sem_delete(u32 u32SignalNum)
 {
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	if(false == ipc_ctrl.sem_exist[u32SignalNum] )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"semphore not exists,may be deleted already.\n");
		return ERROR;
	}
	else
	{
		if (osl_sema_delete(&(ipc_ctrl.sem_ipc_task[u32SignalNum])))
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"Delete semphore failed.\n");
			return ERROR;
		}
		ipc_ctrl.sem_exist[u32SignalNum] = false;
		return OK;
	}
 }

 int bsp_ipc_sem_take(u32 u32SignalNum, int s32timeout)
 {
	u32 u32IntMask = 0,ret = 0;    
	/*参数检查*/
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);  
	 /*将申请的信号量对应的释放中断清零*/
	writel((u32)1<<u32SignalNum, (volatile void *)(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_CLR(ipc_ctrl.core_num)));
	ret =  readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum)));
	if(0 == ret)
	{
		mask_int(u32SignalNum);
		ipc_debug.u32SemTakeTimes[u32SignalNum]++;
		ipc_debug.u32SemId = u32SignalNum;
		return OK;
	}
	else
	{
		if(false == ipc_ctrl.sem_exist[u32SignalNum])
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"need call ipc_sem_create to create this sem before call ipc_sem_take!\n");
			return ERROR;
		}
		if(0 != s32timeout)
		{
			/*使能信号量释放中断*/
			u32IntMask = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num)));
			u32IntMask = u32IntMask | ((u32)1 << u32SignalNum);/* [false alarm]:误报 */
			writel(u32IntMask,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num)));
			 if (OK != osl_sem_downtimeout(&(ipc_ctrl.sem_ipc_task[u32SignalNum]), s32timeout))  
			{
				mask_int(u32SignalNum);
				bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"semTake timeout!\n");
				ipc_debug.u32SemTakeFailTimes[u32SignalNum]++;
				return ERROR;
			}
			 else
			 {
				mask_int(u32SignalNum);
				ipc_debug.u32SemTakeTimes[u32SignalNum]++;
				ipc_debug.u32SemId = u32SignalNum;
				return OK;
			 }
		}
		else
		{
			return ERROR;
		}
	}
	
}
 
 
s32 bsp_ipc_sem_give(u32 u32SignalNum)
{
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	ipc_debug.u32SemGiveTimes[u32SignalNum]++;
	/*向信号量请求寄存器写0*/
	writel(0,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum)));
	return OK;
 }
 static s32 ffSLsb(s32 args)
 {
	 int num = 0; /*lint !e34 */
	 s32 s32ImpVal = args;
	 if(0 == args)
	 {
	 	return 0;
	}
	 for(;;)
	 {
		 num++;
		 if (0x1 == (s32ImpVal & 0x1))
		 {
			 break;
		 }
		 s32ImpVal = (s32)((u32)s32ImpVal >> 1);
	 }
	 return num;
 }

 /*****************************************************************************
 * 函 数 名      : ipc_sem_int_handler
 *
 * 功能描述  : 信号量释放中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值      : 无
 *
 * 修改记录  :  2013年1月9日 lixiaojie 
 *****************************************************************************/
OSL_IRQ_FUNC(irqreturn_t,ipc_sem_int_handler,irq,dev_id)
{
	u32 u32IntStat = 0,u32HsCtrl=0,u32SNum=0, i = 32;
	u32IntStat = readl((const volatile void *)(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_STAT(ipc_ctrl.core_num)));
	u32SNum = ffSLsb(u32IntStat);
	if( u32SNum != 0)
	{
		do
		{
			 /*如果有信号量释放中断，清除该中断*/
			writel((u32)1<<--u32SNum, (volatile void *)(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_CLR(ipc_ctrl.core_num)));
			u32HsCtrl = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SNum)));
			if (0 == u32HsCtrl)
			{
				osl_sem_up(&(ipc_ctrl.sem_ipc_task[u32SNum]));
			}
			else
			{
				ipc_debug.u32SemTakeFailTimes[u32SNum]++;
			}
			u32IntStat = readl((const volatile void *)(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_STAT(ipc_ctrl.core_num)));
			u32SNum = ffSLsb(u32IntStat);
			i--;
		}while((u32SNum != 0) && (i  > 0));
	}
	else
	{
		return  IRQ_NONE;
	}
	return  IRQ_HANDLED;
}

s32 bsp_ipc_spin_lock(u32 u32SignalNum)
{
	u32 u32HsCtrl = 0;
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	for(;;)
	{
		u32HsCtrl = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum)));
		if (0 == u32HsCtrl)
		{
			ipc_debug.u32SemTakeTimes[u32SignalNum]++;
			ipc_debug.u32SemId = u32SignalNum;
			break;
		}
	}
	return OK;
}
s32 bsp_ipc_spin_unlock (u32 u32SignalNum)
{
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	writel(0,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum)));
	return OK;
}
static int hisi_k3_lpm2ap_ipc_handler(struct notifier_block *nb,
					unsigned long len, void *msg)
{
	u32 *result = (u32 *)msg,i=0, u32SNum=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	/*1需要判断是否是lPM3发给哪个用户的msg[0]，如果是LPM3 modem ipc适配接口发送的才做处理*/
	if(result[0]==LPM3_TO_AP_IPC_FLAG)
	{
		u32SNum = ffSLsb(result[1]);
		if(0!=u32SNum)
		{
			u32SNum--;
			if(ipc_ctrl.ipc_int_table[u32SNum].routine)
				ipc_ctrl.ipc_int_table[u32SNum].routine(ipc_ctrl.ipc_int_table[u32SNum].arg);
			else
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc_ctrl.ipc_int_table[%d] cb is null \n",i);
			}
		}
	}
	spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
	return 0;

}

static s32  bsp_ipc_probe(struct platform_device *dev)
{
	s32 ret = 0,i = 0; /*lint !e34 */
	u32 array_size=0;
	k3_ipc_block.next = NULL;
	k3_ipc_block.notifier_call = hisi_k3_lpm2ap_ipc_handler;
	ret = RPROC_MONITOR_REGISTER(HISI_RPROC_LPM3, &k3_ipc_block);
	if (ret) {
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc monitor register Failed!\n");
		return ERROR;
	}
	ipc_ctrl.core_num = IPC_CORE_ACORE;
	for(i = 0;i< INTSRC_NUM;i++ )
	{
		ipc_ctrl.sem_exist[i] = false;
	}
	array_size = sizeof(struct ipc_entry)*INTSRC_NUM;
	memset((void*)(ipc_ctrl.ipc_int_table),0x0,array_size);
	ipc_ctrl.ipc_base = HI_IPCM_REGBASE_ADDR_VIRT;/*lint !e569 */
	writel(0x0,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num)));
	writel(0x0,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num)));
	spin_lock_init(&ipc_ctrl.lock);
	ret = request_irq(IPC_INT, ipc_int_handler, IRQF_NO_SUSPEND, "ipc_irq",(void*) NULL);
	if (ret )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc int handler error,init failed\n");
		return ERROR;
	}
	ret = request_irq(IPC_SEM, ipc_sem_int_handler, IRQF_NO_SUSPEND, "ipc_sem",(void*) NULL);
	if (ret )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc sem handler error,init failed\n");
		return ERROR;
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc probe success\n");
	return OK;    /*lint !e438*/
}
static s32  bsp_ipc_remove(struct platform_device *dev)
{
	free_irq(IPC_SEM,NULL);
	free_irq(IPC_INT,NULL);
	return OK;
}
/*lint -restore +e550*/
void ipc_modem_reset(void)
{
    u32 i = 0,ret = 0;
	for(i=0;i<32;i++)
	{
		ret = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_STAT(ipc_ctrl.core_num,i)));
		if(ret == 0x9)
			writel(0,(volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(IPC_CORE_CCORE, i)));
	}
	return  ;
}

#ifdef CONFIG_PM
static s32 ipc_suspend_noirq(struct device *dev)
{
	u32 i = 0,ret = 0;
	for(i=0;i<32;i++)
	{
		ret = readl((const volatile void *)(ipc_ctrl.ipc_base + BSP_IPC_HS_STAT(ipc_ctrl.core_num,i)));
		if(ret==0x8)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"signum id = %d is occupied\n",i);
			return ERROR;
		}
	}
	return OK;
}
static const struct dev_pm_ops balong_ipc_pm_ops ={
	.suspend_noirq = ipc_suspend_noirq,
};

#define BALONG_DEV_PM_OPS (&balong_ipc_pm_ops)
#else
#define BALONG_DEV_PM_OPS NULL
#endif


static struct platform_driver balong_ipc_driver = {
	.probe = bsp_ipc_probe,
	.remove = bsp_ipc_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner  = THIS_MODULE,
		.pm     = BALONG_DEV_PM_OPS,
	},
};

static struct platform_device balong_ipc_device =
{
    .name = DRIVER_NAME,
    .id       = -1,
    .num_resources = 0,
};
static int __init hi_ipc_init(void)
{
	s32 ret = 0;
	ret = platform_device_register(&balong_ipc_device);
	if(ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"Platform ipc device register is failed!\n");
        	return ret;
	}
	ret = platform_driver_register(&balong_ipc_driver);
	if (ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"Platform ipc deriver register is failed!\n");
		platform_device_unregister(&balong_ipc_device);
		return ret;
	}
	return ret;
}
static void  bsp_ipc_exit(void)
{
	platform_driver_unregister(&balong_ipc_driver);
 	platform_device_unregister(&balong_ipc_device);
}


void bsp_ipc_debug_show(void)
{
	u32 i = 0;
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"\n当前占用的信号量ID为       : \t%d\n", ipc_debug.u32SemId);
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"当前接收中断的Core ID为          : \t%d\n", ipc_debug.u32RecvIntCore);
	for(i = 0; i < INTSRC_NUM; i++)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"信号量%d获取次数             : \t%d\n", i,ipc_debug.u32SemTakeTimes[i]);
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"信号量%d释放次数             : \t%d\n", i,ipc_debug.u32SemGiveTimes[i]);
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"%d号中断接收的次数为         : \t%d\n",i, ipc_debug.u32IntHandleTimes[i]);
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"%d号中断处理函数执行时间为 : \t%d us\n",i, ipc_debug.u32IntTimeDelta[i]*1000000/HI_TCXO_CLK);
	}      
}

void bsp_int_send_info(void)
{
	u32 i = 0,j = 0;
	for(i = 0;i <IPC_CORE_BUTTOM;i++)
	{
		for(j=0;j<IPC_INT_BUTTOM;j++)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"往核%d发送中断%d的次数为: \t%d\n",i,j, ipc_debug.u32IntSendTimes[i][j]);
		}
	}
		
}
/*lint -save -e19*/

EXPORT_SYMBOL(bsp_ipc_int_enable);
EXPORT_SYMBOL(bsp_ipc_int_disable);
EXPORT_SYMBOL(bsp_ipc_int_connect);
EXPORT_SYMBOL(bsp_ipc_int_disconnect);
EXPORT_SYMBOL(bsp_ipc_int_send);
EXPORT_SYMBOL(bsp_ipc_sem_create);
EXPORT_SYMBOL(bsp_ipc_sem_take);
EXPORT_SYMBOL(bsp_ipc_sem_give);
EXPORT_SYMBOL(bsp_ipc_spin_lock);
EXPORT_SYMBOL(bsp_ipc_spin_unlock);
EXPORT_SYMBOL(bsp_ipc_debug_show);
EXPORT_SYMBOL(bsp_int_send_info);
arch_initcall(hi_ipc_init);
module_exit(bsp_ipc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Drive Group");
/*lint -restore +e19*/


#ifdef __cplusplus
}
#endif
