

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafMtaPhy.h"
#include "TafMtaMain.h"
#include "TafMtaComm.h"
#include "TafSdcCtx.h"
#include "TafMtaMntn.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MTA_PHY_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 TAF_MTA_SndPhyBodySarReqMsg(
    MTA_BODY_SAR_STATE_ENUM_UINT16      enState,
    MTA_BODY_SAR_PARA_STRU             *pstBodySarPara
)
{
    VOS_UINT8                           i;
    VOS_UINT32                          ulLength;
    MTA_APM_BODY_SAR_SET_REQ_STRU      *pstMtaPhyBodySarReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength            = sizeof(MTA_APM_BODY_SAR_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaPhyBodySarReq = (MTA_APM_BODY_SAR_SET_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaPhyBodySarReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhyBodySarReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_UINT8*)pstMtaPhyBodySarReq + VOS_MSG_HEAD_LENGTH, 0x00, ulLength);

    /* 构造消息结构体 */
    pstMtaPhyBodySarReq->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstMtaPhyBodySarReq->ulSenderPid        = UEPS_PID_MTA;
    pstMtaPhyBodySarReq->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstMtaPhyBodySarReq->ulReceiverPid      = DSP_PID_APM;
    pstMtaPhyBodySarReq->usMsgID            = ID_MTA_APM_BODY_SAR_SET_REQ;
    pstMtaPhyBodySarReq->enState            = enState;

    PS_MEM_CPY((VOS_UINT8*)&pstMtaPhyBodySarReq->stBodySARPara, pstBodySarPara,  sizeof(MTA_BODY_SAR_PARA_STRU));

    /* 实际传到PHY的功率门限值为用户配置的10倍 */
    for (i = 0; i < MTA_BODY_SAR_GBAND_MAX_NUM; i++)
    {
        pstMtaPhyBodySarReq->stBodySARPara.astGBandPara[i].sGPRSPower   *= 10;
        pstMtaPhyBodySarReq->stBodySARPara.astGBandPara[i].sEDGEPower   *= 10;
    }

    for (i = 0; i < MTA_BODY_SAR_WBAND_MAX_NUM; i++)
    {
        pstMtaPhyBodySarReq->stBodySARPara.astWBandPara[i].sPower       *= 10;
    }

    /* 发送消息到 PHY */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaPhyBodySarReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhyBodySarReqMsg(): WARNING:SEND MSG FIAL");
    }

    return VOS_OK;
}


VOS_VOID TAF_MTA_RcvAtBodySarSetReq(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                    *pstBodySarReqMsg    = VOS_NULL_PTR;
    AT_MTA_BODY_SAR_SET_REQ_STRU       *pstBodySarReqPara   = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU              stBodySarCnf;
    VOS_UINT32                          ulResult;

    /* 局部变量初始化 */
    pstBodySarReqMsg    = (AT_MTA_MSG_STRU*)pMsg;
    pstBodySarReqPara   = (AT_MTA_BODY_SAR_SET_REQ_STRU*)pstBodySarReqMsg->aucContent;

    /* 如果当前定时器已启动，则直接返回失败 */
    if (TAF_MTA_TIMER_STATUS_STOP != TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF))
    {
        stBodySarCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstBodySarReqMsg->stAppCtrl,
                         ID_MTA_AT_BODY_SAR_SET_CNF,
                         sizeof(stBodySarCnf),
                         (VOS_UINT8*)&stBodySarCnf );
        return;
    }

    /* 收到at的BODYSAR设置请求，发ID_MTA_APM_BODY_SAR_SET_REQ消息通知PHY */
    ulResult = TAF_MTA_SndPhyBodySarReqMsg(pstBodySarReqPara->enState,
                                           &pstBodySarReqPara->stBodySARPara);

    /* 消息发送失败，给AT回复ERROR */
    if (VOS_ERR == ulResult)
    {
        stBodySarCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstBodySarReqMsg->stAppCtrl,
                          ID_MTA_AT_BODY_SAR_SET_CNF,
                          sizeof(stBodySarCnf),
                          (VOS_UINT8*)&stBodySarCnf );
        return;
    }

    /* 启动保护定时器 */
    if (TAF_MTA_TIMER_START_FAILURE == TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF, TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF_TIMER_LEN))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvAtBodySarSetReq: WARNING: Start Timer failed!");
    }

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF,
                                     (VOS_UINT8*)&pstBodySarReqMsg->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}


VOS_VOID TAF_MTA_RcvPhyBodySarSetCnf(VOS_VOID *pMsg)
{
    APM_MTA_BODY_SAR_SET_CNF_STRU      *pstBodySarCnfMsg    = VOS_NULL_PTR;
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf           = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU              stBodySarCnf;

    pstBodySarCnfMsg = (APM_MTA_BODY_SAR_SET_CNF_STRU*)pMsg;

    /* 如果当前定时器不为运行状态 */
    if (TAF_MTA_TIMER_STATUS_RUNING != TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvPhyBodySarSetCnf: WARNING: Timer was already stop!");
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    /* 将PHY APM返回结果转换后发给AT */
    if (MTA_PHY_RESULT_NO_ERROR == pstBodySarCnfMsg->enResult)
    {
        stBodySarCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }
    else
    {
        stBodySarCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU*)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_BODY_SAR_SET_CNF,
                     sizeof(stBodySarCnf),
                     (VOS_UINT8*)&stBodySarCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF);

    return;
}
VOS_VOID TAF_MTA_RcvTiWaitPhySetBodySarExpired(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU              stBodySarCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    PS_MEM_SET(&stBodySarCnf, 0x0, sizeof(stBodySarCnf));

    stBodySarCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 把接入层上报的nmr数据上报给AT模块 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_BODY_SAR_SET_CNF,
                     sizeof(stBodySarCnf),
                     (VOS_UINT8*)&stBodySarCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_SET_BODYSAR_CNF);

    return;
}
VOS_UINT32 TAF_MTA_SndPhyEmergencyCallStatus(
    MTA_PHY_EMERGENCY_CALL_STATUS_ENUM_UINT16               enState
)
{
    MTA_PHY_EMERGENCY_CALL_STATUS_NOTIFY_STRU              *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_PHY_EMERGENCY_CALL_STATUS_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_PHY_EMERGENCY_CALL_STATUS_NOTIFY_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhyEmergencyCallStatus: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid           = DSP_PID_APM;
    pstMsg->ulLength                = ulLength;
    pstMsg->usMsgID                 = ID_MTA_APM_EMERGENCY_CALL_STATUS_NOTIFY;
    pstMsg->enEmergencyCallStatus   = enState;
    PS_MEM_SET(pstMsg->ausReserved, 0x00, sizeof(pstMsg->ausReserved));
    PS_MEM_SET(pstMsg->ausReserved1, 0x00, sizeof(pstMsg->ausReserved1));

    /*发送消息到OAM的指定PID ;*/
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MN_ERR_LOG("TAF_MTA_SndPhyEmergencyCallStatus: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID TAF_MTA_RcvAtSetHandleDectReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstSetReq       = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_SET_CNF_STRU      stSetCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usHandleType;

    pstSetReq          = (AT_MTA_MSG_STRU *)pMsg;
    ulResult           = MTA_AT_RESULT_ERROR;
    usHandleType       = pstSetReq->aucContent[0];
    PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF))
    {
        stSetCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                      ID_MTA_AT_HANDLEDECT_SET_CNF,
                      sizeof(stSetCnf),
                      (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 收到at命令请求，发请求消息通知APM */
    ulResult = TAF_MTA_SndPhySetHandleDectReqMsg(usHandleType);

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        stSetCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_HANDLEDECT_SET_CNF,
                         sizeof(stSetCnf),
                         (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF,
                        TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF,
                                     (VOS_UINT8*)&pstSetReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvAtQryHandleDectReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq       = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_QRY_CNF_STRU      stQryCnf;
    VOS_UINT32                          ulResult;

    pstQryReq           = (AT_MTA_MSG_STRU *)pMsg;
    ulResult            = MTA_AT_RESULT_ERROR;
    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF))
    {
        stQryCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_HANDLEDECT_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 收到at命令请求，发请求消息通知APM */
    ulResult = TAF_MTA_SndPhyQryHandleDectReqMsg();

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        stQryCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_HANDLEDECT_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF,
                        TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;

}
VOS_UINT32 TAF_MTA_SndPhySetHandleDectReqMsg(
    VOS_UINT16                          usHandleType
)
{
    VOS_UINT32                          ulLength;
    MTA_APM_HANDLE_DETECT_SET_REQ_STRU *pstMtaPhyReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength            = sizeof(MTA_APM_HANDLE_DETECT_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaPhyReq        = (MTA_APM_HANDLE_DETECT_SET_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaPhyReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhySetHandleDectReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_UINT8*)pstMtaPhyReq + VOS_MSG_HEAD_LENGTH, 0x00, ulLength);

    /* 构造消息结构体 */
    pstMtaPhyReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaPhyReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaPhyReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaPhyReq->ulReceiverPid     = DSP_PID_APM;
    pstMtaPhyReq->usMsgID           = ID_MTA_APM_HANDLE_DETECT_SET_REQ;
    pstMtaPhyReq->usHandle          = usHandleType;

    /* 发送消息到 PHY */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaPhyReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhySetHandleDectReqMsg(): WARNING:SEND MSG FIAL");
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndPhyQryHandleDectReqMsg(VOS_VOID)
{
    VOS_UINT32                          ulLength;
    MTA_APM_HANDLE_DETECT_QRY_REQ_STRU *pstMtaPhyReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength            = sizeof(MTA_APM_HANDLE_DETECT_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaPhyReq        = (MTA_APM_HANDLE_DETECT_QRY_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaPhyReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhyQryHandleDectReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_UINT8*)pstMtaPhyReq + VOS_MSG_HEAD_LENGTH, 0x00, ulLength);

    /* 构造消息结构体 */
    pstMtaPhyReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaPhyReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaPhyReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaPhyReq->ulReceiverPid     = DSP_PID_APM;
    pstMtaPhyReq->usMsgID           = ID_MTA_APM_HANDLE_DETECT_QRY_REQ;

    /* 发送消息到 PHY */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaPhyReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndPhyQryHandleDectReqMsg(): WARNING:SEND MSG FIAL");
    }

    return VOS_OK;
}
VOS_VOID TAF_MTA_RcvPhyHandleDectSetCnf(
    VOS_VOID                           *pMsg
)
{
    APM_MTA_HANDLE_DETECT_SET_CNF_STRU     *pstApmMtaCnf   = VOS_NULL_PTR;
    TAF_MTA_CMD_BUFFER_STRU                *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_SET_CNF_STRU          stMtaAtCnf;

    pstApmMtaCnf = (APM_MTA_HANDLE_DETECT_SET_CNF_STRU *)pMsg;

    /* 如果当前定时器不为运行状态 */
    if (TAF_MTA_TIMER_STATUS_RUNING != TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvPhyHandleDectSetCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    /* 将PHY APM返回结果转换后发给AT */
    if (MTA_PHY_RESULT_NO_ERROR == pstApmMtaCnf->enResult)
    {
        stMtaAtCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }
    else
    {
        stMtaAtCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU*)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_HANDLEDECT_SET_CNF,
                     sizeof(stMtaAtCnf),
                     (VOS_UINT8*)&stMtaAtCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF);

    return;
}
VOS_VOID TAF_MTA_RcvPhyHandleDectQryCnf(
    VOS_VOID                           *pMsg
)
{
    APM_MTA_HANDLE_DETECT_QRY_CNF_STRU     *pstApmMtaCnf   = VOS_NULL_PTR;
    TAF_MTA_CMD_BUFFER_STRU                *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_QRY_CNF_STRU          stMtaAtCnf;

    pstApmMtaCnf = (APM_MTA_HANDLE_DETECT_QRY_CNF_STRU *)pMsg;
    PS_MEM_SET((VOS_UINT8*)&stMtaAtCnf, 0x00, sizeof(MTA_AT_HANDLEDECT_QRY_CNF_STRU));

    /* 如果当前定时器不为运行状态 */
    if (TAF_MTA_TIMER_STATUS_RUNING != TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvPhyHandleDectQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    /* 将PHY APM返回结果转换后发给AT */
    if (MTA_PHY_RESULT_NO_ERROR == pstApmMtaCnf->enResult)
    {
        stMtaAtCnf.enResult = MTA_AT_RESULT_NO_ERROR;
        stMtaAtCnf.usHandle = pstApmMtaCnf->usHandle;
    }
    else
    {
        stMtaAtCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU*)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_HANDLEDECT_QRY_CNF,
                     sizeof(stMtaAtCnf),
                     (VOS_UINT8*)&stMtaAtCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF);

    return;
}
VOS_VOID TAF_MTA_RcvTiWaitPhySetHandleDectExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_SET_CNF_STRU      stMtaAtSetCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiWaitPhySetHandleDectExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stMtaAtSetCnf, 0x0, sizeof(stMtaAtSetCnf));

    stMtaAtSetCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_HANDLEDECT_SET_CNF,
                     sizeof(stMtaAtSetCnf),
                     (VOS_UINT8*)&stMtaAtSetCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}


VOS_VOID TAF_MTA_RcvTiWaitPhyQryHandleDectExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_QRY_CNF_STRU      stMtaAtQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_PHY_HANDLEDECT_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiWaitPhyQryHandleDectExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stMtaAtQryCnf, 0x0, sizeof(stMtaAtQryCnf));

    stMtaAtQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_HANDLEDECT_QRY_CNF,
                     sizeof(stMtaAtQryCnf),
                     (VOS_UINT8*)&stMtaAtQryCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}


VOS_UINT32 TAF_MTA_CheckRatModeTypeValid(
    AT_MTA_CMD_RATMODE_ENUM_UINT8       enRatMode
)
{
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysCurrMode;

    enSysCurrMode = TAF_SDC_GetSysMode();

    if ((AT_MTA_CMD_RATMODE_GSM == enRatMode)
     && (TAF_SDC_SYS_MODE_GSM   == enSysCurrMode))
    {
        return VOS_OK;
    }

    if ((AT_MTA_CMD_RATMODE_WCDMA == enRatMode)
     && (TAF_SDC_SYS_MODE_WCDMA   == enSysCurrMode))
    {
        return VOS_OK;
    }

    if ((AT_MTA_CMD_RATMODE_LTE == enRatMode)
     && (TAF_SDC_SYS_MODE_LTE   == enSysCurrMode))
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 TAF_MTA_SndGuPhySetDpdtTestFlagNtfMsg(
    AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU  *pstSetDpdtFlagReq
)
{
    MTA_GUPHY_SET_DPDTTEST_FLAG_NTF_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_GUPHY_SET_DPDTTEST_FLAG_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_GUPHY_SET_DPDTTEST_FLAG_NTF_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhySetDpdtTestFlagNtfMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulLength                = ulLength;
    pstMsg->usMsgID                 = ID_MTA_GUPHY_SET_DPDTTEST_FLAG_NTF;

    if (AT_MTA_CMD_RATMODE_GSM   == pstSetDpdtFlagReq->enRatMode)
    {
        pstMsg->ulReceiverPid       = DSP_PID_GPHY;
    }
    else
    {
        pstMsg->ulReceiverPid       = DSP_PID_WPHY;
    }

    pstMsg->usFlag = pstSetDpdtFlagReq->ucFlag;

    /* 发送消息到Gu phy */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhySetDpdtTestFlagNtfMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndGuPhySetDpdtValueNtfMsg(
    AT_MTA_SET_DPDT_VALUE_REQ_STRU     *pstSetDpdtReq
)
{
    MTA_GUPHY_SET_DPDT_VALUE_NTF_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_GUPHY_SET_DPDT_VALUE_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_GUPHY_SET_DPDT_VALUE_NTF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhySetDpdtValueNtfMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulLength                = ulLength;
    pstMsg->usMsgID                 = ID_MTA_GUPHY_SET_DPDT_VALUE_NTF;

    if (AT_MTA_CMD_RATMODE_GSM   == pstSetDpdtReq->enRatMode)
    {
        pstMsg->ulReceiverPid       = DSP_PID_GPHY;
    }
    else
    {
        pstMsg->ulReceiverPid       = DSP_PID_WPHY;
    }

    pstMsg->ulDpdtValue = pstSetDpdtReq->ulDpdtValue;

    /* 发送消息到Gu phy */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhySetDpdtValueNtfMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndGuPhyQryDpdtValueReqMsg(
    AT_MTA_QRY_DPDT_VALUE_REQ_STRU     *pstQryDpdtReq
)
{
    MTA_GUPHY_QRY_DPDT_VALUE_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_GUPHY_QRY_DPDT_VALUE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_GUPHY_QRY_DPDT_VALUE_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhyQryDpdtValueReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulLength                = ulLength;
    pstMsg->usMsgID                 = ID_MTA_GUPHY_QRY_DPDT_VALUE_REQ;

    if (AT_MTA_CMD_RATMODE_GSM   == pstQryDpdtReq->enRatMode)
    {
        pstMsg->ulReceiverPid       = DSP_PID_GPHY;
    }
    else
    {
        pstMsg->ulReceiverPid       = DSP_PID_WPHY;
    }

    /* 发送消息到Gu phy */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndGuPhyQryDpdtValueReqMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID TAF_MTA_RcvAtSetDpdtTestFlagReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq      = VOS_NULL_PTR;
    AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU  *pstAtMtaQryReq = VOS_NULL_PTR;
    MTA_AT_SET_DPDTTEST_FLAG_CNF_STRU   stMtaAtQryCnf;
    VOS_UINT32                          ulResult;

    ulResult               = VOS_ERR;
    stMtaAtQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 解析AT发送过来的消息结构 */
    pstQryReq       = (AT_MTA_MSG_STRU *)pMsg;
    pstAtMtaQryReq  = (AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU *)(pstQryReq->aucContent);

    /* 检查AT命令中下发的RatMode跟当前系统驻留的模式是否一致 */
    if (VOS_OK != TAF_MTA_CheckRatModeTypeValid(pstAtMtaQryReq->enRatMode))
    {
        MTA_ERROR_LOG("TAF_MTA_RcvAtSetDpdtTestFlagReq:RatMode not valid!");

        /* MTA回复AT模块DPDT FLAG设置请求结果 */
        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                        ID_MTA_AT_SET_DPDTTEST_FLAG_CNF,
                        sizeof(stMtaAtQryCnf),
                        (VOS_UINT8 *)&stMtaAtQryCnf);
        return;
    }

    /* 根据RatMode,将消息结构发送至GU phy或者LRRC */
    if ((AT_MTA_CMD_RATMODE_GSM   == pstAtMtaQryReq->enRatMode)
     || (AT_MTA_CMD_RATMODE_WCDMA == pstAtMtaQryReq->enRatMode))
    {
        ulResult = TAF_MTA_SndGuPhySetDpdtTestFlagNtfMsg(pstAtMtaQryReq);
    }

#if (FEATURE_ON == FEATURE_LTE)
    if (AT_MTA_CMD_RATMODE_LTE == pstAtMtaQryReq->enRatMode)
    {
        ulResult = TAF_MTA_SndLrrcSetDpdtTestFlagNtfMsg(pstAtMtaQryReq);
    }
#endif

    if (VOS_OK == ulResult)
    {
        stMtaAtQryCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }

    /* MTA回复AT模块DPDT FLAG设置请求结果 */
    TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                    ID_MTA_AT_SET_DPDTTEST_FLAG_CNF,
                    sizeof(stMtaAtQryCnf),
                    (VOS_UINT8 *)&stMtaAtQryCnf);

    return;
}
VOS_VOID TAF_MTA_RcvAtSetDpdtValueReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq      = VOS_NULL_PTR;
    AT_MTA_SET_DPDT_VALUE_REQ_STRU     *pstAtMtaQryReq = VOS_NULL_PTR;
    MTA_AT_SET_DPDT_VALUE_CNF_STRU      stMtaAtQryCnf;
    VOS_UINT32                          ulResult;

    ulResult               = VOS_ERR;
    stMtaAtQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 解析AT发送过来的消息结构 */
    pstQryReq       = (AT_MTA_MSG_STRU *)pMsg;
    pstAtMtaQryReq  = (AT_MTA_SET_DPDT_VALUE_REQ_STRU *)(pstQryReq->aucContent);


    /* 检查AT命令中下发的RatMode跟当前系统驻留的模式是否一致 */
    if (VOS_OK != TAF_MTA_CheckRatModeTypeValid(pstAtMtaQryReq->enRatMode))
    {
        MTA_ERROR_LOG("TAF_MTA_RcvAtSetDpdtValueReq:RatMode not valid!");

        /* MTA回复AT模块DPDT FLAG设置请求结果 */
        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                        ID_MTA_AT_SET_DPDT_VALUE_CNF,
                        sizeof(stMtaAtQryCnf),
                        (VOS_UINT8 *)&stMtaAtQryCnf);
        return;
    }

    /* 根据RatMode,将消息结构发送至GU phy或者LRRC */
    if ((AT_MTA_CMD_RATMODE_GSM   == pstAtMtaQryReq->enRatMode)
     || (AT_MTA_CMD_RATMODE_WCDMA == pstAtMtaQryReq->enRatMode))
    {
        ulResult = TAF_MTA_SndGuPhySetDpdtValueNtfMsg(pstAtMtaQryReq);
    }

#if (FEATURE_ON == FEATURE_LTE)
    if (AT_MTA_CMD_RATMODE_LTE == pstAtMtaQryReq->enRatMode)
    {
        ulResult = TAF_MTA_SndLrrcSetDpdtValueNtfMsg(pstAtMtaQryReq);
    }
#endif

    if (VOS_OK == ulResult)
    {
        stMtaAtQryCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }

    /* MTA回复AT模块DPDT VALUE设置请求结果 */
    TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                    ID_MTA_AT_SET_DPDT_VALUE_CNF,
                    sizeof(stMtaAtQryCnf),
                    (VOS_UINT8 *)&stMtaAtQryCnf);

    return;
}
VOS_VOID TAF_MTA_RcvAtQryDpdtValueReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq      = VOS_NULL_PTR;
    AT_MTA_QRY_DPDT_VALUE_REQ_STRU     *pstAtMtaQryReq = VOS_NULL_PTR;
    MTA_AT_QRY_DPDT_VALUE_CNF_STRU      stMtaAtQryCnf;
    VOS_UINT32                          ulResult;

    ulResult = VOS_ERR;
    PS_MEM_SET(&stMtaAtQryCnf, 0x0, sizeof(stMtaAtQryCnf));
    stMtaAtQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 解析AT发送过来的消息结构 */
    pstQryReq       = (AT_MTA_MSG_STRU *)pMsg;
    pstAtMtaQryReq  = (AT_MTA_QRY_DPDT_VALUE_REQ_STRU *)(pstQryReq->aucContent);

    /* 如果定时器TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF))
    {
        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                        ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                        sizeof(stMtaAtQryCnf),
                        (VOS_UINT8 *)&stMtaAtQryCnf );
        return;
    }

    /* 检查AT命令中下发的RatMode跟当前系统驻留的模式是否一致 */
    if (VOS_OK != TAF_MTA_CheckRatModeTypeValid(pstAtMtaQryReq->enRatMode))
    {
        MTA_ERROR_LOG("TAF_MTA_RcvAtQryDpdtValueReq:RatMode not valid!");

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                        ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                        sizeof(stMtaAtQryCnf),
                        (VOS_UINT8 *)&stMtaAtQryCnf);
        return;

    }


    /* 根据RatMode,将消息结构发送至GU phy或者LRRC */
    if ((AT_MTA_CMD_RATMODE_GSM   == pstAtMtaQryReq->enRatMode)
     || (AT_MTA_CMD_RATMODE_WCDMA == pstAtMtaQryReq->enRatMode))
    {
        ulResult = TAF_MTA_SndGuPhyQryDpdtValueReqMsg(pstAtMtaQryReq);
    }

#if (FEATURE_ON == FEATURE_LTE)
    if (AT_MTA_CMD_RATMODE_LTE == pstAtMtaQryReq->enRatMode)
    {
        ulResult = TAF_MTA_SndLrrcQryDpdtValueReqMsg(pstAtMtaQryReq);
    }
#endif

    if (VOS_OK != ulResult)
    {
        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                        ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                        sizeof(stMtaAtQryCnf),
                        (VOS_UINT8 *)&stMtaAtQryCnf);

        return;
    }

    /* 启动保护定时器TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF,
                        TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}


VOS_VOID TAF_MTA_RcvGuPhyQryDpdtValueCnf(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf         = VOS_NULL_PTR;
    GUPHY_MTA_QRY_DPDT_VALUE_CNF_STRU  *pstGuPhyMtaQryCnf = VOS_NULL_PTR;
    MTA_AT_QRY_DPDT_VALUE_CNF_STRU      stMtaAtQryCnf;
    TAF_MTA_TIMER_ID_ENUM_UINT32        enTimerId;

    /* 局部变量初始化 */
    pstGuPhyMtaQryCnf = (GUPHY_MTA_QRY_DPDT_VALUE_CNF_STRU *)pMsg;
    enTimerId = TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF;

    if (TAF_MTA_TIMER_STATUS_RUNING != TAF_MTA_GetTimerStatus(enTimerId))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvGuPhyQryDpdtValueCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(enTimerId);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(enTimerId);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    /* 构造消息结构体 */
    stMtaAtQryCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    stMtaAtQryCnf.ulDpdtValue = pstGuPhyMtaQryCnf->ulDpdtValue;

    /* 发送消息给AT模块 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                      ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                      sizeof(MTA_AT_QRY_DPDT_VALUE_CNF_STRU),
                      (VOS_UINT8 *)&stMtaAtQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}


VOS_VOID TAF_MTA_RcvTiReqDpdtValueQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_QRY_DPDT_VALUE_CNF_STRU      stQryDpdtValueCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    PS_MEM_SET(&stQryDpdtValueCnf, 0x0, sizeof(stQryDpdtValueCnf));

    stQryDpdtValueCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                     sizeof(stQryDpdtValueCnf),
                     (VOS_UINT8 *)&stQryDpdtValueCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF);

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


