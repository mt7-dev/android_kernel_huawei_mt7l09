

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "NasComm.h"
#include "MmaMmcInterface.h"
#include "NasUtranCtrlCtx.h"
#include "NasUtranCtrlCommFunc.h"
#include "NasUtranCtrlFsmMain.h"
#include "Nasrrcinterface.h"
#include "NasUtranCtrlProcNvim.h"
#include "NasMmcTimerMgmt.h"
#include "NasUtranCtrlFsmPlmnSelectionTbl.h"
#include "NasMmlLib.h"
#include "NasMmcComFunc.h"

#include "NasMmcFsmPlmnSelection.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_NAS_UTRANCTRL_FSM_PLMN_SELECTION_C

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

VOS_UINT32 NAS_UTRANCTRL_RcvWasPlmnSrchCnf_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* W下搜网失败需要在TD下继续搜网 挂起WAS */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterSkipSearchWasIndMsg_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* W下搜网失败需要在TD下继续搜网 挂起WAS */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterSkipSearchTdsIndMsg_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* tds下搜网失败需要在w下继续搜网 挂起tds */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TDS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 向WAS发送停止搜网请求 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(WUEPS_PID_WRR);

    /* 迁移状态到等到停止W搜网后去TD下搜网状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_PLMN_STOP_CNF_SEARCH_TD);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;

}
VOS_UINT32 NAS_UTRANCTRL_RcvWasRrMmRelInd_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 将构造的搜网消息保存到状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* W下搜网失败需要在TD下继续搜网 挂起WAS */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;

}
VOS_UINT32 NAS_UTRANCTRL_RcvTdPlmnSrchCnf_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* TD下搜网失败根据携带的搜网列表不为空的情况下则退出当前搜网过程,
       否则需要挂起TD到W下搜网 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TD挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_PlmnSelection_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 保存状态机入口消息 */
    NAS_UTRANCTRL_SaveCurEntryMsg(ulEventType, pstMsg);

    /* 向TD发送停止搜网请求 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(TPS_PID_RRC);

    /* 迁移状态到等到停止TD搜网后去W下搜网状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_PLMN_STOP_CNF_SEARCH_W);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;

}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnStopCnf_PlmnSelection_WaitWasPlmnStopCnfSearchTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF);

    /* 如果当前存在打断，则直接退出选网状态机 */
    if ( VOS_TRUE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 切换到MAIN状态机 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 如果当前不存在打断，当前需要继续挂起当前模进行搜网 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvRrmmRelInd_PlmnSelection_WaitWasPlmnStopCnfSearchTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvRrmmRelInd_PlmnSelection_WaitWasPlmnStopCnfSearchTD: ENTERED");

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF);

    if ( VOS_TRUE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 切换到MAIN状态机 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 将搜网失败消息发给MMC处理 */
        return VOS_FALSE;
    }

    /* W下搜网失败需要在TD下继续搜网 挂起WAS */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续进入MMC状态机处理 */
    return VOS_TRUE;

}


VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitWasPlmnStopCnfSearchTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}

VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPlmnStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnfSearchTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPlmnStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnfSearchTD: ENTERED");

    /* 如果当前存在打断，则直接退出选网状态机 */
    if ( VOS_TRUE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 切换到MAIN状态机 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 将搜网失败消息发给MMC处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到TD下,向WAS发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到was挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnStopCnf_PlmnSelection_WaitTdPlmnStopCnfSearchW(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF);

    /* 如果当前存在打断，则直接退出选网状态机 */
    if ( VOS_TRUE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 切换到MAIN状态机 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 如果当前不存在打断，当前需要继续挂起当前模进行搜网 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TD挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF);

    /* 启动定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitTdPlmnStopCnfSearchW(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}

VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPlmnStopCnfExpired_PlmnSelection_WaitTdPlmnStopCnfSearchW(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPlmnStopCnfExpired_PlmnSelection_WaitTdPlmnStopCnfSearchW: ENTERED");

    /* 如果当前存在打断，则直接退出选网状态机 */
    if ( VOS_TRUE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 切换到MAIN状态机 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 将搜网失败消息发给MMC处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到w下搜网,向TD发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TD挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}




VOS_UINT32 NAS_UTRANCTRL_RcvRrmmSuspendCnf_PlmnSelection_WaitWasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;
    RRMM_SUSPEND_CNF_ST                *pstRrmmSuspendCnf = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    pstRrmmSuspendCnf  = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    if (MMC_AS_SUSPEND_FAILURE == pstRrmmSuspendCnf->ucResult)
    {
        /* 单板复位 */
        NAS_MML_SoftReBoot();
        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF);

    /* 状态机打断标记不存在 */
    if (VOS_FALSE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 向TDRRC发送指定搜网请求 */
        NAS_UTRANCTRL_SndGuAsSpecPlmnSearchReq(TPS_PID_RRC);

        /* 迁移状态到等到TDRRC搜网回复 */
        NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_PLMN_SEARCH_CNF);

        /* 启动定时器 */
        NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF, TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF_LEN);

        /* 消息处理完成不需继续进入MMC状态机处理 */
        return VOS_TRUE;
    }

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;


    /* 如果当前的入口消息为定时器超时,或者MMC发送的内部SKIP 搜索W消息，则需要构造搜网回复失败消息 */
    if ( (VOS_PID_TIMER == pstMsgHeader->ulSenderPid)
      || ( (WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
        && (MMCMMC_INTER_SKIP_SEARCH_W_IND == pstMsgHeader->ulMsgName) ) )
    {
    
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                   (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );
    }
    else
    {
        /* 取缓存的状态机搜网回复失败消息给MMC模块 */
        NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(pstEntryMsg->ulEventType,
                                (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    }

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitWasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 设置状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasSuspendCnfExpired_PlmnSelection_WaitWasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasSuspendCnfExpired_PlmnSelection_WaitWasSuspendCnf: ENTERED");

    /* 复位 */
    NAS_MML_SoftReBoot();

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmSuspendCnf_PlmnSelection_WaitTdSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;
    RRMM_SUSPEND_CNF_ST                *pstRrmmSuspendCnf = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();
    pstRrmmSuspendCnf  = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    if (MMC_AS_SUSPEND_FAILURE == pstRrmmSuspendCnf->ucResult)
    {
        /* 单板复位 */
        NAS_MML_SoftReBoot();
        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF);

    /* 状态机打断标记不存在 */
    if (VOS_FALSE == NAS_UTRANCTRL_GetAbortFlg_PlmnSelection())
    {
        /* 向WAS发送指定搜网请求 */
        NAS_UTRANCTRL_SndGuAsSpecPlmnSearchReq(WUEPS_PID_WRR);

        /* 迁移状态到等到WAS搜网回复 */
        NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_PLMN_SEARCH_CNF);

        /* 启动定时器 */
        NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF_LEN);

        /* 消息处理完成不需继续处理 */
        return VOS_TRUE;
    }

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果当前的入口消息为定时器超时，或者MMC发送的内部SKIP 搜索TDS消息,则需要构造搜网回复失败消息 */
    if ((VOS_PID_TIMER == pstMsgHeader->ulSenderPid)
      || ((WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
       && (MMCMMC_INTER_SKIP_SEARCH_TDS_IND == pstMsgHeader->ulMsgName)))
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                   (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );
    }
    else
    {
        /* 取缓存的状态机搜网回复失败消息给MMC模块 */
        NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(pstEntryMsg->ulEventType,
                                (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    }

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitTdSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 设置状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdSuspendCnfExpired_PlmnSelection_WaitTdSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdSuspendCnfExpired_PlmnSelection_WaitTdSuspendCnf: ENTERED");

    /* 复位 */
    NAS_MML_SoftReBoot();

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnSearchCnf_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg;

    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    pstSrchCnfMsg = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 如果搜网成功 */
    if (RRC_PLMN_SEARCH_RLT_SPEC == pstSrchCnfMsg->ulSearchRlt)
    {
        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 进MMC状态机处理 */
        return VOS_FALSE;
    }

    
    /* 如果搜网失败，原因为NO RF，则直接进MMC处理，设置当前为FDD模式 */
    if (RRC_PLMN_SEARCH_RLT_SPEC_NO_RF == pstSrchCnfMsg->ulSearchRlt)
    {
        NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);
        
        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 进MMC状态机处理 */
        return VOS_FALSE;
    }    

    /* 根据搜网列表更新中国网络标记 */
    if (MMCMMC_INTER_SKIP_SEARCH_TDS_IND != pstMsgHeader->ulMsgName)
    {
        NAS_UTRANCTRL_UpdateSearchedSpecTdMccFLg(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                 NAS_UTRANCTRL_GetSpecTdMccList(),
                                                 &(pstSrchCnfMsg->PlmnIdList));

        /* 根据中国网络标记设置utran工作模式 */
        if (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
        {
            NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);
        }
        else if ((0 != pstSrchCnfMsg->PlmnIdList.ulHighPlmnNum)
              || (0 != pstSrchCnfMsg->PlmnIdList.ulLowPlmnNum))
        {
            /* 未搜到中国网络并且W携带非空列表,更新工作模式为FDD */
            NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);
        }
        else
        {
            /* W携带空列表不修改工作模式 */
        }
    }
	
    /* 根据当前工作模式为TDD则需要回到TD下 */
    if (NAS_UTRANCTRL_UTRAN_MODE_TDD   == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

        /* 迁移状态到等待回TD时W的挂起回复状态 */
        NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_BACK_TD);

        /* 启动保护定时器 */
        NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

        /* 消息处理完成不需继续处理 */
        return VOS_TRUE;
    }

    /* 状态机切换到MAIN */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvInterAbortUtranCtrlPlmnSearchReq_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 更新utran mode为fdd，utranctrl 搜网状态机退出，消息进mmc继续处理，通知接入层打断由mmc处理 */
    NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);

    /* 通知mmc MMCMMC_INTER_ABORT_UTRAN_CTRL_PLMN_SEARCH_CNF*/
    NAS_MMC_SndInterAbortUtranCtrlPlmnSearchCnfMsg();

    /* 状态机切换到MAIN */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    return VOS_TRUE;
}






VOS_UINT32  NAS_UTRANCTRL_RcvTdPlmnSearchCnf_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSearchCnf_PlmnSelection_WaitWasPlmnSearchCnf: illegal MESSAGE! ");

    /* 丢弃该非法消息 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmRelInd_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvRrmmRelInd_PlmnSelection_WaitWasPlmnSearchCnf: ENTERED");

    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 当前未搜到中国的网络,则认为仍然在TD下,模式保持不变 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等待回TD时W的挂起回复状态 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_BACK_TD);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 向WAS发送停止搜网请求 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS搜网回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_PLMN_STOP_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_PlmnSelection_WaitWasPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_PlmnSelection_WaitWasPlmnSearchCnf: ENTERED");

    /* 停止WAS搜网 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(WUEPS_PID_WRR);

    /* 迁移状态到等到WAS停止搜网回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_PLMN_STOP_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnSearchCnf_PlmnSelection_WaitTdPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg = VOS_NULL_PTR;

    pstSrchCnfMsg = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF);

    /* 如果搜网成功 */
    if (RRC_PLMN_SEARCH_RLT_SPEC == pstSrchCnfMsg->ulSearchRlt)
    {
        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 进MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 根据搜网列表更新中国网络标记 */
    NAS_UTRANCTRL_UpdateSearchedSpecTdMccFLg(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                             NAS_UTRANCTRL_GetSpecTdMccList(),
                                             &(pstSrchCnfMsg->PlmnIdList));

    /* 根据中国网络标记设置utran工作模式 */
    if (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
    {
        NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);
    }

    /* 根据当前工作模式为FDD则需要回到W下 */
    if (NAS_UTRANCTRL_UTRAN_MODE_FDD   == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        /* 挂起TD */
        NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

        /* 迁移状态到等待回W时TD的挂起回复状态 */
        NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF_BACK_W);

        /* 启动保护定时器 */
        NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

        /* 消息处理完成不需继续处理 */
        return VOS_TRUE;
    }

    /* 状态机切换到MAIN */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}
VOS_UINT32  NAS_UTRANCTRL_RcvWasPlmnSearchCnf_PlmnSelection_WaitTdPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSearchCnf_PlmnSelection_WaitTdPlmnSearchCnf: illegal MESSAGE! ");

    /* 丢弃该非法消息 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitTdPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 向TDRRC发送停止搜网请求 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(TPS_PID_RRC);

    /* 迁移状态到等到TDRRC停止搜网回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_PLMN_STOP_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_PlmnSelection_WaitTdPlmnSearchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_PlmnSelection_WaitTdPlmnSearchCnf: ENTERED");

    /* 停止TDRRC搜网 */
    NAS_UTRANCTRL_SndGuAsPlmnStopReq(TPS_PID_RRC);

    /* 迁移状态到等到TDRRC停止搜网回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_PLMN_STOP_CNF);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF, TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnStopCnf_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;

    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_STOP_CNF);

    if ((VOS_FALSE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
     && (WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
     && (MMCMMC_INTER_SKIP_SEARCH_TDS_IND == pstMsgHeader->ulMsgName))
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到TD下,向WAS发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到was挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_BACK_TD);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}


VOS_UINT32  NAS_UTRANCTRL_RcvTdPlmnSearchCnf_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSearchCnf_PlmnSelection_WaitWasPlmnStopCnf: illegal MESSAGE! ");

    /* 丢弃该非法消息 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPlmnStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;

    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPlmnStopCnfExpired_PlmnSelection_WaitWasPlmnStopCnf: ENTERED");

    if ((VOS_FALSE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
     && (WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
     && (MMCMMC_INTER_SKIP_SEARCH_TDS_IND == pstMsgHeader->ulMsgName))
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到TD下,向WAS发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(WUEPS_PID_WRR);

    /* 迁移状态到等到was挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_WAS_SUSPEND_CNF_BACK_TD);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvRrmmPlmnStopCnf_PlmnSelection_WaitTdPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
     NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;
    

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_STOP_CNF);

    
    /* 中国网络标记存在则退出当前状态机 */
    if ( (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg()) 
      || ( (WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
        && (MMCMMC_INTER_SKIP_SEARCH_W_IND == pstMsgHeader->ulMsgName) ) )
    {

        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到W下,向TDRRC发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TDRRC挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF_BACK_W);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}



VOS_UINT32  NAS_UTRANCTRL_RcvWasPlmnStopCnf_PlmnSelection_WaitTdPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnStopCnf_PlmnSelection_WaitTdPlmnStopCnf: illegal MESSAGE! ");

    /* 丢弃该非法消息 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvUserReqMsg_PlmnSelection_WaitTdPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 记录状态机打断标记 */
    NAS_UTRANCTRL_SetAbortFlg_PlmnSelection(VOS_TRUE);

    /* 缓存当前用户请求消息 */
    NAS_UTRANCTRL_SaveCacheMsgInMsgQueue(ulEventType, pstMsg);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPlmnStopCnfExpired_PlmnSelection_WaitTdPlmnStopCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;
    

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPlmnStopCnfExpired_PlmnSelection_WaitTdPlmnStopCnf: ENTERED");

    
    /* 中国网络标记存在则退出当前状态机 */
    if ( (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg()) 
      || ( (WUEPS_PID_MMC                  == pstMsgHeader->ulSenderPid)
        && (MMCMMC_INTER_SKIP_SEARCH_W_IND == pstMsgHeader->ulMsgName) ) )
    {
    
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                   (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 状态机切换到MAIN */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

        /* 消息处理完成继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 此时需要回到W下,向TDRRC发送挂起请求 */
    NAS_UTRANCTRL_SndGuAsSuspendReq(TPS_PID_RRC);

    /* 迁移状态到等到TDRRC挂起回复 */
    NAS_UTRANCTRL_FSM_SetCurrState(NAS_UTRANCTRL_PLMN_SELECTION_STA_WAIT_TD_SUSPEND_CNF_BACK_W);

    /* 启动保护定时器 */
    NAS_UTRANCTRL_StartTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF, TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF_LEN);

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvRrmmSuspendCnf_PlmnSelection_WaitWasSuspendCnfBackTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;
    RRMM_SUSPEND_CNF_ST                *pstRrmmSuspendCnf = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    pstRrmmSuspendCnf  = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    if (MMC_AS_SUSPEND_FAILURE == pstRrmmSuspendCnf->ucResult)
    {
        /* 单板复位 */
        NAS_MML_SoftReBoot();
        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SUSPEND_CNF);

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果当前的入口消息为定时器超时，则需要构造搜网回复失败消息 */
    if ( VOS_PID_TIMER == pstMsgHeader->ulSenderPid )
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );
    }
    else
    {
        /* 取缓存的状态机搜网回复失败消息给MMC模块 */
        NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(pstEntryMsg->ulEventType,
                                (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    }

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasSuspendCnfExpired_PlmnSelection_WaitWasSuspendCnfBackTD(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasSuspendCnfExpired_PlmnSelection_WaitWasSuspendCnfBackTD: ENTERED");

    /* 复位 */
    NAS_MML_SoftReBoot();

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}
VOS_UINT32 NAS_UTRANCTRL_RcvRrmmSuspendCnf_PlmnSelection_WaitTdSuspendCnfBackW(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstEntryMsg       = VOS_NULL_PTR;
    RRMM_SUSPEND_CNF_ST                *pstRrmmSuspendCnf = VOS_NULL_PTR;

    MSG_HEADER_STRU                    *pstMsgHeader      = VOS_NULL_PTR;
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    pstRrmmSuspendCnf  = (RRMM_SUSPEND_CNF_ST*)pstMsg;

    if (MMC_AS_SUSPEND_FAILURE == pstRrmmSuspendCnf->ucResult)
    {
        /* 单板复位 */
        NAS_MML_SoftReBoot();
        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_SUSPEND_CNF);

    /* 取当前缓存的系统消息 */
    pstEntryMsg = NAS_UTRANCTRL_GetCurrEntryMsgAddr();

    /* 取得当前缓存消息的头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果当前的入口消息为定时器超时，则需要构造搜网回复失败消息 */
    if ( VOS_PID_TIMER == pstMsgHeader->ulSenderPid )
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                   (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );
    }
    else
    {
        /* 取缓存的状态机搜网回复失败消息给MMC模块 */
        NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(pstEntryMsg->ulEventType,
                                (struct MsgCB *)pstEntryMsg->aucEntryMsgBuffer);

    }

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 切换到MAIN状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MAIN);

    /* 消息处理完成继续进入MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdSuspendCnfExpired_PlmnSelection_WaitTdSuspendCnfBackW(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdSuspendCnfExpired_PlmnSelection_WaitTdSuspendCnfBackW: ENTERED");

    /* 复位 */
    NAS_MML_SoftReBoot();

    /* 消息处理完成不需继续处理 */
    return VOS_TRUE;
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
