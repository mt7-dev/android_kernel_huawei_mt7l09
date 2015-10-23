

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "PsCommonDef.h"
#include "Taf_Tafm_Remote.h"
#include "MnApsComm.h"
#include "MmaAppLocal.h"
#include "TafLog.h"
#include "TafApsCtx.h"
#include "TafApsProcNvim.h"
#include "TafAgentInterface.h"
#include "TafApsPreproc.h"
#include "TafApsComFunc.h"
#include "TafApsSndAt.h"
#include "ApsNdInterface.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "TafApsSndL4a.h"
#include "MnApsMultiMode.h"
#include "TafSdcLib.h"
#endif
#include "TafApsProcIpFilter.h"
#include "SmDef.h"

#include "TafApsApi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_APS_PREPROC_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/




VOS_UINT32 TAF_APS_RcvAtGetDsFlowInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstMsgHeader;
    TAF_PS_GET_DSFLOW_INFO_REQ_STRU    *pstGetDsFlowInfoReq;
    TAF_DSFLOW_QUERY_INFO_STRU          stDsFlowQryInfo;

    /* 初始化 */
    pstMsgHeader        = (TAF_PS_MSG_STRU *)pstMsg;
    pstGetDsFlowInfoReq = (TAF_PS_GET_DSFLOW_INFO_REQ_STRU *)(pstMsgHeader->aucContent);

    /* 流量上报信息初始化 */
    PS_MEM_SET(&stDsFlowQryInfo, 0, sizeof(TAF_DSFLOW_QUERY_INFO_STRU));

    /* 查询所有RAB承载的流量信息 */
    TAF_APS_QryAllRabDsFlowStats(&stDsFlowQryInfo);

    /* 给AT回复查询到的流量信息 */
    TAF_APS_SndGetDsFlowInfoCnf(&(pstGetDsFlowInfoReq->stCtrl),
                                TAF_ERR_NO_ERROR,
                               &stDsFlowQryInfo);


    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtClearDsFlowReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstMsgHeader;
    TAF_PS_CLEAR_DSFLOW_REQ_STRU       *pstClearDsFlowReq;
    TAF_APS_DSFLOW_STATS_CTX_STRU      *pstApsDsflowCtx;


    /* 初始化 */
    pstMsgHeader        = (TAF_PS_MSG_STRU*)pstMsg;
    pstClearDsFlowReq   = (TAF_PS_CLEAR_DSFLOW_REQ_STRU*)(pstMsgHeader->aucContent);

    /* 清除所有RAB承载的流量信息 */
    TAF_APS_ClearAllRabDsFlowStats();

    /* 如果流量保存NV功能使能，清除NV项中历史信息 */
    pstApsDsflowCtx = TAF_APS_GetDsFlowCtxAddr();
    if (VOS_TRUE == pstApsDsflowCtx->ucApsDsFlowSave2NvFlg)
    {
        TAF_APS_ClearDsFlowInfoInNv();
    }

    /* 给AT回复结果 */
    TAF_APS_SndClearDsFlowCnf(&(pstClearDsFlowReq->stCtrl),
                               TAF_ERR_NO_ERROR);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtConfigDsFlowRptReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstMsgHeader;
    TAF_PS_CONFIG_DSFLOW_RPT_REQ_STRU  *pstConfigDsFlowRptReq;
    VOS_UINT32                          ulRptCmdStatus;

    /* 初始化 */
    pstMsgHeader            = (TAF_PS_MSG_STRU*)pstMsg;
    pstConfigDsFlowRptReq   = (TAF_PS_CONFIG_DSFLOW_RPT_REQ_STRU*)(pstMsgHeader->aucContent);

    /* 更新单个命令设置主动上报的全局变量 */
    if ( VOS_TRUE == pstConfigDsFlowRptReq->stReportConfigInfo.ulRptEnabled )
    {
        /* 启动AT端口流量上报 */
        ulRptCmdStatus = VOS_TRUE;
    }
    else
    {
        /* 停止AT端口流量上报 */
        ulRptCmdStatus = VOS_FALSE;
    }

    TAF_SDC_UpdateRptCmdStatus(TAF_SDC_RPT_CMD_DSFLOWRPT, ulRptCmdStatus);

    /* 给AT回复清除结果 */
    TAF_APS_SndConfigDsFlowRptCnf(&(pstConfigDsFlowRptReq->stCtrl),
                                  TAF_ERR_NO_ERROR);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiDsFlowReportExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    TAF_DSFLOW_REPORT_STRU              stTotalDsFlowRptInfo;
    TAF_APS_DSFLOW_STATS_CTX_STRU      *pstApsDsflowCtx;

    /* 初始化 */
    PS_MEM_SET(&stCtrl, 0x00, sizeof(TAF_CTRL_STRU));
    pstApsDsflowCtx = TAF_APS_GetDsFlowCtxAddr();

    /* 如果当前无PDP激活，返回 */
    if (VOS_FALSE == TAF_APS_CheckIfActiveRabIdExist())
    {
        return VOS_TRUE;
    }

    /* 初始化流量查询信息变量 */
    PS_MEM_SET(&stTotalDsFlowRptInfo, 0, sizeof(TAF_DSFLOW_REPORT_STRU));

    /* 查询所有激活RAB承载的上报流量信息 */
    TAF_APS_QryAllRabDsFlowReportInfo(&stTotalDsFlowRptInfo);

    pstApsDsflowCtx->ucDsFlowATRptTmrExpCnt = (pstApsDsflowCtx->ucDsFlowATRptTmrExpCnt + 1)
                                              % pstApsDsflowCtx->ucDsFlowATRptPeriod;

    if (0 == pstApsDsflowCtx->ucDsFlowATRptTmrExpCnt)
    {
        /* 给AT上报流量信息 */
        stCtrl.ulModuleId    = WUEPS_PID_AT;
        stCtrl.usClientId    = MN_CLIENT_ID_BROADCAST;
        stCtrl.ucOpId        = 0;

        TAF_APS_SndDsFlowRptInd(&stCtrl, TAF_ERR_NO_ERROR, &stTotalDsFlowRptInfo);
    }

    if (VOS_TRUE == pstApsDsflowCtx->ucDsFlowOMReportFlg)
    {
        if (0 == pstApsDsflowCtx->ucDsFlowOMRptTmrExpCnt)
        {
            /* 给OM上报流量信息 */
            TAF_APS_SndDsFlowOMRptInd(&stTotalDsFlowRptInfo);
        }

        pstApsDsflowCtx->ucDsFlowOMRptTmrExpCnt = (pstApsDsflowCtx->ucDsFlowOMRptTmrExpCnt + 1)
                                                  % pstApsDsflowCtx->ucDsFlowOMRptPeriod;
    }

    /* 保存当前流量信息到流量统计上下文全局变量，方便下次定时器超时计算速率 */
    TAF_APS_UpdateAllRabCurrentFlowInfo();

    /* APS统计的流量是MB为单位的，故需要转换为Mb处理 */
    TAF_APS_SwitchDdrRateByCurrentRate((stTotalDsFlowRptInfo.ulCurrentReceiveRate * 8),
                                       (stTotalDsFlowRptInfo.ulCurrentSendRate * 8));

    /* 启动流量上报定时器*/
    TAF_APS_StartTimer(TI_TAF_APS_DSFLOW_REPORT,
                       TI_TAF_APS_DSFLOW_REPORT_LEN,
                       TAF_APS_INVALID_PDPID);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiDsFlowWriteNvExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_DSFLOW_STATS_CTX_STRU      *pstApsDsflowCtx;

    /* 初始化 */
    pstApsDsflowCtx = TAF_APS_GetDsFlowCtxAddr();

    /* 保存历史流量到NV使能，将历史流量信息保存到NV中 */
    TAF_APS_SaveDsFlowInfoToNv();

    /* 判断保存历史流量到NV是否使能 */
    if (VOS_TRUE == pstApsDsflowCtx->ucApsDsFlowSave2NvFlg)
    {
        /* 重新启动定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_DSFLOW_WRITE_NV,
                           (pstApsDsflowCtx->ucApsDsFlowSavePeriod * 60 * TIMER_S_TO_MS_1000),
                           TAF_APS_INVALID_PDPID);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtGetPrimPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetPrimPdpCtxInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetPdpInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                            *pstPsMsg;
    VOS_UINT8                                   ucIndex;
    VOS_UINT32                                  ulPdpCtxInfoLen;
    VOS_UINT32                                  ulCidNum;
    TAF_PS_GET_PDP_CONTEXT_INFO_REQ_STRU       *pstGetPdpCtxInfoReq;
    TAF_PS_GET_PDP_CONTEXT_INFO_CNF_STRU       *pstGetPdpCtxInfoCnf;

    /* 初始化消息 */
    pstPsMsg            = (TAF_PS_MSG_STRU*)pstMsg;

    ulPdpCtxInfoLen     = 0;
    ulCidNum            = 0;
    pstGetPdpCtxInfoReq = (TAF_PS_GET_PDP_CONTEXT_INFO_REQ_STRU*)pstPsMsg->aucContent;
    pstGetPdpCtxInfoCnf = VOS_NULL_PTR;

    /* 计算事件内容长度 */
    ulPdpCtxInfoLen     = sizeof(TAF_PS_GET_PDP_CONTEXT_INFO_CNF_STRU);

    /* 申请内存 */
    pstGetPdpCtxInfoCnf = (TAF_PS_GET_PDP_CONTEXT_INFO_CNF_STRU*)PS_MEM_ALLOC(
                                WUEPS_PID_TAF,
                                ulPdpCtxInfoLen);

    /* 申请消息失败 */
    if ( VOS_NULL_PTR == pstGetPdpCtxInfoCnf )
    {
        MN_ERR_LOG("MN_APS_RcvGetPdpCtxInfoReq:  ERROR : PS_MEM_ALLOC Error!");
        return VOS_TRUE;
    }

    /* 填写事件内容 */
    for ( ucIndex = 1; ucIndex <= TAF_MAX_CID; ucIndex++ )
    {
        /*该PDP没有定义*/
        if (TAF_FREE == g_TafCidTab[ucIndex].ucUsed)
        {
            continue;
        }

        /* 不区分主从PDP,只要是定义的就返回 */
        pstGetPdpCtxInfoCnf->ulCid[ulCidNum]    = ucIndex;
        ulCidNum++;
    }

    /* 填写事件控制头 */
    pstGetPdpCtxInfoCnf->stCtrl     = pstGetPdpCtxInfoReq->stCtrl;
    pstGetPdpCtxInfoCnf->ulCidNum   = ulCidNum;
    pstGetPdpCtxInfoCnf->enCause    = TAF_PS_CAUSE_SUCCESS;

    /* 返回处理结果 */
    TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_PDP_CONTEXT_INFO_CNF,
                    pstGetPdpCtxInfoCnf,
                    ulPdpCtxInfoLen);

    /* 释放用于存放PDP上下文参数的内存 */
    PS_MEM_FREE(WUEPS_PID_TAF, pstGetPdpCtxInfoCnf);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetSecPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetSecPdpCtxInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetTftInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetTftInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetPdpIpAddrInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetPdpIpAddrInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetAnsModeInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetAnsModeInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetPdpContextStateReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetPdpCtxStateReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}




#if ( FEATURE_ON == FEATURE_LTE )


VOS_UINT32 TAF_APS_RcvAtGetDynamicPrimPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 发送消息给L4A */
    TAF_APS_SndL4aSetCgcontrdpReq(pstPsMsg->aucContent);

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF,
                                     (VOS_UINT32 *)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetDynamicSecPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 发送消息给L4A */
    TAF_APS_SndL4aSetCgscontrdpReq(pstPsMsg->aucContent);

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF,
                                     (VOS_UINT32 *)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetDynamicTftInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 发送消息给L4A */
    TAF_APS_SndL4aSetCgtftrdpReq(pstPsMsg->aucContent);

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));
    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF,
                                     (VOS_UINT32 *)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_GET_DYNAMIC_TFT_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetEpsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulErrCode;
    VOS_UINT32                          ulEqosInfoLen;
    VOS_UINT32                          ulCidNum;
    TAF_PS_MSG_STRU                    *pstPsMsg;
    TAF_PS_GET_EPS_QOS_INFO_REQ_STRU   *pstGetEqosInfoReq;
    TAF_PS_GET_EPS_QOS_INFO_CNF_STRU   *pstGetEqosInfoCnf;

    /* 初始化消息 */
    pstPsMsg                = (TAF_PS_MSG_STRU*)pstMsg;
    ulErrCode               = TAF_PARA_OK;
    ulEqosInfoLen           = 0;
    ulCidNum                = 0;
    pstGetEqosInfoReq       = (TAF_PS_GET_EPS_QOS_INFO_REQ_STRU*)pstPsMsg->aucContent;
    pstGetEqosInfoCnf       = VOS_NULL_PTR;

    /* 计算事件内容长度 */
    ulEqosInfoLen     = sizeof(TAF_PS_GET_EPS_QOS_INFO_CNF_STRU)
                            + ((TAF_MAX_CID + 1) * sizeof(TAF_EPS_QOS_EXT_STRU));

    /* 申请内存 */
    pstGetEqosInfoCnf = (TAF_PS_GET_EPS_QOS_INFO_CNF_STRU*)PS_MEM_ALLOC(
                                WUEPS_PID_TAF,
                                ulEqosInfoLen);

    /* 申请消息失败 */
    if ( VOS_NULL_PTR == pstGetEqosInfoCnf )
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvAtGetEpsQosInfoReq_PreProc:  ERROR : PS_MEM_ALLOC Error!");
        return VOS_TRUE;
    }

    /* 填写事件内容 */
    for ( ucIndex = 1; ucIndex <= TAF_MAX_CID; ucIndex++ )
    {
        PS_MEM_SET(&(pstGetEqosInfoCnf->astEpsQosInfo[ulCidNum]),
                   0x00,
                   sizeof(TAF_EPS_QOS_EXT_STRU));

        /* 获取EPS QOS信息 */
        TAF_APS_QueEpsQosInfo(ucIndex, &(pstGetEqosInfoCnf->astEpsQosInfo[ulCidNum]), &ulErrCode);

        if ( TAF_PARA_OK == ulErrCode )
        {
            ulCidNum++;
        }
    }

    /* 填写事件控制头 */
    pstGetEqosInfoCnf->stCtrl           = pstGetEqosInfoReq->stCtrl;
    pstGetEqosInfoCnf->ulCidNum         = ulCidNum;
    pstGetEqosInfoCnf->enCause          = TAF_PS_CAUSE_SUCCESS;

    /* 返回处理结果 */
    TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_EPS_QOS_INFO_CNF,
                     pstGetEqosInfoCnf,
                     ulEqosInfoLen);

    /* 释放用于存放PDP上下文参数的内存 */
    PS_MEM_FREE(WUEPS_PID_TAF, pstGetEqosInfoCnf);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtGetDynamicEpsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 发送消息给L4A */
    TAF_APS_SndL4aSetCgeqosrdpReq(pstPsMsg->aucContent);

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));
    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF,
                                     (VOS_UINT32 *)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtGetLteCsInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    TAF_PS_MSG_STRU                 *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

    /* 发送消息给L4A */
    TAF_APS_SndL4aGetLteCsReq(pstPsMsg->aucContent);

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_GET_LTE_CS_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_LTE_CS_CNF,
                                     (VOS_UINT32*)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_LTECS_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvL4aGetLteCsCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_GET_LTE_CS_CNF_STRU        *pstPsMsg = VOS_NULL_PTR;
    TAF_PS_LTECS_CNF_STRU              *pstApsAtLteCsInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg = (APS_L4A_GET_LTE_CS_CNF_STRU*)pstMsg;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_GET_LTE_CS_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_GET_LTE_CS_CNF, ulPara);

        /* 返回AT处理结果 */
        pstApsAtLteCsInfo =  (TAF_PS_LTECS_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(TAF_PS_LTECS_CNF_STRU));

        if ( VOS_NULL_PTR == pstApsAtLteCsInfo )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aGetLteCsCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        PS_MEM_SET(pstApsAtLteCsInfo, 0x00,  sizeof( TAF_PS_LTECS_CNF_STRU));

        /* L4A错误参数转换 */
        pstApsAtLteCsInfo->enCause = TAF_APS_TransL4aErrorCode(pstPsMsg->ulErrorCode);

        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstApsAtLteCsInfo->stCtrl,
                                                              TI_TAF_APS_WAIT_GET_LTE_CS_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aGetLteCsCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstApsAtLteCsInfo);

            return VOS_TRUE;
        }

        if(pstApsAtLteCsInfo->enCause == TAF_PS_CAUSE_SUCCESS )
        {
            pstApsAtLteCsInfo->stLteCs.ucSG     = pstPsMsg->ucSmsOverSGs;
            pstApsAtLteCsInfo->stLteCs.ucIMS    = pstPsMsg->ucSmsOverIMS;
            pstApsAtLteCsInfo->stLteCs.ucCSFB   = pstPsMsg->ucCSFB;
            pstApsAtLteCsInfo->stLteCs.ucVCC    = pstPsMsg->ucSrVcc;
            pstApsAtLteCsInfo->stLteCs.ucVoLGA  = pstPsMsg->ucVoLGA;
        }

        /* 将消息返回 */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_LTECS_INFO_CNF,
                         pstApsAtLteCsInfo,
                         sizeof(TAF_PS_LTECS_CNF_STRU));

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstApsAtLteCsInfo);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_LTE_CS_CNF, ulPara);

    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetCemodeInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

    /* 发送消息给L4A */
    TAF_APS_SndL4aGetCemodeReq(pstPsMsg->aucContent);

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_GET_CEMODE_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_CEMODE_CNF,
                                     (VOS_UINT32*)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_CEMODE_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aGetCemodeCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_GET_CEMODE_CNF_STRU        *pstPsMsg;
    TAF_PS_CEMODE_CNF_STRU             *pstApsAtCemodeInfo = NULL;
    VOS_UINT32                          i;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_GET_CEMODE_CNF_STRU*)pstMsg;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_GET_CEMODE_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_GET_CEMODE_CNF, ulPara);

        /* 返回AT处理结果 */
        pstApsAtCemodeInfo =  (TAF_PS_CEMODE_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(TAF_PS_CEMODE_CNF_STRU));

        if ( VOS_NULL_PTR == pstApsAtCemodeInfo )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aGetCemodeCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        PS_MEM_SET(pstApsAtCemodeInfo, 0x00,  sizeof( TAF_PS_CEMODE_CNF_STRU));

        /* L4A错误参数转换 */
        pstApsAtCemodeInfo->enCause = TAF_APS_TransL4aErrorCode(pstPsMsg->ulErrorCode);;

        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstApsAtCemodeInfo->stCtrl,
                                                              TI_TAF_APS_WAIT_GET_CEMODE_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aGetCemodeCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstApsAtCemodeInfo);

            return VOS_TRUE;
        }

        if (pstApsAtCemodeInfo->enCause == TAF_PS_CAUSE_SUCCESS )
        {
            pstApsAtCemodeInfo->stCemode.enCurrentUeMode    = pstPsMsg->ulCurrentUeMode;
            pstApsAtCemodeInfo->stCemode.ulSupportModeCnt   = pstPsMsg->ulSupportModeCnt;

            for (i = 0; i < 4; i++)
            {
                pstApsAtCemodeInfo->stCemode.aenSupportModeList[i] = pstPsMsg->aulSupportModeList[i];
            }
        }

            /* 将消息返回AT */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_CEMODE_INFO_CNF,
                         pstApsAtCemodeInfo,
                         sizeof( TAF_PS_CEMODE_CNF_STRU));

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstApsAtCemodeInfo);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_CEMODE_CNF, ulPara);

    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdprofInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    ulPara  = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                            TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));
    /* 发送消息给L4A */
    TAF_APS_SndL4aSetPdprofReq(pstPsMsg->aucContent);

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_PDPROF_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_PDPROF_CNF,
                                     (VOS_UINT32*)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_SET_PROFILE_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aSetPdprofInfoCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_PDPROFMOD_CNF_STRU     *pstPsMsg;
    TAF_CTRL_STRU                       stCtrl = {0};
    VOS_UINT32                          ulErrCode;
    TAF_APS_CMD_BUFFER_STRU            *pstMsgInfo;
    TAF_PS_SET_PROFILE_INFO_REQ_STRU   *pstSetPdprofInfoReq;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_PDPROFMOD_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetPdprofInfoReq     = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_PDPROF_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_PDPROF_CNF, ulPara);

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo  = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_PDPROF_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetPdprofInfoCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetPdprofInfoReq = (TAF_PS_SET_PROFILE_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;

        stCtrl = pstSetPdprofInfoReq->stCtrl;

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 设置Primary PDP上下文和AUTHDATA参数(NDIS)  */
            Aps_DefPsPdprofmod(pstSetPdprofInfoReq->stCtrl.usClientId,
                               pstSetPdprofInfoReq->stCtrl.ucOpId,
                               &(pstSetPdprofInfoReq->stPdpProfInfo),
                               &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_PDPROF_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetPdprofInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}


#endif
VOS_UINT32 TAF_APS_RcvAtGetAuthDataInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetAuthDataInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetUmtsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetUmtsQosInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetUmtsQosMinInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetUmtsQosMinInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetDynamicUmtsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetDynamicUmtsQosInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetPdpAuthInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetPdpAuthInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtGetPdpDnsInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvGetPdpDnsInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtSetPrimPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                             *pstPsMsg;
    TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ_STRU   *pstSetPdpCtxInfoReq;
#if ( FEATURE_ON == FEATURE_LTE )
    VOS_UINT32                          ulPara;
#endif

    /* 初始化消息 */
    pstPsMsg            = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetPdpCtxInfoReq = (TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ_STRU *)pstPsMsg->aucContent;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstSetPdpCtxInfoReq->stCtrl)))
    {
        TAF_APS_SndSetPrimPdpCtxInfoCnf(&(pstSetPdpCtxInfoReq->stCtrl), TAF_PARA_UNSPECIFIED_ERROR);
        return VOS_TRUE;
    }
#endif
#if ( FEATURE_ON == FEATURE_LTE )
    /* 根据LTE能力分别执行 */
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        /* 发送消息给L4A */
        TAF_APS_SndL4aSetCgdcontReq(pstSetPdpCtxInfoReq);

        ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstSetPdpCtxInfoReq),
                                               TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstSetPdpCtxInfoReq));

        /* 启动L4A消息回复保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGDCONT_CNF,
                           TI_TAF_APS_WAIT_L4A_CNF_LEN,
                           ulPara);

        /* 添加消息进等待队列 */
        TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDCONT_CNF,
                                         (VOS_UINT32*)pstSetPdpCtxInfoReq,
                                         sizeof(TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ_STRU),
                                         ulPara);

       return VOS_TRUE;
    }

#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetPrimPdpCtxInfoReq(pstSetPdpCtxInfoReq);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetSecPdpCtxInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
#if ( FEATURE_ON == FEATURE_LTE )
    VOS_UINT32                          ulPara;
#endif

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

#if ( FEATURE_ON == FEATURE_LTE )
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        /* 发送消息给L4A */
        TAF_APS_SndL4aSetCgdscontReq(pstPsMsg->aucContent);

        ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                               TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));
        /* 启动L4A消息回复保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF,
                           TI_TAF_APS_WAIT_L4A_CNF_LEN,
                           ulPara);

        /* 添加消息进等待队列 */
        TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF,
                                         (VOS_UINT32*)pstPsMsg->aucContent,
                                         sizeof(TAF_PS_SET_SEC_PDP_CONTEXT_INFO_REQ_STRU),
                                         ulPara);

	    return VOS_TRUE;
    }
#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetSecPdpCtxInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetTftInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
#if ( FEATURE_ON == FEATURE_LTE )
    VOS_UINT32                          ulPara;
#endif

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;
#if ( FEATURE_ON == FEATURE_LTE )
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        /* 发送消息给L4A */
        TAF_APS_SndL4aSetCgtftReq(pstPsMsg->aucContent);

        ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                               TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

        /* 启动L4A消息回复保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGTFT_CNF,
                           TI_TAF_APS_WAIT_L4A_CNF_LEN,
                           ulPara);

        /* 添加消息进等待队列 */
        TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFT_CNF,
                                         (VOS_UINT32*)pstPsMsg->aucContent,
                                         sizeof(TAF_PS_SET_TFT_INFO_REQ_STRU),
                                         ulPara);

        return VOS_TRUE;
    }

#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetTftInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetAnsModeInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
#if ( FEATURE_ON == FEATURE_LTE )
    VOS_UINT32                          ulPara;
#endif

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

#if ( FEATURE_ON == FEATURE_LTE )
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        /* 发送消息给L4A */
        TAF_APS_SndL4aSetCgautoReq(pstPsMsg->aucContent);

        ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                               TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));
        /* 启动L4A消息回复保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGAUTO_CNF,
                           TI_TAF_APS_WAIT_L4A_CNF_LEN,
                           ulPara);

        /* 添加消息进等待队列 */
        TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGAUTO_CNF,
                                         (VOS_UINT32*)pstPsMsg->aucContent,
                                         sizeof(TAF_PS_SET_ANSWER_MODE_INFO_REQ_STRU),
                                         ulPara);

        return VOS_TRUE;
    }
#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetAnsModeInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetAuthDataInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                     *pstPsMsg;
    TAF_PS_SET_AUTHDATA_INFO_REQ_STRU   *pstAuthdata;
#if ( FEATURE_ON == FEATURE_LTE )
    VOS_UINT32                          ulPara;
#endif

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstAuthdata     = (TAF_PS_SET_AUTHDATA_INFO_REQ_STRU *)pstPsMsg->aucContent;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstAuthdata->stCtrl)))
    {
        TAF_APS_SndSetAuthDataInfoCnf(&(pstAuthdata->stCtrl), TAF_PARA_UNSPECIFIED_ERROR);
        return VOS_TRUE;
    }
#endif

#if ( FEATURE_ON == FEATURE_LTE )
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        /* 发送消息给L4A */
        TAF_APS_SndL4aSetAuthdataReq(pstAuthdata);

        ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstAuthdata),
                                               TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstAuthdata));

        /* 启动L4A消息回复保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF,
                           TI_TAF_APS_WAIT_L4A_CNF_LEN,
                           ulPara);

        /* 添加消息进等待队列 */
        TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF,
                                         (VOS_UINT32*)pstAuthdata,
                                         sizeof(TAF_PS_SET_AUTHDATA_INFO_REQ_STRU),
                                         ulPara);

        return VOS_TRUE;
    }
#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetAuthDataInfoReq(pstAuthdata);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetUmtsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                     *pstPsMsg;
    TAF_PS_SET_UMTS_QOS_INFO_REQ_STRU   *pstSetQosReq;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetQosReq    = (TAF_PS_SET_UMTS_QOS_INFO_REQ_STRU *)pstPsMsg->aucContent;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstSetQosReq->stCtrl)))
    {
        TAF_APS_SndSetUmtsQosInfoCnf(&(pstSetQosReq->stCtrl), TAF_PARA_UNSPECIFIED_ERROR);
        return VOS_TRUE;
    }
#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetUmtsQosInfoReq(pstSetQosReq);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetUmtsQosMinInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                         *pstPsMsg;
    TAF_PS_SET_UMTS_QOS_MIN_INFO_REQ_STRU   *pstSetQosMinReq;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetQosMinReq = (TAF_PS_SET_UMTS_QOS_MIN_INFO_REQ_STRU *)pstPsMsg->aucContent;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstSetQosMinReq->stCtrl)))
    {
        TAF_APS_SndSetUmtsQosMinInfoCnf(&(pstSetQosMinReq->stCtrl), TAF_PARA_UNSPECIFIED_ERROR);
        return VOS_TRUE;
    }
#endif

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetUmtsQosMinInfoReq(pstSetQosMinReq);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtSetPdpAuthInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetPdpAuthInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpDnsInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvSetPdpDnsInfoReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtTrigGprsDataReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvTrigGprsDataReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtConfigNbnsFunctionReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                 *pstPsMsg;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    /* 去消息头, 调用原处理函数处理并回复AT */
    MN_APS_RcvConfigNbnsFunctionReq(pstPsMsg->aucContent);

    return VOS_TRUE;
}






VOS_UINT32 TAF_APS_RcvAtFindCidForDialReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAFAGENT_APS_FIND_CID_FOR_DIAL_CNF_STRU  *pCnf;
    VOS_UINT8                                 ucCid;

    pCnf = (TAFAGENT_APS_FIND_CID_FOR_DIAL_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                                   sizeof(TAFAGENT_APS_FIND_CID_FOR_DIAL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pCnf)
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:alloc msg failed.");
        return VOS_TRUE;
    }

    /* 需找可用于拨号的CID */
    ucCid = TAF_APS_FindCidForDial(WUEPS_PID_AT);
    if ( TAF_INVALID_CID == ucCid )
    {
        pCnf->ulRet       = VOS_ERR;
    }

    pCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pCnf->enMsgId                       = ID_TAFAGENT_APS_FIND_CID_FOR_DIAL_CNF;
    pCnf->ulRet                         = VOS_OK;
    pCnf->ucCid                         = ucCid;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pCnf))
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:send msg failed.");

        return VOS_TRUE;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvAtGetCidParaReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAFAGENT_APS_GET_CID_PARA_CNF_STRU         *pCnf;
    TAFAGENT_APS_GET_CID_PARA_REQ_STRU         *pMsgReq;
    VOS_UINT32                                  ulResult;

    pCnf = (TAFAGENT_APS_GET_CID_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                              sizeof(TAFAGENT_APS_GET_CID_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pCnf)
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:alloc msg failed.");
        return VOS_TRUE;
    }

    pMsgReq                             = (TAFAGENT_APS_GET_CID_PARA_REQ_STRU*)pstMsg;

    ulResult                            = MN_APS_GetPdpCidPara(&(pCnf->stCidInfo),pMsgReq->ucCid);

    pCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pCnf->enMsgId                       = ID_TAFAGENT_APS_GET_CID_PARA_CNF;
    pCnf->ulRet                         = (VOS_UINT8)ulResult;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pCnf))
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:send msg failed.");

        return VOS_TRUE;
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtSetCidParaReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAFAGENT_APS_SET_CID_PARA_REQ_STRU                         *pMsgReq;
    TAFAGENT_APS_SET_CID_PARA_CNF_STRU                         *pCnf;
    VOS_UINT32                                                  ulResult;

    pCnf = (TAFAGENT_APS_SET_CID_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                sizeof(TAFAGENT_APS_SET_CID_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pCnf)
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:alloc msg failed.");
        return VOS_TRUE;
    }

    pMsgReq                  = (TAFAGENT_APS_SET_CID_PARA_REQ_STRU*)(pstMsg);
    ulResult                 = MN_APS_SetPdpCidPara((VOS_UINT16)pMsgReq->ulClientID,&(pMsgReq->stPdpPrimContextExt));

    pCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pCnf->enMsgId                       = ID_TAFAGENT_APS_SET_CID_PARA_CNF;
    pCnf->ulRet                         = (VOS_UINT8)ulResult;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pCnf))
    {
        APS_ERR_LOG("TAF_APS_RcvFindCidForDialReq_PreProc:send msg failed.");

        return VOS_TRUE;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtGetCidQosParaReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAFAGENT_APS_GET_CID_QOS_PARA_CNF_STRU         *pCnf;
    TAFAGENT_APS_GET_CID_QOS_PARA_REQ_STRU         *pMsgReq;
    VOS_UINT32                                      ulResult;

    pCnf = (TAFAGENT_APS_GET_CID_QOS_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                              sizeof(TAFAGENT_APS_GET_CID_QOS_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pCnf)
    {
        APS_ERR_LOG("TAF_APS_GetCidQosParaReq_PreProc:alloc msg failed.");
        return VOS_TRUE;
    }

    pMsgReq                             = (TAFAGENT_APS_GET_CID_QOS_PARA_REQ_STRU*)pstMsg;

    ulResult                            = TAF_APS_GetPdpCidQosPara(&(pCnf->stQosPara), pMsgReq->ucCid);

    pCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pCnf->enMsgId                       = ID_TAFAGENT_APS_GET_CID_QOS_PARA_CNF;
    pCnf->ulRet                         = (VOS_UINT8)ulResult;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pCnf))
    {
        APS_ERR_LOG("TAF_APS_RcvAtGetCidQosParaReq_PreProc:send msg failed.");

        return VOS_TRUE;
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtSetCidQosParaReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAFAGENT_APS_SET_CID_QOS_PARA_REQ_STRU                         *pMsgReq;
    TAFAGENT_APS_SET_CID_QOS_PARA_CNF_STRU                         *pCnf;
    VOS_UINT32                                                      ulResult;

    pCnf = (TAFAGENT_APS_SET_CID_QOS_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_TAF,
                                                sizeof(TAFAGENT_APS_SET_CID_QOS_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pCnf)
    {
        APS_ERR_LOG("TAF_APS_RcvAtSetCidQosParaReq_PreProc:alloc msg failed.");
        return VOS_TRUE;
    }

    pMsgReq                  = (TAFAGENT_APS_SET_CID_QOS_PARA_REQ_STRU*)(pstMsg);

    ulResult                 = TAF_APS_SetPdpCidQosPara(&(pMsgReq->stQosPara));

    pCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pCnf->ulSenderPid                   = WUEPS_PID_TAF;
    pCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;
    pCnf->enMsgId                       = ID_TAFAGENT_APS_SET_CID_QOS_PARA_CNF;
    pCnf->ulRet                         = (VOS_UINT8)ulResult;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pCnf))
    {
        APS_ERR_LOG("TAF_APS_RcvAtSetCidQosParaReq_PreProc:send msg failed.");

        return VOS_TRUE;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpStateReq;
    VOS_UINT8                           ucCidValue;
    VOS_UINT32                          ulBearerActivateFlg;
    VOS_UINT32                          ulRslt;

    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstSetPdpStateReq                   = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstSetPdpStateReq->stCtrl)))
    {
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl), TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }
#endif

    /* 设置激活标记 */
    ulBearerActivateFlg = (TAF_CGACT_ACT == pstSetPdpStateReq->stCidListStateInfo.ucState) ? VOS_TRUE : VOS_FALSE;

    /*
       当前先不支持一次性激活多个PDP的场景，一次激活一个，因此找第一个要激活的
       CID
    */
    ucCidValue = TAF_APS_GetCidValue(ulBearerActivateFlg, pstSetPdpStateReq->stCidListStateInfo.aucCid);

    /* 如果返回的CID为0，表明是无效CID，直接返回ERROR */
    if (0 == ucCidValue)
    {
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
        return VOS_TRUE;
    }

    if (ulBearerActivateFlg == VOS_TRUE)
    {
        ulRslt = TAF_APS_ProcActivatePdpContext_PreProc(ucCidValue, pstSetPdpStateReq);
    }
    else
    {
        ulRslt = TAF_APS_ProcDeactivatePdpContext_PreProc(ucCidValue, pstSetPdpStateReq);
    }

    return ulRslt;
}


VOS_UINT32 TAF_APS_ProcActivatePdpContext_PreProc(
    VOS_UINT8                           ucCidValue,
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpStateReq
)
{
    TAF_APS_MATCH_PARAM_STRU            stMatchParam;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucPdpId;
    TAF_PH_MODE                         ucPhMode;

    PS_MEM_SET(&stMatchParam, 0x00, sizeof(TAF_APS_MATCH_PARAM_STRU));

    ucPhMode = MMA_GetCurrPhMode();

    /* 如果系统为开机,则直接返回VOS_TRUE */
    if ( TAF_PH_MODE_FULL != ucPhMode )
    {
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                    TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }

    /* CID对应的上下文没有定义，则直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetTafCidInfoUsedFlg(ucCidValue))
    {
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                    TAF_PS_CAUSE_CID_INVALID);
        return VOS_TRUE;
    }

    /* 生成匹配参数信息 */
    TAF_APS_GenMatchParamByCidInfo(TAF_APS_GetTafCidInfoAddr(ucCidValue),
                                   &stMatchParam);

#if (FEATURE_ON == FEATURE_LTE)
    /* 找到默认承载的情况下，直接刷新CID  */
    if (VOS_TRUE == TAF_APS_MatchCallWithAttachBearer(
                                &pstSetPdpStateReq->stCtrl,
                                &stMatchParam))
    {
        /* 除了刷新CID，还同时刷新了ClientId和OP ID */
        ucPdpId = TAF_APS_GetPdpIdByDefaultCid();

        /* 清除cid 0 的信息,记录PDP 信息 */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    ucCidValue,
                                    pstSetPdpStateReq->stCtrl.ulModuleId,
                                    pstSetPdpStateReq->stCtrl.usClientId,
                                    pstSetPdpStateReq->stCtrl.ucOpId);

        TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCidValue, TAF_APS_GetTafCidInfoPdpType(ucCidValue));

        /* 通知ESM当前的Cid刷新 */
        MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                                SM_ESM_PDP_OPT_MODIFY);

        return VOS_FALSE;
    }
#endif

    /* 获取PdpId，如果获取不到，说明当前是激活一个新的CID */
    ucPdpId = TAF_APS_GetPdpIdByCid(ucCidValue);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 相同APN
           如果已激活相同APN，PDP类型相容，共用PDP实体
           如果已激活相同APN，PDP类型不同，重新分配PDP实体
           未激活: ACTIVATING,MODIFING,DEACTIVATING
           返回特殊原因值TAF_PS_CAUSE_SAME_APN_OPERATING

           无相同APN，执行原有流程
        */

        /* 激活则分配一个新的PDP ID */
        if (VOS_TRUE == TAF_APS_MatchCallWithAllBearer(
                                    &pstSetPdpStateReq->stCtrl,
                                    &stMatchParam,
                                    &ucPdpId))
        {
            /* 未激活: ACTIVATING,MODIFING,DEACTIVATING
               返回特殊原因值TAF_PS_CAUSE_SAME_APN_OPERATING */
            if (TAF_APS_STA_ACTIVE != TAF_APS_GetPdpIdMainFsmState(ucPdpId))
            {
                TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                             TAF_PS_CAUSE_OPERATION_CONFLICT);
                return VOS_TRUE;
            }
            else
            {
                /* 记录PDP信息 */
                TAF_APS_AddPdpEntClientInfo(ucPdpId,
                                            ucCidValue,
                                            &(pstSetPdpStateReq->stCtrl));

                TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                             ucCidValue,
                                             TAF_APS_GetTafCidInfoPdpType(ucCidValue));
            }
        }
        else
        {
            /* 分配TI(PdP Id) */
            if (TAF_APS_ALLOC_PDPID_FAIL == TAF_APS_AllocPdpId(&pstSetPdpStateReq->stCtrl,
                                                               ucCidValue,
                                                               &ucPdpId))
            {
                /* 分配不到实体，则直接返回ERROR */
                TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                            TAF_PS_CAUSE_UNKNOWN);
                return VOS_TRUE;
            }
        }
    }
    else
    {
        if (TAF_APS_INVALID_CLIENTID == TAF_APS_GetPdpEntClientId(ucPdpId, ucCidValue))
        {
            TAF_APS_SetPdpEntClientId(ucPdpId, ucCidValue, pstSetPdpStateReq->stCtrl.usClientId);
        }
        else if (TAF_APS_GetPdpEntClientId(ucPdpId, ucCidValue) != pstSetPdpStateReq->stCtrl.usClientId)
        {
            /* ClientId不匹配, 则直接返回ERROR */
            TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                        TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }
        else
        {
            ;
        }
    }

    ulRslt = TAF_APS_RcvAtSetPdpContextStateReq_PdpActLimit_PreProc(pstSetPdpStateReq);

    return ulRslt;
}


VOS_UINT32 TAF_APS_ProcDeactivatePdpContext_PreProc(
    VOS_UINT8                           ucCidValue,
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpStateReq
)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_BITCID_INFO_STRU            stCid;

    PS_MEM_SET(&stCid, 0, sizeof(TAF_APS_BITCID_INFO_STRU));

    /* 获取PdpId，如果获取不到，说明当前是去激活一个新的CID */
    ucPdpId = TAF_APS_GetPdpIdByCid(ucCidValue);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /*
           去激活操作，如果是去激活一个新的CID,说明当前对应的CID还没有激活
           或者尝试激活，直接返回OK
        */
        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                    TAF_PS_CAUSE_SUCCESS);

        /* 没有找到匹配的PDP, 则此PDP没有激活, 上报去激活成功事件 */
        TAF_APS_SndCidInactiveEvt(&(pstSetPdpStateReq->stCtrl),
                                  ucCidValue);

        return VOS_TRUE;
    }
    else
    {
        if (TAF_APS_INVALID_CLIENTID == TAF_APS_GetPdpEntClientId(ucPdpId, ucCidValue))
        {
            TAF_APS_SetPdpEntClientId(ucPdpId, ucCidValue, pstSetPdpStateReq->stCtrl.usClientId);
        }
        else if (TAF_APS_GetPdpEntClientId(ucPdpId, ucCidValue) != pstSetPdpStateReq->stCtrl.usClientId)
        {
            /* ClientId不匹配, 则直接返回ERROR */
            TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                         TAF_PS_CAUSE_UNKNOWN);

            return VOS_TRUE;
        }
        else
        {
            /* 检查PDP实体中是否存在多个cid */
            TAF_APS_GetPdpEntBitCidInfo(ucPdpId, &stCid);

            if (stCid.ulNum > 1)
            {
                /* 返回该cid去激活成功 */
                TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                             TAF_PS_CAUSE_SUCCESS);

                TAF_APS_SndPdpDeActivateCnf(ucPdpId, ucCidValue);

                /* 清除PDP实体中该CID信息 */
                TAF_APS_RmvPdpEntClientInfo(ucPdpId, ucCidValue, &stCid);

                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCidValue;
    VOS_UINT32                          ulBearerActivateFlg;
    TAF_APS_BITCID_INFO_STRU                     stCid;

    PS_MEM_SET(&stCid, 0, sizeof(TAF_APS_BITCID_INFO_STRU));

    /* 当前先不支持一次性Modify多个PDP的场景，一次Modify一个 */
    ulBearerActivateFlg                 = VOS_FALSE;
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq                    = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    ucCidValue = TAF_APS_GetCidValue(ulBearerActivateFlg, pstCallModifyReq->stCidListInfo.aucCid);

    /* 如果返回的CID为0，表明是无效CID，直接返回ERROR */
    if ( 0 == ucCidValue )
    {
        /* 上报ERROR */
        TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);
        return VOS_TRUE;

    }

    /* 获取PdpId，如果获取不到，说明当前没有可以修改的PDP ID */
    ucPdpId = TAF_APS_GetPdpIdByCid(ucCidValue);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 直接返回ERROR */
        TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);
        return VOS_TRUE;
    }

    /* 检查PDP实体中是否存在多个cid */
    TAF_APS_GetPdpEntBitCidInfo(ucPdpId, &stCid);
    if (stCid.ulNum > 1)
    {
        /* 存在APN共用时，不允许用户主动MODIFY */
        TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl),
                                 TAF_PS_CAUSE_OPERATION_CONFLICT);
        return VOS_TRUE;
    }

    TAF_APS_SetPdpEntModDialInfo(ucPdpId, &(pstCallModifyReq->stCtrl));

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;
    TAF_APS_MATCH_PARAM_STRU            stMatchParam;
    VOS_UINT8                           ucPdpId;
    TAF_PH_MODE                         ucPhMode;

    PS_MEM_SET(&stMatchParam, 0x00, sizeof(TAF_APS_MATCH_PARAM_STRU));

    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq                      = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);
    ucPhMode                            = MMA_GetCurrPhMode();
    ucPdpId                             = TAF_APS_INVALID_PDPID;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    /* L-C互操作模式，对请求消息有效性做判断 */
    if (VOS_FALSE == TAF_APS_LCCheckReqValid(&(pstCallOrigReq->stCtrl)))
    {
        TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                      pstCallOrigReq->stDialParaInfo.ucCid,
                      TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }
#endif

    /* 如果系统为开机，则直接返回VOS_TRUE */
    if (TAF_PH_MODE_FULL != ucPhMode)
    {
        TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                              pstCallOrigReq->stDialParaInfo.ucCid,
                              TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }

    /* 生成匹配参数信息 */
    TAF_APS_GenMatchParamByDailInfo(&pstCallOrigReq->stDialParaInfo,
                                    &stMatchParam);

#if (FEATURE_ON == FEATURE_LTE)
    /* 找到默认承载的情况下，直接刷新CID  */
    if (VOS_TRUE == TAF_APS_MatchCallWithAttachBearer(
                                &pstCallOrigReq->stCtrl,
                                &stMatchParam))
    {
        /* 除了刷新CID，还同时刷新了ClientId和OP ID */
        ucPdpId = TAF_APS_GetPdpIdByDefaultCid();

        /* 清除CID 0 的信息,记录PDP 信息 */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    pstCallOrigReq->stDialParaInfo.ucCid,
                                    pstCallOrigReq->stCtrl.ulModuleId,
                                    pstCallOrigReq->stCtrl.usClientId,
                                    pstCallOrigReq->stCtrl.ucOpId);

        TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                     pstCallOrigReq->stDialParaInfo.ucCid,
                                     pstCallOrigReq->stDialParaInfo.enPdpType);

        /* 通知ESM当前的Cid刷新 */
        MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                                SM_ESM_PDP_OPT_MODIFY);

        return VOS_FALSE;
    }
#endif

    /* 获取PdpId，如果获取不到，说明当前是激活或者去激活一个新的CID */
    ucPdpId = TAF_APS_GetPdpIdByCid(pstCallOrigReq->stDialParaInfo.ucCid);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 相同APN
             如果已激活相同APN，PDP类型相容，共用PDP实体
             如果已激活相同APN，PDP类型不同，重新分配PDP实体
             未激活: ACTIVATING,MODIFING,DEACTIVATING
             返回特殊原因值TAF_PS_CAUSE_SAME_APN_OPERATING

           无相同APN，执行原有流程  */
        if (VOS_TRUE == TAF_APS_MatchCallWithAllBearer(
                                    &pstCallOrigReq->stCtrl,
                                    &stMatchParam,
                                    &ucPdpId))
        {
            /* 未激活: ACTIVATING,MODIFING,DEACTIVATING
               返回特殊原因值TAF_PS_CAUSE_SAME_APN_OPERATING */
            if (TAF_APS_STA_ACTIVE != TAF_APS_GetPdpIdMainFsmState(ucPdpId))
            {
                TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                                       pstCallOrigReq->stDialParaInfo.ucCid,
                                       TAF_PS_CAUSE_OPERATION_CONFLICT);
                return VOS_TRUE;
            }
            else
            {
                /* 记录PDP信息 */
                TAF_APS_AddPdpEntClientInfo(ucPdpId,
                                            pstCallOrigReq->stDialParaInfo.ucCid,
                                            &(pstCallOrigReq->stCtrl));

                TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                             pstCallOrigReq->stDialParaInfo.ucCid,
                                             pstCallOrigReq->stDialParaInfo.enPdpType);
            }
        }
        else
        {
            /* 分配实体 */
            if (TAF_APS_ALLOC_PDPID_FAIL == TAF_APS_AllocPdpId(&pstCallOrigReq->stCtrl,
                                                               pstCallOrigReq->stDialParaInfo.ucCid,
                                                               &ucPdpId))
            {
                /* 分配不到实体，则直接返回ERROR */
                TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                                      pstCallOrigReq->stDialParaInfo.ucCid,
                                      TAF_PS_CAUSE_UNKNOWN);
                return VOS_TRUE;
            }
        }
    }
    else
    {
        if (TAF_APS_GetPdpEntClientId(ucPdpId, pstCallOrigReq->stDialParaInfo.ucCid) != pstCallOrigReq->stCtrl.usClientId)
        {
            /* ClientId不匹配, 则直接返回ERROR */
            TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                                  pstCallOrigReq->stDialParaInfo.ucCid,
                                  TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }
    }

    if (VOS_TRUE == TAF_APS_IsPdpActLimit_PreProc())
    {
        TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                              pstCallOrigReq->stDialParaInfo.ucCid,
                              TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_IsStaNwActivatingSubStaWaitAppAnswer(
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq
)
{
    VOS_UINT8                           ucPdpId;

    /* 根据CID获取PDP ID */
    ucPdpId = TAF_APS_GetPdpIdByCid(pstPppDialOrigReq->stPppDialParaInfo.ucCid);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 如果获取不到，说明不是应答网络激活 */
        return VOS_FALSE;
    }

    /* 找到PDP ID后获取对应的状态机，如果不是网络激活等待用户应答状态，说明不是应答网络激活 */
    if (TAF_APS_STA_NW_ACTIVATING != TAF_APS_GetPdpIdMainFsmState(ucPdpId))
    {
        return VOS_FALSE;
    }

    if (TAF_APS_NW_ACTIVATING_SUBSTA_WAIT_APP_ANSWER != TAF_APS_GetPdpIdSubFsmState(ucPdpId))
    {
        return VOS_FALSE;
    }

    /* 判断端口是否一致 */
    if (pstPppDialOrigReq->stCtrl.usClientId != TAF_APS_GetPdpEntClientId(ucPdpId, pstPppDialOrigReq->stPppDialParaInfo.ucCid))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDialOrigReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg = VOS_NULL_PTR;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq = VOS_NULL_PTR;
    VOS_UINT8                           ucPdpId;
    TAF_PH_MODE                         ucPhMode;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulErrCode;
#endif

    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq                   = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);
    ucPhMode                            = MMA_GetCurrPhMode();

    /* 判断当前处于网络激活等待用户应答状态，直接进入状态机 */
    if (VOS_TRUE == TAF_APS_IsStaNwActivatingSubStaWaitAppAnswer(pstPppDialOrigReq))
    {
        return VOS_FALSE;
    }

    /* 如果系统未开机或如果CID对应的上下文没有定义，则直接返回ERROR */
    if ( (TAF_PH_MODE_FULL != ucPhMode)
      || (VOS_FALSE == TAF_APS_GetTafCidInfoUsedFlg(pstPppDialOrigReq->stPppDialParaInfo.ucCid)) )
    {
        TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                 TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }

#if (FEATURE_ON == FEATURE_LTE)

    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        ulErrCode = TAF_ERR_NO_ERROR;

        ulResult = TAF_APS_FindSutiablePdpForPppDial_LteMode(&pstPppDialOrigReq->stPppDialParaInfo,
                                                             &ulErrCode);
        if (TAF_ERR_ERROR == ulErrCode)
        {
            TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                     TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }

        /* 找到默认承载的情况下，直接刷新CID */
        if (VOS_TRUE == ulResult)
        {
            /* 除了刷新CID，还同时刷新了ClientId和OP ID */
            ucPdpId = TAF_APS_GetPdpIdByDefaultCid();

            /* 清除cid 0 的信息,记录PDP 信息 */
            TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                        pstPppDialOrigReq->stPppDialParaInfo.ucCid,
                                        pstPppDialOrigReq->stCtrl.ulModuleId,
                                        pstPppDialOrigReq->stCtrl.usClientId,
                                        pstPppDialOrigReq->stCtrl.ucOpId);

            TAF_APS_SetPdpEntDialPdpType(ucPdpId,
                                         pstPppDialOrigReq->stPppDialParaInfo.ucCid,
                                         TAF_APS_GetTafCidInfoPdpType(pstPppDialOrigReq->stPppDialParaInfo.ucCid));

            /* 通知ESM当前的Cid刷新 */
            MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                                    SM_ESM_PDP_OPT_MODIFY);

            return VOS_FALSE;
        }
    }

#endif

    /* 获取PdpId，如果获取不到，说明当前是激活或者去激活一个新的CID */
    ucPdpId = TAF_APS_GetPdpIdByCid(pstPppDialOrigReq->stPppDialParaInfo.ucCid);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 分配实体 */
        if (TAF_APS_ALLOC_PDPID_FAIL == TAF_APS_AllocPdpId(&pstPppDialOrigReq->stCtrl,
                                                           pstPppDialOrigReq->stPppDialParaInfo.ucCid,
                                                           &ucPdpId))
        {
            /* 分配不到实体，则直接返回ERROR */
            TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                     TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }
    }
    else
    {
        if (TAF_APS_GetPdpEntClientId(ucPdpId, pstPppDialOrigReq->stPppDialParaInfo.ucCid) != pstPppDialOrigReq->stCtrl.usClientId)
        {
            /* ClientId不匹配, 则直接返回ERROR */
            TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                     TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }
    }

    if (VOS_TRUE == TAF_APS_IsPdpActLimit_PreProc())
    {
        TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                                 TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;
    VOS_UINT8                           ucPdpId;
    TAF_APS_BITCID_INFO_STRU            stCid;

    PS_MEM_SET(&stCid, 0, sizeof(TAF_APS_BITCID_INFO_STRU));
    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq                       = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取PdpId，如果获取不到，直接返回，不进状态机 */
    ucPdpId = TAF_APS_GetPdpIdByCid(pstCallEndReq->ucCid);

    if (TAF_APS_INVALID_PDPID == ucPdpId)
    {
        /* 该场景下返回ERROR, 防止使用者挂死 */
        TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                              pstCallEndReq->ucCid,
                              TAF_PS_CAUSE_CID_INVALID);
        return VOS_TRUE;

    }
    else
    {
        if (TAF_APS_GetPdpEntClientId(ucPdpId, pstCallEndReq->ucCid) != pstCallEndReq->stCtrl.usClientId)
        {
            TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                                  pstCallEndReq->ucCid,
                                  TAF_PS_CAUSE_CID_INVALID);

            return VOS_TRUE;
        }
        else
        {
            /* 更新用户信息--IMSA会使用不同的opid */
            TAF_APS_SetPdpEntOpId(ucPdpId,
                                  pstCallEndReq->ucCid,
                                  pstCallEndReq->stCtrl.ucOpId);

            /* 检查PDP实体中是否存在多个cid */
            TAF_APS_GetPdpEntBitCidInfo(ucPdpId, &stCid);

            if (stCid.ulNum > 1)
            {
                /* 返回该cid去激活成功 */
                TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                                      pstCallEndReq->ucCid,
                                      TAF_PS_CAUSE_SUCCESS);

                TAF_APS_SndPdpDeActivateCnf(ucPdpId, pstCallEndReq->ucCid);

                /* 清除PDP实体中该CID信息 */
                TAF_APS_RmvPdpEntClientInfo(ucPdpId, pstCallEndReq->ucCid, &stCid);

                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;

}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;
    TAF_PRI_PDP_QUERY_INFO_STRU         stPdpPriPara;
    VOS_UINT32                          ulErrCode;
    TAF_APS_PDP_ENTITY_FSM_CTX_STRU    *pstNwActFsm;
    VOS_UINT8                           ucPdpId;

    VOS_UINT8                           ucCid;

    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq                    = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);
    ucCid                               = pstCallAnswerReq->stAnsInfo.ucCid;

    pstNwActFsm = TAF_APS_GetPdpEntityFsm(TAF_APS_STA_NW_ACTIVATING,
                                          TAF_APS_FSM_NW_ACTIVATING,
                                          TAF_APS_NW_ACTIVATING_SUBSTA_WAIT_APP_ANSWER);

    /* 判断当前是否处于网络激活等待用户应答状态 */
    if (VOS_NULL_PTR == pstNwActFsm)
    {
        TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                 pstCallAnswerReq->stAnsInfo.ucCid,
                                 TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }

    ucPdpId = pstNwActFsm->ucPdpId;

    /* 不支持IPV4/IPV4V6类型，直接回ERROR */
#if (FEATURE_ON == FEATURE_IPV6)
    if ((MN_APS_ADDR_IPV6 == g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum)
     || (MN_APS_ADDR_IPV4V6 == g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum))
    {
        TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                 pstCallAnswerReq->stAnsInfo.ucCid,
                                 TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }
#endif

    /* 用户指定了CID，需要匹配PDP TYPE和ADDR。否则使用网络指定的PDP TYPE和ADDR */
    if (0 != ucCid)
    {
        /* 获取Primary PDP上下文信息 */
        PS_MEM_SET(&stPdpPriPara, 0x00, sizeof(TAF_PRI_PDP_QUERY_INFO_STRU));
        Aps_QuePri(pstCallAnswerReq->stAnsInfo.ucCid, &stPdpPriPara, &ulErrCode);
        if (TAF_PARA_OK != ulErrCode)
        {
            TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                     pstCallAnswerReq->stAnsInfo.ucCid,
                                     TAF_PS_CAUSE_UNKNOWN);
            return VOS_TRUE;
        }
        else
        {
            /* 用户设置的类型和网络下发的类型不一致 */
            if (TAF_APS_ConvertPdpType(g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum) != stPdpPriPara.stPriPdpInfo.stPdpAddr.enPdpType)
            {
                TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                         pstCallAnswerReq->stAnsInfo.ucCid,
                                         TAF_PS_CAUSE_UNKNOWN);
                return VOS_TRUE;
            }

            /* 网络发起的PPP类型的拨号，需要进行匹配拨号类型 */
            /* 网络发起的IPV4类型的拨号，需要进行匹配拨号类型和IP地址 */
            if (APS_ADDR_PPP != g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum)
            {
                /* 用户设置了PDP ADDR */
                if ('\0' != stPdpPriPara.stPriPdpInfo.stPdpAddr.aucIpv4Addr[0])
                {
                    if (0 != PS_MEM_CMP(stPdpPriPara.stPriPdpInfo.stPdpAddr.aucIpv4Addr,
                                        g_PdpEntity[ucPdpId].PdpAddr.aucIpAddr,
                                        TAF_IPV4_ADDR_LEN))
                    {
                        TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                                 pstCallAnswerReq->stAnsInfo.ucCid,
                                                 TAF_PS_CAUSE_UNKNOWN);
                        return VOS_TRUE;
                    }
                }
            }
        }
    }

    ucCid = ((0 != ucCid) ? ucCid : TAF_APS_GetPdpEntCurrCid(ucPdpId));

    /* 记录PDP 信息 */
    TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                ucCid,
                                pstCallAnswerReq->stCtrl.ulModuleId,
                                pstCallAnswerReq->stCtrl.usClientId,
                                pstCallAnswerReq->stCtrl.ucOpId);

    TAF_APS_SetPdpEntDialPdpType(ucPdpId, ucCid, TAF_APS_ConvertPdpType(g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum));

    /* 如果AT^ANSWER发起的IP类型的网络激活响应，直接上报CONNECT给AT，等待PPP发起拨号请求 */
    if (VOS_TRUE == pstCallAnswerReq->stAnsInfo.ucAnsExtFlg)
    {
        if (TAF_PDP_IPV4 == TAF_APS_ConvertPdpType(g_PdpEntity[ucPdpId].PdpAddr.ucPdpTypeNum))
        {
            /* 前面操作成功，记录下应答的客户端和呼叫标识和Cid */
            g_PdpEntity[ucPdpId].PdpProcTrack.ucHowToInitAct
                                                = APS_APP_MANUL_ACT;

            TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                                     TAF_APS_GetPdpEntCurrCid(ucPdpId),
                                     TAF_ERR_AT_CONNECT);

            return VOS_TRUE;
        }
    }


    /* 预处理结束，需要进入状态机 */
    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_PDP_ENTITY_FSM_CTX_STRU    *pstNwActFsm;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    pstAppMsg                           = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq                    = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    pstNwActFsm = TAF_APS_GetPdpEntityFsm(TAF_APS_STA_NW_ACTIVATING,
                                          TAF_APS_FSM_NW_ACTIVATING,
                                          TAF_APS_NW_ACTIVATING_SUBSTA_WAIT_APP_ANSWER);

    /* 判断当前是否处于网络激活等待用户应答状态 */
    if (VOS_NULL_PTR == pstNwActFsm)
    {
        TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_PS_CAUSE_UNKNOWN);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvSmRegPdpActCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_ACTIVATE_CNF_STRU        *pstSmMsg;
    SM_TAF_TRANS_ID_STRU                stTi;
    TAF_APS_PDP_ACT_LIMIT_INFO_STRU    *pstPdpActLimitInfo;


    PS_MEM_SET(&stTi, 0x00, sizeof(stTi));


    pstSmMsg = (SMREG_PDP_ACTIVATE_CNF_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    pstPdpActLimitInfo = TAF_APS_GetPdpActLimitInfoAddr();

    if (VOS_TRUE == pstPdpActLimitInfo->ucPdpActLimitFlg)
    {
        pstPdpActLimitInfo->ucPdpActFailCount = 0;

        if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_PDP_ACT_LIMIT_PDP_ID))
        {
            TAF_APS_StopTimer(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_PDP_ACT_LIMIT_PDP_ID);
        }
    }


    /* mo场景，直接在此处刷新实体中Ti的值 */
    stTi.ucTiValue = pstSmMsg->stTransId.ucTiValue;
    stTi.ucTiFlag  = pstSmMsg->stTransId.ucTiFlag;

    TAF_APS_SetPdpEntityTi(pstSmMsg->ucConnectId, &stTi);

    return VOS_FALSE;

}
VOS_UINT32 TAF_APS_RcvSmRegPdpActRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_ACTIVATE_REJ_STRU        *pstSmMsg;

    pstSmMsg = (SMREG_PDP_ACTIVATE_REJ_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    /* PDP激活受限处理:PDP激活的次数为SM实际尝试的PDP激活次数 */
    if (SM_TAF_CAUSE_SM_MAX_TIME_OUT == pstSmMsg->enCause)
    {
        TAF_APS_ProcPdpActLimit_PreProc(pstSmMsg->ucExpiredCount);
    }
    else
    {
        TAF_APS_ProcPdpActLimit_PreProc(pstSmMsg->ucExpiredCount + 1);
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_APS_RcvSmRegPdpActInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_ACTIVATE_IND_STRU        *pstSmMsg;
    APS_PDP_CONTEXT_ENTITY_ST          *pstCurrEntity;
    SM_TAF_TRANS_ID_STRU                stTi;


    PS_MEM_SET(&stTi, 0x00, sizeof(stTi));


    pstSmMsg = (SMREG_PDP_ACTIVATE_IND_STRU *)pstMsg;


    /* 网络发起的PDP激活，在预处理处分配PDP ID,这里的CR 等同于PDP ID */
    pstCurrEntity = TAF_APS_GetPdpEntInfoAddr(pstSmMsg->ucConnectId);
    pstCurrEntity->stClientInfo.ucCid   = TAF_APS_FindCidForAns();
    pstCurrEntity->ulUsedFlg            = VOS_TRUE;
    pstCurrEntity->ucPdpId              = pstSmMsg->ucConnectId;

    /* 刷新PDP信息 */
    TAF_APS_SetPdpEntClientInfo(pstCurrEntity->ucPdpId,
                                pstCurrEntity->stClientInfo.ucCid,
                                TAF_APS_INVALID_MODULEID,
                                TAF_APS_INVALID_CLIENTID,
                                TAF_APS_INVALID_OPID);

    /* mt场景，直接在此处刷新实体中Ti的值 */
    stTi.ucTiValue = pstSmMsg->stTransId.ucTiValue;
    stTi.ucTiFlag  = pstSmMsg->stTransId.ucTiFlag;

    TAF_APS_SetPdpEntityTi(pstSmMsg->ucConnectId, &stTi);

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvSmRegPdpModifyInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_MODIFY_IND_STRU          *pstSmMsg;

    pstSmMsg = (SMREG_PDP_MODIFY_IND_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    return VOS_FALSE;


}
VOS_UINT32 TAF_APS_RcvSmRegPdpModifyCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_MODIFY_CNF_STRU          *pstSmMsg;

    pstSmMsg = (SMREG_PDP_MODIFY_CNF_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    return VOS_FALSE;

}
VOS_UINT32 TAF_APS_RcvSmRegPdpModifyRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_MODIFY_REJ_STRU          *pstSmMsg;

    pstSmMsg = (SMREG_PDP_MODIFY_REJ_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    return VOS_FALSE;


}
VOS_UINT32 TAF_APS_RcvSmRegPdpDeactivInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_DEACTIVATE_IND_STRU      *pstSmMsg;

    pstSmMsg = (SMREG_PDP_DEACTIVATE_IND_STRU *)pstMsg;

    /* 根据TI获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    /* PDP激活受限处理: 网侧主动发起的PDP去激活，认为发起PDP激活的次数为1 */
    TAF_APS_ProcPdpActLimit_PreProc(1);

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvSmRegPdpDeactivCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_DEACTIVATE_CNF_STRU      *pstSmMsg;

    pstSmMsg = (SMREG_PDP_DEACTIVATE_CNF_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    return VOS_FALSE;


}
VOS_UINT32 TAF_APS_RcvSmRegPdpActSecCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_ACTIVATE_SEC_CNF_STRU    *pstSmMsg;
    SM_TAF_TRANS_ID_STRU                stTi;


    PS_MEM_SET(&stTi, 0x00, sizeof(stTi));


    pstSmMsg = (SMREG_PDP_ACTIVATE_SEC_CNF_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    /* mo场景，直接在此处刷新实体中Ti的值 */
    stTi.ucTiValue = pstSmMsg->stTransId.ucTiValue;
    stTi.ucTiFlag  = pstSmMsg->stTransId.ucTiFlag;

    TAF_APS_SetPdpEntityTi(pstSmMsg->ucConnectId, &stTi);

    return VOS_FALSE;


}
VOS_UINT32 TAF_APS_RcvSmRegPdpActSecRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    SMREG_PDP_ACTIVATE_SEC_REJ_STRU    *pstSmMsg;

    pstSmMsg = (SMREG_PDP_ACTIVATE_SEC_REJ_STRU *)pstMsg;

    /* 根据CR获取不到相应的PDP ID，则打印异常，直接返回，不进状态机 */
    if (VOS_TRUE != TAF_APS_IsPdpIdValid(pstSmMsg->ucConnectId))
    {
        TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvSmRegPdpActCnf_PreProc:Wrong ConnectId", pstSmMsg->ucConnectId);
        return VOS_TRUE;
    }

    return VOS_FALSE;


}

#if ( FEATURE_ON == FEATURE_LTE )
VOS_UINT32 TAF_APS_RcvAtSetEpsQosInfoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstPsMsg;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(TAF_APS_GET_MODULEID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent),
                                           TAF_APS_GET_CLIENTID_FORM_CTRL_HDR((TAF_CTRL_STRU *)pstPsMsg->aucContent));

    /* 发送消息给L4A */
    TAF_APS_SndL4aSetCgeqosReq(pstPsMsg->aucContent);

    /* 启动L4A消息回复保护定时器 */
    TAF_APS_StartTimer(TI_TAF_APS_WAIT_SET_CGEQOS_CNF,
                       TI_TAF_APS_WAIT_L4A_CNF_LEN,
                       ulPara);

    /* 添加消息进等待队列 */
    TAF_APS_SaveItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOS_CNF,
                                     (VOS_UINT32 *)pstPsMsg->aucContent,
                                     sizeof(TAF_PS_SET_EPS_QOS_INFO_REQ_STRU),
                                     ulPara);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aSetCgdcontCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGDCONT_CNF_STRU               *pstPsMsg;
    TAF_CTRL_STRU                               stCtrl = {0};
    VOS_UINT32                                  ulErrCode;
    TAF_APS_CMD_BUFFER_STRU                    *pstMsgInfo;
    TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ_STRU  *pstSetPdpCtxInfoReq;
    VOS_UINT32                                  ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_CGDCONT_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetPdpCtxInfoReq     = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGDCONT_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGDCONT_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDCONT_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgdcontCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }
        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetPdpCtxInfoReq = (TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;
        stCtrl = pstSetPdpCtxInfoReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {

            /* PDP激活受限特性打开，暂停激活定时器正在运行 */
            if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_PDP_ACT_LIMIT_PDP_ID))
            {
                TAF_APS_RcvSetPrimPdpCtxInfoReq_PdpActLimit(pstSetPdpCtxInfoReq);
            }


            /* 设置Primary PDP上下文 */
            Aps_DefPsPdpContext(pstSetPdpCtxInfoReq->stCtrl.usClientId,
                                pstSetPdpCtxInfoReq->stCtrl.ucOpId,
                                &(pstSetPdpCtxInfoReq->stPdpContextInfo),
                                &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDCONT_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetPrimPdpCtxInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aSetCgdscontCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGDSCONT_CNF_STRU              *pstPsMsg;
    TAF_CTRL_STRU                               stCtrl = {0};
    VOS_UINT32                                  ulErrCode;
    TAF_APS_CMD_BUFFER_STRU                    *pstMsgInfo;
    TAF_PS_SET_SEC_PDP_CONTEXT_INFO_REQ_STRU   *pstSetPdpCtxInfoReq;
    VOS_UINT32                                  ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_CGDSCONT_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetPdpCtxInfoReq     = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgdscontCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetPdpCtxInfoReq = (TAF_PS_SET_SEC_PDP_CONTEXT_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;
        stCtrl = pstSetPdpCtxInfoReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 设置Primary PDP上下文 */
            Aps_DefPsSecPdpContext(pstSetPdpCtxInfoReq->stCtrl.usClientId,
                                   pstSetPdpCtxInfoReq->stCtrl.ucOpId,
                                   &(pstSetPdpCtxInfoReq->stPdpContextInfo),
                                   &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetSecPdpCtxInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgtftCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGTFT_CNF_STRU         *pstPsMsg;
    TAF_CTRL_STRU                       stCtrl = {0};
    VOS_UINT32                          ulErrCode;
    TAF_APS_CMD_BUFFER_STRU            *pstMsgInfo;
    TAF_PS_SET_TFT_INFO_REQ_STRU       *pstSetTftInfoReq;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_CGTFT_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetTftInfoReq        = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGTFT_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGTFT_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFT_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgtftCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetTftInfoReq = (TAF_PS_SET_TFT_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;
        stCtrl = pstSetTftInfoReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 设置TFT参数 */
            TAF_APS_SetTftInfo(&(pstSetTftInfoReq->stTftInfo), &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFT_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetTftInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgautoCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGAUTO_CNF_STRU            *pstPsMsg;
    TAF_CTRL_STRU                           stCtrl = {0};
    VOS_UINT32                              ulErrCode;
    TAF_APS_CMD_BUFFER_STRU                *pstMsgInfo;
    TAF_PS_SET_ANSWER_MODE_INFO_REQ_STRU   *pstSetAnsModeInfoReq;
    VOS_UINT32                              ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_CGAUTO_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetAnsModeInfoReq    = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);


    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGAUTO_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGAUTO_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGAUTO_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgautoCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetAnsModeInfoReq = (TAF_PS_SET_ANSWER_MODE_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;
        stCtrl = pstSetAnsModeInfoReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 检查应答模式是否合法 */
            if ( (TAF_PDP_ANSWER_MODE_MANUAL == pstSetAnsModeInfoReq->ulAnsMode)
              || (TAF_PDP_ANSWER_MODE_AUTO   == pstSetAnsModeInfoReq->ulAnsMode) )
            {
                /* 设置应答模式 */
                Aps_SetPsAnsMode(pstSetAnsModeInfoReq->stCtrl.usClientId,
                                 pstSetAnsModeInfoReq->stCtrl.ucOpId,
                                 (TAF_PDP_ANSWER_MODE_ENUM_UINT8)pstSetAnsModeInfoReq->ulAnsMode,
                                 &ulErrCode);
            }
            else
            {
                ulErrCode = TAF_PARA_SET_ERROR;
            }
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGAUTO_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetAnsModeInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvL4aSetCgansCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 处理L4A发来的消息 */

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgeqosCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGEQOS_CNF_STRU        *pstPsMsg;
    TAF_CTRL_STRU                       stCtrl = {0};
    VOS_UINT32                          ulErrCode;
    TAF_APS_CMD_BUFFER_STRU            *pstMsgInfo;
    TAF_PS_SET_EPS_QOS_INFO_REQ_STRU   *pstSetEpsQosInfoReq;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_CGEQOS_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstSetEpsQosInfoReq     = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGEQOS_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGEQOS_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOS_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgeqosCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstSetEpsQosInfoReq = (TAF_PS_SET_EPS_QOS_INFO_REQ_STRU*)pstMsgInfo->pulMsgInfo;
        stCtrl = pstSetEpsQosInfoReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 设置应答模式 */
            TAF_APS_SetEpsQosInfo(&pstSetEpsQosInfoReq->stEpsQosInfo,
                                  &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOS_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetCgeqosCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetAuthdataCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_AUTHDATA_CNF_STRU      *pstPsMsg;
    TAF_CTRL_STRU                       stCtrl = {0};
    VOS_UINT32                          ulErrCode;
    TAF_APS_CMD_BUFFER_STRU            *pstMsgInfo;
    TAF_PS_SET_AUTHDATA_INFO_REQ_STRU  *pstAuthDataReq;
    VOS_UINT32                          ulPara;

    /* 初始化消息 */
    pstPsMsg                = (APS_L4A_SET_AUTHDATA_CNF_STRU*)pstMsg;
    ulErrCode               = pstPsMsg->ulErrorCode;
    pstMsgInfo              = VOS_NULL_PTR;
    pstAuthDataReq          = VOS_NULL_PTR;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF, ulPara);

        /* 从缓冲队列中获取消息 */
        pstMsgInfo      = TAF_APS_GetItemFromCmdBufferQueue(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF, ulPara);
        if (VOS_NULL_PTR == pstMsgInfo)
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetAuthdataCnf_PreProc: Get message info failed!");
            return VOS_TRUE;
        }

        /* 消息获取成功, 更新APS本地全局变量, 写NV操作 */
        pstAuthDataReq = (TAF_PS_SET_AUTHDATA_INFO_REQ_STRU *)pstMsgInfo->pulMsgInfo;
        stCtrl = pstAuthDataReq->stCtrl;

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A全局变量更新成功, 更新APS本地全局变量, 写NV操作 */
        if (TAF_PS_CAUSE_SUCCESS == ulErrCode)
        {
            /* 设置AUTHDATA参数(NDIS) */
            Aps_DefNdisAuthdata(pstAuthDataReq->stCtrl.usClientId,
                                pstAuthDataReq->stCtrl.ucOpId,
                                &(pstAuthDataReq->stAuthDataInfo),
                                &ulErrCode);
        }

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF, ulPara);

        /* 返回AT处理结果 */
        TAF_APS_SndSetAuthDataInfoCnf(&stCtrl, ulErrCode);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgcontrdpCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGCONTRDP_CNF_STRU                     *pstPsMsg;
    VOS_UINT32                                          ulErrCode;
    VOS_UINT32                                          ulCidNum;                   /* CID数量  */
    TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF_STRU  *pstGetDynamicPdpCtxInfoCnf;
    VOS_UINT32                                          ulGetDynamicPdpCtxInfoCnfLen;
    VOS_UINT32                                          ulPara;

    /* 初始化消息 */
    pstPsMsg                            = (APS_L4A_SET_CGCONTRDP_CNF_STRU*)pstMsg;
    ulErrCode                           = pstPsMsg->ulErrorCode;
    ulCidNum                            = pstPsMsg->ulCidNum;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF, ulPara);

        /* L4A错误参数转换 */
        ulErrCode   = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A查询动态参数失败，CID设置为0 */
        if ( TAF_PS_CAUSE_SUCCESS != ulErrCode )
        {
            ulCidNum = 0;
        }

        /* 计算回复事件消息内容长度 */
        ulGetDynamicPdpCtxInfoCnfLen    = sizeof(TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF_STRU)
                                            + (ulCidNum * sizeof(TAF_PDP_DYNAMIC_PRIM_EXT_STRU));

        /* 申请内存 */
        pstGetDynamicPdpCtxInfoCnf      = (TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                                                                                           ulGetDynamicPdpCtxInfoCnfLen);

        /* 申请消息失败 */
        if ( VOS_NULL_PTR == pstGetDynamicPdpCtxInfoCnf )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgcontrdpCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        /* 填写事件内容 */
        PS_MEM_SET(pstGetDynamicPdpCtxInfoCnf,
                   0x00,
                   ulGetDynamicPdpCtxInfoCnfLen);


        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstGetDynamicPdpCtxInfoCnf->stCtrl,
                                                              TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgcontrdpCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicPdpCtxInfoCnf);

            return VOS_TRUE;
        }

        pstGetDynamicPdpCtxInfoCnf->enCause         = ulErrCode;
        pstGetDynamicPdpCtxInfoCnf->ulCidNum        = ulCidNum;

        if (0 != ulCidNum)
        {
            PS_MEM_CPY(pstGetDynamicPdpCtxInfoCnf->astPdpContxtInfo,
                       pstPsMsg->astPrimParaInfo,
                       (ulCidNum * sizeof(TAF_PDP_DYNAMIC_PRIM_EXT_STRU)));
        }

        /* 将消息返回AT */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF,
                        pstGetDynamicPdpCtxInfoCnf,
                        ulGetDynamicPdpCtxInfoCnfLen);

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicPdpCtxInfoCnf);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF, ulPara);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgscontrdpCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGSCONTRDP_CNF_STRU                    *pstPsMsg;
    VOS_UINT32                                          ulErrCode;
    VOS_UINT32                                          ulCidNum;                   /* CID数量  */
    TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF_STRU   *pstGetDynamicPdpCtxInfoCnf;
    VOS_UINT32                                          ulGetDynamicPdpCtxInfoCnfLen;
    VOS_UINT32                                          ulPara;

    /* 初始化消息 */
    pstPsMsg                            = (APS_L4A_SET_CGSCONTRDP_CNF_STRU*)pstMsg;
    ulErrCode                           = pstPsMsg->ulErrorCode;
    ulCidNum                            = pstPsMsg->ulCidNum;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF, ulPara);

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A查询动态参数失败，CID设置为0 */
        if ( TAF_PS_CAUSE_SUCCESS != ulErrCode )
        {
            ulCidNum = 0;
        }

        /* 计算事件内容长度 */
        ulGetDynamicPdpCtxInfoCnfLen    = sizeof(TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF_STRU)
                                            + (ulCidNum * sizeof(TAF_PDP_DYNAMIC_SEC_EXT_STRU));

        /* 申请内存 */
        pstGetDynamicPdpCtxInfoCnf      = (TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                                                                                          ulGetDynamicPdpCtxInfoCnfLen);

        /* 申请消息失败 */
        if ( VOS_NULL_PTR == pstGetDynamicPdpCtxInfoCnf )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgscontrdpCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        /* 填写事件内容 */
        PS_MEM_SET(pstGetDynamicPdpCtxInfoCnf,
                   0x00,
                   ulGetDynamicPdpCtxInfoCnfLen);

        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstGetDynamicPdpCtxInfoCnf->stCtrl,
                                                              TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgscontrdpCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicPdpCtxInfoCnf);

            return VOS_TRUE;
        }

        pstGetDynamicPdpCtxInfoCnf->enCause          = ulErrCode;
        pstGetDynamicPdpCtxInfoCnf->ulCidNum         = ulCidNum;

        if (0 != ulCidNum)
        {
            PS_MEM_CPY(pstGetDynamicPdpCtxInfoCnf->astPdpContxtInfo,
                       pstPsMsg->astSecParaInfo,
                       (ulCidNum * sizeof(TAF_PDP_DYNAMIC_SEC_EXT_STRU)));
        }

        /* 将消息返回AT */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF,
                         pstGetDynamicPdpCtxInfoCnf,
                         ulGetDynamicPdpCtxInfoCnfLen);

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicPdpCtxInfoCnf);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF, ulPara);
    }

    return VOS_TRUE;
}
VOS_VOID TAF_APS_FillPfTftInfo(
    TAF_PF_TFT_STRU                    *pstPfTftInfo,
    APS_L4A_SDF_PF_TFT_STRU            *pstL4aPfTftInfo,
    VOS_UINT32                          ulCidNum
)
{
    VOS_UINT32                          ulIndex1;
    VOS_UINT32                          ulIndex2;

    /* 初始化参数 */
    ulIndex1        = VOS_NULL;
    ulIndex2        = VOS_NULL;

    /* 循环遍历填充TFT参数 */
    for (ulIndex1 = 0; ulIndex1 < ulCidNum; ulIndex1++)
    {
        pstPfTftInfo[ulIndex1].ulCid    = pstL4aPfTftInfo[ulIndex1].ulCid;
        pstPfTftInfo[ulIndex1].ulPFNum  = pstL4aPfTftInfo[ulIndex1].ulPfNum;

        for (ulIndex2 = 0; ulIndex2 < pstL4aPfTftInfo[ulIndex1].ulPfNum; ulIndex2++)
        {
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpPktFilterId    = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpPktFilterId;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpPrecedence     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpPrecedence;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSrcIp          = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSrcIp;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpProtocolId     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpProtocolId;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpDestPortRange  = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpDestPortRange;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSrcPortRange   = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSrcPortRange;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSpi            = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpSpi;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpTosMask        = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpTosMask;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpFlowLable      = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpFlowLable;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpDirection      = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpDirection;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpNwPktFilterId  = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].bitOpNwPktFilterId;

            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucCid               = (VOS_UINT8)pstL4aPfTftInfo[ulIndex1].ulCid;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucDefined           = VOS_TRUE;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucPacketFilterId    = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucPacketFilterId;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucPrecedence        = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucPrecedence;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucProtocolId        = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucProtocolId;

            PS_MEM_CPY(&(pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].stSourceIpaddr),
                       &(pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].stSourceIpaddr),
                       sizeof(TAF_PDP_ADDR_STRU));
            PS_MEM_CPY(&(pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].stSourceIpMask),
                       &(pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].stSourceIpMask),
                       sizeof(TAF_PDP_ADDR_STRU));

            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usLowSourcePort     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usLowSourcePort;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usHighSourcePort    = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usHighSourcePort;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usLowDestPort       = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usLowDestPort;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usHighDestPort      = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].usHighDestPort;

            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ulSecuParaIndex     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ulSecuParaIndex;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ulFlowLable         = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ulFlowLable;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucTypeOfServiceMask = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucTypeOfServiceMask;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucTypeOfService     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucTypeOfService;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucDirection         = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].enTftFilterDirection;
            pstPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucNwPktFilterId     = pstL4aPfTftInfo[ulIndex1].astTftInfo[ulIndex2].ucNwPktFilterId;
        }

    }
    return;
}


VOS_UINT32 TAF_APS_RcvL4aSetCgtftrdpCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGTFTRDP_CNF_STRU          *pstPsMsg;
    VOS_UINT32                              ulErrCode;
    VOS_UINT32                              ulCidNum;                   /* CID数量  */
    TAF_PS_GET_DYNAMIC_TFT_INFO_CNF_STRU   *pstGetDynamicTftInfoCnf;
    VOS_UINT32                              ulGetDynamicTftInfoCnfLen;
    VOS_UINT32                              ulPara;

    /* 初始化消息 */
    pstPsMsg                            = (APS_L4A_SET_CGTFTRDP_CNF_STRU*)pstMsg;
    ulErrCode                           = pstPsMsg->ulErrorCode;
    ulCidNum                            = pstPsMsg->ulCidNum;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF, ulPara);

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A查询动态参数失败，CIDNum设置为0 */
        if ( TAF_PS_CAUSE_SUCCESS != ulErrCode )
        {
            ulCidNum = 0;
        }

        /* 计算事件内容长度 */
        ulGetDynamicTftInfoCnfLen   = sizeof(TAF_PS_GET_DYNAMIC_TFT_INFO_CNF_STRU)
                                        + (ulCidNum * sizeof(TAF_PF_TFT_STRU));

        /* 申请内存 */
        pstGetDynamicTftInfoCnf = (TAF_PS_GET_DYNAMIC_TFT_INFO_CNF_STRU*)PS_MEM_ALLOC(
                                                WUEPS_PID_TAF,
                                                ulGetDynamicTftInfoCnfLen);

        /* 申请消息失败 */
        if ( VOS_NULL_PTR == pstGetDynamicTftInfoCnf )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgtftrdpCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        /* 填写事件内容 */
        PS_MEM_SET(pstGetDynamicTftInfoCnf,
                   0x00,
                   ulGetDynamicTftInfoCnfLen);

        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstGetDynamicTftInfoCnf->stCtrl,
                                                              TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgtftrdpCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicTftInfoCnf);

            return VOS_TRUE;
        }

        pstGetDynamicTftInfoCnf->enCause          = ulErrCode;
        pstGetDynamicTftInfoCnf->ulCidNum         = ulCidNum;

        TAF_APS_FillPfTftInfo(pstGetDynamicTftInfoCnf->astPfTftInfo, pstPsMsg->astPfTftInfo, ulCidNum);

        /* 将消息返回AT */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_DYNAMIC_TFT_INFO_CNF,
                         pstGetDynamicTftInfoCnf,
                         ulGetDynamicTftInfoCnfLen);

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicTftInfoCnf);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF, ulPara);
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvL4aSetCgeqosrdpCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_CGEQOSRDP_CNF_STRU                     *pstPsMsg;
    VOS_UINT32                                          ulErrCode;
    VOS_UINT32                                          ulCidNum;                   /* CID数量  */
    TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF_STRU           *pstGetDynamicEpsQosInfoCnf;
    VOS_UINT32                                          ulGetDynamicEpsQosInfoCnfLen;
    VOS_UINT32                                          ulPara;

    /* 初始化消息 */
    pstPsMsg                            = (APS_L4A_SET_CGEQOSRDP_CNF_STRU*)pstMsg;
    ulErrCode                           = pstPsMsg->ulErrorCode;
    ulCidNum                            = pstPsMsg->ulCidNum;

    ulPara = TAF_APS_BuildTmrParaByCtrlHdr(pstPsMsg->stCtrl.ulPid,
                                           pstPsMsg->stCtrl.ulClientId);

    /* 判断保护定时器是否已超时 */
    if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF,
                                                              ulPara))
    {
        /* 保护定时器没有超时, 停止定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF, ulPara);

        /* L4A错误参数转换 */
        ulErrCode           = TAF_APS_TransL4aErrorCode(ulErrCode);

        /* 如果L4A查询动态参数失败，CIDNum设置为0 */
        if ( TAF_PS_CAUSE_SUCCESS != ulErrCode )
        {
            ulCidNum = 0;
        }

        /* 计算事件内容长度 */
        ulGetDynamicEpsQosInfoCnfLen    = sizeof(TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF_STRU)
                                            + (ulCidNum * sizeof(TAF_EPS_QOS_EXT_STRU));

        /* 申请内存 */
        pstGetDynamicEpsQosInfoCnf      = (TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                                                                                  ulGetDynamicEpsQosInfoCnfLen);

        /* 申请消息失败 */
        if ( VOS_NULL_PTR == pstGetDynamicEpsQosInfoCnf )
        {
            /* 系统异常打印, 直接返回 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgtftrdpCnf_PreProc: PS_MEM_ALLOC Error!");
            return VOS_TRUE;
        }

        /* 填写事件内容 */
        PS_MEM_SET(pstGetDynamicEpsQosInfoCnf,
                   0x00,
                   ulGetDynamicEpsQosInfoCnfLen);

        /* 从缓冲队列中获取消息 */
        if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&pstGetDynamicEpsQosInfoCnf->stCtrl,
                                                              TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF,
                                                              ulPara))
        {
            /* 消息获取失败, 打印错误信息 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aSetCgeqosrdpCnf_PreProc: Get message info failed!");

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicEpsQosInfoCnf);

            return VOS_TRUE;
        }

        pstGetDynamicEpsQosInfoCnf->enCause           = ulErrCode;
        pstGetDynamicEpsQosInfoCnf->ulCidNum          = ulCidNum;

        if (0 != ulCidNum)
        {
            PS_MEM_CPY(pstGetDynamicEpsQosInfoCnf->astEpsQosInfo,
                       pstPsMsg->astEpsQosInfo,
                       (ulCidNum * sizeof(TAF_EPS_QOS_EXT_STRU)));
        }

        /* 将消息返回AT */
        TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF,
                         pstGetDynamicEpsQosInfoCnf,
                         ulGetDynamicEpsQosInfoCnfLen);

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_TAF, pstGetDynamicEpsQosInfoCnf);

        /* 从消息等待队列中删除该消息 */
        TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF, ulPara);

    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgdcontCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGDCONT_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgdcontCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetPrimPdpCtxInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDCONT_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgdscontCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                         TI_TAF_APS_WAIT_SET_CGDSCONT_CNF,
                                                         pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgdscontCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetSecPdpCtxInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGDSCONT_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgtftCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGTFT_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgtftCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetTftInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFT_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgautoCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGAUTO_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgautoCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetAnsModeInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGAUTO_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgeqosCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGEQOS_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgeqosCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetCgeqosCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOS_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetAuthdataCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_AUTHDATA_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetAuthdataCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetAuthDataInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_AUTHDATA_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiGetLtecsCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_GET_LTE_CS_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiGetLtecsCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetLtecsInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_LTE_CS_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiGetCemodeCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_GET_CEMODE_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiGetCemodeCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetCemodeInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_GET_CEMODE_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetPdprofInfoExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_PDPROF_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetPdprofInfoExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndSetPdprofInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_PDPROF_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgcontrdpCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl = {0};
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgcontrdpCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetDynamicPrimPdpCtxInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT, 0);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGCONTRDP_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgscontrdpCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl = {0};
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgscontrdpCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetDynamicSecPdpCtxInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT, 0);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGSCONTRDP_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgtftrdpCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl = {0};
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgtftrdpCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetDynamicTftInfoCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT, 0);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGTFTRDP_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
VOS_UINT32 TAF_APS_RcvTiSetCgeqosrdpCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_CTRL_STRU                       stCtrl;
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    if (VOS_TRUE != TAF_APS_GetCtrlHdrFromCmdBufferQueue(&stCtrl,
                                                          TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF,
                                                          pstRcvTimerMsg->ulPara))
    {
        /* 消息获取失败, 打印错误信息 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvTiSetCgeqosrdpCnfExpired_PreProc: Get message info failed!");
        return VOS_TRUE;
    }

    /* 将失败消息返回给AT */
    TAF_APS_SndGetCgeqosrdpCnf(&stCtrl, TAF_PS_CAUSE_APS_TIME_OUT);

    /* 从消息等待队列中删除该消息 */
    TAF_APS_DelItemInCmdBufferQueue(TI_TAF_APS_WAIT_SET_CGEQOSRDP_CNF, pstRcvTimerMsg->ulPara);

    return VOS_TRUE;
}
TAF_PS_CAUSE_ENUM_UINT32 TAF_APS_TransL4aErrorCode(
    VOS_UINT32                          ulL4aErrCode
)
{
    /* 所有非成功的错误码均返回参数设置错误 */
    if (0 == ulL4aErrCode)
    {
        return TAF_PS_CAUSE_SUCCESS;
    }
    else
    {
        return TAF_PS_CAUSE_INVALID_PARAMETER;
    }
}
VOS_UINT32 TAF_APS_RcvL4aApsPdpActivateCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_ACTIVATE_CNF_STRU      *pstPdpActCnf;

    pstPdpActCnf = (APS_L4A_PDP_ACTIVATE_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpActCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpActivateRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_ACTIVATE_REJ_STRU      *pstPdpActRej;

    pstPdpActRej = (APS_L4A_PDP_ACTIVATE_REJ_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpActRej->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpActivateInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_ACTIVATE_IND_STRU      *pstPdpActInd;
    VOS_UINT8                           ucPdpId;
    TAF_CTRL_STRU                       stCtrl;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));
    /* MODULE ID默认设置为AT, 如果需要上报给IMSA的话，在网络侧上报
       SM_ESM_EPS_BEARER_INFO_IND_STRU消息后重新刷新 */
    stCtrl.ulModuleId   = WUEPS_PID_AT;
    stCtrl.usClientId   = TAF_APS_INVALID_CLIENTID;
    stCtrl.ucOpId       = TAF_APS_INVALID_OPID;

    pstPdpActInd = (APS_L4A_PDP_ACTIVATE_IND_STRU*)pstMsg;

    /* 分配(PdP Id) */
    if (TAF_APS_ALLOC_PDPID_FAIL == TAF_APS_AllocPdpId(&stCtrl,
                                                       pstPdpActInd->ucCid,
                                                       &ucPdpId))
    {
        /* 分配不到实体，打印异常，此时不应该分配不到实体 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvL4aApsPdpActivateInd_PreProc, alloc pdp id fail");
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpModifyCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_MODIFY_CNF_STRU      *pstPdpModifyCnf;

    pstPdpModifyCnf = (APS_L4A_PDP_MODIFY_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpModifyCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpModifyRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_MODIFY_REJ_STRU      *pstPdpModifyRej;

    pstPdpModifyRej = (APS_L4A_PDP_MODIFY_REJ_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpModifyRej->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpModifyInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_MODIFY_IND_STRU      *pstPdpModifyInd;

    pstPdpModifyInd = (APS_L4A_PDP_MODIFY_IND_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpModifyInd->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_APS_RcvL4aApsPdpDeactivateCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_DEACTIVATE_CNF_STRU    *pstPdpDeactCnf;

    pstPdpDeactCnf = (APS_L4A_PDP_DEACTIVATE_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpDeactCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aApsPdpDeactivateRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_DEACTIVATE_REJ_STRU    *pstPdpDeactRej;

    pstPdpDeactRej = (APS_L4A_PDP_DEACTIVATE_REJ_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpDeactRej->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}




VOS_UINT32 TAF_APS_RcvL4aApsPdpDeactivateInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PDP_DEACTIVATE_IND_STRU    *pstPdpDeactInd;

    pstPdpDeactInd = (APS_L4A_PDP_DEACTIVATE_IND_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPdpDeactInd->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aSenNdisconnCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_SET_NDISCONN_CNF_STRU      *pstSetNdisConnCnf;

    pstSetNdisConnCnf = (APS_L4A_SET_NDISCONN_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstSetNdisConnCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_APS_RcvEsmEpsBearInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;
    VOS_UINT8                           ucPdpId;
    VOS_UINT8                           ucCid;
    TAF_CTRL_STRU                       stCtrl;

    PS_MEM_SET(&stCtrl, 0, sizeof(TAF_CTRL_STRU));
    /* MODULE ID默认设置为AT, 如果需要上报给IMSA的话，在网络侧上报
       SM_ESM_EPS_BEARER_INFO_IND_STRU消息后重新刷新 */
    stCtrl.ulModuleId   = WUEPS_PID_AT;
    stCtrl.usClientId   = TAF_APS_INVALID_CLIENTID;
    stCtrl.ucOpId       = TAF_APS_INVALID_OPID;

    pstBearerInfo = (SM_ESM_EPS_BEARER_INFO_IND_STRU*)pstMsg;


    ucCid = (VOS_UINT8)TAF_APS_GetCidFromLteBitCid(pstBearerInfo->ulBitCid);

    /* Cid的值无效，则直接返回 */
    if ( TAF_INVALID_CID == ucCid )
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvEsmEpsBearInfoInd_PreProc, invalid Cid");
        return VOS_TRUE;
    }

    /* 正常情况下，按照CID来查找Pdp Id */
    ucPdpId = TAF_APS_GetPdpIdByCid(ucCid);
    if (TAF_APS_INVALID_PDPID != ucPdpId)
    {
        return VOS_FALSE;
    }
    else
    {
        /* 自动应答模式下, 需要分配新的APS实体来存储承载信息 */
        TAF_NORMAL_LOG(WUEPS_PID_TAF, "TAF_APS_RcvEsmEpsBearInfoInd_PreProc, Get PDP ID fail.");
    }


    /* cid为0时，表示是默认承载，此时如果是激活，则分配一个新的PDP ID
       client id和op id此时认为是无效值，后面真正激活时，再进行刷新
    */
    if ( ((TAF_APS_DEFAULT_CID == ucCid) || (ucCid > APS_PDP_MAX_CID))
      && (SM_ESM_PDP_OPT_ACTIVATE == pstBearerInfo->enPdpOption) )
    {
        /* 分配(PdP Id) */
        if (TAF_APS_ALLOC_PDPID_FAIL == TAF_APS_AllocPdpId(&stCtrl,
                                                           ucCid,
                                                           &ucPdpId))
        {
            /* 分配不到实体，打印异常，此时不应该分配不到实体 */
            TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvEsmEpsBearInfoInd_PreProc, alloc pdp id fail");
            return VOS_TRUE;
        }

        /* 记录默认承载的标志 */
        if (TAF_APS_DEFAULT_CID == ucCid)
        {
           TAF_APS_SetPdpEntAttachBearerFlag(ucPdpId, VOS_TRUE);
        }
    }
    else
    {
        /* 异常消息, 丢弃 */
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_APS_RcvEsmEpsBearInfoInd_PreProc, Abnormal EPS Bearer.");
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aPppDialCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PPP_DIAL_CNF_STRU      *pstPppDialCnf;

    pstPppDialCnf = (APS_L4A_PPP_DIAL_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPppDialCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvL4aPsCallEndCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_L4A_PS_CALL_END_CNF_STRU      *pstPsCallEndCnf;

    pstPsCallEndCnf = (APS_L4A_PS_CALL_END_CNF_STRU*)pstMsg;

    /* 如果找不到对应的PDP ID，则丢弃该消息 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByCid(pstPsCallEndCnf->ucCid))
    {
        /* 丢弃该消息 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 TAF_APS_RcvEsmNotificationInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    ESM_APS_NOTIFICATION_IND_STRU      *pstEsmNotiInd;

    pstEsmNotiInd = (ESM_APS_NOTIFICATION_IND_STRU*)pstMsg;

    /* 发送消息 */
    TAF_APS_SndImsaSrvccCancelNotify(pstEsmNotiInd->enNotificationIndicator);

    return VOS_TRUE;
}

#endif

#endif
VOS_UINT32 TAF_APS_RcvSndcpActivateRsp_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_SNDCP_ACTIVATE_RSP_MSG         *pstSnActRsp;

    pstSnActRsp = (APS_SNDCP_ACTIVATE_RSP_MSG*)pstMsg;

    /* 从SNDCP的消息中获取NSAPI,如果获取不到则丢弃 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByNsapi(pstSnActRsp->ApsSnActRsp.ucNsapi))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvSndcpModifyRsp_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_SNDCP_MODIFY_RSP_MSG           *pstSnModifyRsp;

    pstSnModifyRsp = (APS_SNDCP_MODIFY_RSP_MSG*)pstMsg;

    /* 从SNDCP的消息中获取NSAPI,如果获取不到则丢弃 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByNsapi(pstSnModifyRsp->ApsSnMdfRsp.ucNsapi))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvSndcpDeactRsp_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_SNDCP_DEACTIVATE_RSP_MSG           *pstSnDeactRsp;

    pstSnDeactRsp = (APS_SNDCP_DEACTIVATE_RSP_MSG*)pstMsg;

    /* 从SNDCP的消息中获取NSAPI,如果获取不到则丢弃 */
    if (TAF_APS_INVALID_PDPID == TAF_APS_GetPdpIdByNsapi(pstSnDeactRsp->ApsSnDeActRsp.ucNsapi))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_IPV6)

VOS_UINT32 TAF_APS_RcvNdIpv6ParaInfoNotifyInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    TAF_CTRL_STRU                       stCtrl;
    MMC_APS_RAT_TYPE_ENUM_UINT32        enRatType;
    NDCLIENT_APS_PARA_INFO_NOTIFY_STRU *pstNdIpv6ParaInfoNotifyInd;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;


    PS_MEM_SET(&stCtrl, 0x00, sizeof(stCtrl));

    /* 初始化, 获取消息内容 */
    pstNdIpv6ParaInfoNotifyInd = (NDCLIENT_APS_PARA_INFO_NOTIFY_STRU*)pstMsg;

    /* 获取NSAPI对应的APS实体索引 */
    ucPdpId = TAF_APS_GetPdpIdByNsapi(pstNdIpv6ParaInfoNotifyInd->ucRabId);
    if (TAF_APS_INVALID_PDPID != ucPdpId)
    {
        /* 获取该PDPID的PDP实体内容地址 */
        pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

        /* 更新APS实体的RA参数信息 */
        pstPdpEntity->bitOpIpv6RaInfo = VOS_TRUE;
        PS_MEM_CPY((VOS_CHAR*)&pstPdpEntity->stIpv6RaInfo,
                   (VOS_CHAR*)&pstNdIpv6ParaInfoNotifyInd->stIpv6Info,
                   sizeof(NDCLIENT_APS_IPV6_INFO_STRU));

#if (FEATURE_ON == FEATURE_LTE)
        /* 同步RA信息给ESM */
        TAF_APS_SndL4aIpv6InfoNotifyInd(pstPdpEntity->ucNsapi,
                                        &pstPdpEntity->stIpv6RaInfo);
#endif

        /* 检查当前接入技术, 如果是GU模, 需要配置IPF */
        enRatType = TAF_APS_GetPdpEntityRatType(ucPdpId);
        if ( (MMC_APS_RAT_TYPE_GSM   == enRatType)
          || (MMC_APS_RAT_TYPE_WCDMA == enRatType) )
        {
            TAF_APS_IpfConfigUlFilter(ucPdpId);
        }

        /* 检查当前APS是否有用户使用, 如果有用户使用, 上报RA信息事件 */
        if ((TAF_APS_INVALID_CLIENTID != TAF_APS_GetPdpEntClientId(ucPdpId, pstPdpEntity->stClientInfo.ucCid))
         && (VOS_TRUE == pstPdpEntity->ulUsedFlg))
        {
            /* 填写APP控制头 */
            TAF_APS_SetPsCallEvtCtrl(ucPdpId, pstPdpEntity->stClientInfo.ucCid, &stCtrl);

            /* 上报ID_EVT_TAF_PS_IPV6_INFO_IND事件 */
            TAF_APS_SndIpv6RaInfoNotifyInd(&stCtrl,
                                           pstPdpEntity->stClientInfo.ucCid,
                                           pstPdpEntity->ucNsapi,
                                           (TAF_PDP_IPV6_RA_INFO_STRU *)&pstPdpEntity->stIpv6RaInfo);
        }
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvNdIpv6ParaInfoNotifyInd_PreProc: Can't find PDP ID form RAB ID!");
    }

    return VOS_TRUE;
}
#endif
VOS_UINT32 TAF_APS_RcvAtGetGprsActiveTypeReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                              ulErrCode;
    TAF_PDP_TYPE_ENUM_UINT8                 enPdpType;
    TAF_CID_GPRS_ACTIVE_TYPE_STRU           stCidGprsActiveType;
    TAF_PS_MSG_STRU                        *pstAppMsg;
    TAF_PS_GET_D_GPRS_ACTIVE_TYPE_REQ_STRU *pstGetGprsActiveTypeReq;

    /* 初始化 */
    ulErrCode               = TAF_PARA_OK;
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstGetGprsActiveTypeReq = (TAF_PS_GET_D_GPRS_ACTIVE_TYPE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取GPRS激活类型 */
    PS_MEM_SET(&stCidGprsActiveType, 0x00, sizeof(TAF_CID_GPRS_ACTIVE_TYPE_STRU));
    stCidGprsActiveType.ucCid = pstGetGprsActiveTypeReq->stAtdPara.ucCid;

    enPdpType = Taf_GetCidType(pstGetGprsActiveTypeReq->stAtdPara.ucCid);

    /* 删除L2P协议类型的判断处理 */

    /* 直接根据CID上下文的PDP类型决定创建PPP实体类型 */
    switch (enPdpType)
    {
        case TAF_PDP_IPV4:
            /* IP类型,进行IP激活，TE中启动了PPP，MT中启动PPP */
            stCidGprsActiveType.enActiveType = TAF_IP_ACTIVE_TE_PPP_MT_PPP_TYPE;
            break;

#if (FEATURE_ON == FEATURE_IPV6)
        case TAF_PDP_IPV6:
        case TAF_PDP_IPV4V6:
            /* IP类型,进行IP激活，TE中启动了PPP，MT中启动PPP */
            stCidGprsActiveType.enActiveType = TAF_IP_ACTIVE_TE_PPP_MT_PPP_TYPE;
            break;
#endif

        case TAF_PDP_PPP:
            /* PPP类型,进行PPP激活，TE中启动了PPP，MT中不启动PPP */
            stCidGprsActiveType.enActiveType = TAF_PPP_ACTIVE_TE_PPP_MT_NOT_PPP_TYPE;
            break;

        default:
            ulErrCode = TAF_PS_CAUSE_INVALID_PARAMETER;
            break;
    }

    TAF_APS_SndGetGprsActiveTypeCnf(&(pstGetGprsActiveTypeReq->stCtrl),
                                   ulErrCode,
                                   &stCidGprsActiveType);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetDnsQueryReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                        *pstPsMsg        = VOS_NULL_PTR;
    TAF_PS_GET_NEGOTIATION_DNS_REQ_STRU    *pstNegoDnsReq   = VOS_NULL_PTR;
    TAF_PS_GET_NEGOTIATION_DNS_CNF_STRU     stNegoDnsCnf;
    VOS_UINT8                               ucPdpId         = TAF_APS_INVALID_PDPID;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity    = VOS_NULL_PTR;
    TAF_APS_STA_ENUM_UINT32                 enStaState      = TAF_APS_STA_BUTT;

    /* 初始化 */
    pstPsMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstNegoDnsReq   = (TAF_PS_GET_NEGOTIATION_DNS_REQ_STRU *)pstPsMsg->aucContent;
    ucPdpId         = TAF_APS_GetPdpIdByCid(pstNegoDnsReq->ucCid);
    PS_MEM_SET(&stNegoDnsCnf, 0, sizeof(stNegoDnsCnf));

    /* 填写消息内容 */
    stNegoDnsCnf.stCtrl                 = pstNegoDnsReq->stCtrl;

    stNegoDnsCnf.enCause                = TAF_PS_CAUSE_UNKNOWN;

    /* 获取CID对应的PdpId */
    if (TAF_APS_INVALID_PDPID != ucPdpId)
    {
        /* 检查该CID对应PDP是否激活 */
        enStaState  = TAF_APS_GetPdpIdMainFsmState(ucPdpId);
        if ( (TAF_APS_STA_ACTIVE == enStaState)
          || (TAF_APS_STA_MS_MODIFYING == enStaState)
          || (TAF_APS_STA_MS_DEACTIVATING == enStaState) )
        {
            /* 获取该PDPID的PDP实体内容地址 */
            pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

            /* 获取该激活PDP对应的主DNS, 辅DNS信息 */
            stNegoDnsCnf.stNegotiationDns.ucCid = pstNegoDnsReq->ucCid;
            stNegoDnsCnf.enCause = TAF_PS_CAUSE_SUCCESS;
            stNegoDnsCnf.stNegotiationDns.stDnsInfo.bitOpPrimDnsAddr = VOS_TRUE;
            stNegoDnsCnf.stNegotiationDns.stDnsInfo.bitOpSecDnsAddr  = VOS_TRUE;

            if (VOS_TRUE == pstPdpEntity->PdpDnsFlag)
            {
                if (VOS_TRUE == pstPdpEntity->PdpDns.OP_PriDns)
                {
                    PS_MEM_CPY(stNegoDnsCnf.stNegotiationDns.stDnsInfo.aucPrimDnsAddr,
                               pstPdpEntity->PdpDns.aucPriDns,
                               TAF_IPV4_ADDR_LEN);
                }

                if (VOS_TRUE == pstPdpEntity->PdpDns.OP_SecDns)
                {
                    PS_MEM_CPY(stNegoDnsCnf.stNegotiationDns.stDnsInfo.aucSecDnsAddr,
                               pstPdpEntity->PdpDns.aucSecDns,
                               TAF_IPV4_ADDR_LEN);
                }
            }
        }
    }

    /* 将处理结果返回AT模块 */
    TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_NEGOTIATION_DNS_CNF,
                     &stNegoDnsCnf,
                     sizeof(TAF_PS_GET_NEGOTIATION_DNS_CNF_STRU));

    return VOS_TRUE;
}






VOS_UINT32 TAF_APS_RcvTiApsLimitPdpActExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvTiApsMsActvaingExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* PDP激活受限处理:此定时器超时认为异常,PDP激活次数按一次处理 */
    TAF_APS_ProcPdpActLimit_PreProc(1);

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_GetActPdpLimitTimerLen(
    VOS_UINT8                           ucFailCount
)
{
    VOS_UINT32                          ulTimeLen;

    ulTimeLen = 0;

    /* 暂停PDP激活定时器时长随着激活失败的次数来确定，小于等于10次直接返回时长0.
    11-18次按照10s步长递增时长，大于18次的统一使用90s时长 */
    if (ucFailCount <= TAF_APS_PDP_ACT_LIMIT_NO_DELAY_COUNT_THRESHOLD)
    {
        return 0;
    }

    /* PDP激活被拒11-19次之内，按照10s步长递增启动定时器 */
    if (ucFailCount < TAF_APS_PDP_ACT_LIMIT_STEP_DELAY_COUNT_THRESHOLD)
    {
        ulTimeLen = (ucFailCount - TAF_APS_PDP_ACT_LIMIT_NO_DELAY_COUNT_THRESHOLD)
                        * TAF_APS_PDP_ACT_LIMIT_STEP_TIME;
    }
    else
    {   /* PDP激活被拒19次(包括19次)以上，按照90s时长启动定时器 */
        /* 当失败次数是10的整数倍时，PDP暂停激活定时器使用随机时长 */
        if (0 == (ucFailCount % TAF_APS_PDP_ACT_LIMIT_RANDOM_TIME_FAILCOUNT))
        {
            ulTimeLen = TAF_APS_PDP_ACT_LIMIT_AVERAGE_DELAY_TIME
                        + VOS_Rand(TAF_APS_PDP_ACT_LIMIT_RANDOM_TIME_THRESHOLD);
        }
        else
        {
            ulTimeLen = TAF_APS_PDP_ACT_LIMIT_AVERAGE_DELAY_TIME;
        }
    }

    /* 定时器时长单位为ms */
    return (ulTimeLen * TIMER_S_TO_MS_1000);
}


VOS_VOID TAF_APS_ProcPdpActLimit_PreProc(VOS_UINT8 ucTryingCount)
{
    TAF_APS_PDP_ACT_LIMIT_INFO_STRU    *pstPdpActLimitInfo;

    pstPdpActLimitInfo = TAF_APS_GetPdpActLimitInfoAddr();

    /* 判断PDP激活受限功能是否打开 */
    if (VOS_TRUE == pstPdpActLimitInfo->ucPdpActLimitFlg)
    {
        pstPdpActLimitInfo->ucPdpActFailCount = pstPdpActLimitInfo->ucPdpActFailCount + ucTryingCount;

        if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_PDP_ACT_LIMIT_PDP_ID))
        {
            return;
        }

        if (pstPdpActLimitInfo->ucPdpActFailCount > TAF_APS_PDP_ACT_LIMIT_NO_DELAY_COUNT_THRESHOLD)
        {
            TAF_APS_StartTimer(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_GetActPdpLimitTimerLen(pstPdpActLimitInfo->ucPdpActFailCount), TAF_APS_PDP_ACT_LIMIT_PDP_ID);
        }
    }

    return;
}
VOS_UINT32 TAF_APS_IsPdpActLimit_PreProc(VOS_VOID)
{
    MMC_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    enCurrRatType        = TAF_APS_GetCurrPdpEntityRatType();

    /* PS域无效，到状态机中处理 */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        return VOS_FALSE;
    }

    /* 目前PDP激活受限的功能只适用于GU模 */
    if ((MMC_APS_RAT_TYPE_GSM == enCurrRatType)
     || (MMC_APS_RAT_TYPE_WCDMA == enCurrRatType))
    {
        if (TAF_APS_TIMER_STATUS_RUNING == TAF_APS_GetTimerStatus(TI_TAF_APS_LIMIT_PDP_ACT, TAF_APS_PDP_ACT_LIMIT_PDP_ID))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_PdpActLimit_PreProc(
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpStateReq
)
{
    if (VOS_TRUE == TAF_APS_IsPdpActLimit_PreProc())
    {
        /* 不处理去激活 */
        if (TAF_CGACT_ACT != pstSetPdpStateReq->stCidListStateInfo.ucState)
        {
            return VOS_FALSE;
        }

        TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpStateReq->stCtrl),
                                    TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 TAF_APS_RcvOmSetDsflowRptReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    ID_NAS_OM_INQUIRE_STRU             *pstNasOmInq     = VOS_NULL_PTR;
    NAS_OM_SET_DSFLOW_RPT_REQ_STRU     *pstDsflowRptReq = VOS_NULL_PTR;
    NAS_OM_SET_DSFLOW_RPT_CNF_STRU      stDsflowRptCnf;
    TAF_APS_DSFLOW_STATS_CTX_STRU      *pstApsDsFlowCtx;

    /* 初始化 */
    pstNasOmInq     = (ID_NAS_OM_INQUIRE_STRU *)pstMsg;
    pstDsflowRptReq = (NAS_OM_SET_DSFLOW_RPT_REQ_STRU *)pstNasOmInq->aucData;
    pstApsDsFlowCtx = TAF_APS_GetDsFlowCtxAddr();
    PS_MEM_SET(&stDsflowRptCnf, 0, sizeof(NAS_OM_SET_DSFLOW_RPT_CNF_STRU));

    /* 更新OM流量上报标志位 */
    switch(pstDsflowRptReq->enRptAction)
    {
        case NAS_OM_REPORT_START:
            if ( (pstDsflowRptReq->usRptPeriod >= TI_TAF_APS_DSFLOW_REPORT_LEN)
              && (0 == (pstDsflowRptReq->usRptPeriod % TI_TAF_APS_DSFLOW_REPORT_LEN)) )
            {
                pstApsDsFlowCtx->ucDsFlowOMReportFlg    = VOS_TRUE;
                pstApsDsFlowCtx->ucDsFlowOMRptPeriod    = (VOS_UINT8)(pstDsflowRptReq->usRptPeriod / TI_TAF_APS_DSFLOW_REPORT_LEN);
                stDsflowRptCnf.enResult                 = NAS_OM_RESULT_NO_ERROR;
            }
            else
            {
                stDsflowRptCnf.enResult                 = NAS_OM_RESULT_INCORRECT_PARAMETERS;
            }
            break;
        case NAS_OM_REPORT_STOP:
            pstApsDsFlowCtx->ucDsFlowOMReportFlg        = VOS_FALSE;
            pstApsDsFlowCtx->ucDsFlowOMRptPeriod        = 0;
            break;
        default:
            stDsflowRptCnf.enResult                     = NAS_OM_RESULT_INCORRECT_PARAMETERS;
            break;
    }

    TAF_APS_SndOmData(OM_TRANS_FUNC,
                      pstNasOmInq->usToolsId,
                      ID_NAS_OM_SET_DSFLOW_RPT_CNF,
                      sizeof(NAS_OM_SET_DSFLOW_RPT_CNF_STRU),
                      (VOS_UINT8 *)&stDsflowRptCnf);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvGetCidSdfReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucNum;
    TAF_PS_SDF_INFO_REQ_STRU           *pstSdfInfoReq;
    TAF_PS_SDF_INFO_CNF_STRU           *pstSdfInfoCnf;

    pstSdfInfoReq = (TAF_PS_SDF_INFO_REQ_STRU *)pstMsg;

    /* 申请消息内存 */
    pstSdfInfoCnf = (TAF_PS_SDF_INFO_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                            sizeof(TAF_PS_SDF_INFO_CNF_STRU));

    /* 申请消息失败 */
    if ( VOS_NULL_PTR == pstSdfInfoCnf )
    {
        MN_ERR_LOG("TAF_APS_RcvGetCidSdfReq_PreProc:  ERROR : PS_MEM_ALLOC Error!");
        return VOS_TRUE;
    }

    PS_MEM_SET(pstSdfInfoCnf, 0, sizeof(TAF_PS_SDF_INFO_CNF_STRU));

    /* 填写消息头 */
    pstSdfInfoCnf->stCtrl = pstSdfInfoReq->stCtrl;

    for (ucNum = 1; ucNum <= TAF_MAX_CID_NV; ucNum++)
    {
        if (VOS_OK == TAF_APS_GetSdfParaInfo(ucNum,
                              &(pstSdfInfoCnf->stSdfInfo.astSdfPara[pstSdfInfoCnf->stSdfInfo.ulSdfNum])))
        {
            pstSdfInfoCnf->stSdfInfo.ulSdfNum ++;
        }
    }

    /* 发送ID_MSG_TAF_GET_CID_SDF_CNF消息 */
    TAF_APS_SndPsEvt(ID_EVT_TAF_PS_GET_CID_SDF_CNF,
                     (VOS_VOID *) pstSdfInfoCnf,
                     sizeof(TAF_PS_SDF_INFO_CNF_STRU));

    /* 释放用于存放SDF参数的内存 */
    PS_MEM_FREE(WUEPS_PID_TAF, pstSdfInfoCnf);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
VOS_UINT8 TAF_APS_LCCheckReqValid(
    TAF_CTRL_STRU                       *pstCtrlInfo
)
{
    TAF_PH_MODE                         ucPhMode;

    ucPhMode                            = MMA_GetCurrPhMode();

    /* 非开机状态，不做限制 */
    if (TAF_PH_MODE_FULL != ucPhMode)
    {
        return VOS_TRUE;
    }

    /* 在L+C互操作场景下，非CMMCA下发请求不做处理 */
    if ((VOS_TRUE        == TAF_SDC_IsConfigCLInterWork())
     && (WUEPS_PID_CMMCA != pstCtrlInfo->ulModuleId))
    {
        return VOS_FALSE;
    }

    /* 在非L+C互操作场景下，CMMCA下发请求不做处理 */
    if ((VOS_FALSE       == TAF_SDC_IsConfigCLInterWork())
     && (WUEPS_PID_CMMCA == pstCtrlInfo->ulModuleId))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
