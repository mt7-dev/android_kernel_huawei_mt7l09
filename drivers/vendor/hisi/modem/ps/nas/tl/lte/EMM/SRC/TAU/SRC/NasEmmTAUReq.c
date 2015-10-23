

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
/* lihong00150010 emergency tau&service begin */
#include "NasEmmAttDetInclude.h"
/* lihong00150010 emergency tau&service end */
#include "NasEmmTauInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMTAUREQ_C
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





VOS_VOID    NAS_EMM_TAU_StartTAUREQ(VOS_VOID)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_NORM( "NAS_EMM_TAU_StartTAUREQ is entered");

    /* 如果由于L无线能力变更发起的TAU，则不发起，转为本地DETACH重新ATTACH */
    if (NAS_LMM_UE_RADIO_CAP_LTE_CHG == NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
        {
            /*需要清理TAU和SER资源, 在下面的函数处理中将清理所有相关资源*/
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_TAU_StartTAUREQ:  LTE change, local detach and re-attach needed.");
            NAS_EMM_SomeStateRcvMsgSysInfoLteRadioCapChgCommProc();
            return;
    }

    /*启动定时器T3430*/
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3442);

    /* 清除UPDATE_MM标识 */
    /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_INVALID);*/

    /* DTS201202070220 增加是否有上行数据和信令pending的判断 */
    NAS_EMM_TAU_IsUplinkPending();

    /*转换EMM状态机MS_TAU_INIT+SS_TAU_WAIT_CN_TAU_CNF*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_TAU_INIT, EMM_SS_TAU_WAIT_CN_TAU_CNF, TI_NAS_EMM_STATE_TAU_T3430);

    /*组合并发送MRRC_DATA_REQ(TAU_REQ)*/
    if(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE ==
                                        NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        NAS_EMM_TAU_SendMrrcDataReq_TAUReq(       NAS_EMM_MSG_LOAD_BALANCING_TAU);
    }
    else
    {
        NAS_EMM_TAU_SendMrrcDataReq_TAUReq(       NAS_EMM_MSG_NOT_LOAD_BALANCING_TAU);
    }

    /* 发起TAU，将当前网络信息同步到Last尝试发起注册的网络信息中 */
    NAS_EMM_SaveLastAttemptRegTa();

    #if 0
    /* 切换MRRC上行直传消息的发送状态为READY 状态 */
    NAS_EMM_CHANGE_MRRC_SEND_STATE(     NAS_EMM_MRRC_SEND_STATE_READY);
    #endif

    return;
}
/*lint -e960*/
/*lint -e961*/
VOS_UINT32 NAS_EMM_TAU_IsCurrentTAInTaList(VOS_VOID)
{
    VOS_UINT32                          ulTARslt         = NAS_EMM_FAIL;
    NAS_MM_TA_STRU                      stPresentTa;
    NAS_MM_TA_LIST_STRU                 stLastTaiList;

    NAS_LMM_MEM_SET(&stLastTaiList,0,sizeof(NAS_MM_TA_LIST_STRU));

    /*获取当前的TA*/
    NAS_LMM_MEM_CPY(            &(stPresentTa.stPlmnId),
                            &(g_stEmmInfo.stNetInfo.stPresentNetId.stPlmnId),
                            sizeof(NAS_MM_PLMN_ID_STRU));

    NAS_LMM_MEM_CPY(            &(stPresentTa.stTac),
                            &(g_stEmmInfo.stNetInfo.stPresentNetId.stTac),
                            sizeof(NAS_MM_TAC_STRU));

    /*获取上次注册的TA List*/
    NAS_LMM_MEM_CPY(            &(stLastTaiList),
                            &(g_stEmmInfo.stNetInfo.stTaiList),
                            sizeof(NAS_MM_TA_LIST_STRU));

    /*判断当前TA是否在TA List中*/
    ulTARslt = NAS_LMM_TaMatchTaList(&stPresentTa, &stLastTaiList);

    /*如果新的TA在TA List中*/
    if(NAS_LMM_MATCH_SUCCESS == ulTARslt)
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_IsCurrentTAInTaList:TA is in the TAI List.");
        return NAS_EMM_SUCC;
    }
    /*TA不在TA List中，需要发起TAU流程*/
    else
    {
        NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_IsCurrentTAInTaList:TA is not in the TAI List,TAU Need.");
        return NAS_EMM_FAIL;
    }
}
VOS_UINT32 NAS_EMM_MsRegSsNormalMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo      = NAS_EMM_NULL_PTR;

    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_MsRegSsNormalMsgSysinfo is entered.");

    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    if(NAS_EMM_SUCC != ulRslt)
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsNormalMsgSysinfo:ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }
    /* lihong00150010 emergency tau&service begin */
    /*根据禁止信息判断是否处理当前的系统消息*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {
        NAS_EMM_SetTauTypeNoProcedure();

        /*转换EMM状态机*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                    EMM_SS_REG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);
        /* 如果当前CSFB延时定时器在运行，说明之前在释放过程中收到过CSFB，
           这种情况下直接报SERVICE失败触发MMC搜网去GU */
        if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_CSFB_DELAY))
        {
            /* 停止CSFB时延定时器 */
            NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_CSFB_DELAY);

            /* 给MMC上报SERVICE失败触发搜网去GU */
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        }
        if (MMC_LMM_CONN_IDLE != NAS_EMM_GetConnState())
        {
            NAS_EMM_TAU_LOG_INFO("Nas_Emm_MsRegSsNormalMsgSysinfo: Connected");

            /*发送NAS_EMM_REL_REQ*/
            NAS_EMM_RelReq(         NAS_LMM_NOT_BARRED);
        }
        return NAS_LMM_MSG_HANDLED;
    }
    /* lihong00150010 emergency tau&service end */

    /* 如果当前CSFB延时定时器在运行，说明之前在REG-NORMAL态释放过程中收到过CSFB，
       但是收到系统消息后又需要发起TAU，二者冲突的这种情况下优先发起TAU，之后再解决SERVICE的冲突 */
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_CSFB_DELAY))
    {
        /* 停止CSFB时延定时器 */
        NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_CSFB_DELAY);

        /* 设置流程冲突标志位，后面发起TAU时会因存在冲突自然携带active flag，
           SERVICE发起原因不必记录，在启动CSFB延时定时器时已经设置上了 */
        NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_SERVICE);
        NAS_EMM_MsRegSsNormalCsfbDelayProc();
    }
    else
    {
        NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc();
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgSysinfo
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo      = NAS_EMM_NULL_PTR;

    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgSysinfo is entered.");

    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(   EMM_MS_REG,
                                            EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                            pMsgStru);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    if(NAS_EMM_SUCC != ulRslt)
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgSysinfo:ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }
    /* lihong00150010 emergency tau&service begin */
    /*根据禁止信息判断是否处理当前的系统消息*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {
        /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_VALID);*/

        NAS_EMM_SetTauTypeNoProcedure();

        /*转换EMM状态机*/
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);

        if (MMC_LMM_CONN_IDLE != NAS_EMM_GetConnState())
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgSysinfo: Limited Srv Connected");

            /*发送NAS_EMM_REL_REQ*/
            NAS_EMM_RelReq(         NAS_LMM_NOT_BARRED);
        }
        return NAS_LMM_MSG_HANDLED;
    }
    /* lihong00150010 emergency tau&service end */
     /*判断TA发生变化，重置TAU ATTEMPT COUNTER,重新TAU*/
    if((EMMC_EMM_CHANGE_PLMN == pstsysinfo->ulChangeInfo)
      ||(EMMC_EMM_CHANGE_TA == pstsysinfo->ulChangeInfo))
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    }


    NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc();

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd( VOS_UINT32 ulCause )
{

    NAS_EMM_TAU_LOG_INFO(                         "NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd is entered.");

    /*发送内部消息 INTRA_CONN2IDLE_REQ,更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    /* 停T3411定时器 */
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

    /* 根据RRC携带的释放原因值做不同处理*/
    switch(ulCause)
    {
        case LRRC_LNAS_REL_CAUSE_LOAD_BALANCE_REQ:
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE);
            /*
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_PLMN_SEARCH,
                                           TI_NAS_EMM_STATE_NO_TIMER);
            */
             break;

        case LRRC_LNAS_REL_CAUSE_CONN_FAIL:
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
            /*
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_PLMN_SEARCH,
                                           TI_NAS_EMM_STATE_NO_TIMER);
            */
            break;

        default:
            /* 其他原因不会导致TAU，不处理 */;
            NAS_EMM_TAU_LOG1_INFO("NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd: ulCause = ", ulCause);
            break;

    }

    return;
}
VOS_UINT32  NAS_EMM_MsRegSsNmlSrvMsgAuthFail(
                                                    VOS_UINT32 ulMsgId,
                                                    VOS_VOID *pMsgStru)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU  *pMsgAuthFail =   (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                       ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsNmlSrvMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                     =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL      ==  ulCause)
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(        ulCause);
    }
    else
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd(      ulCause);
    }

    return NAS_LMM_MSG_HANDLED;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name  : NAS_EMM_MsRegSsLimitedSrvMsgAuthFail
 Discription    : MsRegSsLmtSrv状态下，收到网侧主动释放连接的消息;
                 在AUTH过程中,收到RRC_REL_IND或CN_AUTH_REJ,AUTH模块都会转发AUTH FAIL
                 所以在收到AUTH_FAIL时会处理RRC_REL_IND或CN_AUTH_REJ两种消息

 Input          : None
 Output         : None
 Return         :
 History:
      1.  lihong00150010        2012-12-29      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgAuthFail
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU  *pMsgAuthFail =   (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                       ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsLimitedSrvMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                     =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL      ==  ulCause)
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(        ulCause);
    }
    else
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd(      ulCause);
    }

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthFail
(
    VOS_UINT32 ulMsgId,
    VOS_VOID *pMsgStru
)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU  *pMsgAuthFail =   (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                       ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                     =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL      ==  ulCause)
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(        ulCause);
    }
    else
    {
        NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd(      ulCause);
    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsRegSsNmlSrvMsgRrcRelInd(
                                                    VOS_UINT32 ulMsgId,
                                                    VOS_VOID *pMsgStru)
{
    LRRC_LMM_REL_IND_STRU        *pMsgRrcRelInd  =    (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                  ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsNmlSrvMsgRrcRelInd is entered.");

    /*获得原因值*/
    ulCause                                    =    pMsgRrcRelInd->enRelCause;

    NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd(          ulCause);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRrcRelInd
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    LRRC_LMM_REL_IND_STRU        *pMsgRrcRelInd  = (LRRC_LMM_REL_IND_STRU *)pMsgStru;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRrcRelInd is entered.");

    /*发送内部消息 INTRA_CONN2IDLE_REQ,更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    /* 根据RRC携带的释放原因值做不同处理*/
    switch(pMsgRrcRelInd->enRelCause)
    {
        case LRRC_LNAS_REL_CAUSE_LOAD_BALANCE_REQ:

            /* 记录UPDATE_MM标识 */
            /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_VALID);*/
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE);
            /*
            NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                            EMM_SS_REG_PLMN_SEARCH,
                                            TI_NAS_EMM_STATE_NO_TIMER);
            */
            break;

        case LRRC_LNAS_REL_CAUSE_CONN_FAIL:

            /* 记录UPDATE_MM标识 */
            /* NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_VALID);*/
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
            /*
            NAS_EMM_TAUSER_FSMTranState(   EMM_MS_REG,
                                           EMM_SS_REG_PLMN_SEARCH,
                                           TI_NAS_EMM_STATE_NO_TIMER);
            */
            break;

        default:
            /* 其他原因不会导致TAU，不处理 */;
            NAS_EMM_TAU_LOG1_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgRrcRelInd: ulCause = ", pMsgRrcRelInd->enRelCause);
            break;

    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsRegSsNormalMsgT3411Exp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt  = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsNormalMsgT3411Exp is entered.");

    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    if(NAS_EMM_SUCC != ulRslt)
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsNormalMsgT3411Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*调用消息处理函数*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;

}
/*******************************************************************************
  Module   : REG+ATTEMPT_TO_UPDATE_MM状态下收到T3411超时
  Function : NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3411Exp
  Input    :
  Output   :
  NOTE     :
  Return   :
  History  :
    1.  lihong00150010  2011.11.17  新规作成
*******************************************************************************/
VOS_UINT32 NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3411Exp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt  = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3411Exp is entered.");

    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,pMsgStru);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    if(NAS_EMM_SUCC != ulRslt)
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3411Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_IMSI_ATTACH);*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

    /*调用消息处理函数*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;

}

/*******************************************************************************
  Module   : REG+ATTEMPT_TO_UPDATE_MM状态下收到T3402时
  Function : NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3402Exp
  Input    :
  Output   :
  NOTE     :
  Return   :
  History  :
    1.  lihong00150010  2011.11.17  新规作成
*******************************************************************************/
VOS_UINT32 NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3402Exp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                                  ulRslt          = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3402Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3402Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_IMSI_ATTACH);*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

    /*调用消息处理函数*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;

}
VOS_UINT32 NAS_EMM_MsRegSsNormalMsgT3412Exp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt  = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsNormalMsgT3412Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt)
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsNormalMsgT3412Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 触发周期性TAU
       注:Reg.Normal_Service状态下收到T3412Exp说明没有业务请求，不判断上行阻塞*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_T3412EXP);
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3412Exp
(
    VOS_UINT32                              ulMsgId,
    VOS_VOID                               *pMsgStru
)
{
    VOS_UINT32                          ulRslt  = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3412Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt)
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgT3412Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 3412的时长一般情况下比3411和3402都长，因此此场景发生的概率很小，目前简化
       处理，不发起周期性TAU，而是直接发起联合TAU */
    /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsSuspendSsAnyMsgT3412Exp
 Description     : 挂起过程中3412超时，只记录超时。
                   若系统变换回退，进入NO CELL后，收到系统消息时使用此标识；
                   若变换到GU去，清除此超时标识
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-5-11  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsSuspendSsAnyMsgT3412Exp(
                                            VOS_UINT32  ulMsgId,
                                      const VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsSuspendSsAnyMsgT3412Exp: entered.");


    /*  仿照如下设置赋值*/
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsRegSsAtpUpdataMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt            = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo        = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                      stCurTa           = {0};
    NAS_LMM_PTL_TI_ENUM_UINT16          enPtlTimerId    = NAS_LMM_PTL_TI_BUTT;

    /* 获取当前TA和上次尝试注册的TA信息 */
    NAS_EMM_GetCurrentTa(&stCurTa);
    pstLastAttmpRegTa                   = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_MsRegSsAtpUpdataMsgSysinfo is entered.");

    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE,pMsgStru);

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    if ( NAS_EMM_SUCC != ulRslt)
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsAtpUpdataMsgSysinfo ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }
    /* lihong00150010 emergency tau&service begin */
    /*根据禁止信息判断是否处理当前的系统消息*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_SetTauTypeNoProcedure();

        /*转换EMM状态机*/
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);


        return NAS_LMM_MSG_HANDLED;
    }

    /* 如果当前TA在TAI LIST */
    if (NAS_EMM_SUCC == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        /* 如果有定时器(保护作用，正常一定有) */
        if (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
        {
            /* 当前TA与上次尝试发起注册或TAU的TA等效，等超时 */
            if (NAS_EMM_YES == NAS_LMM_IsLastAttemptRegTaInTaiList())
            {
                NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsAtpUpdataMsgSysinfo: Equivalent TA, wait timer expired.");
                return NAS_LMM_MSG_HANDLED;
            }
        }

        /* 非等效TA，或者没定时器，都立即发起TAU */
        /* 重置TAU计数 */
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

        /*启动TAU Procedure*/
        NAS_EMM_TAU_StartTAUREQ();

    }

    /* 当前TA不在TAI LIST */
    else
    {
        /* 与上次尝试注册的TA相同 && 有定时器 && EU值不为EU1 ，等超时;  否则立即发起TAU */
        if(    (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatch(pstLastAttmpRegTa, &stCurTa))
            && (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
            && (EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat()) )
        {
            return NAS_LMM_MSG_HANDLED;
        }
        else
        {
            /* 重置TAU计数 */
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

            /*启动TAU Procedure*/
            NAS_EMM_TAU_StartTAUREQ();
        }
    }

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32 NAS_EMM_MsRegSsAtpUpdataMsgT3411Exp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                                  ulRslt          = NAS_EMM_FAIL;


    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_NORM("Nas_Emm_MsRegSsAtpUpdataMsgT3411Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*重新发起TAU*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32 NAS_EMM_MsRegSsAtpUpdataMsgT3402Exp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                                  ulRslt          = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_MsRegSsAtpUpdataMsgT3402Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*调用消息处理函数*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;

}

/* lihong00150010 emergency tau&service begin */

VOS_UINT32 NAS_EMM_MsRegSsSomeStateMsgEsmDataReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt1          = NAS_EMM_FAIL;
    VOS_UINT32                          ulRslt2          = NAS_EMM_FAIL;

    EMM_ESM_DATA_REQ_STRU              *pstEsmDataReq   = (EMM_ESM_DATA_REQ_STRU*)pMsgStru;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsRegSsSomeStateMsgEsmDataReq is entered.");

/* 此处因多处调用不能只用一个检查状态 */
    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt1 = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE,pMsgStru);
    ulRslt2 = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);

    if (( NAS_EMM_SUCC != ulRslt1 ) && ( NAS_EMM_SUCC != ulRslt2 ))
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsRegSsAtpUpdataMsgEsmDataReq ERROR !");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果不是紧急类型，则丢弃 */
    if (VOS_TRUE != pstEsmDataReq->ulIsEmcType)
    {
        NAS_EMM_TAU_LOG_NORM( "NAS_EMM_MsRegSsSomeStateMsgEsmDataReq:NOT EMC!");
        return  NAS_LMM_MSG_DISCARD;
    }

    NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_TRUE);

    /* 缓存紧急类型的ESM消息 */
    NAS_EMM_SaveEmcEsmMsg(pMsgStru);

    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ);

    /*调用消息处理函数*/
    NAS_EMM_TAU_StartTAUREQ();

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */


VOS_UINT32 NAS_EMM_MsRegSsLimitSRMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo      = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsLimitSRMsgSysinfo entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_LIMITED_SERVICE,pMsgStru);
    if (NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsLimitSRMsgSysinfo:INPUT ERROR.");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 获取系统消息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
    /* lihong00150010 emergency tau&service begin */
    /* 如果系统消息被禁，状态不变，函数返回*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsLimitSRMsgSysinfo:Cell Forb.");

        NAS_EMM_SetTauTypeNoProcedure();

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);

        return NAS_LMM_MSG_HANDLED;
    }

    /* 进入收到SYS_INFO的公共处理*/
    NAS_EMM_RegSomeStateMsgSysInfoCommProc();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsRegSsPLMNSearchMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt          = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo      = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_INFO(               "Nas_Emm_MsRegSsPLMNSearchMsgSysinfo is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_PLMN_SEARCH,pMsgStru);
    if ( NAS_EMM_SUCC                   != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN(           "Nas_Emm_MsRegSsPLMNSearchMsgSysinfo ERROR.");
        return NAS_LMM_MSG_DISCARD;
    }

    /*获取系统消息和网络信息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
    /* lihong00150010 emergency tau&service begin */
    /*如果收到的系统消息被禁，状态迁移到Reg.Limite_Service，函数返回*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsPLMNSearchMsgSysinfo:SysInfo Forb.");

        NAS_EMM_SetTauTypeNoProcedure();

        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);


        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_RegSomeStateMsgSysInfoCommProc();

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsRegSsNocellMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{

    VOS_UINT32  ulRslt                            = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU   *pstsysinfo       = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_INFO(                         "Nas_Emm_MsRegSsNocellMsgSysinfo is entered.");

    /*check the input params*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(         EMM_MS_REG,EMM_SS_REG_NO_CELL_AVAILABLE,pMsgStru);
    if( NAS_EMM_SUCC                              != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN(                     "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果当前处于挂起态，则不处理SYS_INFO,直接丢弃*/
    if(NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
    {
        NAS_EMM_TAU_LOG_ERR("Nas_Emm_MsRegSsNocellMsgSysinfo:LTE SUSPEND.");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 获取系统消息和网络信息*/
    pstsysinfo                                    = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    /* lihong00150010 emergency tau&service begin */
    /*根据禁止信息判断是否处理当前的系统消息*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegSsNocellMsgSysinfo:Cell Forb.");

        NAS_EMM_SetTauTypeNoProcedure();

        /*转换EMM状态机*/
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);

        return NAS_LMM_MSG_HANDLED;
    }
    /*inform ESM that return to E-UTRAN*/
    NAS_EMM_TAU_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_AREA_RE_ENTRY);

    /* 进入收到SYS_INFO的公共处理*/
    NAS_EMM_RegSomeStateMsgSysInfoCommProc();

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service begin */



VOS_VOID NAS_EMM_MsRegSsNormalCsfbDelayProc(VOS_VOID)
{

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsNormalCsfbDelayProc is entered.");

    NAS_EMM_RegNormalServiceOrUpdateMMMsgSysInfoCommProc();

    /* 保持NORMAL态不变的情况 */
    /* 如果当前CSFB延时定时器在运行，说明之前在释放过程中收到过CSFB，
       这种情况下直接发起EXTEND SERVICE */
    if((EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
        &&(EMM_SS_REG_NORMAL_SERVICE ==NAS_LMM_GetEmmCurFsmSS()))
    {
        /* 设置UE接受CSFB */
        NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

        /*启动定时器3417*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
        NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();

        /* 清除之前记录的SERVICE冲突 */
        NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_NONE);
    }
    /* 如果状态不再是REG NORMAL，说明发起了TAU或其他流程，完成其它流程再解决冲突 */

    return;

}

#if 0
/*****************************************************************************
  Function Name   : NAS_EMM_ProcTauBarIsTimerRunning
  Description     :
  Input           : None
  Output          : None
  Return          : VOS_UINT32

  History         :
     1.FTY         2012-02-25  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_EMM_ProcTauBarIsTimerRunning(VOS_VOID)
{
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_CheckTimerRunning:3411 is running.");

        NAS_EMM_TAU_TimerRunningStateChng(TI_NAS_EMM_PTL_T3411);

        return NAS_EMM_YES;
    }

    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_CheckTimerRunning:3402 is running.");

        NAS_EMM_TAU_TimerRunningStateChng(TI_NAS_EMM_STATE_NO_TIMER);

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_EMM_ProcTauBar( VOS_VOID )
{
    /* 检查BarType，如果全部解除，则迁移到REG.NORMAL_SERVICE*/
    if(0 == (NAS_EMM_TAU_GetRegBarType()&NAS_LMM_LOW_HALF_BYTE_F))
    {
        /*清空BarProcedure*/
        NAS_EMM_TAU_SetRegBarProcedure(NAS_EMM_BAR_PROCEDURE_NULL);
        /* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStatePsNormalServiceOrPsLimitService();

        /*转换EMM状态机Reg Normal态*/
        /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/
    }   /* lihong00150010 emergency tau&service end */

    /*如果MO Signal没有被bar，则发起TAU*/
    if( NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        NAS_EMM_RegSomeStateMsgSysInfoCommProc();
    }
}

/* lihong00150010 emergency tau&service begin */
#if 0
/*****************************************************************************
 Function Name   : NAS_EMM_SrvBarIsTauStarted
 Description     : Service流程被Bar,收到Grant_Ind消息，判断如果需要则触发携带
                   'ACTIVE'的TAU时，对定时器的判断
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY         2012-02-25  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_SrvBarIsTimerRunning( VOS_VOID)
{
    /*  T3411在运行，不需要向MMC发TAU结果
       T3402在运行，需要向MMC发TAU结果，触发MMC搜下一个网络*/
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_SrvBarIsTauStarted:T3411 is running");

        NAS_EMM_TAU_TimerRunningStateChng(TI_NAS_EMM_PTL_T3411);

        return NAS_EMM_YES;
    }

    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_SrvBarIsTauStarted:T3402 is running");

        NAS_EMM_TAU_TimerRunningStateChng(TI_NAS_EMM_PTL_T3402);

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}
#endif
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_SrvBarIsTauStarted( VOS_VOID)
{
    /*如果Mo_Signal允许接入，判断是否需要触发TAU*/
    if(NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        if (NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        {
            NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_SERVICE);
        }

        /*判断是否需要发起TAU或者本地Detach*/
        NAS_EMM_RegSomeStateMsgSysInfoCommProc();

        if (EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
        {
            NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_NONE);
            return NAS_EMM_NO;
        }
        else
        {
            /* 如果需要发起TAU或者无线能力变更引起的本地Detach后重新Attach,则返回NAS_EMM_YES */
            return NAS_EMM_YES;
        }
    }
    return NAS_EMM_NO;

}
VOS_VOID  NAS_EMM_ProcSrvBar( VOS_VOID )
{
    VOS_UINT32                          ulRslt;
    /* 检查BarType，如果全部解除，则迁移到REG.NORMAL_SERVICE*/
    if(0 == (NAS_EMM_TAU_GetRegBarType()&NAS_LMM_LOW_HALF_BYTE_F))
    {
        /*清空BarProcedure*/
        NAS_EMM_TAU_SetRegBarProcedure(NAS_EMM_BAR_PROCEDURE_NULL);
        /* lihong00150010 emergency tau&service begin */
        NAS_EMM_TranStatePsNormalServiceOrPsLimitService();

        /*转换EMM状态机Reg Normal态*/
        /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/
    }/* lihong00150010 emergency tau&service end */

    /*判断如果需要则触发携带'active'的TAU流程，函数返回，否则继续*/
    ulRslt = NAS_EMM_SrvBarIsTauStarted();
    if(NAS_EMM_YES == ulRslt)
    {
        return;
    }
    /* lihong00150010 emergency tau&service begin */
    /*如果MT call被bar，并且Service目的响应寻呼，则发起Service流程*/
    if( (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MT))
       && (NAS_EMM_SER_START_CAUSE_RRC_PAGING == NAS_EMM_SER_GetEmmSERStartCause())
       && (VOS_TRUE != NAS_EMM_IsEnterRegLimitService()))
    {/* lihong00150010 emergency tau&service end */
        /*触发Service流程*/
        NAS_EMM_SER_RcvRrcStmsiPagingInd();
        return;
    }

    /*如果MT call不被bar，并且EXTENDED Service目的响应寻呼，则发起EXTENDED Service流程*/
    if( (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MT))
       &&(NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause()))
    {
        /* 设置UE接受CSFB */
        NAS_EMM_SER_SaveEmmSerCsfbRsp(NAS_EMM_CSFB_RSP_ACCEPTED_BY_UE);

        /*启动定时器3417*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
        NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();
        return;
    }

     /*如果MO csfb不被bar，并且CSFB没有abort，则发起Service流程*/
    if ( (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_CSFB))
       &&(NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
       && (NAS_EMM_CSFB_ABORT_FLAG_VALID != NAS_EMM_SER_GetEmmSerCsfbAbortFlag()))
    {
        /*启动定时器3417*/
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
        NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();
    }
    return;
}



VOS_UINT32 NAS_EMM_MsRegSsWaitAccessGrantIndMsgRrcGrantInd(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
 {
    VOS_UINT32                              ulRslt;

    LRRC_LMM_ACCESS_GRANT_IND_STRU            *pRrcMmGrantInd = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsWaitAccessGrantIndMsgRrcGrantInd is entered.");

    /*check the input params and states*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*get the msg info*/
    pRrcMmGrantInd = (LRRC_LMM_ACCESS_GRANT_IND_STRU *) pMsgStru;

    NAS_EMM_TAU_LOG1_INFO("NAS_EMM_MsRegSsWaitAccessGrantIndMsgRrcGrantInd: Granttype =.",pRrcMmGrantInd->enAccessGrantType);

    /* 清除bar类型*/
    NAS_EMM_ClearBarType(pRrcMmGrantInd->enAccessGrantType);

    /* 调用REG态下解Bar的公共处理*/
    NAS_EMM_RegBarCommProc();

    return NAS_LMM_MSG_HANDLED;
 }


VOS_VOID  NAS_EMM_RegBarCommProc( VOS_VOID )
{
    /* 根据BarProcedure进入不同的处理*/
    if (NAS_EMM_BAR_PROCEDURE_TAU == NAS_EMM_GetBarProcedure())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegBarCommProc:TAU BAR");
        NAS_EMM_ProcTauBar();
    }
    else if (NAS_EMM_BAR_PROCEDURE_SERVICE == NAS_EMM_GetBarProcedure())
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_RegBarCommProc:SERVICE BAR");
        NAS_EMM_ProcSrvBar();
    }
    else
    {
        ;
    }

    return;
}
VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgSysinfoInd( VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt;
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo = NAS_EMM_NULL_PTR;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsWaitAccessGrantIndMsgSysinfoInd entered.");
    (VOS_VOID)ulMsgId;

    /* 入参检查*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果系统消息被禁，则清空Bar信息，迁移到 Reg.Limite_Service*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
    /* lihong00150010 emergency tau&service begin */
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_SetTauTypeNoProcedure();

        /* 清空Bar信息*/
        NAS_EMM_ClearBarResouce();

        /* 状态迁移: Reg.Limite_Service*/
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgIntraTauReq( VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                               ulRslt;
    NAS_LMM_INTRA_TAU_REQ_STRU         *pIntraTauReq = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    pIntraTauReq    = (NAS_LMM_INTRA_TAU_REQ_STRU*)pMsgStru;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgIntraTauReq is entered.");

    /*check the input params and states*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }
    if(NAS_LMM_INTRA_TAU_TYPE_PARA_CHANGE == pIntraTauReq->enIntraTauType)
    {
        /*设置DRX或NetCapability改变标志*/
        NAS_LMM_SetEmmInfoDrxNetCapChange( NAS_EMM_YES);
    }

    else if(NAS_LMM_INTRA_TAU_TYPE_IMSI_ATTACH == pIntraTauReq->enIntraTauType)
    {
        if (NAS_EMM_SUCC == NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
        {
            NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsRegSsWaitAccessGrantIndMsgIntraTauReq TAU_IMSI_ATTACH MO SIGNAL!!");
            NAS_EMM_AppSendAttRstInTau(MMC_LMM_ATT_RSLT_FAILURE);
            return NAS_LMM_MSG_HANDLED;
        }
    }
    else
    {

    }

    if (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgIntraTauReq:TAU is start");

        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgBearerStatusReq(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    VOS_UINT32                               ulRslt;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgBearerStatusReq entered.");

    /* 检查当前状态和入参*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 设置承载上下文变化*/
    NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);

    /* 判断如果MO_Signal允许接入，则立即触发TAU，携带'ACTIVE'*/
    if (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgBearerStatusReq:Init TAU");

        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        /* 如果存在未完成的LOAD_BALANCE TAU，则触发LOAD_BALANCE类型TAU
           否则，触发承载上下文改变触发的TAU*/

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgT3412Exp(  VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{
    VOS_UINT32                               ulRslt;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgT3412Exp is entered.");

     /*check the input params and states*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*leili modify for isr begin*/

    /*设置周期TAU定制器超时标志*/
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);

    if (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType())
    {
        /*启动定时器T3423*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3423);

        /*通知GU模T3423启动*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3423, LMM_MMC_TIMER_START);

    }
    /*leili modify for isr end*/

    if (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgT3412Exp:TAU is start");

        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        /*触发周期性TAU*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_T3412EXP);
        NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegSsWaitAccessGrantIndMsgT3411Exp( VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    VOS_UINT32                               ulRslt;

   (VOS_VOID)ulMsgId;
   NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsWaitAccessGrantIndMsgT3411Exp entered.");


    /*状态和入参检查*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_WAIT_ACCESS_GRANT_IND,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*如果MO_SIGNAL允许接入，则检查是否有未完成的TAU*/
    if (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        /*如果有未完成的TAU则重新触发，且携带'ACTIVE'标志*/
        if(NAS_EMM_YES == NAS_EMM_SER_AbnormalNeedtoContinueTau())
        {
            NAS_EMM_TAU_SaveEmmCollisionCtrl(    NAS_EMM_COLLISION_SERVICE);
            NAS_EMM_TAU_StartTAUREQ();
        }
    }
    else
    {
        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
            NAS_EMM_TAU_SaveTAUtype(NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI);

            ulTauRslt = MMC_LMM_TAU_RSLT_ACCESS_BARED;
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);
        }
        #if 0
        if (NAS_EMM_UPDATE_MM_FLAG_VALID == NAS_LMM_GetEmmInfoUpdateMmFlag())
        {
            NAS_EMM_TAU_SaveTAUtype(NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI);

            ulTauRslt = MMC_LMM_TAU_RSLT_ACCESS_BARED;
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

        }
        #endif
    }
    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsDeRegInitSsWaitCNDetachCnfMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    VOS_UINT32                          ulTaRslt    = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo  = NAS_EMM_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU           *pMmNetInfo  = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_NORM("Nas_Emm_MsDeRegInitSsWaitCNDetachCnfMsgSysinfo entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt)
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsDeRegInitSsWaitCNDetachCnfMsgSysinfo ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    pstsysinfo = (EMMC_EMM_SYS_INFO_IND_STRU *)pMsgStru;

    /* 判断TA是否在TA List中*/
    ulTaRslt = NAS_EMM_TAU_IsCurrentTAInTaList();
	/* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_GLO_AD_GetUsimPullOutFlag())
    {/* lihong00150010 emergency tau&service end */
         if (NAS_EMM_FAIL == ulTaRslt)
         {
             /*停止定时器T3421*/
             NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3421);


             /*向ESM发送ID_EMM_ESM_STATUS_IND消息*/
             NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);

             NAS_EMM_ProcLocalNoUsim();

             /* 通知APP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
             NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);
             NAS_EMM_ClearAppMsgPara();

             NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
         }
         return NAS_LMM_MSG_HANDLED;
    }

    /* 获取系统消息 + 当前网络信息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();
	/* lihong00150010 emergency tau&service begin */
    /* 如果系统消息被禁，则释放连接,本地完成DETACH,进入Dereg.Limite_Service*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsDeRegInitSsWaitCNDetachCnfMsgSysinfo: Cell Forb.");

        /* 终止DETACH流程,释放连接,注意:给RRC发送REL_REQ消息和状态转换的顺序不能颠倒*/
        NAS_EMM_DetProcEnterForbTA();

        return  NAS_LMM_MSG_HANDLED;
    }

    if (NAS_EMM_FAIL == ulTaRslt)
    {
         NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsDeRegInitSsWaitCNDetachCnfMsgSysinfo:TA not in TAI List.");

         /* 设置流程冲突标志位DETACH*/
         NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_DETACH);

         /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
         NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

         /* 如果是DETACH类型为EPS/IMSI，则设置联合DETACH被TAU打断标识，这样
            发起的TAU类型为联合TAU */
         if (MMC_LMM_MO_DET_CS_PS == NAS_EMM_GetDetachType())
         {
            /* 设置联合DETACH被TAU打断标识，这样后续发起的TAU类型仍然为联合TAU */
            NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_YES);
         }

         /* 调用DETACH的函数，结束DETACH流程*/
         NAS_EMM_DetachToTAU();

         /* 启动TAU Procedure
            注:MO_DETACH流程中触发TAU，因为DETACH_COLLISION标志，必然携带ACTVIE,
               不判断上行阻塞*/
             NAS_EMM_TAU_StartTAUREQ();
    }
    /* TA在TAI List中，更新 L.V.R TAI*/
    else
    {
        NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
		/* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
		/* lihong00150010 emergency tau&service end */
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgSysinfo
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    VOS_UINT32                          ulTaRslt    = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo  = NAS_EMM_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU           *pMmNetInfo  = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgSysinfo entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt)
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgSysinfo ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    pstsysinfo = (EMMC_EMM_SYS_INFO_IND_STRU *)pMsgStru;

    /* 判断TA是否在TA List中*/
    ulTaRslt = NAS_EMM_TAU_IsCurrentTAInTaList();

    /* 获取系统消息 + 当前网络信息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();
	/* lihong00150010 emergency tau&service begin */
    /* 如果系统消息被禁，则释放连接,本地完成DETACH,进入Dereg.Limite_Service*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgSysinfo: Cell Forb.");

        /* 终止DETACH流程，根据是否存在紧急呼承载，决定是否释放链路 */
        NAS_EMM_ImsiDetProcEnterForbTA();

        return  NAS_LMM_MSG_HANDLED;
    }

    if (NAS_EMM_FAIL == ulTaRslt)
    {
         NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgSysinfo:TA not in TAI List.");

         /* 设置流程冲突标志位DETACH*/
         NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_DETACH);

         /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
         NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);

         /* 设置联合DETACH被TAU打断标识，这样后续发起的TAU类型仍然为联合TAU */
         NAS_EMM_TAU_SetEmmCombinedDetachBlockFlag(NAS_EMM_COM_DET_BLO_YES);

         /* 调用DETACH的函数，结束DETACH流程*/
         NAS_EMM_DetachToTAU();

         /* 启动TAU Procedure
            注:MO_DETACH流程中触发TAU，因为DETACH_COLLISION标志，必然携带ACTVIE,
               不判断上行阻塞*/
         NAS_EMM_TAU_StartTAUREQ();
    }
    /* TA在TAI List中，更新 L.V.R TAI*/
    else
    {
        NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
		/* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
		/* lihong00150010 emergency tau&service end */
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsTauInitSsWaitCNCnfMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo  = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulTaRslt    = NAS_EMM_FAIL;
    NAS_LMM_NETWORK_INFO_STRU           *pMmNetInfo = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_MsTauInitSsWaitCNCnfMsgSysinfo is entered.");

    /* 获取EMM维护的网络信息*/
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsTauInitSsWaitCNCnfMsgSysinfo ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 获取系统消息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;
	/* lihong00150010 emergency tau&service begin */
    /* 如果系统消息被禁,终止TAU流程，释放连接*/
    if ((NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        && ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)))
    {
        /* 终止TAU流程*/
        NAS_LMM_StopStateTimer(          TI_NAS_EMM_STATE_TAU_T3430);

        if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfMsgSysinfo:CAUSE_ESM_EMC_PDN_REQ");

            NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndForbType,
                                                    (VOS_VOID*)&pstsysinfo->ulForbiddenInfo,
                                                    EMM_SS_DEREG_LIMITED_SERVICE);
        }
        else
        {
            /* 状态迁移*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                            EMM_SS_REG_LIMITED_SERVICE,
                            TI_NAS_EMM_STATE_NO_TIMER);

            /*向MMC发送LMM_MMC_TAU_RESULT_IND消息在冲突处理中执行*/
            NAS_EMM_TAU_ProcEnterForbTA(pstsysinfo->ulForbiddenInfo);
        }
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

        return NAS_LMM_MSG_HANDLED;
    }

    /* 如果TA发生变化，先释放连接，再重新触发TAU*/
    if ((EMMC_EMM_CHANGE_PLMN == pstsysinfo->ulChangeInfo)
      ||(EMMC_EMM_CHANGE_TA == pstsysinfo->ulChangeInfo))
    {
        /*设置EPS状态为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;
		/* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
		/* lihong00150010 emergency tau&service end */
        /*停止T3430定时器*/
        NAS_LMM_StopStateTimer(              TI_NAS_EMM_STATE_TAU_T3430);


        /* TauAttemptCount清零*/
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
		/* lihong00150010 emergency tau&service begin */
        /* 如果TA不在TAI List中，修改TAU启动原因，触发TA-Updating TAU,
           如果TA在TAI List中，则不做处理，TAU启动原因不变*/
        ulTaRslt = NAS_EMM_TAU_IsCurrentTAInTaList();
        if ((NAS_EMM_FAIL == ulTaRslt)
            && (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ != NAS_EMM_TAU_GetEmmTAUStartCause()))
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }
		/* lihong00150010 emergency tau&service end */
        /* 重新触发TAU*/
        NAS_EMM_TAU_StartTAUREQ();
    }/* lihong00150010 emergency tau&service begin */
    else
    {
        /*设置L.V.R TAI*/
        NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);

        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
    }
	/* lihong00150010 emergency tau&service end */
    return NAS_LMM_MSG_HANDLED;
}

VOS_VOID NAS_EMM_ProcHoWaitSysInfoBufferMsg( VOS_VOID )
{
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRrcMmDataReqMsgLen;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulCurSaveMsgNum = 0;
    VOS_UINT32                          ulMmRrcOpId;

    ulCurSaveMsgNum = g_ulSaveMsgNum;

    pEmmMrrcMgmtData                    = NAS_EMM_GET_MRRC_MGMT_DATA_ADDR();
    ulRrcMmDataReqMsgLen = (sizeof(NAS_EMM_MRRC_DATA_REQ_STRU)+
                                   pEmmMrrcMgmtData->ulNasMsgLength) -
                                   NAS_EMM_4BYTES_LEN;
    /* 申请消息内存*/
    pMrrcDataMsg = (NAS_EMM_MRRC_DATA_REQ_STRU *)(VOS_VOID*)NAS_LMM_MEM_ALLOC(ulRrcMmDataReqMsgLen);

    if(VOS_NULL_PTR == pMrrcDataMsg)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_ProcHoWaitSysInfoBufferMsg: Mem Alloc Fail");
        return;
    }

    /* 查找HO WAIT SYS INFO 缓存记录 */
    for (ulIndex = 0; ulIndex < ulCurSaveMsgNum; ++ulIndex)
    {
        if (VOS_TRUE == pEmmMrrcMgmtData->ulHoWaitSysInfoFlag)
        {
            /* 重发消息之后，当前指针指向的消息可能已经改变，所以需要将当前消息的OPID保存，
            用于后续清缓存使用*/
            ulMmRrcOpId = pEmmMrrcMgmtData->ulMmRrcOpId;

            if (NAS_EMM_MSG_SERVICE_REQ == pEmmMrrcMgmtData->enLastMsgId)
            {
                /* 重发SERVICE或者EXTENDED SERVICE REQ消息 */
                pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

                pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_SERVICE_REQ;
                pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
                pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

                pMrrcDataMsg->stNasMsg.ulNasMsgSize = pEmmMrrcMgmtData->ulNasMsgLength;

                /* 构造MRRC_DATA_REQ 中的 NAS_MSG_STRU,即CN消息(Compose the msg of): SER_REQ */
                NAS_EMM_SER_CompCnServiceReq(&(pMrrcDataMsg->stNasMsg));

                NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);

            }
            else if (NAS_EMM_MSG_EXTENDED_SERVICE_REQ == pEmmMrrcMgmtData->enLastMsgId)
            {
                /* 重发EXTENDED SERVICE REQ消息 */
                pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

                pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_EXTENDED_SERVICE_REQ;
                pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
                pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

                /*组装EXTENDED SERVICE REQ消息*/
                NAS_EMM_CompCnExtendedSerReqMoNasMsg(pMrrcDataMsg->stNasMsg.aucNasMsg, &ulIndex);

                pMrrcDataMsg->stNasMsg.ulNasMsgSize = ulIndex;

                NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);
            }
            else if (NAS_EMM_MSG_TAU_CMP == pEmmMrrcMgmtData->enLastMsgId)
            {
                pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

                pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_TAU_CMP;
                pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
                pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

                pMrrcDataMsg->stNasMsg.ulNasMsgSize = pEmmMrrcMgmtData->ulNasMsgLength;

                NAS_LMM_MEM_CPY(pMrrcDataMsg->stNasMsg.aucNasMsg,
                                 pEmmMrrcMgmtData->aucNasMsgContent,
                                 pEmmMrrcMgmtData->ulNasMsgLength);

                NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);

            }
            else
            {

            }

            NAS_EMM_ClrDataReqBufferMsg(ulMmRrcOpId);
        }
        else
        {
            pEmmMrrcMgmtData++;
        }

    }
    /*释放所申请内存 */
    NAS_LMM_MEM_FREE(pMrrcDataMsg);
    return;
}

/* lihong00150010 emergency tau&service begin */

VOS_VOID NAS_EMM_MsSerInitSsWaitCNCnfMsgSysInfoCommProc( VOS_VOID )
{
    VOS_UINT32                          ulTaRslt    = NAS_EMM_FAIL;
    NAS_LMM_NETWORK_INFO_STRU           *pMmNetInfo = NAS_EMM_NULL_PTR;

    /* 获取EMM维护的网络信息*/
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();

    /*判断TA不在TA List中，需要发起TAU Procedure,TA在上次注册的TA List中，忽略对当前系统消息的处理*/
    ulTaRslt = NAS_EMM_TAU_IsCurrentTAInTaList();
    if (NAS_EMM_FAIL == ulTaRslt)
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCNCnfMsgSysInfoCommProc:TA not in TAI LIST.");

        /*停止T3417定时器*/
        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

        /*停止T3417ext定时器*/
        NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

        if (VOS_TRUE != NAS_EMM_SER_IsCsfbProcedure())
        {
            /*Inform RABM that SER fail*/
            NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
        }

        /* 清除ulHoWaitSysInfoFlag标识存在的缓存记录 */
        (VOS_VOID)NAS_EMM_ClrHoWaitSysInfoBufferMsg();

        /*设置流程冲突标志位*/
        NAS_EMM_TAU_SaveEmmCollisionCtrl(NAS_EMM_COLLISION_SERVICE);

        if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ);
        }
        else
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }

        /*启动TAU Procedure*/
        NAS_EMM_TAU_StartTAUREQ();
    }
    else
    {
        /*设置L.V.R TAI*/
        NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
        NAS_EMM_ProcHoWaitSysInfoBufferMsg();

        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
    }
}


VOS_VOID NAS_EMM_MsSerInitSsWaitCNCnfMsgForbiddenAnycellSysinfoProc( VOS_VOID )
{
    if (NAS_LMM_REG_STATUS_NORM_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        if ((NAS_EMM_SER_START_CAUSE_RRC_PAGING == NAS_EMM_SER_GetEmmSERStartCause())
            || (NAS_EMM_SER_START_CAUSE_RABM_REEST_EMC == NAS_EMM_SER_GetEmmSERStartCause()))
        {
            NAS_EMM_MsSerInitSsWaitCNCnfMsgSysInfoCommProc();
            return ;
        }

        if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        {
            /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
            NAS_EMM_MmcSendSerResultIndOtherType( MMC_LMM_SERVICE_RSLT_FAILURE);
        }

        /* 终止SER流程*/
        NAS_EMM_SER_AbnormalOver();

        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* 清空Service模块*/
        NAS_EMM_SER_ClearResource();

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return ;
    }

    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
        NAS_EMM_MmcSendSerResultIndOtherType( MMC_LMM_SERVICE_RSLT_FAILURE);
    }

    /* 终止SER流程*/
    NAS_EMM_SER_AbnormalOver();

    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCNCnfMsgForbiddenAnycellSysinfoProc: CAUSE_ESM_DATA_REQ_EMC.");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_LIMITED_SERVICE);
    }
    else
    {
        /* 状态迁移:Reg.Limite_Service*/
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }

    /* 清空Service模块*/
    NAS_EMM_SER_ClearResource();

    NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
}
/* lihong00150010 emergency tau&service end */
/* lihong00150010 emergency tau&service begin */

VOS_UINT32 NAS_EMM_MsSerInitSsWaitCNCnfMsgSysinfo(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{

    VOS_UINT32                          ulRslt       = NAS_EMM_FAIL;
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo   = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCNCnfMsgSysinfo entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsSerInitSsWaitCNCnfMsgSysinfo ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 获取系统消息*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    /* 如果系统消息被禁，终止SER流程，释放连接*/
    if ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
            || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_MsSerInitSsWaitCNCnfMsgSysinfo:CELL FORB.");

        NAS_EMM_MsSerInitSsWaitCNCnfMsgForbiddenAnycellSysinfoProc();

        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_MsSerInitSsWaitCNCnfMsgSysInfoCommProc();

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32 NAS_EMM_MsRegSsNormalServiceMsgIntraTauReq(VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru)
{

    NAS_LMM_INTRA_TAU_REQ_STRU         *pIntraTauReq = NAS_EMM_NULL_PTR;
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_NORM(               "NAS_EMM_MsRegSsNormalServiceMsgIntraTauReq is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
       NAS_EMM_TAU_LOG_ERR(             "NAS_EMM_MsRegSsNormalServiceMsgIntraTauReq TpCmdType Err!!");
       return NAS_LMM_MSG_DISCARD;
    }

    /* 判断是否存在上行Pending*/
    NAS_EMM_TAU_IsUplinkPending();
    /*如果内部TAU的类型为NAS_LMM_INTRA_TAU_TYPE_IMSI_ATTACH，无线能力变更类型为
    NAS_LMM_UE_RADIO_CAP_LTE_CHG，且存储的APP消息类型为ATTACH REQ，则回复ATTACH CNF，
    结果为MMC_LMM_ATT_RSLT_FAILURE*/
    pIntraTauReq = (NAS_LMM_INTRA_TAU_REQ_STRU*)pMsgStru;
    if((NAS_LMM_INTRA_TAU_TYPE_IMSI_ATTACH == pIntraTauReq->enIntraTauType)   &&
        (NAS_LMM_UE_RADIO_CAP_LTE_CHG == NAS_LMM_GetEmmInfoUeRadioCapChgFlag())&&
        (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ)))
      {
            NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_FAILURE);
      }

    /* 如果上次TAU没有成功，则设置参数变更标识，等待T3411超时*/
    if ((NAS_EMM_YES == NAS_EMM_SER_AbnormalNeedtoContinueTau())
        &&(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411)))
    {
        NAS_LMM_SetEmmInfoDrxNetCapChange(NAS_EMM_YES);
        /*修改TAU发起原因*/
        /*NAS_EMM_TAU_SaveEmmTAUStartCause( NAS_EMM_TAU_START_CAUSE_INTRA_TAU_REQ);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    }
    else
    {
        NAS_LMM_SetEmmInfoDrxNetCapChange(NAS_EMM_YES);
        /*NAS_EMM_TAU_SaveEmmTAUStartCause( NAS_EMM_TAU_START_CAUSE_INTRA_TAU_REQ);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegSsNormalMsgBearerStatusReq( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsNormalMsgBearerStatusReq entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
       NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsRegSsNormalMsgBearerStatusReq:NULL PTR.");
       return NAS_LMM_MSG_DISCARD;
    }


    /* 如果上次TAU没有成功，则修改TAU触发原因，等待T3411超时*/
    if ((NAS_EMM_YES == NAS_EMM_SER_AbnormalNeedtoContinueTau())
        &&(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411)))
    {
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);
    }
    else
    {
        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        /* 触发TAU*/
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);
        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgBearerStatusReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgBearerStatusReq entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
       NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgBearerStatusReq:NULL PTR.");
       return NAS_LMM_MSG_DISCARD;
    }

    /* 如果上次TAU没有成功，则修改TAU触发原因，等待T3411或者T3402超时*/
    if ((NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
        ||(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411)))
    {
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    }
    else
    {
        /* 判断是否存在上行Pending*/
        NAS_EMM_TAU_IsUplinkPending();

        /* 触发TAU*/
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsTauInitSsWaitCnCnfMsgBearerStatusReq(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    VOS_UINT32                          ulRslt = NAS_EMM_FAIL;

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);
    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsTauInitSsWaitCnCnfMsgBearerStatusReq entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

     /*终止当前TAU流程*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    NAS_EMM_TAU_ProcAbnormal();
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfMsgSysinfo:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        /* 上报TAU结果在冲突处理中执行 */
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                             (VOS_VOID*)NAS_EMM_NULL_PTR);
    }
	/* lihong00150010 emergency tau&service end */
    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return  NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_MsRegSsNoVailableCellMsgT3412Exp
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsRegSsNoVailableCellMsgT3412Exp is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsRegSsNoVailableCellMsgT3412Exp PARA NULL!!");
        return NAS_LMM_MSG_DISCARD;
    }
    /*leili modify for isr begin*/

    /* 协议24301 5.3.5章节描述，在REG+NO_AVAILABLE_CELL状态下，T3412超时，如果
       ISR激活，就启动T3423定时器;如果ISR没有激活，如果
       当前是联合注册成功，则回到REG+NORMAL_SERVICE态后，发起联合TAU，TAU类型为
       combined TA/LA updating with IMSI attach；如果不是联合注册成功，则发起周期性TAU */

    /*设置周期TAU定制器超时标志*/
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL);

    if (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType())
    {
        /*启动定时器T3423*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3423);

        /*通知GU模T3423启动*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3423, LMM_MMC_TIMER_START);

    }
    /*leili modify for isr end*/
    return NAS_LMM_MSG_HANDLED;

}

VOS_UINT32  NAS_EMM_MsRegSsAnyStatusMsgT3412Exp( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{
    (VOS_VOID)ulMsgId;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsRegSsAnyStatusMsgT3412Exp is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsRegSsAnyStatusMsgT3412Exp PARA NULL!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 针对GCF9.2.3.1.5 用例临时方案 ,UE丢网,MMC发起搜网 ,搜网期间T3412超时,发起的TAU类型是combined TAU with IMSI类型，
       而仪器测试中,存在现网干扰下,UE在现网上受限驻留,导致无法转到no cell态。
       针对GCF测试，该场景TAU定时器超时标志置为REG_NO_AVALABLE_CELL */
    if((PS_SUCC == LPS_OM_IsTestMode())
        &&(EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
        &&(EMM_SS_REG_LIMITED_SERVICE == NAS_LMM_GetEmmCurFsmSS()))
    {
        /*设置周期TAU定制器超时标志*/
        NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL);
    }
    else
    {
        /*设置周期TAU定制器超时标志*/
        NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
    }

    /*leili modify for isr begin*/

    if (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType())
    {
        /*启动定时器T3423*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3423);

        /*通知GU模T3423启动*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3423, LMM_MMC_TIMER_START);

    }
    /*leili modify for isr end*/
    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegSsSomeStatusMsgIntraTauReq( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{
    NAS_LMM_INTRA_TAU_REQ_STRU         *pIntraTauReq = NAS_EMM_NULL_PTR;
    (VOS_VOID)ulMsgId;
    pIntraTauReq    = (NAS_LMM_INTRA_TAU_REQ_STRU*)pMsgStru;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO(               "NAS_EMM_MsRegSsSomeStatusMsgIntraTauReq is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR(            "NAS_EMM_MsRegSsSomeStatusMsgIntraTauReq PARA NULL!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*设置DRX或NetCapability改变标志*/
    if(NAS_LMM_INTRA_TAU_TYPE_PARA_CHANGE == pIntraTauReq->enIntraTauType)
    {
        NAS_LMM_SetEmmInfoDrxNetCapChange( NAS_EMM_YES);
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsSomeStateMsgBearerStatusReq( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsSomeStateMsgBearerStatusReq entered.");
    (VOS_VOID)ulMsgId;

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsRegSsSomeStateMsgBearerStatusReq:NULL PTR.");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 设置承载上下文变化*/
    NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service begin */

VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgRrcRelInd
(
    VOS_UINT32                              ulMsgId,
   const VOS_VOID                          *pMsgStru
)
{
    LRRC_LMM_REL_IND_STRU              *pMsgRrcRelInd  = (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_NORM("NAS_EMM_MsRegSsLimitedSrvMsgRrcRelInd is entered.");

    /*获得原因值*/
    ulCause = pMsgRrcRelInd->enRelCause;

    NAS_EMM_MsRegSsNmlSrvProcMsgRrcRelInd(ulCause);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */
/*****************************************************************************
 Function Name   : NAS_LMM_PreProcIntraTauReq
 Description     : 处理NAS_LMM_INTRA_TAU_TYPE_UPDATE_MM类型的内部TAU，其他类型
                   的内部TAU进状态机处理
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-11-16  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_LMM_PreProcIntraTauReq
(
    MsgBlock                           *pMsg
)
{
    NAS_LMM_INTRA_TAU_REQ_STRU         *pEmmIntraTauReq = VOS_NULL_PTR;

    NAS_EMM_TAU_LOG_INFO("NAS_LMM_PreProcIntraTauReq is enter!");

    pEmmIntraTauReq                     = (VOS_VOID*)pMsg;

    if (EMM_MS_RRC_CONN_REL_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_LMM_PreProcIntraTauReq: EMM_MS_RRC_CONN_REL_INIT is high priority store");
        return NAS_LMM_STORE_HIGH_PRIO_MSG;
    }

    if (EMM_MS_TAU_INIT == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_LMM_PreProcIntraTauReq: EMM_MS_TAU_INIT is discard");
        return NAS_LMM_MSG_HANDLED;
    }


    /* 预处理中只处理UPDATE_MM类型的内部TAU */
    if (NAS_LMM_INTRA_TAU_TYPE_UPDATE_MM == pEmmIntraTauReq->enIntraTauType)
    {
        /* 清除UPDATE_MM标识 */
        /*NAS_LMM_SetEmmInfoUpdateMmFlag(NAS_EMM_UPDATE_MM_FLAG_INVALID);*/

        if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
        {
            NAS_EMM_TAU_LOG_NORM("NAS_LMM_PreProcIntraTauReq:3411 is running");

            /*修改状态：进入主状态REG子状态REG_ATTEMPTING_TO_UPDATE_MM*/
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        }
        else if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
        {
            NAS_EMM_TAU_LOG_NORM("NAS_LMM_PreProcIntraTauReq:3402 is running");

            /*修改状态：进入主状态REG子状态REG_ATTEMPTING_TO_UPDATE_MM*/
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            NAS_EMM_TAU_LOG_NORM("NAS_LMM_PreProcIntraTauReq:3411 and 3402 is not running");

            /* 判断是否存在上行Pending*/
            NAS_EMM_TAU_IsUplinkPending();

            /*启动TAU Procedure*/
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_IMSI_ATTACH);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_EMM_TAU_StartTAUREQ();
        }

        return NAS_LMM_MSG_HANDLED;
    }

    return NAS_LMM_MSG_DISCARD;
}

VOS_UINT32  NAS_LMM_SendMmcUtranModeCnf(VOS_VOID)
{
    LMM_MMC_UTRAN_MODE_CNF_STRU  *pstLmmUtranModCnf = NAS_EMM_NULL_PTR;

    /*分配内存*/
    /* 申请MMC内部消息 */
    pstLmmUtranModCnf  = (VOS_VOID*)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_UTRAN_MODE_CNF_STRU));
    if(NAS_EMM_NULL_PTR == pstLmmUtranModCnf)
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_LMM_SendMmcUtranModeCnf: MSG ALLOC ERR!");
        return NAS_EMM_SEND_MMC_ERR;
    }

    /*清空申请到的消息空间*/
    NAS_LMM_MEM_SET(pstLmmUtranModCnf, 0, sizeof(LMM_MMC_UTRAN_MODE_CNF_STRU));

    /* 填充给MMC的DOPRA头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER(pstLmmUtranModCnf,
                                 sizeof(LMM_MMC_UTRAN_MODE_CNF_STRU) -
                                 EMM_LEN_VOS_MSG_HEADER);

    /* 填充消息ID */
    pstLmmUtranModCnf->ulMsgId       = ID_LMM_MMC_UTRAN_MODE_CNF;

    /* 发送LMM_MMC_UTRAN_MODE_CNF消息 */
    NAS_LMM_SendLmmMmcMsg(pstLmmUtranModCnf);

    return NAS_EMM_SEND_MMC_OK;
}


VOS_UINT32  NAS_LMM_SendRrcUtranModeReq
(
    LRRC_LNAS_UTRAN_MODE_ENUM_UINT8       enUtranMode
)
{
    LRRC_LMM_UTRAN_MODE_REQ_STRU  *pstLmmUtranModReq = NAS_EMM_NULL_PTR;

    /*分配内存*/
    pstLmmUtranModReq = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_UTRAN_MODE_REQ_STRU));
    if(NAS_EMM_NULL_PTR == pstLmmUtranModReq)
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_LMM_SendRrcUtranModeReq: MSG ALLOC ERR!");
        return NAS_EMM_SEND_RRC_ERR;
    }

    /*清空申请到的消息空间*/
    PS_MEM_SET(pstLmmUtranModReq, 0, sizeof(LRRC_LMM_UTRAN_MODE_REQ_STRU));

    /*填充VOS消息头*/
    NAS_EMM_SET_RRC_MSG_HEADER(pstLmmUtranModReq, sizeof(LRRC_LMM_UTRAN_MODE_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    /* 填充消息ID */
    pstLmmUtranModReq->enMsgId       = ID_LRRC_LMM_UTRAN_MODE_REQ;

    /* 填充消息体 */
    pstLmmUtranModReq->enUtranMode   = enUtranMode;

    /* 发送ID_LRRC_LMM_UTRAN_MODE_NTF */
    NAS_LMM_SEND_MSG(pstLmmUtranModReq);

    return NAS_EMM_SEND_RRC_OK;
}
/*lint +e961*/
/*lint +e960*/

#if 0

VOS_UINT32  NAS_EMM_SomeStateRcvUtranModChgCommProc(VOS_VOID)
{
    VOS_UINT32                          ulCurEmmStat;
    VOS_UINT32                          ulRslt = NAS_LMM_MSG_DISCARD;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_SomeStateRcvUtranModChgCommProc is entered.");

    /* 获取EMM当前状态 */
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                     NAS_EMM_CUR_SUB_STAT);

    /* 根据不同的状态进行不同处理 */
    switch(ulCurEmmStat)
    {
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM):

            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_EMM_TAU_StartTAUREQ();

            /* 给MMC回成功 */
            (VOS_VOID)NAS_LMM_SendMmcUtranModeCnf();
            ulRslt = NAS_LMM_MSG_HANDLED;
            break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_WAIT_ACCESS_GRANT_IND):

            /* 看信令是否被BAR，否: TAU;   是: 不做特殊处理 */
            if (NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
            {
                NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
                NAS_EMM_TAU_StartTAUREQ();
            }

            /* 给MMC回成功 */
            (VOS_VOID)NAS_LMM_SendMmcUtranModeCnf();
            ulRslt = NAS_LMM_MSG_HANDLED;
            break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_LIMITED_SERVICE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_PLMN_SEARCH):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_NO_CELL_AVAILABLE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE):
        /*case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_WAIT_MRRC_REL_CNF):*/

            /* 等收到系统消息或者定时器超时再发起TAU更新，只记录标记 */
            /* 给MMC回成功 */
            (VOS_VOID)NAS_LMM_SendMmcUtranModeCnf();
            ulRslt = NAS_LMM_MSG_HANDLED;
            break;

        default:

            /* 其它所有非稳态均低优先级缓存*/
            ulRslt = NAS_LMM_STORE_LOW_PRIO_MSG;
            break;
    }

    return ulRslt;
}
#endif
VOS_UINT32  NAS_LMM_PreProcMmcUtranModeReq( MsgBlock  *pMsg )
{
    MMC_LMM_UTRAN_MODE_REQ_STRU     *pstMmcUtranModReq = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcUtranModeReq is entered.");

    pstMmcUtranModReq = (MMC_LMM_UTRAN_MODE_REQ_STRU *)(VOS_VOID *)pMsg;

    if ( NAS_EMM_NULL_PTR == pstMmcUtranModReq )
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_LMM_PreProcUtranModeReq: NULL  Msg!");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 只有开机完成或收到系统消息MMC才可能下发，因此开关机过程中收到丢弃 */
    if ( EMM_MS_NULL == NAS_EMM_CUR_MAIN_STAT )
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcUtranModeReq : Power_on or power_off, discard message!");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 收到的模式变更类型非法，丢弃 */
    if ( MMC_LMM_UTRAN_MODE_BUTT <= pstMmcUtranModReq->enUtranMode )
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PreProcUtranModeReq: ERROR: Utran mode is invalid.");
        return NAS_LMM_MSG_HANDLED;
    }

    /*sunbing 49683 2013-10-14 VoLTE begin*/
    /*记录UTRAN模式，在classmark3编码时需要*/
    NAS_LMM_SetEmmInfoUtranMode(pstMmcUtranModReq->enUtranMode);
    /*sunbing 49683 2013-10-14 VoLTE end*/

    /* 向RRC发系统配置请求消息*/
    (VOS_VOID)NAS_LMM_SendRrcUtranModeReq(pstMmcUtranModReq->enUtranMode);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_LMM_PreProcRrcUtranModeCnf( MsgBlock  *pMsg )
{
    /*VOS_UINT32                            ulRslt = NAS_LMM_MSG_DISCARD;*/
    LRRC_LMM_UTRAN_MODE_CNF_STRU  *pstRrcUtranModCnf = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcRrcUtranModeCnf  is entered.");

    pstRrcUtranModCnf = (LRRC_LMM_UTRAN_MODE_CNF_STRU*)(VOS_VOID*)pMsg;

    if (NAS_EMM_NULL_PTR == pstRrcUtranModCnf)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PreProcRrcUtranModeCnf: NULL  Msg!");
        return NAS_LMM_MSG_HANDLED;
    }

    /* Utran能力未发生变化 */
    if (LRRC_LNAS_UE_CAPABILITY_CHANGE != pstRrcUtranModCnf->enCapChangeInd)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcRrcUtranModeNotifyCnf: Utran mode isn't changed.");

        /* 给MMC回成功 */
        (VOS_VOID)NAS_LMM_SendMmcUtranModeCnf();
        return NAS_LMM_MSG_HANDLED;
    }

    /* Utran能力发生变化 */
    /* 无论主从模，只要不是非注册态，均进入 */
    if (NAS_EMM_NO == NAS_EMM_IsDeregState())
    {
        /* 只有当前不是LTE能力变更或GU能力变更时才记录，防止冲掉LTE变更 */
        if ( NAS_LMM_UE_RADIO_CAP_NOT_CHG == NAS_LMM_GetEmmInfoUeRadioCapChgFlag() )
        {
            /* 记录Utran能力的变化情况*/
            NAS_LMM_SetEmmInfoUeRadioCapChgFlag(NAS_LMM_UE_RADIO_CAP_GU_CHG);
        }
    }

    #if 0
    if ( EMM_MS_DEREG != NAS_EMM_CUR_MAIN_STAT )
    {
        /* 只有当前不是LTE能力变更或GU能力变更时才记录，防止冲掉LTE变更 */
        if ( NAS_LMM_UE_RADIO_CAP_NOT_CHG == NAS_LMM_GetEmmInfoUeRadioCapChgFlag() )
        {
            /* 记录Utran能力的变化情况*/
            NAS_LMM_SetEmmInfoUeRadioCapChgFlag(NAS_LMM_UE_RADIO_CAP_GU_CHG);
        }

        /* L处于主模进行以下处理，若从模只记录不做特殊处理 */
        if (NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
        {
            ulRslt = NAS_EMM_SomeStateRcvUtranModChgCommProc();
            return ulRslt;
        }
    }
    #endif

    /* 如果是DEREG态则不处理*/
    /* 给MMC回成功 */
    (VOS_VOID)NAS_LMM_SendMmcUtranModeCnf();

    return NAS_LMM_MSG_HANDLED;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




