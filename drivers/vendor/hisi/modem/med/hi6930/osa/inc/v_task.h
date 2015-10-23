/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_task.h                                                        */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement task                                               */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/


#ifndef _VOS_TASK_H
#define _VOS_TASK_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "vos_config.h"


VOS_UINT32 VOS_CreateTask( VOS_UINT32 * pulTaskID,
                           VOS_TASK_ENTRY_TYPE pfnFunc,
                           VOS_UINT32 ulPriority,
                           VOS_UINT32 ulStackSize,
                           VOS_UINT32 aulArgs[VOS_TARG_NUM] );

#if (VOS_WIN32 == VOS_OS_VER)
VOS_UINT32 VOS_SuspendTask( VOS_UINT32 ulTaskID );

VOS_UINT32 VOS_ResumeTask( VOS_UINT32 ulTaskID );
#endif

VOS_UINT32 VOS_TaskDelay( VOS_UINT32 ulMillSecs );

VOS_UINT32 VOS_TaskLock( VOS_VOID );

VOS_UINT32 VOS_TaskUnlock( VOS_VOID );

VOS_UINT32 VOS_ResetTask( VOS_UINT32 ulTaskID );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_TASK_H */
