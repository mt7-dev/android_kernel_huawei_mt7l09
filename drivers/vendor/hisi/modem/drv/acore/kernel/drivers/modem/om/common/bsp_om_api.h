


#ifndef __BSP_OM_API_H__
#define __BSP_OM_API_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "osl_types.h"
#include "bsp_om.h"
#include "bsp_dump.h"
#include "bsp_dump_def.h"
#include "bsp_softtimer.h"
#include <linux/dma-mapping.h>

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define BSP_OM_MAX_TASK_NUM         128
#define  BSP_LOG_SEND_TO_SHELL (0)
#define  BSP_LOG_SEND_TO_HSO    (1)
#define  BSP_LOG_SEND_TO_FILE    (2)

#define BSP_OM_WAIT_FOREVER  -1

#define BSP_OM_TIMESTAMP_LEN   8
/*****************************************************************************
  2 枚举定义
*****************************************************************************/
/* 此枚举的先后顺序不可以随便调整*/
typedef enum
{
    BSP_OM_LOG_BUF_TYPE    ,
    BSP_OM_SOCP_BUF_TYPE   ,
    BSP_OM_BUF_NUM
} bsp_om_buf_type_e;

/*****************************************************************************
  2 结构体
*****************************************************************************/
typedef struct
{
    u32                hso_connect_flag;
    union
    {
        u32 uin_nv_cfg;
        struct
        {
            u32 log_switch    : 2; /* 00:打印到串口；01:打印到HSO；10:打印到文件；11:预留 */
            u32 reserved1      : 30;
        } nv_cfg;
    } om_cfg;
    u32                 print_sn;
    u32                 cpu_info_sn;
    u32                 mem_info_sn;
    u32                 task_info_sn;
    u32                 int_lock_info_sn;

} bsp_om_global_s;

extern   bsp_om_global_s     g_om_global_info ;

typedef struct
{
    u32 print_level;
} bsp_log_swt_cfg_s;



typedef struct
{
    /*下面两个字节的组合统称Service ID */
    u8      service_id;
    u8      s_service_id;
    u8      service_session_id;
    u8      msg_type;
    u32     msg_trans_id;
    u8      time_stamp[BSP_OM_TIMESTAMP_LEN];
} bsp_socp_head_s;

typedef struct
{
    u32     om_id;        /* 结构化ID*/
    u32     data_size;    /* 紧随的ucData的长度*/
    u8      data[0];    /* 填充数据*/
} bsp_om_head_s;


/*****************************************************************************
  2 函数申明
*****************************************************************************/

#define OM_CACHE_FLUSH(data, len)      dma_map_single(NULL, data, len, DMA_TO_DEVICE)
#define OM_CACHE_INVALIDATE(data, len) dma_map_single(NULL, data, len, DMA_FROM_DEVICE)
u32 bsp_log_module_cfg_set(bsp_log_swt_cfg_s *log_swt_stru ,u32 data_len);

void bsp_log_level_reset(void);

u32 bsp_om_free_buf(void* buf_addr,u32 len);
void* bsp_om_get_buf(u32 buf_type,u32 free_buf_len);
void* bsp_om_get_log_buf(u32 get_buf_len);

u32 bsp_om_send_coder_src(u8 *send_data_virt, u32 send_len);

int bsp_om_start_timer(u32 timer_len,void *func,u32 para,struct softtimer_list *timer);
int bsp_om_stop_timer(struct softtimer_list *timer);

int bsp_om_into_send_list(void* buf_addr,u32 len);

int bsp_om_buf_sem_take(void);
int bsp_om_buf_sem_give(void);


#define bsp_om_debug(level, fmt, ...) \
printk("[om]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__)

#define om_debug(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,   BSP_MODU_OM, "[om]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define om_warning(fmt, ...)  (bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_OM, "[om]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define om_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,   BSP_MODU_OM, "[om]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define om_fetal(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_FATAL,   BSP_MODU_OM, "[om]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))


#endif


