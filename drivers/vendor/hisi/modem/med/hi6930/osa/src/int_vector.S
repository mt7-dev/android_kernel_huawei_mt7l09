

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include    <xtensa/coreasm.h>
#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>
#include    <xtensa/simcall.h>
#include    "arch_hifi330.h"



    .data
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* CP信息. */
    .type   g_awVosCoprocSaOffset,@object
    .align  16                      /* minimize crossing cache boundaries */
g_awVosCoprocSaOffset:
    .word   XT_CP0_SA, XT_CP1_SA, XT_CP2_SA, XT_CP3_SA
    .word   XT_CP4_SA, XT_CP5_SA, XT_CP6_SA, XT_CP7_SA

/* CP n's CPENABLE bit. */
    .type   g_bVosCoprocMask,@object
    .align  16,,8                   /* try to keep it all in one cache line */
g_bVosCoprocMask:
    .byte   0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80

/* Owner of CP (0 = unowned). */
    .global g_awVosCoprocOwnerSa
    .type   g_awVosCoprocOwnerSa,@object
    .align  16,,XCHAL_CP_MAX<<2     /* minimize crossing cache boundaries */
g_awVosCoprocOwnerSa:
    .space  XCHAL_CP_MAX << 2,0

    .global g_strVosPanicMessage
    .align      4
g_strVosPanicMessage:
    .string "\npanic at addr 0x%08x.\n"


/*****************************************************************************
  3 函数实现
*****************************************************************************/



    .text
    .global     OS_Panic
    .type       OS_Panic,@function
    .align      4

OS_Panic:
    /**/
#ifdef ZOS_SIM
    /*仿真环境，输出调试打印信息:panic at addr*/
    addi    a4, a0, -3                      /* point to call0 */
    movi    a3, g_strVosPanicMessage
    movi    a2, SYS_log_msg
    simcall
    movi    a2, SYS_gdb_abort
    simcall
#else
    /*否则关闭所有low & med中断，进入死循环*/
    rsil    a2, XCHAL_EXCM_LEVEL            /* disable all low & med ints */
1:  j       1b                              /* loop infinitely */
#endif


    .begin      literal_prefix .DebugExceptionVector
    .section    .DebugExceptionVector.text, "ax"
    .global     OS_DebugExceptionVector
    .align      4

OS_DebugExceptionVector:
        /*
#ifdef ZOS_SIM
   仿真环境,由debugger处理
#else
   调用OS_Panic
#endif */
#ifdef ZOS_SIM
    wsr     a2, EXCSAVE+XCHAL_DEBUGLEVEL
    movi    a2, SYS_gdb_enter_sktloop
    simcall
#else
    wsr     a0, EXCSAVE+XCHAL_DEBUGLEVEL
    call0   OS_Panic
    rfi     XCHAL_DEBUGLEVEL
#endif

    .end        literal_prefix


    .begin      literal_prefix .DoubleExceptionVector
    .section    .DoubleExceptionVector.text, "ax"
    .global     OS_DoubleExceptionVector
    .align      4

OS_DoubleExceptionVector:
    /*
    调用OS_Panic
    */
    call0   OS_Panic                       /* does not return */
    rfde                                    /* make a0 point here not later */

    .end        literal_prefix


    .begin      literal_prefix .KernelExceptionVector
    .section    .KernelExceptionVector.text, "ax"
    .global     OS_KernelExceptionVector
    .align      4

OS_KernelExceptionVector:
    /*
    调用OS_Panic
    */
    call0   OS_Panic                       /* does not return */
    rfe                                    /* make a0 point here not later */

    .end        literal_prefix


    .begin      literal_prefix .UserExceptionVector
    .section    .UserExceptionVector.text, "ax"
    .global     OS_UserExceptionVector
    .type       OS_UserExceptionVector,@function
    .align      4

OS_UserExceptionVector:
    wsr     a0, EXCSAVE_1                   /* preserve a0 */
    call0   OS_UserExc                      /* user exception handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix


    .text

    .align      4
OS_CoProcessorExc:
    s32i    a5, sp, XT_STK_A5               /* save a5 */
    addi    a5, a0, -EXCCAUSE_CP0_DISABLED  /* a0 = CP index */
    bgei    a5, XCHAL_CP_MAX, .L_xt_user_exc_not_coproc
    call0   OS_CoProcessorException                  /* in window vectors section */
    /* never returns here - call0 is used as a jump (see note at top) */

    .align      4
OS_AllocExc:
    call0   OS_AllocaException                  /* in window vectors section */
    /* never returns here - call0 is used as a jump (see note at top) */

    .align      4
OS_SyscallExc:
    call0   OS_SyscallException
    /* never returns here - call0 is used as a jump (see note at top) */


    .global     OS_UserExc
    .type       OS_UserExc,@function
    .align      4
OS_UserExc:
    /*
    1)读入EXCCAUSE

    2)处理EXCCAUSE_ALLOCA，EXCCAUSE_SYSCALL
    if(EXCCAUSE为EXCCAUSE_ALLOCA)
    {
        调用OS_AllocaException
    }
    else if(EXCCAUSE为EXCCAUSE_SYSCALL)
    {
        调用OS_SyscallException
    }
    */
    rsr     a0, EXCCAUSE
    beqi    a0, EXCCAUSE_ALLOCA,  OS_AllocExc
    beqi    a0, EXCCAUSE_SYSCALL, OS_SyscallExc

    /*
    3)
    分配中断栈，保存部分信息
    包括: A0,A1,PC,PS,exit出口(OS_UserExit)
    */
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, OS_UserExit                 /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /*
    4)
    处理协处理器异常
    if(EXCCAUSE为协处理器异常)
    {
        调用OS_CoProcessorException
    }
    */
    rsr     a0, EXCCAUSE
    bgeui   a0, EXCCAUSE_CP0_DISABLED, OS_CoProcessorExc

.L_xt_user_exc_not_coproc:


    /*
    5)
    调用XT_RTOS_INT_ENTER保存剩余的下文
    call0   XT_RTOS_INT_ENTER*/
    call0   XT_RTOS_INT_ENTER


    /*
    6)
    设置C运行环境*/
    movi    a0, PS_INTLEVEL(XCHAL_EXCM_LEVEL) | PS_UM | PS_WOE
    wsr     a0, PS
    rsync

    /*
    7)
    处理level-1 interrupt和其余的user exception
    if(EXCCAUSE为EXCCAUSE_LEVEL1INTERRUPT)
    {
        call4   OS_UserexecHandlers
    }
    else
    {
        call4 OS_InterruptHandlers
    }*/
    rsr     a2, EXCCAUSE
    beqi    a2, EXCCAUSE_LEVEL1INTERRUPT, .L_xt_user_int
    mov     a6,a2

    /*User exception*/
    call4   OS_UserexecHandler
    j       .L_xt_user_done

.L_xt_user_int:
    /* level 1 int */
    rsr     a2, INTENABLE
    rsr     a3, INTERRUPT
    movi    a4, XCHAL_INTLEVEL1_MASK
    and     a2, a2, a3
    and     a2, a2, a4
    beqz    a2, .L_int_done1

    neg     a3,a2
    and     a3,a3,a2
    wsr     a3,INTCLEAR

    find_ls_one a2, a3
    mov     a6,a2
    call4   OS_EnterIntHook

    mov     a6,a2
    call4   OS_InterruptHandler

    mov     a6,a2
    call4   OS_ExitIntHook
    j       .L_xt_user_int

.L_int_done1:


.L_xt_user_done:
    /*
    8)
    调用XT_RTOS_INT_EXIT退出
    call0   XT_RTOS_INT_EXIT
    */
    call0   XT_RTOS_INT_EXIT


    .global     OS_UserExit
    .type       OS_UserExit,@function
    .align      4
OS_UserExit:
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_1
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure PS and EPC written */
    rfe                                     /* PS.EXCM is cleared */


    .type       OS_SyscallException,@function
    .align      4

OS_SyscallException:
    /*
    1)保存上下文*/
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   OS_ContextSave


    /*
    2)update PC*/
    rsr     a2, EPC_1
    addi    a3, a2, 3

    rsr     a0, LEND
    bne     a3, a0, 1f
    rsr     a0, LCOUNT
    beqz    a0, 1f
    addi    a0, a0, -1
    rsr     a3, LBEG
    wsr     a0, LCOUNT

1:  wsr     a3, EPC_1

    /* Restore context,从exception返回 */
    call0   OS_ContextRestore
    addi    sp, sp, XT_STK_FRMSZ

    movi    a0, -1
    movnez  a2, a0, a2                      /* return -1 if not syscall 0 */
    rsr     a0, EXCSAVE_1
    rfe




    .type   OS_CoProcessorException,@function
    .align  4

OS_CoProcessorException:                             /* a5 = CP index */

    /* 保存部分寄存器 */
    s32i    a2,  sp, XT_STK_A2
    s32i    a3,  sp, XT_STK_A3
    s32i    a4,  sp, XT_STK_A4
    s32i    a15, sp, XT_STK_A15


    /* 获取co-processor的old owner和new owner. */
    call0   XT_RTOS_CP_STATE                /* a15 = new owner's save area */
    beqz    a15, .L_xt_coproc_invalid       /* not in a thread (invalid) */

    mov     a0, a5                          /* a0 = CP index */
    movi    a3, g_awVosCoprocOwnerSa
    addx4   a3, a0, a3                      /* a3 = &g_awVosCoprocOwnerSa[CP] */
    l32i    a2, a3, 0                       /* a2 = old owner's save area */

    /* Enable the co-processor's bit in CPENABLE. */
    movi    a4, g_bVosCoprocMask
    rsr     a5, CPENABLE                    /* a5 = CPENABLE */
    add     a4, a4, a0
    l8ui    a4, a4, 0                       /* a4 = bitmask from CP index */
    or      a5, a5, a4                      /* a5 = CPENABLE + CP */
    wsr     a5, CPENABLE


    /* 判断是否需要做context-switch,old owner!=new owner时需要1)，否则不需要2)
     if(做context-switch)
     {
         cp上下文保存
     }*/
    beq     a15, a2, .L_xt_coproc_done      /* new owner == old, we're done */

    s32i    a15, a3,  0                     /* g_awVosCoprocOwnerSa[CP] = new */

    l8ui    a5,  a15, XT_CPSTORED           /* a5 = new owner's old CPSTORED */
    neg     a3,  a4
    addi    a3,  a3, -1                     /* a3 = ~a4 (bitmask for CP) */
    and     a3,  a5,  a3                    /* clr CP in new owner's CPSTORED */
    s8i     a3,  a15, XT_CPSTORED           /* update new owner's CPSTORED */

    /* Adjust new owner's save area pointers to area for CP n. */
    movi    a3,  g_awVosCoprocSaOffset
    and     a5,  a5,  a4                    /* a5 = new owner's CP is stored */
    addx4   a3,  a0,  a3                    /* a3 = &OS_CoprocSaOffset[CP] */
    l32i    a3,  a3,  0                     /* a3 = XT_CP[CP]_SA */
    add     a15, a15, a3                    /* a15 = new owner's area for CP */
    moveqz  a15, a5,  a5                    /* a15 = 0 if state not stored */

    rsync                                   /* ensure wsr.CPENABLE is complete */
    beqz    a2, .L_xt_coproc_new            /* no old owner to save */

    /* If old owner still needs CP, clear CP in CPENABLE and set in CPSTORED. */
    l8ui    a5,  a2,  XT_CPENABLE           /* a5 = old owner's CPENABLE */
    and     a4,  a5,  a4                    /* a4 = CP in old owner CPENABLE */
    beqz    a4,  .L_xt_coproc_new           /* discard old owner's CP state */

    sub     a5,  a5,  a4                    /* clr CP in old owner's CPENABLE */
    s8i     a5,  a2,  XT_CPENABLE           /* update old owner's CPENABLE */
    l8ui    a5,  a2,  XT_CPSTORED           /* a5 = old owner's CPSTORED */
    or      a5,  a5,  a4                    /* set CP in old owner's CPSTORED */
    s8i     a5,  a2,  XT_CPSTORED           /* update old owner's CPSTORED */

    /* Save co-processor state of old owner. */
.Ln_xt_coproc_old:
    add     a2, a2, a3                      /* a2 = old owner's area for CP */
    mov     a3, a0                          /* a3 = CP index */
    /*
    The config-specific HAL macro invoked below destroys a2-5, preserves a0-1.
    It is theoretically possible for Xtensa processor designers to write TIE
    that causes more address registers to be affected, but it is generally
    unlikely. If that ever happens, more registers needs to be saved/restored
    around this macro invocation, and the value in a15 needs to be recomputed.
    */
    xchal_cpi_store_funcbody

    /* Restore co-processor state of new owner. */
.L_xt_coproc_new:
    beqz    a15, .L_xt_coproc_done          /* no valid state to restore */
    mov     a2,  a15                        /* a2 = new owner's area for CP */
    mov     a3,  a0                         /* a3 = CP index */

    xchal_cpi_load_funcbody


.L_xt_coproc_done:
    l32i    a15, sp, XT_STK_A15
    l32i    a5,  sp, XT_STK_A5
    l32i    a4,  sp, XT_STK_A4
    l32i    a3,  sp, XT_STK_A3
    l32i    a2,  sp, XT_STK_A2
    call0   OS_UserExit

.L_xt_coproc_invalid:
    call0   OS_Panic                       /* not in a thread (invalid) */


    .begin      literal_prefix .Level2InterruptVector
    .section    .Level2InterruptVector.text, "ax"
    .global     OS_Level2Vector
    .type       OS_Level2Vector,@function
    .align      4
OS_Level2Vector:
    wsr     a0, EXCSAVE_2                   /* preserve a0 */
    call0   OS_Medint2                     /* load interrupt handler */

    .end        literal_prefix


    .text
    .extern     OS_InterruptHandler
    .type       OS_Medint2,@function
    .align      4
OS_Medint2:
    /*
    1)
     分配中断栈
     保存 pre-interrupt SP
     保存 interruptee's PS
     保存 interruptee's PC
     保存 interruptee's a0
     保存 exit point:OS_Medint2Exit
    */
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_2                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_2                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_2                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, OS_Medint2Exit              /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /*
    2)保存其余的上下文*/
    call0   XT_RTOS_INT_ENTER

    /*
    3)设置C调用环境
    */
    movi    a0, PS_INTLEVEL(2) | PS_UM | PS_WOE
    wsr     a0, PS
    rsync


    /*
    4)
    读入interrupt，得到未处理的level-2的比特
    从low到high处理所有level-2比特位
    并清除对应的中断位(如果是电平中断需要中断服务程序自行清除外部中断源，这里虽然也清除，实际无效)
    调用中断进入hook(中断号)，OS_EnterIntHook
    调用中断处理接口OSInterruptHandler(中断号)
    调用中断退出hook(中断号)，OS_ExitIntHook
    */
.L_ProcAllInt2:
    rsr     a2, INTENABLE
    rsr     a3, INTERRUPT
    movi    a4, XCHAL_INTLEVEL2_MASK
    and     a2, a2, a3
    and     a2, a2, a4
    beqz    a2, .L_int_done2

    neg     a3,a2
    and     a3,a3,a2
    wsr     a3,INTCLEAR

    find_ls_one a2, a3
    mov     a6,a2
    call4   OS_EnterIntHook

    mov     a6,a2
    call4   OS_InterruptHandler

    mov     a6,a2
    call4   OS_ExitIntHook
    j       .L_ProcAllInt2

.L_int_done2:
    /*
    5)
    中断退出*/
    call0   XT_RTOS_INT_EXIT


    .global     OS_Medint2Exit
    .type       OS_Medint2Exit,@function
    .align      4
OS_Medint2Exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_2
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_2
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     2

    .begin      literal_prefix .Level3InterruptVector
    .section    .Level3InterruptVector.text, "ax"
    .global     OS_Level3Vector
    .type       OS_Level3Vector,@function
    .align      4

OS_Level3Vector:
    wsr     a0, EXCSAVE_3                   /* preserve a0 */
    call0   OS_MedInt3                     /* load interrupt handler */

    .end        literal_prefix


    .text
    .extern OS_InterruptHandler
    .type       OS_MedInt3,@function
    .align      4
OS_MedInt3:

    /*
    1)
     分配中断栈
     保存 pre-interrupt SP
     保存 interruptee's PS
     保存 interruptee's PC
     保存 interruptee's a0
     保存 exit point: OS_Medint3Exit
    */
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_3                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_3                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_3                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, OS_Medint3Exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /*
    2)保存其余的上下*/
    call0   XT_RTOS_INT_ENTER

    /*
    3)设置C调用环境
    */
    movi    a0, PS_INTLEVEL(3) | PS_UM | PS_WOE
    wsr     a0, PS
    rsync


    /*
    4)
    读入interrupt，得到未处理的level-3的比特
    从low到high处理所有level-3比特位
    并清除对应的中断位(如果是电平中断需要中断服务程序自行清除外部中断源，这里虽然也清除，实际无效)
    调用中断进入hook(中断号) OS_EnterIntHook
    调用中断处理接口OSInterruptHandler(中断号)
    调用中断退出hook(中断号) OS_ExitIntHook
    */
.L_ProcAllInt3:
    rsr     a2, INTENABLE
    rsr     a3, INTERRUPT
    movi    a4, XCHAL_INTLEVEL3_MASK
    and     a2, a2, a3
    and     a2, a2, a4
    beqz    a2, .L_int_done3

    neg     a3,a2
    and     a3,a3,a2
    wsr     a3,INTCLEAR

    find_ls_one a2, a3
    mov     a6,a2
    call4   OS_EnterIntHook

    mov     a6,a2
    call4   OS_InterruptHandler

    mov     a6,a2
    call4   OS_ExitIntHook
    j       .L_ProcAllInt3

.L_int_done3:

    /*
    5)
    中断退出*/
    call0   XT_RTOS_INT_EXIT


    .text
    .global     OS_Medint3Exit
    .type       OS_Medint3Exit,@function
    .align      4
OS_Medint3Exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_3
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_3
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     3


    .begin      literal_prefix .NMIExceptionVector
    .section    .NMIExceptionVector.text, "ax"
    .global     OS_NMIExceptionVector
    .align      8

OS_NMIExceptionVector:

	/* 保存a3,a4到EXC_SAVE寄存器 */
    wsr     a3, 213
    wsr     a4, 214
	
    /* 清除外部NMI中断源 */
    movi    a3, 0x0
    movi    a4, 0xf711a510
    s32i    a3, a4, 0
    
    /* 恢复a3,a4 */
    rsr     a3, 213
    rsr     a4, 214

    rfi     XCHAL_NMILEVEL

    .end        literal_prefix

    .section .WindowVectors.text, "ax"

/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call4.

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call4 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a3 are registers to be saved;
        a4-a15 must be preserved;
        a5 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x0
    .global _WindowOverflow4
_WindowOverflow4:

    s32e    a0, a5, -16     /* save a0 to call[j+1]'s stack frame */
    s32e    a1, a5, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a5,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a5,  -4     /* save a3 to call[j+1]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call4

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call4 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a3 are undefined, must be reloaded with call[i].reg[0..3];
        a4-a15 must be preserved (they are call[i+1].reg[0..11]);
        a5 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x40
    .global _WindowUnderflow4
_WindowUnderflow4:

    l32e    a0, a5, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a5, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a5,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a3, a5,  -4     /* restore a3 from call[i+1]'s stack frame */
    rfwu

    .global     OS_AllocaException
    .align      4

OS_AllocaException:

    rsr     a0, WINDOWBASE  /* grab WINDOWBASE before rotw changes it */
    rotw    -1              /* WINDOWBASE goes to a4, new a0-a3 are scratch */
    rsr     a2, PS
    extui   a3, a2, XCHAL_PS_OWB_SHIFT, XCHAL_PS_OWB_BITS
    xor     a3, a3, a4      /* bits changed from old to current windowbase */
    rsr     a4, EXCSAVE_1   /* restore original a0 (now in a4) */
    slli    a3, a3, XCHAL_PS_OWB_SHIFT
    xor     a2, a2, a3      /* flip changed bits in old window base */
    wsr     a2, PS          /* update PS.OWB to new window base */
    rsync

    _bbci.l a4, 31, _WindowUnderflow4
    rotw    -1              /* original a0 goes to a8 */
    _bbci.l a8, 30, _WindowUnderflow8
    rotw    -1
    j               _WindowUnderflow12



/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call8

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call8 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a7 are registers to be saved;
        a8-a15 must be preserved;
        a9 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x80
    .global _WindowOverflow8
_WindowOverflow8:

    s32e    a0, a9, -16     /* save a0 to call[j+1]'s stack frame */
    l32e    a0, a1, -12     /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1, a9, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a9,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a9,  -4     /* save a3 to call[j+1]'s stack frame */
    s32e    a4, a0, -32     /* save a4 to call[j]'s stack frame */
    s32e    a5, a0, -28     /* save a5 to call[j]'s stack frame */
    s32e    a6, a0, -24     /* save a6 to call[j]'s stack frame */
    s32e    a7, a0, -20     /* save a7 to call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call8

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call8 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a7 are undefined, must be reloaded with call[i].reg[0..7];
        a8-a15 must be preserved (they are call[i+1].reg[0..7]);
        a9 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0xC0
    .global _WindowUnderflow8
_WindowUnderflow8:

    l32e    a0, a9, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a9, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a9,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a7, a1, -12     /* a7 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3, a9,  -4     /* restore a3 from call[i+1]'s stack frame */
    l32e    a4, a7, -32     /* restore a4 from call[i]'s stack frame */
    l32e    a5, a7, -28     /* restore a5 from call[i]'s stack frame */
    l32e    a6, a7, -24     /* restore a6 from call[i]'s stack frame */
    l32e    a7, a7, -20     /* restore a7 from call[i]'s stack frame */
    rfwu

/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call12

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call12 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a11 are registers to be saved;
        a12-a15 must be preserved;
        a13 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x100
    .global _WindowOverflow12
_WindowOverflow12:

    s32e    a0,  a13, -16   /* save a0 to call[j+1]'s stack frame */
    l32e    a0,  a1,  -12   /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1,  a13, -12   /* save a1 to call[j+1]'s stack frame */
    s32e    a2,  a13,  -8   /* save a2 to call[j+1]'s stack frame */
    s32e    a3,  a13,  -4   /* save a3 to call[j+1]'s stack frame */
    s32e    a4,  a0,  -48   /* save a4 to end of call[j]'s stack frame */
    s32e    a5,  a0,  -44   /* save a5 to end of call[j]'s stack frame */
    s32e    a6,  a0,  -40   /* save a6 to end of call[j]'s stack frame */
    s32e    a7,  a0,  -36   /* save a7 to end of call[j]'s stack frame */
    s32e    a8,  a0,  -32   /* save a8 to end of call[j]'s stack frame */
    s32e    a9,  a0,  -28   /* save a9 to end of call[j]'s stack frame */
    s32e    a10, a0,  -24   /* save a10 to end of call[j]'s stack frame */
    s32e    a11, a0,  -20   /* save a11 to end of call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call12

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call12 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a11 are undefined, must be reloaded with call[i].reg[0..11];
        a12-a15 must be preserved (they are call[i+1].reg[0..3]);
        a13 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org 0x140
    .global _WindowUnderflow12
_WindowUnderflow12:

    l32e    a0,  a13, -16   /* restore a0 from call[i+1]'s stack frame */
    l32e    a1,  a13, -12   /* restore a1 from call[i+1]'s stack frame */
    l32e    a2,  a13,  -8   /* restore a2 from call[i+1]'s stack frame */
    l32e    a11, a1,  -12   /* a11 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3,  a13,  -4   /* restore a3 from call[i+1]'s stack frame */
    l32e    a4,  a11, -48   /* restore a4 from end of call[i]'s stack frame */
    l32e    a5,  a11, -44   /* restore a5 from end of call[i]'s stack frame */
    l32e    a6,  a11, -40   /* restore a6 from end of call[i]'s stack frame */
    l32e    a7,  a11, -36   /* restore a7 from end of call[i]'s stack frame */
    l32e    a8,  a11, -32   /* restore a8 from end of call[i]'s stack frame */
    l32e    a9,  a11, -28   /* restore a9 from end of call[i]'s stack frame */
    l32e    a10, a11, -24   /* restore a10 from end of call[i]'s stack frame */
    l32e    a11, a11, -20   /* restore a11 from end of call[i]'s stack frame */
    rfwu



