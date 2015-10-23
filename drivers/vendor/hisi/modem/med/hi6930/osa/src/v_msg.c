/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_msg.c                                                         */
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

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_msg.h"
#include "v_blkmem.h"
#include "v_queue.h"
#include "vos_id.h"
#include "v_lib.h"
#include "cpu_c.h"
#include "med_drv_mb_hifi.h"
#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#endif

#define    THIS_FILE_ID        FILE_ID_V_MSG_C


/*****************************************************************************
 Function   : V_AllocMsg
 Description: allocates messages block
 Input      : Pid      -- PID who should send the message
              ulLength -- the length of message
 Return     : The pointer of message on success or VOS_NULL_PTR on failure
 Other      : The pointer of message that returned for application is not the
              actual pointer of message which system used
 *****************************************************************************/
MsgBlock * V_AllocMsg( VOS_PID Pid, VOS_UINT32 ulLength,
                       VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_INT         ulTotalLength;
    MSG_BLOCK_HEAD  *pMsgBlkHead;
    MsgBlock        *MsgBlock_Ptr;

    if( 0 == ulLength )
    {
        return((MsgBlock*)VOS_NULL_PTR);
    }

    ulTotalLength =
        (VOS_INT)(VOS_MSG_BLOCK_HEAD_AND_RESERVED_LENGTH + ulLength);

    pMsgBlkHead = (MSG_BLOCK_HEAD*)VOS_MemBlkMalloc( Pid, ulTotalLength,
        ulFileID, usLineNo);
    if( VOS_NULL_PTR == pMsgBlkHead )
    {
        return((MsgBlock*)VOS_NULL_PTR);
    }

    pMsgBlkHead->ulFlag = VOS_MSG_HEAD_FLAG;

    MsgBlock_Ptr = (MsgBlock*)( (VOS_UINT32)pMsgBlkHead + VOS_MSG_BLK_HEAD_LEN );
    MsgBlock_Ptr->uwSenderCpuId = VOS_LOCAL_CPUID;
    MsgBlock_Ptr->uwSenderPid = Pid;
    MsgBlock_Ptr->uwReceiverCpuId = VOS_LOCAL_CPUID;
    MsgBlock_Ptr->uwLength = (VOS_UINT32)ulLength;

    return MsgBlock_Ptr;
}

/*****************************************************************************
 Function   : V_FreeMsg
 Description: Free a message which status must be ALLOCATED
 Input      : Pid  -- PID who free the message
              pMsg -- Pointer of the message to be freed
 Return     : VOS_OK on success or error number on failure
 Other      : This function was only called to free a message which was
              allocated by VOS_AllocMsg but not been send.
 *****************************************************************************/
VOS_UINT32 V_FreeMsg( VOS_PID Pid, VOS_VOID **ppMsg,
                            VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_VOID            *pMsgBlkHead;
    MSG_BLOCK_HEAD      *pstMSG;
    VOS_UINT32          ulBlockAdd;
    VOS_UINT32          ulCtrlkAdd;

    if( VOS_NULL_PTR == ppMsg )
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
        return(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
    }

    if( VOS_NULL_PTR == *ppMsg )
    {
        LogPrint3("# V_FreeMsg,free msg again.F %ld L %ld T %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo, (VOS_INT)VOS_GetTick() );

        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
        return(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
    }

    pMsgBlkHead = (VOS_VOID*)( (VOS_UINT32)(*ppMsg) - VOS_MSG_BLK_HEAD_LEN );

    if ( VOS_OK != VOS_MemCheck( pMsgBlkHead, &ulBlockAdd, &ulCtrlkAdd,
        ulFileID, usLineNo ) )
    {
        return VOS_ERR;
    }

    pstMSG = (MSG_BLOCK_HEAD *)pMsgBlkHead;
    if ( VOS_MSG_RESERVED_HEAD_FLAG == pstMSG->ulFlag )
    {
        return VOS_OK;
    }

    /* Clear user's pointer */
    *ppMsg = VOS_NULL_PTR;

    return VOS_MemCtrlBlkFree( (VOS_MEM_CTRL_BLOCK *)ulCtrlkAdd,
        (VOS_MEM_HEAD_BLOCK *)ulBlockAdd, ulFileID, usLineNo );
}

#if 0
/*****************************************************************************
 Function   : V_ReserveMsg
 Description: In msg proceed function, reserve a message
 Input      : pMsg     -- the pointer of application message (MsgBlock)
              Pid
 Return     : VOS_OK -- success
              ErrorCode -- fail
 *****************************************************************************/
VOS_UINT32 V_ReserveMsg( VOS_PID Pid, MsgBlock * pMsg,
                            VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    MSG_BLOCK_HEAD      *MSG_BLOCK_Ptr;
    VOS_UINT32          ulBlockAdd;
    VOS_UINT32          ulCtrlkAdd;

    if(!VOS_PidCheck(Pid))
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_RESERVE_INVALIDMSG);
        return(VOS_ERRNO_MSG_RESERVE_INVALIDMSG);
    }

    if(pMsg == VOS_NULL_PTR)
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
        return(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
    }

    MSG_BLOCK_Ptr = (MSG_BLOCK_HEAD*)( (VOS_UINT32)pMsg
                         - VOS_MSG_BLK_HEAD_LEN );

    if ( VOS_OK != VOS_MemCheck( (VOS_VOID *)MSG_BLOCK_Ptr, &ulBlockAdd,
        &ulCtrlkAdd, ulFileID, usLineNo ) )
    {
        return VOS_ERR;
    }

    if ( VOS_MSG_HEAD_FLAG == MSG_BLOCK_Ptr->ulFlag )
    {
        MSG_BLOCK_Ptr->ulFlag = VOS_MSG_RESERVED_HEAD_FLAG;
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}

/*****************************************************************************
 Function   : V_FreeReservedMsg
 Description: Free a reserved message.
 Input      : pMsg -- Point of the message to free
              Pid
 Return     : VOS_OK on success and error code on failure
 *****************************************************************************/
VOS_UINT32 V_FreeReservedMsg( VOS_PID Pid, VOS_VOID ** ppMsg,
                                    VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_VOID            *pMsgBlkHead;
    MSG_BLOCK_HEAD      *pstMSG;
    VOS_UINT32          ulBlockAdd;
    VOS_UINT32          ulCtrlkAdd;

    if(!VOS_PidCheck(Pid))
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
        return(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
    }

    if( VOS_NULL_PTR == ppMsg )
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
        return(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
    }

    if( VOS_NULL_PTR == *ppMsg )
    {
        LogPrint3("# V_FreeReservedMsg,free reserved msg again.F %ld L %ld T %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo, (VOS_INT)VOS_GetTick() );

        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
        return(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
    }

    pMsgBlkHead = (VOS_VOID*)( (VOS_UINT32)(*ppMsg)
                         - VOS_MSG_BLK_HEAD_LEN );

    if ( VOS_OK != VOS_MemCheck( pMsgBlkHead, &ulBlockAdd, &ulCtrlkAdd,
        ulFileID, usLineNo ) )
    {
        return VOS_ERR;
    }

    pstMSG = (MSG_BLOCK_HEAD *)pMsgBlkHead;
    if ( VOS_MSG_RESERVED_HEAD_FLAG == pstMSG->ulFlag )
    {
        /* Clear user's pointer */
        *ppMsg = VOS_NULL_PTR;

        return VOS_MemCtrlBlkFree( (VOS_MEM_CTRL_BLOCK *)ulCtrlkAdd,
            (VOS_MEM_HEAD_BLOCK *)ulBlockAdd, ulFileID, usLineNo );
    }
    else
    {
        return VOS_ERRNO_MSG_FREERES_STATUSERROR;
    }
}

#endif

/*****************************************************************************
 Function   : VOS_DeliverMsg
 Description: deliver the msg to a task.
 Input      : Pid  -- PID who send the message
              pMsg -- the pointer of message
              Pri -- the priority of msg
 Return     : VOS_OK on success and error code on failure
 Other      : After sending message, the status would be changed to ready.
 *****************************************************************************/
VOS_UINT32 VOS_DeliverMsg( VOS_PID Pid, VOS_VOID **ppMsg, VOS_UINT32 uwChannelID,
                            VOS_UINT32 Pri, VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    MsgBlock           *pMsgCtrlBlk;
    VOS_UINT32          ulPid;
    VOS_FID             ulFid;
    VOS_UINT32          ulQid;
    VOS_VOID           *pActualMsg;
    VOS_UINT32          ulBlockAdd;
    VOS_UINT32          ulCtrlkAdd;
    VOS_UINT32          uwRet;

    if( VOS_NULL_PTR == ppMsg )
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
        return(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
    }

    if( VOS_NULL_PTR == *ppMsg )
    {
        LogPrint3("# VOS_DeliverMsg,send msg again.F %ld L %ld T %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo, (VOS_INT)VOS_GetTick() );

        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
        return(VOS_ERRNO_MSG_FREE_INPUTMSGISNULL);
    }

    pActualMsg = (VOS_VOID *)((VOS_UINT32)(*ppMsg) - VOS_MSG_BLK_HEAD_LEN);

    if ( VOS_OK != VOS_MemCheck( pActualMsg, &ulBlockAdd, &ulCtrlkAdd,
        ulFileID, usLineNo ) )
    {
        LogPrint("# VOS_DeliverMsg Mem Check error.\n");

        return VOS_ERR;
    }

    if (VOS_EMERGENT_PRIORITY_MSG == Pri)
    {
        ((MSG_BLOCK_HEAD *)pActualMsg)->ulFlag = VOS_EMERGENT_HEAD_FLAG;
        uwChannelID = DRV_MAILBOX_CHANNEL_VOS_CH_URGENT;
    }

    pMsgCtrlBlk      = (MsgBlock*)(*ppMsg);
    ulPid            = pMsgCtrlBlk->uwReceiverPid;
    pMsgCtrlBlk->uwReceiverCpuId = VOS_GetCpuId(ulPid);

    /* 跨核消息处理 */
    if (VOS_LOCAL_CPUID != pMsgCtrlBlk->uwReceiverCpuId)
    {
        // 发送的消息内容带VOS头
        uwRet = DRV_MAILBOX_SendMsg(pMsgCtrlBlk->uwReceiverCpuId,
                                    uwChannelID,
                                    Pri,
                                    (VOS_UCHAR*)pMsgCtrlBlk,
                                    VOS_MSG_HEAD_LENGTH + pMsgCtrlBlk->uwLength);

        VOS_FreeMsg( Pid, *ppMsg );

        return uwRet;
    }

    if(!VOS_PidCheck(ulPid))
    {
        VOS_SetErrorNo(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);

        LogPrint1("# VOS_DeliverMsg Error,invalid PID %ld.\n", (VOS_INT)ulPid);

        VOS_FreeMsg( Pid, *ppMsg );

        return(VOS_ERRNO_MSG_FREE_INPUTPIDINVALID);
    }

    ulFid            = vos_PidRecords[VOS_PidIdx(ulPid)].Fid;
    ulQid            = vos_FidCtrlBlk[ulFid].Qid;

    if ( VOS_OK != VOS_FixedQueueWrite( ulQid, pActualMsg, Pri) )
    {
        LogPrint3("# VOS_DeliverMsg queue full,Tx Pid %ld Rx Pid %ld Name %ld.\n",
            (VOS_INT)(pMsgCtrlBlk->uwSenderPid), (VOS_INT)ulPid,
            (VOS_INT)(*(VOS_UINT32 *)(pMsgCtrlBlk->aucValue)) );

        VOS_FreeMsg( Pid, *ppMsg );

        return VOS_ERR;
    }

    *ppMsg = VOS_NULL_PTR;

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


