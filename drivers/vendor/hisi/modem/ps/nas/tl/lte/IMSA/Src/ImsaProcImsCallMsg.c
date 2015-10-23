/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaProcImsCallMsg.c
  Description     : 该C文件实现IMS呼叫消息处理和IMS呼叫消息发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaProcImsCallMsg.h"
#include "ImsaEntity.h"
#include "ImsaImsEvent.h"
#include "ImsaImsApi.h"
#include "ImsaEntity.h"
#include "ImsaCallManagement.h"
#include "ImsaPublic.h"
#include "ImsaRegManagement.h"
#include "ImsaImsInterface.h"
#include "CallImsaInterface.h"
#include "MnClient.h"
#include "ImsaProcSpmMsg.h"


/*lint -e767*/
#define    THIS_FILE_ID    PS_FILE_ID_IMSAPROCIMSCALLMSG_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#if (FEATURE_ON == FEATURE_IMS)

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

VOS_UINT32 IMSA_CallProcImsStateActive(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateHeld(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateTrying(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateDialing(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateAlerting(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateIncoming(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateWaiting(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
VOS_UINT32 IMSA_CallProcImsStateInitializing(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);

VOS_UINT32 IMSA_CallProcImsState(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary);
MN_CALL_STATE_ENUM_UINT8 IMSA_CallImsSrvccState2TafCallState(IMSA_IMS_CALL_STATE_ENUM_UINT8  enCallState);

extern VOS_VOID IMSA_CallEntityUpdateByCallSummary
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary
);
extern VOS_VOID  IMSA_CallRcvImsStartDtmfOk( VOS_VOID );
extern VOS_VOID  IMSA_CallRcvImsStopDtmfOk( VOS_VOID );
extern VOS_UINT32 IMSA_CallProcImsMsgOk(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_VOID IMSA_CallEntitySaveErrorInfo
(
    IMSA_CALL_ENTITY_STRU                  *pstCallEntity,
    const IMSA_IMS_OUTPUT_CALL_EVENT_STRU  *pstOutputCallEvent
);
extern VOS_VOID  IMSA_CallRcvImsStartDtmfError( VOS_VOID );
extern VOS_UINT32 IMSA_CallProcImsMsgError(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgIncoming(IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo380ForNormal
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU *pstImsError
);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo380ForEmc
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU     *pstImsError
);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo380
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU *pstImsError
);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo503
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU *pstImsError
);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo504
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU *pstImsError
);
extern VOS_UINT32 IMSA_CallEntityProcErrorInfo
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU *pstImsError
);
/*extern TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallTransImsErr2SpmErr
(
    VOS_UINT16                          usImsErr
);
extern VOS_VOID IMSA_CallReleaseCallCommonProc
(
    IMSA_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
);*/
extern VOS_UINT32 IMSA_CallProcImsMsgDisconnect(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgWaiting(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgCallEarlyMedia(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgInitializing(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_VOID IMSA_CallProcImsMsgSrvccCallList(IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgEmergencyInd(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallIsProccessCallSwap(VOS_VOID);
extern VOS_UINT32 IMSA_CallIsAllCallOnHold(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallIsAllCallInConference(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsMsgCallList(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_UINT32 IMSA_CallProcImsStateInvalid
(
    IMSA_CALL_ENTITY_STRU *pstCallEntity,
    const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary
);
extern IMSA_CALL_STATUS_ENUM_UINT8 IMSA_CallImsState2ImsaState
(
    IMSA_IMS_CALL_STATE_ENUM_UINT8 enImsState,
    IMSA_CALL_STATUS_ENUM_UINT8 enOldState
);
extern VOS_UINT32 IMSA_CallAllStateWillBeHeld(VOS_UINT32 ulWillChangedId);

/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/

VOS_VOID IMSA_CallEntityUpdateByCallSummary(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{

    /* 更新呼叫Id */
    pstCallEntity->ucId = (VOS_UINT8)pstCallSummary->ulCallIndex;

    /* 更新呼叫方向 */
    if (IMSA_IMS_CALL_DIR_MOBILE_ORIGINATED == pstCallSummary->enCallDirection)
    {
        pstCallEntity->enDir = MN_CALL_DIR_MO;
    }
    else
    {
        pstCallEntity->enDir = MN_CALL_DIR_MT;
    }

    /* 更新呼叫方式 */
    /**
     * 呼叫模式使用ORIG REQ命令参数的数值，
     * 这里不进行更新
     */

    /* 更新多方呼叫标识 */
    if (IMSA_IMS_CALL_SINGLE_PARTY == pstCallSummary->enMultiParty)
    {
        pstCallEntity->enMpty = MN_CALL_NOT_IN_MPTY;
    }
    else
    {
        pstCallEntity->enMpty = MN_CALL_IN_MPTY;
    }

    /* 更新主叫号码或者被叫号码 */
    if (IMSA_IMS_CALL_DIR_MOBILE_ORIGINATED == pstCallSummary->enCallDirection)
    {
        IMSA_CallEntityUpdateCalledNumberByAscii((VOS_CHAR *)pstCallSummary->acNumber, &pstCallEntity->stCalledNumber);
        IMSA_CallEntityUpdateCallNumberByAscii((VOS_CHAR *)pstCallSummary->acConnectNumber, &pstCallEntity->stConnectNumber);
    }
    else
    {
        IMSA_CallEntityUpdateCallNumberByAscii((VOS_CHAR *)pstCallSummary->acNumber, &pstCallEntity->stCallNumber);

        if (IMSA_IMS_CALL_STATE_INCOMING == pstCallSummary->enCallState)
        {
            /* 只有在INCOMMING状态下，才更新呼叫转移号码，其他状态下，该号码不会变更，没有必要更新 */
            IMSA_CallEntityUpdateCallNumberByAscii((VOS_CHAR *)pstCallSummary->acRedirectNumber, &pstCallEntity->stRedirectNumber);
        }
    }
}
VOS_VOID IMSA_CallDtmfInfoInit(VOS_VOID)
{
    VOS_UINT8                          ucIndex = 0;

    for(ucIndex = 0; ucIndex < IMSA_CALL_DTMF_BUF_MAX_NUM; ucIndex++)
    {
        IMSA_MEM_SET(&IMSA_CallGetDtmfBufList(ucIndex), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));
    }

    IMSA_MEM_SET(IMSA_CallGetCurrentDtmfInfo(), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    IMSA_CallGetDtmfCnt() = 0;

    return;

}



TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallSaveDtmfInfo(VOS_UINT8 ucCallId,
                                                      VOS_CHAR  cKey,
                                                      VOS_UINT16 usDuration,
                                                      VOS_UINT16 usClientId,
                                                      VOS_UINT8  ucSpmOpid)
{
    IMSA_CALL_DTMF_INFO_STRU stDtmfInfo;

    IMSA_MEM_SET(&stDtmfInfo, 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));

    /* 判断缓存是否已经满了 */
    if(IMSA_CallGetDtmfCnt() >= IMSA_CALL_DTMF_BUF_MAX_NUM)
    {
        return TAF_CS_CAUSE_DTMF_BUF_FULL;
    }

    /* 判断是否连续缓存两个STOP DTMF请求*/
    if((0 == usDuration) && (IMSA_CallGetDtmfCnt() >= 1))
    {
        if(0 == IMSA_CallGetDtmfBufList(IMSA_CallGetDtmfCnt() - 1).usOnLength)
        {
            return TAF_CS_CAUSE_DTMF_REPEAT_STOP;
        }
    }

    /* 缓存成功 */
    stDtmfInfo.ucCallId = ucCallId;
    stDtmfInfo.cKey = cKey;
    stDtmfInfo.usOnLength = usDuration;
    stDtmfInfo.usClientId = usClientId;
    stDtmfInfo.ucSpmOpId = ucSpmOpid;

    IMSA_MEM_CPY(&IMSA_CallGetDtmfBufList(IMSA_CallGetDtmfCnt()),
                 &stDtmfInfo,
                 sizeof(IMSA_CALL_DTMF_INFO_STRU));
    IMSA_CallGetDtmfCnt()++;

    return TAF_CS_CAUSE_SUCCESS;
}
VOS_VOID IMSA_CallDeleteSavedDtmfInfo(VOS_UINT8 ucIndex)
{
    VOS_UINT8 i = 0;

    if(ucIndex >= IMSA_CallGetDtmfCnt())
    {
        return;
    }
    for(i = 0; i < ((IMSA_CallGetDtmfCnt() - ucIndex) - 1); i++)
    {
        IMSA_MEM_CPY(&IMSA_CallGetDtmfBufList(ucIndex + i),
                     &IMSA_CallGetDtmfBufList(ucIndex + i + 1),
                     sizeof(IMSA_CALL_DTMF_INFO_STRU));
    }

    IMSA_MEM_SET(&IMSA_CallGetDtmfBufList(IMSA_CallGetDtmfCnt() - 1), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));

    IMSA_CallGetDtmfCnt()--;

    return;
}


VOS_VOID IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_ENUM_UINT32 enCause)
{
    IMSA_CALL_MANAGER_STRU            *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT8                          ucIndex = 0;

    IMSA_INFO_LOG("IMSA_CallClearDtmfInfo is entered!");

    /* 清除缓存的DTMF信息 */
    for(ucIndex = 0; ucIndex < IMSA_CallGetDtmfCnt(); ucIndex++)
    {
        if(0 == IMSA_CallGetDtmfBufList(ucIndex).usOnLength)
        {
            (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(IMSA_CallGetDtmfBufList(ucIndex).usClientId,
                                                      IMSA_CallGetDtmfBufList(ucIndex).ucSpmOpId,
                                                      IMSA_CallGetDtmfBufList(ucIndex).ucCallId,
                                                      enCause);
        }
        else
        {
            (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg(IMSA_CallGetDtmfBufList(ucIndex).usClientId,
                                                       IMSA_CallGetDtmfBufList(ucIndex).ucSpmOpId,
                                                       IMSA_CallGetDtmfBufList(ucIndex).ucCallId,
                                                       enCause);
        }

        IMSA_MEM_SET(&IMSA_CallGetDtmfBufList(ucIndex), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));
    }

    /* 清除当前的DTMF信息 */
    switch(IMSA_CallGetDtmfState())
    {
    case IMSA_CALL_DTMF_WAIT_START_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg(IMSA_CallGetCurrentDtmfInfo()->usClientId,
                                                   IMSA_CallGetCurrentDtmfInfo()->ucSpmOpId,
                                                   IMSA_CallGetCurrentDtmfInfo()->ucCallId,
                                                   enCause);
        break;
    case IMSA_CALL_DTMF_WAIT_STOP_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(IMSA_CallGetCurrentDtmfInfo()->usClientId,
                                                  IMSA_CallGetCurrentDtmfInfo()->ucSpmOpId,
                                                  IMSA_CallGetCurrentDtmfInfo()->ucCallId,
                                                  enCause);
        break;
    case IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        break;
    case IMSA_CALL_DTMF_WAIT_TIME_OUT:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfDurationTimer);
        break;
    default:
        IMSA_INFO_LOG("IMSA_CallClearDtmfInfo: nothing process on this state");
        break;
    }

    IMSA_MEM_SET(IMSA_CallGetCurrentDtmfInfo(), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    IMSA_CallGetDtmfCnt() = 0;

    return;
}


VOS_VOID IMSA_CallSrvccSuccClearDtmfInfo(TAF_CS_CAUSE_ENUM_UINT32 enCause)
{
    IMSA_CALL_MANAGER_STRU            *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT8                          ucIndex = 0;

    IMSA_INFO_LOG("IMSA_CallSrvccSuccClearDtmfInfo is entered!");

    /* 清除缓存的DTMF信息，不用回复结果，SRVCC成功后在CS域继续处理 */
    for(ucIndex = 0; ucIndex < IMSA_CallGetDtmfCnt(); ucIndex++)
    {
        IMSA_MEM_SET(&IMSA_CallGetDtmfBufList(ucIndex), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));
    }

    /* 清除当前的DTMF信息 */
    switch(IMSA_CallGetDtmfState())
    {
    case IMSA_CALL_DTMF_WAIT_START_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg(IMSA_CallGetCurrentDtmfInfo()->usClientId,
                                                   IMSA_CallGetCurrentDtmfInfo()->ucSpmOpId,
                                                   IMSA_CallGetCurrentDtmfInfo()->ucCallId,
                                                   enCause);
        break;
    case IMSA_CALL_DTMF_WAIT_STOP_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(IMSA_CallGetCurrentDtmfInfo()->usClientId,
                                                  IMSA_CallGetCurrentDtmfInfo()->ucSpmOpId,
                                                  IMSA_CallGetCurrentDtmfInfo()->ucCallId,
                                                  enCause);
        break;
    case IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        break;
    case IMSA_CALL_DTMF_WAIT_TIME_OUT:
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfDurationTimer);
        break;
    default:
        IMSA_INFO_LOG("IMSA_CallSrvccSuccClearDtmfInfo: nothing process on this state");
        break;
    }

    IMSA_MEM_SET(IMSA_CallGetCurrentDtmfInfo(), 0, sizeof(IMSA_CALL_DTMF_INFO_STRU));

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    IMSA_CallGetDtmfCnt() = 0;

    return;
}



/*****************************************************************************
 Function Name  : IMSA_CallRcvImsStartDtmfOk
 Description    : start dtmf成功消息处理
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.lihong 0015001     2013-10-14  Draft Enact
*****************************************************************************/
VOS_VOID  IMSA_CallRcvImsStartDtmfOk( VOS_VOID )
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo  = VOS_NULL_PTR;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();

    VOS_UINT32                          ulResult = VOS_FALSE;

    /* 判断状态是否异常 */
    if (IMSA_CallGetDtmfState() != IMSA_CALL_DTMF_WAIT_START_CNF)
    {
        IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfOk: Dtmf State Error!");
        return;
    }

    /* 停止DTMF保护定时器 */
    IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);

    /* 回复DTMF正式响应事件 */
    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg( pstDtmfCurInfo->usClientId,
                                                pstDtmfCurInfo->ucSpmOpId,
                                                pstDtmfCurInfo->ucCallId,
                                                TAF_CS_CAUSE_SUCCESS);

    /* 如果有缓存且为STOP_DTMF请求，则直接发送 */
    if(IMSA_CallGetDtmfCnt() > 0)
    {
        /* 判断是否有合适的CALL来传输DTMF音 */
        ulResult = IMSA_CallCheckUserDtmfCallId(0);
        if(TAF_CS_CAUSE_SUCCESS != ulResult)
        {
            IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfOk: Call Check User Dtmf Fail!");

            IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        if(0 == IMSA_CallGetDtmfBufList(0).usOnLength)
        {
            /* 给IMS发送STOP DTMF */
            if (VOS_TRUE != IMSA_CallSendImsMsgStopDtmf(0, IMSA_CallGetDtmfBufList(0).cKey))
            {
                IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfOk: Send Stop Dtmf Fail!");

                IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
                return;
            }

            /* 发送成功，启动保护定时器，更新状态*/
            IMSA_StartTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
            IMSA_CallSetDtmfState(IMSA_CALL_DTMF_WAIT_STOP_CNF);
            IMSA_MEM_CPY(IMSA_CallGetCurrentDtmfInfo(),
                         &IMSA_CallGetDtmfBufList(0),
                         sizeof(IMSA_CALL_DTMF_INFO_STRU));
            IMSA_CallGetCurrentDtmfInfo()->ulImsOpid = IMSA_GetImsOpId();

            /* 删除这条STOP DTMF缓存*/
            IMSA_CallDeleteSavedDtmfInfo(0);
            return;
        }
    }

    /* 没有缓存，或缓存为START_DTMF请求，则启动dtmf duration定时器，更改DTMF状态 */
    pstCallCtx->stDtmfCtx.stDtmfDurationTimer.ulTimerLen = (VOS_UINT32)pstDtmfCurInfo->usOnLength;
    IMSA_StartTimer(&pstCallCtx->stDtmfCtx.stDtmfDurationTimer);
    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_WAIT_TIME_OUT);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_CallRcvImsStopDtmfOk
 Description    : stop dtmf成功消息处理
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.lihong 0015001     2013-10-14  Draft Enact
*****************************************************************************/
VOS_VOID  IMSA_CallRcvImsStopDtmfOk( VOS_VOID )
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo  = VOS_NULL_PTR;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_DTMF_STATE_ENUM_UINT8     enDtmfState     = IMSA_CALL_DTMF_STATE_BUTT;

    VOS_UINT32                          ulResult = VOS_FALSE;

    /* 判断状态是否异常 */
    enDtmfState = IMSA_CallGetDtmfState();
    if ( (enDtmfState != IMSA_CALL_DTMF_WAIT_STOP_CNF)
      && (enDtmfState != IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF) )
    {
        IMSA_WARN_LOG("IMSA_CallRcvImsStopDtmfOk: Dtmf State Error!");
        return;
    }

    /* 停止DTMF保护定时器 */
    IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);

    /* 如果是用户发出的STOP_DTMF需要回复正式响应事件 */
    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    if (enDtmfState != IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF)
    {
        (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(  pstDtmfCurInfo->usClientId,
                                                    pstDtmfCurInfo->ucSpmOpId,
                                                    pstDtmfCurInfo->ucCallId,
                                                    TAF_CS_CAUSE_SUCCESS);
    }

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    /* 处理缓存，待三阶段实现 */
    if(IMSA_CallGetDtmfCnt() > 0)
    {
        /* 判断缓存的第一条是STOP DTMF*/
        if(0 == IMSA_CallGetDtmfBufList(0).usOnLength)
        {
            (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(IMSA_CallGetDtmfBufList(0).usClientId,
                                                      IMSA_CallGetDtmfBufList(0).ucSpmOpId,
                                                      IMSA_CallGetDtmfBufList(0).ucCallId,
                                                      TAF_CS_CAUSE_SUCCESS);

            /* 删除这条STOP DTMF缓存 */
            IMSA_CallDeleteSavedDtmfInfo(0);
        }

        /* 判断是否还有缓存 */
        if(0 == IMSA_CallGetDtmfCnt())
        {
            return;
        }

        /* 缓存的第一条是STRAT DTMF */
        ulResult = IMSA_CallCheckUserDtmfCallId(0);
        if(TAF_CS_CAUSE_SUCCESS != ulResult)
        {
            IMSA_WARN_LOG("IMSA_CallRcvImsStopDtmfOk: Call Check User Dtmf Fail!");
            IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        if (VOS_TRUE != IMSA_CallSendImsMsgStartDtmf(IMSA_CallGetDtmfBufList(0).ucCallId,
                                                     IMSA_CallGetDtmfBufList(0).cKey,
                                                     IMSA_CallGetDtmfBufList(0).usOnLength))
        {
            IMSA_WARN_LOG("IMSA_CallRcvImsStopDtmfOk: Send Start Dtmf Fail!");

            IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        IMSA_StartTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        IMSA_CallSetDtmfState(IMSA_CALL_DTMF_WAIT_START_CNF);
        IMSA_MEM_CPY(IMSA_CallGetCurrentDtmfInfo(),
                     &IMSA_CallGetDtmfBufList(0),
                     sizeof(IMSA_CALL_DTMF_INFO_STRU));
        IMSA_CallGetCurrentDtmfInfo()->ulImsOpid = IMSA_GetImsOpId();

        /* 删除这条START DTMF缓存 */
        IMSA_CallDeleteSavedDtmfInfo(0);
    }

    return;
}
VOS_UINT32 IMSA_CallProcImsMsgOk(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    VOS_UINT32                          ulResult        = VOS_FALSE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;


    /* 如果REASON OK对应的opid检查失败，则丢弃该消息 */
    ulResult = IMSA_CallImsMsgFind(pstOutputCallEvent->ulOpId);
    if (IMSA_CALL_FIND_FAIL == ulResult)
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgOk: not found op id", pstOutputCallEvent->ulOpId);
        return VOS_TRUE;
    }

    if (IMSA_CALL_FIND_SUCC_DTMF == ulResult)
    {
        if (IMSA_CallGetDtmfState() == IMSA_CALL_DTMF_WAIT_START_CNF)
        {
            IMSA_CallRcvImsStartDtmfOk();
        }
        else
        {
            IMSA_CallRcvImsStopDtmfOk();
        }

        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    IMSA_StopTimer(&pstCallCtx->stProctectTimer);

    /* 根据不同命令回复SPM */
    switch (pstCallCtx->stSpmMsg.ulSpmMsgId)
    {
    case ID_SPM_IMSA_CALL_ORIG_REQ:

        /* 如果是重播时回复的OK，则不报给SPM */
        if ((VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
            && (pstCallCtx->stRedialMaxTimer.usPara < IMSA_CALL_MAX_NUM))
        {
            pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->stRedialMaxTimer.usPara];
            if (pstCallEntity->ucRedialTimes > 0)
            {
                break;
            }
        }

        /* 因为SPM 在ORIG REQ里的CALL ID为0，而IMSA在分配CALL实体也将CALL ID默认
           初始化为无效CALL ID，即0；因此在IMSA_CallSendSpmEvtAndCallInfo函数中
           就能够查找到CALL实体，从而能够正确填写CALL TYPE；在回复ORIG CNF时，
           目前SPM只需要IMSA填写正确的CALL TYPE */
        ulResult = IMSA_CallSendSpmOrigCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                              pstCallCtx->stSpmMsg.ulOpId,
                                              pstCallCtx->stSpmMsg.ulCallId,
                                              TAF_CS_CAUSE_SUCCESS);
        break;
    case ID_SPM_IMSA_CALL_SUPS_CMD_REQ:
        ulResult = IMSA_CallSendSpmSupsResultMsg(pstCallCtx->stSpmMsg.usClientId,
                                              pstCallCtx->stSpmMsg.ulOpId,
                                              pstCallCtx->stSpmMsg.ulCallId,
                                              MN_CALL_SS_RES_SUCCESS);
        break;
    case ID_SPM_IMSA_CALL_GET_CALL_INFO_REQ:
        IMSA_INFO_LOG("IMSA_CallProcImsMsgOk: this command has handled by IMSA");
        break;
    case ID_SPM_IMSA_CALL_GET_CUDR_REQ:
        IMSA_INFO_LOG("IMSA_CallProcImsMsgOk: this command is not supported by IMSA");
        break;
    /* xiongxianghui00253310 add for conference 20140210 begin */
    case ID_SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ:
        ulResult = IMSA_CallSendSpmInviteNewPtptCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                       pstCallCtx->stSpmMsg.ulOpId,
                                                       pstCallCtx->stSpmMsg.ulCallId,
                                                       TAF_CS_CAUSE_SUCCESS);
        break;
    /* xiongxianghui00253310 add for conference 20140210 end */
    default:
        ulResult = VOS_FALSE;
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgOk: invalid call request", pstCallCtx->stSpmMsg.ulSpmMsgId);
    }

    /* 清除缓存命令 */
    IMSA_CallImsMsgClear();
    IMSA_CallSpmMsgClear();

    return ulResult;
}
VOS_VOID IMSA_CallEntitySaveErrorInfo
(
    IMSA_CALL_ENTITY_STRU                  *pstCallEntity,
    const IMSA_IMS_OUTPUT_CALL_EVENT_STRU  *pstOutputCallEvent
)
{
    pstCallEntity->bitOpErrorInfo = IMSA_OP_TRUE;

    IMSA_MEM_CPY(   &pstCallEntity->stImsError,
                    &pstOutputCallEvent->stCallError,
                    sizeof(IMSA_IMS_CALL_ERROR_STRU));

    IMSA_INFO_LOG1("IMSA_CallEntitySaveErrorInfo:errorCode:", pstCallEntity->stImsError.stErrorCode.usSipStatusCode);
}

/*****************************************************************************
 Function Name  : IMSA_CallRcvImsStartDtmfError
 Description    : start dmtf失败处理
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.lihong 0015001     2013-10-14  Draft Enact
*****************************************************************************/
VOS_VOID  IMSA_CallRcvImsStartDtmfError( VOS_VOID )
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo  = VOS_NULL_PTR;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();

    VOS_UINT32                          ulResult = VOS_FALSE;

    /* 判断状态是否异常 */
    if (IMSA_CallGetDtmfState() != IMSA_CALL_DTMF_WAIT_START_CNF)
    {
        IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfError: Dtmf State Error!");
        return;
    }

    /* 停止DTMF保护定时器 */
    IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);

    /* 回复DTMF正式响应事件 */
    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg( pstDtmfCurInfo->usClientId,
                                                pstDtmfCurInfo->ucSpmOpId,
                                                pstDtmfCurInfo->ucCallId,
                                                TAF_CS_CAUSE_DTMF_REJ);

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    /* 缓存处理*/
    if(IMSA_CallGetDtmfCnt() > 0)
    {
        /* 判断缓存的第一条是STOP DTMF*/
        if(0 == IMSA_CallGetDtmfBufList(0).usOnLength)
        {
            (VOS_VOID)IMSA_CallSendSpmStopDtmfCnfMsg( IMSA_CallGetDtmfBufList(0).usClientId,
                                                      IMSA_CallGetDtmfBufList(0).ucSpmOpId,
                                                      IMSA_CallGetDtmfBufList(0).ucCallId,
                                                      TAF_CS_CAUSE_DTMF_REJ);

            /* 删除这条STOP DTMF缓存 */
            IMSA_CallDeleteSavedDtmfInfo(0);
        }

        /* 判断是否还有缓存 */
        if(0 == IMSA_CallGetDtmfCnt())
        {
            return;
        }

        /* 缓存的第一条是START DTMF */
        ulResult = IMSA_CallCheckUserDtmfCallId(0);
        if(TAF_CS_CAUSE_SUCCESS != ulResult)
        {
            IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfError: Call Check User Dtmf Fail!");
            IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        if (VOS_TRUE != IMSA_CallSendImsMsgStartDtmf(IMSA_CallGetDtmfBufList(0).ucCallId,
                                                     IMSA_CallGetDtmfBufList(0).cKey,
                                                     IMSA_CallGetDtmfBufList(0).usOnLength))
        {
            IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfError: Send Start Dtmf Fail!");

            IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        IMSA_StartTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
        IMSA_CallSetDtmfState(IMSA_CALL_DTMF_WAIT_START_CNF);
        IMSA_MEM_CPY(IMSA_CallGetCurrentDtmfInfo(),
                     &IMSA_CallGetDtmfBufList(0),
                     sizeof(IMSA_CALL_DTMF_INFO_STRU));
        IMSA_CallGetCurrentDtmfInfo()->ulImsOpid = IMSA_GetImsOpId();

        /* 删除这条START DTMF缓存 */
        IMSA_CallDeleteSavedDtmfInfo(0);
    }

    return;
}

/* lihong00150010 2014-04-21 降全复杂度 begin */

VOS_UINT32 IMSA_CallProcImsMsgErrorWhenFindOpidSucc
(
    const IMSA_IMS_OUTPUT_CALL_EVENT_STRU  *pstOutputCallEvent,
    VOS_UINT32                              ulFindOpidResult
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult        = VOS_FALSE;
    /* xiongxianghui00253310 add for conference 20140210 begin */
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;
    /* xiongxianghui00253310 add for conference 20140210 end */
    VOS_UINT16                          usRedialCallIndex = IMSA_NULL;

    if (IMSA_CALL_FIND_SUCC_DTMF == ulFindOpidResult)
    {
        if (IMSA_CallGetDtmfState() == IMSA_CALL_DTMF_WAIT_START_CNF)
        {
            IMSA_CallRcvImsStartDtmfError();
        }
        else
        {
            /* 与IMS协议栈接口约定，STOP DTMF没有失败 */
            IMSA_INFO_LOG("IMSA_CallProcImsMsgErrorWhenFindOpidSucc: stop dtmf can not error!");
        }

        return VOS_TRUE;
    }

    /* 停止保护定时器 */
    IMSA_StopTimer(&pstCallCtx->stProctectTimer);

    /* 根据不同命令回复SPM */
    switch (pstCallCtx->stSpmMsg.ulSpmMsgId)
    {
    case ID_SPM_IMSA_CALL_ORIG_REQ:

        /* 如果是重播时回复的ERROR，则打断重播流程 */
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
        {
            usRedialCallIndex   = pstCallCtx->stRedialMaxTimer.usPara;
            if (usRedialCallIndex < IMSA_CALL_MAX_NUM)
            {
                pstCallEntity = &pstCallCtx->astCallEntity[usRedialCallIndex];
                if (pstCallEntity->ucRedialTimes > 0)
                {
                    IMSA_INFO_LOG("IMSA_CallProcImsMsgError:redial orig error");
                    IMSA_CallEntitySaveErrorInfo(pstCallEntity, pstOutputCallEvent);
                    IMSA_CallInterruptRedial(pstCallEntity);
                    break;
                }
            }
        }

        ulResult = IMSA_CallSendSpmOrigCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                              pstCallCtx->stSpmMsg.ulOpId,
                                              pstCallCtx->stSpmMsg.ulCallId,
                                              TAF_CS_CAUSE_IMSA_ERROR);

        pstCallEntity = IMSA_CallEntityGetUsedByCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntityFree(pstCallEntity);
        }

        break;

    case ID_SPM_IMSA_CALL_SUPS_CMD_REQ:
        ulResult = IMSA_CallSendSpmSupsResultMsg(pstCallCtx->stSpmMsg.usClientId,
                                              pstCallCtx->stSpmMsg.ulOpId,
                                              pstCallCtx->stSpmMsg.ulCallId,
                                              MN_CALL_SS_RES_FAIL);
        break;
    /* xiongxianghui00253310 add for conference 20140210 begin */
    case ID_SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ:
        enSpmErr = IMSA_CallTransImsErr2SpmErr(pstOutputCallEvent->stCallError.stErrorCode.usSipStatusCode);
        ulResult = IMSA_CallSendSpmInviteNewPtptCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                       pstCallCtx->stSpmMsg.ulOpId,
                                                       pstCallCtx->stSpmMsg.ulCallId,
                                                       enSpmErr);

        pstCallEntity = IMSA_CallEntityGetUsedByCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntityFree(pstCallEntity);
        }
        break;
    /* xiongxianghui00253310 add for conference 20140210 end */
    default:
        ulResult = VOS_FALSE;
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgError: invalid call request", pstCallCtx->stSpmMsg.ulSpmMsgId);
    }

    /* 清除缓存命令 */
    IMSA_CallImsMsgClear();
    IMSA_CallSpmMsgClear();

    return ulResult;
}
VOS_UINT32 IMSA_CallProcImsMsgError(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult        = VOS_FALSE;
    VOS_UINT32                          ulCallId        = 0;

    /* 如果REASON ERROR对应的opid检查失败，则说明是IMS回复OK之后，网侧拒绝UE的场景；
       如果OPID查找成功，则是没有发起空口流程，IMS直接回复的ERROR */
    ulResult = IMSA_CallImsMsgFind(pstOutputCallEvent->ulOpId);
    if (IMSA_CALL_FIND_FAIL == ulResult)
    {
        ulCallId = (VOS_UINT32)pstOutputCallEvent->stCallError.ulCallIndex;
        /**
         * TODO:
         * 1. 如果是呼叫失败的场景，需要D2在REASON ERROR中也携带呼叫Id
         * 2. 之所以找不到csmid，是因为在之前收到的REASON OK中已经将保存的csmid清除
         * 3. 这里将ErrorInfo保存到每个呼叫实体考虑当有多个呼叫同时发生错误时可以正确处理，
         */
        pstCallEntity = IMSA_CallEntityGetUsedByCallId(ulCallId);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntitySaveErrorInfo(pstCallEntity, pstOutputCallEvent);
        }
        else
        {
            IMSA_ERR_LOG1("IMSA_CallProcImsMsgError: invalid err call id", ulCallId);
        }

        return VOS_TRUE;
    }

    return IMSA_CallProcImsMsgErrorWhenFindOpidSucc(pstOutputCallEvent, ulResult);
}
/* lihong00150010 2014-04-21 降全复杂度 end */

VOS_UINT32 IMSA_CallProcImsMsgIncoming(IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_NORM_LOG("IMSA_CallProcImsMsgIncoming: enterd");
#if 0
    /* 保存Incoming消息携带的Remote地址 */
    IMSA_CallSaveIncomingAddr(pstOutputCallEvent->address);
#else
    /**
     * 1. MT时因Incoming事件可能是第一个事件(无Precondition)，也可能是中间事件(有Precondition)
     * 2. Incoming事件中只包含了remote address，而该信息在其它呼叫相关事件/状态通知中也包含
     * 3. Incoming事件中无Call Id，即使发生一些异常(如Call Entity分配失败)，IMSA也无法去拒绝该呼叫
     *
     * 综上原因，Incoming事件不做任何处理。同理，Waiting事件也不做任何处理
     */
#endif

    (VOS_VOID)pstOutputCallEvent;

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallEntityProcErrorInfo380ForNormal(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_ERROR_STRU *pstImsError)
{
    (VOS_VOID)pstCallEntity;
    (VOS_VOID)pstImsError;

    /**
     * 如果普通呼叫收到380响应，如果alternative-service的type类型是emergency，则发起紧急注册
     *
     * 解法一：由IMSA内不能缓存并实现第二次呼叫的发起
     * 理论上这里是MO呼叫时才去发起，而在呼叫实体也只有MO时才保存SPM下发的呼叫发起命令
     * 所以这里再将该参数保存到呼叫管理上下文的缓存区，这样在收到紧急服务可用的通知时，
     * 可以再一次发起该呼叫(不过这里需要更改呼叫类型??)
     *
     * 解法二: IMSA以特定的呼叫结果返回给SPM(如TAF_CS_CAUSE_IMSA_RETRY_VIA_IMS_EMC)，由SPM来触发第二次呼叫
     * 这样似乎更合理干净一些，但需要和GU商量接口；
     *     - 这种解法的另外一个考虑就是，把这种场景看成重播，而重播是放在SPM实现
     *     - SPM已经有通过CS域重播的机制，再加一种通过IMS域紧急呼叫的方式重播，修改应该只是复用现有的机制
     */

    /* 目前先按解决二处理 */

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallEntityProcErrorInfo380ForEmc
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    const IMSA_IMS_CALL_ERROR_STRU     *pstImsError
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    switch (pstCallEntity->enEmcType)
    {
        case IMSA_EMC_CALL_TYPE_IN_NORMAL_SRV:

            /* 根据Action的值来处理 */
            if (IMSA_IMS_3GPP_TYPE_ACTION_EMERGENCY_REG == pstImsError->stErrorCode.usIms3gppAction)
            {
                /* 由于CALL MANAGER中的缓存SPM消息在收到OK时就清除了，所以这里需要
                   将CALL实体中缓存的SPM消息拷贝至CALL MANAGER中 */
                IMSA_MEM_CPY(   &pstCallCtx->stSpmMsg,
                                &pstCallEntity->stSpmMsg,
                                sizeof(IMSA_CALL_SPM_MSG_STRU));

                /* 通知主控模块进行紧急注册 */
                (VOS_VOID)IMSA_CallSendIntraResultAction(IMSA_CALL_TYPE_EMC, IMSA_RESULT_ACTION_REG_WITH_FIRST_ADDR_PAIR);

                /* 不发RELEASED事件，不清除CALL实体，待IMS紧急注册成功后再尝试紧急呼 */
                return VOS_FALSE;
            }

            break;
        case IMSA_EMC_CALL_TYPE_EMC_CONN_EMC_REG:

            if ((VOS_FALSE == pstCallEntity->bitOpRetryCsCall)
                && (IMSA_IMS_3GPP_TYPE_ACTION_EMERGENCY_REG == pstImsError->stErrorCode.usIms3gppAction))
            {
                /* 由于CALL MANAGER中的缓存SPM消息在收到OK时就清除了，所以这里需要
                   将CALL实体中缓存的SPM消息拷贝至CALL MANAGER中 */
                IMSA_MEM_CPY(   &pstCallCtx->stSpmMsg,
                                &pstCallEntity->stSpmMsg,
                                sizeof(IMSA_CALL_SPM_MSG_STRU));

                /* 通知主控模块尝试使用下一组参数进行注册 */
                (VOS_VOID)IMSA_CallSendIntraResultAction(pstCallEntity->enType, IMSA_RESULT_ACTION_REG_WITH_NEXT_ADDR_PAIR);

                /* 不发RELEASED事件，不清除CALL实体，待更换参数重新IMS紧急注册成功后再尝试紧急呼 */
                return VOS_FALSE;
            }
            break;

        default:

            IMSA_ERR_LOG1("IMSA_CallEntityProcErrorInfo380ForEmc: invalid emergency type", pstCallEntity->enEmcType);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallEntityProcErrorInfo380(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_ERROR_STRU *pstImsError)
{
    VOS_UINT32 ulResult = VOS_TRUE;

    switch (pstCallEntity->enType)
    {
    case IMSA_CALL_TYPE_NORMAL:
        ulResult = IMSA_CallEntityProcErrorInfo380ForNormal(pstCallEntity, pstImsError);
        break;
    case IMSA_CALL_TYPE_EMC:
        ulResult = IMSA_CallEntityProcErrorInfo380ForEmc(pstCallEntity, pstImsError);
        break;
    default:
        ulResult = VOS_TRUE;
        IMSA_ERR_LOG1("IMSA_CallEntityProcErrorInfo380: invalid call type", pstCallEntity->enType);
    }

    return ulResult;
}


VOS_UINT32 IMSA_CallEntityProcErrorInfo503(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_ERROR_STRU *pstImsError)
{
    (VOS_VOID)pstCallEntity;
    (VOS_VOID)pstImsError;

    /**
     * 如果503响应中包含Retry-After，应该通知SPM，在普通错误通知中作为参数报上去吗？
     * 这里需要和GU讨论接口
     *
     * 暂时先不实现此功能
     */

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallEntityProcErrorInfo504(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_ERROR_STRU *pstImsError)
{
    (VOS_VOID)pstCallEntity;

    if (IMSA_IMS_3GPP_TYPE_ACTION_RESTORATION_INIT_REG == pstImsError->stErrorCode.usIms3gppAction)
    {
        /* 通知Service进行Restoration操作 */
        (VOS_VOID)IMSA_CallSendIntraResultAction(pstCallEntity->enType, IMSA_RESULT_ACTION_REG_RESTORATION);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallEntityProcErrorInfo504: not defined procedure");
    }

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallEntityProcErrorInfo(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_ERROR_STRU *pstImsError)
{
    VOS_UINT32 ulResult = VOS_TRUE;

    IMSA_INFO_LOG1("IMSA_CallProcErrorInfo: process error code", pstImsError->stErrorCode.usSipStatusCode);

    switch (pstImsError->stErrorCode.usSipStatusCode)
    {
    /* 380 */
    case IMSA_SIP_NW_ERROR_CAUSE_ALTERNATIVE_SERVICE:
        ulResult = IMSA_CallEntityProcErrorInfo380(pstCallEntity, pstImsError);
        break;
    /* 503 */
    case IMSA_SIP_NW_ERROR_CAUSE_SERVICE_UNAVAILABLE:
        ulResult = IMSA_CallEntityProcErrorInfo503(pstCallEntity, pstImsError);
        break;
    /* 504 */
    case IMSA_SIP_NW_ERROR_CAUSE_SERVER_TIMEOUT:
        ulResult = IMSA_CallEntityProcErrorInfo504(pstCallEntity, pstImsError);
        break;
    default:
        break;
    }

    return ulResult;
}
TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallTransImsErr2SpmErr
(
    VOS_UINT16                          usImsErr
)
{
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr = TAF_CS_CAUSE_BUTT;

    IMSA_INFO_LOG1("IMSA_CallTransImsErr2SpmErr: ims error code:", usImsErr);

    if ((usImsErr < 300) || (usImsErr > 699))
    {
        enSpmErr = TAF_CS_CAUSE_IMSA_ERROR;

        return enSpmErr;
    }

    enSpmErr = usImsErr + TAF_CS_CAUSE_IMS_BEGIN;

    if (enSpmErr >= TAF_CS_CAUSE_IMSA_BEGIN)
    {
        IMSA_ERR_LOG("IMSA_CallTransImsErr2SpmErr:ims err is not illegal");
        enSpmErr = TAF_CS_CAUSE_IMS_FORBIDDEN;
    }

    return enSpmErr;
}



VOS_VOID IMSA_CallReleaseCallCommonProc
(
    IMSA_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT32                          i           = IMSA_NULL;

    /* 如果此CALL是未上报过INCOMING事件给SPM的MT CALL（即IsUsed为TRUE，
       但状态为IDLE，方向为MT），则不需要上报releasd事件 */
    if ((IMSA_CALL_STATUS_IDLE == pstCallEntity->enStatus)
        && (MN_CALL_DIR_MT == pstCallEntity->enDir))
    {
        IMSA_INFO_LOG("IMSA_CallReleaseCallCommonProc:MT,IDLE");
        IMSA_CallEntityFree(pstCallEntity);

        return ;
    }

    /*如果SRVCC流程，该电话已转到CS域，则不上报给SPM，但是要释放call信息*/
    if(pstCallEntity->bitOpTransToCs == VOS_TRUE)
    {
        IMSA_INFO_LOG("IMSA_CallReleaseCallCommonProc:Call transfered to CS.");
        IMSA_CallEntityFree(pstCallEntity);

        return;
    }

    (VOS_VOID)IMSA_CallSendSpmReleasedEvt(  pstCallEntity,
                                            ulResult);

    IMSA_CallEntityFree(pstCallEntity);

    /* 如果不存在CALL（即IsUsed都为FALSE），或者只存在未上报INCOMING事件给SPM的
       MT CALL（即IsUsed为TRUE，但状态为IDLE，方向为MT），则上报
       MN_CALL_EVT_ALL_RELEASED事件 */
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_FALSE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
          ||((IMSA_CALL_STATUS_IDLE == pstCallCtx->astCallEntity[i].enStatus)
                && (MN_CALL_DIR_MT == pstCallCtx->astCallEntity[i].enDir)))
        {
            continue;
        }

        break;
    }

    if (IMSA_CALL_MAX_NUM == i)
    {
        (VOS_VOID)IMSA_CallSendSpmAllReleasedEvt();
    }
}

/*****************************************************************************
 Function Name  : IMSA_CallClearLocalAlertInfo
 Description    : 启动HIFI后，清除本地振铃信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-10-11  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallClearLocalAlertInfo(VOS_VOID )
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_FALSE== pstCallCtx->astCallEntity[i].bitOpEarlyMedia))
        {
            IMSA_INFO_LOG("IMSA_CallClearLocalAlertInfo: Clear Local Alert Info!");

            pstCallCtx->astCallEntity[i].bitOpEarlyMedia = VOS_TRUE;
        }
    }

}


VOS_UINT32 IMSA_CallIsNeedCsRedialSrvccSucc
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity
)
{
    if (IMSA_TRUE != IMSA_CallGetNotReportAllReleasedFlag())
    {
        return IMSA_FALSE;
    }

    if (MN_CALL_DIR_MO != pstCallEntity->enDir)
    {
        return IMSA_FALSE;
    }

    if ((IMSA_CALL_STATUS_DIALING != pstCallEntity->enStatus)
        && (IMSA_CALL_STATUS_TRYING != pstCallEntity->enStatus))
    {
        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


VOS_UINT32 IMSA_CallFindCauseInRedialCauseList
(
    VOS_UINT16                          usSipStatusCode
)
{
    IMSA_IMS_REDIAL_CFG_STRU           *pstImsRedialCfg = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    pstImsRedialCfg = IMSA_GetImsRedialCfgAddress();

    for (i = 0; i < pstImsRedialCfg->ucRedialCauseNum; i++)
    {
        if (usSipStatusCode == pstImsRedialCfg->ausRedialCauseList[i])
        {
            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}


VOS_UINT32 IMSA_CallIsNeedRedial
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    VOS_UINT16                          usSipStatusCode
)
{
    IMSA_IMS_REDIAL_CFG_STRU           *pstImsRedialCfg         = VOS_NULL_PTR;
    VOS_UINT32                          ulRedialMaxTimeRemainLen= IMSA_NULL;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx              = IMSA_CallCtxGet();

    /* IMS重播功能未打开，则不重播 */
    pstImsRedialCfg = IMSA_GetImsRedialCfgAddress();
    if (IMSA_FALSE == pstImsRedialCfg->ucIsCallRedialSupportFlg)
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: ims redial not support");
        return IMSA_FALSE;
    }

    /* 如果正在释放，则不重播 */
    if (VOS_TRUE == pstCallEntity->stDiscDir.ucDiscCallFlag)
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: releasing");
        return IMSA_FALSE;
    }

    /* 如果不是MO，则不重播 */
    if (MN_CALL_DIR_MO != pstCallEntity->enDir)
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: not mo");
        return IMSA_FALSE;
    }

    /* 状态不为DIALING和TRYING，则不重播 */
    if ((IMSA_CALL_STATUS_DIALING != pstCallEntity->enStatus)
        && (IMSA_CALL_STATUS_TRYING != pstCallEntity->enStatus))
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: call state is not dailing or trying");
        return IMSA_FALSE;
    }

    /* 不在重播错误原因值列表中，则不重播 */
    if (IMSA_FAIL == IMSA_CallFindCauseInRedialCauseList(usSipStatusCode))
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: not in redial cause list");
        return IMSA_FALSE;
    }

    /* 重播次数已达最大次数，则不重播 */
    if (pstCallEntity->ucRedialTimes >= pstImsRedialCfg->ulCallRedialMaxNum)
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: redial times is already max");
        return IMSA_FALSE;
    }

    /* 获取当前定时器的剩余时间 */
    IMSA_GetRemainTimeLen(&pstCallCtx->stRedialMaxTimer, &ulRedialMaxTimeRemainLen);

    IMSA_INFO_LOG1("Get redial max timer remain time tick:", PS_GET_TICK());

    IMSA_INFO_LOG1("redial max timer length:", pstCallCtx->stRedialMaxTimer.ulTimerLen);

    IMSA_INFO_LOG1("remain time:", ulRedialMaxTimeRemainLen);

    /* 如果剩余重播时间小于重播间隔，则不重播 */

    if (ulRedialMaxTimeRemainLen < pstImsRedialCfg->ulCallRedialInterval)
    {
        IMSA_INFO_LOG("IMSA_CallIsNeedRedial: remain time < intervel time");
        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


VOS_UINT32 IMSA_CallProcImsMsgDisconnect
(
    const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent
)
{
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult        = VOS_TRUE;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_IMS_REDIAL_CFG_STRU           *pstImsRedialCfg = VOS_NULL_PTR;

    pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
    if (VOS_NULL_PTR == pstCallEntity)
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgDisconnect: not found call entity ",
            pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);

        return VOS_TRUE;
    }

    if (IMSA_FALSE != pstCallEntity->bitOpErrorInfo)
    {
        ulResult = IMSA_CallEntityProcErrorInfo(    pstCallEntity,
                                                    &pstCallEntity->stImsError);

        /* 根据返回值，决定是否向SPM发released事件 */
        if (VOS_TRUE != ulResult)
        {
            IMSA_INFO_LOG("IMSA_CallProcImsMsgDisconnect:Not report released event to spm!");
            return VOS_TRUE;
        }

        /* 判定是否需要重播 */
        if (IMSA_TRUE == IMSA_CallIsNeedRedial( pstCallEntity,
                                                pstCallEntity->stImsError.stErrorCode.usSipStatusCode))
        {
            IMSA_INFO_LOG("IMSA_CallProcImsMsgDisconnect:redial!");

            pstImsRedialCfg = IMSA_GetImsRedialCfgAddress();
            pstCallCtx->stRedialIntervelTimer.ulTimerLen = pstImsRedialCfg->ulCallRedialInterval;

            /* 索引和CALL ID都得放到para中，第8位放索引，高8位放CALL ID；
               记录CALL ID信息，为了防止在间隔定时器启动时，用户下释放这个CALL的动作 */
            pstCallCtx->stRedialIntervelTimer.usPara = pstCallEntity->ucId << IMSA_MOVEMENT_8;
            pstCallCtx->stRedialIntervelTimer.usPara |= pstCallEntity->ucCallEntityIndex;
            IMSA_StartTimer(&pstCallCtx->stRedialIntervelTimer);

            /* 清除CALL ID，否则重播时IMS上报DAILING状态查找CALL实体时会查找不到;
               而且重播对于IMS不可见，因此IMS可能将重播的CALL ID重新分配给INCOMING
               CALL，如果这里不清CALL ID，则INCOMING CALL就直接能找到CALL实体，
               被误认为preconditon场景；
               但这里清除了CALL ID，则需要在打断重播流程时，即在IMSA_CallInterruptRedial
               函数中重新将间隔定时器中记录的CALLID写回CALL实体中，才能保证上报
               RELEASD事件时填写的CALL ID正确 */
            pstCallEntity->ucId = IMSA_CALL_INVALID_ID;

            return VOS_TRUE;
        }

        /* 将IMS错误原因值转化为SPM错误原因值 */
        enSpmErr = IMSA_CallTransImsErr2SpmErr(pstCallEntity->stImsError.stErrorCode.usSipStatusCode);

        IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

        if (IMSA_CallEntityGetUsedCount() == 0)
        {
            IMSA_SrvccSuccBuffProc();
        }
    }
    else
    {
        /* 如果是SRVCC成功，且是MO CALL，且当前转该为DIALING或TRYING，
           则将原因值填为SRVCC_SUCC，由SPM决定是否到CS域重播 */
        if (IMSA_TRUE == IMSA_CallIsNeedCsRedialSrvccSucc(pstCallEntity))
        {
            enSpmErr = TAF_CS_CAUSE_IMSA_SRVCC_SUCC;
        }
        else
        {
            enSpmErr = TAF_CS_CAUSE_SUCCESS;
        }

        IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

        if (IMSA_CallEntityGetUsedCount() == 0)
        {
            IMSA_SrvccSuccBuffProc();
        }
    }

    /* 清除DTMF信息 */
    if(TAF_CS_CAUSE_SUCCESS != IMSA_CallCheckUserDtmfCallId(0))
    {
        IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_CALL_RELEASE);
    }

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallProcImsMsgWaiting(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
#if 0
    /* 保存Waiting消息携带的Remote地址 */
    IMSA_CallSaveWaitingAddr(pstOutputCallEvent->address);
#else
    /**
     * 同Incoming事件的处理，#IMSA_CallProcImsMsgIncomings
     */
#endif

    (VOS_VOID)pstOutputCallEvent;

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcImsMsgCallEarlyMedia(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;


    pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
    if (pstCallEntity)
    {
        pstCallEntity->bitOpEarlyMedia = IMSA_OP_TRUE;
    }
    else
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgCallEarlyMedia: not found call entity ",
            pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcImsMsgInitializing(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT32                          ulResult = VOS_FALSE;
    IMSA_CALL_ENTITY_STRU              *pstRedailCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;


    pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);

    /* 如果找到对应的呼叫实体，则是MO呼叫的INITIALIZING事件 */
    if (pstCallEntity)
    {
        pstCallEntity->ulResRspOpId = pstOutputCallEvent->ulOpId;

        /* 暂不实现承载修改场景下的precondition流程，只要是媒体承载存在就认为
           资源已具备，用于解决媒体承载建立比ims流程快的场景 */
        if (((IMSA_TRUE == pstCallCtx->ulIsMediaPdpReady) && (IMSA_CALL_TYPE_NORMAL == pstCallEntity->enType))
            || ((IMSA_TRUE == pstCallCtx->ulIsEmcMediaPdpReady) && (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)))
        {
            IMSA_INFO_LOG("IMSA_CallProcImsMsgInitializing:call already exist,resource already!");

            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallEntity->ucId,
                                                pstCallEntity->ulResRspOpId,
                                                VOS_TRUE);
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CallProcImsMsgInitializing:call already exist,wati for resource!");

            /* 设置呼叫对应的资源标志位 */
            pstCallEntity->bitOpResReady = IMSA_FALSE;

            /* 刷新等待资源建立定时器 */
            IMSA_RegTimerRefresh(&pstCallCtx->stResReadyTimer, pstCallEntity->ucCallEntityIndex);
        }

        ulResult = VOS_TRUE;
    }
    /* 如果找不到对应的呼叫实体，则是MT呼叫的INITIALIZING事件 */
    else
    {
        pstCallEntity = IMSA_CallEntityAlloc();
        if (pstCallEntity)
        {
            /* 如果存在重拨流程，则打断重拨流程 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
            {
                ucRedialCallIndex   = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
                if (ucRedialCallIndex < IMSA_CALL_MAX_NUM)
                {
                    pstRedailCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                    IMSA_CallInterruptRedial(pstRedailCallEntity);
                }
            }

            /* 更新呼叫实体 */
            IMSA_CallEntityUpdateByCallSummary(pstCallEntity, &(pstOutputCallEvent->stCallReport.astCalls[0]));

            pstCallEntity->ucId = (VOS_UINT8)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex;
            pstCallEntity->ulResRspOpId = pstOutputCallEvent->ulOpId;

            /* 只要是媒体承载存在就认为资源已具备，用于解决媒体承载建立比ims流程快的场景 */
            if (IMSA_TRUE == pstCallCtx->ulIsMediaPdpReady)
            {
                IMSA_INFO_LOG("IMSA_CallProcImsMsgInitializing:call not exist,resource already!");

                (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallEntity->ucId,
                                                    pstCallEntity->ulResRspOpId,
                                                    VOS_TRUE);
            }
            else
            {
                IMSA_INFO_LOG("IMSA_CallProcImsMsgInitializing:call not exist,wait for resource!");

                /* 设置呼叫对应的资源标志位 */
                pstCallEntity->bitOpResReady = IMSA_FALSE;

                /* 刷新等待资源建立定时器 */
                IMSA_RegTimerRefresh(&pstCallCtx->stResReadyTimer, pstCallEntity->ucCallEntityIndex);
            }

            ulResult = VOS_TRUE;
        }
        else
        {
            /* 如果分配呼叫实体失败，则拒绝该呼叫 */
            (VOS_VOID)IMSA_CallImsCmdRel((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
        }
    }

    return ulResult;
}

/*****************************************************************************
 Function Name  : IMSA_CallImsSrvccState2TafCallState
 Description    : SRVCC呼叫状态到IMSA呼叫状态的映射
 Input          : enCallState   呼叫状态
 Output         :
 Return Value   : 映射的新状态，如果没有对应项，则原状态不变

 History        :
      1.sunbing 49683      2013-10-12  Draft Enact
*****************************************************************************/
CALL_IMSA_SRVCC_CALL_STATE_ENUM_UINT8 IMSA_CallImsSrvccState2TafCallState
(
    IMSA_IMS_CALL_STATE_ENUM_UINT8  enCallState
)
{
    CALL_IMSA_SRVCC_CALL_STATE_ENUM_UINT8  enMnCallState = 0;

    switch(enCallState)
    {
        case IMSA_IMS_CALL_STATE_ACTIVE:
            enMnCallState = CALL_IMSA_SRVCC_CALL_ACTIVE;
            break;

        case IMSA_IMS_CALL_STATE_HOLD:
            enMnCallState = CALL_IMSA_SRVCC_CALL_HELD;
            break;
        case IMSA_IMS_CALL_STATE_TRYING:
        case IMSA_IMS_CALL_STATE_DIALING:
            enMnCallState = CALL_IMSA_SRVCC_CALL_DIALING;
            break;

        case IMSA_IMS_CALL_STATE_ALERTING:
            enMnCallState = CALL_IMSA_SRVCC_CALL_ALERTING;
            break;

        case IMSA_IMS_CALL_STATE_INCOMING:
            enMnCallState = CALL_IMSA_SRVCC_CALL_INCOMING;
            break;

        case IMSA_IMS_CALL_STATE_WAITING:
            enMnCallState = CALL_IMSA_SRVCC_CALL_WAITING;
            break;

        case IMSA_IMS_CALL_STATE_ACTIVING:
            enMnCallState = CALL_IMSA_SRVCC_CALL_ACTVING;
            break;

        case IMSA_IMS_CALL_STATE_INITIALIZING:
        case IMSA_IMS_CALL_STATE_INVALID:
        default:
            IMSA_ERR_LOG1("IMSA_CallImsSrvccState2TafCallState: invalid mapping state", enCallState);

    }

    return enMnCallState;
}


/*****************************************************************************
 Function Name  : CALL_IMSA_GetSrvccCallInfo
 Description    :  IMSA提供的API，供TAF CALL模块调用。
                   CALL模块通知IMSA SRVCC启动时，CALL模块调用该函数，
                   用于获取SRVCC过程中转入CS域的呼叫信息。
                   入参内存由调用方分配，pstCallInfo指向的内存大小为CALL_IMSA_MAX_ENTITY_NUM*sizeof(CALL_IMSA_SRVCC_CALL_INFO_STRU)

 Input          : pucCallNum指向存放call个数内容，pstCallInfo指向call信息
 Output         :
 Return Value   : 0 失败，1 成功

 History        :
      1.sunbing 49683     2013-12-11  Draft Enact
*****************************************************************************/
VOS_UINT32 CALL_IMSA_GetSrvccCallInfo(VOS_UINT8* pucCallNum,
                                      CALL_IMSA_SRVCC_CALL_INFO_STRU* pstCallInfo)
{
    VOS_UINT8                               ucCount;
    IMSA_CALL_ENTITY_STRU                  *pstCallEntity     = VOS_NULL_PTR;
    IMSA_IMS_SRVCC_CALL_INFO_STRU          *pstSrvccCallInfo  = VOS_NULL_PTR;
    CALL_IMSA_SRVCC_CALL_INFO_STRU         *pstTmpCallInfo    = VOS_NULL_PTR;
    IMSA_IMS_SRVCC_CALL_LIST_STRU           stImsSrvccCallList= {0};

    if((pucCallNum == VOS_NULL_PTR)||(pstCallInfo == VOS_NULL_PTR))
    {
        IMSA_ERR_LOG("CALL_IMSA_GetSrvccCallInfo:Para is err!");
        return VOS_FALSE;
    }

    /*初始化call个数*/
    *pucCallNum = 0;

    /*从IMS协议栈获取call列表*/
    stImsSrvccCallList.ulCallNum = 0;
    (VOS_VOID)IMSA_ImsWriteSrvccCallList((void*)&stImsSrvccCallList);

    /*判断获取列表信息是否合法，不合法则获取失败*/
    if((stImsSrvccCallList.ulCallNum > IMSA_IMS_EVENT_MAX_CALL_LIST_SIZE)
        ||(stImsSrvccCallList.ulCallNum == 0))
    {
        IMSA_ERR_LOG("CALL_IMSA_GetSrvccCallInfo:call not exist!");

        return VOS_FALSE;
    }

    pstTmpCallInfo = pstCallInfo;

    /*循环处理转到CS域的call信息*/
    for(ucCount = 0; ucCount < stImsSrvccCallList.ulCallNum; ucCount++)
    {
        pstSrvccCallInfo = &stImsSrvccCallList.astCallInfo[ucCount];

        /*根据call id找到呼叫实体*/
        pstCallEntity = IMSA_CallEntityGetUsedByCallId(pstSrvccCallInfo->ulCallIndex);
        if(VOS_NULL_PTR == pstCallEntity)
        {
            IMSA_ERR_LOG("CALL_IMSA_GetSrvccCallInfo: Call info err.");
            continue;
        }

        pstTmpCallInfo->ucCallId    = (VOS_UINT8)pstSrvccCallInfo->ulCallIndex;
        pstTmpCallInfo->enCallState = \
              IMSA_CallImsSrvccState2TafCallState(pstSrvccCallInfo->enCallState);
        pstTmpCallInfo->ucTi        = pstSrvccCallInfo->ucTi;

        /*记录该call转到CS域*/
        pstCallEntity->bitOpTransToCs                       = VOS_TRUE;

        /*把call信息转成CS域需要的信息*/
        pstTmpCallInfo->clientId    = pstCallEntity->usClientId;
        pstTmpCallInfo->opId        = (VOS_UINT8)pstCallEntity->ulOpId;
        pstTmpCallInfo->enCallDir   = pstCallEntity->enDir;
        pstTmpCallInfo->enMptyState = pstCallEntity->enMpty;
        pstTmpCallInfo->enCallMode  = pstCallEntity->enMode;

        if(IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
        {
            pstTmpCallInfo->enCallType = MN_CALL_TYPE_EMERGENCY;
        }
        else
        {
            pstTmpCallInfo->enCallType = MN_CALL_TYPE_VOICE;
        }

        /*没有EarlyMedia标示，说明HIFI没有启动，或者没有网络振铃，需要本地振铃*/
        if(VOS_FALSE== pstCallEntity->bitOpEarlyMedia)
        {
            pstTmpCallInfo->ucLocalAlertedFlag = VOS_TRUE;
        }

        /* 更新主叫号码 */
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stCallNumber, \
                                   &pstTmpCallInfo->stCallNumber);

        /* 更新被叫号码 */
        IMSA_CallCalledNumberImsa2CS(&pstCallEntity->stCalledNumber, \
                                   &pstTmpCallInfo->stCalledNumber);

        /*Call个数增加，call信息指针偏移到下一个结构*/
        *pucCallNum += 1;
        pstTmpCallInfo ++;
    }

    return VOS_TRUE;
}


/*****************************************************************************
 Function Name  : IMSA_CallProcImsMsgEmergencyInd
 Description    : SRVCC过程中，IMS上报转到CS域的call信息
 Input          : pstOutputCallEvent      s输出事件
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.sunbing 49683     2013-10-11  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallProcImsMsgSrvccCallList(IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    VOS_UINT8                               ucCount;
    IMSA_CALL_ENTITY_STRU                  *pstCallEntity     = VOS_NULL_PTR;
    IMSA_IMS_SRVCC_CALL_INFO_STRU          *pstSrvccCallInfo  = VOS_NULL_PTR;
    CALL_IMSA_SRVCC_CALL_INFO_NOTIFY_STRU  *pstCallInfoNotify = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallProcImsMsgSrvccCallList is entered!");

    if((pstOutputCallEvent->stSrvccCallList.ulCallNum > IMSA_IMS_EVENT_MAX_CALL_LIST_SIZE)
       ||(pstOutputCallEvent->stSrvccCallList.ulCallNum == 0))
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgSrvccCallList: Error Call Num. ",
            pstOutputCallEvent->stSrvccCallList.ulCallNum);
        return;
    }

    pstCallInfoNotify = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(CALL_IMSA_SRVCC_CALL_INFO_NOTIFY_STRU));

    if (VOS_NULL_PTR == pstCallInfoNotify)
    {
        IMSA_ERR_LOG("IMSA_CallProcImsMsgSrvccCallList: alloc memory fail");
        return ;
    }

    IMSA_MEM_SET(IMSA_GET_MSG_ENTITY(pstCallInfoNotify), 0, IMSA_GET_MSG_LENGTH(pstCallInfoNotify));

    pstCallInfoNotify->ucCallNum = (VOS_UINT8)pstOutputCallEvent->stSrvccCallList.ulCallNum;

    /*循环处理转到CS域的call信息*/
    for(ucCount = 0; ucCount < pstOutputCallEvent->stSrvccCallList.ulCallNum; ucCount++)
    {
        pstSrvccCallInfo = &pstOutputCallEvent->stSrvccCallList.astCallInfo[ucCount];

        /*根据call id找到呼叫实体*/
        pstCallEntity = IMSA_CallEntityGetUsedByCallId(pstSrvccCallInfo->ulCallIndex);
        if(VOS_NULL_PTR == pstCallEntity)
        {
            IMSA_ERR_LOG("IMSA_CallProcImsMsgSrvccCallList: Call info err.");
            continue;
        }

        pstCallInfoNotify->astCallInfo[ucCount].ucCallId    = (VOS_UINT8)pstSrvccCallInfo->ulCallIndex;
        pstCallInfoNotify->astCallInfo[ucCount].enCallState = \
              IMSA_CallImsSrvccState2TafCallState(pstSrvccCallInfo->enCallState);
        pstCallInfoNotify->astCallInfo[ucCount].ucTi        = pstSrvccCallInfo->ucTi;

        /*记录该call转到CS域*/
        pstCallEntity->bitOpTransToCs                       = VOS_TRUE;

        /*把call信息转成CS域需要的信息*/
        pstCallInfoNotify->astCallInfo[ucCount].clientId    = pstCallEntity->usClientId;
        pstCallInfoNotify->astCallInfo[ucCount].opId        = (VOS_UINT8)pstCallEntity->ulOpId;
        pstCallInfoNotify->astCallInfo[ucCount].enCallDir   = pstCallEntity->enDir;
        pstCallInfoNotify->astCallInfo[ucCount].enMptyState = pstCallEntity->enMpty;
        pstCallInfoNotify->astCallInfo[ucCount].enCallMode  = pstCallEntity->enMode;

        if(IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
        {
            pstCallInfoNotify->astCallInfo[ucCount].enCallType = MN_CALL_TYPE_EMERGENCY;
        }
        else
        {
            pstCallInfoNotify->astCallInfo[ucCount].enCallType = MN_CALL_TYPE_VOICE;
        }

        /*没有EarlyMedia标示，说明HIFI没有启动，或者没有网络振铃，需要本地振铃*/
        if(VOS_FALSE== pstCallEntity->bitOpEarlyMedia)
        {
            pstCallInfoNotify->astCallInfo[ucCount].ucLocalAlertedFlag = VOS_TRUE;
        }

        /* 更新主叫号码 */
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stCallNumber, \
                                   &pstCallInfoNotify->astCallInfo[ucCount].stCallNumber);

        /* 更新被叫号码 */
        IMSA_CallCalledNumberImsa2CS(&pstCallEntity->stCalledNumber, \
                                   &pstCallInfoNotify->astCallInfo[ucCount].stCalledNumber);

        /* 更新连接号码 */
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stConnectNumber, \
                                   &pstCallInfoNotify->astCallInfo[ucCount].stConnectNumber);

        /* 更新转移号码 */
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stRedirectNumber, \
                                   &pstCallInfoNotify->astCallInfo[ucCount].stRedirectNumber);
    }

    /* 赋值DTMF缓存 */
    pstCallInfoNotify->stDtmfBuffInfo.ucNum = IMSA_CallGetDtmfCnt();
    for(ucCount = 0; ucCount < IMSA_CallGetDtmfCnt(); ucCount++)
    {
        pstCallInfoNotify->stDtmfBuffInfo.astDtmf[ucCount].CallId
                = IMSA_CallGetDtmfBufList(ucCount).ucCallId;
        pstCallInfoNotify->stDtmfBuffInfo.astDtmf[ucCount].cKey
                = IMSA_CallGetDtmfBufList(ucCount).cKey;
        pstCallInfoNotify->stDtmfBuffInfo.astDtmf[ucCount].usOnLength
                = IMSA_CallGetDtmfBufList(ucCount).usOnLength;
        pstCallInfoNotify->stDtmfBuffInfo.astDtmf[ucCount].usClientId
                = IMSA_CallGetDtmfBufList(ucCount).usClientId;
        pstCallInfoNotify->stDtmfBuffInfo.astDtmf[ucCount].opId
                = IMSA_CallGetDtmfBufList(ucCount).ucSpmOpId;
    }

    /* 填写消息头 */
    IMSA_WRITE_CALL_MSG_HEAD(pstCallInfoNotify, ID_IMSA_CALL_SRVCC_CALL_INFO_NOTIFY);

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstCallInfoNotify);

}


VOS_UINT32 IMSA_CallProcImsMsgEmergencyInd(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;


    pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
    if (pstCallEntity)
    {
        pstCallEntity->enType = IMSA_CALL_TYPE_EMC;
    }
    else
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsMsgEmergencyInd: not found call entity ",
            pstOutputCallEvent->stCallReport.astCalls[0].ulCallIndex);
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallIsProccessCallSwap(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    if ((IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg) &&
        (ID_SPM_IMSA_CALL_SUPS_CMD_REQ == pstCallCtx->stSpmMsg.ulSpmMsgId) &&
        (MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH == pstCallCtx->stSpmMsg.stParam.stSupsParam.enCallSupsCmd))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_UINT32 IMSA_CallIsAllCallOnHold(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    VOS_UINT32 i = 0;

    for (i = 0; i < pstOutputCallEvent->stCallReport.ulNumCalls; i++)
    {
        if (IMSA_IMS_CALL_STATE_HOLD != pstOutputCallEvent->stCallReport.astCalls[i].enCallState)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallIsAllCallInConference(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    VOS_UINT32 i = 0;

    for (i = 0; i < pstOutputCallEvent->stCallReport.ulNumCalls; i++)
    {
        if (IMSA_IMS_CALL_SINGLE_PARTY == pstOutputCallEvent->stCallReport.astCalls[i].enMultiParty)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcImsStateInvalid(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    /* 收到Invalid状态通知不做处理 */

    (VOS_VOID)pstCallEntity;
    (VOS_VOID)pstCallSummary;

    return VOS_TRUE;
}
IMSA_CALL_STATUS_ENUM_UINT8 IMSA_CallImsState2ImsaState(IMSA_IMS_CALL_STATE_ENUM_UINT8 enImsState, IMSA_CALL_STATUS_ENUM_UINT8 enOldState)
{
    IMSA_CALL_STATUS_ENUM_UINT8 enNewState = enOldState;

    switch (enImsState)
    {
    case IMSA_IMS_CALL_STATE_ACTIVE:
        enNewState = IMSA_CALL_STATUS_ACTIVE;
        break;
    case IMSA_IMS_CALL_STATE_HOLD:
        enNewState = IMSA_CALL_STATUS_HELD;
        break;
    case IMSA_IMS_CALL_STATE_TRYING:
        enNewState = IMSA_CALL_STATUS_TRYING;
        break;
    case IMSA_IMS_CALL_STATE_DIALING:
        enNewState = IMSA_CALL_STATUS_DIALING;
        break;
    case IMSA_IMS_CALL_STATE_ALERTING:
        enNewState = IMSA_CALL_STATUS_ALERTING;
        break;
    case IMSA_IMS_CALL_STATE_INCOMING:
        enNewState = IMSA_CALL_STATUS_INCOMING;
        break;
    case IMSA_IMS_CALL_STATE_WAITING:
        enNewState = IMSA_CALL_STATUS_WAITING;
        break;
    case IMSA_IMS_CALL_STATE_INVALID:
    case IMSA_IMS_CALL_STATE_INITIALIZING:
    default:
        IMSA_ERR_LOG1("IMSA_CallImsState2ImsaState: invalid mapping state", enImsState);
    }

    return enNewState;
}



VOS_UINT32 IMSA_CallProcImsStateActive(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    IMSA_CALL_STATUS_ENUM_UINT8         enPreStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8         enNewStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();


    /**
     * 如果没有找到对应的呼叫实体，则挂断该呼叫
     * 这种场景下，是否需要保存IMS的呼叫并启动保护定时器？
     * 目前先简单来做，不保存，不启动保护定时器
     */

    /* 如果没有找到对应的呼叫实体，直接返回失败 */
    if (VOS_NULL_PTR == pstCallEntity)
    {
        return VOS_FALSE;
    }

    /* 如果重拨的CALL进入ACTIVE状态，则停止重拨相关定时器 */
    if ((VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
        && (pstCallCtx->stRedialMaxTimer.usPara < IMSA_CALL_MAX_NUM))
    {
        if (pstCallEntity == &pstCallCtx->astCallEntity[pstCallCtx->stRedialMaxTimer.usPara])
        {
            IMSA_INFO_LOG("IMSA_CallProcImsStateActive stop redial timer");

            /* 需要清错误原因值，否则CALL释放时上报RELEASE事件时携带原因值会出错 */
            pstCallEntity->bitOpErrorInfo = IMSA_OP_FALSE;
            IMSA_MEM_SET(   &pstCallEntity->stImsError,
                            0,
                            sizeof(IMSA_IMS_CALL_ERROR_STRU));
            pstCallEntity->ucRedialTimes = 0;

            IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
            IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);
        }
    }

    /* 保存之前的状态 */
    enPreStatus = pstCallEntity->enStatus;

    /* IMS上报CALL状态为ACTIVE和ACTIVING状态时，都会进到该函数下处理；IMS上报ACTIVING状态时，需要直接当做
    ACTIVE处理，因此在状态转换时，入参直接传入IMSA_IMS_CALL_STATE_ACTIVE */
    enNewStatus = IMSA_CallImsState2ImsaState(IMSA_IMS_CALL_STATE_ACTIVE, enPreStatus);

    if (enPreStatus != enNewStatus)
    {
        /* 更新呼叫状态 */
        pstCallEntity->enStatus = enNewStatus;

        /* xiongxianghui00253310 modify for conference 20140220 begin */
        IMSA_CallEntityUpdateByCallSummary(pstCallEntity, pstCallSummary);
        /* xiongxianghui00253310 modify for conference 20140220 end */

        IMSA_INFO_LOG1("IMSA_CallProcImsStateActive: MptyState1", pstCallEntity->enMpty);

        /* 如果原状态是HELD，则通知SPM Retrieve事件 */
        if (enPreStatus == IMSA_CALL_STATUS_HELD)
        {
            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_RETRIEVE);
        }
        /* 如果状态发生变化，且原状态不是Active，则通知SPM CONNECT事件 */
        else
        {
            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_CONNECT);
        }
    }
    /* xiongxianghui00253310 modify for conference 20140220 begin */
    else
    {
        /* 多方会议拉人，原状态是ACTIVE，新状态仍是ACTIVE，只更新IMSA内部呼叫实体 */
        if (IMSA_IMS_CALL_CONFERENCE == pstCallSummary->enMultiParty)
        {
            IMSA_CallEntityUpdateByCallSummary(pstCallEntity, pstCallSummary);

            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_CONNECT);
        }
    }
    /* xiongxianghui00253310 modify for conference 20140220 end */

    return VOS_TRUE;
}



VOS_UINT32 IMSA_CallAllStateWillBeHeld(VOS_UINT32 ulWillChangedId)
{
    VOS_UINT32 ulUsedCallCount = 0;
    VOS_UINT32 ulHeldCallCount = 0;
    VOS_UINT32 ulNonHeldCallId = 0;
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    /* 遍历所有呼叫并统计信息 */
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            ulUsedCallCount++;
            if (IMSA_CALL_STATUS_HELD == pstCallCtx->astCallEntity[i].enStatus)
            {
                ulHeldCallCount++;
            }
            else
            {
                ulNonHeldCallId = i;
            }
        }
    }

    /**
     * 如果可用呼叫和处于HELD状态呼叫数相差1，
     * 且相差的这个呼叫Id刚好等于将要变更状态的Id，则满足条件；否则返回失败
     */
    if ((ulUsedCallCount > 1) &&
        (ulUsedCallCount == ulHeldCallCount + 1) &&
        (ulNonHeldCallId == ulWillChangedId))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_UINT32 IMSA_CallProcImsStateHeld(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    IMSA_CALL_STATUS_ENUM_UINT8 enPreStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8 enNewStatus = IMSA_CALL_STATUS_BUTT;


    /* 如果没有找到对应的呼叫实体，直接返回失败 */
    if (VOS_NULL_PTR == pstCallEntity)
    {
        return VOS_FALSE;
    }

    enPreStatus = pstCallEntity->enStatus;
    enNewStatus = IMSA_CallImsState2ImsaState(pstCallSummary->enCallState, enPreStatus);

    /* 如果原状态和新状态不一致，则通知SPM HELD事件 */
    if (enPreStatus != enNewStatus)
    {
        /* 更新呼叫状态 */
        pstCallEntity->enStatus = enNewStatus;

        (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                            pstCallEntity->usClientId,
                                            pstCallEntity->ulOpId,
                                            MN_CALL_EVT_HOLD);
    }

    if(IMSA_CALL_STATUS_HELD == pstCallEntity->enStatus)
    {
        /* 清除DTMF信息 */
        IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_CALL_ON_HOLD);
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcImsStateTrying(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    IMSA_CALL_STATUS_ENUM_UINT8 enPreStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8 enNewStatus = IMSA_CALL_STATUS_BUTT;


    /* 如果没有找到对应的呼叫实体，直接返回失败 */
    if (VOS_NULL_PTR == pstCallEntity)
    {
        return VOS_FALSE;
    }

    enPreStatus = pstCallEntity->enStatus;
    enNewStatus = IMSA_CallImsState2ImsaState(pstCallSummary->enCallState, enPreStatus);

    /* 如果原状态和新状态不一致，则通知SPM PROC事件 */
    if (enPreStatus != enNewStatus)
    {
        /* 更新呼叫状态 */
        pstCallEntity->enStatus = enNewStatus;

        /* 通知SPM */
        (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                            pstCallEntity->usClientId,
                                            pstCallEntity->ulOpId,
                                            MN_CALL_EVT_CALL_PROC);
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcImsStateDialing(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    IMSA_CALL_STATUS_ENUM_UINT8 enPreStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8 enNewStatus = IMSA_CALL_STATUS_BUTT;


    if (pstCallEntity)
    {
        /* 保存呼叫之前状态 */
        enPreStatus = pstCallEntity->enStatus;
        enNewStatus = IMSA_CallImsState2ImsaState(pstCallSummary->enCallState, enPreStatus);

        /* 更新呼叫的其它部分 */
        IMSA_CallEntityUpdateByCallSummary(pstCallEntity, pstCallSummary);

        /* 如果原状态和新状态不一致，则通知SPM ORIG事件 */
        if (enPreStatus != enNewStatus)
        {
            /* 更新呼叫状态 */
            pstCallEntity->enStatus = enNewStatus;

            /* 通知SPM */
            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_ORIG);
        }
    }
    else
    {
        IMSA_ERR_LOG1("IMSA_CallProcImsStateDialing: not found call ", pstCallSummary->ulCallIndex);
    }

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallProcImsStateAlerting(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    IMSA_CALL_STATUS_ENUM_UINT8         enPreStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8         enNewStatus = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();


    /* 如果没有找到对应的呼叫实体，直接返回失败 */
    if (VOS_NULL_PTR == pstCallEntity)
    {
        return VOS_FALSE;
    }

    /* 如果重拨的CALL进入ALERTING状态，则停止重拨相关定时器 */
    if ((VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
        && (pstCallCtx->stRedialMaxTimer.usPara < IMSA_CALL_MAX_NUM))
    {
        if (pstCallEntity == &pstCallCtx->astCallEntity[pstCallCtx->stRedialMaxTimer.usPara])
        {
            IMSA_INFO_LOG("IMSA_CallProcImsStateAlerting stop redial timer");

            /* 需要清错误原因值，否则CALL释放时上报RELEASE事件时携带原因值会出错 */
            pstCallEntity->bitOpErrorInfo = IMSA_OP_FALSE;
            IMSA_MEM_SET(   &pstCallEntity->stImsError,
                            0,
                            sizeof(IMSA_IMS_CALL_ERROR_STRU));
            pstCallEntity->ucRedialTimes = 0;
            IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
            IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);
        }
    }

    enPreStatus = pstCallEntity->enStatus;
    enNewStatus = IMSA_CallImsState2ImsaState(pstCallSummary->enCallState, enPreStatus);

    /* 如果原状态和新状态不一致，则通知SPM ALERT事件 */
    if (enPreStatus != enNewStatus)
    {
        /* 更新呼叫状态 */
        pstCallEntity->enStatus = enNewStatus;

        /* 通知是否本地振铃 */
        if (VOS_FALSE == pstCallEntity->bitOpEarlyMedia)
        {
            IMSA_CallSendSpmLocalAlertingInd(MN_CLIENT_ID_BROADCAST, VOS_TRUE);
        }
        /* lihong00150010 上报网络放音导致AT修改CODEC类型问题 begin */
        else
        {
            /* IMSA_CallSendSpmLocalAlertingInd(MN_CLIENT_ID_BROADCAST, VOS_FALSE); */
            IMSA_INFO_LOG("IMSA_CallProcImsStateAlerting: do not send ID_IMSA_SPM_CHANNEL_INFO_IND");
        }
        /* lihong00150010 上报网络放音导致AT修改CODEC类型问题 end */

        /* 通知SPM */
        (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                            pstCallEntity->usClientId,
                                            pstCallEntity->ulOpId,
                                            MN_CALL_EVT_ALERTING);
    }

    return VOS_TRUE;

}


VOS_UINT32 IMSA_CallProcImsStateIncoming(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    VOS_UINT32                          ulResult            = VOS_TRUE;
    IMSA_CALL_STATUS_ENUM_UINT8         enPreStatus         = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_STATUS_ENUM_UINT8         enNewStatus         = IMSA_CALL_STATUS_BUTT;
    IMSA_CALL_ENTITY_STRU              *pstRedailCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx          = IMSA_CallCtxGet();


    /* 如果能够找到呼叫实体，则认为之前在收到了INITIALIZING EVENT，在那里分配了呼叫实体 */
    if (pstCallEntity)
    {
        enPreStatus = pstCallEntity->enStatus;
        enNewStatus = IMSA_CallImsState2ImsaState(pstCallSummary->enCallState, enPreStatus);

        if (enPreStatus != enNewStatus)
        {
            pstCallEntity->enStatus = enNewStatus;

            /* 通知SPM */
            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_INCOMING);
        }
    }
    else
    /* 如果没有找到呼叫实体，则尝试分配呼叫实体 */
    {
        /* 如果存在重拨流程，则打断重拨流程 */
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
        {
            ucRedialCallIndex   = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
            if (ucRedialCallIndex < IMSA_CALL_MAX_NUM)
            {
                pstRedailCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                IMSA_CallInterruptRedial(pstRedailCallEntity);
            }
        }

        pstCallEntity = IMSA_CallEntityAlloc();
        if (pstCallEntity)
        {
            pstCallEntity->enStatus = IMSA_CallImsState2ImsaState(  pstCallSummary->enCallState,
                                                                    pstCallEntity->enStatus);

            /* 保存呼叫实体 */
            IMSA_CallEntityUpdateByCallSummary(pstCallEntity, pstCallSummary);


            /* 通知SPM */
            (VOS_VOID)IMSA_CallSendSpmStateEvt( pstCallEntity->ucId,
                                                pstCallEntity->usClientId,
                                                pstCallEntity->ulOpId,
                                                MN_CALL_EVT_INCOMING);
        }
        else
        {
            /* 如果实体分配失败，则拒绝该呼叫 */
            (VOS_VOID)IMSA_CallImsCmdRel((VOS_UINT32)pstCallSummary->ulCallIndex);

            ulResult = VOS_FALSE;
        }
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcImsStateWaiting(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    return IMSA_CallProcImsStateIncoming(pstCallEntity, pstCallSummary);
}


VOS_UINT32 IMSA_CallProcImsStateInitializing(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    /**
     * 暂不处理，逻辑均在INITIALIZING EVENT中进行
     */

    (VOS_VOID)pstCallEntity;
    (VOS_VOID)pstCallSummary;

    return VOS_TRUE;
}

/**
 * 收到IMS上报的呼叫状态处理函数，根据呼叫状态进行分别处理
 *
 * @param[in]   pstCallEntity   该呼叫对应的实体
 * @param[in]   pstCallSummary  该呼叫对应上报的信息
 *
 * \note
 * 这里的pstCallEntity可能会空指针，对应到无法找到对应呼叫实体的场景，
 * 各状态处理函数需要根据各自的逻辑来处理这种场景
 */
VOS_UINT32 IMSA_CallProcImsState(IMSA_CALL_ENTITY_STRU *pstCallEntity, const IMSA_IMS_CALL_SUMMARY_STRU *pstCallSummary)
{
    VOS_UINT32 ulResult = VOS_FALSE;


    switch(pstCallSummary->enCallState)
    {
    case IMSA_IMS_CALL_STATE_INVALID:
        ulResult = IMSA_CallProcImsStateInvalid(pstCallEntity, pstCallSummary);
    	break;
    /* IMS上报状态为ACTIVING，如果不进行处理，则在CLCC查询CALL状态时，CALL仍为WAITING状态；
    存在两路CALL时，CLCC查询，会出现一路CALL处于HOLD状态，一路处于WAITING状态，因此增
    加ACTVING状态的处理 */
    case IMSA_IMS_CALL_STATE_ACTIVE:
    case IMSA_IMS_CALL_STATE_ACTIVING:
        ulResult = IMSA_CallProcImsStateActive(pstCallEntity, pstCallSummary);
    	break;
    case IMSA_IMS_CALL_STATE_HOLD:
        ulResult = IMSA_CallProcImsStateHeld(pstCallEntity, pstCallSummary);
        break;
    case IMSA_IMS_CALL_STATE_TRYING:
        ulResult = IMSA_CallProcImsStateTrying(pstCallEntity, pstCallSummary);
        break;
    case IMSA_IMS_CALL_STATE_DIALING:
        ulResult = IMSA_CallProcImsStateDialing(pstCallEntity, pstCallSummary);
    	break;
    case IMSA_IMS_CALL_STATE_ALERTING:
        ulResult = IMSA_CallProcImsStateAlerting(pstCallEntity, pstCallSummary);
    	break;
    case IMSA_IMS_CALL_STATE_INCOMING:
        ulResult = IMSA_CallProcImsStateIncoming(pstCallEntity, pstCallSummary);
        break;
    case IMSA_IMS_CALL_STATE_WAITING:
        ulResult = IMSA_CallProcImsStateWaiting(pstCallEntity, pstCallSummary);
        break;
    case IMSA_IMS_CALL_STATE_INITIALIZING:
        ulResult = IMSA_CallProcImsStateInitializing(pstCallEntity, pstCallSummary);
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallProcImsState: invalid call state", pstCallSummary->enCallState);
        break;
    }

    return ulResult;
}


VOS_UINT32 IMSA_CallProcImsMsgCallList(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent)
{
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT32 i = 0;
    VOS_UINT32 ulResult = VOS_FALSE;
    /*IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();*/

    /**
     * 如果正在执行Call Swap操作，且本次状态跟新会导致所有呼叫状态均为HELD
     * 则不进行呼叫状态的更新，待后续通知后再进行更新
     */
    if ((VOS_TRUE == IMSA_CallIsProccessCallSwap()) &&
        (pstOutputCallEvent->stCallReport.ulNumCalls > 1) &&
        (VOS_TRUE == IMSA_CallIsAllCallOnHold(pstOutputCallEvent)) &&
        (VOS_FALSE == IMSA_CallIsAllCallInConference(pstOutputCallEvent)))
    {
        IMSA_INFO_LOG("IMSA_CallProcImsMsgCallList: skip temporary state notification");

        ulResult = VOS_TRUE;
    }
    else
    {
        for (i = 0; i < pstOutputCallEvent->stCallReport.ulNumCalls; i++)
        {
            if (IMSA_IMS_CALL_STATE_DIALING == pstOutputCallEvent->stCallReport.astCalls[i].enCallState)
            {
                pstCallEntity = IMSA_CallEntityGetUsedByCallId(IMSA_CALL_INVALID_ID);
            }
            else
            {
                pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stCallReport.astCalls[i].ulCallIndex);
            }
            ulResult = IMSA_CallProcImsState(pstCallEntity, &pstOutputCallEvent->stCallReport.astCalls[i]);
        }
    }

    /* 只有在发生补充业务的呼叫实体存在，才需要上报 */
    if (VOS_TRUE == pstOutputCallEvent->bitOpSupsrvInfo)
    {
        pstCallEntity = IMSA_CallEntityGetUsedByCallId((VOS_UINT32)pstOutputCallEvent->stSupsrvInfo.ulCallIndex);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallProcImsSupsrvInfo(&pstOutputCallEvent->stSupsrvInfo);
        }
    }

    return ulResult;
}
MN_CALL_SS_NOTIFY_CODE_ENUM_U8 IMSA_CallImsSupsrvInfo2TafSupsrvInfo
(
    IMSA_IMS_SUPSRV_CALL_NOTICE_ENUM_UINT32 enSupsrvNotification
)
{
    MN_CALL_SS_NOTIFY_CODE_ENUM_U8  enMnSupsrvNotification = MN_CALL_SS_NTFY_BUTT;

    IMSA_INFO_LOG1("IMSA_CallImsSupsrvInfo2TafSupsrvInfo: enSupsrvNotification = ", enSupsrvNotification);

    switch(enSupsrvNotification)
    {
        case IMSA_IMS_SUPSRV_MO_CALL_BEING_FORWARDED:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_BE_FORWORDED;
            break;

        case IMSA_IMS_SUPSRV_MO_CALL_IS_WAITING:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_IS_WAITING;
            break;

        case IMSA_IMS_SUPSRV_MO_CALL_BE_DEFLECTED:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_BE_DEFLECTED;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_IS_FORWARDED:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_FORWORDED_CALL;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_IS_HELD:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_ON_HOLD;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_IS_UNHELD:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_RETRIEVED;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_JOINED_CONFCALL:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_ENTER_MPTY;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_EXPLICIT_CALL_TRANSFER:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_EXPLICIT_CALL_TRANSFER;
            break;

        case IMSA_IMS_SUPSRV_MT_CALL_IS_DEFLECTED_CALL:
            enMnSupsrvNotification = MN_CALL_SS_NTFY_DEFLECTED_CALL;
            break;

        default:
            IMSA_ERR_LOG1("IMSA_CallImsSupsrvInfo2TafSupsrvInfo: invalid SupsrvInfo", enSupsrvNotification);

    }
    return enMnSupsrvNotification;

}


VOS_VOID IMSA_CallProcImsSupsrvInfo(const IMSA_IMS_CALL_SUPSRV_INFO_STRU  *pstSupsrvInfo)
{
    MN_CALL_SS_NOTIFY_CODE_ENUM_U8  enMnSupsrvNotification = 0;

    IMSA_INFO_LOG("IMSA_CallProcImsSupsrvInfo entered");

    enMnSupsrvNotification = IMSA_CallImsSupsrvInfo2TafSupsrvInfo(pstSupsrvInfo->enSupsrvNotification);

    /* 如果SPM上报的补充业务类型错误，不需要报给SPM */
    if (MN_CALL_SS_NTFY_BUTT != enMnSupsrvNotification)
    {
        /* 同GU确认，CLIENT ID填为广播，OPID 填为0 */
        (VOS_VOID)IMSA_CallSendSpmSsNotifyEvt(MN_CLIENT_ALL,
                                              0,
                                              pstSupsrvInfo->ulCallIndex,
                                              MN_CALL_EVT_SS_NOTIFY,
                                              TAF_CS_CAUSE_SUCCESS,
                                              enMnSupsrvNotification);
    }

}


/*****************************************************************************
 Function Name  : IMSA_ImsMsgCallEvent()
 Description    : D2IMS呼叫类型的事件处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ImsMsgCallEvent(VOS_VOID *pCallEvent)
{
    IMSA_IMS_OUTPUT_CALL_EVENT_STRU      *pstOutCall;

    pstOutCall = (IMSA_IMS_OUTPUT_CALL_EVENT_STRU *)pCallEvent;

    if (VOS_NULL_PTR == pstOutCall)
    {
        IMSA_ERR_LOG("IMSA_ImsMsgCallEvent: NULL message");
        return;
    }

    IMSA_INFO_LOG1("IMSA_ImsMsgCallEvent: ", pstOutCall->enOutputCallReason);

    /*通过CSMID查找对应的IMSA此前发送的消息*/
    switch (pstOutCall->enOutputCallReason)
    {
        /**
         * TODO: 需要考虑呼叫对应的REASON OK和ERROR和SMS对应的REASON OK如何区分处理
         */
        case IMSA_IMS_OUTPUT_CALL_REASON_OK:
            (VOS_VOID)IMSA_CallProcImsMsgOk(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_ERROR:
            (VOS_VOID)IMSA_CallProcImsMsgError(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_CALL_LIST:
            (VOS_VOID)IMSA_CallProcImsMsgCallList(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_DISCONNECT_EVENT:
            (VOS_VOID)IMSA_CallProcImsMsgDisconnect(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_INCOMING_EVENT:
            (VOS_VOID)IMSA_CallProcImsMsgIncoming(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_WAITING_EVENT:
            (VOS_VOID)IMSA_CallProcImsMsgWaiting(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_EARLY_MEDIA:
            (VOS_VOID)IMSA_CallProcImsMsgCallEarlyMedia(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_INITIALIZING_EVENT:
            (VOS_VOID)IMSA_CallProcImsMsgInitializing(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_SRVCC_CALL_LIST:
            IMSA_CallProcImsMsgSrvccCallList(pstOutCall);
            break;
        case IMSA_IMS_OUTPUT_CALL_REASON_EMERGENCY_INDICATION:
            (VOS_VOID)IMSA_CallProcImsMsgEmergencyInd(pstOutCall);
            break;

        case IMSA_IMS_OUTPUT_CALL_REASON_EXTRA_INFO:
            IMSA_INFO_LOG1("IMSA_ImsMsgCallEvent: these reasons SHOULD not occurs", pstOutCall->enOutputCallReason);
            break;
        default:
            IMSA_ERR_LOG1("IMSA_ImsMsgCallEvent: invalid call reason", pstOutCall->enOutputCallReason);
    }
}
/*lint +e961*/
/*lint +e960*/


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcImsCallMsg.c */



