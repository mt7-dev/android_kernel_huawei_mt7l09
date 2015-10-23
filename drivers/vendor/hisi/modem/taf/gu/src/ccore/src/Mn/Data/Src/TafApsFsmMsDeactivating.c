

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafTypeDef.h"
#include "TafApsApi.h"
#include "Taf_Aps.h"
#include "TafLog.h"
#include "Taf_Aps.h"
#include "TafApsCtx.h"
#include "TafApsFsmMainTbl.h"
#include "TafApsFsmMsDeactivating.h"
#include "TafApsFsmMsDeactivatingTbl.h"
#include "TafApsProcIpFilter.h"
#include "TafApsSndSm.h"
#include "TafApsSndAt.h"
#include "TafApsSndInternalMsg.h"
#include "TafFsm.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "SmEsmInterface.h"
#include "MnApsMultiMode.h"
#include "ApsL4aInterface.h"
#include "TafApsSndL4a.h"
#include "Taf_ApsForRabm.h"
#include "MmcApsInterface.h"
#include "TafSmInterface.h"
#include "TafApsComFunc.h"
#endif
#include "ApsNdInterface.h"
#include "TafApsSndNd.h"
#include "MnApsComm.h"
#include "TafApsComFunc.h"


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_APS_FSM_MSDEACTIVATING_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
*****************************************************************************/


/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq;
    VOS_UINT8                           ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    /* 获取消息内容 */
    pstAppMsg            = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取当前接入技术类型 */
    enCurrRatType = TAF_APS_GetCurrPdpEntityRatType();

    /* 上报PDP去激活请求响应事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpCtxStateReq->stCtrl), TAF_PS_CAUSE_SUCCESS);

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(ucPdpId))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;
    VOS_UINT8                           ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    /* 获取消息内容 */
    pstAppMsg     = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取当前接入技术类型 */
    enCurrRatType = TAF_APS_GetCurrPdpEntityRatType();

    /* 上报PS呼叫挂断响应事件 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl), pstCallEndReq->ucCid, TAF_PS_CAUSE_SUCCESS);

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(ucPdpId))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                               ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32            enCurrRatType;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;

    /* 初始化, 获取消息内容 */
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)pstMsg;
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType  = TAF_APS_GetCurrPdpEntityRatType();

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(ucPdpId))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    /*获取网络模式*/
    enCurrRatType                       = TAF_APS_GetCurrPdpEntityRatType();

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_GuMode();
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_LteMode();
            break;
#endif

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU        *pstSerStaInd;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg = TAF_APS_GetCurrSubFsmMsgAddr();

    /* 初始化, 获取消息内容 */
    pstSerStaInd        = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 刷新网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 正常流程下，不应该有这个场景，此处仍然尝试去激活操作 */
    if ( (MMC_APS_RAT_TYPE_GSM == pstSerStaInd->enRatType)
      || (MMC_APS_RAT_TYPE_WCDMA == pstSerStaInd->enRatType))
    {
        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)pstEntryMsg->aucEntryMsgBuffer;
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                break;
        }
    }
#if (FEATURE_ON == FEATURE_LTE)
    else
    {
        if (MMC_APS_RAT_TYPE_LTE == pstSerStaInd->enRatType)
        {
            switch(pstEntryMsg->ulEventType)
            {
                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                    if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(TAF_APS_GetCurrFsmEntityPdpId()))
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                    }
                    else
                    {
                        TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
                    }
                    break;

                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                    if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(TAF_APS_GetCurrFsmEntityPdpId()))
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                    }
                    else
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
                    }
                    break;

                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                    TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
                    break;

                default:
                    TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                    break;
            }
        }
    }
#endif
    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq   = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;
    VOS_UINT8                           ucPdpId;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity  = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 处理EPS承载去激活指示 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_Suspend: Wrong option.");
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(TAF_APS_GetPdpEntNsapi(ucPdpId));

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
#endif
VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity   = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 同步PDP状态&信息给ESM */
#if (FEATURE_ON == FEATURE_LTE)
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateCnf_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 释放SNDCP资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 如果去激活的是主PDP,挂接在下面的从PDP都需要本地去激活 */
        TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();

    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstSerStaInd;

    /* 初始化, 获取消息内容 */
    pstSerStaInd        = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 切换网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    if (MMC_APS_RAT_TYPE_NULL == pstSerStaInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSmDeactivateCnf: Wrong RatType");
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();
    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateCnf_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity                        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (APS_USED == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;
    VOS_UINT8                           ucPdpId;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity  = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 处理EPS承载去激活指示 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitSmDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(TAF_APS_GetPdpEntNsapi(ucPdpId));

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstSerStaInd;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucPdpId;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;

    ucPdpId               = TAF_APS_GetCurrFsmEntityPdpId();
#endif

    /* 初始化, 获取消息内容 */
    pstSerStaInd        = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 切换网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    if ( (MMC_APS_RAT_TYPE_GSM == pstSerStaInd->enRatType)
      || (MMC_APS_RAT_TYPE_WCDMA == pstSerStaInd->enRatType))
    {

        /*GU模下 ,不需要处理,继续等SM的回复,状态回到
          TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);
    }
#if (FEATURE_ON == FEATURE_LTE)
    else if (MMC_APS_RAT_TYPE_LTE == pstSerStaInd->enRatType)
    {
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        /* 初始化, 获取消息内容 */
        pstEntryMsg                         = TAF_APS_GetCurrSubFsmMsgAddr();

        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(ucPdpId))
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                }
                else
                {
                    TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
                }
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                if (VOS_TRUE == TAF_APS_GetPdpEntAttachBearerFlag(ucPdpId))
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                }
                else
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
                }

                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                break;

        }
    }
    else
    {
        ;
    }
#endif

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息 */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}



VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpDeactivateRsp_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstSerStaInd;

    /* 初始化, 获取消息内容 */
    pstSerStaInd        = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 刷新网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 挂起指示，此时应该是迁移子状态到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
    if (MMC_APS_RAT_TYPE_NULL == pstSerStaInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSmDeactivateCnf: Wrong RatType");
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);


    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl), pstCallOrigReq->stDialParaInfo.ucCid, TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpDeactivateRsp_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    MMC_APS_SERVICE_STATUS_IND_STRU        *pstSerStaInd;
    VOS_UINT8                               ucPdpId;


    ucPdpId         = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity    = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 初始化, 获取消息内容 */
    pstSerStaInd    = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 切换网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    if ( MMC_APS_RAT_TYPE_GSM == pstSerStaInd->enRatType )
    {
        /*G模下 ,不需要处理,继续等SM的回复,状态回到
          TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }

    /* WCDMA和LTE情况下，认为去激活已经成功，进行上报 */
    if ( ( MMC_APS_RAT_TYPE_WCDMA == pstSerStaInd->enRatType )
#if (FEATURE_ON == FEATURE_LTE)
       ||( MMC_APS_RAT_TYPE_LTE == pstSerStaInd->enRatType )
#endif
       )
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 认为本地去激活，释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity   = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl), pstCallOrigReq->stDialParaInfo.ucCid, TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateCnf_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateRej_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    APS_L4A_PDP_DEACTIVATE_REJ_STRU        *pstPdpRej;
    VOS_UINT8                               ucPdpId;

    /* 当前LTE下有可能会出现去激活失败，目前来说，如果收到L下的去激活失败，直接
       上报去激活成功，对应的状态清除 */

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_IPV6)
    /* 清除SM CAUSE */
    pstPdpEntity->enCause = VOS_FALSE;
#endif

    pstPdpRej = (APS_L4A_PDP_DEACTIVATE_REJ_STRU*)pstMsg;
    if (APS_L4A_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED == pstPdpRej->ulEsmCause)
    {
        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 如果是最后PDP去激活，L模会拒绝，根据此特定原因值进行特殊处理，此时
           该实体不会实际去激活，但是会刷新成默认承载，实体状态保持为激活状态 */


        /* 刷新成默认承载的CID */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    TAF_APS_DEFAULT_CID,
                                    TAF_APS_INVALID_MODULEID,
                                    TAF_APS_INVALID_CLIENTID,
                                    TAF_APS_INVALID_OPID);

        TAF_APS_SetPdpEntAttachBearerFlag(ucPdpId, VOS_TRUE);

        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

        /* 同步PDP信息至SM */
        if (VOS_TRUE == pstPdpEntity->bitOpTransId)
        {
            MN_APS_SndSmBearerActivateInd(pstPdpEntity);
        }

        /* 需要将对应去激活PDP下的专有承载的信息同步给SM，
           因为这个时候SM已经将这些清除掉 */
        MN_APS_SndSmAllSecPdpActivateInd(pstPdpEntity);

        /* 主状态迁移至TAF_APS_STA_ACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();

    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        /* 向SM发送本地去激活请求 */
        TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstSerStaInd;

    /* 初始化, 获取消息内容 */
    pstSerStaInd    = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 刷新网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);


    if (MMC_APS_RAT_TYPE_NULL == pstSerStaInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        /* 此时收到网络指示，认为是一条异常消息 */
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnf: err msg");
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);


    /* 根据EPS承载的操作类型(Activate, Deactivate, Modify),
       调用相应的处理函数 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitL4aDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPsCallEndCnf_MsDeactivating_WaitL4aDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PS_CALL_END_CNF_STRU       *pstL4aPsCallEndCnf;

    /* 初始化, 获取消息内容 */
    pstL4aPsCallEndCnf      = (APS_L4A_PS_CALL_END_CNF_STRU*)pstMsg;

    /* 目前来说，断开拨号失败的场景是无法恢复的，故收到后不做处理 */
    if (APS_L4A_SUCCESS != pstL4aPsCallEndCnf->ulErrorCode)
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF,
                         "TAF_APS_RcvL4aPsCallEndCnf_MsDeactivating_WaitL4aDeactivateCnf,ulErrorCode",
                         pstL4aPsCallEndCnf->ulErrorCode);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl), pstCallOrigReq->stDialParaInfo.ucCid, TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateCnf_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateRej_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    APS_L4A_PDP_DEACTIVATE_REJ_STRU        *pstPdpRej;
    VOS_UINT8                               ucPdpId;

    /* 当前LTE下有可能会出现去激活失败，目前来说，如果收到L下的去激活失败，直接
       上报去激活成功，对应的状态清除 */

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_IPV6)
    /* 清除SM CAUSE */
    pstPdpEntity->enCause = VOS_FALSE;
#endif

    pstPdpRej = (APS_L4A_PDP_DEACTIVATE_REJ_STRU*)pstMsg;

    /* 当前L4A与APS之间需要定义好错误码的值目前还未定义，待陈晓军这两天确认 */
    /* 如果是因为挂起导致的去激活失败，解挂后需要重新发起去激活操作，目前临时
       用一个值来代替 */
    if (APS_L4A_ERR_SM_SUSPENDED == pstPdpRej->ulEsmCause)
    {
        /*
          迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND，解挂后再次发起去激活操作
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

        return VOS_TRUE;
    }
    else if (APS_L4A_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED == pstPdpRej->ulEsmCause)
    {
        /* 如果是最后PDP去激活，L模会拒绝，根据此特定原因值进行特殊处理，此时
           该实体不会实际去激活，但是会刷新成默认承载，实体状态保持为激活状态 */

        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 刷新成默认承载的CID */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    TAF_APS_DEFAULT_CID,
                                    TAF_APS_INVALID_MODULEID,
                                    TAF_APS_INVALID_CLIENTID,
                                    TAF_APS_INVALID_OPID);

        TAF_APS_SetPdpEntAttachBearerFlag(ucPdpId, VOS_TRUE);

        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

        /* 同步PDP信息至SM */
        if (VOS_TRUE == pstPdpEntity->bitOpTransId)
        {
            MN_APS_SndSmBearerActivateInd(pstPdpEntity);
        }

        /* 需要将对应去激活PDP下的专有承载的信息同步给SM，
           因为这个时候SM已经将这些清除掉 */
        MN_APS_SndSmAllSecPdpActivateInd(pstPdpEntity);

        /* 主状态迁移至TAF_APS_STA_ACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();

    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aPdpDeactivateInd_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU        *pstSerStaInd;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg = TAF_APS_GetCurrSubFsmMsgAddr();

    /* 初始化, 获取消息内容 */
    pstSerStaInd        = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 刷新网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 正常流程下，不应该有这个场景，此处仍然尝试去激活操作 */
    if ( (MMC_APS_RAT_TYPE_GSM == pstSerStaInd->enRatType)
      || (MMC_APS_RAT_TYPE_WCDMA == pstSerStaInd->enRatType))
    {
        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)pstEntryMsg->aucEntryMsgBuffer;
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                break;
        }
    }
    if (MMC_APS_RAT_TYPE_LTE == pstSerStaInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF);
    }
    else
    {
         TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend: Wrong Msg");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);


    /* 根据EPS承载的操作类型(Activate, Deactivate, Modify),
       调用相应的处理函数 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitL4aDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPsCallEndCnf_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PS_CALL_END_CNF_STRU       *pstL4aPsCallEndCnf;

    /* 初始化, 获取消息内容 */
    pstL4aPsCallEndCnf      = (APS_L4A_PS_CALL_END_CNF_STRU*)pstMsg;

    /* 目前来说，断开拨号失败的场景是无法恢复的，故收到后不做处理 */
    if (APS_L4A_SUCCESS != pstL4aPsCallEndCnf->ulErrorCode)
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF,
                         "TAF_APS_RcvL4aPsCallEndCnf_MsDeactivating_WaitL4aDeactivateCnfSuspend,ulErrorCode",
                         pstL4aPsCallEndCnf->ulErrorCode);
    }

    return VOS_TRUE;
}

VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        /* 向SM发送本地去激活请求 */
        TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}

#endif
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvAtCallEndReq_MsActivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_GuMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 获取缓存消息内容 */
    pstEntryMsg      = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg        = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId          = TAF_APS_GetCurrFsmEntityPdpId();

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                          ucPdpId);

        /* 上报PDP操作结果 */
        TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl),
                                 TAF_PS_CAUSE_SIM_INVALID);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }

    /* 网络激活手动应答中",则该消息实际为: APP拒绝网络发起的激活
       发送内部激活消息，状态不迁移，在本状态机中处理
    */
    Aps_ActIndRejRsp(ucPdpId);

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                      ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq;

    /* 获取缓存消息内容 */
    pstEntryMsg          = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg            = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();

    /* 向L4A发送ID_APS_L4A_SET_CGACT_REQ消息, 去激活EPS承载 */
    TAF_APS_SndL4aSetCgactReq(ucPdpId, pstSetPdpCtxStateReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取缓存消息内容 */
    pstEntryMsg   = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg     = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 向L4A发送ID_APS_L4A_SET_NDISCONN_REQ消息, 去激活EPS承载 */
    TAF_APS_SndL4aCallEndReq(pstCallEndReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_PS_CALL_END_REQ_STRU            stCallEndReq;

    PS_MEM_SET(&stCallEndReq, 0, sizeof(TAF_PS_CALL_END_REQ_STRU));
    /* 获取当前状态机的APS实体索引 */
    ucPdpId                           = TAF_APS_GetCurrFsmEntityPdpId();

    /* 构造断开拨号参数 */
    stCallEndReq.ucCid             = TAF_APS_GetPdpEntCurrCid(ucPdpId);
    stCallEndReq.stCtrl.ulModuleId = TAF_APS_GetPdpEntModuleId(ucPdpId, stCallEndReq.ucCid);
    stCallEndReq.stCtrl.usClientId = TAF_APS_GetPdpEntClientId(ucPdpId, stCallEndReq.ucCid );
    stCallEndReq.stCtrl.ucOpId     = TAF_APS_GetPdpEntOpId(ucPdpId, stCallEndReq.ucCid);

    /* 向L4A发送ID_APS_L4A_SET_NDISCONN_REQ消息, 去激活EPS承载 */
    TAF_APS_SndL4aCallEndReq(&stCallEndReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_L4A_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return VOS_TRUE;
}

VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_LteMode( VOS_VOID )
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstEntryMsg          = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg            = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallHangupReq     = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /*获取PDP ID*/
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                          ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }

    /*向LTE发送挂断请求，L模保证挂断不会失败*/
    TAF_APS_SndL4aCgansRejReq(pstCallHangupReq);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING, ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef(VOS_VOID)
{
    VOS_UINT8                               ucPdpId;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg     = VOS_NULL_PTR;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity    = VOS_NULL_PTR;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg  = VOS_NULL_PTR;
    TAF_APS_TIMER_STATUS_ENUM_U8            enTimerStatus;

    /* 获取缓存消息内容 */
    pstEntryMsg   = TAF_APS_GetCurrSubFsmMsgAddr();

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(pstPdpEntity->ucNsapi);

    /* 检查定时器是否正在运行, 如果正在运行, 则需要停止定时器 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_RUNING == enTimerStatus)
    {
        /* 停止保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING,
                          ucPdpId);
    }

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 刷新成默认承载的CID */
    TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                TAF_APS_DEFAULT_CID,
                                TAF_APS_INVALID_MODULEID,
                                TAF_APS_INVALID_CLIENTID,
                                TAF_APS_INVALID_OPID);

    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

    /* 同步PDP信息至SM */
    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        MN_APS_SndSmBearerActivateInd(pstPdpEntity);
    }

    /* 主状态迁移至TAF_APS_STA_ACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}

#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



