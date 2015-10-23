


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasMmlCtx.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMATTACH_C
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
NAS_EMM_ATTACH_CTRL_STRU                g_stEmmAttCtrl;

/*****************************************************************************
  3 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMM_AttachInit()
{
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AttachInit                START INIT...");

    /* 初始化ATTACH全局变量 */
    NAS_LMM_MEM_SET(&g_stEmmAttCtrl, 0, sizeof(g_stEmmAttCtrl));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/
    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/

    return;
}
VOS_VOID  NAS_EMM_FreeAttDyn( VOS_VOID )
{
    /*暂时无操作*/

    return;
}
VOS_VOID     NAS_EMM_Attach_ClearResourse(VOS_VOID)
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_Attach_ClearResourse is entered");

    /*动态内存释放*/
    NAS_EMM_FreeAttDyn();

    /* 停止ATTACH定时器 */
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*赋初值*/
    NAS_LMM_MEM_SET(NAS_EMM_GLO_AD_GetAttCtrlAddr(), 0, sizeof(NAS_EMM_ATTACH_CTRL_STRU));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/

    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/
    NAS_EMM_SetAttType();

    return;
}

/*****************************************************************************
 Function Name   :  NAS_EMM_Attach_SuspendInitClearResourse
 Description     :  在挂起时清理ATTACH资源
 Input           :

 Return          :

 History         :
    1.FTY         2012-02-21  Draft Enact

*****************************************************************************/
VOS_VOID     NAS_EMM_Attach_SuspendInitClearResourse(VOS_VOID)
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_Attach_SuspendInitClearResourse is entered");

    /*动态内存释放*/
    NAS_EMM_FreeAttDyn();

    /* 停止ATTACH定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*赋初值*/
    NAS_LMM_MEM_SET(NAS_EMM_GLO_AD_GetAttCtrlAddr(), 0, sizeof(NAS_EMM_ATTACH_CTRL_STRU));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/

    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/
    NAS_EMM_SetAttType();

    return;
}
VOS_VOID  NAS_EMM_SendRrcClearBuffNotify(VOS_VOID )
{
    LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU           *pstCleBuffNotifyIndMsg;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_SendRrcClearBuffNotify is entered");

    /* 申请DOPRA消息 */
    pstCleBuffNotifyIndMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstCleBuffNotifyIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendRrcClearBuffNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstCleBuffNotifyIndMsg,0,sizeof(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));

    /* 打包VOS消息头 */
    NAS_EMM_SET_RRC_MSG_HEADER((pstCleBuffNotifyIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));

    /* 填充消息ID */
    pstCleBuffNotifyIndMsg->enMsgId        = ID_LRRC_LMM_CLEAR_BUFF_NOTIFY;

    /* 发送DOPRA消息 */
    NAS_LMM_SEND_MSG(pstCleBuffNotifyIndMsg);

    return;
}
VOS_VOID  NAS_EMM_AdStateConvert(NAS_EMM_MAIN_STATE_ENUM_UINT16 ucMs,
                                   NAS_EMM_SUB_STATE_ENUM_UINT16 ucSs,
                                   NAS_EMM_STATE_TI_ENUM_UINT16 ucStaTId)
{
    NAS_EMM_FSM_STATE_STRU              stDestState;

    stDestState.enFsmId                 = NAS_LMM_PARALLEL_FSM_EMM;
    stDestState.enMainState             = ucMs;
    stDestState.enSubState              = ucSs;
    stDestState.enStaTId                = ucStaTId;
    NAS_LMM_StaTransProc(stDestState);

}


VOS_VOID    NAS_EMM_EsmSendRelInd()
{
    EMM_ESM_REL_IND_STRU                 *pEmmEsmRelIndMsg;


    /*申请消息内存*/
    pEmmEsmRelIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_REL_IND_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pEmmEsmRelIndMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR(" NAS_EMM_EsmSendRelInd: MSG ALLOC ERR!");
        return;

    }

    /*构造ID_EMM_ESM_REL_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_ESM_MSG_HEADER(      pEmmEsmRelIndMsg,
                                        (sizeof(EMM_ESM_REL_IND_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER));

    /*填充消息ID*/
    pEmmEsmRelIndMsg->ulMsgId            = ID_EMM_ESM_REL_IND;

    /*向ESM发送ID_EMM_ESM_REL_IND消息*/
    NAS_LMM_SEND_MSG(                   pEmmEsmRelIndMsg);

    return;


}
VOS_UINT32  NAS_EMM_IntraAttReqChk(const NAS_LMM_INTRA_ATTACH_REQ_STRU* pMsgMsg)
{
    /*消息内容检查*/
    if (ID_NAS_LMM_INTRA_ATTACH_REQ == pMsgMsg->ulMsgId)
    {
        return  NAS_EMM_PARA_VALID;

    }
    else
    {
        return  NAS_EMM_PARA_INVALID;

    }
}

VOS_VOID    NAS_EMM_AtmpCntLess5Proc()
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AtmpCntLess5Proc is entered");

    /*启动定时器T3411*/
    NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

}
VOS_VOID    NAS_EMM_AtmpCntEqual5Proc()
{

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AtmpCntEqual5Proc is entered");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置update status为EU2*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
    /*启动定时器T3402*/
    NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

    /*重设attach attempt counter*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/

    /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                        TI_NAS_EMM_STATE_NO_TIMER);


}


VOS_VOID    NAS_EMM_AttSendEsmResult( EMM_ESM_ATTACH_STATUS_ENUM_UINT32 attStatus )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AttSendEsmResult is entered");

    /*判断触发原因*/
    if (EMM_ATTACH_CAUSE_ESM == NAS_EMM_GLO_AD_GetAttCau())
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(          attStatus);

        if (EMM_ESM_ATTACH_STATUS_DETACHED == attStatus)
        {
            /*clear global:EpsContextStatusChange*/
            NAS_EMM_GetEpsContextStatusChange() = NAS_EMM_EPS_BEARER_STATUS_NOCHANGE;
        }
    }
    else
    {
        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_EsmSendStatResult(      attStatus);
    }

    return;
}


VOS_VOID  NAS_EMM_UpdateCnAttTypeOfUserReq( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_UpdateCnAttTypeOfUserReq is entered");

    /* 判断是否被原因值2拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_LMM_GetEmmInfoRejCause2Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /* 判断是否被原因值18拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_EMMC_GetRejCause18Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /*用户下发CS_ONLY,但UE模式为PS1,PS2或CS/PS1,CS/PS2,在收到用户请求消息时会回复，
    此处不考虑*/
    if (NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        if(MMC_LMM_ATT_REQ_TYPE_PS_ONLY == NAS_EMM_GLO_GetAttReqType())
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
            return;
        }
        if ((MMC_LMM_ATT_REQ_TYPE_CS_PS == NAS_EMM_GLO_GetAttReqType()) &&
            (NAS_EMM_YES== NAS_EMM_IsCsPsUeMode()))
        {

            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);
        }
        else
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        }
    }
    else
    {
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        /*L单模下,操作模式CS+PS，用户下发PS_ONLY或CS+PS且CS允许注册,触发联合ATTACH,
        其他情况触发PS ATTACH*/
        if((NAS_EMM_YES== NAS_EMM_IsCsPsUeMode()) &&
            (NAS_EMM_AUTO_ATTACH_ALLOW == NAS_EMM_GetCsAttachAllowFlg()))
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);

        }
        else
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);

        }
        /*PC REPLAY MODIFY BY LEILI END*/
    }
    return;
}
VOS_VOID  NAS_EMM_UpdateCnAttTypeOfNotUserReq( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_UpdateCnAttTypeOfNotUserReq is entered");

    /* 判断是否被原因值2拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_LMM_GetEmmInfoRejCause2Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /* 判断是否被原因值18拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_EMMC_GetRejCause18Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
     /* 判断是否允许CS域注册 */
    if (NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    if ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()) ||
        (NAS_LMM_UE_CS_PS_MODE_2 == NAS_LMM_GetEmmInfoUeOperationMode()))
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);
    }
    else
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
    }
    return;
}
VOS_VOID  NAS_EMM_SetAttType( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SetAttType is entered");

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_EMERGENCY);
        return ;
    }

    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_UpdateCnAttTypeOfUserReq();
    }
    else
    {
        NAS_EMM_UpdateCnAttTypeOfNotUserReq();
    }
    return;
}


VOS_VOID  NAS_EMM_SetUeUsageSetting (VOS_VOID )
{
    NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_SetUeUsageSetting: ",
                             NAS_EMM_GLO_AD_GetUsgSetting());

    /* 第一阶段，只支持数据中心，开机时已经赋初值，此处不再变化 */

    /*
    if ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()) ||
        (NAS_LMM_UE_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()))
    {
        NAS_EMM_GLO_AD_SetUsgSetting(EMM_SETTING_VOICE_CENTRIC);
    }
    else
    {
        NAS_EMM_GLO_AD_SetUsgSetting(EMM_SETTING_DATA_CENTRIC);
    }
    */
    return;
}


VOS_UINT32  NAS_EMM_GetCsService (VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU                  *pstPubInfo      = VOS_NULL_PTR;
    NAS_LMM_CS_SERVICE_ENUM_UINT32          ulCsService     = NAS_LMM_CS_SERVICE_BUTT;
    /* lihong00150010 csfb begin */

    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8   enCsServiceCfg  = NAS_MML_LTE_SUPPORT_BUTT;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    enCsServiceCfg  = NAS_EMM_GetLteCsServiceCfg();
    /*PC REPLAY MODIFY BY LEILI END*/
    NAS_EMM_ATTACH_LOG1_INFO("MML CS SERVICE: ", enCsServiceCfg);

    pstPubInfo      = NAS_LMM_GetEmmInfoAddr();

    if (NAS_MML_LTE_SUPPORT_BUTT != enCsServiceCfg)
    {
        pstPubInfo->bitOpCsService  = NAS_EMM_BIT_SLCT;

        pstPubInfo->ulCsService     = enCsServiceCfg;
        /*pstPubInfo->ulCsService = NAS_LMM_ChangeCsServiceMmlToLmm(enCsServiceCfg);*/
    }
    else
    {
        pstPubInfo->bitOpCsService = NAS_EMM_BIT_NO_SLCT;
    }

    /* lihong00150010 csfb end */
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpCsService)
    {

        ulCsService = pstPubInfo->ulCsService;
    }

    return ulCsService;
}
VOS_UINT32  NAS_EMM_GetVoiceDomain(VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_LMM_VOICE_DOMAIN_ENUM_UINT32    ulVoiceDomain = NAS_LMM_VOICE_DOMAIN_BUTT;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
    {

        ulVoiceDomain = pstPubInfo->ulVoiceDomain;
    }

    return ulVoiceDomain;
}



VOS_UINT32  NAS_EMM_GetSuppSrvccToGU(VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_EMM_MS_NET_CAP_STRU             *pstMsNetCap;
    VOS_UINT32                          ulRslt = NAS_EMM_BIT_NO_SLCT;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pstMsNetCap = NAS_LMM_GetEmmInfoMsNetCapAddr();
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpMsNetCap)
    {
        if (NAS_EMM_BIT_SLCT == ((pstMsNetCap->aucMsNetCap[2] >> NAS_EMM_AD_MOVEMENT_3_BITS)
                                    & NAS_EMM_LOW_1_BITS_F))
        {
            ulRslt = NAS_EMM_BIT_SLCT;
        }

    }

    return ulRslt;
}


VOS_VOID  NAS_EMM_ProcCause161722TauAttemptCont(VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_ProcCause161722TauAttemptCont is entered");

    if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        /*启动定时器TI_NAS_EMM_PTL_T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

		/* lihong00150010 emergency delete */

        /*修改状态：进入主状态REG子状态ATTACH_WAIT_ESM_BEARER_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }
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

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();
        /* 识别EPS ONLY,原因值为16,17,22 disable L模的场景,释放链路 */
        /*NAS_EMM_RecogAndProc161722Disable(NAS_EMM_GLO_AD_GetEmmCau());*/
        /*NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);*/
    }

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SetAttachRegDomain
 Description     : ATTACH成功时设置注册域
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-08-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SetAttachRegDomain( VOS_VOID )
{
    if (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt())
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }
    else
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_CS_PS);
    }
}

/*****************************************************************************
 Function Name   : NAS_EMM_ProcRrcDataCnfEpsSuccOnly
 Description     : 收到RRC_MM_DATA_CNF后且结果为EPS ONLY成功的处理
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-09-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_ProcRrcDataCnfEpsSuccOnly( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    if (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == NAS_EMM_GLO_AD_GetEmmCau())
    {
        /* 记录UE被原因值2拒绝过，之后不再发起联合ATTACH和联合TAU，直到关机或者拔卡 */
        NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        return ;
    }


    if (NAS_LMM_CAUSE_CS_NOT_AVAIL == NAS_EMM_GLO_AD_GetEmmCau())
    {
        /*把当前PLMN加入到拒绝#18列表中*/
        NAS_EMMC_AddPlmnInRej18PlmnList(NAS_LMM_GetEmmInfoPresentPlmnAddr());

        /*设置拒绝18标识*/
        NAS_EMMC_SetRejCause18Flag(NAS_EMM_REJ_YES);

        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /* 识别EPS ONLY,原因值为18 disable L模的场景,释放链路 */
        /*NAS_EMM_RecogAndProc18Disable(NAS_EMM_GLO_AD_GetAttRejCau());*/
        return;

    }

    NAS_EMM_ProcCause161722TauAttemptCont();

    return;

    #if 0
    if ((NAS_LMM_CAUSE_MSC_UNREACHABLE == NAS_EMM_GLO_AD_GetAttRejCau()) ||
        (NAS_LMM_CAUSE_NETWORK_FAILURE == NAS_EMM_GLO_AD_GetAttRejCau()) ||
        (NAS_LMM_CAUSE_PROCEDURE_CONGESTION == NAS_EMM_GLO_AD_GetAttRejCau()))
    {
        NAS_EMM_ProcCause161722TauAttemptCont();
        return ;
    }

    /*把当前PLMN加入到拒绝#18列表中*/
    NAS_EMMC_AddPlmnInRej18PlmnList(NAS_LMM_GetEmmInfoPresentPlmnAddr());

    /*设置拒绝18标识*/
    NAS_EMMC_SetRejCause18Flag(NAS_EMM_REJ_YES);

    /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*ATTACH完成向MMC发送ATTACH成功*/
    NAS_EMM_AppSendAttSucc();

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
        NAS_EMM_ClearEmcEsmMsg();
    }

    /* 识别EPS ONLY,原因值为18 disable L模的场景,释放链路 */
    /*NAS_EMM_RecogAndProc18Disable(NAS_EMM_GLO_AD_GetAttRejCau());*/
    #endif

}


VOS_VOID  NAS_EMM_ProcRrcDataCnfSucc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc is entered");

    /*根据触发原因向ESM发送结果*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*发送UE指定的 Drx_Cycle给RRC*/

    if(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag())
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }

    NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
    /*leili modify for isr begin*/
    /*发送 LMM_MMC_TIN_TYPE_IND*/
    NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*更新所有承载的ISR标识为ISR激活前*/
    NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                    (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                    sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                    *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI END*/
    /*leili modify for isr end*/
    NAS_EMM_SetAttachRegDomain();

    if ((MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType()) &&
        (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcRrcDataCnfSucc Eps Only");

        NAS_EMM_ProcRrcDataCnfEpsSuccOnly();
    }
    else
    {
        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_EMC_REGED);
        }
        else
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED);
        }

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();

        /* 识别SMS only或者CS fallback not preferred disable L模的场景,释放链路 */
        /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/
    }



    /* ATTACH 成功，如果之前发生了异系统变换，需清除触发和类型记录信息 */
    NAS_EMM_ClearResumeInfo();

    return;
}

VOS_VOID  NAS_EMM_AttProcT3410Exp( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        NAS_EMM_AppSendAttTimerExp();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* 注意必须先上报结果再清除attach attempt counter */
        NAS_EMM_AppSendAttTimerExp();

        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}
VOS_VOID  NAS_EMM_AttProcEpsOnlyRejValueOther( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        NAS_EMM_AppSendAttRejEpsOnly();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* 注意必须先上报结果再清除attach attempt counter */
        NAS_EMM_AppSendAttRejEpsOnly();

        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq( NAS_LMM_NOT_BARRED);
}

VOS_VOID  NAS_EMM_ProcRejCauOtherAbnormal( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        /* 参照标杆实现，在ATTACH被其他原因值拒绝，尝试次数小于5时，将有效安全上下文写卡 */
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        NAS_EMM_AppSendAttRej();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* 注意必须先上报结果再清除attach attempt counter */
        NAS_EMM_AppSendAttRej();

        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}


VOS_VOID  NAS_EMM_AD_Reattach(VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO(            "NAS_EMM_AD_Reattach entered!");

    /*通知ESM*/
    NAS_EMM_AttSendEsmResult(           EMM_ESM_ATTACH_STATUS_DETACHED);

    /*重新ATTACH*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(          0);
    NAS_LMM_StartStateTimer(             TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
   return;
}
/*****************************************************************************
 Function Name   : NAS_EMM_Attach_IsTimerRunning
 Description     : 判断3411和3402定时器是否在运行
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY         2012-02-24  Draft Enact
*****************************************************************************/
VOS_UINT32 NAS_EMM_Attach_IsTimerRunning(VOS_VOID )
{
    /* T3411在运行，不向MMC发ATTACH结果*/
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_Attach_IsTimerRunning: T3411 is running.");

        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        return NAS_EMM_YES;
    }

    /* T3402运行，向MMC发ATTACH结果，触发MMC搜下一个网络*/
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
         NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_Attach_IsTimerRunning: T3402 is running.");

        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_T3402_RUNNING);
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
VOS_VOID  NAS_EMM_ProcAttachConnFailOtherCause( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*启动定时器T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        NAS_EMM_AppSendAttFailWithPara();

    }

    else
    {
        /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
        /*设置update status为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* 必须先转状态，再上报ATTACH结果，最后清除 attach attempt counter,
            顺序不能变 */

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;


    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ACCESS_BAR);

        /*重设attach attempt counter，顺序必须在上报ATTACH结果之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();

        return ;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_ATTACH_NEEDED,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送ATTACH接入被bar*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ACCESS_BAR);

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return ;
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailCellSrchProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在上报ATTACH结果之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* 状态迁移Dereg.Plmn_Search*/
        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_PLMN_SEARCH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        return ;
    }

    /* 通知ESM,APP注册失败*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 状态迁移Dereg.Plmn_Search*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_PLMN_SEARCH,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return ;
}



VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在上报ATTACH结果之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();

        return ;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_ProcAttachConnFailOtherCause();

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return ;
}


VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU         *pMrrcConnectFailInd = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd is entered");

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

     /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd: NULL PTR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*stop T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

    /*get the msg*/
    pMrrcConnectFailInd = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;

    /*process different with the est result*/
    switch(pMrrcConnectFailInd->enEstResult)
    {
        case LRRC_EST_ACCESS_BARRED_MO_SIGNAL:
        case LRRC_EST_ACCESS_BARRED_ALL:

            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc();
            break;

        /* 建链失败，RRC正在进行小区搜索*/
        case LRRC_EST_CELL_SEARCHING:

            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailCellSrchProc();
            break;

        default:
            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc();
            break;
    }

    return  NAS_LMM_MSG_HANDLED;
}


VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(
                                                    VOS_UINT32  ulCause)
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej entered");

    (VOS_VOID)ulCause;

    /*停止定时器T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    NAS_EMM_AttDataUpdate(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_NO_IMSI*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_ATTCH_CNF或LMM_MMC_ATTCH_IND消息*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_AUTH_REJ);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return;
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(VOS_UINT32 ulCause)
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd is entered");

    (VOS_VOID)(ulCause);

    /*停止定时器T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_PLMN_SEARCH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        return ;
    }

    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*启动定时器T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();
    }

    else
    {
        /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
        /*设置update status为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* 必须先转状态，再上报结果，再清除 attach attempt counter */

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;

}



VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthFail(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   =   (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthFail is entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;


    /*获得原因值*/
    ulCause                                       =         pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL        ==        ulCause)
    {
        NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(        ulCause);
    }
    else
    {
        NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(      ulCause);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru )
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthRej entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);


    return  NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgRrcRelInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_REL_IND_STRU     *pRrcRelInd =         (LRRC_LMM_REL_IND_STRU*)(pMsgStru);
    VOS_UINT32               ulCause;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgRrcRelInd is entered");

    (VOS_VOID)ulMsgId;

    /*获得原因值*/
    ulCause                             =         pRrcRelInd->enRelCause;

    NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(ulCause);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmPdnRspMsgRrcRelInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_REL_IND_STRU                *pRrcRelInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    /*RRC_REL_IND消息预处理中判断了如果是IDLE态，则直接丢弃，所以这里一定是CONN*/

    /*CONN态,释放连接*/
    pRrcRelInd = (LRRC_LMM_REL_IND_STRU*)(pMsgStru);
    NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgRrcRelInd Cause:", pRrcRelInd->enMsgId);
    NAS_EMM_CommProcConn2Ilde();

    return  NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp(VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru )
{

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*ATTACH尝试计数器加1*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttTimerExp();

        /*重设attach attempt counter，顺序必须在上报ATTACH结果之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    /*向ESM发送ATTACH结果*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AttProcT3410Exp();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp( VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru  )
{

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG, EMM_SS_DEREG_ATTEMPTING_TO_ATTACH))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }


    #if 1
    /*重新ATTATCH*/
    NAS_EMM_AD_Reattach();

    #else
    /*保存触发ATTACH的原因*/
    /*NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_TIMER_EXP;*/

    /*向MRRC发送ATTACH REQUEST消息*/
    NAS_EMM_MrrcSendAttReq(             NAS_EMM_GLO_AD_GetEsmMsgLen());

    /*启动定时器T3410*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3410);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_CN_ATTACH_CNF*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                        TI_NAS_EMM_T3410);
    #endif

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp( VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru  )
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTEMPTING_TO_ATTACH))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_PDN_CON_IND消息*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(          0);

    /*启动定时器TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);


    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32    NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd(VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_ACCESS_GRANT_IND_STRU           *pRrcMmGrantInd = NAS_EMM_NULL_PTR;
    VOS_UINT32                              ulRst;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd is entered.");

    (VOS_VOID)ulMsgId;

    /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {

        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: NULL PTR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTACH_NEEDED))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*get the grant msg*/
    pRrcMmGrantInd = (LRRC_LMM_ACCESS_GRANT_IND_STRU *)pMsgStru;
    if ((LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL == pRrcMmGrantInd->enAccessGrantType) ||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MT == pRrcMmGrantInd->enAccessGrantType) ||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MT == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MO_CSFB == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB_AND_MT == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_ALL == pRrcMmGrantInd->enAccessGrantType))
    {
        /*保存触发ATTACH的原因*/
        /*NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_OTHER;*/

        /*发送EMM_ESM_STATUS_IND(去注册)*/
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_DETACHED);

        /*转换EMM状态机MS_DEREG+SS_DEREG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*发送ATTACH REQ内部消息给ATTACH模块*/
        ulRst = NAS_EMM_SendIntraAttachReq();
        if(NAS_EMM_SUCC != ulRst)
        {
            NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: send INTRA ATTACH REQ ERR !");
        }
    }
    else
    {
        return NAS_LMM_MSG_DISCARD;
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd entered.");

    (VOS_VOID)ulMsgId;

    /* 入参检查*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd: NULL PTR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* EMM状态检查*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTACH_NEEDED))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果系统消息被禁，则迁移到 Dereg.Limite_Service*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    if((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
        ||(EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
    {
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_SetAttType();

        NAS_EMM_AppSendAttRsltForb(pstsysinfo->ulForbiddenInfo);
    }

    return NAS_LMM_MSG_HANDLED;

}
VOS_UINT32    NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp(VOS_UINT32 ulMsgId,
                                                            const VOS_VOID * pMsgStru)

{
     /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_PDN_RSP))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_REL_IND*/
    NAS_EMM_EsmSendRelInd();

    /*修改状态：进入主状态DEREG子状态DEREG_PLMN_SEARCH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    /*向MMC发送消息*/
    /*NAS_EMM_AttSendAppResult(APP_ERR_MM_ATTACH_FAIL_UNSPECIFIED_ABNORMAL);*/
    NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_ESM_FAILURE);
    /*ATTACH清除资源*/
    NAS_EMM_Attach_ClearResourse();

    /* 如果处于CONN态,释放连接*/
    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
    }

   return   NAS_LMM_MSG_HANDLED;


}
VOS_UINT32    NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp(VOS_UINT32 ulMsgId,
                                                            const VOS_VOID * pMsgStru)

{
     /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*通知APP缺省承载激活失败*/
    /*NAS_EMM_AttSendAppResult(APP_ERR_MM_ATTACH_FAIL_ACT_DEFAULT_BEARER_FAIL);*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ESM_FAILURE);

    /* 根据ATTACH类型填写DETACH类型 */
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_CS_PS);
    }
    else
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_PS_ONLY);
    }

    /*去注册*/
    NAS_EMM_SendDetachReqMo();

    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_ProcAttCompleteSendFail( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*启动定时器T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        NAS_EMM_AppSendAttFailWithAllPara();
    }

    else
    {
        /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
        /*设置update status为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*必须先删除TAI List再启动T3402,顺序不能颠倒*/
        /*启动定时器T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* 必须先转状态再上报结果，再清除 attach attempt counter*/

        /*修改状态：进入主状态DEREG子状态DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithAllPara();
        /*重设attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
    return;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru  )
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg                          = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*判断是否是TA变化了*/
    if ((EMMC_EMM_CHANGE_PLMN           != pRcvEmmMsg->ulChangeInfo)
        && (EMMC_EMM_CHANGE_TA          != pRcvEmmMsg->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:no change");
        return  NAS_LMM_MSG_HANDLED;
    }

    if ((EMMC_EMM_NO_FORBIDDEN == pRcvEmmMsg->ulForbiddenInfo)
        && (EMMC_EMM_CELL_STATUS_ANYCELL != pRcvEmmMsg->ulCellStatus))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:no forbidden and not any cell");

        /*停止定时器*/
        NAS_LMM_StopStateTimer(     TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

        /* 如果是紧急注册，则重新发起紧急注册 */
        if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

            NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

            /* 发送ATTACH CMP消息失败的处理 */
            NAS_EMM_ProcAttCompleteSendFail();

            /* 通知IMSA再尝试CS域 */

            NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

            return  NAS_LMM_MSG_HANDLED;
        }

        /*重新发起普通ATTACH*/
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        NAS_EMM_EsmSendPdnConnInd(          0);
        NAS_LMM_StartStateTimer(            TI_NAS_EMM_WAIT_ESM_PDN_RSP);
        NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                            EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                            TI_NAS_EMM_WAIT_ESM_PDN_RSP);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:forbidden or any cell");

    /*停止定时器*/
    NAS_LMM_StopStateTimer(     TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        /*状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb(pRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        /* 通知IMSA再尝试CS域 */
    }
    else
    {
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb( pRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(             NAS_LMM_NOT_BARRED);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru)
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pstSysInfoInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    pstSysInfoInd                          = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

     NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }


    /*如果系统消息被禁，则释放连接，进入Dereg.Limite_Service*/
    if ((EMMC_EMM_NO_FORBIDDEN != pstSysInfoInd->ulForbiddenInfo)
        ||(EMMC_EMM_CELL_STATUS_ANYCELL == pstSysInfoInd->ulCellStatus))
    {
        /* 终止ATTACH流程*/
        NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: Cell Forb.");
        if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_StartEmergencyAttach();

            return  NAS_LMM_MSG_HANDLED;
        }

        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb(pstSysInfoInd->ulForbiddenInfo);


        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    /*zhengjunyan注:正常情况下不会执行下面的分支，暂时保留目前实现*/
    /*判断是否是TA变化了*/
    if ((EMMC_EMM_CHANGE_PLMN == pstSysInfoInd->ulChangeInfo)
         || (EMMC_EMM_CHANGE_TA == pstSysInfoInd->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: Enter New TA.!");

        /*停止,启动定时器T3410*/
        NAS_LMM_StartStateTimer(     TI_NAS_EMM_T3410);

        /*向MRRC发送ATTACH REQUEST消息*/
        NAS_EMM_MrrcSendAttReq();

		/* 33401 CR457中描述UE从脱离去注册态转去注册态时，需要将SIM卡或者NV中的安全
		上下文设置为无效*/
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);

        #if 0
        /* 切换MRRC状态为READY状态 */
        NAS_EMM_CHANGE_MRRC_SEND_STATE(     NAS_EMM_MRRC_SEND_STATE_READY);
        #endif
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq( VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru )
{
    NAS_LMM_INTRA_ATTACH_REQ_STRU                     *pRcvEmmMsg;
    pRcvEmmMsg = (NAS_LMM_INTRA_ATTACH_REQ_STRU*)      pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_NORMAL_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_IntraAttReqChk(pRcvEmmMsg))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq: MMC_MM_SYS_INFO_IND_STRU para err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /*保存触发ATTACH的原因*/
    if(EMM_ATTACH_CAUSE_ESM== NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_OTHER;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*检查如果UE不能自动发起ATTACH,则不发起注册流程*/
    if(NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_EMM_GetPsAttachAllowFlg())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq:AUTO ATTACH NOT ALLOW!");

        /* 向MMC上报ATTACH结果 */
        NAS_EMM_AppSendAttPsNotAllow();

        return NAS_LMM_MSG_HANDLED;
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    /*向ESM发送ID_EMM_ESM_PDN_CON_IND消息*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(0);

    /*启动定时器TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsNmlSrvMsgAuthRej(
                                        VOS_UINT32 ulMsgId,
                                        const VOS_VOID *pMsgStru)
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsNmlSrvMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name  : NAS_EMM_MsRegSsLimitedSrvMsgAuthRej
 Discription    : Reg.Limit_Service状态下收到AUTH_REJ消息
 Input          : None
 Output         : None
 Return         :
 History:
      1.lihong00150010      2012-12-29      Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgAuthRej
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsLimitedSrvMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthRej
(
    VOS_UINT32 ulMsgId,
    VOS_VOID *pMsgStru
)
{

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}




VOS_VOID  NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej( VOS_UINT32  ulCause)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej is entered");

    (VOS_VOID)(                           ulCause);

    /*停止定时器*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*删除GUTI,L.V.R TAI,TAI LIST,KSIasme,设置Update Status*/
     /*设置update status为EU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    ulTauRslt = MMC_LMM_TAU_RSLT_AUTH_REJ;
    NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*等待RRC_REL_IND*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    return;
}


VOS_VOID  NAS_EMM_AttResetAttAttempCounter(VOS_VOID)
{
    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    /*停止定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_PLMN_SEARCH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        return  NAS_LMM_MSG_HANDLED;
    }

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

        /* 发送ATTACH CMP消息失败的处理 */
        NAS_EMM_ProcAttCompleteSendFail();

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        /* 通知IMSA再尝试CS域紧急呼 */

        return  NAS_LMM_MSG_HANDLED;
    }

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /*与ESM交互，重新发起ATTACH*/
    /*向ESM发送ID_EMM_ESM_PDN_CON_IND消息*/
    NAS_EMM_EsmSendPdnConnInd(0);

    /*启动定时器TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);


    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return  NAS_LMM_MSG_HANDLED;
}
#if 0
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    LRRC_LMM_DATA_CNF_STRU*               pstRrcMmDataCnf = NAS_EMM_NULL_PTR;

    pstRrcMmDataCnf                     =   (LRRC_LMM_DATA_CNF_STRU*)(pMsgStru);

    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    /* 检查OpId */
#if (VOS_OS_VER != VOS_WIN32)

    if(NAS_LMM_FAIL                      ==  NAS_EMM_ChkMmRrcOpId(pstRrcMmDataCnf->ulOpId))
    {
        NAS_EMM_ATTACH_LOG_WARN(        "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf,ChkMmRrcOpId Err");
        return  NAS_LMM_MSG_HANDLED;
    }
#endif

    /*清除MRRC的管理数据*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /* 确认成功发送处理 */
    if(LRRC_LMM_SEND_RSLT_SUCCESS       == pstRrcMmDataCnf->enSendRslt)
    {
        NAS_EMM_ProcRrcDataCnfSucc();
        return  NAS_LMM_MSG_HANDLED;
    }

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    /*发送RRC_MM_REL_REQ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return  NAS_LMM_MSG_HANDLED;
}
#endif
VOS_VOID  NAS_EMM_AttNrmFailProc( VOS_VOID )
{
     /*ATTACH尝试计数器加1*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()++;*/

    /*判断ATTACH尝试计数器，根据计数器分小于5和等于5处理*/
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
    }

    else
    {
        NAS_EMM_AtmpCntEqual5Proc();
    }
    return;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    #if 0
    /*清除MRRC的管理数据*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_STATE_NO_TIMER);
    #endif

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    if(NAS_EMM_CONN_IDLE           != NAS_EMM_GetConnState())
    {
        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(              NAS_LMM_NOT_BARRED);
    }

    /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo(
                                                    VOS_UINT32      ulMsgId,
                                                    VOS_VOID        *pMsgStru  )
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pstRcvEmmMsg;

    (VOS_VOID)(ulMsgId);
    pstRcvEmmMsg                        = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM( "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo is entered");

    /*判断是否是TA变化了*/
    if ((EMMC_EMM_CHANGE_PLMN           != pstRcvEmmMsg->ulChangeInfo)
        && (EMMC_EMM_CHANGE_TA          != pstRcvEmmMsg->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:no change");
        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    if((EMMC_EMM_NO_FORBIDDEN         == pstRcvEmmMsg->ulForbiddenInfo)
        && (EMMC_EMM_CELL_STATUS_ANYCELL != pstRcvEmmMsg->ulCellStatus))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:no forbidden and not any cell");

        #if 0
        /*清除MRRC的管理数据*/
        NAS_EMM_ClrMrrcMgmtData(    TI_NAS_EMM_WAIT_RRC_DATA_CNF);
        #endif

        NAS_EMM_SendRrcClearBuffNotify();

        /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

            NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

            /* 发送ATTACH CMP消息失败的处理 */
            NAS_EMM_ProcAttCompleteSendFail();

            /* 通知IMSA再尝试CS域 */

            NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

            return  NAS_LMM_MSG_HANDLED;
        }

        /*重新普通ATTACH*/
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        NAS_EMM_EsmSendPdnConnInd(      0);
        NAS_LMM_StartStateTimer(        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
        NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:forbidden or any cell");

    #if 0
    NAS_EMM_ClrMrrcMgmtData(TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    #endif

    /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        /*状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb(pstRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        /* 通知IMSA再尝试CS域 */
    }
    else
    {
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*状态迁移:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb(pstRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)(                         pMsgStru);
    (VOS_VOID)(                         ulMsgId);
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd entered!");

    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    #if 0
    /*清除MRRC的管理数据*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_STATE_NO_TIMER);
    #endif

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    /* 发送ATTACH CMP消息失败的处理 */
    NAS_EMM_ProcAttCompleteSendFail();

    /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return  NAS_LMM_MSG_HANDLED;

}


/*****************************************************************************
 Function Name   : NAS_EMM_AttGuAttachRst
 Description     : 挂起状态下，处理GU发来的ATTACH结果
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.Hanlufeng 41410      2011-5-12  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_AttGuAttachRst( MsgBlock * pMsg )
{
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pMmcActResult;

    pMmcActResult = (MMC_LMM_ACTION_RESULT_REQ_STRU *)pMsg;


    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AttGuAttachRst: ulActRst =.",
                            pMmcActResult->ulActRst);

    if(MMC_LMM_RSLT_TYPE_SUCCESS == pMmcActResult->ulActRst)
    {
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* 通知ESM状态改变 */
        NAS_EMM_PUB_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);
    }
    else
    {
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* 通知ESM状态改变 */
        NAS_EMM_PUB_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    return;
}
/*lint +e961*/
/*lint +e960*/

VOS_UINT32 NAS_EMM_SndAttachCmpSuccProc(VOS_VOID* pMsg)
{
    NAS_EMM_ESM_MSG_BUFF_STRU           *pstEsmMsg = NAS_EMM_NULL_PTR;

    (VOS_VOID)pMsg;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SndAttachCmpSuccProc is entered");

    /*根据触发原因向ESM发送结果*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*发送UE指定的 Drx_Cycle给RRC*/

    if(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag())
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }

    NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
    /*leili modify for isr begin*/
    /*发送 LMM_MMC_TIN_TYPE_IND*/
    NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*更新所有承载的ISR标识为ISR激活前*/
    NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                    (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                    sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                    *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI END*/
    /*leili modify for isr end*/
    NAS_EMM_SetAttachRegDomain();

    if ((MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType()) &&
        (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcRrcDataCnfSucc Eps Only");

        NAS_EMM_ProcRrcDataCnfEpsSuccOnly();
    }
    else
    {
        /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_EMC_REGED);
        }
        else
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED);
        }

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /*ATTACH完成向MMC发送ATTACH成功*/
        NAS_EMM_AppSendAttSucc();
    }
    /* ATTACH 成功，如果之前发生了异系统变换，需清除触发和类型记录信息 */
    NAS_EMM_ClearResumeInfo();
    /* ATTACH 成功，清除之前保存的原因值 */
    NAS_LMM_ClearEmmInfoMmcAttachReason();
    return NAS_EMM_SUCC;
}
VOS_UINT32 NAS_EMM_SndAttachCmpFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    (VOS_VOID)pMsg;
    *pulIsDelBuff = VOS_TRUE;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SndAttachCmpFailProc is entered");

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_SndAttachCmpFailProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*重设attach attempt counter，顺序必须在ATTACH结果上报之后*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    /*发送RRC_MM_REL_REQ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    /* ATTACH触发原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return NAS_EMM_SUCC;
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

