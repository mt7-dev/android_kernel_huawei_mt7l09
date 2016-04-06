

#ifndef __BSP_OM_SYSVIEW_H__
#define __BSP_OM_SYSVIEW_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "bsp_om.h"
#include "drv_comm.h"
#include "osl_list.h"
#include <linux/semaphore.h>

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BSP_SYSVIEW_SWT_ON          1
#define BSP_SYSVIEW_SWT_OFF         0

#define BSP_SYSVIEW_TRUE            1
#define BSP_SYSVIEW_FALSE           0

#define BSP_TASK_INT_REPORT_PERIOD    3
#define BSP_SYSVIEW_TASK_NAME_LEN    12

#define     TIMER_MAX_VALUE       0xffffffff


#define bsp_om_init_list_head(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    } while (0)

#define bsp_om_list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)

#define bsp_om_list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)16)->member)+16))


/*****************************************************************************
  2 枚举定义
*****************************************************************************/



/*****************************************************************************
  2 结构体
*****************************************************************************/

typedef struct
{
    u32                 mod_id;
    u32                 alloc_times;
    u32                 alloc_size;
    u32                 free_times;
    u32                 free_size;
} BSP_MUDU_MEM_LOG_STRU;

typedef struct
{
    u32                 sn;
} BSP_MEM_LOG_HEADER_STRU;

typedef struct
{
    bsp_socp_head_s      socp_header;
    bsp_om_head_s        om_header;
    BSP_MEM_LOG_HEADER_STRU mem_log_header;
    BSP_MUDU_MEM_LOG_STRU   mem_log_stru[0];
} BSP_MEM_LOG_STRU;



typedef struct
{
    u32                 task_id;
    char                   task_name[BSP_SYSVIEW_TASK_NAME_LEN];
    u32                 interval_slice;
} BSP_TASK_CPU_TRACE_STRU;

typedef struct
{
    u32                 sn;
} BSP_CPU_HEADER_STRU;

typedef struct
{
    bsp_socp_head_s      socp_header;
    bsp_om_head_s        om_header;
    BSP_CPU_HEADER_STRU     cpu_info_header;
    BSP_TASK_CPU_TRACE_STRU cpu_info_stru[0];
} BSP_CPU_TRACE_STRU;


typedef struct
{
    u32                 sn;
} BSP_SYS_VIEW_HEADER_STRU;


typedef struct
{
    bsp_socp_head_s      socp_header;
    bsp_om_head_s        om_header;
    BSP_SYS_VIEW_HEADER_STRU sysview_header;
    u32                 sysview_data[0];
} BSP_SYS_VIEW_TRACE_STRU;


typedef struct
{
    void*                                 alloc_addr;
    u32                                 len;
    struct list_head               p_list;
} BSP_MEM_ALLOC_TRACE_STRU;

typedef struct
{
    u32                     alloc_times;
    u32                     alloc_size;
    u32                     free_times;
    u32                     free_size;
    u32                     use_mem_size;
    BSP_MEM_ALLOC_TRACE_STRU    alloc_trace_list;
} BSP_MODULE_MEM_TRACE_STRU;

typedef struct
{
    u32                     total_memory_size;   /* 系统内存总量 */
    BSP_MODULE_MEM_TRACE_STRU   module_trace[BSP_MODU_MAX];
} BSP_MEM_TRACE_STRU;

typedef struct
{
     struct softtimer_list    loop_timer;
    u32                 report_swt;
    u32                 report_timer_len;
} BSP_SYS_MEM_INFO_CTRL_STRU;

typedef struct
{
    u8    task_name[BSP_SYSVIEW_TASK_NAME_LEN + 1];
    u32  cmdslice;
    u32  slices;
    u32  SwitchNum;
    u32  max_slice;
    u32  min_slice;
} BSP_TASK_CPU_INFO_STRU;


typedef struct
{
     struct softtimer_list    loop_timer;
    u32                 report_swt;
    u32                 report_timer_len;
} BSP_SYS_CPU_INFO_CTRL_STRU;

typedef struct
{
     void*                 task_id;
    u32                 report_swt;
    u32                 threshold_size;
    u32                 report_timer_len;
} BSP_TASK_INT_INFO_CTRL_STRU;


typedef struct
{
    u32                task_id;
    u8                  name[BSP_SYSVIEW_TASK_NAME_LEN + 1];
} BSP_SYSVIEW_TASK_INFO_STRU;

typedef struct
{
    u32                swt_value;
    u32                cb_call_times;
    u32                timeout_times;
    u32                info_send_times;
    u32                info_send_size;
    u32                info_lost_times;
    u32                info_lost_size;

} BSP_SYSVIEW_DEBUG_STRU;

extern BSP_SYSVIEW_DEBUG_STRU  g_sysview_debug[BSP_SYSVIEW_SWT_ALL];

void bsp_mem_show(u32 mod_id);

u32 bsp_get_module_mem_trace(u32 mod_id,BSP_MODULE_MEM_TRACE_STRU *module_trace);

u32 bsp_get_system_mem_trace(BSP_MEM_TRACE_STRU *mem_trace);

u32 bsp_int_lock_set(u32 set_swt,u32 period);

u32 bsp_task_swt_set(u32 set_swt,u32 period);

u32 bsp_mem_swt_set(u32 set_swt,u32 period);

u32 bsp_cpu_swt_set(u32 set_swt,u32 period);

void sysview_trace_packet(u8 *trace_buf,u32 buf_size,bsp_sysview_type_e data_type);

void task_swt_read_cb_done(void);

void int_lock_read_cb_done(void);

void bsp_mem_init(void);

void cpu_view_report_init(void);

u32 cpu_utilization_start(void);

void  cpu_timeout_done(void);

u32 cpu_utilization_end(u8 *pTaskName, double *CPUusage);

u32 report_cpu_trace(void);

void  cpu_task_test(u32 secends);

void report_sysview_trace(bsp_sysview_type_e data_type);

int  task_view_task(void * para);

int  int_lock_view_task(void * para);

void cpu_task_swt_hook(void *old_tcb, void *new_tcb);


/*****************************************************************************
  5 错误码定义
*****************************************************************************/
#define BSP_ERR_SYSVIEW_BASE            (u32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_SYSVIEW, 0)))
#define BSP_ERR_SYSVIEW_INVALID_MODULE  (BSP_ERR_SYSVIEW_BASE + 0x1)
#define BSP_ERR_SYSVIEW_MALLOC_FAIL     (BSP_ERR_SYSVIEW_BASE + 0x2)
#define BSP_ERR_SYSVIEW_INVALID_PARAM   (BSP_ERR_SYSVIEW_BASE + 0x3)
#define BSP_ERR_SYSVIEW_NO_BUF          (BSP_ERR_SYSVIEW_BASE + 0x4)
#define BSP_ERR_SYSVIEW_FREE_BUF_ERR    (BSP_ERR_SYSVIEW_BASE + 0x5)
#define BSP_ERR_SYSVIEW_FAIL            (BSP_ERR_SYSVIEW_BASE + 0x6)

#endif

