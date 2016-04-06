/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e537*/
#include "bsp_om_api.h"
#include "bsp_om_sysview.h"
#include "bsp_om_server.h"
#include "bsp_dump.h"
#include "bsp_dump_drv.h"
#include "bsp_memmap.h"
#include "osl_sem.h"
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <drv_comm.h>
/*lint -restore*/

BSP_TASK_INT_INFO_CTRL_STRU      task_info_stru = {0};
BSP_TASK_INT_INFO_CTRL_STRU      int_lock_stru = {0};

osl_sem_id                task_swt_sem;
osl_sem_id                int_lock_sem;


int  QueueFrontDel(dump_queue_t *Q, u32 del_num);

/*****************************************************************************
* 函 数 名  : task_swt_read_cb_done
*
* 功能描述  :任务、中断切换信息触发上报回调函数，注册给dump模块
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

void task_swt_read_cb_done(void)
{
    osl_sem_up(&task_swt_sem);
    g_sysview_debug[BSP_SYSVIEW_TASK_INFO].cb_call_times++;
}


/*****************************************************************************
* 函 数 名  : int_lock_read_cb_done
*
* 功能描述  :锁中断信息触发上报回调函数，注册给dump模块
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

 void int_lock_read_cb_done(void)
{

    osl_sem_up(&int_lock_sem);
    g_sysview_debug[BSP_SYSVIEW_INT_LOCK_INFO].cb_call_times++;
}

/*****************************************************************************
* 函 数 名  : report_sysview_trace
*
* 功能描述  :任务切换、中断切换、锁中断信息上报封装函数
*
* 输入参数  :data_type :上报类型
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

void report_sysview_trace(bsp_sysview_type_e data_type)
{
    dump_queue_t *queue_head = NULL;
    u32 data_size = 0;
    u32 send_buf_len = 0;
    u32 q_front;
    u32 q_maxnum;
    u32 q_num;

    BSP_SYS_VIEW_TRACE_STRU *p_sysview_trace_stru = NULL;
    u32 threshold_size ;

    if( TRUE != bsp_om_get_hso_conn_flag())
    {
        return ;
    }

    if(BSP_SYSVIEW_TASK_INFO == data_type)
    {
        threshold_size  = task_info_stru.threshold_size;
        queue_head = (dump_queue_t *)DUMP_TASK_SWITCH_ADDR;
    }
    /* 内部接口，由入参保证*/
    else
    {
       threshold_size  = int_lock_stru.threshold_size;
       queue_head = (dump_queue_t *)DUMP_INTLOCK_ADDR;
    }

    q_num           = queue_head->num;
    q_maxnum    = queue_head->maxNum;
    q_front           = queue_head->front ;

    if(0 == q_num)
    {
        return ;
    }

    /* 每次触发上报最多上报门限值大小*/

    if((q_num *sizeof(int)) >= threshold_size )
    {
        data_size = threshold_size;
    }
    else
    {
        data_size = (q_num *sizeof(int));
    }

    /*不处理回卷指针*/
    if((q_front + data_size/sizeof(u32)) >=q_maxnum)
    {
        data_size = (q_maxnum - q_front )* sizeof(u32);
    }

    /*申请编码通道缓存buf*/
    send_buf_len = sizeof(BSP_SYS_VIEW_TRACE_STRU)+data_size ;

    if(bsp_om_buf_sem_take())
    {
        return;
    }
    p_sysview_trace_stru = bsp_om_get_buf(BSP_OM_SOCP_BUF_TYPE,send_buf_len );

    if(NULL == p_sysview_trace_stru)
    {
        QueueFrontDel(queue_head,data_size/sizeof(int));
        bsp_om_buf_sem_give();
        return ;
    }

    /*打包数据头*/
    sysview_trace_packet((u8*)p_sysview_trace_stru,send_buf_len,data_type);

    /*从dump模块指定地址中获取数据、发送数据到SOCP、更新dump模块队列指针*/
    memcpy((u8*)p_sysview_trace_stru +sizeof(BSP_SYS_VIEW_TRACE_STRU),(u8*)(queue_head->data+q_front),data_size);

    if(BSP_OK == bsp_om_into_send_list((void*)p_sysview_trace_stru,send_buf_len))
    {
        QueueFrontDel(queue_head,data_size/sizeof(int));
        g_sysview_debug[data_type].info_send_times++;
        g_sysview_debug[data_type].info_send_size += data_size;
    }
    else
    {
        QueueFrontDel(queue_head,data_size/sizeof(int));
        bsp_om_free_buf((void*)p_sysview_trace_stru,send_buf_len );
    }

    bsp_om_buf_sem_give();
    return ;

}


int  task_view_task(void * para)
{
    /* coverity[no_escape] */
    for(;;)
    {
        /* coverity[check_return] */
        osl_sem_downtimeout(&task_swt_sem, 3000);

        if(task_info_stru.report_swt == BSP_SYSVIEW_SWT_ON)
        {
            report_sysview_trace(BSP_SYSVIEW_TASK_INFO);
        }
    }
    /*lint -save -e527*/
    return 0;
    /*lint -restore +e527*/
}

int  int_lock_view_task(void * para)
{
    /* coverity[no_escape] */
    for(;;)
    {
        /* coverity[check_return] */
        osl_sem_downtimeout(&int_lock_sem, 3000);

        if(int_lock_stru.report_swt == BSP_SYSVIEW_SWT_ON)
        {
            report_sysview_trace(BSP_SYSVIEW_INT_LOCK_INFO);
        }

    }
    /*lint -save -e527*/
    return 0;
    /*lint -restore +e527*/
}



u32 bsp_task_swt_set(u32 set_swt,u32 period)
{
    u32 ret = BSP_OK;

    if(BSP_SYSVIEW_SWT_ON == set_swt)
    {

        if(task_info_stru.task_id  != 0 )
        {
            ret = BSP_OK;
        }
        else
        {
            osl_sem_init( SEM_EMPTY,&task_swt_sem);

            task_info_stru.task_id = kthread_run(task_view_task, NULL, "task_view_task");
            if(task_info_stru.task_id != 0)
            {

                ret = BSP_OK;
            }
            else
            {
                ret = (u32)BSP_ERROR;
            }
        }
        /*lint -save -e64*/
        bsp_dump_register_sysview_hook(DUMP_SYSVIEW_TASKSWITCH,(dump_save_hook)task_swt_read_cb_done);
         /*lint -restore +e64*/
    }
    else
    {
        bsp_dump_register_sysview_hook(DUMP_SYSVIEW_TASKSWITCH,NULL);
    }

    if(BSP_OK != ret)
    {
        return ret;
    }

    task_info_stru.report_swt  = set_swt;

    return BSP_OK;
}



u32 bsp_int_lock_set(u32 set_swt,u32 period)
{
    u32 ret =BSP_OK;

    if(BSP_SYSVIEW_SWT_ON == set_swt)
    {
        if(int_lock_stru.task_id  != 0 )
        {
            ret = BSP_OK;
        }
        else
        {
            osl_sem_init(SEM_EMPTY,&int_lock_sem);

            int_lock_stru.task_id = kthread_run(int_lock_view_task, NULL, "int_lock_task");

            if(int_lock_stru.task_id != 0)
            {
                ret = BSP_OK;
            }
            else
            {
                ret = (u32)BSP_ERROR;
            }
        }
        /*lint -save -e64*/
        bsp_dump_register_sysview_hook(DUMP_SYSVIEW_INTLOCK,(dump_save_hook)int_lock_read_cb_done);
        /*lint -restore +e64*/
    }
    else
    {
        bsp_dump_register_sysview_hook(DUMP_SYSVIEW_INTLOCK,NULL);
    }

    if(BSP_OK != ret)
    {
        return ret;
    }

    int_lock_stru.report_swt  = set_swt;
    int_lock_stru.report_timer_len = BSP_TASK_INT_REPORT_PERIOD;

    return BSP_OK;
}




