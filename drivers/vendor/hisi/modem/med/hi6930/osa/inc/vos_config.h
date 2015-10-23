/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_config.h                                                    */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description:                                                              */
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


#ifndef _V_CONFIG_H
#define _V_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#include "dopra_def.h"
#ifndef ASSEMBLE_FILE
#include "v_fileid.h"
#endif

#ifndef VOS_OS_VER
#define VOS_OS_VER                      VOS_ZOS
#endif

#ifndef VOS_CPU_TYPE
#define VOS_CPU_TYPE                    VOS_HIFI
#endif


#ifndef VOS_MEMORY_CHECK
#define VOS_MEMORY_CHECK                VOS_NO
#endif

#ifndef VOS_MEMORY_COUNT
#define VOS_MEMORY_COUNT                VOS_NO
#endif

#if (VOS_HIFI == VOS_CPU_TYPE )
#define MILLISECONDS_PER_TICK           1
#else
#define MILLISECONDS_PER_TICK           10
#endif

/*
define the byte order according to the VOS_HARDWARE_PLATFORM
*/
#ifndef VOS_DOPRA_VER
#define VOS_DOPRA_VER                   VOS_RELEASE
#endif

#define VOS_FILE_ID                     THIS_FILE_ID

/* Defines the lowest priority that can be assigned */
#define VOS_LOWEST_PRIO                 4

/* Max. number of tasks in your application */
#define VOS_MAX_TASKS                   (VOS_LOWEST_PRIO)

/* the Max queue number supported by VOS */
#define VOS_MAX_QUEUE_NUMBER            (VOS_MAX_TASKS-1)

/* the Max Sem number supported by VOS */
#define VOS_MAX_SEM_NUMBER              VOS_MAX_QUEUE_NUMBER

/* Max. number of event control blocks in your application  */
#define VOS_MAX_EVENTS                  (VOS_MAX_SEM_NUMBER+1)

/* total size of msg. unit is sizeof(VOS_UINT8) */
#define VOS_MSG_POOL_SIZE               2648

/* the number of TSK_RT queue */
#define VOS_TSK_RT_QUEUE_NUMBER         6

/* the number of TSK_NORMAL queue */
#define VOS_TSK_NORMAL_QUEUE_NUMBER     6

/* the number of TSK_LOW queue */
#define VOS_TSK_LOW_QUEUE_NUMBER        6

/* total size of queue. unit is sizeof(VOS_UINT8) */
#define VOS_QUEUE_POOL_SIZE\
    (sizeof(VOS_UINT32)\
    *(VOS_TSK_RT_QUEUE_NUMBER+VOS_TSK_NORMAL_QUEUE_NUMBER+VOS_TSK_LOW_QUEUE_NUMBER))


/* the Max messages stored in queue of one FID */
#define VOS_FID_QUEUE_LENGTH            10

/* Enable (1) or Disable (0) argument checking */
#ifndef VOS_ARG_CHK_EN
#define VOS_ARG_CHK_EN                  0
#endif

#ifndef VOS_STK_CHK_EN
#define VOS_STK_CHK_EN                  1
#endif
/* 堆栈检测相关宏定义 */
#define VOS_STK_FLG                     0x55555555
#define IDLE_STK_ID                     254
#define INTR_STK_ID                     255


/* Idle task stack size (sizeof of VOS_UINT8 wide entries) */
/*should be 4*n */
#define VOS_TASK_IDLE_STK_SIZE          (2048)

/* Runtime task stack size (sizeof of VOS_UINT8 wide entries) */
/*should be 4*n */
#define VOS_TSK_RT_STK_SIZE             (3072)

/* Voice task stack size (sizeof of VOS_UINT8 wide entries) */
/*should be 4*n */
#define VOS_TSK_NORMAL_STK_SIZE         (14336)


/* Audio task stack size (sizeof of VOS_UINT8 wide entries) */
/*should be 4*n */
#define VOS_TSK_LOW_STK_SIZE            (4096)


/* total size of stack. unit is sizeof(VOS_UINT8) */
#define VOS_TASK_STACK_SIZE\
    (VOS_TSK_RT_STK_SIZE+ VOS_TSK_NORMAL_STK_SIZE+ VOS_TSK_LOW_STK_SIZE)


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_CONFIG_H */
