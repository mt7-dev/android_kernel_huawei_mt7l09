



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RrmCtrl.h"
#include "RrmDebug.h"
#include "Rrm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RRM_CTRL_C

#if (FEATURE_ON == FEATURE_DSDS)
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* RRM模块处理来自协议栈消息函数对应表*/
const RRM_MSG_PROC_STRU                 g_astRrmPsMsgProcTab[] =
{
    {ID_PS_RRM_RADIO_RESOURCE_APPLY_REQ,    RRM_RcvRadioResourceApplyReq},   /* 资源申请请求 */
    {ID_PS_RRM_RADIO_RESOURCE_RELEASE_IND,  RRM_RcvRadioResourceReleaseInd}, /* 资源释放通知 */
    {ID_PS_RRM_REGISTER_IND,                RRM_RcvRegisterInd},             /* 注册通知 */
    {ID_PS_RRM_DEREGISTER_IND,              RRM_RcvDeregisterInd}            /* 去注册通知 */
};


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 RRM_CheckMsgParam
(
    MODEM_ID_ENUM_UINT16                    enModemId,
    RRM_PS_RAT_TYPE_ENUM_UINT8              enRatType
)
{
    /* MODEM ID 合法性检查 */
    if (enModemId >= MODEM_ID_BUTT)
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "enModemId is error.", enModemId);
        return VOS_ERR;
    }

    /* RAT type 合法性检查 */
    if (enRatType >= RRM_PS_RAT_TYPE_BUTT)
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "enRatType is error.", enRatType);
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_VOID RRM_SndPSStatusInd(VOS_UINT32  ulReceiverPid)
{
    VOS_UINT32                          ulLength;
    RRM_PS_STATUS_IND_STRU             *pstPSStatusInd = VOS_NULL_PTR;


    if (RRM_PID_DEFAULT_VALUE == ulReceiverPid)
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "ulReceiverPid is error!");
        return;
    }


    ulLength = sizeof(RRM_PS_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 分配消息空间 */
    pstPSStatusInd = (RRM_PS_STATUS_IND_STRU *)PS_ALLOC_MSG(UEPS_PID_RRM, ulLength);
    if (VOS_NULL_PTR == pstPSStatusInd)
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "Alloc msg fail!");
        return;
    }

    /* 清消息空间 */
    PS_MEM_SET((VOS_UINT8*)pstPSStatusInd + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填充消息 */
    pstPSStatusInd->stMsgHeader.ulReceiverPid   = ulReceiverPid;
    pstPSStatusInd->stMsgHeader.ulSenderPid     = UEPS_PID_RRM;
    pstPSStatusInd->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstPSStatusInd->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstPSStatusInd->stMsgHeader.ulMsgName       = ID_RRM_PS_STATUS_IND;
    pstPSStatusInd->stMsgHeader.ulLength        = sizeof(RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    RRM_DBG_SND_STATUS_IND_NUM(1);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_RRM, pstPSStatusInd))
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "Snd Msg err.");
        return;
    }

    return;
}




VOS_VOID RRM_SndRadioResourceApplyCnf
(
    VOS_UINT32                          ulRecievePid,
    PS_RRM_RESULT_ENUM_UINT8            enResult
)
{
    VOS_UINT32                              ulLength;
    RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU   *pstRadioResourceApplyCnf = VOS_NULL_PTR;


    ulLength = sizeof(RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 分配消息空间 */
    pstRadioResourceApplyCnf = (RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU *)PS_ALLOC_MSG(UEPS_PID_RRM, ulLength);
    if (VOS_NULL_PTR == pstRadioResourceApplyCnf)
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "Alloc msg fail!");
        return;
    }

    /* 清消息空间 */
    PS_MEM_SET((VOS_UINT8*)pstRadioResourceApplyCnf + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填充消息 */
    pstRadioResourceApplyCnf->stMsgHeader.ulReceiverPid     = ulRecievePid;
    pstRadioResourceApplyCnf->stMsgHeader.ulSenderPid       = UEPS_PID_RRM;
    pstRadioResourceApplyCnf->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstRadioResourceApplyCnf->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstRadioResourceApplyCnf->stMsgHeader.ulMsgName         = ID_RRM_PS_RADIO_RESOURCE_APPLY_CNF;
    pstRadioResourceApplyCnf->stMsgHeader.ulLength          = sizeof(RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstRadioResourceApplyCnf->enResult                      = enResult;


    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_RRM, pstRadioResourceApplyCnf))
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "Snd Msg err.");
        return;
    }

    return;

}



VOS_VOID RRM_RcvRadioResourceApplyReq(VOS_VOID * pstMsg)
{
    VOS_UINT32                          ulPid;
    VOS_UINT32                          ulRslt;
    MODEM_ID_ENUM_UINT16                enModemId;
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType;
    VOS_UINT16                          usRFIDValue;
    PS_BOOL_ENUM_UINT8                  enSameModem;
    RRM_RFID_ENUM_UINT16                enRFIDIndex;
    PS_BOOL_ENUM_UINT8                  enRFIDIsUsed;
    PS_RRM_RADIO_RESOURCE_APPLY_REQ_STRU   *pstRadioResApplyReq;


    /* 参数检查 */
    if (VOS_NULL_PTR == pstMsg)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "pMsg is null.");
        return;
    }

    pstRadioResApplyReq = (PS_RRM_RADIO_RESOURCE_APPLY_REQ_STRU *)pstMsg;
    ulPid               = pstRadioResApplyReq->stMsgHeader.ulSenderPid;
    enModemId           = pstRadioResApplyReq->enModemId;
    RRM_DBG_MODEM_RX_RES_APPLY_REQ_NUM(enModemId, 1);
    if (PS_FALSE == RRM_GetModemSupportFlg(enModemId))
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "modem don't support.", enModemId);
        RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
        RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

        return;
    }

    enRatType   = pstRadioResApplyReq->enRatType;

    /* 消息合法性检查 */
    ulRslt  = RRM_CheckMsgParam(enModemId, enRatType);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "Msg is invalid.");
        RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
        RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

        return;
    }

    usRFIDValue = RRM_GetRFIDCfgValue(enModemId, enRatType);
    if (RRM_RFID_DEFAULT_VALUE == usRFIDValue)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, enRatType is <2>.", enModemId, enRatType);
        RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
        RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

        return;
    }

    /* RFID资源是否在使用判断 */
    ulRslt  = RRM_JudgeRFIDIsUsed(enModemId, usRFIDValue, &enSameModem, &enRFIDIndex, &enRFIDIsUsed);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, usRFIDValue is <2>.", enModemId, usRFIDValue);
        RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
        RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

        return;
    }

    if (PS_TRUE == enRFIDIsUsed)
    {
        /* 是否是同一MODEM来的消息 */
        if (PS_TRUE == enSameModem)
        {
            /* 停止VOS定时器 */
            RRM_StopTimer(enRFIDIndex);

            /* RFID资源当前MODEM正在使用，同一MODEM申请资源回复成功 */
            RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_SUCCESS);
            RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

            return;
        }
        else
        {
            RRM_WARNING_LOG(UEPS_PID_RRM, "It is not the same modem.");

            /* RFID资源当前MODEM正在使用，其他MODEM申请资源回复失败 */
            RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
            RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);

            return;
        }
    }
    else
    {
        /*** RFID资源没有在使用 ***/
        /* 设置该RFID被使用 */
        ulRslt = RRM_SetRFIDUsedInfo(PS_TRUE, usRFIDValue, enModemId);
        if (VOS_OK == ulRslt)
        {
            /* 向申请资源回复成功 */
            RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_SUCCESS);
            RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);
        }
        else
        {
            /* 向申请资源回复失败 */
            RRM_SndRadioResourceApplyCnf(ulPid, PS_RRM_RESULT_FAIL);
            RRM_DBG_MODEM_SND_RES_APPLY_CNF_NUM(enModemId, 1);
        }

        return;
    }

}
VOS_VOID RRM_RcvRadioResourceReleaseInd(VOS_VOID * pstMsg)
{
    VOS_UINT32                          ulRslt;
    MODEM_ID_ENUM_UINT16                enModemId;
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType;
    VOS_UINT16                          usRFIDValue;
    PS_BOOL_ENUM_UINT8                  enSameModem;
    RRM_RFID_ENUM_UINT16                enRFIDIndex;
    PS_BOOL_ENUM_UINT8                  enRFIDIsUsed;
    PS_RRM_RADIO_RESOURCE_RELEASE_IND_STRU   *pstRadioResRelInd;


    /* 参数检查 */
    if (VOS_NULL_PTR == pstMsg)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "pMsg is null.");
        return;
    }

    pstRadioResRelInd = (PS_RRM_RADIO_RESOURCE_RELEASE_IND_STRU *)pstMsg;
    enModemId   = pstRadioResRelInd->enModemId;
    RRM_DBG_MODEM_RX_RES_RELEASE_NUM(enModemId, 1);
    if (PS_FALSE == RRM_GetModemSupportFlg(enModemId))
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "modem don't support.", enModemId);
        return;
    }

    enRatType   = pstRadioResRelInd->enRatType;
    /* 消息合法性检查 */
    ulRslt  = RRM_CheckMsgParam(enModemId, enRatType);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "Msg is invalid.");
        return;
    }

    usRFIDValue = RRM_GetRFIDCfgValue(enModemId, enRatType);
    if (RRM_RFID_DEFAULT_VALUE == usRFIDValue)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, enRatType is <2>.", enModemId, enRatType);
        return;
    }

    /* RFID资源是否在使用判断 */
    ulRslt  = RRM_JudgeRFIDIsUsed(enModemId, usRFIDValue, &enSameModem, &enRFIDIndex, &enRFIDIsUsed);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, usRFIDValue is <2>.", enModemId, usRFIDValue);
        return;
    }

    if (PS_TRUE == enRFIDIsUsed)
    {
        /* 是否是同一MODEM来的消息 */
        if (PS_TRUE == enSameModem)
        {
            /* 启动定时器 */
            RRM_StartTimer(enRFIDIndex);
            return;
        }
        else
        {
            RRM_WARNING_LOG(UEPS_PID_RRM, "It is not the same modem.");
            return;
        }
    }
    else
    {
        /* RFID资源没有在使用 */
        RRM_WARNING_LOG(UEPS_PID_RRM, "usRFIDValue is not used.");
        return;
    }

}



VOS_VOID RRM_RcvRegisterInd(VOS_VOID * pstMsg)
{
    VOS_UINT32                          ulPid;
    VOS_UINT32                          ulRslt;
    MODEM_ID_ENUM_UINT16                enModemId;
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType;
    PS_BOOL_ENUM_UINT8                  enSameModem;
    VOS_UINT16                          usRFIDValue;
    VOS_UINT16                          usRegInfoIndex;
    PS_BOOL_ENUM_UINT8                  enRegInfoIsReged;
    PS_RRM_REGISTER_IND_STRU           *pstRegInd;


    /* 参数检查 */
    if (VOS_NULL_PTR == pstMsg)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "pMsg is null.");
        return;
    }

    pstRegInd   = (PS_RRM_REGISTER_IND_STRU *)pstMsg;
    enModemId   = pstRegInd->enModemId;
    RRM_DBG_MODEM_RX_REG_NUM(enModemId, 1);
    if (PS_FALSE == RRM_GetModemSupportFlg(enModemId))
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "modem don't support.", enModemId);
        return;
    }

    enRatType   = pstRegInd->enRatType;

    /* 消息合法性检查 */
    ulRslt      = RRM_CheckMsgParam(enModemId, enRatType);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "Msg is invalid.");
        return;
    }

    usRFIDValue = RRM_GetRFIDCfgValue(enModemId, enRatType);
    if (RRM_RFID_DEFAULT_VALUE == usRFIDValue)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, enRatType is <2>.", enModemId, enRatType);
        return;
    }

    ulPid       = pstRegInd->stMsgHeader.ulSenderPid;
    /* REG INFO是否已注册判断 */
    ulRslt  = RRM_JudgeRegInfoIsReged(enModemId, ulPid, &enSameModem, &usRegInfoIndex, &enRegInfoIsReged);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, ulPid is <2>.", enModemId, ulPid);
        return;
    }

    if (PS_TRUE == enRegInfoIsReged)
    {
        if (PS_TRUE == enSameModem)
        {
            /* 是同一个MODEM发过来的注册信息 */
            RRM_WARNING_LOG(UEPS_PID_RRM, "It is the same modem.");
            return;
        }
        else
        {
            /* 是不同MODEM发过来的注册信息 */
            RRM_WARNING_LOG(UEPS_PID_RRM, "It is not the same modem.");
            return;
        }
    }
    else
    {
        /*** REG INFO没有被注册 ***/

        /* 设置REG INFO */
        RRM_SetRegInfo(PS_TRUE, enRatType, enModemId, ulPid);
        return;
    }

}
VOS_VOID RRM_RcvDeregisterInd(VOS_VOID * pstMsg)
{
    VOS_UINT32                          ulPid;
    VOS_UINT32                          ulRslt;
    MODEM_ID_ENUM_UINT16                enModemId;
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType;
    PS_BOOL_ENUM_UINT8                  enSameModem;
    VOS_UINT16                          usRFIDValue;
    VOS_UINT16                          usRegInfoIndex;
    PS_BOOL_ENUM_UINT8                  enRegInfoIsReged;
    PS_RRM_DEREGISTER_IND_STRU         *pstDeregInd;


    /* 参数检查 */
    if (VOS_NULL_PTR == pstMsg)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "pMsg is null.");
        return;
    }

    pstDeregInd   = (PS_RRM_DEREGISTER_IND_STRU *)pstMsg;
    enModemId   = pstDeregInd->enModemId;
    RRM_DBG_MODEM_RX_DEREG_NUM(enModemId, 1);
    if (PS_FALSE == RRM_GetModemSupportFlg(enModemId))
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "modem don't support.", enModemId);
        return;
    }

    enRatType   = pstDeregInd->enRatType;
    /* 消息合法性检查 */
    ulRslt      = RRM_CheckMsgParam(enModemId, enRatType);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "Msg is invalid.enModemId is <1>, enRatType is <2>", enModemId, enRatType);
        return;
    }

    usRFIDValue = RRM_GetRFIDCfgValue(enModemId, enRatType);
    if (RRM_RFID_DEFAULT_VALUE == usRFIDValue)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, enRatType is <2>.", enModemId, enRatType);
        return;
    }

    ulPid       = pstDeregInd->stMsgHeader.ulSenderPid;
    /* REG INFO是否已注册判断 */
    ulRslt  = RRM_JudgeRegInfoIsReged(enModemId, ulPid, &enSameModem, &usRegInfoIndex, &enRegInfoIsReged);
    if (VOS_ERR == ulRslt)
    {
        RRM_WARNING_LOG2(UEPS_PID_RRM, "enModemId is <1>, ulPid is <2>.", enModemId, ulPid);
        return;
    }

    if (PS_TRUE == enRegInfoIsReged)
    {
        if (PS_TRUE == enSameModem)
        {
            /* 是同一个MODEM发过来的注册信息, 清除注册信息 */
            RRM_ClearRegInfo(usRegInfoIndex);
            return;
        }
        else
        {
            /* 是不同MODEM发过来的注册信息 */
            RRM_WARNING_LOG(UEPS_PID_RRM, "It is not the same modem.");
            return;
        }
    }
    else
    {
        /*** REG INFO没有被注册，该消息不处理 ***/
        RRM_WARNING_LOG(UEPS_PID_RRM, "Reg info is not exist.");
        return;
    }

}




VOS_VOID RRM_RcvTimeOutProc(RRM_RFID_ENUM_UINT16  enRFIDIndex)
{
    VOS_UINT16                          usIndex;
    VOS_UINT32                          ulRecievePid;
    RRM_REG_INFO_STRU                  *pstRegInfoAddr      = VOS_NULL_PTR;
    RRM_RFID_TIMER_CTRL_STRU           *pstRFIDTimerAddr    = VOS_NULL_PTR;

     /* 获取该状态定时器控制块地址 */
    pstRFIDTimerAddr       = RRM_GetRFIDTimerAddr(enRFIDIndex);

    /* 无法获取该定时器控制块地址，说明定时器ID非法 */
    if( VOS_NULL_PTR == pstRFIDTimerAddr)
    {
        /* 打印错误信息 */
        RRM_ERROR_LOG1(UEPS_PID_RRM, "TimerId is Error!", (VOS_INT32)enRFIDIndex);
        return;
    }

    /* 防止其他分支停止定时器后，缓存的超时处理的消息又收到的场景 */
    if (RRM_RFID_BUTT == pstRFIDTimerAddr->enRFIDTimerId)
    {
        /* 打印错误信息 */
        RRM_WARNING_LOG1(UEPS_PID_RRM, "Timer is already stop!", (VOS_INT32)enRFIDIndex);
        return;
    }

    /* 清除RF资源信息 */
    RRM_ClearRFIDUsedInfo(enRFIDIndex);

    /* 注册信息处理 */
    for (usIndex = 0; usIndex < RRM_REG_MAX_NUMBER; usIndex++)
    {
        pstRegInfoAddr = RRM_GetRegInfoAddr(usIndex);
        if (VOS_NULL_PTR != pstRegInfoAddr)
        {
            if (PS_TRUE == pstRegInfoAddr->enRegisteredFlg)
            {
                ulRecievePid   = pstRegInfoAddr->ulPid;

                /* 向已注册的协议栈任务发送通知 */
                RRM_SndPSStatusInd(ulRecievePid);
            }
        }
    }

}



VOS_VOID RRM_RcvTimerExpireMsg(REL_TIMER_MSG *pTimerMsg)
{
    if (pTimerMsg->ulName < RRM_RFID_BUTT)
    {
        RRM_RcvTimeOutProc((VOS_UINT16)pTimerMsg->ulName);
    }
    else
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "TimerMsg name is error.", pTimerMsg->ulName);
    }

    return;
}



VOS_UINT32 RRM_HandleMsgTab
(
    VOS_UINT32                          ulMsgCnt,
    VOS_VOID                           *pstMsg,
    const RRM_MSG_PROC_STRU            *pstMsgProcTab
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulMsgName;


    /* 从消息包中获取MSG ID */
    ulMsgName  = ((MSG_HEADER_STRU*)pstMsg)->ulMsgName;

    /* 查表，进行消息分发 */
    for (ulLoop = 0; ulLoop < ulMsgCnt; ulLoop++)
    {
        if (pstMsgProcTab[ulLoop].ulMsgType == ulMsgName)
        {
            pstMsgProcTab[ulLoop].pProcMsgFunc(pstMsg);
            break;
        }
    }

    /* 没有找到匹配的消息 */
    if (ulMsgCnt == ulLoop)
    {
        RRM_WARNING_LOG1(UEPS_PID_RRM, "RRM_SearchMsgProcTab: Mismatch Msg, ulMsgName is error.", (VOS_INT32)ulMsgName);
        return VOS_ERR;
    }


    return VOS_OK;
}
VOS_VOID RRM_RcvPSMsg(MsgBlock *pstMsg)
{
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulResult;


    /* 获取消息个数 */
    ulMsgCnt = sizeof(g_astRrmPsMsgProcTab) / sizeof(RRM_MSG_PROC_STRU);

    /* 查表，进行消息分发*/
    ulResult = RRM_HandleMsgTab(ulMsgCnt, pstMsg, g_astRrmPsMsgProcTab);

    /* 没有找到匹配的消息 */
    if (VOS_ERR == ulResult)
    {
        RRM_ERROR_LOG1(UEPS_PID_RRM, "MTC_RcvPSMsg, ulResult is false, ulMsgCnt is error.", (VOS_INT32)ulMsgCnt);
    }


    return;
}
VOS_VOID  RRM_MsgProc(MsgBlock * pstMsg)
{
    if (VOS_NULL_PTR == pstMsg)
    {
        RRM_ERROR_LOG(UEPS_PID_RRM, "RRM_MsgProc, pMsg is NULL.");
        return;
    }

    /* 定时器超时消息 */
    if (VOS_PID_TIMER == pstMsg->ulSenderPid)
    {
        RRM_RcvTimerExpireMsg((REL_TIMER_MSG *)pstMsg);
    }
    else
    {
        /* 协议栈任务发过来的消息 */
        RRM_RcvPSMsg(pstMsg);
    }

    return;
}





#endif /* FEATURE_ON == FEATURE_DSDS */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

