

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <xtensa/coreasm.h>
#include <xtensa/simcall.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/config/core.h>
#include "arch_hifi330.h"




/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


    .global    OSStartHighRdy
    .global    OSTaskSwHook
    .global    g_pstVosTCBHighRdy
    .global    g_bVosRunning

    .data

/* 切换发生标志 */
    .type   OSIntCtxSw_Occur,@object
OSIntCtxSw_Occur:
    .byte 0




/*****************************************************************************
  3 函数实现
*****************************************************************************/


    .text
    .align 4
    .globl  VOSStartHighRdy
    .type   VOSStartHighRdy,@function
    .align  4

VOSStartHighRdy:
    ENTRY(16)

    /*置切换发生标志OSIntCtxSw_Occur为0*/
    movi    a2,  OSIntCtxSw_Occur
    movi    a3,  0
    s8i     a3,  a2, 0

    /*置g_bVosRunning为1，表明多任务内核已运行*/
    movi    a2,  g_bVosRunning
    movi    a3,  1
    s8i    a3,  a2, 0

    /*调用任务切换钩子OS_TaskSwHook */
    movi    a2, OS_TaskSwHook
    callx4  a2

    /*
    切换到当前最高优先级的任务的上下文*/
    call0 OS_TaskSwitch


    .globl  VOSCtxSw
    .type   VOSCtxSw,@function
    .align  4

VOSCtxSw:
    /*保存当前任务的上下文，并更新堆栈指针*/
    entry   sp,  XT_SOL_FRMSZ
    rsr     a2,  PS
    s32i    a0,  sp, XT_SOL_PC
    s32i    a2,  sp, XT_SOL_PS
    movi    a2,  g_pstVosTCBCur
    movi    a3,  0
    l32i    a2,  a2, 0
    s32i    a3,  sp, XT_SOL_EXIT        /* 0 表示任务中引起切换     */
    s32i    sp,  a2, OSTCBStkPtr        /* g_pstVosTCBCur->OSTCBStkPtr = SP;      */

    call4   xthal_window_spill

    /*调用任务切换钩子OS_TaskSwHook */
    movi    a2, OS_TaskSwHook
    callx4  a2


    /* 清除CPENABLE, 包括任务中的co-processor state保存区数据. */
    movi    a3, g_pstVosTCBCur
    l32i    a3, a3, 0
    l32i    a2, a3, OSTCBStkBottom
    movi    a3, 0

    wsr     a3, CPENABLE                /* disable all co-processors        */
    s8i     a3, a2, XT_CPENABLE         /* cp_state->cpenable = 0;          */


    /*
    切换到当前最高优先级的任务的上下文*/
    call0   OS_TaskSwitch


    .globl  VOSIntCtxSw
    .type   VOSIntCtxSw,@function
    .align  4

VOSIntCtxSw:
    ENTRY(16)

    /*调用任务切换钩子OS_TaskSwHook */
    movi    a2, OS_TaskSwHook
    callx4  a2


    /*保存CPENABLE在task的co-processor数据区, 并清除CPENABLE.  */
    movi    a3, g_pstVosTCBCur
    l32i    a3, a3, 0
    l32i    a2, a3, OSTCBStkBottom
    rsr     a3, CPENABLE
    s8i     a3, a2, XT_CPENABLE
    movi    a3, 0
    wsr     a3, CPENABLE

    /*置切换发生标志OSIntCtxSw_Occur为1*/
    movi    a2, OSIntCtxSw_Occur
    movi    a3, 1
    s8i     a3, a2, 0


    /*返回(真正的切换发生在中断退出时，根据OSIntCtxSw_Occur标志做切换)*/
    RET(16)



    .global    OSCPUSaveSR
    .type   OSCPUSaveSR,@function
    .align 4

OSCPUSaveSR:
    entry     a1,32
    rsil    a2,15
    esync
    retw


    .global    OSCPURestoreSR
    .type   OSCPURestoreSR,@function
    .align 4

OSCPURestoreSR:
    entry     a1,32
    wsr.ps    a2
    esync
    retw


    .global    OS_TaskSwitch
    .type   OS_TaskSwitch,@function
    .align 4

OS_TaskSwitch:

    /*1)
    切换上下文，恢复g_ucVosPrioHighRdy的堆栈:
    g_ucVosPrioCur = g_ucVosPrioHighRdy;
    g_pstVosTCBCur = g_pstVosTCBHighRdy;
    SP = g_pstVosTCBHighRdy->OSTCBStkPtr;
    */
    movi    a2,  g_ucVosPrioHighRdy
    movi    a3,  g_ucVosPrioCur
    l8ui   a2,  a2, 0
    movi    a4,  g_pstVosTCBHighRdy
    s8i     a2,  a3, 0
    movi    a2,  g_pstVosTCBCur
    l32i    a3,  a4, 0
    l32i    sp,  a3, OSTCBStkPtr
    s32i    a3,  a2, 0


    /*判断是任务中的切换还是中断里面的切换，分别处理2),3)。*/
    l32i    a2,  sp, XT_STK_EXIT
    bnez    a2,  .L_in_int


    /*
    1)任务中的切换处理:*/
    l32i    a3,  sp, XT_SOL_PS
    l32i    a0,  sp, XT_SOL_PC

    rsync

    wsr     a3,  PS
    retw

.L_in_int:
    /*
    2)中断中的切换处理:*/
    /* 从co-processor 保存区恢复CPENABLE*/
    movi    a3, g_pstVosTCBCur
    l32i    a3, a3, 0
    l32i    a2, a3, OSTCBStkBottom
    l8ui    a3, a2, XT_CPENABLE
    wsr     a3, CPENABLE


    /*  从堆栈恢复所有上下文，从 exit出口返回*/
    call0   OS_ContextRestore
    rsync

    l32i    a0, sp, XT_STK_EXIT
    ret


    .globl  OS_ContextSave
    .type   OS_ContextSave,@function
    .align  4

OS_ContextSave:

    /*1)按照堆栈结构保存下面的寄存器：
    A2
    A3
    A4
    A5
    A6
    A7
    A8
    A9
    A10
    A11
    SAR,LBENG,LEND,LCOUNT*/
    s32i    a2,  sp, XT_STK_A2
    s32i    a3,  sp, XT_STK_A3
    s32i    a4,  sp, XT_STK_A4
    s32i    a5,  sp, XT_STK_A5
    s32i    a6,  sp, XT_STK_A6
    s32i    a7,  sp, XT_STK_A7
    s32i    a8,  sp, XT_STK_A8
    s32i    a9,  sp, XT_STK_A9
    s32i    a10, sp, XT_STK_A10
    s32i    a11, sp, XT_STK_A11
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15


    rsr     a3,  SAR
    s32i    a3,  sp, XT_STK_SAR

    rsr     a3,  LBEG
    s32i    a3,  sp, XT_STK_LBEG
    rsr     a3,  LEND
    s32i    a3,  sp, XT_STK_LEND
    rsr     a3,  LCOUNT
    s32i    a3,  sp, XT_STK_LCOUNT

    /*2)调用系统函数保存需要的寄存器*/
    mov     a9,  a0
    s32i    a12, sp, XT_STK_TMP+0
    s32i    a13, sp, XT_STK_TMP+4
    s32i    a9,  sp, XT_STK_TMP+8
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    l32i    a9,  sp, XT_STK_A9
    addi    sp,  sp, XT_STK_FRMSZ
    call0   xthal_window_spill_nw
    addi    sp,  sp, -XT_STK_FRMSZ
    l32i    a12, sp, XT_STK_TMP+0
    l32i    a13, sp, XT_STK_TMP+4
    l32i    a9,  sp, XT_STK_TMP+8

    addi    a2,  sp, XT_STK_EXTRA
    call0   xthal_save_extra_nw

    /*返回*/
    mov     a0, a9
    ret


    .global OS_ContextRestore
    .type   OS_ContextRestore,@function
    .align  4

OS_ContextRestore:
    /*
    1)调用系统函数 xthal_restore_extra_nw*/
    mov     a13, a0
    addi    a2,  sp, XT_STK_EXTRA
    call0   xthal_restore_extra_nw
    mov     a0,  a13

    /*2)恢复：
    LBEG
    LEND
    LCOUNT
    A2
    SAR
    A3
    A4
    A5
    A6
    A7
    A8
    A9
    A10
    A11
    A12
    A13
    A14
    A15*/
    l32i    a2,  sp, XT_STK_LBEG
    l32i    a3,  sp, XT_STK_LEND
    wsr     a2,  LBEG
    l32i    a2,  sp, XT_STK_LCOUNT
    wsr     a3,  LEND
    wsr     a2,  LCOUNT

    l32i    a3,  sp, XT_STK_SAR
    l32i    a2,  sp, XT_STK_A2
    wsr     a3,  SAR
    l32i    a3,  sp, XT_STK_A3
    l32i    a4,  sp, XT_STK_A4
    l32i    a5,  sp, XT_STK_A5
    l32i    a6,  sp, XT_STK_A6
    l32i    a7,  sp, XT_STK_A7
    l32i    a8,  sp, XT_STK_A8
    l32i    a9,  sp, XT_STK_A9
    l32i    a10, sp, XT_STK_A10
    l32i    a11, sp, XT_STK_A11

    /*
    返回*/
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15

    ret



    .globl  OS_IntEnter
    .type   OS_IntEnter,@function
    .align  4
OS_IntEnter:

    /* Save a12-13 in the stack frame as required by _xt_context_save. */
    s32i    a12, sp, XT_STK_A12
    s32i    a13, sp, XT_STK_A13

    /* Save return address in a safe place (free a0). */
    mov     a12, a0

    /* Save the rest of the interrupted context (preserves A12-13). */
    call0   OS_ContextSave

    /*
    Save interrupted task's SP in TCB only if not nesting.
    Manage nesting directly rather than call the generic OSIntEnter()
    (in windowed ABI we can't call a C function here anyway because PS.EXCM is still set).
    */
    movi    a2,  g_bVosRunning
    movi    a3,  g_ucVosIntNesting
    l8ui   a2,  a2, 0                  /*  if (g_bVosRunning == OS_TRUE) {         */
    beqz    a2,  2f
    l8ui   a2,  a3, 0                  /*      if (g_ucVosIntNesting == 0) {        */
    bnez    a2,  1f
    movi    a4,  g_pstVosTCBCur
    l32i    a4,  a4, 0
    s32i    sp,  a4, OSTCBStkPtr        /*          g_pstVosTCBCur->OSTCBStkPtr = SP; */
1:                                      /*      }                               */
    addi    a2,  a2, 1                  /*      if (g_ucVosIntNesting < 255u) {      */
    bgeui   a2,  256, 2f                /*           g_ucVosIntNesting++;            */
    s8i    a2,  a3, 0                  /*      }                               */
2:                                      /*  }                                   */

    /* Retrieve the return address and return to interrupt handler. */
    mov     a0,  a12
    ret


    .globl  OS_IntExit
    .type   OS_IntExit,@function
    .align  4
OS_IntExit:

    /* Call VOSIntExit() to deal with nesting and call the scheduler. */
    movi    a2, VOSIntExit

    rsil    a0, XCHAL_EXCM_LEVEL    /* enter critical section       */
    callx4  a2                      /* VOSIntExit()                  */

    /* Context-switch deferred from OSIntCtxSw(). Still in critical section. */
    movi    a2, OSIntCtxSw_Occur
    l8ui    a3, a2, 0
    beqz    a3, 1f                  /* if (OSIntCtxSw_Occur) {         */
    movi    a3, 0                   /*    OSIntCtxSw_Occur = false;    */
    s8i     a3, a2, 0
    call0   OS_TaskSwitch           /*    tail-call dispatcher;     */
    /* Never returns here. */       /* }                            */
1:

    /*
    We come here only if there was no context switch, that is if this
    is a nested interrupt or the interrupted task was not preempted.
    We are still on the same stack so there's no need to load the SP.
    */

    /* Restore full context from interrupt stack frame and return to exit dispatcher. */
    call0   OS_ContextRestore

    /*
    Must return via the exit dispatcher corresponding to the entrypoint from which
    this was called. Interruptee's A0, A1, PS, PC are restored and the interrupt
    stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0, sp, XT_STK_EXIT
    ret

    .globl  OS_GetTaskCoprocState
    .type   OS_GetTaskCoprocState,@function
    .align  4
OS_GetTaskCoprocState:

    movi    a2,  g_bVosRunning
    movi    a3,  g_ucVosIntNesting
    l8ui   a2,  a2, 0
    l8ui   a3,  a3, 0
    beqz    a2,  1f
    bnez    a3,  1f
    movi    a15, g_pstVosTCBCur
    l32i    a15, a15, 0
    beqz    a15, 2f
    l32i    a15,  a15, OSTCBStkBottom
    ret

1:  movi    a15, 0
2:  ret



