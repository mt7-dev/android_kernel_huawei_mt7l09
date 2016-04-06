

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "MmaMmcInterface.h"
#include "TafMmaCtx.h"
#include "TafMmaSndApp.h"
#include "TafMmaTimerMgmt.h"
#include "TafMmaSndMmc.h"
#include "TafMmaComFunc.h"
#include "TafLog.h"
#include "TafMmaSndInternalMsg.h"
#include "TafMmaSndTaf.h"

#if (FEATURE_IMS == FEATURE_ON)
#include "TafMmaFsmImsSwitch.h"
#include "TafMmaFsmImsSwitchTbl.h"
#include "TafMmaSndImsa.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_FSM_IMS_SWITCH_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/

/*****************************************************************************
  6 函数实现
*****************************************************************************/

#if (FEATURE_IMS == FEATURE_ON)

VOS_UINT32 TAF_MMA_RcvTafImsSwitchSet_ImsSwitch_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_IMS_SWITCH_SET_REQ_STRU    *pstImsSwitchSet = VOS_NULL_PTR;

    pstImsSwitchSet = (TAF_MMA_IMS_SWITCH_SET_REQ_STRU*)pstMsg;

    /* 保存的入口消息 */
    TAF_MMA_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 打开IMS业务功能的流程 */
    if (TAF_MMA_IMS_SWITCH_SET_POWER_ON == pstImsSwitchSet->enImsSwitch)
    {
        /* 发送开机请求给IMSA */
        TAF_MMA_SndImsaStartReq(MMA_IMSA_START_TYPE_IMS_SWITCH_ON);

        /* 迁移状态到等待IMSA的开机回复 */
        TAF_MMA_FSM_SetCurrState(TAF_MMA_IMS_SWITCH_STA_WAIT_IMSA_START_CNF);

        /* 启动保护定时器 */
        TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_IMSA_START_CNF, TI_TAF_MMA_WAIT_IMSA_START_CNF_LEN);
    }
    else    /* 否则走关闭IMS业务功能的流程 */
    {
        /* 发送关机请求给IMSA */
        TAF_MMA_SndImsaStopReq(MMA_IMSA_STOP_TYPE_IMS_SWITCH_OFF);

        /* 迁移状态到等待IMSA的关机回复 */
        TAF_MMA_FSM_SetCurrState(TAF_MMA_IMS_SWITCH_STA_WAIT_IMSA_STOP_CNF);

        /* 启动保护定时器 */
        TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_IMSA_POWER_OFF_CNF, TI_TAF_MMA_WAIT_IMSA_POWER_OFF_CNF_LEN);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvImsaStartCnf_ImsSwitch_WaitImsaStartCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 停止保护定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_IMSA_START_CNF);

    /* 置IMS为已开机 */
    TAF_SDC_SetCurImsSwitchState(TAF_SDC_IMS_SWITCH_STATE_ON);

    /* 给上层回复成功 */
    TAF_MMA_ReportImsSwitchSetCnf_ImsSwitch(TAF_MMA_APP_OPER_RESULT_SUCCESS,
                                            TAF_ERR_NO_ERROR);

    /* 通知MMC当前IMS的开关状态 */
    TAF_MMA_SndMmcImsSwitchStateInd(MMA_MMC_IMS_SWITCH_STATE_ON);

    /* 通知IMSA 驻留信息 */
    TAF_MMA_SndImsaCampInfoChangeInd();

    /* 通知IMSA Service Change Ind*/
    TAF_MMA_SndImsaSrvInfoNotify((MMA_MMC_SERVICE_STATUS_ENUM_UINT32)TAF_SDC_GetPsServiceStatus());

    TAF_MMA_SndImsSwitchRsltInd(TAF_MMA_IMS_SWITCH_RESULT_ON_SUCC);

    /* 退出状态机 */
    TAF_MMA_FSM_QuitSubFsm();

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvTiWaitImsaStartCnfExpired_ImsSwitch_WaitImsaStartCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
    {
        vos_printf("\n*TAF_MMA_RcvTiWaitImsaStartCnfExpired_ImsSwitch_WaitImsaStartCnf\n");
    }

    /* warning打印 */
    TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvTiWaitImsaStartCnfExpired_ImsSwitch_WaitImsaStartCnf:time expired!");

    NAS_TRACE_HIGH("Enter TAF_MMA_RcvTiWaitImsaStartCnfExpired_ImsSwitch_WaitImsaStartCnf!");

    /* 置IMS为已关机 */
    TAF_SDC_SetCurImsSwitchState(TAF_SDC_IMS_SWITCH_STATE_OFF);

    /* 回退NV为不支持IMS业务，否则AP查询出来的结果和MODEM维护的不一致 */
    if (VOS_TRUE != TAF_MMA_UpdateLteImsSupportFlag(VOS_FALSE))
    {
        /* warning打印 */
        TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_UpdateLteImsSupportFlag fail!");

    }

    /* 给上层回复失败 */
    TAF_MMA_ReportImsSwitchSetCnf_ImsSwitch(TAF_MMA_APP_OPER_RESULT_FAILURE,
                                            TAF_ERR_IMS_STACK_TIMEOUT);

    TAF_MMA_SndImsSwitchRsltInd(TAF_MMA_IMS_SWITCH_RESULT_ON_FAIL);

    /* 退出状态机 */
    TAF_MMA_FSM_QuitSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvImsaStopCnf_ImsSwitch_WaitImsaStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 停止保护定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_IMSA_POWER_OFF_CNF);

    /* 置IMS为已关机 */
    TAF_SDC_SetCurImsSwitchState(TAF_SDC_IMS_SWITCH_STATE_OFF);

    /* 给上层回复成功 */
    TAF_MMA_ReportImsSwitchSetCnf_ImsSwitch(TAF_MMA_APP_OPER_RESULT_SUCCESS,
                                            TAF_ERR_NO_ERROR);

    /* 通知MMC当前IMS的开关状态 */
    TAF_MMA_SndMmcImsSwitchStateInd(MMA_MMC_IMS_SWITCH_STATE_OFF);

    /* 设置当前IMS VOICE不可用 */
    TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);

    /* 通知MMC当前IMS voice是否可用 */
    TAF_MMA_SndMmcImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    /* 通知SPM当前IMS VOICE是否可用 */
    TAF_MMA_SndSpmImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    TAF_MMA_SndImsSwitchRsltInd(TAF_MMA_IMS_SWITCH_RESULT_OFF_SUCC);

    /* 退出状态机 */
    TAF_MMA_FSM_QuitSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvTiWaitImsaStopCnfExpired_ImsSwitch_WaitImsaStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
    {
        vos_printf("\n*TAF_MMA_RcvTiWaitImsaStopCnfExpired_ImsSwitch_WaitImsaStopCnf\n");
    }

    /* warning打印 */
    TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvTiWaitImsaStopCnfExpired_ImsSwitch_WaitImsaStopCnf:time expired!");

    NAS_TRACE_HIGH("Enter!");

    /* 置IMS为已关机 */
    TAF_SDC_SetCurImsSwitchState(TAF_SDC_IMS_SWITCH_STATE_OFF);

    /* 给上层回复失败 */
    TAF_MMA_ReportImsSwitchSetCnf_ImsSwitch(TAF_MMA_APP_OPER_RESULT_FAILURE,
                                            TAF_ERR_IMS_STACK_TIMEOUT);

    /* 通知MMC当前IMS的开关状态 */
    TAF_MMA_SndMmcImsSwitchStateInd(MMA_MMC_IMS_SWITCH_STATE_OFF);

    /* 设置当前IMS VOICE不可用 */
    TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);

    /* 通知MMC当前IMS voice是否可用 */
    TAF_MMA_SndMmcImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    /* 通知SPM当前IMS VOICE是否可用 */
    TAF_MMA_SndSpmImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    TAF_MMA_SndImsSwitchRsltInd(TAF_MMA_IMS_SWITCH_RESULT_OFF_FAIL);

    /* 退出状态机 */
    TAF_MMA_FSM_QuitSubFsm();

    return VOS_TRUE;
}


VOS_VOID TAF_MMA_ReportImsSwitchSetCnf_ImsSwitch(
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enResult,
    TAF_ERROR_CODE_ENUM_UINT32          enErrorCause
)
{
    VOS_UINT32                          ulEventType;
    TAF_MMA_ENTRY_MSG_STRU             *pstEntryMsg     = VOS_NULL_PTR;
    TAF_MMA_IMS_SWITCH_SET_REQ_STRU    *pstImsSwitchSet = VOS_NULL_PTR;

    /* 根据FSM ID获取该FSM 入口消息 */
    pstEntryMsg = TAF_MMA_GetCurrFsmMsgAddr();
    ulEventType = pstEntryMsg->ulEventType;

    if (TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_IMS_SWITCH_SET_REQ) == ulEventType)
    {
        pstImsSwitchSet = (TAF_MMA_IMS_SWITCH_SET_REQ_STRU *)(pstEntryMsg->aucEntryMsgBuffer);

        TAF_MMA_SndImsSwitchSetCnf(&pstImsSwitchSet->stCtrl,
                                   enResult, enErrorCause);

    }

    return;

}

#endif /* FEATURE_IMS */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



