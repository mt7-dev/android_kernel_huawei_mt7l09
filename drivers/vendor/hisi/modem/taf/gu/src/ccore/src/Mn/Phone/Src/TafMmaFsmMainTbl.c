

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "TafFsm.h"
#include "TafMmaFsmMainTbl.h"
#include "MmaAppLocal.h"
#include "TafMmaFsmMain.h"
#include "TafMmaSndInternalMsg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_FSM_MAIN_TBL_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


TAF_FSM_DESC_STRU                       g_stTafMmaMainFsmDesc;


TAF_ACT_STRU   g_astTafMmaMainActTbl[]                      =
{
    /* OM的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_OM,
                      OAM_MMA_PHONE_MODE_SET_REQ,
                      TAF_MMA_RcvAppPhoneModeSetReq_Main),

    /* 内部消息的处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_INTER_POWER_INIT,
                      TAF_MMA_RcvMmaInterPowerInit_Main ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_INTER_USIM_STATUS_CHANGE_IND,
                      TAF_MMA_RcvMmaInterUsimStatusChangeInd_Main ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_PHONE_MODE_RSLT_IND,
                      TAF_MMA_RcvMmaPhoneModeRsltInd_Main),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_SIM_LOCK_STATUS_CHANGE_IND,
                      TAF_MMA_RcvMmaSimlockStatusChangeInd_Main ),

#if (FEATURE_IMS == FEATURE_ON)
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_IMS_SWITCH_RSLT_IND,
                      TAF_MMA_RcvMmaImsSwitchRsltInd_Main),
#endif

    /* USIM的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,
                      PS_USIM_GET_STATUS_IND,
                      TAF_MMA_RcvPihUsimStatusInd_Main),

    TAF_ACT_TBL_ITEM( MAPS_PIH_PID,
                      PS_USIM_GET_STATUS_IND,
                      TAF_MMA_RcvPihUsimStatusInd_Main),

    TAF_ACT_TBL_ITEM( MAPS_PIH_PID,
                      PS_USIM_REFRESH_IND,
                      TAF_MMA_RcvUsimRefreshIndMsg_Main),

    /* MMC的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_ATTACH_CNF,
                      Sta_AttachCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_DETACH_CNF,
                      Sta_DetachCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_DETACH_IND,
                      Sta_DetachInd),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_LIST_CNF,
                      Sta_PlmnListInd),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_LIST_REJ,
                      Sta_PlmnListRej),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_LIST_ABORT_CNF,
                      TAF_MMA_RcvPlmnListAbortCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF,
                      TAF_MMA_RcvSpecPlmnSearchAbortCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_SPECIAL_SEL_CNF,
                      TAF_MMA_RcvMmcPlmnSpecialSelCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_RESEL_CNF,
                      TAF_MMA_RcvMmcPlmnReselCnf),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_PLMN_SPECIAL_SEL_REJ,
                      TAF_MMA_RcvMmcUserPlmnRej),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_SYS_CFG_CNF,
                      Sta_SysCfgCnf),


    /* CMMCA/AT的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_PHONE_MODE_SET_REQ,
                      TAF_MMA_RcvTafPhoneModeSetReq_Main),

    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_SYS_CFG_SET_REQ,
                      TAF_MMA_RcvSysCfgSetReq_Main),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_PLMN_LIST,
                      MMA_PhoneProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_PLMN_LIST_ABORT_REQ,
                      MMA_PhoneProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_PLMN_USER_SEL,
                      MMA_SetPlmnUserSel),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_PLMN_RESEL,
                      MMA_PhoneProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_ATTACH,
                      MMA_PhoneProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_DETACH_REQ,
                      TAF_MMA_RcvDetachReq_Main),

#if (FEATURE_IMS == FEATURE_ON)
    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_IMS_SWITCH_SET_REQ,
                      TAF_MMA_RcvTafImsSwitchSetReq_Main),
#endif
};



TAF_STA_STRU g_astTafMmaMainStaTbl[] =
{
    TAF_STA_TBL_ITEM( TAF_MMA_MAIN_STA_IDLE,
                      g_astTafMmaMainActTbl ),
};
VOS_UINT32 TAF_MMA_GetMainStaTblSize( VOS_VOID  )
{
    return (sizeof(g_astTafMmaMainStaTbl)/sizeof(TAF_STA_STRU));
}


TAF_FSM_DESC_STRU * TAF_MMA_GetMainFsmDescAddr(VOS_VOID)
{
    return (&g_stTafMmaMainFsmDesc);
}

/*****************************************************************************
  3 函数实现
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

