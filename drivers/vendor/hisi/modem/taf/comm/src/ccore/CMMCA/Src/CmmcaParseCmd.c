
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CmmcaCtx.h"
#include "CmmcaParseCmd.h"
#include "CmmcaMntn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_CMMCA_PARSE_CMD_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/


VOS_UINT16 CMMCA_ConvertDoubleOctetStrToU16(VOS_UINT8 *pucStr)
{
    VOS_UINT16                          usTmpValue;
    VOS_UINT16                          usResult;
    VOS_UINT8                           ucIndex;

    usResult = 0;

    for (ucIndex = 0; ucIndex < sizeof(VOS_UINT16); ucIndex++)
    {
        usTmpValue = *(pucStr + ucIndex);
        usTmpValue = (VOS_UINT16)(usTmpValue << (CMMCA_BITS_PER_OCTET * ucIndex));

        usResult += usTmpValue;
    }

    return usResult;

}


VOS_UINT32 CMMCA_ConvertFourOctetStrToU32(VOS_UINT8 *pucStr)
{
    VOS_UINT32                          ulTmpValue;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    ulResult = 0;

    for (ucIndex = 0; ucIndex < sizeof(VOS_UINT32); ucIndex++)
    {
        ulTmpValue = *(pucStr + ucIndex);
        ulTmpValue = (VOS_UINT32)(ulTmpValue << (CMMCA_BITS_PER_OCTET * ucIndex));

        ulResult += ulTmpValue;
    }

    return ulResult;
}


VOS_VOID CMMCA_ConvertU16ToDoubleOctetStr(
    VOS_UINT16                          usResult,
    VOS_UINT8                          *pucStr
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucTmpValue;

    for (ucIndex = 0; ucIndex < sizeof(VOS_UINT16); ucIndex++)
    {
        ucTmpValue = (VOS_UINT8)(usResult >> (CMMCA_BITS_PER_OCTET * ucIndex));

        *(pucStr + ucIndex) = ucTmpValue;
    }

    return;
}


VOS_VOID CMMCA_ConvertU32ToFourOctetStr(
    VOS_UINT32                          ulResult,
    VOS_UINT8                          *pucStr
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucTmpValue;

    for (ucIndex = 0; ucIndex < sizeof(VOS_UINT32); ucIndex++)
    {
        ucTmpValue = (VOS_UINT8)(ulResult >> (CMMCA_BITS_PER_OCTET * ucIndex));

        *(pucStr + ucIndex) = ucTmpValue;
    }

    return;
}




VOS_UINT32 CMMCA_IsNasMccValid(
    VOS_UINT32                          ulNasMcc
)
{
    /* 存储U32中的MCC的字节 */
    VOS_UINT8                           aucSrcTmp[CMMCA_U32_MCC_CONTAN_OCTET_NUM];
    VOS_UINT32                          ulNasMccTmp;
    VOS_UINT32                          i;

    ulNasMccTmp = ulNasMcc;

    for (i = 0; i < CMMCA_U32_MCC_CONTAN_OCTET_NUM; i++ )
    {
        aucSrcTmp[i]   = (ulNasMccTmp & CMMCA_WORD_LOW_EIGHT_BITS);

        /* MCC的有效字符为0~9 */
        if ( aucSrcTmp[i] > 9)
        {
            return VOS_FALSE;
        }

        /* 指向下一个字节 */
        ulNasMccTmp = (ulNasMccTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
    }

    return VOS_TRUE;
}
VOS_UINT32 CMMCA_IsNasMncValid(
    VOS_UINT32                          ulNasMnc
)
{
    /* 存储U32中的MCC的字节 */
    VOS_UINT8                           aucSrcTmp[CMMCA_U32_MCC_CONTAN_OCTET_NUM];
    VOS_UINT32                          ulNasMncTmp;
    VOS_UINT32                          i;

    ulNasMncTmp = ulNasMnc;

    for (i = 0; i < CMMCA_U32_MNC_CONTAN_OCTET_NUM; i++ )
    {
        aucSrcTmp[i]   = (ulNasMncTmp & CMMCA_WORD_LOW_EIGHT_BITS);

        /* MNC的有效字符为0~9,但第三位可以为0X0F */
        if ( (2 == i)
          && (CMMCA_U32_MNC_THIRD_OCTET_NON_EXIST_VALUE == aucSrcTmp[i]) )
        {
            /* 指向下一个字节 */
            ulNasMncTmp = (ulNasMncTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);

            continue;
        }

        /* MCC的有效字符为0~9 */
        if (aucSrcTmp[i] > 9)
        {
            return VOS_FALSE;
        }

        /* 指向下一个字节 */
        ulNasMncTmp = (ulNasMncTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
    }

    return VOS_TRUE;
}
VOS_VOID CMMCA_ConvertNasMccToTwoBcdMccOctetStr(
    VOS_UINT32                          ulNasMcc,
    VOS_UINT8                          *pucMccStr
)
{
    /* 存储U32中的MCC的字节 */
    VOS_UINT8                           aucSrcTmp[CMMCA_U32_MCC_CONTAN_OCTET_NUM];
    VOS_UINT32                          ulNasMccTmp;
    VOS_UINT32                          i;

    ulNasMccTmp = ulNasMcc;

    /* MCC或者MNC的第一位包含F,则为无效PLMN，填写给VIA为0X00 0XFF 0XFF 0XFF */
    if (VOS_FALSE == CMMCA_IsNasMccValid(ulNasMcc))
    {
        /* 获取第一个字节码流,恒为0 */
        pucMccStr[0] = 0XFF;

        /* 获取第二个字节码流,恒为FF */
        pucMccStr[1] = 0xFF;

        return;
    }

    for (i = 0; i < CMMCA_U32_MCC_CONTAN_OCTET_NUM; i++ )
    {
        aucSrcTmp[i]   = (ulNasMccTmp & CMMCA_OCTET_LOW_FOUR_BITS);

        /* 将0转换为A */
        if (0 == aucSrcTmp[i])
        {
            aucSrcTmp[i] = CMMCA_BCD_PLMN_ZERO_TRANSFER;
        }

        /* 指向下一个字节 */
        ulNasMccTmp = (ulNasMccTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
    }

    /* 获取第一个字节码流 */
    pucMccStr[0] = aucSrcTmp[0];

    /* 获取第二个字节码流,将后两位打包为一个字节 */
    pucMccStr[1] = ( aucSrcTmp[2] | (VOS_UINT8)(aucSrcTmp[1] << CMMCA_OCTET_MOVE_FOUR_BITS) );

    return;
}


VOS_VOID CMMCA_ConvertNasMncToTwoBcdMncOctetStr(
    VOS_UINT32                          ulNasMnc,
    VOS_UINT8                          *pucMncStr
)
{
    /* 存储U32中的MCC的字节 */
    VOS_UINT8                           aucSrcTmp[CMMCA_U32_MCC_CONTAN_OCTET_NUM];
    VOS_UINT32                          ulNasMncTmp;
    VOS_UINT32                          i;

    ulNasMncTmp = ulNasMnc;

    /* MCC或者MNC的第一位包含F,则为无效PLMN，填写给VIA为0X00 0XFF 0XFF 0XFF */
    if (VOS_FALSE == CMMCA_IsNasMncValid(ulNasMnc))
    {
        /* 获取第一个字节码流,恒为0 */
        pucMncStr[0] = 0XFF;

        /* 获取第二个字节码流,恒为FF */
        pucMncStr[1] = 0xFF;

        return;
    }

    for (i = 0; i < CMMCA_U32_MNC_CONTAN_OCTET_NUM; i++ )
    {
        aucSrcTmp[i]   = (ulNasMncTmp & CMMCA_OCTET_LOW_FOUR_BITS);

        if (0 == aucSrcTmp[i])
        {
            /* 将0转换为A */
            aucSrcTmp[i] = CMMCA_BCD_PLMN_ZERO_TRANSFER;
        }

        /* 指向下一个字节 */
        ulNasMncTmp = (ulNasMncTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
    }

    /* 获取第一个字节码流 */
    pucMncStr[0] = aucSrcTmp[0];

    /* 获取第二个字节码流,将后两位打包为一个字节 */
    pucMncStr[1] = ( aucSrcTmp[2] | (VOS_UINT8)(aucSrcTmp[1] << CMMCA_OCTET_MOVE_FOUR_BITS) );

    return;
}


VOS_VOID CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(
    VOS_UINT32                          ulNasMcc,
    VOS_UINT32                          ulNasMnc,
    VOS_UINT8                          *pucPlmnStr
)
{
    /* 存储U32中的MCC的字节 */
    VOS_UINT8                           aucMccSrcTmp[CMMCA_U32_MCC_CONTAN_OCTET_NUM];
    VOS_UINT8                           aucMncSrcTmp[CMMCA_U32_MNC_CONTAN_OCTET_NUM];
    VOS_UINT32                          ulNasMccTmp;
    VOS_UINT32                          ulNasMncTmp;
    VOS_UINT32                          ulIsNasMccValid;
    VOS_UINT32                          ulIsNasMncValid;
    VOS_UINT32                          i;

    ulNasMccTmp = ulNasMcc;
    ulNasMncTmp = ulNasMnc;

    ulIsNasMccValid = CMMCA_IsNasMccValid(ulNasMcc);
    ulIsNasMncValid = CMMCA_IsNasMncValid(ulNasMnc);

    /* MCC或者MNC的第一位包含F,则为无效PLMN，填写给VIA为0X00 0XFF 0XFF 0XFF */
    if ( (VOS_FALSE == ulIsNasMccValid)
      || (VOS_FALSE == ulIsNasMncValid) )
    {
        /* 获取第一个字节码流,恒为0 */
        pucPlmnStr[0] = 0XFF;

        /* 获取第二个字节码流,恒为FF */
        pucPlmnStr[1] = 0xFF;

        /* 获取第三个字节码流,恒为FF */
        pucPlmnStr[2] = 0XFF;

        /* 获取第四个字节码流,恒为FF */
        pucPlmnStr[3] = 0XFF;

        return;
    }

    /* 解析出MCC/MNC */
    for (i = 0; i < CMMCA_U32_MNC_CONTAN_OCTET_NUM; i++ )
    {
        aucMccSrcTmp[i]   = (ulNasMccTmp & CMMCA_OCTET_LOW_FOUR_BITS);
        aucMncSrcTmp[i]   = (ulNasMncTmp & CMMCA_OCTET_LOW_FOUR_BITS);

        /* 如果字节为0X00,则转换为0X0A */
        if (0 == aucMncSrcTmp[i])
        {
            /* 将0转换为A */
            aucMncSrcTmp[i] = CMMCA_BCD_PLMN_ZERO_TRANSFER;
        }

        if (0 == aucMccSrcTmp[i])
        {
            /* 将0转换为A */
            aucMccSrcTmp[i] = CMMCA_BCD_PLMN_ZERO_TRANSFER;
        }

        /* 指向下一个字节 */
        ulNasMccTmp = (ulNasMccTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
        ulNasMncTmp = (ulNasMncTmp >> CMMCA_OCTET_MOVE_EIGHT_BITS);
    }

    /* 获取第一个字节码流,恒为0 */
    pucPlmnStr[0] = 0X00;

    /* 获取第二个字节码流,为MCC的第一位与第二位的拼接 */
    pucPlmnStr[1] = ( aucMccSrcTmp[1] | (VOS_UINT8)(aucMccSrcTmp[0] << CMMCA_OCTET_MOVE_FOUR_BITS) );

    /* 获取第三个字节码流,为MCC的第三位与MNC的第一位的拼接 */
    pucPlmnStr[2] = ( aucMncSrcTmp[0] | (VOS_UINT8)(aucMccSrcTmp[2] << CMMCA_OCTET_MOVE_FOUR_BITS) );

    /* 获取第四个字节码流,为MNC的第二位与MNC的第三位的拼接 */
    pucPlmnStr[3] = ( aucMncSrcTmp[2] | (VOS_UINT8)(aucMncSrcTmp[1] << CMMCA_OCTET_MOVE_FOUR_BITS) );

    return;
}
VOS_VOID CMMCA_ConvertFourBcdMccMncOctetStrToNasPlmn(
    VOS_UINT8                          *pucPlmnStr,
    VOS_UINT32                         *pulNasMcc,
    VOS_UINT32                         *pulNasMnc
)
{
    VOS_UINT8                           ucSrcTmp;

    *pulNasMcc = 0;
    *pulNasMnc = 0;

    /* 获取第二个字节码流,将高四位解析出来为MCC第一位字节 */
    ucSrcTmp = (pucPlmnStr[1] >> CMMCA_OCTET_MOVE_FOUR_BITS) & CMMCA_OCTET_LOW_FOUR_BITS;
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMcc |= (VOS_UINT32)ucSrcTmp;

    /* 获取第二个字节码流,将低四位解析出来MCC第二位字节 */
    ucSrcTmp = pucPlmnStr[1] & CMMCA_OCTET_LOW_FOUR_BITS;
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMcc |= (VOS_UINT32)(ucSrcTmp << CMMCA_OCTET_MOVE_EIGHT_BITS);

    /* 获取第三个字节码流,将第二个码流高四位解析出来作为MCC第三位字节 */
    ucSrcTmp = ((pucPlmnStr[2] >> CMMCA_OCTET_MOVE_FOUR_BITS) & CMMCA_OCTET_LOW_FOUR_BITS);
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMcc |= (VOS_UINT32)(ucSrcTmp << CMMCA_OCTET_MOVE_SIXTEEN_BITS);

    /* 获取第三个字节码流,将低四位打包MNC为一个字节 */
    ucSrcTmp = (pucPlmnStr[2] & CMMCA_OCTET_LOW_FOUR_BITS);
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMnc |= (VOS_UINT32)ucSrcTmp;

    /* 获取第四个字节码流,将高四位解析出来作为MNC第二位字节 */
    ucSrcTmp = ((pucPlmnStr[3] >> CMMCA_OCTET_MOVE_FOUR_BITS) & CMMCA_OCTET_LOW_FOUR_BITS);
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMnc |= (VOS_UINT32)(ucSrcTmp << CMMCA_OCTET_MOVE_EIGHT_BITS);

    /* 获取第四个字节码流,将低四位解析出来作为MNC第三位字节 */
    ucSrcTmp = (pucPlmnStr[3] & CMMCA_OCTET_LOW_FOUR_BITS);
    if (CMMCA_BCD_PLMN_ZERO_TRANSFER == ucSrcTmp)
    {
        ucSrcTmp = 0x00;
    }
    *pulNasMnc |= (VOS_UINT32)(ucSrcTmp << CMMCA_OCTET_MOVE_SIXTEEN_BITS);

    return;
}


#if (FEATURE_ON == FEATURE_CL_INTERWORK)


VOS_UINT8 CMMCA_CheckApnFormat(
    VOS_UINT8                          *pucApn,
    VOS_UINT16                          usApnLen
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucApnCharacter;

    if (usApnLen > CMMCA_APN_LEN_MAX)
    {
        return VOS_ERR;
    }

    for (i = 0; i < usApnLen; i++)
    {
        ucApnCharacter = pucApn[i];

        /* 检查APN字符有效性 */
        if (!CMMCA_IS_VALID_APN_CHAR(ucApnCharacter))
        {
            CMMCA_ERROR_LOG("CMMCA_CheckApnFormat: Invalid character in APN.");
            return VOS_ERR;
        }
    }

    /* 检查APN开始字符有效性 */
    ucApnCharacter = pucApn[0];
    if (!CMMCA_IS_VALID_APN_FIRST_AND_LAST_CHAR(ucApnCharacter))
    {
        CMMCA_ERROR_LOG("CMMCA_CheckApnFormat: Invalid begin character in APN.");
        return VOS_ERR;
    }

    /* 检查APN末尾字符有效性 */
    ucApnCharacter = pucApn[usApnLen - 1];
    if (!CMMCA_IS_VALID_APN_FIRST_AND_LAST_CHAR(ucApnCharacter))
    {
        CMMCA_ERROR_LOG("CMMCA_CheckApnFormat: Invalid end character in APN.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParseSetPdnTabReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_SET_PDN_TAB_REQ_STRU *pstSetPdnTabReq
)
{
    /* CMMC码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> RatMmcAPI_RAT_PDNTable_Setup_ReqT:
        typedef struct
        {
           MmcRatIdT                    RatId;
           RatMmcPDNTableSetupReqT      pdnTable;
        } RatMmcAPI_RAT_PDNTable_Setup_ReqT;

        typedef PACKED struct
        {
           MmcPDNTableT                PdnTable[MAX_PDN_NUM];
        } RatMmcPDNTableSetupReqT;

        typedef PACKED struct
        {
           uint8                       PDNid;
           MmcIPAddrTypeT              AddressType;
           uint8                       APNname[MAX_APN_LEN];
        } MmcPDNTableT;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        PDNid          uint8          This value is defined by carrier. In the example of VzW: 1 = IMS, 2 = Internet, 3 = Admin, 4 = APP
        AddressType    uint8          IP Address type proposed by UE for a particular PDN.1 = IPv4, 2 = IPv6, 3 = IPv4 and IPv6
        APNname        uint8[101]     APN proposed by UE
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;
    CMMCA_PDN_INFO_STRU                *pstPdnInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucPdnInfoIndex;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_SET_PDN_TAB_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_PDN_TABLE_SETUP_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff))
    {
        CMMCA_ERROR1_LOG("CMMCA_ParseSetPdnTabReq", *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));
    }

    CMMCA_Debug_PrintRatId(ID_CMMCA_MMC_RAT_PDN_TABLE_SETUP_REQ,
                           *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    pstSetPdnTabReq->ulTotalNum = 0;

    for (ucPdnInfoIndex = 0; ucPdnInfoIndex < CMMCA_SUPPORTED_PDN_NUM_MAX; ucPdnInfoIndex++)
    {
        pstPdnInfo = &(pstSetPdnTabReq->astPdnInfo[ucPdnInfoIndex]);

        /* 解析出PdnId */
        pstPdnInfo->ucPdnId = *pucMsgBuff;
        pucMsgBuff += sizeof(VOS_UINT8);

        CMMCA_Debug_PrintPdnId(pstPdnInfo->ucPdnId);

        /* 检查PdnId值是否有效，如果无效告警 */
        if (!CMMCA_IS_VALID_PDN_ID(pstPdnInfo->ucPdnId))
        {
            CMMCA_WARNING_LOG("CMMCA_ParseSetPdnTabReq, Pdn Id Is invalid");
            break;
        }

        /* 解析出PdnType */
        pstPdnInfo->enPdnType = *((CMMCA_IP_TYPE_ENUM_UINT8 *)pucMsgBuff);
        pucMsgBuff += sizeof(CMMCA_IP_TYPE_ENUM_UINT8);

        CMMCA_Debug_PrintPdnType(pstPdnInfo->enPdnType);

        /* 检查PdnType是否合法, 如果是删除PdnId上下文信息，PdnType其值为0 */
        if (pstPdnInfo->enPdnType >= CMMCA_IP_TYPE_BUTT)
        {
            return VOS_ERR;
        }

        /* 解析出Apn长度 */
        pstPdnInfo->ucApnLen = (VOS_UINT8)(VOS_StrLen((VOS_CHAR*)pucMsgBuff));

        CMMCA_Debug_PrintApn(pucMsgBuff, pstPdnInfo->ucApnLen);

        /* Apn合法性进行检查 */
        if(0 != pstPdnInfo->ucApnLen)
        {
            if (VOS_OK != CMMCA_CheckApnFormat(pucMsgBuff, pstPdnInfo->ucApnLen))
            {
                return VOS_ERR;
            }

            /* 解析出Apn字符串 */
            VOS_MemCpy(pstPdnInfo->aucApn, pucMsgBuff, pstPdnInfo->ucApnLen);
        }

        pucMsgBuff += (CMMCA_SET_PDN_TAB_REQ_APN_MAX_LEN * sizeof(VOS_UINT8));

        pstSetPdnTabReq->ulTotalNum++;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackSetPdnTabRsp(
    CMMCA_RAT_MMC_SET_PDN_TAB_CNF_STRU *pstSetPdnTabCnf,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* 打包码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> MmcRatAPI_RAT_APDNTable_Setup_RspT:
        typedef PACKED struct
        {
           MmcRatIdT                RatId;
           MmcPDNTableSetupResultT  status;
        } MmcRatAPI_RAT_APDNTable_Setup_RspT;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        status    uint8          Value is 0 = PDNTable_Setup_Succ or 1 = PDNTable_Setup_Fail
    */

    /* 输入参数由调用者保证正确性 */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_PDN_TABLE_SETUP_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = CMMCA_MMC_RAT_ID_EUTRAN;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置Pdn Tab的Rsp信息 */
    *pucMsgBuff = pstSetPdnTabCnf->enRslt;
    pucMsgBuff += sizeof(CMMCA_RAT_MMC_PDN_TAB_SET_RESULT_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParseDefaultBearConnReq(
    VOS_UINT16                                      usParamBlklength,
    VOS_UINT8                                      *pucParamBlk,
    CMMCA_MMC_RAT_DEFAULT_BEAR_CONN_REQ_STRU       *pstDefaultBearConnReq
)
{
    /* CMMC码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> RatMmcPktDefaultBearerReqT:
        typedef PACKED struct
        {
          MmcRatIdT RatId;
          uint8    PdnId;
          uint8    PdnType;
          uint8    AttachType;
          uint32   PDN_Address[5];
          uint32   IP4DefRouterAddr;
          uint8    PDPFlag;
          uint8    APNLen;
          uint8    APN[MAX_APN_LEN];
#ifdef SPRINT_EXTENSIONS
          uint8    AuthType;
          uint8    UserId[HLP_MAX_USRID_LEN+1];
          uint8    pwdLen;
          uint8    PassWord[HLP_MAX_PSWD_LEN];
          uint8    ModuleName[HLP_MAX_USRID_LEN+1];
#endif
        } RatMmcPktDefaultBearerReqT;

        拨号请求只需要用以下参数:
        Parameter Name Parameter Type Parameter Description
        PDNid          uint8          This value is defined by carrier. In the example of VzW: 1 = IMS, 2 = Internet, 3 = Admin, 4 = APP
        PDNType        uint8          IP Address type proposed by UE for a particular PDN.1 = IPv4, 2 = IPv6, 3 = IPv4 and IPv6
        AttachType     uint8          1 = initial attach, 3 = handover attach
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_DEFAULT_BEAR_CONN_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_DEFAULT_BEARER_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff))
    {
        CMMCA_ERROR1_LOG("CMMCA_ParseDefaultBearConnReq:", *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));
    }

    CMMCA_Debug_PrintRatId(ID_CMMCA_MMC_RAT_DEFAULT_BEARER_REQ,
                           *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 解析出PdnId */
    pstDefaultBearConnReq->ucPdnId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    CMMCA_Debug_PrintPdnId(pstDefaultBearConnReq->ucPdnId);

    /* 检查PdnId值是否有效 */
    if (!CMMCA_IS_VALID_PDN_ID(pstDefaultBearConnReq->ucPdnId))
    {
        return VOS_ERR;
    }

    /* 解析出PdnType */
    pstDefaultBearConnReq->enPdnType = *((CMMCA_IP_TYPE_ENUM_UINT8 *)pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_IP_TYPE_ENUM_UINT8);

    CMMCA_Debug_PrintPdnType(pstDefaultBearConnReq->enPdnType);

    /* 检查PdnType是否合法 */
    if (!CMMCA_IS_VALID_PDN_TYPE(pstDefaultBearConnReq->enPdnType))
    {
        return VOS_ERR;
    }

    /* 解析出RequsetType */
    pstDefaultBearConnReq->enRequestType = *((CMMCA_PDN_REQUEST_TYPE_ENUM_UINT8 *)pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_PDN_REQUEST_TYPE_ENUM_UINT8);

    /* 检查PdnRequestType是否合法 */
    if (!CMMCA_IS_VALID_PDN_REQUEST_TYPE(pstDefaultBearConnReq->enRequestType))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackDefaultBearConnRsp(
    CMMCA_RAT_MMC_DEFAULT_BEAR_CONN_CNF_STRU               *pstDefaultBearConnCnf,
    VOS_UINT16                                              usParamBlklength,
    VOS_UINT8                                              *pucParamBlk
)
{
    /* 打包码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> MmcRatPktDefaultBearerRsp_T:
        typedef PACKED struct
        {
          MmcRatIdT      RatId;
          PDNidT         PdnId;
          BearerIdT      BearerId;
          AppOptStatusT  Status;
          IPAddrTypeT    AddrType;
          uint32         LocalIPAddr[5];
          uint8          Pcscf6Num;
          uint32         PCSCF6Addr[MAX_SUPPORTED_ADDR_NUM][4];
          uint8          DNS6Num;
          uint32         DNS6Addr[MAX_SUPPORTED_ADDR_NUM][4];
          uint8          Pcscf4Num;
          uint32         PCSCF4Addr[MAX_SUPPORTED_ADDR_NUM];
          uint8          DNS4Num;
          uint32         DNS4Addr[MAX_SUPPORTED_ADDR_NUM];
          uint32         RouteAddr;
          uint8          IP6Interfaceid[8];
          uint8          SelBearerCtrlMode;
          PdnCfgErrorCodeT  ErrCode;
        } MmcRatPktDefaultBearerRsp_T;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        PDNid          uint8          This value is the PDNid for underlined PDN of which we report status
        BearerId       uint8          It is drawn from 0xf0|PDNid. It can be used as aquick reference for flow number
                                      of the IP packet at reverse link for default bearer
        Status         uint8          0 = success,1 = fail,2 = duplicate,3 = re-negotiate
        AddrType       uint8          Validate type of IP address obtained from network
                                      1 = IPv4 only,2 = IPv6 only,3 = IPv4 and IPv6
        LocalIPAddr    uint32[5]      Full 16 bytes representation of UE’s IPv6 address for underlined PDN
                                      And Full 4 bytes representation of UE’s IPv4 address for underlined PDN
        Pcscf6Num      uint8          Number of valid P-CSCF IPv6 addresses up to 4 of them in PCSCFAddrV6
        PCSCF6Addr     uint32[4][4]   Full 16 bytes representation of P-CSCF’s IPv6 address for underlined PDN
        DNS6Num        uint8          Number of valid DNS IPv4 addresses up to 4 of them in DNSAddrV6
        DNS6Addr       uint32[4][4]   Full 16 bytes representation of DNS’s IPv6 address for underlined PDN
        Pcscf4Num      uint8          Number of valid P-CSCF IPv4 addresses up to 4 of them in PCSCFAddrV4
        PCSCF4Addr     uint32[4]      Full 4 bytes representation of P-CSCF’s IPv4 address for underlined PDN
        DNS4Num        uint8          Number of valid DNS IPv4 addresses up to 4 of them in DNSAddrV4
        DNS4Addr       uint32[4]      Full 4 bytes representation of DNS’s IPv4 address for underlined PDN
        RouteAddr      uint32         This is the default IPv4 router address or aka remote IP address
        IP6Interfaceid uint8[8]       8-byte address representation of Interface ID for IPv6. In conjunction of IPv6 pre-fix address
                                      obtained from Agent Advertisement message will form a global routable full 16-byte IPv6 address
        SelBearerCrtlMode uint8       Selected Bearer Control Mode 1 = UE only 2 = NW and UE (default)s
        ErrCode        uint8          Error codes
    */

    /* 输入参数由调用者保证正确性 */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_DEFAULTBEARER_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = CMMCA_MMC_RAT_ID_EUTRAN;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包PdnId信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucPdnId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包BearerId信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucBearerId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包Status激活结果信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->enRslt;
    pucMsgBuff += sizeof(CMMCA_RAT_MMC_BEAR_CONN_RESULT_ENUM_UINT8);

    /* 打包AddressType信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->enPdnType;
    pucMsgBuff += sizeof(CMMCA_IP_TYPE_ENUM_UINT8);

    /* 打包IPv6地址信息 */
    VOS_MemCpy(pucMsgBuff, pstDefaultBearConnCnf->aucIpv6Addr, CMMCA_IPV6_ADDR_LEN);
    pucMsgBuff += CMMCA_IPV6_ADDR_LEN;

    /* 打包IPv4地址信息 */
    VOS_MemCpy(pucMsgBuff, pstDefaultBearConnCnf->aucIpv4Addr, CMMCA_IPV4_ADDR_LEN);
    pucMsgBuff += CMMCA_IPV4_ADDR_LEN;

    /* 打包PCSCFNumV6信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucPcscf6Num;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包PCSCFAddrV6地址信息 */
    VOS_MemCpy(pucMsgBuff,
               pstDefaultBearConnCnf->aucPcscf6Addr,
               CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV6_ADDR_LEN);
    pucMsgBuff += (CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV6_ADDR_LEN);

    /* 打包DNSNum16信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucDns6Num;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包DNSAddrV6地址信息 */
    VOS_MemCpy(pucMsgBuff,
               pstDefaultBearConnCnf->aucDns6Addr,
               CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV6_ADDR_LEN);
    pucMsgBuff += (CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV6_ADDR_LEN);

    /* 打包PCSCFNumV4信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucPcscf4Num;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包PCSCFAddrV4地址信息 */
    VOS_MemCpy(pucMsgBuff,
               pstDefaultBearConnCnf->aucPcscf4Addr,
               CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV4_ADDR_LEN);
    pucMsgBuff += (CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV4_ADDR_LEN);

    /* 打包DNSNum4信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->ucDns4Num;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包DNSAddrV4地址信息 */
    VOS_MemCpy(pucMsgBuff,
               pstDefaultBearConnCnf->aucDns4Addr,
               CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV4_ADDR_LEN);
    pucMsgBuff += (CMMCA_SUPPORTED_ADDR_NUM_MAX * CMMCA_IPV4_ADDR_LEN);

    /* 打包RouteAddress地址信息 */
    VOS_MemCpy(pucMsgBuff, pstDefaultBearConnCnf->aucRouteAddr, CMMCA_IPV4_ADDR_LEN);
    pucMsgBuff += CMMCA_IPV4_ADDR_LEN;

    /* 打包InterfaceIdV6地址信息 */
    VOS_MemCpy(pucMsgBuff,
               pstDefaultBearConnCnf->aucIp6InterfaceId,
               CMMCA_IPV6_INTERFACE_ID_BYTE_LEN);
    pucMsgBuff += CMMCA_IPV6_INTERFACE_ID_BYTE_LEN;

    /* 打包SelBearerCrtlMode信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->enSelBearerCtrlMode;
    pucMsgBuff += sizeof(CMMCA_PDN_SEL_BEARER_CTRL_MODE_ENUM_UINT8);

    /* 打包Error code信息 */
    *pucMsgBuff = pstDefaultBearConnCnf->enErrCode;
     pucMsgBuff += sizeof(CMMCA_RAT_MMC_PKT_ERRCODE_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_ParseBearDiscReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_BEAR_DISC_REQ_STRU   *pstBearDiscReq
)
{
    /* CMMC码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> RatMmcPktBearerDiscntReqT:
        typedef PACKED struct
        {
           MmcRatIdT       RatId;
           uint8           PdnId;
           uint8           BearerId;
           uint8           DefaultBearer;
        } RatMmcPktBearerDiscntReqT;

        默认承载去激活需要用以下参数:
        Parameter Name Parameter Type Parameter Description
        PdnId          uint8          This value is the PDN id for underlined PDN
        BearerId       uint8          EPS Bearer Identity
        DefaultBearer  uint8          Default = 1, Dedicated = 0
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_BEAR_DISC_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_BEARER_DISCONNECT_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff))
    {
        CMMCA_ERROR1_LOG("CMMCA_ParseBearDiscReq:", *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));
    }

    CMMCA_Debug_PrintRatId(ID_CMMCA_MMC_RAT_BEARER_DISCONNECT_REQ,
                           *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 解析出PdnId */
    pstBearDiscReq->ucPdnId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    CMMCA_Debug_PrintPdnId(pstBearDiscReq->ucPdnId);

    /* 检查PdnId值是否有效 */
    if (!CMMCA_IS_VALID_PDN_ID(pstBearDiscReq->ucPdnId))
    {
        return VOS_ERR;
    }

    /* 解析出BearerId */
    pstBearDiscReq->ucBearerId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 解析出默认承载标志 */
    pstBearDiscReq->ucIsDefaultBearer = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackBearDiscRsp(
    CMMCA_RAT_MMC_BEAR_DISC_CNF_STRU   *pstBearDiscCnf,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* 打包码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> MmcPktRatBearerDisconnectRspMsg_T:
        typedef PACKED struct
        {
           MmcRatIdT                RatId;
           PDNidT                   PdnId;
           BearerIdT                BearerId;
           bool                     Status;
        } MmcPktRatBearerDisconnectRspMsg_T;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        PDNid          uint8          This value is the PDN id for underlined PDN.
        BearerId       uint8          EPS Bearer Identity. Use 0xf0|PDNid for defaut bearer.
        Status         bool           1 = Success, 0 = Failed, not legitimate
    */

    /* 输入参数由调用者保证正确性 */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_BEARER_DISCONNECT_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = CMMCA_MMC_RAT_ID_EUTRAN;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包去激活的PdnId */
    *pucMsgBuff = pstBearDiscCnf->ucPdnId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包去激活的BearerId */
    *pucMsgBuff = pstBearDiscCnf->ucBearerId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包去激活的结果信息 */
    *pucMsgBuff = pstBearDiscCnf->enRslt;
    pucMsgBuff += sizeof(CMMCA_RAT_MMC_RESULT_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackBearDiscInd(
    CMMCA_RAT_MMC_BEAR_DISC_IND_STRU   *pstBearDiscInd,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* 打包码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> MmcPktRatBearerDisconnectIndMsg_T:
        typedef PACKED struct
        {
            MmcRatIdT                 RatId;
            PDNidT                    PdnId;
            BearerIdT                 BearerId;
        } MmcPktRatBearerDisconnectIndMsg_T;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        PDNid          uint8          This value is the PDN id for underlined PDN.
        BearerId       uint8          EPS Bearer Identity. Use 0xf0|PDNid for defaut bearer.
    */

    /* 输入参数由调用者保证正确性 */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_BEARER_DISCONNECT_IND, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = CMMCA_MMC_RAT_ID_EUTRAN;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包去激活的PdnId */
    *pucMsgBuff = pstBearDiscInd->ucPdnId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包去激活的BearerId */
    *pucMsgBuff = pstBearDiscInd->ucBearerId;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_ParseSystemChangeRsp(
    VOS_UINT16                                              usParamBlklength,
    VOS_UINT8                                              *pucParamBlk,
    CMMCA_MMC_RAT_IRAT_SYSTEM_CHG_RSP_STRU                 *pstSystemChgInd
)
{
    /* CMMC码流格式描述:
        第一层:
        typedef struct{
            uint16  Cmd;
            uint8   ParamBlk[1];
        } ValMmcMsgT;

        第二层ParamBlk --> RatMmcAPI_RAT_PDNTable_Setup_ReqT:
        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcSystemChangeRsp_Rpc_T SystemChangeRsp;
        } RatMmcAPI_IRAT_System_Change_RspT;

        typedef PACKED struct
        {
           MmcRatStatusT Result;
           MmcIratHOCauseT Cause;
        } MmcSystemChangeRsp_Rpc_T;

        参数说明:
        Parameter Name Parameter Type Parameter Description
        Result         enum           IRAT_SUCCESS=0,IRAT_ACQ_FAIL,IRAT_PS_REG_FAIL,IRAT_PS_SESSION_FAIL,IRAT_FAIL

    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_SYSTEM_CHG_RSP_LEN)
    {
        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff))
    {
        CMMCA_ERROR1_LOG("CMMCA_ParseSystemChangeInd: ", *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));
    }

    CMMCA_Debug_PrintRatId(ID_CMMCA_MMC_RAT_IRAT_SYSTEM_CHG_RSP,
                           *((CMMCA_MMC_RAT_ID_ENUM_UINT8 *)pucMsgBuff));

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 解析出system change result */
    pstSystemChgInd->enResult = *((CMMCA_IRAT_SYSTEM_CHG_RESULT_ENUM_UINT8 *)pucMsgBuff);
    if (pstSystemChgInd->enResult >= CMMCA_IRAT_SYSTEM_CHG_RESULT_BUTT)
    {
        return VOS_ERR;
    }

    pucMsgBuff += sizeof(CMMCA_IRAT_SYSTEM_CHG_RESULT_ENUM_UINT8);

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParseModeReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_MODE_REQ_STRU        *pstModeReq
)
{
    /* MODE REQ 需要用以下参数:
        Parameter Name Parameter Type Parameter              Description
        enRatId          CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
        enMode           CMMCA_RAT_MMC_MODE_ENUM_UINT8
    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_MODE_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_RAT_MODE_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);


    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseModeReq: invalid RatId!");
    }
    pstModeReq->enRatId = *pucMsgBuff;

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);
    pstModeReq->enMode = *pucMsgBuff;

    if (CMMCA_RAT_MODE_LTE != pstModeReq->enMode)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseModeReq: invalid enMode!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackModeRsp(
    CMMCA_RAT_MMC_RAT_MODE_RSP_STRU    *pstModeRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* mode rsp
        Parameter Name Parameter Type Parameter Description
        enRatId    CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
        ucResult   VOS_UINT8
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_RAT_MODE_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstModeRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包ucResult信息 */
    *pucMsgBuff      = pstModeRsp->enResult;
    pucMsgBuff += sizeof(CMMCA_MMC_MODE_RSP_RESULT_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParsePowerUpReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_POWERUP_REQ_STRU     *pstPowerUpReq
)
{
    /* POWER UP REQ 需要用以下参数:
        Parameter Name Parameter Type Parameter              Description
        enRatId          CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_POWERUP_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_POWERUP_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParsePowerUpReq: invalid RatId!");
    }
    pstPowerUpReq->enRatId = *pucMsgBuff;

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackPowerUpRsp(
    CMMCA_RAT_MMC_POWERUP_RSP_STRU     *pstPowerUpRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* 开机响应
        Parameter Name Parameter Type Parameter Description
        enRatId    CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
        enResult   CMMCA_MMC_RAT_POWERUP_RESULT_RSP_UINT8
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_POWERUP_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstPowerUpRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 解析enResult */
    *pucMsgBuff = pstPowerUpRsp->enResult;
    pucMsgBuff += sizeof(CMMCA_RAT_POWERUP_RSP_RESULT_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParsePowerDownReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_POWERDOWN_REQ_STRU   *pstPowerDownReq
)
{
    /* POWER DOWN REQ 需要用以下参数:
        Parameter Name Parameter Type Parameter              Description
        enRatId          CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_POWERDOWN_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_POWERDOWN_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParsePowerDownReq: invalid RatId!");
    }
    pstPowerDownReq->enRatId = *pucMsgBuff;

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackPowerDownRsp(
    CMMCA_RAT_MMC_POWERDOWN_RSP_STRU   *pstPowerDownRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /* 开机响应
        Parameter Name Parameter Type Parameter Description
        enRatId    CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_POWERDOWN_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstPowerDownRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_ParseAcqReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_ACQ_REQ_STRU         *pstAcqReq
)
{
    /* ACQ REQ 需要用以下参数:
        Parameter Name Parameter Type Parameter              Description
        enRatId          CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_ACQ_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_ACQ_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseAcqReq: RatId is not CMMCA_MMC_RAT_ID_EUTRAN!");
    }
    pstAcqReq->enRatId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    pstAcqReq->enCause = *pucMsgBuff;

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackAcqRsp(
    CMMCA_RAT_MMC_ACQ_RSP_STRU         *pstAcqRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*
        typedef enum
        {
           RAT_ACQ_ACQUIRED = 0,
           RAT_ACQ_ACQFAILED,
           RAT_ACQ_CMD_REJ
        } MmcRatAcqResultT;

        typedef PACKED struct
        {
           uint16 Earfcn;
           MmcGmssPriorityClassT PriorityClass;
           uint32 PlmnId;
        } MmcRatSysInfo_Rpc_T;

        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcRatAcqResultT Result;
           MmcRatSysInfo_Rpc_T RatSysInfo;
        } MmcRatAPI_RAT_Acquisition_RspT;
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_ACQ_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstAcqRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置enResult信息 */
    *pucMsgBuff      = pstAcqRsp->enResult;
    pucMsgBuff += sizeof(CMMCA_RAT_ACQ_RESULT_ENUM_UINT8);

    /* 打包设置stRatSysInfo中enPriorityClass信息信息 */
    *pucMsgBuff      = pstAcqRsp->stRatSysInfo.enPriorityClass;
    pucMsgBuff += sizeof(CMMCA_RAT_GMSS_PRIORITY_CLASS_ENUM_UINT8);

    /* 打包设置stRatSysInfo中mcc信息信息 */
    CMMCA_ConvertNasMccToTwoBcdMccOctetStr(pstAcqRsp->stRatSysInfo.ulMcc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包设置stRatSysInfo中mnc信息信息 */
    CMMCA_ConvertNasMncToTwoBcdMncOctetStr(pstAcqRsp->stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包设置stRatSysInfo中ucRatInfoValid信息信息 */
    *pucMsgBuff      = pstAcqRsp->stRatSysInfo.ucRatInfoValid;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包设置stRatSysInfo中ucRatInfoValid信息信息 */
    /* 打包RatInfo信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstAcqRsp->stRatSysInfo.stEutran.usEarfcn, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(pstAcqRsp->stRatSysInfo.ulMcc, pstAcqRsp->stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT32);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParseNoServiceReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_NO_SERVICE_REQ_STRU  *pstNoServReq
)
{
    /* NoServiceReq需要用以下参数:
        Parameter Name Parameter Type Parameter              Description
        enRatId          CMMCA_MMC_RAT_ID_ENUM_UINT8
        enCause          CMMCA_MMC_NOSVC_CAUSE_ENUM_UINT8
    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_NO_SERVICE_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_NO_SERVICE_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseNoServiceReq: invalid RatId!");
    }
    pstNoServReq->enRatId = *pucMsgBuff;

    /* 解析enCause */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);
    pstNoServReq->enCause = *pucMsgBuff;

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackNoServiceRsp(
    CMMCA_RAT_MMC_NO_SERVICE_RSP_STRU  *pstNoServRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*
       typedef PACKED struct
       {
          MmcRatIdT RatId;
          uint8 Status;
       } MmcRatAPI_RAT_No_Service_RspT;
    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_NO_SERVICE_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstNoServRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包ucStatus信息 */
    *pucMsgBuff = pstNoServRsp->ucStatus;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackMccSearchRsp(
    CMMCA_RAT_MMC_MCC_SEARCH_RSP_STRU  *pstMccSearchRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*
        typedef PACKED struct
        {
           MmcRatIdT RatId;
           uint16 Mcc;
           uint16 Mnc;
           union {
             MmcCdma1XRatInfo Cdma1X;
             MmcCdmaDORatInfo CdmaDO;
             Mmc3GppRatInfo Geran;
             Mmc3GppRatInfo Utran;
             Mmc3GppRatInfo Eutran;
           } RatInfo;
        } MmcRatAPI_RAT_MCC_Search_RspT;

    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_MCC_SEARCH_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstMccSearchRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包mcc信息 */
    CMMCA_ConvertNasMccToTwoBcdMccOctetStr(pstMccSearchRsp->ulMcc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包mnc信息 */
    CMMCA_ConvertNasMncToTwoBcdMncOctetStr(pstMccSearchRsp->ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包RatInfoValid消息 */
    *pucMsgBuff = pstMccSearchRsp->ucRatInfoValid;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包RatInfo信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstMccSearchRsp->stEutran.usEarfcn, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(pstMccSearchRsp->ulMcc, pstMccSearchRsp->ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT32);

    /* 防止内存越界 */
    if ((pucMsgBuff- pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_ParsePsRegReq(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_MMC_RAT_PS_REG_REQ_STRU      *pstPsRegReq
)
{
    /*
        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcGmssPriorityClassT PriorityClass;
        } RatMmcAPI_RAT_PS_Reg_ReqT;

    */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_PS_REG_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_PS_REG_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_ERROR_LOG("CMMCA_ParsePsRegReq: RatId is not CMMCA_MMC_RAT_ID_EUTRAN!");
    }
    pstPsRegReq->enRatId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 解析enPriorityClass */
    pstPsRegReq->enPriorityClass = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 解析attach type */
    pstPsRegReq->ucAttachType = *pucMsgBuff;

    return VOS_OK;
}



VOS_UINT32 CMMCA_PackPsRegRsp(
    CMMCA_RAT_MMC_PS_REG_RSP_STRU      *pstPsRegRsp,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*

        typedef PACKED struct
        {
           uint16 Earfcn;
           MmcGmssPriorityClassT PriorityClass;
           uint32 PlmnId;
        } MmcRatSysInfo_Rpc_T;

        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcRatRegResultT Result;
           MmcRatSysInfo_Rpc_T RatSysInfo;
        } MmcRatAPI_RAT_PS_Reg_RspT;


    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_PS_REG_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstPsRegRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置pstPsRegRsp的Rslt信息 */
    *pucMsgBuff      = pstPsRegRsp->enResult;
    pucMsgBuff += sizeof(CMMCA_RAT_ACQ_RESULT_ENUM_UINT8);

    /* 打包设置stRatSysInfo中enPriorityClass信息信息 */
    *pucMsgBuff      = pstPsRegRsp->stRatSysInfo.enPriorityClass;
    pucMsgBuff += sizeof(CMMCA_RAT_GMSS_PRIORITY_CLASS_ENUM_UINT8);

    /* 打包设置stRatSysInfo中mcc信息信息 */
    CMMCA_ConvertNasMccToTwoBcdMccOctetStr(pstPsRegRsp->stRatSysInfo.ulMcc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包设置stRatSysInfo中mnc信息信息 */
    CMMCA_ConvertNasMncToTwoBcdMncOctetStr(pstPsRegRsp->stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包设置stRatSysInfo中ucRatInfoValid信息信息 */
    *pucMsgBuff      = pstPsRegRsp->stRatSysInfo.ucRatInfoValid;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包设置stRatSysInfo中ucRatInfoValid信息信息 */
    /* 打包RatInfo信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstPsRegRsp->stRatSysInfo.stEutran.usEarfcn, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(pstPsRegRsp->stRatSysInfo.ulMcc, pstPsRegRsp->stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT32);

    /* 防止内存越界 */
    if ((pucMsgBuff- pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_ParseCellInfoPsRegReq(
    VOS_UINT16                                              usParamBlklength,
    VOS_UINT8                                              *pucParamBlk,
    CMMCA_MMC_RAT_CELL_INFO_PS_REG_REQ_STRU                *pstCellInfoPsRegReq
)
{
    /*
        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcSystemChangeReq_Rpc_T SystemChangeReq;
        } RatMmcAPI_RAT_Cell_Info_PS_Reg_ReqT;


    */

    VOS_UINT8                                              *pucMsgBuff          = VOS_NULL_PTR;
    CMMCA_MMC_CARRIER_INFO_EUTRAN_RPC_STRU                 *pstcarrierInfoParam = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    VOS_UINT32                                              ulMncValidFlg;
    VOS_UINT8                                               ucNumPlmn;
    
    /* 输入参数由调用者保证正确性 */

    /* 对总的码流长度length进行检查 */
    if (usParamBlklength < CMMCA_CMD_RAT_CELL_INFO_PS_REG_REQ_LEN)
    {
        CMMCA_Debug_PrintDataIndLenErr(ID_CMMCA_MMC_RAT_CELL_INFO_PS_REG_REQ, usParamBlklength);

        return VOS_ERR;
    }

    pstcarrierInfoParam = &(pstCellInfoPsRegReq->stSystemChangeReq.stTargetSystemCarrierInfo.stCarrierInfoEutran);
    pucMsgBuff = pucParamBlk;

    /* CmdId由调用函数保证正确性 */
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 判断RatId是否是LTE */
    if (CMMCA_MMC_RAT_ID_EUTRAN != *pucMsgBuff)
    {
        CMMCA_WARNING_LOG("CMMCA_ParseCellInfoPsRegReq: RatId is not CMMCA_MMC_RAT_ID_EUTRAN!");
    }
    pstCellInfoPsRegReq->enRatId = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 解析stSystemChangeReq.enCause */
    pstCellInfoPsRegReq->stSystemChangeReq.enCause= *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 解析stSystemChangeReq.stTargetSystemCarrierInfo.usTargetSystemType */
    pstCellInfoPsRegReq->stSystemChangeReq.stTargetSystemCarrierInfo.enTargetSystemType = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    if (CMMCA_RAT_SYSTEM_TYPE_EUTRAN != pstCellInfoPsRegReq->stSystemChangeReq.stTargetSystemCarrierInfo.enTargetSystemType)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseCellInfoPsRegReq: invalid TragetSystemType!");
    }

    pstcarrierInfoParam->usEarfcn = CMMCA_ConvertDoubleOctetStrToU16(pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    pstcarrierInfoParam->usNumCell = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 判断usNumCell是否越界 */
    if (pstcarrierInfoParam->usNumCell > CMMCA_MMC_MAX_CELL_NUM)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseCellInfoPsRegReq: invalid usNumCell!");
        return VOS_ERR;
    }

    for ( i = 0; i < CMMCA_MMC_MAX_CELL_NUM; i++ )
    {
        if (i < pstcarrierInfoParam->usNumCell)
        {
            pstcarrierInfoParam->ausCellId[i] = CMMCA_ConvertDoubleOctetStrToU16(pucMsgBuff);
        }
        pucMsgBuff += sizeof(VOS_UINT16);
    }

    ucNumPlmn   = *pucMsgBuff;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 判断usNumCell是否越界 */
    if (ucNumPlmn > CMMCA_MMC_MAX_NUM_EUTRA_PLMN)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseCellInfoPsRegReq: invalid ucNumPlmn!");
        return VOS_ERR;
    }
    
    pstcarrierInfoParam->ucNumPlmn = 0;
    
    for ( i = 0; i < ucNumPlmn;  i++)
    {
        CMMCA_ConvertFourBcdMccMncOctetStrToNasPlmn(pucMsgBuff,
                                                 &(pstcarrierInfoParam->aulMcc[pstcarrierInfoParam->ucNumPlmn]),
                                                 &(pstcarrierInfoParam->aulMnc[pstcarrierInfoParam->ucNumPlmn]));
        pucMsgBuff += sizeof(VOS_UINT32);

        ulMncValidFlg = CMMCA_IsNasMncValid(pstcarrierInfoParam->aulMnc[pstcarrierInfoParam->ucNumPlmn]);

        if ((VOS_FALSE == CMMCA_IsNasMccValid(pstcarrierInfoParam->aulMcc[pstcarrierInfoParam->ucNumPlmn]))
         || (VOS_FALSE == ulMncValidFlg))
        {
            continue;
        }
        else
        {
            pstcarrierInfoParam->ucNumPlmn++;
        }
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackCellInfoPsRegRsp(
    CMMCA_RAT_MMC_CELL_INFO_PS_REG_RSP_STRU                *pstCellInfoPsRegRsp,
    VOS_UINT16                                              usParamBlklength,
    VOS_UINT8                                              *pucParamBlk
)
{
    /*
        typedef enum
        {
          MMC_IRAT_SUCCESS=0,
          MMC_IRAT_ACQ_FAIL,
          MMC_IRAT_PS_REG_FAIL,
          MMC_IRAT_PS_SESSION_FAIL,
          MMC_IRAT_FAIL
        } MmcRatStatusT;

        typedef PACKED struct
        {
           uint16 Earfcn;
           MmcGmssPriorityClassT PriorityClass;
           uint32 PlmnId;
        } MmcRatSysInfo_Rpc_T;

        typedef PACKED struct
        {
           MmcRatStatusT Result;
           MmcRatSysInfo_Rpc_T RatSysInfo;
        } MmcRatCellInfoPsRegRsp_Rpc_T

        typedef PACKED struct
        {
           MmcRatIdT RatId;
           MmcRatCellInfoPsRegRsp_Rpc_T IratRegRsp;
        } MmcRatAPI_RAT_Cell_Info_PS_Reg_RspT;

    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_CELL_INFO_PS_REG_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstCellInfoPsRegRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置pstPsRegRsp的Rslt信息 */
    *pucMsgBuff      = pstCellInfoPsRegRsp->stIratRegRsp.enResult;
    pucMsgBuff += sizeof(CMMCA_RAT_MMC_STATUS_ENUM_UINT8);

    /* 打包设置pstPsRegRsp的Target System信息 */
    *pucMsgBuff      = pstCellInfoPsRegRsp->stIratRegRsp.enTargetSystemType;
    pucMsgBuff += sizeof(CMMCA_RAT_SYSTEM_TYPE_ENUM_UINT8);

    /* 打包设置pstPsRegRsp的enPriorityClass信息 */
    *pucMsgBuff      = pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.enPriorityClass;
    pucMsgBuff += sizeof(CMMCA_RAT_GMSS_PRIORITY_CLASS_ENUM_UINT8);

    /* 打包mcc信息 */
    CMMCA_ConvertNasMccToTwoBcdMccOctetStr(pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.ulMcc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包mnc信息 */
    CMMCA_ConvertNasMncToTwoBcdMncOctetStr(pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包设置pstPsRegRsp的valid信息 */
    *pucMsgBuff      = pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.ucRatInfoValid;
    pucMsgBuff += sizeof(VOS_UINT8);

    /* 打包RatInfo信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.stEutran.usEarfcn, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.ulMcc, pstCellInfoPsRegRsp->stIratRegRsp.stRatSysInfo.ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT32);

    /* 防止内存越界 */
    if ((pucMsgBuff- pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackNtwStateChangeInd(
    CMMCA_RAT_MMC_NTW_ST_CHG_IND_STRU  *pstNtwStChgInd,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*
     typedef PACKED struct
     {
         MmcRatIdT RatId;
         MmcRatCpStT Status;
         MmcRatNotifyCauseT Cause;
         MmcGmssPriorityClassT SysPri;
         MmcPsTypeT PsType;
         UINT16 Mcc;
         UINT16 Mnc;
         PACKED union {
         MmcCdma1XRatInfoT Cdma1X;
         MmcCdmaDORatInfoT CdmaDO;
         Mmc3GppRatInfoT Geran;
         Mmc3GppRatInfoT Utran;
         Mmc3GppRatInfoT Eutran;
        } RatInfo;
     } MmcRatAPI_RAT_Ntw_State_Change_IndT;


    */
    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_NWT_ST_CHG_IND, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff      = pstNtwStChgInd->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置Cause信息 */
    *pucMsgBuff      = pstNtwStChgInd->enStatus;
    pucMsgBuff += sizeof(CMMCA_RAT_CPST_ENUM_UINT8);

    /* 打包status信息 */
    *pucMsgBuff      = pstNtwStChgInd->enCause;
    pucMsgBuff += sizeof(CMMCA_RAT_NOTIFY_CAUSE_ENUM_UINT8);

    /* 打包SysPri信息 */
    *pucMsgBuff      = pstNtwStChgInd->enSysPri;
    pucMsgBuff += sizeof(CMMCA_RAT_GMSS_PRIORITY_CLASS_ENUM_UINT8);

    /* 打包status信息 */
    *pucMsgBuff      = pstNtwStChgInd->enPsType;
    pucMsgBuff += sizeof(CMMCA_RAT_PS_TYPE_ENUM_UINT8);

    /* 打包mcc信息 */
    CMMCA_ConvertNasMccToTwoBcdMccOctetStr(pstNtwStChgInd->ulMcc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包mnc信息 */
    CMMCA_ConvertNasMncToTwoBcdMncOctetStr(pstNtwStChgInd->ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    /* 打包RatInfo信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstNtwStChgInd->stEutran.usEarfcn, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    CMMCA_ConvertNasPlmnToBcdPlmnOctetStr(pstNtwStChgInd->ulMcc, pstNtwStChgInd->ulMnc, pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT32);

    /* 防止内存越界 */
    if ((pucMsgBuff- pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CMMCA_PackCmdRejInd(
    CMMCA_MMC_CMD_REJ_IND_STRU         *pstCmdRejInd,
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk
)
{
    /*
     enRatId    CMMCA_RAT_MMC_RAT_ID_ENUM_UINT8
     typedef uint16 MmcRatApiIdT;
     typedef enum
     {
        RAT_NOT_CACHABLE = 0,
        RAT_WRONG_MODE,
        RAT_WRONG_RESUME,
        RAT_WRONG_SUSPEND,
        RAT_WRONG_PWR_DOWN,
        RAT_REJ_NO_REASON
     }MmcRatCmdRejReasonT;

     typedef PACKED struct
     {
        MmcRatIdT RatId;
        MmcRatApiIdT ApiID;
        MmcRatCmdRejReasonT Cause;
     } MmcRatAPI_RAT_Cmd_Reject_IndT;
     */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 输入参数由调用者保证正确性 */

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_CMD_REJ_IND, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstCmdRejInd->enSourceRat;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 打包设置enApiID信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(pstCmdRejInd->enApiID, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包设置Cause信息 */
    *pucMsgBuff = pstCmdRejInd->enCause;
    pucMsgBuff += sizeof(CMMCA_RAT_CMD_REJ_REASON_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff- pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 CMMCA_PackPktDataCmdRsp(
    CMMCA_RAT_MMC_PKT_DATA_CMD_RSP_STRU                    *pstDataCmdRsp,
    VOS_UINT16                                              usParamBlklength,
    VOS_UINT8                                              *pucParamBlk
)
{

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;

    pucMsgBuff = pucParamBlk;

    /* 打包CmdId信息 */
    CMMCA_ConvertU16ToDoubleOctetStr(ID_CMMCA_RAT_MMC_DATACONNECTION_CMD_RSP, pucMsgBuff);
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    /* 打包RatId信息 */
    *pucMsgBuff = pstDataCmdRsp->enRatId;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    /* 解析CmdRsp信息 */
    *pucMsgBuff = pstDataCmdRsp->enCmdRsp;
    pucMsgBuff += sizeof(CMMCA_IRAT_DATA_CONNECT_CMD_RSP_ENUM_UINT8);

    /* 防止内存越界 */
    if ((pucMsgBuff - pucParamBlk) > usParamBlklength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}




VOS_VOID CMMCA_ParseOtherRatInfo(
    VOS_UINT8                         **ppMsgBuff,
    CMMCA_OTHER_RAT_INFO_STRU          *pstOtherRatInfo
)
{
    VOS_UINT8                          *pTmpBuff = VOS_NULL_PTR;
    VOS_UINT8                           ucLoop;

    pTmpBuff = *ppMsgBuff;

    pstOtherRatInfo->usEarfcn       = CMMCA_ConvertDoubleOctetStrToU16(pTmpBuff);
    pTmpBuff += sizeof(VOS_UINT16);

    pstOtherRatInfo->ucEarfcnPri    = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    pstOtherRatInfo->ucThreshX      = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    pstOtherRatInfo->usRxLevMinEutra = CMMCA_ConvertDoubleOctetStrToU16(pTmpBuff);
    pTmpBuff += sizeof(VOS_UINT16);

    pstOtherRatInfo->ucRxLevMinEutraOffset = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    pstOtherRatInfo->ucPeMax        = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    pstOtherRatInfo->ucMeasBandWidth = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    pstOtherRatInfo->ucNumPlmn      = *(VOS_UINT8 *)pTmpBuff;
    pTmpBuff += sizeof(VOS_UINT8);

    for (ucLoop = 0; ucLoop < CMMCA_MAX_EUTRA_NEIGHBOR_PLMN_NUM; ucLoop++)
    {
        pstOtherRatInfo->aulPlmnId[ucLoop] = CMMCA_ConvertFourOctetStrToU32(pTmpBuff);

        pTmpBuff += sizeof(VOS_UINT32);
    }

    *ppMsgBuff = pTmpBuff;

    return;
}



VOS_UINT32 CMMCA_ParseOtherRatInfoInd(
    VOS_UINT16                          usParamBlklength,
    VOS_UINT8                          *pucParamBlk,
    CMMCA_OTHER_RAT_INFO_IND_STRU      *pstOtherRatInfoInd
)
{
    /* CMMC码流格式描述:
       第一层:
       typedef struct{
           uint16  Cmd;
           uint8   ParamBlk[1];
       } ValMmcMsgT;

       第二层ParamBlk --> RatMmcAPI_RAT_OtherRAT_Info_IndT:
       typedef PACKED struct
        {
           MmcRatIdT RatId;
           uint16 EarfcnListSize;
           MmcOtherRatInfoT OtherRatInfo[1];
        } RatMmcAPI_RAT_OtherRAT_Info_IndT;

       typedef PACKED struct
        {
          uint16 Earfcn;       // EUTRA channel number
          uint8 EarfcnPri;     // priority of EARFCN of the neighbir EUTRA system. 0xff means not provided
          uint8 ThreshX;       // Min required signal threshold used for selecting an EARFCN, The actual threshold value in dB used to compare against the measured reference signal receive Power(RSRP).
          int16 RxLevMinEutra; // Min ref power level to select EUTRA, which in dBm is equal to the minimum reference signal received power(RSRP) level of a EUTRA cell required for the UE to reselect EUTRA.
          uint8 RxLevMinEutraOffset; // Offset to min ref power level to select EUTRA. 0xff means not provided
          uint8 PeMax; // Max TX power level UE may use when transmitting up-link of EUTRA
          uint8 MeasBandWidth; //  measurement bandwidth Nrb value, Table 7.1-1
          uint8 NumPlmn; // number of PLMN on this EARFCN channel
          uint32 PlmnId[MAX_PLMN_NUM];  //PLMN ID of the neighbor EUTRA system. 0xffffffff means not provided
        } MmcOtherRatInfoT;
       */

    VOS_UINT8                          *pucMsgBuff = VOS_NULL_PTR;
    VOS_UINT16                          usLoop;

    /* 输入参数由调用者保证正确性 */

    pucMsgBuff = pucParamBlk;

    /* CmdId */
    pstOtherRatInfoInd->usCmdId     = CMMCA_ConvertDoubleOctetStrToU16(pucMsgBuff);

    pucMsgBuff += sizeof(CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16);

    pstOtherRatInfoInd->enRatId     = *(CMMCA_MMC_RAT_ID_ENUM_UINT8*)pucMsgBuff;
    pucMsgBuff += sizeof(CMMCA_MMC_RAT_ID_ENUM_UINT8);

    pstOtherRatInfoInd->usEarfcnListSize = CMMCA_ConvertDoubleOctetStrToU16(pucMsgBuff);
    pucMsgBuff += sizeof(VOS_UINT16);

    if (pstOtherRatInfoInd->usEarfcnListSize > CMMCA_MAX_ARFCN_NUM)
    {
        CMMCA_ERROR_LOG("CMMCA_ParseOtherRatInfoInd: usEarfcnListSize too big!");
        return VOS_ERR;
    }

    for (usLoop = 0; usLoop < pstOtherRatInfoInd->usEarfcnListSize; usLoop++)
    {
        CMMCA_ParseOtherRatInfo(&pucMsgBuff, &pstOtherRatInfoInd->astOtherRatInfo[usLoop]);
    }

    return VOS_OK;
}
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif


