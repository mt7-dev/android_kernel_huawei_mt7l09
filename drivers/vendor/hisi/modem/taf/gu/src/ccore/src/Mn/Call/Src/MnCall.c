
/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "MnCallProcNvim.h"
#include "MnCall.h"
#include "MnCallReqProc.h"
#include "MnCallMnccProc.h"
#include "MnCallMgmt.h"
#include "MnCallApi.h"
#include "MnCallBcProc.h"
#include "NasCc.h"
#include "PsNvId.h"
#include "NVIM_Interface.h"
#include "VcCallInterface.h"
#include "UsimPsInterface.h"
#include "MnCallCstProc.h"
#include "MnCallTimer.h"
#include "CstNasInterface.h"
#include "RabmTafInterface.h"
#include "NasOmInterface.h"
#include "om.h"
#include "MnCallSendCc.h"
#include "MnComm.h"
#include "MnErrorCode.h"
#include "MnCallMgmt.h"
#include "NVIM_Interface.h"
#include "MnCallCtx.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"
#include "NasUsimmApi.h"

#if (FEATURE_ON == FEATURE_PTM)
#include "NasComm.h"
#include "TafSdcCtx.h"
#include "TafSdcLib.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#define    THIS_FILE_ID        PS_FILE_ID_MNCALL_C

VOS_UINT8 gucTafCallStatusControl = VOS_FALSE;                                  /* 呼叫控制，决定是否上报相应的呼叫事件和是否允许呼叫 */
VOS_UINT8 gucTafMultiSimCallStatusControl = VOS_FALSE;                          /* 是否支持一号双卡，用于一卡支持语音，一卡不支持语音的特性 */
MN_CALL_VP_NV_CFG_STATE_ENUM_U8 g_enVpNvCfgState = MN_CALL_VP_MO_MT_BOTH;       /* NV项，用控制是否支持可视电话的特性 */


/*****************************************************************************
   2 变量定义
*****************************************************************************/
/* call所有呼叫信息 */
LOCAL MN_CALL_INFO_STRU                   f_astCallInfos[MN_CALL_MAX_NUM];



extern    VOS_VOID MN_CALL_SetTchStatus(
    VOS_BOOL                            bAvailable
);


/*****************************************************************************
   3 函数实现
*****************************************************************************/


VOS_VOID  MN_CALL_Init(MN_CALL_POWER_STATE_ENUM_U8 enPowerState)
{
    NAS_NVIM_CUSTOMIZE_SERVICE_STRU               stCustomServ;
    VOS_UINT32                                    ulResult;
    MN_CALL_NV_ITEM_MULTISIM_CALLCON_STRU         stMultiSimCallCon;
    MN_CALL_NV_ITEM_VIDEO_CALL_STRU               stVideoCall;
    VOS_UINT32                                    ulLength;

    ulLength = 0;


    stCustomServ.ulStatus                = VOS_FALSE;
    stCustomServ.ulCustomizeService      = VOS_FALSE;

    stMultiSimCallCon.usMultiSimCallConf = VOS_FALSE;

    stVideoCall.ucStatus                 = VOS_FALSE;
    stVideoCall.ucVpCfgState             = MN_CALL_VP_MO_MT_BOTH;

    MN_CALL_InitCtx();

    /*读取NIM信息*/
    MN_CALL_ReadNvimInfo();

    MN_CALL_InitAllTimers(enPowerState);

    MN_CALL_CapaProcInit();

    /*如果声码器打开则需关闭声码器*/
    if (VOS_TRUE == MN_CALL_GetChannelOpenFlg())
    {
        MN_CALL_SetChannelOpenFlg(VOS_FALSE);
        MN_CALL_ReportChannelEvent(MN_CALL_EVT_CHANNEL_CLOSE);
    }

    MN_CALL_ResetAllCalls(enPowerState);
    MN_CALL_SetTchStatus(VOS_FALSE);


    /* 读取对应的几个NV项，并保存到全局变量中 */
    ulResult = NV_Read(en_NV_Item_CustomizeService,
                       &stCustomServ,
                       sizeof(stCustomServ));

    /* PC工程,回放工程,CC默认打开 */
    #if (defined(__PS_WIN32_RECUR__))
        stCustomServ.ulStatus = VOS_TRUE;
        stCustomServ.ulCustomizeService = VOS_TRUE;
    #endif

    if((ulResult == NV_OK)
    &&(VOS_TRUE == stCustomServ.ulStatus)
    &&(VOS_TRUE == (stCustomServ.ulCustomizeService&0x01)))
    {
       gucTafCallStatusControl = VOS_TRUE;
    }
    else
    {
       gucTafCallStatusControl = VOS_FALSE;
    }
    NV_GetLength(en_NV_Item_MultiSimCallConf, &ulLength);
    ulResult = NV_Read(en_NV_Item_MultiSimCallConf,
                      &stMultiSimCallCon,
                      ulLength);

    if((ulResult == NV_OK)
    &&(VOS_TRUE == stMultiSimCallCon.usMultiSimCallConf))
    {
       gucTafMultiSimCallStatusControl = VOS_TRUE;
    }
    else
    {
       gucTafMultiSimCallStatusControl = VOS_FALSE;
    }
    NV_GetLength(en_NV_Item_VIDEO_CALL, &ulLength);
    ulResult = NV_Read(en_NV_Item_VIDEO_CALL,
                       &stVideoCall,
                       ulLength);

    /* PC工程,回放工程,CC默认打开 */
    #if (defined(__PS_WIN32_RECUR__))
        stVideoCall.ucStatus = VOS_TRUE;
        stVideoCall.ucVpCfgState = MN_CALL_VP_MO_MT_BOTH;
    #endif

    if((ulResult == NV_OK)
     &&(VOS_TRUE == stVideoCall.ucStatus))
    {
        g_enVpNvCfgState = stVideoCall.ucVpCfgState;
    }
    else
    {
        g_enVpNvCfgState = MN_CALL_VP_MO_MT_BOTH;
    }

    

    TAF_CALL_InitDtmfCtx();

    return;
}
VOS_VOID MN_CALL_LineInfo(
    MNPH_USIM_STATUS_ENUM_U32           enUsimStatus
)
{
    MN_CALL_ALS_LINE_INFO_STRU          stAlsInfo;
    VOS_UINT8                           aucMmaImsi[9];
    VOS_BOOL                            bWriteNvim;
    VOS_UINT32                          ulLength;

    ulLength = 0;
    stAlsInfo.enAlsLine = MN_CALL_ALS_LINE_NO_BUTT;
    PS_MEM_SET(stAlsInfo.aucMmaImsi, 0x00, sizeof(stAlsInfo.aucMmaImsi));

    if ( MNPH_USIM_STATUS_AVILABLE == enUsimStatus )
    {
        bWriteNvim = VOS_FALSE;

        if ( NV_OK != NV_Read(en_NV_Item_ALS_LINE_Config,&stAlsInfo,sizeof(stAlsInfo)))
        {
            return;
        }
        if ( USIMM_API_SUCCESS != NAS_USIMMAPI_GetCardIMSI(aucMmaImsi) )
        {
            return ;
        }

        /*线路号值不在取值范围内*/
        if  ( ( MN_CALL_ALS_LINE_NO_1 != stAlsInfo.enAlsLine )
           && ( MN_CALL_ALS_LINE_NO_2 != stAlsInfo.enAlsLine ) )
        {
            stAlsInfo.enAlsLine = MN_CALL_ALS_LINE_NO_1;
            bWriteNvim = VOS_TRUE;
        }

        /*判断NV中当前使用线路号对应的IMSI和SIM卡中IMSI是否相同，若不同则清NV
          项，并填入默认线路号1*/
        if ( 0 != VOS_StrCmp( (VOS_CHAR *)aucMmaImsi,
                               (VOS_CHAR *)stAlsInfo.aucMmaImsi) )
        {
            stAlsInfo.enAlsLine = MN_CALL_ALS_LINE_NO_1;
            PS_MEM_CPY(stAlsInfo.aucMmaImsi,aucMmaImsi,9);
            bWriteNvim = VOS_TRUE;
        }

        /*需要更新NVIM*/
        if ( VOS_TRUE == bWriteNvim )
        {
            NV_GetLength(en_NV_Item_ALS_LINE_Config, &ulLength);
            if (NV_OK != NV_Write(en_NV_Item_ALS_LINE_Config,
                     &stAlsInfo,
                     ulLength) )
            {
                TAF_WARNING_LOG(WUEPS_PID_TAF, "MN_CALL_LineInfo: en_NV_Item_ALS_LINE_Config NV Write Fail!");
            }
        }

        MN_CALL_UpdateAlsLineInfo(stAlsInfo.enAlsLine,VOS_FALSE);
    }
}
VOS_VOID  MN_CALL_ProcAppReqMsg(
    const VOS_VOID                      *pstMsg
)
{

    MN_CALL_APP_REQ_MSG_STRU            *pstCallMsg;

    pstCallMsg = (MN_CALL_APP_REQ_MSG_STRU*)pstMsg;

    MN_NORM_LOG1("MN_CALL_ProcAppReqMsg: enAppReq:", (VOS_INT32)pstCallMsg->enReq);
    /* 根据消息中的应用请求类型，将消息交给相应的请求处理函数处理 */

/*回放工程恢复全局变量*/
#ifdef __PS_WIN32_RECUR__
    if ( VOS_TRUE == NAS_CALL_RestoreContextData((struct MsgCB*)pstMsg))
    {
        return;
    }
#endif

    switch (pstCallMsg->enReq)
    {
    case MN_CALL_APP_ORIG_REQ:
/*PC回放导出全局变量使用*/
#ifndef __PS_WIN32_RECUR__
        NAS_CALL_SndOutsideContextData();
#endif
        MN_CALL_CallOrigReqProc(pstCallMsg->clientId,
                                pstCallMsg->opId,
                                pstCallMsg->callId,
                                (MN_CALL_ORIG_PARAM_STRU*)&pstCallMsg->unParm);
        break;

    case MN_CALL_APP_END_REQ:
        MN_CALL_CallEndReqProc(pstCallMsg->clientId,
                               pstCallMsg->opId,
                               pstCallMsg->callId,
                               (MN_CALL_END_PARAM_STRU*)&pstCallMsg->unParm);
        break;

    case MN_CALL_APP_ANSWER_REQ:
        MN_CALL_CallAnswerReqProc(pstCallMsg->clientId,
                                  pstCallMsg->opId,
                                  pstCallMsg->callId,
                                  (MN_CALL_ANS_PARAM_STRU*)&pstCallMsg->unParm);
        break;

    case MN_CALL_APP_START_DTMF_REQ:
        TAF_CALL_RcvStartDtmfReq((struct MsgCB *)pstCallMsg);
        break;

    case MN_CALL_APP_STOP_DTMF_REQ:
        TAF_CALL_RcvStopDtmfReq((struct MsgCB *)pstCallMsg);
        break;

    case MN_CALL_APP_SUPS_CMD_REQ:
        MN_CALL_CallSupsCmdReqProc((struct MsgCB *)pstCallMsg);
        break;

    case MN_CALL_APP_GET_INFO_REQ:
        MN_CALL_CallInfoReqProc(pstCallMsg->clientId,
                                pstCallMsg->opId);
        break;

    case MN_CALL_APP_GET_CDUR_REQ:
        MN_CALL_GetCdur(pstCallMsg->clientId,
                        pstCallMsg->opId,
                        pstCallMsg->callId);
        break;
    case MN_CALL_APP_SET_UUSINFO_REQ:
        MN_CALL_SetUus1Info(pstCallMsg->clientId,
                            pstCallMsg->opId,
                            (MN_CALL_UUS1_PARAM_STRU*)&pstCallMsg->unParm);
        break;
    case MN_CALL_APP_QRY_UUSINFO_REQ:
        MN_CALL_QryUus1Info(pstCallMsg->clientId,
                            pstCallMsg->opId);
        break;
    case MN_CALL_APP_SET_ALS_REQ:
        MN_CALL_SetAls(pstCallMsg->clientId,
                       pstCallMsg->opId,
                       (MN_CALL_SET_ALS_PARAM_STRU*)&pstCallMsg->unParm);
        break;
    case MN_CALL_APP_QRY_ALS_REQ:
        MN_CALL_QryAls(pstCallMsg->clientId,
                       pstCallMsg->opId);
        break;


    /*AT获取CALLINFO的同步API消息处理*/
    case ID_TAFAGENT_MN_GET_CALL_INFO_REQ:
        MN_CALL_RcvTafAgentGetCallInfo();
        break;


    case MN_CALL_APP_CUSTOM_ECC_NUM_REQ:
        MN_CALL_RcvTafEccNumReq((MN_CALL_APP_CUSTOM_ECC_NUM_REQ_STRU*)&pstCallMsg->unParm);
        break;

    case MN_CALL_APP_CLPR_GET_REQ:
        MN_CALL_RcvAtClprGetReq(pstCallMsg);
        break;

    case MN_CALL_APP_SET_CSSN_REQ:
        MN_CALL_RcvAtCssnSetReq(pstCallMsg->clientId,
                                pstCallMsg->opId,
                                (MN_CALL_SET_CSSN_REQ_STRU*)&pstCallMsg->unParm);
        break;

    case MN_CALL_APP_XLEMA_REQ:
        MN_CALL_RcvAtXlemaReq(pstCallMsg);
        break;

    default:
        MN_WARN_LOG("MN_CALL_ProcAppReqMsg:Invalid Msg Name");
        break;

    }


}


VOS_VOID MN_CALL_ModifyCallId(MNCC_IND_PRIM_MSG_STRU           *pstMsg)
{
    VOS_UINT32                          ulLoop;
    MN_CALL_MGMT_STRU                  *pstMgmtEntity;

    /* 消息中CALL ID在有效范围不需要修改 */
    if ((pstMsg->ucCallId > 0)
     && (pstMsg->ucCallId <= MN_CALL_MAX_NUM))
    {
        return;
    }

    pstMgmtEntity = PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_CALL_MGMT_STRU));
    if (VOS_NULL_PTR == pstMgmtEntity)
    {
        return;
    }

    /*
       获取CALL本地与消息TI匹配的实体，更新CALL ID到消息中
       使用过程中的呼叫实体，状态在INCOMING状态，且TI相同，更新实体的CALL ID到消息
    */
    for (ulLoop = 0; ulLoop < MN_CALL_MAX_NUM; ulLoop++)
    {
        MN_CALL_GetMgmtEntityInfoByCallId((VOS_UINT8)(ulLoop + 1), pstMgmtEntity);
        if (VOS_TRUE != pstMgmtEntity->bUsed)
        {
            continue;
        }

        if (MN_CALL_S_INCOMING != pstMgmtEntity->stCallInfo.enCallState)
        {
            continue;
        }

        if ((VOS_UINT8)pstMgmtEntity->ulTi == pstMsg->ucTi)
        {
            pstMsg->ucCallId = (VOS_UINT8)pstMgmtEntity->stCallInfo.callId;
            break;
        }
    }

    PS_MEM_FREE(WUEPS_PID_TAF, pstMgmtEntity);

    return;
}


VOS_VOID  MN_CALL_ProcMnccPrimMsg(
    const VOS_VOID                      *pMsg
)
{
    MNCC_IND_PRIM_MSG_STRU              *pstMnccMsg;
    MN_CALL_CUSTOM_CFG_INFO_STRU        *pstCustomCfgAddr;

    /* 获取特性控制NV地址 */
    pstCustomCfgAddr                    = MN_CALL_GetCustomCfgInfo();

    pstMnccMsg = (MNCC_IND_PRIM_MSG_STRU*)pMsg;
    MN_NORM_LOG1("MN_CALL_ProcAppReqMsg: enPrimName:", pstMnccMsg->enPrimName);

    /* 根据消息中的原语名，将消息交给相应的MNCC原语处理函数处理 */
    switch(pstMnccMsg->enPrimName)
    {
    case MNCC_SETUP_IND:
/*PC回放导出全局变量使用*/
#ifndef __PS_WIN32_RECUR__
        NAS_CALL_SndOutsideContextData();
#endif
        MN_CALL_ProcMnccSetupInd(pstMnccMsg);
        break;

    case MNCC_SETUP_CNF:
        MN_CALL_ProcMnccSetupCnf(pstMnccMsg);
        break;

    case MNCC_SETUP_COMPL_IND:
        MN_CALL_ProcMnccSetupComplInd(pstMnccMsg);
        break;

    case MNCC_CALL_PROC_IND:
        MN_CALL_ProcMnccCallProcInd(pstMnccMsg);
        break;

    case MNCC_ALERT_IND:
        MN_CALL_ProcMnccAlertInd(pstMnccMsg);
        break;

    case MNCC_DISC_IND:
        MN_CALL_ProcMnccDiscInd(pstMnccMsg);
        break;

    case MNCC_REL_IND:
        MN_CALL_ProcMnccRelInd(pstMnccMsg);
        break;

    case MNCC_REL_CNF:
        MN_CALL_ModifyCallId(pstMnccMsg);
        MN_CALL_ProcMnccRelCnf(pstMnccMsg);
        break;

    case MNCC_REJ_IND:
        MN_CALL_ProcMnccRejInd(pstMnccMsg);
        break;

    case MNCC_FACILITY_IND:
        MN_CALL_ProcMnccFacilityInd(pstMnccMsg);
        break;

    case MNCC_FACILITY_LOCAL_REJ:
        MN_CALL_ProcMnccFacilityLocalRej(pstMnccMsg);
        break;

    case MNCC_HOLD_CNF:
        MN_CALL_ProcMnccHoldCnf(pstMnccMsg);
        break;

    case MNCC_HOLD_REJ:
        MN_CALL_ProcMnccHoldRej(pstMnccMsg);
        break;

    case MNCC_RETRIEVE_CNF:
        MN_CALL_ProcMnccRetrieveCnf(pstMnccMsg);
        break;

    case MNCC_RETRIEVE_REJ:
        MN_CALL_ProcMnccRetrieveRej(pstMnccMsg);
        break;

    case MNCC_START_DTMF_CNF:
        TAF_CALL_RcvCcStartDtmfCnf(pstMnccMsg);
        break;

    case MNCC_START_DTMF_REJ:
        TAF_CALL_RcvCcStartDtmfRej(pstMnccMsg);
        break;

    case MNCC_STOP_DTMF_CNF:
        TAF_CALL_RcvCcStopDtmfCnf(pstMnccMsg);
        break;

    case MNCC_MODIFY_IND:
        /* in-call modification, CC暂不支持 */
        break;

    case MNCC_MODIFY_CNF:
        /* in-call modification, CC暂不支持 */
        break;

    case MNCC_MODIFY_REJ_IND:
        /* in-call modification, CC暂不支持 */
        break;

    case MNCC_SYNC_IND:
        MN_CALL_ProcMnccSyncInd(pstMnccMsg);
        break;

    case MNCC_PROGRESS_IND:
        MN_CALL_ProcMnccProgressInd(pstMnccMsg);
        break;

    case MNCC_CC_EST_IND:

        /*如果控制CCBS的NV打开，则处理*/
        if (MN_CALL_NV_ITEM_ACTIVE == pstCustomCfgAddr->ucCcbsSupportFlg)
        {
            MN_CALL_ProcMnccEstInd(pstMnccMsg);
        }
        break;

    case MNCC_RECALL_IND:

        /*如果控制CCBS的NV打开，则处理*/
        if (MN_CALL_NV_ITEM_ACTIVE == pstCustomCfgAddr->ucCcbsSupportFlg)
        {
            MN_CALL_ProcMnccRecallInd(pstMnccMsg);
        }
        break;

    case MNCC_UUSINFO_IND:
        MN_CALL_ProcUusInfoInd(pstMnccMsg);
        break;

    case MNCC_EMERGENCY_LIST_IND:
        MN_CALL_ProcEmergencyListInd(pstMnccMsg);
        break;

#if (FEATURE_ON == FEATURE_IMS)
    case MNCC_SRVCC_STATUS_IND:
        TAF_CALL_ProcMnccSrvccStatusInd((MNCC_SRVCC_STATUS_IND_STRU*)&(pstMnccMsg->unParam.stSrvccStaInd));
        break;
#endif

    default:
        MN_WARN_LOG("MN_CALL_ProcAppReqMsg:Invalid Msg Name");
        break;
    }

}


/*****************************************************************************
 函 数 名  : MN_CALL_PowerOff
 功能描述  : 软关机时使用，用于清除call模块的所有状态机
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  MN_CALL_PowerOff( VOS_VOID )
{
    MN_CALL_Init(MN_CALL_POWER_STATE_OFF);

}


VOS_VOID  MN_CALL_ProcVCMsg (VOS_VOID * pstMsg)
{
    VC_CALL_MSG_STRU                    *pstTmpMsg;
    TAF_UINT8                           ucNumOfCalls;
    MN_CALL_END_PARAM_STRU              stEndParm;
    VOS_UINT32                          ulRet;

    stEndParm.enEndCause = MN_CALL_INTERWORKING_UNSPECIFIED;

    pstTmpMsg = (VC_CALL_MSG_STRU*)pstMsg;

    switch(pstTmpMsg->enMsgName)
    {
        case VC_CALL_END_CALL:
            MN_CALL_GetCallInfoList(&ucNumOfCalls,f_astCallInfos);
            if (ucNumOfCalls != 0)
            {
                ulRet = MN_CALL_InternalCallEndReqProc(MN_CLIENT_ALL,
                                                       0,
                                                       f_astCallInfos[0].callId,
                                                       &stEndParm);
                if (TAF_CS_CAUSE_SUCCESS != ulRet)
                {
                    MN_WARN_LOG("MN_CALL_ProcVCMsg: Fail to MN_CALL_InternalCallEndReqProc.");
                }
            }
            break;

        default:
            MN_WARN_LOG1("MN_CALL_ProcVCMsg:Wrong Msg.", pstTmpMsg->enMsgName);
            break;
    }
}


VOS_VOID MN_Com_FillNasOmTransMsgHeader(
    ID_NAS_OM_CNF_STRU                  *pstNasOmCnf,
    VOS_UINT16                          usPrimId,
    VOS_UINT16                          usToolsId
)
{
    MODEM_ID_ENUM_UINT16       enModemId;

    enModemId   =   VOS_GetModemIDFromPid(WUEPS_PID_TAF);

    pstNasOmCnf->ucFuncType      = ((enModemId << 6) & NAS_OM_FUNCTION_TYPE_MODEM_MASK) \
                                 | (OM_TRANS_FUNC & NAS_OM_FUNCTION_TYPE_VALUE_MASK);

    pstNasOmCnf->ucReserve       = 0;
    OM_AddSNTime( &(pstNasOmCnf->ulSn), &(pstNasOmCnf->ulTimeStamp) );

    pstNasOmCnf->usPrimId        = usPrimId;
    pstNasOmCnf->usToolsId       = usToolsId;


}

VOS_VOID MN_CALL_ProcOmConfigGsmCodec(
    ID_NAS_OM_INQUIRE_STRU              *pstMsg
)
{
    NAS_OM_GSM_CODEC_CONFIG_ENUM_U8     enGsmCodecConfig;
    VOS_UINT32                          ulRet;
    ID_NAS_OM_CNF_STRU                  *pstNasOmCnf;
    NAS_OM_SET_GSM_CODEC_CONFIG_CNF_STRU stCodecConfigCnf;

    enGsmCodecConfig = pstMsg->aucData[0];
    ulRet = MN_CALL_ConfigGsmCapaProc(enGsmCodecConfig);

    if ( VOS_OK ==  ulRet)
    {
        stCodecConfigCnf.ulRet = VOS_OK;
    }
    else
    {
        stCodecConfigCnf.ulRet = VOS_ERR;
    }

    pstNasOmCnf = (ID_NAS_OM_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_TAF,
         (sizeof(ID_NAS_OM_CNF_STRU)+ sizeof(NAS_OM_SET_GSM_CODEC_CONFIG_CNF_STRU))
                                                - NAS_OM_DATA_PTR_LEN);         /* 申请内存                                 */

    if (VOS_NULL_PTR == pstNasOmCnf)
    {
         MN_ERR_LOG("MN_CALL_ProcOmConfigGsmCodec:Error: Memory Alloc fail");
         return;
    }

    PS_MEM_SET(pstNasOmCnf, 0, sizeof(ID_NAS_OM_CNF_STRU));

    MN_Com_FillNasOmTransMsgHeader(pstNasOmCnf,
                          ID_NAS_OM_SET_CODEC_TYPE_CONFIRM,pstMsg->usToolsId);

    PS_MEM_CPY(pstNasOmCnf->aucData, &stCodecConfigCnf,sizeof(stCodecConfigCnf));

    pstNasOmCnf->usLength = NAS_OM_DATA_OFFSET
                               + sizeof(NAS_OM_SET_GSM_CODEC_CONFIG_CNF_STRU);

    ulRet = PS_SEND_MSG(WUEPS_PID_TAF, pstNasOmCnf);                                   /* 原语发送                                 */
    if (VOS_OK != ulRet)
    {
        MN_WARN_LOG("MN_CALL_ProcOmConfigGsmCodec:WARNING: Send msg fail.");
    }
}


VOS_VOID MN_CALL_ProcOmCodecRpt(
    ID_NAS_OM_INQUIRE_STRU              *pstMsg
)
{
    VOS_UINT32                          ulRet;
    ID_NAS_OM_CNF_STRU                  *pstNasOmCnf;
    NAS_OM_CODEC_TYPE_STRU              stCodecType;
    MN_CALL_CHANNEL_PARAM_STRU          stChannParm;


    PS_MEM_SET(&stCodecType, 0x00, sizeof(stCodecType));

    pstNasOmCnf = (ID_NAS_OM_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_TAF,
                    (sizeof(ID_NAS_OM_CNF_STRU)
                  + sizeof(NAS_OM_CODEC_TYPE_STRU)) - NAS_OM_DATA_PTR_LEN);         /* 申请内存                                 */

    if (VOS_NULL_PTR == pstNasOmCnf)
    {
        MN_ERR_LOG("MN_CALL_ProcOmCodecRpt:Error: Memory Alloc fail");
        return;
    }

    ulRet = MN_CALL_GetTchParm(&stChannParm);

    if( VOS_OK == ulRet )
    {
        stCodecType.enCurrCodec = MN_CALL_ConvertMnCodecToOmCodec(stChannParm.enCodecType);
    }
    else
    {
        stCodecType.enCurrCodec  = NAS_OM_CODEC_TYPE_BUTT;
    }

    stCodecType.enGsmCodecConfig = MN_CALL_ConvertSpeechVers();

    PS_MEM_SET(pstNasOmCnf, 0, sizeof(ID_NAS_OM_CNF_STRU));

    MN_Com_FillNasOmTransMsgHeader(pstNasOmCnf,
                          ID_NAS_OM_CODEC_TYPE_CONFIRM,pstMsg->usToolsId);

    PS_MEM_CPY(pstNasOmCnf->aucData, &stCodecType,sizeof(stCodecType));

    pstNasOmCnf->usLength = NAS_OM_DATA_OFFSET + sizeof(NAS_OM_CODEC_TYPE_STRU);

    ulRet = PS_SEND_MSG(WUEPS_PID_TAF, pstNasOmCnf);                                   /* 原语发送                                 */
    if (VOS_OK != ulRet)
    {
        MN_WARN_LOG("MN_CALL_ProcOmCodecRpt:Warning Send msg fail.");
    }
}



VOS_VOID MN_CALL_ProcOmMsg(
    VOS_VOID                            *pstMsg
)
{
    ID_NAS_OM_INQUIRE_STRU              *pstOmInquire;

    pstOmInquire = (ID_NAS_OM_INQUIRE_STRU *)pstMsg;

    switch ( pstOmInquire->usPrimId )
    {
        case ID_NAS_OM_SET_CODEC_TYPE_REQUIRE :
            MN_CALL_ProcOmConfigGsmCodec(pstOmInquire);
            break;

        case ID_NAS_OM_CODEC_TYPE_INQUIRE :
            MN_CALL_ProcOmCodecRpt(pstOmInquire);
            break;

        default:
            MN_NORM_LOG("MN_CALL_ProcOmMsg:wrong PrimId!");
            break;
    }

}



VOS_VOID  MN_CALL_ProcCstCallPrimitive(
    const VOS_VOID                      *pMsg
)
{
    VOS_UINT16                           usMsgType;

    usMsgType =  *((VOS_UINT16*)((VOS_UINT8*)pMsg + VOS_MSG_HEAD_LENGTH));

    MN_INFO_LOG1("MN_CALL_ProcCstCcPrimitive: primitive:", (VOS_INT32)(usMsgType));

    switch ( usMsgType )
    {
        case ID_CST_CALL_ERROR_IND :
            MN_CALL_ProcCstErrorInd(pMsg);
            break;

        case ID_CST_CALL_SETUP_CNF :
            MN_CALL_ProcCstSetupCnf(pMsg);
            break;

        default:
            MN_WARN_LOG1("MN_CALL_ProcCstCcPrimitive: wrong enMsgID:",  (VOS_INT32)(usMsgType));
            break;
    }
}






VOS_VOID  MN_CALL_ProcRabmCallSyncInd(
    const VOS_VOID                      *pMsg
)
{
    RABM_CALL_SYNC_IND_STRU             *pstSyncMsg;
    VOS_UINT32                          ulNumOfCalls;
    VOS_UINT8                           i;
    MN_CALL_ID_T                        aCallIds[MN_CALL_MAX_NUM];
    MN_CALL_TYPE_ENUM_U8                enCallType;
    VOS_UINT8                           ucCallId;
    VOS_BOOL                            bWaitSendAlertStatus;


    PS_MEM_SET(aCallIds, 0x00, sizeof(aCallIds));

    pstSyncMsg = (RABM_CALL_SYNC_IND_STRU*)pMsg;

    /*
       当指示rab激活，有RB存在，并且当前有呼叫，并且呼叫类型是video call
       的时候，刷新呼叫模块管理实体中对应rb值
       由于VP下CC不会有sync_ind上报，因此增加alerting的发送
    */
    if (RABMCALL_RAB_ACTIVE == pstSyncMsg->ulRabState)
    {
        if (0 != pstSyncMsg->ulRbNum)
        {
            MN_CALL_GetNotIdleStateCalls(&ulNumOfCalls, aCallIds);
            for (i = 0; i < ulNumOfCalls; i++)
            {
                enCallType = MN_CALL_GetCallType(aCallIds[i]);
                if ( MN_CALL_TYPE_VIDEO == enCallType )
                {
                    MN_CALL_UpdateRbId(aCallIds[i], pstSyncMsg->aulRbId[0]);
                    MN_CALL_GetWaitSendAlertStatus(&bWaitSendAlertStatus, &ucCallId);
                    if ((MN_CALL_DIR_MT == MN_CALL_GetCcCallDir(aCallIds[i]))
                      &&(VOS_TRUE == bWaitSendAlertStatus)
                      &&(aCallIds[i] == ucCallId))
                    {
                        MN_CALL_SendCcAlertReq(aCallIds[i]);
                        MN_CALL_SetWaitSendAlertStatus(VOS_FALSE, ucCallId);

                        MN_CALL_ReportEvent(ucCallId, MN_CALL_EVT_INCOMING);
                        
                        MN_CALL_StartTimer(MN_CALL_TID_RING, 0, 0, VOS_RELTIMER_NOLOOP);
                    }
                    
                    return;
                }
            }
        }
    }
}



VOS_VOID  MN_CALL_ProcRabmCallPrimitive(
    const VOS_VOID                      *pMsg
)
{
    MSG_HEADER_STRU                     *pstTmpMsg;

    pstTmpMsg = (MSG_HEADER_STRU*)pMsg;

    switch ( pstTmpMsg->ulMsgName)
    {
        case RABM_CALL_SYNC_IND:
            MN_CALL_ProcRabmCallSyncInd(pMsg);
            break;

        default:
            /*异常打印*/
            MN_WARN_LOG1( "MN_CALL_ProcRabmCallPrimitive: wrong MsgName:",
                          (VOS_INT32)(pstTmpMsg->ulMsgName));
            break;
    }

}




/* MN_CALL_ProcUsimFdnInd */


VOS_VOID MN_CALL_DispatchUsimMsg(struct MsgCB * pstMsg)
{

    /* 预留后续使用 */

    return;
}
VOS_VOID MN_CALL_SupportMoCallType(
    MN_CALL_TYPE_ENUM_U8                enCallType,
    VOS_BOOL                           *pSupported
)
{
    if ((MN_CALL_TYPE_VOICE == enCallType)
     || (MN_CALL_TYPE_EMERGENCY == enCallType))
    {
        *pSupported = VOS_TRUE;
    }
    else
    {
        *pSupported = VOS_FALSE;
    }
}







#if (FEATURE_ON == FEATURE_PTM)

VOS_VOID MN_CALL_CsCallErrRecord(
    MN_CALL_ID_T                        ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            enCause
)
{
    NAS_ERR_LOG_CS_CALL_FAIL_EVENT_STRU                     stCsCallFailEvent;
    MN_CALL_INFO_STRU                                       stCallInfo;
    VOS_UINT32                                              ulIsLogRecord;
    VOS_UINT32                                              ulLength;
    VOS_UINT32                                              ulResult;
    VOS_UINT16                                              usLevel;

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel       = NAS_GetErrLogAlmLevel(NAS_ERR_LOG_ALM_CS_CALL_FAIL);
    ulIsLogRecord = TAF_SDC_IsErrLogNeedRecord(usLevel);

    /* 不需要记录或没有异常时，不保存异常信息 */
    if ((VOS_FALSE == ulIsLogRecord)
     || (TAF_CS_CAUSE_SUCCESS == enCause)
     || (TAF_CS_CAUSE_CC_NW_NORMAL_CALL_CLEARING == enCause))
    {
        return;
    }

    ulLength = sizeof(NAS_ERR_LOG_CS_CALL_FAIL_EVENT_STRU);

    /* 填充Cs呼叫失败异常信息 */
    PS_MEM_SET(&stCsCallFailEvent, 0x00, ulLength);
    PS_MEM_SET(&stCallInfo, 0x00, sizeof(stCallInfo));

    NAS_COMM_BULID_ERRLOG_HEADER_INFO(&stCsCallFailEvent.stHeader,
                                      VOS_GetModemIDFromPid(WUEPS_PID_TAF),
                                      NAS_ERR_LOG_ALM_CS_CALL_FAIL,
                                      usLevel,
                                      VOS_GetSlice(),
                                      (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stCsCallFailEvent.ulCcCause = enCause;

    if (0 == ucCallId)
    {
        /* ucCallId为0说明为本地异常，无法在全局变量中获取信息 */
        stCsCallFailEvent.ucCallId            = ucCallId;
        stCsCallFailEvent.enCallState         = NAS_ERR_LOG_CALL_S_IDLE;
        stCsCallFailEvent.stDiscDir.ucIsUser  = VOS_TRUE;
    }
    else
    {
        MN_CALL_GetCallInfoByCallId(ucCallId, &stCallInfo);

        stCsCallFailEvent.ucCallId    = stCallInfo.callId;
        stCsCallFailEvent.enCallState = stCallInfo.enCallState;
        PS_MEM_CPY(&stCsCallFailEvent.stDiscDir, &stCallInfo.stDiscDir, sizeof(NAS_ERR_LOG_MN_CALL_DISC_DIR_STRU) );
    }

    /*
       将异常信息写入Buffer中
       实际写入的字符数与需要写入的不等则打印异常
     */
    ulResult = TAF_SDC_PutErrLogRingBuf((VOS_CHAR *)&stCsCallFailEvent, ulLength);
    if (ulResult != ulLength)
    {
        NAS_ERROR_LOG(WUEPS_PID_TAF, "MN_CALL_CsCallErrRecord(): Push buffer error.");
    }

    NAS_COM_MntnPutRingbuf(NAS_ERR_LOG_ALM_CS_CALL_FAIL,
                           WUEPS_PID_TAF,
                           (VOS_UINT8 *)&stCsCallFailEvent,
                           sizeof(stCsCallFailEvent));
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
