

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafLog.h"
#include "TafMmaMntn.h"
#include "Taf_Status.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_MNTN_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
extern STATUS_CONTEXT_STRU                      g_StatusContext;


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID  TAF_MMA_LogFsmInfo(VOS_VOID)
{
    TAF_MMA_LOG_FSM_INFO_STRU          *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_MMA_LOG_FSM_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA, sizeof(TAF_MMA_LOG_FSM_INFO_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_MMA_LogFsmInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_FSM_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_FSM_INFO_IND;

    pstMsg->ulState                     = TAF_MMA_FSM_GetFsmTopState();
    pstMsg->enFsmId                     = TAF_MMA_GetCurrFsmId();

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

    return;
}



VOS_VOID  TAF_MMA_LogBufferQueueMsg(
    VOS_UINT32                          ulFullFlg
)
{
    TAF_MMA_LOG_BUffER_MSG_INFO_STRU    *pstBufferMsg = VOS_NULL_PTR;
    TAF_MMA_MSG_QUEUE_STRU              *pstMsgQueue  = VOS_NULL_PTR;

    pstBufferMsg = (TAF_MMA_LOG_BUffER_MSG_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                             sizeof(TAF_MMA_LOG_BUffER_MSG_INFO_STRU));
    if ( VOS_NULL_PTR == pstBufferMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LogBufferQueueMsg:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsgQueue                               = TAF_MMA_GetCachMsgBufferAddr();
    pstBufferMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstBufferMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstBufferMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstBufferMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_BUffER_MSG_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstBufferMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_BUFFER_MSG_INFO_IND;
    pstBufferMsg->ulFullFlg                   = ulFullFlg;
    PS_MEM_CPY(&(pstBufferMsg->stMsgQueue), pstMsgQueue, sizeof(pstBufferMsg->stMsgQueue));

    OM_TraceMsgHook(pstBufferMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstBufferMsg);

    return;
}



VOS_VOID TAF_MMA_LogOperCtxInfo(VOS_VOID)
{
    TAF_MMA_LOG_OPER_CTX_INFO_STRU     *pstBufferMsg    = VOS_NULL_PTR;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;

    pstBufferMsg = (TAF_MMA_LOG_OPER_CTX_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                             sizeof(TAF_MMA_LOG_OPER_CTX_INFO_STRU));
    if ( VOS_NULL_PTR == pstBufferMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LogOperCtxInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMmaOperCtx                             = TAF_MMA_GetOperCtxAddr();
    pstBufferMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstBufferMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstBufferMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstBufferMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstBufferMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_OPER_CTX_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstBufferMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_OPER_CTX_INFO_IND;
    PS_MEM_CPY(pstBufferMsg->astMmaOperCtx, pstMmaOperCtx, sizeof(pstBufferMsg->astMmaOperCtx));

    OM_TraceMsgHook(pstBufferMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstBufferMsg);

    return;
}





VOS_VOID  TAF_MMA_SndOmInternalMsgQueueInfo(
    VOS_UINT8                          ucFullFlg,
    VOS_UINT8                          ucMsgLenValidFlg
)
{
    TAF_MMA_LOG_INTER_MSG_INFO_STRU     *pstInterMsg = VOS_NULL_PTR;

    pstInterMsg = (TAF_MMA_LOG_INTER_MSG_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                             sizeof(TAF_MMA_LOG_INTER_MSG_INFO_STRU));

    if ( VOS_NULL_PTR == pstInterMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndOmInternalMsgQueueInfo:ERROR:Alloc Mem Fail.");
        return;
    }

    pstInterMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstInterMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstInterMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstInterMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_INTER_MSG_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstInterMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_INTER_MSG_INFO_IND;
    pstInterMsg->ucFullFlg                   = ucFullFlg;
    pstInterMsg->ucMsgLenValidFlg            = ucMsgLenValidFlg;

    OM_TraceMsgHook(pstInterMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstInterMsg);

    return;
}



VOS_VOID  TAF_MMA_LogDrvApiPwrCtrlSleepVoteUnlock(
    VOS_UINT32                          ulRslt,
    PWC_CLIENT_ID_E                     enClientId
)
{
    TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU        *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                              sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LogDrvApiPwrCtrlSleepVoteUnlock:ERROR:Alloc Mem Fail.");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_UNLOCK;

    pstMsg->ulRslt                      = ulRslt;
    pstMsg->enClientId                  = enClientId;

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

    return;
}




VOS_VOID  TAF_MMA_LogDrvApiPwrCtrlSleepVoteLock(
    VOS_UINT32                          ulRslt,
    PWC_CLIENT_ID_E                     enClientId
)
{
    TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU        *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMA,
                              sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LogDrvApiPwrCtrlSleepVoteLock:ERROR:Alloc Mem Fail.");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength        = sizeof(TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName       = TAF_MMA_LOG_DRV_API_PWRCTRL_SLEEPVOTE_LOCK;

    pstMsg->ulRslt                      = ulRslt;
    pstMsg->enClientId                  = enClientId;

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_MMA, pstMsg);

    return;
}


VOS_VOID TAF_MMA_ShowPhoneModeCtrlInfo(VOS_VOID)
{
    vos_printf("\n");
    vos_printf("*****************************\n");
    vos_printf("*Auto Init Ps Flag:      %d\n", TAF_MMA_GetAutoInitPsFlg());
    vos_printf("*Auto Switch On Flag:    %d\n", TAF_MMA_GetAutoSwitchOnFlg());
    vos_printf("*Cur  Phone Mode:        %d\n", TAF_SDC_GetCurPhoneMode());
    vos_printf("*Cur  Fsm State:         %d\n", g_StatusContext.ulFsmState);
    vos_printf("*Phone Mode Error Code:  %d\n", TAF_MMA_GetCurPhoneErrorCode_PhoneMode());
    vos_printf("*****************************\n");
}


VOS_VOID TAF_MMA_ShowCLConfigInfo(VOS_VOID)
{
    vos_printf("\n");
    vos_printf("*****************************\n");
    vos_printf("*CL CONFIG Enable Flag: %d\n",          TAF_SDC_GetLCEnableFlg());
    vos_printf("*CL CONFIG RatCombined Flag: %d\n",     TAF_SDC_GetLCRatCombined());
    vos_printf("*CL CONFIG LCWORK Flag: %d\n",          TAF_SDC_GetLCWorkCfg());
    vos_printf("*****************************\n");
}






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


