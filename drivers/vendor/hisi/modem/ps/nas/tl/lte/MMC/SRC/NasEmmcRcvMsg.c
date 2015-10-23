

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasEmmcRcvMsg.h"
#include  "NasEmmcPublic.h"
#include  "NasEmmcSendMsg.h"
#include  "NasEmmcMain.h"
#include  "NasEmmPubUResume.h"
#include  "NasLmmPubMPrint.h"
#include  "NasLmmPubMTimer.h"
#include  "NasEmmTauSerInclude.h"
#include  "NasEmmAttDetInclude.h"

#include "DrvInterface.h"
/*end*/

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMCRCVMSG_C
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


VOS_UINT32  NAS_EMMC_RcvEmmStartReq( MsgBlock *pMsg )
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvEmmStartReq is entered.");

	(void)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvEmmStartReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    NAS_EMMC_Init();

    /*发送EMMC_EMM_START_CNF消息*/
    NAS_EMMC_SendEmmStartCnf();

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMMC_RcvEmmStopReq( MsgBlock *pMsg )
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvEmmStopReq is entered.");
    (void)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvEmmStopReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*发送EMMC_EMM_START_CNF消息*/
    NAS_EMMC_SendEmmStopCnf();

    return NAS_LMM_MSG_HANDLED;
}



/*lint -e960*/
/*lint -e961*/
VOS_UINT32  NAS_EMMC_RcvMmcPlmnSrchReq(MsgBlock *pMsg )
{

    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMmcPlmnReq = NAS_EMMC_NULL_PTR;
    LRRC_LNAS_PLMN_SEARCH_TYPE_ENUM_UINT32   ulPlmnType;
#if (VOS_OS_VER != VOS_WIN32)
    static VOS_UINT32  s_ulPlmnSrchReqCnt = 1;
#endif

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcPlmnSrchReq is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcPlmnSrchReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 结构转化 */
    pstMmcPlmnReq = (MMC_LMM_PLMN_SRCH_REQ_STRU *)pMsg;

    /* 检查搜网类型 */
    ulPlmnType = NAS_LMM_MmcPlmnReqTypeCheck(pstMmcPlmnReq->enSrchType);
    if (LRRC_LNAS_PLMN_SEARCH_TYPE_BUTT == ulPlmnType)
    {
        NAS_LMM_EMMC_LOG_ERR(" NAS_EMMC_RcvMmcPlmnSrchReq: PLMN TYPE IS ERR!");
        return NAS_LMM_MSG_HANDLED;
    }

    /*非空闲态时，回复LIST搜索拒绝.
    LMM无论是否处于空闲态，MMC均可直接下发MMC_LMM_PLMN_SEARCH_REQ要求LMM进行
    LIST搜索；
    1) 若LMM处于非空闲态，LMM回复LMM_MMC_PLMN_SEARCH_CNF，
    结果为 LIST搜索拒绝;
    2) 如果MMC在空闲态发送MMC_LMM_PLMN_SEARCH_REQ命令，但是LMM收到该消息时转到了
    建链过程中，若搜索类型为LIST,则LMM回复MMC_LMM_PLMN_SEARCH_CNF，结果为
    LIST搜索拒绝；
    20130603 BEGIN */
    /*
    if((LRRC_LNAS_PLMN_SEARCH_LIST == ulPlmnType)
        && (NAS_EMM_CONN_ESTING == NAS_EMM_GetConnState()))
    {
        NAS_LMM_EMMC_LOG_INFO(" NAS_EMMC_RcvMmcPlmnSrchReq: PLMN LIST TYPE IS REJECT!");
        NAS_EMMC_SendMmcPlmnListSrchRej();
        return NAS_LMM_MSG_HANDLED;
    }
    */

    /* 如果当前不是IDLE态 和 NAS_EMM_CONN_RELEASING */
    /*lint -e539*/
    /*lint -e830*/
    if ((NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
        && (NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState()))
    {
         /* 若是list搜索 , 则回复list拒绝 */
        if(LRRC_LNAS_PLMN_SEARCH_LIST == ulPlmnType)
        {
            NAS_LMM_EMMC_LOG_INFO(" NAS_EMMC_RcvMmcPlmnSrchReq: PLMN LIST TYPE IS REJECT!");
            NAS_EMMC_SendMmcPlmnListSrchRej();
        }
        else
        {   /* 若不是 list 搜索，则 丢弃此搜网请求消息 */
            NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcPlmnSrchReq:Not Idle,Discard!");
        }

        return NAS_LMM_MSG_HANDLED;
    }
    NAS_LMM_EMMC_LOG_INFO(" NAS_EMMC_RcvMmcPlmnSrchReq: PLMN SEARCH BEGIN!");
    /*lint +e830*/
    /*lint +e539*/

#if (VOS_OS_VER != VOS_WIN32)
    if(1 == s_ulPlmnSrchReqCnt)
    {
        (VOS_VOID) ddmPhaseScoreBoot("LTE Cell search start",__LINE__);
        s_ulPlmnSrchReqCnt++;
    }
#endif
    /*end*/

    /* 结构转化 */
    pstMmcPlmnReq = (MMC_LMM_PLMN_SRCH_REQ_STRU *)pMsg;

    /* 如果当前接入技术没有L或者LTE被DISABLED，则只响应LIST搜网，其他类型直接返回 */
    if((NAS_LMM_FAIL == NAS_EMM_lteRatIsExist())
     ||(NAS_LMM_SUCC == NAS_EMM_IsLteDisabled()))
    {
        if(MMC_LMM_PLMN_SRCH_LIST != pstMmcPlmnReq->enSrchType)
        {
            NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcPlmnSrchReq: LTE IS NOT EXIST or LTE DISABLED");
            return NAS_LMM_MSG_HANDLED;
        }
    }

    if (MMC_LMM_PLMN_SRCH_USER_SPEC == pstMmcPlmnReq->enSrchType)
    {
        NAS_EMMC_SetUserSpecPlmnFlag(NAS_EMMC_USER_SPEC_PLMN_YES);
    }

    if (pstMmcPlmnReq->enSrchType != MMC_LMM_PLMN_SRCH_LIST)
    {
        /*发送EMMC_EMM_PLMN_IND消息*/
        NAS_EMMC_SendEmmPlmnInd();
    }

    /*之前的逻辑是系统消息发生变化后才上报位置变更信息（通过注册状态上报消息），
      由于SVLTE项目PS域在不同modem上迁移需求，新需求为只要MMC发起了搜网，且不是list搜，不论
      搜到小区是否变化都需要上报注册状态。
      因此在MMC发搜网时，设置搜网标记，收到系统消息后上报注册状态，并清除搜网标记*/
    if((MMC_LMM_PLMN_SRCH_LIST != pstMmcPlmnReq->enSrchType)
        || (MMC_LMM_PLMN_SRCH_TYPE_BUTT != pstMmcPlmnReq->enSrchType))
    {
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_RcvMmcPlmnSrchReq: set single plmn search flag!");
        NAS_EMMC_SetSinglePlmnSrchFlag(NAS_EMM_YES);
    }

    /*发送RRC_MM_PLMN_SEARCH_REQ消息*/
    NAS_EMMC_SendRrcPlmnSrchReq(pstMmcPlmnReq);

    /* 背景lIST搜时不设置搜网标记 */
    if (VOS_FALSE == NAS_EMMC_IsBgListSearch(pstMmcPlmnReq->enSrchType))
    {
        NAS_EMMC_SetPlmnSrchFlag(NAS_EMM_PLMN_SRCH_FLAG_VALID);
        NAS_LMM_StartStateTimer(TI_NAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER);
    }

    return NAS_LMM_MSG_HANDLED;

}






VOS_UINT32  NAS_EMMC_RcvRrcPlmnSrchCnf (MsgBlock *pMsg)
{
    LRRC_LMM_PLMN_SEARCH_CNF_STRU         *pstPlmnCnf = NAS_EMMC_NULL_PTR;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcPlmnSrchCnf is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcPlmnSrchCnf:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 如果设置了搜网标识，则清除 */
    if (NAS_EMM_PLMN_SRCH_FLAG_VALID == NAS_EMMC_GetPlmnSrchFlag())
    {
        NAS_EMMC_SetPlmnSrchFlag(NAS_EMM_PLMN_SRCH_FLAG_INVALID);
        NAS_LMM_StopStateTimer(TI_NAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER);
    }

    /* 结构转化 */
    pstPlmnCnf = (LRRC_LMM_PLMN_SEARCH_CNF_STRU *)pMsg;

    /*发送LMM_MMC_PLMN_SRCH_CNF消息*/
    NAS_EMMC_SendMmcPlmnSrchCnf(pstPlmnCnf);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_LMM_PreProcMmcLteSysInfoInd(MsgBlock *pMsg )
{
    MMC_LMM_LTE_SYS_INFO_IND_STRU      *pstLteSysInfoInd = NAS_EMMC_NULL_PTR;
    EMMC_EMM_CHANGE_INFO_ENUM_UINT32    ulChangeInfo;
    VOS_UINT32                          ulTaCellIdChangeInfo;
    NAS_EMM_REG_CHANGE_TYPE_ENUM_UINT32 ulChangeType;
    NAS_EMM_REJ_FLAG_ENUM_UINT32        ulMatchRslt;
    /* lihong00150010 emergency tau&service begin */
    EMMC_EMM_CELL_STATUS_ENUM_UINT32    enCellStatus = EMMC_EMM_CELL_STATUS_NORMAL;
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 enForbdInfo = EMMC_EMM_NO_FORBIDDEN;
    /* lihong00150010 emergency tau&service end */

    NAS_LMM_EMMC_LOG_NORM("NAS_LMM_PreProcMmcLteSysInfoInd is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_LMM_PreProcMmcLteSysInfoInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 如果是从模，则丢弃 */
    if(NAS_EMM_IS_SUSPEND == NAS_EMM_IsSuspended())
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_LMM_PreProcMmcLteSysInfoInd:EMM_MS_RRC_CONN_REL_INIT is discard");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 结构转化 */
    pstLteSysInfoInd = (MMC_LMM_LTE_SYS_INFO_IND_STRU *)pMsg;

    /* 目前RRC给NAS上报的系统消息在所有场景下astSuitPlmnList都是只上报1个PLMN,
       stSpecPlmnIdList中的ulSuitPlmnNum 也只填的固定值为1*/

    /*TA活着CELL ID 改变*/
    NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID( &pstLteSysInfoInd->stLteSysInfo,
                                                &ulTaCellIdChangeInfo);

    /*系统消息中的PLMN是否在拒绝#18列表中*/
    ulMatchRslt = NAS_EMMC_PlmnMatchRej18PlmnList((NAS_MM_PLMN_ID_STRU *)&pstLteSysInfoInd->stLteSysInfo.stSpecPlmnIdList.astSuitPlmnList[0]);

    /*设置当前PLMN是否在拒绝#18列表中的标识*/
    NAS_EMMC_SetRejCause18Flag(ulMatchRslt);

    NAS_LMM_EMMC_LOG1_NORM("NAS_LMM_PreProcMmcLteSysInfoInd:ulMatchRslt = ",
                                ulMatchRslt);

    if ((NAS_EMM_REJ_YES == ulMatchRslt) &&
        (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_LMM_PreProcMmcLteSysInfoInd:Reg Domain update");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /* 判断系统消息是否发生变化*/
    /* lihong00150010 emergency tau&service begin */
    NAS_EMMC_ProcessMmcLteSysInfoInd(   &pstLteSysInfoInd->stLteSysInfo,
                                        &ulChangeInfo,
                                        &enCellStatus,
                                        &enForbdInfo);
    /* lihong00150010 emergency tau&service end */

/* lihong00150010 csfb begin */
    /* 如果当前PLMN的MCC与MML中存储的MCC不同，则删除紧急呼号码列表，同时指示MM模块
       紧急呼号码列表个数为0；*/
    if (VOS_TRUE == NAS_EMM_IsMccChanged())
    {
        NAS_EMM_ClearEmergencyList();

        /* lihong00150010 ims begin */
        /* 通知MMC紧急呼号码列表，网侧是否支持IMS VOICE和紧急呼，以及LTE的CS域能力 */
        /* NAS_EMM_SendGmmInfoChangeNotify(); */
        NAS_EMM_SendMmcInfoChangeNotify();
        /*NAS_EMM_SendMmInfoChangeNotify();*/
        /* lihong00150010 ims end */
    }
/* lihong00150010 csfb end */
    /* lihong00150010 emergency tau&service begin */
    NAS_EMMC_SendEmmSysInfoInd(ulChangeInfo,enCellStatus,enForbdInfo);
    /* lihong00150010 emergency tau&service end */

    if (EMMC_EMM_NO_CHANGE != ulChangeInfo)
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_LMM_PreProcMmcLteSysInfoInd:Sys info is change");

        /*向APP发送APP_MM_REG_STAT_IND消息*/
        NAS_EMM_AppRegStatInd(NAS_LMM_GetNasAppRegState(),NAS_LMM_GetEmmCurFsmMS());
    }

    /* 如果TA或者CELL ID 改变，向MMC上报注册状态改变指示消息 */
    if ( ( NAS_EMMC_LA_INFO_MASK_SEC_BIT == (ulTaCellIdChangeInfo & NAS_EMMC_LA_INFO_MASK_SEC_BIT))
       ||( NAS_EMMC_LA_INFO_MASK_THD_BIT == (ulTaCellIdChangeInfo & NAS_EMMC_LA_INFO_MASK_THD_BIT))
       /*只要发生过非list类型的搜网，不论小区是否变化，都上报注册状态*/
       ||( NAS_EMM_YES == NAS_EMMC_GetSinglePlmnSrchFlag()))
    {
        ulChangeType = NAS_EMM_REG_CHANGE_TYPE_LOCATION_INFO;

        NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcSysInfoInd:TAC OR CellId info is change");

        /*向MMC上报ID_LMM_MMC_REGISTER_STATUS_IND消息*/
        NAS_EMM_SendMmcRegisterStatInd(ulChangeType);

        /*上报完注册状态消息后，清掉搜网标记*/
        NAS_EMMC_SetSinglePlmnSrchFlag(NAS_EMM_NO);
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvRrcSysInfoInd(MsgBlock *pMsg )
{
    LRRC_LMM_SYS_INFO_IND_STRU         *pstSysInfoInd = NAS_EMMC_NULL_PTR;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcSysInfoInd is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcSysInfoInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 链路释放过程中和AUTH_INIT状态正常不会收到系统消息，直接丢弃 */
    if ((EMM_MS_RRC_CONN_REL_INIT == NAS_LMM_GetEmmCurFsmMS())
        || (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS()))
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcSysInfoInd:EMM_MS_RRC_CONN_REL_INIT is discard");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 结构转化 */
    pstSysInfoInd = (LRRC_LMM_SYS_INFO_IND_STRU *)pMsg;

    /* 清除Not Camp On标识 */
    NAS_EMMC_SetNotCampOnFlag(VOS_FALSE);

    /*发送LMM_MMC_SYS_INFO_IND消息*/
    NAS_EMMC_SendMmcSysInfo(pstSysInfoInd);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMMC_RcvMmcEplmnNotifyReq(MsgBlock *pMsg)
{
    MMC_LMM_EPLMN_NOTIFY_REQ_STRU       *pstEplmnReq;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcEplmnNotifyReq is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcEplmnNotifyReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    pstEplmnReq                        = (MMC_LMM_EPLMN_NOTIFY_REQ_STRU *)pMsg;


    /*发送RRC_MM_EQU_PLMN_NOTIFY_REQ消息*/
    NAS_EMMC_SendRrcEplmnNotifyReq(pstEplmnReq);

    if(NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
    {
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_RcvMmcEplmnNotifyReq: SEND FORB_TA TO LRRC.");
        NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);
    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMMC_RcvMmcStopSrchReq( MsgBlock *pMsg)
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcStopSrchReq is entered.");

    (void)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcStopSrchReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 如果设置了搜网标识，则清除 */
    if (NAS_EMM_PLMN_SRCH_FLAG_VALID == NAS_EMMC_GetPlmnSrchFlag())
    {
        NAS_EMMC_SetPlmnSrchFlag(NAS_EMM_PLMN_SRCH_FLAG_INVALID);
        NAS_LMM_StopStateTimer(TI_NAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER);
    }

    /*发送RRC_MM_PLMN_SEARCH_STOP_REQ消息*/
    NAS_EMMC_SendRrcPlmnSrchStopReq();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvRrcPlmnSrchStopCnf(MsgBlock *pMsg)
{

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcPlmnSrchStopCnf is entered.");

    (void)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcPlmnSrchStopCnf:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*发送LMM_MMC_STOP_SRCH_CNF消息*/
    NAS_EMMC_SendMmcStopSrchCnf();

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMMC_RcvRrcAreaLostInd(MsgBlock *pMsg )
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcAreaLostInd is entered.");

    (void)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcAreaLostInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 收到area lost 后处理*/
    NAS_EMMC_ProcessRrcAreaLostInd();

    /*发送ID_EMMC_EMM_COVERAGE_LOST_IND消息*/
    NAS_EMMC_SendEmmCoverageLostInd();

    /* 如果当前CSFB延时定时器在运行，说明在REG-NORMAL态下释放过程中收到CSFB，
       但是在释放后搜小区出现丢网，此时应触发去GU搜网继续CSFB */
    if((NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_CSFB_DELAY))
        &&(VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure()))
    {
        /* 停止CSFB时延定时器 */
        NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_CSFB_DELAY);

        /* 给MMC上报SERVICE失败触发搜网去GU */
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
    }

    /*发送LMM_MMC_AREA_LOST_IND消息*/
    NAS_EMMC_SendMmcAreaLostInd();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvMmcUserPlmnEndNotify(MsgBlock *pMsg )
{
    VOS_UINT32                          i = 0;
    NAS_LMM_NETWORK_INFO_STRU           *pstNetInfo = NAS_LMM_NULL_PTR;

    NAS_EMM_PUBU_LOG_NORM("NAS_EMMC_RcvMmcUserPlmnEndNotify is entered");

    (void)pMsg;

     /* 入参检查 */
    if ( NAS_EMM_NULL_PTR == pMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMMC_RcvMmcUserPlmnEndNotify:input ptr null!");
        return  NAS_LMM_MSG_HANDLED;
    }

    pstNetInfo = (VOS_VOID*)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_NET_INFO);

    NAS_EMM_PUBU_LOG1_NORM("NAS_LMM_PreProcMmcUserPlmnEndNotify:Rej15 TA NUM =, ",
                               pstNetInfo->stForbTaForRej15.ulTaNum);

    for (i = 0; i < pstNetInfo->stForbTaForRej15.ulTaNum; i++)
    {
        if (NAS_LMM_MATCH_FAIL == NAS_LMM_TaMatchTaList(&pstNetInfo->stForbTaForRej15.astTa[i],&pstNetInfo->stForbTaForRoam))
        {
            NAS_LMM_AddTaInTaList( &pstNetInfo->stForbTaForRej15.astTa[i],
                                &pstNetInfo->stForbTaForRoam,
                                NAS_MM_MAX_TA_NUM);

        }

    }

    /*删除拒绝15 TA列表*/
    NAS_LMM_MEM_SET(&pstNetInfo->stForbTaForRej15, 0, sizeof(NAS_MM_FORB_TA_LIST_STRU));

    /*清除用户指定搜网标识*/
    NAS_EMMC_SetUserSpecPlmnFlag(NAS_EMMC_USER_SPEC_PLMN_NO);

    NAS_EMM_PUBU_LOG1_NORM("NAS_LMM_PreProcMmcUserPlmnEndNotify:FORB TA NUM =, ",
                                  pstNetInfo->stForbTaForRoam.ulTaNum);

    /*发送LRRC_LMM_CELL_SELECTION_CTRL_REQ消息给RRC*/
    NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMMC_RcvMmcBgPlmnSearchReq(MsgBlock *pMsg)
{
    MMC_LMM_BG_PLMN_SEARCH_REQ_STRU  *pstMmcMsg = NAS_EMMC_NULL_PTR;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcBgPlmnSearchReq: entered.");

    /* 入口参数检查 */
    if (NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcBgPlmnSearchReq: input ptr null!");
        return NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 如果当前不是空闲态，则丢弃此背景搜请求 */
    if ((NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
        && (NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState()))
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcBgPlmnSearchReq: not idle, discard!");
        return NAS_LMM_MSG_DISCARD;
    }

    pstMmcMsg = (MMC_LMM_BG_PLMN_SEARCH_REQ_STRU *)pMsg;

    /* 向LRRC发送背景搜请求 */
    NAS_EMMC_SendRrcBgPlmnSearchReq(pstMmcMsg);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvMmcStopBgPlmnSearchReq(MsgBlock *pMsg)
{
    MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU *pstMmcMsg;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcStopBgPlmnSearchReq: entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcStopBgPlmnSearchReq: input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    pstMmcMsg = (MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU *)pMsg;

    /* 向LRRC发送停止背景搜请求 */
    NAS_EMMC_SendRrcBgPlmnSearchStopReq(pstMmcMsg);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMMC_RcvRrcBgPlmnSearchCnf(MsgBlock *pMsg)
{
    LRRC_LMM_BG_PLMN_SEARCH_CNF_STRU *pLrrcMsg = NAS_LMM_NULL_PTR;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcBgPlmnSearchCnf: entered");

    /* 入参检查 */
    if (NAS_LMM_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcBgPlmnSearchCnf: input null");
        return NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 结构转化 */
    pLrrcMsg = (LRRC_LMM_BG_PLMN_SEARCH_CNF_STRU *)pMsg;

    /* 发送LMM_MMC_BG_PLMN_SEARCH_CNF消息 */
    NAS_EMMC_SendMmcBgPlmnSearchCnf(pLrrcMsg);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvRrcBgPlmnSearchStopCnf(MsgBlock *pMsg)
{
    LRRC_LMM_BG_PLMN_SEARCH_STOP_CNF_STRU *pLrrcMsg = NAS_LMM_NULL_PTR;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcBgPlmnSearchStopCnf: entered.");

    /* 入参检查 */
    if (NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcBgPlmnSearchStopCnf: input null");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 结构转化 */
    pLrrcMsg = (LRRC_LMM_BG_PLMN_SEARCH_STOP_CNF_STRU *)pMsg;

    /* 发送LMM_MMC_STOP_BG_PLMN_SEARCH_CNF消息 */
    NAS_EMMC_SendMmcBgPlmnSearchStopCnf(pLrrcMsg);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvRrcNotCampOnInd(MsgBlock *pMsg)
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvRrcNotCampOnInd: entered");

    (VOS_VOID)pMsg;

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvRrcNotCampOnInd: input null");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 收到NOT_CAMP_ON后处理*/
    NAS_EMMC_ProcessRrcNotCampOnInd();

    /* 发送ID_EMMC_EMM_COVERAGE_LOST_IND消息 */
    /* NAS_EMMC_SendEmmCoverageLostInd(); */

    /* 发送LMM_MMC_NOT_CAMP_ON_IND消息 */
    NAS_EMMC_SendMmcNotCampOnInd();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMMC_RcvMmcCellSelectionReq(  MsgBlock *pMsg )
{
    MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU    *pstCellSelReq;
    LRRC_LNAS_RESEL_TYPE_ENUM_UINT32          ulReselType;

    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_RcvMmcCellSelectionReq is entered.");

    /* 入参检查 */
    if ( NAS_EMMC_NULL_PTR == pMsg)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcCellSelectionReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 结构转化 */
    pstCellSelReq = (MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU *)pMsg;

    if (MMC_LMM_RESEL_SUITABLE == pstCellSelReq->ulSelecType)
    {
        NAS_LMM_EMMC_LOG1_NORM("NAS_EMMC_RcvMmcCellSelectionReq: Cell type = ",
                                pstCellSelReq->ulSelecType);
        ulReselType = LRRC_LNAS_RESEL_SUITABLE;
    }
    else
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_RcvMmcCellSelectionReq:Input Cell type is err! ");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }


    /*发送RRC_MM_CELL_SELECTION_CTRL_REQ消息*/
    NAS_EMMC_SendRrcCellSelectionReq(ulReselType);

    return NAS_LMM_MSG_HANDLED;
}
/*lint +e961*/
/*lint +e960*/

VOS_UINT32 NAS_EMMC_RcvMmcGuSysInfoInd(MsgBlock *pMsg )
{
    (VOS_VOID)pMsg;

    NAS_EMM_PUBU_LOG_NORM("NAS_EMMC_RcvMmcGuSysInfoInd is entered.");

    /* 如果L模当前处于激活态，则不处理GU的SYS_INFO,直接丢弃*/
    if(NAS_LMM_CUR_LTE_SUSPEND != NAS_EMM_GetCurLteState())
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMMC_RcvMmcGuSysInfoNotify:LTE is not SUSPEND.");
        return NAS_LMM_MSG_HANDLED;
    }

    /*收到GU系统消息,停止运行的T3402以及T3411 */
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

/*lint -e960*/
/* GCF测试情况下如果3402在运行不停止3402，也不必清次数，待3402超时后自然会清，
   3411定时器先不修改，暂还按照原来的停止处理 */
#if (VOS_OS_VER != VOS_WIN32)
    if(PS_SUCC == LPS_OM_IsTestMode())
    {
        if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
        {
           return NAS_LMM_MSG_HANDLED;
        }
    }
#endif
/*lint +e960*/

    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);

    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(0);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;


    return NAS_LMM_MSG_HANDLED;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif









