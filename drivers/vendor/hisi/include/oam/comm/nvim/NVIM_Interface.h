/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: NVIM_Interface.h                                                */
/*                                                                           */
/* Author: Jiang kaibo                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-06                                                             */
/*                                                                           */
/* Description: 存放内容仅供外部模块使用.                                */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-06                                                          */
/*    Author: Jiang kaibo                                                    */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _NVIM_INTERFACE_H
#define _NVIM_INTERFACE_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vos.h"
#include "NvIdList.h"
#include "SysNv.h"
#include "nvim_comminterface.h"
#include "DrvInterface.h"

#pragma pack(4)

#define GCF_AUTO_STUB_ITEM_NUM          10                  /* comm gcf*/
#define GCF_STUB_ITEM_LEN               8                   /* comm gcf*/


/*************************************************************\
|  请在这里维护 GCF CASE NO与 GCF ITEM NO的对照表            *|
|------------------------------------------------------------*|
|------------------------------------------------------------*|
|  GCF_TEST_MODULE       GCF_CASE_NO        GCF_ITEM_NO      *|
|------------------------------------------------------------*|
|  2G3                     20.22.29         1                *|
|  ????                    ????????         ?                *|
\*************************************************************/
typedef enum     /*comm gcf*/
{
    GCF_2G3_CASE_MIN,
    GCF_2G3_CASE_20_22_29,
    GCF_2G3_CASE_6_1_2_1,
    GCF_2G3_CASE_6_2_2_1,
    GCF_GSM_CASE_20_7,
    GCF_GSM_CASE_MIN,
    GCF_SMS_34_2_7,
    GCF_WCDMA_CASE_MIN,
    GCF_WCDMA_12_4_1_4B,
    GCF_WCDMA_12_9_7B,
    GCF_WCDMA_MAC_TFCI,
    GCF_RRM_8_3_5_3,
    GCF_RRM_8_3_4,
    GCF_RRM_8_7_3A,
    GCF_ALL_2G3_WCDMA_RRM,            /* 只要是在作GCF,就需要 */
    GCF_WCDMA_8_2_2_18,
    GCF_WCDMA_8_2_6_44,
    GCF_WCDMA_8_1_7_1d,
    GCF_WCDMA_6_1_2_2,
    GCF_NAS_9_4_3_AND_26_7_4_3
}ENUM_GCF_ITEM_NO;


#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

