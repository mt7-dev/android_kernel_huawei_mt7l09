

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "NasFsm.h"
#include "NasMmcTimerMgmt.h"
#include "Nasrrcinterface.h"
#include "MmcGmmInterface.H"
#include "MmcMmInterface.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "MmcLmmInterface.h"
#endif

/* 删除ExtAppMmcInterface.h*/

#include "NasMmcSndInternalMsg.h"
#include "NasMmcFsmPlmnList.h"
#include "NasMmcFsmPLmnListTbl.h"

#include "MmaMmcInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_FSM_PLMN_LIST_TBL_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* NAS MMC状态机数组:list搜网层二状态机名称*/
NAS_FSM_DESC_STRU                       g_stNasMmcPlmnListFsmDesc;

/* NAS_MMC_PLMN_LIST_STA_INIT动作表 */
NAS_ACT_STRU   g_astNasPlmnListInitActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_Init),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_INTER_PLMN_LIST_REQ,
                     NAS_MMC_RcvInterPlmnListReq_PlmnList_Init),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_PERIOD_TRYING_USER_PLMN_LIST,
                     NAS_MMC_RcvTiPeriodTryingUserPlmnListExpired_PlmnList_Init),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_PERIOD_TRYING_INTER_PLMN_LIST,
                     NAS_MMC_RcvTiPeriodTryingInterPlmnListExpired_PlmnList_Init),
};

/* NAS_MMC_PLMN_LIST_STA_WAIT_RRC_CONN_REL_IND动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitRrcConnRelIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     MMCGMM_SIGNALING_STATUS_IND,
                     NAS_MMC_RcvGmmSignalingStatusInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMCMM_RR_CONN_INFO_IND,
                     NAS_MMC_RcvMmRrConnInfoInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMCMM_RR_REL_IND,
                     NAS_MMC_RcvMmRrRelInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_REL_IND,
                     NAS_MMC_RcvRrMmRelInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     MMCGMM_TBF_REL_IND,
                     NAS_MMC_RcvGmmTbfRelInd_PlmnList_WaitRrcConnRelInd),

    /*网络发起的detach*/
    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     MMCGMM_NETWORK_DETACH_IND,
                     NAS_MMC_RcvGmmNetworkDetachInd_PlmnList_WaitRrcConnRelInd),

    /*CS注册结果*/
    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMMMC_CS_REG_RESULT_IND,
                     NAS_MMC_RcvMmCsRegResultInd_PlmnList_WaitRrcConnRelInd),

    /*PS注册结果*/
    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     GMMMMC_PS_REG_RESULT_IND,
                     NAS_MMC_RcvGmmPsRegResultInd_PlmnList_WaitRrcConnRelInd),

    /* 收到MM的Abort消息的处理 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMMMC_ABORT_IND,
                     NAS_MMC_RcvMmAbortInd_PlmnList_WaitRrcConnRelInd),

    /* WAS系统消息 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SYS_INFO_IND,
                     NAS_MMC_RcvRrMmSysInfoInd_PlmnList_WaitRrcConnRelInd),

    /* GAS系统消息 */
    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     GRRMM_SYS_INFO_IND,
                     NAS_MMC_RcvGrrMmSysInfoInd_PlmnList_WaitRrcConnRelInd),

    /* 重选/HO/CCO状态机完成结果的处理 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_SUSPEND_RSLT_CNF,
                     NAS_MMC_RcvMmcMmcSuspendRslt_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SUSPEND_IND,
                     NAS_MMC_RcvRrMmSuspendInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_SUSPEND_IND,
                     NAS_MMC_RcvRrMmSuspendInd_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_RRC_CONN_REL,
                     NAS_MMC_RcvTiWaitRrcConnRelIndExpired_PlmnList_WaitRrcConnRelInd),

    NAS_ACT_TBL_ITEM( WUEPS_PID_WRR,
                      RRMM_LIMIT_SERVICE_CAMP_IND,
                      NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnList_WaitRrcConnRelInd),
};

#if (FEATURE_ON == FEATURE_LTE)
/* NAS_MMC_PLMN_LIST_STA_WAIT_EPS_CONN_REL_IND动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitEpsConnRelIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitEpsConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitEpsConnRelInd),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_SUSPEND_IND,
                     NAS_MMC_RcvLmmSuspendInd_PlmnList_WaitEpsConnRelInd),

    /* L模注册的过程中会收到信令链接存在的标志需要处理 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_STATUS_IND,
                      NAS_MMC_RcvLmmMmcStatusInd_PlmnList_WaitEpsConnRelInd),

    /* LMM系统消息 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_SYS_INFO_IND,
                      NAS_MMC_RcvLmmSysInfoInd_PlmnList_WaitEpsConnRelInd),

    /* LMM ATTACH指示 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_ATTACH_IND,
                      NAS_MMC_RcvLmmAttachInd_PlmnList_WaitEpsConnRelInd),

    /* LMM TAU结果 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_TAU_RESULT_IND,
                      NAS_MMC_RcvLmmTauResultInd_PlmnList_WaitEpsConnRelInd),

    /* LMM detach结果 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_DETACH_IND,
                      NAS_MMC_RcvLmmDetachInd_PlmnList_WaitEpsConnRelInd),

    /* 重选/HO/CCO状态机完成结果的处理 */
    NAS_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      MMCMMC_SUSPEND_RSLT_CNF,
                      NAS_MMC_RcvMmcMmcSuspendRslt_PlmnList_WaitEpsConnRelInd),

    NAS_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_NAS_MMC_WAIT_EPS_CONN_REL_IND,
                      NAS_MMC_RcvTiWaitEpsConnRelIndExpired_PlmnList_WaitEpsConnRelInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_IMS_VOICE_CAP_IND,
                     NAS_MMC_RcvMmaImsVoiceCapInd_PlmnList_WaitEpsConnRelInd),
};

/* NAS_MMC_PLMN_LIST_STA_WAIT_EPS_REG_IND动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitEpsRegIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitEpsRegInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitEpsRegInd),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_SUSPEND_IND,
                     NAS_MMC_RcvLmmSuspendInd_PlmnList_WaitEpsRegInd),

    /* LMM系统消息 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_SYS_INFO_IND,
                      NAS_MMC_RcvLmmSysInfoInd_PlmnList_WaitEpsRegInd),

    /* LMM ATTACH指示 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_ATTACH_IND,
                      NAS_MMC_RcvLmmAttachInd_PlmnList_WaitEpsRegInd),

    /* LMM TAU结果 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_TAU_RESULT_IND,
                      NAS_MMC_RcvLmmTauResultInd_PlmnList_WaitEpsRegInd),

    /* L模注册的过程中会收到信令链接存在的标志需要处理 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_STATUS_IND,
                      NAS_MMC_RcvLmmMmcStatusInd_PlmnList_WaitEpsRegInd),

    /* 等注册的时候发生丢网 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_AREA_LOST_IND,
                      NAS_MMC_RcvLmmAreaLostInd_PlmnList_WaitEpsRegInd),

    /* 重选/HO/CCO状态机完成结果的处理 */
    NAS_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      MMCMMC_SUSPEND_RSLT_CNF,
                      NAS_MMC_RcvMmcMmcSuspendRslt_PlmnList_WaitEpsRegInd),

    NAS_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_NAS_MMC_WAIT_EPS_REG_IND,
                      NAS_MMC_RcvTiWaitEpsRegIndExpired_PlmnList_WaitEpsRegInd),

    NAS_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      ID_MMA_MMC_IMS_VOICE_CAP_IND,
                      NAS_MMC_RcvMmaImsVoiceCapInd_PlmnList_WaitEpsRegInd),
};

#endif

/* NAS_MMC_PLMN_LIST_STA_WAIT_WAS_SYS_INFO_IND动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitWasSysInfoIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitWasSysInfoInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitWasSysInfoInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SYS_INFO_IND,
                     NAS_MMC_RcvRrMmSysInfoInd_PlmnList_WaitWasSysInfoInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvRrMmAreaLostInd_PlmnList_WaitWasSysInfoInd),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_WAS_SYS_INFO,
                     NAS_MMC_RcvTiWaitWasSysInfoIndExpired_PlmnList_WaitWasSysInfoInd),

};


/* NAS_MMC_PLMN_LIST_STA_WAIT_GAS_SYS_INFO_IND动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitGasSysInfoIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitGasSysInfoInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitGasSysInfoInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     GRRMM_SYS_INFO_IND,
                     NAS_MMC_RcvGrrMmSysInfoInd_PlmnList_WaitGasSysInfoInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvRrMmAreaLostInd_PlmnList_WaitGasSysInfoInd),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_GAS_SYS_INFO,
                     NAS_MMC_RcvTiWaitGasSysInfoIndExpired_PlmnList_WaitGasSysInfoInd),

};

/* NAS_MMC_PLMN_LIST_STA_WAIT_CSPS_REG_IND 动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitCsPsRegRsltIndActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     GMMMMC_PS_REG_RESULT_IND,
                     NAS_MMC_RcvGmmPsRegResultInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMMMC_CS_REG_RESULT_IND,
                     NAS_MMC_RcvMmCsRegResultInd_PlmnList_WaitCsPsRegRsltInd),

    /* 收到MM的Abort消息的处理 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MM,
                     MMMMC_ABORT_IND,
                     NAS_MMC_RcvMmAbortInd_PlmnList_WaitCsPsRegRsltInd),

    /*网络发起的detach*/
    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     MMCGMM_NETWORK_DETACH_IND,
                     NAS_MMC_RcvGmmNetworkDetachInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_GMM,
                     GMMMMC_SERVICE_REQUEST_RESULT_IND,
                     NAS_MMC_RcvGmmServiceRequestResultInd_PlmnList_WaitCsPsRegRsltInd),

    /* AS上报当前丢网 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvAreaLostInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvAreaLostInd_PlmnList_WaitCsPsRegRsltInd),

    /* 同系统重选 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SYS_INFO_IND,
                     NAS_MMC_RcvRrMmSysInfoInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     GRRMM_SYS_INFO_IND,
                     NAS_MMC_RcvGrrMmSysInfoInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SUSPEND_IND,
                     NAS_MMC_RcvRrMmSuspendInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_SUSPEND_IND,
                     NAS_MMC_RcvRrMmSuspendInd_PlmnList_WaitCsPsRegRsltInd),

    /* suspend状态机完成结果的处理 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_SUSPEND_RSLT_CNF,
                     NAS_MMC_RcvMmcMmcSuspendRslt_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_CSPS_REG_IND,
                     NAS_MMC_RcvTiWaitCsPsRegRsltIndExpired_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_LIMIT_SERVICE_CAMP_IND,
                     NAS_MMC_RcvRrmmLimitServiceCampInd_PlmnList_WaitCsPsRegRsltInd),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_IMS_VOICE_CAP_IND,
                     NAS_MMC_RcvMmaImsVoiceCapInd_PlmnList_WaitCsPsRegRsltInd),
};


/* NAS_MMC_PLMN_LIST_STA_WAIT_WAS_PLMN_SEARCH_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitWasPlmnSeachCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitWasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitWasPlmnSearchCnf),


    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_PLMN_SEARCH_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnList_WaitWasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_REL_IND,
                     NAS_MMC_RcvRrMmRelInd_PlmnList_WaitWasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SYS_INFO_IND,
                     NAS_MMC_RcvRrMmSysInfoInd_PlmnList_WaitWasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_WAS_PLMN_LIST_CNF,
                     NAS_MMC_RcvTiWaitWasPlmnListCnfExpired_PlmnList_WaitWasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvRrMmAreaLostInd_PlmnList_WaitAsPlmnSearchCnf),
};


/* NAS_MMC_PLMN_LIST_STA_WAIT_GAS_PLMN_SEARCH_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitGasPlmnSearchCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitGasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitGasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_PLMN_SEARCH_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnList_WaitGasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     GRRMM_SYS_INFO_IND,
                     NAS_MMC_RcvGrrMmSysInfoInd_PlmnList_WaitGasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_GAS_PLMN_LIST_CNF,
                     NAS_MMC_RcvTiWaitGasPlmnListCnfExpired_PlmnList_WaitGasPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_AREA_LOST_IND,
                     NAS_MMC_RcvRrMmAreaLostInd_PlmnList_WaitAsPlmnSearchCnf),
};

#if (FEATURE_ON == FEATURE_LTE)
/* NAS_MMC_PLMN_LIST_STA_WAIT_LMM_PLMN_SEARCH_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitLmmPlmnSearchCnfActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitLmmPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvMmcTafPlmnListAbortReq_PlmnList_WaitLmmPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_PLMN_SRCH_CNF,
                     NAS_MMC_RcvLmmPlmnSrchCnf_PlmnList_WaitLmmPlmnSearchCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_LMM_PLMN_LIST_CNF,
                     NAS_MMC_RcvTiWaitLmmPlmnListCnfExpired_PlmnList_WaitLmmPlmnSearchCnf),

    /* L下内部LIST搜网收到用户LIST搜网消息 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM( PS_PID_MM,
                      ID_LMM_MMC_AREA_LOST_IND,
                      NAS_MMC_RcvLmmAreaLostInd_PlmnList_WaitLmmPlmnSearchCnf),

    /* L list搜过程可能发生同系统重选，需要处理系统消息 */
    NAS_ACT_TBL_ITEM( PS_PID_MM,
                         ID_LMM_MMC_SYS_INFO_IND,
                         NAS_MMC_RcvLmmSysInfoInd_PlmnList_WaitLmmPlmnSearchCnf),
};

#endif
/* NAS_MMC_PLMN_LIST_STA_WAIT_WAS_SUSPEND_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitWasSuspendCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitWasSuspendCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitWasSuspendCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SUSPEND_CNF,
                     NAS_MMC_RcvRrMmSuspendCnf_PlmnList_WaitWasSuspendCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_WAS_SUSPEND_CNF,
                     NAS_MMC_RcvTiWaitWasSuspendCnfExpired_PlmnList_WaitWasSuspendCnf),

};

/* NAS_MMC_PLMN_LIST_STA_WAIT_GAS_SUSPEND_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitGasSuspendCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitGasSuspendCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortReq_PlmnList_WaitGasSuspendCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_SUSPEND_CNF,
                     NAS_MMC_RcvRrMmSuspendCnf_PlmnList_WaitGasSuspendCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_GAS_SUSPEND_CNF,
                     NAS_MMC_RcvTiWaitGasSuspendCnfExpired_PlmnList_WaitGasSuspendCnf),

};

#if (FEATURE_ON == FEATURE_LTE)
/* NAS_MMC_PLMN_LIST_STA_WAIT_LMM_SUSPEND_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitLmmSuspendCnfActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitLmmSuspendCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortFsmMsg_PlmnList_WaitLmmSuspendCnf),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_SUSPEND_CNF,
                     NAS_MMC_RcvLmmSuspendCnf_PlmnList_WaitLmmSuspendCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_LMM_SUSPEND_CNF,
                     NAS_MMC_RcvTiWaitLmmSuspendCnfExpired_PlmnList_WaitLmmSuspendCnf),

    /* L下内部LIST搜网与ID_LMM_MMC_NOT_CAMP_ON_IND消息对冲，在非驻留态下内部LIST搜网时收到用户LIST搜网消息 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

};

#endif

/* NAS_MMC_PLMN_LIST_STA_WAIT_WAS_STOP_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitWasPlmnSearchStopCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitWasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_PLMN_SEARCH_STOP_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchStopCnf_PlmnList_WaitWasPlmnSearchStopCnf),

    /* 同系统重选 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_SYS_INFO_IND,
                     NAS_MMC_RcvRrMmSysInfoInd_PlmnList_WaitWasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortFsmMsg_PlmnList_WaitWasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_PLMN_SEARCH_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnList_WaitWasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_WRR,
                     RRMM_REL_IND,
                     NAS_MMC_RcvRrMmRelInd_PlmnList_WaitWasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_WAS_PLMN_STOP_CNF,
                     NAS_MMC_RcvTiWaitWasStopCnfExpired_PlmnList_WaitWasPlmnSearchStopCnf),

};


/* NAS_MMC_PLMN_LIST_STA_WAIT_WAS_STOP_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitGasPlmnSearchStopCnfActTbl[]              =
{
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitGasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_PLMN_SEARCH_STOP_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchStopCnf_PlmnList_WaitGasPlmnSearchStopCnf),

    /* 同系统重选 */
    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     GRRMM_SYS_INFO_IND,
                     NAS_MMC_RcvGrrMmSysInfoInd_PlmnList_WaitGasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortFsmMsg_PlmnList_WaitGasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_GAS,
                     RRMM_PLMN_SEARCH_CNF,
                     NAS_MMC_RcvRrMmPlmnSrchCnf_PlmnList_WaitGasPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_GAS_PLMN_STOP_CNF,
                     NAS_MMC_RcvTiWaitGasStopCnfExpired_PlmnList_WaitGasPlmnSearchStopCnf),

};

#if (FEATURE_ON == FEATURE_LTE)
/* NAS_MMC_PLMN_LIST_STA_WAIT_LMM_STOP_CNF动作表 */
NAS_ACT_STRU   g_astNasPlmnListWaitLmmPlmnSearchStopCnfActTbl[]              =
{
    /* 用户LIST搜网过程中掉卡的情况 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMC,
                     MMCMMC_ABORT_FSM_REQ,
                     NAS_MMC_RcvMmcAbortFsmMsg_PlmnList_WaitLmmPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_ABORT_REQ,
                     NAS_MMC_RcvTafPlmnListAbortFsmMsg_PlmnList_WaitLmmPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_PLMN_SRCH_CNF,
                     NAS_MMC_RcvLmmPlmnSrchCnf_PlmnList_WaitLmmPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(PS_PID_MM,
                     ID_LMM_MMC_STOP_PLMN_SRCH_CNF,
                     NAS_MMC_RcvLmmPlmnSrchStopCnf_PlmnList_WaitLmmPlmnSearchStopCnf),

    NAS_ACT_TBL_ITEM(VOS_PID_TIMER,
                     TI_NAS_MMC_WAIT_LMM_PLMN_STOP_CNF,
                     NAS_MMC_RcvTiWaitLmmStopCnfExpired_PlmnList_WaitLmmPlmnSearchStopCnf),

    /* L下内部LIST搜网收到用户LIST搜网消息 */
    NAS_ACT_TBL_ITEM(WUEPS_PID_MMA,
                     ID_MMA_MMC_PLMN_LIST_REQ,
                     NAS_MMC_RcvTafPlmnListReq_PlmnList_NonInit),

};

#endif

/*Plmn List状态机*/
NAS_STA_STRU g_astNasMmcPlmnListStaTbl[] =
{
    /*Plmn List状态机初始化状态*/
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_INIT,
                     g_astNasPlmnListInitActTbl ),

    /* List搜网过程中等待RRC链路释放*/
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_RRC_CONN_REL_IND,
                      g_astNasPlmnListWaitRrcConnRelIndActTbl ),

    /* List搜网过程中等待WAS的系统消息 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_WAS_SYS_INFO_IND,
                      g_astNasPlmnListWaitWasSysInfoIndActTbl ),

    /* List搜网过程中等待GAS的系统消息 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_GAS_SYS_INFO_IND,
                      g_astNasPlmnListWaitGasSysInfoIndActTbl ),

    /* List搜网过程中等待CS/PS的注册结果 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_CSPS_REG_IND,
                      g_astNasPlmnListWaitCsPsRegRsltIndActTbl ),

    /* List搜网过程中等待WAS的搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_WAS_PLMN_SEARCH_CNF,
                      g_astNasPlmnListWaitWasPlmnSeachCnfActTbl ),

    /* List搜网过程中等待GAS的搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_GAS_PLMN_SEARCH_CNF,
                      g_astNasPlmnListWaitGasPlmnSearchCnfActTbl ),

    /* List搜网过程中等待WAS的挂起回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_WAS_SUSPEND_CNF,
                      g_astNasPlmnListWaitWasSuspendCnfActTbl ),

    /* List搜网过程中等待GAS的挂起回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_GAS_SUSPEND_CNF,
                      g_astNasPlmnListWaitGasSuspendCnfActTbl ),

    /* List搜网过程中等待WAS的停止搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_WAS_PLMN_SEARCH_STOP_CNF,
                      g_astNasPlmnListWaitWasPlmnSearchStopCnfActTbl ),

    /* List搜网过程中等待GAS的停止搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_GAS_PLMN_SEARCH_STOP_CNF,
                      g_astNasPlmnListWaitGasPlmnSearchStopCnfActTbl ),

#if (FEATURE_ON == FEATURE_LTE)

    /* List搜网过程中等待EPS链路释放 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_EPS_CONN_REL_IND,
                      g_astNasPlmnListWaitEpsConnRelIndActTbl ),

    /* List搜网过程中等待EPS注册结果 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_EPS_REG_IND,
                      g_astNasPlmnListWaitEpsRegIndActTbl ),

    /* List搜网过程中等待LMM的搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_LMM_PLMN_SEARCH_CNF,
                      g_astNasPlmnListWaitLmmPlmnSearchCnfActTbl ),

    /* List搜网过程中等待LMM的挂起回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_LMM_SUSPEND_CNF,
                      g_astNasPlmnListWaitLmmSuspendCnfActTbl ),

    /* List搜网过程中等待LMM的停止搜网回复 */
    NAS_STA_TBL_ITEM( NAS_MMC_PLMN_LIST_STA_WAIT_LMM_PLMN_SEARCH_STOP_CNF,
                      g_astNasPlmnListWaitLmmPlmnSearchStopCnfActTbl ),

#endif
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 NAS_MMC_GetPlmnListStaTblSize( VOS_VOID  )
{
    return (sizeof(g_astNasMmcPlmnListStaTbl)/sizeof(NAS_STA_STRU));
}



NAS_FSM_DESC_STRU * NAS_MMC_GetPlmnListFsmDescAddr(VOS_VOID)
{
    return (&g_stNasMmcPlmnListFsmDesc);
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

