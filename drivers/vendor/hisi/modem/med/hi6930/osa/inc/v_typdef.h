/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_typedef.h                                                     */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: define the type of VAR                                       */
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

#ifndef _V_TYPDEF_H
#define _V_TYPDEF_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos_config.h"

#ifndef VOS_OS_VER
#error "Please include v_configOS.h first!!!"
#endif

#if (VOS_WIN32 == VOS_OS_VER)
#define _WIN32_WINNT 0x0400
#include "Windows.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#endif

#ifndef FALSE
#define FALSE                          0
#endif

#ifndef TRUE
#define TRUE                           1
#endif

#define VOS_TRUE                        TRUE
#define VOS_FALSE                       FALSE


#define VOS_NULL               (0)
#define VOS_NULL_PTR           (0L)
#define VOS_NULL_DWORD         (0xFFFFFFFF)

#define VOS_MEMORY_CRC         0x55AA55AA

#define CONTROL_BLOCK_IDLE     0x55aa
#define CONTROL_BLOCK_BUSY     0x5a5a

#if (VOS_HIFI == VOS_CPU_TYPE)

typedef unsigned short          VOS_BOOL;

typedef unsigned char           VOS_UINT8;

typedef signed   char           VOS_INT8;

typedef signed   char           VOS_CHAR;

typedef unsigned char           VOS_UCHAR;

typedef unsigned long           VOS_CPU_SR;

typedef unsigned long           VOS_STK;

#endif

#if (VOS_ZSP == VOS_CPU_TYPE)

typedef unsigned short          VOS_BOOL;

typedef unsigned short          VOS_UINT8;

typedef signed   short          VOS_INT8;

typedef signed   short          VOS_CHAR;

typedef unsigned short          VOS_UCHAR;

typedef unsigned short          VOS_CPU_SR;

typedef unsigned short          VOS_STK;

#endif

#if ( (VOS_ARM == VOS_CPU_TYPE) || (VOS_X86 == VOS_CPU_TYPE) )

typedef unsigned long          VOS_BOOL;

typedef unsigned char          VOS_UINT8;

typedef signed char            VOS_INT8;

typedef char                   VOS_CHAR;

typedef unsigned char          VOS_UCHAR;

typedef unsigned long          VOS_CPU_SR;

typedef unsigned long          VOS_STK;

#endif

typedef void                   VOS_VOID;

typedef unsigned short         VOS_UINT16;

typedef signed   short         VOS_INT16;

typedef unsigned long          VOS_UINT32;

typedef signed   long          VOS_INT32;

typedef signed   long          VOS_INT;

typedef unsigned long          VOS_PID;

typedef unsigned long          VOS_FID;


#define VOS_TARG_NUM                                        4

typedef VOS_VOID ( * VOS_TASK_ENTRY_TYPE )( VOS_UINT32 ulPara0,
                                            VOS_UINT32 ulPara1,
                                            VOS_UINT32 ulPara2,
                                            VOS_UINT32 ulPara3);


enum VOS_INIT_PHASE_DEFINE
{
    VOS_IP_LOAD_CONFIG,
    VOS_IP_FARMALLOC,
    VOS_IP_INITIAL,
    VOS_IP_ENROLLMENT,
    VOS_IP_LOAD_DATA,     /* File System specific     */
    VOS_IP_FETCH_DATA,    /* General Inquiry specific */
    VOS_IP_STARTUP,
    VOS_IP_RIVAL,
    VOS_IP_KICKOFF,
    VOS_IP_STANDBY,
    VOS_IP_BROADCAST_STATE,
    VOS_IP_RESTART,
    VOS_IP_BUTT
};

typedef VOS_UINT32 (*Init_Fun_Type)( enum VOS_INIT_PHASE_DEFINE InitPhrase );

#define VOS_PRIORITY_NUM 14

enum VOS_PRIORITY_DEFINE
{
    VOS_PRIORITY_NULL,
    VOS_PRIORITY_M6,
    VOS_PRIORITY_M5,
    VOS_PRIORITY_M4,
    VOS_PRIORITY_M3,
    VOS_PRIORITY_M2,
    VOS_PRIORITY_M1,
    VOS_PRIORITY_BASE,
    VOS_PRIORITY_P1,
    VOS_PRIORITY_P2,
    VOS_PRIORITY_P3,
    VOS_PRIORITY_P4,
    VOS_PRIORITY_P5,
    VOS_PRIORITY_P6
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_TYPDEF_H */

