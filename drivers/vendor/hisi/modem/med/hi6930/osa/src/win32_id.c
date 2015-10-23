/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_Id.c                                                        */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement FID&PID                                            */
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
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */



#include "v_fileid.h"
#include "vos_Id.h"
#include "v_iddef.inc"
#include "v_id.inc"
#include "v_int.h"
#include "v_lib.h"

#include "v_task.h"

#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#endif

#if (VOS_WIN32 == VOS_OS_VER) 

#include "cpu_c.h"
#define    THIS_FILE_ID        FILE_ID_V_WIN32_ID_C

/* recoed PID info VOS_PID_BUTT comes from v_id.inc */
VOS_INT                 vos_PidRecordsNumber;
VOS_PID_RECORD          vos_PidRecords[VOS_PID_BUTT - VOS_PID_DOPRAEND];

/* recoed FID info VOS_FID_BUTT comes from v_id.inc */
VOS_INT                 vos_FidCtrlBlkNumber;
VOS_FID_CONTROL_BLOCK   vos_FidCtrlBlk[VOS_FID_BUTT];

#if (VOS_ZOS == VOS_OS_VER)
/* the map of PRI between FID and VxWorks or Nucleus */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
#endif
	
/* the map of PRI between FID and Win32 */
VOS_UINT32 vos_FidTaskRealPriority[VOS_PRIORITY_NUM]
    = { 33, 64, 65, 97, 128, 129, 161, 192, 193, 224, 225, 32, 96, 160 };

/* default value of task stack size */
#define TASK_STACK_SIZE                      32768

HANDLE					evtHighTask;
HANDLE					evtLowTask;
HANDLE					evtNormalTask;
HANDLE					evtTerminate;
VOS_UINT32				HighTaskNum=0;
VOS_UINT32				NormalTaskNum=0;
VOS_UINT32				LowTaskNum=0;

/*****************************************************************************
 Function   : MOD_RegFidPidRSP
 Description: map the PID and FID
 Input      : ulPID -- PID
            : ulFID -- the belone of PID
            : priority -- PID's priority
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 MOD_RegFidPidRSP( VOS_UINT32 ulPID, VOS_UINT32 ulFID)
{
    if(!VOS_PidCheck(ulPID))
    {
        VOS_SetErrorNo(VOS_ERRNO_REG_FID_PID_INVALID_PID);
        return(VOS_ERRNO_REG_FID_PID_INVALID_FID);
    }

    if( VOS_NULL_DWORD != vos_PidRecords[VOS_PidIdx(ulPID)].Pid )
    {
        Print("# VOS_RegisterPIDInfo multiple register.\r\n");
    }

    vos_PidRecords[VOS_PidIdx(ulPID)].Pid = ulPID;
    vos_PidRecords[VOS_PidIdx(ulPID)].Fid = ulFID;

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_RegisterPIDInfo
 Description: record PID info
 Input      : ulPID -- PID
            : pfnInitFun -- Init FUN of the pid
            : pfnMsgFun -- process functin of the pid
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_RegisterPIDInfo( VOS_PID ulPID,
                                Init_Fun_Type pfnInitFun,
                                Msg_Fun_Type pfnMsgFun )
{
    if(!VOS_PidCheck(ulPID))
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGPIDI_INPUTPIDINVALID);
        return(VOS_ERRNO_FIDPID_REGPIDI_INPUTPIDINVALID);
    }

    if(VOS_NULL_PTR != pfnInitFun)
    {
        vos_PidRecords[VOS_PidIdx(ulPID)].InitFunc = pfnInitFun;
    }

    if(VOS_NULL_PTR == pfnMsgFun)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGPIDI_INPUTFUNCARENULL);
        return(VOS_ERRNO_FIDPID_REGPIDI_INPUTFUNCARENULL);
    }

    vos_PidRecords[VOS_PidIdx(ulPID)].MsgFunction = pfnMsgFun;

    return(VOS_OK);
}

/*****************************************************************************
 Function   : PidInitFuncDefault
 Description: the default Init function of pid
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 PidInitFuncDefault ( enum VOS_INIT_PHASE_DEFINE InitPhrase )
{
    return( VOS_OK );
}

/*****************************************************************************
 Function   : MsgProcFuncDefault
 Description: the default process function of pid
 Input      : pMsg -- the processing message
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID MsgProcFuncDefault( MsgBlock * pMsg )
{
    return;
}

/*****************************************************************************
 Function   : VOS_PidCtrlBlkInit
 Description: Init the control block of PID
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidCtrlBlkInit(VOS_VOID)
{
    VOS_UINT32 i;

    for(i=0; i<(VOS_PID_BUTT - VOS_PID_DOPRAEND); i++)
    {
        vos_PidRecords[i].Pid          = VOS_NULL_DWORD;
        vos_PidRecords[i].Fid          = VOS_NULL_DWORD;

        vos_PidRecords[i].InitFunc     = PidInitFuncDefault;
        vos_PidRecords[i].MsgFunction  = MsgProcFuncDefault;
    }

    /* which comes from v_id.inc */
    REG_FID_PID_RSP();

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_PidsInitOnePhase
 Description: one step of PID initialization
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidsInitOnePhase( enum VOS_INIT_PHASE_DEFINE InitPhrase)
{
    VOS_UINT32      i;
    VOS_UINT32      ulReturnValue, ulErrorCounter;

    ulErrorCounter  = 0;

    for(i=0; i<(VOS_PID_BUTT - VOS_PID_DOPRAEND); i++)
    {
        if( VOS_NULL_PTR == vos_PidRecords[i].InitFunc )
        {
            continue;
        }

        ulReturnValue = vos_PidRecords[i].InitFunc(InitPhrase);

        if( VOS_OK != ulReturnValue )
        {
            Print("# VOS_PidsInitOnePhase Error.\r\n");
            ulErrorCounter++;
        }
    }

    return(ulErrorCounter);
}

/*****************************************************************************
 Function   : VOS_PidsInit
 Description: Init PID
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_PidsInit(VOS_VOID)
{
    VOS_UINT32 ulStartUpFailStage = 0;

    VOS_UINT32 ulInitPhase;

    for (ulInitPhase = VOS_IP_LOAD_CONFIG;
         ulInitPhase <= VOS_IP_BUTT; ulInitPhase++)
    {
        if ( VOS_OK
            != VOS_PidsInitOnePhase( (enum VOS_INIT_PHASE_DEFINE)ulInitPhase) )
        {
            Print("# VOS_PidsInitOnePhase VOS_IP_LOAD_CONFIG Error.\r\n");

            ulStartUpFailStage |= ulInitPhase;

            return(ulStartUpFailStage);
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : FidInitFuncDefault
 Description: default init function of FID
 Input      : one step of Init
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 FidInitFuncDefault ( enum VOS_INIT_PHASE_DEFINE InitPhrase )
{
    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_FidCtrlBlkInit
 Description: Init the control block of FID
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidCtrlBlkInit(VOS_VOID)
{
    VOS_UINT32               i,j;
    VOS_UINT32               ulTotalStackSize = 0;
    VOS_FID_CONTROL_BLOCK    *pFidCtrlBlk = vos_FidCtrlBlk;
    FID_Static_Table_Type    *pFidStaticTable;

    for(i=0; i<VOS_FID_BUTT; i++)
    {
        pFidCtrlBlk->Fid = i;

        pFidCtrlBlk->InitFunc = FidInitFuncDefault;
        pFidCtrlBlk->Priority = vos_FidTaskRealPriority[VOS_PRIORITY_BASE];
        pFidCtrlBlk->StackSize  = TASK_STACK_SIZE;
        pFidCtrlBlk->MaxMsgNumber = VOS_FID_QUEUE_LENGTH;

        pFidCtrlBlk++;
    }

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        pFidCtrlBlk = &vos_FidCtrlBlk[i];
        pFidStaticTable = &g_aFidStaticTable[i];

        if(pFidStaticTable->ulFID != (VOS_FID)i)
        {
            Print2("# Fid Init Fid invalid ulFID %ld I %ld",
                pFidStaticTable->ulFID, i );

            return(VOS_ERR);
        }

        pFidCtrlBlk->InitFunc = pFidStaticTable->pfnInitFun;

        if( 0 != pFidStaticTable->usStackSize )
        {
            pFidCtrlBlk->StackSize = pFidStaticTable->usStackSize;
        }

        ulTotalStackSize += pFidCtrlBlk->StackSize;

        if( 0 != pFidStaticTable->usMsgNumber)
        {
            pFidCtrlBlk->MaxMsgNumber = pFidStaticTable->usMsgNumber;
        }

        for(j=0; j<(VOS_PID_BUTT - VOS_PID_DOPRAEND); j++)
        {
            if( vos_PidRecords[j].Fid == i )
            {
                pFidCtrlBlk->PidsBelong[j] = VOS_ID_PID_BELONG_TO_FID;
            }
            else
            {
                pFidCtrlBlk->PidsBelong[j] = VOS_ID_PID_NOT_BELONG_TO_FID;
            }
        }
    }

    /* Added for checking stack */
    if ( VOS_TASK_STACK_SIZE < ulTotalStackSize )
    {
        Print1("# Fid Init stack too small %ld.\r\n", ulTotalStackSize);

        return(VOS_ERR);
    }

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_FidsInitOnePhase
 Description: one step of FID initialization
 Input      : InitPhrase -- the step of INit
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidsInitOnePhase(enum VOS_INIT_PHASE_DEFINE InitPhrase)
{
    VOS_UINT32      i;
    VOS_UINT32      ulReturnValue, ulErrorCounter;

    ulErrorCounter  = 0;

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        if(VOS_NULL_PTR == vos_FidCtrlBlk[i].InitFunc)
        {
            continue;
        }

        ulReturnValue = vos_FidCtrlBlk[i].InitFunc(InitPhrase);

        if(VOS_OK != ulReturnValue)
        {
            Print3("# InitPhrase %ld i %ld ulReturnValue %ld",
                InitPhrase, i, ulReturnValue);

            ulErrorCounter++;
        }
    }

    return(ulErrorCounter);
}

/*****************************************************************************
 Function   : VOS_FidsInit
 Description: Init PID
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_FidsInit(VOS_VOID)
{
    VOS_UINT32 ulStartUpFailStage = 0;

    VOS_UINT32 ulInitPhase;

    for (ulInitPhase = VOS_IP_LOAD_CONFIG;
         ulInitPhase <= VOS_IP_BUTT; ulInitPhase++)
    {
        if (VOS_OK
            != VOS_FidsInitOnePhase( (enum VOS_INIT_PHASE_DEFINE)ulInitPhase) )
        {
            Print("# VOS_FidsInitOnePhase VOS_IP_LOAD_CONFIG Error.\r\n");

            ulStartUpFailStage |= ulInitPhase;

            return(ulStartUpFailStage);
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_RegisterMsgTaskPrio
 Description: Register priority of FID message handling task.
 Input      : ulFID    -- Function module Identifier
            : TaskPrio -- priority of normal message handling task
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_RegisterMsgTaskPrio( VOS_FID ulFID,
                                    enum VOS_PRIORITY_DEFINE TaskPrio )
{
    if(ulFID >= VOS_FID_BUTT)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID);
    }

    if(TaskPrio > VOS_PRIORITY_P6)
    {
        VOS_SetErrorNo(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
        return(VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID);
    }

    vos_FidCtrlBlk[ulFID].Priority = vos_FidTaskRealPriority[TaskPrio];

    return(VOS_OK);
}

/*****************************************************************************
 Function   : CreateFidsQueque
 Description: create queue of one FID
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 CreateFidsQueque(VOS_VOID)
{
    VOS_UINT32          i;
    VOS_UINT32          ulReturnValue;

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        ulReturnValue   = VOS_FixedQueueCreate( vos_FidCtrlBlk[i].MaxMsgNumber,
                                                &(vos_FidCtrlBlk[i].Qid));

        if( VOS_OK != ulReturnValue )
        {
            Print1("# create %ld FID queue error.\r\n", i);

            return( ulReturnValue );
        }
    }

    return( VOS_OK );
}


//VOS_MSG_HOOK_FUNC   vos_MsgHook = VOS_NULL_PTR;

/*****************************************************************************
 Function   : VOS_RegisterMsgGetHook
 Description: Register the hook for VOS_GetMsg
 Input      : pfnMsgHook -- Hook function
 Return     : Old function hook
 Other      : This hook would be called after getting message by system.
              If VOS_OK was returned by the hook function, the message
              processing would go on, otherwise the message would be
              thrown away and the corresponding handling function for this
              message would not be called.
 *****************************************************************************/
/*VOS_MSG_HOOK_FUNC VOS_RegisterMsgGetHook( VOS_MSG_HOOK_FUNC pfnMsgHook )
{
    VOS_MSG_HOOK_FUNC   oldMsgHook;

    oldMsgHook          = vos_MsgHook;
    vos_MsgHook         = pfnMsgHook;

    return(oldMsgHook);
}*/

/*****************************************************************************
 Function   : vos_FidTask
 Description: the process entry of every FID
 Input      : ulQueueID -- the queue of the FID
            : the ID of the FID
 Return     : VOS_VOID
 *****************************************************************************/
VOS_VOID vos_FidTask( VOS_UINT32 ulQueueID, VOS_UINT32 FID_value,
                      VOS_UINT32 Para1, VOS_UINT32 Para2 )
{

    MSG_CB                  *pMsg;
    VOS_UINT32              iThePid;
    VOS_UINT32              Msg_Address;

    for ( ; ; )
    {
        if ( VOS_ERR == VOS_FixedQueueRead(ulQueueID, &Msg_Address))
        {
            Print1("# FID fetch message error. the Q ID is %ld.\r\n",
                (VOS_INT)ulQueueID);

            continue;
        }

        Msg_Address += VOS_MSG_BLK_HEAD_LEN;

        pMsg = (MSG_CB *)Msg_Address;

        iThePid = pMsg->uwReceiverPid;

        if(!VOS_PidCheck(iThePid))
        {
            Print("# vos_FidTask Pid too big.\r\n");

            VOS_FreeMsg( iThePid, pMsg );

            continue;
        }

        if( VOS_ID_PID_BELONG_TO_FID
            == vos_FidCtrlBlk[FID_value].PidsBelong[VOS_PidIdx(iThePid)] )
        {
#ifdef _TEST_DSDA_
			//check 3 task status   
			//suspend low priority, carry out high priority
			//resume low priority if high task complete
			if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_RtFidInit)
			{
				HighTaskNum++;
				if (NormalTaskNum !=0 )
				{
					VOS_SuspendTask(1);
				}
				
				if (LowTaskNum != 0)
				{
					VOS_SuspendTask(2);
				}		
				
			}
			else if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_NormalFidInit)
			{
				NormalTaskNum++;
				if (HighTaskNum != 0)
				{
					//continue;
					WaitForSingleObject(evtHighTask,INFINITE);
				}

				if (LowTaskNum != 0)
				{
					VOS_SuspendTask(2);
				}	
			}
			else if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_LowFidInit)
			{
				LowTaskNum++;
				if ((HighTaskNum != 0) || (NormalTaskNum != 0))
				{
					//continue;
					WaitForSingleObject(evtHighTask,INFINITE);   //block this for cycle, can't receive next message
					WaitForSingleObject(evtNormalTask,INFINITE);
				}
			}
#endif
            /* 接收消息时调用hook */
            if ( VOS_NULL_PTR != g_pfVosHookFuncTable[OS_HOOK_TYPE_MSG_GET] )
            {
                ((VOS_MSG_HOOK_FUNC)g_pfVosHookFuncTable[OS_HOOK_TYPE_MSG_GET])(pMsg);
            }

            (vos_PidRecords[VOS_PidIdx(iThePid)].MsgFunction)(pMsg);
			
			//resume task environment
			if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_RtFidInit)
			{
				HighTaskNum--;
				if (NormalTaskNum !=0 )
				{
					VOS_ResumeTask(1);
				}
				
				if (LowTaskNum != 0)
				{
					VOS_ResumeTask(2);
				}   							
				SetEvent(evtHighTask);
				SetEvent(evtNormalTask);
			}
			else if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_NormalFidInit)
			{
				NormalTaskNum--;

				if (LowTaskNum != 0)
				{
					VOS_ResumeTask(2);
				}  

				VOS_ResumeTask(2);
				SetEvent(evtHighTask);
				SetEvent(evtNormalTask);
			}
			else if (vos_FidCtrlBlk[FID_value].InitFunc == UCOM_COMM_LowFidInit)
			{
				LowTaskNum--;
			}

        }
        else
        {
            Print("# vos_FidTask Pid not belong the Fid.\r\n");
        }

		if (HighTaskNum == 0 && NormalTaskNum == 0 && LowTaskNum == 0)
		{
			SetEvent(evtTerminate);
		}

        VOS_FreeMsg( iThePid, pMsg );
    }
}

/*****************************************************************************
 Function   : CreateFidsTask
 Description: create tasks of all FIDs
 Input      : VOS_VOID
            : VOS_VOID
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 CreateFidsTask(VOS_VOID)
{
    VOS_UINT32              i;
    VOS_UINT32              ulReturnValue;
    VOS_UINT32              TaskArgument[VOS_TARG_NUM];

	evtTerminate = CreateEvent(NULL, FALSE, FALSE, "TERMINATE");
	evtHighTask = CreateEvent(NULL, FALSE, FALSE, "HIGHTASK");
	evtLowTask = CreateEvent(NULL, FALSE, FALSE, "LOWTASK");
	evtNormalTask = CreateEvent(NULL, FALSE, FALSE, "NORMALTASK");

    for(i=VOS_FID_DOPRAEND; i<VOS_FID_BUTT; i++)
    {
        TaskArgument[0] = vos_FidCtrlBlk[i].Qid;
        TaskArgument[1] = i;
        ulReturnValue = VOS_CreateTask(&(vos_FidCtrlBlk[i].Tid),
                                       vos_FidTask, vos_FidCtrlBlk[i].Priority,
                                       vos_FidCtrlBlk[i].StackSize, TaskArgument);
        if( VOS_OK != ulReturnValue )
        {
            Print("# VOS_CreateTask Fail in CreateFidsTask.\r\n");
            return( ulReturnValue );
        }
    }

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_ResetFidsTask
 Description: Resume tasks of all FIDs
 Input      : VOS_VOID
            : VOS_VOID
 Return     : VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_ResetFidsTask(VOS_FID ulFid)
{
    VOS_CPU_SR              CpuLockLevel;
    VOS_UINT32              iThePid;
    VOS_UINT32              Msg_Address;
    MSG_CB                 *pMsg;

    if( (ulFid >= VOS_FID_BUTT) || (ulFid < VOS_FID_DOPRAEND) )
    {
        return VOS_ERR;
    }

    CpuLockLevel = VOS_SplIMP();

    if ( VOS_OK != VOS_FixedQueueClean(vos_FidCtrlBlk[ulFid].Qid, &Msg_Address) )
    {
        VOS_Splx(CpuLockLevel);

        return VOS_ERR;
    }

    if ( VOS_OK != VOS_ResetTask(vos_FidCtrlBlk[ulFid].Tid) )
    {
        VOS_Splx(CpuLockLevel);

        return VOS_ERR;
    }

    if ( FALSE == VOS_IsMemUsed(Msg_Address) )
    {
        VOS_Splx(CpuLockLevel);

        return VOS_OK;
    }

    Msg_Address += VOS_MSG_BLK_HEAD_LEN;

    pMsg         = (MSG_CB *)Msg_Address;

    iThePid      = pMsg->uwReceiverPid;

    if(!VOS_PidCheck(iThePid))
    {
        VOS_Splx(CpuLockLevel);

        return VOS_ERR;
    }

    if( VOS_ID_PID_BELONG_TO_FID
        == vos_FidCtrlBlk[ulFid].PidsBelong[iThePid] )
    {
        VOS_FreeMsg( iThePid, pMsg );
    }


    VOS_Splx(CpuLockLevel);

    return VOS_OK;
}

#if VOS_STK_CHK_EN
extern VOS_UINT8               g_ulTaskStack[VOS_TASK_STACK_SIZE];
VOS_UINT32 GetFidTaskSize(VOS_FID uwFId)
{
    return  g_aFidStaticTable[uwFId].usStackSize;
}
/*****************************************************************************
 函 数 名  : VOS_GetMaxStackUsed
 功能描述  : 计算FID对应当前堆栈使用量
 输入参数  : VOS_FID uwFId : uwFId=task fid,IDLE task ->254,INTR ->255
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年11月19日
    作    者   : guodongfeng
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 VOS_GetMaxStackUsed(VOS_FID uwFId)
{
    VOS_UINT16 uhwLen,i;
    VOS_UINT32 *puwCurStk;

    if (uwFId >= VOS_FID_BUTT)
    {
        if (IDLE_STK_ID == uwFId)
        {
            puwCurStk = (VOS_UINT32 *)(&g_stVosTaskIdleStk[0]);
            /*bytes of stack*/
            uhwLen = VOS_TASK_IDLE_STK_SIZE;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        puwCurStk = (VOS_UINT32 *)(&g_ulTaskStack[0]);
        uhwLen = g_aFidStaticTable[1].usStackSize;  /* 第0个为无效值，从1开始 */

        for (i = 2; i <= uwFId; i++)
        {
            puwCurStk = (VOS_UINT32 *)(((VOS_UINT32)puwCurStk) + uhwLen);
            uhwLen = g_aFidStaticTable[i].usStackSize;
        }
    }

    uhwLen = uhwLen /sizeof(VOS_UINT32);
    for (i = 1; i < uhwLen; i++)
    {
        if (puwCurStk[i] != VOS_STK_FLG)  /* 如果局部变量曾经赋值为0x55555555，忽略这种情况 */
        {
            return (uhwLen - i) * sizeof(VOS_UINT32);
        }
    }

    return 0;
}
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

