/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_int.c                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement int function                                       */
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
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos_config.h"
#include "v_typdef.h"
#include "v_lib.h"


#if (VOS_WIN32 == VOS_OS_VER) 

CRITICAL_SECTION VOS_CriticalSection;

/*****************************************************************************
 Function   : VOS_SplInit()
 Description: Initialize the interrupt
 Input      : None
 Return     : Nnoe
 *****************************************************************************/
VOS_VOID VOS_SplInit()
{
    InitializeCriticalSection( &VOS_CriticalSection );
}

/*****************************************************************************
 Function   : VOS_SplIMP()
 Description: Turn off the interrupt
 Input      : None
 Return     : VOS_OK;
 Other      : none
 *****************************************************************************/
VOS_CPU_SR VOS_SplIMP(VOS_VOID)
{
    EnterCriticalSection( &VOS_CriticalSection );
    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_Splx()
 Description: Turn on the interrupt
 Input      : s -- value returned by VOS_SplIMP()
 Return     : None
 *****************************************************************************/
VOS_VOID VOS_Splx( VOS_CPU_SR s )
{
    LeaveCriticalSection ( &VOS_CriticalSection );
}


#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


