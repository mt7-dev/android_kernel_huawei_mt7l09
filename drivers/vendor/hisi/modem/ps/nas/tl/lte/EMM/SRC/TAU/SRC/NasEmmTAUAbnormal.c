

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include "NasEmmAttDetInclude.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMTAUABNORMAL_C
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

NAS_EMM_BAR_STRU                        g_stEmmRegStateBarInfo;

static NAS_EMM_GRANT_ACT_STRU g_astGrantActTable[] =
{
    {LRRC_LNAS_ACCESS_GRANT_MT,                                 0xfb},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL,                            0xfd},
    {LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL,                          0xfe},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MT,                     0xf9},
    {LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MT,                   0xfa},
    {LRRC_LNAS_ACCESS_GRANT_ALL,                                0xf0},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL,              0xfc},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MT,       0xf8},
    {LRRC_LNAS_ACCESS_GRANT_MO_CSFB,                            0xf7},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_CSFB,                0xf5},
    {LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB,              0xf6},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MO_CSFB,  0xf4},
    {LRRC_LNAS_ACCESS_GRANT_MO_CSFB_AND_MT,                     0xf3},
    {LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_CSFB_AND_MT,         0xf1},
    {LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB_AND_MT,       0xf2},

};

VOS_UINT32 ulGrantTypeNum = sizeof(g_astGrantActTable) / sizeof(NAS_EMM_GRANT_ACT_STRU);



/*lint -e960*/
/*lint -e961*/
VOS_UINT32  NAS_EMM_CheckEnterRegNormal(VOS_VOID)
{
    VOS_UINT32                          ulRslt = NAS_EMM_FAIL;
    MMC_LMM_TIN_TYPE_ENUM_UINT32        ulTinType = MMC_LMM_TIN_INVALID;

    ulRslt = NAS_EMM_CheckMutiModeSupport();
    ulTinType = NAS_EMM_GetTinType();

    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_TIN=", ulTinType);
    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_UPDATE_STATE=",(NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat));

    /* 如果是DRX改变发起的TAU失败，不能进入NOR-SERVICE态，需修改EU值 */
    if (NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
       return NAS_EMM_FAIL;
    }

    /* 如果是UE无线能力改变发起的TAU失败，不能进入NOR-SERVICE态，需修改EU值 */
    if (NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        return NAS_EMM_FAIL;
    }

    /* 异系统变换标识有效，且是GPRS挂起类型, 不能进入NORMAL-SERVICE态，需修改EU值*/
    if (   (NAS_EMM_YES == NAS_LMM_GetEmmInfoTriggerTauSysChange())
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState()) )
    {
        return NAS_EMM_FAIL;
    }

    /* 不满足上面的所有不能进入的条件 */
    /* TA在TAI LIST && EU值为EU1 && L单模或者多模但TIN不是P-TMSI，可进入 */
    if(   (NAS_EMM_SUCC == NAS_EMM_TAU_IsCurrentTAInTaList())
        && (EMM_US_UPDATED_EU1 == (NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat))
        && ((NAS_EMM_FAIL == ulRslt)
                || (    (NAS_EMM_SUCC == ulRslt)
                     && (MMC_LMM_TIN_P_TMSI != ulTinType))))
    {
        return NAS_EMM_SUCC;
    }
    return NAS_EMM_FAIL;

}
VOS_VOID    NAS_EMM_TAU_RcvT3440Exp()
{

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_RcvT3440Exp is entered.");

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return;
}


VOS_VOID    NAS_EMM_TAU_RcvT3430Exp()
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_RcvT3430Exp is entered.");

    NAS_EMM_TAU_ProcAbnormal();

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_RcvT3430Exp:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndTimerExp,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);

        /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

        return;
    }
    /* lihong00150010 emergency tau&service end */

    /****检查冲突标志并做相应的处理********************/
    NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndTimerExp,
                    (VOS_VOID*)NAS_EMM_NULL_PTR);

    #if 0

    /*如果T3402定时器运行，则处理冲突标志*/
    if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
        /****检查冲突标志并做相应的处理********************/
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndTimerExp,
                    (VOS_VOID*)NAS_EMM_NULL_PTR);

        /*NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);*/
    }
    else
    {
        NAS_EMM_MmcSendTauActionResultIndTimerExp((VOS_VOID*)NAS_EMM_NULL_PTR);
    }
    #endif

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;

}
VOS_VOID  NAS_EMM_TAU_ProcAbnormal(VOS_VOID)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_ProcAbnormal is entered.");

    /* TAU尝试次数小于5 */
    if(NAS_EMM_TAU_ATTEMPT_CNT_MAX > NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        if(NAS_EMM_SUCC == NAS_EMM_CheckEnterRegNormal())
        {
            /*启动定时器T3411*/
            NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);
			/* lihong00150010 emergency tau&service begin */
            NAS_EMM_TranStatePsNormalServiceOrPsLimitService();
            /* lihong00150010 emergency tau&service end */
            /*转换EMM状态机MS_REG+EMM_SS_REG_NORMAL_SERVICE*/
            /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_PTL_T3411);*/

            return;
        }

        /* 尝试次数小于5但是不能进入NORMAL SERVICE态 */
        /*启动定时器T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*设置EPS状态为EU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
        {
            /*保存PS LOC信息*/
            NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
        }
        /* lihong00150010 emergency tau&service end */

        /*转换EMM状态机MS_REG+SS_NORMAL_SERVICE*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE, TI_NAS_EMM_STATE_NO_TIMER);

        return;
    }

    /* TAU尝试次数等于5 */

    /* 在启动T3402时先判断PLMN 是否在 EPLMN LIST中，如果不在，则设置为默认值 */
    if (VOS_TRUE== NAS_LMM_CheckModifyT3402DefaultValue())
    {
        /*T3402设置为默认值*/
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3402,NAS_LMM_TIMER_T3402_LEN);
    }

    /*启动定时器T3402*/
    NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3402);

    /*设置EPS状态为EU2*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
    /* lihong00150010 emergency tau&service end */

    /*如果当前TAU为周期类型，修改TAU启动原因值*/
    if (NAS_EMM_TAU_START_CAUSE_T3412EXP == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    }

    /*转换EMM状态机MS_REG+SS_ATTEMPT_TO_UPDATE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE, TI_NAS_EMM_STATE_NO_TIMER);

    return;
}



VOS_VOID  NAS_EMM_TAU_ProcCause161722TauAttemptCont(VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_ProcCause161722TauAttemptCont is entered");
    /* lihong00150010 emergency tau&service begin */
    if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
    {
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_MmcSendTauActionResultIndSucc();

        return;
    }
    /* lihong00150010 emergency tau&service end */

    if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        /*启动定时器TI_NAS_EMM_PTL_T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

		/* lihong00150010 emergency delete */
		
        /*修改状态：进入主状态REG子状态ATTACH_WAIT_ESM_BEARER_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_MmcSendTauActionResultIndSucc();

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

        NAS_EMM_MmcSendTauActionResultIndSucc();

        /*NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);*/

        /* 会调用NAS_EMM_TAU_ProcCause161722TauAttemptCont函数的场景都是IDLE发起
           的TAU，对于CS/PS1的场景会自动等待网侧来释放链路，无需LMM主动释放链路 */
    }

    return;
}


VOS_VOID  NAS_EMM_TAU_ProcEnterForbTA
(
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 ulForbInfo
)
{

    /* 根据Collison标志进行 不同处理*/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
                NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&ulForbInfo);

                /* 如果是IMSI DETACH，则本地CS域去注册，进入REG+LIMIT_SERVICE;
                   否则本地PS域去注册，进入DEREG+LIMMIT_SERVICE */
                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {
                    NAS_EMM_TAU_LOG_NORM( "NAS_EMM_TAU_ProcEnterForbTA : Process EPS Detach or EPS/IMSI Detach.");

                    /*修改状态：进入主状态DEREG子状态DEREG_NORMAL_SERVICE*/
                    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                                EMM_SS_DEREG_LIMITED_SERVICE,
                                                TI_NAS_EMM_STATE_NO_TIMER);

                    NAS_EMM_TAU_CollisionDetachProc();

                    /* 本地DETACH*/
                    NAS_LMM_DeregReleaseResource();
                }
                else
                {
                    NAS_EMM_TAU_LOG_NORM( "NAS_EMM_TAU_ProcEnterForbTA : Process Imsi Detach.");

                    /* 设置注册域为PS */
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

                    NAS_EMM_TAU_CollisionDetachProc();

                    /* IMSI DETACH后释放资源 ,必需放在给APP上报结果之后，否则DETACH TYPE
                    会不正确*/
                    NAS_LMM_ImsiDetachReleaseResource();
                }

                break;

        case    NAS_EMM_COLLISION_SERVICE    :

                /* 向MMC发送LMM_MMC_TAU_RESULT_IND消息放到冲突处理中处理 */
                NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndForbType,
                                (VOS_VOID*)&ulForbInfo,
                                NAS_EMM_TRANSFER_RAT_ALLOW);
                break;

        default :
                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_ProcEnterForbTA : NO Collision.");

                /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
                NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&ulForbInfo);
                break;
    }

    return;
}


VOS_VOID    NAS_EMM_TAU_MsRegMsgRelInd( LRRC_LNAS_REL_CAUSE_ENUM_UINT32   enRelCause )
{


    if(LRRC_LNAS_REL_CAUSE_LOAD_BALANCE_REQ         ==    enRelCause)
    {
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE);
    }
    else if(LRRC_LNAS_REL_CAUSE_CONN_FAIL           ==    enRelCause)
    {
        /* 如果上次TAU成功，重新赋值
        */
        if(NAS_EMM_TRIGGER_TAU_RRC_REL_NO        ==  NAS_LMM_GetEmmInfoTriggerTauRrcRel())
        {
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
        }
    }
    else
    {
        /* 其他原因不会导致TAU，不处理 */;
        NAS_EMM_TAU_LOG1_INFO("NAS_EMM_TAU_MsRegMsgRelInd: enRelCause = ", enRelCause);

        return;
    }

    /* 判断是否存在上行Pending*/
    NAS_EMM_TAU_IsUplinkPending();

    NAS_EMM_TAU_StartTAUREQ();

    return;
}
VOS_UINT32 NAS_EMM_MsTauInitSsWaitCNCnfMsgT3430Exp(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                                  ulRslt              = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_NORM("Nas_Emm_MsTauInitSsWaitCNCnfMsgT3430Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        /* 打印异常 */
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*TAU ATTEMPT COUNT ++*/
    NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;

    /*调用消息处理函数*/
    NAS_EMM_TAU_RcvT3430Exp();

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd( VOS_UINT32 ulCause )
{

    NAS_EMM_TAU_LOG_INFO(                         "NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd is entered.");

    /*common process*/
    NAS_LMM_StopStateTimer(                        TI_NAS_EMM_STATE_TAU_T3430);
    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);

        NAS_EMM_CommProcConn2Ilde();

        return;
    }
    /* lihong00150010 emergency tau&service end */
    NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;

    /*不存在DETACH_COLLISION标志，则根据不同释放原因值处理*/
    switch(ulCause)
    {
        /* RRC处于IDLE态，重新触发TAU*/
        case LRRC_LNAS_REL_CAUSE_NAS_DATA_ABSENT:
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd:DATA ABSENT.");

            /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
            NAS_EMM_CommProcConn2Ilde();

            /* 判断是否存在上行Pending*/
            NAS_EMM_TAU_IsUplinkPending();

            /* 触发TAU*/
            NAS_EMM_TAU_StartTAUREQ();
            return;

        default:

            NAS_EMM_TAU_ProcAbnormal();

            /* 向MMC发送LMM_MMC_TAU_RESULT_IND消息在冲突内部处理 */
            NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                               (VOS_VOID*)NAS_EMM_NULL_PTR);
            #if 0
            /* 如果处理异常启动了3402，清除TAU计数Counter */
            if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
            {
                /*清空 Tau Attempt Counter*/
                NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
            }
            #endif
            break;
    }

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return;
}
VOS_UINT32 NAS_EMM_MsTauInitSsWaitCnCnfMsgAuthFail(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   = (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCnCnfMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                             =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL              ==  ulCause)
    {
        NAS_EMM_MsTauInitSsWaitCnTauCnfProcMsgAuthRej(      ulCause);
    }
    else
    {
        NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd(       ulCause);
    }


    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsTauInitSsWaitCnCnfMsgAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        const VOS_VOID   *pMsgStru)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCnCnfMsgAuthRej is entered.");

    NAS_EMM_MsTauInitSsWaitCnTauCnfProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32 NAS_EMM_MsTauInitSsWaitCnCnfMsgRrcRelInd(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru)
{
    LRRC_LMM_REL_IND_STRU                 *pRrcRelInd     = (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCnCnfMsgRrcRelInd is entered.");

    /*获得原因值*/
    ulCause                                             =   pRrcRelInd->enRelCause;

    NAS_EMM_MsTauInitSsWaitCNCnfProcMsgRrcRelInd(           ulCause);

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsTauInitSsWaitCnCnfMsgDrbSetupInd
 Description     : 在等待TAU acp消息过程中收到Drb建立成功消息的处理
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing49683      2010-10-8  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsTauInitSsWaitCnCnfMsgDrbSetupInd(
                                                  VOS_UINT32  ulMsgId,
                                                  const VOS_VOID   *pMsgStru)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCnCnfMsgDrbSetupInd is entered.");

    /*TAU过程中，在等待网络响应时，收到底层Drb建立成功的消息，
      为了避免随后的TAU流程完成后，释放链路，需要将本次TAU过程认为是携带了active flag的TAU流程*/
    NAS_EMM_TAU_SaveEmmTauReqActiveCtrl(NAS_EMM_TAU_WITH_BEARER_EST_REQ);

    return NAS_LMM_MSG_HANDLED;
}



VOS_VOID  NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_ENUM_UINT32 enBarProc,
                            LRRC_LNAS_EST_RESULT_ENUM_UINT32 enBarType)
{
    /*Coverity 告警消除 2013-9-26 Begin*/
    NAS_EMM_BAR_PROCEDURE_ENUM_UINT32 enBarProcOld;
    /*Coverity 告警消除 2013-9-26 End*/

    /*检查Bar的流程:TAU or SER */
    if ((NAS_EMM_BAR_PROCEDURE_TAU != enBarProc)
      &&(NAS_EMM_BAR_PROCEDURE_SERVICE != enBarProc))
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_SetBarInfo:Input enBarProc Err.");
        return;
    }

    /*Coverity 告警消除 2013-9-26 Begin*/
    enBarProcOld = NAS_EMM_TAU_GetRegBarProcedure();
    /*Coverity 告警消除 2013-9-26 End*/

    /*入参有效:记录Bar的流程和类型*/
    NAS_EMM_TAU_SetRegBarProcedure(enBarProc);

    switch (enBarType)
    {
        case LRRC_EST_ACCESS_BARRED_MO_SIGNAL:
            NAS_EMM_TAU_GetRegBarType() |= NAS_EMM_BIT_1;
            break;

        case LRRC_EST_ACCESS_BARRED_ALL:
            NAS_EMM_TAU_GetRegBarType() |= NAS_LMM_LOW_HALF_BYTE_F;
            break;

        case LRRC_EST_ACCESS_BARRED_MO_CALL:
            NAS_EMM_TAU_GetRegBarType() |= NAS_EMM_BIT_2;
            break;

        case LRRC_EST_ACCESS_BARRED_MO_CSFB:
            NAS_EMM_TAU_GetRegBarType() |= NAS_EMM_BIT_4;
            break;

        case LRRC_EST_ACCESS_BARRED_MO_CALL_AND_CSFB:
            NAS_EMM_TAU_GetRegBarType() |= 0x0a;
            break;

         case LRRC_EST_ACCESS_BARRED_MT_CALL:
            NAS_EMM_TAU_GetRegBarType() |= NAS_EMM_BIT_3;
            break;

        default:
            /*Coverity 告警消除 2013-9-26 Begin*/
            NAS_EMM_TAU_SetRegBarProcedure(enBarProcOld);
            NAS_EMM_TAU_LOG_ERR("NAS_EMM_SetBarInfo:Input enBarType Err.");
            /*Coverity 告警消除 2013-9-26 End*/
            break;
    }
    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_JudgeBarType
 Description     : 判断是否是某种类型的bar
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing49683      2010-8-25  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_JudgeBarType( NAS_EMM_BAR_TYPE_ENUM_UINT32 enBarType )
{
    if( NAS_EMM_BAR_TYPE_MO_SIGNAL == enBarType)
    {
        /*bit1 为1，标识MO SIGNAL类型被bar*/
        if(NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
        {
            return NAS_EMM_SUCC;
        }
        else
        {
            return NAS_EMM_FAIL;
        }
    }
    else if(NAS_EMM_BAR_TYPE_MO_CALL == enBarType)
    {
        /*bit2 为1，标识MO CALL类型被bar*/
        if(NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>> NAS_LMM_MOVEMENT_1_BITS))
        {
            return NAS_EMM_SUCC;
        }
        else
        {
            return NAS_EMM_FAIL;
        }
    }
    else if(NAS_EMM_BAR_TYPE_MT == enBarType)
    {
        /*bit3 为1，标识MT CALL类型被bar*/
        if(NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
        {
            return NAS_EMM_SUCC;
        }
        else
        {
            return NAS_EMM_FAIL;
        }

    }
    else if(NAS_EMM_BAR_TYPE_MO_CSFB == enBarType)
    {
        /*bit4 为1，标识MO CSFB类型被bar*/
        if(NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_4)>>NAS_LMM_MOVEMENT_3_BITS))
        {
            return NAS_EMM_SUCC;
        }
        else
        {
            return NAS_EMM_FAIL;
        }

    }
    else if(NAS_EMM_BAR_TYPE_MO_CALL_AND_CSFB == enBarType)
    {
        /*bit4&bit2 为1，标识MO CALL& MO CSFB类型被bar*/
        if((NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_4)>>NAS_LMM_MOVEMENT_3_BITS))
            && (NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)))
        {
            return NAS_EMM_SUCC;
        }
        else
        {
            return NAS_EMM_FAIL;
        }

    }
    else
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_JudgeBarType: Bar type is err.");
        return NAS_EMM_FAIL;
    }
}
/*VOS_UINT32  NAS_EMM_GetBarType (VOS_VOID )
{
    NAS_EMM_BAR_TYPE_ENUM_UINT32 enBarType = NAS_EMM_BAR_TYPE_BUTT;

    if ((NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
        && (NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
        && (NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_MO_SIGNAL;
    }
    else if ((NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
             &&(NAS_EMM_BIT_SLCT == ((NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>> NAS_LMM_MOVEMENT_1_BITS))
             && (NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))

    {
        enBarType = NAS_EMM_BAR_TYPE_MO_CALL;
    }
    else if ((NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
            && (NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
            && (NAS_EMM_BIT_SLCT ==(NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_ALL;
    }
    else if ((NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
            && (NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
            && (NAS_EMM_BIT_NO_SLCT ==(NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_MO;
    }
    else if ((NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
            && (NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
            && (NAS_EMM_BIT_SLCT ==(NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_MT;
    }
    else if ((NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
            && (NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
            && (NAS_EMM_BIT_SLCT ==(NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_MO_CALL_AND_MT;
    }
    else if ((NAS_EMM_BIT_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_1))
            && (NAS_EMM_BIT_NO_SLCT == (NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_2)>>NAS_LMM_MOVEMENT_1_BITS)
            && (NAS_EMM_BIT_SLCT ==(NAS_EMM_TAU_GetRegBarType() & NAS_EMM_BIT_3)>>NAS_LMM_MOVEMENT_2_BITS))
    {
        enBarType = NAS_EMM_BAR_TYPE_MO_SIGNAL_AND_MT;
    }
    else
    {
        ;
    }

    return enBarType;
}*/


VOS_VOID  NAS_EMM_ClearBarType( LRRC_LNAS_ACCESS_GRANT_ENUM_UINT32 enBarType)
{
    VOS_UINT32                          i = 0;

    for(i = 0; i < ulGrantTypeNum; i++)
    {
        if (enBarType == g_astGrantActTable[i].enGrantType)
        {
            NAS_EMM_TAU_GetRegBarType() &= g_astGrantActTable[i].ucGrantValue;

            break;
        }
    }

}





VOS_VOID  NAS_EMM_ClearBarResouce(VOS_VOID)
{
    NAS_LMM_MEM_SET(NAS_EMM_TAU_GetRegBarInfoAddr(), 0, sizeof(NAS_EMM_BAR_STRU));
    return;
}


VOS_UINT32  NAS_EMM_GetBarProcedure(VOS_VOID )
{
    NAS_EMM_BAR_PROCEDURE_ENUM_UINT32   ulBarProc;

    ulBarProc = NAS_EMM_TAU_GetRegBarProcedure();

    return ulBarProc;
}




VOS_UINT32 NAS_EMM_MsTauInitSsWaitCNCnfMsgIntraConnectFailInd(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru)
{

    VOS_UINT32                          ulRslt                  = NAS_EMM_FAIL;
    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *pMrrcConnectFailRelInd  = NAS_EMM_NULL_PTR;
    MMC_LMM_TAU_RSLT_ENUM_UINT32        enTauRslt               = MMC_LMM_TAU_RSLT_ACCESS_BARED;
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg               = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfMsgIntraConnectFailInd is entered.");

    /*check the input params and current states*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN( "NAS_EMM_MsTauInitSsWaitCNCnfMsgIntraConnectFailInd ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*get the msg INTRA_REL_IND*/
    pMrrcConnectFailRelInd = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;

    /*commom process*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);
	/* lihong00150010 emergency tau&service begin */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsTauInitSsWaitCNCnfMsgIntraConnectFailInd:CAUSE_ESM_EMC_PDN_REQ");

        if ((LRRC_EST_ACCESS_BARRED_MO_SIGNAL == pMrrcConnectFailRelInd->enEstResult)
            || (LRRC_EST_ACCESS_BARRED_ALL == pMrrcConnectFailRelInd->enEstResult))
        {
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);
        }
        else if (LRRC_EST_CELL_SEARCHING == pMrrcConnectFailRelInd->enEstResult)
        {
            enTauRslt = MMC_LMM_TAU_RSLT_FAILURE;
            NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);
        }
        else
        {
            NAS_EMM_MmcSendTauActionResultIndFailWithPara((VOS_VOID*)NAS_EMM_NULL_PTR);
        }

        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* 本地DETACH释放资源:动态内存、赋初值 */
        NAS_LMM_DeregReleaseResource();

        /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
        NAS_EMM_MmcSendDetIndLocal( MMC_LMM_L_LOCAL_DETACH_OTHERS);

        NAS_EMM_CommProcConn2Ilde();

        if (LRRC_EST_CELL_SEARCHING == pMrrcConnectFailRelInd->enEstResult)
        {
            /* 记录ATTACH触发原因值 */
            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

            NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /*保存ESM消息*/
            NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                                   &(pstEsmMsg->stEsmMsg),
                                    (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

            NAS_EMM_StartEmergencyAttach();
        }

        return NAS_LMM_MSG_HANDLED;
    }
	/* lihong00150010 emergency tau&service end */
    switch (pMrrcConnectFailRelInd->enEstResult)
    {
        case LRRC_EST_ACCESS_BARRED_MO_SIGNAL:
        case LRRC_EST_ACCESS_BARRED_ALL:

            /*设置bar流程和类型*/
            NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_TAU,pMrrcConnectFailRelInd->enEstResult);

            /*转换EMM状态机MS_REG+SS_WAIT_ACCESS_GRANT_IND*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                        EMM_SS_REG_WAIT_ACCESS_GRANT_IND,
                                        TI_NAS_EMM_STATE_NO_TIMER);

            NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndOthertype,
                               (VOS_VOID*)&enTauRslt);
            break;

        /* 建链失败，RRC正在进行小区搜索，迁移到Reg.Plmn_Search*/
        /*
        case LRRC_EST_CELL_SEARCHING:
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                        EMM_SS_REG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

            NAS_EMM_TAU_RelIndCollisionProc((NAS_LMM_SEND_TAU_RESULT_ACT_FUN)NAS_EMM_NULL_PTR,
                                 (VOS_VOID*)NAS_EMM_NULL_PTR);
            break;
        */

        default:
            NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;

            NAS_EMM_TAU_ProcAbnormal();

            /* 向MMC发送LMM_MMC_TAU_RESULT_IND消息在冲突内部处理 */
            NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndFailWithPara,
                                 (VOS_VOID*)NAS_EMM_NULL_PTR);
            #if 0
            /* 如果处理异常启动了3402，清除TAU计数Counter */
            if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
            {
                /*清空 Tau Attempt Counter*/
                NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
            }
            #endif
            break;

    }

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return NAS_LMM_MSG_HANDLED;
}

#if 0   /*暂时不会出现释放原因为参数变更的场景*/

static VOS_VOID  NAS_EMM_TAU_RrcRelCauseDrxOrNetCapChange( VOS_VOID )
{
    NAS_EMM_TAU_START_CAUSE_ENUM_UINT8            enTauStartCause;

    NAS_EMM_TAU_LOG_NORM(                         "NAS_EMM_TAU_RrcRelCauseDrxOrNetCapChange is entered.");

    /* 依据原因值发起不同类型的TAU
    */
    enTauStartCause                               = NAS_EMM_TAU_GetEmmTAUStartCause();

    if(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE
                                                  == enTauStartCause)
    {
        NAS_EMM_TAU_StartTAUREQ(                  NAS_EMM_MSG_LOAD_BALANCING_TAU);
    }
    else
    {
        NAS_EMM_TAU_StartTAUREQ(                  NAS_EMM_MSG_NOT_LOAD_BALANCING_TAU);
    }

    return;

}
#endif

VOS_UINT32 NAS_EMM_SndTauCompleteFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_EMM_MRRC_DATA_REQ_STRU          *pMrrcDataMsg = VOS_NULL_PTR;
    VOS_UINT32                            ulRrcMmDataReqMsgLen;

    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;

    *pulIsDelBuff = VOS_TRUE;

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
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SndTauCompleteFailProc: Mem Alloc Fail");
        return NAS_EMM_FAIL;
    }

    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_HO:
            /*  需要将对应的缓存消息的ulHoWaitSysInfoFlag设置为true */
            pEmmMrrcMgmtData->ulHoWaitSysInfoFlag = VOS_TRUE;
            *pulIsDelBuff = VOS_FALSE;

            break;

        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:
            pMrrcDataMsg->enDataCnf = LRRC_LMM_DATA_CNF_NEED;

            pMrrcDataMsg->enEmmMsgType = NAS_EMM_MSG_TAU_CMP;
            pMrrcDataMsg->ulMsgId = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
            pMrrcDataMsg->ulEsmMmOpId = pEmmMrrcMgmtData->ulEsmMmOpId;

            pMrrcDataMsg->stNasMsg.ulNasMsgSize = pEmmMrrcMgmtData->ulNasMsgLength;

            NAS_LMM_MEM_CPY(pMrrcDataMsg->stNasMsg.aucNasMsg,
                             pEmmMrrcMgmtData->aucNasMsgContent,
                             pEmmMrrcMgmtData->ulNasMsgLength);

            NAS_EMM_SndUplinkNasMsg(pMrrcDataMsg);


            break;

        case LRRC_LMM_SEND_RSLT_FAILURE_CONN_REL:
        case LRRC_LMM_SEND_RSLT_FAILURE_CTRL_NOT_CONN:
            NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
            NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);

            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
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

VOS_UINT32 NAS_EMM_SndTauReqFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_LMM_MAIN_STATE_ENUM_UINT16       enMainState;

    enMainState                         = NAS_LMM_GetEmmCurFsmMS();

    *pulIsDelBuff = VOS_TRUE;

    /*检查状态是否匹配，若不匹配，退出*/
    if (EMM_MS_TAU_INIT != enMainState)
    {
        /*打印出错信息*/
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_SndTauReqFailProc: STATE ERR!");
        return  NAS_EMM_SUCC;
    }

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    switch (pstRrcMmDataCnf->enSendRslt)
    {
        case LRRC_LMM_SEND_RSLT_FAILURE_HO:
        case LRRC_LMM_SEND_RSLT_FAILURE_TXN:
        case LRRC_LMM_SEND_RSLT_FAILURE_RLF:

            /*设置EPS状态为EU2*/
            NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

            if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
            {
                /*保存PS LOC信息*/
                NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
            }
            /*停止T3430定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

            /*停止T3440定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            NAS_EMM_TAU_StartTAUREQ();
            break;

        default:
            break;
        }

    return NAS_EMM_SUCC;
}


VOS_UINT32 NAS_EMM_SndTauCmpSuccProc(VOS_VOID* pMsg)
{
    (VOS_VOID)pMsg;

    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SndTauCmpSuccProc: entern");
    NAS_EMM_TAU_SaveEmmTauCompleteFlag(NAS_EMM_TAU_COMPLETE_INVALID);

    return NAS_EMM_SUCC;
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



