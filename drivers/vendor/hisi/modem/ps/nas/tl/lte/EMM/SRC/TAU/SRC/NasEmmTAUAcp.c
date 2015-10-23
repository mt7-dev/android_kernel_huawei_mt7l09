

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include "NasEmmAttDetInclude.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMTAUACP_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMM_TAU_SetLai(VOS_VOID *pstRcvMsg )
{
    NAS_EMM_CN_TAU_ACP_STRU             *pstTauAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SetLai: Input para is invalid!");
        return;
    }

    pstTauAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;

    if(NAS_EMM_BIT_SLCT == pstTauAcp->ucBitOpLai)
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_SetLai: LAI is valid");
        NAS_EMM_TAU_GetOpLai()          = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_EMM_TAU_GetLaiAddr(),
                                        &pstTauAcp->stLai,
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    else
    {
        NAS_EMM_TAU_GetOpLai()          = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                NAS_EMM_TAU_GetLaiAddr(),
                                        0,
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    return;
}
VOS_VOID  NAS_EMM_TAU_SetMsId(VOS_VOID *pstRcvMsg )
{
    NAS_EMM_CN_TAU_ACP_STRU             *pstTauAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SetMsId: Input para is invalid!");
        return;
    }

    pstTauAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;

    if(NAS_EMM_BIT_SLCT == pstTauAcp->ucBitOpMsId)
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_SetMsId: MS ID is valid");
        NAS_EMM_TAU_GetOpMsId()         = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_EMM_TAU_GetMsIdAddr(),
                                        &pstTauAcp->stMsId,
                                        sizeof(MMC_LMM_MS_IDNETITY_STRU));
    }
    else
    {
        NAS_EMM_TAU_GetOpMsId()         = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                NAS_EMM_TAU_GetMsIdAddr(),
                                        0,
                                        sizeof(MMC_LMM_MS_IDNETITY_STRU));
    }
    return;
}
VOS_VOID  NAS_EMM_TAU_SetCnCause(VOS_VOID *pstRcvMsg )
{
    NAS_EMM_CN_TAU_ACP_STRU             *pstTauAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SetCnCause: Input para is invalid!");
        return;
    }

    pstTauAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;
    if(NAS_EMM_BIT_SLCT == pstTauAcp->ucBitOpEmmCau)
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_SetEplmnList: EPLMN is valid");
        NAS_EMM_TAU_GetOpCnCause()      = NAS_EMM_BIT_SLCT;
        NAS_EMM_TAU_GetCnCause()        = pstTauAcp->ucEMMCause;
    }
    else
    {
        NAS_EMM_TAU_GetOpCnCause()      = NAS_EMM_BIT_NO_SLCT;
        NAS_EMM_TAU_GetCnCause()        = NAS_LMM_CAUSE_NULL;
    }
    return;
}
VOS_VOID  NAS_EMM_TAU_SetEplmnList(VOS_VOID *pstRcvMsg )
{
    NAS_EMM_CN_TAU_ACP_STRU             *pstTauAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SetEplmnList: Input para is invalid!");
        return;
    }

    pstTauAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;

    if(NAS_EMM_BIT_SLCT == pstTauAcp->ucBitOpEquivalentPLMNs)
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_SetCnCause: cause is valid");


        NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr()    = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        &pstTauAcp->stEquivalentPLMNs,
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }
    else
    {

        NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr()    = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        0,
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    return;
}
VOS_VOID NAS_EMM_TAU_TauAcpEmergencyCsfbSerCollisionProc(VOS_VOID)
{
    /*给MMC上报TAU结果*/
    NAS_EMM_MmcSendTauActionResultIndSucc();

    /* 如果是SMS ONLY，或者是 (语音中心 且是 CSFB_NOT_PREFERED)，则需要disable LTE */
    if((NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())
        || ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode())
            &&(NAS_LMM_ADDITIONAL_UPDATE_CSFB_NOT_PREFERED == NAS_EMM_GetAddUpdateRslt())))
    {
        /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息 */
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        NAS_EMM_DisableLteCommonProc();
    }
    else
    {
        /* 设置UE接受CSFB */
        NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

        /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
        NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();

        /*启动定时器3417ext*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);
    }

    return ;
}
VOS_VOID NAS_EMM_TAU_TauAcpNormalCsfbSerCollisionProc(VOS_VOID)
{

    if (NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())
    {
        /* 对于SMS ONLY,普通CSFB必须先通知MM模块CSFB流程终止，然后上报TAU结果 */
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
        NAS_EMM_MmcSendTauActionResultIndSucc();
        /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/
        return ;
    }

    if ((NAS_EMM_YES == NAS_EMM_IsSmsOnlyCsfbNotPrefDisableNeeded())
        && (NAS_LMM_ADDITIONAL_UPDATE_CSFB_NOT_PREFERED == NAS_EMM_GetAddUpdateRslt()))
    {
        /* 先通知TAU结果，然后通知MM模块需要到GU模去继续CS域流程 */
        NAS_EMM_MmcSendTauActionResultIndSucc();
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        /*NAS_EMM_DisableLteCommonProc();*/
        return ;
    }

    NAS_EMM_MmcSendTauActionResultIndSucc();

    /* 设置UE接受CSFB */
    NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

    /* 组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();

    /* 启动定时器3417ext */
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /* 转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF */
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);
    return ;
}


VOS_VOID NAS_EMM_TAU_TauAcpSerCollisionProc(VOS_VOID)
{
    /* 根据SERVICE发起原因的不同，进行相应处理 */
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_SMS_EST_REQ:

            NAS_EMM_MmcSendTauActionResultIndSucc();
            NAS_LMM_SndLmmSmsEstCnf();
            NAS_LMM_SetConnectionClientId(NAS_LMM_CONNECTION_CLIENT_ID_SMS);
            NAS_EMM_SER_SndEsmBufMsg();

            /* 识别需要DISABLE LTE的场景，向RRC发链路释放请求 */
            /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/
            NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
            return;

        /* 如果不是联合TAU理应不会进入CSFB分支，下面的判断主要用作合法保护 */
        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:

            if ((NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
                && (NAS_EMM_CSFB_ABORT_FLAG_VALID != NAS_EMM_SER_GetEmmSerCsfbAbortFlag()))
            {
                NAS_EMM_TAU_TauAcpNormalCsfbSerCollisionProc();
                return;
            }
            break;

        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:

            if ((NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
                && (NAS_EMM_CSFB_ABORT_FLAG_VALID != NAS_EMM_SER_GetEmmSerCsfbAbortFlag()))
            {
                NAS_EMM_TAU_TauAcpEmergencyCsfbSerCollisionProc();
                return;
            }
            break;

        default:
            break;
    }

    NAS_EMM_MmcSendTauActionResultIndSucc();
    NAS_EMM_SER_SndEsmBufMsg();

    /* 识别需要DISABLE LTE的场景，向RRC发链路释放请求 */
    /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/

    return;

}


VOS_VOID NAS_EMM_TAU_TauAcpCollisionProc()
{

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO(               "NAS_EMM_TAU_TauAcpCollisionProc is entered.");

    /*根据原因值做不用处理*/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :
                NAS_EMM_MmcSendTauActionResultIndSucc();

                /* 清除联合DETACH被TAU打断标识 */
                NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_NO);
                NAS_EMM_TAU_SendDetachReq();
                break;

        case    NAS_EMM_COLLISION_SERVICE    :

                NAS_EMM_TAU_TauAcpSerCollisionProc();
                break;

        default    :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_TauAcpCollisionProc : NO Emm Collision.");
                break;
    }
    NAS_EMM_TAU_SaveEmmCollisionCtrl(   NAS_EMM_COLLISION_NONE);

}
VOS_VOID NAS_EMM_TAU_TauAcpTaOnlySerCollisionProc
(
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;
    LMM_SMS_ERR_CAUSE_ENUM_UINT32       enErrCause = LMM_SMS_ERR_CAUSE_OTHERS;

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pMsgStru;

    /* 根据SERVICE发起原因的不同，进行相应处理 */
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_SMS_EST_REQ:

            /* 根据不同的原因值, 给SMS回复不同原因值的ERR_IND */
            if ( NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pstTAUAcp->ucEMMCause)    /* #2 */
            {
                enErrCause = LMM_SMS_ERR_CAUSE_USIM_CS_INVALID;
            }
            else if (NAS_LMM_CAUSE_CS_NOT_AVAIL == pstTAUAcp->ucEMMCause)       /* #18 */
            {
                enErrCause = LMM_SMS_ERR_CAUSE_CS_SER_NOT_AVAILABLE;
            } /* #16#17#22 */
            else if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
            {
                /* 3402运行 */
                enErrCause = LMM_SMS_ERR_CAUSE_T3402_RUNNING;
            }
            else   /* 3411运行或其他原因 */
            {
                enErrCause = LMM_SMS_ERR_CAUSE_OTHERS;
            }

            NAS_LMM_SndLmmSmsErrInd(enErrCause);
            NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
            break ;

        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:

            if(NAS_EMM_CSFB_ABORT_FLAG_VALID != NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
            {
                /* 普通CSFB则通知MM终止CSFB */
                NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
            }
            break ;

        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:

            /*向MMC上报TAU结果 */
            NAS_EMM_MmcSendTauActionResultIndSucc();

            /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息, 注意与上报SERVICE结果顺序不可颠倒 */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
            NAS_EMM_SER_SndEsmBufMsg();

            /* 先diable LTE，改为API发送TAU CMP消息后直接释放即可 */
            NAS_EMM_DisableLteCommonProc();
            return ;

        default:
            break ;
    }

    NAS_EMM_MmcSendTauActionResultIndSucc();

    NAS_EMM_SER_SndEsmBufMsg();
    #if 0
    if ((NAS_EMM_YES == NAS_EMM_IsCause18DisableNeeded(pstTAUAcp->ucEMMCause))
        || (NAS_EMM_YES == NAS_EMM_IsCause161722DisableNeeded(pstTAUAcp->ucEMMCause)))
    {
        NAS_EMM_DisableLteCommonProc();
    }
    #endif
    return ;
}
VOS_VOID NAS_EMM_TAU_TauAcpTaOnlyCollisionProc
(
    VOS_VOID                           *pMsgStru
)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_TauAcpTaOnlyCollisionProc is entered.");

    /*根据原因值做不用处理*/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :
                NAS_EMM_MmcSendTauActionResultIndSucc();

                /* 清除联合DETACH被TAU打断标识 */
                NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_NO);
                NAS_EMM_TAU_SendDetachReq();
                break;

        case    NAS_EMM_COLLISION_SERVICE    :
                NAS_EMM_TAU_TauAcpTaOnlySerCollisionProc(pMsgStru);
                break;

        default    :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_TauAcpTaOnlyCollisionProc : NO Emm Collision.");
                break;
    }
    NAS_EMM_TAU_SaveEmmCollisionCtrl(   NAS_EMM_COLLISION_NONE);

}
VOS_VOID  NAS_EMM_TAU_TauSuccProc
(
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp      = NAS_EMM_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU          *pMmNetInfo     = NAS_EMM_NULL_PTR;

    pstTAUAcp  = (NAS_EMM_CN_TAU_ACP_STRU*)pMsgStru;

    pMmNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /*设置EU1 UPDATED,并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_UPDATED_EU1;

    /* 更新LastRegNetId */
    NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
	/* lihong00150010 emergency tau&service end */
    /*if new GUTI or TMSI, send TAU COMPLETE*/
    if((EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpGuti)
        || ((EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpMsId)
            && (NAS_EMM_MS_ID_TYPE_TMSI_PTMSI_MTMSI == NAS_EMM_TAU_GetMsIdentityType(pstTAUAcp->stMsId))))
    {
        NAS_EMM_TAU_SendMrrcDataReqTauCmpl();
        NAS_EMM_TAU_SaveEmmTauCompleteFlag(NAS_EMM_TAU_COMPLETE_VALID);
    }
/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_SndEmcEsmMsg();
    }
	/* lihong00150010 emergency tau&service end */

    /*leili modify for isr begin*/
    NAS_EMM_TAU_SetISRAct(pstTAUAcp->ucEPSupdataRst);
    /*leili modify for isr end*/

    /*清空 EPS 承载上下文变更标志*/
    NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_NOCHANGE);

    /* 清除发起TAU的原因值 */
    NAS_EMM_TAU_SaveEmmTAUStartCause(   NAS_EMM_TAU_START_CAUSE_DEFAULT);

    /*如果UE指定的DRX周期变化，发送UE指定的 Drx_Cycle给RRC*/
    #if 0
    if((NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoOpDrx())
     &&(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag()))
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }
    #endif
    if(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag())
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }
    NAS_EMM_UpdateLteContainDrxFlag();
    NAS_EMM_TAU_SaveEmmTauContainDrxFlag(NAS_EMM_NO);

    /* TAU成功后，需清除FIRST TAU FLAG，下次发起TAU时不再携带
       radio capability information update needed IE */
    NAS_LMM_SetEmmInfoFirstTauFlag(NAS_EMM_NOT_FIRST_TAU);

    /* TAU成功后，需清除LAU或者联合RAU标识 */
    NAS_LMM_SetEmmInfoLauOrComRauFlag(NAS_EMM_LAU_OR_COMBINED_RAU_NOT_HAPPENED);
    NAS_LMM_SetEmmInfoSrvccFlag(NAS_EMM_SRVCC_NOT_HAPPENED);
    NAS_LMM_SetEmmInfoT3412ExpCtrl(    NAS_EMM_T3412_EXP_NO);
    NAS_LMM_SetEmmInfoDrxNetCapChange(  NAS_EMM_NO);
    NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_NO);
    NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_NO);
    NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_NOCHANGE);
    NAS_LMM_SetEmmInfoPsState(GMM_LMM_GPRS_NOT_SUSPENSION);

    NAS_LMM_SetEmmInfoGConnState(GMM_LMM_GPRS_MM_STATE_IDLE);
    NAS_LMM_SetEmmInfoUConnState(GMM_LMM_PACKET_MM_STATE_PMM_IDLE);


    /*TAU成功后，表明异系统变换成功，清除触发的原因记录*/
     NAS_EMM_ClearResumeInfo();

     /* 清除UE无线能力变化记录标识 */
    NAS_EMM_ClearUeRadioAccCapChgFlag();

}
VOS_VOID NAS_EMM_TAU_ProcTauAcpCauseVal2
(
    VOS_VOID                           *pstRcvMsg
)
{
    /*清除TRACKING AREA UPDATING ATTEMPT计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /* 记录UE被原因值2拒绝过，之后不再发起联合ATTACH和联合TAU，直到关机或者拔卡 */
    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

    /* TAU成功的处理 */
    NAS_EMM_TAU_TauSuccProc(pstRcvMsg);

    /*TAU在IDLE态下触发，且不携带'ACTIVE'标志，则启动T3440，释放连接*/
    if((NAS_EMM_TAU_NO_BEARER_EST_REQ == NAS_EMM_TAU_GetEmmTauReqActiveCtrl())
       &&(NAS_EMM_CONN_DATA != NAS_EMM_TAU_GetEmmTauStartConnSta()))
    {
        /*设置TAU释放原因*/
        NAS_EMM_SetNormalServiceRelCause(NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_2);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        /*NAS_EMM_AdStateConvert( EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_MmcSendTauActionResultIndSucc();

        NAS_EMM_WaitNetworkRelInd();
    }
    else
    {
        /*clear 'active' flag */
        NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*转换EMM状态机MS_REG+SS_NORMAL_SERVICE*/
        /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        if(NAS_EMM_COLLISION_NONE != NAS_EMM_TAU_GetEmmCollisionCtrl())
        {
            /*判断流程冲突标志位，并启动相应流程*/
            NAS_EMM_TAU_TauAcpTaOnlyCollisionProc(pstRcvMsg);
        }
        else
        {
            NAS_EMM_MmcSendTauActionResultIndSucc();
        }
    }
}
VOS_VOID NAS_EMM_TAU_ProcTauAcpCauseVal18
(
    VOS_VOID                           *pstRcvMsg
)
{
   /* NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;*/

    /*清除TRACKING AREA UPDATING ATTEMPT计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*把当前PLMN加入到拒绝#18列表中*/
    NAS_EMMC_AddPlmnInRej18PlmnList(NAS_LMM_GetEmmInfoPresentPlmnAddr());

    /*设置拒绝18标识*/
    NAS_EMMC_SetRejCause18Flag(NAS_EMM_REJ_YES);

    /* TAU成功的处理 */
    NAS_EMM_TAU_TauSuccProc(pstRcvMsg);

    /*TAU在IDLE态下触发，且不携带'ACTIVE'标志，则启动T3440，释放连接*/
    if((NAS_EMM_TAU_NO_BEARER_EST_REQ == NAS_EMM_TAU_GetEmmTauReqActiveCtrl())
       &&(NAS_EMM_CONN_DATA != NAS_EMM_TAU_GetEmmTauStartConnSta()))
    {
        /*设置TAU释放原因*/
        NAS_EMM_SetNormalServiceRelCause(NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_18);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        /*NAS_EMM_AdStateConvert( EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_MmcSendTauActionResultIndSucc();

        NAS_EMM_WaitNetworkRelInd();
    }
    else
    {
        /*clear 'active' flag */
        NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*转换EMM状态机MS_REG+SS_NORMAL_SERVICE*/
        /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        if(NAS_EMM_COLLISION_NONE != NAS_EMM_TAU_GetEmmCollisionCtrl())
        {
            /*判断流程冲突标志位，并启动相应流程*/
            NAS_EMM_TAU_TauAcpTaOnlyCollisionProc(pstRcvMsg);
        }
        else
        {
            NAS_EMM_MmcSendTauActionResultIndSucc();

            /* 识别原因值18 disable L模的场景,释放链路 */
            /*NAS_EMM_RecogAndProc18Disable(pstTAUAcp->ucEMMCause);*/
        }
    }
}
VOS_VOID NAS_EMM_TAU_ProcTauAcpCauseVal161722
(
    VOS_VOID                           *pstRcvMsg
)
{
    /*NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;*/

    /*TAU ATTEMPT COUNT ++*/
    NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;

    /* TAU成功的处理 */
    NAS_EMM_TAU_TauSuccProc(pstRcvMsg);

    /*TAU在IDLE态下触发，且不携带'ACTIVE'标志，则启动T3440，释放连接*/
    if((NAS_EMM_TAU_NO_BEARER_EST_REQ == NAS_EMM_TAU_GetEmmTauReqActiveCtrl())
       &&(NAS_EMM_CONN_DATA != NAS_EMM_TAU_GetEmmTauStartConnSta()))
    {
        /*设置TAU释放原因*/
        NAS_EMM_SetNormalServiceRelCause(NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_161722);
        NAS_EMM_TAU_ProcCause161722TauAttemptCont();

        NAS_EMM_WaitNetworkRelInd();

        return ;
    }

    /*clear 'active' flag */
    NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);
	/* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
		
		NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {/* lihong00150010 emergency tau&service end */
        /*启动定时器TI_NAS_EMM_PTL_T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

		/* lihong00150010 emergency delete */
        /*修改状态：进入主状态REG子状态ATTACH_WAIT_ESM_BEARER_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        /*启动定时器TI_NAS_EMM_T3402*/
        NAS_LMM_StartPtlTimer(      TI_NAS_EMM_PTL_T3402);

		/* lihong00150010 emergency delete */
        /*修改状态：进入主状态REG子状态EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                    TI_NAS_EMM_PTL_T3402);
    }

    if(NAS_EMM_COLLISION_NONE != NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        /*判断流程冲突标志位，并启动相应流程*/
        NAS_EMM_TAU_TauAcpTaOnlyCollisionProc(pstRcvMsg);
        #if 0
        if (NAS_EMM_TAU_ATTEMPT_CNT_MAX == NAS_EMM_TAU_GetEmmTAUAttemptCnt())
        {
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        }
        #endif
    }
    else
    {
        NAS_EMM_MmcSendTauActionResultIndSucc();

        /* 识别原因值16,17,22 disable L模的场景,释放链路 */
        /*NAS_EMM_RecogAndProc161722Disable(pstTAUAcp->ucEMMCause);*/
        #if 0
        if (NAS_EMM_TAU_ATTEMPT_CNT_MAX == NAS_EMM_TAU_GetEmmTAUAttemptCnt())
        {
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        }
        #endif
    }
}
#if 0

VOS_VOID NAS_EMM_TAU_ProcTauAcpCauseValOther
(
    NAS_EMM_CN_CAUSE_ENUM_UINT8         ucRejCauseVal
)
{
    if((NAS_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_INVALID_MANDATORY_INF == ucRejCauseVal)       ||
       (NAS_LMM_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_IE_NONEXIST_NOTIMPLEMENTED == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_PROTOCOL_ERROR == ucRejCauseVal))
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_MAX);
    }
    else
    {
        NAS_EMM_TAU_GetEmmTAUAttemptCnt() ++;
    }

    NAS_EMM_TAU_ProcAbnormal();

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_ProcTauAcpCauseValOther:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        /*判断流程冲突标志位，并启动相应流程*/
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly,
                       (VOS_VOID*)NAS_EMM_NULL_PTR);
    }
	/* lihong00150010 emergency tau&service end */
    #if 0
    /* 如果启动了3402,清空TAU计数 */
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    }
    #endif

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
}
#endif
VOS_VOID    NAS_EMM_TAU_ProcTaUpdatedOnlyWithCause
(
    VOS_VOID                *pstRcvMsg
)
{
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;

    switch (pstTAUAcp->ucEMMCause)
    {
        case    NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS:
                NAS_EMM_TAU_ProcTauAcpCauseVal2(pstRcvMsg);


                break;

        case    NAS_LMM_CAUSE_CS_NOT_AVAIL:
                NAS_EMM_TAU_ProcTauAcpCauseVal18(pstRcvMsg);

                break;
        default:
                NAS_EMM_TAU_ProcTauAcpCauseVal161722(pstRcvMsg);

                break;

        #if 0
        case    NAS_LMM_CAUSE_MSC_UNREACHABLE:
        case    NAS_LMM_CAUSE_NETWORK_FAILURE:
        case    NAS_LMM_CAUSE_PROCEDURE_CONGESTION:
                NAS_EMM_TAU_ProcTauAcpCauseVal161722(pstRcvMsg);

                /*leili modify for isr begin*/
                NAS_EMM_TAU_SetISRAct(pstTAUAcp->ucEPSupdataRst);
                /*leili modify for isr end*/
                break;

        default :
                NAS_EMM_TAU_ProcTauAcpCauseValOther(pstTAUAcp->ucEMMCause);
                break;
        #endif
    }
    return;
}


VOS_VOID    NAS_EMM_TAU_ProcTaUpdatedOnlyNoCause( VOS_VOID *pstRcvMsg )
{
    NAS_EMM_TAU_ProcTauAcpCauseVal161722(pstRcvMsg);

    #if 0
    NAS_EMM_TAU_GetEmmTAUAttemptCnt() ++;

    NAS_EMM_TAU_ProcAbnormal();

    	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_ProcTaUpdatedOnlyNoCause:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        /*判断流程冲突标志位，并启动相应流程*/
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly,
                          (VOS_VOID*)NAS_EMM_NULL_PTR);
    }
	/* lihong00150010 emergency tau&service end */
    #if 0
    /* 如果启动了3402,清空TAU计数 */
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    }
    #endif

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    #endif

}
VOS_VOID    NAS_EMM_TAU_ProcTaUpdatedOnlySucc
(
    VOS_VOID                *pstRcvMsg
)
{
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;

    /*打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcTaUpdatedOnlySucc is entered!");

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU *)pstRcvMsg;

    if (NAS_EMM_BIT_SLCT == pstTAUAcp->ucBitOpEmmCau)
    {
        NAS_EMM_TAU_LOG1_NORM("NAS_EMM_ProcTaUpdatedOnlySucc EmmCause:",pstTAUAcp->ucEMMCause);
        NAS_EMM_TAU_ProcTaUpdatedOnlyWithCause(pstRcvMsg);
    }
    else
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_ProcTaUpdatedOnlySucc No EmmCause");
        NAS_EMM_TAU_ProcTaUpdatedOnlyNoCause(pstRcvMsg);
    }
    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_TAU_IsCombinedTauTaUpdatedOnly
 Description     : 判断是否为联合TAU，结果值为TA UPDATE ONLY的场景
 Input           : enEmmTauType-------------------TAU 类型
                   enEpsUpdateRslt----------------TAU结果值
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-08-23  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_TAU_IsCombinedTauTaUpdatedOnly
(
    MMC_LMM_TAU_TYPE_ENUM_UINT32            enEmmTauType,
    NAS_EMM_EPS_UPDATE_RST_ENUM_UINT8       enEpsUpdateRslt
)
{
    /* 判断是否为联合TAU */
    if ((NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING != enEmmTauType)
         && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI != enEmmTauType))
    {
        return  NAS_EMM_NO;
    }

    /* 判断结果值是否为TA UPDATE ONLY */
    if ((NAS_EMM_EPS_UPDATE_RST_TA_UPDATED != enEpsUpdateRslt)
        && (NAS_EMM_EPS_UPDATE_RST_TA_UPDATED_AND_ISR != enEpsUpdateRslt))
    {
        return  NAS_EMM_NO;
    }

    return  NAS_EMM_YES;
}

/*****************************************************************************
 Function Name   : NAS_EMM_TAU_SaveTauAcpIe
 Description     : 存储TAU ACCPET中携带的信元
 Input           : pMsgStru------------TAU ACCEPT消息指针
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-08-23  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_TAU_SaveTauAcpIe
(
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulTimerLen  = NAS_EMM_NULL;
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp   = NAS_EMM_NULL_PTR;
    NAS_EMM_TA_LIST_STRU               *pstTALIST   = NAS_EMM_NULL_PTR;

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU*)pMsgStru;

    NAS_EMM_TAU_SaveEpsUpdateRslt(pstTAUAcp->ucEPSupdataRst);

    /*更新全局变量中的GUTI*/
    NAS_EMM_TauUpdateGutiFromCnMsg(pstTAUAcp);

    if(EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpEPSstatus)
    {
        /*向ESM发送消息MM_ESM_BEARER_STATUS_IND*/
        NAS_EMM_TAU_SendEsmBearerStatusInd(pstTAUAcp);
    }

    if(EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpTaiList)
    {
        pstTALIST = (NAS_EMM_TA_LIST_STRU*)  &(pstTAUAcp->stTaiList);

        NAS_EMM_TAUSER_TaiListSet(pstTALIST);

        /*保存新的TAI List的同时更新ForbTA for Roaming和ForbTA for RPOS*/
        NAS_EMMC_UpdateForbTaList();
    }

    /*设置T3412定时器周期*/
    if(EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpT3412)
    {
        ulTimerLen = NAS_EMM_TimerValTransfer(pstTAUAcp->stT3412.ucUnit,
                                              pstTAUAcp->stT3412.ucTimerVal);
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3412,ulTimerLen);
    }

    /*设置Location area identification*/
    NAS_EMM_TAU_SetLai(pMsgStru);

    /*设置MS identity*/
    NAS_EMM_TAU_SetMsId(pMsgStru);

    /*设置EMM cause*/
    NAS_EMM_TAU_SetCnCause(pMsgStru);

    /*设置T3402定时器周期*/
    if(EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpT3402)
    {
         ulTimerLen = NAS_EMM_TimerValTransfer(pstTAUAcp->stT3402.ucUnit,
                                              pstTAUAcp->stT3402.ucTimerVal);
         NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3402,ulTimerLen);
    }
    else
    {
        /*如果TAU ACP消息中不携带T3402信元，则恢复默认值12min*/
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3402,NAS_LMM_TIMER_T3402_LEN);
    }

    /*leili modify for isr begin*/
    /*设置T3402定时器周期*/
    if(EMM_TAU_AD_BIT_SLCT == pstTAUAcp->ucBitOpT3423)
    {
         ulTimerLen = NAS_EMM_TimerValTransfer(pstTAUAcp->stT3423.ucUnit,
                                              pstTAUAcp->stT3423.ucTimerVal);
         NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3423,ulTimerLen);
    }
    else
    {
        /*如果ATTACH ACP消息中不携带T3423信元，则恢复默认值为T3412时长*/
        NAS_LMM_GetPtlTimerLen(TI_NAS_EMM_PTL_T3412, &ulTimerLen);
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3423,ulTimerLen);
    }
    /*leili modify for isr end*/
    /*设置Equivalent PLMNs*/
    NAS_EMM_TAU_SetEplmnList(pMsgStru);

    /*TAU ACP携带Emergency number list,更新Emergency number list到全局变量中*/
    NAS_EMM_SetEmergencyList(   pstTAUAcp->ucBitOpEmergencyList,
                                &(pstTAUAcp->stEmergencyNumList));

    /*TAU ACP携带EPS network feature support,更新EPS network feature support到全局变量中*/
    NAS_EMM_SetEpsNetFeature(   pstTAUAcp->ucBitOpEpsNetFeature,
                                pstTAUAcp->ucEpsNetFeature);

    /*TAU ACP携带Additional update result,更新Additional update result到全局变量中*/
    if(NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING == NAS_EMM_TAU_GetTAUtype())
    {
        /* 周期性TAU,若TAU ACP携带Additional update result,则更新Additional update result到全局变量中*/
        if(NAS_EMM_BIT_SLCT == pstTAUAcp->ucBitOpAdditionalUpRslt)
        {
            NAS_EMM_SetAddUpdateRslt(   pstTAUAcp->ucBitOpAdditionalUpRslt,
                                        pstTAUAcp->ulAdditionUpRslt);
        }
    }
    else
    {
        if ((NAS_EMM_EPS_UPDATE_RST_COMBINED_TA_LA_UPDATED != pstTAUAcp->ucEPSupdataRst)
            && (NAS_EMM_EPS_UPDATE_RST_COMBINED_TA_LA_UPDATED_AND_ISR != pstTAUAcp->ucEPSupdataRst))
        {
            /* 非周期性TAU ACP为TA UPDAE ONLY 则ulAdditionUpRslt保存为NAS_LMM_ADDITIONAL_UPDATE_NOT_ATTACHED */
            NAS_EMM_SetAddUpdateRslt(NAS_EMM_BIT_SLCT, NAS_LMM_ADDITIONAL_UPDATE_NOT_ATTACHED);
        }
        else
        {
            /* 非周期性TAU ACP为COMBINED TA UPDAE 则更新Additional update result到全局变量中 */
            NAS_EMM_SetAddUpdateRslt(   pstTAUAcp->ucBitOpAdditionalUpRslt,
                                        pstTAUAcp->ulAdditionUpRslt);
        }
    }

    /* lihong00150010 ims begin */
    /* 通知MMC紧急呼号码列表，网侧是否支持IMS VOICE和紧急呼，以及LTE的CS域能力 */
    /* NAS_EMM_SendGmmInfoChangeNotify(); */
    NAS_EMM_SendMmcInfoChangeNotify();
    /*NAS_EMM_SendMmInfoChangeNotify();*/
    /* lihong00150010 ims end */
}

/*****************************************************************************
 Function Name   : NAS_EMM_TAU_SetTauRegDomain
 Description     : 收到TAU ACCPET消息时设置注册域
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-08-23  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_TAU_SetTauRegDomain
(
    NAS_EMM_EPS_UPDATE_RST_ENUM_UINT8   enEPSupdataRst
)
{
    if((NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING == NAS_EMM_TAU_GetTAUtype())
        && ((NAS_EMM_EPS_UPDATE_RST_TA_UPDATED == enEPSupdataRst)
         || (NAS_EMM_EPS_UPDATE_RST_TA_UPDATED_AND_ISR == enEPSupdataRst)))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_TAU_SetTauRegDomain: periodic TAU not needed set regdomains:");
        return;
    }
    if ((NAS_EMM_EPS_UPDATE_RST_TA_UPDATED == enEPSupdataRst)
     || (NAS_EMM_EPS_UPDATE_RST_TA_UPDATED_AND_ISR == enEPSupdataRst))
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }
    else
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_CS_PS);
    }
}
/*leili modify for isr begin*/

VOS_UINT32  NAS_EMM_GetUeSuppISR(VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_EMM_MS_NET_CAP_STRU             *pstMsNetCap;
    VOS_UINT32                          ulRslt = NAS_EMM_BIT_NO_SLCT;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pstMsNetCap = NAS_LMM_GetEmmInfoMsNetCapAddr();
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpMsNetCap)
    {
        if (NAS_EMM_BIT_SLCT == ((pstMsNetCap->aucMsNetCap[2] >> NAS_EMM_MOVEMENT_4_BITS)
                                    & NAS_EMM_LOW_1_BITS_F))
        {
            ulRslt = NAS_EMM_BIT_SLCT;
        }

    }

    return ulRslt;
}



VOS_UINT32 NAS_EMM_IsAnnexP5AConditionSatisfied(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    /*目前GU和L公用voice domain preference for E-UTRAN,后续不公用后修改*/

    NAS_LMM_PUBM_LOG2_INFO("NAS_EMM_IsAnnexP5AConditionSatisfied:gu nw IMS VOICE =,lte nw ims voice=",
                                  NAS_MML_GetGUNwImsVoiceSupportFlg(),
                                  NAS_MML_GetLteNwImsVoiceSupportFlg());

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if ((NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetGUNwImsVoiceSupportFlg())
        &&(NAS_MML_NW_IMS_VOICE_NOT_SUPPORTED == NAS_MML_GetLteNwImsVoiceSupportFlg())
        &&((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
            && (NAS_LMM_VOICE_DOMAIN_CS_ONLY != pstPubInfo->ulVoiceDomain)))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_IsAnnexP5AConditionSatisfied: B)condition is Satisfied");
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
VOS_UINT32 NAS_EMM_IsAnnexP5BConditionSatisfied(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    NAS_LMM_PUBM_LOG2_INFO("NAS_EMM_IsAnnexP5BConditionSatisfied:gu nw IMS VOICE =,lte nw ims voice=",
                                  NAS_MML_GetGUNwImsVoiceSupportFlg(),
                                  NAS_MML_GetLteNwImsVoiceSupportFlg());

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if ((NAS_MML_NW_IMS_VOICE_NOT_SUPPORTED == NAS_MML_GetGUNwImsVoiceSupportFlg())
        &&(NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetLteNwImsVoiceSupportFlg())
        &&((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
            && (NAS_LMM_VOICE_DOMAIN_CS_ONLY != pstPubInfo->ulVoiceDomain)))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_IsAnnexP5BConditionSatisfied: B)condition is Satisfied");
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
#if 0

VOS_UINT32 NAS_EMM_IsAnnexP5CConditionSatisfied(VOS_VOID)
{

    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    NAS_LMM_PUBM_LOG2_INFO("NAS_EMM_IsAnnexP5CConditionSatisfied:gu nw IMS VOICE =,lte nw ims voice=",
                                  NAS_MML_GetGUNwImsVoiceSupportFlg(),
                                  NAS_MML_GetLteNwImsVoiceSupportFlg());

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    /*the voice domain preferences for UTRAN没有实现*/
    if ((NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetGUNwImsVoiceSupportFlg())
        &&(NAS_MML_NW_IMS_VOICE_SUPPORTED == NAS_MML_GetLteNwImsVoiceSupportFlg())
        &&((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
            && (NAS_LMM_VOICE_DOMAIN_CS_ONLY != pstPubInfo->ulVoiceDomain)))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_IsAnnexP5CConditionSatisfied: C)condition is Satisfied");
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
#endif
VOS_UINT32  NAS_EMM_IsAnnexP5ConditionSatisfied(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    /*C)条件没有实现*/
    if ((MMC_LMM_IMS_VOICE_CAP_AVAILABLE == pstPubInfo->enImsaVoiceCap)
        &&(VOS_TRUE == NAS_MML_GetImsVoiceMMEnableFlg())
        &&( (NAS_EMM_YES ==NAS_EMM_IsAnnexP5AConditionSatisfied())
            ||(NAS_EMM_YES == NAS_EMM_IsAnnexP5BConditionSatisfied())))
    {

        return NAS_EMM_YES;
    }


    return NAS_EMM_NO;
}
#if 0
VOS_UINT32  NAS_EMM_ChangeRegAreaOfIMS(VOS_VOID)
{
    NAS_EMM_PUB_INFO_STRU              *pstPubInfo;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    /*IMS已注册，L模IMS voice over PS session改变或GU->L时IMS voice over PS session
    改变*/
    if (NAS_LMM_SYS_CHNG_ORI_BUTT != NAS_EMM_GetResumeOri())
    {

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
#endif


VOS_VOID  NAS_EMM_TAU_SetISRAct
(
    NAS_EMM_EPS_UPDATE_RST_ENUM_UINT8   enEPSupdataRst
)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_TAU_SetISRAct: GET MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*UE不支持ISR能力*/
    if (NAS_EMM_BIT_NO_SLCT == NAS_EMM_GetUeSuppISR())
    {
        /*发送 LMM_MMC_TIN_TYPE_IND*/
        NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        /*更新承载的ISR标识*/
        NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());

        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_TAU_SetISRAct,UE NOT SUPPORT ISR: UPDATE MML PS BEARER INFO:");
        NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
        return;
    }

    if ((NAS_EMM_EPS_UPDATE_RST_TA_UPDATED == enEPSupdataRst) ||
        (NAS_EMM_EPS_UPDATE_RST_COMBINED_TA_LA_UPDATED == enEPSupdataRst))
    {
        /*发送 LMM_MMC_TIN_TYPE_IND*/
        NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);

        /*更新承载的ISR标识*/
        NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());

    }
    else
    {
        /*满足annex P.5,设置TIN值为GUTI*/
        if ((NAS_LMM_RSM_SYS_CHNG_DIR_W2L == NAS_EMM_GetResumeDir())
            && (NAS_EMM_YES == NAS_EMM_IsAnnexP5ConditionSatisfied()))
        {
            /*发送 LMM_MMC_TIN_TYPE_IND*/
            NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);

            /*更新承载的ISR标识*/
            NAS_EMM_UpdateBearISRFlag(NAS_MML_GetPsBearerCtx());

            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_TAU_SetISRAct,UE SUPPORT ISR: UPDATE MML PS BEARER INFO:");
            NAS_COMM_PrintArray(        NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
            return;
        }
        #if 0
        /*IMS已注册且IMS voice over PS session改变，设置ITN值为GUTI*/
        if(NAS_EMM_YES == NAS_EMM_ChangeRegAreaOfIMS())
        {
            /*发送 LMM_MMC_TIN_TYPE_IND*/
            NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);

            /*更新承载的ISR标识*/
            NAS_EMM_UpdateBearISRFlag(NAS_MML_GetPsBearerCtx());
            return;
        }
        #endif
        if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
        {
            if (GMM_LMM_TIMER_EXP == NAS_EMM_GetEmmInfoT3312State())
            {
                 /*发送 LMM_MMC_TIN_TYPE_IND*/
                NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);

                /*更新承载的ISR标识*/
                NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
            }/*PC REPLAY MODIFY BY LEILI END*/
            else
            {
                /*发送 LMM_MMC_TIN_TYPE_IND*/
                NAS_EMM_SetTinType(MMC_LMM_TIN_RAT_RELATED_TMSI);
            }
        }
    }
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_TAU_SetISRAct,UE SUPPORT ISR: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
    return;
}
/*leili modify for isr end*/

VOS_VOID    NAS_EMM_TAU_RcvTAUAcp(VOS_VOID *pMsgStru)
{
    NAS_EMM_CN_TAU_ACP_STRU            *pstTAUAcp      = NAS_EMM_NULL_PTR;

    LRRC_LMM_SYS_INFO_IND_STRU          stRrcSysInfo = {0};
    NAS_LMM_NETWORK_INFO_STRU          *pMmNetInfo   = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_Tau_RcvTAUAcp is entered.");

    pstTAUAcp = (NAS_EMM_CN_TAU_ACP_STRU*)pMsgStru;

    pMmNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /*停止T3430*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    /* 存储TAU ACCPET中携带的信元 */
    NAS_EMM_TAU_SaveTauAcpIe(pMsgStru);

    /* 如果GUTI中的PLMN与当前PLMN不同，将当前PLMN更新成GUTI中的，同时给MMC报系统消息 */
    if ( NAS_EMM_SUCC == NAS_EMM_UpdatePresentPlmnInfo() )
    {
        stRrcSysInfo.stSpecPlmnIdList.ulSuitPlmnNum                   = 1;
        stRrcSysInfo.stSpecPlmnIdList.astSuitPlmnList[0].aucPlmnId[0] = pMmNetInfo->stPresentNetId.stPlmnId.aucPlmnId[0];
        stRrcSysInfo.stSpecPlmnIdList.astSuitPlmnList[0].aucPlmnId[1] = pMmNetInfo->stPresentNetId.stPlmnId.aucPlmnId[1];
        stRrcSysInfo.stSpecPlmnIdList.astSuitPlmnList[0].aucPlmnId[2] = pMmNetInfo->stPresentNetId.stPlmnId.aucPlmnId[2];
        stRrcSysInfo.stTac.ucTac                                      = pMmNetInfo->stPresentNetId.stTac.ucTac;
        stRrcSysInfo.stTac.ucTacCont                                  = pMmNetInfo->stPresentNetId.stTac.ucTacCnt;
        stRrcSysInfo.ulCellId                                         = pMmNetInfo->stPresentNetId.ulCellId;
        stRrcSysInfo.enCellStatusInd                                  = LRRC_LNAS_CELL_STATUS_NORMAL;
        stRrcSysInfo.aulLteBand[0]                                    = pMmNetInfo->aulLteBand[0];
        stRrcSysInfo.aulLteBand[1]                                    = pMmNetInfo->aulLteBand[1];

        /*发送LMM_MMC_SYS_INFO_IND消息*/
        NAS_EMMC_SendMmcSysInfo(&stRrcSysInfo);
    }


    NAS_EMM_TAU_SetTauRegDomain(pstTAUAcp->ucEPSupdataRst);

    /*联合TAU的结果值为TA UPDAE ONLY处理*/
    if (NAS_EMM_YES == NAS_EMM_TAU_IsCombinedTauTaUpdatedOnly(  NAS_EMM_TAU_GetTAUtype(),
                                                                pstTAUAcp->ucEPSupdataRst))
    {
        NAS_EMM_TAU_ProcTaUpdatedOnlySucc(pMsgStru);

        return ;
    }

    /*重置TAU尝试计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /* TAU成功的处理 */
    NAS_EMM_TAU_TauSuccProc(pMsgStru);

    /*TAU在IDLE态下触发，且不携带'ACTIVE'标志，则启动T3440，释放连接*/
    if((NAS_EMM_TAU_NO_BEARER_EST_REQ == NAS_EMM_TAU_GetEmmTauReqActiveCtrl())
       &&(NAS_EMM_CONN_DATA != NAS_EMM_TAU_GetEmmTauStartConnSta()))
    {
        /*设置TAU释放原因*/
        NAS_EMM_SetNormalServiceRelCause(NAS_EMM_NORMALSERVICE_REL_CAUSE_NULL);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        /*NAS_EMM_AdStateConvert( EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_MmcSendTauActionResultIndSucc();

        NAS_EMM_WaitNetworkRelInd();
    }
    else
    {
        /*clear 'active' flag */
        NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);
		/* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /*转换EMM状态机MS_REG+SS_NORMAL_SERVICE*/
        /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/
		/* lihong00150010 emergency tau&service end */
        if(NAS_EMM_COLLISION_NONE != NAS_EMM_TAU_GetEmmCollisionCtrl())
        {
            /*判断流程冲突标志位，并启动相应流程*/
            NAS_EMM_TAU_TauAcpCollisionProc();
        }
        else
        {
            /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
            NAS_EMM_MmcSendTauActionResultIndSucc();

            /* 识别SMS only或者CS fallback not preferred disable L模的场景,释放链路 */
            /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/
        }
    }


    return;

}





VOS_UINT32 NAS_EMM_MsTauInitSsWaitCNCnfMsgTAUAcp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                                  ulRslt              = NAS_EMM_FAIL;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_NORM( "Nas_Emm_MsTauInitSsWaitCNCnfMsgTAUAcp is entered.");

    (VOS_VOID)(ulMsgId);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*停止T3416，删除RAND,RES*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*调用消息处理函数*/
    NAS_EMM_TAU_RcvTAUAcp(pMsgStru);

    return NAS_LMM_MSG_HANDLED;

}
VOS_VOID  NAS_EMM_TAU_SpecialRelCauseCollisionProc
(
    NAS_LMM_SEND_TAU_RESULT_ACT_FUN     pfTauRslt,
    const VOS_VOID                     *pvPara
)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_SpecialRelCauseCollisionProc is entered.");

    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                /* 上报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

                NAS_EMM_TAU_CollisionDetachProc();
                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_SpecialRelCauseCollisionProc:Process IMSI Detach");

                    /* 本地DETACH*/
                    NAS_LMM_DeregReleaseResource();
                }
                else
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_SpecialRelCauseCollisionProc:Process IMSI Detach");

                    /* IMSI DETACH后释放资源 */
                    NAS_LMM_ImsiDetachReleaseResource();
                }
                break;

        case    NAS_EMM_COLLISION_SERVICE    :
                NAS_EMM_TAU_CollisionServiceProc(pfTauRslt,pvPara,NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
                break;

        default :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_SpecialRelCauseCollisionProc : NO Emm Collision.");

                /* 上报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);
                break;
    }
    return;
}


VOS_VOID  NAS_EMM_TAU_RelCause1315CollisionProc
(
    NAS_LMM_SEND_TAU_RESULT_ACT_FUN     pfTauRslt,
    const VOS_VOID                     *pvPara
)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_RelCause1315CollisionProc is entered.");

    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                /* 上报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_RelCause1315CollisionProc:Process IMSI Detach");

                    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
                    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                                EMM_SS_DEREG_LIMITED_SERVICE,
                                                TI_NAS_EMM_STATE_NO_TIMER);
                    NAS_EMM_TAU_CollisionDetachProc();

                    /* 本地DETACH*/
                    NAS_LMM_DeregReleaseResource();
                }
                else
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_RelCause1315CollisionProc:Process IMSI Detach");

                    /* 设置注册域为PS */
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
                    NAS_EMM_TAU_CollisionDetachProc();

                    /* IMSI DETACH后释放资源 */
                    NAS_LMM_ImsiDetachReleaseResource();
                }
                break;

        case    NAS_EMM_COLLISION_SERVICE    :
                NAS_EMM_TAU_CollisionServiceProc(pfTauRslt, pvPara,NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
                break;

        default :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_RelCause1315CollisionProc : NO Emm Collision.");

                /* 上报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);
                break;
    }
    return;
}


VOS_VOID  NAS_EMM_TAU_CollisionDetachProc( VOS_VOID )
{
    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_CollisionDetachProc is entered.");

    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_DETACH_REQ))
    {
        /**向APP 发送APP DETACH CNF(SUCCESS)*******/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
    }

    /*清除冲突标志*/
    NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_NONE);

    /*clear 'active' flag */
    NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);

    /* 清除联合DETACH被TAU打断标识 */
    NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_NO);

    /* 如果冲突的DETACH类型不是IMSI DEACH，则还需停止定时器，通知ESM进入DETACHED态 */
    if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
    {
        /*停所有EMM(MMC除外)定时器*/
        NAS_LMM_StopAllEmmStateTimer();

        NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);

        /*向ESM 发送STATUS IND(DETACH)*/
        NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        /*清空TAU计数器*/
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt( NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    }

    return;
}
VOS_VOID  NAS_EMM_TAU_CollisionServiceProc
(
    NAS_LMM_SEND_TAU_RESULT_ACT_FUN     pfTauRslt,
    const VOS_VOID                     *pvPara,
    VOS_UINT8                           ucRatChange
)
{
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_SMS_EST_REQ:

            /* 上报TAU结果 */
            NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);
            NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_OTHERS);
            NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);

            /* 通知RABM 这次SERVICE 失败 */
            NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
            break ;

        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
            if (NAS_EMM_TRANSFER_RAT_ALLOW == ucRatChange)
            {
                 /* 上报TAU结果, 顺序一定要在给MM发终止之后 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

                NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
            }
            else
            {
                NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);

                /* 上报TAU结果, 顺序一定要在给MM发终止之后 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);
            }
            break;
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:

            /* NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);*/

            /* 上报TAU结果, 顺序一定要在给MM发终止之后 */
            NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

            /* 给MMC上报SERVICE失败触发搜网去GU */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
            break ;

        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:

            /* 上报TAU结果, 顺序一定要在给MMC发SER结果之前*/
            NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

            /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

            break ;

        default:
            /* 上报TAU结果 */
            NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

            /* 通知RABM 这次SERVICE 失败 */
            NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
            break ;
    }

    /* 清除冲突标志 */
    NAS_EMM_TAU_SaveEmmCollisionCtrl( NAS_EMM_COLLISION_NONE);

    /* 清除Active Flag */
    NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);

    /**清除保存数据的标志和数据区**************/
    NAS_EMM_SerClearEsmDataBuf();

    return;

}


VOS_VOID  NAS_EMM_TAU_RelIndCollisionProc
(
    NAS_LMM_SEND_TAU_RESULT_ACT_FUN     pfTauRslt,
    const VOS_VOID                     *pvPara
)
{

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_RelIndCollisionProc is entered.");

    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH:

                /* 给MMC报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {

                    /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
                    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                                EMM_SS_DEREG_NORMAL_SERVICE,
                                                TI_NAS_EMM_STATE_NO_TIMER);

                    /*清空TAU相关全局变量，通知ESM，APP去注册*/
                    NAS_EMM_TAU_CollisionDetachProc();

                    /*本地DETACH释放资源:动态内存、赋初值 */
                    NAS_LMM_DeregReleaseResource();
                }
                else
                {
                    /* 如果是IMSI DETACH类型，则本地CS域去注册，继续处理TAU过程中收到RRC链路释放 */

                    /* 设置注册域为PS */
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

                    /*清空TAU相关全局变量，通知ESM，APP去注册*/
                    NAS_EMM_TAU_CollisionDetachProc();

                    /* IMSI DETACH后释放资源 */
                    NAS_LMM_ImsiDetachReleaseResource();
                }
                break;

        case    NAS_EMM_COLLISION_SERVICE:

                /* 给MMC报TAU结果在冲突中进行处理 */
                NAS_EMM_TAU_CollisionServiceProc(pfTauRslt,pvPara,NAS_EMM_TRANSFER_RAT_ALLOW);
                break;

        default :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_RelIndCollisionProc : NO Emm Collision.");

                /* 给MMC报TAU结果 */
                NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt,pvPara);

                break;
    }

    return;
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




