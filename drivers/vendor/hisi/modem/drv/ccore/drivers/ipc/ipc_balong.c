/*lint --e{537,713,732,752}*/
#ifdef __KERNEL__
#include <linux/platform_device.h>
#include <soc_interrupts_app.h>

#elif defined(__VXWORKS__)
#include <string.h>
#include <soc_interrupts_mdm.h>
//#include <bsp_uart.h>
#endif
#include <osl_module.h>
#include <bsp_memmap.h>
#include <bsp_om.h>
#include <bsp_ipc.h>
#include <bsp_hardtimer.h>
#include <bsp_dpm.h>
#include "ipc_balong.h"
#ifdef __cplusplus
extern "C" {
#endif
void bsp_ipc_debug_show(void);
extern s32 bsp_ipc_test_init(void);

#ifdef __KERNEL__
#define IPC_INT       INT_LVL_IPCM2APP0
#define IPC_SEM       INT_LVL_IPCM2APP1
#elif defined(__VXWORKS__)
#define IPC_INT       INT_LVL_IPCM2MDM0
#define IPC_SEM       INT_LVL_IPCM2MDM1
#endif

static struct ipc_control		ipc_ctrl;
static struct ipc_debug_s		ipc_debug = {0};
#define IPC_CHECK_PARA(para,max) \
    do {\
        if (para >= max)\
        {\
            bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]Wrong para , line:%d,para = %d\n",__FUNCTION__, __LINE__,para); \
            return ERROR; \
        } \
    } while (0)

s32 bsp_ipc_int_enable (IPC_INT_LEV_E ulLvl)
{
    u32 u32IntMask = 0;
    unsigned long flags=0;
    IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
    /*写中断屏蔽寄存器*/
    spin_lock_irqsave(&ipc_ctrl.lock,flags);
    u32IntMask = readl(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num));
    u32IntMask |= (u32)1 << ulLvl;
    writel(u32IntMask,ipc_ctrl.ipc_base+BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num));
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
    u32IntMask = readl(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num));
    u32IntMask = u32IntMask & (~((u32)1 << ulLvl));
    writel(u32IntMask, ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num));
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

OSL_IRQ_FUNC(static irqreturn_t,ipc_int_handler,irq,dev_id)
{
	u32 i = 0;
	u32 u32IntStat = 0,begin = 0,end = 0;
	u32 u32Date = 0x1;
	u32 u32BitValue = 0;
	u32IntStat=readl(ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_STAT(ipc_ctrl.core_num));
	/*清中断*/
	writel(u32IntStat,ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_CLR(ipc_ctrl.core_num));
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
	unsigned long flags = 0;
	IPC_CHECK_PARA(ulLvl,IPC_INT_BUTTOM);
	IPC_CHECK_PARA(enDstCore,IPC_CORE_BUTTOM);
	/*写原始中断寄存器,产生中断*/
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	writel((u32)1 << ulLvl,ipc_ctrl.ipc_base + BSP_IPC_CPU_RAW_INT(enDstCore));
	#ifdef CONFIG_P531_DRX_IPC
	switch(ulLvl)
	{
		case IPC_INT_DICC_RELDATA:
		case IPC_ACPU_INT_SRC_CCPU_MSG:
		case IPC_ACPU_INT_SRC_CCPU_NVIM:
		case IPC_INT_DICC_USRDATA:
		case IPC_ACPU_INT_SRC_ICC:
		case IPC_ACPU_INT_SRC_ICC_PRIVATE:
		case IPC_ACPU_SRC_CCPU_DUMP:
			writel(1<<IPC_MCU_INT_SRC_CCPU_DRX,ipc_ctrl.ipc_base + BSP_IPC_CPU_RAW_INT(IPC_CORE_MCORE));
			break;
		default:
			break;
	}
	#endif
	spin_unlock_irqrestore(&ipc_ctrl.lock,flags);
	ipc_debug.u32RecvIntCore = enDstCore;
	ipc_debug.u32IntSendTimes[enDstCore][ulLvl]++;
	return OK;
}

static void  mask_int(u32 u32SignalNum)
{
	u32 u32IntMask = 0;
	unsigned long flags=0;
	spin_lock_irqsave(&ipc_ctrl.lock,flags);
	u32IntMask = readl(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num));
	u32IntMask = u32IntMask & (~((u32)1 << u32SignalNum));
	writel(u32IntMask,ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num));
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

 s32 bsp_ipc_sem_take(u32 u32SignalNum,s32 s32timeout)
 {
	u32 u32IntMask = 0,ret = 0;
	/*参数检查*/
	IPC_CHECK_PARA(u32SignalNum,IPC_SEM_BUTTOM);
	 /*将申请的信号量对应的释放中断清零*/
	writel((u32)1<<u32SignalNum, ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_CLR(ipc_ctrl.core_num));
	ret =  readl(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum));
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
			u32IntMask = readl(ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num));
			u32IntMask = u32IntMask | ((u32)1 << u32SignalNum);
			writel(u32IntMask,ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num));
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
	writel(0,ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum));
	return OK;
 }
 static s32 ffSLsb(s32 args)
 {
	 s32 num = 0;
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
OSL_IRQ_FUNC(static irqreturn_t,ipc_sem_int_handler,irq,dev_id)
{
	u32 u32IntStat = 0,u32HsCtrl=0,u32SNum=0, i = 32;
	u32IntStat = readl(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_STAT(ipc_ctrl.core_num));
	u32SNum = ffSLsb(u32IntStat);
	if( u32SNum != 0)
	{
		do
		{
			 /*如果有信号量释放中断，清除该中断*/
			writel((u32)1<<--u32SNum, ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_CLR(ipc_ctrl.core_num));
			u32HsCtrl = readl(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SNum));
			if (0 == u32HsCtrl)
			{
				osl_sem_up(&(ipc_ctrl.sem_ipc_task[u32SNum]));
			}
			else
			{
				ipc_debug.u32SemTakeFailTimes[u32SNum]++;
			}
			u32IntStat = readl(ipc_ctrl.ipc_base+BSP_IPC_SEM_INT_STAT(ipc_ctrl.core_num));
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
		u32HsCtrl = readl(ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum));
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
	writel(0,ipc_ctrl.ipc_base + BSP_IPC_HS_CTRL(ipc_ctrl.core_num, u32SignalNum));
	return OK;
}

static s32 ipc_suspend_late(struct dpm_device *dev)
{
	u32 i = 0,ret = 0;
	for(i=0;i<32;i++)
	{
		ret = readl(ipc_ctrl.ipc_base + BSP_IPC_HS_STAT(ipc_ctrl.core_num,i));
		if(ret==0x9)
		{
			//printksync("ipc signum id = %d is occupied\n",i);
			return ERROR;
		}
	}
	return OK;
}
struct dpm_device ipc_dpm =
{
	.device_name = "ipc dpm",
	.suspend_late = ipc_suspend_late,
};

void bsp_ipc_init(void)
{
	s32 ret = 0,i = 0;
	ipc_ctrl.core_num = IPC_CORE_CCORE;
	for(i = 0;i< INTSRC_NUM;i++ )
	{
		ipc_ctrl.sem_exist[i] = false;
	}
	#ifdef CONFIG_CCORE_PM
	ret = bsp_device_pm_add(&ipc_dpm);
	if(ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc init fail\n");
		return;
	}
	#endif
	memset((void*)(ipc_ctrl.ipc_int_table),0x0,sizeof(struct ipc_entry) *INTSRC_NUM);
	ipc_ctrl.ipc_base = HI_IPCM_REGBASE_ADDR_VIRT;
	writel(0x0,ipc_ctrl.ipc_base + BSP_IPC_CPU_INT_MASK(ipc_ctrl.core_num));
	writel(0x0,ipc_ctrl.ipc_base + BSP_IPC_SEM_INT_MASK(ipc_ctrl.core_num));
	spin_lock_init(&ipc_ctrl.lock);
	ret = request_irq(IPC_INT,(irq_handler_t) ipc_int_handler, 0, "ipc_irq",(void*) NULL);
	if (ret )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc int handler error,init failed\n");
		return;
	}
	ret = request_irq(IPC_SEM, (irq_handler_t) ipc_sem_int_handler, 0, "ipc_sem",(void*) NULL);
	if (ret )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc sem handler error,init failed\n");
		return;
	}
	#ifdef ENABLE_TEST_CODE
	ret = bsp_ipc_test_init();
	if (ret )
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"bsp_ipc_test_init failed\n");
		return;
	}
	#endif
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc init success\n");
	return;
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
#ifdef __cplusplus
}
#endif
