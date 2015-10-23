




/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "TafSdcCtx.h"
#if (FEATURE_IMS == FEATURE_ON)
#include "ImsaMmaInterface.h"
#endif
#include "PsCommonDef.h"
#include "NasComm.h"
#include "MmaAppLocal.h"
#include "TafLog.h"
#include "NasUtranCtrlInterface.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_SND_IMSA_C


/*****************************************************************************
   2 全局变量定义和宏
****************************************************************************/


/*****************************************************************************
   3 函数实现
*****************************************************************************/
#if (FEATURE_IMS == FEATURE_ON)

MMA_IMSA_SERVICE_STATUS_ENUM_UINT8 TAF_MMA_ConvertMmaPsServiceStatusToImsaFormat(
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32 enMmcPsServiceStatus
)
{
    MMA_IMSA_SERVICE_STATUS_ENUM_UINT8  ucPsServiceStatus;

    switch ((VOS_UINT8)enMmcPsServiceStatus)
    {
        case MMA_MMC_SERVICE_STATUS_NORMAL_SERVICE:

            ucPsServiceStatus =  MMA_IMSA_NORMAL_SERVICE;
            break;

        case MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE:
        case MMA_MMC_SERVICE_STATUS_LIMITED_SERVICE_REGION:
        case MMA_MMC_SERVICE_STATUS_NO_IMSI:

            ucPsServiceStatus =  MMA_IMSA_LIMITED_SERVICE;
            break;

        case MMA_MMC_SERVICE_STATUS_NO_CHANGE:
        case MMA_MMC_SERVICE_STATUS_DEEP_SLEEP:

            TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_ConvertMmaPsServiceStatusToImsaFormat: service status is abnormal!");
            ucPsServiceStatus =  MMA_IMSA_NO_SERVICE;
            break;

        case MMA_MMC_SERVICE_STATUS_NO_SERVICE:

            ucPsServiceStatus =  MMA_IMSA_NO_SERVICE;
            break;

        default:

            ucPsServiceStatus =  MMA_IMSA_SERVICE_STATUS_BUTT;
            break;
    }

    return ucPsServiceStatus;
}



VOS_VOID TAF_MMA_SndImsaSrvInfoNotify(
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32  enPsServiceStatus
)
{
    MMA_IMSA_SERVICE_CHANGE_IND_STRU   *pstSndMsg    = VOS_NULL_PTR;
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstNwCapInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    /* 申请内存  */
    pstSndMsg = (MMA_IMSA_SERVICE_CHANGE_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_IMSA_SERVICE_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstSndMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaSrvInfoNotify: Memory allocation is failure");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSndMsg) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMA_IMSA_SERVICE_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* fill in header information */
    pstSndMsg->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid        = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid      = PS_PID_IMSA;
    pstSndMsg->ulLength           = sizeof(MMA_IMSA_SERVICE_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* fill in message name */
    pstSndMsg->ulMsgId            = ID_MMA_IMSA_SERVICE_CHANGE_IND;

    /* set PS service status */
    pstSndMsg->enPsServiceStatus  = TAF_MMA_ConvertMmaPsServiceStatusToImsaFormat(enPsServiceStatus);

    /* set PS sim valid */
    pstSndMsg->ucPsSimValid       = VOS_TRUE;
    
    if (MMA_MMC_SERVICE_STATUS_NO_IMSI == enPsServiceStatus)
    {
        pstSndMsg->ucPsSimValid       = VOS_FALSE;
    }
    
    if (TAF_SDC_SYS_MODE_LTE == TAF_SDC_GetSysMode())
    {
        /* by the way, here get lte network capability information */
        pstNwCapInfo              = TAF_SDC_GetLteNwCapInfo();
    }
    else
    {
        /* by the way, here get GU network capability information */
        pstNwCapInfo              = TAF_SDC_GetGuNwCapInfo();
    }

    /* fill RAT */
    pstSndMsg->enRat              = (MMA_IMSA_RAT_TYPE_ENUM_UINT8)TAF_SDC_GetSysMode();

    /* fill in network capability infomation */
    pstSndMsg->enEmsInd           = (MMA_IMSA_EMS_INDICATOR_ENUM_UINT8)pstNwCapInfo->enNwEmcBsCap;
    pstSndMsg->enImsVoPsInd       = (MMA_IMSA_IMS_VOPS_INDICATOR_ENUM_UINT8)pstNwCapInfo->enNwImsVoCap;

    /* send message */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);
    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaSrvInfoNotify: Send message is failure");
    }
}


VOS_VOID TAF_MMA_SndImsaCampInfoChangeInd(VOS_VOID)
{
    MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU *pstSndMsg    = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    /* 申请内存  */
    pstSndMsg = (MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( VOS_NULL_PTR == pstSndMsg )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaCampInfoChangeInd: Memory allocation is failure");
        return;
    }

    PS_MEM_SET(pstSndMsg, 0, sizeof(MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU));
    
    /* fill in header information */
    pstSndMsg->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid        = WUEPS_PID_MMA;
    pstSndMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid      = PS_PID_IMSA;
    pstSndMsg->ulLength           = sizeof(MMA_IMSA_CAMP_INFO_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* fill in message name */
    pstSndMsg->ulMsgId            = ID_MMA_IMSA_CAMP_INFO_CHANGE_IND;

    /* fill in Roaming flag */
    pstSndMsg->ucRoamingFlg       = TAF_SDC_GetRoamFlag();

    /* set PLMN identity */
    pstSndMsg->stPlmnId.ulMcc     = TAF_SDC_GetCurrCampPlmnId()->ulMcc;
    pstSndMsg->stPlmnId.ulMnc     = TAF_SDC_GetCurrCampPlmnId()->ulMnc;
    
    /* set cell identity */
    pstSndMsg->ulCellId           = TAF_SDC_GetCurrCampCellId();

    if (TAF_SDC_SYS_MODE_LTE == TAF_SDC_GetSysMode())
    {
        /* hardcode this access type to TDD EUTRAN if RAT is LTE here */
        pstSndMsg->enAccessType   = MMA_IMSA_ACCESS_TYPE_EUTRAN_TDD;

        /* Get TAC */
        pstSndMsg->usTac          = ((TAF_SDC_GetCurrCampLac() >> 8) & 0x00FF);
    }
    else
    {
        if (TAF_SDC_SYS_MODE_WCDMA == TAF_SDC_GetSysMode())
        {
            if (NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode())
            {
                pstSndMsg->enAccessType = MMA_IMSA_ACCESS_TYPE_UTRAN_FDD;
            }
            else
            {
                pstSndMsg->enAccessType = MMA_IMSA_ACCESS_TYPE_UTRAN_TDD;
            }
        }
        else
        {
            pstSndMsg->enAccessType     = MMA_IMSA_ACCESS_TYPE_GERAN;
        }

        /* Get LAC */
        pstSndMsg->usLac          = TAF_SDC_GetCurrCampLac();
    }
    
    /* send message */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstSndMsg);

    if (VOS_OK != ulRet)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaCampInfoChangeInd: Send message is failure");
    }
}


VOS_VOID TAF_MMA_SndImsaStopReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_IMSA_STOP_REQ_STRU             *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_IMSA_STOP_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_IMSA_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaStopReq:ERROR: Memory Alloc Error for pMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_IMSA_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_IMSA;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_IMSA_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMA_IMSA_STOP_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstMsg);

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaStopReq:ERROR:PS_SEND_MSG FAILURE!");
    }

    return;
}


VOS_VOID TAF_MMA_SndImsaStartReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MMA_IMSA_START_REQ_STRU            *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (MMA_IMSA_START_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMA,
                           sizeof(MMA_IMSA_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaStartReq:ERROR: Memory Alloc Error for pMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMA_IMSA_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_IMSA;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMA;
    pstMsg->ulLength         = sizeof(MMA_IMSA_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMA_IMSA_START_REQ;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_MMA, pstMsg);

    if ( VOS_OK != ulRet )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SndImsaStartReq:ERROR:PS_SEND_MSG FAILURE!");
    }

    return;
}
#endif


VOS_UINT32 TAF_MMA_IsCGIInfoChanged(
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstOldCampInfo,
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstNewCampInfo
)
{
    /* 新老Plmn或 lac,cellid,access type或系统子模式是否有变化 */
    if ((pstOldCampInfo->stPlmnId.ulMcc  != pstNewCampInfo->stPlmnId.ulMcc)
     || (pstOldCampInfo->stPlmnId.ulMnc  != pstNewCampInfo->stPlmnId.ulMnc)
     || (pstOldCampInfo->enSysMode       != pstNewCampInfo->enSysMode)
     || (pstOldCampInfo->usLac           != pstNewCampInfo->usLac)
     || (pstOldCampInfo->ulCellId        != pstNewCampInfo->ulCellId)
     || (pstOldCampInfo->ucRoamFlag      != pstNewCampInfo->ucRoamFlag))
    {
        return VOS_TRUE;
    }
    
    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_IsNetworkCapInfoChanged(
    TAF_SDC_NETWORK_CAP_INFO_STRU       *pstNewNwCapInfo
)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU       *pstOldNwCapInfo = VOS_NULL_PTR;

    /* 先获取LTE的网络能力信息 */
    pstOldNwCapInfo = TAF_SDC_GetLteNwCapInfo();

    /* 新旧能力是否有变化 */
    if ((pstOldNwCapInfo->enLteCsCap   != pstNewNwCapInfo->enLteCsCap)
     || (pstOldNwCapInfo->enNwEmcBsCap != pstNewNwCapInfo->enNwEmcBsCap)
     || (pstOldNwCapInfo->enNwImsVoCap != pstNewNwCapInfo->enNwImsVoCap))
    {
        return VOS_TRUE;
    }

    /* 再获取GU的网络能力信息 */
    pstOldNwCapInfo = TAF_SDC_GetGuNwCapInfo();

    /* 新旧能力是否有变化 */
    if ((pstOldNwCapInfo->enLteCsCap   != pstNewNwCapInfo->enLteCsCap)
     || (pstOldNwCapInfo->enNwEmcBsCap != pstNewNwCapInfo->enNwEmcBsCap)
     || (pstOldNwCapInfo->enNwImsVoCap != pstNewNwCapInfo->enNwImsVoCap))
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


