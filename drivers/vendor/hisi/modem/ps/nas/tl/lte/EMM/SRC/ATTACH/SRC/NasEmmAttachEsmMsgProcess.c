




/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasMmlLib.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMATTACHESMMSGPROC_C
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

/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_UINT32  NAS_EMM_EsmPdnRspMsgChk( const EMM_ESM_PDN_CON_RSP_STRU *pMsgpMsg)
{
    /*消息内容检查*/
    if ((EMM_ESM_PDN_CON_RSLT_SUCC == pMsgpMsg->ulRst) ||
         (EMM_ESM_PDN_CON_RSLT_FAIL == pMsgpMsg->ulRst))
    {
        return  NAS_EMM_PARA_VALID;

    }
    else
    {
        return  NAS_EMM_PARA_INVALID;

    }

}
/*lint -e960*/
/*lint -e961*/
VOS_VOID *NAS_EMM_AttRsltCommProc(VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pstMmcAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AttRsltCommProc:构造LMM_MMC_ATTACH_IND!");

    /*申请消息内存*/
    pstMmcAttMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_ATTACH_IND_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if ((NAS_EMM_NULL_PTR == pstMmcAttMsg))
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AttRsltCommProc: MSG ALLOC ERR!");
        return NAS_EMM_NULL_PTR;

    }

    NAS_LMM_MEM_SET(pstMmcAttMsg,0,sizeof(LMM_MMC_ATTACH_IND_STRU));

    /*构造LMM_MMC_ATTACH_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pstMmcAttMsg,
                                        sizeof(LMM_MMC_ATTACH_IND_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /*填充消息ID*/

    pstMmcAttMsg->ulMsgId       = ID_LMM_MMC_ATTACH_IND;

    /*填充消息内容----OPID*/
    pstMmcAttMsg->ulOpId        = NAS_EMM_OPID_TYPE_ATTACH_IND;


    return pstMmcAttMsg;

}
VOS_VOID    NAS_EMM_AppSendAttInd
(
    VOS_VOID                *pstRcvMsg
)
{
    LMM_MMC_ATTACH_IND_STRU            *pstMmcAttIndMsg = NAS_EMM_NULL_PTR;
    LMM_MMC_ATTACH_CNF_STRU            *pstAttCnf;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttInd: Input para is invalid!");
        return;
    }

    pstAttCnf = (LMM_MMC_ATTACH_CNF_STRU *)pstRcvMsg;

    /*申请消息内存*/
    pstMmcAttIndMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if ((NAS_EMM_NULL_PTR == pstMmcAttIndMsg))
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AttRsltCommProc: MSG ALLOC ERR!");
        return ;

    }

    NAS_LMM_MEM_SET(pstMmcAttIndMsg,0,sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pstMmcAttIndMsg,
                                        sizeof(LMM_MMC_ATTACH_CNF_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /* 填充消息 */
    NAS_LMM_MEM_CPY(                    pstMmcAttIndMsg,
                                        pstAttCnf,
                                        sizeof(LMM_MMC_ATTACH_CNF_STRU));

    pstMmcAttIndMsg->ulMsgId       = ID_LMM_MMC_ATTACH_IND;

    /*填充消息内容----OPID*/
    pstMmcAttIndMsg->ulOpId        = NAS_EMM_OPID_TYPE_ATTACH_IND;

    NAS_LMM_SendLmmMmcMsg(pstMmcAttIndMsg);
}
VOS_VOID    NAS_EMM_MmcSendAttCnf
(
    MMC_LMM_ATTACH_RSLT_ENUM_UINT32                ulAttRslt
)
{
    LMM_MMC_ATTACH_CNF_STRU            *pstAttCnf;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MmcSendAttCnf:回复LMM_MMC_ATTACH_CNF!");

    /*申请消息内存*/
    pstAttCnf = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if ((NAS_EMM_NULL_PTR == pstAttCnf))
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AttRsltCommProc: MSG ALLOC ERR!");
        return ;

    }

    NAS_LMM_MEM_SET(pstAttCnf,0,sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pstAttCnf,
                                        sizeof(LMM_MMC_ATTACH_CNF_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /* 填充消息 */

    pstAttCnf->ulMsgId       = ID_LMM_MMC_ATTACH_CNF;

    /*填充消息内容----OPID*/
    pstAttCnf->ulOpId        = NAS_EMM_GetAppMsgOpId();

    pstAttCnf->ulAttachRslt  = ulAttRslt;
    pstAttCnf->ulReqType     = NAS_EMM_GLO_GetAttReqType();

    NAS_LMM_SendLmmMmcMsg(pstAttCnf);

    /*清空APP_ATTACH_REQ消息*/
    NAS_EMM_ClearAppMsgPara();
}
VOS_VOID    NAS_EMM_AppSendAttSucc( VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pstMmcAttMsg = NAS_EMM_NULL_PTR;

     NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_SUCCESS);
    }

    /* 构造 LMM_MMC_ATTACH_IND */
    pstMmcAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstMmcAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttSucc:MSG MALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    pstMmcAttMsg->ulAttachRslt      = MMC_LMM_ATT_RSLT_SUCCESS;

    /*填充消息内容----ATTACH请求类型*/
    pstMmcAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*填充消息内容----网侧结果类型*/
    pstMmcAttMsg->bitOpCnRslt       = NAS_EMM_BIT_SLCT;

    if (EMM_ATTACH_RSLT_EPS_ONLY == NAS_EMM_GLO_AD_GetAttRslt())
    {
        pstMmcAttMsg->ulCnRslt      = MMC_LMM_ATT_CN_RSLT_EPS_ONLY;
    }
    else if (EMM_ATTACH_RSLT_COMBINED_ATTACH == NAS_EMM_GLO_AD_GetAttRslt())
    {
        pstMmcAttMsg->ulCnRslt      = MMC_LMM_ATT_CN_RSLT_COMBINED_EPS_IMSI;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_AppSendAttSucc:CN RSLT is err!");
    }

    /*填充消息内容----EPLMN*/
    pstMmcAttMsg->bitOpEplmn    = NAS_EMM_BIT_SLCT;
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr())
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttSucc:EPLMN NUM =",
                                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr()->ulPlmnNum);

        NAS_LMM_MEM_CPY(                &pstMmcAttMsg->stEplmnList,
                                        NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    /*填充消息内容----LAI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetLai())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:LAI");
        pstMmcAttMsg->bitOpLai      = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 &pstMmcAttMsg->stLai,
                                        NAS_EMM_GLO_AD_GetLaiAddr(),
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    /*填充消息内容----TMSI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetMsId())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:TMSI");
        pstMmcAttMsg->bitOpMsIdentity     = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 pstMmcAttMsg->stMsIdentity.aucMsIdentity,
                                        NAS_EMM_GLO_AD_GetMsIdAddr(),
                                        NAS_MAX_SIZE_MS_IDENTITY);

    }
    /*填充消息内容----EMM CAUSE*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetEmmCau())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:EMM CAUSE");
        pstMmcAttMsg->bitOpCnCause      = NAS_EMM_BIT_SLCT;
        pstMmcAttMsg->ucCnCause         = NAS_EMM_GLO_AD_GetEmmCau();
    }

    /* xiongxianghui00253310 modify 2014-01-13 begin */
    /*
    if ((NAS_LMM_CAUSE_MSC_UNREACHABLE == NAS_EMM_GLO_AD_GetEmmCau())
        || (NAS_LMM_CAUSE_NETWORK_FAILURE == NAS_EMM_GLO_AD_GetEmmCau())
        || (NAS_LMM_CAUSE_PROCEDURE_CONGESTION == NAS_EMM_GLO_AD_GetEmmCau()))
    */
    if (0 != NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    /* xiongxianghui00253310 modify 2014-01-13 end */
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:tau attempt couter");
        pstMmcAttMsg->bitOpAtmpCnt      = NAS_EMM_BIT_SLCT;
        pstMmcAttMsg->ulAttemptCount    = NAS_EMM_TAU_GetEmmTAUAttemptCnt();
    }

    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pstMmcAttMsg);

    return;
}


VOS_VOID NAS_EMM_AppSendAttRejEpsOnly( VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_CN_REJ);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();

    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttRejEpsOnly:MSG MALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt      = MMC_LMM_ATT_RSLT_CN_REJ;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*填充消息内容----网侧结果类型*/
    pEmmAppAttMsg->bitOpCnRslt  = NAS_EMM_BIT_SLCT;
    pEmmAppAttMsg->ulCnRslt     = MMC_LMM_ATT_CN_RSLT_EPS_ONLY;

    /* 只有#2#16#17#18#22时填原因值，其它原因值或者无原因值的情况不填 */
    pEmmAppAttMsg->bitOpCnCause      = NAS_EMM_BIT_NO_SLCT;
    pEmmAppAttMsg->ucCnCause         = NAS_EMM_CAUSE_BUTT;

    /*填充消息内容----EPLMN*/
    pEmmAppAttMsg->bitOpEplmn    = NAS_EMM_BIT_SLCT;
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr())
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttRejEpsOnly:EPLMN NUM =",
                                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr()->ulPlmnNum);

        NAS_LMM_MEM_CPY(                &pEmmAppAttMsg->stEplmnList,
                                        NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    /*填充消息内容----LAI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetLai())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRejEpsOnly:LAI");
        pEmmAppAttMsg->bitOpLai      = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 &pEmmAppAttMsg->stLai,
                                        NAS_EMM_GLO_AD_GetLaiAddr(),
                                        sizeof(MMC_LMM_LAI_STRU));
    }

    /*填充消息内容----TMSI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetMsId())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRejEpsOnly:TMSI");
        pEmmAppAttMsg->bitOpMsIdentity     = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                pEmmAppAttMsg->stMsIdentity.aucMsIdentity,
                                        NAS_EMM_GLO_AD_GetMsIdAddr(),
                                        NAS_MAX_SIZE_MS_IDENTITY);

    }

    if (NAS_EMM_NULL != NAS_EMM_GLO_AD_GetAttAtmpCnt())
    {
       pEmmAppAttMsg->bitOpAtmpCnt   = NAS_EMM_BIT_SLCT;
       pEmmAppAttMsg->ulAttemptCount = NAS_EMM_GLO_AD_GetAttAtmpCnt();
    }

    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}
VOS_VOID    NAS_EMM_AppSendAttRej( VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRej:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_CN_REJ);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();

    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttRej:MSG MALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt      = MMC_LMM_ATT_RSLT_CN_REJ;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*填充消息内容----网侧拒绝原因*/
    pEmmAppAttMsg->bitOpCnCause      = NAS_EMM_BIT_SLCT;
    pEmmAppAttMsg->ucCnCause         = NAS_EMM_GLO_AD_GetAttRejCau();

    if (NAS_EMM_NULL != NAS_EMM_GLO_AD_GetAttAtmpCnt())
    {
       pEmmAppAttMsg->bitOpAtmpCnt   = NAS_EMM_BIT_SLCT;
       pEmmAppAttMsg->ulAttemptCount = NAS_EMM_GLO_AD_GetAttAtmpCnt();
    }

    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}
VOS_VOID    NAS_EMM_AppSendAttTimerExp(VOS_VOID)
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttTimerExp:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_TIMER_EXP);
    }


    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();

    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttTimerExp:MSG MALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt      = MMC_LMM_ATT_RSLT_TIMER_EXP;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*填充消息内容----ATTACH尝试次数*/
    pEmmAppAttMsg->bitOpAtmpCnt      = NAS_EMM_BIT_SLCT;
    pEmmAppAttMsg->ulAttemptCount    = NAS_EMM_GLO_AD_GetAttAtmpCnt();

    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);
    return;
}
VOS_VOID NAS_EMM_AppSendAttFailWithPara( VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttFailWithPara:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_FAILURE);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttFailWithPara:MSG ALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt         = MMC_LMM_ATT_RSLT_FAILURE;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    if (NAS_EMM_NULL != NAS_EMM_GLO_AD_GetAttAtmpCnt())
    {
       pEmmAppAttMsg->bitOpAtmpCnt   = NAS_EMM_BIT_SLCT;
       pEmmAppAttMsg->ulAttemptCount = NAS_EMM_GLO_AD_GetAttAtmpCnt();
    }

    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}



VOS_VOID    NAS_EMM_AppSendAttOtherType
(
    MMC_LMM_ATTACH_RSLT_ENUM_UINT32     ulAttRslt
)
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttOtherType:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(ulAttRslt);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttOtherType:MSG ALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    if ((MMC_LMM_ATT_RSLT_ACCESS_BAR == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_AUTH_REJ == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_FAILURE == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_ESM_FAILURE == ulAttRslt)||
        (MMC_LMM_ATT_RSLT_MT_DETACH_FAILURE == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_MO_DETACH_FAILURE == ulAttRslt)||
        (MMC_LMM_ATT_RSLT_T3402_RUNNING == ulAttRslt))
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttOtherType:ulAttRslt =",
                                ulAttRslt);

        pEmmAppAttMsg->ulAttachRslt  = ulAttRslt;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_AppSendAttOtherType:ulAttRslt is err!");
    }

    /*填充消息内容----ATTACH请求类型*/
    /*NAS_EMM_SetAttType();*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);
    return;
}
VOS_VOID    NAS_EMM_AppSendAttRstDefaultReqType
(
    MMC_LMM_ATTACH_RSLT_ENUM_UINT32     ulAttRslt
)
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRstDefaultReqType:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(ulAttRslt);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttRstDefaultReqType:MSG ALLOC FAIL");
        return;
    }

    /*填充消息内容----ATTACH结果*/
    if ((MMC_LMM_ATT_RSLT_ACCESS_BAR == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_AUTH_REJ == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_FAILURE == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_SUCCESS == ulAttRslt)||
        (MMC_LMM_ATT_RSLT_ESM_FAILURE == ulAttRslt)||
        (MMC_LMM_ATT_RSLT_MT_DETACH_FAILURE == ulAttRslt) ||
        (MMC_LMM_ATT_RSLT_MO_DETACH_FAILURE == ulAttRslt)||
        (MMC_LMM_ATT_RSLT_T3402_RUNNING == ulAttRslt))
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttRstDefaultReqType:ulAttRslt =",
                                ulAttRslt);

        pEmmAppAttMsg->ulAttachRslt  = ulAttRslt;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_AppSendAttOtherType:ulAttRslt is err!");
    }

    /*填充消息内容----ATTACH请求类型*/
    NAS_EMM_SetAttType();

    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}



VOS_VOID    NAS_EMM_AppSendAttRstInTau(MMC_LMM_ATTACH_RSLT_ENUM_UINT32 ulAttRst)
{
    LMM_MMC_ATTACH_CNF_STRU             *pEmmAppAttMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRstInTau:回复LMM_MMC_ATTACH_CNF!");

    /*申请消息内存*/
    pEmmAppAttMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if ((NAS_EMM_NULL_PTR == pEmmAppAttMsg))
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AttRsltCommProc: MSG ALLOC ERR!");
        return ;

    }

    NAS_LMM_MEM_SET(pEmmAppAttMsg,0,sizeof(LMM_MMC_ATTACH_CNF_STRU));

    /*消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pEmmAppAttMsg,
                                        sizeof(LMM_MMC_ATTACH_CNF_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /* 填充消息 */

    pEmmAppAttMsg->ulMsgId              = ID_LMM_MMC_ATTACH_CNF;

    /*填充消息内容----OPID*/
    pEmmAppAttMsg->ulOpId               = NAS_EMM_GetAppMsgOpId();

    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt         = ulAttRst;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType            = NAS_EMM_GLO_GetAttReqType();

    /*向MMC发送LMM_MMC_ATTACH_CNF，不报LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    /*清空APP_ATTACH_REQ消息*/
    NAS_EMM_ClearAppMsgPara();
    return;
}

VOS_VOID NAS_EMM_AppSendAttFailWithAllPara( VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;


    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttFailWithAllPara:ENTER!");

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_FAILURE);
    }

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();

    /*判断申请结果，若失败打印错误并退出*/
    if ((NAS_EMM_NULL_PTR == pEmmAppAttMsg))
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttFailWithAllPara: MSG ALLOC ERR!");
        return;

    }


    /*填充消息内容----ATTACH结果*/
    pEmmAppAttMsg->ulAttachRslt         = MMC_LMM_ATT_RSLT_FAILURE;

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    if (NAS_EMM_NULL != NAS_EMM_GLO_AD_GetAttAtmpCnt())
    {
       pEmmAppAttMsg->bitOpAtmpCnt   = NAS_EMM_BIT_SLCT;
       pEmmAppAttMsg->ulAttemptCount = NAS_EMM_GLO_AD_GetAttAtmpCnt();
    }

    /*填充消息内容----网侧结果类型*/
    pEmmAppAttMsg->bitOpCnRslt       = NAS_EMM_BIT_SLCT;

    /* zhangcaixia for syscfg_attach begin */
    if (EMM_ATTACH_RSLT_EPS_ONLY == NAS_EMM_GLO_AD_GetAttRslt())
    {
        pEmmAppAttMsg->ulCnRslt      = MMC_LMM_ATT_CN_RSLT_EPS_ONLY;
    }
    else if (EMM_ATTACH_RSLT_COMBINED_ATTACH == NAS_EMM_GLO_AD_GetAttRslt())
    {
        pEmmAppAttMsg->ulCnRslt      = MMC_LMM_ATT_CN_RSLT_COMBINED_EPS_IMSI;
    }

    else
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_AppSendAttSucc:CN RSLT is err!");
    }

    /*填充消息内容----EPLMN*/
    pEmmAppAttMsg->bitOpEplmn    = NAS_EMM_BIT_SLCT;
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr())
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttSucc:EPLMN NUM =",
                                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr()->ulPlmnNum);

        NAS_LMM_MEM_CPY(                &pEmmAppAttMsg->stEplmnList,
                                        NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    /*填充消息内容----LAI*/
    /* zhangcaixia for syscfg_attach begin */
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetLai())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:LAI");
        pEmmAppAttMsg->bitOpLai      = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                &pEmmAppAttMsg->stLai,
                                        NAS_EMM_GLO_AD_GetLaiAddr(),
                                        sizeof(MMC_LMM_LAI_STRU));
    }

    /*填充消息内容----TMSI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetMsId())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:TMSI");
        pEmmAppAttMsg->bitOpMsIdentity     = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                pEmmAppAttMsg->stMsIdentity.aucMsIdentity,
                                        NAS_EMM_GLO_AD_GetMsIdAddr(),
                                        NAS_MAX_SIZE_MS_IDENTITY);
    }

    /*填充消息内容----EMM CAUSE*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetEmmCau())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:EMM CAUSE");
        pEmmAppAttMsg->bitOpCnCause      = NAS_EMM_BIT_SLCT;
        pEmmAppAttMsg->ucCnCause         = NAS_EMM_GLO_AD_GetEmmCau();
    }

    /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}
VOS_VOID    NAS_EMM_AppSendAttPsNotAllow(  VOS_VOID )
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttPsNotAllow:ENTER!");

    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttPsNotAllow:MSG ALLOC FAIL");
        return;
    }

    pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW;


    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = MMC_LMM_ATT_TYPE_BUTT;

    /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    /*清空MMC_LMM_ATTACH_REQ消息*/
    NAS_EMM_ClearAppMsgPara();
    return;
}

/* zhangcaixia for syscfg_attach end */

VOS_VOID  NAS_EMM_AppSendAttRsltForb
(
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 ulForbInfo
)
{
    LMM_MMC_ATTACH_IND_STRU             *pEmmAppAttMsg;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttRsltForb:ENTER!");


    pEmmAppAttMsg = NAS_EMM_AttRsltCommProc();
    if (NAS_EMM_NULL_PTR == pEmmAppAttMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttRsltForb:MSG ALLOC FAIL");
        return;
    }

    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AppSendAttRsltForb:ulForbInfo =",
                                  ulForbInfo);
    /*填充消息内容----ATTACH结果*/
    if (EMMC_EMM_FORBIDDEN_PLMN == ulForbInfo)
    {
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FORBID_PLMN;
    }
    else if (EMMC_EMM_FORBIDDEN_PLMN_FOR_GPRS == ulForbInfo)
    {
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FORBID_PLMN_FOR_GPRS;
    }
    else if (EMMC_EMM_FORBIDDEN_TA_FOR_ROAMING == ulForbInfo)
    {
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FORBID_TA_FOR_ROAMING;
    }
    else if (EMMC_EMM_FORBIDDEN_TA_FOR_RPOS == ulForbInfo)
    {
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FORBID_TA_FOR_RPOS;
    }
    else if (EMMC_EMM_NO_FORBIDDEN == ulForbInfo)
    {
        /* 如果系统消息未被禁且调用此函数上报ATTACH结果，则一定是ANY CELL的情况 */
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FAILURE;
    }

    else
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_AppSendAttRsltForb:ulForbInfo is err!");
        pEmmAppAttMsg->ulAttachRslt  = MMC_LMM_ATT_RSLT_FAILURE;
    }

    /*填充消息内容----ATTACH请求类型*/
    pEmmAppAttMsg->ulReqType         = NAS_EMM_GLO_GetCnAttReqType();

    /* 向用户发LMM_MMC_ATACH_CNF */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_MmcSendAttCnf(pEmmAppAttMsg->ulAttachRslt);
    }
    /*向MMC发送LMM_MMC_ATTACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(pEmmAppAttMsg);

    return;
}
VOS_UINT32  NAS_EMM_CountMrrcDataReqLen(VOS_UINT32 ulNasCnMsgLen)
{
    VOS_UINT32                          ulMsgLen;
    ulMsgLen                            = (sizeof(NAS_EMM_MRRC_DATA_REQ_STRU)+
                                          (ulNasCnMsgLen-NAS_EMM_LEN_VOS_MSG_HEADER))
                                          -NAS_EMM_4BYTES_LEN;

    return ulMsgLen;
}


VOS_VOID    NAS_EMM_CompCnAttachReqNasMsg(  VOS_UINT8      *pucAttReqCn,
                                            VOS_UINT32     *pulIndex)
{
    VOS_UINT32                          ulIndex     = NAS_LMM_NULL;
    VOS_UINT32                          ulIeLength = NAS_LMM_NULL;

    VOS_UINT8                           ucKsi;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_CompCnAttachReqNasMsg: is entered");

    ulIndex                             = *pulIndex;

    /* 填充 Protocol Discriminator + Security header type */
    pucAttReqCn[ulIndex++]              = EMM_CN_MSG_PD_EMM;

    /* 填充 Attach request message identity   */
    pucAttReqCn[ulIndex++]              = NAS_EMM_CN_MT_ATTACH_REQ;

    /*填充Attach type + NKSI*/
    ucKsi = NAS_EMM_SecuGetKSIValue();
    NAS_EMM_SetAttType();
    NAS_EMM_ATTACH_LOG1_NORM("Attach Type: ", NAS_EMM_GLO_GetCnAttReqType());
    pucAttReqCn[ulIndex++]              = ((VOS_UINT8)(NAS_EMM_GLO_GetCnAttReqType())|
                                           ((VOS_UINT8)(ucKsi << NAS_LMM_MOVEMENT_4_BITS)));

    /*填充EPS mobile identity*/
    NAS_EMM_EncodeEpsMobileId(          &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /* 填充UE network capability 是必选IE, LV，3~14*/
    NAS_EMM_EncodeUeNetCapa(            &(pucAttReqCn[ulIndex]),
                                        NAS_LMM_IE_FORMAT_LV,
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充ESM message container*/
    NAS_EMM_EncodeEsmContainer(         &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Old P-TMSI signature*/
    NAS_EMM_EncodePtmsiSignature(       &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Additional GUTI*/
    NAS_EMM_EncodeAdditionalGuti(       &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Last visited registered TAI*/
    NAS_EMM_EncodeLRVTai(               &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充DRX parameter*/
    NAS_EMM_EncodeDrxPara(              &(pucAttReqCn[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_ATTACH_REQ);
    ulIndex                             += ulIeLength;

    /*填充MS network capability*/
    NAS_EMM_EncodeMsNetworkCapability( &(pucAttReqCn[ulIndex]),
                                       &ulIeLength,
                                        NAS_EMM_CN_MT_ATTACH_REQ);
    ulIndex                             += ulIeLength;


    /*填充LAI*/
    NAS_EMM_EncodeLai(                  &(pucAttReqCn[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_ATTACH_REQ);
    ulIndex                             += ulIeLength;


    /*填充TMSI STATUS*/
    NAS_EMM_EncodeTmsiStatus(           &(pucAttReqCn[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_ATTACH_REQ);
    ulIndex                             += ulIeLength;

    /*填充Mobile station classmark 2*/
    NAS_EMM_EncodeMsClassMark2(         &(pucAttReqCn[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_ATTACH_REQ);
    ulIndex                             += ulIeLength;

    /*填充Mobile station classmark 3*/
    NAS_EMM_EncodeMsClassMark3(         &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Supported Codecs*/
    NAS_EMM_EncodeSupportedCodecs(      &(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Additional update type*/
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        NAS_EMM_EncodeAdditionUpdateType(   &(pucAttReqCn[ulIndex]),
                                            &ulIeLength);
        ulIndex                             += ulIeLength;
    }

    /*填充Voice domain preference and UE's usage setting*/
    NAS_EMM_EncodeVoiceDomainAndUsageSetting(&(pucAttReqCn[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;
    if (NAS_RELEASE_CTRL)
    {
        /* 填充Old GUTI type */
        NAS_EMM_EncodeGutiType(             &(pucAttReqCn[ulIndex]),
                                            &ulIeLength);
        ulIndex                             += ulIeLength;
    }
    *pulIndex = ulIndex;

}

VOS_VOID    NAS_EMM_CompCnAttachReq(NAS_EMM_MRRC_DATA_REQ_STRU *pAttReqMsg)
{
    NAS_EMM_MRRC_DATA_REQ_STRU         *pTempAttReqMsg  =pAttReqMsg;
    VOS_UINT32                          ulIndex         = 0;
    VOS_UINT32                          ulDataReqMsgLenNoHeader;

    if (NAS_EMM_NULL_PTR == pAttReqMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompCnAttachReq: Mem Alloc Err!");
        return;
    }

    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_NULL);

    /*编码ATTACH REQ空口消息*/
    NAS_EMM_CompCnAttachReqNasMsg(pTempAttReqMsg->stNasMsg.aucNasMsg, &ulIndex);

    /*内部消息长度计算*/
    ulDataReqMsgLenNoHeader             = NAS_EMM_CountMrrcDataReqLen(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < ulDataReqMsgLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_CompCnAttachReq, Size error");
        return ;
    }

    /*填充NAS消息长度*/
    pTempAttReqMsg->stNasMsg.ulNasMsgSize    = ulIndex;

    /*填充消息头*/
    NAS_EMM_COMP_AD_INTRA_MSG_HEADER(pTempAttReqMsg, ulDataReqMsgLenNoHeader);

    /*填充消息ID*/
    pTempAttReqMsg->ulMsgId                  = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
    /*填充消息内容*/
    if ((EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
        || (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau()))
    {
        pTempAttReqMsg->enEstCaue           = LRRC_LNAS_EST_CAUSE_EMERGENCY_CALL;
        pTempAttReqMsg->enCallType          = LRRC_LNAS_CALL_TYPE_EMERGENCY_CALL;
    }
    else
    {
        pTempAttReqMsg->enEstCaue           = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
        pTempAttReqMsg->enCallType          = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    }
    pTempAttReqMsg->enEmmMsgType             = NAS_EMM_MSG_ATTACH_REQ;

    pTempAttReqMsg->enDataCnf               = LRRC_LMM_DATA_CNF_NEED;
    #if 0
    /*预留消息重传接口*/
    if(ulDataReqMsgLenNoHeader < NAS_EMM_CN_MSGCON_MAX_LEN)
    {
        NAS_LMM_MEM_CPY(             g_stAirMsgEncode.aucMsg,
                                        pTempAttReqMsg,
                                        ulDataReqMsgLenNoHeader);

        g_stAirMsgEncode.ulMsgLen =ulDataReqMsgLenNoHeader;
    }
    else
    {
        /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_CompCnAttachReq: MSG LEN OVERFLOW!");
        return;
    }
    #endif

    return;
}


VOS_VOID    NAS_EMM_EncodeUeNetCapa(    VOS_UINT8                  *pMsg,
                                        NAS_LMM_IE_FORMAT_ENUM_UINT8 ucIeFormat,
                                        VOS_UINT32                 *pulIeLength)
{
    VOS_UINT8                          *pMsgPreAddr;
    NAS_MM_UE_NET_CAP_STRU             *pstUeNetCap;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength) ||
       (NAS_LMM_IE_FORMAT_ERR(ucIeFormat)))
    {
        NAS_EMM_ATTACH_LOG2_ERR(        "NAS_EMM_EncodeUeNetCapa ERR !! pMsg = , pIeLength = ",
                                        pMsg,
                                        pulIeLength);
        return;
    }

    /* 把IE放入消息的地址赋值给临时变量 */
    pMsgPreAddr     = pMsg;

    /* 输出的参数IE长度初始化为0 */
    (*pulIeLength)  = NAS_EMM_NULL;

    /* 指向 UeNetCap*/
    pstUeNetCap = NAS_LMM_GetEmmInfoSecuParaUeNetCapAddr();

    switch(ucIeFormat)
    {
        case    NAS_LMM_IE_FORMAT_LV:
                (*pMsgPreAddr) = pstUeNetCap->ucUeNetCapLen;
                pMsgPreAddr ++;         /* 地址加1, 指向V的首地址 */
                (*pulIeLength) += 1;    /* NAS_LMM_IE_FORMAT_LV中的L,长度增加 1*/

                /* 不BREAK，继续使用下面的CASE完成V的填充 */
                /*lint -fallthrough*/
                /*lint +fallthrough*/
        case    NAS_LMM_IE_FORMAT_V:
                NAS_LMM_MEM_CPY(        pMsgPreAddr,
                                        (&(pstUeNetCap->ucUeNetCapLen) + 1),
                                        (pstUeNetCap->ucUeNetCapLen));
                (*pulIeLength) += (pstUeNetCap->ucUeNetCapLen);
                break;


        default :
                NAS_EMM_ATTACH_LOG1_ERR("NAS_EMM_EncodeUeNetCapa ERR!! ucIeFormat =",
                                        ucIeFormat);
                break;
    }

    return;
}



VOS_VOID  NAS_EMM_CompMappedGuti
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_LMM_GUTI_STRU                    stMappedGuti = {0};

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_CompMappedGuti is entered!");

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompMappedGuti: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*获取Mapped*/
    NAS_EMM_DeriveMappedGuti(&stMappedGuti);

    /*填充GUTI*/
    pucReqMsg[ulIndex++]                = NAS_EMM_AD_LEN_GUTI;
    pucReqMsg[ulIndex++]                = NAS_EMM_AD_ID_GUTI;
    NAS_LMM_MEM_CPY(                     &(pucReqMsg[ulIndex]),
                                        stMappedGuti.stPlmnId.aucPlmnId,
                                        NAS_EMM_AD_LEN_PLMN_ID);
    ulIndex                             += NAS_EMM_AD_LEN_PLMN_ID;

    pucReqMsg[ulIndex++]                = stMappedGuti.stMmeGroupId.ucGroupId;
    pucReqMsg[ulIndex++]                = stMappedGuti.stMmeGroupId.ucGroupIdCnt;

    pucReqMsg[ulIndex++]                = stMappedGuti.stMmeCode.ucMmeCode;

    pucReqMsg[ulIndex++]                = stMappedGuti.stMTmsi.ucMTmsi;
    pucReqMsg[ulIndex++]                = stMappedGuti.stMTmsi.ucMTmsiCnt1;
    pucReqMsg[ulIndex++]                = stMappedGuti.stMTmsi.ucMTmsiCnt2;
    pucReqMsg[ulIndex++]                = stMappedGuti.stMTmsi.ucMTmsiCnt3;

    *pulIeLength                        = ulIndex;

    return;
}



VOS_VOID  NAS_EMM_CompNativeGuti
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_CompNativeGuti is entered!");

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompNativeGuti: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*填充GUTI*/
    /*============ GUTI ============ begin */
    pucReqMsg[ulIndex++]                = NAS_EMM_GLO_AD_GetLen();
    pucReqMsg[ulIndex++]                = (NAS_EMM_GLO_AD_GetOeToi()|
                                             NAS_EMM_HIGH_HALF_BYTE_F);

    NAS_LMM_MEM_CPY(                     &(pucReqMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetGutiPlmn(),
                                        NAS_EMM_AD_LEN_PLMN_ID);
    ulIndex                             += NAS_EMM_AD_LEN_PLMN_ID;
    pucReqMsg[ulIndex++]                = NAS_EMM_GLO_AD_GetMmeGroupId();
    pucReqMsg[ulIndex++]                = NAS_EMM_GLO_AD_GetMMeGroupIdCnt();
    pucReqMsg[ulIndex++]                = NAS_EMM_GLO_AD_GetMMeCode();

    NAS_LMM_MEM_CPY(                     &(pucReqMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetMTmsiAddr(),
                                        sizeof(NAS_EMM_MTMSI_STRU));
    ulIndex                             += sizeof(NAS_EMM_MTMSI_STRU);

    *pulIeLength                        = ulIndex;

    return;
}


VOS_VOID  NAS_EMM_CompImsi
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompImsi: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*填充IMSI*/
    /*============ IMSI ============ begin */
    NAS_LMM_MEM_CPY(                     &(pucReqMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetImsi(),
                                        NAS_EMM_AD_LEN_CN_MSG_IMSI);

    ulIndex                             += NAS_EMM_AD_LEN_CN_MSG_IMSI;
    *pulIeLength                        = ulIndex;
    /*============ IMSI ============ end */

    return;
}
VOS_VOID  NAS_EMM_CompImei
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*填充IMEI*/
    /*============ IMEI ============ begin */
    NAS_LMM_MEM_CPY(                     &(pucReqMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetImei(),
                                        NAS_EMM_AD_LEN_CN_MSG_IMEI);

    /* GU NV中存储的IMEI与ATTACH REQ中EPS mobile identity中的结构有所不同，
       NV IMEI中的type of identity为'010'，而ATTACH REQ中携带的IMEI中的
       type of identity为'011' */
    pucReqMsg[1]                        &= (~NAS_EMM_LOW_3_BITS_F);
    pucReqMsg[1]                        |= 0x03;

    ulIndex                             += NAS_EMM_AD_LEN_CN_MSG_IMEI;
    *pulIeLength                        = ulIndex;
    /*============ IMEI ============ end */

    return;
}
VOS_VOID  NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulLen       = NAS_EMM_NULL;

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId is entered!");
    if (NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId: P-TMSI and RAI VALID");

        NAS_EMM_CompMappedGuti(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
    else if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_IMSI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId: imsi");

        NAS_EMM_CompImsi(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId: imei");

        NAS_EMM_CompImei(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
}


VOS_VOID  NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulLen       = NAS_EMM_NULL;

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId is entered!");
    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId:GUTI VALID");
        NAS_EMM_CompNativeGuti(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
        return;
    }

    if (NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId:P-TMSI and RAI VALID");
        NAS_EMM_CompMappedGuti(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
        return;
    }

    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_IMSI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId:imsi");
        NAS_EMM_CompImsi(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId:imei");
        NAS_EMM_CompImei(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
}


VOS_VOID  NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulLen       = NAS_EMM_NULL;

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId is entered!");
    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId:GUTI VALID");
        NAS_EMM_CompNativeGuti(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
    else if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_IMSI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId:imsi");
        NAS_EMM_CompImsi(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
    else
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId:imei");
        NAS_EMM_CompImei(&(pucReqMsg[0]),&ulLen);
        *pulIeLength            = ulLen;
    }
}


VOS_VOID  NAS_EMM_EncodeEpsMobileId
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          ulLen = NAS_EMM_NULL;

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    if (NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
        {
            NAS_EMM_MultiModeTinPtmsiEncodeEpsMobileId( &(pucReqMsg[ulIndex]),
                                                        &ulLen);
            *pulIeLength            = ulLen;
        }
        else if ((MMC_LMM_TIN_GUTI == NAS_EMM_GetTinType()) ||
            (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
        {
            NAS_EMM_MultiModeTinGutiOrRatRelatedTmsiEncodeEpsMobileId(  &(pucReqMsg[ulIndex]),
                                                                        &ulLen);
            *pulIeLength            = ulLen;
        }
        else if (MMC_LMM_TIN_INVALID == NAS_EMM_GetTinType())
        {
            NAS_EMM_MultiModeTinInvalidEncodeEpsMobileId( &(pucReqMsg[ulIndex]),
                                                          &ulLen);
            *pulIeLength            = ulLen;
        }
        else
        {

        }
    }
    else
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeEpsMobileId:NOT SUPPOR MUTI MODE");
        if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
        {
            NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeEpsMobileId: GUTI VALID!");

            NAS_EMM_CompNativeGuti(&(pucReqMsg[ulIndex]),&ulLen);
            *pulIeLength                = ulLen;
        }
        else if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_IMSI())
        {
            NAS_EMM_CompImsi(&(pucReqMsg[ulIndex]),&ulLen);
            *pulIeLength            = ulLen;
        }
        else
        {
            NAS_EMM_CompImei(&(pucReqMsg[ulIndex]),&ulLen);
            *pulIeLength            = ulLen;
        }
    }

    return;
}


VOS_VOID  NAS_EMM_EncodeEsmContainer
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT16                          usEsmMsgLen = NAS_EMM_NULL;


    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeEsmContainer: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;


    usEsmMsgLen                         = (VOS_UINT16)NAS_EMM_GLO_AD_GetEsmMsgAddr()->ulMsgLen;
    pucReqMsg[ulIndex++]                = (usEsmMsgLen>>8)& 0xFF;

    pucReqMsg[ulIndex++]                = usEsmMsgLen & 0xFF;
    NAS_LMM_MEM_CPY(                    &(pucReqMsg[ulIndex]),
                                        (NAS_EMM_GLO_AD_GetEsmMsgAddr()->aucMsg),
                                        NAS_EMM_GLO_AD_GetEsmMsgAddr()->ulMsgLen);

    ulIndex                             += NAS_EMM_GLO_AD_GetEsmMsgAddr()->ulMsgLen;

    *pulIeLength                        = ulIndex;

    return;
}



VOS_VOID  NAS_EMM_EncodePtmsiSignature
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_GUMM_INFO_STRU                  stGuInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt;
    /*Coverity 告警消除 2013-9-26 Begin */
    VOS_UINT32                          ulEmmRslt;
    VOS_UINT32                          ulTinType;
    /*Coverity 告警消除 2013-9-26 End */

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodePtmsiSignature: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*初始化*/
    NAS_LMM_MEM_SET(&stGuInfo, 0, sizeof(NAS_GUMM_INFO_STRU));
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*判断是否存在LAI*/
    ulRslt = NAS_EMM_GetGuInfo(NAS_GUMM_PTMSI_SIGNATURE,&stGuInfo);
    /*Coverity 告警消除 2013-9-26 Begin */
    ulEmmRslt = NAS_EMM_CheckMutiModeSupport();
    ulTinType = NAS_EMM_GetTinType();
    /* 携带 old P-TMSI signature IE的条件:
       (MultiMode) && (TIN = P-TMSI或者INVALID)&&(P-TMSI,P-TMSI signature和RAI有效)
    */
    if (NAS_EMM_SUCC == ulEmmRslt)
    {
        if ((MMC_LMM_TIN_P_TMSI == ulTinType) ||
            (MMC_LMM_TIN_INVALID == ulTinType))
        {
            if ((MMC_LMM_SUCC == ulRslt)
            &&(NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity()))
            {
                NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodePtmsiSignature:P-TMSI SIGNATURE VALID");

                /*填充Old P-TMSI signature*/
                pucReqMsg[ulIndex++]    = NAS_EMM_AD_PTMSI_SIGNATURE_IEI;
                NAS_LMM_MEM_CPY(        &(pucReqMsg[ulIndex]),
                                        stGuInfo.u.stPtmsiSignature.aucPtmsiSign,
                                        NAS_MAX_SIZE_PTMSI_SIGNATURE);
                ulIndex                 += NAS_MAX_SIZE_PTMSI_SIGNATURE;
                *pulIeLength            = ulIndex;
            }
        }

    }
    /*PC REPLAY MODIFY BY LEILI END*/
    NAS_EMM_ATTACH_LOG2_INFO("NAS_EMM_EncodePtmsiSignature: NAS_EMM_CheckMutiModeSupport()= ,NAS_EMM_GetTinType() = ",
                            ulEmmRslt,
                            ulTinType);
    /*Coverity 告警消除 2013-9-26 End */
    return;
}
VOS_VOID  NAS_EMM_EncodeAdditionalGuti
(
    VOS_UINT8   *pMsg,
    VOS_UINT32  *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          ulLen = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeAdditionalGuti: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

     /* 携带Additional GUTI的条件:
      (MultiMode) && (TIN = P-TMSI且 P-TMSI和RAI有效) &&(GUTI有效)
    */
    if (NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        if ((MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
          &&(NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity()))
        {
            if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
            {
                NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeAdditionalGuti: GUTI VALID");
                pucReqMsg[ulIndex++]    = NAS_EMM_AD_ADDITION_GUTI_IEI;
                NAS_EMM_CompNativeGuti(&(pucReqMsg[ulIndex]), &ulLen);
                *pulIeLength            = ulLen + ulIndex;
            }
        }
    }
    return;
}


VOS_VOID  NAS_EMM_EncodeLRVTai
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeLRVTai: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;


    /*判断是否支持Last visited registered TAI且TAC不为0XFFFE*/
    if((NAS_EMM_AD_BIT_SLCT == NAS_EMM_GetLVRTaiOpBit()) &&
        (VOS_FALSE == NAS_LMM_CheckPlmnIsInvalid(NAS_LMM_GetEmmInfoLastRegPlmnAddr())) &&
        (VOS_FALSE == NAS_LMM_TacIsInvalid(NAS_LMM_GetEmmInfoLastRegTacAddr())))
    {
        /*填充Last visited registered TAI*/
        pucReqMsg[ulIndex++]            = NAS_EMM_AD_TAI_IEI;
        NAS_LMM_MEM_CPY(                &(pucReqMsg[ulIndex]),
                                        NAS_EMM_GLO_AD_GetLastRegPlmnId(),
                                        NAS_EMM_AD_LEN_PLMN_ID);
        ulIndex                         += NAS_EMM_AD_LEN_PLMN_ID;
        pucReqMsg[ulIndex++]            = NAS_EMM_GLO_AD_GetLastRegTac();
        pucReqMsg[ulIndex++]            = NAS_EMM_GLO_AD_GetLastRegTacCnt();

        *pulIeLength                    = ulIndex;

    }
    return;
}


VOS_VOID  NAS_EMM_EncodeDrxPara
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength,
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8      enCnMsgType
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT8                           ucDrxByte;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeDrxPara: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*从GU->L,且GU模携带DRX参数时，L模不携带*/
    if ((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
        && (NAS_EMM_SUCC == NAS_EMM_IsGuContainPsRegDrx())
        && (NAS_EMM_NO == NAS_LMM_GetEmmInfoDrxNetCapChange()))
    {
        return;
    }

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_EncodeDrxPara:ucNonDrxTimer = , ucSplitOnCcch = ",
                           NAS_MML_GetNonDrxTimer(),NAS_MML_GetSplitOnCcch());

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_EncodeDrxPara:ucSplitPgCode = , ucPsDrxLen = ",
                           NAS_MML_GetSplitPgCycleCode(),NAS_MML_GetUeEutranPsDrxLen());
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    if (NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
    {
        NAS_EMM_TAU_SaveEmmTauContainDrxFlag(NAS_EMM_YES);
    }
    #if 0
    /*判断是否支持DRX parameter*/
    if(NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoOpDrx())
    {
        pstDrxParam = NAS_LMM_GetEmmInfoDrxAddr();
        pucReqMsg[ulIndex++]            = NAS_EMM_AD_DRX_IEI;
        pucReqMsg[ulIndex++]            = pstDrxParam->ucSplitPgCode;
        ucDrxByte                       = pstDrxParam->ucNonDrxTimer;
        ucDrxByte                      |= (VOS_UINT8)(pstDrxParam->ucSplitOnCcch
                                                        << NAS_EMM_AD_MOVEMENT_3_BITS);
        ucDrxByte                      |= (VOS_UINT8)(pstDrxParam->ucPsDrxLen
                                                        << NAS_EMM_AD_MOVEMENT_4_BITS);
        pucReqMsg[ulIndex++]            = ucDrxByte;

        *pulIeLength                    = ulIndex;

    }
    #endif

    pucReqMsg[ulIndex++]            = NAS_EMM_AD_DRX_IEI;
    pucReqMsg[ulIndex++]            = NAS_EMM_GetSplitPgCycleCode();
    ucDrxByte                       = NAS_EMM_GetNonDrxTimer();
    ucDrxByte                      |= (VOS_UINT8)(NAS_EMM_GetSplitOnCcch()
                                                    << NAS_EMM_AD_MOVEMENT_3_BITS);
    ucDrxByte                      |= (VOS_UINT8)(NAS_EMM_GetUeEutranPsDrxLen()
                                                    << NAS_EMM_AD_MOVEMENT_4_BITS);
    pucReqMsg[ulIndex++]            = ucDrxByte;

    *pulIeLength                    = ulIndex;
    /*PC REPLAY MODIFY BY LEILI END*/
    return;
}




VOS_VOID  NAS_EMM_EncodeMsNetworkCapability
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength,
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8      enCnMsgType
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_EMM_MS_NET_CAP_STRU             *pstMsNetCap;
    NAS_LMM_PUB_INFO_STRU               *pstEmmInfo;
    NAS_MML_MS_NETWORK_CAPACILITY_STRU  stMsNetCapa;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeMsNetworkCapability: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;
    pstEmmInfo                          = NAS_LMM_GetEmmInfoAddr();
    pstMsNetCap                         = NAS_LMM_GetEmmInfoMsNetCapAddr();

    /* 检视如果是单模，则不填充MS network capability信元*/
    if(NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_EncodeMsNetworkCapability:Single Mode.");
        return;

    }

    /* 当前为多模下的TAU消息，且为Periodic TAU，则不填充MS network capability信元*/
    if((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
     &&(NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING == NAS_EMM_TAU_GetTAUtype()))
    {
        NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_EncodeMsNetworkCapability:Periodic TAU.");
        return;
    }

    /* 其他情况，填充MS network capability信元*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_EncodeMsNetworkCapability:Fill MsNetCap IE.");

    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    NAS_EMM_GetMsNetworkCapability(&stMsNetCapa);
    /*PC REPLAY MODIFY BY LEILI END*/

    /* 将从GU获取的MS网络能力保存到本地 */
    if (NAS_LMM_NULL != stMsNetCapa.ucNetworkCapabilityLen)
    {
        pstEmmInfo->bitOpMsNetCap = NAS_EMM_BIT_SLCT;
        pstMsNetCap->ucMsNetCapLen = stMsNetCapa.ucNetworkCapabilityLen;
        NAS_LMM_MEM_CPY(            pstMsNetCap->aucMsNetCap,
                                                stMsNetCapa.aucNetworkCapability,
                                                NAS_MM_MAX_MS_NET_CAP_LEN);
    }
    else
    {
        pstEmmInfo->bitOpMsNetCap = NAS_EMM_BIT_NO_SLCT;

    }
    if(NAS_MM_MAX_MS_NET_CAP_LEN >= stMsNetCapa.ucNetworkCapabilityLen)
    {
        pucReqMsg[ulIndex++]        = NAS_EMM_AD_MS_NET_CAPA_IEI;

        pucReqMsg[ulIndex++]        = stMsNetCapa.ucNetworkCapabilityLen;
        NAS_LMM_MEM_CPY(            &(pucReqMsg[ulIndex]),
                                    stMsNetCapa.aucNetworkCapability,
                                    stMsNetCapa.ucNetworkCapabilityLen);

        ulIndex                     += stMsNetCapa.ucNetworkCapabilityLen;

        *pulIeLength                = ulIndex;
        return;
    }

    if(NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_MsNetCap())
    {
        pucReqMsg[ulIndex++]        = NAS_EMM_AD_MS_NET_CAPA_IEI;
        pucReqMsg[ulIndex++]        = pstMsNetCap->ucMsNetCapLen;
        NAS_LMM_MEM_CPY(            &(pucReqMsg[ulIndex]),
                                    pstMsNetCap->aucMsNetCap,
                                    pstMsNetCap->ucMsNetCapLen);

        ulIndex                     += pstMsNetCap->ucMsNetCapLen;

        *pulIeLength                = ulIndex;

    }

    return;
}



VOS_VOID  NAS_EMM_EncodeLai
(
    VOS_UINT8 *pMsg,
    VOS_UINT32                         *pulIeLength,
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8      enCnMsgType
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_GUMM_INFO_STRU                  stGuInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeLai: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG3_NORM("NAS_EMM_EncodeLai:cn msg type,attach type,tau type",
                             enCnMsgType,
                             NAS_EMM_GLO_GetCnAttReqType(),
                             NAS_EMM_TAU_GetTAUtype());

    /* 如果是ATTACH REQ消息，且不是联合注册，则不携带LAI 信元 */
    if ((NAS_EMM_CN_MT_ATTACH_REQ == enCnMsgType)
        && (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI != NAS_EMM_GLO_GetCnAttReqType()))
    {
        return ;
    }

    /* 如果是TAU REQ消息，且不是联合TAU，则不携带LAI 信元 */
    if ((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING != NAS_EMM_TAU_GetTAUtype())
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI != NAS_EMM_TAU_GetTAUtype()))
    {
        return ;
    }

    /* 如果是IMSI CN DETACH 导致的联合TAU，则不携带LAI信息 */
    if ((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
        && (NAS_EMM_IMSI_DETACH_VALID == NAS_EMM_TAU_GetImsiDetachFlag()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeLai:Imsi cn detach no LAI!");
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*判断是否存在LAI*/
    ulRslt = NAS_EMM_GetGuInfo(NAS_GUMM_LAI,&stGuInfo);

    if (MMC_LMM_SUCC == ulRslt)
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeLai:LAI VALID");

        /*填充Old location area identification*/
        pucReqMsg[ulIndex++]            = NAS_EMM_AD_LAI_IEI;
        NAS_LMM_MEM_CPY(                 &(pucReqMsg[ulIndex]),
                                        stGuInfo.u.stLai.stPlmnId.aucPlmnId,
                                        NAS_EMM_AD_LEN_PLMN_ID);
        ulIndex                         += NAS_EMM_AD_LEN_PLMN_ID;

        pucReqMsg[ulIndex++]            = stGuInfo.u.stLai.stLac.ucLac;
        pucReqMsg[ulIndex++]            = stGuInfo.u.stLai.stLac.ucLacCnt;
        *pulIeLength                    = ulIndex;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    return;
}
VOS_VOID  NAS_EMM_EncodeTmsiStatus
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength,
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8      enCnMsgType
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_GUMM_INFO_STRU                  stGuInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeTmsiStatus: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG3_NORM("NAS_EMM_EncodeTmsiStatus:cn msg type,attach type,tau type",
                             enCnMsgType,
                             NAS_EMM_GLO_GetCnAttReqType(),
                             NAS_EMM_TAU_GetTAUtype());

    /* 如果是ATTACH REQ消息，且不是联合注册，则不携带TMSI STATUS 信元 */
    if ((NAS_EMM_CN_MT_ATTACH_REQ == enCnMsgType)
        && (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI != NAS_EMM_GLO_GetCnAttReqType()))
    {
        return ;
    }

    /* 如果是TAU REQ消息，且不是联合TAU，则不携带TMSI STATUS 信元 */
    if ((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING != NAS_EMM_TAU_GetTAUtype())
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI != NAS_EMM_TAU_GetTAUtype()))
    {
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ulRslt = NAS_EMM_GetGuInfo(NAS_GUMM_TMSI_STATUS,&stGuInfo);

    /* 如果是IMSI CN DETACH 导致的联合TAU，或者GU模不存在TMSI信息，则编码TMSI STATUS信元 */
    if (((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
                && (NAS_EMM_IMSI_DETACH_VALID == NAS_EMM_TAU_GetImsiDetachFlag()))
        || ((MMC_LMM_SUCC == ulRslt) && (MMC_LMM_TMSI_STATUS_INVALID == stGuInfo.u.ulTmsiStatus)))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeTmsiStatus:TMSI STATUS INVALID");

        /*填充TMSI IEI*/
        pucReqMsg[ulIndex]              = NAS_EMM_IEI_TMSI;

        /* 填充 TMSI的 spare */
        pucReqMsg[ulIndex]              &= NAS_EMM_IE_TMSI_SPARE;

        /* 填充 TMSI的 flag */
        pucReqMsg[ulIndex]              |= (VOS_UINT8)MMC_LMM_TMSI_STATUS_VALID;
        ulIndex ++;
        *pulIeLength                    = ulIndex;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    return;
}


VOS_VOID  NAS_EMM_EncodeMsClassMark2
(
    VOS_UINT8                          *pMsg,
    VOS_UINT32                         *pulIeLength,
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8      enCnMsgType
)
{
    VOS_UINT8                          *pucReqMsg           = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulIndex             = NAS_EMM_NULL;
    VOS_UINT8                           aucMsClassMark2[NAS_MML_CLASSMARK2_LEN] = {0};
    VOS_UINT32                          ulIsSupportSrvccToGU= NAS_EMM_BIT_NO_SLCT;


    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeMsClassMark2: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    ulIsSupportSrvccToGU = NAS_EMM_GetSuppSrvccToGU();

    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_EncodeMsClassMark2:cn mst type",
                             enCnMsgType);

    NAS_EMM_ATTACH_LOG3_NORM("NAS_EMM_EncodeMsClassMark2:support srvcc or not,attach type,tau type",
                             ulIsSupportSrvccToGU,
                             NAS_EMM_GLO_GetCnAttReqType(),
                             NAS_EMM_TAU_GetTAUtype());

    /* 如果是ATTACH REQ消息，且不是联合注册，也不支持SRVCC TO GU，则不携带CLASSMARK2信元 */
    if ((NAS_EMM_CN_MT_ATTACH_REQ == enCnMsgType)
        && (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI != NAS_EMM_GLO_GetCnAttReqType())
        && (NAS_EMM_BIT_NO_SLCT == ulIsSupportSrvccToGU))
    {
        return ;
    }

    /* 如果是TAU REQ消息，且不是联合TAU，也不支持SRVCC TO GU，则不携带CLAMARK2信元 */
    if ((NAS_EMM_CN_MT_TAU_REQ == enCnMsgType)
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING != NAS_EMM_TAU_GetTAUtype())
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI != NAS_EMM_TAU_GetTAUtype())
        && (NAS_EMM_BIT_NO_SLCT == ulIsSupportSrvccToGU))
    {
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 从GU获取classmark2 */
    NAS_EMM_FillIEClassMark2(aucMsClassMark2);
    /*PC REPLAY MODIFY BY LEILI END*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_EncodeMsClassMark2: MsClassMark2's value is:");
    NAS_COMM_PrintArray(NAS_COMM_GET_MM_PRINT_BUF(),
                        aucMsClassMark2,
                        NAS_MML_CLASSMARK2_LEN);

    if(NAS_MML_CLASSMARK2_LEN > aucMsClassMark2[0])
    {
        /*填充Mobile station classmark 2 IEI*/
        pucReqMsg[ulIndex++]        = NAS_EMM_AD_MS_CLASSMARK2_IEI;

        /* 信元长度 */
        pucReqMsg[ulIndex++]        = aucMsClassMark2[0];

        /* 拷贝除长度之外的剩余部分 */
        NAS_LMM_MEM_CPY(&pucReqMsg[ulIndex], &aucMsClassMark2[1], aucMsClassMark2[0]);
        ulIndex += aucMsClassMark2[0];
    }
    else
    {
        NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_EncodeMsClassMark2: MsClassMark2's length is ERR:", aucMsClassMark2[0]);
    }

    *pulIeLength = ulIndex;

    return;
}



VOS_VOID  NAS_EMM_EncodeMsClassMark3
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength
)
{
    VOS_UINT8            *pucReqMsg;
    VOS_UINT32           ulIndex = NAS_EMM_NULL;
    /*sunbing 49683 2013-10-14 VoLTE begin*/
    VOS_UINT8            aucMsClassMark3[NAS_EMM_AD_LEN_MS_CLASSMARK3+1] = {0};/*增加一个byte的Lenth的保存空间*/
    /*sunbing 49683 2013-10-14 VoLTE end*/


    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeMsClassMark3: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*判断是否支持Mobile station classmark 3*/
    if (NAS_EMM_BIT_SLCT != NAS_EMM_GetSuppSrvccToGU())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeMsClassMark3:Not support SRVCC");
        return;
    }

    /*sunbing 49683 2013-10-14 VoLTE begin*/
    /* 从GU获取classmark3 */
    if(MMC_LMM_UTRAN_MODE_FDD == NAS_LMM_GetEmmInfoUtranMode())
    {
        NAS_MML_Fill_IE_FddClassMark3(aucMsClassMark3);
    }
    else
    {
        NAS_MML_Fill_IE_TddClassMark3(aucMsClassMark3);
    }

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_EncodeMsClassMark3: MsClassMark3's value is:");
    NAS_COMM_PrintArray(NAS_COMM_GET_MM_PRINT_BUF(),
                        aucMsClassMark3,
                        NAS_EMM_AD_LEN_MS_CLASSMARK3+1);

    if(NAS_EMM_AD_LEN_MS_CLASSMARK3 >= aucMsClassMark3[0])
    {
        /*填充Mobile station classmark 3 IEI*/
        pucReqMsg[ulIndex++]        = NAS_EMM_AD_MS_CLASSMARK3_IEI;

        /* 信元长度 */
        pucReqMsg[ulIndex++]        = aucMsClassMark3[0];

        /* 拷贝除长度之外的剩余部分 */
        NAS_LMM_MEM_CPY(&pucReqMsg[ulIndex], &aucMsClassMark3[1], aucMsClassMark3[0]);
        ulIndex += aucMsClassMark3[0];
    }
    else
    {
        NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_EncodeMsClassMark3: MsClassMark3's length is ERR:", aucMsClassMark3[0]);
    }

    *pulIeLength = ulIndex;
    /*sunbing 49683 2013-10-14 VoLTE end*/

    return;
}


VOS_VOID  NAS_EMM_EncodeSupportedCodecs
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_LMM_CODEC_LIST_STRU             *pstCodecList;


    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeSupportedCodecs: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;
    pstCodecList                        = NAS_LMM_GetEmmInfoSupCodecListAddr();

    /*判断是否支持Supported Codecs*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_GetSuppSrvccToGU())
    {
        if (NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_SupCodecList())
        {
            NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeSupportedCodecs:Encode upported Codecs");

            /*填充Supported Codecs IEI*/
            pucReqMsg[ulIndex++]        = NAS_EMM_AD_SUPPORT_CODECS_IEI;

            /* 填充 Supported Codecs的 L */
            pucReqMsg[ulIndex++]        = pstCodecList->ucLenOfCodecList;

            /* 填充 Supported Codecs的 V */
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[0].ucSysId;
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[0].ucLenOfBitmap;
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[0].aucCodecBitmap[0];
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[0].aucCodecBitmap[1];

            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[1].ucSysId;
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[1].ucLenOfBitmap;
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[1].aucCodecBitmap[0];
            pucReqMsg[ulIndex++]        = pstCodecList->astCodec[1].aucCodecBitmap[1];

            *pulIeLength                = ulIndex;

        }
    }

    return;
}



VOS_VOID  NAS_EMM_EncodeAdditionUpdateType
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeAdditionUpdateType: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_EncodeAdditionUpdateType:cs service :",
                             NAS_EMM_GetCsService());

    /*判断是否携带Additional update type*/
    if (NAS_LMM_CS_SERVICE_SMS_ONLY == NAS_EMM_GetCsService())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeAdditionUpdateType:SMS ONLY");

        /*填充Additional update type IEI*/
        pucReqMsg[ulIndex]              = NAS_EMM_IEI_AUT;

        /* 填充 Additional update type的 spare */
        pucReqMsg[ulIndex]              &= NAS_EMM_IE_AUT_SPARE;

        /* 填充 Additional update type的 AUTV */
        pucReqMsg[ulIndex]              |= NAS_EMM_IE_AUTV;
        ulIndex ++;
        *pulIeLength                    = ulIndex;

    }

    return;
}


VOS_VOID  NAS_EMM_EncodeVoiceDomainAndUsageSetting
(
    VOS_UINT8 *pMsg,
    VOS_UINT32 *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          ulRslt;

    if((VOS_NULL_PTR == pMsg) ||
       (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_EncodeVoiceDomainAndUsageSetting: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    NAS_EMM_ATTACH_LOG2_NORM("NAS_EMM_EncodeVoiceDomainAndUsageSetting:\
                            NAS_EMM_GetVoiceDomain=,NAS_EMM_GetCsService()=",
                            NAS_EMM_GetVoiceDomain(),
                            NAS_EMM_GetCsService());

    /*根据协议要求， 对VOIC domain preference and UE's usage setting, 协议有
    如下: The IE shall be included if the UE supports CS fallback and SMS
    over SGs, or if the UE is configured to support IMS voice, 当支持csfb的
    时候，必须是CS+PS的注册方式，所以EPS only时，则不需要携带此IE*/
    /*判断是否携带Voice domain preference and UE's usage setting*/
    if (((NAS_LMM_CS_SERVICE_CSFB_SMS == NAS_EMM_GetCsService()) &&
         (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode())) ||
        (((NAS_LMM_VOICE_DOMAIN_CS_ONLY < NAS_EMM_GetVoiceDomain()) &&
          (NAS_LMM_VOICE_DOMAIN_BUTT > NAS_EMM_GetVoiceDomain())) &&
          (NAS_LMM_CS_SERVICE_1xCSFB != NAS_EMM_GetCsService())))
    {
        pstPubInfo = NAS_LMM_GetEmmInfoAddr();
        if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
        {
            NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeVoiceDomainAndUsageSetting: Encode Voice Domain");

            /*填充Voice domain preference and UE's usage setting IEI*/
            pucReqMsg[ulIndex++]            = NAS_EMM_AD_VOICE_DOMAIN_AND_USAGE_SETTING_IEI;

            /* 填充 Voice domain preference and UE's usage setting的 L */
            pucReqMsg[ulIndex++]            = NAS_EMM_AD_LEN_VOICE_DOMAIN;

            /* 填充 Voice domain preference and UE's usage setting的 V */
            NAS_EMM_SetUeUsageSetting();
            ulRslt = NAS_LMM_GetEmmInfoVoiceDomain();
            pucReqMsg[ulIndex]              = (VOS_UINT8)(NAS_EMM_GLO_AD_GetUsgSetting() << NAS_EMM_AD_MOVEMENT_2_BITS);
            pucReqMsg[ulIndex]              |= (VOS_UINT8)ulRslt;
            ulIndex ++;
            *pulIeLength                    = ulIndex;
        }
    }

    return;
}


NAS_EMM_GUTI_TYPE_ENUM_UINT8 NAS_EMM_MutiModeTinPtmsiEncodeGutiType
(
    VOS_VOID
)
{
    NAS_EMM_GUTI_TYPE_ENUM_UINT8        ucGutiType = NAS_EMM_GUTI_TYPE_BUTT;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinPtmsiEncodeGutiType:TIN IS P-TMSI");
    if (NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinPtmsiEncodeGutiType: P-TMSI and RAI VALID");
        /*映射GUTI有效*/
        ucGutiType = NAS_EMM_GUTI_TYPE_MAPPED;
    }
    return ucGutiType;
}
NAS_EMM_GUTI_TYPE_ENUM_UINT8 NAS_EMM_MutiModeTinGutiOrRatRelatedTmsiEncodeGutiType
(
    VOS_VOID
)
{
    NAS_EMM_GUTI_TYPE_ENUM_UINT8        ucGutiType = NAS_EMM_GUTI_TYPE_BUTT;

    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinGutiOrRatRelatedTmsiEncodeGutiType:GUTI VALID");
        /*GUTI有效*/
        ucGutiType = NAS_EMM_GUTI_TYPE_NATIVE;
    }
    return ucGutiType;
}
NAS_EMM_GUTI_TYPE_ENUM_UINT8 NAS_EMM_MutiModeTinInvaidEncodeGutiType
(
    VOS_VOID
)
{
    NAS_EMM_GUTI_TYPE_ENUM_UINT8        ucGutiType = NAS_EMM_GUTI_TYPE_BUTT;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinInvaidEncodeGutiType:TIN INVALID");
    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinInvaidEncodeGutiType:GUTI VALID");
        /*GUTI有效*/
        ucGutiType = NAS_EMM_GUTI_TYPE_NATIVE;

    }
    else if (NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MutiModeTinInvaidEncodeGutiType:P-TMSI and RAI VALID");
        /*映射GUTI有效*/
        ucGutiType = NAS_EMM_GUTI_TYPE_MAPPED;

    }
    else
    {
        ;
    }
    return ucGutiType;
}


NAS_EMM_GUTI_TYPE_ENUM_UINT8 NAS_EMM_SingleModeEncodeGutiType
(
    VOS_VOID
)
{
    NAS_EMM_GUTI_TYPE_ENUM_UINT8        ucGutiType = NAS_EMM_GUTI_TYPE_BUTT;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_SingleModeEncodeGutiType:NOT SUPPOR MUTI MODE");
    if (NAS_EMM_AD_BIT_SLCT == NAS_EMM_GLO_AD_OP_GUTI())
    {
        /*GUTI有效*/
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_SingleModeEncodeGutiType: GUTI VALID!");

        ucGutiType = NAS_EMM_GUTI_TYPE_NATIVE;
    }
    return ucGutiType;
}
VOS_VOID  NAS_EMM_EncodeGutiType
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    NAS_EMM_GUTI_TYPE_ENUM_UINT8        ucGutiType = NAS_EMM_GUTI_TYPE_BUTT;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeGutiType: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    /*支持多模*/
    if (NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
        {
            ucGutiType = NAS_EMM_MutiModeTinPtmsiEncodeGutiType();
        }
        else if ((MMC_LMM_TIN_GUTI == NAS_EMM_GetTinType()) ||
            (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
        {
            ucGutiType = NAS_EMM_MutiModeTinGutiOrRatRelatedTmsiEncodeGutiType();
        }
        else if (MMC_LMM_TIN_INVALID == NAS_EMM_GetTinType())
        {
            ucGutiType = NAS_EMM_MutiModeTinInvaidEncodeGutiType();

        }
        else
        {

        }
    }
    else
    {
        /*单模*/
        ucGutiType = NAS_EMM_SingleModeEncodeGutiType();

    }

    if (NAS_EMM_GUTI_TYPE_BUTT == ucGutiType)
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EncodeGutiType:not encode guti type");
        return;
    }

    /*GUTI有效携带该IE*/
    /*填充GUTI TYPE IEI*/
    pucReqMsg[ulIndex]              = NAS_EMM_TAU_REQ_GUTI_TYPE_IEI;

    /* 填充 GUTI TYPE的 spare */
    pucReqMsg[ulIndex]              &= NAS_EMM_TAU_REQ_IE_SPARE;

    /* 填充GUTI TYPE的GUTI type */
    pucReqMsg[ulIndex]              |= ucGutiType;

    ulIndex ++;

    *pulIeLength                        = ulIndex;

    return;
}


VOS_VOID    NAS_EMM_CompCnAttachCmp
(
    LRRC_LNAS_MSG_STRU                 *pAttCmp,
    VOS_UINT32                         *pulIndex,
    const EMM_ESM_MSG_STRU             *pstEsmMsg
)
{
    VOS_UINT32                          ulIndex     = *pulIndex;
    VOS_UINT16                          usEsmMsgLen = NAS_EMM_NULL;

    /*清空填充CN消息的空间*/
    /* xiongxianghui00253310 delete memset 2013-11-26 begin */
    /* NAS_LMM_MEM_SET(pAttCmp->aucNasMsg, 0, pAttCmp->ulNasMsgSize); */
    /* xiongxianghui00253310 delete memset 2013-11-26 end   */

    /* 填充 Protocol Discriminator + Security header type */
    pAttCmp->aucNasMsg[ulIndex++]       = EMM_CN_MSG_PD_EMM;

    /* 填充 Attach request message identity   */
    pAttCmp->aucNasMsg[ulIndex++]       = NAS_EMM_CN_MT_ATTACH_CMP;

    /*填充ESM message container*/
    usEsmMsgLen                         = (VOS_UINT16)pstEsmMsg->ulEsmMsgSize;
    pAttCmp->aucNasMsg[ulIndex++]       = (usEsmMsgLen>>8)& 0xFF;
    pAttCmp->aucNasMsg[ulIndex++]       = usEsmMsgLen & 0xFF;

    NAS_LMM_MEM_CPY(                    &(pAttCmp->aucNasMsg[ulIndex]),
                                        pstEsmMsg->aucEsmMsg,
                                        pstEsmMsg->ulEsmMsgSize);

    *pulIndex = ulIndex + pstEsmMsg->ulEsmMsgSize;
}
VOS_VOID    NAS_EMM_MrrcSendAttReq( VOS_VOID )
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pIntraMsg = NAS_EMM_NULL_PTR;
    NAS_MSG_STRU                        *pstNasMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_INFO("Enter NAS_EMM_MrrcSendAttReq,ESM MSG LEN");

    /*申请消息内存*/
    pIntraMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MrrcSendAttReq: MSG ALLOC ERR!");
        return;
    }

    /*构造ATTACH REQUEST消息*/
    NAS_EMM_CompCnAttachReq( pIntraMsg);

    /* 如果是联合ATTACH，则通知MM进入MM LOCATION UPDATING PENDING状态 */
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        NAS_EMM_SendMmCombinedStartNotifyReq(MM_LMM_COMBINED_ATTACH);
    }

    /*向OM发送空口消息ATTACH REQUEST*/
    pstNasMsg = (NAS_MSG_STRU *)(&(pIntraMsg->stNasMsg));

    NAS_LMM_SendOmtAirMsg(               NAS_EMM_OMT_AIR_MSG_UP,
                                        NAS_EMM_ATTACH_REQ,
                                        pstNasMsg);
    NAS_LMM_SendOmtKeyEvent(             EMM_OMT_KE_ATTACH_REQ);


    /*向MRRC发送ATTACH REQUEST消息*/
    NAS_EMM_SndUplinkNasMsg(             pIntraMsg);

    /* 发起ATTACH，将当前TA信息同步到Last尝试发起注册的TA信息中 */
    NAS_EMM_SaveLastAttemptRegTa();

    NAS_LMM_MEM_FREE(pIntraMsg);

    return;

}
#if 0

VOS_VOID NAS_EMM_InitAttachWhenAttempToAttach( VOS_VOID )
{
    /*启动定时器T3410*/
    NAS_LMM_StartStateTimer(     TI_NAS_EMM_T3410);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_CN_ATTACH_CNF*/
    NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                    EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                    TI_NAS_EMM_T3410);
    /*向MRRC发送ATTACH REQUEST消息*/
    NAS_EMM_MrrcSendAttReq();

    return;
}
#endif
VOS_VOID    NAS_EMM_MrrcSendAttCmp
(
    const EMM_ESM_MSG_STRU             *pstEsmMsg
)
{
    NAS_EMM_MRRC_DATA_REQ_STRU         *pIntraMsg;
    VOS_UINT32                          ulMrrcDataReqMsgLenNoHeader;
    NAS_MSG_STRU                       *pstNasMsg;
    VOS_UINT32                          ulIndex             =0;

    NAS_EMM_ATTACH_LOG_INFO("Enter NAS_EMM_MrrcSendAttCmp,ESM MSG LEN");

    /*申请消息内存*/
    pIntraMsg = (VOS_VOID *) NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MrrcSendAttCmp: MSG ALLOC ERR!");
        return;

    }

    /* xiongxianghui00253310 add memset 2013-11-26 begin */
    /*lint -e669*/
    NAS_LMM_MEM_SET(pIntraMsg, 0, NAS_EMM_INTRA_MSG_MAX_SIZE);
    /*lint +e669*/
    /* xiongxianghui00253310 add memset 2013-11-26 end   */

    /*构造ATTACH COMPLETE消息*/
    NAS_EMM_CompCnAttachCmp(            &(pIntraMsg->stNasMsg),&ulIndex,pstEsmMsg);

    ulMrrcDataReqMsgLenNoHeader         = NAS_EMM_CountMrrcDataReqLen(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < ulMrrcDataReqMsgLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_MrrcSendAttCmp, Size error");
        NAS_LMM_MEM_FREE(pIntraMsg);
        return ;
    }

    /*填充消息头*/
    NAS_EMM_COMP_AD_INTRA_MSG_HEADER(pIntraMsg, ulMrrcDataReqMsgLenNoHeader);

    /*填充消息ID*/
    pIntraMsg->ulMsgId                  = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填充消息内容*/
    pIntraMsg->enEstCaue                = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
    pIntraMsg->enCallType               = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    pIntraMsg->enEmmMsgType             = NAS_EMM_MSG_ATTACH_CMP;

    /*填充消息是否需要RRC对传输结果确认*/
    pIntraMsg->enDataCnf                = LRRC_LMM_DATA_CNF_NEED;

    /*填充消息是EMM发送的还是ESM发送的*/
    pIntraMsg->ulEsmMmOpId             = NAS_LMM_OPID;

    /*填充消息长度*/
    pIntraMsg->stNasMsg.ulNasMsgSize    = ulIndex;

    /*向OM发送空口消息ATTACH COMPLETE*/
    pstNasMsg = (NAS_MSG_STRU *)(&(pIntraMsg->stNasMsg));
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_ATTACH_CMP, pstNasMsg);
    NAS_LMM_SendOmtKeyEvent(             EMM_OMT_KE_ATTACH_CMP);

    /*向MRRC发送ATTACH COMPLETE消息*/
    NAS_EMM_SndUplinkNasMsg(             pIntraMsg);

    NAS_LMM_MEM_FREE(pIntraMsg);
    return;

}
VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmPdnRspMsgEsmPdnRsp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{
#if (VOS_OS_VER != VOS_WIN32)
    FTM_TMODE_ENUM						enMspFtmMode = EN_FTM_TMODE_SIGNAL ;
#endif

    EMM_ESM_PDN_CON_RSP_STRU             *pRcvEmmMsg;

    pRcvEmmMsg                          = (EMM_ESM_PDN_CON_RSP_STRU *) pMsgStru;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgEsmPdnRsp is entered!");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_PDN_RSP))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG1_WARN("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgEsmPdnRsp: STATE ERR! ulMsgId:",ulMsgId);
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_EsmPdnRspMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgEsmPdnRsp: EMM_ESM_PDN_CON_RSP_STRU PARA ERR!");
        return NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    /*停止定时器WAIT_ESM_PDN_CNF*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*停止定时器T3411*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*停止定时器T3402*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3402);

    if(EMM_ESM_PDN_CON_RSLT_SUCC == pRcvEmmMsg->ulRst)
    {
        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(             NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                                        &(pRcvEmmMsg->stEsmMsg),
                                        (pRcvEmmMsg->stEsmMsg.ulEsmMsgSize)+4);

#if (VOS_OS_VER != VOS_WIN32)
        /*BT模式下，规避仪器问题，修改启动定时器等待网络响应*/
        if((LPS_OM_GetTmode(&enMspFtmMode)== ERR_MSP_SUCCESS)&&(EN_FTM_TMODE_SIGNAL_NOCARD == enMspFtmMode))
        {
            NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgEsmPdnRsp:T3410 modify 240s!");
            NAS_LMM_ModifyStateTimer(TI_NAS_EMM_T3410, 240000);         /*240s*/
        }
#endif

        /*启动定时器T3410*/
        NAS_LMM_StartStateTimer(     TI_NAS_EMM_T3410);

        /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_CN_ATTACH_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                        TI_NAS_EMM_T3410);

        NAS_EMM_MrrcSendAttReq();

		/* 33401 CR457中描述UE从脱离去注册态转去注册态时，需要将SIM卡或者NV中的安全
		上下文设置为无效*/
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);

        #if 0
        /* 切换MRRC状态为READY状态 */
        NAS_EMM_CHANGE_MRRC_SEND_STATE(     NAS_EMM_MRRC_SEND_STATE_READY);
        #endif
    }
    else
    {
        /*向MMC发送消息*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_ESM_FAILURE);

        /*ATTACH清除资源*/
        NAS_EMM_Attach_ClearResourse();

        /*向ESM发送ID_EMM_ESM_REL_IND*/
        NAS_EMM_EsmSendRelInd();

        /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* 如果处于CONN态,释放连接*/
        if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
        {
            NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        }

    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmBearerCnf(VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{
    EMM_ESM_PDN_CON_SUCC_REQ_STRU      *pRcvEmmMsg = VOS_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU          *pMmNetInfo = VOS_NULL_PTR;
#if (VOS_OS_VER != VOS_WIN32)
    static VOS_UINT32  s_ulNasSndDrvCnt = 1;
#endif

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg                          = (EMM_ESM_PDN_CON_SUCC_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmBearerCnf is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmBearerCnf: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StopStateTimer(                  TI_NAS_EMM_WAIT_ESM_BEARER_CNF);


#if (VOS_OS_VER != VOS_WIN32)
    if (1 == s_ulNasSndDrvCnt)
    {
        (VOS_VOID)ddmPhaseScoreBoot("Nas reg End",__LINE__);
        s_ulNasSndDrvCnt++;
    }
#endif

    /*启动定时器TI_NAS_EMM_WAIT_RRC_DATA_REQ_CNF*/
    NAS_LMM_StartStateTimer(            TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*修改状态：进入主状态REG子状态REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_RRC_DATA_CNF,
                                        TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*向MRRC发送ATTACH COMPLETE*/
    NAS_EMM_MrrcSendAttCmp(             &pRcvEmmMsg->stEsmMsg);

    /*更新LastRegNetId到LASTREG网络ID中 */
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();
    NAS_EMM_SetLVRTai(                  &pMmNetInfo->stPresentNetId);

    /* 判断没有放到函数NAS_LMM_WritePsLoc中来做，是由于紧急注册被拒或者尝试次数
       达到5次时还是要删除参数 ，协议只规定是紧急注册成功后才不写卡或者NV项 */
    if (NAS_LMM_REG_STATUS_EMC_REGING != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmDataReq(VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{
    NAS_EMM_ATTACH_LOG2_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmDataReq is entered",
                                        ulMsgId,
                                        pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmDataReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*check input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgEsmDataReq: NULL PTR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

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

    /*发送DETACH REQUEST消息*/
    NAS_EMM_SendDetachReqMo();

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsNmlSrvMsgEsmEstReq(VOS_UINT32  ulMsgId,
                                           VOS_VOID   *pMsgStru)
{
    EMM_ESM_EST_REQ_STRU                 *pRcvEmmMsg;

    pRcvEmmMsg                          = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsDrgSsNmlSrvMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_NORMAL_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsNmlSrvMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*停止定时器T3411*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*停止定时器T3402*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);

    NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);

    if (VOS_TRUE == pRcvEmmMsg->ulIsEmcType)
    {
        /* 记录ATTACH触发原因值 */
        NAS_EMM_GLO_AD_GetAttCau()      = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);

        /* 缓存紧急类型的ESM消息 */
        NAS_EMM_SaveEmcEsmMsg(          (VOS_VOID*)pRcvEmmMsg);

        /* 发起普通ATTACH流程 */
        NAS_EMM_EsmSendPdnConnInd(      0);
        NAS_LMM_StartStateTimer(        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
        NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    }
    else
    {
        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(                NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                                        &(pRcvEmmMsg->stEsmMsg),
                                        (pRcvEmmMsg->stEsmMsg.ulEsmMsgSize)+4);
        /*保存触发ATTACH的原因*/
        NAS_EMM_GLO_AD_GetAttCau()      = EMM_ATTACH_CAUSE_ESM;

        /*启动定时器T3410*/
        NAS_LMM_StartStateTimer(        TI_NAS_EMM_T3410);

        /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_CN_ATTACH_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                        TI_NAS_EMM_T3410);

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

/*****************************************************************************
 Function Name   : NAS_EMM_MsDrgSsAttemptToAttMsgEsmEstReq
 Description     : 主状态DEREG+子状态DEREG_ATTEMPT_TO_ATTACH下收到ESM的消息
                   PDN CONNECTIVITY REQUEST
 Input           : ulMsgId
                   pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010         2012-10-31  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsDrgSsAttemptToAttMsgEsmEstReq
(
    VOS_UINT32                              ulMsgId,
    VOS_VOID                               *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pRcvEmmMsg = NAS_EMM_NULL_PTR;

    pRcvEmmMsg                          = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsDrgSsAttemptToAttMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTEMPTING_TO_ATTACH))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttemptToAttMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    if (VOS_TRUE != pRcvEmmMsg->ulIsEmcType)
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);
        return  NAS_LMM_MSG_HANDLED;
    }

    /*停止定时器T3411*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*停止定时器T3402*/
    NAS_LMM_StopPtlTimer(           TI_NAS_EMM_PTL_T3402);

    NAS_MML_SetPsAttachAllowFlg(    NAS_EMM_AUTO_ATTACH_ALLOW);

    /* 记录ATTACH触发原因值 */
    NAS_EMM_GLO_AD_GetAttCau()      = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;
    NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);

    /* 缓存紧急类型的ESM消息 */
    NAS_EMM_SaveEmcEsmMsg(          pMsgStru);

    /* 发起普通ATTACH流程 */
    NAS_EMM_EsmSendPdnConnInd(      0);
    NAS_LMM_StartStateTimer(        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                    EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                    TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_StartEmergencyAttach( VOS_VOID )
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_StartEmergencyAttach is entered");

    /*停止定时器T3411*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*停止定时器T3402*/
    NAS_LMM_StopPtlTimer(   TI_NAS_EMM_PTL_T3402);

    /*保存触发ATTACH的原因*/
    NAS_EMM_GLO_AD_GetAttCau()    = EMM_ATTACH_CAUSE_ESM_EMC_ATTACH;
    NAS_MML_SetPsAttachAllowFlg(    NAS_EMM_AUTO_ATTACH_ALLOW);
    NAS_LMM_SetEmmInfoRegStatus(    NAS_LMM_REG_STATUS_EMC_REGING);
    NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);

    /*启动定时器T3410*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3410);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_CN_ATTACH_CNF*/
    NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                    EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                    TI_NAS_EMM_T3410);

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
VOS_UINT32  NAS_EMM_MsDrgSsLimitedSrvMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsDrgSsLimitedSrvMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_LIMITED_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsLimitedSrvMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型则直接回复失败 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);
        return  NAS_LMM_MSG_HANDLED;
    }

    /*保存ESM消息*/
    NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                           &(pstEsmEstReq->stEsmMsg),
                            (pstEsmEstReq->stEsmMsg.ulEsmMsgSize)+4);

    NAS_EMM_StartEmergencyAttach();

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsDrgSsNoImsiMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsDrgSsNoImsiMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_NO_IMSI))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsNoImsiMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型则直接丢弃 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);
        return  NAS_LMM_MSG_HANDLED;
    }

    /*保存ESM消息*/
    NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                           &(pstEsmEstReq->stEsmMsg),
                            (pstEsmEstReq->stEsmMsg.ulEsmMsgSize)+4);

    NAS_EMM_StartEmergencyAttach();

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsDrgSsAttachNeededMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsDrgSsAttachNeededMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTACH_NEEDED))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttachNeededMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型则直接丢弃 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);
        return  NAS_LMM_MSG_HANDLED;
    }

    /*保存ESM消息*/
    NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                           &(pstEsmEstReq->stEsmMsg),
                            (pstEsmEstReq->stEsmMsg.ulEsmMsgSize)+4);

    NAS_EMM_StartEmergencyAttach();

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgEsmDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_DATA_REQ_STRU                 *pstEsmDataReq = NAS_EMM_NULL_PTR;

    pstEsmDataReq                        = (EMM_ESM_DATA_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsRegSsLimitedSrvMsgEsmDataReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG, EMM_SS_REG_LIMITED_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsLimitedSrvMsgEsmDataReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型则直接丢弃 */
    if (VOS_TRUE != pstEsmDataReq->ulIsEmcType)
    {
		/* lihong00150010 emergency tau&service begin */
        if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState()) ||
            (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState()))
        {
            NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);
            return NAS_LMM_MSG_HANDLED;
        }
		/* lihong00150010 emergency tau&service end */
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsLimitedSrvMsgEsmDataReq: not emergency!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*//* ESM不能清除紧急动态记录 */
    NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    /*保存ESM消息*/
    NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                           &(pstEsmDataReq->stEsmMsg),
                            (pstEsmDataReq->stEsmMsg.ulEsmMsgSize)+4);

    NAS_EMM_StartEmergencyAttach();

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_MsRegInitMsgEsmEstReq is entered",
                                        ulMsgId);

    /*检查状态是否匹配，若不匹配，退出*/
    if (EMM_MS_REG_INIT != NAS_EMM_AD_CUR_MAIN_STAT)
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型则直接丢弃 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitMsgEsmEstReq: not emergency!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 打断正在进行的ATTACH流程 */
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_T3410);
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    #if 0
    /*清除MRRC的管理数据*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    #endif

    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;
    NAS_EMM_AppSendAttFailWithPara();
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt() >= 5)
    {
        /*设置update status为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*删除GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);
    }

    NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

    /* 如果处于IDLE态,直接发起紧急注册*/
    if (NAS_EMM_CONN_IDLE               == NAS_EMM_GetConnState())
    {
        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(                NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                                       &(pstEsmEstReq->stEsmMsg),
                                        (pstEsmEstReq->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

    /* 记录ATTACH触发原因值 */
    NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

    NAS_LMM_SetEmmInfoIsEmerPndEsting(  VOS_TRUE);

    /* 缓存紧急类型的ESM消息 */
    NAS_EMM_SaveEmcEsmMsg(              (VOS_VOID*)pstEsmEstReq);

    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsPlmnSrcMsgMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsPlmnSrcMsgMsgEsmEstReq is entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG, EMM_SS_DEREG_PLMN_SEARCH))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsPlmnSrcMsgMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
    NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsDrgSsNoCellAvailMsgMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsNoCellAvailMsgMsgEsmEstReq is entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG, EMM_SS_DEREG_NO_CELL_AVAILABLE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsNoCellAvailMsgMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
    NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsNormalMsgMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegSsNormalMsgMsgEsmEstReq is entered");
    (VOS_VOID)ulMsgId;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsNormalMsgMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型，则不可能收到，如果收到，直接丢弃 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsNormalMsgMsgEsmEstReq: not emergency!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
    NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmEstReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_EST_REQ_STRU                 *pstEsmEstReq = NAS_EMM_NULL_PTR;

    pstEsmEstReq                        = (EMM_ESM_EST_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmEstReq is entered");
    (VOS_VOID)ulMsgId;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmEstReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型，则不可能收到，如果收到，直接丢弃 */
    if (VOS_TRUE != pstEsmEstReq->ulIsEmcType)
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmEstReq: not emergency!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
    NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsAnyStateSsAnySateProcMsgEsmStatusReq( VOS_VOID )
{
    VOS_UINT32                                    ulCurEmmMsStat;
    VOS_UINT32                                    ulCurEmmSsStat;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM(                      "NAS_EMM_MsAnyStateSsAnySateProcMsgEsmStatusReq:EpsNumId=0");

    ulCurEmmMsStat                                =   NAS_EMM_CUR_MAIN_STAT;
    ulCurEmmSsStat                                =   NAS_EMM_CUR_SUB_STAT;
    if (NAS_RELEASE_CTRL)
    {
        if ((EMM_MS_SER_INIT == ulCurEmmMsStat)
            && (EMM_SS_SER_WAIT_CN_SER_CNF == ulCurEmmSsStat)
            && (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure()))
        {
            NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsAnyStateSsAnySateProcMsgEsmStatusReq:ESR procedure");

            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

            NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                    EMM_SS_DEREG_PLMN_SEARCH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

            /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
            NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

            /* 本地DETACH释放资源:动态内存、赋初值 */
            NAS_LMM_DeregReleaseResource();

            /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
            NAS_EMM_RelReq(                           NAS_LMM_NOT_BARRED);

            return NAS_LMM_MSG_HANDLED;

        }
    }
    if((EMM_MS_NULL                               !=   ulCurEmmMsStat)&&
       (EMM_MS_DEREG                              !=   ulCurEmmMsStat))
    {
        NAS_EMM_ATTACH_LOG_NORM(                   "NAS_EMM_MsAnyStateSsAnySateProcMsgEsmStatusReq:Local Detach");

        /*本地Detach的清资源操作在DETACH 模块收到 REL IND后
        执行，此处只先停止定时器*/
        #if 0
        /* 停止所有EMM状态定时器 */
        NAS_LMM_StopAllEmmStateTimer();

        /* 停止所有EMM协议定时器 */
        NAS_LMM_StopAllEmmPtlTimer();

        /*保存APP DETACH类型消息*/
        NAS_EMM_GLO_AD_GetDetTypeMo()             =   MMC_LMM_MO_DET_PS_ONLY;
        #endif

        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
        /* lihong00150010 emergency tau&service begin */
        if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
        {
            /*状态转换，通知MMC卡无效*/
            NAS_EMM_ProcLocalNoUsim();

            /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
            NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);
        }/* lihong00150010 emergency tau&service end */
        else
        {
            NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                    EMM_SS_DEREG_PLMN_SEARCH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

            /* 本地DETACH释放资源:动态内存、赋初值 */
            NAS_LMM_DeregReleaseResource();

            /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
            NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

        }

        /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
        NAS_EMM_RelReq(                           NAS_LMM_NOT_BARRED);

    }

    return(NAS_LMM_MSG_HANDLED);
}
/*leili modify for isr begin*/

NAS_EMM_BEARER_STATE_ENUM_UINT8  NAS_EMM_ProcEsmBearState
(
    VOS_UINT32      ulEpsId,
    VOS_VOID        *pstMsg
)
{
    EMM_ESM_BEARER_STATUS_REQ_STRU     *pstEsmBearerStatusReq   = VOS_NULL_PTR;
    VOS_UINT32              i = 0;

    pstEsmBearerStatusReq               =(EMM_ESM_BEARER_STATUS_REQ_STRU *)(pstMsg);

    for (i = 0; i < pstEsmBearerStatusReq->ulEpsIdNum; i++)
    {
        if (ulEpsId == pstEsmBearerStatusReq->aulEsmEpsId[i])
        {
            return NAS_EMM_BEARER_STATE_ACTIVE;
        }
    }
    return NAS_EMM_BEARER_STATE_INACTIVE;
}


NAS_EMM_BEARER_STATE_ENUM_UINT8  NAS_EMM_IsEpsBearStatusAct (VOS_VOID)
{
    NAS_MML_PS_BEARER_CONTEXT_STRU      *pstEpsBearerCxt;
    VOS_UINT32              i = 0;

    pstEpsBearerCxt = NAS_MML_GetPsBearerCtx();

    for(i = 0; i < EMM_ESM_MAX_EPS_BEARER_NUM; i++)
    {
        if (NAS_MML_PS_BEARER_STATE_ACTIVE == pstEpsBearerCxt[i].enPsBearerState)
        {
            return NAS_EMM_BEARER_STATE_ACTIVE;
        }
    }
    return NAS_EMM_BEARER_STATE_INACTIVE;
}


VOS_VOID  NAS_EMM_UpdateEpsBearStatus(VOS_VOID *pstMsg)
{
    NAS_MML_PS_BEARER_CONTEXT_STRU      astPsBearerCtx[EMM_ESM_MAX_EPS_BEARER_NUM] = {0};
    EMM_ESM_BEARER_STATUS_REQ_STRU     *pstEsmBearerStatusReq   = VOS_NULL_PTR;
    VOS_UINT32                          ulEpsId;
    NAS_EMM_BEARER_STATE_ENUM_UINT8     ucEsmBearerState;
    VOS_UINT32                          i = NAS_EMM_NULL;
    VOS_UINT32                          j = NAS_EMM_NULL;
    VOS_UINT16                          usBearerSate;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateEpsBearStatus: GET MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);

    pstEsmBearerStatusReq =(EMM_ESM_BEARER_STATUS_REQ_STRU *)(pstMsg);

    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*ISR不激活，根据ESM发送的承载信息更新MML全局变量*/
    if(MMC_LMM_TIN_RAT_RELATED_TMSI != NAS_EMM_GetTinType())
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateEpsBearStatus: ISR没有激活");
        for(i = 0; i < pstEsmBearerStatusReq->ulEpsIdNum; i++)
        {
            j = pstEsmBearerStatusReq->aulEsmEpsId[i] - NAS_EMM_MIN_EPS_ID;

            astPsBearerCtx[j].enPsBearerState = NAS_MML_PS_BEARER_STATE_ACTIVE;
        }
        NAS_LMM_MEM_CPY(NAS_EMM_GetPsBearerCtx(), astPsBearerCtx, sizeof(astPsBearerCtx));

        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateEpsBearStatus: UPDATE MML PS BEARER INFO:");
        NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                         *EMM_ESM_MAX_EPS_BEARER_NUM);
        return;
    }

    /*ISR激活，根据ESM承载信息更新MML全局变量*/
    NAS_LMM_MEM_CPY(astPsBearerCtx, NAS_EMM_GetPsBearerCtx(), sizeof(astPsBearerCtx));

    for (i = 0; i < EMM_ESM_MAX_EPS_BEARER_NUM; i++)
    {
        ulEpsId = i + NAS_EMM_MIN_EPS_ID;
        ucEsmBearerState = NAS_EMM_ProcEsmBearState(ulEpsId,pstEsmBearerStatusReq);

        usBearerSate = NAS_LMM_PUB_COMP_BEARERSTATE(astPsBearerCtx[i].enPsBearerState,ucEsmBearerState);
        switch(usBearerSate)
        {
            case NAS_LMM_PUB_COMP_BEARERSTATE(NAS_MML_PS_BEARER_STATE_ACTIVE,NAS_EMM_BEARER_STATE_INACTIVE):

                /*MML中承载状态激活，ESM中承载状态不激活，根据当前ISR标识判断是否去激活ISR*/
                astPsBearerCtx[i].enPsBearerState = NAS_MML_PS_BEARER_STATE_INACTIVE;
                if (NAS_MML_PS_BEARER_EXIST_BEFORE_ISR_ACT == astPsBearerCtx[i].enPsBearerIsrFlg)
                {
                    /*更新TIN值为GUTI*/
                    NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);

                    /*更新所有承载的ISR标识为NONE*/
                    NAS_EMM_UpdateBearISRFlag(astPsBearerCtx);

                }
                astPsBearerCtx[i].enPsBearerIsrFlg = NAS_MML_PS_BEARER_EXIST_BEFORE_ISR_ACT;
                break;
            case NAS_LMM_PUB_COMP_BEARERSTATE(NAS_MML_PS_BEARER_STATE_INACTIVE,NAS_EMM_BEARER_STATE_ACTIVE):

                /*MML中承载状态不激活，ESM中承载状态激活，更新MML中承载状态和ISR标识*/
                astPsBearerCtx[i].enPsBearerState = NAS_MML_PS_BEARER_STATE_ACTIVE;
                astPsBearerCtx[i].enPsBearerIsrFlg = NAS_MML_PS_BEARER_EXIST_AFTER_ISR_ACT;
                break;

            case NAS_LMM_PUB_COMP_BEARERSTATE(NAS_MML_PS_BEARER_STATE_ACTIVE,NAS_EMM_BEARER_STATE_ACTIVE):
            case NAS_LMM_PUB_COMP_BEARERSTATE(NAS_MML_PS_BEARER_STATE_INACTIVE,NAS_EMM_BEARER_STATE_INACTIVE):

                NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateEpsBearStatus: MML BEARER INFO NOT CHANGE:");
                break;
            default:
                break;
        }
    }

    NAS_LMM_MEM_CPY(NAS_EMM_GetPsBearerCtx(), astPsBearerCtx, sizeof(astPsBearerCtx));

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateEpsBearStatus: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                         sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                         *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI END*/
    return;
}

/*leili modify for isr end*/

/* lihong00150010 ims begin */
/*****************************************************************************
 Function Name   : NAS_EMM_SendMmcEmcBearerStatusNotify
 Description     : 向MMC发送LMM_MMC_EMC_BEARER_STATUS_NOTIFY消息
 Input           : ucIsEmcPdpActive----------------EMC PDP是否激活标识
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010    2013-12-27  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_SendMmcEmcBearerStatusNotify
(
    VOS_UINT8                           ucIsEmcPdpActive
)
{
    LMM_MMC_EMC_PDP_STATUS_NOTIFY_STRU *pstEmcPdpStatusNotify = VOS_NULL_PTR;

    /* 申请DOPRA消息 */
    pstEmcPdpStatusNotify = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_EMC_PDP_STATUS_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstEmcPdpStatusNotify)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendMmcEmcBearerStatusNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstEmcPdpStatusNotify,0,sizeof(LMM_MMC_EMC_PDP_STATUS_NOTIFY_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstEmcPdpStatusNotify),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_EMC_PDP_STATUS_NOTIFY_STRU));

    /* 填充消息ID */
    pstEmcPdpStatusNotify->ulMsgId          = ID_LMM_MMC_EMC_PDP_STATUS_NOTIFY;
    pstEmcPdpStatusNotify->ucIsEmcPdpActive = ucIsEmcPdpActive;

    /* 发送DOPRA消息 */
    NAS_LMM_SendLmmMmcMsg(pstEmcPdpStatusNotify);

    return;
}
/* lihong00150010 ims end */

/*****************************************************************************
 Function Name   : NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq
 Description     : 收到ESM BEARER STATUS REQ消息时更新注册状态
 Input           : Pointer of EMM_ESM_BEARER_STATUS_REQ_STRU
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010    2012-10-30  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq
(
    const EMM_ESM_BEARER_STATUS_REQ_STRU *pstEsmBearerStatusReq
)
{
    NAS_MML_PS_BEARER_CONTEXT_STRU      astPsBearerCtx[EMM_ESM_MAX_EPS_BEARER_NUM] = {0};
    VOS_UINT32                          ulCurEmmStat = NAS_EMM_NULL;

    if (0 == pstEsmBearerStatusReq->ulEpsIdNum)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:DEREG");
        NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_DEREG);
        return ;
    }

    /* 注册过程中，则不更新状态，待ATTACH COMPLETE发送成功
       后再更新，如果这里就更新，则无法在REG_INIT+WAIT_RRC_DATA_CNF状态下使用
       NAS_LMM_REG_STATUS_EMC_REGING标识来区分紧急注册还是普通注册，ATTACH触发
       原因值在紧急承载激活成功清动态表时已通知EMM清除 */
    if (EMM_MS_REG_INIT == NAS_EMM_AD_CUR_MAIN_STAT)
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:attaching.");
        return;
    }

    /* 如果有承载，且没有紧急承载，则更新状态为正常注册 */
    if (VOS_FALSE == pstEsmBearerStatusReq->ulExistEmcPdnFlag)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:NORM REGED");

        /* 如果状态从正常注册且有紧急承载变为正常注册，则给MMC发送
           LMM_MMC_EMC_BEARER_STATUS_NOTIFY消息，指示紧急承载释放 */
        if (NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_SendMmcEmcBearerStatusNotify(VOS_FALSE);
        }

        NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED);
        return ;
    }
     /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 如果有紧急承载，且承载数为1，则更新状态为紧急注册 */
    if (1 == pstEsmBearerStatusReq->ulEpsIdNum)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:EMC REGED");

        /* lihong00150010 emergency tau&service begin */
        ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(   NAS_EMM_CUR_MAIN_STAT,
                                                    NAS_EMM_CUR_SUB_STAT);

        if ((ulCurEmmStat == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE))
            || (ulCurEmmStat == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
        {
            NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3411);
            NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3402);

            /* TAU完成后，网侧通过ESM承载信息IE去激活所有非紧急承载 */
            NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }

        NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_EMC_REGED);

        if ((MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType())
            && (NAS_EMM_NOT_SUSPEND == NAS_EMM_IsSuspended()))
        {
            /* 去激活ISR */
            NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
        }
        /* lihong00150010 emergency tau&service end */

        NAS_LMM_MEM_CPY(astPsBearerCtx, NAS_EMM_GetPsBearerCtx(), sizeof(astPsBearerCtx));

        /*更新所有承载的ISR标识为NONE*/
        NAS_EMM_UpdateBearISRFlag(astPsBearerCtx);

        NAS_LMM_MEM_CPY(NAS_EMM_GetPsBearerCtx(), astPsBearerCtx, sizeof(astPsBearerCtx));

        return ;
    }
     /*PC REPLAY MODIFY BY LEILI END*/
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:NORM REGED AND EMC BEARER");
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq:state:",NAS_LMM_GetEmmInfoRegStatus());

    /* 如果有紧急承载，且承载数大于1，则更新状态为正常注册且有紧急承载 */
    /* 如果状态从正常注册变为正常注册且有紧急承载，则给MMC发送
       LMM_MMC_EMC_BEARER_STATUS_NOTIFY消息，指示紧急承载建立 */
    if (NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
    {
        NAS_EMM_SendMmcEmcBearerStatusNotify(VOS_TRUE);
    }

    NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR);

    return ;
}


VOS_UINT32  NAS_EMM_MsAnyStateSsAnySateEsmBearStatusReq( VOS_VOID *pstMsg )
{
    VOS_UINT32                          ulRes = NAS_LMM_MSG_HANDLED;
    EMM_ESM_BEARER_STATUS_REQ_STRU     *pstEsmBearerStatusReq   = VOS_NULL_PTR;


    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsAnyStateSsAnySateEsmBearStatusReq is enter.");

    /* 参数检查*/
    if (NAS_EMM_NULL_PTR == pstMsg)
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsAnyStateSsAnySateEsmBearStatusReq: NULL PTR.");
        return  NAS_LMM_FAIL;
    }
    /*leili modify for isr begin*/
    #if 0
    /* 更新EMM本地维护的承载上下文*/
    pstEsmBearerStatusReq               =(EMM_ESM_BEARER_STATUS_REQ_STRU *)(pstMsg);
    pstEmmBearerCntxt                   = NAS_EMM_GetEpsContextStatusAddr();

    pstEmmBearerCntxt->ulEpsIdNum       = pstEsmBearerStatusReq->ulEpsIdNum;
    NAS_LMM_MEM_CPY(                     pstEmmBearerCntxt->aulEsmEpsId,
                                        pstEsmBearerStatusReq->aulEsmEpsId,
                                        sizeof(VOS_UINT32)*EMM_ESM_MAX_EPS_BEARER_NUM);
    #endif

    pstEsmBearerStatusReq               =(EMM_ESM_BEARER_STATUS_REQ_STRU *)(pstMsg);



    /* 若是挂起期间收到的，更新本地全局变量后则退出 */
    if((NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsAnyStateSsAnySateEsmBearStatusReq: LTE CUR SUSPEND");
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);
        return  NAS_LMM_MSG_HANDLED;
    }

    /* 更新EMM本地维护的承载上下文*/
    NAS_EMM_UpdateEpsBearStatus((VOS_VOID*)pstEsmBearerStatusReq);
    /*leili modify for isr end*/

    /* 更新注册状态 */
    NAS_EMM_UpdateRegStatusWhenEsmBearStatusReq(pstEsmBearerStatusReq);

    /* 如果激活承载数为0，本地DETACH，再重新ATTACH,流程结束*/
    if (0 == pstEsmBearerStatusReq->ulEpsIdNum)
    {
        /* 判读状态是否需要出栈*/
        NAS_LMM_IfEmmHasBeenPushedThenPop();
        ulRes = NAS_EMM_MsAnyStateSsAnySateProcMsgEsmStatusReq();
        return ulRes;
    }

    /* 激活承载数不为零，判断本次承载变化如果是ESM本地执行，
       则消息进入状态机继续处理，
       否则流程结束*/
    if (EMM_ESM_BEARER_CNTXT_MOD_LOCAL == pstEsmBearerStatusReq->enBearerCntxtMod)
    {
        return NAS_LMM_MSG_DISCARD;
    }
    else
    {
        return NAS_LMM_MSG_HANDLED;
    }
}


VOS_UINT32 NAS_EMM_SndAttachReqFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_LMM_MAIN_STATE_ENUM_UINT16       enMainState;

    enMainState                         = NAS_LMM_GetEmmCurFsmMS();

    *pulIsDelBuff = VOS_TRUE;

    /*检查状态是否匹配，若不匹配，退出*/
    if (EMM_MS_REG_INIT != enMainState)
    {
        /*打印出错信息*/
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SndAttachReqFailProc: STATE ERR!");
        return  NAS_EMM_SUCC;
    }

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_HO:
        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:
            /* 重启ATTACH流程 */
            /*停止定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);

            if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
            {
                NAS_EMM_StartEmergencyAttach();

                return  NAS_EMM_SUCC;
            }

            if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            {
                /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
                NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
            }
            else
            {
                /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
                NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
            }

            /* 状态迁移Dereg.Normal_Service，发送INTRA_ATTACH_REQ消息*/
            NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                    EMM_SS_DEREG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

            (VOS_VOID)NAS_EMM_SendIntraAttachReq();

            break;

        default:
            break;
        }

    return NAS_EMM_SUCC;
}
VOS_UINT32 NAS_EMM_SndEsmMsgFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataMsg = VOS_NULL_PTR;
    NAS_LMM_MAIN_STATE_ENUM_UINT16       enMainState;
    NAS_LMM_SUB_STATE_ENUM_UINT16        enSubState;
    VOS_UINT32                            ulRrcMmDataReqMsgLen;
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;

    *pulIsDelBuff = VOS_TRUE;

    /* 从当前状态机中获取状态 */
    enMainState                         = NAS_LMM_GetEmmCurFsmMS();
    enSubState                          = NAS_LMM_GetEmmCurFsmSS();

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    pEmmMrrcMgmtData = NAS_EMM_FindMsgInDataReqBuffer(pstRrcMmDataCnf->ulOpId);

    if (NAS_EMM_NULL_PTR == pEmmMrrcMgmtData)
    {
        return NAS_EMM_FAIL;
    }
    ulRrcMmDataReqMsgLen = (sizeof(NAS_EMM_MRRC_DATA_REQ_STRU)+
                           pEmmMrrcMgmtData->ulNasMsgLength) -
                           NAS_EMM_4BYTES_LEN;
    /* 申请消息内存*/
    pMrrcDataMsg = (NAS_EMM_MRRC_DATA_REQ_STRU *)(VOS_VOID*)NAS_LMM_MEM_ALLOC(ulRrcMmDataReqMsgLen);

    if(VOS_NULL_PTR == pMrrcDataMsg)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SndMtDetachAccFailProc: Mem Alloc Fail");
        return NAS_EMM_FAIL;
    }
    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:
            if ((EMM_MS_REG_INIT == enMainState)
                || (EMM_MS_TAU_INIT == enMainState)
                || ((EMM_MS_REG == enMainState)
                    && ((EMM_SS_REG_NORMAL_SERVICE == enSubState)
                        || (EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM == enSubState)
                        || (EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF == enSubState))))
            {
                pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

                pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_ESM;
                pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
                pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

                pMrrcDataMsg->stNasMsg.ulNasMsgSize = pEmmMrrcMgmtData->ulNasMsgLength;

                NAS_LMM_MEM_CPY(pMrrcDataMsg->stNasMsg.aucNasMsg,
                                 pEmmMrrcMgmtData->aucNasMsgContent,
                                 pEmmMrrcMgmtData->ulNasMsgLength);

                NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);
            }
            break;

        default:
            break;
    }

    /*释放所申请内存 */
    NAS_LMM_MEM_FREE(pMrrcDataMsg);
    return NAS_EMM_SUCC;
}
/*lint +e961*/
/*lint +e960*/

VOS_VOID NAS_EMM_RestartAttProcedural(VOS_VOID)
{
    /* 重启ATTACH流程 */
    /* 停止定时器TI_NAS_EMM_T3410 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_StartEmergencyAttach();

        return;
    }

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /* 状态迁移Dereg.Normal_Service，发送INTRA_ATTACH_REQ消息*/
    NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                            EMM_SS_DEREG_NORMAL_SERVICE,
                            TI_NAS_EMM_STATE_NO_TIMER);

    /*发送ATTACH REQ内部消息给ATTACH模块*/
    (VOS_VOID)NAS_EMM_SendIntraAttachReq();

    return ;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

