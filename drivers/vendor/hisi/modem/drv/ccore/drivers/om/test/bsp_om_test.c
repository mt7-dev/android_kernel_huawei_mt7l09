/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "bsp_om_api.h"
#include "bsp_om.h"

#define    THIS_MODU_ID        BSP_MODU_TEST
#if 1
#define bsp_om_test_log(log_level,mod_id,fmt, ...) \
bsp_trace(log_level,mod_id,"<%s>: "fmt"\n", __FUNCTION__, ##__VA_ARGS__)

#else
#define bsp_om_test_log(log_level,mod_id,fmt, ...) \
logMsg("[om_test]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__)
#endif

int g_test_om_init = 0;
int g_test_task_001_times = 10;
int g_test_task_002_times = 10;

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

void log_test_task001(void)
{
    u32 start_time = 0;
    u32 end_time = 0;

    u32 start_time1 = 0;
    u32 end_time1 = 0;

    taskDelay(6000);

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


void log_test_task002(void)
{
    for(;;)
    {
        bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_LOG,"log_test1_debug\n");
        bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_LOG,"log_test1_info\n");
        bsp_trace(BSP_LOG_LEVEL_NOTICE,BSP_MODU_LOG,"log_test1_notic\n");
        bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_LOG,"log_test1_warning\n");
        bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_LOG,"log_test1_error\n");
        bsp_trace(BSP_LOG_LEVEL_CRIT,BSP_MODU_LOG,"log_test1_crit\n");
        bsp_trace(BSP_LOG_LEVEL_ALERT,BSP_MODU_LOG,"log_test1_alert\n");
        bsp_trace(BSP_LOG_LEVEL_FATAL,BSP_MODU_LOG,"log_test1_fatal\n");

        g_test_task_002_times--;

        if(g_test_task_002_times <= 0)
        {
            return;
        }

        /*taskDelay(50);*/
    }
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


u32 g_server_buf_add[1024] = {0};


void om_get_buf_test(u32 len)
{
    u32 i;
    u32 ok_times = 0;

    memset(g_server_buf_add,0,sizeof(u32)*1024);

    for(i = 0;i< 1024;i++)
    {
        if( 0 == len)
        {
            g_server_buf_add[i] = bsp_om_get_buf(0,i+1);
        }
        else
        {
            g_server_buf_add[i] = bsp_om_get_buf(0,len);
        }

        if(g_server_buf_add[i] != 0)
        {
            printf("g_server_buf_add[i] = 0x%x\n",g_server_buf_add[i]);
            ok_times++;
        }
    }

    printf("ok_times = %d\n",ok_times);
}

void om_free_buf_test(u32 len)
{
    u32 i;
    u32 ret;
    u32 ok_times = 0;

    for(i = 0;i< 1024;i++)
    {
        if( 0 != g_server_buf_add[i])
        {
            if( 0 == len)
            {
                 ret = bsp_om_free_buf(g_server_buf_add[i],i+1);
            }
            else
            {
                 ret = bsp_om_free_buf(g_server_buf_add[i],len);
            }

            if(ret == 0)
            {
                ok_times++;
            }
        }
    }

    printf("ok_times = %d\n",ok_times);
}


void log_test2(void)
{
    bsp_log_level_set(0);

    //kthread_run(log_test_task001, NULL, "log_test_task001");
    //taskSpawn("log_test_task001", 130, 0, 0x2000, log_test_task001, 0, 0, 0, 0,0, 0, 0, 0, 0, 0);

}

u32 *p_test01 = 0;
u32 *p_test02 = 0;
u32 *p_test03 = 0;
u32 addr_test03 = 0;


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

void bsp_ind_test(void)
{
    u8 array[20] = {1,2,3,4,5,6,7,8,};

    bsp_log_bin_ind(0x5220,array,8);
}





