


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "Taf_Tafm_Remote.h"
#include "PsCommonDef.h"
#include "NVIM_Interface.h"
#include "TafClientApi.h"

/* #include "MnComm.h" */
#include "MnCommApi.h"
#include "TafAppMma.h"


#ifdef  __cplusplus
 #if  __cplusplus
extern "C" {
 #endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define    THIS_FILE_ID PS_FILE_ID_TAF_MMA_API_C

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    extern VOS_UINT32 AT_GetDestPid(
        MN_CLIENT_ID_T                      usClientId,
        VOS_UINT32                          ulRcvPid
    );
#endif

/*****************************************************************************
   3 函数实现
*****************************************************************************/



TAF_UINT32  Taf_DefPhClassType ( MN_CLIENT_ID_T       ClientId,
                                 MN_OPERATION_ID_T    OpId,
                                 TAF_PH_MS_CLASS_TYPE MsClass)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_MS_CLASS,
                                   &MsClass,
                                   sizeof(TAF_PH_MS_CLASS_TYPE),
                                   I0_WUEPS_PID_MMA);
}


VOS_UINT32 Taf_DefPhMode(MN_CLIENT_ID_T   ClientId,
                         MN_OPERATION_ID_T   OpId,
                         TAF_PH_OP_MODE_STRU PhModeSet)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_FUN_QUERY,
                                   &PhModeSet,
                                   sizeof(TAF_PH_OP_MODE_STRU),
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 Taf_PhonePinHandle ( MN_CLIENT_ID_T          ClientId,
                                MN_OPERATION_ID_T       OpId,
                                TAF_PH_PIN_DATA_STRU   *pPinData)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_OP_PIN_REQ,
                                   pPinData,
                                   sizeof(TAF_PH_PIN_DATA_STRU),
                                   I0_WUEPS_PID_MMA);
}



VOS_UINT32 Taf_MePersonalisationHandle(MN_CLIENT_ID_T          ClientId,
                                       MN_OPERATION_ID_T                 OpId,
                                       TAF_ME_PERSONALISATION_DATA_STRU *pMePersonalData)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_ME_PERSONAL_REQ,
                                   pMePersonalData,
                                   sizeof(TAF_ME_PERSONALISATION_DATA_STRU),
                                   I0_WUEPS_PID_MMA);
}

/**********************************************************/
/*                      电话管理API                       */
/**********************************************************/

TAF_UINT32 Taf_PhonePlmnResel ( MN_CLIENT_ID_T           ClientId,
                                MN_OPERATION_ID_T        OpId,
                                TAF_PLMN_RESEL_MODE_TYPE ReselType)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_PLMN_RESEL,
                                   &ReselType,
                                   sizeof(TAF_PLMN_RESEL_MODE_TYPE),
                                   I0_WUEPS_PID_MMA);
}

VOS_UINT32 TAF_PhonePlmnUserSel(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PLMN_USER_SEL_STRU             *pstPlmnUserSel
)
{
    VOS_UINT32                           ulRst;


    /* 发送消息 TAF_MSG_MMA_PLMN_USER_SEL 给 MMA 处理，
       该消息带TAF_PLMN_USER_SEL_STRU类型的参数结构 */
    ulRst = MN_FillAndSndAppReqMsg(ClientId,
                                   OpId,
                                   TAF_MSG_MMA_PLMN_USER_SEL,
                                   (VOS_UINT8*)pstPlmnUserSel, /*要求查询PS的注册状态。*/
                                   sizeof(TAF_PLMN_USER_SEL_STRU),
                                   I0_WUEPS_PID_MMA);



    if (TAF_SUCCESS == ulRst )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;

}
TAF_UINT32 Taf_PhoneAttach ( MN_CLIENT_ID_T     ClientId,
                             MN_OPERATION_ID_T  OpId,
                             TAF_PH_ATTACH_TYPE AttachType)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_ATTACH,
                                   &AttachType,
                                   sizeof(TAF_PH_ATTACH_TYPE),
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 Taf_PhonePlmnList ( MN_CLIENT_ID_T           ClientId,
                               MN_OPERATION_ID_T        OpId,
                               TAF_AVAIL_PLMN_LIST_TYPE ListType)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_PLMN_LIST,
                                   &ListType,
                                   sizeof(TAF_AVAIL_PLMN_LIST_TYPE),
                                   I0_WUEPS_PID_MMA);
}



VOS_UINT32 TAF_AbortPlmnListReq (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_PLMN_LIST_ABORT_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_MMA );
}





TAF_UINT32 Taf_PhoneStartInfoInd ( MN_CLIENT_ID_T          ClientId,
                                   MN_OPERATION_ID_T       OpId,
                                   TAF_START_INFO_IND_STRU StartInfoInd)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_INFO_REPORT,
                                   &StartInfoInd,
                                   sizeof(TAF_START_INFO_IND_STRU),
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 TAF_DefQuickStart(MN_CLIENT_ID_T ClientId, MN_OPERATION_ID_T OpId, TAF_UINT32 ulSetValue)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_SET_QUICKSTART,
                                   &ulSetValue,
                                   sizeof(TAF_UINT32),
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 Taf_DefPhAutoAttach(MN_CLIENT_ID_T ClientId, MN_OPERATION_ID_T OpId, TAF_UINT32 ulSetValue)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_SET_AUTOATTACH,
                                   &ulSetValue,
                                   sizeof(TAF_UINT32),
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 TAF_DefLoadDefault(MN_CLIENT_ID_T ClientId, MN_OPERATION_ID_T OpId)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_SET_LOADDEFAULT,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_MMA);
}


TAF_UINT32 Taf_UsimRestrictedAccessCommand(MN_CLIENT_ID_T               ClientId,
                                           MN_OPERATION_ID_T            OpId,
                                           TAF_SIM_RESTRIC_ACCESS_STRU *pPara)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_USIM_RESTRICTED_ACCESS,
                                   pPara,
                                   sizeof(TAF_SIM_RESTRIC_ACCESS_STRU),
                                   I0_WUEPS_PID_MMA);
}




VOS_UINT32 Taf_IndPhFreq(MN_CLIENT_ID_T     ClientId,
                         MN_OPERATION_ID_T        OpId,
                         TAF_IND_FREQ_STRU        Freq)
{
    VOS_UINT16 temp_Freq;
    VOS_UINT16 temp_value;

    switch (Freq.RatType)
    {
    case TAF_SYS_SUBMODE_GSM:
        temp_value   = Freq.GsmBand;
        temp_value <<= 12;
        temp_Freq   = temp_value;
        temp_value  = Freq.IndFreq;
        temp_value &= 0x0fff;
        temp_Freq |= temp_value;

        /* write temp_GSM_Freq to NVIM */
        if (NV_OK != NV_Write( en_NV_Item_Gsm_Ind_Freq, &temp_Freq, sizeof(VOS_UINT16)))
        {
            return TAF_FAILURE;
        }

        break;

    case TAF_SYS_SUBMODE_WCDMA:

        /*write Freq to NVIM */
        temp_Freq = Freq.IndFreq;
        if (NV_OK != NV_Write( en_NV_Item_Wcdma_Ind_Freq, &temp_Freq, sizeof(VOS_UINT16)))
        {
            return TAF_FAILURE;
        }

        break;

    default:
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


TAF_UINT32 Taf_FplmnHandle(MN_CLIENT_ID_T            ClientId,
                           MN_OPERATION_ID_T         OpId,
                           TAF_PH_FPLMN_OPERATE_STRU stFplmnOperate)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_FPLMN_HANDLE,
                                   &stFplmnOperate,
                                   sizeof(TAF_PH_FPLMN_OPERATE_STRU),
                                   I0_WUEPS_PID_MMA);
}



VOS_UINT32 Taf_SysCfgHandle(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfg
)
{

    return MN_FillAndSndAppReqMsg(ClientId,
                                  OpId,
                                  TAF_MSG_MMA_SYSCFG_QUERY_REQ,
                                  &stSysCfg,
                                  sizeof(stSysCfg),
                                  I0_WUEPS_PID_MMA);
}




VOS_UINT32 Taf_NetScanHandle(
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScan
)
{
    return MN_FillAndSndAppReqMsg(usClientId,
                                  ucOpId,
                                  TAF_MSG_MMA_NET_SCAN_REQ,
                                  pstNetScan,
                                  sizeof(TAF_MMA_NET_SCAN_REQ_STRU),
                                  I0_WUEPS_PID_MMA);
}



VOS_UINT32 Taf_AbortNetScan (
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    return MN_FillAndSndAppReqMsg( usClientId,
                                   ucOpId,
                                   TAF_MSG_MMA_ABORT_NET_SCAN_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_MMA );
}



TAF_UINT32 TAF_QryUsimInfo(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_QRY_USIM_INFO_STRU           *pstInfo
)
{
    if ( (TAF_PH_ICC_UNKNOW == pstInfo->Icctype)
      || (pstInfo->Icctype > TAF_PH_ICC_USIM))
    {
        return TAF_FAILURE;
    }

    if (pstInfo->enEfId > TAF_PH_OPL_FILE)
    {
        return TAF_FAILURE;
    }
    return MN_FillAndSndAppReqMsg( ClientId,
                             OpId,
                             TAF_MSG_MMA_USIM_INFO,
                             pstInfo,
                             sizeof(TAF_PH_QRY_USIM_INFO_STRU),
                             I0_WUEPS_PID_MMA);

}


TAF_UINT32 TAF_QryCpnnInfo(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_ICC_TYPE                     IccType
)
{
    if ( (TAF_PH_ICC_UNKNOW == IccType)
      || (IccType > TAF_PH_ICC_USIM))
    {
        return TAF_FAILURE;
    }

    return MN_FillAndSndAppReqMsg( ClientId,
                             OpId,
                             TAF_MSG_MMA_CPNN_INFO,
                             &IccType,
                             sizeof(IccType),
                             I0_WUEPS_PID_MMA);
}


VOS_UINT32 Taf_SetEOPlmnHandle(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnCfg
)
{

    return MN_FillAndSndAppReqMsg(ClientId,
                                  OpId,
                                  TAF_MSG_MMA_EOPLMN_SET_REQ,
                                  pstEOPlmnCfg,
                                  sizeof(TAF_MMA_SET_EOPLMN_LIST_STRU),
                                  I0_WUEPS_PID_MMA);
}


VOS_UINT32 TAF_MMA_PhoneModeSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_PHONE_MODE_PARA_STRU       *pstPhoneModePara
)
{
    TAF_MMA_PHONE_MODE_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstPhoneModePara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_PHONE_MODE_SET_REQ_STRU */
    pstMsg = (TAF_MMA_PHONE_MODE_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PHONE_MODE_SET_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_PHONE_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PHONE_MODE_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    PS_MEM_CPY(&(pstMsg->stPhoneModePara), pstPhoneModePara, sizeof(TAF_MMA_PHONE_MODE_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_SetSysCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    TAF_MMA_SYS_CFG_REQ_STRU           *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstSysCfgPara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_SYS_CFG_REQ_STRU */
    pstMsg = (TAF_MMA_SYS_CFG_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_SYS_CFG_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 根据输入参数填充TAF_MMA_SYS_CFG_REQ_STRU */
    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_SYS_CFG_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    PS_MEM_CPY(&(pstMsg->stSysCfgPara), pstSysCfgPara, sizeof(TAF_MMA_SYS_CFG_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_AcqBestNetworkReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_ACQ_PARA_STRU              *pstAcqPara
)
{
    TAF_MMA_ACQ_REQ_STRU               *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstAcqPara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_ACQ_REQ_STRU */
    pstMsg = (TAF_MMA_ACQ_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_ACQ_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_ACQ_BEST_NETWORK_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    PS_MEM_CPY(&(pstMsg->stAcqPara), pstAcqPara, sizeof(TAF_MMA_ACQ_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RegReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_REG_PARA_STRU              *pstRegPara
)
{
    TAF_MMA_REG_REQ_STRU               *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstRegPara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_REG_REQ_STRU */
    pstMsg = (TAF_MMA_REG_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_REG_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 根据输入参数填充TAF_MMA_REG_REQ_STRU */
    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_REG_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    PS_MEM_CPY(&(pstMsg->stRegPara), pstRegPara, sizeof(TAF_MMA_REG_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_PowerSaveReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_POWER_SAVE_PARA_STRU       *pstPowerSavePara
)
{

    TAF_MMA_POWER_SAVE_REQ_STRU        *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstPowerSavePara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_POWER_SAVE_REQ_STRU */
    pstMsg = (TAF_MMA_POWER_SAVE_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_POWER_SAVE_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_POWER_SAVE_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    PS_MEM_CPY(&(pstMsg->stPowerSavePara), pstPowerSavePara, sizeof(TAF_MMA_POWER_SAVE_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_DetachReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_DETACH_PARA_STRU           *pstDetachPara
)
{
    TAF_MMA_DETACH_REQ_STRU            *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
#else
    ulReceiverPid = WUEPS_PID_MMA;
    ulSenderPid   = WUEPS_PID_TAF;
#endif

    /* 参数检查 */
    if (VOS_NULL_PTR == pstDetachPara)
    {
        return VOS_FALSE;
    }

    /* 申请消息包TAF_MMA_DETACH_REQ_STRU */
    pstMsg = (TAF_MMA_DETACH_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_DETACH_REQ_STRU));

    /* 内存申请失败，返回 */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(TAF_MMA_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 根据输入参数填充TAF_MMA_DETACH_REQ_STRU */
    /* 发送PID统一填写为WUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_DETACH_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    PS_MEM_CPY(&(pstMsg->stDetachPara), pstDetachPara, sizeof(TAF_MMA_DETACH_PARA_STRU));

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


#ifdef  __cplusplus
 #if  __cplusplus
}
 #endif
#endif
