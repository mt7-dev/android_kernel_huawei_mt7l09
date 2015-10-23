
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafLog.h"
#include "Taf_Aps.h"
#include "TafApsCtx.h"
#include "TafApsSndInternalMsg.h"
#include "MnApsComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_SND_INTERNALMSG_C

/*****************************************************************************
  2 全局变量
*****************************************************************************/

/*****************************************************************************
  3 宏定义
*****************************************************************************/


/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 TAF_APS_SndInterPdpDeactivateReq(
    VOS_UINT8                           ucPdpId,
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU   *pstInternalMsg;

    /* 构造消息 */
    pstInternalMsg  = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)TAF_APS_GetIntMsgSendBuf(
                            sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU));
    if (VOS_NULL_PTR == pstInternalMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterDeactivateReq: Get internal message failed!");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET(pstInternalMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头 */
    pstInternalMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstInternalMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_TAF;
    pstInternalMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstInternalMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_TAF;
    pstInternalMsg->stMsgHeader.ulMsgName       = ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ;
    pstInternalMsg->stMsgHeader.ulLength        = sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填写消息内容 */
    pstInternalMsg->ucPdpId                     = ucPdpId;
    pstInternalMsg->enCause                     = enCause;

    /* 发送内部消息 */
    if (VOS_OK != TAF_APS_SndInternalMsg(pstInternalMsg))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterDeactivateReq: Send internal message failed.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_APS_SndLocalPdpDeactivateInd(
    VOS_UINT8                           ucPdpId,
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU   *pstLocalMsg;

    /* 构造消息 */
    pstLocalMsg  = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)TAF_APS_GetIntMsgSendBuf(
                            sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU));
    if (VOS_NULL_PTR == pstLocalMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndLocalPdpDeactivateInd: Get Local message failed!");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET(pstLocalMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头 */
    pstLocalMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstLocalMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_TAF;
    pstLocalMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstLocalMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_TAF;
    pstLocalMsg->stMsgHeader.ulMsgName       = ID_MSG_TAF_PS_APS_LOCAL_PDP_DEACTIVATE_IND;
    pstLocalMsg->stMsgHeader.ulLength        = sizeof(TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填写消息内容 */
    pstLocalMsg->ucPdpId                     = ucPdpId;
    pstLocalMsg->enCause                     = enCause;

    /* 发送内部消息 */
    if (VOS_OK != TAF_APS_SndInternalMsg(pstLocalMsg))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndLocalPdpDeactivateInd: Send internal message failed.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID TAF_APS_SndLocalAllSecPdpDeactivateInd(
    VOS_UINT8                           ucPdpId,
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           i;

    if (APS_PDP_ACT_PRI == TAF_APS_GetPdpEntActType(ucPdpId))
    {
        for (i = 0; i < TAF_APS_MAX_PDPID; i++)
        {
            if ((TAF_APS_GetPdpEntNsapi(ucPdpId) == TAF_APS_GetPdpEntLinkedNsapi(i))
             && (APS_PDP_ACT_SEC == TAF_APS_GetPdpEntActType(i))
             && (VOS_TRUE == TAF_APS_GetPdpEntUsedFlg(i)))
            {
                TAF_APS_SndLocalPdpDeactivateInd(i, enCause);
            }
        }
    }
}


VOS_VOID  TAF_APS_SndOmDfsInfo(TAF_APS_SWITCH_DDR_RATE_INFO_STRU *pstSwitchDdrInfo)
{
    TAF_APS_MNTN_DFS_INFO_STRU         *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_APS_MNTN_DFS_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF,
                             sizeof(TAF_APS_MNTN_DFS_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_SndOmDfsInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 定时器勾包消息赋值 */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_TAF;
    pstMsg->ulReceiverPid   = WUEPS_PID_TAF;
    pstMsg->ulLength        = sizeof(TAF_APS_MNTN_DFS_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->enMsgId         = ID_MSG_TAF_PS_APS_MNTN_DFS_INFO;

    PS_MEM_CPY(&pstMsg->stDfsInfo, pstSwitchDdrInfo, sizeof(TAF_APS_SWITCH_DDR_RATE_INFO_STRU));

    /* 消息勾包 */
    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_TAF, pstMsg);

}

#if (FEATURE_ON == FEATURE_LTE)

VOS_VOID TAF_APS_SndInterAttachBearerActivateInd(VOS_VOID)
{
    TAF_APS_ATTACH_BEARER_ACTIVATE_IND_STRU    *pstInternalMsg = VOS_NULL_PTR;

    /* 构造内部消息 */
    pstInternalMsg  = (TAF_APS_ATTACH_BEARER_ACTIVATE_IND_STRU *)TAF_APS_GetIntMsgSendBuf(
                            sizeof(TAF_APS_ATTACH_BEARER_ACTIVATE_IND_STRU));
    if (VOS_NULL_PTR == pstInternalMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterFsmEntryMsg: Get internal message failed!");
        return;
    }

    /* 填写消息头 */
    TAF_APS_CFG_INTER_MSG_HDR(pstInternalMsg, (sizeof(TAF_APS_ATTACH_BEARER_ACTIVATE_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    pstInternalMsg->enMsgId = ID_MSG_TAF_PS_ATTACH_BEARER_ACTIVATE_IND;

    /* 发送内部消息 */
    if (VOS_OK != TAF_APS_SndInternalMsg(pstInternalMsg))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterAttachBearerActivateInd: Send message failed.");
    }

    return;
}


VOS_VOID TAF_APS_SndInterFsmEntryMsg(TAF_APS_ENTRY_MSG_STRU *pstEntryMsg)
{
    VOS_VOID                           *pstInternalMsg = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader   = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLength;

    pstMsgHeader = (MSG_HEADER_STRU *)(&pstEntryMsg->aucEntryMsgBuffer[0]);
    ulMsgLength  = pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH;

    /* 构造内部消息 */
    pstInternalMsg  = (VOS_VOID *)TAF_APS_GetIntMsgSendBuf(ulMsgLength);
    if (VOS_NULL_PTR == pstInternalMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterFsmEntryMsg: Get internal message failed!");
        return;
    }

    /* 拷贝消息内容 */
    PS_MEM_CPY(pstInternalMsg, (VOS_VOID *)pstMsgHeader, ulMsgLength);

    /* 发送内部消息 */
    if (VOS_OK != TAF_APS_SndInternalMsg(pstInternalMsg))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_SndInterFsmEntryMsg: Send internal message failed.");
    }

    return;
}
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

