

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include  "NasLmmPubMPrint.h"
#include "NasMmlCtx.h"
/* lihong00150010 emergency tau&service begin */
#include "NasEmmAttDetInclude.h"
/* lihong00150010 emergency tau&service end */


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMTAUPROC_C
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
EMM_TAU_CTRL_STRU                g_stEmmTAUCtrlTbl;




/*lint -e589*/
/*lint -e669*/
/*lint -e831*/
/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_EMM_TAU_Init(VOS_VOID)
{
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_Init                   START INIT...");

    /* 初始化TAU全局变量 */
    NAS_LMM_MEM_SET(&g_stEmmTAUCtrlTbl, 0, sizeof(g_stEmmTAUCtrlTbl));

    g_stEmmTAUCtrlTbl.enEmmTauType = NAS_EMM_CN_TAU_TYPE_TA_UPDATING;

    /* 初始化BAR信息*/
    NAS_EMM_ClearBarResouce();

    return;
}
VOS_VOID  NAS_EMM_TAU_ClearResouce( VOS_VOID )
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_ClearResouce entered.");

    /* 初始化TAU全局变量 */
    NAS_LMM_MEM_SET(&g_stEmmTAUCtrlTbl, 0, sizeof(g_stEmmTAUCtrlTbl));

    g_stEmmTAUCtrlTbl.enEmmTauType = NAS_EMM_CN_TAU_TYPE_TA_UPDATING;

    /* 释放缓存的ESM消息*/
    NAS_EMM_SerClearEsmDataBuf();

    /* 清空接入Bar信息*/
    NAS_EMM_ClearBarResouce();

    /*停止相关定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    return;
}
/*****************************************************************************
 Function Name   : NAS_EMM_TAU_SuspendInitClearResouce
 Description     : UE挂起清理TAU流程所有资源
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY       2012-02-24  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_TAU_SuspendInitClearResouce( VOS_VOID )
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_ClearResouce entered.");

    /* 初始化TAU全局变量 */
    NAS_LMM_MEM_SET(&g_stEmmTAUCtrlTbl, 0, sizeof(g_stEmmTAUCtrlTbl));

    g_stEmmTAUCtrlTbl.enEmmTauType = MMC_LMM_TA_UPDATING;

    /* 释放缓存的ESM消息*/
    NAS_EMM_SerClearEsmDataBuf();

    /* 清空接入Bar信息*/
    NAS_EMM_ClearBarResouce();

    /*停止相关定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    return;
}

VOS_VOID  NAS_EMM_FreeTauDyn( VOS_VOID )
{
    /*需要时添加操作*/

    return;
}
VOS_VOID    NAS_EMM_TAUSER_FSMTranState(
                                    NAS_EMM_MAIN_STATE_ENUM_UINT16  stMS,
                                    NAS_EMM_SUB_STATE_ENUM_UINT16   stSS,
                                    NAS_EMM_STATE_TI_ENUM_UINT16    stTI

)
{

    NAS_LMM_FSM_STATE_STRU              EmmStat;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAUSER_FSMTranState is entered.");

    EmmStat.enFsmId                   = NAS_LMM_PARALLEL_FSM_EMM;
    EmmStat.enMainState               = stMS;
    EmmStat.enSubState                = stSS;
    EmmStat.enStaTId                  = stTI;
    NAS_LMM_StaTransProc(EmmStat);

    return;
}
VOS_VOID    NAS_EMM_TauUpdateGutiFromCnMsg(
                                const NAS_EMM_CN_TAU_ACP_STRU     *pstTAUAcp)
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TauUpdateGutiFromCnMsg is entered");

    if(NAS_EMM_BIT_SLCT == pstTAUAcp->ucBitOpGuti)
    {
        NAS_LMM_GetEmmInfoUeidAddr()->bitOpGuti = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_LMM_GetEmmInfoUeidGutiAddr() ,
                                        &(pstTAUAcp->stGuti),
                                        sizeof(NAS_EMM_GUTI_STRU));

    }

    return;
}


VOS_VOID NAS_EMM_TAUSER_TaiListSet( const NAS_EMM_TA_LIST_STRU *pstTALIST)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAUSER_TaiListSet is entered.");

    /* 更新TAILIST的全局变量 */
    NAS_LMM_MEM_CPY(                            &(g_stEmmInfo.stNetInfo.stTaiList),
                                                pstTALIST,
                                                sizeof(NAS_EMM_TA_LIST_STRU));
    g_stEmmInfo.stNetInfo.bitOpTaiList =        NAS_EMM_BIT_SLCT;

    return;
}


NasMmIntraMsgBlock *    NAS_EMM_TAUSER_GetSendIntraMsgBuffAddr(VOS_UINT32 ulBuffSize)
{
    NasMmIntraMsgBlock *                    pIntraMsg = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAUSER_GetSendIntraMsgBuffAddr is entered.");

    pIntraMsg = NAS_LMM_GetSendIntraMsgBuffAddr(ulBuffSize );

    return pIntraMsg;
}
VOS_VOID* NAS_EMM_TauRsltCommProc( VOS_VOID )
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;

    /* 申请MMC内部消息 */
    pstRsltInd  = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_TAU_RESULT_IND_STRU));

    if(NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TauRsltCommProc: MSG ALLOC ERR !!");
        return NAS_EMM_NULL_PTR;
    }

    NAS_LMM_MEM_SET(pstRsltInd,0,sizeof(LMM_MMC_TAU_RESULT_IND_STRU));

    /* 填充给MMC的DOPRA头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER(       pstRsltInd,
                                        sizeof(LMM_MMC_TAU_RESULT_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /*填充OPID*/


    /* 填充消息ID */
    pstRsltInd->ulMsgId                 = ID_LMM_MMC_TAU_RESULT_IND;

    return pstRsltInd;
}

VOS_VOID NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATTACH_RSLT_ENUM_UINT32 ulAttRst)
{
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        if( (MMC_LMM_ATT_REQ_TYPE_CS_ONLY == NAS_EMM_GLO_GetAttReqType())
          ||(MMC_LMM_ATT_REQ_TYPE_CS_PS == NAS_EMM_GLO_GetAttReqType()))
        {
           NAS_EMM_AppSendAttRstInTau(ulAttRst);
        }
    }
    return;
}

VOS_VOID NAS_EMM_TauRstSuccLeadSndAttachRst( VOS_VOID)
{
    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        if( (MMC_LMM_ATT_REQ_TYPE_CS_ONLY == NAS_EMM_GLO_GetAttReqType())
          ||(MMC_LMM_ATT_REQ_TYPE_CS_PS == NAS_EMM_GLO_GetAttReqType()))
        {
            /* CS_PS的TAU成功,返回结果为成功*/
            if( (NAS_EMM_EPS_UPDATE_RST_COMBINED_TA_LA_UPDATED == NAS_EMM_TAU_GetEpsUpdateRslt())
              ||(NAS_EMM_EPS_UPDATE_RST_COMBINED_TA_LA_UPDATED_AND_ISR == NAS_EMM_TAU_GetEpsUpdateRslt()))
            {
                NAS_EMM_AppSendAttRstInTau(MMC_LMM_ATT_RSLT_SUCCESS);
            }
            /* 只有PS成功，结果为失败 */
            else
            {
                NAS_EMM_AppSendAttRstInTau(MMC_LMM_ATT_RSLT_FAILURE);
            }
        }
    }
    return;
}
MMC_LMM_TAU_TYPE_ENUM_UINT32  NAS_EMM_MmcTauReqTypeInTauRslt(VOS_VOID)
{
    NAS_EMM_CN_TAU_TYPE_ENUM_UINT32     ulCnTauType;

    ulCnTauType = NAS_EMM_TAU_GetTAUtype();
    if(ulCnTauType == NAS_EMM_CN_TAU_TYPE_TA_UPDATING)
    {
        return  MMC_LMM_TA_UPDATING;
    }
    if(ulCnTauType == NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING)
    {
        return  MMC_LMM_COMBINED_TA_LA_UPDATING;
    }
    if(ulCnTauType == NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI)
    {
        return  MMC_LMM_COMBINED_TA_LA_WITH_IMSI;
    }

    if( NAS_LMM_REG_DOMAIN_PS == NAS_EMM_TAU_GetPeriodTauRegDomain())
    {
        return  MMC_LMM_PS_PERIODIC_UPDATING;
    }
    else if(NAS_LMM_REG_DOMAIN_CS_PS == NAS_EMM_TAU_GetPeriodTauRegDomain())
    {
        return  MMC_LMM_CS_PS_PERIODIC_UPDATING;
    }
    else
    {

    }

    NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcPeriodTauReqType: RegState is WRONG !!");
    return  MMC_LMM_TAU_TYPE_BUTT;

}



VOS_VOID  NAS_EMM_MmcSendTauActionResultIndSucc(VOS_VOID)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndSucc is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndSucc: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_SUCCESS;
    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();

    pstRsltInd->bitOpCnRst              = NAS_EMM_BIT_SLCT;
    pstRsltInd->ulCnRst                 = NAS_EMM_TAU_GetEpsUpdateRslt();


    /*填充EPLMN*/
    pstRsltInd->bitOpEplmn              = NAS_EMM_BIT_SLCT;
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr())
    {
        NAS_EMM_TAU_LOG1_INFO("NAS_EMM_MmcSendTauActionResultIndSucc: EPMN NUM ==",
                            NAS_LMM_GetEmmInfoNetInfoEplmnListAddr()->ulPlmnNum);
        NAS_LMM_MEM_CPY(                &pstRsltInd->stEplmnList,
                                        NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    /*填充LAI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_TAU_GetOpLai())
    {
        pstRsltInd->bitOpLai            = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                &pstRsltInd->stLai,
                                        NAS_EMM_TAU_GetLaiAddr(),
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    /*填充TMSI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_TAU_GetOpMsId())
    {
        pstRsltInd->bitOpMsIdentity           = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 pstRsltInd->stMsIdentity.aucMsIdentity,
                                        NAS_EMM_TAU_GetMsIdAddr()->aucMsId,
                                        NAS_MAX_SIZE_MS_IDENTITY);
    }
    /*填充消息内容----EMM CAUSE*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_TAU_GetOpCnCause())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AppSendAttSucc:EMM CAUSE");
        pstRsltInd->bitOpCnCause        = NAS_EMM_BIT_SLCT;
        pstRsltInd->ucCnCause           = NAS_EMM_TAU_GetCnCause();
    }
    /* 填充TAU ATTEMPT COUTER */
    if (NAS_EMM_NULL != NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        pstRsltInd->bitOpAtmpCnt        = NAS_EMM_BIT_SLCT;
        pstRsltInd->ulAttemptCount      = NAS_EMM_TAU_GetEmmTAUAttemptCnt();
    }
    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    NAS_EMM_TauRstSuccLeadSndAttachRst();
    return;
}


VOS_VOID  NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly
(
    const VOS_VOID                     *pvTauRslt
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;
    VOS_UINT32                      ulTaRslt;

    (VOS_VOID)pvTauRslt;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_CN_REJ;
    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();

    pstRsltInd->bitOpCnRst              = NAS_EMM_BIT_SLCT;
    pstRsltInd->ulCnRst                 = NAS_EMM_TAU_GetEpsUpdateRslt();

    /*填充EPLMN*/
    pstRsltInd->bitOpEplmn              = NAS_EMM_BIT_SLCT;
    if (NAS_EMM_BIT_SLCT == NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr())
    {
        NAS_EMM_TAU_LOG1_INFO("NAS_EMM_MmcSendTauActionResultIndRejTaUpdatedOnly: EPMN NUM ==",
                            NAS_LMM_GetEmmInfoNetInfoEplmnListAddr()->ulPlmnNum);
        NAS_LMM_MEM_CPY(                &pstRsltInd->stEplmnList,
                                        NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }

    /*填充LAI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_TAU_GetOpLai())
    {
        pstRsltInd->bitOpLai            = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                &pstRsltInd->stLai,
                                        NAS_EMM_TAU_GetLaiAddr(),
                                        sizeof(MMC_LMM_LAI_STRU));
    }

    /*填充TMSI*/
    if (NAS_EMM_BIT_SLCT == NAS_EMM_TAU_GetOpMsId())
    {
        pstRsltInd->bitOpMsIdentity           = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 pstRsltInd->stMsIdentity.aucMsIdentity,
                                        NAS_EMM_TAU_GetMsIdAddr()->aucMsId,
                                        NAS_MAX_SIZE_MS_IDENTITY);
    }

    /* 原因值不为#2#16#17#18#22时不给MMC上报原因值 */
    /*填充消息内容----EMM CAUSE*/
    pstRsltInd->bitOpCnCause            = NAS_EMM_BIT_NO_SLCT;
    pstRsltInd->ucCnCause               = NAS_EMM_CAUSE_BUTT;

    /* 填充TAU ATTEMPT COUTER */
    if (NAS_EMM_NULL != NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        pstRsltInd->bitOpAtmpCnt        = NAS_EMM_BIT_SLCT;
        pstRsltInd->ulAttemptCount      = NAS_EMM_TAU_GetEmmTAUAttemptCnt();

        /* 如果TAU尝试次数小于5，则携带EPS update status和TAI是否在TAI LIST中标识 */
        if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > pstRsltInd->ulAttemptCount)
        {
            pstRsltInd->bitOpEpsUpdateStuts = NAS_EMM_BIT_SLCT;
            pstRsltInd->enEmmUpStat         = NAS_EMM_UPDATE_STAE;
            pstRsltInd->bitOpTaiInListFlag  = NAS_EMM_BIT_SLCT;

            ulTaRslt                        = NAS_EMM_TAU_IsCurrentTAInTaList();
            if (NAS_EMM_SUCC == ulTaRslt)
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_IN_TAI_LIST;
            }
            else
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_NOT_IN_TAI_LIST;
            }
        }
    }

    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}
VOS_VOID  NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara( VOS_VOID )
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_SUCCESS;
    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();


    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}
/*****************************************************************************
 Function Name   : NAS_EMM_IsTauRejCauseOthers
 Description     : 判断TAU REJ原因值是否为5.5.3.2.5章节所列
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-11-18  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_IsTauRejCauseOthers
(
    VOS_UINT8                           ucCnCause
)
{
    VOS_UINT32                          ulRslt = NAS_EMM_YES;

    switch(ucCnCause)
    {
        case    NAS_LMM_CAUSE_ILLEGAL_UE   :
        case    NAS_LMM_CAUSE_ILLEGAL_ME   :
        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW   :
        case    NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW:
        case    NAS_LMM_CAUSE_UE_ID_NOT_DERIVED    :
        case    NAS_LMM_CAUSE_IMPLICIT_DETACHED   :
        case    NAS_LMM_CAUSE_PLMN_NOT_ALLOW    :
        case    NAS_LMM_CAUSE_TA_NOT_ALLOW   :
        case    NAS_LMM_CAUSE_ROAM_NOT_ALLOW    :
        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN    :
        case    NAS_LMM_CAUSE_NO_SUITABL_CELL   :
        case    NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG   :
        case    NAS_LMM_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED:

                ulRslt = NAS_EMM_NO;
                break;
        default :

                ulRslt = NAS_EMM_YES;
                break;
    }

    return ulRslt;
}



VOS_VOID  NAS_EMM_MmcSendTauActionResultIndRej
(
    const VOS_VOID                     *pvTauRslt
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulTaRslt = NAS_EMM_FAIL;
    NAS_LMM_CN_CAUSE_ENUM_UINT8         ucCnCause  = NAS_EMM_CAUSE_BUTT;

    (VOS_VOID)pvTauRslt;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndRej is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndRej: MSG ALLOC ERR !!");
        return;
    }

    ucCnCause                             = NAS_EMM_TAU_GetEmmTauCnRejCause();
    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_CN_REJ;
    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();


    /*填充拒绝原因值*/
    pstRsltInd->bitOpCnCause            = NAS_EMM_BIT_SLCT;
    pstRsltInd->ucCnCause               = ucCnCause;

    if (NAS_EMM_NULL != NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        pstRsltInd->bitOpAtmpCnt        = NAS_EMM_BIT_SLCT;
        pstRsltInd->ulAttemptCount      = NAS_EMM_TAU_GetEmmTAUAttemptCnt();

        /* 如果TAU REJ原因值不为5.5.3.2.5章节所列，且TAU尝试次数小于5，
           则携带EPS update status和TAI是否在TAI LIST中标识 */
        if ((NAS_EMM_YES == NAS_EMM_IsTauRejCauseOthers(ucCnCause))
            && (NAS_EMM_TAU_ATTEMPT_CNT_MAX > pstRsltInd->ulAttemptCount))
        {
            pstRsltInd->bitOpEpsUpdateStuts = NAS_EMM_BIT_SLCT;
            pstRsltInd->enEmmUpStat         = NAS_EMM_UPDATE_STAE;
            pstRsltInd->bitOpTaiInListFlag  = NAS_EMM_BIT_SLCT;

            ulTaRslt                        = NAS_EMM_TAU_IsCurrentTAInTaList();
            if (NAS_EMM_SUCC == ulTaRslt)
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_IN_TAI_LIST;
            }
            else
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_NOT_IN_TAI_LIST;
            }
        }
    }

    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}


VOS_VOID  NAS_EMM_MmcSendTauActionResultIndTimerExp
(
    const VOS_VOID                     *pvTauRslt
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulTaRslt = NAS_EMM_FAIL;

    (VOS_VOID)pvTauRslt;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndTimerExp is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndTimerExp: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_TIMER_EXP;
    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();


    /*填充尝试次数*/
    pstRsltInd->bitOpAtmpCnt            = NAS_EMM_BIT_SLCT;
    pstRsltInd->ulAttemptCount          = NAS_EMM_TAU_GetEmmTAUAttemptCnt();

   /* 如果TAU尝试次数小于5，则携带EPS update status和TAI是否在TAI LIST中标识 */
    if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > pstRsltInd->ulAttemptCount)
    {
        pstRsltInd->bitOpEpsUpdateStuts = NAS_EMM_BIT_SLCT;
        pstRsltInd->enEmmUpStat         = NAS_EMM_UPDATE_STAE;
        pstRsltInd->bitOpTaiInListFlag  = NAS_EMM_BIT_SLCT;

        ulTaRslt                        = NAS_EMM_TAU_IsCurrentTAInTaList();
        if (NAS_EMM_SUCC == ulTaRslt)
        {
            pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_IN_TAI_LIST;
        }
        else
        {
            pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_NOT_IN_TAI_LIST;
        }
    }

    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}



VOS_VOID  NAS_EMM_MmcSendTauActionResultIndFailWithPara
(
    const VOS_VOID                     *pvTauRslt
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulTaRslt = NAS_EMM_FAIL;

    (VOS_VOID)pvTauRslt;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndFailWithPara is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndFailWithPara: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    pstRsltInd->ulTauRst                = MMC_LMM_TAU_RSLT_FAILURE;

    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();


    if (NAS_EMM_NULL != NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        pstRsltInd->bitOpAtmpCnt        = NAS_EMM_BIT_SLCT;
        pstRsltInd->ulAttemptCount      = NAS_EMM_TAU_GetEmmTAUAttemptCnt();

        /* 如果TAU尝试次数小于5，则携带EPS update status和TAI是否在TAI LIST中标识 */
        if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > pstRsltInd->ulAttemptCount)
        {
            pstRsltInd->bitOpEpsUpdateStuts = NAS_EMM_BIT_SLCT;
            pstRsltInd->enEmmUpStat         = NAS_EMM_UPDATE_STAE;
            pstRsltInd->bitOpTaiInListFlag  = NAS_EMM_BIT_SLCT;

            ulTaRslt                        = NAS_EMM_TAU_IsCurrentTAInTaList();
            if (NAS_EMM_SUCC == ulTaRslt)
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_IN_TAI_LIST;
            }
            else
            {
                pstRsltInd->enTaiInListFlag = MMC_LMM_TAI_NOT_IN_TAI_LIST;
            }
        }
    }

    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}
VOS_VOID  NAS_EMM_MmcSendTauActionResultIndOthertype
(
    const VOS_VOID                     *pvTauRslt
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;
    MMC_LMM_TAU_RSLT_ENUM_UINT32     ulTauRslt;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndOthertype is entered.");

    ulTauRslt = *(MMC_LMM_TAU_RSLT_ENUM_UINT32*)pvTauRslt;


    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndOthertype: MSG ALLOC ERR !!");
        return;
    }

    /* 填充消息内容 */
    if ((MMC_LMM_TAU_RSLT_ACCESS_BARED == ulTauRslt) ||
        (MMC_LMM_TAU_RSLT_AUTH_REJ == ulTauRslt) ||
        (MMC_LMM_TAU_RSLT_FAILURE == ulTauRslt)||
        (MMC_LMM_TAU_RSLT_T3402_RUNNING == ulTauRslt)||
        (MMC_LMM_TAU_RSLT_MO_DETACH_FAILURE == ulTauRslt)||
        (MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE == ulTauRslt)||
        (MMC_LMM_TAU_RSLT_T3402_RUNNING == ulTauRslt))
    {
        NAS_EMM_TAU_LOG1_NORM("NAS_EMM_MmcSendTauActionResultIndOthertype:ulTauRslt =",
                            ulTauRslt);
        pstRsltInd->ulTauRst            = ulTauRslt;
    }
    else
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndOthertype:\
                                Input TAU Result Type is err !!");
    }

    pstRsltInd->ulReqType               = NAS_EMM_MmcTauReqTypeInTauRslt();


    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}


VOS_VOID  NAS_EMM_MmcSendTauActionResultIndForbType
(
    const VOS_VOID                     *pvForbInfo
)
{
    LMM_MMC_TAU_RESULT_IND_STRU     *pstRsltInd = NAS_EMM_NULL_PTR;
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 ulForbInfo;

    ulForbInfo = *(EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32*)pvForbInfo;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcSendTauActionResultIndForbType is entered.");

    pstRsltInd = NAS_EMM_TauRsltCommProc();
    if (NAS_EMM_NULL_PTR == pstRsltInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndForbType: MSG ALLOC ERR !!");
        return;
    }

    NAS_EMM_TAU_LOG1_NORM("NAS_EMM_MmcSendTauActionResultIndForbType:ulForbInfo =",
                            ulForbInfo);

    /* 填充消息内容 */
    if (EMMC_EMM_FORBIDDEN_PLMN == ulForbInfo)
    {
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FORBID_PLMN;
    }
    else if (EMMC_EMM_FORBIDDEN_PLMN_FOR_GPRS == ulForbInfo)
    {
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FORBID_PLMN_FOR_GPRS;
    }
    else if (EMMC_EMM_FORBIDDEN_TA_FOR_ROAMING == ulForbInfo)
    {
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FORBID_TA_FOR_ROAMING;
    }
    else if (EMMC_EMM_FORBIDDEN_TA_FOR_RPOS == ulForbInfo)
    {
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FORBID_TA_FOR_RPOS;
    }
    else if(EMMC_EMM_NO_FORBIDDEN == ulForbInfo)
    {
        /* 系统消息未被禁，但是调用此函数上报结果，一定是ANY CELL的系统消息 */
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FAILURE;
    }
    else
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MmcSendTauActionResultIndForbType:\
                                Input TAU Result Type is err !!");
        pstRsltInd->ulTauRst            = MMC_LMM_TAU_RSLT_FAILURE;
    }

    pstRsltInd->ulReqType               =  NAS_EMM_MmcTauReqTypeInTauRslt();


    /* 发送LMM_MMC_TAU_RESULT_IND消息 */
    NAS_LMM_SendLmmMmcMsg(                    pstRsltInd);

    /* 如果是ATTACH引起的TAU，在TAU结束后上报ATTACH */
    NAS_EMM_TauRstLeadSndAttachRst(MMC_LMM_ATT_RSLT_FAILURE);

    return;
}
VOS_UINT32    NAS_EMM_SendIntraAttachReq(VOS_VOID)
{
    NAS_LMM_INTRA_ATTACH_REQ_STRU        *pIntraAttachReqMsg = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_SendAttachReq is entered.");

    /* 申请DOPRA消息 */
    pIntraAttachReqMsg = (VOS_VOID *)NAS_EMM_TAUSER_GetSendIntraMsgBuffAddr(
                                        sizeof(NAS_LMM_INTRA_ATTACH_REQ_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    if(NAS_EMM_NULL_PTR == pIntraAttachReqMsg)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_SendAttachReq: MSG ALLOC ERR !!");
        return NAS_EMM_FAIL;
    }

    /* 填写NAS_EMM_INTRA_ATTACH_REQ 的DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(pIntraAttachReqMsg, sizeof(NAS_LMM_INTRA_ATTACH_REQ_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* 填写NAS_EMM_INTRA_ATTACH_REQ 的消息ID标识 */
    pIntraAttachReqMsg->ulMsgId         = ID_NAS_LMM_INTRA_ATTACH_REQ;

    /* 发送消息(Send the msg of) NAS_EMM_INTRA_ATTACH_REQ */
    NAS_EMM_SEND_INTRA_MSG(             pIntraAttachReqMsg);

    return NAS_EMM_SUCC;

}
VOS_VOID NAS_EMM_TAU_AbnormalOver( )
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_AbnormalOver is entered.");

    /*停止T3430定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    /*停止T3440定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    /* 清除发起TAU的原因值*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(   NAS_EMM_TAU_START_CAUSE_DEFAULT);

    /* 清空Tau Attempt Counter*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    return;
}
NAS_EMM_UPDATE_STATE_ENUM_UINT8 NAS_EMM_TAU_GetEmmEPSUPState()
{
    NAS_EMM_UPDATE_STATE_ENUM_UINT8   ucupdatastate;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_GetEmmEPSUPState is entered.");

    ucupdatastate = NAS_EMM_TAUSER_GetAuxFsmUpStat();

    return ucupdatastate;
}
VOS_VOID    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ENUM_UINT32  stATTACHStatus)
{


    EMM_ESM_STATUS_IND_STRU                *pstsmstatusInd            = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG1_INFO( "NAS_EMM_TAU_SendEsmStatusInd is entered.", stATTACHStatus);

    /* 申请DOPRA消息 */
    pstsmstatusInd  = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_STATUS_IND_STRU));

    if(NAS_EMM_NULL_PTR == pstsmstatusInd)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SendEsmStatusInd: MSG ALLOC ERR !!");
        return;
    }

    /* 填充给RRC的DOPRA头 */
    EMM_COMP_ESM_MSG_HEADER(pstsmstatusInd, sizeof(EMM_ESM_STATUS_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* 填充消息ID */
    pstsmstatusInd->ulMsgId = ID_EMM_ESM_STATUS_IND;

    /* 填充消息内容 -- stATTACHStatus */
    pstsmstatusInd->enEMMStatus = stATTACHStatus;

    /*clear global:EpsContextStatusChange*/
    if(stATTACHStatus == EMM_ESM_ATTACH_STATUS_DETACHED)
    {
        NAS_EMM_GetEpsContextStatusChange() = NAS_EMM_EPS_BEARER_STATUS_NOCHANGE;
    }

    /* 发送DOPRA消息 */
    NAS_LMM_SEND_MSG(pstsmstatusInd);

    return;

}
/*****************************************************************************
 Function Name   : NAS_EMM_TAU_GetEpsUpdateType
 Description     : 编码TAU REQ消息时，获取Eps Update Type的值
 Input           : 无
 Output          : 无
 Return          : MMC_LMM_TAU_TYPE_ENUM_UINT32

 History         :
    1.lihong00150010      2011-11-18    Draft Enact

*****************************************************************************/
NAS_EMM_CN_TAU_TYPE_ENUM_UINT32 NAS_EMM_TAU_GetEpsUpdateType
(
    NAS_EMM_TAU_START_CAUSE_ENUM_UINT8      enTauStartCause
)
{
    NAS_LMM_REG_DOMAIN_ENUM_UINT32          enRegDomain     = NAS_LMM_REG_DOMAIN_BUTT;


    /* 获取L模注册域 */
    enRegDomain = NAS_LMM_GetEmmInfoRegDomain();

    if (NAS_EMM_TAU_START_CAUSE_T3412EXP == enTauStartCause)
    {
        /* 保存周期性TAU的注册域，
            便于向MMC上报结果时区分是PS_PERIOD_TAU还是CS_PS_PERIOD_TAU */
        NAS_EMM_TAU_SavePeriodTauRegDomain(enRegDomain);
        return  NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING;
    }

    /* 判断是否为CP/PS1或者CS/PS2 */
    if (NAS_EMM_NO == NAS_EMM_IsCsPsUeMode())
    {
        return  NAS_EMM_CN_TAU_TYPE_TA_UPDATING;
    }

    /* 判断是否被原因值2拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_LMM_GetEmmInfoRejCause2Flag())
    {
        return  NAS_EMM_CN_TAU_TYPE_TA_UPDATING;
    }

    /* 判断是否被原因值18拒绝过 */
    if (NAS_EMM_REJ_YES == NAS_EMMC_GetRejCause18Flag())
    {
        return  NAS_EMM_CN_TAU_TYPE_TA_UPDATING;
    }

    /* 判断联合DETACH是否被TAU打断 */
    if (NAS_EMM_COM_DET_BLO_YES == NAS_EMM_TAU_GetEmmCombinedDetachBlockFlag())
    {
        return  NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 判断是否允许CS域注册 */
    if (NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
    {
    /*PC REPLAY MODIFY BY LEILI END*/
        return  NAS_EMM_CN_TAU_TYPE_TA_UPDATING;
    }

    if(NAS_LMM_REG_DOMAIN_PS == enRegDomain)
    {
        return  NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI;
    }
    else if(NAS_LMM_REG_DOMAIN_CS_PS == enRegDomain)
    {
        return  NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING;
    }
    else
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_GetEpsUpdateType:enRegDomain = NAS_LMM_REG_DOMAIN_NULL");

        return  NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING;
    }
}


VOS_VOID    NAS_EMM_TAU_CompCnTAUReqNasMsg
(
    VOS_UINT8      *pucCnNasMsg,
    VOS_UINT32     *pulIndex
)
{
    VOS_UINT32                          ulIndex             = 0;
    VOS_UINT32                          ulIeLength          = 0;
    VOS_UINT8                           ucKsi;
    NAS_EMM_CN_TAU_TYPE_ENUM_UINT32     enTauUpdateType     = NAS_EMM_CN_TAU_TYPE_BUTT;
    NAS_EMM_TAU_START_CAUSE_ENUM_UINT8  enTauStartCause     = NAS_EMM_TAU_START_CAUSE_BUTT;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG1_INFO("NAS_EMM_TAU_CompCnTAUReqNasMsg is entered.",*pulIndex);


    /* 填充 Security header type  +  Protocol Discriminator */
    pucCnNasMsg[ulIndex++]              = EMM_TAU_REQ_MSG_PD_AND_SEC_TY;

    /* 填充 TAU request message identity   */
    pucCnNasMsg[ulIndex++]              = NAS_EMM_CN_MT_TAU_REQ;

    /* 获取TAU更新类型 */
    enTauStartCause = NAS_EMM_TAU_GetEmmTAUStartCause();
    enTauUpdateType = NAS_EMM_TAU_GetEpsUpdateType(enTauStartCause);
    NAS_EMM_TAU_LOG1_NORM("Tau Type: ", enTauUpdateType);
    NAS_EMM_TAU_SaveTAUtype(enTauUpdateType);

    /* 填充 EPS update type */
    pucCnNasMsg[ulIndex] = (VOS_UINT8)NAS_EMM_TAU_GetTAUtype();
    pucCnNasMsg[ulIndex] |= (NAS_EMM_TAU_GetEmmTauReqActiveCtrl()&0x1f)<<3;

    /* 填充信元NAS key set identifierASME */
    ucKsi = NAS_EMM_SecuGetKSIValue();
    pucCnNasMsg[ulIndex] |= (VOS_UINT8)(ucKsi << 4);
    ulIndex++;

    /*填充OLD GUTI*/
    NAS_EMM_EncodeOldGuti(              &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Non-current native NAS key set identifier*/
    NAS_EMM_EncodeNonCurrentNativeKSI(  &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充GPRS ciphering key sequence number*/
    NAS_EMM_EncodeGprsCkSn(             &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /* 填充Old P-TMSI signature*/
    NAS_EMM_EncodePtmsiSignature(       &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Additional GUTI*/
    NAS_EMM_EncodeAdditionalGuti(       &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充NonceUE*/
    NAS_EMM_EncodeNonceUE(              &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /* 填充UE network capability*/
    if((EMM_TAU_AD_BIT_SLCT == NAS_EMM_TAU_UE_NET_CAP_BIT())
        && ( NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING != NAS_EMM_TAU_GetTAUtype()))
    {
        pucCnNasMsg[ulIndex++]          = NAS_EMM_TAU_REQ_UE_NET_CAP_IEI;

        /* UE NETWORK CAPABILITY 是必选IE, LV，3~14*/
        NAS_EMM_EncodeUeNetCapa(        &(pucCnNasMsg[ulIndex]),
                                        NAS_LMM_IE_FORMAT_LV,
                                        &ulIeLength);
        ulIndex += ulIeLength;

    }

    /* 填充Last visited registered TAI */
    NAS_EMM_EncodeLRVTai(               &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /* 填充 DRX parameter */
    NAS_EMM_EncodeDrxPara(              &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_TAU_REQ);
    ulIndex                             += ulIeLength;


    /*填充UE radio capability information update needed*/
    NAS_EMM_EncodeURCUpdateNeed(        &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /* 填充EPS bearer context status */
    /*leili modify for isr begin*/
    NAS_EMM_EncodeEpsBearStatus(        &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;
    /*leili modify for isr end*/
    /* 填充 MS network capability */
    NAS_EMM_EncodeMsNetworkCapability(  &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_TAU_REQ);
    ulIndex                             += ulIeLength;


    /* 填充Old location area identification */
    NAS_EMM_EncodeLai(                  &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_TAU_REQ);
    ulIndex                             += ulIeLength;


    /*填充TMSI STATUS*/
    NAS_EMM_EncodeTmsiStatus(           &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_TAU_REQ);
    ulIndex                             += ulIeLength;

    /*填充Mobile station classmark 2*/
    NAS_EMM_EncodeMsClassMark2(         &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength,
                                        NAS_EMM_CN_MT_TAU_REQ);
    ulIndex                             += ulIeLength;

    /*填充Mobile station classmark 3*/
    NAS_EMM_EncodeMsClassMark3(         &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Supported Codecs*/
    NAS_EMM_EncodeSupportedCodecs(      &(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;

    /*填充Additional update type*/
    if ((NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING  == NAS_EMM_TAU_GetTAUtype()) ||
        (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI == NAS_EMM_TAU_GetTAUtype()))
    {
        NAS_EMM_EncodeAdditionUpdateType(   &(pucCnNasMsg[ulIndex]),
                                            &ulIeLength);
        ulIndex                             += ulIeLength;
    }

    /*填充Voice domain preference and UE's usage setting*/
    NAS_EMM_EncodeVoiceDomainAndUsageSetting(&(pucCnNasMsg[ulIndex]),
                                        &ulIeLength);
    ulIndex                             += ulIeLength;
    if (NAS_RELEASE_CTRL)
    {
        /* 填充Old GUTI type */
        NAS_EMM_EncodeGutiType(             &(pucCnNasMsg[ulIndex]),
                                            &ulIeLength);
        ulIndex                            += ulIeLength;
    }
    *pulIndex += ulIndex;

    return;
}
VOS_VOID  NAS_EMM_EncodeOldGuti
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          ulLen = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeOldGuti: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    if(NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:SUPPORT GU MODE ");
        if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:TIN IS P-TMSI ");
            if (NAS_EMM_SUCC == NAS_EMM_CheckPtmsiAndRaiValidity())
            {
                NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:P-TMSI AND RAI VALID ");
                NAS_EMM_CompMappedGuti(&(pucReqMsg[ulIndex]),&ulLen);
                *pulIeLength                = ulLen;
            }
        }
        else if ((MMC_LMM_TIN_GUTI == NAS_EMM_GetTinType()) ||
            (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:TIN IS GUTI or RAT RELATED TMSI");
            if (NAS_EMM_BIT_SLCT == NAS_EMM_TAUSER_OP_GUTI())
            {
                NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti: NATIVE GUTI VALID");
                NAS_EMM_CompNativeGuti(&(pucReqMsg[ulIndex]),&ulLen);
                *pulIeLength            = ulLen;
            }
        }
        else
        {
            NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeOldGuti: TIN INVALID");
        }
    }
    else
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:NOT SUPPORT GU MODE ");
        if (NAS_EMM_BIT_SLCT == NAS_EMM_TAUSER_OP_GUTI())
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_EncodeOldGuti:NATIVE GUTI INVALID ");
            NAS_EMM_CompNativeGuti(&(pucReqMsg[ulIndex]),&ulLen);
            *pulIeLength                = ulLen;
        }
    }

    return;
}




VOS_UINT32  NAS_EMM_IsConnChangeToLte( VOS_VOID )
{
    if((MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
        || (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {   /* 刚从GU过来 */

        if( (NAS_LMM_SYS_CHNG_ORI_LRRC == NAS_EMM_GetResumeOri()) &&
            (NAS_LMM_SYS_CHNG_TYPE_HO   == NAS_EMM_GetResumeType())
           )
        {   /* HO,过来 */

            return  NAS_EMM_YES;
        }
    }

    return  NAS_EMM_NO;
}


/*****************************************************************************
 Function Name   : NAS_EMM_IsPtmsiAndIdle
 Description     : 预研认为，CCO和重定向时，网络节点间不存在安全上下文的传递，
                   因此CCO和重定向当做IDLE过程来处理；
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Hanlufeng 41410      2011-6-24  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_IsPtmsiAndIdle( VOS_VOID )
{
    if(MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
    {   /* 刚从GU过来 */

        if( (NAS_LMM_SYS_CHNG_ORI_LRRC == NAS_EMM_GetResumeOri()) &&
            (   (NAS_LMM_SYS_CHNG_TYPE_RSL  == NAS_EMM_GetResumeType()) ||
                (NAS_LMM_SYS_CHNG_TYPE_CCO  == NAS_EMM_GetResumeType()) ||
                (NAS_LMM_SYS_CHNG_TYPE_REDIR== NAS_EMM_GetResumeType())
            ))
        {   /* 重选/CCO/重定向 过来 */

            return  NAS_EMM_YES;
        }
        else if(NAS_LMM_SYS_CHNG_ORI_MMC == NAS_EMM_GetResumeOri())
        {   /* MMC触发过来的 */

            return  NAS_EMM_YES;
        }
        else
        {

        }
    }

    return  NAS_EMM_NO;
}
VOS_VOID  NAS_EMM_EncodeNonCurrentNativeKSI
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{
    VOS_UINT8                          *pucReqMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    /* 入参合法性检查*/
    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeNonCurrentNativeKSI: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
   *pulIeLength                         = NAS_EMM_NULL;

    /* 如果不存在 non-current native security context,则直接返回，不填充该信元*/
    if(NAS_LMM_NKSI_NO_KEY_IS_AVAILABLE == NAS_EMM_SecuGetNonCurKSIasme())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_EncodeNonCurrentNativeKSI: no para.");
        return;
    }

    /* 若是从GU过来，并且CONNECT 态则填，与预研交流，认为重定向也属连接态.
    8.2.29.2    Non-current native NAS key set identifier
    The UE shall include this IE if the UE has a valid non-current native EPS
    security context when the UE performs an A/Gb mode or Iu mode to S1 mode
    inter-system change in EMM-CONNECTED mode and the UE uses a mapped EPS
    security context to protect the TRACKING AREA UPDATE REQUEST message.
    */
    if(NAS_EMM_YES == NAS_EMM_IsConnChangeToLte())
    {
        NAS_EMM_TAU_LOG2_INFO("NAS_EMM_EncodeNonCurrentNativeKSI: TinType = , ResumeType = ",
                                NAS_EMM_GetTinType(),
                                NAS_EMM_GetResumeType());

        /* 填充Non-current native NAS key set identifier IEI*/
        pucReqMsg[ulIndex]              = NAS_EMM_TAU_REQ_NON_CURRENT_NATIVE_KSI;

        /* 填充 Non-current native NAS key set identifier的 TSC */
        pucReqMsg[ulIndex]              |= NAS_EMM_SecuGetNonCurKSIasme();
        ulIndex++;

        /* 信元长度*/
        *pulIeLength = ulIndex;
    }

    /* non-current native security context存在，填充该信元*/

    return;

}


VOS_VOID  NAS_EMM_EncodeGprsCkSn
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT8                           ucGprsCkSn = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeGprsCkSn: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;

    ucGprsCkSn = (VOS_UINT8)NAS_EMM_SecuGetUmtsKsi();

    /*
    8.2.29.3    GPRS ciphering key sequence number
    The UE shall include this IE if the UE performs an A/Gb mode or Iu mode
    to S1 mode inter-system change in EMM-IDLE mode and the TIN indicates "P-TMSI"
    */
    if (NAS_EMM_YES == NAS_EMM_IsPtmsiAndIdle() )
    {
        /*填充GPRS ciphering key sequence number IEI*/
        pucReqMsg[ulIndex]              = NAS_EMM_TAU_REQ_GPRS_CK_SN;

        /* 填充 GPRS ciphering key sequence number的 spare */
        pucReqMsg[ulIndex]              &= NAS_EMM_TAU_REQ_IE_SPARE;

        /* 填充GPRS ciphering key sequence number的key sequence */
        pucReqMsg[ulIndex]              |= ucGprsCkSn;

        ulIndex ++;
        *pulIeLength                = ulIndex;

    }

    return;
}
VOS_VOID  NAS_EMM_EncodeNonceUE
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{

    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeNonceUE: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;


    /*
    8.2.29.6    NonceUE
    This IE is included if the UE performs an A/Gb mode or Iu mode to S1 mode
    inter-system change in idle mode.
    */
    if (NAS_EMM_YES == NAS_EMM_IsPtmsiAndIdle() )
    {

        /*填充NONCE UE IEI*/
        pucReqMsg[ulIndex++]                = NAS_EMM_TAU_REQ_Nonce_UE_IEI;

        /* 填充 NONCE UE的 VALUE */
        NAS_LMM_MEM_CPY(                     &pucReqMsg[ulIndex],
                                            NAS_EMM_SecuGetNonceUE(),
                                            sizeof(VOS_UINT32));

        ulIndex                             += sizeof(VOS_UINT32);

        *pulIeLength                        = ulIndex;
    }

    return;

}


VOS_VOID  NAS_EMM_EncodeURCUpdateNeed
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{

    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeURCUpdateNeed: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;


    /* 如果发起TAU时UE的GU无线能力改变标识仍存在，则应填充URC_UPDATE_NEED
       用于UE无线能力上报，提醒MME对UE能力进行重新查询 */
    if(NAS_LMM_UE_RADIO_CAP_GU_CHG == NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        NAS_LMM_SetEmmInfoFirstTauFlag(NAS_EMM_FIRST_TAU);
    }


    /*if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())*/
    if (NAS_EMM_FIRST_TAU == NAS_LMM_GetEmmInfoFirstTauFlag())
    {
        /*填充UE radio capability information update needed IEI*/
        pucReqMsg[ulIndex]            = NAS_EMM_TAU_REQ_RADIO_CAP_UPDATE_NEED_IEI;

        /* 填充 UE radio capability information update needed IEI的spare*/
        pucReqMsg[ulIndex]              &= NAS_EMM_TAU_REQ_IE_SPARE;

        /* 填充UE radio capability information update needed IEI的update needed */
        pucReqMsg[ulIndex]              |= NAS_EMM_URC_UPDATE_NEED;

        ulIndex ++;

        *pulIeLength                        = ulIndex;

    }

    return;
}


/*leili modify for isr begin*/

VOS_VOID  NAS_EMM_EncodeEpsBearStatus
(
    VOS_UINT8       *pMsg,
    VOS_UINT32      *pulIeLength
)
{
    VOS_UINT8                           *pucReqMsg;
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          i;
    NAS_MML_PS_BEARER_CONTEXT_STRU      *pstEpsBearerCxt;
    VOS_UINT32                          ucEpsId;
    VOS_UINT8                           ucEsmEPS1           = 0;
    VOS_UINT8                           ucEsmEPS2           = 0;

    if((VOS_NULL_PTR == pMsg) ||
        (VOS_NULL_PTR == pulIeLength))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_EncodeEpsBearStatus: Input para is invalid");
        return;
    }

    pucReqMsg                           = pMsg;
    *pulIeLength                        = NAS_EMM_NULL;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    pstEpsBearerCxt = NAS_EMM_GetPsBearerCtx();
    /*PC REPLAY MODIFY BY LEILI END*/
    pucReqMsg[ulIndex++]          = NAS_EMM_TAU_REQ_EPS_STATUS_IEI;
    pucReqMsg[ulIndex++]          = NAS_EMM_CN_EPS_CONTEXT_LEN;
    for(i = 0; i < EMM_ESM_MAX_EPS_BEARER_NUM; i++)
    {
        if (NAS_MML_PS_BEARER_STATE_ACTIVE == pstEpsBearerCxt[i].enPsBearerState)
        {
            ucEpsId = i + NAS_EMM_MIN_EPS_ID;
            if( NAS_EMM_EPS_BEARER_STATUS_NUM_SEVEN < ucEpsId)
            {
                ucEpsId             = ucEpsId - NAS_EMM_EPS_BEARER_STATUS_NUM_EIGNT;
                ucEsmEPS2 = ucEsmEPS2 | NAS_EMM_TAU_ESM_BEARER_LEFT_N(ucEpsId);
            }
            else
            {
                ucEsmEPS1 = ucEsmEPS1 | NAS_EMM_TAU_ESM_BEARER_LEFT_N(ucEpsId);
            }

        }
    }
    pucReqMsg[ulIndex++]                = ucEsmEPS1;
    pucReqMsg[ulIndex++]                = ucEsmEPS2;
    *pulIeLength                        = ulIndex;


    return;
}
/*leili modify for isr end*/

VOS_VOID    NAS_EMM_TAU_SendMrrcDataReq_TAUReq(NAS_EMM_MSG_TYPE_ENUM_UINT32 ulEmmMsgType)
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataReqMsg    = NAS_EMM_NULL_PTR;
    VOS_UINT32                          uldataReqMsgLenNoHeader;
    VOS_UINT32                          ulIndex             =0;


    /* 打印进入该函数， INFO_LEVEL */
    if(NAS_EMM_MSG_LOAD_BALANCING_TAU == ulEmmMsgType)
    {
        NAS_EMM_TAU_LOG_NORM( "NAS_EMM_TAU_SendMrrcDataReq_TAUReq is entered,TAUType:NAS_EMM_MSG_LOAD_BALANCING_TAU");
    }
    else
    {
        NAS_EMM_TAU_LOG_NORM( "NAS_EMM_TAU_SendMrrcDataReq_TAUReq is entered,TAUType:NAS_EMM_MSG_NOT_LOAD_BALANCING_TAU");
    }

    /* 计算NAS_MSG_STRU消息长度 */


    /* 以最短消息长度申请DOPRA消息 */
    pMrrcDataReqMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC( NAS_EMM_INTRA_MSG_MAX_SIZE);
    if(NAS_EMM_NULL_PTR == pMrrcDataReqMsg)
    {
        NAS_EMM_TAU_LOG_ERR( "NAS_EMM_TAU_SendMrrcDataReq_TAUReq: MSG ALLOC ERR !!");
        return;
    }
	/* lihong00150010 emergency tau&service begin */
    /* 如果存在上行Pending或者存在COLLISION标志，则携带'ACTIVE'标志*/
    if((NAS_EMM_UPLINK_PENDING == NAS_EMM_TAU_GetEmmUplinkPending())
     || (NAS_EMM_COLLISION_NONE != NAS_EMM_TAU_GetEmmCollisionCtrl())
     || (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause()))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_WITH_BEARER_EST_REQ);
    }
    else
    {
        NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_NO_BEARER_EST_REQ);
    }

    /* 构造MRRC_DATA_REQ 中的 NAS_MSG_STRU,即CN消息(Compose the msg of): TAU_REQ */
    NAS_EMM_TAU_CompCnTAUReqNasMsg(pMrrcDataReqMsg->stNasMsg.aucNasMsg,&ulIndex);

    NAS_EMM_TAU_SaveEmmTauImsiDetachFlag(NAS_EMM_IMSI_DETACH_INVALID);

    uldataReqMsgLenNoHeader              = EMM_COUNT_DATA_REQ_LEN(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < uldataReqMsgLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_TAU_CompCnTAUReqNasMsg, Size error ");

        NAS_LMM_MEM_FREE(pMrrcDataReqMsg);
        return ;
    }

    /* 填写MRRC_DATA_REQ 的DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(pMrrcDataReqMsg, uldataReqMsgLenNoHeader);

    /* 填写MRRC_DATA_REQ 的消息ID标识 */
    pMrrcDataReqMsg->ulMsgId                 = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填充 TAU_REQ_EST_CAUSE*/
    /* lihong00150010 emergency tau&service begin */
    /*填充 TAU_REQ_EST_CAUSE*/
    if ((NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus())
        || (NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR == NAS_LMM_GetEmmInfoRegStatus())
        || (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause()))
    {
        pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_EMERGENCY_CALL;
        pMrrcDataReqMsg->enCallType= LRRC_LNAS_CALL_TYPE_EMERGENCY_CALL;
    }
    else
    {
        pMrrcDataReqMsg->enEstCaue = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
        pMrrcDataReqMsg->enCallType= LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    }
    /* lihong00150010 emergency tau&service end */
    pMrrcDataReqMsg->enEmmMsgType = ulEmmMsgType;

    pMrrcDataReqMsg->enDataCnf               = LRRC_LMM_DATA_CNF_NEED;

    /* 填写MRRC_DATA_REQ 的NAS消息长度标识 */
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize   = ulIndex;

    /*设置TAU触发时的连接状态*/
    NAS_EMM_TAU_SaveEmmTauStartConnSta(NAS_EMM_GetConnState());

    /* 如果是联合TAU，则通知MM进入MM LOCATION UPDATING PENDING状态 */
    if ((NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING == NAS_EMM_TAU_GetTAUtype())
        || (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI == NAS_EMM_TAU_GetTAUtype()))
    {
        NAS_EMM_SendMmCombinedStartNotifyReq(MM_LMM_COMBINED_TAU);
    }

    /*NAS_EMM_TAU_SaveEmmT3412ExpCtrl(NAS_EMM_NO);*/

    /*空口消息上报TAU REQ*/
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_TAU_REQ,(NAS_MSG_STRU *)&(pMrrcDataReqMsg->stNasMsg));

    /*关键事件上报TAU REQ*/
    NAS_LMM_SendOmtKeyEvent(         EMM_OMT_KE_TAU_REQ);

    /* 发送消息(Send the msg of) MRRC_DATA_REQ(TAU_REQ) */
    NAS_EMM_SndUplinkNasMsg(             pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);

    return;

}
VOS_VOID    NAS_EMM_TAU_SendEsmBearerStatusInd(const NAS_EMM_CN_TAU_ACP_STRU   *pMsgStru )
{
    EMM_ESM_BEARER_STATUS_IND_STRU       *psmbearerstatusind  =  NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_SendEsmBearerStatusInd is entered.");

    /* 申请DOPRA消息 */
    psmbearerstatusind = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_BEARER_STATUS_IND_STRU));

    if(NAS_EMM_NULL_PTR == psmbearerstatusind)
    {
        NAS_EMM_TAU_LOG_ERR( "NAS_EMM_TAU_SendEsmBearerStatusInd: MSG ALLOC ERR !!");
        return;
    }

    /* 填写DOPRA消息头 */
    EMM_COMP_ESM_MSG_HEADER(psmbearerstatusind, sizeof(EMM_ESM_BEARER_STATUS_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* 填写消息ID标识 */
    psmbearerstatusind->ulMsgId    = ID_EMM_ESM_BEARER_STATUS_IND;

    /* 填写当前已激活的EPSID个数  */
    psmbearerstatusind->ulEpsIdNum = pMsgStru->stEPSBearerstatus.ulEpsIdNum;

    /* 填写 存贮已激活的EPS BID  */
    NAS_LMM_MEM_CPY(                        &(psmbearerstatusind->aulEsmEpsId[0]),
                                        &(pMsgStru->stEPSBearerstatus.aulEsmEpsId[0]),
                                        EMM_ESM_MAX_EPS_BEARER_NUM*sizeof(VOS_UINT32));


    /* 发送消息*/
    NAS_LMM_SEND_MSG(psmbearerstatusind);

    return;


}
VOS_VOID    NAS_EMM_TAU_SendDetachReq()
{
    NAS_LMM_INTRA_DETACH_REQ_STRU             *pIntraDetachReqMsg = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_SendDetachReq is entered.");

    /* 申请DOPRA消息 */
    pIntraDetachReqMsg = (VOS_VOID *)NAS_EMM_TAUSER_GetSendIntraMsgBuffAddr(
                                        sizeof(NAS_LMM_INTRA_DETACH_REQ_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    if(NAS_EMM_NULL_PTR == pIntraDetachReqMsg)
    {
        NAS_EMM_TAU_LOG_ERR( "Nas_Emm_Sau_SendAttachReq: MSG ALLOC ERR !!");
        return;
    }

    /* 填写NAS_EMM_INTRA_DETACH_REQ 的DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(pIntraDetachReqMsg, sizeof(NAS_LMM_INTRA_DETACH_REQ_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* 填写NAS_EMM_INTRA_DETACH_REQ 的消息ID标识 */
    pIntraDetachReqMsg->ulMsgId                 = ID_NAS_LMM_INTRA_DETACH_REQ;

    /* 发送消息(Send the msg of) NAS_EMM_INTRA_DETACH_REQ */
/*    NAS_EMM_TAUSER_SendIntraMsg(pIntraDetachReqMsg);*/
    NAS_EMM_SEND_INTRA_MSG(             pIntraDetachReqMsg);

    return;
}




VOS_VOID    NAS_EMM_TAU_CompCnTAUCmpl(LRRC_LNAS_MSG_STRU  *pNasMsg,
                                                        VOS_UINT32 *pulIndex )
{
    VOS_UINT32                          ulIndex             = 0;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_CompCnTAUCmpl is entered.");

    /* 清空将填充CN消息的空间 */
    /* xiongxianghui00253310 delete memset 2013-11-26 begin */
    /* NAS_LMM_MEM_SET(pNasMsg->aucNasMsg, 0, pNasMsg->ulNasMsgSize); */
    /* xiongxianghui00253310 delete memset 2013-11-26 end   */

    /* 填充 Skip indicator  +  Protocol Discriminator */
    pNasMsg->aucNasMsg[ulIndex++]       = EMM_CN_MSG_PD_EMM;

    /* 填充 TAU CMPL message identity   */
    pNasMsg->aucNasMsg[ulIndex++]       = NAS_EMM_CN_MT_TAU_CMP;

    *pulIndex = ulIndex;

    return;

}
VOS_VOID    NAS_EMM_TAU_SendMrrcDataReqTauCmpl( )
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataReqMsg    = NAS_EMM_NULL_PTR;
    VOS_UINT32                          uldataReqMsgLenNoHeader;
    VOS_UINT32                          ulIndex             = 0;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_SendMrrcDataReqTauCmpl is entered.");


    /* 申请消息队列 */
    pMrrcDataReqMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC( NAS_EMM_INTRA_MSG_MAX_SIZE);
    if(NAS_EMM_NULL_PTR == pMrrcDataReqMsg)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_TAU_SendMrrcDataReqTauCmpl: MSG ALLOC ERR !!");
        return;
    }

    /* xiongxianghui00253310 add memset 2013-11-26 begin */

    NAS_LMM_MEM_SET(pMrrcDataReqMsg, 0, NAS_EMM_INTRA_MSG_MAX_SIZE);
    /* xiongxianghui00253310 add memset 2013-11-26 end   */

    /* 构造MRRC_DATA_REQ 中的 NAS_MSG_STRU,即CN消息(Compose the msg of): TAU_REQ */
    NAS_EMM_TAU_CompCnTAUCmpl(            &(pMrrcDataReqMsg->stNasMsg),&ulIndex);

    uldataReqMsgLenNoHeader              = EMM_COUNT_DATA_REQ_LEN(ulIndex);

    if ( NAS_EMM_INTRA_MSG_MAX_SIZE < uldataReqMsgLenNoHeader )
    {
        /* 打印错误信息 */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_TAU_CompCnTAUCmpl, Size error ");
        NAS_LMM_MEM_FREE(pMrrcDataReqMsg);
        return ;
    }

    /* 填写MRRC_DATA_REQ 的DOPRA消息头 */
    EMM_COMP_MM_MSG_HEADER(pMrrcDataReqMsg, uldataReqMsgLenNoHeader);

    /* 填写MRRC_DATA_REQ 的消息ID标识 */
    pMrrcDataReqMsg->ulMsgId            = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填充 TAU_COMPL_EST_CAUSE*/
    pMrrcDataReqMsg->enEstCaue          = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
    pMrrcDataReqMsg->enCallType         = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    pMrrcDataReqMsg->enEmmMsgType       = NAS_EMM_MSG_TAU_CMP;

    pMrrcDataReqMsg->enDataCnf          = LRRC_LMM_DATA_CNF_NEED;

    /* 填写MRRC_DATA_REQ 的NAS消息长度标识 */
    pMrrcDataReqMsg->stNasMsg.ulNasMsgSize   = ulIndex;

    /*空口消息上报TAU COMPLETE*/
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_TAU_CMP, (NAS_MSG_STRU *)&(pMrrcDataReqMsg->stNasMsg));

    /*关键事件上报TAU COMPLETE*/
    NAS_LMM_SendOmtKeyEvent(         EMM_OMT_KE_TAU_CMP);

    NAS_EMM_SndUplinkNasMsg(             pMrrcDataReqMsg);

    NAS_LMM_MEM_FREE(pMrrcDataReqMsg);
    return;

}




VOS_UINT32    NAS_EMM_TAU_CHKFSMStateMsgp(
                                    NAS_EMM_MAIN_STATE_ENUM_UINT16  stMS,
                                    NAS_EMM_SUB_STATE_ENUM_UINT16   stSS,
                                    VOS_VOID   *pMsgStru)
{
    (VOS_VOID)(pMsgStru);
    if(NAS_EMM_CHK_STAT_INVALID(stMS, stSS))
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMState: STATE ERR !!");
        return NAS_EMM_FAIL;
    }
    if ( NAS_EMM_NULL_PTR == pMsgStru )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKMsgp: pMsgStru is NULL_PTR !!");
        return NAS_EMM_FAIL;
    }
    return NAS_EMM_SUCC;
}
NAS_EMM_TAU_START_CAUSE_ENUM_UINT8  NAS_EMM_TAU_GetTauStartCause(
                                        VOS_VOID)
{
    return NAS_EMM_TAU_GetEmmTAUStartCause();
}


NAS_EMM_IMSI_DETACH_FLAG_ENUM_UINT8  NAS_EMM_TAU_GetImsiDetachFlag( VOS_VOID )
{
    return NAS_EMM_TAU_GetEmmTauImsiDetachFlag();
}


VOS_VOID NAS_EMM_TAU_SetImsiDetachFlag
(
    NAS_EMM_IMSI_DETACH_FLAG_ENUM_UINT8 enImsiDetachFlag
)
{
    NAS_EMM_TAU_SaveEmmTauImsiDetachFlag(enImsiDetachFlag);
}


VOS_VOID  NAS_EMM_TAU_IsUplinkPending( VOS_VOID )
{
   if((EMM_ESM_UP_SINGNAL_PENDING == NAS_ESM_IsUpSingnalPending())
    ||(EMM_ERABM_UP_DATA_PENDING == NAS_ERABM_IsDataPending()))
   {
        NAS_EMM_TAU_SetEmmUplinkPending(NAS_EMM_UPLINK_PENDING);
   }
   else
   {
        NAS_EMM_TAU_SetEmmUplinkPending(NAS_EMM_NO_UPLINK_PENDING);
   }

   return;
}
/*****************************************************************************
 Function Name   : NAS_EMM_TAU_CanTriggerComTauWithIMSI
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY               2012-02-25  Draft Enact
    2.lihong00150010    2012-12-19  Modify:Emergency

*****************************************************************************/
VOS_UINT32 NAS_EMM_TAU_CanTriggerComTauWithIMSI(VOS_VOID)
{/* lihong00150010 emergency tau&service begin */
    if((NAS_LMM_REG_DOMAIN_PS == NAS_LMM_GetEmmInfoRegDomain())
        && ( NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI == NAS_EMM_TAU_GetEpsUpdateType(NAS_EMM_TAU_START_CAUSE_OTHERS))
        && (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus()))
    {/* lihong00150010 emergency tau&service end */
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
/* lihong00150010 emergency tau&service begin */
#if 0
/*****************************************************************************
 Function Name   : NAS_EMM_TimerRunningStateChng
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY         2012-02-25  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_TAU_TimerRunningStateChng( NAS_LMM_STATE_TI_ENUM_UINT16 enStateTimerId )
{
    if ((EMM_US_UPDATED_EU1 == NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat))
    {
        /* 判断能否发起联合TAU */
        if(NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {

            /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM状态 */
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                           enStateTimerId);
         }
         else
         {
             /* 将状态转移至MS_REG + SS_REG_NORMAL_SERVICE状态 */
             NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            enStateTimerId);

         }
    }
    else
    {
        /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE状态 */
        NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                       EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                       enStateTimerId);

    }
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_TAU_RrcRelCauseTriggerTau(VOS_VOID)
{
    NAS_LMM_NETWORK_INFO_STRU           *pMmNetInfo = NAS_EMM_NULL_PTR;

    if(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE == NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:load balance");
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }
    else if(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE ==
                                        NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
         /* 不能删除，如果有上行PENDING,则会发起SERVICE */
         NAS_EMM_TAU_IsUplinkPending();

         if(NAS_EMM_UPLINK_PENDING == NAS_EMM_TAU_GetEmmUplinkPending())
         {
             /*存在上行数据pending时,触发SR之前需要给MMC上报TAU结果以使MMC通知CDS切换至LTE模式 */
             pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();
             NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
             NAS_EMM_SetTauTypeNoProcedure();
             NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();
             NAS_EMM_TAUSER_SaveAuxFsmUpStat(EMM_US_UPDATED_EU1);
             /* lihong00150010 emergency tau&service begin */
             if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
             {
                 /*保存PS LOC信息*/
                 NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
             }
             /* lihong00150010 emergency tau&service end */
             NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:Init Ser.");
             NAS_EMM_SER_UplinkPending();
         }
         else
         {
             NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:RRC connection failure");
             /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE );*/
             NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
             NAS_EMM_TAU_StartTAUREQ();
         }

         return NAS_EMM_YES;
    }
    else
    {

    }

    return NAS_EMM_NO;
}
/*****************************************************************************
 Function Name   : NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc
 Description     : Reg.Limite_Service
                   Reg.Plmn_Search
                   Reg.No_Cell_Available
                   状态下收到SYS_INFO_IND时处理T3412定时器超时
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010  2011-08-31  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc( VOS_VOID )
{
    /* 协议24301 5.3.5章节描述，在REG+NO_AVAILABLE_CELL状态下，T3412超时，如果
       ISR激活，就启动T3423定时器;如果ISR没有激活，如果
       当前是联合注册成功，则回到REG+NORMAL_SERVICE态后，发起联合TAU，TAU类型为
       combined TA/LA updating with IMSI attach；如果不是联合注册成功，则发起周期性TAU
   */
    if (NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL ==  NAS_LMM_GetEmmInfoT3412ExpCtrl())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc:REG+NO_AVAILABE_CELL T3412exp.");

        if (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
        {
             /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                TAU类型应该填为combined TA/LA updating with IMSI attach */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
             /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
             NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }
        else
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_T3412EXP);
        }

        NAS_EMM_TAU_StartTAUREQ();
        return NAS_LMM_MSG_HANDLED;
    }

    /* 协议24301 5.3.5章节描述，如果在非REG+NO_AVAILABLE_CELL状态下，T3412超时，
       则当回到REG+NORMAL_SERVICE态后，应触发 Periodic TAU*/
    if(NAS_EMM_T3412_EXP_YES_OTHER_STATE ==  NAS_LMM_GetEmmInfoT3412ExpCtrl())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc:Other State T3412exp.");
        NAS_EMM_TAU_SaveEmmTAUStartCause( NAS_EMM_TAU_START_CAUSE_T3412EXP);
        NAS_EMM_TAU_StartTAUREQ();

        return NAS_LMM_MSG_HANDLED;
    }

    return NAS_LMM_MSG_DISCARD;
}




VOS_UINT32  NAS_EMM_RegSomeStateStartTAUNeeded( VOS_VOID )
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateStartTAUNeeded is entered.");

    /* 如果期间发生过ESM本地修改承载上下文*/
    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateStartTAUNeeded:Bearer Status Change.");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过UE_NET_CAP or DRX改变，触发TA-Updating TAU*/
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateStartTAUNeeded:DrxOrNetCap Change.");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /*对异系统标识进行判断*/
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoTriggerTauSysChange() )
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateStartTAUNeeded: Sys Change flag");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 如果当前注册域为PS，且当前有能力发起CS域的注册，则发起联合TAU */
    if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateStartTAUNeeded: with imsi attach");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 判断接入技术变化标识，如果变化 */
    if (NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateStartTAUNeeded: UE radio capability is changed !");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /*对RRC Rel原因进行判断*/
    if(NAS_EMM_YES == NAS_EMM_TAU_RrcRelCauseTriggerTau())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateStartTAUNeeded:rrc rel cause");
        return NAS_EMM_YES;
    }

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI"
       主要用于在L模下激活ISR，后续在L模下3412和3423定时器均超时的场景，此场景不能发起周期性TAU */
    if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateStartTAUNeeded: TIN is P-TMSI !");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 处理定时器T3412超时 */
    if (NAS_LMM_MSG_HANDLED == NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc())
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}

/* lihong00150010 emergency tau&service begin */

VOS_VOID  NAS_EMM_RegSomeStateMsgSysInfoTaiNotInTaiList( VOS_VOID )
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                      stCurTa           = {0};
    NAS_LMM_PTL_TI_ENUM_UINT16          enPtlTimerId     = NAS_LMM_PTL_TI_BUTT;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoTaiNotInTaiList is entered!");

    /* 获取当前TA和上次尝试注册的TA信息 */
    NAS_EMM_GetCurrentTa(&stCurTa);
    pstLastAttmpRegTa                  = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    /* TA没变&&有定时器&&EU值不是EU1，等超时; 否则立即发起TAU*/
    if( (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatch(pstLastAttmpRegTa, &stCurTa))
        && (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
        && (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat()))
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoTaiNotInTaiList: wait timer expired.");
        NAS_EMM_TAUSER_FSMTranState( EMM_MS_REG,
                                     EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                     TI_NAS_EMM_STATE_NO_TIMER);

        /* 有定时器3402在运行, 需上报TAU结果 */
        if ( TI_NAS_EMM_PTL_T3402 == enPtlTimerId )
        {
            NAS_EMM_SetTauTypeNoProcedure();

            ulTauRslt = MMC_LMM_TAU_RSLT_T3402_RUNNING;
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);


        }
        return;
    }

    /* 重置TAU计数，启动TAU Procedure */
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    NAS_EMM_TAU_StartTAUREQ();
    return;
}
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_EMM_RegSomeStateMsgSysInfoCommProc( VOS_VOID )
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_MM_NETWORK_ID_STRU             *pstPresentNetInfo = NAS_EMM_NULL_PTR;
    NAS_LMM_PTL_TI_ENUM_UINT16          enPtlTimerId     = NAS_LMM_PTL_TI_BUTT;

    /* 获取当前TA和上次尝试注册的TA信息 */
    pstPresentNetInfo                  = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegSomeStateMsgSysInfoCommProc is entered.");

    /* 如果当前TA不在TAI LIST */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        NAS_EMM_RegSomeStateMsgSysInfoTaiNotInTaiList();

        return ;
    }

    /* TA在TAI LIST && EU值不为EU1 */
    if (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoCommProc: TA is in TAI LIST, not EU1.");

        /* 上次尝试发起注册或TAU的TA不在TAI LIST，重置计数发起TAU */
        if (NAS_EMM_NO == NAS_LMM_IsLastAttemptRegTaInTaiList())
        {
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_EMM_TAU_StartTAUREQ();
            return;
        }

        /*有定时器 && 当前TA与上次尝试发起注册的TA等效，等超时*/
        if (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
        {
                NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoCommProc: Equivalent TA, wait timer expired.");
                NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                            EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            /* 有定时器是3402在运行, 需上报TAU结果 */
            if ( TI_NAS_EMM_PTL_T3402 == enPtlTimerId )
            {
                NAS_EMM_SetTauTypeNoProcedure();

                ulTauRslt = MMC_LMM_TAU_RSLT_T3402_RUNNING;
                NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

            }
            return;
        }

        /* 没有定时器 && 当前TA与上次尝试发起注册的TA等效，触发TAU但不重置尝试次数 */
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return;
    }

    /* TA在TAI LIST && EU值为EU1*/
    /* 如果有定时器，则判断该转入NORMAL SEVICE还是UPDATE MM态，等超时 */
    /* lihong00150010 emergency tau&service begin */
    if ((NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
        && (VOS_TRUE != NAS_EMM_IsEnterRegLimitService()))
    /* lihong00150010 emergency tau&service end */
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoCommProc: Check enter Nor-service or Upt-MM state.");

        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
            /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM状态 */
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                           TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* 将状态转移至MS_REG + SS_REG_NORMAL_SERVICE状态 */
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_NORMAL_SERVICE,
                                           TI_NAS_EMM_STATE_NO_TIMER);
        }

        NAS_EMM_SetTauTypeNoProcedure();

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();

        return ;
    }

    /* TA在TAI LIST && EU值为EU1*/
    /* 如果没有定时器 */
    /* 如果满足发起TAU的条件，直接发起，否则转NORMAL SERVICE */
    if (NAS_EMM_NO == NAS_EMM_RegSomeStateStartTAUNeeded())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegSomeStateMsgSysInfoCommProc: TAU not needed, return Nor-service.");

        /* TA在TAI List中，更新 L.V.R TAI*/
        NAS_EMM_SetLVRTai(pstPresentNetInfo);
		/* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }

        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /* 转换EMM状态机MS_REG+SS_NORMAL_SEARVICE*/
        /*NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);*/
        /* lihong00150010 emergency tau&service end */
        NAS_EMM_SetTauTypeNoProcedure();

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();
    }

    return;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_RegStateMmcOriResumeSysInfoWaitTimerExp3411or3402Running
 Description     : 收到系统消息时识别出要等待T3402超时的情况
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010    2014-01-23  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_RegStateMmcOriResumeSysInfoWaitTimerExp3411or3402Running( VOS_VOID )
{
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp: Check enter Nor-service or Upt-MM state.");

    if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
        /* lihong00150010 emergency tau&service end */

        /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM状态 */
        NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                       EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                       TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {   /* lihong00150010 emergency tau&service begin */
        if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
        {
            return NAS_EMM_FAIL;
        }
        /* lihong00150010 emergency tau&service end */
        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
            /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM状态 */
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                           TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* 将状态转移至MS_REG + SS_REG_NORMAL_SERVICE状态 */
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_NORMAL_SERVICE,
                                           TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    NAS_EMM_SetTauTypeNoProcedure();

    /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
    NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();

    return NAS_EMM_SUCC;
}
/* lihong00150010 emergency tau&service end */

/*****************************************************************************
 Function Name   : NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp
 Description     : 收到系统消息时识别出要等待T3402超时的情况
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.wangchen          2012-10-31  Draft Enact
    2.lihong00150010    2012-12-18  Modify:Emergency
*****************************************************************************/
VOS_UINT32  NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp( VOS_VOID )
{
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                      stCurTa           = {0};
    NAS_LMM_PTL_TI_ENUM_UINT16          enPtlTimerId     = NAS_LMM_PTL_TI_BUTT;
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;

    /* 获取当前TA和上次尝试注册的TA信息 */
    NAS_EMM_GetCurrentTa(&stCurTa);
    pstLastAttmpRegTa                  = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    /* 如果当前TA不在TAI LIST */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        /* TA没变&&有定时器&&EU值不是EU1，等超时; 否则立即发起TAU*/
        if( (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatch(pstLastAttmpRegTa, &stCurTa))
            && (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
            && (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat()))
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp: TA is not in TAI LIST, wait timer expired.");
            NAS_EMM_TAUSER_FSMTranState( EMM_MS_REG,
                                         EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                         TI_NAS_EMM_STATE_NO_TIMER);

            /* 有定时器3402在运行, 需上报TAU结果 */
            if ( TI_NAS_EMM_PTL_T3402 == enPtlTimerId )
            {
                NAS_EMM_SetTauTypeNoProcedure();

                ulTauRslt = MMC_LMM_TAU_RSLT_T3402_RUNNING;
                NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);


            }
            return NAS_EMM_SUCC;
        }
        return NAS_EMM_FAIL;

    }

    if (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp: TA is in TAI LIST, not EU1.");

        /*有定时器 && 当前TA与上次尝试发起注册的TA等效，等超时*/
        if ((NAS_EMM_YES == NAS_LMM_IsLastAttemptRegTaInTaiList())
            && (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId)))
        {
                NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp: Equivalent TA, wait timer expired.");
                NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                            EMM_SS_REG_ATTEMPTING_TO_UPDATE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            /* 有定时器3402在运行, 需上报TAU结果 */
            if ( TI_NAS_EMM_PTL_T3402 == enPtlTimerId )
            {
                NAS_EMM_SetTauTypeNoProcedure();

                ulTauRslt = MMC_LMM_TAU_RSLT_T3402_RUNNING;
                NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

            }
            return NAS_EMM_SUCC;
        }
        return NAS_EMM_FAIL;
    }
    /* TA在TAI LIST && EU值为EU1*/

    /* 如果有定时器，则判断该转入NORMAL SEVICE还是UPDATE MM态，等超时 */
    if (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
    {
        /* lihong00150010 emergency tau&service begin */
        return NAS_EMM_RegStateMmcOriResumeSysInfoWaitTimerExp3411or3402Running();
        /* lihong00150010 emergency tau&service end */
    }

    return NAS_EMM_FAIL;
}


/*****************************************************************************
 Function Name   : NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc
 Description     : 注册态下收到系统消息,不需要等T3411或者T3402超时的公共处理
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.wangchen          2012-10-31  Draft Enact
    2.lihong00150010    2012-12-18  Modify:Emergency
*****************************************************************************/
VOS_VOID  NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc( VOS_VOID )
{
    NAS_MM_NETWORK_ID_STRU             *pstPresentNetInfo = NAS_EMM_NULL_PTR;
    GMM_LMM_GPRS_MM_STATE_ENUM_UINT32   enGprsMmState   = GMM_LMM_GPRS_MM_STATE_BUTT;


    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc is entered.");

    /*c)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI"*/
    if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc:TIN is p-tmsi");

        if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
            && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc:combined succ");

            /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                TAU类型应该填为combined TA/LA updating with IMSI attach */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        }
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
     and the UE previously either performed a location area update procedure
     or a combined routing area update procedure in A/Gb or Iu mode,
     in order to re-establish the SGs association. In this case the EPS update
     type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if ((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc:FIRST TAU");

        /*发起TAU流程*/
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();

        return;
    }
    /* 获取G模连接状态 */
    enGprsMmState = NAS_LMM_GetEmmInfoGConnState();

    /* l)when the UE reselects an E-UTRAN cell while it was in GPRS READY state or PMM-CONNECTED mode; */
    if (GMM_LMM_GPRS_MM_STATE_READY  == enGprsMmState)
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc:GPRS READY init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return ;
    }

    /* 触发TAU */
    if ((GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc:EPS service suspended init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return ;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == NAS_EMM_GetResumeDir())
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:ISR act and P.4 init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /* 如果当前TA不在TAI LIST */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        /* 重置TAU计数，启动TAU Procedure */
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return;

    }

    /* TA在TAI LIST && EU值不为EU1 */
    if (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc: TA is in TAI LIST, not EU1.");

        /* 上次尝试发起注册或TAU的TA不在TAI LIST，重置计数发起TAU */
        if (NAS_EMM_NO == NAS_LMM_IsLastAttemptRegTaInTaiList())
        {
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_EMM_TAU_StartTAUREQ();
            return;
        }

        /* 没有定时器 && 当前TA与上次尝试发起注册的TA等效，触发TAU但不重置尝试次数 */
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return;
    }


    /* TA在TAI LIST && EU值为EU1*/
    /* 如果没有定时器 */
    /* 如果满足发起TAU的条件，直接发起，否则转NORMAL SERVICE */
    if (NAS_EMM_NO == NAS_EMM_RegSomeStateStartTAUNeeded())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc: TAU not needed, return Nor-service.");

        /* TA在TAI List中，更新 L.V.R TAI*/
        NAS_EMM_SetLVRTai(pstPresentNetInfo);
        /* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }

        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();

        /* 转换EMM状态机MS_REG+SS_NORMAL_SEARVICE*/
        /*NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);*/
        /* lihong00150010 emergency tau&service end */

        NAS_EMM_SetTauTypeNoProcedure();

        NAS_EMM_ProcHoWaitSysInfoBufferMsg();

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();

        return;
    }

    (VOS_VOID)NAS_EMM_ClrHoWaitSysInfoBufferMsg();

    return;
}



VOS_VOID  NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc( VOS_VOID)
{
    NAS_MM_NETWORK_ID_STRU             *pstPresentNetInfo = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                      stCurTa           = {0};
    NAS_LMM_PTL_TI_ENUM_UINT16          enPtlTimerId     = NAS_LMM_PTL_TI_BUTT;
    VOS_UINT32                          ulRslt = NAS_EMM_FAIL;

    /* 获取当前TA信息 */
    NAS_EMM_GetCurrentTa(&stCurTa);
    pstPresentNetInfo                  = NAS_LMM_GetEmmInfoNetInfoPresentNetAddr();

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc is entered.");

    /* 如果当前TA不在TAI LIST */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        /* 重置TAU计数，启动TAU Procedure */
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        ulRslt = NAS_EMM_ClrHoWaitSysInfoBufferMsg();
        if (NAS_EMM_SUCC == ulRslt)
        {
            NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
        NAS_EMM_TAU_StartTAUREQ();
        return;

    }
     /* TA在TAI LIST */
    /* 如果有定时器，等超时 */
    /* lihong00150010 emergency tau&service begin */
    if ((NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
        && (VOS_TRUE != NAS_EMM_IsEnterRegLimitService()))
    /* lihong00150010 emergency tau&service end */
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc: Check enter Nor-service or Upt-MM state.");

        NAS_EMM_ProcHoWaitSysInfoBufferMsg();

        return ;
    }

    /* TA在TAI LIST && 没有定时器  */
    /* 如果满足发起TAU的条件，直接发起，否则状态不变(UPDATE MM态一定有定时器) */
    if (NAS_EMM_NO == NAS_EMM_RegSomeStateStartTAUNeeded())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc: TAU not needed, return Nor-service.");

        /* TA在TAI List中，更新 L.V.R TAI*/
        NAS_EMM_SetLVRTai(pstPresentNetInfo);

        /* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }

        if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
        {
            NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                            EMM_SS_REG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        }
        /* lihong00150010 emergency tau&service end */

        NAS_EMM_ProcHoWaitSysInfoBufferMsg();

        return;
    }

    (VOS_VOID)NAS_EMM_ClrHoWaitSysInfoBufferMsg();

    return;

}
VOS_VOID  NAS_EMM_TAU_StartTauForInterRat( VOS_VOID )
{
    if (NAS_LMM_SYS_CHNG_TYPE_HO != NAS_EMM_GetResumeType())
    {
        /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
        NAS_EMM_SecuResumeIdle();
    }

    /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
    NAS_EMM_TAU_StartTAUREQ();

    return;
}

/* lihong00150010 emergency tau&service begin */
#if 0
VOS_VOID  NAS_EMM_TAU_SetT3412Exp
(
    NAS_EMM_T3412_EXP_ENUM_UINT32       enEmmT3412ExpCtrl
)
{
    NAS_LMM_SetEmmInfoT3412ExpCtrl(enEmmT3412ExpCtrl);
}


VOS_UINT32  NAS_EMM_QueryEmergencyBear(VOS_VOID)
{

    return NAS_LMM_FAIL;
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_LMM_PreProcMsgSrvccNofify(MsgBlock *    pMsg )
{
    MM_LMM_SRVCC_STATUS_NOTIFY_STRU            *pstSrvccNotify;

    pstSrvccNotify = (VOS_VOID*)pMsg;

    if (MM_LMM_SRVCC_STATUS_SUCC == pstSrvccNotify->enSrvccStatus)
    {
        NAS_LMM_SetEmmInfoSrvccFlag(NAS_EMM_SRVCC_HAPPENED);

    }
    else
    {
        NAS_LMM_SetEmmInfoSrvccFlag(NAS_EMM_SRVCC_NOT_HAPPENED);
    }
    return NAS_LMM_MSG_HANDLED;
}


/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_EmcPndReqTauAbnormalCommProc
 Description     : 紧急PDN连接建立引起的TAU异常公共处理
 Input           : pfTauRslt----------------TAU结果上报函数指针
                   pvPara-------------------TAU结果上报函数的入参
                   ucSs---------------------子状态
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-12-31  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_EmcPndReqTauAbnormalCommProc
(
    NAS_LMM_SEND_TAU_RESULT_ACT_FUN     pfTauRslt,
    const VOS_VOID                     *pvPara,
    NAS_EMM_SUB_STATE_ENUM_UINT16       ucSs
)
{
    /* 上报TAU结果 */
    NAS_EMM_TAU_SEND_MMC_RESULT_IND(pfTauRslt, pvPara);

    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
    NAS_EMM_MmcSendDetIndLocal( MMC_LMM_L_LOCAL_DETACH_OTHERS);

    /* 记录ATTACH触发原因值 */
    NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                ucSs,
                                TI_NAS_EMM_STATE_NO_TIMER);
}
/* lihong00150010 emergency tau&service end */

/*lint +e961*/
/*lint +e960*/
/*lint +e831*/
/*lint +e669*/
/*lint +e589*/



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



