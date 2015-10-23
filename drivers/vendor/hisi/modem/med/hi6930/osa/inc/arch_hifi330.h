

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include <xtensa/simcall.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/config/core.h>

#ifndef __ARCH_HIFI330_H__
#define __ARCH_HIFI330_H__

#ifdef __ASSEMBLER__
#include    <xtensa/coreasm.h>
#endif

#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*
Align a value up to nearest n-byte boundary, where n is a power of 2.
*/
#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))


/*******************************************************************************
INTERRUPT STACK FRAME FOR A THREAD OR NESTED INTERRUPT
*******************************************************************************/
#define XT_STK_EXIT             0x00    /* (offset 0) exit point for dispatch */
#define XT_STK_PC               0x04    /* return address */
#define XT_STK_PS               0x08    /* at level 1 PS.EXCM is set here */
#define XT_STK_A0               0x0C
#define XT_STK_A1               0x10    /* stack ptr before interrupt */
#define XT_STK_A2               0x14
#define XT_STK_A3               0x18
#define XT_STK_A4               0x1C
#define XT_STK_A5               0x20
#define XT_STK_A6               0x24
#define XT_STK_A7               0x28
#define XT_STK_A8               0x2C
#define XT_STK_A9               0x30
#define XT_STK_A10              0x34
#define XT_STK_A11              0x38
#define XT_STK_A12              0x3C    /* Call0 callee-save */
#define XT_STK_A13              0x40    /* Call0 callee-save */
#define XT_STK_A14              0x44    /* Call0 callee-save */
#define XT_STK_A15              0x48    /* Call0 callee-save */
#define XT_STK_SAR              0x4C

#define XT_STK_LBEG             0x50
#define XT_STK_LEND             0x54
#define XT_STK_LCOUNT           0x58
#define XT_STK_NEXT1            0x5C    /* next unused offset */

#define XT_STK_EXTRA            ALIGNUP(XCHAL_EXTRA_SA_ALIGN, XT_STK_NEXT1)

#define XT_STK_NEXT2            (XT_STK_EXTRA    + XCHAL_EXTRA_SA_SIZE)

#define XT_STK_N_TMP            3       /* # of 4-byte temp. slots */
#define XT_STK_TMP              XT_STK_NEXT2
#define XT_STK_NEXT3            XT_STK_TMP    + (4 * XT_STK_N_TMP)
#define XT_STK_FRMSZ            (ALIGNUP(0x10, XT_STK_NEXT3) + 0x20)


/*******************************************************************************
SIMPLE STACK FRAME FOR A THREAD
*******************************************************************************/
#define XT_SOL_EXIT             XT_STK_EXIT /* code indicates solicited frame */
#define XT_SOL_PC               0x04    /* return address (b30-31=callinc) */
#define XT_SOL_PS               0x08
#define XT_SOL_NEXT             0x0c    /* next unused offset */
                                /* there may be some unused space here */
#define XT_SOL_A0               ALIGNUP(0x10, XT_SOL_NEXT)
#define XT_SOL_A1               XT_SOL_A0    + 4
#define XT_SOL_A2               XT_SOL_A1    + 4
#define XT_SOL_A3               XT_SOL_A2    + 4
#define XT_SOL_FRMSZ            ALIGNUP(0x10, XT_SOL_A3)


/*******************************************************************************
CO-PROCESSOR STATE SAVE AREA FOR A THREAD
*******************************************************************************/
#define XT_CPENABLE 0
#define XT_CPSTORED (XT_CPENABLE + 1)
#define XT_CP0_SA   ALIGNUP(XCHAL_CP0_SA_ALIGN, XT_CPSTORED + 1)
#define XT_CP1_SA   ALIGNUP(XCHAL_CP1_SA_ALIGN, XT_CP0_SA + XCHAL_CP0_SA_SIZE)
#define XT_CP2_SA   ALIGNUP(XCHAL_CP2_SA_ALIGN, XT_CP1_SA + XCHAL_CP1_SA_SIZE)
#define XT_CP3_SA   ALIGNUP(XCHAL_CP3_SA_ALIGN, XT_CP2_SA + XCHAL_CP2_SA_SIZE)
#define XT_CP4_SA   ALIGNUP(XCHAL_CP4_SA_ALIGN, XT_CP3_SA + XCHAL_CP3_SA_SIZE)
#define XT_CP5_SA   ALIGNUP(XCHAL_CP5_SA_ALIGN, XT_CP4_SA + XCHAL_CP4_SA_SIZE)
#define XT_CP6_SA   ALIGNUP(XCHAL_CP6_SA_ALIGN, XT_CP5_SA + XCHAL_CP5_SA_SIZE)
#define XT_CP7_SA   ALIGNUP(XCHAL_CP7_SA_ALIGN, XT_CP6_SA + XCHAL_CP6_SA_SIZE)
#define XT_CP_SIZE  ALIGNUP(4                 , XT_CP7_SA + XCHAL_CP7_SA_SIZE)

#ifdef __ASSEMBLER__
/* Windowed */
#define ENTRY(sz)     entry   sp, sz
#define ENTRY0        entry   sp, 0x10
#define RET(sz)       retw
#define RET0          retw
#endif

#define XT_TICK_PER_SEC     100
#define XT_RTOS_INT_ENTER   OS_IntEnter
#define XT_RTOS_INT_EXIT    OS_IntExit
#define XT_RTOS_CP_STATE    OS_GetTaskCoprocState

#ifdef __ASSEMBLER__
// typedef struct vos_tcb {
#define OSTCBStkPtr             0x00    // OSTCBStkPtr
#define OSTCBStkBottom          0x10    // StackAddress
//...
// } VOS_TCB;
#endif /* __ASSEMBLER__ */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/






#endif /* end of arch_balongV7r1.h */
