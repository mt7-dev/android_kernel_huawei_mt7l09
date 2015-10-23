
/*lint  --e{740,958,767,718,746}*/

#include "UsimmApi.h"
#include "usimmbase.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_USIMM_API_C


/*Modified by ouyangfei for DART,2010-2-4,begin*/
#if defined (__PS_WIN32_RECUR__) || defined (__DART__)
VOS_UINT8 g_ucCardType = USIMM_CARD_USIM;
#endif
/*Modified by ouyangfei for DART,2010-2-4,end*/

VOS_UINT32 USIMM_Read_OpenSpeedFile(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara, VOS_UINT16 usFileID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT8                   aucContent[250];
    VOS_UINT32                  i;
    USIMM_GETCNF_INFO_STRU      stFileInfo;
    USIMM_OPENSPEED_FILE_TO_NVID ausNVId[23] = {
                                {0x6F62, USIMM_EF6F62Len, en_NV_Item_EF6F62},
                                {0x6F7B, USIMM_EF6F7BLen, en_NV_Item_EF6F7B},
                                {0x6F60, USIMM_EF6F60Len, en_NV_Item_EF6F60},
                                {0x6F61, USIMM_EF6F61Len, en_NV_Item_EF6F61},
                                {0x6F31, USIMM_EF6F31Len, en_NV_Item_EF6F31},
                                {0x6FAD, USIMM_EF6FADLen, en_NV_Item_EF6FAD},
                                {0x6F38, USIMM_EF6F38Len, en_NV_Item_EF6F38},
                                {0x6F7E, USIMM_EF6F7ELen, en_NV_Item_EF6F7E},
                                {0x6F73, USIMM_EF6F73Len, en_NV_Item_EF6F73},
                                {0x6F53, USIMM_EF6F53Len, en_NV_Item_EF6F53},
                                {0x6F07, USIMM_EF6F07Len, en_NV_Item_EF6F07},
                                {0x6F08, USIMM_EF6F08Len, en_NV_Item_EF6F08},
                                {0x6F09, USIMM_EF6F09Len, en_NV_Item_EF6F09},
                                {0x6F20, USIMM_EF6F20Len, en_NV_Item_EF6F20},
                                {0x6F52, USIMM_EF6F52Len, en_NV_Item_EF6F52},
                                {0x4F20, USIMM_EF4F20Len, en_NV_Item_EF4F20},
                                {0x4F52, USIMM_EF4F52Len, en_NV_Item_EF4F52},
                                {0x6FB7, USIMM_EF6FB7Len, en_NV_Item_EF6FB7},
                                {0x6F78, USIMM_EF6F78Len, en_NV_Item_EF6F78},
                                {0x6F5B, USIMM_EF6F5BLen, en_NV_Item_EF6F5B},
                                {0x6F5C, USIMM_EF6F5CLen, en_NV_Item_EF6F5C},
                                {0x6FC4, USIMM_EF6FC4Len, en_NV_Item_EF6FC4},
                                {0x6F74, USIMM_EF6F74Len, en_NV_Item_EF6F74}
                            };

    for(i=0; i<23; i++)
    {
        if(ausNVId[i].usFileId == usFileID)
        {
            break;
        }
    }

    if(i >=  23)
    {
        USIMM_WARNING_LOG("USIMM_Read_OpenSpeedFile: The File is Not Exist in Flash");

        return USIMM_API_FAILED;
    }

    ulResult = NV_Read(ausNVId[i].enNVId, aucContent, ausNVId[i].usEfLen);

    if(NV_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_Read_OpenSpeedFile: Read NV is Failed");

        return USIMM_API_FAILED;
    }

    stFileInfo.usEfId       = usFileID;
    stFileInfo.ucRecordNum  = 0;
    stFileInfo.ucTotalNum   = 0;
    stFileInfo.usDataLen    = ausNVId[i].usEfLen;
    stFileInfo.usEfLen      = ausNVId[i].usEfLen;
    stFileInfo.pucEf        = aucContent;

    USIMM_GetFileCnf(ulSenderPid, ulSendPara, USIMM_SW_OK, &stFileInfo);

    return USIMM_API_SUCCESS;
}


VOS_UINT32 USIMM_ApiParaCheckSimple(VOS_VOID)
{
    if(gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM)   /*检查消息队列数量*/
    {
        USIMM_WARNING_LOG("USIMM_ApiParaCheck: The Msg Queue is Full");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    if((gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService == USIMM_CARD_SERVIC_ABSENT)
        ||(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed))/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_ApiParaCheck: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    return USIMM_API_SUCCESS;
}

VOS_UINT32 USIMM_ApiParaCheck(VOS_VOID)
{
    VOS_UINT32 ulResult;

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_ApiParaCheck: The vSIM is Open");

        return USIMM_API_NOTAVAILABLE;
    }

    ulResult = USIMM_ApiParaCheckSimple();

    if(USIMM_API_SUCCESS != ulResult)
    {
        return ulResult;
    }

    return USIMM_API_SUCCESS;
}
VOS_UINT32 USIMM_ApiSendMsg(USIMM_MsgBlock *pMsg,USIMM_API_MSG_TYPE_ENUM_UINT32 enMsgType)
{
    VOS_UINT32 ulResult;
    VOS_UINT32 ulMsgId;

    ulResult = USIMM_API_SMP;

    if(VOS_OK != ulResult)
    {
        LogPrint1("\r\nError:USIMM_ApiSendMsg: Send Msg ID %d Get Sm is Failed!\r\n", (VOS_INT)pMsg->enMsgType);/*打印错误*/

        VOS_FreeMsg(WUEPS_PID_USIM, pMsg);

        return USIMM_API_SMSP_FAILED;
    }

    ulMsgId = pMsg->enMsgType;

    gulUSIMMAPIMessageNum++;

    if(USIMM_API_MSG_URGENT == enMsgType)
    {
        ulResult = VOS_SendUrgentMsg(pMsg->ulSenderPid, pMsg);
    }
    else
    {
        ulResult = VOS_SendMsg(pMsg->ulSenderPid, pMsg);
    }

    if(VOS_OK != ulResult)
    {
        LogPrint1("Error:USIMM_ApiSendMsg: VOS_SendMsg is Failed, Msg Id is %d.", (VOS_INT)ulMsgId);/*打印错误*/

        gulUSIMMAPIMessageNum--;

        USIMM_API_SMV;                          /*PV释放*/

        return USIMM_API_SENDMSG_FAILED;
    }

    USIMM_API_SMV;                          /*PV释放*/

    return USIMM_API_SUCCESS;
}


VOS_UINT32 USIMM_SendInitCardMsg(VOS_UINT32 ulSenderPid, USIMM_CMDTYPE_ENUM_UINT16 enMsgType)
{
    USIMM_MsgBlock *pstMsg = VOS_NULL_PTR;

    pstMsg = (USIMM_MsgBlock *)VOS_AllocMsg(ulSenderPid, sizeof(USIMM_MsgBlock)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_InitCardStart:AllocMsg Failed.");

        return VOS_ERR;
    }

    pstMsg->ulReceiverPid      = WUEPS_PID_USIM;

    pstMsg->enMsgType          = enMsgType;

    if(VOS_OK != VOS_SendMsg(ulSenderPid, pstMsg))
    {
        USIMM_ERROR_LOG("USIMM_InitCardStart:SendMsg Failed.\n");

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_BOOL USIMM_VsimIsActive(VOS_VOID)
{
#if (FEATURE_VSIM == FEATURE_ON)
    VSIM_CARD_STATE_NV_STRU stNvData;

#if defined(INSTANCE_1)
    /* SVLTE下Modem1虚拟卡永远不使能 */
    if (VOS_TRUE == USIMM_IsSvlte())
    {
        return VOS_FALSE;
    }
#endif
    if(NV_OK != NV_Read(en_NV_Item_VSIM_SUPPORT_FLAG, &stNvData, sizeof(stNvData)))
    {
        USIMM_ERROR_LOG("USIMM_VsimIsActive:Read NV Failed.\n");
        return VOS_FALSE;
    }

    if(VSIM_VIRT_CARD_DEACTIVE == stNvData.enVsimState)
    {
        return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }
#else
    return VOS_FALSE;
#endif
}
VOS_UINT32  USIMM_TerminalResponseReq(VOS_UINT32 ulSenderPid,VOS_UINT8 ucLen,VOS_UINT8 *pucRsp)
{
    USIMM_TERMINALRESPONSE_REQ_STRU *pMsg;
    VOS_UINT32                      ulResult;

    if((0 == ucLen) || (VOS_NULL_PTR == pucRsp)) /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_TerminalResponseReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_TerminalResponseReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_TERMINALRESPONSE_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                (sizeof(USIMM_TERMINALRESPONSE_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+ucLen);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_TerminalResponseReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->enMsgType         = USIMM_CMDTYPE_TERMINALRSP_REQ;
    pMsg->ulSendPara        = USIMM_TR_TYPE_BALONG;
    pMsg->ucDataLen         = ucLen;

    VOS_MemCpy(pMsg->aucContent, pucRsp, ucLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}


VOS_UINT32  USIMM_CbpTerminalResponseReq(VOS_UINT32 ulSenderPid,VOS_UINT8 ucLen,VOS_UINT8 *pucRsp)
{
    USIMM_TERMINALRESPONSE_REQ_STRU    *pMsg;

    if ((0 == ucLen) || (VOS_NULL_PTR == pucRsp)) /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_CbpTerminalResponseReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    pMsg = (USIMM_TERMINALRESPONSE_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                (sizeof(USIMM_TERMINALRESPONSE_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+ucLen);

    if (VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_CbpTerminalResponseReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->enMsgType         = USIMM_CMDTYPE_TERMINALRSP_REQ;
    pMsg->ulSendPara        = USIMM_TR_TYPE_CBP;
    pMsg->ucDataLen         = ucLen;

    VOS_MemCpy(pMsg->aucContent, pucRsp, ucLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_EnvelopeReq(VOS_UINT32 ulSenderPid,VOS_UINT32 ulSendPara,VOS_UINT8 ucLen,VOS_UINT8 *pucEnvelope)
{
    USIMM_ENVELOPE_REQ_STRU     *pMsg;
    VOS_UINT32                  ulResult;

    if((0 == ucLen) || (VOS_NULL_PTR == pucEnvelope)) /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_EnvelopeReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_EnvelopeReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_ENVELOPE_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                (sizeof(USIMM_ENVELOPE_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+ucLen);
    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_EnvelopeReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->enMsgType         = USIMM_CMDTYPE_ENVELOPE_REQ;
    pMsg->ulSendPara        = ulSendPara;
    pMsg->ucDataLen         = ucLen;

    VOS_MemCpy(pMsg->aucContent, pucEnvelope, ucLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_FDNHandleReq (VOS_UINT32                       ulSenderPid,
                                        USIMM_FBDN_HANDLE_ENUM_UINT32    enFucEnable,
                                        VOS_UINT8                       *pucPin2)
{
    USIMM_FDN_REQ_STRU *pMsg;
    VOS_UINT32         ulResult;

    if((enFucEnable >= USIMM_FBDN_BUTT)||(VOS_NULL_PTR == pucPin2))     /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_FDNHandleReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_FDNHandleReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_FDN_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                            sizeof(USIMM_FDN_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_FDNHandleReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    pMsg->ulReceiverPid         = WUEPS_PID_USIM;
    pMsg->enMsgType             = USIMM_CMDTYPE_FDN_REQ;
    pMsg->enFDNHandleType       = enFucEnable;

    VOS_MemCpy(pMsg->aucPIN2, pucPin2, 8);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg, USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_SetFileReq(VOS_UINT32                ulSenderPid,
                                VOS_UINT32                  ulSendPara,
                                USIMM_SET_FILE_INFO_STRU    *pstSetFileInfo)
{
    USIMM_SETFILE_REQ_STRU      *pMsg;
    VOS_UINT32                  ulResult;

    if((VOS_NULL_PTR == pstSetFileInfo->pucEfContent)||(0 == pstSetFileInfo->ulEfLen))  /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_SetFileReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    ulResult = USIMM_ApiParaCheckSimple();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SetFileReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_SETFILE_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                (sizeof(USIMM_SETFILE_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+pstSetFileInfo->ulEfLen);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_SetFileReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    if ((0x6f73 == pstSetFileInfo->usEfId) && (USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType) )
    {
        pMsg->usFileID  = 0x6f53;
    }
    else
    {
        pMsg->usFileID  = pstSetFileInfo->usEfId;
    }

    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSendPara        = ulSendPara;
    pMsg->enMsgType         = USIMM_CMDTYPE_SETFILE_REQ;
    pMsg->usDataLen         = (VOS_UINT16)pstSetFileInfo->ulEfLen;
    pMsg->ucRecordNum       = pstSetFileInfo->ucRecordNum;
    pMsg->enAppType         = pstSetFileInfo->enAppType;

    VOS_MemCpy(pMsg->aucContent, pstSetFileInfo->pucEfContent, pstSetFileInfo->ulEfLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_GetFileReq(VOS_UINT32                   ulSenderPid,
                                    VOS_UINT32                  ulSendPara,
                                    USIMM_GET_FILE_INFO_STRU    *pstGetFileInfo)
{
    USIMM_GETFILE_REQ_STRU      *pMsg;
    VOS_UINT32                  ulResult;

    if((0 != (pstGetFileInfo->usEfId & 0x9000))||(0 == pstGetFileInfo->usEfId))   /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_GetAppFileReq: The Parameter is Wrong"); /*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    if(USIMM_OPENSPEEDENABLE== gulUSIMMOpenSpeed) /*快速开机开启*/
    {
        USIMM_INFO_LOG("USIMM_GetAppFileReq: The Open Speed is ON, Read Usim File from Flash");

        return USIMM_Read_OpenSpeedFile(ulSenderPid, ulSendPara, pstGetFileInfo->usEfId);
    }

    ulResult = USIMM_ApiParaCheckSimple();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_GetAppFileReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    if((0x6f73 == pstGetFileInfo->usEfId) && (USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType))
    {
        pstGetFileInfo->usEfId = 0x6f53;
    }

    pMsg = (USIMM_GETFILE_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                sizeof(USIMM_GETFILE_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_GetAppFileReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSendPara        = ulSendPara;
    pMsg->enMsgType         = USIMM_CMDTYPE_GETFILE_REQ;
    pMsg->usFileID          = pstGetFileInfo->usEfId;
    pMsg->ucRecordNum       = pstGetFileInfo->ucRecordNum;
    pMsg->enAppType         = pstGetFileInfo->enAppType;

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_GetAuthDataLen(USIMM_AUTH_DATA_STRU *pstAuth)
{
    VOS_UINT32                          ulDataLen = 0;

    switch (pstAuth->enAuthType)
    {
        /* 由于数据采用LV格式，所以每个字段都需要预留一个字节存放长度，下同 */
        case USIMM_3G_AUTH:
            ulDataLen = pstAuth->unAuthPara.st3GAuth.stRand.ulDataLen
                    + pstAuth->unAuthPara.st3GAuth.stAutn.ulDataLen + 2;
            break;

        case USIMM_2G_AUTH:
            ulDataLen = pstAuth->unAuthPara.st2GAuth.stRand.ulDataLen + 1;
            break;

        case USIMM_IMS_AUTH:
            ulDataLen = pstAuth->unAuthPara.stImsAuth.stRand.ulDataLen
                        + pstAuth->unAuthPara.stImsAuth.stAutn.ulDataLen + 2;
            break;

        case USIMM_HTTP_AUTH:
            ulDataLen = pstAuth->unAuthPara.stHttpAuth.stRealm.ulDataLen
                        + pstAuth->unAuthPara.stHttpAuth.stNonce.ulDataLen
                        + pstAuth->unAuthPara.stHttpAuth.stCnonce.ulDataLen + 3;
            break;

        /* 由于GBA鉴权方式在头部需要增加一个tag字段，长度要加1 */
        case USIMM_GBA_AUTH:
            ulDataLen = pstAuth->unAuthPara.stGbaAuth.stRand.ulDataLen
                        + pstAuth->unAuthPara.stGbaAuth.stAutn.ulDataLen + 3;
            break;

        /* 由于NAF鉴权方式在头部需要增加一个tag字段，长度要加1 */
        case USIMM_NAF_AUTH:
            ulDataLen = pstAuth->unAuthPara.stNafAuth.stNafID.ulDataLen
                        + pstAuth->unAuthPara.stNafAuth.stImpi.ulDataLen + 3;
            break;

        default:
            break;
    }

    return ulDataLen;
}
VOS_VOID USIMM_CopyAuthData(USIMM_AUTH_DATA_STRU *pstAuth, VOS_UINT8 *pucDest)
{
    VOS_UINT32                          ulOffset;

    switch (pstAuth->enAuthType)
    {
        case USIMM_3G_AUTH:
            pucDest[0] = (VOS_UINT8)pstAuth->unAuthPara.st3GAuth.stRand.ulDataLen;

            VOS_MemCpy(pucDest + 1, pstAuth->unAuthPara.st3GAuth.stRand.pucData, pucDest[0]);

            ulOffset = pucDest[0] + 1;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.st3GAuth.stAutn.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.st3GAuth.stAutn.pucData, pucDest[ulOffset]);

            break;

        case USIMM_2G_AUTH:
            pucDest[0] = (VOS_UINT8)pstAuth->unAuthPara.st2GAuth.stRand.ulDataLen;

            VOS_MemCpy(pucDest + 1, pstAuth->unAuthPara.st2GAuth.stRand.pucData, pucDest[0]);

            break;

        case USIMM_IMS_AUTH:
            pucDest[0] = (VOS_UINT8)pstAuth->unAuthPara.stImsAuth.stRand.ulDataLen;

            VOS_MemCpy(pucDest + 1, pstAuth->unAuthPara.stImsAuth.stRand.pucData, pucDest[0]);

            ulOffset = pucDest[0] + 1;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.stImsAuth.stAutn.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.stImsAuth.stAutn.pucData, pucDest[ulOffset]);

            break;

        case USIMM_HTTP_AUTH:
            pucDest[0] = (VOS_UINT8)pstAuth->unAuthPara.stHttpAuth.stRealm.ulDataLen;

            VOS_MemCpy(pucDest + 1, pstAuth->unAuthPara.stHttpAuth.stRealm.pucData, pucDest[0]);

            ulOffset = pucDest[0] + 1;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.stHttpAuth.stNonce.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.stHttpAuth.stNonce.pucData, pucDest[ulOffset]);

            ulOffset += pucDest[ulOffset] + 1;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.stHttpAuth.stCnonce.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.stHttpAuth.stCnonce.pucData, pucDest[ulOffset]);

            break;

        case USIMM_GBA_AUTH:
            pucDest[0] = USIMM_GBA_AUTH_TAG;

            pucDest[1] = (VOS_UINT8)pstAuth->unAuthPara.stGbaAuth.stRand.ulDataLen;

            VOS_MemCpy(pucDest + 2, pstAuth->unAuthPara.stGbaAuth.stRand.pucData, pucDest[1]);

            ulOffset = pucDest[1] + 2;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.stGbaAuth.stAutn.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.stGbaAuth.stAutn.pucData, pucDest[ulOffset]);

            break;

        case USIMM_NAF_AUTH:
            pucDest[0] = USIMM_NAF_AUTH_TAG;

            pucDest[1] = (VOS_UINT8)pstAuth->unAuthPara.stNafAuth.stNafID.ulDataLen;

            VOS_MemCpy(pucDest + 2, pstAuth->unAuthPara.stNafAuth.stNafID.pucData, pucDest[1]);

            ulOffset = pucDest[1] + 2;

            pucDest[ulOffset] = (VOS_UINT8)pstAuth->unAuthPara.stNafAuth.stImpi.ulDataLen;

            VOS_MemCpy(pucDest + ulOffset + 1, pstAuth->unAuthPara.stNafAuth.stImpi.pucData, pucDest[ulOffset]);

            break;

        default:
            break;
    }

    return;
}


VOS_UINT32 USIMM_AuthReq(VOS_UINT32 ulSenderPid, USIMM_APP_TYPE_ENUM_UINT32 enAppType, USIMM_AUTH_DATA_STRU *pstAuth)
{
    USIMM_AUTH_REQ_STRU                *pstMsg;
    VOS_UINT32                          ulAuthDataLen;
    VOS_UINT32                          ulResult;

    /* 应用类型检测 */
    if ((USIMM_GSM_APP != enAppType) && (USIMM_UMTS_APP != enAppType) && (USIMM_ISIM_APP != enAppType))
    {
        USIMM_WARNING_LOG("USIMM_AuthReq: The App Type is Wrong");

        return USIMM_API_WRONG_PARA;
    }

    if ((VOS_NULL_PTR == pstAuth) || (pstAuth->enAuthType >= USIMM_AUTH_BUTT))/*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_AuthReq: The Auth Type is Wrong");/*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    ulResult = USIMM_ApiParaCheckSimple();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_AuthReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    ulAuthDataLen = USIMM_GetAuthDataLen(pstAuth);

    if (USIMM_APDU_MAXLEN <= ulAuthDataLen)
    {
        USIMM_ERROR_LOG("USIMM_AuthReq: Auth data is too large.");

        return USIMM_API_WRONG_PARA;
    }

    pstMsg = (USIMM_AUTH_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                            sizeof(USIMM_AUTH_REQ_STRU) + ulAuthDataLen - VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_AuthReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
#ifdef __DART__
    pstMsg->ulReceiverPid     = DART_PID_USIM_STUB;
#else
    pstMsg->ulReceiverPid     = WUEPS_PID_USIM;
#endif

    pstMsg->enMsgType         = USIMM_CMDTYPE_AUTH_REQ;
    pstMsg->enAppType         = enAppType;
    pstMsg->enAuthType        = pstAuth->enAuthType;
    pstMsg->ucOpId            = pstAuth->ucOpId;
    pstMsg->ulDataLen         = ulAuthDataLen;

    USIMM_CopyAuthData(pstAuth, pstMsg->aucData);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_URGENT);
}
VOS_VOID USIMM_QueryPINType(VOS_UINT8 *pucPINType)
{
    /*根据校验状态判断需查寻的PIN码类型*/
    if(USIMM_PUK_BLOCK == gstUSIMMADFInfo.enPin1Verified)
    {
        USIMM_NORMAL_LOG("USIMM_QueryPINType: The Card PUK is Block");

        *pucPINType = USIMM_PUK;
    }
    else if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)/*先判断PUK状态*/
    {
        USIMM_NORMAL_LOG("USIMM_QueryPINType: The Card Need the PUK Password");

        *pucPINType = USIMM_PUK;
    }
    else if(USIMM_PIN_NEED == gstUSIMMADFInfo.enPin1Verified) /*再判断当前PIN1状态*/
    {
        USIMM_NORMAL_LOG("USIMM_QueryPINType: The Card Need the PIN Password");

        *pucPINType = USIMM_PIN;
    }
    else if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)   /*再判断PUK2状态*/
    {
        USIMM_NORMAL_LOG("USIMM_QueryPINType: The Card Need the PUK2 Password");

        *pucPINType = USIMM_PUK2;
    }
    else  /*再判断PIN2状态*/                                                                                  /*最后默认需要PIN2*/
    {
        USIMM_NORMAL_LOG("USIMM_QueryPINType: The Card Need the PIN2 Password");

        *pucPINType = USIMM_PIN2;
    }

    return;
}
VOS_UINT32 USIMM_HandlePINType(VOS_UINT8 ucCmdType, VOS_UINT32 *pulPINType)
{
    VOS_UINT32 ulResult = VOS_OK;

    switch(ucCmdType)
    {
        case USIMM_PINCMD_DISABLE:          /*激活，去激活和验证PIN用同一函数处理*/
            if(gstUSIMMADFInfo.stPINInfo.enPin1Enable == USIMM_PIN_ENABLED)
            {
                *pulPINType = USIMM_PIN;
            }
            else if(gstUSIMMADFInfo.stPINInfo.enPin2Enable == USIMM_PIN_ENABLED)
            {
                *pulPINType = USIMM_PIN2;
            }
            else
            {
                USIMM_ERROR_LOG("USIMM_HandlePINType: The PIN Enable State is Error");

                ulResult = VOS_ERR;
            }

            break;

        case USIMM_PINCMD_VERIFY:
            if(gstUSIMMADFInfo.enPin1Verified == USIMM_PIN_NEED)
            {
                *pulPINType = USIMM_PIN;
            }
            else if(gstUSIMMADFInfo.enPin2Verified == USIMM_PIN_NEED)
            {
                *pulPINType = USIMM_PIN2;
            }
            else
            {
                USIMM_ERROR_LOG("USIMM_HandlePINType: The PIN Verified State is Error");

                ulResult = VOS_ERR;
            }

            break;

        case USIMM_PINCMD_UNBLOCK:          /*解锁PIN处理*/
            if(gstUSIMMADFInfo.enPin1Verified == USIMM_PUK_NEED)
            {
                *pulPINType = USIMM_PIN;
            }
            else if(gstUSIMMADFInfo.enPin2Verified == USIMM_PUK_NEED)
            {
                *pulPINType = USIMM_PIN2;
            }
            else
            {
                USIMM_ERROR_LOG("USIMM_HandlePINType: The PIN Block State is Error");

                ulResult = VOS_ERR;
            }

            break;

        default:
            USIMM_ERROR_LOG("USIMM_HandlePINType: The PIN Cmd Type State is Error");
            break;
    }

    return ulResult;
}


VOS_UINT32 USIMM_PinReq(VOS_UINT32  ulSenderPid,
                                VOS_UINT8 ucCmdType,
                                VOS_UINT8 ucPINType,
                                VOS_UINT8 *pucOldPIN,
                                VOS_UINT8 *pucNewPIN)
{
    USIMM_PIN_REQ_STRU *pMsg;
    VOS_UINT32         ulResult = VOS_OK;

    if(gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM)   /*检查消息队列数量*/
    {
        USIMM_WARNING_LOG("USIMM_PinReq: The Msg Queue is Full");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    if(VOS_NULL_PTR == pucOldPIN)    /*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_PinReq: The Parameter is Wrong");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    if((gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService <= USIMM_CARD_SERVIC_UNAVAILABLE)
        ||(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed))/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_PinReq: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_PinReq: The vSIM is Open");

        return USIMM_API_NOTAVAILABLE;
    }

    pMsg = (USIMM_PIN_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                            sizeof(USIMM_PIN_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_PinReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_PINHANDLE_REQ;
    pMsg->enCmdType         = ucCmdType;

    if(ucPINType == USIMM_SIM_NON)
    {
        ulResult = USIMM_HandlePINType(ucCmdType, &pMsg->enPINType);
    }
    else
    {
        pMsg->enPINType     = ucPINType;
    }

    if ( VOS_OK!= ulResult )
    {
        VOS_FreeMsg(WUEPS_PID_USIM, pMsg);
        return USIMM_API_WRONG_PARA;
    }

    if ( USIMM_PUK == pMsg->enPINType )
    {
        pMsg->enPINType = USIMM_PIN;
    }
    else if ( USIMM_PUK2 == pMsg->enPINType )
    {
        pMsg->enPINType = USIMM_PIN2;
    }
    else
    {
        /*do nothing*/
    }

    VOS_MemCpy(pMsg->aucOldPIN, pucOldPIN, USIMM_PINNUMBER_LEN);

    if(pucNewPIN != VOS_NULL_PTR)
    {
        VOS_MemCpy(pMsg->aucNewPIN, pucNewPIN, USIMM_PINNUMBER_LEN);
    }

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_GenericAccessReq(VOS_UINT32               ulSenderPid,
                                            USIMM_U8_LVDATA_STRU    *pstData,
                                            VOS_UINT32              ulPathLen,
                                            VOS_UINT16              *pusPath)
{
    USIMM_GACCESS_REQ_STRU  *pMsg;

    if((gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM)   /*检查消息队列数量*/
        ||(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed))/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_GenericAccessReq: The Msg Queue is Full");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_GenericAccessReq: The vSIM is Open");

        return USIMM_API_NOTAVAILABLE;
    }

    if((VOS_NULL_PTR            == pstData)
        ||(0                    == pstData->ulDataLen)
        ||(VOS_NULL_PTR         == pstData->pucData)
        ||(USIMM_MAX_PATH_LEN   < ulPathLen))/*参数检查*/
    {
        USIMM_WARNING_LOG("USIMM_GenericAccessReq: The Parameter is Wrong");/*打印错误*/

        return USIMM_API_WRONG_PARA;/*返回函数错误信息*/
    }

    pMsg = (USIMM_GACCESS_REQ_STRU  *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                    (sizeof(USIMM_GACCESS_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+pstData->ulDataLen);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_GenericAccessReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_GACCESS_REQ;
    pMsg->usDataLen         = (VOS_UINT16)pstData->ulDataLen;
    pMsg->ulPathLen         = ulPathLen;

    if(ulPathLen != 0)
    {
        VOS_MemCpy(pMsg->ausPath, pusPath, ulPathLen*sizeof(VOS_UINT16));
    }

    VOS_MemCpy(pMsg->aucContent, pstData->pucData, pstData->ulDataLen);

	return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_RestrictedAccessReq(VOS_UINT32     ulSenderPid,
                                VOS_UINT32                  ulSendPara,
                                TAF_SIM_RESTRIC_ACCESS_STRU *pstData)
{
    USIMM_RACCESS_REQ_STRU *pMsg;
    VOS_UINT32              ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_GenericAccessReq: The Para Check is Error");/*打印错误*/

        return ulResult;
    }

    pMsg = (USIMM_RACCESS_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                  sizeof(USIMM_RACCESS_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_RestrictedAccessReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    VOS_MemSet(pMsg->aucContent, 0, sizeof(pMsg->aucContent));

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->ulSendPara        = ulSendPara;
    pMsg->enMsgType         = USIMM_CMDTYPE_RACCESS_REQ;
    pMsg->enCmdType         = pstData->usCmdType;
    pMsg->ucP1              = pstData->ucP1;
    pMsg->ucP2              = pstData->ucP2;
    pMsg->ucP3              = pstData->ucP3;
    pMsg->usEfId            = pstData->usEfId;

    /* 拷贝文件路径 */
    pMsg->usPathLen         = ((pstData->usPathLen > USIMM_MAX_PATH_LEN)?\
                              USIMM_MAX_PATH_LEN:pstData->usPathLen);

    /* 文件路径长度是U16为单位的，路径拷贝的长度要乘2 */
    VOS_MemCpy(pMsg->ausPath, pstData->ausPath, (pMsg->usPathLen)*(sizeof(VOS_UINT16)));

    /* 根据P3参数拷贝<DATA>字段 */
    VOS_MemCpy(pMsg->aucContent, pstData->aucCommand, pstData->ucP3);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_MaxEFRecordNumReq(VOS_UINT32                   ulSenderPid,
                                                USIMM_APP_TYPE_ENUM_UINT32  enAppType,
                                                VOS_UINT16                  usEFid)
{
    USIMM_MAXRECORD_REQ_STRU    *pMsg;
    VOS_UINT32                  ulResult;

    if ((usEFid & 0x9000 ) != 0)    /*检查是否是合法的EFID*/
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: The Parameter is Wrong");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_MAXRECORD_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                    sizeof(USIMM_MAXRECORD_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_MaxEFRecordNumReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->enAppType         = enAppType;
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_MAXRECORD_REQ;
    pMsg->usEfId            = usEFid;

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_StatusReq (VOS_UINT32 ulSenderPid, USIMM_POLL_FCP_ENUM_UINT32 enNeedFcp)
{
    USIMM_STATUS_REQ_STRU              *pMsg;
    VOS_UINT32                          ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_StatusReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_STATUS_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                sizeof(USIMM_STATUS_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_StatusReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    ulResult = USIMM_IsTestCard();

    /*填充消息内容*/
    if((gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType == USIMM_CARD_SIM)||(VOS_TRUE == ulResult))
    {
        pMsg->enNeedFcp     = USIMM_POLL_NEED_FCP;
    }
    else
    {
        pMsg->enNeedFcp         = enNeedFcp;
    }

    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_STATUS_REQ;

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_ResetReq(
    VOS_UINT32                          ulSenderPid,
    USIMM_STK_COMMAND_DETAILS_STRU     *pstCMDDetail,
    USIMM_RESET_INFO_STRU              *pstRstInfo)
{
    USIMM_REFRESH_REQ_STRU *pMsg;
    VOS_UINT32              ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_ResetReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_REFRESH_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                sizeof(USIMM_REFRESH_REQ_STRU)-VOS_MSG_HEAD_LENGTH + pstRstInfo->usFileListLen);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_ResetReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_REFRESH_REQ;
    pMsg->ulCommandNum      = pstCMDDetail->ucCommandNum;
    pMsg->ulCommandType     = pstCMDDetail->ucCommandType;
    pMsg->enRefreshType     = pstCMDDetail->ucCommandQua;

    if (USIMM_AID_LEN_MAX < pstRstInfo->usAidLen)
    {
        pMsg->ulAidLen = USIMM_AID_LEN_MAX;
    }
    else
    {
        pMsg->ulAidLen = pstRstInfo->usAidLen;
    }

    if (VOS_NULL != pMsg->ulAidLen)
    {
        VOS_MemCpy(pMsg->aucAid, pstRstInfo->aucAid, pMsg->ulAidLen);
    }

    pMsg->usLen = pstRstInfo->usFileListLen;

    if (VOS_NULL != pstRstInfo->usFileListLen)
    {
        VOS_MemCpy(pMsg->aucFileList, pstRstInfo->aucFileList, pstRstInfo->usFileListLen);
    }

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_IsServiceAvailable(UICC_SERVICES_TYPE_ENUM_UINT32 enService)
{
    /* 参数检查 */
    if (UICC_SVR_NONE == enService)
    {
        USIMM_WARNING_LOG("USIMM_IsServiceAvailable: The Parameter is Wrong");

        return PS_USIM_SERVICE_NOT_AVAILIABLE;
    }

    if (enService < SIM_SVR_BUTT)
    {
        return USIMM_IsSIMServiceAvailable(enService);
    }

    if (enService < USIM_SVR_BUTT)
    {
        return USIMM_IsUSIMServiceAvailable(enService);
    }

    if (enService < ISIM_SVR_BUTT)
    {
        return USIMM_IsISIMServiceAvailable(enService);
    }

    USIMM_WARNING_LOG("USIMM_IsServiceAvailable: The Parameter is Wrong");

    return PS_USIM_SERVICE_NOT_AVAILIABLE;
}
VOS_BOOL USIMM_IsTestCard(VOS_VOID)
{
    VOS_UINT32                          ulFileNum;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          i;
    USIMM_TEST_CARD_CFG_ST              stPlmnInfo;
    VOS_UINT8                           aucPLMN[USIMM_TEST_CARD_PLMN_LEN];

    if ((gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService != USIMM_CARD_SERVIC_AVAILABLE)
     || (USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed))/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_IsTestCard: The Card Status is Wrong");

        return VOS_FALSE;
    }

    /* 查询当前的文件缓冲 */
    if (USIMM_API_SUCCESS != USIMM_PoolFindFile(0x6F07, &ulFileNum, USIMM_UNLIMIT_APP))
    {
        USIMM_WARNING_LOG("USIMM_IsTestCard: Read File is Failed");

        return VOS_FALSE;
    }

    ulRslt = NV_Read(en_NV_Item_Test_Card_Cfg, &stPlmnInfo, sizeof(USIMM_TEST_CARD_CFG_ST));

    if ((VOS_OK != ulRslt)
     || (USIMM_TEST_CARD_CFG_NUM < stPlmnInfo.ulPlmnNum))
    {
        USIMM_WARNING_LOG("USIMM_IsTestCard: Read NV is Failed");

        return VOS_FALSE;
    }

    VOS_MemCpy(aucPLMN, gstUSIMMPOOL.astpoolRecord[ulFileNum].pucContent + 1, USIMM_TEST_CARD_PLMN_LEN);
    /* coverity[uninit_use] */
    aucPLMN[0] &= 0xF0;

    for (i = 0; i < stPlmnInfo.ulPlmnNum; i++)
    {
        if (VOS_OK == VOS_MemCmp(aucPLMN,
                                 stPlmnInfo.astPlmnList[i].aucPlmn,
                                 USIMM_TEST_CARD_PLMN_LEN))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
VOS_UINT32  USIMM_GetPinStatus(VOS_UINT8 *pucType,VOS_UINT8 *pucBeEnable,VOS_UINT8 *pucBeNeed,VOS_UINT8 *pucNum)
{
    USIMM_PIN_TYPE_ENUM_UINT32 enPINType;

    /*卡状态检查*/
    if (gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService == USIMM_CARD_SERVIC_ABSENT)
    {
        USIMM_WARNING_LOG("USIMM_GetPinStatus: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    if (USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
    {
        USIMM_WARNING_LOG("USIMM_GetPinStatus: Speed Open Mode");

        *pucBeNeed = USIMM_NONEED;

        return USIMM_API_SUCCESS;
    }

    if(gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService == USIMM_CARD_SERVIC_UNAVAILABLE)
    {
        if(USIMM_PUK_BLOCK == gstUSIMMADFInfo.enPin1Verified)
        {
            *pucType = USIMM_PUK;

            *pucNum  = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.ucPuk1RemainTime);
			*pucBeNeed = USIMM_PUK_BLOCK;

            return USIMM_API_SUCCESS;
        }
        else
        {
            USIMM_WARNING_LOG("USIMM_GetPinStatus: The Card Status is Wrong");

            return USIMM_API_NOTAVAILABLE;
        }
    }

    if( USIMM_SIM_NON == *pucType )
    {
        USIMM_QueryPINType(pucType);
    }

    enPINType = *pucType;

    switch(enPINType)
    {
        /* 获取PIN1信息 */
        case USIMM_PIN:
            *pucBeEnable = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.enPin1Enable);
            *pucBeNeed   = (VOS_UINT8)(gstUSIMMADFInfo.enPin1Verified);
            *pucNum      = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.ucPin1RemainTime);
            break;

        /* 获取PIN2信息 */
        case USIMM_PIN2:
            *pucBeEnable = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.enPin2Enable);
            *pucBeNeed   = USIMM_PIN_NEED;
            *pucNum      = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.ucPin2RemainTime);
            break;

        /* 获取PUK1信息目前返回剩余次数 */
        case USIMM_PUK:
            *pucNum      = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.ucPuk1RemainTime);
            break;

        /* 获取PUK2(信息目前返回剩余次数 */
        case USIMM_PUK2:
            *pucNum      = (VOS_UINT8)(gstUSIMMADFInfo.stPINInfo.ucPuk2RemainTime);
            break;

        default:
            USIMM_WARNING_LOG("USIMM_GetPinStatus: The PIN Type is Wrong");
            return USIMM_API_WRONG_PARA;
    }

    return USIMM_API_SUCCESS;
}


VOS_UINT32 USIMM_GetCardType(VOS_UINT8 *pucCardStatus, VOS_UINT8 *pucCardType)
{
    if(VOS_NULL_PTR != pucCardStatus)
    {
        *pucCardStatus = (VOS_UINT8)gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService;
    }

    if(VOS_NULL_PTR != pucCardType)
    {
/*Modified by ouyangfei for DART,2010-2-4,begin*/
#if defined (__PS_WIN32_RECUR__) || defined (__DART__)
        *pucCardType   =   g_ucCardType;
#else
        *pucCardType   = (VOS_UINT8)gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType;
#endif
/*Modified by ouyangfei for DART,2010-2-4,end*/
    }

    return USIMM_API_SUCCESS;
}



VOS_UINT32 USIMM_GetCachedFile(
    VOS_UINT16                          usFileID,
    VOS_UINT32                         *pulDataLen,
    VOS_UINT8                         **ppucData,
    USIMM_APP_TYPE_ENUM_UINT32          enAppType)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulRecordNum;

    if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
    {
        USIMM_WARNING_LOG("USIMM_GetCachedFile: The Open Speed Mode is Not Support it");

        return USIMM_API_FAILED;
    }

    if(gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService != USIMM_CARD_SERVIC_AVAILABLE)/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_GetCachedFile: The Card Status is Wrong");

        return USIMM_API_FAILED;
    }

    if((USIMM_POOL_NOINTIAL == gstUSIMMPOOL.enPoolStatus)
        ||(0 == gstUSIMMPOOL.ucNowLen)
        ||(0 == usFileID))   /*状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_GetCachedFile: Parameter wrong");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    /* 查询文件位置 */
    ulResult    =   USIMM_PoolFindFile(usFileID, &ulRecordNum, enAppType);

    if(VOS_ERR == ulResult)
    {
        USIMM_ERROR_LOG("USIMM_GetCachedFile: File Could not Found");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    if(VOS_NULL_PTR == gstUSIMMPOOL.astpoolRecord[ulRecordNum].pucContent)
    {
        USIMM_ERROR_LOG("USIMM_PoolReadOneFile: File Content is Empty");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    *pulDataLen = gstUSIMMPOOL.astpoolRecord[ulRecordNum].usLen;

    *ppucData = gstUSIMMPOOL.astpoolRecord[ulRecordNum].pucContent;

    return USIMM_API_SUCCESS;
}


/*old API 2阶段删除或者修改替换*/

VOS_UINT32 USIMM_GetCardIMSI(VOS_UINT8 *pucImsi)
{
    VOS_UINT32  ulResult;

    if(VOS_NULL_PTR == pucImsi)
    {
        USIMM_WARNING_LOG("USIMM_GetCardIMSI: The Parameter is Error");

        return USIMM_API_WRONG_PARA;
    }

    if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
    {
        ulResult = NV_Read(en_NV_Item_EF6F07, (VOS_VOID *)pucImsi, USIMM_EF_IMSI_LEN);

        return ((ulResult == VOS_OK)?USIMM_API_SUCCESS:USIMM_API_FAILED);/* [false alarm]: 屏蔽Fortify 错误 */
    }

    if(gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService != USIMM_CARD_SERVIC_AVAILABLE)/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_GetCardIMSI: The Card Status is Wrong");

        return USIMM_API_FAILED;
    }

    if(VOS_OK == USIMM_PoolReadOneFile(EFIMSI, USIMM_EF_IMSI_LEN, pucImsi, USIMM_UNLIMIT_APP))
    {
        return USIMM_API_SUCCESS;
    }

    return USIMM_API_FAILED;
}


/*****************************************************************************
函 数 名  : USIMM_GetPinRemainTime
功能描述  : 获取pin码操作剩余次数
输入参数  :
输出参数  : 无
返 回 值  : VOS_UINT32，表示函数执行结果
调用函数  :
被调函数  : 外部接口
*****************************************************************************/
VOS_UINT32 USIMM_GetPinRemainTime(USIMM_PIN_REMAIN_STRU *pstRemainTime)
{
    pstRemainTime->ucPin1RemainTime = gstUSIMMADFInfo.stPINInfo.ucPin1RemainTime;
    pstRemainTime->ucPin2RemainTime = gstUSIMMADFInfo.stPINInfo.ucPin2RemainTime;
    pstRemainTime->ucPuk1RemainTime = gstUSIMMADFInfo.stPINInfo.ucPuk1RemainTime;
    pstRemainTime->ucPuk2RemainTime = gstUSIMMADFInfo.stPINInfo.ucPuk2RemainTime;

    return USIMM_API_SUCCESS;
}


VOS_VOID USIMM_BdnQuery(VOS_UINT32 *pulState)
{
    if(VOS_NULL_PTR != pulState)
    {
        *pulState = gstUSIMMBaseInfo.enBDNStatus;
    }

    return ;
}


VOS_VOID USIMM_FdnQuery(VOS_UINT32 *pulState)
{
    if(VOS_NULL_PTR != pulState)
    {
        *pulState = gstUSIMMBaseInfo.enFDNStatus;
    }

    return ;
}


VOS_UINT32 USIMM_SetSPBFileReq(VOS_UINT32 ulSenderPid,USIMM_SPB_API_STRU *pstSPBReq)
{
    VOS_UINT32                  i;
    USIMM_SET_SPBFILE_REQ_STRU  *pMsg;
    VOS_UINT32                  ulResult;

    if(VOS_NULL_PTR == pstSPBReq)
    {
        USIMM_WARNING_LOG("USIMM_SetSPBFileReq: The Parameter is Wrong");
        return USIMM_API_WRONG_PARA;
    }

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SetSPBFileReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    if(0 == pstSPBReq->usFileNum)/*没有需要更新的电话本文件*/
    {
        USIMM_WARNING_LOG("USIMM_SetSPBFileReq: No file to update");

        return USIMM_API_WRONG_PARA;
    }

    pMsg = (USIMM_SET_SPBFILE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                        sizeof(USIMM_SET_SPBFILE_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_SetSPBFileReq: VOS_AllocMsg is Failed");

        return USIMM_API_ALLOCMEM_FAILED;
    }

    for(i = 0; i < pstSPBReq->usFileNum; i++)
    {
        pMsg->stSPBReq[i].usFileID    = pstSPBReq->usFileID[i];
        pMsg->stSPBReq[i].ucRecordNum = pstSPBReq->ucRecordNum[i];
        pMsg->stSPBReq[i].usDataLen   = pstSPBReq->usDataLen[i];
        pMsg->stSPBReq[i].enAppType = USIMM_PB_APP;

        if(VOS_NULL_PTR == pstSPBReq->pContent[i])
        {
            USIMM_WARNING_LOG("USIMM_SetSPBFileReq: Req Content Empty");
            VOS_FreeMsg(WUEPS_PID_USIM, pMsg);
            return USIMM_API_WRONG_PARA;
        }

        VOS_MemCpy(pMsg->stSPBReq[i].aucContent, pstSPBReq->pContent[i], pstSPBReq->usDataLen[i]);
    }

    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_SPBFILE_REQ;
    pMsg->ulSPBReqCount     = pstSPBReq->usFileNum;

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_VOID USIMM_PBInitStatusInd(USIMM_PB_INIT_STATUS_ENUM_UINT16 enPBInitStatus)
{
    USIMM_PB_INIT_STATUS_IND_STRU *pMsg;

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_PBInitStatusInd: The vSIM is Open");

        return;
    }

    pMsg = (USIMM_PB_INIT_STATUS_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                sizeof(USIMM_PB_INIT_STATUS_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_ERROR_LOG("USIMM_PBInitStatusInd: VOS_AllocMsg is Failed");

        return ;
    }

    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = MAPS_PB_PID;
    pMsg->enMsgType         = USIMM_CMDTYPE_PBINIT_IND;
    pMsg->enPBInitStatus    = enPBInitStatus;

    USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);

    return ;
}

/*****************************************************************************
函 数 名  :USIMM_DeactiveCardReq
功能描述  :SIM卡去激活请求
输入参数  :ucClientId:调用者ID
输出参数  :无
返 回 值  :VOS_OK
           VOS_ERR
调用函数  :VOS_AllocMsg
           VOS_SendMsg
被调函数  :
修订记录  :
1. 日    期   : 2010年 7 月 26日
   作    者   : zhuli
   修改内容   : Creat
*****************************************************************************/
VOS_UINT32 USIMM_DeactiveCardReq(VOS_UINT32 ulSenderPid)
{
    USIMM_DEACTIVE_CARD_REQ_STRU        *pMsg;
    VOS_UINT32                          ulResult;
    OAM_MNTN_SIM_CARD_LOST_EVENT_STRU   stSimCardLost;
    VOS_UINT32                          ulErrlogNumber;

    if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_DeactiveCardReq: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    /* 因为双开双待所以高16位记录pid,低16位记录原来的id */
    ulErrlogNumber = ulSenderPid;
    ulErrlogNumber <<= 16;
    ulErrlogNumber |= MNTN_OAM_SIM_CARD_LOST_EVENT;

    /* 在Errorlog文件中记录SIM卡掉卡事件 */
    if(VOS_OK != MNTN_RecordErrorLog(ulErrlogNumber, (void *)&stSimCardLost,
                                            sizeof(OAM_MNTN_SIM_CARD_LOST_EVENT_STRU)))
    {
        USIMM_WARNING_LOG("USIMM_DeactiveCardReq: Fail to record  SIM card lost event in Errorlog file");
    }

    pMsg = (USIMM_DEACTIVE_CARD_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                (sizeof(USIMM_DEACTIVE_CARD_REQ_STRU)-VOS_MSG_HEAD_LENGTH));

    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_DeactiveCardReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_DEACTIVE_REQ;

    ulResult = VOS_SendMsg(pMsg->ulSenderPid, pMsg);

    if(VOS_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_DeactiveCardReq: VOS_SendMsg is Failed");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    return USIMM_API_SUCCESS;
}

/*****************************************************************************
函 数 名  :USIMM_ActiveCardReq
功能描述  :SIM卡激活请求
输入参数  :ulSenderPid:发送请求的PID
输出参数  :无
返 回 值  :VOS_OK
           VOS_ERR
被调函数  :
修订记录  :
1. 日    期   : 2010年 7 月 26日
   作    者   : zhuli
   修改内容   : Creat
*****************************************************************************/
VOS_UINT32 USIMM_ActiveCardReq(VOS_UINT32 ulSenderPid)
{
    VOS_UINT32                          ulResult;

    if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_ActiveCardReq: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    ulResult = USIMM_SendInitCardMsg(ulSenderPid, USIMM_CMDTYPE_INITSTART);/* [false alarm]:屏蔽Fortify错误 */

    ulResult = USIMM_SendInitCardMsg(ulSenderPid, USIMM_CMDTYPE_INITISIM_REQ);/* [false alarm]:屏蔽Fortify错误 */

    if(VOS_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_ActiveCardReq: VOS_SendMsg is Failed");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    return USIMM_API_SUCCESS;
}


VOS_UINT32 USIMM_ProtectResetReq(VOS_UINT32 ulSenderPid)
{
    USIMM_PROTECT_RESET_REQ_STRU    *pMsg;
    VOS_UINT32                      ulResult;

    if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)/*卡状态检查*/
    {
        USIMM_WARNING_LOG("USIMM_ProtectResetReq: The Card Status is Wrong");

        return USIMM_API_NOTAVAILABLE;
    }

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_ProtectResetReq: The vSIM is Open");

        return USIMM_API_NOTAVAILABLE;
    }

    pMsg = (USIMM_PROTECT_RESET_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                (sizeof(USIMM_PROTECT_RESET_REQ_STRU)-VOS_MSG_HEAD_LENGTH));
    if(VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_ProtectResetReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /*填充消息内容*/
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_PROTECTRESET_REQ;

    ulResult = VOS_SendMsg(pMsg->ulSenderPid, pMsg);

    if(VOS_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_ProtectResetReq: VOS_SendMsg is Failed");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    return USIMM_API_SUCCESS;
}


VOS_UINT32  USIMM_SearchReq(VOS_UINT32                   ulSenderPid,
                                    USIMM_APP_TYPE_ENUM_UINT32  enAppType,
                                    USIMM_U8_LVDATA_STRU        *pstData,
                                    VOS_UINT16                  usFileId)
{
    USIMM_SEARCH_REQ_STRU       *pMsg;
    VOS_UINT32                  ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SearchReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    pMsg = (USIMM_SEARCH_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                (sizeof(USIMM_SEARCH_REQ_STRU)-VOS_MSG_HEAD_LENGTH)+pstData->ulDataLen);
    if (VOS_NULL_PTR == pMsg)
    {
        USIMM_WARNING_LOG("USIMM_SearchReq: VOS_AllocMsg is Failed");/* 打印错误 */

        return USIMM_API_ALLOCMEM_FAILED; /* 返回函数错误信息 */
    }

    /* 填充消息内容 */
    pMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pMsg->ulSenderPid       = ulSenderPid;
    pMsg->enMsgType         = USIMM_CMDTYPE_SEARCH_REQ;
    pMsg->enAppType         = enAppType;
    pMsg->usEfId            = usFileId;
    pMsg->ucLen             = (VOS_UINT8)pstData->ulDataLen;

	VOS_MemCpy(pMsg->aucContent, pstData->pucData, pstData->ulDataLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pMsg,USIMM_API_MSG_NORMAL);
}
VOS_VOID USIMM_SCICardOutNotify(VOS_UINT ulCardInOutStatus)
{
    USIMM_CARD_INOUT_IND_STRU           *pUsimMsg;

    LogPrint1("USIMM_SCICardOutNotify: The card status is %d .\r\n", (VOS_INT)ulCardInOutStatus);

    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        USIMM_WARNING_LOG("USIMM_SCICardOutNotify: The vSIM is Open");

        return;
    }

    pUsimMsg = (USIMM_CARD_INOUT_IND_STRU*)VOS_AllocMsg(WUEPS_PID_USIM,
                                                sizeof(USIMM_CARD_INOUT_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_CardInOutHandle: VOS_AllocMsg is Failed");

        return;
    }

    pUsimMsg->enMsgType         = USIMM_CMDTYPE_CARDINOUT_IND;
    pUsimMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pUsimMsg->ulSenderPid       = WUEPS_PID_USIM;
    pUsimMsg->ulCardInOutStatus = (VOS_UINT32)ulCardInOutStatus;

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_USIM, pUsimMsg);

    return;
}
VOS_VOID USIMM_SciCardStatusIndCbFunc(USIMM_CARDSTATUS_IND_ENUM_UINT32 enCardStatus)
{
    USIMM_CARD_STATUS_IND_STRU         *pstMsg;

    if (VOS_TRUE == USIMM_VsimIsActive())    /*vSIM功能打开*/
    {
        return;
    }

    pstMsg = (USIMM_CARD_STATUS_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                       sizeof(USIMM_CARD_STATUS_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_SciCardStatusIndCbFunc: VOS_AllocMsg is Failed");

        return;
    }

    pstMsg->enMsgType         = USIMM_CMDTYPE_CARDSTATUS_IND_CB;
    pstMsg->ulReceiverPid     = WUEPS_PID_USIM;
    pstMsg->ulSenderPid       = MAPS_PIH_PID;
    pstMsg->ulCardStatus      = enCardStatus;

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_USIM, pstMsg);

    return;
}
VOS_UINT32 USIMM_IsdbAccessReq(VOS_UINT32 ulSenderPid, VOS_UINT16 usLen, VOS_UINT8 *pucCommand)
{
    USIMM_ISDB_ACCESS_REQ_STRU         *pstMsg;
    VOS_UINT32                          ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    /* 检查消息队列数量 */
    if (gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM)
    {
        USIMM_WARNING_LOG("USIMM_IsdbAccessReq: The Msg Queue is Full");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    /* 参数检查 */
    if ((0 == usLen)||(VOS_NULL_PTR == pucCommand))
    {
        USIMM_WARNING_LOG("USIMM_IsdbAccessReq: The Parameter is Wrong");

        return USIMM_API_WRONG_PARA;
    }

    /* 申请内存 */
    pstMsg = (USIMM_ISDB_ACCESS_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                        (sizeof(USIMM_ISDB_ACCESS_REQ_STRU) - VOS_MSG_HEAD_LENGTH) + usLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_IsdbAccessReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /* 填充消息内容 */
    pstMsg->ulReceiverPid               =   WUEPS_PID_USIM;
    pstMsg->ulSenderPid                 =   ulSenderPid;
    pstMsg->enMsgType                   =   USIMM_CMDTYPE_ISDBACC_REQ;
    pstMsg->usDataLen                   =   usLen;

    VOS_MemCpy(pstMsg->aucContent, pucCommand, usLen);

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_OpenChannelReq(VOS_UINT32 ulSenderPid,
                                                VOS_UINT32 ulAIDLen,
                                                VOS_UINT8 *pucADFName)
{
    USIMM_OPEN_CHANNEL_REQ_STRU        *pstMsg;
    VOS_UINT32                          ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }
    /* 卡状态检查 */
    if ((USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
        || (gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM))
    {
        USIMM_WARNING_LOG("USIMM_OpenChannelReq: The Card Status is Wrong");

        return USIMM_API_SENDMSG_FAILED;
    }

    /* 参数判断 */
    if (ulAIDLen > (2*USIMM_AID_LEN_MAX))
    {
        USIMM_WARNING_LOG("USIMM_OpenChannelReq: the length of AID is too long.");

        return USIMM_API_WRONG_PARA;
    }

    /* 申请内存 */
    pstMsg = (USIMM_OPEN_CHANNEL_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                        (sizeof(USIMM_OPEN_CHANNEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannelReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /* 填充消息内容 */
    pstMsg->ulReceiverPid               =   WUEPS_PID_USIM;
    pstMsg->ulSenderPid                 =   ulSenderPid;
    pstMsg->enMsgType                   =   USIMM_CMDTYPE_OPENCHANNEL_REQ;
    pstMsg->stChannelInfo.ulAIDLen      =   ulAIDLen;

    if (0 != ulAIDLen)
    {
        VOS_MemCpy(pstMsg->stChannelInfo.aucADFName, pucADFName, ulAIDLen);
    }

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_CloseChannelReq(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSessionID)
{
    USIMM_CLOSE_CHANNEL_REQ_STRU        *pstMsg;
    VOS_UINT32                          ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }
    /* 卡状态检查 */
    if ((USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
        || (gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM))
    {
        USIMM_WARNING_LOG("USIMM_CloseChannelReq: The Card Status is Wrong");

        return USIMM_API_SENDMSG_FAILED;
    }

    /* 申请内存 */
    pstMsg = (USIMM_CLOSE_CHANNEL_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                        (sizeof(USIMM_CLOSE_CHANNEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_CloseChannelReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /* 填充消息内容 */
    pstMsg->ulReceiverPid               =   WUEPS_PID_USIM;
    pstMsg->ulSenderPid                 =   ulSenderPid;
    pstMsg->enMsgType                   =   USIMM_CMDTYPE_CLOSECHANNEL_REQ;
    pstMsg->ulSessionID                 =   ulSessionID;

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_NORMAL);
}
VOS_UINT32 USIMM_AccessChannelReq(VOS_UINT32 ulSenderPid,
                    VOS_UINT32 ulSessionID, VOS_UINT32 ulDataLen, VOS_UINT8 *pucData)
{
    USIMM_ACCESS_CHANNEL_REQ_STRU      *pstMsg;
    VOS_UINT32                          ulResult;

    ulResult = USIMM_ApiParaCheck();

    if(USIMM_API_SUCCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_MaxEFRecordNumReq: USIMM_ApiParaCheck is Failed");/*打印错误*/

        return ulResult; /*返回函数错误信息*/
    }

    /* 检查卡状态和消息队列数量 */
    if ((USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
         || (gulUSIMMAPIMessageNum > USIMM_MAX_MSG_NUM))
    {
        USIMM_WARNING_LOG("USIMM_AccessChannelReq: The Msg Queue is Full");/*打印错误*/

        return USIMM_API_SENDMSG_FAILED;
    }

    /* 数据内容为空 */
    if ((USIMM_APDU_LEN_MIN > ulDataLen) || (USIMM_APDU_LEN_MAX < ulDataLen))
    {
        USIMM_WARNING_LOG("USIMM_AccessChannelReq: ulDataLen is incorrect.");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    /* 申请内存 */
    pstMsg = (USIMM_ACCESS_CHANNEL_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_USIM,
                                                        (sizeof(USIMM_ACCESS_CHANNEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH) + ulDataLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /* 填充消息内容 */
    pstMsg->ulReceiverPid               =   WUEPS_PID_USIM;
    pstMsg->ulSenderPid                 =   ulSenderPid;
    pstMsg->enMsgType                   =   USIMM_CMDTYPE_ACCESSCHANNEL_REQ;
    pstMsg->ulSessionID                 =   ulSessionID;
    pstMsg->ulDataLen                   =   ulDataLen;

    /*lint -e669*/
    VOS_MemCpy(pstMsg->aucData, pucData, ulDataLen);
    /*lint +e669*/

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_NORMAL);
}


VOS_UINT32 USIMM_SendTPDUReq(
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulSessionID,
    USIMM_FILEPATH_INFO_STRU           *pstFilePath,
    USIMM_TPDU_DATA_STRU               *pstTPDU)
{
    USIMM_SENDTPDU_REQ_STRU            *pstMsg;

    if (VOS_NULL_PTR == pstTPDU)
    {
        USIMM_WARNING_LOG("USIMM_SendTPDUReq: Input Para is incorrect.");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    /* 数据内容为空 */
    if (USIMM_TPDU_DATA_LEN_MAX < pstTPDU->ulDataLen)
    {
        USIMM_WARNING_LOG("USIMM_SendTPDUReq: ulDataLen is incorrect.");/*打印错误*/

        return USIMM_API_WRONG_PARA;
    }

    /* 申请内存 */
    pstMsg = (USIMM_SENDTPDU_REQ_STRU *)VOS_AllocMsg(ulSenderPid,
                                                    (sizeof(USIMM_SENDTPDU_REQ_STRU) - VOS_MSG_HEAD_LENGTH) +  pstTPDU->ulDataLen - 4);

    if (VOS_NULL_PTR == pstMsg)
    {
        USIMM_ERROR_LOG("USIMM_SendTPDUReq: VOS_AllocMsg is Failed");/*打印错误*/

        return USIMM_API_ALLOCMEM_FAILED; /*返回函数错误信息*/
    }

    /* 填充消息内容 */
    pstMsg->ulReceiverPid = WUEPS_PID_USIM;
    pstMsg->enMsgType     = USIMM_CMDTYPE_SENDTPDU_REQ;
    pstMsg->ulSessionID   = ulSessionID;
    pstMsg->ulTPDUDataLen = pstTPDU->ulDataLen;

    VOS_MemCpy(pstMsg->aucTPDUHead, pstTPDU->aucTPDUHead, sizeof(pstMsg->aucTPDUHead));

    /*lint -e669*/
    VOS_MemCpy(pstMsg->aucTPDUData, pstTPDU->pucData, pstTPDU->ulDataLen);
    /*lint +e669*/

    if (VOS_NULL_PTR != pstFilePath)
    {
        VOS_MemCpy(&pstMsg->stFilePath, pstFilePath, sizeof(USIMM_FILEPATH_INFO_STRU));
    }
    else
    {
        VOS_MemSet(&pstMsg->stFilePath, 0, sizeof(USIMM_FILEPATH_INFO_STRU));
    }

    return USIMM_ApiSendMsg((USIMM_MsgBlock*)pstMsg, USIMM_API_MSG_NORMAL);
}
VOS_VOID USIMM_SessionIdToChanNum(VOS_UINT32 ulSessionId, VOS_UINT32 *pulChanNum)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < USIMM_CHANNEL_NUM_MAX; ucIndex++)
    {
        if (ulSessionId == g_astUSIMMChCtrl[ucIndex].ulSessionId)
        {
            *pulChanNum = g_astUSIMMChCtrl[ucIndex].ulChanNum;

            return;
        }
    }

    *pulChanNum = USIMM_CHANNEL_INVALID_NUM;

    return;
}


VOS_VOID USIMM_ChanNumToSessionId(VOS_UINT32 ulChanNum, VOS_UINT32 *pulSessionId)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < USIMM_CHANNEL_NUM_MAX; ucIndex++)
    {
        if (ulChanNum == g_astUSIMMChCtrl[ucIndex].ulChanNum)
        {
            *pulSessionId = g_astUSIMMChCtrl[ucIndex].ulSessionId;

            return;
        }
    }

    *pulSessionId = USIMM_SESSION_ID_MAX;

    return;
}


VOS_UINT32 USIMM_GetUiccChanNumByAid(VOS_UINT8 *pucAid, VOS_UINT32 ulAIDLen, VOS_UINT8 *pucChanNum)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < USIMM_CHANNEL_NUM_MAX; ucIndex++)
    {
        if (VOS_OK == VOS_MemCmp(pucAid, g_astUSIMMChCtrl[ucIndex].aucADFName, ulAIDLen))
        {
            *pucChanNum = (VOS_UINT8)g_astUSIMMChCtrl[ucIndex].ulChanNum;

            return VOS_OK;
        }
    }

    *pucChanNum = 0xff;

    return VOS_ERR;
}


VOS_UINT32 USIMM_GetAid(VOS_UINT32 ulAidType, VOS_UINT32 *pulLen, VOS_UINT8 *pucAid)
{
    if ((VOS_NULL_PTR == pulLen) || (VOS_NULL_PTR == pucAid))
    {
        return USIMM_API_WRONG_PARA;
    }

    if (ulAidType >= USIMM_AID_TYPE_BUTT)
    {
        return USIMM_API_WRONG_PARA;
    }

    if ((VOS_NULL == g_astAidInfo[ulAidType].ulAIDLen)
        || (g_astAidInfo[ulAidType].ulAIDLen > USIMM_AID_LEN_MAX))
    {
        return USIMM_API_NOTAVAILABLE;
    }

    VOS_MemCpy(pucAid, g_astAidInfo[ulAidType].aucAID, g_astAidInfo[ulAidType].ulAIDLen);

    *pulLen = g_astAidInfo[ulAidType].ulAIDLen;

    return USIMM_API_SUCCESS;
}



VOS_UINT32 USIMM_GetCardCGType(VOS_UINT8 *pucCardMode, VOS_UINT8 *pucHasCModule, VOS_UINT8 *pucHasWGModule)
{
    if ((VOS_NULL_PTR == pucCardMode) || (VOS_NULL_PTR == pucHasCModule) || (VOS_NULL_PTR == pucHasWGModule))
    {
        return USIMM_API_WRONG_PARA;
    }

    *pucCardMode    = g_stUSIMMCardVersionType.enCardMode;
    *pucHasCModule  = g_stUSIMMCardVersionType.ucHasCModule;
    *pucHasWGModule = g_stUSIMMCardVersionType.ucHasWGModule;

    return USIMM_API_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

