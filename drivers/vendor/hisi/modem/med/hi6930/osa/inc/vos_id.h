/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_Id.h                                                        */
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

#ifndef _VOS_ID_H
#define _VOS_ID_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "vos_config.h"
#include "v_queue.h"
#include "v_msg.h"
#include "v_id.h"
#include "v_io.h"
#include "v_task.h"

typedef struct
{
    VOS_PID             Pid;
    VOS_FID             Fid;
    Init_Fun_Type       InitFunc;
    Msg_Fun_Type        MsgFunction;
} VOS_PID_RECORD;

extern VOS_PID_RECORD   vos_PidRecords[ ];

#define VOS_ID_PID_NOT_BELONG_TO_FID    0x5a5a55aa
#define VOS_ID_PID_BELONG_TO_FID        0x55aa5a5a

typedef struct
{
    VOS_FID             Fid;
    Init_Fun_Type       InitFunc;
    VOS_UINT32          PidsBelong[VOS_PID_BUTT - VOS_PID_DOPRAEND];
    VOS_UINT32          Tid;
    VOS_UINT32          Priority;
    VOS_UINT32          Qid;
    VOS_UINT16          StackSize;
    VOS_UINT16          MaxMsgNumber;
} VOS_FID_CONTROL_BLOCK;

extern VOS_FID_CONTROL_BLOCK   vos_FidCtrlBlk[ ];

VOS_UINT32 VOS_RegisterPIDInfo( VOS_PID ulPID ,
                               Init_Fun_Type pfnInitFun ,
                               Msg_Fun_Type pfnMsgFun );

VOS_UINT32 VOS_RegisterMsgTaskPrio(VOS_FID ulFID,
                                   enum VOS_PRIORITY_DEFINE TaskPrio);

VOS_UINT32 VOS_PidCtrlBlkInit(VOS_VOID);

VOS_UINT32 VOS_PidsInit(VOS_VOID);

VOS_UINT32 VOS_FidCtrlBlkInit(VOS_VOID);

VOS_UINT32 VOS_FidsInit(VOS_VOID);

VOS_UINT32 CreateFidsQueque(VOS_VOID);

VOS_UINT32 CreateFidsTask(VOS_VOID);

VOS_VOID vos_FidTask( VOS_UINT32 ulQueueID, VOS_UINT32 FID_value,
                      VOS_UINT32 Para1, VOS_UINT32 Para2 );



VOS_UINT32 VOS_ResetFidsTask(VOS_FID ulFid);

#if VOS_STK_CHK_EN
VOS_UINT32 VOS_GetMaxStackUsed(VOS_FID uwFId);
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_ID_H */

