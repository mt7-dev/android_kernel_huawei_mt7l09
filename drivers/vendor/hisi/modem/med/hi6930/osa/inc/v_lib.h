/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_lib.h                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement general function                                   */
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

#ifndef _V_MINILIB_H
#define _V_MINILIB_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "dopra_def.h"
#include "vos_config.h"
#include "v_typdef.h"

/* error definition */
enum
{
/*  000  */    VOS_OK,
/*  001  */    VOS_ERR,
/*  002  */    VOS_ERRNO_EVENT_TYPE,
/*  003  */    VOS_ERRNO_PEND_ISR,
/*  004  */    VOS_ERRNO_PEVENT_NULL,
/*  005  */    VOS_ERRNO_TIMEOUT ,
/*  006  */    VOS_ERRNO_TASK_NOT_EXIST,
/*  007  */    VOS_ERRNO_PRIO_EXIST,
/*  008  */    VOS_ERRNO_PRIO_INVALID,
/*  009  */    VOS_ERRNO_SEM_OVF,
/*  010  */    VOS_ERRNO_NO_MORE_TCB,
/*  011  */    VOS_ERRNO_REG_FID_PID_INVALID_PID,
/*  012  */    VOS_ERRNO_REG_FID_PID_INVALID_FID,
/*  013  */    VOS_ERRNO_FIDPID_REGPIDI_INPUTPIDINVALID,
/*  014  */    VOS_ERRNO_FIDPID_REGPIDI_INPUTFUNCARENULL,
/*  015  */    VOS_ERRNO_FIDPID_REGPIDI_PIDINACTIVE,
/*  016  */    VOS_ERRNO_FIDPID_REGMPRI_INPUT1INVALID,
/*  017  */    VOS_ERRNO_FIDPID_REGMPRI_INPUT2INVALID,
/*  018  */    VOS_ERRNO_MEMORY_NON_DOPRAMEM,
/*  019  */    VOS_ERRNO_MEMORY_DOPRAMEM_OVERFLOW,
/*  020  */    VOS_ERRNO_MEMORY_HEAD_COVER,
/*  021  */    VOS_ERRNO_MEMORY_TAIL_COVER,
/*  022  */    VOS_ERRNO_MEMORY_GLOBAL_COVER,
/*  023  */    VOS_ERRNO_MSG_MEMORY_FULL,
/*  024  */    VOS_ERRNO_MEMORY_FULL ,
/*  025  */    VOS_ERRNO_MEMORY_FREE_INPUTMSGISNULL,
/*  026  */    VOS_ERRNO_MEMORY_FREE_REPECTION,
/*  027  */    VOS_ERRNO_MEMORY_ALLOC_INPUTMSGISNULL,
/*  028  */    VOS_ERRNO_MEMORY_FREE_INPUTPIDINVALID,
/*  029  */    VOS_ERRNO_MEMORY_LOCATION_INPUTINVALID,
/*  030  */    VOS_ERRNO_MEMORY_LOCATION_TIMERSPACE,
/*  031  */    VOS_ERRNO_MEMORY_LOCATION_CANNOTDO,
/*  032  */    VOS_ERRNO_MSG_GETPRIO_INPUT1ISNULL,
/*  033  */    VOS_ERRNO_MSG_FREE_INPUTMSGISNULL,
/*  034  */    VOS_ERRNO_MSG_FREE_INPUTPIDINVALID,
/*  035  */    VOS_ERRNO_MSG_FREE_NON_DOPRA_ADDR,
/*  036  */    VOS_ERRNO_MSG_FREE_NON_START_ADDR,
/*  037  */    VOS_ERRNO_MSG_FREE_NON_START_UNIT,
/*  038  */    VOS_ERRNO_MSG_FREE_STATUSERROR,
/*  039  */    VOS_ERRNO_MSG_FREERES_STATUSERROR,
/*  040  */    VOS_ERRNO_MSG_RESERVE_INVALIDMSG,
/*  041  */    VOS_ERRNO_MSG_POST_INPUTMSGISNULL,
/*  042  */    VOS_ERRNO_MSG_POST_INPUTPIDINVALID,
/*  043  */    VOS_ERRNO_MSG_POST_RECVCPUNOTLOCAL,
/*  044  */    VOS_ERRNO_MSG_POST_RECVPIDINVALID,
/*  045  */    VOS_ERRNO_MSG_POST_MSGFUNCISNULL,
/*  046  */    VOS_ERRNO_QUEUE_CREATE_LENISZERO,
/*  047  */    VOS_ERRNO_QUEUE_CREATE_SIZEISZERO,
/*  048  */    VOS_ERRNO_QUEUE_CREATE_OPTINVALID,
/*  049  */    VOS_ERRNO_QUEUE_CREATE_NAMEISNULL,
/*  050  */    VOS_ERRNO_QUEUE_CREATE_OUTPUTISNULL,
/*  051  */    VOS_ERRNO_QUEUE_CREATE_NOFREECB,
/*  052  */    VOS_ERRNO_QUEUE_CREATE_OSALFAIL,
/*  053  */    VOS_ERRNO_QUEUE_WRITE_QUEIDINVALID,
/*  054  */    VOS_ERRNO_QUEUE_WRITE_BUFADDRISNULL,
/*  055  */    VOS_ERRNO_QUEUE_WRITE_BUFSIZEINVALID,
/*  056  */    VOS_ERRNO_QUEUE_FULL,
/*  057  */    VOS_ERRNO_SEMA4_CCREATE_NAME_NULL,
/*  058  */    VOS_ERRNO_SEMA4_CCREATE_INVALID_SMID,
/*  059  */    VOS_ERRNO_SEMA4_CCREATE_FLAG_ERR,
/*  060  */    VOS_ERRNO_SEMA4_CCREATE_OBJTFULL,
/*  061  */    VOS_ERRNO_SEMA4_CCREATE_OSALCREATEERR,
/*  062  */    VOS_TRAID_SEMA4_CCREATE,
/*  063  */    VOS_EVENTID_SEMA4_CCREATED,
/*  064  */    VOS_ERRNO_SEMA4_P_IDERR,
/*  065  */    VOS_ERRNO_SEMA4_P_NOTACTIVE,
/*  066  */    VOS_ERRNO_SEMA4_P_TYPEERR,
/*  067  */    VOS_ERRNO_SEMA4_P_CANOTP,
/*  068  */    VOS_ERRNO_SEMA4_P_TIMEOUT,
/*  069  */    VOS_ERRNO_SEMA4_V_IDERR,
/*  070  */    VOS_ERRNO_SEMA4_V_NOTACTIVE,
/*  071  */    VOS_ERRNO_SEMA4_V_M_CANOTV,
/*  072  */    VOS_ERRNO_SEMA4_V_CANOTV,
/*  073  */    VOS_TRAID_SEMA4_V,
/*  074  */    VOS_ERRNO_SEMA4_BCREATE_INVALID_INIT,
/*  075  */    VOS_ERRNO_SEMA4_R_IDERR,
/*  076  */    VOS_ERRNO_SEMA4_R_NOTACTIVE,
/*  077  */    VOS_ERRNO_SEMA4_FULL,
/*  078  */    VOS_ERRNO_TASK_CREATE_INPUTENTRYISNULL,
/*  079  */    VOS_ERRNO_TASK_CREATE_INPUTARGSISNULL,
/*  080  */    VOS_ERRNO_TASK_CREATE_INPUTTIDISNULL,
/*  081  */    VOS_ERRNO_TASK_CREATE_INPUTNAMEISNULL,
/*  082  */    VOS_ERRNO_TASK_CREATE_INPUTPRIOINVALID,
/*  083  */    VOS_ERRNO_TASK_CREATE_NAMESAMETOPREV,
/*  084  */    VOS_ERRNO_TASK_CREATE_NOFREETCB,
/*  085  */    VOS_ERRNO_TASK_CREATE_CREATEEVENTFAIL,
/*  086  */    VOS_ERRNO_TASK_CREATE_OSALCREATEFAIL
};


VOS_UINT32 VOS_GetTick( VOS_VOID );

VOS_UINT32 V_SetErrorNo( VOS_UINT32 ulErrorNo,
                         VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_SetErrorNo( ulErrorNo )\
    V_SetErrorNo( (ulErrorNo), VOS_FILE_ID, __LINE__ )

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_MINILIB_H */



