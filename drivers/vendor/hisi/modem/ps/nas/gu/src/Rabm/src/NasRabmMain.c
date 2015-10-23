

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "pslog.h"
#include "PsCommonDef.h"
#include "rabminclude.h"
#include "NasRabmMain.h"
#include "NasRabmMsgProc.h"
#include "AtRabmInterface.h"

#include "NasUtranCtrlCommFunc.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define    THIS_FILE_ID        PS_FILE_ID_NAS_RABM_MGR_C



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 当前系统模式 */
GMM_RABM_NET_RAT_ENUM_UINT32            g_enNasRabmSysMode;

/* 基于系统模式的函数处理指针表 */
NAS_RABM_PROC_FUNC_TBL_STRU            *g_pstNasRabmProcFuncTbl[NAS_MML_NET_RAT_TYPE_BUTT];

NAS_RABM_FASTDORM_CTX_STRU              g_stNasRabmFastDormCtx;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID NAS_RABM_RegProcFuncTbl(
    GMM_RABM_NET_RAT_ENUM_UINT32         enSysMode,
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFucTbl
)
{
    /* 系统模式有效性检查 */
    if (NAS_MML_NET_RAT_TYPE_BUTT == enSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RegProcFuncTbl: Wrong system mode.");
        return;
    }

    NAS_RABM_SetProFuncTblAddr(enSysMode, pstProcFucTbl);

}
VOS_VOID NAS_RABM_CtxInit(VOS_VOID)
{
    VOS_UINT32                          i;

#if (FEATURE_ON == FEATURE_LTE)
    /* 初始化系统模式: 默认为LTE */
    NAS_RABM_SetSysMode(NAS_MML_NET_RAT_TYPE_LTE);
#else
    /* 初始化系统模式: 默认为WCDMA */
    NAS_RABM_SetSysMode(NAS_MML_NET_RAT_TYPE_WCDMA);
#endif

    /* 初始化函数处理指针表为NULL */
    for ( i = 0; i < NAS_MML_NET_RAT_TYPE_BUTT; i++)
    {
        NAS_RABM_SetProFuncTblAddr(i, VOS_NULL_PTR);
    }

    /* 初始化RABM实体 */
    Rabm_Init();
    NAS_Rabm2GInit();

    /* 初始化挂起标志 */
    NAS_RABM_ClearDataSuspendFlg();

    /* 初始化处理函数表 */
    NAS_RABM_InitProcFuncTbl();

}


VOS_VOID NAS_RABM_MsgProc(
    struct MsgCB                       *pMsg
)
{
    MSG_HEADER_STRU                    *pstMsg;
    struct MsgCB                       *pstDestMsg;


    pstDestMsg = VOS_NULL_PTR;

    /* 入口消息非空检查 */
    if (VOS_NULL_PTR == pMsg)
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM,
            "NAS_RABM_MsgProc: Message is NULL.");
        return;
    }

    /* 支持TD-SCDMA特性时，需对消息进行适配处理，将TD模的发送PID适配为W模的PID */
    if ( VOS_TRUE == NAS_UTRANCTRL_MsgProc(pMsg, &pstDestMsg) )
    {
        /* 消息在UTRANCTRL模块处理完成，直接返回，不进入后续处理 */
        return;
    }

    /* 替换消息指针 */
    pstMsg = (MSG_HEADER_STRU*)pstDestMsg;

    /* 处理消息 */
    switch (pstMsg->ulSenderPid)
    {
        case WUEPS_PID_SM:
            NAS_RABM_RcvSmMsg(pstDestMsg);
            break;

        case WUEPS_PID_GMM:
            NAS_RABM_RcvGmmMsg(pstDestMsg);
            break;

        case WUEPS_PID_AT:
            NAS_RABM_RcvAtMsg(pstDestMsg);
            break;

        case UEPS_PID_CDS:
            NAS_RABM_RcvCdsMsg(pstDestMsg);
            break;

        default:
            NAS_RABM_RcvCommMsg(pstDestMsg);
            break;
    }
}


VOS_VOID NAS_RABM_RcvSmMsg(
    struct MsgCB                       *pMsg
)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch (pstMsg->ulMsgName)
    {
        case RABMSM_ACTIVATE_IND:
            NAS_RABM_RcvSmActivateInd((RABMSM_ACTIVATE_IND_STRU*)pMsg);
            break;

        case RABMSM_DEACTIVATE_IND:
            NAS_RABM_RcvSmDeactivateInd((RABMSM_DEACTIVATE_IND_STRU*)pMsg);
            break;

        case RABMSM_MODIFY_IND:
            NAS_RABM_RcvSmModifyInd((RABMSM_MODIFY_IND_STRU*)pMsg);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case ID_SM_RABM_BEARER_ACTIVATE_IND:
            NAS_RABM_RcvSmBearerActivateInd((SM_RABM_BEARER_ACTIVATE_IND_STRU*)pMsg);
            break;

        case ID_SM_RABM_BEARER_DEACTIVATE_IND:
            NAS_RABM_RcvSmBearerDeactivateInd((SM_RABM_BEARER_DEACTIVATE_IND_STRU*)pMsg);
            break;

        case ID_SM_RABM_BEARER_MODIFY_IND:
            NAS_RABM_RcvSmBearerModifyInd((SM_RABM_BEARER_MODIFY_IND_STRU*)pMsg);
            break;
#endif

        default:
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvSmMsg: Unknown message from SM.");
            break;
    }
}


VOS_VOID NAS_RABM_RcvGmmMsg(
    struct MsgCB                       *pMsg
)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch (pstMsg->ulMsgName)
    {
        case ID_GMM_RABM_REESTABLISH_CNF:
            NAS_RABM_RcvGmmReestablishCnf((GMMRABM_REESTABLISH_CNF_STRU*)pMsg);
            break;

        case ID_GMM_RABM_SYS_SRV_CHG_IND:
            NAS_RABM_RcvGmmSysSrvChgInd((GMM_RABM_SYS_SRV_CHG_IND_STRU*)pMsg);
            break;

        case ID_GMM_RABM_MML_PROC_STATUS_QRY_CNF:
            NAS_RABM_RcvGmmMmlProcStatusQryCnf(pMsg);
            break;

        default:
            NAS_RABM_RcvCommMsg(pMsg);
            break;
    }
}


VOS_VOID NAS_RABM_RcvSmActivateInd(
    RABMSM_ACTIVATE_IND_STRU           *pstSmActivateInd
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmActivateInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pSmActivateIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pSmActivateIndProcFunc(pstSmActivateInd);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvSmActivateInd: pSmActivateIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmActivateInd: Msg is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvSmDeactivateInd(
    RABMSM_DEACTIVATE_IND_STRU         *pstSmDeactivateInd
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmDeactivateInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pSmDeactivateIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pSmDeactivateIndProcFunc(pstSmDeactivateInd);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvSmDeactivateInd: pSmDeactivateIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmDeactivateInd: Msg is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvSmModifyInd(
    RABMSM_MODIFY_IND_STRU             *pstSmModifyInd
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmModifyInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pSmModifyIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pSmModifyIndProcFunc(pstSmModifyInd);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvSmModifyInd: pSmModifyIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmModifyInd: Msg is received in wrong system mode.");
    }
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_VOID NAS_RABM_RcvSmBearerActivateInd(
    SM_RABM_BEARER_ACTIVATE_IND_STRU   *pstSmBearerActivateInd
)
{
    RABM_ENTITY_PS_STRU                *pstPsEnt;
    VOS_UINT8                           ucEntId;

    /* 获取PS域RABM实体指针 */
    ucEntId  = (VOS_UINT8)(pstSmBearerActivateInd->ulNsapi - RABM_NSAPI_OFFSET);
    pstPsEnt = NAS_RABM_GetWPsEntAddr(ucEntId);

    /* 更新QOS */
    pstPsEnt->QoS.ulQosLength   = pstSmBearerActivateInd->stQos.ulLength;
    PS_MEM_CPY(pstPsEnt->QoS.aucQosValue,
               pstSmBearerActivateInd->stQos.aucQosValue,
               NAS_RABM_MAX_QOS_LEN);

    /* 更新PPP标识 */
    pstPsEnt->ucPppFlg          = RABM_SM_IP_PROT;

    /* 创建RAB_MAP实体 */
    NAS_RABM_CreateRabMapEntity((VOS_UINT8)pstSmBearerActivateInd->ulNsapi,
                                (VOS_UINT8)pstSmBearerActivateInd->ulLinkdNsapi,
                                (VOS_UINT8)pstSmBearerActivateInd->ulNsapi);

    /* 更新RABM状态至挂起状态 */
    RABM_SetWState(ucEntId, RABM_DATA_TRANSFER_STOP);

    return;
}
VOS_VOID NAS_RABM_RcvSmBearerModifyInd(
    SM_RABM_BEARER_MODIFY_IND_STRU     *pstSmBearerModifyInd
)
{
    RABM_ENTITY_PS_STRU                *pstPsEnt;
    VOS_UINT8                           ucEntId;

    /* 获取PS域RABM实体索引 */
    ucEntId = (VOS_UINT8)(pstSmBearerModifyInd->ulNsapi - RABM_NSAPI_OFFSET);

    /* 检查PS域RABM实体状态是否正确 */
    if (RABM_NULL == NAS_RABM_GetWPsEntState(ucEntId))
    {
        NAS_ERROR_LOG1(WUEPS_PID_RABM,
            "NAS_RABM_RcvSmBearerModifyInd: Wrong state of RABM entity. NSAPI:",
            pstSmBearerModifyInd->ulNsapi);
        return;
    }

    /* 获取PS域RABM实体指针 */
    pstPsEnt = NAS_RABM_GetWPsEntAddr(ucEntId);

    /* 更新QOS */
    pstPsEnt->QoS.ulQosLength = pstSmBearerModifyInd->stQos.ulLength;
    PS_MEM_CPY(pstPsEnt->QoS.aucQosValue,
               pstSmBearerModifyInd->stQos.aucQosValue,
               NAS_RABM_MAX_QOS_LEN);

    return;
}


VOS_VOID NAS_RABM_RcvSmBearerDeactivateInd(
    SM_RABM_BEARER_DEACTIVATE_IND_STRU *pstSmBearerDeactivateInd
)
{
    VOS_UINT8                           ucEntId;

    /* 获取PS域RABM实体指针 */
    ucEntId = (VOS_UINT8)(pstSmBearerDeactivateInd->ulNsapi - RABM_NSAPI_OFFSET);

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_RABM_GetCurrentSysMode())
    {
        /* G实体状态置为RABM_2G_NULL */
        RABM_SetGState((RABM_NSAPI_ENUM)ucEntId, RABM_2G_NULL);

        /* 清除G实体信息 */
        NAS_RABM_ClearRabmGPsEnt(ucEntId);
    }
    else
    {
        /* W实体状态置为RABM_NULL */
        RABM_SetWState(ucEntId, RABM_NULL);

        /* 清除G实体信息 */
        NAS_RABM_ClearRabmWPsEnt(ucEntId);
    }

    /* 删除RAB_MAP实体 */
    RABM_DelRabMap((VOS_UINT8)pstSmBearerDeactivateInd->ulNsapi);
}
#endif
VOS_VOID NAS_RABM_RcvGmmReestablishCnf(
    GMMRABM_REESTABLISH_CNF_STRU       *pstGmmReestablishCnf
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvGmmReestablishCnf: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pGmmReestablishCnfProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pGmmReestablishCnfProcFunc(pstGmmReestablishCnf);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvGmmReestablishCnf: pGmmReestablishCnfProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvGmmReestablishCnf: Message is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvGmmSysSrvChgInd(
    GMM_RABM_SYS_SRV_CHG_IND_STRU      *pstGmmSysSrvChgInd
)
{
    GMM_RABM_NET_RAT_ENUM_UINT32        enOldSysMode;

    /* 判断当前的系统模式是否有效 */
    enOldSysMode = NAS_RABM_GetCurrentSysMode();

    /* 根据挂起指示标志是否设置, 执行挂起或恢复处理 */
    if (VOS_TRUE == pstGmmSysSrvChgInd->bDataSuspendFlg)
    {
        NAS_RABM_RcvDataSuspendInd(pstGmmSysSrvChgInd->bRatChangeFlg);
        NAS_RABM_RcvSysModeChgInd(pstGmmSysSrvChgInd->enSysMode);
    }
    else
    {
        NAS_RABM_RcvSysModeChgInd(pstGmmSysSrvChgInd->enSysMode);

        /* GMM通知RABM模块进行恢复时需要通知RABM模块是否需要进行RAB的恢复，
           RABM模块进行恢复时，判断是否需要等待接入层进行RAB恢复 */
        NAS_RABM_RcvDataResumeInd(pstGmmSysSrvChgInd->ucRebuildRabFlag);
    }

    NAS_RABM_SysModeChgProcFastDorm(enOldSysMode,pstGmmSysSrvChgInd->enSysMode);
}


VOS_VOID NAS_RABM_RcvDataSuspendInd(
    VOS_BOOL                            bRatChangeFlg
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvDataSuspendInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pDataSuspendIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pDataSuspendIndProcFunc(bRatChangeFlg);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvDataSuspendInd: pDataSuspendIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvDataSuspendInd: Message is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvDataResumeInd(
    VOS_UINT8                           ucRebuildRabFlag
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvDataResumeInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pDataResumeIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pDataResumeIndProcFunc(ucRebuildRabFlag);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvDataResumeInd: pDataResumeIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvDataResumeInd: Message is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvSysModeChgInd(
    GMM_RABM_NET_RAT_ENUM_UINT32        enNewSysMode
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enOldSysMode;

    /* 判断当前的系统模式是否有效 */
    enOldSysMode = NAS_RABM_GetCurrentSysMode();

    /* 检测系统模式有效性, 若无效, 不更新系统模式 */
    if (NAS_MML_NET_RAT_TYPE_BUTT == enNewSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSysModeChgInd: Invalid System Mode.");
        return;
    }

    /* 更新系统模式 */
    NAS_RABM_SetSysMode(enNewSysMode);

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enOldSysMode);

    /* 确认函数处理表已经注册 */
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pSysModeChgIndProcFunc)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pSysModeChgIndProcFunc(enOldSysMode, enNewSysMode);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_RcvSysModeChgInd: pDataResumeIndProcFunc is not registered.");
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM,
            "NAS_RABM_RcvSysModeChgInd: Message is received in wrong system mode.");
    }
}


VOS_VOID NAS_RABM_RcvCommMsg(
    struct MsgCB                       *pMsg
)
{
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFuncTbl;
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if (NAS_MML_NET_RAT_TYPE_BUTT == enCurrentSysMode)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_ProcSysSrvChgInd: Invalid System Mode.");
        return;
    }

    /* 根据当前系统模式获取对应的处理函数表的指针 */
    pstProcFuncTbl = NAS_RABM_GetProFuncTblAddr(enCurrentSysMode);
    if (VOS_NULL_PTR != pstProcFuncTbl)
    {
        if (VOS_NULL_PTR != pstProcFuncTbl->pTaskEntry)
        {
            /* 调用对应模式的处理函数 */
            pstProcFuncTbl->pTaskEntry(pMsg);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                "NAS_RABM_ProcessCommMsg: TaskEntry is not registered.");
        }
    }
    else
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM,
            "NAS_RABM_ProcessCommMsg: Msg is received in wrong system mode.");
    }
}
VOS_UINT32    NAS_RABM_IsPdpActiveInWCDMA(VOS_VOID)
{
    VOS_UINT32                          ulEntId;
    VOS_UINT8                           ucState;

    for (ulEntId = 0; ulEntId < RABM_3G_PS_MAX_ENT_NUM; ulEntId++)
    {
        ucState = NAS_RABM_GetWPsEntState(ulEntId);
        if ( (RABM_NSAPI_ACTIVE_NO_RAB == ucState)
          || (RABM_NSAPI_ACTIVE_WITH_RAB == ucState)
          || (RABM_DATA_TRANSFER_STOP == ucState) )
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_VOID NAS_RABM_ConvertAtFasTDormTypeToWrrFasTDormType(
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32                enAtFastDormType,
    RRC_FASTDORM_START_TYPE_ENUM_UINT32                 *penWrrFastDormType
)
{
    switch(enAtFastDormType)
    {
        case AT_RABM_FASTDORM_START_FD_ONLY:
            *penWrrFastDormType = RRC_FASTDORM_START_TYPE_FD;
            break;

        case AT_RABM_FASTDORM_START_ASCR_ONLY:
            *penWrrFastDormType = RRC_FASTDORM_START_TYPE_ASCR;
            break;

        case AT_RABM_FASTDORM_START_FD_ASCR:
            *penWrrFastDormType = RRC_FASTDORM_START_TYPE_FD_ASCR;
            break;

        default:
            NAS_WARNING_LOG(WUEPS_PID_RABM,
                        "NAS_RABM_ConvertAtFasTDormTypeToWrrFasTDormType: Wrong Type.");

            break;

    }
}



VOS_VOID NAS_RABM_RcvSetFastDormParaReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
)
{
    NAS_RABM_NVIM_FASTDORM_ENABLE_FLG_STRU                  stEnableFlg;
    VOS_UINT32                                              ulLength;

    ulLength                 = 0;
    stEnableFlg.ucEnableFlag = VOS_FALSE;

    /* 读NV失败，认为不允许Fastdormancy */
    NV_GetLength(en_NV_Item_Fastdorm_Enable_Flag, &ulLength);

    if (NV_OK != NV_Read(en_NV_Item_Fastdorm_Enable_Flag, &stEnableFlg,
                          ulLength))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_RcvSetFastDormParaReq:WARNING:NV_Read faild!");
    }

    /* 不允许Fastdormancy */
    if (VOS_FALSE == stEnableFlg.ucEnableFlag)
    {
        /* 回复给AT ID_RABM_AT_FASTDORM_START_CNF */
        NAS_RABM_SndAtSetFastDormParaCnf(pstFastDormPara->usClientId,
                                         pstFastDormPara->ucOpId,
                                         AT_RABM_PARA_SET_RSLT_FAIL);

        return;
    }

    if (AT_RABM_FASTDORM_STOP_FD_ASCR == pstFastDormPara->stFastDormPara.enFastDormOperationType)
    {
        /* 调用函数，停止FAST DORMANCY */
        NAS_RABM_RcvAtFastDormStopReq(pstFastDormPara);
    }
    else
    {
        /* 调用函数 启动或者设置FAST DORMANCY */
        NAS_RABM_RcvAtFastDormStartReq(pstFastDormPara);
    }
}
VOS_VOID NAS_RABM_RcvGetFastDormParaReq(
    AT_RABM_QRY_FASTDORM_PARA_REQ_STRU *pstFastDormPara
)
{
    NAS_RABM_NVIM_FASTDORM_ENABLE_FLG_STRU                  stEnableFlag;
    VOS_UINT32                                              ulLength;

    ulLength                  = 0;
    stEnableFlag.ucEnableFlag = VOS_FALSE;

    /* 读NV失败，认为不允许Fastdormancy */
    NV_GetLength(en_NV_Item_Fastdorm_Enable_Flag, &ulLength);

    if (NV_OK != NV_Read(en_NV_Item_Fastdorm_Enable_Flag, &stEnableFlag,
                          ulLength))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_RcvGetFastDormParaReq:WARNING:NV_Read faild!");
    }

    /* 回复查询结果 */
    NAS_RABM_SndAtQryFastDormParaCnf(pstFastDormPara->usClientId,
                                     pstFastDormPara->ucOpId,
                                     stEnableFlag.ucEnableFlag);

}



VOS_VOID NAS_RABM_SuspendFastDorm(VOS_VOID)
{
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 如果当前在NAS_RABM_FASTDORM_RUNNING状态，并且在W网络下，需要向WRR发送停止
       操作请求，停止FAST DORMANCY特性 */
    /* 判断当前的系统模式是否有效 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();
    if ((NAS_RABM_FASTDORM_RUNNING == NAS_RABM_GetCurrFastDormStatus())
     && (NAS_MML_NET_RAT_TYPE_WCDMA == enCurrentSysMode))
    {
        /* 向WAS发送RRRABM_FASTDORM_STOP_REQ */
        NAS_RABM_SndWasFastDormStopReq();
    }

    /* 当前不是NAS_RABM_FASTDORM_STOP状态，则需要挂起当前的运行，进入
       SUSPEND状态 */
    if (NAS_RABM_FASTDORM_STOP != NAS_RABM_GetCurrFastDormStatus())
    {
        /* 无流量时长计数清0 */
        NAS_RABM_SetFastDormCurrNoFluxCntValue(0);

        /* 停止定时器RABM_TIMER_FASTDORM_FLUX_DETECT */
        /* 停止定时器RABM_TIMER_FASTDORM_RETRY */
        /* 停止定时器RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT*/

        NAS_RabmStopTimer(RABM_TIMER_NAME_FD_FLUX_DETECT,RABM_TIMER_FASTDORM_FLUX_DETECT);

        NAS_RabmStopTimer(RABM_TIMER_NAME_FD_RETRY,RABM_TIMER_FASTDORM_RETRY);
        NAS_RabmStopTimer(RABM_TIMER_NAME_COMMON,RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT);

        /* 设置为NAS_RABM_FASTDORM_SUSPEND状态 */
        NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_SUSPEND);
        NAS_RABM_SndOmFastdormStatus();

    }
}
VOS_VOID NAS_RABM_ResumeFastDorm(VOS_VOID)
{
    /* 如果异系统后，发现当前PDP是激活的，则需要开始流量检测，准备进入
       FAST DORMANCY，否则仍然在SUSPEND状态 */
    if (VOS_TRUE == NAS_RABM_IsPdpActiveInWCDMA())
    {
        /* 如果当前在NAS_RABM_FASTDORM_SUSPEND状态，说明之前启动了FAST DORMANCY */
        if (NAS_RABM_FASTDORM_SUSPEND == NAS_RABM_GetCurrFastDormStatus())
        {
            /* 设置为NAS_RABM_FASTDORM_INIT_DETECT状态 */
            NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_INIT_DETECT);
            NAS_RABM_SndOmFastdormStatus();


            /* FD数据计数器清零 */
            NAS_RABM_ClrFastDormUlDataCnt();
            NAS_RABM_ClrFastDormDlDataCnt();


            /* 启动周期性流量检测定时器 */
            NAS_RabmStartTimer( RABM_TIMER_NAME_FD_FLUX_DETECT, RABM_TIMER_FASTDORM_FLUX_DETECT );

        }
    }
}




VOS_VOID NAS_RABM_SysModeChgProcFastDorm(
    GMM_RABM_NET_RAT_ENUM_UINT32        enOldSysMode,
    GMM_RABM_NET_RAT_ENUM_UINT32        enNewSysMode
)
{

    /* 系统变化，判定如果当前从W模到其他模，挂起FAST DORMANCY */
    if ( (NAS_MML_NET_RAT_TYPE_WCDMA == enOldSysMode)
      && (NAS_MML_NET_RAT_TYPE_WCDMA != enNewSysMode)
      && (NAS_MML_NET_RAT_TYPE_BUTT != enNewSysMode))
    {
        NAS_RABM_SuspendFastDorm();
    }

    /* 系统变化，判定如果当前从其他模到W模，恢复FAST DORMANCY */
    if ( (NAS_MML_NET_RAT_TYPE_WCDMA != enOldSysMode)
      && (NAS_MML_NET_RAT_TYPE_WCDMA == enNewSysMode) )
    {
        NAS_RABM_ResumeFastDorm();
    }

}


VOS_VOID NAS_RABM_RcvAtFastDormStartReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
)
{
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 获取当前所处网络 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();

    /* 缓存参数 */
    NAS_RABM_SetFastDormOperationType(pstFastDormPara->stFastDormPara.enFastDormOperationType);
    NAS_RABM_SetFastDormUserDefNoFluxCntValue(pstFastDormPara->stFastDormPara.ulTimeLen);

    /* 由于FAST DORMANCY为W下特性，如果不在W网络下，则一改设定为SUSPEND状态 */
    if (NAS_MML_NET_RAT_TYPE_WCDMA != enCurrentSysMode)
    {
        /* 设置为NAS_RABM_FASTDORM_SUSPEND状态 */
        NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_SUSPEND);
    }
    else
    {
        /* 当前在PDP非激活状态，一律设置为在NAS_RABM_FASTDORM_SUSPEND状态,如果
           已经在PDP激活状态，则根据不同状态，做不同的处理 */
        if (VOS_TRUE == NAS_RABM_IsPdpActiveInWCDMA())
        {
            /* 当前是NAS_RABM_FASTDORM_INIT_DETECT，说明之前已经启动了FAST DORMANCY */
            if (NAS_RABM_FASTDORM_INIT_DETECT == NAS_RABM_GetCurrFastDormStatus())
            {
                /* 如果无流量时长已经超过新的设定时长门限值，此时认为可以
                   可以发起FAST DORMANCY */
                if (NAS_RABM_GetFastDormUserDefNoFluxCntValue()
                        < NAS_RABM_GetFastDormCurrNoFluxCntValue())
                {
                    /* 向GMM发送查询消息 */
                    NAS_RABM_SndGmmMmlProcStatusQryReq(RABM_FASTDORM_ENUM);

                    /* 无流量时长计数清0 */
                    NAS_RABM_SetFastDormCurrNoFluxCntValue(0);

                    /* 设置为NAS_RABM_FASTDORM_WAIT_GMM_QRY_RESULT状态 */
                    NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_WAIT_GMM_QRY_RESULT);

                    /* 启动等待查询结果的保护定时器 */
                    NAS_RabmStartTimer( RABM_TIMER_NAME_COMMON, RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT );
                }
            }


            /* if (当前是NAS_RABM_FASTDORM_STOP状态) */
            if (NAS_RABM_FASTDORM_STOP == NAS_RABM_GetCurrFastDormStatus())
            {
                /* 设置为NAS_RABM_FASTDORM_INIT_DETECT状态 */
                NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_INIT_DETECT);


                /* FD数据计数器清零 */
                NAS_RABM_ClrFastDormUlDataCnt();
                NAS_RABM_ClrFastDormDlDataCnt();


                /* 启动周期性流量检测定时器 */
                NAS_RabmStartTimer( RABM_TIMER_NAME_FD_FLUX_DETECT, RABM_TIMER_FASTDORM_FLUX_DETECT );
            }

            /* if (当前是NAS_RABM_FASTDORM_RUNNING状态) */
            if (NAS_RABM_FASTDORM_RUNNING == NAS_RABM_GetCurrFastDormStatus())
            {
                /* 直接发送消息 */
                NAS_RABM_SndWasFastDormStartReq(RABM_FASTDORM_ENUM);
            }


        }
        else
        {
            /* 设置为NAS_RABM_FASTDORM_SUSPEND状态 */
            NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_SUSPEND);
        }
    }

    /* 回复给AT ID_RABM_AT_FASTDORM_START_CNF */
    NAS_RABM_SndAtSetFastDormParaCnf(pstFastDormPara->usClientId,
                                     pstFastDormPara->ucOpId,
                                     AT_RABM_PARA_SET_RSLT_SUCC);

    NAS_RABM_SndOmFastdormStatus();
}



VOS_VOID NAS_RABM_RcvAtFastDormStopReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
)
{

    /* 如果当前在NAS_RABM_FASTDORM_RUNNING状态，需要向WRR发送停止
       操作请求，停止FAST DORMANCY特性 */
    if (NAS_RABM_FASTDORM_RUNNING == NAS_RABM_GetCurrFastDormStatus())
    {
        /* 向WAS发送RRRABM_FASTDORM_STOP_REQ */
        NAS_RABM_SndWasFastDormStopReq();
    }

    /* 无流量时长计数清0 */
    NAS_RABM_SetFastDormCurrNoFluxCntValue(0);

    /* 停止定时器RABM_TIMER_FASTDORM_FLUX_DETECT */
    /* 停止定时器RABM_TIMER_FASTDORM_RETRY */
    /* 停止定时器RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT*/

    NAS_RabmStopTimer(RABM_TIMER_NAME_FD_FLUX_DETECT,RABM_TIMER_FASTDORM_FLUX_DETECT);

    NAS_RabmStopTimer(RABM_TIMER_NAME_FD_RETRY,RABM_TIMER_FASTDORM_RETRY);
    NAS_RabmStopTimer(RABM_TIMER_NAME_COMMON,RABM_TIMER_FASTDORM_WAIT_GMM_QRY_RESULT);

    NAS_RABM_SetFastDormOperationType(pstFastDormPara->stFastDormPara.enFastDormOperationType);

    /* 设置为NAS_RABM_FASTDORM_STOP状态 */
    NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_STOP);
    NAS_RABM_SndOmFastdormStatus();

    /* 回复给AT ID_RABM_AT_SET_FASTDORM_PARA_CNF */
    NAS_RABM_SndAtSetFastDormParaCnf(pstFastDormPara->usClientId,
                                     pstFastDormPara->ucOpId,
                                     AT_RABM_PARA_SET_RSLT_SUCC);
}
VOS_VOID NAS_RABM_RcvAtMsg(
    struct MsgCB                       *pstMsg
)
{
    MSG_HEADER_STRU                     *pstMsgHeader;

    pstMsgHeader = (MSG_HEADER_STRU*)pstMsg;

    switch (pstMsgHeader->ulMsgName)
    {
        case ID_AT_RABM_SET_FASTDORM_PARA_REQ:
            NAS_RABM_RcvSetFastDormParaReq((AT_RABM_SET_FASTDORM_PARA_REQ_STRU*)pstMsg);
            break;

        case ID_AT_RABM_QRY_FASTDORM_PARA_REQ:
            NAS_RABM_RcvGetFastDormParaReq((AT_RABM_QRY_FASTDORM_PARA_REQ_STRU*)pstMsg);
            break;

        case ID_AT_RABM_SET_RELEASE_RRC_REQ:
            NAS_RABM_RcvReleaseRrcReq((AT_RABM_RELEASE_RRC_REQ_STRU *)pstMsg);
            break;
        default:
            break;
    }
}


VOS_VOID NAS_RABM_RcvWasFastDormInfoInd(
    struct MsgCB                       *pMsg
)
{
    VOS_UINT8                               ucEntId;

    RRRABM_FASTDORM_INFO_IND_STRU      *pstInfoInd;

    pstInfoInd = (RRRABM_FASTDORM_INFO_IND_STRU*)pMsg;

    /* 如果在NAS_RABM_FASTDORM_RUNNING状态，则需要进入RETRY状态 */
    if ((NAS_RABM_FASTDORM_RUNNING == NAS_RABM_GetCurrFastDormStatus())
      &&(RRRABM_FASTDORM_INFO_NEED_RETRY == pstInfoInd->enInfoType))
    {

        /* 当前只有存在RAB的时候，才需要启动retry定时器，否则应该可以直接
            return */
        for ( ucEntId = 0; ucEntId < RABM_PS_MAX_ENT_NUM; ucEntId++ )
        {
            if (g_aRabmPsEnt[ucEntId].ucState == RABM_NSAPI_ACTIVE_WITH_RAB)
            {
                break;
            }
        }

        if (ucEntId >= RABM_PS_MAX_ENT_NUM)
        {

            return;
        }

        /* 当前在NAS_RABM_FASTDORM_RETRY状态 */;
        NAS_RABM_SetCurrFastDormStatus(NAS_RABM_FASTDORM_RETRY);
        NAS_RABM_SndOmFastdormStatus();
        /* 启动RABM_TIMER_FASTDORM_RETRY定时器 */
        NAS_RabmStartTimer( RABM_TIMER_NAME_FD_RETRY, RABM_TIMER_FASTDORM_RETRY );
    }
}


VOS_VOID NAS_RABM_SetFastDormUserDefNoFluxCntValue(
    VOS_UINT32                          ulNoFluxCnt
)
{
    g_stNasRabmFastDormCtx.ulUserDefNoFluxCnt = ulNoFluxCnt;
}


VOS_UINT32 NAS_RABM_GetFastDormUserDefNoFluxCntValue(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.ulUserDefNoFluxCnt;
}


VOS_VOID NAS_RABM_SetFastDormCurrNoFluxCntValue(
    VOS_UINT32                          ulNoFluxCnt
)
{
    g_stNasRabmFastDormCtx.ulCurrNoFluxCnt = ulNoFluxCnt;
}


VOS_UINT32 NAS_RABM_GetFastDormCurrNoFluxCntValue(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.ulCurrNoFluxCnt;
}


VOS_VOID NAS_RABM_SetFastDormOperationType(
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32      enFastDormOperationType
)
{
    g_stNasRabmFastDormCtx.enFastDormOperationType = enFastDormOperationType;
}


AT_RABM_FASTDORM_OPERATION_ENUM_UINT32 NAS_RABM_GetFastDormOperationType(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.enFastDormOperationType;
}


NAS_RABM_FASTDORM_STATUS_ENUM_UINT32 NAS_RABM_GetCurrFastDormStatus(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.enCurrFastDormStatus;
}


VOS_VOID NAS_RABM_SetCurrFastDormStatus(
    NAS_RABM_FASTDORM_STATUS_ENUM_UINT32    enCurrFastDormStatus
)
{
    g_stNasRabmFastDormCtx.enCurrFastDormStatus = enCurrFastDormStatus;
}


VOS_VOID NAS_RABM_InitFastDormCtx(VOS_VOID)
{
    g_stNasRabmFastDormCtx.enCurrFastDormStatus    = NAS_RABM_FASTDORM_STOP;
    g_stNasRabmFastDormCtx.ulCurrNoFluxCnt         = 0;
    g_stNasRabmFastDormCtx.enFastDormOperationType = AT_RABM_FASTDORM_STOP_FD_ASCR;
    g_stNasRabmFastDormCtx.ulUserDefNoFluxCnt      = 0;

    g_stNasRabmFastDormCtx.ulDlDataCnt             = 0;
    g_stNasRabmFastDormCtx.ulUlDataCnt             = 0;
}




VOS_VOID NAS_RABM_IncFastDormUlDataCnt(VOS_VOID)
{
    g_stNasRabmFastDormCtx.ulUlDataCnt++;
}


VOS_VOID NAS_RABM_IncFastDormDlDataCnt(VOS_VOID)
{
    g_stNasRabmFastDormCtx.ulDlDataCnt++;
}


VOS_UINT32 NAS_RABM_GetFastDormUlDataCnt(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.ulUlDataCnt;
}


VOS_UINT32 NAS_RABM_GetFastDormDlDataCnt(VOS_VOID)
{
    return g_stNasRabmFastDormCtx.ulDlDataCnt;
}


VOS_VOID NAS_RABM_ClrFastDormUlDataCnt(VOS_VOID)
{
    g_stNasRabmFastDormCtx.ulUlDataCnt = 0;
}


VOS_VOID NAS_RABM_ClrFastDormDlDataCnt(VOS_VOID)
{
    g_stNasRabmFastDormCtx.ulDlDataCnt = 0;
}



VOS_VOID NAS_RABM_RcvCdsMsg(
    struct MsgCB                       *pstMsg
)
{
    MSG_HEADER_STRU                     *pstMsgHeader;

    pstMsgHeader = (MSG_HEADER_STRU*)pstMsg;

    switch (pstMsgHeader->ulMsgName)
    {
        case ID_CDS_RABM_SERVICE_IND:
            NAS_RABM_RcvCdsServiceInd((CDS_RABM_SERVICE_IND_STRU *)pstMsg);
            break;

        default:
            break;
    }
}


VOS_UINT32 NAS_RABM_IsDataServiceRequestPending(VOS_VOID)
{
    if (VOS_TRUE == NAS_RABM_GetRabRsestTimerFlg())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_RABM_IsRabReestablishPending(VOS_VOID)
{
    VOS_UINT8                           ucEntId;

    if (VOS_TRUE == NAS_RABM_GetRabRsestTimerFlg())
    {
        return VOS_FALSE;
    }

    for (ucEntId = 0; ucEntId < RABM_3G_PS_MAX_ENT_NUM; ucEntId++)
    {
        if (VOS_TRUE == NAS_RABM_GetWPsEntRabReestFlg(ucEntId))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_VOID NAS_RABM_RcvCdsServiceInd(
    CDS_RABM_SERVICE_IND_STRU          *pstCdsServiceInd
)
{
    /* 检查RABID有效性 */
    if (!RAB_MAP_RAB_IS_VALID(pstCdsServiceInd->ucRabId))
    {
        NAS_ERROR_LOG1(WUEPS_PID_RABM,
            "NAS_RABM_RcvCdsServiceInd: Invalid RABID. <RABID>", pstCdsServiceInd->ucRabId);
        return;
    }

    switch (NAS_RABM_GetWPsEntState(pstCdsServiceInd->ucRabId - RABM_NSAPI_OFFSET))
    {
        case RABM_NSAPI_ACTIVE_WITH_RAB:
            NAS_NORMAL_LOG1(WUEPS_PID_RABM,
                "NAS_RABM_RcvCdsServiceInd: RABID is already active. <RABID>", pstCdsServiceInd->ucRabId);
            NAS_RABM_SndCdsSendBuffDataInd(pstCdsServiceInd->ucRabId, CDS_RABM_SEND_BUFF_DATA_ALLOWED_TYPE_SERVICE_SUCC);
            break;

        case RABM_NSAPI_ACTIVE_NO_RAB:
            if (VOS_TRUE == NAS_RABM_GetWPsEntRabReestFlg(pstCdsServiceInd->ucRabId - RABM_NSAPI_OFFSET))
            {
                NAS_NORMAL_LOG1(WUEPS_PID_RABM,
                    "NAS_RABM_RcvCdsServiceInd: Reestablish is in process. <RABID>", pstCdsServiceInd->ucRabId);
            }
            else if (VOS_TRUE == NAS_RABM_IsDataServiceRequestPending())
            {
                NAS_NORMAL_LOG1(WUEPS_PID_RABM,
                    "NAS_RABM_RcvCdsServiceInd: Data service request is sent, set reestablish flag. <RABID>", pstCdsServiceInd->ucRabId);
                NAS_RABM_SetWPsEntRabReestFlg(pstCdsServiceInd->ucRabId - RABM_NSAPI_OFFSET);
            }
            else if (VOS_TRUE == NAS_RABM_IsRabReestablishPending())
            {
                NAS_NORMAL_LOG1(WUEPS_PID_RABM,
                    "NAS_RABM_RcvCdsServiceInd: Start RB setup protect timer. <RABID>", pstCdsServiceInd->ucRabId);
                NAS_RABM_SetWPsEntRabReestFlg(pstCdsServiceInd->ucRabId - RABM_NSAPI_OFFSET);
                NAS_RABM_StartReestRabPendingTmr(pstCdsServiceInd->ucRabId);
            }
            else
            {
                NAS_NORMAL_LOG1(WUEPS_PID_RABM,
                    "NAS_RABM_RcvCdsServiceInd: Send data service request.  <RABID>", pstCdsServiceInd->ucRabId);
                NAS_RABM_SetWPsEntRabReestFlg(pstCdsServiceInd->ucRabId - RABM_NSAPI_OFFSET);
                NAS_RABM_SetRabRsestTimerFlg();
                RABM_TimerStart(RABM_REESTABLISH_REQ_SENT, RABM_RABM_REEST_PROT_TIMER_LEN);
                RABM_SndRabReestReq();
            }
            break;

        default:
            NAS_WARNING_LOG1(WUEPS_PID_RABM,
                "NAS_RABM_RcvCdsServiceInd: Can't reestablish RAB. <RABID>", pstCdsServiceInd->ucRabId);
            NAS_RABM_SndCdsFreeBuffDataInd(pstCdsServiceInd->ucRabId);
            break;
    }

    return;
}
VOS_VOID NAS_RABM_RcvReleaseRrcReq(
    AT_RABM_RELEASE_RRC_REQ_STRU *pstMsg
)
{
    GMM_RABM_NET_RAT_ENUM_UINT32        enCurrentSysMode;

    /* 获取当前所处网络 */
    enCurrentSysMode = NAS_RABM_GetCurrentSysMode();

    /* 如果不在W网络下，则不能释放RRC连接 */
    if (NAS_MML_NET_RAT_TYPE_WCDMA != enCurrentSysMode)
    {
        NAS_RABM_SndAtReleaseRrcCnf(pstMsg->usClientId, pstMsg->ucOpId, VOS_ERR);
    }
    else
    {
        /* 直接回复rrc释放成功 */
        NAS_RABM_SndAtReleaseRrcCnf(pstMsg->usClientId, pstMsg->ucOpId, VOS_OK);

        /* 向GMM发送查询消息 */
        NAS_RABM_SndGmmMmlProcStatusQryReq(RABM_RELEASE_RRC_ENUM);

    }

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


