
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ScInterface.h"
#include "ScComm.h"
#include "ScFactory.h"
#include "ScPersonalization.h"
#include "UsimPsInterface.h"
#include "ScCtx.h"
#include "NVIM_Interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_SC_PERSONALIZATION_C

/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/*****************************************************************************
  3 函数申明
*****************************************************************************/


SC_PERSONALIZATION_CATEGORY_ENUM_UINT8 SC_PERS_GetCategoryByStatus(
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enSIMLockStatus
)
{
    switch (enSIMLockStatus)
    {
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK:
            return SC_PERSONALIZATION_CATEGORY_NETWORK;

        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK:
            return SC_PERSONALIZATION_CATEGORY_NETWORK_SUBSET;

        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN:
        case SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK:
            return SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER;

        default:
            SC_ERROR_LOG("SC_PERS_GetCategoryByStatus: enSIMLockStatus is out of range.");
            break;
    }

    return SC_PERSONALIZATION_CATEGORY_BUTT;
}


SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 SC_PERS_GetStatusByCategory(
    SC_SIMLOCK_CATEGORY_STRU *pstSimlockCatogory
)
{
    /* 如果所在PIN状态，根据category返回锁网锁卡状态 */
    if (SC_PERSONALIZATION_STATUS_PIN == pstSimlockCatogory->enStatus)
    {
        switch (pstSimlockCatogory->enCategory)
        {
            case SC_PERSONALIZATION_CATEGORY_NETWORK:
                return SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN;

            case SC_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
                return SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN;

            case SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
                return SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN;

            default:
                SC_ERROR_LOG("SC_PERS_GetStatusByCategory: pstSimlockCatogory is out of range.");
                return SC_VERIFY_SIMLOCK_RESULT_BUTT;
        }
    }

    if (SC_PERSONALIZATION_STATUS_PUK == pstSimlockCatogory->enStatus)
    {
        switch (pstSimlockCatogory->enCategory)
        {
            case SC_PERSONALIZATION_CATEGORY_NETWORK:
                return SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK;

            case SC_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
                return SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK;

            case SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
                return SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK;

            default:
                SC_ERROR_LOG("SC_PERS_GetStatusByCategory: pstSimlockCatogory is out of range.");
                return SC_VERIFY_SIMLOCK_RESULT_BUTT;
        }
    }

    return SC_VERIFY_SIMLOCK_RESULT_BUTT;
}
VOS_VOID SC_PERS_Bcd2Num(
    VOS_UINT8                          *pucDestNum,
    const VOS_UINT8                    *pucBcdNum,
    VOS_UINT32                          ulBcdNumLen
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucFirstNumber;
    VOS_UINT8                           ucSecondNumber;

    for (i = 0; i < ulBcdNumLen; i++)
    {
        /* 分别取出高低字节 */
        ucFirstNumber = (VOS_UINT8)((*(pucBcdNum + i)) & 0x0f);
        ucSecondNumber = (VOS_UINT8)(((*(pucBcdNum + i)) >> 4) & 0x0f);

        if (ucFirstNumber <= 9)
        {
            *pucDestNum++ = ucFirstNumber;
        }

        if (ucSecondNumber <= 9)
        {
            *pucDestNum++ = ucSecondNumber;
        }
    }

    return;
}
VOS_UINT32 SC_PERS_ImsiBcd2Num(
    VOS_UINT8                           ucLen,
    const VOS_UINT8                    *pucImsi,
    VOS_UINT8                          *pucNum
)
{
    VOS_UINT8                           ucFirstByte;
    VOS_UINT8                           ucTmp;

    ucFirstByte = pucImsi[0];

    /* 如果IMSI第一个字节大于0x08，说明IMSI数据不正确 */
    if (SC_IMSI_FIRST_BYTE < ucFirstByte)
    {
        SC_ERROR_LOG("SC_PERS_ImsiBcd2Num: IMSI is wrong.");

        return VOS_ERR;
    }

    ucTmp = (pucImsi[1]>>4) & 0x0F;

    /* 先转换IMSI第二个字节 */
    SC_PERS_Bcd2Num(pucNum, &ucTmp, 1);

    /* 转换IMSI后面的字节 */
    SC_PERS_Bcd2Num(pucNum + 1, pucImsi + 2, ucLen - 2);

    return VOS_OK;
}
VOS_UINT32 SC_PERS_SPBcd2Num(
    VOS_UINT8                          *pucNum,
    const VOS_UINT8                    *pucSP,
    VOS_UINT8                           ucLen
)
{
    /* 参数检查 */
    if ((0xFF == *pucSP) || (ucLen > SC_SP_LEN))
    {
        SC_ERROR_LOG("SC_PERS_SPBcd2Num: Para is wrong.");

        return VOS_ERR;
    }

    *pucNum = ((*pucSP)>>4) & 0x0F;

    *(pucNum + 1) = *pucSP & 0x0F;

    return VOS_OK;
}
VOS_UINT32 SC_PERS_CheckSimlockCodeLast2Char(
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8  enCategory,
    VOS_UINT8                              *pucLast2Num
)
{
    VOS_UINT8                           i           = 0;

    /* 检测最后两位的有效性, NET无需检测 */
    if (SC_PERSONALIZATION_CATEGORY_NETWORK < enCategory)
    {
        for (i = 0; i < 2; i++)
        {
            /* NETSUB支持范围: 0x00~0x99 */
            if (9 >= pucLast2Num[i])
            {
                continue;
            }
            /* SP支持范围: 0x00~0xFF */
            else if ( (SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER == enCategory)
                   && (0x0F >= pucLast2Num[i]) )
            {
                continue;
            }
            else
            {
                return VOS_ERR;
            }
        }
    }

    return VOS_OK;
}
VOS_UINT32 SC_PERS_CheckSimlockCodeNum(
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8  enCategory,
    VOS_UINT8                              *pucNum,
    VOS_UINT8                              *pucDepartLen
)
{
    VOS_UINT8                           i           = 0;
    VOS_UINT8                           j           = 0;
    VOS_UINT8                           ucDepartLen;        /* 数字串正确的长度 */
    VOS_UINT8                          *pucTmpNum;

    pucTmpNum   = pucNum;

    switch(enCategory)
    {
        case SC_PERSONALIZATION_CATEGORY_NETWORK:
            ucDepartLen = SC_NET_CODE_DEPART_LEN;
            break;
        default:
            ucDepartLen = SC_CODE_DEPART_LEN;
            break;
    }

    /* 检测PLMN合法性及其位数 */
    for (i = 0; i < SC_NET_CODE_DEPART_LEN; i++)
    {
        if (9 >= pucTmpNum[i])
        {
            continue;
        }
        else if ( ((SC_NET_CODE_DEPART_LEN - 1) == i)
               && (0x0F == pucTmpNum[i]) )
        {
            for (j = i + 1; j < ucDepartLen; j++)
            {
                pucTmpNum[j - 1] = pucTmpNum[j];
            }
            ucDepartLen = ucDepartLen - 1;
            break;
        }
        else
        {
            return VOS_ERR;
        }
    }

    *pucDepartLen           = ucDepartLen;

    /* 检测锁网锁卡号码最后两位的合法性 */
    if (VOS_OK != SC_PERS_CheckSimlockCodeLast2Char(enCategory, &pucTmpNum[ucDepartLen - 2]))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 SC_PERS_SimlockCodeBcd2Num(
    VOS_UINT8                              *pucNum,
    VOS_UINT8                              *pucDepartLen,
    const VOS_UINT8                        *pucBCD,
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8  enCategory
)
{
    VOS_UINT8                           ucLen;
    VOS_UINT8                           ucFirstNum;
    VOS_UINT8                           ucSecondNum;
    VOS_UINT8                          *pucTmpNum;
    VOS_UINT8                           ucNum;
    VOS_UINT8                           i;

    VOS_MemSet(pucNum, 0x00, SC_CODE_DEPART_LEN);
    pucTmpNum   = pucNum;

    switch(enCategory)
    {
        case SC_PERSONALIZATION_CATEGORY_NETWORK:
            ucLen       = SC_NET_CODE_LEN;
            break;
        case SC_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
            ucLen       = SC_NETSUB_CODE_LEN;
            break;
        case SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
            ucLen       = SC_SP_CODE_LEN;
            break;
        default:
            return VOS_ERR;
    }

    for (i = 0; i < ucLen; i++)
    {
        /* 分别取出高低字节 */
        ucFirstNum  = (VOS_UINT8)((*(pucBCD + i)) & 0x0F);
        ucSecondNum = (VOS_UINT8)(((*(pucBCD + i)) >> 4) & 0x0F);

        *pucTmpNum++    = ucFirstNum;
        *pucTmpNum++    = ucSecondNum;
    }

    /* 将第四位号码后移到第六位(与产线对接) */
    pucTmpNum           = &pucNum[SC_CODE_FOURTH_NUM_INDEX];
    ucNum               = pucTmpNum[0];
    pucTmpNum[0]        = pucTmpNum[1];
    pucTmpNum[1]        = pucTmpNum[2];
    pucTmpNum[2]        = ucNum;

    /* 对高低字节转换后的锁网锁卡号码进行合法性检查 */
    if (VOS_OK != SC_PERS_CheckSimlockCodeNum(enCategory, pucNum, pucDepartLen))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 SC_PERS_CheckCode(
    SC_SIMLOCK_CATEGORY_STRU           *pstSimLockCat,
    VOS_UINT8                          *pucIMSI,
    MODEM_ID_ENUM_UINT16                enModemID
)
{
    VOS_UINT8                           aucExCode[SC_SP_DEPART_LEN];
    VOS_UINT8                           aucPLMNEx[SC_IMSI_DEPART_LEN];
    VOS_UINT8                           ucBeginLen;
    VOS_UINT8                           ucEndLen;
    VOS_UINT8                           aucCodeBegin[SC_CODE_DEPART_LEN];
    VOS_UINT8                           aucCodeEnd[SC_CODE_DEPART_LEN];
    VOS_INT32                           lResultBegin;
    VOS_INT32                           lResultEnd;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           i;
    VOS_UINT32                          ulEfLen;
    VOS_UINT8                          *pucEf;
    VOS_UINT32                          aucExCmpLen[SC_MAX_SUPPORT_CATEGORY] = {0, SC_NET_SUB_DEPART_LEN, SC_SP_DEPART_LEN};
    VOS_UINT32                          ulSimSvr;
    VOS_UINT32                          ulUsimSvr;

    /* 没有激活，返回号段匹配*/
    if (SC_PERSONALIZATION_INDICATOR_INACTIVE == pstSimLockCat->enIndicator)
    {
        SC_INFO1_LOG("SC_PERS_CheckCode: The indicator is not active.",
                     pstSimLockCat->enCategory);

        return VOS_TRUE;
    }

    /* 已经解锁过，返回号段匹配 */
    if (SC_PERSONALIZATION_STATUS_READY == pstSimLockCat->enStatus)
    {
        SC_INFO1_LOG("SC_PERS_CheckCode: The status is not active.",
                     pstSimLockCat->enStatus);

        return VOS_TRUE;
    }

    /* 判断PI文件类别的合法性，是否支持 */
    if (pstSimLockCat->enCategory >= SC_MAX_SUPPORT_CATEGORY)
    {
        SC_ERROR1_LOG("SC_PERS_CheckCode: The Category is out of range.",
                      pstSimLockCat->enCategory);

        return VOS_FALSE;
    }

    VOS_MemCpy(aucPLMNEx, pucIMSI, SC_IMSI_DEPART_LEN);

    VOS_MemSet(aucExCode, 0, SC_SP_DEPART_LEN);

    if (SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER == pstSimLockCat->enCategory)
    {
        /* 如果服务不可用直接返回失败 */
        ulSimSvr = SC_COMM_IsUsimServiceAvailable(SIM_SVR_GIDL1,  enModemID);
        ulUsimSvr = SC_COMM_IsUsimServiceAvailable(USIM_SVR_GID1, enModemID);

        if ((PS_USIM_SERVICE_NOT_AVAILIABLE == ulSimSvr)
            && (PS_USIM_SERVICE_NOT_AVAILIABLE == ulUsimSvr))
        {
            SC_ERROR1_LOG("SC_PERS_CheckCode: USIMM_IsServiceAvailable is failed.",
                          PS_USIM_SERVICE_NOT_AVAILIABLE);

            return VOS_FALSE;
        }

        /* 读取GID1文件，失败返回号段不匹配 */
        ulResult = SC_COMM_GetUsimmCachedFile(EF_GID1_FILE_ID, &ulEfLen, &pucEf, USIMM_UNLIMIT_APP, enModemID);
        if (VOS_OK != ulResult)
        {
            SC_ERROR1_LOG("SC_PERS_CheckCode: USIMM_GetCachedFile is failed.",
                          ulResult);

            return VOS_FALSE;
        }

        if (VOS_OK != SC_PERS_SPBcd2Num(aucExCode, pucEf, SC_SP_LEN))
        {
            SC_ERROR1_LOG("SC_PERS_CheckCode: USIMM_GetCachedFile is failed.",
                          ulResult);

            return VOS_FALSE;
        }
    }

    /* 在配置的号段数组中循环比较 */
    for (i = 0; i < pstSimLockCat->ucGroupNum; i++)
    {
        /* 转换号段Begin, 转换失败则认为不在此号段范围内 */
        if (VOS_OK != SC_PERS_SimlockCodeBcd2Num(aucCodeBegin,
                                        &ucBeginLen,
                                        pstSimLockCat->astLockCode[i].aucPhLockCodeBegin,
                                        pstSimLockCat->enCategory))
        {
            continue;
        }

        /* 转换号段End, 转换失败则认为不在此号段范围内 */
        if (VOS_OK != SC_PERS_SimlockCodeBcd2Num(aucCodeEnd,
                                        &ucEndLen,
                                        pstSimLockCat->astLockCode[i].aucPhLockCodeEnd,
                                        pstSimLockCat->enCategory))
        {
            continue;
        }

        /* 号段的Begin和End长度不一致, 认为不在此号段范围内 */
        if (ucBeginLen != ucEndLen)
        {
            continue;
        }

        /* 补齐PLMN后面的扩展SP字节 */
        if (SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER == pstSimLockCat->enCategory)
        {
            VOS_MemCpy(aucPLMNEx, pucIMSI, SC_IMSI_DEPART_LEN);

            VOS_MemCpy(&aucPLMNEx[ucBeginLen - aucExCmpLen[pstSimLockCat->enCategory]],
                        aucExCode,
                        SC_SP_DEPART_LEN);
        }

        /* 确保比较号段长度不会大于8 */
        if ((ucBeginLen > SC_CODE_DEPART_LEN)||(ucEndLen > SC_CODE_DEPART_LEN))
        {
            return VOS_FALSE;
        }

        lResultBegin    = VOS_MemCmp(aucPLMNEx, aucCodeBegin, ucBeginLen);
        lResultEnd      = VOS_MemCmp(aucPLMNEx, aucCodeEnd, ucEndLen);

        /* 号段在范围内 */
        if ((lResultBegin >= 0) && (lResultEnd <= 0))
        {
            return VOS_TRUE;
        }
    }

    SC_ERROR_LOG("SC_PERS_CheckCode: The result is failed.");

    /* 如果匹配失败 */
    return VOS_FALSE;
}
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_VerifySimLock(
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 *penVerifySimlockRslt,
    MODEM_ID_ENUM_UINT16                enModemID
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulEfLen;
    VOS_UINT8                          *pucEf;
    VOS_UINT8                           aucIMSI[SC_IMSI_DEPART_LEN];
    VOS_UINT32                          ulCatIndex;
    SC_PI_FILE_STRU                    *pstPIFile;

    /*针对如下两种情况，需判断此单板的HUK是否无效(无效即为全0，代表未写入HUK)，
      若无效，则按照不锁网处理:
      1.产线试制若不走锁网锁卡定制工位，则HUK不写入，此情况需保证单板不被锁住，可正常使用;
      2.某些运营商不定制锁网，则产线可能不走锁网锁卡定制工位，则HUK不写入，此情况需保证单板不被锁住，可正常使用*/
    if (VOS_FALSE == DRV_CHECK_HUK_IS_VALID())
    {
        *penVerifySimlockRslt = SC_VERIFY_SIMLOCK_RESULT_READY;
        return SC_ERROR_CODE_NO_ERROR;
    }

    *penVerifySimlockRslt = SC_VERIFY_SIMLOCK_RESULT_BUTT;

    /* 申请PI文件缓冲内存 */
    pstPIFile = (SC_PI_FILE_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                                               DYNAMIC_MEM_PT,
                                               sizeof(SC_PI_FILE_STRU));

    if (VOS_NULL_PTR == pstPIFile)
    {
        SC_ERROR_LOG("SC_PERS_VerifySimLock: VOS_MemAlloc is failed.");

        /* 返回内存申请失败 */
        return SC_ERROR_CODE_ALLOC_MEM_FAIL;
    }

    /* 读取PI文件并进行校验 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_PI,
                                            (VOS_UINT8*)pstPIFile,
                                            sizeof(SC_PI_FILE_STRU));

    /* 返回失败 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_VerifySimLock: SC_COMM_ReadSCFileAndCmpSign is failed.",
                      ulResult);

        return ulResult;
    }

    pucEf   = VOS_NULL_PTR;
    ulEfLen = 0;

    /* 调用USIMM_GetCachedFile获取IMSI */
    ulResult = SC_COMM_GetUsimmCachedFile(EF_IMSI_FILE_ID, &ulEfLen, &pucEf, USIMM_UNLIMIT_APP, enModemID);

    /* 返回失败 */
    if (USIMM_API_SUCCESS != ulResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_VerifySimLock: USIMM_GetCachedFile is failed.",
                      ulResult);

        return SC_ERROR_CODE_OTHER_ERROR;
    }

    /* 由BCD转换为数字 */
    ulResult = SC_PERS_ImsiBcd2Num((VOS_UINT8)ulEfLen, pucEf, aucIMSI);

    if (VOS_OK != ulResult)
    {
        *penVerifySimlockRslt = SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN;

        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_VerifySimLock: SC_PERS_ImsiBcd2Num is failed.",
                      ulResult);

        return SC_ERROR_CODE_NO_ERROR;
    }

    /* 循环category进行号段校验 */
    for (ulCatIndex = 0; ulCatIndex < SC_MAX_SUPPORT_CATEGORY; ulCatIndex++)
    {
        ulResult = SC_PERS_CheckCode(&pstPIFile->astSimlockCategory[ulCatIndex],
                                     aucIMSI,
                                     enModemID);

        if (VOS_FALSE == ulResult)
        {
            *penVerifySimlockRslt = SC_PERS_GetStatusByCategory(&pstPIFile->astSimlockCategory[ulCatIndex]);
            break;
        }

        *penVerifySimlockRslt = SC_VERIFY_SIMLOCK_RESULT_READY;
    }

    VOS_MemFree(WUEPS_PID_OM, pstPIFile);

    return SC_ERROR_CODE_NO_ERROR;
}


VOS_VOID SC_PERS_AsciiNum2DecNum(
    VOS_UINT8                          *pucAsciiNum,
    VOS_UINT8                          *pucDecNum,
    VOS_UINT32                          ulLen
)
{
    VOS_UINT32                          ulIndex         = 0;

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */
    /* 只判断 "0" ~ "9" */

    for (ulIndex = 0; ulIndex < ulLen; ulIndex++)
    {
        /* 判断是否是数字 */
        if ( ('0' <= pucAsciiNum[ulIndex]) && ('9' >= pucAsciiNum[ulIndex]) )
        {
            pucDecNum[ulIndex] = pucAsciiNum[ulIndex] - '0';
        }
    }

    return ;
}
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_ReadCMPIMEI(MODEM_ID_ENUM_UINT16 enModemID)
{
    VOS_UINT8                           aucScImei [SC_RSA_ENCRYPT_LEN]  = {0};
    VOS_UINT8                           aucPubImei[SC_IMEI_LEN + 1] = {0};
    VOS_UINT8                           aucNvImei [SC_IMEI_NV_LEN]  = {0};
    VOS_UINT32                          ulPubLen = SC_IMEI_LEN;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucImeiNum[SC_IMEI_NV_LEN] = {0};

    /* HUK 是否写入*/
    if (VOS_FALSE == DRV_CHECK_HUK_IS_VALID())
    {
        return SC_ERROR_CODE_NO_ERROR;
    }

    /* 读取IMEI-FILE并进行签名校验 */
    SC_INFO1_LOG("#info:SC_PERS_VerifyIMEI: modem id is: .", enModemID);
    if (MODEM_ID_0 == enModemID)
    {
        ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_IMEI_I0,
                                            aucScImei,
                                            SC_RSA_ENCRYPT_LEN);
    }
    else if (MODEM_ID_1 == enModemID)
    {
        ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_IMEI_I1,
                                            aucScImei,
                                            SC_RSA_ENCRYPT_LEN);
    }
    else
    {
        SC_ERROR_LOG("SC_PERS_VerifyIMEI: modem id  error.");
        return SC_ERROR_CODE_MODEM_ID_FAIL;
    }

    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_VerifyIMEI: SC_COMM_ReadSCFileAndCmpSign is failed.", ulResult);

        return ulResult;
    }

    /* IMEI 密文解密 */
    ulResult = SC_COMM_RsaDecrypt(aucScImei, SC_RSA_ENCRYPT_LEN, aucPubImei, &ulPubLen);
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_VerifyIMEI: SC_COMM_RsaDecrypt is failed.", ulResult);

        return ulResult;
    }

    /* 由于AT命令下发密文码流直接保存到文件中，再次解密得到的明文是字符串，
       NV项读取的到得码流是十进制数字串，存在不一致，统一转换成十进制数字串  */
    SC_PERS_AsciiNum2DecNum(aucPubImei, aucImeiNum, ulPubLen);

    /* 读取NV中明文IMEI 比较 */
    ulResult = NV_ReadEx(enModemID, en_NV_Item_IMEI, aucNvImei, SC_IMEI_NV_LEN);
    if (NV_OK != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_VerifyIMEI: NV_Read Imei is failed.", ulResult);

        return SC_ERROR_CODE_NV_READ_FAIL;
    }

    /* 判断明文IMEI是否相等, IMEI 明文长度 15位 */
    if (VOS_OK != VOS_MemCmp(aucImeiNum, aucNvImei, SC_IMEI_LEN))
    {
        SC_ERROR_LOG("SC_PERS_VerifyIMEI: MemCmp Imei is failed.");

        return SC_ERROR_CODE_CMP_IMEI_FAIL;
    }

    return SC_ERROR_CODE_NO_ERROR;
}


SC_ERROR_CODE_ENUM_UINT32 SC_PERS_VerifyIMEI(MODEM_ID_ENUM_UINT16 enModemID)
{
/* 对读取IMEI低概率失败问题的一个规避，COMM读取三次，只要有一次成功返回，否则返回全0 */
    VOS_UINT32                          ulLoop   = 0;
    SC_ERROR_CODE_ENUM_UINT32           enResult = SC_ERROR_CODE_OTHER_ERROR;

    for(ulLoop=0; ulLoop<SC_MAX_CMP_IMEI; ulLoop++)
    {
        enResult = SC_PERS_ReadCMPIMEI(enModemID);
        if (SC_ERROR_CODE_NO_ERROR == enResult)
        {
            return SC_ERROR_CODE_NO_ERROR;
        }
    }

    return enResult;
}
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_NvRead(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT16                          usID,
    VOS_VOID                           *pItem,
    VOS_UINT32                          ulLength
)
{
    VOS_UINT32                          ulResult;

    /* IMEI 校验 */
    ulResult = SC_PERS_VerifyIMEI(enModemID);
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR_LOG("SC_PERS_NvRead: Verify IMEI fail.");

        VOS_MemSet(pItem, 0, ulLength);

        return SC_ERROR_CODE_NO_ERROR;
    }

    /* 读取NV */
    ulResult = NV_ReadEx(enModemID, usID, pItem, ulLength);
    if (NV_OK != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_NvRead: NV_Read fail.", ulResult);

        return SC_ERROR_CODE_NV_READ_FAIL;
    }

    return SC_ERROR_CODE_NO_ERROR;
}


SC_ERROR_CODE_ENUM_UINT32 SC_PERS_CheckCryptoCKUKPwd(
    VOS_UINT8                          *pucPwd,
    VOS_UINT8                           ucLen,
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enSIMLockStatus,
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8 enCategory
)
{
    SC_CK_FILE_STRU                     stCKFile;
    VOS_UINT32                          ulResult;
    VOS_INT                             lResult;
    VOS_UINT8                           aucCryptoPwd[SC_CRYPTO_PWD_LEN];

    /* 读取CK-FILE并进行签名校验 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_CK,
                                            (VOS_UINT8*)&stCKFile,
                                            sizeof(SC_CK_FILE_STRU));

    /* 如果结果不为SC_ERROR_CODE_NO_ERROR */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_CheckCryptoCKUKPwd: SC_COMM_ReadSCFileAndCmpSign is failed.", ulResult);

        return ulResult;
    }

    /* 对输入的密码加密生成密文 */
    ulResult = SC_COMM_GenerateCryptoPwd(SC_CRYPTO_PASSWORD_TYPE_CK,
                                         pucPwd,
                                         ucLen,
                                         aucCryptoPwd,
                                         SC_CRYPTO_PWD_LEN);

    /* 如果结果不为SC_ERROR_CODE_NO_ERROR */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_CheckCryptoCKUKPwd: SC_COMM_GenerateCryptoPwd is failed.", ulResult);

        return ulResult;
    }

    /* 表明所在PIN */
    if ((SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN == enSIMLockStatus)
        || (SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN == enSIMLockStatus)
        || (SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN == enSIMLockStatus))
    {
        lResult = VOS_MemCmp(aucCryptoPwd,
                             stCKFile.astCKCategory[enCategory].aucCKCryptoPwd,
                             SC_CRYPTO_PWD_LEN);
    }
    /* 表明所在PUK */
    else if ((SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK == enSIMLockStatus)
        || (SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK == enSIMLockStatus)
        || (SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK == enSIMLockStatus))
    {
        lResult = VOS_MemCmp(aucCryptoPwd,
                             stCKFile.astCKCategory[enCategory].aucUKCryptoPwd,
                             SC_CRYPTO_PWD_LEN);
    }
    else
    {
        lResult = VOS_ERR;
    }

    /* 密码密文比对失败 */
    if (VOS_OK != lResult)
    {
        SC_ERROR1_LOG("SC_PERS_CheckCryptoCKUKPwd: VOS_MemCmp is failed.", lResult);

        return SC_ERROR_CODE_UNLOCK_KEY_INCORRECT;
    }

    return SC_ERROR_CODE_NO_ERROR;
}
VOS_VOID SC_PERS_UpdateSimlockStatus(
    SC_ERROR_CODE_ENUM_UINT32               enResult,
    SC_SIMLOCK_CATEGORY_STRU               *pstSimlockCategory,
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8    *penVerifySimlockRslt
)
{
    /* 密码校验成功 */
    if (SC_ERROR_CODE_NO_ERROR == enResult)
    {
        /* 更新锁网锁卡的状态，同时次数更新为最大值 */
        if (SC_PERSONALIZATION_STATUS_PIN == pstSimlockCategory->enStatus)
        {
            pstSimlockCategory->enStatus = SC_PERSONALIZATION_STATUS_READY;
        }
        else
        {
            pstSimlockCategory->enStatus = SC_PERSONALIZATION_STATUS_PIN;
        }

        pstSimlockCategory->ucRemainUnlockTimes = pstSimlockCategory->ucMaxUnlockTimes;
    }
    else
    {
        /* PIN状态且失败 */
        if ((SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN == *penVerifySimlockRslt)
            || (SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN == *penVerifySimlockRslt)
            || (SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN == *penVerifySimlockRslt))
        {
            /*更新锁网锁卡剩余次数-1*/
            pstSimlockCategory->ucRemainUnlockTimes -= 1;

            /* 如果剩余次数为0，更新为PUK */
            if (0 == pstSimlockCategory->ucRemainUnlockTimes)
            {
                pstSimlockCategory->enStatus = SC_PERSONALIZATION_STATUS_PUK;
            }
        }
        else
        {
            /*PUK状态且执行失败*/
        }
    }
    return;
}


SC_ERROR_CODE_ENUM_UINT32 SC_PERS_UnLock(
    VOS_UINT8                          *pucPwd,
    VOS_UINT8                           ucLen,
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 *penVerifySimlockRslt,
    MODEM_ID_ENUM_UINT16                enModemID
)
{
    VOS_UINT32                                              ulResult;
    VOS_UINT32                                              ulResultLater;
    SC_PI_FILE_STRU                                        *pstPIFile;
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory;

    /* 调用SC_VerifySimLock获取锁网锁卡状态 */
    ulResult = SC_PERS_VerifySimLock(penVerifySimlockRslt, enModemID);

    /* 如果结果不为SC_ERROR_CODE_NO_ERROR */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UnLock: SC_PERS_VerifySimLock is failed.", ulResult);

        return ulResult;
    }

    /* 如果当前锁网锁卡状态是SC_VERIFY_SIMLOCK_RESULT_READY */
    if (SC_VERIFY_SIMLOCK_RESULT_READY == *penVerifySimlockRslt)
    {
        SC_ERROR1_LOG("SC_PERS_UnLock: The status is wrong.", *penVerifySimlockRslt);

        return SC_ERROR_CODE_UNLOCK_STATUS_ABNORMAL;
    }

    /* 对解锁码长度做判断 */
    if (SC_PERS_PWD_LEN != ucLen)
    {
        SC_ERROR1_LOG("SC_PERS_UnLock: ucLen is not 16!", ucLen);

        return SC_ERROR_CODE_OTHER_ERROR;
    }

    /* 申请PI文件缓冲内存 */
    pstPIFile = (SC_PI_FILE_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                                               sizeof(SC_PI_FILE_STRU));

    if (VOS_NULL_PTR == pstPIFile)
    {
        SC_ERROR_LOG("SC_PERS_UnLock: VOS_MemAlloc is failed.");
        /* 返回内存申请失败 */
        return SC_ERROR_CODE_ALLOC_MEM_FAIL;
    }

    /* 读取PI-FILE并进行签名校验 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_PI,
                                            (VOS_UINT8*)pstPIFile,
                                            sizeof(SC_PI_FILE_STRU));

    /* 如果结果不为SC_ERROR_CODE_NO_ERROR */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_UnLock: SC_COMM_ReadSCFileAndCmpSign is failed.", ulResult);

        return ulResult;
    }

    /* 根据锁网锁卡状态获取category */
    enCategory = SC_PERS_GetCategoryByStatus(*penVerifySimlockRslt);
    if (SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER < enCategory)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_UnLock: SC_PERS_GetCategoryByStatus return bad enCategory.", enCategory);

        return SC_ERROR_CODE_PARA_FAIL;
    }

    /* 密码校验 */
    ulResult = SC_PERS_CheckCryptoCKUKPwd(pucPwd,
                                          ucLen,
                                          *penVerifySimlockRslt,
                                          enCategory);

    /* 根据校验结果更新PI文件的状态 */
    SC_PERS_UpdateSimlockStatus(ulResult,
                                &pstPIFile->astSimlockCategory[enCategory],
                                penVerifySimlockRslt);

    /* 重新生成PI文件 */
    ulResultLater = SC_COMM_WriteSecretFile(SC_SECRET_FILE_TYPE_PI,
                                            (VOS_UINT8*)pstPIFile,
                                            sizeof(SC_PI_FILE_STRU));

    if (SC_ERROR_CODE_NO_ERROR != ulResultLater)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_UnLock: SC_COMM_WriteSecretFile is failed.", ulResultLater);

        return ulResultLater;
    }

    /* 调用SC_VerifySimLock获取锁网锁卡状态 */
    ulResultLater = SC_PERS_VerifySimLock(penVerifySimlockRslt, enModemID);

    if (SC_ERROR_CODE_NO_ERROR != ulResultLater)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR1_LOG("SC_PERS_UnLock: SC_PERS_VerifySimLock is failed.", ulResultLater);

        return ulResultLater;
    }

    VOS_MemFree(WUEPS_PID_OM, pstPIFile);

    return ulResult;
}
VOS_VOID SC_PERS_ErasePIFile(SC_PI_FILE_STRU *pstPiFile)
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory;
    VOS_UINT32                                              i;

    for (i = 0; i < SC_MAX_SUPPORT_CATEGORY; i++)
    {
        enCategory = (SC_PERSONALIZATION_CATEGORY_ENUM_UINT8)i;

        VOS_MemSet((VOS_VOID*)&pstPiFile->astSimlockCategory[i], 0, sizeof(SC_SIMLOCK_CATEGORY_STRU));

        pstPiFile->astSimlockCategory[i].enCategory = enCategory;
    }

    return;
}


VOS_VOID SC_PERS_EraseCKFile(SC_CK_FILE_STRU *pstCkFile)
{
    VOS_MemSet(pstCkFile, 0, sizeof(SC_CK_FILE_STRU));

    return;
}


SC_ERROR_CODE_ENUM_UINT32 SC_PERS_UpdatePIFile(
    SC_PI_FILE_STRU                    *pstPiFile,
    SC_WRITE_SIMLOCK_DATA_STRU         *pstWriteSimLockData
)
{
    VOS_UINT32                          ulResult;
    VOS_INT                             lResult;
    VOS_UINT32                          ulRsaDecryptLen;
    VOS_INT                             lHashLen;
    VOS_UINT8                           aucRsaDecrypt[SC_HASH_LEN];
    VOS_UINT8                           aucHash[SC_HASH_LEN];

    if (SC_MAX_SUPPORT_CATEGORY <= pstWriteSimLockData->enCategory)
    {
        SC_ERROR_LOG("SC_PERS_UpdatePIFile: Catogory is not support.");

        return SC_ERROR_CODE_LOCK_CODE_INVALID;
    }

    /* 先使用AK-File中的公钥对aucSimlockRsaData进行解密 */
    ulRsaDecryptLen = SC_HASH_LEN;

    ulResult = SC_COMM_RsaDecrypt(pstWriteSimLockData->aucSimlockRsaData,
                                  SC_RSA_ENCRYPT_LEN,
                                  aucRsaDecrypt,
                                  &ulRsaDecryptLen);

    /* 如果进行RSA解密失败，返回错误原因值 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UpdatePIFile: SC_COMM_RsaDecrypt is failed.", ulResult);

        return ulResult;
    }

    /* 通过MD5算法，计算输入锁网锁卡的号段内容的摘要 */
    lHashLen = SC_HASH_LEN;

    lResult = CRYPTO_HASH((VOS_CHAR*)pstWriteSimLockData->astLockCode,
                          SC_MAX_CODE_NUM*sizeof(SC_PH_LOCK_CODE_STRU),
                          CRYPTO_ALGORITHM_SHA256,
                          (VOS_CHAR*)aucHash,
                          &lHashLen);

    /* 如果通过计算MD5摘要失败，返回计算HASH失败 */
    if ((VOS_OK != lResult) || (SC_HASH_LEN != lHashLen))
    {
        SC_ERROR1_LOG("SC_PERS_UpdatePIFile: CRYPTO_HASH is failed.", ulResult);

        return SC_ERROR_CODE_GENERATE_HASH_FAIL;
    }

#if (VOS_WIN32 != VOS_OS_VER)
    /* 由于PC工程上无法模拟RSA加密(因为RSA需要私钥和公钥对，但私钥无法获取，无法模拟真实的RSA加密码流)，
        故如下这段逻辑在PC工程上不执行 */
    /* 如果RSA解密和MD5计算的摘要长度不相等，或者两者比较不相等返回 */
    if (VOS_OK != VOS_MemCmp(aucRsaDecrypt, aucHash, SC_HASH_LEN))
    {
        SC_ERROR_LOG("SC_PERS_UpdatePIFile: VOS_MemCmp is failed.");

        return SC_ERROR_CODE_VERIFY_SIGNATURE_FAIL;
    }
#endif

    /* 将输入的锁网锁卡的号段内容写入PI文件的内存中 */
    VOS_MemCpy(pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].astLockCode,
               pstWriteSimLockData->astLockCode,
               sizeof(SC_PH_LOCK_CODE_STRU) * SC_MAX_CODE_NUM);

    /* 将AT命令传入的enIndicator/ucGroupNum/ucMaxUnlockTimes写入，并更新最大剩余次数 */
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].enIndicator          = pstWriteSimLockData->enIndicator;
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].ucGroupNum           = pstWriteSimLockData->ucGroupNum;
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].ucMaxUnlockTimes     = pstWriteSimLockData->ucMaxUnlockTimes;
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].ucRemainUnlockTimes  = pstWriteSimLockData->ucMaxUnlockTimes;
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].enCategory           = pstWriteSimLockData->enCategory;
    pstPiFile->astSimlockCategory[pstWriteSimLockData->enCategory].enStatus             = SC_PERSONALIZATION_STATUS_PIN;

    /* 将新生成的PI文件写入文件系统 */
    ulResult = SC_COMM_WriteSecretFile(SC_SECRET_FILE_TYPE_PI,
                                       (VOS_UINT8*)pstPiFile,
                                       sizeof(SC_PI_FILE_STRU));

    SC_INFO1_LOG("SC_PERS_UpdatePIFile: SC_COMM_WriteSecretFile result is:", ulResult);

    return ulResult;

}


SC_ERROR_CODE_ENUM_UINT32 SC_PERS_UpdateCKUKPwd(
    VOS_UINT8                                              *pucPwd,
    VOS_UINT8                                              *pucEncryptPwd,
    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulPwdDecryptLen;
    VOS_UINT32                          ulCipherLen;
    VOS_UINT8                           aucPwdDecrypt[SC_PERS_PWD_LEN];
    VOS_UINT8                           aucCipherData[SC_CRYPTO_PWD_LEN];
    VOS_UINT8                           ucLoop;

    ulPwdDecryptLen = SC_PERS_PWD_LEN;

    /* 先使用AK-File中的公钥对pucEncryptPwd进行解密 */
    ulResult = SC_COMM_RsaDecrypt(pucEncryptPwd,
                                  SC_RSA_ENCRYPT_LEN,
                                  aucPwdDecrypt,
                                  &ulPwdDecryptLen);

    /* 如果进行RSA解密失败，返回错误原因值 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UpdateCKUKPwd: SC_COMM_RsaDecrypt is failed.", ulResult);

        return ulResult;
    }

    if (SC_PERSONALIZATION_INDICATOR_ACTIVE == enIndicator)
    {
        /* 判断解密后的密码明文是否为数值为"0"~"9"的数字 */
        for (ucLoop = 0; ucLoop < SC_PERS_PWD_LEN; ucLoop++)
        {
            if (0x09 < aucPwdDecrypt[ucLoop])
            {
                SC_ERROR_LOG("SC_PERS_UpdateCKUKPwd: aucPwdDecrypt is incorrect!");

                return SC_ERROR_CODE_OTHER_ERROR;
            }
        }
    }

    /* 生成密码密文 */
    ulCipherLen = SC_CRYPTO_PWD_LEN;

    ulResult = SC_COMM_GenerateCryptoPwd(SC_CRYPTO_PASSWORD_TYPE_CK,
                                         aucPwdDecrypt,
                                         (VOS_UINT8)ulPwdDecryptLen,
                                         aucCipherData,
                                         ulCipherLen);

    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UpdateCKUKPwd: SC_COMM_GenerateCryptoPwd is failed.", ulResult);

        return ulResult;
    }

    /* 将计算出来的密码写回到CK文件内存中 */
    VOS_MemCpy(pucPwd, aucCipherData, ulCipherLen);

    return SC_ERROR_CODE_NO_ERROR;
}
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_UpdateCKFile(
    SC_CK_FILE_STRU                    *pstCkFile,
    SC_WRITE_SIMLOCK_DATA_STRU         *pstWriteSimLockData
)
{
    VOS_UINT32                          ulResult;

    /* 将AT命令输入的PIN码进行RSA解密后再AES ECB算法加密再写入CK文件的内存中 */
    ulResult = SC_PERS_UpdateCKUKPwd(pstCkFile->astCKCategory[pstWriteSimLockData->enCategory].aucCKCryptoPwd,
                                     pstWriteSimLockData->aucLockRsaPin,
                                     pstWriteSimLockData->enIndicator);

    /* 更新CK不成功，返回失败原因 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UpdateCKFile: SC_PERS_UpdateCKUKPwd is failed.", ulResult);

        return ulResult;
    }

    /* 将AT命令输入的PUK码进行RSA解密后再AES ECB算法加密再写入CK文件的内存中 */
    ulResult = SC_PERS_UpdateCKUKPwd(pstCkFile->astCKCategory[pstWriteSimLockData->enCategory].aucUKCryptoPwd,
                                     pstWriteSimLockData->aucLockRsaPuk,
                                     pstWriteSimLockData->enIndicator);

    /* 更新UK不成功，返回失败原因 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        SC_ERROR1_LOG("SC_PERS_UpdateCKFile: SC_PERS_UpdateCKUKPwd is failed.", ulResult);

        return ulResult;
    }

    /* 生成新的CK文件和签名文件 */
    ulResult = SC_COMM_WriteSecretFile(SC_SECRET_FILE_TYPE_CK,
                                       (VOS_UINT8*)pstCkFile,
                                       sizeof(SC_CK_FILE_STRU));

    SC_ERROR1_LOG("SC_PERS_UpdateCKFile: SC_COMM_WriteSecretFile result is:", ulResult);

    return ulResult;
}
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_WriteSimLockData(SC_WRITE_SIMLOCK_DATA_STRU *pstWriteSimLockData)
{
    VOS_UINT32                          ulResult;
    SC_PI_FILE_STRU                    *pstSimlockInfo;
    SC_CK_FILE_STRU                     stCkFile;
    VOS_UINT32                          ulFlag;

    ulFlag = VOS_FALSE;

    /* 鉴权未完成直接返回身份校验未完成 */
    if (SC_AUTH_STATUS_UNDO == SC_CTX_GetFacAuthStatus())
    {
        SC_ERROR_LOG("SC_PERS_WriteSimLockData: SC_CTX_GetFacAuthStatus is not done.");
        /* 返回身份校验未完成 */
        return SC_ERROR_CODE_IDENTIFY_NOT_FINISH;
    }

    /* 由于PI文件比较大，为PI文件申请内存 */
    pstSimlockInfo = (SC_PI_FILE_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, sizeof(SC_PI_FILE_STRU));

    if (VOS_NULL_PTR == pstSimlockInfo)
    {
        SC_ERROR_LOG("SC_PERS_WriteSimLockData: VOS_MemAlloc is failed.");

        /* 返回内存申请失败 */
        return SC_ERROR_CODE_ALLOC_MEM_FAIL;
    }

    /* 读取PIFile文件内容并比较签名文件内容 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_PI,
                                            (VOS_UINT8*)pstSimlockInfo,
                                            sizeof(SC_PI_FILE_STRU));

    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        /* 设置标记为真 */
        ulFlag = VOS_TRUE;
    }

    /* 读取CKFile文件内容并比较签名文件内容 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_CK,
                                            (VOS_UINT8*)&stCkFile,
                                            sizeof(SC_CK_FILE_STRU));

    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        /* 设置标记为真 */
        ulFlag = VOS_TRUE;
    }

    /* PI或CK文件读取或比较签名不正确，需要将两个文件的内容清零 */
    if (VOS_TRUE == ulFlag)
    {
        /* 如果之前发现PI或CK文件读取失败或签名比较失败，将PI文件中除CATEGORY外的所有字段全部清零 */
        SC_PERS_ErasePIFile(pstSimlockInfo);

        /* CK文件内容全部清零 */
        SC_PERS_EraseCKFile(&stCkFile);
    }

    /* 写入锁网数据前，先进行参数有效性检查(检查enCategory、enIndicator、ucGroupNum、ucMaxUnlockTimes，
       号段内容不做合法性检查) */
    if ((pstWriteSimLockData->enCategory >= SC_MAX_SUPPORT_CATEGORY)
     || (pstWriteSimLockData->enIndicator >= SC_PERSONALIZATION_INDICATOR_BUTT)
     || (pstWriteSimLockData->ucGroupNum > SC_MAX_CODE_NUM)
     || ( (SC_PERSONALIZATION_INDICATOR_ACTIVE == pstWriteSimLockData->enIndicator)
       && (0 == pstWriteSimLockData->ucMaxUnlockTimes)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstSimlockInfo);

        SC_ERROR_LOG("SC_PERS_WriteSimLockData: Parameter check fail!");
        return SC_ERROR_CODE_OTHER_ERROR;
    }

    /* 将改制数据内容写入CK文件的缓存中 */
    ulResult = SC_PERS_UpdateCKFile(&stCkFile, pstWriteSimLockData);

    /* 如果写入失败，返回错误原因值并释放内存 */
    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pstSimlockInfo);

        SC_ERROR1_LOG("SC_PERS_WriteSimLockData: SC_PERS_UpdateCKFile is failed.", ulResult);

        return ulResult;
    }

    /* 将改制数据内容写入PI文件的中 */
    ulResult = SC_PERS_UpdatePIFile(pstSimlockInfo, pstWriteSimLockData);

    /* 释放内存，返回操作结果 */
    VOS_MemFree(WUEPS_PID_OM, pstSimlockInfo);

    SC_INFO1_LOG("SC_PERS_WriteSimLockData: SC_PERS_UpdatePIFile result is:", ulResult);

    return ulResult;
}



SC_ERROR_CODE_ENUM_UINT32 SC_PERS_GetSimlockInfo(SC_SIMLOCK_INFO_STRU *pstSimlockInfo)
{
    VOS_UINT32                          ulResult;

    /* 读取PI-FILE并进行签名校验 */
    ulResult = SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_PI,
                                            (VOS_UINT8*)pstSimlockInfo,
                                            sizeof(SC_SIMLOCK_INFO_STRU));

    SC_INFO1_LOG("SC_PERS_GetSimlockInfo: SC_COMM_ReadSCFileAndCmpSign result is:", ulResult);

    return ulResult;
}
VOS_UINT32 SC_PERS_SimlockUnlock(
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8  enCategory,
    VOS_UINT8                              *pucPwd)
{
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 enVerifySimlockRslt;
    SC_PI_FILE_STRU                    *pstPIFile;
    VOS_UINT32                          ulResult;

    if (SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER < enCategory)
    {
        SC_ERROR_LOG("SC_PERS_SimlockUnlock: Category error.");

        return VOS_ERR;
    }


    /* 申请PI文件缓冲内存 */
    pstPIFile = (SC_PI_FILE_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                                               sizeof(SC_PI_FILE_STRU));

    if (VOS_NULL_PTR == pstPIFile)
    {
        SC_ERROR_LOG("SC_PERS_SimlockUnlock: VOS_MemAlloc is failed.");
        /* 返回内存申请失败 */
        return VOS_ERR;
    }

    /* 读取PI-FILE并进行签名校验,如果结果不为SC_ERROR_CODE_NO_ERROR */
    if (SC_ERROR_CODE_NO_ERROR != SC_COMM_ReadSCFileAndCmpSign(SC_SECRET_FILE_TYPE_PI,
                                                               (VOS_UINT8*)pstPIFile,
                                                               sizeof(SC_PI_FILE_STRU)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR_LOG("SC_PERS_SimlockUnlock: SC_COMM_ReadSCFileAndCmpSign is failed.");

        return VOS_ERR;
    }

    /* 对应的enCategory类型未激活SIMLCOK功能 */
    if (SC_PERSONALIZATION_INDICATOR_ACTIVE != pstPIFile->astSimlockCategory[enCategory].enIndicator)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR_LOG("SC_PERS_SimlockUnlock: SimLock not active.");

        return VOS_OK;
    }

    /* 对应的enCategory类型SIMLCOK已经验证通过 */
    if (SC_PERSONALIZATION_STATUS_READY == pstPIFile->astSimlockCategory[enCategory].enStatus)
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR_LOG("SC_PERS_SimlockUnlock: SimLock PIN status is ready.");

        return VOS_OK;
    }

    enVerifySimlockRslt = SC_PERS_GetStatusByCategory(&pstPIFile->astSimlockCategory[enCategory]);

    /* 密码校验 */
    ulResult    = SC_PERS_CheckCryptoCKUKPwd(pucPwd,
                                             SC_PERS_PWD_LEN,
                                             enVerifySimlockRslt,
                                             enCategory);
    /* 根据校验结果更新PI文件的状态 */
    SC_PERS_UpdateSimlockStatus(ulResult,
                                &pstPIFile->astSimlockCategory[enCategory],
                                &enVerifySimlockRslt);

    /* 重新生成PI文件 */
    if (VOS_OK != SC_COMM_WriteSecretFile(SC_SECRET_FILE_TYPE_PI,
                                          (VOS_UINT8*)pstPIFile,
                                          sizeof(SC_PI_FILE_STRU)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstPIFile);

        SC_ERROR_LOG("SC_PERS_SimlockUnlock: SC_COMM_WriteSecretFile is failed.");

        return VOS_ERR;
    }

    VOS_MemFree(WUEPS_PID_OM, pstPIFile);

    if (SC_ERROR_CODE_NO_ERROR != ulResult)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

