#ifdef __KERNEL__
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#define delay(a) msleep(a)
#elif defined(__VXWORKS__)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <taskLib.h>
#define delay(a) taskDelay(a)
#endif
#include <bsp_om.h>
#include <bsp_ipc.h>
#include <osl_module.h>
#include "ipc_balong.h"

u32 count_num[32] = {0};
#define  TEST_COUNT  10
#ifdef __KERNEL__
#define DEST_CORE IPC_CORE_CCORE
#define TEST_INT_NUM IPC_CCPU_INT_SRC_ACPU_TEST_ENABLE

#elif defined(__VXWORKS__)
#define DEST_CORE IPC_CORE_ACORE
#define TEST_INT_NUM IPC_ACPU_INT_SRC_CCPU_TEST_ENABLE
#endif
/*lint --e{129, 63, 64, 409, 52,716} */
s32 ipc_test_case01(void);
s32 ipc_test_case02(void);
s32 ipc_test_case03(void);
s32 bsp_ipc_test_init(void);
int ipc_func1(void*data);
s32 ipc_test_spinlock(void);


static void bsp_for_intenable_handler(u32 n)
{
	
	if(OK!= bsp_ipc_sem_take(IPC_SEM2_IPC_TEST,0))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"int handler take  error\n");
	}
}

/********************************ST BEGIN****************************************/
s32 ipc_test_case01(void)
{
	s32 ret=0;
	ret = bsp_ipc_sem_create(IPC_SEM1_IPC_TEST);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], bsp_ipc_sem_create error\n",__FUNCTION__);
		return ERROR;
	}
	ret = bsp_ipc_sem_take(IPC_SEM1_IPC_TEST,10);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], bsp_ipc_sem_take error\n",__FUNCTION__);
		return ERROR;
	}
	ret = bsp_ipc_sem_give(IPC_SEM1_IPC_TEST);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], sem_give error\n",__FUNCTION__);
		return ERROR;
	}
	ret = bsp_ipc_int_send(DEST_CORE,TEST_INT_NUM);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], int send error\n",__FUNCTION__);
		return ERROR;
	}
	delay(100);
	ret = bsp_ipc_sem_take(IPC_SEM1_IPC_TEST,10);
	if(OK!=ret)
	{
		ret = bsp_ipc_sem_delete(IPC_SEM1_IPC_TEST);
		if(OK!=ret)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], bsp_ipc_sem_delete error\n",__FUNCTION__);
			return ERROR;
		}
		return OK;
	}
	else
	{
		ret = bsp_ipc_sem_delete(IPC_SEM1_IPC_TEST);
		if(OK!=ret)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], bsp_ipc_sem_delete error\n",__FUNCTION__);
			return ERROR;
		}
		return ERROR;
	}	
}
/*测试多次take和give同一个信号量*/
s32 ipc_test_case02(void)
{
	s32 ret = 0,i = 0;
	ret = bsp_ipc_sem_give(IPC_SEM2_IPC_TEST);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], sem_give error\n",__FUNCTION__);
		return ret;
	}
	ret = bsp_ipc_sem_create(IPC_SEM2_IPC_TEST);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]: sem_create error\n",__FUNCTION__);
		return ERROR;
	}
	for(i=0;i<TEST_COUNT;i++)
	{
		ret = bsp_ipc_sem_take(IPC_SEM2_IPC_TEST,0);
		if(OK!=ret)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]: sem_take error,loop i=%d\n",__FUNCTION__,i);
			return ERROR;
		}
		ret = bsp_ipc_sem_give(IPC_SEM2_IPC_TEST);
		if(OK!=ret)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]: sem_give error,loop i=%d\n",__FUNCTION__,i);
			return ERROR;
		}
	}
	ret = bsp_ipc_sem_delete(IPC_SEM2_IPC_TEST);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s], sem_delete error\n",__FUNCTION__);
		return ERROR;
	}
	return OK;
}
/*测试bsp_ipc_spin_lock和bsp_ipc_spin_unlock函数实现*/

s32 ipc_test_case03(void)
{
	s32 ret = 0;
	unsigned long flags = 0;
	local_irq_save(flags);
	ret = bsp_ipc_spin_lock(IPC_SEM2_IPC_TEST);
	if(ret != OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]:spinlock error\n",__FUNCTION__);
		local_irq_restore(flags);
		return ERROR;
	}
	ret = bsp_ipc_spin_unlock(IPC_SEM2_IPC_TEST);
	if(ret != OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]:spinunlock error\n",__FUNCTION__);
		local_irq_restore(flags);
		return ERROR;
	}
	local_irq_restore(flags);
	return OK;
}

s32 bsp_ipc_test_init(void)
{
	s32 ret = 0;
	ret = bsp_ipc_int_connect(IPC_ACPU_INT_SRC_CCPU_TEST_ENABLE,bsp_for_intenable_handler,0);
	if(ret !=OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"bsp_ipc_test_init connect error\n");
		return ERROR;
	}
	ret = bsp_ipc_int_enable(IPC_ACPU_INT_SRC_CCPU_TEST_ENABLE);
	if(ret != OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"bsp_ipc_test_init enable int  error\n");
		return ERROR;
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"bsp_ipc_test_init ok\n");
	return OK;
}
/*lint -save -e19*/

EXPORT_SYMBOL(bsp_ipc_test_init);
#ifdef ENABLE_TEST_CODE
module_init(bsp_ipc_test_init);
#endif
/*lint -restore +e19*/

int ipc_func1(void*data)
{
	s32 ret = 0;
	ret = bsp_ipc_sem_create(IPC_SEM1_IPC_TEST);
	if(ret!=OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc_func1 bsp_ipc_sem_create fail\n");
	}
	while(1)
	{
		delay(2);
		
		ret = bsp_ipc_sem_take(IPC_SEM1_IPC_TEST,100);
		if(ret==OK)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc task1 take success\n");
			delay(2);
			ret = bsp_ipc_sem_give(IPC_SEM1_IPC_TEST);
		}
		else
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"ipc task1 take fail\n");
		}	
	}
}
/*A核和C核各启动一个任务，同时抢占同一个信号量*/
s32 test_take_ipc_sem(void)
{
	struct task_struct *task1;
	 task1 = kthread_create(ipc_func1,(void *)NULL,"acore ipc");
	if ( NULL !=task1)
	{
		wake_up_process( task1);
	}
	return OK;
}
/*****************************begin:测试A核和M3交互************************************************/
void bsp_for_test_mcore(u32 n)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"test_mcore n = %d\n",n);
}

s32 bsp_ipc_test_to_m(void)
{
	(void)bsp_ipc_int_send(IPC_CORE_MCORE,IPC_MCU_INT_SRC_TEST);
	return OK;
}
s32 bsp_ipc_test_from_m(void)
{
	(void)bsp_ipc_int_connect(IPC_ACPU_INT_SRC_MCU_FOR_TEST,bsp_for_test_mcore,31);
	(void)bsp_ipc_int_enable(IPC_ACPU_INT_SRC_MCU_FOR_TEST);
	return OK;
}
s32 ipc_test_spinlock(void)
{
	s32 ret = 0;
	unsigned long flags = 0;
	local_irq_save(flags);
	ret = bsp_ipc_spin_lock(IPC_SEM1_IPC_TEST);/*复用此信号量，测试与m3争用*/
	if(ret != OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]:spinlock error\n",__FUNCTION__);
		local_irq_restore(flags);
		return ERROR;
	}
	delay(5000);
	ret = bsp_ipc_spin_unlock(IPC_SEM1_IPC_TEST);
	if(ret != OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_IPC,"[%s]:spinunlock error\n",__FUNCTION__);
		local_irq_restore(flags);
		return ERROR;
	}
	local_irq_restore(flags);
	return OK;
}

/*****************************end:测试A核和M3交互************************************************/

