/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MnCallTimer.c
  版 本 号   : 初稿
  作    者   : dfakl;f
  生成日期   : 2010年5月19日
  最近修改   :
  功能描述   : dakfdjk
  函数列表   :
  修改历史   :
  1.日    期   : 2010年5月19日
    作    者   : dfakl;f
    修改内容   : 创建文件

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "MnCallApi.h"
#include  "MnCallMgmt.h"
#include  "MnComm.h"
#include  "NasComm.h"
#include  "MnCallReqProc.h"
#include  "MnCallTimer.h"
#include "MnErrorCode.h"
#include "TafLog.h"


#include "MnCallSendCc.h"
#include "MnCallCtx.h"

#define    THIS_FILE_ID  PS_FILE_ID_MNCALL_TIMER_C

/*****************************************************************************
  1 全局变量定义
*************************************************************************/

/* 所有可用的定时器资源 */
MN_CALL_TIMER_HANDLE_STRU                g_astCallTimerHandle[MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA];

/*定时器信息表，该表中记录了每种定时器的超时时间和超时处理函数 */
MN_CALL_TIMER_INFO_STRU                  g_astCallTimerInfoTbl[MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA] =
{
    {5000, MN_CALL_CstSetupTimeout},                                            /* 发送setup消息后的超时处理 */
    {6000, MN_CALL_RingTimerTimeout},                                           /* 循环上报Ring的处理 */

    

    {30000, MN_CALL_CallRedialPeriodTimeout},
    {5000,  MN_CALL_CallRedialIntervalTimeout},

    {TAF_CALL_DTMF_DEFAULT_ONLENGTH, TAF_CALL_RcvTiDtmfOnLengthExpired},

    {TAF_CALL_DTMF_DEFAULT_OFFLENGTH, TAF_CALL_RcvTiDtmfOffLengthExpired}
};



/*****************************************************************************
  2 函数定义
*****************************************************************************/

VOS_VOID MN_CALL_RingTimerTimeout(
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          ulNumOfCalls;
    MN_CALL_ID_T                        aCallIds[MN_CALL_MAX_NUM];

    ulNumOfCalls = 0;
    PS_MEM_SET(aCallIds, 0x00, sizeof(aCallIds));


    /* 停止MN_CALL_TID_RING定时器 */
    MN_CALL_StopTimer(MN_CALL_TID_RING);

    MN_CALL_GetCallsByState(MN_CALL_S_INCOMING, &ulNumOfCalls, aCallIds);

    if ( 0 != ulNumOfCalls )
    {
        /* 启动定时器MN_CALL_TID_RING */
        MN_CALL_StartTimer(MN_CALL_TID_RING, 0, 0, VOS_RELTIMER_NOLOOP);

        MN_CALL_ReportEvent(aCallIds[0], MN_CALL_EVT_INCOMING);

        return;
    }

    MN_CALL_GetCallsByState(MN_CALL_S_WAITING, &ulNumOfCalls, aCallIds);

    if ( 0 != ulNumOfCalls )
    {
        /* 启动定时器MN_CALL_TID_RING */
        MN_CALL_StartTimer(MN_CALL_TID_RING, 0, 0, VOS_RELTIMER_NOLOOP);

        MN_CALL_ReportEvent(aCallIds[0], MN_CALL_EVT_INCOMING);

        return;
    }

}
VOS_VOID  MN_CALL_CstSetupTimeout(
    VOS_UINT32                          ulParam
)
{
    MN_CALL_END_PARAM_STRU              stEndParm;
    VOS_UINT32                          ulNumOfCalls;
    VOS_UINT8                           i;
    MN_CALL_ID_T                        aCallIds[MN_CALL_MAX_NUM];
    MN_CALL_TYPE_ENUM_U8                enCallType;
    MN_CALL_INFO_STRU                   stCallInfo;
    VOS_UINT32                          ulRet;

    MN_CALL_StopTimer(MN_CALL_TID_WAIT_CST_SETUP);


    PS_MEM_SET(aCallIds, 0x00, sizeof(aCallIds));

    stEndParm.enEndCause = MN_CALL_INTERWORKING_UNSPECIFIED;


    PS_MEM_SET(&stCallInfo, 0x00, sizeof(stCallInfo));

    /* 查找当前存在的video call，如果存在，就退出循环 */
    MN_CALL_GetNotIdleStateCalls(&ulNumOfCalls, aCallIds);
    for ( i = 0; i < ulNumOfCalls; i++ )
    {
        enCallType = MN_CALL_GetCallType(aCallIds[i]);
        if ( MN_CALL_TYPE_VIDEO == enCallType )
        {
            break;
        }

    }

    if ( ulNumOfCalls == i )
    {
        MN_WARN_LOG("MN_CALL_CstSetupTimeout: no video type call exist.");
        return;
    }

    MN_CALL_GetCallInfoByCallId(aCallIds[i],&stCallInfo);
    ulRet = MN_CALL_InternalCallEndReqProc(stCallInfo.clientId,
                                           0,
                                           aCallIds[i],
                                           &stEndParm);
    if (TAF_CS_CAUSE_SUCCESS != ulRet)
    {
        MN_WARN_LOG("MN_CALL_CstSetupTimeout: Fail to MN_CALL_InternalCallEndReqProc.");
    }                                           
}
VOS_VOID  MN_CALL_StartTimer(
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId,
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulLength,
    VOS_UINT8                           ucTimerMode
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulTimeLen;


    if (enTimerId >= sizeof(g_astCallTimerInfoTbl)/sizeof(g_astCallTimerInfoTbl[0]))
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "MN_CALL_StartTimer: enTimerId error!");
        return;
    }

    /* 寻找空闲的定时器句柄 */
    for (i = 0; i < MN_CALL_TID_BUTT; i++)
    {
        if (MN_CALL_TID_BUTT == g_astCallTimerHandle[i].enTimerId)
        {
            break;
        }
    }

    if (i < MN_CALL_TID_BUTT)
    {
        /* 启动VOS定时器 */
        ulTimeLen = ulLength;
        if (0 == ulTimeLen)
        {
            ulTimeLen = g_astCallTimerInfoTbl[enTimerId].ulTimeout;
        }

        /* 需要将TimerId输入 */
        if (VOS_OK == NAS_StartRelTimer(&g_astCallTimerHandle[i].hTimer,
                                        WUEPS_PID_TAF,
                                        ulTimeLen,
                                        enTimerId,
                                        ulParam,
                                        ucTimerMode))
        {
            g_astCallTimerHandle[i].enTimerId = enTimerId;

            g_astCallTimerHandle[i].enTimerStatus = MN_CALL_TIMER_STATUS_RUNING;
        }
        else
        {
            MN_ERR_LOG("MN_CALL_StartTimer: VOS_StartRelTimer failed.");
        }
    }
    else
    {
        MN_ERR_LOG("MN_CALL_StartTimer: Can not find free timer handle.");
    }

}
VOS_VOID MN_CALL_StopTimer(
    MN_CALL_TIMER_ID_ENUM_U32            enTimerId
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulRet;

    if ( enTimerId >= MN_CALL_TID_BUTT )
    {
        return;
    }

    /* 寻找enTimerId与输入匹配的句柄 */
    for (i = 0; i < (MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA); i++)
    {
      if (enTimerId == g_astCallTimerHandle[i].enTimerId)
      {
          break;
      }
    }

    if (i < (MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA))
    {
        /* 停止VOS定时器 */
        ulRet = NAS_StopRelTimer(WUEPS_PID_TAF, enTimerId, &g_astCallTimerHandle[i].hTimer);
        if (VOS_OK == ulRet)
        {
            g_astCallTimerHandle[i].hTimer = VOS_NULL_PTR;
            g_astCallTimerHandle[i].enTimerId = MN_CALL_TID_BUTT;

            g_astCallTimerHandle[i].enTimerStatus = MN_CALL_TIMER_STATUS_STOP;
        }
        else
        {
            MN_WARN_LOG("MN_CALL_StopTimer: NAS_StopRelTimer failed.");
        }
    }
    else
    {
        MN_INFO_LOG("MN_CALL_StopTimer: Can not find the timer handle.");
    }
}



VOS_VOID  MN_CALL_ProcTimeoutMsg(
    REL_TIMER_MSG                       *pTmrMsg
)
{
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId;

    /* 找到对应的TimerId后执行对应的超时处理函数 */
    enTimerId = (MN_CALL_TIMER_ID_ENUM_U32)pTmrMsg->ulName - MN_TIMER_CLASS_CCA;

    if (enTimerId < (sizeof(g_astCallTimerInfoTbl)/sizeof(g_astCallTimerInfoTbl[0])))
    {
        g_astCallTimerInfoTbl[enTimerId].pfnTimeoutProc(pTmrMsg->ulPara);
    }
    else
    {
        MN_ERR_LOG("MN_CALL_ProcTimeoutMsg: TimerId out of range.");
    }
}
VOS_VOID  MN_CALL_InitAllTimers(MN_CALL_POWER_STATE_ENUM_U8 enPowerState)
{
    VOS_UINT32                          i;

    /* 关机状态需要先判断是否有启动的定时器,如果有需要先停止定时器*/
    if (MN_CALL_POWER_STATE_OFF == enPowerState)
    {
        for (i = 0; i < (MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA); i++)
        {
            if ((MN_CALL_TID_BUTT != g_astCallTimerHandle[i].enTimerId)
             && (g_astCallTimerHandle[i].hTimer != VOS_NULL_PTR))
            {
                MN_CALL_StopTimer(g_astCallTimerHandle[i].enTimerId);
            }
        }
    }

    for ( i = 0; i < (MN_CALL_TID_BUTT - MN_TIMER_CLASS_CCA); i++)
    {
        g_astCallTimerHandle[i].hTimer    = VOS_NULL_PTR;
        g_astCallTimerHandle[i].enTimerId = MN_CALL_TID_BUTT;

        g_astCallTimerHandle[i].enTimerStatus = MN_CALL_TIMER_STATUS_STOP;
    }
}
VOS_VOID MN_CALL_UpdateTimerPeriod(
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId,
    VOS_UINT32                          ulTimerLen
)
{
    if (enTimerId >= sizeof(g_astCallTimerInfoTbl) / sizeof(g_astCallTimerInfoTbl[0]))
    {
        MN_WARN_LOG("MN_CALL_UpdateTimerPeriod: Can not find the timer Id.");
        return;
    }

    g_astCallTimerInfoTbl[enTimerId].ulTimeout = ulTimerLen;
}



VOS_UINT32 MN_CALL_GetTimerLen(
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId
)
{
    if (enTimerId >= sizeof(g_astCallTimerInfoTbl) / sizeof(g_astCallTimerInfoTbl[0]))
    {
        MN_WARN_LOG("MN_CALL_GetTimerLen: Can not find the timer Id.");
        return 0;
    }

    return g_astCallTimerInfoTbl[enTimerId].ulTimeout;
}



VOS_VOID MN_CALL_CallRedialPeriodTimeout(VOS_UINT32 ulParam)
{
    MN_CALL_StopTimer(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD);

    return;
}


VOS_VOID MN_CALL_CallRedialIntervalTimeout(VOS_UINT32 ulParam)
{
    MN_CALL_MSG_BUFF_STRU              *pstBufferdMsg = VOS_NULL_PTR;
    MN_CALL_STATE_ENUM_U8               enCallState;
    MN_CALL_MPTY_STATE_ENUM_U8          enMptyState;
    VOS_UINT32                          ulRslt;


    enCallState = MN_CALL_S_BUTT;
    enMptyState = MN_CALL_MPYT_STATE_BUTT;

    pstBufferdMsg  = MN_CALL_GetBufferedMsg();

    MN_CALL_GetCallState(pstBufferdMsg->stBufferedSetupMsg.ucCallId, &enCallState, &enMptyState);

    MN_CALL_StopTimer(MN_CALL_TID_WAIT_CALL_REDAIL_INTERVAL);

    if (MN_CALL_S_DIALING != enCallState)
    {
        MN_WARN_LOG("MN_CALL_CallRedialIntervalTimeout: call state is invalid.");
        MN_CALL_ClearBufferedMsg();
        MN_CALL_StopTimer(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD);
        return;
    }

    /* 取缓存消息 */
    if (VOS_TRUE == pstBufferdMsg->bitOpBufferedSetupMsg)
    {
        if (MN_CALL_TYPE_EMERGENCY == MN_CALL_GetCallType(pstBufferdMsg->stBufferedSetupMsg.ucCallId))
        {
            ulRslt = MN_CALL_SendCcBufferedEmgSetupReq(&pstBufferdMsg->stBufferedSetupMsg);
        }
        else
        {
            ulRslt = MN_CALL_SendCcBufferedSetupReq(&pstBufferdMsg->stBufferedSetupMsg);
        }

        /* 消息发送成功返回，否则处理缓存的rej ind消息*/
        if (VOS_TRUE == ulRslt)
        {
            return;
        }
    }

    MN_CALL_StopTimer(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD);

    if (VOS_TRUE == pstBufferdMsg->bitOpBufferedRejIndMsg)
    {
        MN_CALL_ProcBufferedMnccRejInd(&pstBufferdMsg->stBufferedRejIndMsg);
    }

    return;

}


VOS_UINT32  MN_CALL_GetTimerRemainLen(
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId
)
{
    VOS_UINT32                          ulTick;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulTimerNum;

    ulTick     = 0;
    ulTimerNum = sizeof(g_astCallTimerInfoTbl) / sizeof(g_astCallTimerInfoTbl[0]);

    if (enTimerId >= MN_CALL_TID_BUTT)
    {
        return 0;
    }

    for (ulIndex = 0; ulIndex < ulTimerNum; ulIndex++)
    {
        if (enTimerId == g_astCallTimerHandle[ulIndex].enTimerId)
        {
            break;
        }
    }

    if (ulIndex >= ulTimerNum)
    {
       return 0;
    }

    /* hTimer 为空时，说明此定时器已超时或停止 */
    if (VOS_NULL_PTR != g_astCallTimerHandle[ulIndex].hTimer)
    {
        if (VOS_OK != VOS_GetRelTmRemainTime(&(g_astCallTimerHandle[ulIndex].hTimer), &ulTick))
        {
            ulTick = 0;
        }
    }

    /* tick值乘以10转换为毫秒 */
    return ulTick * MN_CALL_TIMER_TICK;
}



MN_CALL_TIMER_STATUS_ENUM_U8  MN_CALL_GetTimerStatus(
    MN_CALL_TIMER_ID_ENUM_U32           enTimerId
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulTimerNum;

    ulTimerNum = sizeof(g_astCallTimerInfoTbl) / sizeof(g_astCallTimerInfoTbl[0]);

    if (enTimerId >= ulTimerNum)
    {
        return MN_CALL_TIMER_STATUS_STOP;
    }

    for (ulIndex = 0; ulIndex < ulTimerNum; ulIndex++)
    {
        if (enTimerId == g_astCallTimerHandle[ulIndex].enTimerId)
        {
            return g_astCallTimerHandle[ulIndex].enTimerStatus;
        }
    }

    return MN_CALL_TIMER_STATUS_STOP;
}



VOS_VOID TAF_CALL_RcvTiDtmfOnLengthExpired(
    VOS_UINT32                          ulParam
)
{
    MN_CALL_ID_T                        CallId;

    MN_CALL_StopTimer(TAF_CALL_TID_DTMF_ON_LENGTH);

    /* 判断状态是否异常 */
    if (TAF_CALL_GetDtmfState() != TAF_CALL_DTMF_WAIT_ON_LENGTH_TIME_OUT)
    {
        MN_WARN_LOG("TAF_CALL_RcvTiDtmfOnLengthExpired: Dtmf State Error!");
        return;
    }

    /* 获取可以用来发送DTMF的CallId，获取失败则清空缓存重置DTMF状态，退出 */
    CallId = 0;
    if (TAF_CS_CAUSE_SUCCESS != TAF_CALL_GetAllowedDtmfCallId(&CallId))
    {
        MN_WARN_LOG("TAF_CALL_RcvTiDtmfOnLengthExpired: Dtmf Not Allowed!");
        TAF_CALL_ResetDtmfCtx(TAF_CS_CAUSE_UNKNOWN);
        return;
    }

    if (VOS_OK != MN_CALL_SendCcStopDtmfReq(CallId))
    {
        MN_ERR_LOG("TAF_CALL_RcvTiDtmfExpired: Send Stop Dtmf Failed!");
        return;
    }

    /* 发送成功，更新DTMF状态 */
    TAF_CALL_SetDtmfState(TAF_CALL_DTMF_WAIT_AUTO_STOP_CNF);

    return;
}


VOS_VOID TAF_CALL_RcvTiDtmfOffLengthExpired(
    VOS_UINT32                          ulParam
)
{
    MN_CALL_ID_T                        CallId;
    TAF_CALL_DTMF_NODE_STRU            *pstNode         = VOS_NULL_PTR;
    HI_LIST_S                          *pstListHead     = VOS_NULL_PTR;
    TAF_CALL_DTMF_INFO_STRU            *pstDtmfCurInfo  = VOS_NULL_PTR;

    pstDtmfCurInfo  = TAF_CALL_GetDtmfCurInfo();

    MN_CALL_StopTimer(TAF_CALL_TID_DTMF_OFF_LENGTH);

    /* 判断状态是否异常 */
    if (TAF_CALL_GetDtmfState() != TAF_CALL_DTMF_WAIT_OFF_LENGTH_TIME_OUT)
    {
        MN_WARN_LOG("TAF_CALL_RcvTiDtmfOffLengthExpired: Dtmf State Error!");
        return;
    }

    /* 若缓存为空，则重置缓存及DTMF状态，退出 */
    if (0 == TAF_CALL_GetDtmfBufCnt())
    {
        TAF_CALL_ResetDtmfCtx(TAF_CS_CAUSE_UNKNOWN);
        return;
    }

    /* 有缓存，若为STOP_DTMF请求，则直接释放此缓存，之后若为START_DTMF请求，则发出 */
    pstListHead = TAF_CALL_GetDtmfListHead();
    pstNode     = msp_list_entry(pstListHead->next, TAF_CALL_DTMF_NODE_STRU, stList);

    if (0 == pstNode->stDtmf.usOnLength)
    {
        *pstDtmfCurInfo = pstNode->stDtmf;

        /* 从过滤表中删除节点 */
        TAF_CALL_DelDtmfNode(pstNode);

        /* 回复STOP_DTMF的正式响应 */
        TAF_CALL_SendDtmfCnf(pstDtmfCurInfo->usClientId,
                             pstDtmfCurInfo->opId,
                             MN_CALL_EVT_STOP_DTMF_RSLT,
                             TAF_CS_CAUSE_SUCCESS);

        /* 如果缓存为空，则直接重置缓存及DTMF状态，退出 */
        if (TAF_CALL_GetDtmfBufCnt() == 0)
        {
            TAF_CALL_ResetDtmfCtx(TAF_CS_CAUSE_UNKNOWN);
            return;
        }

        /* 缓存不为空，则为START_DTMF请求，准备发出 */
        pstNode = msp_list_entry(pstListHead->next, TAF_CALL_DTMF_NODE_STRU, stList);
    }

    /* 获取可以用来发送DTMF的CallId，获取失败则清空缓存重置DTMF状态，退出 */
    CallId = 0;
    if (TAF_CS_CAUSE_SUCCESS != TAF_CALL_GetAllowedDtmfCallId(&CallId))
    {
        MN_WARN_LOG("TAF_CALL_RcvTiDtmfOffLengthExpired: Dtmf Not Allowed!");
        TAF_CALL_ResetDtmfCtx(TAF_CS_CAUSE_UNKNOWN);
        return;
    }

    /* 从缓存中取出节点，发出START_DTMF请求 */
    *pstDtmfCurInfo = pstNode->stDtmf;

    /* 从过滤表中删除节点 */
    TAF_CALL_DelDtmfNode(pstNode);

    if (VOS_OK != MN_CALL_SendCcStartDtmfReq(CallId, pstDtmfCurInfo->cKey))
    {
        TAF_CALL_ResetDtmfCtx(TAF_CS_CAUSE_UNKNOWN);
        MN_ERR_LOG("TAF_CALL_RcvTiDtmfOffLengthExpired: Send Stop Dtmf Failed!");
        return;
    }

    /* 发送成功，更新DTMF状态 */
    TAF_CALL_SetDtmfState(TAF_CALL_DTMF_WAIT_START_CNF);

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

