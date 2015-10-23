
#include <string.h>
#include "bsp_om_api.h"
#include "bsp_om.h"
#include "drv_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ENABLE_BUILD_OM
/*****************************************************************************
* 函 数 名  : DRV_LOG_LVL_SET
*
* 功能描述  : HSO设置底软打印级别处理函数
*
* 输入参数  : log_swt_stru:各个模块的打印级别值
*                         data_len:      参数log_swt_stru的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 成功;其他 失败
*****************************************************************************/

BSP_U32 DRV_LOG_LVL_SET(VOID *log_swt_stru ,BSP_U32 data_len)
{
    return 0;
}


/*****************************************************************************
* 函 数 名  : DRV_OM_SET_HSO_CONN_FLAG
*
* 功能描述  : flag :连接标志，1表示连接，0表示断开
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

BSP_VOID DRV_OM_SET_HSO_CONN_FLAG(BSP_U32 flag)
{
    return;
}

/*****************************************************************************
* 函 数 名  : DRV_SYSVIEW_SWT_SET
*
* 功能描述  :sysview上报开关设置，封装函数，提供给MSP使用
*
* 输入参数  :tarce_type :trace类型
*                        set_swt  :开关值
*                       period  :上报周期
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_GET_ALL_TASK_ID_NAME
*
* 功能描述  :获取当前所有任务的ID和名称
*
* 输入参数  :p_task_stru:任务名称和ID输出的结构体
*                       param_len:参数1的长度，单位字节
*
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK/错误码
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
    return 0;
}


/*****************************************************************************
* 函 数 名  : DRV_INT_LOCK
*
* 功能描述  :锁中断接口，用于锁中断统计
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : 锁中断后获得的key
*****************************************************************************/
BSP_S32 DRV_INT_LOCK(BSP_VOID)
{
    return 0;
}


/*****************************************************************************
* 函 数 名  : DRV_INT_UNLOCK
*
* 功能描述  : 解锁中断接口，用于锁中断统计
*
* 输入参数  : 锁中断后获得的key
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID DRV_INT_UNLOCK(BSP_S32 LockKey)
{
    return;
}


/*****************************************************************************
* 函 数 名  : DRV_SOCP_CHAN_ENABLE
*
* 功能描述  : 底软SOCP通道使能操作
*
* 输入参数  : 无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

BSP_VOID DRV_SOCP_CHAN_ENABLE(BSP_VOID)
{
    return;
}

int DRV_GET_FS_ROOT_DIRECTORY(char * data, unsigned int len)
{
	strncpy(data, "/modem_log", len);

    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_LOG_BIN_IND
*
* 功能描述  : 底软主动上报接口
*
* 输入参数  :
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/
BSP_VOID DRV_LOG_BIN_IND(BSP_S32 str_id, BSP_VOID* ind_data, BSP_S32 ind_data_size)
{
	return;
}

/*****************************************************************************
* 函 数 名  : DRV_LOG_BIN_IND_REGISTER
*
* 功能描述  : 底软主动上报注册接口
*
* 输入参数  :
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/
BSP_VOID DRV_LOG_BIN_IND_REGISTER(log_bin_ind_cb log_ind_cb)
{
	return;
}


#else
/*****************************************************************************
* 函 数 名  : DRV_LOG_LVL_SET
*
* 功能描述  : HSO设置底软打印级别处理函数
*
* 输入参数  : log_swt_stru:各个模块的打印级别值
*                         data_len:      参数log_swt_stru的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_OK 成功;其他 失败
*****************************************************************************/

BSP_U32 DRV_LOG_LVL_SET(VOID *log_swt_stru ,BSP_U32 data_len)
{
    return bsp_log_module_cfg_set((bsp_log_swt_cfg_s * )log_swt_stru , data_len);
}


/*****************************************************************************
* 函 数 名  : DRV_OM_SET_HSO_CONN_FLAG
*
* 功能描述  : flag :连接标志，1表示连接，0表示断开
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

BSP_VOID DRV_OM_SET_HSO_CONN_FLAG(BSP_U32 flag)
{
    bsp_log_level_reset();
    bsp_om_set_hso_conn_flag( flag);
}

#ifndef ENABLE_BUILD_SYSVIEW
/*****************************************************************************
* 函 数 名  : DRV_SYSVIEW_SWT_SET
*
* 功能描述  :sysview上报开关设置，封装函数，提供给MSP使用
*
* 输入参数  :tarce_type :trace类型
*                        set_swt  :开关值
*                       period  :上报周期
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_GET_ALL_TASK_ID_NAME
*
* 功能描述  :获取当前所有任务的ID和名称
*
* 输入参数  :p_task_stru:任务名称和ID输出的结构体
*                       param_len:参数1的长度，单位字节
*
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK/错误码
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
    return 0;
}

#else
/*****************************************************************************
* 函 数 名  : DRV_SYSVIEW_SWT_SET
*
* 功能描述  :sysview上报开关设置，封装函数，提供给MSP使用
*
* 输入参数  :tarce_type :trace类型
*                        set_swt  :开关值
*                       period  :上报周期
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return bsp_sysview_swt_set(set_type,set_swt,period);
}

/*****************************************************************************
* 函 数 名  : DRV_GET_ALL_TASK_ID_NAME
*
* 功能描述  :获取当前所有任务的ID和名称
*
* 输入参数  :p_task_stru:任务名称和ID输出的结构体
*                       param_len:参数1的长度，单位字节
*
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK/错误码
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
      return sysview_get_all_task_name((void *)p_task_stru,param_len);
}
#endif

/*****************************************************************************
* 函 数 名  : DRV_INT_LOCK
*
* 功能描述  :锁中断接口，用于锁中断统计
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : 锁中断后获得的key
*****************************************************************************/
BSP_S32 DRV_INT_LOCK(BSP_VOID)
{
    BSP_S32 flags;

    flags = (unsigned long)intLock();

    bsp_int_lock_enter();
    return flags;
}


/*****************************************************************************
* 函 数 名  : DRV_INT_UNLOCK
*
* 功能描述  : 解锁中断接口，用于锁中断统计
*
* 输入参数  : 锁中断后获得的key
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID DRV_INT_UNLOCK(BSP_S32 LockKey)
{
    bsp_int_lock_out();

    intUnlock(LockKey);
}


/*****************************************************************************
* 函 数 名  : DRV_SOCP_CHAN_ENABLE
*
* 功能描述  : 底软SOCP通道使能操作
*
* 输入参数  : 无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

BSP_VOID DRV_SOCP_CHAN_ENABLE(BSP_VOID)
{
    bsp_socp_chan_enable();
}

int DRV_GET_FS_ROOT_DIRECTORY(char * data, unsigned int len)
{
	strncpy(data, "/modem_log", len);

    return 0;
}


#endif

#ifdef __cplusplus
}
#endif


