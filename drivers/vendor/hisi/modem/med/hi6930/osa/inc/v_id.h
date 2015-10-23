/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: \vos\include\vos\v_id.h                                         */
/*                                                                           */
/* Author: Qin Peifeng                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2000-04-21                                                          */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2000-04-21                                                       */
/*    Author: Qin Peifeng                                                    */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _V_ID_H
#define _V_ID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_iddef.h"
#include "ucom_comm.h"

/************************* MACRO & TYPE DEFINITION ***************************/
/* VOS编程模式下的用户模块划分信息 */
BEGIN_FID_DEFINITION()


/* add your FID here, e.g.
 * DEFINE_FID(VOS_FID_APP0)
 * DEFINE_FID(VOS_FID_APPx) */
    DEFINE_FID(DSP_FID_RT)
    DEFINE_FID(DSP_FID_NORMAL)
    DEFINE_FID(DSP_FID_LOW)
END_FID_DEFINITION()


/*define product FID table here*/
/* FID value.  Initialize fid.  Stack's size uint is 16bit.  number of queue */
#undef DEFINE_PRODUCT_FID_TABLE
#define DEFINE_PRODUCT_FID_TABLE \
    {DSP_FID_RT,    UCOM_COMM_RtFidInit,    VOS_TSK_RT_STK_SIZE,    VOS_TSK_RT_QUEUE_NUMBER},\
    {DSP_FID_NORMAL,UCOM_COMM_NormalFidInit,VOS_TSK_NORMAL_STK_SIZE,VOS_TSK_NORMAL_QUEUE_NUMBER},\
    {DSP_FID_LOW,   UCOM_COMM_LowFidInit,   VOS_TSK_LOW_STK_SIZE,   VOS_TSK_LOW_QUEUE_NUMBER},






#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_ID_H */
