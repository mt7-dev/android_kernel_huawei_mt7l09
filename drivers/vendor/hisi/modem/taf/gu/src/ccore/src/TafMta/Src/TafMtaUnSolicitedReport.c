

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "TafMtaUnSolicitedReport.h"


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID                 PS_FILE_ID_TAF_MTA_UNSILICITED_REPORT_C
/*lint +e767 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID TAF_MTA_RcvAtSetCurcNotify(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                            *pstAtMtaMsg             = VOS_NULL_PTR;
    AT_MTA_CURC_SET_NOTIFY_STRU                *pstAtMtaSetCurcNotify   = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU                 *pstCurcRptCtrl          = VOS_NULL_PTR;

    pstAtMtaMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstAtMtaSetCurcNotify   = (AT_MTA_CURC_SET_NOTIFY_STRU *)pstAtMtaMsg->aucContent;
    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();

    /* 更新CURC全局变量 */
    pstCurcRptCtrl->enStatusRptGeneralControl = pstAtMtaSetCurcNotify->enCurcRptType;
    PS_MEM_CPY(pstCurcRptCtrl->aucRptCfg, pstAtMtaSetCurcNotify->aucRptCfg, sizeof(pstCurcRptCtrl->aucRptCfg));

    /* 给STK发送消息通知本次CURC设置 */
    TAF_MTA_SndStkCurcRptCfgInfoInd(pstAtMtaSetCurcNotify);

    return;
}


VOS_VOID TAF_MTA_RcvAtQryCurcReq(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                            *pstAtMtaMsg             = VOS_NULL_PTR;
    TAF_SDC_CURC_RPT_CTRL_STRU                 *pstCurcRptCtrl          = VOS_NULL_PTR;
    MTA_AT_CURC_QRY_CNF_STRU                    stQryCurcCnf;

    pstAtMtaMsg             = (AT_MTA_MSG_STRU *)pMsg;

    /* 获取CURC全局变量 */
    pstCurcRptCtrl          = TAF_SDC_GetCurcRptCtrl();

    /* 填充消息内容 */
    PS_MEM_SET(&stQryCurcCnf, 0x00, sizeof(stQryCurcCnf));
    stQryCurcCnf.enResult       = MTA_AT_RESULT_NO_ERROR;
    stQryCurcCnf.enCurcRptType  = (AT_MTA_RPT_GENERAL_CTRL_TYPE_ENUM_UINT8)
                                    pstCurcRptCtrl->enStatusRptGeneralControl;

    PS_MEM_CPY(stQryCurcCnf.aucRptCfg, pstCurcRptCtrl->aucRptCfg, sizeof(stQryCurcCnf.aucRptCfg));

    /* 给AT发送查询CURC回复 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)&(pstAtMtaMsg->stAppCtrl),
                     ID_MTA_AT_CURC_QRY_CNF,
                     sizeof(stQryCurcCnf),
                     (VOS_UINT8*)&stQryCurcCnf);

    return;
}
VOS_VOID TAF_MTA_RcvAtSetUnsolicitedRptReq(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                            *pstAtMtaMsg             = VOS_NULL_PTR;
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU        *pstSetUnsolicitedRptReq = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_SET_CNF_STRU         stSetUnsolicitedRptCnf;
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex;
    VOS_UINT32                                  ulRptCmdStatus;

    pstAtMtaMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstSetUnsolicitedRptReq = (AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU *)pstAtMtaMsg->aucContent;

    /* 更新单个命令设置主动上报的全局变量 */
    switch (pstSetUnsolicitedRptReq->enReqType)
    {
        case AT_MTA_SET_MODE_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_MODE;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucModeRptFlg;
            break;

        case AT_MTA_SET_SRVST_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_SRVST;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucSrvstRptFlg;
            break;

        case AT_MTA_SET_RSSI_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_RSSI;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucRssiRptFlg;
            break;

        case AT_MTA_SET_TIME_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_TIME;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucTimeRptFlg;
            break;

        case AT_MTA_SET_CTZR_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_CTZV;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucCtzrRptFlg;
            break;

        /* DSFLOWRPT设置命令通过ID_MSG_TAF_PS_CONFIG_DSFLOW_RPT_REQ到APS更新，此处删除 */

        case AT_MTA_SET_SIMST_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_SIMST;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucSimstRptFlg;
            break;


        case AT_MTA_SET_CREG_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_CREG;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucCregRptFlg;
            break;

        case AT_MTA_SET_CGREG_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_CGREG;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucCgregRptFlg;
            break;

        case AT_MTA_SET_CEREG_RPT_TYPE:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_CEREG;
            ulRptCmdStatus  = (VOS_UINT32)pstSetUnsolicitedRptReq->u.ucCeregRptFlg;
            break;

        default:
            enRptCmdIndex   = TAF_SDC_RPT_CMD_BUTT;
            ulRptCmdStatus  = VOS_TRUE;
            break;
    }

    TAF_SDC_UpdateRptCmdStatus(enRptCmdIndex, ulRptCmdStatus);

    /* 填充消息内容 */
    PS_MEM_SET(&stSetUnsolicitedRptCnf, 0x00, sizeof(stSetUnsolicitedRptCnf));
    stSetUnsolicitedRptCnf.enResult = MTA_AT_RESULT_NO_ERROR;

    /* 给AT发送设置回复 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)&(pstAtMtaMsg->stAppCtrl),
                     ID_MTA_AT_UNSOLICITED_RPT_SET_CNF,
                     sizeof(stSetUnsolicitedRptCnf),
                     (VOS_UINT8*)&stSetUnsolicitedRptCnf);

    return;
}
VOS_VOID TAF_MTA_RcvAtQryUnsolicitedRptReq(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                            *pstAtMtaMsg             = VOS_NULL_PTR;
    AT_MTA_UNSOLICITED_RPT_QRY_REQ_STRU        *pstQryUnsolicitedRptReq = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU          *pstUnsolicitedRptCtrl   = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_QRY_CNF_STRU         stQryUnsolicitedRptCnf;

    pstAtMtaMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstQryUnsolicitedRptReq     = (AT_MTA_UNSOLICITED_RPT_QRY_REQ_STRU *)pstAtMtaMsg->aucContent;

    /* 获取单个命令主动上报的全局变量 */
    pstUnsolicitedRptCtrl   = TAF_SDC_GetUnsolicitedRptCtrl();

    PS_MEM_SET(&stQryUnsolicitedRptCnf, 0x00, sizeof(stQryUnsolicitedRptCnf));
    stQryUnsolicitedRptCnf.enReqType = pstQryUnsolicitedRptReq->enReqType;
    stQryUnsolicitedRptCnf.enResult  = MTA_AT_RESULT_NO_ERROR;

    switch (pstQryUnsolicitedRptReq->enReqType)
    {
        case AT_MTA_QRY_TIME_RPT_TYPE:
            stQryUnsolicitedRptCnf.u.ucTimeRptFlg =
                        (VOS_UINT8)TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                                TAF_SDC_RPT_CMD_TIME);
            break;

        case AT_MTA_QRY_CTZR_RPT_TYPE:
            stQryUnsolicitedRptCnf.u.ucCtzvRptFlg =
                    (VOS_UINT8)TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CTZV);

            break;

        case AT_MTA_QRY_CSSN_RPT_TYPE:
            stQryUnsolicitedRptCnf.u.stCssnRptFlg.ucCssiRptFlg =
                    (VOS_UINT8)TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CSSI);

            stQryUnsolicitedRptCnf.u.stCssnRptFlg.ucCssuRptFlg =
                    (VOS_UINT8)TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CSSU);
            break;

        case AT_MTA_QRY_CUSD_RPT_TYPE:
            stQryUnsolicitedRptCnf.u.ucCusdRptFlg =
                    (VOS_UINT8)TAF_SDC_CheckRptCmdStatus(pstUnsolicitedRptCtrl->aucRptCfg,
                                            TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,
                                            TAF_SDC_RPT_CMD_CUSD);
            break;

         default:
            break;
    }

    /* 给AT发送查询回复 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)&(pstAtMtaMsg->stAppCtrl),
                     ID_MTA_AT_UNSOLICITED_RPT_QRY_CNF,
                     sizeof(stQryUnsolicitedRptCnf),
                     (VOS_UINT8*)&stQryUnsolicitedRptCnf);

    return;
}


VOS_VOID TAF_MTA_RcvTafAgentGetSysModeReq(VOS_VOID *pMsg)
{
    TAF_SDC_CAMP_PLMN_INFO_STRU                *pstCampPlmnInfo     = VOS_NULL_PTR;
    TAFAGENT_MTA_GET_SYSMODE_CNF_STRU          *pstGetSysModeCnf    = VOS_NULL_PTR;

    /* 获取系统模式 */
    pstCampPlmnInfo   = TAF_SDC_GetCampPlmnInfo();

    pstGetSysModeCnf = (TAFAGENT_MTA_GET_SYSMODE_CNF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA,
                                                              sizeof(TAFAGENT_MTA_GET_SYSMODE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstGetSysModeCnf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvTafAgentGetSysModeReq:alloc msg failed.");
        return;
    }

    /* 消息头 */
    pstGetSysModeCnf->ulSenderCpuId                 = VOS_LOCAL_CPUID;
    pstGetSysModeCnf->ulSenderPid                   = UEPS_PID_MTA;
    pstGetSysModeCnf->ulReceiverCpuId               = VOS_LOCAL_CPUID;
    pstGetSysModeCnf->ulReceiverPid                 = ACPU_PID_TAFAGENT;

    /* 消息内容 */
    pstGetSysModeCnf->enMsgId                 = ID_TAFAGENT_MTA_GET_SYSMODE_CNF;
    pstGetSysModeCnf->ulRet                   = VOS_OK;
    pstGetSysModeCnf->stSysMode.enSysSubMode  = pstCampPlmnInfo->enSysSubMode;
    pstGetSysModeCnf->stSysMode.enRatType     = pstCampPlmnInfo->enSysMode;

    /* 给TAFAGENT发送查询回复 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstGetSysModeCnf))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvTafAgentGetSysModeReq:send msg failed.");

        return;
    }

    return;
}



VOS_VOID TAF_MTA_SndStkCurcRptCfgInfoInd(AT_MTA_CURC_SET_NOTIFY_STRU *pMsg)
{
    TAF_STK_CURC_RPT_CFG_INFO_MSG_STRU          *pstStkCurcRptInfo  = VOS_NULL_PTR;

    /* 分配内存 */
    pstStkCurcRptInfo = (TAF_STK_CURC_RPT_CFG_INFO_MSG_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA,
                                                              sizeof(TAF_STK_CURC_RPT_CFG_INFO_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 内存分配异常处理 */
    if (VOS_NULL_PTR == pstStkCurcRptInfo)
    {
        MTA_WARNING_LOG("TAF_MTA_SndStkCurcRptCfgInfoInd:alloc msg failed.");
        return;
    }

    /* 填写新消息内容 */
    pstStkCurcRptInfo->ulSenderPid      = UEPS_PID_MTA;
    pstStkCurcRptInfo->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstStkCurcRptInfo->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstStkCurcRptInfo->ulReceiverPid    = MAPS_STK_PID;
    pstStkCurcRptInfo->ulMsgId          = TAF_STK_CURC_RPT_CFG_INFO_IND;
    PS_MEM_CPY(pstStkCurcRptInfo->aucCurcRptCfg,
               pMsg->aucRptCfg,
               sizeof(pstStkCurcRptInfo->aucCurcRptCfg));

    /* 发送消息异常处理 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstStkCurcRptInfo))
    {
        MTA_WARNING_LOG("TAF_MTA_SndStkCurcRptCfgInfoInd: PS_SEND_MSG ERROR");
        return;
    }

    return;
}

#ifdef DMT

VOS_UINT32 DMT_GetSysMode(TAF_AGENT_SYS_MODE_STRU *pstSysMode)
{
    TAF_SDC_CAMP_PLMN_INFO_STRU                *pstCampPlmnInfo     = VOS_NULL_PTR;

    /* 获取系统模式 */
    pstCampPlmnInfo   = TAF_SDC_GetCampPlmnInfo();

    pstSysMode->enSysSubMode  = pstCampPlmnInfo->enSysSubMode;
    pstSysMode->enRatType     = pstCampPlmnInfo->enSysMode;

    return VOS_OK;
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

