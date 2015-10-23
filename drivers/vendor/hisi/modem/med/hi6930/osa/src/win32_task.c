/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: win32_task.c                                                    */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement win32 task                                         */
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
#include "v_IO.h"
#include "v_int.h"
#include "vos_Id.h"
#include "v_lib.h"



#if (VOS_WIN32 == VOS_OS_VER)

#define    THIS_FILE_ID        FILE_ID_V_WIN32_TASK_C

typedef struct
{
    VOS_UINT32          Flag;
    VOS_UINT32          Tid;/* task ID */
    VOS_TASK_ENTRY_TYPE Function;/* the entry of task */
    VOS_UINT32          Priority;
    VOS_UINT32          StackSize;
    VOS_UINT32          Args[VOS_TARG_NUM];/* the argument of the entry */
    HANDLE              Win32Handle;/* maped win32 handle */
    VOS_UINT32          ulWin32ThreadId;/* maped win32 thread id */
} VOS_TASK_CONTROL_BLOCK;

/* the number of task's control block */
VOS_UINT32              vos_TaskCtrlBlkNumber;

/* the start address of task's control block */
VOS_TASK_CONTROL_BLOCK *vos_TaskCtrlBlk;

/*****************************************************************************
 Function   : VOS_TaskCtrlBlkInit
 Description: Init task's control block
 Input      : ulTaskCtrlBlkNumber -- number
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_TaskCtrlBlkInit(VOS_UINT32 ulTaskCtrlBlkNumber)
{
    VOS_UINT32 i;

    vos_TaskCtrlBlkNumber = ulTaskCtrlBlkNumber;

    vos_TaskCtrlBlk = (VOS_TASK_CONTROL_BLOCK*)
                       malloc( ulTaskCtrlBlkNumber
                             * sizeof(VOS_TASK_CONTROL_BLOCK) );

    if(vos_TaskCtrlBlk == VOS_NULL_PTR)
    {
        Print("# VOS_TaskCtrlBlkInit malloc Error.\r\n");
        return(VOS_ERR);
    }

    for(i=0; i<vos_TaskCtrlBlkNumber; i++)
    {
        vos_TaskCtrlBlk[i].Flag       = CONTROL_BLOCK_IDLE;
        vos_TaskCtrlBlk[i].Tid        = i;
        vos_TaskCtrlBlk[i].Win32Handle= VOS_NULL_PTR;
    }
    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_TaskCtrlBlkGet
 Description: allocate a block
 Input      : VOS_VOID
            : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_TaskCtrlBlkGet(VOS_VOID)
{
    VOS_UINT32       i;
    VOS_CPU_SR       CpuLockLevel;

    CpuLockLevel = VOS_SplIMP();

    for(i=0; i<vos_TaskCtrlBlkNumber; i++)
    {
        if(vos_TaskCtrlBlk[i].Flag == CONTROL_BLOCK_IDLE)
        {
            vos_TaskCtrlBlk[i].Flag = CONTROL_BLOCK_BUSY;

            break;
        }
    }

    VOS_Splx(CpuLockLevel);

    if( i < vos_TaskCtrlBlkNumber)
    {
        return i;
    }
    else
    {
        Print("# allocate task control block fail.\r\n");

        return(VOS_NULL_DWORD);
    }
}

/*****************************************************************************
 Function   : VOS_TaskCtrlBlkFree
 Description: free a block
 Input      : Tid -- task ID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_TaskCtrlBlkFree(VOS_UINT32 Tid)
{
    VOS_CPU_SR              CpuLockLevel;

    if( Tid < vos_TaskCtrlBlkNumber )
    {
        if(vos_TaskCtrlBlk[Tid].Flag == CONTROL_BLOCK_IDLE)
        {
            Print("# VOS_TaskCtrlBlkFree free Idle Task.\r\n");

            return VOS_ERR;
        }
        else
        {
            CpuLockLevel = VOS_SplIMP();
            vos_TaskCtrlBlk[Tid].Flag = CONTROL_BLOCK_IDLE;
            VOS_Splx(CpuLockLevel);

            return VOS_OK;
        }
    }
    else
    {
        Print("# VOS_TaskCtrlBlkFree Error.\r\n");

        return VOS_ERR;
    }
}

/*****************************************************************************
 Function   : VOS_WIN32PriMap
 Description: Map WIN32 priority to OSAL priority
 Calls      :
 Called By  :
 Input      : ulTaskPriority    --  task's priority
 Output     : none
 Return     : task's OSAL priority
 Other      : none
******************************************************************************/
VOS_UINT32 VOS_WIN32PriMap( VOS_UINT32 ulTaskPriority )
{
    ulTaskPriority >>= 5;
    switch(ulTaskPriority)
    {
        case 0:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_IDLE;
            break;

        case 1:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_LOWEST;
            break;

        case 2:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case 3:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_NORMAL;
            break;


        case 4:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case 5:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_HIGHEST;
            break;

        case 6:
        case 7:
        default:
            ulTaskPriority = (VOS_UINT32) THREAD_PRIORITY_TIME_CRITICAL;
            break;
    }
    return ulTaskPriority;
}

/*****************************************************************************
 Function   : VOS_Win32TaskEntry
 Description: Get current task's ID
 Input      : none
 Output     : none
 Return     : Return task's ID if success or fail information if fail
 Other      : none
 *****************************************************************************/

VOS_VOID VOS_Win32TaskEntry( VOS_VOID * pulArg )
{
    VOS_TASK_CONTROL_BLOCK     *pstTemp;
    VOS_UINT32      ulPara1;
    VOS_UINT32      ulPara2;
    VOS_UINT32      ulPara3;
    VOS_UINT32      ulPara4;

    pstTemp = (VOS_TASK_CONTROL_BLOCK *)pulArg;


    ulPara1 = pstTemp->Args[0];
    ulPara2 = pstTemp->Args[1];
    ulPara3 = pstTemp->Args[2];
    ulPara4 = pstTemp->Args[3];

    pstTemp->Function( ulPara1, ulPara2, ulPara3, ulPara4 );
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
    VOS_UINT32      i;
    VOS_UINT32      iTid;
    HANDLE          pulTemp;
    VOS_UINT32      ulOsTaskPriority;

    iTid = VOS_TaskCtrlBlkGet();
    if( iTid == (VOS_UINT32)VOS_NULL_DWORD )
    {
        VOS_SetErrorNo(VOS_ERRNO_TASK_CREATE_NOFREETCB);
        return( VOS_ERRNO_TASK_CREATE_NOFREETCB );
    }

    *pulTaskID = iTid;

    vos_TaskCtrlBlk[iTid].Function   = pfnFunc;
    vos_TaskCtrlBlk[iTid].Priority   = ulPriority;
    vos_TaskCtrlBlk[iTid].StackSize  = ulStackSize;

    for ( i=0; i<VOS_TARG_NUM; i++ )
    {
        vos_TaskCtrlBlk[iTid].Args[i]    = aulArgs[i];
    }

    pulTemp = CreateThread( VOS_NULL_PTR, ulStackSize,
        ( LPTHREAD_START_ROUTINE )VOS_Win32TaskEntry,
        ( VOS_VOID * )&(vos_TaskCtrlBlk[iTid]), 0,
        &(vos_TaskCtrlBlk[iTid].ulWin32ThreadId)  );

    if( VOS_NULL_PTR == pulTemp )
    {
        VOS_TaskCtrlBlkFree(iTid);
        VOS_SetErrorNo(VOS_ERRNO_TASK_CREATE_OSALCREATEFAIL);
        return( VOS_ERRNO_TASK_CREATE_OSALCREATEFAIL );
    }

    vos_TaskCtrlBlk[iTid].Win32Handle = pulTemp;

    ulOsTaskPriority = VOS_WIN32PriMap( ulPriority );

    if ( VOS_NULL == SetThreadPriority( pulTemp, (VOS_INT)ulOsTaskPriority ) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_SuspendTask
 Description: Suspend a task
 Calls      : OSAL_SuspendTask
 Called By  :
 Input      : ulTaskID  --  id of a task
 Output     : none
 Return     : return VOS_OK if success
 Other      : none
 *****************************************************************************/
VOS_UINT32 VOS_SuspendTask( VOS_UINT32 ulTaskID )
{
    HANDLE             pulTemp;

    if( ulTaskID >= vos_TaskCtrlBlkNumber )
    {
        return(VOS_ERR);
    }
    else
    {
        pulTemp = vos_TaskCtrlBlk[ulTaskID].Win32Handle;
        if(VOS_NULL_PTR == pulTemp)
        {
            return(VOS_ERR);
        }
    }

    if ( VOS_NULL_DWORD == SuspendThread( pulTemp ) )
    {
        Print("# taskSuspend error.\r\n");
        return(VOS_ERR);
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_ResumeTask
 Description: Resume a task
 Input      : ulTaskID  --  ID of task
 Return     : Return VOS_OK if successd
 *****************************************************************************/
VOS_UINT32 VOS_ResumeTask( VOS_UINT32 ulTaskID )
{
    HANDLE             pulTemp;

    if(ulTaskID >= vos_TaskCtrlBlkNumber)
    {
        return(VOS_ERR);
    }
    else
    {
        pulTemp = vos_TaskCtrlBlk[ulTaskID].Win32Handle;
        if(VOS_NULL_PTR == pulTemp)
        {
            return(VOS_ERR);
        }
    }

    if ( VOS_NULL_DWORD == ResumeThread( pulTemp ) )
    {
        Print("# taskSuspend error.\r\n");
        return(VOS_ERR);
    }

    return VOS_OK;
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
    Sleep( ulMillSecs );
    return VOS_OK;
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
    /* VOS_SplIMP(); */

    return VOS_OK;
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
    /* VOS_Splx(0); */

    return VOS_OK;
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
    return VOS_OK;
}


VOS_VOID VOS_Win32ReleaseAllTask(VOS_VOID)
{
    VOS_UINT32 i;

    if(vos_TaskCtrlBlk == VOS_NULL_PTR)
    {
        return;
    }

    for(i=0; i<vos_TaskCtrlBlkNumber; i++)
    {
        if (vos_TaskCtrlBlk[i].Win32Handle != VOS_NULL_PTR)
        {
            TerminateThread(vos_TaskCtrlBlk[i].Win32Handle, 0);
            CloseHandle(vos_TaskCtrlBlk[i].Win32Handle);
        }
    }

    free(vos_TaskCtrlBlk);
    vos_TaskCtrlBlk = VOS_NULL_PTR;

}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



