


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasMmlCtx.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMDETACHCNMSGPROC_C
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

VOS_UINT32    NAS_EMM_CnDetachAcpMoMsgChk(                /*入参暂时没有使用, 使用LINT的语法规避*/
                                   const NAS_EMM_CN_DETACH_ACP_MO_STRU   *pMsgMsg )
{

    /*消息内容检查*/

    (VOS_VOID)pMsgMsg;
    return  NAS_EMM_PARA_VALID;
}


VOS_UINT32    NAS_EMM_CnDetachReqMtMsgChk(
                                    const NAS_EMM_CN_DETACH_REQ_MT_STRU   *pMsgMsg )
{

    /*消息内容检查*/
    if ((NAS_EMM_DETACH_TYPE_MT_REATTACH == pMsgMsg->ucDetType)
        || (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pMsgMsg->ucDetType)
        || (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pMsgMsg->ucDetType))
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
VOS_VOID NAS_EMM_MsRegSsNmlSrvMsgCnImsiDetachReq()
{
    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
     /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 如果UE模式为CS/PS，允许CS域注册，没有被原因值2拒绝过，则发起
       "combined TA/LA updating with IMSI attach"类型的联合TAU */
    if ((NAS_EMM_YES == NAS_EMM_IsCsPsUeMode())
        && (NAS_EMM_AUTO_ATTACH_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
        && (NAS_EMM_REJ_NO == NAS_LMM_GetEmmInfoRejCause2Flag())
        && (NAS_EMM_REJ_NO == NAS_EMMC_GetRejCause18Flag()))
    {
        /* 设置TAU START CAUSE 为 CN IMSI DETACH */
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_CN_IMSI_DETACH);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

        NAS_EMM_TAU_SetImsiDetachFlag(NAS_EMM_IMSI_DETACH_VALID);

        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        NAS_EMM_TAU_StartTAUREQ();
    }
    /*PC REPLAY MODIFY BY LEILI END*/
}


VOS_VOID    NAS_EMM_DetDataUpdate(VOS_UINT32 ulDeleteRplmn, VOS_UINT8 ucCnCause)
{
    NAS_MM_TA_STRU                      stTa;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    /*设置update status为EU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(ulDeleteRplmn);

    if ((NAS_LMM_CAUSE_ROAM_NOT_ALLOW == ucCnCause) ||
        (NAS_LMM_CAUSE_NO_SUITABL_CELL == ucCnCause))
    {
        NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());
        NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);
    }
    else if (NAS_LMM_CAUSE_TA_NOT_ALLOW == ucCnCause)
    {
        NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr());

        /* 增加REJ cause 12标记,用于向MMC上报有区域限制的限制服务状态 2011-07-27*/
        NAS_LMM_SetNasRejCause12Flag(NAS_LMM_REJ_CAUSE_IS_12);
    }
    else
    {
        ;
    }
    return;
}


VOS_VOID  NAS_EMM_ProcDetCauseVal2( VOS_VOID )
{
    /* 无法直接进入DEREG+NORMAL_SERVICE态，因为此状态无法发送上行信令，
       会导致DETACH ACCPET消息无法发送；不能马上直接给RRC发链路释放，这样
       不能保证DETACH ACCPET能够正常发送，因为LMM先给RRC发了链路释放请求，
       后给RRC发送上行直传消息 */

    /* 如果CN DETACH，reattach not required类型，原因值为2，
       则L模后续不能发起联合ATTACH和联合TAU */
    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

    NAS_EMM_GLO_AD_GetDetAtmpCnt()  = 0;

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;
}
VOS_VOID    NAS_EMM_ProcDetCauseVal3678(VOS_UINT8 ucDetCause)
{
    NAS_EMM_DetDataUpdate(NAS_EMM_DELETE_RPLMN,ucDetCause);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
    return;
}


VOS_VOID    NAS_EMM_ProcDetCauseVal11(VOS_UINT8 ucDetCause)
{
    /*reset attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;

    NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN,ucDetCause);

    /*为上报服务状态,进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}


VOS_VOID    NAS_EMM_ProcDetCauseVal12(VOS_UINT8 ucDetCause)
{
    /*重设attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;

    NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN, ucDetCause);

    /*为上报服务状态,进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}


VOS_VOID    NAS_EMM_ProcDetCauseVal13(VOS_UINT8 ucDetCause)
{

    /*重设attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;

    NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN,ucDetCause);

    /*为上报服务状态,进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}



VOS_VOID    NAS_EMM_ProcDetCauseVal14(VOS_UINT8 ucDetCause)
{

    /*重设attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;

    NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN,ucDetCause);

    /*为上报服务状态,进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}


VOS_VOID    NAS_EMM_ProcDetCauseVal15(VOS_UINT8 ucDetCause)
{

    /*重设attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;

    NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN,ucDetCause);

    /*为上报服务状态,进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);


    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}
#if 0
/*****************************************************************************
 Function Name   : NAS_EMM_ProcDetCauseVal25
 Description     : 对原因值#25处理
                   1)set the EPS update status to EU3 ROAMING NOT ALLOWED
                   2)reset the attach attempt counter and shall enter the
                     state EMM-DEREGISTERED.LIMITED-SERVICE
                   3)remove the CSG ID of the cell where the UE has received
                     the DETACH REQUEST message from the Allowed CSG list
                   4)search for a suitable cell in the same PLMN
 Input           :
 Return          :
 History         :
    1.qilili00145085         2009-02-02  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_ProcDetCauseVal25()
{

    /*设置update status为EU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*重设attach attempt counter*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

    /*Remove the CSG ID from Allowed CSG List*/
    /*暂不实现*/

   /*启动定时器T3440*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_T3440);

    /*保存释放连接的原因*/
    NAS_EMM_GLO_AD_GetDetRelCau() = EMM_CN_DETACH_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG;

    /*转换状态到:EMM_DEREG.WT_RRC_REL_IND*/
    NAS_EMM_AdStateConvert( EMM_MS_DEREG_INIT,
                            EMM_SS_DETACH_WAIT_RRC_REL_IND,
                            TI_NAS_EMM_STATE_T3440);
}
#endif
VOS_VOID    NAS_EMM_ProcDetNoCauseOrOtherCause(VOS_VOID )
{
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_ProcDetNoCauseOrOtherCause enter");

    NAS_EMM_RrcRelCauseCnDetachSrcClrAttemptToAtt();

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();
}
VOS_VOID    NAS_EMM_DetCauseValProc(NAS_EMM_CN_CAUSE_ENUM_UINT8 ucDetCause)
{
    /*根据不同原因值进行处理*/
    switch (ucDetCause)
    {
        case NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS:
            NAS_EMM_ProcDetCauseVal2();
            break;

        case NAS_LMM_CAUSE_ILLEGAL_UE:
        case NAS_LMM_CAUSE_ILLEGAL_ME:
        case NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW:
        case NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW:

            NAS_EMM_ProcDetCauseVal3678(ucDetCause);
            break;

        case NAS_LMM_CAUSE_PLMN_NOT_ALLOW:
            NAS_EMM_ProcDetCauseVal11(ucDetCause);
            break;

        case NAS_LMM_CAUSE_TA_NOT_ALLOW:
            NAS_EMM_ProcDetCauseVal12(ucDetCause);
            break;

        case NAS_LMM_CAUSE_ROAM_NOT_ALLOW:
            NAS_EMM_ProcDetCauseVal13(ucDetCause);
            break;

        case NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN:
            NAS_EMM_ProcDetCauseVal14(ucDetCause);
            break;

        case NAS_LMM_CAUSE_NO_SUITABL_CELL:
            NAS_EMM_ProcDetCauseVal15(ucDetCause);
            break;

/* CSG功能尚未实现，收到REJ #25按非CSG处理，进入default处理分支*/
#if 0
        case NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG:
            NAS_EMM_ProcDetCauseVal25();
            break;
#endif
        default:
            NAS_EMM_ATTACH_LOG1_INFO(   "DETACH REQUEST-- OtherCause: ",
                                        ucDetCause);

            NAS_EMM_ProcDetNoCauseOrOtherCause();
            break;

    }
}
VOS_VOID    NAS_EMM_AppSendDetIndMt
(
   const  NAS_EMM_CN_DETACH_REQ_MT_STRU   *pRcvEmmMsg
)
{
    LMM_MMC_DETACH_IND_STRU              *pDetIndMsg;

    if(NAS_EMM_NULL_PTR == pRcvEmmMsg)
    {
        /*打印错误*/
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_AppSendDetIndMt: Input null!");
        return;
    }

    /*申请消息内存*/
    pDetIndMsg= (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_DETACH_IND_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pDetIndMsg)
    {
        /*打印错误*/
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_AppSendDetIndMt: MSG ALLOC ERR!");
        return;
    }

    /*清空消息空间*/
    NAS_LMM_MEM_SET(pDetIndMsg, 0, sizeof(LMM_MMC_DETACH_IND_STRU));


    /*构造LMM_MMC_DETACH_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pDetIndMsg,
                                        sizeof(LMM_MMC_DETACH_IND_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /*填充消息ID*/
    pDetIndMsg->ulMsgId                 = ID_LMM_MMC_DETACH_IND;

    /*填充消息内容*/
    pDetIndMsg->ulOpId                  = NAS_EMM_OPID_TYPE_DETACH_IND;
    pDetIndMsg->ulDetachType            = MMC_LMM_DETACH_MT;

    pDetIndMsg->bitOpCnReqType          = NAS_EMM_AD_BIT_SLCT;
    pDetIndMsg->ulCnReqType             = pRcvEmmMsg->ucDetType;

    if (NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
    {
        NAS_EMM_DETACH_LOG1_NORM("NAS_EMM_AppSendDetIndMt:detach cause =",
                                pRcvEmmMsg->ucEmmCause);
        pDetIndMsg->bitOpCnCause    = NAS_EMM_AD_BIT_SLCT;
        pDetIndMsg->ucCnCause       = pRcvEmmMsg->ucEmmCause;
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(                   pDetIndMsg);

    return;
}



VOS_VOID  NAS_EMM_MmcSendDetIndLocal
(
    MMC_LMM_L_LOCAL_DETACH_TYPE_ENUM_UINT32 ulLocDetType
)
{

    LMM_MMC_DETACH_IND_STRU              *pDetIndMsg;

    /*申请消息内存*/
    pDetIndMsg= (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_DETACH_IND_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pDetIndMsg)
    {
        /*打印错误*/
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MmcSendDetIndLocal: MSG ALLOC ERR!");
        return;
    }

    /*清空消息空间*/
    NAS_LMM_MEM_SET(pDetIndMsg, 0, sizeof(LMM_MMC_DETACH_IND_STRU));



    /*构造LMM_MMC_DETACH_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_MMC_PS_MSG_HEADER(  pDetIndMsg,
                                        sizeof(LMM_MMC_DETACH_IND_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /*填充消息ID*/
    pDetIndMsg->ulMsgId                 = ID_LMM_MMC_DETACH_IND;

    /*填充消息内容*/
    pDetIndMsg->ulOpId                  = NAS_EMM_OPID_TYPE_DETACH_IND;
    pDetIndMsg->ulDetachType            = MMC_LMM_DETACH_LOCAL;
    pDetIndMsg->bitOpLocDetType         = NAS_EMM_AD_BIT_SLCT;
    pDetIndMsg->ulLocDetType            = ulLocDetType;

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_LMM_SendLmmMmcMsg(                   pDetIndMsg);

}
VOS_VOID    NAS_EMM_CompCnDetachAcpMt(NAS_EMM_MRRC_DATA_REQ_STRU *pDetAccMtMsg)
{
    VOS_UINT32                          ulIndex             = 0;
    NAS_EMM_MRRC_DATA_REQ_STRU         *pTempDetAccMtMsg      = pDetAccMtMsg;
    VOS_UINT32                          ulDataReqLenNoHeader;


    if (NAS_EMM_NULL_PTR == pDetAccMtMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_CompCnDetachReqMo: Mem Alloc Err!");
        return;
    }

    /* 填充Protocol Discriminator + Security header type */
    pTempDetAccMtMsg->stNasMsg.aucNasMsg[ulIndex++]  = EMM_CN_MSG_PD_EMM;

    /* 填充 Detach accept message identity   */
    pTempDetAccMtMsg->stNasMsg.aucNasMsg[ulIndex++]  = NAS_EMM_CN_MT_DETACH_ACP_MO;

    /*内部消息长度计算*/
    ulDataReqLenNoHeader             = NAS_EMM_CountMrrcDataReqLen(ulIndex);

    /*填充消息长度*/
    pTempDetAccMtMsg->stNasMsg.ulNasMsgSize    = ulIndex;

    /*填充消息头*/
    NAS_EMM_COMP_AD_INTRA_MSG_HEADER(pTempDetAccMtMsg, ulDataReqLenNoHeader);

    /*填充消息ID*/
    pTempDetAccMtMsg->ulMsgId                  = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;

    /*填充消息内容*/
    pTempDetAccMtMsg->enEstCaue                = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
    pTempDetAccMtMsg->enCallType               = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    pTempDetAccMtMsg->enEmmMsgType             = NAS_EMM_MSG_DETACH_ACP;

    pTempDetAccMtMsg->enDataCnf                = LRRC_LMM_DATA_CNF_NEED;

    return;
}


VOS_VOID    NAS_EMM_MrrcSendDetAcpMt()
{
    NAS_EMM_MRRC_DATA_REQ_STRU          *pIntraMsg;
    NAS_MSG_STRU                        *pstNasMsg;


    /*以最小消息长度，申请消息内存,主要是看是否队列有空间*/
    pIntraMsg = (VOS_VOID *) NAS_LMM_MEM_ALLOC( NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraMsg)
    {
        /*打印错误*/
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MrrcSendDetAcpMt: MSG ALLOC ERR!");
        return;

    }

    /*构造发向网侧的DETACH ACCEPT消息*/
    NAS_EMM_CompCnDetachAcpMt(pIntraMsg);


    /*向OM发送空口消息DETACH REQUEST*/
    pstNasMsg = (NAS_MSG_STRU *)(&(pIntraMsg->stNasMsg));
    NAS_LMM_SendOmtAirMsg(NAS_EMM_OMT_AIR_MSG_UP, NAS_EMM_DETACH_ACP_MT, pstNasMsg);
    NAS_LMM_SendOmtKeyEvent(             EMM_OMT_KE_DETACH_ACP_MT);

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_SndUplinkNasMsg(            pIntraMsg);

    NAS_LMM_MEM_FREE(pIntraMsg);

    return;

}


VOS_VOID    NAS_EMM_DetCnDetReqNotReattach(
                               const NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg)
{
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_DetCnDetReqNotReattach: is entered");

    if(NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
    {
        /*判断原因值,并根据原因值进行不同处理*/
        NAS_EMM_DetCauseValProc(        pRcvEmmMsg->ucEmmCause);
    }
    else
    {   /* 若CN消息中没有填写原因 */

        /* 若CN消息中没有填写原因,同原因值为"OTHER"的处理 */
        NAS_EMM_ProcDetNoCauseOrOtherCause();
    }

}
VOS_VOID  NAS_EMM_ProcNoUsimCnDetReqNotReattach(
                               const NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg)
{
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_ProcNoUsimCnDetReqNotReattach: is entered");

    if(NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
    {
        switch(pRcvEmmMsg->ucEmmCause)
        {
            case NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS:

                NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

                NAS_EMM_GLO_AD_GetDetAtmpCnt()  = 0;
                break;
            case NAS_LMM_CAUSE_ILLEGAL_UE:
            case NAS_LMM_CAUSE_ILLEGAL_ME:
            case NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW:
            case NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW:
                NAS_EMM_DetDataUpdate(NAS_EMM_DELETE_RPLMN,pRcvEmmMsg->ucEmmCause);
                break;

            case NAS_LMM_CAUSE_PLMN_NOT_ALLOW:
            case NAS_LMM_CAUSE_TA_NOT_ALLOW:
            case NAS_LMM_CAUSE_ROAM_NOT_ALLOW:
            case NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN:
            case NAS_LMM_CAUSE_NO_SUITABL_CELL:
                 /*reset attach attempt counter*/
                NAS_EMM_GLO_AD_GetAttAtmpCnt()           = 0;
                NAS_EMM_DetDataUpdate(NAS_EMM_NOT_DELETE_RPLMN,pRcvEmmMsg->ucEmmCause);
                break;
            default:
                NAS_EMM_RrcRelCauseCnDetachSrcClrAttemptToAtt();
                break;

        }
    }
    else
    {
        NAS_EMM_RrcRelCauseCnDetachSrcClrAttemptToAtt();
    }
    return;
}
VOS_UINT32  NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq(VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru )
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: NAS_EMM_CN_DETACH_REQ_MT_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process Imsi Detach!");
        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        NAS_EMM_MsRegSsNmlSrvMsgCnImsiDetachReq();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached required*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach!");

        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

        /*状态迁移,等待收到系统消息后发起ATTACH流程*/
        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        /* 判断是否为原因值2 */
        if((NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
            && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
        {
            NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach not required,cause2!");

            /* 设置注册域为PS */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

            /* 如果CN DETACH，reattach not required类型，原因值为2，
               则L模后续不能发起联合ATTACH和联合TAU */
            NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);
        }
        else
        {
            NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach not required,cause other!");
            NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
            NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
        }
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    return  NAS_LMM_MSG_HANDLED;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsLimitedSrvMsgCnDetachReq
 Description     : 主状态REG+子状态REG_LIMIT_SERVICE下收到网侧发送的
                   DETACH REQUEST消息

 Input           :

 Return          :

 History         :
    1.lihong00150010         2012-12-31     Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgCnDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsLimitedSrvMsgCnDetachReq is entered");

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegSsLimitedSrvMsgCnDetachReq: NAS_EMM_CN_DETACH_REQ_MT_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsLimitedSrvMsgCnDetachReq: Process Imsi Detach!");

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /* 设置注册域为PS */
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached required*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach!");

        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

        /*状态迁移,等待收到系统消息后发起ATTACH流程*/
        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_PLMN_SEARCH,
                                TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        /* 判断是否为原因值2 */
        if((NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
            && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
        {
            NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach not required,cause2!");

            /* 设置注册域为PS */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

            /* 如果CN DETACH，reattach not required类型，原因值为2，
               则L模后续不能发起联合ATTACH和联合TAU */
            NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);
        }
        else
        {
            NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgCnDetachReq: Process reattach not required,cause other!");
            NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
            NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
        }
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    return  NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq: NAS_EMM_CN_DETACH_REQ_MT_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /* 丢弃IMSI DETACH类型的CN DETACH */
    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq:Discard Imsi Detach");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 丢弃re-attach not required类型，原因值为#2的CN DETACH */
    if ((NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
        && (NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
        && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgCnDetachReq:Discard cause2 detach");
        return  NAS_LMM_MSG_DISCARD;
    }

    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

    /*detach type == re-attached required*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    return  NAS_LMM_MSG_HANDLED;
}
/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-08-24  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_LMM_INTRA_DETACH_REQ_STRU      *pRcvEmmMsg = NAS_EMM_NULL_PTR;

    pRcvEmmMsg                          = (NAS_LMM_INTRA_DETACH_REQ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG1_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq is entered", ulMsgId);

    (VOS_VOID)ulMsgId;

     /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_IntraDetReqChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq: NAS_EMM_IntraDetReqChk err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }
    /* 如果是IMSI DETACH类型，则本地去注册CS域，停止定时器3411和3402，转到REG+NORMAL_SERVICE态 */
    if (MMC_LMM_MO_DET_CS_ONLY == NAS_EMM_GLO_AD_GetDetTypeMo())
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq:Proceed Imsi Detach");

        /* 设置注册域为PS */
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        /* 如果3411在运行，需要给MMC报TAU结果，以让MMC退出搜网状态机；3402定时器在
           运行无需通知，MMC收到TAU结果尝试次数为5时，已退出搜网状态机 */
        if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
        {
            NAS_EMM_SetTauTypeNoProcedure();
            NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();
        }

        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

        /* IMSI DETACH后释放资源 */
        NAS_LMM_ImsiDetachReleaseResource();

        /*停止定时器*/
        NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3411);
        NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3402);

        /*修改状态：进入主状态EMM_MS_REG子状态EMM_SS_REG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgIntraDetReq:Proceed EPS Detach or EPS/IMSI Detach");

    /* 如果3411在运行，需要给MMC报TAU结果，以让MMC退出搜网状态机；3402定时器在
       运行无需通知，MMC收到TAU结果尝试次数为5时，已退出搜网状态机 */
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_TAU_SaveTAUtype(NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI);

        ulTauRslt = MMC_LMM_TAU_RSLT_MO_DETACH_FAILURE;
        NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

    }

    /*停止定时器*/
    NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopPtlTimer(       TI_NAS_EMM_PTL_T3402);

    /*发送DETACH REQUEST消息*/
    NAS_EMM_SendDetachReqMo();

   return   NAS_LMM_MSG_HANDLED;
}


VOS_VOID  NAS_EMM_ProcNoUsimOfDrgInitMsgCnDetachReq( VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_ProcNoUsimCnDetReqNotReattach(pRcvEmmMsg);
    }

    /*NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);*/
    NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                            EMM_SS_DEREG_NO_IMSI,
                            TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    /* 给MMC回复LMM_MMC_USIM_STATUS_CNF */
    NAS_EMM_SendMmcUsimStatusCnf();

    /* lihong00150010 emergency tau&service begin */
    /* 清除拔卡标识 */
    NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_FALSE;
    /* lihong00150010 emergency tau&service end */

    return;
}
VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq(VOS_UINT32  ulMsgId,
                                                        VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;


    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq is entered");

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*check the msg content*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq: NAS_EMM_CN_DETACH_REQ_MT_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }
    if (VOS_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_DETACH_REQ))
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq: Send LMM_MMC_DETACH_CNF");
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {
        /* 本地DETACH释放资源:动态内存、赋初值 */
        NAS_LMM_DeregReleaseResource();
        NAS_EMM_ProcNoUsimOfDrgInitMsgCnDetachReq((VOS_VOID*)pRcvEmmMsg);

        return NAS_LMM_MSG_HANDLED;
    }

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /* lihong00150010 emergency tau&service end */
    /* 如果是IMSI类型的CN DETACH，则给RRC发送RRC_MM_REL_REQ消息，并更新连接状态； */
    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq:Process Imsi Detach");

        /* 无法直接进入DEREG+NORMAL_SERVICE态，因为此状态无法发送上行信令，
           会导致DETACH ACCPET消息无法发送；不能马上直接给RRC发链路释放，这样
           不能保证DETACH ACCPET能够正常发送，因为LMM先给RRC发了链路释放请求，
           后给RRC发送上行直传消息 */
        NAS_EMM_GLO_AD_GetDetAtmpCnt()  = 0;

        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();

        return  NAS_LMM_MSG_HANDLED;
    }
    /*detach type == re-attached required,will not attach for inside UE detach*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq: Cn Detach Reattach Request!");

        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        /* 实现GCF对App和网侧Detach冲突的处理 */
        NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachReq: Cn Detach Not Reattach!");

        NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);

    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);
    return  NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq is entered",
                                                    ulMsgId,
                                                    pMsgStru);

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*check the msg content*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq: NAS_EMM_CN_DETACH_REQ_MT_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    if (VOS_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_DETACH_REQ))
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq: Send LMM_MMC_DETACH_CNF");
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3421);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*修改状态：进入主状态EMM_MS_REG子状态EMM_SS_REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    /*给网络回复DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /* 如果是IMSI类型的CN DETACH，则给RRC发送RRC_MM_REL_REQ消息，并更新连接状态； */
    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq:Process Imsi Detach");

        if (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState())
        {

            /*向MMC发送LMM_MMC_DETACH_IND消息*/
            NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

            /*等待RRC_REL_IND*/
            NAS_EMM_WaitNetworkRelInd();

            return  NAS_LMM_MSG_HANDLED;
        }
    }

    /*detach type == re-attached required,will not attach for inside UE detach*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq: Cn Detach Reattach Request!");

        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;
    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachReq: Cn Detach Not Reattach!");

        /* 判断是否为原因值2 */
        if((NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
            && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
        {
            /* 如果CN DETACH，reattach not required类型，原因值为2，
               则L模后续不能发起联合ATTACH和联合TAU */
            NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

            if (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState())
            {
                /*等待RRC_REL_IND*/
                NAS_EMM_WaitNetworkRelInd();
            }
        }
        else
        {
            NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
            NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
        }
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegInitMsgCnDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegInitMsgCnDetachReq is entered");

    /* 判断主状态是否为REG_INIT态 */
    if (EMM_MS_REG_INIT != NAS_EMM_CUR_MAIN_STAT)
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegInitMsgCnDetachReq: STATE ERROR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegInitMsgCnDetachReq: MMC_LMM_DETACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /* 丢弃IMSI DETACH类型的CN DETACH */
    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegInitMsgCnDetachReq:Discard Imsi Detach");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*detach type == re-attach required*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        /*忽略此消息，继续ATTACH过程，等待网侧响应消息*/
        NAS_EMM_DETACH_LOG_NORM("DETACH REQUEST MESSAGE IS IGNORE");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 停止所有ATTACH定时器 */
    NAS_LMM_StopAllAttachTimer();

    /*detach type == re-attach not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        if(EMM_SS_ATTACH_WAIT_ESM_PDN_RSP == NAS_EMM_CUR_SUB_STAT)
        {
            /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
            NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_MT_DETACH_FAILURE);
        }
        else
        {
            /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
            NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_MT_DETACH_FAILURE);
        }

        NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

        /*向MRRC发送DETACH ACCEPT消息*/
        NAS_EMM_MrrcSendDetAcpMt();

        /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
        NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

        NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        /*cn detach上报结果顺序不会影响MMC的处理，因此不调整*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

    }

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32    NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq(VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_DETACH_REQ_MT_STRU      *pRcvEmmMsg;
    MMC_LMM_TAU_RSLT_ENUM_UINT32        enTauRslt = MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *)pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq: MMC_LMM_DETACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    /* 丢弃IMSI DETACH类型的CN DETACH */
    if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq:Discard Imsi Detach");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 丢弃re-attach not required类型，原因值为#2的CN DETACH */
    if ((NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
        && (NAS_EMM_AD_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
        && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsTauInitSsWtCnTauCnfMsgCnDetatchReq:Discard cause2 detach");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*调用TAU模块提供的函数*/
    NAS_EMM_TAU_AbnormalOver();

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case NAS_EMM_COLLISION_SERVICE:

            /* 向MMC上报TAU结果在冲突处理中执行 */
            NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndOthertype,
                                      (VOS_VOID*)&enTauRslt,
                                      NAS_EMM_TRANSFER_RAT_NOT_ALLOW);

            /* 如果是紧急CSFB且是re-attach，则不再发起ATTACH直接主动释放 */
            if((NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
                && (VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb()))
            {
                NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);

                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NORMAL_SERVICE,
                                       TI_NAS_EMM_STATE_NO_TIMER);

                /*等待RRC_REL_IND*/
                NAS_EMM_WaitNetworkRelInd();

                /*向MMC发送LMM_MMC_DETACH_IND消息*/
                NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);
                return NAS_LMM_MSG_HANDLED;
            }
            break;

        case NAS_EMM_COLLISION_DETACH:

            /* 向MMC发被 MO DETACH 打断 LMM_MMC_TAU_RESULT_IND*/
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);

            if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
            {
                /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
                NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                            EMM_SS_DEREG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
                NAS_EMM_TAU_CollisionDetachProc();

                /*本地DETACH释放资源:动态内存、赋初值 */
                NAS_LMM_DeregReleaseResource();
            }
            else
            {
                /*修改状态：进入主状态EMM_MS_REG子状态EMM_SS_REG_NORMAL_SERVICE*/
                NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
                NAS_EMM_TAU_CollisionDetachProc();

                /* 设置注册域为PS */
                NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

                /* IMSI DETACH后释放资源 */
                NAS_LMM_ImsiDetachReleaseResource();
            }
            break;

        default:

            /* 向MMC发被 MO DETACH 打断 LMM_MMC_TAU_RESULT_IND*/
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);
            break;

    }

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

    /*detach type == re-attached required*/
    if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);

        /*向MMC发送LMM_MMC_DETACH_IND消息*/
        NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
        return  NAS_LMM_MSG_HANDLED;

    }

    /*detach type == re-attached not required*/
    if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
    {
        NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
    }

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

   return  NAS_LMM_MSG_HANDLED;
}

VOS_VOID NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnImsiDetatchReq
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /*如果SR流程是由于SMS触发，由于CS域没有注册，需要回复SMS建链失败，并且清除SR的发起原因，
      而ESM或者RABM等触发的SR流程则继续*/
    if(NAS_EMM_SER_START_CAUSE_SMS_EST_REQ == NAS_EMM_SER_GetSerStartCause())
    {
        NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_OTHERS);
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
    }
     /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 如果UE模式为CS/PS，允许CS域注册，没有被原因值2拒绝过，则发起
       "combined TA/LA updating with IMSI attach"类型的联合TAU */
    if ((NAS_EMM_YES == NAS_EMM_IsCsPsUeMode())
        && (NAS_EMM_AUTO_ATTACH_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
        && (NAS_EMM_REJ_NO == NAS_LMM_GetEmmInfoRejCause2Flag())
        && (NAS_EMM_REJ_NO == NAS_EMMC_GetRejCause18Flag()))
    {
        /*停止T3417定时器*/
        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

        /*停止T3417ext定时器*/
        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        if (VOS_TRUE != NAS_EMM_SER_IsCsfbProcedure())
        {
            /*Inform RABM that SER fail*/
            NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
        }

        /*设置流程冲突标志位*/
        NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_SERVICE);
		/* lihong00150010 emergency tau&service begin */
        if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ);
        }
        else
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_SetImsiDetachFlag(NAS_EMM_IMSI_DETACH_VALID);

        /*启动TAU Procedure*/
        NAS_EMM_TAU_StartTAUREQ();
    }
     /*PC REPLAY MODIFY BY LEILI END*/
}



VOS_VOID NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnReAttachDetatchReq
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    NAS_EMM_SER_AbnormalOver();
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /* 向MRRC发送DETACH ACCEPT消息 */
    NAS_EMM_MrrcSendDetAcpMt();

    /* CSFB流程 */
    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        /* 如果不是紧急CSFB给MM发终止消息,继续RE-ATTACH流程 */
        if (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb())
        {
            NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
        }
        else
        {
            NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                   EMM_SS_DEREG_NORMAL_SERVICE,
                                   TI_NAS_EMM_STATE_NO_TIMER);

            NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

            /* 向MMC发送LMM_MMC_DETACH_IND消息 */
            NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

            /* 向MMC发送LMM_MMC_SERVICE_RESULT_IND消息 */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

            /*等待RRC_REL_IND*/
            NAS_EMM_WaitNetworkRelInd();

            return;
        }

    }
    NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 重新ATTACH的处理 */
    NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
    /* 向MMC发送LMM_MMC_DETACH_IND消息 */
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;

}



/*****************************************************************************
 Function Name   : NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetachCause2
 Description     : 主状态SER_INIT+子状态ATTACH_WAIT_CN_SER_CNF下收到网络侧
                   消息DETACH REQUEST(reattach not required类型，原因值为2)

 Input           : pstRcvEmmMsg--------------CN DETACH消息指针
 Output          : NONE
 Return          : NODE

 History         :
    1.lihong00150010         2012-02-25  Draft Enact
*****************************************************************************/
VOS_VOID NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetachCause2
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    /* 向MRRC发送DETACH ACCEPT消息 */
    NAS_EMM_MrrcSendDetAcpMt();

     /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* 如果CN DETACH，reattach not required类型，原因值为2，
       则L模后续不能发起联合ATTACH和联合TAU */
    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

    /* 根据SER发起原因进行相应处理 */
    switch(NAS_EMM_SER_GetEmmSERStartCause())
    {
        case NAS_EMM_SER_START_CAUSE_SMS_EST_REQ:
            NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_USIM_CS_INVALID);
            NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
            break;

        case NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ:
        case NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ:
            NAS_EMM_SER_AbnormalOver();

            /* 给MM发终止 */
            NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);

            /* 状态迁移到REG.NORMAL_SERVICE*/
            NAS_EMM_AdStateConvert(EMM_MS_REG,
                           EMM_SS_REG_NORMAL_SERVICE,
                           TI_NAS_EMM_STATE_NO_TIMER);

            if (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState())
        	{
            	/*等待RRC_REL_IND*/
                NAS_EMM_WaitNetworkRelInd();
        	}
            break;


        case NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ:
            NAS_EMM_SER_AbnormalOver();
            NAS_EMM_AdStateConvert(EMM_MS_REG,
                                   EMM_SS_REG_NORMAL_SERVICE,
                                   TI_NAS_EMM_STATE_NO_TIMER);

            /* 向MMC发送LMM_MMC_SERVICE_RESULT_IND消息 */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

            /*等待RRC_REL_IND*/
            NAS_EMM_WaitNetworkRelInd();
            break;

        default:
            break;
    }

    /* 向MMC发送LMM_MMC_DETACH_IND消息 */
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    return;
}
VOS_VOID NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnNotReAttachDetatchReq
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    /* 如果CN DETACH，reattach not required类型，原因值为2，
       则L模后续不能发起联合ATTACH和联合TAU */
    if ((NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pstRcvEmmMsg->ucEmmCause)
        &&(NAS_EMM_AD_BIT_SLCT == pstRcvEmmMsg->ucBitOpCause))
    {
        NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetachCause2(pstRcvEmmMsg);

        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
             NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
             NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
        }
        #if 0
        if (NAS_EMM_UPDATE_MM_FLAG_VALID == NAS_LMM_GetEmmInfoUpdateMmFlag())
        {
            NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
            NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
        }
        #endif
        return;
    }

    /* 调用SER_INIT模块提供的函数 */
    NAS_EMM_SER_AbnormalOver();
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /* 向MRRC发送DETACH ACCEPT消息 */
    NAS_EMM_MrrcSendDetAcpMt();

    /* CSFB流程 */
    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        /* 如果不是紧急CSFB给MM发终止消息 */
        if (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb())
        {
            NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
        }
        else
        {
            /* 紧急CSFB向MMC发送LMM_MMC_SERVICE_RESULT_IND消息 */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        }
    }

    NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 根据不同原因值进行相应处理 */
    NAS_EMM_DetCnDetReqNotReattach(pstRcvEmmMsg);

    /* 向MMC发送LMM_MMC_DETACH_IND消息 */
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    return;

}
VOS_UINT32    NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetatchReq(VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{

    NAS_EMM_CN_DETACH_REQ_MT_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetatchReq is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetatchReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachReqMtMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnDetatchReq: MMC_LMM_DETACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }


    NAS_EMM_DETACH_LOG1_INFO("Detach type is:",pRcvEmmMsg->ucDetType);

    NAS_EMM_GLO_AD_GetMtDetachAccTxCnt() = 0;

    switch(pRcvEmmMsg->ucDetType)
    {
        case NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH:

            NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnImsiDetatchReq(pRcvEmmMsg);
            break;

        case NAS_EMM_DETACH_TYPE_MT_REATTACH:

            NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnReAttachDetatchReq(pRcvEmmMsg);
            break;

        case NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH:

            NAS_EMM_MsSerInitSsWtCnSerCnfMsgCnNotReAttachDetatchReq(pRcvEmmMsg);
            break;

        default:
            break;
    }

    return NAS_LMM_MSG_HANDLED;

}
/*****************************************************************************
 Function Name   : NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgImsiDetachReq
 Description     : 主状态RrcConnRelIni+子状态WaitRrcRel状态下收到网络侧
                   消息DETACH REQUEST(IMSI DETACH类型)

 Input           : pstRcvEmmMsg--------------CN DETACH消息指针
 Output          : NONE
 Return          : NODE

 History         :
    1.lihong00150010         2011-10-19  Draft Enact
    2.lihong00150010         2012-12-18  Modify:Emergency

*****************************************************************************/
VOS_VOID NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgImsiDetachReq
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    /* 原因值16,17,22会导致UE进入REG+ATTEMPT_TO_UPDATE_MM状态，而该状态下目前是
       丢弃IMSI类型的CN DETACH */
    if (NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_161722 == NAS_EMM_GetNormalServiceRelCause())
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgImsiDetachReq:Discard Imsi Detach");
        return ;
    }

    /* 向MMC发被 MT DETACH 打断 LMM_MMC_TAU_RESULT_IND*/
    /*NAS_EMM_MmcSendTauActionResultIndOthertype(MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE);*/

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
     /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 如果UE模式为CS/PS，允许CS域注册，没有被原因值2拒绝过，则发起
       "combined TA/LA updating with IMSI attach"类型的联合TAU */
    if ((NAS_EMM_YES == NAS_EMM_IsCsPsUeMode())
        && (NAS_EMM_AUTO_ATTACH_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
        && (NAS_EMM_REJ_NO == NAS_LMM_GetEmmInfoRejCause2Flag())
        && (NAS_EMM_REJ_NO == NAS_EMMC_GetRejCause18Flag()))
    {

        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

        /* 设置TAU START CAUSE 为 CN IMSI DETACH */
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_CN_IMSI_DETACH);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

        NAS_EMM_TAU_SetImsiDetachFlag(NAS_EMM_IMSI_DETACH_VALID);

        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        NAS_EMM_TAU_StartTAUREQ();
    }
    else
    {
        /* 终止TAU流程 */
        NAS_EMM_TAU_AbnormalOver();
        /* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStateRegNormalServiceOrRegLimitService();
        /*NAS_EMM_AdStateConvert( EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);*/
        /* lihong00150010 emergency tau&service end */
        /* IMSI DETACH后释放资源 */
        NAS_LMM_ImsiDetachReleaseResource();

        /*等待RRC_REL_IND*/
        NAS_EMM_WaitNetworkRelInd();
    }
    /*PC REPLAY MODIFY BY LEILI END*/
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgNotReattach2DetachReq
 Description     : 主状态TAU_INIT+子状态WAIT_RRC_REL_IND状态下收到网络侧
                   消息DETACH REQUEST(reattach not required，原因值为2)

 Input           : pstRcvEmmMsg--------------CN DETACH消息指针
 Output          : NONE
 Return          : NODE

 History         :
    1.lihong00150010         2011-10-19  Draft Enact
    2.lihong00150010         2012-12-18  Modify:Emergency

*****************************************************************************/
VOS_VOID NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgNotReattach2DetachReq
(
    const NAS_EMM_CN_DETACH_REQ_MT_STRU *pstRcvEmmMsg
)
{
    /* 原因值16,17,22会导致UE进入REG+ATTEMPT_TO_UPDATE_MM状态，而该状态下目前是
       丢弃reattach not required，原因值为2的CN DETACH */
    if (NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_161722 == NAS_EMM_GetNormalServiceRelCause())
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgNotReattach2DetachReq:Discard Imsi Detach");
        return ;
    }

    /* 终止TAU流程 */
    NAS_EMM_TAU_AbnormalOver();

    /* 向MMC发被 MO DETACH 打断 LMM_MMC_TAU_RESULT_IND*/
    /*NAS_EMM_MmcSendTauActionResultIndOthertype(MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE);*/

    /*向MRRC发送DETACH ACCEPT消息*/
    NAS_EMM_MrrcSendDetAcpMt();

    /*向MMC发送LMM_MMC_DETACH_IND消息*/
    NAS_EMM_AppSendDetIndMt(pstRcvEmmMsg);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* 如果CN DETACH，reattach not required类型，原因值为2，
       则L模后续不能发起联合ATTACH和联合TAU */
    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);
    /* lihong00150010 emergency tau&service begin */
    NAS_EMM_TranStateRegNormalServiceOrRegLimitService();
    /*NAS_EMM_AdStateConvert(             EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);*/
    /* lihong00150010 emergency tau&service end */
    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_WaitNetworkRelInd();
    return;
}
VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_DETACH_ACP_MO_STRU       *pRcvEmmMsg;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_ACP_MO_STRU *) pMsgStru;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp is entered",
                                                ulMsgId,
                                                pMsgStru);

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachAcpMoMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp: MMC_LMM_DETACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* 如果当前是数据连接态，则直接转到REG+NORMAL_SERVIC态，否则待释放RRC链路
       后再转到REG+NORMAL_SERVIC态 */
    if (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState())
    {
        NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp:data conn");

        NAS_EMM_AdStateConvert(             EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

        /* IMSI DETACH后释放资源 */
        NAS_LMM_ImsiDetachReleaseResource();

        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp:not data conn");

    NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return   NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachAcp( VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_DETACH_ACP_MO_STRU       *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_ACP_MO_STRU *) pMsgStru;

    NAS_EMM_DETACH_LOG_NORM("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachAcp is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachAcp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnDetachAcpMoMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_DETACH_LOG_ERR("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachAcp: MMC_LMM_DETACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);

    NAS_EMM_GLO_AD_GetDetAtmpCnt()  = 0;

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {
        NAS_EMM_ProcLocalNoUsim();
    }/* lihong00150010 emergency tau&service end */
    else
    {
        NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    }

    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return   NAS_LMM_MSG_HANDLED;
}


VOS_VOID  NAS_EMM_ProcDetachAuthRej(VOS_UINT32  ulCause)
{
   NAS_EMM_DETACH_LOG_INFO("NAS_EMM_ProcDetachAuthRej is entered");

    (VOS_VOID)ulCause;


    /*终止DETACH流程*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

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
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_AUTH_REJ);
	/* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {
        /* 给MMC回复LMM_MMC_USIM_STATUS_CNF */
        NAS_EMM_SendMmcUsimStatusCnf();

        /* 清除拔卡标识 */
        NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_FALSE;
    }
	/* lihong00150010 emergency tau&service end */
    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return;

}


VOS_UINT32  NAS_EMM_MsDrgInitSsWtCnDetCnfMsgAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        const VOS_VOID   *pMsgStru)
{
    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_MsDrgInitSsWtCnDetCnfMsgAuthRej enter!");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_ProcDetachAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);


    return NAS_LMM_MSG_HANDLED;
}
/*****************************************************************************
 Function Name   : NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthRej
 Description     : 主状态EMM_MS_REG+子状态EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF下收到AUTH REJ消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-09-28  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthRej
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{
    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgAuthRej is entered",
                                                ulMsgId,
                                                pMsgStru);

    NAS_EMM_ProcDetachAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}


VOS_VOID  NAS_EMM_ProcMsRegImsiDetachInitMsgRrcRelInd
(
    VOS_UINT32                          ulCause
)
{
    (VOS_VOID)ulCause;

    /*停止定时器T3421*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3421);

    /* 有卡，不区分释放原因值，完成本地DETACH*/

    /* 设置注册域为PS */
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);


    /*upon different cause value, enter different dealing*/
    switch (ulCause)
    {
        case LRRC_LNAS_REL_CAUSE_LOAD_BALANCE_REQ:
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE);
            break;

        case LRRC_LNAS_REL_CAUSE_CONN_FAIL:
            /* NAS_EMM_TAU_SaveEmmTAUStartCause(    NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE); */
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
            break;
        default:
            break;
    }

    /*修改状态：进入主状态sREG子状态EMM_SS_REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_DETACH_CNF消息*/
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* IMSI DETACH后释放资源 */
    NAS_LMM_ImsiDetachReleaseResource();

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();
}


VOS_UINT32  NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRrcRelInd
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    LRRC_LMM_REL_IND_STRU              *pMsgRrcRelInd  = (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    NAS_EMM_DETACH_LOG2_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRrcRelInd is entered",
                                                ulMsgId,
                                                pMsgStru);
    /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRrcRelInd: NULL PTR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*check current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF))
    {
        NAS_EMM_DETACH_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgRrcRelInd: STATE ERR!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*获得原因值*/
    ulCause = pMsgRrcRelInd->enRelCause;

    NAS_EMM_ProcMsRegImsiDetachInitMsgRrcRelInd(ulCause);

    return NAS_LMM_MSG_HANDLED;
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif









































