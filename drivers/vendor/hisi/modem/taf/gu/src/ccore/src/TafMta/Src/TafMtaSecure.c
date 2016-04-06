

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafMtaSecure.h"
#include "TafMtaMain.h"
#include "ScInterface.h"
#include "SysNvId.h"
#include "TafNvInterface.h"
#include "NVIM_Interface.h"
#include "TafMtaComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MTA_SECURE_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

MTA_AT_RESULT_ENUM_UINT32 TAF_MTA_ConvertScErr(
    SC_ERROR_CODE_ENUM_UINT32           enErrCode
)
{
    switch(enErrCode)
    {
        case SC_ERROR_CODE_NO_ERROR:
            return MTA_AT_RESULT_NO_ERROR;

        case SC_ERROR_CODE_OPEN_FILE_FAIL:
        case SC_ERROR_CODE_READ_FILE_FAIL:
        case SC_ERROR_CODE_WRITE_FILE_FAIL:
        case SC_ERROR_CODE_WRITE_FILE_IMEI_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_FLASH_ERROR;

        case SC_ERROR_CODE_VERIFY_SIGNATURE_FAIL:
        case SC_ERROR_CODE_VERIFY_PUB_KEY_SIGNATURE_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_SIGNATURE_FAIL;

        case SC_ERROR_CODE_DK_INCORRECT:
            return MTA_AT_RESULT_DEVICE_SEC_DK_INCORRECT;

        case SC_ERROR_CODE_UNLOCK_KEY_INCORRECT:
            return MTA_AT_RESULT_DEVICE_SEC_UNLOCK_KEY_INCORRECT;

        case SC_ERROR_CODE_RSA_ENCRYPT_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_RSA_ENCRYPT_FAIL;

        case SC_ERROR_CODE_RSA_DECRYPT_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_RSA_DECRYPT_FAIL;

        case SC_ERROR_CODE_GET_RAND_NUMBER_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_GET_RAND_NUMBER_FAIL;

        case SC_ERROR_CODE_IDENTIFY_FAIL:
        case SC_ERROR_CODE_IDENTIFY_NOT_FINISH:
            return MTA_AT_RESULT_DEVICE_SEC_IDENTIFY_FAIL;

        case SC_ERROR_CODE_WRITE_HUK_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_WRITE_HUK_FAIL;

        case SC_ERROR_CODE_NV_READ_FAIL:
        case SC_ERROR_CODE_NV_WRITE_FAIL:
            return MTA_AT_RESULT_DEVICE_SEC_NV_ERROR;

        case SC_ERROR_CODE_LOCK_CODE_INVALID:
        case SC_ERROR_CODE_CREATE_KEY_FAIL:
        case SC_ERROR_CODE_GENERATE_HASH_FAIL:
        case SC_ERROR_CODE_AES_ECB_ENCRYPT_FAIL:
        case SC_ERROR_CODE_ALLOC_MEM_FAIL:
        case SC_ERROR_CODE_UNLOCK_STATUS_ABNORMAL:
        case SC_ERROR_CODE_BUTT:
        default:
            /* 上述错误以及其他错误，返回OTHER_ERROR */
            return MTA_AT_RESULT_DEVICE_SEC_OTHER_ERROR;
    }
}
VOS_VOID TAF_MTA_RcvAtApSecSetReq( VOS_VOID *pMsg )
{
    AT_MTA_MSG_STRU                    *pstAtMtaReqMsg;
    AT_MTA_APSEC_REQ_STRU              *pstApSecReq;
    MTA_AT_APSEC_CNF_STRU               stApSecCnf;
    VOS_UINT32                          ulResult;

    /* 局部变量初始化 */
    pstAtMtaReqMsg  = (AT_MTA_MSG_STRU*)pMsg;
    pstApSecReq     = (AT_MTA_APSEC_REQ_STRU*)pstAtMtaReqMsg->aucContent;

    /* 调用SC模块提供的安全封包解析函数 */
    ulResult        = SC_APSEC_ParseSecPacket(&pstApSecReq->ulSPLen,
                                              pstApSecReq->aucSecPacket);
    if (VOS_OK == ulResult)
    {
        /* 解析成功，将返回的安全封包发送给AT模块 */
        stApSecCnf.enResult = MTA_AT_RESULT_NO_ERROR;
        stApSecCnf.ulSPLen  = pstApSecReq->ulSPLen;
        PS_MEM_CPY(stApSecCnf.aucSecPacket, pstApSecReq->aucSecPacket, MTA_SEC_PACKET_MAX_LEN);
    }
    else
    {
        /* 解析失败，返回对应的错误码 */
        PS_MEM_SET(&stApSecCnf, 0x00, sizeof(MTA_AT_APSEC_CNF_STRU));
        stApSecCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
    TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                      ID_MTA_AT_APSEC_SET_CNF,
                      sizeof(MTA_AT_APSEC_CNF_STRU),
                      (VOS_UINT8*)&stApSecCnf );

    return;
}
VOS_VOID TAF_MTA_RcvAtSimlockUnlockSetReq( VOS_VOID *pMsg )
{
    AT_MTA_MSG_STRU                    *pstAtMtaReqMsg;
    AT_MTA_SIMLOCKUNLOCK_REQ_STRU      *pstSimlockUnlockReq;
    MTA_AT_SIMLOCKUNLOCK_CNF_STRU       stSimlockUnlockCnf;
    VOS_UINT32                          ulResult;

    /* 局部变量初始化 */
    pstAtMtaReqMsg          = (AT_MTA_MSG_STRU *)pMsg;
    pstSimlockUnlockReq     = (AT_MTA_SIMLOCKUNLOCK_REQ_STRU *)pstAtMtaReqMsg->aucContent;

    /* 调用SC模块提供的锁网锁卡解锁函数 */
    ulResult        = SC_PERS_SimlockUnlock(pstSimlockUnlockReq->enCategory,
                                            pstSimlockUnlockReq->aucPassword);
    if (VOS_OK == ulResult)
    {
        /* 解锁成功，返回结果给AT模块 */
        stSimlockUnlockCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }
    else
    {
        /* 解锁失败，返回结果给AT模块 */
        stSimlockUnlockCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
    TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                      ID_MTA_AT_SIMLOCKUNLOCK_SET_CNF,
                      sizeof(MTA_AT_SIMLOCKUNLOCK_CNF_STRU),
                      (VOS_UINT8*)&stSimlockUnlockCnf );

    return;
}
VOS_VOID TAF_MTA_RcvQryImeiVerifyReq(VOS_VOID *pMsg)
{
    SC_ERROR_CODE_ENUM_UINT32           enScResult;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulImeiVerify;
    AT_MTA_MSG_STRU                    *pstAtMtaReqMsg = VOS_NULL_PTR;

    pstAtMtaReqMsg = (AT_MTA_MSG_STRU *)pMsg;
    ulImeiVerify = VOS_TRUE;

    /* 由PID获取MODEMID */
    enModemId = VOS_GetModemIDFromPid(UEPS_PID_MTA);

    /* 调用SC接口进行IMEI校验，若函数执行失败，则按照IMEI校验失败处理 */
    enScResult = SC_PERS_VerifyIMEI(enModemId);
    if (SC_ERROR_CODE_NO_ERROR != enScResult)
    {
        ulImeiVerify = VOS_FALSE;
    }

    /* 返回跨核消息ID_MTA_AT_IMEI_VERIFY_QRY_CNF到AT模块 */
    TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                      ID_MTA_AT_IMEI_VERIFY_QRY_CNF,
                      sizeof(VOS_UINT32),
                      (VOS_UINT8*)&ulImeiVerify );

    return;
}
VOS_VOID TAF_MTA_RcvQryCgsnReq(VOS_VOID *pMsg)
{
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MTA_MSG_STRU                    *pstAtMtaReqMsg = VOS_NULL_PTR;
    MTA_AT_CGSN_QRY_CNF_STRU            stCgsnCnf;

    /* 初始化 */
    pstAtMtaReqMsg = (AT_MTA_MSG_STRU *)pMsg;
    PS_MEM_SET(&stCgsnCnf, 0x00, sizeof(MTA_AT_CGSN_QRY_CNF_STRU));

    /* 由PID获取MODEMID */
    enModemId = VOS_GetModemIDFromPid(UEPS_PID_MTA);

    /* 调用SC接口读取IMEI号码 */
    SC_PERS_NvRead(enModemId, en_NV_Item_IMEI, stCgsnCnf.aucImei, NV_ITEM_IMEI_SIZE);

    /* 返回跨核消息ID_MTA_AT_IMEI_VERIFY_QRY_CNF到AT模块 */
    TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                      ID_MTA_AT_CGSN_QRY_CNF,
                      sizeof(MTA_AT_CGSN_QRY_CNF_STRU),
                      (VOS_UINT8*)&stCgsnCnf );

    return;
}



VOS_VOID TAF_MTA_RcvAtNvwrSecCtrlSetReq( VOS_VOID *pMsg )
{
    AT_MTA_MSG_STRU                    *pstAtMtaReqMsg;
    AT_MTA_NVWRSECCTRL_SET_REQ_STRU    *pstNvwrSecCtrlReq;
    MTA_AT_RESULT_CNF_STRU              stNvwrSecCtrlCnf;
    TAF_NV_NVWR_SEC_CTRL_STRU           stNvwrSecCtrlNV;
    IMEI_STRU                           stImeiSrc;
    IMEI_STRU                           stImeiPara;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulImeiLen;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           aucImeiStr[TAF_NVIM_ITEM_IMEI_SIZE + 1];

    /* 局部变量初始化 */
    pstAtMtaReqMsg          = (AT_MTA_MSG_STRU *)pMsg;
    pstNvwrSecCtrlReq       = (AT_MTA_NVWRSECCTRL_SET_REQ_STRU *)pstAtMtaReqMsg->aucContent;

    /* SC提供的RSA封装需要调用者将解密后结果的预期长度传入 */
    ulImeiLen               = MTA_IMEI_LEN;

    PS_MEM_SET(&stNvwrSecCtrlNV, 0x00, sizeof(stNvwrSecCtrlNV));
    PS_MEM_SET(&stNvwrSecCtrlCnf, 0x00, sizeof(stNvwrSecCtrlCnf));
    PS_MEM_SET(&stImeiSrc, 0x00, sizeof(stImeiSrc));
    PS_MEM_SET(&stImeiPara, 0x00, sizeof(stImeiPara));
    PS_MEM_SET(aucImeiStr, 0x00, sizeof(aucImeiStr));

    /* 如果单板没有HUK则不作安全校验 */
    if (VOS_TRUE == DRV_CHECK_HUK_IS_VALID())
    {
        /* 单板已经写入HUK但未下发安全密文则返回失败 */
        if (VOS_TRUE != pstNvwrSecCtrlReq->ucSecStrFlg)
        {
            stNvwrSecCtrlCnf.enResult = MTA_AT_RESULT_DEVICE_SEC_SIGNATURE_FAIL;

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }

        /* 由PID获取MODEMID */
        enModemId = VOS_GetModemIDFromPid(UEPS_PID_MTA);

        /* 调用SC接口读取IMEI号码 */
        ulResult = SC_PERS_NvRead(enModemId, en_NV_Item_IMEI, stImeiSrc.aucImei, TAF_NVIM_ITEM_IMEI_SIZE);

        if (SC_ERROR_CODE_NO_ERROR != ulResult)
        {
            /* 异常，返回结果给AT模块 */
            stNvwrSecCtrlCnf.enResult = TAF_MTA_ConvertScErr(ulResult);

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }

        /* 调用SC模块接口, 对密文进行RSA解密 */
        ulResult = SC_FAC_RsaDecrypt(pstNvwrSecCtrlReq->aucSecString,
                                     AT_RSA_CIPHERTEXT_LEN,
                                     aucImeiStr,
                                     &ulImeiLen);

        if (MTA_IMEI_LEN != ulImeiLen)
        {
            ulResult = SC_ERROR_CODE_RSA_DECRYPT_FAIL;
        }

        if (SC_ERROR_CODE_NO_ERROR != ulResult)
        {
            /* 异常，返回结果给AT模块 */
            stNvwrSecCtrlCnf.enResult = TAF_MTA_ConvertScErr(ulResult);

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }

        /* 数字字符串转换为数字串 */
        if (VOS_OK != TAF_MTA_AcNums2DecNums(aucImeiStr, stImeiPara.aucImei, ulImeiLen))
        {
            stNvwrSecCtrlCnf.enResult = MTA_AT_RESULT_DEVICE_SEC_SIGNATURE_FAIL;

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }

        /* 校验IMEI码 */
        if (0 != PS_MEM_CMP(stImeiPara.aucImei, stImeiSrc.aucImei, ulImeiLen))
        {
            stNvwrSecCtrlCnf.enResult = MTA_AT_RESULT_DEVICE_SEC_SIGNATURE_FAIL;

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }
    }

    /* 修改安全控制NV */
    if (NV_OK == NV_Read(en_NV_Item_NVWR_SEC_CTRL, &stNvwrSecCtrlNV, sizeof(stNvwrSecCtrlNV)))
    {
        stNvwrSecCtrlNV.ucSecType = pstNvwrSecCtrlReq->ucSecType;

        if (NV_OK == NV_Write(en_NV_Item_NVWR_SEC_CTRL, &stNvwrSecCtrlNV, sizeof(stNvwrSecCtrlNV)))
        {
            /* 设置成功 */
            stNvwrSecCtrlCnf.enResult = MTA_AT_RESULT_NO_ERROR;

            /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
            TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                              ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                              sizeof(stNvwrSecCtrlCnf),
                              (VOS_UINT8*)&stNvwrSecCtrlCnf );

            return;
        }
    }

    /* NV修改失败 */
    stNvwrSecCtrlCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 返回跨核消息ID_MTA_AT_APSEC_SET_CNF到AT模块 */
    TAF_MTA_SndAtMsg( &pstAtMtaReqMsg->stAppCtrl,
                      ID_MTA_AT_NVWRSECCTRL_SET_CNF,
                      sizeof(stNvwrSecCtrlCnf),
                      (VOS_UINT8*)&stNvwrSecCtrlCnf );

    return;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

