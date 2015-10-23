

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "TafMmcInterface.h"
#include "MmaMmcInterface.h"
/* 删除ExtAppMmcInterface.h*/
#include "NasMmSublayerDef.h"
#include "NasMmlCtx.h"
#include "NasMmcCtx.h"
#include "NasMmlLib.h"
#include "NasMmcSndMm.h"
#include "NasComm.h"
#include "NasMmcSndMma.h"
#include "MmcMmaItf.h"
#include "MnComm.h"
#include "NasMmcFsmMainTbl.h"
#include "NasMmcComFunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_MMC_SND_MMA_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_VOID NAS_MMC_SndMmaUserSpecPlmnSearchCnf(
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8              enResult
)
{
    MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU  *pstSpecPlmnSrchCnf = VOS_NULL_PTR;

    pstSpecPlmnSrchCnf = (MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                             sizeof(MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstSpecPlmnSrchCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaUserSpecPlmnSearchCnf():ERROR:Memory Alloc Error for pstSpecPlmnSrchCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSpecPlmnSrchCnf)+VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU)- VOS_MSG_HEAD_LENGTH);
    pstSpecPlmnSrchCnf->enResult                    = enResult;

    /* 填充消息头 */
    pstSpecPlmnSrchCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_SPECIAL_SEL_CNF;
    pstSpecPlmnSrchCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSpecPlmnSrchCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSpecPlmnSrchCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSpecPlmnSrchCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstSpecPlmnSrchCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_SPECIAL_SEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstSpecPlmnSrchCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaUserSpecPlmnSearchCnf()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaPlmnReselCnf(
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8              enPlmnReselRslt
)
{
    MMC_MMA_PLMN_RESEL_CNF_STRU        *pstReselPlmnSrchCnf = VOS_NULL_PTR;

    pstReselPlmnSrchCnf = (MMC_MMA_PLMN_RESEL_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                              sizeof(MMC_MMA_PLMN_RESEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstReselPlmnSrchCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaPlmnReselCnf():ERROR:Memory Alloc Error for pstReselPlmnSrchCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstReselPlmnSrchCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_RESEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    pstReselPlmnSrchCnf->enResult                    = enPlmnReselRslt;

    pstReselPlmnSrchCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_RESEL_CNF;
    pstReselPlmnSrchCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstReselPlmnSrchCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstReselPlmnSrchCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstReselPlmnSrchCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstReselPlmnSrchCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_RESEL_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstReselPlmnSrchCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnReselCnf()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaPlmnSelectStartInd(VOS_VOID)
{
    MMC_MMA_PLMN_SELECTION_START_IND_STRU  *pstPlmnSeleStartInd = VOS_NULL_PTR;

    pstPlmnSeleStartInd = (MMC_MMA_PLMN_SELECTION_START_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                           sizeof(MMC_MMA_PLMN_SELECTION_START_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstPlmnSeleStartInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaPlmnSelectStartInd():ERROR:Memory Alloc Error for pstPlmnSelRlstInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPlmnSeleStartInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_SELECTION_RLST_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstPlmnSeleStartInd->enPlmnSeleStartFlag         = MMA_MMC_PLMN_SELECTION_START;

    /* 填充消息头 */
    pstPlmnSeleStartInd->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_SELE_START_IND;
    pstPlmnSeleStartInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPlmnSeleStartInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPlmnSeleStartInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPlmnSeleStartInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPlmnSeleStartInd->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_SELECTION_START_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnSeleStartInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnSelectStartInd()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaPlmnSelectionRsltInd(
    MMA_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32               enResult
)
{
    MMC_MMA_PLMN_SELECTION_RLST_IND_STRU                   *pstPlmnSelRlstInd = VOS_NULL_PTR;

    pstPlmnSelRlstInd = (MMC_MMA_PLMN_SELECTION_RLST_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_PLMN_SELECTION_RLST_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstPlmnSelRlstInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaPlmnSelectionRsltInd():ERROR:Memory Alloc Error for pstPlmnSelRlstInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPlmnSelRlstInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_SELECTION_RLST_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstPlmnSelRlstInd->enResult                    = enResult;

    /* 填充消息头 */
    pstPlmnSelRlstInd->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_SELECTION_RLST_IND;
    pstPlmnSelRlstInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPlmnSelRlstInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPlmnSelRlstInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPlmnSelRlstInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPlmnSelRlstInd->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_SELECTION_RLST_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnSelRlstInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnSelectionRsltInd()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaDetachInd(
    VOS_UINT32                          ulCnDomainId,
    VOS_UINT32                          ulDetachType,
    VOS_UINT32                          ulCause
)
{
    MMC_MMA_DETACH_IND_STRU            *pstDetachInd   = VOS_NULL_PTR;
    NAS_MMC_SERVICE_INFO_CTX_STRU      *pstServiceInfo = VOS_NULL_PTR;

    pstDetachInd = (MMC_MMA_DETACH_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_DETACH_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstDetachInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachInd():ERROR:Memory Alloc Error for pstDetachInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstDetachInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_DETACH_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 获取服务状态 */
    pstServiceInfo = NAS_MMC_GetServiceInfo();

    pstDetachInd->enCnDomainId                = ulCnDomainId;
    if (MMA_MMC_SRVDOMAIN_CS == ulCnDomainId)
    {
        /* CS域信息有效, 填写"CS服务状态" */
        pstDetachInd->enServiceStatus = pstServiceInfo->enCsCurrService;
    }
    else if (MMA_MMC_SRVDOMAIN_PS == ulCnDomainId)
    {
        /* PS域信息有效, 填写"PS服务状态" */
        pstDetachInd->enServiceStatus = pstServiceInfo->enPsCurrService;
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachInd():ERROR: ulCnDomainId Error ");

        /* 释放内存 */
        PS_MEM_FREE(WUEPS_PID_MMC, pstDetachInd);

        return;
    }

    /* 填充消息头 */
    pstDetachInd->MsgHeader.ulMsgName         = ID_MMC_MMA_DETACH_IND;
    pstDetachInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstDetachInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstDetachInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstDetachInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstDetachInd->MsgHeader.ulLength          = sizeof(MMC_MMA_DETACH_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstDetachInd->ulDetachCause               = ulCause;
    pstDetachInd->enNetworkDetachType         = (VOS_UINT8)ulDetachType;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstDetachInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaDetachInd()");
    }

    return;
}




VOS_VOID NAS_MMC_SndMmaAttachCnf(
     VOS_UINT32                                             ulCnDomainId,
     NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8                   enMmaServiceStatus,
     VOS_UINT32                                             ulOpid
)
{
    MMC_MMA_ATTACH_CNF_STRU             *pstAttachCnf = VOS_NULL_PTR;

    pstAttachCnf = (MMC_MMA_ATTACH_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_ATTACH_CNF_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstAttachCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaAttachCnf():ERROR:Memory Alloc Error for pstAttachCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstAttachCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_ATTACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstAttachCnf->enServiceStatus             = enMmaServiceStatus;
    pstAttachCnf->enCnDomainId                = ulCnDomainId;

    /* 填充消息头 */
    pstAttachCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_ATTACH_CNF;
    pstAttachCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAttachCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAttachCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAttachCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstAttachCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_ATTACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstAttachCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaAttachCnf()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaDetachCnf(
    VOS_UINT32                                              ulCnDomainId,
    NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8                    enMmaServiceStatus,
    VOS_UINT32                                              ulOpid
)
{
    MMC_MMA_DETACH_CNF_STRU            *pstDetachCnf = VOS_NULL_PTR;

    pstDetachCnf = (MMC_MMA_DETACH_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_DETACH_CNF_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstDetachCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachCnf():ERROR:Memory Alloc Error for pstDetachCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstDetachCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_DETACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstDetachCnf->enServiceStatus             = enMmaServiceStatus;
    pstDetachCnf->enCnDomainId                = ulCnDomainId;

    pstDetachCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_DETACH_CNF;
    pstDetachCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstDetachCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstDetachCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstDetachCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstDetachCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_DETACH_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstDetachCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaDetachCnf()");
    }

    return;
}



VOS_VOID NAS_MMC_SndMmaServiceStatusInd(
    VOS_UINT32                          ulCnDomainId,
    VOS_UINT32                          ulServiceStatus
)
{
    /* 按MMC_MMA_SERVICE_STATUS_IND_STRU消息结果封装 */
    MMC_MMA_SERVICE_STATUS_IND_STRU    *pstServiceStatusInd = VOS_NULL_PTR;

    pstServiceStatusInd = (MMC_MMA_SERVICE_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                              sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstServiceStatusInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaServiceStatusInd():ERROR:Memory Alloc Error for pstServiceStatusInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstServiceStatusInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstServiceStatusInd->bitOpSrvSta                 = VOS_TRUE;
    pstServiceStatusInd->bitOpRegSta                 = VOS_FALSE;
    pstServiceStatusInd->enServiceStatus             = ulServiceStatus;
    pstServiceStatusInd->enCnDomainId                = ulCnDomainId;
    pstServiceStatusInd->ucSimCsRegStatus            = NAS_MML_GetSimCsRegStatus();
    pstServiceStatusInd->ucSimPsRegStatus            = NAS_MML_GetSimPsRegStatus();

    /* 填充消息头 */
    pstServiceStatusInd->MsgHeader.ulMsgName         = ID_MMC_MMA_SERVICE_STATUS_IND;
    pstServiceStatusInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstServiceStatusInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstServiceStatusInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstServiceStatusInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstServiceStatusInd->MsgHeader.ulLength          = sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstServiceStatusInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaServiceStatusInd()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaRegStatusInd(
    VOS_UINT32                          ulCnDomainId,
    MMA_MMC_REG_STATE_ENUM_UINT8        enRegStatus
)
{
    /* 按MMC_MMA_SERVICE_STATUS_IND_STRU消息结果封装 */
    MMC_MMA_SERVICE_STATUS_IND_STRU    *pstServiceStatusInd = VOS_NULL_PTR;

    pstServiceStatusInd = (MMC_MMA_SERVICE_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstServiceStatusInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaServiceStatusInd():ERROR:Memory Alloc Error for pstServiceStatusInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstServiceStatusInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstServiceStatusInd->bitOpSrvSta                 = VOS_FALSE;
    pstServiceStatusInd->bitOpRegSta                 = VOS_TRUE;
    pstServiceStatusInd->enRegState                  = enRegStatus;
    pstServiceStatusInd->enCnDomainId                = ulCnDomainId;
    pstServiceStatusInd->ucSimCsRegStatus            = NAS_MML_GetSimCsRegStatus();
    pstServiceStatusInd->ucSimPsRegStatus            = NAS_MML_GetSimPsRegStatus();


    /* 填充消息头 */
    pstServiceStatusInd->MsgHeader.ulMsgName         = ID_MMC_MMA_SERVICE_STATUS_IND;
    pstServiceStatusInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstServiceStatusInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstServiceStatusInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstServiceStatusInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstServiceStatusInd->MsgHeader.ulLength          = sizeof(MMC_MMA_SERVICE_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstServiceStatusInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaServiceStatusInd()");
    }
    return;
}


VOS_VOID NAS_MMC_SndMmaCoverageInd(
    VOS_UINT8                           ucType
)
{
    MMC_MMA_COVERAGE_AREA_IND_STRU     *pstCoverageInd   = VOS_NULL_PTR;
    NAS_MML_PLMN_ID_STRU               *pstCurrPlmn      = VOS_NULL_PTR;
    NAS_MML_AVAIL_TIMER_CFG_STRU       *pstAvailTimerCfg = VOS_NULL_PTR;
    VOS_UINT32                          ulDeepTimerLen;

    pstCoverageInd = (MMC_MMA_COVERAGE_AREA_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                          sizeof(MMC_MMA_COVERAGE_AREA_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstCoverageInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaCoverageInd():ERROR:Memory Alloc Error for pstCoverageInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstCoverageInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_COVERAGE_AREA_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if ( MMA_MMC_COVERAGE_AREA_CHANGE_MODE_LOST == ucType)
    {
        pstCoverageInd->enCoverageChgMode  = MMA_MMC_COVERAGE_AREA_CHANGE_MODE_LOST;
    }
    else
    {
        pstCoverageInd->enCoverageChgMode  = MMA_MMC_COVERAGE_AREA_CHANGE_MODE_ENTER;

    }

    pstCoverageInd->enCsSvcSta             = NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_CS);
    pstCoverageInd->enPsSvcSta             = NAS_MMC_GetServiceStatusForMma(MMA_MMC_SRVDOMAIN_PS);

    /* 用于获取当前驻留网络的PLMN */
    pstCurrPlmn = NAS_MML_GetCurrCampPlmnId();

    pstCoverageInd->stPlmnId.ulMcc           = pstCurrPlmn->ulMcc;
    pstCoverageInd->stPlmnId.ulMnc           = pstCurrPlmn->ulMnc;

    /* 用于获取available timer定时器深睡的时长 */
    pstAvailTimerCfg = NAS_MML_GetAvailTimerCfg();

    ulDeepTimerLen = (pstAvailTimerCfg->ulDeepSearchTimeLen) * 1000;

    if ( NAS_MMC_GetNextAvailableTimerValue() < ulDeepTimerLen)
    {
        pstCoverageInd->ulTimMaxFlg        = VOS_FALSE;
    }
    else
    {
        pstCoverageInd->ulTimMaxFlg        = VOS_TRUE;
    }

    /* 填充消息头 */
    pstCoverageInd->MsgHeader.ulMsgName         = ID_MMC_MMA_COVERAGE_AREA_IND;
    pstCoverageInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstCoverageInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstCoverageInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstCoverageInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstCoverageInd->MsgHeader.ulLength          = sizeof(MMC_MMA_COVERAGE_AREA_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstCoverageInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaCoverageInd()");
    }
    return;
}
VOS_VOID NAS_MMC_SndMmaMmInfo(
    NAS_MM_INFO_IND_STRU               *pstMmInfo
)
{
    MMC_MMA_MM_INFO_IND_STRU           *pstMmInfoInd       = VOS_NULL_PTR;
    NAS_MML_OPERATOR_NAME_INFO_STRU    *pstNasOperatorName = VOS_NULL_PTR;

    pstMmInfoInd = (MMC_MMA_MM_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_MM_INFO_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstMmInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaMmInfo():ERROR:Memory Alloc Error for pstMmInfoInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstMmInfoInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_MM_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 按MMC_MMA_MM_INFO_IND_STRU消息封装发送 */
    pstMmInfoInd->ucIeFlg        = pstMmInfo->ucIeFlg;
    pstMmInfoInd->ucDST          = pstMmInfo->ucDST;
    pstMmInfoInd->cLocalTimeZone = pstMmInfo->cLocalTimeZone;
    PS_MEM_CPY(pstMmInfoInd->aucLSAID, pstMmInfo->ucLSAID, sizeof(pstMmInfoInd->aucLSAID));
    pstMmInfoInd->stUniversalTimeandLocalTimeZone = pstMmInfo->stUniversalTimeandLocalTimeZone;
    pstNasOperatorName           = NAS_MML_GetOperatorNameInfo();
    pstMmInfoInd->stName         = *pstNasOperatorName;

    /* 填充消息头 */
    pstMmInfoInd->MsgHeader.ulMsgName         = ID_MMC_MMA_MM_INFO_IND;
    pstMmInfoInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMmInfoInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstMmInfoInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMmInfoInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstMmInfoInd->MsgHeader.ulLength          = sizeof(MMC_MMA_MM_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstMmInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaMmInfo()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaDataTranAttri(
    VOS_UINT8                           ucDataTranAttri
)
{
    MMC_MMA_DATATRAN_ATTRI_IND_STRU    *pstDataTransAttriInd = VOS_NULL_PTR;

    pstDataTransAttriInd = (MMC_MMA_DATATRAN_ATTRI_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                               sizeof(MMC_MMA_DATATRAN_ATTRI_IND_STRU)- VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstDataTransAttriInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDataTranAttri():ERROR:Memory Alloc Error for pstDataTransAttriInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstDataTransAttriInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_DATATRAN_ATTRI_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstDataTransAttriInd->enDataTranAttri             = ucDataTranAttri;

    /* 填充消息头 */
    pstDataTransAttriInd->MsgHeader.ulMsgName         = ID_MMC_MMA_DATATRAN_ATTRI_IND;
    pstDataTransAttriInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstDataTransAttriInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstDataTransAttriInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstDataTransAttriInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstDataTransAttriInd->MsgHeader.ulLength          = sizeof(MMC_MMA_DATATRAN_ATTRI_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstDataTransAttriInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaDataTranAttri()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaSysInfo( VOS_VOID )
{
    MMC_MMA_SYS_INFO_IND_STRU          *pstSysInfoInd       = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo     = VOS_NULL_PTR;
    VOS_UINT32                          i;

    VOS_UINT32                          ulForbiddenFlg;

#if (FEATURE_ON == FEATURE_LTE)
    MMC_LMM_PLMN_ID_STRU                stLmmPlmnId;
#endif

    ulForbiddenFlg            = VOS_FALSE;

    pstSysInfoInd = (MMC_MMA_SYS_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstSysInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaSysInfo():ERROR:Memory Alloc Error for pstSysInfoInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSysInfoInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充MMC_MMA_SYS_INFO_IND_STRU消息*/
    pstCampPlmnInfo                             = NAS_MML_GetCurrCampPlmnInfo();

    pstSysInfoInd->stCellId.ucCellNum                  = pstCampPlmnInfo->stCampCellInfo.ucCellNum;
    for ( i = 0; i < pstCampPlmnInfo->stCampCellInfo.ucCellNum ; i++ )
    {
        pstSysInfoInd->stCellId.aulCellId[i] = pstCampPlmnInfo->stCampCellInfo.astCellInfo[i].ulCellId;
    }

    pstSysInfoInd->ucRac            = pstCampPlmnInfo->ucRac;
    pstSysInfoInd->usLac            = (pstCampPlmnInfo->stLai.aucLac[0] << 8) | (pstCampPlmnInfo->stLai.aucLac[1]);
    pstSysInfoInd->stPlmnId.ulMcc   = pstCampPlmnInfo->stLai.stPlmnId.ulMcc;
    pstSysInfoInd->stPlmnId.ulMnc   = pstCampPlmnInfo->stLai.stPlmnId.ulMnc;
    pstSysInfoInd->enCurNetWork     = NAS_MML_GetCurrNetRatType();
    pstSysInfoInd->ucSysSubMode     = NAS_MMC_CovertMmlSysSubModeToMmaSysSubMode(pstCampPlmnInfo->enSysSubMode);

    pstSysInfoInd->ucCsAttachAllowFlag = NAS_MML_GetCsAttachAllowFlg();
    pstSysInfoInd->ucPsAttachAllowFlag = NAS_MML_GetPsAttachAllowFlg();

    pstSysInfoInd->usArfcn          = NAS_MML_GetCurrCampArfcn();
    pstSysInfoInd->enPrioClass      = NAS_MMC_GetPlmnPrioClass(&(pstCampPlmnInfo->stLai.stPlmnId));

    ulForbiddenFlg = NAS_MMC_GetCurrentLaiForbbidenType();

#if (FEATURE_ON == FEATURE_LTE)
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        /* 将PLMN ID进行转换 */
        NAS_MML_ConvertNasPlmnToLMMFormat(&stLmmPlmnId, &pstCampPlmnInfo->stLai.stPlmnId);

        /* 禁止网络或者access bar都认为是forbidden plmn */
        if (MMC_LMM_PLMN_ALLOW_REG != Nas_PlmnIdIsForbid(&stLmmPlmnId))
        {
            ulForbiddenFlg = VOS_TRUE;
        }
    }
    else
#endif
    {
        if (VOS_TRUE == NAS_MML_GetPsRestrictRegisterFlg())
        {
            ulForbiddenFlg = MMCMM_FORBIDDEN_PLMN;
        }
    }

    pstSysInfoInd->ucIsFobbiddenPlmnFlag = (VOS_UINT8)ulForbiddenFlg;

    pstSysInfoInd->ucRoamFlag       = VOS_FALSE;

    pstSysInfoInd->ucPsSupportFlg   = NAS_MML_GetPsSupportFLg();

    /* 在Hplmn上CS注册被拒后，非E5时，不在发起注册 ,E5时，HplmnRejlist不记录值 */
    if (VOS_TRUE == NAS_MML_ComparePlmnIdWithHplmn(NAS_MML_GetCurrCampPlmnId()))
    {
        if (VOS_FALSE == NAS_MMC_GetHPlmnSpecDomainAccessCapa(NAS_MMC_REG_DOMAIN_PS))
        {
            if (MMCMM_NO_FORBIDDEN == ulForbiddenFlg)
            {
                pstSysInfoInd->ucIsFobbiddenPlmnFlag = MMCMM_FORBIDDEN_PLMN;
            }

            if ( VOS_TRUE == NAS_MML_GetHplmnRegisterCtrlFlg())
            {
                pstSysInfoInd->ucIsFobbiddenPlmnFlag  &= (~MMCMM_FORBIDDEN_PLMN);
            }
        }
    }
    else
    {
        pstSysInfoInd->ucRoamFlag   = VOS_TRUE;
    }

    /* 填充消息头 */
    pstSysInfoInd->MsgHeader.ulMsgName         = ID_MMC_MMA_SYS_INFO_IND;
    pstSysInfoInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSysInfoInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSysInfoInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSysInfoInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstSysInfoInd->MsgHeader.ulLength          = sizeof(MMC_MMA_SYS_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstSysInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaSysInfo()");
    }

    return;
}



VOS_VOID NAS_MMC_SndMmaRegResultInd(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enSrvDomain,
    VOS_UINT8                           ucResult,
    NAS_MMC_REG_FAIL_CAUSE_ENUM_UINT16  enRejCause
)
{
    MMC_MMA_REG_RESULT_IND_STRU        *pstRegResultInd       = VOS_NULL_PTR;

    pstRegResultInd = (MMC_MMA_REG_RESULT_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_REG_RESULT_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstRegResultInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaRegResultInd():ERROR:Memory Alloc Error for pstRejInfoInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstRegResultInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_REG_RESULT_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息*/
    pstRegResultInd->enCnDomainId   = enSrvDomain;
    pstRegResultInd->ucRegRslt      = ucResult;

    if (VOS_FALSE == ucResult)
    {
        pstRegResultInd->enRejCause     = enRejCause;

        /* 只上报小于等于256或者等于258、301、302、303的拒绝原因值，其余内部扩展的拒绝原因值不进行上报 */
        if ((pstRegResultInd->enRejCause <= NAS_MML_REG_FAIL_CAUSE_AUTH_REJ)
         || (NAS_MML_REG_FAIL_CAUSE_COMB_REG_CS_FAIL_OTHER_CAUSE == pstRegResultInd->enRejCause)
         || (NAS_MML_REG_FAIL_CAUSE_TIMER_TIMEOUT == pstRegResultInd->enRejCause)
         || (NAS_MML_REG_FAIL_CAUSE_RR_CONN_EST_FAIL == pstRegResultInd->enRejCause)
         || (NAS_MML_REG_FAIL_CAUSE_RR_CONN_FAIL == pstRegResultInd->enRejCause))
        {
            pstRegResultInd->ucReportCauseCtrl = VOS_TRUE;
        }
    }

    /* 填充消息头 */
    pstRegResultInd->MsgHeader.ulMsgName         = ID_MMC_MMA_REG_RESULT_IND;
    pstRegResultInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstRegResultInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstRegResultInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstRegResultInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstRegResultInd->MsgHeader.ulLength          = sizeof(MMC_MMA_REG_RESULT_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstRegResultInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaRegResultInd()");
    }

    return;
}



VOS_UINT32 NAS_MMC_SndMmaServRejRsltInd(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId,
    VOS_UINT16                          usRejCause
)
{
    MMC_MMA_SERV_REJ_IND_STRU          *pstServReqRsltInd = VOS_NULL_PTR;

    pstServReqRsltInd = (MMC_MMA_SERV_REJ_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_SERV_REJ_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if(VOS_NULL_PTR == pstServReqRsltInd)
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaServRejRsltInd():ERROR:Memory Alloc Error");
        return VOS_ERR;
    }
    PS_MEM_SET(((VOS_UINT8*)pstServReqRsltInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SERV_REJ_IND_STRU) - VOS_MSG_HEAD_LENGTH);


    /* 填充消息头 */
    pstServReqRsltInd->MsgHeader.ulMsgName        = ID_MMC_MMA_SRV_REJ_IND;
    pstServReqRsltInd->MsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstServReqRsltInd->MsgHeader.ulSenderPid      = WUEPS_PID_MMC;
    pstServReqRsltInd->MsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstServReqRsltInd->MsgHeader.ulReceiverPid    = WUEPS_PID_MMA;
    pstServReqRsltInd->MsgHeader.ulLength         = sizeof(MMC_MMA_SERV_REJ_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填充消息 */
    pstServReqRsltInd->enCnDomainId = enCnDomainId;
    pstServReqRsltInd->usRejCause   = usRejCause;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstServReqRsltInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error:Failed in NAS_MMC_SndMmaServRejRsltInd()");
    }

    return VOS_OK;
}
VOS_VOID NAS_MMC_SndMmaUsimAuthFailInd(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enSrvDomain,
    NAS_MMC_SIM_AUTH_FAIL_ENUM_UINT16   enRejCause
                    )
{
    MMC_MMA_USIM_AUTH_FAIL_IND_STRU    *pstUsimAuthFailInd       = VOS_NULL_PTR;

    pstUsimAuthFailInd = (MMC_MMA_USIM_AUTH_FAIL_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_USIM_AUTH_FAIL_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstUsimAuthFailInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaUsimAuthFailInd():ERROR:Memory Alloc Error for pstUsimAuthFailInds ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstUsimAuthFailInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_USIM_AUTH_FAIL_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息*/
    pstUsimAuthFailInd->enCnDomainId   = enSrvDomain;
    pstUsimAuthFailInd->enRejCause     = enRejCause;

    /* 填充消息头 */
    pstUsimAuthFailInd->MsgHeader.ulMsgName         = ID_MMC_MMA_USIM_AUTH_FAIL_IND;
    pstUsimAuthFailInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstUsimAuthFailInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstUsimAuthFailInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstUsimAuthFailInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstUsimAuthFailInd->MsgHeader.ulLength          = sizeof(MMC_MMA_USIM_AUTH_FAIL_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstUsimAuthFailInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaUsimAuthFailInd()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaCsServiceConnStatusInd(
    VOS_UINT8                           ucCsServiceConnStatusFlag
)
{
    MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU    *pstCsServiceConnStatusInd = VOS_NULL_PTR;

    pstCsServiceConnStatusInd = (MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstCsServiceConnStatusInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaCsServiceConnStatusInd():ERROR:Memory Alloc Error for pstUsimAuthFailInds ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstCsServiceConnStatusInd) + VOS_MSG_HEAD_LENGTH, 0,
               sizeof(MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstCsServiceConnStatusInd->stMsgHeader.ulMsgName         = ID_MMC_MMA_CS_SERVICE_CONN_STATUS_IND;
    pstCsServiceConnStatusInd->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstCsServiceConnStatusInd->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstCsServiceConnStatusInd->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstCsServiceConnStatusInd->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstCsServiceConnStatusInd->stMsgHeader.ulLength          = sizeof(MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 填充消息*/
    pstCsServiceConnStatusInd->ucCsServiceConnStatusFlag   = ucCsServiceConnStatusFlag;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstCsServiceConnStatusInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaCsServiceConnStatusInd()");
    }

    return;
}



VOS_VOID NAS_MMC_SndMmaRssiInd(MMA_MMC_SCELL_MEAS_REPORT_TYPE_UN *punMeasReportType)
{
    MMC_MMA_RSSI_IND_STRU              *pstRssiInfoInd      = VOS_NULL_PTR;
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo     = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstRssiInfoInd = (MMC_MMA_RSSI_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_RSSI_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstRssiInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaRssiInd():ERROR:Memory Alloc Error for pstRssiInfoInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstRssiInfoInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_RSSI_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 按MMA_MMC_RSSI_INFO_STRU消息封装 */
    pstCampPlmnInfo                             = NAS_MML_GetCurrCampPlmnInfo();
    pstRssiInfoInd->usCellDlFreq = pstCampPlmnInfo->stCampCellInfo.usCellDlFreq;
    pstRssiInfoInd->usCellUlFreq = pstCampPlmnInfo->stCampCellInfo.usCellUlFreq;
    pstRssiInfoInd->sUeRfPower   = pstCampPlmnInfo->stCampCellInfo.sUeRfPower;
    pstRssiInfoInd->ucRssiNum    = pstCampPlmnInfo->stCampCellInfo.ucRssiNum;

    for ( i = 0; i < pstRssiInfoInd->ucRssiNum ; i++ )
    {
        pstRssiInfoInd->astRssi[i].ucBer      = pstCampPlmnInfo->stCampCellInfo.astRssiInfo[i].ucChannalQual;
        pstRssiInfoInd->astRssi[i].sRssiValue = pstCampPlmnInfo->stCampCellInfo.astRssiInfo[i].sRssiValue;
        pstRssiInfoInd->astRssi[i].sRscpValue = pstCampPlmnInfo->stCampCellInfo.astRssiInfo[i].sRscpValue;
    }

    PS_MEM_CPY((VOS_UINT8 *)&(pstRssiInfoInd->unMeasReportType),
                   (VOS_UINT8 *)punMeasReportType,
                   sizeof(MMA_MMC_SCELL_MEAS_REPORT_TYPE_UN));

    /* 填充消息头 */
    pstRssiInfoInd->MsgHeader.ulMsgName         = ID_MMC_MMA_RSSI_IND;
    pstRssiInfoInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstRssiInfoInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstRssiInfoInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstRssiInfoInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstRssiInfoInd->MsgHeader.ulLength          = sizeof(MMC_MMA_RSSI_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstRssiInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaRssiInd()");
    }

    return;
}






VOS_VOID NAS_MMC_SndMmaSysCfgCnf(
    MMA_MMC_SYS_CFG_SET_RESULT_ENUM_UINT32                   ulRst
)
{
    MMC_MMA_SYS_CFG_SET_CNF_STRU        *pstSyscfgSetCnf = VOS_NULL_PTR;

    pstSyscfgSetCnf = (MMC_MMA_SYS_CFG_SET_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                          sizeof(MMC_MMA_SYS_CFG_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstSyscfgSetCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaSysCfgCnf():ERROR:Memory Alloc Error for pstSyscfgSetCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSyscfgSetCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SYS_CFG_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSyscfgSetCnf->enRst                       = ulRst;

    /* 填充消息头 */
    pstSyscfgSetCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_SYS_CFG_CNF;
    pstSyscfgSetCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSyscfgSetCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSyscfgSetCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSyscfgSetCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstSyscfgSetCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_SYS_CFG_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstSyscfgSetCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaSysCfgCnf()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaNetScanCnf(
    MMC_MMA_NET_SCAN_CNF_STRU             *pstMmcMmaNetScanCnfMsg
)
{
    MMC_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf       = VOS_NULL_PTR;

    pstNetScanCnf = (MMC_MMA_NET_SCAN_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                          sizeof(MMC_MMA_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstNetScanCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaNetScanCnf():ERROR:Memory Alloc Error");
        return;
    }

    PS_MEM_SET( ((VOS_UINT8*)pstNetScanCnf) + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(MMC_MMA_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 填充消息头 */
    pstNetScanCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_NET_SCAN_CNF;
    pstNetScanCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstNetScanCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstNetScanCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstNetScanCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstNetScanCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstNetScanCnf->enResult                    = pstMmcMmaNetScanCnfMsg->enResult;
    pstNetScanCnf->enCause                     = pstMmcMmaNetScanCnfMsg->enCause;
    pstNetScanCnf->ucFreqNum                   = pstMmcMmaNetScanCnfMsg->ucFreqNum;

    PS_MEM_CPY( pstNetScanCnf->astNetScanInfo,
                pstMmcMmaNetScanCnfMsg->astNetScanInfo,
                sizeof(pstNetScanCnf->astNetScanInfo) );

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstNetScanCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaNetScanCnf()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaAbortNetScanCnf(
    MMC_MMA_ABORT_NET_SCAN_CNF_STRU    *pstSndMmaAbortNetScanCnfMsg
)
{
    MMC_MMA_ABORT_NET_SCAN_CNF_STRU    *pstNetScanStopCnf   = VOS_NULL_PTR;

    pstNetScanStopCnf = (MMC_MMA_ABORT_NET_SCAN_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                          sizeof(MMC_MMA_ABORT_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstNetScanStopCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaAbortNetScanCnf():ERROR:Memory Alloc Error");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstNetScanStopCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_ABORT_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstNetScanStopCnf->MsgHeader.ulMsgName                  = ID_MMC_MMA_ABORT_NET_SCAN_CNF;
    pstNetScanStopCnf->MsgHeader.ulSenderCpuId              = VOS_LOCAL_CPUID;
    pstNetScanStopCnf->MsgHeader.ulSenderPid                = WUEPS_PID_MMC;
    pstNetScanStopCnf->MsgHeader.ulReceiverCpuId            = VOS_LOCAL_CPUID;
    pstNetScanStopCnf->MsgHeader.ulReceiverPid              = WUEPS_PID_MMA;
    pstNetScanStopCnf->MsgHeader.ulLength                   = sizeof(MMC_MMA_ABORT_NET_SCAN_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstNetScanStopCnf->ucFreqNum                            = pstSndMmaAbortNetScanCnfMsg->ucFreqNum;

    PS_MEM_CPY( pstNetScanStopCnf->astNetScanInfo,
                pstSndMmaAbortNetScanCnfMsg->astNetScanInfo,
                sizeof(pstNetScanStopCnf->astNetScanInfo) );


    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstNetScanStopCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaAbortNetScanCnf()");
    }


    return;
}
VOS_VOID NAS_Mmc_SndMmaPowerOffCnf(VOS_VOID)
{
    MMC_MMA_POWER_OFF_CNF_STRU         *pstPowerOffCnf = VOS_NULL_PTR;

    pstPowerOffCnf = (MMC_MMA_POWER_OFF_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_POWER_OFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstPowerOffCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_Mmc_SndMmaPowerOffCnf():ERROR:Memory Alloc Error for pstPowerOffCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPowerOffCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_POWER_OFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstPowerOffCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_POWER_OFF_CNF;

    /* 填充消息头 */
    pstPowerOffCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPowerOffCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPowerOffCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPowerOffCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPowerOffCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_POWER_OFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPowerOffCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_Mmc_SndMmaPowerOffCnf()");
    }

    NAS_TRACE_HIGH("Send Msg!");

    return;
}
VOS_VOID NAS_Mmc_SndMmaStartCnf(VOS_UINT32  ulResult)
{
    MMC_MMA_START_CNF_STRU             *pstStartCnf = VOS_NULL_PTR;

    pstStartCnf = (MMC_MMA_START_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                      sizeof(MMC_MMA_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstStartCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_Mmc_SndMmaStartCnf():ERROR:Memory Alloc Error for pstStartCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstStartCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstStartCnf->ulResult                    = ulResult;

    /* 填充消息头 */
    pstStartCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_START_CNF;
    pstStartCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstStartCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstStartCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstStartCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstStartCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstStartCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_Mmc_SndMmaStartCnf()");
    }

    NAS_TRACE_HIGH("Send Msg!");

    return;
}
VOS_VOID NAS_MMC_SndMmaPlmnListAbortCnf(
    MMC_MMA_PLMN_LIST_ABORT_CNF_STRU    *pstSndMmaMsg
)
{
    MMC_MMA_PLMN_LIST_ABORT_CNF_STRU    *pstPlmnListAbortCnf = VOS_NULL_PTR;
    VOS_UINT32                           i;

    pstPlmnListAbortCnf = (MMC_MMA_PLMN_LIST_ABORT_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                              sizeof(MMC_MMA_PLMN_LIST_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstPlmnListAbortCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaPlmnListAbortCnf():ERROR:Memory Alloc Error for pstPlmnListAbortCnf ");
        return;
    }

    /* 填充消息头 */
    PS_MEM_SET(((VOS_UINT8*)pstPlmnListAbortCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_LIST_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    pstPlmnListAbortCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_LIST_ABORT_CNF;
    pstPlmnListAbortCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPlmnListAbortCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPlmnListAbortCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPlmnListAbortCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPlmnListAbortCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_LIST_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 若传进来的消息为空指针则表示列表为空 */
    if ( VOS_NULL_PTR == pstSndMmaMsg)
    {
        pstPlmnListAbortCnf->ulCnt = 0;

        if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnListAbortCnf))
        {
            NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnListAbortCnf()");
        }

        return;
    }


    /* 转换GU格式的消息为MMA格式的 */
    pstPlmnListAbortCnf->ulCnt  = pstSndMmaMsg->ulCnt;

    for (i =0; i<pstSndMmaMsg->ulCnt; i++)
    {
        pstPlmnListAbortCnf->aPlmnList[i].ulMcc = pstSndMmaMsg->aPlmnList[i].ulMcc;
        pstPlmnListAbortCnf->aPlmnList[i].ulMnc = pstSndMmaMsg->aPlmnList[i].ulMnc;

        pstPlmnListAbortCnf->aucPlmnStatus[i]   = pstSndMmaMsg->aucPlmnStatus[i];
        pstPlmnListAbortCnf->aucRaMode[i]       = pstSndMmaMsg->aucRaMode[i];
    }


    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnListAbortCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnListAbortCnf()");
    }
}


VOS_VOID NAS_MMC_SndMmaPlmnListInd(
    MMC_MMA_PLMN_LIST_CNF_STRU         *pstSndMmaMsg
)
{
    MMC_MMA_PLMN_LIST_CNF_STRU          *pstPlmnListCnf = VOS_NULL_PTR;
    VOS_UINT32                           i;

    pstPlmnListCnf = (MMC_MMA_PLMN_LIST_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_PLMN_LIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstPlmnListCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachInd():ERROR:Memory Alloc Error for pstPlmnListCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPlmnListCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_LIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 转换GU格式的消息为MMA格式的 */
    pstPlmnListCnf->ulCnt  = pstSndMmaMsg->ulCnt;

    for (i =0; i<pstSndMmaMsg->ulCnt; i++)
    {
        pstPlmnListCnf->aPlmnList[i].ulMcc = pstSndMmaMsg->aPlmnList[i].ulMcc;
        pstPlmnListCnf->aPlmnList[i].ulMnc = pstSndMmaMsg->aPlmnList[i].ulMnc;

        pstPlmnListCnf->aucPlmnStatus[i]   = pstSndMmaMsg->aucPlmnStatus[i];
        pstPlmnListCnf->aucRaMode[i]       = pstSndMmaMsg->aucRaMode[i];
    }

    /* 填充消息头 */
    pstPlmnListCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_LIST_CNF;
    pstPlmnListCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPlmnListCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPlmnListCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPlmnListCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPlmnListCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_LIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnListCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPlmnListInd()");
    }

    return;
}


VOS_VOID NAS_Mmc_SndMmaPlmnListRej(VOS_VOID)
{
    MMC_MMA_PLMN_LIST_REJ_STRU          *pstPlmnListRej = VOS_NULL_PTR;

    pstPlmnListRej = (MMC_MMA_PLMN_LIST_REJ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_PLMN_LIST_REJ_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstPlmnListRej )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_Mmc_SndMmaPlmnListRej():ERROR:Memory Alloc Error for pstPlmnListRej ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPlmnListRej) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_LIST_REJ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstPlmnListRej->MsgHeader.ulMsgName         = ID_MMC_MMA_PLMN_LIST_REJ;
    pstPlmnListRej->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPlmnListRej->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPlmnListRej->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPlmnListRej->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPlmnListRej->MsgHeader.ulLength          = sizeof(MMC_MMA_PLMN_LIST_REJ_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPlmnListRej))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_Mmc_SndMmaPlmnListRej()");
    }

    return;
}
VOS_VOID  NAS_MMC_GetOperPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST          *pstRptMmaOperList
)
{
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOperPlmnList = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstOperPlmnList                     = NAS_MML_GetSimOperPlmnList();

    pstRptMmaOperList->usCurPlmnNum     = pstOperPlmnList->usOperPlmnNum;

    if (pstRptMmaOperList->usCurPlmnNum > TAF_USER_MAX_PLMN_NUM)
    {
        pstRptMmaOperList->usCurPlmnNum = TAF_USER_MAX_PLMN_NUM;
    }

    for(i = 0; i < pstRptMmaOperList->usCurPlmnNum; i++)
    {
        PS_MEM_CPY(&(pstRptMmaOperList->astPlmnInfo[i].stPlmn) , &(pstOperPlmnList->astOperPlmnInfo[i].stPlmnId), sizeof(MMA_MMC_PLMN_ID_STRU));

        pstRptMmaOperList->astPlmnInfo[i].usRaMode  = pstOperPlmnList->astOperPlmnInfo[i].usSimRat;
    }
}


VOS_VOID  NAS_MMC_GetUserPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST          *pstRptMmaUserList
)
{
    NAS_MML_SIM_USERPLMN_INFO_STRU     *pstUserPlmnList = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstUserPlmnList                     = NAS_MML_GetSimUserPlmnList();

    pstRptMmaUserList->usCurPlmnNum     = (VOS_UINT16)pstUserPlmnList->ucUserPlmnNum;

    if (pstRptMmaUserList->usCurPlmnNum > NAS_MML_MAX_USERPLMN_NUM)
    {
        pstRptMmaUserList->usCurPlmnNum = NAS_MML_MAX_USERPLMN_NUM;
    }

    for(i = 0; i < pstRptMmaUserList->usCurPlmnNum; i++)
    {
        PS_MEM_CPY(&(pstRptMmaUserList->astPlmnInfo[i].stPlmn) , &(pstUserPlmnList->astUserPlmnInfo[i].stPlmnId), sizeof(MMA_MMC_PLMN_ID_STRU));

        pstRptMmaUserList->astPlmnInfo[i].usRaMode  = pstUserPlmnList->astUserPlmnInfo[i].usSimRat;
    }
}


VOS_VOID  NAS_MMC_GetHPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST           *pstRptMmaHplmnList
)
{
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU *pstHplmnWithRat = VOS_NULL_PTR;
    VOS_UINT32                            i;

    pstHplmnWithRat                     = NAS_MML_GetSimHplmnWithRatList();

    pstRptMmaHplmnList->usCurPlmnNum    = (VOS_UINT16)pstHplmnWithRat->ucHPlmnWithRatNum;

    if (pstRptMmaHplmnList->usCurPlmnNum > NAS_MML_MAX_HPLMN_WITH_RAT_NUM)
    {
        pstRptMmaHplmnList->usCurPlmnNum = NAS_MML_MAX_HPLMN_WITH_RAT_NUM;
    }

    for(i = 0; i < pstRptMmaHplmnList->usCurPlmnNum; i++)
    {
        PS_MEM_CPY(&(pstRptMmaHplmnList->astPlmnInfo[i].stPlmn) , &(pstHplmnWithRat->astHPlmnWithRatInfo[i].stPlmnId), sizeof(MMA_MMC_PLMN_ID_STRU));

        pstRptMmaHplmnList->astPlmnInfo[i].usRaMode = pstHplmnWithRat->astHPlmnWithRatInfo[i].usSimRat;
    }

}


VOS_VOID  NAS_MMC_GetSelPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST          *pstRptMmaSelPlmnList
)
{
    NAS_MML_SIM_SELPLMN_INFO_STRU      *pstSelPlmnList = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstSelPlmnList                      = NAS_MML_GetSimSelPlmnList();

    pstRptMmaSelPlmnList->usCurPlmnNum  = (VOS_UINT16)pstSelPlmnList->ucSelPlmnNum;

    if (pstRptMmaSelPlmnList->usCurPlmnNum > NAS_MML_MAX_SELPLMN_NUM)
    {
        pstRptMmaSelPlmnList->usCurPlmnNum = NAS_MML_MAX_SELPLMN_NUM;
    }

    pstRptMmaSelPlmnList->usCurPlmnNum = NAS_COMM_MIN(pstRptMmaSelPlmnList->usCurPlmnNum, NAS_MML_MAX_SELPLMN_NUM);

    for (i = 0; i <  pstRptMmaSelPlmnList->usCurPlmnNum; i++)
    {
        PS_MEM_CPY(&(pstRptMmaSelPlmnList->astPlmnInfo[i].stPlmn), &(pstSelPlmnList->astPlmnId[i]), sizeof(MMA_MMC_PLMN_ID_STRU));

#if (FEATURE_ON == FEATURE_LTE)
        pstRptMmaSelPlmnList->astPlmnInfo[i].usRaMode   = NAS_MML_SIM_UE_SUPPORT_RAT_SUPPORT_LTE;
#else
        pstRptMmaSelPlmnList->astPlmnInfo[i].usRaMode   = NAS_MML_SIM_UE_SUPPORT_RAT;
#endif
    }

}



NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8  NAS_MMC_GetServiceStatusForMma(
    NAS_MM_COM_SERVICE_DOMAIN_ENUM_UINT8 enSrvDomain
)
{
    VOS_UINT8                           ucSimStatus;
    NAS_MMC_SERVICE_ENUM_UINT8          enServiceStatus;

    /* 卡不在位 */
    if ( VOS_FALSE == NAS_MML_GetSimPresentStatus())
    {
        return MM_COM_SRVST_NO_IMSI;
    }

    if (MMA_MMC_SRVDOMAIN_CS == enSrvDomain)
    {
        ucSimStatus      = NAS_MML_GetSimCsRegStatus();
        enServiceStatus  = NAS_MMC_GetCsServiceStatus();
    }
    else
    {
        ucSimStatus      = NAS_MML_GetSimPsRegStatus();
        enServiceStatus  = NAS_MMC_GetPsServiceStatus();
    }

    /* 该域卡无效 */
    if ( VOS_FALSE == ucSimStatus )
    {
        return MM_COM_SRVST_NO_IMSI;
    }

    if ( NAS_MMC_NORMAL_SERVICE == enServiceStatus )
    {
        return MM_COM_SRVST_NORMAL_SERVICE;
    }

    if ( NAS_MMC_LIMITED_SERVICE_REGION == enServiceStatus )
    {
        return MM_COM_SRVST_LIMITED_SERVICE_REGION;
    }


    if ( NAS_MMC_LIMITED_SERVICE == enServiceStatus )
    {
        return MM_COM_SRVST_LIMITED_SERVICE;
    }

    return MM_COM_SRVST_NO_SERVICE;
}

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

VOS_VOID NAS_MMC_SndCbaMsgNetModInd(
    VOS_UINT8                           ucNetWorkMode
)
{
    TAFMMC_CBS_CURR_NET_MODE_IND_STRU  *pstMsg;
    VOS_UINT32                          ulRet;


    if ( (ucNetWorkMode != NAS_MML_NET_RAT_TYPE_GSM)
      && (ucNetWorkMode != NAS_MML_NET_RAT_TYPE_WCDMA)
      && (ucNetWorkMode != NAS_MML_NET_RAT_TYPE_LTE) )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndCbaMsgNetModInd: NetMode ERROR");
        return;
    }


    pstMsg = (TAFMMC_CBS_CURR_NET_MODE_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                           sizeof(TAFMMC_CBS_CURR_NET_MODE_IND_STRU));


    if( VOS_NULL_PTR == pstMsg )
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_ERROR, "NAS_MMC_SndCbaMsgNetModInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    PS_MEM_SET(pstMsg,0,sizeof(TAFMMC_CBS_CURR_NET_MODE_IND_STRU));

    pstMsg->MsgHeader.ulLength          = sizeof(TAFMMC_CBS_CURR_NET_MODE_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_TAF;

    /* 填写消息类别  */
    pstMsg->MsgHeader.ulMsgName         = TAFMMC_CBS_CURR_NET_MODE_IND;


    if ( NAS_MML_NET_RAT_TYPE_WCDMA == ucNetWorkMode)
    {
        pstMsg->ucCurNetWork            = MN_NET_MODE_WCDMA;
    }
    else if( NAS_MML_NET_RAT_TYPE_GSM == ucNetWorkMode)
    {
        pstMsg->ucCurNetWork            = MN_NET_MODE_GSM;
    }
    else if (NAS_MML_NET_RAT_TYPE_LTE == ucNetWorkMode)
    {
        pstMsg->ucCurNetWork            = MN_NET_MODE_LTE;
    }
    else
    {

    }

    ulRet = PS_SEND_MSG( WUEPS_PID_MMC, pstMsg);

    if( VOS_OK != ulRet )
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_WARNING, "NAS_MMC_SndCbaMsgNetModInd:WARNING: Message sends fail");
        return;
    }

}



/*****************************************************************************
 函 数 名  : NAS_MMC_SndMmaMsgNoNetWorkInd
 功能描述  : 通知CBS模块当前网络模式为非法
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年10月13日
   作    者   : zhoujun \ 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MMC_SndMmaMsgNoNetWorkInd( VOS_VOID )
{
    TAFMMC_CBS_NO_NETWORK_IND_STRU      *pstSndMsg;                               /* 定义原语类型指针                         */
    VOS_UINT32                           ulRet;

    /*申请消息  */
    pstSndMsg = (TAFMMC_CBS_NO_NETWORK_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_MMC,
                                             sizeof(TAFMMC_CBS_NO_NETWORK_IND_STRU));
    if( VOS_NULL_PTR == pstSndMsg )
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_ERROR, "NAS_MMC_SndMmaMsgNoNetWorkInd:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    pstSndMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSndMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_TAF;

    pstSndMsg->MsgHeader.ulMsgName   = TAFMMC_CBS_NO_NETWORK_IND;

    ulRet = PS_SEND_MSG(WUEPS_PID_MMC, pstSndMsg);
    if (VOS_OK != ulRet)
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_WARNING, "NAS_MMC_SndMmaMsgNoNetWorkInd:WARNING: Message sends fail");
    }

    return;
}

#endif


VOS_VOID NAS_MMC_SndMmaSpecPlmnSearchAbortCnf(VOS_VOID)
{
    /* 按MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU封装消息并发送 */
    MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU                *pstSpecPlmnSrchAbortCnf = VOS_NULL_PTR;

    pstSpecPlmnSrchAbortCnf = (MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                                  sizeof(MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstSpecPlmnSrchAbortCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaSpecPlmnSearchAbortCnf():ERROR:Memory Alloc Error for pstSpecPlmnSrchAbortCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSpecPlmnSrchAbortCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulMsgName            = ID_MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF;
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulSenderCpuId        = VOS_LOCAL_CPUID;
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulSenderPid          = WUEPS_PID_MMC;
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulReceiverCpuId      = VOS_LOCAL_CPUID;
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulReceiverPid        = WUEPS_PID_MMA;
    pstSpecPlmnSrchAbortCnf->MsgHeader.ulLength             = sizeof(MMC_MMA_SPEC_PLMN_SEARCH_ABORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstSpecPlmnSrchAbortCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaSpecPlmnSearchAbortCnf()");
    }
    return;

}

VOS_VOID NAS_MMC_SndMmaWCipherInfoInd(
    RRMM_CIPHER_INFO_IND_STRU          *pstCipherInfoInd
)
{

    MMC_MMA_UMTS_CIPHER_INFO_IND_STRU  *pstUmtsCipherInfoInd = VOS_NULL_PTR;

    pstUmtsCipherInfoInd = (MMC_MMA_UMTS_CIPHER_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                               sizeof(MMC_MMA_UMTS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstUmtsCipherInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachInd():ERROR:Memory Alloc Error for pstAppMsg ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstUmtsCipherInfoInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_UMTS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 按MMC_MMA_UMTS_CIPHER_INFO_IND_STRU封装消息并发送 */
    pstUmtsCipherInfoInd->ucCipherAlgorForCSDomain    = pstCipherInfoInd->enCipherAlgorForCSDomain;
    pstUmtsCipherInfoInd->ucCipherAlgorForPSDomain    = pstCipherInfoInd->enCipherAlgorForPSDomain;

    /* 填充消息头 */
    pstUmtsCipherInfoInd->MsgHeader.ulMsgName         = ID_MMC_MMA_UMTS_CIPHER_INFO_IND;
    pstUmtsCipherInfoInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstUmtsCipherInfoInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstUmtsCipherInfoInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstUmtsCipherInfoInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstUmtsCipherInfoInd->MsgHeader.ulLength          = sizeof(MMC_MMA_UMTS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstUmtsCipherInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaWCipherInfoInd()");
    }
    return;
}


VOS_VOID NAS_MMC_SndMmaGCipherInfoInd(
    MMC_GMM_CIPHER_INFO_IND_STRU       *pstCipherInfoInd
)
{

    MMC_MMA_GPRS_CIPHER_INFO_IND_STRU  *pstGprsCipherInfoInd = VOS_NULL_PTR;

    pstGprsCipherInfoInd = (MMC_MMA_GPRS_CIPHER_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                               sizeof(MMC_MMA_GPRS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstGprsCipherInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaDetachInd():ERROR:Memory Alloc Error for pstAppMsg ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstGprsCipherInfoInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_GPRS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 按MMC_MMA_GPRS_CIPHER_INFO_IND_STRU封装消息并发送 */
    pstGprsCipherInfoInd->ucGprsCipher                      = pstCipherInfoInd->ucGprsCipher;
    pstGprsCipherInfoInd->ucGprsCipherAlg                   = pstCipherInfoInd->ucGprsCipherAlg;

    /* 填充消息头 */
    pstGprsCipherInfoInd->MsgHeader.ulMsgName               = ID_MMC_MMA_GPRS_CIPHER_INFO_IND;
    pstGprsCipherInfoInd->MsgHeader.ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstGprsCipherInfoInd->MsgHeader.ulSenderPid             = WUEPS_PID_MMC;
    pstGprsCipherInfoInd->MsgHeader.ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstGprsCipherInfoInd->MsgHeader.ulReceiverPid           = WUEPS_PID_MMA;
    pstGprsCipherInfoInd->MsgHeader.ulLength                = sizeof(MMC_MMA_GPRS_CIPHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstGprsCipherInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_Mmc_SndMmaPowerOffCnf()");
    }
    return;
}



VOS_VOID NAS_MMC_SndMmaUserSpecPlmnSearchRej( VOS_VOID )
{
    MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU   *pstUserSelRej = VOS_NULL_PTR;

    pstUserSelRej = (MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU) - VOS_MSG_HEAD_LENGTH);
    if( VOS_NULL_PTR == pstUserSelRej )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaUserSpecPlmnSearchRej():ERROR:Memory Alloc Error for pstUserSelRej ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstUserSelRej) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstUserSelRej->MsgHeader.ulMsgName                      = ID_MMC_MMA_PLMN_SPECIAL_SEL_REJ;
    pstUserSelRej->MsgHeader.ulSenderCpuId                  = VOS_LOCAL_CPUID;
    pstUserSelRej->MsgHeader.ulSenderPid                    = WUEPS_PID_MMC;
    pstUserSelRej->MsgHeader.ulReceiverCpuId                = VOS_LOCAL_CPUID;
    pstUserSelRej->MsgHeader.ulReceiverPid                  = WUEPS_PID_MMA;
    pstUserSelRej->MsgHeader.ulLength                       = sizeof(MMC_MMA_PLMN_SPECIAL_SEL_REJ_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstUserSelRej))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaUserSpecPlmnSearchRej()");
    }
    return;
}
VOS_VOID NAS_MMC_SndMmaAcInfoChangeInd(
    VOS_UINT32                          ulCnDomainId,
    NAS_MML_ACCESS_RESTRICTION_STRU    *pstAcInfo
)
{
    MMC_MMA_AC_INFO_CHANGE_IND_STRU    *pstAcInfoChgInd = VOS_NULL_PTR;

    pstAcInfoChgInd = (MMC_MMA_AC_INFO_CHANGE_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                         sizeof(MMC_MMA_AC_INFO_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstAcInfoChgInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaAcInfoChangeInd():ERROR:Memory Alloc Error for pstAcInfoChgInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstAcInfoChgInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_AC_INFO_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 按MMC_MMA_AC_INFO_CHANGE_IND_STRU封装消息并发送 */
    pstAcInfoChgInd->enCnDomainId                           = (VOS_UINT8)ulCnDomainId;
    pstAcInfoChgInd->ucRestrictPagingRsp                    = pstAcInfo->ucRestrictPagingRsp;
    pstAcInfoChgInd->ucRestrictRegister                     = pstAcInfo->ucRestrictRegister;

    if ((VOS_TRUE == pstAcInfo->ucRestrictEmergencyService)
     && (VOS_TRUE == pstAcInfo->ucRestrictNormalService))
    {
        pstAcInfoChgInd->enCellAcType = MMA_MMC_CELL_RESTRICTION_TYPE_ALL_SERVICE;
    }
    else if (VOS_TRUE == pstAcInfo->ucRestrictNormalService)
    {
        pstAcInfoChgInd->enCellAcType = MMA_MMC_CELL_RESTRICTION_TYPE_NORMAL_SERVICE;
    }
    else if (VOS_TRUE == pstAcInfo->ucRestrictEmergencyService)
    {
        pstAcInfoChgInd->enCellAcType = MMA_MMC_CELL_RESTRICTION_TYPE_EMERGENCY_SERVICE;
    }
    else
    {
        pstAcInfoChgInd->enCellAcType = MMA_MMC_CELL_RESTRICTION_TYPE_NONE;
    }

    /* 填充消息头 */
    pstAcInfoChgInd->MsgHeader.ulMsgName         = ID_MMC_MMA_AC_INFO_CHANGE_IND;
    pstAcInfoChgInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAcInfoChgInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAcInfoChgInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAcInfoChgInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstAcInfoChgInd->MsgHeader.ulLength          = sizeof(MMC_MMA_AC_INFO_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstAcInfoChgInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaAcInfoChangeInd()");
    }

    return;
}





VOS_VOID NAS_MMC_SndMmaEplmnInfoInd(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEplmnInfo
)
{
    MMC_MMA_EPLMN_INFO_IND_STRU        *pstEplmnInfoInd = VOS_NULL_PTR;

    pstEplmnInfoInd = (MMC_MMA_EPLMN_INFO_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                          sizeof(MMC_MMA_EPLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstEplmnInfoInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaEplmnInfoInd():ERROR:Memory Alloc Error");
        return;
    }

    PS_MEM_SET( ((VOS_UINT8*)pstEplmnInfoInd) + VOS_MSG_HEAD_LENGTH, 0,
                sizeof(MMC_MMA_EPLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH );

    /* 填充消息头 */
    pstEplmnInfoInd->stMsgHeader.ulMsgName         = ID_MMC_MMA_EPLMN_INFO_IND;
    pstEplmnInfoInd->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstEplmnInfoInd->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstEplmnInfoInd->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstEplmnInfoInd->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstEplmnInfoInd->stMsgHeader.ulLength          = sizeof(MMC_MMA_EPLMN_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstEplmnInfoInd->stEplmnInfo.ucEquPlmnNum    = pstEplmnInfo->ucEquPlmnNum;
    PS_MEM_CPY(pstEplmnInfoInd->stEplmnInfo.astEquPlmnAddr, pstEplmnInfo->astEquPlmnAddr,
               sizeof(pstEplmnInfoInd->stEplmnInfo.astEquPlmnAddr));

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstEplmnInfoInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaEplmnInfoInd()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaEOPlmnSetCnf(
    VOS_UINT32                     ulRst
)
{
    MMC_MMA_EOPLMN_SET_CNF_STRU        *pstSPlmnSetCnf = VOS_NULL_PTR;

    pstSPlmnSetCnf = (MMC_MMA_EOPLMN_SET_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                              sizeof(MMC_MMA_EOPLMN_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstSPlmnSetCnf )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaEOPlmnSetCnf():ERROR:Memory Alloc Error for pstSPlmnSetCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSPlmnSetCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_EOPLMN_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    pstSPlmnSetCnf->ulResult                    = ulRst;

    pstSPlmnSetCnf->MsgHeader.ulMsgName         = ID_MMC_MMA_EOPLMN_SET_CNF;
    pstSPlmnSetCnf->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSPlmnSetCnf->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSPlmnSetCnf->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSPlmnSetCnf->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstSPlmnSetCnf->MsgHeader.ulLength          = sizeof(MMC_MMA_EOPLMN_SET_CNF_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstSPlmnSetCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaEOPlmnSetCnf()");
    }

    return;

}


VOS_VOID  NAS_MMC_SndMmaNetworkCapabilityInfoInd(
    MMA_MMC_NW_IMS_VOICE_CAP_ENUM_UINT8                     enNwImsVoCap,
    MMA_MMC_NW_EMC_BS_CAP_ENUM_UINT8                        enNwEmcBsCap,
    MMA_MMC_LTE_CS_CAPBILITY_ENUM_UINT8                     enLteCsCap
)
{
    MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU               *pstSndMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulRet;

    /*申请消息  */
    pstSndMsg = (MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                    WUEPS_PID_MMC,
                    sizeof(MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU));

    if (VOS_NULL_PTR == pstSndMsg)
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_ERROR, "NAS_MMC_SndMmaNetworkCapabilityInfoInd: Memory Alloc Fail!");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSndMsg) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* fill in message header */
    pstSndMsg->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstSndMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;

    /* fill in message name */
    pstSndMsg->MsgHeader.ulMsgName         = ID_MMC_MMA_NETWORK_CAPABILITY_INFO_IND;

    /* fill in message content */
    pstSndMsg->enLteCsCap         = enLteCsCap;    
    
    pstSndMsg->enNwImsVoCap       = enNwImsVoCap;
    pstSndMsg->enNwEmcBsCap       = enNwEmcBsCap;

    ulRet = PS_SEND_MSG(WUEPS_PID_MMC, pstSndMsg);
    if (VOS_OK != ulRet)
    {
        PS_LOG(WUEPS_PID_MMC, VOS_NULL, PS_PRINT_WARNING, "NAS_MMC_SndMmaNetworkCapabilityInfoInd: Message sends fail");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaCampOnInd(
    VOS_UINT8                           ucCampOnFlg
)
{
    MMC_MMA_CAMP_ON_IND_STRU_STRU      *pstCampOnInd       = VOS_NULL_PTR;

    pstCampOnInd = (MMC_MMA_CAMP_ON_IND_STRU_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                        sizeof(MMC_MMA_CAMP_ON_IND_STRU_STRU) - VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pstCampOnInd )
    {
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaCampOnInd():ERROR:Memory Alloc Error for pstCampOnInd ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstCampOnInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_CAMP_ON_IND_STRU_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstCampOnInd->MsgHeader.ulMsgName         = ID_MMC_MMA_CAMP_ON_IND;
    pstCampOnInd->MsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstCampOnInd->MsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstCampOnInd->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstCampOnInd->MsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstCampOnInd->MsgHeader.ulLength          = sizeof(MMC_MMA_CAMP_ON_IND_STRU_STRU) - VOS_MSG_HEAD_LENGTH;

    pstCampOnInd->ucCampOnFlg                 = ucCampOnFlg;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstCampOnInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaCampOnInd()");
    }

    return;
}
VOS_VOID NAS_MMC_SndMmaAcqCnf(
    MMC_MMA_ACQ_RESULT_ENUM_UINT32      enAcqRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
)
{
    MMC_MMA_ACQ_CNF_STRU               *pstAcqCnf = VOS_NULL_PTR;

    /* 申请消息包 */
    pstAcqCnf = (MMC_MMA_ACQ_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_MMA_ACQ_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstAcqCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaAcqCnf():ERROR:Memory Alloc Error for pstAcqCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstAcqCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_ACQ_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstAcqCnf->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAcqCnf->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAcqCnf->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAcqCnf->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstAcqCnf->stMsgHeader.ulLength          = sizeof(MMC_MMA_ACQ_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstAcqCnf->stMsgHeader.ulMsgName         = ID_MMC_MMA_ACQ_CNF;

    /* 填充消息内容 */
    pstAcqCnf->enAcqRslt                     = enAcqRslt;
    pstAcqCnf->usArfcn                       = usArfcn;
    pstAcqCnf->stPlmnId.ulMcc                = pstPlmnId->ulMcc;
    pstAcqCnf->stPlmnId.ulMnc                = pstPlmnId->ulMnc;
    pstAcqCnf->enPrioClass                   = NAS_MMC_GetPlmnPrioClass(pstPlmnId);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstAcqCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaAcqCnf()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaAcqInd(
    MMC_MMA_ACQ_RESULT_ENUM_UINT32      enAcqRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
)
{
    MMC_MMA_ACQ_IND_STRU               *pstAcqInd = VOS_NULL_PTR;

    /* 申请消息包 */
    pstAcqInd = (MMC_MMA_ACQ_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_MMA_ACQ_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstAcqInd)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaAcqInd():ERROR:Memory Alloc Error for pstAcqCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstAcqInd) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_ACQ_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstAcqInd->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstAcqInd->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstAcqInd->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstAcqInd->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstAcqInd->stMsgHeader.ulLength          = sizeof(MMC_MMA_ACQ_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstAcqInd->stMsgHeader.ulMsgName         = ID_MMC_MMA_ACQ_IND;

    /* 填充消息内容 */
    pstAcqInd->enAcqRslt                     = enAcqRslt;
    pstAcqInd->usArfcn                       = usArfcn;
    pstAcqInd->stPlmnId.ulMcc                = pstPlmnId->ulMcc;
    pstAcqInd->stPlmnId.ulMnc                = pstPlmnId->ulMnc;
    pstAcqInd->enPrioClass                   = NAS_MMC_GetPlmnPrioClass(pstPlmnId);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstAcqInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaAcqInd()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaRegCnf(
    MMC_MMA_REG_RESULT_ENUM_UINT32      enRegRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
)
{
    MMC_MMA_REG_CNF_STRU               *pstRegCnf = VOS_NULL_PTR;

    /* 申请消息包 */
    pstRegCnf = (MMC_MMA_REG_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_MMA_REG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstRegCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaRegCnf():ERROR:Memory Alloc Error for pstRegCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstRegCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_REG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstRegCnf->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstRegCnf->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstRegCnf->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstRegCnf->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstRegCnf->stMsgHeader.ulLength          = sizeof(MMC_MMA_REG_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstRegCnf->stMsgHeader.ulMsgName         = ID_MMC_MMA_REG_CNF;

    /* 填充消息内容 */
    pstRegCnf->enRegRslt                     = enRegRslt;
    pstRegCnf->usArfcn                       = usArfcn;
    pstRegCnf->stPlmnId.ulMcc                = pstPlmnId->ulMcc;
    pstRegCnf->stPlmnId.ulMnc                = pstPlmnId->ulMnc;
    pstRegCnf->enPrioClass                   = NAS_MMC_GetPlmnPrioClass(pstPlmnId);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstRegCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaRegCnf()");
    }

    return;
}


VOS_VOID NAS_MMC_SndMmaPowerSaveCnf(VOS_VOID)
{
    MMC_MMA_POWER_SAVE_CNF_STRU        *pstPowerSaveCnf = VOS_NULL_PTR;

    /* 申请消息包 */
    pstPowerSaveCnf = (MMC_MMA_POWER_SAVE_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_MMC,
                    sizeof(MMC_MMA_POWER_SAVE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstPowerSaveCnf)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_MMC_SndMmaPowerSaveCnf():ERROR:Memory Alloc Error for pstPowerSaveCnf ");
        return;
    }

    PS_MEM_SET(((VOS_UINT8*)pstPowerSaveCnf) + VOS_MSG_HEAD_LENGTH, 0, sizeof(MMC_MMA_POWER_SAVE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息头 */
    pstPowerSaveCnf->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstPowerSaveCnf->stMsgHeader.ulSenderPid       = WUEPS_PID_MMC;
    pstPowerSaveCnf->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstPowerSaveCnf->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstPowerSaveCnf->stMsgHeader.ulLength          = sizeof(MMC_MMA_POWER_SAVE_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstPowerSaveCnf->stMsgHeader.ulMsgName         = ID_MMC_MMA_POWER_SAVE_CNF;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_MMC, pstPowerSaveCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "PS_SEND_MSG:Error: Failed in NAS_MMC_SndMmaPowerSaveCnf()");
    }

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



