
 
/*lint --e{740,958,767,537,718,746}*/

#include "UsimmApi.h"
#include "UsimPsInterface.h"
#include "usimmbase.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_USIMM_APICNF_C



extern VOS_MSG_HOOK_FUNC    vos_MsgHook;


VOS_VOID USIMM_MaxRecordNumCnf(VOS_UINT32                ulReceiverPid,
                                            VOS_UINT32              ulErrorCode,
                                           USIMM_FILECNF_INFO_STRU  *pstCnfInfo)
{
    PS_USIM_EFMAX_CNF_STRU *pUsimMsg;

    pUsimMsg = (PS_USIM_EFMAX_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM,
                     sizeof(PS_USIM_EFMAX_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_MaxRecordNumCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid        = ulReceiverPid;
    pUsimMsg->ulErrorCode          = ulErrorCode;
    pUsimMsg->ulMsgName            = PS_USIM_EF_MAX_RECORD_NUM_CNF;
    pUsimMsg->usEfId               = pstCnfInfo->usEFId;
    pUsimMsg->ucRecordNum          = pstCnfInfo->ucRecordNum;
    pUsimMsg->ucRecordLen          = pstCnfInfo->ucRecordLen;
    pUsimMsg->ulFileStatus         = pstCnfInfo->ulFileStatus;
    pUsimMsg->ulFileReadUpdateFlag = pstCnfInfo->ulFileReadUpdateFlag;

    if(ulErrorCode != USIMM_SW_OK)
    {
        pUsimMsg->ulResult  = VOS_ERR;
    }
    else
    {
        pUsimMsg->ulResult  = VOS_OK;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_PinHandleCnf(VOS_UINT32                      ulReceiverPid,
                                    USIMM_PIN_CMD_TYPE_ENUM_UINT32  enCmdType,
                                    USIMM_PIN_TYPE_ENUM_UINT32      enPINType,
                                    VOS_UINT32                      ulResult,
                                    USIMM_PIN_INFO_STRU             *pstPINinfo)
{
    PS_USIM_PIN_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_PIN_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_PIN_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_PinHandleCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_PIN_OPERATE_RSP;
    pUsimMsg->ulResult      = ulResult;
    pUsimMsg->enCmdType     = enCmdType;
    pUsimMsg->enPinType     = enPINType;

    VOS_MemCpy(&pUsimMsg->stPinInfo, pstPINinfo, sizeof(USIMM_PIN_INFO_STRU));

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_OmFileCnf(VOS_UINT32 ulResult,VOS_UINT16 usEfLen,VOS_UINT8  *pucEf,VOS_UINT8 ucCmdType)
{
    PS_USIM_OM_TRANS_IND_STRU *pstTransMsg;

    pstTransMsg = (PS_USIM_OM_TRANS_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, (sizeof(PS_USIM_OM_TRANS_IND_STRU) - VOS_MSG_HEAD_LENGTH) + usEfLen);

    if (VOS_NULL_PTR == pstTransMsg)
    {
        USIMM_ERROR_LOG("USIMM_OmFileCnf: VOS_AllocMsg for Om is Failed");

        return;
    }

    pstTransMsg->ulReceiverPid  = WUEPS_PID_OM;
    pstTransMsg->usTransPrimId  = USIMM_TRANS_PRIMID;             /*固定填写0x5001*/
    pstTransMsg->ucFuncType     = 4;                  /*对于透明消息，功能类型固定填写4*/
    pstTransMsg->usAppLength    = 14 + usEfLen; /*14为透明消息结构体usAppLength之后的长度*/
    pstTransMsg->ucResult       = (VOS_UINT8)ulResult;
    pstTransMsg->ucEFLen        = (VOS_UINT8)(usEfLen);
    pstTransMsg->ulTimeStamp    = OM_GetSlice();

    if ( USIMM_OM_GET_CNF == ucCmdType )
    {
        pstTransMsg->usPrimId = USIMM_GET_PRIMID;/*OAM的透明消息，读取文件原语0xAA01*/
    }
    else
    {
        pstTransMsg->usPrimId = USIMM_SET_PRIMID;/*OAM的透明消息，更新文件原语0xAA02*/
    }

    if(0 != usEfLen)        /*当前存在回复文件内容*/
    {
        VOS_MemCpy(&pstTransMsg->aucData[0],pucEf,usEfLen);
    }

    (VOS_VOID)VOS_SendMsg(pstTransMsg->ulSenderPid, pstTransMsg);

    return;
}


VOS_VOID USIMM_SetFileCnf(VOS_UINT32               ulReceiverPid,
                                VOS_UINT32              ulSendPara,
                                VOS_UINT32              ulErrorCode,
                                USIMM_SETCNF_INFO_STRU  *pstCnfInfo)
{
    PS_USIM_SET_FILE_CNF_STRU  *pUsimMsg;
    VOS_UINT32                 ulResult;

    if(ulErrorCode != USIMM_SW_OK)
    {
        ulResult = VOS_ERR;
    }
    else
    {
        ulResult = VOS_OK;
    }

    if( WUEPS_PID_OM == ulReceiverPid )
    {
        USIMM_OmFileCnf(ulResult,0,0,USIMM_OM_SET_CNF);/*最后一个参数1表示更新文件*/

        return;
    }

    pUsimMsg = (PS_USIM_SET_FILE_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_SET_FILE_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_SetFileCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulMsgName     = PS_USIM_SET_FILE_RSP;
    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulResult      = ulResult;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->ulSendPara    = ulSendPara;
    pUsimMsg->usEfId        = pstCnfInfo->usEfid;
    pUsimMsg->usEfLen       = pstCnfInfo->usEFLen;
    pUsimMsg->ucRecordNum   = pstCnfInfo->ucRecordNum ;

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}
VOS_VOID USIMM_GetFileCnf(VOS_UINT32              ulReceiverPid,
                                VOS_UINT32              ulSendPara,
                                VOS_UINT32              ulErrorCode,
                                USIMM_GETCNF_INFO_STRU  *pstCnfInfo)
{
    PS_USIM_GET_FILE_CNF_STRU  *pUsimMsg;
    VOS_UINT32                 ulResult;

    if(ulErrorCode != USIMM_SW_OK)
    {
        ulResult = VOS_ERR;
    }
    else
    {
        ulResult = VOS_OK;
    }

    if( WUEPS_PID_OM == ulReceiverPid )
    {
        USIMM_OmFileCnf(ulResult,pstCnfInfo->usEfLen,pstCnfInfo->pucEf,USIMM_OM_GET_CNF);

        return;
    }

    pUsimMsg = (PS_USIM_GET_FILE_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, (sizeof(PS_USIM_GET_FILE_CNF_STRU)-VOS_MSG_HEAD_LENGTH)+pstCnfInfo->usDataLen);

    if(VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_GetFileCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulMsgName     = PS_USIM_GET_FILE_RSP;
    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->ulSendPara    = ulSendPara;
    pUsimMsg->usEfId        = pstCnfInfo->usEfId;

    if(USIMM_SW_OK == ulErrorCode)
    {
        pUsimMsg->ucRecordNum = pstCnfInfo->ucRecordNum;
        pUsimMsg->usEfLen     = pstCnfInfo->usEfLen;
        pUsimMsg->ucTotalNum  = pstCnfInfo->ucTotalNum;
        pUsimMsg->ulResult    = VOS_OK;

        VOS_MemCpy(pUsimMsg->aucEf, pstCnfInfo->pucEf, pstCnfInfo->usDataLen);
    }
    else
    {
        pUsimMsg->ucRecordNum = 0;
        pUsimMsg->usEfLen     = 0;
        pUsimMsg->ucTotalNum  = 0;
        pUsimMsg->ulResult    = VOS_ERR;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_ResetCnf(VOS_UINT32                            ulReceiverPid,
                                USIMM_REFRESH_TYPE_ENUM_UINT32      enRefreshType,
                                VOS_UINT32                          ulResult,
                                VOS_UINT16                          usSATLen)
{
    PS_USIM_REFRESH_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_REFRESH_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_REFRESH_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_ResetCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_REFRESH_CNF;
    pUsimMsg->enRefreshType = enRefreshType;
    pUsimMsg->ulResult      = ulResult;
    pUsimMsg->ulSatLen      = usSATLen;

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_TelecomAuthCnf(USIMM_AUTH_REQ_STRU *pstMsg, USIMM_TELECOM_AUTH_INFO_STRU *pstCnfInfo)
{
    USIMM_AUTH_CNF_STRU                *pstCnfMsg;

    /* 分配消息内存  */
    pstCnfMsg = (USIMM_AUTH_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(USIMM_AUTH_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstCnfMsg)
    {
        USIMM_ERROR_LOG("USIMM_TelecomAuthCnf: VOS_AllocMsg is Failed");

        return;
    }

    VOS_MemSet((VOS_UINT8*)pstCnfMsg + VOS_MSG_HEAD_LENGTH, 0, pstCnfMsg->ulLength);

    pstCnfMsg->ulReceiverPid    = pstMsg->ulSenderPid;
    pstCnfMsg->ulMsgName        = PS_USIM_AUTHENTICATION_CNF;
    pstCnfMsg->enAppType        = pstMsg->enAppType;
    pstCnfMsg->enAuthType       = pstMsg->enAuthType;
    pstCnfMsg->ucOpId           = pstMsg->ucOpId;
    pstCnfMsg->enResult         = pstCnfInfo->enResult;

    if ((VOS_NULL_PTR != pstCnfInfo->pucIK)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucIK) > pstCnfInfo->pucIK[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stTELECnf.aucIK, pstCnfInfo->pucIK, pstCnfInfo->pucIK[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucCK)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucCK) > pstCnfInfo->pucCK[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stTELECnf.aucCK, pstCnfInfo->pucCK, pstCnfInfo->pucCK[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucGsmKC)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucGsmKc) > pstCnfInfo->pucGsmKC[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stTELECnf.aucGsmKc, pstCnfInfo->pucGsmKC, pstCnfInfo->pucGsmKC[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucAuts)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucAuts) > pstCnfInfo->pucAuts[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stTELECnf.aucAuts, pstCnfInfo->pucAuts, pstCnfInfo->pucAuts[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucAuthRes)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucAuthRes) > pstCnfInfo->pucAuthRes[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stTELECnf.aucAuthRes, pstCnfInfo->pucAuthRes, pstCnfInfo->pucAuthRes[0] + 1);
    }

    /* 回复消息 */
    (VOS_VOID)VOS_SendMsg(pstCnfMsg->ulSenderPid, pstCnfMsg);

    return;
}


VOS_VOID USIMM_ImsAuthCnf(USIMM_AUTH_REQ_STRU *pstMsg, USIMM_IMS_AUTH_INFO_STRU *pstCnfInfo)
{
    USIMM_AUTH_CNF_STRU                *pstCnfMsg;
    VOS_UINT32                          ulNafLen = 0;

    /* 获取NAF内容的长度 */
    if (VOS_NULL_PTR != pstCnfInfo->pucKs_ext_NAF)
    {
        ulNafLen = pstCnfInfo->pucKs_ext_NAF[0];
    }

    /* 分配消息内存  */
    pstCnfMsg = (USIMM_AUTH_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(USIMM_AUTH_CNF_STRU)+ulNafLen-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstCnfMsg)
    {
        USIMM_ERROR_LOG("USIMM_ImsAuthCnf: VOS_AllocMsg is Failed");

        return;
    }

    VOS_MemSet((VOS_UINT8*)pstCnfMsg + VOS_MSG_HEAD_LENGTH, 0, pstCnfMsg->ulLength);

    pstCnfMsg->ulReceiverPid    = pstMsg->ulSenderPid;
    pstCnfMsg->ulMsgName        = PS_USIM_AUTHENTICATION_CNF;
    pstCnfMsg->enAppType        = pstMsg->enAppType;
    pstCnfMsg->enAuthType       = pstMsg->enAuthType;
    pstCnfMsg->ucOpId           = pstMsg->ucOpId;
    pstCnfMsg->enResult         = pstCnfInfo->enResult;

    if ((VOS_NULL_PTR != pstCnfInfo->pucIK)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucIK) > pstCnfInfo->pucIK[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stIMSCnf.aucIK, pstCnfInfo->pucIK, pstCnfInfo->pucIK[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucCK)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucCK) > pstCnfInfo->pucCK[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stIMSCnf.aucCK, pstCnfInfo->pucCK, pstCnfInfo->pucCK[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucAuts)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucAuts) > pstCnfInfo->pucAuts[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stIMSCnf.aucAuts, pstCnfInfo->pucAuts, pstCnfInfo->pucAuts[0] + 1);
    }

    if ((VOS_NULL_PTR != pstCnfInfo->pucAutsRes)
        && (sizeof(pstCnfMsg->cnfdata.stTELECnf.aucAuthRes) > pstCnfInfo->pucAutsRes[0]))
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stIMSCnf.aucAuthRes, pstCnfInfo->pucAutsRes, pstCnfInfo->pucAutsRes[0] + 1);
    }

    if (VOS_NULL_PTR != pstCnfInfo->pucKs_ext_NAF)
    {
        VOS_MemCpy(pstCnfMsg->cnfdata.stIMSCnf.aucKs_ext_NAF, pstCnfInfo->pucKs_ext_NAF, pstCnfInfo->pucKs_ext_NAF[0] + 1);
    }

    /* 回复消息 */
    (VOS_VOID)VOS_SendMsg(pstCnfMsg->ulSenderPid, pstCnfMsg);

    return;
}
VOS_VOID USIMM_GenericAccessCnf(VOS_UINT32          ulReceiverPid,
                                        VOS_UINT32          ulResult,
                                        VOS_UINT16          usPathLen,
                                        VOS_UINT16          *pusPath,
                                        USIMM_CSIM_CNF_STRU *pstData)
{
    PS_USIM_GENERIC_ACCESS_CNF_STRU *pUsimMsg = VOS_NULL_PTR;

    pUsimMsg = (PS_USIM_GENERIC_ACCESS_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_GENERIC_ACCESS_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_GenericAccessCnf: VOS_AllocMsg is Failed");

        return;
    }

    if(0 != usPathLen)
    {
        VOS_MemCpy(pUsimMsg->ausPath, pusPath, usPathLen*sizeof(VOS_UINT16));
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_GENERIC_ACCESS_CNF;
    pUsimMsg->ulResult      = ulResult;
    pUsimMsg->usPathLen     = usPathLen;
    pUsimMsg->ucINS         = pstData->ucINS;
    pUsimMsg->ucSw1         = pstData->ucSw1;
    pUsimMsg->ucSw2         = pstData->ucSw2;
    pUsimMsg->usLen         = pstData->usDataLen;

    if (0 != pstData->usDataLen)
    {
        VOS_MemCpy(pUsimMsg->aucContent, pstData->pucData, pstData->usDataLen);
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_RestrictedAccessCnf(VOS_UINT32                   ulReceiverPid,
                                            VOS_UINT32                  ulSendPara,
                                            VOS_UINT32                  ulResult,
                                            USIMM_RACCESSCNF_INFO_STRU  *pstCnfInfo)
{
    PS_USIM_RESTRICTED_ACCESS_CNF_STRU  *pUsimMsg = VOS_NULL_PTR;

    pUsimMsg = (PS_USIM_RESTRICTED_ACCESS_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,  sizeof(PS_USIM_RESTRICTED_ACCESS_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_RestrictedAccessCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid   = ulReceiverPid;
    pUsimMsg->ulMsgName       = PS_USIM_RESTRICTED_ACCESS_CNF;
    pUsimMsg->ulResult        = ulResult;

    pUsimMsg->ulErrorCode     = pstCnfInfo->ulErrorCode;

    pUsimMsg->ulSendPara      = ulSendPara;
    pUsimMsg->ucSW1           = pstCnfInfo->ucSW1;
    pUsimMsg->ucSW2           = pstCnfInfo->ucSW2;
    pUsimMsg->usLen           = 0;

    if((USIMM_API_SUCCESS == ulResult)&&(VOS_NULL_PTR != pstCnfInfo->pContent))
    {
        pUsimMsg->usLen = pstCnfInfo->ucLen;

        VOS_MemCpy(pUsimMsg->aucContent, pstCnfInfo->pContent, pstCnfInfo->ucLen);
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_TerminalResponseCnf(
    VOS_UINT32                          ulReceiverPid,
    VOS_UINT32                          ulErrorCode,
    VOS_UINT32                          ulSendPara,
    VOS_UINT8                           ucSW1,
    VOS_UINT8                           ucSW2)
{
    PS_USIM_TERMINALRESPONSE_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_TERMINALRESPONSE_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_TERMINALRESPONSE_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_TerminalResponseCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_TERMINALRESPONSE_CNF;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->ulSendPara    = ulSendPara;
    pUsimMsg->ucSW1         = ucSW1;
    pUsimMsg->ucSW2         = ucSW2;

    if(ulErrorCode != USIMM_SW_OK)
    {
        pUsimMsg->ulResult = VOS_ERR;
    }
    else
    {
        pUsimMsg->ulResult = VOS_OK;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_EnvelopeCnf(VOS_UINT32                    ulReceiverPid,
                                    VOS_UINT32                  ulSendPara,
                                    VOS_UINT32                  ulErrorCode,
                                    USIMM_ENVELOPECNF_INFO_STRU *pstCnfInfo)
{
    PS_USIM_ENVELOPE_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_ENVELOPE_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_ENVELOPE_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_EnvelopeCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_ENVELOPE_CNF;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->ulSendPara    = ulSendPara;
    pUsimMsg->ucDataType    = pstCnfInfo->ucDataType;
    pUsimMsg->ucSW1         = pstCnfInfo->ucSW1;
    pUsimMsg->ucSW2         = pstCnfInfo->ucSW2;

    if(ulErrorCode != USIMM_SW_OK)
    {
        pUsimMsg->ulResult      = VOS_ERR;
    }
    else
    {
        pUsimMsg->ulResult      = VOS_OK;
    }

    /* 对于SMS PP DOWNLOAD，可能有数据要回复到网侧，需要上报给TAF */
    if (pstCnfInfo->ulDataLen > VOS_NULL)
    {
        VOS_MemCpy(pUsimMsg->aucData, pstCnfInfo->pucData, pstCnfInfo->ulDataLen);

        pUsimMsg->ucDataLen = (VOS_UINT8)pstCnfInfo->ulDataLen;
    }
    else
    {
        pUsimMsg->ucDataLen = 0;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}
VOS_VOID USIMM_SingleCmdCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulMsgName, VOS_UINT32 ulResult)
{
    PS_USIM_SINGLECMD_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_SINGLECMD_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, (sizeof(PS_USIM_SINGLECMD_CNF_STRU)-VOS_MSG_HEAD_LENGTH));

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_SingleCmdCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid     = ulReceiverPid;
    pUsimMsg->ulMsgName         = ulMsgName;
    pUsimMsg->ulResult          = ulResult;

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_FDNCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulFDNState)
{
    PS_USIM_FDN_CNF_STRU  *pUsimMsg;

    pUsimMsg = (PS_USIM_FDN_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_FDN_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_FDNCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid     = ulReceiverPid;
    pUsimMsg->ulMsgName         = PS_UISM_FDN_CNF;
    pUsimMsg->ulResult          = ulResult;
    pUsimMsg->enFDNState        = ulFDNState;

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_SetSPBFileCnf(VOS_UINT32                     ulReceiverPid,
                                      VOS_UINT32                    ulErrorCode,
                                      USIMM_SET_SPBFILE_REQ_STRU    *pstMsg)
{
    PS_USIM_SPB_CNF_STRU *pUsimMsg = VOS_NULL_PTR;
    VOS_UINT32           i;

    pUsimMsg = (PS_USIM_SPB_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_SPB_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_SetSPBFileCnf: VOS_AllocMsg is Failed");

        return;
    }

    if(ulErrorCode != USIMM_SW_OK)
    {
        pUsimMsg->ulResult = VOS_ERR;
    }
    else
    {
        pUsimMsg->ulResult = VOS_OK;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_SET_SPBFILE_CNF;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->ucFileNum     = (VOS_UINT8)pstMsg->ulSPBReqCount;

    if (VOS_OK == pUsimMsg->ulResult)
    {
        for(i = 0; i < pstMsg->ulSPBReqCount; i++)
        {
            pUsimMsg->ausEfId[i]      = pstMsg->stSPBReq[i].usFileID;
            pUsimMsg->aucRecordNum[i] = pstMsg->stSPBReq[i].ucRecordNum;
            pUsimMsg->ausEfLen[i]     = pstMsg->stSPBReq[i].usDataLen;
        }
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_SearchHandleCnf(VOS_UINT32                   ulReceiverPid,
                                        VOS_UINT32                   ulErrorCode,
                                         USIMM_SEARCHCNF_INFO_STRU  *pstCnfInfo)
{
    PS_USIM_SEARCH_CNF_STRU     *pUsimMsg;

    pUsimMsg = (PS_USIM_SEARCH_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                            (sizeof(PS_USIM_SEARCH_CNF_STRU) - VOS_MSG_HEAD_LENGTH) + pstCnfInfo->ulLen);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_SearchHandleCnf: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_SEARCH_CNF;
    pUsimMsg->ulErrorCode   = ulErrorCode;
    pUsimMsg->usTotalRecNum = pstCnfInfo->usTotalRecNum;
    pUsimMsg->usEfId        = pstCnfInfo->usEfid;
    pUsimMsg->ulResult      = ulErrorCode;
    pUsimMsg->ucLen         = 0;

    if ((VOS_NULL_PTR != pstCnfInfo->pucData) && (pstCnfInfo->ulLen <= 255))
    {
        VOS_MemCpy(pUsimMsg->aucContent, pstCnfInfo->pucData, pstCnfInfo->ulLen);

        pUsimMsg->ucLen = (VOS_UINT8)pstCnfInfo->ulLen;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}


VOS_VOID USIMM_CardStatusInd(VOS_UINT32                     ulReceiverPid,
                                    USIMM_CARD_TYPE_ENUM_UINT32     enCardType,
                                    USIMM_CARD_SERVIC_ENUM_UINT32   enCardStatus)
{
    PS_USIM_STATUS_IND_STRU            *pUsimMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulFileNum;
    VOS_UINT8                          *pucIMSI = VOS_NULL_PTR;

    if(USIMM_CARD_SERVIC_AVAILABLE == enCardStatus)
    {
        ulResult = USIMM_PoolFindFile(EFIMSI, &ulFileNum, USIMM_UNLIMIT_APP);

        if(VOS_OK == ulResult)
        {
            pucIMSI = gstUSIMMPOOL.astpoolRecord[ulFileNum].pucContent;
        }
    }

    pUsimMsg = (PS_USIM_STATUS_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_STATUS_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pUsimMsg )
    {
        USIMM_ERROR_LOG("USIMM_CardStatusInd: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->ulReceiverPid = ulReceiverPid;
    pUsimMsg->ulMsgName     = PS_USIM_GET_STATUS_IND;    /* 设置消息名称 */
    pUsimMsg->enCardType    = enCardType;
    pUsimMsg->enCardStatus  = enCardStatus;

    if(VOS_NULL_PTR != pucIMSI)
    {
        VOS_MemCpy(pUsimMsg->aucIMSI, pucIMSI, sizeof(pUsimMsg->aucIMSI));

        pUsimMsg->ucIMSILen = sizeof(pUsimMsg->aucIMSI);
    }
    else
    {
        pUsimMsg->ucIMSILen = VOS_NULL;
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    if(USIMM_CARD_SERVIC_AVAILABLE != enCardStatus) /*卡状态不是有卡，保存数据*/
    {
        USIMM_SaveLogFile();
    }

    g_stUSIMMLogData.enLogState = USIMM_NO_NEED_LOG;/*上报卡状态就不需要记录log*/

    return;
}

/*2阶段开发使用*/

VOS_VOID USIMM_SatDataInd(VOS_UINT8   ucCmdType,
                                    VOS_UINT16 usDataLen,
                                    VOS_UINT8 *pucData)
{
    PS_USIM_SAT_IND_STRU *pUsimMsg;

    pUsimMsg = (PS_USIM_SAT_IND_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_SAT_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        return;
    }

    pUsimMsg->ulReceiverPid = MAPS_STK_PID;
    pUsimMsg->ulMsgName     = PS_USIM_SAT_IND;
    pUsimMsg->usLen         = usDataLen;
    pUsimMsg->ucCmdType     = ucCmdType;

    VOS_MemCpy(pUsimMsg->aucContent, pucData, usDataLen);

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}

#if (FEATURE_VSIM == FEATURE_ON)

VOS_VOID USIMM_VsimReDhNegotiateInd(VOS_VOID)
{
    PS_USIM_VSIM_REDH_IND_STRU         *pstMsg;

    pstMsg = (PS_USIM_VSIM_REDH_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_VSIM_REDH_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    pstMsg->ulReceiverPid = MAPS_PIH_PID;
    pstMsg->ulMsgName     = PS_USIMM_VSIM_RDH_IND;

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_USIM, pstMsg);

    return;
}
#endif

VOS_VOID USIMM_EccNumberInd(VOS_UINT8 ucEccType,
                                        VOS_UINT16 usEfLen,
                                        VOS_UINT8 ucRecordNum,
                                        VOS_UINT8 *pucData)
{

    PS_USIM_ECC_IND_STRU *pUsimMsg;

    pUsimMsg = (PS_USIM_ECC_IND_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, (sizeof(PS_USIM_ECC_IND_STRU)-VOS_MSG_HEAD_LENGTH)+usEfLen);

    if (VOS_NULL_PTR == pUsimMsg)
    {
        return;
    }

    pUsimMsg->ulReceiverPid = MAPS_PB_PID;                                           /*填写PB的PID*/
    pUsimMsg->ulMsgName     = PS_USIM_ECCNUMBER_IND;
    pUsimMsg->ucEccType     = ucEccType;
    pUsimMsg->ucRecordNum   = ucRecordNum;
    pUsimMsg->usFileSize    = usEfLen;

    if(0 != usEfLen)
    {
        VOS_MemCpy(pUsimMsg->ucContent, pucData, usEfLen);
    }

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}
VOS_VOID USIMM_IsdbAccessCnf(VOS_UINT32                 ulReceiverPid,
                                    VOS_UINT32                  ulResult,
                                    USIMM_ISDB_ACCESS_CNF_STRU  *pstData)
{
    PS_USIM_ISDB_ACCESS_CNF_STRU       *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_USIM_ISDB_ACCESS_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                              sizeof(PS_USIM_ISDB_ACCESS_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_IsdbAccessCnf: VOS_AllocMsg is Failed");

        return;
    }

    pstUsimMsg->ulReceiverPid           =   ulReceiverPid;
    pstUsimMsg->ulMsgName               =   PS_USIMM_ISDB_ACCESS_CNF;
    pstUsimMsg->ulResult                =   ulResult;
    pstUsimMsg->ucSw1                   =   pstData->ucSw1;
    pstUsimMsg->ucSw2                   =   pstData->ucSw2;
    pstUsimMsg->usLen                   =   pstData->usDataLen;

    if (VOS_NULL != pstData->usDataLen)
    {
        VOS_MemCpy(pstUsimMsg->aucContent, pstData->pucData, pstData->usDataLen);
    }

    (VOS_VOID)VOS_SendMsg(pstUsimMsg->ulSenderPid, pstUsimMsg);

    return;
}


VOS_VOID USIMM_OpenChannelCnf(VOS_UINT32                 ulReceiverPid,
                                        VOS_UINT32                ulResult,
                                        VOS_UINT32                ulErrCode,
                                        VOS_UINT32                ulSessionId)
{
    PS_USIM_OPEN_CHANNEL_CNF_STRU      *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_USIM_OPEN_CHANNEL_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                              sizeof(PS_USIM_OPEN_CHANNEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannelCnf: VOS_AllocMsg is Failed");

        return;
    }

    pstUsimMsg->ulReceiverPid           =   ulReceiverPid;
    pstUsimMsg->ulMsgName               =   PS_USIMM_OPENCHANNEL_CNF;
    pstUsimMsg->ulResult                =   ulResult;
    pstUsimMsg->ulErrCode               =   ulErrCode;
    pstUsimMsg->ulSessionId             =   ulSessionId;

    (VOS_VOID)VOS_SendMsg(pstUsimMsg->ulSenderPid, pstUsimMsg);

    return;
}


VOS_VOID USIMM_CloseChannelCnf(VOS_UINT32                 ulReceiverPid,
                                        VOS_UINT32                ulResult,
                                        VOS_UINT32                ulErrCode)
{
    PS_USIM_CLOSE_CHANNEL_CNF_STRU     *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_USIM_CLOSE_CHANNEL_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                              sizeof(PS_USIM_CLOSE_CHANNEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_CloseChannelCnf: VOS_AllocMsg is Failed");

        return;
    }

    pstUsimMsg->ulReceiverPid           =   ulReceiverPid;
    pstUsimMsg->ulMsgName               =   PS_USIMM_CLOSECHANNEL_CNF;
    pstUsimMsg->ulResult                =   ulResult;
    pstUsimMsg->ulErrCode               =   ulErrCode;

    (VOS_VOID)VOS_SendMsg(pstUsimMsg->ulSenderPid, pstUsimMsg);

    return;
}


VOS_VOID USIMM_AccessChannelCnf(VOS_UINT32                 ulReceiverPid,
                                            VOS_UINT32                  ulResult,
                                            VOS_UINT32                  ulErrCode,
                                            USIMM_ACCESS_CHANNEL_CNF_STRU *pstDataCnf)
{
    PS_USIM_ACCESS_CHANNEL_CNF_STRU    *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_USIM_ACCESS_CHANNEL_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                              sizeof(PS_USIM_ACCESS_CHANNEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelCnf: VOS_AllocMsg is Failed");

        return;
    }

    pstUsimMsg->ulReceiverPid           =   ulReceiverPid;
    pstUsimMsg->ulMsgName               =   PS_USIMM_ACCESSCHANNEL_CNF;
    pstUsimMsg->ulResult                =   ulResult;
    pstUsimMsg->ulErrCode               =   ulErrCode;
    pstUsimMsg->ucSw1                   =   pstDataCnf->ucSw1;
    pstUsimMsg->ucSw2                   =   pstDataCnf->ucSw2;
    pstUsimMsg->usLen                   =   pstDataCnf->usDataLen;

    if (VOS_NULL != pstDataCnf->usDataLen)
    {
        VOS_MemCpy(pstUsimMsg->aucContent, pstDataCnf->pucData, pstDataCnf->usDataLen);
    }

    (VOS_VOID)VOS_SendMsg(pstUsimMsg->ulSenderPid, pstUsimMsg);

    return;
}

/*****************************************************************************
函 数 名  :USIMM_SendTPDUCnf
功能描述  :Send TPDU消息回复
输入参数  :ulReceiverPid:接收消息PID
           ulResult:操作结果
           ulErrCode:错误码
           pstDataCnf:Send TPDU消息回复内容
输出参数  :无
返 回 值  :无

修订记录  :
1. 日    期   : 2014年1月15日
   作    者   : zhuli
   修改内容   : Creat
*****************************************************************************/
VOS_VOID USIMM_SendTPDUCnf(VOS_UINT32                       ulReceiverPid,
                                        VOS_UINT32                      ulResult,
                                        VOS_UINT32                      ulErrCode,
                                        USIMM_SENDTPDU_CNF_STRU         *pstDataCnf)
{
    PS_USIM_SENDTPDU_CNF_STRU    *pstUsimMsg = VOS_NULL_PTR;

    pstUsimMsg = (PS_USIM_SENDTPDU_CNF_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                          sizeof(PS_USIM_SENDTPDU_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelCnf: VOS_AllocMsg is Failed");

        return;
    }

    pstUsimMsg->ulReceiverPid           =   ulReceiverPid;
    pstUsimMsg->ulMsgName               =   PS_USIM_SENDTPDU_CNF;
    pstUsimMsg->ulResult                =   ulResult;
    pstUsimMsg->ulErrorCode             =   ulErrCode;
    pstUsimMsg->ucSw1                   =   pstDataCnf->ucSw1;
    pstUsimMsg->ucSw2                   =   pstDataCnf->ucSw2;
    pstUsimMsg->ulLen                   =   pstDataCnf->usDataLen;

    if (VOS_NULL != pstDataCnf->usDataLen)
    {
        VOS_MemCpy(pstUsimMsg->aucContent, pstDataCnf->pucData, pstDataCnf->usDataLen);
    }

    if (VOS_NULL_PTR != pstDataCnf->pucTPDUHead)
    {
        VOS_MemCpy(pstUsimMsg->aucTPDUHead, pstDataCnf->pucTPDUHead, USIMM_TPDU_HEAD_LEN);
    }

    (VOS_VOID)VOS_SendMsg(pstUsimMsg->ulSenderPid, pstUsimMsg);

    return;

}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

