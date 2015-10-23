


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include "EmmTcInterface.h"
/* lihong00150010 emergency tau&service begin */
#include "NasEmmAttDetInclude.h"
/* lihong00150010 emergency tau&service ends */

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMSERVICESERREQ_C
/*lint +e767*/



/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lihong00150010 emergency tau&service begin */

VOS_UINT32 NAS_EMM_MsRegSsLimitedSrvMsgRabmReestReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    static VOS_UINT32                   ulMsgCnt    = NAS_EMM_NULL;
    EMM_ERABM_REEST_REQ_STRU           *pstReestReq = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsRegSsLimitedSrvMsgRabmReestReq is entered.");

    pstReestReq = (EMM_ERABM_REEST_REQ_STRU *)pMsgStru;

    /* 如果不是紧急业务，则丢弃 */
    if (VOS_TRUE != pstReestReq->ulIsEmcType)
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsLimitedSrvMsgRabmReestReq:Not EMC!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*如果处于连接态，打印出错信息*/
    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsLimitedSrvMsgRabmReestReq: CONN.");
        ulMsgCnt++;

        /* 做保护,防止EMM与RABM维护的RRC链路状态不一致,导致UE长时间无法发起建链*/
        /* 连续2次收到ERABM的建链请求时,将RRC链路状态设置为IDLE态*/
        if (NAS_EMM_DISCARD_ERABM_RESET_REQ_MAX_CNT > ulMsgCnt)
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*更新连接状态*/
        NAS_EMM_MrrcChangeRrcStatusToIdle();
    }
    ulMsgCnt = NAS_EMM_NULL;

    NAS_EMM_SER_RcvRabmReestReq(pstReestReq->ulIsEmcType);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32 NAS_EMM_MsRegSsNormalMsgRabmReestReq(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    static VOS_UINT32                   ulMsgCnt    = NAS_EMM_NULL;
    EMM_ERABM_REEST_REQ_STRU           *pstReestReq = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "Nas_Emm_MsRegSsNormalMsgRabmReestReq is entered.");

    /*检查当前状态和输入指针*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
         return NAS_LMM_MSG_HANDLED;
    }

    /*如果处于连接态，打印出错信息*/
    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsNormalMsgRabmReestReq: CONN.");
        ulMsgCnt++;

        /* 做保护,防止EMM与RABM维护的RRC链路状态不一致,导致UE长时间无法发起建链*/
        /* 连续2次收到ERABM的建链请求时,将RRC链路状态设置为IDLE态*/
        if (NAS_EMM_DISCARD_ERABM_RESET_REQ_MAX_CNT > ulMsgCnt)
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*更新连接状态*/
        NAS_EMM_MrrcChangeRrcStatusToIdle();
    }
    ulMsgCnt = NAS_EMM_NULL;

    /* 如果CSFB延时定时器在运行，优先考虑CSFB，PS数据业务优先级放低 ，
       (另: 即使现在发起SER，也将因当前处于释放过程中而直接失败)，
       做保护处理，此处丢弃RABM的建链请求*/
    if((NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_CSFB_DELAY))
        &&(VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        &&(NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsRegSsNormalMsgRabmReestReq: Msg discard, CSFB delay timer is running.");
        return NAS_LMM_MSG_HANDLED;
    }

	/* lihong00150010 emergency tau&service begin */
    pstReestReq = (EMM_ERABM_REEST_REQ_STRU *)pMsgStru;
    NAS_EMM_SER_RcvRabmReestReq(pstReestReq->ulIsEmcType);
	/* lihong00150010 emergency tau&service end */
    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmReestReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    static VOS_UINT32                   ulMsgCnt    = NAS_EMM_NULL;
    EMM_ERABM_REEST_REQ_STRU           *pstReestReq = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmReestReq is entered.");

    /*检查当前状态和输入指针*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
         return NAS_LMM_MSG_HANDLED;
    }

    /*如果处于连接态，打印出错信息*/
    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmReestReq: CONN.");
        ulMsgCnt++;

        /* 做保护,防止EMM与RABM维护的RRC链路状态不一致,导致UE长时间无法发起建链*/
        /* 连续2次收到ERABM的建链请求时,将RRC链路状态设置为IDLE态*/
        if (NAS_EMM_DISCARD_ERABM_RESET_REQ_MAX_CNT > ulMsgCnt)
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*更新连接状态*/
        NAS_EMM_MrrcChangeRrcStatusToIdle();
    }
    ulMsgCnt = NAS_EMM_NULL;

    /* 记录UPDATE_MM标识 */
    /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_VALID);*/
	/* lihong00150010 emergency tau&service begin */
    pstReestReq = (EMM_ERABM_REEST_REQ_STRU *)pMsgStru;
    NAS_EMM_SER_RcvRabmReestReq(pstReestReq->ulIsEmcType);
	/* lihong00150010 emergency tau&service end */

    return NAS_LMM_MSG_HANDLED;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsLimitedSrvMsgRabmRelReq
 Description     : Reg.Limit_Service状态下收到RABM数传异常，释放连接
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-12-29  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgRabmRelReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsLimitedSrvMsgRabmRelReq entered.");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsNormalMsgRabmRelReq
 Description     : Reg.Normal_Service状态下收到RABM数传异常，释放连接
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing      2010-12-29  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsNormalMsgRabmRelReq( VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsNormalMsgRabmRelReq entered.");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return NAS_LMM_MSG_HANDLED;

}

/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmRelReq
 Description     : 处理在REG+ATTEMPT_TO_UPDATE_MM状态下收到RABM的释放链路请求
 Input           : ulMsgId-------------------------消息ID
                   pMsgStru------------------------消息指针
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-11-17  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmRelReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRabmRelReq entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgTcDataReq
 Description     : 在住状态为EMM_RS_REG_INIT，子状态为EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF
                   下收到ID_EMM_ETC_DATA_REQ消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-10-16  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgTcDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgTcDataReq is entered.");

    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgTcDataReq ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    NAS_EMM_SER_SendMrrcDataReq_Tcdata((EMM_ETC_DATA_REQ_STRU *)pMsgStru);

    return NAS_LMM_MSG_HANDLED;
}


/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsNormalMsgTcDataReq
 Description     : 在住状态为EMM_RS_REG，子状态为EMM_SS_NORMAL_SERVICE
                   下收到ID_EMM_ETC_DATA_REQ消息
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-10-16  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsNormalMsgTcDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                      ulRslt              = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsNormalMsgTcDataReq is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*CONN模式下，转发TC消息；IDLE模式下，打印出错信息*/
    if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState()) ||
        (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_SendMrrcDataReq_Tcdata((EMM_ETC_DATA_REQ_STRU *)pMsgStru);
    }
    else
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsNormalMsgTcDataReq:Warning:RRC connection is not Exist!");
    }

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgTcDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                      ulRslt              = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgTcDataReq is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*CONN模式下，转发TC消息；IDLE模式下，打印出错信息*/
    if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState()) ||
        (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_SendMrrcDataReq_Tcdata((EMM_ETC_DATA_REQ_STRU *)pMsgStru);
    }
    else
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsRegAttemptUpdateMmMsgTcDataReq:Warning:RRC connection is not Exist!");
    }

    return NAS_LMM_MSG_HANDLED;
}


/*******************************************************************************
  Module   :
  Function : NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgEsmDataReq
  Input    :
  Output   :
  NOTE     :
  Return   :
  History  :
    1.  lihong00150010  2009-09-25  新规作成
*******************************************************************************/
VOS_UINT32 NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgEsmDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt;
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgEsmDataReq is entered.");

    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegInitSsWaitCnAttachCnfMsgEsmDataReq ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsRegSsNormalMsgEsmDataReq(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq   = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsNormalMsgEsmDataReq entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    if (VOS_TRUE == pstEsmDataReq->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
    }

    /*CONN态，转发ESM消息*/
    if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState()) ||
        (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);
        return NAS_LMM_MSG_HANDLED;
    }

    /* 如果CSFB延时定时器在运行，优先考虑CSFB，PS数据业务优先级放低 ，
       (另: 即使现在发起SER，也将因当前处于释放过程中而直接失败)，
       做保护处理，此处丢弃ESM的建链请求*/
    if((NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_CSFB_DELAY))
        &&(VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        &&(NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsRegSsNormalMsgEsmDataReq: Msg discard, CSFB delay timer is running.");
        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_SER_RcvEsmDataReq(pMsgStru);

    return NAS_LMM_MSG_HANDLED;

}

/*****************************************************************************
 Function Name   : NAS_EMM_MsRegSsNormalMsgSmsEstReq
 Description     : 正常服务状态下处理SMS建链请求
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing 49683      2011-11-3  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsNormalMsgSmsEstReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsNormalMsgSmsEstReq entered.");

    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*因为建链函数已经经过预处理，进入这个函数，说明CS域已经注册，
      如果不处于空闲态，可以直接回复建链成功*/
    if (NAS_EMM_NO == NAS_LMM_IsRrcConnectStatusIdle())
    {
        NAS_LMM_SndLmmSmsEstCnf();

        NAS_LMM_SetConnectionClientId(NAS_LMM_CONNECTION_CLIENT_ID_SMS);
        return NAS_LMM_MSG_HANDLED;
    }

    /*设置SER触发原因为 NAS_EMM_SER_START_CAUSE_SMS_EST_REQ*/
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_SMS_EST_REQ);

    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmDataReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq   = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgEsmDataReq entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    if (VOS_TRUE == pstEsmDataReq->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
    }

    /*CONN态，转发ESM消息*/
    if((NAS_EMM_CONN_SIG == NAS_EMM_GetConnState()) ||
        (NAS_EMM_CONN_DATA == NAS_EMM_GetConnState()))
    {
        NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);
        return NAS_LMM_MSG_HANDLED;
    }
    /* 记录UPDATE_MM标识 */
    /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_VALID);*/
    NAS_EMM_SER_RcvEsmDataReq(pMsgStru);

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32 NAS_EMM_MsTauInitMsgRabmReestReq(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                                )
{
    VOS_UINT32                      ulRslt                = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "Nas_Emm_MsTauInitMsgRabmReestReq is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    return NAS_LMM_MSG_DISCARD;
    }
VOS_UINT32 NAS_EMM_MsTauInitMsgRrcPagingInd(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                                )
{
    VOS_UINT32                      ulRslt              = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "Nas_Emm_MsTauInitMsgRrcPagingInd is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    return NAS_LMM_STORE_HIGH_PRIO_MSG;
}
VOS_UINT32 NAS_EMM_MsSerInitMsgEsmdataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt             = NAS_EMM_FAIL;
    EMM_ESM_DATA_REQ_STRU              *pstsmdatareq       = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitMsgEsmdataReq is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_SER_INIT ,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*SER模块自行缓存ESM DATA消息*/
    pstsmdatareq = (EMM_ESM_DATA_REQ_STRU        *)pMsgStru;

    if (VOS_TRUE == pstsmdatareq->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
		/* lihong00150010 emergency tau&service begin */
        /* 缓存紧急类型的ESM消息 */
        NAS_EMM_SaveEmcEsmMsg(pMsgStru);
		/* lihong00150010 emergency tau&service end */
    }
    NAS_EMM_SER_SaveEsmMsg(pstsmdatareq);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsTauInitMsgEsmdataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt             = NAS_EMM_FAIL;
    EMM_ESM_DATA_REQ_STRU              *pstsmdatareq       = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "Nas_Emm_MsTauInitMsgEsmdataReq is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    pstsmdatareq = (EMM_ESM_DATA_REQ_STRU *)pMsgStru;

    if (VOS_TRUE == pstsmdatareq->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);
    }

    /* 若当前TAU是打断了SERVICE的TAU类型,
       则将此收到的ESM消息缓存 */
    if (NAS_EMM_COLLISION_SERVICE == NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        /*SER模块自行缓存ESM DATA消息*/
        NAS_EMM_SER_SaveEsmMsg(pstsmdatareq);
        /* lihong00150010 emergency tau&service begin */
        if (VOS_TRUE == pstsmdatareq->ulIsEmcType)
        {
            /* 缓存紧急类型的ESM消息 */
            NAS_EMM_SaveEmcEsmMsg(pMsgStru);
        }
        /* lihong00150010 emergency tau&service end */
        return  NAS_LMM_MSG_HANDLED;
    }

    /* 透传SM消息 */
    NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstsmdatareq->stEsmMsg);

    return  NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsTauInitMsgRabmRelReq
 Description     : TauInit.WtTauCnf状态下收到数传异常，终止当前TAU流程，处理同
                   底层异常
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing           2010-12-29  Draft Enact
    2.lihong00150010    2012-12-14  Modify:Emergency

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsTauInitMsgRabmRelReq(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsTauInitMsgRabmRelReq is entered.");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*终止当前TAU流程*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;

    NAS_EMM_TAU_ProcAbnormal();

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitMsgRabmRelReq:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndFailWithPara,
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
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);



    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID    NAS_EMM_SER_RcvRabmReestReq
(
    VOS_UINT32                          ulIsEmcType
)
{
    NAS_EMM_SER_LOG_INFO( "Nas_Emm_Ser_RcvRabmReestReq is entered.");
	/* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == ulIsEmcType)
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_RABM_REEST_EMC);
    }
    else
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_RABM_REEST);
    }
	/* lihong00150010 emergency tau&service end */
    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();
    return;
}


VOS_VOID    NAS_EMM_SER_RcvRrcStmsiPagingInd(VOS_VOID)
{

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_RcvRrcStmsiPagingInd is entered.");

    /*设置SER触发原因为 NAS_EMM_SER_START_CAUSE_RRC_PAGING*/
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_RRC_PAGING);

    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();
    return;

}
VOS_UINT32 NAS_EMM_SER_VerifyMtCsfb( VOS_VOID )
{
    NAS_LMM_CS_SERVICE_ENUM_UINT32      ulCsService = NAS_LMM_CS_SERVICE_BUTT;

    /* 判断注册域是否为CS+PS */
    if (NAS_LMM_REG_DOMAIN_CS_PS != NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyMtCsfb:cs is not registered!");
        return NAS_EMM_FAIL;
    }

    /* 判断UE是否支持CSFB,如果CS SERVICE未使能，则默认支持CSFB */
    ulCsService = NAS_EMM_GetCsService();
    if ((NAS_LMM_CS_SERVICE_CSFB_SMS != ulCsService)
        && (NAS_LMM_CS_SERVICE_BUTT != ulCsService))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyMtCsfb:ue is not support csfb!");
        return NAS_EMM_FAIL;
    }

    /* 判断网侧是否携带了SMS ONLY */
    /*leili modify for isr begin*/
    if (NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyMtCsfb:additional update result sms only");
        return NAS_EMM_FAIL;
    }
    #if 0
    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    if ((NAS_EMM_BIT_SLCT == pstPubInfo->bitOpAddUpRslt)
        && (NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == pstPubInfo->ulAdditionUpRslt))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyMtCsfb:additional update result sms only");
        return NAS_EMM_FAIL;
    }
    #endif
    /*leili modify for isr end*/

    /* 判断是否是L单模 */
    if(NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyMtCsfb:lte only");
        return NAS_EMM_FAIL;
    }

    return NAS_EMM_SUCC;
}


VOS_UINT32 NAS_EMM_SER_VerifyCsfb(MM_LMM_CSFB_SERVICE_TYPE_ENUM_UINT32  enCsfbSrvType)
{

    NAS_LMM_CS_SERVICE_ENUM_UINT32      ulCsService = NAS_LMM_CS_SERVICE_BUTT;

    /* 判断注册域是否为CS+PS */
    if (NAS_LMM_REG_DOMAIN_CS_PS != NAS_LMM_GetEmmInfoRegDomain())
    {
       NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyCsfb:cs is not registered!");
       return NAS_EMM_FAIL;
    }

    /* 判断UE是否支持CSFB,如果CS SERVICE未使能，则默认支持CSFB */
    ulCsService = NAS_EMM_GetCsService();
    if ((NAS_LMM_CS_SERVICE_CSFB_SMS != ulCsService)
       && (NAS_LMM_CS_SERVICE_BUTT != ulCsService))
    {
       NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyCsfb:ue is not support csfb!");
       return NAS_EMM_FAIL;
    }

    /* 对于MO类型的，进入稳态后处理，对于紧急类型的，在预处理里面已经判断，
    应该不会出现，在调用分支中注意 */
    /* 判断网侧是否携带了SMS ONLY */
    if ((NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())
        && (MM_LMM_CSFB_SERVICE_MT_NORMAL == enCsfbSrvType))
    {
       NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyCsfb:additional update result sms only");
       return NAS_EMM_FAIL;
    }

    /* 判断是否是L单模 */
    if(NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
       NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_VerifyCsfb:lte only");
       return NAS_EMM_FAIL;
    }
    return NAS_EMM_SUCC;
}



VOS_VOID    NAS_EMM_SER_RcvRrcCsPagingInd
(
    LRRC_LNAS_PAGING_UE_ID_ENUM_UINT32 enPagingUeId
)
{

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_RcvRrcCsPagingInd is entered.");

    /* 检测MT CSFB流程是否能够发起 */
    if (NAS_EMM_FAIL == NAS_EMM_SER_VerifyCsfb(MM_LMM_CSFB_SERVICE_MT_NORMAL))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_RcvRrcCsPagingInd:cannot csfb!");
        return ;
    }

    /* 给MM模块发送MM_MM_CSFB_SERVICE_PAGING_IND消息 */
    NAS_EMM_MmSendCsfbSerPaingInd(  NAS_EMM_MT_CSFB_TYPE_CS_PAGING,
                                    VOS_NULL_PTR,
                                    enPagingUeId);
    return;
}


VOS_VOID    NAS_EMM_SER_RcvEsmDataReq(VOS_VOID   *pMsgStru)
{
    EMM_ESM_DATA_REQ_STRU        *pstsmdatareq = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    NAS_EMM_SER_LOG_INFO( "Nas_Emm_Ser_RcvEsmDataReq is entered.");
	/* lihong00150010 emergency tau&service begin */
    /* 设置SERVICE触发原因值 */
    if (VOS_TRUE == pstsmdatareq->ulIsEmcType)
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC);

        /* 缓存紧急类型的ESM消息 */
        NAS_EMM_SaveEmcEsmMsg(          pMsgStru);
    }
    else
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ);
    }
	/* lihong00150010 emergency tau&service end */
    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*SER模块自行缓存ESM DATA消息*/
    pstsmdatareq = (EMM_ESM_DATA_REQ_STRU        *)pMsgStru;
    NAS_EMM_SER_SaveEsmMsg(pstsmdatareq);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();
    return;
}
VOS_VOID  NAS_EMM_SER_UplinkPending( VOS_VOID )
{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_UplinkPending is entered.");

    /*设置SER触发原因为 NAS_ESM_SER_START_CAUSE_UPLINK_PENDING*/
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_ESM_SER_START_CAUSE_UPLINK_PENDING);

    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

     /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();
    return;
}
VOS_VOID  NAS_EMM_SER_SmsEstReq( VOS_VOID )
{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SmsEstReq is entered.");

    /*设置SER触发原因为 NAS_EMM_SER_START_CAUSE_SMS_EST_REQ*/
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_SMS_EST_REQ);

    /*Inform RABM that SER init*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_INITIATE);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ServiceReq();

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

    return;
}
/*lint -e960*/
/*lint -e961*/
VOS_BOOL NAS_EMM_SER_IsSameEsmMsgInBuf
(
    const EMM_ESM_DATA_REQ_STRU               *pMsgStru
)
{
    VOS_UINT32                          i       = 0;
    EMM_ESM_DATA_REQ_STRU              *pEsmMsg = NAS_LMM_NULL_PTR;

    /* 如果消息长度和内容相同，就认为是重复消息 */
    for (i = 0; i < g_stEmmEsmMsgBuf.ulEsmMsgCnt; i++)
    {
        if (NAS_LMM_NULL_PTR != g_stEmmEsmMsgBuf.apucEsmMsgBuf[i])
        {
            pEsmMsg = (EMM_ESM_DATA_REQ_STRU *)g_stEmmEsmMsgBuf.apucEsmMsgBuf[i];

            /* lihong00150010 emergency tau&service begin */
            if ((pMsgStru->stEsmMsg.ulEsmMsgSize == pEsmMsg->stEsmMsg.ulEsmMsgSize)
             && (0 == NAS_LMM_MEM_CMP(pMsgStru->stEsmMsg.aucEsmMsg,
                                     pEsmMsg->stEsmMsg.aucEsmMsg,
                                     pEsmMsg->stEsmMsg.ulEsmMsgSize))
             && (pMsgStru->ulOpId == pEsmMsg->ulOpId)
             && (pMsgStru->ulIsEmcType == pEsmMsg->ulIsEmcType)
               )
            {
                return VOS_TRUE;
            }
            /* lihong00150010 emergency tau&service end */
        }
        else
        {
            NAS_EMM_SER_LOG_WARN( "NAS_EMM_SER_IsSameEsmMsgInBuf: Null buffer item.");
        }
    }

    return VOS_FALSE;
}

/* lihong00150010 emergency tau&service begin */

VOS_UINT32 NAS_EMM_SER_FindEsmMsg
(
    VOS_UINT32                          ulOpid
)
{
    VOS_UINT32                          ulIndex     = NAS_EMM_NULL;
    EMM_ESM_DATA_REQ_STRU              *pstEsmMsg   = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_FindEsmMsg is entered.");

    for (ulIndex = NAS_EMM_NULL; ulIndex < g_stEmmEsmMsgBuf.ulEsmMsgCnt; ulIndex++)
    {
        pstEsmMsg = (EMM_ESM_DATA_REQ_STRU *)g_stEmmEsmMsgBuf.apucEsmMsgBuf[ulIndex];
        if (ulOpid == pstEsmMsg->ulOpId)
        {
            return ulIndex;
        }
    }

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_FindEsmMsg failed!");

    return NAS_EMM_SER_MAX_ESM_BUFF_MSG_NUM;
}
VOS_VOID NAS_EMM_SER_DeleteEsmMsg
(
    VOS_UINT32                          ulOpid
)
{
    VOS_UINT32                          ulIndex = NAS_EMM_NULL;
    VOS_UINT32                          ulRslt  = NAS_EMM_NULL;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_DeleteEsmMsg is entered.");

    ulIndex = NAS_EMM_SER_FindEsmMsg(ulOpid);
    if (ulIndex >= g_stEmmEsmMsgBuf.ulEsmMsgCnt)
    {
        return ;
    }

    ulRslt = NAS_COMM_FreeBuffItem(NAS_COMM_BUFF_TYPE_EMM, g_stEmmEsmMsgBuf.apucEsmMsgBuf[ulIndex]);

    if (NAS_COMM_BUFF_SUCCESS != ulRslt)
    {
       NAS_EMM_SER_LOG_WARN("NAS_EMM_SER_DeleteEsmMsg, Memory Free is not succ");
    }

    g_stEmmEsmMsgBuf.apucEsmMsgBuf[ulIndex] = NAS_LMM_NULL_PTR;

    for (; ulIndex < (g_stEmmEsmMsgBuf.ulEsmMsgCnt - 1); ulIndex++)
    {
        g_stEmmEsmMsgBuf.apucEsmMsgBuf[ulIndex] = g_stEmmEsmMsgBuf.apucEsmMsgBuf[ulIndex+1];
    }

    g_stEmmEsmMsgBuf.apucEsmMsgBuf[g_stEmmEsmMsgBuf.ulEsmMsgCnt - 1] = NAS_LMM_NULL_PTR;

    g_stEmmEsmMsgBuf.ulEsmMsgCnt--;
}
/* lihong00150010 emergency tau&service end */


VOS_VOID NAS_EMM_SER_SaveEsmMsg(const EMM_ESM_DATA_REQ_STRU  *pMsgStru)
{
    VOS_VOID                            *pMsgBuf   = NAS_LMM_NULL_PTR;
    VOS_UINT32                           ulBufSize = 0;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_Ser_SaveEsmMsg is entered.");

    /* 不是重复的SM消息，插入队列*/
    if ((VOS_FALSE == NAS_EMM_SER_IsSameEsmMsgInBuf(pMsgStru))
     && (NAS_EMM_SER_MAX_ESM_BUFF_MSG_NUM > g_stEmmEsmMsgBuf.ulEsmMsgCnt))
    {
        ulBufSize = pMsgStru->stEsmMsg.ulEsmMsgSize +
                    sizeof(pMsgStru->stEsmMsg.ulEsmMsgSize) +
                    sizeof(pMsgStru->ulOpId) +
                    sizeof(pMsgStru->ulIsEmcType) +
                    EMM_LEN_VOS_MSG_HEADER +
                    EMM_LEN_MSG_ID;

        /* 分配空间 */
        pMsgBuf = NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_EMM, ulBufSize);

        if (NAS_LMM_NULL_PTR != pMsgBuf)
        {
            NAS_LMM_MEM_CPY(pMsgBuf, pMsgStru, ulBufSize);

            g_stEmmEsmMsgBuf.apucEsmMsgBuf[g_stEmmEsmMsgBuf.ulEsmMsgCnt] = pMsgBuf;
            g_stEmmEsmMsgBuf.ulEsmMsgCnt++;

        }
        else
        {
            NAS_EMM_SER_LOG_INFO( "NAS_EMM_Ser_SaveEsmMsg: NAS_AllocBuffItem return null pointer.");
        }
    }
    else
    {
        NAS_EMM_SER_LOG1_INFO( "NAS_EMM_Ser_SaveEsmMsg: ESM Msg Not Buffered, Buffered msg number is:",
                               g_stEmmEsmMsgBuf.ulEsmMsgCnt);
    }

    NAS_EMM_SER_LOG1_INFO( "NAS_EMM_Ser_SaveEsmMsg: Buffered msg number is:",
                           g_stEmmEsmMsgBuf.ulEsmMsgCnt);

    return;
}

#if 0
VOS_VOID  NAS_EMM_SER_GetEsmMsg( EMM_ESM_DATA_REQ_STRU *pstData )
{
    EMM_ESM_DATA_REQ_STRU               *pstEsmData = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulLen = NAS_LMM_NULL;

    if (NAS_LMM_NULL == g_stEmmEsmMsgBuf.ulEsmMsgCnt)
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_SER_GetEsmMsg: ESM msg is not exist.");
        return;
    }

    pstEsmData = (EMM_ESM_DATA_REQ_STRU *)g_stEmmEsmMsgBuf.apucEsmMsgBuf[g_stEmmEsmMsgBuf.ulEsmMsgCnt-1];

    ulLen   = pstEsmData->stEsmMsg.ulEsmMsgSize + sizeof(pstEsmData->stEsmMsg.ulEsmMsgSize)
                + EMM_LEN_VOS_MSG_HEADER + EMM_LEN_MSG_ID;

    NAS_LMM_MEM_CPY(pstData,pstEsmData,ulLen);


    return;
}
#endif



VOS_UINT32 NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgEsmDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    (VOS_VOID)(ulMsgId);
    NAS_EMM_SER_LOG_INFO("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgEsmDataReq is entered.");
    NAS_EMM_SER_SendMrrcDataReq_ESMdata(&pstEsmDataReq->stEsmMsg);
    return NAS_LMM_MSG_HANDLED;
}




VOS_VOID  NAS_EMM_MsTauSerSsWaitCnCnfEmergencyCsfbProc(VOS_VOID)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;

    /* TAU过程中, 后面挂起会自动清除资源 */
    if (EMM_MS_TAU_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        /* 向MMC发送LMM_MMC_TAU_RESULT_IND消息 */
        ulTauRslt = MMC_LMM_TAU_RSLT_FAILURE;
        NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

        NAS_EMM_TAU_AbnormalOver();
    }
    else  /* SER过程中,终止SER */
    {
        NAS_EMM_SER_AbnormalOver();
    }

    /* 转入REG.PLMN-SEARCH等MMC挂起 */
    NAS_EMM_AdStateConvert(EMM_MS_REG,
                           EMM_SS_REG_PLMN_SEARCH,
                           TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

    /* 如果处于连接态，压栈释放处理 */
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;

}


VOS_VOID  NAS_EMM_MsAnySsWaitCnDetachCnfEmergencyCsfbProc(VOS_VOID)
{
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3421);

    /* REG. EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF 态*/
    if (EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        /* 向MMC发送本地LMM_MMC_DETACH_IND消息 */
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
        NAS_EMM_AdStateConvert(EMM_MS_REG,
                               EMM_SS_REG_NORMAL_SERVICE,
                               TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_ImsiDetachReleaseResource();
    }
    else /* DEREG_INIT. EMM_SS_DETACH_WAIT_CN_DETACH_CNF 态*/
    {
        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        NAS_EMM_SendDetRslt(MMC_LMM_DETACH_RSLT_SUCCESS);
        NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                               EMM_SS_DEREG_NORMAL_SERVICE,
                               TI_NAS_EMM_STATE_NO_TIMER);

        /* 通知ESM清除资源 */
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

    /* 如果处于连接态，压栈释放处理 */
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;

}
VOS_VOID  NAS_EMM_MsRegInitSsAnyStateEmergencyCsfbProc(VOS_VOID)
{
    /* 给MMC上报ATTACH结果为失败 */
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_FAILURE);

    /* ATTACH停定时器 + 清除资源 */
    NAS_EMM_Attach_SuspendInitClearResourse();

    /* 修改状态：进入主状态DEREG子状态DEREG_PLMN_SEARCH, 此时服务状态不上报改变*/
    NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                           EMM_SS_DEREG_PLMN_SEARCH ,
                           TI_NAS_EMM_STATE_NO_TIMER);

    /* 通知ESM清除资源 */
    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

    /* 如果处于连接态，压栈释放处理 */
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;

}


VOS_UINT32  NAS_EMM_UnableDirectlyStartMoEmergencyCsfbProc(VOS_VOID)
{
    switch(NAS_EMM_CUR_MAIN_STAT)
    {
        case    EMM_MS_REG_INIT:
            NAS_EMM_MsRegInitSsAnyStateEmergencyCsfbProc();
            break;

        case    EMM_MS_TAU_INIT:
        case    EMM_MS_SER_INIT:
            NAS_EMM_MsTauSerSsWaitCnCnfEmergencyCsfbProc();
            break;

        case    EMM_MS_RRC_CONN_EST_INIT:
        case    EMM_MS_RRC_CONN_REL_INIT:
            NAS_EMM_SER_LOG_NORM( "NAS_EMM_CannotDirectlyStartMoEmergencyCsfbProc:High priority storage!");
            return NAS_LMM_STORE_HIGH_PRIO_MSG;

        case    EMM_MS_AUTH_INIT:
        case    EMM_MS_RESUME:
            NAS_EMM_SER_LOG_NORM( "NAS_EMM_CannotDirectlyStartMoEmergencyCsfbProc:Low priority storage!");
            return NAS_LMM_STORE_LOW_PRIO_MSG;

        case    EMM_MS_REG:
        case    EMM_MS_DEREG_INIT:
            if ((EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF == NAS_LMM_GetEmmCurFsmSS())
                || (EMM_SS_DETACH_WAIT_CN_DETACH_CNF == NAS_LMM_GetEmmCurFsmSS()))
            {
                NAS_EMM_MsAnySsWaitCnDetachCnfEmergencyCsfbProc();
            }
            else
            {
                /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
                NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
            }
            break;

        default:

            /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
            NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
            break;
    }

    return NAS_LMM_MSG_HANDLED;

}



VOS_UINT32  NAS_EMM_MsRegPreProcMmNormalCsfbNotifyMsg
(
    MM_LMM_CSFB_SERVICE_TYPE_ENUM_UINT32  enCsfbSrvTyp
)
{
    NAS_EMM_SER_LOG_NORM( "NAS_EMM_MsRegPreProcMmNormalCsfbNotifyMsg:enter!");

    if ((EMM_SS_REG_NORMAL_SERVICE == NAS_EMM_CUR_SUB_STAT)
        || (EMM_SS_REG_WAIT_ACCESS_GRANT_IND == NAS_EMM_CUR_SUB_STAT))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_MsRegPreProcMmNormalCsfbNotifyMsg:REG+NORMAL!");
        return NAS_EMM_FAIL;
    }
    /* 在这几个状态下，如果是MO类型的，且T3442没有再运行，且不是SMS ONLY,则选网到GU模 */
    else if ((EMM_SS_REG_ATTEMPTING_TO_UPDATE == NAS_EMM_CUR_SUB_STAT)
                || (EMM_SS_REG_LIMITED_SERVICE == NAS_EMM_CUR_SUB_STAT)
                || (EMM_SS_REG_NO_CELL_AVAILABLE == NAS_EMM_CUR_SUB_STAT))
    {
        if (MM_LMM_CSFB_SERVICE_MO_NORMAL == enCsfbSrvTyp)
        {
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
            return NAS_EMM_SUCC;
        }
        NAS_EMM_MmSendCsfbSerEndInd(MMC_LMM_SERVICE_RSLT_FAILURE);
        return NAS_EMM_SUCC;
    }
    else
    {
        NAS_EMM_PUBU_LOG1_ERR("NAS_EMM_RcvMmNormalCsfbNotifyMsgProc: Sub State is err!",NAS_EMM_CUR_SUB_STAT);

        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);

        return NAS_EMM_SUCC;
    }
}
VOS_UINT32  NAS_EMM_RcvMmNormalCsfbNotifyMsgProc
(
    MM_LMM_CSFB_SERVICE_TYPE_ENUM_UINT32  enCsfbSrvTyp
)
{
    /* 清除CSFB ABORT标识 */
    NAS_EMM_SER_SaveEmmSerCsfbAbortFlag(NAS_EMM_CSFB_ABORT_FLAG_INVALID);

    /* 检测CSFB流程是否能够发起 */
    if (NAS_EMM_FAIL == NAS_EMM_SER_VerifyCsfb(enCsfbSrvTyp))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmNormalCsfbNotifyMsgProc:cannot csfb!");
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
        return NAS_LMM_MSG_HANDLED;
    }

    if ((MM_LMM_CSFB_SERVICE_MO_NORMAL == enCsfbSrvTyp)
        && ((NAS_LMM_TIMER_RUNNING == NAS_LMM_IsStaTimerRunning(TI_NAS_EMM_STATE_SERVICE_T3442))
            || (NAS_LMM_ADDITIONAL_UPDATE_SMS_ONLY == NAS_EMM_GetAddUpdateRslt())))
    {
        NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmNormalCsfbNotifyMsgProc:SMS ONLY and mo csfb!");
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        return NAS_LMM_MSG_HANDLED;
    }

    switch(NAS_EMM_CUR_MAIN_STAT)
    {
        /*压栈中间状态，需要高优先级缓存，等pop出状态之后再处理*/
        case    EMM_MS_RRC_CONN_EST_INIT:
        case    EMM_MS_RRC_CONN_REL_INIT:
            NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmNormalCsfbNotifyMsgProc:High priority storage!");
            return NAS_LMM_STORE_HIGH_PRIO_MSG;

        /*TAU和SR流程中，鉴权过程中，恢复过程中，低优先级缓存，待进入稳态后处理，*/
        case    EMM_MS_TAU_INIT:
        case    EMM_MS_AUTH_INIT:
        case    EMM_MS_RESUME:
            NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmNormalCsfbNotifyMsgProc:Low priority storage!");
            return NAS_LMM_STORE_LOW_PRIO_MSG;

        case    EMM_MS_SER_INIT:

            /* 判断当前是MT CSFB触发的ESR流程中，且网侧再次触发MT CSFB，则重新触发ESR，定时器T3417EXT重新启动
               如果收到了MO类型的CSFB则直接丢弃*/
            if ((NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause()))
            {
                if (MM_LMM_CSFB_SERVICE_MT_NORMAL == enCsfbSrvTyp)
                {
                    /*停止定时器T3417ext*/
                    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

                    /*设置SER触发原因值*/
                    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ);

                    /* 设置UE接受CSFB */
                    NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

                    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
                    NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();

                    /*启动定时器T3417ext*/
                    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);
                }
                return NAS_LMM_MSG_HANDLED;
            }
            else
            {
                return NAS_LMM_STORE_LOW_PRIO_MSG;
            }

        /*只有REG+NORMAL_SERVICE态和REG+WAIT_ACCESS_GRANT_IND态能发起CSFB*/
        case    EMM_MS_REG:
            if (NAS_EMM_FAIL == NAS_EMM_MsRegPreProcMmNormalCsfbNotifyMsg(enCsfbSrvTyp))
            {
                return NAS_LMM_MSG_DISCARD;
            }
            else
            {
                return NAS_LMM_MSG_HANDLED;
            }

        default:/*其他状态为错误的状态，增加告警打印*/
            NAS_EMM_PUBU_LOG1_ERR("NAS_EMM_RcvMmNormalCsfbNotifyMsgProc: Main State is err!",NAS_EMM_CUR_MAIN_STAT);

            /*为容错，增加对MM的回复*/
            NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);

            return NAS_LMM_MSG_HANDLED;
    }
}
/*lint +e961*/
/*lint +e960*/

VOS_UINT32  NAS_EMM_RcvMmMoEmergencyCsfbNotifyMsgProc(VOS_VOID)
{
    /* 清除CSFB ABORT标识 */
    NAS_EMM_SER_SaveEmmSerCsfbAbortFlag(NAS_EMM_CSFB_ABORT_FLAG_INVALID);

    /* L单模不能发起紧急CSFB */
    if (NAS_EMM_SUCC != NAS_EMM_CheckMutiModeSupport())
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_FAILURE);
        return NAS_LMM_MSG_HANDLED;
    }

    /* 设置SERVICE发起原因为紧急CSFB, 用于给MMC上报SERVICE_RESULT_IND */
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ);

    /* 可能可以直接发起紧急CSFB流程, 根据当前所处状态进行不同处理*/
    if (NAS_EMM_SUCC == NAS_EMM_SER_VerifyCsfb(MM_LMM_CSFB_SERVICE_MO_EMERGENCY))
    {
        switch(NAS_EMM_CUR_MAIN_STAT)
        {
            case    EMM_MS_RRC_CONN_EST_INIT:
            case    EMM_MS_RRC_CONN_REL_INIT:
                NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmMoEmergencyCsfbNotifyMsgProc:High priority storage!");
                return NAS_LMM_STORE_HIGH_PRIO_MSG;

            case    EMM_MS_TAU_INIT:
            case    EMM_MS_SER_INIT:
            case    EMM_MS_AUTH_INIT:
            case    EMM_MS_RESUME:
                NAS_EMM_SER_LOG_NORM( "NAS_EMM_RcvMmMoEmergencyCsfbNotifyMsgProc:Low priority storage!");
                return NAS_LMM_STORE_LOW_PRIO_MSG;

            /*只有REG+NORMAL_SERVICE态可能直接发起紧急CSFB*/
            case    EMM_MS_REG:
                if (EMM_SS_REG_NORMAL_SERVICE == NAS_EMM_CUR_SUB_STAT)
                {
                    return NAS_LMM_MSG_DISCARD;
                }
                break;

            default:
                break;

        }
    }

    /* 必然不能直接发起或者状态不是上面所列则认为不能发起 */
    return NAS_EMM_UnableDirectlyStartMoEmergencyCsfbProc();

}

VOS_UINT32  NAS_EMM_PreProcMsgMmCsfbSerStartNotify( MsgBlock * pMsg )
{
    MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU *pstCsfbSerStartNotify = VOS_NULL_PTR;

    pstCsfbSerStartNotify = (MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU*)pMsg;

    /* 紧急CSFB的处理 */
    if (MM_LMM_CSFB_SERVICE_MO_EMERGENCY == pstCsfbSerStartNotify->enCsfbSrvType)
    {
        return NAS_EMM_RcvMmMoEmergencyCsfbNotifyMsgProc();
    }
    else  /* MO或MT的NORMAL CSFB的处理 */
    {
        return NAS_EMM_RcvMmNormalCsfbNotifyMsgProc(pstCsfbSerStartNotify->enCsfbSrvType);
    }
}


VOS_UINT32  NAS_EMM_MsRegSsNormalMsgMmCsfbSerStartNotify
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                             ulRslt                   = NAS_EMM_FAIL;
    MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU *pstCsfbSerStartNotify    = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsNormalMsgMmCsfbSerStartNotify entered.");

    pstCsfbSerStartNotify = (MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU *)pMsgStru;

    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsNormalMsgMmCsfbSerStartNotify ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*设置SER触发原因值*/
    if (MM_LMM_CSFB_SERVICE_MO_NORMAL == pstCsfbSerStartNotify->enCsfbSrvType)
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ);
    }
    else if (MM_LMM_CSFB_SERVICE_MO_EMERGENCY == pstCsfbSerStartNotify->enCsfbSrvType)
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ);
    }
    else
    {
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ);
    }

    /* 如果处于释放过程中，因底层处于未驻留状态，启动CSFB延时定时器，等收到系统消息时再考虑发起 */
    if(NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState())
    {
       NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_CSFB_DELAY);
       return NAS_LMM_MSG_HANDLED;
    }

    /* 设置UE接受CSFB */
    NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();
    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgMmCsfbSerStartNotify
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                             ulRslt                   = NAS_EMM_FAIL;
    MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU *pstCsfbSerStartNotify    = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsRegSsWaitAccessGrantIndMsgMmCsfbSerStartNotify entered.");

    pstCsfbSerStartNotify = (MM_LMM_CSFB_SERVICE_START_NOTIFY_STRU *)pMsgStru;

    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsRegSsWaitAccessGrantIndMsgMmCsfbSerStartNotify ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* MO CSFB不能发起 */
    if (MM_LMM_CSFB_SERVICE_MO_NORMAL == pstCsfbSerStartNotify->enCsfbSrvType)
    {
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        return NAS_LMM_MSG_HANDLED;
    }

    /* 此状态不可能收到紧急CSFB,预处理中已规避 */


    /* 设置SER启动原因为MT CSFB */
    NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ);

    /* 设置UE接受CSFB */
    NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

    /*启动定时器3417*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

    /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
    NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();
    return NAS_LMM_MSG_HANDLED;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



