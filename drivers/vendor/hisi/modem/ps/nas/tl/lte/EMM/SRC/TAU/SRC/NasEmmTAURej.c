

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmTauSerInclude.h"
#include "NasEmmAttDetInclude.h"
#include "LNasStkInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMTAUREJ_C
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





VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue367( )
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue367 is entered.");


    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*USIM无效，直到关机或拔卡*/
    /*暂不处理*/

    /*删除equivalent PLMNs由MMC模块完成*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;
}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue8( )
{

    MMC_LMM_TAU_TYPE_ENUM_UINT32            enEmmTauType = MMC_LMM_TAU_TYPE_BUTT;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_Tau_TAUReqRejectCasue8 is entered.");

    enEmmTauType = NAS_EMM_TAU_GetTAUtype();

    if ((NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING != enEmmTauType)
        && (NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI != enEmmTauType))
    {
        /* 非联合TAU的处理分支 */
        NAS_EMM_TAU_ProcAbnormal();
		/* lihong00150010 emergency tau&service begin */
        if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue8:CAUSE_ESM_EMC_PDN_REQ");

            NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRej,
                                                    (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                    EMM_SS_DEREG_NORMAL_SERVICE);
        }
        else
        {
            /* 上报TAU结果在冲突处理中执行 */
            NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                              (VOS_VOID*)NAS_EMM_NULL_PTR);
        }
		/* lihong00150010 emergency tau&service end */

        #if 0
        /*如果启动了T3402定时器，清零TAU计数*/
        if(NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
        {
            NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
        }
        #endif
        /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

        return ;
    }

    /* 联合TAU的处理分支 */

    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*USIM无效，直到关机或拔卡*/
    /*暂不处理*/

    /*删除equivalent PLMNs由MMC模块完成*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    /*转换EMM状态机MS_DEREG+SS_DEREG_NO_IMSI*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;
}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue9( )
{
    VOS_UINT32                          ulAttachFlag    = VOS_TRUE;
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg       = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_Tau_TAUReqRejectCasue9 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置设置EU2 NOT UPDATED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*删除equivalent PLMNs由MMC模块完成*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_DEREG+SS_DEREG_NORMAL_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);
                NAS_EMM_TAU_CollisionDetachProc();
                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue9:Process EPS/Combined Detach");

                    /* 本地DETACH*/
                    NAS_LMM_DeregReleaseResource();


                    NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
                    return;
                }

                NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue9:Process IMSI Detach");

                /* IMSI DETACH后释放资源 */
                NAS_LMM_ImsiDetachReleaseResource();

                break;

        case    NAS_EMM_COLLISION_SERVICE    :

                /* 如果是紧急CSFB，则标识不发内部ATTACH */
                if(VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb())
                {
                   ulAttachFlag = VOS_FALSE;
                }

                NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndRej,
                                  (VOS_VOID*)NAS_EMM_NULL_PTR,
                                  NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
                break;

        default :

                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_RrcRelCause_TAUREJ9 : NO Emm Collision.");
                NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);
                break;
    }
	/* lihong00150010 emergency tau&service begin */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_RrcRelCause_TAUREJ9: CAUSE_ESM_EMC_PDN_REQ");

        /*保存ESM消息*/
        /*lint -e960*/
        NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);
        /*lint +e960*/
        NAS_EMM_StartEmergencyAttach();
    }
    else if(VOS_TRUE == ulAttachFlag)
    {/* lihong00150010 emergency tau&service end */
        /* 延时发送attach请求 */
        NAS_EMM_ReattachDelay();
    }
    else
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    /* 清空TAU流程所有资源*/
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;

}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue10(VOS_VOID)
{
    VOS_UINT32                          ulAttachFlag    = VOS_TRUE;
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg       = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue10 is entered.");
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /* 状态迁移Dereg.Normal_Service，发送INTRA_ATTACH_REQ消息*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 进入DEREG态不再写统一写安全上下文，参照标杆，在TAU被#10拒绝，将安全上下文写卡 */
    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);

    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {

        case NAS_EMM_COLLISION_DETACH:

            /* 向MMC报TAU结果需在DETACH结果上报前 */
            NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);

            NAS_EMM_TAU_CollisionDetachProc();

            if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
            {
                NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue10:Process EPS Detach or EPS/IMSI Detach");

                /* 本地DETACH*/
                NAS_LMM_DeregReleaseResource();
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
                return;
            }

            NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue10:Process IMSI Detach");

            /* IMSI DETACH后释放资源 */
            NAS_LMM_ImsiDetachReleaseResource();
            break;

        case NAS_EMM_COLLISION_SERVICE:

            /* 如果是紧急CSFB，则标识不发内部ATTACH */
            if(VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb())
            {
               ulAttachFlag = VOS_FALSE;
            }

            /* 上报TAU结果在SER冲突处理中执行 */
            NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndRej,
                         (VOS_VOID*)NAS_EMM_NULL_PTR,
                         NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
            break;

        default:

            NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);
            break;
    }
	/* lihong00150010 emergency tau&service begin */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_RrcRelCause_TAUREJ9: CAUSE_ESM_EMC_PDN_REQ");

        /*保存ESM消息*/
        /*lint -e960*/
        NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);
        /*lint +e960*/

        NAS_EMM_StartEmergencyAttach();
    }
    else if(VOS_TRUE == ulAttachFlag)
    {/* lihong00150010 emergency tau&service end */
        /* 延时发送attach请求 */
        NAS_EMM_ReattachDelay();
    }
    else
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    /* 清空TAU和APP资源*/
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;
}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue11()
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue11 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*删除equivalent PLMNs由MMC模块完成*/

    /*Reset the TAU attempt counter*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    /*为上报服务状态,转换EMM状态机MS_DEREG+SS_DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    NAS_EMM_WaitNetworkRelInd();

    return;

}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue12()
{
    NAS_MM_TA_STRU                      stTa;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue12 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*store the current TAI in the list of
              "forbidden tracking areas for regional provision of service"*/
    NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr());

    /* 增加REJ cause 12标记,用于向MMC上报有区域限制的限制服务状态 2011-07-27*/
    NAS_LMM_SetNasRejCause12Flag(NAS_LMM_REJ_CAUSE_IS_12);

    /*重置TAU计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    NAS_EMM_WaitNetworkRelInd();
    return;

}


VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue13()
{
    NAS_MM_TA_STRU                      stCurrentTa;
    NAS_MM_TA_LIST_STRU                *pstTaiList;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_Tau_TAUReqRejectCasue13 is entered.");

    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;
    /* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
    /* lihong00150010 emergency tau&service end */
    /*重置TAU计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*Remove current TA from TAI List*/
    NAS_EMM_GetCurrentTa(&stCurrentTa);

    pstTaiList = NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
    NAS_LMM_DeleteTaFromTaList(&stCurrentTa, pstTaiList, NAS_MM_MAX_TA_NUM);

    /*store the current TAI in the list of
                "forbidden tracking areas for roaming"*/
    NAS_EMM_AddForbTa(&stCurrentTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());
    NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue13:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRej,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_LIMITED_SERVICE);
    }
    else
    {
        /*发送EMM_ESM_STATUS_IND(去注册)*/
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);

        /*为上报服务状态,转换EMM状态机MS_DEREG+SS_REG_LIMITED_SERVICE*/
        /*在收到MMC搜网指令后将转入REG_PLMN_SEARCH态*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                    EMM_SS_REG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /****检查冲突标志并做相应的处理********************************************/
        /* 上报TAU结果在冲突中处理 */
        NAS_EMM_TAU_RelCause1315CollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                        (VOS_VOID*)NAS_EMM_NULL_PTR);
    }
    /* lihong00150010 emergency tau&service end */

    NAS_EMM_WaitNetworkRelInd();

    return;

}
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue14()
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue14 is entered.");

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*重置TAU计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    /*为上报服务状态,转换EMM状态机MS_DEREG+SS_DEREG_LIMITED_SERVICE*/
    /*在收到MMC搜网指令后将转入DEREG_PLMN_SEARCH态*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);
    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    NAS_EMM_WaitNetworkRelInd();

    return;

}


VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue15()
{
    NAS_MM_TA_STRU                     stCurrentTa;
    NAS_MM_TA_LIST_STRU                *pstTaiList;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasue15 is entered.");

    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;
    /* lihong00150010 emergency tau&service begin */
    if (NAS_LMM_REG_STATUS_EMC_REGED != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
    /* lihong00150010 emergency tau&service end */
    /*重置TAU计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*Remove current TA from TAI List*/
    NAS_EMM_GetCurrentTa(&stCurrentTa);

    pstTaiList = NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
    NAS_LMM_DeleteTaFromTaList(&stCurrentTa, pstTaiList, NAS_MM_MAX_TA_NUM);

    if (NAS_EMMC_USER_SPEC_PLMN_YES == NAS_EMMC_GetUserSpecPlmnFlag())
    {
        NAS_EMM_AddForbTa(&stCurrentTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRej15Addr());
    }
    else
    {
        /*store the current TAI in the list of
                "forbidden tracking areas for roaming"*/
        NAS_EMM_AddForbTa(&stCurrentTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());
        NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);
    }

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue15:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRej,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_LIMITED_SERVICE);
    }
    else
    {
        /*发送EMM_ESM_STATUS_IND(去注册)*/
    	NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);

        /*转换EMM状态机MS_REG+SS_REG_LIMITED_SERVICE*/
        NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                    EMM_SS_REG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /****检查冲突标志并做相应的处理********************************************/
        /* 上报TAU结果在冲突中处理 */
        NAS_EMM_TAU_RelCause1315CollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                        (VOS_VOID*)NAS_EMM_NULL_PTR);
    }
    /* lihong00150010 emergency tau&service end */

    NAS_EMM_WaitNetworkRelInd();

    return;
}
/* lihong00150010 emergency tau&service begin */
#if 0
VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue25(VOS_VOID)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_TAUReqRejectCasue25 is entered.");

    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    /*设置EU3 ROAMING NOT ALLOWED*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*重置TAU计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /* 删除 CSG ID  LIST*/
    /* CGS 待实现 */

    /*发送EMM_ESM_STATUS_IND(去注册)*/
    NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*转换EMM状态机MS_REG+SS_REG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_REG,
                                EMM_SS_REG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /****检查冲突标志并做相应的处理********************************************/
    /* 上报TAU结果在冲突中处理 */
    NAS_EMM_TAU_SpecialRelCauseCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
                (VOS_VOID*)NAS_EMM_NULL_PTR);

    return;
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_VOID NAS_EMM_TAU_TAUReqRejectCasue40()
{
    VOS_UINT32                          ulAttachFlag    = VOS_TRUE;
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg       = NAS_EMM_NULL_PTR;

    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_TAUReqRejectCasue40 is entered.");
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        NAS_EMM_TAU_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }
	/* lihong00150010 emergency tau&service end */
    /*转换EMM状态机MS_REG+SS_REG_LIMITED_SERVICE*/
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG, EMM_SS_DEREG_NORMAL_SERVICE, TI_NAS_EMM_STATE_NO_TIMER);

    /* 上报TAU结果在冲突处理中执行 */
    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);

                if (MMC_LMM_MO_DET_CS_ONLY != NAS_EMM_GLO_AD_GetDetTypeMo())
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue40:Process EPS Detach or EPS/IMSI Detach");

                    NAS_EMM_TAU_CollisionDetachProc();

                    /* 本地DETACH*/
                    NAS_LMM_DeregReleaseResource();

                    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
                    return;
                }
                else
                {
                    NAS_EMM_DETACH_LOG_INFO("NAS_EMM_TAU_TAUReqRejectCasue40:Process IMSI Detach");

                    NAS_EMM_TAU_CollisionDetachProc();

                    /* IMSI DETACH后释放资源 */
                    NAS_LMM_ImsiDetachReleaseResource();
                }
                break;

        case    NAS_EMM_COLLISION_SERVICE    :

                /* 如果是紧急CSFB，则标识不发内部ATTACH */
                if(VOS_FALSE == NAS_EMM_SER_IsNotEmergencyCsfb())
                {
                   ulAttachFlag = VOS_FALSE;
                }

                NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndRej,
                                  (VOS_VOID*)NAS_EMM_NULL_PTR,
                                  NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
                break;

        default :

                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_TAU_TAUReqRejectCasue40 : NO Emm Collision.");
                NAS_EMM_MmcSendTauActionResultIndRej((VOS_VOID*)NAS_EMM_NULL_PTR);
                break;
    }
	/* lihong00150010 emergency tau&service begin */
    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)(VOS_VOID*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_RrcRelCause_TAUREJ9: CAUSE_ESM_EMC_PDN_REQ");

        /*保存ESM消息*/
        /*lint -e960*/
        NAS_LMM_MEM_CPY(        (VOS_VOID*)NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);
        /*lint +e960*/
        NAS_EMM_StartEmergencyAttach();
    }
    else if (VOS_TRUE == ulAttachFlag)
    {/* lihong00150010 emergency tau&service end */
        /* 延时发送attach请求 */
        NAS_EMM_ReattachDelay();
    }
    else
    {
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    /* 清空TAU流程所有资源*/
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_ClearAppMsgPara();

    return;
}
VOS_VOID NAS_EMM_TAU_TAUReqRejectOtherCause
(
    const NAS_EMM_CN_CAUSE_ENUM_UINT8   ucTAUrejcause
)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO( "Nas_Emm_Tau_TAUReqRejectCasueOthers is entered.");

    if( (NAS_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG == ucTAUrejcause)||
        (NAS_LMM_CAUSE_INVALID_MANDATORY_INF == ucTAUrejcause)||
        (NAS_LMM_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE == ucTAUrejcause)||
        (NAS_LMM_CAUSE_IE_NONEXIST_NOTIMPLEMENTED == ucTAUrejcause)||
        (NAS_LMM_CAUSE_PROTOCOL_ERROR == ucTAUrejcause))
    {
        NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_MAX);
    }
    else
    {
        NAS_EMM_TAU_GetEmmTAUAttemptCnt()++;
    }

    NAS_EMM_TAU_ProcAbnormal();

    /* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_TAU_TAUReqRejectOtherCause:CAUSE_ESM_EMC_PDN_REQ");

        NAS_EMM_EmcPndReqTauAbnormalCommProc(   NAS_EMM_MmcSendTauActionResultIndRej,
                                                (VOS_VOID*)NAS_EMM_NULL_PTR,
                                                EMM_SS_DEREG_NORMAL_SERVICE);
    }
    else
    {
        NAS_EMM_TAU_RelIndCollisionProc(NAS_EMM_MmcSendTauActionResultIndRej,
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
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return;

}
VOS_VOID    NAS_EMM_TAU_RcvTAURej
(
    const NAS_EMM_CN_CAUSE_ENUM_UINT8   ucTAUrejcause
)
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_INFO("Nas_Emm_Tau_RcvTAURej is entered.");

    /* 停止定时器3430*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_TAU_T3430);

    /* 根据原因值做不同处理*/
    switch(ucTAUrejcause)
    {
        case    NAS_LMM_CAUSE_ILLEGAL_UE   :
        case    NAS_LMM_CAUSE_ILLEGAL_ME   :
        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW   :
                NAS_EMM_TAU_TAUReqRejectCasue367();
                break;

        case    NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW:
                NAS_EMM_TAU_TAUReqRejectCasue8();
                break;

        case    NAS_LMM_CAUSE_UE_ID_NOT_DERIVED    :
                NAS_EMM_TAU_TAUReqRejectCasue9();
                break;

        case    NAS_LMM_CAUSE_IMPLICIT_DETACHED   :
                /* 必须先给MMC发送LMM_MMC_TAU_RESULT_IND，后处理拒绝原因值10，
                   因为NAS_EMM_TAU_TAUReqRejectCasue10函数会调用NAS_EMM_TAU_ClearResouce
                   请求TAU流程所有资源，如果先处理拒绝原因值10，则在发送LMM_MMC_TAU_RESULT_IND
                   时，无法得到正确的TAU类型 */

                /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
                NAS_EMM_TAU_TAUReqRejectCasue10();
                break;

        case    NAS_LMM_CAUSE_PLMN_NOT_ALLOW    :
                NAS_EMM_TAU_TAUReqRejectCasue11();
                break;
        case    NAS_LMM_CAUSE_REQUESTED_SER_OPTION_NOT_AUTHORIZED_IN_PLMN:
                if (NAS_RELEASE_CTRL)
                {
                    NAS_EMM_TAU_TAUReqRejectCasue11();
                }
                else
                {
                    NAS_EMM_TAU_TAUReqRejectOtherCause(ucTAUrejcause);
                }
                break;
        case    NAS_LMM_CAUSE_TA_NOT_ALLOW   :
                NAS_EMM_TAU_TAUReqRejectCasue12();
                break;

        case    NAS_LMM_CAUSE_ROAM_NOT_ALLOW    :
                NAS_EMM_TAU_TAUReqRejectCasue13();
                break;

        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN    :
                NAS_EMM_TAU_TAUReqRejectCasue14();
                break;

        case    NAS_LMM_CAUSE_NO_SUITABL_CELL   :
                NAS_EMM_TAU_TAUReqRejectCasue15();
                break;

/* CSG功能尚未实现，收到REJ #25按非CSG处理，进入default处理分支*/
#if 0
        case    NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG   :
                NAS_EMM_TAU_TAUReqRejectCasue25();

                #if 0
                /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
                NAS_EMM_MmcSendTauActionResultIndRej(ucTAUrejcause);
                #endif
                break;
#endif
        case    NAS_LMM_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED:
                NAS_EMM_TAU_TAUReqRejectCasue40();
                break;

        default :
                NAS_EMM_TAU_TAUReqRejectOtherCause(ucTAUrejcause);
                break;
    }

    return;
}
VOS_UINT32 NAS_EMM_MsTauInitSsWaitCNCnfMsgTAURej(VOS_UINT32  ulMsgId,
                                                   VOS_VOID   *pMsgStru
                               )
{
    VOS_UINT32                          ulRslt      = NAS_EMM_FAIL;
    NAS_EMM_CN_TAU_REJ_STRU            *pstTAURej   = NAS_EMM_NULL_PTR;
    /*lifuxin00253982 clear the pclint error*/
    LNAS_STK_UPDATE_TYPE_ENUM_UINT8     uTauType = LNAS_STK_UPDATE_TYPE_BUTT;

    (VOS_VOID)ulMsgId;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_TAU_LOG_NORM( "Nas_Emm_MsTauInitSsWaitCNCnfMsgTAURej is entered.");

    /* 函数输入指针参数检查, 状态匹配检查,若不匹配,退出*/
    ulRslt = NAS_EMM_TAU_CHKFSMStateMsgp(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF,pMsgStru);
    if ( NAS_EMM_SUCC != ulRslt )
    {
        NAS_EMM_TAU_LOG_WARN("NAS_EMM_TAUSER_CHKFSMStateMsgp ERROR !!");
        return ulRslt;
    }

    /*停止T3416，删除RAND,RES*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*调用消息处理函数*/
    pstTAURej = (NAS_EMM_CN_TAU_REJ_STRU*) pMsgStru;
    /*保存TAU拒绝原因*/
    NAS_EMM_TAU_GetEmmTauCnRejCause() =  pstTAURej->ucEMMCause;

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_LMM_ErrlogInfoProc(pstTAURej->ucEMMCause);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */

    /* 根据当前场景考虑是否需要修改原因值 */
    NAS_LMM_AdaptRegRejCau(&NAS_EMM_TAU_GetEmmTauCnRejCause());

    switch(NAS_EMM_TAU_GetTAUtype())
    {
        case NAS_EMM_CN_TAU_TYPE_TA_UPDATING:
                uTauType = LNAS_STK_UPDATE_TYPE_TA_UPDATEING;
                break;

        case NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_UPDATING:
                uTauType = LNAS_STK_UPDATE_TYPE_COMBINED_TALA_UPDATING ;
                break;

        case NAS_EMM_CN_TAU_TYPE_COMBINED_TA_LA_WITH_IMSI:
                uTauType = LNAS_STK_UPDATE_TYPE_COMBINED_TALA_UPDATING_WITH_IMSI_ATTACH;
                break;

        case NAS_EMM_CN_TAU_TYPE_PERIODIC_UPDATING:
                uTauType = LNAS_STK_UPDATE_TYPE_PERIODIC_UPDATING;
                break;

        default:
                NAS_EMM_TAU_LOG_ERR("NAS_EMM_SendRejEventToSTK: TAU Type error!");
                break;
    }

    /*当TAU rej时，将rej事件发送给STK*/
    NAS_EMM_SendRejEventToSTK(NAS_EMM_TAU_GetEmmTauCnRejCause(), uTauType);

    NAS_EMM_TAU_RcvTAURej(NAS_EMM_TAU_GetEmmTauCnRejCause());

    return NAS_LMM_MSG_HANDLED;
}


VOS_VOID NAS_EMM_MsTauInitSsWaitCnTauCnfProcMsgAuthRej(
                                                   VOS_UINT32  ulCause)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        enTauRslt = MMC_LMM_TAU_RSLT_AUTH_REJ;

    (VOS_VOID)ulCause;

    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsTauInitSsWaitCnTauCnfProcMsgAuthRej is entered.");

    /*停止定时器*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_STATE_TAU_T3430);
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /* 删除GUTI后会自动保存EPS_LOC,所以需要先设置STATUS */
    /*设置EU3 ROAMING NOT ALLOWED，并设置到NVIM中*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*USIM无效，直到关机或拔卡*/
    /*暂不处理*/

    /*删除equivalent PLMNs由MMC模块完成*/
	/* lihong00150010 emergency tau&service begin */
    if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
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
    NAS_EMM_TAUSER_FSMTranState(EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 上报TAU结果在冲突处理中执行 */
    /****检查冲突标志并做相应的处理********************************************/
    switch(NAS_EMM_TAU_GetEmmCollisionCtrl())
    {
        case    NAS_EMM_COLLISION_DETACH   :

                NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);
                NAS_EMM_TAU_CollisionDetachProc();
                break;

        case    NAS_EMM_COLLISION_SERVICE    :

                NAS_EMM_TAU_CollisionServiceProc(NAS_EMM_MmcSendTauActionResultIndOthertype,
                                (VOS_VOID*)&enTauRslt,
                                NAS_EMM_TRANSFER_RAT_NOT_ALLOW);
                break;

        default :

                NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsTauInitSsWaitCnTauCnfProcMsgAuthRej : NO Emm Collision.");
                NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&enTauRslt);
                break;
    }

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



