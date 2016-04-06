/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaCallManagement.c
  Description     : 该C文件实现呼叫管理模块的初始化，内部消息的处理和发送，
                    提供外部调用的API
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include <limits.h>
#include "ImsaEntity.h"
#include "ImsaCallManagement.h"
#include "ImsaRegManagement.h"
#include "ImsaPublic.h"
#include "ImsaImsAdaption.h"
#include "ImsaImsInterface.h"
#include "CallImsaInterface.h"
#include "ImsaServiceManagement.h"
#include "ImsaLrrcInterface.h"
#include "ImsaProcAtMsg.h"
#include "ImsaProcSpmMsg.h"
#include "VcImsaInterface.h"
#include "ImsaProcImsCallMsg.h"

/* xiongxianghui00253310 for PC-LINT 20140210 begin */
#include "ImsaProcUssdMsg.h"
/* xiongxianghui00253310 for PC-LINT 20140210 end */

/*lint -e767*/
#define    THIS_FILE_ID     PS_FILE_ID_IMSACALLMANAGEMENT_C
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
static VOS_UINT16 g_ausSsacBarringTimer[] = {4, 8, 16, 32, 64, 128, 256, 512};


extern VOS_VOID IMSA_CallConverterDtmf2Ims
(
    VOS_UINT32                          ulCallId,
    VOS_CHAR                            cKey,
    VOS_UINT32                          ulDuration,
    IMSA_IMS_INPUT_EVENT_STRU          *pstInputEvent
);
extern TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallIsAllowDtmf
(
    const VOS_UINT8                    *pucCallIds,
    VOS_UINT32                          ulNumOfCalls,
    VOS_UINT8                           ucCallId
);

/*****************************************************************************
  3 Function Declaration
*****************************************************************************/

/*lint -e960*/
/*lint -e961*/
/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_UINT32 IMSA_CallTypeAvailabilityCheck(MN_CALL_TYPE_ENUM_UINT8 enCallType)
{


    IMSA_INFO_LOG("IMSA_CallTypeAvailabilityCheck is entered!");

    if ((MN_CALL_TYPE_EMERGENCY != enCallType) &&
        (MN_CALL_TYPE_VOICE != enCallType) &&
        (MN_CALL_TYPE_VIDEO != enCallType))
    {
        return VOS_FALSE;
    }
    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallAvailabilityCheck(MN_CALL_TYPE_ENUM_UINT8 enCallType,
                                      IMSA_EMC_CALL_TYPE_ENUM_UINT32 *penEmcType)
{
    VOS_UINT32 ulServiceRet = 0;
    VOS_UINT32 ulResult = IMSA_CALL_AVAILABILITY_CHECK_RESULT_REJECT;
    VOS_UINT32                          ulSsacRslt = 0;


    IMSA_INFO_LOG("IMSA_CallAvailabilityCheck is entered!");

    /* 根据不同呼叫类型去判断对应服务是否可用 */
    if (MN_CALL_TYPE_EMERGENCY == enCallType)
    {

        ulServiceRet = IMSA_StartImsEmergService(penEmcType);
    }
    else
    {
        ulServiceRet = IMSA_StartImsNormalService(enCallType);
    }

    /* 返回服务判定结果  */
    switch (ulServiceRet)
    {
    case IMSA_START_SRV_RESULT_CAN_MAKE_CALL:
        ulSsacRslt = IMSA_ProcSsacInfo(enCallType);

        if (VOS_TRUE == ulSsacRslt)
        {
            ulResult = IMSA_CALL_AVAILABILITY_CHECK_RESULT_CONTINUE;

        }
        else
        {
            ulResult = IMSA_CALL_AVAILABILITY_CHECK_RESULT_REJECT;

        }
        break;
    case IMSA_START_SRV_RESULT_BUFFER_CALL_WAIT_INDICATION:
        ulResult = IMSA_CALL_AVAILABILITY_CHECK_RESULT_CACHED;
        break;
    default:
        ulResult = IMSA_CALL_AVAILABILITY_CHECK_RESULT_REJECT;
    }

    return ulResult;
}


VOS_UINT32 IMSA_CallCheckIsAllowNewOrig(VOS_VOID)
{
    VOS_UINT32    i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallCheckIsAllowNewOrig is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (IMSA_OP_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            if ((IMSA_CALL_STATUS_IDLE != pstCallCtx->astCallEntity[i].enStatus) &&
                (IMSA_CALL_STATUS_HELD != pstCallCtx->astCallEntity[i].enStatus))
            {
                IMSA_INFO_LOG("IMSA_CallCheckIsAllowNewOrig: not allow orig!");
                return VOS_FALSE;
            }
        }
    }

    return VOS_TRUE;
}



VOS_UINT32 IMSA_CallCtxInit(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallCtxInit is entered!");

    /* Initialize memory */
    IMSA_MEM_SET(pstCallCtx, 0, sizeof(IMSA_CALL_MANAGER_STRU));

    /* Initialize timer */
    pstCallCtx->stProctectTimer.usName = TI_IMSA_CALL_PROTECT;
    pstCallCtx->stProctectTimer.ulTimerLen = IMSA_CALL_TIMER_LEN_PROTECT;
    pstCallCtx->stProctectTimer.ucMode = VOS_RELTIMER_NOLOOP;

    pstCallCtx->stResReadyTimer.usName = TI_IMSA_CALL_RES_READY;
    pstCallCtx->stResReadyTimer.ulTimerLen = IMSA_CALL_TIMER_LEN_WAIT_RES;
    pstCallCtx->stResReadyTimer.ucMode = VOS_RELTIMER_NOLOOP;

    pstCallCtx->stDtmfCtx.stDtmfDurationTimer.usName = TI_IMSA_CALL_DTMF_DURATION;
    pstCallCtx->stDtmfCtx.stDtmfDurationTimer.ucMode = VOS_RELTIMER_NOLOOP;

    pstCallCtx->stDtmfCtx.stDtmfProtectTimer.usName = TI_IMSA_CALL_DTMF_PROTECT;
    pstCallCtx->stDtmfCtx.stDtmfProtectTimer.ulTimerLen = IMSA_CALL_TIMER_LEN_PROTECT;
    pstCallCtx->stDtmfCtx.stDtmfProtectTimer.ucMode = VOS_RELTIMER_NOLOOP;

    /* 初始化BACK-OFF定时器 */
    pstCallCtx->stBackOffTxTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stBackOffTxTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stBackOffTxTimer.usName      = TI_IMSA_BACK_OFF_TX;
    pstCallCtx->stBackOffTxTimer.ulTimerLen  = 0;

    pstCallCtx->stBackOffTyTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stBackOffTyTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stBackOffTyTimer.usName      = TI_IMSA_BACK_OFF_TY;
    pstCallCtx->stBackOffTyTimer.ulTimerLen  = 0;

    /* 初始化重播相关定时器 */
    pstCallCtx->stRedialMaxTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stRedialMaxTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stRedialMaxTimer.usName      = TI_IMSA_CALL_REDIAL_MAX_TIME;
    pstCallCtx->stRedialMaxTimer.ulTimerLen  = 0;

    pstCallCtx->stRedialIntervelTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stRedialIntervelTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stRedialIntervelTimer.usName      = TI_IMSA_CALL_REDIAL_INTERVEL;
    pstCallCtx->stRedialIntervelTimer.ulTimerLen  = 0;

    /* 初始化Tcall定时器, SilentRedial机制,CMCC定制 */
    pstCallCtx->stNormalTcallTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stNormalTcallTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stNormalTcallTimer.usName      = TI_IMSA_CALL_NORMAL_TCALL;
    pstCallCtx->stNormalTcallTimer.ulTimerLen  = 0;
    pstCallCtx->stEmcTcallTimer.phTimer     = VOS_NULL_PTR;
    pstCallCtx->stEmcTcallTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstCallCtx->stEmcTcallTimer.usName      = TI_IMSA_CALL_EMC_TCALL;
    pstCallCtx->stEmcTcallTimer.ulTimerLen  = 0;

    /* DTMF缓存buffer初始化 */
    IMSA_CallDtmfInfoInit();

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallCtxDeinit(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT16                          usRedialCallIndex = IMSA_NULL;
    VOS_UINT32                          i = 0;

    IMSA_INFO_LOG("IMSA_CallCtxDeinit is entered!");

    /* 清除DTMF信息 */
    IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_POWER_OFF);

    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        switch (pstCallCtx->stSpmMsg.ulSpmMsgId)
        {
        case ID_SPM_IMSA_CALL_ORIG_REQ:
            (VOS_VOID)IMSA_CallSendSpmOrigCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                 pstCallCtx->stSpmMsg.ulOpId,
                                                 pstCallCtx->stSpmMsg.ulCallId,
                                                 TAF_CS_CAUSE_IMSA_SERVICE_NOT_AVAILABLE);
            break;
        case ID_SPM_IMSA_CALL_SUPS_CMD_REQ:
            (VOS_VOID)IMSA_CallSendSpmSupsResultMsg(pstCallCtx->stSpmMsg.usClientId,
                                              pstCallCtx->stSpmMsg.ulOpId,
                                              pstCallCtx->stSpmMsg.ulCallId,
                                              MN_CALL_SS_RES_FAIL);
            break;
        default:
            IMSA_ERR_LOG1("IMSA_CallCtxDeinit: invalid call request", pstCallCtx->stSpmMsg.ulSpmMsgId);
            break;
        }

        IMSA_CallSpmMsgClear();
    }

    /* Stop timer if running */
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stProctectTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stProctectTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stResReadyTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stResReadyTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stDtmfCtx.stDtmfDurationTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfDurationTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stDtmfCtx.stDtmfProtectTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stBackOffTxTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stBackOffTxTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stBackOffTyTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stBackOffTyTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
    {
        /* 重拨过程中关机，需要给SPM回复消息 */
        usRedialCallIndex   = pstCallCtx->stRedialMaxTimer.usPara;
        if (usRedialCallIndex < IMSA_CALL_MAX_NUM)
        {
            pstCallEntity = &pstCallCtx->astCallEntity[usRedialCallIndex];
            IMSA_CallInterruptRedial(pstCallEntity);
        }
        IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);
    }

    /* 关机时，如果有CALL存在，需要通知SPM release事件 */
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            IMSA_CallReleaseCallCommonProc(&(pstCallCtx->astCallEntity[i]), TAF_CS_CAUSE_IMSA_SERVICE_NOT_AVAILABLE);
        }
    }

    /* Stop active call */
    if (IMSA_CallEntityGetUsedCount() > 0)
    {
        (VOS_VOID)IMSA_CallImsCmdRelAll();
    }
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stNormalTcallTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stNormalTcallTimer);
    }

    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stEmcTcallTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stEmcTcallTimer);
    }

    (VOS_VOID)IMSA_CallCtxInit();

    return VOS_TRUE;
}
IMSA_CALL_ENTITY_STRU* IMSA_CallEntityAlloc( VOS_VOID )
{
    VOS_UINT16                          i = 0;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityAlloc is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_FALSE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            IMSA_MEM_SET(&(pstCallCtx->astCallEntity[i]), 0, sizeof(IMSA_CALL_ENTITY_STRU));

            pstCallCtx->astCallEntity[i].bitOpIsUsed = IMSA_OP_TRUE;

            pstCallCtx->astCallEntity[i].usClientId = MN_CLIENT_ALL;
            pstCallCtx->astCallEntity[i].ulOpId = 0;
            pstCallCtx->astCallEntity[i].ucSpmcallId = (VOS_UINT8)(i + 1);
            pstCallCtx->astCallEntity[i].enStatus = IMSA_CALL_STATUS_DIALING;

            pstCallCtx->astCallEntity[i].ucCallEntityIndex = (VOS_UINT8)i;

            return &(pstCallCtx->astCallEntity[i]);
        }
    }

    return VOS_NULL_PTR;
}


VOS_VOID IMSA_CallEntityFree(IMSA_CALL_ENTITY_STRU *pstCallEntity)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityFree is entered!");

    if (pstCallEntity)
    {
        /**
         * TODO: 需要考虑资源等待定时器以及保护定时器，缓存消息等
         * 如果这些定时器或消息是刚好是基于此呼叫，
         * 则需要释放这些资源
         *
         * 这个动作应该放在EntityFree外边
         *
         * 等待资源定时器有可能是对应到多个呼叫，
         * 目前没有保存多个呼叫映射关系
         */

        /* 如果要释放的CALL正在进行资源预留，则需要停止资源预留定时器 */
        if ((VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stResReadyTimer))
            && (pstCallEntity == &pstCallCtx->astCallEntity[pstCallCtx->stResReadyTimer.usPara]))
        {
            IMSA_StopTimer(&pstCallCtx->stResReadyTimer);
        }

        IMSA_MEM_SET(pstCallEntity, 0, sizeof(IMSA_CALL_ENTITY_STRU));

        pstCallEntity->bitOpIsUsed     = VOS_FALSE;
        /* NOTE: other clear procedure, nothing now*/
    }
}

/*****************************************************************************
 Function Name  : IMSA_CallClearTransToCsInfo
 Description    : 清除所有SRVCC转移到CS域的call标示
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-10-14  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallClearTransToCsInfo(VOS_VOID)
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            pstCallCtx->astCallEntity[i].bitOpTransToCs = VOS_FALSE;
        }
    }

}


IMSA_CALL_ENTITY_STRU* IMSA_CallEntityGetUsedByImsCallId(VOS_UINT32 ulCallId)
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityGetUsedByImsCallId is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            ((VOS_UINT8)ulCallId == pstCallCtx->astCallEntity[i].ucImscallId))
        {
            return &(pstCallCtx->astCallEntity[i]);
        }
    }

    return VOS_NULL_PTR;
}

IMSA_CALL_ECONF_SUMMARY_STRU* IMSA_EconfCalledGetUsedByCalledNum
(
    IMSA_CALL_ENTITY_STRU              *pstCallEntity,
    VOS_CHAR                                 *pCalledNum
)
{
    VOS_UINT32 i = 0;

    IMSA_INFO_LOG("IMSA_EconfCalledGetUsedByCalledNum is entered!");

    for (i = 0; i < IMSA_IMS_ECONF_CALLED_MAX_NUM; i++)
    {
        if ((IMSA_OP_TRUE == pstCallEntity->stEconfCalllist[i].bitOpIsUsed) &&
            (0 == IMSA_MEM_CMP(pCalledNum,pstCallEntity->stEconfCalllist[i].stCalledNumber.aucNumber,IMSA_IMS_NUMBER_STRING_SZ)))
        {
            return &(pstCallEntity->stEconfCalllist[i]);
        }
    }

    /* 如果IMS上报的与会者不在列表中，则需要将被叫号码添加到呼叫实体中,增强型多方通话和普通通话合并时，IMS会多报一个用户上来 */
    for (i = 0; i < IMSA_IMS_ECONF_CALLED_MAX_NUM; i++)
    {
        if (VOS_FALSE == pstCallEntity->stEconfCalllist[i].bitOpIsUsed)
        {
            pstCallEntity->stEconfCalllist[i].bitOpIsUsed = IMSA_OP_TRUE;

            return &(pstCallEntity->stEconfCalllist[i]);
        }
    }

    return VOS_NULL_PTR;
}
IMSA_CALL_ENTITY_STRU* IMSA_CallEntityGetUsedByEconfFlag(VOS_VOID)
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityGetUsedByEconfFlag is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_TRUE == pstCallCtx->astCallEntity[i].ucIsEconfFlag))
        {
            return &(pstCallCtx->astCallEntity[i]);
        }
    }

    return VOS_NULL_PTR;
}


IMSA_CALL_ENTITY_STRU* IMSA_CallEntityGetUsedBySpmCallId(VOS_UINT32 ulCallId)
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityGetUsedBySpmCallId is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            ((VOS_UINT8)ulCallId == pstCallCtx->astCallEntity[i].ucSpmcallId))
        {
            return &(pstCallCtx->astCallEntity[i]);
        }
    }

    return VOS_NULL_PTR;
}



VOS_VOID IMSA_CallUpdateDiscCallDir
(
    VOS_UINT32                          ulCallId,
    VOS_UINT8                           ucIsUser
)
{
    IMSA_CALL_ENTITY_STRU              *pstCallEntity = VOS_NULL_PTR;

    if ( (ulCallId == 0) || ( ulCallId > IMSA_CALL_MAX_NUM ) )
    {
        IMSA_ERR_LOG("IMSA_CallUpdateDiscCallDir:call id error!");
        return;
    }

    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(ulCallId);

    if (VOS_NULL_PTR == pstCallEntity)
    {
        IMSA_ERR_LOG("IMSA_CallUpdateDiscCallDir:call not exist!");
        return;
    }

    if (VOS_TRUE == pstCallEntity->stDiscDir.ucDiscCallFlag)
    {
        return;
    }

    pstCallEntity->stDiscDir.ucIsUser       = ucIsUser;
    pstCallEntity->stDiscDir.ucDiscCallFlag = VOS_TRUE;

    return;
}


VOS_VOID IMSA_CallGetCallsByState
(
    IMSA_CALL_STATUS_ENUM_UINT8         enStatus,
    VOS_UINT32                         *pulNumOfCalls,
    VOS_UINT32                         *pulCallIds
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT32                          i           = IMSA_NULL;

    *pulNumOfCalls = 0;

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (enStatus == pstCallCtx->astCallEntity[i].enStatus))
        {
            pulCallIds[*pulNumOfCalls] = (VOS_UINT32)pstCallCtx->astCallEntity[i].ucSpmcallId;
            (*pulNumOfCalls)++;
        }
    }
}


VOS_UINT32 IMSA_CallEntityGetUsedCount(VOS_VOID)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulCount = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityGetUsedCount is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            ulCount++;
        }
    }

    return ulCount;
}


VOS_VOID IMSA_CallSpmOrigReqMsgSave(const SPM_IMSA_CALL_ORIG_REQ_STRU *pstAppMsg)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = IMSA_CALL_INVALID_ID;

    IMSA_MEM_CPY(   &(pstCallCtx->stSpmMsg.stParam.stOrigParam),
                    &pstAppMsg->stOrig,
                    sizeof(MN_CALL_ORIG_PARAM_STRU));
}
VOS_VOID IMSA_CallSpmEconfDialReqMsgSave(const SPM_IMSA_CALL_ECONF_DIAL_REQ_STRU *pstAppMsg)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = IMSA_CALL_INVALID_ID;

    IMSA_MEM_CPY(   &(pstCallCtx->stSpmMsg.stParam.stEconfDialParam),
                    &pstAppMsg->stDialReq,
                    sizeof(TAF_CALL_ECONF_DIAL_REQ_STRU));
}
VOS_VOID IMSA_CallSpmEconfAddUserMsgSave(const SPM_IMSA_CALL_ECONF_ADD_USERS_REQ_STRU *pstAppMsg)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmEconfAddUserMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmEconfAddUserMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = IMSA_CALL_INVALID_ID;

    IMSA_MEM_CPY(   &(pstCallCtx->stSpmMsg.stParam.stEconfAddParam),
                    &pstAppMsg->stEconfCalllist,
                    sizeof(TAF_CALL_ECONF_CALL_LIST_STRU));
}
VOS_VOID IMSA_CallSpmSupsCmdReqMsgSave
(
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmSupsCmdReqMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmSupsCmdReqMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = pstAppMsg->stCallMgmtCmd.callId;

    IMSA_MEM_CPY(   &(pstCallCtx->stSpmMsg.stParam.stSupsParam),
                    &pstAppMsg->stCallMgmtCmd,
                    sizeof(MN_CALL_SUPS_PARAM_STRU));
}
VOS_VOID IMSA_CallSpmModifyMsgSave
(
    const SPM_IMSA_CALL_MODIFY_REQ_STRU  *pstAppMsg
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmModifyReqMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmModifyReqMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = pstAppMsg->callId;

}
VOS_VOID IMSA_CallSpmAnswerRemoteModifyMsgSave
(
    const SPM_IMSA_CALL_ANSWER_REMOTE_MODIFY_REQ_STRU *pstAppMsg
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmAnswerRemoteModifyMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmAnswerRemoteModifyMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = pstAppMsg->callId;

}
VOS_VOID IMSA_CallSpmMsgClear(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmMsgClear is entered!");

    IMSA_MEM_SET(&pstCallCtx->stSpmMsg, 0, sizeof(IMSA_CALL_SPM_MSG_STRU));

    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_FALSE;
}


VOS_UINT32 IMSA_CallImsMsgSave(const IMSA_IMS_INPUT_CALL_EVENT_STRU *pstCallEvt)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallImsMsgSave is entered!");

    /* 如果之前已经有保存，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stImsMsg.bitOpImsMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSaveImsMsg has existed");
    }

    /* 保存命令内容 */
    pstCallCtx->stImsMsg.bitOpImsMsg = IMSA_OP_TRUE;
    pstCallCtx->stImsMsg.ulCsmId     = pstCallEvt->ulOpId;
    pstCallCtx->stImsMsg.ulCsmReason = pstCallEvt->enInputCallReason;

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallImsMsgFind(VOS_UINT32 ulOpId)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallImsMsgFind is entered!");

    if ((IMSA_OP_TRUE == pstCallCtx->stImsMsg.bitOpImsMsg) &&
        (ulOpId == pstCallCtx->stImsMsg.ulCsmId))
    {
        return IMSA_CALL_FIND_SUCC;
    }
    else if(((IMSA_CALL_DTMF_WAIT_START_CNF == pstCallCtx->stDtmfCtx.enDtmfState)
                || (IMSA_CALL_DTMF_WAIT_STOP_CNF == pstCallCtx->stDtmfCtx.enDtmfState)
                || (IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF == pstCallCtx->stDtmfCtx.enDtmfState))
         && (ulOpId == pstCallCtx->stDtmfCtx.stCurrentDtmf.ulImsOpid))
    {
        return IMSA_CALL_FIND_SUCC_DTMF;
    }
    else
    {
        return IMSA_CALL_FIND_FAIL;
    }
}


VOS_VOID IMSA_CallImsMsgClear(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallImsMsgClear is entered!");

    pstCallCtx->stImsMsg.bitOpImsMsg = IMSA_FALSE;

    pstCallCtx->stImsMsg.ulCsmId = 0;
    pstCallCtx->stImsMsg.ulCsmReason = 0xffffffff;
}


VOS_VOID IMSA_CallGetCurrentProcessingCallTypeByCallStatus
(
    IMSA_CALL_TYPE_ENUM_UINT8        *penCallType
)
{
    IMSA_CALL_MANAGER_STRU           *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT32                       i           = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallGetCurrentProcessingCallTypeByCallStatus is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (IMSA_CALL_STATUS_HELD != pstCallCtx->astCallEntity[i].enStatus) &&
            (IMSA_CALL_STATUS_ACTIVE != pstCallCtx->astCallEntity[i].enStatus))
        {
            *penCallType = pstCallCtx->astCallEntity[i].enType;
            return;
        }
    }

    if (IMSA_CALL_MAX_NUM == i)
    {
        IMSA_INFO_LOG("IMSA_CallGetCurrentProcessingCallTypeByCallStatus:not found call type!");
    }

    return;
}


VOS_VOID IMSA_ProcCallResourceIsReady
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32    enMediaPdpType
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    VOS_UINT32 i = 0;

    IMSA_INFO_LOG("IMSA_ProcCallResourceIsReady is entered!");

    if (IMSA_CONN_MEDIA_PDP_TYPE_VOICE == enMediaPdpType)
    {
        pstCallCtx->ucVoiceBearExistFlag = IMSA_TRUE;
    }
    else if (IMSA_CONN_MEDIA_PDP_TYPE_VIDEO == enMediaPdpType)
    {
        pstCallCtx->ucVideoBearExistFlag = IMSA_TRUE;
    }
    else
    {
    }

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpNeedSendResResult) &&
            (IMSA_CALL_CALL_REASON_RESOURCE_READY == IMSA_CallIsResourceReady(&pstCallCtx->astCallEntity[i])))
        {
            pstCallCtx->astCallEntity[i].bitOpNeedSendResResult = IMSA_OP_FALSE;
            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallCtx->astCallEntity[i].ucImscallId,
                                                pstCallCtx->astCallEntity[i].ulResRspOpId,
                                                IMSA_CALL_CALL_REASON_RESOURCE_READY);
            /* 停止等待资源预留结果定时 */
            IMSA_StopTimer(&pstCallCtx->stResReadyTimer);
        }

    }

}
VOS_VOID IMSA_ProcCallResourceIsNotReady
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_ProcCallResourceIsNotReady is entered!");

    /* 视频call需要语音承载和视频承载都存在，所以如果语音承载去激活后，ulIsVideoMediaPdpReady
    需要设置为FALSE */
    if (IMSA_FALSE == IMSA_CONN_HasActiveVoicePdp())
    {
        pstCallCtx->ucVoiceBearExistFlag = IMSA_FALSE;
    }
    else if (IMSA_FALSE == IMSA_CONN_HasActiveVideoPdp())
    {
        pstCallCtx->ucVideoBearExistFlag = IMSA_FALSE;
    }
    else
    {
    }

#if 0
    /* 通知各呼叫实体资源预留失败 */
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_FALSE == pstCallCtx->astCallEntity[i].bitOpResReady))
        {
            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallCtx->astCallEntity[i].ucId,
                                                pstCallCtx->astCallEntity[i].ulResRspOpId,
                                                VOS_FALSE);
        }
    }

    /* 停止等待资源预留结果定时 */
    if (IMSA_IsTimerRunning(&pstCallCtx->stResReadyTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stResReadyTimer);
    }
#endif
}


IMSA_IMS_EMERGENCY_TYPE_ENUM_UINT8 IMSA_CallNum2EmcSubType(const MN_CALL_EMERGENCY_CAT_STRU *pstEmcCat)
{
    IMSA_IMS_EMERGENCY_TYPE_ENUM_UINT8 enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_GENERIC;

    IMSA_INFO_LOG("IMSA_CallNum2EmcSubType is entered!");

    switch (pstEmcCat->ucEmergencyCat)
    {
    case MN_CALL_EMER_CATEGORG_POLICE:
        enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_POLICE;
        break;
    case MN_CALL_EMER_CATEGORG_AMBULANCE:
        enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_AMBULANCE;
        break;
    case MN_CALL_EMER_CATEGORG_FIRE_BRIGADE:
        enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_FIRE;
        break;
    case MN_CALL_EMER_CATEGORG_MARINE_GUARD:
        enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_MARINE;
        break;
    case MN_CALL_EMER_CATEGORG_MOUNTAIN_RESCUE:
        enEmcSubType = IMSA_IMS_EMERGENCY_SUB_TYPE_MOUNTAIN;
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallNum2EmcSubType: invalid emergency category", pstEmcCat->ucEmergencyCat);
    }

    return enEmcSubType;
}


MN_CALL_EMER_CATEGORG_TYPE_ENUM_U8 IMSA_CallEmcSubType2SPMEmcCat(const IMSA_IMS_EMERGENCY_TYPE_ENUM_UINT8 EmcSubType)
{
    MN_CALL_EMER_CATEGORG_TYPE_ENUM_U8 enEmcCat = MN_CALL_EMER_CATEGORG_MAX;
    IMSA_INFO_LOG1("IMSA_CallEmcSubType2SpmEmcCat: IMS Emc Type: ", EmcSubType);

    switch (EmcSubType)
    {
    case IMSA_IMS_EMERGENCY_SUB_TYPE_AMBULANCE:
        enEmcCat = MN_CALL_EMER_CATEGORG_AMBULANCE;
        break;
    case IMSA_IMS_EMERGENCY_SUB_TYPE_POLICE:
        enEmcCat = MN_CALL_EMER_CATEGORG_POLICE;
        break;
    case IMSA_IMS_EMERGENCY_SUB_TYPE_FIRE:
        enEmcCat = MN_CALL_EMER_CATEGORG_FIRE_BRIGADE;
        break;
    case IMSA_IMS_EMERGENCY_SUB_TYPE_MARINE:
        enEmcCat = MN_CALL_EMER_CATEGORG_MARINE_GUARD;
        break;
    case IMSA_IMS_EMERGENCY_SUB_TYPE_MOUNTAIN:
        enEmcCat = MN_CALL_EMER_CATEGORG_MOUNTAIN_RESCUE;
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallEmcSubType2SPMEmcCat: Unexpected EmcSubType: ", EmcSubType);
    }

    return enEmcCat;
}


IMSA_IMS_CALL_CLIR_TYPE_ENUM_UINT8 IMSA_CallConverterClir2Ims(MN_CALL_CLIR_CFG_ENUM_UINT8 enClir)
{
    IMSA_IMS_CALL_CLIR_TYPE_ENUM_UINT8 enResult = IMSA_IMS_CALL_CLIR_TYPE_NONE;

    IMSA_INFO_LOG("IMSA_CallClir2CidType is entered!");

    switch (enClir)
    {
    case MN_CALL_CLIR_AS_SUBS:
        enResult = IMSA_IMS_CALL_CLIR_TYPE_NONE;
        break;
    case MN_CALL_CLIR_INVOKE:
        enResult = IMSA_IMS_CALL_CLIR_TYPE_INVOCATION;
        break;
    case MN_CALL_CLIR_SUPPRESS:
        enResult = IMSA_IMS_CALL_CLIR_TYPE_SUPPRESSION;
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallClir2CidType: invalid clir setting ", enClir);
    }

    return enResult;
}

VOS_VOID IMSA_CallCopyRemoteAddrForm(const MN_CALL_CALLED_NUM_STRU *pstCalledNum, VOS_CHAR *paucRemoteAddr)
{
    VOS_UINT8 ucCallTon;

    IMSA_INFO_LOG("IMSA_CallCopyRemoteAddrForm is entered!");

    /*24.008 10.5.4.9,Type of Number,5,6,7bit of the MN_CALL_NUM_TYPE_ENUM_U8*/
    ucCallTon = (pstCalledNum->enNumType>>4)&0x7;

    /*国际号码，号码中有"+"*/
    if(ucCallTon == MN_CALL_TON_INTERNATIONAL)
    {
        paucRemoteAddr[0] = '+';

        if (VOS_FALSE == IMSA_UtilBcdNumberToAscii(pstCalledNum->aucBcdNum, pstCalledNum->ucNumLen, &paucRemoteAddr[1]))
        {
            IMSA_ERR_LOG("IMSA_CallCopyRemoteAddrForm: BCD to ASCII fail!");
        }

        return;
    }

    /*非国际号码*/
    if (VOS_FALSE == IMSA_UtilBcdNumberToAscii(pstCalledNum->aucBcdNum, pstCalledNum->ucNumLen, paucRemoteAddr))
    {
        IMSA_ERR_LOG("IMSA_CallCopyRemoteAddrForm: BCD to ASCII fail.");
    }
}
VOS_VOID IMSA_CallConverterOrig2Ims(VOS_UINT32 ulCallId,
                                    const MN_CALL_ORIG_PARAM_STRU*  pSpmParam,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_INFO_LOG("IMSA_CallConverterOrig2Ims is entered!");

    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_DIAL;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    /* 设置呼叫类型 */
    pstInputEvent->evt.stInputCallEvent.enCallType = (IMSA_IMS_CALL_MODE_ENUM_UINT8)pSpmParam->enCallType;

    /* 设置紧急呼标识 */
    if (MN_CALL_TYPE_EMERGENCY == pSpmParam->enCallType)
    {
        pstInputEvent->evt.stInputCallEvent.bitOpEmergencyType = IMSA_OP_TRUE;
        pstInputEvent->evt.stInputCallEvent.enEmergencyType = IMSA_CallNum2EmcSubType(&pSpmParam->stEmergencyCat);
    }
    else
    {
        pstInputEvent->evt.stInputCallEvent.bitOpEmergencyType = IMSA_OP_TRUE;
        pstInputEvent->evt.stInputCallEvent.enEmergencyType = IMSA_IMS_EMERGENCY_SUB_TYPE_NONE;
    }

    /* 设置被叫用户 */
    pstInputEvent->evt.stInputCallEvent.bitOpRemoteAddress = IMSA_OP_TRUE;
    IMSA_CallCopyRemoteAddrForm(&pSpmParam->stDialNumber, pstInputEvent->evt.stInputCallEvent.aucRemoteAddress);

    /* 设置CLIR服务标识 */
    pstInputEvent->evt.stInputCallEvent.bitOpClirType = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.enClirCfg = IMSA_CallConverterClir2Ims(pSpmParam->enClirCfg);
}
VOS_VOID IMSA_CallConverterCreatEconf2Ims(VOS_UINT32 ulCallId,
                                    const TAF_CALL_ECONF_DIAL_REQ_STRU*  pSpmParam,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    VOS_UINT8                           i = 0;
    IMSA_INFO_LOG("IMSA_CallConverterCreatEconf2Ims is entered!");

    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_CREAT_NEW_ECONFERENCE;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    /* 设置呼叫类型 */
    pstInputEvent->evt.stInputCallEvent.enCallType = (IMSA_IMS_CALL_MODE_ENUM_UINT8)pSpmParam->enCallType;

    /* 设置被叫用户 */
    pstInputEvent->evt.stInputCallEvent.bitOpEconfList = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.stEconfList.ucNumCalls = (VOS_UINT8)pSpmParam->stEconfCalllist.ulCallNum;
    for (i = 0; i < pSpmParam->stEconfCalllist.ulCallNum; i ++)
    {
        IMSA_CallCopyRemoteAddrForm(&(pSpmParam->stEconfCalllist.astDialNumber[i]), pstInputEvent->evt.stInputCallEvent.stEconfList.astEconfList[i].aucRemoteAddress);
    }


    /* 设置CLIR服务标识 */
    pstInputEvent->evt.stInputCallEvent.bitOpClirType = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.enClirCfg = IMSA_CallConverterClir2Ims(pSpmParam->enClirCfg);
}

VOS_VOID IMSA_CallConverterEconfAddUser2Ims(VOS_UINT32 ulCallId,
                                    const TAF_CALL_ECONF_CALL_LIST_STRU*  pSpmParam,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    VOS_UINT8                           i = 0;
    IMSA_INFO_LOG("IMSA_CallConverterEconfAddUser2Ims is entered!");

    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_ECONFERENCE_INVITE_NEW_PARTICIPANT;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    /* 设置被叫用户 */
    pstInputEvent->evt.stInputCallEvent.bitOpEconfList = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.stEconfList.ucNumCalls = (VOS_UINT8)pSpmParam->ulCallNum;
    for (i = 0; i < pSpmParam->ulCallNum; i ++)
    {
        IMSA_CallCopyRemoteAddrForm(&(pSpmParam->astDialNumber[i]), pstInputEvent->evt.stInputCallEvent.stEconfList.astEconfList[i].aucRemoteAddress);
    }

}


VOS_VOID IMSA_CallConverterSups2Ims(VOS_UINT32 ulCallId,
                                    const MN_CALL_SUPS_PARAM_STRU*  pSpmParam,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallConverterSups2Ims is entered!");

    (VOS_VOID)ulCallId;

    /* 需要将SPMCALLID转换为IMS CALLID */
    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(pSpmParam->callId);

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫OpId */
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    pstInputEvent->evt.stInputCallEvent.enCallType = (IMSA_IMS_CALL_MODE_ENUM_UINT8)pSpmParam->enCallType;

    switch (pSpmParam->enCallSupsCmd)
    {
        /* CHLD=0, Releases all held calls or sets User Determined User Busy (UDUB) for a waiting call */
        case MN_CALL_SUPS_CMD_REL_HELD_OR_UDUB:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_REL_HELD_OR_WAITING;
                break;
            }
        /* CHLD=1, Releases all active calls (if any exist) and accepts the other (held or waiting) call */
        case MN_CALL_SUPS_CMD_REL_ACT_ACPT_OTH:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_REL_ACTIVE_AND_ACCEPT_OTH;
                break;
            }
        /* CHLD=1x, Releases a specific active call X*/
        case MN_CALL_SUPS_CMD_REL_CALL_X:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RELEASE_AT_X;

                pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
                if (pstCallEntity)
                {
                    pstInputEvent->evt.stInputCallEvent.ulCallIndex = pstCallEntity->ucImscallId;
                }
                break;
            }
        /* ATH, Release all calls */
        case MN_CALL_SUPS_CMD_REL_ALL_CALL:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_HANGUP;
                break;
            }
        /* CHLD=2, Places all active calls (if any exist) on hold and accepts the other (held or waiting) call */
        case MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_SWAP;
                break;
            }
        /* CHLD=2x, Places all active calls on hold except call X with which communication shall be supported */
        case MN_CALL_SUPS_CMD_HOLD_ALL_EXCPT_X:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_HOLD_ALL_EXCEPT_X;

                pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
                if (pstCallEntity)
                {
                    pstInputEvent->evt.stInputCallEvent.ulCallIndex = pstCallEntity->ucImscallId;
                }
                break;
            }
        /* CHLD=3, Adds a held call to the conversation */
        case MN_CALL_SUPS_CMD_BUILD_MPTY:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_CONFERENCE;
                break;
            }
        case MN_CALL_SUPS_CMD_ECONF_REL_USER:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_ECONFERENCE_KICK_PARTICIPANT;
                pstInputEvent->evt.stInputCallEvent.bitOpEconfList = IMSA_OP_TRUE;
                /* 每次踢人仅允许踢一个 */
                pstInputEvent->evt.stInputCallEvent.stEconfList.ucNumCalls = 1;
                IMSA_CallCopyRemoteAddrForm(&pSpmParam->stRemoveNum,
                                            pstInputEvent->evt.stInputCallEvent.stEconfList.astEconfList[0].aucRemoteAddress);
                break;
            }
        case MN_CALL_SUPS_CMD_ECONF_MERGE_CALL:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_ECONFERENCE_MERGER_NEW_PARTICIPANT;
                break;
            }
        default:
            IMSA_ERR_LOG1("IMSA_CallConverterSups2Ims : unsupported sups cmd", pSpmParam->enCallSupsCmd);
    }
}


VOS_VOID IMSA_CallConverterDtmf2Ims
(
    VOS_UINT32                          ulCallId,
    VOS_CHAR                            cKey,
    VOS_UINT32                          ulDuration,
    IMSA_IMS_INPUT_EVENT_STRU          *pstInputEvent
)
{
    IMSA_IMS_DTMF_KEY_ENUM_UINT8        enDtmfKey = IMSA_IMS_DTMF_KEY_0;

    IMSA_INFO_LOG("IMSA_CallConverterStartDtmf2Ims is entered!");

    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_DTMF;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    pstInputEvent->evt.stInputCallEvent.bitOpDtmf = IMSA_OP_TRUE;

    (VOS_VOID)IMSA_AsciiToDtmfKeyEnum(cKey, &enDtmfKey);
    pstInputEvent->evt.stInputCallEvent.stDtmf.enDtmfKey = enDtmfKey;
    pstInputEvent->evt.stInputCallEvent.stDtmf.ulDuration = ulDuration;
}

#if 0
VOS_VOID IMSA_CallConverterRejectCallById2Ims(VOS_UINT32 ulCallId, IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_INFO_LOG("IMSA_CallConverterRejectCallById2Ims is entered!");

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RELEASE_AT_X;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = ulCallId;
}
#endif
VOS_VOID IMSA_CallConverterResRsp2Ims
(
    VOS_UINT32 ulImsOpId,
    VOS_UINT32 ulCallId,
    IMSA_CALL_CALL_REASON_RESOURCE_RESULT_ENUM_UINT8 enResResult,
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent
)
{
    IMSA_INFO_LOG("IMSA_CallConverterResRsp2Ims is entered!");

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置OpId */
    pstInputEvent->evt.stInputCallEvent.ulOpId = ulImsOpId;

    switch(enResResult)
    {
        case IMSA_CALL_CALL_REASON_RESOURCE_READY:
            pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RESOURCE_READY;
            break;

        case IMSA_CALL_CALL_REASON_RESOURCE_ONLY_VOICE:
            pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RESOURCE_ONLY_VOICE_READY;
            break;

        case IMSA_CALL_CALL_REASON_RESOURCE_FAILED:
            pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RESOURCE_FAILED;
            break;
        default:
            IMSA_INFO_LOG("IMSA_CallConverterResRsp2Ims error result!");
            break;
    }

    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = ulCallId;
}
VOS_UINT32 IMSA_CallSendImsMsgResRsp
(
    VOS_UINT32 ulCallId,
    VOS_UINT32 ulImsOpId,
    IMSA_CALL_CALL_REASON_RESOURCE_RESULT_ENUM_UINT8 enResResult
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgResRsp is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgResRsp: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterResRsp2Ims(ulImsOpId,
                                 ulCallId,
                                 enResResult,
                                 pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendImsMsgDial
(
    VOS_UINT32                          ulCallId,
    const SPM_IMSA_CALL_ORIG_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgDial is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgDial: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterOrig2Ims(ulCallId, &pstAppMsg->stOrig, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmOrigReqMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallSendImsMsgCreatEconf
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_ECONF_DIAL_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgCreatEconf is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgCreatEconf: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterCreatEconf2Ims(ulCallId, &pstAppMsg->stDialReq, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmEconfDialReqMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallSendImsMsgEconfAddUser
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_ECONF_ADD_USERS_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgEconfAddUser is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgEconfAddUser: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterEconfAddUser2Ims(ulCallId, &pstAppMsg->stEconfCalllist, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmEconfAddUserMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}

/* xiongxianghui00253310 add for conference 20140210 begin */
/*****************************************************************************
 Function Name  : IMSA_CallInviteNewPtptAvailableCheck
 Description    : 拉会议第三方可行性检查
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 可行性检查是否成功

 History        :
      1.xiongxianghui00253310      2014-02-10  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_CallInviteNewPtptAvailableCheck(VOS_VOID)
{

    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    VOS_UINT32 i = 0;
    VOS_UINT32 ulResult = VOS_FALSE;

    IMSA_INFO_LOG("IMSA_CallInviteNewPtptAvailableCheck is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (MN_CALL_IN_MPTY == pstCallCtx->astCallEntity[i].enMpty) &&
            (VOS_FALSE == pstCallCtx->astCallEntity[i].ucIsEconfFlag))
        {
            ulResult = VOS_TRUE;
            break;
        }
    }

    return ulResult;
}

/*****************************************************************************
 Function Name  : IMSA_CallSpmInviteNewPtptReqMsgSave
 Description    : 保存正在处理的SPM命令
 Input          : pstAppMsg   SPM下发的拉会议第三方命令
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.xiongxianghui00253310      2014-02-10  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallSpmInviteNewPtptReqMsgSave(const SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ_STRU *pstAppMsg)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave is entered!");

    /* 如果之前已经有保存的命令，则返回失败 */
    if (IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg)
    {
        IMSA_INFO_LOG("IMSA_CallSpmOrigReqMsgSave has existed");
    }

    /* 设置比特位 */
    pstCallCtx->stSpmMsg.bitOpSpmMsg = IMSA_OP_TRUE;

    /* 保存公共部分 */
    pstCallCtx->stSpmMsg.ulSpmMsgId = pstAppMsg->ulMsgId;
    pstCallCtx->stSpmMsg.usClientId = pstAppMsg->usClientId;
    pstCallCtx->stSpmMsg.ulOpId     = pstAppMsg->ucOpId;
    pstCallCtx->stSpmMsg.ulCallId   = IMSA_CALL_INVALID_ID;

}


/*****************************************************************************
 Function Name  : IMSA_CallConverterInviteNewPtpt2Ims
 Description    : 拉会议第三方命令及参数到D2 Input消息的转化函数
 Input          : ulCallId          呼叫Id
                  pstCalledNum      SPM命令参数
 Output         : pstInputEvent     转换结果保存的地址
 Return Value   : VOS_VOID

 History        :
      1.xiongxianghui00253310      2014-02-10  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallConverterInviteNewPtpt2Ims(VOS_UINT32 ulCallId,
                                    const MN_CALL_CALLED_NUM_STRU* pstCalledNum,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_INFO_LOG("IMSA_CallConverterInviteNewPtpt2Ims is entered!");
    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫命令和OpId */
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_CONFERENCE_INVITE_NEW_PARTICIPANT;
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

    /* 设置被叫用户 */
    pstInputEvent->evt.stInputCallEvent.bitOpRemoteAddress = IMSA_OP_TRUE;
    IMSA_CallCopyRemoteAddrForm(pstCalledNum, pstInputEvent->evt.stInputCallEvent.aucRemoteAddress);
}


/*****************************************************************************
 Function Name  : IMSA_CallSendImsMsgInviteNewPtpt
 Description    : 向IMS发送拉会议第三方请求
 Input          : ulCallId          呼叫Id
                  pstAppMsg         SPM下发的拉会议第三方请求参数
 Output         : VOS_VOID
 Return Value   : 发送消息结果

 History        :
      1.xiongxianghui00253310      2014-02-10  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_CallSendImsMsgInviteNewPtpt
(
    VOS_UINT32 ulCallId,
    const SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ_STRU *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgInviteNewPtpt is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgInviteNewPtpt: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterInviteNewPtpt2Ims(ulCallId, &pstAppMsg->stNewPtptNumber, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /*  保存SPM命令 */
    IMSA_CallSpmInviteNewPtptReqMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}

/* xiongxianghui00253310 add for conference 20140210 end */



VOS_UINT32 IMSA_CallSendImsMsgStartDtmf
(
    VOS_UINT32                          ulCallId,
    VOS_CHAR                            cKey,
    VOS_UINT32                          ulDuration
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgStartDtmf is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgStartDtmf: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterDtmf2Ims(ulCallId, cKey, ulDuration, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendImsMsgStopDtmf
(
    VOS_UINT32                          ulCallId,
    VOS_CHAR                            cKey
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgStopDtmf is entered!");

    (VOS_VOID)cKey;

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgStopDtmf: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterDtmf2Ims(ulCallId, '0', 0, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendImsMsgSups
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgSups is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgSups: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_CallConverterSups2Ims(ulCallId, &pstAppMsg->stCallMgmtCmd, pstInputEvent);

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmSupsCmdReqMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}

VOS_UINT32 IMSA_CallSendImsMsgModify
(
    const SPM_IMSA_CALL_MODIFY_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgModify is entered!");

    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(pstAppMsg->callId);

    if (VOS_NULL_PTR == pstCallEntity)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgModify: NO CALL ID");
        return VOS_FALSE;
    }

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgModify: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));


    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;
    pstInputEvent->evt.stInputCallEvent.ulOpId= IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_MODIFY;
    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = pstCallEntity->ucSpmcallId;

    pstInputEvent->evt.stInputCallEvent.bitOpModify = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.stModify.enSrcCallType = pstAppMsg->enCurrCallType;
    pstInputEvent->evt.stInputCallEvent.stModify.enDstCallType = pstAppMsg->enExpectCallType;

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmModifyMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}

VOS_UINT32 IMSA_CallSendImsMsgAnswerRemoteModify
(
    const SPM_IMSA_CALL_ANSWER_REMOTE_MODIFY_REQ_STRU  *pstAppMsg
)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgAnswerRemoteModify is entered!");

    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(pstAppMsg->callId);

    if (VOS_NULL_PTR == pstCallEntity)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgModify: NO CALL ID");
        return VOS_FALSE;
    }

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgAnswerRemoteModify: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));


    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;
    pstInputEvent->evt.stInputCallEvent.ulOpId= IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_ANSWER_REMOTE_MODIFY;
    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = pstCallEntity->ucImscallId;

    pstInputEvent->evt.stInputCallEvent.bitOpModify = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.stModify.enSrcCallType = pstAppMsg->enCurrCallType;
    pstInputEvent->evt.stInputCallEvent.stModify.enDstCallType = pstAppMsg->enExpectCallType;

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

    /* 保存SPM命令 */
    IMSA_CallSpmAnswerRemoteModifyMsgSave(pstAppMsg);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallSendImsMsgSrvcc(IMSA_IMS_INPUT_CALL_REASON_ENUM_UINT32 enSrvccReason)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendImsMsgSrvcc is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallSendImsMsgSrvcc: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;
    pstInputEvent->evt.stInputCallEvent.ulOpId= IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = enSrvccReason;

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    /* srvcc通过TAF call模块发起，不是AT命令发起，不需要IMS回复确认，
       所有当给ims发送SRVCC流程指示时，不需要在保存ims命令 */
    /* (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent); */

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}

/*****************************************************************************
 Function Name  : IMSA_CallInterruptRedial
 Description    : 打断重播流程，即停止重播最大时长定时器和重播间隔定时器，
                  给SPM上报RELEASED事件，携带重播信息中保存的错误原因值，
                  如果是这是最后一个CALL，则还需向SPM上报ALL RELEASED事件，
                  清除CALL实体信息，并清除重播信息；
 Input          : pstCallEntity------------------CALL实体指针
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong 0015001     2013-12-23  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallInterruptRedial
(
    IMSA_CALL_ENTITY_STRU        *pstCallEntity
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr    = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallInterruptRedial is entered!");

    /* 启动重播间隔定时器时，将CALL实体中的CALL ID信息清除为0，
       所以需要重新将间隔定时器中记录的CALLID写回CALL实体中，才能保证上报
       RELEASD事件时填写的CALL ID正确 */
    pstCallEntity->ucSpmcallId = (VOS_UINT8)((pstCallCtx->stRedialIntervelTimer.usPara & 0xff00) >> IMSA_MOVEMENT_8);

    /* 停止重播最大时长定时器和重播间隔定时器 */
    IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
    IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);

    /* 将IMS错误原因值转化为SPM错误原因值 */
    /* 修改函数入参 */
    enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

    IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);
}
VOS_UINT32 IMSA_CallImsCmdRel(VOS_UINT32 ulCallId)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallImsCmdRel is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallImsCmdRel: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    pstInputEvent->evt.stInputCallEvent.ulOpId= IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RELEASE_AT_X;

    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = ulCallId;

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallImsCmdRelAll(VOS_VOID)
{
    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallImsCmdRelAll is entered!");

    pstInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));
    if (VOS_NULL_PTR == pstInputEvent)
    {
        IMSA_ERR_LOG("IMSA_CallImsCmdRelAll: alloc memory fail");
        return VOS_FALSE;
    }

    IMSA_MEM_SET(pstInputEvent, 0, sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();
    pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_HANGUP;

    IMSA_SndImsMsgCallEvent(pstInputEvent);

    IMSA_MEM_FREE(pstInputEvent);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallCmdRelAllEmc(VOS_UINT8 ucIsUserDisc)
{
    VOS_UINT32                          i                   = 0;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx          = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity       = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallCmdRelAllEmc is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType))
        {
            /* 如果紧急Tcall定时器还在运行，需要停止 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stEmcTcallTimer))
            {
                IMSA_StopTimer(&pstCallCtx->stEmcTcallTimer);
            }

            /* 如果要释放的CALL正好是重播CALL，则打断重拨流程 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
            {
                ucRedialCallIndex = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
                if (i == ucRedialCallIndex)
                {
                    pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                    #if (FEATURE_ON == FEATURE_PTM)
                    IMSA_CallErrRecord(pstCallEntity->stImsError.stErrorCode.usSipStatusCode, pstCallEntity);
                    #endif
                    IMSA_CallInterruptRedial(pstCallEntity);

                    continue;
                }
            }
            if (IMSA_TRUE == pstCallCtx->ucRetryEmcRegFlag)
            {
                /* 如果是紧急呼叫，收到#380，重新发起紧急注册，并且失败的场景，需要通知SPM release事件 */
                pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->ucCallEntityIndex];

                pstCallEntity->ucSpmcallId = pstCallCtx->ucId;
                #if (FEATURE_ON == FEATURE_PTM)
                IMSA_CallErrRecord(pstCallEntity->stImsError.stErrorCode.usSipStatusCode, pstCallEntity);
                #endif
                /* 将IMS错误原因值转化为SPM错误原因值 */
                /* 修改函数入参 */
                enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

                IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

                /* 清除保存的重注册标识等参数 */
                pstCallCtx->ucRetryEmcRegFlag = IMSA_FALSE;
                pstCallCtx->ucId = IMSA_NULL;
                pstCallCtx->ucCallEntityIndex = IMSA_NULL;
                continue;
            }

            /* 只有将消息发送给IMS，且收到IMS上报的callid后，才需要给IMS发送释放命令 */
            if (IMSA_CALL_INVALID_ID !=pstCallCtx->astCallEntity[i].ucImscallId)
            {
                IMSA_CallUpdateDiscCallDir( (VOS_UINT32)pstCallCtx->astCallEntity[i].ucSpmcallId,
                                            ucIsUserDisc);

                (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucImscallId);

                #if (FEATURE_ON == FEATURE_PTM)
                IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_SERVICE_NOT_AVAILABLE, pstCallEntity);
                #endif
            }

        }
    }

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallCmdRelAllNormal(VOS_UINT8 ucIsUserDisc)
{
    VOS_UINT32                          i                   = 0;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx          = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity       = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallCmdRelAllNormal is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            ((IMSA_CALL_TYPE_EMC != pstCallCtx->astCallEntity[i].enType) ||
            ((IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType) &&
            (IMSA_EMC_CALL_TYPE_IN_NORMAL_SRV == pstCallCtx->astCallEntity[i].enEmcType))))
        {
            /* 如果要释放的CALL正好是重播CALL，则打断重拨流程 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
            {
                ucRedialCallIndex = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
                if (i == ucRedialCallIndex)
                {
                    pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                    #if (FEATURE_ON == FEATURE_PTM)
                    IMSA_CallErrRecord(pstCallEntity->stImsError.stErrorCode.usSipStatusCode, pstCallEntity);
                    #endif
                    IMSA_CallInterruptRedial(pstCallEntity);

                    continue;
                }
            }
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_SERVICE_NOT_AVAILABLE, pstCallEntity);
            #endif
            IMSA_CallUpdateDiscCallDir( (VOS_UINT32)pstCallCtx->astCallEntity[i].ucSpmcallId,
                                        ucIsUserDisc);

            (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucImscallId);
        }
    }

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallSendIntraResultAction(IMSA_CALL_TYPE_ENUM_UINT8 enCallType,
                                          IMSA_RESULT_ACTION_ENUM_UINT32 enAction)
{
    IMSA_CALL_RESULT_ACTION_IND_STRU stAction = {0};

    IMSA_INFO_LOG("IMSA_CallSendIntraResultAction is entered!");

    stAction.enCallType     = enCallType;
    stAction.enResultAction = enAction;

    return IMSA_RegSendIntraMsg(ID_IMSA_CALL_RESULT_ACTION_IND, (VOS_UINT8 *)&stAction, sizeof(IMSA_CALL_RESULT_ACTION_IND_STRU));
}


VOS_UINT32 IMSA_CallSendSpmEventMsg(
                                    const VOS_VOID  *pEvtData,
                                    VOS_UINT16 usLen)
{
    IMSA_SPM_CALL_MSG_STRU *pstSpmCallMsg = VOS_NULL_PTR;
    VOS_UINT32 ulMsgLen = 0;

    IMSA_INFO_LOG("IMSA_CallSendSpmEventMsg is entered!");

    ulMsgLen = ((sizeof(IMSA_SPM_CALL_MSG_STRU) + usLen) - sizeof(MN_CALL_INFO_STRU)) - sizeof(MN_CALL_EVENT_ENUM_UINT32);

    /* 分配空间并检验分配是否成功 */
    pstSpmCallMsg = (VOS_VOID*)IMSA_ALLOC_MSG(ulMsgLen);
    if (VOS_NULL_PTR == pstSpmCallMsg)
    {
        IMSA_ERR_LOG("IMSA_CallSendSpmEventMsg: alloc memory fail");
        return VOS_FALSE;
    }

    /* 清空 */
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstSpmCallMsg), 0, IMSA_GET_MSG_LENGTH(pstSpmCallMsg));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstSpmCallMsg, ID_IMSA_SPM_CALL_MSG);

    /* 设置Client Id，与GU沟通，这个外围的CLIENT ID写死成广播*/
    pstSpmCallMsg->usClientId = MN_CLIENT_ALL;

    /* 设置消息内容及长度 */
    pstSpmCallMsg->usLen = usLen;
    IMSA_MEM_CPY((VOS_UINT8 *)&pstSpmCallMsg->enEventType, pEvtData, pstSpmCallMsg->usLen);

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstSpmCallMsg);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendSpmEvtAndCallInfo(VOS_UINT16 usClientId,
                                          VOS_UINT32 ulOpId,
                                          VOS_UINT32 ulCallId,
                                          MN_CALL_EVENT_ENUM_UINT32 enEvt,
                                          TAF_CS_CAUSE_ENUM_UINT32 ulResult)
{
    IMSA_CALL_ENTITY_STRU *pstCallEntity = VOS_NULL_PTR;
    MN_CALL_EVT_ORIG_CNF_STRU stOrigCnf = {0};

    IMSA_INFO_LOG("IMSA_CallSendSpmEvtAndCallInfo is entered!");

    /* 设置事件类型 */
    stOrigCnf.enEventType = enEvt;

    /* 设置MN_CALL_INFO_STRU中的call id等公共信息 */
    stOrigCnf.stCallInfo.opId     = (MN_OPERATION_ID_T)ulOpId;
    stOrigCnf.stCallInfo.clientId = usClientId;

    /* 设置命令结果 */
    stOrigCnf.stCallInfo.enCause  = ulResult;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    /**
     * 如果该Call Id对应的呼叫实体存在，则根据IMSA呼叫实体内容填充回复结果中的stCallInfo；
     * 如果呼叫实体不存在，则IMSA只保证基本的op id, client id及结果有效
     */
    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(ulCallId);
    if (pstCallEntity)
    {
        IMSA_CallEntity2SpmCallInfo(pstCallEntity, enEvt, &stOrigCnf.stCallInfo);
    }

    IMSA_INFO_LOG1("IMSA_CallSendSpmEvtAndCallInfo: MptyState", stOrigCnf.stCallInfo.enMptyState);

    /* 调用发送函数 */
    return IMSA_CallSendSpmEventMsg((VOS_VOID *)&stOrigCnf,
                                    sizeof(MN_CALL_EVT_ORIG_CNF_STRU));
}
VOS_UINT32 IMSA_CallSendSpmReleasedEvt
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
)
{
    MN_CALL_EVT_ORIG_CNF_STRU stOrigCnf = {0};

    IMSA_INFO_LOG("IMSA_CallSendSpmEvtAndCallInfo is entered!");

    /* 设置事件类型 */
    stOrigCnf.enEventType = MN_CALL_EVT_RELEASED;

    /* 设置MN_CALL_INFO_STRU中的call id等公共信息 */
    stOrigCnf.stCallInfo.callId   = (MN_CALL_ID_T)pstCallEntity->ucSpmcallId;

    stOrigCnf.stCallInfo.opId     = (MN_OPERATION_ID_T)pstCallEntity->ulOpId;
    stOrigCnf.stCallInfo.clientId = pstCallEntity->usClientId;

    /* 设置命令结果 */
    stOrigCnf.stCallInfo.enCause  = ulResult;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    IMSA_CallEntity2SpmCallInfo(pstCallEntity, MN_CALL_EVT_RELEASED, &stOrigCnf.stCallInfo);

    /* 调用发送函数 */
    return IMSA_CallSendSpmEventMsg((VOS_VOID *)&stOrigCnf,
                                    sizeof(MN_CALL_EVT_ORIG_CNF_STRU));
}

VOS_UINT32 IMSA_CallSendSpmSsNotifyEvt
(
    VOS_UINT16 usClientId,
    VOS_UINT32 ulOpId,
    VOS_UINT32 ulCallId,
    MN_CALL_EVENT_ENUM_UINT32 enEvt,
    TAF_CS_CAUSE_ENUM_UINT32 ulResult,
    MN_CALL_SS_NOTIFY_CODE_ENUM_U8 enCode
)
{
    MN_CALL_EVT_ORIG_CNF_STRU stOrigCnf = {0};

    IMSA_INFO_LOG("IMSA_CallSendSpmEvtAndCallInfo is entered!");

    /* 设置事件类型 */
    stOrigCnf.enEventType = enEvt;

    /* 设置MN_CALL_INFO_STRU中的call id等公共信息 */
    stOrigCnf.stCallInfo.callId   = (MN_CALL_ID_T)ulCallId;
    stOrigCnf.stCallInfo.opId     = (MN_OPERATION_ID_T)ulOpId;
    stOrigCnf.stCallInfo.clientId = usClientId;

    /* 设置命令结果 */
    stOrigCnf.stCallInfo.enCause  = ulResult;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    stOrigCnf.stCallInfo.stSsNotify.enCode = enCode;

    /* 调用发送函数 */
    return IMSA_CallSendSpmEventMsg((VOS_VOID *)&stOrigCnf,
                                    sizeof(MN_CALL_EVT_ORIG_CNF_STRU));
}


VOS_UINT32 IMSA_CallSendSpmAllReleasedEvt( VOS_VOID )
{
    MN_CALL_EVT_ORIG_CNF_STRU stOrigCnf = {0};

    IMSA_INFO_LOG("IMSA_CallSendSpmAllReleasedEvt is entered!");

    if (IMSA_TRUE ==IMSA_CallGetNotReportAllReleasedFlag())
    {
        /* 删除对标志的判断，今后需要将涉及到该标志的调用都删掉 */
        IMSA_CallSetNotReportAllReleasedFlag(IMSA_FALSE);
    }

    /* 设置事件类型 */
    stOrigCnf.enEventType = MN_CALL_EVT_ALL_RELEASED;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    /* 调用发送函数 */
    return IMSA_CallSendSpmEventMsg((VOS_VOID *)&stOrigCnf,
                                    sizeof(MN_CALL_EVT_ORIG_CNF_STRU));
}
VOS_UINT32 IMSA_CallSendSpmStartDtmfRsltMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
)
{
    IMSA_SPM_CALL_START_DTMF_RSLT_IND_STRU   *pstStartDtmfRsltInd = VOS_NULL_PTR;

    (VOS_VOID)ucCallId;

    IMSA_INFO_LOG("IMSA_CallSendSpmStartDtmfRsltMsg is entered!");

    /* 分配空间并检验分配是否成功 */
    pstStartDtmfRsltInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_SPM_CALL_START_DTMF_RSLT_IND_STRU));
    if (VOS_NULL_PTR == pstStartDtmfRsltInd)
    {
        IMSA_ERR_LOG("IMSA_CallSendSpmStartDtmfRsltMsg: alloc memory fail");
        return VOS_FALSE;
    }

    /* 清空 */
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstStartDtmfRsltInd), 0, IMSA_GET_MSG_LENGTH(pstStartDtmfRsltInd));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstStartDtmfRsltInd, ID_IMSA_SPM_CALL_START_DTMF_RSLT_IND);

    pstStartDtmfRsltInd->usClientId = usClientId;
    pstStartDtmfRsltInd->ucOpId     = ucOpId;
    pstStartDtmfRsltInd->enCause    = ulResult;

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstStartDtmfRsltInd);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendSpmStopDtmfRsltMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
)
{
    IMSA_SPM_CALL_STOP_DTMF_RSLT_IND_STRU   *pstStopDtmfRsltInd = VOS_NULL_PTR;

    (VOS_VOID)ucCallId;

    IMSA_INFO_LOG("IMSA_CallSendSpmStopDtmfRsltMsg is entered!");

    /* 分配空间并检验分配是否成功 */
    pstStopDtmfRsltInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_SPM_CALL_STOP_DTMF_RSLT_IND_STRU));
    if (VOS_NULL_PTR == pstStopDtmfRsltInd)
    {
        IMSA_ERR_LOG("IMSA_CallSendSpmStopDtmfRsltMsg: alloc memory fail");
        return VOS_FALSE;
    }

    /* 清空 */
    IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstStopDtmfRsltInd), 0, IMSA_GET_MSG_LENGTH(pstStopDtmfRsltInd));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstStopDtmfRsltInd, ID_IMSA_SPM_CALL_STOP_DTMF_RSLT_IND);

    pstStopDtmfRsltInd->usClientId = usClientId;
    pstStopDtmfRsltInd->ucOpId     = ucOpId;
    pstStopDtmfRsltInd->enCause    = ulResult;

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstStopDtmfRsltInd);

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallSendSpmSupsResultMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT32                          ulOpId,
    VOS_UINT32                          ulCallId,
    MN_CALL_SS_RESULT_ENUM_U8           enSsResult
)
{
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    MN_CALL_EVT_ORIG_CNF_STRU           stOrigCnf       = {0};

    IMSA_INFO_LOG("IMSA_CallSendSpmSupsResultMsg is entered!");

    /* 设置事件类型 */
    stOrigCnf.enEventType = MN_CALL_EVT_SS_CMD_RSLT;

    /* 设置MN_CALL_INFO_STRU中的call id等公共信息 */
    stOrigCnf.stCallInfo.callId   = (MN_CALL_ID_T)ulCallId;
    stOrigCnf.stCallInfo.opId     = (MN_OPERATION_ID_T)ulOpId;
    stOrigCnf.stCallInfo.clientId = usClientId;

    /* 设置命令结果 */
    stOrigCnf.stCallInfo.enSsResult = enSsResult;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    /**
     * 如果该Call Id对应的呼叫实体存在，则根据IMSA呼叫实体内容填充回复结果中的stCallInfo；
     * 如果呼叫实体不存在，则IMSA只保证基本的op id, client id及结果有效
     */
    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(ulCallId);

    if (pstCallEntity)
    {
        IMSA_CallEntity2SpmCallInfo(pstCallEntity, MN_CALL_EVT_SS_CMD_RSLT, &stOrigCnf.stCallInfo);
    }

    /* 调用发送函数 */
    return IMSA_CallSendSpmEventMsg((VOS_VOID *)&stOrigCnf,
                                    sizeof(MN_CALL_EVT_ORIG_CNF_STRU));
}


VOS_UINT32 IMSA_CallSendSpmStateEvt(VOS_UINT32 ulCallId,VOS_UINT16 usClientId,
                                          VOS_UINT32 ulOpId, MN_CALL_EVENT_ENUM_UINT32 enEvt)
{
    IMSA_INFO_LOG("IMSA_CallSendSpmStateEvt is entered!");

    return IMSA_CallSendSpmEvtAndCallInfo(usClientId,
                                          ulOpId,
                                          ulCallId,
                                          enEvt,
                                          TAF_CS_CAUSE_SUCCESS);
}
VOS_VOID IMSA_CallSendSpmLocalAlertingInd(VOS_UINT16 usClientId,
                                            VOS_UINT8 ucIsLocalAlerting)
{
    IMSA_SPM_CHANNEL_INFO_IND_STRU *pstSpmChannelInfoMsg = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallSendSpmLocalAlertingInd is entered!");

    /* 分配空间并检验分配是否成功 */
    pstSpmChannelInfoMsg = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_SPM_CHANNEL_INFO_IND_STRU));
    if (VOS_NULL_PTR == pstSpmChannelInfoMsg)
    {
        IMSA_ERR_LOG("IMSA_CallSendSpmLocalAlertingInd: alloc memory fail");
        return ;
    }

    IMSA_MEM_SET(IMSA_GET_MSG_ENTITY(pstSpmChannelInfoMsg), 0, IMSA_GET_MSG_LENGTH(pstSpmChannelInfoMsg));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstSpmChannelInfoMsg, ID_IMSA_SPM_CHANNEL_INFO_IND);

    /* 设置Client Id */
    pstSpmChannelInfoMsg->usClientId = usClientId;
    /* pstSpmChannelInfoMsg->enChannelEvent = MN_CALL_EVT_CHANNEL_LOCAL_ALERTING; */
    pstSpmChannelInfoMsg->ucIsLocalAlertingFlag = ucIsLocalAlerting;

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstSpmChannelInfoMsg);
}
VOS_VOID IMSA_CallSendSpmEconfNotifyInd
(
    IMSA_CALL_ENTITY_STRU* pstCallEntity
)
{
    IMSA_SPM_CALL_ECONF_NOTIFY_IND_STRU *pstSpmEconfNotifyIndMsg = VOS_NULL_PTR;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;
    VOS_UINT8                           i = 0;

    IMSA_INFO_LOG("IMSA_CallSendSpmEconfNotifyInd is entered!");

    /* 分配空间并检验分配是否成功 */
    pstSpmEconfNotifyIndMsg = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_SPM_CALL_ECONF_NOTIFY_IND_STRU));
    if (VOS_NULL_PTR == pstSpmEconfNotifyIndMsg)
    {
        IMSA_ERR_LOG("IMSA_CallSendSpmEconfNotifyInd: alloc memory fail");
        return ;
    }

    IMSA_MEM_SET(IMSA_GET_MSG_ENTITY(pstSpmEconfNotifyIndMsg), 0, IMSA_GET_MSG_LENGTH(pstSpmEconfNotifyIndMsg));

    /* 填写消息头 */
    IMSA_WRITE_SPM_MSG_HEAD(pstSpmEconfNotifyIndMsg, ID_IMSA_SPM_CALL_ECONF_NOTIFY_IND);

    pstSpmEconfNotifyIndMsg->usClientId = MN_CLIENT_ALL;
    pstSpmEconfNotifyIndMsg->ucOpId = 0;

    pstSpmEconfNotifyIndMsg->ucNumOfCalls = pstCallEntity->ucCurUserNum;

    for (i = 0; i < pstCallEntity->ucCurUserNum; i ++)
    {
        /* 更新与会者状态 */
        pstSpmEconfNotifyIndMsg->astCallInfo[i].enCallState = IMSA_EconfImsState2CsState( pstCallEntity->stEconfCalllist[i].enCallState);

        /* 更新与会者号码 */
        IMSA_CallCalledNumberImsa2CS(&pstCallEntity->stEconfCalllist[i].stCalledNumber, &pstSpmEconfNotifyIndMsg->astCallInfo[i].stCallNumber);

        /* 更新display name */
        IMSA_MEM_CPY(pstSpmEconfNotifyIndMsg->astCallInfo[i].aucDisplaytext,
                     pstCallEntity->stEconfCalllist[i].acAlpha,
                     TAF_IMSA_ALPHA_STRING_SZ);

        /* 更新原因值 */
        if (IMSA_OP_TRUE == pstCallEntity->stEconfCalllist[i].bitOpErrorInfo)
        {
            /* 将IMS错误原因值转化为SPM错误原因值 */
            /* 修改函数入参 */
            enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stEconfCalllist[i].stErrorCode);

            pstSpmEconfNotifyIndMsg->astCallInfo[i].enCause = enSpmErr;
        }
        else
        {
            pstSpmEconfNotifyIndMsg->astCallInfo[i].enCause = TAF_CS_CAUSE_SUCCESS;
        }

    }

    /* 调用消息发送函数 */
    IMSA_SND_MSG(pstSpmEconfNotifyIndMsg);
}


VOS_VOID IMSA_CallEntityUpdateCalledNumber(const MN_CALL_CALLED_NUM_STRU *pstSrcNumber,
                                           IMSA_CALLED_NUMBER_STRU *pstDstNumber)
{
    IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumber is entered!");

    if ((VOS_NULL_PTR == pstSrcNumber) ||
        (VOS_NULL_PTR == pstDstNumber))
    {
        IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumber: null pointer");
        return;
    }

    /* 更新号码类型 */
    if (MN_CALL_TON_INTERNATIONAL == (0x07 & (pstDstNumber->enNumType >> 4)))
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_INTERNATIONAL;
    }
    else
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_NATIONAL;
    }

    /* 更新号码内容，如果转换失败，则结果字符串长度为0 */

    IMSA_MEM_SET(pstDstNumber->aucNumber, 0, sizeof(pstDstNumber->aucNumber));
    /*IMSA_MEM_SET(pstDstNumber->aucNumber, 0, IMSA_CALLED_NUMBER_MAX_NUM);*/
    if (VOS_FALSE == IMSA_UtilBcdNumberToAscii(pstSrcNumber->aucBcdNum, pstSrcNumber->ucNumLen, pstDstNumber->aucNumber))
    {
        pstDstNumber->aucNumber[0] = 0;
    }
}


VOS_VOID IMSA_CallEntityUpdateCallNumberByAscii(const VOS_CHAR *pcAddress,
                                                IMSA_CALL_NUMBER_STRU *pstDstNumber)
{
    IMSA_INFO_LOG("IMSA_CallEntityUpdateCallNumberByAscii is entered!");

    if ((VOS_NULL_PTR == pcAddress) ||
        (VOS_NULL_PTR == pstDstNumber))
    {
        IMSA_INFO_LOG("IMSA_CallEntityUpdateCallNumberByAscii: null pointer");
        return;
    }

    IMSA_MEM_SET(pstDstNumber->aucNumber, 0, sizeof(pstDstNumber->aucNumber));

    /* 更新号码类型 */
    if ('+' == pcAddress[0])
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_INTERNATIONAL;

        /*去掉'+'*/
        IMSA_UtilStrNCpy(pstDstNumber->aucNumber, &pcAddress[1], IMSA_CALL_NUMBER_MAX_NUM);
    }
    else
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_NATIONAL;

        IMSA_UtilStrNCpy(pstDstNumber->aucNumber, pcAddress, IMSA_CALL_NUMBER_MAX_NUM);
    }
}


VOS_VOID IMSA_CallEntityUpdateCalledNumberByAscii(const VOS_CHAR *pcAddress,
                                                  IMSA_CALLED_NUMBER_STRU *pstDstNumber)
{
    IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumberByAscii is entered!");

    if ((VOS_NULL_PTR == pcAddress) ||
        (VOS_NULL_PTR == pstDstNumber))
    {
        IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumberByAscii: null pointer");
        return;
    }

    IMSA_MEM_SET(pstDstNumber->aucNumber, 0, sizeof(pstDstNumber->aucNumber));

    /* 更新号码类型和内容 */
    if ('+' == pcAddress[0])
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_INTERNATIONAL;

        /*去掉'+'*/
        IMSA_UtilStrNCpy(pstDstNumber->aucNumber, &pcAddress[1], IMSA_CALLED_NUMBER_MAX_NUM);
    }
    else
    {
        pstDstNumber->enNumType = IMSA_CALL_NUM_NATIONAL;

        IMSA_UtilStrNCpy(pstDstNumber->aucNumber, pcAddress, IMSA_CALLED_NUMBER_MAX_NUM);
    }
}

VOS_VOID IMSA_CallEntityUpdateCalledNumberByBcd
(
    MN_CALL_CALLED_NUM_STRU            *pstDialNumber,
    IMSA_CALLED_NUMBER_STRU              *pstCalledNumber
)
{
    IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumberByBcd is entered!");

    if ((VOS_NULL_PTR == pstDialNumber) ||
        (VOS_NULL_PTR == pstCalledNumber))
    {
        IMSA_INFO_LOG("IMSA_CallEntityUpdateCalledNumberByBcd: null pointer");
        return;
    }

    if (MN_CALL_TON_INTERNATIONAL == (((pstDialNumber->enNumType)>>4)&0x7))
    {
        pstCalledNumber->enNumType = IMSA_CALL_NUM_INTERNATIONAL;
        IMSA_UtilBcdNumberToAscii(pstDialNumber->aucBcdNum,
                                  pstDialNumber->ucNumLen,
                                  pstCalledNumber->aucNumber);

    }
    else
    {
        pstCalledNumber->enNumType = IMSA_CALL_NUM_NATIONAL;
        IMSA_UtilBcdNumberToAscii(pstDialNumber->aucBcdNum,
                                  pstDialNumber->ucNumLen,
                                  pstCalledNumber->aucNumber);
    }
}



MN_CALL_NUM_TYPE_ENUM_U8 IMSA_CallImsaNumType2SpmNumType(IMSA_CALL_NUM_TYPE_ENUM_UINT8 enNumType)
{
    MN_CALL_NUM_TYPE_ENUM_U8 enSpmNumType = 0;

    IMSA_INFO_LOG("IMSA_CallImsaNumType2SpmNumType is entered!");

    switch (enNumType)
    {
    case IMSA_CALL_NUM_NATIONAL:
        enSpmNumType = (MN_CALL_IS_EXIT | (MN_CALL_TON_NATIONAL << 4) | MN_CALL_NPI_ISDN);
        break;
    case IMSA_CALL_NUM_INTERNATIONAL:
        enSpmNumType = (MN_CALL_IS_EXIT | (MN_CALL_TON_INTERNATIONAL << 4) | MN_CALL_NPI_ISDN);
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallImsaNumType2SpmNumType: invalid number type", enNumType);
        enSpmNumType = (MN_CALL_IS_EXIT | (MN_CALL_TON_UNKNOWN << 4) | MN_CALL_NPI_UNKNOWN);
    }

    return enSpmNumType;
}


VOS_VOID IMSA_CallCallNumberImsa2CS(const IMSA_CALL_NUMBER_STRU *pstSrcNumber,
                                    MN_CALL_BCD_NUM_STRU *pstDstNumber)
{
    IMSA_INFO_LOG("IMSA_CallCallNumberImsa2CS is entered!");

    /* 更新号码类型 */
    pstDstNumber->enNumType = IMSA_CallImsaNumType2SpmNumType(pstSrcNumber->enNumType);

    /* 更新号码内容 */
    if (VOS_FALSE == IMSA_UtilAsciiNumberToBcd(pstSrcNumber->aucNumber, pstDstNumber->aucBcdNum, &pstDstNumber->ucNumLen))
    {
        pstDstNumber->ucNumLen = 0;
    }
}


VOS_VOID IMSA_CallCalledNumberImsa2CS(const IMSA_CALLED_NUMBER_STRU *pstSrcNumber,
                                      MN_CALL_CALLED_NUM_STRU *pstDstNumber)
{
    IMSA_INFO_LOG("IMSA_CallCalledNumberImsa2CS is entered!");

    /* 更新号码类型 */
    pstDstNumber->enNumType = IMSA_CallImsaNumType2SpmNumType(pstSrcNumber->enNumType);

    /* 更新号码内容 */
    if (VOS_FALSE == IMSA_UtilAsciiNumberToBcd(pstSrcNumber->aucNumber, pstDstNumber->aucBcdNum, &pstDstNumber->ucNumLen))
    {
        pstDstNumber->ucNumLen = 0;
    }
}



VOS_UINT32 IMSA_ProcSsacInfo
(
    MN_CALL_TYPE_ENUM_UINT8             enCallType
)
{
    IMSA_LRRC_SSAC_INFO_STRU            stSsacInfo;
    VOS_UINT32                          ulRand1 = 0;
    VOS_UINT32                          ulRand2 = 0;
    VOS_UINT32                          ulTxLen = 0;
    VOS_UINT32                          ulRand3 = 0;
    VOS_UINT32                          ulRand4 = 0;
    VOS_UINT32                          ulTyLen = 0;
    IMSA_CALL_MANAGER_STRU              *pstImsaCallCtx  = VOS_NULL_PTR;

    pstImsaCallCtx   = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_ProcSsacInfo is entered!");

    if(MN_CALL_TYPE_EMERGENCY == enCallType)
    {
        IMSA_INFO_LOG("IMSA_ProcSsacInfo: emc voice or vedio!");
        return VOS_TRUE;
    }

    if (MN_CALL_TYPE_VIDEO == enCallType)
    {
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstImsaCallCtx->stBackOffTxTimer))
        {
            /*拒绝多媒体业务*/

            return VOS_FALSE;
        }

        /*通过API获取底层的SSAC信息*/
        if (VOS_OK != IMSA_LRRC_GetSSACInfo(&stSsacInfo))
        {
            IMSA_WARN_LOG("IMSA_ProcSsacInfo: vedio get LRRC SSAC Info err!");

            return VOS_TRUE;
        }

        /* 生产一个随机数 fRandom(0--15) */
        ulRand1 = VOS_Rand(16);

        if (ulRand1 < stSsacInfo.ucBarFactorForVideo)
        {
            return VOS_TRUE;
        }
        ulRand2 = VOS_Rand(100);
        ulTxLen = (700 +(6*ulRand2))*(g_ausSsacBarringTimer[stSsacInfo.ucBarTimeForVideo]);
        pstImsaCallCtx->stBackOffTxTimer.ulTimerLen = ulTxLen;
        IMSA_StartTimer(&pstImsaCallCtx->stBackOffTxTimer);
        /*拒绝多媒体业务*/

        return VOS_FALSE;
    }

    if (MN_CALL_TYPE_VOICE == enCallType)
    {
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstImsaCallCtx->stBackOffTyTimer))
        {
            /*拒绝多媒体业务*/

            return VOS_FALSE;
        }

        /*通过API获取底层的SSAC信息*/
        if (VOS_OK != IMSA_LRRC_GetSSACInfo(&stSsacInfo))
        {
            IMSA_WARN_LOG("IMSA_ProcSsacInfo: voice get LRRC SSAC Info err!");

            return VOS_TRUE;
        }

        /* 生产一个随机数 fRandom(0--15) */
        ulRand3 = VOS_Rand(16);
        if (ulRand3 < stSsacInfo.ucBarFactorForVoice)
        {
            return VOS_TRUE;
        }
        ulRand4 = VOS_Rand(100);
        ulTyLen = (700 + (6*ulRand4))*(g_ausSsacBarringTimer[stSsacInfo.ucBarTimeForVoice]);
        pstImsaCallCtx->stBackOffTyTimer.ulTimerLen = ulTyLen;
        IMSA_StartTimer(&pstImsaCallCtx->stBackOffTyTimer);

        /*拒绝多媒体业务*/

        return VOS_FALSE;

    }
    return VOS_TRUE;

}


VOS_VOID IMSA_CallSaveCliendIdOpidWhenAnswerCall
(
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU     *pstAppMsg
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();
    VOS_UINT32                          ulLoop      = IMSA_NULL;

    for (ulLoop = 0; ulLoop < IMSA_CALL_MAX_NUM; ulLoop++)
    {
        if (VOS_TRUE != pstCallCtx->astCallEntity[ulLoop].bitOpIsUsed)
        {
            continue;
        }

        if ((IMSA_CALL_STATUS_INCOMING == pstCallCtx->astCallEntity[ulLoop].enStatus)
            || (IMSA_CALL_STATUS_WAITING == pstCallCtx->astCallEntity[ulLoop].enStatus))
        {
            IMSA_INFO_LOG("IMSA_CallSaveCliendIdOpidWhenAnswerCall: save client id and opid!");

            pstCallCtx->astCallEntity[ulLoop].usClientId = pstAppMsg->usClientId;
            pstCallCtx->astCallEntity[ulLoop].ulOpId = pstAppMsg->ucOpId;

            return ;
        }
    }
}



VOS_UINT32 IMSA_CallProcSpmMsgSupsBuidMpty(VOS_UINT32 ulCallId, const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU *pstAppMsg)
{
    VOS_UINT32 ulResult = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsBuidMpty is entered!");


    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsBuidMpty: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsEconfRelUser(VOS_UINT32 ulCallId, const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU *pstAppMsg)
{
    VOS_UINT32 ulResult = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsEconfRelUser is entered!");


    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsEconfRelUser: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsHoldAllExpectX
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32 ulResult = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsHoldAllExpectX is entered!");


    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsHoldAllExpectX: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsHoldActAcptOth
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32 ulResult = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsHoldActAcptOth is entered!");


    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        IMSA_CallSaveCliendIdOpidWhenAnswerCall(pstAppMsg);

        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsHoldActAcptOth: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsRelAllCall
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32                          ulResult        = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    VOS_UINT32                          i               = IMSA_NULL;
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;
    VOS_UINT8                           ucIsNeedSendtoIms = VOS_FALSE;

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsRelAllCall is entered!");

    /* 如果仅存在一个呼叫实体，且未给IMS发送DIAL消息，则不需要通知IMS；
    如果当前存在2路call，一路处于HOLD，一路IMS未上报过DIALING状态，则需要通知IMS
    REL ALL，但是IMS未上报过DIALING状态的呼叫实体，需要IMSA自己释放；SPM需要先收到AT命令解锁。
    才能处理release事件；*/
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            if (IMSA_CALL_INVALID_ID != pstCallCtx->astCallEntity[i].ucImscallId)
            {
                ucIsNeedSendtoIms = VOS_TRUE;
            }
            else
            {
                pstCallEntity = &(pstCallCtx->astCallEntity[i]);
            }
        }
    }

    /* 如果不需要通知SPM，则需要先AT命令解锁，在上报RELEASE事件 */
    if(VOS_FALSE == ucIsNeedSendtoIms)
    {
        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        (VOS_VOID)IMSA_CallSendSpmSupsResultMsg(pstAppMsg->usClientId,
                                                pstAppMsg->ucOpId,
                                                pstAppMsg->stCallMgmtCmd.callId,
                                                MN_CALL_SS_RES_SUCCESS);

        /* 如果请求释放的CALL ID跟重拨CALL ID一致，则直接回复成功，并打断重播流程 */
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
        {
            ucRedialCallIndex   = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
            if (ucRedialCallIndex < IMSA_CALL_MAX_NUM)
            {
                pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                IMSA_CallInterruptRedial(pstCallEntity);
            }
            return ulResult;
        }

         /*  如果请求释放的CALL ID和正在紧急重注册的CALL ID一致，则通知SPM RELEASE 事件 */
        if (IMSA_TRUE == pstCallCtx->ucRetryEmcRegFlag)
        {
            pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->ucCallEntityIndex];


            /* 将IMS错误原因值转化为SPM错误原因值 */
            /* 修改函数入参 */
            enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

            pstCallEntity->ucSpmcallId = pstCallCtx->ucId;

            IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

            /* 清除保存的重注册标识等参数 */
            pstCallCtx->ucRetryEmcRegFlag = IMSA_FALSE;
            pstCallCtx->ucId = IMSA_NULL;
            pstCallCtx->ucCallEntityIndex = IMSA_NULL;

            return ulResult;
        }

        /* 如果只是在收到IMS上报的DIALING状态前，用户挂断电话 */
        IMSA_CallSpmMsgClear();

        if (pstCallEntity)
        {
            /* 设置为用户主动挂断 */
            IMSA_CallUpdateDiscCallDir( pstCallEntity->ucSpmcallId, IMSA_TRUE);

            IMSA_CallReleaseCallCommonProc(pstCallEntity, TAF_CS_CAUSE_SUCCESS);
        }

        return ulResult;
    }

    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        /* 如果有precondition流程，那些还未上报过状态给SPM的incoming和 waiting
           CALL(处于IDLE态)不需要标记，因为这种CALL被释放并不上报给SPM */

        /* 获取非IDLE状态的CALL，并设置这些CALL实体设置为用户主动挂断 */
        for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
        {
            if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
                (IMSA_CALL_STATUS_IDLE != pstCallCtx->astCallEntity[i].enStatus))
            {
                IMSA_CallUpdateDiscCallDir( pstCallCtx->astCallEntity[i].ucSpmcallId,
                                            IMSA_TRUE);
            }
        }


    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsRelAllCall: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsRelCallX
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32                          ulResult        = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT8                           ucRedialCallId  = IMSA_NULL;
    VOS_UINT8                           ucRedialCallIndex   = IMSA_NULL;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsRelCallX is entered!");

    /* 如果请求释放的CALL ID跟重拨CALL ID一致，则直接回复成功，并打断重播流程 */
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
    {
        ucRedialCallId = (pstCallCtx->stRedialIntervelTimer.usPara & 0xff00) >> IMSA_MOVEMENT_8;
        if (ulCallId == (VOS_UINT32)ucRedialCallId)
        {
            /* 直接返回SPM成功 */
            (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                                 pstAppMsg->ucOpId,
                                                 pstAppMsg->stCallMgmtCmd.callId,
                                                 TAF_CS_CAUSE_SUCCESS);

            (VOS_VOID)IMSA_CallSendSpmSupsResultMsg( pstAppMsg->usClientId,
                                                     pstAppMsg->ucOpId,
                                                     pstAppMsg->stCallMgmtCmd.callId,
                                                     MN_CALL_SS_RES_SUCCESS);

            ucRedialCallIndex   = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
            if (ucRedialCallIndex < IMSA_CALL_MAX_NUM)
            {
                pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                IMSA_CallInterruptRedial(pstCallEntity);
            }

            return VOS_TRUE;
        }
    }
    if ((IMSA_TRUE == pstCallCtx->ucRetryEmcRegFlag) && (ulCallId == pstCallCtx->ucId))
    {
        pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->ucCallEntityIndex];

        /* 直接返回SPM成功 */
        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        (VOS_VOID)IMSA_CallSendSpmSupsResultMsg( pstAppMsg->usClientId,
                                                 pstAppMsg->ucOpId,
                                                 pstAppMsg->stCallMgmtCmd.callId,
                                                 MN_CALL_SS_RES_SUCCESS);

        /* 将IMS错误原因值转化为SPM错误原因值 */
        /* 修改函数入参 */
        enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

        pstCallEntity->ucSpmcallId = pstCallCtx->ucId;

        IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

        /* 清除保存的重注册标识等参数 */
        pstCallCtx->ucRetryEmcRegFlag = IMSA_FALSE;
        pstCallCtx->ucId = IMSA_NULL;
        pstCallCtx->ucCallEntityIndex = IMSA_NULL;

        return VOS_TRUE;
    }

    pstCallEntity = IMSA_CallEntityGetUsedBySpmCallId(ulCallId);

    if (pstCallEntity)
    {
        /* 如果在收到IMS上报的DIALING状态前，收到用户的挂断命令，则直接给SPM回复CMD_RSLT，
        不需要通知IMS，等收到IMS的DIALIGN状态时，如果找不到呼叫实体，再通知IMS RELEASE事件 */
        if (IMSA_CALL_INVALID_ID == pstCallEntity->ucImscallId)
        {
            /* 如果有缓存的SPM消息，则直接清除 */
            if ((IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg) &&
                (ID_SPM_IMSA_CALL_ORIG_REQ == pstCallCtx->stSpmMsg.ulSpmMsgId) )
            {
                IMSA_CallSpmMsgClear();
            }

             /* 直接返回SPM成功 */
            (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                                 pstAppMsg->ucOpId,
                                                 pstAppMsg->stCallMgmtCmd.callId,
                                                 TAF_CS_CAUSE_SUCCESS);

            (VOS_VOID)IMSA_CallSendSpmSupsResultMsg(pstAppMsg->usClientId,
                                                    pstAppMsg->ucOpId,
                                                    pstAppMsg->stCallMgmtCmd.callId,
                                                    MN_CALL_SS_RES_SUCCESS);
            /* 设置为用户主动挂断 */
            IMSA_CallUpdateDiscCallDir( pstCallEntity->ucSpmcallId, IMSA_TRUE);

            IMSA_CallReleaseCallCommonProc(pstCallEntity, TAF_CS_CAUSE_SUCCESS);
        }

        else
        {
            ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
            if (VOS_TRUE == ulResult)
            {
                /* 启动保护定时器 */
                IMSA_StartTimer(&pstCallCtx->stProctectTimer);

                (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                                 pstAppMsg->ucOpId,
                                                 pstAppMsg->stCallMgmtCmd.callId,
                                                 TAF_CS_CAUSE_SUCCESS);

                IMSA_CallUpdateDiscCallDir(ulCallId, IMSA_TRUE);
            }
            else
            {
                IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsRelActAcptOth: error occurs, should return failure to SPM");

                (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                                 pstAppMsg->ucOpId,
                                                 pstAppMsg->stCallMgmtCmd.callId,
                                                 TAF_CS_CAUSE_IMSA_ERROR);
            }
        }
    }
    else
    {
        /* 直接返回SPM成功 */
        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        (VOS_VOID)IMSA_CallSendSpmSupsResultMsg(pstAppMsg->usClientId,
                                                pstAppMsg->ucOpId,
                                                pstAppMsg->stCallMgmtCmd.callId,
                                                MN_CALL_SS_RES_SUCCESS);
    }

    return ulResult;
}
VOS_UINT32 IMSA_CallProcSpmMsgSupsRelActAcptOth
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32                          ulResult        = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    VOS_UINT32                          ulNumOfCalls    = IMSA_NULL;
    VOS_UINT32                          aulCallIds[IMSA_CALL_MAX_NUM] = {0};
    VOS_UINT32                          i               = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsRelActAcptOth is entered!");


    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        IMSA_CallSaveCliendIdOpidWhenAnswerCall(pstAppMsg);

        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        /* 获取ACTIVE状态的CALL，并设置这些CALL实体设置为用户主动挂断 */
        IMSA_CallGetCallsByState(   IMSA_CALL_STATUS_ACTIVE,
                                    &ulNumOfCalls,
                                    aulCallIds);

        for (i = 0; i < ulNumOfCalls; i++)
        {
            IMSA_CallUpdateDiscCallDir(aulCallIds[i], IMSA_TRUE);
        }
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsRelActAcptOth: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}


VOS_UINT32 IMSA_CallProcSpmMsgSupsRelHeldOrUdub
(
    VOS_UINT32                              ulCallId,
    const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU  *pstAppMsg
)
{
    VOS_UINT32                          ulResult        = VOS_TRUE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    VOS_UINT32                          ulNumOfCalls    = IMSA_NULL;
    VOS_UINT32                          aulCallIds[IMSA_CALL_MAX_NUM] = {0};
    VOS_UINT32                          i               = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsRelHeldOrUdub is entered!");

    ulResult = IMSA_CallSendImsMsgSups(ulCallId, pstAppMsg);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_SUCCESS);

        /* 获取HOLD状态的CALL，并设置这些CALL实体设置为用户主动挂断 */
        IMSA_CallGetCallsByState(   IMSA_CALL_STATUS_HELD,
                                    &ulNumOfCalls,
                                    aulCallIds);

        for (i = 0; i < ulNumOfCalls; i++)
        {
            IMSA_CallUpdateDiscCallDir(aulCallIds[i], IMSA_TRUE);
        }

        /* 如果有precondition流程，那些还未上报过状态给SPM的incoming和 waiting
           CALL(处于IDLE态)不需要标记，因为这种CALL被释放并不上报给SPM */

        /* 获取WATING状态的CALL，并设置这些CALL实体设置为用户主动挂断 */
        IMSA_CallGetCallsByState(   IMSA_CALL_STATUS_WAITING,
                                    &ulNumOfCalls,
                                    aulCallIds);

        for (i = 0; i < ulNumOfCalls; i++)
        {
            IMSA_CallUpdateDiscCallDir(aulCallIds[i], IMSA_TRUE);
        }

        /* 获取INCOMING状态的CALL，并设置这些CALL实体设置为用户主动挂断 */
        IMSA_CallGetCallsByState(   IMSA_CALL_STATUS_INCOMING,
                                    &ulNumOfCalls,
                                    aulCallIds);

        for (i = 0; i < ulNumOfCalls; i++)
        {
            IMSA_CallUpdateDiscCallDir(aulCallIds[i], IMSA_TRUE);
        }
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcSpmMsgSupsRelHeldOrUdub: error occurs, should return failure to SPM");

        (VOS_VOID)IMSA_CallSendSpmSupsCnfMsg(pstAppMsg->usClientId,
                                             pstAppMsg->ucOpId,
                                             pstAppMsg->stCallMgmtCmd.callId,
                                             TAF_CS_CAUSE_IMSA_ERROR);
    }

    return ulResult;
}


VOS_UINT32 IMSA_CallCheckDtmfKey
(
    VOS_CHAR                           *pckey
)
{
    /* 转换为大写字符 */
    if ( (*pckey >= 'a') && (*pckey <= 'z'))
    {
        *pckey = *pckey - 0x20;
    }

    if (!( (('0' <= *pckey) && ('9' >= *pckey))
         || ('*' == *pckey) || ('#' == *pckey)
         || ('A' == *pckey) || ('B' == *pckey)
         || ('C' == *pckey) || ('D' == *pckey) ))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallIsAllowDtmf
(
    const VOS_UINT8                    *pucCallIds,
    VOS_UINT32                          ulNumOfCalls,
    VOS_UINT8                           ucCallId
)
{
    VOS_UINT8                           ucLoop;

    /* 判断CallID的有效性 0默认为当前可用CallID */
    if (0 != ucCallId)
    {
        for (ucLoop = 0; ucLoop < ulNumOfCalls; ucLoop++)
        {
            if (pucCallIds[ucLoop] == ucCallId)
            {
                break;
            }
        }

        if (ucLoop == ulNumOfCalls)
        {
            return TAF_CS_CAUSE_NO_CALL_ID;
        }
    }

    return TAF_CS_CAUSE_SUCCESS;
}
TAF_CS_CAUSE_ENUM_UINT32  IMSA_CallCheckUserDtmfCallId
(
    VOS_UINT8                           ucCallId
)
{
    VOS_UINT8                           ucLoop                          = IMSA_NULL;
    VOS_UINT32                          ulNumOfCalls                    = IMSA_NULL;
    VOS_UINT8                           aucCallIds[IMSA_CALL_MAX_NUM]   = {0};
    IMSA_CALL_MANAGER_STRU             *pstCallCtx                      = IMSA_CallCtxGet();

    IMSA_MEM_SET(aucCallIds, 0, sizeof(aucCallIds));

    for (ucLoop = 0; ucLoop < IMSA_CALL_MAX_NUM; ucLoop++)
    {
        if (VOS_TRUE != pstCallCtx->astCallEntity[ucLoop].bitOpIsUsed)
        {
            continue;
        }

        #if 0
        if ((IMSA_CALL_STATUS_ACTIVE == pstCallCtx->astCallEntity[ucLoop].enStatus) ||
            (IMSA_CALL_STATUS_ALERTING == pstCallCtx->astCallEntity[ucLoop].enStatus))
        {
            aucCallIds[ulNumOfCalls] = pstCallCtx->astCallEntity[ucLoop].ucSpmcallId;
            ulNumOfCalls++;
        }
        #endif
        aucCallIds[ulNumOfCalls] = pstCallCtx->astCallEntity[ucLoop].ucSpmcallId;
        ulNumOfCalls++;
    }

    if (0 == ulNumOfCalls)
    {
        return TAF_CS_CAUSE_NO_CALL_ID;
    }

    /*
        DTMF发送请求接口约定，
        用户输入的CALL ID是无效值0，指示使用任意CALL ID发送DTMF消息
        用户输入的CALL ID不是无效值0，指示用用户下发的CALL ID发送DTMF消息，所以这个CALL ID
        需要满足发送DTMF消息的条件
    */
    return IMSA_CallIsAllowDtmf(aucCallIds, ulNumOfCalls, ucCallId);
}


VOS_VOID IMSA_CallFillCurrentDtmfInfo
(
    VOS_UINT8                           ucCallId,
    VOS_CHAR                            cKey,
    VOS_UINT16                          usDuration,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucSpmOpid
)
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo  = VOS_NULL_PTR;

    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    pstDtmfCurInfo->ucCallId    = ucCallId;
    pstDtmfCurInfo->cKey        = cKey;
    pstDtmfCurInfo->usOnLength  = usDuration;
    pstDtmfCurInfo->usClientId  = usClientId;
    pstDtmfCurInfo->ucSpmOpId   = ucSpmOpid;
    pstDtmfCurInfo->ulImsOpid   = IMSA_GetImsOpId();
}


MN_CALL_STATE_ENUM_UINT8 IMSA_CallImsaState2SpmState(IMSA_CALL_STATUS_ENUM_UINT8 enImsaState)
{
    MN_CALL_STATE_ENUM_UINT8 enSpmState = MN_CALL_S_BUTT;

    IMSA_INFO_LOG("IMSA_CallImsaState2SpmState is entered!");

    switch (enImsaState)
    {
    case IMSA_CALL_STATUS_IDLE:
        enSpmState = MN_CALL_S_IDLE;
        break;
    case IMSA_CALL_STATUS_DIALING:
        enSpmState = MN_CALL_S_DIALING;
        break;
    case IMSA_CALL_STATUS_TRYING:
        enSpmState = MN_CALL_S_DIALING;
        break;
    case IMSA_CALL_STATUS_ALERTING:
        enSpmState = MN_CALL_S_ALERTING;
        break;
    case IMSA_CALL_STATUS_ACTIVE:
        enSpmState = MN_CALL_S_ACTIVE;
        break;
    case IMSA_CALL_STATUS_INCOMING:
        enSpmState = MN_CALL_S_INCOMING;
        break;
    case IMSA_CALL_STATUS_HELD:
        enSpmState = MN_CALL_S_HELD;
        break;
    case IMSA_CALL_STATUS_WAITING:
        enSpmState = MN_CALL_S_WAITING;
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallImsaState2SpmState: invalid imsa call state", enImsaState);
    }

    return enSpmState;
}


VOS_VOID IMSA_CallEntity2SpmCallInfoParam(const IMSA_CALL_ENTITY_STRU *pstCallEntity,
                                          MN_CALL_INFO_PARAM_STRU *pstCallInfoParam)
{
    IMSA_INFO_LOG("IMSA_CallEntity2SpmCallInfoParam is entered!");

    pstCallInfoParam->callId      = pstCallEntity->ucSpmcallId;
    pstCallInfoParam->enCallDir   = pstCallEntity->enDir;
    pstCallInfoParam->enCallState = IMSA_CallImsaState2SpmState(pstCallEntity->enStatus);
    pstCallInfoParam->enMptyState = pstCallEntity->enMpty;
    pstCallInfoParam->enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    pstCallInfoParam->ucEConferenceFlag = pstCallEntity->ucIsEconfFlag;

    pstCallInfoParam->enCallType = (MN_CALL_TYPE_ENUM_UINT8)pstCallEntity->enType;
    #if 0
    if (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
    {
        pstCallInfoParam->enCallType = MN_CALL_TYPE_EMERGENCY;
    }
    else
    {
        pstCallInfoParam->enCallType = MN_CALL_TYPE_VOICE;
    }
    #endif

    /* 更新主叫号码 */
    IMSA_CallCallNumberImsa2CS(&pstCallEntity->stCallNumber, &pstCallInfoParam->stCallNumber);

    /* 更新被叫号码 */
    IMSA_CallCalledNumberImsa2CS(&pstCallEntity->stCalledNumber, &pstCallInfoParam->stCalledNumber);

    IMSA_CallCallNumberImsa2CS(&pstCallEntity->stConnectNumber, &pstCallInfoParam->stConnectNumber);

}


VOS_VOID IMSA_CallEntity2SpmCallInfo
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    MN_CALL_EVENT_ENUM_UINT32           enEvt,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    IMSA_INFO_LOG("IMSA_CallEntity2SpmCallInfo is entered!");

    pstCallInfo->callId      = pstCallEntity->ucSpmcallId;
    pstCallInfo->enCallDir   = pstCallEntity->enDir;
    pstCallInfo->enCallState = IMSA_CallImsaState2SpmState(pstCallEntity->enStatus);
    pstCallInfo->enMptyState = pstCallEntity->enMpty;

    pstCallInfo->ucEconfFlag = pstCallEntity->ucIsEconfFlag;

    pstCallInfo->enCallType = (MN_CALL_TYPE_ENUM_UINT8)pstCallEntity->enType;
    #if 0
    if (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
    {
        pstCallInfo->enCallType = MN_CALL_TYPE_EMERGENCY;
    }
    else
    {
        pstCallInfo->enCallType = MN_CALL_TYPE_VOICE;
    }
    #endif

    /* 更新主叫号码 */
    IMSA_CallCallNumberImsa2CS(&pstCallEntity->stCallNumber, &pstCallInfo->stCallNumber);

    /* 更新被叫号码 */
    IMSA_CallCalledNumberImsa2CS(&pstCallEntity->stCalledNumber, &pstCallInfo->stCalledNumber);

    /* 只在UE发起MO CALL时，IMSA上报CONNECT事件时，需要填写stConnectNumber */
    if ((MN_CALL_EVT_CONNECT == enEvt) && (MN_CALL_DIR_MO == pstCallEntity->enDir))
    {
        /* 待实现呼叫转移，且有呼叫转移时再上报CONNECT NUMBER */
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stConnectNumber, &pstCallInfo->stConnectNumber);
    }

    /* 更新转移号码 */
    IMSA_CallCallNumberImsa2CS(&pstCallEntity->stRedirectNumber, &pstCallInfo->stRedirectNumber);

    /*如果是RELEASE事件，则需要指示是否是用户挂断*/
    if (MN_CALL_EVT_RELEASED == enEvt)
    {
        pstCallInfo->enCallState                = MN_CALL_S_IDLE;
        pstCallInfo->stDiscDir.ucIsUser         = pstCallEntity->stDiscDir.ucIsUser;
        pstCallInfo->stDiscDir.ucDiscCallFlag   = IMSA_TRUE;
    }

    pstCallInfo->enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;
}



VOS_VOID  IMSA_CallReadCallEntityRedirectInfo
(
    const IMSA_CALL_ENTITY_STRU        *pstCallEntity,
    IMSA_SPM_CALL_GET_CLPR_CNF_STRU    *pstGetClprCnf
)
{
    if (MN_CALL_DIR_MT != pstCallEntity->enDir)
    {
        pstGetClprCnf->ulRet = VOS_ERR;
        return;
    }

    /* 填写redirect信息 */
    /* 只有呼叫转移号码存在的情况下才才需要填写转移号码 */
    if ('\0' != pstCallEntity->stRedirectNumber.aucNumber[0])
    {
        IMSA_CallCallNumberImsa2CS(&pstCallEntity->stRedirectNumber, &pstGetClprCnf->stRedirectInfo.stRedirectNum);

        if (0 != pstGetClprCnf->stRedirectInfo.stRedirectNum.ucNumLen)
        {
            pstGetClprCnf->stRedirectInfo.bitOpRedirectNum   = VOS_TRUE;
        }
    }


    pstGetClprCnf->ulRet = VOS_OK;
    return;
}

/*****************************************************************************
 Function Name  : IMSA_CallRcvImsStartDtmfProtectTimerExp
 Description    : start dmtf保护定时器超时处理
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.lihong 0015001     2013-10-14  Draft Enact
*****************************************************************************/
VOS_VOID  IMSA_CallRcvImsStartDtmfProtectTimerExp( VOS_VOID )
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo = VOS_NULL_PTR;

    /* 判断状态是否异常 */
    if (IMSA_CallGetDtmfState() != IMSA_CALL_DTMF_WAIT_START_CNF)
    {
        IMSA_WARN_LOG("IMSA_CallRcvImsStartDtmfProtectTimerExp: Dtmf State Error!");
        return;
    }

    /* 回复DTMF正式响应事件 */
    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    (VOS_VOID)IMSA_CallSendSpmStartDtmfRsltMsg( pstDtmfCurInfo->usClientId,
                                                pstDtmfCurInfo->ucSpmOpId,
                                                pstDtmfCurInfo->ucCallId,
                                                TAF_CS_CAUSE_IMSA_TIMEOUT);

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    /* 清除缓存 */
    IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_CallRcvImsStopDtmfProtectTimerExp
 Description    : stop dmtf保护定时器超时处理
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : 处理结果

 History        :
      1.lihong 0015001     2013-10-14  Draft Enact
*****************************************************************************/
 VOS_VOID  IMSA_CallRcvImsStopDtmfProtectTimerExp( VOS_VOID )
{
    IMSA_CALL_DTMF_INFO_STRU           *pstDtmfCurInfo  = VOS_NULL_PTR;
    IMSA_CALL_DTMF_STATE_ENUM_UINT8     enDtmfState     = IMSA_CALL_DTMF_STATE_BUTT;

    /* 判断状态是否异常 */
    enDtmfState = IMSA_CallGetDtmfState();
    if ( (enDtmfState != TAF_CALL_DTMF_WAIT_STOP_CNF)
      && (enDtmfState != TAF_CALL_DTMF_WAIT_AUTO_STOP_CNF) )
    {
        IMSA_WARN_LOG("IMSA_CallRcvImsStopDtmfProtectTimerExp: Dtmf State Error!");
        return;
    }

    /* 如果是用户发出的STOP_DTMF需要回复正式响应事件 */
    pstDtmfCurInfo  = IMSA_CallGetCurrentDtmfInfo();
    if (enDtmfState != (IMSA_CALL_DTMF_STATE_ENUM_UINT8)IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF)
    {
        (VOS_VOID)IMSA_CallSendSpmStopDtmfRsltMsg(  pstDtmfCurInfo->usClientId,
                                                    pstDtmfCurInfo->ucSpmOpId,
                                                    pstDtmfCurInfo->ucCallId,
                                                    TAF_CS_CAUSE_IMSA_TIMEOUT);
    }

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_IDLE);

    /* 清除缓存 */
    IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);

    return;
}


VOS_VOID IMSA_CallProcOrigReqProtectTimeout(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT16                          usRedialCallIndex = IMSA_NULL;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;

    /* 如果是重播，则打断重拨流程 */
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialMaxTimer))
    {
        usRedialCallIndex   = pstCallCtx->stRedialMaxTimer.usPara;
        if (usRedialCallIndex < IMSA_CALL_MAX_NUM)
        {
            pstCallEntity = &pstCallCtx->astCallEntity[usRedialCallIndex];
            if (pstCallEntity->ucRedialTimes > 0)
            {
                IMSA_INFO_LOG("IMSA_CallProcImsMsgError:redial orig error");
                #if (FEATURE_ON == FEATURE_PTM)
                IMSA_CallErrRecord(pstCallEntity->stImsError.stErrorCode.usSipStatusCode, pstCallEntity);
                #endif
                IMSA_CallInterruptRedial(pstCallEntity);

                return;
            }
        }
    }
    if (IMSA_TRUE == pstCallCtx->ucRetryEmcRegFlag)
    {
        pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->ucCallEntityIndex];

        /* 将IMS错误原因值转化为SPM错误原因值 */
        /* 修改函数入参 */
        enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

        pstCallEntity->ucSpmcallId = pstCallCtx->ucId;
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallErrRecord(pstCallEntity->stImsError.stErrorCode.usSipStatusCode, pstCallEntity);
        #endif
        IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

        /* 清除保存的重注册标识等参数 */
        pstCallCtx->ucRetryEmcRegFlag = IMSA_FALSE;
        pstCallCtx->ucId = IMSA_NULL;
        pstCallCtx->ucCallEntityIndex = IMSA_NULL;

        return;
    }
    /* imsa在收到SPM发送的ORIG_REQ消息后，立即回复了ORIG_CNF，所以不需要再上报ORIG_CNF事件；
    由于IMSA在收到ORIG_REQ的时候，还给SPM上报了ORIG事件，所以需要通知SPM RELEASE */
    pstCallEntity = IMSA_CallEntityGetUsedByImsCallId(IMSA_CALL_INVALID_ID);
    if (VOS_NULL_PTR != pstCallEntity)
    {
        IMSA_CallReleaseCallCommonProc(pstCallEntity, TAF_CS_CAUSE_SUCCESS);
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_SERVICE_NOT_AVAILABLE, VOS_NULL_PTR);
        #endif
    }
    #if 0
    (VOS_VOID)IMSA_CallSendSpmOrigCnfMsg(   pstCallCtx->stSpmMsg.usClientId,
                                            pstCallCtx->stSpmMsg.ulOpId,
                                            pstCallCtx->stSpmMsg.ulCallId,
                                            TAF_CS_CAUSE_IMSA_TIMEOUT);

    pstCallEntity = IMSA_CallEntityGetUsedByImsCallId(IMSA_CALL_INVALID_ID);
    if (VOS_NULL_PTR != pstCallEntity)
    {
        IMSA_CallEntityFree(pstCallEntity);
    }
    #endif
    return;
}


VOS_VOID IMSA_CallProcTimeoutProtect(const VOS_VOID *pTimerMsg)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx      = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallProcTimeoutProtect is entered!");

    (VOS_VOID)pTimerMsg;

    /* 根据不同命令回复SPM */
    switch (pstCallCtx->stSpmMsg.ulSpmMsgId)
    {
    case ID_SPM_IMSA_CALL_ORIG_REQ:
        IMSA_CallProcOrigReqProtectTimeout();
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_REMOTE, VOS_NULL_PTR);
        #endif
        break;

    case ID_SPM_IMSA_CALL_SUPS_CMD_REQ:
        (VOS_VOID)IMSA_CallSendSpmSupsResultMsg(pstCallCtx->stSpmMsg.usClientId,
                                                pstCallCtx->stSpmMsg.ulOpId,
                                                pstCallCtx->stSpmMsg.ulCallId,
                                                MN_CALL_SS_RES_FAIL);
        break;

    case ID_SPM_IMSA_CALL_GET_CALL_INFO_REQ:
        IMSA_INFO_LOG("IMSA_CallProcTimeoutProtect: this command has handled by IMSA");
        break;

    case ID_SPM_IMSA_CALL_START_DTMF_REQ:
        IMSA_CallRcvImsStartDtmfProtectTimerExp();
        break;

    case ID_SPM_IMSA_CALL_STOP_DTMF_REQ:
        IMSA_CallRcvImsStopDtmfProtectTimerExp();
        break;

    case ID_SPM_IMSA_CALL_GET_CUDR_REQ:
        IMSA_INFO_LOG("IMSA_CallProcTimeoutProtect: this command is not supported by IMSA");
        break;

    /* xiongxianghui00253310 add for conference 20140210 begin */
    case ID_SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ:
        (VOS_VOID)IMSA_CallSendSpmInviteNewPtptCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                      pstCallCtx->stSpmMsg.ulOpId,
                                                      pstCallCtx->stSpmMsg.ulCallId,
                                                      TAF_CS_CAUSE_IMSA_TIMEOUT);

        pstCallEntity = IMSA_CallEntityGetUsedByImsCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntityFree(pstCallEntity);
        }
        break;
    /* xiongxianghui00253310 add for conference 20140210 end */
    case ID_SPM_IMSA_CALL_MODIFY_REQ:
         /* 需要通知SPM 切换流程失败 */
        (VOS_VOID)IMSA_CallSendSpmModifyCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                pstCallCtx->stSpmMsg.ulOpId,
                                                pstCallCtx->stSpmMsg.ulCallId,
                                                TAF_CS_CAUSE_IMSA_TIMEOUT);
        break;
    case ID_SPM_IMSA_CALL_ANSWER_REMOTE_MODIFY_REQ:
        /* 需要通知SPM 切换流程失败 */
        (VOS_VOID)IMSA_CallSendSpmAnswerRemoteModifyCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                            pstCallCtx->stSpmMsg.ulOpId,
                                                            pstCallCtx->stSpmMsg.ulCallId,
                                                            TAF_CS_CAUSE_IMSA_TIMEOUT);
        break;
    case ID_SPM_IMSA_CALL_ECONF_DIAL_REQ:
        /* 需要通知SPMrelease */
        pstCallEntity = IMSA_CallEntityGetUsedByImsCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallReleaseCallCommonProc(pstCallEntity, TAF_CS_CAUSE_SUCCESS);
        }
        break;
    case ID_SPM_IMSA_CALL_ECONF_ADD_USERS_REQ:
        IMSA_CallSendSpmEconfAddUserCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                           pstCallCtx->stSpmMsg.ulOpId,
                                           TAF_CS_CAUSE_IMSA_TIMEOUT);
        break;
    default:
        IMSA_ERR_LOG1("IMSA_CallProcTimeoutProtect: invalid call request", pstCallCtx->stSpmMsg.ulSpmMsgId);
    }

    /* 清除缓存命令 */
    IMSA_CallSpmMsgClear();
    IMSA_CallImsMsgClear();

    return ;
}

/*****************************************************************************
 Function Name  : IMSA_CallProcTimeoutDtmfProtect
 Description    : DTMF保护定时器超时处理
 Input          : pTimerMsg     超时定时器消息
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong 0015001     2013-10-28  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallProcTimeoutDtmfProtect(const VOS_VOID *pTimerMsg)
{
    IMSA_CALL_DTMF_STATE_ENUM_UINT8     enDtmfState = IMSA_CALL_DTMF_STATE_BUTT;

    IMSA_INFO_LOG("IMSA_CallProcTimeoutDtmfProtect is entered!");

    (VOS_VOID)pTimerMsg;

    enDtmfState = IMSA_CallGetDtmfState();
    if (enDtmfState == IMSA_CALL_DTMF_WAIT_START_CNF)
    {
        IMSA_CallRcvImsStartDtmfProtectTimerExp();
    }
    else if ((enDtmfState == IMSA_CALL_DTMF_WAIT_STOP_CNF)
            || (enDtmfState == IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF))
    {
        IMSA_CallRcvImsStopDtmfProtectTimerExp();
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_CallProcTimeoutDtmfProtect: invalid dtmf state", enDtmfState);
    }

    return ;
}

/*****************************************************************************
 Function Name  : IMSA_CallProcTimeoutRedialMaxTime
 Description    : 重播最大时长定时器超时处理
 Input          : pTimerMsg     超时定时器消息
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong 0015001     2013-12-23  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallProcTimeoutRedialMaxTime(const VOS_VOID *pTimerMsg)
{
    IMSA_INFO_LOG1("IMSA_CallProcTimeoutRedialMaxTime: timer para:",PS_GET_REL_TIMER_PARA(pTimerMsg));

    return ;
}

/*****************************************************************************
 Function Name  : IMSA_CallProcTimeoutRedialIntervel
 Description    : 重播间隔定时器超时处理
 Input          : pTimerMsg     超时定时器消息
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong 0015001     2013-12-23  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CallProcTimeoutRedialIntervel(const VOS_VOID *pTimerMsg)
{
    VOS_UINT16                          usTimerPara         = IMSA_NULL;
    VOS_UINT8                           ucCallEntityIndex   = IMSA_NULL;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx          = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity       = VOS_NULL_PTR;
    SPM_IMSA_CALL_ORIG_REQ_STRU         stCallOrigReq       = {0};
    VOS_UINT32                          ulResult            = VOS_FALSE;

    usTimerPara = (VOS_UINT16)PS_GET_REL_TIMER_PARA(pTimerMsg);

    IMSA_INFO_LOG1("IMSA_CallProcTimeoutRedialIntervel: timer para:", usTimerPara);

    ucCallEntityIndex = (VOS_UINT8)usTimerPara;

    /* 识别CALL实体索引的合法性 */
    if (ucCallEntityIndex >= IMSA_CALL_MAX_NUM)
    {
        IMSA_WARN_LOG("IMSA_CallProcTimeoutRedialIntervel: call index illegal");
        IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
        return ;
    }

    pstCallEntity = &pstCallCtx->astCallEntity[ucCallEntityIndex];

    /* 识别CALL状态的合法性 */
    if ((IMSA_CALL_STATUS_DIALING != pstCallEntity->enStatus)
        && (IMSA_CALL_STATUS_TRYING != pstCallEntity->enStatus))
    {
        IMSA_WARN_LOG("IMSA_CallProcTimeoutRedialIntervel: call state illegal");
        IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
        return ;
    }

    /* 重播次数加1 */
    pstCallEntity->ucRedialTimes++;

    IMSA_INFO_LOG1("IMSA_CallProcTimeoutRedialIntervel redial times:", pstCallEntity->ucRedialTimes);

    /* 取存储的ORIG REQ信息，再次发起呼叫建立请求 */
    stCallOrigReq.ulMsgId       = ID_SPM_IMSA_CALL_ORIG_REQ;
    stCallOrigReq.usClientId      = pstCallEntity->stSpmMsg.usClientId;
    stCallOrigReq.ucOpId          = (MN_OPERATION_ID_T)pstCallEntity->stSpmMsg.ulOpId;
    stCallOrigReq.stOrig        = pstCallEntity->stSpmMsg.stParam.stOrigParam;
    ulResult = IMSA_CallSendImsMsgDial(IMSA_CALL_INVALID_ID, &stCallOrigReq);
    if (VOS_TRUE == ulResult)
    {
        /* 启动保护定时器 */
        IMSA_StartTimer(&pstCallCtx->stProctectTimer);

        IMSA_MEM_CPY(   &pstCallCtx->stSpmMsg,
                        &pstCallEntity->stSpmMsg,
                        sizeof(IMSA_CALL_SPM_MSG_STRU));
    }
    else
    {
        IMSA_WARN_LOG("IMSA_CallProcTimeoutRedialIntervel: send ims dial failed");

        IMSA_CallInterruptRedial(pstCallEntity);
    }

    return ;
}
VOS_VOID IMSA_CallProcTimeoutResReady(const VOS_VOID *pTimerMsg)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    VOS_UINT32 i = 0;
    IMSA_CALL_CALL_REASON_RESOURCE_RESULT_ENUM_UINT8 enResResult = IMSA_CALL_CALL_REASON_RESOURCE_FAILED;

    IMSA_INFO_LOG("IMSA_CallProcTimeoutResReady is entered!");

    (VOS_VOID)pTimerMsg;

    /**
     * 等待资源预留定时器超时处理
     *
     * 由于目前的实现是D2 IMS上报资源预留请求后，
     * IMSA本地启动等待资源预留成功定时器；
     * 如果在第一个呼叫的资源预留还未响应之前又来第二个资源预留的请求，
     * 则IMSA的处理只是刷新定时器。
     *
     * 因此受到等待资源定时器超时后的处理，目前是
     * 遍历所有呼叫，如果该呼叫正在等待资源预留，
     * 则回复该呼叫资源预留失败
     *
     * 目前这部分的处理是个简化处理，后续需要再优化
     *
     */

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpNeedSendResResult))
        {
            /* 只有被叫的场景下，才有可能存在承载在收到IMS的资源预留请求之前建立 */
            if (MN_CALL_DIR_MO == pstCallCtx->astCallEntity[i].enDir)
            {
                enResResult = IMSA_CallIsResourceReady(&pstCallCtx->astCallEntity[i]);
            }
            else
            {
                /* 为避免第二路电话呼叫时，网侧未下发MODIFY，或者MT CALL时，网侧建立承载比IMS信令流程快，
                在资源预留定时器超时后，增加判断是否有对应类型的承载存在，如果存在，则通知IMS资源预留结果为
                成功 */

                enResResult = IMSA_CALL_CALL_REASON_RESOURCE_FAILED;

                if (((IMSA_CALL_TYPE_VOICE == pstCallCtx->astCallEntity[i].enType) ||
                     (IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType)) &&
                    (IMSA_TRUE == IMSA_CONN_HasActiveVoicePdp()))
                {
                    enResResult = IMSA_CALL_CALL_REASON_RESOURCE_READY;
                }

                if ((IMSA_CALL_TYPE_VIDEO == pstCallCtx->astCallEntity[i].enType) &&
                    (IMSA_TRUE == IMSA_CONN_HasActiveVoicePdp()) &&
                    (IMSA_TRUE == IMSA_CONN_HasActiveVideoPdp()))
                {
                    enResResult = IMSA_CALL_CALL_REASON_RESOURCE_READY;
                }

            }

            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallCtx->astCallEntity[i].ucImscallId,
                                                pstCallCtx->astCallEntity[i].ulResRspOpId,
                                                enResResult);

            pstCallCtx->astCallEntity[i].bitOpNeedSendResResult = IMSA_FALSE;

            if (IMSA_CALL_CALL_REASON_RESOURCE_FAILED == enResResult)
            {
                /* 如果是资源预留失败，记录资源预留定时器超时的标识 */
                pstCallCtx->astCallEntity[i].ucTqosExpFlag = VOS_TRUE;
            }

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_RES_READY_FAIL, VOS_NULL_PTR);
            #endif
        }
    }

    return ;
}
VOS_VOID IMSA_CallProcTimeoutDtmfDuration(const VOS_VOID *pTimerMsg)
{
    VOS_UINT32                          ulResult    = VOS_FALSE;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx  = IMSA_CallCtxGet();

    (VOS_VOID)pTimerMsg;

    /* 判断状态是否异常 */
    if (IMSA_CallGetDtmfState() != IMSA_CALL_DTMF_WAIT_TIME_OUT)
    {
        IMSA_WARN_LOG("IMSA_CallProcTimeoutDtmfDuration: Dtmf State Error!");
        return;
    }

    /* 先判断当前是否允许发起DTMF，以及用户输入的呼叫ID是否可以发送DTMF */
    ulResult    = IMSA_CallCheckUserDtmfCallId(pstCallCtx->stDtmfCtx.stCurrentDtmf.ucCallId);
    if (TAF_CS_CAUSE_SUCCESS != ulResult)
    {
        IMSA_WARN_LOG("IMSA_CallProcTimeoutDtmfDuration: no active call!");
        IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);

        return;
    }

    if (VOS_TRUE != IMSA_CallSendImsMsgStopDtmf(pstCallCtx->stDtmfCtx.stCurrentDtmf.ucCallId,
                                                pstCallCtx->stDtmfCtx.stCurrentDtmf.cKey))
    {
        IMSA_ERR_LOG("IMSA_CallProcTimeoutDtmfDuration: IMSA_CallSendImsMsgStopDtmf failed!");
        IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_UNKNOWN);

        return ;
    }

    /* 发送成功，更新DTMF状态 */
    IMSA_StartTimer(&pstCallCtx->stProctectTimer);

    IMSA_CallFillCurrentDtmfInfo(   pstCallCtx->stDtmfCtx.stCurrentDtmf.ucCallId,
                                    pstCallCtx->stDtmfCtx.stCurrentDtmf.cKey,
                                    0,
                                    pstCallCtx->stDtmfCtx.stCurrentDtmf.usClientId,
                                    pstCallCtx->stDtmfCtx.stCurrentDtmf.ucSpmOpId);

    IMSA_CallSetDtmfState(IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF);

    return;
}
VOS_VOID IMSA_CallProcEmcTcallExpWhenEmcSrvEstablishSucc(VOS_VOID)
{
    VOS_UINT32  i = 0;
    VOS_UINT32  ulIsTimeOut  = IMSA_FALSE;
    IMSA_CALL_MANAGER_STRU   *pstCallCtx   = IMSA_CallCtxGet();

    IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
            && (IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType))
        {
            if(IMSA_CALL_STATUS_DIALING == pstCallCtx->astCallEntity[i].enStatus)
            {
                ulIsTimeOut = IMSA_TRUE;
                break;
            }
        }
    }

    if(IMSA_CALL_MAX_NUM == i)
    {
        return;
    }

    if(ulIsTimeOut)
    {
        (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucImscallId);
    }

    pstCallCtx->ucIsTcallTimeOutProc = VOS_TRUE;

    return;
}



VOS_VOID IMSA_CallProcTimeoutEmcTCall(const VOS_VOID *pTimerMsg)
{
    if (IMSA_FALSE == IMSA_IsImsEmcServiceEstablishSucc())
    {
        IMSA_StopImsEmcService();
    }
    else
    {
        IMSA_CallProcEmcTcallExpWhenEmcSrvEstablishSucc();
    }
}




VOS_VOID IMSA_CallProcTimeoutTCall(const VOS_VOID *pTimerMsg)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulIsDialingTimeOut = IMSA_FALSE;
    IMSA_CALL_MANAGER_STRU   *pstCallCtx      = IMSA_CallCtxGet();

    IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
    IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);

    for(i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if(VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            if(IMSA_CALL_STATUS_DIALING == pstCallCtx->astCallEntity[i].enStatus)
            {
                ulIsDialingTimeOut = IMSA_TRUE;
                break;
            }
            else if(IMSA_CALL_STATUS_IDLE == pstCallCtx->astCallEntity[i].enStatus)
            {
                break;
            }
        }
    }

    if(IMSA_CALL_MAX_NUM == i)
    {
        IMSA_ERR_LOG("IMSA_CallProcTimeoutTCall: Call Entity not found.");
        return;
    }

    if(ulIsDialingTimeOut)
    {
        IMSA_INFO_LOG1("IMSA_CallProcTimeoutTCall: Hang up call id: ", pstCallCtx->astCallEntity[i].ucImscallId);
        (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucImscallId);
    }
    pstCallCtx->ucIsTcallTimeOutProc = VOS_TRUE;
}


VOS_VOID IMSA_StartTcallTimer(const IMSA_CALL_ENTITY_STRU *pstCallEntity)
{
    IMSA_CALL_MANAGER_STRU    *pstCallCtx = IMSA_CallCtxGet();

    if(VOS_TRUE == IMSA_GetConfigParaAddress()->stCMCCCustomReq.ucCMCCCustomTcallFlag)
    {
        if (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
        {
            /* 启动紧急呼叫Tcall定时器 */
            pstCallCtx->stEmcTcallTimer.ulTimerLen = IMSA_GetConfigParaAddress()->stCMCCCustomReq.ulTcallTimerLen;
            pstCallCtx->stEmcTcallTimer.usPara = pstCallEntity->ucCallEntityIndex;
            IMSA_StartTimer(&pstCallCtx->stEmcTcallTimer);
        }
        else
        {
            /* 启动普通呼叫或视频呼叫Tcall定时器 */
            pstCallCtx->stNormalTcallTimer.ulTimerLen = IMSA_GetConfigParaAddress()->stCMCCCustomReq.ulTcallTimerLen;
            pstCallCtx->stNormalTcallTimer.usPara = pstCallEntity->ucCallEntityIndex;
            IMSA_StartTimer(&pstCallCtx->stNormalTcallTimer);
        }
    }
}

VOS_VOID IMSA_CallProcIntraMsgEmcCallSrvStatusWhenNoSrv
(
    const IMSA_CALL_NO_SRV_CAUSE_ENUM_UINT32          enNoSrvCause
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity       = VOS_NULL_PTR;

    /* 如果紧急服务不能使用，则尝试释放所有紧急会话.
       如果是缓存的紧急呼，由于还没有分配CALL实体，所以实际上不会
       请求IMS协议栈释放紧急呼 */
    if ((IMSA_CALL_NO_SRV_CAUSE_HIFI_EXCEPTION == enNoSrvCause)
        || (IMSA_CALL_NO_SRV_CAUSE_NON_SRVCC_RAT_CHANGE == enNoSrvCause))
    {
        (VOS_VOID)IMSA_CallCmdRelAllEmc(IMSA_TRUE);
    }
    else
    {
        (VOS_VOID)IMSA_CallCmdRelAllEmc(IMSA_FALSE);
    }

    /**
     * 如果当前缓存了发起紧急呼的命令，则直接回复SPM失败；
     * 这里也有可能缓存其它命令，这种场景下页需要回复SPM结果，
     * 目前暂时先处理最常见的紧急呼的情况，其它需要结合具体命令来分别考虑
     */
    if ((IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg) &&
        (ID_SPM_IMSA_CALL_ORIG_REQ == pstCallCtx->stSpmMsg.ulSpmMsgId) &&
        (MN_CALL_TYPE_EMERGENCY == pstCallCtx->stSpmMsg.stParam.stOrigParam.enCallType))
    {
        /* 由于收到SPM发送的ORIG_REQ消息，已经给SPM发送过ORIG_CNF，同时上报了ORIG事件，所以
        不需要再回复ORIG_CNF，而是需要上报RELEASE事件 */
        pstCallEntity = IMSA_CallEntityGetUsedByImsCallId(IMSA_CALL_INVALID_ID);

        if (pstCallEntity)
        {
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_CallErrRecord(IMSA_ERR_LOG_CALL_FAIL_REASON_SERVICE_NOT_AVAILABLE, pstCallEntity);
            #endif
            IMSA_CallReleaseCallCommonProc(pstCallEntity, TAF_CS_CAUSE_SUCCESS);
        }

        #if 0
        (VOS_VOID)IMSA_CallSendSpmOrigCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                             pstCallCtx->stSpmMsg.ulOpId,
                                             pstCallCtx->stSpmMsg.ulCallId,
                                             TAF_CS_CAUSE_IMSA_SERVICE_NOT_AVAILABLE);
        #endif
        IMSA_CallSpmMsgClear();
    }
    return;
}

VOS_VOID IMSA_CallProcIntraMsgEmcCallSrvStatusWhenNorSrv
(
    VOS_VOID
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    SPM_IMSA_CALL_ORIG_REQ_STRU *pstAppReq = VOS_NULL_PTR;
    IMSA_CALL_ENTITY_STRU              *pstCallEntity       = VOS_NULL_PTR;
    SPM_IMSA_CALL_ORIG_REQ_STRU         stCallOrigReq       = {0};
    VOS_UINT32                          ulResult            = VOS_FALSE;
    TAF_CS_CAUSE_ENUM_UINT32            enSpmErr        = IMSA_NULL;
    VOS_UINT8                           i= 0;

    /* 如果紧急服务可用，则看是否有缓存的紧急呼叫建立请求，如果有，则紧急呼叫建立流程 */

    if ((IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg) &&
        (ID_SPM_IMSA_CALL_ORIG_REQ == pstCallCtx->stSpmMsg.ulSpmMsgId) &&
        (MN_CALL_TYPE_EMERGENCY == pstCallCtx->stSpmMsg.stParam.stOrigParam.enCallType))
    {
        pstAppReq = (SPM_IMSA_CALL_ORIG_REQ_STRU*)IMSA_MEM_ALLOC(sizeof(SPM_IMSA_CALL_ORIG_REQ_STRU));
        if(VOS_NULL_PTR == pstAppReq)
        {
            IMSA_ERR_LOG("IMSA_CallProcIntraMsgEmcCallSrvStatus: Mem Alloc fail!");
            return;
        }
        pstAppReq->ulMsgId      = ID_SPM_IMSA_CALL_ORIG_REQ;
        pstAppReq->usClientId   = pstCallCtx->stSpmMsg.usClientId;
        pstAppReq->ucOpId         = (MN_OPERATION_ID_T)pstCallCtx->stSpmMsg.ulOpId;
        pstAppReq->stOrig       = pstCallCtx->stSpmMsg.stParam.stOrigParam;

        #if 0
        (VOS_VOID)IMSA_CallProcSpmMsgOrig(pstAppReq);

        #endif
        ulResult = IMSA_CallSendImsMsgDial(IMSA_CALL_INVALID_ID, pstAppReq);
        if (VOS_TRUE == ulResult)
        {
            /* 启动保护定时器 */
            IMSA_StartTimer(&pstCallCtx->stProctectTimer);
        }
        else
        {
            /* 如果紧急Tcall还在运行，需要停止 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stEmcTcallTimer))
            {
                IMSA_StopTimer(&pstCallCtx->stEmcTcallTimer);
            }

            /* 通知SPM RELEASE事件 */
            /* 查找紧急呼叫的呼叫实体 */
            for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
            {
                if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
                    (IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType))
                {
                    IMSA_CallReleaseCallCommonProc(&(pstCallCtx->astCallEntity[i]), TAF_CS_CAUSE_IMSA_ERROR);
                    IMSA_CallSpmMsgClear();
                    break;
                }
            }
        }
        IMSA_MEM_FREE(pstAppReq);
        return;
    }

    /* 如果是紧急呼叫，收到#380，重新发起紧急注册，并且成功的场景，不需要重新分配呼叫实体 */
    if (IMSA_TRUE == pstCallCtx->ucRetryEmcRegFlag)
    {
        IMSA_ERR_LOG1("IMSA_CallProcIntraMsgEmcCallSrvStatus: pstCallCtx->ucRetryEmcRegFlag = ",pstCallCtx->ucRetryEmcRegFlag);
        pstCallEntity = &pstCallCtx->astCallEntity[pstCallCtx->ucCallEntityIndex];

        /* 取存储的ORIG REQ信息，再次发起呼叫建立请求 */
        stCallOrigReq.ulMsgId       = ID_SPM_IMSA_CALL_ORIG_REQ;
        stCallOrigReq.usClientId      = pstCallEntity->stSpmMsg.usClientId;
        stCallOrigReq.ucOpId          = (MN_OPERATION_ID_T)pstCallEntity->stSpmMsg.ulOpId;
        stCallOrigReq.stOrig        = pstCallEntity->stSpmMsg.stParam.stOrigParam;
        ulResult = IMSA_CallSendImsMsgDial(IMSA_CALL_INVALID_ID, &stCallOrigReq);
        if (VOS_TRUE == ulResult)
        {
            /* 启动保护定时器 */
            IMSA_StartTimer(&pstCallCtx->stProctectTimer);

            IMSA_MEM_CPY(   &pstCallCtx->stSpmMsg,
                            &pstCallEntity->stSpmMsg,
                            sizeof(IMSA_CALL_SPM_MSG_STRU));
        }
        else
        {
            /* 将IMS错误原因值转化为SPM错误原因值 */
            /* 修改函数入参 */
            enSpmErr = IMSA_CallTransImsErr2SpmErr(&pstCallEntity->stImsError.stErrorCode);

            pstCallEntity->ucSpmcallId = pstCallCtx->ucId;

            IMSA_CallReleaseCallCommonProc(pstCallEntity, enSpmErr);

            /* 清除保存的重注册标识等参数 */
            pstCallCtx->ucRetryEmcRegFlag = IMSA_FALSE;
            pstCallCtx->ucId = IMSA_NULL;
            pstCallCtx->ucCallEntityIndex = IMSA_NULL;
        }
        IMSA_ERR_LOG1("IMSA_CallProcIntraMsgEmcCallSrvStatus: pstCallCtx->ucRetryEmcRegFlag = ",pstCallCtx->ucRetryEmcRegFlag);
    }
    return;
}
VOS_UINT32 IMSA_CallProcIntraMsgEmcCallSrvStatus(const VOS_VOID *pMsg)
{
    IMSA_EMC_CALL_SRV_STATUS_IND_STRU *pstSrvStatus = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallProcIntraMsgEmcCallSrvStatus is entered!");

    pstSrvStatus = (IMSA_EMC_CALL_SRV_STATUS_IND_STRU *)pMsg;

    if (IMSA_CALL_SERVICE_STATUS_NO_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        IMSA_CallProcIntraMsgEmcCallSrvStatusWhenNoSrv(pstSrvStatus->enNoSrvCause);
    }
    else if (IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        IMSA_CallProcIntraMsgEmcCallSrvStatusWhenNorSrv();
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcIntraMsgEmcCallSrvStatus: invalid srv status");
    }

    return VOS_TRUE;
}


VOS_UINT32 IMSA_CallProcIntraMsgNrmCallSrvStatus(const VOS_VOID *pMsg)
{
    IMSA_NRM_CALL_SRV_STATUS_IND_STRU *pstSrvStatus = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallProcIntraMsgNrmCallSrvStatus is entered!");

    if (VOS_NULL_PTR == pMsg)
    {
        IMSA_ERR_LOG("IMSA_CallProcIntraMsgNrmCallSrvStatus: NULL message");
        return VOS_FALSE;
    }

    pstSrvStatus = (IMSA_NRM_CALL_SRV_STATUS_IND_STRU *)pMsg;

    if (IMSA_CALL_SERVICE_STATUS_NO_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        /* 如果普通服务不能使用，则尝试释放所有普通会话 */
        if ((IMSA_CALL_NO_SRV_CAUSE_HIFI_EXCEPTION == pstSrvStatus->enNoSrvCause)
            || (IMSA_CALL_NO_SRV_CAUSE_NON_SRVCC_RAT_CHANGE == pstSrvStatus->enNoSrvCause))
        {
            (VOS_VOID)IMSA_CallCmdRelAllNormal(IMSA_TRUE);
        }
        else
        {
            (VOS_VOID)IMSA_CallCmdRelAllNormal(IMSA_FALSE);
        }
    }
    else if (IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        /* 如果普通服务可用，不做任何特殊处理 */
    }
    else
    {
        IMSA_ERR_LOG("IMSA_CallProcIntraMsgNrmCallSrvStatus: invalid srv status");
    }

    return VOS_TRUE;
}


/*****************************************************************************
 Function Name  : IMSA_ProcHifiExceptionNtf()
 Description    : HIFI异常消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-22  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcHifiExceptionNtf(VOS_VOID *pRcvMsg)
{
    VC_IMSA_HIFI_EXCEPTION_NTF_STRU    *pstHifiExption = VOS_NULL_PTR;

    pstHifiExption = (VC_IMSA_HIFI_EXCEPTION_NTF_STRU *)pRcvMsg;

    if (VC_IMSA_EXCEPTION_CAUSE_STARTED == pstHifiExption->enCause)
    {
        IMSA_NORM_LOG("IMSA_ProcHifiExceptionNtf,hifi already started!");
        return ;
    }

    /* 通知CALL模块正常服务进入无服务状态，需要释放普通CALL */
    IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                    IMSA_CALL_NO_SRV_CAUSE_HIFI_EXCEPTION);

    /* 通知CALL模块紧急服务进入无服务状态，需要释放紧急CALL */
    IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                    IMSA_CALL_NO_SRV_CAUSE_HIFI_EXCEPTION);
}

/*****************************************************************************
 Function Name  : IMSA_ProcCallSrvccSatusNoify()
 Description    : CALL模块SRVCC指示的处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-10-15  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcCallSrvccSatusNoify(const VOS_VOID *pRcvMsg)
{

    CALL_IMSA_SRVCC_STATUS_NOTIFY_STRU    *pstSrvccStatusNotify;

    IMSA_INFO_LOG("IMSA_ProcCallSrvccSatusNoify is entered!");

    pstSrvccStatusNotify = (CALL_IMSA_SRVCC_STATUS_NOTIFY_STRU*)pRcvMsg;

    if(CALL_IMSA_SRVCC_STATUS_START ==pstSrvccStatusNotify->enSrvccStatus)
    {
        /*记录等待异系统指示标示*/
        IMSA_CallSetSrvccFlag(IMSA_TRUE);

        /* 记录不能上报ALL RELEASED事件 */
        IMSA_CallSetNotReportAllReleasedFlag(IMSA_TRUE);

        /*AT上报SRVCC启动*/
        IMSA_SndMsgAtCirephInd(AT_IMSA_SRVCC_HANDOVER_STARTED);

        /*通知IMS协议栈 SRVCC启动*/
        (VOS_VOID)IMSA_CallSendImsMsgSrvcc(IMSA_IMS_INPUT_CALL_REASON_SRVCC_START);
    }
    else if(CALL_IMSA_SRVCC_STATUS_SUCCESS ==pstSrvccStatusNotify->enSrvccStatus)
    {
        /*AT上报SRVCC成功*/
        IMSA_SndMsgAtCirephInd(AT_IMSA_SRVCC_HANDOVER_SUCCESS);

        /*通知IMS协议栈 SRVCC成功*/
        (VOS_VOID)IMSA_CallSendImsMsgSrvcc(IMSA_IMS_INPUT_CALL_REASON_SRVCC_SUCCESS);

        IMSA_USSD_ClearResource();

        /* 清除DTMF缓存 */
        IMSA_CallSrvccSuccClearDtmfInfo(TAF_CS_CAUSE_IMSA_SRVCC_SUCC);
        /* 由于代码调整，该标志无需再在收到SERVICE_CHANGE_IND时清除，转移到SRVCC成功时清除 */
        IMSA_CallSetSrvccFlag(IMSA_FALSE);
    }
    else /*CALL_IMSA_SRVCC_STATUS_FAIL*/
    {
        /*清除等待异系统指示标示*/
        IMSA_CallSetSrvccFlag(IMSA_FALSE);

        /* 清除不能上报ALL RELEASED事件标识 */
        IMSA_CallSetNotReportAllReleasedFlag(IMSA_FALSE);

        /*AT上报SRVCC失败，底层handover失败*/
        IMSA_SndMsgAtCirephInd(AT_IMSA_SRVCC_HANDOVER_CANCEL);

        /*清除SRVCC转到CS域的呼叫*/
        IMSA_CallClearTransToCsInfo();

        /*通知IMS协议栈 SRVCC失败*/
        (VOS_VOID)IMSA_CallSendImsMsgSrvcc(IMSA_IMS_INPUT_CALL_REASON_SRVCC_FAILED);

        IMSA_SrvccFailBuffProc();
    }

}


/*****************************************************************************
 Function Name  : IMSA_ProcTafCallMsg
 Description    : TAF CALL下发请求处理
 Input          : pRcvMsg      CALL下发的命令及参数
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-10-11  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcTafCallMsg(const VOS_VOID *pRcvMsg)
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU          *pHeader = VOS_NULL_PTR;

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;

    switch(pHeader->ulMsgName)
    {
        case ID_CALL_IMSA_SRVCC_STATUS_NOTIFY:
            IMSA_ProcCallSrvccSatusNoify(pRcvMsg);
            break;
        default:
            IMSA_ERR_LOG("IMSA_ProcTafCallMsg: Not support call message");
            break;
    }

}


VOS_VOID IMSA_CallSetSrvccFlag
(
    VOS_UINT32                          ulSrvccFlag
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    pstCallCtx->ulSrvccFlag = ulSrvccFlag;
}


VOS_UINT32 IMSA_CallGetSrvccFlag( VOS_VOID )
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    return pstCallCtx->ulSrvccFlag;
}


VOS_VOID IMSA_CallSetNotReportAllReleasedFlag
(
    VOS_UINT32                          ulNotReportAllReleasdFlag
)
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    pstCallCtx->ulNotReprotAllReleasedFlag = ulNotReportAllReleasdFlag;
}


VOS_UINT32 IMSA_CallGetNotReportAllReleasedFlag( VOS_VOID )
{
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    return pstCallCtx->ulNotReprotAllReleasedFlag;
}


VOS_UINT32 IMSA_IsCallConnExist(VOS_VOID)
{

    VOS_UINT16                          i = 0;
    IMSA_CALL_MANAGER_STRU             *pstCallCtx = IMSA_CallCtxGet();

    for (i = 0; i < IMSA_CALL_MAX_NUM; i ++)
    {
        if (IMSA_OP_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 IMSA_LMM_IsExitImsService(VOS_VOID)
{
    return IMSA_IsImsExist();
}

#if (FEATURE_ON == FEATURE_PTM)

VOS_VOID IMSA_SndCallErrLogInfo
(
    IMSA_ERR_LOG_CALL_FAIL_STRU  stImsCallRstEvent
)
{
    IMSA_CALL_ERROR_LOG_INFO_STRU  *pstMsg = VOS_NULL_PTR;

    pstMsg = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_CALL_ERROR_LOG_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        IMSA_ERR_LOG("IMSA_SndCallErrLogInfo:ERROR:Alloc Mem Fail.");
        return;
    }

     /*清空*/
    IMSA_MEM_SET(IMSA_GET_MSG_ENTITY(pstMsg), 0, \
                             IMSA_GET_MSG_LENGTH(pstMsg));

    IMSA_WRITE_OM_MSG_HEAD( pstMsg,
                            ID_IMSA_OM_CALL_ERROR_LOG_IND);

    IMSA_MEM_CPY(&pstMsg->stImsaCallErrlog, &stImsCallRstEvent, sizeof(IMSA_ERR_LOG_CALL_FAIL_STRU));


#if(VOS_WIN32 == VOS_OS_VER)
    /*PC测试，将消息发送出去，用于ST验证*/
    IMSA_SND_MSG(pstMsg);

#else
    /*消息勾到HSO上*/
    (VOS_VOID)LTE_MsgHook(pstMsg);

    /*释放消息空间*/
    IMSA_FREE_MSG(pstMsg);

#endif

    return;
}


VOS_VOID IMSA_CallErrRecord
(
    IMSA_ERR_LOG_CALL_FAIL_REASON_ENUM_UINT16       enCallFailReason,
    IMSA_CALL_ENTITY_STRU *pstCallEntity
)
{
    IMSA_ERR_LOG_CALL_FAIL_STRU                             stImsCallRstEvent;
    VOS_UINT32                                              ulIsLogRecord;
    VOS_UINT32                                              ulLength;
    VOS_UINT32                                              ulResult;
    VOS_UINT16                                              usLevel;
    IMSA_REG_MANAGER_STRU              *pstRegCtx = IMSA_RegCtxGet();
    VOS_CHAR                            aucUeAddr[IMSA_IPV6_ADDR_STRING_LEN+1] = {0};
    VOS_CHAR                            aucPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN+1] = {0};


    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel       = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_CALL_FAIL_EVENT);
    ulIsLogRecord = IMSA_IsErrLogNeedRecord(usLevel);
    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    ulLength = sizeof(IMSA_ERR_LOG_REG_FAIL_STRU);

    /* 填充CS注册失败异常信息 */
    PS_MEM_SET(&stImsCallRstEvent, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsCallRstEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_CALL_FAIL_EVENT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));
    if (VOS_NULL_PTR == pstCallEntity)
    {
        stImsCallRstEvent.enCallStatus = IMSA_ERR_LOG_CALL_STATUS_IDLE;
        stImsCallRstEvent.enMpty = IMSA_ERR_LOG_CALL_NOT_IN_MPTY;
    }
    else
    {
        stImsCallRstEvent.enCallStatus = IMSA_CallImsaState2ErrlogState(pstCallEntity->enStatus);
        stImsCallRstEvent.enMpty = IMSA_CallImsaMpty2ErrlogMpty(pstCallEntity->enMpty);
    }

    stImsCallRstEvent.enCallFailReason      = enCallFailReason;
    stImsCallRstEvent.enEmcRegStatus        = IMSA_RegState2ErrlogState(pstRegCtx->stEmcRegEntity.enStatus);
    stImsCallRstEvent.enNormRegStatus       = IMSA_RegState2ErrlogState(pstRegCtx->stNormalRegEntity.enStatus);
    stImsCallRstEvent.enPdnConnStatus       = IMSA_ConnState2ErrlogConnState(IMSA_CONN_GetNormalConnStatus());
    stImsCallRstEvent.enVopsStatus          = IMSA_VoPsState2ErrlogVoPsState(IMSA_GetNwImsVoiceCap());
    stImsCallRstEvent.ucIsVoiceMediaExist   = (VOS_UINT8)IMSA_CONN_HasActiveVoicePdp();
    stImsCallRstEvent.ucIsVideoMediaExist   = (VOS_UINT8)IMSA_CONN_HasActiveVideoPdp();

    if (VOS_TRUE == IMSA_RegAddrPairMgrGetCurrent(IMSA_REG_TYPE_NORMAL, aucUeAddr, aucPcscfAddr))
    {
        IMSA_MEM_CPY(stImsCallRstEvent.aucPcscfAddr,
                     aucPcscfAddr,
                     IMSA_IPV6_ADDR_STRING_LEN+1);
    }

    /* 勾包到HIDS */
    IMSA_SndCallErrLogInfo(stImsCallRstEvent);

    /*
       将异常信息写入Buffer中
       实际写入的字符数与需要写入的不等则打印异常
     */
    ulResult = IMSA_PutErrLogRingBuf((VOS_CHAR *)&stImsCallRstEvent, ulLength);
    if (ulResult != ulLength)
    {
        IMSA_ERR_LOG("IMSA_CallErrRecord: Push buffer error.");
    }

    return;
}
#endif

/*lint +e961*/
/*lint +e960*/

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaCallManagement.c */



