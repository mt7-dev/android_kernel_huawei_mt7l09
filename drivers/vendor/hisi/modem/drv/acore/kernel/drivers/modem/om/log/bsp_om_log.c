

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e322 -e7 -e537*/
#include "bsp_om_api.h"
#include "bsp_om_log.h"
#include "bsp_om.h"
#include "bsp_socp.h"
#include "bsp_om_server.h"
#include "bsp_bbp.h"
#include <linux/kernel.h>
#include <stdarg.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include "osl_sem.h"
#include "drv_om.h"
/*lint -restore*/

#define    THIS_MODU_ID        BSP_MODU_LOG

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
bsp_log_swt_cfg_s  g_mod_peint_level_info[BSP_MODU_MAX]    =
{
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR},
    {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}, {BSP_LOG_LEVEL_ERROR}
};

u32 g_get_buf_fail = 0;

log_bin_ind_cb        g_log_ind_cb = NULL;

#ifdef BSP_CONFIG_HI3630
extern dump_nv_s            g_dump_config;
#else
extern dump_nv_s            g_dump_cfg;
#endif
extern osl_sem_id                send_task_sem;

u32 bsp_log_level_set(bsp_log_level_e log_level);
void bsp_log_header_packet(bsp_module_e mod_id,bsp_log_level_e log_level,u8 *print_buf,u32 buf_size);
void bsp_log_show(void);


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
* 函 数 名  : bsp_log_module_cfg_set
*
* 功能描述  : HSO设置底软打印级别处理函数
*
* 输入参数  : log_swt_stru:各个模块的打印级别值
*                         data_len:      参数log_swt_stru的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 成功;其他 失败
*****************************************************************************/

u32 bsp_log_module_cfg_set(bsp_log_swt_cfg_s *log_swt_stru ,u32 data_len)
{
    u32 mod_num = 0;
    u32 i;

    if((NULL == log_swt_stru )||( 0 == data_len)||((data_len % sizeof(bsp_log_swt_cfg_s) != 0)))
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," bsp_log_module_cfg_set error!!  data_len = %d\n",data_len);
       return BSP_ERR_LOG_INVALID_PARAM;
    }

    mod_num = data_len / sizeof(bsp_log_swt_cfg_s);

    if(mod_num > BSP_MODU_MAX )
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," bsp_log_module_cfg_set error!!  mod_num = %d\n",mod_num);
        return BSP_ERR_LOG_INVALID_MODULE;
    }

    for(i = 0; i < mod_num; i++)
    {
        if(log_swt_stru[i].print_level <= BSP_LOG_LEVEL_MAX)
        {
            g_mod_peint_level_info[i].print_level = log_swt_stru[i].print_level;

        }
        else
        {
            bsp_om_debug(BSP_LOG_LEVEL_ERROR," bsp_log_module_cfg_set   log_swt_stru[i].print_level = %d\n",log_swt_stru[i].print_level);
            return BSP_ERR_LOG_INVALID_LEVEL ;
        }

    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_log_module_cfg_get
*
* 功能描述  : 查询模块当前设置的打印级别
*
* 输入参数  : mod_id:被查询模块ID
*
* 输出参数  : 无
*
* 返 回 值  : 打印级别
*****************************************************************************/

u32 bsp_log_module_cfg_get(bsp_module_e mod_id)
{
    if(mod_id >= BSP_MODU_MAX )
    {
        return BSP_ERR_LOG_INVALID_MODULE;
    }

    return g_mod_peint_level_info[mod_id].print_level;
}

/*****************************************************************************
* 函 数 名  : bsp_mod_level_set
*
* 功能描述  : 设置单个模块的打印级别
*
* 输入参数  : mod_id:模块ID
*                         print_level: 打印级别
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 设置成功，其他  设置失败
*****************************************************************************/

u32 bsp_mod_level_set(bsp_module_e  mod_id ,u32 print_level)
{
    if(mod_id >= BSP_MODU_MAX )
    {
        return BSP_ERR_LOG_INVALID_MODULE;
    }

    if(print_level >BSP_LOG_LEVEL_MAX)
    {
        return BSP_ERR_LOG_INVALID_LEVEL;
    }

    g_mod_peint_level_info[mod_id].print_level = print_level;

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_log_level_set
*
* 功能描述  : 设置所有模块的打印级别
*
* 输入参数  : print_level: 打印级别
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 设置成功，其他  设置失败
*****************************************************************************/

u32 bsp_log_level_set(bsp_log_level_e log_level)
{
    u32 mod_id = 0;

    if(log_level > BSP_LOG_LEVEL_MAX)
    {
        return BSP_ERR_LOG_INVALID_LEVEL;
    }

    for(mod_id = 0; mod_id < BSP_MODU_MAX; mod_id++)
    {
        g_mod_peint_level_info[mod_id].print_level = log_level;
    }

    return BSP_OK;
}


/*****************************************************************************
* 函 数 名  : bsp_log_level_reset
*
* 功能描述  : 将所有模块的打印级别设置为默认值
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_log_level_reset(void)
{
    u32 i;

    for(i = 0; i < BSP_MODU_MAX;i++)
    {
        g_mod_peint_level_info[i].print_level= BSP_LOG_LEVEL_ERROR;
    }

}

/*****************************************************************************
* 函 数 名  : bsp_log_header_packet
*
* 功能描述  : 打印数据包头信息填充
*
* 输入参数  :  mod_id: 输出模块
*                           print_level: 打印级别
*                           print_buf :整个buf指针
*                           buf_size: 整个输出的buf长度
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 打包成功
*****************************************************************************/

void bsp_log_header_packet(bsp_module_e mod_id,bsp_log_level_e log_level,u8 *print_buf,u32 buf_size)
{

    bsp_om_head_s        *bsp_om_header  = NULL;
    bsp_trace_txt_s  *print_header   = NULL;

    /*打包SOCP头*/
    PACKET_BSP_SOCP_HEAD(print_buf);

    bsp_om_header = (bsp_om_head_s *)(print_buf + sizeof(bsp_socp_head_s));

    /*lint -save -e648*/
    bsp_om_header->om_id            = ((BSP_STRU_ID_28_31_GROUP_BSP << 28) | (BSP_STRU_ID_16_23_BSP_PRINT << 16));
    /*lint -restore +e648*/
    bsp_om_header->data_size        = buf_size - sizeof(bsp_socp_head_s)-sizeof(bsp_om_head_s);

    print_header = ( bsp_trace_txt_s  *)(print_buf + sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s));
    print_header->mod_id            = mod_id;
    print_header->level             = log_level;
    print_header->log_sn            = g_om_global_info.print_sn++;

    return ;

}


/*****************************************************************************
* 函 数 名  : bsp_trace
*
* 功能描述  : 底软打印输出处理接口
*
* 输入参数  :  mod_id: 输出模块
*                           print_level: 打印级别
*                           fmt :打印输入参数
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_trace(bsp_log_level_e log_level,bsp_module_e mod_id,char *fmt,...)
{
    char                      bsp_print_buffer[BSP_PRINT_BUF_LEN] ;
    va_list arglist;

    /*打印级别过滤*/
    if(mod_id >= BSP_MODU_MAX )
    {
        return ;
    }

    if(g_mod_peint_level_info[mod_id].print_level > log_level )
    {
        return ;
    }

    /*lint -save -e530*/
    va_start(arglist, fmt);
    /*lint -restore +e530*/
    vsnprintf(bsp_print_buffer, BSP_PRINT_BUF_LEN, fmt, arglist); /* [false alarm]:屏蔽Fortify错误 */
    va_end(arglist);

    bsp_print_buffer[BSP_PRINT_BUF_LEN - 1] = '\0';

    printk("%s", bsp_print_buffer);

    return;
}


/*****************************************************************************
* 函 数 名  : bsp_log_bin_ind
*
* 功能描述  : 底软主动上报接口
*
* 输入参数  :  str_id: 和HSO交互的结构化ID
*                           ind_data_size: 上报的可维可测信息长度
*                           ind_data :上报的可维可测信息
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
u32 log_ind_enter_cnt=0;
u32 log_ind_exit_cnt=0;
void bsp_log_bin_ind(s32 str_id, void* ind_data, u32 ind_data_size)
{
    void* pbuf = 0;
    u32 buflen = 0;
    bsp_om_head_s        *bsp_om_header  = NULL;
    log_ind_enter_cnt++;

    if((NULL == ind_data) ||(0 == ind_data_size) || (ind_data_size > BSP_DIAG_IND_DATA_MAX_LEN))
    {
        return ;
    }

    if( TRUE != bsp_om_get_hso_conn_flag())
    {
        return ;
    }

    buflen = sizeof(bsp_socp_head_s)+sizeof(bsp_om_head_s)+ind_data_size ;

    /*获取buf*/
    if(bsp_om_buf_sem_take())
    {
        return;
    }
    pbuf = bsp_om_get_buf(BSP_OM_SOCP_BUF_TYPE,buflen);

    if(NULL == pbuf)
    {
        bsp_om_buf_sem_give();
        return ;
    }

     /*打包SOCP头*/
    PACKET_BSP_SOCP_HEAD(pbuf);

    bsp_om_header = (bsp_om_head_s*)(pbuf + sizeof(bsp_socp_head_s));

    bsp_om_header->om_id            = (BSP_STRU_ID_28_31_GROUP_MSP << 28) | (BSP_STRU_ID_16_23_BSP_CMD_IND << 16) |(u16)str_id;

    /*lint -save -e713*/
    memcpy((u8*)bsp_om_header +sizeof(bsp_om_head_s),ind_data,ind_data_size);
    /*lint -restore*/

    if(BSP_OK != bsp_om_into_send_list(pbuf,buflen))
    {
        bsp_om_free_buf(pbuf,buflen);
    }

    bsp_om_buf_sem_give();
    log_ind_exit_cnt++;
}


/*debug 接口*/
void bsp_log_show(void)
{
    printk("trace level              = %d\n",g_mod_peint_level_info[0].print_level);
    printk("hso_connect_flag   = %d\n",g_om_global_info.hso_connect_flag);
    printk("print_sn                  = %d\n",  g_om_global_info.print_sn);
    printk("git buf fail               = %d\n",  g_get_buf_fail);

    printk("log_ind_enter_cnt = %d\n",log_ind_enter_cnt);
    printk("log_ind_exit_cnt = %d\n",log_ind_exit_cnt);

}


