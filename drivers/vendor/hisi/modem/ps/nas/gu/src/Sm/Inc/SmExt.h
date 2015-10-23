

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


extern VOS_UINT8                  g_TiMapToNsapi[256];                              /* TI和NSAPI的映射关系                      */
extern VOS_UINT8                  g_CrMapToNsapi[256];                              /* CR和NSAPI的映射关系                      */
extern VOS_UINT8                  g_ucTiExt;                                        /* 记录ti是否有扩展位                       */
extern SM_ENTITY_STRU             g_SmEntity;
extern SM_PDP_ADDR_AND_APN_STRU   g_SmPdpAddrApn;
extern VOS_UINT8                  g_aucSapi[SM_MAX_NSAPI_NUM + SM_NSAPI_OFFSET][2];
extern GMMSM_SGSN_REL_VER_ENUM    g_SmSgsnVersion;
extern VOS_UINT32                 g_ulSmActAccSlice;

extern VOS_UINT8                  g_ucSmTimerFiveExpireFlag;

/* SmRcvTaf.c */
extern VOS_VOID NAS_SM_ProcTafPdpActiveReq(SMREG_PDP_ACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpActiveRej(SMREG_PDP_ACTIVATE_REJ_RSP_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpDeActiveReq(SMREG_PDP_DEACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpModifyRsp(SMREG_PDP_MODIFY_RSP_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpModifyReq(SMREG_PDP_MODIFY_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpActiveSecReq(SMREG_PDP_ACTIVATE_SEC_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpActiveSecReq_S00(SMREG_PDP_ACTIVATE_SEC_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpCnxtActReq_S00(SMREG_PDP_ACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpCnxtActReq_S05(SMREG_PDP_ACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_ProcTafPdpModifyRejRsp(SMREG_PDP_MODIFY_REJ_RSP_STRU*);

extern VOS_VOID NAS_SM_RcvTafPdpCnxtActReq(SMREG_PDP_ACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpCnxtActRejRsp(SMREG_PDP_ACTIVATE_REJ_RSP_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpCntxtModifyReq(SMREG_PDP_MODIFY_REQ_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpCnxtDeActReq(SMREG_PDP_DEACTIVATE_REQ_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpCnxtActSecReq(SMREG_PDP_ACTIVATE_SEC_REQ_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpModifyRsp(SMREG_PDP_MODIFY_RSP_STRU*);
extern VOS_VOID NAS_SM_RcvTafPdpModifyRejRsp(SMREG_PDP_MODIFY_REJ_RSP_STRU*);

extern VOS_UINT8 SM_PdpAddrApnComp(SMREG_PDP_ACTIVATE_REQ_STRU*);

/* send.c */
extern VOS_VOID SM_ReportM2NOtaMsg(NAS_MSG_STRU *pNasMsg);
extern VOS_VOID SM_ComGetTI(VOS_UINT8,VOS_UINT8**,VOS_UINT32*);
extern VOS_VOID SM_SndPdpActReqMsg(SMREG_PDP_ACTIVATE_REQ_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndReqPdpActRejMsg(SMREG_PDP_ACTIVATE_REJ_RSP_STRU *,VOS_UINT8);
extern VOS_VOID SM_SndPdpDeActReqMsg(SMREG_PDP_DEACTIVATE_REQ_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndPdpModAccMsg(SMREG_PDP_MODIFY_RSP_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndPdpModReqMsg(SMREG_PDP_MODIFY_REQ_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndPdpActSecReqMsg(SMREG_PDP_ACTIVATE_SEC_REQ_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndPdpDeActAccMsg(VOS_UINT8);
extern VOS_VOID SM_SndGmmPdpStatusInd(VOS_VOID);
extern VOS_VOID SM_SndGmmEstablishReq(VOS_VOID);

extern VOS_VOID SM_SndTafSmPdpActivateRej(VOS_UINT8,SM_TAF_CAUSE_ENUM_UINT16,SM_OCT_VARY_STRU*);

extern VOS_VOID SM_SndTafSmPdpActivateInd(SM_NW_MSG_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndTafSmPdpActivateCnf(SM_NW_MSG_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndTafSmPdpDeActCnf(VOS_UINT8);

extern VOS_VOID SM_SndTafSmPdpDeActInd(VOS_UINT8,SM_TAF_CAUSE_ENUM_UINT16,VOS_UINT8);

extern VOS_VOID SM_SndTafSmPdpModifyInd(SM_NW_MSG_STRU*,VOS_UINT8);
extern VOS_VOID SM_SndTafSmPdpModifyCnf(SM_NW_MSG_STRU*,VOS_UINT8);

extern VOS_VOID SM_SndTafSmPdpModifyRej(SM_TAF_CAUSE_ENUM_UINT16,VOS_UINT8);

extern VOS_VOID SM_SndTafSmPdpActSecCnf(SM_NW_MSG_STRU*,VOS_UINT8);

extern VOS_VOID SM_SndTafSmPdpActSecRej(VOS_UINT8,SM_TAF_CAUSE_ENUM_UINT16);

extern VOS_VOID SM_SndRabmSmActivateInd(VOS_UINT8    ucCntxtIndex,
                                    VOS_UINT8    ucActMsgType );
extern VOS_VOID SM_SndRabmSmModifyInd(VOS_UINT8);
extern VOS_VOID SM_SndRabmSmDeactivateInd(VOS_UINT8,VOS_UINT8*);

extern VOS_VOID SM_SndNwStatusMsg(VOS_UINT8,VOS_UINT8);
extern VOS_VOID SM_SndStatusMsg( VOS_UINT8 ucTi, VOS_UINT8  ucSmCause );

extern VOS_UINT8 SM_GetMostHighPri(VOS_VOID);
extern VOS_VOID SM_FillMsgData(VOS_VOID *pMsgContent,VOS_VOID *pData,VOS_UINT32 ulLen);
extern VOS_VOID SM_SndTafMsg(VOS_UINT8 *, VOS_UINT8, VOS_UINT32, VOS_UINT8, VOS_UINT8 );
extern VOS_VOID NAS_SM_SndGmmPdpDeactivatedInd(VOS_VOID);
extern VOS_VOID NAS_SM_SndPdpModRejMsg(SMREG_PDP_MODIFY_REJ_RSP_STRU *pstMsgIE, VOS_UINT8 ucSMIndex);

extern VOS_VOID NAS_SM_SndGmmPdpModifyInd(VOS_UINT8 ucNsapi);

/* main.c */
extern VOS_VOID SM_Init(VOS_VOID);
extern VOS_VOID NAS_SM_RcvTafMsg(MSG_HEADER_STRU*);
extern VOS_VOID SM_GmmMsgDistr(VOS_VOID*);
extern VOS_VOID SM_RabmMsgDistr(VOS_VOID*);
extern VOS_VOID SM_RcvTimerMsgDistr(VOS_UINT32);
extern VOS_VOID SM_T3380Expire(VOS_UINT8);
extern VOS_VOID SM_T3381Expire(VOS_UINT8);
extern VOS_VOID SM_T3390Expire(VOS_UINT8);
extern VOS_VOID SM_RegisterProtectTimerExpire(VOS_UINT8);
extern VOS_VOID SM_NwActProtectTimerExpire(VOS_UINT8);
extern VOS_VOID SM_NwModifyProtectTimerExpire(VOS_UINT8);
extern VOS_VOID SM_TimerStart(VOS_UINT8,VOS_UINT32,VOS_UINT8);
extern VOS_VOID SM_TimerStop( VOS_UINT8  ucTimerId );
extern VOS_VOID SM_RcvStatusMsg( GMMSM_DATA_IND_STRU *);
extern VOS_VOID Sm_FillQosInfo(NAS_QOS_STRUCT *pMsg);
extern VOS_VOID Sm_OmQuery(VOS_VOID *pMsg);


extern VOS_UINT8 SM_MemCmp(VOS_UINT8 *p1, VOS_UINT8 *p2, VOS_UINT32 ulLength);

extern VOS_VOID SM_AgentMsgDistr( VOS_VOID *pRcvMsg );
extern VOS_VOID SM_ReportN2MOtaMsg(VOS_UINT8 ucMsgType, NAS_MSG_STRU *pNasMsg);

extern VOS_UINT16 NAS_SM_CalcMaxSduSize(VOS_UINT8 *pucQos, VOS_UINT8 ucCurrOctet);
extern VOS_UINT16 NAS_SM_CalcTransDelay(VOS_UINT8 *pucQos, VOS_UINT8 ucCurrOctet);
extern VOS_UINT32 NAS_SM_CalcQosBitRate(VOS_UINT8 *pucQos, VOS_UINT8 ucCurrOctetIndex, VOS_UINT8 ucExtOctetIndex, VOS_UINT8 ucBitRateType, VOS_UINT32 ulQosLen);
extern VOS_VOID   NAS_SM_FillQos(NAS_QOS_STRUCT *pMsg, VOS_UINT8 *pucQos, VOS_UINT32 ulQosLen);
extern VOS_UINT32 NAS_SM_FindCurrPdpId(VOS_UINT8 *pucNSAPI);

/* SmRcvRabm.c */
extern VOS_VOID SM_RcvRabmDeActReq( RABMSM_DEACTIVATE_REQ_STRU *);

/* SmRcvGmm.c */
extern VOS_UINT8 SM_ComSaveTICheck(VOS_UINT8 ,VOS_UINT8**,VOS_UINT32*,SM_NW_MSG_STRU*);
extern VOS_UINT32 SM_ComOptionalCheck(VOS_UINT8*, VOS_UINT32 ,VOS_UINT32*, SM_NW_MSG_STRU *);
extern SM_NW_MSG_STRU* SM_ComActPdpCntxtAccCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComActPdpCntxtRejCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComReqPdpCntxtActMsgCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComModifyPdpCntxtAccCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComModifyPdpCntxtReqCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComActSecPdpCntxtAccCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComActSecPdpCntxtRejCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComModifyPdpCntxtRejCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComDeActPdpCntxtReqCheck(VOS_UINT8*,VOS_UINT32);
extern SM_NW_MSG_STRU* SM_ComDeActPdpCntxtAccCheck(VOS_UINT8*,VOS_UINT32);
extern VOS_VOID SM_RcvGmmSmEstabishCnf(GMMSM_ESTABLISH_CNF_STRU*);
extern VOS_VOID SM_RcvGmmSmStatusInd(GMMSM_STATUS_IND_STRU*);

extern VOS_VOID SM_RcvGmmSmServiceRej(GMMSM_SERVICE_REJ_STRU*);

extern VOS_VOID SM_RcvGmmSysInfoInd(GMMSM_SYS_INFO_IND_STRU *);
extern VOS_VOID SM_RcvGmmSmPdpStatusInd(GMMSM_PDP_STATUS_IND_STRU*);
extern VOS_VOID SM_RcvGmmRelInd(GMMSM_REL_IND_STRU *);
extern VOS_VOID SM_RcvNwReqPdpCntxtActMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwActPdpCntxtAccMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwActPdpCntxtRejMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwActSecPdpCntxtAccMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwActSecPdpCntxtRejMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwModifyPdpCntxtReqMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwModifyPdpCntxtAccMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwModifyPdpCntxtRejMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwDeActPdpCntxtReqMsg(SM_NW_MSG_STRU*);
extern VOS_VOID SM_RcvNwDeActPdpCntxtAccMsg(SM_NW_MSG_STRU*);
extern VOS_UINT8 SM_ComFindNullAddrApn(VOS_VOID);
extern VOS_VOID SM_ComClearPdpCntxt(VOS_UINT8);
extern VOS_VOID SM_ComClearPdpAddrApn(VOS_UINT8);
extern VOS_VOID SM_ComDelNsapiFromAddr(VOS_UINT8,VOS_UINT8);
extern VOS_VOID SM_RcvNwReqPdpCnxtActMsg_S00(SM_NW_MSG_STRU*,VOS_UINT8);
extern VOS_UINT8 SM_RcvNwReqPdpCnxtActMsg_S01(SM_NW_MSG_STRU*,VOS_UINT8,VOS_UINT8);
extern VOS_VOID SM_RcvNwReqPdpCnxtActMsg_S02(SM_NW_MSG_STRU*,VOS_UINT8);

extern VOS_UINT32 NAS_SM_ComMsgCheck(VOS_UINT8,VOS_UINT32,VOS_UINT8*,SM_NW_MSG_STRU**);
extern VOS_UINT32 NAS_SM_ValidNasMsg(GMMSM_DATA_IND_STRU*,VOS_UINT8*,VOS_UINT8*);
extern VOS_VOID NAS_SM_RcvGmmSmDataInd(GMMSM_DATA_IND_STRU*);
extern VOS_BOOL NAS_SM_ComparePdpAddr(SM_MSG_IE_COMMON_STRU*, SM_PDP_ADDR_STRU*);

/* SmAgent.c */
extern VOS_VOID SM_RcvAgentSmInquire( VOS_VOID    *pRcvMsg );
extern VOS_VOID SM_RcvAgentSmPdpnumInquire( VOS_VOID );
extern VOS_VOID SM_SndAgentSmAgentInfo( VOS_UINT8 ucNsapi );
extern VOS_VOID SM_SndAgentSmPdpnumInd( VOS_VOID );
extern VOS_VOID SM_SndAgentStatusInd( VOS_UINT8 ucTi, VOS_UINT8 ucSmCause, VOS_UINT8 ucDirection );

/* 外部API接口 */
extern VOS_INT8 NAS_Common_Get_Supported_3GPP_Version(VOS_UINT32 ulCnDomainId);
extern VOS_VOID Aps_GetPdpContextInfo(VOS_UINT8 ucNsapi, NAS_PDP_CONTEXT_STRU *pstPdpCntxt);


/*****************************************************************************
 函 数 名  : NAS_SM_SndGmmAbortReq
 功能描述  : 发送GMMSM_ABORT_REQ消息至GMM, 清除SM缓存
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 NAS_SM_SndGmmAbortReq(VOS_VOID);


/*****************************************************************************
 函 数 名  : NAS_SM_RcvTafPdpAbortReq
 功能描述  : 处理ID_APS_SM_PDP_ABORT_REQ消息, 用于取消激活/去激活/修改PDP操作
 输入参数  : pstPdpAbortReq             - 消息指针
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_SM_RcvTafPdpAbortReq(
    SMREG_PDP_ABORT_REQ_STRU          *pstPdpAbortReq
);


/*****************************************************************************
 函 数 名  : NAS_SM_RcvTafPdpLocalDeactivateReq
 功能描述  : 处理ID_APS_SM_PDP_LOCAL_DEACTIVATE_REQ消息, 用于释放SM实体
 输入参数  : pstPdpLocalDeactivateReq   - 消息指针
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_SM_RcvTafPdpLocalDeactivateReq(
    SMREG_PDP_LOCAL_DEACTIVATE_REQ_STRU    *pstPdpLocalDeactivateReq
);




/*****************************************************************************
 函 数 名  : NAS_SM_RcvGmmSigConnInd
 功能描述  : 处理 GMMSM_SIG_CONN_IND 连接建立成功 原语
 输入参数  : pstSigConnInd - 消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_SM_RcvGmmSigConnInd(
    GMMSM_SIG_CONN_IND_STRU            *pstSigConnInd
);

/*****************************************************************************
 函 数 名  : NAS_SM_MapGmmCause
 功能描述  : 将GMM错误码映射成SM/APS错误码
 输入参数  : enGmmCause - GMM错误码
 输出参数  : 无
 返 回 值  : SM_TAF_CAUSE_ENUM_UINT16
 调用函数  :
 被调函数  :
*****************************************************************************/
SM_TAF_CAUSE_ENUM_UINT16 NAS_SM_MapGmmCause(
    GMM_SM_CAUSE_ENUM_UINT16            enGmmCause
);

VOS_UINT8 * NAS_SM_GetTiMapToNsapi(VOS_VOID);
VOS_UINT8 * NAS_SM_GetCrMapToNsapi(VOS_VOID);
VOS_UINT8 NAS_SM_GetTiExt(VOS_VOID);
VOS_VOID NAS_SM_SetTiExt(VOS_UINT8 ucTiExt);
SM_ENTITY_STRU * NAS_SM_GetSmEntity(VOS_VOID);
SM_PDP_ADDR_AND_APN_STRU * NAS_SM_GetSmPdpAddrApn(VOS_VOID);
VOS_UINT8 * NAS_SM_GetSapi(VOS_UINT8 ucIndex);
GMMSM_SGSN_REL_VER_ENUM NAS_SM_GetSmSgsnVersion(VOS_VOID);
VOS_VOID NAS_SM_SetSmSgsnVersion(GMMSM_SGSN_REL_VER_ENUM enSmSgsnVersion);
VOS_VOID NAS_SM_SndGmmPdpStatusEx(VOS_VOID);

#if (FEATURE_ON == FEATURE_LTE)
/*****************************************************************************
 函 数 名  : NAS_SM_ProcGmmLteMoDetachInd
 功能描述  : 处理GMM的LTE去附着指示
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID NAS_SM_ProcGmmLteMoDetachInd(VOS_VOID);
#endif


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
