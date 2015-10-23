

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmInclude.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMTIMERMSGPROC_C
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



/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_VOID NAS_ESM_TimerMsgDistr(const VOS_VOID *pRcvMsg )
{
    VOS_UINT32                          ulIndex         = NAS_ESM_NULL;
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType     = TI_NAS_ESM_TIMER_PARA_BUTT;

    NAS_ESM_INFO_LOG("NAS_ESM_TimerMsgDistr is entered.");

    /*从消息中取出状态表ID*/
    ulIndex = PS_GET_REL_TIMER_NAME(pRcvMsg);

    enTimerType = (NAS_ESM_TIMER_PARA_ENUM_UINT8)PS_GET_REL_TIMER_PARA(pRcvMsg);

    /*判断承载ulStateTblIndex合法性*/
    if (PS_FALSE == NAS_ESM_IsTimerNameValid(ulIndex, enTimerType))
    {
        /*打印异常信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_TimerMsgDistr:WARNING:Timer->SM ,Invalid Timer Name or Para !");
        return;
    }

    /*定时器超时处理*/
    switch(enTimerType)
    {
        case TI_NAS_ESM_T3482:
            NAS_ESM_ProcTimerMsgT3482Exp(pRcvMsg);
            break;
        case TI_NAS_ESM_T3492:
            NAS_ESM_ProcTimerMsgT3492Exp(pRcvMsg);
            break;
        case TI_NAS_ESM_T3480:
            NAS_ESM_ProcTimerMsgT3480Exp(pRcvMsg);
            break;
        case TI_NAS_ESM_T3481:
            NAS_ESM_ProcTimerMsgT3481Exp(pRcvMsg);
            break;
        case TI_NAS_ESM_WAIT_APP_CNF:
            NAS_ESM_ProcTimerMsgWaitAppCnfExp(pRcvMsg);
            break;
        case TI_NAS_ESM_WAIT_PTI_INVALID:
            NAS_ESM_ProcTimerMsgWaitPtiInvaidExp(pRcvMsg);
            break;
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
            NAS_ESM_ProcTimerMsgAttachBearerReestExp(pRcvMsg);
            break;
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
            NAS_ESM_ProcTimerMsgDeferRelNonEmcExp(pRcvMsg);
            break;
        default:
            NAS_ESM_WARN_LOG("NAS_ESM_TimerMsgDistr:WARNING:Illegal Timer Type!");
            break;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ProcTimerMsgWaitPtiInvaidExp
 Description     : 等待PTI失效定时器超时处理
 Input           : pMsg
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-2-2  Draft Enact

*****************************************************************************/
/*lint -e961*/
VOS_VOID NAS_ESM_ProcTimerMsgWaitPtiInvaidExp(const VOS_VOID * pMsg)
{
    VOS_UINT32                          ulEsmBuffIndex      = NAS_ESM_NULL;
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstPtiBuffItem      = VOS_NULL_PTR;

    /* 获取消息中包含的ESM缓存记录索引号 */
    ulEsmBuffIndex = PS_GET_REL_TIMER_NAME(pMsg);

    /* 获取缓存记录 */
    pstPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                            NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulEsmBuffIndex);

    if (pstPtiBuffItem == VOS_NULL_PTR)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgWaitPtiInvaidExp:WARN:Get buffer item failed!");
        return ;
    }

    NAS_ESM_INFO_LOG2("NAS_ESM_ProcTimerMsgWaitPtiInvaidExp:NORM:PTI is deleted from Esm Buffer!",
                       pstPtiBuffItem->ucPti, ulEsmBuffIndex);

    /* 清除缓存记录 */
    NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulEsmBuffIndex);
}


VOS_VOID    NAS_ESM_ProcTimerMsgT3482Exp(const VOS_VOID * pMsg)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    NAS_ESM_TIMER_STRU                 *pTimer3482          = VOS_NULL_PTR;
    VOS_UINT32                          ulErrorCode         = APP_ERR_TIME_OUT;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgT3482Exp is entered.");

    /*获取消息中包含的状态表StateTblIndex*/
    ulStateTblIndex = PS_GET_REL_TIMER_NAME(pMsg);
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgT3482Exp: L mode is already suspended!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

    /*根据消息对应的ulStateTblIndex,获取相关联的定时器T3482*/
    pTimer3482 = NAS_ESM_GetStateTblTimer(ulStateTblIndex);

    /*超时次数++*/
    pTimer3482->ucExpireTimes++;

    /*如果当前定时器超时次数小于NAS_ESM_MAX_TIMER_EXPIRE_TIMES次*/
    if(pTimer3482->ucExpireTimes < TI_NAS_ESM_T3482_TIMER_MAX_EXP_NUM)
    {
        /*调用函数:SM向MM发送消息:NAS_ESMCN_MSG_TYPE_PDN_CONNECT_REQ*/
        NAS_ESM_SndEsmEmmDataReqMsg(pstStateAddr->stNwMsgRecord.ulOpId,
                                    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq,
                                    pstStateAddr->stNwMsgRecord.ulMsgLength,
                                    pstStateAddr->stNwMsgRecord.aucMsgBuff);

        /*重新启动定时器*/
        NAS_ESM_TimerStart(ulStateTblIndex,TI_NAS_ESM_T3482);

        /*上报空口消息*/
        NAS_ESM_SndAirMsgReportInd(pstStateAddr->stNwMsgRecord.aucMsgBuff,
                                   pstStateAddr->stNwMsgRecord.ulMsgLength,
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                   ESM_PDN_CONNECT_REQ);
        return;
    }
    else if(ID_APP_ESM_NDISCONN_REQ == NAS_ESM_GetStateTblAppMsgType(ulStateTblIndex) )
    {
        /*调用函数:发送确认消息通知APP，本次承载资源分配过程失败*/
        NAS_ESM_LOG1("NAS_ESM_ProcTimerMsgT3482Exp:ERROR =", ulErrorCode);
        NAS_ESM_SndEsmAppNdisConnCnfFailMsg(ulStateTblIndex, ulErrorCode);
    }
    else
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgT3482Exp:WARNING:ExpireTimes exceed!");

        /*向APP回复消息*/
        NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblIndex, \
                                            APP_ERR_TIME_OUT);
    }

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);
}
/*lint +e961*/

VOS_VOID    NAS_ESM_ProcTimerMsgT3492Exp(const VOS_VOID * pMsg)
{
    VOS_UINT32                          ulStateTblIndex  = NAS_ESM_NULL;
    NAS_ESM_TIMER_STRU                 *pTimer3492       = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgT3492Exp is entered.");

    /*获取消息中包含的状态表StateTblIndex*/
    ulStateTblIndex = PS_GET_REL_TIMER_NAME(pMsg);
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgT3492Exp: L mode is already suspended!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

    /*根据消息对应的ulStateTblIndex,获取相关联的定时器*/
    pTimer3492 = NAS_ESM_GetStateTblTimer(ulStateTblIndex);

    /*超时次数++*/
    pTimer3492->ucExpireTimes++;

    /*如果当前定时器超时次数小于NAS_ESM_MAX_TIMER_EXPIRE_TIMES次*/
    if(pTimer3492->ucExpireTimes < TI_NAS_ESM_T3492_TIMER_MAX_EXP_NUM)
    {
        /*调用函数:SM向MM发送消息:NAS_ESMCN_PDN_DISCONNECT_REQ*/
        NAS_ESM_SndEsmEmmDataReqMsg(pstStateAddr->stNwMsgRecord.ulOpId,
                                    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq,
                                    pstStateAddr->stNwMsgRecord.ulMsgLength,
                                    pstStateAddr->stNwMsgRecord.aucMsgBuff);

        /*重新启动定时器*/
        NAS_ESM_TimerStart(ulStateTblIndex,TI_NAS_ESM_T3492);

        /*上报空口消息*/
        NAS_ESM_SndAirMsgReportInd(pstStateAddr->stNwMsgRecord.aucMsgBuff,
                                   pstStateAddr->stNwMsgRecord.ulMsgLength,
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                   ESM_PDN_DISCONNECT_REQ);
        return;

    }
    else
    {
        NAS_ESM_WARN_LOG("NAS_ESM_DefltAttingBInactPPendMsgT3492Exp:WARNING:ExpireTimes exceed!");

        /*本地释放缺省承载资源,向APP回复释放成功消息*/
        NAS_ESM_DeactBearerAndInformApp(pstStateAddr->ulEpsbId);

        /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
        NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_LOCAL);
    }

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);
}


VOS_VOID   NAS_ESM_ProcTimerMsgT3480Exp
(
    const VOS_VOID * pMsg
)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    VOS_UINT32                          ulErrorCode         = APP_ERR_TIME_OUT;
    NAS_ESM_TIMER_STRU                 *pTimer3480          = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgT3480Exp is entered.");

    /*获取消息中包含的状态表StateTblIndex*/
    ulStateTblIndex = PS_GET_REL_TIMER_NAME(pMsg);
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgT3480Exp: L mode is already suspended!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

    /*根据消息对应的ulStateTblIndex,获取相关联的定时器*/
    pTimer3480 = NAS_ESM_GetStateTblTimer(ulStateTblIndex);

    /*超时次数++*/
    pTimer3480->ucExpireTimes++;

    /*如果当前定时器超时次数小于NAS_ESM_MAX_TIMER_EXPIRE_TIMES次*/
    if(pTimer3480->ucExpireTimes < TI_NAS_ESM_T3480_TIMER_MAX_EXP_NUM)
    {
        /*调用函数:SM向MM发送消息:NAS_ESMCN_BEARER_RES_MOD_REQ*/
        NAS_ESM_SndEsmEmmDataReqMsg(pstStateAddr->stNwMsgRecord.ulOpId,
                                    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq,
                                    pstStateAddr->stNwMsgRecord.ulMsgLength,
                                    pstStateAddr->stNwMsgRecord.aucMsgBuff);

        /*重新启动定时器*/
        NAS_ESM_TimerStart(ulStateTblIndex,TI_NAS_ESM_T3480);

        /*上报空口消息*/
        NAS_ESM_SndAirMsgReportInd(pstStateAddr->stNwMsgRecord.aucMsgBuff,
                                   pstStateAddr->stNwMsgRecord.ulMsgLength,
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                   ESM_BEARER_RESOUCE_ALLOC_REQ);

        return;

    }

    if(ID_APP_ESM_PDP_SETUP_REQ == NAS_ESM_GetStateTblAppMsgType(ulStateTblIndex) )
    {
        /*调用函数:发送确认消息通知APP，本次承载资源分配过程失败*/
        NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblIndex, ulErrorCode);
    }

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);
}
VOS_VOID    NAS_ESM_ProcTimerMsgT3481Exp(const VOS_VOID * pMsg)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    VOS_UINT32                          ulErrorCode         = APP_ERR_TIME_OUT;
    VOS_UINT32                          ulActCidNum         = NAS_ESM_NULL;
    NAS_ESM_TIMER_STRU                 *pTimer3481          = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr        = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgT3481Exp is entered.");

    /*获取消息中包含的状态表StateTblIndex*/
    ulStateTblIndex = PS_GET_REL_TIMER_NAME(pMsg);
    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgT3481Exp: L mode is already suspended!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

    /*根据消息对应的ulStateTblIndex,获取相关联的定时器*/
    pTimer3481 = NAS_ESM_GetStateTblTimer(ulStateTblIndex);

    /*超时次数++*/
    pTimer3481->ucExpireTimes++;

    /*如果当前定时器超时次数小于NAS_ESM_MAX_TIMER_EXPIRE_TIMES次*/
    if(pTimer3481->ucExpireTimes < TI_NAS_ESM_T3481_TIMER_MAX_EXP_NUM)
    {
        /*调用函数:SM向MM发送消息:NAS_ESMCN_BEARER_RES_MOD_REQ*/
        NAS_ESM_SndEsmEmmDataReqMsg(pstStateAddr->stNwMsgRecord.ulOpId,
                                    pstStateAddr->stNwMsgRecord.ucIsEmcPdnReq,
                                    pstStateAddr->stNwMsgRecord.ulMsgLength,
                                    pstStateAddr->stNwMsgRecord.aucMsgBuff);

        /*重新启动定时器*/
        NAS_ESM_TimerStart(ulStateTblIndex,TI_NAS_ESM_T3481);

        /*上报空口消息*/
        NAS_ESM_SndAirMsgReportInd(pstStateAddr->stNwMsgRecord.aucMsgBuff,
                                   pstStateAddr->stNwMsgRecord.ulMsgLength,
                                   NAS_ESM_AIR_MSG_DIR_ENUM_UP,
                                   ESM_BEARER_RESOUCE_MOD_REQ);
        return;

    }

    if(ID_APP_ESM_PDP_SETUP_REQ == NAS_ESM_GetStateTblAppMsgType(ulStateTblIndex) )
    {
        /*调用函数:发送确认消息通知APP，本次承载资源分配过程失败*/
        NAS_ESM_SndEsmAppSdfSetupCnfFailMsg(ulStateTblIndex, ulErrorCode);
    }
    else  if(ID_APP_ESM_PDP_RELEASE_REQ == NAS_ESM_GetStateTblAppMsgType(ulStateTblIndex) )
    {
        ulActCidNum = NAS_ESM_GetBearerLinkedCidNum(pstStateAddr->ulEpsbId);

        /* 如果只对应一个激活的Cid,说明App要释放所有的SDF */
        if (ulActCidNum == NAS_ESM_BEARER_ACT_CID_NUM_ONE)
        {
            /* 本地释放此承载 */
            NAS_ESM_DeactBearerAndInformApp(pstStateAddr->ulEpsbId);

            /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
            NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_LOCAL);
        }
        else
        {
            NAS_ESM_SndEsmAppSdfModCnfFailMsg(ulStateTblIndex, ulErrorCode);
        }
    }
    else
    {
        NAS_ESM_SndEsmAppSdfModCnfFailMsg(ulStateTblIndex, ulErrorCode);
    }

    /*释放状态表资源*/
    NAS_ESM_RelStateTblResource(ulStateTblIndex);
}

/*****************************************************************************
 Function Name   : NAS_ESM_ProcTimerMsgWaitAppCnfExp
 Description     : 等待APP回复定时器超时处理函数
 Input           : VOS_VOID *pMsg
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-4  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_ProcTimerMsgWaitAppCnfExp(const VOS_VOID * pMsg)
{
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;

    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgWaitAppCnfExp is entered.");

    /*获取消息中包含的状态表StateTblIndex*/
    ulStateTblIndex = PS_GET_REL_TIMER_NAME(pMsg);

    if (NAS_ESM_L_MODE_STATUS_NORMAL != NAS_ESM_GetLModeStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ProcTimerMsgWaitAppCnfExp: L mode is already suspended!");

        /*释放状态表资源*/
        NAS_ESM_RelStateTblResource(ulStateTblIndex);

        return ;
    }

    /* App拒绝承载激活或承载修改的处理 */
    NAS_ESM_AppPdpManageRspMsgRefuseProc(ulStateTblIndex);
}


VOS_VOID NAS_ESM_ProcTimerMsgAttachBearerReestExp(const VOS_VOID * pMsg)
{
    /*打印进入该函数*/
    NAS_ESM_NORM_LOG("NAS_ESM_ProcTimerMsgAttachBearerReestExp is entered.");

    (VOS_VOID)pMsg;

    /*如果支持双APN，则检查注册承载，并进行处理*/
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        if (NAS_ESM_UNASSIGNED_EPSB_ID == NAS_ESM_GetAttachBearerId())
        {
            NAS_ESM_ReestablishAttachBearer();
        }
    }
}


VOS_VOID NAS_ESM_ProcTimerMsgDeferRelNonEmcExp(const VOS_VOID * pMsg)
{
    NAS_ESM_INFO_LOG("NAS_ESM_ProcTimerMsgDeferRelNonEmcExp: enter");

    (VOS_VOID)pMsg;

    if (PS_TRUE == NAS_ESM_HasEstingEmergencyPdn())
    {
        NAS_ESM_INFO_LOG("NAS_ESM_ProcTimerMsgDeferRelNonEmcExp: esting emergency PDN");

        /*如果紧急承载还未建立成功，重新启动延迟定时器*/
        NAS_ESM_TimerStart(0, TI_NAS_ESM_REL_NON_EMC_BEARER);
    }
    else if (PS_TRUE == NAS_ESM_HasExistedEmergencyPdn())
    {
        NAS_ESM_INFO_LOG("NAS_ESM_ProcTimerMsgDeferRelNonEmcExp: existed emergency PDN");

        /*如果紧急承载建立成功，则释放所有非紧急承载*/
        NAS_ESM_DeactAllNonEmcPdn();
    }
    else
    {
        /*如果紧急承载建立失败，不做处理*/
        NAS_ESM_INFO_LOG("NAS_ESM_ProcTimerMsgDeferRelNonEmcExp: no emergency PDN");
    }

    return;
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
