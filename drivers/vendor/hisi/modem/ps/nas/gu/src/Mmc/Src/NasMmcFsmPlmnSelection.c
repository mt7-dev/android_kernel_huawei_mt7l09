

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"

#include "NasMmcFsmPlmnSelectionTbl.h"
#include "NasMmcCtx.h"
#include "NasMmcFsmPlmnSelection.h"
#include "NasMmcSndGuAs.h"
#include "NasMmcSndLmm.h"
#include "NasMmcSndGmm.h"
#include "NasMmcSndMm.h"
#include "NasMmcProcRegRslt.h"
#include "NasMmcPlmnSelectionStrategy.h"
#include "NasMmcTimerMgmt.h"
#include "NasComm.h"
#include "NasMmlLib.h"
#include "NasMmlCtx.h"
#include "NasMmlMsgProc.h"
#include "NasMmcSndMma.h"
#include "NasMmcProcUsim.h"
#include "NasMmcProcSuspend.h"
#include "NasMmcSndOm.h"
#include "NasMmcProcNvim.h"
/* 删除ExtAppMmcInterface.h*/
#include "MmcMmInterface.h"
#include "MmcGmmInterface.H"
#include "NasMmcComFunc.h"

#include "MmaMmcInterface.h"
#include "NasUtranCtrlInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_FSM_PLMN_SELECTION_C



VOS_UINT32 NAS_MMC_RcvTafPlmnSearchReq_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;


	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 上报AT搜网开始 */
    NAS_MMC_SndMmaPlmnSelectStartInd();

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 根据场景初始化搜网列表信息 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_SWITCH_ON,
                                  VOS_NULL_PTR,
                                  pstPlmnSelectionListInfo);

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 获取需要搜索的网络及其接入技术 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {

        /* 选网状态机发起选网，则清除NO RF标记 */
        NAS_MMC_ClearNoRfInfo();
        
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向MM、GMM发送搜网指示 */
        NAS_MMC_SndGmmPlmnSchInit();
        NAS_MMC_SndMmPlmnSchInit();

        NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

        NAS_MMC_UpdateRegStateSpecPlmnSearch();

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);

#if   (FEATURE_ON == FEATURE_LTE)
        /* 开机搜网时，根据卡类型更新Lte的能力，如果是SIM卡，需要Disable Lte */
        NAS_MMC_DisableLteForSimReason_PlmnSelection();
#endif

    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTafPlmnSearchReq_PlmnSelection_Init: Get Next Search Plmn fail at switch on.");

        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enNetRatType;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;

    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapabilityStatus;
#endif

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 上报AT搜网开始 */
    NAS_MMC_SndMmaPlmnSelectStartInd();

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

#if (FEATURE_ON == FEATURE_LTE)
    enLteCapabilityStatus = NAS_MML_GetLteCapabilityStatus();
#endif

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 保存手动搜网信息 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                       pstUserSelReqMsg->enAccessMode);

    /* 根据场景初始化搜网列表信息 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSelectionListInfo);


    /* 选网状态机发起选网，则清除NO RF标记 */
    NAS_MMC_ClearNoRfInfo();

    if ( VOS_TRUE == NAS_MMC_IsNormalServiceStatus() )
    {
        /* SYSCFG设置后需要搜网，更新为限制服务状态并上报 */
        NAS_MMC_SetCsServiceStatus(NAS_MMC_LIMITED_SERVICE);
        NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_NOT_REG_PLMN_SEARCHING, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

        NAS_MMC_UpdateRegStateSpecPlmnSearch();
    }


    /* 当前未驻留,发起搜网时先设置搜网标记,触发状态机退出后的anycell搜网 */
    if (NAS_MMC_AS_CELL_NOT_CAMP_ON  == NAS_MMC_GetAsCellCampOn())
    {
        NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);
    }

    /* 获取当前接入技术 */
    enNetRatType = NAS_MML_GetCurrNetRatType();

#if (FEATURE_ON == FEATURE_LTE)
    if ( VOS_TRUE == NAS_MMC_IsNeedEnableLte_PlmnSelection(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH) )
    {
        if ((VOS_FALSE == NAS_MML_GetDisableLteRoamFlg())
         && (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLteCapabilityStatus))
        {
            /* 向WAS/GAS发送enable LTE通知消息 */
            NAS_MMC_SndAsLteCapabilityStatus(WUEPS_PID_WRR, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);
            NAS_MMC_SndAsLteCapabilityStatus(UEPS_PID_GAS, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);

            /* 向LMM发送enable LTE消息 */
            NAS_MMC_SndLmmEnableLteNotify();
        }

        if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER))
        {
            NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER);
        }

        /* 更新disable LTE能力标记 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }
#endif

    /* 信令连接存在 */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        /* 根据当前不同的接入技术,发送释放请求,迁移到不同的等待连接释放的L2状态，并启动保护定时器 */
        NAS_MMC_SndRelReq_PlmnSelection(enNetRatType);

        return VOS_TRUE;
    }

    /* 获取下个网络失败，则退出状态机 */
    if (VOS_TRUE != NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 打印异常 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_Init: Get Next Search Plmn fail at user spec search.");

        /* 向MMA报指定搜网失败 */
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);


        /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
        NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出层二状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);


    /* 判断需要搜索网络接入技术与当前接入技术是否相同,或者当前接入技术为Butt,
       对应的场景是选网状态机挂起接入层状态接收到Abort消息,接入层回复SUSPNED CNF
       后会将当前接入模式设置为Butt再退出,表示可以向任何接入技术直接发起搜网 */
    if ((enNetRatType == stDestPlmn.enRat)
     || (NAS_MML_NET_RAT_TYPE_BUTT == enNetRatType))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    else
    {
        /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSuspendReq_PlmnSelection(enNetRatType);
    }

    NAS_MMC_UpdateRegStateSpecPlmnSearch();


    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvMmcMmcInterPlmnSearchReq_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU                     *pstPlmnSearchReq = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enNetRatType;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;

    VOS_UINT32                                              ulIsNormalService;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enCampPlmnNetRat;
    NAS_MML_LAI_STRU                                       *pstLai;
    VOS_UINT32                                              ucSpecPlmnSearchFlg;
    VOS_UINT32                                              ulSingleDomainPlmnListFlg;


    VOS_UINT8                                               ucCsAttachAllowFlg;
    VOS_UINT8                                               ucPsAttachAllowFlg;


#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurPlmn;

    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapabilityStatus;

    VOS_UINT32                                              ulIsCsPsMode1NeedPeriodSearchGU;

    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    VOS_UINT8                                               ucEmcFlg;
    NAS_MMC_TIMER_STATUS_ENUM_U8                            enTimerStatus;

    /* 获取当前驻留的网络信息 */
    pstCurCampInfo     = NAS_MML_GetCurrCampPlmnInfo();
    stCurPlmn.stPlmnId = pstCurCampInfo->stLai.stPlmnId;
    stCurPlmn.enRat    = pstCurCampInfo->enNetRatType;
    enLteCapabilityStatus = NAS_MML_GetLteCapabilityStatus();
#endif

    ucCsAttachAllowFlg = NAS_MML_GetCsAttachAllowFlg();
    ucPsAttachAllowFlg = NAS_MML_GetPsAttachAllowFlg();

    /* 德电需求:单域注册失败发起搜网特性打开时,如果注册失败原因值是#17,需要发起搜网 */
    ulSingleDomainPlmnListFlg = NAS_MMC_IsSingleDomainRegFailNeedPlmnSrch(MMA_MMC_SRVDOMAIN_CS);
    ulSingleDomainPlmnListFlg |= NAS_MMC_IsSingleDomainRegFailNeedPlmnSrch(MMA_MMC_SRVDOMAIN_PS);

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 上报AT搜网开始 */
    NAS_MMC_SndMmaPlmnSelectStartInd();

    ucSpecPlmnSearchFlg = NAS_MMC_GetSpecPlmnSearchState();
    pstLai              = NAS_MML_GetCurrCampLai();
    enCampPlmnNetRat    = pstLai->enCampPlmnNetRat;

    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 内部搜网请求处理  */
    pstPlmnSearchReq = (NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU*)pstMsg;

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 根据场景初始化搜网列表信息 */
    NAS_MMC_InitPlmnSelectionList(pstPlmnSearchReq->enPlmnSearchScene,
                                  &(pstPlmnSearchReq->astInterPlmnSearchInfo[0]),
                                  pstPlmnSelectionListInfo);


    /* 选网状态机发起选网，则清除NO RF标记 */
    NAS_MMC_ClearNoRfInfo();

    /* 不区分搜网场景统一根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchInfo_PlmnSelection(pstPlmnSearchReq, pstPlmnSelectionListInfo);
#if (FEATURE_ON == FEATURE_LTE)
    /* 存在CSFB的EMC或者缓存的EMC，并且ENABLE LTE定时器运行期间,不ENABLE LTE */
    ucEmcFlg      = NAS_MML_GetCsEmergencyServiceFlg();
    enTimerStatus = NAS_MMC_GetTimerStatus(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER);
    if (( (MMC_LMM_DISABLE_LTE_REASON_EMERGENCY_CALL == NAS_MML_GetDisableLteReason())
       || (VOS_TRUE == ucEmcFlg))
     && (NAS_MMC_TIMER_STATUS_RUNING == enTimerStatus))
    {
        enPlmnSearchScene = NAS_MMC_PLMN_SEARCH_SCENE_DISABLE_LTE;
    }
    else
    {
        enPlmnSearchScene = pstPlmnSearchReq->enPlmnSearchScene;
    }
    if ( VOS_TRUE == NAS_MMC_IsNeedEnableLte_PlmnSelection(enPlmnSearchScene) )
    {
        if ((VOS_FALSE == NAS_MML_GetDisableLteRoamFlg())
         && (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLteCapabilityStatus))
        {
            /* 向WAS/GAS发送enable LTE通知消息 */
            NAS_MMC_SndAsLteCapabilityStatus(WUEPS_PID_WRR, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);
            NAS_MMC_SndAsLteCapabilityStatus(UEPS_PID_GAS, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);

            /* 向LMM发送enable LTE消息 */
            NAS_MMC_SndLmmEnableLteNotify();
        }

        if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER))
        {
            NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER);
        }

        /* 更新disable LTE能力标记 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /*判断当前是否已经驻留在L网络提供正常服务，
      对应的场景是CS/PS mode 1重回LTE后，周期性搜GU网络*/
    ulIsCsPsMode1NeedPeriodSearchGU = NAS_MMC_IsCsPsMode1NeedPeriodSearchGU();

    if ((VOS_TRUE == NAS_MMC_IsCampLteNormalService())
     && (VOS_TRUE == ulIsCsPsMode1NeedPeriodSearchGU))
    {
        /* 保存当前网络到状态机上下文 */
        NAS_MMC_SetCsPsMode1ReCampLtePlmn_PlmnSelection(NAS_MML_GetCurrCampPlmnId());

        /* 更新选网列表中当前网络状态为已注册 */
        NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                                       NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                       pstPlmnSelectionListInfo);
    }
#endif

    /* list搜网状态机退出有两种搜网场景NAS_MMC_PLMN_SEARCH_SCENE_USER_LIST和NAS_MMC_PLMN_SEARCH_SCENE_DISABLE_LTE*/
    if ((NAS_MMC_PLMN_SEARCH_SCENE_USER_LIST == pstPlmnSearchReq->enPlmnSearchScene)
     || (NAS_MMC_PLMN_SEARCH_SCENE_DISABLE_LTE == pstPlmnSearchReq->enPlmnSearchScene))
    {
        /* 如果cs和ps都是限制服务，list搜网结果中有当前网络，无需重复搜索当前网络*/
        ulIsNormalService = NAS_MMC_IsNormalServiceStatus();

        if (((VOS_FALSE == ulIsNormalService)
          || (VOS_TRUE  == ulSingleDomainPlmnListFlg))
         && (NAS_MMC_SPEC_PLMN_SEARCH_STOP == ucSpecPlmnSearchFlg))
        {
            /* svlte modem0 ps迁移到modem1，g下限制驻留，available定时器超时，g下内部列表搜带上来l的网络和g当前驻留
               网络，用户列表搜场景搜网无需删除当前驻留g的网络，因为如果后续指定搜l的网络失败，需要重新回到g */
            if (!((VOS_TRUE == NAS_MML_GetSvlteSupportFlag())
              && (VOS_FALSE == ucCsAttachAllowFlg)
              && (VOS_FALSE == ucPsAttachAllowFlg)))
            {
                /* 删除当前注册被拒的网络 */
                NAS_MMC_DelSpecPlmnWithRatInPlmnList(NAS_MML_GetCurrCampPlmnId(),
                                                    enCampPlmnNetRat,
                                                    pstPlmnSelectionListInfo);
            }

        }

    }

    /* 当前未驻留,发起搜网时先设置搜网标记,触发状态机退出后的anycell搜网 */
    if (NAS_MMC_AS_CELL_NOT_CAMP_ON  == NAS_MMC_GetAsCellCampOn())
    {
        NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);
    }
    /* 获取下个网络失败，则退出状态机 */
    if (VOS_TRUE != NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 打印异常 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvMmcMmcInterPlmnSearchReq_PlmnSelection_Init: Get Next Search Plmn fail at inter search.");

        if ( (VOS_TRUE == NAS_MMC_IsSorTriggerAdditionalLau())
          && (NAS_MMC_PLMN_SEARCH_SCENE_USER_LIST == pstPlmnSearchReq->enPlmnSearchScene) )
        {
            /* 清除additional lau信息 */
            NAS_MML_InitSorAdditionalLauCtx();
            
            /* LIST搜结束后，通知MM发送触发SOR的LAU */
            NAS_MMC_SndMmLauReq(MMC_MM_LAU_REQ_REASON_SOR);           
        }
        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 获取当前接入技术 */
    enNetRatType = NAS_MML_GetCurrNetRatType();

    /*信令连接存在*/
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        /* 根据当前不同的接入技术,发送释放请求,迁移到不同的等待连接释放的L2状态，并启动保护定时器 */
        NAS_MMC_SndRelReq_PlmnSelection(enNetRatType);

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    /* 判断需要搜索网络接入技术与当前接入技术是否相同,或者当前接入技术为Butt,
       对应的场景是选网状态机挂起接入层状态接收到Abort消息,接入层回复SUSPNED CNF
       后会将当前接入模式设置为Butt再退出,表示可以向任何接入技术直接发起搜网 */
    if ((enNetRatType == stDestPlmn.enRat)
     || (NAS_MML_NET_RAT_TYPE_BUTT == enNetRatType))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    /*需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    else
    {
        /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSuspendReq_PlmnSelection(enNetRatType);
    }

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvMmaAcqReq_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enNetRatType;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;
    MMA_MMC_ACQ_REQ_STRU                                   *pstAcqReq = VOS_NULL_PTR;

    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 上报AT搜网开始 */
    NAS_MMC_SndMmaPlmnSelectStartInd();

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    pstAcqReq = (MMA_MMC_ACQ_REQ_STRU*)pstMsg;

    /* 根据获取原因进行网络列表初始化 */
    if (MMA_MMC_ACQ_REASON_POWER_UP == pstAcqReq->enAcqReason)
    {
        NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_SWITCH_ON,
                                      VOS_NULL_PTR,
                                      pstPlmnSelectionListInfo);
    }
    else if (MMA_MMC_ACQ_REASON_OOS == pstAcqReq->enAcqReason)
    {
        NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED,
                                      VOS_NULL_PTR,
                                      pstPlmnSelectionListInfo);
    }
    else
    {
        NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_HIGH_PRIO_PLMN_SEARCH,
                                      VOS_NULL_PTR,
                                      pstPlmnSelectionListInfo);
    }

    if (VOS_TRUE != NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 打印异常 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvMmaAcqReq_PlmnSelection_Init: Get Next Search Plmn fail at inter search.");

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 获取当前接入技术 */
    enNetRatType = NAS_MML_GetCurrNetRatType();

    /*信令连接存在*/
    if ((VOS_TRUE == NAS_MML_IsRrcConnExist())
     && (NAS_MML_NET_RAT_TYPE_BUTT != enNetRatType))
    {
        /* 根据当前不同的接入技术,发送释放请求,迁移到不同的等待连接释放的L2状态，并启动保护定时器 */
        NAS_MMC_SndRelReq_PlmnSelection(enNetRatType);

        return VOS_TRUE;
    }

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    /* 判断需要搜索网络接入技术与当前接入技术是否相同,或者当前接入技术为Butt,
       对应的场景是选网状态机挂起接入层状态接收到Abort消息,接入层回复SUSPNED CNF
       后会将当前接入模式设置为Butt再退出,表示可以向任何接入技术直接发起搜网 */
    if ((enNetRatType == stDestPlmn.enRat)
     || (NAS_MML_NET_RAT_TYPE_BUTT == enNetRatType))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    /*需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    else
    {
        /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSuspendReq_PlmnSelection(enNetRatType);
    }

#if   (FEATURE_ON == FEATURE_LTE)
    /* 如果开机获取，需要判断SIM卡进行Disable Lte操作 */
    if (MMA_MMC_ACQ_REASON_POWER_UP == pstAcqReq->enAcqReason)
    {
        NAS_MMC_DisableLteForSimReason_PlmnSelection();
    }
#endif

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF);
    
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /* 通知AS停止指定搜网 */
    NAS_MMC_SndAsPlmnSrchStopReq(WUEPS_PID_WRR);

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvRrMmRelInd_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulResult;
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

    /* 无需判断原因值 */

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvRrMmRelInd_PlmnSelection_WaitWasPlmnSearchCnf: ENTERED");

    /* 停止保护定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF);


    /* 获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
       若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
       若相同则在当前模式发起搜网，否则挂起当前网络 */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /*  获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }


    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器 */
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitGasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_GAS_PLMN_SEARCH_CNF);
    
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /* 通知AS停止指定搜网 */
    NAS_MMC_SndAsPlmnSrchStopReq(UEPS_PID_GAS);

    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU                              *pstSrchCnfMsg = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;

    pstSrchCnfMsg = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 停止保护定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 更新当前模式的网络覆盖信息 */
    NAS_MMC_UpdateGUCoverageFlg_PlmnSelection(pstSrchCnfMsg);

    if (RRC_PLMN_SEARCH_RLT_SPEC == pstSrchCnfMsg->ulSearchRlt)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /* 根据不同的搜网模式，迁移到不同的L2状态，启动保护定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_SYSINFO_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_SYS_INFO, TI_NAS_MMC_WAIT_WAS_SYS_INFO_LEN);

        return VOS_TRUE;
    }


    /* 如果当前为NO RF，则直接退状态机 */
    if (RRC_PLMN_SEARCH_RLT_SPEC_NO_RF == pstSrchCnfMsg->ulSearchRlt)
    {
        NAS_MMC_ProcPlmnSearchNoRfFail_PlmnSelection();
        
        return VOS_TRUE;
    }

    /* 设置当前接入技术进行了全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA, VOS_TRUE);

    /* 将搜网消息中携带的网络信息转换为内部消息中网络信息 */
    NAS_MMC_ConvertRrcPlmnList2SearchedPlmnListInfo(NAS_MML_NET_RAT_TYPE_WCDMA,
                                                     &pstSrchCnfMsg->PlmnIdList,
                                                     &stPlmnSearchInfo);

    /* 根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);

    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_WCDMA == stDestPlmn.enRat)
        {
            /* 向W接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向W接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;

}
VOS_UINT32 NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnSelection_WaitGasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU                              *pstSrchCnfMsg = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;

    pstSrchCnfMsg = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 停止保护定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_GAS_PLMN_SEARCH_CNF);

    /* 设置当前模式是否进行全频搜网，是否存在网络覆盖 */
    NAS_MMC_UpdateGUCoverageFlg_PlmnSelection(pstSrchCnfMsg);

    if (RRC_PLMN_SEARCH_RLT_SPEC == pstSrchCnfMsg->ulSearchRlt)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /* 根据不同的搜网模式，迁移到不同的L2状态，启动保护定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_SYSINFO_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_SYS_INFO, TI_NAS_MMC_WAIT_GAS_SYS_INFO_LEN);

        return VOS_TRUE;
    }


    /* 如果当前为NO RF，则直接退状态机 */
    if (RRC_PLMN_SEARCH_RLT_SPEC_NO_RF == pstSrchCnfMsg->ulSearchRlt)
    {
        NAS_MMC_ProcPlmnSearchNoRfFail_PlmnSelection();
        
        return VOS_TRUE;
    }

    /* 更新当前接入技术进行了全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM, VOS_TRUE);

    /* 将搜网消息中携带的网络信息转换为内部消息中网络信息 */
    NAS_MMC_ConvertRrcPlmnList2SearchedPlmnListInfo(NAS_MML_NET_RAT_TYPE_GSM,
                                                     &pstSrchCnfMsg->PlmnIdList,
                                                     &stPlmnSearchInfo);

    /* 根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);

    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_GSM == stDestPlmn.enRat)
        {
            /* 向G接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向G接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;

}

VOS_UINT32 NAS_MMC_RcvTiWaitWasPlmnSearchCnfExpired_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{   
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;


    PS_MEM_SET(&stPlmnSearchInfo, 0x00, sizeof(stPlmnSearchInfo));


    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitWasPlmnSearchCnfExpired_PlmnSelection_WaitWasPlmnSearchCnf: Timer Expired");

    /* 设置当前网络已经进行全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA, VOS_TRUE);

    /* 设置当前网络无覆盖 */
    NAS_MMC_SetRatCoverage_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA, NAS_MMC_COVERAGE_TYPE_NONE);

    /* 设置当前接入技术没有搜索到任何网络 */
    stPlmnSearchInfo.ulHighPlmnNum = 0x0;
    stPlmnSearchInfo.ulLowPlmnNum  = 0x0;
    stPlmnSearchInfo.enRatType     = NAS_MML_NET_RAT_TYPE_WCDMA;

    /* 更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);
    
    /* 通知AS停止指定搜网 */
    NAS_MMC_SndAsPlmnSrchStopReq(WUEPS_PID_WRR);

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvTiWaitGasPlmnSearchCnfExpired_PlmnSelection_WaitGasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;


    PS_MEM_SET(&stPlmnSearchInfo, 0x00, sizeof(stPlmnSearchInfo));


    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitGasPlmnSearchCnfExpired_PlmnSelection_WaitGasPlmnSearchCnf: Timer Expired");

    /* 设置当前网络已经进行全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM, VOS_TRUE);

    /* 设置当前网络无覆盖 */
    NAS_MMC_SetRatCoverage_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM, NAS_MMC_COVERAGE_TYPE_NONE);

    /* 设置当前接入技术没有搜索到任何网络 */
    stPlmnSearchInfo.ulHighPlmnNum = 0x0;
    stPlmnSearchInfo.ulLowPlmnNum  = 0x0;
    stPlmnSearchInfo.enRatType     = NAS_MML_NET_RAT_TYPE_GSM;

    /* 根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);
    
    NAS_MMC_SndAsPlmnSrchStopReq(UEPS_PID_GAS);

    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitAsSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录需要终止当前状态机标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitWasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_CNF_ST                *pstSuspendCnf = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurRat;

    pstSuspendCnf = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    enCurRat      = NAS_MML_GetCurrNetRatType();

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF);

    if (MMC_AS_SUSPEND_FAILURE == pstSuspendCnf->ucResult)
    {
        /* 不可恢复错误,调用底软接口RESET */

        if ( (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
          && (NAS_MML_NET_RAT_TYPE_WCDMA == enCurRat) )
        {
            NAS_MML_SoftReBoot();
        }
        else
        {
            NAS_MML_SoftReBoot();
        }

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 获取需要搜索的网络及其接入技术 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    else
    {
        /* Error级别打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitWasSuspendCnf: Get next search plmn fail.");

        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitGasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_CNF_ST                *pstSuspendCnf = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

    pstSuspendCnf = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF);

    if (MMC_AS_SUSPEND_FAILURE == pstSuspendCnf->ucResult)
    {
        /* 不可恢复错误,调用底软接口RESET */

        NAS_MML_SoftReBoot();

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 发送搜网结果,原因为退出状态机 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }


    /* 获取需要搜索的网络及其接入技术 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    else
    {
        /* Error级别打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitGasSuspendCnf: Get next search plmn fail.");

        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvTiWaitAsSuspendCnfExpired_PlmnSelection_WaitAsSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurRat;

    enCurRat      = NAS_MML_GetCurrNetRatType();

    /* 不可恢复故障，复位 */

    if ( (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
      && (NAS_MML_NET_RAT_TYPE_WCDMA == enCurRat) )
    {
        NAS_MML_SoftReBoot();
    }
    else if ( NAS_MML_NET_RAT_TYPE_GSM == enCurRat )
    {
        NAS_MML_SoftReBoot();
    }
    else
    {
        NAS_MML_SoftReBoot();
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvRrMmPlmnSearchStopCnf_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{   
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    NAS_MMC_StopTimer( TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF );

    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_WCDMA == stDestPlmn.enRat)
        {
            /* 向W接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向W接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }
    
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvRrMmRelInd_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 无需判断原因值 */

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvRrMmRelInd_PlmnSelection_WaitWasPlmnStopCnf: ENTERED");

    /* 停止保护定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF);

    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvRrMmPlmnSearchStopCnf_PlmnSelection_WaitGasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    NAS_MMC_StopTimer( TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF );

    /* 有打断时，退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_GSM == stDestPlmn.enRat)
        {
            /* 向G接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向G接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }


    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvTiWaitWasStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{   
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitWasStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnf: Timer Expired");

    /* 有打断时，退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }
    
    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_WCDMA == stDestPlmn.enRat)
        {
            /* 向W接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向W接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvTiWaitGasStopCnfExpired_PlmnSelection_WaitGasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{   
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitGasStopCnfExpired_PlmnSelection_WaitGasPlmnStopCnf: Timer Expired");

    /* 有打断时，退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_GSM == stDestPlmn.enRat)
        {
            /* 向G接入层发送搜网请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向G接入层发送挂起请求，迁移到相应的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_GSM);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;
}

/************************ 注册部分*************************************/

VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitWasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitGasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvSysInfoInd_PlmnSelection_WaitWasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

    NAS_MMC_SPEC_PLMN_SEARCH_STATE_ENUM_UINT8                enSpecPlmnSearchState;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();

#endif

    /* 停止等待系统消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_SYS_INFO);

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg,enLCapabilityStatus);
#endif

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_WasSysInfo(pstMsg);

    /* 驻留小区变化或重新搜网时，上报系统消息 */
    enSpecPlmnSearchState = NAS_MMC_GetSpecPlmnSearchState();

    if ((VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
     || (NAS_MMC_SPEC_PLMN_SEARCH_RUNNING == enSpecPlmnSearchState))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();

    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_STOP);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_WCDMA);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 更新当前等待CSPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 更新CSPS的AdditionalAction */
    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

    /* 更新CSPS的RegCause */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }

    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    if ( VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType) )
    {
        /* 转发系统消息通知 给 MM/GMM */
        NAS_MMC_SndMmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);

        NAS_MMC_SndGmmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);
    }
    else
    {
        /* 转发系统消息通知 给 MM/GMM,并将当前系统消息的 Forbidden 类型携带过去 */
        NAS_MMC_SndMmWasSysInfoInd(NAS_MML_GetLaiForbType(&(pstCurCampInfo->stLai)),
                                   VOS_FALSE,
                                   pstMsg);

        NAS_MMC_SndGmmWasSysInfoInd(NAS_MML_GetLaiForbType(&(pstCurCampInfo->stLai)),
                                   VOS_FALSE,
                                   pstMsg);        
    }

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmWcdmaSysInfoInd(pstMsg);
#endif

    /* 收到系统消息后迁移到等待CS+PS域的注册结果并启动定时器 */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_CSPS_REG_IND);
    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND, TI_NAS_MMC_WAIT_CSPS_REG_IND_LEN);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MMC_RcvSysInfoInd_PlmnSelection_WaitGasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

    NAS_MMC_SPEC_PLMN_SEARCH_STATE_ENUM_UINT8                enSpecPlmnSearchState;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;


    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();

#endif

    /* 如果收到GAS的系统消息，且目前Utran被Enable，则重新Disable Utran */
    NAS_MMC_RcvGasSysInfoSetUtranCapabilityStatus();

    /* 停止等待系统消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_GAS_SYS_INFO);

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg, enLCapabilityStatus);
#endif

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_GasSysInfo(pstMsg);

    /* 驻留小区变化或重新搜网时，上报系统消息 */
    enSpecPlmnSearchState = NAS_MMC_GetSpecPlmnSearchState();

    if ((VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
     || (NAS_MMC_SPEC_PLMN_SEARCH_RUNNING == enSpecPlmnSearchState))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();
    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_STOP);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_GSM);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 更新当前等待CSPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 更新CSPS的AdditionalAction */
    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

    /* 更新CSPS的RegCause */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();


    /* 在用户指定搜网时，不需要携带FORB类型 */
    if ( VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType) )
    {
        /* 转发系统消息通知 给 MM/GMM */
        NAS_MMC_SndMmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);

        NAS_MMC_SndGmmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);
    }
    else
    {
        /* 转发系统消息通知 给 MM/GMM,并将当前系统消息的 Forbidden 类型携带过去 */
        NAS_MMC_SndMmGsmSysInfoInd(NAS_MML_GetLaiForbType(&(pstCurCampInfo->stLai)),
                                   VOS_FALSE,
                                   pstMsg);

        NAS_MMC_SndGmmGsmSysInfoInd(NAS_MML_GetLaiForbType(&(pstCurCampInfo->stLai)),
                                   VOS_FALSE,
                                   pstMsg);
        
    }

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmGsmSysInfoInd(pstMsg);
#endif

    /* 收到系统消息后迁移到等待CS+PS域的注册结果并启动定时器 */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_CSPS_REG_IND);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND, TI_NAS_MMC_WAIT_CSPS_REG_IND_LEN);


    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvTiWaitSysInfoExpired_PlmnSelection_WaitWasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitSysInfoExpired_PlmnSelection_WaitWasSysInfoInd: Timer Expired");

    /*如果当前需要退出状态机 */
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

       /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络
     */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /*  获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器 */
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;
}



VOS_UINT32  NAS_MMC_RcvTiWaitSysInfoExpired_PlmnSelection_WaitGasSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitSysInfoExpired_PlmnSelection_WaitGasSysInfoInd: Timer Expired");

    /*如果当前需要退出状态机*/
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络
     */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /*  获取下个网络失败，则进行出服务区处理*/
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg = VOS_NULL_PTR;
    VOS_UINT8                           ucWaitRegFlag;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;


    /* 若打断类型是用户请求的关机，则直接退出等注册结果状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    ucWaitRegFlag = NAS_MMC_GetWaitRegRsltFlag_PlmnSelection();

    /* 当前没有链接可以直接退出；主要指cs/ps注册被拒，如cause17,需要继续等待注册结果的情况 */
    if (( VOS_FALSE == NAS_MML_IsRrcConnExist())
     || (NAS_MMC_WAIT_REG_RESULT_IND_NULL == ucWaitRegFlag))
    {

        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 设置Abort标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /* 如果需要立即退出 */
    if (NAS_MMC_ABORT_FSM_IMMEDIATELY == pstAbortMsg->enAbortFsmType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);
        NAS_MMC_SndRelReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvGmmPsRegResultInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    GMMMMC_PS_REG_RESULT_IND_STRU      *pstPsRegRslt = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    enCurrentFsmId      = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType  = NAS_MMC_GetCurrFsmEventType();

    pstPsRegRslt        = (GMMMMC_PS_REG_RESULT_IND_STRU*)pstMsg;

    /* 针对CCO失败回退过程中收到的异系统的注册结果，不需要处理 */
    if (pstPsRegRslt->enNetType != NAS_MML_GetCurrNetRatType())
    {
        return VOS_TRUE;
    }

    /* 清除等待PS注册结果mask */
    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 根据注册结果做对应处理: 设置服务状态，记录Cause值，设置下一步动作类型，通知对应结果 */
    NAS_MMC_ProcPsRegRslt_PlmnSelection(pstPsRegRslt);

    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /* 更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机 */
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

            /* 用户指定搜完成后，需要将MML全局变量中的禁止漫游LA信息通知接入层*/
            NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 向上层状态机发送搜网结果 ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
        {
            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif
        }

        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmCsRegResultInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMMMC_CS_REG_RESULT_IND_STRU       *pstCsRegRslt = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    pstCsRegRslt = (MMMMC_CS_REG_RESULT_IND_STRU*)pstMsg;

    /* 针对CCO失败回退过程中收到的异系统的注册结果，不需要处理 */
    if (pstCsRegRslt->enNetType != NAS_MML_GetCurrNetRatType())
    {
        return VOS_TRUE;
    }

    /* 清除等待CS注册结果mask */
    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);

    /* 根据注册结果做对应处理: 设置服务状态，记录Cause值，设置下一步动作类型，通知对应结果 */
    NAS_MMC_ProcCsRegRslt_PlmnSelection(pstCsRegRslt);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

            /* 用户指定搜完成后，需要将MML全局变量中的禁止漫游LA信息通知接入层*/
            NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /*如果当前需要退出状态机*/
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvTiWaitCsPsRegRsltIndExpired_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enPsAdditionalAction;
    NAS_MMC_FSM_ID_ENUM_UINT32           enCurrentFsmId;
    VOS_UINT32                           ulCurrentEventType;

    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitCsPsRegRsltIndExpired_PlmnSelection_WaitCsPsRegRsltInd: Timer Expired");

    /* 清除当前等待注册的标识 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

        /* 更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机 */
        NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif

    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 向上层状态机发送搜网结果 ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
    enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();

    /* 等待CS PS的注册结果时，有可能一个域发起注册后进行了呼叫以及紧急呼叫等业务
      (比如G下进行CS域进行呼叫，PS域无法发起注册),导致MMC一致等不到另外一个域的
      注册结果，此时MMC直接通过additional触发搜网不合理，MMC此时根据当前收到的注
      册结果更新的additional值来决定是否成功退出搜网状态机或者失败退出搜网状态机，
      由L1状态机根据搜网状态机的退出结果决定是否启动周期性选网定时器还是继续等待
      业务结束以及CSPS的注册结果 */

    enAdditionalAction = NAS_MMC_GetPrioAddtionalAction(enCsAdditionalAction, enPsAdditionalAction);

    /* 附加操作为驻留当前网络 */
    if ((NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_CELL_SELECTION == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_ACCESS_BARRED == enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);
    }
    else
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvGmmNetworkDetachInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    MMCGMM_NETWORK_DETACH_IND_STRU     *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;
    NAS_MMC_FSM_ID_ENUM_UINT32           enCurrentFsmId;
    VOS_UINT32                           ulCurrentEventType;

    pstDetachMsg = (MMCGMM_NETWORK_DETACH_IND_STRU*)pstMsg;

    /* 根据当前的Detach类型， 做如下处理 */
    enAdditionalAction = NAS_MMC_ProcGmmNetworkDetachInd(pstDetachMsg);

    /* AdditionalAction有效，更新PS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 记录Ps 的注册结果 */
    NAS_MMC_SetPsRegCause_PlmnSelection((NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstDetachMsg->ulDetachCause);


    /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
       当用户指定搜网时，则需要把禁止LA带给接入层 */
    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
      && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstDetachMsg->ulDetachCause) )
    {
        NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                               NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
    }

    /* 需要继续注册时，不需要执行后续的动作 */
    if (NAS_MMC_GMM_NT_DETACH_REATTACH == pstDetachMsg->ulDetachType)
    {
        return VOS_TRUE;
    }

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

#if   (FEATURE_ON == FEATURE_LTE)
    /* 给LMM发送detach结果消息 */
    NAS_MMC_SndLmmMtDetachIndActionResultReq(pstDetachMsg->ulDetachType,
        (NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstDetachMsg->ulDetachCause);
#endif

    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /* 更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机 */
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

            /* 用户指定搜完成后，需要将MML全局变量中的禁止漫游LA信息通知接入层*/
            NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复上层状态机搜网结果 ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvGmmServiceRequestResultInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    GMMMMC_SERVICE_REQUEST_RESULT_IND_STRU                 *pstServiceRsltInd = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU                        stActionRslt;
#endif
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

    pstServiceRsltInd = (GMMMMC_SERVICE_REQUEST_RESULT_IND_STRU*)pstMsg;

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 根据Service Reqest Result 更新做不同处理 */
    if (GMM_MMC_ACTION_RESULT_SUCCESS == pstServiceRsltInd->enActionResult)
    {
        /* 更新服务状态 */
        NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_NULL);

        /* 更新PS的AdditionalAction */
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON);

#if (FEATURE_ON == FEATURE_LTE)
        /* 给LMM发送Service requestResult结果消息处理*/
        stActionRslt.enProcType     = NAS_MML_PROC_SER;
        stActionRslt.enRegRst       = NAS_MML_REG_RESULT_SUCCESS;
        stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enCnCause      = NAS_MML_REG_FAIL_CAUSE_NULL;
        stActionRslt.ulAttemptCount = 0;

        NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);

#endif

    }
    else
    {
        enAdditionalAction = NAS_MMC_ProcPsServiceRegFail(pstServiceRsltInd);

        NAS_MMC_SetPsRegCause_PlmnSelection(pstServiceRsltInd->enRegFailCause);


        /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
           当用户指定搜网时，则需要把禁止LA带给接入层 */
        if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
          && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstServiceRsltInd->enRegFailCause) )
        {
            NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                                   NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
        }

        /* AdditionalAction有效，更新PS的下一步的动作类型 */
        if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
        {
            NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
        }

#if (FEATURE_ON == FEATURE_LTE)
        /* 给LMM发送Service requestResult结果消息处理 */
        stActionRslt.enProcType     = NAS_MML_PROC_SER;
        stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
        stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enCnCause      = pstServiceRsltInd->enRegFailCause;
        stActionRslt.ulAttemptCount = 0;

        NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
#endif

    }

    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause, enPsRegCause))
        {
            /* 更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机 */
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

            /* 用户指定搜完成后，需要将MML全局变量中的禁止漫游LA信息通知接入层*/
            NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }


    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvCmServiceRejectInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    MMCMM_CM_SERVICE_REJECT_IND_STRU   *pstRcvMsg = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU    stActionRslt;
#endif
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    pstRcvMsg = (MMCMM_CM_SERVICE_REJECT_IND_STRU*)pstMsg;

#if (FEATURE_ON == FEATURE_LTE)
    stActionRslt.enProcType     = NAS_MML_PROC_CM_SER;
    stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
    stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
    stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
    stActionRslt.enCnCause      = (NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstRcvMsg->ulCause;
    stActionRslt.ulAttemptCount = 0;

    NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);

#endif

    /*CmService Rej只在 #4和#6被拒时，会发送给MMC */
    if (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME == pstRcvMsg->ulCause)
    {
        /* 清除CS域的等待标志 */
        NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);

        /* 设置新的enCsAdditioalAction为Anycell */
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL);
    }


    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

            /* 用户指定搜完成后，需要将MML全局变量中的禁止漫游LA信息通知接入层*/
            NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }


    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvWasSuspendInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_IND_ST                *pstSuspendMsg = VOS_NULL_PTR;

    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat;

    pstSuspendMsg = (RRMM_SUSPEND_IND_ST*)pstMsg;

    enRat         = NAS_MML_GetCurrNetRatType();

    /* 如果为OOS或者参数不正确, 则不允许进行异系统重选 */
    if ((MMC_SUSPEND_CAUSE_PLMNSEARCH == pstSuspendMsg->ucSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= pstSuspendMsg->ucSuspendCause))
    {
        NAS_MMC_SndAsSuspendRsp(RRC_NAS_SUSPEND_FAILURE, WUEPS_PID_WRR);
    }
    else
    {
        NAS_MMC_SetInterSysSuspendRat_PlmnSelection(enRat);
        
        NAS_MMC_LoadInterSysFsm_PlmnSelection(pstSuspendMsg->ucSuspendCause);
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvGasSuspendInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    RRMM_SUSPEND_IND_ST                *pstSuspendMsg = VOS_NULL_PTR;

    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat;

    pstSuspendMsg = (RRMM_SUSPEND_IND_ST*)pstMsg;

    enRat         = NAS_MML_GetCurrNetRatType();

    /* 如果为OOS或者参数不正确, 则不允许进行异系统重选 */
    if ((MMC_SUSPEND_CAUSE_PLMNSEARCH == pstSuspendMsg->ucSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= pstSuspendMsg->ucSuspendCause))
    {
        NAS_MMC_SndAsSuspendRsp(RRC_NAS_SUSPEND_FAILURE, UEPS_PID_GAS);
    }
    else
    {
        NAS_MMC_SetInterSysSuspendRat_PlmnSelection(enRat);
        
        NAS_MMC_LoadInterSysFsm_PlmnSelection(pstSuspendMsg->ucSuspendCause);
    }


    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvSuspendFailCauseArealost_WaitCsPsRegRsltInd(VOS_VOID
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurPlmn;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 停止等待注册结果消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

    /* 获取高优先级网络列表 */
    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);
    
    /* 获取下个网络失败 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
        return VOS_TRUE;
    }
    
    /* 获取当前驻留的网络信息 */
    pstCurCampInfo     = NAS_MML_GetCurrCampPlmnInfo();
    stCurPlmn.stPlmnId = stDestPlmn.stPlmnId;
    stCurPlmn.enRat    = pstCurCampInfo->enNetRatType;
    
    /* 更新当前网络的搜索状态为已注册过，在此轮高优先级搜网中，不会再搜此网络 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                            NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                            pstPlmnSelectionListInfo);
    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络
     */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /* 获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    /* 不需要Suspend */
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvSuspendRsltHoSucc_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CONN_STATUS_INFO_STRU                          *pstConnStatus       = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType; 
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         ucInterSysSuspendRat;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         ucCurrRat; 

    enCurrentFsmId       = NAS_MMC_GetCurrFsmId();
    pstConnStatus        = NAS_MML_GetConnStatus();
    ucInterSysSuspendRat = NAS_MMC_GetInterSysSuspendRat_PlmnSelection();
    ucCurrRat            = NAS_MML_GetCurrNetRatType();


#if (FEATURE_LTE == FEATURE_ON)

    /* 迁移到不同的接入技术时，需要迁移到不同的处理注册结果状态 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        /* 停止当前等待CS/PS注册结果的状态 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

#endif

    /* 等注册结果时候，如果当前有CS业务链接时候发生了异系统切换，则退状态机到L1 */
    if ( (VOS_TRUE  == pstConnStatus->ucCsServiceConnStatusFlg)
      && (ucCurrRat != ucInterSysSuspendRat) )
    {
        /* 停止当前等待CS/PS注册结果的状态 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);
        
        /* 当前用户发起的是指定搜网,则回复注册结果成功 */
        ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
        
        if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
        {
            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

            /* 更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机 */
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif
        }

        enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
        enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();

        /* 等待CS PS的注册结果时，有可能一个域发起注册后进行了呼叫以及紧急呼叫等业务,
           发生了G<->W之间的异系统的切换(比如G下进行CS域进行呼叫，PS域无法发起注册),
           电话结束后，在先前发起电话的网络状态被更改为REGISTERED,因而在选网状态机不会再
           去选择先前尝试的网络。MMC此时根据当前收到的注册结果更新的additional值来决定
           是否成功退出搜网状态机或者失败退出搜网状态机,由L1状态机根据搜网状态机的退出
           结果决定是否启动周期性选网定时器还是继续等待业务结束以及CSPS的注册结果 */

        enAdditionalAction = NAS_MMC_GetPrioAddtionalAction(enCsAdditionalAction, enPsAdditionalAction);

        /* 附加操作为驻留当前网络 */
        if ((NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON  == enAdditionalAction)
         || (NAS_MMC_ADDITIONAL_ACTION_CELL_SELECTION  == enAdditionalAction)
         || (NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON == enAdditionalAction)
         || (NAS_MMC_ADDITIONAL_ACTION_ACCESS_BARRED   == enAdditionalAction))
        {
            /* 回复状态机退出成功 */
            NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);
        }
        else
        {
            NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
        }

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE; 
    }

    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvMmcMmcSuspendRslt_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMCMMC_SUSPEND_RSLT_STRU       *pstSuspendRsltMsg = VOS_NULL_PTR;

    pstSuspendRsltMsg = (NAS_MMCMMC_SUSPEND_RSLT_STRU*)pstMsg;

    
    /* HO成功的处理 */
    if ( (MMC_SUSPEND_CAUSE_HANDOVER == pstSuspendRsltMsg->enSuspendCause)
      && (NAS_MMC_SUSPEND_SUCCESS    == pstSuspendRsltMsg->enRslt) )
    {
        NAS_MMC_RcvSuspendRsltHoSucc_PlmnSelection_WaitCsPsRegRsltInd(ulEventType, pstMsg);

        return VOS_TRUE; 
    }
    
    /* 如果重选结果为失败,原因为出服务区,则当作注册过程中出服务区处理 */
    if ((NAS_MMC_SUSPEND_FAIL == pstSuspendRsltMsg->enRslt)
     && (NAS_MMC_SUSPEND_FAIL_CAUSE_AREA_LOST == pstSuspendRsltMsg->enFailCause))
    {
        /*如果当前需要退出状态机*/
        if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
        {
            NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);
            NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
            
            /* 回复状态机退出成功 */
            NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

            /* 直接退出当前状态机 */
            NAS_MMC_FSM_QuitFsmL2();

            return VOS_TRUE;
        }
        else
        {
            return NAS_MMC_RcvSuspendFailCauseArealost_WaitCsPsRegRsltInd();
        }
    }
    
#if (FEATURE_LTE == FEATURE_ON)

    /* 迁移到不同的接入技术时，需要迁移到不同的处理注册结果状态 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        /* 停止当前等待CS/PS注册结果的状态 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);


        /* HO成功的处理封装到NAS_MMC_RcvSuspendRsltHoSucc_PlmnSelection_WaitCsPsRegRsltInd */

        /* 更新当前等待EPS的注册结果 */
        NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

#if (FEATURE_ON == FEATURE_IMS)    
        /* 增加等待IMS是否可用结果FLG,MMA保证收到PS注册结果上报IMS是否可用指示到MMC */    
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);
#endif    

        /* 更新CS的注册Cause和AdditionalAction */
        NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT);
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

        /* 迁移到等待注册结果子状态PLMN_SRCH:NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_REG_IND, TI_NAS_MMC_WAIT_EPS_REG_IND_LEN);

        return VOS_TRUE;
    }

#endif

     /* 与Intersys状态机的配合待后续补充
     switch (pstSuspendRsltMsg->enRslt)
     {
         case NAS_MMC_SUSPEND_SUCCESS:
         break;

         case NAS_MMC_SUSPEND_FAIL :
         case NAS_MMC_SUSPEND_QUIT :

         break;

         case NAS_MMC_SUSPEND_AREA_LOST :
         break;

         default:

         break;
     }
     */


    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvAreaLostInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 停止等待注册结果消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);


    /* 获取当前驻留的网络信息 */
    pstCurCampInfo     = NAS_MML_GetCurrCampPlmnInfo();
    stCurPlmn.stPlmnId = pstCurCampInfo->stLai.stPlmnId;
    stCurPlmn.enRat    = pstCurCampInfo->enNetRatType;

    NAS_MMC_SaveLastCampedPlmnid(NAS_MML_GetCurrCampPlmnInfo()->stLai.stPlmnId.ulMcc,
                           NAS_MML_GetCurrCampPlmnInfo()->stLai.stPlmnId.ulMnc);

    /* 获取高优先级网络列表 */
    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 更新当前网络的搜索状态为已注册过，在此轮高优先级搜网中，不会再搜此网络 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                            NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                            pstPlmnSelectionListInfo);

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*如果当前需要退出状态机*/
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络
     */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /* 获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    /* 不需要Suspend */
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvWasSysInfoInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg,enLCapabilityStatus);
#endif

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_WasSysInfo(pstMsg);

    /* 驻留小区变化时，上报系统消息 */
    if (VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();

    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_WCDMA);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 更新当前等待CSPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 更新CSPS的RegCause */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);



    /* 转发系统消息通知 给 MM/GMM,并将当前系统消息的 Forbidden 类型携带过去 */

    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);
        
        NAS_MMC_SndGmmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);
    }
    else
    {
        NAS_MMC_SndMmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(),
                                   VOS_FALSE,
                                   pstMsg);
        
        NAS_MMC_SndGmmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(),
                                   VOS_FALSE,
                                   pstMsg);
    }


#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmWcdmaSysInfoInd(pstMsg);
#endif

    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvGasSysInfoInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();
#endif

    /* 如果收到GAS的系统消息，且目前Utran被Enable，则重新Disable Utran */
    NAS_MMC_RcvGasSysInfoSetUtranCapabilityStatus();

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_GasSysInfo(pstMsg);

    /* 驻留小区变化时，上报系统消息 */
    if (VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();

    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg, enLCapabilityStatus);
#endif

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_GSM);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 更新当前等待CSPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 更新CSPS的RegCause */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);


    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);
        
        NAS_MMC_SndGmmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenTypeForUserSpecPlmnSrch(),
                                   VOS_TRUE,
                                   pstMsg);        
    }
    else
    {
        NAS_MMC_SndMmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(),
                                   VOS_FALSE,
                                   pstMsg);
        
        NAS_MMC_SndGmmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(),
                                   VOS_FALSE,
                                   pstMsg);          
    }

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmGsmSysInfoInd(pstMsg);
#endif

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList   = VOS_NULL_PTR;

    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    /* 保存手动搜网信息 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                       pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表, 目前只有当前注册网络与指定搜索网络相同时，才会走到这一步 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause, enPsRegCause))
    {
        return VOS_TRUE;
    }

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmAbortInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMMMC_ABORT_IND_STRU               *pstMmAbortInd   = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU    stGuActionRslt;
#endif

    pstMmAbortInd = (MMMMC_ABORT_IND_STRU*)pstMsg;

    /* 如果原因值不为#6，且不为#17，直接返回 */
    if ((NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME != (pstMmAbortInd->enAbortCause))
     && (NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE != (pstMmAbortInd->enAbortCause)))
    {
        return VOS_TRUE;
    }

    /* 如果原因值#6，设置CS Addition为Anycell驻留 */
    if (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME == pstMmAbortInd->enAbortCause)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL);
    }

#if (FEATURE_ON == FEATURE_LTE)

    /* 给LMM发送ID_MMC_LMM_ACTION_RESULT_REQ */
    /* 构造ActionRslt消息，结果为CN_Rej,类型填为新增NAS_MML_PROC_MM_ABORT类型
       请求和结果域都是CS，原因值为Abort带的原因值 */
    stGuActionRslt.enProcType     = NAS_MML_PROC_MM_ABORT;
    stGuActionRslt.enRegRst       = NAS_MML_REG_RESULT_CN_REJ;
    stGuActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
    stGuActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
    stGuActionRslt.enCnCause      = pstMmAbortInd->enAbortCause;
    stGuActionRslt.ulAttemptCount = 0x0;

    NAS_MMC_SndLmmRegActionResultReq(&stGuActionRslt);
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmaImsVoiceCapInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* GU下收到IMS VOICE是否可用目前暂不处理
       对于异系统过程中收到的IMS VOICE是否可用的场景由于可能是异系统导致的IMS VOICE
       不可用,此时用来决定disable L模不准确,暂不考虑该场景 */

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg = VOS_NULL_PTR;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;

    /* 若打断类型是用户请求的关机，则直接退出等链接释放结果状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 记录 Abort标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvGmmSignalingStatusInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvGmmNetworkDetachInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMCGMM_NETWORK_DETACH_IND_STRU     *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;

    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

    pstDetachMsg = (MMCGMM_NETWORK_DETACH_IND_STRU *)pstMsg;

    enAdditionalAction = NAS_MMC_ProcGmmNetworkDetachInd(pstDetachMsg);

    /* AdditionalAction有效，更新PS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 记录Ps 的注册结果 */
    NAS_MMC_SetPsRegCause_PlmnSelection((NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstDetachMsg->ulDetachCause);


    /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
       当用户指定搜网时，则需要把禁止LA带给接入层 */
    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
   ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
      && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstDetachMsg->ulDetachCause) )
    {
        NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                               NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
    }


#if   (FEATURE_ON == FEATURE_LTE)
    /* 给LMM发送detach结果消息 */
    NAS_MMC_SndLmmMtDetachIndActionResultReq(pstDetachMsg->ulDetachType,
        (NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstDetachMsg->ulDetachCause);
#endif

    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvGmmServiceRequestResultInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    GMMMMC_SERVICE_REQUEST_RESULT_IND_STRU                 *pstServiceRsltInd = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU                        stActionRslt;
#endif

    pstServiceRsltInd = (GMMMMC_SERVICE_REQUEST_RESULT_IND_STRU*)pstMsg;

    /* 根据Service Reqest Result 更新做不同处理 */
    if (GMM_MMC_ACTION_RESULT_SUCCESS == pstServiceRsltInd->enActionResult)
    {
        /* 更新服务状态 */
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;
        NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_NULL);

#if (FEATURE_ON == FEATURE_LTE)
        /* 给LMM发送Service requestResult结果消息处理 */
        stActionRslt.enProcType     = NAS_MML_PROC_SER;
        stActionRslt.enRegRst       = NAS_MML_REG_RESULT_SUCCESS;
        stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enCnCause      = NAS_MML_REG_FAIL_CAUSE_NULL;
        stActionRslt.ulAttemptCount = 0;

        NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
#endif


    }
    else
    {
        enAdditionalAction = NAS_MMC_ProcPsServiceRegFail(pstServiceRsltInd);

        NAS_MMC_SetPsRegCause_PlmnSelection(pstServiceRsltInd->enRegFailCause);


        /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
           当用户指定搜网时，则需要把禁止LA带给接入层 */
        enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
        if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
        {
            NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
        }
        ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
        if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
          && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstServiceRsltInd->enRegFailCause) )
        {
            NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                                   NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
        }

#if (FEATURE_ON == FEATURE_LTE)

        /* 给LMM发送Service requestResult结果消息处理 */
        stActionRslt.enProcType     = NAS_MML_PROC_SER;
        stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
        stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_PS;
        stActionRslt.enCnCause      = pstServiceRsltInd->enRegFailCause;
        stActionRslt.ulAttemptCount = 0;

            NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
#endif

    }

    /* AdditionalAction有效，更新PS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmConnInfoInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}



VOS_UINT32  NAS_MMC_RcvMmRrRelInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvRrMmRelInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_REL_IND_STRU                  *pstRelIndMsg = VOS_NULL_PTR;

    pstRelIndMsg = ( RRMM_REL_IND_STRU * )pstMsg;


    /* 清除连接存在标志 */
    if (RRC_RRC_CONN_STATUS_ABSENT == pstRelIndMsg->ulRrcConnStatus)
    {
        NAS_MML_SetRrcConnStatusFlg(VOS_FALSE);
    }

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvGmmTbfRelInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    /* 停止等待连接释放定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvWasSuspendInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    RRMM_SUSPEND_IND_ST                *pstSuspendMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRelRequestFlg;

    pstSuspendMsg = (RRMM_SUSPEND_IND_ST*)pstMsg;

    ulRelRequestFlg = NAS_MMC_GetRelRequestFlag_PlmnSelection();

    /* 如果是主动释放或者已经接收到Abort消息,则不允许进行异系统重选 */
    if ((VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
     || (VOS_TRUE == ulRelRequestFlg)
     || (MMC_SUSPEND_CAUSE_PLMNSEARCH == pstSuspendMsg->ucSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= pstSuspendMsg->ucSuspendCause))
    {
        NAS_MMC_SndAsSuspendRsp(RRC_NAS_SUSPEND_FAILURE, WUEPS_PID_WRR);
    }
    else
    {
        NAS_MMC_LoadInterSysFsm_PlmnSelection(pstSuspendMsg->ucSuspendCause);
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvGasSuspendInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    RRMM_SUSPEND_IND_ST                *pstSuspendMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRelRequestFlg;

    pstSuspendMsg = (RRMM_SUSPEND_IND_ST*)pstMsg;

    ulRelRequestFlg = NAS_MMC_GetRelRequestFlag_PlmnSelection();

    /* 如果是主动释放或者已经接收到Abort消息,则不允许进行异系统重选 */
    if ((VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
     || (VOS_TRUE == ulRelRequestFlg)
     || (MMC_SUSPEND_CAUSE_PLMNSEARCH == pstSuspendMsg->ucSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= pstSuspendMsg->ucSuspendCause))

    {
        NAS_MMC_SndAsSuspendRsp(RRC_NAS_SUSPEND_FAILURE, UEPS_PID_GAS);
    }
    else
    {
        NAS_MMC_LoadInterSysFsm_PlmnSelection(pstSuspendMsg->ucSuspendCause);
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSuspendRslt_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
#if (FEATURE_LTE == FEATURE_ON)

    NAS_MMCMMC_SUSPEND_RSLT_STRU       *pstSuspendRsltMsg = VOS_NULL_PTR;

    pstSuspendRsltMsg = (NAS_MMCMMC_SUSPEND_RSLT_STRU*)pstMsg;

     /* 迁移到不同的接入技术时，需要迁移到不同的处理注册结果状态 */

    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        /* 停止当前等待CS/PS连接释放的状态 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

        /* GU handover 到 L 成功后，可能很长时间无法收到系统消息，转到L1处理  */
        if ((NAS_MMC_SUSPEND_SUCCESS == pstSuspendRsltMsg->enRslt)
         && (MMC_SUSPEND_CAUSE_HANDOVER == pstSuspendRsltMsg->enSuspendCause))
        {
            /* 回复状态机退出成功 */
            NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

            /* 直接退出当前状态机 */
            NAS_MMC_FSM_QuitFsmL2();

            return VOS_TRUE;
        }

        /* 更新当前等待EPS的注册结果 */
        NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);
        
#if (FEATURE_ON == FEATURE_IMS)    
        /* 增加等待IMS是否可用结果FLG,MMA保证收到PS注册结果上报IMS是否可用指示到MMC */    
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);
#endif    

        /* 更新CS的注册Cause和AdditionalAction */
        NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT);
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

        /* 迁移到等待注册结果子状态PLMN_SRCH:NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_REG_IND, TI_NAS_MMC_WAIT_EPS_REG_IND_LEN);

        return VOS_TRUE;
    }

#endif

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    /* 在重选之前已经判断,此时不可能存在Abort或者主动释放,如果信令连接不存在可能发起搜网 */
    NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();

   /* 与Intersys状态机的配合待后续补充
    switch (pstSuspendRsltMsg->enRslt)
    {
        case NAS_MMC_SUSPEND_SUCCESS:
        break;

        case NAS_MMC_SUSPEND_FAIL :
        case NAS_MMC_SUSPEND_QUIT :

        break;

        case NAS_MMC_SUSPEND_AREA_LOST :
        break;

        default:

        break;
    }
    */




    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvWasSysInfoInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU         stOldCampInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;


    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();

#endif

    /* 如果是主动请求释放,由于需要立即进行搜网,系统消息不处理 */
    if (VOS_TRUE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_WasSysInfo(pstMsg);

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg,enLCapabilityStatus);
#endif

    /* 驻留小区变化时，上报系统消息 */
    if (VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();

    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_WCDMA);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /*转发系统消息通知 给 MM/GMM,并将当前系统消息的 Forbidden 类型携带过去 */
    NAS_MMC_SndMmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(), VOS_FALSE, pstMsg);
    NAS_MMC_SndGmmWasSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(), VOS_FALSE, pstMsg);

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmWcdmaSysInfoInd(pstMsg);
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvGasSysInfoInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurCsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldCsRestrictInfo;
    NAS_MML_ACCESS_RESTRICTION_STRU                         *pstCurPsRestrictInfo = VOS_NULL_PTR;
    NAS_MML_ACCESS_RESTRICTION_STRU                          stOldPsRestrictInfo;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;


    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();
#endif

    /* 如果收到GAS的系统消息，且目前Utran被Enable，则重新Disable Utran */
    NAS_MMC_RcvGasSysInfoSetUtranCapabilityStatus();

    /* 如果是主动请求释放,由于需要立即进行搜网,系统消息不处理 */
    if (VOS_TRUE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    pstCurCsRestrictInfo    = NAS_MML_GetCsAcRestrictionInfo();
    pstCurPsRestrictInfo    = NAS_MML_GetPsAcRestrictionInfo();

    PS_MEM_CPY(&stOldCsRestrictInfo, pstCurCsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));
    PS_MEM_CPY(&stOldPsRestrictInfo, pstCurPsRestrictInfo, sizeof(NAS_MML_ACCESS_RESTRICTION_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_GasSysInfo(pstMsg);

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_RcvGuSysInfoIndSetLteAbility(pstMsg, enLCapabilityStatus);
#endif

    /* 驻留小区变化时，上报系统消息 */
    if (VOS_TRUE == NAS_MML_IsCampPlmnInfoChanged(&stOldCampInfo, pstCurCampInfo))
    {
        /* 主动上报 */
        NAS_MMC_SndMmaSysInfo();

    }

    /* 驻留小区的接入受限信息变化时,通知MMA模块当前接入受限信息 */
    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldCsRestrictInfo, pstCurCsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_CS, pstCurCsRestrictInfo);
    }

    if (VOS_TRUE == NAS_MML_IsAcInfoChanged(&stOldPsRestrictInfo, pstCurPsRestrictInfo))
    {
        NAS_MMC_SndMmaAcInfoChangeInd(MMA_MMC_SRVDOMAIN_PS, pstCurPsRestrictInfo);
    }

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_GSM);
#endif

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /*转发系统消息通知 给 MM/GMM,并将当前系统消息的 Forbidden 类型携带过去 */
    NAS_MMC_SndMmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(), VOS_FALSE, pstMsg);
    NAS_MMC_SndGmmGsmSysInfoInd(NAS_MMC_GetCurrentLaiForbbidenType(), VOS_FALSE, pstMsg);

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_SndLmmGsmSysInfoInd(pstMsg);
#endif

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTiWaitRrcConnRelExpired_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulCsServiceStatusFlg;
    VOS_UINT32                          ulCsEmergencyServiceFlg;
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitRrcConnRelExpired_PlmnSelection_WaitRrcConnRelInd ENTERED");

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));


    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接存在, 而且是主动释放连接,则进行搜网动作 */
    if (VOS_TRUE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();

        return VOS_TRUE;
    }

    /*
    Connrel 被动释放，
    判断是否有业务，有，则退出 (正常服务，返回成功，受限服务，返回失败)
    否则，获取下一个网络成功，发主动释放，置标志位，失败则退出
    */
    ulCsServiceStatusFlg    = NAS_MML_GetCsServiceExistFlg();
    ulCsEmergencyServiceFlg = NAS_MML_GetCsEmergencyServiceFlg();
    if ( (VOS_TRUE == ulCsServiceStatusFlg)
      && (VOS_TRUE == ulCsEmergencyServiceFlg))
    {
        /* 回复状态机搜网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }
    else
    {
        /* 获取下个网络，如果成功则主动释放，否则退出 */
        if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
        {
            /* 根据当前不同的接入技术,发送释放请求,迁移到不同的等待连接释放的L2状态，并启动保护定时器 */
            NAS_MMC_SndRelReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
        }
        else
        {
            NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

            NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
        }
    }


    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList   = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;


    /* 保存手动搜网信息 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                       pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    enCsCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    /* 根据保存的CS/PS注册结果判断指定搜网处理是否已结束 */
    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsCause, enPsCause))
    {
        return VOS_TRUE;
    }

    /* 因为目前状态机更新为手动搜网流程,需要保存入口消息 , 目前应该用不到*/
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitRrcConnRelInd: ENTERED. ");
    return VOS_TRUE;
}




VOS_UINT32 NAS_MMC_RcvCmServiceRejectInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMCMM_CM_SERVICE_REJECT_IND_STRU   *pstRcvMsg = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU    stActionRslt;
#endif

    pstRcvMsg = (MMCMM_CM_SERVICE_REJECT_IND_STRU*)pstMsg;

#if (FEATURE_ON == FEATURE_LTE)
    stActionRslt.enProcType     = NAS_MML_PROC_CM_SER;
    stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
    stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
    stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
    stActionRslt.enCnCause      = (NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16)pstRcvMsg->ulCause;
    stActionRslt.ulAttemptCount = 0;

    NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);

#endif

    /* 对#6做特殊处理时 */
    if (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME == pstRcvMsg->ulCause)
    {
        /* 设置新的enCsAdditioalAction，*/
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL);

    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvGmmPsRegResultInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    GMMMMC_PS_REG_RESULT_IND_STRU      *pstPsRegRslt = VOS_NULL_PTR;

    pstPsRegRslt = (GMMMMC_PS_REG_RESULT_IND_STRU*)pstMsg;

    /* 针对CCO失败回退过程中收到的异系统的注册结果，不需要处理 */
    if (pstPsRegRslt->enNetType != NAS_MML_GetCurrNetRatType())
    {
        return VOS_TRUE;
    }

    /* 根据注册结果做对应处理: 设置服务状态，记录Cause值，设置下一步动作类型，通知对应结果 */
    NAS_MMC_ProcPsRegRslt_PlmnSelection(pstPsRegRslt);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmCsRegResultInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMMMC_CS_REG_RESULT_IND_STRU       *pstCsRegRslt;

    pstCsRegRslt = (MMMMC_CS_REG_RESULT_IND_STRU*)pstMsg;

    /* 针对CCO失败回退过程中收到的异系统的注册结果，不需要处理 */
    if (pstCsRegRslt->enNetType != NAS_MML_GetCurrNetRatType())
    {
        return VOS_TRUE;
    }

    /* 根据注册结果做对应处理: 设置服务状态，记录Cause值，设置下一步动作类型，通知对应结果 */
    NAS_MMC_ProcCsRegRslt_PlmnSelection(pstCsRegRslt);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmAbortInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMMMC_ABORT_IND_STRU               *pstMmAbortInd   = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU    stGuActionRslt;
#endif

    pstMmAbortInd = (MMMMC_ABORT_IND_STRU*)pstMsg;

    /* 如果原因值不为#6，且不为#17，直接返回 */
    if ((NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME != pstMmAbortInd->enAbortCause)
     && (NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE != pstMmAbortInd->enAbortCause))
    {
        return VOS_TRUE;
    }

    /* 如果原因值#6，设置CS Addition为Anycell驻留 */
    if (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME == pstMmAbortInd->enAbortCause)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL);
    }

#if (FEATURE_ON == FEATURE_LTE)

    /* 给LMM发送ID_MMC_LMM_ACTION_RESULT_REQ */
    /* 构造ActionRslt消息，结果为CN_Rej,类型填为新增NAS_MML_PROC_MM_ABORT类型
       请求和结果域都是CS，原因值为Abort带的原因值 */
    stGuActionRslt.enProcType     = NAS_MML_PROC_MM_ABORT;
    stGuActionRslt.enRegRst       = NAS_MML_REG_RESULT_CN_REJ;
    stGuActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
    stGuActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
    stGuActionRslt.enCnCause      = pstMmAbortInd->enAbortCause;
    stGuActionRslt.ulAttemptCount = 0x0;

    NAS_MMC_SndLmmRegActionResultReq(&stGuActionRslt);
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmaRegReq_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enNetRatType;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU                     *pstInterPlmnSrchMsg = VOS_NULL_PTR;
    MMA_MMC_REG_REQ_STRU                                   *pstRegReq = VOS_NULL_PTR;

    /* 停止保护定时器TI_NAS_MMC_WAIT_MMA_REG_REQ */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_MMA_REG_REQ);

    /* 如果有ABORT标志，直接退出 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    pstRegReq = (MMA_MMC_REG_REQ_STRU*)pstMsg;

    /* 保存NCELL信息 */
    if (VOS_TRUE == pstRegReq->bitOpCellInfo)
    {
        NAS_MMC_SaveRegReqNCellInfo(&(pstRegReq->stCellInfo));
    }

    enPlmnSearchScene = NAS_MMC_GetPlmnSearchSceneMmaRegReq(pstRegReq);

    /* 将状态机入口消息替换为构造的内部搜网消息，后续搜到网络后可以直接发起注册 */
    pstInterPlmnSrchMsg = (NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMC, sizeof(NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU));
    if (VOS_NULL_PTR == pstInterPlmnSrchMsg)
    {
        /* 异常打印 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvMmaRegReq_PlmnSelection_WaitMmaRegReq Alloc mem fail");

        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_BulidInterPlmnSearchReqMsg(enPlmnSearchScene, VOS_NULL_PTR, 0, pstInterPlmnSrchMsg);
    NAS_MMC_SaveCurEntryMsg(NAS_BuildEventType(WUEPS_PID_MMC, MMCMMC_INTER_PLMN_SEARCH_REQ),
                            (struct MsgCB*)pstInterPlmnSrchMsg);
    PS_MEM_FREE(WUEPS_PID_MMC, pstInterPlmnSrchMsg);

    /* 根据enPlmnSearchScene重新刷新选网列表，删除不满足优先级要求的网络 */
    NAS_MMC_RefreshPlmnSelectionListRcvMmaRegReq_PlmnSelection(enPlmnSearchScene);

    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 获取当前接入技术 */
    enNetRatType = NAS_MML_GetCurrNetRatType();

    if (VOS_TRUE != NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return VOS_TRUE;
    }

    if (enNetRatType == stDestPlmn.enRat)
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    /*需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    else
    {
        /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSuspendReq_PlmnSelection(enNetRatType);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg = VOS_NULL_PTR;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;

    /* 若打断类型是用户请求的关机，则直接退出等注册请求状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_MMA_REG_REQ);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 记录打断标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);


    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTiWaitMmaRegReqExpired_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitMmaRegReqExpired_PlmnSelection_WaitMmaRegReq ENTERED");

    /* 若有abort标志则回复abort，否则回复失败 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }
    
    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)


VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitLmmPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();
    
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmPlmnCnf_PlmnSelection_WaitLmmPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_PLMN_SRCH_CNF_STRU                             *pstLmmSrchCnfMsg = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
    
    pstLmmSrchCnfMsg = (LMM_MMC_PLMN_SRCH_CNF_STRU*)pstMsg;

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 设置当前模式是否进行全频搜网，是否存在网络覆盖 */
    NAS_MMC_UpdateLmmCoverageFlg_PlmnSelection(pstLmmSrchCnfMsg);

    if (MMC_LMM_PLMN_SRCH_RLT_SPEC_SUCC == pstLmmSrchCnfMsg->enRlst)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /*根据不同的搜网模式，迁移到不同的L2状态，启动保护定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_SYSINFO_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_SYS_INFO, TI_NAS_MMC_WAIT_LMM_SYS_INFO_LEN);

        return VOS_TRUE;
    }

    /* 更新当前接入技术进行了全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, VOS_TRUE);

    /* 将搜网消息中携带的网络信息转换为内部消息中网络信息 */
    NAS_MMC_ConvertLmmPlmnList2SearchedPlmnListInfo(&(pstLmmSrchCnfMsg->stPlmnIdList),
                                                     &stPlmnSearchInfo);

    /* 根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);


    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_LTE == stDestPlmn.enRat)
        {
            /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }


    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLmmAttachCnf_PlmnSelection_WaitLmmPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 当前LTE的能力状态不为DISABLE 状态，则不处理该消息 */
    if ( NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS != NAS_MML_GetLteCapabilityStatus())
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmAttachCnf_PlmnSelection_WaitLmmPlmnSearchCnf: unexpect message received");

        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmDetachCnf_PlmnSelection_WaitLmmPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 当前LTE的能力状态不为DISABLE 状态，则不处理该消息 */
    if ( NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS != NAS_MML_GetLteCapabilityStatus())
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmDetachCnf_PlmnSelection_WaitLmmPlmnSearchCnf: unexpect message received");

        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTiWaitLmmPlmnSearchCnfExpired_PlmnSelection_WaitLmmPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{   
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    stPlmnSearchInfo;

    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLmmPlmnSearchCnfExpired_PlmnSelection_WaitLmmPlmnSearchCnf: Timer Expired");

    /* 设置当前网络已经进行全频搜网 */
    NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, VOS_TRUE);

    /* 设置当前网络无覆盖 */
    NAS_MMC_SetRatCoverage_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, NAS_MMC_COVERAGE_TYPE_NONE);

    /* 设置当前接入技术没有搜索到任何网络 */
    stPlmnSearchInfo.ulHighPlmnNum = 0x0;
    stPlmnSearchInfo.ulLowPlmnNum  = 0x0;
    stPlmnSearchInfo.enRatType     = NAS_MML_NET_RAT_TYPE_LTE;

    /* 根据消息中携带的搜网信息更新搜网列表 */
    NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&stPlmnSearchInfo);
    
    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*根据不同的搜网模式，迁移到不同的L2状态，启动不同保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
    
}



VOS_UINT32 NAS_MMC_RcvLmmSuspendCnf_PlmnSelection_WaitLmmSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_CNF_ST                *pstGuSuspendCnf = VOS_NULL_PTR;
    LMM_MMC_SUSPEND_CNF_STRU           *pstLmmSuspendCnf = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

    pstGuSuspendCnf = (RRMM_SUSPEND_CNF_ST*)pstMsg;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* L模时，做如下格式转换 */
    pstLmmSuspendCnf = (LMM_MMC_SUSPEND_CNF_STRU*)pstMsg;

    pstGuSuspendCnf->ucResult = (VOS_UINT8)(pstLmmSuspendCnf->ulRst);

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_SUSPEND_CNF);

    if (MMC_AS_SUSPEND_FAILURE == pstGuSuspendCnf->ucResult)
    {
        /* 不可恢复错误,调用底软接口RESET */

        NAS_MML_SoftReBoot();

        return VOS_TRUE;
    }


    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 发送搜网结果,原因为退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }


    /* 获取需要搜索的网络及其接入技术 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        /* 根据获取的接入技术设置当前接入技术 */
        NAS_MML_SetCurrNetRatType(stDestPlmn.enRat);

        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);

        /* 保证挂起L回到GU时PS的注册状态始终为2,此处进行一次更新(挂起L时L可能上报的注册状态不为2) */
        NAS_MMC_UpdateRegStateSpecPlmnSearch();
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmSuspendCnf_PlmnSelection_WaitLmmSuspendCnf,WARNING:CAN'T FIND THE NEXT PLMN");

        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvLmmPlmnSearchStopCnf_PlmnSelection_WaitLmmPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    NAS_MMC_StopTimer( TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF );

    /* 有打断时，退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
        
        NAS_MMC_FSM_QuitFsmL2();
        
        return VOS_TRUE;
    }

    /* 无打断时:如果当前LTE的能力状态为disable LTE，则搜索下个非LTE的网络
        如果当前LTE的能力状态为Enable LTE,则搜索下个可用的LTE网络
        Enable LTE对应的场景为:等LMM的搜网结果时收到LmmDetachCnf，需要Disable LTE,迁到等LmmStopCnf状态
        在等待LmmStopCnf时，收到LmmAttachCnf，又需要Enable LTE,此时在收到LmmStopCnf需要继续去LTE下搜网 */
    
    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_LTE == stDestPlmn.enRat)
        {
            /* 向LMM发送搜网请求，迁移状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

            NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;

}



VOS_UINT32 NAS_MMC_RcvTiWaitLmmStopCnfExpired_PlmnSelection_WaitLmmPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;

	/* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLmmStopCnfExpired_PlmnSelection_WaitLmmPlmnStopCnf: Timer Expired");

    /* 有打断时，退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
        
        NAS_MMC_FSM_QuitFsmL2();
        
        return VOS_TRUE;
    }

    /* 无打断时:如果当前LTE的能力状态为disable LTE，则搜索下个非LTE的网络
        如果当前LTE的能力状态为Enable LTE,则搜索下个可用的LTE网络
        Enable LTE对应的场景为:等LMM的搜网结果时收到LmmDetachCnf，需要Disable LTE,迁到等LmmStopCnf状态
        在等待LmmStopCnf时，收到LmmAttachCnf，又需要Enable LTE,此时在收到LmmStopCnf需要继续去LTE下搜网 */
    
    /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，进入限制驻留 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        if (NAS_MML_NET_RAT_TYPE_LTE == stDestPlmn.enRat)
        {
            /* 向LMM发送搜网请求，迁移状态，并启动保护定时器 */
            NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
        }
        else
        {
            /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
            NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE);
        }
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

            NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
    }

    return VOS_TRUE;


}
VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitLSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitLSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;

    MMA_MMC_PLMN_ID_STRU                stPlmnId;
    VOS_UINT32                          ulCurrentEventType;
    LMM_MMC_SYS_INFO_IND_STRU          *pstLmmSysInfoMsg    = VOS_NULL_PTR;

    /* 停止等待系统消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_SYS_INFO);

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    if (VOS_FALSE == NAS_MMC_IsAllowedRegAfterAcqSucc_PlmnSelection())
    {
        /* 如果入口是获取请求，向MMA回复获取成功，否则上报获取指示 */
        pstLmmSysInfoMsg  = (LMM_MMC_SYS_INFO_IND_STRU*)pstMsg;
        NAS_MMC_ConvertLmmPlmnToGUNasFormat(&(pstLmmSysInfoMsg->stLteSysInfo.stSpecPlmnIdList.astSuitPlmnList[0]), &stPlmnId);

        if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_ACQ_REQ) == ulCurrentEventType)
        {
            NAS_MMC_SndMmaAcqCnf(MMC_MMA_ACQ_RESULT_SUCCESS, &stPlmnId, pstLmmSysInfoMsg->stLteSysInfo.usArfcn);

            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();
        }
        else
        {
            NAS_MMC_SndMmaAcqInd(MMC_MMA_ACQ_RESULT_SUCCESS, &stPlmnId, pstLmmSysInfoMsg->stLteSysInfo.usArfcn);
        }

        /* 状态迁移到NAS_MMC_PLMN_SELECTION_STA_WAIT_MMA_REG_REQ */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_MMA_REG_REQ);

        /* 启动保护定时器 */
        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_MMA_REG_REQ, TI_NAS_MMC_WAIT_MMA_REG_REQ_LEN);

        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_LmmSysInfo(pstMsg);



    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_STOP);

    /* 更新当前等待EPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);
    
#if (FEATURE_ON == FEATURE_IMS)    
    /* 增加等待IMS是否可用结果FLG,MMA保证收到PS注册结果上报IMS是否可用指示到MMC */    
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);
#endif    

    /* 更新CS的注册Cause和AdditionalAction */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT);

    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);

    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_LTE);

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 转发GMM系统消息,供rabm使用 */
    NAS_MMC_SndGmmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);
    NAS_MMC_SndMmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    NAS_MMC_SndLmmLteSysInfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    /* 迁移到等待注册结果子状态PLMN_SRCH:NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_REG_IND, TI_NAS_MMC_WAIT_EPS_REG_IND_LEN);

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvTiWaitLSysInfoExpired_PlmnSelection_WaitLSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLSysInfoExpired_PlmnSelection_WaitLSysInfoInd: Timer Expired");

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);

    /*如果当前需要退出状态机*/
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络
     */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /*  获取下个网络失败，则进行出服务区处理*/
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /*下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器*/
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    /* 不需要Suspend */
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }


    return VOS_TRUE;

}





VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;


    /* 若打断类型是用户请求的关机，则直接退出等注册结果状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 当前没有链接可以直接退出 */
    if ( VOS_FALSE == NAS_MML_IsRrcConnExist())
    {
        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 设置Abort标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /* 如果需要立即退出 */
    if (NAS_MMC_ABORT_FSM_IMMEDIATELY == pstAbortMsg->enAbortFsmType)
    {
        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 主动请求释放链接 */
        NAS_MMC_SndLmmRelReq();

        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);
        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);
    }

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLmmMmcDetachInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_IND_STRU                                *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    pstDetachMsg        = (LMM_MMC_DETACH_IND_STRU*)pstMsg;

    /* 处理DETACH消息，返回下一步动作 */
    NAS_MMC_ProcLmmDetachInd(pstDetachMsg, &enCsAdditionalAction, &enPsAdditionalAction);

    /*更新EPS的下一步的动作类型*/
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /*更新CS的下一步的动作类型*/
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcDetachCnf_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_CNF_STRU                                *pstDetachCnfMsg = VOS_NULL_PTR;

    pstDetachCnfMsg        = (LMM_MMC_DETACH_CNF_STRU*)pstMsg;

    /* 不过detach结果不是鉴权被拒，不需要进状态机，预处理中就可以处理 */
    if ( MMC_LMM_DETACH_RSLT_AUTH_REJ != pstDetachCnfMsg->ulDetachRslt)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmMmcDetachCnf_PlmnSelection_WaitEpsConnRelInd: Unexpected detach result!");

        return VOS_TRUE;
    }

    /*更新EPS的下一步的动作类型*/
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL);

    /*更新CS的下一步的动作类型*/
    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcStatusInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_STATUS_IND_STRU            *pstLmmStatusIndMsg = VOS_NULL_PTR;

    pstLmmStatusIndMsg = (LMM_MMC_STATUS_IND_STRU*)pstMsg;

    if ( VOS_FALSE == pstLmmStatusIndMsg->bitOpConnState)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmMmcStatusInd_PlmnSelection_WaitEpsRegRsltInd:bitOpConnState IS VOS_FALSE");
        return VOS_TRUE;
    }

    /* 更新LMM的链接状态 */
    switch (pstLmmStatusIndMsg->ulConnState)
    {
        case MMC_LMM_CONN_ESTING:
        case MMC_LMM_CONNECTED_SIG:
            NAS_MML_SetEpsSigConnStatusFlg(VOS_TRUE);
            break;

        case MMC_LMM_CONNECTED_DATA:
            NAS_MML_SetEpsServiceConnStatusFlg(VOS_TRUE);
            break;

        case MMC_LMM_CONN_IDLE:
            NAS_MML_SetEpsSigConnStatusFlg(VOS_FALSE);
            NAS_MML_SetEpsServiceConnStatusFlg(VOS_FALSE);
            break;

        default:
            break;
    }

    return VOS_TRUE;

}
VOS_UINT32  NAS_MMC_RcvLmmMmcAttachInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    LMM_MMC_ATTACH_IND_STRU            *pstLmmAttachIndMsg = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    NAS_MML_LTE_UE_OPERATION_MODE_ENUM_UINT8                enUeOperationMode;

    enUeOperationMode   =  NAS_MML_GetLteUeOperationMode();
    pstLmmAttachIndMsg  = (LMM_MMC_ATTACH_IND_STRU*)pstMsg;

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* LMM在搜网未回复search cnf时收到用户detach ps请求后，后续不会做注册给mmc回复attach result
       为MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW，请求类型填的是MMC_LMM_ATT_TYPE_BUTT，
       MMC需要根据当前ue operation mode给请求类型重新赋值 */
    if ((MMC_LMM_ATT_TYPE_BUTT == pstLmmAttachIndMsg->ulReqType)
     && (MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW == pstLmmAttachIndMsg->ulAttachRslt))
    {
        pstLmmAttachIndMsg->ulReqType = NAS_MMC_ConvertLmmAttachReqType(enUeOperationMode);
    }

    /* 向GMM和MM转发注册结果消息 */
    NAS_MMC_SndMmLmmAttachInd(pstLmmAttachIndMsg);
    NAS_MMC_SndGmmLmmAttachInd(pstLmmAttachIndMsg);

    /* 处理attach结果 */
    NAS_MMC_ProcLmmAttachInd_PlmnSelection(pstLmmAttachIndMsg);

    /* 已经Disable L并且注册成功,需要将当前网络保存在状态机上下文中,用于后续
       GU搜索失败重新回该网络提供PS业务*/
    if ((NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS == NAS_MML_GetLteCapabilityStatus())
      && (MMC_LMM_ATT_RSLT_SUCCESS == pstLmmAttachIndMsg->ulAttachRslt))
    {
        /* 保存当前网络在状态机上下文中 */
        NAS_MMC_SetCsPsMode1ReCampLtePlmn_PlmnSelection(NAS_MML_GetCurrCampPlmnId());
    }

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {

        if (VOS_TRUE == NAS_MMC_IsDisableLteNeedLocalReleaseEpsConn())
        {
            NAS_MMC_SndLmmRelReq();

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

            return VOS_TRUE;
        }


        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;

}
VOS_UINT32  NAS_MMC_RcvLmmMmcTauResultInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    LMM_MMC_TAU_RESULT_IND_STRU        *pstLmmTauIndMsg = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    pstLmmTauIndMsg  = (LMM_MMC_TAU_RESULT_IND_STRU*)pstMsg;

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 向GMM和MM转发注册结果消息 */
    NAS_MMC_SndMmLmmTauResultInd(pstLmmTauIndMsg);
    NAS_MMC_SndGmmLmmTauResultInd(pstLmmTauIndMsg);

    /* 处理tau结果 */
    NAS_MMC_ProcLmmTauResultInd_PlmnSelection(pstLmmTauIndMsg);

    /* 已经Disable L并且注册成功,需要将当前网络保存在状态机上下文中,用于后续
       GU搜索失败重新回该网络提供PS业务*/
    if ((NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS == NAS_MML_GetLteCapabilityStatus())
      && (MMC_LMM_TAU_RSLT_SUCCESS == pstLmmTauIndMsg->ulTauRst))
    {
        /* 保存当前网络在状态机上下文中 */
        NAS_MMC_SetCsPsMode1ReCampLtePlmn_PlmnSelection(NAS_MML_GetCurrCampPlmnId());
    }

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();
        }
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLmmMmcDetachInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_IND_STRU                                *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    pstDetachMsg        = (LMM_MMC_DETACH_IND_STRU*)pstMsg;

    /* 处理DETACH消息，返回下一步动作 */
    NAS_MMC_ProcLmmDetachInd(pstDetachMsg, &enCsAdditionalAction, &enPsAdditionalAction);

    /*更新EPS的下一步的动作类型*/
    if (NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /*更新CS的下一步的动作类型*/
    if (NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcServiceRsltInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SERVICE_RESULT_IND_STRU                        *pstSerRsltMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    pstSerRsltMsg     = (LMM_MMC_SERVICE_RESULT_IND_STRU*)pstMsg;

    /* 根据LMM的service result结果返回下一步处理动作 */
    enAdditionalAction = NAS_MMC_ProcLmmServiceRsltInd(pstSerRsltMsg);

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvLmmSuspendInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    LMM_MMC_SUSPEND_IND_STRU           *pstLSuspendMsg = VOS_NULL_PTR;
    MMC_SUSPEND_CAUSE_ENUM_UINT8        enSuspendCause;

    pstLSuspendMsg = (LMM_MMC_SUSPEND_IND_STRU*)pstMsg;

    /* L模下，挂起原因定义与GU下不同，转换一下挂起原因定义 */
    NAS_MMC_ConvertLmmSysChngTypeToMmcType(pstLSuspendMsg->ulSysChngType, &enSuspendCause);

    /* 如果为OOS或者参数不正确, 则不允许进行异系统重选 */
    if ((MMC_SUSPEND_CAUSE_PLMNSEARCH == enSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= enSuspendCause))
    {
        NAS_MMC_SndLmmSuspendRsp(MMC_LMM_FAIL);
    }
    else
    {
        /* 根据SUSPEND消息中的不同类型起相应的状态机 */
        NAS_MMC_LoadInterSysFsm_PlmnSelection(enSuspendCause);
    }


    return VOS_TRUE;

}
VOS_UINT32 NAS_MMC_RcvMmcMmcSuspendRslt_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    NAS_MMCMMC_SUSPEND_RSLT_STRU       *pstSuspendRslt = VOS_NULL_PTR;
#endif

     /* 迁移到不同的接入技术时，需要迁移到不同的处理注册结果状态 */

     enRatType = NAS_MML_GetCurrNetRatType();

    if ((NAS_MML_NET_RAT_TYPE_WCDMA == enRatType)
     || (NAS_MML_NET_RAT_TYPE_GSM == enRatType))
    {
        /* 停止当前等待EPS注册结果的定时器 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 更新当前等待CSPS的注册结果 */
        NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

        /* 更新CSPS的AdditionalAction */
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

        /* 更新CSPS的RegCause */
        NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
        NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);


        /* 收到系统消息后迁移到等待CS+PS域的注册结果并启动定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_CSPS_REG_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND, TI_NAS_MMC_WAIT_CSPS_REG_IND_LEN);


        return VOS_TRUE;
    }


#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    pstSuspendRslt  = (NAS_MMCMMC_SUSPEND_RSLT_STRU*)pstMsg;

    /* 重选到HRPD后,退出选网状态机 */
    if ((NAS_MML_NET_RAT_TYPE_BUTT == enRatType)
     && (NAS_MMC_SUSPEND_SUCCESS == pstSuspendRslt->enRslt))
    {
        /* 停止当前等待EPS注册结果的定时器 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 向上层状态机发送搜索结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_INTER_SYS_HRPD);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }
#endif

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvMmcMmcSuspendRslt_PlmnSelection_WaitEpsRegRsltInd:ENTERED");
    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvTiWaitEpsRegRsltIndExpired_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_FSM_ID_ENUM_UINT32              enCurrentFsmId;
    VOS_UINT32                              ulCurrentEventType;

    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8    enAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8    enPsAdditionalAction;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitEpsRegRsltIndExpired_PlmnSelection_WaitEpsRegRsltInd: Timer Expired");

    /* 更新服务状态 */
    NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);

    /* 通知TAF当前的服务状态 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();

    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }

    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

        /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
        NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
    enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();

    /* 等待CS PS的注册结果时，有可能一个域发起注册后进行了呼叫以及紧急呼叫等业务
      (比如G下进行CS域进行呼叫，PS域无法发起注册),导致MMC一致等不到另外一个域的
      注册结果，此时MMC直接通过additional触发搜网不合理，MMC此时根据当前收到的注
      册结果更新的additional值来决定是否成功退出搜网状态机或者失败退出搜网状态机，
      由L1状态机根据搜网状态机的退出结果决定是否启动周期性选网定时器还是继续等待
      业务结束以及CSPS的注册结果 */

    enAdditionalAction = NAS_MMC_GetPrioAddtionalAction(enCsAdditionalAction, enPsAdditionalAction);

    /* 附加操作为驻留当前网络 */
    if ((NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_CELL_SELECTION == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_ACCESS_BARRED == enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);
    }
    else
    {
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;

}
VOS_UINT32 NAS_MMC_RcvLmmAreaLostInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;
    VOS_UINT32                          ulResult;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurPlmn;

    /* 解决版本Coverity问题 */
    PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /* 获取当前驻留的网络信息 */
    pstCurCampInfo     = NAS_MML_GetCurrCampPlmnInfo();
    stCurPlmn.stPlmnId = pstCurCampInfo->stLai.stPlmnId;
    stCurPlmn.enRat    = pstCurCampInfo->enNetRatType;

    /* 获取高优先级网络列表 */
    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 更新当前网络的搜索状态为已注册过，在此轮搜网中，不会再搜此网络 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                            NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                            pstPlmnSelectionListInfo);

    /* 停止等待Eps注册结果消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 清空等待注册结果标志 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();

    /* 如果是NCELL搜网，不需要再搜索其他网络了，退出状态机 */
    if (NAS_MMC_PLMN_SEARCH_SCENE_REG_NCELL == NAS_MMC_GetPlmnSearchScene_PlmnSelection())
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 获取下一个要搜索的网络,若获取失败，则给上层状态机上报搜网失败，退出
      若获取成功，则判断下一个要搜索的网络是否与当前驻留网络相同，
      若相同则在当前模式发起搜网，否则挂起当前网络 */
    ulResult = NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn);

    /*  获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != ulResult)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();
        return VOS_TRUE;
    }

    /* 向MM、GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /* 下一个要搜索的网络是否与当前驻留网络不同,需要挂起当前模式，切换到等待挂起状态，启动保护定时器 */
    if (stDestPlmn.enRat != NAS_MML_GetCurrNetRatType())
    {
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }
    else
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_LmmSysInfo(pstMsg);



    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_LTE);

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 转发GMM系统消息,供rabm使用 */
    NAS_MMC_SndGmmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);
    NAS_MMC_SndMmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    NAS_MMC_SndLmmLteSysInfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    return VOS_TRUE;

}
VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList   = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    /* 保存用户指定搜网信息，更新当前的搜网类型为手动模式 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                        pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause, enPsRegCause))
    {
        return VOS_TRUE;
    }

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);
    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmaImsVoiceCapInd_PlmnSelection_WaitEpsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_IMS_VOICE_CAP_IND_STRU     *pstImsVoiceInd = VOS_NULL_PTR;
    VOS_UINT8                           ucWaitRegRsltFlag;
    
    ucWaitRegRsltFlag   = NAS_MMC_GetWaitRegRsltFlag_PlmnSelection();
    pstImsVoiceInd      = (MMA_MMC_IMS_VOICE_CAP_IND_STRU *)pstMsg;
    
    /* 如果在等待PS的注册结果则继续等待PS注册结果 */
    if (NAS_MMC_WAIT_REG_RESULT_IND_PS == (ucWaitRegRsltFlag & NAS_MMC_WAIT_REG_RESULT_IND_PS))
    {
        return VOS_TRUE;
    }

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);        

    /* IMS不可用时判断是否需要disable LTE */
    if (VOS_FALSE  == pstImsVoiceInd->ucAvail) 
    {
        if (VOS_TRUE == NAS_MMC_IsNeedDisableLte_ImsVoiceNotAvail())
        {
            NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_PLMN_SELECTION);

            NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS);

            NAS_MML_SetDisableLteReason(MMC_LMM_DISABLE_LTE_REASON_LTE_VOICE_NOT_AVAILABLE);

            /* 已经Disable L并且注册成功,需要将当前网络保存在状态机上下文中,用于后续
               GU搜索失败重新回该网络提供PS业务*/
            NAS_MMC_SetCsPsMode1ReCampLtePlmn_PlmnSelection(NAS_MML_GetCurrCampPlmnId());
        }
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        if (VOS_TRUE == NAS_MMC_IsDisableLteNeedLocalReleaseEpsConn())
        {
            NAS_MMC_SndLmmRelReq();

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

            return VOS_TRUE;
        }

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg = VOS_NULL_PTR;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;

    /* 若打断类型是用户请求的关机，则直接退出等链接释放结果状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 设置Abort标志, 等链接释放后进行quit */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvLmmMmcStatusInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_STATUS_IND_STRU            *pstLmmStatusIndMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;

    pstLmmStatusIndMsg  = ( LMM_MMC_STATUS_IND_STRU* )pstMsg;


    /* 更新EPS域的链接存在状态 */
    if ( (MMC_LMM_CONN_ESTING == pstLmmStatusIndMsg->ulConnState)
      || (MMC_LMM_CONNECTED_SIG == pstLmmStatusIndMsg->ulConnState) )
    {
        /* 设置存在EPS信令连接 */
        NAS_MML_SetEpsSigConnStatusFlg(VOS_TRUE);
    }
    else if (MMC_LMM_CONNECTED_DATA == pstLmmStatusIndMsg->ulConnState)
    {
        /* 设置存在EPS业务连接 */
        NAS_MML_SetEpsServiceConnStatusFlg(VOS_TRUE);
    }
    else if (MMC_LMM_CONN_IDLE == pstLmmStatusIndMsg->ulConnState)
    {
        /* 设置不存在EPS信令连接 */
        NAS_MML_SetEpsSigConnStatusFlg(VOS_FALSE);
        NAS_MML_SetEpsServiceConnStatusFlg(VOS_FALSE);
    }
    else
    {
        ;
    }

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

    /* 是否需要 Abort状态机 */
    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        /* 主动释放连接时,由于addition也可能导致搜网,前面收到注册结果未刷新选网列表,此处需要再刷新一次 */
        enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(), 
                                                              NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());
        
        NAS_MMC_RefreshPlmnSelectionListAfterRegFail_PlmnSelection(enAdditionalAction);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvTiWaitEpsConnRelIndExpired_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitEpsConnRelIndExpired_PlmnSelection_WaitEpsConnRelInd ENTERED");


    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在而且不是主动释放连接则进行附加操作 */
    if (VOS_FALSE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        /* 发送主动释放请求到MM、GMM，重新启动等待连接释放定时器 */
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_TRUE);

        NAS_MMC_SndLmmRelReq();
        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);
    }
    else
    {
        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_FALSE);

        NAS_MMC_SearchNextPlmn_PlmnSelection();
    }


    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU         stOldCampInfo;

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    /* 如果是主动请求释放,由于需要立即进行搜网,系统消息不处理 */
    if (VOS_TRUE == NAS_MMC_GetRelRequestFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_LmmSysInfo(pstMsg);
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_STOP);



    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_LTE);

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 转发GMM系统消息,供rabm使用 */
    NAS_MMC_SndGmmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);
    NAS_MMC_SndMmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    NAS_MMC_SndLmmLteSysInfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmSuspendInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    LMM_MMC_SUSPEND_IND_STRU           *pstLSuspendMsg = VOS_NULL_PTR;
    MMC_SUSPEND_CAUSE_ENUM_UINT8        enSuspendCause;
    VOS_UINT32                          ulRelRequestFlg;

    pstLSuspendMsg = (LMM_MMC_SUSPEND_IND_STRU*)pstMsg;

    ulRelRequestFlg = NAS_MMC_GetRelRequestFlag_PlmnSelection();

    /* L模下，挂起原因定义与GU下不同，转换一下挂起原因定义 */
    NAS_MMC_ConvertLmmSysChngTypeToMmcType(pstLSuspendMsg->ulSysChngType, &enSuspendCause);

    /* 如果是主动释放或者已经接收到Abort消息,则不允许进行异系统重选 */
    if ((VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
     || (VOS_TRUE == ulRelRequestFlg)
     || (MMC_SUSPEND_CAUSE_PLMNSEARCH == enSuspendCause)
     || (MMC_SUSPEND_CAUSE_BUTT <= enSuspendCause))
    {
        NAS_MMC_SndLmmSuspendRsp(MMC_LMM_FAIL);
    }
    else
    {
        /* 根据SUSPEND消息中的不同类型起相应的状态机 */
        NAS_MMC_LoadInterSysFsm_PlmnSelection(enSuspendCause);
    }


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSuspendRslt_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    NAS_MMCMMC_SUSPEND_RSLT_STRU       *pstSuspendRslt = VOS_NULL_PTR;
#endif

    /* 迁移到不同的接入技术时，需要迁移到不同的处理注册结果状态 */
    enRatType = NAS_MML_GetCurrNetRatType();

    if ((NAS_MML_NET_RAT_TYPE_WCDMA == enRatType)
     || (NAS_MML_NET_RAT_TYPE_GSM == enRatType))
    {
        /* 停止当前等待eps连接释放定时器 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

        /* 更新当前等待CSPS的注册结果 */
        NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_CS);
        NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

        /* 更新CSPS的AdditionalAction */
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

        /* 更新CSPS的RegCause */
        NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
        NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);


        /* 收到系统消息后迁移到等待CS+PS域的注册结果并启动定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_CSPS_REG_IND);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND, TI_NAS_MMC_WAIT_CSPS_REG_IND_LEN);


        return VOS_TRUE;
    }

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    pstSuspendRslt  = (NAS_MMCMMC_SUSPEND_RSLT_STRU*)pstMsg;

    /* 重选到HRPD后,退出选网状态机 */
    if ((NAS_MML_NET_RAT_TYPE_BUTT == enRatType)
     && (NAS_MMC_SUSPEND_SUCCESS == pstSuspendRslt->enRslt))
    {
        /* 停止当前等待eps连接释放定时器 */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

        /* 向上层状态机发送高优先级搜索失败，需要搜网 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_INTER_SYS_HRPD);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }
#endif

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL);

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 如果信令连接不存在可能发起搜网 */
    NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection();


    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    /* 保存手动搜网信息 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                       pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    /* 由于用户指定的PlmnId及RAT和当前驻留的PlmnId及RAT相同，直接返回指定搜网结果 */
    enCsCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsCause, enPsCause))
    {
        return VOS_TRUE;
    }

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLmmMmcServiceRsltInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SERVICE_RESULT_IND_STRU                        *pstSerRsltMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    pstSerRsltMsg     = (LMM_MMC_SERVICE_RESULT_IND_STRU*)pstMsg;

    /* 根据LMM的service result结果返回下一步处理动作 */
    enAdditionalAction = NAS_MMC_ProcLmmServiceRsltInd(pstSerRsltMsg);

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection();

    return VOS_TRUE;
}




VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitAsSuspendCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录需要终止当前状态机标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitWasSuspendCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_CNF_ST                *pstSuspendCnf = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU          stReCampLtePlmn;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurRat;
    NAS_MML_PLMN_LIST_WITH_RAT_STRU     stDestPlmnList;

    pstSuspendCnf            = (RRMM_SUSPEND_CNF_ST*)pstMsg;
    stReCampLtePlmn.enRat    = NAS_MML_NET_RAT_TYPE_LTE;

    PS_MEM_CPY(&(stReCampLtePlmn.stPlmnId),
               NAS_MMC_GetCsPsMode1ReCampLtePlmn_PlmnSelection(),
               sizeof(stReCampLtePlmn.stPlmnId));

    enCurRat      = NAS_MML_GetCurrNetRatType();

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF);

    if (MMC_AS_SUSPEND_FAILURE == pstSuspendCnf->ucResult)
    {
        /* 不可恢复错误,调用底软接口RESET */
        if ( (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
          && (NAS_MML_NET_RAT_TYPE_WCDMA == enCurRat) )
        {
            NAS_MML_SoftReBoot();
        }
        else
        {
            NAS_MML_SoftReBoot();
        }

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 搜索需要重新回LTE的网络，
       由于挂起过程中可能接收到用户Detach PS操作，此处需要再判断网络是否有效 */
    if (VOS_TRUE == NAS_MMC_IsReCampLtePlmnValid_PlmnSelection())
    {
        /* 设置当前接入技术为LTE */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_LTE);

        /* 向LMM发送指定搜网 */
        PS_MEM_SET(&stDestPlmnList, 0x0, sizeof(stDestPlmnList));

        stDestPlmnList.ulPlmnNum = 1;
        PS_MEM_CPY(&stDestPlmnList.astPlmnId[0], &stReCampLtePlmn, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

        NAS_MMC_SndLmmSpecPlmnSearchReq(&stDestPlmnList, VOS_FALSE);

        /* 状态迁移NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE);

        /* 启动保护定时器TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF */
        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF_LEN);
    }
    else
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvRrMmSuspendCnf_PlmnSelection_WaitGasSuspendCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SUSPEND_CNF_ST                *pstSuspendCnf = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU          stReCampLtePlmn;
    NAS_MML_PLMN_LIST_WITH_RAT_STRU     stDestPlmnList;

    pstSuspendCnf            = (RRMM_SUSPEND_CNF_ST*)pstMsg;
    stReCampLtePlmn.enRat    = NAS_MML_NET_RAT_TYPE_LTE;

    PS_MEM_CPY(&(stReCampLtePlmn.stPlmnId),
               NAS_MMC_GetCsPsMode1ReCampLtePlmn_PlmnSelection(),
               sizeof(stReCampLtePlmn.stPlmnId));

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF);

    if (MMC_AS_SUSPEND_FAILURE == pstSuspendCnf->ucResult)
    {
        /* 不可恢复错误,调用底软接口RESET */
        NAS_MML_SoftReBoot();

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 搜索需要重新回LTE的网络，
       由于挂起过程中可能接收到用户Detach PS操作，此处需要再判断网络是否有效 */
    if (VOS_TRUE == NAS_MMC_IsReCampLtePlmnValid_PlmnSelection())
    {
        /* 设置当前接入技术为LTE */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_LTE);

        /* 向LMM发送指定搜网 */
        PS_MEM_SET(&stDestPlmnList, 0x0, sizeof(stDestPlmnList));

        stDestPlmnList.ulPlmnNum = 1;
        PS_MEM_CPY(&stDestPlmnList.astPlmnId[0], &stReCampLtePlmn, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

        NAS_MMC_SndLmmSpecPlmnSearchReq(&stDestPlmnList, VOS_FALSE);

        /* 状态迁移NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE);

        /* 启动保护定时器TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF */
        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF_LEN);
    }
    else
    {
        /* 设置当前接入技术为BUTT */
        NAS_MML_SetCurrNetRatType(NAS_MML_NET_RAT_TYPE_BUTT);

        /* 回复Plmn Selection执行结果 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvTiWaitAsSuspendCnfExpired_PlmnSelection_WaitAsSuspendCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurRat;

    enCurRat      = NAS_MML_GetCurrNetRatType();

    /* 不可恢复故障，复位 */
    if ( (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
      && (NAS_MML_NET_RAT_TYPE_WCDMA == enCurRat) )
    {
        NAS_MML_SoftReBoot();
    }
    else if ( NAS_MML_NET_RAT_TYPE_GSM == enCurRat )
    {
        NAS_MML_SoftReBoot();
    }
    else
    {
        NAS_MML_SoftReBoot();
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*迁移到等待停止搜网确认状态，启动保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF_CSPSMODE1_RECAMP_LTE);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmPlmnCnf_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_PLMN_SRCH_CNF_STRU                             *pstLmmSrchCnfMsg = VOS_NULL_PTR;

    pstLmmSrchCnfMsg = (LMM_MMC_PLMN_SRCH_CNF_STRU*)pstMsg;

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 设置当前模式是否进行全频搜网，是否存在网络覆盖 */
    NAS_MMC_UpdateLmmCoverageFlg_PlmnSelection(pstLmmSrchCnfMsg);

    if (MMC_LMM_PLMN_SRCH_RLT_SPEC_SUCC == pstLmmSrchCnfMsg->enRlst)
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_SUCCESS,
                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /*迁移状态，启动保护定时器 */
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_SYSINFO_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_SYS_INFO, TI_NAS_MMC_WAIT_LMM_SYS_INFO_LEN);
    }
    else
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /* 在注册结果中设置了CS/PS服务状态但未上报，此时需要更新 */
        NAS_MMC_SetCsServiceStatus(NAS_MMC_NO_SERVICE);
        NAS_MMC_SetPsServiceStatus(NAS_MMC_NO_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_NO_SERVICE);


        /* 更新当前接入技术进行了全频搜网 */
        NAS_MMC_SetAllBandSearch_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, VOS_TRUE);


        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出层二状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmAttachCnf_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 当前LTE的能力状态不为DISABLE状态，则不处理该消息 */
    if (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS != NAS_MML_GetLteCapabilityStatus())
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmAttachCnf_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte: unexpect message received");

        return VOS_TRUE;
    }

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*迁移状态，启动保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF_CSPSMODE1_RECAMP_LTE);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmDetachCnf_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 当前LTE的能力状态不为DISABLE状态，则不处理该消息 */
    if (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS != NAS_MML_GetLteCapabilityStatus())
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmDetachCnf_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte: unexpect message received");

        return VOS_TRUE;
    }

    /* 停止定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF);

    /* 通知LMM停止指定搜网 */
    NAS_MMC_SndLmmPlmnSrchStopReq();

    /*迁移状态，启动保护定时器*/
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_STOP_CNF_CSPSMODE1_RECAMP_LTE);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF_LEN);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTiWaitLmmPlmnSearchCnfExpired_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLmmPlmnSearchCnfExpired_PlmnSelection_WaitLmmPlmnSearchCnfCsPsMode1ReCampLte ENTERED");

    NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                    VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

    /* 在注册结果中设置了CS/PS服务状态但未上报，此时需要更新 */
    NAS_MMC_SetCsServiceStatus(NAS_MMC_NO_SERVICE);
    NAS_MMC_SetPsServiceStatus(NAS_MMC_NO_SERVICE);

    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_NO_SERVICE);

    /* 向上层状态机指示选网失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_RcvLmmPlmnStopCnf_PlmnSelection_WaitLmmPlmnStopCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 停止定时器 */
    NAS_MMC_StopTimer( TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF );

    /* 判断是否存在打断标志 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 向上层状态机指示选网被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvTiWaitLmmStopCnfExpired_PlmnSelection_WaitLmmPlmnStopCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLmmStopCnfExpired_PlmnSelection_WaitLmmPlmnStopCnfCsPsMode1ReCampLte: Timer Expired");

    /* 判断是否存在打断标志 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 向上层状态机指示选网被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}

VOS_UINT32 NAS_MMC_RcvLmmPlmnSearchCnf_PlmnSelection_WaitLmmPlmnStopCnfCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_PLMN_SRCH_CNF_STRU          *pstLmmSrchCnfMsg = VOS_NULL_PTR;

    pstLmmSrchCnfMsg = (LMM_MMC_PLMN_SRCH_CNF_STRU*)pstMsg;

    /* 停止定时器 */
    NAS_MMC_StopTimer( TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF );

    /* 设置当前模式是否进行全频搜网，是否存在网络覆盖 */
    NAS_MMC_UpdateLmmCoverageFlg_PlmnSelection(pstLmmSrchCnfMsg);

    /* 判断是否存在打断标志 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 向上层状态机指示选网终止 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitLSysInfoIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 设置Abort标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitLSysInfoIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    /* 停止等待系统消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_LMM_SYS_INFO);

    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE  == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_LmmSysInfo(pstMsg);



    /* 更新当前搜网状态结束 */
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_STOP);

    /* 更新当前等待EPS的注册结果 */
    NAS_MMC_ClearAllWaitRegRsltFlag_PlmnSelection();
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);
    
#if (FEATURE_ON == FEATURE_IMS)        
    /* 增加等待IMS是否可用结果FLG,MMA保证收到PS注册结果上报IMS是否可用指示到MMC */    
    NAS_MMC_SetWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);
#endif    

    /* 更新CS/PS的注册Cause和AdditionalAction */
    NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);
    NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_BUTT);

    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_BUTT);

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_LTE);

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 转发GMM系统消息 */
    NAS_MMC_SndGmmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);
    NAS_MMC_SndMmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    NAS_MMC_SndLmmLteSysInfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    /* 迁移到等待注册结果子状态PLMN_SRCH:NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND_CSPSMODE1_RECAMP_LTE);

    NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_REG_IND, TI_NAS_MMC_WAIT_EPS_REG_IND_LEN);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTiWaitLSysInfoExpired_PlmnSelection_WaitLSysInfoIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitLSysInfoExpired_PlmnSelection_WaitLSysInfoIndCsPsMode1ReCampLte: Timer Expired");

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);

    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /*如果当前需要退出状态机*/
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;

    /* 若打断类型是用户请求的关机或立即打断或当前没有链接可以直接退出 */
    if ((VOS_FALSE == NAS_MML_IsRrcConnExist())
     || (NAS_MMC_ABORT_FSM_IMMEDIATELY == pstAbortMsg->enAbortFsmType)
     || (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType))
    {
        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 回复状态机被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 设置Abort标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcStatusInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_STATUS_IND_STRU            *pstLmmStatusIndMsg = VOS_NULL_PTR;

    pstLmmStatusIndMsg = (LMM_MMC_STATUS_IND_STRU*)pstMsg;

    /* 更新LMM的链接状态 */
    NAS_MML_UpdateEpsConnectionStatus(pstLmmStatusIndMsg->ulConnState);

    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvLmmMmcAttachInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    LMM_MMC_ATTACH_IND_STRU                                *pstLmmAttachIndMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    pstLmmAttachIndMsg  = (LMM_MMC_ATTACH_IND_STRU*)pstMsg;

    /* 清除等待PS标志 */
    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* LMM在搜网未回复search cnf时收到用户detach ps请求后，后续不会做注册给mmc回复attach result
       为MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW，请求类型填的是MMC_LMM_ATT_TYPE_BUTT，
       MMC需要根据当前ue operation mode给请求类型重新赋值 */
    if ((MMC_LMM_ATT_TYPE_BUTT == pstLmmAttachIndMsg->ulReqType)
     && (MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW == pstLmmAttachIndMsg->ulAttachRslt))
    {
        pstLmmAttachIndMsg->ulReqType = NAS_MMC_ConvertLmmAttachReqType(NAS_MML_GetLteUeOperationMode());
    }

    /* 向GMM和MM转发注册结果消息 */
    NAS_MMC_SndMmLmmAttachInd(pstLmmAttachIndMsg);
    NAS_MMC_SndGmmLmmAttachInd(pstLmmAttachIndMsg);

    /* 处理attach结果 */
    NAS_MMC_ProcLmmAttachInd_PlmnSelection(pstLmmAttachIndMsg);

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 状态机退出结果为Abort */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(),
                                                          NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());

    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
        return VOS_TRUE;
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 停定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 附加操作为驻留当前网络 */
    if (VOS_FALSE == NAS_MMC_IsAdditionalActionTrigerPlmnSrch(enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*  如果存在信令连接，迁移到等待连接释放状态  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REL_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

        return VOS_TRUE;
    }

    /* 重新驻留LTE时将服务状态上报 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    if (VOS_TRUE == NAS_MML_GetSimPsRegStatus())
    {
        /* 清除Disable L全局变量标志 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvLmmMmcTauResultInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_TAU_RESULT_IND_STRU                            *pstLmmTauIndMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    pstLmmTauIndMsg  = (LMM_MMC_TAU_RESULT_IND_STRU*)pstMsg;

    /* 清除等待PS标志 */
    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 向GMM和MM转发TAU结果消息 */
    NAS_MMC_SndMmLmmTauResultInd(pstLmmTauIndMsg);
    NAS_MMC_SndGmmLmmTauResultInd(pstLmmTauIndMsg);

    /* 处理TAU结果 */
    NAS_MMC_ProcLmmTauResultInd_PlmnSelection(pstLmmTauIndMsg);

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 状态机退出结果为Abort */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(),
                                                          NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());

    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
        return VOS_TRUE;
    }

    if (NAS_MMC_WAIT_REG_RESULT_IND_NULL != NAS_MMC_GetWaitRegRsltFlag_PlmnSelection())
    {
        return VOS_TRUE;
    }

    /* 停定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 附加操作为驻留当前网络 */
    if (VOS_FALSE == NAS_MMC_IsAdditionalActionTrigerPlmnSrch(enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*  如果存在信令连接，迁移到等待连接释放状态  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REL_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

        return VOS_TRUE;
    }

    /* 重新驻留LTE时将服务状态上报 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    if (VOS_TRUE == NAS_MML_GetSimPsRegStatus())
    {
        /* 清除Disable L全局变量标志 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;

}
VOS_UINT32  NAS_MMC_RcvLmmMmcDetachInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_IND_STRU                                *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    pstDetachMsg        = (LMM_MMC_DETACH_IND_STRU*)pstMsg;

    /* 处理DETACH消息，输出下一步动作 */
    NAS_MMC_ProcLmmDetachInd(pstDetachMsg, &enCsAdditionalAction, &enPsAdditionalAction);

    /*更新EPS的下一步的动作类型*/
    if (NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /*更新CS的下一步的动作类型*/
    if (NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 状态机退出结果为Abort */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(),
                                                              NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());

    /* 选网动作AdditionalAction为需要继续等待注册结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
        /*返回继续等待注册结果*/
        return VOS_TRUE;
    }

    /* 停定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 附加操作为驻留当前网络 */
    if (VOS_FALSE == NAS_MMC_IsAdditionalActionTrigerPlmnSrch(enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*  如果存在信令连接，迁移到等待连接释放状态  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REL_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);
    }
    else
    {
        /* 回复状态机退出失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return VOS_TRUE;

}


VOS_UINT32 NAS_MMC_RcvLmmMmcServiceRsltInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SERVICE_RESULT_IND_STRU                        *pstSerRsltMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;

    pstSerRsltMsg     = (LMM_MMC_SERVICE_RESULT_IND_STRU*)pstMsg;

    /* 对LMM的服务结果进行处理，处理完成后返回需要进行的选网动作 */
    enAdditionalAction = NAS_MMC_ProcLmmServiceRsltInd(pstSerRsltMsg);

    /* 选网动作AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 状态机退出结果为Abort */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(),
                                                              NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());

    /* 选网动作AdditionalAction为需要继续等待注册结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
        /*返回继续等待注册结果*/
        return VOS_TRUE;
    }

    /* 停定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 附加操作为驻留当前网络 */
    if (VOS_FALSE == NAS_MMC_IsAdditionalActionTrigerPlmnSrch(enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*  如果存在信令连接，迁移到等待连接释放状态  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REL_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

        return VOS_TRUE;
    }

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmSuspendInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SUSPEND_IND_STRU           *pstLSuspendMsg = VOS_NULL_PTR;
    MMC_SUSPEND_CAUSE_ENUM_UINT8        enSuspendCause;

    pstLSuspendMsg = (LMM_MMC_SUSPEND_IND_STRU*)pstMsg;

    /* L模下，挂起原因定义与GU下不同，转换一下挂起原因定义 */
    NAS_MMC_ConvertLmmSysChngTypeToMmcType(pstLSuspendMsg->ulSysChngType, &enSuspendCause);

    /* 如果为OOS或者参数不正确, 则不允许进行异系统重选 */
    if (MMC_SUSPEND_CAUSE_BUTT <= enSuspendCause)
    {
        NAS_MMC_SndLmmSuspendRsp(MMC_LMM_FAIL);
    }
    else
    {
        /* 根据SUSPEND消息中的不同类型起相应的状态机 */
        NAS_MMC_LoadInterSysFsm_PlmnSelection(enSuspendCause);
    }

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcMmcSuspendRslt_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType;

    enRatType = NAS_MML_GetCurrNetRatType();

    /* 如果当前异系统改变到GU,直接退出状态机 */
    if ((NAS_MML_NET_RAT_TYPE_WCDMA == enRatType)
     || (NAS_MML_NET_RAT_TYPE_GSM == enRatType))
    {
        /* 状态机退出结果为成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }


#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* 重选到HRPD后,退出选网状态机 */
    if (NAS_MML_NET_RAT_TYPE_BUTT == enRatType)
    {
        /* 停止等待eps注册结果消息的定时器  */
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

        /* 向上层状态机发送高优先级搜索失败，需要搜网 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_INTER_SYS_HRPD);

        /* 退出状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }
#endif

    return VOS_TRUE;
}


VOS_UINT32  NAS_MMC_RcvTiWaitEpsRegRsltIndExpired_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitEpsRegRsltIndExpired_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte: Timer Expired");

    /* 调用NAS_MMC_SetPsServiceStatus更新EPS服务状态NAS_MMC_LIMITED_SERVICE */
    NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);
    NAS_MMC_SetCsServiceStatus(NAS_MMC_LIMITED_SERVICE);

    /* 通知TAF当前的服务状态 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 发送状态机退出结果消息，结果为失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmAreaLostInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 停止等待Eps注册结果消息的定时器  */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 清除驻留状态 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MML_InitCampRai(NAS_MML_GetCurrCampPlmnInfo());
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    /* 当前信号更新 */
    NAS_MML_InitRssiValue(NAS_MML_GetCampCellInfo());

    /* 更新服务状态 */
    NAS_MMC_SetCsServiceStatus(NAS_MMC_NO_SERVICE);

    NAS_MMC_SetPsServiceStatus(NAS_MMC_NO_SERVICE);

    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_NO_SERVICE);

    /* 如果当前需要退出状态机 */
    if (VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection())
    {
        /* 回复状态机选网被打断 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);
    }
    else
    {
        /* 发送状态机退出结果消息，结果为失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                             stOldCampInfo;

    /* 保留老的驻留网络信息 */
    pstCurCampInfo  = NAS_MML_GetCurrCampPlmnInfo();
    PS_MEM_CPY(&stOldCampInfo, pstCurCampInfo, sizeof(NAS_MML_CAMP_PLMN_INFO_STRU));

    /* 根据系统消息，更新MML的全部变量 */
    NAS_MMC_UpdateNetworkInfo_LmmSysInfo(pstMsg);



    /* 状态更新为当前已经驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    /* CBS上报*/
    NAS_MMC_SndCbaMsgNetModInd(NAS_MML_NET_RAT_TYPE_LTE);

    NAS_MMC_SndMmaCampOnInd(VOS_TRUE);

    /* 转发MM/GMM系统消息 */
    NAS_MMC_SndGmmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);
    NAS_MMC_SndMmLteSysinfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    NAS_MMC_SndLmmLteSysInfoInd((LMM_MMC_SYS_INFO_IND_STRU*)pstMsg);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList   = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    /* 保存用户指定搜网信息，更新当前的搜网类型为手动模式 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                        pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause, enPsRegCause))
    {
        return VOS_TRUE;
    }

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 状态迁移到NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 状态机中不需要对SYSCFG状态机结果进行处理，直接返回 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmaImsVoiceCapInd_PlmnSelection_WaitEpsRegRsltIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_IMS_VOICE_CAP_IND_STRU                         *pstImsVoiceInd = VOS_NULL_PTR;
    VOS_UINT8                                               ucWaitRegRsltFlag;    
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    
    ucWaitRegRsltFlag   = NAS_MMC_GetWaitRegRsltFlag_PlmnSelection();
    pstImsVoiceInd      = (MMA_MMC_IMS_VOICE_CAP_IND_STRU *)pstMsg;
    
    /* 如果在等待PS的注册结果则继续等待PS注册结果 */
    if (NAS_MMC_WAIT_REG_RESULT_IND_PS == (ucWaitRegRsltFlag & NAS_MMC_WAIT_REG_RESULT_IND_PS))
    {
        return VOS_TRUE;
    }

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_IMS);

    /* IMS不可用时判断是否需要disable LTE */
    if (VOS_FALSE  == pstImsVoiceInd->ucAvail) 
    {
        if (VOS_TRUE == NAS_MMC_IsNeedDisableLte_ImsVoiceNotAvail())
        {
            NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_PLMN_SELECTION);

            NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS);

            NAS_MML_SetDisableLteReason(MMC_LMM_DISABLE_LTE_REASON_LTE_VOICE_NOT_AVAILABLE);
        }
    }
  
    /* 停定时器 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(NAS_MMC_GetCsRegAdditionalAction_PlmnSelection(),
                                                          NAS_MMC_GetPsRegAdditionalAction_PlmnSelection());

    if (VOS_FALSE == NAS_MMC_IsAdditionalActionTrigerPlmnSrch(enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /*  如果存在信令连接，迁移到等待连接释放状态  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REL_IND_CSPSMODE1_RECAMP_LTE);

        NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);

        return VOS_TRUE;
    }

    /* 重新驻留LTE时将服务状态上报 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    if (VOS_TRUE == NAS_MML_GetSimPsRegStatus())
    {
        /* 清除Disable L全局变量标志 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_ABORT_FSM_STRU             *pstAbortMsg = VOS_NULL_PTR;

    pstAbortMsg = (NAS_MMC_ABORT_FSM_STRU*)pstMsg;

    /* 若打断类型是用户请求的关机，则直接退出等链接释放结果状态机 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_POWER_OFF_REQ) == pstAbortMsg->ulEventType)
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 设置Abort标志, 等链接释放后进行quit */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmMmcStatusInd_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_STATUS_IND_STRU            *pstLmmStatusIndMsg = VOS_NULL_PTR;

    pstLmmStatusIndMsg = (LMM_MMC_STATUS_IND_STRU*)pstMsg;

    /* 更新LMM的链接状态 */
    NAS_MML_UpdateEpsConnectionStatus(pstLmmStatusIndMsg->ulConnState);

    /*  如果存在信令连接，则继续等待  */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
        return VOS_TRUE;
    }

    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND);

    /* 是否需要 Abort状态机 */
    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 向上层状态机指示选网ABORT */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 重新驻留LTE时将服务状态上报 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    if (VOS_TRUE == NAS_MML_GetSimPsRegStatus())
    {
        /* 清除Disable L全局变量标志 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvTiWaitEpsRelIndExpired_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvTiWaitEpsRelIndExpired_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte ENTERED");

    if ( VOS_TRUE == NAS_MMC_GetAbortFlag_PlmnSelection() )
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_ABORTED);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return VOS_TRUE;
    }

    /* 重新驻留LTE时将服务状态上报 */
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
    NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

    /* 回复状态机退出失败 */
    NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

    if (VOS_TRUE == NAS_MML_GetSimPsRegStatus())
    {
        /* 清除Disable L全局变量标志 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);
    }

    /* 直接退出当前状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTafPlmnSpecialReq_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MMC_PLMN_SPECIAL_REQ_STRU                          *pstUserSelReqMsg  = VOS_NULL_PTR;

    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSrchList   = VOS_NULL_PTR;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;

    pstUserSelReqMsg    = (MMA_MMC_PLMN_SPECIAL_REQ_STRU*)pstMsg;

    /* 保存用户指定搜网信息，更新当前的搜网类型为手动模式 */
    NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection((NAS_MML_PLMN_ID_STRU*)(&pstUserSelReqMsg->stPlmnId),
                                                        pstUserSelReqMsg->enAccessMode);

    /* 获取选网列表 */
    pstPlmnSrchList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 根据用户指定搜网场景重新初始化选网列表 */
    NAS_MMC_InitPlmnSelectionList(NAS_MMC_PLMN_SEARCH_SCENE_USER_SPEC_PLMN_SEARCH,
                                  VOS_NULL_PTR,
                                  pstPlmnSrchList);

    /* 将用户指定接入技术的网络(也是当前网络)更新为已搜索注册避免重复注册 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(NAS_MMC_GetUserSpecPlmnId(),
                                                   NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                                   pstPlmnSrchList);

    enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
    enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

    if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause, enPsRegCause))
    {
        return VOS_TRUE;
    }

    /* 保存入口消息 */
    NAS_MMC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 状态迁移到NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND */
    NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvTafSysCfgReq_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 直接调起syscfg层二状态机 */
    NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_SYSCFG);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvMmcSyscfgRsltCnf_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 状态机中不需要对SYSCFG状态机结果进行处理，直接返回 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcDetachInd_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_IND_STRU                                *pstDetachMsg = VOS_NULL_PTR;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    pstDetachMsg        = (LMM_MMC_DETACH_IND_STRU*)pstMsg;

    /* 处理DETACH消息，返回下一步动作 */
    NAS_MMC_ProcLmmDetachInd(pstDetachMsg, &enCsAdditionalAction, &enPsAdditionalAction);

    /*更新EPS的下一步的动作类型*/
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /*更新CS的下一步的动作类型*/
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction)
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MMC_RcvLmmMmcDetachCnf_PlmnSelection_WaitEpsRelIndCsPsMode1ReCampLte(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_DETACH_CNF_STRU            *pstDetachCnfMsg = VOS_NULL_PTR;

    pstDetachCnfMsg        = (LMM_MMC_DETACH_CNF_STRU*)pstMsg;

    /* 如果detach结果不是鉴权被拒，不需要进状态机，预处理中就可以处理 */
    if ( MMC_LMM_DETACH_RSLT_AUTH_REJ != pstDetachCnfMsg->ulDetachRslt)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_RcvLmmMmcDetachCnf_PlmnSelection_WaitEpsConnRelInd: Unexpected detach result!");

        return VOS_TRUE;
    }

    /*更新EPS的下一步的动作类型*/
    NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL);

    /*更新CS的下一步的动作类型*/
    NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLSysInfoInd_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stPlmnId;
    LMM_MMC_SYS_INFO_IND_STRU                              *pstLmmSysInfoMsg    = VOS_NULL_PTR;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo    = VOS_NULL_PTR;

    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_CAMP_ON);

    /* 解析系统消息中PLMNID，设置选网列表中该网络状态为存在 */
    pstLmmSysInfoMsg  = (LMM_MMC_SYS_INFO_IND_STRU*)pstMsg;
    NAS_MMC_ConvertLmmPlmnToGUNasFormat(&(pstLmmSysInfoMsg->stLteSysInfo.stSpecPlmnIdList.astSuitPlmnList[0]), &(stPlmnId.stPlmnId));

    stPlmnId.enRat           = NAS_MML_NET_RAT_TYPE_LTE;
    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stPlmnId, NAS_MMC_NET_STATUS_SEARCHED_EXIST, pstPlmnSelectionListInfo);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmSuspendInd_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 该状态不允许进行异系统重选，回复LMM挂起失败 */
    NAS_MMC_SndLmmSuspendRsp(MMC_LMM_FAIL);

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmAreaLostInd_PlmnSelection_WaitMmaRegReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 设置接入层驻留状态为未驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);

    return VOS_TRUE;
}



#endif  /* (FEATURE_ON == FEATURE_LTE) */


VOS_UINT32 NAS_MMC_GetNextSearchPlmn_PlmnSelection(
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDestPlmn
)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList = VOS_NULL_PTR;
    NAS_MML_PLMN_RAT_PRIO_STRU                             *pstPrioRatList       = VOS_NULL_PTR;
    VOS_UINT32                                              i;
    VOS_UINT32                                              ulUserSpecPlmnSearch;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    NAS_MML_SIM_TYPE_ENUM_UINT8                             enSimType;
#endif


    enCurrentFsmId       = NAS_MMC_GetCurrFsmId();
    if (enCurrentFsmId >= NAS_FSM_BUTT )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType   = NAS_MMC_GetCurrFsmEventType();

    /* 获取是否用户指定搜网 */
    ulUserSpecPlmnSearch = NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType);

    /* 获取当前接入技术优先级信息 */
    pstPrioRatList = NAS_MML_GetMsPrioRatList();

    pstPlmnSelectionList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 获取选网列表中排在最前面的状态为未搜索或者已搜索存在的网络 */
    if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn(pstDestPlmn, pstPrioRatList,
                                              ulUserSpecPlmnSearch, pstPlmnSelectionList))
    {
        NAS_MMC_SetCurrSearchingPlmn_PlmnSelection(pstDestPlmn);

        return VOS_TRUE;
    }

    /* 如果当前为自动选网 */
    if (VOS_TRUE == NAS_MMC_IsNeedSearchAvailPlmn_PlmnSelection())
    {
        /* 按SYSCFG设置的接入技术优先级判断是否有某个接入技术未进行全频搜网，
           如果有，返回VOS_TRUE,输出该接入技术的全F无效网络,指示接入层搜索,
           接入层搜索失败时会进行一次全频搜网 */
        for (i = 0; i < pstPrioRatList->ucRatNum; i++)
        {
            /* 增加判断，如果RAT被禁止的话，不需要搜RAT */
            if (VOS_TRUE == NAS_MML_IsRatInForbiddenList(pstPrioRatList->aucRatPrio[i]))
            {
                continue;
            }

#if (FEATURE_ON == FEATURE_LTE)
            enSimType = NAS_MML_GetSimType();

            /* 获取LTE能力状态 */
            enLCapabilityStatus = NAS_MML_GetLteCapabilityStatus();

            /* 满足如下条件,则不需要搜索LTE的网络:
                1)当前网络的接入技术为L且当前PS域卡无效
                2)当前网络的接入技术为L且当前DISABLE LTE
                3)卡类型为SIM卡:对应的场景为:SIM卡时设置成LTE only，不去掉L模，但此时搜网时不搜L模
            */
            if (((VOS_FALSE                             == NAS_MML_GetSimPsRegStatus())
              || ((NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLCapabilityStatus)
               || (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS == enLCapabilityStatus))
              ||(NAS_MML_SIM_TYPE_SIM == enSimType))
             && (NAS_MML_NET_RAT_TYPE_LTE == pstPrioRatList->aucRatPrio[i]))
            {
                continue;
            }
#endif
            /* 如果USIM卡GSM禁止接入，则不选择GSM网络 */
            if ( (VOS_TRUE == NAS_MML_GetGsmForbidFlg())
              && (NAS_MML_NET_RAT_TYPE_GSM == pstPrioRatList->aucRatPrio[i]) )
            {
                continue;
            }

            /* 依次查找所有支持的接入技术 */
            if (VOS_FALSE == NAS_MMC_GetAllBandSearch_PlmnSelection(pstPrioRatList->aucRatPrio[i]))
            {
                pstDestPlmn->stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
                pstDestPlmn->stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
                pstDestPlmn->enRat          = pstPrioRatList->aucRatPrio[i];

                NAS_MMC_SetCurrSearchingPlmn_PlmnSelection(pstDestPlmn);

                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_VOID NAS_MMC_GetNextSearchPlmnFail_PlmnSelection(VOS_VOID)
{
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 如果是用户指定搜网，向上层回复搜网失败 */
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

        
        /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
         NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {

            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif
    }

#if   (FEATURE_ON == FEATURE_LTE)
    /* 判断是否需要重新驻留LTE网络 */
    if (VOS_TRUE == NAS_MMC_IsReCampLtePlmnValid_PlmnSelection())
    {
        NAS_MMC_ReCampLte_PlmnSelection();
    }
    else
#endif
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出层二状态机 */
        NAS_MMC_FSM_QuitFsmL2();
    }

    return;
}


VOS_VOID NAS_MMC_ProcPlmnSearchNoRfFail_PlmnSelection(VOS_VOID)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList     = VOS_NULL_PTR;
    NAS_MMC_RAT_SEARCH_INFO_STRU                           *pstSearchRatInfo = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    pstSearchRatInfo   = NAS_MMC_GetRatCoverage_PlmnSelection();
    pstPlmnSelectionList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();
    
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 设置NO RF存在标记信息 */
    NAS_MMC_SetCurrRatPlmnSrchNoRfFlg(NAS_MML_GetCurrNetRatType(), VOS_TRUE);

    /* NO RF失败，需保存当前的选网状态机列表信息 */
    NAS_MMC_SetPlmnSelectionListType(NAS_MMC_BACKUP_PLMN_SELECTION_LIST);
    PS_MEM_CPY(NAS_MMC_GetHighPrioPlmnList(), pstPlmnSelectionList, sizeof(NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU));

    /* 如果是用户指定搜网，向上层回复搜网失败 */
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

        
        /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
         NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {

            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif
    }

    /* 向上层状态机指示选网失败,按照丢网处理:启动AVAILABLE定时器 */
    NAS_MMC_SndPlmnSelectionRslt(NAS_MMC_PLMN_SELECTION_FAIL, VOS_FALSE, 
                                 pstSearchRatInfo, 0);

    /* 退出层二状态机 */
    NAS_MMC_FSM_QuitFsmL2();

    return;
}

#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID NAS_MMC_SndLmmSpecPlmnSearchReq_PlmnSelection(
    NAS_MML_PLMN_LIST_WITH_RAT_STRU    *pstDestPlmnList,
    VOS_UINT32                          ulUserSpecFlag
)
{
    NAS_MMC_NCELL_SEARCH_INFO_STRU                         *pstNcellSearchInfo = VOS_NULL_PTR;
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    NAS_MML_PLMN_WITH_RAT_STRU                             *pstDestPlmn = VOS_NULL_PTR;

    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    PS_MEM_SET(&stSndEquPlmnInfo, 0, sizeof(stSndEquPlmnInfo));

    pstNcellSearchInfo = NAS_MMC_GetNcellSearchInfo();
    enPlmnSearchScene  = NAS_MMC_GetPlmnSearchScene_PlmnSelection();

    pstDestPlmn = &(pstDestPlmnList->astPlmnId[0]);

    /* ncell搜网场景 */
    if ((VOS_TRUE == NAS_MMC_IsInterPlmnSearch_PlmnSelection())
     && ((NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_NCELL_SEARCH == enPlmnSearchScene)
      || (NAS_MMC_PLMN_SEARCH_SCENE_REG_NCELL == enPlmnSearchScene)))
    {
        if (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_NCELL_SEARCH == enPlmnSearchScene)
        {
            /* 构造ncell搜网等效plmn网络为modem1的等效plmn+modem0等效EHPLMN+modem0的RPLMN */
            NAS_MMC_BuildNcellPlmnSearchEquPlmnInfo_PlmnSelection(&stSndEquPlmnInfo, pstDestPlmn->enRat);
        }
        else
        {
            stSndEquPlmnInfo.ucEquPlmnNum = NAS_MML_MIN(NAS_MML_MAX_EQUPLMN_NUM, pstNcellSearchInfo->stOtherModemEplmnInfo.ucEquPlmnNum);
            PS_MEM_CPY(stSndEquPlmnInfo.astEquPlmnAddr, pstNcellSearchInfo->stOtherModemEplmnInfo.astEquPlmnAddr,
                       (stSndEquPlmnInfo.ucEquPlmnNum*sizeof(NAS_MML_PLMN_ID_STRU)));
        }

        /* 删除禁止网络 */
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmNcellSpecPlmnSearchReq(&pstNcellSearchInfo->stLteNcellInfo,
                                     &stSndEquPlmnInfo);

    }
    else
    {
        NAS_MMC_SndLmmSpecPlmnSearchReq(pstDestPlmnList, ulUserSpecFlag);
    }

}

#endif
VOS_VOID NAS_MMC_BuildNcellPlmnSearchEquPlmnInfo_PlmnSelection(
    NAS_MML_EQUPLMN_INFO_STRU          *pstOutEplmnInfo,
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType
)
{
    MMA_MMC_PLMN_ID_STRU                astEquPlmnAddr[MMA_MMC_MAX_EQUPLMN_NUM];
    NAS_MMC_NCELL_SEARCH_INFO_STRU     *pstNcellSearchInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulIndex;
    VOS_UINT8                           ucSndEplmnNum;
    NAS_MML_PLMN_ID_STRU                stRPlmnId;
    VOS_UINT32                          ulRplmnExistFlg;
    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstSimEHPlmnInfo = VOS_NULL_PTR;

    PS_MEM_SET(astEquPlmnAddr, 0, sizeof(astEquPlmnAddr));
    PS_MEM_SET(&stRPlmnId, 0, sizeof(stRPlmnId));
    ucSndEplmnNum      = 0;
    ulIndex            = 0;
    pstSimEHPlmnInfo   = NAS_MML_GetSimEhplmnList();
    ulRplmnExistFlg    = VOS_FALSE;
    pstNcellSearchInfo = NAS_MMC_GetNcellSearchInfo();

    if (NAS_MML_MAX_EQUPLMN_NUM < pstNcellSearchInfo->stOtherModemEplmnInfo.ucEquPlmnNum)
    {
        pstNcellSearchInfo->stOtherModemEplmnInfo.ucEquPlmnNum  = NAS_MML_MAX_EQUPLMN_NUM;
        
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_BuildNcellPlmnSearchEquPlmnInfo_PlmnSelection:other modem eplmn num incorrect!");
        
    }
    
    /* 等效plmn信息中加入modem1的等效plmn信息 */
    for (ulIndex = 0; ulIndex < pstNcellSearchInfo->stOtherModemEplmnInfo.ucEquPlmnNum; ulIndex++)
    {
        if (ucSndEplmnNum >= MMA_MMC_MAX_EQUPLMN_NUM)
        {
            pstOutEplmnInfo->ucEquPlmnNum = ucSndEplmnNum;
            PS_MEM_CPY(pstOutEplmnInfo->astEquPlmnAddr, astEquPlmnAddr, sizeof(pstOutEplmnInfo->astEquPlmnAddr));
            return;
        }

        astEquPlmnAddr[ucSndEplmnNum].ulMcc = pstNcellSearchInfo->stOtherModemEplmnInfo.astEquPlmnAddr[ulIndex].ulMcc;
        astEquPlmnAddr[ucSndEplmnNum].ulMnc = pstNcellSearchInfo->stOtherModemEplmnInfo.astEquPlmnAddr[ulIndex].ulMnc;
        ucSndEplmnNum++;
    }

    /* 等效plmn信息中加入modem0的RPLMN信息 */
#if (FEATURE_ON == FEATURE_LTE)
    if (NAS_MML_NET_RAT_TYPE_LTE == enRatType)
    {
        ulRplmnExistFlg = NAS_MML_GetLteRPlmn(&stRPlmnId);
    }
    else
#endif
    {
        ulRplmnExistFlg = NAS_MML_GetGURPlmn(&stRPlmnId);
    }

    if ((VOS_TRUE == ulRplmnExistFlg)
     && (ucSndEplmnNum < MMA_MMC_MAX_EQUPLMN_NUM))
    {
        astEquPlmnAddr[ucSndEplmnNum].ulMcc = stRPlmnId.ulMcc;
        astEquPlmnAddr[ucSndEplmnNum].ulMnc = stRPlmnId.ulMnc;
        ucSndEplmnNum++;
    }

    if (NAS_MML_MAX_EHPLMN_NUM < pstSimEHPlmnInfo->ucEhPlmnNum)
    {
        pstSimEHPlmnInfo->ucEhPlmnNum   = NAS_MML_MAX_EHPLMN_NUM;

        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_BuildNcellPlmnSearchEquPlmnInfo_PlmnSelection:other modem ehplmn num incorrect!");
    }

    /* 等效plmn信息中加入modem0的EHPLMN信息 */
    for (ulIndex = 0; ulIndex < pstSimEHPlmnInfo->ucEhPlmnNum; ulIndex++)
    {
        if (ucSndEplmnNum >= MMA_MMC_MAX_EQUPLMN_NUM)
        {
            pstOutEplmnInfo->ucEquPlmnNum = ucSndEplmnNum;
            PS_MEM_CPY(pstOutEplmnInfo->astEquPlmnAddr, astEquPlmnAddr, sizeof(pstOutEplmnInfo->astEquPlmnAddr));
            return;
        }

        astEquPlmnAddr[ucSndEplmnNum].ulMcc = pstSimEHPlmnInfo->astEhPlmnInfo[ulIndex].stPlmnId.ulMcc;
        astEquPlmnAddr[ucSndEplmnNum].ulMnc = pstSimEHPlmnInfo->astEhPlmnInfo[ulIndex].stPlmnId.ulMnc;
        ucSndEplmnNum++;
    }

    pstOutEplmnInfo->ucEquPlmnNum = ucSndEplmnNum;
    PS_MEM_CPY(pstOutEplmnInfo->astEquPlmnAddr, astEquPlmnAddr, sizeof(pstOutEplmnInfo->astEquPlmnAddr));

    return;
}
VOS_VOID NAS_MMC_SndAsSpecPlmnSearchReq_PlmnSelection(
    NAS_MML_PLMN_LIST_WITH_RAT_STRU                        *pstDestPlmnList,
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU               *pstForbRoamLaInfo
)
{
    NAS_MMC_NCELL_SEARCH_INFO_STRU                         *pstNcellSearchInfo = VOS_NULL_PTR;
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    NAS_MML_PLMN_WITH_RAT_STRU                             *pstDestPlmn = VOS_NULL_PTR;

    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    PS_MEM_SET(&stSndEquPlmnInfo, 0, sizeof(stSndEquPlmnInfo));

    pstNcellSearchInfo = NAS_MMC_GetNcellSearchInfo();
    enPlmnSearchScene  = NAS_MMC_GetPlmnSearchScene_PlmnSelection();
    pstDestPlmn        = &(pstDestPlmnList->astPlmnId[0]);

    if ((VOS_TRUE == NAS_MMC_IsInterPlmnSearch_PlmnSelection())
     && (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_NCELL_SEARCH == enPlmnSearchScene))
    {
        /* 构造ncell搜网等效plmn网络为modem1的等效plmn+modem0等效EHPLMN+modem0的RPLMN */
        NAS_MMC_BuildNcellPlmnSearchEquPlmnInfo_PlmnSelection(&stSndEquPlmnInfo, pstDestPlmn->enRat);

        /* 删除禁止网络 */
        NAS_MMC_BuildEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndAsNcellSpecPlmnSearchReq(&pstNcellSearchInfo->stTdsNcellInfo,
                                            &stSndEquPlmnInfo);
    }
    else
    {
        NAS_MMC_SndAsSpecPlmnSearchReq(pstDestPlmnList, pstForbRoamLaInfo);
    }


}
VOS_UINT32 NAS_MMC_IsNeedAddEhplmnWhenSearchRplmn_PlmnSelection(
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDestPlmn
)
{
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    NAS_MMC_PLMN_TYPE_ENUM_UINT8                            enPlmnType;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList = VOS_NULL_PTR;
    VOS_UINT32                                              ulEventType;
    VOS_UINT8                                               ucSearchRplmnAndHplmnFlg;

    enPlmnSearchScene                = NAS_MMC_GetPlmnSearchScene_PlmnSelection();
    pstPlmnSelectionList             = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();
    enPlmnType                       = NAS_MMC_GetPlmnTypeInPlmnSelectionList(pstDestPlmn, pstPlmnSelectionList);
    ulEventType                      = NAS_MMC_GetCurrFsmEventType();
    ucSearchRplmnAndHplmnFlg         = NAS_MMC_GetSearchRplmnAndHplmnFlg_PlmnSelection(pstDestPlmn->enRat);

    if ( (VOS_TRUE == NAS_MMC_IsRPlmnFirstSearchWithHPlmn())
      && (NAS_MMC_PLMN_TYPE_RPLMN == enPlmnType)
      && (VOS_FALSE == ucSearchRplmnAndHplmnFlg)
      && ( (ulEventType == NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_PLMN_SEARCH_REQ))
        || (NAS_MMC_PLMN_SEARCH_SCENE_AREA_LOST == enPlmnSearchScene) ) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}




VOS_VOID NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDestPlmn
)
{

    VOS_UINT32                                              ulUserSpecPlmnSearch;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU                stForbRoamLaInfo;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                        *pstForbPlmnInfo = VOS_NULL_PTR;
    VOS_UINT32                                              ulWaitWasPlmnSrchCnfTimerLen;
    NAS_MML_PLMN_LIST_WITH_RAT_STRU                         stDestPlmnList;
#if (FEATURE_ON == FEATURE_LTE)
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8                enLteForbiddenStatus;

    enLteForbiddenStatus = NAS_MML_GetLteForbiddenStatusFlg();
    
    /* 获取LTE能力状态 */
    enLCapabilityStatus  = NAS_MML_GetLteCapabilityStatus();
#endif

    ulWaitWasPlmnSrchCnfTimerLen = NAS_MMC_GetWaitWasPlmnSrchCnfTimerLen();
    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 获取是否用户指定搜网 */
    ulUserSpecPlmnSearch = NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType);
    if ( VOS_TRUE == ulUserSpecPlmnSearch )
    {
        /* 是用户指定搜网，从当前选网状态机中获取FORBIDEN ROAMING LAS信息 */
        PS_MEM_CPY( &stForbRoamLaInfo,
                    NAS_MMC_GetForbRoamLaInfo_PlmnSelection(),
                    sizeof(stForbRoamLaInfo) );
    }
    else
    {
        /* 非用户指定搜网，从全局变量中获取FORBIDEN ROAMING LAS信息 */
        pstForbPlmnInfo                  = NAS_MML_GetForbidPlmnInfo();
        stForbRoamLaInfo.ucForbRoamLaNum = pstForbPlmnInfo->ucForbRoamLaNum;
        PS_MEM_CPY( stForbRoamLaInfo.astForbRomLaList,
                    pstForbPlmnInfo->astForbRomLaList,
                    sizeof(pstForbPlmnInfo->astForbRomLaList) );
    }

    /* 状态更新为当前未驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    NAS_MMC_SetAsAnyCampOn(VOS_FALSE);

#if (FEATURE_ON == FEATURE_LTE)
    /* 如果当前已经通知LTE disable,此时存在LTE的国内网络需要enable LTE */

    if ((VOS_TRUE == NAS_MML_GetDisableLteRoamFlg())
     && (NAS_MML_NET_RAT_TYPE_LTE == pstDestPlmn->enRat)
     && (NAS_MML_RAT_CAPABILITY_STATUS_DISABLE != enLteForbiddenStatus))
    {
        /* 当前LTE能力恢复可用或者为默认值 */
        if ((NAS_MML_LTE_CAPABILITY_STATUS_BUTT == enLCapabilityStatus)
         || (NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS == enLCapabilityStatus))
        {
            /* 向WAS/GAS发送enable LTE通知消息 */
            NAS_MMC_SndAsLteCapabilityStatus(WUEPS_PID_WRR, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);
            NAS_MMC_SndAsLteCapabilityStatus(UEPS_PID_GAS, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);

            /* 向LMM发送enable LTE消息 */
            NAS_MMC_SndLmmEnableLteNotify();
        }

        if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER))
        {
            NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER);
        }


        /* 更新disable LTE能力标记 */
        NAS_MML_SetDisableLteRoamFlg(VOS_FALSE);
    }
#endif

    PS_MEM_SET(&stDestPlmnList, 0x0, sizeof(stDestPlmnList));

    stDestPlmnList.ulPlmnNum = 1;
    PS_MEM_CPY(&stDestPlmnList.astPlmnId[0], pstDestPlmn, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    if ( VOS_TRUE == NAS_MMC_IsNeedAddEhplmnWhenSearchRplmn_PlmnSelection(pstDestPlmn) )
    {
        NAS_MMC_AddEHPlmnInDestPlmnList(&stDestPlmnList);

        NAS_MMC_SetSearchRplmnAndHplmnFlg_PlmnSelection(pstDestPlmn->enRat, VOS_TRUE);
    }

    /* 根据不同的接入技术发送搜网请求 */
    switch (pstDestPlmn->enRat)
    {
#if (FEATURE_ON == FEATURE_LTE)
        case NAS_MML_NET_RAT_TYPE_LTE:
            NAS_MMC_SndLmmSpecPlmnSearchReq_PlmnSelection(&stDestPlmnList, ulUserSpecPlmnSearch);

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF_LEN);
            break;
#endif

        case NAS_MML_NET_RAT_TYPE_WCDMA:
            NAS_MMC_SndAsSpecPlmnSearchReq_PlmnSelection(&stDestPlmnList, &stForbRoamLaInfo);

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_PLMN_SEARCH_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF, ulWaitWasPlmnSrchCnfTimerLen);
            break;

        case NAS_MML_NET_RAT_TYPE_GSM:
            NAS_MMC_SndAsSpecPlmnSearchReq(&stDestPlmnList, &stForbRoamLaInfo);
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_PLMN_SEARCH_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_PLMN_SEARCH_CNF, TI_NAS_MMC_WAIT_GAS_PLMN_SEARCH_CNF_LEN);
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection: Rat not support.");
            break;
    }


    return;
}
VOS_VOID NAS_MMC_SndSuspendReq_PlmnSelection(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat
)
{
    /* 状态更新为当前未驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

    NAS_MMC_SetAsAnyCampOn(VOS_FALSE);

    /* 根据不同的接入技术设置等待搜网回复的状态 */
    switch (enRat)
    {
#if (FEATURE_ON == FEATURE_LTE)
        case NAS_MML_NET_RAT_TYPE_LTE:
            NAS_MMC_SndLmmSuspendReq();
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_SUSPEND_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_SUSPEND_CNF, TI_NAS_MMC_WAIT_LMM_SUSPEND_CNF_LEN);
            break;
#endif

        case NAS_MML_NET_RAT_TYPE_WCDMA:
            NAS_MMC_SndAsSuspendReq(WUEPS_PID_WRR);
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF, TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF_LEN);
            break;

        case NAS_MML_NET_RAT_TYPE_GSM:
            NAS_MMC_SndAsSuspendReq(UEPS_PID_GAS);
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_SUSPEND_CNF);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF, TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF_LEN);
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndSuspendReq_PlmnSelection: Rat not support.");
            break;
    }

    return;
}


VOS_VOID NAS_MMC_SndRelReq_PlmnSelection(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat
)
{
    /* 根据不同的接入技术分别处理 */
    switch (enRat)
    {
#if (FEATURE_ON == FEATURE_LTE)
        case NAS_MML_NET_RAT_TYPE_LTE:

            /* 向EMM发送释放请求 */
            NAS_MMC_SndLmmRelReq();
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);
            NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_TRUE);
            break;
#endif

        case NAS_MML_NET_RAT_TYPE_WCDMA:
        case NAS_MML_NET_RAT_TYPE_GSM:

            /* 向MM/GMM发送释放请求 */
            NAS_MMC_SndMmRelReq();
            NAS_MMC_SndGmmRelReq();

            NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_RRC_CONN_REL_IND);
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL, TI_NAS_MMC_WAIT_RRC_CONN_REL_LEN);
            NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_TRUE);
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRelReq_PlmnSelection: Rat not support.");
            break;
    }

    return;
}
VOS_VOID NAS_MMC_UpdateGUCoverageFlg_PlmnSelection(
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg
)
{
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat;
    NAS_MMC_COVERAGE_TYPE_ENUM_UINT8    enCoverType;

    if (WUEPS_PID_WRR == pstSrchCnfMsg->MsgHeader.ulSenderPid)
    {
        enRat = NAS_MML_NET_RAT_TYPE_WCDMA;
    }
    else
    {
        enRat = NAS_MML_NET_RAT_TYPE_GSM;
    }

    if (RRC_PLMN_SEARCH_RLT_SPEC_FAIL == pstSrchCnfMsg->ulSearchRlt)
    {
        /* 将RRC的覆盖类型转换为NAS格式的覆盖类型 */
        NAS_MMC_ConvertRrcCoverTypeToNasFormat(pstSrchCnfMsg->enCoverageType, &enCoverType);

        /* 根据RRC的当前接入技术覆盖信息更新 */
        NAS_MMC_SetRatCoverage_PlmnSelection(enRat, enCoverType);
    }
    else
    {
        /* 更新当前接入技术存在覆盖 */
        if (NAS_MMC_COVERAGE_TYPE_BUTT == NAS_MMC_GetSpecRatCoverageTypeInRatSearchInfoList(
                                          NAS_MMC_GetRatCoverage_PlmnSelection(), enRat))
        {
            NAS_MMC_SetRatCoverage_PlmnSelection(enRat, NAS_MMC_COVERAGE_TYPE_LOW);
        }
    }

    return;
}

#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID NAS_MMC_UpdateLmmCoverageFlg_PlmnSelection(
    LMM_MMC_PLMN_SRCH_CNF_STRU         *pstLmmSrchCnfMsg
)
{
    NAS_MMC_COVERAGE_TYPE_ENUM_UINT8    enCoverType;

    if (MMC_LMM_PLMN_SRCH_RLT_SPEC_FAIL == pstLmmSrchCnfMsg->enRlst)
    {
        /* 将RRC的覆盖类型转换为NAS格式的覆盖类型 */
        NAS_MMC_ConvertLmmCoverTypeToNasFormat(pstLmmSrchCnfMsg->enCoverageType, &enCoverType);

        /* 根据RRC的当前接入技术覆盖信息更新 */
        NAS_MMC_SetRatCoverage_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, enCoverType);
    }
    else
    {
        /* 更新当前接入技术存在覆盖 */
        if (NAS_MMC_COVERAGE_TYPE_BUTT == NAS_MMC_GetSpecRatCoverageTypeInRatSearchInfoList(
                                          NAS_MMC_GetRatCoverage_PlmnSelection(), NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SetRatCoverage_PlmnSelection(NAS_MML_NET_RAT_TYPE_LTE, NAS_MMC_COVERAGE_TYPE_LOW);
        }
    }

    return;
}
VOS_UINT32 NAS_MMC_IsReCampLtePlmnValid_PlmnSelection(VOS_VOID)
{
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbidPlmnInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                                   *pstReCampLtePlmn  = VOS_NULL_PTR;
    NAS_MMC_PLMN_SELECTION_MODE_ENUM_UINT8                  enPlmnSelectionMode;

    pstReCampLtePlmn          = NAS_MMC_GetCsPsMode1ReCampLtePlmn_PlmnSelection();
    pstForbidPlmnInfo         = NAS_MML_GetForbidPlmnInfo();
    enPlmnSelectionMode       = NAS_MMC_GetPlmnSelectionMode();

    /* 当前不支持L时,重新回LTE的网络无效 */
    if ( VOS_FALSE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE) )
    {
        return VOS_FALSE;
    }
    
    if (VOS_TRUE == NAS_MML_IsRatInForbiddenList(NAS_MML_NET_RAT_TYPE_LTE))
    {
        return VOS_FALSE;
    }

    /* 状态机上下文中回LTE网络无效 */
    if (VOS_FALSE == NAS_MML_IsPlmnIdValid(pstReCampLtePlmn))
    {
        return VOS_FALSE;
    }

    /* PS卡无效 */
    if ( VOS_FALSE == NAS_MML_GetSimPsRegStatus() )
    {
        return VOS_FALSE;
    }

    /* PS卡不允许注册 */
    if ( VOS_FALSE == NAS_MML_GetPsAttachAllowFlg() )
    {
        return VOS_FALSE;
    }

    /* 回LTE网络在禁止列表中,对应的场景是该网络在GU注册时被原因值#11拒绝*/
    if (VOS_TRUE == NAS_MML_IsSimPlmnIdInDestBcchPlmnList(pstReCampLtePlmn,
                                          pstForbidPlmnInfo->ucForbPlmnNum,
                                          pstForbidPlmnInfo->astForbPlmnIdList))
    {
        return VOS_FALSE;
    }

    /* 回LTE网络在禁止GPRS列表中,对应的场景是该网络在GU注册时被原因值#14拒绝 */
    if (VOS_TRUE == NAS_MML_IsSimPlmnIdInDestBcchPlmnList(pstReCampLtePlmn,
                                          pstForbidPlmnInfo->ucForbGprsPlmnNum,
                                          pstForbidPlmnInfo->astForbGprsPlmnList))
    {
        return VOS_FALSE;
    }

    /* 自动选网模式该网络有效 */
    if (NAS_MMC_PLMN_SELECTION_MODE_AUTO == enPlmnSelectionMode)
    {
        return VOS_TRUE;
    }

    /* 手动选网模式时重回LTE网络是否允许 */
    if (VOS_TRUE == NAS_MMC_IsPlmnAllowedInManualMode(pstReCampLtePlmn))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID NAS_MMC_ReCampLte_PlmnSelection(VOS_VOID)
{
    NAS_MMC_SPEC_PLMN_SEARCH_STATE_ENUM_UINT8   enSpecPlmnSearchFlg;
    VOS_UINT32                                  ulEqual;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8             enRat;
    NAS_MML_PLMN_WITH_RAT_STRU                  stReCampLtePlmn;
    NAS_MML_PLMN_LIST_WITH_RAT_STRU     stDestPlmnList;
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32   enLteCapabilityStatus;

    enLteCapabilityStatus               = NAS_MML_GetLteCapabilityStatus();

    stReCampLtePlmn.enRat               = NAS_MML_NET_RAT_TYPE_LTE;
    enSpecPlmnSearchFlg                 = NAS_MMC_GetSpecPlmnSearchState();
    enRat                               = NAS_MML_GetCurrNetRatType();

    PS_MEM_CPY(&(stReCampLtePlmn.stPlmnId),
               NAS_MMC_GetCsPsMode1ReCampLtePlmn_PlmnSelection(),
               sizeof(stReCampLtePlmn.stPlmnId));

    ulEqual = NAS_MML_CompareBcchPlmnwithSimPlmn(&(stReCampLtePlmn.stPlmnId),
                                                  NAS_MML_GetCurrCampPlmnId());

    if (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLteCapabilityStatus)
    {
        /* 向WAS/GAS发送enable LTE通知消息 */
        NAS_MMC_SndAsLteCapabilityStatus(WUEPS_PID_WRR, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);
        NAS_MMC_SndAsLteCapabilityStatus(UEPS_PID_GAS, RRC_NAS_LTE_CAPABILITY_STATUS_REENABLE);

        /* 向LMM发送enable LTE消息 */
        NAS_MMC_SndLmmEnableLteNotify();
    }

    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER))
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_ENABLE_LTE_TIMER);
    }

    /* 更新disable LTE能力标记 */
    NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS);

    /* 当前已经驻留LTE网络,且该网络与需要重新驻留网络相同 */
    if ( (NAS_MML_NET_RAT_TYPE_LTE == enRat)
      && (NAS_MMC_SPEC_PLMN_SEARCH_STOP == enSpecPlmnSearchFlg)
      && (VOS_TRUE == ulEqual) )
    {
        /* 重新驻留LTE时将服务状态上报 */
        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS));
        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS));

        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 退出层二状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    /* 状态更新为当前未驻留 */
    NAS_MMC_SetAsCellCampOn(NAS_MMC_AS_CELL_NOT_CAMP_ON);
    NAS_MMC_SetSpecPlmnSearchState(NAS_MMC_SPEC_PLMN_SEARCH_RUNNING);


    switch (enRat)
    {
        /* suspend过程中detach PS了，导致获取下一个网络失败 */
        case NAS_MML_NET_RAT_TYPE_BUTT:
        case NAS_MML_NET_RAT_TYPE_LTE:

            /* 向MM、GMM发送搜网指示 */
            NAS_MMC_SndGmmPlmnSchInit();
            NAS_MMC_SndMmPlmnSchInit();

            NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

            NAS_MMC_UpdateRegStateSpecPlmnSearch();

            /* 向LMM发送指定搜网 */
            PS_MEM_SET(&stDestPlmnList, 0x0, sizeof(stDestPlmnList));

            stDestPlmnList.ulPlmnNum = 1;
            PS_MEM_CPY(&stDestPlmnList.astPlmnId[0], &stReCampLtePlmn, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

            NAS_MMC_SndLmmSpecPlmnSearchReq(&stDestPlmnList, VOS_FALSE);

            /* 状态迁移NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE */
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_LMM_PLMN_SEARCH_CNF_CSPSMODE1_RECAMP_LTE);

            /* 启动保护定时器TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF */
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF, TI_NAS_MMC_WAIT_LMM_PLMN_SEARCH_CNF_LEN);

            break;

        case NAS_MML_NET_RAT_TYPE_WCDMA:


            /* 向MM、GMM发送搜网指示 */
            NAS_MMC_SndGmmPlmnSchInit();
            NAS_MMC_SndMmPlmnSchInit();

            NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

            NAS_MMC_UpdateRegStateSpecPlmnSearch();

            /* 向WRR发送挂起请求 */
            NAS_MMC_SndAsSuspendReq(WUEPS_PID_WRR);

            /* 状态迁移NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_CSPSMODE1_RECAMP_LTE */
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_CSPSMODE1_RECAMP_LTE);

            /* 启动保护定时器TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF */
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF, TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF_LEN);

            break;

        case NAS_MML_NET_RAT_TYPE_GSM:


            /* 向MM、GMM发送搜网指示 */
            NAS_MMC_SndGmmPlmnSchInit();
            NAS_MMC_SndMmPlmnSchInit();

            NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

            NAS_MMC_UpdateRegStateSpecPlmnSearch();

            /* 向Gas发送挂起请求 */
            NAS_MMC_SndAsSuspendReq(UEPS_PID_GAS);

            /* 状态迁移NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_SUSPEND_CNF_CSPSMODE1_RECAMP_LTE */
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_GAS_SUSPEND_CNF_CSPSMODE1_RECAMP_LTE);

            /* 启动保护定时器TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF */
            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF, TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF_LEN);

            break;

        default:

            /* 异常打印 */
            NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_ReCampLte_PlmnSelection,ERROR:Rat Error");

            break;
    }

    return;
}

#endif  /* (FEATURE_ON == FEATURE_LTE) */



VOS_VOID NAS_MMC_UpdatePlmnSearchInfo_PlmnSelection(
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU                     *pstPlmnSearchReq,
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo
)
{
    VOS_UINT32                          i;

    for (i = 0; i < NAS_MML_MAX_RAT_NUM; i++)
    {
        if (NAS_MML_NET_RAT_TYPE_BUTT == pstPlmnSearchReq->astInterPlmnSearchInfo[i].enRatType)
        {
            continue;
        }

        /* 设置当前接入技术进行了全频搜网 */
        NAS_MMC_SetAllBandSearch_PlmnSelection(pstPlmnSearchReq->astInterPlmnSearchInfo[i].enRatType, VOS_TRUE);

        /* 更新当前接入技术的网络覆盖类型 */
        NAS_MMC_SetRatCoverage_PlmnSelection(pstPlmnSearchReq->astInterPlmnSearchInfo[i].enRatType,
                                             pstPlmnSearchReq->astInterPlmnSearchInfo[i].enCoverType);

        /* 自动搜网 */
        if (NAS_MMC_PLMN_SELECTION_MODE_AUTO == NAS_MMC_GetPlmnSelectionMode())
        {
            /* 更新完毕需要将剩余网络补充到选网列表 */
            NAS_MMC_UpdatePlmnListInPlmnSelectionList(&(pstPlmnSearchReq->astInterPlmnSearchInfo[i]),
                                                      pstPlmnSelectionListInfo,
                                                      VOS_TRUE);

            /*重新对高质量网络进行排序*/
            NAS_MMC_SortAvailPlmnInPlmnSelectionList(pstPlmnSelectionListInfo);
        }
        else
        {
            /* 更新完毕不需要将剩余网络补充到选网列表 */
            NAS_MMC_UpdatePlmnListInPlmnSelectionList(&(pstPlmnSearchReq->astInterPlmnSearchInfo[i]),
                                                      pstPlmnSelectionListInfo,
                                                      VOS_FALSE);
        }
    }

    /* 可维可测，输出选网列表信息 */
    NAS_MMC_LogPlmnSelectionList(pstPlmnSelectionListInfo);

    return;
}


VOS_VOID NAS_MMC_UpdatePlmnSearchList_PlmnSelection(
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU                    *pstInterPlmnSearchInfo
)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList = VOS_NULL_PTR;

    pstPlmnSelectionList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 自动搜网且搜网场景不为REG_HPLMN,也不为REG_PREF_PLMN,这两种搜网场景可以搜索的网络已经都在选网列表中了 */
    if (VOS_TRUE == NAS_MMC_IsNeedSearchAvailPlmn_PlmnSelection())
    {
        /* 更新完毕需要将剩余网络补充到选网列表 */
        NAS_MMC_UpdatePlmnListInPlmnSelectionList(pstInterPlmnSearchInfo,
                                                  pstPlmnSelectionList,
                                                  VOS_TRUE);

        /*重新对高质量网络进行排序*/
        NAS_MMC_SortAvailPlmnInPlmnSelectionList(pstPlmnSelectionList);
    }
    else
    {
        /* 更新完毕不需要将剩余网络补充到选网列表 */
        NAS_MMC_UpdatePlmnListInPlmnSelectionList(pstInterPlmnSearchInfo,
                                                  pstPlmnSelectionList,
                                                  VOS_FALSE);
    }

    /* 可维可测，输出选网列表信息 */
    NAS_MMC_LogPlmnSelectionList(pstPlmnSelectionList);

    return;
}
VOS_VOID NAS_MMC_RefreshPlmnSelectionListAfterRegFail_PlmnSelection(
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction
)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList = VOS_NULL_PTR;
    NAS_MMC_RAT_SEARCH_INFO_STRU                           *pstSearchRatInfo     = VOS_NULL_PTR;
    NAS_MMC_PLMN_TYPE_ENUM_UINT8                            enPlmnType;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurrPlmnWithRat;

#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                                              ulDisableLteByCsPsModeOne;

    ulDisableLteByCsPsModeOne = NAS_MMC_IsDisableLteNeedLocalReleaseEpsConn();
#endif


    pstPlmnSelectionList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();
    pstSearchRatInfo     = NAS_MMC_GetRatCoverage_PlmnSelection();

    stCurrPlmnWithRat.stPlmnId = (*NAS_MML_GetCurrCampPlmnId());
    stCurrPlmnWithRat.enRat    = NAS_MML_GetCurrNetRatType();

    /* 在选网列表中获取当前网络的类型 */
    enPlmnType = NAS_MMC_GetPlmnTypeInPlmnSelectionList(&stCurrPlmnWithRat, pstPlmnSelectionList);

    if ((NAS_MMC_ADDITIONAL_ACTION_PLMN_SELECTION == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_OPTIONAL_PLMN_SELECTION == enAdditionalAction))
    {
#if (FEATURE_ON == FEATURE_LTE)
        if ((NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS == NAS_MML_GetLteCapabilityStatus())
         && ( VOS_TRUE == ulDisableLteByCsPsModeOne))
        {
            NAS_MMC_RefreshPlmnSelectionList_DisableLte_CsPsMode1RegSuccess(pstPlmnSelectionList, pstSearchRatInfo);
        }
        /* 当前网络搜索网络类型为Rplmn或者Eplmn时才需要对选网列表中EPLMN进行调整 */
        else if ((NAS_MMC_PLMN_TYPE_RPLMN == enPlmnType)
         || (NAS_MMC_PLMN_TYPE_EPLMN == enPlmnType)
         || (NAS_MMC_PLMN_TYPE_USER_SEPC_PLMN == enPlmnType))
        {
            /* 需要进行PLMN SELECTION时,自动选网模式后续搜网应该优先搜索EHPLMN,
               手动选网模式只能搜索用户指定网络 */
            NAS_MMC_RefreshEPlmnInPlmnSelectionList(pstPlmnSelectionList);
        }
        else
        {
        }
#else
        if ((NAS_MMC_PLMN_TYPE_RPLMN == enPlmnType)
         || (NAS_MMC_PLMN_TYPE_EPLMN == enPlmnType)
         || (NAS_MMC_PLMN_TYPE_USER_SEPC_PLMN == enPlmnType))
        {
            /* 需要进行PLMN SELECTION时,自动选网模式后续搜网应该优先搜索EHPLMN,
               手动选网模式只能搜索用户指定网络 */
            NAS_MMC_RefreshEPlmnInPlmnSelectionList(pstPlmnSelectionList);
        }
#endif
    }
    else if (NAS_MMC_ADDITIONAL_ACTION_SEARCH_SUITABLE_CELL_IN_SAME_PLMN == enAdditionalAction)
    {
        /* 需要搜索当前网络其它位置区时,需要将当前网络所有未尝试注册的接入技术
           放在选网列表最前面,当前接入技术放在其它接入技术前面 */

        NAS_MMC_RefreshCurPlmnInPlmnSelectionList(pstPlmnSelectionList, pstSearchRatInfo, enPlmnType);
    }
    else
    {
    }

    /* 可维可测，输出选网列表信息 */
    NAS_MMC_LogPlmnSelectionList(pstPlmnSelectionList);

    return;
}
VOS_VOID NAS_MMC_SearchNextPlmn_PlmnSelection(VOS_VOID)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stDestPlmn;

	PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));

    /*  获取下个网络失败，则进行出服务区处理 */
    if (VOS_TRUE != NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
    {
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_PLMN_SELECTION_FAILURE,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);


        NAS_MMC_GetNextSearchPlmnFail_PlmnSelection();

        return;
    }

    /* 向GMM发送搜网指示 */
    NAS_MMC_SndGmmPlmnSchInit();

    /* 向MM发送搜网指示 */
    NAS_MMC_SndMmPlmnSchInit();

    NAS_MMC_SndMmaCampOnInd(VOS_FALSE);

    NAS_MMC_UpdateRegStateSpecPlmnSearch();

    /* 需要挂起当前模式，切换到等待挂起状态，启动保护定时器 */
    if (stDestPlmn.enRat == NAS_MML_GetCurrNetRatType())
    {
        /* 向接入层或LMM发送搜网请求，根据不同的搜网模式，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSpecPlmnSearchReq_PlmnSelection(&stDestPlmn);
    }
    else
    {
        /* 向接入层或LMM发送挂起请求，根据当前不同的接入技术，迁移到不同的L2状态，并启动保护定时器 */
        NAS_MMC_SndSuspendReq_PlmnSelection(NAS_MML_GetCurrNetRatType());
    }

    return;
}




VOS_VOID NAS_MMC_PerformAdditionalActionRegFinished_PlmnSelection()
{
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU                            *pstCurCampInfo           = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stCurPlmn;

    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;
    NAS_MML_PLMN_WITH_RAT_STRU                             *pstCurrSearchingPlmn = VOS_NULL_PTR;

    enPlmnSearchScene     = NAS_MMC_GetPlmnSearchScene_PlmnSelection();
    pstCurrSearchingPlmn  = NAS_MMC_GetCurrSearchingPlmn_PlmnSelection();

    /* 获取当前驻留的网络信息 */
    pstCurCampInfo     = NAS_MML_GetCurrCampPlmnInfo();
    stCurPlmn.stPlmnId = pstCurCampInfo->stLai.stPlmnId;
    stCurPlmn.enRat    = pstCurCampInfo->enNetRatType;

    /* 获取高优先级网络列表 */
    pstPlmnSelectionListInfo = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    /* 更新当前网络的搜索状态为已注册过，在此轮高优先级搜网中，不会再搜此网络 */
    NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                            NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                            pstPlmnSelectionListInfo);

    /* ncell搜网场景,系统消息上报的网络不一定是ncell搜的网络，所以刷新搜网列表时应该刷新之前搜索的plmn */
    if ((VOS_TRUE == NAS_MMC_IsInterPlmnSearch_PlmnSelection())
     && (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_NCELL_SEARCH == enPlmnSearchScene))
    {
        stCurPlmn.stPlmnId = pstCurrSearchingPlmn->stPlmnId;
        stCurPlmn.enRat    = pstCurrSearchingPlmn->enRat;
        NAS_MMC_UpdatePlmnNetStatusInPlmnSelectionList(&stCurPlmn,
                                            NAS_MMC_NET_STATUS_SEARCHED_REGISTERED,
                                            pstPlmnSelectionListInfo);
    }

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
    enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(enCsAdditionalAction, enPsAdditionalAction);

    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
        return;
    }

    /* 由于L和GU的注册结果处理均会调用此函数，故两个定时器都停一下 */
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_EPS_REG_IND);
    NAS_MMC_StopTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND);

    /* 附加操作为驻留当前网络 */
    if ((NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_CELL_SELECTION == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_ACCESS_BARRED == enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    /* 如果是NCELL搜网，不需要再搜索其他网络了，退出状态机 */
    if (NAS_MMC_PLMN_SEARCH_SCENE_REG_NCELL == NAS_MMC_GetPlmnSearchScene_PlmnSelection())
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    /* 信令链接存在，且当前需要搜网，则迁移到等待连接释放的状态 */
    if (VOS_TRUE == NAS_MML_IsRrcConnExist())
    {
#if (FEATURE_ON == FEATURE_LTE)
        if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
        {
            if (VOS_FALSE == NAS_MMC_IsDisableLteNeedWaitEpsConnRel_PlmnSelection())
            {
                return;
            }

            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_CONN_REL_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_CONN_REL_IND, TI_NAS_MMC_WAIT_EPS_CONN_REL_IND_LEN);
        }
        else
#endif
        {
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_RRC_CONN_REL_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_RRC_CONN_REL, TI_NAS_MMC_WAIT_RRC_CONN_REL_LEN);
        }

        return;
    }

    if ((NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL == enAdditionalAction))
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    /* 若enRegAdditionalAction为搜网，则根据类型更新列表 */
    NAS_MMC_RefreshPlmnSelectionListAfterRegFail_PlmnSelection(enAdditionalAction);

    NAS_MMC_SearchNextPlmn_PlmnSelection();

    return;
}
VOS_VOID NAS_MMC_PerformAdditionalActionConnRel_PlmnSelection(VOS_VOID)
{
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enCsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enPsAdditionalAction;

    /* 调用优先级比较表格得出CS/PS的处理优先级，并转换为状态机下一步行为 */
    enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
    enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();
    enAdditionalAction   = NAS_MMC_GetPrioAddtionalAction(enCsAdditionalAction, enPsAdditionalAction);

    /* 如果附加操作为继续等待,表示等待连接过程中又接收到了注册结果,此时将状态迁回到等待注册结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT == enAdditionalAction)
    {
#if (FEATURE_ON == FEATURE_LTE)
        if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
        {
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_EPS_REG_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_EPS_REG_IND, TI_NAS_MMC_WAIT_EPS_REG_IND_LEN);
        }
        else
#endif
        {
            NAS_MMC_FSM_SetCurrState(NAS_MMC_PLMN_SELECTION_STA_WAIT_CSPS_REG_IND);

            NAS_MMC_StartTimer(TI_NAS_MMC_WAIT_CSPS_REG_IND, TI_NAS_MMC_WAIT_CSPS_REG_IND_LEN);
        }

        return;
    }

    /* 附加操作为驻留当前网络 */
    if ((NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_CELL_SELECTION == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_ACCESS_BARRED == enAdditionalAction))
    {
        /* 回复状态机退出成功 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_SUCC);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    if ((NAS_MMC_ADDITIONAL_ACTION_CSPS_ANYCELL == enAdditionalAction)
     || (NAS_MMC_ADDITIONAL_ACTION_SINGLE_DOMAIN_ANYCELL == enAdditionalAction))
    {
        /* 向上层状态机指示选网失败 */
        NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

        /* 直接退出当前状态机 */
        NAS_MMC_FSM_QuitFsmL2();

        return;
    }

    /* 若enRegAdditionalAction为搜网，则根据类型更新列表 */
    NAS_MMC_RefreshPlmnSelectionListAfterRegFail_PlmnSelection(enAdditionalAction);

    NAS_MMC_SearchNextPlmn_PlmnSelection();

    return;
}




VOS_UINT32 NAS_MMC_ProcUserSearchCsRegRslt_PlmnSelection(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsCause
)
{
    VOS_UINT32                          ulRlst;

    ulRlst = VOS_FALSE;

    /* 只要有一个域注册成功，向MMA发送用户指定搜网成功 */
    if (NAS_MML_REG_FAIL_CAUSE_NULL == enCsCause)
    {

        /* 设置当前注册状态为成功 */
        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS);

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif

        return VOS_TRUE;
    }

    /*
       注册失败时
       1、CS域首先注册失败，原因值是#2,#3,#6，根据PS注册结果通知TAF搜网结果
       2、某一个域中注册失败原因值是#11,#12,#13，通知TAF搜网失败
       3、CS Other Cause，继续等待PS的注册结果
    */

    switch(enCsCause)
    {
        /* 注册失败原因值是#11,#12,#13，通知TAF指定搜网失败 */
        case NAS_MML_REG_FAIL_CAUSE_PLMN_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_LA_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_AUTH_REJ:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif

            ulRlst = VOS_TRUE;
            break;

        /* 其它原因值，暂不向MMA发送用户指定搜网结果，继续等待PS域的注册结果 */
        default:
            break;
    }
    return ulRlst;
}
VOS_UINT32 NAS_MMC_ProcUserSearchPsRegRslt_PlmnSelection(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsCause
)
{
    VOS_UINT32                          ulRlst;

    ulRlst = VOS_FALSE;

    /* 只要有一个域注册成功，通知用户搜网成功, CAUSE_NULL表示注册成功 */
    if (NAS_MML_REG_FAIL_CAUSE_NULL == enPsCause)
    {

        /* 设置当前注册状态为成功 */
        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        /* 上报成功，更新当前搜网类型为手动搜网 */
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS);

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif

        return VOS_TRUE;
    }

    /*  注册失败时
        1、PS域注册失败，原因值是#3,#6，进入无卡状态，通知TAF搜网失败
        2、某一个域中注册失败原因值是#11,#12,#13，通知TAF搜网失败
        3、PS注册失败#8，通知TAF搜网失败
        4、PS首先注册失败#7，继续等待CS的注册结果
        5、PS首先注册失败#7，继续等待CS的注册结果
        6、注册结果是#9,#10继续等待PS注册结果;如果PS注册结果成功，返回成功；如果PS注册失败，返回搜网失败
    */

    switch(enPsCause)
    {
        /* 原因值#3,#6,#8的处理 */
        case NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS:
        case NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME:
        case NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_AND_NON_GPRS_SERV_NOT_ALLOW:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif

            ulRlst = VOS_TRUE;
            break;

        /* 注册失败原因值是#11,#12,#13，通知TAF指定搜网失败 */
        case NAS_MML_REG_FAIL_CAUSE_PLMN_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_LA_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_AUTH_REJ:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif

            ulRlst = VOS_TRUE;
            break;

        /* PS注册失败原因值#7,#9,#10,#14继续等待CS注册结果 */
        case NAS_MML_REG_FAIL_CAUSE_MS_ID_NOT_DERIVED:
        case NAS_MML_REG_FAIL_CAUSE_IMPLICIT_DETACHED:
        case NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN:
            break;

        /* 其它原因值的处理 ,等待CS的处理结果 */
        default:
            break;
    }

    return ulRlst;
}


VOS_UINT32 NAS_MMC_ProcUserSearchRegCause15_PlmnSelection(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsCause,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsCause
)
{
    VOS_UINT32                          ulRlst;

    ulRlst = VOS_FALSE;

    /*  PS注册15号原因值时
        1、CS注册失败原因值是#11,#12,#13，通知TAF搜网失败
        2、CS注册失败其它原因值继续搜网注册

        CS注册15号原因值时
        1、PS域注册失败，原因值是#3,#6，进入无卡状态，通知TAF搜网失败
        2、PS注册失败原因值是#11,#12,#13，通知TAF搜网失败
        3、PS注册失败#8，通知TAF搜网失败
        4、PS注册失败其它原因值继续搜网注册
    */

    switch (enCsCause)
    {
        /* 注册失败原因值是#11,#12,#13，通知TAF指定搜网失败 */
        case NAS_MML_REG_FAIL_CAUSE_PLMN_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_LA_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_AUTH_REJ:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif

            ulRlst = VOS_TRUE;
            break;

        /* 其它原因值，暂不向MMA发送用户指定搜网结果，继续下次搜网注册 */
        default:
            break;
    }

    switch (enPsCause)
    {
        /* 原因值#3,#6,#8的处理 */
        case NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS:
        case NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME:
        case NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_AND_NON_GPRS_SERV_NOT_ALLOW:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);
#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif
            ulRlst = VOS_TRUE;
            break;

        /* 注册失败原因值是#11,#12,#13，通知TAF指定搜网失败 */
        case NAS_MML_REG_FAIL_CAUSE_PLMN_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_LA_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW:
        case NAS_MML_REG_FAIL_CAUSE_AUTH_REJ:

            NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
            /* 向LMM发送用户搜网结束通知 */
            if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
            {
                NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
            }
#endif

            ulRlst = VOS_TRUE;
            break;

        /* 其它原因值，暂不向MMA发送用户指定搜网结果，继续下次搜网注册 */
        default:
            break;
    }

    return ulRlst;
}


VOS_UINT32 NAS_MMC_ProcUserSearchRegOtherCause_PlmnSelection(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsCause,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsCause
)
{

    /* 只要有一个域注册成功，通知用户搜网成功, CAUSE_NULL表示注册成功 */
    if ((NAS_MML_REG_FAIL_CAUSE_NULL == enCsCause)
     || (NAS_MML_REG_FAIL_CAUSE_NULL == enPsCause))
    {

        /* 设置当前注册状态为成功 */
        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        /* 上报成功，更新当前搜网类型为手动搜网 */
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS);

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif

        return VOS_TRUE;
    }

    /* 如果设置为CS ONLY或者PS ONLY,另一个域当前小区不支持时,需要搜索列表中下一
       个网络后回复指定搜网结果 */
    if ((NAS_MML_REG_FAIL_CAUSE_MS_CFG_DOMAIN_NOT_SUPPORT == enCsCause)
     && (NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT == enPsCause))
    {
        return VOS_FALSE;
    }

    if ((NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT == enCsCause)
     && (NAS_MML_REG_FAIL_CAUSE_MS_CFG_DOMAIN_NOT_SUPPORT == enPsCause))
    {
        return VOS_FALSE;
    }

    /* PS注册失败原因值#9,#10继续等待PS注册结果 */
    if ((NAS_MML_REG_FAIL_CAUSE_MS_ID_NOT_DERIVED == enPsCause)
     || (NAS_MML_REG_FAIL_CAUSE_IMPLICIT_DETACHED == enPsCause))
    {
        return VOS_FALSE;
    }

    NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
    /* 向LMM发送用户搜网结束通知 */
    if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
    {
        NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
    }
#endif

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsCause,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsCause
)
{
    /* Cause值为 NAS_MML_REG_FAIL_CAUSE_NULL 表示该域注册成功，
       Cause值为 NAS_MML_REG_FAIL_CAUSE_BUTT 表示该域还未收到注册结果 */

    /* 还未获取到注册结果,继续等待注册结果 */
    if ((NAS_MML_REG_FAIL_CAUSE_BUTT == enPsCause)
     && (NAS_MML_REG_FAIL_CAUSE_BUTT == enCsCause))
    {
        return VOS_FALSE;
    }

    /* 还未获取到PS注册结果，当前获取到CS注册结果, 处理CS的注册原因值 */
    if ( (NAS_MML_REG_FAIL_CAUSE_BUTT == enPsCause)
      && (NAS_MML_REG_FAIL_CAUSE_BUTT != enCsCause) )
    {
        return NAS_MMC_ProcUserSearchCsRegRslt_PlmnSelection(enCsCause);
    }

    /* 还未获取到CS注册结果，当前获取到PS注册结果，处理PS的注册原因值 */
    if ( (NAS_MML_REG_FAIL_CAUSE_BUTT == enCsCause)
      && (NAS_MML_REG_FAIL_CAUSE_BUTT != enPsCause) )
    {
        return NAS_MMC_ProcUserSearchPsRegRslt_PlmnSelection(enPsCause);
    }

    
    /* 两个域的注册结果都获取,CS或PS注册15号原因值时,对注册结果处理 */
    if ( (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL       == enCsCause)
      || (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL       == enPsCause)
      || (NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_LA_FOR_ROAM == enCsCause)
      || (NAS_MML_REG_FAIL_CAUSE_FORBIDDEN_LA_FOR_ROAM == enPsCause))
    {
        return NAS_MMC_ProcUserSearchRegCause15_PlmnSelection(enCsCause, enPsCause);
    }

    /* 两个域的注册结果都获取,CS或PS注册15号原因值时,对注册结果处理 */
    if ((NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE <= enCsCause)
     || (NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE <= enPsCause))
    {
        return NAS_MMC_ProcUserSearchRegOtherCause_PlmnSelection(enCsCause, enPsCause);
    }

    /* 只要有一个域注册成功，向MMA发送用户指定搜网成功 */
    if ((NAS_MML_REG_FAIL_CAUSE_NULL == enCsCause)
     || (NAS_MML_REG_FAIL_CAUSE_NULL == enPsCause))
    {

        /* 设置当前注册状态为成功 */
        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_SUCCESS);

#if   (FEATURE_ON == FEATURE_LTE)
        /* 向LMM发送用户搜网结束通知 */
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
#endif

        return VOS_TRUE;
    }

    /* 已获取到CS和PS的注册结果，且有明确原因值，非other cause和15,上报失败 */
    NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

#if   (FEATURE_ON == FEATURE_LTE)
    /* 向LMM发送用户搜网结束通知 */
    if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
    {
        NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
    }
#endif

    return VOS_TRUE;
}



VOS_VOID NAS_MMC_LoadInterSysFsm_PlmnSelection(
    MMC_SUSPEND_CAUSE_ENUM_UINT8        enSuspendCause
)
{
    switch ( enSuspendCause )
    {
        /* 启动HANDOVER状态机 */
        case MMC_SUSPEND_CAUSE_HANDOVER:
            NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_INTER_SYS_HO);
            break;

        /* 启动 CCO状态机 */
        case MMC_SUSPEND_CAUSE_CELLCHANGE:
            NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_INTER_SYS_CCO);
            break;

        /* 启动 小区重选 状态机 */
        case MMC_SUSPEND_CAUSE_CELLRESELECT:
        case MMC_SUSPEND_CAUSE_REDIRECTION:
            NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_INTER_SYS_CELLRESEL);
            break;

        /* 启动 系统出服务区搜网 状态机 */
        case MMC_SUSPEND_CAUSE_PLMNSEARCH:
            NAS_MMC_FSM_InitFsmL2(NAS_MMC_FSM_INTER_SYS_OOS);
            break;

        default:
            NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_LoadInterSysFsm_PlmnSelection():default case");
            break;
    }

}


VOS_VOID NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection( VOS_VOID )
{
    MMA_MMC_PLMN_SEARCH_REQ_STRU        stPlmnSrhReg;
    VOS_UINT32                          ulBuildEventType;

    stPlmnSrhReg.MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    stPlmnSrhReg.MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    stPlmnSrhReg.MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    stPlmnSrhReg.MsgHeader.ulReceiverPid     = WUEPS_PID_MMC;
    stPlmnSrhReg.MsgHeader.ulLength          = sizeof(MMA_MMC_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    stPlmnSrhReg.MsgHeader.ulMsgName         = ID_MMA_MMC_PLMN_SEARCH_REQ;

    ulBuildEventType  = NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_PLMN_SEARCH_REQ);

    /*设置当前入口消息为开机搜网*/
    NAS_MMC_SaveCurEntryMsg(ulBuildEventType, (struct MsgCB*)&stPlmnSrhReg);

}
VOS_VOID  NAS_MMC_UpdateUserSpecPlmnSearchInfo_PlmnSelection(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT8                           ucAccessMode
)
{
    NAS_MML_PLMN_WITH_RAT_STRU          stUserSpecPlmn;

    /* 保存用户指定的PlmnID及其接入技术 */
    stUserSpecPlmn.stPlmnId.ulMcc = pstPlmnId->ulMcc;
    stUserSpecPlmn.stPlmnId.ulMnc = pstPlmnId->ulMnc;

    stUserSpecPlmn.enRat = ucAccessMode;

    NAS_MMC_SetUserSpecPlmnId(&stUserSpecPlmn);

    /* 保存指定搜网前驻留的PlmnID及接入技术 */
    NAS_MMC_SaveUserReselPlmnInfo();


    /* 当前为指定搜，则初始化选网状态机的禁止漫游LA信息变量 */
    PS_MEM_SET( NAS_MMC_GetForbRoamLaInfo_PlmnSelection(),
                0X0, sizeof(NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU) );

    PS_MEM_SET(NAS_MMC_GetForbPlmnForGrpsInfo_PlmnSelection(),
               0X0, sizeof(NAS_MML_PLMN_ID_STRU));

    /* 设置当前注册状态未注册 */
    NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_FALSE);

    /* 当前模式已是MANUAL，则不更新NV */
    if (NAS_MMC_PLMN_SELECTION_MODE_MANUAL == NAS_MMC_GetPlmnSelectionMode())
    {
        return;
    }

    /* 设置当前搜网模式为手动模式 */
    NAS_MMC_SetPlmnSelectionMode(NAS_MMC_PLMN_SELECTION_MODE_MANUAL);


    /* 如果高优先级搜网定时器在运行过程中，则停止该定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_HPLMN_TIMER))
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_HPLMN_TIMER);

        /* HPLMN TIMER 停止事件上报 */
        NAS_EventReport(WUEPS_PID_MMC, NAS_OM_EVENT_HPLMN_TIMER_STOP,
                        VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
    }

    /* 如果周期高优先级搜网定时器在运行过程中，则停止该定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_PERIOD_TRYING_HIGH_PRIO_PLMN_SEARCH))
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_PERIOD_TRYING_HIGH_PRIO_PLMN_SEARCH);
    }

    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_MMC_HIGH_PRIO_RAT_HPLMN_TIMER))
    {
        NAS_MMC_StopTimer(TI_NAS_MMC_HIGH_PRIO_RAT_HPLMN_TIMER);
        
        NAS_MMC_ResetCurHighPrioRatHplmnTimerFirstSearchCount_L1Main();
    }

    /* 将当前搜网模式写入NVIM中 */
    NAS_MMC_WritePlmnSelectionModeNvim();

    /* 搜网模式变化上报USIM */
    if (VOS_TRUE == NAS_MMC_IsNeedSndStkSearchModeChgEvt())
    {
        NAS_MMC_SndStkSearchModeChgEvt(NAS_MMC_PLMN_SELECTION_MODE_MANUAL);
    }

}
VOS_VOID NAS_MMC_ProcCsRegRslt_PlmnSelection(
    MMMMC_CS_REG_RESULT_IND_STRU       *pstCsRegRsltInd
)
{
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    NAS_MMC_SERVICE_ENUM_UINT8                              enService;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

#if (FEATURE_ON == FEATURE_LTE)
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU                        stActionRslt;
#endif

    enService = NAS_MMC_ConverMmStatusToMmc(NAS_MMC_REG_DOMAIN_CS,
                                            (NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8)pstCsRegRsltInd->ulServiceStatus);

    NAS_MMC_SetCsServiceStatus(enService);

    if (MM_MMC_LU_RESULT_SUCCESS == pstCsRegRsltInd->enLuResult)
    {
        NAS_MMC_SetCsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_NULL);

        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        /* 根据注册结果更新注册信息表 */
        NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_CS, NAS_MML_REG_FAIL_CAUSE_NULL);

        /* 对Hplmn的Rej Lai信息的清除 */
        NAS_MMC_ClearHplmnRejDomainInfo(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_CS);

        /* 注册成功时，需要删除ForbPlmn,ForbLa,ForbGprs等信息 */
        NAS_MMC_DelForbInfo_GuRegRsltSucc(NAS_MMC_REG_DOMAIN_CS);

        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_UpdateCsRegStateCsRegSucc();

#if   (FEATURE_ON == FEATURE_LTE)

        /* 联合注册时不通知LMM此时LU的结果 */
        if ((VOS_TRUE  == NAS_MML_IsRegFailCauseNotifyLmm(pstCsRegRsltInd->enRegFailCause))
         && (VOS_FALSE == pstCsRegRsltInd->ucIsComBined))
        {
            stActionRslt.enProcType     = NAS_MML_PROC_LAU;
            stActionRslt.enRegRst       = NAS_MML_REG_RESULT_SUCCESS;
            stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
            stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
            stActionRslt.enCnCause      = NAS_MML_REG_FAIL_CAUSE_NULL;
            stActionRslt.ulAttemptCount = 0;

            NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
        }
#endif

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_CS, VOS_TRUE, pstCsRegRsltInd->enRegFailCause);
    }
    else
    {
        enAdditionalAction = NAS_MMC_ProcCsRegFail(MM_MMC_CS_REG_RESULT_IND, pstCsRegRsltInd);


        /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
           当用户指定搜网时，则需要把禁止LA带给接入层 */
        enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
        if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
        {
            NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
        }
        ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
        if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
          && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstCsRegRsltInd->enRegFailCause) )
        {
            NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                                   NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
        }

        /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
           以便于用户指定搜网回复结果 */
        if ((NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enAdditionalAction)
         && (NAS_MMC_ADDITIONAL_ACTION_BUTT             != enAdditionalAction))
        {
            NAS_MMC_SetCsRegCause_PlmnSelection(pstCsRegRsltInd->enRegFailCause);
        }

        if ( VOS_TRUE == NAS_MML_IsNetworkRegFailCause(pstCsRegRsltInd->enRegFailCause) )
        {
            /* 只在跟网测真实发生交互的时候才更新注册信息表 */
            NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_CS, pstCsRegRsltInd->enRegFailCause);
        }

        /* PS注册成功, CS注册失败, 原因为11 12 13 15的情况, 需要清除该标志.
           因为此时CS的 RPLMN没有更新为用户指定的PLMN, 此后丢网等触发的搜网不能搜
           RPLMN及其EPLMN, 故需要清除该标志. */

        NAS_MMC_UpdateUserSpecPlmnRegisterStatusWhenRegFail(pstCsRegRsltInd->enRegFailCause);


#if   (FEATURE_ON == FEATURE_LTE)

        /* 小于 NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE的拒绝原因是UE尝试发起注册的原因值，需要通知LMM */
        /* 联合注册时不通知LMM此时LU的结果 */
        if ((VOS_TRUE  == NAS_MML_IsRegFailCauseNotifyLmm(pstCsRegRsltInd->enRegFailCause))
         && (VOS_FALSE == pstCsRegRsltInd->ucIsComBined))
        {
            /* 当前SYSCFG中L不在时，需要向LMM通知 */
            stActionRslt.enProcType     = NAS_MML_PROC_LAU;
            stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
            stActionRslt.enReqDomain    = NAS_MMC_REG_DOMAIN_CS;
            stActionRslt.enRstDomain    = NAS_MMC_REG_DOMAIN_CS;
            stActionRslt.enCnCause      = pstCsRegRsltInd->enRegFailCause;
            stActionRslt.ulAttemptCount = pstCsRegRsltInd->ulLuAttemptCnt;

            NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
        }
#endif
    }


    /* AdditionalAction有效，更新CS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndGuAsEquPlmnReq(&stSndEquPlmnInfo, NAS_MML_GetCurrNetRatType());
        NAS_MMC_SndOmEquPlmn();

        NAS_MMC_SndMmaEplmnInfoInd(&stSndEquPlmnInfo);
    }

    return;
}


VOS_VOID NAS_MMC_ProcPsRegRslt_PlmnSelection(
    GMMMMC_PS_REG_RESULT_IND_STRU      *pstPsRegRsltInd
)
{
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enAdditionalAction;
    NAS_MMC_SERVICE_ENUM_UINT8                              enService;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;


    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;

    NAS_MML_PLMN_ID_STRU                                   *pstForbPlmnForGprs = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                                   *pstCurrCampOnPlmn = VOS_NULL_PTR;

#if (FEATURE_ON == FEATURE_LTE)

    NAS_MMC_GU_ACTION_RSLT_INFO_STRU                        stActionRslt;
    NAS_MML_PROC_TYPE_ENUM_U32                              enProcType;

    /* 转换发送给L的proctype */
    enProcType = NAS_MML_PROC_BUTT;

    NAS_MMC_ConverGmmActionTypeToMml(pstPsRegRsltInd->enGmmActionType, &enProcType);


    NAS_MMC_ConvertGmmRegDomainToMmcDomain(pstPsRegRsltInd->enReqDomain, &stActionRslt.enReqDomain);

    NAS_MMC_ConvertGmmRegDomainToMmcDomain(pstPsRegRsltInd->enRsltDomain, &stActionRslt.enRstDomain);



#endif

    enService = NAS_MMC_ConverMmStatusToMmc(NAS_MMC_REG_DOMAIN_PS,
                                            (NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8)pstPsRegRsltInd->ulServiceStatus);

    NAS_MMC_SetPsServiceStatus(enService);

    if (GMM_MMC_ACTION_RESULT_SUCCESS == pstPsRegRsltInd->enActionResult)
    {
        /* 保存注册结果消息 */
        NAS_MMC_SaveRegRsltCtx(GMM_MMC_PS_REG_RESULT_IND, pstPsRegRsltInd);
    
        NAS_MMC_SetPsRegCause_PlmnSelection(NAS_MML_REG_FAIL_CAUSE_NULL);
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        /* 根据注册结果更新注册信息表 */
        NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_PS, NAS_MML_REG_FAIL_CAUSE_NULL);
        NAS_MMC_ClearHplmnRejDomainInfo(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_PS);

        /* 注册成功时，需要删除ForbPlmn,ForbLa,ForbGprs等信息 */
        NAS_MMC_DelForbInfo_GuRegRsltSucc(NAS_MMC_REG_DOMAIN_PS);

        NAS_MMC_NotifyModeChange(NAS_MML_GetCurrNetRatType(),
                                 NAS_MML_GetSimPsRegStatus());


        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_UpdatePsRegStatePsRegSucc();

#if   (FEATURE_ON == FEATURE_LTE)

        /* 假流程不通知给LMM */
        if ( VOS_TRUE == NAS_MML_IsRegFailCauseNotifyLmm(pstPsRegRsltInd->enRegFailCause) )
        {
            stActionRslt.enProcType     = enProcType;
            stActionRslt.enRegRst       = NAS_MML_REG_RESULT_SUCCESS;
            stActionRslt.enCnCause      = NAS_MML_REG_FAIL_CAUSE_NULL;
            stActionRslt.ulAttemptCount = 0;

            NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
        }

#endif

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_PS, VOS_TRUE, pstPsRegRsltInd->enRegFailCause);
    }
    else
    {
        enAdditionalAction = NAS_MMC_ProcPsRegFail(GMM_MMC_PS_REG_RESULT_IND,
                                                   pstPsRegRsltInd);


        /* 若被拒原因值为#15, 则添加禁止网络信息到选网状态机的禁止漫游LA变量里去,
           当用户指定搜网时，则需要把禁止LA带给接入层 */
        enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
        if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
        {
            NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
        }
        ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
        if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
          && (NAS_MML_REG_FAIL_CAUSE_NO_SUITABL_CELL == pstPsRegRsltInd->enRegFailCause) )
        {
            NAS_MML_AddForbRoamLa( NAS_MML_GetCurrCampLai(),
                                   NAS_MMC_GetForbRoamLaInfo_PlmnSelection() );
        }

        if ( (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
          && (NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN == pstPsRegRsltInd->enRegFailCause) )
        {
            pstForbPlmnForGprs = NAS_MMC_GetForbPlmnForGrpsInfo_PlmnSelection();
            pstCurrCampOnPlmn  = NAS_MML_GetCurrCampPlmnId();

            *pstForbPlmnForGprs = *pstCurrCampOnPlmn;
        }

        /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
           以便于用户指定搜网回复结果 */
        if ((NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enAdditionalAction)
         && (NAS_MMC_ADDITIONAL_ACTION_BUTT             != enAdditionalAction))
        {
            NAS_MMC_SetPsRegCause_PlmnSelection(pstPsRegRsltInd->enRegFailCause);
        }

        if ( VOS_TRUE == NAS_MML_IsNetworkRegFailCause(pstPsRegRsltInd->enRegFailCause) )
        {
            /* 只在跟网测真实发生交互的时候才更新注册信息表 */
            NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_PS, pstPsRegRsltInd->enRegFailCause);
        }

        NAS_MMC_UpdateUserSpecPlmnRegisterStatusWhenRegFail(pstPsRegRsltInd->enRegFailCause);

#if   (FEATURE_ON == FEATURE_LTE)

        /* 小于 NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE的拒绝原因是UE尝试发起注册的原因值，需要通知LMM */
        if ( VOS_TRUE == NAS_MML_IsRegFailCauseNotifyLmm(pstPsRegRsltInd->enRegFailCause) )
        {
            stActionRslt.enProcType     = enProcType;
            stActionRslt.enRegRst       = NAS_MML_REG_RESULT_FAILURE;
            stActionRslt.enCnCause      = pstPsRegRsltInd->enRegFailCause;
            stActionRslt.ulAttemptCount = pstPsRegRsltInd->ulRegCounter;

            NAS_MMC_SndLmmRegActionResultReq(&stActionRslt);
        }
#endif
    }

    /* AdditionalAction有效，更新PS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndGuAsEquPlmnReq(&stSndEquPlmnInfo, NAS_MML_GetCurrNetRatType());
        NAS_MMC_SndOmEquPlmn();

        NAS_MMC_SndMmaEplmnInfoInd(&stSndEquPlmnInfo);
    }

    return;
}
VOS_VOID  NAS_MMC_SndRslt_PlmnSelection(
    NAS_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32               enPlmnSelectionRslt
)
{

    NAS_MMC_RAT_SEARCH_INFO_STRU       *pstSearchRatInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstPrioRatList   = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulCampFlg;
    VOS_UINT32                          ulRatNum;

    MMA_MMC_PLMN_ID_STRU                stPlmnId;
    VOS_UINT32                          ulCurrentEventType;

    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;

    pstPrioRatList      = NAS_MML_GetMsPrioRatList();

    pstSearchRatInfo    = NAS_MMC_GetRatCoverage_PlmnSelection();

    ulRatNum            = pstPrioRatList->ucRatNum;

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();

    if (NAS_MMC_SPEC_PLMN_SEARCH_RUNNING == NAS_MMC_GetSpecPlmnSearchState())
    {
        ulCampFlg = VOS_FALSE;
    }
    else
    {
        ulCampFlg = VOS_TRUE;
    }

    stPlmnId.ulMcc     = NAS_MML_INVALID_MCC;
    stPlmnId.ulMnc     = NAS_MML_INVALID_MNC;
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();

    /* 如果是获取网络操作，发送获取网络失败回复消息 */
    if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MMC_ACQ_REQ) == ulCurrentEventType)
    {
        NAS_MMC_SndMmaAcqCnf(MMC_MMA_ACQ_RESULT_FAILURE, &stPlmnId, 0);
    }


    /* 增加手动搜网异常处理，直接回复搜网失败 */
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType))
    {
        NAS_MMC_SndMmaUserSpecPlmnSearchCnf(MMA_MMC_USER_PLMN_SEARCH_RESULT_FAIL);

        /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
        NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

    #if (FEATURE_ON == FEATURE_LTE)
        if ( VOS_TRUE == NAS_MML_IsNetRatSupported(NAS_MML_NET_RAT_TYPE_LTE))
        {
            NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
        }
    #endif

        NAS_MMC_SndRrMmCellSelReq(RRC_NAS_FORBLA_CHANGE);    
    }

    /* 如果是注册操作，发送注册回复消息 */
    if (VOS_TRUE == NAS_MMC_IsMmaRegReq_PlmnSelection())
    {
        if (VOS_TRUE == NAS_MMC_IsNormalServiceStatus())
        {
            NAS_MMC_SndMmaRegCnf(MMC_MMA_REG_RESULT_SUCCESS, NAS_MML_GetCurrCampPlmnId(),
                                 NAS_MML_GetCurrCampArfcn());
        }
        else
        {
            if (VOS_TRUE == ulCampFlg)
            {
                NAS_MMC_SndMmaRegCnf(MMC_MMA_REG_RESULT_REG_FAILURE, NAS_MML_GetCurrCampPlmnId(),
                                 NAS_MML_GetCurrCampArfcn());
            }
            else
            {
                NAS_MMC_SndMmaRegCnf(MMC_MMA_REG_RESULT_ACQ_FAILURE, &stPlmnId, 0x0);
            }
        }
    }

    /* 获取当前存在覆盖的网络模式 */
    for (i = 0; i < NAS_MML_MAX_RAT_NUM; i++)
    {
        /* 进行过全频搜网且当前没有覆盖 */
        if ((VOS_TRUE == NAS_MML_IsNetRatSupported(pstSearchRatInfo[i].enRatType))
         && (VOS_TRUE == pstSearchRatInfo[i].ucSearchAllBand)
         && (NAS_MMC_COVERAGE_TYPE_NONE == pstSearchRatInfo[i].enCoverageType))
        {
            ulRatNum--;
        }


        /* 增加异常保护 */
        if ( 0 == ulRatNum )
        {
            break;
        }
    }


    NAS_MMC_SndPlmnSelectionRslt(enPlmnSelectionRslt, ulCampFlg, pstSearchRatInfo, ulRatNum);

    return;
}

#if   (FEATURE_ON == FEATURE_LTE)
VOS_VOID  NAS_MMC_ProcLmmCombinedAttachInd_AttRsltSucc_PlmnSelection(
    LMM_MMC_ATTACH_IND_STRU                                *pstLmmAttachIndMsg,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegRsltCause,
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                   *penCsAddition,
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                   *penPsAddition
)
{
    if (VOS_TRUE == pstLmmAttachIndMsg->bitOpCnRslt)
    {
        if (MMC_LMM_ATT_CN_RSLT_COMBINED_EPS_IMSI == pstLmmAttachIndMsg->ulCnRslt)
        {
            NAS_MMC_ProcLmmCombinedAttachInd_AttRsltCsEpsSucc(pstLmmAttachIndMsg,
                          penCsAddition, penPsAddition);
        }
        else if (MMC_LMM_ATT_CN_RSLT_EPS_ONLY == pstLmmAttachIndMsg->ulCnRslt)
        {
            NAS_MMC_ProcLmmCombinedAttachInd_AttRsltEpsOnlySucc(pstLmmAttachIndMsg,enCsRegRsltCause,
                          penCsAddition, penPsAddition);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmCombinedAttachInd_AttRsltSucc_PlmnSelection:Unexpected cn result!");
        }
    }
    else
    {
        /* 设置当前注册状态为成功 */
        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_SetCsServiceStatus(NAS_MMC_NORMAL_SERVICE);
        /* 假流程注册成功上报正常服务返回 */
        *penPsAddition = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        NAS_MMC_SetPsServiceStatus(NAS_MMC_NORMAL_SERVICE);
        NAS_MMC_SetCsServiceStatus(NAS_MMC_NORMAL_SERVICE);

        *penCsAddition = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE);
        
        

        NAS_MMC_UpdateCsRegStateCsRegSucc();

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_CS_PS, VOS_TRUE, NAS_MML_REG_FAIL_CAUSE_NULL);
    }


    NAS_MMC_NotifyModeChange(NAS_MML_GetCurrNetRatType(),
                             NAS_MML_GetSimPsRegStatus());


    return;
}
VOS_VOID  NAS_MMC_ProcLmmCombinedAttachInd_PlmnSelection(
    LMM_MMC_ATTACH_IND_STRU            *pstLmmAttachIndMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegRsltCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegRsltCause;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU                               stSndEquPlmnInfo;

    enPsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_BUTT;
    enCsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_BUTT;

    NAS_MMC_ConverLmmCombinedAttachRsltToMMLCause(pstLmmAttachIndMsg,&enPsRegRsltCause, &enCsRegRsltCause);

    switch (pstLmmAttachIndMsg->ulAttachRslt)
    {
        case MMC_LMM_ATT_RSLT_SUCCESS:
            NAS_MMC_ProcLmmCombinedAttachInd_AttRsltSucc_PlmnSelection(pstLmmAttachIndMsg,
                       enCsRegRsltCause, &enCsAdditionalAction, &enPsAdditionalAction);

            break;

        case MMC_LMM_ATT_RSLT_FAILURE:
            if (VOS_FALSE == pstLmmAttachIndMsg->bitOpAtmpCnt)
            {
                enPsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
                enCsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
                NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);
                NAS_MMC_SetCsServiceStatus(NAS_MMC_LIMITED_SERVICE);

                NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

                NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_CS_PS, VOS_FALSE, NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);
            }
            else
            {
                NAS_MMC_ProcCombinedAttachEpsRegFail(LMM_MMC_ATTACH_IND, pstLmmAttachIndMsg,
                                      &enPsAdditionalAction, &enCsAdditionalAction);
            }
            break;

        case MMC_LMM_ATT_RSLT_ESM_FAILURE:
        case MMC_LMM_ATT_RSLT_MO_DETACH_FAILURE:
        case MMC_LMM_ATT_RSLT_MT_DETACH_FAILURE:
        case MMC_LMM_ATT_RSLT_T3402_RUNNING:
        case MMC_LMM_ATT_RSLT_TIMER_EXP:
        case MMC_LMM_ATT_RSLT_ACCESS_BAR:
        case MMC_LMM_ATT_RSLT_FORBID_PLMN:
        case MMC_LMM_ATT_RSLT_FORBID_TA_FOR_RPOS:
        case MMC_LMM_ATT_RSLT_FORBID_PLMN_FOR_GPRS:
        case MMC_LMM_ATT_RSLT_FORBID_TA_FOR_ROAMING:
        case MMC_LMM_ATT_RSLT_AUTH_REJ:
        case MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW:
            NAS_MMC_ProcCombinedAttachEpsRegFail(LMM_MMC_ATTACH_IND, pstLmmAttachIndMsg,
                                  &enPsAdditionalAction, &enCsAdditionalAction);
            break;

        case MMC_LMM_ATT_RSLT_CN_REJ:
            if ((VOS_TRUE == pstLmmAttachIndMsg->bitOpCnRslt)
             && (MMC_LMM_ATT_CN_RSLT_EPS_ONLY == pstLmmAttachIndMsg->ulCnRslt))
            {
                /* 当ATTACH请求类型为联合，网侧相应的ATTACH结果类型为EPS ONLY，
                   且原因值不为协议24301 5.5.1.3.4.3章节所列或者没有携带原因值，
                   则LMM通过ATTACH结果中MMC_LMM_ATT_RSLT_CN_REJ通知MMC，
                   且携带相应的ATTACH请求类型、网侧响应的ATTCH结果类型、原因值，
                   并携带处理该原因后的attach attempt counter值 */
                NAS_MMC_ProcLmmCombinedAttachInd_AttRsltEpsOnlySucc(pstLmmAttachIndMsg,enCsRegRsltCause,
                                  &enCsAdditionalAction, &enPsAdditionalAction);

            }
            else
            {
                NAS_MMC_ProcCombinedAttachEpsRegFail(LMM_MMC_ATTACH_IND, pstLmmAttachIndMsg,
                                      &enPsAdditionalAction, &enCsAdditionalAction);
            }
            break;

        default:
            /* 异常打印 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmCombinedAttachInd_PlmnSelection:Unexpected attach result!");
            enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();
            enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
            break;
    }

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /* AdditionalAction有效，更新CS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction )
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

    /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
       以便于用户指定搜网回复结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegCause_PlmnSelection(enPsRegRsltCause);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }


    NAS_MMC_UpdateUserSpecPlmnRegisterStatus(enPsRegRsltCause);

    return;

}
VOS_VOID  NAS_MMC_ProcLmmMmcEpsOnlyAttachInd_PlmnSelection(
    LMM_MMC_ATTACH_IND_STRU            *pstLmmAttachIndMsg
)
{
    VOS_UINT32                          ulEquPlmnNum;
    MMC_LMM_PLMN_ID_STRU               *pstLmmEquPlmnList = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU           stLEplmnInfo;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enRegRsltCause;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU           stSndEquPlmnInfo;

    VOS_UINT8                           ucSimCsRegStatus;

    NAS_MMC_ConverLmmAttachRsltToMMLCause(pstLmmAttachIndMsg, &enRegRsltCause);

    if (MMC_LMM_ATT_RSLT_SUCCESS == pstLmmAttachIndMsg->ulAttachRslt)
    {
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        NAS_MMC_SaveRegRsltCtx(LMM_MMC_ATTACH_IND, pstLmmAttachIndMsg);

        /* 根据注册结果更新注册信息表 */
        NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_EPS, NAS_MML_REG_FAIL_CAUSE_NULL);

        NAS_MMC_SetPsServiceStatus(NAS_MMC_NORMAL_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE);

        /* EPS单域注册成功时，需要删除ForbPlmn,ForbGprs等信息 */
        NAS_MMC_DelForbInfo_LmmAttRsltSucc(VOS_NULL_PTR);

        /* 如果CS允许注册而且CS卡有效则更改CS注册状态为NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH */
        ucSimCsRegStatus = NAS_MML_GetSimCsRegStatus();
        if ((VOS_TRUE == NAS_MML_GetCsAttachAllowFlg())
         && (VOS_TRUE == ucSimCsRegStatus))
        {
            NAS_MMC_ChangeCsRegState(NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH);
        }

        /* 更新EPLMN */
        if ( VOS_TRUE == pstLmmAttachIndMsg->bitOpEplmn)
        {
            ulEquPlmnNum        = pstLmmAttachIndMsg->stEplmnList.ulPlmnNum;
            pstLmmEquPlmnList   = pstLmmAttachIndMsg->stEplmnList.astEplmnList;

            /* 将LMM的PLMN ID格式转换为GU的格式 */
            NAS_MMC_ConvertLmmPlmnToGUNasPlmn(ulEquPlmnNum, pstLmmEquPlmnList, (stLEplmnInfo.astEquPlmnAddr));
            NAS_MML_SaveEquPlmnList(ulEquPlmnNum, stLEplmnInfo.astEquPlmnAddr);

            NAS_MMC_WriteEplmnNvim();
        }

        NAS_MMC_UpdateGURegRlstRPlmnIdInNV();


        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_PS, VOS_TRUE, NAS_MML_REG_FAIL_CAUSE_NULL);

        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_NotifyModeChange(NAS_MML_GetCurrNetRatType(),
                                  NAS_MML_GetSimPsRegStatus());
    }
    else if ((MMC_LMM_ATT_RSLT_FAILURE == pstLmmAttachIndMsg->ulAttachRslt)
          && (VOS_FALSE == pstLmmAttachIndMsg->bitOpAtmpCnt))
    {
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
        NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_PS, VOS_FALSE, NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);
    }
    else
    {
        enAdditionalAction= NAS_MMC_ProcEpsAttachRegFail(LMM_MMC_ATTACH_IND, pstLmmAttachIndMsg);

        NAS_MMC_UpdateUserSpecPlmnRegisterStatusWhenRegFail(enRegRsltCause);
    }

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
       以便于用户指定搜网回复结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enAdditionalAction)
    {
        NAS_MMC_SetPsRegCause_PlmnSelection(enRegRsltCause);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    return ;

}



VOS_VOID  NAS_MMC_ProcLmmAttachInd_PlmnSelection(
    LMM_MMC_ATTACH_IND_STRU            *pstLmmAttachIndMsg
)
{
    switch (pstLmmAttachIndMsg->ulReqType)
    {
        case MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI:

            /* 处理lmm的联合attach结果 */
            NAS_MMC_ProcLmmCombinedAttachInd_PlmnSelection(pstLmmAttachIndMsg);
            break;

        case MMC_LMM_ATT_TYPE_EPS_ONLY:

            /* 处理lmm的非联合attach，EPS only注册的结果 */
            NAS_MMC_ProcLmmMmcEpsOnlyAttachInd_PlmnSelection(pstLmmAttachIndMsg);
            break;

        case MMC_LMM_ATT_TYPE_EPS_EMERGENCY:

            /* 暂不支持未实现 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmAttachInd_PlmnSelection:Unsupported attach req type!");
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmAttachInd_PlmnSelection:Unexpected attach req type!");
            break;

    }
    return;
}
VOS_VOID  NAS_MMC_ProcLmmEpsOnlyTauResultInd_PlmnSelection(
    LMM_MMC_TAU_RESULT_IND_STRU        *pstLmmTauIndMsg
)
{
    VOS_UINT32                          ulEquPlmnNum;
    MMC_LMM_PLMN_ID_STRU               *pstLmmEquPlmnList = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU           stLEplmnAddr;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enRegRsltCause;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8 enAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU           stSndEquPlmnInfo;

    VOS_UINT8                           ucSimCsRegStatus;

    NAS_MMC_ConverTauResultToMMLCause(pstLmmTauIndMsg, &enRegRsltCause);

    if (MMC_LMM_TAU_RSLT_SUCCESS == pstLmmTauIndMsg->ulTauRst)
    {
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        /* 保存注册结果消息 */
        NAS_MMC_SaveRegRsltCtx(LMM_MMC_TAU_RESULT_IND, pstLmmTauIndMsg);

        /* 根据注册结果更新注册信息表 */
        NAS_MMC_UpdatePlmnRegInfoList(NAS_MML_GetCurrCampPlmnId(), NAS_MMC_REG_DOMAIN_EPS, NAS_MML_REG_FAIL_CAUSE_NULL);

        NAS_MMC_SetPsServiceStatus(NAS_MMC_NORMAL_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE);

        /* EPS单域注册成功时，需要删除ForbPlmn,ForbGprs等信息 */
        NAS_MMC_DelForbInfo_LmmAttRsltSucc(VOS_NULL_PTR);

        /* 此处更新CS addition原因是联合TAU仅EPS成功时,LMM启动T3411再次尝试TAU过程中,
           用户Detach CS,LMM会直接报TAU成功,不再尝试联合TAU */
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON);

        /* 如果CS允许注册而且CS卡有效则更改CS注册状态为NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH */
        ucSimCsRegStatus = NAS_MML_GetSimCsRegStatus();
        if ((VOS_TRUE == NAS_MML_GetCsAttachAllowFlg())
         && (VOS_TRUE == ucSimCsRegStatus))
        {
            NAS_MMC_ChangeCsRegState(NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH);
        }

        

        /* 更新EPLMN */
        if ( VOS_TRUE == pstLmmTauIndMsg->bitOpEplmn)
        {
            ulEquPlmnNum        = pstLmmTauIndMsg->stEplmnList.ulPlmnNum;
            pstLmmEquPlmnList   = pstLmmTauIndMsg->stEplmnList.astEplmnList;

            /* 将LMM的PLMN ID格式转换为MMC的格式 */
            NAS_MMC_ConvertLmmPlmnToGUNasPlmn(ulEquPlmnNum, pstLmmEquPlmnList, (stLEplmnAddr.astEquPlmnAddr));

            NAS_MML_SaveEquPlmnList(ulEquPlmnNum, stLEplmnAddr.astEquPlmnAddr);

            NAS_MMC_WriteEplmnNvim();
        }

        NAS_MMC_UpdateGURegRlstRPlmnIdInNV();
        
        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_PS, VOS_TRUE, NAS_MML_REG_FAIL_CAUSE_NULL);

        NAS_MMC_SetUserSpecPlmnRegisterStatus(VOS_TRUE);

        NAS_MMC_NotifyModeChange(NAS_MML_GetCurrNetRatType(),
                                  NAS_MML_GetSimPsRegStatus());
    }
    else if ((MMC_LMM_TAU_RSLT_FAILURE == pstLmmTauIndMsg->ulTauRst)
          && (VOS_FALSE == pstLmmTauIndMsg->bitOpAtmpCnt))
    {
        enAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
        NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_PS, VOS_FALSE, NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);
    }
    else
    {
        enAdditionalAction = NAS_MMC_ProcEpsTauRegFail(LMM_MMC_TAU_RESULT_IND, pstLmmTauIndMsg);

        NAS_MMC_UpdateUserSpecPlmnRegisterStatusWhenRegFail(enRegRsltCause);
    }

    /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
       以便于用户指定搜网回复结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enAdditionalAction)
    {
        NAS_MMC_SetPsRegCause_PlmnSelection(enRegRsltCause);
    }

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enAdditionalAction);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }

    return ;

}



VOS_VOID NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltSucc_PlmnSelection(
    LMM_MMC_TAU_RESULT_IND_STRU                            *pstLmmTauIndMsg,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegRsltCause,
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                   *penCsAddition,
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                   *penPsAddition
)
{
    if (VOS_TRUE == pstLmmTauIndMsg->bitOpCnRst)
    {
        /* CS+PS的周期性TAU时,不论是否携带CS CAUSE值,都认为联合注册成功 */
        if ((MMC_LMM_COMBINED_TA_LA_UPDATED == pstLmmTauIndMsg->ulCnRst)
         || (MMC_LMM_COMBINED_TA_LA_UPDATED_ISR_ACTIVATED == pstLmmTauIndMsg->ulCnRst)
         || (MMC_LMM_CS_PS_PERIODIC_UPDATING == pstLmmTauIndMsg->ulReqType))
        {
            /* 和网侧交互EPS和CS均注册成功 */
            NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltCsEpsSucc(pstLmmTauIndMsg,
                          penCsAddition, penPsAddition);

        }
        else if ((MMC_LMM_TA_UPDATED == pstLmmTauIndMsg->ulCnRst)
              || (MMC_LMM_TA_UPDATED_ISR_ACTIVATED == pstLmmTauIndMsg->ulCnRst))
        {
            /* 与网侧交互EPS成功CS失败，且原因值为协议24301 5.5.3.3.4.3章节所列 */
            NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltEpsOnlySucc(pstLmmTauIndMsg,enCsRegRsltCause,
                          penCsAddition, penPsAddition);

        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltSucc_PlmnSelection:Unexpected cn result!");
        }
    }
    else
    {
        /* 假流程注册成功上报正常服务返回 */
        *penPsAddition = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;
        NAS_MMC_SetPsServiceStatus(NAS_MMC_NORMAL_SERVICE);

        NAS_MMC_SetCsServiceStatus(NAS_MMC_NORMAL_SERVICE);

        *penCsAddition = NAS_MMC_ADDITIONAL_ACTION_NORMAL_CAMP_ON;

        NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE);
        


        NAS_MMC_UpdateCsRegStateCsRegSucc();

        NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_CS_PS, VOS_TRUE, NAS_MML_REG_FAIL_CAUSE_NULL);
    }

    NAS_MMC_NotifyModeChange(NAS_MML_GetCurrNetRatType(),
                             NAS_MML_GetSimPsRegStatus());


    return;
}
VOS_VOID  NAS_MMC_ProcLmmCombinedTauResultInd_PlmnSelection(
    LMM_MMC_TAU_RESULT_IND_STRU        *pstLmmTauIndMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegRsltCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegRsltCause;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enPsAdditionalAction;
    NAS_MMC_ADDITIONAL_ACTION_ENUM_UINT8                    enCsAdditionalAction;

    NAS_MML_EQUPLMN_INFO_STRU                              *pstEquPlmnInfo = VOS_NULL_PTR;
     NAS_MML_EQUPLMN_INFO_STRU                              stSndEquPlmnInfo;


    enPsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_BUTT;
    enCsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_BUTT;

    NAS_MMC_ConverLmmCombinedTauRsltToMMLCause(pstLmmTauIndMsg, &enPsRegRsltCause, &enCsRegRsltCause);

    switch (pstLmmTauIndMsg->ulTauRst)
    {
        case MMC_LMM_TAU_RSLT_SUCCESS:
            NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltSucc_PlmnSelection(pstLmmTauIndMsg,
                       enCsRegRsltCause, &enCsAdditionalAction, &enPsAdditionalAction);

            break;

        case MMC_LMM_TAU_RSLT_FAILURE:
            if (VOS_FALSE == pstLmmTauIndMsg->bitOpAtmpCnt)
            {
                enPsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
                enCsAdditionalAction = NAS_MMC_ADDITIONAL_ACTION_LIMITED_CAMP_ON;
                NAS_MMC_SetPsServiceStatus(NAS_MMC_LIMITED_SERVICE);
                NAS_MMC_SetCsServiceStatus(NAS_MMC_LIMITED_SERVICE);

                NAS_MMC_SndMmaServiceStatusInd(MMA_MMC_SRVDOMAIN_CS_PS, MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE);

                NAS_MMC_SndMmaRegResultInd(MMA_MMC_SRVDOMAIN_CS_PS, VOS_FALSE, NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);
            }
            else
            {
                /* EPS和CS注册均失败 */
                NAS_MMC_ProcCombinedTauEpsRegFail(LMM_MMC_TAU_RESULT_IND, pstLmmTauIndMsg,
                                              &enPsAdditionalAction, &enCsAdditionalAction);
            }
            break;

        case MMC_LMM_TAU_RSLT_MO_DETACH_FAILURE:
        case MMC_LMM_TAU_RSLT_MT_DETACH_FAILURE:
        case MMC_LMM_TAU_RSLT_T3402_RUNNING:
        case MMC_LMM_TAU_RSLT_TIMER_EXP:
        case MMC_LMM_TAU_RSLT_ACCESS_BARED:
        case MMC_LMM_TAU_RSLT_FORBID_PLMN:
        case MMC_LMM_TAU_RSLT_FORBID_TA_FOR_RPOS:
        case MMC_LMM_TAU_RSLT_FORBID_PLMN_FOR_GPRS:
        case MMC_LMM_TAU_RSLT_FORBID_TA_FOR_ROAMING:
        case MMC_LMM_TAU_RSLT_AUTH_REJ:
            /* EPS和CS注册均失败 */
            NAS_MMC_ProcCombinedTauEpsRegFail(LMM_MMC_TAU_RESULT_IND, pstLmmTauIndMsg,
                                          &enPsAdditionalAction, &enCsAdditionalAction);
            break;

        case MMC_LMM_TAU_RSLT_CN_REJ:
            if ((VOS_TRUE == pstLmmTauIndMsg->bitOpCnRst)
             && ((MMC_LMM_TA_UPDATED == pstLmmTauIndMsg->ulCnRst)
              || (MMC_LMM_TA_UPDATED_ISR_ACTIVATED == pstLmmTauIndMsg->ulCnRst)))
            {
                /* 当TAU请求类型为联合，网侧相应的TAU结果类型为TA UPDATED，
                   且原因值不为协议24301 5.5.3.3.4.3章节所列或者没有携带原因值，
                   则LMM通过TAU结果中MMC_LMM_TAU_RSLT_CN_REJ通知MMC，且携带相应
                   的TAU请求类型、网侧响应的TAU结果类型、原因值，并携带处理该原
                   因后的tau attempt counter值 */
                NAS_MMC_ProcLmmCombinedTauResultInd_TauRsltEpsOnlySucc(pstLmmTauIndMsg,enCsRegRsltCause,
                                  &enCsAdditionalAction, &enPsAdditionalAction);
            }
            else
            {
                /* EPS和CS注册均失败 */
                NAS_MMC_ProcCombinedTauEpsRegFail(LMM_MMC_TAU_RESULT_IND, pstLmmTauIndMsg,
                                              &enPsAdditionalAction, &enCsAdditionalAction);

            }
            break;

        default:
            /* 异常打印 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmCombinedTauResultInd_PlmnSelection:Unexpected attach result!");
            enPsAdditionalAction = NAS_MMC_GetPsRegAdditionalAction_PlmnSelection();
            enCsAdditionalAction = NAS_MMC_GetCsRegAdditionalAction_PlmnSelection();
            break;

    }

    /* AdditionalAction有效，更新EPS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enPsAdditionalAction )
    {
        NAS_MMC_SetPsRegAdditionalAction_PlmnSelection(enPsAdditionalAction);
    }

    /* AdditionalAction有效，更新CS的下一步的动作类型 */
    if ( NAS_MMC_ADDITIONAL_ACTION_BUTT != enCsAdditionalAction )
    {
        NAS_MMC_SetCsRegAdditionalAction_PlmnSelection(enCsAdditionalAction);
    }

     /* 如果还需要等待下一次注册结果,则不更新注册原因值,等待conut达到最大后更新,
    以便于用户指定搜网回复结果 */
    if (NAS_MMC_ADDITIONAL_ACTION_WAIT_REG_ATTEMPT != enPsAdditionalAction)
    {
        NAS_MMC_SetPsRegCause_PlmnSelection(enPsRegRsltCause);
    }

    if (VOS_TRUE == NAS_MMC_IsNeedSndEplmn_PlmnSelection())
    {
        pstEquPlmnInfo = NAS_MML_GetEquPlmnList();
        PS_MEM_CPY(&stSndEquPlmnInfo, pstEquPlmnInfo, sizeof(stSndEquPlmnInfo));
        NAS_MMC_BuildSndLmmEquPlmnInfo(&stSndEquPlmnInfo);
        NAS_MMC_SndLmmEquPlmnReq(&stSndEquPlmnInfo);

        NAS_MMC_SndOmEquPlmn();
    }


    NAS_MMC_UpdateUserSpecPlmnRegisterStatus(enPsRegRsltCause);

    return;
}


VOS_VOID  NAS_MMC_ProcLmmTauResultInd_PlmnSelection(
    LMM_MMC_TAU_RESULT_IND_STRU        *pstLmmTauIndMsg
)
{
    switch (pstLmmTauIndMsg->ulReqType)
    {
        case MMC_LMM_COMBINED_TA_LA_UPDATING:
        case MMC_LMM_COMBINED_TA_LA_WITH_IMSI:
        case MMC_LMM_CS_PS_PERIODIC_UPDATING:

            /* 处理lmm的联合tau结果 */
            NAS_MMC_ProcLmmCombinedTauResultInd_PlmnSelection(pstLmmTauIndMsg);
            break;

        case MMC_LMM_TA_UPDATING:
        case MMC_LMM_PS_PERIODIC_UPDATING:

            /* 处理lmm的非联合tau，EPS only注册的结果 */
            NAS_MMC_ProcLmmEpsOnlyTauResultInd_PlmnSelection(pstLmmTauIndMsg);
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_ProcLmmTauResultInd_PlmnSelection:Unexpected tau req type!");
            break;

    }

    return;
}



VOS_UINT32 NAS_MMC_RcvLmmMmcTauResultInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enPsRegCause;
    LMM_MMC_TAU_RESULT_IND_STRU        *pstLmmTauIndMsg = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32          enCurrentFsmId;
    VOS_UINT32                          ulCurrentEventType;

    pstLmmTauIndMsg  = (LMM_MMC_TAU_RESULT_IND_STRU*)pstMsg;

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* 向GMM和MM转发注册结果消息 */
    NAS_MMC_SndMmLmmTauResultInd(pstLmmTauIndMsg);
    NAS_MMC_SndGmmLmmTauResultInd(pstLmmTauIndMsg);

    /* 搜网注册过程中 ,用户设置syscfg需要搜网场景，MMC会主动释放连接，
       在等待连接释放状态，L可能会上注册结果，需要处理 */
    NAS_MMC_ProcLmmTauResultInd_PlmnSelection(pstLmmTauIndMsg);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();
        }
    }

    return VOS_TRUE;
}
VOS_UINT32 NAS_MMC_RcvLmmMmcAttachInd_PlmnSelection_WaitEpsConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enCsRegCause;
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16                      enPsRegCause;
    LMM_MMC_ATTACH_IND_STRU                                *pstLmmAttachIndMsg = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    VOS_UINT32                                              ulCurrentEventType;
    NAS_MML_LTE_UE_OPERATION_MODE_ENUM_UINT8                enUeOperationMode;

    enUeOperationMode   =  NAS_MML_GetLteUeOperationMode();
    pstLmmAttachIndMsg  = (LMM_MMC_ATTACH_IND_STRU*)pstMsg;

    NAS_MMC_ClearSingleDomainWaitRegRsltFlag_PlmnSelection(NAS_MMC_WAIT_REG_RESULT_IND_PS);

    /* LMM在搜网未回复search cnf时收到用户detach ps请求后，后续不会做注册给mmc回复attach result
       为MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW，请求类型填的是MMC_LMM_ATT_TYPE_BUTT，
       MMC需要根据当前ue operation mode给请求类型重新赋值 */
    if ((MMC_LMM_ATT_TYPE_BUTT == pstLmmAttachIndMsg->ulReqType)
     && (MMC_LMM_ATT_RSLT_PS_ATT_NOT_ALLOW == pstLmmAttachIndMsg->ulAttachRslt))
    {
        pstLmmAttachIndMsg->ulReqType = NAS_MMC_ConvertLmmAttachReqType(enUeOperationMode);
    }

    /* 向GMM和MM转发注册结果消息 */
    NAS_MMC_SndMmLmmAttachInd(pstLmmAttachIndMsg);
    NAS_MMC_SndGmmLmmAttachInd(pstLmmAttachIndMsg);

    /* 搜网注册过程中 ,用户设置syscfg需要搜网场景，MMC会主动释放连接，
      在等待连接释放状态，L可能会上注册结果，需要处理 */
    NAS_MMC_ProcLmmAttachInd_PlmnSelection(pstLmmAttachIndMsg);

    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    if (NAS_MMC_FSM_PLMN_SELECTION != enCurrentFsmId)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "ERROR:FsmId Error");
    }
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    if (VOS_TRUE == NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId,ulCurrentEventType))
    {
        enCsRegCause = NAS_MMC_GetCsRegCause_PlmnSelection();
        enPsRegCause = NAS_MMC_GetPsRegCause_PlmnSelection();

        if (VOS_TRUE == NAS_MMC_ProcUserSearchRegRslt_PlmnSelection(enCsRegCause,enPsRegCause))
        {
            /*更新入口消息为普通的搜网请求，后续有新的用户请求时，可以打断该状态机*/
            NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();
        }
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MMC_IsNeedEnableLte_PlmnSelection(
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene
)
{
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapabilityStatus;

    enLteCapabilityStatus = NAS_MML_GetLteCapabilityStatus();

    /* 如果开机时已经根据en_NV_Item_Rat_Forbidden_List_Accord_Imsi_Config的配置禁止了LTE，则返回VOS_FALSE */
    if (NAS_MML_RAT_CAPABILITY_STATUS_DISABLE == NAS_MML_GetLteForbiddenStatusFlg())
    {
        return VOS_FALSE;
    }

    /* 如果是SIM卡，不需要Enable Lte */
    if (NAS_MML_SIM_TYPE_SIM == NAS_MML_GetSimType())
    {
        return VOS_FALSE;
    }

    /* PS卡无效, 不需要重新 Enable L */
    if ( VOS_FALSE == NAS_MML_GetSimPsRegStatus() )
    {
        return VOS_FALSE;
    }

    /* PS卡不允许注册,不需要重新 Enable L */
    if ( VOS_FALSE == NAS_MML_GetPsAttachAllowFlg() )
    {
        return VOS_FALSE;
    }

    /* 搜网场景是NAS_MMC_PLMN_SEARCH_SCENE_DISABLE_LTE,不需要重新 Enable L */
    if ( NAS_MMC_PLMN_SEARCH_SCENE_DISABLE_LTE == enPlmnSearchScene )
    {
        return VOS_FALSE;
    }

    /* disable 或enable lte不再判断syscfg是否支持L */

    /* 已经Disable L,需要重新 Enable L */
    if ( (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLteCapabilityStatus)
      || (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_UNNOTIFY_AS == enLteCapabilityStatus) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MMC_IsDisableLteNeedWaitEpsConnRel_PlmnSelection(VOS_VOID)
{
    NAS_MML_PLMN_WITH_RAT_STRU                              stDestPlmn;
	
	/* 解决版本Coverity问题 */
	PS_MEM_SET(&stDestPlmn, 0, sizeof(NAS_MML_PLMN_WITH_RAT_STRU));
	
    if (VOS_TRUE == NAS_MMC_IsDisableLteNeedLocalReleaseEpsConn())
    {
        /* 最后一个网络*/
        if (VOS_FALSE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
        {
            /* 回复状态机退出成功 */
            NAS_MMC_SndRslt_PlmnSelection(NAS_MMC_PLMN_SELECTION_FAIL);

            /* 直接退出当前状态机 */
            NAS_MMC_FSM_QuitFsmL2();

            return VOS_FALSE;
        }

        NAS_MMC_SetRelRequestFlag_PlmnSelection(VOS_TRUE);

        NAS_MMC_SndLmmRelReq();
    }

    return VOS_TRUE;
}

#endif


VOS_UINT32  NAS_MMC_RcvTafSpecPlmnSearchAbortReq_PlmnSelection(
     VOS_UINT32                         ulEventType,
     struct MsgCB                      *pstMsg
)
{
    /* 更新搜网状态机入口消息为普通搜网 */
    NAS_MMC_SaveCommonPlmnSrchEntryMsg_PlmnSelection();

#if (FEATURE_ON == FEATURE_LTE)
    /* 通知通知LMM当前在非用户指定搜网模式 */
    NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify();
#endif

    /* 回复TAFMMC_SPEC_PLMN_SEARCH_ABORT_CNF给MMA */
    NAS_MMC_SndMmaSpecPlmnSearchAbortCnf();
    return VOS_TRUE;
}
VOS_UINT32  NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnSelection_WaitCsPsRegRsltInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnSelection(ulEventType, pstMsg);

}


VOS_UINT32  NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnSelection_WaitRrcConnRelInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnSelection(ulEventType, pstMsg);

}


VOS_UINT32  NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnSelection(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_LIMIT_SERVICE_CAMP_IND_STRU        *pstRrmmLimitCampOnMsg = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU               stDestPlmn;
    NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU    *pastIntraPlmnSrchInfo = VOS_NULL_PTR;
    VOS_UINT32                               ulIndex;

    pstRrmmLimitCampOnMsg = (RRMM_LIMIT_SERVICE_CAMP_IND_STRU *)pstMsg;

    pastIntraPlmnSrchInfo = (NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                                             sizeof(NAS_MMC_SEARCHED_PLMN_LIST_INFO_STRU) * NAS_MML_MAX_RAT_NUM);
    /* 分配内存失败 */
    if (VOS_NULL_PTR == pastIntraPlmnSrchInfo)
    {
        return VOS_TRUE;
    }

    NAS_MMC_InitSearchedPlmnListInfo(pastIntraPlmnSrchInfo);

    /* 转换W下的网络格式 */
    NAS_MMC_UpdateSearchedPlmnListInfo(&(pstRrmmLimitCampOnMsg->stPlmnIdList), pastIntraPlmnSrchInfo);


    /* 删除列表中禁止的PLMN */
    NAS_MMC_DelForbPlmnFromAsSrchedPlmnList(pastIntraPlmnSrchInfo);

    /* 手动搜网模式下删除非用户指定的PLMN */
    NAS_MMC_DelNotUserSpecPlmnFromSrchedPlmnList(pastIntraPlmnSrchInfo);

    /* 可维可测信息勾出 */
    NAS_MMC_LogAsPlmnList(pastIntraPlmnSrchInfo);

    if (VOS_TRUE == NAS_MMC_IsExistAvailPlmnInSearchedPlmnList(pastIntraPlmnSrchInfo))
    {
        for (ulIndex = 0; ulIndex < NAS_MML_MAX_RAT_NUM; ulIndex++)
        {
            NAS_MMC_UpdatePlmnSearchList_PlmnSelection(&(pastIntraPlmnSrchInfo[ulIndex]));
        }

        /* 当还存在下一个需要搜索的网络时，继续进行搜网，否则，继续等待 */
        if (VOS_TRUE == NAS_MMC_GetNextSearchPlmn_PlmnSelection(&stDestPlmn))
        {
            /* 当前应该肯定存在信令连接,如果不存在连接,不用处理 */
            if (VOS_TRUE == NAS_MML_IsRrcConnExist())
            {
                /* 根据当前不同的接入技术,发送释放请求,迁移到不同的等待连接释放的L2状态，并启动保护定时器 */
                NAS_MMC_SndRelReq_PlmnSelection(NAS_MML_NET_RAT_TYPE_WCDMA);
            }
        }
    }

    PS_MEM_FREE(WUEPS_PID_MMC, pastIntraPlmnSrchInfo);

    return VOS_TRUE;
}


VOS_VOID NAS_MMC_UpdateOosAreaLostPlmnSearchInfo_PlmnSelection(
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU                     *pstPlmnSearchReq,
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionListInfo
)
{
    NAS_MML_PLMN_RAT_PRIO_STRU          stPrioRatList;
    NAS_MMC_PLMN_TYPE_ENUM_UINT8        enPlmnType;
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    enPlmnType = NAS_MMC_PLMN_TYPE_AVAIL_PLMN;

    for (i = 0; i < NAS_MML_MAX_RAT_NUM; i++)
    {
        if (NAS_MML_NET_RAT_TYPE_BUTT == pstPlmnSearchReq->astInterPlmnSearchInfo[i].enRatType)
        {
            continue;
        }

        stPrioRatList.aucRatPrio[0] = pstPlmnSearchReq->astInterPlmnSearchInfo[i].enRatType;
        stPrioRatList.ucRatNum      = 1;

        for (j = 0; j < pstPlmnSearchReq->astInterPlmnSearchInfo[i].ulHighPlmnNum; j++)
        {
            NAS_MMC_InsertPlmnToPlmnSelectionListHead(&pstPlmnSearchReq->astInterPlmnSearchInfo[i].astHighPlmnList[j],
                                                      enPlmnType,
                                                      &stPrioRatList,
                                                      pstPlmnSelectionListInfo);
        }

        for (j = 0; j < pstPlmnSearchReq->astInterPlmnSearchInfo[i].ulLowPlmnNum; j++)
        {
            NAS_MMC_InsertPlmnToPlmnSelectionListHead(&pstPlmnSearchReq->astInterPlmnSearchInfo[i].astLowPlmnList[j].stPlmnId,
                                                      enPlmnType,
                                                      &stPrioRatList,
                                                      pstPlmnSelectionListInfo);
        }
    }

    /* 可维可测，输出选网列表信息 */
    NAS_MMC_LogPlmnSelectionList(pstPlmnSelectionListInfo);

    return;
}

VOS_UINT32 NAS_MMC_RcvMmcAbortFsmMsg_PlmnSelection(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
     /* 记录需要退出当前状态机标志 */
    NAS_MMC_SetAbortFlag_PlmnSelection(VOS_TRUE);

    return VOS_TRUE;
}


NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32 NAS_MMC_GetPlmnSearchScene_PlmnSelection(VOS_VOID)
{
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU *pstInterPlmnSearchReq = VOS_NULL_PTR;
    NAS_MMC_ENTRY_MSG_STRU             *pstEntryMsg           = VOS_NULL_PTR;

    /* 获取状态机的入口消息信息 */
    pstEntryMsg  = NAS_MMC_GetCurrEntryMsg();

    if (VOS_TRUE == NAS_MMC_IsInterPlmnSearch_PlmnSelection())
    {
        pstInterPlmnSearchReq = (NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
        return pstInterPlmnSearchReq->enPlmnSearchScene;
    }

    /* 如果入口消息不为内部搜网消息，则返回无效的搜网场景 */
    return NAS_MMC_PLMN_SEARCH_SCENE_BUTT;
}


VOS_UINT32  NAS_MMC_IsInterPlmnSearch_PlmnSelection(VOS_VOID)
{
    NAS_MMC_ENTRY_MSG_STRU             *pstEntryMsg    = VOS_NULL_PTR;

   /* 根据FSM ID获取该FSM 入口消息 */
    pstEntryMsg     = NAS_MMC_GetCurrFsmMsgAddr();

    if (MMCMMC_INTER_PLMN_SEARCH_REQ == NAS_ExtractMsgNameFromEvtType(pstEntryMsg->ulEventType))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 NAS_MMC_IsNeedSndEplmn_PlmnSelection( VOS_VOID )
{
    NAS_MML_EQUPLMN_INFO_STRU                              *pstEplmnInfo  = VOS_NULL_PTR;
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU                *pstDisabledRatPlmnCfg = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                                   *pstCurPlmnId = VOS_NULL_PTR;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbidPlmnInfo = VOS_NULL_PTR;
    NAS_MMC_FSM_ID_ENUM_UINT32                              enCurrentFsmId;
    NAS_MML_PLMN_WITH_RAT_STRU                              stPlmnWithRat;
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8                         enCurRat;
    VOS_UINT32                                              ulCurrentEventType;
    VOS_UINT32                                              ulIsPlmnForbid;
    VOS_UINT32                                              ulIsUserSpecSrch;

    ulIsPlmnForbid  = VOS_FALSE;
    
    /* 无卡时，无须发送等效 Plmn */
    if (VOS_FALSE == NAS_MML_GetSimPresentStatus())
    {
        return VOS_FALSE;
    }

    /*  当前驻留网络，非Rplmn时，不发送 Eplmn给接入层，避免接入层丢网 */
    pstEplmnInfo = NAS_MML_GetEquPlmnList();

    /* 当前无EPlmn，通知接入层,联合RAU或者联合ATTACH,LAU时，需要删除Eplmn，24008:4.7.3.2.5
    if the attach attempt counter is smaller than 5 and, additionally,
    the update status is different from U1 UPDATED or the stored LAI is different
    from the one of the current serving cell, then the mobile station shall delete
    any LAI, TMSI, ciphering key sequence number stored in the SIM/USIM
    and list of equivalent PLMNs */
    if ( 0 == pstEplmnInfo->ucEquPlmnNum)
    {
        /* 没有EPlmn时，会通知接入层当前驻留的网络 */
        return VOS_TRUE;
    }

    if (VOS_FALSE == NAS_MML_CompareBcchPlmnwithSimPlmn(NAS_MML_GetCurrCampPlmnId(), &(pstEplmnInfo->astEquPlmnAddr[0])))
    {
        return VOS_FALSE;
    }

    pstCurPlmnId        = NAS_MML_GetCurrCampPlmnId();
    enCurRat            = NAS_MML_GetCurrNetRatType();

#if   (FEATURE_ON == FEATURE_LTE)
    /* 当不允许LTE国际漫游时 */
    if ((VOS_TRUE == NAS_MMC_IsNeedDisableLteRoam(pstCurPlmnId->ulMcc))
     && (NAS_MML_NET_RAT_TYPE_LTE == enCurRat))
    {        
        ulIsPlmnForbid = VOS_TRUE;
    }
#endif    

    /* 获取禁止接入技术的PLMN信息 */
    pstDisabledRatPlmnCfg = NAS_MML_GetDisabledRatPlmnCfg();
    
    /* 构造带接入技术的PLMN ID */
    stPlmnWithRat.enRat          = NAS_MML_GetCurrNetRatType();
    stPlmnWithRat.stPlmnId.ulMcc = pstCurPlmnId->ulMcc;
    stPlmnWithRat.stPlmnId.ulMnc = pstCurPlmnId->ulMnc;
    
    /* 判断等效PLMN是否在禁止接入技术的PLMN列表中,如果在则不将该PLMN ID发给RRC */
    if (VOS_TRUE == NAS_MML_IsBcchPlmnIdWithRatInDestPlmnWithRatList(&stPlmnWithRat, 
                                                                     pstDisabledRatPlmnCfg->ulDisabledRatPlmnNum,
                                                                     pstDisabledRatPlmnCfg->astDisabledRatPlmnId))
    {   
        ulIsPlmnForbid = VOS_TRUE;
    }
    
    if (VOS_TRUE == NAS_MML_IsPlmnIdInForbidPlmnList(pstCurPlmnId))
    {
        ulIsPlmnForbid = VOS_TRUE;
    }

    pstForbidPlmnInfo = NAS_MML_GetForbidPlmnInfo();
    
    /* 对于LTE，只要在forbidden plmn for gprs则认为禁止 */
    if ((VOS_TRUE == NAS_MML_IsSimPlmnIdInDestBcchPlmnList(pstCurPlmnId, pstForbidPlmnInfo->ucForbGprsPlmnNum, pstForbidPlmnInfo->astForbGprsPlmnList))
     && (NAS_MML_NET_RAT_TYPE_LTE == enCurRat))
    {
        ulIsPlmnForbid = VOS_TRUE;
    }
                   
    enCurrentFsmId     = NAS_MMC_GetCurrFsmId();
    ulCurrentEventType = NAS_MMC_GetCurrFsmEventType();
    
    ulIsUserSpecSrch    = NAS_MMC_IsUserSpecPlmnSrchType_PlmnSelection(enCurrentFsmId, ulCurrentEventType);

    /* 用户指定搜网的情况,EPLMN无效时,当前驻留PLMN在禁止列表里不应该通知RRC，
       否则RRC不能驻留 */
    if ((VOS_TRUE == ulIsUserSpecSrch)
     && (VOS_TRUE == ulIsPlmnForbid))
    {
        return VOS_FALSE;
    }
    
    return VOS_TRUE;
}

#if   (FEATURE_ON == FEATURE_LTE)

VOS_VOID NAS_MMC_DisableLteForSimReason_PlmnSelection(VOS_VOID)
{
    NAS_MML_SIM_TYPE_ENUM_UINT8                             enSimType;
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus;

    enSimType           = NAS_MML_GetSimType();
    enLCapabilityStatus = NAS_MML_GetLteCapabilityStatus();

    if ((VOS_TRUE == NAS_MML_IsPlatformSupportLte())
     && (NAS_MML_SIM_TYPE_SIM == enSimType))
    {
        if ((VOS_FALSE == NAS_MML_GetDisableLteRoamFlg())
         && (NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS != enLCapabilityStatus))
        {
            if (NAS_MML_RAT_CAPABILITY_STATUS_DISABLE != NAS_MML_GetLteForbiddenStatusFlg())
            {
                /* 向WAS/GAS发送disable LTE通知消息 */
                NAS_MMC_SndAsLteCapabilityStatus(WUEPS_PID_WRR, RRC_NAS_LTE_CAPABILITY_STATUS_DISABLE);
                NAS_MMC_SndAsLteCapabilityStatus(UEPS_PID_GAS, RRC_NAS_LTE_CAPABILITY_STATUS_DISABLE);

                /* 向LMM发送disable LTE消息 */
                NAS_MMC_SndLmmDisableLteNotify(MMC_LMM_DISABLE_LTE_REASON_SIM_CARD_SWITCH_ON);

            }
        }

        /* 更新disable LTE能力标记 */
        NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS);
    }

    return;
}
#endif
VOS_VOID NAS_MMC_RefreshPlmnSelectionListRcvMmaRegReq_PlmnSelection(
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene
)
{
    NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU                  *pstPlmnSelectionList = VOS_NULL_PTR;

    pstPlmnSelectionList = NAS_MMC_GetPlmnSelectionListInfo_PlmnSelection();

    switch (enPlmnSearchScene)
    {
        case NAS_MMC_PLMN_SEARCH_SCENE_REG_NCELL:

            /* 重新按NCELL初始化选网列表 */
            PS_MEM_SET(pstPlmnSelectionList, 0x0, sizeof(NAS_MMC_PLMN_SELECTION_LIST_INFO_STRU));
            NAS_MMC_InitPlmnSelectionList_RegNCell(pstPlmnSelectionList, VOS_NULL_PTR);
            break;

        case NAS_MMC_PLMN_SEARCH_SCENE_REG_HPLMN:

            /* 删除非HPLMN */
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_RPLMN, pstPlmnSelectionList);
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_UPLMN, pstPlmnSelectionList);
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_OPLMN, pstPlmnSelectionList);
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_EPLMN, pstPlmnSelectionList);
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_AVAIL_PLMN, pstPlmnSelectionList);
            break;

        case NAS_MMC_PLMN_SEARCH_SCENE_REG_PREF_PLMN:

            /* 删除非HPLMN+UOPLMN */
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_RPLMN, pstPlmnSelectionList);
            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_EPLMN, pstPlmnSelectionList);

            if (VOS_TRUE == NAS_MML_Get3GPP2UplmnNotPrefFlg())
            {
                NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_UPLMN, pstPlmnSelectionList);
            }

            NAS_MMC_DeleteSpecTypePlmnInPlmnSelectionList(NAS_MMC_PLMN_TYPE_AVAIL_PLMN, pstPlmnSelectionList);
            break;

        default:

            /* 其他场景不需要刷新选网列表 */
            break;
    }

    /* 可维可测，输出选网列表信息 */
    NAS_MMC_LogPlmnSelectionList(pstPlmnSelectionList);

    return;
}
VOS_UINT32 NAS_MMC_IsAllowedRegAfterAcqSucc_PlmnSelection(VOS_VOID)
{
    /* 如果当前注册是由3GPP MMC控制返回VOS_TRUE */
    if (NAS_MMC_REG_CONTROL_BY_3GPP_MMC == NAS_MMC_GetRegCtrl())
    {
        return VOS_TRUE;
    }

    /* 如果当前注册是由3GPP2 CBP控制而且是MMA触发的注册返回VOS_TRUE */
    if (VOS_TRUE == NAS_MMC_IsMmaRegReq_PlmnSelection())
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_UINT32 NAS_MMC_IsMmaRegReq_PlmnSelection(VOS_VOID)
{
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;

    enPlmnSearchScene = NAS_MMC_GetPlmnSearchScene_PlmnSelection();

    if ((NAS_MMC_PLMN_SEARCH_SCENE_REG_NCELL == enPlmnSearchScene)
     || (NAS_MMC_PLMN_SEARCH_SCENE_REG_HPLMN == enPlmnSearchScene)
     || (NAS_MMC_PLMN_SEARCH_SCENE_REG_PREF_PLMN == enPlmnSearchScene)
     || (NAS_MMC_PLMN_SEARCH_SCENE_REG_ANY_PLMN == enPlmnSearchScene))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MMC_IsNeedSearchAvailPlmn_PlmnSelection(VOS_VOID)
{
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;

    enPlmnSearchScene = NAS_MMC_GetPlmnSearchScene_PlmnSelection();

    if ((NAS_MMC_PLMN_SELECTION_MODE_AUTO == NAS_MMC_GetPlmnSelectionMode())
     && (NAS_MMC_PLMN_SEARCH_SCENE_REG_HPLMN != enPlmnSearchScene)
     && (NAS_MMC_PLMN_SEARCH_SCENE_REG_PREF_PLMN != enPlmnSearchScene))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#ifdef  __cplusplus
#if  __cplusplus
  }
#endif
#endif
