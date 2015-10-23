

#ifndef __NASMMCSNDMMA_H__
#define __NASMMCSNDMMA_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "Nasrrcinterface.h"
#include  "MmcGmmInterface.H"
#include  "MmcMmInterface.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "MmcLmmInterface.h"
#endif
#include  "NasCommDef.h"
#include "MmaMmcInterface.h"
#include  "MmcMmaItf.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID NAS_MMC_SndMmaUserSpecPlmnSearchCnf(
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8         enResult
)
;


VOS_VOID NAS_MMC_SndMmaPlmnSelectionRsltInd(
    MMA_MMC_PLMN_SELECTION_RESULT_ENUM_UINT32                enResult
);

VOS_VOID NAS_MMC_SndMmaPlmnSelectStartInd(VOS_VOID);

VOS_VOID NAS_MMC_SndMmaDetachInd(
    VOS_UINT32                          ulCnDomainId,
    VOS_UINT32                          ulDetachType,
    VOS_UINT32                          ulCause
);




VOS_VOID NAS_MMC_SndMmaAttachCnf(
     VOS_UINT32                           ulCnDomainId,
     NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8 enMmaServiceStatus,
     VOS_UINT32                           ulOpid
);
VOS_VOID NAS_MMC_SndMmaDetachCnf(
    VOS_UINT32                           ulCnDomainId,
    NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8 enMmaServiceStatus,
    VOS_UINT32                           ulOpid
);



VOS_VOID NAS_MMC_SndMmaServiceStatusInd(
    VOS_UINT32                          ulCnDomainId,
    VOS_UINT32                          ulServiceStatus
);

VOS_VOID NAS_MMC_SndMmaRegStatusInd(
    VOS_UINT32                          ulCnDomainId,
    MMA_MMC_REG_STATE_ENUM_UINT8        enRegStatus
);
VOS_VOID NAS_MMC_SndMmaPlmnReselCnf(
    MMA_MMC_USER_PLMN_SEARCH_RESULT_ENUM_UINT8        enPlmnReselRslt
);

VOS_VOID NAS_Mmc_SndMmaPowerOffCnf(VOS_VOID);

VOS_VOID NAS_Mmc_SndMmaStartCnf(
    VOS_UINT32                          ulResult
);

VOS_VOID NAS_MMC_SndMmaPlmnListAbortCnf(
    MMC_MMA_PLMN_LIST_ABORT_CNF_STRU    *pstSndMmaMsg
);


VOS_VOID NAS_MMC_SndMmaPlmnListInd(
    MMC_MMA_PLMN_LIST_CNF_STRU          *pstSndMmaMsg
);

VOS_VOID NAS_Mmc_SndMmaPlmnListRej(VOS_VOID);

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
VOS_VOID NAS_MMC_SndCbaMsgNetModInd(
    VOS_UINT8                           ucNetWorkMode
);


VOS_VOID  NAS_MMC_SndMmaMsgNoNetWorkInd( VOS_VOID );


#endif



VOS_VOID NAS_MMC_SndMmaMmInfo(
    NAS_MM_INFO_IND_STRU                *pstMmInfo
);

VOS_VOID NAS_MMC_SndMmaCoverageInd(
    VOS_UINT8                               ucType
);

VOS_VOID NAS_MMC_SndMmaDataTranAttri(
    VOS_UINT8                           ucDataTranAttri
);

VOS_VOID NAS_MMC_SndMmaSysInfo( VOS_VOID );

VOS_VOID NAS_MMC_SndMmaRegResultInd( 
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enSrvDomain,
    VOS_UINT8                           ucResult,    
    NAS_MMC_REG_FAIL_CAUSE_ENUM_UINT16  enRejCause
);

VOS_UINT32 NAS_MMC_SndMmaServRejRsltInd(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enCnDomainId,
    VOS_UINT16                          usRejCause
);



VOS_VOID NAS_MMC_SndMmaRssiInd(
    MMA_MMC_SCELL_MEAS_REPORT_TYPE_UN  *punMeasReportType
);


VOS_VOID NAS_MMC_SndAsEquPlmnQueryCnf(
    EQUIVALENT_PLMN_STRU               *pstEquPlmnList,
    VOS_UINT32                          ulOpCurPlmnId,
    NAS_MML_PLMN_ID_STRU               *pstCurPlmnId,
    VOS_UINT32                          ulPid
);

VOS_VOID NAS_MMC_SndRrcPlmnQueryCnf(
    PS_BOOL_ENUM_UINT8                  enQueryWPlmn
);

VOS_VOID NAS_MMC_SndMmaSysCfgCnf(MMA_MMC_SYS_CFG_SET_RESULT_ENUM_UINT32 ulRst);

VOS_VOID NAS_MMC_SndMmaNetScanCnf(
    MMC_MMA_NET_SCAN_CNF_STRU             *pstRcvRrmmMsg
);

VOS_VOID NAS_MMC_SndMmaAbortNetScanCnf(
    MMC_MMA_ABORT_NET_SCAN_CNF_STRU    *pstSndMmaAbortNetScanCnfMsg
);


VOS_VOID NAS_MMC_GetMmInfoPlmnName(
    TAF_MMA_MM_INFO_PLMN_NAME_STRU          *pstNwNameInfo
);

VOS_VOID  NAS_MMC_GetCellInfoForMma(
    MMA_MMC_CAMP_CELL_INFO_STRU                    *pstCellInfo
);

VOS_VOID  NAS_MMC_GetOperPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST           *pstRptMmaOperList
);

VOS_VOID  NAS_MMC_GetUserPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST           *pstRptMmaUserList
);

VOS_VOID  NAS_MMC_GetHPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST           *pstRptMmaHplmnList
);

VOS_VOID  NAS_MMC_GetSelPlmnInfoForMma(
    MMC_TAF_PLMN_LIST_INFO_ST           *pstRptMmaSelPlmnList
);



NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8  NAS_MMC_GetServiceStatusForMma(
    NAS_MM_COM_SERVICE_DOMAIN_ENUM_UINT8 enSrvDomain
);




VOS_VOID NAS_MMC_SndMmaSpecPlmnSearchAbortCnf(VOS_VOID);


VOS_VOID NAS_MMC_SndMmaWCipherInfoInd(
    RRMM_CIPHER_INFO_IND_STRU          *pstCipherInfoInd
);

VOS_VOID NAS_MMC_SndMmaGCipherInfoInd(
    MMC_GMM_CIPHER_INFO_IND_STRU       *pstCipherInfoInd
);


VOS_VOID NAS_MMC_SndMmaUserSpecPlmnSearchRej( VOS_VOID );


VOS_VOID NAS_MMC_SndMmaAcInfoChangeInd(
    VOS_UINT32                          ulCnDomainId,
    NAS_MML_ACCESS_RESTRICTION_STRU    *pstAcInfo
);

VOS_VOID NAS_MMC_SndMmaEOPlmnSetCnf(
    VOS_UINT32                          ulRst
);



VOS_VOID NAS_MMC_SndMmaEplmnInfoInd(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEplmnInfo
);



VOS_VOID NAS_MMC_SndMmaUsimAuthFailInd(
    MMA_MMC_SRVDOMAIN_ENUM_UINT32       enSrvDomain,
    NAS_MMC_SIM_AUTH_FAIL_ENUM_UINT16   enRejCause
);

VOS_VOID NAS_MMC_SndMmaCsServiceConnStatusInd(
    VOS_UINT8                           ucCsServiceConnStatusFlag
);


VOS_VOID  NAS_MMC_SndMmaNetworkCapabilityInfoInd(
    MMA_MMC_NW_IMS_VOICE_CAP_ENUM_UINT8                     enNwImsVoCap,
    MMA_MMC_NW_EMC_BS_CAP_ENUM_UINT8                        enNwEmcBsCap,
    MMA_MMC_LTE_CS_CAPBILITY_ENUM_UINT8                     enLteCsCap
);

VOS_VOID NAS_MMC_SndMmaCampOnInd(
    VOS_UINT8                           ucCampOnFlg 
);

VOS_VOID NAS_MMC_SndMmaAcqCnf(
    MMC_MMA_ACQ_RESULT_ENUM_UINT32      enAcqRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
);

VOS_VOID NAS_MMC_SndMmaAcqInd(
    MMC_MMA_ACQ_RESULT_ENUM_UINT32      enAcqRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
);

VOS_VOID NAS_MMC_SndMmaRegCnf(
    MMC_MMA_REG_RESULT_ENUM_UINT32      enRegRslt,
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT16                          usArfcn
);

VOS_VOID NAS_MMC_SndMmaPowerSaveCnf(VOS_VOID);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasMmcSndMma.h */
