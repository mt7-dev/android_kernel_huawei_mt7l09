
/*lint --e{537}*/
#ifdef __KERNEL__
//#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#define delay(a) msleep(a)
#elif defined(__VXWORKS__)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <taskLib.h>
#define delay(a) taskDelay(a)
#endif
#include <osl_types.h>
#include <bsp_softtimer.h>
#include <bsp_om.h>
#include <osl_sem.h>
#include "softtimer_balong.h"
/*lint --e{129, 516, 63 } */

void for_softtimer_test_1(u32 para);
void for_softtimer_test_2(u32 para);
void for_softtimer_test_3(u32 para);
void for_softtimer_test_4(u32 para);
void for_softtimer_test_5(u32 para);
void for_softtimer_test_6(u32 para);
void for_softtimer_test_7(u32 para);
void for_softtimer_test_8(u32 para);
void for_softtimer_test_9(u32 para);
s32 st_softtimer_test_case01(void);
s32 st_softtimer_test_case02(void);
s32 st_softtimer_test_case03(void);
s32 st_softtimer_test_case04(void);
s32 st_softtimer_test_case05(void);
s32 st_softtimer_test_case06(void);
s32 st_softtimer_test_case07(void);
s32 st_softtimer_test_case08(void);
s32 st_softtimer_test_case09(void);
s32 st_softtimer_test_case10(void);
s32 st_softtimer_test_case11(void);
s32 st_softtimer_test_case12(void);
s32 st_softtimer_test_case13(void);
s32 st_softtimer_test_case14(void);
void st_softtimer_test_case15(void);
void st_softtimer_test_case16(void);
s32 st_softtimer_test_case17(void);
s32 softtimer_test(void);


osl_sem_id sem1,sem2,sem3,sem4,sem5,sem6;
struct softtimer_list my_softtimer,my_sec_softtimer,my_thir_softtimer;
struct softtimer_list myown_timer[50];
u32 num[10] = {0};
void for_softtimer_test_1(u32 para)
{
    s32 ret;
    ret = bsp_softtimer_free(&my_softtimer);
    if (ret !=OK)
    {
    	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_1  free error\n");
	    return;
    }
    osl_sem_up(&sem1);
    return;
}
void for_softtimer_test_2(u32 para)
{
    s32 ret = 0;
    num[0]++;
    if (num[0]<10)
    {
       bsp_softtimer_add(&my_softtimer);
    }
    else if (num[0]==10)
     {
         num[0] = 0;
         ret = bsp_softtimer_free(&my_softtimer);
         if (ret !=OK)
		{
	  	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_2  free error\n");
		return;
		}
	 osl_sem_up(&sem2);
   }
   return;
}
void for_softtimer_test_3(u32 para)
{
    s32 ret = 0;
    num[1]++;
    if (num[1]<10)
    {   
   	  ret = bsp_softtimer_modify(&my_softtimer,num[1]*100);
	  if (ret !=OK)
	  {
	 	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_3  11modify error\n");
		return;
	  }
	  bsp_softtimer_add(&my_softtimer);
    }
    if (num[1]==10)
    {
    	num[1] = 0;
        ret = bsp_softtimer_free(&my_softtimer);
        if (ret !=OK)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_3  free error\n");
			return;
        }
		osl_sem_up(&sem3);
	}	
	return;
}
void for_softtimer_test_4(u32 para)
{
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"para = %d \n",para);
    num[2]++;
    if (num[2]<50)
    {   
	  bsp_softtimer_add(&myown_timer[para]);
    }
    num[2] = 0;
    return;
}
void  for_softtimer_test_5(u32 para)
{
    s32 ret;
    ret = bsp_softtimer_free(&my_sec_softtimer);
    if (ret !=OK)
    {
    	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_5  free error\n");
    	return ;
    }
    osl_sem_up(&sem5);
    return ;
}
void for_softtimer_test_6(u32 para)
{
    s32 ret;
    ret = bsp_softtimer_free(&my_thir_softtimer);
    if (ret !=OK)
    {
    	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"for_softtimer_test_6  free error\n");
	    return ;
    }
    osl_sem_up(&sem6);
    return ;
}
void for_softtimer_test_7(u32 para)
{
    //bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"para = %d\n",para);
    bsp_softtimer_add(&myown_timer[para]);
    return ;
}
void for_softtimer_test_8(u32 para)
{
    bsp_softtimer_add(&my_softtimer);
   return ;
}
void for_softtimer_test_9(u32 para)
{
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"para = %d \n",para);
    bsp_softtimer_add(&my_sec_softtimer);
    return ;
}


/*测试唤醒源软timer申请和加入链表,在回调函数释放*/
s32 st_softtimer_test_case01(void)
{
	
	my_softtimer.func =for_softtimer_test_1;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	osl_sem_init(0,&sem1);
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem1);
	osl_sema_delete(&sem1);
	return OK;
}
/*测试唤醒源软timer申请和加入链表,重复执行十次后释放*/
s32 st_softtimer_test_case02(void)
{
	
	my_softtimer.func = for_softtimer_test_2;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	osl_sem_init(0,&sem2);
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem2);
	osl_sema_delete(&sem2);
	return OK;
}

/*测试唤醒源软timer申请和加入链表重复10次，每次都在回调函数修改超时时间*/
s32 st_softtimer_test_case03(void)
{
	
	my_softtimer.func = for_softtimer_test_3;	
	my_softtimer.para = 2;
	my_softtimer.timeout = 10;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	osl_sem_init(0,&sem3);
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem3);
	osl_sema_delete(&sem3);
	return OK;
}

/*测试创建三个唤醒源软timer，按照超时时间大小按顺序执行其超时函数*/
s32 st_softtimer_test_case04(void)
{
	
	my_softtimer.func = for_softtimer_test_1;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	my_softtimer.para = 1;
	my_softtimer.timeout = 100;
	osl_sem_init(0,&sem1);
	osl_sem_init(0,&sem5);
	osl_sem_init(0,&sem6);
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	my_sec_softtimer.func = for_softtimer_test_5;
	my_sec_softtimer.para = 2;
	my_sec_softtimer.timeout = 200;
	if (bsp_softtimer_create(&my_sec_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_sec_softtimer);
	my_thir_softtimer.func = for_softtimer_test_6;
	my_thir_softtimer.para = 3;
	my_thir_softtimer.timeout = 300;
	if (bsp_softtimer_create(&my_thir_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_thir_softtimer);
	osl_sem_down(&sem1);
	osl_sem_down(&sem5);
	osl_sem_down(&sem6);
	osl_sema_delete(&sem1);
	osl_sema_delete(&sem5);
	osl_sema_delete(&sem6);
	return OK;
}

/*测试唤醒源软timer申请和加入链表，在回调函数外删除软timer
   超时时间较短*/
s32 st_softtimer_test_case05(void)
{
	s32 ret= 0;
	
	my_softtimer.func = for_softtimer_test_1;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	osl_sem_init(0,&sem1);
	 if (bsp_softtimer_create(&my_softtimer))
	 {
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	 }
	 bsp_softtimer_add(&my_softtimer);
	 ret = bsp_softtimer_delete(&my_softtimer);
	 if (ret ==OK)
	 {
	    ret = bsp_softtimer_free(&my_softtimer);
	    if (ret !=OK)
	    {
	    	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"softtimer_test_5  free error\n");
			return ret;
	    }
	    
	}
	else if(NOT_ACTIVE == ret)
	{
		osl_sem_down(&sem1);
		osl_sema_delete(&sem1);
	}
	return OK;	
}
/*测试唤醒源软timer申请和加入链表，在回调函数外删除软timer
   超时时间较长*/
s32 st_softtimer_test_case06(void)
{
	s32 ret= 0;
	
	my_softtimer.func = for_softtimer_test_1;
	my_softtimer.para = 123;
	my_softtimer.timeout = 100;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	osl_sem_init(0,&sem1);
	 if (bsp_softtimer_create(&my_softtimer))
	 {
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	 }
	 bsp_softtimer_add(&my_softtimer);
	 ret = bsp_softtimer_delete(&my_softtimer);
	 if (ret ==OK)
	 {
	    ret = bsp_softtimer_free(&my_softtimer);
	    if (ret !=OK)
	    {
	    		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"softtimer_test_6  free error\n");
			return ERROR;
	    }
	    
	}
	else if(NOT_ACTIVE == ret)
	{
		osl_sem_down(&sem1);
		osl_sema_delete(&sem1);
	}
	return OK;
}
/*测试唤醒源软timer申请和加入链表,LOOP模式，执行一段时间后在回调函数外delete*/
s32 st_softtimer_test_case07(void)
{
	s32 ret=0;
	
	my_softtimer.func = for_softtimer_test_8;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_WAKE;
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	delay(200);
	ret = bsp_softtimer_delete_sync(&my_softtimer);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"bsp_softtimer_delete_sync failed\n");
		return ERROR;
	}
	ret = bsp_softtimer_free(&my_softtimer);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"bsp_softtimer_free failed\n");
		return ERROR;
	}
	
	return OK;	
}

/*测试非唤醒源软timer申请和加入链表,在回调函数释放*/
s32 st_softtimer_test_case08(void)
{
	
	my_softtimer.func = for_softtimer_test_1;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	osl_sem_init(0,&sem1);
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem1);
	osl_sema_delete(&sem1);
	return OK;
}
/*测试非唤醒源软timer申请和加入链表,重复执行十次后释放*/
s32 st_softtimer_test_case09(void)
{
	
	my_softtimer.func = for_softtimer_test_2;	
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	osl_sem_init(0,&sem2);
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem2);
	osl_sema_delete(&sem2);
	return OK;
}

/*测试非唤醒源软timer申请和加入链表重复10次，每次都在回调函数修改超时时间*/
s32 st_softtimer_test_case10(void)
{
	
	my_softtimer.func = for_softtimer_test_3;	
	my_softtimer.para = 2;
	my_softtimer.timeout = 10;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	osl_sem_init(0,&sem3);
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	osl_sem_down(&sem3);
	osl_sema_delete(&sem3);
	return OK;
}

/*测试非唤醒源软timer申请和加入链表*/
s32 st_softtimer_test_case11(void)
{
	
	my_softtimer.func = for_softtimer_test_1;	
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	my_softtimer.para = 1;
	my_softtimer.timeout = 100;
	osl_sem_init(0,&sem1);
	osl_sem_init(0,&sem5);
	osl_sem_init(0,&sem6);
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	my_sec_softtimer.func = for_softtimer_test_5;
	my_sec_softtimer.para = 2;
	my_sec_softtimer.timeout = 200;
	if (bsp_softtimer_create(&my_sec_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_sec_softtimer);
	my_thir_softtimer.func = for_softtimer_test_6;
	my_thir_softtimer.para = 3;
	my_thir_softtimer.timeout = 300;
	if (bsp_softtimer_create(&my_thir_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_thir_softtimer);
	osl_sem_down(&sem1);
	osl_sem_down(&sem5);
	osl_sem_down(&sem6);
	osl_sema_delete(&sem1);
	osl_sema_delete(&sem5);
	osl_sema_delete(&sem6);
	return OK;
}

/*测试非唤醒源软timer申请和加入链表，在回调函数外删除软timer
   超时时间较短*/
s32 st_softtimer_test_case12(void)
{
	s32 ret=0;
	
	my_softtimer.func = for_softtimer_test_1;	
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	osl_sem_init(0,&sem1);
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	ret = bsp_softtimer_delete(&my_softtimer);
	if (ret ==OK)
	{
		ret = bsp_softtimer_free(&my_softtimer);
		if (ret !=OK)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"softtimer_test_14  free error\n");
			return ERROR;
		} 
		
	}
	else if(NOT_ACTIVE == ret)
	{
		osl_sem_down(&sem1);
		osl_sema_delete(&sem1);
	}
	return OK;	
}
/*测试非唤醒源软timer申请和加入链表，在回调函数外删除软timer
   超时时间较长*/
s32 st_softtimer_test_case13(void)
{
	s32 ret= 0;
	
	my_softtimer.func = for_softtimer_test_1;
	my_softtimer.para = 123;
	my_softtimer.timeout = 100;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	osl_sem_init(0,&sem1);
	
	 if (bsp_softtimer_create(&my_softtimer))
	   {
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error \n");
	    }
	 bsp_softtimer_add(&my_softtimer);
	ret = bsp_softtimer_delete(&my_softtimer);
	if (ret ==OK)
	{
	   ret = bsp_softtimer_free(&my_softtimer);
	   if (ret !=OK)
	   {
	   	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"softtimer_test_6  free error\n");
		return ERROR;
	   }  
	   
	}
	else if(NOT_ACTIVE == ret)
	{
		osl_sem_down(&sem1);
		osl_sema_delete(&sem1);
	}
	return ret;	
}
/*测试非唤醒源软timer申请和加入链表,LOOP模式，执行一段时间后在回调函数外delete*/
s32 st_softtimer_test_case14(void)
{
	s32 ret=0;
	
	my_softtimer.func = for_softtimer_test_8;
	my_softtimer.para = 123;
	my_softtimer.timeout = 1;
	my_softtimer.wake_type=SOFTTIMER_NOWAKE;
	
	if (bsp_softtimer_create(&my_softtimer))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error\n");
		return ERROR;
	}
	bsp_softtimer_add(&my_softtimer);
	delay(200);
	ret = bsp_softtimer_delete_sync(&my_softtimer);
	if(OK!=ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"bsp_softtimer_delete_sync failed\n");
		return ERROR;
	}
	ret = bsp_softtimer_free(&my_softtimer);
	 if (ret !=OK)
	 {
	 	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case16  free error\n");
		return ERROR;
	 }
	 
	 return OK;
}
/*lint !e52*/

void st_softtimer_test_case15(void)
{
	u32 i=0;
	
	
	for(i=0;i<40;i++)/*lint !e52 */
	{
		myown_timer[i].func = for_softtimer_test_7;
		myown_timer[i].wake_type=SOFTTIMER_NOWAKE;
		myown_timer[i].para = i;
		myown_timer[i].timeout = 1000+i*1000;
		if (bsp_softtimer_create(&myown_timer[i]))
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error i =%d\n",i);
			return ;
		}
		bsp_softtimer_add(&myown_timer[i]);
	}	
}

void st_softtimer_test_case16(void)
{
	u32 i=0;
	for(i=0;i<40;i++)/*lint !e52 */
	{
		myown_timer[i].func = for_softtimer_test_7;
		myown_timer[i].wake_type=SOFTTIMER_WAKE;
		myown_timer[i].para = i;
		myown_timer[i].timeout = 1000+i*1000;
		if (bsp_softtimer_create(&myown_timer[i]))
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"create error i =%d\n",i);
			return ;
		}
		bsp_softtimer_add(&myown_timer[i]);
	}	
}

/*在case15或case16运行的情况下运行本测试用例，测试是否可以删除多个定时器*/
s32 st_softtimer_test_case17(void)
{
    int i = 0;
    int ret;
    while(i<31)
    {
		ret = bsp_softtimer_delete_sync(&myown_timer[i]);
	 	if(OK!=ret)
	 	{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"bsp_softtimer_delete_sync failed\n");
			return ERROR;
	 	}
	 	ret = bsp_softtimer_free(&myown_timer[i]);
	  	if(OK!=ret)
	 	{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"bsp_softtimer_free failed\n");
			return ERROR;
	 	}
	 	i++;
    }
    return OK;
}

s32 softtimer_test(void)
{
	s32 ret = 0;
	ret = st_softtimer_test_case01();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case01 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case01 fail\n");
	ret = st_softtimer_test_case02();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case02 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case02 fail\n");
	ret = st_softtimer_test_case03();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case03 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case03 fail\n");
	ret = st_softtimer_test_case04();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case04 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case04 fail\n");
	ret = st_softtimer_test_case05();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case05 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case05 fail\n");
	ret = st_softtimer_test_case06();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case06 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case06 fail\n");
	ret = st_softtimer_test_case07();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case07 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case07 fail\n");
	ret = st_softtimer_test_case08();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case08 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case08 fail\n");
	ret = st_softtimer_test_case09();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case09 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case09 fail\n");
	ret = st_softtimer_test_case10();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case10 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case10 fail\n");
	ret = st_softtimer_test_case11();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case11 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case11 fail\n");
	ret = st_softtimer_test_case12();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case12 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case12 fail\n");
	ret = st_softtimer_test_case13();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case13 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case13 fail\n");
	ret = st_softtimer_test_case14();
	if(OK==ret)
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case14 pass\n");
	else
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SOFTTIMER,"st_softtimer_test_case14 fail\n");
	return OK;
}
