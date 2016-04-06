
#include <bsp_hardtimer.h>
#include <osl_sem.h>
#include <bsp_om.h>
#include <osl_irq.h> /*lint !e537*/
#include <linux/delay.h>
typedef irqreturn_t return_type;

void hardtimer_test(void);

s32 adp_test_case01(void);
s32 adp_test_case02(void);
void print_count(void);
int func(int para);

#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR  (-1)
#endif
#ifndef SEM_EMPTY
#define SEM_EMPTY 0
#endif
#ifndef SEM_FULL
#define SEM_FULL 1
#endif

struct bsp_hardtimer_control my_timer;
osl_sem_id sem;
s32 cnt = 0;
OSL_IRQ_FUNC(static return_type,for_timer_test,irq,para)
{
    bsp_hardtimer_int_clear(my_timer.timerId);
    osl_sem_up(&sem);   
    return IRQ_HANDLED;
}
/*
OSL_IRQ_FUNC(static return_type,for_test_unit,irq,para)
{
   s32 ret;
   ret = bsp_hardtimer_disable(my_timer.timerId);
   if (ret == OK)
   	cnt++;
   ret = bsp_hardtimer_enable(my_timer.timerId);
   return IRQ_HANDLED;
}*/
void print_count(void)
{
   bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"cnt = %d\n",cnt);
   return;
}

/*测试函数接口操作timer0*/
s32 k3_hardtimer_test_case01(void)
{
   unsigned long i=0; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
   my_timer.para =(void*) i;
   osl_sem_init(SEM_EMPTY,&sem);
   ret = bsp_hardtimer_alloc(&my_timer);
   if (OK != ret)
   	return ERROR;
   ret = bsp_hardtimer_enable(my_timer.timerId);
   if (OK != ret)
   	return ERROR;
   osl_sem_down(&sem);
   ret = bsp_hardtimer_free(i);
   if (OK != ret)
   {
	osl_sem_up(&sem);
	osl_sema_delete(&sem);
	return ERROR;
    }
   osl_sem_up(&sem);
   osl_sema_delete(&sem);
   return OK;
}
/*测试函数接口操作timer0*/
s32 k3_hardtimer_test_case02(void)
{
   unsigned long i=4; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
   my_timer.para =(void*) i;
   osl_sem_init(SEM_EMPTY,&sem);
   ret = bsp_hardtimer_alloc(&my_timer);
   if (OK != ret)
   	return ERROR;
   ret = bsp_hardtimer_enable(my_timer.timerId);
   if (OK != ret)
   	return ERROR;
   osl_sem_down(&sem);
   ret = bsp_hardtimer_free(i);
   if (OK != ret)
   {
	osl_sem_up(&sem);
	osl_sema_delete(&sem);
	return ERROR;
    }
   osl_sem_up(&sem);
   osl_sema_delete(&sem);
   return OK;
}
/*测试函数接口操作timer0*/
s32 k3_hardtimer_test_case03(void)
{
   unsigned long i=5; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
   my_timer.para = (void*)i;
   osl_sem_init(SEM_EMPTY,&sem);
   ret = bsp_hardtimer_alloc(&my_timer);
   if (OK != ret)
   	return ERROR;
   ret = bsp_hardtimer_enable(my_timer.timerId);
   if (OK != ret)
   	return ERROR;
   osl_sem_down(&sem);
   ret = bsp_hardtimer_free(i);
   if (OK != ret)
   {
	osl_sem_up(&sem);
	osl_sema_delete(&sem);
	return ERROR;
    }
   osl_sem_up(&sem);
   osl_sema_delete(&sem);
   return OK;
}

s32 k3_hardtimer_test_case04(void)
{
   unsigned long i=6; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
   my_timer.para = (void*)i;
   osl_sem_init(SEM_EMPTY,&sem);
   ret = bsp_hardtimer_alloc(&my_timer);
   if (OK != ret)
   	return ERROR;
   ret = bsp_hardtimer_enable(my_timer.timerId);
   if (OK != ret)
   	return ERROR;
   osl_sem_down(&sem);
   ret = bsp_hardtimer_free(i);
   if (OK != ret)
   {
	osl_sem_up(&sem);
	osl_sema_delete(&sem);
	return ERROR;
    }
   osl_sem_up(&sem);
   osl_sema_delete(&sem);
   return OK;
}

void hardtimer_test(void)
{
   s32 ret;
   ret = k3_hardtimer_test_case01();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case01 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case01 fail\n");
   ret = k3_hardtimer_test_case02();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case02 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case02 fail\n");
   ret =k3_hardtimer_test_case03();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case03 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case03 fail\n");
   ret = k3_hardtimer_test_case04();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case04 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case04 fail\n");
   }
/*****************************begin: test adp function***************************************/
int func(int para)
{
	osl_sem_up(&sem);
	return 0;
}
s32 k3_adp_test_case01(void)
{
	s32 ret = 0;
	osl_sem_init(SEM_EMPTY,&sem);
	ret = DRV_TIMER_START(TIMER_ACPU_CPUVIEW_ID,func,0,1000,0,TIMER_UNIT_MS);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	osl_sem_down(&sem);
	ret = DRV_TIMER_STOP(TIMER_ACPU_CPUVIEW_ID);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	osl_sema_delete(&sem);
	return OK;
}
s32 k3_adp_test_case02(void)
{
	s32 ret = 0;
	osl_sem_init(SEM_EMPTY,&sem);
	ret = DRV_TIMER_START(TIMER_ACPU_OSA_ID,func,0,1000,0,TIMER_UNIT_MS);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	osl_sem_down(&sem);
	ret = DRV_TIMER_STOP(TIMER_ACPU_OSA_ID);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	osl_sema_delete(&sem);
	return OK;
}
s32 k3_adp_test_case03(void)
{
	s32 ret = 0;
	unsigned int restTime;
	osl_sem_init(SEM_EMPTY,&sem);
	ret = DRV_TIMER_START(TIMER_ACPU_OSA_ID,func,0,10000,0,TIMER_UNIT_MS);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	msleep(100);
	ret = DRV_TIMER_GET_REST_TIME(TIMER_ACPU_OSA_ID,TIMER_UNIT_MS,&restTime);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_GET_REST_TIME fail\n");
		return ERROR;
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"rest time = %d ms\n",restTime);
	osl_sem_down(&sem);
	ret = DRV_TIMER_STOP(TIMER_ACPU_OSA_ID);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	osl_sema_delete(&sem);
	return OK;
}


/*****************************end: test adp function***************************************/

