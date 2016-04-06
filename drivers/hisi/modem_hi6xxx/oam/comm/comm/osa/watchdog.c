/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: WatchDog.c                                                      */
/*                                                                           */
/* Author: Cheng Xu                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2007-05                                                             */
/*                                                                           */
/* Description: implement watch dog                                          */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*lint --e{537,527}*/
#include "v_typdef.h"
#include "v_timer.h"
#include "v_int.h"
#include "v_lib.h"
#include "v_task.h"
#include "WatchDog.h"
#include "v_IO.h"
#include "DrvInterface.h"
#include "NvIdList.h"
#include "omnvinterface.h"

#if (VOS_VXWORKS== VOS_OS_VER)
#include "stdio.h"
#include "stdlib.h"
#endif

#include "NVIM_Interface.h"

#if (RAT_MODE == RAT_GU)
#include "FileSysInterface.h"
#include "errorlog.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_V_WATCH_DOG_C

/* timer task's stack size */
#define WD_TASK_STACK_SIZE                                  4096

/* 1S -> 1000 ms*/
#define WD_TIME_UNIT                                        1000

/* the task ID of timer's task */
VOS_UINT32                       g_ulWDTaskId;

/*****************************************************************************
 Function   : WD_TaskFunc
 Description: timer task entry
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID WD_TaskFunc( VOS_UINT32 Para0, VOS_UINT32 Para1,
                      VOS_UINT32 Para2, VOS_UINT32 Para3 )
{
    OM_LIVE_TIME_CONTROL_STRU           stLiveTime = {0};
    VOS_UINT32                          ulCountCycle = 0;
    VOS_UINT32                          ulCycle = 0;
    OM_LIVE_TIME_STRU                   stTotalTime;

    stTotalTime.ulLiveTime = 0;

    if ( VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_LiveTimeControl,
        (VOS_VOID *)(&stLiveTime), sizeof(OM_LIVE_TIME_CONTROL_STRU)))
    {
        stLiveTime.ulEnable= 0;
    }

    /* record */
    if ( VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_LiveTime,
                    (VOS_VOID *)(&stTotalTime), sizeof(OM_LIVE_TIME_STRU)))
    {
        stLiveTime.ulEnable= 0;
    }

    if ( stLiveTime.ulEnable )
    {
        /* minute -> second */
        ulCycle = (stLiveTime.ulCycle) * 60;
    }

    /* coverity[no_escape] */
    for( ; ; )
    {
        /* SemTake SEM when releas*/
        VOS_TaskDelay(WD_TIME_UNIT);

        if ( stLiveTime.ulEnable )
        {
            ulCountCycle++;

            if ( ulCountCycle >= ulCycle )
            {
                ulCountCycle = 0;

                /* record */
                stTotalTime.ulLiveTime += stLiveTime.ulCycle;

                (VOS_VOID)NV_WriteEx(MODEM_ID_0, en_NV_Item_LiveTime,
                    (VOS_VOID *)(&stTotalTime), sizeof(stTotalTime));
            }
        }
    }

    return ;
}

/*****************************************************************************
 Function   : WD_TaskCreat
 Description: create timer task
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 WD_TaskCreat(VOS_VOID)
{
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
	return VOS_OK;
#else
    VOS_UINT32 TimerArguments[4] = {0,0,0,0};

    return( VOS_CreateTask( "WatchDog",
                            &g_ulWDTaskId,
                            WD_TaskFunc,
                            COMM_WD_SELFTASK_PRIO,
                            WD_TASK_STACK_SIZE,
                            TimerArguments) );
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


