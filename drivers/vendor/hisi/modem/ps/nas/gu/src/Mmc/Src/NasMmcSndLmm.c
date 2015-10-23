
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "om.h"
#include "NasComm.h"
#include "NasMmcComFunc.h"

#include "MmaMmcInterface.h"

#if (FEATURE_ON == FEATURE_LTE)
#include "NasMmlCtx.h"
#include "NasMmcCtx.h"
#include "NasMmcSndLmm.h"
#include "NasMmcProcNvim.h"
#include "NasMmcProcUsim.h"
#include "asnasapi.h"
#include "NasMmlLib.h"
/* 删除ExtAppMmcInterface.h*/
#include "NasMmlMsgProc.h"
#include "MmcLmmInterface.h"
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
#include "NasUtranCtrlCommFunc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_SND_LMM_C


/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
/*****************************************************************************
  3 宏定义
*****************************************************************************/


#if (FEATURE_ON == FEATURE_LTE)

enum NAS_MMC_LMM_START_RAT_MASK{

    NAS_MMC_LMM_START_GSM_MASK          = (0x01),
    NAS_MMC_LMM_START_WCDMA_MASK        = (0x02),
    NAS_MMC_LMM_START_LTE_FDD_MASK      = (0x04),
    NAS_MMC_LMM_START_LTE_TDD_MASK      = (0x08),
    NAS_MMC_LMM_START_MASK_BUTT

};


VOS_UINT32 NAS_MMC_SndLmmStopReq( VOS_VOID )
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_STOP_REQ_STRU              *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_STOP_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_STOP_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_STOP_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStopReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(MMC_LMM_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_MM;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMC;
    pstMsg->ulLength         = sizeof(MMC_LMM_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->ulMsgId          = ID_MMC_LMM_STOP_REQ;
    pstMsg->ulOpId           = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStopReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStopReq:ERROR:PS_SEND_MSG FAILURE");
        return VOS_ERR;
    }
#endif

    vos_printf("NAS_MMC_SndLmmStopReq, ulRet:%d. tick = %d\r\n ", ulRet,OM_GetSlice());

    NAS_TRACE_HIGH("Snd poweroff req to LMM!");

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmUsimStatusReq(
    MMC_LMM_USIM_STATUS_ENUM_UINT32     enUsimState
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_USIM_STATUS_REQ_STRU*       pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_USIM_STATUS_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_USIM_STATUS_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_USIM_STATUS_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_USIM_STATUS_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUsimStatusReq:ERROR: Memory Alloc Error for pMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                     sizeof(MMC_LMM_USIM_STATUS_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_USIM_STATUS_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_USIM_STATUS_REQ;
    pstMsg->ulOpId                      = 0;
    pstMsg->ulUsimState                 = enUsimState;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUsimStatusReq:ERROR: SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUsimStatusReq:ERROR: SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

VOS_VOID NAS_MMC_SndLmmPlmnAnyCellSrchReq(VOS_VOID)
{
    /* 返回值变量 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnAnyCellSrchReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    pstMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_PLMN_SRCH_REQ;
    pstMsg->ulOpId                          = 0;
    pstMsg->enSrchType                      = MMC_LMM_PLMN_SRCH_ANY;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
     {
         NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnAnyCellSrchReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
         return;
     }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
     {
         NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnAnyCellSrchReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
         return;
     }
#endif

}



VOS_VOID NAS_MMC_SndLmmPlmnListReq(VOS_VOID)
{
    /* 返回值变量 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnListReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    pstMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_PLMN_SRCH_REQ;
    pstMsg->ulOpId                          = 0;
    pstMsg->enSrchType                      = MMC_LMM_PLMN_SRCH_LIST;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnListReq():WARNING:SEND LMM PLMN list SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnListReq():WARNING:SEND LMM PLMN list SEARCH REQ FIAL");
        return;
    }
#endif

}


VOS_VOID NAS_MMC_SndLmmSuspendReq(VOS_VOID)
{
    /* 定义调用VOS发送函数的返回值  */
    VOS_UINT32                          ulRet;
    /* 定义原语类型指针 */
    MMC_LMM_SUSPEND_REQ_STRU           *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_SUSPEND_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_SUSPEND_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_SUSPEND_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_SUSPEND_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                     sizeof(MMC_LMM_SUSPEND_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_SUSPEND_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_SUSPEND_REQ;
    pstMsg->ulOpId                      = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
        return;
    }
#endif

#ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif



}

VOS_VOID NAS_MMC_SndLmmPlmnSrchStopReq( VOS_VOID )
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_STOP_PLMN_SRCH_REQ_STRU    *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_STOP_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_STOP_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
        pstMsg = (MMC_LMM_STOP_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_STOP_PLMN_SRCH_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnSrchStopReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_STOP_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_STOP_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_STOP_PLMN_SRCH_REQ;
    pstMsg->ulOpId                      = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnSrchStopReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmPlmnSrchStopReq():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
    }
#endif

    return;
}
VOS_VOID NAS_MMC_SndLmmStartReq(VOS_VOID)
{
    MMC_LMM_START_REQ_STRU             *pstMsg     = VOS_NULL_PTR;
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatList = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_START_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                        sizeof(MMC_LMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_START_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_START_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStartReq():ERROR:Memory Alloc Error for pstMsg");

        return ;
    }

    PS_MEM_SET(((VOS_UCHAR *)pstMsg) + VOS_MSG_HEAD_LENGTH, 0,
                                         sizeof(MMC_LMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_START_REQ;

    pstMsg->enUtranMode     = NAS_UTRANCTRL_ConvertNasUtranModeToLmmUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 获取卡状态 */
    if (VOS_TRUE == NAS_MML_GetSimPresentStatus())
    {
        pstMsg->ulUsimState             = MMC_LMM_USIM_VALID;
    }
    else
    {
        pstMsg->ulUsimState             = MMC_LMM_USIM_INVALID;
    }

    /* 取得接入优先级列表 */
    pstRatList  = NAS_MML_GetMsPrioRatList();

    /* 设置接入模式 */
    pstMsg->aucRatPrioList[0]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_GSM, pstRatList);
    pstMsg->aucRatPrioList[1]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_WCDMA, pstRatList);
    pstMsg->aucRatPrioList[2]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_LTE, pstRatList);

    /* pstMsg->aucRatPrioList[3]记录的是CDMA1X的优先级，目前阶段只支持HRPD，不支持CDMA1X */
    pstMsg->aucRatPrioList[3]              = RRMM_RAT_PRIO_NULL;

    /* 如果有支持的3GPP2接入技术pstMsg->aucRatPrioList[4]填写为HRPD的优先级RRMM_RAT_PRIO_MIDDLE */
    if (VOS_TRUE == NAS_MML_IsSpec3Gpp2RatInRatList(NAS_MML_3GPP2_RAT_TYPE_HRPD, NAS_MML_GetMs3Gpp2PrioRatList()))
    {
        pstMsg->aucRatPrioList[4]          = RRMM_RAT_PRIO_MIDDLE;
    }
    /* 否则pstMsg->aucRatPrioList[4]填写为RRMM_RAT_PRIO_NULL */
    else
    {
        pstMsg->aucRatPrioList[4]          = RRMM_RAT_PRIO_NULL;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStartReq():WARNING:SEND LMM START REQ FIAL");
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmStartReq():WARNING:SEND LMM START REQ FIAL");
    }
#endif
    vos_printf("NAS_MMC_SndLmmStartReq, ulRet:%d. tick = %d\r\n ", ulRet,OM_GetSlice());

    return ;
}


VOS_VOID NAS_MMC_BuildSndLmmEquPlmnInfo(
     NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo
)
{
    NAS_MML_PLMN_ID_STRU                                    astUniquePlmnList[NAS_MML_MAX_EQUPLMN_NUM];          /* 删除重复项后的EquPLMN列表 */
    VOS_UINT32                                              aulComparedMcc[NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER]; /*允许LTE漫游的MCC列表*/
    VOS_UINT8                                               ucSndEplmnNum;
    VOS_UINT8                                               ucEmcPdpStatusFlg;
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU                *pstDisabledRatPlmnCfg = VOS_NULL_PTR;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbidPlmnInfo = VOS_NULL_PTR;

    ucEmcPdpStatusFlg = NAS_MML_GetEmcPdpStatusFlg();

            
    /* 获取禁止接入技术的PLMN信息 */
    pstDisabledRatPlmnCfg = NAS_MML_GetDisabledRatPlmnCfg();
    
    /* 获取运营商定制的MCC列表 */
    NAS_MML_GetRoamEnabledMccList(aulComparedMcc);

    /* 获取forbidden列表 */
    pstForbidPlmnInfo = NAS_MML_GetForbidPlmnInfo();

    ucSndEplmnNum   = pstEquPlmnInfo->ucEquPlmnNum;
    PS_MEM_CPY(astUniquePlmnList, pstEquPlmnInfo->astEquPlmnAddr, ucSndEplmnNum * sizeof(NAS_MML_PLMN_ID_STRU));
    
    /* 从列表中删除重复的网络 */
    NAS_MMC_DelDuplicatedPlmnInPlmnList(&ucSndEplmnNum, astUniquePlmnList);


    /* 紧急PDN连接不存在时，需要删除forbidden plmn\forbidden plmn for gprs\disabled rat plmn列表的EPLMN */
    if (VOS_FALSE == ucEmcPdpStatusFlg)
    {
        /* 从列表中删除不允许LTE漫游的网络 */
        NAS_MML_DelEqualPlmnNotInAllowLteRoamMccList(&ucSndEplmnNum, astUniquePlmnList, NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER, aulComparedMcc);

        /* 删除禁止的EPLMN，包括forb plmn, 锁网锁卡的PLMN, 不允许漫游的plmn, 卡无效等 */
        NAS_MML_DelForbPlmnInPlmnList(&ucSndEplmnNum, astUniquePlmnList);

        /* 删除forb plmn for gprs */
        NAS_MML_DelPlmnsInRefList(&ucSndEplmnNum, astUniquePlmnList,
                       pstForbidPlmnInfo->ucForbGprsPlmnNum, pstForbidPlmnInfo->astForbGprsPlmnList);
        
        /* 删除在禁止接入技术的PLMN列表中的EPLMN */
        NAS_MML_DelEqualPlmnsInDisableRatList(&ucSndEplmnNum, 
                                              astUniquePlmnList, 
                                              NAS_MML_NET_RAT_TYPE_LTE,
                                              pstDisabledRatPlmnCfg->ulDisabledRatPlmnNum, 
                                              pstDisabledRatPlmnCfg->astDisabledRatPlmnId);

    }

    pstEquPlmnInfo->ucEquPlmnNum = ucSndEplmnNum;
    PS_MEM_CPY(pstEquPlmnInfo->astEquPlmnAddr, astUniquePlmnList, ucSndEplmnNum * sizeof(NAS_MML_PLMN_ID_STRU));

    return;
}






VOS_UINT32 NAS_MMC_SndLmmEquPlmnReq(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo
)
{
    VOS_UINT32                          i;
    MMC_LMM_EPLMN_NOTIFY_REQ_STRU      *pstLmmEquPlmnStru = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstLmmEquPlmnStru = (MMC_LMM_EPLMN_NOTIFY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_EPLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstLmmEquPlmnStru = (MMC_LMM_EPLMN_NOTIFY_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_EPLMN_NOTIFY_REQ_STRU));
#endif

    /* 若分配内存出错,返回失败 */
    if ( VOS_NULL_PTR == pstLmmEquPlmnStru )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEquPlmnReq():ERROR:Memory Alloc Error for pstMsg");

        return  VOS_ERR;
    }

    PS_MEM_SET( (VOS_INT8*)pstLmmEquPlmnStru + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_EPLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstLmmEquPlmnStru->ulReceiverPid   = PS_PID_MM;
    pstLmmEquPlmnStru->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstLmmEquPlmnStru->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstLmmEquPlmnStru->ulSenderPid     = WUEPS_PID_MMC;
    pstLmmEquPlmnStru->ulMsgId         = ID_MMC_LMM_EPLMN_NOTIFY_REQ;
    pstLmmEquPlmnStru->ulLength        = sizeof(MMC_LMM_EPLMN_NOTIFY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填充Eplmn,转换为LMM的格式 */
    pstLmmEquPlmnStru->ulEplmnNum = pstEquPlmnInfo->ucEquPlmnNum;

    for ( i = 0 ; i < pstEquPlmnInfo->ucEquPlmnNum ; i++ )
    {
        NAS_MML_ConvertNasPlmnToLMMFormat(&(pstLmmEquPlmnStru->astEplmnList[i]),
                                          &(pstEquPlmnInfo->astEquPlmnAddr[i]));
    }
#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstLmmEquPlmnStru );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEquPlmnReq():ERROR:SEND MESSAGE FAILURE");

        return  VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstLmmEquPlmnStru);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEquPlmnReq():ERROR:SEND MESSAGE FAILURE");

        return  VOS_ERR;
    }
#endif

    return VOS_OK;
}


VOS_UINT32 NAS_MMC_SndLmmLocalDetachIndActionResultReq(
    MMC_LMM_GU_LOCAL_DETACH_TYPE_ENUM_UINT32                ulLocalDetachType
)
{
    VOS_UINT32                          ulRet;
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
              sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLocalDetachIndActionResultReq():ERROR:Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_ACTION_RESULT_REQ;
    pstMsg->ulOpId                      = 0;
    pstMsg->enActionType                = MMC_LMM_ACTION_LOCAL_DETACH;
    pstMsg->bitOpLocalDetachType        = VOS_TRUE;
    pstMsg->ulLocalDetachType           = ulLocalDetachType;
    pstMsg->ulActRst                    = MMC_LMM_RSLT_TYPE_SUCCESS;
    pstMsg->bitOpReqDomain              = VOS_TRUE;
    pstMsg->bitOpRstDomain              = VOS_TRUE;
    pstMsg->ulReqDomain                 = MMC_LMM_PS_ONLY;
    pstMsg->ulRstDomain                 = MMC_LMM_PS_ONLY;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLocalDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLocalDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmMoDetachIndActionResultReq(
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32  ulMoDetachType
)
{
    VOS_UINT32                          ulRet;
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
              sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMoDetachIndActionResultReq():ERROR:Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_ACTION_RESULT_REQ;
    pstMsg->ulOpId                      = 0;
    pstMsg->enActionType                = MMC_LMM_ACTION_MO_DETACH;
    pstMsg->bitOpMoDetachType           = VOS_TRUE;
    pstMsg->ulMoDetachType              = ulMoDetachType;
    pstMsg->ulActRst                    = MMC_LMM_RSLT_TYPE_SUCCESS;

    if (MMC_LMM_MO_DET_PS_ONLY == ulMoDetachType)
    {
        pstMsg->bitOpReqDomain = VOS_TRUE;
        pstMsg->bitOpRstDomain = VOS_TRUE;
        pstMsg->ulReqDomain    = MMC_LMM_PS_ONLY;
        pstMsg->ulRstDomain    = MMC_LMM_PS_ONLY;
    }
    else if (MMC_LMM_MO_DET_CS_ONLY == ulMoDetachType)
    {
        pstMsg->bitOpReqDomain = VOS_TRUE;
        pstMsg->bitOpRstDomain = VOS_TRUE;
        pstMsg->ulReqDomain    = MMC_LMM_IMSI_ONLY;
        pstMsg->ulRstDomain    = MMC_LMM_IMSI_ONLY;
    }
    else if(MMC_LMM_MO_DET_CS_PS == ulMoDetachType)
    {
        pstMsg->bitOpReqDomain = VOS_TRUE;
        pstMsg->bitOpRstDomain = VOS_TRUE;
        pstMsg->ulReqDomain    = MMC_LMM_COMBINED_PS_IMSI;
        pstMsg->ulRstDomain    = MMC_LMM_COMBINED_PS_IMSI;
    }
    else
    {
    }
#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMoDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMoDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmMtDetachIndActionResultReq(
    NAS_MMC_GMM_NT_DETACH_ENUM_U32      enMtType,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCnCause
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMtDetachIndActionResultReq():ERROR:Memory Alloc Error for pstMsg");

        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    NAS_MMC_BuildMmcLmmDetachIndActionResultReqMsg(enMtType, enCnCause, pstMsg);

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMtDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmMtDetachIndActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmRegActionResultReq(
    NAS_MMC_GU_ACTION_RSLT_INFO_STRU   *pstGuActionRslt
)
{
    VOS_UINT32                          ulRet;
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    NAS_LMM_CN_CAUSE_ENUM_UINT8         enCnRejCause;
    MMC_LMM_DOMAIN_ENUM_UINT32          enLmmDomain;
    MMC_LMM_ACTION_TYPE_ENUM_UINT32     enActionType;

    /* 判断Action类型是否正确 */
    if ( VOS_OK != NAS_MMC_FormatProcTypeToLmm(pstGuActionRslt->enProcType, &enActionType))
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRegActionResultReq():NAS_MMC_FormatProcTypeToLmm:RETURNS FAILURE");

        return VOS_ERR;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ACTION_RESULT_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRegActionResultReq():ERROR:Memory Alloc Error for pstMsg");

        return VOS_ERR;
    }

    /* 初始化内存 */
    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ucCnCause = NAS_EMM_CAUSE_BUTT;


    /* 填充消息发送内容 */
    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    pstMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_ACTION_RESULT_REQ;

    pstMsg->ulOpId                          = 0;
    pstMsg->bitOpAtmpCnt                    = VOS_TRUE;

    pstMsg->ulAttemptCount                  = pstGuActionRslt->ulAttemptCount;
    pstMsg->ulMtDetachType                  = MMC_LMM_MT_DETACH_TYPE_BUTT;
    pstMsg->enActionType                    = enActionType;

    /* 获取注册结果 */
    NAS_MMC_ConvertGuRsltToLmmFormat(pstGuActionRslt->enRegRst,
                                     pstGuActionRslt->enCnCause,
                                     &(pstMsg->ulActRst));


    /* 获取注册原因值 */
    if ( VOS_OK == NAS_MMC_ConvertMmCauseToEmmCause(pstGuActionRslt->enCnCause, &enCnRejCause) )
    {
        pstMsg->bitOpCnCause                = VOS_TRUE;
        pstMsg->ucCnCause                   = enCnRejCause;
    }

    if ( VOS_OK == NAS_MMC_ConverCnDomainToLmm(pstGuActionRslt->enReqDomain, &enLmmDomain) )
    {
        pstMsg->bitOpReqDomain              = VOS_TRUE;
        pstMsg->ulReqDomain                 = enLmmDomain;
    }

    if ( VOS_OK == NAS_MMC_ConverCnDomainToLmm(pstGuActionRslt->enRstDomain, &enLmmDomain) )
    {
        pstMsg->bitOpRstDomain              = VOS_TRUE;
        pstMsg->ulRstDomain                 = enLmmDomain;
    }

    /* 填充CS update状态和Lai是否改变标志 */
    NAS_MMC_FillCsRegInfoInLmmActionRslt(pstMsg);

    /* 填充PS update状态和Rai是否改变标志 */
    NAS_MMC_FillGprsRegInfoInLmmActionRslt(pstMsg);

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRegActionResultReq():ERROR:SEND MESSAGE FAILURE");

        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRegActionResultReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}


VOS_UINT32 NAS_MMC_SndLmmRelReq( VOS_VOID )
{

    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_REL_REQ_STRU               *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_REL_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_REL_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_REL_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRelReq,ERROR:ALLOC MEM ERROR!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_REL_REQ;
    pstMsg->ulOpId                      = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRelReq,ERROR:SNED MSG FAIL!");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmRelReq,ERROR:SNED MSG FAIL!");
        return VOS_ERR;
    }

#endif

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmAttachReq(
    VOS_UINT32                                              ulOpId,
    VOS_UINT32                                              ulAttachType,
    MMA_MMC_EPS_ATTACH_REASON_ENUM_UINT32                   enEpsAttachReason
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_ATTACH_REQ_STRU            *pstMsg = VOS_NULL_PTR;

    MMC_LMM_ATT_REQ_TYPE_ENUM_UINT32    enAttachType;

    if ( VOS_OK != NAS_MMC_ConvertAttachTypeToLmm(ulAttachType, &enAttachType) )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmAttachReq():NAS_MMC_ConvertAttachTypeToLmm:RETURNS FAILURE");

        return VOS_ERR;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ATTACH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ATTACH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ATTACH_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                    sizeof(MMC_LMM_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_ATTACH_REQ;
    pstMsg->ulOpId                      = ulOpId;
    pstMsg->ulAttachType                = enAttachType;

    pstMsg->ulAttachReason              = enEpsAttachReason;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmAttachReq():ERROR:SEND MESSAGE FAILURE");

        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmAttachReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32 NAS_MMC_SndLmmDetachReq(
    VOS_UINT32                          ulOpId,
    VOS_UINT32                          ulDetachType,
    MMA_MMC_DETACH_REASON_ENUM_UINT32   enDetachReason
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_DETACH_REQ_STRU            *pstMsg = VOS_NULL_PTR;
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32  enLmmDetachType;

    if ( VOS_OK != NAS_MMC_ConvertDetachTypeToLmm(ulDetachType, &enLmmDetachType) )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDetachReq():NAS_MMC_ConvertDetachTypeToLmm:RETURNS FAILURE");

        return VOS_ERR;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_DETACH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_DETACH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_DETACH_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_DETACH_REQ;
    pstMsg->ulOpId                      = ulOpId;
    pstMsg->ulDetachType                = enLmmDetachType;

    pstMsg->ulDetachReason              = enDetachReason;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDetachReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDetachReq():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32  NAS_MMC_SndLmmResumeNotify( VOS_VOID )
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_RESUME_NOTIFY_STRU         *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_RESUME_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_RESUME_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_RESUME_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_RESUME_NOTIFY_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmResumeNotify():ERROR:Memory Alloc Error for pstMsg");

        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_RESUME_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_RESUME_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_RESUME_NOTIFY;
    pstMsg->ulOpId                      = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmResumeNotify():ERROR:SEND MESSAGE FAILURE");

        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmResumeNotify():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_UINT32  NAS_MMC_SndLmmSuspendRsp(
    VOS_UINT32                          ulRst
)
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_SUSPEND_RSP_STRU           *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_SUSPEND_RSP_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_SUSPEND_RSP_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_SUSPEND_RSP_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_SUSPEND_RSP_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRsp():ERROR:Memory Alloc Error for pstMsg");

        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_SUSPEND_RSP_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_SUSPEND_RSP_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_SUSPEND_RSP;
    pstMsg->ulOpId                      = 0;
    pstMsg->ulRst                       = ulRst;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRsp():ERROR:SEND MESSAGE FAILURE");

        return VOS_ERR;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRsp():ERROR:SEND MESSAGE FAILURE");
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
VOS_VOID  NAS_MMC_SndLmmSyscfgReq(
    MMA_MMC_SYS_CFG_SET_REQ_STRU        *pstSysCfgSetParm
)
{
    NAS_MML_PLMN_RAT_PRIO_STRU          stRatList;
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_SYS_CFG_REQ_STRU           *pstSndLmmSysCfgMsg  = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstSndLmmSysCfgMsg = (MMC_LMM_SYS_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstSndLmmSysCfgMsg = (MMC_LMM_SYS_CFG_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_SYS_CFG_REQ_STRU));
#endif

    if( VOS_NULL_PTR == pstSndLmmSysCfgMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSyscfgReq,ERRORALLOC MEM MMC_LMM_SYS_CFG_REQ_STRU FAIL");
        return ;
    }

    PS_MEM_SET( (VOS_INT8*)pstSndLmmSysCfgMsg + VOS_MSG_HEAD_LENGTH, 0X0,
                sizeof(MMC_LMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstSndLmmSysCfgMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstSndLmmSysCfgMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstSndLmmSysCfgMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstSndLmmSysCfgMsg->ulReceiverPid                   = PS_PID_MM;
    pstSndLmmSysCfgMsg->ulLength                        = sizeof(MMC_LMM_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSndLmmSysCfgMsg->ulMsgId                         = ID_MMC_LMM_SYS_CFG_REQ;
    pstSndLmmSysCfgMsg->ulOpId                          = 0;

    /* 设置OPT为VOS_TRUE */
    pstSndLmmSysCfgMsg->bitOpBand                       = VOS_TRUE;
    pstSndLmmSysCfgMsg->bitOpRat                        = VOS_TRUE;

    /* 设置频段信息 */
    PS_MEM_CPY( &(pstSndLmmSysCfgMsg->stBand.stLteBand ),
                &(pstSysCfgSetParm->stBand.stLteBand),
                sizeof(LTE_BAND_STRU) );

    pstSndLmmSysCfgMsg->stBand.stWcdmaBand.ulWcdmaBand = pstSysCfgSetParm->stBand.unWcdmaBand.ulBand;
    pstSndLmmSysCfgMsg->stBand.stGsmBand.ulGsmBand     = pstSysCfgSetParm->stBand.unGsmBand.ulBand;

    /* 设置接入模式 */
    PS_MEM_CPY(&stRatList, &(pstSysCfgSetParm->stRatPrioList), sizeof(NAS_MML_PLMN_RAT_PRIO_STRU));
    pstSndLmmSysCfgMsg->aucRatPrioList[0]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_GSM, &stRatList);
    pstSndLmmSysCfgMsg->aucRatPrioList[1]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_WCDMA, &stRatList);
    pstSndLmmSysCfgMsg->aucRatPrioList[2]              = NAS_MMC_GetSpecRatPrio(NAS_MML_NET_RAT_TYPE_LTE, &stRatList);

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstSndLmmSysCfgMsg );
    if( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrMmCellSelReq():ERROR:SEND MESSAGE FAILURE");
        return ;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstSndLmmSysCfgMsg);
    if( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndRrMmCellSelReq():ERROR:SEND MESSAGE FAILURE");
        return ;
    }
#endif

    return ;
}


VOS_VOID  NAS_MMC_SndLmmDisableLteNotify(
    MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32                  enDisableLteReason
)
{
    /* 返回值变量 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_DISABLE_LTE_NOTIFY_STRU    *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_DISABLE_LTE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_DISABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_DISABLE_LTE_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_DISABLE_LTE_NOTIFY_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDisableLteNotify():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_DISABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    pstMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_DISABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_DISABLE_LTE_NOTIFY;
    pstMsg->ulOpId                          = 0;
    pstMsg->enDisableLteReason              = enDisableLteReason;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDisableLteNotify():WARNING:SEND LMM disable lte notify FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmDisableLteNotify():WARNING:SEND LMM disable lte notify FIAL");
        return;
    }
#endif


}


VOS_VOID  NAS_MMC_SndLmmEnableLteNotify(VOS_VOID)
{
    /* 返回值变量 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_ENABLE_LTE_NOTIFY_STRU     *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_ENABLE_LTE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_ENABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_ENABLE_LTE_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_ENABLE_LTE_NOTIFY_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEnableLteNotify():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH, 0,
                         sizeof(MMC_LMM_ENABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    pstMsg->ulSenderCpuId                   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                     = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_ENABLE_LTE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_ENABLE_LTE_NOTIFY;
    pstMsg->ulOpId                          = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEnableLteNotify():WARNING:SEND LMM enbale lte notify FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEnableLteNotify():WARNING:SEND LMM enbale lte notify FIAL");
        return;
    }
#endif

}


VOS_UINT32  NAS_MMC_FormatProcTypeToLmm(
    NAS_MML_PROC_TYPE_ENUM_U32          enProcType,
    MMC_LMM_ACTION_TYPE_ENUM_UINT32    *penActionType
)
{
    switch (enProcType)
    {
        case NAS_MML_PROC_ATTACH:
        case NAS_MML_PROC_COMBINED_ATTACH:
            *penActionType = MMC_LMM_ACTION_ATTACH;
            break;

        case NAS_MML_PROC_MT_DETACH:
            *penActionType = MMC_LMM_ACTION_MT_DETACH;
            break;

        case NAS_MML_PROC_LOCAL_DETACH:
           *penActionType = MMC_LMM_ACTION_LOCAL_DETACH;
           break;

        case NAS_MML_PROC_UE_DETACH:
            *penActionType = MMC_LMM_ACTION_MO_DETACH;
             break;

        case NAS_MML_PROC_RAU:
        case NAS_MML_PROC_COMBINED_RAU:
            *penActionType = MMC_LMM_ACTION_RAU;

            break;

        case NAS_MML_PROC_PERIODC_RAU:
            *penActionType = MMC_LMM_ACTION_PERIODC_RAU;
            break;


        case NAS_MML_PROC_LAU:
            *penActionType = MMC_LMM_ACTION_LAU;
            break;

        case NAS_MML_PROC_SER:
            *penActionType = MMC_LMM_ACTION_PS_SERVICE_REQ;
            break;

        case NAS_MML_PROC_CM_SER:
            *penActionType = MMC_LMM_ACTION_CM_REQ;
            break;

        /* 新增Abort类型转换 */
        case NAS_MML_PROC_MM_ABORT:
            *penActionType = MMC_LMM_ACTION_MM_ABORT;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32  NAS_MMC_ConvertAttachTypeToLmm(
    VOS_UINT32                          ulAttachType,
    MMC_LMM_ATT_REQ_TYPE_ENUM_UINT32   *penLmmAttachType
)
{
    switch (ulAttachType)
    {
        case MMA_MMC_ATTACH_TYPE_GPRS:
            *penLmmAttachType = MMC_LMM_ATT_REQ_TYPE_PS_ONLY;
            break;

        case MMA_MMC_ATTACH_TYPE_GPRS_IMSI:
            *penLmmAttachType = MMC_LMM_ATT_REQ_TYPE_CS_PS;
            break;

        case MMA_MMC_ATTACH_TYPE_IMSI:
            *penLmmAttachType = MMC_LMM_ATT_REQ_TYPE_CS_ONLY;
            break;

        default:
            return VOS_ERR;

    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : NAS_MMC_ConvertLmmPlmnToGUNasFormat
 功能描述  : 将LMM的3字节格式PLMN转换为MCC和MNC分开的格式
 输入参数  : pLMMPlmn
 输出参数  : pGUNasPlmn
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年3月28日
    作    者   : likelai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MMC_ConvertLmmPlmnToGUNasFormat(
    MMC_LMM_PLMN_ID_STRU               *pstLMMPlmn,
    NAS_MML_PLMN_ID_STRU               *pstGUNasPlmn
)
{
    NAS_MML_SIM_FORMAT_PLMN_ID          stSimPlmn;

    /* 先转换为SIM格式 */
    PS_MEM_CPY(stSimPlmn.aucSimPlmn, pstLMMPlmn->aucPlmnId, sizeof(pstLMMPlmn->aucPlmnId));

    NAS_MMC_ConvertSimPlmnToNasPLMN(&stSimPlmn, pstGUNasPlmn);

    return;
}
VOS_VOID NAS_MMC_ConvertLmmPlmnToGUNasPlmn(
    VOS_UINT32                          ulLmmPlmnNum,
    MMC_LMM_PLMN_ID_STRU               *pstLmmPlmn,
    NAS_MML_PLMN_ID_STRU               *pstGUNasPlmn
)
{
    VOS_UINT32                          i;

    for ( i = 0; i < ulLmmPlmnNum ; i++  )
    {
        NAS_MMC_ConvertLmmPlmnToGUNasFormat( &(pstLmmPlmn[i]), &(pstGUNasPlmn[i]) );
    }
}


VOS_UINT32  NAS_MMC_ConvertDetachTypeToLmm(
    VOS_UINT32                          ulDetachType,
    MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32 *penLmmDetachType
)
{
    if ( MMA_MMC_DETACH_TYPE_GPRS          == ulDetachType )
    {
        *penLmmDetachType = MMC_LMM_MO_DET_PS_ONLY;
    }
    else if ( MMA_MMC_DETACH_TYPE_GPRS_IMSI  == ulDetachType )
    {
        *penLmmDetachType = MMC_LMM_MO_DET_CS_PS;
    }
    else if ( MMA_MMC_DETACH_TYPE_IMSI  == ulDetachType )
    {
        *penLmmDetachType = MMC_LMM_MO_DET_CS_ONLY;
    }
    else
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetTinTypeToLmm
 功能描述  : 获取TIN类型给LMM
 输入参数  : 无
 输出参数  : penTinType:LMM的TIN类型
 返 回 值  : VOS_OK:获取成功,VOS_ERR:获取失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月28日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  NAS_MMC_GetTinTypeToLmm(
    MMC_LMM_TIN_TYPE_ENUM_UINT32        *penTinType
)
{
    NAS_MML_TIN_TYPE_ENUM_UINT8         enTinType;

    enTinType = NAS_MML_GetTinType();
    if ( NAS_MML_TIN_TYPE_GUTI == enTinType )
    {
        *penTinType = MMC_LMM_TIN_GUTI;
    }
    else if ( NAS_MML_TIN_TYPE_PTMSI == enTinType )
    {
        *penTinType = MMC_LMM_TIN_P_TMSI;
    }
    else if ( NAS_MML_TIN_TYPE_RAT_RELATED_TMSI == enTinType )
    {
        *penTinType = MMC_LMM_TIN_RAT_RELATED_TMSI;
    }
    else
    {
        return VOS_ERR;
    }

    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetEplmnToLmm
 功能描述  : 获取EPLMN通知LMM
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK:获取成功,VOS_ERR:获取失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月28日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  NAS_MMC_GetEplmnToLmm(
    MMC_LMM_EPLMN_STRU                  *pstEplmnList
)
{
    NAS_MML_EQUPLMN_INFO_STRU          *pstGuEplmnAddr;
    VOS_UINT32                          i;

    pstGuEplmnAddr          = NAS_MML_GetEquPlmnList();
    pstEplmnList->ulPlmnNum = pstGuEplmnAddr->ucEquPlmnNum;

    for ( i = 0 ; i <  pstEplmnList->ulPlmnNum; i++ )
    {
        NAS_MML_ConvertNasPlmnToLMMFormat(&(pstEplmnList->astEplmnList[i]),
                                          &(pstGuEplmnAddr->astEquPlmnAddr[i]));
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetFplmnToLmm
 功能描述  : 获取FPLMN通知LMM
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK:获取成功,VOS_ERR:获取失败
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年4月28日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  NAS_MMC_GetFplmnToLmm(
    MMC_LMM_PLMN_LIST_STRU              *pstFplmnList
)
{
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo;
    VOS_UINT8                           ucFplmnNum;
    VOS_UINT32                          i;

    pstForbidPlmnInfo       = NAS_MML_GetForbidPlmnInfo();
    ucFplmnNum              = pstForbidPlmnInfo->ucForbPlmnNum;
    pstFplmnList->ulPlmnNum = ucFplmnNum;

    for ( i = 0 ; i <  ucFplmnNum; i++ )
    {
        NAS_MML_ConvertNasPlmnToLMMFormat(&(pstFplmnList->astPlmnId[i]),
                                          &(pstForbidPlmnInfo->astForbPlmnIdList[i]));
    }

    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetFplmnForGprsToLmm
 功能描述  : 获取FPLMN for Gprs通知LMM
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK:获取成功,VOS_ERR:获取失败
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年4月28日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  NAS_MMC_GetFplmnForGprsToLmm(
    MMC_LMM_PLMN_LIST_STRU              *pstFplmnForGprsList
)
{
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo;
    VOS_UINT8                           ucFplmnNum;
    VOS_UINT32                          i;

    pstForbidPlmnInfo       = NAS_MML_GetForbidPlmnInfo();
    ucFplmnNum              = pstForbidPlmnInfo->ucForbGprsPlmnNum;

    pstFplmnForGprsList->ulPlmnNum = ucFplmnNum;

    for ( i = 0 ; i <  ucFplmnNum; i++ )
    {
        NAS_MML_ConvertNasPlmnToLMMFormat(&(pstFplmnForGprsList->astPlmnId[i]),
                                          &(pstForbidPlmnInfo->astForbGprsPlmnList[i]));
    }

    return VOS_OK;

}



VOS_UINT32  NAS_MMC_GetEHplmnToLmm(
    MMC_LMM_PLMN_LIST_STRU             *pstEHplmnList
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulHplmnNum ;
    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstEhplmnInfo = VOS_NULL_PTR;

    pstEhplmnInfo = NAS_MML_GetSimEhplmnList();

    if (NAS_MML_HPLMN_TYPE_EHPLMN == NAS_MML_GetHPlmnType())
    {
        /* EHPLMN的实际个数 */
        ulHplmnNum   = pstEhplmnInfo->ucEhPlmnNum;

        pstEHplmnList->ulPlmnNum = ulHplmnNum;

        /* 转换为LMM需要的格式 */
        for ( i = 0 ; i < ulHplmnNum; i++ )
        {
            NAS_MML_ConvertNasPlmnToLMMFormat(&(pstEHplmnList->astPlmnId[i]),
                                              &(pstEhplmnInfo->astEhPlmnInfo[i].stPlmnId));
        }
    }
    else
    {
        pstEHplmnList->ulPlmnNum = 0;
    }

    return VOS_OK;
}


VOS_UINT32  NAS_MMC_GetHplmnToLmm(
    MMC_LMM_PLMN_ID_STRU               *pstHplmn
)
{
    NAS_MML_SIM_EHPLMN_INFO_STRU       *pstEHPlmnInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                stPlmnId;
    NAS_MML_HPLMN_TYPE_ENUM_UINT8       enHplmnType;

    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity   = VOS_NULL_PTR;

    pstMsIdentity                       = NAS_MML_GetSimMsIdentity();

    enHplmnType   = NAS_MML_GetHPlmnType();
    pstEHPlmnInfo = NAS_MML_GetSimEhplmnList();

    /* 当前HPLMN类型为HPLMN或者ACTING-HPLMN */
    if ((NAS_MML_HPLMN_TYPE_HPLMN == enHplmnType)
     || (NAS_MML_HPLMN_TYPE_ACTING_HPLMN == enHplmnType))
    {
        /* 赋值HPLMN */
        stPlmnId = pstEHPlmnInfo->astEhPlmnInfo[0].stPlmnId;
    }
    /* 当前HPLMN类型为EHPLMN */
    else if (NAS_MML_HPLMN_TYPE_EHPLMN == enHplmnType)
    {
        /* 赋值HPLMN */
        stPlmnId = NAS_MML_GetImsiHomePlmn(pstMsIdentity->aucImsi);
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndAsHplmnReq(): HPLMN TYPE ERROR");
        return VOS_ERR;
    }

    NAS_MML_ConvertNasPlmnToLMMFormat(pstHplmn, &stPlmnId);

    return VOS_OK;
}


MMC_LMM_RESULT_ID_ENUM_UINT32 Nas_GetPubInfo(
    MMC_LMM_INFO_TYPE_ENUM_UINT32       ulInfoType,
    MMC_LMM_PUB_INFO_STRU              *pstPubInfo
)
{
    VOS_UINT32                          ulRet;

    ulRet   = VOS_ERR;

    switch (ulInfoType)
    {
        case MMC_LMM_TIN_TYPE:
            ulRet = NAS_MMC_GetTinTypeToLmm(&(pstPubInfo->u.ulTinType));
            break;

        case MMC_LMM_EPLMN:
            ulRet = NAS_MMC_GetEplmnToLmm(&(pstPubInfo->u.stEplmnList));
            break;

        case MMC_LMM_FPLMN:
            ulRet = NAS_MMC_GetFplmnToLmm(&(pstPubInfo->u.stForbiddenPlmnList));
            break;

        case MMC_LMM_FPLMN_FOR_GPRS:
            ulRet = NAS_MMC_GetFplmnForGprsToLmm(&(pstPubInfo->u.stForbiddenPlmnListForGPRS));
            break;

        case MMC_LMM_EHPLMN:
            ulRet = NAS_MMC_GetEHplmnToLmm(&(pstPubInfo->u.stEHplmnList));
            break;

        case MMC_LMM_HPLMN:
            ulRet = NAS_MMC_GetHplmnToLmm(&(pstPubInfo->u.stHplmn));
            break;

        default:
            break;

    }

    if ( VOS_OK == ulRet )
    {
        return MMC_LMM_SUCC;
    }

    return MMC_LMM_FAIL;
}


VOS_VOID NAS_MMC_FillGprsRegInfoInLmmActionRslt(
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg
)
{
    /* 如果不携带ulAttemptCount，直接返回 */
    if (VOS_FALSE == pstMsg->bitOpAtmpCnt)
    {
        return;
    }

    /* 如果ulAttemptCount为0，直接返回 */
    if (0 == pstMsg->ulAttemptCount)
    {
        return;
    }

    /* 如果是联合RAU或联合ATTACH，ulCombReg为TRUE */
    if ((MMC_LMM_ACTION_RAU == pstMsg->enActionType)
     || (MMC_LMM_ACTION_ATTACH == pstMsg->enActionType))
    {
        /* 调用函数NAS_MML_GetPsUpdateStatus填充PS update状态 */
        pstMsg->bitOpGprsUpdateStatus = VOS_TRUE;
        pstMsg->ucGprsUpdateStatus    = NAS_MML_GetPsUpdateStatus();

        /* 调用函数NAS_MML_IsRaiChanged填充Rai是否改变标志 */
        pstMsg->bitOpRaiChangFlg      = VOS_TRUE;
        pstMsg->ucRaiChangFlg         = NAS_MML_IsRaiChanged();
    }

    return;
}
VOS_VOID NAS_MMC_FillCsRegInfoInLmmActionRslt(
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg
)
{
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCampInfo     = VOS_NULL_PTR;
    NAS_MML_LAI_STRU                   *pstLastSuccLai  = VOS_NULL_PTR;
    VOS_UINT32                          ulCombReg;

    pstCampInfo    = NAS_MML_GetCurrCampPlmnInfo();
    pstLastSuccLai = NAS_MML_GetCsLastSuccLai();

    ulCombReg      = VOS_FALSE;

    /* 如果不带enReqDomain，直接返回 */
    if (VOS_FALSE == pstMsg->bitOpReqDomain)
    {
        return;
    }

    /* 如果不携带ulAttemptCount，直接返回 */
    if (VOS_FALSE == pstMsg->bitOpAtmpCnt)
    {
        return;
    }

    /* 如果ulAttemptCount为0，直接返回 */
    if (0 == pstMsg->ulAttemptCount)
    {
        return;
    }

    /* 如果是联合RAU或联合ATTACH，ulCombReg为TRUE */
    if ((MMC_LMM_ACTION_RAU == pstMsg->enActionType)
     || (MMC_LMM_ACTION_ATTACH == pstMsg->enActionType))
    {
        if (MMC_LMM_COMBINED_PS_IMSI == pstMsg->ulReqDomain)
        {
            ulCombReg = VOS_TRUE;
        }
    }

    /* 如果是LAU,联合RAU或联合ATTACH,填充CS update状态和Lai是否改变标志 */
    if ((MMC_LMM_ACTION_LAU == pstMsg->enActionType)
     || (VOS_TRUE == ulCombReg))
    {
        /* 调用函数NAS_MML_GetCsUpdateStatus填充CS update状态 */
        pstMsg->bitOpCsUpdateStatus = VOS_TRUE;
        pstMsg->ucCsUpdateStatus    = NAS_MML_GetCsUpdateStatus();

        /* 调用函数NAS_MML_CompareLai填充Lai是否改变标志 */
        pstMsg->bitOpLaiChangFlg    = VOS_TRUE;

        if (VOS_TRUE == NAS_MML_CompareLai(&(pstCampInfo->stLai), pstLastSuccLai))
        {
            pstMsg->ucLaiChangFlg = VOS_FALSE;
        }
        else
        {
            pstMsg->ucLaiChangFlg = VOS_TRUE;
        }
    }

    return;
}
VOS_UINT32 NAS_MMC_PlmnIdIsAllowRegManualMode(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
)
{
    NAS_MML_PLMN_WITH_RAT_STRU         *pstUserSpecPlmn = VOS_NULL_PTR;
    NAS_MML_EQUPLMN_INFO_STRU          *pstEplmnInfo  = VOS_NULL_PTR;
    VOS_UINT32                          ulIsUserPlmn;

    pstEplmnInfo = NAS_MML_GetEquPlmnList();

    pstUserSpecPlmn = NAS_MMC_GetUserSpecPlmnId();

    ulIsUserPlmn = NAS_MML_CompareBcchPlmnwithSimPlmn(pstPlmnId, &(pstUserSpecPlmn->stPlmnId));

    /* 手动搜网模式下,判断该plmn是否为用户指定的PLMN */
    if ((NAS_MMC_PLMN_SELECTION_MODE_MANUAL == NAS_MMC_GetPlmnSelectionMode())
     && (VOS_FALSE == ulIsUserPlmn))
    {
        /* EPLMN无效 */
        if (VOS_FALSE == NAS_MML_GetEplmnValidFlg())
        {
            return VOS_FALSE;
        }

        /* 该PLMN不在EPLMN列表里 */
        if (VOS_FALSE == NAS_MML_IsBcchPlmnIdInDestSimPlmnList(pstPlmnId,
                                                                pstEplmnInfo->ucEquPlmnNum,
                                                                pstEplmnInfo->astEquPlmnAddr))
        {
            return VOS_FALSE;
        }

        /* 用户指定搜网注册状态为 FALSE */
        if (VOS_FALSE == NAS_MMC_GetUserSpecPlmnRegisterStatus())
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}

MMC_LMM_PLMN_CTRL_ENUM_UINT32  Nas_PlmnIdIsForbid(
    MMC_LMM_PLMN_ID_STRU                *pstPlmn
)
{
    NAS_MML_PLMN_ID_STRU                                    stGuPlmnId;
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU                *pstDisabledRatPlmnCfg   = VOS_NULL_PTR;
    NAS_MML_PLMN_WITH_RAT_STRU                              stPlmnWithRat;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbidPlmnInfo = VOS_NULL_PTR;
    VOS_UINT8                                               ucAsAnyCampOn;

    /* 将PLMN ID进行转换 */
    NAS_MMC_ConvertLmmPlmnToGUNasFormat(pstPlmn, &stGuPlmnId);

    /* 先判断该Plmn是否有效 */
    if ( VOS_FALSE == NAS_MML_IsPlmnIdValid(&stGuPlmnId) )
    {
        return MMC_LMM_PLMN_CTRL_BUTT;
    }

    if (NAS_MML_SIM_TYPE_SIM == NAS_MML_GetSimType())
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }


    /* 当前plmn的MCC在禁止LTE国际漫游的MCC列表里,PLMN不允许发起注册 */
    if (VOS_TRUE == NAS_MMC_IsNeedDisableLteRoam(stGuPlmnId.ulMcc))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    ucAsAnyCampOn = NAS_MMC_GetAsAnyCampOn();

    /* 如果是3GPP2控制注册，当前L为限制驻留，不允许发起注册 */
    if ((NAS_MMC_REG_CONTROL_BY_3GPP_MMC != NAS_MMC_GetRegCtrl())
     && (VOS_TRUE == ucAsAnyCampOn))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    if (VOS_FALSE == NAS_MMC_PlmnIdIsAllowRegManualMode(&stGuPlmnId))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    /* 该PLMN ID是否在锁网锁卡的PLMN 列表中 */
    if (VOS_TRUE == NAS_MML_IsBcchPlmnIdInLockPlmnList(&stGuPlmnId))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    /* 不允许漫游时 */
    if (VOS_FALSE == NAS_MML_IsPlmnIdRoamingAllowed(&stGuPlmnId))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    pstDisabledRatPlmnCfg   = NAS_MML_GetDisabledRatPlmnCfg();

    /* 构造当前接入技术的PLMN ID */
    stPlmnWithRat.stPlmnId.ulMcc    = stGuPlmnId.ulMcc;
    stPlmnWithRat.stPlmnId.ulMnc    = stGuPlmnId.ulMnc;
    stPlmnWithRat.enRat             = NAS_MML_NET_RAT_TYPE_LTE;

    /* 判断当前PLMN是否在禁止RAT的PLMN列表中同时当前接入技术被禁止,则认为当前PLMN为禁止网络 */
    if (VOS_TRUE == NAS_MML_IsBcchPlmnIdWithRatInDestPlmnWithRatList(&stPlmnWithRat,
                                                                     pstDisabledRatPlmnCfg->ulDisabledRatPlmnNum,
                                                                     pstDisabledRatPlmnCfg->astDisabledRatPlmnId))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    /* 判断PLMN的RAT在禁止接入技术列表中，则认为当前RAT对应网络为禁止网络 */
    if (VOS_TRUE == NAS_MML_IsRatInForbiddenList(stPlmnWithRat.enRat))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }


    pstForbidPlmnInfo = NAS_MML_GetForbidPlmnInfo();

    /* 在forbid plmn列表中 */
    if (VOS_TRUE == NAS_MML_IsSimPlmnIdInDestBcchPlmnList(&stGuPlmnId,
                                                         pstForbidPlmnInfo->ucForbPlmnNum,
                                                         pstForbidPlmnInfo->astForbPlmnIdList))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    if (VOS_TRUE == NAS_MML_IsSimPlmnIdInDestBcchPlmnList(&stGuPlmnId,
                                                            pstForbidPlmnInfo->ucForbGprsPlmnNum,
                                                            pstForbidPlmnInfo->astForbGprsPlmnList))
    {
        return MMC_LMM_PLMN_NOT_ALLOW_REG;
    }

    return MMC_LMM_PLMN_ALLOW_REG;
}


VOS_VOID NAS_MMC_BuildLmmSpecPlmnSearchReqMsg(
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg,
    NAS_MML_PLMN_LIST_WITH_RAT_STRU    *pstDestPlmnList,
    VOS_UINT32                          ulUserSpecFlag
)
{
    VOS_UINT32                          ulIndex;

    PS_MEM_SET((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId                 = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid                   = PS_PID_MM;
    /* 发送PID需要置上 */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                        = sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                         = ID_MMC_LMM_PLMN_SRCH_REQ;
    pstMsg->ulOpId                          = 0;

    if (VOS_TRUE == ulUserSpecFlag)
    {
        pstMsg->enSrchType                      = MMC_LMM_PLMN_SRCH_USER_SPEC;
    }
    else
    {
        pstMsg->enSrchType                      = MMC_LMM_PLMN_SRCH_SPEC;
    }

    pstMsg->bitOpSpecPlmn                   = VOS_TRUE;
    NAS_MML_ConvertNasPlmnToLMMFormat(&(pstMsg->stSpecPlmnId), &(pstDestPlmnList->astPlmnId[0].stPlmnId));

    if ( VOS_FALSE == ulUserSpecFlag )
    {
        if ( pstDestPlmnList->ulPlmnNum > 1 )
        {
            pstMsg->bitOpEqlPlmn = VOS_TRUE;
        }

        for ( ulIndex = 1; ulIndex < NAS_MML_MIN(pstDestPlmnList->ulPlmnNum, MMC_LMM_MAX_EQUPLMN_NUM); ulIndex++ )
        {
            NAS_MML_ConvertNasPlmnToLMMFormat(&(pstMsg->stEplmnList.astEplmnList[ulIndex-1]), &(pstDestPlmnList->astPlmnId[ulIndex].stPlmnId));
            pstMsg->stEplmnList.ulPlmnNum++;
        }
    }

    return;
}
VOS_VOID NAS_MMC_SndLmmSpecPlmnSearchReq(
    NAS_MML_PLMN_LIST_WITH_RAT_STRU    *pstDestPlmnList,
    VOS_UINT32                          ulUserSpecFlag
)
{
    /* 定义原语类型指针 */
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSpecPlmnSearchReq():WARNING:PS_ALLOC_MSG FIAL");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
                sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    NAS_MMC_BuildLmmSpecPlmnSearchReqMsg(pstMsg, pstDestPlmnList, ulUserSpecFlag);

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if (VOS_OK != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSpecPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if (VOS_TRUE != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSpecPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#endif


#ifndef __PS_WIN32_RECUR__

    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif

    return;
}



VOS_VOID NAS_MMC_SndLmmNcellSpecPlmnSearchReq(
    NAS_MMC_LTE_NCELL_INFO_STRU        *pstLteNcellInfo,
    NAS_MML_EQUPLMN_INFO_STRU          *pstOtherModemEplmnInfo
)
{
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucLoopEplmnNum;
    VOS_UINT8                           ucMaxEplmnNum;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmNcellSpecPlmnSearchReq():WARNING:PS_ALLOC_MSG FIAL");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
                sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_MM;

    /* 发送PID需要置上 */
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMC;
    pstMsg->ulLength         = sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMC_LMM_PLMN_SRCH_REQ;
    pstMsg->ulOpId           = 0;

    pstMsg->enSrchType       = MMC_LMM_PLMN_SRCH_NCELL_SPEC;

    /* 带ncell 频点列表信息 */
    pstMsg->bitOpNcellInfo   = VOS_TRUE;
    pstMsg->stNcellInfo.ucLteArfcnNum = pstLteNcellInfo->ucLteArfcnNum;
    PS_MEM_CPY(pstMsg->stNcellInfo.ausLteArfcnList, pstLteNcellInfo->ausLteArfcnList,
               sizeof(pstMsg->stNcellInfo.ausLteArfcnList));

    pstMsg->stNcellInfo.ucLteCellIdNum = pstLteNcellInfo->ucLteCellNum;
    PS_MEM_CPY(pstMsg->stNcellInfo.ausLteCellIdList, pstLteNcellInfo->ausLteCellList,
               sizeof(pstMsg->stNcellInfo.ausLteArfcnList));

    ucMaxEplmnNum        = pstOtherModemEplmnInfo->ucEquPlmnNum;

    if (ucMaxEplmnNum > MMC_LMM_MAX_EQUPLMN_NUM)
    {
        ucMaxEplmnNum = MMC_LMM_MAX_EQUPLMN_NUM;
    }

    pstMsg->stNcellInfo.stEplmnList.ulPlmnNum = ucMaxEplmnNum;

    for (ucLoopEplmnNum = 0; ucLoopEplmnNum < ucMaxEplmnNum; ucLoopEplmnNum++ )
    {
        NAS_MML_ConvertNasPlmnToLMMFormat(&(pstMsg->stNcellInfo.stEplmnList.astEplmnList[ucLoopEplmnNum]),
                                          &(pstOtherModemEplmnInfo->astEquPlmnAddr[ucLoopEplmnNum]));
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if (VOS_OK != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmNcellSpecPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if (VOS_TRUE != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSpecPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#endif

#ifndef __PS_WIN32_RECUR__
    NAS_MML_SetUePcRecurFlg(VOS_TRUE);
#endif

    return;
}



VOS_VOID NAS_MMC_BuildMmcLmmDetachIndActionResultReqMsg(
    NAS_MMC_GMM_NT_DETACH_ENUM_U32      enMtType,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCnCause,
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pstMsg
)
{
    NAS_LMM_CN_CAUSE_ENUM_UINT8         enCnRejCause;

    PS_MEM_SET((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,0,
                         sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_ACTION_RESULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_ACTION_RESULT_REQ;
    pstMsg->ulOpId                      = 0;

    /* 根据DETACH 类型设置发起业务类型和结果域类型 */
    switch(enMtType)
    {
        case NAS_MMC_GMM_NT_DETACH_IMSI:
            pstMsg->bitOpReqDomain = VOS_TRUE;
            pstMsg->bitOpRstDomain = VOS_TRUE;
            pstMsg->ulReqDomain    = MMC_LMM_IMSI_ONLY;
            pstMsg->ulRstDomain    = MMC_LMM_IMSI_ONLY;
            pstMsg->ulMtDetachType = MMC_LMM_MT_DET_IMSI;
            break;

        case NAS_MMC_GMM_NT_DETACH_REATTACH:
            pstMsg->bitOpReqDomain = VOS_TRUE;
            pstMsg->bitOpRstDomain = VOS_TRUE;
            pstMsg->ulReqDomain    = MMC_LMM_PS_ONLY;
            pstMsg->ulRstDomain    = MMC_LMM_PS_ONLY;
            pstMsg->ulMtDetachType = MMC_LMM_MT_DET_REATTACH_REQUIRED;
            break;

        case NAS_MMC_GMM_NT_DETACH_NOATTACH:
            pstMsg->bitOpReqDomain = VOS_TRUE;
            pstMsg->bitOpRstDomain = VOS_TRUE;
            pstMsg->ulReqDomain    = MMC_LMM_PS_ONLY;
            pstMsg->ulRstDomain    = MMC_LMM_PS_ONLY;
            pstMsg->ulMtDetachType = MMC_LMM_MT_DET_REATTACH_NOT_REQUIRED;
            break;

        default:
            break;
    }

    pstMsg->bitOpMtDetachType           = VOS_TRUE;
    pstMsg->enActionType                = MMC_LMM_ACTION_MT_DETACH;
    pstMsg->ulActRst                    = MMC_LMM_RSLT_TYPE_SUCCESS;

    /* TS 24008,4.7.4.2.2.If the detach type IE indicates "IMSI detach", or "re-attach required" then
       the MS shall ignore the cause code if received. */
    if ( (VOS_OK == NAS_MMC_ConvertMmCauseToEmmCause(enCnCause, &enCnRejCause))
     && (MMC_LMM_MT_DET_REATTACH_NOT_REQUIRED == pstMsg->ulMtDetachType))
    {
        pstMsg->bitOpCnCause            = VOS_TRUE;
        pstMsg->ucCnCause               = enCnRejCause;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MMC_ConverCnDomainToLmm
 功能描述  : 将MMC内部保存的Domain转换为LMM格式的
 输入参数  : enCnDoamin:MMC内部保存的域
 输出参数  : penLmmDomain:LMM格式的域
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月12日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_MMC_ConverCnDomainToLmm(
    NAS_MMC_REG_DOMAIN_ENUM_UINT8      enCnDoamin,
    MMC_LMM_DOMAIN_ENUM_UINT32         *penLmmDomain
)
{
    if ( NAS_MMC_REG_DOMAIN_CS           == enCnDoamin)
    {
        *penLmmDomain = MMC_LMM_IMSI_ONLY;
    }
    else if ( NAS_MMC_REG_DOMAIN_PS_CS   == enCnDoamin)
    {
        *penLmmDomain = MMC_LMM_COMBINED_PS_IMSI;
    }
    else
    {
        *penLmmDomain = MMC_LMM_PS_ONLY;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : NAS_MMC_ConvertMmCauseToEmmCause
 功能描述  : 将MM的注册结果转换为EMM的注册结果,由于需要明确告诉EMM是否是网络
              拒绝还是其他原因拒绝
 输入参数  : enCnCause:MM注册结果
 输出参数  : penProtolCause:协议中规定的结果
 返 回 值  : VOS_OK:转换成功
              VOS_ERR:转换失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月11日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  NAS_MMC_ConvertMmCauseToEmmCause(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enCnCause,
    NAS_LMM_CN_CAUSE_ENUM_UINT8        *penProtolCause
)
{
    /* 该原因值表明注册成功或网络无响应 */
    if ( NAS_MML_REG_FAIL_CAUSE_NULL == enCnCause )
    {
        return VOS_ERR;
    }

    if ( enCnCause >= NAS_MML_REG_FAIL_CAUSE_AUTH_REJ )
    {
        return VOS_ERR;
    }

    *penProtolCause = (VOS_UINT8)enCnCause;

    return VOS_OK;
}
VOS_UINT32  NAS_MMC_ConvertGuRsltToLmmFormat(
    NAS_MML_REG_RESULT_ENUM_UINT8       enGuRegRslt,
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  enGuCnCause,
    MMC_LMM_RSLT_TYPE_ENUM_UINT32      *penLmmActRslt
)
{
    MMC_LMM_RSLT_TYPE_ENUM_UINT32      enRslt;

    /* 获取注册结果 */
    if ( NAS_MML_REG_RESULT_SUCCESS == enGuRegRslt )
    {
        enRslt = MMC_LMM_RSLT_TYPE_SUCCESS;
    }
    else
    {
        /* 调整判断语句顺序，先判断enGuCnCause小于NAS_MML_REG_FAIL_CAUSE_AUTH_REJ
           再判断enGuCnCause等于NAS_MML_REG_FAIL_CAUSE_AUTH_REJ
           增加判断enGuCnCause等于NAS_MML_REG_FAIL_CAUSE_ACCESS_BARRED */
        if (enGuCnCause < NAS_MML_REG_FAIL_CAUSE_AUTH_REJ)
        {
            enRslt = MMC_LMM_RSLT_TYPE_CN_REJ;
        }
        else if (NAS_MML_REG_FAIL_CAUSE_AUTH_REJ == enGuCnCause)
        {
            enRslt = MMC_LMM_RSLT_TYPE_AUTH_REJ;
        }
        else if (NAS_MML_REG_FAIL_CAUSE_ACCESS_BARRED == enGuCnCause)
        {
            enRslt = MMC_LMM_RSLT_TYPE_ACCESS_BARRED;
        }
        else
        {
            enRslt = MMC_LMM_RSLT_TYPE_FAILURE;
        }
    }

    *penLmmActRslt = enRslt;

    return VOS_OK;
}



VOS_VOID NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify( VOS_VOID )
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_USER_PLMN_END_NOTIFY_STRU   *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_USER_PLMN_END_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_USER_PLMN_END_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_USER_PLMN_END_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_USER_PLMN_END_NOTIFY_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify():ERROR:Memory Alloc Error for pstMsg");
        return;
    }

    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    /* 发送PID需要置上 */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_MMC;
    pstMsg->ulLength                    = sizeof(MMC_LMM_USER_PLMN_END_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId                     = ID_MMC_LMM_USER_PLMN_END_NOTIFY;
    pstMsg->ulOpId                      = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUserSpecPlmnSearchEndNotify():WARNING:SEND LMM PLMN ANYCELL SEARCH REQ FIAL");
        return;
    }
#endif
}


VOS_VOID NAS_MMC_SndLmmBgStopPlmnSearchReq(VOS_VOID)
{
    /* 定义原语类型指针 */
    MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU                   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgStopPlmnSearchReq():WARNING:PS_ALLOC_MSG FIAL");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
                sizeof(MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_MM;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMC;
    pstMsg->ulLength         = sizeof(MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMC_LMM_STOP_BG_PLMN_SEARCH_REQ;
    pstMsg->ulOpId           = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if (VOS_OK != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgStopPlmnSearchReq():WARNING:SEND LMM BG STOP PLMN SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if (VOS_TRUE != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgStopPlmnSearchReq():WARNING:SEND LMM BG STOP PLMN SEARCH REQ FIAL");
        return;
    }
#endif

    return;
}
VOS_VOID NAS_MMC_SndLmmFastPlmnSearchReq(
    NAS_MML_PLMN_ID_STRU               *pstDestPlmn
)
{
    /* 定义原语类型指针 */
    MMC_LMM_PLMN_SRCH_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_PLMN_SRCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmFastPlmnSearchReq():WARNING:PS_ALLOC_MSG FIAL");
        return;
    }

    PS_MEM_SET( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
                sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_MM;
    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMC;
    pstMsg->ulLength         = sizeof(MMC_LMM_PLMN_SRCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMC_LMM_PLMN_SRCH_REQ;
    pstMsg->ulOpId           = 0;

    /* 快速指定的PLMN的搜索 */
    pstMsg->enSrchType       = MMC_LMM_PLMN_SRCH_FAST_SPEC;

    pstMsg->bitOpSpecPlmn    = VOS_TRUE;
    NAS_MML_ConvertNasPlmnToLMMFormat(&(pstMsg->stSpecPlmnId), pstDestPlmn);

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if (VOS_OK != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmFastPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if (VOS_TRUE != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmFastPlmnSearchReq():WARNING:SEND LMM PLMN SEARCH REQ FIAL");
        return;
    }
#endif

    return;
}


VOS_VOID NAS_MMC_SndLmmBgPlmnSearchReq(
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDestPlmn
)
{
    /* 定义原语类型指针 */
    MMC_LMM_BG_PLMN_SEARCH_REQ_STRU                        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU                       *pstForbPlmnInfo;
    VOS_UINT32                                              i;
    MMC_LMM_PLMN_ID_STRU                                    stLmmPlmn;

    /* 从全局变量中获取FORBIDEN ROAMING LAS信息 */
    pstForbPlmnInfo = NAS_MML_GetForbidPlmnInfo();


#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_BG_PLMN_SEARCH_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_BG_PLMN_SEARCH_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_BG_PLMN_SEARCH_REQ_STRU));
#endif

    if ( VOS_NULL_PTR == pstMsg )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgPlmnSearchReq():WARNING:PS_ALLOC_MSG FIAL");
        return;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0X00,
                sizeof(MMC_LMM_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 构造发送消息 */
    pstMsg->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid     = PS_PID_MM;
    pstMsg->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid       = WUEPS_PID_MMC;
    pstMsg->ulLength          = sizeof(MMC_LMM_BG_PLMN_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId           = ID_MMC_LMM_BG_PLMN_SEARCH_REQ;
    pstMsg->ulOpId            = 0;

    NAS_MML_ConvertNasPlmnToLMMFormat(&stLmmPlmn, &pstDestPlmn->stPlmnId);
    PS_MEM_CPY(pstMsg->stSpecPlmnWithRat.aucPlmnId, stLmmPlmn.aucPlmnId, MMC_LMM_PLMN_ID_LEN);
    pstMsg->stSpecPlmnWithRat.enPlmnRat = pstDestPlmn->enRat;

    if (NAS_MML_NET_RAT_TYPE_LTE != pstDestPlmn->enRat)
    {
        /* L下背景搜GU的网络，需要将所有禁止漫游 LA 添加到ForbLa列表 */
        for ( i = 0; i < pstForbPlmnInfo->ucForbRoamLaNum; i++ )
        {
            NAS_MML_ConvertNasPlmnToLMMFormat(&(pstMsg->astForbRoamLaiList[i].stPlmnId),
                                              &(pstForbPlmnInfo->astForbRomLaList[i].stPlmnId));
            pstMsg->astForbRoamLaiList[i].stLac.ucLac   =
                pstForbPlmnInfo->astForbRomLaList[i].aucLac[0];
            pstMsg->astForbRoamLaiList[i].stLac.ucLacCnt =
                pstForbPlmnInfo->astForbRomLaList[i].aucLac[1];
        }

        /* ForbLa列表中的禁止漫游 LA 的个数 */
        pstMsg->ulForbRoamLaiNum = pstForbPlmnInfo->ucForbRoamLaNum;
    }


#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if (VOS_OK != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgPlmnSearchReq():WARNING:SEND LMM BG PLMN SEARCH REQ FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if (VOS_TRUE != ulRet)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmBgPlmnSearchReq():WARNING:SEND LMM BG PLMN SEARCH REQ FIAL");
        return;
    }
#endif

    return;
}







VOS_VOID  NAS_MMC_SndLmmUeOocStatusInd( VOS_VOID )
{
    /* 定义调用VOS发送函数的返回值 */
    VOS_UINT32                          ulRet;

    /* 定义原语类型指针 */
    MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU  *pstMsg = VOS_NULL_PTR;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstMsg = (MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMsg = (MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU));
#endif

    if( VOS_NULL_PTR == pstMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUeOocStatusInd():ERROR:Memory Alloc Error for pstMsg");
        return;
    }

    pstMsg->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid      = WUEPS_PID_MMC;
    pstMsg->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid    = PS_PID_MM;
    pstMsg->ulLength         = sizeof(MMC_LMM_UE_OOC_STATUS_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId          = ID_MMC_LMM_UE_OOC_STATUS_NOTIFY;
    pstMsg->ulOpId           = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUeOocStatusInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMsg);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmUeOocStatusInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#endif
}
VOS_VOID NAS_MMC_SndLmmWcdmaSysInfoInd(
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_INFO_IND_STRU             *pstRrMmSysInfoInd  = VOS_NULL_PTR;
    MMC_LMM_WCDMA_SYS_INFO_IND_STRU    *pstLmmWcdmaSysInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                stGUNasPlmnId;
    VOS_UINT32                          ulRet;

    pstRrMmSysInfoInd = (RRMM_SYS_INFO_IND_STRU*)pstMsg;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstLmmWcdmaSysInfo = (MMC_LMM_WCDMA_SYS_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_WCDMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstLmmWcdmaSysInfo = (MMC_LMM_WCDMA_SYS_INFO_IND_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_WCDMA_SYS_INFO_IND_STRU));
#endif

    if( VOS_NULL_PTR == pstLmmWcdmaSysInfo )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmWcdmaSysInfoInd():ERROR:Memory Alloc Error for pstMsg");
        return;
    }

    pstLmmWcdmaSysInfo->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstLmmWcdmaSysInfo->ulSenderPid      = WUEPS_PID_MMC;
    pstLmmWcdmaSysInfo->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstLmmWcdmaSysInfo->ulReceiverPid    = PS_PID_MM;
    pstLmmWcdmaSysInfo->ulLength         = sizeof(MMC_LMM_WCDMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstLmmWcdmaSysInfo->ulMsgId          = ID_MMC_LMM_WCDMA_SYS_INFO_IND;

    /* 填充PLMN 字段 */
    stGUNasPlmnId.ulMcc = pstRrMmSysInfoInd->PlmnId.ulMcc;;
    stGUNasPlmnId.ulMnc = pstRrMmSysInfoInd->PlmnId.ulMnc;

    NAS_MML_ConvertNasPlmnToLMMFormat(&(pstLmmWcdmaSysInfo->stPlmnId),
                                      &(stGUNasPlmnId));

    /* 填充小区ID */
    pstLmmWcdmaSysInfo->ulCellId = pstRrMmSysInfoInd->ulCellId;

    /* 填充LAC */
    /* aucCnCommonInfo[0] 和 aucCnCommonInfo[1] 代表 lac[0] 和  Lac[1]*/
    pstLmmWcdmaSysInfo->usLac  = (VOS_UINT16)pstRrMmSysInfoInd->aucCnCommonInfo[1];
    pstLmmWcdmaSysInfo->usLac |= (VOS_UINT16)(pstRrMmSysInfoInd->aucCnCommonInfo[0]<< NAS_MML_OCTET_MOVE_EIGHT_BITS);


    /* 填充RAC */
    if ( 0 == pstRrMmSysInfoInd->ucPsInfoSize )
    {
        pstLmmWcdmaSysInfo->bitOpRac = VOS_FALSE;
        pstLmmWcdmaSysInfo->ucRac    = 0;
    }
    else
    {
        pstLmmWcdmaSysInfo->bitOpRac = VOS_TRUE;
        pstLmmWcdmaSysInfo->ucRac    = pstRrMmSysInfoInd->aucPsInfo[0];
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstLmmWcdmaSysInfo );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmWcdmaSysInfoInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstLmmWcdmaSysInfo);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmWcdmaSysInfoInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#endif
}


VOS_VOID NAS_MMC_SndLmmGsmSysInfoInd(
    struct MsgCB                       *pstMsg
)
{
    GRRMM_SYS_INFO_IND_ST              *pstGrrMmSysInfo  = VOS_NULL_PTR;
    MMC_LMM_GSM_SYS_INFO_IND_STRU      *pstLmmGsmSysInfo = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU                stGUNasPlmnId;
    VOS_UINT32                          ulRet;

    pstGrrMmSysInfo = (GRRMM_SYS_INFO_IND_ST*)pstMsg;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 申请内存  */
    pstLmmGsmSysInfo = (MMC_LMM_GSM_SYS_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_LMM_GSM_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstLmmGsmSysInfo = (MMC_LMM_GSM_SYS_INFO_IND_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_GSM_SYS_INFO_IND_STRU));
#endif

    if( VOS_NULL_PTR == pstLmmGsmSysInfo )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmGsmSysInfoInd():ERROR:Memory Alloc Error for pstMsg");
        return;
    }

    pstLmmGsmSysInfo->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstLmmGsmSysInfo->ulSenderPid      = WUEPS_PID_MMC;
    pstLmmGsmSysInfo->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstLmmGsmSysInfo->ulReceiverPid    = PS_PID_MM;
    pstLmmGsmSysInfo->ulLength         = sizeof(MMC_LMM_GSM_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstLmmGsmSysInfo->ulMsgId          = ID_MMC_LMM_GSM_SYS_INFO_IND;

    /* 填充PLMN 字段 */
    stGUNasPlmnId.ulMcc = pstGrrMmSysInfo->PlmnId.ulMcc;;
    stGUNasPlmnId.ulMnc = pstGrrMmSysInfo->PlmnId.ulMnc;

    NAS_MML_ConvertNasPlmnToLMMFormat(&(pstLmmGsmSysInfo->stPlmnId),
                                      &(stGUNasPlmnId));

    /* 填充小区ID */
    pstLmmGsmSysInfo->ulCellId = (VOS_UINT32)pstGrrMmSysInfo->usCellId;

    /* 填充LAC */
    pstLmmGsmSysInfo->usLac    = pstGrrMmSysInfo->usLac;

    /* 填充RAC */
    if ( 0 == pstGrrMmSysInfo->ucGprsSupportInd )
    {
        pstLmmGsmSysInfo->bitOpRac = VOS_FALSE;
        pstLmmGsmSysInfo->ucRac    = 0;
    }
    else
    {
        pstLmmGsmSysInfo->bitOpRac = VOS_TRUE;
        pstLmmGsmSysInfo->ucRac    = pstGrrMmSysInfo->ucRac;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstLmmGsmSysInfo );
    if ( VOS_OK != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmGsmSysInfoInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstLmmGsmSysInfo);
    if ( VOS_TRUE != ulRet )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmGsmSysInfoInd():WARNING:SEND LMM MSG FIAL");
        return;
    }
#endif

}
VOS_VOID NAS_MMC_SndLmmCellSelReq(
    MMC_LMM_CELL_SEL_TYPE_ENUM_UINT32       ulSelType
)
{
    MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU                   *pstLmmCellSelStru = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstLmmCellSelStru = (MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstLmmCellSelStru = (MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU));
#endif

    /* 若分配内存出错,返回*/
    if ( VOS_NULL_PTR == pstLmmCellSelStru )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmEquPlmnReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstLmmCellSelStru + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstLmmCellSelStru->ulReceiverPid   = PS_PID_MM;
    pstLmmCellSelStru->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstLmmCellSelStru->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstLmmCellSelStru->ulSenderPid     = WUEPS_PID_MMC;
    pstLmmCellSelStru->ulMsgId         = ID_MMC_LMM_CELL_SELECTION_CTRL_REQ;
    pstLmmCellSelStru->ulLength        = sizeof(MMC_LMM_CELL_SELECTION_CTRL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstLmmCellSelStru->ulOpId          = 0;
    pstLmmCellSelStru->ulSelecType     = ulSelType;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstLmmCellSelStru );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmCellSelReq():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstLmmCellSelStru);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmCellSelReq():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#endif
}
VOS_VOID NAS_MMC_SndLmmSuspendRelReq(VOS_VOID)
{
    MMC_LMM_SUSPEND_REL_REQ_STRU                   *pstMmcLmmSuspendRelReq = VOS_NULL_PTR;
    VOS_UINT32                                      ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstMmcLmmSuspendRelReq = (MMC_LMM_SUSPEND_REL_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_SUSPEND_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMmcLmmSuspendRelReq = (MMC_LMM_SUSPEND_REL_REQ_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_SUSPEND_REL_REQ_STRU));
#endif

    /* 若分配内存出错,返回*/
    if ( VOS_NULL_PTR == pstMmcLmmSuspendRelReq )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRelReq():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMmcLmmSuspendRelReq + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_SUSPEND_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH );

    pstMmcLmmSuspendRelReq->ulReceiverPid   = PS_PID_MM;
    pstMmcLmmSuspendRelReq->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMmcLmmSuspendRelReq->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMmcLmmSuspendRelReq->ulSenderPid     = WUEPS_PID_MMC;
    pstMmcLmmSuspendRelReq->ulMsgId         = ID_MMC_LMM_SUSPEND_REL_REQ;
    pstMmcLmmSuspendRelReq->ulLength        = sizeof(MMC_LMM_SUSPEND_REL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmcLmmSuspendRelReq->ulOpId          = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMmcLmmSuspendRelReq );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRelReq():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMmcLmmSuspendRelReq);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmSuspendRelReq():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#endif
}

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_VOID NAS_MMC_SndLmmOtherModemInfoNotify(
    MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU                   *pstOtherModemInfoMsg
)
{
    MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU                   *pstMmcLmmOtherModemInfo = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstMmcLmmOtherModemInfo = (MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                            sizeof(MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMmcLmmOtherModemInfo = (MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU));
#endif

    /* 若分配内存出错,返回*/
    if ( VOS_NULL_PTR == pstMmcLmmOtherModemInfo )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmOtherModemInfoNotify():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMmcLmmOtherModemInfo + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 消息IE项赋值 */
    pstMmcLmmOtherModemInfo->ulReceiverPid       = PS_PID_MM;
    pstMmcLmmOtherModemInfo->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMmcLmmOtherModemInfo->ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstMmcLmmOtherModemInfo->ulSenderPid         = WUEPS_PID_MMC;
    pstMmcLmmOtherModemInfo->ulMsgId             = ID_MMC_LMM_OTHER_MODEM_INFO_NOTIFY;
    pstMmcLmmOtherModemInfo->ulLength            = sizeof(MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmcLmmOtherModemInfo->bitOpCurrCampPlmnId = pstOtherModemInfoMsg->bitOpCurrCampPlmnId;

    PS_MEM_CPY(&pstMmcLmmOtherModemInfo->stCurrCampPlmnId, &pstOtherModemInfoMsg->stCurrCampPlmnId, sizeof(pstMmcLmmOtherModemInfo->stCurrCampPlmnId));

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMmcLmmOtherModemInfo );

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmOtherModemInfoNotify():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMmcLmmOtherModemInfo);

    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmOtherModemInfoNotify():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#endif
}
#endif



VOS_VOID NAS_MMC_SndLmmLteSysInfoInd(
    LMM_MMC_SYS_INFO_IND_STRU          *pstLmmSysInfoMsg
)
{
    MMC_LMM_LTE_SYS_INFO_IND_STRU      *pstMmcLmmSysInfoInd = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstMmcLmmSysInfoInd = (MMC_LMM_LTE_SYS_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_LTE_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMmcLmmSysInfoInd = (MMC_LMM_LTE_SYS_INFO_IND_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_LTE_SYS_INFO_IND_STRU));
#endif

    /* 若分配内存出错,返回*/
    if ( VOS_NULL_PTR == pstMmcLmmSysInfoInd )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLteSysInfoInd():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMmcLmmSysInfoInd + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_LTE_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 消息IE项赋值 */
    PS_MEM_CPY(pstMmcLmmSysInfoInd, pstLmmSysInfoMsg, sizeof(MMC_LMM_LTE_SYS_INFO_IND_STRU));

    pstMmcLmmSysInfoInd->ulReceiverPid   = PS_PID_MM;
    pstMmcLmmSysInfoInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMmcLmmSysInfoInd->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMmcLmmSysInfoInd->ulSenderPid     = WUEPS_PID_MMC;
    pstMmcLmmSysInfoInd->ulMsgId         = ID_MMC_LMM_LTE_SYS_INFO_IND;
    pstMmcLmmSysInfoInd->ulLength        = sizeof(MMC_LMM_LTE_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmcLmmSysInfoInd->ulOpId          = 0;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMmcLmmSysInfoInd );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLteSysInfoInd():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMmcLmmSysInfoInd);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmLteSysInfoInd():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#endif
}


VOS_VOID NAS_MMC_SndLmmImsVoiceCapChangeNtf(
    VOS_UINT8       ucImsVoiceAvail
)
{
    MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU               *pstMmcLmmImsVoiceChgNtf = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    pstMmcLmmImsVoiceChgNtf = (MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                               sizeof(MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH);
#else
    pstMmcLmmImsVoiceChgNtf = (MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU *)NAS_MML_GetIntMsgSendBuf(sizeof(MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU));
#endif

    /* 若分配内存出错,返回*/
    if ( VOS_NULL_PTR == pstMmcLmmImsVoiceChgNtf )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmImsVoiceCapChangeNtf():ERROR:Memory Alloc Error for pstMsg");

        return;
    }

    PS_MEM_SET( (VOS_INT8*)pstMmcLmmImsVoiceChgNtf + VOS_MSG_HEAD_LENGTH, 0X0,
               sizeof(MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 消息IE项赋值 */
    pstMmcLmmImsVoiceChgNtf->ulReceiverPid   = PS_PID_MM;
    pstMmcLmmImsVoiceChgNtf->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMmcLmmImsVoiceChgNtf->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMmcLmmImsVoiceChgNtf->ulSenderPid     = WUEPS_PID_MMC;
    pstMmcLmmImsVoiceChgNtf->ulMsgId         = ID_MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY;
    pstMmcLmmImsVoiceChgNtf->ulLength        = sizeof(MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMmcLmmImsVoiceChgNtf->ulOpId          = 0;

    pstMmcLmmImsVoiceChgNtf->enImsVoiceCap   = MMC_LMM_IMS_VOICE_CAP_UNAVAILABLE;

    if (VOS_TRUE == ucImsVoiceAvail)
    {
        pstMmcLmmImsVoiceChgNtf->enImsVoiceCap   = MMC_LMM_IMS_VOICE_CAP_AVAILABLE;
    }

#if (defined (NAS_STUB) || defined(__PS_WIN32_RECUR__))
    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMmcLmmImsVoiceChgNtf );
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmImsVoiceCapChangeNtf():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#else
    /* 内部消息的发送 */
    ulRet = NAS_MML_SndInternalMsg(pstMmcLmmImsVoiceChgNtf);
    if ( VOS_TRUE != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndLmmImsVoiceCapChangeNtf():ERROR:SEND MESSAGE FAILURE");

        return;
    }
#endif
}



#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

