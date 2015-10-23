


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmSecuInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasLmmPubMStack.h"
#include    "NasEmmSerProc.h"
#include    "NasEmmAttDetInterface.h"
#include    "NasEmmTauInterface.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMSECUAUTH_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
NAS_LMM_SECU_CONTROL_STRU    g_NasEmmSecuControl;


/*****************************************************************************
  3 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
VOS_UINT32  NAS_EMM_DeriveKasme(        const VOS_UINT8 *pucCK,
                                        const VOS_UINT8 *pucIK,
                                        VOS_UINT8 *pucKasme)
{
    VOS_UINT8                       aucAlgParam[NAS_EMM_SECU_MAX_ALG_PARAM_LEN];
    VOS_UINT8                       aucKeyParam[NAS_LMM_SECU_AUTH_KEY_ASME_LEN];
    VOS_UINT32                      ulIndex      = 0;
    VOS_UINT32                      ulRslt = NAS_EMM_SECU_FAIL;
    NAS_EMM_PLMN_ID_STRU           *pstSerPlmn = VOS_NULL_PTR;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_DeriveKasme is entered.");

    /*空指针*/
    if((VOS_NULL_PTR == pucCK) || (VOS_NULL_PTR == pucIK))
    {
         NAS_EMM_SECU_LOG_ERR("NAS_EMM_DeriveKasme:-> Input Pointer Null!");
         return NAS_EMM_SECU_FAIL;
    }

    /*获取 current PLMN作为计算Kasme的参数*/
    pstSerPlmn = NAS_LMM_GetEmmInfoPresentPlmnAddr();

    /* 编码KDF算法需要的参数 */

    /*encode FC*/
    aucAlgParam[ulIndex++] = NAS_EMM_SECU_ALG_PARAM_KASME_FC;

    /*encode PLMNID&Len*/
    aucAlgParam[ulIndex++] = pstSerPlmn->aucPlmnId[0];
    aucAlgParam[ulIndex++] = pstSerPlmn->aucPlmnId[1];
    aucAlgParam[ulIndex++] = pstSerPlmn->aucPlmnId[2];

    aucAlgParam[ulIndex++] = NAS_EMM_SECU_ALG_PARAM_KASME_PRE_LEN;
    aucAlgParam[ulIndex++] = NAS_EMM_SECU_ALG_PARAM_KASME_PLMNID_LEN;

    /* encode SQN&AK */
    NAS_LMM_MEM_CPY(                    &aucAlgParam[ulIndex],
                                        NAS_EMM_AuthReqAutnAddr(),
                                        NAS_EMM_SECU_SQN_LEN);

    ulIndex                             += NAS_EMM_SECU_SQN_LEN;

    aucAlgParam[ulIndex++]              = NAS_EMM_SECU_ALG_PARAM_KASME_PRE_LEN;
    aucAlgParam[ulIndex++]              = NAS_EMM_SECU_SQN_LEN;

    NAS_LMM_MEM_CPY((VOS_VOID *)aucKeyParam,pucCK,NAS_EMM_SECU_CKEY_LEN);

    NAS_LMM_MEM_CPY((VOS_VOID *)&aucKeyParam[NAS_EMM_SECU_CKEY_LEN],
                                      pucIK,NAS_EMM_SECU_IKEY_LEN);

    ulRslt = NAS_EMM_KDF_HmacSha256(aucAlgParam,ulIndex,aucKeyParam,pucKasme);

    return ulRslt;

}


VOS_VOID NAS_EMM_SendAuthRsp(VOS_VOID)
{
     VOS_UINT32                         ulPos          = 0;
     NAS_EMM_MRRC_DATA_REQ_STRU        *pIntraMsg      = VOS_NULL_PTR;
     VOS_UINT32                         uldataReqMsgLenNoHeader;

     /* 打印进入该函数， INFO_LEVEL */
     NAS_EMM_SECU_LOG_INFO("NAS_EMM_SendAuthRsp entered.");

     /* 申请消息内存 */
     pIntraMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

     /* 判断申请结果，若失败打印错误并退出 */
     if (NAS_EMM_NULL_PTR == pIntraMsg)
     {
         NAS_EMM_SECU_LOG_ERR("NAS_EMM_SendAuthRsp:->MSG ALLOC ERR!");
         return;
     }

     /* 构造AUTH RSP消息 */
     pIntraMsg->stNasMsg.aucNasMsg[ulPos++] = EMM_CN_MSG_PD_EMM;
     pIntraMsg->stNasMsg.aucNasMsg[ulPos++] = NAS_EMM_CN_MT_AUTH_RSP;

     /* 拷贝 RES的长度和内容 */
     pIntraMsg->stNasMsg.aucNasMsg[ulPos++] = NAS_EMM_GetSecuAuthResLen();
     NAS_LMM_MEM_CPY(      (VOS_VOID *)&(pIntraMsg->stNasMsg.aucNasMsg[ulPos]),
                                        NAS_EMM_GetSecuAuthResVAddr(),
                                        NAS_EMM_GetSecuAuthResLen());

     ulPos += NAS_EMM_GetSecuAuthResLen();

     /* 填充消息体 */
     pIntraMsg->ulMsgId                 = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
     pIntraMsg->enEstCaue               = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
     pIntraMsg->enCallType              = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
     pIntraMsg->enEmmMsgType            = NAS_EMM_MSG_AUTH_RSP;

     pIntraMsg->enDataCnf               = LRRC_LMM_DATA_CNF_NEED;

     /* 填充其中的NAS CN消息的长度 */
     pIntraMsg->stNasMsg.ulNasMsgSize   = ulPos;

     /*计算不包含消息头的消息长度*/
     uldataReqMsgLenNoHeader = NAS_EMM_COUNT_INTRA_MSG_NO_HEADER_LEN(
                                        pIntraMsg->stNasMsg.ulNasMsgSize);

     /* 填充消息头 */
     NAS_EMM_SET_SECU_INTRA_MSG_HEADER(pIntraMsg, uldataReqMsgLenNoHeader);

    /*空口消息上报 AUTH RSP*/
    NAS_LMM_SendOmtAirMsg(              NAS_EMM_OMT_AIR_MSG_UP,
                                        NAS_EMM_AUTH_RSP,
                                        (NAS_MSG_STRU*)&(pIntraMsg->stNasMsg));

    /*关键事件上报 AUTH RSP*/
    NAS_LMM_SendOmtKeyEvent(            EMM_OMT_KE_AUTH_RSP);


    /*向MRRC发送AUTH RSP消息*/
    NAS_EMM_SndUplinkNasMsg(            pIntraMsg);

     NAS_LMM_MEM_FREE(pIntraMsg);

     return;
}


VOS_VOID NAS_EMM_SendAuthFail(NAS_EMM_AUTH_FAIL_CAUSE_ENUM_UINT8 enAuthFailCause)
{
    VOS_UINT32                          uldataReqMsgLenNoHeader     = 0;
    VOS_UINT32                          ulPos                       = 0;
    VOS_UINT8                           aucNasMsg[NAS_EMM_AUTH_LEN_CN_MAX_FAIL_MSG];
    NAS_EMM_MRRC_DATA_REQ_STRU          *pIntraMsg                  = VOS_NULL_PTR;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_SendAuthFail entered.");

    /*申请消息内存*/
    pIntraMsg = (VOS_VOID *)NAS_LMM_MEM_ALLOC(NAS_EMM_INTRA_MSG_MAX_SIZE);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraMsg)
    {
       NAS_EMM_SECU_LOG_ERR("NAS_EMM_SendAuthFail: MSG ALLOC ERR!");
       return;
    }

    /* 构造Auth Fai消息 */
    aucNasMsg[ulPos++]                  = EMM_CN_MSG_PD_EMM;
    aucNasMsg[ulPos++]                  = NAS_EMM_CN_MT_AUTH_FAIl;
    aucNasMsg[ulPos++]                  = enAuthFailCause;

    if (NAS_EMM_AUTH_SYNC_FAILURE == enAuthFailCause)
    {
        aucNasMsg[ulPos++]              = NAS_EMM_AUTH_FAIl_PARAM_IEI;

        aucNasMsg[ulPos++]              = NAS_EMM_SECU_AUTH_AUTS_LEN;

        /* 获得 AUTS*/
        NAS_LMM_MEM_CPY(   (VOS_VOID *)&aucNasMsg[ulPos],
                                        NAS_EMM_GetSecuAuthAutsAddr(),
                                        NAS_EMM_SECU_AUTH_AUTS_LEN);

        ulPos += NAS_EMM_SECU_AUTH_AUTS_LEN;

    }

    /*lint -e669*/
    NAS_LMM_MEM_CPY(pIntraMsg->stNasMsg.aucNasMsg, aucNasMsg,ulPos);
    /*lint +e669*/

    /* 填充消息ID */
    pIntraMsg->ulMsgId                  = ID_NAS_LMM_INTRA_MRRC_DATA_REQ;
    pIntraMsg->enEstCaue                = LRRC_LNAS_EST_CAUSE_MO_SIGNALLING;
    pIntraMsg->enCallType               = LRRC_LNAS_CALL_TYPE_ORIGINATING_SIGNALLING;
    pIntraMsg->enEmmMsgType             = NAS_EMM_MSG_AUTH_FAIL;

    pIntraMsg->enDataCnf                = LRRC_LMM_DATA_CNF_NEED;

    /* 填充消息长度 */
    pIntraMsg->stNasMsg.ulNasMsgSize = ulPos;

    /*计算不包含消息头的消息长度*/
    uldataReqMsgLenNoHeader =
            NAS_EMM_COUNT_INTRA_MSG_NO_HEADER_LEN(pIntraMsg->stNasMsg.ulNasMsgSize);

    /* 填充消息头 */
    NAS_EMM_SET_SECU_INTRA_MSG_HEADER(pIntraMsg, uldataReqMsgLenNoHeader);

    /*空口消息上报 AUTH FAIL*/
    NAS_LMM_SendOmtAirMsg(               NAS_EMM_OMT_AIR_MSG_UP,
                                        NAS_EMM_AUTH_FAIL,
                       (NAS_MSG_STRU*)&(pIntraMsg->stNasMsg));

    /*关键事件上报 AUTH FAIL*/
    NAS_LMM_SendOmtKeyEvent(             EMM_OMT_KE_AUTH_FAIL);

    /*向MRRC发送Auth Fai消息*/
    NAS_EMM_SndUplinkNasMsg(pIntraMsg);

    NAS_LMM_MEM_FREE(pIntraMsg);

    return;

}
/* lihong00150010 ims begin */

VOS_VOID  NAS_EMM_UsimAuthSucc( const USIMM_AUTH_CNF_STRU  *pstUsimAuthCnf  )
{
    VOS_UINT32                          ulDeriveKeyRslt;
    USIMM_TELECOM_AUTH_CNF_STRU        *pstAuthCnf;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_UsimAuthSucc entered.");

    pstAuthCnf = (USIMM_TELECOM_AUTH_CNF_STRU*)&pstUsimAuthCnf->cnfdata.stTELECnf;

    /*1.更新新安全上下文:保存AuthReq消息携带的Ksi并计算新的Kasme*/
    NAS_EMM_ClearNonCurNativeSecuCntxt();
    NAS_EMM_SetSecuNewNativeKSIasme(NAS_EMM_AuthReqKsi());
    ulDeriveKeyRslt = NAS_EMM_DeriveKasme(&pstAuthCnf->aucCK[1],
                                          &pstAuthCnf->aucIK[1],
                                          NAS_EMM_GetSecuNewNativeKasmeAddr());
    if( NAS_EMM_SECU_FAIL == ulDeriveKeyRslt)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_PreProcUsimAuthCnf: Caculate Kasme ERR.");
    }

    /*安全上下文状态迁移*/
    NAS_EMM_SecuStateConvert(NAS_LMM_GetSecuCurFsmCS() ,EMM_NEW_SECU_EXIST);

    /*2. 启动T3416,保存Rand 和 Res*/
    /*启动T3416*/
    NAS_LMM_StartPtlTimer(                TI_NAS_EMM_PTL_T3416);

    /*保存Rand*/
    NAS_EMM_GetSecuAuthOpRand() = NAS_EMM_BIT_SLCT;
    NAS_LMM_MEM_CPY(                    NAS_EMM_GetSecuAuthRandAddr(),
                                        NAS_EMM_AuthReqRandAddr(),
                                        NAS_EMM_SECU_AUTH_RAND_LEN);

    /*保存Res:pstUsimAuthCnf->u.aucAuthRes第一个字节为Res的长度*/
    NAS_EMM_GetSecuAuthResLen() = pstAuthCnf->aucAuthRes[NAS_EMM_USIM_CNF_RES_L_POS];
    NAS_LMM_MEM_CPY( NAS_EMM_GetSecuAuthResVAddr(),
                   &(pstAuthCnf->aucAuthRes[NAS_EMM_USIM_CNF_RES_V_POS]),
                     NAS_EMM_GetSecuAuthResLen());

    /*3. 如果AUTH失败次数不为零，出栈并将失败次数清零*/
    if(0 != NAS_EMM_GetAuthFailTimes())
    {
        NAS_EMM_FSM_PopState();
        NAS_EMM_GetAuthFailTimes() = 0;
    }

    /*4. 发送AUTH RSP*/
    NAS_EMM_SendAuthRsp();

    return;
}
/* lihong00150010 ims end */

VOS_VOID  NAS_EMM_UsimAuthFail( NAS_EMM_AUTH_FAIL_CAUSE_ENUM_UINT8 enAuthFailCause )
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_UsimAuthFail entered.");

    /*1.鉴权失败的次数*/
    NAS_EMM_GetAuthFailTimes()++;

    if(NAS_EMM_FIRST_AUTH_FAIL == NAS_EMM_GetAuthFailTimes())
    {
        NAS_EMM_FSM_PushState();
    }

    /*连续鉴权失败达到三次，不再发送AUTH FAIL,释放连接，通知RRC当前小区被Bar*/
    if(NAS_EMM_MAX_AUTH_FAIL_TIMES == NAS_EMM_GetAuthFailTimes())
    {
        NAS_EMM_AuthCheckFail();
        return;
    }

    /*2.转换状态*/
    if(NAS_EMM_AUTH_SYNC_FAILURE == enAuthFailCause)
    {
        NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3420);
        NAS_EMM_AuthStateConvert(       EMM_MS_AUTH_INIT,
                                        EMM_SS_AUTH_WAIT_CN_AUTH,
                                        TI_NAS_EMM_T3420);
    }
    else
    {
        NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3418);
        NAS_EMM_AuthStateConvert(       EMM_MS_AUTH_INIT,
                                        EMM_SS_AUTH_WAIT_CN_AUTH,
                                        TI_NAS_EMM_T3418);
    }

    /*3.停止T3416，删除 Rand和Res*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*4.发送AUTH FAIL消息*/
    NAS_EMM_SendAuthFail(enAuthFailCause);

    return;
}
VOS_VOID NAS_EMM_IntraAuthFail( VOS_UINT32 ulCause)
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU       *pIntraAuthFail = VOS_NULL_PTR;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_IntraAuthFail entered.");

    /*申请消息内存*/
    pIntraAuthFail = (VOS_VOID *)NAS_LMM_GetSendIntraMsgBuffAddr(
                                        sizeof(NAS_EMM_INTRA_AUTH_FAIL_STRU) -
                                        NAS_EMM_LEN_VOS_MSG_HEADER);

    /*判断申请结果，若失败打印错误并退出*/
    if (NAS_EMM_NULL_PTR == pIntraAuthFail)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_IntraAuthFail: MSG ALLOC ERR!");
        return;
    }

    /* 打包VOS消息头 */
    NAS_EMM_SET_SECU_INTRA_MSG_HEADER(  pIntraAuthFail,
                                        sizeof(NAS_EMM_INTRA_AUTH_FAIL_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER);


    /* 填充DOPRA消息ID    */
    pIntraAuthFail->ulMsgId =           ID_NAS_LMM_INTRA_AUTH_FAIL;
    pIntraAuthFail->ulCause =           ulCause;

    /*发送内部消息入队列*/
    NAS_EMM_SEND_INTRA_MSG(             pIntraAuthFail);

    return;
}
VOS_VOID  NAS_EMM_AuthCommProcCnRej(    VOS_VOID )
{
    VOS_UINT32                          ulCause;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_AuthCommProcCnRej is entered.");

    ulCause                            = NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL;

    /* 停T3420，T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    /*发送内部消息INTRA_AUTH_FAIL*/
    NAS_EMM_IntraAuthFail(              ulCause);

    return;
}


VOS_VOID NAS_EMM_AuthCheckFail(VOS_VOID)
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_AuthCheckFail entered.");

    /* 修改状态：进入主状态EMM_MS_AUTH_INIT子状态EMM_SS_AUTH_WAIT_RRC_REL_IND */


    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_BARRED);
}


VOS_VOID  NAS_EMM_SecuClearRandRes( VOS_VOID )
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_SecuClearRandRes entered!");

    /* 删除RAND and RES */
    NAS_EMM_GetSecuAuthOpRand() = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_MEM_SET(                    NAS_EMM_GetSecuAuthRandAddr(),
                                        0,
                                        NAS_EMM_SECU_AUTH_RAND_LEN);
    NAS_LMM_MEM_SET(                    NAS_EMM_GetSecuAuthResAddr(),
                                        0,
                                        sizeof(NAS_EMM_SECU_AUTH_RES_STRU));
    return;
}
VOS_VOID NAS_EMM_AbortAuthProcedure(VOS_VOID)
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_AbortAuthProcedure entered!");

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    return;
}
VOS_VOID  NAS_EMM_AuthStateConvert(NAS_EMM_MAIN_STATE_ENUM_UINT16 ucMs,
                                   NAS_EMM_SUB_STATE_ENUM_UINT16 ucSs,
                                   NAS_EMM_STATE_TI_ENUM_UINT16 ucStaTId)
{
    NAS_EMM_FSM_STATE_STRU              stDestState;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_AuthStateConvert is entered.");

    stDestState.enFsmId                 = NAS_LMM_PARALLEL_FSM_EMM;
    stDestState.enMainState             = ucMs;
    stDestState.enSubState              = ucSs;
    stDestState.enStaTId                = ucStaTId;
    NAS_LMM_StaTransProc(stDestState);

    return;
}


VOS_VOID NAS_EMM_SendEsmDeactNonEmcBearInd( VOS_VOID )
{
    EMM_ESM_DEACT_NON_EMC_BEARER_IND_STRU *pstDeactNonEmerBearInd = NAS_EMM_NULL_PTR;

    /* 打印进入该函数， INFO_LEVEL */
    NAS_EMM_SECU_LOG_INFO( "NAS_EMM_SendEsmDeactNonEmcBearInd is entered.");

    /* 申请DOPRA消息 */
    pstDeactNonEmerBearInd = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_DEACT_NON_EMC_BEARER_IND_STRU));

    if(NAS_EMM_NULL_PTR == pstDeactNonEmerBearInd)
    {
        NAS_EMM_SECU_LOG_ERR( "NAS_EMM_SendEsmDeactNonEmcBearInd: MSG ALLOC ERR !!");
        return;
    }

    /* 填写DOPRA消息头 */
    NAS_EMM_SET_SECU_ESM_MSG_HEADER(pstDeactNonEmerBearInd, sizeof(EMM_ESM_DEACT_NON_EMC_BEARER_IND_STRU) -
                                        NAS_EMM_VOS_HEADER_LEN);

    /* 填写消息ID标识 */
    pstDeactNonEmerBearInd->ulMsgId    = ID_EMM_ESM_DEACT_NON_EMC_BEARER_IND;

    /* 发送消息*/
    NAS_LMM_SEND_MSG(pstDeactNonEmerBearInd);

    return;
}
/* lihong00150010 ims begin */

VOS_UINT32 NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq(        VOS_UINT32  ulMsgId,
                                                            VOS_VOID   *pMsg )
{
    NAS_EMM_AUTH_REQ_STRU              *pstAuthReq = VOS_NULL_PTR;
    /*NAS_EMM_USIM_PARAM_STRU             stAuthVector;*/
    VOS_UINT32                          ulHardUsimAuthRslt;
    VOS_UINT8                           ucSepBitInAMF;
    VOS_UINT8                           ucKsiAuthReq = 0;
    USIMM_AUTH_DATA_STRU                stAuth={0};

    (VOS_VOID)ulMsgId;
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq entered!");

    pstAuthReq                          = (NAS_EMM_AUTH_REQ_STRU *)pMsg;
    /* lihong00150010 emergency tau&service begin */
    if ((NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
        || (NAS_LMM_SIM_INVALID == NAS_LMM_GetPsSimValidity()))
    {
        NAS_EMM_SECU_LOG_WARN("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq: no USIM");

        return NAS_LMM_MSG_HANDLED;
    }
    /* lihong00150010 emergency tau&service end */
    /* 判断AUTH REQ消息中AMF携带的 Separation BIT,是否 For E_UTRAN*/
    ucSepBitInAMF = ((pstAuthReq->aucAutn[NAS_EMM_AUTH_AMF_IN_AUTN_POS])&0x80) >> 7;
    if(NAS_EMM_AUTH_FOR_EUTRAN != ucSepBitInAMF)
    {
        NAS_EMM_SECU_LOG1_NORM("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq: ERR. AMF Separation BIT = ",
                                        ucSepBitInAMF);
        NAS_EMM_UsimAuthFail(NAS_EMM_AUTH_NON_EPS_AUTH_UNACCEPTABLE);
        return NAS_LMM_MSG_HANDLED;
    }

    /* 判断当前随机值是否有效，如果有效，判断AUTH REQ消息携带的 RAND与本次保存的 RAND,如果相同，直接回复AUTH RSP*/
    if((NAS_EMM_BIT_SLCT == NAS_EMM_GetSecuAuthOpRand())
       &&(NAS_EMM_SECU_MEM_CMP_EQUAL == NAS_LMM_MEM_CMP(pstAuthReq->aucRand,
                                                    NAS_EMM_GetSecuAuthRandAddr(),
                                                    NAS_EMM_SECU_AUTH_RAND_LEN)))
    {
        NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq: New Rand == Old Rand!");
        NAS_EMM_SendAuthRsp();
        return NAS_LMM_MSG_HANDLED;
    }

    /* AUTH REQ消息携带的Ksi不能与当前安全上下文的Ksi相同*/
    ucKsiAuthReq = pstAuthReq->ucKsi & NAS_EMM_LOW_HALF_BYTE_F;
    if(NAS_EMM_GetSecuCurKSIasme() == ucKsiAuthReq)
    {
        NAS_EMM_SECU_LOG_WARN("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq: Ksi Of AUTH REQ same as Cur Ksi!");
    }

    /*记录AUTH REQ消息携带的 KSI,Rand,Autn:AUTN
      USIM回复鉴权成功时，Ksi保存到stNewSecuContext,Rand保存到stNasUsimAuthParam*/
    NAS_EMM_AuthReqKsi()                = ucKsiAuthReq;
    NAS_LMM_MEM_CPY(                    NAS_EMM_AuthReqRandAddr(),
                                        pstAuthReq->aucRand,
                                        NAS_EMM_SECU_AUTH_RAND_LEN);
    NAS_LMM_MEM_CPY(                    NAS_EMM_AuthReqAutnAddr(),
                                        pstAuthReq->aucAutn,
                                        NAS_EMM_SECU_AUTH_AUTN_LEN);

    stAuth.enAuthType = USIMM_3G_AUTH;

    /*调用硬USIM进行鉴权*/
    stAuth.unAuthPara.st3GAuth.stAutn.ulDataLen = \
                     (VOS_UINT8)pstAuthReq->ulAutnLen;
    stAuth.unAuthPara.st3GAuth.stAutn.pucData = NAS_EMM_AuthReqAutnAddr();

    stAuth.unAuthPara.st3GAuth.stRand.ulDataLen = \
                     NAS_EMM_SECU_AUTH_RAND_LEN;
    stAuth.unAuthPara.st3GAuth.stRand.pucData = NAS_EMM_AuthReqRandAddr();

    NAS_EMM_GetSecuAuthOpId()++;

    stAuth.ucOpId = NAS_EMM_GetSecuAuthOpId();


    /* 调用USIM的鉴权接口，以给USIM发送鉴权消息，然后等待USIM的鉴权结果，
    但不启定时器和状态等待 */
    ulHardUsimAuthRslt = USIMM_AuthReq(     PS_PID_MM,
                                            USIMM_UMTS_APP,
                                            &stAuth);



    NAS_EMM_SECU_LOG1_NORM("NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq: USIMM_AuthReq return = ",
                                            ulHardUsimAuthRslt);

    return NAS_LMM_MSG_HANDLED;

}
/* lihong00150010 ims end */

VOS_UINT32 NAS_EMM_MsAuthInitSsWtCnAuthMsgCnAuthReq(        VOS_UINT32  ulMsgId,
                                                            VOS_VOID   *pMsg )
{
    VOS_UINT32                          ulAuthRslt;

    (VOS_VOID)ulMsgId;
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgCnAuthReq entered!");

    /*停止3418,T3420 */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3420);

    ulAuthRslt = NAS_EMM_MsAnyStateSsAnyStateMsgCnAuthReq(ulMsgId, pMsg);

    return ulAuthRslt;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgCnAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru)
{
    (VOS_VOID)(pMsgStru);
    (VOS_VOID)ulMsgId;
    NAS_EMM_SECU_LOG_INFO( "NAS_EMM_MsAuthInitSsWtCnAuthMsgCnAuthRej entered.");

    /*CN AUTH REJ统一处理*/
    NAS_EMM_AuthCommProcCnRej();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_ATTACH_REJ_STRU         *pAttachRej = NAS_EMM_NULL_PTR;

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachRej entered.");

    pAttachRej                          = (NAS_EMM_CN_ATTACH_REJ_STRU *) pMsgStru;

    /*判断如果原因值不是#3,输出warn信息，统一做cause #3的处理*/
    if(NAS_LMM_CAUSE_ILLEGAL_UE != pAttachRej->ucCause)
    {
        NAS_EMM_SECU_LOG1_INFO( "NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachRej:cause is not #3!",
                               pAttachRej->ucCause);
    }

    NAS_EMM_AuthCommProcCnRej();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgTauRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_TAU_REJ_STRU         *pTauRej = NAS_EMM_NULL_PTR;

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);


    NAS_EMM_SECU_LOG_INFO( "NAS_EMM_MsAuthInitSsWtCnAuthMsgTauRej entered.");

    pTauRej                          = (NAS_EMM_CN_TAU_REJ_STRU *) pMsgStru;

    /*判断如果原因值不是#3,输出warn信息，统一做cause #3的处理*/
    if(NAS_LMM_CAUSE_ILLEGAL_UE != pTauRej->ucEMMCause)
    {
        NAS_EMM_SECU_LOG1_INFO( "NAS_EMM_MsAuthInitSsWaitCnAuthMsgTauRej:cause is not #3!",
                               pTauRej->ucEMMCause);
    }

    NAS_EMM_AuthCommProcCnRej();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgSerRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_SER_REJ_STRU         *pSerRej = NAS_EMM_NULL_PTR;

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);


    NAS_EMM_SECU_LOG_INFO( "NAS_EMM_MsAuthInitSsWtCnAuthMsgSerRej entered.");

    pSerRej                          = (NAS_EMM_CN_SER_REJ_STRU *) pMsgStru;

   /*判断如果原因值不是#3,输出warn信息，统一做cause #3的处理*/
    if(NAS_LMM_CAUSE_ILLEGAL_UE != pSerRej->ucEMMCause)
    {
         NAS_EMM_SECU_LOG1_INFO( "NAS_EMM_MsAuthInitSsWtCnAuthMsgSerRej:cause is not #3!",
                               pSerRej->ucEMMCause);
    }

    NAS_EMM_AuthCommProcCnRej();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgT3418Exp(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru )
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgT3418Exp entered.");

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);

    /* 如果当前没有正在建立紧急承载且没有紧急承载，则通知RRC释放链路，被BAR类型 */
    if ((VOS_TRUE != NAS_LMM_GetEmmInfoIsEmerPndEsting())
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGED)
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR))
    {
        /*确定网侧不能通过本次鉴权*/
        NAS_EMM_AuthCheckFail();
        return NAS_LMM_MSG_HANDLED;
    }

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    /* 通知ESM去激活所有非紧急承载 */
    if ((EMM_MS_REG == NAS_EMM_CUR_MAIN_STAT)
        || (EMM_MS_TAU_INIT == NAS_EMM_CUR_MAIN_STAT)
        || (EMM_MS_SER_INIT == NAS_EMM_CUR_MAIN_STAT))
    {
        NAS_EMM_SendEsmDeactNonEmcBearInd();
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgT3420Exp(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru )
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgT3420Exp is entered.");

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);

    /* 如果当前没有正在建立紧急承载且没有紧急承载，则通知RRC释放链路，被BAR类型 */
    if ((VOS_TRUE != NAS_LMM_GetEmmInfoIsEmerPndEsting())
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGED)
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR))
    {
        /*确定网侧不能通过本次鉴权*/
        NAS_EMM_AuthCheckFail();
        return NAS_LMM_MSG_HANDLED;
    }

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    /* 通知ESM去激活所有非紧急承载 */
    if ((EMM_MS_REG == NAS_EMM_CUR_MAIN_STAT)
        || (EMM_MS_TAU_INIT == NAS_EMM_CUR_MAIN_STAT)
        || (EMM_MS_SER_INIT == NAS_EMM_CUR_MAIN_STAT))
    {
        NAS_EMM_SendEsmDeactNonEmcBearInd();
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgRrcRelInd(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru)
{
    LRRC_LMM_REL_IND_STRU                *pRrcRelInd =       VOS_NULL_PTR;
    VOS_UINT32                          ulCause;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgRrcRelInd entered.");

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);

    pRrcRelInd                          = (LRRC_LMM_REL_IND_STRU*)(pMsgStru);
    ulCause                             = pRrcRelInd->enRelCause;

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    /* 向相关的模块发消息*/
    NAS_EMM_IntraAuthFail(              ulCause);

    return NAS_LMM_MSG_HANDLED;

}
/*****************************************************************************
 Function Name   : NAS_EMM_MsAuthInitSsWtCnAuthMsgEia0ActNotify
 Description     : 在AuthInit等待CN消息时处理EIA0 ACT NOTIFY消息
 Input           : ulMsgId,pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2012-10-19      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgEia0ActNotify
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgEia0ActNotify entered.");

    (VOS_VOID)(ulMsgId);
    (VOS_VOID)(pMsgStru);

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd
 Description     : 在AuthInit等待CN消息时处理DRB建立消息
 Input           : ulMsgId,pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2012-10-19      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd entered.");

    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }
    (VOS_VOID)ulMsgId;

    /* 栈顶的状态如果不为SERVICE_INIT态，则丢弃 */
    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);
    if (ulStaAtStackTop != NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd:not service_init+wait_cn_ser_cnf state!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果当前没有正在建立紧急承载且没有紧急承载，则丢弃 */
    if ((VOS_TRUE != NAS_LMM_GetEmmInfoIsEmerPndEsting())
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGED)
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgRabmDrbSetupInd:no emergency bearer and not esting emergency bearer");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    NAS_EMM_SER_RcvRabmDrbSetupInd(pMsgStru);

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept
 Description     : 在AuthInit等待CN消息时ATTACH ACCPET消息
 Input           : ulMsgId,pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2012-10-19      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept entered.");

    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 栈顶的状态如果不为REG_INIT态，则丢弃 */
    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);
    if (ulStaAtStackTop != NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept:not reg_init+wait_cn_attach_cnf state!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果当前不是紧急注册过程中，则丢弃 */
    if ((VOS_TRUE != NAS_LMM_GetEmmInfoIsEmerPndEsting())
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGING))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgAttachAccept:not emergency attach");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    return NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachAcp(ulMsgId, pMsgStru);
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept
 Description     : 在AuthInit等待CN消息时TAU ACCPET消息
 Input           : ulMsgId,pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2012-10-19      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept entered.");

    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 栈顶的状态如果不为TAU_INIT态，则丢弃 */
    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);
    if (ulStaAtStackTop != NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept:not tau_init+wait_cn_tau_cnf state!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果当前没有正在建立紧急承载且没有紧急承载，则丢弃 */
    if ((VOS_TRUE != NAS_LMM_GetEmmInfoIsEmerPndEsting())
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGED)
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgTauAccept:no emergency bearer and not esting emergency bearer");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    return NAS_EMM_MsTauInitSsWaitCNCnfMsgTAUAcp(ulMsgId, pMsgStru);
}

/*****************************************************************************
 Function Name   : NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp
 Description     : 在AuthInit等待CN消息时DETACH ACCPET消息
 Input           : ulMsgId,pMsgStru
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010        2012-10-19      Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp
(
    VOS_UINT32                          ulMsgId,
    VOS_VOID                           *pMsgStru
)
{
    VOS_UINT32                          ulStaAtStackTop = NAS_EMM_NULL;
    VOS_UINT32                          ulCurEmmStat    = NAS_EMM_NULL;

    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp entered.");

    if ( NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }

    /* 栈顶的状态如果不为TAU_INIT态，则丢弃 */
    ulStaAtStackTop = NAS_LMM_FSM_GetStaAtStackTop(NAS_LMM_PARALLEL_FSM_EMM);
    if ((ulStaAtStackTop != NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
        && (ulStaAtStackTop != NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF)))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp:not dereg_init+wait_cn_detach_cnf and not reg+wait_imsi_detach_cnf state!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 如果当前没有紧急承载，则丢弃 */
    if ((NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_EMC_REGED)
        && (NAS_LMM_GetEmmInfoRegStatus() != NAS_LMM_REG_STATUS_NORM_REGED_AND_EMC_BEAR))
    {
        NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAuthInitSsWtCnAuthMsgCnDetachAcp:no emergency bearer");
        return NAS_LMM_MSG_DISCARD;
    }

    /* 停T3420,T3418*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3418);
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3420);

    /* AUTH失败次数清零*/
    NAS_EMM_GetAuthFailTimes() = 0;

    /* 状态出栈*/
    NAS_EMM_FSM_PopState();

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(   NAS_EMM_CUR_MAIN_STAT,
                                                NAS_EMM_CUR_SUB_STAT);
    if (ulCurEmmStat == NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_DEREG_INIT,
                                                    EMM_SS_DETACH_WAIT_CN_DETACH_CNF))
    {
        return NAS_EMM_MsDrgInitSsWtCnDetCnfMsgCnDetachAcp(ulMsgId, pMsgStru);
    }
    else
    {
        return NAS_EMM_MsRegImsiDetachWtCnDetCnfMsgCnDetachAcp(ulMsgId, pMsgStru);
    }
}
/* lihong00150010 ims begin */

VOS_UINT32  NAS_EMM_MsAnyStateSsAnyStateMsgUsimAuthCnf(  VOS_UINT32  ulMsgId,
                                                         VOS_VOID   *pMsg )
{
    USIMM_AUTH_CNF_STRU      *pstUsimAuthCnf = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;
    NAS_EMM_SECU_LOG_INFO("NAS_EMM_MsAnyStateSsAnyStateMsgUsimAuthCnf entered!");
    NAS_LMM_PrintFsmState(NAS_LMM_PARALLEL_FSM_EMM);

    pstUsimAuthCnf = (USIMM_AUTH_CNF_STRU *)pMsg;
    /* 如果OpId不同，说明在这个鉴权CNF之前又收到了网侧的鉴权请求 ,继续等待USIM回复*/
    if(pstUsimAuthCnf->ucOpId != NAS_EMM_GetSecuAuthOpId())
    {
        return NAS_LMM_MSG_HANDLED;
    }

    /* 鉴权成功，清空OpId */
    NAS_EMM_GetSecuAuthOpId() = 0;
    /*根据 USIM_AUTH_CNF中的结果进行不同处理*/
    switch(pstUsimAuthCnf->enResult)
    {
             /*USIM鉴权成功*/
        case USIMM_AUTH_UMTS_SUCCESS:
             NAS_EMM_UsimAuthSucc(pstUsimAuthCnf);
             break;

             /*USIM鉴权失败:MAC FAIL*/
        case USIMM_AUTH_MAC_FAILURE:
             NAS_EMM_UsimAuthFail(NAS_EMM_AUTH_MAC_FAILURE);
             break;

             /*USIM鉴权失败:SYNCFAIL*/
        case USIMM_AUTH_SYNC_FAILURE:
             /*获取USIM_AUTH_CNF携带的AUTS:pstUsimAuthCnf->u.aucAuts第一个字节为Auts的长度*/
             NAS_LMM_MEM_CPY( NAS_EMM_GetSecuAuthAutsAddr(),
                            &(pstUsimAuthCnf->cnfdata.stTELECnf.aucAuts[NAS_EMM_USIM_CNF_AUTS_V_POS]),
                              pstUsimAuthCnf->cnfdata.stTELECnf.aucAuts[NAS_EMM_USIM_CNF_AUTS_L_POS]);

             /*调用失败处理函数*/
             NAS_EMM_UsimAuthFail(NAS_EMM_AUTH_SYNC_FAILURE);
             break;

        default:
             NAS_EMM_SECU_LOG_ERR("NAS_EMM_MsAnyStateSsAnyStateMsgUsimAuthCnf: USIM CNF RSLT ERR.");
             break;
    }

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 ims end */

VOS_VOID    NAS_EMM_SECU_ClearResource(VOS_VOID)
{
    NAS_EMM_SECU_LOG_NORM("NAS_EMM_SECU_ClearResource: enter.");

    NAS_LMM_MEM_SET( NAS_EMM_GetSecuUsimAuthParamAddr(),
                    0,
                    sizeof(NAS_LMM_USIM_AUTH_PARAM_STRU));

    NAS_EMM_GetSmcType() = NAS_EMM_SMC_UPDATE_BUTT;

    NAS_LMM_MEM_SET( NAS_EMM_GetSecuSrcSpace(),
                    0,
                    LRRC_LNAS_MAX_DATA_LENGTH - NAS_EMM_CATCH_OP_UINT);

    NAS_LMM_MEM_SET( NAS_EMM_GetSecuDstSpace(),
                    0,
                    LRRC_LNAS_MAX_DATA_LENGTH - NAS_EMM_CATCH_OP_UINT);

    NAS_LMM_MEM_SET( NAS_EMM_GetSecuBufSpace(),
                    0,
                    LRRC_LNAS_MAX_DATA_LENGTH);

    NAS_LMM_MEM_SET( NAS_EMM_GetAuthReqParamAddr(),
                    0,
                    sizeof(NAS_LMM_AUTH_REQ_PARAM_STRU));

    NAS_EMM_AuthReqKsi() = NAS_LMM_NKSI_NO_KEY_IS_AVAILABLE;

    NAS_EMM_GetAuthFailTimes() = 0;

    return;
}
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

