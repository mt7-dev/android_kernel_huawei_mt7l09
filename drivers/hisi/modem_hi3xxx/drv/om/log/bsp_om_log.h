

#ifndef __BSP_OM_LOG_H__
#define __BSP_OM_LOG_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "bsp_om.h"
#include "bsp_om_api.h"
#include "bsp_om_server.h"
#include "drv_comm.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define  BSP_PRINT_BUF_LEN  256

#define  BSP_DIAG_IND_DATA_MAX_LEN         (4096)


/*****************************************************************************
  2 枚举定义
*****************************************************************************/



/*****************************************************************************
  2 结构体
*****************************************************************************/

typedef struct
{
    u32     mod_id;     /* 打印信息所在的模块ID*/
    u32     level;      /* 输出级别*/
    u32     log_sn;     /* IND标号*/
    char     text[0];    /* 所有打印文本内容，可能包括文件和行号,以'\0'结尾 */
} bsp_trace_txt_s;

typedef struct
{
    bsp_socp_head_s      socp_header;
    bsp_om_head_s        om_header;
    bsp_trace_txt_s         print_stu;
} bsp_trace_s;

typedef struct
{
    void*                     addr;
    u16                     is_edle;
    u16                     is_valible;
}bsp_log_node_ctrl_s;

typedef struct
{
    void*                     start_ptr;      /*  起始地址*/
    void*                     start_phy_ptr;
    u32                     node_num;    /* buf 块总数*/
    u32                     buys_node_num;   /* 被占用的节点数目*/
    u32                     valible_node_num;   /* 写入数据的节点数目*/
    u32                     init_state;
    bsp_log_node_ctrl_s  stnode[BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN];
}bsp_om_log_buf_ctrl_s;


u32 bsp_log_module_cfg_get(bsp_module_e mod_id);


/*****************************************************************************
  5 错误码定义
*****************************************************************************/
#define BSP_ERR_LOG_BASE                (int)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_LOG, 0)))
#define BSP_ERR_LOG_INVALID_MODULE      (BSP_ERR_LOG_BASE + 0x1)
#define BSP_ERR_LOG_INVALID_LEVEL       (BSP_ERR_LOG_BASE + 0x2)
#define BSP_ERR_LOG_INVALID_PARAM       (BSP_ERR_LOG_BASE + 0x3)
#define BSP_ERR_LOG_NO_BUF              (BSP_ERR_LOG_BASE + 0x4)
#define BSP_ERR_LOG_SOCP_ERR            (BSP_ERR_LOG_BASE + 0x5)


#endif

