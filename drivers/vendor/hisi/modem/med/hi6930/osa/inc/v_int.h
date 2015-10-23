/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_int.h                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement interrupt                                          */
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

#ifndef _VOS_INTERRUPT_H
#define _VOS_INTERRUPT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "kernel.h"

#if (VOS_ZOS == VOS_OS_VER)

extern VOS_CPU_SR OSCPUSaveSR (VOS_VOID);
extern VOS_VOID OSCPURestoreSR( VOS_CPU_SR flag );

#define VOS_SplIMP()        OSCPUSaveSR()

#define VOS_Splx(s)         OSCPURestoreSR((s))

/* example */
/*
VOS_CPU_SR      CpuLockLevel;

CpuLockLevel = VOS_SplIMP();

code

VOS_Splx(CpuLockLevel);
*/

#if (VOS_ZSP == VOS_CPU_TYPE )
#define VOS_EnterInt() asm("lda a0,_g_ucVosIntNesting \n ld r0,a0 \n add r0,1 \n st r0,a0"\
                            :\
                            :\
                            )

#define VOS_ExitInt()                           VOSIntExit()

/* Enable single interrupts  */
#define VOS_EnableSingleInt(intr_no)            bitset_creg(%imask,intr_no)

/* Disable single interrupts  */
#define VOS_DisableSingleInt(intr_no)           bitclear_creg(%imask,intr_no)

/* Enable multiple interrupts and keep the rests as it is */
#define VOS_EnableMultiInt(intr_mask)           V_EnableMultiInt(intr_mask)

/* Disable multiple interrupts and keep the rests as it is */
#define VOS_DisableMultiInt(intr_mask)          V_DisableMultiInt(intr_mask)

#define VOS_EnableAllInt()                      VOS_EnableMultiInt(0x1fff)
#define VOS_DisableAllInt()                     VOS_DisableMultiInt(0x1fff)

#define VOS_GetImaskState()                     V_GetImaskValue()
#endif

#endif

#if (VOS_WIN32 == VOS_OS_VER)

VOS_VOID VOS_SplInit();

VOS_CPU_SR VOS_SplIMP(VOS_VOID);

VOS_VOID VOS_Splx( VOS_CPU_SR s );

#endif


#if (VOS_ZSP == VOS_CPU_TYPE )
extern VOS_VOID V_DisableMultiInt(VOS_UINT16 uhwIntMask);
extern VOS_VOID V_EnableMultiInt(VOS_UINT16 uhwIntMask);
extern VOS_CPU_SR V_GetImaskValue(VOS_VOID);

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_INTERRUPT_H */

