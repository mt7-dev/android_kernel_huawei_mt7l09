
#include <bsp_hardtimer.h>
#include <osl_sem.h>
#include <bsp_om.h>
#include <osl_irq.h>  /*lint !e537*/
#ifdef __KERNEL__
#include <linux/delay.h>
typedef irqreturn_t return_type;

#elif defined(__VXWORKS__)
#include <taskLib.h>
extern int sysClkRateGet(void);
static void msleep(int x)
{
    taskDelay(sysClkRateGet()*x/1000+1);
}
typedef void return_type;

#endif
s32 hardtimer_test_case01(void);
s32 hardtimer_test_case02(void);
s32 hardtimer_test_case03(void);
s32 hardtimer_test_case04(void);
s32 hardtimer_test_case05(void);
s32 hardtimer_test_case06(void);
s32 hardtimer_test_case07(void);
s32 hardtimer_test_case08(void);
s32 hardtimer_test_case09(void);
s32 hardtimer_test_case10(void);
s32 hardtimer_test_case11(void);
s32 hardtimer_test_case12(void);
s32 hardtimer_test_case13(void);
s32 hardtimer_test_case14(void);
s32 hardtimer_test_case15(void);
s32 hardtimer_test(void);

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
u32 cnt = 0;
OSL_IRQ_FUNC(static return_type,for_timer_test,irq,para)
{
    bsp_hardtimer_int_clear(my_timer.timerId);
    osl_sem_up(&sem);   
    return IRQ_HANDLED;
}

OSL_IRQ_FUNC(static return_type,for_test_unit,irq,para)
{
   s32 ret;
   ret = bsp_hardtimer_disable(my_timer.timerId);
   if (ret == OK)
   	cnt++;
 	bsp_hardtimer_enable(my_timer.timerId);
   return IRQ_HANDLED;
}
void print_count(void)
{
   bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"cnt = %d\n",cnt);
   return;
}

/*测试函数接口操作timer0*/
s32 hardtimer_test_case01(void)
{
   u32 i=0; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
   my_timer.para = i;
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

/*测试函数接口操作timer9*/
s32 hardtimer_test_case02(void)
{
   u32 i=9; 
   s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test; 
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer10*/
s32 hardtimer_test_case03(void)
{
   u32 i=11;
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;  
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer17*/

s32 hardtimer_test_case04(void)
{
   u32 i=17;
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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

/*测试函数接口操作timer19*/
s32 hardtimer_test_case05(void)
{
   u32 i=19;
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test; 
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer20*/

s32 hardtimer_test_case06(void)
{
   u32 i=20; 
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;   
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer21*/

s32 hardtimer_test_case07(void)
{
   u32 i=21; 
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;  
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer22*/

s32 hardtimer_test_case08(void)
{
   u32 i=22; 
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;  
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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
/*测试函数接口操作timer23*/

s32 hardtimer_test_case09(void)
{
   u32 i=23; 
    s32 ret = 0;
   my_timer.func =(timer_func) for_timer_test;  
   my_timer.mode = TIMER_PERIOD_COUNT;
   my_timer.timeout = 32768;
   my_timer.timerId = i;
    my_timer.para = i;
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

s32 hardtimer_test_case10(void)
{
	u32 i=17,temp=0; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_MS;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	ret = bsp_hardtimer_free(i);
	if(ret!=OK)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"bsp_hardtimer_free failed\n");
		return ERROR;
	}
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
s32 hardtimer_test_case11(void)
{
	u32 i=17,temp=0; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_US;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	bsp_hardtimer_free(i);
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
s32 hardtimer_test_case12(void)
{
	u32 i=9,temp; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_MS;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	bsp_hardtimer_free(i);
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
s32 hardtimer_test_case13(void)
{
	u32 i=9,temp=0; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_US;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	bsp_hardtimer_free(i);
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
s32 hardtimer_test_case14(void)
{
	u32 i=20,temp=0; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_MS;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	bsp_hardtimer_free(i);
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
s32 hardtimer_test_case15(void)
{
	u32 i=20,temp=0; 
	s32 ret = 0;
	my_timer.func =(timer_func) for_test_unit;
	my_timer.mode = TIMER_PERIOD_COUNT;
	my_timer.timeout = 1000000;
	my_timer.timerId = i;
	my_timer.para = i;
	my_timer.unit = TIMER_UNIT_US;
	temp = cnt;
	ret = bsp_hardtimer_start(&my_timer);
	if (OK != ret)
		return ERROR;
	msleep(3000);
	bsp_hardtimer_free(i);
	if(cnt-temp!=3)
		return ERROR;
	return OK;
}
int func(int para)
{
	osl_sem_up(&sem);
	return 0;
}
s32 adp_test_case01(void)
{
	s32 ret = 0;
	unsigned int restTime;
	osl_sem_init(SEM_EMPTY,&sem);
	ret = DRV_TIMER_START(TIMER_ACPU_OSA_ID,func,0,10000,0,TIMER_UNIT_NONE);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	ret = DRV_TIMER_GET_REST_TIME(TIMER_ACPU_OSA_ID,TIMER_UNIT_NONE,&restTime);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_GET_REST_TIME fail\n");
		return ERROR;
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"rest time = %d \n",restTime);
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
s32 adp_test_case02(void)
{
	s32 ret = 0;
	unsigned int restTime;
	osl_sem_init(SEM_EMPTY,&sem);
	ret = DRV_TIMER_START(TIMER_ACPU_CPUVIEW_ID ,func,0,10000,0,TIMER_UNIT_NONE);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_START fail\n");
		return ERROR;
	}
	ret = DRV_TIMER_GET_REST_TIME(TIMER_ACPU_CPUVIEW_ID,TIMER_UNIT_NONE,&restTime);
	if(ret!=0)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"DRV_TIMER_GET_REST_TIME fail\n");
		return ERROR;
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"rest time = %d \n",restTime);
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


s32 hardtimer_test(void)
{
   s32 ret;
   ret = hardtimer_test_case01();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case01 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case01 fail\n");
   ret = hardtimer_test_case02();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case02 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case02 fail\n");
   ret = hardtimer_test_case03();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case03 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case03 fail\n");
   ret = hardtimer_test_case04();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case04 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case04 fail\n");
   ret = hardtimer_test_case05();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case05 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case05 fail\n");
   ret = hardtimer_test_case06();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case06 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case06 fail\n");
   ret = hardtimer_test_case07();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case07 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case07 fail\n");
   ret = hardtimer_test_case08();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case08 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case08 fail\n");
   ret = hardtimer_test_case09();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case09 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case09 fail\n");
   ret = hardtimer_test_case10();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case10 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case10 fail\n");
   ret = hardtimer_test_case11();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case11 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case11 fail\n");
   ret = hardtimer_test_case12();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case12 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case12 fail\n");
   ret = hardtimer_test_case13();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case13 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case13 fail\n");
   ret = hardtimer_test_case14();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case14 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case14 fail\n");
   ret = hardtimer_test_case15();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case15 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"hardtimer_test_case15 fail\n");
    ret = adp_test_case01();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"adp_test_case01 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"adp_test_case01 fail\n");
   ret = adp_test_case02();
   if (OK == ret)
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"adp_test_case02 pass\n");
   else
   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HARDTIMER,"adp_test_case02 fail\n");
    return OK;

   }
