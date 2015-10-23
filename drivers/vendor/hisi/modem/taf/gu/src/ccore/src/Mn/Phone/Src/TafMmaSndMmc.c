

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "PsLib.h"
#include "pslog.h"
#include "PsCommonDef.h"
#include "TafSdcCtx.h"
#include "TafMmaSndMmc.h"
#include "MmaMmcInterface.h"
#include "MmaAppLocal.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define THIS_FILE_ID  PS_FILE_ID_TAF_MMA_SND_MMC_C

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
  6 函数定义
*****************************************************************************/


VOS_UINT32 TAF_MMA_SndMmcStartReq(
    MMA_MMC_CARD_STATUS_ENUM_UINT8      enCardStatus,
    MMA_MMC_PLMN_RAT_PRIO_STRU         *pstPlmnRatPrio
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_START_REQ_STRU             *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_START_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcStartReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_START_REQ;
    pstMsg->enRegDomain                = TAF_MMA_GetRegDomain();
    pstMsg->enCardStatus               = enCardStatus;
    pstMsg->stPlmnRatPrio              = *pstPlmnRatPrio;


    /* 开机请求适配新的接口，支持CDMA技术接口 */
    if (VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())/* C+L模式 */
    {
        pstMsg->enRegCtrl                   = MMA_MMC_REG_CONTROL_BY_3GPP2_CBP;
        pstMsg->st3Gpp2Rat.ucRatNum         = 1;
        pstMsg->st3Gpp2Rat.auc3GPP2Rat[0]   = MMA_MMC_3GPP2_RAT_TYPE_HRPD;
    }
    else
    {
        pstMsg->enRegCtrl                   = MMA_MMC_REG_CONTROL_BY_3GPP_MMC;
        pstMsg->st3Gpp2Rat.ucRatNum         = 0;
    }

    if (MMA_MMC_CARD_STATUS_ABSENT != enCardStatus)
    {
        PS_MEM_CPY(pstMsg->aucImsi, TAF_SDC_GetSimImsi(), NAS_MAX_IMSI_LENGTH);
    }

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcStartReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    NAS_TRACE_HIGH("Send Msg!");

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcSignalReportReq(
    VOS_UINT8                           ucActionType,
    VOS_UINT8                           ucRrcMsgType,
    VOS_UINT8                           ucSignThreshold,
    VOS_UINT8                           ucMinRptTimerInterval
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_SIGN_REPORT_REQ_STRU       *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_SIGN_REPORT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_SIGN_REPORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcSignalReportReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_SIGN_REPORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_SIGN_REPORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_SIGN_REPORT_REQ;
    pstMsg->ucActionType               = ucActionType;
    pstMsg->ucRrcMsgType               = ucRrcMsgType;
    pstMsg->ucSignThreshold            = ucSignThreshold;
    pstMsg->ucMinRptTimerInterval      = ucMinRptTimerInterval;


    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcSignalReportReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcModeChangeReq(
    MMA_MMC_MS_MODE_ENUM_UINT32         enMsMode
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_MODE_CHANGE_REQ_STRU       *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_MODE_CHANGE_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_MODE_CHANGE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcModeChangeReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_MODE_CHANGE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_MODE_CHANGE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_MODE_CHANGE_REQ;
    pstMsg->enMsMode                   = enMsMode;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcModeChangeReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcAttachReq(
    VOS_UINT32                                              ulOpID,
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32                         enAttachType,
    TAF_MMA_EPS_ATTACH_REASON_ENUM_UINT8                    enAttachReason
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_ATTACH_REQ_STRU            *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_ATTACH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcAttachReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_ATTACH_REQ;
    pstMsg->ulOpID                     = ulOpID;
    pstMsg->enAttachType               = enAttachType;
    pstMsg->enEpsAttachReason          = enAttachReason;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcAttachReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcDetachReq(
    VOS_UINT32                                 ulOpID,
    MMA_MMC_DETACH_TYPE_ENUM_UINT32            enDetachType,
    TAF_MMA_DETACH_CAUSE_ENUM_UINT8            enDetachCause
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_DETACH_REQ_STRU            *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_DETACH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcDetachReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_DETACH_REQ;
    pstMsg->ulOpID                     = ulOpID;
    pstMsg->enDetachType               = enDetachType;

    pstMsg->enDetachReason             = enDetachCause;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcDetachReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 TAF_MMA_SndMmcPlmnListReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PLMN_LIST_REQ_STRU         *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_PLMN_LIST_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PLMN_LIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnListReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PLMN_LIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_PLMN_LIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_PLMN_LIST_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnListReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcPlmnListAbortReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PLMN_LIST_ABORT_REQ_STRU   *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_PLMN_LIST_ABORT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PLMN_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnListAbortReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PLMN_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_PLMN_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_PLMN_LIST_ABORT_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnListAbortReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcPlmnUserReselReq(MMA_MMC_PLMN_SEL_MODE_ENUM_UINT32 enPlmnSelMode)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PLMN_USER_RESEL_REQ_STRU   *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_PLMN_USER_RESEL_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PLMN_USER_RESEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnUserReselReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PLMN_USER_RESEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_PLMN_USER_RESEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_PLMN_USER_RESEL_REQ;
    pstMsg->enPlmnSelMode              = enPlmnSelMode;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnUserReselReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcPlmnSpecialReq(
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8     enAccessMode
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PLMN_SPECIAL_REQ_STRU      *pstMsg = VOS_NULL_PTR;

    pstMsg = (MMA_MMC_PLMN_SPECIAL_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PLMN_SPECIAL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnSpecialReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PLMN_SPECIAL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_PLMN_SPECIAL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_PLMN_SPECIAL_REQ;
    pstMsg->stPlmnId                   = *pstPlmnId;
    pstMsg->enAccessMode               = enAccessMode;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnSpecialReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_SndMmcPowerOffReq(
    MMA_MMC_POWER_OFF_CAUSE_ENUM_UINT32 enCause
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_POWER_OFF_REQ_STRU         *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_POWER_OFF_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPowerOffReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_POWER_OFF_REQ;
    pstMsg->enCause                    = enCause;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPowerOffReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    NAS_TRACE_HIGH("Send Msg!");

    return VOS_OK;
}



VOS_UINT32  TAF_MMA_SndMmcSysCfgReq(
    TAF_MMA_SYS_CFG_PARA_STRU          *pSysCfgReq,
    MMA_SYS_CFG_SET_FLG                 usSetFlg
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_SYS_CFG_SET_REQ_STRU       *pMsg = VOS_NULL_PTR;
    MMA_USER_BAND_SET_UN                uUserSetBand;
    MMA_UE_SUPPORT_FREQ_BAND_STRU       stUeBand;   /* UE支持的用户设置的频段 */
    VOS_UINT32                          ulBand;
    VOS_UINT32                          i;


    /* 申请消息内存 */
    pMsg = (MMA_MMC_SYS_CFG_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(MMA_MMC_SYS_CFG_SET_REQ_STRU));
    if (VOS_NULL_PTR == pMsg)
    {
        /* 内存申请失败                             */
        MN_ERR_LOG("TAF_MMA_SndMmcSysCfgReq():ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    pMsg->MsgHeader.ulSenderCpuId       = VOS_LOCAL_CPUID;
    pMsg->MsgHeader.ulSenderPid         = WUEPS_PID_MMA;
    pMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pMsg->MsgHeader.ulReceiverPid       = WUEPS_PID_MMC;
    pMsg->MsgHeader.ulMsgName           = ID_MMA_MMC_SYS_CFG_SET_REQ;

    pMsg->ucOnlyRoamParaChanged         = VOS_FALSE;

    /*漫游参数填写*/
    pMsg->enRoamCapability = pSysCfgReq->ucRoam;

    /*接入模式pMsg->stRatPrioList参数填写*/


#if (FEATURE_ON == FEATURE_LTE)
    if (pSysCfgReq->stRatOrder.ucRatOrderNum > TAF_PH_MAX_GUL_RAT_NUM)
    {
        pSysCfgReq->stRatOrder.ucRatOrderNum = TAF_PH_MAX_GUL_RAT_NUM;
    }
#else
    if (pSysCfgReq->stRatOrder.ucRatOrderNum > TAF_PH_MAX_GU_RAT_NUM)
    {
        pSysCfgReq->stRatOrder.ucRatOrderNum = TAF_PH_MAX_GU_RAT_NUM;
    }
#endif

    /* 如果UE不支持GSM，当设置包含G模去掉G模 */
    if (MMA_TRUE == MMA_IsGsmForbidden())
    {
        pMsg->stRatPrioList.ucRatNum = 0;
        for ( i = 0 ; i < pSysCfgReq->stRatOrder.ucRatOrderNum; i++ )
        {
            if ( TAF_PH_RAT_GSM == pSysCfgReq->stRatOrder.aenRatOrder[i])
            {
                continue;
            }
            pMsg->stRatPrioList.aucRatPrio[pMsg->stRatPrioList.ucRatNum] = pSysCfgReq->stRatOrder.aenRatOrder[i];
            pMsg->stRatPrioList.ucRatNum++;
        }
    }
    else
    {
        PS_MEM_CPY(&(pMsg->stRatPrioList), &(pSysCfgReq->stRatOrder), sizeof(MMA_MMC_PLMN_RAT_PRIO_STRU));
    }

    /* GU频带设置参数填写*/
    /*将pSysCfgReq->stGuBand 从64位转换为32位*/
    MN_MMA_Convert64BitBandTo32Bit(&pSysCfgReq->stGuBand, &ulBand);
    MN_MMA_ConvertGUFrequencyBand(&ulBand);
    uUserSetBand.ulPrefBand         = ulBand;

    MMA_SwitchUserSetBand2UESupportBand(uUserSetBand, &stUeBand);
    pMsg->stBand.unGsmBand.ulBand   = stUeBand.unGsmBand.ulBand;
    pMsg->stBand.unWcdmaBand.ulBand = stUeBand.unWcdmaBand.ulBand;

    /* L频段设置参数填写 */
    pMsg->stBand.stLteBand.aulLteBand[0] = pSysCfgReq->stLBand.ulBandLow;
    pMsg->stBand.stLteBand.aulLteBand[1] = pSysCfgReq->stLBand.ulBandHigh;

    if ((MMA_SYS_CFG_MODE_SET != (usSetFlg & MMA_SYS_CFG_MODE_SET))
     && (MMA_SYS_CFG_BAND_SET != (usSetFlg & MMA_SYS_CFG_BAND_SET))
     && (MMA_SYS_CFG_ROAM_SET == (usSetFlg & MMA_SYS_CFG_ROAM_SET)))
    {
        pMsg->ucOnlyRoamParaChanged = VOS_TRUE;
    }

    /*消息发送*/
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pMsg);
    if (VOS_OK != ulRet)
    {
        MN_ERR_LOG("TAF_MMA_SndMmcSysCfgReq():ERROR: Send Msg Fail.");
        return VOS_ERR;
    }


    return VOS_OK;
}




VOS_UINT32 TAF_MMA_SndMmcNetScanMsgReq(
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanReq
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_NET_SCAN_REQ_STRU          *pstMsg              = VOS_NULL_PTR;
    MMA_UE_SUPPORT_FREQ_BAND_STRU       stUeBand;                               /* UE支持的用户设置的频段 */
    VOS_UINT32                          ulBand;
    MMA_USER_BAND_SET_UN                uUserSetBand;

    /* 申请消息内存 */
    pstMsg = (MMA_MMC_NET_SCAN_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(MMA_MMC_NET_SCAN_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        /* 内存申请失败 */
        MN_ERR_LOG("TAF_MMA_SndNetScanMsgReq():ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET(pstMsg, 0x00, sizeof(MMA_MMC_NET_SCAN_REQ_STRU));

    pstMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulLength          = sizeof(MMA_MMC_NET_SCAN_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = ID_MMA_MMC_NET_SCAN_REQ;

    /* GU频带设置参数填写 */
    /* 将pstNetScanReq->stGuBand 从64位转换为32位 */
    MN_MMA_Convert64BitBandTo32Bit(&pstNetScanReq->stBand, &ulBand);
    MN_MMA_ConvertGUFrequencyBand(&ulBand);
    uUserSetBand.ulPrefBand             = ulBand;

    MMA_SwitchUserSetBand2UESupportBand(uUserSetBand, &stUeBand);

    pstMsg->stBand.unGsmBand.ulBand     = stUeBand.unGsmBand.ulBand;
    pstMsg->stBand.unWcdmaBand.ulBand   = stUeBand.unWcdmaBand.ulBand;
    pstMsg->sCellPow                    = pstNetScanReq->sCellPow;
    pstMsg->usCellNum                   = pstNetScanReq->usCellNum;
    pstMsg->ucRat                       = pstNetScanReq->ucRat;

    /*消息发送*/
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        MN_ERR_LOG("TAF_MMA_SndMmcNetScanMsgReq():ERROR: Send Msg Fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcAbortNetScanMsgReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_ABORT_NET_SCAN_REQ_STRU    *pstMsg              = VOS_NULL_PTR;

    /* 申请消息内存 */
    pstMsg = (MMA_MMC_ABORT_NET_SCAN_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(MMA_MMC_ABORT_NET_SCAN_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        /* 内存申请失败 */
        MN_ERR_LOG("TAF_MMA_SndMmcAbortNetScanMsgReq():ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET(pstMsg, 0x00, sizeof(MMA_MMC_ABORT_NET_SCAN_REQ_STRU));

    pstMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulLength          = sizeof(MMA_MMC_ABORT_NET_SCAN_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = ID_MMA_MMC_ABORT_NET_SCAN_REQ;

    /*消息发送*/
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        MN_ERR_LOG("TAF_MMA_SndMmcAbortNetScanMsgReq():ERROR: Send Msg Fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_SndMmcPlmnSearchReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PLMN_SEARCH_REQ_STRU       *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnSearchReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_PLMN_SEARCH_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPlmnSearchReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcSpecPlmnSearchAbortReq(VOS_VOID)
{
    VOS_UINT32                                               ulRet;
    MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU                 *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcSpecPlmnSearchAbortReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_SPEC_PLMN_SEARCH_ABORT_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcSpecPlmnSearchAbortReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcOmMaintainInfoInd(
    VOS_UINT8                           ucOmConnectFlg,
    VOS_UINT8                           ucOmPcRecurEnableFlg
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_OM_MAINTAIN_INFO_IND_STRU  *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_OM_MAINTAIN_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_OM_MAINTAIN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcOmMaintainInfoInd:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_OM_MAINTAIN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_OM_MAINTAIN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_OM_MAINTAIN_INFO_IND;
    pstMsg->ucOmConnectFlg             = ucOmConnectFlg;
    pstMsg->ucOmPcRecurEnableFlg       = ucOmPcRecurEnableFlg;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcOmMaintainInfoInd:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 TAF_MMA_SndMmcUpdateUplmnNtf( VOS_VOID )
{
    VOS_UINT32                          ulRet;
    MMA_MMC_UPDATE_UPLMN_NTF_STRU      *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_UPDATE_UPLMN_NTF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_UPDATE_UPLMN_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcUpdateUplmnNtf:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_UPDATE_UPLMN_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_UPDATE_UPLMN_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_UPDATE_UPLMN_NTF;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcUpdateUplmnNtf:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


#if (FEATURE_MULTI_MODEM == FEATURE_ON)

VOS_UINT32 TAF_MMA_SndMmcOtherModemInfoNotify(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU                   *pstMmaSndMmcMsg = VOS_NULL_PTR;
    MTC_MMA_OTHER_MODEM_INFO_NOTIFY_STRU                   *pstMtcMmaOtherModemInfo = VOS_NULL_PTR;

    pstMtcMmaOtherModemInfo = (MTC_MMA_OTHER_MODEM_INFO_NOTIFY_STRU *)pstMsg;

    /* 申请内存  */
    pstMmaSndMmcMsg = (MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMmaSndMmcMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcOtherModemInfoNotify:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMmaSndMmcMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMmaSndMmcMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMmaSndMmcMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmaSndMmcMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_OTHER_MODEM_INFO_NOTIFY;
    pstMmaSndMmcMsg->bitOpCurrCampPlmnId        = pstMtcMmaOtherModemInfo->bitOpCurrCampPlmnId;
    pstMmaSndMmcMsg->stCurrCampPlmnId.ulMcc     = pstMtcMmaOtherModemInfo->stCurrCampPlmnId.ulMcc;
    pstMmaSndMmcMsg->stCurrCampPlmnId.ulMnc     = pstMtcMmaOtherModemInfo->stCurrCampPlmnId.ulMnc;

    pstMmaSndMmcMsg->bitOpEplmnInfo             = pstMtcMmaOtherModemInfo->bitOpEplmnInfo;
    pstMmaSndMmcMsg->stEplmnInfo.ucEquPlmnNum   = pstMtcMmaOtherModemInfo->stEplmnInfo.ucEquPlmnNum;
    PS_MEM_CPY(pstMmaSndMmcMsg->stEplmnInfo.astEquPlmnAddr, pstMtcMmaOtherModemInfo->stEplmnInfo.astEquPlmnAddr,
               sizeof(pstMmaSndMmcMsg->stEplmnInfo.astEquPlmnAddr));


    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMmaSndMmcMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcOtherModemInfoNotify:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcNcellInfoInd(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulRet;
    MMA_MMC_NCELL_INFO_NOTIFY_STRU                *pstMmaSndMmcMsg = VOS_NULL_PTR;
    MTC_MMA_NCELL_INFO_IND_STRU                   *pstMtcMmaNcellInfo = VOS_NULL_PTR;

    pstMtcMmaNcellInfo = (MTC_MMA_NCELL_INFO_IND_STRU *)pstMsg;

    /* 申请内存  */
    pstMmaSndMmcMsg = (MMA_MMC_NCELL_INFO_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_NCELL_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMmaSndMmcMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcNcellInfoInd:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMmaSndMmcMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_NCELL_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMmaSndMmcMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMmaSndMmcMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_NCELL_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmaSndMmcMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_NCELL_INFO_NOTIFY;

    pstMmaSndMmcMsg->stTdsNcellInfo.ucTdsArfcnNum = pstMtcMmaNcellInfo->stTdsNcellInfo.ucTdsArfcnNum;
    PS_MEM_CPY(pstMmaSndMmcMsg->stTdsNcellInfo.ausTdsArfcnList, pstMtcMmaNcellInfo->stTdsNcellInfo.ausTdsArfcnList,
               sizeof(pstMmaSndMmcMsg->stTdsNcellInfo.ausTdsArfcnList));

    pstMmaSndMmcMsg->stLteNcellInfo.ucLteArfcnNum = pstMtcMmaNcellInfo->stLteNcellInfo.ucLteArfcnNum;
    PS_MEM_CPY(pstMmaSndMmcMsg->stLteNcellInfo.ausLteArfcnList, pstMtcMmaNcellInfo->stLteNcellInfo.ausLteArfcnList,
               sizeof(pstMmaSndMmcMsg->stLteNcellInfo.ausLteArfcnList));

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMmaSndMmcMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcNcellInfoInd:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 TAF_MMA_SndMmcPsTransferInd(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_PS_TRANSFER_IND_STRU       *pstMmaSndMmcMsg = VOS_NULL_PTR;
    MTC_MMA_PS_TRANSFER_IND_STRU       *pstMtcMmaPsTransferInd = VOS_NULL_PTR;

    pstMtcMmaPsTransferInd = (MTC_MMA_PS_TRANSFER_IND_STRU *)pstMsg;

    /* 申请内存  */
    pstMmaSndMmcMsg = (MMA_MMC_PS_TRANSFER_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_PS_TRANSFER_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMmaSndMmcMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPsTransferInd:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMmaSndMmcMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_PS_TRANSFER_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMmaSndMmcMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMmaSndMmcMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMmaSndMmcMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_PS_TRANSFER_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmaSndMmcMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_PS_TRANSFER_IND;
    pstMmaSndMmcMsg->enSolutionCfg                = pstMtcMmaPsTransferInd->enSolutionCfg;
    pstMmaSndMmcMsg->ucCause                      = pstMtcMmaPsTransferInd->ucCause;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMmaSndMmcMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPsTransferInd:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif


VOS_VOID TAF_MMA_SndMmcEOPlmnSetReq(
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnSetPara
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_EOPLMN_SET_REQ_STRU        *pstMsg = VOS_NULL_PTR;

    /* 申请消息内存 */
    pstMsg = (MMA_MMC_EOPLMN_SET_REQ_STRU*)PS_ALLOC_MSG(
                                         WUEPS_PID_MMA,
                                         sizeof(MMA_MMC_EOPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        /* 内存申请失败 */
        MN_ERR_LOG("TAF_MMA_SndMmcEOPlmnSetReq():ERROR: Memory Alloc Error for pMsg");
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_EOPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulMsgName         = ID_MMA_MMC_EOPLMN_SET_REQ;
    pstMsg->MsgHeader.ulLength          = sizeof(MMA_MMC_EOPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填充消息内容 */
    pstMsg->ucIndex      = pstEOPlmnSetPara->ucIndex;
    pstMsg->ucOPlmnCount = pstEOPlmnSetPara->ucOPlmnCount;

    PS_MEM_CPY((VOS_INT8*)pstMsg->aucVersion,
                    pstEOPlmnSetPara->aucVersion,
                    TAF_MAX_USER_CFG_OPLMN_VERSION_LEN * sizeof(TAF_UINT8));

    PS_MEM_CPY((VOS_INT8*)pstMsg->aucOPlmnWithRat,
                    pstEOPlmnSetPara->aucOPlmnWithRat,
                    TAF_MAX_GROUP_CFG_OPLMN_DATA_LEN * sizeof(TAF_UINT8));

    /*消息发送*/
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        MN_ERR_LOG("TAF_MMA_SndMmcEOPlmnSetReq():ERROR: Send Msg Fail.");
        return;
    }

    return;
}


VOS_VOID TAF_MMA_SndMmcImsVoiceCapInd(
    VOS_UINT8                           ucImsVoiceAvail
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_IMS_VOICE_CAP_IND_STRU     *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_IMS_VOICE_CAP_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstMsg )
    {
        MN_ERR_LOG("TAF_MMA_SndMmcImsVoiceCapInd:ERROR: Memory Alloc Error for pMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->MsgHeader.ulLength         = sizeof(MMA_MMC_IMS_VOICE_CAP_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName        = ID_MMA_MMC_IMS_VOICE_CAP_IND;
    pstMsg->ucAvail                    = ucImsVoiceAvail;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMA, pstMsg );
    if ( VOS_OK != ulRet )
    {
        MN_ERR_LOG("TAF_MMA_SndMmcAttachReq:ERROR:PS_SEND_MSG FAILURE");
    }

    return;
}


VOS_UINT32 TAF_MMA_SndMmcAcqReq(
    TAF_MMA_ACQ_PARA_STRU              *pstMmaAcqPara
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_ACQ_REQ_STRU               *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_ACQ_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcAcqReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_ACQ_REQ;
    pstMsg->enAcqReason                  = pstMmaAcqPara->enAcqReason;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcAcqReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MMA_SndMmcRegReq(
    TAF_MMA_REG_PARA_STRU              *pstMmaRegPara
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          i;
    MMA_MMC_REG_REQ_STRU               *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_REG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcRegReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_REG_REQ;
    pstMsg->bitOpPrioClass               = pstMmaRegPara->bitOpPrioClass;
    pstMsg->bitOpCellInfo                = pstMmaRegPara->bitOpCellInfo;
    pstMsg->bitSpare                     = pstMmaRegPara->bitSpare;
    pstMsg->enPrioClass                  = pstMmaRegPara->enPrioClass;

    if (VOS_TRUE == pstMmaRegPara->bitOpCellInfo)
    {
        /* 目前只支持到CDMA到LTE的重选，接入技术固定填写为LTE */
        pstMsg->stCellInfo.ucRat   = NAS_MML_NET_RAT_TYPE_LTE;
        pstMsg->stCellInfo.usArfcn = pstMmaRegPara->stCellInfo.usArfcn;

        pstMsg->stCellInfo.ucCellNum = TAF_SDC_MIN(pstMmaRegPara->stCellInfo.ucCellNum, MMA_MMC_REG_MAX_CELL_NUM);
        PS_MEM_CPY(pstMsg->stCellInfo.ausCellId, pstMmaRegPara->stCellInfo.ausCellId,
                   pstMsg->stCellInfo.ucCellNum*sizeof(VOS_UINT16));

        pstMsg->stCellInfo.ucPlmnNum = TAF_SDC_MIN(pstMmaRegPara->stCellInfo.ucPlmnNum, MMA_MMC_REG_MAX_PLMN_NUM);
        for (i = 0; i < pstMsg->stCellInfo.ucPlmnNum; i++)
        {
            pstMsg->stCellInfo.astPlmnId[i].ulMcc = pstMmaRegPara->stCellInfo.astPlmnId[i].Mcc;
            pstMsg->stCellInfo.astPlmnId[i].ulMnc = pstMmaRegPara->stCellInfo.astPlmnId[i].Mnc;
        }
    }

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcRegReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MMA_SndMmcPowerSaveReq(
    TAF_MMA_POWER_SAVE_PARA_STRU       *pstMmaPowerSavePara
)
{
    VOS_UINT32                          ulRet;
    MMA_MMC_POWER_SAVE_REQ_STRU        *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_MMC_POWER_SAVE_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_MMC_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPowerSaveReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_MMC_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid    = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->stMsgHeader.ulLength         = sizeof(MMA_MMC_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName        = ID_MMA_MMC_POWER_SAVE_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstMsg);

    if (VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndMmcPowerSaveReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }

    return VOS_OK;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

