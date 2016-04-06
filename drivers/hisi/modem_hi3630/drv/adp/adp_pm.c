/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : adp_pm.c */
/* Version       : 1.0 */
/* Created       : 2013-09-26*/
/* Last Modified : */
/* Description   :  pm adp*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#include <drv_pm.h>
#include <bsp_pm.h>

/*
set&unset wakeup interrupt source
input : wake_src defined in drv_pm.h [DRV_PM_WAKE_SRC_ENUM]
*/
BSP_VOID DRV_PM_SET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src)
{
    pm_enable_wake_src(wake_src);
}

BSP_VOID DRV_PM_UNSET_WAKE_SRC(DRV_PM_WAKE_SRC_ENUM wake_src)
{

    pm_disable_wake_src(wake_src);
}


/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_DEEPSLEEP_FOREVER
 功能描述  : AARM CARM 下电接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None

*****************************************************************************/
 void DRV_PWRCTRL_DEEPSLEEP_FOREVER(void )
 {}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEP_CALLBACK_REG
 功能描述  : 注册PWC函数给底软使用
 输入参数  : 待注册的函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
 int DRV_PWRCTRL_SLEEP_CALLBACK_REG(PWC_SLEEP_CALLBACK_STRU pCallback)
 {
	return 0;
 }


/*****************************************************************************
Function:   DRV_PWRCTRL_SLEEP_IN_CB
Description:
Input:
Output:     None;
Return:
Others:
*****************************************************************************/
 int DRV_PWRCTRL_SLEEP_IN_CB(PWC_DS_SOCP_CB_STRU stFunc)
 {
	return 0;
 }



