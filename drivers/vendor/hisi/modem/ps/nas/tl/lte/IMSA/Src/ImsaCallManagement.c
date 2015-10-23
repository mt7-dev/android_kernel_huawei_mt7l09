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
        ulServiceRet = IMSA_StartImsNormalService();
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

    /* DTMF缓存buffer初始化 */
    IMSA_CallDtmfInfoInit();

    return VOS_TRUE;
}
VOS_UINT32 IMSA_CallCtxDeinit(VOS_VOID)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    IMSA_CALL_ENTITY_STRU              *pstCallEntity   = VOS_NULL_PTR;
    VOS_UINT16                          usRedialCallIndex = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CallCtxDeinit is entered!");

    /* 清除DTMF信息 */
    IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_POWER_OFF);

    /* Stop active call */
    if (IMSA_CallEntityGetUsedCount() > 0)
    {
        (VOS_VOID)IMSA_CallImsCmdRelAll();
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

            pstCallCtx->astCallEntity[i].bitOpResReady = IMSA_OP_TRUE;

            pstCallCtx->astCallEntity[i].usClientId = MN_CLIENT_ALL;
            pstCallCtx->astCallEntity[i].ulOpId = 0;
            pstCallCtx->astCallEntity[i].ucId = IMSA_CALL_INVALID_ID;

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



IMSA_CALL_ENTITY_STRU* IMSA_CallEntityGetUsedByCallId(VOS_UINT32 ulCallId)
{
    VOS_UINT32 i = 0;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_CallEntityGetUsedByCallId is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            ((VOS_UINT8)ulCallId == pstCallCtx->astCallEntity[i].ucId))
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

    pstCallEntity = IMSA_CallEntityGetUsedByCallId(ulCallId);

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
            pulCallIds[*pulNumOfCalls] = (VOS_UINT32)pstCallCtx->astCallEntity[i].ucId;
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


VOS_VOID IMSA_ProcCallResourceIsReady
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    VOS_UINT32 i = 0;

    IMSA_INFO_LOG("IMSA_ProcCallResourceIsReady is entered!");

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        IMSA_INFO_LOG("IMSA_ProcCallResourceIsReady:normal media pdp modify!");
        pstCallCtx->ulIsMediaPdpReady = IMSA_TRUE;
    }
    else
    {
        IMSA_INFO_LOG("IMSA_ProcCallResourceIsReady:emc media pdp modify!");
        pstCallCtx->ulIsEmcMediaPdpReady = IMSA_TRUE;
    }

    /* 通知各呼叫实体资源预留成功 */
    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (VOS_FALSE == pstCallCtx->astCallEntity[i].bitOpResReady))
        {
            IMSA_INFO_LOG("IMSA_ProcCallResourceIsReady:Info ims!");

            pstCallCtx->astCallEntity[i].bitOpResReady = IMSA_OP_TRUE;

            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallCtx->astCallEntity[i].ucId,
                                                pstCallCtx->astCallEntity[i].ulResRspOpId,
                                                VOS_TRUE);
        }
    }

    /* 停止等待资源预留结果定时 */
    if (IMSA_IsTimerRunning(&pstCallCtx->stResReadyTimer))
    {
        IMSA_StopTimer(&pstCallCtx->stResReadyTimer);
    }
}


VOS_VOID IMSA_ProcCallResourceIsNotReady
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType
)
{
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();

    IMSA_INFO_LOG("IMSA_ProcCallResourceIsNotReady is entered!");

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstCallCtx->ulIsMediaPdpReady = IMSA_FALSE;
    }
    else
    {
        pstCallCtx->ulIsEmcMediaPdpReady = IMSA_FALSE;
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


VOS_VOID IMSA_CallConverterSups2Ims(VOS_UINT32 ulCallId,
                                    const MN_CALL_SUPS_PARAM_STRU*  pSpmParam,
                                    IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_INFO_LOG("IMSA_CallConverterSups2Ims is entered!");

    (VOS_VOID)ulCallId;

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置呼叫OpId */
    pstInputEvent->evt.stInputCallEvent.ulOpId = IMSA_AllocImsOpId();

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
                pstInputEvent->evt.stInputCallEvent.ulCallIndex = pSpmParam->callId;
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
                pstInputEvent->evt.stInputCallEvent.ulCallIndex = pSpmParam->callId;
                break;
            }
        /* CHLD=3, Adds a held call to the conversation */
        case MN_CALL_SUPS_CMD_BUILD_MPTY:
            {
                pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_CONFERENCE;
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
VOS_VOID IMSA_CallConverterResRsp2Ims(VOS_UINT32 ulImsOpId,
                                      VOS_UINT32 ulCallId,
                                      VOS_UINT32 ulSuccess,
                                      IMSA_IMS_INPUT_EVENT_STRU *pstInputEvent)
{
    IMSA_INFO_LOG("IMSA_CallConverterResRsp2Ims is entered!");

    pstInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_CALL;

    /* 设置OpId */
    pstInputEvent->evt.stInputCallEvent.ulOpId = ulImsOpId;

    if (VOS_TRUE == ulSuccess)
    {
        pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RESOURCE_READY;
    }
    else
    {
        pstInputEvent->evt.stInputCallEvent.enInputCallReason = IMSA_IMS_INPUT_CALL_REASON_RESOURCE_FAILED;
    }

    pstInputEvent->evt.stInputCallEvent.bitOpCallIndex = IMSA_OP_TRUE;
    pstInputEvent->evt.stInputCallEvent.ulCallIndex = ulCallId;
}
VOS_UINT32 IMSA_CallSendImsMsgResRsp(VOS_UINT32 ulCallId, VOS_UINT32 ulImsOpId, VOS_UINT32 ulSuccess)
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
                                 ulSuccess,
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
            (MN_CALL_IN_MPTY == pstCallCtx->astCallEntity[i].enMpty))
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

    (VOS_VOID)IMSA_CallImsMsgSave(&pstInputEvent->evt.stInputCallEvent);

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
    pstCallEntity->ucId = (VOS_UINT8)((pstCallCtx->stRedialIntervelTimer.usPara & 0xff00) >> IMSA_MOVEMENT_8);

    /* 停止重播最大时长定时器和重播间隔定时器 */
    IMSA_StopTimer(&pstCallCtx->stRedialMaxTimer);
    IMSA_StopTimer(&pstCallCtx->stRedialIntervelTimer);

    /* 将IMS错误原因值转化为SPM错误原因值 */
    enSpmErr = IMSA_CallTransImsErr2SpmErr(pstCallEntity->stImsError.stErrorCode.usSipStatusCode);

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

    IMSA_INFO_LOG("IMSA_CallCmdRelAllEmc is entered!");

    for (i = 0; i < IMSA_CALL_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstCallCtx->astCallEntity[i].bitOpIsUsed) &&
            (IMSA_CALL_TYPE_EMC == pstCallCtx->astCallEntity[i].enType))
        {
            /* 如果要释放的CALL正好是重播CALL，则打断重拨流程 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
            {
                ucRedialCallIndex = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
                if (i == ucRedialCallIndex)
                {
                    pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                    IMSA_CallInterruptRedial(pstCallEntity);

                    continue;
                }
            }

            IMSA_CallUpdateDiscCallDir( (VOS_UINT32)pstCallCtx->astCallEntity[i].ucId,
                                        ucIsUserDisc);

            (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucId);
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
            (IMSA_CALL_TYPE_EMC != pstCallCtx->astCallEntity[i].enType))
        {
            /* 如果要释放的CALL正好是重播CALL，则打断重拨流程 */
            if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
            {
                ucRedialCallIndex = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
                if (i == ucRedialCallIndex)
                {
                    pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
                    IMSA_CallInterruptRedial(pstCallEntity);

                    continue;
                }
            }

            IMSA_CallUpdateDiscCallDir( (VOS_UINT32)pstCallCtx->astCallEntity[i].ucId,
                                        ucIsUserDisc);

            (VOS_VOID)IMSA_CallImsCmdRel(pstCallCtx->astCallEntity[i].ucId);
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
    stOrigCnf.stCallInfo.callId   = (MN_CALL_ID_T)ulCallId;
    stOrigCnf.stCallInfo.opId     = (MN_OPERATION_ID_T)ulOpId;
    stOrigCnf.stCallInfo.clientId = usClientId;

    /* 设置命令结果 */
    stOrigCnf.stCallInfo.enCause  = ulResult;

    stOrigCnf.stCallInfo.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;

    /**
     * 如果该Call Id对应的呼叫实体存在，则根据IMSA呼叫实体内容填充回复结果中的stCallInfo；
     * 如果呼叫实体不存在，则IMSA只保证基本的op id, client id及结果有效
     */
    pstCallEntity = IMSA_CallEntityGetUsedByCallId(ulCallId);
    if (pstCallEntity)
    {
        IMSA_CallEntity2SpmCallInfo(pstCallEntity, enEvt, &stOrigCnf.stCallInfo);
    }

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
    stOrigCnf.stCallInfo.callId   = (MN_CALL_ID_T)pstCallEntity->ucId;
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
        IMSA_INFO_LOG("IMSA_CallSendSpmAllReleasedEvt:srvcc not report all released!");
        IMSA_CallSetNotReportAllReleasedFlag(IMSA_FALSE);
        return VOS_TRUE;
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
    pstCallEntity = IMSA_CallEntityGetUsedByCallId(ulCallId);
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

    IMSA_INFO_LOG("IMSA_CallProcSpmMsgSupsRelAllCall is entered!");

    /* 如果请求释放的CALL ID跟重拨CALL ID一致，则直接回复成功，并打断重播流程 */
    if (VOS_TRUE == IMSA_IsTimerRunning(&pstCallCtx->stRedialIntervelTimer))
    {
        ucRedialCallIndex   = (VOS_UINT8)pstCallCtx->stRedialIntervelTimer.usPara;
        if (ucRedialCallIndex < IMSA_CALL_MAX_NUM)
        {
            pstCallEntity = &pstCallCtx->astCallEntity[ucRedialCallIndex];
            IMSA_CallInterruptRedial(pstCallEntity);
        }
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
                IMSA_CallUpdateDiscCallDir( pstCallCtx->astCallEntity[i].ucId,
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

    pstCallEntity = IMSA_CallEntityGetUsedByCallId(ulCallId);
    if (pstCallEntity)
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

        if (IMSA_CALL_STATUS_ACTIVE == pstCallCtx->astCallEntity[ucLoop].enStatus)
        {
            aucCallIds[ulNumOfCalls] = pstCallCtx->astCallEntity[ucLoop].ucId;
            ulNumOfCalls++;
        }
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


    pstCallInfoParam->callId      = pstCallEntity->ucId;
    pstCallInfoParam->enCallDir   = pstCallEntity->enDir;
    pstCallInfoParam->enCallState = IMSA_CallImsaState2SpmState(pstCallEntity->enStatus);
    pstCallInfoParam->enMptyState = pstCallEntity->enMpty;
    pstCallInfoParam->enVoiceDomain = TAF_CALL_VOICE_DOMAIN_IMS;
    if (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
    {
        pstCallInfoParam->enCallType = MN_CALL_TYPE_EMERGENCY;
    }
    else
    {
        pstCallInfoParam->enCallType = MN_CALL_TYPE_VOICE;
    }

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

    pstCallInfo->callId      = pstCallEntity->ucId;
    pstCallInfo->enCallDir   = pstCallEntity->enDir;
    pstCallInfo->enCallState = IMSA_CallImsaState2SpmState(pstCallEntity->enStatus);
    pstCallInfo->enMptyState = pstCallEntity->enMpty;
    if (IMSA_CALL_TYPE_EMC == pstCallEntity->enType)
    {
        pstCallInfo->enCallType = MN_CALL_TYPE_EMERGENCY;
    }
    else
    {
        pstCallInfo->enCallType = MN_CALL_TYPE_VOICE;
    }

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
    if (enDtmfState != IMSA_CALL_DTMF_WAIT_AUTO_STOP_CNF)
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
        (VOS_VOID)IMSA_CallSendSpmOrigCnfMsg(   pstCallCtx->stSpmMsg.usClientId,
                                                pstCallCtx->stSpmMsg.ulOpId,
                                                pstCallCtx->stSpmMsg.ulCallId,
                                                TAF_CS_CAUSE_IMSA_TIMEOUT);

        pstCallEntity = IMSA_CallEntityGetUsedByCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntityFree(pstCallEntity);
        }

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

        pstCallEntity = IMSA_CallEntityGetUsedByCallId(IMSA_CALL_INVALID_ID);
        if (VOS_NULL_PTR != pstCallEntity)
        {
            IMSA_CallEntityFree(pstCallEntity);
        }
        break;
    /* xiongxianghui00253310 add for conference 20140210 end */
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
            (VOS_FALSE == pstCallCtx->astCallEntity[i].bitOpResReady))
        {
            (VOS_VOID)IMSA_CallSendImsMsgResRsp(pstCallCtx->astCallEntity[i].ucId,
                                                pstCallCtx->astCallEntity[i].ulResRspOpId,
                                                VOS_FALSE);
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
VOS_UINT32 IMSA_CallProcIntraMsgEmcCallSrvStatus(const VOS_VOID *pMsg)
{
    IMSA_EMC_CALL_SRV_STATUS_IND_STRU *pstSrvStatus = VOS_NULL_PTR;
    IMSA_CALL_MANAGER_STRU *pstCallCtx = IMSA_CallCtxGet();
    SPM_IMSA_CALL_ORIG_REQ_STRU *pstAppReq = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CallProcIntraMsgEmcCallSrvStatus is entered!");

    pstSrvStatus = (IMSA_EMC_CALL_SRV_STATUS_IND_STRU *)pMsg;

    if (IMSA_CALL_SERVICE_STATUS_NO_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        /* 如果紧急服务不能使用，则尝试释放所有紧急会话.
           如果是缓存的紧急呼，由于还没有分配CALL实体，所以实际上不会
           请求IMS协议栈释放紧急呼 */
        if ((IMSA_CALL_NO_SRV_CAUSE_HIFI_EXCEPTION == pstSrvStatus->enNoSrvCause)
            || (IMSA_CALL_NO_SRV_CAUSE_NON_SRVCC_RAT_CHANGE == pstSrvStatus->enNoSrvCause))
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
            (VOS_VOID)IMSA_CallSendSpmOrigCnfMsg(pstCallCtx->stSpmMsg.usClientId,
                                                 pstCallCtx->stSpmMsg.ulOpId,
                                                 pstCallCtx->stSpmMsg.ulCallId,
                                                 TAF_CS_CAUSE_IMSA_SERVICE_NOT_AVAILABLE);

            IMSA_CallSpmMsgClear();
        }
    }
    else if (IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE == pstSrvStatus->enCallSrvStatus)
    {
        /* 如果紧急服务可用，则看是否有缓存的紧急呼叫建立请求，如果有，则紧急呼叫建立流程 */

        if ((IMSA_OP_TRUE == pstCallCtx->stSpmMsg.bitOpSpmMsg) &&
            (ID_SPM_IMSA_CALL_ORIG_REQ == pstCallCtx->stSpmMsg.ulSpmMsgId) &&
            (MN_CALL_TYPE_EMERGENCY == pstCallCtx->stSpmMsg.stParam.stOrigParam.enCallType))
        {
            pstAppReq = (SPM_IMSA_CALL_ORIG_REQ_STRU*)IMSA_MEM_ALLOC(sizeof(SPM_IMSA_CALL_ORIG_REQ_STRU));
            if(VOS_NULL_PTR == pstAppReq)
            {
                IMSA_ERR_LOG("IMSA_CallProcIntraMsgEmcCallSrvStatus: Mem Alloc fail!");
                return VOS_FALSE;
            }
            pstAppReq->ulMsgId      = ID_SPM_IMSA_CALL_ORIG_REQ;
            pstAppReq->usClientId   = pstCallCtx->stSpmMsg.usClientId;
            pstAppReq->ucOpId         = (MN_OPERATION_ID_T)pstCallCtx->stSpmMsg.ulOpId;
            pstAppReq->stOrig       = pstCallCtx->stSpmMsg.stParam.stOrigParam;

            (VOS_VOID)IMSA_CallProcSpmMsgOrig(pstAppReq);
            IMSA_MEM_FREE(pstAppReq);
        }
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
/*lint +e961*/
/*lint +e960*/

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaCallManagement.c */



