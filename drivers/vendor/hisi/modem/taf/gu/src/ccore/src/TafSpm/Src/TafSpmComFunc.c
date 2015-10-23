

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "v_msg.h"
#include "PsTypeDef.h"
#include "TafLog.h"
#include "TafSpmMntn.h"
#include "TafSdcCtx.h"
#include "TafStdlib.h"
#include "NasUsimmApi.h"
#include "MnCallMnccProc.h"
#include "MmaAppLocal.h"
#include "TafSpmComFunc.h"
#include "MnCallCtx.h"
#include "MnCall.h"
#include "MnComm.h"
#include "TafAgentInterface.h"
#include "TafStdlib.h"
#include "MnErrorCode.h"
#include "MnMsgApi.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_SPM_COM_FUNC_C

/*****************************************************************************
  2 常量定义
*****************************************************************************/



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

/*****************************************************************************
 函 数 名  : TAF_SPM_IsValidEmerCategory
 功能描述  : 判定是否紧急呼叫号码的类型是否合法
 输入参数  : ucEmerCategory  - 紧急呼叫类型
 输出参数  : 无
 返 回 值  : VOS_TRUE  - 合法
             VOS_FALSE - 不合法
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年12月29日
    作    者   : 周君 40661
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 TAF_SPM_IsValidEmerCategory(
    VOS_UINT8                           ucEmerCategory
)
{
    VOS_UINT32                          ulRet;

    if (ucEmerCategory & MN_CALL_EMER_CATEGORG_POLICE)
    {
        ulRet = VOS_TRUE;
    }
    else if (ucEmerCategory & MN_CALL_EMER_CATEGORG_AMBULANCE)
    {
        ulRet = VOS_TRUE;
    }
    else if (ucEmerCategory & MN_CALL_EMER_CATEGORG_FIRE_BRIGADE)
    {
        ulRet = VOS_TRUE;
    }
    else if (ucEmerCategory & MN_CALL_EMER_CATEGORG_MARINE_GUARD)
    {
        ulRet = VOS_TRUE;
    }
    else if (ucEmerCategory & MN_CALL_EMER_CATEGORG_MOUNTAIN_RESCUE)
    {
        ulRet = VOS_TRUE;
    }
    else
    {
        ulRet = VOS_FALSE;
    }

    return ulRet;
}


VOS_UINT32  TAF_SPM_IsNetworkEmergencyNum(
    MN_CALL_CALLED_NUM_STRU             *pstDialNum,
    VOS_UINT32                          *pulEmerCatExist,
    VOS_UINT8                           *pucEmerCategory
)
{
    VOS_UINT8                           i;
    TAF_SDC_MM_EMERGENCY_LIST_STRU     *pstMmCallEmerNumList    = VOS_NULL_PTR;

    pstMmCallEmerNumList    = TAF_SDC_GetMmCallEmerNumList();

    for (i = 0; i < pstMmCallEmerNumList->ucEmergencyNumber; i++)
    {
        if (pstMmCallEmerNumList->astEmergencyLists[i].ucEmcNumLen != pstDialNum->ucNumLen)
        {
            continue;
        }

        if ( 0 == VOS_MemCmp(pstMmCallEmerNumList->astEmergencyLists[i].aucEmergencyList,
                             pstDialNum->aucBcdNum,
                             pstDialNum->ucNumLen))
        {
            *pulEmerCatExist
                = TAF_SPM_IsValidEmerCategory(pstMmCallEmerNumList->astEmergencyLists[i].ucCategoryValue);

             if (VOS_TRUE == *pulEmerCatExist)
             {
                 *pucEmerCategory = (pstMmCallEmerNumList->astEmergencyLists[i].ucCategoryValue
                                   & (0x1F));
             }

             return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_UINT32  TAF_SPM_IsAppCustomEmergencyNum(
    MN_CALL_CALLED_NUM_STRU             *pstDialNum,
    VOS_UINT32                          *pulEmerCatExist,
    VOS_UINT8                           *pucEmerCategory
)
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSimStatus;
    TAF_SDC_CUSTOM_ECC_CTX_STRU        *pstCustomCallEmerNumCtx    = VOS_NULL_PTR;
    VOS_UINT8                           i;

    pstCustomCallEmerNumCtx = TAF_SDC_GetCustomCallEccNumCtx();

    /* 获取卡的状态，根据卡的存在与否做不同处理 */
    enSimStatus = TAF_SDC_GetSimStatus();

    /* 检查是否APP定制紧急呼号码 */
    for (i = 0; i < pstCustomCallEmerNumCtx->stCustomEccNumList.ucEccNumCount; i++)
    {
        if (pstDialNum->ucNumLen != pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucEccNumLen)
        {
            continue;
        }

        if (0 == VOS_MemCmp(pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].aucEccNum,
                             pstDialNum->aucBcdNum,
                             pstDialNum->ucNumLen) )
        {
            if (TAF_SDC_USIM_STATUS_NO_PRESENT == enSimStatus)
            {
                /* 号码是有卡时的紧急呼号码 */
                if ((MN_CALL_ECC_NUM_VALID_SIM_ABSENT != pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucValidSimPresent))
                {
                    continue;
                }
            }
            else
            {
                /* 号码是无卡时的紧急呼号码 */
                if ((MN_CALL_ECC_NUM_VALID_SIM_PRESENT != pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucValidSimPresent))
                {
                    continue;
                }
            }

            *pulEmerCatExist
                = TAF_SPM_IsValidEmerCategory(pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucCategory);

             if (VOS_TRUE == *pulEmerCatExist)
             {
                 *pucEmerCategory = (pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucCategory
                                   & (0x1F));
             }

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SPM_IsPhoneEmergencyNum(
    MN_CALL_CALLED_NUM_STRU            *pstDialNum
)
{
    VOS_UINT32                          ulLoop;
    VOS_CHAR                          **pucEmcNumsNoUsim    = VOS_NULL_PTR;
    VOS_CHAR                          **pucEmcNumsWithUsim  = VOS_NULL_PTR;
    VOS_INT8                            acCalledNum[MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1];

    /* 号码转换 BCD-> Ascii */
    if (MN_ERR_NO_ERROR != TAF_STD_ConvertBcdNumberToAscii(pstDialNum->aucBcdNum,
                                                           pstDialNum->ucNumLen,
                                                           (VOS_CHAR *)acCalledNum))
    {
        MN_WARN_LOG("TAF_SPM_IsPhoneEmergencyNum: BCD -> Ascii ERROR.");

        return VOS_FALSE;
    }

    pucEmcNumsNoUsim    = TAF_SDC_GetCallEmerNumsNoUsimTblAddr();
    pucEmcNumsWithUsim  = TAF_SDC_GetCallEmerNumsWithUsimTblAddr();

    if (TAF_SDC_USIM_STATUS_NO_PRESENT == TAF_SDC_GetSimStatus())
    {
        for (ulLoop = 0; ulLoop < TAF_SDC_NUMOF_EMER_NUMS_NO_USIM; ulLoop++)
        {
            if (0 == VOS_StrCmp(pucEmcNumsNoUsim[ulLoop], (VOS_CHAR *)acCalledNum))
            {
                return VOS_TRUE;
            }
        }
    }
    else
    {

        /* 有卡时检查手机中存储的紧急呼叫号码 */
        for (ulLoop = 0; ulLoop < TAF_SDC_NUMOF_EMER_NUMS_WITH_USIM; ulLoop++ )
        {
            if (0 == VOS_StrCmp(pucEmcNumsWithUsim[ulLoop], (VOS_CHAR *)acCalledNum))
            {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_UINT32  TAF_SPM_IsUsimEmergencyNum(
    MN_CALL_CALLED_NUM_STRU            *pstDialNum,
    VOS_UINT32                         *pulEmcCatExist,
    VOS_UINT8                          *pucEmerCategory
)
{
    SI_PB_ECC_DATA_STRU                *pstEccData = VOS_NULL_PTR;
    VOS_INT8                            acUsimCalledNum[MN_CALL_MAX_ASCII_NUM_LEN + 1];
    VOS_INT8                            acCalledNum[MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1];
    MN_CALL_TYPE_ENUM_U8                enCallType;
    VOS_UINT32                          i;

    enCallType  = MN_CALL_TYPE_VOICE;

    /* 获取USIM中的紧急呼叫号码 */
    pstEccData = (SI_PB_ECC_DATA_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(SI_PB_ECC_DATA_STRU));

    if (VOS_NULL_PTR == pstEccData)
    {
        MN_WARN_LOG("TAF_SPM_IsUsimEmergencyNum: Alloc Mem Fail.");

        return VOS_FALSE;
    }

    /* 号码转换 BCD-> Ascii */
    if (MN_ERR_NO_ERROR != TAF_STD_ConvertBcdNumberToAscii(pstDialNum->aucBcdNum,
                                                           pstDialNum->ucNumLen,
                                                           (VOS_CHAR *)acCalledNum))
    {
        MN_WARN_LOG("TAF_SPM_IsUsimEmergencyNum: BCD -> Ascii ERROR.");

        PS_MEM_FREE(WUEPS_PID_TAF, pstEccData);

        return VOS_FALSE;
    }

    PS_MEM_SET(pstEccData, 0, sizeof(SI_PB_ECC_DATA_STRU));
    NAS_PBAPI_GetEccNumber(pstEccData);

    /* 比较号码是否是EFECC文件中的紧急呼叫号码 */
    for (i = 0; i< pstEccData->ulReocrdNum; i++)
    {
        PS_MEM_SET(acUsimCalledNum,0,MN_CALL_MAX_ASCII_NUM_LEN + 1);

        if (MN_ERR_NO_ERROR != TAF_STD_ConvertBcdNumberToAscii(pstEccData->astEccRecord[i].aucEccCode,
                                                               USIM_ECC_LEN,
                                                               (VOS_CHAR *)acUsimCalledNum))
        {
            MN_INFO_LOG("TAF_SPM_IsUsimEmergencyNum: TAF_STD_ConvertBcdNumberToAscii fail");
        }

        if (0 == VOS_StrCmp((VOS_CHAR *)acUsimCalledNum, (VOS_CHAR *)acCalledNum))
        {
            enCallType = MN_CALL_TYPE_EMERGENCY;
            MN_INFO_LOG("TAF_SPM_IsUsimEmergencyNum: get card Ecc change to emergency");
            break;
        }
    }

    /* 当前是紧急呼叫号码则判断号码类型是否有效 */
    if (MN_CALL_TYPE_EMERGENCY == enCallType)
    {
        if (VOS_TRUE == pstEccData->astEccRecord[i].bESC)
        {
            *pulEmcCatExist = TAF_SPM_IsValidEmerCategory(pstEccData->astEccRecord[i].ucESC);
        }

        if (VOS_TRUE == *pulEmcCatExist)
        {
            *pucEmerCategory = (pstEccData->astEccRecord[i].ucESC & 0x1F);
        }

        PS_MEM_FREE(WUEPS_PID_TAF, pstEccData);

        return VOS_TRUE;
    }

    PS_MEM_FREE(WUEPS_PID_TAF, pstEccData);

    return VOS_FALSE;
}


VOS_UINT32  TAF_SPM_IsEmergencyNum(
    MN_CALL_CALLED_NUM_STRU            *pstDialNum,
    VOS_UINT32                          ulCheckEfecc,
    MN_CALL_EMERGENCY_CAT_STRU         *pstEmergencyCat
)
{
    VOS_UINT8                           ucEmergencyCall;

    ucEmergencyCall = VOS_FALSE;
    
    /* 检查是否(U)SIM卡中EFECC文件中的紧急呼叫号码 */
    if (TAF_SDC_USIM_STATUS_NO_PRESENT != TAF_SDC_GetSimStatus())
    {
        if (VOS_TRUE == ulCheckEfecc)
        {
            if (VOS_TRUE == TAF_SPM_IsUsimEmergencyNum(pstDialNum,
                                                        &pstEmergencyCat->bExist,
                                                        &pstEmergencyCat->ucEmergencyCat))
            {
                /* 是USIM中的紧急呼叫,且CATEGORY有效则直接返回 */
                if (VOS_TRUE  == pstEmergencyCat->bExist)
                {
                    return VOS_TRUE;
                }

                ucEmergencyCall = VOS_TRUE;
            }
        }

        /* 检查是否网络上报紧急呼叫号码列表中的紧急呼叫号码 */
        if (VOS_TRUE == TAF_SPM_IsNetworkEmergencyNum(pstDialNum,
                                                      &pstEmergencyCat->bExist,
                                                      &pstEmergencyCat->ucEmergencyCat))
        {
            /* 是网络中的紧急呼叫,且CATEGORY有效则直接返回 */
            if (VOS_TRUE == pstEmergencyCat->bExist)
            {
                return VOS_TRUE;
            }

            ucEmergencyCall = VOS_TRUE;

        }
    }

    /* 检查是否运营商定制紧急呼叫号码列表中的紧急呼叫号码 */
    if (VOS_TRUE == TAF_SPM_IsAppCustomEmergencyNum(pstDialNum,
                                                    &pstEmergencyCat->bExist,
                                                    &pstEmergencyCat->ucEmergencyCat))
    {
        /* 是网络中的紧急呼叫,且CATEGORY有效则直接返回 */
        if (VOS_TRUE == pstEmergencyCat->bExist)
        {
            return VOS_TRUE;
        }

        ucEmergencyCall = VOS_TRUE;
    }

    /* 检查是否协议定义的紧急呼叫号码 */
    if (VOS_TRUE == TAF_SPM_IsPhoneEmergencyNum(pstDialNum))
    {
        ucEmergencyCall = VOS_TRUE;
    }

    if (VOS_TRUE == ucEmergencyCall)
    {
        return VOS_TRUE;
    }
    
    return VOS_FALSE;
}
VOS_UINT32 TAF_SPM_GetBcCallType(
    VOS_UINT32                          ulExistBc,
    VOS_UINT32                          ulBcLength,
    VOS_UINT8                          *pucBc,
    MN_CALL_TYPE_ENUM_U8               *penCallType
)
{
    TAF_CALL_BC_STRU                    stBc1;

    /* 紧急呼叫不修改呼叫类型 */
    if (MN_CALL_TYPE_EMERGENCY == *penCallType)
    {
        return VOS_TRUE;
    }

    /* BC不存在不修改呼叫类型 */
    if (VOS_TRUE != ulExistBc)
    {
        return VOS_TRUE;
    }
    else
    {
        PS_MEM_SET(&stBc1, 0, sizeof(stBc1));
        stBc1.ulLength = ulBcLength;
        PS_MEM_CPY(&stBc1.Octet3, pucBc, stBc1.ulLength);
    }

    /* 获取新的BC是指示的呼叫类型 */
    if (VOS_OK != MN_CALL_JudgeMtCallType(&stBc1.Octet3, &stBc1.Octet5a, penCallType))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_SPM_IsUsimServiceAvailable(VOS_VOID)
{
    VOS_UINT32                           ulRslt;

    /* 检查PIN状态 */
    switch (  TAF_SDC_GetSimStatus() )
    {
        case TAF_SDC_USIM_STATUS_ROMSIM:
        case TAF_SDC_USIM_STATUS_VALID:
        case TAF_SDC_USIM_STATUS_CS_UNVALID:
        /* 当PS业务无效时，在L下会disable LTE，因此此种场景下，后续允许被缓存 */
        case TAF_SDC_USIM_STATUS_PS_UNVALID:
            ulRslt = VOS_TRUE;
            break;

        case TAF_SDC_USIM_STATUS_SIM_PIN:
        case TAF_SDC_USIM_STATUS_SIM_PUK:
        case TAF_SDC_USIM_STATUS_NET_LCOK:
        case TAF_SDC_USIM_STATUS_IMSI_LCOK:
        case TAF_SDC_USIM_STATUS_UNVALID:
        case TAF_SDC_USIM_STATUS_NO_PRESENT:
        case TAF_SDC_USIM_STATUS_BUTT:
        case TAF_SDC_USIM_STATUS_CS_PS_UNVALID:
        case TAF_SDC_USIM_STATUS_READY_TIMEROUT:
            ulRslt = VOS_FALSE;
            break;

        default:
            ulRslt = VOS_FALSE;
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_GetStatusOfUsimServiceAvailable(): something wrong!");
            break;
    }

    return ulRslt;
}
VOS_UINT32 TAF_SPM_IsUssdServiceType(
    VOS_UINT32                          ulEventType
)
{
    VOS_UINT32                          ulResult;

    ulResult  = VOS_FALSE;

    switch (ulEventType)
    {
        case TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_PROCESS_USS_MSG):
        case TAF_BuildEventType(MAPS_STK_PID, STK_SS_USSD_REQ):

            ulResult = VOS_TRUE;
            break;

        default:

            break;
    }

    return ulResult;
}



TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8 TAF_SPM_GetServiceRequestTypeWithAtClient(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstAppMsg = VOS_NULL_PTR;
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;

    pstAppMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    enSrvReqType = TAF_SPM_SRV_REQ_TYPE_BUTT;

    switch (pstAppMsg->usMsgName & MN_APP_MSG_CLASS_MASK)
    {
        /* call service request */
        case MN_APP_MSG_CLASS_CALL:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_CALL;
            break;

        /* SMS service request */
        case MN_APP_MSG_CLASS_MSG:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_SMS;
            break;

        /* SS service request */
        case MN_APP_MSG_CLASS_SSA:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_SS;
            break;

        default:
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_GetServiceRequestTypeWithAtClient: unknown message is received from AT!");
            break;
    }

    return enSrvReqType;
}
TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8 TAF_SPM_GetServiceRequestTypeWithStkClient(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstAppMsg = VOS_NULL_PTR;
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;

    pstAppMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    enSrvReqType = TAF_SPM_SRV_REQ_TYPE_BUTT;

    switch (pstAppMsg->usMsgName)
    {
        /* STK call service request */
        case STK_CALL_CALLORIG_REQ:
        case STK_CALL_SUPS_CMD_REQ:
        case STK_CALL_START_DTMF_REQ:
        case STK_CALL_STOP_DTMF_REQ:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_CALL;
            break;

        /* STK SMS service request */
        case STK_MSG_SEND_REQ:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_SMS;
            break;

        /* STK SS service request */
        case STK_SS_REGISTERSS_REQ:
        case STK_SS_ERASESS_REQ:
        case STK_SS_ACTIVATESS_REQ:
        case STK_SS_DEACTIVATESS_REQ:
        case STK_SS_INTERROGATESS_REQ:
        case STK_SS_USSD_REQ:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_SS;
            break;

        default:
            TAF_ERROR_LOG(WUEPS_PID_TAF,
                          "TAF_SPM_GetServiceRequestTypeWithStkClient: unknown message is received from STK!");

            break;
    }

    return enSrvReqType;
}
TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8 TAF_SPM_GetServiceRequestTypeWithTafClient(
    struct MsgCB                       *pstMsg
)
{
    TAF_INTERNAL_MSG_STRU              *pstTafMsg = VOS_NULL_PTR;
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;

    pstTafMsg = (TAF_INTERNAL_MSG_STRU *)pstMsg;

    enSrvReqType = TAF_SPM_SRV_REQ_TYPE_BUTT;

    switch (pstTafMsg->ulMsgId)
    {
        /* ID_TAF_SPM_SMMA_IND */
        case ID_TAF_SPM_SMMA_IND:
            enSrvReqType = TAF_SPM_SRV_REQ_TYPE_SMS;
            break;

        default:
            TAF_ERROR_LOG(WUEPS_PID_TAF,
                          "TAF_SPM_GetServiceRequestTypeWithTafClient: unknown message is received from TAF!");

            break;
    }

    return enSrvReqType;
}
TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8 TAF_SPM_GetServiceRequestType(
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstAppMsg = VOS_NULL_PTR;
    TAF_SPM_SRV_REQ_TYPE_ENUM_UINT8     enSrvReqType;

    pstAppMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    enSrvReqType = TAF_SPM_SRV_REQ_TYPE_BUTT;

    /* messages from AT */
    if (WUEPS_PID_AT == (pstAppMsg->ulSenderPid))
    {
        enSrvReqType = TAF_SPM_GetServiceRequestTypeWithAtClient(pstMsg);
    }
    /* messages from STK */
    else if (MAPS_STK_PID == (pstAppMsg->ulSenderPid))
    {
        enSrvReqType = TAF_SPM_GetServiceRequestTypeWithStkClient(pstMsg);
    }
    /* other messages */
    else if (WUEPS_PID_TAF == (pstAppMsg->ulSenderPid))
    {
        enSrvReqType = TAF_SPM_GetServiceRequestTypeWithTafClient(pstMsg);
    }
    else
    {
        /* nothing to do */
    }

    return (enSrvReqType);
}


#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID TAF_SPM_SendTafAgentGetCallInfoCnf(
    IMSA_SPM_CALL_GET_CALL_INFO_CNF_STRU    *pstImsaMsg
)
{
    TAFAGERNT_MN_GET_CALL_INFO_CNF_STRU *pstCnf = VOS_NULL_PTR;
    VOS_UINT32                           i;

    pstCnf = (TAFAGERNT_MN_GET_CALL_INFO_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                                sizeof(TAFAGERNT_MN_GET_CALL_INFO_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstCnf)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_SendTafAgentGetCallInfoCnf:alloc msg failed.");
        return;
    }

    PS_MEM_SET(pstCnf, 0x00, sizeof(TAFAGERNT_MN_GET_CALL_INFO_CNF_STRU));

    /* 填写消息内容 */
    pstCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pstCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pstCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pstCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pstCnf->ulLength                      = sizeof(TAFAGERNT_MN_GET_CALL_INFO_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstCnf->enMsgId                       = ID_TAFAGENT_MN_GET_CALL_INFO_CNF;
    pstCnf->ucNumOfCalls                  = pstImsaMsg->ucNumOfCalls;

    /* IMSA消息中带的信息比较多，回复给TAFAGENT时，只需要callId,enCallType,enCallState */
    for (i = 0; i < pstImsaMsg->ucNumOfCalls; i++)
    {
        pstCnf->stCallInfo[i].callId       = pstImsaMsg->astCallInfo[i].callId;
        pstCnf->stCallInfo[i].enCallType   = pstImsaMsg->astCallInfo[i].enCallType;
        pstCnf->stCallInfo[i].enCallState  = pstImsaMsg->astCallInfo[i].enCallState;

    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pstCnf))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_SPM_SendTafAgentGetCallInfoCnf:send msg failed.");

    }

    return;

}


VOS_VOID TAF_SPM_SendAtGetCallInfoCnf(
    IMSA_SPM_CALL_GET_CALL_INFO_CNF_STRU    *pstImsaMsg
)
{
    MN_CALL_INFO_QRY_CNF_STRU           stAppCnfMsg;
    VOS_UINT16                          usLength;

    /* 构造一条消息，给AT回复 */
    PS_MEM_SET(&stAppCnfMsg, 0x00, sizeof(MN_CALL_INFO_QRY_CNF_STRU));

    /* 填写事件名称 */
    stAppCnfMsg.enEvent         = MN_CALL_EVT_CLCC_INFO;
    stAppCnfMsg.clientId        = MN_GetRealClientId(pstImsaMsg->usClientId, WUEPS_PID_TAF);
    stAppCnfMsg.ucNumOfCalls    = pstImsaMsg->ucNumOfCalls;

    PS_MEM_CPY(stAppCnfMsg.astCallInfos,
               pstImsaMsg->astCallInfo,
               sizeof(MN_CALL_INFO_PARAM_STRU)*(stAppCnfMsg.ucNumOfCalls));

    /* 确定消息长度 */
    usLength = (VOS_UINT16)sizeof(MN_CALL_INFO_QRY_CNF_STRU);

    /* 通过异步消息发送给AT */
    MN_SendReportMsg(MN_CALLBACK_CS_CALL, (TAF_UINT8 *)&stAppCnfMsg, usLength);

    return;

}
#endif



VOS_VOID TAF_SPM_GetCallInfoFromFsmEntryMsg(
    MN_CALL_CALLED_NUM_STRU            *pstDialNumber,
    MN_CALL_TYPE_ENUM_U8               *penCallType,
    MN_CALL_MODE_ENUM_U8               *penCallMode,
    MN_CALL_CS_DATA_CFG_STRU           *pstDataCfg
)
{
    MN_APP_REQ_MSG_STRU                *pstAppMsg       = VOS_NULL_PTR;
    MN_APP_CALL_CALLORIG_REQ_STRU      *pstStkOrigParam = VOS_NULL_PTR;
    TAF_SPM_ENTRY_MSG_STRU             *pstEntryMsg     = VOS_NULL_PTR;
    MN_CALL_APP_REQ_PARM_UNION         *pstAtOrigParam  = VOS_NULL_PTR;
    NAS_CC_IE_BC_STRU                   stBc1CallCnf;

    pstEntryMsg     = TAF_SPM_GetCurrEntityFsmEntryMsgAddr();
    pstAppMsg       = (MN_APP_REQ_MSG_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 区分AT或STK的 */
    if (WUEPS_PID_AT == TAF_SPM_GetCurrEntityFsmEntryMsgSndPid())
    {
        pstAtOrigParam  = (MN_CALL_APP_REQ_PARM_UNION *)pstAppMsg->aucContent;

        *penCallType    = pstAtOrigParam->stOrig.enCallType;
        *penCallMode    = pstAtOrigParam->stOrig.enCallMode;

        PS_MEM_CPY(pstDialNumber, &pstAtOrigParam->stOrig.stDialNumber, sizeof(pstAtOrigParam->stOrig.stDialNumber));
        PS_MEM_CPY(pstDataCfg, &pstAtOrigParam->stOrig.stDataCfg, sizeof(pstAtOrigParam->stOrig.stDataCfg));
    }
    else
    {
        pstStkOrigParam = (MN_APP_CALL_CALLORIG_REQ_STRU *)pstAppMsg;

        *penCallType    = pstStkOrigParam->enCallType;
        *penCallMode    = MN_CALL_MODE_SINGLE;

        PS_MEM_CPY(pstDialNumber, (MN_CALL_CALLED_NUM_STRU *)&pstStkOrigParam->stCalledAddr, sizeof(pstStkOrigParam->stCalledAddr));

        stBc1CallCnf.IsExist        = VOS_TRUE;
        stBc1CallCnf.LastOctOffset  = (VOS_UINT8)pstStkOrigParam->stBc.ucLen;
        PS_MEM_CPY(&stBc1CallCnf.Octet3, pstStkOrigParam->stBc.aucBc, pstStkOrigParam->stBc.ucLen);

        MN_CALL_GetDataCfgInfoFromBc(&stBc1CallCnf, (MN_CALL_CS_DATA_CFG_INFO_STRU *)pstDataCfg);
    }

    return;
}
VOS_UINT32 TAF_SPM_IsNeedCheckFdn()
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU *pstServiceCtrlCfgInfo = VOS_NULL_PTR;

    VOS_UINT32                          ulFdnStatus;

    pstServiceCtrlCfgInfo   = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    /* ME没有启用FDN检查功能，直接返回业务检查通过 */
    if (TAF_FDN_CHECK_FEATURE_ON != pstServiceCtrlCfgInfo->ulMeStatus)
    {
        return VOS_FALSE;
    }

    /*
    根据(U)SIM卡的FDN状态确定是否发起FDN检查
       0 去使能: 不做FDN检查
       1 使能:   做FDN检查
    */
    NAS_USIMMAPI_FdnQuery(&ulFdnStatus);

    if (VOS_TRUE == ulFdnStatus)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SPM_IsNeedCallControl(VOS_VOID)
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU *pstServiceCtrlCfgInfo = VOS_NULL_PTR;

    pstServiceCtrlCfgInfo   = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    /* ME没有启用CALL CONTROL检查功能，直接返回业务检查通过 */
    if (VOS_FALSE == pstServiceCtrlCfgInfo->ucSimCallCtrlSupportFlg)
    {
        return VOS_FALSE;
    }

    /*
    根据(U)SIM卡的CALL CONTROL状态确定是否发起CALL CONTROL检查
       0 去使能: 不做CALL CONTROL检查
       1 使能:   做CALL CONTROL检查
    如果是去使能状态，则退出；
    */
    if (VOS_TRUE != NAS_USIMMAPI_IsServiceAvailable(NAS_USIM_SVR_CALL_CONTROL))
    {

        return VOS_FALSE;
    }


    return VOS_TRUE;
}


VOS_UINT32 TAF_SPM_IsUESupportMoCallType(
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU *pstServiceCtrlCfgInfo = VOS_NULL_PTR;

    pstServiceCtrlCfgInfo   = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    /* 修改为VIDEO CALL且NV支持VT的主叫 */
    if (MN_CALL_TYPE_VIDEO     == enCallType)
    {
        if ((TAF_SPM_VP_MO_ONLY    == pstServiceCtrlCfgInfo->enVpCfgState)
         || (TAF_SPM_VP_MO_MT_BOTH == pstServiceCtrlCfgInfo->enVpCfgState))
        {
            return VOS_TRUE;
        }

        return VOS_FALSE;
    }

    if ((MN_CALL_TYPE_VOICE     == enCallType)
     || (MN_CALL_TYPE_EMERGENCY == enCallType))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



#if (FEATURE_ON == FEATURE_IMS)
VOS_VOID TAF_SPM_ProcSmmaRpt(TAF_SPM_MSG_REPORT_IND_STRU  *pstMsgReportInd)
{
    MN_MSG_EVENT_INFO_STRU             *pstMsgEventInfo = VOS_NULL_PTR;

    /* 不需要报道SMMA结果事件，返回 */
    if (VOS_FALSE == pstMsgReportInd->ucReportFlag)
    {
        return;
    }

    pstMsgEventInfo = PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstMsgEventInfo )
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
                      "TAF_SPM_ProcSmmaRpt: ERROR:Memory Alloc is failure!");

        return;
    }

    PS_MEM_SET(pstMsgEventInfo, 0, sizeof(MN_MSG_EVENT_INFO_STRU));

    /* 设置client ID与op ID */
    pstMsgEventInfo->clientId = pstMsgReportInd->usClientId;
    pstMsgEventInfo->opId     = pstMsgReportInd->ucOpId;

    if (TAF_MSG_ERROR_NO_ERROR != pstMsgReportInd->stRptEvtInfo.enErrorCode)
    {
        pstMsgEventInfo->u.stMemStatusInfo.bSuccess    = VOS_FALSE;

        pstMsgEventInfo->u.stMemStatusInfo.ulFailCause = pstMsgReportInd->stRptEvtInfo.enErrorCode;

        pstMsgEventInfo->u.stMemStatusInfo.enMemFlag   = MN_MSG_MEM_FULL_SET;

        /* 上报SMMA事件 */
        MN_SendClientEvent(pstMsgReportInd->usClientId,
                           MN_CALLBACK_MSG,
                           MN_MSG_EVT_SET_MEMSTATUS,
                           (VOS_VOID *)pstMsgEventInfo);

    }
    else
    {
        pstMsgEventInfo->u.stMemStatusInfo.bSuccess    = VOS_TRUE;
        pstMsgEventInfo->u.stMemStatusInfo.ulFailCause = MN_ERR_NO_ERROR;
        pstMsgEventInfo->u.stMemStatusInfo.enMemFlag   = MN_MSG_MEM_FULL_UNSET;

        /* 上报SMMA事件 */
        MN_SendClientEvent(pstMsgReportInd->usClientId,
                           MN_CALLBACK_MSG,
                           MN_MSG_EVT_SET_MEMSTATUS,
                           (VOS_VOID *)&pstMsgEventInfo);

        /* 更新op ID为广播op ID */
        pstMsgEventInfo->opId = pstMsgReportInd->ucBcOpId;
        /* 广播事件 */
        MN_SendClientEvent(MN_CLIENT_ALL,
                           MN_CALLBACK_MSG,
                           MN_MSG_EVT_MEMSTATUS_CHANGED,
                           (VOS_VOID *)pstMsgEventInfo);

    }

    PS_MEM_FREE(WUEPS_PID_TAF, pstMsgEventInfo);
}


VOS_VOID TAF_SPM_ProcMoSmsRpt(TAF_SPM_MSG_REPORT_IND_STRU  *pstMsgReportInd)
{
    MN_MSG_EVENT_INFO_STRU             *pstMsgEventInfo = VOS_NULL_PTR;

    pstMsgEventInfo = PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_EVENT_INFO_STRU));

    if (VOS_NULL_PTR == pstMsgEventInfo)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
                      "TAF_SPM_ProcMoSmsRpt: ERROR:Memory Alloc is failure!");

        return;
    }

    PS_MEM_SET(pstMsgEventInfo, 0, sizeof(MN_MSG_EVENT_INFO_STRU));

    pstMsgEventInfo->u.stSubmitRptInfo.enSaveArea  = pstMsgReportInd->stRptEvtInfo.enSaveArea;
    pstMsgEventInfo->u.stSubmitRptInfo.ulSaveIndex = pstMsgReportInd->stRptEvtInfo.ulSaveIndex;
    pstMsgEventInfo->u.stSubmitRptInfo.ucMr        = pstMsgReportInd->stRptEvtInfo.ucMr;

    pstMsgEventInfo->clientId                      = pstMsgReportInd->usClientId;
    pstMsgEventInfo->opId                          = pstMsgReportInd->ucOpId;

    if (TAF_MSG_ERROR_NO_ERROR != pstMsgReportInd->stRptEvtInfo.enErrorCode)
    {
        /* 短信发送失败，则转换cause值 */
        pstMsgEventInfo->u.stSubmitRptInfo.enErrorCode = pstMsgReportInd->stRptEvtInfo.enErrorCode;
    }

    /* 通知应用短信发送结果 */
    MN_SendClientEvent(pstMsgReportInd->usClientId,
                       MN_CALLBACK_MSG,
                       MN_MSG_EVT_SUBMIT_RPT,
                       (VOS_VOID *)pstMsgEventInfo);

    PS_MEM_FREE(WUEPS_PID_TAF, pstMsgEventInfo);
}


VOS_VOID TAF_SPM_ProcSmsRptEvent(TAF_SPM_MSG_REPORT_IND_STRU  *pstMsgReportInd)
{
    if (TAF_SMS_MO_TYPE_SMMA == pstMsgReportInd->enSmsMoType)
    {
        TAF_SPM_ProcSmmaRpt(pstMsgReportInd);
    }
    else
    {
        TAF_SPM_ProcMoSmsRpt(pstMsgReportInd);
    }
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


