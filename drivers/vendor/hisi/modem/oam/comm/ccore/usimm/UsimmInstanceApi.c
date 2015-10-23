

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint --e{767,718,746,537,958}*/

#include "UsimmApi.h"
#include "usimmbase.h"
#include "siapppb.h"
#include "siappstk.h"
#include "siapppih.h"
#include "NasNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


#define    THIS_FILE_ID        PS_FILE_ID_USIMM_INSTANCE_API_C



#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
extern VOS_UINT32 I1_USIMM_SetFileReq(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara,USIMM_SET_FILE_INFO_STRU *pstSetFileInfo);

extern VOS_UINT32 I1_USIMM_GetFileReq(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara, USIMM_GET_FILE_INFO_STRU *pstGetFileInfo);

extern VOS_UINT32 I1_USIMM_AuthReq(VOS_UINT32 ulSenderPid, USIMM_APP_TYPE_ENUM_UINT32 enAppType, USIMM_AUTH_DATA_STRU *pstAuth);

extern VOS_UINT32 I1_USIMM_PinReq(VOS_UINT32 ulSenderPid, VOS_UINT8 ucCmdType, VOS_UINT8 ucPINType, VOS_UINT8 *pucOldPIN, VOS_UINT8 *pucNewPIN);

extern VOS_UINT32 I1_USIMM_MaxEFRecordNumReq(VOS_UINT32 ulSenderPid, USIMM_APP_TYPE_ENUM_UINT32 enAppType, VOS_UINT16 usEFid);

extern VOS_UINT32 I1_USIMM_IsServiceAvailable(UICC_SERVICES_TYPE_ENUM_UINT32 enService);

extern VOS_BOOL I1_USIMM_IsTestCard(VOS_VOID);

extern VOS_UINT32 I1_USIMM_GetPinStatus(VOS_UINT8 *pucType,VOS_UINT8 * pucBeEnable,VOS_UINT8 * pucBeNeed,VOS_UINT8 * pucNum);

extern VOS_UINT32 I1_USIMM_GetCardType(VOS_UINT8 *pucCardStatus, VOS_UINT8 *pucCardType);

extern VOS_UINT32 I1_USIMM_GetCardIMSI(VOS_UINT8 *pucImsi);

extern VOS_UINT32 I1_USIMM_GetPinRemainTime(USIMM_PIN_REMAIN_STRU *pstRemainTime);

extern VOS_VOID I1_USIMM_BdnQuery(VOS_UINT32 *pulState);

extern VOS_VOID I1_USIMM_FdnQuery(VOS_UINT32 *pulState);

extern VOS_UINT32 I1_USIMM_GetCachedFile(VOS_UINT16 usFileID, VOS_UINT32 *pulDataLen, VOS_UINT8 **ppucData, USIMM_APP_TYPE_ENUM_UINT32 enAppType);

extern VOS_UINT32 I1_PIH_RegUsimCardStatusIndMsg(VOS_UINT32 ulRegPID);

extern VOS_UINT32 I1_PIH_DeregUsimCardStatusIndMsg(VOS_UINT32 ulRegPID);

extern VOS_UINT32 I1_PIH_RegCardRefreshIndMsg(VOS_UINT32 ulRegPID);

extern VOS_UINT32 I1_PIH_DeregCardRefreshIndMsg(VOS_UINT32 ulRegPID);

extern VOS_UINT32 I1_SI_PB_FdnNumCheck(VOS_UINT32 ulPid, VOS_UINT32 ulContextIndex, VOS_UINT32 ulClientId, PS_PB_FDN_NUM_STRU *pstFdnInfo);

extern SI_UINT32 I1_SI_PB_GetEccNumber(SI_PB_ECC_DATA_STRU *pstEccData);

extern VOS_UINT32 I1_SI_STK_EnvelopeRsp_Decode(SI_STK_ENVELOPE_TYPE enDataType,VOS_UINT32 ulDataLen,VOS_UINT8 *pucCmdData,SI_STK_ENVELOPE_RSP_STRU *pstRspData);

extern VOS_VOID I1_SI_STK_EnvelopeRspDataFree(SI_STK_ENVELOPE_RSP_STRU *pstData);

extern SI_UINT32 I1_SI_STK_EnvelopeDownload(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara, SI_STK_ENVELOPE_STRU* pstENStru);

extern VOS_VOID I1_SI_STK_CCResultInd(SI_STK_ENVELOPE_RSP_STRU    *pstRspData);

extern VOS_VOID I1_SI_STK_SMSCtrlResultInd(SI_STK_ENVELOPE_RSP_STRU *pstRspData);

extern VOS_UINT32 I1_USIMM_VsimIsActive(VOS_VOID);

#endif
VOS_UINT32 USIMM_IsSvlte(VOS_VOID)
{
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    SVLTE_SUPPORT_FLAG_STRU             stSvlteFlag;

    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_SVLTE_FLAG, &stSvlteFlag, sizeof(SVLTE_SUPPORT_FLAG_STRU)))
    {
        USIMM_ERROR_LOG("USIMM_IsSvlte: Read NV en_NV_Item_SVLTE_FLAG Fail");

        return VOS_FALSE;
    }

    return (VOS_UINT32)stSvlteFlag.ucSvlteSupportFlag;

#else

    return VOS_FALSE;

#endif
}


VOS_UINT32 USIMM_VsimIsActive_Instance(MODEM_ID_ENUM_UINT16 enModemID)
{
#if (FEATURE_VSIM == FEATURE_ON)
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_VsimIsActive_Instance: call USIMM_VsimIsActive");

        return USIMM_VsimIsActive();
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_VsimIsActive_Instance: call I1_USIMM_VsimIsActive");

        return I1_USIMM_VsimIsActive();
    }
#endif
#endif
    USIMM_ERROR_LOG("USIMM_VsimIsActive_Instance: Para is Error");

    return VOS_FALSE;
}


VOS_UINT32 USIMM_SetFileReq_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulSendPara,
    USIMM_SET_FILE_INFO_STRU           *pstSetFileInfo)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_SetFileReq_Instance: call USIMM_SetFileReq");

        return USIMM_SetFileReq(ulSenderPid, ulSendPara, pstSetFileInfo);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_SetFileReq_Instance: call I1_USIMM_SetFileReq");

        return I1_USIMM_SetFileReq(ulSenderPid, ulSendPara, pstSetFileInfo);
    }
#endif

    USIMM_ERROR_LOG("USIMM_SetFileReq_Instance: Para is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_GetFileReq_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulSendPara,
    USIMM_GET_FILE_INFO_STRU           *pstGetFileInfo)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetFileReq_Instance: call USIMM_GetFileReq");

        return USIMM_GetFileReq(ulSenderPid, ulSendPara, pstGetFileInfo);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetFileReq_Instance: call I1_USIMM_GetFileReq");

        return I1_USIMM_GetFileReq(ulSenderPid, ulSendPara, pstGetFileInfo);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetFileReq_Instance: Para is Error");

    return USIMM_API_WRONG_PARA;
}



VOS_UINT32 USIMM_AuthReq_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    USIMM_APP_TYPE_ENUM_UINT32          enAppType,
    USIMM_AUTH_DATA_STRU               *pstAuth)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_AuthReq_Instance: call USIMM_AuthReq");

        return USIMM_AuthReq(ulSenderPid, enAppType, pstAuth);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        return I1_USIMM_AuthReq(ulSenderPid, enAppType, pstAuth);
    }
#endif

    USIMM_ERROR_LOG("USIMM_AuthReq_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_PinReq_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    VOS_UINT8                           ucCmdType,
    VOS_UINT8                           ucPINType,
    VOS_UINT8                          *pucOldPIN,
    VOS_UINT8                          *pucNewPIN)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_PinReq_Instance: call USIMM_PinReq");

        return USIMM_PinReq(ulSenderPid, ucCmdType, ucPINType, pucOldPIN, pucNewPIN);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_PinReq_Instance: call I1_USIMM_PinReq");

        return I1_USIMM_PinReq(ulSenderPid, ucCmdType, ucPINType, pucOldPIN, pucNewPIN);
    }
#endif

    USIMM_ERROR_LOG("USIMM_PinReq_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}



VOS_UINT32 USIMM_MaxEFRecordNumReq_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    USIMM_APP_TYPE_ENUM_UINT32          enAppType,
    VOS_UINT16                          usEFid)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_MaxEFRecordNumReq_Instance: call USIMM_MaxEFRecordNumReq");

        return USIMM_MaxEFRecordNumReq(ulSenderPid, enAppType, usEFid);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_MaxEFRecordNumReq_Instance: call USIMM_MaxEFRecordNumReq");

        return I1_USIMM_MaxEFRecordNumReq(ulSenderPid, enAppType, usEFid);
    }
#endif

    USIMM_ERROR_LOG("USIMM_PinReq_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_IsServiceAvailable_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    UICC_SERVICES_TYPE_ENUM_UINT32      enService)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_IsServiceAvailable_Instance: call USIMM_IsServiceAvailable");

        return USIMM_IsServiceAvailable(enService);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_IsServiceAvailable_Instance: call I1_USIMM_IsServiceAvailable");

        return I1_USIMM_IsServiceAvailable(enService);
    }
#endif

    USIMM_ERROR_LOG("USIMM_IsServiceAvailable_Instance: Para Is Error");

    return PS_USIM_SERVICE_NOT_AVAILIABLE;
}


VOS_BOOL USIMM_IsTestCard_Instance(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_IsTestCard_Instance: call USIMM_IsTestCard");

        return USIMM_IsTestCard();
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_IsTestCard_Instance: call I1_USIMM_IsTestCard");

        return I1_USIMM_IsTestCard();
    }
#endif

    USIMM_ERROR_LOG("USIMM_IsTestCard_Instance: Para Is Error");

    return VOS_FALSE;
}


VOS_UINT32 USIMM_GetPinStatus_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT8                          *pucType,
    VOS_UINT8                          *pucBeEnable,
    VOS_UINT8                          *pucBeNeed,
    VOS_UINT8                          *pucNum)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetPinStatus_Instance: call USIMM_GetPinStatus");

        return USIMM_GetPinStatus(pucType, pucBeEnable, pucBeNeed, pucNum);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetPinStatus_Instance: call I1_USIMM_GetPinStatus");

        return I1_USIMM_GetPinStatus(pucType, pucBeEnable, pucBeNeed, pucNum);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetPinStatus_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_GetCardType_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT8                          *pucCardStatus,
    VOS_UINT8                          *pucCardType)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCardType_Instance: call USIMM_GetCardType");

        return USIMM_GetCardType(pucCardStatus, pucCardType);
    }

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCardType_Instance: call I1_USIMM_GetCardType");

        return I1_USIMM_GetCardType(pucCardStatus, pucCardType);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetCardType_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_GetCachedFile_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT16                          usFileID,
    VOS_UINT32                         *pulDataLen,
    VOS_UINT8                         **ppucData,
    USIMM_APP_TYPE_ENUM_UINT32          enAppType)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCachedFile_Instance: call USIMM_GetCachedFile");

        return USIMM_GetCachedFile(usFileID, pulDataLen, ppucData, enAppType);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCachedFile_Instance: call I1_USIMM_GetCachedFile");

        return I1_USIMM_GetCachedFile(usFileID, pulDataLen, ppucData, enAppType);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetCachedFile_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_GetCardIMSI_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT8                          *pucImsi)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCardIMSI_Instance: call USIMM_GetCardIMSI");

        return USIMM_GetCardIMSI(pucImsi);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetCardIMSI_Instance: call I1_USIMM_GetCardIMSI");

        return I1_USIMM_GetCardIMSI(pucImsi);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetCardIMSI_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_UINT32 USIMM_GetPinRemainTime_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    USIMM_PIN_REMAIN_STRU              *pstRemainTime)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetPinRemainTime_Instance: call USIMM_GetPinRemainTime");

        return USIMM_GetPinRemainTime(pstRemainTime);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_GetPinRemainTime_Instance: call I1_USIMM_GetPinRemainTime");

        return I1_USIMM_GetPinRemainTime(pstRemainTime);
    }
#endif

    USIMM_ERROR_LOG("USIMM_GetPinRemainTime_Instance: Para Is Error");

    return USIMM_API_WRONG_PARA;
}


VOS_VOID USIMM_BdnQuery_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                         *pulState)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_BdnQuery_Instance: call USIMM_BdnQuery");

        USIMM_BdnQuery(pulState);

        return;
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_BdnQuery_Instance: call I1_USIMM_BdnQuery");

        I1_USIMM_BdnQuery(pulState);

        return;
    }
#endif

    USIMM_ERROR_LOG("USIMM_BdnQuery_Instance: Para Is Error");

    *pulState = USIMM_BDNSTATUS_BUTT;

    return;
}


VOS_VOID USIMM_FdnQuery_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                         *pulState)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_FdnQuery_Instance: call USIMM_FdnQuery");

        USIMM_FdnQuery(pulState);

        return;
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("USIMM_FdnQuery_Instance: call I1_USIMM_FdnQuery");

        I1_USIMM_FdnQuery(pulState);

        return;
    }
#endif

    USIMM_ERROR_LOG("USIMM_FdnQuery_Instance: Para Is Error");

    *pulState = USIMM_FDNSTATUS_BUTT;

    return;
}


VOS_UINT32 PIH_RegUsimCardStatusIndMsg_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulRegPID)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_RegUsimCardStatusIndMsg_Instance: call PIH_RegUsimCardStatusIndMsg");

        return PIH_RegUsimCardStatusIndMsg(ulRegPID);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_RegUsimCardStatusIndMsg_Instance: call I1_PIH_RegUsimCardStatusIndMsg");

        return I1_PIH_RegUsimCardStatusIndMsg(ulRegPID);
    }
#endif

    USIMM_ERROR_LOG("PIH_RegUsimCardStatusIndMsg_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 PIH_DeregUsimCardStatusIndMsg_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulRegPID)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_DeregUsimCardStatusIndMsg_Instance: call PIH_DeregUsimCardStatusIndMsg");

        return PIH_DeregUsimCardStatusIndMsg(ulRegPID);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_DeregUsimCardStatusIndMsg_Instance: call I1_PIH_DeregUsimCardStatusIndMsg");

        return I1_PIH_DeregUsimCardStatusIndMsg(ulRegPID);
    }
#endif

    USIMM_ERROR_LOG("PIH_DeregUsimCardStatusIndMsg_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 PIH_RegCardRefreshIndMsg_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulRegPID)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_RegCardRefreshIndMsg_Instance: call PIH_RegCardRefreshIndMsg");

        return PIH_RegCardRefreshIndMsg(ulRegPID);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_RegCardRefreshIndMsg_Instance: call I1_PIH_RegCardRefreshIndMsg");

        return I1_PIH_RegCardRefreshIndMsg(ulRegPID);
    }
#endif

    USIMM_ERROR_LOG("PIH_RegCardRefreshIndMsg_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 PIH_DeregCardRefreshIndMsg_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulRegPID)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_DeregCardRefreshIndMsg_Instance: call PIH_RegCardRefreshIndMsg");

        return PIH_DeregCardRefreshIndMsg(ulRegPID);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("PIH_DeregCardRefreshIndMsg_Instance: call PIH_RegCardRefreshIndMsg");

        return I1_PIH_DeregCardRefreshIndMsg(ulRegPID);
    }
#endif

    USIMM_ERROR_LOG("PIH_DeregCardRefreshIndMsg_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 SI_PB_FdnNumCheck_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulPid,
    VOS_UINT32                          ulContextIndex,
    VOS_UINT32                          ulSendPara,
    PS_PB_FDN_NUM_STRU                 *pstFdnInfo)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_PB_FdnNumCheck_Instance: call SI_PB_FdnNumCheck");

        return SI_PB_FdnNumCheck(ulPid, ulContextIndex, ulSendPara, pstFdnInfo);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_PB_FdnNumCheck_Instance: call I1_SI_PB_FdnNumCheck");

        return I1_SI_PB_FdnNumCheck(ulPid, ulContextIndex, ulSendPara, pstFdnInfo);
    }
#endif

    USIMM_ERROR_LOG("SI_PB_FdnNumCheck_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetEccNumber_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    SI_PB_ECC_DATA_STRU                *pstEccData)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_PB_GetEccNumber_Instance: call SI_PB_GetEccNumber");

        return SI_PB_GetEccNumber(pstEccData);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_PB_GetEccNumber_Instance: call I1_SI_PB_GetEccNumber");

        return I1_SI_PB_GetEccNumber(pstEccData);
    }
#endif

    USIMM_ERROR_LOG("SI_PB_GetEccNumber_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_UINT32 SI_STK_EnvelopeRsp_Decode_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    SI_STK_ENVELOPE_TYPE                enDataType,
    VOS_UINT32                          ulDataLen,
    VOS_UINT8                          *pucCmdData,
    SI_STK_ENVELOPE_RSP_STRU           *pstRspData)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeRsp_Decode_Instance: call SI_STK_EnvelopeRsp_Decode");

        return SI_STK_EnvelopeRsp_Decode(enDataType, ulDataLen, pucCmdData, pstRspData);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeRsp_Decode_Instance: call I1_SI_STK_EnvelopeRsp_Decode");

        return I1_SI_STK_EnvelopeRsp_Decode(enDataType, ulDataLen, pucCmdData, pstRspData);
    }
#endif

    USIMM_ERROR_LOG("SI_STK_EnvelopeRsp_Decode_Instance: Para Is Error");

    return VOS_ERR;
}


VOS_VOID SI_STK_EnvelopeRspDataFree_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    SI_STK_ENVELOPE_RSP_STRU           *pstData)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeRspDataFree_Instance: call SI_STK_EnvelopeRspDataFree");

        SI_STK_EnvelopeRspDataFree(pstData);

        return;
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeRspDataFree_Instance: call I1_SI_STK_EnvelopeRspDataFree");

        I1_SI_STK_EnvelopeRspDataFree(pstData);

        return;
    }
#endif

    USIMM_ERROR_LOG("SI_STK_EnvelopeRsp_Decode_Instance: Para Is Error");

    return;
}


VOS_UINT32 SI_STK_EnvelopeDownload_Instance(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulSendPara,
    SI_STK_ENVELOPE_STRU               *pstENStru)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeDownload_Instance: call SI_STK_EnvelopeDownload");

        return SI_STK_EnvelopeDownload(ulSenderPid, ulSendPara, pstENStru);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_EnvelopeDownload_Instance: call I1_SI_STK_EnvelopeDownload");

        return I1_SI_STK_EnvelopeDownload(ulSenderPid, ulSendPara, pstENStru);
    }
#endif

    USIMM_ERROR_LOG("SI_STK_EnvelopeDownload_Instance: Para Is Error");

    return VOS_ERR;
}

/*****************************************************************************
函 数 名  : SI_STK_CCResultInd_Instance
功能描述  : 将MO CALL CONTROL的结果广播上报给AT
输入参数  : enModemID --Modem ID
            pstRspData--指向CALL CONTROL的解码后码流
输出参数  : 无
返 回 值  : 无
History     :
1.日    期  : 2013年06月14日
  作    者  : zhuli
  修改内容  : Create
*****************************************************************************/
VOS_VOID SI_STK_CCResultInd_Instance(
    MODEM_ID_ENUM_UINT16                                    enModemID,
    SI_STK_ENVELOPE_RSP_STRU                                *pstRspData)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_CCResultInd_Instance: call SI_STK_CCResultInd");

        SI_STK_CCResultInd(pstRspData);

        return;
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_CCResultInd_Instance: call I1_SI_STK_CCResultInd");

        I1_SI_STK_CCResultInd(pstRspData);

        return;
    }
#endif

    USIMM_ERROR_LOG("SI_STK_CCResultInd_Instance: Para Is Error");

    return;
}

/*****************************************************************************
函 数 名  : SI_STK_SMSCtrlResultInd_Instance
功能描述  : 将MO SMS CONTROL的结果广播上报给AT
输入参数  : enModemID --Modem ID
            pstRspData--指向SMS CONTROL的解码后码流
输出参数  : 无
返 回 值  : 执行结果
History     :
1.日    期  : 2013年06月14日
  作    者  : zhuli
  修改内容  : Create
*****************************************************************************/
VOS_VOID SI_STK_SMSCtrlResultInd_Instance(
    MODEM_ID_ENUM_UINT16                                    enModemID,
    SI_STK_ENVELOPE_RSP_STRU                                *pstRspData)
{
    if (MODEM_ID_0 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_SMSCtrlResultInd_Instance: call SI_STK_SMSCtrlResultInd");

        SI_STK_SMSCtrlResultInd(pstRspData);

        return;
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    if (MODEM_ID_1 == enModemID)
    {
        USIMM_NORMAL_LOG("SI_STK_SMSCtrlResultInd_Instance: call I1_SI_STK_SMSCtrlResultInd");

        I1_SI_STK_SMSCtrlResultInd(pstRspData);

        return;
    }
#endif

    USIMM_ERROR_LOG("SI_STK_SMSCtrlResultInd_Instance: Para Is Error");

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




