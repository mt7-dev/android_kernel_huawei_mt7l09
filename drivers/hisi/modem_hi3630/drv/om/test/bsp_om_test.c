/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "bsp_om_api.h"
#include "bsp_om.h"

#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/kthread.h>


#define    THIS_MODU_ID        BSP_MODU_TEST

#define bsp_om_test_log(log_level,mod_id,fmt, ...) \
bsp_trace(log_level,mod_id,"<%s>: "fmt"\n", __FUNCTION__, ##__VA_ARGS__)

int g_test_om_init = 0;
int g_test_task_001_times = 0;
int g_test_task_002_times = 10;

u32 *p_test01 = 0;
u32 *p_test02 = 0;
u32 *p_test03 = 0;
u32 addr_test03 = 0;


void test_socp_coder_dest_rcv(void)
{

}

void bsp_om_init_test(void)
{
    if(0 == g_test_om_init)
    {
        /* register socp coder dest recv func*/

    }
}


/*LOG_TEST*/
u32 bsp_log_level_set(bsp_log_level_e log_level);


void log_test_task001(void)
{
    u32 start_time = 0;
    u32 end_time = 0;

    u32 start_time1 = 0;
    u32 end_time1 = 0;

    msleep(60000);

    bsp_log_level_set(0);

    start_time  = om_timer_get();
    for(;;)
    {
         start_time1  = om_timer_get();

        bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_trace(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_trace(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_trace(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_trace(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");

        bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_trace(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_trace(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_trace(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_trace(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");

        bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_trace(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_trace(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_trace(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_trace(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");

        bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_trace(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_trace(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_trace(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_trace(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");

        end_time1  = om_timer_get();

        g_test_task_001_times++;

        //msleep(10);

        if(g_test_task_001_times >=128)
        {
            end_time  = om_timer_get();

            bsp_om_debug(BSP_LOG_LEVEL_DEBUG,"&& log tast end: start_time = 0x%x,end_time = 0x%x",start_time,end_time);
            bsp_om_debug(BSP_LOG_LEVEL_DEBUG,"&& log tast end: start_time1 = 0x%x,end_time1 = 0x%x",start_time1,end_time1);
            return;
        }

    }
}

int log_test_task002(void * para)
{
    unsigned long value = 0;
    u32 temp = 0;

    for(;;)
    {
        local_irq_save(value);

        temp = temp*3;

        if(temp >= 0x10000)
        {
            temp = temp/3;
        }

        local_irq_restore(value);

        msleep(10);
    }

    return 0;
}


void log_test1(u32 times)
{
    u32 i;

    for(i = 0;i< times;i++)
    {
        bsp_om_test_log(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug : %d \n",BSP_LOG_LEVEL_DEBUG);
        bsp_om_test_log(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info : %d \n",BSP_LOG_LEVEL_INFO);
        bsp_om_test_log(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic : %d \n",BSP_LOG_LEVEL_NOTICE);
        bsp_om_test_log(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning: %d \n",BSP_LOG_LEVEL_WARNING);
        bsp_om_test_log(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error: %d \n",BSP_LOG_LEVEL_ERROR);
        bsp_om_test_log(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit : %d \n",BSP_LOG_LEVEL_CRIT);
        bsp_om_test_log(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert : %d \n",BSP_LOG_LEVEL_ALERT);
        bsp_om_test_log(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal : %d \n",BSP_LOG_LEVEL_FATAL);

        bsp_om_test_log(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_om_test_log(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_om_test_log(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_om_test_log(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_om_test_log(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_om_test_log(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_om_test_log(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_om_test_log(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");
    }



}


void log_test3(bsp_log_level_e log_level)
{
    bsp_om_test_log(BSP_LOG_LEVEL_DEBUG,BSP_MODU_ICC,"log_test1_debug\n");
}


void log_test2(void)
{

    //kthread_run(log_test_task001, NULL, "log_task001");
    (void)kthread_run(log_test_task002, NULL, "log_task002");

}


void mem_test3(u32 test_type)
{
    if(0== test_type)
    {

    }
    else if(1 == test_type)
    {

    }
    else if(2 == test_type)
    {

    }
}

void error_log_test(void)
{
    error_log("error_log_test: %d %d %d %s %d\n", 0xaa, 0xbb, 0xcc, __FUNCTION__, __LINE__);
}

void bsp_ind_test(void)
{
    u8 array[20] = {1,2,3,4,5,6,7,8,};

    bsp_log_bin_ind(0x5220,array,8);
}





