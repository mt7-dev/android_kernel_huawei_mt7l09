
/*lint --e{958,718,746,767}*/

#include "usimmapdu.h"
#include "usimmdl.h"
#include "usimmt1dl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_USIMM_APDUMODULE_C



VOS_UINT32 USIMM_CheckSW(USIMM_APDU_ST *pstUSIMMAPDU)
{
    USIMM_SWCHECK_ST                   *pstSWCheck;
    VOS_UINT32                          ulLen;
    VOS_UINT32                          i;
    VOS_UINT8                           ucSW1;
    VOS_UINT8                           ucSW2;
    USIMM_SWCHECK_ENUM_UINT16           enResult = USIMM_SW_OTHER_ERROR;

    ucSW1 = pstUSIMMAPDU->ucSW1;
    ucSW2 = pstUSIMMAPDU->ucSW2;

    if (USIMM_SIM_CLA == pstUSIMMAPDU->aucAPDU[CLA])  /*根据卡类型判断*/
    {
        pstSWCheck = gausSimSWCheck;

        ulLen      = USIMM_SIMSW_MAXNUMBER;
    }
    else
    {
        pstSWCheck = gausUsimSWCheck;

        ulLen      = USIMM_USIMSW_MAXNUMBER;
    }

    for(i=0; i<ulLen; i++)
    {
        if(pstSWCheck[i].ucSW1 != ucSW1)        /*先判断SW1匹配*/
        {
            continue;
        }

        if((0xC0 == (ucSW2&0xF0))&&(0x63 == pstSWCheck[i].ucSW1))
        {
            enResult = pstSWCheck[i].enResult;

            break;
        }

        if((0xFF == pstSWCheck[i].ucSW2)||(ucSW2 == pstSWCheck[i].ucSW2))/*不需要判断SW2或者SW2匹配*/
        {
            enResult = pstSWCheck[i].enResult;

            break;
        }
    }

    if (USIMM_SW_OK_WITH_SAT == enResult)    /*SAT需要特殊处理*/
    {
        USIMM_INFO_LOG("USIMM_CheckSW: There is SAT Data Need Fetch");

        gstUSIMMBaseInfo.enSATState = USIMM_SAT_STATE_NEEDFETCH;

        if (0x00 == ucSW2)           /*代表当前获取主动命令长度256*/
        {
            gstUSIMMBaseInfo.usSATLen = 0x100;
        }
        else
        {
            gstUSIMMBaseInfo.usSATLen = ucSW2;
        }

        enResult = USIMM_SW_OK;
    }

    if (VOS_TRUE == g_stUsimmFeatureCfg.unCfg.stFeatureCfg.ulAtt_flg)
    {
        if ((0x6F == ucSW1)&&(0x00 == ucSW2))
        {
            g_ulATTSpecErrSWCnt++;
        }
        else
        {
            g_ulATTSpecErrSWCnt = 0;
        }

        if (g_ulATTSpecErrSWCnt >= 3)                                 /*连续超过3次上报无卡*/
        {
            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardType        = USIMM_CARD_NOCARD;        /*当前状态为无卡*/
            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enCardService     = USIMM_CARD_SERVIC_ABSENT; /*当前状态为无卡*/

            gastUSIMMCardAppInfo[USIMM_UICC_USIM].enStateChange     = USIMM_CARD_STATE_CHANGED;
        }
    }

    return (VOS_UINT32)enResult;
}



VOS_UINT32 USIMM_SelectFileByChannelID_APDU(VOS_UINT8 ucChannelID, USIMM_APDU_ST *pstApduInfo)
{
    VOS_UINT32                          ulResult;

    pstApduInfo->aucAPDU[CLA]   = pstApduInfo->aucAPDU[CLA]|ucChannelID; /*填充APDU命令头*/

    pstApduInfo->aucAPDU[INS]   = CMD_INS_SELECT;

    pstApduInfo->ulLcValue      = pstApduInfo->aucAPDU[P3];                  /*填充命令其它内容*/

    if(USIMM_SELECT_RETURN_FCP_TEMPLATE == pstApduInfo->aucAPDU[P2])
    {
        /* 此种场景下为case4 */
        pstApduInfo->ulLeValue      = USIMM_LE_MAX_LEN;
    }
    else
    {
        /* case3 */
        pstApduInfo->ulLeValue      = VOS_NULL;
    }

    ulResult = USIMM_DLHandle(pstApduInfo);/*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_SelectFile_APDU:USIMM_DLHandle error");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_SelectFile_APDU(USIMM_APDU_ST   *pstApduInfo)
{
    return USIMM_SelectFileByChannelID_APDU(0, pstApduInfo);
}


VOS_UINT32 USIMM_ReselectFileAPDU(USIMM_APDU_ST   *pstApduInfo)
{
    VOS_UINT32                          ulIndex = 0;
    VOS_UINT32                          ulResult = 0;

    for (ulIndex = 0; ulIndex < USIMM_SELECT_MAX_TIME; ulIndex++)
    {
        /* 不带FCP情况下连续出错则采用带FCP的方式尝试 */
        if ((USIMM_SELECT_NO_DATA_RETURNED == pstApduInfo->aucAPDU[P2])
            && ((USIMM_SELECT_MAX_TIME - 1) == ulIndex))
        {
            pstApduInfo->aucAPDU[P2] = USIMM_SELECT_RETURN_FCP_TEMPLATE;
        }

        ulResult = USIMM_SelectFile_APDU(pstApduInfo);

        if (VOS_OK != ulResult)
        {
            USIMM_ERROR_LOG("USIMM_ReselectFileAPDU: USIMM_SelectFile_APDU fail.");

            return USIMM_SW_SENDCMD_ERROR;
        }

        ulResult = USIMM_CheckSW(pstApduInfo);

        /* 状态字为Warning或Technical Problem则再次进行选文件操作 */
        if ((USIMM_SW_WARNING == ulResult) || (USIMM_SW_TECH_ERROR == ulResult))
        {
            continue;
        }

        break;
    }

    return ulResult;
}


VOS_UINT32 USIMM_StatusByChannelID_APDU(VOS_UINT8 ucChannelID, VOS_UINT8 ucP1,VOS_UINT8 ucP2,VOS_UINT8 ucP3)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|ucChannelID|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_STATUS;

    gstUSIMMAPDU.aucAPDU[P1]  = ucP1;

    gstUSIMMAPDU.aucAPDU[P2]  = ucP2;

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    gstUSIMMAPDU.ulLcValue = 0x00;                              /*填充命令其它内容*/

    if((ucP3 == 0x00)&&(ucP2 != 0x0C))
    {
        gstUSIMMAPDU.ulLeValue = 0x100;
    }
    else
    {
        gstUSIMMAPDU.ulLeValue = ucP3;
    }


    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_STATUS_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_Status_APDU(VOS_UINT8 ucP1,VOS_UINT8 ucP2,VOS_UINT8 ucP3)
{
    return USIMM_StatusByChannelID_APDU(0, ucP1, ucP2, ucP3);
}


VOS_UINT32 USIMM_ReadBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_READ_BINARY;

    gstUSIMMAPDU.aucAPDU[P1]  = (VOS_UINT8)((usOffset>>8)&0xFF);

    gstUSIMMAPDU.aucAPDU[P2]  = (VOS_UINT8)(usOffset&0xFF);

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    gstUSIMMAPDU.ulLcValue    = 0x00;           /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue    = ucP3;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Status_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_UpdateBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    /*填充APDU命令头*/
    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA;

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_UPDATE_BINARY;

    gstUSIMMAPDU.aucAPDU[P1]  = (VOS_UINT8)((usOffset>>0x08)&0xFF);

    gstUSIMMAPDU.aucAPDU[P2]  = (VOS_UINT8)(usOffset&0xFF);

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, pucData, ucP3);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue   = ucP3;                  /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue   = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);/*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_UpdateBinary_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_ReadRecord_APDU(VOS_UINT8 ucRecordNum, VOS_UINT8 ucMode, VOS_UINT8 ucLen)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_READ_RECORD;

    gstUSIMMAPDU.aucAPDU[P1]  = ucRecordNum;

    gstUSIMMAPDU.aucAPDU[P2]  = ucMode;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    gstUSIMMAPDU.ulLcValue    = 0x00;           /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue    = ucLen;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ReadRecord_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_UpdateRecord_APDU(VOS_UINT8 ucRecordNum, VOS_UINT8 ucMode, VOS_UINT8 ucLen, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_UPDATE_RECORD;

    gstUSIMMAPDU.aucAPDU[P1]  = ucRecordNum;

    gstUSIMMAPDU.aucAPDU[P2]  = ucMode;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, pucData, ucLen);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_UpdateRecord_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_Search_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, VOS_UINT8 ucP3, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_SEARCH_RECORD;

    gstUSIMMAPDU.aucAPDU[P1]  = ucP1;

    gstUSIMMAPDU.aucAPDU[P2]  = ucP2;

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, pucData, ucP3);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucP3;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    /* 此种场景下为case4 */
    gstUSIMMAPDU.ulLeValue = USIMM_LE_MAX_LEN;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Search_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_Increase_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_INCREASE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, pucData, ucLen);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    /* 此种场景下为case4 */
    gstUSIMMAPDU.ulLeValue = USIMM_LE_MAX_LEN;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Increase_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_PINVerify_APDU(VOS_UINT8 ucCmdType,VOS_UINT8 ucPINType, VOS_UINT8* pucPINData)
{
    VOS_UINT32 ulResult;
    VOS_UINT8  ucLen;

    if(VOS_NULL_PTR == pucPINData)  /*获取当前的PIN剩余次数*/
    {
        ucLen = 0x00;
    }
    else                                                /*校验PIN*/
    {
        ucLen = 0x08;
    }

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = ucCmdType;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = ucPINType;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    if(8 == ucLen)
    {
        VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, pucPINData, 0x08);/*填充APDU命令内容*/
    }

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_PINVerify_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_ChangePIN_APDU(VOS_UINT8 ucPINType, VOS_UINT8* pucOldPIN, VOS_UINT8* pucNewPIN)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_CHANGE_PIN;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = ucPINType;

    gstUSIMMAPDU.aucAPDU[P3]  = 0x10;

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucOldPIN, 0x08);/*填充APDU命令内容*/

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[8], pucNewPIN, 0x08);

    gstUSIMMAPDU.ulLcValue = 0x10;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ChangePIN_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_UnblockPIN_APDU(VOS_UINT8 ucPINType, VOS_UINT8* pucPUKData, VOS_UINT8* pucPINData)
{
    VOS_UINT32 ulResult;
    VOS_UINT8  ucP3;

    if(VOS_NULL_PTR == pucPUKData)
    {
        ucP3 = 0x00;
    }
    else
    {
        ucP3 = 0x10;
    }

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_UNBLOCK_PIN;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = ucPINType;

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    if(0x00  != ucP3)
    {
        VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucPUKData, 0x08);/*填充APDU命令内容*/

        VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[8], pucPINData, 0x08);
    }

    gstUSIMMAPDU.ulLcValue = ucP3;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_UnblockPIN_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_DeactivateFile_APDU(VOS_VOID)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_DEACTIVATE_FILE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = 0x00;

    gstUSIMMAPDU.ulLcValue    = 0x00;                /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue    = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_DeactivateFile_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_ActivateFile_APDU(VOS_VOID)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_ACTIVATE_FILE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = 0x00;

    gstUSIMMAPDU.ulLcValue    = 0x00;           /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue    = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_ActivateFile_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_Authentication_APDU(VOS_UINT8 ucMode, VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_UINT32                          ulResult;

    /* 参数检测 */
    if (USIMM_APDU_MAXLEN <= ulDataLen)
    {
        USIMM_ERROR_LOG("USIMM_Authentication_APDU: ulDataLen is too big.");

        return VOS_ERR;
    }

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_AUTHENTICATE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = ucMode;

    gstUSIMMAPDU.aucAPDU[P3]  = (VOS_UINT8)ulDataLen;

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucData, ulDataLen);/*填充随机数内容*/

    gstUSIMMAPDU.ulLcValue = gstUSIMMAPDU.aucAPDU[P3];      /*填充命令其它内容*/

    /* 此种场景下为case4 */
    gstUSIMMAPDU.ulLeValue = USIMM_LE_MAX_LEN;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Authentication_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_TermimalProfile_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_TERMINAL_PROFILE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucData, ucLen);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_TermimalProfile_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_Envelope_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_ENVELOPE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucData, ucLen);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    /* 此种场景下为case4 */
    gstUSIMMAPDU.ulLeValue = USIMM_LE_MAX_LEN;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Envelope_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_Fetch_APDU(VOS_UINT8 ucLen)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_FETCH;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    gstUSIMMAPDU.ulLcValue = 0x00;                              /*填充命令其它内容*/

    if(ucLen == 0x00)
    {
        gstUSIMMAPDU.ulLeValue = 0x100;
    }
    else
    {
        gstUSIMMAPDU.ulLeValue = ucLen;
    }

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Fetch_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 USIMM_TerminalResponse_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData)
{
    VOS_UINT32 ulResult;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_TERMINAL_RESPONSE;

    gstUSIMMAPDU.aucAPDU[P1]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P2]  = 0x00;

    gstUSIMMAPDU.aucAPDU[P3]  = ucLen;

    VOS_MemCpy(&gstUSIMMAPDU.aucSendBuf[0], pucData, ucLen);/*填充APDU命令内容*/

    gstUSIMMAPDU.ulLcValue = ucLen;                              /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_TerminalResponse_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_FormatCsimApdu(USIMM_GACCESS_REQ_STRU *pstMsg,
                                            USIMM_APDU_ST          *pstUsimmApdu)
{
    /* 如果数据长度小于四字节，则是出错的场景 */
    if(pstMsg->usDataLen < T1_CASE1_APDU_LEN)
    {
        /* 打印错误 */
        USIMM_ERROR_LOG("USIMM_FormatCsimApdu: data len is less than 4 bytes");

        return VOS_ERR;
    }

    /* CASE1和CASE2的情况 */
    if ( (T1_CASE1_APDU_LEN == pstMsg->usDataLen)
        || (pstMsg->usDataLen == sizeof(pstUsimmApdu->aucAPDU)))/*无Lc字段*/
    {
        USIMM_INFO_LOG("USIMM_FormatCsimApdu: The Command have any Lc Data");

        /* copy apdu */
        VOS_MemCpy(pstUsimmApdu->aucAPDU, pstMsg->aucContent, pstMsg->usDataLen);

        pstUsimmApdu->ulLcValue = 0;

        pstUsimmApdu->ulLeValue = pstUsimmApdu->aucAPDU[P3];  /*Le的数据可能为0*/

        return VOS_OK;
    }

    /* 数据填入对应的结构体 */
    VOS_MemCpy(pstUsimmApdu->aucAPDU, pstMsg->aucContent, sizeof(pstUsimmApdu->aucAPDU));

    /* 判断P3字节指示是否正确 */
    if(pstMsg->usDataLen < (pstUsimmApdu->aucAPDU[P3] + USIMM_APDU_HEADLEN))
    {
        USIMM_ERROR_LOG("USIMM_FormatCsimApdu: LC is wrong");/*打印错误*/

        return VOS_ERR;
    }

    VOS_MemCpy(pstUsimmApdu->aucSendBuf, (pstMsg->aucContent + USIMM_APDU_HEADLEN), pstUsimmApdu->aucAPDU[P3]);

    pstUsimmApdu->ulLcValue = pstUsimmApdu->aucAPDU[P3];

    /* 数据过长 */
    if(pstMsg->usDataLen > (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
    {
        USIMM_ERROR_LOG("USIMM_FormatCsimApdu: the data len is more");/*打印错误*/

        return VOS_ERR;
    }

    /* case4场景，有一字节的LE字段 */
    if(pstMsg->usDataLen == (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
    {
        pstUsimmApdu->ulLeValue = pstMsg->aucContent[pstMsg->usDataLen - 1];
    }
    else
    {
        /* case3场景 */
        pstUsimmApdu->ulLeValue = 0;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_FormatIsdbApdu(
    USIMM_ISDB_ACCESS_REQ_STRU         *pstMsg,
    USIMM_APDU_ST                      *pstUsimmApdu)
{
    /* 如果数据长度小于四字节，则是出错的场景 */
    if(pstMsg->usDataLen < T1_CASE1_APDU_LEN)
    {
        /* 打印错误 */
        USIMM_ERROR_LOG("USIMM_FormatIsdbApdu: data len is less than 4 bytes");

        return VOS_ERR;
    }

    /* CASE1和CASE2的情况 */
    if ( (T1_CASE1_APDU_LEN == pstMsg->usDataLen)
        || (pstMsg->usDataLen == sizeof(pstUsimmApdu->aucAPDU)))/*无Lc字段*/
    {
        USIMM_INFO_LOG("USIMM_FormatIsdbApdu: The Command have any Lc Data");

        /* copy apdu */
        VOS_MemCpy(pstUsimmApdu->aucAPDU, pstMsg->aucContent, pstMsg->usDataLen);

        pstUsimmApdu->ulLcValue = 0;

        pstUsimmApdu->ulLeValue = pstUsimmApdu->aucAPDU[P3];  /*Le的数据可能为0*/

        return VOS_OK;
    }

    /* 数据填入对应的结构体 */
    /*lint -e420*/
    VOS_MemCpy(pstUsimmApdu->aucAPDU, pstMsg->aucContent, sizeof(pstUsimmApdu->aucAPDU));
    /*lint +e420*/

    /* 判断P3字节指示是否正确 */
    if(pstMsg->usDataLen < (pstUsimmApdu->aucAPDU[P3] + USIMM_APDU_HEADLEN))
    {
        USIMM_ERROR_LOG("USIMM_FormatIsdbApdu: LC is wrong");/*打印错误*/

        return VOS_ERR;
    }

    /*lint -e416*/
    VOS_MemCpy(pstUsimmApdu->aucSendBuf, (pstMsg->aucContent + USIMM_APDU_HEADLEN), pstUsimmApdu->aucAPDU[P3]);
    /*lint +e416*/

    pstUsimmApdu->ulLcValue = pstUsimmApdu->aucAPDU[P3];

    /* 数据过长 */
    if(pstMsg->usDataLen > (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
    {
        USIMM_ERROR_LOG("USIMM_FormatIsdbApdu: the data len is more");/*打印错误*/

        return VOS_ERR;
    }

    /* case4场景，有一字节的LE字段 */
    if(pstMsg->usDataLen == (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
    {
        pstUsimmApdu->ulLeValue = pstMsg->aucContent[pstMsg->usDataLen - 1];/* [false alarm]:fortify */
    }
    else
    {
        /* case3场景 */
        pstUsimmApdu->ulLeValue = 0;
    }

    return VOS_OK;
}


VOS_UINT32 USIMM_FormatStandardApdu(VOS_UINT8 *pucData, VOS_UINT16 usLen, USIMM_APDU_ST *pstUsimmApdu)
{
    /* 无Lc字段 */
    if (usLen <= sizeof(pstUsimmApdu->aucAPDU))
    {
        USIMM_INFO_LOG("USIMM_FormatStandardApdu: The Command have any Lc Data");

        /* 当APDU为case1时，Le字段为0 */
        pstUsimmApdu->aucAPDU[P3] = 0;

        VOS_MemCpy(pstUsimmApdu->aucAPDU, pucData, usLen);

        pstUsimmApdu->ulLcValue = 0;

        /* Le的数据可能为0 */
        pstUsimmApdu->ulLeValue = pstUsimmApdu->aucAPDU[P3];
    }
    /* 有Lc字段 */
    else if ((usLen > sizeof(pstUsimmApdu->aucAPDU))
             &&(usLen <= (sizeof(pstUsimmApdu->aucAPDU)+USIMM_APDU_MAXLEN)))
    {
        USIMM_INFO_LOG("USIMM_FormatStandardApdu: The Command have Lc Data");

        /* 判断Lc字段是否正确 */
        if ((sizeof(pstUsimmApdu->aucAPDU) + pucData[P3]) > usLen)/* [false alarm]:*/
        {
            USIMM_ERROR_LOG("USIMM_FormatStandardApdu: Lc is Error");

            return VOS_ERR;
        }

        VOS_MemCpy(pstUsimmApdu->aucAPDU, pucData, sizeof(pstUsimmApdu->aucAPDU));

        /*lint -e416*/
        VOS_MemCpy(pstUsimmApdu->aucSendBuf,
                   pucData+sizeof(pstUsimmApdu->aucAPDU),
                   pstUsimmApdu->aucAPDU[P3]);
        /*lint +e416*/
        pstUsimmApdu->ulLcValue = pstUsimmApdu->aucAPDU[P3];

        /* 数据过长 */
        if(usLen > (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
        {
            USIMM_ERROR_LOG("USIMM_FormatCsimApdu: the data len is more");/*打印错误*/

            return VOS_ERR;
        }

        if(usLen == (USIMM_APDU_HEADLEN + pstUsimmApdu->aucAPDU[P3] + 1))
        {
            /* case4 */
            pstUsimmApdu->ulLeValue = pucData[usLen - 1];/* [false alarm]:fortify */
        }
        else
        {
            /* case3 */
            pstUsimmApdu->ulLeValue = 0;
        }
    }
    else
    {
        USIMM_ERROR_LOG("USIMM_FormatStandardApdu: The Command Type is Error");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_Manage_Channel_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, VOS_UINT8 ucP3)
{
    VOS_UINT32 ulResult;

    /* 在对应的逻辑通道上发 在打开逻辑通道时:p2为0，关闭逻辑通道时:p2为对应逻辑通道号 */
    gstUSIMMAPDU.aucAPDU[CLA] = USIMM_USIM_CLA | ucP2;

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_MANAGE_CHANNEL;

    gstUSIMMAPDU.aucAPDU[P1]  = ucP1;

    gstUSIMMAPDU.aucAPDU[P2]  = ucP2;

    gstUSIMMAPDU.aucAPDU[P3]  = ucP3;

    gstUSIMMAPDU.ulLcValue = 0x00;

    gstUSIMMAPDU.ulLeValue = ucP3;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_Manage_Channel_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_Terminal_Capability_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, USIMM_TERMINAL_CAPABILITY_STRU *pstTerminalCapability)
{
    VOS_UINT32 ulResult;
    VOS_UINT32 ulLen = 0;

    gstUSIMMAPDU.aucAPDU[CLA] = gstUSIMMBaseInfo.ucCLA|0x80; /*填充APDU命令头*/

    gstUSIMMAPDU.aucAPDU[INS] = CMD_INS_TERMINAL_CAPABILITY;

    gstUSIMMAPDU.aucAPDU[P1]  = ucP1;

    gstUSIMMAPDU.aucAPDU[P2]  = ucP2;

    VOS_MemCpy(gstUSIMMAPDU.aucSendBuf, (VOS_UINT8 *)pstTerminalCapability->aucData,
                           pstTerminalCapability->ulLen);/*填充APDU命令内容*/

    ulLen = pstTerminalCapability->ulLen;

    gstUSIMMAPDU.aucAPDU[P3]  = (VOS_UINT8)ulLen;

    gstUSIMMAPDU.ulLcValue = ulLen;                  /*填充命令其它内容*/

    gstUSIMMAPDU.ulLeValue = 0x00;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_STATUS_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 USIMM_T1SendIFSD_APDU(VOS_UINT8 ucData)
{
    VOS_UINT32      ulResult;

    VOS_MemSet(gstUSIMMAPDU.aucAPDU, 0, USIMM_APDU_HEADLEN);

    VOS_MemSet(gstUSIMMAPDU.aucSendBuf, 0, USIMM_APDU_MAXLEN);

    /* 需要发送一个字节的IFS大小 */
    gstUSIMMAPDU.ulLcValue = 0x01;

    gstUSIMMAPDU.ulLeValue = 0x00;

    gstUSIMMAPDU.aucSendBuf[0] = ucData;

    /* 标记当前要发送IFS REQ */
    g_bSendIFSReqFlag = VOS_TRUE;

    ulResult = USIMM_DLHandle(&gstUSIMMAPDU);   /*调用链路层处理函数*/

    g_bSendIFSReqFlag = VOS_FALSE;

    if(USIMM_DL_SUCESS != ulResult)
    {
        USIMM_ERROR_LOG("USIMM_T1SendIFSD_APDU: USIMM_DLHandle is Failed");/*打印错误*/

        return VOS_ERR;
    }

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

