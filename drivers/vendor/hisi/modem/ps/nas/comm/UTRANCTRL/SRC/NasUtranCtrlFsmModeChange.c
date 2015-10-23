

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "NasComm.h"

#include "NasUtranCtrlCtx.h"
#include "NasUtranCtrlCommFunc.h"
#include "NasUtranCtrlFsmMain.h"
#include "Nasrrcinterface.h"
#include "NasUtranCtrlProcNvim.h"
#include "NasMmcTimerMgmt.h"
#include "NasUtranCtrlFsmModeChangeTbl.h"
#include "NasUtranCtrlFsmModeChange.h"
#include "NasMmlLib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_NAS_UTRANCTRL_FSM_MODE_CHANGE_C

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
  3 函数实现
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_TDS)

VOS_UINT32 NAS_UTRANCTRL_RcvWasSysInfo_ModeChange_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    /*  保持当前入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* W为主模时，将当前UtranMode通知L/G/TD */
#if (FEATURE_ON == FEATURE_LTE)
    if (VOS_TRUE == NAS_MML_IsPlatformSupportLte())
    {
        NAS_UTRANCTRL_SndLmmUtranModeReq(enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_LMM_UTRAN_MODE_CNF);
    }
#endif

    if (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
    {
        NAS_UTRANCTRL_SndGuAsUtranModeReq(UEPS_PID_GAS, enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_GAS_UTRAN_MODE_CNF);
    }

    /* 如果平台只支持W/TD中的一个，或者两者都不支持，会在NAS_UTRANCTRL_MsgProc或NAS_UTRANCTRL_ProcessMmcMsg返回VOS_FALSE
       不会进到Utran的状态机里，所以此处不需要判断平台是否支持W/TD */
    NAS_UTRANCTRL_SndGuAsUtranModeReq(TPS_PID_RRC, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_TD_UTRAN_MODE_CNF);

    /*迁到等待从模回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_SLAVEMODE_CNF);

    /*启等待从模回复定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTdSysInfo_ModeChange_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    /*  保持当前入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* TD为主模时，将当前UtranMode通知L/G/W */
#if (FEATURE_ON == FEATURE_LTE)
    if (VOS_TRUE == NAS_MML_IsPlatformSupportLte())
    {
        NAS_UTRANCTRL_SndLmmUtranModeReq(enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_LMM_UTRAN_MODE_CNF);
    }
#endif

    if (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
    {
        NAS_UTRANCTRL_SndGuAsUtranModeReq(UEPS_PID_GAS, enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_GAS_UTRAN_MODE_CNF);
    }

    /* 如果平台只支持W/TD中的一个，或者两者都不支持，会在NAS_UTRANCTRL_MsgProc或NAS_UTRANCTRL_ProcessMmcMsg返回VOS_FALSE
       不会进到Utran的状态机里，所以此处不需要判断平台是否支持W/TD */
    NAS_UTRANCTRL_SndGuAsUtranModeReq(WUEPS_PID_WRR, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_WAS_UTRAN_MODE_CNF);

    /*迁到等待从模回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_SLAVEMODE_CNF);

    /*启等待从模回复定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvGasSysInfo_ModeChange_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    /*  保持当前入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* G为主模时，将当前UtranMode通知L/W/TD */
#if (FEATURE_ON == FEATURE_LTE)
    if (VOS_TRUE == NAS_MML_IsPlatformSupportLte())
    {
        NAS_UTRANCTRL_SndLmmUtranModeReq(enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_LMM_UTRAN_MODE_CNF);
    }
#endif

    /* 如果平台只支持W/TD中的一个，或者两者都不支持，会在NAS_UTRANCTRL_MsgProc或NAS_UTRANCTRL_ProcessMmcMsg返回VOS_FALSE
       不会进到Utran的状态机里，所以此处不需要判断平台是否支持W/TD */
    NAS_UTRANCTRL_SndGuAsUtranModeReq(WUEPS_PID_WRR, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_WAS_UTRAN_MODE_CNF);

    NAS_UTRANCTRL_SndGuAsUtranModeReq(TPS_PID_RRC, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_TD_UTRAN_MODE_CNF);

    /*迁到等待从模回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_SLAVEMODE_CNF);

    /*启等待从模回复定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF_LEN);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 NAS_UTRANCTRL_RcvLmmSysInfo_ModeChange_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    /*  保持当前入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* L为主模时，将当前UtranMode通知G/W/TD */
    if (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
    {
        NAS_UTRANCTRL_SndGuAsUtranModeReq(UEPS_PID_GAS, enCurrUtranMode);
        NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_GAS_UTRAN_MODE_CNF);
    }

    /* 如果平台只支持W/TD中的一个，或者两者都不支持，会在NAS_UTRANCTRL_MsgProc或NAS_UTRANCTRL_ProcessMmcMsg返回VOS_FALSE
       不会进到Utran的状态机里，所以此处不需要判断平台是否支持W/TD */
    NAS_UTRANCTRL_SndGuAsUtranModeReq(WUEPS_PID_WRR, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_WAS_UTRAN_MODE_CNF);

    NAS_UTRANCTRL_SndGuAsUtranModeReq(TPS_PID_RRC, enCurrUtranMode);
    NAS_UTRANCTRL_SetWaitSlaveModeUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_TD_UTRAN_MODE_CNF);

    /*迁到等待从模回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_SLAVEMODE_CNF);

    /*启等待从模回复定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF_LEN);

    return VOS_TRUE;
}
#endif


VOS_UINT32 NAS_UTRANCTRL_RcvWasUtranModeCnf_ModeChange_WaitSlaveModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8  enMasterMode;

    enMasterMode = NAS_UTRANCTRL_GetMasterMode_ModeChange();

    /* 如果W为主模，不处理，直接返回 */
    if (NAS_UTRANCTRL_MASTER_MODE_WCDMA == enMasterMode)
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasUtranModeCnf_ModeChange_WaitSlaveModeCnf: ENTERED");

        return VOS_TRUE;
    }

    /* 收到WAS的UTRAN MODE CNF消息，清除等待标志 */
    NAS_UTRANCTRL_ClearWaitSlaveUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_WAS_UTRAN_MODE_CNF);

    /* 收到从模的回复后，调用处理从模回复的函数 */
    NAS_UTRANCTRL_ProcSlaveModeCnf_ModeChange(enMasterMode);

    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTdUtranModeCnf_ModeChange_WaitSlaveModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8  enMasterMode;

    enMasterMode = NAS_UTRANCTRL_GetMasterMode_ModeChange();

    /* 如果TD为主模，不处理，直接返回 */
    if (NAS_UTRANCTRL_MASTER_MODE_TDSCDMA == enMasterMode)
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdUtranModeCnf_ModeChange_WaitSlaveModeCnf: ENTERED");

        return VOS_TRUE;
    }

    /* 收到TDRRC的UTRAN MODE CNF消息，清除等待标志 */
    NAS_UTRANCTRL_ClearWaitSlaveUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_TD_UTRAN_MODE_CNF);

    /* 收到从模的回复后，调用处理从模回复的函数 */
    NAS_UTRANCTRL_ProcSlaveModeCnf_ModeChange(enMasterMode);

    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvGasUtranModeCnf_ModeChange_WaitSlaveModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8  enMasterMode;

    enMasterMode = NAS_UTRANCTRL_GetMasterMode_ModeChange();

    /* 如果G为主模，不处理，直接返回 */
    if (NAS_UTRANCTRL_MASTER_MODE_GSM == enMasterMode)
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdUtranModeCnf_ModeChange_WaitSlaveModeCnf: ENTERED");

        return VOS_TRUE;
    }

    /* 收到GAS的UTRAN MODE CNF消息，清除等待标志 */
    NAS_UTRANCTRL_ClearWaitSlaveUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_GAS_UTRAN_MODE_CNF);

    /* 收到从模的回复后，调用处理从模回复的函数 */
    NAS_UTRANCTRL_ProcSlaveModeCnf_ModeChange(enMasterMode);

    return VOS_TRUE;
}


#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 NAS_UTRANCTRL_RcvLmmUtranModeCnf_ModeChange_WaitSlaveModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8  enMasterMode;

    enMasterMode = NAS_UTRANCTRL_GetMasterMode_ModeChange();

    /* 如果L为主模，不处理，直接返回 */
    if (NAS_UTRANCTRL_MASTER_MODE_LTE == enMasterMode)
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvLmmUtranModeCnf_ModeChange_WaitSlaveModeCnf: ENTERED");

        return VOS_TRUE;
    }

    /* 收到TDRRC的UTRAN MODE CNF消息，清除等待标志 */
    NAS_UTRANCTRL_ClearWaitSlaveUtranModeCnfFlg_ModeChange(NAS_UTRANCTRL_WAIT_LMM_UTRAN_MODE_CNF);

    /* 收到从模的回复后，调用处理从模回复的函数 */
    NAS_UTRANCTRL_ProcSlaveModeCnf_ModeChange(enMasterMode);

    return VOS_TRUE;
}

#endif


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitSlaveModeUtranModeCnfExpired_ModeChange_WaitSlaveModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitSlaveModeUtranModeCnfExpired_ModeChange_WaitSlaveModeCnf: ENTERED");

    /* 向主模发模式通知 */
    NAS_UTRANCTRL_SndMasterModeUtranModeReq_ModeChange(NAS_UTRANCTRL_GetMasterMode_ModeChange());

    /* 启等待主模回复定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_MASTERMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_MASTERMODE_UTRAN_MODE_CNF_LEN);

    /* 迁到等待主模回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_MASTERMODE_CNF);

    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvMasterModeCnf_ModeChange_WaitMasterModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU                           *pstEntryMsg;

    /* 停等待主模回复定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_MASTERMODE_UTRAN_MODE_CNF);

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg( pstEntryMsg->ulEventType,
                                     (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    /* 设置缓存消息有效标志 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 退出MODE CHANGE状态机，切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitMasterModeCnfExpired_ModeChange_WaitMasterModeCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitMasterModeCnfExpired_ModeChange_WaitMasterModeCnf: ENTERED");

    /* 清除所有等待模式变更回复的标志 */
    NAS_UTRANCTRL_ClearAllWaitAsUtranModeCnfFlg_ModeChange();

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg( pstEntryMsg->ulEventType,
                                     (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    /* 设置缓存消息有效标志 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 退出MODE CHANGE状态机，切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    return VOS_FALSE;
}
VOS_VOID NAS_UTRANCTRL_ProcSlaveModeCnf_ModeChange(
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8                    enMasterMode
)
{
    VOS_UINT8                           ucWaitSlaveModeUtranModeCnfFlg;

    ucWaitSlaveModeUtranModeCnfFlg = NAS_UTRANCTRL_GetWaitSlaveModeUtranModeCnfFlg_ModeChange();

    if (NAS_UTRANCTRL_WAIT_UTRAN_MODE_CNF_NULL != ucWaitSlaveModeUtranModeCnfFlg)
    {
        return;
    }

    /* 如果已经收齐三个从模的回复，停等待从模回复定时器,给主模发模式变更请求，
       启等待主模回复定时器，迁到等待主模回复状态 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_SLAVEMODE_UTRAN_MODE_CNF);

    NAS_UTRANCTRL_SndMasterModeUtranModeReq_ModeChange(enMasterMode);

    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_MODE_CHANGE_STA_WAIT_MASTERMODE_CNF);

    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_MASTERMODE_UTRAN_MODE_CNF, TI_NAS_UTRANCTRL_WAIT_MASTERMODE_UTRAN_MODE_CNF_LEN);

    return;
}


VOS_VOID NAS_UTRANCTRL_SndMasterModeUtranModeReq_ModeChange(
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8                    enMasterMode
)
{

    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8                      enCurrUtranMode;

    enCurrUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();

    switch (enMasterMode)
    {
        case NAS_UTRANCTRL_MASTER_MODE_WCDMA:
            NAS_UTRANCTRL_SndGuAsUtranModeReq(WUEPS_PID_WRR, enCurrUtranMode);
            break;

        case NAS_UTRANCTRL_MASTER_MODE_TDSCDMA:
            NAS_UTRANCTRL_SndGuAsUtranModeReq(TPS_PID_RRC, enCurrUtranMode);
            break;

        case NAS_UTRANCTRL_MASTER_MODE_GSM:
            NAS_UTRANCTRL_SndGuAsUtranModeReq(UEPS_PID_GAS, enCurrUtranMode);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case NAS_UTRANCTRL_MASTER_MODE_LTE:
            NAS_UTRANCTRL_SndLmmUtranModeReq(enCurrUtranMode);
            break;
#endif

        default:
            /* 异常打印 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_SndMasterModeUtranModeReq_ModeChange: unexpected master mode.");
            break;
    }

    return;
}
NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8  NAS_UTRANCTRL_GetMasterMode_ModeChange(VOS_VOID)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU                           *pstEntryMsg;
    MSG_HEADER_STRU                                        *pstHead;
    NAS_UTRANCTRL_MASTER_MODE_ENUM_UINT8                    enMasterMode;

    /*  取保存的入口消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();
    pstHead     = (MSG_HEADER_STRU *)(pstEntryMsg->aucEntryMsgBuffer);

    switch (pstHead->ulSenderPid)
    {
        case WUEPS_PID_WRR:
            enMasterMode = NAS_UTRANCTRL_MASTER_MODE_WCDMA;
            break;

        case TPS_PID_RRC:
            enMasterMode = NAS_UTRANCTRL_MASTER_MODE_TDSCDMA;
            break;

        case UEPS_PID_GAS:
            enMasterMode = NAS_UTRANCTRL_MASTER_MODE_GSM;
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case PS_PID_MM:
            enMasterMode = NAS_UTRANCTRL_MASTER_MODE_LTE;
            break;
#endif

        default:
            /* 异常打印 */
            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitSlaveModeUtranModeCnfExpired_ModeChange_WaitSlaveModeCnf: ENTERED");
            enMasterMode = NAS_UTRANCTRL_MASTER_MODE_BUTT;
            break;
    }

    return enMasterMode;
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

