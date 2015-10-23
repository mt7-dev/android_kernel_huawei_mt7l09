/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e537*/
#include <stdio.h>
#include <stdlib.h>
#include <taskLib.h>
#include <usrLib.h>
#include <logLib.h>
#include <osl_thread.h>
#include "bsp_softtimer.h"
#include "bsp_om_api.h"
#include "bsp_om_sysview.h"
/*lint -restore*/

/*lint -save -e156*/
BSP_SYS_MEM_INFO_CTRL_STRU      mem_info_stru={{(softtimer_func)NULL,0,0,SOFTTIMER_NOWAKE,{NULL,NULL},0,0,0,0},0,0};
/*lint -restore +e156*/
BSP_MEM_TRACE_STRU          g_om_mem_trace ;

u32 mem_free_trace_node(bsp_module_e modu_id,u32 addr);
u32 mem_free_trace(bsp_module_e modu_id,u32 addr);
u32 report_mem_trace(void);
void* bsp_om_mem_alloc(bsp_module_e modu_id,u32 size,u32 flag);
void bsp_om_mem_free(bsp_module_e modu_id,u32 addr);


/*****************************************************************************
* 函 数 名  : mem_alloc_trace
*
* 功能描述  :malloc操作trace记录
*
* 输入参数  :modu_id :模块ID
*                        size :申请的内存长度
*                        addr :申请的内存地址
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
/*lint -save -e429*/
static u32 mem_alloc_trace(bsp_module_e modu_id,u32 size,u32 addr)
{
    BSP_MEM_ALLOC_TRACE_STRU     *p_alloc_trace     = NULL;

    if((modu_id >= BSP_MODU_MAX )||( BSP_MODU_SYSVIEW == modu_id))
    {
        return BSP_ERR_SYSVIEW_INVALID_MODULE;
    }
#ifdef __KERNEL__

    p_alloc_trace = kmalloc(sizeof(BSP_MEM_ALLOC_TRACE_STRU),GFP_KERNEL);
#else
    p_alloc_trace = malloc(sizeof(BSP_MEM_ALLOC_TRACE_STRU));
#endif
    if( NULL == p_alloc_trace)
    {
        return BSP_ERR_SYSVIEW_MALLOC_FAIL;
    }

    /* BSP_MODU_SYSVIEW 模块申请的内存只记录总大小和次数*/
    g_om_mem_trace.module_trace[BSP_MODU_SYSVIEW].alloc_times++;
    g_om_mem_trace.module_trace[BSP_MODU_SYSVIEW].alloc_size += sizeof(BSP_MEM_ALLOC_TRACE_STRU);
    g_om_mem_trace.module_trace[BSP_MODU_SYSVIEW].use_mem_size += sizeof(BSP_MEM_ALLOC_TRACE_STRU);

    p_alloc_trace->alloc_addr = addr;
    p_alloc_trace->len        = size;

    g_om_mem_trace.module_trace[modu_id].alloc_size += size;
    g_om_mem_trace.module_trace[modu_id].alloc_times++;
    g_om_mem_trace.module_trace[modu_id].use_mem_size += size;

    list_add_tail(&(p_alloc_trace->p_list),&(g_om_mem_trace.module_trace[modu_id].alloc_trace_list.p_list));

    return BSP_OK;
}
/*lint -restore +e429*/

/*****************************************************************************
* 函 数 名  : mem_free_trace_node
*
* 功能描述  :删除malloc trace链表中的节点
*
* 输入参数  :modu_id :模块ID
*                        addr :free的内存地址
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 mem_free_trace_node(bsp_module_e modu_id,u32 addr)
{
    struct list_head       *me;
    BSP_MEM_ALLOC_TRACE_STRU        *p_alloc_trace     = NULL;
   
    bsp_om_list_for_each(me, (struct list_head *)&(g_om_mem_trace.module_trace[modu_id].alloc_trace_list.p_list))
    {
        p_alloc_trace  = bsp_om_list_entry(me,BSP_MEM_ALLOC_TRACE_STRU,p_list);

        if(addr ==  p_alloc_trace->alloc_addr)
        {
            g_om_mem_trace.module_trace[modu_id].free_size +=  p_alloc_trace->len;
            g_om_mem_trace.module_trace[modu_id].free_times++;
            g_om_mem_trace.module_trace[modu_id].use_mem_size  -=p_alloc_trace->len;

            list_del(&(p_alloc_trace->p_list));
#ifdef __KERNEL__
            kfree(p_alloc_trace);
#else
            free(p_alloc_trace);
#endif

            g_om_mem_trace.module_trace[BSP_MODU_SYSVIEW].free_times++;
            g_om_mem_trace.module_trace[BSP_MODU_SYSVIEW].free_size += sizeof(BSP_MEM_ALLOC_TRACE_STRU);
            g_om_mem_trace.module_trace[modu_id].use_mem_size  -=sizeof(BSP_MEM_ALLOC_TRACE_STRU);

            return BSP_OK;
        }

    }

    return BSP_ERR_SYSVIEW_NO_BUF;
}

/*****************************************************************************
* 函 数 名  : mem_free_trace
*
* 功能描述  :free操作trace记录
*
* 输入参数  :modu_id :模块ID
*                        addr :free的内存地址
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 mem_free_trace(bsp_module_e modu_id,u32 addr)
{
    u32         mod = 0;

    if((modu_id >= BSP_MODU_MAX )||( BSP_MODU_SYSVIEW == modu_id))
    {
        return BSP_ERR_SYSVIEW_INVALID_MODULE;
    }

    /* 优先在 modu_id 模块对应的列表中查找*/
    if(BSP_OK == mem_free_trace_node(modu_id,addr))
    {
        return BSP_OK;
    }

    /* 如果在当前的mod列表中没有找到对应的地址，需要遍历所有的mod列表，查找*/
    for(mod = 0; mod < BSP_MODU_MAX; mod++)
    {
        if(BSP_MODU_SYSVIEW == mod)
        {
            continue;
        }

        if(BSP_OK == mem_free_trace_node((bsp_module_e)mod,addr))
        {
            return BSP_OK;
        }
    }

    /* 记录错误打印*/
    printf("ERROR:the addr is invalid\n");
    return BSP_ERR_SYSVIEW_FREE_BUF_ERR;
}

/*****************************************************************************
* 函 数 名  : report_mem_trace
*
* 功能描述  :内存 trace上报处理接口
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

u32 report_mem_trace(void)
{
    BSP_MEM_LOG_STRU        *p_report_buf = NULL;
    BSP_MUDU_MEM_LOG_STRU   *p_mod_trace = NULL;
    u32          buf_len = 0;
    u32          mod;


    buf_len = sizeof(BSP_MEM_LOG_STRU) + sizeof(BSP_MUDU_MEM_LOG_STRU)*BSP_MODU_MAX;

    p_report_buf = (BSP_MEM_LOG_STRU *)bsp_om_get_buf(BSP_OM_SOCP_BUF_TYPE,buf_len);

    if(NULL == p_report_buf)
    {
        return BSP_ERR_SYSVIEW_INVALID_PARAM;
    }

    sysview_trace_packet((u8*)p_report_buf,buf_len,BSP_SYSVIEW_MEM_TRACE);

    p_mod_trace = (BSP_MUDU_MEM_LOG_STRU *)((u8*)p_report_buf + sizeof(BSP_MEM_LOG_STRU));

    for(mod = 0; mod < BSP_MODU_MAX; mod++)
    {
        p_mod_trace->mod_id         = mod;
        p_mod_trace->alloc_size     = g_om_mem_trace.module_trace[mod].alloc_size;
        p_mod_trace->free_size     = g_om_mem_trace.module_trace[mod].free_size;
        p_mod_trace->alloc_times    = g_om_mem_trace.module_trace[mod].alloc_times;
        p_mod_trace->free_times     = g_om_mem_trace.module_trace[mod].free_times;
    }

    if( BSP_OK != bsp_om_send_coder_src((u8 *)p_report_buf,buf_len))
    {
        bsp_om_free_buf((u32)p_report_buf,buf_len );

        return BSP_ERR_SYSVIEW_FAIL;
    }

    return BSP_OK;

}

/*****************************************************************************
* 函 数 名  : bsp_om_mem_alloc
*
* 功能描述  :内存 申请接口封装
*
* 输入参数  :modu_id :模块ID
*                        size  :申请的内存大小
*                        flag :内存属性
* 输出参数  : 无
*
* 返 回 值  : 内存地址
*****************************************************************************/

void* bsp_om_mem_alloc(bsp_module_e modu_id,u32 size,u32 flag)
{
    void  *p_addr = NULL;
#ifdef __KERNEL__
    p_addr =  kmalloc(size,flag);
#else
    p_addr =  malloc(size);
#endif

    mem_alloc_trace(modu_id,size,(u32)p_addr);

    return p_addr;
}

/*****************************************************************************
* 函 数 名  : bsp_om_mem_free
*
* 功能描述  :内存 释放接口封装
*
* 输入参数  :modu_id :模块ID
*                        addr  : 需要释放的内存
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_om_mem_free(bsp_module_e modu_id,u32 addr)
{
    if(BSP_OK != mem_free_trace(modu_id,addr))
    {
        /* system err */
    }

#ifdef __KERNEL__
    kfree(addr);
#else
    free((void *)addr);
#endif
    return;
}


void bsp_mem_show(u32 mod_id)
{
    printf("[mod_id].alloc_size = %d\n",g_om_mem_trace.module_trace[mod_id].alloc_size);
    printf("[mod_id].alloc_times = %d\n",g_om_mem_trace.module_trace[mod_id].alloc_times);
    printf("[mod_id].free_times = %d\n",g_om_mem_trace.module_trace[mod_id].free_times);
    printf("[mod_id].free_times = %d\n",g_om_mem_trace.module_trace[mod_id].free_size);

}


u32 bsp_get_module_mem_trace(u32 mod_id,BSP_MODULE_MEM_TRACE_STRU *module_trace)
{
    return BSP_OK;
}

u32 bsp_get_system_mem_trace(BSP_MEM_TRACE_STRU *mem_trace)
{
    return BSP_OK;
}


static void  mem_timeout_done(void)
{
        report_mem_trace();

        bsp_softtimer_add(&mem_info_stru.loop_timer);
}

u32 bsp_mem_swt_set(u32 set_swt,u32 period)
{
    u32 ret = BSP_OK;

    if(BSP_SYSVIEW_SWT_ON == set_swt)
    {
        if(mem_info_stru.loop_timer.init_flags == TIMER_INIT_FLAG )
        {
            bsp_om_stop_timer(&mem_info_stru.loop_timer);
            bsp_softtimer_modify(&mem_info_stru.loop_timer,period*1000);
            bsp_softtimer_add(&mem_info_stru.loop_timer);
            ret = BSP_OK;
        }
        else
        {
            ret = (u32)bsp_om_start_timer(period,(void *)mem_timeout_done , 0, &(mem_info_stru.loop_timer));

            if(ret == BSP_OK)
            {
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
        if(mem_info_stru.loop_timer.init_flags == TIMER_INIT_FLAG )
        {
            ret = (u32)bsp_om_stop_timer(&mem_info_stru.loop_timer);
        }
        else
        {
            ret = BSP_OK;
        }
    }

    if(BSP_OK != ret)
    {
        return ret;
    }

    mem_info_stru.report_swt  = set_swt;
    mem_info_stru.report_timer_len = period;
    return BSP_OK;
}


