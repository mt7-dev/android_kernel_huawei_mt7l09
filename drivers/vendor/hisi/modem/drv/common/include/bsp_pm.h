/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : pm.c */
/* Version       : 2.0 */
/* Created       : 2013-09-22*/
/* Last Modified : */
/* Description   :  pm drv*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#ifndef __BSP_PM_H__
#define __BSP_PM_H__

#include <osl_types.h>
#include <drv_pm.h>

/*
动态设置唤醒源
参数 int_id  对应 m3 上的中断号
*/

#ifdef CONFIG_CCORE_BALONG_PM
void pm_enable_wake_src(enum pm_wake_src wake_src);
void pm_disable_wake_src(enum pm_wake_src wake_src);

#else
static inline void pm_enable_wake_src(enum pm_wake_src wake_src){}
static inline void pm_disable_wake_src(enum pm_wake_src wake_src){}
#endif

#endif
