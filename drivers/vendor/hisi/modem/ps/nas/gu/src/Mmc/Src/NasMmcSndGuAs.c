
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "NasCommDef.h"
#include "NasMmcSndGuAs.h"
#include "NasComm.h"
#include "NasMmSublayerDef.h"
#include "NasMmcCtx.h"
#include "NasMmlCtx.h"
#include "product_config.h"
#include "Nasrrcinterface.h"
#include "MmaMmcInterface.h"
#include "NasMmlLib.h"
#include "NasUtranCtrlInterface.h"
#include "NasUtranCtrlCommFunc.h"
#include "NasMmcComFunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_SND_GUAS_C

/*****************************************************************************
  2 全局变量
*****************************************************************************/
/*****************************************************************************
  3 宏定义
*****************************************************************************/


VOS_VOID NAS_MMC_SndAsPowerOffReq(
    VOS_UINT32                          ulReceiverPid
)
{
    VOS_UINT32                          ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RRMM_POWER_OFF_REQ_STRU            *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RRMM_POWER_OFF_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasPowerOffReq:ERROR: Memory Alloc Error for pMsg");
        return ;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(RRMM_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = ulReceiverPid;
    pstMsg->MsgHeader.ulLength          = sizeof(RRMM_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = RRMM_POWER_OFF_REQ;

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasPowerOffReq:ERROR:PS_SEND_MSG ");
        return ;
    }

    vos_printf("NAS_MMC_SndAsPowerOffReq, ulReceiverPid:%d, ulRet:%d. tick = %d\r\n ", ulReceiverPid, ulRet,OM_GetSlice());

    NAS_TRACE_HIGH("Send poweroff req to %s!",
        (WUEPS_PID_WRR == ulReceiverPid) ? ((NAS_UTRANCTRL_UTRAN_MODE_FDD == NAS_UTRANCTRL_GetCurrUtranMode()) ? "WRRC" : "TRRC") : "GAS");

    return ;
}

VOS_VOID NAS_MMC_SndAsPlmnAnyCellSrchReq(VOS_UINT32 ulPid)
{
    /* 返回值变量 */
    VOS_UINT32                                              ulRet;
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU                stForbRoamLaInfo;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                        *pstForbPlmnInfo = VOS_NULL_PTR;


    /* 发送 PLMN SPEC 搜索请求变量 */
    RRMM_PLMN_SEARCH_REQ_STRU          *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                           sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        return;
    }

    /* 构造发送消息 */
    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
        sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid       = ulPid;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName           = RRMM_PLMN_SEARCH_REQ;

    pstMsg->ulSearchType         = RRC_PLMN_SEARCH_SPEC;
    pstMsg->ulPlmnIdNum          = 0;
    pstMsg->aPlmnIdList[0].ulMcc = 0;
    pstMsg->aPlmnIdList[0].ulMnc = 0;

    /* 获取全局变量中的FORB ROAM LA 信息 */
    pstForbPlmnInfo = NAS_MML_GetForbidPlmnInfo();

    stForbRoamLaInfo.ucForbRoamLaNum = pstForbPlmnInfo->ucForbRoamLaNum;
    PS_MEM_CPY( stForbRoamLaInfo.astForbRomLaList,
                pstForbPlmnInfo->astForbRomLaList,
                sizeof(pstForbPlmnInfo->astForbRomLaList) );

    /* 添加禁止LAI信息 */
    NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg(&(pstMsg->ulForbLaNum),
                                    pstMsg->aForbLaList,
                                    &stForbRoamLaInfo);

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsPlmnAnyCellSrchReq():WARNING:SEND AS PLMN ANYCELL SEARCH REQ FIAL");
        return;
    }

}


VOS_VOID NAS_MMC_SndAsSuspendReq(VOS_UINT32 ulPid)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;
    /* 定义原语类型指针 */
    RRMM_SUSPEND_REQ_ST                *pstMsg = VOS_NULL_PTR;

    pstMsg = (RRMM_SUSPEND_REQ_ST *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(RRMM_SUSPEND_REQ_ST));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendReq():ERROR:Memory Alloc Error for pstMsg");
        /* 返回失败 */
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(RRMM_SUSPEND_REQ_ST) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId             = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid               = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid             = ulPid;
    pstMsg->MsgHeader.ulMsgName                 = RRMM_SUSPEND_REQ;
    pstMsg->ucStopCause                         = MMC_AS_SUSPEND_STOP_DSP;

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndASSuspendReq():ERROR:Message sends fail");
        return;
    }

#ifndef __PS_WIN32_RECUR__
    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif

}



VOS_VOID NAS_MMC_SndAsPlmnSrchStopReq(VOS_UINT32 ulPid)
{
    VOS_UINT32                           ulRet;
    RRMM_PLMN_SEARCH_STOP_REQ_ST        *pstMsg = VOS_NULL_PTR;

    /*申请消息  */
    pstMsg = (RRMM_PLMN_SEARCH_STOP_REQ_ST*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                         WUEPS_PID_MMC,
                                            sizeof(RRMM_PLMN_SEARCH_STOP_REQ_ST));

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsPlmnSrchStopReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
        sizeof(RRMM_PLMN_SEARCH_STOP_REQ_ST) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulMsgName         = RRMM_PLMN_SEARCH_STOP_REQ;
    pstMsg->MsgHeader.ulReceiverPid     = ulPid;

    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    if( VOS_OK != ulRet)
    {
        /* 发送失败 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsPlmnSrchStopReq():WARNING:SEND AS PLMN SEARCH STOP REQ FIAL");
    }

    #ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
    #endif

    return;
}



VOS_UINT32 NAS_MMC_SndGuAsEquPlmnReq(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEplmnInfo,
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType
)
{
    RRMM_EQU_PLMN_NOTIFY_REQ_STRU                          *pstMsg        = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_EQU_PLMN_NOTIFY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_EQU_PLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGuAsEquPlmnReq():ERROR:ALLOC MEM EQU PLMN LIST FIAL");
        /* 返回 */
        return VOS_FALSE;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(RRMM_EQU_PLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid         = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_EQU_PLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;

    pstMsg->MsgHeader.ulMsgName           = RRMM_EQU_PLMN_NOTIFY_REQ;

    if (NAS_MML_NET_RAT_TYPE_GSM == enRatType)
    {
        pstMsg->MsgHeader.ulReceiverPid       = UEPS_PID_GAS;
    }
    else
    {
        pstMsg->MsgHeader.ulReceiverPid       = WUEPS_PID_WRR;
    }

    if (VOS_NULL_PTR != pstEplmnInfo)
    {
        pstMsg->ulEquPlmnNum = pstEplmnInfo->ucEquPlmnNum;
        PS_MEM_CPY(pstMsg->aEquPlmnIdList, pstEplmnInfo->astEquPlmnAddr, sizeof(pstMsg->aEquPlmnIdList));
    }

    /* 调用VOS发送原语 */
    if ( VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg) )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGuAsEquPlmnReq():WARNING:SEND AS EPLMN REQ FIAL");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID NAS_MMC_SndAsStartReq(VOS_UINT32 ulReceiverPid)
{
    VOS_UINT32                      ulRet;
    RRMM_START_REQ_STRU            *pstMsg      = VOS_NULL_PTR;
    NAS_MML_PLMN_RAT_PRIO_STRU     *pstRatList  = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_START_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsStartReq: Alloc memeory Error");
        return ;
    }

    PS_MEM_SET( (VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(RRMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = ulReceiverPid;
    pstMsg->MsgHeader.ulLength          = sizeof(RRMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = RRMM_START_REQ;

    /* 修改调用函数名 */
    pstMsg->ulSimStatus                 = NAS_MML_GetRrcSimStatus();

    /* 取得接入优先级列表 */
    pstRatList  = NAS_MML_GetMsPrioRatList();

    /* 设置接入模式 */
    pstMsg->aucRatPrioList[0]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_GSM, pstRatList);
    pstMsg->aucRatPrioList[1]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_WCDMA, pstRatList);
    pstMsg->aucRatPrioList[2]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_LTE, pstRatList);


    pstMsg->enUtranMode                     = NAS_UTRANCTRL_ConvertNasUtranModeToGuRrcUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    vos_printf("NAS_MMC_SndAsStartReq, ulReceiverPid:%d, ulRet:%d. \n ", ulReceiverPid, ulRet);

    if (VOS_OK != ulRet)
    {
        return ;
    }

#ifdef NAS_STUB
    {
        VOS_UINT32               ulRet = 0;            /* 定义调用VOS发送函数的返回值 */
        RRMM_START_CNF_STRU*     pStartCnfMsg = VOS_NULL_PTR; /* 定义原语类型指针 */

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
        /* 给TD发开机，则TD回复开机结果 */
        if ( (NAS_UTRANCTRL_UTRAN_MODE_TDD == NAS_UTRANCTRL_GetCurrUtranMode())
           && (WUEPS_PID_WRR               == ulReceiverPid))
        {
            ulReceiverPid = TPS_PID_RRC;
        }
#endif

        /* 申请内存  */
        pStartCnfMsg = (RRMM_START_CNF_STRU *)PS_ALLOC_MSG(ulReceiverPid,
                                                   sizeof(RRMM_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
        if( VOS_NULL_PTR == pStartCnfMsg )
        {
            return ;
        }

        pStartCnfMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
        pStartCnfMsg->MsgHeader.ulReceiverPid       = WUEPS_PID_MMC;
        pStartCnfMsg->MsgHeader.ulMsgName           = RRMM_START_CNF;

        pStartCnfMsg->ulResult                      = MMC_START_SUCCESS;

        /* 调用VOS发送原语 */
        ulRet = PS_SEND_MSG(ulReceiverPid, pStartCnfMsg);
        if(VOS_OK != ulRet)
        {
            return ;
        }
    }

#endif
}


VOS_VOID NAS_MMC_SndAsSuspendRelReq(
    VOS_UINT32                          ulReceiverPid
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                       ulRet = 0;
    /* 定义原语类型指针 */
    RRMM_SUSPEND_REL_REQ_STRU       *pstAsMsg = VOS_NULL_PTR;

    /*申请消息  */
    pstAsMsg = (RRMM_SUSPEND_REL_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMC,
                                            sizeof(RRMM_SUSPEND_REL_REQ_STRU));

    /* 内存申请失败 */
    if( VOS_NULL_PTR == pstAsMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendRelReq: Alloc memeory Error");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstAsMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(RRMM_SUSPEND_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstAsMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAsMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAsMsg->MsgHeader.ulMsgName         = RRMM_SUSPEND_REL_REQ;

    pstAsMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAsMsg->MsgHeader.ulReceiverPid     = ulReceiverPid;

    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstAsMsg);

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendRelReq():NORMAL:MMC SEND AS SUSPEND REL REQ.");

    if(VOS_OK != ulRet)
    {
        /* 发送失败 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendRelReq: pstAsMsg");
        return;
    }

    return;
}
VOS_VOID  NAS_MMC_SndAsSyscfgReq(
    MMA_MMC_SYS_CFG_SET_REQ_STRU        *pstSysCfgSetParm,
    VOS_UINT32                          ulReceiverPid
)
{
    /* 使用V7的SYSCFG接口进行发送 */
    RRMM_SYS_CFG_REQ_STRU              *pstSysCfgMsg        = VOS_NULL_PTR;
    NAS_MML_PLMN_RAT_PRIO_STRU          stRatList;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucSimPresentStatus;


    /* 申请内存  */
    pstSysCfgMsg = (RRMM_SYS_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstSysCfgMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSyscfgReq,ERRORALLOC MEM RRMM_SYS_CFG_REQ FAIL");
        return ;
    }

    PS_MEM_SET( (VOS_INT8*)pstSysCfgMsg + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(RRMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstSysCfgMsg->MsgHeader.ulReceiverCpuId      = VOS_LOCAL_CPUID;
    pstSysCfgMsg->MsgHeader.ulReceiverPid        = ulReceiverPid;
    pstSysCfgMsg->MsgHeader.ulLength             = sizeof(RRMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSysCfgMsg->MsgHeader.ulMsgName            = RRMM_SYS_CFG_REQ;

    /* 设置OPT为VOS_TRUE */
    pstSysCfgMsg->bitOpBand                      = VOS_TRUE;
    pstSysCfgMsg->bitOpRat                       = VOS_TRUE;

    /* 设置频段信息 */
    PS_MEM_CPY(&(pstSysCfgMsg->stBand.stLteBand), &(pstSysCfgSetParm->stBand.stLteBand), sizeof(LTE_BAND_STRU));
    pstSysCfgMsg->stBand.stWcdmaBand.ulWcdmaBand = pstSysCfgSetParm->stBand.unWcdmaBand.ulBand;
    pstSysCfgMsg->stBand.stGsmBand.ulGsmBand     = pstSysCfgSetParm->stBand.unGsmBand.ulBand;

    /* 设置接入模式 */
    PS_MEM_CPY(&stRatList, &(pstSysCfgSetParm->stRatPrioList), sizeof(NAS_MML_PLMN_RAT_PRIO_STRU));
    pstSysCfgMsg->aucRatPrioList[0]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_GSM, &stRatList);
    pstSysCfgMsg->aucRatPrioList[1]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_WCDMA, &stRatList);
    pstSysCfgMsg->aucRatPrioList[2]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_LTE, &stRatList);

    /* 设置sim卡状态信息 */
    ucSimPresentStatus                           = NAS_MML_GetSimPresentStatus();
    pstSysCfgMsg->ulSimStatus                    = ucSimPresentStatus;


    /* 调用VOS发送原语 */
    ulRslt = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstSysCfgMsg);

    if(VOS_OK != ulRslt)
    {
         NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSyscfgReq,ERROR:发送失败");
    }

    return ;
}


VOS_VOID  NAS_MMC_SndAsSuspendRsp(
    RRC_NAS_SUSPEND_RESULT_ENUM_UINT8   enSuspendRslt,
    VOS_UINT32                          ulReceiverPid
)
{
    VOS_UINT32                          ulResult;
    RRMM_SUSPEND_RSP_ST                *pstAsMsg = VOS_NULL_PTR;

    /*申请消息  */

    pstAsMsg = (RRMM_SUSPEND_RSP_ST*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                 WUEPS_PID_MMC,
                                                 sizeof(RRMM_SUSPEND_RSP_ST));

    /* 内存申请失败,直接返回 */
    if( VOS_NULL_PTR == pstAsMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendRsp():ERROR:Alloc Msg fail");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstAsMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(RRMM_SUSPEND_RSP_ST) - VOS_MSG_HEAD_LENGTH );

    /*填写消息内容*/
    pstAsMsg->MsgHeader.ulMsgName       = RRMM_SUSPEND_RSP;
    pstAsMsg->MsgHeader.ulReceiverPid   = ulReceiverPid;
    pstAsMsg->MsgHeader.ulLength        = sizeof(RRMM_SUSPEND_RSP_ST) - VOS_MSG_HEAD_LENGTH;
    pstAsMsg->ucResult                  = enSuspendRslt;


    /*发送消息*/
    ulResult = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstAsMsg);

    if (VOS_OK != ulResult)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSuspendRsp():WARNING:MMC SEND AS SUSPEND RSP failed!");
    }

    return ;
}




VOS_VOID NAS_MMC_SndAsResumeRsp(
    VOS_UINT32                          ulReceiverPid
)
{
    VOS_UINT32                          ulResult;
    RRMM_RESUME_RSP_ST                 *pstAsMsg;

    /*申请消息  */
    pstAsMsg = (RRMM_RESUME_RSP_ST*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMC,
                                            sizeof(RRMM_RESUME_RSP_ST));

    /* 内存申请失败，返回 */
    if( VOS_NULL_PTR == pstAsMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsResumeRsp():ERROR:Alloc Msg fail");

        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstAsMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(RRMM_RESUME_RSP_ST) - VOS_MSG_HEAD_LENGTH );

     /*填写消息内容*/
    pstAsMsg->MsgHeader.ulMsgName       = RRMM_RESUME_RSP;
    pstAsMsg->MsgHeader.ulReceiverPid   = ulReceiverPid;


    /* 调用VOS发送原语 */
    ulResult = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstAsMsg);


    if(VOS_OK != ulResult)
    {
        /* 发送失败 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsResumeRsp():WARNING:MMC SEND AS RESUME RSP failed!");
        return;
    }
    return;
}


VOS_VOID NAS_MMC_SndWasInfoReportReq(VOS_VOID)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet = 0;

    /* 定义原语类型指针 */
    RRMM_AT_MSG_REQ_ST                 *pstWasMsg = VOS_NULL_PTR;

    NAS_MMC_MAINTAIN_CTX_STRU          *pstMainCtx = VOS_NULL_PTR;

    /* 平台W/TD都不支持时，不需要给U模发送RRMM_AT_MSG_REQ */
    if (VOS_FALSE == NAS_MML_IsPlatformSupportUtran())
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_SndWasInfoReportReq():NORMAL:Platform Not Support UMTS");

        /* 返回 */
        return;
    }
    /*申请消息  */
    pstWasMsg = (RRMM_AT_MSG_REQ_ST*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                        WUEPS_PID_MMC,
                                                        sizeof(RRMM_AT_MSG_REQ_ST));
    /* 内存申请失败 */
    if( VOS_NULL_PTR == pstWasMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndWasInfoReportReq():ERROR:Alloc Msg fail");

        /* 返回 */
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstWasMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(RRMM_AT_MSG_REQ_ST) - VOS_MSG_HEAD_LENGTH );

    pstMainCtx                             = NAS_MMC_GetMaintainInfo();

    pstWasMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstWasMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstWasMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstWasMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_WRR;

    /* 消息名称 */
    pstWasMsg->MsgHeader.ulMsgName         = RRMM_AT_MSG_REQ;

    /*操作方式*/
    pstWasMsg->ucActionType                = pstMainCtx->ucActionType;

    /*WAS上报的信息类型*/
    pstWasMsg->ucMsgType                   = pstMainCtx->ucMsgType;

    pstWasMsg->ucSignThreshold             = pstMainCtx->ucSignThreshold;
    pstWasMsg->ucMinRptTimerInterval       = pstMainCtx->ucMinRptTimerInterval;

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstWasMsg);

    if(VOS_OK != ulRet)
    {
        /* 发送失败 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndWasInfoReportReq():WARNING:MMC SEND WAS START AT INFO failed!");
        return;
    }

    /* 返回 */
    return;
}
VOS_VOID NAS_MMC_SndGasInfoReportReq(VOS_VOID)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                       ulRet = 0;

    /* 定义原语类型指针 */
    GRRMM_AT_MSG_REQ_ST             *pstGasMsg = VOS_NULL_PTR;

    NAS_MMC_MAINTAIN_CTX_STRU       *pstMainCtx = VOS_NULL_PTR;
    /* 平台不支持G模时，不需要给G模发送GRRMM_AT_MSG_REQ */
    if (VOS_FALSE == NAS_MML_IsPlatformSupportGsm())
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasInfoReportReq():NORMAL:Platform Not Support GSM");

        /* 返回 */
        return;
    }

    /*申请消息  */
    pstGasMsg = (GRRMM_AT_MSG_REQ_ST*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                    WUEPS_PID_MMC,
                                                    sizeof(GRRMM_AT_MSG_REQ_ST));

    /* 内存申请失败 */
    if( VOS_NULL_PTR == pstGasMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasInfoReportReq():ERROR:Alloc Msg fail");

        /* 返回 */
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstGasMsg + VOS_MSG_HEAD_LENGTH, 0X00,
            sizeof(GRRMM_AT_MSG_REQ_ST) - VOS_MSG_HEAD_LENGTH );

    pstMainCtx                             = NAS_MMC_GetMaintainInfo();
    pstGasMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstGasMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstGasMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstGasMsg->MsgHeader.ulReceiverPid     = UEPS_PID_GAS;

    /* 消息名称 */
    pstGasMsg->MsgHeader.ulMsgName        = GRRMM_AT_MSG_REQ;
    /*GAS上报的小区信息在系统信息中上报，误码率和信号质量使用同一个结构
    上报，不存在信息类型*/
    pstGasMsg->ucActionType               = pstMainCtx->ucActionType;

    pstGasMsg->ucSignThreshold            = pstMainCtx->ucSignThreshold;

    pstGasMsg->ucMinRptTimerInterval      = pstMainCtx->ucMinRptTimerInterval;


    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstGasMsg);

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasInfoReportReq():NORMAL:MMC SEND GAS START AT INFO");

    if(VOS_OK != ulRet)
    {
        /* 发送失败 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGasInfoReportReq():WARNING:MMC SEND GAS START AT INFO failed!");
        return;
    }

    /* 返回 */
    return;
}



VOS_VOID NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg(
    VOS_UINT32                                             *pulForbidLaNum,
    RRC_FORB_LA_STRU                                        aForbLaList[],
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU               *pstForbRoamLaInfo
)
{
    VOS_UINT32                          i;

    /* 禁止漫游 LA 的个数为0，则不向ForbLa列表中进行添加, 直接返回 */
    if ( 0 == pstForbRoamLaInfo->ucForbRoamLaNum )
    {
        NAS_INFO_LOG(WUEPS_PID_MMC, "NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg():ERROR:Memory Alloc Error for pstMsg");
        return;
    }

    /* 非指定搜网，将所有禁止漫游 LA 添加到ForbLa列表 */
    for ( i = 0; i < pstForbRoamLaInfo->ucForbRoamLaNum; i++ )
    {
        aForbLaList[i].PlmnId.ulMcc =
            pstForbRoamLaInfo->astForbRomLaList[i].stPlmnId.ulMcc;
        aForbLaList[i].PlmnId.ulMnc =
            pstForbRoamLaInfo->astForbRomLaList[i].stPlmnId.ulMnc;
        aForbLaList[i].ulForbLac    =
            pstForbRoamLaInfo->astForbRomLaList[i].aucLac[0] << 8;
        aForbLaList[i].ulForbLac    |=
            pstForbRoamLaInfo->astForbRomLaList[i].aucLac[1];
    }

    /* 输出添加到ForbLa列表中的禁止漫游 LA 的个数 */
    (*pulForbidLaNum) = pstForbRoamLaInfo->ucForbRoamLaNum;

    return;
}


VOS_VOID NAS_MMC_BuildAsSpecPlmnSearchReqMsg(
    RRMM_PLMN_SEARCH_REQ_STRU                              *pstMsg,
    NAS_MML_PLMN_LIST_WITH_RAT_STRU                        *pstDestPlmnList,
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU               *pstForbRoamLaInfo
)
{
    VOS_UINT32                              ulIndex;
    NAS_MML_PLMN_WITH_RAT_STRU             *pstDestPlmn = VOS_NULL_PTR;

    pstDestPlmn                           = &(pstDestPlmnList->astPlmnId[0]);

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
         sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulMsgName           = RRMM_PLMN_SEARCH_REQ;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulSearchType                  = RRC_PLMN_SEARCH_SPEC;
    pstMsg->ulPlmnIdNum                   = pstDestPlmnList->ulPlmnNum;

    for ( ulIndex = 0 ; ulIndex < NAS_MML_MIN(pstDestPlmnList->ulPlmnNum, RRC_MAX_EQUPLMN_NUM) ; ulIndex++ )
    {
        pstMsg->aPlmnIdList[ulIndex].ulMcc          = pstDestPlmnList->astPlmnId[ulIndex].stPlmnId.ulMcc;
        pstMsg->aPlmnIdList[ulIndex].ulMnc          = pstDestPlmnList->astPlmnId[ulIndex].stPlmnId.ulMnc;
    }

    if (NAS_MML_NET_RAT_TYPE_WCDMA == pstDestPlmn->enRat)
    {
        pstMsg->MsgHeader.ulReceiverPid       = WUEPS_PID_WRR;
    }
    else
    {
        pstMsg->MsgHeader.ulReceiverPid       = UEPS_PID_GAS;
    }

    /* 添加禁止LAI信息 */
    NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg(&(pstMsg->ulForbLaNum),
                                                pstMsg->aForbLaList,
                                                pstForbRoamLaInfo);

    return;
}


VOS_VOID NAS_MMC_SndAsSpecPlmnSearchReq(
    NAS_MML_PLMN_LIST_WITH_RAT_STRU                        *pstDestPlmnList,
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU               *pstForbRoamLaInfo
)
{
    RRMM_PLMN_SEARCH_REQ_STRU          *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSpecPlmnSearchReq:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    NAS_MMC_BuildAsSpecPlmnSearchReqMsg(pstMsg, pstDestPlmnList, pstForbRoamLaInfo);

    /* 调用VOS发送原语 */
    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSpecPlmnSearchReq():WARNING:SEND AS PLMN SEARCH REQ FIAL");
        return;
    }

#ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif


}
VOS_VOID NAS_MMC_SndAsNcellSpecPlmnSearchReq(
    NAS_MMC_TDS_NCELL_INFO_STRU        *pstTdsNcellInfo,
    NAS_MML_EQUPLMN_INFO_STRU          *pstOtherModemEplmnInfo
)
{
    RRMM_PLMN_SEARCH_REQ_STRU          *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSpecPlmnSearchReq:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                    sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulMsgName           = RRMM_PLMN_SEARCH_REQ;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulSearchType                  = RRC_PLMN_SEARCH_NCELL_SPEC;
    pstMsg->MsgHeader.ulSenderPid         = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid       = WUEPS_PID_WRR;

    pstMsg->stNcellInfo.ucTdsArfcnNum = pstTdsNcellInfo->ucTdsArfcnNum;
    PS_MEM_CPY(pstMsg->stNcellInfo.ausTdsArfcnList, pstTdsNcellInfo->ausTdsArfcnList,
               sizeof(pstMsg->stNcellInfo.ausTdsArfcnList));

    pstMsg->stNcellInfo.ucEplmnIdNum = pstOtherModemEplmnInfo->ucEquPlmnNum;
    PS_MEM_CPY(pstMsg->stNcellInfo.astEplmnIdList, pstOtherModemEplmnInfo->astEquPlmnAddr,
                sizeof(pstMsg->stNcellInfo.astEplmnIdList));

    /* 调用VOS发送原语 */
    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSpecPlmnSearchReq():WARNING:SEND AS PLMN SEARCH REQ FIAL");
        return;
    }

#ifndef __PS_WIN32_RECUR__
    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif

}



VOS_VOID NAS_MMC_SndAsPlmnListReq(
    VOS_UINT32                          ulReceiverPid
)
{
    RRMM_PLMN_SEARCH_REQ_STRU          *pstMsg = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsPlmnListReq:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                    sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulMsgName           = RRMM_PLMN_SEARCH_REQ;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulSearchType                  = RRC_PLMN_SEARCH_LIST;

    pstMsg->MsgHeader.ulReceiverPid       = ulReceiverPid;

    /* 调用VOS发送原语 */
    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsPlmnListReq():WARNING:SEND AS PLMN SEARCH REQ FIAL");
        return;
    }

#ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif
}




VOS_VOID NAS_MMC_SndAsAvailPlmnMsg(
    NAS_MMC_AVAIL_PLMN_STRU             *pAvailPlmnInfo
)
{
    VOS_UINT8                           i;
    RRMM_PLMN_QUERY_CNF_STRU           *pstMsg = VOS_NULL;
    VOS_UINT32                          ulRet;

    pstMsg = (RRMM_PLMN_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                WUEPS_PID_MMC,sizeof(RRMM_PLMN_QUERY_CNF_STRU));

    if( VOS_NULL == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsAvailPlmnMsg():ERROR:Memory Alloc Error for pstMsg");
        return ;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0x00,
         sizeof(RRMM_PLMN_QUERY_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_WRR;
    pstMsg->MsgHeader.ulMsgName         = RRMM_PLMN_QUERY_CNF;

    pstMsg->ulAvailPlmnIdNum= pAvailPlmnInfo->ulAvailPlmnNum;
    for(i = 0; i < pAvailPlmnInfo->ulAvailPlmnNum; i++)
    {
        pstMsg->astAvailPlmnIdList[i] = pAvailPlmnInfo->astAvailPlmnIdList[i];
    }

    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);
    if (VOS_OK != ulRet)
    {
        /* 打印错误信息 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsAvailPlmnMsg():Error: Failed in NAS_UTRANCTRL_SndAsMsg()");
    }
}

/*****************************************************************************
 函 数 名  : NAS_MMC_JudgePlmnIfInAvailPlmnList
 功能描述  : 判断输入的plmn是否已经在available plmn列表中
 输入参数  : pstAvailPlmn - 优先搜索的网络信息列表
              stPlmnId     - 优先搜索的网络信息
 输出参数  : 无
 返 回 值  : VOS_TRUE  - 输入的plmn已在available plmn列表中
              VOS_FALSE - 输入的plmn不在available plmn列表中
 调用函数  :
 被调函数  :
 修改历史  :
 1.日    期   : 2011年7月20日
   作    者   : zhoujun \40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_MMC_IsPlmnInAvailPlmnList(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId,
    NAS_MMC_AVAIL_PLMN_STRU            *pstAvailPlmn
)
{
    VOS_UINT32                          i;

    for (i = 0; i < pstAvailPlmn->ulAvailPlmnNum; i++)
    {
        if (VOS_TRUE == NAS_MML_CompareBcchPlmnwithSimPlmn((NAS_MML_PLMN_ID_STRU *)&pstAvailPlmn->astAvailPlmnIdList[i],
                                                            pstPlmnId))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}

/*****************************************************************************
 函 数 名  : NAS_MMC_BuildAsPlmnInfo
 功能描述  : 构造PLMN列表信息
 输入参数  : 无
 输出参数  : pstAvailPlmn:构造的PLMN的信息
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年8月6日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MMC_BuildAsFirstPlmnInfo(
    NAS_MMC_AVAIL_PLMN_STRU            *pstAvailPlmn
)
{
    NAS_MML_PLMN_WITH_RAT_STRU              *pstUserPlmnInfo;
    NAS_MML_SIM_EHPLMN_INFO_STRU            *pstEhplmnInfo;

    pstEhplmnInfo   = NAS_MML_GetSimEhplmnList();


    pstUserPlmnInfo = NAS_MMC_GetUserSpecPlmnId();

    PS_MEM_SET(pstAvailPlmn, 0, sizeof(NAS_MMC_AVAIL_PLMN_STRU));

    /* 首先把HPLMN/UPlmn加入候补列表中,然后把RPLMN加入候补列表中 */

    if (NAS_MMC_PLMN_SELECTION_MODE_MANUAL == NAS_MMC_GetPlmnSelectionMode())
    {
        pstAvailPlmn->astAvailPlmnIdList[pstAvailPlmn->ulAvailPlmnNum].ulMcc = pstUserPlmnInfo->stPlmnId.ulMcc;
        pstAvailPlmn->astAvailPlmnIdList[pstAvailPlmn->ulAvailPlmnNum].ulMnc = pstUserPlmnInfo->stPlmnId.ulMnc;
    }
    else
    {
        pstAvailPlmn->astAvailPlmnIdList[pstAvailPlmn->ulAvailPlmnNum].ulMcc = pstEhplmnInfo->astEhPlmnInfo[0].stPlmnId.ulMcc;
        pstAvailPlmn->astAvailPlmnIdList[pstAvailPlmn->ulAvailPlmnNum].ulMnc = pstEhplmnInfo->astEhPlmnInfo[0].stPlmnId.ulMnc;
    }

    pstAvailPlmn->ulAvailPlmnNum++;
}

VOS_VOID NAS_MMC_SndRrcPlmnQueryCnf(
    PS_BOOL_ENUM_UINT8                  enQueryWPlmn
)
{
    NAS_MMC_AVAIL_PLMN_STRU                 stAvailPlmn;
    VOS_UINT32                              i;
    NAS_MMC_PLMN_SELECTION_MODE_ENUM_UINT8  enSelectionMode;

    NAS_MML_RPLMN_CFG_INFO_STRU            *pstRplmnCfg     = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU              *pstEquPlmnInfo  = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                   *pstCurrPlmnId   = VOS_NULL_PTR;

    pstRplmnCfg     = NAS_MML_GetRplmnCfg();
    pstEquPlmnInfo  = NAS_MML_GetEquPlmnList();
    pstCurrPlmnId   = NAS_MML_GetCurrCampPlmnId();

    /* 构造第一个PLMN列表 */
    NAS_MMC_BuildAsFirstPlmnInfo(&stAvailPlmn);

    if(PS_FALSE == enQueryWPlmn)
    {
        enSelectionMode = NAS_MMC_GetPlmnSelectionMode();
        if( (VOS_TRUE == NAS_MML_IsPlmnIdValid(&(pstRplmnCfg->stGRplmnInNV)))
         && (NAS_MMC_PLMN_SELECTION_MODE_AUTO == enSelectionMode) )
        {
            /* 判断g_MmcGlobalCtrl.stGRplmnInNV是否已经存在于stAvailPlmn中,如果存在则无需添加 */
            if (VOS_FALSE == NAS_MMC_IsPlmnInAvailPlmnList(&(pstRplmnCfg->stGRplmnInNV),
                                                           &stAvailPlmn))
            {
                stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMcc = pstRplmnCfg->stGRplmnInNV.ulMcc;
                stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMnc = pstRplmnCfg->stGRplmnInNV.ulMnc;
                stAvailPlmn.ulAvailPlmnNum++;
            }
        }
    }


    /* Eplmn存在时，添加Eplmn  */
    if ( pstEquPlmnInfo->ucEquPlmnNum > 0 )
    {
        for(i = 0; i < pstEquPlmnInfo->ucEquPlmnNum; i++)
        {
            if (VOS_TRUE == NAS_MML_IsBcchPlmnIdInDestSimPlmnList(&(pstEquPlmnInfo->astEquPlmnAddr[i]),
                             (VOS_UINT8)stAvailPlmn.ulAvailPlmnNum, (NAS_MML_PLMN_ID_STRU *)stAvailPlmn.astAvailPlmnIdList))
            {
                continue;
            }

            stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMcc = pstEquPlmnInfo->astEquPlmnAddr[i].ulMcc;
            stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMnc = pstEquPlmnInfo->astEquPlmnAddr[i].ulMnc;
            stAvailPlmn.ulAvailPlmnNum++;
        }

    }
    /*Rplmn不存在时，只添加当前网络*/
    else
    {
        stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMcc = pstCurrPlmnId->ulMcc;
        stAvailPlmn.astAvailPlmnIdList[stAvailPlmn.ulAvailPlmnNum].ulMnc = pstCurrPlmnId->ulMnc;
        stAvailPlmn.ulAvailPlmnNum++;
    }

    NAS_MMC_SndAsAvailPlmnMsg(&stAvailPlmn);
}



VOS_VOID NAS_MMC_SndAsEquPlmnQueryCnf(
    EQUIVALENT_PLMN_STRU               *pstEquPlmnList,
    VOS_UINT32                          ulOpCurPlmnId,
    NAS_MML_PLMN_ID_STRU               *pstCurPlmnId,
    VOS_UINT32                          ulPid
)
{
    VOS_UINT8                           i;
    RRMM_EPLMN_QUERY_CNF_STRU          *pstMsg = VOS_NULL;
    VOS_UINT32                          ulRet;

    pstMsg = (RRMM_EPLMN_QUERY_CNF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                WUEPS_PID_MMC,sizeof(RRMM_EPLMN_QUERY_CNF_STRU));
    if( VOS_NULL == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsEquPlmnQueryCnf():ERROR:Memory Alloc Error for pstMsg");
        return ;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
         sizeof(RRMM_EPLMN_QUERY_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    pstMsg->MsgHeader.ulReceiverPid = ulPid;
    pstMsg->MsgHeader.ulMsgName     = RRMM_EPLMN_QUERY_CNF;
    pstMsg->ulEPlmnIdNum            = pstEquPlmnList->ulEquPlmnNum;
    for(i = 0;i < pstEquPlmnList->ulEquPlmnNum;i++)
    {
        pstMsg->astEPlmnIdList[i].ulMcc = pstEquPlmnList->aEquPlmnIdList[i].ulMcc;
        pstMsg->astEPlmnIdList[i].ulMnc = pstEquPlmnList->aEquPlmnIdList[i].ulMnc;
    }

    if (VOS_TRUE == ulOpCurPlmnId)
    {
        pstMsg->bitOpCurPlmn = VOS_TRUE;
        pstMsg->stCurCampedPlmnId.ulMcc = pstCurPlmnId->ulMcc;
        pstMsg->stCurCampedPlmnId.ulMnc = pstCurPlmnId->ulMnc;
    }

    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);
    if (VOS_OK != ulRet)
    {
        /* 打印错误信息 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "Failed in NAS_UTRANCTRL_SndAsMsg()");
    }
}

VOS_VOID   NAS_MMC_SndRrMmCellSelReq(
    VOS_UINT8                           ucSrhType
)
{
    VOS_UINT8                           i;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidInfo = VOS_NULL_PTR;
    VOS_UINT16                          usLac;
    RRMM_CELL_SELECTION_CTRL_REQ_STRU  *pstMsg        = VOS_NULL_PTR;                                   /* 定义原语类型指针                          */
    VOS_UINT32                          ulRet;


    pstMsg = (RRMM_CELL_SELECTION_CTRL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                WUEPS_PID_MMC,sizeof(RRMM_CELL_SELECTION_CTRL_REQ_STRU));
    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrMmCellSelReq():ERROR:Memory Alloc Error for pstMsg");

        return ;
    }

    /* 数组未初始化导致PC回放时出错 */
    PS_MEM_SET(pstMsg->aForbLaList, 0,sizeof(RRC_FORB_LA_STRU)*RRC_MAX_FORBLA_NUM);

    pstForbidInfo           = NAS_MML_GetForbidPlmnInfo();
    pstMsg->ulForbLaNum     = pstForbidInfo->ucForbRoamLaNum;

    for ( i= 0; i < pstForbidInfo->ucForbRoamLaNum; i++ )
    {
        usLac = ( ((pstForbidInfo->astForbRomLaList[i].aucLac[0]) << 8 )
                | ((pstForbidInfo->astForbRomLaList[i].aucLac[1])));

        pstMsg->aForbLaList[i].ulForbLac      = usLac;
        pstMsg->aForbLaList[i].PlmnId.ulMcc   =
                    pstForbidInfo->astForbRomLaList[i].stPlmnId.ulMcc;

        pstMsg->aForbLaList[i].PlmnId.ulMnc   =
                    pstForbidInfo->astForbRomLaList[i].stPlmnId.ulMnc;
    }


    pstMsg->MsgHeader.ulMsgName = RRMM_CELL_SELECTION_CTRL_REQ;                          /* 消息名称                                  */

    if ( NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        pstMsg->MsgHeader.ulReceiverPid     = UEPS_PID_GAS;
    }
    else
    {
        pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_WRR;
    }

    pstMsg->ulReselectInd                   = ucSrhType;

    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);
    if (VOS_OK != ulRet)
    {
        /* 打印错误信息 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "Failed in NAS_UTRANCTRL_SndAsMsg()");
    }

}


VOS_UINT32 MMC_ComJudgeLai(
    RRC_PLMN_ID_STRU                    stPlmn,
    VOS_UINT16                          usLac,
    PS_BOOL_ENUM_UINT8                 *penLaiValidFlg
)
{
    NAS_MML_PLMN_WITH_RAT_STRU         *pstUserPlmn;
    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmn;
    NAS_MML_LAI_STRU                    stLai;
    NAS_MMC_PLMN_SELECTION_MODE_ENUM_UINT8                  enSelectionMode;

    pstEquPlmn          = NAS_MML_GetEquPlmnList();
    pstUserPlmn         = NAS_MMC_GetUserSpecPlmnId();
    *penLaiValidFlg     = PS_TRUE;

    if (VOS_TRUE == NAS_MML_IsPlmnIdInForbidPlmnList((NAS_MML_PLMN_ID_STRU*)&stPlmn))
    {
       *penLaiValidFlg =  PS_FALSE;
    }

    stLai.stPlmnId.ulMcc = stPlmn.ulMcc;
    stLai.stPlmnId.ulMnc = stPlmn.ulMnc;
    stLai.aucLac[0]      = (usLac & 0xFF00) >> 8;
    stLai.aucLac[1]      = (usLac & 0xFF);

    if (VOS_TRUE == NAS_MML_IsInForbidLaiList(&stLai))
    {
       *penLaiValidFlg =  PS_FALSE;
    }


    /* 手动模式下,用户指定搜网PLMN ID不在EPLMN中 */
    enSelectionMode = NAS_MMC_GetPlmnSelectionMode();
    if ( (PS_TRUE == *penLaiValidFlg)
      && (NAS_MMC_PLMN_SELECTION_MODE_MANUAL == enSelectionMode) )
    {
        if ( VOS_FALSE == NAS_MML_IsBcchPlmnIdInDestSimPlmnList((NAS_MML_PLMN_ID_STRU*)&stPlmn,
                                                        1,
                                                        &(pstUserPlmn->stPlmnId)))
        {
            if (VOS_FALSE == NAS_MML_IsBcchPlmnIdInDestSimPlmnList((NAS_MML_PLMN_ID_STRU*)&stPlmn,
                                                pstEquPlmn->ucEquPlmnNum,
                                                pstEquPlmn->astEquPlmnAddr))
            {
                *penLaiValidFlg = PS_FALSE;
            }
        }


    }


    return VOS_OK;
}
VOS_VOID NAS_MMC_SndRrMmRelReq(
    RRC_NAS_CN_DOMAIN_TYPE_ENUM_UINT32  ulCnDomain
)
{
    RRMM_REL_REQ_STRU                  *pstMsg = VOS_NULL_PTR;

    pstMsg = (RRMM_REL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_MMC,
                        sizeof(RRMM_REL_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrMmRelReq: memery alloc failed.");

        return;
    }

    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = WUEPS_PID_WRR;
    pstMsg->MsgHeader.ulMsgName       = RRMM_REL_REQ;

    pstMsg->ulCnDomainId              = ulCnDomain;

    /* MMC释放连接时，此值始终为no barred */
    pstMsg->enBarValidFlg             = RRC_CELL_UNBARRED;

    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrMmRelReq: send message failed.");
    }

    return;
}
VOS_VOID NAS_MMC_SndAsFastPlmnSearchReq(
    VOS_UINT32                          ulReceiverPid,
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
)
{
    RRMM_PLMN_SEARCH_REQ_STRU          *pstMsg = VOS_NULL_PTR;

    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU                stForbRoamLaInfo;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                        *pstForbPlmnInfo = VOS_NULL_PTR;

    /* 申请内存  */
    pstMsg = (RRMM_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsSpecPlmnSearchReq:ERROR: Memory Alloc Error for pMsg");

        return;
    }

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid       = ulReceiverPid;
    pstMsg->MsgHeader.ulMsgName           = RRMM_PLMN_SEARCH_REQ;
    pstMsg->MsgHeader.ulLength            = sizeof(RRMM_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->ulSearchType                  = RRC_PLMN_SEARCH_SPEC_FAST;
    pstMsg->ulPlmnIdNum                   = 1;
    pstMsg->aPlmnIdList[0].ulMcc          = pstPlmnId->ulMcc;
    pstMsg->aPlmnIdList[0].ulMnc          = pstPlmnId->ulMnc;

    /* 获取全局变量中的FORB ROAM LA 信息 */
    pstForbPlmnInfo = NAS_MML_GetForbidPlmnInfo();

    stForbRoamLaInfo.ucForbRoamLaNum = pstForbPlmnInfo->ucForbRoamLaNum;
    PS_MEM_CPY( stForbRoamLaInfo.astForbRomLaList,
                pstForbPlmnInfo->astForbRomLaList,
                sizeof(pstForbPlmnInfo->astForbRomLaList) );

    /* 添加禁止LAI信息 */
    NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg(&(pstMsg->ulForbLaNum),
                                    pstMsg->aForbLaList,
                                    &stForbRoamLaInfo);

    /* 调用VOS发送原语 */
    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsFastPlmnSearchReq():WARNING:SEND AS PLMN SEARCH REQ FIAL");
    }

    #ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
    #endif

    return;
}


VOS_VOID NAS_MMC_SndAsBgPlmnSearchReq(
    VOS_UINT32                          ulReceiverPid,
    NAS_MML_PLMN_WITH_RAT_STRU         *pstPlmnId
)
{
    RRMM_BG_PLMN_SEARCH_REQ_ST                             *pstMsg = VOS_NULL_PTR;

    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU                stForbRoamLaInfo;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbPlmnInfo = VOS_NULL_PTR;

    pstMsg = (RRMM_BG_PLMN_SEARCH_REQ_ST *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                        WUEPS_PID_MMC,
                        sizeof(RRMM_BG_PLMN_SEARCH_REQ_ST));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsBgPlmnSearchReq: memery alloc failed.");

        return;
    }

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(RRMM_BG_PLMN_SEARCH_REQ_ST) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = ulReceiverPid;

    pstMsg->MsgHeader.ulMsgName       = RRMM_BG_PLMN_SEARCH_REQ;

    pstMsg->stPlmnId.stPlmnId.ulMcc   = pstPlmnId->stPlmnId.ulMcc ;
    pstMsg->stPlmnId.stPlmnId.ulMnc   = pstPlmnId->stPlmnId.ulMnc ;

    /* 把MMC内部的RAT转换为和AS接口中的类型 */
    switch (pstPlmnId->enRat)
    {
        case NAS_MML_NET_RAT_TYPE_WCDMA:

            pstMsg->stPlmnId.ulRat = RRC_NAS_RAT_WCDMA_FDD;
            break;

        case NAS_MML_NET_RAT_TYPE_GSM:

            pstMsg->stPlmnId.ulRat = RRC_NAS_RAT_GSM;
            break;
#if (FEATURE_ON == FEATURE_LTE)
        case NAS_MML_NET_RAT_TYPE_LTE:

            pstMsg->stPlmnId.ulRat = RRC_NAS_RAT_LTE;
            break;
#endif

        default:

            NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsBgPlmnSearchReq: Unexpected rat type.");
            break;
    }

    /* 获取全局变量中的FORB ROAM LA 信息 */
    pstForbPlmnInfo = NAS_MML_GetForbidPlmnInfo();

    stForbRoamLaInfo.ucForbRoamLaNum = pstForbPlmnInfo->ucForbRoamLaNum;
    PS_MEM_CPY( stForbRoamLaInfo.astForbRomLaList,
                pstForbPlmnInfo->astForbRomLaList,
                sizeof(pstForbPlmnInfo->astForbRomLaList) );

    /* 获取禁止位置区列表 */
    NAS_MMC_BuildForbLaInAsSpecPlmnSearchReqMsg(&(pstMsg->ulForbLaNum),
                                    pstMsg->aForbLaList,
                                    &stForbRoamLaInfo);


    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsBgPlmnSearchReq: send message failed.");
    }


    #ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
    #endif

    return;
}


VOS_VOID NAS_MMC_SndAsBgStopPlmnSearchReq(
    VOS_UINT32                          ulReceiverPid
)
{
    RRMM_BG_PLMN_STOP_REQ_ST           *pstMsg = VOS_NULL_PTR;

    pstMsg = (RRMM_BG_PLMN_STOP_REQ_ST *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_MMC,
                        sizeof(RRMM_BG_PLMN_STOP_REQ_ST));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsBgStopPlmnSearchReq: memery alloc failed.");

        return;
    }

    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = ulReceiverPid;

    pstMsg->MsgHeader.ulMsgName       = RRMM_BG_PLMN_STOP_REQ;

    pstMsg->aucReserve1[0] = 0;
    pstMsg->aucReserve1[1] = 0;
    pstMsg->aucReserve1[2] = 0;
    pstMsg->aucReserve1[3] = 0;

    if (VOS_OK != NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsBgStopPlmnSearchReq: send message failed.");
    }

    #ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
    #endif

    return;
}




VOS_VOID NAS_MMC_SndAsNasCommInfoChangeReq(
    VOS_UINT32                          ulReceiverPid,
    NAS_COMM_INFO_STRU                 *pstNasComInfo
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;
    /* 定义原语类型指针 */
    RRMM_NAS_COMM_INFO_REQ_STRU        *pstMsg = VOS_NULL_PTR;

    pstMsg = (RRMM_NAS_COMM_INFO_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(RRMM_NAS_COMM_INFO_REQ_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNasCommInfoChangeReq():ERROR:Memory Alloc Error for pstMsg");
        /* 返回失败 */
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(RRMM_NAS_COMM_INFO_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId             = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid               = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid             = ulReceiverPid;
    pstMsg->MsgHeader.ulMsgName                 = RRMM_NAS_COMM_INFO_CHANGE_REQ;

    /* 如果是空指针 */
    if ( VOS_NULL_PTR == pstNasComInfo)
    {
        pstMsg->stNasCommInfo.bitOpUsimValid        = VOS_FALSE;


        pstMsg->stNasCommInfo.bitOpLTECapability    = VOS_FALSE;

        pstMsg->stNasCommInfo.bitOpEHPlmn           = VOS_FALSE;
        pstMsg->stNasCommInfo.bitOpHPlmn            = VOS_FALSE;

        pstMsg->stNasCommInfo.bitOpUTRANCapability  = VOS_FALSE;
        pstMsg->stNasCommInfo.bitOpGSMCapability    = VOS_FALSE;

        /* 调用VOS发送原语 */
        ulRet = NAS_UTRANCTRL_SndAsMsg( WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

        return;
    }

    PS_MEM_CPY(&(pstMsg->stNasCommInfo), pstNasComInfo, sizeof(NAS_COMM_INFO_STRU));


    /*发送消息*/
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNasCommInfoChangeReq():ERROR:Message sends fail");
        return;
    }
}

VOS_VOID NAS_MMC_SndAsNasUsimStatus(
    RRC_NAS_USIM_STATUS_ENUM_UINT32     enUsimStatus
)
{

    NAS_COMM_INFO_STRU                 stNasComInfo;

    PS_MEM_SET( &stNasComInfo, 0X0, sizeof(NAS_COMM_INFO_STRU) );

    /* 构造发送给接入层的消息 */
    stNasComInfo.bitOpUsimValid         = VOS_TRUE;
    stNasComInfo.enUsimStatus           = enUsimStatus;


    /* 平台接入技术能力支持W或TD时，需要给WRR发送消息 */
    if (VOS_TRUE == NAS_MML_IsPlatformSupportUtran())
    {
        /* 向WAS发送卡无效通知指示 */
        NAS_MMC_SndAsNasCommInfoChangeReq(WUEPS_PID_WRR, &stNasComInfo);
    }

    /* 平台接入技术能力支持G时，需要给GAS发送消息 */
    if (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
    {
        /* 向GAS发送卡无效通知指示 */
        NAS_MMC_SndAsNasCommInfoChangeReq(UEPS_PID_GAS, &stNasComInfo);
    }

    return;
}
VOS_VOID NAS_MMC_SndAsRatCapabilityStatusChangeInfo(VOS_VOID)
{
    NAS_COMM_INFO_STRU                  stNasComInfo;
    VOS_UINT8                           ucIsNeedSendAsChangeInfo;

    PS_MEM_SET( &stNasComInfo, 0X00, sizeof(NAS_COMM_INFO_STRU) );

    ucIsNeedSendAsChangeInfo = VOS_FALSE;

    /* 读取NV时做过参数检查，如果NUM是1的话就是Disable Lte Only;
                                 NUM是2的话就是Disable LTE+UTRAN */

    if (NAS_MML_RAT_CAPABILITY_STATUS_BUTT != NAS_MML_GetLteForbiddenStatusFlg())
    {
        ucIsNeedSendAsChangeInfo            = VOS_TRUE;
        stNasComInfo.bitOpLTECapability     = VOS_TRUE;
        stNasComInfo.enLCapabilityStatus    = NAS_MML_GetLteForbiddenStatusFlg();
    }

    if (NAS_MML_RAT_CAPABILITY_STATUS_BUTT != NAS_MML_GetUtranForbiddenStatusFlg())
    {
        ucIsNeedSendAsChangeInfo            = VOS_TRUE;
        stNasComInfo.bitOpUTRANCapability   = VOS_TRUE;
        stNasComInfo.enUCapabilityStatus    = NAS_MML_GetUtranForbiddenStatusFlg();
    }

    if (NAS_MML_RAT_CAPABILITY_STATUS_BUTT != NAS_MML_GetGsmForbiddenStatusFlg())
    {
        ucIsNeedSendAsChangeInfo            = VOS_TRUE;
        stNasComInfo.bitOpGSMCapability     = VOS_TRUE;
        stNasComInfo.enGCapabilityStatus    = NAS_MML_GetGsmForbiddenStatusFlg();
    }

    /* 如果全是BUTT，不需要发送 */
    if (VOS_FALSE == ucIsNeedSendAsChangeInfo)
    {
        return;
    }

    /* 如果平台支持GSM则向GAS发送消息 */
    if ( VOS_TRUE == NAS_MML_IsPlatformSupportGsm() )
    {
        NAS_MMC_SndAsNasCommInfoChangeReq(UEPS_PID_GAS, &stNasComInfo);
    }

    /* 如果平台支持UTRAN并且UTRAN的使能字段有效，就向WRR发送消息 */
    if ( VOS_TRUE == NAS_MML_IsPlatformSupportUtran() )
    {
        NAS_MMC_SndAsNasCommInfoChangeReq(WUEPS_PID_WRR, &stNasComInfo);
    }

    return;
}
VOS_VOID NAS_MMC_SndAsLteCapabilityStatus(
    VOS_UINT32                                              ulReceiverPid,
    RRC_NAS_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapabilityStatus
)
{
    NAS_COMM_INFO_STRU                  stNasComInfo;

    PS_MEM_SET( &stNasComInfo, 0X0, sizeof(NAS_COMM_INFO_STRU) );

    /* 构造发送给接入层的消息 */
    stNasComInfo.bitOpLTECapability     = VOS_TRUE;
    stNasComInfo.enLCapabilityStatus    = (VOS_UINT8)enLteCapabilityStatus;

    /* ulReceiverPid为WRR，并且平台支持UTRAN时，给WRR发送消息 */
    if ( (VOS_TRUE == NAS_MML_IsPlatformSupportUtran())
      && (WUEPS_PID_WRR == ulReceiverPid) )
    {
        /* 向WAS发送LTE能力状态指示 */
        NAS_MMC_SndAsNasCommInfoChangeReq(WUEPS_PID_WRR, &stNasComInfo);
    }

    /* ulReceiverPid为GAS，并且平台支持GSM时，给GAS发送消息 */
    if ( (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
      && (UEPS_PID_GAS == ulReceiverPid) )
    {
        /* 向GAS发送LTE能力状态指示 */
        NAS_MMC_SndAsNasCommInfoChangeReq(UEPS_PID_GAS, &stNasComInfo);
    }

    return;
}
VOS_VOID  NAS_MMC_SndGuAsUeOocStatus(
    VOS_UINT32                          ulReceiverPid
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                                  ulRet;
    /* 定义原语类型指针 */
    RRMM_UE_OOC_STATUS_NOTIFY_IND_STRU         *pstMsg = VOS_NULL_PTR;

    pstMsg = (RRMM_UE_OOC_STATUS_NOTIFY_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(RRMM_UE_OOC_STATUS_NOTIFY_IND_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGuAsUeOocStatus():ERROR:Memory Alloc Error for pstMsg");
        /* 返回失败 */
        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(RRMM_UE_OOC_STATUS_NOTIFY_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->MsgHeader.ulSenderCpuId             = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid               = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid             = ulReceiverPid;
    pstMsg->MsgHeader.ulMsgName                 = RRMM_UE_OOC_STATUS_NOTIFY_IND;


    /*发送消息*/
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    if (VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndGuAsUeOocStatus():WARNING:MMC SEND AS SUSPEND RSP failed!");
    }

}


VOS_VOID NAS_MMC_SndAsTransactionStatus(
    RRC_NAS_CS_TRANSACTION_ENUM_UINT8    enCsTransactionStatus,
    VOS_UINT32                           ulSenderEnqPid
)
{
    /* 定义原语类型指针 */
    RRMM_TRANSACTION_INFO_ANS_STRU         *pstAsMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulRet;

    /*申请消息  */
    pstAsMsg = (RRMM_TRANSACTION_INFO_ANS_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                      WUEPS_PID_MMC,
                                      sizeof(RRMM_TRANSACTION_INFO_ANS_STRU));
    /* 内存申请失败 */
    if( VOS_NULL_PTR == pstAsMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsCsTransactionStatus():ERROR:Alloc Msg fail");
        return;
    }

    pstAsMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAsMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAsMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAsMsg->MsgHeader.ulReceiverPid     = ulSenderEnqPid;

    /* 消息名称 */
    pstAsMsg->MsgHeader.ulMsgName                  = RRMM_TRANSACTION_INFO_ANS;
    pstAsMsg->stCsTransactInfo.enCsTransactionType = enCsTransactionStatus;

    PS_MEM_SET(pstAsMsg->stCsTransactInfo.aucReserve1, 0, sizeof(pstAsMsg->stCsTransactInfo.aucReserve1));
    PS_MEM_SET(pstAsMsg->stPsTransactInfo.aucReserve1, 0, sizeof(pstAsMsg->stPsTransactInfo.aucReserve1));

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstAsMsg);

    if(VOS_OK != ulRet)
    {
        /* 发送失败 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsCsTransactionStatus():WARNING:MMC SEND AS START AT INFO failed!");
        return;
    }

    /* 返回 */
    return;
}


VOS_VOID NAS_MMC_SndAsHplmnReq(
    VOS_UINT32                          ulReceiverPid
)
{
    NAS_COMM_INFO_STRU                  stNasCommInfoChange;
    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstEHPlmnInfo   = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                stImsiHplmn;
    VOS_UINT8                           i;
    NAS_MML_HPLMN_TYPE_ENUM_UINT8       enHplmnType;

    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity   = VOS_NULL_PTR;

    pstMsIdentity                       = NAS_MML_GetSimMsIdentity();

    VOS_MemSet(&stNasCommInfoChange, 0, sizeof(NAS_COMM_INFO_STRU));

    stNasCommInfoChange.bitOpHPlmn  = 1;
    stNasCommInfoChange.bitOpEHPlmn = 1;

    enHplmnType   = NAS_MML_GetHPlmnType();

    pstEHPlmnInfo = NAS_MML_GetSimEhplmnList();

    /* 当前HPLMN类型为HPLMN或者ACTING-HPLMN */
    if ((NAS_MML_HPLMN_TYPE_HPLMN == enHplmnType)
     || (NAS_MML_HPLMN_TYPE_ACTING_HPLMN == enHplmnType))
    {
        /* 赋值HPLMN */
        stNasCommInfoChange.stHPlmn.ulMcc = pstEHPlmnInfo->astEhPlmnInfo[0].stPlmnId.ulMcc;
        stNasCommInfoChange.stHPlmn.ulMnc = pstEHPlmnInfo->astEhPlmnInfo[0].stPlmnId.ulMnc;

        /* 将EHPLMN的个数设置为0 */
        stNasCommInfoChange.stEHPlmn.ucEHPlmnNum = 0;

    }
    /* 当前HPLMN类型为EHPLMN */
    else if (NAS_MML_HPLMN_TYPE_EHPLMN == enHplmnType)
    {
        /* 赋值HPLMN */
        stImsiHplmn = NAS_MML_GetImsiHomePlmn(pstMsIdentity->aucImsi);
        stNasCommInfoChange.stHPlmn.ulMcc = stImsiHplmn.ulMcc;
        stNasCommInfoChange.stHPlmn.ulMnc = stImsiHplmn.ulMnc;

        /* 赋值EPLMN */
        stNasCommInfoChange.stEHPlmn.ucEHPlmnNum = pstEHPlmnInfo->ucEhPlmnNum;

        for (i = 0; i < pstEHPlmnInfo->ucEhPlmnNum; i++)
        {
            stNasCommInfoChange.stEHPlmn.astEHPlmnList[i].ulMcc = pstEHPlmnInfo->astEhPlmnInfo[i].stPlmnId.ulMcc;
            stNasCommInfoChange.stEHPlmn.astEHPlmnList[i].ulMnc = pstEHPlmnInfo->astEhPlmnInfo[i].stPlmnId.ulMnc;
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsHplmnReq(): HPLMN TYPE ERROR");
        return;
    }

    /* ulReceiverPid为WRR，并且平台支持UTRAN时，给WRR发送消息 */
    if ( (VOS_TRUE == NAS_MML_IsPlatformSupportUtran())
      && (WUEPS_PID_WRR == ulReceiverPid) )
    {
        /* 向AS发送HPLMN和EHPLMN */
        NAS_MMC_SndAsNasCommInfoChangeReq(WUEPS_PID_WRR, &stNasCommInfoChange);
    }

    /* ulReceiverPid为GAS，并且平台支持GSM时，给GAS发送消息 */
    if ( (VOS_TRUE == NAS_MML_IsPlatformSupportGsm())
      && (UEPS_PID_GAS == ulReceiverPid) )
    {
        /* 向AS发送HPLMN和EHPLMN */
        NAS_MMC_SndAsNasCommInfoChangeReq(UEPS_PID_GAS, &stNasCommInfoChange);
    }
}

VOS_UINT32 NAS_GetTransactionInfo(
    RRC_NAS_CS_TRANSACTION_INFO_STRU *pstCsTransactInfo,
    RRC_NAS_PS_TRANSACTION_INFO_STRU *pstPsTransactInfo
)
{
    RRC_NAS_CS_TRANSACTION_ENUM_UINT8                       enCsTransactionstatus;

    /* 将NAS维护的CS标志类型转换为AS格式的CS状态标志 */
    NAS_MMC_GetMmlCsTransactionStatus(&enCsTransactionstatus);

    if ( VOS_NULL_PTR != pstCsTransactInfo)
    {
        pstCsTransactInfo->enCsTransactionType = enCsTransactionstatus;
    }

    if ( VOS_NULL_PTR != pstPsTransactInfo)
    {

        PS_MEM_SET(pstPsTransactInfo->aucReserve1, 0, sizeof(pstPsTransactInfo->aucReserve1));
    }

    return VOS_OK;
}
VOS_VOID NAS_MMC_SndAsNetScanReq(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat,
    MMA_MMC_NET_SCAN_REQ_STRU          *pstNetScanReq
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulReceiverPid;

    /* 定义原语类型指针 */
    RRMM_NET_SCAN_REQ_STRU             *pstMsg    = VOS_NULL_PTR;

    pstMsg = (RRMM_NET_SCAN_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(RRMM_NET_SCAN_REQ_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNetScanReq():ERROR:Memory Alloc Error for pstMsg");
        /* 返回失败 */
        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                 sizeof(RRMM_NET_SCAN_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    if ( NAS_MML_NET_RAT_TYPE_GSM == enRat )
    {
        ulReceiverPid = UEPS_PID_GAS;
    }
    else
    {
        ulReceiverPid = WUEPS_PID_WRR;
    }

    pstMsg->stMsgHeader.ulSenderCpuId             = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid               = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid             = ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName                 = RRMM_NET_SCAN_REQ;

    /* 如果是空指针 */
    NAS_MMC_ConvertNasRatToRrcFormat(pstNetScanReq->ucRat, &(pstMsg->enRat));
    pstMsg->usFreqNum                      = pstNetScanReq->usCellNum;
    pstMsg->sPower                         = pstNetScanReq->sCellPow;
    pstMsg->stBand.stGsmBand.ulGsmBand     = pstNetScanReq->stBand.unGsmBand.ulBand;
    pstMsg->stBand.stWcdmaBand.ulWcdmaBand = pstNetScanReq->stBand.unWcdmaBand.ulBand;

    PS_MEM_CPY(&pstMsg->stBand.stLteBand, &pstNetScanReq->stBand.stLteBand, sizeof(pstMsg->stBand.stLteBand));

    /*发送消息*/
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNetScanReq():ERROR:Message sends fail");
        return;
    }
}
VOS_VOID NAS_MMC_SndAsNetScanStopReq(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulReceiverPid;

    /* 定义原语类型指针 */
    RRMM_NET_SCAN_STOP_REQ_STRU        *pstMsg    = VOS_NULL_PTR;

    pstMsg = (RRMM_NET_SCAN_STOP_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(RRMM_NET_SCAN_STOP_REQ_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNetScanStopReq():ERROR:Memory Alloc Error for pstMsg");

        /* 返回失败 */
        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                 sizeof(RRMM_NET_SCAN_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    if ( NAS_MML_NET_RAT_TYPE_GSM == enRat )
    {
        ulReceiverPid = UEPS_PID_GAS;
    }
    else
    {
        ulReceiverPid = WUEPS_PID_WRR;
    }

    pstMsg->stMsgHeader.ulSenderCpuId             = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid               = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid             = ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName                 = RRMM_NET_SCAN_STOP_REQ;

    /*发送消息*/
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_MMC, (struct MsgCB **)&pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsNetScanStopReq():ERROR:Message sends fail");

        return;
    }
}






#if (FEATURE_ON == FEATURE_DSDS)

VOS_VOID NAS_MMC_SndRrmRegisterInd(
    MODEM_ID_ENUM_UINT16               enModemId,
    RRM_PS_TASK_TYPE_ENUM_UINT16       enTaskType,
    RRM_PS_RAT_TYPE_ENUM_UINT8         enRatType
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    PS_RRM_REGISTER_IND_STRU            *pstMsg    = VOS_NULL_PTR;

    pstMsg = (PS_RRM_REGISTER_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(PS_RRM_REGISTER_IND_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrmRegisterInd():ERROR:Memory Alloc Error for pstMsg");

        /* 返回失败 */
        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                 sizeof(PS_RRM_REGISTER_IND_STRU) - VOS_MSG_HEAD_LENGTH );

    pstMsg->stMsgHeader.ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid         = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid       = UEPS_PID_RRM;
    pstMsg->stMsgHeader.ulMsgName           = ID_PS_RRM_REGISTER_IND;

    pstMsg->enModemId                       = enModemId;
    pstMsg->enTaskType                      = enTaskType;
    pstMsg->enRatType                       = enRatType;

    /*发送消息*/
    ulRet = PS_SEND_MSG(WUEPS_PID_MMC, pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrmRegisterInd():ERROR:Message sends fail");

        return;
    }

    return;
}



VOS_VOID NAS_MMC_SndRrmDeRegisterInd(
    MODEM_ID_ENUM_UINT16               enModemId,
    RRM_PS_TASK_TYPE_ENUM_UINT16       enTaskType,
    RRM_PS_RAT_TYPE_ENUM_UINT8         enRatType
)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    PS_RRM_REGISTER_IND_STRU            *pstMsg    = VOS_NULL_PTR;

    pstMsg = (PS_RRM_REGISTER_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                  WUEPS_PID_MMC,
                                                  sizeof(PS_RRM_REGISTER_IND_STRU));
    /* 内存申请失败 */
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrmRegisterInd():ERROR:Memory Alloc Error for pstMsg");

        /* 返回失败 */
        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                 sizeof(PS_RRM_REGISTER_IND_STRU) - VOS_MSG_HEAD_LENGTH );

    pstMsg->stMsgHeader.ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid         = WUEPS_PID_MMC;
    pstMsg->stMsgHeader.ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid       = UEPS_PID_RRM;
    pstMsg->stMsgHeader.ulMsgName           = ID_PS_RRM_DEREGISTER_IND;

    pstMsg->enModemId                       = enModemId;
    pstMsg->enTaskType                      = enTaskType;
    pstMsg->enRatType                       = enRatType;

    /*发送消息*/
    ulRet = PS_SEND_MSG(WUEPS_PID_MMC, pstMsg);

    /* 发送失败 */
    if ( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrmDeRegisterInd():ERROR:Message sends fail");

        return;
    }

    return;
}


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

