
/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "om.h"
#include "TafSdcCtx.h"
#include "MmaAppLocal.h"
#include "TafMmaCtx.h"
#include "Taf_Status.h"
#include "TafMmaSndInternalMsg.h"
#include "TafMmaMain.h"
#include "TafMmaSndApp.h"

#include "TafMmaMntn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define     THIS_FILE_ID        PS_FILE_ID_TAF_MMA_FSM_MAIN_C
#define     STA_WARNING_PRINT   PS_PRINT_WARNING

/*****************************************************************************
  2 全局变量
*****************************************************************************/
extern STATUS_CONTEXT_STRU                     g_StatusContext;

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/

/*****************************************************************************
  6 函数实现
*****************************************************************************/


VOS_UINT32 TAF_MMA_RcvAppPhoneModeSetReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PH_OP_MODE_STRU                *pstPhModeSet    = VOS_NULL_PTR;
    MN_APP_REQ_MSG_STRU                *pstRcvMsg       = VOS_NULL_PTR;

    pstRcvMsg           = (MN_APP_REQ_MSG_STRU*)pstMsg;
    pstPhModeSet        = (TAF_PH_OP_MODE_STRU *)pstRcvMsg->aucContent;

    /* 关机时需要判断是否需要终止其他流程,开机时不需要判断 */
    if ((STA_FSM_ENABLE   != g_StatusContext.ulFsmState)
     && (TAF_PH_MODE_FULL != pstPhModeSet->PhMode))
    {
        TAF_MMA_ProcPhoneStop_NotEnableStatus();
    }

    /* 启动phone mode状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_PHONE_MODE);

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_RcvTafPhoneModeSetReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_PHONE_MODE_SET_REQ_STRU    *pstPhoneModeSetReq  = VOS_NULL_PTR;

    pstPhoneModeSetReq  = (TAF_MMA_PHONE_MODE_SET_REQ_STRU *)pstMsg;

    /* 关机时需要判断是否需要终止其他流程,开机时不需要判断 */
    if ((STA_FSM_ENABLE   != g_StatusContext.ulFsmState)
     && (TAF_PH_MODE_FULL != pstPhoneModeSetReq->stPhoneModePara.PhMode))
    {
        TAF_MMA_ProcPhoneStop_NotEnableStatus();
    }

    /* 启动phone mode状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_PHONE_MODE);

    return VOS_TRUE;
}




VOS_UINT32 TAF_MMA_RcvSysCfgSetReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_SYS_CFG_REQ_STRU           *pstRcvMsg = VOS_NULL_PTR;
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;

    VOS_UINT8                           ucCtxIndex;

    pstRcvMsg = (TAF_MMA_SYS_CFG_REQ_STRU *)pstMsg;

    /*ClientId*/
    usClientId      = pstRcvMsg->stCtrl.usClientId;
    ucOpId          = pstRcvMsg->stCtrl.ucOpId;


    /* 如果g_stTafMmaCtx.astMmaOperCtx里TAF_MMA_OPER_SYS_CFG_REQ类型结构正在被使用 */
    if (VOS_TRUE == TAF_MMA_IsOperTypeUsed(TAF_MMA_OPER_SYS_CFG_REQ))
    {
        /* 直接回复SYSCFG设置失败 */
        TAF_MMA_SndSysCfgSetCnf(&(pstRcvMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_FAILURE,
                                TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 找不到未被使用的，回复失败 */
    if (VOS_FALSE == TAF_MMA_GetUnusedOperCtxIndex(&ucCtxIndex))
    {
        /* 直接回复SYSCFG设置失败 */
        TAF_MMA_SndSysCfgSetCnf(&(pstRcvMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_FAILURE,
                                TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 分配一个g_stTafMmaCtx.astMmaOperCtx到对应操作 */
    TAF_MMA_SetOperCtx(pstRcvMsg->stCtrl,
                       TAF_MMA_OPER_SYS_CFG_REQ, ucCtxIndex);


    /* 在关机状态，SYSCFG设置时候不检测CL模式，CL模式只有在开机后生效 */
    if ( (STA_FSM_NULL == g_StatusContext.ulFsmState)
      || (STA_FSM_STOP == g_StatusContext.ulFsmState) )
    {
        /* 继续原有的SYSCFG设置流程 */
        MMA_PhSysCfgSet(usClientId, ucOpId, &(pstRcvMsg->stSysCfgPara));

        return VOS_TRUE;
    }

    /* 如果是开机状态，C+L模式，此时是AT触发了SYSCFG设置，则回复失败,进行保护  */
    if ((VOS_TRUE        == TAF_MMA_IsPowerOnCLInterWork())
     && (CMMCA_CLIENT_ID != usClientId))
    {
        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 如果开机状态，不是C+L模式，此时是CMMCA触发了SYSCFG设置，则回复失败,进行保护  */
    if ((VOS_FALSE       == TAF_MMA_IsPowerOnCLInterWork())
     && (CMMCA_CLIENT_ID == usClientId))
    {
        TAF_MMA_ReportSysCfgSetCnf(TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 继续原有的SYSCFG设置流程 */
    MMA_PhSysCfgSet(usClientId, ucOpId, &(pstRcvMsg->stSysCfgPara));

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmaInterPowerInit_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 启动phone mode状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_PHONE_MODE);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmaInterUsimStatusChangeInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (STA_FSM_ENABLE != g_StatusContext.ulFsmState)
    {
        TAF_MMA_ProcPhoneStop_NotEnableStatus();
    }


    /* 如果当前为CL模式，则等待CDMA触发关机 */
    if (VOS_TRUE == TAF_SDC_IsConfigCLInterWork())
    {
        return VOS_TRUE;
    }


    /* 启动phone mode状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_PHONE_MODE);

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvMmaSimlockStatusChangeInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (STA_FSM_ENABLE != g_StatusContext.ulFsmState)
    {
        TAF_MMA_ProcPhoneStop_NotEnableStatus();
    }

    /* 启动phone mode状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_PHONE_MODE);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmaPhoneModeRsltInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_PHONE_MODE_RSLT_IND_STRU   *pstPhoneModeRslt    = VOS_NULL_PTR;
    TAF_SDC_CTX_STRU                   *pstSdcCtx           = VOS_NULL_PTR;

    pstPhoneModeRslt = (TAF_MMA_PHONE_MODE_RSLT_IND_STRU*)pstMsg;
    pstSdcCtx = TAF_SDC_GetSdcCtx();

    if (TAF_MMA_PHONE_MODE_RESULT_SWITCH_ON_SUCC == pstPhoneModeRslt->enRslt)
    {
        /*进入使能状态，设置状态标志量*/
        g_StatusContext.ulFsmState = STA_FSM_ENABLE;
    }
    else
    {
        /* 初始化全局变量 */
        TAF_MMA_InitCtx(TAF_MMA_INIT_CTX_POWEROFF, TAF_MMA_GetMmaCtxAddr());

        TAF_SDC_InitLcConfigPara(&(pstSdcCtx->stMsCfgInfo.stMsCapInfo.stLcConfigPara));

        g_StatusContext.ulFsmState = STA_FSM_NULL;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvPihUsimStatusInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 发送内部卡状态改变消息 */
    TAF_MMA_SndInterUsimChangeInd();

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvUsimRefreshIndMsg_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 发送内部卡状态改变消息 */
    TAF_MMA_SndInterUsimChangeInd();

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvDetachReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_DETACH_REQ_STRU            *pstDetachReq = VOS_NULL_PTR;
    TAF_PH_DETACH_TYPE                  ucDetachType;
    VOS_UINT8                           ucIndex;

    pstDetachReq = (TAF_MMA_DETACH_REQ_STRU *)pstMsg;

    /* 如果是C+L模式，此时是AT触发了detach，则回复失败,进行保护  */
    if ((VOS_TRUE == TAF_MMA_IsPowerOnCLInterWork())
     && (CMMCA_CLIENT_ID != pstDetachReq->stCtrl.usClientId))
    {
        TAF_MMA_SndDetachCnf(&(pstDetachReq->stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 如果当前不是C+L模式，但是CMMCA触发DETACH,则直接回复失败 */
    if ((VOS_FALSE == TAF_MMA_IsPowerOnCLInterWork())
     && (CMMCA_CLIENT_ID == pstDetachReq->stCtrl.usClientId))
    {
        TAF_MMA_SndDetachCnf(&(pstDetachReq->stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_ERROR);
        return VOS_TRUE;
    }

    /* 如果g_stTafMmaCtx.astMmaOperCtx里TAF_MMA_OPER_ACQ_REQ类型结构正在被使用 */
    if (VOS_TRUE == TAF_MMA_IsOperTypeUsed(TAF_MMA_OPER_DETACH_REQ))
    {
        TAF_MMA_SndDetachCnf(&(pstDetachReq->stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_ERROR);
        return VOS_TRUE;
    }

    /* 找不到未被使用的，回复失败 */
    if (VOS_FALSE == TAF_MMA_GetUnusedOperCtxIndex(&ucIndex))
    {
        TAF_MMA_SndDetachCnf(&(pstDetachReq->stCtrl),
                             TAF_MMA_APP_OPER_RESULT_FAILURE,
                             TAF_ERR_ERROR);
        return VOS_TRUE;
    }

    /* 分配一个g_stTafMmaCtx.astMmaOperCtx到对应操作 */
    TAF_MMA_SetOperCtx(pstDetachReq->stCtrl,
                       TAF_MMA_OPER_DETACH_REQ, ucIndex);

    if (TAF_PH_SERVICE_CS == pstDetachReq->stDetachPara.ucDetachDomain)
    {
        ucDetachType = TAF_PH_CS_OPERATE;
    }
    else if (TAF_PH_SERVICE_PS == pstDetachReq->stDetachPara.ucDetachDomain)
    {
        ucDetachType = TAF_PH_PS_OPERATE;
    }
    else if (TAF_PH_SERVICE_CS_PS == pstDetachReq->stDetachPara.ucDetachDomain)
    {
        ucDetachType = TAF_PH_PS_CS_OPERATE;
    }
    else
    {
        ucDetachType = TAF_PH_CS_OPERATE;
    }

    MMA_PhoneDetach(pstDetachReq->stCtrl.usClientId, pstDetachReq->stCtrl.ucOpId,
                    ucDetachType, pstDetachReq->stDetachPara.enDetachCause);

    return VOS_TRUE;
}


#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 TAF_MMA_RcvTafImsSwitchSetReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 启动IMS SWITCH状态机 */
    TAF_MMA_FSM_InitSubFsm(TAF_MMA_FSM_IMS_SWITCH);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmaImsSwitchRsltInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 暂时没有处理，只是为了后续的缓存处理，以后如果有需要，可以在这里添加处理 */

    return VOS_TRUE;
}
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


