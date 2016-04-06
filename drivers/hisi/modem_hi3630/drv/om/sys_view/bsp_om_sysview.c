
/*lint -save -e767*/
#define    THIS_MODU_ID        BSP_MODU_SYSVIEW
/*lint -restore +e767*/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e537*/
#include "bsp_om_api.h"
#include "bsp_om_sysview.h"
#include "bsp_dump.h"
#include "bsp_dump_drv.h"
#include "bsp_softtimer.h"
#include "bsp_memmap.h"
#include "bsp_om_server.h"
#include "bsp_socp.h"
#include "bsp_bbp.h"
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/semaphore.h>
/*lint -restore*/

#ifndef SEM_Q_FIFO
#define SEM_Q_FIFO          (1)
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

BSP_SYSVIEW_DEBUG_STRU  g_sysview_debug[BSP_SYSVIEW_SWT_ALL] = {{0}};
extern BSP_MEM_TRACE_STRU          g_om_mem_trace;

extern BSP_TASK_INT_INFO_CTRL_STRU      task_info_stru ;
extern BSP_TASK_INT_INFO_CTRL_STRU      int_lock_stru ;


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : sysview_trace_packet
*
* 功能描述  :任务、中断切换信息，锁中断信息上报信息头填充
*
* 输入参数  : trace_buf :buf指针
*                         buf_size: buf长度
*                         data_type : 上报类型
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

void sysview_trace_packet(u8 *trace_buf,u32 buf_size,bsp_sysview_type_e data_type)
{
    bsp_om_head_s        *bsp_om_header  = NULL;
    BSP_SYS_VIEW_HEADER_STRU     *sysview_info_header = NULL;

    PACKET_BSP_SOCP_HEAD(trace_buf);;

    bsp_om_header = (bsp_om_head_s*)(trace_buf + sizeof(bsp_socp_head_s));
    sysview_info_header = (BSP_SYS_VIEW_HEADER_STRU *)((char*)trace_buf + sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s));

    bsp_om_header->data_size        =buf_size - sizeof(bsp_socp_head_s)- sizeof(bsp_om_head_s);
    /*lint -save -e648*/
    if(BSP_SYSVIEW_TASK_INFO == data_type)
    {
        bsp_om_header->om_id            = (BSP_STRU_ID_28_31_GROUP_BSP << 28) | (BSP_STRU_ID_16_23_BSP_TASK << 16)|CMD_BSP_SYSVIEW_IND_ACORE;
        sysview_info_header->sn              = g_om_global_info.task_info_sn++;
    }
    else if(BSP_SYSVIEW_INT_LOCK_INFO== data_type)
    {
        bsp_om_header->om_id            = (BSP_STRU_ID_28_31_GROUP_BSP << 28) | (BSP_STRU_ID_16_23_BSP_INT_LOCK << 16)|CMD_BSP_SYSVIEW_IND_ACORE;
        sysview_info_header->sn              = g_om_global_info.int_lock_info_sn++;
    }
    else if(BSP_SYSVIEW_MEM_TRACE== data_type)
    {
        bsp_om_header->om_id            = (BSP_STRU_ID_28_31_GROUP_BSP << 28) | (BSP_STRU_ID_16_23_BSP_MEM<< 16)|CMD_BSP_SYSVIEW_IND_ACORE;
        sysview_info_header->sn              = g_om_global_info.mem_info_sn++;
    }
    else if(BSP_SYSVIEW_CPU_INFO== data_type)
    {
        bsp_om_header->om_id            = (BSP_STRU_ID_28_31_GROUP_BSP << 28) | (BSP_STRU_ID_16_23_BSP_CPU<< 16)|CMD_BSP_SYSVIEW_IND_ACORE;
        sysview_info_header->sn              = g_om_global_info.cpu_info_sn++;
    }
    else
    {
        /* 内部接口，由入参保证*/
    }
    /*lint -restore +e648*/
    return ;
}


/*****************************************************************************
* 函 数 名  : bsp_om_sysview_swt_reset
*
* 功能描述  : 重置所有sysview trace上报开关
*
* 输入参数  : 无
*
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_om_sysview_swt_reset(void)
{
   bsp_task_swt_set(BSP_SYSVIEW_SWT_OFF,0);
    bsp_int_lock_set(BSP_SYSVIEW_SWT_OFF,0);
    bsp_mem_swt_set(BSP_SYSVIEW_SWT_OFF,0);
    bsp_cpu_swt_set(BSP_SYSVIEW_SWT_OFF,0);
}


/*****************************************************************************
* 函 数 名  : bsp_sysview_swt_set
*
* 功能描述  : 设置sysview上报开关和上报周期
*
* 输入参数  : tarce_type :trace类型
*                         set_swt  :开关值
*                         period  :上报周期
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

u32 bsp_sysview_swt_set(bsp_sysview_type_e set_type,u32 set_swt,u32 period)
{
    u32         ret = BSP_OK;

    bsp_om_debug(BSP_LOG_LEVEL_DEBUG,"set_type = %d,set_swt = %d,period = %d\n",set_type,set_swt,period);

    switch(set_type)
    {
        case BSP_SYSVIEW_MEM_TRACE:
            ret =  bsp_mem_swt_set(set_swt,period);

            break;
        case BSP_SYSVIEW_CPU_INFO:

            ret =  bsp_cpu_swt_set(set_swt,period);
            break;

        case BSP_SYSVIEW_TASK_INFO:

            ret =  bsp_task_swt_set(set_swt,period);
            break;
         case BSP_SYSVIEW_INT_LOCK_INFO:

             ret =  bsp_int_lock_set(set_swt,period);
            break;

        default:
            ret = BSP_ERR_SYSVIEW_INVALID_PARAM;
            break;
    }

    return ret;
}


#define PID_PPID_GET(taskTCB)  ((((struct task_struct *)taskTCB)->pid & 0xffff)| \
                                 ((((struct task_struct *)taskTCB)->real_parent->pid & 0xffff)<< 16))

u32 sysview_get_all_task_name(void *p_task_stru,u32 param_len)
{
    struct task_struct *pTid = NULL;
    int task_num = 0x00;
    BSP_SYSVIEW_TASK_INFO_STRU *ptask_stru;

    if((param_len/sizeof(BSP_SYSVIEW_TASK_INFO_STRU)) < BSP_OM_MAX_TASK_NUM)
    {
        return BSP_ERR_SYSVIEW_INVALID_PARAM;
    }

    if(p_task_stru == NULL)
    {
        return BSP_ERR_SYSVIEW_INVALID_PARAM;
    }

    /*lint -save -e613 -e701*/
    ptask_stru = (BSP_SYSVIEW_TASK_INFO_STRU*)p_task_stru;
    for_each_process(pTid)
    {
        if(task_num >=  BSP_OM_MAX_TASK_NUM)
        {
            break;
        }

        ptask_stru[task_num].task_id= PID_PPID_GET(pTid);
        strncpy( (char *)(ptask_stru[task_num].name), pTid->comm, BSP_SYSVIEW_TASK_NAME_LEN);
        task_num++;
    }
     /*lint -restore*/

    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : bsp_sysview_swt_get
*
* 功能描述  :
*
* 输入参数  :
*
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

u32 bsp_sysview_swt_get(bsp_sysview_type_e set_type,u32 swt_val)
{
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : sys_view_init
*
* 功能描述  :sysview模块初始化接口
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 sys_view_init(void)
{
    u32 mod ;

    /*初始化mem 记录信息*/
    memset(&g_om_mem_trace,0,sizeof(BSP_MEM_TRACE_STRU));

    /* Mmon*/
    for(mod = 0; mod < BSP_MODU_MAX; mod++)
    {
        bsp_om_init_list_head(&(g_om_mem_trace.module_trace[mod].alloc_trace_list.p_list));
    }

    task_info_stru.threshold_size= DUMP_TASK_SWT_THRESHOLD_SIZE;
    int_lock_stru.threshold_size= DUMP_INT_LOCK_THRESHOLD_SIZE;

    return BSP_OK;
}


void bsp_sysview_debug_show(void)
{
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "task_info_swt =%d\n",task_info_stru.report_swt);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "task_cb_call_times =%d\n",g_sysview_debug[BSP_SYSVIEW_TASK_INFO].cb_call_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "task_timeout_times =%d\n",g_sysview_debug[BSP_SYSVIEW_TASK_INFO].timeout_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "task_info_send_times =%d\n",g_sysview_debug[BSP_SYSVIEW_TASK_INFO].info_send_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "task_info_send_size = %d\n",g_sysview_debug[BSP_SYSVIEW_TASK_INFO].info_send_size);

    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "intlock_info_swt =%d\n",int_lock_stru.report_swt);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "intlock_cb_call_times =%d\n",g_sysview_debug[BSP_SYSVIEW_INT_LOCK_INFO].cb_call_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "intlock_timeout_times =%d\n",g_sysview_debug[BSP_SYSVIEW_INT_LOCK_INFO].timeout_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "intlock_info_send_times =%d\n",g_sysview_debug[BSP_SYSVIEW_INT_LOCK_INFO].info_send_times);
    bsp_om_debug(BSP_LOG_LEVEL_FATAL, "intlock_info_send_size =%d\n",g_sysview_debug[BSP_SYSVIEW_INT_LOCK_INFO].info_send_size);
}


