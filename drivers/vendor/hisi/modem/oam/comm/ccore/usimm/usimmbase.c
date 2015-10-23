
/*lint --e{740,958,537,767,718,746,701,438}*/

#include "vos_Id.h"
#include "usimmbase.h"
#include "DrvInterface.h"
#include "usimmapdu.h"
#include "usimmdl.h"
#include "om.h"
#include "siappstk.h"
#include "NvIdList.h"
#include "usimmt1dl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID PS_FILE_ID_USIMM_BASE_C



VOS_UINT32 USIMM_RefreshHandle(USIMM_MsgBlock *pMsg)
{
    USIMM_REFRESH_REQ_STRU             *pstMsg;
    VOS_UINT8                           aucTemp[] = {0x81,0x03,0x00,0x00,0x00,0x02,0x02,0x82,0x81,0x83,0x01,0x00};
    USIMM_MODE_TYPE_ENUM_UINT8          ucMode;


    pstMsg = (USIMM_REFRESH_REQ_STRU*)pMsg;

    USIMM_PoolDelAll();

    if (USIMM_RESET == pstMsg->enRefreshType)
    {
        USIMM_INFO_LOG("USIMM_RefreshHandle: The Card Need Cold Reset.");

        if(USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
        {
            USIMM_Status_APDU(USIMM_STATUS_TERMINATION_CURAPP, USIMM_STATUS_DF_NAME_RETURNED, 0xFF);
        }

        USIMM_InitGlobalVarOnPower();

        USIMM_InitCard(VOS_NULL_PTR);

        USIMM_InitIsimHandle(VOS_NULL_PTR);
    }
    else
    {
        if (VOS_OK != USIMM_RefreshAidVerify(pstMsg->ulAidLen, pstMsg->aucAid))
        {
            aucTemp[2]  = (VOS_UINT8)pstMsg->ulCommandNum;
            aucTemp[3]  = (VOS_UINT8)pstMsg->ulCommandType;
            aucTemp[4]  = (VOS_UINT8)pstMsg->enRefreshType;
            aucTemp[11] = COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;

            /* 为了规避重启过程的主动命令，此处需要主动命令执行结果 */
            USIMM_TerminalResponse_APDU(sizeof(aucTemp), aucTemp);

            /* 回复操作结果 */
            USIMM_ResetCnf(pstMsg->ulSenderPid, pstMsg->enRefreshType, VOS_ERR, gstUSIMMBaseInfo.usSATLen);

            return VOS_OK;
        }

        if ((USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
         && (USIMM_APPLICATION_RESET_3G_ONLY == pstMsg->enRefreshType))
        {
            USIMM_Status_APDU(USIMM_STATUS_TERMINATION_CURAPP, USIMM_STATUS_DF_NAME_RETURNED, 0xFF);

            USIMM_Reselect3GApp();
        }

        USIMM_InsertNoNeedPinFileToPool();

        /* 初始化ATT定制文件，其不受PIN码控制 */
        USIMM_AttFileInit();

        if(USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
        {
            USIMM_InsertLPFileToPool();

            USIMM_InitUsimEccFile();
        }
        else
        {
            USIMM_InsertLPFileToPool();

            USIMM_InitSimEccFile();
        }

        /* SIM卡下，在refresh type为0/2/3情况下，需要下发Terminal Profile，refresh type为1不需要；
            USIM卡下都不需要下发Terminal Profile */
        if (USIMM_SIM_REFRESH_IS_NEED_TP(pstMsg->enRefreshType))
        {
            ucMode = USIMM_RESET_MODE;
        }
        else
        {
            ucMode = USIMM_REFRESH_MODE;
        }

        if (USIMM_CARD_SERVIC_AVAILABLE == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService)
        {
            USIMM_InitNeedPinSteps(ucMode);

            USIMM_InitCardOptionalFile(VOS_NULL_PTR);
        }

        if(((USIMM_FILE_CHANGE_NOTIFY == pstMsg->enRefreshType)||(USIMM_INITIAL_FILE_CHANGE_NOTIFY == pstMsg->enRefreshType))
            &&(pstMsg->usLen != 0))
        {
            USIMM_RefreshFileRead(pstMsg);
        }

        aucTemp[2]  = (VOS_UINT8)pstMsg->ulCommandNum;
        aucTemp[3]  = (VOS_UINT8)pstMsg->ulCommandType;
        aucTemp[4]  = (VOS_UINT8)pstMsg->enRefreshType;

        /*为了规避重启过程的主动命令，此处需要主动命令执行结果*/
        USIMM_TerminalResponse_APDU(sizeof(aucTemp), aucTemp);

        USIMM_RefreshCardStatusInd();
    }

    USIMM_ResetCnf(pstMsg->ulSenderPid, pstMsg->enRefreshType, VOS_OK, gstUSIMMBaseInfo.usSATLen); /*回复操作结果*/

    USIMM_RefreshInd(pstMsg);

    return VOS_OK;
}


VOS_VOID USIMM_SAT_Fetch(VOS_UINT16 usLen)
{
    USIMM_APP_TYPE_ENUM_UINT32          enAppType;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulOffset;
    VOS_UINT8                           ucCmdType;
    VOS_UINT8                           ucP3;
    VOS_UINT8                           aucData[USIMM_EF_IMSI_LEN];

    if (USIMM_SAT_STATE_NEEDRESPONSE == gstUSIMMBaseInfo.enSATState)
    {
        USIMM_ERROR_LOG("USIMM_SAT_Fetch: The Last Data Need Response");

        return;
    }

    if(usLen >  0x100)
    {
        gstUSIMMBaseInfo.usSATLen   = 0;

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

        USIMM_ERROR_LOG("USIMM_SAT_Fetch: The Data Len is Error");

        return;
    }
    else
    {
        ucP3 = (VOS_UINT8)usLen;
    }

    ulResult = USIMM_Fetch_APDU(ucP3);

    if(VOS_OK != ulResult)/*判断结果*/
    {
        USIMM_ERROR_LOG("USIMM_SAT_Fetch: Send APUD Command is Failed");    /*打印错误*/

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

        gstUSIMMBaseInfo.usSATLen   = 0;

        return;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if(USIMM_SW_OK != ulResult)/*判断结果*/
    {
        USIMM_WARNING_LOG("USIMM_SAT_Fetch: The Send APDU Result is Error");/*打印错误*/

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

        gstUSIMMBaseInfo.usSATLen   = 0;

        return;
    }

    /*查询当前的主动命令类型*/
    ulResult = USIMM_FindTagInSMPTLV(gstUSIMMAPDU.aucRecvBuf, USIMM_SATCMD_TAG, usLen);

    if(USIMM_TAGNOTFOUND == ulResult)/*判断结果*/
    {
        USIMM_WARNING_LOG("USIMM_SAT_Fetch: The Command Type Tag is Not Founded in the SAT Data ");  /*打印错误*/

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

        gstUSIMMBaseInfo.usSATLen   = 0;

        return;
    }

    ulOffset = USIMM_FindTagInSMPTLV(&gstUSIMMAPDU.aucRecvBuf[ulResult+1],USIMM_CMDDETAIL_TAG,
                                    gstUSIMMAPDU.aucRecvBuf[ulResult]);

    if(USIMM_TAGNOTFOUND == ulOffset)/*判断结果*/
    {
        USIMM_WARNING_LOG("USIMM_SAT_Fetch: The Command Detail Tag is Not Founded in the SAT Data");    /*打印错误*/

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

        gstUSIMMBaseInfo.usSATLen   = 0;

        return;
    }

    ulOffset += ulResult;

    ucCmdType = gstUSIMMAPDU.aucRecvBuf[ulOffset+3];

    USIMM_INFO_LOG("USIMM_SAT_Fetch: IND SAT Command, Wait Terminal Response");

    gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NEEDRESPONSE;

    USIMM_SatDataInd(ucCmdType, usLen, gstUSIMMAPDU.aucRecvBuf);/*上报主动命令的内容*/

    gstUSIMMBaseInfo.usSATLen = 0;

    enAppType   =   ((USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)?USIMM_GSM_APP:USIMM_UMTS_APP);

    /* 如果是0x01 Refresh主动命令上报，可能卡内IMSI发生变更，需要更新内存 */
    if((0x01 == ucCmdType)&&(USIMM_NONEED == gstUSIMMADFInfo.enPin1Verified))
    {
        if(VOS_OK == USIMM_PoolDelOneFile(EFIMSI, enAppType))
        {
            /* 读IMSI */
            if(VOS_OK != USIMM_GetTFFile(enAppType, EFIMSI, USIMM_READ_ALL_FILE_CONTENT, aucData))
            {
                USIMM_ERROR_LOG("USIMM_SAT_Fetch: Fail to Read IMSI");

                return;
            }

            USIMM_PoolInsertOneFile(EFIMSI, USIMM_EF_IMSI_LEN, aucData,
                                          enAppType, USIMM_EFSTRUCTURE_TRANSPARENT);
        }
        else
        {
            USIMM_ERROR_LOG("USIMM_SAT_Fetch: Can not find IMSI in Pool");

            return;
        }
    }

    return;
}


VOS_UINT32 USIMM_SAT_Envelope(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulSWRslt;
    USIMM_ENVELOPE_REQ_STRU            *pstMsg;
    USIMM_ENVELOPECNF_INFO_STRU         stCnfInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    pstMsg = (USIMM_ENVELOPE_REQ_STRU*)pMsg;

    stCnfInfo.ucDataType = pstMsg->aucContent[0];

    ulResult = USIMM_Envelope_APDU(pstMsg->ucDataLen, pstMsg->aucContent);  /*发送命令数据*/

    stCnfInfo.ucSW1 = gstUSIMMAPDU.ucSW1;

    stCnfInfo.ucSW2 = gstUSIMMAPDU.ucSW2;

    if(VOS_OK != ulResult)/*检查结果*/
    {
        USIMM_ERROR_LOG("USIMM_SAT_Envelope: Send APDU Command is Failed");

        USIMM_EnvelopeCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara,USIMM_SW_SENDCMD_ERROR,&stCnfInfo);

        return VOS_ERR;
    }

    ulSWRslt = USIMM_CheckSW(&gstUSIMMAPDU);

    if(USIMM_SW_OK != ulSWRslt)
    {
        USIMM_WARNING_LOG("USIMM_SAT_Envelope: check SW Result is Failed");   /* coverity by yangzhi */
    }

    /* 如果ENVELOPE命令返回WARNING */
    if ((0x62 == gstUSIMMAPDU.ucApduSW1)||(0x63 == gstUSIMMAPDU.ucApduSW1)||(0x9E == gstUSIMMAPDU.ucApduSW1))
    {
        stCnfInfo.ucSW1 = gstUSIMMAPDU.ucApduSW1;

        stCnfInfo.ucSW2 = gstUSIMMAPDU.ucApduSW2;

        if ((USIMM_ENVELOPE_PPDOWN == pstMsg->aucContent[0])
           || (USIMM_ENVELOPE_CBDOWN == pstMsg->aucContent[0]))
        {
            ulResult = USIMM_SW_DOWNLOAD_ERROR;
        }
        else
        {
            ulResult = ulSWRslt;    /* 其他按照命令的最后结果返回 */
        }
    }
    else
    {
        ulResult = ulSWRslt;
    }

    stCnfInfo.ulDataLen = gstUSIMMAPDU.ulRecDataLen;

    stCnfInfo.pucData   = gstUSIMMAPDU.aucRecvBuf;

    USIMM_EnvelopeCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara, ulResult, &stCnfInfo);

    if(USIMM_SW_OK != ulResult) /*需要带回具体原因，暂时返回错误*/
    {
        USIMM_WARNING_LOG("USIMM_SAT_Envelope: Send APDU Command Result is Failed");

        ulResult = VOS_ERR;
    }

    return ulResult;
}


VOS_UINT32 USIMM_SAT_TerminalResopnse(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                      ulResult;
    USIMM_TERMINALRESPONSE_REQ_STRU *pstMsg;

    pstMsg = (USIMM_TERMINALRESPONSE_REQ_STRU*)pMsg;

    ulResult = USIMM_TerminalResponse_APDU(pstMsg->ucDataLen, pstMsg->aucContent);

    if(VOS_OK != ulResult)/*检查发送结果*/
    {
        USIMM_ERROR_LOG("USIMM_SAT_TerminalResopnse: Send APDU Command is Failed");

        USIMM_TerminalResponseCnf(pstMsg->ulSenderPid, USIMM_SW_SENDCMD_ERROR, pstMsg->ulSendPara,0,0);

        return VOS_ERR;
    }

    gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NODATA;

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if(USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SAT_TerminalResopnse: check SW Result is Failed");   /* coverity by yangzhi */
    }

    USIMM_TerminalResponseCnf(pstMsg->ulSenderPid, ulResult, pstMsg->ulSendPara, gstUSIMMAPDU.ucSW1,gstUSIMMAPDU.ucSW2);

    if(USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SAT_TerminalResopnse: The Command Result is Error");

        ulResult = VOS_ERR;
    }

    return ulResult;
}



VOS_UINT32  USIMM_PinStatusCheck(USIMM_PIN_CMD_TYPE_ENUM_UINT32  enCmdType,
                                           USIMM_PIN_TYPE_ENUM_UINT32      enPINType,
                                           VOS_UINT32                      *pulResult)
{
    VOS_UINT32  ulResult;

    switch (enCmdType)
    {
        case USIMM_PINCMD_ENABLE:
            if(USIMM_PIN == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK_REQUIRED;
                }
                else if(USIMM_PIN_DISABLED == gstUSIMMADFInfo.stPINInfo.enPin1Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else if(USIMM_PIN2 == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK2_REQUIRED;
                }
                else if(USIMM_PIN_DISABLED == gstUSIMMADFInfo.stPINInfo.enPin2Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else
            {
                *pulResult = USIMM_OPERATION_NOT_ALLOW;
                ulResult = VOS_FALSE;
            }
            break;

        case USIMM_PINCMD_VERIFY:
            if(USIMM_PIN == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK_REQUIRED;
                }
                else if((USIMM_PIN_DISABLED == gstUSIMMADFInfo.stPINInfo.enPin1Enable)
                    ||(USIMM_NONEED == gstUSIMMADFInfo.enPin1Verified))
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_OPERATION_NOT_ALLOW;
                }
                else
                {
                    ulResult = VOS_TRUE;
                }
            }
            else if(USIMM_PIN2 == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK2_REQUIRED;
                }
                else if((USIMM_PIN_DISABLED == gstUSIMMADFInfo.stPINInfo.enPin2Enable)
                    ||(USIMM_NONEED == gstUSIMMADFInfo.enPin2Verified))
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_OPERATION_NOT_ALLOW;
                }
                else
                {
                    ulResult = VOS_TRUE;
                }
            }
            else
            {
                *pulResult = USIMM_OPERATION_NOT_ALLOW;
                ulResult = VOS_FALSE;
            }
            break;

        case USIMM_PINCMD_DISABLE:
            if(USIMM_PIN == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK_REQUIRED;
                }
                else if(USIMM_PIN_ENABLED == gstUSIMMADFInfo.stPINInfo.enPin1Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else if(USIMM_PIN2 == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK2_REQUIRED;
                }
                else if(USIMM_PIN_ENABLED == gstUSIMMADFInfo.stPINInfo.enPin2Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else
            {
                *pulResult = USIMM_OPERATION_NOT_ALLOW;
                ulResult = VOS_FALSE;
            }
            break;
        case USIMM_PINCMD_UNBLOCK:
            if(USIMM_PIN == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_OPERATION_NOT_ALLOW;
                }
            }
            else if(USIMM_PIN2 == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_OPERATION_NOT_ALLOW;
                }
            }
            else
            {
                ulResult = VOS_FALSE;
                *pulResult = USIMM_OPERATION_NOT_ALLOW;
            }
            break;
        case USIMM_PINCMD_CHANGE:
            if(USIMM_PIN == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin1Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK_REQUIRED;
                }
                else if(USIMM_PIN_ENABLED == gstUSIMMADFInfo.stPINInfo.enPin1Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else if(USIMM_PIN2 == enPINType)
            {
                if(USIMM_PUK_NEED == gstUSIMMADFInfo.enPin2Verified)
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_PUK2_REQUIRED;
                }
                else if(USIMM_PIN_ENABLED == gstUSIMMADFInfo.stPINInfo.enPin2Enable)
                {
                    ulResult = VOS_TRUE;
                }
                else
                {
                    ulResult = VOS_FALSE;
                    *pulResult = USIMM_SIM_FAILURE;
                }
            }
            else
            {
                ulResult = VOS_FALSE;
                *pulResult = USIMM_SIM_FAILURE;
            }
            break;
        case USIMM_PINCMD_PHNET:
        case USIMM_PINCMD_PHSIM:
            ulResult = VOS_TRUE;
            break;
        default:
            *pulResult = USIMM_OPERATION_NOT_ALLOW;
            ulResult = VOS_FALSE;
            break;
    }

    return ulResult;
}



VOS_UINT32 USIMM_PINHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32          ulResult = VOS_ERR;
    USIMM_PIN_REQ_STRU  *pstMsg;
    VOS_UINT16          ausUsimPath[] = {MF, ADF};
    VOS_UINT16          ausSimPath[] = {MF, DFGSM};

    pstMsg = (USIMM_PIN_REQ_STRU*)pMsg;

    if (USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        /* 解析ADF的密码索引 */
        ulResult = USIMM_SelectFile(USIMM_UMTS_APP, USIMM_NEED_FCP, ARRAYSIZE(ausUsimPath), ausUsimPath);
    }
    else
    {
        /* 解析DFGSM的密码索引 */
        ulResult = USIMM_SelectFile(USIMM_GSM_APP, USIMM_NEED_FCP, ARRAYSIZE(ausSimPath), ausSimPath);
    }

    if(ulResult != VOS_OK)/*检查文件的选中结果*/
    {
        USIMM_ERROR_LOG("USIMM_PINHandle: Select Current DF is Failed");

        USIMM_PinHandleCnf(pstMsg->ulSenderPid,pstMsg->enCmdType,pstMsg->enPINType,
                            VOS_ERR,&gstUSIMMADFInfo.stPINInfo);

        if(USIMM_SW_SENDCMD_ERROR == ulResult)
        {
            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType        = USIMM_CARD_NOCARD;
            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService     = USIMM_CARD_SERVIC_ABSENT;
            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enStateChange     = USIMM_CARD_STATE_CHANGED; /*需要上报卡状态*/
        }

        return VOS_ERR;
    }

    if(VOS_TRUE !=  USIMM_PinStatusCheck(pstMsg->enCmdType, pstMsg->enPINType, &ulResult))
    {
        USIMM_PinHandleCnf(pstMsg->ulSenderPid, pstMsg->enCmdType, pstMsg->enPINType,
                            ulResult, &gstUSIMMADFInfo.stPINInfo);

        return VOS_ERR;
    }

    switch(pstMsg->enCmdType)
    {
        case USIMM_PINCMD_DISABLE:          /*激活，去激活和验证PIN用同一函数处理*/
        case USIMM_PINCMD_ENABLE:
        case USIMM_PINCMD_VERIFY:
            ulResult = USIMM_PINVerify(pstMsg->enPINType,(VOS_UINT8)pstMsg->enCmdType,
                                        pstMsg->aucOldPIN);
            VOS_MemCpy(pstMsg->aucNewPIN, pstMsg->aucOldPIN, USIMM_PINNUMBER_LEN);
            break;

        case USIMM_PINCMD_CHANGE:           /*更改PIN处理*/
            ulResult = USIMM_ChangePIN(pstMsg->enPINType,pstMsg->aucOldPIN,pstMsg->aucNewPIN);
            break;

        case USIMM_PINCMD_UNBLOCK:          /*解锁PIN处理*/
            ulResult = USIMM_UnblockPIN(pstMsg->enPINType,pstMsg->aucOldPIN,pstMsg->aucNewPIN);
            break;

        default:
            USIMM_WARNING_LOG("USIMM_PINHandle: The CMD Type is Error");
            ulResult = VOS_ERR;
            break;
    }

    USIMM_AdjudgeCardState();    /*判断当前的状态*/

    if(USIMM_PIN == pstMsg->enPINType)    /*返回PIN操作的结果*/
    {
        if(VOS_OK == ulResult)
        {
            VOS_MemCpy(gstUSIMMADFInfo.aucPin1, pstMsg->aucNewPIN, USIMM_PINNUMBER_LEN);/*记录成功校验、去激活或激活后的PIN1码*/

            if(gstUSIMMPOOL.enPoolStatus < USIMM_POOL_AVAILABLE)        /*如果当前卡初始化未完成则需要继续初始化*/
            {
                USIMM_INFO_LOG("USIMM_PINHandle: The Initialation is Continue, Go Into Secorend Step");

                USIMM_InitNeedPinSteps(USIMM_NORMAL_MODE);

                /* 初始化可选文件 */
                if (VOS_OK != USIMM_InitThirdSteps())
                {
                    USIMM_ERROR_LOG("USIMM_PINHandle:USIMM_InitThirdSteps is Failed");

                    USIMM_PinHandleCnf(pstMsg->ulSenderPid, pstMsg->enCmdType, pstMsg->enPINType,
                                        VOS_ERR, &gstUSIMMADFInfo.stPINInfo);

                    return VOS_ERR;
                }
            }

            gstUSIMMPOOL.enPoolStatus = USIMM_POOL_AVAILABLE;

            USIMM_SendInitCardMsg(WUEPS_PID_USIM, USIMM_CMDTYPE_INITISIM_REQ);
        }
        else
        {
            VOS_MemSet(gstUSIMMADFInfo.aucPin1, 0, USIMM_PINNUMBER_LEN);
        }
    }

    USIMM_PinHandleCnf(pstMsg->ulSenderPid,pstMsg->enCmdType,pstMsg->enPINType,
                        ulResult, &gstUSIMMADFInfo.stPINInfo);

    return VOS_OK;
}


VOS_UINT32 USIMM_AutoVerifyPIN(USIMM_PIN_ENABLE_STATUS_ENUM_UINT32 enPinEnable,
                                     USIMM_PIN_VERIY_STATUS_ENUM_UINT32  *penPinVerified,
                                     VOS_UINT8                           ucPinRefNum,
                                     VOS_UINT8                           *pucPin)
{
    VOS_UINT32                      ulResult;

    if ((USIMM_PIN_ENABLED == enPinEnable)
     && (USIMM_PIN_NEED == *penPinVerified))
    {
        USIMM_NORMAL_LOG("USIMM_ProtectReset: Need Verify The Pin");

        if (0 == pucPin[0])  /*当前PIN码无效，需要上报卡状态*/
        {
            if (USIMM_POOL_AVAILABLE == gstUSIMMPOOL.enPoolStatus)
            {
                gstUSIMMPOOL.enPoolStatus = USIMM_POOL_NEED_PASSWORD;
            }

            return USIMM_INIT_NEEDPIN;
        }

        /* 成功校验过PIN码，则自动校验PIN码 */
        ulResult = USIMM_PINVerify_APDU(USIMM_PINCMD_VERIFY, ucPinRefNum, pucPin);

        if (VOS_OK != ulResult)/*检查结果*/
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: USIMM_PINVerify_APDU Error");

            return USIMM_INIT_FAIL;
        }

        ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

        if (USIMM_SW_OK != ulResult) /*当前操作不成功*/
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: Verify PIN1 Error");

            return USIMM_INIT_FAIL;
        }

        *penPinVerified = USIMM_NONEED;
    }

    return USIMM_INIT_OK;
}


VOS_UINT32 USIMM_CheckChangeCardByICCID()
{
    VOS_UINT32                      ulResult;
    VOS_UINT8                       aucOldIccid[20];
    VOS_UINT8                       aucNewIccid[20];
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;

    enAppType   =   ((USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)?USIMM_GSM_APP:USIMM_UMTS_APP);

    ulResult = USIMM_PoolReadOneFile(EFICCID, USIMM_READ_ALL_FILE_CONTENT, aucOldIccid, enAppType);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ProtectReset: USIMM_PoolReadOneFile EFICCID Error");

        return VOS_ERR;
    }

    /* 读ICCID */
    ulResult = USIMM_GetTFFile(enAppType, EFICCID, USIMM_READ_ALL_FILE_CONTENT, aucNewIccid);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ProtectReset: Select EFICCID Error");

        return VOS_ERR;
    }

    if(VOS_OK != VOS_MemCmp(aucOldIccid, aucNewIccid,gstUSIMMCurFileInfo.stEFInfo.usFileLen))
    {
        USIMM_ERROR_LOG("USIMM_ProtectReset: Not the same ICCID");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_ProtectFirstStep(VOS_UINT32  ulOldCardType)
{
    VOS_UINT32  ulResult;
    VOS_INT32   lResult;
    VOS_UINT16  usFileID;

    /* 复位卡，如果复位失败会将卡类型的全局变量置为无卡，因此复位成功后要重置卡类型的全局变量 */
    if (VOS_OK != DRV_USIMMSCI_DEACT())
    {
        USIMM_ERROR_LOG("USIMM_ProtectFirstStep: Deactive Card Error");

        return VOS_ERR;
    }

    ulResult = USIMM_DLResetCard(USIMM_RESET_CARD, &lResult);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ProtectFirstStep: USIMM_DLResetCard Error");

        return VOS_ERR;
    }

    /* 重置卡类型的全局变量 */
    gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType = ulOldCardType;

    /* 对于SIM卡要选择GSM应用，并切换电压 */
    if(USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        ulResult = USIMM_Select2GApp();

        if(VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ProtectFirstStep: USIMM_Select2GApp is Failed");

            return VOS_ERR;
        }
    }

    /* 对于USIM卡要根据MF fcp切换电压 */
    if(USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        usFileID = MF;

        ulResult = USIMM_SelectFile(USIMM_UMTS_APP, USIMM_NEED_FCP, 1, &usFileID);

        if(ulResult != VOS_OK)
        {
            USIMM_ERROR_LOG("USIMM_InsertNoNeedPinFileToPool: Select MF is Failed");

            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService = USIMM_CARD_SERVIC_UNAVAILABLE;          /*卡当前不可用*/

            return VOS_ERR;
        }

        ulResult = USIMM_UsimVoltageSwitch(gstUSIMMCurFileInfo.stDFInfo.ucCharaByte,
                                           &g_stUSIMMInitInfo.stUSIMMInfo.ucVoltageSwitchFlag);

        if(ulResult != VOS_OK)
        {
            USIMM_ERROR_LOG("USIMM_InsertNoNeedPinFileToPool: Select MF is Failed");

            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService = USIMM_CARD_SERVIC_UNAVAILABLE;          /*卡当前不可用*/

            return VOS_ERR;
        }

        USIMM_UsimTerminalCapability();

    }

    return VOS_OK;
}


VOS_UINT32 USIMM_ProtectSecondStep(VOS_VOID)
{
    if(USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        /* 下发profileDownload */
        USIMM_UsimProfileDownload();

        /* 激活UICC */
        if(VOS_ERR == USIMM_SelectUiccADF())
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: USIMM_SelectUiccADF Error");    /*打印错误*/
            return VOS_ERR;
        }
    }

    if(VOS_ERR == USIMM_InitCardPINInfo())    /*获取PIN出错*/
    {
        USIMM_ERROR_LOG("USIMM_ProtectReset: USIMM_InitCardPINInfo Error");    /*打印错误*/
        return VOS_ERR;
    }

    USIMM_AdjudgeCardState();

    return VOS_OK;
}
VOS_VOID USIMM_ResumeAppChannel(VOS_VOID)
{
    VOS_UINT32      ulAIDLen = 0;
    VOS_UINT8       aucADFName[USIMM_AID_LEN_MAX];
    VOS_UINT32      ulResult;
    VOS_UINT8       ucOldChanNum;
    VOS_UINT8       ucNewChanNum = 0;
    VOS_UINT32      ulSessionId = 0;
    VOS_UINT8       i;
    USIMM_APDU_ST   stApduInfo;

    if(VOS_FALSE == USIMM_IsCLEnable())
    {
        /* 清空逻辑通道信息，但是基本通道不清 */
        VOS_MemSet(&g_astUSIMMChCtrl[1], 0, sizeof(USIMM_CHANNEL_INFO_STRU)*(USIMM_CHANNEL_NUM_MAX - 1));

        return;
    }

    VOS_MemSet(&stApduInfo, 0, sizeof(stApduInfo));

    ulResult = USIMM_GetAid(USIMM_AID_TYPE_CSIM, &ulAIDLen, aucADFName);

    if (VOS_OK == ulResult)
    {
        ulResult = USIMM_GetUiccChanNumByAid(aucADFName, ulAIDLen, &ucOldChanNum);

        if ((VOS_OK != ulResult) || (ucOldChanNum >= USIMM_CHANNEL_NUM_MAX))
        {
            USIMM_ERROR_LOG("USIMM_ResumeAppChannel: USIMM_GetUiccChanNumByAid failed.");

            /* 清空逻辑通道信息，但是基本通道不清 */
            VOS_MemSet(&g_astUSIMMChCtrl[1], 0, sizeof(USIMM_CHANNEL_INFO_STRU)*(USIMM_CHANNEL_NUM_MAX - 1));

            return;
        }

        ulSessionId = g_astUSIMMChCtrl[ucOldChanNum].ulSessionId;

        /* 清空逻辑通道信息，但是基本通道不清 */
        VOS_MemSet(&g_astUSIMMChCtrl[1], 0, sizeof(USIMM_CHANNEL_INFO_STRU)*(USIMM_CHANNEL_NUM_MAX - 1));

        while (ucOldChanNum > ucNewChanNum)
        {
            /* 下发manage打开逻辑通道,打开通道的时候在基本逻辑通道上，因此P2参数为0,P3参数填1，期望带回一个字节的channelID */
            if (VOS_OK != USIMM_Manage_Channel_APDU(USIMM_OPEN_CHANNEL, 0, 1))
            {
                USIMM_ERROR_LOG("USIMM_ResumeAppChannel: The USIMM_Manage_Channel_APDU Return Error");

                return;
            }

            ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

            /*判断检查结果*/
            if (USIMM_SW_OK != ulResult)
            {
                USIMM_WARNING_LOG("USIMM_ResumeAppChannel: USIMM_CheckSW Error");

                return;
            }

            ucNewChanNum = gstUSIMMAPDU.aucRecvBuf[0];

            /* 通道ID超出合法范围 */
            if (USIMM_CHANNEL_NUM_MAX <= ucNewChanNum)
            {
                USIMM_ERROR_LOG("USIMM_ResumeAppChannel: Channel ID is not valid.");

                USIMM_CloseChannel(ucNewChanNum);

                return;
            }
        }

        for (i = 1; i < ucNewChanNum; i++)
        {
            USIMM_CloseChannel(i);
        }

        stApduInfo.aucAPDU[CLA] = USIMM_USIM_CLA;

        stApduInfo.aucAPDU[P1] = USIMM_SELECT_BY_DF_NAME;
        stApduInfo.aucAPDU[P2] = USIMM_SELECT_RETURN_FCP_TEMPLATE;
        stApduInfo.aucAPDU[P3] = (VOS_UINT8)ulAIDLen;

        VOS_MemCpy(stApduInfo.aucSendBuf, aucADFName, ulAIDLen);

        /* 下发激活AID命令数据单元,cla字段应该在打开的逻辑通道上 */
        ulResult = USIMM_SelectFileByChannelID_APDU(ucNewChanNum, &stApduInfo);

        if (VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ResumeAppChannel: select csim aid failed.");

            USIMM_CloseChannel(ucNewChanNum);

            return;
        }

        ulResult = USIMM_CheckSW(&stApduInfo);

        if (USIMM_SW_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ResumeAppChannel: USIMM_CheckSW failed.");

            USIMM_CloseChannel(ucNewChanNum);

            return;
        }

        g_astUSIMMChCtrl[ucNewChanNum].ulSessionId = ulSessionId;
        g_astUSIMMChCtrl[ucNewChanNum].ulChanNum   = ucNewChanNum;

        VOS_MemCpy(g_astUSIMMChCtrl[ucNewChanNum].aucADFName, aucADFName, ulAIDLen);

        g_astUSIMMChCtrl[ucNewChanNum].ulAIDLen = ulAIDLen;
    }
    else
    {
        /* 清空逻辑通道信息，但是基本通道不清 */
        VOS_MemSet(&g_astUSIMMChCtrl[1], 0, sizeof(USIMM_CHANNEL_INFO_STRU)*(USIMM_CHANNEL_NUM_MAX - 1));
    }

    return;
}

#if(FEATURE_ON == FEATURE_PTM)

VOS_VOID USIMM_CardErrorLogReport(VOS_VOID)
{
    OM_ERR_LOG_REPORT_CNF_STRU                *pstCnfMsg;

    if (OM_APP_STATUS_CLOSE == g_stErrLogCtrlInfo.ucAlmStatus)
    {
        return;
    }

    /* 分配消息内存  */
    pstCnfMsg = (OM_ERR_LOG_REPORT_CNF_STRU*)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(OM_ERR_LOG_REPORT_CNF_STRU)
                                                                          - VOS_MSG_HEAD_LENGTH
                                                                          - 4
                                                                          + sizeof(g_stCardErrorLog));
    if (VOS_NULL_PTR == pstCnfMsg)
    {
        USIMM_ERROR_LOG("USIMM_CardErrorLogReport: VOS_AllocMsg is Failed");

        return;
    }

    /* 填充cnf消息 */
    pstCnfMsg->ulReceiverPid    = ACPU_PID_OM;
    pstCnfMsg->ulMsgName        = ID_OM_ERR_LOG_REPORT_CNF;
    pstCnfMsg->ulMsgType        = OM_ERR_LOG_MSG_ERR_REPORT;
    pstCnfMsg->ulRptlen         = sizeof(g_stCardErrorLog);

    /*lint -e419*/
    VOS_MemCpy(pstCnfMsg->aucContent, &g_stCardErrorLog, sizeof(g_stCardErrorLog));
    /*lint +e419*/

    /* 回复消息 */
    (VOS_VOID)VOS_SendMsg(pstCnfMsg->ulSenderPid, pstCnfMsg);

    return;
}
VOS_VOID USIMM_RecordCardErrorLog(
    USIMM_MNTN_STATE_ERROR_REASON_ENUM_UINT32                   enErrorReason,
    USIMM_MNTN_ERROR_LEVEL_ENUM_UINT16                          enAlmLevel)
{
    VOS_INT32       lResult;
    BSP_U32         ulLength;

    g_stCardErrorLog.stHeader.ulMsgModuleId = OM_ERR_LOG_MOUDLE_ID_USIMM;

#if defined (INSTANCE_1)
    g_stCardErrorLog.stHeader.usModemId = MODEM_ID_1; /* 标识主卡副卡 */
#else
    g_stCardErrorLog.stHeader.usModemId = MODEM_ID_0;
#endif

    g_stCardErrorLog.stHeader.usAlmId = 1;
    g_stCardErrorLog.stHeader.usAlmLevel = enAlmLevel;
    g_stCardErrorLog.stHeader.usAlmType = 0;        /* 0代表通信告警 */
    g_stCardErrorLog.stHeader.usAlmLowSlice = OM_GetSlice();
    g_stCardErrorLog.stHeader.usAlmHighSlice = 0;   /* 暂时不用，默认填0 */

    g_stCardErrorLog.enStateErrorReason = enErrorReason;

    /* 调用底软接口获取下面参数 */
    lResult = DRV_USIMMSCI_RECORDLOG_READ((VOS_UINT8 *)&g_stCardErrorLog.stSCIRecordInfo,
                                  &ulLength, sizeof(g_stCardErrorLog.stSCIRecordInfo));

    if (VOS_OK != lResult)
    {
        USIMM_ERROR_LOG("USIMM_RecordCardErrorLog: DRV API RETURN ERR");
    }

    g_stCardErrorLog.stHeader.ulAlmLength = sizeof(VOS_UINT32) + sizeof(SCI_RECORD_INFO) + sizeof(g_stCardErrorLog.aulRsv);

    return;
}
#endif

/*****************************************************************************
 函 数 名  : USIMM_TaskDelayTimerHandler
 功能描述  : 
 输入参数  : ulParam     --- 启动定时器时所传入的ulParam
             ulTimerName --- 定时器名字
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 
    作    者   : 
    修改内容   : 

*****************************************************************************/
VOS_VOID USIMM_TaskDelayTimerHandler(VOS_UINT32 ulParam, VOS_UINT32 ulTimerName)
{
    VOS_SmV(g_ulUSIMMTaskDelaySemID);

    return;
}

/*****************************************************************************
函 数 名  :USIMM_TaskDelay
功能描述  :
输入参数  :
输出参数  :
返 回 值  :无
修订记录  :
1. 日    期   : 
   作    者   : 
   修改内容   : 

*****************************************************************************/
VOS_VOID USIMM_TaskDelay(VOS_UINT32 ulTimeLen)
{
    if (0 == ulTimeLen)
    {
        return;
    }

#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
    VOS_TaskDelay(ulTimeLen);
#else
    if(VOS_OK != VOS_StartCallBackRelTimer(&g_ulUSIMMTaskDelayTimerID,
                                              WUEPS_PID_USIM,
                                              ulTimeLen,
                                              USIMM_CB_TIMER_NAME_TASKDELAY,
                                              VOS_NULL,
                                              VOS_RELTIMER_NOLOOP,
                                              USIMM_TaskDelayTimerHandler,
                                              VOS_TIMER_PRECISION_5))
    {
        USIMM_ERROR_LOG("USIMM_TaskDelay: VOS_StartCallBackRelTimer fail!");

        return;
    }

    if(VOS_OK != VOS_SmP(g_ulUSIMMTaskDelaySemID, USIMM_GET_CARD_STATUS_SEM_LEN))
    {
        USIMM_ERROR_LOG("USIMM_TaskDelay: VOS_SmP fail!");
    }
#endif

    return;
}
VOS_UINT32 USIMM_ProtectReset(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                      ulResult = VOS_ERR;
    USIMM_PROTECT_RESET_REQ_STRU    *pstMsg;
    VOS_UINT32                      i;
    VOS_UINT32                      ulOldCardStatus;
    VOS_UINT32                      ulOldCardType;

    pstMsg = (USIMM_PROTECT_RESET_REQ_STRU *)pMsg;

    /* 当前已经无卡情况下不做保护性复位，直接回复失败 */
    if (USIMM_CARD_SERVIC_ABSENT == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService)
    {
        USIMM_SingleCmdCnf(pstMsg->ulSenderPid, PS_USIM_PROTECT_RESET_CNF, VOS_ERR);

        return VOS_OK;
    }

    USIMM_NORMAL_LOG("USIMM_ProtectReset: Protect Reset Start");

    /* 保存保护性复位之前的卡状态 */
    ulOldCardStatus = gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService;
    ulOldCardType   = gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType;

    /*上报SDT*/
    USIMM_ReportEvent(USIMM_PROTECT_RESET);

    if((USIMM_CARD_USIM != ulOldCardType) && (USIMM_CARD_SIM != ulOldCardType))
    {
        USIMM_ERROR_LOG("USIMM_ProtectReset: Card type is neither SIM nor USIM");
        USIMM_SingleCmdCnf(pstMsg->ulSenderPid, PS_USIM_PROTECT_RESET_CNF, VOS_ERR);
        return VOS_ERR;
    }

    for(i = 0; i < USIMM_PROTECT_MAX_NUM; i++)
    {
        USIMM_TaskDelay((i*100));

        USIMM_ClearCurFileInfo();

        USIMM_T1InitGlobalVar();

        ulResult = USIMM_ProtectFirstStep(ulOldCardType);

        if(VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: USIMM_ProtectFirstStep is err");    /*打印错误*/

            continue;
        }

        ulResult = USIMM_CheckChangeCardByICCID();

        if(VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: the card has changed!");    /*打印错误*/

            break;
        }

        ulResult = USIMM_ProtectSecondStep();

        if(VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: the USIMM_ProtectSecondStep is Error!");    /*打印错误*/

            continue;
        }

        /* 成功校验过PIN码，则自动校验PIN码 */
        ulResult = USIMM_AutoVerifyPIN(gstUSIMMADFInfo.stPINInfo.enPin1Enable,
                                       &gstUSIMMADFInfo.enPin1Verified,
                                       gstUSIMMADFInfo.ucPIN1RefNum,
                                       gstUSIMMADFInfo.aucPin1);

        if(USIMM_INIT_FAIL == ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ProtectReset: the USIMM_ProtectSecondStep is Error!");    /*打印错误*/

            continue;
        }

        /* 未校验pin码，需要上报卡状态 */
        if(USIMM_INIT_NEEDPIN == ulResult)
        {
            ulResult = VOS_OK;

            break;
        }

        if(USIMM_CARD_SIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
        {
            USIMM_SimProfileDownload(USIMM_PROTECT_RESET_MODE);
            USIMM_InitSimFDNBDNStatus();
        }

        USIMM_AdjudgeCardState();

        if(USIMM_CARD_SERVIC_AVAILABLE == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService)
        {
            gstUSIMMPOOL.enPoolStatus = USIMM_POOL_AVAILABLE;
        }

        break;
    }

    if ((VOS_OK == ulResult) && (ulOldCardStatus == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService))
    {
        gastUSIMMCardAppInfo[USIMM_UICC_USIM].enStateChange = USIMM_CARD_STATE_NOCHANGE;

        USIMM_ResumeAppChannel();
    }

    if (VOS_OK != ulResult)
    {
        USIMM_DeactiveRealCard(pMsg);
#if(FEATURE_ON == FEATURE_PTM)
        USIMM_RecordCardErrorLog(USIMM_MNTN_STATE_POLLING_FAIL, USIMM_MNTN_ERROR_LEVEL_CRITICAL);
#endif
    }

    USIMM_SingleCmdCnf(pstMsg->ulSenderPid, PS_USIM_PROTECT_RESET_CNF, ulResult);

    USIMM_NORMAL_LOG("USIMM_ProtectReset: Protect Reset End");

    return VOS_OK;
}


VOS_UINT32 USIMM_StatusHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32              ulResult = VOS_ERR;
    USIMM_STATUS_REQ_STRU   *pstMsg;

    pstMsg = (USIMM_STATUS_REQ_STRU*)pMsg;

    if(pstMsg->enNeedFcp == USIMM_POLL_NEED_FCP)
    {
        ulResult = USIMM_STATUSProc();
    }
    else
    {
        ulResult = USIMM_Status_APDU(USIMM_STATUS_NO_INDICATION, USIMM_STATUS_NO_DATA_RETURNED, 0);

        if(VOS_OK != ulResult)
        {
            ulResult = USIMM_SW_SENDCMD_ERROR;
        }
    }

    USIMM_SingleCmdCnf(pMsg->ulSenderPid, PS_USIM_STATUSCMD_CNF, ulResult);

    return ulResult;
}


VOS_UINT32 USIMM_UpdateFile(USIMM_EF_TYPE_ENUM_UINT32  enEFFileType,
                                 VOS_UINT8                   ucRecordNum,
                                 VOS_UINT16                  usDataLen,
                                 VOS_UINT8                   *pucData)
{
    VOS_UINT32                              ulResult;

    switch (enEFFileType)
    {
        case USIMM_EFSTRUCTURE_TRANSPARENT:

            /*更新二进制文件*/
            ulResult = USIMM_UpdateTFFile(usDataLen, pucData);
            break;

        case USIMM_EFSTRUCTURE_FIXED:

            /*更新记录文件*/
            ulResult = USIMM_UpdateLFFile(ucRecordNum, pucData, (VOS_UINT8)usDataLen);
            break;

        case USIMM_EFSTRUCTURE_CYCLE:

            /*更新循环文件*/
            ulResult = USIMM_UpdateCLFFile(pucData, (VOS_UINT8)usDataLen);
            break;

        default:
            USIMM_WARNING_LOG("USIMM_SetFileHandle: The File Type is Not Support");
            ulResult = USIMM_SW_FILETYPE_ERROR;
            break ;
    }

    return ulResult;
}
VOS_UINT32 USIMM_SetSPBFileHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                 i;
    USIMM_SET_SPBFILE_REQ_STRU *pstMsg;
    VOS_UINT32                 ulResult;

    pstMsg = (USIMM_SET_SPBFILE_REQ_STRU *)pMsg;

    for(i = 0; i < pstMsg->ulSPBReqCount; i++)
    {
        /* 选择电话本文件, 电话本文件为记录文件，因此不需要带回FCP */
        if (VOS_OK != USIMM_SelectFile(pstMsg->stSPBReq[i].enAppType, USIMM_NO_NEED_FCP, 1, &pstMsg->stSPBReq[i].usFileID))
        {
            USIMM_SetSPBFileCnf(pstMsg->ulSenderPid, USIMM_SW_NOFILE_SELECT,pstMsg);

            return VOS_ERR;
        }

        ulResult = USIMM_UpdateFile(USIMM_EFSTRUCTURE_FIXED,
                                    pstMsg->stSPBReq[i].ucRecordNum,
                                    pstMsg->stSPBReq[i].usDataLen,
                                    pstMsg->stSPBReq[i].aucContent);
        if (VOS_OK != ulResult)
        {
            USIMM_SetSPBFileCnf(pstMsg->ulSenderPid, USIMM_SW_DOWNLOAD_ERROR, pstMsg);

            return VOS_ERR;
        }
    }

    USIMM_SetSPBFileCnf(pstMsg->ulSenderPid, USIMM_SW_OK, pstMsg);

    return VOS_OK;
}
VOS_UINT32 USIMM_SelectFile(USIMM_APP_TYPE_ENUM_UINT32         enFileApp,
                                USIMM_FILE_NEED_FCP_ENUM_UINT32     enEfFcpFlag,
                                VOS_UINT32                          ulPathLen,
                                VOS_UINT16                         *pusFilePath)
{
    VOS_UINT32                          ulResult;
    USIMM_SELECT_PARA_STUR              stSelectPara;
    USIMM_U16_LVDATA_STRU               stFilePath;

    if (VOS_NULL_PTR == pusFilePath)
    {
        USIMM_ERROR_LOG("USIMM_SelectFile:Input Parameter error.");

        return USIMM_SW_ERR;
    }

    stSelectPara.enFileApp      = enFileApp;
    stSelectPara.enCardType     = gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType;
    stSelectPara.enEfFcpFlag    = enEfFcpFlag;

    stFilePath.ulDataLen        = ulPathLen;
    stFilePath.pusData          = pusFilePath;

    /* 选择文件处理 */
    ulResult = USIMM_SelectFileHandle(&stSelectPara, &stFilePath);

    if(ulResult != USIMM_SW_OK)
    {
        USIMM_ERROR_LOG("USIMM_SelectFile: Select File with Path Error.");
    }

    return ulResult;
}
VOS_UINT32 USIMM_GetFileAllPath(USIMM_SELECT_PARA_STUR       *pstSelectPara,
                                        USIMM_U16_LVDATA_STRU      *pstFilePathIn,
                                        USIMM_U16_LVDATA_STRU      *pstFilePathOut)
{
    VOS_UINT32                          ulResult = VOS_ERR;
    VOS_UINT32                          ulPathAllLen = 0;
    VOS_UINT16                          ausPathTemp[USIMM_MAX_PATH_LEN] = {0};
    VOS_UINT16                          ausPathAll[USIMM_MAX_PATH_LEN] = {0};
#if 0
    VOS_UINT32                          i;
    VOS_UINT8                           aucTypeTemp[USIMM_MAX_PATH_LEN] = {0};
    VOS_UINT8                           aucTypeOK0[] = {MFTAG};
    VOS_UINT8                           aucTypeOK1[] = {MFTAG, EFUNDERMF};
    VOS_UINT8                           aucTypeOK2[] = {MFTAG, DFUNDERMF};
    VOS_UINT8                           aucTypeOK3[] = {MFTAG, DFUNDERMF, EFUNDERDF};
    VOS_UINT8                           aucTypeOK4[] = {MFTAG, DFUNDERMF, DFUNDERDF};
    VOS_UINT8                           aucTypeOK5[] = {MFTAG, DFUNDERMF, DFUNDERDF, EFUNDERGRANDADF};
#endif

    /* 将文件ID改为路径 */
    if (1 == pstFilePathIn->ulDataLen)
    {
        ulResult = USIMM_GetCardFilePath(pstSelectPara->enFileApp, pstFilePathIn->pusData[0], &ulPathAllLen, ausPathTemp);
    }

    /* 输入为路径 或 通过文件ID获得路径失败 */
    if (VOS_OK != ulResult)
    {
        VOS_MemCpy(ausPathTemp, pstFilePathIn->pusData, (pstFilePathIn->ulDataLen)*sizeof(VOS_UINT16));
        ulPathAllLen = pstFilePathIn->ulDataLen;
    }

    /* 确保路径都是绝对路径 */
    if (MFTAG == USIMM_FILE_TYPE(ausPathTemp[0]))
    {
        VOS_MemCpy(ausPathAll, ausPathTemp, ulPathAllLen*sizeof(VOS_UINT16));
    }
    else
    {
        ausPathAll[0] = MF;

        VOS_MemCpy(&ausPathAll[1], ausPathTemp, ulPathAllLen*sizeof(VOS_UINT16));

        ulPathAllLen++;
    }

    VOS_MemCpy(pstFilePathOut->pusData, ausPathAll, ulPathAllLen*sizeof(VOS_UINT16));

    pstFilePathOut->ulDataLen = ulPathAllLen;

    return VOS_OK;

#if 0
    /* 将绝对路径改为类型形式 */
    for (i = 0; i < ulPathAllLen; i++)
    {
        aucTypeTemp[i] = USIMM_FILE_TYPE(ausPathAll[i]);
    }

    /* ATT卡不用进行路径合法性检测 */
    if (USIMM_ATT_APP == pstSelectPara->enFileApp)
    {
        ulResult = VOS_OK;
    }
    else
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK0, aucTypeTemp, ulPathAllLen);
    }

    if (ulResult != VOS_OK)
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK1, aucTypeTemp, ulPathAllLen);
    }

    if (ulResult != VOS_OK)
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK2, aucTypeTemp, ulPathAllLen);
    }

    if (ulResult != VOS_OK)
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK3, aucTypeTemp, ulPathAllLen);
    }

    if (ulResult != VOS_OK)
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK4, aucTypeTemp, ulPathAllLen);
    }

    if (ulResult != VOS_OK)
    {
        ulResult = (VOS_UINT32)VOS_MemCmp(aucTypeOK5, aucTypeTemp, ulPathAllLen);
    }

    if (VOS_OK == ulResult)
    {
        VOS_MemCpy(pstFilePathOut->pusData, ausPathAll, ulPathAllLen*sizeof(VOS_UINT16));

        pstFilePathOut->ulDataLen = ulPathAllLen;
    }

    return ulResult;
#endif

}
VOS_UINT32 USIMM_SelectFileHandle(USIMM_SELECT_PARA_STUR        *pstSelectPara,
                                        USIMM_U16_LVDATA_STRU          *pstFilePath)
{
    VOS_UINT16                          ausFileAllPath[USIMM_MAX_PATH_LEN] = {0};
    USIMM_U16_LVDATA_STRU               stFileAllPath = {0};
    VOS_UINT16                          usCurPathLen = 0;
    VOS_UINT32                          ulResult;

    /* 参数检测 */
    if ((VOS_NULL_PTR == pstSelectPara)||(VOS_NULL_PTR == pstFilePath))
    {
        USIMM_ERROR_LOG("USIMM_SelectFileHandle:Input Parameter error.");

        return USIMM_SW_ERR;
    }

    stFileAllPath.pusData = ausFileAllPath;

    /* 获取选择文件的全路径 */
    if (VOS_OK != USIMM_GetFileAllPath(pstSelectPara, pstFilePath, &stFileAllPath))
    {
        USIMM_ERROR_LOG("USIMM_SelectFileHandle:Can't find the file.");

        return USIMM_SW_NOFILE_FOUND;
    }

    /* 选择文件已经是当前文件则退出并返回成功 */
    ulResult = USIMM_CardFilePathCMP(stFileAllPath.ulDataLen, stFileAllPath.pusData,
                    gstUSIMMCurFileInfo.usFilePathLen, gstUSIMMCurFileInfo.ausFilePath);

    if (VOS_OK == ulResult)
    {
        if (USIMM_IS_DF_FILE(stFileAllPath.pusData[stFileAllPath.ulDataLen-1])
            || (0 != gstUSIMMCurFileInfo.stEFInfo.usFileLen)
            || (USIMM_NO_NEED_FCP == pstSelectPara->enEfFcpFlag))
        {
            USIMM_INFO_LOG("USIMM_SelectFileHandle: Selected file is current file.");

            return USIMM_SW_OK;
        }
    }

    /* 通过全路径选择文件 */
    ulResult = USIMM_SelectFileWithPath(pstSelectPara, &stFileAllPath, &gstUSIMMCurFileInfo, &usCurPathLen);

    if (VOS_NULL == usCurPathLen)
    {
        USIMM_ClearCurFileInfo();
    }
    else
    {
        stFileAllPath.ulDataLen = usCurPathLen;

        /* 更新路径 */
        USIMM_UpdateFilePath(pstSelectPara, &stFileAllPath, &gstUSIMMCurFileInfo);
    }

    return ulResult;
}
VOS_UINT32 USIMM_CheckSetFilePara(USIMM_EFFCP_ST *pstCurEFFcp, VOS_UINT8 ucRecordNum, VOS_UINT16 usDataLen)
{
    VOS_UINT32                          ulResult = USIMM_SW_OK;

    switch (pstCurEFFcp->enFileType)
    {
        case USIMM_EFSTRUCTURE_TRANSPARENT:
            /*检查更新数据是否过长*/
            if(pstCurEFFcp->usFileLen < usDataLen)
            {
                USIMM_ERROR_LOG("USIMM_CheckSetFilePara: The Length of Data is too Large to the File");

                ulResult = USIMM_SW_OUTOF_RANGE;
            }

            break;

        case USIMM_EFSTRUCTURE_FIXED:
            /*检查更新的记录号是否越界*/
            if(pstCurEFFcp->ucRecordNum < ucRecordNum)
            {
                USIMM_ERROR_LOG("USIMM_CheckSetFilePara: The Record Index of Data is Out of File Range");

                ulResult = USIMM_SW_OUTOF_RANGE;
            }

            break;

        case USIMM_EFSTRUCTURE_CYCLE:
            ulResult = USIMM_SW_OK;
            break;

        default:
            USIMM_WARNING_LOG("USIMM_CheckSetFilePara: The File Type is Not Support");
            ulResult = USIMM_SW_CMD_ERROR;
            break;
    }

    return ulResult;
}


VOS_UINT32 USIMM_SetFileCommHandle(USIMM_SET_COMM_FILE_STRU  *pstSetFileInfo)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulFileIndex;
    USIMM_FILE_NEED_FCP_ENUM_UINT32     enFcpFlag;
    VOS_BOOL                            bParaCheckFlag;
    USIMM_EFFCP_ST                      stEFFcp;

    VOS_MemSet(&stEFFcp, 0, sizeof(stEFFcp));

    /* 先判断是否已确定文件类型 */
    if (USIMM_EFSTRUCTURE_NONE == pstSetFileInfo->enEFFileType)
    {
        bParaCheckFlag = VOS_TRUE;

        if (VOS_OK == USIMM_PoolFindFile(pstSetFileInfo->usFileID, &ulFileIndex, pstSetFileInfo->enAppType))
        {
            enFcpFlag                               = USIMM_NO_NEED_FCP;
            stEFFcp.enFileType = gstUSIMMPOOL.astpoolRecord[ulFileIndex].enFileType;
            stEFFcp.usFileLen  = gstUSIMMPOOL.astpoolRecord[ulFileIndex].usLen;
        }
        else
        {
            enFcpFlag                               = USIMM_NEED_FCP;
        }
    }
    else
    {
        stEFFcp.enFileType = pstSetFileInfo->enEFFileType;
        enFcpFlag          = USIMM_NO_NEED_FCP;
        bParaCheckFlag     = VOS_FALSE;
    }

    ulResult = USIMM_SelectFile(pstSetFileInfo->enAppType, enFcpFlag, 1, &pstSetFileInfo->usFileID);

    if(USIMM_SW_OK != ulResult)/*判断选择结果*/
    {
        USIMM_WARNING_LOG("USIMM_SetFileCommHandle: Select File is Failed");

        return ulResult;
    }

    if (USIMM_NEED_FCP == enFcpFlag)
    {
        VOS_MemCpy(&stEFFcp, &(gstUSIMMCurFileInfo.stEFInfo), sizeof(stEFFcp));
    }

    if (VOS_TRUE == bParaCheckFlag)
    {
        ulResult = USIMM_CheckSetFilePara(&stEFFcp, pstSetFileInfo->ucRecordNum, pstSetFileInfo->usDataLen);

        if(USIMM_SW_OK != ulResult)/*判断选择结果*/
        {
            USIMM_WARNING_LOG("USIMM_SetFileCommHandle: USIMM_CheckSetFilePara is Failed");

            return ulResult;
        }
    }

    /* 根据文件类型更新文件 */
    ulResult = USIMM_UpdateFile(stEFFcp.enFileType, pstSetFileInfo->ucRecordNum, pstSetFileInfo->usDataLen, pstSetFileInfo->pucData);

    if(USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_SetFileCommHandle: Update File Content is Failed");

        return ulResult;
    }

    /* 如果文件存在在缓冲中则需要更新 */
    if (USIMM_SW_OK == USIMM_PoolDelOneFile(pstSetFileInfo->usFileID, pstSetFileInfo->enAppType))
    {
        USIMM_PoolInsertOneFile(pstSetFileInfo->usFileID, pstSetFileInfo->usDataLen, pstSetFileInfo->pucData,
                                pstSetFileInfo->enAppType, stEFFcp.enFileType);
    }

    return USIMM_SW_OK;
}
VOS_UINT32 USIMM_SetUsimFileHandle(USIMM_SETFILE_REQ_STRU *pstMsg)
{
    USIMM_SET_COMM_FILE_STRU            stSetFileInfo = {0};

    stSetFileInfo.enAppType         = pstMsg->enAppType;
    stSetFileInfo.usFileID          = pstMsg->usFileID;
    stSetFileInfo.usDataLen         = pstMsg->usDataLen;
    stSetFileInfo.ucRecordNum       = pstMsg->ucRecordNum;
    stSetFileInfo.pucData           = pstMsg->aucContent;

    return USIMM_SetFileCommHandle(&stSetFileInfo);
}


VOS_UINT32 USIMM_SetPBFileHandle(USIMM_SETFILE_REQ_STRU *pstMsg)
{
    USIMM_SET_COMM_FILE_STRU            stSetFileInfo;

    stSetFileInfo.enAppType         = pstMsg->enAppType;
    stSetFileInfo.usFileID          = pstMsg->usFileID;
    stSetFileInfo.usDataLen         = pstMsg->usDataLen;
    stSetFileInfo.ucRecordNum       = pstMsg->ucRecordNum;
    stSetFileInfo.pucData           = pstMsg->aucContent;

    /* 这几个电话本文件为二进制文件 */
    if((EFPSC == pstMsg->usFileID) || (EFCC == pstMsg->usFileID) || (EFPUID == pstMsg->usFileID))
    {
        stSetFileInfo.enEFFileType    = USIMM_EFSTRUCTURE_TRANSPARENT;
    }
    else
    {
        stSetFileInfo.enEFFileType    = USIMM_EFSTRUCTURE_FIXED;
    }

    return USIMM_SetFileCommHandle(&stSetFileInfo);
}
VOS_UINT32 USIMM_SetIsimFileHandle(USIMM_SETFILE_REQ_STRU *pstMsg)
{
    USIMM_SET_COMM_FILE_STRU            stSetFileInfo;
    VOS_UINT16                          usFileID = VOS_NULL_WORD;
    VOS_UINT32                          ulResult = VOS_OK;

    VOS_MemSet(&stSetFileInfo, 0, sizeof(stSetFileInfo));

    /* 当前ISIM卡服务不可用 */
    if (USIMM_CARD_SERVIC_AVAILABLE != gastUSIMMCardAppInfo[USIMM_UICC_ISIM].enCardService)
    {
        USIMM_WARNING_LOG("USIMM_SetIsimFileHandle: ISIM app unavailable");

        return USIMM_SW_ERR;
    }

    stSetFileInfo.enAppType         = pstMsg->enAppType;
    stSetFileInfo.usFileID          = pstMsg->usFileID;
    stSetFileInfo.usDataLen         = pstMsg->usDataLen;
    stSetFileInfo.ucRecordNum       = pstMsg->ucRecordNum;
    stSetFileInfo.pucData           = pstMsg->aucContent;

    /* 根据ISIM文件ID查找对应的USIM文件ID 找不到对应的USIM文件ID则认为文件不可更新 */
    usFileID = USIMM_IsimEFIDToUsimEFID(stSetFileInfo.usFileID);

    /* 找不到对应的USIM文件ID */
    if (VOS_NULL_WORD == usFileID)
    {
        /* 文件不可更新 */
        USIMM_WARNING_LOG("USIMM_SetIsimFileHandle: ISIM file can't write.");

        return USIMM_SW_NOFILE_FOUND;
    }

    /* 按USIM文件id更新文件 */
    stSetFileInfo.usFileID  = usFileID;
    stSetFileInfo.enAppType = USIMM_UMTS_APP;

    /* 调用USIM接口更新USIM文件 */
    ulResult = USIMM_SetFileCommHandle(&stSetFileInfo);

    return ulResult;
}


VOS_UINT32 USIMM_SetRealFile(USIMM_MsgBlock *pMsg)
{
    USIMM_SETFILE_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulResult;
    USIMM_SETCNF_INFO_STRU              stCnfInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    pstMsg = (USIMM_SETFILE_REQ_STRU *)pMsg;

    stCnfInfo.ucRecordNum  = pstMsg->ucRecordNum;
    stCnfInfo.usEFLen      = pstMsg->usDataLen;
    stCnfInfo.usEfid       = pstMsg->usFileID;

    /* 执行ISIM更新文件 */
    if (USIMM_ISIM_APP == pstMsg->enAppType)
    {
        ulResult = USIMM_SetIsimFileHandle(pstMsg);
    }
    else if (USIMM_PB_APP == pstMsg->enAppType)
    {
        ulResult = USIMM_SetPBFileHandle(pstMsg);
    }
    else
    {
        ulResult = USIMM_SetUsimFileHandle(pstMsg);
    }

    USIMM_SetFileCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara, ulResult, &stCnfInfo);/*回复更新结果*/

    return VOS_OK;
}


VOS_UINT32 USIMM_SetFileHandle(USIMM_MsgBlock *pMsg)
{
#if (FEATURE_VSIM == FEATURE_ON)
    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM is Active*/
    {
        return USIMM_SetVsimFile((USIMM_SETFILE_REQ_STRU *)pMsg);
    }
#endif

    /*硬卡操作*/
    return USIMM_SetRealFile(pMsg);
}


VOS_UINT32 USIMM_GetFileFromCard(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT32                          ulResult;

    ulResult = USIMM_SelectFile(pstFileInfo->enAppType, USIMM_NEED_FCP, pstFileInfo->ulPathLen, pstFileInfo->pusFilePath);

    if(VOS_OK != ulResult) /*文件选择失败*/
    {
        USIMM_ERROR_LOG("USIMM_GetFileFromCard: Select File is Failed");

        return ulResult;
    }

    /* 获取要读的文件参数，stCnfInfo结构域中usEfLen, usDataLen, ucRecordNum, ucTotalNum的值 */
    ulResult = USIMM_GetReadFilePara(pstFileInfo->ucRecordNum, &gstUSIMMCurFileInfo.stEFInfo, pstCnfInfo);

    /* 获取读文件参数失败 */
    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_GetFileFromCard: Get file para Failed");

        return ulResult;
    }

    pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, pstCnfInfo->usDataLen);

    if(VOS_NULL_PTR == pstCnfInfo->pucEf)/*检查返回值*/
    {
        USIMM_ERROR_LOG("USIMM_GetFileFromCard: VOS_MemAlloc is Error");

        return USIMM_SW_ERR;
    }

    ulResult = USIMM_ReadFile(&gstUSIMMCurFileInfo.stEFInfo, pstCnfInfo->ucRecordNum, pstCnfInfo->usDataLen, pstCnfInfo->pucEf);

    return ulResult;
}


VOS_UINT32 USIMM_GetFileCommHandle(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT32                  ulResult;
    VOS_UINT32                  ulFileNum;
    USIMM_GETCNF_INFO_STRU      stCnfInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    if (USIMM_SW_OK == USIMM_PoolFindFile(*(pstFileInfo->pusFilePath), &ulFileNum, pstFileInfo->enAppType))
    {
        USIMM_INFO_LOG("USIMM_GetFileCommHandle: Get File Success from Usimm Pool");

        pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, gstUSIMMPOOL.astpoolRecord[ulFileNum].usLen);

        if(VOS_NULL_PTR == pstCnfInfo->pucEf)/*检查返回值*/
        {
            USIMM_ERROR_LOG("USIMM_GetFileCommHandle: VOS_MemAlloc is Error");

            return USIMM_SW_ERR;
        }

        VOS_MemCpy(pstCnfInfo->pucEf, gstUSIMMPOOL.astpoolRecord[ulFileNum].pucContent, gstUSIMMPOOL.astpoolRecord[ulFileNum].usLen);
        pstCnfInfo->usDataLen    = gstUSIMMPOOL.astpoolRecord[ulFileNum].usLen;
        pstCnfInfo->usEfLen      = gstUSIMMPOOL.astpoolRecord[ulFileNum].usLen;

        return USIMM_SW_OK;
    }

    ulResult = USIMM_GetFileFromCard(pstFileInfo, &stCnfInfo);

    /* 给输出参数赋值 */
    VOS_MemCpy(pstCnfInfo, &stCnfInfo, sizeof(stCnfInfo));

    return ulResult;
}


VOS_UINT32 USIMM_GetUsimFileHandle(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    return USIMM_GetFileCommHandle(pstFileInfo, pstCnfInfo);
}


VOS_UINT32 USIMM_GetIsimIMPI(USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT8                           aucAsciiImsi[USIMM_EF_IMSI_LEN*2];
    VOS_UINT32                          ulAsciiImsiLen;
    VOS_UINT8                           aucAsciiMCC[4];
    VOS_UINT8                           aucAsciiMNC[4];
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucStr[]="@ims.mnc000.mcc000.3gppnetwork.org";
    VOS_UINT32                          ulStrLen;
    VOS_UINT32                          ulFileLen;

    ulResult = USIMM_ImsiBcd2Ascii(aucAsciiImsi);

    if (USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIMPI: decode imsi fail.");

        return USIMM_SW_ERR;
    }

    USIMM_GetAsciiMCCAndMNC(aucAsciiImsi, aucAsciiMCC, aucAsciiMNC);

    /* 按协议23.003 13.3节示例计算imsi内容 */
    /* 文件内容格式为"<IMSI>@ims.mnc<MNC>.mcc<MCC>.3gppnetwork.org" */
    ulStrLen = VOS_StrLen((VOS_CHAR *)aucStr);
    ulAsciiImsiLen = VOS_StrLen((VOS_CHAR *)aucAsciiImsi);

    /* 前2个字节为TL字段 */
    ulFileLen = 2 + ulStrLen + ulAsciiImsiLen;

    pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, ulFileLen);

    if (VOS_NULL_PTR == pstCnfInfo->pucEf)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIMPI:VOS_MemAlloc is Error");

        return USIMM_SW_ERR;
    }

    /* copy mnc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(aucStr+8, aucAsciiMNC, USIMM_MNC_MCC_FIG_NUM);

    /* copy mcc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(aucStr+15, aucAsciiMCC, USIMM_MNC_MCC_FIG_NUM);

    pstCnfInfo->pucEf[0] = USIMM_ISIM_FILE_TAG;
    pstCnfInfo->pucEf[1] = (VOS_UINT8)(ulFileLen - 2);

    /* copy imsi */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2]), aucAsciiImsi, ulAsciiImsiLen);

    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2])+ulAsciiImsiLen, aucStr, ulStrLen);

    pstCnfInfo->usEfLen   = (VOS_UINT16)ulFileLen;

    pstCnfInfo->usDataLen = (VOS_UINT16)ulFileLen;

    return USIMM_SW_OK;
}


VOS_UINT32 USIMM_GetIsimIMPU(USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT8                           aucAsciiImsi[USIMM_EF_IMSI_LEN*2];
    VOS_UINT32                          ulAsciiImsiLen;
    VOS_UINT8                           aucAsciiMCC[4];
    VOS_UINT8                           aucAsciiMNC[4];
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucStr1[] = "sip:";
    VOS_UINT32                          ulStr1Len;
    VOS_UINT8                           aucStr2[]="@ims.mnc000.mcc000.3gppnetwork.org";
    VOS_UINT32                          ulStr2Len;
    VOS_UINT32                          ulFileLen;

    ulResult = USIMM_ImsiBcd2Ascii(aucAsciiImsi);

    if (USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIMPI: decode imsi fail.");

        return USIMM_SW_ERR;
    }

    USIMM_GetAsciiMCCAndMNC(aucAsciiImsi, aucAsciiMCC, aucAsciiMNC);

    /* 按协议23.003 13.4B节示例计算文件内容 */
    /* 文件格式为"sip:<IMSI>@ims.mnc<MNC>.mcc<MCC>.3gppnetwork.org"*/
    ulStr1Len = VOS_StrLen((VOS_CHAR *)aucStr1);
    ulStr2Len = VOS_StrLen((VOS_CHAR *)aucStr2);
    ulAsciiImsiLen = VOS_StrLen((VOS_CHAR *)aucAsciiImsi);

    /* 前2个字节为TL字段 */
    ulFileLen = 2 + ulStr1Len + ulAsciiImsiLen + ulStr2Len;

    pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, ulFileLen);

    if (VOS_NULL_PTR == pstCnfInfo->pucEf)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIMPU:VOS_MemAlloc is Error");

        return USIMM_SW_ERR;
    }

    /* copy mnc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(aucStr2+8, aucAsciiMNC, USIMM_MNC_MCC_FIG_NUM);

    /* copy mcc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(aucStr2+15, aucAsciiMCC, USIMM_MNC_MCC_FIG_NUM);

    pstCnfInfo->pucEf[0] = USIMM_ISIM_FILE_TAG;
    pstCnfInfo->pucEf[1] = (VOS_UINT8)(ulFileLen - 2);

    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2]), aucStr1, ulStr1Len);

    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2]) + ulStr1Len, aucAsciiImsi, ulAsciiImsiLen);

    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2]) + ulStr1Len + ulAsciiImsiLen, aucStr2, ulStr2Len);

    pstCnfInfo->usEfLen = (VOS_UINT16)ulFileLen;

    pstCnfInfo->usDataLen = (VOS_UINT16)ulFileLen;

    return USIMM_SW_OK;
}


VOS_UINT32 USIMM_GetIsimDOMAIN(USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT8                           aucAsciiImsi[USIMM_EF_IMSI_LEN*2];
    VOS_UINT8                           aucAsciiMCC[4];
    VOS_UINT8                           aucAsciiMNC[4];
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucStr[]="ims.mnc000.mcc000.3gppnetwork.org";
    VOS_UINT32                          ulFileLen;

    ulResult = USIMM_ImsiBcd2Ascii(aucAsciiImsi);

    if (USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIMPI: decode imsi fail.");

        return USIMM_SW_ERR;
    }

    USIMM_GetAsciiMCCAndMNC(aucAsciiImsi, aucAsciiMCC, aucAsciiMNC);

    /* 按协议23.003 13.2节示例计算文件内容 */
    /* 文件格式为"ims.mnc<MNC>.mcc<MCC>.3gppnetwork.org" */
    /* 前2个字节为TL字段 */
    ulFileLen = 2 + VOS_StrLen((VOS_CHAR *)aucStr);

    pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, ulFileLen);

    if (VOS_NULL_PTR == pstCnfInfo->pucEf)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimDOMAIN:VOS_MemAlloc is Error");

        return USIMM_SW_ERR;
    }

    /* copy mnc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy((aucStr+7), aucAsciiMNC, USIMM_MNC_MCC_FIG_NUM);

    /* copy mcc */
    /* coverity[uninit_use_in_call] */
    VOS_MemCpy((aucStr+14), aucAsciiMCC, USIMM_MNC_MCC_FIG_NUM);

    pstCnfInfo->pucEf[0] = USIMM_ISIM_FILE_TAG;
    pstCnfInfo->pucEf[1] = (VOS_UINT8)(ulFileLen - 2);

    /* coverity[uninit_use_in_call] */
    VOS_MemCpy(&(pstCnfInfo->pucEf[2]), (VOS_UINT8 *)aucStr, ulFileLen);

    pstCnfInfo->usEfLen = (VOS_UINT16)ulFileLen;

    pstCnfInfo->usDataLen = (VOS_UINT16)ulFileLen;

    return USIMM_SW_OK;
}


VOS_UINT32 USIMM_GetIsimIST(USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;
    VOS_UINT8                           ucSVRContent = 0;
    VOS_UINT16                          usFileLen;
    /*lint -e656*/
    VOS_UINT8                           aucSVRBit[ISIM_SVR_BUTT - USIM_SVR_BUTT];
    /*lint +e656*/
    VOS_UINT32                          ulArrayIndex;

    /*lint -e656*/
    usFileLen = ((ISIM_SVR_BUTT - USIM_SVR_BUTT) + USIMM_BIT_NUM_IN_BYTE - 1) / USIMM_BIT_NUM_IN_BYTE;
    /*lint +e656*/

    for(i = ISIM_SVR_PCSCF_ADDR; i < ISIM_SVR_BUTT; i++)
    {
        /* 调用接口获取服务位状态 */
        aucSVRBit[i-ISIM_SVR_PCSCF_ADDR] = (VOS_UINT8)USIMM_IsISIMServiceAvailable(i);
    }

    pstCnfInfo->pucEf = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_USIM, DYNAMIC_MEM_PT, usFileLen);

    if (VOS_NULL_PTR == pstCnfInfo->pucEf)
    {
        USIMM_ERROR_LOG("USIMM_GetIsimIST:VOS_MemAlloc is Error");

        return USIMM_SW_ERR;
    }

    for(i = 0; i < usFileLen; i++)
    {
        ucSVRContent = 0;

        for(j = 0; j < USIMM_BIT_NUM_IN_BYTE; j++)
        {
            ulArrayIndex = (i*USIMM_BIT_NUM_IN_BYTE + j);

            /*lint -e656*/
            if (ulArrayIndex < (ISIM_SVR_BUTT - USIM_SVR_BUTT))
            {
                ucSVRContent = (VOS_UINT8)(ucSVRContent | ((aucSVRBit[ulArrayIndex] & 0x1) << j));
            }
            /*lint +e656*/
        }

        pstCnfInfo->pucEf[i] = ucSVRContent;
    }

    pstCnfInfo->usEfLen = usFileLen;

    pstCnfInfo->usDataLen = usFileLen;

    return USIMM_SW_OK;
}
VOS_UINT32 USIMM_GetIsimFileHandle(USIMM_GET_COMM_FILE_STRU *pstFileInfo, USIMM_GETCNF_INFO_STRU *pstCnfInfo)
{
    VOS_UINT16                          usFileID = VOS_NULL_WORD;
    VOS_UINT32                          ulResult = VOS_OK;

    /* 当前ISIM卡服务不可用 */
    if (USIMM_CARD_SERVIC_AVAILABLE != gastUSIMMCardAppInfo[USIMM_UICC_ISIM].enCardService)
    {
        USIMM_WARNING_LOG("USIMM_GetIsimFileHandle: ISIM app unavailable");

        return USIMM_SW_ERR;
    }

    if (EFISIMIMPI == *pstFileInfo->pusFilePath)
    {
        return USIMM_GetIsimIMPI(pstCnfInfo);
    }

    if (EFISIMDOMAIN == *pstFileInfo->pusFilePath)
    {
        return USIMM_GetIsimDOMAIN(pstCnfInfo);
    }

    if (EFISIMIMPU == *pstFileInfo->pusFilePath)
    {
        return USIMM_GetIsimIMPU(pstCnfInfo);
    }

    if (EFISIMIST == *pstFileInfo->pusFilePath)
    {
        return USIMM_GetIsimIST(pstCnfInfo);
    }

    /* 其他文件根据ISIM文件ID查找对应的USIM文件ID 找不到对应的USIM文件ID则认为文件不可读取 */
    usFileID = USIMM_IsimEFIDToUsimEFID(*pstFileInfo->pusFilePath);

    /* 找不到对应的USIM文件ID */
    if (VOS_NULL_WORD == usFileID)
    {
        /* 文件不可读取 */
        USIMM_WARNING_LOG("USIMM_GetIsimFileHandle: ISIM file can't read");

        return USIMM_SW_NOFILE_FOUND;
    }

    /* 按USIM文件id读取文件 */
    *pstFileInfo->pusFilePath  = usFileID;
    pstFileInfo->enAppType = USIMM_UMTS_APP;

    /* 调用USIM接口读取USIM文件 */
    ulResult = USIMM_GetFileCommHandle(pstFileInfo, pstCnfInfo);

    return ulResult;

}


VOS_UINT32 USIMM_GetRealFile(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    USIMM_GETFILE_REQ_STRU              *pstMsg;
    USIMM_GETCNF_INFO_STRU              stCnfInfo;
    USIMM_GET_COMM_FILE_STRU            stGetFileInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));
    VOS_MemSet(&stGetFileInfo, 0, sizeof(stGetFileInfo));

    pstMsg = (USIMM_GETFILE_REQ_STRU*)pMsg;

    stGetFileInfo.enAppType     = pstMsg->enAppType;
    stGetFileInfo.ucRecordNum   = pstMsg->ucRecordNum;
    stGetFileInfo.pusFilePath   = &pstMsg->usFileID;
    stGetFileInfo.ulPathLen     = 1;

    if (USIMM_ISIM_APP == pstMsg->enAppType)
    {
        ulResult = USIMM_GetIsimFileHandle(&stGetFileInfo, &stCnfInfo);
    }
    else
    {
        ulResult = USIMM_GetUsimFileHandle(&stGetFileInfo, &stCnfInfo);
    }

    stCnfInfo.ucRecordNum      = pstMsg->ucRecordNum;
    stCnfInfo.usEfId           = pstMsg->usFileID;

    USIMM_GetFileCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara, ulResult, &stCnfInfo);

    if(VOS_NULL_PTR != stCnfInfo.pucEf)
    {
        VOS_MemFree(WUEPS_PID_USIM, stCnfInfo.pucEf);
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_GetFileHandle(USIMM_MsgBlock *pMsg)
{
#if (FEATURE_VSIM == FEATURE_ON)
    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM is Active*/
    {
        return USIMM_GetVsimFile((USIMM_GETFILE_REQ_STRU*)pMsg);
    }
#endif

    /*硬卡操作*/
    return USIMM_GetRealFile(pMsg);
}


VOS_UINT32 USIMM_GetMaxRecordNum(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                  ulResult;
    USIMM_MAXRECORD_REQ_STRU    *pstMsg;
    USIMM_FILECNF_INFO_STRU     stCnfInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    pstMsg = (USIMM_MAXRECORD_REQ_STRU*)pMsg;

    stCnfInfo.usEFId = pstMsg->usEfId;

    ulResult = USIMM_SelectFile(pstMsg->enAppType, USIMM_NEED_FCP, 1, &pstMsg->usEfId);

    if(VOS_OK != ulResult)/*判断文件的选择结果*/
    {
        USIMM_ERROR_LOG("USIMM_GetMaxRecordNum: The File Not Exist");

        USIMM_MaxRecordNumCnf(pstMsg->ulSenderPid, ulResult, &stCnfInfo);

        return VOS_ERR;
    }

    if(USIMM_EFSTRUCTURE_TRANSPARENT == gstUSIMMCurFileInfo.stEFInfo.enFileType)/*判断文件的类型*/
    {
        USIMM_ERROR_LOG("USIMM_GetMaxRecordNum: The File Type is Wrong");

        ulResult = USIMM_SW_FILETYPE_ERROR;
    }
    else                    /*对于记录文件和循环文件都认为成功*/
    {
        USIMM_INFO_LOG("USIMM_GetMaxRecordNum: The File Select is OK");

        ulResult                = USIMM_SW_OK;

        stCnfInfo.ucRecordLen   = gstUSIMMCurFileInfo.stEFInfo.ucRecordLen;

        stCnfInfo.ucRecordNum   = gstUSIMMCurFileInfo.stEFInfo.ucRecordNum;
    }

    stCnfInfo.ulFileStatus          = gstUSIMMCurFileInfo.stEFInfo.enFileStatus;

    stCnfInfo.ulFileReadUpdateFlag  = gstUSIMMCurFileInfo.stEFInfo.enFileReadUpdateFlag;

    USIMM_MaxRecordNumCnf(pstMsg->ulSenderPid,ulResult,&stCnfInfo);

    return ulResult;
}
VOS_UINT32 USIMM_GAccessHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    USIMM_GACCESS_REQ_STRU              *pstMsg;
    USIMM_CSIM_CNF_STRU                 stData;
    VOS_UINT32                          ulPathLen;
    VOS_UINT16                          ausPath[USIMM_MAX_PATH_LEN];
    USIMM_SELECT_PARA_STUR              stSelectPara;
    USIMM_U16_LVDATA_STRU               stFilePath;

    pstMsg = (USIMM_GACCESS_REQ_STRU*)pMsg;

    VOS_MemSet(&stData, 0, sizeof(stData));

    stData.ucINS = pstMsg->aucContent[INS];

    /* TMO不需要+CSIM命令 */
    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulTmo_flg)
    {
        USIMM_GenericAccessCnf(pstMsg->ulSenderPid, USIMM_SW_ERR, 0, VOS_NULL_PTR, &stData);

        return VOS_ERR;
    }

    if(CMD_INS_SELECT == pstMsg->aucContent[INS])   /* 对于超过路径和应用选中方式不支持 */
    {
        if( (pstMsg->aucContent[P1] == USIMM_SELECT_BY_DF_NAME)||(pstMsg->aucContent[P3] > USIMM_MAX_PATH_LEN*2) )
        {
            USIMM_ERROR_LOG("USIMM_GAccessHandle: The USIMM_SelectFile Return Error");

            USIMM_GenericAccessCnf(pstMsg->ulSenderPid, USIMM_SW_ERR, 0, VOS_NULL_PTR, &stData);

            return VOS_ERR;
        }
    }

    if (pstMsg->ulPathLen != 0) /*需要重新选中之前的路径*/
    {
        ulResult = USIMM_SelectFile(USIMM_UNLIMIT_APP, USIMM_NEED_FCP, pstMsg->ulPathLen, pstMsg->ausPath);

        if(ulResult != VOS_OK)
        {
            USIMM_ERROR_LOG("USIMM_GAccessHandle: The USIMM_SelectFile Return Error");
        }
    }

    VOS_MemSet(&gstUSIMMAPDU, 0, sizeof(gstUSIMMAPDU));

    if (VOS_OK != USIMM_FormatCsimApdu(pstMsg, &gstUSIMMAPDU))  /*组合成标准格式*/
    {
        USIMM_GenericAccessCnf(pstMsg->ulSenderPid, USIMM_SW_ERR, 0, VOS_NULL_PTR, &stData);

        USIMM_ERROR_LOG("USIMM_GAccessHandle: The USIMM_FormatCsimApdu Return Error");

        VOS_MemSet(&gstUSIMMAPDU, 0, sizeof(gstUSIMMAPDU));

        return VOS_ERR;
    }

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU); /* 调用链路层函数 */

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SearchHandle: DL Handle is Failed");;   /* coverity by yangzhi */
    }

    if((CMD_INS_SELECT == pstMsg->aucContent[INS])&&(VOS_OK == ulResult)&&(gstUSIMMAPDU.ucSW1 == 0x90))   /*当有新的选择过程时候需要重新更新当前地址*/
    {
        stSelectPara.enCardType = gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType;

        stSelectPara.enFileApp  = USIMM_UNLIMIT_APP;

        /* 增加T=1协议后不能再根据response的INS判断是都需要FCP了，只能看是否有响应数据 */
        if(VOS_NULL != gstUSIMMAPDU.ulRecDataLen)
        {
            stSelectPara.enEfFcpFlag = USIMM_NEED_FCP;
        }
        else
        {
            stSelectPara.enEfFcpFlag = USIMM_NO_NEED_FCP;
        }

        USIMM_ChangePathToU16(pstMsg->aucContent[P3], &pstMsg->aucContent[P3+1], &ulPathLen, ausPath);

        stFilePath.pusData      = ausPath;

        stFilePath.ulDataLen    = ulPathLen;

        if(VOS_NULL != gstUSIMMAPDU.ulRecDataLen)
        {
            USIMM_DecodeFileFcp(gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType, &stFilePath, &gstUSIMMAPDU, &gstUSIMMCurFileInfo);
        }

        USIMM_UpdateFileByRelativePath(&stSelectPara, &stFilePath, &gstUSIMMCurFileInfo);
    }

    stData.usDataLen= (VOS_UINT16)gstUSIMMAPDU.ulRecDataLen;
    stData.pucData  = gstUSIMMAPDU.aucRecvBuf;
    stData.ucSw1    = gstUSIMMAPDU.ucSW1;
    stData.ucSw2    = gstUSIMMAPDU.ucSW2;

    USIMM_GenericAccessCnf(pstMsg->ulSenderPid, ulResult, gstUSIMMCurFileInfo.usFilePathLen, gstUSIMMCurFileInfo.ausFilePath, &stData);

    return ulResult;
}
VOS_UINT32 USIMM_RAccessPathHandle(
    VOS_UINT16                          usFileID,
    VOS_UINT16                          usPathLen,
    VOS_UINT16                         *pusPath,
    USIMM_RESTRIC_CMD_ENUM_UINT32       ulCmdType)
{
    VOS_UINT16                          ausFilePath[USIMM_MAX_PATH_LEN];
    VOS_UINT32                          ulResult;

    if((usPathLen+1) > USIMM_MAX_PATH_LEN)
    {
        USIMM_ERROR_LOG("USIMM_RAccessPathHandle: The File Path Len is error");

        return USIMM_SW_ERR;
    }

    /* GET RESPONSE命令清除文件信息，确保文件可以重新选择 */
    if (USIMM_GET_RESPONSE == ulCmdType)
    {
        USIMM_ClearCurFileInfo();
    }

    /* AT命令中如果有把文件路径带下来，拼接文件路径后选择文件。
       如果没有文件路径，直接按文件ID选文件*/
    if (0 == usPathLen)
    {
        /* 如果选不中，再按电话本的应用尝试选择文件 */
        ulResult = USIMM_SelectFile(USIMM_UNLIMIT_APP, USIMM_NEED_FCP, 1, &usFileID);

        if ((USIMM_SW_OK != ulResult)&&(USIMM_SW_SENDCMD_ERROR != ulResult))
        {
            ulResult = USIMM_SelectFile(USIMM_PB_APP, USIMM_NEED_FCP, 1, &usFileID);
        }
    }
    else
    {
        /* 拷贝文件路径 */
        VOS_MemCpy(ausFilePath, pusPath, usPathLen*sizeof(VOS_UINT16));

        /* 拷贝文件ID */
        VOS_MemCpy((ausFilePath + usPathLen), &usFileID, sizeof(usFileID));

        /* 按路径选择文件，路径长度需要加1，把文件ID带上 */
        ulResult = USIMM_SelectFile(USIMM_UNLIMIT_APP, USIMM_NEED_FCP, (usPathLen+1), ausFilePath);
    }

    return ulResult;
}


VOS_UINT32 USIMM_RAccessHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult   = VOS_OK;
    VOS_UINT16                          usOffset;
    VOS_UINT32                          ulNeedCopy          = VOS_NO;
    USIMM_RACCESS_REQ_STRU             *pstMsg;
    VOS_UINT8                           ucLen               = 0;
    VOS_UINT8                          *pucContent          = VOS_NULL_PTR;
    NAS_NVIM_SYSTEM_APP_CONFIG_STRU     stSystemAppConfig;
    USIMM_RACCESSCNF_INFO_STRU          stCnfInfo           = {0};
    VOS_UINT32                          i = 0;

    pstMsg = (USIMM_RACCESS_REQ_STRU*)pMsg;

    /* 兼容问题卡，那张卡在envelop命令后再执行status就会出现之前选中的文件未选中,
    状态字返回USIMM_SW_NOFILE_SELECT，此时清空之前选中的文件路径，重新选择文件，操作文件；
    对于正常卡，不需要循环，执行一次就退出 */
    while(i < 2)
    {

        /*如果当前有数据返回，那么需要清空保存数据到全局变量*/
        VOS_MemSet(&gstUSIMMAPDU, 0, sizeof(gstUSIMMAPDU));

        if(VOS_NULL_WORD != pstMsg->usEfId)
        {
            ulResult = USIMM_RAccessPathHandle(pstMsg->usEfId,
                                               pstMsg->usPathLen,
                                               pstMsg->ausPath,
                                               pstMsg->enCmdType);
        }

        if(USIMM_SW_OK != ulResult)/*判断选择结果*/
        {
            stCnfInfo.ulErrorCode = ulResult;

            stCnfInfo.ucSW1 = gstUSIMMAPDU.ucSW1;

            stCnfInfo.ucSW2 = gstUSIMMAPDU.ucSW2;

            USIMM_RestrictedAccessCnf(pstMsg->ulSenderPid,
                                      pstMsg->ulSendPara,
                                      VOS_OK,
                                      &stCnfInfo);

            return VOS_OK;
        }

        switch (pstMsg->enCmdType)
        {
            case USIMM_READ_BINARY:
                usOffset   = ((VOS_UINT16)(pstMsg->ucP1<< 8) & 0xFF00) + pstMsg->ucP2;
                ulNeedCopy = VOS_YES;                                                                 /*有数据需要拷贝*/
                ulResult   = USIMM_ReadBinary_APDU(usOffset, pstMsg->ucP3);
                break;

            case USIMM_READ_RECORD:
                ulNeedCopy = VOS_YES;                                                                 /*有数据需要拷贝*/
                ulResult   = USIMM_ReadRecord_APDU(pstMsg->ucP1, pstMsg->ucP2, pstMsg->ucP3);
                break;

            case USIMM_UPDATE_BINARY:
                usOffset = ((VOS_UINT16)(pstMsg->ucP1 << 8) & 0xFF00) + pstMsg->ucP2;
                ulResult = USIMM_UpdateBinary_APDU(usOffset, pstMsg->ucP3, pstMsg->aucContent);
                break;

            case USIMM_UPDATE_RECORD:
                ulResult = USIMM_UpdateRecord_APDU(pstMsg->ucP1,pstMsg->ucP2,pstMsg->ucP3,pstMsg->aucContent);
                break;

            case USIMM_STATUS:
                ulNeedCopy = VOS_YES;                                                          /*有数据需要拷贝*/
                ulResult   = USIMM_Status_APDU(pstMsg->ucP1, pstMsg->ucP2, pstMsg->ucP3);
                break;
            case USIMM_AUTHENTICATION:
                ulNeedCopy = VOS_YES;
                ulResult   = USIMM_Authentication_APDU(pstMsg->ucP2, pstMsg->aucContent, pstMsg->ucP3);
                break;

            case USIMM_GET_RESPONSE:
                ulNeedCopy = VOS_YES;
                break;

            default:
                ulResult =  USIMM_SW_SENDCMD_ERROR;
                break;
        }

        if (VOS_OK != ulResult)/*检查发送结果*/
        {
            USIMM_ERROR_LOG("USIMM_RAccessHandle: Send Command APDU is Failed");

            stCnfInfo.ulErrorCode = USIMM_SW_SENDCMD_ERROR;

            USIMM_RestrictedAccessCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara, VOS_ERR, &stCnfInfo);

            return VOS_ERR;
        }

        ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

        if (USIMM_SW_NOFILE_SELECT == ulResult)
        {
            i++;
            USIMM_ClearCurFileInfo();
        }
        else
        {
            break;
        }
    }

    if (USIMM_SW_OK != ulResult)/*检查结果*/
    {
        USIMM_WARNING_LOG("USIMM_RAccessHandle: The Command Result is Failed");

        ucLen = 0;

        ulNeedCopy = VOS_NO;
    }

    /* 规避MP和ANDROID在GET RESPONSE命令对P3参数的差异 */
    if (VOS_OK != NV_Read(en_NV_Item_System_APP_Config, &stSystemAppConfig, sizeof(NAS_NVIM_SYSTEM_APP_CONFIG_STRU)))
    {
        stSystemAppConfig.usSysAppConfigType =   SYSTEM_APP_MP;
    }

    if (VOS_YES == ulNeedCopy) /*有数据拷贝*/
    {
        ucLen       = (VOS_UINT8)gstUSIMMAPDU.ulRecDataLen;

        pucContent  = &gstUSIMMAPDU.aucRecvBuf[0];

        /* 在非ANDROID上，返回P3参数指定的长度。在ANDROID上返回实际长度 */
        if ((USIMM_GET_RESPONSE == pstMsg->enCmdType)
            && (SYSTEM_APP_ANDROID != stSystemAppConfig.usSysAppConfigType))
        {
            ucLen = (ucLen > pstMsg->ucP3)?pstMsg->ucP3:ucLen;
        }
    }

    stCnfInfo.ulErrorCode = ulResult;

    stCnfInfo.ucSW1 = gstUSIMMAPDU.ucSW1;

    stCnfInfo.ucSW2 = gstUSIMMAPDU.ucSW2;

    stCnfInfo.ucLen = ucLen;

    stCnfInfo.pContent = pucContent;

    USIMM_RestrictedAccessCnf(pstMsg->ulSenderPid, pstMsg->ulSendPara, VOS_OK, &stCnfInfo);

    return VOS_OK;
}


VOS_UINT32  USIMM_SearchHandle(USIMM_MsgBlock *pMsg)
{
    USIMM_SEARCH_REQ_STRU       *pstMsg;
    VOS_UINT32                  ulResult;
    USIMM_SEARCHCNF_INFO_STRU   stCnfInfo;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    pstMsg = (USIMM_SEARCH_REQ_STRU *)pMsg;

    stCnfInfo.usEfid = pstMsg->usEfId;

    ulResult = USIMM_SelectFile(pstMsg->enAppType, USIMM_NEED_FCP, 1, &pstMsg->usEfId); /*调用对应的API*/

    if (VOS_OK != ulResult)
    {
        USIMM_SearchHandleCnf(pstMsg->ulSenderPid, ulResult, &stCnfInfo);
        return VOS_OK;
    }

    if (USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        ulResult = USIMM_Search_APDU(0x01, 0x04, pstMsg->ucLen, pstMsg->aucContent);
    }
    else
    {
        ulResult = USIMM_Search_APDU(0x00, 0x10, pstMsg->ucLen, pstMsg->aucContent);
    }

    if (VOS_OK != ulResult)
    {
        USIMM_SearchHandleCnf(pstMsg->ulSenderPid, ulResult, &stCnfInfo);
        return VOS_OK;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);/*检查选择结果*/

    if(USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SearchHandle: check SW Result is Failed");   /* coverity by yangzhi */
    }

    stCnfInfo.pucData = gstUSIMMAPDU.aucRecvBuf;

    stCnfInfo.ulLen   = gstUSIMMAPDU.ulRecDataLen;

    stCnfInfo.usTotalRecNum = gstUSIMMCurFileInfo.stEFInfo.ucRecordNum;;

    USIMM_SearchHandleCnf(pstMsg->ulSenderPid, ulResult, &stCnfInfo);

    return VOS_OK;
}
VOS_UINT32 USIMM_PBInitStatusIndHandle(USIMM_MsgBlock *pMsg)
{
    USIMM_PB_INIT_STATUS_IND_STRU   *pstMsg;

    pstMsg = (USIMM_PB_INIT_STATUS_IND_STRU *)pMsg;

    gstUSIMMBaseInfo.stPBInfo.enPBInitStatus = pstMsg->enPBInitStatus;

    /*电话本初始化完成，可以保存数据*/
    if(USIMM_PB_IDLE == pstMsg->enPBInitStatus)
    {
        USIMM_SaveLogFile();
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_SIMAuthHandle(USIMM_AUTH_REQ_STRU *pstMsg)
{
    USIMM_TELECOM_AUTH_INFO_STRU        stCnfInfo;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          ausSimPath[]={MF, DFGSM};
    VOS_UINT8                           aucGsmRes[USIMM_2G_AUTH_SRES_LEN + 1];
    VOS_UINT8                           aucGsmKc[USIMM_2G_AUTH_KC_LEN + 1];

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    if (USIMM_CARD_SIM != gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        USIMM_ERROR_LOG("USIMM_SIMAuthHandle: SIM is disable.");

        stCnfInfo.enResult = USIMM_AUTH_GSM_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* 检测鉴权参数是否正确 */
    if (USIMM_2G_AUTH != pstMsg->enAuthType)
    {
        USIMM_ERROR_LOG("USIMM_SIMAuthHandle: enAuthType is incorrcet.");

        stCnfInfo.enResult = USIMM_AUTH_GSM_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_SelectFile(USIMM_GSM_APP, USIMM_NEED_FCP, ARRAYSIZE(ausSimPath), ausSimPath);

    if(VOS_OK != ulResult) /*判断结果*/
    {
        USIMM_ERROR_LOG("USIMM_SIMAuthHandle: Select DFGSM Error");

        stCnfInfo.enResult = USIMM_AUTH_GSM_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* SIM卡下发鉴权数据不需要长度字段 */
    ulResult = USIMM_Authentication_APDU(USIMM_SIM_2G_AUTH_MODE, pstMsg->aucData + 1, pstMsg->ulDataLen - 1);

    if (VOS_OK != ulResult) /*判断鉴权发送结果*/
    {
        USIMM_ERROR_LOG("USIMM_SIMAuthHandle: Send SIM Authentication APDU Error");

        stCnfInfo.enResult = USIMM_AUTH_GSM_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if (USIMM_SW_AUTN_ERROR == ulResult)
    {
        USIMM_WARNING_LOG("USIMM_SIMAuthHandle: SIM Check SW Error");

        stCnfInfo.enResult = USIMM_AUTH_MAC_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    if (VOS_OK != ulResult)
    {
        stCnfInfo.enResult = USIMM_AUTH_GSM_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* SIM卡鉴权返回数据没有长度字段，需要添加 */
    aucGsmRes[0] = USIMM_2G_AUTH_SRES_LEN;
    VOS_MemCpy(&aucGsmRes[1], &gstUSIMMAPDU.aucRecvBuf[0], USIMM_2G_AUTH_SRES_LEN);

    aucGsmKc[0]  = USIMM_2G_AUTH_KC_LEN;
    VOS_MemCpy(&aucGsmKc[1], &gstUSIMMAPDU.aucRecvBuf[USIMM_2G_AUTH_SRES_LEN], USIMM_2G_AUTH_KC_LEN);

    stCnfInfo.pucGsmKC   = aucGsmKc;
    stCnfInfo.pucAuthRes = aucGsmRes;

    USIMM_INFO_LOG("USIMM_SIMAuthHandle: SIM Authentication Right");

    stCnfInfo.enResult = USIMM_AUTH_GSM_SUCCESS;

    USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

    return VOS_OK;

}
VOS_UINT32 USIMM_USIMAuthHandle(USIMM_AUTH_REQ_STRU *pstMsg)
{
    USIMM_TELECOM_AUTH_INFO_STRU        stCnfInfo;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          ausUsimPath[]={MF, ADF};
    VOS_UINT8                           ucMode;
    VOS_UINT32                          ulOffset = 1;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    if (USIMM_CARD_USIM != gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        USIMM_ERROR_LOG("USIMM_USIMAuthHandle: SIM is disable.");

        stCnfInfo.enResult = USIMM_AUTH_UMTS_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* 参数检测并确定鉴权内容参数 */
    if (USIMM_2G_AUTH == pstMsg->enAuthType)
    {
        ucMode = USIMM_USIM_2G_AUTH_MODE;
    }
    else if (USIMM_3G_AUTH == pstMsg->enAuthType)
    {
        ucMode = USIMM_USIM_3G_AUTH_MODE;
    }
    else
    {
        USIMM_ERROR_LOG("USIMM_USIMAuthHandle: enAuthType is incorrect.");

        stCnfInfo.enResult = USIMM_AUTH_UMTS_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_SelectFile(USIMM_UMTS_APP, USIMM_NEED_FCP, ARRAYSIZE(ausUsimPath), ausUsimPath);

    if(VOS_OK != ulResult)/*判断结果*/
    {
        USIMM_ERROR_LOG("USIMM_USIMAuthHandle: Select ADF Error");

        stCnfInfo.enResult = USIMM_AUTH_UMTS_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_Authentication_APDU(ucMode, pstMsg->aucData, pstMsg->ulDataLen);

    if(VOS_OK != ulResult)/*判断鉴权发送结果*/
    {
        USIMM_ERROR_LOG("USIMM_USIMAuthHandle: Send USIM Authentication APDU Error");

        stCnfInfo.enResult = USIMM_AUTH_UMTS_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if(USIMM_SW_AUTN_ERROR == ulResult)
    {
        USIMM_WARNING_LOG("USIMM_USIMAuthHandle: SIM Check SW Error");

        stCnfInfo.enResult = USIMM_AUTH_MAC_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    if (VOS_OK != ulResult)
    {
        stCnfInfo.enResult = USIMM_AUTH_UMTS_OTHER_FAILURE;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* 当前USIM卡的GSM鉴权 */
    if (USIMM_2G_AUTH == pstMsg->enAuthType)
    {
        stCnfInfo.pucAuthRes = &gstUSIMMAPDU.aucRecvBuf[0];

        stCnfInfo.pucGsmKC   = &gstUSIMMAPDU.aucRecvBuf[USIMM_2G_AUTH_SRES_LEN + 1];

        USIMM_INFO_LOG("USIMM_USIMAuthHandle: USIM GSM Authentication right");

        stCnfInfo.enResult = USIMM_AUTH_GSM_SUCCESS;

        USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

        return VOS_OK;
    }

    /* 3G 鉴权成功 */
    if (USIMM_AUTH_SUCCESS_TAG == gstUSIMMAPDU.aucRecvBuf[0])
    {
        stCnfInfo.enResult  = USIMM_AUTH_UMTS_SUCCESS;

        stCnfInfo.pucAuthRes= &gstUSIMMAPDU.aucRecvBuf[ulOffset];/*指向Res*/

        ulOffset            += gstUSIMMAPDU.aucRecvBuf[ulOffset] + 1;/*指向CK*/

        stCnfInfo.pucCK     = &gstUSIMMAPDU.aucRecvBuf[ulOffset];

        ulOffset            += gstUSIMMAPDU.aucRecvBuf[ulOffset] + 1;/*指向IK*/

        stCnfInfo.pucIK     = &gstUSIMMAPDU.aucRecvBuf[ulOffset];

        ulOffset            += gstUSIMMAPDU.aucRecvBuf[ulOffset] + 1;/*指向后面可能的Kc*/

        if(ulOffset < gstUSIMMAPDU.ucSW2)
        {
            stCnfInfo.pucGsmKC = &gstUSIMMAPDU.aucRecvBuf[ulOffset];

            USIMM_INFO_LOG("USIMM_USIMAuthHandle: USIM 3G Authentication have the KC Data");
        }

        USIMM_INFO_LOG("USIMM_USIMAuthHandle: USIM 3G Authenctication Right");
    }
    else if (USIMM_AUTH_SYNC_FAIL_TAG == gstUSIMMAPDU.aucRecvBuf[0]) /* 3G重同步 */
    {
        stCnfInfo.enResult  = USIMM_AUTH_SYNC_FAILURE;

        stCnfInfo.pucAuts   = &gstUSIMMAPDU.aucRecvBuf[1];

        USIMM_WARNING_LOG("USIMM_USIMAuthHandle: USIM 3G Authentication Need ReSynchronization");
    }
    else                                                                /*其它数据错误*/
    {
        USIMM_ERROR_LOG("USIMM_USIMAuthHandle: USIM Authentication Response Data Error");

        ulResult = USIMM_AUTH_UMTS_OTHER_FAILURE;/* [false alarm]:屏蔽Fortify错误 */
    }

    USIMM_TelecomAuthCnf(pstMsg, &stCnfInfo);

    return VOS_OK;
}


VOS_UINT32 USIMM_ISIMAuthParaCheck(USIMM_AUTH_REQ_STRU *pstMsg, VOS_UINT8 *pucMode)
{
    VOS_UINT32                          ulIMSSVR;

    /* 对于AKA鉴权，ISIM与USIM相同，在USIM模拟IMS的情况，直接下发鉴权，
    不用判断95号服务位，其他鉴权需要判断 */
    if (USIMM_IMS_AUTH == pstMsg->enAuthType)
    {
        *pucMode = USIMM_USIM_3G_AUTH_MODE;

        return VOS_OK;
    }

    /* 调用USIM服务接口，判断USIM卡是否支持IMS */
    ulIMSSVR = USIMM_IsUSIMServiceAvailable(USIM_SVR_ACCESS_IMS);

    if(PS_USIM_SERVICE_AVAILIABLE != ulIMSSVR)
    {
        return VOS_ERR;
    }

    if ((USIMM_GBA_AUTH == pstMsg->enAuthType) || (USIMM_NAF_AUTH == pstMsg->enAuthType))
    {
        *pucMode = USIMM_USIM_GBA_AUTH_MODE;

        return VOS_OK;
    }

    return VOS_ERR;
}
VOS_UINT32 USIMM_ISIMAuthHandle(USIMM_AUTH_REQ_STRU *pstMsg)
{
    USIMM_IMS_AUTH_INFO_STRU            stCnfInfo;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          ausUsimPath[]={MF, ADF};
    VOS_UINT8                           ucMode;
    VOS_UINT32                          ulOffset = 1;

    VOS_MemSet(&stCnfInfo, 0, sizeof(stCnfInfo));

    if (USIMM_CARD_ISIM != gastUSIMMCardAppInfo[USIMM_UICC_ISIM].enCardType)
    {
        USIMM_ERROR_LOG("USIMM_ISIMAuthHandle: ISIM is disable.");

        stCnfInfo.enResult = USIMM_AUTH_IMS_OTHER_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* 参数检测并确定鉴权内容参数 */
    if (VOS_OK != USIMM_ISIMAuthParaCheck(pstMsg, &ucMode))
    {
        USIMM_ERROR_LOG("USIMM_ISIMAuthHandle: enAuthType is wrong.");

        stCnfInfo.enResult = USIMM_AUTH_IMS_OTHER_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_SelectFile(USIMM_UMTS_APP, USIMM_NEED_FCP, ARRAYSIZE(ausUsimPath), ausUsimPath);

    if (VOS_OK != ulResult)/*判断结果*/
    {
        USIMM_ERROR_LOG("USIMM_ISIMAuthHandle: Select ADF Error");

        stCnfInfo.enResult = USIMM_AUTH_IMS_OTHER_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_Authentication_APDU(ucMode, pstMsg->aucData, pstMsg->ulDataLen);

    if (VOS_OK != ulResult)/*判断鉴权发送结果*/
    {
        USIMM_ERROR_LOG("USIMM_ISIMAuthHandle: Send USIM Authentication APDU Error");

        stCnfInfo.enResult = USIMM_AUTH_IMS_OTHER_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if (USIMM_SW_AUTN_ERROR == ulResult)
    {
        USIMM_WARNING_LOG("USIMM_ISIMAuthHandle: SIM Check SW Error");

        stCnfInfo.enResult = USIMM_AUTH_MAC_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    if (VOS_OK != ulResult)
    {
        stCnfInfo.enResult = USIMM_AUTH_IMS_OTHER_FAILURE;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_ERR;
    }

    /* 鉴权成功 */
    if (USIMM_AUTH_SUCCESS_TAG == gstUSIMMAPDU.aucRecvBuf[0])
    {
        if (USIMM_IMS_AUTH == pstMsg->enAuthType)
        {
            stCnfInfo.pucAutsRes    = &gstUSIMMAPDU.aucRecvBuf[ulOffset];/*指向Res*/

            ulOffset                += gstUSIMMAPDU.aucRecvBuf[ulOffset] + 1;/*指向CK*/

            stCnfInfo.pucCK         = &gstUSIMMAPDU.aucRecvBuf[ulOffset];

            ulOffset                += gstUSIMMAPDU.aucRecvBuf[ulOffset] + 1;/*指向IK*/

            stCnfInfo.pucIK         = &gstUSIMMAPDU.aucRecvBuf[ulOffset];
        }
        else if (USIMM_GBA_AUTH == pstMsg->enAuthType)
        {
            stCnfInfo.pucAutsRes    = &gstUSIMMAPDU.aucRecvBuf[ulOffset];
        }
        else
        {
            stCnfInfo.pucKs_ext_NAF = &gstUSIMMAPDU.aucRecvBuf[ulOffset];
        }

        stCnfInfo.enResult  = USIMM_AUTH_IMS_SUCCESS;

        USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

        return VOS_OK;
    }

    /* 同步失败 */
    if (USIMM_AUTH_SYNC_FAIL_TAG == gstUSIMMAPDU.aucRecvBuf[0])
    {
        if ((USIMM_IMS_AUTH == pstMsg->enAuthType) || (USIMM_GBA_AUTH == pstMsg->enAuthType))
        {
            stCnfInfo.pucAuts  = &gstUSIMMAPDU.aucRecvBuf[ulOffset];

            stCnfInfo.enResult = USIMM_AUTH_SYNC_FAILURE;

            USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

            return VOS_ERR;
        }
    }

    stCnfInfo.enResult  = USIMM_AUTH_IMS_OTHER_FAILURE;

    USIMM_ImsAuthCnf(pstMsg, &stCnfInfo);

    return VOS_ERR;
}


VOS_UINT32 USIMM_AuthenticationHandle(USIMM_MsgBlock *pMsg)
{
    USIMM_AUTH_REQ_STRU                *pstMsg;

    pstMsg = (USIMM_AUTH_REQ_STRU*)pMsg;

#if (FEATURE_VSIM == FEATURE_ON)
    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM is Active*/
    {
        if (USIMM_UMTS_APP == pstMsg->enAppType)
        {
            return USIMM_AuthenVirtulUsim(pstMsg);
        }
        else if (USIMM_GSM_APP == pstMsg->enAppType)
        {
            return USIMM_AuthenVirtulSim(pstMsg);
        }
        else
        {
            return VOS_ERR;
        }
    }
#endif

    /*硬卡操作*/
    if (USIMM_GSM_APP == pstMsg->enAppType)
    {
        return USIMM_SIMAuthHandle(pstMsg);
    }

    if (USIMM_UMTS_APP == pstMsg->enAppType)
    {
        return USIMM_USIMAuthHandle(pstMsg);
    }

    if (USIMM_ISIM_APP == pstMsg->enAppType)
    {
        return USIMM_ISIMAuthHandle(pstMsg);
    }

    USIMM_ERROR_LOG("USIMM_AuthenticationHandle: enAppType is incorrect.");

    return VOS_ERR;
}


VOS_UINT32 USIMM_FDNHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                      ulResult;
    USIMM_FDN_REQ_STRU              *pstMsg;
    VOS_UINT16                      ausUsimPath[]={MF, ADF};
    VOS_UINT16                      ausSimPath[]={MF, DFGSM};
    VOS_UINT32                      ulSimSvr;
    VOS_UINT32                      ulUsimSvr;

    pstMsg = (USIMM_FDN_REQ_STRU*)pMsg;

    ulSimSvr    = USIMM_IsServiceAvailable(SIM_SVR_FDN);
    ulUsimSvr   = USIMM_IsServiceAvailable(USIM_SVR_FDN);

    if ((PS_USIM_SERVICE_NOT_AVAILIABLE == ulSimSvr)
        && (PS_USIM_SERVICE_NOT_AVAILIABLE == ulUsimSvr))
    {
        USIMM_FDNCnf(pMsg->ulSenderPid, VOS_ERR,
                    (VOS_UINT32)gstUSIMMBaseInfo.enFDNStatus);/*回复操作结果*/

        return VOS_OK;
    }

    if (USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)
    {
        /*解析ADF的密码索引*/
        ulResult = USIMM_SelectFile(USIMM_UMTS_APP, USIMM_NEED_FCP, ARRAYSIZE(ausUsimPath), ausUsimPath);/* [false alarm]:*/
    }
    else
    {
        /*解析ADF的密码索引*/
        ulResult = USIMM_SelectFile(USIMM_GSM_APP, USIMM_NEED_FCP, ARRAYSIZE(ausSimPath), ausSimPath);/* [false alarm]:*/
    }

    ulResult = USIMM_PINVerify(USIMM_PIN2, USIMM_PINCMD_VERIFY, pstMsg->aucPIN2);

    USIMM_AdjudgeCardState();    /*判断当前的状态*/

    if(ulResult != VOS_OK)/*判断结果*/
    {
        USIMM_FDNCnf(pMsg->ulSenderPid,ulResult,
                    (VOS_UINT32)gstUSIMMBaseInfo.enFDNStatus);/*回复操作结果*/

        return VOS_ERR;
    }

    if(pstMsg->enFDNHandleType == gstUSIMMBaseInfo.enFDNStatus)/*判断当前全局变量的FDN状态*/
    {
        USIMM_FDNCnf(pMsg->ulSenderPid,VOS_OK,
                    (VOS_UINT32)gstUSIMMBaseInfo.enFDNStatus);/*回复操作结果*/

        return VOS_OK;
    }

    if (USIMM_FBDN_ACTIVE == pstMsg->enFDNHandleType)
    {
        ulResult = USIMM_FDNEnable();
    }
    else
    {
        ulResult = USIMM_FDNDisable();
    }

    /* 上报FDN状态变更 */
    if (VOS_OK == ulResult)
    {
        gstUSIMMBaseInfo.enFDNStatus = pstMsg->enFDNHandleType;
    }

    USIMM_FDNCnf(pMsg->ulSenderPid,ulResult,
                (VOS_UINT32)gstUSIMMBaseInfo.enFDNStatus);/*回复操作结果*/

    return ulResult;
}


VOS_UINT32 USIMM_DeactiveRealCard(USIMM_MsgBlock *pMsg)
{
    USIMM_NORMAL_LOG("USIMM_DeactiveRealCard: Deactive Card");

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_1, WUEPS_PID_USIM, WUEPS_PID_USIM, USIMMDL_DRV_USIMMSCI_DEACT);

    if(VOS_OK != DRV_USIMMSCI_DEACT())
    {
        USIMM_ERROR_LOG("USIMM_DeactiveRealCard: Deactive Card Error");
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

    USIMM_InitGlobalVarOnPower();

    USIMM_InitGlobalVarOnReset();

    gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType     = USIMM_CARD_NOCARD;
    gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService  = USIMM_CARD_SERVIC_ABSENT;

    USIMM_ExcLogDataSave((VOS_UINT8 *)&pMsg->ulSenderPid, sizeof(VOS_UINT32));

    USIMM_SaveExcLogFile();

    return VOS_OK;
}


VOS_UINT32 USIMM_DeactiveCardHandle(USIMM_MsgBlock *pMsg)
{
#if (FEATURE_VSIM == FEATURE_ON)
    if(VOS_TRUE == USIMM_VsimIsActive())    /*vSIM is Active*/
    {
        return USIMM_DeactiveVsim(pMsg);
    }
#endif

    /*硬卡操作*/
    return USIMM_DeactiveRealCard(pMsg);
}


VOS_UINT32 USIMM_CardInOutHandle(USIMM_MsgBlock *pMsg)
{
    USIMM_CARD_INOUT_IND_STRU           *pstMsg;
    PS_HOT_IN_OUT_USIM_STATUS_IND_STRU  *pUsimMsg;

    pstMsg = (USIMM_CARD_INOUT_IND_STRU*)pMsg;

    pUsimMsg = (PS_HOT_IN_OUT_USIM_STATUS_IND_STRU*)VOS_AllocMsg(WUEPS_PID_USIM,
                                                sizeof(PS_HOT_IN_OUT_USIM_STATUS_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pUsimMsg)
    {
        USIMM_ERROR_LOG("USIMM_CardInOutHandle: VOS_AllocMsg is Failed");

        return VOS_ERR;
    }

    pUsimMsg->ulMsgName         = PS_HOT_IN_OUT_USIM_STATUS_IND;
    pUsimMsg->ulReceiverPid     = WUEPS_PID_MMA;
    pUsimMsg->ulCardInOutStatus = pstMsg->ulCardInOutStatus;

    (VOS_VOID)VOS_SendMsg(WUEPS_PID_USIM, pUsimMsg);

    if (USIMM_CARDSTATUS_IND_PLUGOUT == pstMsg->ulCardInOutStatus)  /*调用去激活接口实现卡状态上报和保护性复位停止*/
    {
        USIMM_DeactiveRealCard(pMsg);
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_InitCardHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                      ulResult;
    USIMM_ACTIVE_CARD_REQ_STRU      *pstMsg;

#if (FEATURE_VSIM == FEATURE_ON)
    if(VOS_FALSE == USIMM_VsimIsActive())    /*vSIM Close*/
    {
        ulResult =  USIMM_InitCard(pMsg);
    }
    else
    {
        ulResult =  USIMM_InitVsimCard(pMsg);
    }
#else
    ulResult = USIMM_InitCard(pMsg);
#endif

    pstMsg = (USIMM_ACTIVE_CARD_REQ_STRU *)pMsg;

    if (WUEPS_PID_USIM != pstMsg->ulSenderPid)  /*外部Module发送的初始化*/
    {
        USIMM_SingleCmdCnf(pstMsg->ulSenderPid, PS_USIMM_ACTIVE_CNF, ulResult);
    }
#if(FEATURE_ON == FEATURE_PTM)
    if (VOS_OK == ulResult)
    {
        USIMM_RecordCardErrorLog(USIMM_MNTN_STATE_OK, USIMM_MNTN_ERROR_LEVEL_INFO);
    }
    else
    {
        USIMM_RecordCardErrorLog(USIMM_MNTN_STATE_INIT_FAIL, USIMM_MNTN_ERROR_LEVEL_MAJOR);
    }
 #endif

    return ulResult;
}
VOS_UINT32 USIMM_IsdbAccessHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    USIMM_ISDB_ACCESS_REQ_STRU         *pstMsg;
    USIMM_ISDB_ACCESS_CNF_STRU          stData;

    pstMsg = (USIMM_ISDB_ACCESS_REQ_STRU *)pMsg;

    VOS_MemSet(&stData, 0, sizeof(stData));

    if (VOS_OK != USIMM_FormatIsdbApdu(pstMsg, &gstUSIMMAPDU))  /*组合成标准格式*/
    {
        USIMM_IsdbAccessCnf(pstMsg->ulSenderPid, USIMM_SW_ERR, &stData);

        USIMM_ERROR_LOG("USIMM_IsdbAccessHandle: The USIMM_FormatCsimApdu Return Error");

        return VOS_ERR;
    }

    /* 从基本通道下发的命令要清除数据 */
    if ((USIMM_USIM_CLA == gstUSIMMAPDU.aucAPDU[CLA])
      &&(CMD_INS_SELECT == gstUSIMMAPDU.aucAPDU[INS]))
    {
        USIMM_ClearCurFileInfo();
    }

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU); /* 调用链路层函数 */

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_AccessChannelHandle: DLHandle is Failed");   /* coverity by yangzhi */
    }

    if((0x62 == gstUSIMMAPDU.ucApduSW1)&&(0xF3 == gstUSIMMAPDU.ucApduSW2))
    {
        stData.ucSw1        =   gstUSIMMAPDU.ucApduSW1;
        stData.ucSw2        =   gstUSIMMAPDU.ucApduSW2;
        stData.usDataLen    =   VOS_NULL;
        stData.pucData      =   VOS_NULL_PTR;
    }
    else
    {
        stData.ucSw1        =   gstUSIMMAPDU.ucSW1;
        stData.ucSw2        =   gstUSIMMAPDU.ucSW2;
        stData.usDataLen    =   (VOS_UINT16)gstUSIMMAPDU.ulRecDataLen;
        stData.pucData      =   gstUSIMMAPDU.aucRecvBuf;
    }

    USIMM_IsdbAccessCnf(pstMsg->ulSenderPid, ulResult, &stData);

    return ulResult;
}


VOS_UINT32 USIMM_SearchCHFree(VOS_VOID)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < USIMM_CHANNEL_NUM_MAX; ucIndex++)
    {
        if (0 == g_astUSIMMChCtrl[ucIndex].ulSessionId)
        {
             return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT8 USIMM_SearchCHCtrl(VOS_UINT32 ulSessionId)
{
    VOS_UINT8                          ucIndex;

    for (ucIndex = 0; ucIndex < USIMM_CHANNEL_NUM_MAX; ucIndex++)
    {
        if (ulSessionId == g_astUSIMMChCtrl[ucIndex].ulSessionId)
        {
            return ucIndex;
        }
    }

    return USIMM_CHANNEL_NUM_MAX;
}


VOS_UINT32 USIMM_CloseChannel(VOS_UINT8 ucChannelID)
{
    VOS_UINT32                          ulResult;

    /* 下发manage打开逻辑通道,打开通道的时候P2参数为ucChannelID,P3参数填0,此时不期望带回数据 */
    if (VOS_OK != USIMM_Manage_Channel_APDU(USIMM_CLOSE_CHANNEL, ucChannelID, 0))
    {
        USIMM_ERROR_LOG("USIMM_CloseChannel: The USIMM_Manage_Channel_APDU Return Error");

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    /*判断检查结果*/
    if (USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_CloseChannel: USIMM_CheckSW Error");

        return ulResult;
    }

    return VOS_OK;
}



VOS_UINT32 USIMM_OpenChannel(VOS_UINT8 *pucChannelID, VOS_UINT32 *pulSessionId)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucChannelID;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulSessionId;

    /* 下发manage打开逻辑通道,打开通道的时候在基本逻辑通道上，因此P2参数为0,P3参数填1，期望带回一个字节的channelID */
    if (VOS_OK != USIMM_Manage_Channel_APDU(USIMM_OPEN_CHANNEL, 0, 1))
    {
        USIMM_ERROR_LOG("USIMM_OpenChannel: The USIMM_Manage_Channel_APDU Return Error");

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    /*判断检查结果*/
    if (USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_OpenChannel: USIMM_CheckSW Error");

        return ulResult;
    }

    ucChannelID = gstUSIMMAPDU.aucRecvBuf[0];

    /* 通道ID超出合法范围 */
    if (USIMM_CHANNEL_NUM_MAX <= ucChannelID)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannel: Channel ID is not valid.");

        USIMM_CloseChannel(ucChannelID);

        return VOS_ERR;
    }

    /* 此通道ID已经被使用 */
    if (0 != g_astUSIMMChCtrl[ucChannelID].ulSessionId)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannel: This channel is used already.");

        USIMM_CloseChannel(ucChannelID);

        return VOS_ERR;
    }

    for(ucIndex = 0; ucIndex < VOS_NULL_BYTE; ucIndex++)
    {
        ulSessionId = VOS_Rand(USIMM_SESSION_ID_MAX);

        if(0 != ulSessionId)
        {
            break;
        }
    }

    if(VOS_NULL_BYTE == ucIndex)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannel: make rand err.");

        USIMM_CloseChannel(ucChannelID);

        return VOS_ERR;
    }

    *pulSessionId = USIMM_SESSION_ID(ulSessionId, ucChannelID);

    *pucChannelID = ucChannelID;

    return VOS_OK;
}


VOS_UINT32 USIMM_OpenChannelHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucChannelId = VOS_NULL_BYTE;
    VOS_UINT32                          ulSessionId = VOS_NULL_DWORD;
    USIMM_OPEN_CHANNEL_REQ_STRU        *pstMsg;
    USIMM_APDU_ST                       stApduInfo = {0};
    VOS_UINT32                          i;

    pstMsg = (USIMM_OPEN_CHANNEL_REQ_STRU *)pMsg;

    /* 判断当前是否还有剩余通道 */
    if (VOS_OK != USIMM_SearchCHFree())
    {
        USIMM_ERROR_LOG("USIMM_OpenChannelHandle: The logic channel is full.");

        USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_MISSING_RESOURCE, VOS_ERR, VOS_NULL_DWORD);

        return VOS_ERR;
    }

    /* 一个通道使用者只允许打开一个通道，如果之前没有打开，则下发MANAGE CHANNEL命令打开通道 */
    ulResult = USIMM_OpenChannel(&ucChannelId, &ulSessionId);

    if (VOS_OK != ulResult)
    {
        USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_ERROR, ulResult, VOS_NULL_DWORD);

        return VOS_ERR;
    }

    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAidLenCheckFlg)
    {
        if (pstMsg->stChannelInfo.ulAIDLen < USIMM_AID_LEN_MIN)
        {
            USIMM_ERROR_LOG("USIMM_OpenChannelHandle: the length of AID is error");

            USIMM_CloseChannel(ucChannelId);

            USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_ERROR, VOS_ERR, VOS_NULL_DWORD);

            return VOS_ERR;
        }
    }

    stApduInfo.aucAPDU[CLA] = USIMM_USIM_CLA;

    stApduInfo.aucAPDU[P1] = USIMM_SELECT_BY_DF_NAME;
    stApduInfo.aucAPDU[P2] = USIMM_SELECT_RETURN_FCP_TEMPLATE;
    stApduInfo.aucAPDU[P3] = (VOS_UINT8)pstMsg->stChannelInfo.ulAIDLen;

    VOS_MemCpy(stApduInfo.aucSendBuf, pstMsg->stChannelInfo.aucADFName, pstMsg->stChannelInfo.ulAIDLen);

    for (i = 0; i < 2; i++)
    {
        /* 下发激活AID命令数据单元,cla字段应该在打开的逻辑通道上 */
        ulResult = USIMM_SelectFileByChannelID_APDU(ucChannelId, &stApduInfo);

        if(VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_OpenChannelHandle: Select ADF Error");

            USIMM_CloseChannel(ucChannelId);

            USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_NO_SUCH_ELEMENT, ulResult, VOS_NULL_DWORD);

            return VOS_ERR;
        }

        ulResult = USIMM_CheckSW(&stApduInfo);

        /* 选择文件失败后需要将P2参数改为0x00,再次尝试选择文件 */
        if (USIMM_SW_OK != ulResult)
        {
            stApduInfo.aucAPDU[P2] = USIMM_SELECT_ACTIVATE_AID;
            USIMM_ERROR_LOG("USIMM_OpenChannelHandle: USIMM_CheckSW set P2 == 0 and try again.");

            continue;
        }

        break;
    }

    if (USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_OpenChannelHandle: USIMM_CheckSW failed.");

        USIMM_CloseChannel(ucChannelId);

        USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_NO_SUCH_ELEMENT, ulResult, VOS_NULL_DWORD);

        return VOS_ERR;
    }

    VOS_MemCpy(&g_astUSIMMChCtrl[ucChannelId], &pstMsg->stChannelInfo, sizeof(USIMM_CHANNEL_INFO_STRU));

    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAIDFCPSave)
    {
        VOS_MemCpy(g_astUSIMMChAIDFCP[ucChannelId].aucADFFcp, stApduInfo.aucRecvBuf, stApduInfo.ulRecDataLen);

        g_astUSIMMChAIDFCP[ucChannelId].ulAIDFCPLen = stApduInfo.ulRecDataLen;
    }

    g_astUSIMMChCtrl[ucChannelId].ulSessionId = ulSessionId;
    g_astUSIMMChCtrl[ucChannelId].ulChanNum   = ucChannelId;

    g_astUSIMMChGetRsp[ucChannelId].ulRspLen  = 0;

    USIMM_OpenChannelCnf(pstMsg->ulSenderPid, TAF_ERR_NO_ERROR, VOS_OK, ulSessionId);

    return VOS_OK;
}


VOS_UINT32 USIMM_ChannelCmdParaCheck(VOS_VOID)
{
    /* 中移动要求对鉴权命令也下发，非中移动版本要做检查 */
    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAuthCmdCheckFlg)
    {
        /* 不支持GSM鉴权命令:即SIM gsm鉴权和USIM gsm鉴权，协议27.007关于cgla描述
        SIM gsm鉴权时，P2参数为0，参考协议11.11 9.2.16
        USIM gsm鉴权时，P2参数低三bit为0，参看协议31.102 7.1.2
        */
        if ((CMD_INS_AUTHENTICATE == gstUSIMMAPDU.aucAPDU[INS])
            &&(0 == (gstUSIMMAPDU.aucAPDU[P2] & 0x07)))
        {
            USIMM_ERROR_LOG("USIMM_ChannelCmdParaCheck: The authenticate operation is forbided.");

            return VOS_ERR;
        }
    }

    /* 检查INS字节是否为"6X"或者"9X"，如果是则返回失败.此处检查是协议标准，7816-4明确规定ins为"6X","9X"非法 */
    if ((0x60 == (gstUSIMMAPDU.aucAPDU[INS]&0xf0))
        || (0x90 == (gstUSIMMAPDU.aucAPDU[INS]&0xf0)))
    {
        USIMM_ERROR_LOG("USIMM_ChannelCmdParaCheck: The operation is not supportable.");

        return VOS_ERR;
    }

    /* 此处检查根据前期分析，在cgla访问逻辑通道时不能下发下列命令:
       1 用manage打开或关闭通道
       2 select aid命令 P1为04指的是select by df name
       3 fetch指令，逻辑通道回复91XX后，要在基本通道上fetch，参见协议102.221 7.4.2.1
    */
    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulCglaInsCheckFlg)
    {
        if((CMD_INS_MANAGE_CHANNEL == gstUSIMMAPDU.aucAPDU[INS])
            || (CMD_INS_FETCH == gstUSIMMAPDU.aucAPDU[INS]))
        {
            USIMM_ERROR_LOG("USIMM_ChannelCmdParaCheck: The operation is not supportable.");

            return VOS_ERR;
        }
    }

    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulCglaSelectCheckFlg)
    {
        if ((CMD_INS_SELECT == gstUSIMMAPDU.aucAPDU[INS])
            && (USIMM_SELECT_BY_DF_NAME == gstUSIMMAPDU.aucAPDU[P1]))
        {
            USIMM_ERROR_LOG("USIMM_ChannelCmdParaCheck: The operation is not supportable.");

            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_CloseChannelHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucChannelId;
    USIMM_CLOSE_CHANNEL_REQ_STRU       *pstMsg;

    pstMsg = (USIMM_CLOSE_CHANNEL_REQ_STRU *)pMsg;

    ucChannelId = USIMM_SearchCHCtrl(pstMsg->ulSessionID);

    /* 通道号参数检测 */
    if ((0 == ucChannelId) || (USIMM_CHANNEL_NUM_MAX <= ucChannelId))
    {
        USIMM_ERROR_LOG("USIMM_CloseChannelHandle: ulSessionID is not correct");

        USIMM_CloseChannelCnf(pstMsg->ulSenderPid, TAF_ERR_PARA_ERROR, VOS_ERR);

        return VOS_ERR;
    }

    /* 去激活ADF 参考协议102.221 8.5.3节，终止应用时先下发status通知UICC，然后通过三种方式结束应用，
     1 在通道上重新选择一个新的应用
     2 在通道上select旧应用，p2参数填终止应用
     3 关闭通道
     此处因为是关闭逻辑通道，因此选择方式3
    */
    ulResult = USIMM_StatusByChannelID_APDU(ucChannelId, USIMM_STATUS_TERMINATION_CURAPP, USIMM_STATUS_NO_DATA_RETURNED, 0);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_CloseChannelHandle: Terminate ADF Error");

        USIMM_CloseChannelCnf(pstMsg->ulSenderPid, TAF_ERR_ERROR, ulResult);

        return VOS_ERR;
    }

    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if (USIMM_SW_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_CloseChannelHandle: USIMM_CheckSW failed.");
    }

    /* 关闭逻辑通道 */
    ulResult = USIMM_CloseChannel(ucChannelId);

    if (VOS_OK != ulResult)
    {
        USIMM_CloseChannelCnf(pstMsg->ulSenderPid, TAF_ERR_ERROR, ulResult);

        return VOS_ERR;
    }

    VOS_MemSet(&g_astUSIMMChCtrl[ucChannelId], 0, sizeof(USIMM_CHANNEL_INFO_STRU));

    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAIDFCPSave)
    {
        g_astUSIMMChAIDFCP[ucChannelId].ulAIDFCPLen = VOS_NULL;
    }

    USIMM_CloseChannelCnf(pstMsg->ulSenderPid, TAF_ERR_NO_ERROR, VOS_OK);

    return VOS_OK;

}
VOS_UINT32 USIMM_CheckAIDFcp(VOS_UINT32 ulChannelId, USIMM_APDU_ST *pstUsimmApdu, USIMM_ACCESS_CHANNEL_CNF_STRU *pstDataCnf)
{
    if (VOS_FALSE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAIDFCPSave)
    {
        return VOS_ERR;
    }

    if (VOS_NULL == g_astUSIMMChAIDFCP[ulChannelId].ulAIDFCPLen)
    {
        return VOS_ERR;
    }

    if (0xA4 == pstUsimmApdu->aucAPDU[INS])
    {
        if (VOS_OK == VOS_MemCmp(g_astUSIMMChCtrl[ulChannelId].aucADFName, 
                                pstUsimmApdu->aucSendBuf, 
                                pstUsimmApdu->aucAPDU[P3]))
        {
            pstDataCnf->pucData     = g_astUSIMMChAIDFCP[ulChannelId].aucADFFcp;
            pstDataCnf->ucSw1       = 0x90;
            pstDataCnf->ucSw2       = 0x00;
            pstDataCnf->usDataLen   = (VOS_UINT16)g_astUSIMMChAIDFCP[ulChannelId].ulAIDFCPLen;

            return VOS_OK;
        }
    }

    g_astUSIMMChAIDFCP[ulChannelId].ulAIDFCPLen = VOS_NULL;

    return VOS_ERR;
}


VOS_UINT32 USIMM_AccessChannelHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucChannelId;
    USIMM_ACCESS_CHANNEL_REQ_STRU      *pstMsg;
    USIMM_ACCESS_CHANNEL_CNF_STRU       stDataCnf;

    pstMsg = (USIMM_ACCESS_CHANNEL_REQ_STRU *)pMsg;

    VOS_MemSet(&stDataCnf, 0, sizeof(stDataCnf));

    ucChannelId = USIMM_SearchCHCtrl(pstMsg->ulSessionID);

    /* 通道号参数检测 */
    if ((0 == ucChannelId) || (USIMM_CHANNEL_NUM_MAX <= ucChannelId))
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelHandle: The channel ID is too big.");

        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_ERR, VOS_ERR, &stDataCnf);

        return VOS_ERR;
    }

    /* GetResponse选择从缓冲区中读取 */
    if ((CMD_INS_GET_RESPONSE == pstMsg->aucData[INS])
        &&(0 != g_astUSIMMChGetRsp[ucChannelId].ulRspLen))
    {
        stDataCnf.usDataLen =   (VOS_UINT16)g_astUSIMMChGetRsp[ucChannelId].ulRspLen;
        stDataCnf.pucData   =   g_astUSIMMChGetRsp[ucChannelId].aucRspContent;
        stDataCnf.ucSw1     =   0x90;
        stDataCnf.ucSw2     =   0x00;

        g_astUSIMMChGetRsp[ucChannelId].ulRspLen = 0;

        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_OK, USIMM_SW_OK, &stDataCnf);

        return USIMM_SW_OK;
    }

    g_astUSIMMChGetRsp[ucChannelId].ulRspLen = 0;

    /* 组合成标准格式 */
    /* coverity[overrun-buffer-val] */
    if (VOS_OK != USIMM_FormatStandardApdu((VOS_UINT8 *)(pstMsg->aucData), (VOS_UINT16)pstMsg->ulDataLen, &gstUSIMMAPDU))
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelHandle: The USIMM_FormatStandardApdu Return Error");

        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_ERR, VOS_ERR, &stDataCnf);

        return VOS_ERR;
    }

    if (VOS_OK != USIMM_ChannelCmdParaCheck())
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelHandle: The authenticate operation is forbided.");

        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_ERR, VOS_ERR, &stDataCnf);

        return VOS_ERR;
    }

    if (VOS_OK == USIMM_CheckAIDFcp(ucChannelId, &gstUSIMMAPDU, &stDataCnf))
    {
        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_OK, VOS_OK, &stDataCnf);

        return VOS_OK;
    }

    /* 对低两bit进行掩码 */
    gstUSIMMAPDU.aucAPDU[CLA] = (gstUSIMMAPDU.aucAPDU[CLA] & 0xFC)|ucChannelId;

    /* 调用链路层函数 */
    ulResult = USIMM_CglaDLHandle(&gstUSIMMAPDU);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_AccessChannelHandle: The operation is not supportable.");

        USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_ERR, ulResult, &stDataCnf);

        return VOS_ERR;
    }

    /* 假如发生了Warning操作则进行缓存处理 */
    if ((VOS_TRUE == gstUSIMMDLControl.bSendRep)
        && (USIMM_SW_WARNING == gstUSIMMAPDU.ucResult))
    {
        stDataCnf.usDataLen                 =   0;
        stDataCnf.ucSw1                     =   gstUSIMMAPDU.ucApduSW1;
        stDataCnf.ucSw2                     =   gstUSIMMAPDU.ucApduSW2;

        /* 保存逻辑通道上GetResponse结果 */
        g_astUSIMMChGetRsp[ucChannelId].ulRspLen    =   gstUSIMMAPDU.ulRecDataLen;

        VOS_MemCpy(g_astUSIMMChGetRsp[ucChannelId].aucRspContent,
                    gstUSIMMAPDU.aucRecvBuf,
                    gstUSIMMAPDU.ulRecDataLen);
    }
    else
    {
        stDataCnf.usDataLen                 =   (VOS_UINT16)gstUSIMMAPDU.ulRecDataLen;
        stDataCnf.pucData                   =   gstUSIMMAPDU.aucRecvBuf;
        stDataCnf.ucSw1                     =   gstUSIMMAPDU.ucSW1;
        stDataCnf.ucSw2                     =   gstUSIMMAPDU.ucSW2;
    }

    /* 此处必须check sw，因为在逻辑通道上回复91XX，必须在基本通道上fetch */
    ulResult = USIMM_CheckSW(&gstUSIMMAPDU);

    if(USIMM_SW_OK != ulResult)
    {
        USIMM_WARNING_LOG("USIMM_AccessChannelHandle: check SW Result is Failed");   /* coverity by yangzhi */
    }

    /* MANAGE命令进行打印 */
    if(CMD_INS_MANAGE_CHANNEL == gstUSIMMAPDU.aucAPDU[INS])
    {
        USIMM_INFO_LOG("USIMM_AccessChannelHandle: manage command send by cgla.");
    }

    /* check sw无论结果是否ok，都给at回复ok，为了将sw报给app */
    USIMM_AccessChannelCnf(pstMsg->ulSenderPid, VOS_OK, ulResult, &stDataCnf);

    return ulResult;

}


VOS_UINT32 USIMM_CardStatusHandle(USIMM_MsgBlock *pstMsg)
{
    USIMM_CARD_STATUS_IND_STRU         *pstCardStatus;

    pstCardStatus = (USIMM_CARD_STATUS_IND_STRU *)pstMsg;

    /* 当前是有卡情况下收到卡拔出消息 */
    if ((USIMM_CARDSTATUS_IND_PLUGOUT == pstCardStatus->ulCardStatus)
       && (USIMM_CARD_SERVIC_ABSENT != gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService))
    {
        return USIMM_DeactiveRealCard(pstMsg);
    }
    else if ((VOS_TRUE == USIMM_IsCLEnable())&&(USIMM_CARDSTATUS_IND_PLUGOUT == pstCardStatus->ulCardStatus))
    {
        return USIMM_DeactiveRealCard(pstMsg);
    }
    /* 当前是无卡情况下收到卡插入消息 */
    else if ((USIMM_CARDSTATUS_IND_PLUGIN == pstCardStatus->ulCardStatus)
       && (USIMM_CARD_SERVIC_ABSENT == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService))
    {
        return USIMM_ActiveCardReq(WUEPS_PID_USIM);
    }
    else
    {
        return VOS_OK;
    }
}

/*****************************************************************************
函 数 名  :USIMM_SendTPDUHandle
功能描述  :访问通道下发命令消息处理
输入参数  :USIMM_MsgBlock: API下发消息内容
输出参数  :无
返 回 值  :VOS_ERR
           VOS_OK
修订记录  :
1. 日    期   : 2014年1月15日
   作    者   : zhuli
   修改内容   : Creat
*****************************************************************************/
VOS_UINT32 USIMM_SendTPDUHandle(USIMM_MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    USIMM_SENDTPDU_REQ_STRU             *pstMsg;
    USIMM_SENDTPDU_CNF_STRU             stDataCnf;
    USIMM_APDU_ST                       stUSIMMAPDU;

    pstMsg = (USIMM_SENDTPDU_REQ_STRU *)pMsg;

    VOS_MemSet(&stDataCnf, 0, sizeof(stDataCnf));

    VOS_MemSet(&stUSIMMAPDU, 0, sizeof(USIMM_APDU_ST));

    VOS_MemCpy(stUSIMMAPDU.aucAPDU, pstMsg->aucTPDUHead, sizeof(pstMsg->aucTPDUHead));

    stDataCnf.pucTPDUHead = pstMsg->aucTPDUHead;

    if (VOS_NULL == pstMsg->ulTPDUDataLen)
    {
        stUSIMMAPDU.ulLcValue = 0;
        stUSIMMAPDU.ulLeValue = stUSIMMAPDU.aucAPDU[P3];
    }
    else
    {
        stUSIMMAPDU.ulLcValue = pstMsg->ulTPDUDataLen;
        stUSIMMAPDU.ulLeValue = 0;

        VOS_MemCpy(stUSIMMAPDU.aucSendBuf, pstMsg->aucTPDUData, pstMsg->ulTPDUDataLen);
    }

    ulResult = USIMM_CglaDLHandle(&stUSIMMAPDU);

    if(VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_SendTPDUHandle: USIMM_CglaDLHandle Return Failed.");

        USIMM_SendTPDUCnf(pstMsg->ulSenderPid, VOS_ERR, ulResult, &stDataCnf);

        return VOS_ERR;
    }

    stDataCnf.usDataLen                 =   (VOS_UINT16)stUSIMMAPDU.ulRecDataLen;
    stDataCnf.pucData                   =   stUSIMMAPDU.aucRecvBuf;
    stDataCnf.ucSw1                     =   stUSIMMAPDU.ucSW1;
    stDataCnf.ucSw2                     =   stUSIMMAPDU.ucSW2;

    /* 此处必须check sw，因为在逻辑通道上回复91XX，必须在基本通道上fetch */
    /* coverity[check_return] */
    ulResult = USIMM_CheckSW(&stUSIMMAPDU);

    if (VOS_OK != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_SendTPDUHandle: USIMM_CglaDLHandle Return Failed.");
    }

    /* check sw无论结果是否ok，都给at回复ok，为了将sw报给app */
    /* coverity[unchecked_return] */
    USIMM_SendTPDUCnf(pstMsg->ulSenderPid, VOS_OK, ulResult, &stDataCnf);

    return ulResult;
}



static USIMMAPP_FUNCLIST_ST gastUSIMMAppFuncList[] =
{
    {USIMM_CMDTYPE_INITSTART,           USIMM_InitCardHandle},
    {USIMM_CMDTYPE_INITTHIRD,           USIMM_InitCardOptionalFile},
    {USIMM_CMDTYPE_SETFILE_REQ,         USIMM_SetFileHandle},
    {USIMM_CMDTYPE_GETFILE_REQ,         USIMM_GetFileHandle},
    {USIMM_CMDTYPE_AUTH_REQ,            USIMM_AuthenticationHandle},
    {USIMM_CMDTYPE_FDN_REQ,             USIMM_FDNHandle},
    {USIMM_CMDTYPE_PINHANDLE_REQ,       USIMM_PINHandle},
    {USIMM_CMDTYPE_MAXRECORD_REQ,       USIMM_GetMaxRecordNum},
    {USIMM_CMDTYPE_REFRESH_REQ,         USIMM_RefreshHandle},
    {USIMM_CMDTYPE_ENVELOPE_REQ,        USIMM_SAT_Envelope},
    {USIMM_CMDTYPE_TERMINALRSP_REQ,     USIMM_SAT_TerminalResopnse},
    {USIMM_CMDTYPE_GACCESS_REQ,         USIMM_GAccessHandle},
    {USIMM_CMDTYPE_RACCESS_REQ,         USIMM_RAccessHandle},
    {USIMM_CMDTYPE_SPBFILE_REQ,         USIMM_SetSPBFileHandle},
    {USIMM_CMDTYPE_PBINIT_IND,          USIMM_PBInitStatusIndHandle},
    {USIMM_CMDTYPE_PROTECTRESET_REQ,    USIMM_ProtectReset},
    {USIMM_CMDTYPE_SEARCH_REQ,          USIMM_SearchHandle},
    {USIMM_CMDTYPE_STATUS_REQ,          USIMM_StatusHandle},
    {USIMM_CMDTYPE_DEACTIVE_REQ,        USIMM_DeactiveCardHandle},
    {USIMM_CMDTYPE_CARDINOUT_IND,       USIMM_CardInOutHandle},
    {USIMM_CMDTYPE_ISDBACC_REQ,         USIMM_IsdbAccessHandle},
    {USIMM_CMDTYPE_OPENCHANNEL_REQ,     USIMM_OpenChannelHandle},
    {USIMM_CMDTYPE_CLOSECHANNEL_REQ,    USIMM_CloseChannelHandle},
    {USIMM_CMDTYPE_ACCESSCHANNEL_REQ,   USIMM_AccessChannelHandle},
    {USIMM_CMDTYPE_INITISIM_REQ,        USIMM_InitIsimHandle},
    {USIMM_CMDTYPE_CARDSTATUS_IND_CB,   USIMM_CardStatusHandle},
    {USIMM_CMDTYPE_SENDTPDU_REQ,        USIMM_SendTPDUHandle},
};


VOS_VOID USIMM_ISIMStateChangeWithUsim(VOS_VOID)
{
    if (USIMM_CARD_STATE_CHANGED == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enStateChange)
    {
        if ((USIMM_CARD_USIM == gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType)&&(VOS_TRUE == gulUsimSupportIMS))
        {
            gastUSIMMCardAppInfo[USIMM_UICC_ISIM].enCardService = gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService;
            gastUSIMMCardAppInfo[USIMM_UICC_ISIM].enStateChange = USIMM_CARD_STATE_CHANGED;
        }
    }

    return;
}


VOS_VOID USIMM_CardStateChangeProc(VOS_VOID)
{
    VOS_UINT32                          i;

    /* 判断ISIM的状态是否要跟着USIM变化 */
    USIMM_ISIMStateChangeWithUsim();

    for(i = 0; i < USIMM_UICC_BUTT; i++)
    {
        if (USIMM_CARD_STATE_CHANGED == gastUSIMMCardAppInfo[i].enStateChange)
        {
            gastUSIMMCardAppInfo[i].enStateChange = USIMM_CARD_STATE_NOCHANGE;

            USIMM_CardStatusInd(MAPS_PIH_PID,gastUSIMMCardAppInfo[i].enCardType,gastUSIMMCardAppInfo[i].enCardService);
        }
    }

    return;
}

#if(FEATURE_ON == FEATURE_PTM)
VOS_VOID USIMM_ErrorLogProc(USIMM_MsgBlock * pMsg)
{
    OM_ERROR_LOG_CTRL_IND_STRU   *pstCrtlInd;

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_1, pMsg->ulSenderPid, WUEPS_PID_USIM, pMsg->enMsgType);

    if (ID_OM_ERR_LOG_REPORT_REQ == pMsg->enMsgType)
    {
        USIMM_CardErrorLogReport();

        OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

        return;
    }

    if (ID_OM_ERR_LOG_CTRL_IND == pMsg->enMsgType)
    {
        pstCrtlInd = (OM_ERROR_LOG_CTRL_IND_STRU *)pMsg;

        g_stErrLogCtrlInfo.ucAlmLevel = pstCrtlInd->ucAlmLevel;
        g_stErrLogCtrlInfo.ucAlmStatus = pstCrtlInd->ucAlmStatus;
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

    return;
}
#endif


VOS_VOID USIMM_ApplicationMsgProc(USIMM_MsgBlock * pMsg)
{
    VOS_INT32 lLockLevel;

    if( pMsg->enMsgType >= (sizeof(gastUSIMMAppFuncList)/sizeof(USIMMAPP_FUNCLIST_ST)))
    {
        LogPrint2("\r\nUSIMM_ApplicationMsgProc:CmdType error!, Msg ID is %d, Send PID is %d\r\n", (VOS_INT)pMsg->enMsgType, (VOS_INT)pMsg->ulSenderPid);
        return;
    }

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_1, pMsg->ulSenderPid, WUEPS_PID_USIM, pMsg->enMsgType);

    if(VOS_OK != gastUSIMMAppFuncList[pMsg->enMsgType].pProcFun(pMsg))
    {
        USIMM_WARNING_LOG("USIMM_ApplicationMsgProc: Message Handle Error");
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_1);

    USIMM_CardStateChangeProc();

    /*当前产生SAT命令，并且SAT功能打开*/
    if(USIMM_SAT_STATE_NEEDFETCH == gstUSIMMBaseInfo.enSATState)
    {
        USIMM_SAT_Fetch(gstUSIMMBaseInfo.usSATLen);
    }

    if(gulUSIMMAPIMessageNum != 0)
    {
     /* coverity[lock_acquire] */
        lLockLevel = VOS_SplIMP();

        gulUSIMMAPIMessageNum--;

        VOS_Splx(lLockLevel);
    }

    /*进入时钟停止的条件*/
    if((VOS_FALSE == USIMM_VsimIsActive())
        &&(0 == gulUSIMMAPIMessageNum)
        &&(USIMM_PB_IDLE == gstUSIMMBaseInfo.stPBInfo.enPBInitStatus)
        &&(USIMM_CLKSTOP_ALLOW == gstUSIMMDrvInfo.enCLKStop))
    {
        USIMM_NORMAL_LOG("USIMM_ApplicationMsgProc:Clock Stop Begin");

        /* coverity[sleep] */
        DRV_USIMMSCI_TM_STOP(gstUSIMMDrvInfo.enCLKLevel);
    }
}


VOS_VOID USIMM_MsgProc(USIMM_MsgBlock * pMsg)
{
    if(VOS_NULL_PTR == pMsg)
    {
        return;
    }

    if (ACPU_PID_OM == pMsg->ulSenderPid)
    {
#if(FEATURE_ON == FEATURE_PTM)
        USIMM_ErrorLogProc(pMsg);
#endif
    }
    else
    {
        USIMM_ApplicationMsgProc(pMsg);
    }

    return;
}


VOS_UINT32 USIMM_FID_Init(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32                              ulResult = VOS_OK;
    USIMM_CMD_DEBUG_MODE_ST                 stDebugInfo;
    NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU   stUSIMMOpenSpeed;

    switch (ip)
    {
        case VOS_IP_LOAD_CONFIG:    /* USIMM Application PID 注册 */
            ulResult = VOS_RegisterPIDInfo(WUEPS_PID_USIM,
                                           (Init_Fun_Type)VOS_NULL_PTR,
                                           (Msg_Fun_Type)USIMM_MsgProc);
            if (ulResult != VOS_OK)
            {
                return VOS_ERR;
            }

            /*注册USIM Manager模块的FID*/
            ulResult = VOS_RegisterMsgTaskPrio(MAPS_USIMM_FID, MAPS_USIMM_FID_PRIORITY);

            if (ulResult != VOS_OK)
            {
                return VOS_ERR;
            }

            break;

        case VOS_IP_INITIAL:    /*   本模块初始化入口*/
            if(VOS_OK != VOS_SmCreate( "USIMM APPLICATIOM API SCMD",1,VOS_SEMA4_FIFO,&gulUSIMMApiSmId))/*创建api访问信号量*/
            {
                return VOS_ERR;
            }

            /*yangzhi Usim Init begin 0520 */
            ddmPhaseScoreBoot("start usim init",__LINE__);

            /* 创建获取卡状态时所起的callback定时器所用信号量*/
            if(VOS_OK != VOS_SmBCreate("GCS", 0, VOS_SEMA4_FIFO, (VOS_UINT32*)&g_ulUSIMMGetCardStatSemID))
            {
                return VOS_ERR;
            }
			
            if(VOS_OK != VOS_SmBCreate("UTD", 0, VOS_SEMA4_FIFO, (VOS_UINT32*)&g_ulUSIMMTaskDelaySemID))
            {
                return VOS_ERR;
            }
			
            USIMM_InitGlobalVarOnPower();

            /*读取NV判断快速开机状态*/
            if(NV_OK != NV_Read(en_NV_Item_FollowOn_OpenSpeed_Flag, &stUSIMMOpenSpeed, sizeof(NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU)))
            {
                gulUSIMMOpenSpeed = USIMM_OPENSPEEDDISABLE;
            }

            gulUSIMMOpenSpeed = stUSIMMOpenSpeed.ulQuickStartSta;

            if(USIMM_OPENSPEEDENABLE == gulUSIMMOpenSpeed)
            {
                USIMM_NORMAL_LOG("USIMM_FID_Init: Usimm Open Speed is ON");

                USIMM_InitOpenSpeed();

                ulResult = VOS_OK;
            }
            else
            {
                USIMM_NORMAL_LOG("USIMM_FID_Init: Start Normal Init SIM Step");

                ulResult = NV_Read(en_NV_Item_Usim_Debug_Mode_Set, &stDebugInfo, sizeof(USIMM_CMD_DEBUG_MODE_ST));

                if((NV_OK ==  ulResult)&&(VOS_TRUE == stDebugInfo.usFlag))/*延迟时间捕获初始化信息*/
                {
                    return VOS_OK;
                }

                USIMM_InitLogFile();

                USIMM_InitExcLogFile();

#ifndef OAM_DMT
                ulResult = USIMM_ActiveCardReq(WUEPS_PID_USIM);
#endif

                USIMM_CardStatusRegCbFuncProc();
            }
            break;

        default:
            break;
    }

    return ulResult;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */





