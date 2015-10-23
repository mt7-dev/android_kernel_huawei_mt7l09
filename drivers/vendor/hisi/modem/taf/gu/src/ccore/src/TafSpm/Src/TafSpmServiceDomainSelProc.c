

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "PsTypeDef.h"
#include "VosPidDef.h"
#include "v_msg.h"
#include "TafLog.h"
#include "TafSpmServiceDomainSelProc.h"
#include "MnMsgApi.h"
#include "AtMnInterface.h"
#if (FEATURE_IMS == FEATURE_ON)
#include "SpmImsaInterface.h"
#endif
#include "TafSpmSndImsa.h"
#include "TafFsm.h"
#include "MnCallMgmt.h"
#include "MnCall.h"
#include "TafSdcLib.h"
#include "TafSdcCtx.h"
#include "MnCallReqProc.h"
#include "TafSpmSndInternalMsg.h"
#include "MnErrorCode.h"
#include "Ssa_Define.h"
#include "TafSpmMntn.h"
#include "TafMmaCtx.h"

#include "TafSpmRedial.h"
#include "TafSpmComFunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_SPM_SERVICE_DOMAIN_SEL_PROC_C

#if (FEATURE_IMS == FEATURE_ON)
/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    TAF_SPM_GET_IMS_DOMAIN_SEL_MSG_TAB_SIZE  (sizeof(g_astTafSpmImsDomainSelMsgTbl)/sizeof(TAF_SPM_PROC_MSG_STRU))

/*****************************************************************************
  3 类型定义
*****************************************************************************/
typedef VOS_UINT32 (*TAF_SPM_PROC_MSG_FUNC)(struct MsgCB *pstMsg);

typedef struct
{
    VOS_UINT32                          ulMsgEvtType;
    TAF_SPM_PROC_MSG_FUNC               pfMsgFun;
} TAF_SPM_PROC_MSG_STRU;
/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/
TAF_SPM_PROC_MSG_STRU g_astTafSpmImsDomainSelMsgTbl[]=
{
    /* AT CALL message */
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_ORIG_REQ,               TAF_SPM_RcvAppOrigReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_SUPS_CMD_REQ,           TAF_SPM_RcvAppSupsCmdReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_GET_INFO_REQ,           TAF_SPM_RcvAppGetInfoReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_START_DTMF_REQ,         TAF_SPM_RcvAppStartDtmfReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_STOP_DTMF_REQ,          TAF_SPM_RcvAppStopDtmfReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_GET_CDUR_REQ,           TAF_SPM_RcvAppGetCdurReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  ID_TAFAGENT_MN_GET_CALL_INFO_REQ,   TAF_SPM_RcvAppGetCallInfoReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_CALL_APP_CLPR_GET_REQ,           TAF_SPM_RcvAppGetClprReqBasedOnCsOverIp),

    /*  AT SS message */
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  TAF_MSG_PROCESS_USS_MSG,            TAF_SPM_RcvProcUssSsReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  TAF_MSG_RLEASE_MSG,                 TAF_SPM_RcvRleaseSsReqBasedOnCsOverIp),

    /* AT SMS message */
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_MSG_MSGTYPE_SEND_RPDATA_DIRECT,  TAF_SPM_RcvSendRpdataDirectBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(WUEPS_PID_AT,  MN_MSG_MSGTYPE_SEND_RPDATA_FROMMEM, TAF_SPM_RcvSendRpdataFromMemBasedOnCsOverIp),

    /* STK CALL message */
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_CALL_CALLORIG_REQ,              TAF_SPM_RcvStkOrigReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_CALL_SUPS_CMD_REQ,              TAF_SPM_RcvStkSupsCmdReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_CALL_START_DTMF_REQ,            TAF_SPM_RcvStkStartDtmfReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_CALL_STOP_DTMF_REQ,             TAF_SPM_RcvStkStopDtmfReqBasedOnCsOverIp),

    /* STK SS message */
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_SS_USSD_REQ,                    TAF_SPM_RcvProcUssSsReqBasedOnCsOverIp),
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_SS_REGISTERSS_REQ,              TAF_SPM_RcvRegisterSsReqBasedOnCsOverIp),

    /* STK SMS message */
    TAF_ACT_TBL_ITEM(MAPS_STK_PID,  STK_MSG_SEND_REQ,                   TAF_SPM_RcvSendRpdataDirectBasedOnCsOverIp)
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

TAF_SPM_PROC_MSG_STRU *TAF_SPM_GetImsDomainSelMsgTabAddr(VOS_VOID)
{
    return (&g_astTafSpmImsDomainSelMsgTbl[0]);
}

/* TAF_SPM_GetCallType删除 */



#if ( FEATURE_ON == FEATURE_LTE )
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcEmergencyCallDomainSelectionInCsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /*  network support IMS voice */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
        /*  IMS is in registered status, select CS over IP */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
        }
        /* select buffer message if IMS is registering */
        else if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
        }
        else
        {
            /* nothing to do */
        }        
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcEmergencyCallDomainSelectionInImsPsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /*  IMS is in registered status and network support IMS voice, select
     *  CS over IP
     */
    if ((TAF_SDC_IMS_NORMAL_REG_STATUS_REG     == TAF_SDC_GetImsNormalRegStatus())
     && (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap))    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /*  network support IMS voice */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
         /* select buffer message if IMS is registering */
         if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())         
         {
             return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
         }
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcEmergencyCallDomainSelectionInImsPsVoiceOnlyOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* network supports IMS vocie */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
        /* select CS over IP if IMS is in registered status */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
        }

        /* select buffer message if IMS is registering */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
        }
    }

    /* service request is failure if IMS voice is not supported by network or IMS is in
     * deregistering or deregistered status.
     */
    return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcNormalCallDomainSelectionInCsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucRoamSupport;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }    

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /*  network support IMS voice */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
        /*  IMS is in registered status, select CS over IP */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
        }
        /* select buffer message if IMS is registering */
        else if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
        }
        else
        {
            /* nothing to do */
        }
    }

    /* check UE's usage setting if IMS voice is not supported by network or IMS is in
     * deregistering or deregistered status. If it is data centric, select nas signalling.
     * Or buffer message is selected.
     */
    if (TAF_SDC_UE_USAGE_DATA_CENTRIC == TAF_SDC_GetUeUsageSetting())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcNormalCallDomainSelectionInImsPsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucRoamSupport;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }    

    /*  IMS is in registered status and network support IMS voice, select
     *  CS over IP
     */
    if ((TAF_SDC_IMS_NORMAL_REG_STATUS_REG     == TAF_SDC_GetImsNormalRegStatus())
     && (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /*  network support IMS voice */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
         /* select buffer message if IMS is registering */
         if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())         
         {
             return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
         }
    }

    /* check UE's usage setting if IMS voice is not supported by network or IMS is in
     * deregistering or deregistered status. If it is data centric, select nas signalling.
     * Or buffer message is selected.
     */
    if (TAF_SDC_UE_USAGE_DATA_CENTRIC == TAF_SDC_GetUeUsageSetting())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcNormalCallDomainSelectionInImsPsVoiceOnlyOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucRoamSupport;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
    }    

    /* network supports IMS vocie */
    if (TAF_SDC_NW_IMS_VOICE_SUPPORTED == pstLteNwCapInfo->enNwImsVoCap)
    {
        /* select CS over IP if IMS is in registered status */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
        }

        /* select buffer message if IMS is registering */
        if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())        
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
        }
    }

    /* service request is failure if IMS voice is not supported by network or IMS is in
     * deregistering or deregistered status.
     */
    return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcNormalCallDomainSelectionOnEutran(VOS_VOID)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32                        enVoicePerferred;  

    enDomainSelRslt  = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;

    enVoicePerferred = TAF_SDC_GetVoiceDomain();
    switch (enVoicePerferred)
    {
        /* domain selection in CS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_CS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcNormalCallDomainSelectionInCsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcNormalCallDomainSelectionInImsPsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice only */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY:
            enDomainSelRslt = TAF_SPM_ProcNormalCallDomainSelectionInImsPsVoiceOnlyOnEutran();
            break;

        /* domain selection in CS voice only */
        case TAF_SDC_VOICE_DOMAIN_CS_ONLY:
        default:
            break;
    }

    return (enDomainSelRslt);
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcEmergencyCallDomainSelectionOnEutran(VOS_VOID)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    TAF_SDC_NETWORK_CAP_INFO_STRU                          *pstNwCapInfo = VOS_NULL_PTR;
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32                        enVoicePerferred;

    enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;

    pstNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* nas signalling is selected if PS service status is not nomal service */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* nas signalling is selected if UE or network don't support EMS */
    if ((VOS_FALSE                       == TAF_SDC_GetLteEmsSupportFlag())
     || (TAF_SDC_NW_EMC_BS_NOT_SUPPORTED == pstNwCapInfo->enNwEmcBsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    enVoicePerferred = TAF_SDC_GetVoiceDomain();
    switch (enVoicePerferred)
    {
        /* domain selection in CS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_CS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcEmergencyCallDomainSelectionInCsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcEmergencyCallDomainSelectionInImsPsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice only */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY:
            enDomainSelRslt = TAF_SPM_ProcEmergencyCallDomainSelectionInImsPsVoiceOnlyOnEutran();
            break;

        /* domain selection in CS voice only */
        case TAF_SDC_VOICE_DOMAIN_CS_ONLY:
        default:
            break;
    }

    /* select CS call */
    return (enDomainSelRslt);
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcCcDomainSelectionOnEutran(
    struct MsgCB                       *pstMsg,
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;

    enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;

    if (VOS_FALSE == TAF_SDC_GetLteImsSupportFlag())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }  

    switch (enCallType)
    {
        /* emergency call */
        case MN_CALL_TYPE_EMERGENCY:
            enDomainSelRslt = TAF_SPM_ProcEmergencyCallDomainSelectionOnEutran();
            break;

        /* video call */
        case MN_CALL_TYPE_VIDEO:
            if (VOS_TRUE == TAF_SDC_GetVideoCallOnImsSupportFlag())
            {
                enDomainSelRslt = TAF_SPM_ProcNormalCallDomainSelectionOnEutran();
            }
            break;

        /* voice call */
        case MN_CALL_TYPE_VOICE:
            enDomainSelRslt = TAF_SPM_ProcNormalCallDomainSelectionOnEutran();
            break;

        /* if call type is butt, return request fail */
        case MN_CALL_TYPE_BUTT:
            enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
            break;

        /* other */
        case MN_CALL_TYPE_FAX:
        case MN_CALL_TYPE_CS_DATA:
        default:
            break;
    }

    return enDomainSelRslt;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelectionInCsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucRoamSupport;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }    

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /*  IMS is in registered status, select CS over IP */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* select buffer message if IMS is registering */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* check UE's usage setting if IMS is in deregistering or deregistered status.
     * If it is data centric, select nas signalling. Or buffer message is selected.
     */
    if (TAF_SDC_UE_USAGE_DATA_CENTRIC == TAF_SDC_GetUeUsageSetting())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
}



TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelectionInImsPsVoicePreferredOnEutran(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstLteNwCapInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucRoamSupport;

    pstLteNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }    

    /*  IMS is in registered status, select CS over IP */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* select nas signalling if UE support CSFB and LTE network support CSFB preferred */
    if ((TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS == TAF_SDC_GetLteCsSeviceCap())
     && (TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO == pstLteNwCapInfo->enLteCsCap))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* select buffer message if IMS is registering */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* check UE's usage setting if IMS is in deregistering or deregistered status.
     * If it is data centric, select nas signalling. Or buffer message is selected.
     */
    if (TAF_SDC_UE_USAGE_DATA_CENTRIC == TAF_SDC_GetUeUsageSetting())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
}



TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelectionInImsPsVoiceOnlyOnEutran(VOS_VOID)
{
    VOS_UINT8                           ucRoamSupport;

    /* PS service is not in normal status, return buffer message */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != TAF_SDC_GetPsServiceStatus())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
    }    

    /* select CS over IP if IMS is in registered status */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* select buffer message if IMS is registering */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == TAF_SDC_GetImsNormalRegStatus())    
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* service request is failure if IMS is in deregistering or deregistered status. */
    return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
}



TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelectionOnEutran(VOS_VOID)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32                        enVoicePerferred;

    if (VOS_FALSE == TAF_SDC_GetLteImsSupportFlag())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    enVoicePerferred = TAF_SDC_GetVoiceDomain();

    switch (enVoicePerferred)
    {
        /* domain selection in CS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_CS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelectionInCsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice preferred */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_PREFERRED:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelectionInImsPsVoicePreferredOnEutran();
            break;

        /* domain selection in IMS PS voice only */
        case TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelectionInImsPsVoiceOnlyOnEutran();
            break;

        /* domain selection in CS voice only */
        case TAF_SDC_VOICE_DOMAIN_CS_ONLY:
        default:
            enDomainSelRslt  = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
            break;
    }

    return (enDomainSelRslt);
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSmsDomainSelectionOnEutran(VOS_VOID)
{
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enImsaRegStatus;
    TAF_SDC_LTE_CS_CAPBILITY_ENUM_UINT8 enLteCsCap;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   enPsServiceStatus;
    VOS_UINT8                           ucRoamSupport;

    /*
       3.驻留在LTE下时，但不支持IMS短信，发送CS/PS短信
       4.驻留在LTE下，支持IMS短信，SMS OVER IP不优先，发送CS/PS短信
       5.驻留在LTE下，支持IMS短信，SMS OVER IP优先，PS不正常服务服务，缓存消息
       6.IMS正在注册，缓存消息
       7.驻留在LTE下，支持IMS短信，SMS OVER IP优先，IMS未注册成功，发送CS/PS短信
       8.驻留在LTE下，支持IMS短信，SMS OVER IP优先，IMS已经注册成功，
         IMSA主动上报的消息中ext_info指示IMS可以使用SMS，发送IMS短信
       9.驻留在LTE下，支持IMS短信，SMS OVER IP优先，IMS已经注册成功，
         IMSA主动上报的消息中ext_info指示IMS不可以使用SMS，发送CS/PS短信
    */

    /* LTE下的IMS不支持 */
    if (VOS_FALSE == TAF_SDC_GetLteImsSupportFlag())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* 不支持IMS短信 */
    if (VOS_FALSE == TAF_SDC_GetSmsOnImsSupportFlag())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    enPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    /* SMS OVER IP不优先 */
    if (TAF_SDC_SMS_DOMAIN_PREFER_TO_USE_SMS_OVER_IP != TAF_SDC_GetSmsDomain())
    {
        /* 判断CS业务能力之前，首先确认PS服务状态，否则仅仅判定CS业务能力参数不准确 */
        enLteCsCap = TAF_SDC_GetLteNwCapInfo()->enLteCsCap;
        if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE  == enPsServiceStatus)
         && (TAF_SDC_LTE_CS_CAPBILITY_NOT_SUPPORTED == enLteCsCap))
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
        }

        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* PS域非正常服务 */
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE != enPsServiceStatus)
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    ucRoamSupport = TAF_SDC_GetRoamingSupportFlag();
    if ((VOS_TRUE  == TAF_SDC_GetRoamFlag())
     && (VOS_FALSE == ucRoamSupport))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }    

    enImsaRegStatus = TAF_SDC_GetImsNormalRegStatus();    

    /* IMS正在注册 */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REGING == enImsaRegStatus)
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* IMS未注册成功 */
    if (TAF_SDC_IMS_NORMAL_REG_STATUS_REG != enImsaRegStatus)
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* IMS已经注册成功，IMSA主动上报的消息中ext_info指示IMS可以使用SMS */
    /* 目前没有主动上报，接口先保留，上下文中初始化为支持 */
    if (VOS_TRUE == TAF_SPM_GetImsSmsSupportedOnImsServer())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* IMSA主动上报的消息中ext_info指示IMS不可以使用SMS */
    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;

}
#endif


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcCcDomainSelectionOnNoRat(
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    if ((VOS_TRUE == TAF_SDC_GetImsSupportFlag())
     && (MN_CALL_TYPE_EMERGENCY != enCallType))
    {       
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* select NAS signalling if UE don't supported IMS on any RAT or call type is emergency */
    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSmsDomainSelectionOnNoRat(VOS_VOID)
{
    if (VOS_TRUE == TAF_SDC_GetImsSupportFlag())
    {        
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* select NAS signalling if UE don't supported IMS on any RAT */
    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelectionOnNoRat(VOS_VOID)
{
    if (VOS_TRUE == TAF_SDC_GetImsSupportFlag())
    {       
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* select NAS signalling if UE don't supported IMS on any RAT */
    return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcCcDomainSelection(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    MN_CALL_TYPE_ENUM_U8                                    enCallType;
    VOS_UINT8                                               ucImsInvitePtptFlag;
    MN_CALL_APP_REQ_MSG_STRU                               *pstAppMsg       = VOS_NULL_PTR;
    MN_APP_CALL_CALLORIG_REQ_STRU                          *pstStkOrigReq   = VOS_NULL_PTR;

    pstAppMsg       = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;
    pstStkOrigReq   = (MN_APP_CALL_CALLORIG_REQ_STRU *)pstMsg;
    
    enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;

    ucImsInvitePtptFlag = VOS_FALSE;
    
    if (ulEventType == TAF_BuildEventType(WUEPS_PID_AT, MN_CALL_APP_ORIG_REQ))
    {
        enCallType          = pstAppMsg->unParm.stOrig.enCallType;
		
        ucImsInvitePtptFlag = pstAppMsg->unParm.stOrig.ucImsInvitePtptFlag;
    }
    else
    {
        enCallType      = pstStkOrigReq->enCallType;
    }

    /* 这是IMS邀请第三方加入电话会议呼叫，直接选择IMS域发送 */
    if (VOS_TRUE == ucImsInvitePtptFlag)
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }
    
    /* select NAS signalling if call is running on CS domain*/
    if (VOS_TRUE == TAF_SDC_GetCsCallExistFlg())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* call is running on IMS domain and select OVER IP */
    if (VOS_TRUE == TAF_SDC_GetImsCallExistFlg())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* In this case, NAS SIGNALLING is selected */
    /* 异常场景:如MN_CALL_APP_SUPS_CMD_REQ等一般在有业务存在时才下发；如果业务不存在，默认走nas信令 */
    if ((ulEventType != TAF_BuildEventType(WUEPS_PID_AT, MN_CALL_APP_ORIG_REQ))
     && (ulEventType != TAF_BuildEventType(MAPS_STK_PID, STK_CALL_CALLORIG_REQ)))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* 呼叫类型和卡状态的判断放到预处理里面来做 */

    
    /* check whether voice call on IMS is supported or not */
    if (VOS_FALSE == TAF_SDC_GetVoiceCallOnImsSupportFlag())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* 取当前驻留的RAT */
    switch (TAF_SDC_GetCampSysMode())
    {
#if (FEATURE_ON == FEATURE_LTE)
        case TAF_SDC_SYS_MODE_LTE:
            enDomainSelRslt = TAF_SPM_ProcCcDomainSelectionOnEutran(pstMsg, enCallType);
            break;
#endif
        /* select NAS signalling in GEARN or UTRAN currently */
        case TAF_SDC_SYS_MODE_WCDMA:
        case TAF_SDC_SYS_MODE_GSM:
            enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
            break;

        /* process no RAT case */
        default:
            enDomainSelRslt = TAF_SPM_ProcCcDomainSelectionOnNoRat(enCallType);
            break;
    }

    /* log service domain selection pre-condition infomation */
    TAF_SPM_LogSrvDomainSelPreconditionInfo();

    return (enDomainSelRslt);
}
TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSsDomainSelection(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    VOS_UINT8                                               ucCsCallExistFlg;
    VOS_UINT8                                               ucImsCallExistFlg;
    VOS_UINT8                                               ucImsSmsExistFlg;
    VOS_UINT8                                               ucImsSsExistFlg;
    VOS_UINT8                                               ucUssdOnImsSupportFlag;

    ucImsCallExistFlg       = TAF_SDC_GetImsCallExistFlg();
    ucImsSmsExistFlg        = TAF_SDC_GetImsSmsSrvExistFlg();
    ucImsSsExistFlg         = TAF_SDC_GetImsSsSrvExistFlg();

    /* 目前UT布署在A核, AT或STK发起的非USSD的SS业务统一走CS域;
       STK或AT发起的USSD业务需要根据支持IMS USSD NV项是否打开，如果打开，
       经过正常SPM进行域选择，否则走CS域；
       IMS电话或短信业务存在时,收到AT或STK发起的非USSD的SS业务直接回复失败; */

    ucUssdOnImsSupportFlag = TAF_SDC_GetUssdOnImsSupportFlag();
    
    if ((VOS_FALSE == TAF_SPM_IsUssdServiceType(ulEventType))
     || (VOS_FALSE == ucUssdOnImsSupportFlag))
    {
        if ((VOS_TRUE   ==  ucImsCallExistFlg)
         || (VOS_TRUE   ==  ucImsSmsExistFlg)
         || (VOS_TRUE   ==  ucImsSsExistFlg))
        {
            return TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL;
        }
    
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }
    

    ucCsCallExistFlg = TAF_SDC_GetCsCallExistFlg();
    if ((VOS_TRUE == TAF_SDC_GetCsSsSrvExistFlg())
     || (VOS_TRUE == ucCsCallExistFlg))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    if ((VOS_TRUE == ucImsSsExistFlg)
     || (VOS_TRUE == ucImsCallExistFlg))
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* release消息域选择，CUSD不带ussd string的场景，卡状态的判断放到预处理里面来做 */


    switch (TAF_SDC_GetCampSysMode())
    {
#if ( FEATURE_ON == FEATURE_LTE )
        case TAF_SDC_SYS_MODE_LTE:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelectionOnEutran();
            break;
#endif

        /* currently, select NAS signalling in GEARN or UTRAN */
        case TAF_SDC_SYS_MODE_WCDMA:
        case TAF_SDC_SYS_MODE_GSM:
            enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
            break;

        /* no network */
        default:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelectionOnNoRat();
            break;
    }

    /* log service domain selection pre-condition infomation */
    TAF_SPM_LogSrvDomainSelPreconditionInfo();

    return (enDomainSelRslt);
}


TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcSmsDomainSelection(VOS_VOID)
{
    VOS_UINT8                                                   ucCsSmsExistFlg;
    VOS_UINT8                                                   ucPsSmsExistFlg;
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                        enDomainSelRslt;

    /*
       有CS短信业务存在时，选择当前域;
       如果有IMS短信业务存在时，且无MO IMS短信在发送时，选择当前域；
       如果有IMS短信业务存在时，且MO IMS短信在发送时，选择缓存
       无短信业务存在时，进行如下判断:
       1.驻留在LTE下时，调用EUTRAN下的短信域选择函数
       2.驻留在GU下时，发送CS/PS短信
       3.不驻留在GUL下，缓存消息
    */
    ucCsSmsExistFlg = TAF_SDC_GetCsSmsSrvExistFlg();
    ucPsSmsExistFlg = TAF_SDC_GetPsSmsSrvExistFlg();

    /* 如果有CS/PS域SMS业务正在运行，发送CS/PS短信 */
    if ( (VOS_TRUE == ucCsSmsExistFlg)
      || (VOS_TRUE == ucPsSmsExistFlg) )
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
    }

    /* 如果有IMS短信正在发送，需放消息进入域选择缓存中 */
    if (VOS_TRUE == TAF_SPM_GetImsSmsSendingFlg())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE;
    }

    /* 如果有MT IMS域SMS业务正在运行，选择当前域 */
    if (VOS_TRUE == TAF_SDC_GetImsSmsSrvExistFlg())
    {
        return TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP;
    }

    /* 卡状态的判断放到预处理里面来做 */


    switch (TAF_SDC_GetCampSysMode())
    {
        /* 驻留在LTE下时，调用EUTRAN下的短信域选择函数 */
#if ( FEATURE_ON == FEATURE_LTE )
        case TAF_SDC_SYS_MODE_LTE:
            enDomainSelRslt = TAF_SPM_ProcSmsDomainSelectionOnEutran();
            break;
#endif

        /* 2.驻留在GU下 */
        case TAF_SDC_SYS_MODE_GSM:
        case TAF_SDC_SYS_MODE_WCDMA:
            enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING;
            break;

        /* 3.无网络下 */
        default:
            enDomainSelRslt = TAF_SPM_ProcSmsDomainSelectionOnNoRat();;
            break;

    }

    /* log service domain selection pre-condition infomation */
    TAF_SPM_LogSrvDomainSelPreconditionInfo();

    return enDomainSelRslt;
}




TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 TAF_SPM_ProcServiceRequestDomainSelection(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8                         enSrvReqType;

    enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_BUTT;

    enSrvReqType    = TAF_SPM_GetServiceRequestType(pstMsg);

    switch (enSrvReqType)
    {
        case TAF_SPM_SRV_REQ_TYPE_CALL:
            enDomainSelRslt = TAF_SPM_ProcCcDomainSelection(ulEventType, pstMsg);
            break;

        case TAF_SPM_SRV_REQ_TYPE_SS:
            enDomainSelRslt = TAF_SPM_ProcSsDomainSelection(ulEventType, pstMsg);
            break;

        case TAF_SPM_SRV_REQ_TYPE_SMS:
            enDomainSelRslt = TAF_SPM_ProcSmsDomainSelection();
            break;

        default:
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_ProcServiceRequestDomainSelection: unknow message is received!");
            break;
    }

    return (enDomainSelRslt);
}


VOS_UINT32 TAF_SPM_ProcCcSrvReqProtectTimerExpired(VOS_VOID)
{
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstCachedMsg = VOS_NULL_PTR;

    /* get first index message address */
    pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromCcQueue(0);

    /* indicate sender that CC service request is failure */
    TAF_SPM_SendCcServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                    (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                    TAF_CS_CAUSE_DOMAIN_SELECTION_TIMER_EXPIRED);

    /* clear the first cached message in queue */
    TAF_SPM_FreeSpecificedIndexMessageInCcQueue(0);

    /* log service domain selection infomation */
    TAF_SPM_LogSrvDomainSelQueueInfo();

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_ProcSmsSrvReqProtectTimerExpired(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           i;
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstCachedMsg   = VOS_NULL_PTR;
    MN_APP_MSG_SEND_REQ_STRU           *pstAppMsg      = VOS_NULL_PTR;
    REL_TIMER_MSG                      *pstRcvTimerMsg = VOS_NULL_PTR;
    VOS_UINT16                          usClientId;
    VOS_UINT32                          ulEventType;


    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;
    usClientId     = (VOS_UINT16)pstRcvTimerMsg->ulPara;
    
    /* 查找起动保护定时器消息 */
    for (i = 0; i < TAF_SPM_GetSmsMsgQueueNum(); i++)
    {
        pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromSmsQueue(i);

        pstAppMsg   = (MN_APP_MSG_SEND_REQ_STRU *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]);
        ulEventType = pstCachedMsg->stMsgEntry.ulEventType;

        /* 根据client ID查找缓存消息, SMMA消息不起域选择定时器，因此不care */
        if ((pstAppMsg->usClientId == usClientId)
         && (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_SPM_SMMA_IND) != ulEventType))
        {
            /* send fail indication to user */
            TAF_SPM_SendSmsServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                             (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                             MN_ERR_CLASS_SMS_DOMAIN_SELECTION_TIMER_EXPIRED);

            /* clear the first cached message in queue */
            TAF_SPM_FreeSpecificedIndexMessageInSmsQueue(i);

            break;
        }
    }

    /* log service domain selection infomation */
    TAF_SPM_LogSrvDomainSelQueueInfo();

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_ProcSsSrvReqProtectTimerExpired(VOS_VOID)
{
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstCachedMsg = VOS_NULL_PTR;

    /* get the first cached message in queue */
    pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromSsQueue(0);

    TAF_SPM_SendSsServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                    (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                    TAF_ERR_SS_DOMAIN_SELECTION_TIMER_EXPIRED);

    /* clear the first cached message in queue */
    TAF_SPM_FreeSpecificedIndexMessageInSsQueue(0);

    /* log service domain selection infomation */
    TAF_SPM_LogSrvDomainSelQueueInfo();

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_ProcBufferedDomainSelectionResult(
    TAF_SPM_ENTRY_MSG_STRU                                     *pstMsgEntry,
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                        enDomainSelRslt
)
{
    VOS_UINT32                          ulRst;

    /* tranfer message to call module if the result is nas signalling */
    if (TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING == enDomainSelRslt)
    {
        TAF_SPM_ProcReqMsgBasedOnNasSignalling(pstMsgEntry->ulEventType,
                                               (struct MsgCB*)&(pstMsgEntry->aucEntryMsgBuffer[0]));

        TAF_SPM_UpdateServiceCtrlEntryMsg(pstMsgEntry->ulEventType,
                                          (struct MsgCB*)&(pstMsgEntry->aucEntryMsgBuffer[0]));

        TAF_SPM_SetUpdateEntryMsgFlg(VOS_TRUE);

        /* message need to be processed further by call module, so return VOS_FALSE */
        ulRst = VOS_FALSE;
    }
    /* tranfer message to call module if the result is nas signalling */
    else if (TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP == enDomainSelRslt)
    {
        ulRst = TAF_SPM_ProcReqMsgBasedOnCsOverIp(pstMsgEntry->ulEventType,
                                                     (struct MsgCB*)&(pstMsgEntry->aucEntryMsgBuffer[0]));

        /* for SMS message, message need to be processed by MSG module, not IMSA. */
        if (VOS_FALSE == ulRst)
        {
            TAF_SPM_UpdateServiceCtrlEntryMsg(pstMsgEntry->ulEventType,
                                              (struct MsgCB*)&(pstMsgEntry->aucEntryMsgBuffer[0]));

            TAF_SPM_SetUpdateEntryMsgFlg(VOS_TRUE);
        }
    }
    /* send fail to sender of service request if the result is select fail */
    else
    {
        ulRst = TAF_SPM_ProcServiceRequestFail(pstMsgEntry->ulEventType,
                                          (struct MsgCB*)&(pstMsgEntry->aucEntryMsgBuffer[0]));
    }

    return ulRst;
}
VOS_UINT8 TAF_SPM_IsSmmaMsgExistedInSericeSelectionBuffer(VOS_VOID)
{
    VOS_UINT8                           i;
    VOS_UINT8                           ucNumOfMsg;
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstCacheMsg = VOS_NULL_PTR;

    ucNumOfMsg = TAF_SPM_GetSmsMsgQueueNum();

    for(i = 0; (i < ucNumOfMsg) && (i < TAF_SPM_MAX_SMS_MSG_QUEUE_NUM); i++)
    {
        pstCacheMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromSmsQueue(i);

        if (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_SPM_SMMA_IND) == pstCacheMsg->stMsgEntry.ulEventType)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_VOID TAF_SPM_ProcSmmaIndDomainSelectionResult(
    VOS_UINT32                                              ulEventType,
    struct MsgCB                                           *pstMsg,
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt
)
{
    TAF_MSG_SIGNALLING_TYPE_ENUM_UINT32                     enMsgSignallingType;
    MN_APP_REQ_MSG_STRU                                    *pstSmsMsg = VOS_NULL_PTR;       

    if (TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP == enDomainSelRslt)
    {        
        enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_CS_OVER_IP;        
    }
    else if (TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING == enDomainSelRslt)
    {
        enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_NAS_SIGNALLING;
    }
    else if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE == enDomainSelRslt)
    {
        if (VOS_FALSE == TAF_SPM_IsSmmaMsgExistedInSericeSelectionBuffer())
        {
            /* 缓存SMMA消息后，不care缓存是否失败，返回 */
            TAF_SPM_BufferAppRequestMessage(ulEventType, pstMsg);
        }

        return;
    }
    else
    {
        /* 忽略该SMMA消息 */
        return;
    }
    
    /* 短信换域重拨功能打开，保存消息到缓存 */
    if (TAF_MSG_SIGNALLING_TYPE_CS_OVER_IP == enMsgSignallingType)
    {
        
        /* 设置IMS短信发送标志为TRUE */
        TAF_SPM_SetImsSmsSendingFlg(VOS_TRUE);

        if (VOS_TRUE == TAF_SDC_GetSmsRedailFromImsToCsSupportFlag())
        {
            pstSmsMsg   = (MN_APP_REQ_MSG_STRU *)pstMsg;
            ulEventType = TAF_BuildEventType(pstSmsMsg->ulSenderPid, pstSmsMsg->usMsgName);

            if (VOS_FALSE == TAF_SPM_StoreMsgIntoSmsRedialBuffer(ulEventType, pstMsg))
            {
                TAF_WARNING_LOG(WUEPS_PID_TAF,
                          "TAF_SPM_ProcSmmaIndDomainSelectionResult: TAF_SPM_StoreMsgIntoSmsRedialBuffer error!");
            }
        }
    }

    /* 发送消息到MSG模块 */
    TAF_SPM_SendMsgSmmaRsp(enMsgSignallingType);
}
VOS_UINT32 TAF_SPM_ProcCcMsgQueue(TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 *pEnDomainSelRslt)
{
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstMsgInfo = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pstAppMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    /* get first index message address */
    pstMsgInfo  = TAF_SPM_GetSpecifiedIndexMessageAddrFromCcQueue(0);

    /* execute doman selection for cached message */
    *pEnDomainSelRslt = TAF_SPM_ProcCcDomainSelection(pstMsgInfo->stMsgEntry.ulEventType,
                                                      (struct MsgCB*)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]));

    /* if the result is buffer message, nothing to do */
    if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE == *pEnDomainSelRslt)
    {
        return VOS_TRUE;
    }

    /* process domain selection result */
    ulRst = TAF_SPM_ProcBufferedDomainSelectionResult(&(pstMsgInfo->stMsgEntry), *pEnDomainSelRslt);

    /* stop CC service protect timer with client ID */
    pstAppMsg = (MN_APP_REQ_MSG_STRU *)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]);
    TAF_SPM_StopTimer(TI_TAF_SPM_CC_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);

    /* clear the first cached message in queue */
    TAF_SPM_FreeSpecificedIndexMessageInCcQueue(0);

    return ulRst;
}


VOS_UINT32 TAF_SPM_ProcSmsMsgQueue(TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8 *pEnDomainSelRslt)
{
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstMsgInfo = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pstAppMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    /* run SMS domain selection */
    *pEnDomainSelRslt = TAF_SPM_ProcSmsDomainSelection();

    /* if the result is buffer message, nothing to do */
    if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE == *pEnDomainSelRslt)
    {
        return VOS_TRUE;
    }

    /* get the first cached message in queue */
    pstMsgInfo = TAF_SPM_GetSpecifiedIndexMessageAddrFromSmsQueue(0);

    /* SMMA消息需要特殊处理: 不需要停止定时器且是发送外部消息给MSG模块 */
    if (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_SPM_SMMA_IND) == pstMsgInfo->stMsgEntry.ulEventType)
    {
        TAF_SPM_ProcSmmaIndDomainSelectionResult(pstMsgInfo->stMsgEntry.ulEventType,
                                                 (struct MsgCB *)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]),
                                                 *pEnDomainSelRslt);
        
        ulRst = VOS_TRUE;
    }
    else    
    {
        /* process domain selection result */
        ulRst = TAF_SPM_ProcBufferedDomainSelectionResult(&(pstMsgInfo->stMsgEntry), *pEnDomainSelRslt);
        
        /* stop SMS service protect timer */
        pstAppMsg = (MN_APP_REQ_MSG_STRU *)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]);
        TAF_SPM_StopTimer(TI_TAF_SPM_SMS_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);

    }

    /* clear the first cached message in queue */
    TAF_SPM_FreeSpecificedIndexMessageInSmsQueue(0);

    return ulRst;
}
VOS_UINT32 TAF_SPM_ProcSsMsgQueue(VOS_VOID)
{
    TAF_SPM_CACHE_MSG_INFO_STRU                            *pstMsgInfo = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                                    *pstAppMsg  = VOS_NULL_PTR;
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    VOS_UINT32                                              ulRst;

    /* get the first cached message in queue */
    pstMsgInfo = TAF_SPM_GetSpecifiedIndexMessageAddrFromSsQueue(0);

    /* run SS domain selection */
    enDomainSelRslt = TAF_SPM_ProcSsDomainSelection(pstMsgInfo->stMsgEntry.ulEventType,
                                                    (struct MsgCB*)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]));

    /* if the result is buffer message, nothing to do */
    if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE == enDomainSelRslt)
    {
        return VOS_TRUE;
    }

    ulRst = TAF_SPM_ProcBufferedDomainSelectionResult(&(pstMsgInfo->stMsgEntry), enDomainSelRslt);

    /* stop SS service protect timer */
    pstAppMsg = (MN_APP_REQ_MSG_STRU *)&(pstMsgInfo->stMsgEntry.aucEntryMsgBuffer[0]);
    TAF_SPM_StopTimer(TI_TAF_SPM_SS_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);

    /* clear the first cached message in queue */
    TAF_SPM_FreeSpecificedIndexMessageInSsQueue(0);

    return ulRst;
}
VOS_UINT32 TAF_SPM_ProcBufferedMsgInQueue(VOS_VOID)
{
    VOS_UINT32                                              ulRst;
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt;
    VOS_UINT8                                               ucSmsMsgQueueNum;
    VOS_UINT8                                               ucSsMsgQueueNum;

    ulRst = VOS_TRUE;

    if (0 != TAF_SPM_GetCcMsgQueueNum())
    {
        enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_BUTT;

        ulRst = TAF_SPM_ProcCcMsgQueue(&enDomainSelRslt);

        /* check if domain selection type is buffer message */
        if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE != enDomainSelRslt)
        {
            /* if SS or SMS message queue is not null, send domain selection indication message to self */
            ucSmsMsgQueueNum = TAF_SPM_GetSmsMsgQueueNum();
            ucSsMsgQueueNum  = TAF_SPM_GetSsMsgQueueNum();
            if ((0 != ucSmsMsgQueueNum) || (0 != ucSsMsgQueueNum))
            {
                TAF_SPM_SndInternalDomainSelectionInd();
            }

            /* log service domain selection infomation */
            TAF_SPM_LogSrvDomainSelQueueInfo();

            return ulRst;
        }
    }

    /* continue to process SMS message queue */
    if (0 != TAF_SPM_GetSmsMsgQueueNum())
    {
        enDomainSelRslt = TAF_SPM_DOMAIN_SEL_RESULT_BUTT;

        ulRst = TAF_SPM_ProcSmsMsgQueue(&enDomainSelRslt);

        /* check if domain selection type is buffer message */
        if (TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE != enDomainSelRslt)
        {
            /* if SS or SMS message queue is not null, send domain selection indication message to self */
            ucSmsMsgQueueNum = TAF_SPM_GetSmsMsgQueueNum();
            ucSsMsgQueueNum  = TAF_SPM_GetSsMsgQueueNum();
            if ((0 != ucSmsMsgQueueNum) 
             || (0 != ucSsMsgQueueNum))
            {
                TAF_SPM_SndInternalDomainSelectionInd();
            }            

            /* log service domain selection infomation */
            TAF_SPM_LogSrvDomainSelQueueInfo();

            return ulRst;
        }
    }

    /* continue to process SS message queue */
    if (0 != TAF_SPM_GetSsMsgQueueNum())
    {
        ulRst = TAF_SPM_ProcSsMsgQueue();
    }

    /* log service domain selection infomation */
    TAF_SPM_LogSrvDomainSelQueueInfo();

    return ulRst;
}
VOS_VOID TAF_SPM_FreeMessagesInMessageQueue(VOS_VOID)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucMsgQueueNum;
    TAF_SPM_CACHE_MSG_INFO_STRU        *pstCachedMsg = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pstAppMsg    = VOS_NULL_PTR;

    /*
     * Note: At present, we just cache not more than one message. Or here need
     * to process all messages in queue.
     */

     /* free CC service request message in queue */
    ucMsgQueueNum = TAF_SPM_GetCcMsgQueueNum();
    ucMsgQueueNum = TAF_SDC_MIN(ucMsgQueueNum, TAF_SPM_MAX_CC_MSG_QUEUE_NUM);    
    for (ucIndex = 0; ucIndex < ucMsgQueueNum; ucIndex++)
    {
        /* get the first cached message in queue */
        pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromCcQueue(ucIndex);

        /* stop CC service protect timer */
        pstAppMsg    = (MN_APP_REQ_MSG_STRU *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]);
        TAF_SPM_StopTimer(TI_TAF_SPM_CC_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);

        /* notify user that service request is failure */
        TAF_SPM_SendCcServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                        (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                        TAF_CS_CAUSE_POWER_OFF);

        /* clear the first cached message in queue */
        TAF_SPM_FreeSpecificedIndexMessageInCcQueue(ucIndex);

    }    

    /* free SMS service request message in queue */
    ucMsgQueueNum = TAF_SPM_GetSmsMsgQueueNum();
    ucMsgQueueNum = TAF_SDC_MIN(ucMsgQueueNum, TAF_SPM_MAX_SMS_MSG_QUEUE_NUM);    
    for (ucIndex = 0; ucIndex < ucMsgQueueNum; ucIndex++)
    {
        /* get the first cached message in queue */
        pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromSmsQueue(ucIndex);

        /* stop SMS service protect timer */
        pstAppMsg    = (MN_APP_REQ_MSG_STRU *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]);

        /* notify user that service request is failure */
        if (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_SPM_SMMA_IND) != pstCachedMsg->stMsgEntry.ulEventType)
        {
            /* 停止域选择缓存定时器 */
            TAF_SPM_StopTimer(TI_TAF_SPM_SMS_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);   

            /* 通知应用层消息发送失败 */
            TAF_SPM_SendSmsServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                             (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                             MN_ERR_CLASS_SMS_POWER_OFF);                                          
        }

        /* clear the first cached message in queue */
        TAF_SPM_FreeSpecificedIndexMessageInSmsQueue(ucIndex);
    }

    /* free SS service request message in queue */
    ucMsgQueueNum = TAF_SPM_GetSsMsgQueueNum();
    ucMsgQueueNum = TAF_SDC_MIN(ucMsgQueueNum, TAF_SPM_MAX_SS_MSG_QUEUE_NUM);    
    for (ucIndex = 0; ucIndex < ucMsgQueueNum; ucIndex++)
    {
        /* get the first cached message in queue */
        pstCachedMsg = TAF_SPM_GetSpecifiedIndexMessageAddrFromSsQueue(ucIndex);

        /* stop SS service protect timer */
        pstAppMsg    = (MN_APP_REQ_MSG_STRU *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]);
        TAF_SPM_StopTimer(TI_TAF_SPM_SS_SRV_REQ_PROTECT_TIMER, pstAppMsg->clientId);

        /* notify user that service request is failure */
        TAF_SPM_SendSsServiceRequetFail(pstCachedMsg->stMsgEntry.ulEventType,
                                        (struct MsgCB *)&(pstCachedMsg->stMsgEntry.aucEntryMsgBuffer[0]),
                                        TAF_ERR_SS_POWER_OFF);

        /* clear the first cached message in queue */
        TAF_SPM_FreeSpecificedIndexMessageInSsQueue(ucIndex);
    }

    /* log service domain selection information */
    TAF_SPM_LogSrvDomainSelQueueInfo();
}


VOS_VOID TAF_SPM_ProcMmaPowerOffInd(VOS_VOID)
{    
    /* free all buffered message */
    TAF_SPM_FreeMessagesInMessageQueue();

    /* reset IMS normal register status */
    TAF_SDC_SetImsNormalRegStatus(TAF_SDC_IMS_NORMAL_REG_STATUS_DEREG);
}



VOS_UINT32 TAF_SPM_ProcImsaNormalRegStatusNotify(
    struct MsgCB                       *pstMsg
)
{
    IMSA_NORMAL_REG_STATUS_NOTIFY_STRU *pstImsaNormalRegNotify = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    ulRst = VOS_TRUE;

    pstImsaNormalRegNotify = (IMSA_NORMAL_REG_STATUS_NOTIFY_STRU *)pstMsg;

    /* update IMS register status */
    TAF_SDC_SetImsNormalRegStatus((TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM_UINT8)pstImsaNormalRegNotify->enNormalRegStatus);

    /* process the cached message in queue */
    ulRst = TAF_SPM_ProcBufferedMsgInQueue();

    return ulRst;
}


VOS_UINT32 TAF_SPM_BufferAppRequestMessage(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;
    VOS_UINT32                          ulRst;
    MN_APP_REQ_MSG_STRU                *pstAppMsg = VOS_NULL_PTR;

    ulRst        = VOS_FALSE;

    /* get service request type */
    enSrvReqType = TAF_SPM_GetServiceRequestType(pstMsg);

    pstAppMsg    = (MN_APP_REQ_MSG_STRU *)pstMsg;

    switch (enSrvReqType)
    {
        case TAF_SPM_SRV_REQ_TYPE_CALL:
            /* queue is full? */
            if (VOS_FALSE == TAF_SPM_IsCcMsgQueueFull())
            {
                /* put message in queue */
                ulRst = TAF_SPM_PutMessageToCcQueue(ulEventType, pstMsg);

                /* message is cached successfully, start timer */
                if (VOS_TRUE == ulRst)
                {
                    TAF_SPM_StartTimer(TI_TAF_SPM_CC_SRV_REQ_PROTECT_TIMER,
                                       TI_TAF_SPM_CC_SRV_REQ_PROTECT_TIMER_VAL,
                                       pstAppMsg->clientId);
                }
            }
            break;

        case TAF_SPM_SRV_REQ_TYPE_SMS:
            if (VOS_FALSE == TAF_SPM_IsSmsMsgQueueFull())
            {
                ulRst = TAF_SPM_PutMessageToSmsQueue(ulEventType, pstMsg);

                /* message is cached successfully, start timer */
                if (VOS_TRUE == ulRst)
                {
                    TAF_SPM_StartTimer(TI_TAF_SPM_SMS_SRV_REQ_PROTECT_TIMER,
                                       TI_TAF_SPM_SMS_SRV_REQ_PROTECT_TIMER_VAL,
                                       pstAppMsg->clientId);
                }
            }
            break;

        case TAF_SPM_SRV_REQ_TYPE_SS:
            if (VOS_FALSE == TAF_SPM_IsSsMsgQueueFull())
            {
                ulRst = TAF_SPM_PutMessageToSsQueue(ulEventType, pstMsg);

                /* message is cached successfully, start timer */
                if (VOS_TRUE == ulRst)
                {
                    TAF_SPM_StartTimer(TI_TAF_SPM_SS_SRV_REQ_PROTECT_TIMER,
                                       TI_TAF_SPM_SS_SRV_REQ_PROTECT_TIMER_VAL,
                                       pstAppMsg->clientId);
                }
            }
            break;

        default:
            break;
    }

    /* log service domain selection infomation */
    TAF_SPM_LogSrvDomainSelQueueInfo();

    return (ulRst);
}
VOS_UINT32 TAF_SPM_ProcServiceRequestFail(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;

    enSrvReqType = TAF_SPM_GetServiceRequestType(pstMsg);

    switch (enSrvReqType)
    {
        case TAF_SPM_SRV_REQ_TYPE_CALL:
            TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_DOMAIN_SELECTION_FAILURE);
            break;

        case TAF_SPM_SRV_REQ_TYPE_SMS:
            TAF_SPM_SendSmsServiceRequetFail(ulEventType, pstMsg, MN_ERR_CLASS_SMS_DOMAIN_SELECTION_FAILURE);
            break;

        case TAF_SPM_SRV_REQ_TYPE_SS:
            TAF_SPM_SendSsServiceRequetFail(ulEventType, pstMsg, TAF_ERR_SS_DOMAIN_SELECTION_FAILURE);
            break;

        default:
            break;

    }
    return (VOS_TRUE);
}


VOS_UINT32 TAF_SPM_ProcInitialDomainSelectionResult(
    VOS_UINT32                                              ulEventType,
    struct MsgCB                                           *pstMsg,
    TAF_SPM_DOMAIN_SEL_RESULT_ENUM_UINT8                    enDomainSelRslt
)
{
    VOS_UINT32                                              ulRet;

    ulRet = VOS_FALSE;

    switch (enDomainSelRslt)
    {
        case TAF_SPM_DOMAIN_SEL_RESULT_NAS_SIGNALLING:
            ulRet = TAF_SPM_ProcReqMsgBasedOnNasSignalling(ulEventType, pstMsg);
            break;

        case TAF_SPM_DOMAIN_SEL_RESULT_CS_OVER_IP:
            ulRet = TAF_SPM_ProcReqMsgBasedOnCsOverIp(ulEventType, pstMsg);
            break;

        case TAF_SPM_DOMAIN_SEL_RESULT_BUFFER_MESSAGE:
            ulRet = TAF_SPM_BufferAppRequestMessage(ulEventType, pstMsg);

            /* if buffer message is failure, send service fail to message sender */
            if (VOS_FALSE == ulRet)
            {
                ulRet = TAF_SPM_ProcServiceRequestFail(ulEventType, pstMsg);
            }
            break;

        case TAF_SPM_DOMAIN_SEL_RESULT_SELECT_FAIL:
            ulRet = TAF_SPM_ProcServiceRequestFail(ulEventType, pstMsg);
            break;

        default:
            TAF_ERROR_LOG(WUEPS_PID_TAF,
                          "TAF_SPM_ProcInitDomainSelectionResult: domain selection type is error!");
            break;
    }

    return (ulRet);
}
VOS_UINT32 TAF_SPM_ProcReqMsgBasedOnCsOverIp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRetFlg;
    TAF_SPM_PROC_MSG_STRU              *pstTabHeader = VOS_NULL_PTR;

    /* Get tab first address */
    pstTabHeader = TAF_SPM_GetImsDomainSelMsgTabAddr();

    /* look up the matched message event type */
    for (i = 0; i < TAF_SPM_GET_IMS_DOMAIN_SEL_MSG_TAB_SIZE; i++)
    {
        if (ulEventType == pstTabHeader->ulMsgEvtType)
        {
            ulRetFlg = pstTabHeader->pfMsgFun(pstMsg);

            return ulRetFlg;
        }

        pstTabHeader++;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SPM_RcvRegisterSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaRegisterSsReq(pstMsg);

    TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvEraseSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaEraseSsReq(pstMsg);

    TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvActivateSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaActivateSsReq(pstMsg);

    TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvDeactivateSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaDeactivateSsReq(pstMsg);

    TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvInterrogateSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaInterrogateSsReq(pstMsg);

    TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvProcUssSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg   = VOS_NULL_PTR;
    TAF_SS_PROCESS_USS_REQ_STRU        *pstSsReqMsg = VOS_NULL_PTR;

    pstRcvMsg   = (MN_APP_REQ_MSG_STRU *)pstMsg;
    pstSsReqMsg = (TAF_SS_PROCESS_USS_REQ_STRU *)&(pstRcvMsg->aucContent[0]);


    /* check whether or not USSD string exists */
    if (0 != pstSsReqMsg->UssdStr.usCnt)
    {
        TAF_SPM_SendImsaProcessUssSsReq(pstMsg);

        TAF_SDC_SetImsSsSrvExistFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvRleaseSsReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SPM_SendImsaReleaseSsReq(pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvAppOrigReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32    enVoicePerferred;
    VOS_UINT32                          ulEventType;
    MN_CALL_APP_REQ_MSG_STRU           *pstAppMsg       = VOS_NULL_PTR;

    pstAppMsg = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;

    if (VOS_TRUE == pstAppMsg->unParm.stOrig.ucImsInvitePtptFlag)
    {
        TAF_SPM_SendImsaCallInviteNewPtptReq(pstAppMsg);

        return VOS_TRUE;
    }
	
    /* 重拨功能打开且语音优先域为CS preferred 或VOIP preferred时，保存消息到缓存 */
    enVoicePerferred = TAF_SDC_GetVoiceDomain();
    if ((VOS_TRUE                         == TAF_SDC_GetCallRedailFromImsToCsSupportFlag())
     && (TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY != enVoicePerferred))
    {    
        ulEventType = TAF_BuildEventType(pstAppMsg->ulSenderPid, pstAppMsg->enReq);
        if (VOS_FALSE == TAF_SPM_StoreMsgIntoCallRedialBuffer(ulEventType, pstMsg))
        {
            /* 存储消息失败，应该有一个call正在发起，拒绝当前call */
            TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_NOT_ALLOW);

            return VOS_TRUE;
        }
    }

    TAF_SPM_SendImsaCallOrigReq(pstAppMsg);
    
    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvAppSupsCmdReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_CALL_APP_REQ_MSG_STRU            *pstAppMsg = VOS_NULL_PTR;

    pstAppMsg = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;
    
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaSupsCmdReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg);

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
    
    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_RcvAppGetInfoReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaGetCallInfoReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg, CALL_IMSA_GET_CALL_INFO_REQ_TYPE_CLCC);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvAppStartDtmfReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaStartDtmfReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvAppStopDtmfReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaStopDtmfReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvAppGetCdurReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaGetCdurReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvAppGetCallInfoReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaGetCallInfoReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg, CALL_IMSA_GET_CALL_INFO_REQ_TYPE_ATA);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvAppGetClprReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaGetClprReq((MN_CALL_APP_REQ_MSG_STRU *)pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvStkOrigReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_CALL_CALLORIG_REQ_STRU      *pstOrigParam       = VOS_NULL_PTR;
    MN_CALL_APP_REQ_MSG_STRU            stAppMsg;
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32    enVoicePerferred;
    VOS_UINT32                          ulEventType;    

    pstOrigParam           = (MN_APP_CALL_CALLORIG_REQ_STRU *)pstMsg;

    /* 重拨功能打开且语音优先域为CS preferred 或VOIP preferred时，保存消息到缓存 */    
    enVoicePerferred = TAF_SDC_GetVoiceDomain();
    if ((VOS_TRUE == TAF_SDC_GetCallRedailFromImsToCsSupportFlag())
     && (TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY != enVoicePerferred))
    {  
        ulEventType = TAF_BuildEventType(pstOrigParam->ulSenderPid, pstOrigParam->ulMsgId);
        if (VOS_FALSE == TAF_SPM_StoreMsgIntoCallRedialBuffer(ulEventType, pstMsg))
        {
            /* 存储消息失败，应该有一个call正在发起，拒绝当前call */
            TAF_SPM_SendCcServiceRequetFail(ulEventType, pstMsg, TAF_CS_CAUSE_NOT_ALLOW);
            return VOS_TRUE;
        }
    }

    PS_MEM_SET(&stAppMsg, 0, sizeof(stAppMsg));
    stAppMsg.clientId                   = pstOrigParam->usClientId;
    stAppMsg.opId                       = pstOrigParam->opID;
    stAppMsg.callId                     = pstOrigParam->callID;
    stAppMsg.unParm.stOrig.enCallType   = pstOrigParam->enCallType;

    /* enClirCfg等参数按默认值填写 */
    stAppMsg.unParm.stOrig.enClirCfg                = MN_CALL_CLIR_AS_SUBS;
    stAppMsg.unParm.stOrig.enCallMode               = MN_CALL_MODE_SINGLE;
    stAppMsg.unParm.stOrig.stCugCfg.bEnable         = VOS_FALSE;
    stAppMsg.unParm.stOrig.stDataCfg.enSpeed        = MN_CALL_CSD_SPD_64K_MULTI;
    stAppMsg.unParm.stOrig.stDataCfg.enName         = MN_CALL_CSD_NAME_SYNC_UDI;
    stAppMsg.unParm.stOrig.stDataCfg.enConnElem     = MN_CALL_CSD_CE_T;

    if (0 != pstOrigParam->stSubAddr.ucLen)
    {
        stAppMsg.unParm.stOrig.stSubaddr.IsExist        = VOS_TRUE;
        stAppMsg.unParm.stOrig.stSubaddr.LastOctOffset  = 0;
    
        PS_MEM_CPY(stAppMsg.unParm.stOrig.stSubaddr.SubAddrInfo,
                    pstOrigParam->stSubAddr.aucSubAddr,
                    MN_CALL_MAX_SUBADDR_INFO_LEN);
    }

    stAppMsg.unParm.stOrig.stDialNumber.enNumType = pstOrigParam->stCalledAddr.ucAddrType;

    stAppMsg.unParm.stOrig.stDialNumber.ucNumLen  = pstOrigParam->stCalledAddr.ucLen;
    if (MN_CALL_MAX_CALLED_BCD_NUM_LEN < pstOrigParam->stCalledAddr.ucLen)
    {
        stAppMsg.unParm.stOrig.stDialNumber.ucNumLen = MN_CALL_MAX_CALLED_BCD_NUM_LEN;
    }

    PS_MEM_CPY(&(stAppMsg.unParm.stOrig.stDialNumber.aucBcdNum[0]),
               &(pstOrigParam->stCalledAddr.aucAddr[0]),
               stAppMsg.unParm.stOrig.stDialNumber.ucNumLen);
                
    PS_MEM_CPY(&(stAppMsg.unParm.stOrig.stEmergencyCat),
                (MN_CALL_EMERGENCY_CAT_STRU *)&pstOrigParam->stEmergencyCat,
                sizeof(MN_CALL_EMERGENCY_CAT_STRU));

    TAF_SPM_SendImsaCallOrigReq(&stAppMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvStkSupsCmdReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_CALL_SUPS_REQ_STRU          *pstCallSups    = VOS_NULL_PTR;
    MN_CALL_APP_REQ_MSG_STRU            stAppMsg;

    pstCallSups = (MN_APP_CALL_SUPS_REQ_STRU *)pstMsg;

    PS_MEM_SET(&stAppMsg, 0, sizeof(stAppMsg));
    stAppMsg.clientId                   = pstCallSups->usClientId;
    stAppMsg.opId                       = pstCallSups->opID;
    stAppMsg.callId                     = pstCallSups->stSupsPara.callId;
    stAppMsg.unParm.stCallMgmtCmd       = pstCallSups->stSupsPara;

    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaSupsCmdReq(&stAppMsg);

    /* 用户挂机，如果有对应的重拨缓存存在，清除它 */
    switch (pstCallSups->stSupsPara.enCallSupsCmd)
    {
        case MN_CALL_SUPS_CMD_REL_CALL_X:
            TAF_SPM_FreeCallRedialBufferWithCallId(pstCallSups->stSupsPara.callId);
            break;
            
        case MN_CALL_SUPS_CMD_REL_ALL_CALL:
        case MN_CALL_SUPS_CMD_REL_ALL_EXCEPT_WAITING_CALL:
            TAF_SPM_FreeCallRedialBufferWithClientId(pstCallSups->usClientId);
            break;

        default:
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_SPM_RcvStkStartDtmfReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_CALL_START_DTMF_REQ_STRU    *pstStartDtmf   = VOS_NULL_PTR;
    MN_CALL_APP_REQ_MSG_STRU            stAppMsg;


    pstStartDtmf = (MN_APP_CALL_START_DTMF_REQ_STRU *)pstMsg;

    PS_MEM_SET(&stAppMsg, 0, sizeof(stAppMsg));
    stAppMsg.clientId                   = pstStartDtmf->usClientId;
    stAppMsg.opId                       = pstStartDtmf->opID;
    stAppMsg.callId                     = pstStartDtmf->callID;
    stAppMsg.unParm.stDtmf              = pstStartDtmf->stTafStartDtmfPara;

    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaStartDtmfReq(&stAppMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvStkStopDtmfReqBasedOnCsOverIp(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_CALL_STOP_DTMF_REQ_STRU     *pstStopDtmf     = VOS_NULL_PTR;
    MN_CALL_APP_REQ_MSG_STRU            stAppMsg;

    pstStopDtmf = (MN_APP_CALL_STOP_DTMF_REQ_STRU *)pstMsg;

    PS_MEM_SET(&stAppMsg, 0, sizeof(stAppMsg));
    stAppMsg.clientId                   = pstStopDtmf->usClientId;
    stAppMsg.opId                       = pstStopDtmf->opID;
    stAppMsg.callId                     = pstStopDtmf->callID;

    /* 发送给IMSA处理 */
    TAF_SPM_SendImsaStopDtmfReq(&stAppMsg);

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_RcvSendRpdataDirectBasedOnCsOverIp(
    struct MsgCB                       *pstRcvMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstSmsMsg = VOS_NULL_PTR;
    MN_MSG_SEND_PARM_STRU              *pstMsgSnd = VOS_NULL_PTR;
    VOS_UINT32                          ulEventType;

    pstSmsMsg = (MN_APP_REQ_MSG_STRU *)pstRcvMsg;

    pstMsgSnd = (MN_MSG_SEND_PARM_STRU *)(pstSmsMsg->aucContent);

    pstMsgSnd->enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_CS_OVER_IP;

    /* 短信换域重拨功能打开，保存消息到缓存 */    
    if (VOS_TRUE == TAF_SDC_GetSmsRedailFromImsToCsSupportFlag())
    {
        ulEventType = TAF_BuildEventType(pstRcvMsg->ulSenderPid, pstSmsMsg->usMsgName);
        
        if (VOS_FALSE == TAF_SPM_StoreMsgIntoSmsRedialBuffer(ulEventType, pstRcvMsg))
        {
            /* 存储消息失败，缓存队列满，拒绝当前SMS */
            TAF_SPM_SendSmsServiceRequetFail(ulEventType, pstRcvMsg, MN_ERR_CLASS_SMS_REDIAL_BUFFER_FULL);
            return VOS_TRUE;
        }
    }

    /* 设置IMS短信发送标志 */    
    TAF_SPM_SetImsSmsSendingFlg(VOS_TRUE);

    return VOS_FALSE;
}
VOS_UINT32 TAF_SPM_RcvSendRpdataFromMemBasedOnCsOverIp(
    struct MsgCB                       *pstRcvMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstSmsMsg    = VOS_NULL_PTR;
    MN_MSG_SEND_FROMMEM_PARM_STRU      *pstSndMemMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulEventType;

    pstSmsMsg    = (MN_APP_REQ_MSG_STRU *)pstRcvMsg;

    pstSndMemMsg = (MN_MSG_SEND_FROMMEM_PARM_STRU *)(pstSmsMsg->aucContent);

    /* set SMS sending flag to IMS stack */
    pstSndMemMsg->enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_CS_OVER_IP;

    /* 短信换域重拨功能打开，保存消息到缓存 */    
    if (VOS_TRUE == TAF_SDC_GetSmsRedailFromImsToCsSupportFlag())
    {
        ulEventType = TAF_BuildEventType(pstRcvMsg->ulSenderPid, pstSmsMsg->usMsgName);
        
        if (VOS_FALSE == TAF_SPM_StoreMsgIntoSmsRedialBuffer(ulEventType, pstRcvMsg))
        {
            /* 存储消息失败，缓存队列满，拒绝当前SMS */
            TAF_SPM_SendSmsServiceRequetFail(ulEventType, pstRcvMsg, MN_ERR_CLASS_SMS_REDIAL_BUFFER_FULL);
            return VOS_TRUE;
        }
    }

    /* 设置IMS短信发送标志 */
    TAF_SPM_SetImsSmsSendingFlg(VOS_TRUE);

    return VOS_FALSE;
}


VOS_UINT8 TAF_SPM_IsNeedtoWaitImsRegStatus(
    TAF_SPM_SERVICE_STATUS_ENUM_UINT8   enLastPsStatus
)
{
    TAF_SPM_SERVICE_STATUS_ENUM_UINT8   enCurrPsStatus;
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32    enVoicePerferred;
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstNwCapInfo = VOS_NULL_PTR;
    
    /* 获取当前网络能力 */
    pstNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* 获取当前的PS服务状态 */
    enCurrPsStatus = (TAF_SPM_SERVICE_STATUS_ENUM_UINT8)TAF_SDC_GetPsServiceStatus();

    /* 获取语音优先域设置 */
    enVoicePerferred = TAF_SDC_GetVoiceDomain();
    
    if ((TAF_SPM_SERVICE_STATUS_NORMAL_SERVICE == enCurrPsStatus)
     && (enCurrPsStatus                        != enLastPsStatus)
     && (TAF_SDC_NW_IMS_VOICE_SUPPORTED        == pstNwCapInfo->enNwImsVoCap)
     && (TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY      == enVoicePerferred))
    {
        return VOS_TRUE;
    }
    
    return VOS_FALSE;
}

#endif
VOS_VOID TAF_SPM_SendCcServiceRequetFail(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_CS_CAUSE_ENUM_UINT32            enCause
)
{
    MN_CALL_APP_REQ_MSG_STRU           *pstAtMsg  = VOS_NULL_PTR;
    MN_APP_CALL_CALLORIG_REQ_STRU      *pstStkMsg = VOS_NULL_PTR;

    /* Notify sender that CC service request is failure */
    if (TAF_BuildEventType(WUEPS_PID_AT, MN_CALL_APP_ORIG_REQ) == ulEventType)
    {
        pstAtMsg = (MN_CALL_APP_REQ_MSG_STRU *)pstMsg;

        TAF_CALL_SendCallOrigCnf(pstAtMsg->clientId, pstAtMsg->opId, pstAtMsg->callId, enCause);

        MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL, 0, enCause, 0);
    }
    else if (TAF_BuildEventType(MAPS_STK_PID, STK_CALL_CALLORIG_REQ) == ulEventType)
    {
        pstStkMsg = (MN_APP_CALL_CALLORIG_REQ_STRU *)pstMsg;
        TAF_CALL_SendCallOrigCnf(pstStkMsg->usClientId, pstStkMsg->opID, pstStkMsg->callID, enCause);

        MN_CALL_ReportErrIndEvent(MN_CLIENT_ALL, 0, enCause, 0);
    }
    else
    {
        /* nothing to do */
    }
}


VOS_VOID TAF_SPM_SendSmsServiceRequetFail(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    VOS_UINT32                          ulCause
)
{
    MN_APP_MSG_SEND_REQ_STRU           *pstAppMsg = VOS_NULL_PTR;

    pstAppMsg = (MN_APP_MSG_SEND_REQ_STRU *)pstMsg;

    switch (ulEventType)
    {
        case TAF_BuildEventType(WUEPS_PID_AT, MN_MSG_MSGTYPE_SEND_RPDATA_FROMMEM):
        case TAF_BuildEventType(WUEPS_PID_AT, MN_MSG_MSGTYPE_SEND_RPDATA_DIRECT):
        case TAF_BuildEventType(MAPS_STK_PID, STK_MSG_SEND_REQ):
            /* Notify sender that SMS service request is failure */
            MN_SendClientResponse(pstAppMsg->usClientId, pstAppMsg->opID, ulCause);
            break;

         default:
            break;
    }

}
VOS_VOID TAF_SPM_SendSsServiceRequetFail(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_SS_ERROR                        errorCode
)
{
    MN_APP_MSG_SEND_REQ_STRU           *pstAppMsg  = VOS_NULL_PTR;

    pstAppMsg  = (MN_APP_MSG_SEND_REQ_STRU *)pstMsg;

    switch (ulEventType)
    {
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_REGISTERSS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_ERASESS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_ACTIVATESS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_DEACTIVATESS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_INTERROGATESS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_PROCESS_USS_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_REGPWD_MSG):
        case TAF_BuildEventType(WUEPS_PID_AT,  TAF_MSG_ERASECCENTRY_MSG):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_REGISTERSS_REQ):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_ERASESS_REQ):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_ACTIVATESS_REQ):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_DEACTIVATESS_REQ):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_INTERROGATESS_REQ):
        case TAF_BuildEventType(MAPS_STK_PID,  STK_SS_USSD_REQ):

            /* 向AT或STK发送失败结果,携带cause  */
            MN_SendClientResponse(pstAppMsg->usClientId, pstAppMsg->opID, errorCode);
            break;

        default:
            break;
    }
}
VOS_UINT32 TAF_SPM_ProcReqMsgBasedOnNasSignalling(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstRcvMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstSmsMsg    = VOS_NULL_PTR;
    MN_MSG_SEND_PARM_STRU              *pstMsgSnd    = VOS_NULL_PTR;
    MN_MSG_SEND_FROMMEM_PARM_STRU      *pstSndMemMsg = VOS_NULL_PTR;

    /* only care SMS request messages from APP */
    switch (ulEventType)
    {
        case TAF_BuildEventType(WUEPS_PID_AT, MN_MSG_MSGTYPE_SEND_RPDATA_DIRECT):
        case TAF_BuildEventType(MAPS_STK_PID, STK_MSG_SEND_REQ):
            pstSmsMsg = (MN_APP_REQ_MSG_STRU *)pstRcvMsg;
            pstMsgSnd = (MN_MSG_SEND_PARM_STRU *)(pstSmsMsg->aucContent);

            /* set SMS sending flag to GUL stack */
            pstMsgSnd->enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_NAS_SIGNALLING;
            break;

        case TAF_BuildEventType(WUEPS_PID_AT, MN_MSG_MSGTYPE_SEND_RPDATA_FROMMEM):
            pstSmsMsg    = (MN_APP_REQ_MSG_STRU *)pstRcvMsg;
            pstSndMemMsg = (MN_MSG_SEND_FROMMEM_PARM_STRU *)(pstSmsMsg->aucContent);

            /* set SMS sending flag to GUL stack */
            pstSndMemMsg->enMsgSignallingType = TAF_MSG_SIGNALLING_TYPE_NAS_SIGNALLING;
            break;

        default:
            break;
    }

    return VOS_FALSE;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

