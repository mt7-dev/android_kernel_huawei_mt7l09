

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafTypeDef.h"
#include "TafApsApi.h"
#include "Taf_Aps.h"
#include "MnApsComm.h"
#include "MmcApsInterface.h"
#include "TafLog.h"
#include "TafApsCtx.h"
#include "TafApsTimerMgmt.h"
#include "TafApsFsmMainTbl.h"
#include "TafApsFsmMsActivating.h"
#include "TafApsFsmMsActivatingTbl.h"
#include "TafApsFsmMsDeactivatingTbl.h"
#include "TafApsFsmMsModifyingTbl.h"
#include "TafApsProcIpFilter.h"
#include "TafApsSndSm.h"
#include "TafApsSndRabm.h"
#include "TafApsSndInternalMsg.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "SmEsmInterface.h"
#include "MnApsMultiMode.h"
#include "ApsL4aInterface.h"
#include "TafApsSndL4a.h"
#include "TafApsFsmMain.h"
#endif
#include "TafApsComFunc.h"
#include "ApsNdInterface.h"
#include "TafApsSndNd.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_APS_FSM_MSACTIVATING_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq;
    TAF_PDP_TABLE_STRU                 *pstCidCtx;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    VOS_UINT32                          ulTimerLen;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化, 获取PDP ID和当前接入技术 */
    pstAppMsg            = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType        = TAF_APS_GetCurrPdpEntityRatType();
    ucCid                = TAF_APS_GetPdpEntCurrCid(ucPdpId);
    pstCidCtx            = TAF_APS_GetTafCidInfoAddr(ucCid);

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 上报PDP操作结果 */
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpCtxStateReq->stCtrl),
                                    TAF_PS_CAUSE_SIM_INVALID);

        /* 清除资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }
    else
    {
        /* 如果CID对应的剩余定时器时长为0, 需要上报响应事件 */
        if (0 == TAF_APS_GetCallRemainTmrLen(ucCid))
        {
            TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpCtxStateReq->stCtrl),
                                         TAF_PS_CAUSE_SUCCESS);
        }
    }

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    if (APS_USED == pstCidCtx->ucPriCidFlag)
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(pstCidCtx->ucPriCid));
    }
    else
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(ucCid));
    }

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_GuMode();
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_LteMode();
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);

            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
            break;

        default:
            TAF_WARNING_LOG(WUEPS_PID_TAF,
                "TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_Init: Wrong RAT type!");
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    VOS_UINT32                          ulTimerLen;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化, 获取PDP ID和当前接入技术 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId           = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType     = TAF_APS_GetCurrPdpEntityRatType();
    ucCid             = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 上报PDP操作结果 */
        TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                 TAF_PS_CAUSE_SIM_INVALID);

        /* 清除资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }
    else
    {
        /* 如果CID对应的剩余定时器时长为0, 需要上报响应事件 */
        if (0 == TAF_APS_GetCallRemainTmrLen(ucCid))
        {
            TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                      TAF_PS_CAUSE_SUCCESS);
        }
    }

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType,pstMsg);

    TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(ucCid));

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_GuMode();
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_LteMode();
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);

            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
            break;

        default:
            TAF_WARNING_LOG(WUEPS_PID_TAF,
                "TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_Init: Wrong RAT type!");
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    VOS_UINT32                          ulTimerLen;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化, 获取PDP ID和当前接入技术 */
    pstAppMsg      = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType  = TAF_APS_GetCurrPdpEntityRatType();
    ucCid          = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 上报PDP操作结果 */
        TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                              pstCallOrigReq->stDialParaInfo.ucCid,
                              TAF_PS_CAUSE_SIM_INVALID);

        /* 清除资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }
    else
    {
        /* 如果CID对应的剩余定时器时长为0, 需要上报响应事件 */
        if (0 == TAF_APS_GetCallRemainTmrLen(ucCid))
        {
            TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                                   pstCallOrigReq->stDialParaInfo.ucCid,
                                   TAF_PS_CAUSE_SUCCESS);
        }
    }

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                 pstCallOrigReq->stDialParaInfo.ucCid,
                                 pstCallOrigReq->stDialParaInfo.enPdpType);

    switch (enCurrRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
        case MMC_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtPsCallOrigReq_MsActivating_GuMode();
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
            TAF_APS_SetPdpEntAddrType(ucPdpId, pstCallOrigReq->stDialParaInfo.enPdpType);
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_RcvAtPsCallOrigReq_MsActivating_LteMode();
            break;
#endif

        case MMC_APS_RAT_TYPE_NULL:
            TAF_APS_SetPdpEntAddrType(ucPdpId, pstCallOrigReq->stDialParaInfo.enPdpType);

            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_ACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);

            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
            break;

        default:
            TAF_WARNING_LOG(WUEPS_PID_TAF,
                "TAF_APS_RcvAtPsCallOrigReq_MsActivating_Init: Wrong RAT type!");
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId               = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP操作结果, ERROR */
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报PDP操作结果, NO_ERROR */
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_SUCCESS);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg      = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstPsCallEndReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstPsCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId         = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报PDP操作结果, NO_ERROR */
    TAF_APS_SndCallEndCnf(&(pstPsCallEndReq->stCtrl),
                         pstPsCallEndReq->ucCid,
                         TAF_PS_CAUSE_SUCCESS);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
    TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    pstEntryMsg            = TAF_APS_GetCurrSubFsmMsgAddr();
    pstMmcServiceStatusInd = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;
    ucPdpId                = TAF_APS_GetCurrFsmEntityPdpId();

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    /* 此时如果卡状态为无效，需要终止流程 */
    if (VOS_FALSE == pstMmcServiceStatusInd->ulPsSimRegStatus)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报错误事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SIM_INVALID);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }

    if ( (MMC_APS_RAT_TYPE_GSM   == pstMmcServiceStatusInd->enRatType)
      || (MMC_APS_RAT_TYPE_WCDMA == pstMmcServiceStatusInd->enRatType))
    {
        switch (TAF_ExtractMsgNameFromEvtType(pstEntryMsg->ulEventType))
        {
            case ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ:
                TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_GuMode();
                break;

            case ID_MSG_TAF_PS_PPP_DIAL_ORIG_REQ:
                TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_GuMode();
                break;

            case ID_MSG_TAF_PS_CALL_ORIG_REQ:
                TAF_APS_RcvAtPsCallOrigReq_MsActivating_GuMode();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvMmcServiceStatusInd_MsActivating_Suspend: Unknown message!");
                break;
        }
    }
#if (FEATURE_ON == FEATURE_LTE)
    else if (MMC_APS_RAT_TYPE_LTE == pstMmcServiceStatusInd->enRatType)
    {
        switch (TAF_ExtractMsgNameFromEvtType(pstEntryMsg->ulEventType))
        {
            case ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ:
                TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_LteMode();
                break;

            case ID_MSG_TAF_PS_PPP_DIAL_ORIG_REQ:
                TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_LteMode();
                break;

            case ID_MSG_TAF_PS_CALL_ORIG_REQ:
                TAF_APS_RcvAtPsCallOrigReq_MsActivating_LteMode();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvMmcServiceStatusInd_MsActivating_Suspend: Unknown message!");
                break;
        }
    }
#endif
    else
    {
        TAF_NORMAL_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_Suspend: RAT type is NULL!");
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvInterAttachBearerActivateInd_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    VOS_UINT32                          ulRemainTimeLen;
    VOS_UINT8                           ucPdpId;

    ulRemainTimeLen = 0;
    pstEntryMsg     = TAF_APS_GetCurrSubFsmMsgAddr();
    ucPdpId         = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取定时器剩余时长 */
    TAF_APS_GetRemainTimerLen(TI_TAF_APS_MS_ACTIVATING, ucPdpId, &ulRemainTimeLen);

    /* 如果剩余时长非0, 停止当前流程, 重新处理当前的状态机入口消息 */
    if (0 != ulRemainTimeLen)
    {
        /* 记录剩余时长 */
        TAF_APS_SetCallRemainTmrLen(TAF_APS_GetPdpEntCurrCid(ucPdpId), ulRemainTimeLen);

        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 发送状态机入口消息至内部消息队列 */
        TAF_APS_SndInterFsmEntryMsg(pstEntryMsg);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
#endif
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq   = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在去激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP激活错误事件 */
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 向SM发送ABORT请求, 取消当前激活操作 */
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstPsCallEndReq;

    /* 获取当前状态机的APS实体索引 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstPsCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报PDP操作结果, NO_ERROR */
    TAF_APS_SndCallEndCnf(&(pstPsCallEndReq->stCtrl),
                         pstPsCallEndReq->ucCid,
                         TAF_PS_CAUSE_SUCCESS);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
    TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 向SM发送ABORT请求, 取消当前激活操作 */
    TAF_APS_SndSmPdpAbortReq(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivInd_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_DEACTIVATE_IND_STRU      *pstSmPdpDeactivateInd;

    /* ID_PMC_SMREG_PDP_DEACTIV_IND消息处理说明,在状态机中不需要对TEARDOWN
       再进行判断处理,APS在收到ID_PMC_SMREG_PDP_DEACTIV_IND消息适?先进行预处
       理,如果消息中带有TEARDOWN,预处理中找出都有哪些PDP待去激活,然后,将
       ID_PMC_SMREG_PDP_DEACTIV_IND分发到相应的实体状态中,所以,在状态机中不再
       对ID_PMC_SMREG_PDP_DEACTIV_IND中的TEARDOWN再作处理 */

    /* 初始化, 获取消息内容 */
    pstSmPdpDeactivateInd = (SMREG_PDP_DEACTIVATE_IND_STRU *)pstMsg;

    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 本状态下收到ID_PMC_SMREG_PDP_DEACTIV_IND为异常消息,上报错误事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpDeactivateInd->enCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSmPdpActRej_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_ACTIVATE_REJ_STRU        *pstSmPdpActivateRej;

    /* 初始化, 获取消息内容 */
    pstSmPdpActivateRej = (SMREG_PDP_ACTIVATE_REJ_STRU *)pstMsg;
    ucPdpId             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 处理PDP去激活指示消息 ,上报给AT在Aps_PdpActRej内部实现 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpActivateRej->enCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSmPdpActSecRej_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_ACTIVATE_SEC_REJ_STRU    *pstSmSecPdpActivateRej;

    /* 初始化, 获取消息内容 */
    pstSmSecPdpActivateRej  = (SMREG_PDP_ACTIVATE_SEC_REJ_STRU *)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 处理PDP去激活指示消息 ,上报给AT在Aps_PdpActRej内部实现 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmSecPdpActivateRej->enCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSmPdpActCnf_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    APS_ACTCNF_PARA_ST                  stParam;
    #if ((FEATURE_ON == FEATURE_LTE)||(FEATURE_ON == FEATURE_IPV6))
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    #endif
    SMREG_PDP_ACTIVATE_CNF_STRU        *pstSmPdpActivateCnf;


    PS_MEM_SET(&stParam, 0x00, sizeof(stParam));


    /* 初始化, 获取消息内容 */
    pstSmPdpActivateCnf = (SMREG_PDP_ACTIVATE_CNF_STRU *)pstMsg;
    ucPdpId             = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType       = TAF_APS_GetCurrPdpEntityRatType();
    #if ((FEATURE_ON == FEATURE_LTE)||(FEATURE_ON == FEATURE_IPV6))
    pstPdpEntity        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);
    #endif

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 检查输入的参数 */
    ulRet = Aps_PdpActCnfParaCheck(pstSmPdpActivateCnf, &stParam);
    if (APS_PARA_VALID != ulRet)
    {
        if (APS_PDPTYPE_INVALID == ulRet)
        {
            /* 上报激活失败 */
            TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);

            /* 发送内部消息, 去激活PDP */
            TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);
        }
        else
        {
            /* 上报激活失败 */
            TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

            /* 发送内部消息, 去激活PDP */
            TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);
        }

        return VOS_TRUE;
    }

    /* 检查QOS是否满足MIN要求 */
    if (APS_PARA_VALID  == Aps_CheckQosSatisify(ucPdpId, &stParam.PdpQos))
    {
        /* 满足MINQOS */
        Aps_PdpActCnfQosSatisfy(ucPdpId, &stParam, pstSmPdpActivateCnf);

#if (FEATURE_ON == FEATURE_LTE)
        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_ACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
        /* 如果地址类型是IPv6, 需要同步给ND Client */
        if (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId))
        {
            TAF_APS_SndNdPdpActInd(pstPdpEntity->ucNsapi,
                                   pstPdpEntity->PdpAddr.aucIpV6Addr);

            pstPdpEntity->ulNdClientActiveFlg = VOS_TRUE;
        }
#endif

        switch (enCurrRatType)
        {
            case MMC_APS_RAT_TYPE_GSM:
                /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP */
                TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP);
                break;

            case MMC_APS_RAT_TYPE_WCDMA:
                /* 激活成功，启动流量统计 */
                TAF_APS_StartDsFlowStats(pstSmPdpActivateCnf->ucNsapi);

                /* 如果当前APS实体的PDP类型为IPv4, 需要配置IPF */
                if (TAF_APS_CheckPdpAddrTypeIpv4(ucPdpId))
                {
                    /* 配置IP过滤器 */
                    TAF_APS_IpfConfigUlFilter(ucPdpId);
                }

                /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态状态机 */
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
                TAF_APS_QuitCurrSubFsm();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvSmPdpActCnf_MsActivating_WaitSmActivateCnf: Wrong RAT type!");
                break;
        }
    }
    else
    {
        /* 不满足MINQOS */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_QOS_NOT_ACCEPTED);

        /* 发送内部消息, 去激活PDP */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_QOS_NOT_ACCEPTED);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpActSecCnf_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    APS_ACTSECCNF_PARA_ST               stParam;
    SMREG_PDP_ACTIVATE_SEC_CNF_STRU    *pstSmSecPdpActivateCnf;

    /* 初始化, 获取消息内容 */
    pstSmSecPdpActivateCnf  = (SMREG_PDP_ACTIVATE_SEC_CNF_STRU *)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatType           = TAF_APS_GetCurrPdpEntityRatType();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 检查输入的参数 */
    ulRet = Aps_PdpActSecCnfParaCheck(pstSmSecPdpActivateCnf, &stParam);
    if (APS_PARA_VALID != ulRet)
    {
        /* 上报激活失败 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER );

        /* 发送内部消息, 去激活PDP */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);

        return VOS_TRUE;
    }

    /* 检查QOS是否满足MIN要求 */
    if (APS_PARA_VALID == Aps_CheckQosSatisify(ucPdpId, &stParam.PdpQos))
    {
        /* 满足MINQOS */
        Aps_PdpActSecCnfQosSatisfy(ucPdpId, &stParam, pstSmSecPdpActivateCnf);

#if (FEATURE_ON == FEATURE_LTE)
        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId), SM_ESM_PDP_OPT_ACTIVATE);
#endif

        /* 根据当前模做状态迁移 */
        switch (enCurrRatType)
        {
            case MMC_APS_RAT_TYPE_GSM:

                /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP */
                TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP);
                break;

            case MMC_APS_RAT_TYPE_WCDMA:

                /* 如果当前APS实体的PDP类型为IPv4, 需要配置IPF, 目前不支持Secondary PDP */

                /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态状态机 */
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
                TAF_APS_QuitCurrSubFsm();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvSmPdpActSecCnf_MsActivating_WaitSmActivateCnf: Wrong RAT type!");
                break;
        }

    }
    else
    {
        /* 不满足MINQOS */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_QOS_NOT_ACCEPTED);

        /* 发送内部消息, 去激活PDP */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_QOS_NOT_ACCEPTED);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvApsInterPdpDeactivateReq_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_APS_APS_INTERNAL_PDP_DEACTIVATE_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstLocalMsg;

    /* 初始化, 获取消息内容 */
    ucPdpId     = TAF_APS_GetCurrFsmEntityPdpId();
    pstLocalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstMsg;

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 本状态下收到ID_APS_APS_LOCAL_PDP_DEACTIVATE_IND为异常消息,上报错误事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstLocalMsg->enCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstSerStaInd;

    /* 初始化, 获取消息内容 */
    pstSerStaInd = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstSerStaInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstSerStaInd->ulPsSimRegStatus);

    /* 此时到L模应该是一条异常消息 */
    if (MMC_APS_RAT_TYPE_NULL != pstSerStaInd->enRatType)
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnf: Wrong RAT Type!");

        return VOS_TRUE;
    }

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF_SUSPEND */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF_SUSPEND);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitSmActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 释放SM资源 */
    TAF_APS_SndSmPdpAbortReq(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq   = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在去激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if ( TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState )
    {
        /* 上报PDP激活错误事件*/
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 向SM发送ABORT请求, 取消当前激活操作 */
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq                   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq                      = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstPsCallEndReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstPsCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报PDP操作结果, NO_ERROR */
    TAF_APS_SndCallEndCnf(&(pstPsCallEndReq->stCtrl),
                         pstPsCallEndReq->ucCid,
                         TAF_PS_CAUSE_SUCCESS);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
    TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 向SM发送ABORT请求, 取消当前激活操作 */
    TAF_APS_SndSmPdpAbortReq(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq                    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivInd_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_DEACTIVATE_IND_STRU      *pstSmPdpDeactivateInd;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    pstSmPdpDeactivateInd = (SMREG_PDP_DEACTIVATE_IND_STRU *)pstMsg;

    /* ID_PMC_SMREG_PDP_DEACTIV_IND消息处理说明,在状态机中不需要对TEARDOWN
       再进行判断处理,APS在收到ID_PMC_SMREG_PDP_DEACTIV_IND消息首先进行预处
       理,如果消息中带有TEARDOWN,预处理中找出都有哪些PDP待去激活,然后,将
       ID_PMC_SMREG_PDP_DEACTIV_IND分发到相应的实体状态中,所以,在状态机中不再
       对ID_PMC_SMREG_PDP_DEACTIV_IND中的TEARDOWN再作处理 */

   /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 本状态下收到ID_PMC_SMREG_PDP_DEACTIV_IND为异常消息,上报错误事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpDeactivateInd->enCause));

    /* 清除资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE*/
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstLocalMsg;

    /* 初始化, 获取消息内容 */
    ucPdpId     = TAF_APS_GetCurrFsmEntityPdpId();
    pstLocalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstMsg;

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 本状态下收到ID_APS_APS_LOCAL_PDP_DEACTIVATE_IND为异常消息,上报错误事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstLocalMsg->enCause));

    /* 清除资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE*/
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSmPdpActRej_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_ACTIVATE_REJ_STRU        *pstSmPdpActivateRej;

    /* 初始化, 获取消息内容 */
    pstSmPdpActivateRej = (SMREG_PDP_ACTIVATE_REJ_STRU *)pstMsg;
    ucPdpId             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 处理PDP去激活指示消息 ,上报给AT在Aps_PdpActRej内部实现 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpActivateRej->enCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucPdpId;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
#endif
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;

    /* 初始化, 获取消息内容 */
    pstMmcServiceStatusInd = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;
#if (FEATURE_ON == FEATURE_LTE)
    ucPdpId                = TAF_APS_GetCurrFsmEntityPdpId();
    pstEntryMsg            = TAF_APS_GetCurrSubFsmMsgAddr();
#endif

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    if ( (MMC_APS_RAT_TYPE_GSM   == pstMmcServiceStatusInd->enRatType)
      || (MMC_APS_RAT_TYPE_WCDMA == pstMmcServiceStatusInd->enRatType) )
    {
        /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF);
    }
#if (FEATURE_ON == FEATURE_LTE)
    else if (MMC_APS_RAT_TYPE_LTE == pstMmcServiceStatusInd->enRatType)
    {
        /* LTE模需要向GU模发送abort消息,再向L模发送PDP激活请求 */
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        switch (TAF_ExtractMsgNameFromEvtType(pstEntryMsg->ulEventType))
        {
            case ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ:
                TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_LteMode();
                break;

            case ID_MSG_TAF_PS_PPP_DIAL_ORIG_REQ:
                TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_LteMode();
                break;

            case ID_MSG_TAF_PS_CALL_ORIG_REQ:
                TAF_APS_RcvAtPsCallOrigReq_MsActivating_LteMode();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnfSuspend: Unknown message!");
                break;
        }
    }
#endif
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnfSuspend: Wrong RAT type!");
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_APS_RcvInterAttachBearerActivateInd_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    VOS_UINT32                          ulRemainTimeLen;
    VOS_UINT8                           ucPdpId;

    ulRemainTimeLen = 0;
    pstEntryMsg     = TAF_APS_GetCurrSubFsmMsgAddr();
    ucPdpId         = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取定时器剩余时长 */
    TAF_APS_GetRemainTimerLen(TI_TAF_APS_MS_ACTIVATING, ucPdpId, &ulRemainTimeLen);

    /* 如果剩余时长非0, 停止当前流程, 重新处理当前的状态机入口消息 */
    if (0 != ulRemainTimeLen)
    {
        /* 记录剩余时长 */
        TAF_APS_SetCallRemainTmrLen(TAF_APS_GetPdpEntCurrCid(ucPdpId), ulRemainTimeLen);

        /* 向GU模发送ABORT消息 */
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 发送状态机入口消息至内部消息队列 */
        TAF_APS_SndInterFsmEntryMsg(pstEntryMsg);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 向SM发送ABORT请求 */
    TAF_APS_SndSmPdpAbortReq(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSmPdpActCnf_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucPdpId;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatTypeSuspend;
    APS_ACTCNF_PARA_ST                  stParam;
    #if ((FEATURE_ON == FEATURE_LTE)||(FEATURE_ON == FEATURE_IPV6))
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    #endif
    SMREG_PDP_ACTIVATE_CNF_STRU        *pstSmPdpActivateCnf;

    PS_MEM_SET(&stParam, 0x00, sizeof(stParam));

    /* 初始化, 获取消息内容 */
    pstSmPdpActivateCnf         = (SMREG_PDP_ACTIVATE_CNF_STRU *)pstMsg;
    ucPdpId                     = TAF_APS_GetCurrFsmEntityPdpId();
    enCurrRatTypeSuspend        = TAF_APS_GET_RAT_TYPE_IN_SUSPEND();
    #if ((FEATURE_ON == FEATURE_LTE)||(FEATURE_ON == FEATURE_IPV6))
    pstPdpEntity        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);
    #endif

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 检查输入的参数 */
    ulRet = Aps_PdpActCnfParaCheck(pstSmPdpActivateCnf, &stParam);
    if (APS_PARA_VALID != ulRet)
    {
        if (APS_PDPTYPE_INVALID == ulRet)
        {
            /* 上报激活失败 */
            TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);

            /* 发送内部消息, 去激活PDP */
            TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);
        }
        else
        {
            /* 上报激活失败 */
            TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

            /* 发送内部消息, 去激活PDP */
            TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);
        }

        return VOS_TRUE;
    }

    /* 检查QOS是否满足MIN要求 */
    if (APS_PARA_VALID  == Aps_CheckQosSatisify(ucPdpId, &stParam.PdpQos))
    {
        /* 满足MINQOS */
        Aps_PdpActCnfQosSatisfy(ucPdpId, &stParam, pstSmPdpActivateCnf);

#if (FEATURE_ON == FEATURE_LTE)
        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_ACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
        /* 如果地址类型是IPv6, 需要同步给ND Client */
        if (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId))
        {
            TAF_APS_SndNdPdpActInd(pstPdpEntity->ucNsapi,
                                   pstPdpEntity->PdpAddr.aucIpV6Addr);

            pstPdpEntity->ulNdClientActiveFlg = VOS_TRUE;
        }
#endif

        switch (enCurrRatTypeSuspend)
        {
            case MMC_APS_RAT_TYPE_GSM:
                /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP_SUSPEND */
                TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP_SUSPEND);
                break;

            case MMC_APS_RAT_TYPE_WCDMA:
                /* 激活成功，启动流量统计 */
                TAF_APS_StartDsFlowStats(pstSmPdpActivateCnf->ucNsapi);

                /* 如果当前APS实体的PDP类型为IPv4, 需要配置IPF */
                if (TAF_APS_CheckPdpAddrTypeIpv4(ucPdpId))
                {
                    /* 配置IP过滤器 */
                    TAF_APS_IpfConfigUlFilter(ucPdpId);
                }

                /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态状态机 */
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
                TAF_APS_QuitCurrSubFsm();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                    "TAF_APS_RcvSmPdpActCnf_MsActivating_WaitSmActivateCnf: Wrong RAT type!");
                break;
        }
    }
    else
    {
        /* 不满足MINQOS */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_QOS_NOT_ACCEPTED);

        /* 发送内部消息, 去激活PDP */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_QOS_NOT_ACCEPTED);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsInterPdpDeactivateReq_MsActivating_WaitSmActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_APS_APS_INTERNAL_PDP_DEACTIVATE_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq               = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在去激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP激活错误事件*/
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /*---------------------------------------------------------
           加载TAF_APS_STA_MS_DEACTIVATING状态机
           加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

           在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
           处理ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ消息
        ---------------------------------------------------------*/
        TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                           TAF_APS_GetMsDeactivatingFsmDescAddr(),
                           TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;


    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_MSG_TAF_PS_CALL_END_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq                    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivInd_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;

    SMREG_PDP_DEACTIVATE_IND_STRU      *pstSmPdpDeactivateInd;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    pstSmPdpDeactivateInd = (SMREG_PDP_DEACTIVATE_IND_STRU *)pstMsg;

    /* ID_PMC_SMREG_PDP_DEACTIV_IND消息处理说明,在状态机中不需要对TEARDOWN
       再进行判断处理,APS在收到ID_PMC_SMREG_PDP_DEACTIV_IND消息进行预处理,
       如果消息中带有TEARDOWN,预处理中找出都有哪些PDP待去激活,然后,将
       ID_PMC_SMREG_PDP_DEACTIV_IND分发到相应的实体状态中,所以,在状态机中不再
       对ID_PMC_SMREG_PDP_DEACTIV_IND中的TEARDOWN再作处理 */

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(pstPdpEntity->ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_DEACTIVATE);
#endif

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpDeactivateInd->enCause));


    /* 释放SNDCP资源 */
    Aps_ReleaseSndcpResource(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSndcpActivateRsp_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    APS_SNDCP_ACTIVATE_RSP_ST          *pstSnActivateRsp;

    /* 初始化, 获取消息内容 */
    ulResult                            = VOS_OK;
    pstSnActivateRsp                    = &((APS_SNDCP_ACTIVATE_RSP_MSG*)pstMsg)->ApsSnActRsp;
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity                        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 检查消息参数 */
    ulResult = Aps_SnMsgModSnActRspParaCheck(pstSnActivateRsp);
    if (APS_PARA_VALID != ulResult)
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvSndcpActivateRsp_MsActivating_WaitSndcpActivateRsp: Check para failed!");

        /* 上报激活失败 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

        /* 发送内部消息, 触发PDP去激活流程 */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

        return VOS_TRUE;
    }

    /* 收到SN_ACT_RSP后修改APS实体参数, 包括XID参数, TRANSMODE */
    Aps_SnActRspChngEntity(pstSnActivateRsp, ucPdpId);

    /* 设置RABM的传输模式 */
    TAF_APS_SndRabmSetTransModeMsg(pstSnActivateRsp->ucNsapi,
                                   pstPdpEntity->GprsPara.TransMode);

    /* 如果当前APS实体的PDP类型为IPv4, 需要配置IPF, 目前只支持Primary PDP */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv4(ucPdpId)) )
    {
        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);
    }

    /* 激活成功，启动流量统计 */
    TAF_APS_StartDsFlowStats(pstSnActivateRsp->ucNsapi);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
    TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpStatusReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucPdpId;
    APS_SNDCP_STATUS_REQ_ST            *pstSnStatusReq;

    pstSnStatusReq                      = &((APS_SNDCP_STATUS_REQ_MSG*)pstMsg)->ApsSnStatusReq;
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    ulRet = TAF_APS_ValidatePdpForSnStatusReq(ucPdpId, pstSnStatusReq);
    if (VOS_TRUE == ulRet)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /*上报激活失败并清资源*/
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_LLC_OR_SNDCP_FAILURE);

        /* 发送内部消息, 触发PDP去激活流程 */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvApsInterPdpDeactivateReq_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_APS_APS_INTERNAL_PDP_DEACTIVATE_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                               ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;

    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstLocalMsg;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);
    pstLocalMsg  = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstMsg;

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(pstPdpEntity->ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_DEACTIVATE);
#endif

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstLocalMsg->enCause));

    /* 释放SNDCP资源 */
    Aps_ReleaseSndcpResource(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvMmmcServiceStatusInd_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;

    /* 初始化, 获取消息内容 */
    pstMmcServiceStatusInd  = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    /* 此时到NULL模应该是一条异常消息 */
    if (MMC_APS_RAT_TYPE_NULL != pstMmcServiceStatusInd->enRatType)
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnf: Wrong RAT Type:", pstMmcServiceStatusInd->enRatType);

        return VOS_TRUE;
    }

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP_SUSPEND */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP_SUSPEND);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitSndcpActivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, SM_TAF_CAUSE_SM_MAX_TIME_OUT);

    /* 发送内部消息, 触发PDP去激活流程 */
    TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq               = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP激活错误事件*/
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /*---------------------------------------------------------
           加载TAF_APS_STA_MS_DEACTIVATING状态机
           加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

           在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
           处理ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ消息
        ---------------------------------------------------------*/
        TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                           TAF_APS_GetMsDeactivatingFsmDescAddr(),
                           TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq                   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq                      = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_MSG_TAF_PS_CALL_END_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitSndcpActivateRspSuspend(
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
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    SMREG_PDP_DEACTIVATE_IND_STRU      *pstSmPdpDeactivateInd;

    /* ID_PMC_SMREG_PDP_DEACTIV_IND消息处理说明,在状态机中不需要对TEARDOWN
       再进行判断处理,APS在收到ID_PMC_SMREG_PDP_DEACTIV_IND消息适?先进行预处
       理,如果消息中带有TEARDOWN,预处理中找出都有哪些PDP待去激活,然后,将
       ID_PMC_SMREG_PDP_DEACTIV_IND分发到相应的实体状态中,所以,在状态机中不再
       对ID_PMC_SMREG_PDP_DEACTIV_IND中的TEARDOWN再作处理 */

    /* 初始化, 获取消息内容 */
    pstSmPdpDeactivateInd = (SMREG_PDP_DEACTIVATE_IND_STRU *)pstMsg;

    /* 获取当前状态机的APS索引 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstSmPdpDeactivateInd->enCause));

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId), SM_ESM_PDP_OPT_DEACTIVATE);
#endif

    /* 释放SNDCP资源 */
    Aps_ReleaseSndcpResource(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpStatusReqMsgType_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucPdpId;
    APS_SNDCP_STATUS_REQ_ST            *pstSnStatusReq;

    /* 获取消息内容 */
    pstSnStatusReq = &((APS_SNDCP_STATUS_REQ_MSG*)pstMsg)->ApsSnStatusReq;

    /* 获取当前状态机的APS索引 */
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();

    /* 检查SNDCP指定的SAPI与APS实体的SAPI是否匹配 */
    ulRet = TAF_APS_ValidatePdpForSnStatusReq(ucPdpId, pstSnStatusReq);
    if (VOS_TRUE == ulRet)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_LLC_OR_SNDCP_FAILURE);

        /* 发送内部消息, 触发PDP去激活流程 */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);
    }
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsInterPdpDeactivateReq_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_APS_APS_INTERNAL_PDP_DEACTIVATE_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstLocalMsg;

    /* 获取当前状态机的APS索引 */
    ucPdpId     = TAF_APS_GetCurrFsmEntityPdpId();
    pstLocalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstMsg;

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapSmCause(pstLocalMsg->enCause));

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId), SM_ESM_PDP_OPT_DEACTIVATE);
#endif

    /* 释放SNDCP资源 */
    Aps_ReleaseSndcpResource(ucPdpId);

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;

    /* 初始化, 获取消息内容 */
    pstMmcServiceStatusInd  = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity            = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    switch (pstMmcServiceStatusInd->enRatType)
    {
        case MMC_APS_RAT_TYPE_GSM:
            /* GU模下 ,不需要处理,继续等SM的回复,状态回到
               TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP */
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SNDCP_ACTIVATE_RSP);
            break;

        case MMC_APS_RAT_TYPE_WCDMA:
#if (FEATURE_ON == FEATURE_LTE)
        case MMC_APS_RAT_TYPE_LTE:
#endif
            /* 停止激活流程定时器 */
            TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                              ucPdpId);

            /* 释放SNDCP资源 */
            Aps_ReleaseSndcpResource(ucPdpId);

            /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
            TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

            /* 激活成功，启动流量统计 */
            TAF_APS_StartDsFlowStats(pstPdpEntity->ucNsapi);

            /* 主状态迁移至TAF_APS_STA_ACTIVE_SUSPEND, 退出子状态机 */
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
            TAF_APS_QuitCurrSubFsm();
            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF,
                "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitSmActivateCnf: Wrong RAT Type:", pstMmcServiceStatusInd->enRatType);
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 发送内部消息, 触发PDP去激活流程 */
    TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_MAX_TIME_OUT);
    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvSndcpActivateRsp_MsActivating_WaitSndcpActivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    APS_SNDCP_ACTIVATE_RSP_ST          *pstSnActivateRsp;

    /* 初始化, 获取消息内容 */
    ulResult                            = VOS_OK;
    pstSnActivateRsp                    = &((APS_SNDCP_ACTIVATE_RSP_MSG*)pstMsg)->ApsSnActRsp;
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity                        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 检查消息参数 */
    ulResult = Aps_SnMsgModSnActRspParaCheck(pstSnActivateRsp);
    if (APS_PARA_VALID != ulResult)
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvSndcpActivateRsp_MsActivating_WaitSndcpActivateRsp: Check para failed!");

        /* 上报激活失败 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

        /* 发送内部消息, 触发PDP去激活流程 */
        TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED);

        return VOS_TRUE;
    }

    /* 收到SN_ACT_RSP后修改APS实体参数, 包括XID参数, TRANSMODE */
    Aps_SnActRspChngEntity(pstSnActivateRsp, ucPdpId);

    /* 设置RABM的传输模式 */
    TAF_APS_SndRabmSetTransModeMsg(pstSnActivateRsp->ucNsapi,
                                   pstPdpEntity->GprsPara.TransMode);

    /* 如果当前APS实体的PDP类型为IPv4, 需要配置IPF, 目前只支持Primary PDP */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv4(ucPdpId)) )
    {
        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);
    }

    /* 激活成功，启动流量统计 */
    TAF_APS_StartDsFlowStats(pstSnActivateRsp->ucNsapi);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
    TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq               = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在去激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP激活错误事件*/
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 向SM发送ABORT请求, 取消当前激活操作 */
        TAF_APS_SndL4aAbortReq(TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq                   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq                      = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_MSG_TAF_PS_CALL_END_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPdpActivateCnf_MsActivate_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_PDP_ACTIVATE_CNF_STRU      *pstL4aPdpActivateCnf;
#if (FEATURE_ON == FEATURE_IPV6)
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
#endif
    VOS_UINT32                          ulRet;

    /* 初始化, 获取消息内容 */
    pstL4aPdpActivateCnf = (APS_L4A_PDP_ACTIVATE_CNF_STRU *)pstMsg;
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();
#if (FEATURE_ON == FEATURE_IPV6)
    pstPdpEntity         = TAF_APS_GetPdpEntInfoAddr(ucPdpId);
#endif

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 缺省承载激活成功后, 需要检查PDP类型是否匹配 */
    if (VOS_FALSE == pstL4aPdpActivateCnf->bitOpLinkCid)
    {
        ulRet = TAF_APS_ValidatePdpType(TAF_APS_GetPdpEntDialPdpType(ucPdpId, pstL4aPdpActivateCnf->ucCid),
                                        pstL4aPdpActivateCnf->stIpAddrInfo.enPdpType);
        if (ulRet != VOS_OK)
        {
            /* 上报激活失败 */
            TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);

            /* 发送内部消息, 去激活PDP */
            TAF_APS_SndInterPdpDeactivateReq(ucPdpId, SM_TAF_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER);

            return VOS_TRUE;
        }
    }

#if (FEATURE_ON == FEATURE_IPV6)
    /* 更新错误码 */
    if ( (VOS_TRUE == pstL4aPdpActivateCnf->bitOpEsmCause)
      && (APS_L4A_SUCCESS != pstL4aPdpActivateCnf->ulEsmCause) )
    {
        pstPdpEntity->bitOpCause    = VOS_TRUE;
        pstPdpEntity->enCause       = TAF_APS_MapL4aCause(pstL4aPdpActivateCnf->ulEsmCause);
    }
#endif

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF事件 */
    TAF_APS_SndPdpActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /*------------------------------------------------------------------
       如果地址类型是IPv6, 需要同步给ND Client, 目前架构需要在收到ESM
       的EPS承载同步消息后通知ND Client
    ------------------------------------------------------------------*/

    /* 激活成功，启动流量统计 */
    TAF_APS_StartDsFlowStats((VOS_UINT8)pstL4aPdpActivateCnf->ulRabId);

    /* 主状态迁移至TAF_APS_STA_ACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvL4aPdpActivateRej_MsActivate_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    APS_L4A_PDP_ACTIVATE_REJ_STRU      *pstL4aPdpActivateRej;

    /* 初始化, 获取消息内容 */
    pstL4aPdpActivateRej    = (APS_L4A_PDP_ACTIVATE_REJ_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapL4aCause(pstL4aPdpActivateRej->ulEsmCause));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;

    /* 初始化, 获取消息内容 */
    pstMmcServiceStatusInd = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    if (MMC_APS_RAT_TYPE_NULL != pstMmcServiceStatusInd->enRatType)
    {
        TAF_ERROR_LOG1(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitL4aActivateCnf: Wrong RAT type:", pstMmcServiceStatusInd->enRatType);
        return VOS_TRUE;
    }

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF_SUSPEND */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF_SUSPEND);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsActivating_WaitL4aActivateCnf(
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
    if (SM_ESM_PDP_OPT_ACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptActivate(pstPdpEntity, pstBearerInfo);

#if (FEATURE_ON == FEATURE_IPV6)
        /* 如果地址类型是IPv6, 需要同步给ND Client */
        if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
          && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
        {
            TAF_APS_SndNdPdpActInd(pstPdpEntity->ucNsapi,
                                   pstPdpEntity->PdpAddr.aucIpV6Addr);

            pstPdpEntity->ulNdClientActiveFlg = VOS_TRUE;
        }
#endif
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_Inactive: Wrong option.");
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 向SM发送ABORT请求, 取消当前激活操作 */
    TAF_APS_SndL4aAbortReq(TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetNdisconnCnf_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_SET_NDISCONN_CNF_STRU      *pstL4aSetNdisconnCnf;

    /* 初始化, 获取消息内容 */
    pstL4aSetNdisconnCnf    = (APS_L4A_SET_NDISCONN_CNF_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    if (APS_L4A_SUCCESS != pstL4aSetNdisconnCnf->ulErrorCode)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_UNKNOWN);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aPppDialCnf_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_PPP_DIAL_CNF_STRU          *pstL4aPppDialCnf;

    /* 初始化, 获取消息内容 */
    pstL4aPppDialCnf        = (APS_L4A_PPP_DIAL_CNF_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    if (APS_L4A_SUCCESS != pstL4aPppDialCnf->ulErrorCode)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_UNKNOWN);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq               = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 正在去激活该APS实体, 再激活该APS实体所用的CID,上报ERROR事件,
       否则,发起去激活状态进行迁移 */
    if (TAF_CGACT_ACT == pstPdpContextStateReq->stCidListStateInfo.ucState)
    {
        /* 上报PDP激活错误事件*/
        TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
    }
    else
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq                   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq                      = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstPsCallEndReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstPsCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);


    /* 上报PDP操作结果, NO_ERROR */
    TAF_APS_SndCallEndCnf(&(pstPsCallEndReq->stCtrl),
                         pstPsCallEndReq->ucCid,
                         TAF_PS_CAUSE_SUCCESS);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
    TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                            TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aPdpActivateRej_MsActivate_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_PDP_ACTIVATE_REJ_STRU      *pstL4aPdpActivateRej;

    /* 初始化, 获取消息内容 */
    pstL4aPdpActivateRej                = (APS_L4A_PDP_ACTIVATE_REJ_STRU*)pstMsg;
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 如果错误码不是异系统, 则该激活流程终止 */
    if (APS_L4A_ERR_SM_SUSPENDED != pstL4aPdpActivateRej->ulEsmCause)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_APS_MapL4aCause(pstL4aPdpActivateRej->ulEsmCause));

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);
    }

    return VOS_TRUE;

}
VOS_UINT32 TAF_APS_RcvTiMsActivatingExpired_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                      ucPdpId);

    /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
    TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_SM_MAX_TIME_OUT);

    /* 向SM发送ABORT请求, 取消当前激活操作 */
    TAF_APS_SndL4aAbortReq(TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 释放APS资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsActivating_WaitL4aActivateCnfSuspend(
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
    if (SM_ESM_PDP_OPT_ACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptActivate(pstPdpEntity, pstBearerInfo);

#if (FEATURE_ON == FEATURE_IPV6)
        /* 如果地址类型是IPv6, 需要同步给ND Client */
        if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
          && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
        {
            TAF_APS_SndNdPdpActInd(pstPdpEntity->ucNsapi,
                                   pstPdpEntity->PdpAddr.aucIpV6Addr);

            pstPdpEntity->ulNdClientActiveFlg = VOS_TRUE;
        }
#endif
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_Inactive: Wrong option.");
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_APS_SERVICE_STATUS_IND_STRU    *pstMmcServiceStatusInd;

    pstMmcServiceStatusInd = (MMC_APS_SERVICE_STATUS_IND_STRU*)pstMsg;

    /* 设置当前网络类型 */
    TAF_APS_SetCurrPdpEntityRatType(pstMmcServiceStatusInd->enRatType);

    /* 设置PS域SIM卡状态信息 */
    TAF_APS_SetCurrPdpEntitySimRegStatus(pstMmcServiceStatusInd->ulPsSimRegStatus);

    if (MMC_APS_RAT_TYPE_LTE != pstMmcServiceStatusInd->enRatType)
    {
        TAF_ERROR_LOG1(WUEPS_PID_TAF,
            "TAF_APS_RcvMmcServiceStatusInd_MsActivating_WaitL4aActivateCnfSuspend: Wrong RAT type:", pstMmcServiceStatusInd->enRatType);
        return VOS_TRUE;
    }

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetNdisconnCnf_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_SET_NDISCONN_CNF_STRU      *pstL4aSetNdisconnCnf;

    /* 初始化, 获取消息内容 */
    pstL4aSetNdisconnCnf    = (APS_L4A_SET_NDISCONN_CNF_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    if (APS_L4A_SUCCESS != pstL4aSetNdisconnCnf->ulErrorCode)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_UNKNOWN);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aPppDialCnf_MsActivating_WaitL4aActivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_L4A_PPP_DIAL_CNF_STRU          *pstL4aPppDialCnf;

    /* 初始化, 获取消息内容 */
    pstL4aPppDialCnf        = (APS_L4A_PPP_DIAL_CNF_STRU*)pstMsg;
    ucPdpId                 = TAF_APS_GetCurrFsmEntityPdpId();

    if (APS_L4A_SUCCESS != pstL4aPppDialCnf->ulErrorCode)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING,
                          ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_UNKNOWN);

        /* 释放APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvApsInterPdpDeactivateReq_MsActivating_WaitL4aActivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /*---------------------------------------------------------
       加载TAF_APS_STA_MS_DEACTIVATING状态机
       加载后子状态切换为TAF_APS_MS_DEACTIVATING_SUBSTA_INIT

       在TAF_APS_MS_DEACTIVATING_SUBSTA_INIT子状态中
       处理ID_APS_APS_INTERNAL_PDP_DEACTIVATE_REQ消息
    ---------------------------------------------------------*/
    TAF_APS_InitSubFsm(TAF_APS_FSM_MS_DEACTIVATING,
                       TAF_APS_GetMsDeactivatingFsmDescAddr(),
                       TAF_APS_MS_DEACTIVATING_SUBSTA_INIT);

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_GuMode(VOS_VOID)
{
    TAF_PDP_TABLE_STRU                 *pstTafCidInfo;
    TAF_PS_CAUSE_ENUM_UINT32            enRet;
    VOS_UINT32                          ulTimerLen;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化 */
    enRet   = TAF_PS_CAUSE_SUCCESS;

    /* 获取APS实体索引 */
    ucPdpId = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid   = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    pstTafCidInfo = TAF_APS_GetTafCidInfoAddr(ucCid);
    if (APS_USED == pstTafCidInfo->ucPriCidFlag)
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(pstTafCidInfo->ucPriCid));
    }
    else
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(ucCid));
    }

    /* 发起PDP激活请求 */
    enRet = TAF_APS_PsCallOrig(ucPdpId);
    if (TAF_PS_CAUSE_SUCCESS != enRet)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, enRet);

        /* 清除APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_GuMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_PS_CAUSE_ENUM_UINT32            enRet;
    VOS_UINT32                          ulTimerLen;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化 */
    enRet             = TAF_PS_CAUSE_SUCCESS;

    /* 获取消息内容 */
    pstEntryMsg       = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg         = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId           = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid             = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(ucCid));

    /* 发起PDP激活请求 */
    enRet = MN_APS_RcvPppDialOrigReq(ucPdpId,
                                     &(pstPppDialOrigReq->stPppDialParaInfo));
    if (TAF_PS_CAUSE_SUCCESS != enRet)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, enRet);

        /* 清除APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_GuMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_PS_CAUSE_ENUM_UINT32            enRet;
    VOS_UINT32                          ulTimerLen;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化错误码 */
    enRet          = TAF_PS_CAUSE_SUCCESS;

    /* 获取消息内容 */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg      = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallOrigReq = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid          = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 设置呼叫IP类型 */
    TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, pstCallOrigReq->stDialParaInfo.enPdpType);

    /* 发起PDP激活请求 */
    enRet = MN_APS_RcvCallOrigReq(ucPdpId, &(pstCallOrigReq->stDialParaInfo));
    if (TAF_PS_CAUSE_SUCCESS != enRet)
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, enRet);

        /* 清除APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_SM_ACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsActivating_LteMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq;
    TAF_PDP_TABLE_STRU                 *pstCidCtx;
    VOS_UINT32                          ulTimerLen;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    VOS_UINT8                           ucAttachAllowFlg;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 获取消息内容 */
    pstEntryMsg          = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg            = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid                = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 如果注册承载不存在且注册允许标识为TRUE, 则缓存该呼叫建立请求 */
    ucAttachAllowFlg = TAF_APS_GetAttachAllowFlg();
    if ( (VOS_FALSE == TAF_APS_IsAnyBearerExist())
      && (VOS_TRUE == ucAttachAllowFlg) )
    {
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
        return VOS_TRUE;
    }

    /* 保存该次激活请求的PDP类型，如果是二次PDP激活，则需要找到主PDP所对应的CID */
    pstCidCtx = TAF_APS_GetTafCidInfoAddr(ucCid);
    if (APS_USED == pstCidCtx->ucPriCidFlag)
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(pstCidCtx->ucPriCid));
    }
    else
    {
        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_GetTafCidInfoPdpType(ucCid));
    }

    /* 向L4A发送EPS承载激活请求 */
    TAF_APS_SndL4aSetCgactReq(ucPdpId, pstSetPdpCtxStateReq);

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                     TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
        TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
        TAF_APS_SetCallRemainTmrLen(ucCid, 0);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsPppDialOrigReq_MsActivating_LteMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;
    TAF_APS_AUTH_INFO_STRU              stAuthInfo;
    TAF_APS_IPCP_INFO_STRU              stIpcpInfo;
    VOS_UINT32                          ulDecAuthRst;
    VOS_UINT32                          ulDecIpcpRst;
    VOS_UINT32                          ulTimerLen;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    VOS_UINT8                           ucAttachAllowFlg;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 初始化 */
    PS_MEM_SET(&stAuthInfo, 0, sizeof(TAF_APS_AUTH_INFO_STRU));
    PS_MEM_SET(&stIpcpInfo, 0, sizeof(TAF_APS_IPCP_INFO_STRU));

    /* 获取消息内容 */
    pstEntryMsg       = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg         = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId           = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid             = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 如果注册承载不存在且注册允许标识为TRUE, 则缓存该呼叫建立请求 */
    ucAttachAllowFlg = TAF_APS_GetAttachAllowFlg();
    if ( (VOS_FALSE == TAF_APS_IsAnyBearerExist())
      && (VOS_TRUE == ucAttachAllowFlg) )
    {
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
        return VOS_TRUE;
    }

    /* 解码PPP DIAL REQ中AUTH和IPCP信元 */
    ulDecAuthRst = TAF_APS_DecodeAuthInfo(&stAuthInfo,
                                             &(pstPppDialOrigReq->stPppDialParaInfo.stPppReqConfigInfo.stAuth));
    ulDecIpcpRst = TAF_APS_DecodeIpcpPkt(&stIpcpInfo,
                                         pstPppDialOrigReq->stPppDialParaInfo.stPppReqConfigInfo.stIPCP.aucIpcp,
                                         pstPppDialOrigReq->stPppDialParaInfo.stPppReqConfigInfo.stIPCP.usIpcpLen);
    if ((VOS_OK != ulDecAuthRst)
     || (VOS_OK != ulDecIpcpRst))
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_ACTIVATING, ucPdpId);

        /* 上报ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ事件 */
        TAF_APS_SndPdpActivateRej(ucPdpId, TAF_PS_CAUSE_UNKNOWN);

        /* 清除APS资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return VOS_TRUE;
    }

    /* 保存该次激活请求的PDP类型, PPP只支持IPV4 */
    TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                TAF_APS_GetPdpEntCurrCid(ucPdpId),
                                TAF_PDP_IPV4);

    /* 向L4A发送PPP激活请求 */
    TAF_APS_SndL4aPppDialReq(pstPppDialOrigReq,
                             &stAuthInfo,
                             &stIpcpInfo);

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                     TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
        TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
        TAF_APS_SetCallRemainTmrLen(ucCid, 0);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsActivating_LteMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;
    VOS_UINT32                          ulTimerLen;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    VOS_UINT8                           ucAttachAllowFlg;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;

    /* 获取消息内容 */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg      = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallOrigReq = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取APS实体索引 */
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取CID */
    ucCid          = TAF_APS_GetPdpEntCurrCid(ucPdpId);

    /* 如果注册承载不存在且注册允许标识为TRUE, 则缓存该呼叫建立请求 */
    ucAttachAllowFlg = TAF_APS_GetAttachAllowFlg();
    if ( (VOS_FALSE == TAF_APS_IsAnyBearerExist())
      && (VOS_TRUE == ucAttachAllowFlg) )
    {
        TAF_APS_SetPdpEntAddrType(ucPdpId, pstCallOrigReq->stDialParaInfo.enPdpType);
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_SUSPEND);
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                         TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
            TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
            TAF_APS_SetCallRemainTmrLen(ucCid, 0);
        }
        return VOS_TRUE;
    }

    /* 设置呼叫IP类型 */
    TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                 ucCid,
                                 pstCallOrigReq->stDialParaInfo.enPdpType);

    /* 向L4A发送EPS承载激活请求 */
    TAF_APS_SndL4aCallOrigReq(pstCallOrigReq);

    /* 子状态迁移至TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_ACTIVATING_SUBSTA_WAIT_L4A_ACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_ACTIVATING, ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        ulTimerLen = (0 != TAF_APS_GetCallRemainTmrLen(ucCid)) ?
                     TAF_APS_GetCallRemainTmrLen(ucCid) : TI_TAF_APS_MS_ACTIVATING_LEN;
        TAF_APS_StartTimer(TI_TAF_APS_MS_ACTIVATING, ulTimerLen, ucPdpId);
        TAF_APS_SetCallRemainTmrLen(ucCid, 0);
    }

    return VOS_TRUE;
}

#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

