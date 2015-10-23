/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_msg.h                                                         */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement message function                                   */
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

#ifndef _VOS_MSG_H
#define _VOS_MSG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "vos_config.h"
#include "v_blkmem.h"

/* The message block (packege) head for each message package    */
typedef struct tagMsgBlockHead_S
{
    VOS_UINT32    ulFlag;  /* The message head flag, = VOS_MSG_HEAD_FLAG  */
} MSG_BLOCK_HEAD;

#define VOS_MSG_HEADER  VOS_UINT32 uwSenderCpuId;  \
                        VOS_UINT32 uwSenderPid;    \
                        VOS_UINT32 uwReceiverCpuId;\
                        VOS_UINT32 uwReceiverPid;  \
                        VOS_UINT32 uwLength;

#define ZOS_MSG_HEADER  VOS_MSG_HEADER

typedef struct VOS_MSG_HEADER_tag
{
    VOS_MSG_HEADER
}VOS_MSG_HEADER_STRU;

/* VOS common message header length, sizeof(ZOS_MSG_HEADER) */
#if (VOS_ZSP == VOS_CPU_TYPE)
#define VOS_MSG_HEAD_LENGTH              sizeof(VOS_MSG_HEADER_STRU) //10
#else
#define VOS_MSG_HEAD_LENGTH              sizeof(VOS_MSG_HEADER_STRU) //20
#endif

/* length of message block head */
#define VOS_MSG_BLK_HEAD_LEN    ( sizeof(MSG_BLOCK_HEAD) )

/* Flag of message packet valid */
#define VOS_MSG_HEAD_FLAG                0xA1D55555
#define VOS_EMERGENT_HEAD_FLAG           0xA1D5AAAA
#define VOS_MSG_RESERVED_HEAD_FLAG       0xA1D538FF

#define VOS_MSG_BLOCK_HEAD_AND_RESERVED_LENGTH  \
                ((VOS_MSG_BLK_HEAD_LEN) + (VOS_MSG_HEAD_LENGTH))

#define VOS_IsMsgEmergent(pMsg)                 \
                (VOS_EMERGENT_HEAD_FLAG == ((MSG_BLOCK_HEAD*)(pMsg))->ulFlag)

#pragma pack(1)
struct MsgCB
{
    ZOS_MSG_HEADER
    VOS_UINT8 aucValue[4];
};
#pragma pack()

typedef struct MsgCB MSG_CB;
typedef struct MsgCB MsgBlock;

enum MSG_PRIORITY
{
    VOS_EMERGENT_PRIORITY_MSG,/* put msg om the head of queue */
    VOS_NORMAL_PRIORITY_MSG/* put msg om the tail of queue */
};

typedef VOS_VOID   (*Msg_Fun_Type)( MsgBlock * pMsg );

MsgBlock * V_AllocMsg( VOS_PID Pid, VOS_UINT32 ulLength,
                       VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_AllocMsg( Pid, ulLength ) \
    V_AllocMsg( (Pid), (ulLength), VOS_FILE_ID, __LINE__)

VOS_UINT32 V_FreeMsg( VOS_PID Pid, VOS_VOID **ppMsg,
                         VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_FreeMsg( Pid, pMsg )\
    V_FreeMsg( (Pid), (VOS_VOID**)(&(pMsg)), VOS_FILE_ID, __LINE__ )

VOS_UINT32 VOS_DeliverMsg( VOS_PID Pid, VOS_VOID **ppMsg, VOS_UINT32 uwCh,   \
                    VOS_UINT32 Pri, VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_SendMsg( Pid, pMsg, uwCh )\
    VOS_DeliverMsg( (Pid), (VOS_VOID**)(&(pMsg)), (uwCh),   \
                    VOS_NORMAL_PRIORITY_MSG, VOS_FILE_ID, __LINE__ )

#define VOS_SendUrgentMsg( Pid, pMsg, uwCh )\
    VOS_DeliverMsg( (Pid), (VOS_VOID**)(&(pMsg)), (uwCh),   \
                    VOS_EMERGENT_PRIORITY_MSG, VOS_FILE_ID, __LINE__ )

#if 0
VOS_UINT32 V_ReserveMsg( VOS_PID Pid, MsgBlock * pMsg,
                            VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_ReserveMsg( Pid, pMsg )\
    V_ReserveMsg( (Pid), (pMsg), VOS_FILE_ID, __LINE__ )

VOS_UINT32 V_FreeReservedMsg( VOS_PID Pid, VOS_VOID **ppMsg,
                                   VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

#define VOS_FreeReservedMsg( Pid, pMsg )\
    V_FreeReservedMsg( (Pid), (VOS_VOID**)(&(pMsg)),  VOS_FILE_ID, __LINE__  )
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_MSG_H */
