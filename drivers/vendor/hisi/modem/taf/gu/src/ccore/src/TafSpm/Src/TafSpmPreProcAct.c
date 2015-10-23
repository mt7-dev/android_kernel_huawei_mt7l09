

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "PsTypeDef.h"
#include "PsCommonDef.h"
#include "MnCallApi.h"
#include "MnCallReqProc.h"
#include "TafLog.h"
#include "MnCallMgmt.h"
#include "MnCallMnccProc.h"
#include "TafSpmServiceDomainSelProc.h"
#include "TafMmaCtx.h"
#include "TafSdcLib.h"
#include "TafSpmSndInternalMsg.h"
#include "SpmImsaInterface.h"
#include "TafSpmRedial.h"
#include "TafSpmRedial.h"
#include "TafSpmPreProcTbl.h"
#include "TafSpmPreProcAct.h"
#include "TafAgentInterface.h"
#include "TafSpmComFunc.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "CallImsaInterface.h"
#endif

#include "TafSpmRedial.h"
#include "TafSpmCtx.h"
#include "NasSms.h"
#include "TafSpmMntn.h"
#include "MnMsgApi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_SPM_PREPROC_ACT_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 TAF_SPM_RcvAppOrigReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_CALL_APP_REQ_MSG_STRU           *pstAppMsg  = VOS_NULL_PTR;
    MN_CALL_EMERGENCY_CAT_STRU          stEmergencyCat;

    pstAppMsg                 = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;
    
    PS_MEM_SET(&stEmergencyCat, 0, sizeof(MN_CALL_EMERGENCY_CAT_STRU));    
    
    /* call is allowed if phone mode is power on */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_POWER_OFF);
        
        return VOS_TRUE;
    }

    /* VIDEO call当做普通呼叫处理不做紧急呼叫号码检查 */
    if (MN_CALL_TYPE_VIDEO != pstAppMsg->unParm.stOrig.enCallType)
    {
        /* 判断是紧急呼更新紧急呼叫的CAT信息 */
        if (VOS_TRUE  == TAF_SPM_IsEmergencyNum(&pstAppMsg->unParm.stOrig.stDialNumber, 
                                                VOS_TRUE,
                                                &stEmergencyCat))
        {
            pstAppMsg->unParm.stOrig.enCallType  = MN_CALL_TYPE_EMERGENCY;

            PS_MEM_CPY(&(pstAppMsg->unParm.stOrig.stEmergencyCat),
                        &stEmergencyCat,
                        sizeof(MN_CALL_EMERGENCY_CAT_STRU));

            return VOS_FALSE;
        }
    }  

    /* forbid normal call when USIM service is not available */
    if (VOS_FALSE == TAF_SPM_IsUsimServiceAvailable())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_SIM_NOT_EXIST);
        return VOS_TRUE;
    }
    
    /* 需要判断两个域卡无效,因为单域卡无效的情况能会disable LTE到GU下,需要到GU下继续尝试 */
    if (VOS_FALSE == TAF_SDC_IsUsimStausValid())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_SIM_INVALID);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_SPM_RcvAppSupsCmdReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
#if (FEATURE_ON == FEATURE_IMS)    
    MN_CALL_APP_REQ_MSG_STRU           *pstAppMsg = VOS_NULL_PTR;
#endif
    
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }
    else
    {
        /* 该场景解决由于SRVCC刚切换成功，如果此时用户发起挂断相应的call时，需要
         * 清除重拨缓存中保存的call请求信息，避免在GU下又发起重拨
         */
        pstAppMsg = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;
        
        /* 用户挂机，如果有对应的重拨缓存存在，清除它 */
        switch (pstAppMsg->unParm.stCallMgmtCmd.enCallSupsCmd)
        {
            case MN_CALL_SUPS_CMD_REL_CALL_X:
                TAF_SPM_FreeCallRedialBufferWithCallId(pstAppMsg->callId);
                break;
                
            case MN_CALL_SUPS_CMD_REL_ALL_CALL:
            case MN_CALL_SUPS_CMD_REL_ALL_EXCEPT_WAITING_CALL:
                TAF_SPM_FreeCallRedialBufferWithClientId(pstAppMsg->clientId);
                break;

            default:
                break;
        }    
    }
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppGetInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppStartDtmfReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppStopDtmfReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppGetCdurReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppGetCallInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppGetClprReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    ulRet           = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域呼叫存在，直接选择IMS域，其他情况返回VOS_FALSE，到CALL模块处理 */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }    
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvAppSendRpdataDirect_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulCause;

    /* 检查当前卡状态及开关机状态是否允许发起SMS业务 */
    if (VOS_FALSE == TAF_SPM_IsSmsServiceReqAllowed_PreProc(ulEventType, pstMsg, &ulCause))
    {
        /* 如果不允许发起SMS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSmsServiceRequetFail(ulEventType, pstMsg, ulCause);
        
        return VOS_TRUE;
    }
    
    /* 允许发起SMS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;

}
VOS_UINT32 TAF_SPM_RcvProcUssSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg   = VOS_NULL_PTR;
    TAF_SS_PROCESS_USS_REQ_STRU        *pstSsReqMsg = VOS_NULL_PTR;
    TAF_PH_ERR_CODE                     enCause;

    pstRcvMsg   = (MN_APP_REQ_MSG_STRU *)pstMsg;
    pstSsReqMsg = (TAF_SS_PROCESS_USS_REQ_STRU *)&(pstRcvMsg->aucContent[0]);

    /* configure CUSD setting */
    if (TAF_SS_CUSD_TYPE_ENABLE == pstSsReqMsg->enCusdType)
    {
        TAF_SDC_UpdateRptCmdStatus(TAF_SDC_RPT_CMD_CUSD, VOS_TRUE);
    }
    else
    {
        TAF_SDC_UpdateRptCmdStatus(TAF_SDC_RPT_CMD_CUSD, VOS_FALSE);
    }

    /* check whether or not USSD string exists */
    if (0 == pstSsReqMsg->UssdStr.usCnt)
    {
        /* 如果只带CUSD是否主动上报参数，AT口已经释放，不需要等待C核回复 */
        return VOS_TRUE;
    }

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;
}


VOS_UINT32 TAF_SPM_RcvRegisterSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;
}
VOS_UINT32 TAF_SPM_RcvEraseSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvActivateSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvDeactivateSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvInterrogateSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvRegPwdSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvEraseCCentrySsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    
    TAF_PH_ERR_CODE                     enCause;

    /* 检查当前卡状态及开关机状态是否允许发起SS业务 */
    if (VOS_FALSE == TAF_SPM_IsSsServiceReqAllowed_PreProc(ulEventType, pstMsg, &enCause))
    {
        /* 如果不允许发起SS业务，给AT回复失败，带相应的原因值 */
        TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, enCause);
        
        return VOS_TRUE;
    }

    /* 允许发起SS业务时，要进行FDN&CALL CONTROL检查 */
    return VOS_FALSE;    
}
VOS_UINT32 TAF_SPM_RcvRleaseSsReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    
    ulRet               = VOS_FALSE;
    
#if (FEATURE_ON == FEATURE_IMS)
    /* 如果已经IMS域USSD业务存在，直接选择IMS域，其他情况返回VOS_FALSE，按NAS信令流程处理 */
    if (VOS_TRUE == TAF_SDC_GetImsSsSrvExistFlg())
    {
        ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
    }
#endif
    
    return ulRet;
}
VOS_UINT32 TAF_SPM_RcvStkOrigReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    MN_CALL_EMERGENCY_CAT_STRU          stEmergencyCat;
    MN_CALL_TYPE_ENUM_U8                enCallType; 
    VOS_UINT32                          ulExistBc;
    MN_APP_CALL_CALLORIG_REQ_STRU      *pstOrigParam = VOS_NULL_PTR;

    pstOrigParam = (MN_APP_CALL_CALLORIG_REQ_STRU *)pstMsg;

    PS_MEM_SET(&stEmergencyCat, 0, sizeof(MN_CALL_EMERGENCY_CAT_STRU));
    enCallType  = MN_CALL_TYPE_VOICE;
    ulExistBc   = VOS_TRUE;
    
    /* call is allowed if phone mode is power on */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_POWER_OFF);
        return VOS_TRUE;
    }

    if (0 == pstOrigParam->stBc.ucLen)
    {
        ulExistBc    = VOS_FALSE;
    }

    /* 根据BC参数获取呼叫类型 */
    ulRet = TAF_SPM_GetBcCallType(ulExistBc,
                                  pstOrigParam->stBc.ucLen,
                                  pstOrigParam->stBc.aucBc,
                                  &enCallType);
    if (VOS_TRUE != ulRet)
    {
        MN_WARN_LOG("MN_CALL_StkCallOrigReqProc: Fail to TAF_SPM_GetBcCallType.");
        
        TAF_CALL_SendCallOrigCnf(pstOrigParam->usClientId,
                                 pstOrigParam->opID,
                                 pstOrigParam->callID,
                                 TAF_CS_CAUSE_CALL_CTRL_BEYOND_CAPABILITY);
                                 
        MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL,
                                  0,
                                  TAF_CS_CAUSE_CALL_CTRL_BEYOND_CAPABILITY,
                                  pstOrigParam->callID);
        return VOS_TRUE;
    }
    
    /* VIDEO call当做普通呼叫处理不做紧急呼叫号码检查 */
    pstOrigParam->enCallType = enCallType;
    if (MN_CALL_TYPE_VIDEO != pstOrigParam->enCallType)
    {
        /* 判断是紧急呼更新紧急呼叫的CAT信息 */
        if (VOS_TRUE  == TAF_SPM_IsEmergencyNum((MN_CALL_CALLED_NUM_STRU *)&pstOrigParam->stCalledAddr,
                                                  VOS_FALSE,
                                                  &stEmergencyCat))
        {
            pstOrigParam->enCallType        = MN_CALL_TYPE_EMERGENCY;
            
            PS_MEM_CPY(&(pstOrigParam->stEmergencyCat),
                        &stEmergencyCat,
                        sizeof(MN_CALL_EMERGENCY_CAT_STRU));

            return VOS_FALSE;
        }
        else
        {
            PS_MEM_SET(&(pstOrigParam->stEmergencyCat),
                       0,
                       sizeof(MN_CALL_EMERGENCY_CAT_STRU));

        }
    }
    
    /* forbid normal call when USIM service is not available */
    if (VOS_FALSE == TAF_SPM_IsUsimServiceAvailable())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_SIM_NOT_EXIST);
        return VOS_TRUE;
    }
    
    /* 需要判断两个域卡无效,因为单域卡无效的情况能会disable LTE到GU下,需要到GU下继续尝试 */
    if (VOS_FALSE == TAF_SDC_IsUsimStausValid())
    {
        TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_SIM_INVALID);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 TAF_SPM_RcvMsgSmmaInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;

    /* 进行域选择 */
    enDomainSelRslt = TAF_SPM_ProcSmsDomainSelection();

    TAF_SPM_ProcSmmaIndDomainSelectionResult(ulEventType, pstMsg, enDomainSelRslt);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvMsgReportInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_MSG_REPORT_IND_STRU                            *pstMsgReportInd = VOS_NULL_PTR;
    TAF_SPM_ENTRY_MSG_STRU                                 *pstCacheMsg     = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    VOS_UINT32                                              ulRst;
    VOS_UINT8                                               ucImsSmsSndFlg;

    pstMsgReportInd = (TAF_SPM_MSG_REPORT_IND_STRU *)pstMsg;

    /* 清除IMS短信正在发送标志 */
    TAF_SPM_SetImsSmsSendingFlg(VOS_FALSE);

    if (VOS_TRUE == TAF_SDC_GetSmsRedailFromImsToCsSupportFlag())
    {
        /* 从短信重拨缓存中取出缓存消息 */
        pstCacheMsg = TAF_SPM_GetSmsRedialBufferWithClientId(&ucIndex, pstMsgReportInd->usClientId);
        
        if (VOS_NULL_PTR != pstCacheMsg)
        {
            if (VOS_TRUE == TAF_MSG_IsSmsRedialCauseValueFromImsDomain(pstMsgReportInd->stRptEvtInfo.enErrorCode))
            {
                if (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_SPM_SMMA_IND) == pstCacheMsg->ulEventType)
                {
                    /* SMMA消息，需要发送外部消息给MSG模块 */
                    TAF_SPM_SendMsgSmmaRsp(TAF_MSG_SIGNALLING_TYPE_NAS_SIGNALLING);
                }
                else
                {
                    /* 修改消息中的信令类型 */
                    TAF_SPM_ProcReqMsgBasedOnNasSignalling(pstCacheMsg->ulEventType, 
                                                           (struct MsgCB*)&(pstCacheMsg->aucEntryMsgBuffer[0]));
                    
                    /* 更新入口消息 */
                    TAF_SPM_UpdateServiceCtrlEntryMsg(pstCacheMsg->ulEventType,
                                                      (struct MsgCB*)&(pstCacheMsg->aucEntryMsgBuffer[0]));
                    
                    TAF_SPM_SetUpdateEntryMsgFlg(VOS_TRUE);
                }
                
                /* 清除重拨缓存 */
                TAF_SPM_FreeSpecificedIndexSmsRedialBuffer(ucIndex);
        
                /* 如果此时域选择缓存有消息存在，需要把消息转发给MSG模块 */
                if (0 != TAF_SPM_GetSmsMsgQueueNum())
                {
                    /* 发送内部域选择给SPM */
                    TAF_SPM_SndInternalDomainSelectionInd();
                }

                TAF_SPM_LogSrvDomainSelRedialInfo();
                
                return VOS_FALSE;
            }
        
            /* 清除重拨缓存 */
            TAF_SPM_FreeSpecificedIndexSmsRedialBuffer(ucIndex);

            TAF_SPM_LogSrvDomainSelRedialInfo();
        }
    }

    /* 不支持换域重拨，这儿上报短信发送报告 */
    TAF_SPM_ProcSmsRptEvent(pstMsgReportInd);

    /* 继续处理短信域选择queue */
    if (0 != TAF_SPM_GetSmsMsgQueueNum())
    {
        enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_BUTT;

        ulRst = TAF_SPM_ProcSmsMsgQueue(&enDomainSelRslt);

        /* check if domain selection type is buffer message */
        if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE != enDomainSelRslt)
        {
            /* 如果短信域选择缓存不为空，且当前无IMS短信在发送，继续处理短信域选择缓存 */
            ucImsSmsSndFlg = TAF_SPM_GetImsSmsSendingFlg();
            if ((0         != TAF_SPM_GetSmsMsgQueueNum())
             && (VOS_FALSE == ucImsSmsSndFlg))
            {
                TAF_SPM_SndInternalDomainSelectionInd();
            }

            /* log service domain selection infomation */
            TAF_SPM_LogSrvDomainSelQueueInfo();

            return ulRst;
        }
    }
    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallOrigCnf_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    TAF_SPM_ENTRY_MSG_STRU             *pstCacheInfo             = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));
    
    /* 只要MO成功时，才设置IMS域呼叫存在标识 */
    if (TAF_CS_CAUSE_SUCCESS == stCallInfo.enCause)
    {
        TAF_SDC_SetImsCallExistFlg(VOS_TRUE);
    }
    else
    {
        /* IMSA不填写呼叫方向，本地修改一下该项，为后续重拨判断使用 */
        stCallInfo.enCallDir = MN_CALL_DIR_MO;
        
        if (VOS_TRUE == TAF_SPM_IsCsCallRedialAllowed(&stCallInfo))
        {
            /* 获取呼叫重拨缓存消息 */
            pstCacheInfo = TAF_SPM_GetSpecificedIndexFromCallRedialBuffer(0);
        
            /* 更新入口消息 */
            TAF_SPM_UpdateServiceCtrlEntryMsg(pstCacheInfo->ulEventType,
                                              (struct MsgCB*)&(pstCacheInfo->aucEntryMsgBuffer[0]));
        
            TAF_SPM_SetUpdateEntryMsgFlg(VOS_TRUE);
        
            /* 清除重拨缓存 */
            TAF_SPM_FreeSpecificedIndexCallRedialBuffer(0);
        
            return VOS_FALSE;
        }

        /* 清除重拨缓存 */
        TAF_SPM_FreeSpecificedIndexCallRedialBuffer(0);
    }

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_CALL_ORIG_CNF消息，给相应的cliendId回复 */
    MN_SendClientEvent(stCallInfo.clientId,
                        MN_CALLBACK_CS_CALL,
                        MN_CALL_EVT_CALL_ORIG_CNF,
                        &stCallInfo);

    /* 构造Err_Ind消息，cliendId为广播类型 */
    MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL, 0, stCallInfo.enCause, 0);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallOrig_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_ORIG消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_ORIG,
                       &stCallInfo);

    /* 更新重拨缓存消息的call ID，方便消息在释放时查找 */
    TAF_SPM_UpdateCallRedialBufferMsgWithCallId(stCallInfo.clientId, stCallInfo.callId);
    
    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallProc_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_CALL_PROC消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_CALL_PROC,
                       &stCallInfo);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallAlerting_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_ALERTING消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_ALERTING,
                       &stCallInfo);

    /* 清除重拨缓存 */
    TAF_SPM_FreeCallRedialBufferWithCallId(pstImsaCallMsg->stCallInfo.callId);
    
    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallConnect_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_CONNECT消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_CONNECT,
                       &stCallInfo);

    /* 清除重拨缓存 */
    TAF_SPM_FreeCallRedialBufferWithCallId(pstImsaCallMsg->stCallInfo.callId);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallSupsCmdCnf_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_SUPS_CMD_CNF消息，给相应的cliendId回复 */
    MN_SendClientEvent(stCallInfo.clientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_SUPS_CMD_CNF,
                       &stCallInfo);


    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallRelease_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    TAF_SPM_ENTRY_MSG_STRU             *pstCacheInfo             = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    if (VOS_TRUE == TAF_SPM_IsCsCallRedialAllowed(&stCallInfo))
    {
        /* 获取呼叫重拨缓存消息 */
        pstCacheInfo = TAF_SPM_GetCallRedialBufferWithCallId(stCallInfo.callId);
        if (VOS_NULL_PTR != pstCacheInfo)
        {
            /* 更新入口消息 */
            TAF_SPM_UpdateServiceCtrlEntryMsg(pstCacheInfo->ulEventType,
                                              (struct MsgCB*)&(pstCacheInfo->aucEntryMsgBuffer[0]));
            
            TAF_SPM_SetUpdateEntryMsgFlg(VOS_TRUE);
            
            /* 清除重拨缓存 */
            TAF_SPM_FreeCallRedialBufferWithCallId(stCallInfo.callId);
            
            return VOS_FALSE;
        }
    }

    /* 清除重拨缓存 */
    TAF_SPM_FreeCallRedialBufferWithCallId(stCallInfo.callId);

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_RELEASED消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_RELEASED,
                       &stCallInfo);

    /* 构造Err_Ind消息,不受原来CS域NV项的控制 */
    MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL, 0, stCallInfo.enCause, 0);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallIncoming_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 设置IMS域呼叫存在标识 */
    TAF_SDC_SetImsCallExistFlg(VOS_TRUE);

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_INCOMING消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_INCOMING,
                       &stCallInfo);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallStartDtmfCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_START_DTMF_CNF_STRU  *pstStartDtmfCnfMsg       = VOS_NULL_PTR;
    TAF_CALL_EVT_DTMF_CNF_STRU          stDtmfCnf;    

    pstStartDtmfCnfMsg      = (IMSA_SPM_CALL_START_DTMF_CNF_STRU *)pstMsg;
    
    PS_MEM_SET(&stDtmfCnf, 0, sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));
    
    stDtmfCnf.callId        = pstStartDtmfCnfMsg->ucCallId;
    stDtmfCnf.usClientId    = pstStartDtmfCnfMsg->usClientId;
    stDtmfCnf.opId          = pstStartDtmfCnfMsg->ucOpId;
    stDtmfCnf.enCause       = pstStartDtmfCnfMsg->enCause;
    stDtmfCnf.enDtmfState   = pstStartDtmfCnfMsg->enDtmfState;
    stDtmfCnf.ucDtmfCnt     = pstStartDtmfCnfMsg->ucDtmfCnt;

    
    TAF_CALL_SendMsg(pstStartDtmfCnfMsg->usClientId,
                     MN_CALL_EVT_START_DTMF_CNF,
                     &stDtmfCnf,
                     sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallStartDtmfRsltInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_START_DTMF_RSLT_IND_STRU                 *pstDtmfRsltIndMsg       = VOS_NULL_PTR;
    TAF_CALL_EVT_DTMF_CNF_STRU                              stDtmfCnf;

    pstDtmfRsltIndMsg      = (IMSA_SPM_CALL_START_DTMF_RSLT_IND_STRU *)pstMsg;
    
    PS_MEM_SET(&stDtmfCnf, 0, sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));
    
    stDtmfCnf.callId        = pstDtmfRsltIndMsg->ucCallId;
    stDtmfCnf.usClientId    = pstDtmfRsltIndMsg->usClientId;
    stDtmfCnf.opId          = pstDtmfRsltIndMsg->ucOpId;
    stDtmfCnf.enCause       = pstDtmfRsltIndMsg->enCause;
    stDtmfCnf.enDtmfState   = pstDtmfRsltIndMsg->enDtmfState;
    stDtmfCnf.ucDtmfCnt     = pstDtmfRsltIndMsg->ucDtmfCnt;

    
    TAF_CALL_SendMsg(pstDtmfRsltIndMsg->usClientId,
                     MN_CALL_EVT_START_DTMF_RSLT,
                     &stDtmfCnf,
                     sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));

    return VOS_TRUE;
}



VOS_UINT32 TAF_SPM_RcvImsaCallStopDtmfCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_STOP_DTMF_CNF_STRU   *pstStopDtmfCnfMsg       = VOS_NULL_PTR;
    TAF_CALL_EVT_DTMF_CNF_STRU          stDtmfCnf;    

    pstStopDtmfCnfMsg      = (IMSA_SPM_CALL_STOP_DTMF_CNF_STRU *)pstMsg;
    
    PS_MEM_SET(&stDtmfCnf, 0, sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));
    
    stDtmfCnf.callId       = pstStopDtmfCnfMsg->ucCallId;
    stDtmfCnf.usClientId   = pstStopDtmfCnfMsg->usClientId;
    stDtmfCnf.opId         = pstStopDtmfCnfMsg->ucOpId;
    stDtmfCnf.enCause      = pstStopDtmfCnfMsg->enCause;
    stDtmfCnf.enDtmfState  = pstStopDtmfCnfMsg->enDtmfState;
    stDtmfCnf.ucDtmfCnt    = pstStopDtmfCnfMsg->ucDtmfCnt;

    
    TAF_CALL_SendMsg(pstStopDtmfCnfMsg->usClientId,
                     MN_CALL_EVT_STOP_DTMF_CNF,
                     &stDtmfCnf,
                     sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallStopDtmfRsltInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_STOP_DTMF_RSLT_IND_STRU                  *pstDtmfRsltIndMsg       = VOS_NULL_PTR;
    TAF_CALL_EVT_DTMF_CNF_STRU                              stDtmfCnf;

    pstDtmfRsltIndMsg      = (IMSA_SPM_CALL_STOP_DTMF_RSLT_IND_STRU *)pstMsg;
    
    PS_MEM_SET(&stDtmfCnf, 0, sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));
    
    stDtmfCnf.callId        = pstDtmfRsltIndMsg->ucCallId;
    stDtmfCnf.usClientId    = pstDtmfRsltIndMsg->usClientId;
    stDtmfCnf.opId          = pstDtmfRsltIndMsg->ucOpId;
    stDtmfCnf.enCause       = pstDtmfRsltIndMsg->enCause;
    stDtmfCnf.enDtmfState   = pstDtmfRsltIndMsg->enDtmfState;
    stDtmfCnf.ucDtmfCnt     = pstDtmfRsltIndMsg->ucDtmfCnt;

    
    TAF_CALL_SendMsg(pstDtmfRsltIndMsg->usClientId,
                     MN_CALL_EVT_STOP_DTMF_RSLT,
                     &stDtmfCnf,
                     sizeof(TAF_CALL_EVT_DTMF_CNF_STRU));

    return VOS_TRUE;
}



VOS_UINT32 TAF_SPM_RcvImsaCallSsCmdRslt_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg      = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_SS_CMD_RSLT消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_SS_CMD_RSLT,
                       &stCallInfo);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallSsNotify_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_SS_NOTIFY消息，给相应的cliendId回复 */
    MN_SendClientEvent(pstImsaCallMsg->usClientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_SS_NOTIFY,
                       &stCallInfo);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallAllRelease_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg           = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl           = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl    = VOS_NULL_PTR;
    MN_CALL_INFO_STRU                   stCallInfo;

    /* 设置IMS域呼叫存在标识 */
    TAF_SDC_SetImsCallExistFlg(VOS_FALSE);

    /* 如果无重拨在GU下发起，需要上报call all release事件给应用 */
    if (VOS_FALSE == TAF_SDC_GetCsCallExistFlg())
    {
        pstImsaCallMsg          = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

        pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
        pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

        PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

        PS_MEM_CPY(&stCallInfo, &(pstImsaCallMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));
    
        /* 增加主动上报相关全局变量的值 */
        PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                    pstCurcRptCtrl->aucRptCfg,
                    MN_CALL_RPT_CFG_MAX_SIZE);
        
        PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                    pstUnsolicitedRptCtrl->aucRptCfg,
                    MN_CALL_RPT_CFG_MAX_SIZE);
        
        /* 构造一条MN_CALL_EVT_ALL_RELEASED消息，给相应的cliendId回复 */
        MN_SendClientEvent(pstImsaCallMsg->usClientId,
                           MN_CALLBACK_CS_CALL,
                           MN_CALL_EVT_ALL_RELEASED,
                           &stCallInfo);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallMsg_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRetFlg;
    TAF_SPM_PROC_IMSA_CALL_MSG_STRU    *pstTabHeader    = VOS_NULL_PTR;
    IMSA_SPM_CALL_MSG_STRU             *pstImsaCallMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulTabSize;

    pstImsaCallMsg      = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    /* 获取处理函数的表的首地址 */
    pstTabHeader = TAF_SPM_GetImsaCallMsgTabAddr();

    ulRetFlg     = VOS_TRUE;
    ulTabSize    = TAF_SPM_GetImsaCallMsgTabSize();

    /* 根据EventType查表 */
    for (i = 0; i < ulTabSize; i++)
    {
        if (pstImsaCallMsg->enEventType == pstTabHeader->ulMsgEvtType)
        {
            ulRetFlg = pstTabHeader->pfMsgFun(pstMsg);

            break;
        }

        pstTabHeader++;
    }

    return ulRetFlg;
}



VOS_UINT32 TAF_SPM_RcvImsaGetCallInfoCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_GET_CALL_INFO_CNF_STRU                   *pstImsaMsg  = VOS_NULL_PTR;

    pstImsaMsg = (IMSA_SPM_CALL_GET_CALL_INFO_CNF_STRU *)pstMsg;

    if (MN_CALL_MAX_NUM < pstImsaMsg->ucNumOfCalls)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_RcvImsaGetCallInfoCnf_PreProc: call numbers is abnormal!");

        return VOS_TRUE;
    }

    if (CALL_IMSA_GET_CALL_INFO_REQ_TYPE_ATA == pstImsaMsg->enReqType)
    {
        /* 请求类型是ATA时调用TAFAGENT API查询呼叫信息,给TAFAGENT回复 */
        TAF_SPM_SendTafAgentGetCallInfoCnf(pstImsaMsg);
    }
    else
    {
        /* 请求类型是CLCC查询呼叫信息,给AT回复 */
        TAF_SPM_SendAtGetCallInfoCnf(pstImsaMsg);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaChannelInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CHANNEL_INFO_IND_STRU     *pstImsaMsg  = VOS_NULL_PTR;
    MN_CALL_EVT_CHANNEL_INFO_STRU       stChannelEvt;

    pstImsaMsg  = (IMSA_SPM_CHANNEL_INFO_IND_STRU *)pstMsg;

    /* 构造一条消息，给AT回复 */
    PS_MEM_SET(&stChannelEvt, 0x00, sizeof(MN_CALL_EVT_CHANNEL_INFO_STRU));

    stChannelEvt.enEvent                = MN_CALL_EVT_CHANNEL_INFO_IND;
    stChannelEvt.usClientId             = MN_GetRealClientId(pstImsaMsg->usClientId, WUEPS_PID_TAF);
    stChannelEvt.ucIsLocalAlertingFlag  = pstImsaMsg->ucIsLocalAlertingFlag;
    stChannelEvt.enVoiceDomain          = TAF_CALL_VOICE_DOMAIN_IMS;
    stChannelEvt.enCodecType            = pstImsaMsg->enCodecType;

    MN_SendReportMsg(MN_CALLBACK_CS_CALL, (VOS_UINT8 *)&stChannelEvt, sizeof(MN_CALL_EVT_CHANNEL_INFO_STRU));

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaGetClprCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_GET_CLPR_CNF_STRU    *pstImsaMsg  = VOS_NULL_PTR;
    MN_CALL_CLPR_GET_CNF_STRU           stClprCnf;

    pstImsaMsg  = (IMSA_SPM_CALL_GET_CLPR_CNF_STRU *)pstMsg;

    /* 构造一条消息，给AT回复 */
    PS_MEM_SET(&stClprCnf, 0x00, sizeof(MN_CALL_CLPR_GET_CNF_STRU));

    stClprCnf.enEvent                   = MN_CALL_EVT_CLPR_SET_CNF;
    stClprCnf.stAppCtrl.usClientId      = pstImsaMsg->usClientId;
    stClprCnf.ulRet                     = pstImsaMsg->ulRet;
    stClprCnf.stRedirectInfo            = pstImsaMsg->stRedirectInfo;

    MN_SendReportMsg(MN_CALLBACK_CS_CALL, (VOS_UINT8 *)&stClprCnf, sizeof(MN_CALL_CLPR_GET_CNF_STRU));

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaCallHold_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaMsg              = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    MN_CALL_EVT_HOLD_STRU               stEvent;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();


    pstImsaMsg  = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    PS_MEM_SET(&stEvent, 0, sizeof(MN_CALL_EVT_HOLD_STRU));
    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    stEvent.enEvent         = MN_CALL_EVT_HOLD;
    stEvent.usClientId      = MN_GetRealClientId(pstImsaMsg->usClientId, WUEPS_PID_TAF);
    stEvent.ucCallNum       = 1;
    stEvent.aucCallId[0]    = pstImsaMsg->stCallInfo.callId;
    stEvent.enVoiceDomain   = pstImsaMsg->stCallInfo.enVoiceDomain;

    PS_MEM_CPY(stEvent.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                sizeof(stEvent.aucCurcRptCfg));
    PS_MEM_CPY(stEvent.aucUnsolicitedRptCfg,
            pstUnsolicitedRptCtrl->aucRptCfg,
            sizeof(stEvent.aucUnsolicitedRptCfg));

    /* 将HOLD事件发送到AT */
    if (VOS_TRUE == TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_CALLSTATE))
    {
        MN_SendReportMsg(MN_CALLBACK_CS_CALL, (VOS_UINT8 *)&stEvent, sizeof(MN_CALL_EVT_HOLD_STRU));
    }

    /* 将HOLD事件发送到OAM */
    PS_MEM_CPY(&stCallInfo, &(pstImsaMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    MN_CS_SendMsgToOam(MN_CLIENT_ALL,
                       MN_CALL_EVT_HOLD,
                       MAPS_STK_PID,
                       &stCallInfo);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallRetrieve_PreProc(
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_MSG_STRU             *pstImsaMsg              = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl          = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    MN_CALL_EVT_RETRIEVE_STRU           stEvent;
    MN_CALL_INFO_STRU                   stCallInfo;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    pstImsaMsg  = (IMSA_SPM_CALL_MSG_STRU *)pstMsg;

    PS_MEM_SET(&stEvent, 0, sizeof(MN_CALL_EVT_RETRIEVE_STRU));
    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));

    stEvent.enEvent         = MN_CALL_EVT_RETRIEVE;
    stEvent.usClientId      = MN_GetRealClientId(pstImsaMsg->usClientId, WUEPS_PID_TAF);
    stEvent.ucCallNum       = 1;
    stEvent.aucCallId[0]    = pstImsaMsg->stCallInfo.callId;
    stEvent.enVoiceDomain   = pstImsaMsg->stCallInfo.enVoiceDomain;

    PS_MEM_CPY(stEvent.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                sizeof(stEvent.aucCurcRptCfg));
    PS_MEM_CPY(stEvent.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                sizeof(stEvent.aucUnsolicitedRptCfg));

    /* 将RETRIEVE事件发送到AT */
    if (VOS_TRUE == TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_CALLSTATE))
    {
        MN_SendReportMsg(MN_CALLBACK_CS_CALL, (VOS_UINT8 *)&stEvent, sizeof(MN_CALL_EVT_RETRIEVE_STRU));
    }

    /* 将RETRIEVE事件发送到OAM */
    PS_MEM_CPY(&stCallInfo, &(pstImsaMsg->stCallInfo), sizeof(MN_CALL_INFO_STRU));

    MN_CS_SendMsgToOam(MN_CLIENT_ALL,
                       MN_CALL_EVT_RETRIEVE,
                       MAPS_STK_PID,
                       &stCallInfo);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvImsaSsMsg_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstRcvMsg
)
{
    IMSA_SPM_SS_MSG_STRU               *pstImsaMsg = VOS_NULL_PTR;


    pstImsaMsg = (IMSA_SPM_SS_MSG_STRU *)pstRcvMsg;

    MN_SendClientEvent(pstImsaMsg->usClientId,
                       MN_CALLBACK_SS,
                       (VOS_UINT32)(pstImsaMsg->stSsEventInfo.SsEvent),
                       (VOS_VOID *)&(pstImsaMsg->stSsEventInfo));

    switch(pstImsaMsg->stSsEventInfo.SsEvent)
    {
        /* 网络主动发起的SS业务需要置上标志 */
        case TAF_SS_EVT_REGISTERSS_CNF:
        case TAF_SS_EVT_USS_NOTIFY_IND:
        
            TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

            break;
            
        /* 以下事件需要清除标志 */
        case TAF_SS_EVT_ERROR:
        case TAF_SS_EVT_ERASESS_CNF:
        case TAF_SS_EVT_ACTIVATESS_CNF:
        case TAF_SS_EVT_DEACTIVATESS_CNF:
        case TAF_SS_EVT_INTERROGATESS_CNF:
        case TAF_SS_EVT_PROCESS_USS_REQ_CNF:
        case TAF_SS_EVT_USS_RELEASE_COMPLETE_IND:
        case TAF_SS_EVT_ERASE_CC_ENTRY_CNF:
        case TAF_SS_EVT_PROBLEM:
        
            TAF_SDC_SetImsSsSrvExistFlg(VOS_FALSE);
        
            break;
            
        /* 其他事件时不需要设置/清除标志 */
        default:

            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvImsaNormalRegStatusNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRst;

    /* 处理缓存的消息 */
    ulRst = TAF_SPM_ProcImsaNormalRegStatusNotify(pstMsg);

    return ulRst;
}
VOS_UINT32 TAF_SPM_RcvInternalDomainSelInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRst;

    /* 处理缓存的消息 */
    ulRst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRst;
}
VOS_UINT32 TAF_SPM_RcvMmaServiceStatusChangeNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRlst;
    TAF_SPM_SERVICE_STATUS_ENUM_UINT8   enLastPsStatus;
    TAF_SPM_SERVICE_STATUS_ENUM_UINT8   enCurrPsStatus;
    
    /* 获取上次PS服务状态 */
    enLastPsStatus = TAF_SPM_GetLastPsServiceStatus();

    /* 更新当前的PS服务状态到SPM CONTEXT中 */
    enCurrPsStatus = (TAF_SPM_SERVICE_STATUS_ENUM_UINT8)TAF_SDC_GetPsServiceStatus();   
    if (TAF_SPM_SERVICE_STATUS_BUTT < enCurrPsStatus)
    {
        return  VOS_FALSE;
    }
    TAF_SPM_SetLastPsServiceStatus(enCurrPsStatus);

    if (VOS_TRUE == TAF_SPM_IsNeedtoWaitImsRegStatus(enLastPsStatus))
    {
        /* 等待新IMS注册结果 */
        return VOS_TRUE;
    }  

    /* 处理缓存的消息 */
    ulRlst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRlst;

}


VOS_UINT32 TAF_SPM_RcvMmaNetworkCapabilityChangeNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRlst;
    
    /* 处理缓存的消息 */
    ulRlst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRlst;

}
VOS_UINT32 TAF_SPM_RcvMmaRatChangeNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRlst;

    /* 处理缓存的消息，和网络能力变化时的处理一致 */
    ulRlst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRlst;
}
VOS_UINT32 TAF_SPM_RcvMmaImsVoiceCapInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRlst;

    /* 处理缓存的消息，和网络能力变化时的处理一致 */
    ulRlst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRlst;
}
VOS_UINT32 TAF_SPM_RcvCcSrvReqProtectTimerExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRst;

    /* 处理缓存的消息 */
    ulRst = TAF_SPM_ProcCcSrvReqProtectTimerExpired();

    return ulRst;
}
VOS_UINT32 TAF_SPM_RcvSmsSrvReqProtectTimerExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRst;

    /* 处理缓存的消息 */
    ulRst = TAF_SPM_ProcSmsSrvReqProtectTimerExpired(pstMsg);

    return ulRst;
}
VOS_UINT32 TAF_SPM_RcvSsSrvReqProtectTimerExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRst;

    /* 处理缓存的消息 */
    ulRst = TAF_SPM_ProcSsSrvReqProtectTimerExpired();

    return ulRst;
}
VOS_UINT32 TAF_SPM_RcvImsaCallMsgSyncInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucNum;
    VOS_UINT8                           i;    

    /* 呼叫重拨未打开，直接返回 */
    if (VOS_TRUE == TAF_SDC_GetCallRedailFromImsToCsSupportFlag())
    {
        /* 由于SRVCC过程前的呼叫请求，走换域重拨流程，IMSA保证在该消息前先发给SPM,
         * 因此，如果收到该消息时，重拨缓存中仍然有消息，它一定是在SRVCC过程中收到，
         * 可以清除呼叫重拨缓存，呼叫请求消息由call模块负责发起
         */    
        ucNum = TAF_SPM_GetNumberOfCallRedialBuffer();

        for (i = 0; i < ucNum; i++)
        {
            TAF_SPM_FreeSpecificedIndexCallRedialBuffer(i);
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SPM_RcvImsaCallInviteNewPtptCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_SPM_CALL_INVITE_NEW_PTPT_CNF_STRU                 *pstNewPtptCnf         = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU                             *pstCurcRptCtrl        = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU                      *pstUnsolicitedRptCtrl = VOS_NULL_PTR;    
    MN_CALL_INFO_STRU                                       stCallInfo;    

    pstNewPtptCnf = (IMSA_SPM_CALL_INVITE_NEW_PTPT_CNF_STRU *)pstMsg;

    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stCallInfo, 0, sizeof(MN_CALL_INFO_STRU));    

    stCallInfo.clientId = pstNewPtptCnf->usClientId;
    stCallInfo.opId     = pstNewPtptCnf->ucOpId;
    stCallInfo.enCause  = pstNewPtptCnf->enCause;
    
    /* 增加主动上报相关全局变量的值 */
    PS_MEM_CPY(stCallInfo.aucCurcRptCfg,
                pstCurcRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    PS_MEM_CPY(stCallInfo.aucUnsolicitedRptCfg,
                pstUnsolicitedRptCtrl->aucRptCfg,
                MN_CALL_RPT_CFG_MAX_SIZE);

    /* 构造一条MN_CALL_EVT_CALL_ORIG_CNF消息，给相应的cliendId回复 */
    MN_SendClientEvent(stCallInfo.clientId,
                       MN_CALLBACK_CS_CALL,
                       MN_CALL_EVT_CALL_ORIG_CNF,
                       &stCallInfo);

    /* 构造Err_Ind消息，cliendId为广播类型 */
    MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL, 0, stCallInfo.enCause, 0);

    return VOS_TRUE;    
}
#endif
VOS_UINT32 TAF_SPM_RcvMmaPowerOffInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

#if (FEATURE_ON == FEATURE_IMS)
    TAF_SPM_ProcMmaPowerOffInd();
#endif    

    /* return VOS_FASLE， APS also need this message */
    return VOS_FALSE;
}
VOS_UINT32 TAF_SPM_IsSsServiceReqAllowed_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_PH_ERR_CODE                    *penCause
)
{
    VOS_UINT32                          ulIsUsimValid;


    *penCause   = TAF_ERR_NO_ERROR;
    
    ulIsUsimValid   = TAF_SDC_IsUsimStausValid();
    
    /* SS is allowed if phone mode is power on */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        *penCause   = TAF_ERR_SS_POWER_OFF;
        return VOS_FALSE;
    }

    /* forbid SS when USIM service is not available */
    /* 需要判断两个域卡无效,因为单域卡无效的情况能会disable LTE到GU下,需要到GU下继续尝试 */
    if ( (VOS_FALSE == TAF_SPM_IsUsimServiceAvailable())
      || (VOS_FALSE == ulIsUsimValid) )
    {
        *penCause   = TAF_ERR_USIM_SIM_INVALIDATION;
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_IsSmsServiceReqAllowed_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    VOS_UINT32                         *pulCause
)
{
    *pulCause   = MN_ERR_NO_ERROR;
        
    /* SMS is allowed if phone mode is power on */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        *pulCause    = MN_ERR_CLASS_SMS_POWER_OFF;
        return VOS_FALSE;
    }

    /* forbid SMS when USIM service is not available */
    if (VOS_FALSE == TAF_SPM_IsUsimServiceAvailable())
    {
        *pulCause    = MN_ERR_CLASS_SMS_NOUSIM;
        return VOS_FALSE;
    }

    /* 判断两个域卡无效时，直接给AT回复失败 */
    if (VOS_FALSE == TAF_SDC_IsUsimStausValid())
    {
        *pulCause    = MN_ERR_CLASS_SMS_UNAVAILABLE;
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


