/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e537*/
#include "bsp_om_api.h"
#include "bsp_om_sysview.h"
#include "bsp_om_server.h"
#include "bsp_softtimer.h"
#include "drv_dump.h"
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include "osl_malloc.h"
/*lint -restore*/

extern int task_switch_hook_del ( FUNC_VOID switchHook);

u32 g_cpu_task_taet = 0;


BSP_TASK_CPU_INFO_STRU      g_om_cpu_trace[BSP_OM_MAX_TASK_NUM];
u32                     g_task_num = 0;
BSP_SYS_CPU_INFO_CTRL_STRU      cpu_info_stru = {{(softtimer_func)NULL,0,0,SOFTTIMER_NOWAKE,{NULL,NULL},0,0,0,0},0,0};

void*         kernel_tid_list[BSP_OM_MAX_TASK_NUM] = {0};

u32         old_slice           = 0;
u32         cpu_state           = 0;
u32         cmd_start_slice = 0;

void cpu_task_swt_hook(void *old_tcb, void *new_tcb)
{
    u32  current_slice;
    u32  interval_slice;
    int  suffix;
    struct task_struct *pTid;

    /* get current time */
    current_slice = om_timer_get();

    /* timer expire */
    if (old_slice > current_slice)
    {
        interval_slice = (TIMER_MAX_VALUE - old_slice) + current_slice;
    }
    else
    {
        interval_slice = current_slice - old_slice;
    }

    /* sub interrupt's time */
    pTid = ( struct task_struct *)old_tcb;

    suffix = pTid->suffix;

    if((suffix >= 0)&&(suffix < BSP_OM_MAX_TASK_NUM))
    {
        /* Max slice */
        if( g_om_cpu_trace[suffix].max_slice< interval_slice )
        {
            g_om_cpu_trace[suffix].max_slice = interval_slice;
        }

        /* Min slice */
        if( g_om_cpu_trace[suffix].min_slice> interval_slice )
        {
            g_om_cpu_trace[suffix].min_slice = interval_slice;
        }

        g_om_cpu_trace[suffix].slices += interval_slice;
        g_om_cpu_trace[suffix].SwitchNum ++;
    }

    /* get the time that a new task begin to run */
    old_slice = current_slice;
}


void cpu_view_report_init(void)
{
    struct task_struct     *tempPtr;
    u32         startslice = 0;
    int task_num = 0x00;
    struct task_struct *pTid;

    for_each_process(pTid)
    {
        if(task_num >=  BSP_OM_MAX_TASK_NUM)
        {
            break;
        }

        kernel_tid_list[task_num]= (void*)(pTid);

        tempPtr = (struct task_struct *)(kernel_tid_list[task_num]);
        tempPtr->suffix = task_num;
        strncpy((char *)g_om_cpu_trace[task_num].task_name, (const char *)tempPtr->comm, BSP_SYSVIEW_TASK_NAME_LEN);
        g_om_cpu_trace[task_num].slices = 0;
        g_om_cpu_trace[task_num].max_slice = 0;
        g_om_cpu_trace[task_num].min_slice = 0xffffffff;
        task_num++;
    }
    g_task_num = (u32)task_num;

    if (0 == g_task_num)
    {
        return;
    }

    /*Init Task/interrupt Switch Hook*/
    if ( ERROR == task_switch_hook_add( (FUNC_VOID)cpu_task_swt_hook ) )
    {
        printk("CPU_utilization_Init fail.\r\n");
        return;
    }

    startslice = om_timer_get();

    old_slice = startslice;

    cpu_state = 1;

    return;
}

 u32 cpu_utilization_start(void)
{
    int          i;

    if (1 != cpu_state)
    {
        return (u32)-1;
    }

    cmd_start_slice = om_timer_get();


    for ( i=0; i<BSP_OM_MAX_TASK_NUM; i++ )
    {
        g_om_cpu_trace[i].cmdslice = g_om_cpu_trace[i].slices;
        g_om_cpu_trace[i].SwitchNum = 0;
    }

    return BSP_OK;
}

u32 cpu_utilization_end(u8 *pTaskName, double *CPUusage)
{

    /*remove task and interrupt switch hook*/
    if ( ERROR == task_switch_hook_del( (FUNC_VOID)cpu_task_swt_hook ) )
    {
        return (u32)-1;
    }

    return BSP_OK;
}


/*****************************************************************************
* 函 数 名  : get_task_time
*
* 功能描述  :记录当前时刻各个任务已使用的时间片
*
* 输入参数  :start_end_flag :标示记录周期开始或者是结束
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 get_task_time(u32 start_end_flag)
{
    u32 ret = BSP_OK;

    if(start_end_flag)
    {
        ret = cpu_utilization_end(0, 0);
        if(BSP_OK != ret)
        {
            return ret;
        }
    }
    else
    {
        cpu_view_report_init();

        ret = cpu_utilization_start();
        if(BSP_OK != ret)
        {
            return ret;
        }
    }

    return ret;

}

#define PID_PPID_GET(taskTCB)  ((((struct task_struct *)taskTCB)->pid & 0xffff)| \
                                 ((((struct task_struct *)taskTCB)->real_parent->pid & 0xffff)<< 16))


/*****************************************************************************
* 函 数 名  : report_cpu_trace
*
* 功能描述  :上报当前周期内cpu占用率信息
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 report_cpu_trace(void)
{
    u32 task_num = 0;
    BSP_CPU_TRACE_STRU  *p_cpu_trace_stru = NULL;
    BSP_TASK_CPU_TRACE_STRU *p_cpu_trace_data = NULL;
    u32          buf_len = 0;
    u32          task_slices = 0;


    buf_len = sizeof(BSP_CPU_TRACE_STRU) + sizeof(BSP_TASK_CPU_TRACE_STRU)*g_task_num;

    if(cpu_info_stru.report_swt == BSP_SYSVIEW_SWT_ON)
    {
        if(bsp_om_buf_sem_take())
        {
            return BSP_ERR_SYSVIEW_FAIL;
        }
        p_cpu_trace_stru = (BSP_CPU_TRACE_STRU *)bsp_om_get_buf(BSP_OM_SOCP_BUF_TYPE,buf_len);
         if(NULL == p_cpu_trace_stru)
        {
            bsp_om_buf_sem_give();
            return BSP_ERR_SYSVIEW_MALLOC_FAIL;
        }
    }
    else
    {
        p_cpu_trace_stru = (BSP_CPU_TRACE_STRU *)osl_malloc(buf_len);
         if(NULL == p_cpu_trace_stru)
        {
            return BSP_ERR_SYSVIEW_MALLOC_FAIL;
        }
    }

#ifdef ENABLE_BUILD_SYSVIEW
    sysview_trace_packet((u8*)p_cpu_trace_stru,buf_len,BSP_SYSVIEW_CPU_INFO);
#endif

    p_cpu_trace_data = p_cpu_trace_stru->cpu_info_stru;

    for(task_num =0;task_num < g_task_num; task_num++)
    {
        /*lint -save -e701*/
        p_cpu_trace_data[task_num].task_id =  PID_PPID_GET(kernel_tid_list[task_num]);
        /*lint -restore*/
        strncpy( (char *)(p_cpu_trace_data[task_num].task_name), (char *)(g_om_cpu_trace[task_num].task_name), BSP_SYSVIEW_TASK_NAME_LEN);
        p_cpu_trace_data[task_num].task_name[BSP_SYSVIEW_TASK_NAME_LEN-1] = '\0';
        task_slices =g_om_cpu_trace[task_num].slices - g_om_cpu_trace[task_num].cmdslice;
        p_cpu_trace_data[task_num].interval_slice = task_slices;

        /* for uart debug begin*/
        if(cpu_info_stru.report_swt == BSP_SYSVIEW_SWT_OFF)
        {
            if(task_slices != 0)
            {
                printk("taskid:0x%x,  %-11s,  rate = %u%% \n"
                                        ,p_cpu_trace_data[task_num].task_id,p_cpu_trace_data[task_num].task_name
                                        ,(u32)((task_slices *100)/g_cpu_task_taet));
            }
        }
        /* for uart debug end*/
    }

    if(cpu_info_stru.report_swt == BSP_SYSVIEW_SWT_ON)
    {
        if( BSP_OK != bsp_om_into_send_list(p_cpu_trace_stru,buf_len))
        {
            bsp_om_free_buf(p_cpu_trace_stru,buf_len );
            bsp_om_buf_sem_give();
            return BSP_ERR_SYSVIEW_FAIL;
        }

        bsp_om_buf_sem_give();
    }
    else
    {
        osl_free(p_cpu_trace_stru);
    }

    return BSP_OK;
}



void  cpu_timeout_done(void)
{

        get_task_time(1);

        if(cpu_info_stru.report_swt  == BSP_SYSVIEW_SWT_ON)
        {
            if( TRUE == bsp_om_get_hso_conn_flag())
            {
                report_cpu_trace();
            }

            get_task_time(0);

            bsp_softtimer_add(&cpu_info_stru.loop_timer);
        }

}




u32 bsp_cpu_swt_set(u32 set_swt,u32 period)
{
    u32 ret = BSP_OK;

    if(BSP_SYSVIEW_SWT_ON == set_swt)
    {
        if(cpu_info_stru.loop_timer.init_flags == TIMER_INIT_FLAG )
        {
            bsp_om_stop_timer(&cpu_info_stru.loop_timer);
            bsp_softtimer_modify(&cpu_info_stru.loop_timer,period*1000);
            bsp_softtimer_add(&cpu_info_stru.loop_timer);
            /* 初始化计数信息*/
            get_task_time(0);

            ret = BSP_OK;
        }
        else
        {
            ret = (u32)bsp_om_start_timer(period,cpu_timeout_done , 0, &(cpu_info_stru.loop_timer));

            if(ret == BSP_OK)
            {
                 /* 初始化计数信息*/
                get_task_time(0);
                ret = BSP_OK;
            }
            else
            {
                ret = (u32)BSP_ERROR;
            }
        }
    }
    else
    {
        if(cpu_info_stru.loop_timer.init_flags == TIMER_INIT_FLAG )
        {
            ret = (u32)bsp_om_stop_timer(&cpu_info_stru.loop_timer);
        }
        else
        {
            ret = BSP_OK;
        }

         /*去 初始化计数信息*/
        get_task_time(1);
    }

    if(BSP_OK != ret)
    {
        return ret;
    }

    cpu_info_stru.report_swt  = set_swt;
    cpu_info_stru.report_timer_len = period;
    return BSP_OK;
}



void  cpu_task_test(u32 secends)
{
        g_cpu_task_taet = secends*32*1024;

        get_task_time(0);

        msleep(secends *1000);
        get_task_time(1);

        report_cpu_trace();


}


