/* excArchLib.c - ARM exception handling facilities */

/*
 * Copyright (c) 1996-1998, 2001, 2003-2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */



/*
This module contains ARM architecture dependent portions of the
exception handling facilities.  See excLib for the portions that are
architecture independent.

SEE ALSO: dbgLib, sigLib, intLib, "Debugging"
*/

#include <vxWorks.h>
#include <vsbConfig.h>
#include <esf.h>
#include <iv.h>
#include <sysLib.h>
#include <intLib.h>
#include <taskLib.h>
#include <qLib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fppLib.h>
#include <vxLib.h>
#include <logLib.h>
#include <rebootLib.h>
#include <dbgLib.h>
#include <cacheLib.h>
#include <ioLib.h>
#include <private/intLibP.h>            /* intCnt */
#include <private/funcBindP.h>
#include <private/kernelLibP.h>
#include <private/taskLibP.h>
#include <private/sigLibP.h>
#include <private/vmLibP.h>
#include <edrLib.h>

#include <private/kernelBaseLibP.h> /* for IS_KERNEL_TASK() */

#ifndef _WRS_CONFIG_MMU_BASIC
#undef  _WRS_OSM_INIT
#endif  /* _WRS_CONFIG_MMU_BASIC */

#ifdef _WRS_HW_VFP_SUPPORT
#include <arch/arm/vfpArmLib.h>         /* VFP_CONTEXT */
#endif /* _WRS_HW_VFP_SUPPORT */

#define TEST_SWI_HANDLER 0

/* externals */

/*
 * The following imports are assembler routines which are put directly on the
 * hardware vectors (well almost directly - each vector contains a
 * LDR PC,[PC,#thing] to cause the PC to branch to these routines)
 */

IMPORT void excEnterUndef (void);
IMPORT void excEnterSwi (void);
IMPORT void excEnterPrefetchAbort (void);
IMPORT void excEnterDataAbort (void);
IMPORT void intEnt (void);
IMPORT void FIQ_SysInt (void);
IMPORT void armInitExceptionModes (void);
extern void FIQ_IntHandle();
/* globals */

/*
 * The following function pointers are used by the interrupt stubs. After
 * the saving of relevant registers and stack switching has been done,
 * the stub eventually calls the function installed on one of these vectors.
 */

VOIDFUNCPTR _func_armIrqHandler;    /* IRQ handler */
VOIDFUNCPTR _func_armMmuCurrentSet; /* Set MMU current TTBR */
FUNCPTR     _func_armMmuCurrentGet; /* Get MMU current TTBR */
VOIDFUNCPTR _func_excBreakpoint = NULL; /* Breakpoint handler */
UINT32 (* _func_mmuFaultAddrGet) (void);/* Get MMU fault address */

#ifdef _WRS_HW_VFP_SUPPORT
FUNCPTR _func_vfpExcHandle    = NULL;     /* VFP handler */
FUNCPTR _func_vfpHasException = NULL;
FUNCPTR _func_vfpSave         = NULL;
FUNCPTR _func_vfpIsEnabled    = NULL;
#endif /* _WRS_HW_VFP_SUPPORT */

IMPORT FUNCPTR excExcepHook;

#ifdef ARMCPU920T
#include <ffsLib.h>
#define MASK_AL         0xE0000000      /* Always execute conditional mask */
#define MASK_BLX        0x0FFFFFF0      /* Mask for detecting blx instruction */
#define INSTR_BLX       0x012FFF30      /* blx instruction */
#define DST_REG_BLX(x)  ( x & 0xF )     /* Macro for destination register */
#define MASK_CLZ        0x0FFF0FF0      /* Mask for detecting clz instruction */
#define INSTR_CLZ       0x016F0F10      /* clz instruction */
#define SRC_REG_CLZ(x)  ( x & 0xF )     /* Macro for clz source register */
#define DST_REG_CLZ(x)  ( (x & 0xF000) >> 12 ) /* Macro for clz destination register */
#define BUMP_PC(rSet)   ( rSet->pc = (INSTR *)(((ULONG)(rSet->pc) + 4)) )

LOCAL BOOL excExcHandleUndef (ESF *pEsf, REG_SET *pRegSet);
#endif /* ARMCPU920T */

/* local definitions */

typedef struct excTbl
    {
    UINT32  vecAddr;    /* vector address */
    VOIDFUNCPTR fn;     /* exception entry veneer */
    } EXC_TBL;

#define NUM_EXC_VECS 6
#define NUM_CHANGEABLE_EXC_VECS 6

/*
 * EXC_VEC_TABLE_BASE is the address of the table of addresses loaded
 * by LDR PC,<> instructions stored in the hardware exception vectors
 * The default value can be overrriden in a VSB configuration by setting
 * the value of ARM_VEC_TABLE_BASE
 */
#ifdef _WRS_CONFIG_ARCH_ARM_VEC_TABLE_BASE
#define EXC_VEC_TABLE_BASE _WRS_CONFIG_ARCH_ARM_VEC_TABLE_BASE
#else
#define EXC_VEC_TABLE_BASE 0x100
#endif

/* forward functions */

void excIntHandle (void);
void excExcHandle (ESF *, REG_SET *);
#if TEST_SWI_HANDLER
void excExcHandleSwi (ESF *, REG_SET *);
#endif  /* TEST_SWI_HANDLER */
LOCAL void excGetInfoFromESF (int, FAST ESF *, REG_SET *, EXC_INFO *);

#ifdef  _WRS_CONFIG_RTP
/* excVecUpdate is used to externally alter software interrupt if RTP enabled */

STATUS excVecUpdate (int i, VOIDFUNCPTR fn);
#endif  /* _WRS_CONFIG_RTP */


/* local variables */

/*
 * excEnterTbl is a table of vector addresses and routines which should
 * be installed. See excVecInit for more details.
 */

LOCAL EXC_TBL excEnterTbl[NUM_EXC_VECS] =
    {
    /* no entry for branch through zero */
    { EXC_OFF_UNDEF,    excEnterUndef},     /* undefined instr */
        { EXC_OFF_SWI,          excEnterSwi},           /* software interrupt */
    { EXC_OFF_PREFETCH,     excEnterPrefetchAbort}, /* prefetch abort */
    { EXC_OFF_DATA,     excEnterDataAbort}, /* data abort */
    /* no entry for old address exception */
    { EXC_OFF_IRQ,      intEnt},        /* interrupt request */

    { EXC_OFF_FIQ,      FIQ_SysInt},       /* interrupt request */
    };

#define FIRST_VECTOR    EXC_OFF_UNDEF


/*
 * excHandlerTbl is a table of handlers to be called from the exception stubs
 * NOTE: this table MUST be initialised such that the index into the
 * table for a particular exception is equal to the address of that
 * exception's vector >> 2.
 */

LOCAL EXC_TBL excHandlerTbl[NUM_CHANGEABLE_EXC_VECS] =
    {
    { EXC_OFF_RESET,    excExcHandle},  /* reset */
    { EXC_OFF_UNDEF,    excExcHandle},  /* undefined instruction */
#if TEST_SWI_HANDLER
    { EXC_OFF_SWI,      excExcHandleSwi},   /* software interrupt */
#else   /* TEST_SWI_HANDLER */
    { EXC_OFF_SWI,      excExcHandle},  /* software interrupt */
#endif  /* TEST_SWI_HANDLER */
    { EXC_OFF_PREFETCH, excExcHandle},  /* prefetch abort */
    { EXC_OFF_DATA,     excExcHandle},  /* data abort */
    { EXC_OFF_FIQ,      excExcHandle},  /* data abort */
    
    /* no entry for old address exception */
    /* no entries for IRQ/FIQ */
    };

/*******************************************************************************
*
* excVecBaseSet - Specify a non-zero vector base address
*
* This routine is used for processors that do not have zero-based exception
* vectors.  The argument specifies the location of the exception vector table.
*
* WHEN TO CALL
* This routine is usually called from sysHwInit0.
*
* RETURNS: N/A
*/

void excVecBaseSet (UINT32 base)
    {
    excEnterTbl[0].vecAddr = EXC_OFF_UNDEF    + base;
    excEnterTbl[1].vecAddr = EXC_OFF_SWI      + base;
    excEnterTbl[2].vecAddr = EXC_OFF_PREFETCH + base;
    excEnterTbl[3].vecAddr = EXC_OFF_DATA     + base;
    excEnterTbl[4].vecAddr = EXC_OFF_IRQ      + base;
    excEnterTbl[5].vecAddr = EXC_OFF_FIQ      + base;
    }


/*******************************************************************************
*
* excVecInit - initialize the exception/interrupt vectors
*
* This routine sets all exception vectors to point to the appropriate
* default exception handlers. All exception vectors are initialized to
* default handlers except 0x14 (Address) which is now reserved on the ARM
* and 0x1C (FIQ), which is not used by VxWorks.
*
* WHEN TO CALL
* This routine is usually called from the system start-up routine
* usrInit() in usrConfig, before interrupts are enabled.
*
* RETURNS: OK (always).
*/

STATUS excVecInit (void)
    {
    FAST int i;


    /* initialise ARM exception mode registers */
    armInitExceptionModes ();


    /* initialise hardware exception vectors */

    for (i = 0; i < NUM_EXC_VECS; ++i)
    {
    /*
     * Each vector contains a LDR PC,[PC,#offset] instruction to
     * load the PC from a table of addresses stored at
     * EXC_VEC_TABLE_BASE. This allows full 32 bit addressing rather
     * than 12 bit (MOV #) or 24 bit (B).
     */

#if (ARM_THUMB2)

#  ifdef BIG_ENDIAN
    *(UINT32 *)excEnterTbl[i].vecAddr = 0xF8DFF000 |
            (EXC_VEC_TABLE_BASE - 4 - FIRST_VECTOR);
#  else /*BIG_ENDIAN*/
    *(UINT32 *)excEnterTbl[i].vecAddr = 0xF000F8DF |
            (EXC_VEC_TABLE_BASE - 4 - FIRST_VECTOR) << 16;
#  endif /*BIG_ENDIAN*/

#else /*(ARM_THUMB2)*/

    *(UINT32 *)excEnterTbl[i].vecAddr = SWAP32_BE8(0xE59FF000 |
            (EXC_VEC_TABLE_BASE - 8 - FIRST_VECTOR));

#endif /*(ARM_THUMB2)*/


    *(VOIDFUNCPTR *)
        (excEnterTbl[i].vecAddr + EXC_VEC_TABLE_BASE - FIRST_VECTOR) =
                                excEnterTbl[i].fn;
    }


    /*
     * Branch through zero has to be handled differently if it is
     * possible for address 0 to be be branched to in ARM and Thumb
     * states (no LDR pc,[pc,#n] in Thumb state). The following
     * instruction, installed at address 0, will cause an undefined
     * instruction exception in both ARM and Thumb states.
     */

    *(UINT32 *)EXC_OFF_RESET = SWAP32_BE8(0xE7FDDEFE);


    /* now sort out the instruction cache to reflect the changes */

#if (CPU==XSCALE)

    /*
     * any call to single-line cache-invalidate could corrupt the
     * the visionProbe debug session of the visionTools on XScale...
     */

    CACHE_TEXT_UPDATE(EXC_OFF_RESET, ENTIRE_CACHE);
#else
    CACHE_TEXT_UPDATE(EXC_OFF_RESET, EXC_OFF_IRQ + 4);
#endif /* (CPU==XSCALE) */


    /* install default IRQ handler */

    _func_armIrqHandler = excIntHandle;

    return OK;
    }

#ifdef  _WRS_CONFIG_RTP
/*******************************************************************************
*
* excVecUpdate - update an exception/interrupt vector
*
* This routine sets an exception vector to point to the appropriate
* exception handler. All exception vectors are initialized to
* default handlers except 0x14 (Address) which is now reserved on the ARM
* and 0x1C (FIQ), which is not used by VxWorks.
*
* WHEN TO CALL
* This routine is usually called during the system start-up
* before interrupts are enabled.
*
* excVecUpdate is currently used to externally alter software interrupt if RTP
* is enabled.
*
* RETURNS: OK or ERROR.
*/

STATUS excVecUpdate (int i, VOIDFUNCPTR fn)
    {

    /* initialise hardware exception vector */

    if ( (i < 0) || (i >= NUM_EXC_VECS))
      return ERROR;
    else
      excEnterTbl[i].fn = fn;

    /*
     * Each vector contains a LDR PC,[PC,#offset] instruction to
     * load the PC from a table of addresses stored at
     * EXC_VEC_TABLE_BASE. This allows full 32 bit addressing rather
     * than 12 bit (MOV #) or 24 bit (B).
     */

    *(UINT32 *)excEnterTbl[i].vecAddr = SWAP32_BE8(0xE59FF000 |
                                    (EXC_VEC_TABLE_BASE - 8 - FIRST_VECTOR));
    *(VOIDFUNCPTR *)
        (excEnterTbl[i].vecAddr + EXC_VEC_TABLE_BASE - FIRST_VECTOR) =
                                                        excEnterTbl[i].fn;

    /* now sort out the instruction cache to reflect the changes */

#if (CPU==XSCALE)

    /*
     * any call to single-line cache-invalidate could corrupt the
     * the visionProbe debug session of the visionTools on XScale...
     */

    CACHE_TEXT_UPDATE(EXC_OFF_RESET, ENTIRE_CACHE);
#else
    CACHE_TEXT_UPDATE(EXC_OFF_RESET, EXC_OFF_IRQ + 4);
#endif /* (CPU==XSCALE) */

    return OK;
    }
#endif  /* _WRS_CONFIG_RTP */

/*******************************************************************************
*
* excIntConnect - connect a C routine to an asynchronous exception vector
*
* This routine connects a specified C routine to a specified asynchronous
* exception vector (IRQ). The address of <routine> is stored in a
* function pointer to be called by intEnt following an asynchronous
* exception.
*
* When the C routine is invoked the interrupt is still locked. It is the
* C routine responsibility to re-enable the interrupt.
*
* The routine can be any normal C code, except that it must not
* invoke certain operating system functions that may block or perform
* I/O operations
*
* NOTE:
* On the ARM, the address of <routine> is stored in a function pointer
* to be called by the stub installed on the IRQ exception vector
* following an asynchronous exception.  This routine is responsible for
* determining the interrupt source and despatching the correct handler
* for that source.
*
* Before calling the routine, the interrupt stub switches to SVC mode,
* changes to a separate interrupt stack and saves necessary registers. In
* the case of a nested interrupt, no SVC stack switch occurs.
*
* RETURNS: OK always.
*
* SEE ALSO: excVecSet().
*/

STATUS excIntConnect
    (
    VOIDFUNCPTR * vector,       /* exception vector to attach to */
    VOIDFUNCPTR   routine       /* routine to be called */
    )
    {
    if ((UINT32)vector != EXC_OFF_IRQ)
    return ERROR;

    _func_armIrqHandler = routine;
    return OK;
    }

#if TEST_SWI_HANDLER
/*******************************************************************************
*
* excExcHandleSwi - SWI handler for test purposes only
*
* NOMANUAL
*/

void excExcHandleSwi
    (
    ESF *   pEsf,   /* pointer to exception stack frame */
    REG_SET *   pRegs   /* pointer to register info on stack */
    )
    {
    ++(pRegs->pc);  /* step over SWI instruction */
    ++(pRegs->r[0]);    /* increment r0 */
    }

#endif  /* TEST_SWI_HANDLER */

/*******************************************************************************
*
* excExcHandle - interrupt level handling of exceptions
*
* This routine handles exception traps.  It is never to be called except
* from the special assembly language interrupt stub routine.
*
* It prints out a bunch of pertinent information about the trap that
* occurred via excTask.
*
* Note that this routine runs in the context of the task that got the exception.
*
* NOMANUAL
*/

void excExcHandle
    (
    ESF *   pEsf,   /* pointer to exception stack frame */
    REG_SET *   pRegs   /* pointer to register info on stack */
    )
    {
    EXC_INFO excInfo;
    int vec = pEsf->vecAddr;        /* exception vector */
    void * faultAddr;
    WIND_TCB *  pTcbCurrent;
#ifdef  _WRS_CONFIG_SMP
    WIND_TCB *  pTcbIdleTask;
#endif /* _WRS_CONFIG_SMP */

    _WRS_KERNEL_LOCKED_GLOBAL_GET(taskIdCurrent, pTcbCurrent);
#ifdef  _WRS_CONFIG_SMP
    _WRS_KERNEL_LOCKED_GLOBAL_GET(idleTaskId, pTcbIdleTask);
#endif /* _WRS_CONFIG_SMP */

    excGetInfoFromESF (vec, pEsf, pRegs, &excInfo); /* fill excInfo/pRegs */

    if ((_func_excBaseHook != NULL) &&          /* user hook around? */
    ((* _func_excBaseHook) (vec, pEsf, pRegs, &excInfo)))
    return;                     /* user hook fixed it */
#ifdef _WRS_CONFIG_SV_INSTRUMENTATION

    /* system viewer - level 3 event logging */

    EVT_CTX_1(EVENT_EXCEPTION, vec);
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */

    /*
     * If the exception happened at interrupt level, or prior to
     * kernel initialization, the ED&R policy handler will most
     * likely reboot the system.
     */

    if (INT_CONTEXT () || (KERNEL_ENTERED_ME () == TRUE))
    {
    EDR_INTERRUPT_INFO  intInfo;    /* information for system */
    char *                  errStr = NULL;
    int                     key;

#ifdef _WRS_CONFIG_SMP

    unsigned int            cpuIndex;
    cpuset_t                cpusetCpu;     /* CPU as a bit-field */

    /*
     * errBuf must be large enough to contain both
     * of the following error messages (65 bytes):
     * "exception while in kernel state!" and
     * "\nbad idle task state on CPU # X!".
     */

    char errBuf[68] = "";

#endif /* _WRS_CONFIG_SMP */

#ifdef  _WRS_CONFIG_FULL_EDR_STUBS
        /*
     * Error cases handled here are:
         *  INT_CONTEXT() == TRUE
         *      interrupt stack overflow or other interrupt-level exceptions
     *  !INT_CONTEXT() && kernelState == TRUE
     *      exception while in kernel state
     */

    if (INT_CONTEXT())
        errStr = "interrupt-level/stack exception!";
    else
        {
        errStr = "exception while in kernel state!";

# ifdef _WRS_CONFIG_SMP
            /*
             * If this CPU holds the kernel lock now, it must also have held
             * it when the exception occurred, thus no migration is possible
             * and we are still on the CPU where the exception occurred.
             * Check whether something bad has happened to the idle task as
             * this is perhaps the most likely cause of such an exception.
             */

        cpuIndex = _WRS_CPU_INDEX_GET ();
        CPUSET_ZERO (cpusetCpu);
        CPUSET_SET (cpusetCpu, cpuIndex); /* get CPU as a bit-field */

        if (pTcbIdleTask->status != WIND_READY ||
        pTcbIdleTask->windSmpInfo.affinity != cpusetCpu)
        {
        int where;

        strncpy (errBuf, errStr ,sizeof(errBuf));
        errBuf[sizeof(errBuf) -1] = EOS;
        
        strcat (errBuf, "\nbad idle task state on CPU # X!");
        where = strlen(errBuf) - 2;
        if (cpuIndex > 9)
            {
            errBuf[where - 1] = ((char)(cpuIndex / 10)) | '0';
            cpuIndex %= 10;
            }
        errBuf[where] = (char)cpuIndex | '0';
        errStr = &errBuf[0];
        }
# endif /* _WRS_CONFIG_SMP */
        }

#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */

    intInfo.vector      = vec;
    intInfo.pEsf        = (char *) pEsf;
    intInfo.pRegs       = pRegs;
    intInfo.pExcInfo    = &excInfo;
    intInfo.isException = TRUE;

    /*
     * Inject to ED&R, using EDR_INTERRUPT_FATAL_INJECT even for the
     * kernel-state cases since all of these conditions require a reboot.
     * Fake intCnt to cover the non-interrupt cases.
     */

    key = INT_CPU_LOCK ();
    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)++;

    EDR_INTERRUPT_FATAL_INJECT(&intInfo,
                   pRegs,
                   &excInfo,
                   (void *) pRegs->pc,
                   errStr);

    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)--;
    INT_CPU_UNLOCK(key);

    return;
    }

    if (pTcbCurrent == NULL)
      {
      EDR_INIT_INFO   initInfo;       /* information for system rebooting */
      char *          errStr = NULL;
      int             key;

    initInfo.vector     = vec;
    initInfo.pEsf       = (char *) pEsf;
    initInfo.pRegs      = pRegs;
    initInfo.pExcInfo   = &excInfo;
    initInfo.isException    = TRUE;

#ifdef  _WRS_CONFIG_FULL_EDR_STUBS
    errStr = "exception before root task!";
#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */

    /* inject to ED&R */

      key = INT_CPU_LOCK ();
      _WRS_KERNEL_GLOBAL_ACCESS (intCnt)++;

      EDR_INIT_FATAL_INJECT(&initInfo,
                            pRegs,
                            &excInfo,
                            (void *) pRegs->pc,
                            errStr);

     _WRS_KERNEL_GLOBAL_ACCESS (intCnt)--;
     INT_CPU_UNLOCK(key);

    return;
    }

    /* task caused exception */

#ifdef  _WRS_CONFIG_SMP
    {
   /* Exceptions that occur while holding a spinlock are fatal. */

    char *       errStr = NULL;
    int          key;
    unsigned int cpuIndex;

    key = INT_CPU_LOCK ();                  /* prevent migration */
    cpuIndex = _WRS_CPU_INDEX_GET ();

    if (_WRS_KERNEL_CPU_GLOBAL_GET (cpuIndex, spinLockIsrCnt) > 0)
#ifdef  _WRS_CONFIG_FULL_EDR_STUBS
    errStr = "exception while holding an ISR spinlock!";
#else   /* _WRS_CONFIG_FULL_EDR_STUBS */
    errStr = "I";
#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */
    else if (_WRS_KERNEL_CPU_GLOBAL_GET (cpuIndex, spinLockTaskCnt) > 0)
#ifdef  _WRS_CONFIG_FULL_EDR_STUBS
    errStr = "exception while holding a task spinlock!";
#else   /* _WRS_CONFIG_FULL_EDR_STUBS */
    errStr = "T";
#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */
    else
    errStr = NULL;

    INT_CPU_UNLOCK(key);                    /* migration OK */

    if (errStr)
    {
    /*
     * Use EDR_INTERRUPT_FATAL_INJECT (EDR_INTERRUPT_INFO) because an
     * exception while holding one of these locks requires a reboot.
     * EDR_KERNEL_FATAL_INJECT (EDR_TASK_INFO) would only stop the task.
     */

EDR_INTERRUPT_INFO  intInfo;        /* information for system */

    intInfo.vector      = vec;
    intInfo.pEsf        = (char *) pEsf;
    intInfo.pRegs       = pRegs;
    intInfo.pExcInfo    = &excInfo;
    intInfo.isException = TRUE;

    /*
     * edrInterruptFatalPolicyHandler() expects intCnt > 0, because
     * it would ordinarily be called from interrupt context.  Fake it,
     * and don't allow any actual interrupts while intCnt is bogus.
     */

    key = INT_CPU_LOCK ();
    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)++;

    EDR_INTERRUPT_FATAL_INJECT (&intInfo,
                                pRegs,
                                &excInfo,
                                (void *) pRegs->pc,
                                errStr);

    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)--;
    INT_CPU_UNLOCK(key);

    return;
    }
    }

    /*
     * There is one more fatal condition to check for:  an exception in the
     * context of an idle task requires a reboot because the idle task cannot
     * be stopped or suspended.  Idle tasks are supposed to have CPU affinity,
     * so if the exception did occur in an idle task we will still be on the
     * same CPU; thus we need only check whether currentTask matches our CPU's
     * idle task ID.  Granted this ignores the possibility of the idle task
     * having lost its affinity and then taken an exception, however this is
     * considered unlikely.
     *
     * By far the most likely cause for an exception in an idle task is a
     * problem in a task switch/swap hook, but since such hooks execute in
     * kernel state any exceptions in them will have been detected by the
     * (KERNEL_ENTERED_ME () == TRUE) test above.  An idle-task exception
     * detected here may have been caused by additions to the idleTaskEntry()
     * function, or by hardware problems.
     */

    if (pTcbCurrent == pTcbIdleTask)
    {
    char *             errStr = NULL;
    int                key;
    EDR_INTERRUPT_INFO intInfo;        /* information for system */

    intInfo.vector         = vec;
    intInfo.pEsf           = (char *) pEsf;
    intInfo.pRegs          = pRegs;
    intInfo.pExcInfo       = &excInfo;
    intInfo.isException    = TRUE;

#ifdef _WRS_CONFIG_FULL_EDR_STUBS
    errStr = "exception in idle task!";
#endif /* _WRS_CONFIG_FULL_EDR_STUBS */

    /*
     * edrInterruptFatalPolicyHandler() expects intCnt > 0, because
     * it would ordinarily be called from interrupt context.  Fake it,
     * and don't allow any actual interrupts while intCnt is bogus.
     */

    key = INT_CPU_LOCK ();
    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)++;

    EDR_INTERRUPT_FATAL_INJECT (&intInfo,
                                pRegs,
                                &excInfo,
                                (void *) pRegs->pc,
                                errStr);

    _WRS_KERNEL_GLOBAL_ACCESS (intCnt)--;
    INT_CPU_UNLOCK(key);

    return;
    }

#endif  /* _WRS_CONFIG_SMP */

    pTcbCurrent->pExcRegSet = pRegs;            /* for taskRegs[GS]et */

#ifdef  _WRS_CONFIG_SHELL
    taskIdDefault ((int)pTcbCurrent);           /* update default tid */
#endif  /* _WRS_CONFIG_SHELL */

    bcopy ((char *) &excInfo, (char *) &(pTcbCurrent->excInfo),
           sizeof (EXC_INFO));              /* copy in exc info */

    if (pEsf->vecAddr != EXC_OFF_DATA)
    faultAddr = (void *)((UINT32)pRegs->lrReg - 4);  /* instruction addr */
    else
        if (_func_mmuFaultAddrGet != NULL)
        {
        /*
         * Get the address that caused the data abort from the
         * MMU Fault Address Register (CP15_FAR).
         */
        faultAddr = (void *)(*_func_mmuFaultAddrGet)();
        }
    else
        {
        /*
         * Cannot determine the fault address without MMU and CP15_FAR, so
         * provide the address of the instruction that caused the fault.
         */
        faultAddr = (void *)((UINT32)pRegs->lrReg - 8);
        }

    /* An explanation of ED&R interaction with signals:-
     *
     * We invoke the signal hook first, and then inject an
     * error. If the signal hook doesn't return, its because
     * someone has taken care of the problem, in which case its
     * okay for ED&R not to worry about it.
     *
     * This has the advantage of allowing us to merge the
     * error-inject call and the policy invocation into one,
     * potentially.
     */

#ifdef  _WRS_CONFIG_RTP
    if (IS_KERNEL_TASK (pTcbCurrent))
#endif  /* _WRS_CONFIG_RTP */
    {
    if (_func_sigExcKill != NULL)
        (* _func_sigExcKill) ((int) vec, vec, pRegs);
    }
#ifdef  _WRS_CONFIG_RTP
    else
    {
    /* Do signal raise and delivery for RTP task */

    /*
     * Make sure the exception didn't happen in the kernel code
     * that is after the system call
     * in such case we do not run user handlers.
     */

    if (pTcbCurrent->excCnt == 1)
        {
        /* raise and deliver signal exception */

        if (_func_rtpSigExcKill != NULL)
        {
        (* _func_rtpSigExcKill) ((int) vec, vec, pRegs, faultAddr);

        /*
         * If signal got delivered never here.... otherwise
         * a) It is possible that User mode stack is full and
         *    signal handler context for RTP task could not be
         *    carved.  In such case apply default policy.
         * b) It is possible that the sigaction associated with
         *    this exception signal is SIG_IGN.  As per POSIX
         *    the behaviour onwards is undefined.
         *    We will continue and do the default policy.
         */

        }

        } /* else exception in syscall, continue */
    }
#endif  /* _WRS_CONFIG_RTP */

    /* Call the exception show routine if one has been installed */

    if (_func_excInfoShow != NULL)
    (*_func_excInfoShow) (&excInfo, TRUE, NULL, NULL);

    /* Invoke legacy exc-hook if installed, and if kernel task. */

    if (IS_KERNEL_TASK (pTcbCurrent))
    {
    if (excExcepHook != NULL)
        (* excExcepHook) (pTcbCurrent, vec, pEsf);
    }

    /* Now record the exception, since no signal-handler took it. */

#ifdef  _WRS_CONFIG_RTP
    if (_WRS_IS_SUPV_EXC ())
#endif  /* _WRS_CONFIG_RTP */
    {
    EDR_TASK_INFO   taskInfo;   /* information for task handling */
    char *      errStr = NULL;
#ifdef  _WRS_CONFIG_SMP
    char        buf [250];  /* buffer to format strings */
#endif /* _WRS_CONFIG_SMP */

    taskInfo.taskId     = (int) pTcbCurrent;
    taskInfo.status     = 0;
    taskInfo.vector     = vec;
    taskInfo.pEsf       = (char *) pEsf;
    taskInfo.pRegs      = pRegs;
    taskInfo.pExcInfo   = &excInfo;
    taskInfo.isException    = TRUE;

#ifdef  _WRS_CONFIG_FULL_EDR_STUBS
        /*
     * A fatal exception in a kernel task.
     *
     * if _errno != 0, it was either caused by an exception
     * stack overflow, or one occurred while constructing the
     * ESF.  (As of VxWorks 6.1 FCS, a non-zero pEsf->_errno
     * can only be S_excLib_EXCEPTION_STACK_OVERFLOW or
     * S_excLib_INTERRUPT_STACK_OVERFLOW; the latter is handled
     * above.)
     */

#ifdef  _WRS_CONFIG_SMP
    if (pTcbCurrent == pTcbIdleTask)
        {
        sprintf (buf, "fatal exception in the idle task of CPU #%d!", 
              (int) pTcbIdleTask->windSmpInfo.cpuIndex);
        strcat (errStr, buf);
        }
        else
        errStr = "fatal exception in a kernel task or stack overflow!";
#else
    errStr = "fatal exception in a kernel task or stack overflow!";
#endif  /* _WRS_CONFIG_SMP */

#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */

    EDR_KERNEL_FATAL_INJECT (&taskInfo,
                 pRegs,
                 &excInfo,
                 (void *) pRegs->pc,
                             errStr);
    }
#ifdef  _WRS_CONFIG_RTP
    else
    {
    EDR_RTP_INFO    rtpInfo;     /* information for rtp handling */
    char *      errStr = NULL;

    rtpInfo.rtpId       = pTcbCurrent->rtpId;
    rtpInfo.taskId      = (int) pTcbCurrent;
    rtpInfo.options     = 0;
    rtpInfo.status      = 0;
    rtpInfo.vector      = vec;
    rtpInfo.pEsf        = (char *) pEsf;
    rtpInfo.pRegs       = pRegs;
    rtpInfo.pExcInfo    = &excInfo;
    rtpInfo.isException = TRUE;

#ifdef  _WRS_CONFIG_FULL_EDR_STUBS 
    errStr = "fatal RTP exception!";
#endif  /* _WRS_CONFIG_FULL_EDR_STUBS */

    /* A fatal exception in an RTP task. */

    EDR_RTP_FATAL_INJECT (&rtpInfo,
                  pRegs,
                  &excInfo,
                  (void *) pRegs->pc,
                  errStr);
    }
#endif  /* _WRS_CONFIG_RTP */
    }

/*******************************************************************************
*
* excIntHandle - interrupt level handling of interrupts
*
* This routine handles interrupts.  It is never to be called except
* from the special assembly language interrupt stub routine.
*
* It prints out a bunch of pertinent information about the trap that
* occurred via excTask().
*
* NOMANUAL
*/

void excIntHandle ()
    {

    WIND_TCB *  pTcbCurrent;


#ifdef _WRS_CONFIG_SV_INSTRUMENTATION

    /* system viewer - level 3 event logging */

    EVT_CTX_1(EVENT_EXCEPTION, EXC_OFF_IRQ);
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */

    _WRS_KERNEL_LOCKED_GLOBAL_GET(taskIdCurrent, pTcbCurrent);

    if (_func_excIntHook != NULL)
    (*_func_excIntHook) ();

    if (_func_logMsg != NULL)
    _func_logMsg ("Uninitialized interrupt\n", 0,0,0,0,0,0);

    if (pTcbCurrent == NULL)                /* pre kernel */
    reboot (BOOT_WARM_AUTOBOOT);            /* better reboot */

    }

/*****************************************************************************
*
* excGetInfoFromESF - get relevent info from exception stack frame
*
*/

LOCAL void excGetInfoFromESF
    (
    int     vec,        /* vector */
    FAST ESF *  pEsf,       /* pointer to exception stack frame */
    REG_SET *   pRegs,      /* pointer to register info on stack */
    EXC_INFO *  pExcInfo    /* where to fill in exception info */
    )
    {
    WIND_TCB *  pTcbCurrent;

    pExcInfo->valid = EXC_INFO_VECADDR | EXC_INFO_PC | EXC_INFO_CPSR;
    pExcInfo->vecAddr = pEsf->vecAddr;
    pExcInfo->pc = pRegs->pc;
    pExcInfo->cpsr = pRegs->cpsr;

    _WRS_KERNEL_LOCKED_GLOBAL_GET(taskIdCurrent, pTcbCurrent);

    switch (pEsf->vecAddr)
    {
    case EXC_OFF_RESET: /* branch through zero */
        pExcInfo->valid ^= EXC_INFO_PC;     /* PC not valid */
        break;


    case EXC_OFF_UNDEF: /* undefined instruction */
#ifdef _WRS_HW_VFP_SUPPORT
        {
         if ((_func_vfpIsEnabled != NULL) && (_func_vfpIsEnabled ()) && \
         (_func_vfpHasException != NULL) && (_func_vfpHasException ()))
               {
                WIND_TCB * pTcb;
                COPROC_DESC * pDesc;
                COPROC_TBL_ENTRY * pTbl;

                pExcInfo->valid |= EXC_INFO_VFPCSR;

                /* search VFP context pointer and save VFP regs */

                if (((pTcb = taskTcb ((int) pTcbCurrent)) != NULL) &&
                    ((pTbl = pTcb->pCoprocTbl) != NULL))
                   {
                while ((pDesc = pTbl->pDescriptor) != NULL)
                     {
                      if (pDesc->mask & VX_VFP_TASK)
                        {
                         /* save VFP regs in VFP context */

            if (_func_vfpSave != NULL)
                _func_vfpSave ((VFP_CONTEXT *) pTbl->pCtx);

                         break;
                        }

                      pTbl++;
                     }
                   }
               }
            }
#endif /* _WRS_HW_VFP_SUPPORT */
    case EXC_OFF_SWI:   /* software interrupt */
    case EXC_OFF_PREFETCH:  /* prefetch abort */
        break;


    case EXC_OFF_DATA:  /* data abort */

        /*
         * note that registers may need unwinding if reexecution of
         * the instruction is to be attempted after the cause of
         * the abort has been fixed
         */

        break;


    default:        /* what else can there be? */
        break;
    }
    }

/*******************************************************************************
*
* excExcContinue - continue low level handling of exception
*
* This routine is called from the excEnter stubs to pass control to
* the required exception handler.
*
* Note that this routine runs in the context of the task that got the exception.
*
* NOMANUAL
*/

void excExcContinue
    (
    ESF *   pEsf,   /* pointer to exception stack frame */
    REG_SET *   pRegs   /* pointer to register info on stack */
    )
    {
    EXC_INFO excInfo;
    FAST UINT32 vec = pEsf->vecAddr;    /* exception vector */

    /*
     * call exception handler for this exception
     * if exception occurred at address 0, we don't care what sort of
     * exception it was - it MUST be a branch-through-zero (EXC_OFF_RESET).
     */

    if (pRegs->pc == 0)
    vec = pEsf->vecAddr = EXC_OFF_RESET;


    switch (vec)
    {
    case EXC_OFF_RESET: /* reset = branch through zero */
    case EXC_OFF_SWI:   /* software interrupt */
    case EXC_OFF_PREFETCH:  /* prefetch abort */
    case EXC_OFF_DATA:  /* data abort */
        (excHandlerTbl[vec >> 2].fn) (pEsf, pRegs);
        break;


    case EXC_OFF_UNDEF: /* undefined instruction */

        /*
         * check explicitly for an undefined instruction exception
         * caused by the undefined instruction we use as a breakpoint
         * and invoke its handler directly. WDB and DBG both use this
         * so only one can be installed at a time.
         */

        if (*(pRegs->pc) == DBG_BREAK_INST && _func_excBreakpoint != NULL)
                _func_excBreakpoint(pEsf, pRegs);
#ifdef ARMCPU920T
            else if ( excExcHandleUndef (pEsf, pRegs) == TRUE )
                break;
#endif /* ARMCPU920T */
#ifdef _WRS_HW_VFP_SUPPORT
            else if ((_func_vfpIsEnabled != NULL) &&
             (_func_vfpIsEnabled ()) &&
             (_func_vfpHasException != NULL) &&
             (_func_vfpHasException ()) &&
             (_func_vfpExcHandle != NULL) && _func_vfpExcHandle (pEsf, pRegs))
                break;
#endif /* _WRS_HW_VFP_SUPPORT */

            /* Default goes to excExcHandler */

            else
            (excHandlerTbl[vec >> 2].fn) (pEsf, pRegs);
        break;
        
    default:        /* should not happen - reboot */
        excGetInfoFromESF (vec, pEsf, pRegs, &excInfo);

        if (_func_excPanicHook != NULL)     /* panic hook? */
        (*_func_excPanicHook) (vec, pEsf, pRegs, &excInfo);

        reboot (BOOT_WARM_AUTOBOOT);
        break;

    }
    }

/*******************************************************************************
*
* excVecSet - set an exception vector
*
* This routine specifies the C routine that will be called when the exception
* corresponding to <vector> occurs.  This routine does not create the
* exception stub; it simply replaces the C routine to be called in the
* exception stub.
*
* NOTE
* On the ARM, there is no excConnect() routine, unlike the PowerPC. The C
* routine is attached to a default stub using excVecSet().
*
*
* SEE ALSO: excVecGet()
*/

void excVecSet
    (
    FUNCPTR * vector,   /* exception vector */
    FUNCPTR function    /* routine to be called */
    )
    {
    FAST int i;


    /*
     * find entry in table for this exception
     * NOTE: -1 because we don't put IRQ through here and FIQ isn't
     * in the table
     */

    for (i = 0; i < NUM_CHANGEABLE_EXC_VECS; ++i)
    if (excHandlerTbl[i].vecAddr == (UINT32)vector)
        break;

    if (i < NUM_CHANGEABLE_EXC_VECS)
    excHandlerTbl[i].fn = (VOIDFUNCPTR)function;    /* install handler */
    }


/*******************************************************************************
*
* excVecGet - get an exception vector
*
* This routine returns the address of the C routine currently connected to
* <vector>.
*
* SEE ALSO: excVecSet()
*/

FUNCPTR excVecGet
    (
    FUNCPTR * vector    /* exception vector */
    )
    {
    FAST int i;


    /*
     * find entry in table for this exception
     * NOTE: -1 because we don't put IRQ through here and FIQ isn't
     * in the table
     */

    for (i = 0; i < NUM_CHANGEABLE_EXC_VECS; ++i)
    if (excHandlerTbl[i].vecAddr == (UINT32)vector)
        return (FUNCPTR)excHandlerTbl[i].fn;

    return NULL;
    }

#ifdef  _WRS_OSM_INIT
/****************************************************************************
*
* excVmStateSet - change the state of a block of virtual memory
*
* This function will call VM_STATE_SET() that is a workaround for calling
* VM_STATE_SET() from assembly.
*
* RETURNS: OK, or ERROR if the validation fails, <virtAdrs> is not on a page
* boundary, <len> is not a multiple of the page size, the
* architecture-dependent state set fails for the specified virtual address,
* or vmLibInfo is not initialized.
*
* SEE ALSO: vmStateSet
*/

STATUS excVmStateSet
    (
    VM_CONTEXT_ID context,      /* context - NULL == currentContext */
    VIRT_ADDR     virtAdrs,     /* virtual address to modify state of */
    int           len,          /* len of virtual space to modify state of */
    UINT          stateMask,    /* state mask */
    UINT          state         /* state */
    )
    {
    return (VM_STATE_SET (context, virtAdrs, len, stateMask, state));
    }
#endif  /* _WRS_OSM_INIT */

#ifdef ARMCPU920T
/****************************************************************************
*
* excExcHandleUndef - Undefined instruction handler for the 920 running with
* v5 instructions.
*
* This routine detects v5 instructions being executed on the 920 and emulates
* the v5 instruction.
* RETURNS: TRUE if the instruction was emulated, FALSE if not.
*
* NOMANUAL
*/

LOCAL BOOL excExcHandleUndef
    (
    ESF *pEsf,
    REG_SET *pRegSet
    )
    {
    register ULONG cpsr;        /* Processor Status Register */
    register ULONG inst;        /* A copy of the instruction */
    register ULONG source;      /* source register */
    register ULONG destination; /* destination register */
    BOOL  conditionMet = FALSE;


    /* Get a copy of the instruction */

    inst = (ULONG)(*pEsf->pc);

    /* Check for blx or clz */

    if ( (inst & MASK_BLX) != INSTR_BLX
        && (inst & MASK_CLZ) != INSTR_CLZ )
        return ( FALSE);

    /* Get a copy of the CPSR */

    cpsr = pEsf->cpsr & MASK_CC;

    /*
     * Check for conditional execution
     */

    if( (inst & MASK_CC) != MASK_AL && (inst & MASK_CC) != cpsr )
        {

        /* Not unconditional and condition not met. */

        BUMP_PC(pRegSet);
        return ( TRUE );

        }

    /*
     * Check for blx
     */

    if ( (inst & MASK_BLX) == INSTR_BLX )
        {

        /* extract register */

        destination = DST_REG_BLX(inst);

        /* Get return pc for link register */

        source = (ULONG)pRegSet->pc;

        /* Update link register */

        pRegSet->lrReg = (INSTR)(source + 4);

        /* Set pc to branch location */

        pRegSet->pc = (INSTR *)(pRegSet->r[destination]);


        }

    /*
     * Check for clz
     */

    else if ( (inst & MASK_CLZ) == INSTR_CLZ )
        {

        /* Get source register */

        source = SRC_REG_CLZ(inst);

        /* Get destination register */

        destination = DST_REG_CLZ(inst);

        pRegSet->r[destination] = 32 - ffsMsb(pRegSet->r[source]);

        BUMP_PC(pRegSet);

        }
    else
        {

        /* Should never be here */

        return ( FALSE );

        }

    return ( TRUE );

    }
#endif /* ARMCPU920T */


