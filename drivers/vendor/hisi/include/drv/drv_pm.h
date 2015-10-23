/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_pm.h
*
*   作    者 :
*
*   描    述 : 本文件用于低功耗对外接口
*
*   修改记录 :  2013年9月26日  v1.00  创建
*************************************************************************/
#ifndef __DRV_PM_H__
#define __DRV_PM_H__

#include <drv_comm.h>

typedef enum pm_wake_src
{
    DRV_WAKE_SRC_UART0 = 0, /*for acore at_uart*/
    DRV_WAKE_SRC_SOCP ,     /*for acore sd_log*/
    DRV_WAKE_SRC_DRX_TIMER, /*for ccore drx timer*/
    DRV_WAKE_SRC_BUT,
}DRV_PM_WAKE_SRC_ENUM;

/*******************************************************************************
  函数名:       BSP_VOID DRV_PM_SET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src)
  函数描述:     动态设置唤醒源
  输入参数:     wake_src  唤醒源枚举值
  输出参数:     无
  返回值:       无
*******************************************************************************/
BSP_VOID DRV_PM_SET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src);

/*******************************************************************************
  函数名:       BSP_VOID DRV_PM_SET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src)
  函数描述:     动态取消唤醒源的设置
  输入参数:     wake_src  唤醒源枚举值
  输出参数:     无
  返回值:       无
*******************************************************************************/
BSP_VOID DRV_PM_UNSET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src);


/*****************************************************************************
* 函 数 名  : DRV_SET_DRX_TIMER_WAKE_SRC
* 功能描述  : 设置 DRX timer 作为唤醒源
* 输入参数  :
* 输出参数  :
* 返 回 值   :
* 修改记录  :
*****************************************************************************/
BSP_VOID DRV_SET_DRX_TIMER_WAKE_SRC(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_SET_DRX_TIMER_WAKE_SRC
* 功能描述  : 取消设置 DRX timer 作为唤醒源
* 输入参数  :
* 输出参数  :
* 返 回 值   :
* 修改记录  :
*****************************************************************************/
BSP_VOID DRV_DEL_DRX_TIMER_WAKE_SRC(BSP_VOID);


/* adp */


/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_DEEPSLEEP_FOREVER
 功能描述  : AARM CARM 下电接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None

*****************************************************************************/
extern void DRV_PWRCTRL_DEEPSLEEP_FOREVER(void );

typedef struct
{
    PWRCTRLFUNCPTR drx_slow;
} PWC_SLEEP_CALLBACK_STRU;

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEP_CALLBACK_REG
 功能描述  : 注册PWC函数给底软使用
 输入参数  : 待注册的函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int DRV_PWRCTRL_SLEEP_CALLBACK_REG(PWC_SLEEP_CALLBACK_STRU pCallback);


typedef struct
{
    PWRCTRLFUNCPTRVOID  pFuncDsIn;
    PWRCTRLFUNCPTRVOID  pFuncDsOut;
}PWC_DS_SOCP_CB_STRU;

typedef unsigned long (*pULULFUNCPTR)( unsigned long ulPara );

typedef struct
{
    pULULFUNCPTR pRestoreFunction;
    pULULFUNCPTR pEnableFunction;
    pULULFUNCPTR pDisableFunction;
    pFUNCPTR2    pDfsSetFunc;
    pULULFUNCPTR pDfsReleaseFunc;
}DPM_SLEEP_CALLBACK_STRU;

/*****************************************************************************
Function:   DRV_PWRCTRL_SLEEP_IN_CB
Description:
Input:
Output:     None;
Return:
Others:
*****************************************************************************/
 int DRV_PWRCTRL_SLEEP_IN_CB(PWC_DS_SOCP_CB_STRU stFunc);



#endif

