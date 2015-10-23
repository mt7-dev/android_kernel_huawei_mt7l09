/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_task.c                                                        */
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

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos_config.h"
#include "v_typdef.h"
#include "v_task.h"
#include "v_io.h"
#include "v_int.h"
#include "vos_id.h"
#include "v_lib.h"

#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#include "string.h"
#endif

#if (VOS_ZOS == VOS_OS_VER)

#include "kernel.h"
#include "ucom_comm.h"

#define    THIS_FILE_ID        FILE_ID_V_TASK_C

/* stack space, keep lint happy */
UCOM_SECTION(".os.stack.bss")
VOS_UINT8               g_ulTaskStack[VOS_TASK_STACK_SIZE]={0};


/* the current value of task's stack */
VOS_UINT32              g_ulCurrentStackAddress = (VOS_UINT32)(&g_ulTaskStack[0]);

/* start tick service or not */
VOS_UINT16              g_TickBootFlag = 0x5aa5;


/*****************************************************************************
 Function   : VOSTaskCreate
 Description: This function is used to have kernel manage the execution of a
              task. Tasks can either be created prior to the start of
              multitasking or by a running task.
              A task cannot be created by an ISR.
 Input      : task is a pointer to the task's code
            : pdata is a pointer to an optional data area which can be used
              to pass parameters to the task when the task first executes.
              Where the task is concerned it thinks it was invoked and passed
              the argument 'pdata' as follows:
            : ptos  is a pointer to the task's top of stack.
              If the configuration constant OS_STK_GROWTH is set to 1,
              the stack is assumed to grow downward (i.e. from high
              memory to low memory).  'pstk' will thus point to the highest
              (valid) memory location of the stack.
              If OS_STK_GROWTH is set to 0, 'pstk' will point to the lowest
              memory location of the stack and the stack will grow with
              increasing memory locations.
            : prio is the task's priority.
              A unique priority MUST be assigned to each task and the lower the
              number, the higher the priority.
 Return     : VOS_OK        if the function was successful.
              OS_PRIO_EXIT     if the task priority already exist
                               (each task MUST have a unique priority).
              OS_PRIO_INVALID  if the priority you specify is higher
              that the maximum allowed (i.e. >= OS_LOWEST_PRIO)
 Other      :
 *****************************************************************************/
VOS_UINT8 VOSTaskCreate(VOS_VOID (*task)(VOS_UINT32 d), VOS_UINT32 data,
                         VOS_STK *ptos, VOS_UINT8 prio)
{
    VOS_CPU_SR  cpu_sr;
    VOS_STK     *psp;
    VOS_UINT8   err;


#if VOS_ARG_CHK_EN > 0
    /* Make sure priority is within allowable range  */
    if (VOS_LOWEST_PRIO < prio)
    {
        return (VOS_ERRNO_PRIO_INVALID);
    }
#endif

    cpu_sr = VOS_SplIMP();

    /* Make sure task doesn't already exist at this priority  */
    if (g_pastVosTCBPrioTbl[prio] == (VOS_TCB *)0)
    {
        /* Reserve the priority to prevent others from doing ...  */
        g_pastVosTCBPrioTbl[prio] = (VOS_TCB *)1;

        VOS_Splx(cpu_sr);

        /* Initialize the task's stack */
        psp = (VOS_STK *)VOS_TaskStkInit(task, data, ptos, 0);
        err = VOS_TCBInit(prio, psp, ptos, 0, 0, (VOS_VOID *)0, 0);
        if (VOS_OK == err)
        {
            cpu_sr = VOS_SplIMP();
             /* Increment the #tasks counter  */
            g_ucVosTaskCtr++;
             VOS_Splx(cpu_sr);

            /* Find highest priority task if multitasking has started */
            if (g_bVosRunning == TRUE)
            {
                VOS_Sched();
            }
        }
        else
        {
            cpu_sr = VOS_SplIMP();
            /* Make this priority available to others  */
            g_pastVosTCBPrioTbl[prio] = (VOS_TCB *)0;
            VOS_Splx(cpu_sr);
        }

        return (err);
    }
    VOS_Splx(cpu_sr);

    return (VOS_ERRNO_PRIO_EXIST);
}

/*****************************************************************************
 Function   : VOSTaskReset
 Description: reset a task
 Input      : same as 'OSTaskCreate'
 Return     : same as 'OSTaskCreate'
 Other      :
 *****************************************************************************/
VOS_UINT8  VOSTaskReset(VOS_VOID (*task)(VOS_UINT32 d), VOS_UINT32 data,
                        VOS_STK *ptos, VOS_UINT8 prio)
{
    VOS_CPU_SR  cpu_sr;
    VOS_STK     *psp;
    VOS_UINT8   err;

#if VOS_ARG_CHK_EN > 0
    /* Make sure priority is within allowable range */
    if (VOS_LOWEST_PRIO < prio)
    {
        return (VOS_ERRNO_PRIO_INVALID);
    }
#endif

    cpu_sr = VOS_SplIMP();

    /* Make sure task already exist at this priority  */
    if (g_pastVosTCBPrioTbl[prio] != (VOS_TCB *)0)
    {
        VOS_Splx(cpu_sr);

        /* Initialize the task's stack */
        psp = (VOS_STK *)VOS_TaskStkInit(task, data, ptos, 0);
        err = VOS_TCBReset(prio, psp, (VOS_STK *)0, 0, 0, (VOS_VOID *)0, 0);

        return (err);
    }

    VOS_Splx(cpu_sr);

    return (VOS_ERRNO_TASK_NOT_EXIST);
}

/*****************************************************************************
 Function   : VOS_DspTaskEntry
 Description: task entry
 Input      : none
 Output     : none
 Return     : none
 Other      : none
 *****************************************************************************/
VOS_VOID VOS_DspTaskEntry( VOS_UINT32 ulArg )
{
    VOS_CPU_SR     CpuLockLevel;

    CpuLockLevel = VOS_SplIMP();

    if ( 0x5aa5 == g_TickBootFlag )
    {
        g_TickBootFlag = 0;

        /* start tick service */
        /*VOS_StartTick();*/
    }

    VOS_Splx(CpuLockLevel);

    (g_pastVosTCBPrioTbl[ulArg])->Function(
        (g_pastVosTCBPrioTbl[ulArg])->Args[0],(g_pastVosTCBPrioTbl[ulArg])->Args[1],
        (g_pastVosTCBPrioTbl[ulArg])->Args[2],(g_pastVosTCBPrioTbl[ulArg])->Args[3]);
}

/*****************************************************************************
 Function   : VOS_CreateTask
 Description: create task with default task mode:
              VOS_T_PREEMPT | VOS_T_NO_TSLICE | VOS_T_SUPV
 Input      : puchName              -- name identify task
              pfnFunc               -- task entry function
              ulPriority            -- task priority
              ulStackSize           -- task stack size
              aulArgs[VOS_TARG_NUM] -- arguments for task
 Output     : pulTaskID             -- task id allocated by dopra
 Return     : result of VOS_CreateTaskEx
 *****************************************************************************/
VOS_UINT32 VOS_CreateTask( VOS_UINT32 * pulTaskID,
                           VOS_TASK_ENTRY_TYPE pfnFunc,
                           VOS_UINT32 ulPriority,
                           VOS_UINT32 ulStackSize,
                           VOS_UINT32 aulArgs[VOS_TARG_NUM] )
{
    VOS_TCB         *ptcb;
    VOS_UINT32      ulBeginAddress;
    VOS_CPU_SR      CpuLockLevel;
    VOS_UINT16      i;
    VOS_UINT8       ucReturn;

    /* Allocate stack. I can not consider freeing space.
        Nothing be done for managing memory */
    CpuLockLevel = VOS_SplIMP();

    #if VOS_STK_CHK_EN
        #ifndef _lint
        memset((VOS_INT8 *)g_ulCurrentStackAddress,VOS_STK_FLG,ulStackSize);
        #endif
    #endif

    /* Init top of stack. avoid stack overflow */
    *(VOS_UINT32 *)g_ulCurrentStackAddress = VOS_MEMORY_CRC;



    g_ulCurrentStackAddress += ulStackSize;

    /*-4是由于SP所在的位置是用户数据，ZSP不一样，是无用数据*/
    ulBeginAddress = g_ulCurrentStackAddress - sizeof(VOS_UINT32);

    VOS_Splx(CpuLockLevel);

    /* create task */
    ucReturn = VOSTaskCreate (VOS_DspTaskEntry, ulPriority,
                            (VOS_STK *)ulBeginAddress, (VOS_UINT8) ulPriority);

    if( VOS_OK != ucReturn )
    {
        /* VOS_TaskCtrlBlkFree(iTid); *//* economize text */

        VOS_SetErrorNo(VOS_ERRNO_TASK_CREATE_OSALCREATEFAIL);
        return( VOS_ERRNO_TASK_CREATE_OSALCREATEFAIL );
    }

    /* Record information */
    ptcb = g_pastVosTCBPrioTbl[ulPriority];
    ptcb->Function = pfnFunc;
    /*ptcb->StackSize = ulStackSize;*//* economize text*/
    ptcb->StackAddress = ulBeginAddress;

    for ( i=0; i<VOS_TARG_NUM; i++ )
    {
        ptcb->Args[i] = aulArgs[i];
    }

    *pulTaskID = ulPriority;

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_TaskDelay
 Description: Let the task sleep specified millseconds, only tick precision
 Output     : None
 Input      : ulMillSecs: specified millseconds
 Return     : VOS_OK on success or error code on failed
 *****************************************************************************/
VOS_UINT32 VOS_TaskDelay( VOS_UINT32 ulMillSecs )
{
    VOS_CPU_SR  cpu_sr;
    VOS_UINT16  ticks;

    /* See if called from ISR ... */
    if (g_ucVosIntNesting > 0)
    {
        /* ... can't call from an ISR */
        return (VOS_ERR);
    }

    ticks = (VOS_UINT16)(ulMillSecs/MILLISECONDS_PER_TICK);

    /* 0 means no delay! */
    if (ticks > 0)
    {
        cpu_sr = VOS_SplIMP();

        /* Delay current task */
        if ((g_aucVosRdyTbl[g_pstVosTCBCur->OSTCBY] &= ~g_pstVosTCBCur->OSTCBBitX) == 0)
        {
            g_ucVosRdyGrp &= ~g_pstVosTCBCur->OSTCBBitY;
        }

        /* Load ticks in TCB */
        g_pstVosTCBCur->OSTCBDly = ticks;
        VOS_Splx(cpu_sr);

        /* Find next task to run! */
        VOS_Sched();
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_TaskLock
 Description: forbid task attemper.
 Input      : None
 Output     : None
 Return     : VOS_OK on success and others on failure
 Other      : This functin can not be called in interrupt routine.
 *****************************************************************************/
VOS_UINT32 VOS_TaskLock( VOS_VOID )
{
    VOS_CPU_SR  cpu_sr;

    /* See if called from ISR ... */
    if (g_ucVosIntNesting > 0)
    {
        /* ... can't call from an ISR */
        return (VOS_ERR);
    }

    /* Make sure multitasking is running  */
    if (g_bVosRunning == TRUE)
    {
        cpu_sr = VOS_SplIMP();

        /* Prevent OSLockNesting from wrapping back to 0 */
        if (g_ucVosLockNesting < 255)
        {
            /* Increment lock nesting level */
            g_ucVosLockNesting++;
        }

        VOS_Splx(cpu_sr);
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_TaskUnlock
 Description: permit task attemper.
 Input      : None
 Output     : None
 Return     : VOS_OK on success and others on failure
 Other      : This functin can not be called in interrupt routine.
 *****************************************************************************/
VOS_UINT32 VOS_TaskUnlock( VOS_VOID )
{
    VOS_CPU_SR  cpu_sr;

    /* See if called from ISR ... */
    if (g_ucVosIntNesting > 0)
    {
        /* ... can't call from an ISR */
        return (VOS_ERR);
    }

    /* Make sure multitasking is running */
    if (g_bVosRunning == TRUE)
    {
        cpu_sr = VOS_SplIMP();

        /* Do not decrement if already 0 */
        if (g_ucVosLockNesting > 0)
        {
            /* Decrement lock nesting level*/
            g_ucVosLockNesting--;

            /* See if sched. enabled and not an ISR */
            if ((g_ucVosLockNesting == 0) && (g_ucVosIntNesting == 0))
            {
                VOS_Splx(cpu_sr);

                /* See if a HPT is ready */
                VOS_Sched();
            }
            else
            {
                VOS_Splx(cpu_sr);
            }
        }
        else
        {
            VOS_Splx(cpu_sr);
        }
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_ResetTask
 Description: permit task attemper.
 Input      : None
 Output     : None
 Return     : VOS_OK on success and others on failure
 Other      : This functin can not be called in interrupt routine.
 *****************************************************************************/
VOS_UINT32 VOS_ResetTask( VOS_UINT32 ulTaskID )
{
    VOS_CPU_SR CpuLockLevel;
    VOS_UINT32 ulAddress;
    VOS_TCB    *ptcb;
    VOS_UINT8  ucReturn;

    if(VOS_LOWEST_PRIO < ulTaskID)
    {
        return (VOS_ERR);
    }

    ptcb = g_pastVosTCBPrioTbl[ulTaskID];

    ulAddress = ptcb->StackAddress;

    CpuLockLevel = VOS_SplIMP();

    /* restart task */
    ucReturn = VOSTaskReset(VOS_DspTaskEntry, ulTaskID,
        (VOS_STK *)(ulAddress), (VOS_UINT8)ulTaskID);

    VOS_Splx(CpuLockLevel);

    if ( VOS_OK != ucReturn )
    {
        Print1("# VOS_ResetTask errno is %d.\r\n", ucReturn);
        return (VOS_ERR);
    }

    return (VOS_OK);
}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


