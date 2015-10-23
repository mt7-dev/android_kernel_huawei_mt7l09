


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "MnComm.h"
#include "TafSdcCtx.h"
#include "TafLog.h"
#include "TafMmaSndApp.h"
#include "MmaMmcInterface.h"
#include "MmaAppLocal.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_SND_APP_C


/*****************************************************************************
   2 全局变量定义和宏
****************************************************************************/


/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID TAF_MMA_SndSysCfgSetCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_SYS_CFG_CNF_STRU           *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SYS_CFG_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SYS_CFG_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysCfgSetCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SYS_CFG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SYS_CFG_SET_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->usErrorCause             = usErrorCause;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysCfgSetCnf:Send Msg Failed");
        return;
    }

    return;

}



VOS_VOID TAF_MMA_SndPhoneModeSetCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_PH_MODE                         ucPhMode,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_PHONE_MODE_SET_CNF_STRU    *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_PHONE_MODE_SET_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_PHONE_MODE_SET_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPhoneModeSetCnf:Alloc Msg Failed");

        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_PHONE_MODE_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_PHONE_MODE_SET_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->ucPhMode                 = ucPhMode;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->usErrorCause             = usErrorCause;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPhoneModeSetCnf:Send Msg Failed");

        return;
    }

    return;

}



VOS_VOID TAF_MMA_SndAcqBestNetworkCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_ACQ_CNF_STRU               *pstAcqCnfMsg
)
{
    TAF_MMA_ACQ_CNF_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_ACQ_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_ACQ_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqBestNetworkCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_ACQ_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_ACQ_BEST_NETWORK_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstAcqCnfMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstAcqCnfMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstAcqCnfMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstAcqCnfMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqBestNetworkCnf:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndRegCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_REG_CNF_STRU               *pstRegCnfMsg
)
{
    TAF_MMA_REG_CNF_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_REG_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_REG_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndRegCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_REG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_REG_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstRegCnfMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstRegCnfMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstRegCnfMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstRegCnfMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndRegCnf:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndPowerSaveCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt
)
{
    TAF_MMA_POWER_SAVE_CNF_STRU        *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_POWER_SAVE_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                 WUEPS_PID_MMA,
                                                 sizeof(TAF_MMA_POWER_SAVE_CNF_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerSaveCnf:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_POWER_SAVE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = pstCtrl->ulModuleId;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_POWER_SAVE_CNF;
    pstSndMsg->usClientId               = pstCtrl->usClientId;
    pstSndMsg->ucOpid                   = pstCtrl->ucOpId;
    pstSndMsg->enRslt                   = enRslt;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndPowerSaveCnf:Send Msg Failed");
        return;
    }

    return;

}


VOS_VOID TAF_MMA_SndDetachCnf(
    TAF_MMA_CTRL_STRU                  *pstCtrl,
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enDetachRslt,
    TAF_PHONE_ERROR                     usErrorCause
)
{
    TAF_MMA_DETACH_CNF_STRU            *pstDetachCnf = VOS_NULL_PTR;

    /* 申请消息包TAF_MMA_DETACH_CNF_STRU */
    pstDetachCnf = (TAF_MMA_DETACH_CNF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           WUEPS_PID_MMA,
                                           sizeof(TAF_MMA_DETACH_CNF_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstDetachCnf)
    {
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstDetachCnf + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_DETACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 根据输入参数填充TAF_MMA_DETACH_CNF_STRU */
    pstDetachCnf->ulSenderPid       = WUEPS_PID_MMA;
    pstDetachCnf->ulReceiverPid     = pstCtrl->ulModuleId;
    pstDetachCnf->ulMsgName         = ID_TAF_MMA_DETACH_CNF;

    pstDetachCnf->usClientId        = pstCtrl->usClientId;
    pstDetachCnf->ucOpid            = pstCtrl->ucOpId;
    pstDetachCnf->enRslt            = enDetachRslt;
    pstDetachCnf->usErrorCause      = usErrorCause;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMA, pstDetachCnf))
    {
        return;
    }

    return;

}


#if (FEATURE_ON == FEATURE_CL_INTERWORK)


VOS_VOID TAF_MMA_SndCmmcaServiceInd(VOS_VOID)
{
    TAF_MMA_SERVICE_STATUS_IND_STRU    *pstSndMsg    = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    /* 申请内存  */
    pstSndMsg = (TAF_MMA_SERVICE_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndCmmcaServiceInd: Memory allocation is failure");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSndMsg) + VOS_MSG_HEAD_LENGTH, 0, sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* fill in header information */
    pstSndMsg->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid        = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid      = WUEPS_PID_CMMCA;
    pstSndMsg->ulLength           = sizeof(TAF_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* fill in message name */
    pstSndMsg->ulMsgName          = ID_TAF_MMA_SERVICE_STATUS_IND;

    /* fill in clientid and opid */
    pstSndMsg->usClientId         = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid             = 0x0;

    /* fill in message content */
    TAF_MMA_ConvertServiceStatus(TAF_SDC_GetCsServiceStatus(), &(pstSndMsg->CsSrvSta));
    TAF_MMA_ConvertServiceStatus(TAF_SDC_GetPsServiceStatus(), &(pstSndMsg->PsSrvSta));

    pstSndMsg->ucCsSimValid       = TAF_SDC_GetSimCsRegStatus();
    pstSndMsg->ucPsSimValid       = TAF_SDC_GetSimPsRegStatus();

    /* send message */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if (VOS_OK != ulRet)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndCmmcaServiceInd: Send message is failure");
    }

    return;
}



VOS_VOID TAF_MMA_SndAcqInd(
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt,
    MMC_MMA_ACQ_IND_STRU               *pstAcqIndMsg
)
{
    TAF_MMA_ACQ_IND_STRU               *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_ACQ_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMA,
                                         sizeof(TAF_MMA_ACQ_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_ACQ_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_ACQ_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->enRslt                   = enRslt;
    pstSndMsg->stPlmnId.Mcc             = pstAcqIndMsg->stPlmnId.ulMcc;
    pstSndMsg->stPlmnId.Mnc             = pstAcqIndMsg->stPlmnId.ulMnc;
    pstSndMsg->usArfcn                  = pstAcqIndMsg->usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(pstAcqIndMsg->enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndAcqInd:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndCmmcaSysInfoInd(
    VOS_UINT32                                              ulMcc,
    VOS_UINT32                                              ulMnc,
    VOS_UINT16                                              usArfcn,
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                  enPrioClass
)
{
    TAF_MMA_SYS_INFO_IND_STRU          *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SYS_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SYS_INFO_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysInfoInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SYS_INFO_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->stPlmnId.Mcc             = ulMcc;
    pstSndMsg->stPlmnId.Mnc             = ulMnc;
    pstSndMsg->usArfcn                  = usArfcn;

    /* 转换并设置当前的Plmn Priority Class */
    TAF_MMA_ConvertPlmnPriorityClass(enPrioClass, &(pstSndMsg->enPrioClass));

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSysInfoInd:Send Msg Failed");
        return;
    }

    return;

}
VOS_VOID TAF_MMA_SndSimStatusInd(
    TAF_MMA_USIMM_CARD_TYPE_ENUM_UINT32           enCardType,                    /* 卡类型:SIM、USIM、ROM-SIM  */
    TAF_MMA_CARD_STATUS_ENUM_UINT8                enCardStatus
)
{
    TAF_MMA_SIM_STATUS_IND_STRU        *pstSndMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulRst;

    pstSndMsg = (TAF_MMA_SIM_STATUS_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMA,
                                             sizeof(TAF_MMA_SIM_STATUS_IND_STRU));
    if (VOS_NULL_PTR == pstSndMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSimStatusInd:Alloc Msg Failed");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SIM_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_CMMCA;
    pstSndMsg->ulMsgName                = ID_TAF_MMA_SIM_STATUS_IND;
    pstSndMsg->usClientId               = CMMCA_CLIENT_ID;
    pstSndMsg->ucOpid                   = 0;
    pstSndMsg->enCardType               = enCardType;
    pstSndMsg->enCardStatus             = enCardStatus;

    ulRst = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if (VOS_OK != ulRst)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,"TAF_MMA_SndSimStatusInd:Send Msg Failed");
        return;
    }

    return;

}

#endif




#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



