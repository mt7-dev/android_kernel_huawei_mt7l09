

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasEmmPubUResume.h"
#include "NasEmmPubUSuspend.h"
#include "NasLmmPubMFsm.h"
#include "NasLmmPublic.h"
#include "NasLmmPubMOsa.h"
#include "NasEmmPubUGlobal.h"
#include "NasLmmPubMTimer.h"
#include "MmcLmmInterface.h"
#include "NasEmmcEmmInterface.h"
#include "NasEmmTauInterface.h"
#include "NasEmmMrrcPubInterface.h"
#include "NasEmmAttDetInterface.h"
#include "NasEmmPlmnInterface.h"
#include "NasEmmSecuInterface.h"
#include "NasEmmAttDetInclude.h"

#include "NasEmmTAUProc.h"
#include "NasEmmPlmn.h"
#include "NasEmmPubUGlobal.h"


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

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMPUBURESUME_C
/*lint +e767*/


/* DCM定制需求 GU到L的切换不判断被禁TA开关, 初始化为关闭 */
VOS_UINT32    g_ulHoIgnoreForbidTaFlag = NAS_EMM_HO_IGNORE_FORBID_TA_FLAG_INVALID;


/*****************************************************************************
  3 Function
*****************************************************************************/

/*****************************************************************************
 Function Name   : NAS_EMM_PUBU_FSMTranState
 Description     :
 Input           :
 Output          :
 Return          :

 History         :
    1.HanLufeng 41410      2011-4-26  Draft Enact
*****************************************************************************/
VOS_VOID    NAS_EMM_PUBU_FSMTranState(
                                    NAS_EMM_MAIN_STATE_ENUM_UINT16  usMs,
                                    NAS_EMM_SUB_STATE_ENUM_UINT16   usSs,
                                    NAS_EMM_STATE_TI_ENUM_UINT16    usTi)
{
    NAS_LMM_FSM_STATE_STRU              EmmStat;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PUBU_FSMTranState is entered.");

    EmmStat.enFsmId                   = NAS_LMM_PARALLEL_FSM_EMM;
    EmmStat.enMainState               = usMs;
    EmmStat.enSubState                = usSs;
    EmmStat.enStaTId                  = usTi;
    NAS_LMM_StaTransProc(EmmStat);

    /* 启动定时器 */
    NAS_LMM_StartStateTimer(usTi);

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SaveLrrcResumePara
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-28  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMM_SaveLrrcResumePara( const LRRC_LMM_RESUME_IND_STRU  *pLrrcLmmResumeInd )
{
    NAS_EMM_SetResumeOri(NAS_LMM_SYS_CHNG_ORI_LRRC);
    NAS_EMM_SetResumeDir(pLrrcLmmResumeInd->enRsmDir);
    NAS_EMM_SetResumeType(pLrrcLmmResumeInd->enResumeType);

    NAS_LMM_MEM_CPY( NAS_EMM_GetStateBeforeResumeAddr(),
                    NAS_LMM_GetCurFsmAddr(NAS_LMM_PARALLEL_FSM_EMM),
                    sizeof(NAS_LMM_FSM_STATE_STRU));
    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SaveMmcResumePara
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-28  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SaveMmcResumePara( VOS_VOID )
{
    NAS_EMM_SetResumeOri(NAS_LMM_SYS_CHNG_ORI_MMC);

    NAS_LMM_MEM_CPY( NAS_EMM_GetStateBeforeResumeAddr(),
                    NAS_LMM_GetCurFsmAddr(NAS_LMM_PARALLEL_FSM_EMM),
                    sizeof(NAS_LMM_FSM_STATE_STRU));

    return;
}



VOS_UINT32  NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru )
{
    LRRC_LMM_RESUME_IND_STRU  *pLrrcLmmResumeInd;

    /*Coverity 告警消除 2013-9-26 Begin*/
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcRegState = MMC_LMM_REGISTER_STATUS_BUTT;
    /*Coverity 告警消除 2013-9-26 End*/

    (VOS_VOID)ulMsgId;

    pLrrcLmmResumeInd = (LRRC_LMM_RESUME_IND_STRU *)pMsgStru;

    NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd: DIR = , enResumeType =.",
                        pLrrcLmmResumeInd->enRsmDir,
                        pLrrcLmmResumeInd->enResumeType);

    /* 记录RESUME参数 */
    NAS_EMM_SaveLrrcResumePara(pLrrcLmmResumeInd);

    /* 激活ESM, ERABM */
    NAS_EMM_SendEsmResumeInd();
    NAS_EMM_SendErabmResumeInd();

    /* 状态迁移 */
    NAS_EMM_PUBU_FSMTranState(  EMM_MS_RESUME,
                                EMM_SS_RESUME_RRCRSM_WAIT_OTHER_RESUME,
                                TI_NAS_EMM_RRCRSM_WAIT_OTHER_RESUME_RSP_TIMER);

    /* 当前的 LTE为激活态 */
    NAS_EMM_GetCurLteState() = NAS_LMM_CUR_LTE_ACTIVE;
    if(LRRC_LMM_SYS_CHNG_TYPE_RESEL_REVERSE > pLrrcLmmResumeInd->enResumeType)
    {
        NAS_LMM_CovertMmlRegStatusToLmmRegStatus(NAS_MML_GetPsRegStatus(),&ulNasMmcRegState);

        NAS_LMM_SetNasMmcRegisterState(ulNasMmcRegState);

        /*NAS_EMM_SendMmcRegisterStatInd(NAS_EMM_REG_CHANGE_TYPE_REG_STATE);*/

        /*清空服务状态 */
        NAS_LMM_SetNasMmcCSServiceState(MMC_LMM_SERVICE_STATUS_TYPE_BUTT);
        NAS_LMM_SetNasMmcServiceState(MMC_LMM_SERVICE_STATUS_TYPE_BUTT);

        /* 删除当前小区 */
        NAS_EMMC_DeleteCurrentAreaCellId();
    }

    /* 判读如果是HO类型的resume,则清空映射安全上下文产生的标志*/
    if(LRRC_LMM_SYS_CHNG_TYPE_HO == pLrrcLmmResumeInd->enResumeType)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd:HO succ.");
        NAS_EMM_SecuSetInterRatHoNotMapped();

        /*在GU->L的切换场景，先将L的链路状态切换为信令连接态，便于处理后续收到的RABM DRB建立的消息 */
        NAS_EMM_ChangeRrcConnStateToEstSignal();

        /* 停止T3412和T3423定时器*/
        NAS_LMM_StopPtlTimer(               TI_NAS_EMM_PTL_T3412);
        NAS_LMM_StopPtlTimer(               TI_NAS_EMM_PTL_T3423);

        /*leili modify for isr begin*/

        /*通知GU模T3412停止*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3412, LMM_MMC_TIMER_STOP);

        /*通知GU模T3423停止*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3423, LMM_MMC_TIMER_STOP);

        /*leili modify for isr end*/
    }

    /*leili modify for isr begin*/

    /*RESEL/HO/REDIR:L模收到RESUME消息后就认为该过程结束，不会回退到GU模*/
    if ((LRRC_LMM_SYS_CHNG_TYPE_RESEL == pLrrcLmmResumeInd->enResumeType) ||
        (LRRC_LMM_SYS_CHNG_TYPE_HO == pLrrcLmmResumeInd->enResumeType) ||
        (LRRC_LMM_SYS_CHNG_TYPE_REDIR == pLrrcLmmResumeInd->enResumeType))
    {
        if (EMM_MS_REG != NAS_EMM_GetMsBefResume())
        {
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd:HO succ.");
            return NAS_LMM_MSG_HANDLED;
        }
        NAS_EMM_ProcInterSystemTransferBearerInfo();
    }
    /*CCO过程等待触发TAU建链成功后才认为该过程结束:存在ISR激活后承载，如果CCO成功
       去激活ISR并更新承载的ISR标识；如果CCO失败不去激活ISR*/
    if (LRRC_LMM_SYS_CHNG_TYPE_CCO == pLrrcLmmResumeInd->enResumeType)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd:CCO TYPE.");
        if (EMM_MS_REG != NAS_EMM_GetMsBefResume())
        {
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd:HO succ.");
            return NAS_LMM_MSG_HANDLED;
        }
        if ((MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()) &&
            (NAS_EMM_SUCC == NAS_EMM_IsBearIsrActAfter()))
        {
            NAS_LMM_SetEmmInfoTinChagFlag(NAS_EMM_CCO_CHANGE_TIN_VALID);
            NAS_LMM_SetTempEmmInfoTinType(MMC_LMM_TIN_RAT_RELATED_TMSI);
            NAS_EMM_SetTinType(MMC_LMM_TIN_P_TMSI);
        }
    }
    /*leili modify for isr end*/
    return NAS_LMM_MSG_HANDLED;

}


/*****************************************************************************
 Function Name   : NAS_EMM_SendMmcResumeInd
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SendMmcResumeInd( VOS_VOID )
{
    LMM_MMC_RESUME_IND_STRU           *pstMmcSuspendIndMsg;

    /* 申请MMC内部消息 */
    pstMmcSuspendIndMsg = (VOS_VOID *) NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_RESUME_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstMmcSuspendIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendMmcResumeInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstMmcSuspendIndMsg, 0, sizeof(LMM_MMC_RESUME_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_MMC_MSG_HEADER((pstMmcSuspendIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LMM_MMC_RESUME_IND_STRU));

    /* 填充消息ID */
    pstMmcSuspendIndMsg->ulMsgId        = ID_LMM_MMC_RESUME_IND;

    /* 填充消息体 */
    pstMmcSuspendIndMsg->ulOpId         = NAS_EMM_OPID_MMC;

    /* 发送MMC内部消息 */
    NAS_LMM_SendLmmMmcMsg(pstMmcSuspendIndMsg);

    return;
}


/*****************************************************************************
 Function Name   : NAS_EMM_SendEsmResumeInd
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SendEsmResumeInd( VOS_VOID )
{
    EMM_ESM_RESUME_IND_STRU           *pstEsmResumeIndMsg;

    /* 申请DOPRA消息 */
    pstEsmResumeIndMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_RESUME_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstEsmResumeIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendEsmResumeInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstEsmResumeIndMsg, 0, sizeof(EMM_ESM_RESUME_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_ESM_MSG_HEADER((pstEsmResumeIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(EMM_ESM_RESUME_IND_STRU));

    /* 填充消息ID */
    pstEsmResumeIndMsg->ulMsgId        = ID_EMM_ESM_RESUME_IND;


    /* 发送DOPRA消息 */
    NAS_LMM_SEND_MSG(pstEsmResumeIndMsg);

    return;
}



/*****************************************************************************
 Function Name   : NAS_EMM_SendErabmResumeInd
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SendErabmResumeInd( VOS_VOID )
{
    EMM_ERABM_RESUME_IND_STRU           *pstLrabmResumeIndMsg;

    /* 申请DOPRA消息 */
    pstLrabmResumeIndMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(EMM_ERABM_RESUME_IND_STRU));
    if (NAS_LMM_NULL_PTR == pstLrabmResumeIndMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendErabmResumeInd: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstLrabmResumeIndMsg, 0, sizeof(EMM_ERABM_RESUME_IND_STRU));

    /* 打包VOS消息头 */
    EMM_PUBU_COMP_ERABM_MSG_HEADER((pstLrabmResumeIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(EMM_ERABM_RESUME_IND_STRU));

    /* 填充消息ID */
    pstLrabmResumeIndMsg->ulMsgId        = ID_EMM_ERABM_RESUME_IND;
    pstLrabmResumeIndMsg->enSysChngType  = NAS_EMM_GetResumeType();


    /* 发送DOPRA消息 */
    NAS_LMM_SEND_MSG(pstLrabmResumeIndMsg);

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SendLrcResumeRsp
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-4-28  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_SendLrcResumeRsp( VOS_VOID)
{
    LRRC_LMM_RESUME_RSP_STRU           *pstLrrcResumeRspMsg;

    /* 申请DOPRA消息 */
    pstLrrcResumeRspMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_RESUME_RSP_STRU));
    if (NAS_LMM_NULL_PTR == pstLrrcResumeRspMsg)
    {
        /* 打印异常，ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_SendLrcResumeRsp: MSG ALLOC ERROR!!!");
        return ;
    }

    /* 清空 */
    NAS_LMM_MEM_SET(pstLrrcResumeRspMsg, 0, sizeof(LRRC_LMM_RESUME_RSP_STRU));

    /* 打包VOS消息头 */
    NAS_EMM_SET_RRC_MSG_HEADER((pstLrrcResumeRspMsg),
            NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LRRC_LMM_RESUME_RSP_STRU));

    /* 填充消息ID */
    pstLrrcResumeRspMsg->enMsgId       = ID_LRRC_LMM_RESUME_RSP;

    /* 发送DOPRA消息 */
    NAS_LMM_SEND_MSG(pstLrrcResumeRspMsg);

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_NoCellSuspendMsgMmcPlmnReq
 Description     : 若当前的 LTE为挂起态，则给ESM和ERABM通知激活，并改为LTE激活态
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Hanlufeng 41410      2011-5-5  Draft Enact

*****************************************************************************/
VOS_UINT32    NAS_EMM_NoCellSuspendMsgMmcPlmnReq( VOS_VOID )
{
    MMC_LMM_REGISTER_STATUS_ENUM_UINT32 ulNasMmcRegState = MMC_LMM_REGISTER_STATUS_BUTT;

    /* 若当前的 LTE为挂起态，则给ESM和ERABM通知激活，并改为LTE激活态 */
    if( (   (NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_DEREG,EMM_SS_DEREG_NO_CELL_AVAILABLE))
           ||(NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_REG,EMM_SS_REG_NO_CELL_AVAILABLE))
         )
         && (NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
       )
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_NoCellSuspendMsgMmcPlmnReq: LTE SUSPEND. ");

        /* 当前的 LTE为激活态 */
        NAS_EMM_GetCurLteState() = NAS_LMM_CUR_LTE_ACTIVE;

        /* 记录RESUME参数 */
        NAS_EMM_SaveMmcResumePara();

        /*leili modify for isr begin*/
        /*去激活ISR必须放在获取安全参数之前，否则，获取不到正确的安全参数*/
        NAS_EMM_ProcInterSystemTransferBearerInfo();
        /*leili modify for isr end*/

        /* 连接状态仍为空闲态 */

        /* 若TIN ==  PTMSI，则获取GU的安全参数 */
        NAS_EMM_MmcRsmGetSecuPara();

        /* 激活ESM, ERABM */
        NAS_EMM_SendEsmResumeInd();
        NAS_EMM_SendErabmResumeInd();

        /* 丢网后，上层会清除PLMN ID等消息，因此重新驻留后，需要上报location info */
        /* 删除当前小区 */
        NAS_EMMC_DeleteCurrentAreaCellId();
        /* 清空服务状态 */
        NAS_LMM_SetNasMmcCSServiceState(MMC_LMM_SERVICE_STATUS_TYPE_BUTT);
        NAS_LMM_SetNasMmcServiceState(MMC_LMM_SERVICE_STATUS_TYPE_BUTT);

        NAS_LMM_CovertMmlRegStatusToLmmRegStatus(NAS_MML_GetPsRegStatus(),&ulNasMmcRegState);

        NAS_LMM_SetNasMmcRegisterState(ulNasMmcRegState);

        /*NAS_EMM_SendMmcRegisterStatInd(NAS_EMM_REG_CHANGE_TYPE_REG_STATE);*/

        if(EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
        {
             /* 如果当前为注册态，且当前只有L接入技术，当TIN为PTMSI时，先LOCAL DETACH，
                这样等收到系统信息时一定发起ATTACH */
            if ((MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
                &&(NAS_LMM_SUCC ==  NAS_EMM_lteRatIsOnlyExist()))
            {

                NAS_EMM_LocalDetachProc();

                /* 将状态转移至MS_DEREG + SS_DEREG_PLMN_SEARCH状态 */
                NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                            EMM_SS_DEREG_PLMN_SEARCH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

                /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
                NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

                return NAS_LMM_MSG_HANDLED;
            }

        }

        /* 将状态转移至MS_RESUME + SS_RESUME_MMCORI_WAIT_SYS_INFO状态 */
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_RESUME,
                                        EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        return NAS_LMM_MSG_HANDLED;
    }

    return NAS_LMM_MSG_DISCARD;
}


/*****************************************************************************
 Function Name   : NAS_EMM_NoImsiSuspendMsgMmcPlmnReq
 Description     : 若当前的 LTE为挂起态，则给ESM和ERABM通知激活，并改为LTE激活态
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.Hanlufeng 41410      2011-5-5  Draft Enact

*****************************************************************************/
VOS_VOID    NAS_EMM_NoImsiSuspendMsgMmcPlmnReq( VOS_VOID )
{
    /* 若当前的 LTE为挂起态，则给ESM和ERABM通知激活，并改为LTE激活态 */
    if(     (NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_DEREG,EMM_SS_DEREG_NO_IMSI))
         && (NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
       )
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_NoImsiSuspendMsgMmcPlmnReq: LTE SUSPEND. ");

        /* 当前的 LTE为激活态 */
        NAS_EMM_GetCurLteState() = NAS_LMM_CUR_LTE_ACTIVE;

        /* 记录RESUME参数 */
        NAS_EMM_SaveMmcResumePara();

        /* 连接状态仍为空闲态 */


        /* 激活ESM, ERABM */
        NAS_EMM_SendEsmResumeInd();
        NAS_EMM_SendErabmResumeInd();
        /* 转移到EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND状态 */
        NAS_EMM_PUBU_FSMTranState(EMM_MS_RESUME,
                                  EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND,
                                  TI_NAS_EMM_STATE_NO_TIMER);
    }

    return;
}


/*****************************************************************************
 Function Name   : NAS_EMM_MmcRsmGetSecuPara
 Description     : MMC恢复LTE时，获取GU的安全参数
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-5-11  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_MmcRsmGetSecuPara( VOS_VOID )
{
    MMC_LMM_PUB_INFO_STRU           stTin;
    MMC_LMM_RESULT_ID_ENUM_UINT32   ulRst;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    ulRst = NAS_EMM_GetGulPubInfo(MMC_LMM_TIN_TYPE, &stTin);
    if(MMC_LMM_FAIL == ulRst)
    {
        NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_MmcRsmGetSecuPara: NAS_EMM_GetGulPubInfo rst = ",
                                ulRst);
        return;
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_MmcRsmGetSecuPara:  ulTinType = ",
                            stTin.u.ulTinType);

    if(MMC_LMM_TIN_P_TMSI == stTin.u.ulTinType)
    {
        NAS_EMM_SecuResumeIdle();
    }

    return;
}


VOS_UINT32  NAS_EMM_MsSuspendSsWaitEndMsgLrrcResumeInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    VOS_UINT32                          ulRst;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsSuspendSsWaitEndMsgLrrcResumeInd: enter. ");

    /* 停止定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_SUSPEND_END_TIMER);

    /* 编码时发现回退的处理和从GU过来的RESUME完全相同 */
    ulRst = NAS_EMM_MsAnySsNoCellAvailMsgRrcRsumeInd(ulMsgId, pMsgStru);

    return ulRst;

}


VOS_UINT32  NAS_EMM_MsRegSsNoCellAvailMsgRrcSecuParaInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru )
{
    LRRC_LMM_SECU_PARA_IND_STRU        *pstLrrcSecuParaInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;
    /* 获取 LRRC_LMM_SECU_PARA_IND消息*/
    pstLrrcSecuParaInd = (LRRC_LMM_SECU_PARA_IND_STRU *)pMsgStru;

    /* 如果LMM不处在挂起态，则报错*/
    if(NAS_LMM_CUR_LTE_SUSPEND != NAS_EMM_GetCurLteState())
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_MsRegSsNoCellAvailMsgRrcSecuParaInd:sta err.");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 调用 secu 模块的函数，产生 current mapped secu context并激活*/
    NAS_EMM_SecuRrcSecuParamIndHo(pstLrrcSecuParaInd->aucSecuPara);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsNoCellAvailMsgRrcRelInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru )
{
    LRRC_LMM_REL_IND_STRU              *pLrrcRelInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsRegSsNoCellAvailMsgRrcRelInd enter.");

    pLrrcRelInd = (LRRC_LMM_REL_IND_STRU *)pMsgStru;

    /* 检查释放原因值,如果不是切换失败，则应该是 L->GU的 HO或CCO 成功 */
    if(LRRC_LNAS_REL_CAUSE_GU2L_HO_ERR != pLrrcRelInd->enRelCause)
    {
        NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_MsRegSsNoCellAvailMsgRrcRelInd: enRelCause = , .",
                                pLrrcRelInd->enRelCause );

        NAS_EMM_PUBU_LOG2_INFO("NAS_EMM_MsRegSsNoCellAvailMsgRrcRelInd: SuspendType = , ResumeType = .",
                                NAS_EMM_GetSuspendType(),
                                NAS_EMM_GetResumeType() );

        return NAS_LMM_MSG_HANDLED;
    }

    /* 释放原因值如果是切换失败，恢复安全上下文 */
    /* 调用 secu 模块函数，恢复安全上下文状态*/
    NAS_EMM_SecuRrcRelIndHoFail();

    return NAS_LMM_MSG_HANDLED;

}
VOS_UINT32  NAS_EMM_MsResumeSsRrcRsmWaitOtherMsgRsmRsp(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsResumeSsRrcRsmWaitOtherMsgRsmRsp: enter. ");

    NAS_EMM_GetUplayerCount()++;

    if(NAS_EMM_RESUME_UPLAYER_NUM_CM == NAS_EMM_GetUplayerCount())
    {

        /* 停定时器 */
        NAS_LMM_StopStateTimer(TI_NAS_EMM_RRCRSM_WAIT_OTHER_RESUME_RSP_TIMER);

        /* 清除此次挂起的上层响应记录，为下一次挂起过程做准备 */
        NAS_EMM_GetUplayerCount() = NAS_EMM_RESUME_UPLAYER_NUM_INITVALUE;

        /* 给MMC发送恢复 */
        NAS_EMM_SendMmcResumeInd();

        /* RESUME，不改变连接状态:
        REVERSE类型的，继续REVERSE前的连接状态；
        从GU过来的，重选、重定向: 持续IDLE，
                     切换: 等待LRRC的自动建链接消息，直接进入数传状态，或
                            收到REL_IND消息；
                     CCO: LNAS将发起建链，建链时改连接状态 */
        /*NAS_EMM_ResumeConnStateChng(); */

        /* 如果是HO_REVERSE/CCO_REVERSE，需要将安全状态恢复为激活态*/
        if((NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO == NAS_EMM_GetResumeType())
         ||(NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO == NAS_EMM_GetResumeType()))
        {
            NAS_EMM_SecuHoOrCcoReverse();
        }

        /* 给LRRC响应 */
        NAS_EMM_SendLrcResumeRsp();

        /* 改状态: 无论回退，还是GU->L，都先进入等系统消息的状态，
        在等系统消息的状态下，在处理系统消息、REL消息、丢网消息时，
        再根据RESUME的类型、消息类型，决定回退后的状态。*/

        NAS_EMM_PUBU_FSMTranState(  EMM_MS_RESUME,
                                    EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND,
                                    TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER);

    }


    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsResumeSsRrcRsmWaitOtherMsgWtOtherTimerExp(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /* 清除此次挂起的上层响应记录，为下一次挂起过程做准备 */
    NAS_EMM_GetUplayerCount() = NAS_EMM_RESUME_UPLAYER_NUM_INITVALUE;

    /* 复位处理*/
    NAS_EMM_SendMmcErrInd(NAS_EMM_REBOOT_TYPE_RRC_RESUME_ECM_FAIL);

    return NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_PreProcSysWhenResumingReverse(
                            const EMMC_EMM_SYS_INFO_IND_STRU *pstsysinfo )
{
    VOS_UINT32                          ulRslt = NAS_LMM_MSG_DISCARD;

    /* 停定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER);

    /* 判断RESUME类型如果不是REVERSE类型，则直接返回，继续进入状态机处理*/
    if((NAS_LMM_SYS_CHNG_TYPE_RSL   == NAS_EMM_GetResumeType())
     ||(NAS_LMM_SYS_CHNG_TYPE_HO    == NAS_EMM_GetResumeType())
     ||(NAS_LMM_SYS_CHNG_TYPE_CCO   == NAS_EMM_GetResumeType())
     ||(NAS_LMM_SYS_CHNG_TYPE_REDIR == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysWhenResumingReverse:NOT REVERSE. ");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 没有执行上面分支，则RESUME类型为REVERSE，满足以下条件需要释放连接:
       1)小区被禁
       2)不在IDLE态，
       3)不存在Emergency Bearer*/

    /* 然后区分不同的回退类型，进行不同处理*/
    switch(NAS_EMM_GetResumeType())
    {
        /* 检查ResumeType，如果是重选回退，处理如下:
           1)挂起前的状态为Dereg.Attach_Needed，状态恢复为Dereg.Attach_Needed，
             进入状态机继续处理；
           2)挂起前的状态为Reg.WtAccessGrantInd，根据BarProcedure做不同处理，
             不再进入状态；
           3)其他状态迁移到Dereg.No_Cell_Available;Reg.No_Cell_Available或者Dereg.No_Imsi,
             进入状态机继续处理
        */
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL:
                ulRslt = NAS_EMM_PreProcSysInfoReselReverse(pstsysinfo);
                break;

        /*对于挂起过程中非重选回退的场景，收到RESUME后，继续收到SYS后，要根据挂起前
        的状态，转换到回退到的状态，然后再进入状态机处理，则开始标准的LTE处理 */
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO:
                ulRslt = NAS_EMM_PreProcSysInfoHoAndCcoReverse(pstsysinfo);
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR:
                NAS_EMM_ReverseStateChng();
                ulRslt = NAS_LMM_MSG_DISCARD;
                break;

        default :
                NAS_EMM_PUBU_LOG_ERR("NAS_EMM_PreProcSysWhenResumingReverse:ResumeType err. ");
                break;
    }

    return  ulRslt;
}



VOS_VOID  NAS_EMM_ReverseStateChng( VOS_VOID )
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_ReverseStateChng: CurState = ",
                            NAS_EMM_GetMsBefSuspend());

    /* 根据记录的 staBeforeSuspend 迁移状态*/
    if(( EMM_MS_REG == NAS_EMM_GetMsBefSuspend())
        ||( EMM_MS_TAU_INIT == NAS_EMM_GetMsBefSuspend())
        ||( EMM_MS_SER_INIT == NAS_EMM_GetMsBefSuspend()))
    {
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                        EMM_SS_REG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        if( EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefSuspend())
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    return;
}
VOS_VOID  NAS_EMM_ReverseStateChngAsPlmnInd( VOS_VOID )
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_ReverseStateChng: CurState = ",
                            NAS_EMM_GetMsBefSuspend());

    /* 根据记录的 staBeforeSuspend 迁移状态*/
    if(( EMM_MS_REG == NAS_EMM_GetMsBefSuspend())
        ||( EMM_MS_TAU_INIT == NAS_EMM_GetMsBefSuspend())
        ||( EMM_MS_SER_INIT == NAS_EMM_GetMsBefSuspend()))
    {
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                        EMM_SS_REG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        if( EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefSuspend())
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    return;
}
VOS_UINT32  NAS_EMM_PreProcSysInfoReselReverse(
                                   const EMMC_EMM_SYS_INFO_IND_STRU *pstsysinfo )
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    VOS_UINT32                          ulEmmStaBeforeSuspend = 0;
    VOS_UINT32                          ulRslt = NAS_LMM_MSG_HANDLED;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysInfoReselReverse entered.");

    /* 获取挂起前的状态*/
    ulEmmStaBeforeSuspend = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_GetMsBefSuspend(),
                                                      NAS_EMM_GetSsBefSuspend());

    /* 根据挂起前的状态做不同处理*/
    switch(ulEmmStaBeforeSuspend)
    {
        /* 挂起前的状态为Dereg.Attach_Needed，根据小区是否被禁，给MMC上报ATTACH
           结果，不再进入状态机处理
         */
        case NAS_LMM_PUB_COMP_EMMSTATE( EMM_MS_DEREG,
                                        EMM_SS_DEREG_ATTACH_NEEDED):
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysInfoReselReverse:Dereg.Attach_Needed");
             /* 判断系统消息如果被禁，则迁移到限制服务*/

            if((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
                || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
            {
                /* 状态迁移: Reg.Limite_Service*/
                NAS_EMM_PUBU_FSMTranState(      EMM_MS_DEREG,
                                                EMM_SS_DEREG_LIMITED_SERVICE,
                                                TI_NAS_EMM_STATE_NO_TIMER);

                NAS_EMM_SetAttType();

                /*向MMC发送ATTACH结果*/
                NAS_EMM_AppSendAttRsltForb(pstsysinfo->ulForbiddenInfo);
            }
            else
            {
                /*EMM当前状态迁移到DEREG.ATTACH_NEEDED*/
                NAS_EMM_PUBU_FSMTranState(      EMM_MS_DEREG,
                                                EMM_SS_DEREG_ATTACH_NEEDED,
                                                TI_NAS_EMM_STATE_NO_TIMER);
                /*向MMC发送ATTACH结果*/
                NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_ACCESS_BAR);

            }
            ulRslt = NAS_LMM_MSG_HANDLED;
            break;

        /* 挂起前的状态为Reg.WtAccessGrantInd,处理完不再进入状态机  */
        case NAS_LMM_PUB_COMP_EMMSTATE( EMM_MS_REG,
                                        EMM_SS_REG_WAIT_ACCESS_GRANT_IND):
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysInfoReselReverse:Reg.WtAccessGrantInd");

            /* 判断系统消息如果被禁，则迁移到限制服务*/
            if((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
                 || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
            {
                NAS_EMM_SetTauTypeNoProcedure();

                /* 清空Bar信息*/
                NAS_EMM_ClearBarResouce();

                /* 状态迁移: Reg.Limite_Service*/
                NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                                EMM_SS_REG_LIMITED_SERVICE,
                                                TI_NAS_EMM_STATE_NO_TIMER);

                /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
                NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);

            }
            else
            {
                /* 系统消息不被禁，调用REG态下解Bar的公共处理*/
                NAS_EMM_RegBarCommProc();

                /* 如果经过公共处理，EMM当前的状态仍然处于Resume.RrcOriWtSysInfo,
                   则说明没有触发任何流程，需要给MMC上报结果，并将状态迁移到
                   Reg.WtAccessGrantInd状态；反之，清空BarProcedure
                */
                if((EMM_MS_RESUME                           == NAS_LMM_GetEmmCurFsmMS())
                 &&(EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND  == NAS_LMM_GetEmmCurFsmSS()))
                {
                    NAS_EMM_SetTauTypeNoProcedure();

                    NAS_EMM_PUBU_FSMTranState(  EMM_MS_REG,
                                                EMM_SS_REG_WAIT_ACCESS_GRANT_IND,
                                                TI_NAS_EMM_STATE_NO_TIMER);

                    ulTauRslt = MMC_LMM_TAU_RSLT_ACCESS_BARED;
                    NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);
 

                }
            }

            ulRslt = NAS_LMM_MSG_HANDLED;
            break;

        /* 其他状态,迁移状态后，继续进入状态机处理*/
        default:
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysInfoReselReverse:Others.");
            NAS_EMM_ReverseStateChng();
            ulRslt = NAS_LMM_MSG_DISCARD;
            break;
    }

    return ulRslt;
}
VOS_UINT32  NAS_EMM_PreProcSysInfoHoAndCcoReverse(
                                   const EMMC_EMM_SYS_INFO_IND_STRU *pstsysinfo )
{/* lihong00150010 emergency tau&service begin */
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_PreProcSysInfoHoAndCcoReverse is entered");
    if(((EMMC_EMM_NO_FORBIDDEN  != pstsysinfo->ulForbiddenInfo)
        || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
        &&(NAS_EMM_CONN_IDLE   != NAS_EMM_GetConnState())
        &&(NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus()))
    {/* lihong00150010 emergency tau&service end */
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                            EMM_SS_REG_LIMITED_SERVICE,
                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_SetTauTypeNoProcedure();

        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&pstsysinfo->ulForbiddenInfo);


        /*发送RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(      NAS_LMM_NOT_BARRED);

        return NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                        EMM_SS_REG_NO_CELL_AVAILABLE,
                        TI_NAS_EMM_STATE_NO_TIMER);
    return NAS_LMM_MSG_DISCARD;

}


VOS_VOID  NAS_EMM_GU2LResumeStateChng(VOS_VOID)
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_GU2LResumeStateChng: CurState = ",
                            NAS_EMM_GetMsBefResume());

    /* 根据记录的 staBeforeSuspend 迁移状态*/
    if( EMM_MS_REG == NAS_EMM_GetMsBefResume())
    {
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                        EMM_SS_REG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        if( EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefResume())
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    return;
}
VOS_VOID  NAS_EMM_GU2LResumeStateChngAsPlmnInd(VOS_VOID)
{
    NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_GU2LResumeStateChngAsPlmnInd: CurState = ",
                            NAS_EMM_GetMsBefResume());

    /* 根据记录的 staBeforeSuspend 迁移状态*/
    if( EMM_MS_REG == NAS_EMM_GetMsBefResume())
    {
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                        EMM_SS_REG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        if( EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefResume())
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    return;
}
VOS_VOID  NAS_EMM_RegForbidSysInfoProc( EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  ulForbiddenInfo)
{
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_RegForbidSysInfoProc is entered");

    if (NAS_RELEASE_CTRL)
    {
        /* 判断承载上下文是否激活，如果未激活，则本地DETACH，转入DEREG+LIMITED_SERVICE状态；
        如果激活，则保持原有处理 */
        if(NAS_MML_PS_BEARER_STATE_INACTIVE == NAS_EMM_IsEpsBearStatusAct())
        {
            NAS_EMM_LocalDetachProc();

            NAS_LMM_DeregReleaseResource();

            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

            /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
            NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

            NAS_EMM_AppSendAttRsltForb(ulForbiddenInfo);

        }
        else
        {
            /* 注册态，进入REG.LIMITED； */
            NAS_EMM_SetTauTypeNoProcedure();

            NAS_EMM_PUBU_FSMTranState(  EMM_MS_REG,
                                        EMM_SS_REG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
            /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
            NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&ulForbiddenInfo);
        }
    }
    else
    {
        /* 注册态，进入REG.LIMITED； */
        NAS_EMM_SetTauTypeNoProcedure();

        NAS_EMM_PUBU_FSMTranState(  EMM_MS_REG,
                                    EMM_SS_REG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);
        /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
        NAS_EMM_MmcSendTauActionResultIndForbType((VOS_VOID*)&ulForbiddenInfo);
    }


    return;
}
VOS_VOID  NAS_EMM_GU2LNoEpsBearProc(VOS_VOID)
{
    VOS_UINT32                          ulRst;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_GU2LNoEpsBearProc is entered");

    NAS_EMM_LocalDetachProc();

    NAS_LMM_DeregReleaseResource();

    NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                EMM_SS_DEREG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*向MMC发送本地LMM_MMC_DETACH_IND消息*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    /* 重新发起ATTACH */
    ulRst = NAS_EMM_SendIntraAttachReq();
    if (NAS_EMM_SUCC != ulRst)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_GU2LNoEpsBearProc:  send INTRA ATTACH REQ ERR !");
    }

    return;
}


VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcPlmnInd
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcPlmnInd: \
                            Input para invalid");
        return NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER);

    /*清除BAR标识*/
    NAS_EMM_ClearBarResouce();


    NAS_EMM_PUBU_LOG2_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcPlmnInd:\
                            NAS_EMM_GetResumeType()= ,NAS_EMM_GetMsBefSuspend()",
                            NAS_EMM_GetResumeType(),NAS_EMM_GetMsBefSuspend());

    /* GU到L的重选、重定向时，MMC收到LMM的RESUME IND就会下发搜网请求 */
    if (NAS_LMM_SYS_CHNG_TYPE_RSL == NAS_EMM_GetResumeType())
    {
        if (EMM_MS_REG == NAS_EMM_GetMsBefResume())
        {
            NAS_EMM_RrcResumeReselTypeSetTauStartCause();
        }
        /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
        NAS_EMM_SecuResumeIdle();
        NAS_EMM_GU2LResumeStateChngAsPlmnInd();

        return NAS_LMM_MSG_HANDLED;
    }

    if (NAS_LMM_SYS_CHNG_TYPE_REDIR == NAS_EMM_GetResumeType())
    {
        if (EMM_MS_REG == NAS_EMM_GetMsBefResume())
        {
            NAS_EMM_RrcResumeRedirTypeSetTauStartCause();
        }
        /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
        NAS_EMM_SecuResumeIdle();
        NAS_EMM_GU2LResumeStateChngAsPlmnInd();

        return NAS_LMM_MSG_HANDLED;
    }

    /* L2GU回退过程中可以收到MMC的搜网请求消息，CCO/HO的回退过程中为连接态，
       也不会收到MMC的搜网请求消息 */
    if((NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_ReverseStateChngAsPlmnInd();
    }

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /* 通知ESM执行了本地DETACH */
    NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 给MMC回DETACH成功 */
    NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* 无卡状态下如果是紧急注册中有可能要到L */
    if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcDetachReq: NO_IMSI ");
        NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                               EMM_SS_DEREG_NO_IMSI,
                               TI_NAS_EMM_STATE_NO_TIMER);

        NAS_LMM_DeregReleaseResource();

        /* 除了CCO的场景，全部都给RRC发释放，CCO只能靠后续不发起建链让GAS等CNF超时回退 */
        if(LRRC_LMM_SYS_CHNG_TYPE_CCO != NAS_EMM_GetResumeType())
        {
            NAS_EMM_SndRrcRelReq(NAS_LMM_NOT_BARRED);
            NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);
        }

        return  NAS_LMM_MSG_HANDLED;

    }

    /* 有卡 */
    switch(NAS_EMM_GetResumeType())
    {
                /* 重选、重定向、重选回退、重定向回退和CCO的场景，本地DETACH等系统消息 */
        case    LRRC_LMM_SYS_CHNG_TYPE_RESEL:
        case    LRRC_LMM_SYS_CHNG_TYPE_REDIR:
        case    LRRC_LMM_SYS_CHNG_TYPE_CCO:
        case    LRRC_LMM_SYS_CHNG_TYPE_RESEL_REVERSE:
        case    LRRC_LMM_SYS_CHNG_TYPE_REDIR_REVERSE:

                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                       TI_NAS_EMM_STATE_NO_TIMER);

                /* 本地DETACH释放资源:动态内存、赋初值 */
                NAS_LMM_DeregReleaseResource();
                break;

                /* 切换、切换回退和CCO回退的场景，均作本地DETACH后给RRC发释放 */
        case    LRRC_LMM_SYS_CHNG_TYPE_HO:
        case    LRRC_LMM_SYS_CHNG_TYPE_HO_REVERSE:
        case    LRRC_LMM_SYS_CHNG_TYPE_CCO_REVERSE:

                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_PLMN_SEARCH,
                                       TI_NAS_EMM_STATE_NO_TIMER);

                /* 本地DETACH释放资源:动态内存、赋初值 */
                NAS_LMM_DeregReleaseResource();

                /* 释放RRC，修改连接状态为释放过程中 */
                NAS_EMM_SndRrcRelReq(NAS_LMM_NOT_BARRED);
                NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);
                break;

        default:
                NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgMmcDetachReq: Invalid resume type = ",
                                        NAS_EMM_GetResumeType());
                break;
    }

    return  NAS_LMM_MSG_HANDLED;

}


VOS_VOID  NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LReg(VOS_VOID)
{

    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LReg is entered");
    if (NAS_RELEASE_CTRL)
    {
        if(NAS_MML_PS_BEARER_STATE_INACTIVE == NAS_EMM_IsEpsBearStatusAct())
        {
        	NAS_EMM_GU2LNoEpsBearProc();
        }
        else
        {
        	NAS_EMM_ProcSuitSysWhenRsmGu2LReg();
        }
    }
    else
    {
        NAS_EMM_ProcSuitSysWhenRsmGu2LReg();
    }
    return;
}

/* lihong00150010 emergency tau&service begin */

VOS_VOID  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAnyCellOrForbidden
(
    const EMMC_EMM_SYS_INFO_IND_STRU   *pstsysinfo
)
{
    /* 从异系统变换过来时，可能是ANY CELL驻留时向LTE的重选 */
    NAS_EMM_PUBU_LOG_WARN("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAnyCellOrForbidden: Forbidden ");

    /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
    NAS_EMM_SecuResumeIdle();

    if(EMM_MS_REG == NAS_EMM_GetMsBefResume())
    {   /* 注册态，进入REG.LIMITED； */
        /* lihong00150010 emergency tau&service begin */
        if (NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_RegForbidSysInfoProc(pstsysinfo->ulForbiddenInfo);
            if (MMC_LMM_CONN_IDLE != NAS_EMM_GetConnState())
            {
                NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd:Limited Srv Connected");

                /*发送NAS_EMM_REL_REQ*/
                NAS_EMM_RelReq(         NAS_LMM_NOT_BARRED);
            }
        }
        else
        {
            NAS_EMM_ProcSuitSysWhenRsmGu2LReg();
        }
    }/* lihong00150010 emergency tau&service end */
    else
    {   /* 未注册: 包括两种状态: DEREG. NO_CELL和NO IMSI；可能进入DEREG.LIMITED */
        NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                    EMM_SS_DEREG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_SetAttType();

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRsltForb(pstsysinfo->ulForbiddenInfo);
    }
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo;
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  ulForbiddenInfo = EMMC_EMM_NO_FORBIDDEN;

    (VOS_VOID)ulMsgId;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd: ResumeType = ",
                        NAS_EMM_GetResumeType());

    pstsysinfo = (EMMC_EMM_SYS_INFO_IND_STRU *)pMsgStru;

    /* 停定时器 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER);

    if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        ||(NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd:LAU OR COMBINED RAU");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /* 若RESUME之前是 NO IMSI状态，过来后还要继续NO IMSI状态 */
    if(EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefResume())
    {
        /*向MMC发送注册结果 消息*/

        NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                    EMM_SS_DEREG_NO_IMSI,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送ATTACH失败*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_FAILURE);

        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd: NO_IMSI ");
        return  NAS_LMM_MSG_HANDLED;
    }

    ulForbiddenInfo = pstsysinfo->ulForbiddenInfo;

    /* 如果DCM定制需求关于切换被禁TA的标识有效，则GU到L的切换不考虑被禁TA FOR Roaming */
    if((NAS_EMM_HO_IGNORE_FORBID_TA_FLAG_VALID == g_ulHoIgnoreForbidTaFlag)
        && (NAS_LMM_SYS_CHNG_TYPE_HO == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_PUBU_LOG1_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgSysInfoInd:DCM handover,forbiddenInfo = ",
                               pstsysinfo->ulForbiddenInfo);
        if(EMMC_EMM_FORBIDDEN_TA_FOR_ROAMING == pstsysinfo->ulForbiddenInfo)
        {
            ulForbiddenInfo = EMMC_EMM_NO_FORBIDDEN;
        }
    }

    /*GU2L后LRRC能否驻留属于"forbidden tracking areas for regional provision of service"列表
    中的TA小区需讨论确定，暂时按照会驻留实现，SYS判断禁止注册 */
    if((EMMC_EMM_NO_FORBIDDEN != ulForbiddenInfo)
        || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
    {
/* lihong00150010 emergency tau&service begin */
        NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAnyCellOrForbidden(pstsysinfo);
/* lihong00150010 emergency tau&service end */
    }
    else
    {   /* 如果SYS判断能够注册, 根据RESUME前的状态, 判断是发起ATTACH，还是TAU */

        if(EMM_MS_REG == NAS_EMM_GetMsBefResume())
        {
            /* 判断承载上下文是否激活，如果未激活，则本地DETACH，转入DEREG+NORMAL_SERVICE状态；
            发起ATTACH流程；
            如果激活，则保持原有处理 */
            NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LReg();
        }
        else
        {
            /*未注册态: 改状态为DEREG.NORMAL_SERVICE*/
            NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LDeReg();
        }
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAreaLostInd
(
    VOS_UINT32      ulMsgId,
    const VOS_VOID  *pMsgStru
)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;


    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAreaLostInd:NAS_EMM_GetResumeType() = ",
                            NAS_EMM_GetResumeType());

    /*清除BAR标识*/
    NAS_EMM_ClearBarResouce();

    /* 关闭当前EMM的状态定时器*/
    NAS_LMM_StopAllEmmStateTimer();

    switch(NAS_EMM_GetResumeType())
    {
        case    NAS_LMM_SYS_CHNG_TYPE_HO:
                if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
                    || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
                    && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
                {
                    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAreaLostInd:LAU OR COMBINED RAU");
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
                }
                NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
                NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
                NAS_EMM_PUBU_FSMTranState(  EMM_MS_REG,
                                            EMM_SS_REG_NO_CELL_AVAILABLE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

                break;
        case    NAS_LMM_SYS_CHNG_TYPE_RSL:
        case    NAS_LMM_SYS_CHNG_TYPE_CCO:
        case    NAS_LMM_SYS_CHNG_TYPE_REDIR:
                /*CCO/REDIR/RESEL流程在此状态不会收到丢网消息*/
                break;

        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR:
                NAS_EMM_ReverseStateChng();
                break;
        default:
                NAS_EMM_PUBU_LOG_WARN("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgAreaLostInd:Resume type is err!");
                break;

    }
    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_RrcResumeReselTypeSetTauStartCause(VOS_VOID  )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32        enTinType       = MMC_LMM_TIN_INVALID;
    GMM_LMM_PACKET_MM_STATE_ENUM_UINT32 enPacketMmState = GMM_LMM_PACKET_MM_STATE_BUTT;
    GMM_LMM_GPRS_MM_STATE_ENUM_UINT32   enGprsMmState   = GMM_LMM_GPRS_MM_STATE_BUTT;
    NAS_LMM_RSM_SYS_CHNG_DIR_ENUM_UINT32    enRsmDir    = NAS_LMM_RSM_SYS_CHNG_DIR_BUTT;

    /* 获取TIN值 */
    enTinType = NAS_EMM_GetTinType();

    /* 获取U模连接状态 */
    enPacketMmState = NAS_LMM_GetEmmInfoUConnState();

    if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:LAU OR COMBINED RAU");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /* For case l, if the UE was in PMM-CONNECTED mode and the TIN indicates
       "RAT-related TMSI", the UE shall set the TIN to "P-TMSI" before initiating
       the combined tracking area updating procedure. */
    if ((GMM_LMM_PACKET_MM_STATE_PMM_CONNECTED == enPacketMmState)
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == enTinType))
    {
        NAS_EMM_SetTinType(MMC_LMM_TIN_P_TMSI);
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        /*更新所有承载的ISR标识为ISR激活前*/
        NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
        /*PC REPLAY MODIFY BY LEILI END*/
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause: UPDATE MML PS BEARER INFO:");
        NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);
    }

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI" */
    if (MMC_LMM_TIN_P_TMSI == enTinType)
    {
        NAS_EMM_ResumeProcPTMSITau();
        return;
    }

    /* 获取G模连接状态 */
    enGprsMmState = NAS_LMM_GetEmmInfoGConnState();

    /* 获取恢复方向 */
    enRsmDir = NAS_EMM_GetResumeDir();

    /* l)when the UE reselects an E-UTRAN cell while it was in GPRS READY state or PMM-CONNECTED mode; */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_MM_STATE_READY == enGprsMmState))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:GPRS READY init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    if ((NAS_LMM_RSM_SYS_CHNG_DIR_W2L == enRsmDir)
        && (GMM_LMM_PACKET_MM_STATE_PMM_CONNECTED == enPacketMmState))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:PMM-CONNECTED init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    /* c)when the UE performs an intersystem change from A/Gb mode to S1 mode and
         the EPS services were previously suspended in A/Gb mode;
         这个条件时联合TAU所独有的 */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:EPS service suspended init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
         and the UE previously either performed a location area update procedure
         or a combined routing area update procedure in A/Gb or Iu mode,
         in order to re-establish the SGs association. In this case the EPS update
         type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if ((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:LAU or Combined Rau init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeReselTypeSetTauStartCause:ISR ACT and P.4 init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }
    return;
}



VOS_VOID  NAS_EMM_RrcResumeRedirTypeSetTauStartCause(VOS_VOID )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32            ulTinType   = MMC_LMM_TIN_INVALID;
    NAS_LMM_RSM_SYS_CHNG_DIR_ENUM_UINT32    enRsmDir    = NAS_LMM_RSM_SYS_CHNG_DIR_BUTT;

    /* 获取TIN值 */
    ulTinType = NAS_EMM_GetTinType();

    if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_RrcResumeRedirTypeSetTauStartCause:LAU OR COMBINED RAU");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI" */
    if (MMC_LMM_TIN_P_TMSI == ulTinType)
    {
        NAS_EMM_ResumeProcPTMSITau();
        return;
    }

    /* 获取恢复方向 */
    enRsmDir = NAS_EMM_GetResumeDir();

    /* c)when the UE performs an intersystem change from A/Gb mode to S1 mode and
         the EPS services were previously suspended in A/Gb mode;
         这个条件时联合TAU所独有的 */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_RrcResumeRedirTypeSetTauStartCause:EPS service suspended init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
         and the UE previously either performed a location area update procedure
         or a combined routing area update procedure in A/Gb or Iu mode,
         in order to re-establish the SGs association. In this case the EPS update
         type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if ((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_RrcResumeRedirTypeSetTauStartCause:LAU or Combined Rau init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcResumeRedirTypeSetTauStartCause:ISR ACT and P.4 init tau");
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return;
    }
    return;
}



VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgWtSysInfoTimerExp(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgWtSysInfoTimerExp:NAS_EMM_GetResumeType()=",
                               NAS_EMM_GetResumeType());

    switch(NAS_EMM_GetResumeType())
    {
        /*对于挂起过程中回退的场景，根据挂起状态进入NO CELL状态或NO IMSI状态 */

        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO:
                NAS_EMM_ReverseStateChng();
                if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
                {
                    /*发送RRC_MM_REL_REQ*/
                    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
                }
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR:
                NAS_EMM_ReverseStateChng();
                break;
        /*对于GU2L的场景，根据激活前状态进入NO CELL状态或NO IMSI状态 */
        case    NAS_LMM_SYS_CHNG_TYPE_HO:
                if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
                    || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
                    && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
                {
                    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgWtSysInfoTimerExp:LAU OR COMBINED RAU");
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
                }
                NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
                NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
                NAS_EMM_GU2LResumeStateChng();
                if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
                {
                    /*发送RRC_MM_REL_REQ*/
                    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
                }
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_RSL:
                if (EMM_MS_REG == NAS_EMM_GetMsBefResume())
                {
                    NAS_EMM_RrcResumeReselTypeSetTauStartCause();
                }
                /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
                NAS_EMM_SecuResumeIdle();
                NAS_EMM_GU2LResumeStateChng();
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_CCO:

                if((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
                    && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
                {
                    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgWtSysInfoTimerExp:LAU OR COMBINED RAU");
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
                }

                NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
                NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);

                /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
                NAS_EMM_SecuResumeIdle();
                NAS_EMM_GU2LResumeStateChng();
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_REDIR:
                if (EMM_MS_REG == NAS_EMM_GetMsBefResume())
                {
                    NAS_EMM_RrcResumeRedirTypeSetTauStartCause();
                }
                /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
                NAS_EMM_SecuResumeIdle();
                NAS_EMM_GU2LResumeStateChng();
                break;
        default :
                NAS_EMM_PUBU_LOG_WARN("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgRrcRelInd:Resume type is err!");
                break;
    }
    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgRrcRelInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;


    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgRrcRelInd:NAS_EMM_GetResumeType()=",
                            NAS_EMM_GetResumeType());

    /* 通知ERABM释放连接,预处理中已经发了，此处不再发 */

    switch(NAS_EMM_GetResumeType())
    {
        /*对于挂起过程中回退的场景，收到释放后，进入NO CELL状态或NO IMSI状态 */
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO:
        case    NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR:
                /*停止定时器TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER*/
                NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_SYS_INFO_IND_TIMER);
                NAS_EMM_ReverseStateChng();
                break;
        case    NAS_LMM_SYS_CHNG_TYPE_HO:
        case    NAS_LMM_SYS_CHNG_TYPE_RSL:
        case    NAS_LMM_SYS_CHNG_TYPE_CCO:
        case    NAS_LMM_SYS_CHNG_TYPE_REDIR:
                /*GU->L仅在HO过程中能收到LRRC_LMM_REL_IND消息，且在预处理中连接已释放*/
                break;
        default :
                NAS_EMM_PUBU_LOG_WARN("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgRrcRelInd:Resume type is err!");
                break;
    }
    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsResumeSsRrcOriMsgT3412Exp
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriMsgT3412Exp:NAS_EMM_GetResumeType() =",
                            NAS_EMM_GetResumeType());
    if((NAS_LMM_SYS_CHNG_TYPE_RSL == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_HO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_CCO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_REDIR == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriMsgT3412Exp:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

        /*根据RESUME前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*设置周期TAU定制器超时标志*/
        NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);

    }
    else
    {
        /*回退过程的处理*/
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriMsgT3412Exp:NAS_EMM_GetMsBefResume() =",
                                    NAS_EMM_GetMsBefSuspend());

        /*根据SUSPEND前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefSuspend())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*设置周期TAU定制器超时标志*/
        NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_OTHER_STATE);
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetResumeType() =",
                            NAS_EMM_GetResumeType());
    if((NAS_LMM_SYS_CHNG_TYPE_RSL == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_HO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_CCO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_REDIR == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

        /*根据RESUME前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /* 设置承载上下文变化*/
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);
    }
    else
    {
        /*回退过程的处理*/
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetMsBefResume() =",
                                    NAS_EMM_GetMsBefSuspend());

        /*根据SUSPEND前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefSuspend())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /* 设置承载上下文变化*/
        NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);

    }
    return NAS_LMM_MSG_HANDLED;
}

#if 0
VOS_VOID  NAS_EMM_ProcSysWhenRsmGu2LIdle( VOS_VOID )
{

    /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
    NAS_EMM_SecuResumeIdle();

    /*此阶段，ISR功能暂不提供，所以，从GU重选过来，ulTinType按
    MMC_LMM_TIN_P_TMSI 处理，即发起TAU
    发起TAU，修改状态，启动定时器 */
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LIdle: Init tau.");
    NAS_EMM_TAU_StartTauForInterRat();

    return;
}
#endif


VOS_UINT32  NAS_EMM_ProcInterSysCheckRrcRelCause(VOS_VOID)
{
    if(NAS_EMM_TRIGGER_TAU_RRC_REL_LOAD_BALANCE == NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:load balance");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }
    else if(NAS_EMM_TRIGGER_TAU_RRC_REL_CONN_FAILURE == NAS_LMM_GetEmmInfoTriggerTauRrcRel())
    {
        /* 不能删除，如果有上行PENDING,则会发起SERVICE */
        NAS_EMM_TAU_IsUplinkPending();

        if(NAS_EMM_UPLINK_PENDING == NAS_EMM_TAU_GetEmmUplinkPending())
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:Init Ser.");
            NAS_EMM_SER_UplinkPending();
        }
        else
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_RrcRelCauseTriggerTau:RRC connection failure");

            NAS_EMM_TAU_StartTauForInterRat();
        }

        return NAS_EMM_YES;
    }
    else
    {

    }

    return NAS_EMM_NO;
}
VOS_UINT32 NAS_EMM_ProcSysCheck3412Exp( VOS_VOID )
{
    if (NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL ==  NAS_LMM_GetEmmInfoT3412ExpCtrl())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCheck3412Exp:REG+NO_AVAILABE_CELL T3412exp.");
        if (NAS_LMM_SYS_CHNG_TYPE_HO != NAS_EMM_GetResumeType())
        {
            /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
            NAS_EMM_SecuResumeIdle();
        }

        if (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
        {
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }
        else
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_T3412EXP);
        }

        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 协议24301 5.3.5章节描述，如果在非REG+NO_AVAILABLE_CELL状态下，T3412超时，
     则当回到REG+NORMAL_SERVICE态后，应触发 Periodic TAU*/
    if(NAS_EMM_T3412_EXP_YES_OTHER_STATE == NAS_LMM_GetEmmInfoT3412ExpCtrl())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCheck3412Exp:Other State T3412exp.");
        if (NAS_LMM_SYS_CHNG_TYPE_HO != NAS_EMM_GetResumeType())
        {
            /* 读取GU的UMTS安全上下文，生成LTE的安全参数，并设置安全状态*/
            NAS_EMM_SecuResumeIdle();
        }

        NAS_EMM_TAU_SaveEmmTAUStartCause( NAS_EMM_TAU_START_CAUSE_T3412EXP);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
VOS_UINT32 NAS_EMM_ProcSysCommonCheckTauFlag( VOS_VOID )
{
    /* 如果TA不在TA List中，需要发起TAU */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:TAI not in TAI LIST init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过ESM本地修改承载上下文*/
    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag: Bearer Status Change.");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过UE_NET_CAP or DRX改变，触发TA-Updating TAU*/
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag: DrxOrNetCap Change.");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果当前注册域为PS，且当前有能力发起CS域的注册，则发起联合TAU */
    if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:current reg domain=PS,can register in cs domain");
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /*如果当前EU值不是EU1*/
    if(EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:EU value is not EU1");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /*判断RRC Rel原因标识*/
    if(NAS_EMM_YES == NAS_EMM_ProcInterSysCheckRrcRelCause())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:rrc rel cause");
        return NAS_EMM_YES;
    }

    /* 如果UE无线能力发生变更 */
    if(NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /*如果T3412曾经超时过*/
    if(NAS_EMM_YES == NAS_EMM_ProcSysCheck3412Exp() )
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:3412 exp");

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
/*****************************************************************************
 Function Name   : NAS_EMM_ProcHoSysCommonCheckTauFlag
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2013-11-02  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_EMM_ProcHoSysCommonCheckTauFlag( VOS_VOID )
{
    /* 如果TA不在TA List中，需要发起TAU */
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:TAI not in TAI LIST init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过ESM本地修改承载上下文*/
    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag: Bearer Status Change.");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过UE_NET_CAP or DRX改变，触发TA-Updating TAU*/
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag: DrxOrNetCap Change.");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果当前注册域为PS，且当前有能力发起CS域的注册，则发起联合TAU */
    if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:current reg domain=PS,can register in cs domain");
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /*如果当前EU值不是EU1*/
    if(EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:EU value is not EU1");

        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /* 如果UE无线能力发生变更 */
    if(NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        NAS_EMM_TAU_StartTauForInterRat();
        return NAS_EMM_YES;
    }

    /*如果T3412曾经超时过*/
    if(NAS_EMM_YES == NAS_EMM_ProcSysCheck3412Exp() )
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysCommonCheckTauFlag:3412 exp");

        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}

/*leili modify for isr begin*/

VOS_VOID  NAS_EMM_ProcConnGu2LReselect( VOS_VOID )
{
    GMM_LMM_PACKET_MM_STATE_ENUM_UINT32 enPacketMmState = GMM_LMM_PACKET_MM_STATE_BUTT;
    MMC_LMM_TIN_TYPE_ENUM_UINT32        enTinType       = MMC_LMM_TIN_INVALID;

     /* 获取TIN值 */
    enTinType = NAS_EMM_GetTinType();

    /* 获取U模连接状态 */
    enPacketMmState = NAS_LMM_GetEmmInfoUConnState();


    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcConnGu2LReselect: GET MML PS BEARER INFO:");
    NAS_COMM_PrintArray(                NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);

    /* For case l, if the UE was in PMM-CONNECTED mode and the TIN indicates
       "RAT-related TMSI", the UE shall set the TIN to "P-TMSI" before initiating
       the combined tracking area updating procedure. */
    if ((GMM_LMM_PACKET_MM_STATE_PMM_CONNECTED == enPacketMmState)
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == enTinType))
    {
        NAS_EMM_SetTinType(MMC_LMM_TIN_P_TMSI);
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
        /*PC REPLAY MODIFY BY LEILI END*/
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcConnGu2LReselect: UPDATE MML PS BEARER INFO:");
        NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                        (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                        sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                        *EMM_ESM_MAX_EPS_BEARER_NUM);

    }
    return;
}
/*leili modify for isr end*/

VOS_VOID  NAS_EMM_ProcSysWhenRsmGu2LRegReselect( VOS_VOID )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32        enTinType       = MMC_LMM_TIN_INVALID;
    GMM_LMM_PACKET_MM_STATE_ENUM_UINT32 enPacketMmState = GMM_LMM_PACKET_MM_STATE_BUTT;
    GMM_LMM_GPRS_MM_STATE_ENUM_UINT32   enGprsMmState   = GMM_LMM_GPRS_MM_STATE_BUTT;
    NAS_LMM_RSM_SYS_CHNG_DIR_ENUM_UINT32    enRsmDir    = NAS_LMM_RSM_SYS_CHNG_DIR_BUTT;
    NAS_LMM_NETWORK_INFO_STRU          *pMmNetInfo      = NAS_EMM_NULL_PTR;

    /* 获取TIN值 */
    enTinType = NAS_EMM_GetTinType();

    /* 获取U模连接状态 */
    enPacketMmState = NAS_LMM_GetEmmInfoUConnState();

    /* For case l, if the UE was in PMM-CONNECTED mode and the TIN indicates
       "RAT-related TMSI", the UE shall set the TIN to "P-TMSI" before initiating
       the combined tracking area updating procedure. */
    /*leili modify for isr begin*/
    NAS_EMM_ProcConnGu2LReselect();
    /*leili modify for isr end*/

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI" */
    if (MMC_LMM_TIN_P_TMSI == enTinType)
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:P-TMSI init tau");

        if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
            && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:combined succ");

            /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                TAU类型应该填为combined TA/LA updating with IMSI attach */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        }
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /* 获取G模连接状态 */
    enGprsMmState = NAS_LMM_GetEmmInfoGConnState();

    /* 获取恢复方向 */
    enRsmDir = NAS_EMM_GetResumeDir();

    /* l)when the UE reselects an E-UTRAN cell while it was in GPRS READY state or PMM-CONNECTED mode; */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_MM_STATE_READY  == enGprsMmState))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:GPRS READY init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    if ((NAS_LMM_RSM_SYS_CHNG_DIR_W2L == enRsmDir)
        && (GMM_LMM_PACKET_MM_STATE_PMM_CONNECTED == enPacketMmState))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:PMM-CONNECTED init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /* c)when the UE performs an intersystem change from A/Gb mode to S1 mode and
         the EPS services were previously suspended in A/Gb mode;
         这个条件时联合TAU所独有的 */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:EPS service suspended init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
         and the UE previously either performed a location area update procedure
         or a combined routing area update procedure in A/Gb or Iu mode,
         in order to re-establish the SGs association. In this case the EPS update
         type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if (((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:LAU or Combined Rau init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:ISR act and P.4 init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /*把TA不在TA List等条件封装到下面函数判断*/
    if(NAS_EMM_YES == NAS_EMM_ProcSysCommonCheckTauFlag())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:entered NAS_EMM_ProcSysCommonCheckTauFlag");
        return;
    }

    /* 以上条件都不符合，则不需要触发TAU,直接回复正常服务*/
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegReselect:Trans Reg.Normal_Servie.");

    /* 获取EMM维护的网络信息*/
    pMmNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /* TA在TAI List中，更新 L.V.R TAI*/
    NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
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

    /* 确保EU状态为EU1*/
    NAS_EMM_TAUSER_SaveAuxFsmUpStat(EMM_US_UPDATED_EU1);

    return;
}
VOS_VOID  NAS_EMM_ProcSysWhenRsmGu2LRegRedirect( VOS_VOID )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32            ulTinType   = MMC_LMM_TIN_INVALID;
    NAS_LMM_RSM_SYS_CHNG_DIR_ENUM_UINT32    enRsmDir    = NAS_LMM_RSM_SYS_CHNG_DIR_BUTT;
    NAS_LMM_NETWORK_INFO_STRU              *pMmNetInfo  = NAS_EMM_NULL_PTR;

    /* 获取TIN值 */
    ulTinType = NAS_EMM_GetTinType();

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI" */
    if (MMC_LMM_TIN_P_TMSI == ulTinType)
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:P-TMSI init tau");

        if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
            && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
        {
            NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:combined succ");

            /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                TAU类型应该填为combined TA/LA updating with IMSI attach */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        }
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /* 获取恢复方向 */
    enRsmDir = NAS_EMM_GetResumeDir();

    /* c)when the UE performs an intersystem change from A/Gb mode to S1 mode and
         the EPS services were previously suspended in A/Gb mode;
         这个条件时联合TAU所独有的 */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:EPS service suspended init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
         and the UE previously either performed a location area update procedure
         or a combined routing area update procedure in A/Gb or Iu mode,
         in order to re-establish the SGs association. In this case the EPS update
         type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if (((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:LAU or Combined Rau init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:ISR act and P.4 init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /*把TA不在TA List等条件的判断，封装到下面的函数*/
    if(NAS_EMM_YES == NAS_EMM_ProcSysCommonCheckTauFlag())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:entered NAS_EMM_ProcSysCommonCheckTauFlag");
        return;
    }

    /* 以上条件都不符合，则不需要触发TAU,直接回复正常服务*/
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:Trans Reg.Normal_Servie.");

    /* 获取EMM维护的网络信息*/
    pMmNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /* TA在TAI List中，更新 L.V.R TAI*/
    NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);
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

    /* 确保EU状态为EU1*/
    NAS_EMM_TAUSER_SaveAuxFsmUpStat(EMM_US_UPDATED_EU1);

    return;
}
VOS_VOID  NAS_EMM_ProcSysWhenRsmGu2LHo( VOS_VOID )
{
    MMC_LMM_TIN_TYPE_ENUM_UINT32            ulTinType   = MMC_LMM_TIN_INVALID;
    NAS_LMM_RSM_SYS_CHNG_DIR_ENUM_UINT32    enRsmDir    = NAS_LMM_RSM_SYS_CHNG_DIR_BUTT;
    NAS_LMM_NETWORK_INFO_STRU              *pMmNetInfo  = NAS_EMM_NULL_PTR;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LHo: ENTER.");

    NAS_LMM_SetEmmInfoTriggerTauRrcRel(NAS_EMM_TRIGGER_TAU_RRC_REL_NO);

    /* 获取TIN值 */
    ulTinType = NAS_EMM_GetTinType();

    /* e)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI" */
    if (MMC_LMM_TIN_P_TMSI == ulTinType)
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LHo:P-TMSI init tau");

        if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
            && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
        {
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LHo:combined succ");

            /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                TAU类型应该填为combined TA/LA updating with IMSI attach */
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

        }
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /* ISR激活的情况下，判断是否需要发起TAU */

    /* 获取恢复方向 */
    enRsmDir = NAS_EMM_GetResumeDir();

    /* c)when the UE performs an intersystem change from A/Gb mode to S1 mode and
         the EPS services were previously suspended in A/Gb mode;
         这个条件时联合TAU所独有的 */
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (GMM_LMM_GPRS_SUSPENSION == NAS_LMM_GetEmmInfoPsState())
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:EPS service suspended init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
         and the UE previously either performed a location area update procedure
         or a combined routing area update procedure in A/Gb or Iu mode,
         in order to re-establish the SGs association. In this case the EPS update
         type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if (((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:LAU or Combined Rau init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return ;
    }

    /*q)When the UE performs an intersystem change from A/Gb mode to S1 mode and the TIN indicates "RAT-related TMSI",
    but the UE is required to perform tracking area updating for IMS voice termination as specified in 3GPP TS 24.008 [13],
    annex P.4*/
    if ((NAS_LMM_RSM_SYS_CHNG_DIR_G2L == enRsmDir)
        && (NAS_EMM_YES == NAS_EMM_IsAnnexP4ConditionSatisfied())
        && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:ISR act and P.4 init tau");
        NAS_EMM_TAU_StartTauForInterRat();
        return;
    }

    /*把TA不在TA List等条件的判断，封装到下面的函数*/
    if(NAS_EMM_YES == NAS_EMM_ProcHoSysCommonCheckTauFlag())
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:entered NAS_EMM_ProcSysCommonCheckTauFlag");
        return;
    }

    /* 以上条件都不符合，则不需要触发TAU,直接回复正常服务*/
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSysWhenRsmGu2LRegRedirect:Trans Reg.Normal_Servie.");

    /* 获取EMM维护的网络信息*/
    pMmNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    /* TA在TAI List中，更新 L.V.R TAI*/
    NAS_EMM_SetLVRTai(&pMmNetInfo->stPresentNetId);

    /* 转换EMM状态机MS_REG+SS_NORMAL_SEARVICE*/
    NAS_EMM_TAUSER_FSMTranState(    EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

    NAS_EMM_SetTauTypeNoProcedure();

    /*向MMC发送LMM_MMC_TAU_RESULT_IND消息*/
    NAS_EMM_MmcSendTauActionResultIndSuccWithoutPara();

    /* 确保EU状态为EU1*/
    NAS_EMM_TAUSER_SaveAuxFsmUpStat(EMM_US_UPDATED_EU1);

    return;
}
VOS_VOID  NAS_EMM_ProcSuitSysWhenRsmGu2LReg(VOS_VOID)
{
    switch ( NAS_EMM_GetResumeType())
    {
        case NAS_LMM_SYS_CHNG_TYPE_RSL:

            NAS_EMM_ProcSysWhenRsmGu2LRegReselect();
            break;

        case NAS_LMM_SYS_CHNG_TYPE_REDIR:

            NAS_EMM_ProcSysWhenRsmGu2LRegRedirect();
            break;
        case NAS_LMM_SYS_CHNG_TYPE_CCO:

            /* CCO需要发起建链，暂定发起TAU */
            if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
            && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
            {
                NAS_EMM_TAU_LOG_INFO("NAS_EMM_ProcSuitSysWhenRsmGu2LReg:combined succ");

                /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
                    TAU类型应该填为combined TA/LA updating with IMSI attach */
                NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

            }
            NAS_EMM_TAU_StartTauForInterRat();
            break;

        case NAS_LMM_SYS_CHNG_TYPE_HO:
            NAS_EMM_ProcSysWhenRsmGu2LHo();
            break;

        default:
            NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcSuitSysWhenRsmGu2LReg: HO.");
            break;
    }

    return;
}



VOS_VOID  NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LDeReg(VOS_VOID)
{
    VOS_UINT32                          ulSendResult = NAS_EMM_FAIL;

    /* 将状态转移至MS_DEREG + SS_DEREG_NORMAL_SERVICE状态 */
    NAS_EMM_PUBU_FSMTranState(          EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

    /* 若TIN ==  PTMSI，则获取GU的安全参数 */
    NAS_EMM_MmcRsmGetSecuPara();

    /* 向ATTACH模块发送EMM_INTRA_ATTACH_REQ消息 */
    ulSendResult = NAS_EMM_SendIntraAttachReq();
    if (NAS_EMM_FAIL == ulSendResult)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_ProcSuitSysWhenRrcOriRsmGu2LDeReg : Send IntraMsg Failure !");
        return ;
    }

    return;
}
VOS_VOID  NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LDeReg(VOS_VOID)
{
    VOS_UINT32                          ulSendResult;
    NAS_MM_TA_STRU                     *pstLastAttmpRegTa = NAS_EMM_NULL_PTR;
    NAS_MM_TA_STRU                      stCurTa           = {0};

    /* 获取当前TA和上次尝试注册的TA信息 */
    NAS_EMM_GetCurrentTa(&stCurTa);
    pstLastAttmpRegTa                  = NAS_LMM_GetEmmInfoNetInfoLastAttmpRegTaAddr();

    /* TA与上次尝试发起注册的TA相同，有定时器，则应该等超时*/
    if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatch(pstLastAttmpRegTa, &stCurTa))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LDeReg: The same as last attempt TA.");

        if(NAS_EMM_YES == NAS_EMM_Attach_IsTimerRunning())
        {
            return;
        }
    }

    /*重设attach attempt counter*/
    NAS_EMM_AttResetAttAttempCounter();

    /* 将状态转移至MS_DEREG + SS_DEREG_NORMAL_SERVICE状态 */
    NAS_EMM_PUBU_FSMTranState(          EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

    /* 若TIN ==  PTMSI，则获取GU的安全参数 */
    NAS_EMM_MmcRsmGetSecuPara();

    /* 向ATTACH模块发送EMM_INTRA_ATTACH_REQ消息 */
    ulSendResult = NAS_EMM_SendIntraAttachReq();
    if (NAS_EMM_FAIL == ulSendResult)
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LDeReg : Send IntraMsg Failure !");
        return ;
    }

    return;
}


VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgMmcPlmnInd
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;
    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgMmcPlmnInd is entered");

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_REG == NAS_EMM_GetMsBefResume())
    {
        /*MMC激活L模后需要设置TAU原因的条件*/
        NAS_EMM_MmcResumeSetTauStartCause();

        /* 将状态转移至MS_DEREG + SS_DEREG_PLMN_SEARCH状态 */
        NAS_EMM_PUBU_FSMTranState(      EMM_MS_REG,
                                        EMM_SS_REG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        /* 如果RESUME之前子状态为NO IMSI */
        if (EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefResume())
        {
            /* 将状态转移至MS_DEREG + SS_DEREG_NO_IMSI状态 */
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* 将状态转移至MS_DEREG + SS_DEREG_NORMAL_SERVICE状态 */
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_VOID  NAS_EMM_ResumeProcPTMSITau( VOS_VOID )
{

    NAS_EMM_TAU_LOG2_INFO("NAS_EMM_ProcPTMSITau:NAS_EMM_TAU_GetEmmT3412ExpCtrl() = ,\
        NAS_LMM_GetEmmInfoRegDomain()", NAS_LMM_GetEmmInfoT3412ExpCtrl(),
        NAS_LMM_GetEmmInfoRegDomain());

    if ((NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL == NAS_LMM_GetEmmInfoT3412ExpCtrl())
        && (NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain()))
    {
        /* 将注册域改为PS，是为了回到REG+NORMAL_SERVICE态后，如果要发起联合TAU，则
            TAU类型应该填为combined TA/LA updating with IMSI attach */
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);

    }
    /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
    NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
    NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
    return;
}
VOS_VOID  NAS_EMM_MmcResumeSetTauStartCause(VOS_VOID)
{

    if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcResumeSetTauStartCause:LAU OR COMBINED RAU");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /*c)when the UE enters EMM-REGISTERED.NORMAL-SERVICE and the UE's TIN indicates "P-TMSI"*/
    if (MMC_LMM_TIN_P_TMSI == NAS_EMM_GetTinType())
    {
        NAS_EMM_ResumeProcPTMSITau();
        return;
    }

    /* d)when the UE performs an intersystem change from A/Gb or Iu mode to S1 mode,
           and the UE previously either performed a location area update procedure
           or a combined routing area update procedure in A/Gb or Iu mode,
           in order to re-establish the SGs association. In this case the EPS update
           type IE shall be set to "combined TA/LA updating with IMSI attach"; */
    if (NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
    {
        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
    }

    return;
}
VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAreaLostInd
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAreaLostInd:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

    /* 关闭当前EMM的状态定时器*/
    NAS_LMM_StopAllEmmStateTimer();

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        /* 如果RESUME之前是NO IMSI态 */
        if(EMM_SS_DEREG_NO_IMSI== NAS_EMM_GetSsBefResume())
        {
            /* 将状态转移至MS_DEREG + SS_DEREG_NO_IMSI状态 */
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_IMSI,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* 将状态转移至MS_DEREG + SS_DEREG_NO_CELL_AVAILABLE状态 */
            NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
        }
        return NAS_LMM_MSG_HANDLED;
    }

    /*MMC激活L模后需要设置TAU原因的条件*/
    NAS_EMM_MmcResumeSetTauStartCause();

    /* 将状态转移至MS_REG + SS_REG_NO_CELL_AVAILABLE状态 */
    NAS_EMM_PUBU_FSMTranState(          EMM_MS_REG,
                                        EMM_SS_REG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgMmcDetachReq
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;

    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgMmcDetachReq is entered.");

    /* 通知ESM执行了本地DETACH */
    NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /* 给MMC回DETACH成功 */
    NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

    /* 有可能是处于紧急注册的无卡状态 */
    if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
    {
        /* 无卡 */
        NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                               EMM_SS_DEREG_NO_IMSI,
                               TI_NAS_EMM_STATE_NO_TIMER);
    }
    else
    {
        /* 有卡 */
        NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                               EMM_SS_DEREG_PLMN_SEARCH,
                               TI_NAS_EMM_STATE_NO_TIMER);
    }

    /* 本地DETACH释放资源:动态内存、赋初值 */
    NAS_LMM_DeregReleaseResource();

    return NAS_LMM_MSG_HANDLED;

}

#if 0
/*该函数需要考虑增加判断注册域为CS的处理 leili 2012-6-20*/

VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAppAttachReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    MMC_LMM_ATTACH_REQ_STRU             *pRcvEmmMsg;

    (void)ulMsgId;
    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAppAttachReq is entered");

    pRcvEmmMsg = (MMC_LMM_ATTACH_REQ_STRU*)pMsgStru;

    /*消息内容检查,若有错，打印并退出*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_AppAttachReqMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAppAttachReq: MMC_LMM_ATTACH_REQ_STRU PARA ERR!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /*保存APP参数*/
    NAS_EMM_SaveAppMsgPara(pRcvEmmMsg->ulMsgId,pRcvEmmMsg->ulOpId);

    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgAppAttachReq:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        /*向MMC发送ATTACH失败*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_FAILURE);
    }
    else
    {
        /*向MMC发送ATTACH成功*/
        #if 0
        NAS_EMM_AppSendAttRstSuccFail(MMC_LMM_ATT_RSLT_SUCCESS);
        #endif
        NAS_EMM_MmcSendAttCnf(MMC_LMM_ATT_RSLT_SUCCESS);
    }

    return NAS_LMM_MSG_HANDLED;
}

#endif
/*****************************************************************************
 Function Name   : NAS_EMM_MmcOriResumeCheckTauFlag
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY         2012-02-27   Draft Enact

*****************************************************************************/
VOS_UINT32   NAS_EMM_MmcOriResumeCheckTauFlag(VOS_VOID)
{
    /* 如果TA不在TAI List中，触发TA-Updating类型的TAU*/
    if (NAS_EMM_FAIL == NAS_EMM_TAU_IsCurrentTAInTaList())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:TA not in TAI List.");

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过ESM本地修改承载上下文*/
    if(NAS_EMM_EPS_BEARER_STATUS_CHANGEED == NAS_EMM_GetEpsContextStatusChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:Bearer Status Change.");
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /* 如果期间发生过UE_NET_CAP or DRX改变，触发TA-Updating TAU*/
    if(NAS_EMM_YES == NAS_LMM_GetEmmInfoDrxNetCapChange())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:DrxOrNetCap Change.");
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /*当前注册域为PS,并且有能力发起CS域的注册*/
    if (NAS_EMM_YES == NAS_EMM_TAU_CanTriggerComTauWithIMSI())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:current reg domain=PS,can register in cs domain.");

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(     NAS_EMM_TAU_START_CAUSE_SYSINFO);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /*EU值不是EU1*/
    if(EMM_US_UPDATED_EU1 != NAS_EMM_TAUSER_GetAuxFsmUpStat() )
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag: EU is not EU1");
        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);

        /*NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_BEARER_CNTXT_LOCAL_CHANGED);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }

    /*对RRC Rel原因标识进行判断*/
    if(NAS_EMM_YES == NAS_EMM_TAU_RrcRelCauseTriggerTau())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:rrc rel cause");

        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return NAS_EMM_YES;
    }


    /* 如果UE无线能力发生变更 */
    if(NAS_LMM_UE_RADIO_CAP_NOT_CHG != NAS_LMM_GetEmmInfoUeRadioCapChgFlag())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag: UE radio capability change. ");
        /*NAS_EMM_TAU_SaveEmmTAUStartCause( NAS_EMM_TAU_START_CAUSE_INTRA_TAU_REQ);*/
        NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        NAS_EMM_TAU_StartTAUREQ();
        return NAS_EMM_YES;
    }


    /* 处理定时器T3412超时 */
    if (NAS_LMM_MSG_HANDLED == NAS_EMM_RegSomeStateMsgSysInfoT3412ExpProc())
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MmcOriResumeCheckTauFlag:T3412 Exp.");

        NAS_LMM_SetEmmInfoTriggerTauSysChange(NAS_EMM_YES);
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_VOID  NAS_EMM_MmcOriResumeSysInfoProc(VOS_VOID)
{
    /* 如果T3411/3402在运行，且需要等超时，则先将该场景识别出来 */
    if (NAS_EMM_SUCC == NAS_EMM_RegStateMmcOriResumeSysInfoRecogniseWaitTimerExp())
    {
        return;
    }

    /* 注册态下收到系统消息,不需要等T3411或者T3402超时的公共处理 */

    NAS_EMM_RegStateMmcOriResumeSysInfoNeednotWaitTimerExpProc();

    return;
}


VOS_VOID  NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LReg(VOS_VOID)
{

    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LReg is entered");
    if (NAS_RELEASE_CTRL)
    {
        if(NAS_MML_PS_BEARER_STATE_INACTIVE == NAS_EMM_IsEpsBearStatusAct())
        {
            NAS_EMM_GU2LNoEpsBearProc();
        }
        else
        {
            /* 进入收到SYS_INFO的公共处理*/
            NAS_EMM_MmcOriResumeSysInfoProc();
        }
    }
    else
    {
        /* 进入收到SYS_INFO的公共处理*/
        NAS_EMM_MmcOriResumeSysInfoProc();
    }
    return;
}



VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgSysInfoInd
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    EMMC_EMM_SYS_INFO_IND_STRU         *pstsysinfo;

    (VOS_VOID)ulMsgId;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgSysInfoInd: ResumeType = ",
                        NAS_EMM_GetResumeType());

    pstsysinfo = (EMMC_EMM_SYS_INFO_IND_STRU *)pMsgStru;

    if(((NAS_EMM_LAU_OR_COMBINED_RAU_HAPPENED == NAS_LMM_GetEmmInfoLauOrComRauFlag())
        || (NAS_EMM_SRVCC_HAPPENED == NAS_LMM_GetEmmInfoSrvccFlag()))
        && (NAS_EMM_YES == NAS_EMM_IsCsPsUeMode()))
    {
        NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgSysInfoInd:LAU OR COMBINED RAU");
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }

    /* 如果RESUME之前状态为MS_DEREG + SS_DEREG_NO_IMSI */
    if (EMM_SS_DEREG_NO_IMSI == NAS_EMM_GetSsBefResume())
    {
        /* 将状态转移至MS_DEREG + SS_DEREG_NO_IMSI状态 */
        NAS_EMM_PUBU_FSMTranState(  EMM_MS_DEREG,
                                    EMM_SS_DEREG_NO_IMSI,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_FAILURE);;

        return NAS_LMM_MSG_HANDLED;
    }


    if ((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
        || (EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgSysInfoInd: Forb sys info");

        /*根据RESUME前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
        {
            /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
            NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

            NAS_EMM_SetAttType();

            /*向MMC发送LMM_MMC_ATTACH_CNF或LMM_MMC_ATTACH_IND消息*/
            NAS_EMM_AppSendAttRsltForb(pstsysinfo->ulForbiddenInfo);
        }
        else
        {   /* lihong00150010 emergency tau&service begin */
            if (NAS_LMM_REG_STATUS_NORM_REGED == NAS_LMM_GetEmmInfoRegStatus())
            {
				/*MMC激活L模后需要设置TAU原因的条件*/
                NAS_EMM_MmcResumeSetTauStartCause();
            	NAS_EMM_RegForbidSysInfoProc(pstsysinfo->ulForbiddenInfo);
            }
            else
            {
                NAS_EMM_MmcOriResumeSysInfoProc();
            }/* lihong00150010 emergency tau&service end */
        }

        return NAS_LMM_MSG_HANDLED;
    }

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LDeReg();
    }
    else
    {
        /* 判断承载上下文是否激活，如果未激活，则本地DETACH，转入DEREG+NORMAL_SERVICE状态；
            发起ATTACH流程；
            如果激活，则保持原有处理 */
        NAS_EMM_ProcSuitSysWhenMmcOriRsmGu2LReg();
    }
    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgT3412Exp
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgT3412Exp:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        return NAS_LMM_MSG_HANDLED;
    }

    /*设置周期TAU定制器超时标志*/
	/* 针对GCF9.2.3.1.5 用例，UE丢网，MMC发起搜网，搜网期间T3412超时，发起的TAU类型是combined TAU with IMSI类型的*/
    NAS_LMM_SetEmmInfoT3412ExpCtrl(NAS_EMM_T3412_EXP_YES_REG_NO_AVALABLE_CELL);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgBearStatusReq
(
    VOS_UINT32  ulMsgId,
    VOS_VOID   *pMsgStru
)
{
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

    /*根据RESUME前的状态进行状态迁移*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        return NAS_LMM_MSG_HANDLED;
    }

    /* 设置承载上下文变化*/
    NAS_EMM_SetEpsContextStatusChange(NAS_EMM_EPS_BEARER_STATUS_CHANGEED);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsResumeReverseMsgRrcAccessGrantInd(
                                        VOS_UINT32  ulMsgId,
                                  const VOS_VOID   *pMsgStru  )
{
    VOS_UINT32                          ulRslt;
    (void)ulMsgId;
    (void)pMsgStru;
    NAS_EMM_PUBU_LOG_NORM("NAS_EMM_MsResumeReverseMsgRrcAccessGrantInd entered.");

    /* 检查ResumeType，如果不是重选回退，则打印出错信息*/
    if(LRRC_LMM_SYS_CHNG_TYPE_RESEL_REVERSE != NAS_EMM_GetResumeType())
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_EMM_MsResumeReverseMsgRrcAccessGrantInd:ResumeType Err.");
        return NAS_LMM_MSG_HANDLED;
    }

    /* 如果是重选回退，则与挂起态下的处理相同*/
    ulRslt = NAS_EMM_MsSuspendSsRrcOriMsgRrcAccessGrantInd(ulMsgId, pMsgStru);

    return ulRslt;
}

VOS_UINT32  NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgIntraTauReq( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{
    NAS_LMM_INTRA_TAU_REQ_STRU         *pIntraTauReq = NAS_EMM_NULL_PTR;
    (VOS_VOID)ulMsgId;
    pIntraTauReq    = (NAS_LMM_INTRA_TAU_REQ_STRU*)pMsgStru;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgIntraTauReq is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgIntraTauReq PARA NULL!!");
        return NAS_LMM_MSG_DISCARD;
    }

    if((NAS_LMM_SYS_CHNG_TYPE_RSL == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_HO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_CCO == NAS_EMM_GetResumeType()) ||
        (NAS_LMM_SYS_CHNG_TYPE_REDIR == NAS_EMM_GetResumeType()))
    {
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetMsBefResume() =",
                            NAS_EMM_GetMsBefResume());

        /*根据RESUME前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*设置DRX或NetCapability改变标志*/
        if(NAS_LMM_INTRA_TAU_TYPE_PARA_CHANGE == pIntraTauReq->enIntraTauType)
        {
            NAS_LMM_SetEmmInfoDrxNetCapChange( NAS_EMM_YES);
        }
    }
    else
    {
        /*回退过程的处理*/
        NAS_EMM_PUBU_LOG1_NORM("NAS_EMM_MsResumeSsRrcOriWaitSysInfoIndMsgBearStatusReq:NAS_EMM_GetMsBefResume() =",
                                    NAS_EMM_GetMsBefSuspend());

        /*根据SUSPEND前的状态进行状态迁移*/
        if(EMM_MS_DEREG == NAS_EMM_GetMsBefSuspend())
        {
            return NAS_LMM_MSG_HANDLED;
        }

        /*设置DRX或NetCapability改变标志*/
        if(NAS_LMM_INTRA_TAU_TYPE_PARA_CHANGE == pIntraTauReq->enIntraTauType)
        {
            NAS_LMM_SetEmmInfoDrxNetCapChange( NAS_EMM_YES);
        }
    }

    return NAS_LMM_MSG_HANDLED;
}
/*lint +e961*/
/*lint +e960*/

VOS_UINT32  NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgIntraTauReq( VOS_UINT32  ulMsgId,
                                                   const VOS_VOID   *pMsgStru )
{
    NAS_LMM_INTRA_TAU_REQ_STRU         *pIntraTauReq = NAS_EMM_NULL_PTR;
    (VOS_VOID)ulMsgId;
    pIntraTauReq    = (NAS_LMM_INTRA_TAU_REQ_STRU*)pMsgStru;

    /* 打印进入该函数*/
    NAS_EMM_TAU_LOG_INFO( "NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgIntraTauReq is entered.");

    /* 函数输入指针参数检查*/
    if(NAS_EMM_NULL_PTR                 == pMsgStru)
    {
        NAS_EMM_TAU_LOG_ERR("NAS_EMM_MsResumeSsMmcOriWaitSysInfoIndMsgIntraTauReq PARA NULL!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*如果RESUME前的状态DEREG，则不设置*/
    if(EMM_MS_DEREG == NAS_EMM_GetMsBefResume())
    {
        return NAS_LMM_MSG_HANDLED;
    }

    /*设置DRX或NetCapability改变标志*/
    if(NAS_LMM_INTRA_TAU_TYPE_PARA_CHANGE == pIntraTauReq->enIntraTauType)
    {
        NAS_LMM_SetEmmInfoDrxNetCapChange( NAS_EMM_YES);
    }

    return NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_LMM_PreProcMmcImsVoiceCapChangeNotify(MsgBlock *    pMsg )
{

    MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU     *pstImsVoiceCapNotify;
    NAS_EMM_PUB_INFO_STRU                        *pstPubInfo;
    MMC_LMM_IMS_VOICE_CAP_ENUM_UINT8              enImsaVoiceCap;

    NAS_EMM_PUBU_LOG_INFO("NAS_LMM_PreProcMmcImsVoiceCapChangeNotify is entered");

    pstImsVoiceCapNotify = (VOS_VOID*)pMsg;
    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    enImsaVoiceCap = pstPubInfo->enImsaVoiceCap;

    pstPubInfo->enImsaVoiceCap = pstImsVoiceCapNotify->enImsVoiceCap;

    if ((MMC_LMM_IMS_VOICE_CAP_UNAVAILABLE == enImsaVoiceCap)
        && (MMC_LMM_IMS_VOICE_CAP_AVAILABLE == pstImsVoiceCapNotify->enImsVoiceCap))
    {
        NAS_EMM_PUBU_LOG_INFO("NAS_LMM_PreProcMmcImsVoiceCapChangeNotify: IMS voice avail");

        if ((NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
            && ((NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE))
                ||(NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE))
                ||(NAS_EMM_PUBU_CHK_STAT_VALID(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM))))
        {
            /*p）when the UE activates mobility management for IMS voice termination as specified in 3GPP TS 24.008 [13],
            annex P.2, and the TIN indicates "RAT-related TMSI";*/
            /*IMS voice from not available to available,ISR激活，start TAU*/
            /*lint -e960*/
            if ((NAS_EMM_YES == NAS_EMM_IsAnnexP2ConditionSatisfied())
                && (MMC_LMM_TIN_RAT_RELATED_TMSI == NAS_EMM_GetTinType()))
            {
                NAS_EMM_PUBU_LOG_INFO("NAS_LMM_PreProcMmcImsVoiceCapChangeNotify: start TAU");
                NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
                NAS_EMM_TAU_StartTAUREQ();
            }
            /*lint +e960*/
        }

    }

    return NAS_LMM_MSG_HANDLED;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

/* end of 子系统+模块+文件名.c */

