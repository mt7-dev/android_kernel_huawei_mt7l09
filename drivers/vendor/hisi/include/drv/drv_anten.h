/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_anten.h
*
*   作    者 :  liuwenhui
*
*   描    述 :
*
*   修改记录 :  2011年1月18日  v1.00  liuwenhui创建
*************************************************************************/

#ifndef __DRV_ANTEN_H__
#define __DRV_ANTEN_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <drv_comm.h>
#include <drv_dpm.h>


/*****************************************************************************
 函 数 名  : DRV_MNTN_GetExtAntenLockState
 功能描述  : 获取有线与无线的连接状态
 输入参数  : None
 输出参数  : 0 - 无线连接
             1 - 有线连接
 返 回 值  : 0 - 成功
             其它为失败

*****************************************************************************/
extern int drv_anten_lock_status_get(PWC_COMM_MODEM_E modem_id, unsigned int *status);
#define DRV_GET_ANTEN_LOCKSTATE(modem_id, status)    drv_anten_lock_status_get(modem_id, status)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExtAntenIntInstall
 功能描述  : 中断注册函数，用来获得当前的天线状态
 输入参数  : routine   - 中断处理函数
             para      - 保留字段
 输出参数  : None
 返 回 值  : void

*****************************************************************************/
extern void drv_anten_int_install(PWC_COMM_MODEM_E modem_id,void* routine, int para);
#define DRV_ANTEN_INT_INSTALL(modem_id, routine, para)    drv_anten_int_install(modem_id,routine, para)

extern int drv_anten_ipc_reg_fun(void);
#define BSP_MNTN_ANT_IFC_REGFUNC()    drv_anten_ipc_reg_fun()

#define BSP_MNTN_GetExtAntenLockState(modem_id, status) drv_anten_lock_status_get( modem_id, status)

#ifdef __cplusplus
}
#endif

#endif

