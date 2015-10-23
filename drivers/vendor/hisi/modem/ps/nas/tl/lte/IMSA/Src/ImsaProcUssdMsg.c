

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaProcUssdMsg.h"
#include "ImsaEntity.h"
#include "ImsaImsInterface.h"
#include "ImsaPublic.h"
#include "ImsaImsAdaption.h"
/* modify by jiqiang 2014.03.25 pclint fix error 718 begin */
#include "ImsaProcUssdMsg.h"
/* modify by jiqiang 2014.03.25 pclint fix error 718 end */
/*lint -e767*/
#define    THIS_FILE_ID     PS_FILE_ID_IMSAPROCUSSDMSG_C
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

/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/

VOS_VOID IMSA_USSD_Init(VOS_VOID)
{
    IMSA_USSD_MANAGER_STRU              *pstUssdManager;

    pstUssdManager = IMSA_USSD_GetEntityAddress();

    /*初始化变量*/
    IMSA_MEM_SET(pstUssdManager, 0, sizeof(IMSA_USSD_MANAGER_STRU));
    pstUssdManager->enUssdState = IMSA_USSD_IDLE_STATE;

    /*设置定时器长度，名称等*/
    pstUssdManager->stUssdWaitNetRspTimer.usName = TI_IMSA_USSD_WAIT_NET_RSP;
    pstUssdManager->stUssdWaitNetRspTimer.ulTimerLen = IMSA_USSD_TIMER_WAIT_NET_RSP_LENGTH;
    pstUssdManager->stUssdWaitNetRspTimer.ucMode = 0;

    pstUssdManager->stUssdWaitAppRspTimer.usName = TI_IMSA_USSD_WAIT_APP_RSP;
    pstUssdManager->stUssdWaitAppRspTimer.ulTimerLen = IMSA_USSD_TIMER_WAIT_APP_RSP_LENGTH;
    pstUssdManager->stUssdWaitAppRspTimer.ucMode = 0;

}


VOS_VOID IMSA_USSD_ClearResource(VOS_VOID)
{
    IMSA_USSD_MANAGER_STRU              *pstUssdManager;

    pstUssdManager = IMSA_USSD_GetEntityAddress();

    /*停止定时器*/
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstUssdManager->stUssdWaitNetRspTimer))
    {
        IMSA_StopTimer(&pstUssdManager->stUssdWaitNetRspTimer);

        (VOS_VOID)IMSA_SendSpmUssdErrorEvt(pstUssdManager->usClientId,
                                            pstUssdManager->ucOpId,
                                            TAF_ERR_IMSA_USSD_UNSPECIFIC);

    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstUssdManager->stUssdWaitAppRspTimer))
    {
        IMSA_StopTimer(&pstUssdManager->stUssdWaitAppRspTimer);

        (VOS_VOID)IMSA_SendSpmUssdErrorEvt(pstUssdManager->usClientId,
                                            pstUssdManager->ucOpId,
                                            TAF_ERR_IMSA_USSD_UNSPECIFIC);
    }


    /*初始化变量*/
    IMSA_MEM_SET(pstUssdManager, 0, sizeof(IMSA_USSD_MANAGER_STRU));
    pstUssdManager->enUssdState = IMSA_USSD_IDLE_STATE;
}




VOS_VOID IMSA_SsProcSpmMsgProcessUssdReq(const SPM_IMSA_PROCESS_USSD_REQ_STRU *pstUssdReq)
{
    IMSA_IMS_USSD_ENCTYPE_ENUM_UINT8   encType;
    VOS_UINT16   usLength = 0;
    VOS_UINT8    aucMessage[TAF_SS_MAX_USSDSTRING_LEN*2];
    IMSA_USSD_MANAGER_STRU              *pstUssdManager;

    pstUssdManager = IMSA_USSD_GetEntityAddress();

    if (IMSA_USSD_MO_CONN_STATE == pstUssdManager->enUssdState)
    {
        IMSA_INFO_LOG("IMSA_SsProcSpmMsgProcessUssdReq: USSD State is err!");

        /*给SPM回复消息错误，不缓存该USSD请求消息,直接回复失败*/
        (VOS_VOID)IMSA_SendSpmUssdErrorEvt(  pstUssdReq->usClientId,
                                    pstUssdReq->ucOpId,
                                    TAF_ERR_IMSA_USSD_BUSY);
        return;
    }

    /*保存ClientId和opId*/
    pstUssdManager->usClientId  = pstUssdReq->usClientId;
    pstUssdManager->ucOpId      = pstUssdReq->ucOpId;

    usLength = pstUssdReq->stProcUssdReq.UssdStr.usCnt;
    IMSA_MEM_CPY(aucMessage,pstUssdReq->stProcUssdReq.UssdStr.aucUssdStr,usLength);

    if( (TAF_SS_8bit != pstUssdReq->stProcUssdReq.DatacodingScheme)
        && (TAF_SS_UCS2 != pstUssdReq->stProcUssdReq.DatacodingScheme))
    {
        encType = TAF_SS_7bit_LANGUAGE_UNSPECIFIED;

    }
    else
    {
        encType =  pstUssdReq->stProcUssdReq.DatacodingScheme;
    }

    if (IMSA_USSD_IDLE_STATE == pstUssdManager->enUssdState)
    {
         /*设置USSD状态*/
        pstUssdManager->enUssdState = IMSA_USSD_MO_CONN_STATE;

        /*启动等待网侧响应定时器*/
        IMSA_StartTimer(&(pstUssdManager->stUssdWaitNetRspTimer));

    }
    else if (IMSA_USSD_MT_CONN_STATE == pstUssdManager->enUssdState)
    {

        /*停止等待APP响应定时器*/
        IMSA_StopTimer(&(pstUssdManager->stUssdWaitAppRspTimer));

        /*设置USSD状态*/
        pstUssdManager->enUssdState = IMSA_USSD_MO_CONN_STATE;

        /*启动等待网侧响应定时器*/
        IMSA_StartTimer(&(pstUssdManager->stUssdWaitNetRspTimer));
    }
    else
    {
        IMSA_INFO_LOG("IMSA_SsProcSpmMsgProcessUssdReq: not process ussd msg");
    }

    /*给IMS发送消息*/
    IMSA_USSD_SndImsUssdReqMsg(encType,usLength,aucMessage);
    return;

}


VOS_VOID IMSA_SsProcSpmMsgReleaseReq(const SPM_IMSA_RELEASE_REQ_STRU *pstUssdRelReq)
{

    IMSA_USSD_MANAGER_STRU              *pstUssdManager;
    (void)pstUssdRelReq;

    pstUssdManager = IMSA_USSD_GetEntityAddress();

    if ((IMSA_USSD_MO_CONN_STATE != pstUssdManager->enUssdState)
        && (IMSA_USSD_MT_CONN_STATE != pstUssdManager->enUssdState))
    {
        IMSA_INFO_LOG("IMSA_SsProcSpmMsgReleaseReq: USSD State is err!");


        return;
    }

    /*停止等待网侧响应定时器*/
    IMSA_StopTimer(&(pstUssdManager->stUssdWaitNetRspTimer));

    /*停止等待APP响应定时器*/
    IMSA_StopTimer(&(pstUssdManager->stUssdWaitAppRspTimer));

    pstUssdManager->enUssdState = IMSA_USSD_IDLE_STATE;

    /*给IMS发送消息*/
    IMSA_USSD_SndImsUssdDisconnectMsg();

    return;

}
VOS_VOID IMSA_USSD_SndImsUssdReqMsg
(
    IMSA_IMS_USSD_ENCTYPE_ENUM_UINT8   encType,
    VOS_UINT16   usLength,
    const VOS_UINT8   *pucMessage
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_USSD_SndImsUssdReqMsg is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_USSD_SndImsUssdReqMsg: alloc memory fail");
        return;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_USSD;
    pstInputEvent->evt.stInputUssdEvent.enInputUssdReason = IMSA_IMS_INPUT_USSD_REASON_AT_CMD_SEND;
    pstInputEvent->evt.stInputUssdEvent.ulOpId = IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputUssdEvent.encType = encType;
    pstInputEvent->evt.stInputUssdEvent.usMessageLen = usLength;
    IMSA_MEM_CPY(pstInputEvent->evt.stInputUssdEvent.ucMessage,
                pucMessage,
                usLength);

    IMSA_SndImsMsgUssdEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return;

}


VOS_VOID IMSA_USSD_SndImsUssdDisconnectMsg
(
    VOS_VOID
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_USSD_SndImsUssdDisconnectMsg is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_USSD_SndImsUssdDisconnectMsg: alloc memory fail");
        return;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_USSD;
    pstInputEvent->evt.stInputUssdEvent.enInputUssdReason = IMSA_IMS_INPUT_USSD_REASON_AT_CMD_DISCONNECT;
    pstInputEvent->evt.stInputUssdEvent.ulOpId = IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputUssdEvent.encType = IMSA_IMS_USSD_ENCTYPE_UNSPECIFIED;

    IMSA_SndImsMsgUssdEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return;

}


VOS_UINT32 IMSA_SendImsaSpmSsMsg(
                                    const TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvtData,
                                    VOS_UINT16 usLen )
{
    IMSA_SPM_SS_MSG_STRU *pstSpmSsMsg = VOS_NULL_PTR;
    VOS_UINT32 ulMsgLen = 0;

    IMSA_INFO_LOG("IMSA_SendImsaSpmSsMsg is entered!");

    ulMsgLen = (sizeof(IMSA_SPM_SS_MSG_STRU) + usLen) - sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU);

    /* 分配空间并检验分配是否成功 */
    pstSpmSsMsg = (VOS_VOID*)IMSA_ALLOC_MSG(ulMsgLen);
    if (VOS_NULL_PTR == pstSpmSsMsg)
    {
        IMSA_ERR_LOG("IMSA_SendImsaSpmSsMsg: alloc memory fail");
        return VOS_FALSE;
    }

    /* 清空 */
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstSpmSsMsg), 0, IMSA_GET_MSG_LENGTH(pstSpmSsMsg));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstSpmSsMsg, ID_IMSA_SPM_SS_MSG);

    /* 设置Client Id，与GU沟通，这个外围的CLIENT ID填写同事件中的client id*/
    pstSpmSsMsg->usClientId =  0x3fff;

    /* 设置消息内容及长度 */
    pstSpmSsMsg->usLen = usLen;
    IMSA_MEM_CPY(&pstSpmSsMsg->stSsEventInfo, pEvtData, pstSpmSsMsg->usLen);

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstSpmSsMsg);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_SendSpmUssdDataSndEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdDataSndEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdDataSndEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_USSD_DATA_SND;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的call id等公共信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;


    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}



VOS_UINT32 IMSA_SendSpmUssdReqIndEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucEncType,
    const TAF_SS_USSD_STRING_STRU             *pstUssdString
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdReqIndEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdReqIndEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_USS_REQ_IND;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;

    pstSsEvt->OP_DataCodingScheme = 1;
    pstSsEvt->OP_UssdString = 1;
    pstSsEvt->DataCodingScheme = (TAF_SS_DATA_CODING_SCHEME)ucEncType;
    pstSsEvt->UssdString.usCnt = pstUssdString->usCnt;

    IMSA_MEM_CPY (pstSsEvt->UssdString.aucUssdStr,pstUssdString->aucUssdStr,pstUssdString->usCnt);


    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}


VOS_UINT32 IMSA_SendSpmUssdNotifyIndEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucEncType,
    const TAF_SS_USSD_STRING_STRU       *pstUssdString
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdNotifyIndEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdNotifyIndEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_USS_NOTIFY_IND;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;

    pstSsEvt->OP_DataCodingScheme = 1;
    pstSsEvt->OP_UssdString = 1;
    pstSsEvt->DataCodingScheme = (TAF_SS_DATA_CODING_SCHEME)ucEncType;
    pstSsEvt->UssdString.usCnt = pstUssdString->usCnt;

    IMSA_MEM_CPY (pstSsEvt->UssdString.aucUssdStr,pstUssdString->aucUssdStr,pstUssdString->usCnt);


    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}


VOS_UINT32 IMSA_SendSpmUssdRelCompleteIndEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdRelCompleteIndEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdRelCompleteIndEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_USS_RELEASE_COMPLETE_IND;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;

    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}


VOS_UINT32 IMSA_SendSpmUssdReqCnfEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT16                          usError
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdRelCompleteIndEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdRelCompleteIndEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_PROCESS_USS_REQ_CNF;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;
    pstSsEvt->OP_Cause = 1;
    pstSsEvt->Cause = (VOS_UINT8)usError;

    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}


VOS_UINT32 IMSA_SendSpmUssdErrorEvt
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT16                          usError
)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pstSsEvt = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_SendSpmUssdErrorEvt is entered!");

    pstSsEvt = IMSA_MEM_ALLOC(sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstSsEvt )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SendSpmUssdErrorEvt:ERROR:Alloc Mem fail!");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstSsEvt, 0, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    /* 设置事件类型 */
    pstSsEvt->SsEvent= TAF_SS_EVT_ERROR;

    /* 设置TAF_SS_CALL_INDEPENDENT_EVENT_STRU中的信息 */
    pstSsEvt->ClientId   = (MN_CLIENT_ID_T)usClientId;
    pstSsEvt->OpId     = (MN_OPERATION_ID_T)ucOpId;
    pstSsEvt->OP_Error = 1;
    pstSsEvt->ErrorCode = usError;

    /* 调用发送函数 */
    ulRslt = IMSA_SendImsaSpmSsMsg(pstSsEvt,
                                    sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));

    IMSA_MEM_FREE(pstSsEvt);

    return ulRslt;
}


VOS_VOID IMSA_USSD_ProcTimerMsgWaitNetRsp(const VOS_VOID *pRcvMsg)
{
    IMSA_USSD_MANAGER_STRU              *pstUssdManager;
    (void)pRcvMsg;

    IMSA_INFO_LOG("IMSA_SMS_ProcTimerMsgWaitNetRsp is entered");

    pstUssdManager = IMSA_USSD_GetEntityAddress();

    (VOS_VOID)IMSA_SendSpmUssdErrorEvt(pstUssdManager->usClientId,
                            pstUssdManager->ucOpId,
                            TAF_ERR_IMSA_WAIT_NET_TIMEOUT);



    return;
}


VOS_VOID IMSA_USSD_ProcTimerMsgWaitAppRsp(const VOS_VOID *pRcvMsg)
{
    /*IMSA_USSD_MANAGER_STRU              *pstUssdManager;*/
    (void)pRcvMsg;

    IMSA_INFO_LOG("IMSA_SMS_ProcTimerMsgWaitAppRsp is entered");

    /*pstUssdManager = IMSA_USSD_GetEntityAddress();*/
    (VOS_VOID)IMSA_SendSpmUssdErrorEvt(0x3fff,
                            0,
                            TAF_ERR_IMSA_WAIT_APP_TIMEOUT);


    return;
}
/*lint +e961*/
/*lint +e960*/




#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcUssdMsg.c */






