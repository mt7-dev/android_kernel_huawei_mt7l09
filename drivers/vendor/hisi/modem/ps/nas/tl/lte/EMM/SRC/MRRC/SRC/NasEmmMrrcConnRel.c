


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmGIMComm.h"
#include    "NasEmmMrrc.h"
#include    "NasEmmAttDetInclude.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMMRRCREL_C
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
NAS_EMM_MRRC_CTRL_STRU                  g_stEmmMrrcCtrl;


/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name  : NAS_EMM_MrrcRelInit
 Discription    : MRRC 建链模块初始化函数
 Input          :
 Output         : None
 Return         : None
 History:
      1.  zangyalan 57968 2008-10-16  Draft Enact
*****************************************************************************/
VOS_VOID NAS_EMM_MrrcRelInit(VOS_VOID)
{
    g_stEmmMrrcCtrl.ulRelCause = NAS_EMM_NORMALSERVICE_REL_CAUSE_NULL;

    /*清资源*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    /*改变连接状态,同时断开安全连接*/
    NAS_EMM_MrrcChangeRrcStatusToIdle();

    return;
}

/*lint -e960*/
/*lint -e961*/
VOS_VOID    NAS_EMM_SndRrcRelReq(NAS_LMM_BARRED_IND_ENUM_UINT32 enBarStatus)
{
    LRRC_LMM_REL_REQ_STRU                *pRelReqMsg;

    /* 分配内存*/
    pRelReqMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_REL_REQ_STRU));
    if(NAS_EMM_NULL_PTR == pRelReqMsg)
    {
        /* 打印内存分配失败log */
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_SndRrcRelReq:Alloc Msg Fail!");
        return;
    }

    /*填充VOS消息头*/
    NAS_EMM_GIM_RRC_MSG_HEADER(pRelReqMsg, (sizeof(LRRC_LMM_REL_REQ_STRU) - NAS_EMM_VOS_HEADER_LEN));

    /*填充消息ID*/
    pRelReqMsg->enMsgId    = ID_LRRC_LMM_REL_REQ;

    /*填充内容*/
    pRelReqMsg->enBarInd   = enBarStatus;

    /*发送RRC_MM_EST_REQ*/
    NAS_LMM_SEND_MSG(pRelReqMsg);

    return;
}


VOS_VOID NAS_EMM_WaitNetworkRelInd(VOS_VOID)
{
    NAS_EMM_FSM_STATE_STRU              EmmState;

    /* 察看RRC连接是否已经释放 */
    if (NAS_EMM_YES == NAS_LMM_IsRrcConnectStatusIdle())
    {
        /*打印已经释放*/
        NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_WaitNetworkRelInd:Rrc Conn release already");

        /*转发内部的REL IND*/
        NAS_EMM_CommProcConn2Ilde();
    }
    else
    {
        NAS_LMM_MEM_SET(&EmmState, 0, sizeof(NAS_EMM_FSM_STATE_STRU));

        /*原状态压栈*/
        NAS_EMM_FSM_PushState();

        /* 进入RRC_CONN_RELINIT.WAIT_RRC_REL状态 */
        EmmState.enFsmId     = NAS_LMM_PARALLEL_FSM_EMM;
        EmmState.enMainState = EMM_MS_RRC_CONN_REL_INIT;
        EmmState.enSubState  = EMM_SS_RRC_CONN_WAIT_REL_CNF;
        EmmState.enStaTId = TI_NAS_EMM_STATE_T3440;
        NAS_LMM_StaTransProc(EmmState);

        /*启动定时器TI_NAS_EMM_MRRC_WAIT_RRC_REL*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_T3440);
    }
    return ;
}



VOS_VOID NAS_EMM_ReattachDelay(VOS_VOID)
{
    NAS_EMM_FSM_STATE_STRU              EmmState;


    NAS_LMM_MEM_SET(&EmmState, 0, sizeof(NAS_EMM_FSM_STATE_STRU));

    /*原状态压栈*/
    NAS_EMM_FSM_PushState();

    /* 进入RRC_CONN_RELINIT.WAIT_RRC_REL状态 */
    EmmState.enFsmId     = NAS_LMM_PARALLEL_FSM_EMM;
    EmmState.enMainState = EMM_MS_RRC_CONN_REL_INIT;
    EmmState.enSubState  = EMM_SS_RRC_CONN_WAIT_REL_CNF;
    EmmState.enStaTId = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(EmmState);
    NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_REATTACH_DELAY);

    return ;
}




VOS_VOID NAS_EMM_RelReq(NAS_LMM_BARRED_IND_ENUM_UINT32 enBarStatus)
{
    NAS_EMM_FSM_STATE_STRU              EmmState;

    /* 察看RRC连接是否已经释放 */
    if (NAS_EMM_YES == NAS_LMM_IsRrcConnectStatusIdle())
    {
        /*打印已经释放*/
        NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_RelReq:Rrc Conn release already");

        /*转发内部的REL IND*/
        NAS_EMM_CommProcConn2Ilde();
    }
    else
    {
        NAS_LMM_MEM_SET(&EmmState, 0, sizeof(NAS_EMM_FSM_STATE_STRU));

        /*原状态压栈*/
        NAS_EMM_FSM_PushState();

        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_SndRrcRelReq(enBarStatus);

        /* 给RRC发释放后，连接状态改为释放过程中 */
        NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);

        /* 进入RRC_CONN_RELINIT.WAIT_RRC_REL状态 */
        EmmState.enFsmId     = NAS_LMM_PARALLEL_FSM_EMM;
        EmmState.enMainState = EMM_MS_RRC_CONN_REL_INIT;
        EmmState.enSubState  = EMM_SS_RRC_CONN_WAIT_REL_CNF;
        EmmState.enStaTId = TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF;
        NAS_LMM_StaTransProc(EmmState);

        /*启动定时器TI_NAS_EMM_MRRC_WAIT_RRC_REL*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);
    }

    return ;
}


VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelCnfMsgRrcMmRelInd
                                 (VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru)
{
    LRRC_LMM_REL_IND_STRU              *pstRrcRelInd = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelCnfMsgRrcMmRelInd is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelCnfMsgRrcMmRelInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*get the RRC Release Cause*/
    pstRrcRelInd = (LRRC_LMM_REL_IND_STRU *)pMsgStru;

    /*停止定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_REATTACH_DELAY);


    /*状态出栈，恢复上一个状态 */
    NAS_EMM_FSM_PopState();

    NAS_EMM_CommProcConn2Ilde();
    if((EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
        &&((EMM_SS_REG_NORMAL_SERVICE == NAS_LMM_GetEmmCurFsmSS())
            ||(EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM == NAS_LMM_GetEmmCurFsmSS())))
    {
        NAS_EMM_TAU_MsRegMsgRelInd(pstRrcRelInd->enRelCause);
    }

    if (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        /* AUTH失败次数清零*/
        NAS_EMM_GetAuthFailTimes() = 0;

        /* 状态出栈*/
        NAS_EMM_FSM_PopState();
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelCnfMsgRrcMmRelCnf
(
    VOS_UINT32 ulMsgId,
    const VOS_VOID *pMsgStru
)
{
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgRrcMmRelCnf is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgRrcMmRelCnf:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*停止定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    /*状态出栈，恢复上一个状态 */
    NAS_EMM_FSM_PopState();

    NAS_EMM_CommProcConn2Ilde();
    if (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        /* AUTH失败次数清零*/
        NAS_EMM_GetAuthFailTimes() = 0;

        /* 状态出栈*/
        NAS_EMM_FSM_PopState();
    }

    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTIWaitRrcRelTO
                                              (VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru)
{
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTIWaitRrcRelTO is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTIWaitRrcRelTO:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*停止定时器*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    /*状态出栈，恢复上一个状态 */
    NAS_EMM_FSM_PopState();

    NAS_EMM_CommProcConn2Ilde();
    if (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        /* AUTH失败次数清零*/
        NAS_EMM_GetAuthFailTimes() = 0;

        /* 状态出栈*/
        NAS_EMM_FSM_PopState();
    }
    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsg3440Exp(VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru)
{
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsg3440Exp is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsg3440Exp:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /*发送RRC_MM_REL_REQ*/
    NAS_EMM_SndRrcRelReq(NAS_LMM_NOT_BARRED);

    NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);

    /*启动定时器TI_NAS_EMM_MRRC_WAIT_RRC_REL*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    return NAS_LMM_MSG_HANDLED;
}




VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgReattachDelayTmrExp(VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /* 紧急状态下处理暂不考虑,待紧急TAU及Service合入后修改 */

    /* 停止T3440定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    /*状态出栈，恢复上一个状态 */
    NAS_EMM_FSM_PopState();

    /*发内部attach请求 */
    (VOS_VOID)NAS_EMM_SendIntraAttachReq();

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd
(
    VOS_UINT32      ulMsgId,
    const VOS_VOID *pMsgStru
)

{
    VOS_UINT32                          ulStaAtStackTop= NAS_EMM_NULL;

    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }
    (VOS_VOID)ulMsgId;

    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);

    if ((ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
         ||(ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
    {
        /* 如果没给RRC发释放请求，则一定之前启动了3440等网络释放 */
        if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            /*状态出栈，恢复上一个状态 */
            NAS_EMM_FSM_PopState();

            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);
            #if 0
            if (NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_18 == NAS_EMM_GetNormalServiceRelCause())
            {
                NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd Rel cause18.");
                NAS_EMM_RecogAndProc18Disable(NAS_LMM_CAUSE_CS_NOT_AVAIL);
                return NAS_LMM_MSG_HANDLED;
            }

            if (NAS_EMM_NORMALSERVICE_REL_CAUSE_EPSONLY_161722 == NAS_EMM_GetNormalServiceRelCause())
            {
                NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd Rel cause161722.");
                NAS_EMM_RecogAndProc161722Disable(NAS_LMM_CAUSE_MSC_UNREACHABLE);
                return NAS_LMM_MSG_HANDLED;
            }

            NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd Rel cause2 or other disable lte.");
            NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();
            #endif
        }
    }
    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgCnDetatchReq
(
    VOS_UINT32 ulMsgId,
    const VOS_VOID *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;
    NAS_EMM_CN_DETACH_REQ_MT_STRU      *pRcvEmmMsg;


    /* 打印进入该函数，INFO_LEVEL */
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgCnDetatchReq is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgCnDetatchReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }
    (VOS_VOID)ulMsgId;

    pRcvEmmMsg = (NAS_EMM_CN_DETACH_REQ_MT_STRU *)pMsgStru;

    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);

    if ((ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
         ||(ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
    {
        /* 如果没给RRC发释放请求，则之前一定启动了3440等网络释放 */
        if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            /*状态出栈，恢复上一个状态 */
            NAS_EMM_FSM_PopState();

            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            NAS_EMM_GLO_AD_GetMtDetachAccTxCnt()  = 0;

            /* 不可能存在TAU打断DETACH或者TAU打断SERVICE的场景，因为如果存在冲突，则会设置
               ACTIVE标识，因此在TAU成功后不会进入等待RRC链路释放的状态 */

            /* 24301 5.3.1章节指出此状态下收到CN DETACH时需要停止3440定时器，处理CN DETACH消息，
               upon receipt of a DETACH REQUEST message, the UE shall stop timer T3440
               and respond to the network initiated detach as specified in subclause 5.5.2.3.*/

            /* 处理IMSI DETACH类型的CN DETACH */
            if (NAS_EMM_DETACH_TYPE_MT_IMSI_DETACH == pRcvEmmMsg->ucDetType)
            {
                NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgImsiDetachReq:Process Imsi Detach");

                NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgImsiDetachReq(pRcvEmmMsg);

                return  NAS_LMM_MSG_HANDLED;
            }

            /* 处理re-attach not required类型，原因值为#2的CN DETACH */
            if ((NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
                && (NAS_EMM_BIT_SLCT == pRcvEmmMsg->ucBitOpCause)
                && (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == pRcvEmmMsg->ucEmmCause))
            {
                NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgNotReattach2DetachReq:Process cause2 detach");

                NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgNotReattach2DetachReq(pRcvEmmMsg);

                return  NAS_LMM_MSG_HANDLED;
            }

            /*调用TAU模块提供的函数*/
            NAS_EMM_TAU_AbnormalOver();

            /* 向MMC发被 MO DETACH 打断 LMM_MMC_TAU_RESULT_IND*/
            /*NAS_EMM_MmcSendTauActionResultIndOthertype(MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE);*/

            /*向MRRC发送DETACH ACCEPT消息*/
            NAS_EMM_MrrcSendDetAcpMt();

            /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
            NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

            /*detach type == re-attached required*/
            if (NAS_EMM_DETACH_TYPE_MT_REATTACH == pRcvEmmMsg->ucDetType)
            {

                NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

                NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
                /*等待RRC_REL_IND*/
                NAS_EMM_WaitNetworkRelInd();
            }

            /*detach type == re-attached not required*/
            if (NAS_EMM_DETACH_TYPE_MT_NOT_REATTACH == pRcvEmmMsg->ucDetType)
            {
                NAS_EMM_DetCnDetReqNotReattach(pRcvEmmMsg);
            }

            /*向MMC发送LMM_MMC_DETACH_IND消息*/
            NAS_EMM_AppSendDetIndMt(pRcvEmmMsg);

        }
        /*等待链路释放定时器启动,丢弃不处理该消息*/

    }
    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgEsmdataReq
(
    VOS_UINT32 ulMsgId,
    const VOS_VOID *pMsgStru
)

{
    EMM_ESM_DATA_REQ_STRU              *pstsmdatareq = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_GIM_NORMAL_LOG( "NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgEsmdataReq is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgEsmdataReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    pstsmdatareq = (EMM_ESM_DATA_REQ_STRU *)pMsgStru;

    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);

    if ((ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
         ||(ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
    {
        /* 如果没给RRC发释放请求，则之前一定启动了3440等网络释放 */
        if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            if (VOS_TRUE == pstsmdatareq->ulIsEmcType)
            {
                NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
            }

            /* 透传SM消息 */
            NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstsmdatareq->stEsmMsg);
        }
    }

    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTcDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ETC_DATA_REQ_STRU              *pstTcdataReq = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    (VOS_VOID)ulMsgId;
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_GIM_NORMAL_LOG( "NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTcDataReq is entered.");

    /*入参检查*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgTcDataReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    pstTcdataReq = (EMM_ETC_DATA_REQ_STRU*)pMsgStru;

    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);

    if ((ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
         ||(ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
    {
        if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            /* 透传TC消息 */
            NAS_EMM_SER_SendMrrcDataReq_Tcdata(pstTcdataReq);
        }
    }
    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgRabmRelReq
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    (VOID)ulMsgId;
    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgRabmRelReq is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_GIM_ERROR_LOG("NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgRabmRelReq:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);

    if ((ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
         ||(ulStaAtStackTop == NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM)))
    {
        if (NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            /*启动定时器TI_NAS_EMM_MRRC_WAIT_RRC_REL*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

            /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
            NAS_EMM_SndRrcRelReq(NAS_LMM_NOT_BARRED);

            /* 设置连接状态为释放过程中 */
            NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);

        }
    }
    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRrcRelInitSsWaitRelCnfMsgMmcCoverageLostInd(
                    VOS_UINT32                              ulMsgId,
                    VOS_VOID                                *pMsg)
{
    NAS_EMM_GIM_INFO_LOG("NAS_EMM_MsRrcRelInitSsWaitRelCnfMsgMmcCoverageLostInd: Enter.");

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsg);

    /* 停止当前的压栈流程 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

    /* 出栈 */
    NAS_EMM_FSM_PopState();


    /* EMM给EMM自己发送AREA LOST消息，以给压栈前的流程处理 */
    NAS_EMMC_SendEmmCoverageLostInd();
    if (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        /* AUTH失败次数清零*/
        NAS_EMM_GetAuthFailTimes() = 0;

        /* 状态出栈*/
        NAS_EMM_FSM_PopState();
    }
    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_MrrcChangeRrcStatusToIdle( VOS_VOID )
{
    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_MrrcChangeRrcStatusToIdle is enter");

    /*如果当前连接状态不是IDLE，则向 MMC上报*/
    if(NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_SendMmcStatusInd(       MMC_LMM_STATUS_TYPE_CONN_STATE,
                                        MMC_LMM_CONN_IDLE);
    }

    /*把RRC连接状态改为非连接状态*/
    NAS_EMM_GetConnState() = NAS_EMM_CONN_IDLE;

    /*连接态从CONN进入IDLE，导致当前安全上下文状态变化*/
    NAS_EMM_ConnToIdleTransSecuSta();

    return;
}
VOS_VOID  NAS_EMM_ProcDeregMmcRelReq(VOS_VOID )
{
    NAS_EMM_FSM_STATE_STRU              EmmState;

    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_ProcDeregMmcRelReq is enter");

    /* 停止ATTACH定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    /*NAS_LMM_StopStateTimer(TI_NAS_EMM_T3421);*/
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3416);

    /* 将EMM状态转移至MS_DEREG + SS_PLMN_SEARCH状态 */
    EmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    EmmState.enMainState                = EMM_MS_DEREG;
    EmmState.enSubState                 = EMM_SS_DEREG_PLMN_SEARCH;
    EmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(EmmState);

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    /*给MMC上报attach结果*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_FAILURE);

    /*向RRC发送RRC_MM_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;
}


VOS_VOID  NAS_EMM_ProcDetachMmcRelReq(VOS_VOID )
{
    NAS_EMM_FSM_STATE_STRU              EmmState;

    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_ProcDetachMmcRelReq is enter");

    /* 停止detach定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3421);
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {
        /* 将EMM状态转移至MS_DEREG + SS_PLMN_SEARCH状态 */
        EmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
        EmmState.enMainState                = EMM_MS_DEREG;
        EmmState.enSubState                 = EMM_SS_DEREG_NO_IMSI;
        EmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
        NAS_LMM_StaTransProc(EmmState);

        /* 给MMC回复LMM_MMC_USIM_STATUS_CNF */
        NAS_EMM_SendMmcUsimStatusCnf();

        /* 清除拔卡标识 */
        NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_FALSE;
    }/* lihong00150010 emergency tau&service end */
    else
    {
        /* 将EMM状态转移至MS_DEREG + SS_PLMN_SEARCH状态 */
        EmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
        EmmState.enMainState                = EMM_MS_DEREG;
        EmmState.enSubState                 = EMM_SS_DEREG_NORMAL_SERVICE;
        EmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
        NAS_LMM_StaTransProc(EmmState);
    }

    /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    /*向RRC发送RRC_MM_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;
}





VOS_VOID  NAS_EMM_ProcRegMmcRelReq(VOS_VOID )
{
    NAS_EMM_GIM_NORMAL_LOG("NAS_EMM_ProcRegMmcRelReq is enter");

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);
    /*NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);
    /*NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3416);*/

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /* 释放缓存的ESM消息*/
    NAS_EMM_SerClearEsmDataBuf();

    /* 清空接入Bar信息*/
    NAS_EMM_ClearBarResouce();


    /* 将状态转移至MS_REG + SS_REG_PLMN_SEARCH状态 */
    /*
    EmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    EmmState.enMainState                = EMM_MS_REG;
    EmmState.enSubState                 = EMM_SS_REG_PLMN_SEARCH;
    EmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(EmmState);
    */
    return;
}



VOS_VOID NAS_EMM_ProcRegMmcRelReqWhileTauInit(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ProcRegMmcRelReqWhileTauInit");

    /*释放处理*/
    NAS_EMM_ProcRegMmcRelReq();

    /*按照TAU异常的流程来处理*/
    NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;
    NAS_EMM_TAU_ProcAbnormal();
    /* 向MMC发送LMM_MMC_TAU_RESULT_IND消息在冲突内部处理 */
    NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                                    (VOS_VOID*)NAS_EMM_NULL_PTR);

    /*向RRC发送RRC_MM_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
}



VOS_VOID NAS_EMM_ProcRegMmcRelReqWhileSerInit(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ProcRegMmcRelReqWhileSerInit");

    /*释放处理*/
    NAS_EMM_ProcRegMmcRelReq();

    /*转换状态*/
    NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();

    /*向RRC发送RRC_MM_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
}
VOS_VOID NAS_EMM_ProcRegMmcRelReqWhileImsiDetach(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ProcRegMmcRelReqWhileImsiDetach");

    /*对IMSI detach进行处理*/
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
    NAS_LMM_ImsiDetachReleaseResource();

    /*主动RRC发送连接释放*/
    NAS_EMM_ProcRegMmcRelReq();

    /*转换状态*/
    NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();

    /*向RRC发送RRC_MM_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

}
/*lint +e961*/
/*lint +e960*/




/***************************************************************************/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of 子系统+模块+文件名.c */
