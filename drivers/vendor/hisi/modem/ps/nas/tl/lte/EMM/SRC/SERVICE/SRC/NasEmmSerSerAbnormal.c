


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include "NasEmmAttDetInclude.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMSERVICEABNORMAL_C
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

/*******************************************************************************
  Module   : NAS_EMM_SER_AbnormalNeedtoContinueTau
  Function :
  Input    :
  Output   :
  NOTE     : 无
  Return   : 无
  History  :
    1.FTY    2012-02-27  新规作成
*******************************************************************************/
VOS_UINT32 NAS_EMM_SER_AbnormalNeedtoContinueTau(VOS_VOID)
{
    if((NAS_EMM_T3412_EXP_NO < NAS_LMM_GetEmmInfoT3412ExpCtrl())
      &&(NAS_EMM_T3412_EXP_BUTT > NAS_LMM_GetEmmInfoT3412ExpCtrl()))
    {
        return NAS_EMM_YES;
    }

    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        return NAS_EMM_YES;
    }

    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        return NAS_EMM_YES;
    }

    if(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE == NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        return NAS_EMM_YES;
    }
    if(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE == NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        return NAS_EMM_YES;
    }
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoTriggerTauSysChange())
    {
        return NAS_EMM_YES;
    }

    if(NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        return NAS_EMM_YES;
    }


    return NAS_EMM_NO;

}

/*******************************************************************************
  Module   : NAS_EMM_SER_SuccNeedtoContinueTau
  Function :
  Input    :
  Output   :
  NOTE     : 无
  Return   : 无
  History  :
    1.FTY    2012-02-27  新规作成
*******************************************************************************/
VOS_UINT32 NAS_EMM_SER_SuccNeedtoContinueTau(VOS_VOID)
{
    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        return NAS_EMM_YES;
    }

    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        return NAS_EMM_YES;
    }

    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoTriggerTauSysChange())
    {
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;

}
#if 0

VOS_VOID NAS_EMM_SER_RrcRelCause_SERREJ25(VOS_VOID)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_RrcRelCause_SERREJ25 is entered.");

    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

     /*发送EMM_ESM_STATUS_IND(注册)*/
    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*转换EMM状态机MS_REG+SS_DEREG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_LIMITED_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /*发送CELL_SELECTION_CTRL_REQ内部消息给PUBLIC模块*/
    /*NAS_EMM_TAUSER_SendCellSelectionReq();*/

    return;
}

#endif

VOS_VOID    NAS_EMM_SER_RcvRrcRelInd(VOS_VOID)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_RcvRrcRelInd is entered.");

    /*设置SER的结果为 NAS_EMM_SER_RESULT_ABNORMAL*/
    /*NAS_EMM_SER_SaveSERresult(NAS_EMM_SER_RESULT_ABNORMAL);*/
    /* lihong00150010 emergency tau&service begin */
    NAS_EMM_TranStatePsNormalServiceOrPsLimitService();
    /* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_REG+EMM_SS_REG_NORMAL_SERVICE*/
    /*NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);*/

    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();
        return ;
    }

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    /* 如果有未完成的TAU，无需在此触发，有定时器等定时器超时，无定时器等后续收到系统消息再触发 */

#if 0
    /*如果有未完成的TAU，则需要重新触发TAU*/
    if(NAS_EMM_YES == NAS_EMM_SER_AbnormalNeedtoContinueTau())
    {
        /*如果T3411已经停止运行，则立即触发，(如果是TAU被Ba后触发的Service,则T3411没有启动)
          反之，如果T3411还在运行，则这里不触发，等T3411超时后，再触发*/
        if(NAS_LMM_TIMER_STOPED == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
        {
            NAS_EMM_TAU_StartTAUREQ();
        }
    }
#endif
    return;
}


VOS_VOID  NAS_EMM_ServiceReqRejectOtherCause
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_ServiceReqRejectOtherCause is entered.");

	/* lihong00150010 emergency tau&service begin */
    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ServiceReqRejectOtherCause: CAUSE_ESM_DATA_REQ_EMC.");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        NAS_EMM_TranStatePsNormalServiceOrPsLimitService();
    }
	/* lihong00150010 emergency tau&service end */
    /* 如果有未完成的TAU，等在REG+NORMAL_SERVICE状态下收到系统消息后再处理 */

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);


    return;
}


/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_EMM_SerClearEsmDataBuf( VOS_VOID )
{
    VOS_UINT32                          i      = 0;
    VOS_UINT32                          ulRslt = 0;

     NAS_EMM_SER_LOG_INFO("NAS_EMM_SerClearEsmDataBuf entered!");

     /**清除保存数据的标志和数据区**************/
     for (i = 0; i < g_stEmmEsmMsgBuf.ulEsmMsgCnt; i++)
     {
         if (NAS_LMM_NULL_PTR != g_stEmmEsmMsgBuf.apucEsmMsgBuf[i])
         {
            ulRslt = NAS_COMM_FreeBuffItem(NAS_COMM_BUFF_TYPE_EMM,
                                (VOS_VOID *)g_stEmmEsmMsgBuf.apucEsmMsgBuf[i]);

            if (NAS_COMM_BUFF_SUCCESS != ulRslt)
            {
                NAS_EMM_SER_LOG_WARN("NAS_EMM_SerClearEsmDataBuf, Memory Free is not succ");
            }

            g_stEmmEsmMsgBuf.apucEsmMsgBuf[i] = NAS_LMM_NULL_PTR;
         }
         else
         {
             NAS_EMM_SER_LOG1_WARN("NAS_EMM_SerClearEsmDataBuf, Buffer item is null:",i);
         }
     }

     g_stEmmEsmMsgBuf.ulEsmMsgCnt = 0;

}
VOS_UINT32 NAS_EMM_SER_IsNotEmergencyCsfb(VOS_VOID)
{
    if (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ != NAS_EMM_SER_GetEmmSERStartCause())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_VOID  NAS_EMM_SER_SERREJ367
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ367 entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /* set the EPS update status to EU3 ROAMING NOT ALLOWED */
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);
    /*USIM无效，直到关机或拔卡*/
    /*暂不处理*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* 记录ATTACH触发原因值 */
        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_DEREG+SS_DEREG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG, EMM_SS_DEREG_NO_IMSI, TI_NAS_EMM_STATE_NO_TIMER);

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }


    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return;
}
VOS_VOID  NAS_EMM_SER_SERREJ9
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ9 entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /* 设置设置EU2 NOT UPDATED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /* 删除GUTI,KSIasme,TAI list,L.V.R TAI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);
	/* lihong00150010 emergency tau&service begin */
    /* 如果是紧急呼PDN建立引起的SERVICE，则直接发起紧急注册；
       如果是紧急CSFB,直接释放链路，否则发内部ATTACH; */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_SERREJ9: CAUSE_ESM_DATA_REQ_EMC");

        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();
    }
    else if (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb())
    {/* lihong00150010 emergency tau&service end */
        /* 延时发送attach请求 */
        NAS_EMM_ReattachDelay();
    }
    else
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    /* 清空SER和TAU所有资源*/
    NAS_EMM_SER_ClearResource();
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;

}


VOS_VOID  NAS_EMM_SER_SERREJ10
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ10 is entered.");
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /* 转换EMM状态机MS_DEREG+SS_DEREG_NORMAL_SERVICE:状态转换函数执行删除新安全上下文*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 进入DEREG态不再写统一写安全上下文，参照标杆，在SER被#10拒绝，将安全上下文写卡 */
    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
    /* lihong00150010 emergency tau&service end */
    /* 如果是紧急呼PDN建立引起的SERVICE，则直接发起紧急注册；
       如果是紧急CSFB,直接释放链路，否则发内部ATTACH; */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SER_SERREJ10: CAUSE_ESM_DATA_REQ_EMC");

        /*保存ESM消息*/
        NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();
    }
    else if (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb())
    {/* lihong00150010 emergency tau&service end */
        /* 延时发送attach请求 */
        NAS_EMM_ReattachDelay();

    }
    else
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    /* 清空SER和TAU所有资源*/
    NAS_EMM_SER_ClearResource();
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;

}


VOS_VOID  NAS_EMM_SER_SERREJ11
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{
     /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ11 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,L.V.R TAI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* 记录ATTACH触发原因值 */
        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_DEREG+EMM_SS_DEREG_LIMITED_SERVICE*/
    /*收到MMC搜网指令后再转入PLMN_SEARCH态*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG, EMM_SS_DEREG_LIMITED_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;
}


VOS_VOID  NAS_EMM_SER_SERREJ12
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{

    NAS_MM_TA_STRU                      stTa;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ12 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,L.V.R TAI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*store the current TAI in the list of
                  "forbidden tracking areas for regional provision of service"*/
    NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr());
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* 记录ATTACH触发原因值 */
        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */

    /* 增加REJ cause 12标记,用于向MMC上报有区域限制的限制服务状态 2011-07-27*/
    NAS_LMM_SetNasRejCause12Flag(NAS_LMM_REJ_CAUSE_IS_12);

    /*转换EMM状态机MS_DEREG+SS_DEREG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG, EMM_SS_DEREG_LIMITED_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;
}


VOS_VOID  NAS_EMM_SER_SERREJ13
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{

    NAS_MM_TA_STRU                      stCurrentTa;
    NAS_MM_TA_LIST_STRU                *pstTaiList;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ13 is entered.");

    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;
	/* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
    /* lihong00150010 emergency tau&service end */
    /*Remove current TA from TAI List*/
    NAS_EMM_GetCurrentTa(&stCurrentTa);

    pstTaiList = NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
    NAS_LMM_DeleteTaFromTaList(&stCurrentTa, pstTaiList, NAS_MM_MAX_TA_NUM);

    /*store the current TAI in the list of
                "forbidden tracking areas for roaming"*/
    NAS_EMM_AddForbTa(&stCurrentTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());

    NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

    /* lihong00150010 emergency tau&service begin */
    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_SER_SERREJ13: CAUSE_ESM_DATA_REQ_EMC.");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_LIMITED_SERVICE);
    }
    else
    {
        /*为上报服务状态,转换EMM状态机MS_REG+SS_REG_LIMITED_SERVICE收到MMC搜网指令
          后再转入PLMN_SEARCH态*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_LIMITED_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);
    }
	/* lihong00150010 emergency tau&service end */
    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;
}


VOS_VOID  NAS_EMM_SER_SERREJ15
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)

{

    NAS_MM_TA_STRU                      stCurrentTa;
    NAS_MM_TA_LIST_STRU                *pstTaiList;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ15 is entered.");

    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;
	/* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
	/* lihong00150010 emergency tau&service end */
    /*Remove current TA from TAI List*/
    NAS_EMM_GetCurrentTa(&stCurrentTa);

    pstTaiList = NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
    NAS_LMM_DeleteTaFromTaList(&stCurrentTa, pstTaiList, NAS_MM_MAX_TA_NUM);

    /*store the current TAI in the list of
                   "forbidden tracking areas for roaming"*/
    NAS_EMM_AddForbTa(&stCurrentTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());

    NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

	/* lihong00150010 emergency tau&service begin */
    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_SER_SERREJ15: CAUSE_ESM_DATA_REQ_EMC.");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_LIMITED_SERVICE);
    }
    else
    {
        /*为上报服务状态,转换EMM状态机MS_REG+SS_REG_LIMITED_SERVICE收到MMC搜网指令
          后再转入PLMN_SEARCH态*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_LIMITED_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);
    }
	/* lihong00150010 emergency tau&service end */
    /*等待RRC_REL_IND*/
    NAS_EMM_WaitNetworkRelInd();

    return;
}


VOS_VOID  NAS_EMM_SER_SERREJ18
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)
{
    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*转换EMM状态机MS_REG+SS_REG_NORMAL_SERVIC*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /* 如果是紧急CSFB或者不是数据连接态,主动释放 */
    if ((VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb())
        || (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState()))
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    return;

}

VOS_VOID  NAS_EMM_SER_SERREJ40
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)
{
    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ40 is entered.");

    (VOS_VOID)pMsgStru;

    /* 发送EMM_ESM_STATUS_IND(去注册)*/
    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 转换EMM状态机MS_DEREG+SS_DEREG_NORMAL_SERVICE:状态转换函数执行删除新安全上下文*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /* 如果是CSFB流程或紧急CSFB，则重选到GU模 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        || (VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }
    else
    {
        (VOS_VOID)NAS_EMM_SendIntraAttachReq();
    }

    /* 清空SER和TAU所有资源*/
    NAS_EMM_SER_ClearResource();
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;
}
VOS_VOID  NAS_EMM_SER_SERREJ39
(
    const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru
)
{
    VOS_UINT32                      ulTimerLen      = NAS_EMM_NULL;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_SER_SERREJ39 is entered.");

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_CN_REJ);
    }

    /*转换EMM状态机MS_REG+SS_REG_NORMAL_SERVIC*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndRej(pMsgStru->ucEMMCause);

    /* 如果是紧急CSFB或者不是数据连接态,主动释放 */
    if ((VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb())
        || (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState()))
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    if (NAS_EMM_BIT_SLCT != pMsgStru->ucBitOpT3442)
    {
        NAS_EMM_SER_LOG_WARN("NAS_EMM_SER_SERREJ39:No T3442!");
        return ;
    }

    ulTimerLen = NAS_EMM_TimerValTransfer(  pMsgStru->stT3442.ucUnit,
                                            pMsgStru->stT3442.ucTimerVal);

    if (NAS_EMM_NULL != ulTimerLen)
    {
        NAS_LMM_ModifyStateTimer(TI_NAS_EMM_STATE_SERVICE_T3442,ulTimerLen);
        NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3442);
    }

    return;
}



VOS_VOID    NAS_EMM_SER_RcvServiceRejectMsg(const NAS_EMM_CN_SER_REJ_STRU   *pMsgStru)
{
    NAS_EMM_CN_CAUSE_ENUM_UINT8     ucSerRejcause   = NAS_LMM_CAUSE_PROTOCOL_ERROR;

    NAS_EMM_SER_LOG_INFO( "Nas_Emm_Ser_RcvServiceRejectMsg is entered.");

    /*异常停止 SER*/
    NAS_EMM_SER_AbnormalOver();

    /*设置SER的结果为 NAS_EMM_SER_RESULT_REJ*/
    /*NAS_EMM_SER_SaveSERresult(NAS_EMM_SER_RESULT_REJ);*/

    /*提取SERVICE REJ原因值*/
    ucSerRejcause = pMsgStru->ucEMMCause;


    /*设置SER REJ原因*/
    /*NAS_EMM_SER_SaveEmmSERRejCause(ucSerRejcause);*/



    /*SER REJ #9&#10不释放连接，支持处理；
      其他原因值统一处理:释放连接*/
    switch(ucSerRejcause)
    {
        case    NAS_LMM_CAUSE_ILLEGAL_UE   :
        case    NAS_LMM_CAUSE_ILLEGAL_ME   :
        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW   :

                NAS_EMM_SER_SERREJ367(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_UE_ID_NOT_DERIVED    :


                NAS_EMM_SER_SERREJ9(pMsgStru);
               break;

        case    NAS_LMM_CAUSE_IMPLICIT_DETACHED   :
                /*NAS_EMM_TAU_SaveEmmRRCRelCause(NAS_EMM_RRC_REL_CAUSE_SERREJ10);
                break;*/
                NAS_EMM_SER_SERREJ10(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_PLMN_NOT_ALLOW    :
                NAS_EMM_SER_SERREJ11(pMsgStru);
                break;
        case    NAS_LMM_CAUSE_REQUESTED_SER_OPTION_NOT_AUTHORIZED_IN_PLMN:
                if (NAS_RELEASE_CTRL)
                {
                    NAS_EMM_SER_SERREJ11(pMsgStru);
                }
                else
                {
                    NAS_EMM_ServiceReqRejectOtherCause(pMsgStru);
                }
                break;
        case    NAS_LMM_CAUSE_TA_NOT_ALLOW   :
                NAS_EMM_SER_SERREJ12(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_ROAM_NOT_ALLOW    :
                NAS_EMM_SER_SERREJ13(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_NO_SUITABL_CELL   :
                NAS_EMM_SER_SERREJ15(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED:
                if (NAS_RELEASE_CTRL)
                {
                    NAS_EMM_SER_SERREJ40(pMsgStru);
                }
                else
                {
                    NAS_EMM_ServiceReqRejectOtherCause(pMsgStru);
                }
                break;

/* CSG功能尚未实现，收到REJ #25按非CSG处理，进入default处理分支*/
#if 0
        case    NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG:
                NAS_EMM_TAU_SaveEmmRRCRelCause(NAS_EMM_RRC_REL_CAUSE_SERREJ25);
                break;
#endif
        case    NAS_LMM_CAUSE_CS_NOT_AVAIL:
                NAS_EMM_SER_SERREJ18(pMsgStru);
                break;

        case    NAS_LMM_CAUSE_CS_DOMAIN_TMP_NOT_ALLOWED:
                NAS_EMM_SER_SERREJ39(pMsgStru);
                break;

        default:

                NAS_EMM_ServiceReqRejectOtherCause(pMsgStru);
                break;

    }


    return;

}
VOS_VOID NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm(VOS_VOID)
{
    NAS_LMM_PTL_TI_ENUM_UINT16  enPtlTimerId = NAS_LMM_STATE_TI_BUTT;

    if (NAS_EMM_YES == NAS_EMM_IsT3411orT3402Running(&enPtlTimerId))
    {
        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm:Upt-MM state.");

            /* 将状态转移至MS_REG + EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM状态 */
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* 将状态转移至MS_REG + SS_REG_NORMAL_SERVICE状态 */
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }

    }
    else
    {
        if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
        {
            NAS_EMM_TAU_LOG_NORM("NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm: with imsi attach");
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);

            NAS_EMM_SendIntraTauReq(    ID_APP_MM_MSG_TYPE_BUTT,
                                    NAS_LMM_INTRA_TAU_TYPE_UPDATE_MM);
            return;
        }

        NAS_EMM_TAU_LOG_NORM("NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm: normal service state.");
        /* 将状态转移至MS_REG + SS_REG_NORMAL_SERVICE状态 */
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

    }
    return;

}

/* lihong00150010 emergency tau&service begin */

VOS_VOID NAS_EMM_TranStatePsNormalServiceOrPsLimitService(VOS_VOID)
{
    if (VOS_TRUE == NAS_EMM_IsEnterRegLimitService())
    {
        NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();
    }
}
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd( VOS_UINT32 ulCause)
{
    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd is entered.");

    /*停止SERVICE流程*/
    NAS_EMM_SER_AbnormalOver();

    /* CSFB流程的处理 */
    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd:CSFB");

        /*转换EMM状态机MS_REG+SS_REG_NORMAL_SERVICE*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);
        if ((LRRC_LNAS_REL_CAUSE_L2GU_REDIRECT != ulCause) && (LRRC_LNAS_REL_CAUSE_CSFB_HIGH_PRIOR != ulCause))
        {
            /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
            NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        }

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();
        return;
    }
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd:CAUSE_ESM_DATA_REQ_EMC");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_NORMAL_SERVICE);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();
        return;
    }
    /* lihong00150010 emergency tau&service end */

    /*upon different cause value, enter different dealing*/
    switch (ulCause)
    {
        case LRRC_LNAS_REL_CAUSE_LOAD_BALANCE_REQ:
            /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_RRC_REL_LOAD_BALANCE);*/
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE);
            NAS_EMM_TAU_SaveEmmCollisionCtrl(  NAS_EMM_COLLISION_SERVICE);
            /*NAS_EMM_TAU_StartTAUREQ(             NAS_EMM_MSG_LOAD_BALANCING_TAU);*/
            /*
            NAS_EMM_TAUSER_FSMTranState(       EMM_MS_REG,
                                                EMM_SS_REG_PLMN_SEARCH,
                                                TI_NAS_EMM_STATE_NO_TIMER);
            */
            NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();

            /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
            NAS_EMM_CommProcConn2Ilde();
            break;

        case LRRC_LNAS_REL_CAUSE_CONN_FAIL:
            /* NAS_EMM_TAU_SaveEmmTAUStartCause(    NAS_EMM_TAU_START_CAUSE_RRC_REL_CONN_FAILURE); */
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
            NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE);
            /*NAS_EMM_TAU_SaveEmmCollisionCtrl(    NAS_EMM_COLLISION_SERVICE);*/
            /*NAS_EMM_TAU_StartTAUREQ(             NAS_EMM_MSG_LOAD_BALANCING_TAU);*/
            /*
            NAS_EMM_TAUSER_FSMTranState(       EMM_MS_REG,
                                                EMM_SS_REG_PLMN_SEARCH,
                                                TI_NAS_EMM_STATE_NO_TIMER);
            */
            NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();

            /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
            NAS_EMM_CommProcConn2Ilde();
            break;

        default:
            NAS_EMM_SER_RcvRrcRelInd();
            break;
    }

    return;
}
VOS_UINT32 NAS_EMM_MsSerInitSsWaitCnSerCnfMsgAuthFail(
                                                            VOS_UINT32  ulMsgId,
                                                            VOS_VOID   *pMsgStru)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   = (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCnSerCnfMsgAuthFail is entered.");

    /*获得原因值*/
    ulCause                                             =   pMsgAuthFail->ulCause;

    /*依据原因值处理*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL              ==  ulCause)
    {
        NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgAuthRej(      ulCause);

    }
    else
    {
        NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd(    ulCause);
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsSerInitSsWaitCnSerCnfMsgAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        const VOS_VOID   *pMsgStru)

{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_SER_LOG_INFO("NAS_EMM_MsSerInitSsWaitCnSerCnfMsgAuthRej enter!");

    NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);


    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32 NAS_EMM_MsSerInitSsWaitCnSerCnfMsgRrcRelInd(
                                                            VOS_UINT32  ulMsgId,
                                                            VOS_VOID   *pMsgStru)
{
    LRRC_LMM_REL_IND_STRU                 *pRrcRelInd     = (LRRC_LMM_REL_IND_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    (VOS_VOID)ulMsgId;

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCnSerCnfMsgRrcRelInd is entered.");

    /*获得原因值*/
    ulCause                                             =   pRrcRelInd->enRelCause;

    NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgRrcRelInd(        ulCause);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417Exp(  VOS_UINT32  ulMsgId,
                                                             VOS_VOID   *pMsgStru
                                                          )
{
    VOS_UINT32                      ulRslt          = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417Exp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*如果SR流程是由于SMS触发，需要回复SMS建链失败，并且清除SR的发起原因，*/
    if(NAS_EMM_SER_START_CAUSE_SMS_EST_REQ == NAS_EMM_SER_GetSerStartCause())
    {
        NAS_LMM_SndLmmSmsErrInd(LMM_SMS_ERR_CAUSE_OTHERS);
        NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
    }

    /*清空ESM_DATA缓存*/
    NAS_EMM_SerClearEsmDataBuf();
    /*设置SER的结果为 NAS_EMM_SER_RESULT_ABNORMAL*/
    /*NAS_EMM_SER_SaveSERresult(NAS_EMM_SER_RESULT_ABNORMAL);*/

    /*设置报告MMC结果的原因值*/
    /*NAS_EMM_SER_SaveEmmSERRejCause(NAS_LMM_CAUSE_NULL);*/

    /*设置RRC连接释放的原因*/


    /*Inform RABM that SER fail*/
    NAS_EMM_SER_SendRabmReestInd(EMM_ERABM_REEST_STATE_FAIL);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417Exp: CAUSE_ESM_DATA_REQ_EMC.");

        NAS_EMM_EmcPndReqSerAbnormalCommProc(EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        NAS_EMM_TranStatePsNormalServiceOrPsLimitService();
    }
	/* lihong00150010 emergency tau&service end */
    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return NAS_LMM_MSG_HANDLED;

}

VOS_UINT32 NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417ExtExp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                      ulRslt          = NAS_EMM_FAIL;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCnSerCnfMsgTimer3417Exp is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    NAS_EMM_SER_AbnormalOver();

	/* 状态迁移到REG.NORMAL_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                            EMM_SS_REG_NORMAL_SERVICE,
                            TI_NAS_EMM_STATE_NO_TIMER);

    if (NAS_EMM_CSFB_ABORT_FLAG_VALID == NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
    {

        if (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState())
        {
            /* 如果有未完成的TAU，等在REG+NORMAL_SERVICE状态下收到系统消息后再处理 */
            NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
        }
    }
    /* 如果不是紧急CSFB和MO CSFB给MM发终止消息,否则给MMC上报SER结果释放等挂起 */
    else if ((VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb())
         && (VOS_FALSE == NAS_EMM_SER_IsMoCsfbProcedure()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_T3417EXT_TIME_OUT);
        if (NAS_EMM_CONN_DATA != NAS_EMM_GetConnState())
    	{
        	/* 如果有未完成的TAU，等在REG+NORMAL_SERVICE状态下收到系统消息后再处理 */
        	NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    	}
    }
    else
    {
        /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32 NAS_EMM_MsSerInitSsWaitCNSerCnfMsgServiceReject(  VOS_UINT32  ulMsgId,
                                                                  VOS_VOID   *pMsgStru
                                                              )
{
    VOS_UINT32                      ulRslt          = NAS_EMM_FAIL;
    NAS_EMM_CN_SER_REJ_STRU         *pstserrej      = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCNSerCnfMsgServiceReject is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);

    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*停止T3416，删除RAND,RES*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*调用消息处理函数*/
    pstserrej  =  (NAS_EMM_CN_SER_REJ_STRU*)pMsgStru;
    NAS_EMM_SER_RcvServiceRejectMsg(pstserrej);

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgAuthRej(VOS_UINT32  ulCause)
{
    NAS_EMM_SER_LOG_INFO(              "NAS_EMM_MsSerInitSsWaitCnSerCnfProcMsgAuthRej is entered.");

    (VOS_VOID)ulCause;

    /*终止SER*/
    NAS_EMM_SER_AbnormalOver();
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /* 如果是CSFB流程，且不是紧急CSFB，则给MM发终止消息 */
    if ((VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
        && (VOS_TRUE == NAS_EMM_SER_IsNotEmergencyCsfb()))
    {
        NAS_EMM_MmSendCsfbSerEndInd(MM_LMM_CSFB_SERVICE_RSLT_AUTH_REJ);
    }

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /* set the EPS update status to EU3 ROAMING NOT ALLOWED */
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*USIM无效，直到关机或拔卡*/
    /*暂不处理*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
    {
        /* 记录ATTACH触发原因值 */
        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER;

        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_DEREG+SS_DEREG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG, EMM_SS_DEREG_NO_IMSI, TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_AUTH_REJ);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);


    return;
}
/*lint +e961*/
/*lint +e960*/

VOS_VOID NAS_EMM_SerbarCommProc(VOS_VOID)
{
    /*停止SERVICE流程*/
    NAS_EMM_SER_AbnormalOver();
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_WAIT_ACCESS_GRANT_IND, TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_ACCESS_BARED);

    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return;
 }
VOS_VOID  NAS_EMM_SER_ProcMoCallAccessBar(VOS_VOID)

{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ProcMoCallAccessBar is entered!");
    if((NAS_EMM_SER_START_CAUSE_RRC_PAGING == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause()))
    {
       /*响应寻呼的SERVICE流程不出现RRC_EST_ACCESS_BARRED_MO_CALL的情况*/
        NAS_EMM_SER_LOG_WARN("NAS_EMM_SER_ProcMoCallAccessBar:Barred Type ERR!!");
        return;
    }

    NAS_EMM_SerbarCommProc();

    return;
}
VOS_VOID  NAS_EMM_SER_ProcBarAllAccessBar(VOS_VOID)

{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ProcBarAllAccessBar is entered!");
    if (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        NAS_EMM_SER_LOG_WARN("NAS_EMM_SER_ProcBarAllAccessBar:emergency Barred Type ERR!!");
        return;
    }

    NAS_EMM_SerbarCommProc();

    return;
}


VOS_VOID  NAS_EMM_SER_ProcMoCsfbAccessBar(VOS_VOID)

{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ProcMoCsfbAccessBar is entered!");
    if(NAS_EMM_SER_START_CAUSE_MO_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
    {
        /*停止SERVICE流程*/
        NAS_EMM_SER_AbnormalOver();

        /*不需给MMC上报结果，继续在L模，解bar后重新触发CSFB流程*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG, EMM_SS_REG_WAIT_ACCESS_GRANT_IND, TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
        NAS_EMM_CommProcConn2Ilde();
    }
    return;
}
VOS_VOID  NAS_EMM_SER_ProcMoCallAndCsfbAccessBar(VOS_VOID)

{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ProcMoCallAndCsfbAccessBar is entered!");
    if((NAS_EMM_SER_START_CAUSE_RRC_PAGING == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MT_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause())
        || (NAS_EMM_SER_START_CAUSE_MO_EMERGENCY_CSFB_REQ == NAS_EMM_SER_GetEmmSERStartCause()))
    {
        /*响应寻呼的SERVICE流程不出现RRC_EST_ACCESS_BARRED_MO_CALL_AND_CSFB的情况*/
        NAS_EMM_SER_LOG_WARN("NAS_EMM_SER_ProcMoCallAndCsfbAccessBar:mo call and csfb Barred Type ERR!!");
        return;
    }
    NAS_EMM_SerbarCommProc();

    return;
}
VOS_VOID  NAS_EMM_SER_ProcCellSearchFail(VOS_VOID)

{
    NAS_EMM_SER_LOG_NORM("NAS_EMM_SER_ProcCellSearchFail is entered!");
    /*停止SERVICE流程*/
    NAS_EMM_SER_AbnormalOver();

    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        /*向MMC发送LMM_MMC_SERVICE_RESULT_IND消息*/
        NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);
    }

    NAS_EMM_TranStateRegNormalServiceOrRegUpdateMm();
	 
    /*send INTRA_CONN2IDLE_REQ，更新连接状态*/
    NAS_EMM_CommProcConn2Ilde();

    return;
}
VOS_UINT32 NAS_EMM_MsSerInitSsWaitCnSerCnfMsgIntraConnectFailInd(   VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsgStru)
{

    VOS_UINT32                          ulRslt;
    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *pMrrcConnectFailRelInd       = NAS_EMM_NULL_PTR;
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;

    NAS_EMM_SER_LOG_INFO( "NAS_EMM_MsSerInitSsWaitCnSerCnfMsgIntraConnectFailInd is entered.");
    pMrrcConnectFailRelInd              = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_SER_CHKFSMStateMsgp(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_SER_LOG_WARN( "NAS_EMM_MsSerInitSsWaitCnSerCnfMsgIntraConnectFailInd ERROR !!");
        return NAS_LMM_MSG_DISCARD;
    }
    #if 0
    /*停止SERVICE流程*/
    NAS_EMM_SER_AbnormalOver();
    #endif

    /* lihong00150010 emergency tau&service begin */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        /*停止SERVICE流程*/
        NAS_EMM_SER_AbnormalOver();

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

            NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                    EMM_SS_DEREG_PLMN_SEARCH,
                                    TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /*保存ESM消息*/
            /*lint -e960*/
            NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                                   &(pstEsmMsg->stEsmMsg),
                                    (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);
            /*lint +e960*/
            NAS_EMM_StartEmergencyAttach();
        }

        return  NAS_LMM_MSG_HANDLED;
    }
    /* lihong00150010 emergency tau&service end */

    /*Process according different EST result*/
    switch(pMrrcConnectFailRelInd->enEstResult)
    {
        case    LRRC_EST_ACCESS_BARRED_MO_CALL:

                /*设置bar流程和类型*/
                NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_SERVICE, pMrrcConnectFailRelInd->enEstResult);

                NAS_EMM_SER_ProcMoCallAccessBar();

                break;
        case    LRRC_EST_ACCESS_BARRED_ALL:

                /*设置bar流程和类型*/
                NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_SERVICE, pMrrcConnectFailRelInd->enEstResult);

                NAS_EMM_SER_ProcBarAllAccessBar();
                break;

        case    LRRC_EST_ACCESS_BARRED_MO_CSFB:
                /*设置bar流程和类型*/
                NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_SERVICE, pMrrcConnectFailRelInd->enEstResult);

                NAS_EMM_SER_ProcMoCsfbAccessBar();
                break;

        case    LRRC_EST_ACCESS_BARRED_MO_CALL_AND_CSFB:

                /*设置bar流程和类型*/
                NAS_EMM_SetBarInfo(NAS_EMM_BAR_PROCEDURE_SERVICE, pMrrcConnectFailRelInd->enEstResult);

                NAS_EMM_SER_ProcMoCallAndCsfbAccessBar();
                break;
                /* 建链失败，RRC正在进行小区搜索，迁移到Reg.Plmn_Search*/
        case    LRRC_EST_CELL_SEARCHING:

                NAS_EMM_SER_ProcCellSearchFail();
                break;

        default:
                /*停止SERVICE流程*/
                NAS_EMM_SER_AbnormalOver();
                NAS_EMM_SER_RcvRrcRelInd();

                break;

    }
    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_SndServiceReqFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{

    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;

    *pulIsDelBuff = VOS_TRUE;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SndServiceReqFailProc: STATE ERR!");
        return  NAS_EMM_SUCC;
    }
    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    pEmmMrrcMgmtData = NAS_EMM_FindMsgInDataReqBuffer(pstRrcMmDataCnf->ulOpId);

    if (NAS_EMM_NULL_PTR == pEmmMrrcMgmtData)
    {
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
            /*停止T3417定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*停止T3440定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            /*停止T3417ext定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*启动定时器3417*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417);

            /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
            NAS_EMM_SER_SendMrrcDataReq_ServiceReq();

            break;

        default:
            break;
        }

    return NAS_EMM_SUCC;
}
VOS_UINT32 NAS_EMM_SndExtendedServiceReqFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    LRRC_LMM_DATA_CNF_STRU              *pstRrcMmDataCnf = VOS_NULL_PTR;
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;


    *pulIsDelBuff = VOS_TRUE;

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SndExtendedServiceReqFailProc: STATE ERR!");
        return  NAS_EMM_SUCC;
    }

    if (NAS_EMM_CSFB_ABORT_FLAG_VALID == NAS_EMM_SER_GetEmmSerCsfbAbortFlag())
    {
        /*打印出错信息*/
        NAS_EMM_SER_LOG_INFO("NAS_EMM_SndExtendedServiceReqFailProc: ABORT!");
        return  NAS_EMM_SUCC;
    }

    pstRrcMmDataCnf = (LRRC_LMM_DATA_CNF_STRU*) pMsg;

    pEmmMrrcMgmtData = NAS_EMM_FindMsgInDataReqBuffer(pstRrcMmDataCnf->ulOpId);

    if (NAS_EMM_NULL_PTR == pEmmMrrcMgmtData)
    {
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
            /* 重启SERVICE 流程 */
            /*停止T3417定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417);

            /*停止T3440定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            /*停止T3417ext定时器*/
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*启动定时器3417*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*转换EMM状态机MS_SER_INIT+SS_SER_WAIT_CN_CNF*/
            NAS_EMM_TAUSER_FSMTranState(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF, TI_NAS_EMM_STATE_SERVICE_T3417_EXT);

            /*组合并发送MRRC_DATA_REQ(SERVICE_REQ)*/
            NAS_EMM_SER_SendMrrcDataReq_ExtendedServiceReq();

            break;

        default:
            break;
        }

    return NAS_EMM_SUCC;
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


