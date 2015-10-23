/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasMmlLib.h
  版 本 号   : 初稿
  作    者   : zhoujun 40661
  生成日期   : 2011年7月25日
  最近修改   :
  功能描述   : NasMmlLib.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年7月25日
    作    者   : zhoujun 40661
    修改内容   : 创建文件

******************************************************************************/
#ifndef _NAS_MML_LIB_H_
#define _NAS_MML_LIB_H_
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "NasMmlCtx.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "MmcLmmInterface.h"
#include  "MmLmmInterface.h"
#endif



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define NAS_MML_SoftReBoot() NAS_MML_SoftReBoot_WithLineNoAndFileID( (__LINE__), (THIS_FILE_ID) )

#define NAS_MML_MIN(n1,n2)                                  (((n1) > (n2)) ? (n2):(n1))

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
VOS_UINT32  NAS_MML_CompareBcchPlmnwithSimPlmn(
   NAS_MML_PLMN_ID_STRU                *pstBccPlmnId,
   NAS_MML_PLMN_ID_STRU                *pstSimPlmnId
);
VOS_UINT32  NAS_MML_GetCsServiceExistFlg( VOS_VOID );


VOS_UINT32  NAS_MML_GetPsServiceExistFlg( VOS_VOID );


VOS_UINT32  NAS_MML_IsInForbidLaiList(
   NAS_MML_LAI_STRU                    *pstLai
);

VOS_UINT32 NAS_MML_IsMccInDestMccList (
    VOS_UINT32                          ulMcc,
    VOS_UINT32                          ulMccNum,
    VOS_UINT32                         *pulMccList
);


VOS_UINT32  NAS_MML_IsPlmnIdInForbidPlmnList(
   NAS_MML_PLMN_ID_STRU                *pstPlmnId
);

VOS_UINT32  NAS_MML_IsLaiInDestLaiList (
   NAS_MML_LAI_STRU                   *pstLai,
   VOS_UINT32                          ulLaiNum,
   NAS_MML_LAI_STRU                   *pstLaiList
);

VOS_UINT32 NAS_MML_GetLaiIndexInDestLaiList (
    NAS_MML_LAI_STRU                   *pstLai,
    VOS_UINT8                          *pucLaiIdx,
    VOS_UINT8                           ulLaiNum,
    NAS_MML_LAI_STRU                   *pstLaiList
);

VOS_UINT32 NAS_MML_ComparePlmnIdWithHplmn (
   NAS_MML_PLMN_ID_STRU                *pstPlmnId
);

VOS_UINT32 NAS_MML_ComparePlmnIdWithUplmn (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32 NAS_MML_ComparePlmnIdWithOplmn (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32  NAS_MML_IsNetRatSupported(
   NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enSpecRat
);

VOS_UINT32 NAS_MML_IsSpecRatInRatList(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enSpecRat,
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatList
);


VOS_UINT32 NAS_MML_IsSpec3Gpp2RatInRatList(
    NAS_MML_3GPP2_RAT_TYPE_ENUM_UINT8   enSpec3Gpp2Rat,
    NAS_MML_3GPP2_RAT_PRIO_STRU        *pst3Gpp2RatList
);

VOS_UINT32  NAS_MML_IsBcchPlmnIdInDestSimPlmnList (
   NAS_MML_PLMN_ID_STRU               *pstPlmnId,
   VOS_UINT8                           ucPlmnNum,
   NAS_MML_PLMN_ID_STRU               *pstPlmnIdList
);

VOS_UINT32 NAS_MML_IsSimPlmnIdInDestBcchPlmnList (
    NAS_MML_PLMN_ID_STRU               *pstSimPlmnId,
    VOS_UINT8                           ucBcchPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstBcchPlmnIdList
);



VOS_UINT32 NAS_MML_IsBcchPlmnIdInDestSimPlmnWithRatList (
    NAS_MML_PLMN_ID_STRU               *pstBcchPlmnId,
    VOS_UINT16                          usSimPlmnWithRatNum,
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstSimPlmnWithRatList
);
VOS_UINT32  NAS_MML_IsPlmnIdRoamingAllowed(
   NAS_MML_PLMN_ID_STRU                *pstPlmnId
);

VOS_UINT32  NAS_MML_IsPlmnIdValid(
   NAS_MML_PLMN_ID_STRU                *pstPlmnId
);

VOS_UINT32 NAS_MML_IsBcchPlmnIdInLockPlmnList (
    NAS_MML_PLMN_ID_STRU               *pstBcchPlmnId
);

VOS_UINT32 NAS_MML_IsMccValid(
    VOS_UINT32                          ulMcc
);
VOS_UINT32  NAS_MML_IsRrcConnExist( VOS_VOID );

VOS_UINT32  NAS_MML_IsTmsiValid(VOS_VOID);

VOS_UINT32  NAS_MML_IsPtmsiValid(VOS_VOID);

VOS_UINT32  NAS_MML_IsQuickStartFlg( VOS_VOID );


VOS_UINT32  NAS_MML_GetGsmForbidFlg( VOS_VOID );

VOS_VOID    NAS_MML_InitSimPlmnInfo(
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstPlmnInfo
);

VOS_VOID  NAS_MML_ResumeSimEhplmnList(
    VOS_UINT8                          *pucPlmnNum,
    NAS_MML_SIM_PLMN_WITH_RAT_STRU     *pstPlmnInfo
);
VOS_UINT32 NAS_MML_IsUsimStausValid(VOS_VOID);

VOS_UINT32 NAS_MML_GetRPlmn(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRat,
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 NAS_MML_GetLteRPlmn(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_VOID NAS_MML_UpdateEpsConnectionStatus(MMC_LMM_CONN_STATE_ENUM_UINT32 enEpsConnState);

VOS_UINT32 NAS_MML_IsTaiInDestTaiList (
    NAS_MML_TAI_STRU                   *pstTai,
    VOS_UINT32                          ulTaiNum,
    NAS_LMM_TAI_STRU                   *pstTaiList
);


VOS_UINT32 NAS_MML_IsSupportLteCapability(VOS_VOID);

#endif

VOS_UINT32 NAS_MML_GetGURPlmn(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32 NAS_MML_GetEfLociPlmn(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32 NAS_MML_GetPlmnIdxInList (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId,
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstPlmnIdList
);

VOS_UINT32 NAS_MML_DelInvalidPlmnFromList(
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pHandlingPlmnList
);

VOS_VOID NAS_MML_DelPlmnsInRefList(
    VOS_UINT8                          *pHandlingPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pHandlingPlmnList,
    VOS_UINT8                           ucRefPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pRefPlmnList
);

VOS_VOID NAS_MML_DelPlmnsNotInRefList(
    VOS_UINT8                          *pHandlingPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pHandlingPlmnList,
    VOS_UINT8                           ucRefPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pRefPlmnList
);

VOS_UINT32 NAS_MML_AddForbPlmn (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32 NAS_MML_DelForbPlmn (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_VOID NAS_MML_AddForbGprsPlmn(
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_VOID NAS_MML_DelForbGprsPlmn (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);

VOS_VOID NAS_MML_AddForbRoamLa (
    NAS_MML_LAI_STRU                                       *pstLai,
    NAS_MML_FORBIDPLMN_ROAMING_LAS_INFO_STRU               *pstForbRoamLaInfo
);


VOS_VOID NAS_MML_AddForbRegLa (
    NAS_MML_LAI_STRU                   *pstLai
);

VOS_UINT32 NAS_MML_DelForbLa (
    NAS_MML_LAI_STRU                   *pstLai
);

VOS_VOID NAS_MML_DelNotAllowRoamPlmnInList(
    VOS_UINT8                          *pucPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstPlmnList
);

VOS_UINT32 NAS_MML_DelForbPlmnInList(
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pHandlingPlmnList
);


VOS_UINT32 NAS_MML_GetLaiForbType(
    NAS_MML_LAI_STRU                   *pstLai
);

VOS_UINT32 NAS_MML_IsGsmOnlyMode(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatOrder
);

VOS_UINT32 NAS_MML_IsWcdmaPrioGsm(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatOrder
);


VOS_UINT32 NAS_MML_IsWcdmaOnlyMode(NAS_MML_PLMN_RAT_PRIO_STRU * pstRatOrder);

VOS_UINT32 NAS_MML_IsLteOnlyMode(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatOrder
);

VOS_VOID NAS_MML_RemoveRoamPlmnInSrchList(
    VOS_UINT8                          *pPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstPlmnList
);

VOS_UINT32 NAS_MMC_IsRoam(VOS_VOID);

VOS_UINT32 NAS_MML_IsCampPlmnInfoChanged(
    NAS_MML_CAMP_PLMN_INFO_STRU        *pOldCampInfo,
    NAS_MML_CAMP_PLMN_INFO_STRU        *pNewCampInfo
);

NAS_MML_RRC_SYS_SUBMODE_ENUM_UINT8 NAS_MML_ConvertGprsSupportIndToSysSubMode(
    VOS_UINT8                           ucGprsSupportInd
);

VOS_VOID NAS_MML_SecContext3GTO2G(
    VOS_UINT8                           *pucIk,
    VOS_UINT8                           *pucCk,
    VOS_UINT8                           *pucKc
);

VOS_VOID NAS_MML_SecContext2GTO3G(
    VOS_UINT8                           *pucIk,
    VOS_UINT8                           *pucCk,
    VOS_UINT8                           *pucKc
);
#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID  NAS_MML_ConvertNasPlmnToLMMFormat(
    MMC_LMM_PLMN_ID_STRU               *pLMMPlmn,
    NAS_MML_PLMN_ID_STRU               *pstGUNasPlmn
);

VOS_VOID NAS_MML_ConvertGuMsModeToLteUeOperationMode(
    NAS_MML_MS_MODE_ENUM_UINT8                              enMsMode,
    NAS_MML_LTE_UE_OPERATION_MODE_ENUM_UINT8               *penOpertaionMode
);

VOS_UINT32 NAS_MML_IsCsfbServiceStatusExist(VOS_VOID);

VOS_UINT32 NAS_MML_IsCsfbMoServiceStatusExist(VOS_VOID);

VOS_UINT32 NAS_MML_IsLteOnlyMode(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstRatOrder
);

NAS_MML_LTE_UE_OPERATION_MODE_ENUM_UINT8 NAS_MML_GetLteUeOperationMode( VOS_VOID);
#endif
VOS_VOID NAS_MML_SortSpecRatPrioHighest(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enSpecRat,
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstPrioRatList
);

VOS_VOID NAS_MML_SortSpecRatPrioLowest(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enSpecRat,
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstPrioRatList
);


VOS_UINT32 NAS_MML_ComparePlmnMcc(
    VOS_UINT32                          ulSrcMcc,
    VOS_UINT32                          ulDestMcc
);


VOS_UINT32 NAS_MML_IsPlmnIdForbiddenByWhiteBlackList (
    NAS_MML_PLMN_ID_STRU               *pstPlmnId
);


VOS_UINT32 NAS_MML_IsEquPlmnInfoChanged(
    NAS_MML_EQUPLMN_INFO_STRU          *pstSrcEquPlmnInfo
);

VOS_VOID NAS_MML_SoftReBoot_WithLineNoAndFileID(
    VOS_UINT32                          ulLineNO,
    VOS_UINT32                          ulFileID
);




VOS_VOID NAS_MML_GetMsNetworkCapability(
    NAS_MML_MS_NETWORK_CAPACILITY_STRU  *pstMsNetworkCapbility
);


VOS_UINT8 NAS_MML_IsRaiChanged(VOS_VOID);

#if defined (__PS_WIN32_RECUR__)
VOS_UINT32 NAS_MML_UnCompressData(
    VOS_UINT8                          *pucDest,
    VOS_UINT32                         *pulDestLen,
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                          ulSrcLen
);
#endif
VOS_UINT32	NAS_MML_CompressData(
    VOS_UINT8                          *pucDest,
    VOS_UINT32                         *pulDestLen,
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                          ulSrcLen
);



VOS_UINT32 NAS_MML_IsNetworkRegFailCause(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16 usRegFailCause
);

VOS_UINT32 NAS_MML_IsRegFailCauseNotifyLmm(
    NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16  usRegFailCause
);


VOS_UINT32 NAS_MML_IsSndOmPcRecurMsgValid(VOS_VOID);


VOS_VOID NAS_MML_DelEqualPlmnsInForbList(
    VOS_UINT8                          *pEPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pEPlmnList,
    VOS_UINT8                           ucRefPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pRefPlmnList
);

VOS_UINT32 NAS_MML_IsPsBearerExist(VOS_VOID);
VOS_UINT32 NAS_MML_IsPsBearerExistBeforeIsrAct(VOS_UINT8 ucRabId);
VOS_UINT32 NAS_MML_IsPsBearerAfterIsrActExist(VOS_VOID);
VOS_VOID NAS_MML_UpdateAllPsBearIsrFlg(
    NAS_MML_PS_BEARER_ISR_ENUM_UINT8    enPsBearerIsrFlg
);

#if  (FEATURE_ON == FEATURE_LTE)
VOS_VOID NAS_MML_ConvertMmlCsfbStatusToLmmCsfbStatus(
    NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8                  enMmlCsfbStatus,
    MM_LMM_CSFB_SERVICE_TYPE_ENUM_UINT32                   *penLmmCsfbStatus
);

VOS_UINT32 NAS_MML_IsLteCapabilityDisabled(
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLteCapStatus,
    VOS_UINT32                                              ulDisableLteRoamFlg
);

#endif

VOS_UINT8 NAS_MML_IsCsLaiChanged(VOS_VOID);

VOS_VOID NAS_MML_DelEqualPlmnsInForbList(
    VOS_UINT8                          *pEPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pEPlmnList,
    VOS_UINT8                           ucRefPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pRefPlmnList
);
VOS_UINT8 NAS_MML_CompareLai(
    NAS_MML_LAI_STRU                    *pstCurLai,
    NAS_MML_LAI_STRU                    *pstOldLai
);

VOS_UINT32 NAS_MML_ExactlyCompareBcchPlmnwithSimPlmn(
    NAS_MML_PLMN_ID_STRU                *pstBccPlmnId,
    NAS_MML_PLMN_ID_STRU                *pstSimPlmnId
);

VOS_UINT32 NAS_MML_IsExistBufferedEmgCall(VOS_VOID);


VOS_VOID NAS_MML_GetValidUserCfgEhplmnInfo(
    VOS_UINT8                          *pucImsi,
    VOS_UINT8                          *pucEhplmnListNum,
    NAS_MML_PLMN_ID_STRU               *pstEhPlmnList
);

VOS_UINT32  NAS_MML_GetRrcSimStatus(VOS_VOID);

VOS_UINT32 NAS_MML_IsBcchPlmnIdWithRatInDestPlmnWithRatList (
    NAS_MML_PLMN_WITH_RAT_STRU         *pstSrcPlmnId,
    VOS_UINT32                          ulDestPlmnNum,
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDestPlmnIdList
);

VOS_UINT32 NAS_MML_IsRatInForbiddenList(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enRatType
);

VOS_UINT32 NAS_MML_IsAllMsRatInForbiddenList (VOS_VOID);

VOS_UINT32 NAS_MML_IsImsiInForbiddenList (VOS_VOID);


#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 NAS_MML_IsPlatformSupportLte(VOS_VOID);
#endif
VOS_UINT32 NAS_MML_IsPlatformSupportUtranFdd (VOS_VOID);
#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_UINT32 NAS_MML_IsPlatformSupportUtranTdd (VOS_VOID);
#endif
VOS_UINT32 NAS_MML_IsPlatformSupportGsm (VOS_VOID);
VOS_UINT32 NAS_MML_IsPlatformSupportUtran (VOS_VOID);


VOS_UINT32 NAS_MML_IsAcInfoChanged(
    NAS_MML_ACCESS_RESTRICTION_STRU    *pstOldAcRestrictInfo,
    NAS_MML_ACCESS_RESTRICTION_STRU    *pstNewAcRestrictInfo
);

VOS_VOID NAS_MML_Update_Revision_Level(
    VOS_UINT8                          *pucClassMark
);


VOS_VOID NAS_MML_GetSupportBandNum(
    VOS_UINT32                          ulBand,
    VOS_UINT8                          *pucBandNum
);

VOS_VOID NAS_MML_Fill_IE_ClassMark2(
    VOS_UINT8                          *pClassMark2
);


VOS_UINT32  NAS_MML_IsCsServDomainAvail(
    NAS_MML_MS_MODE_ENUM_UINT8          enMsMode
);


VOS_VOID NAS_MML_ConvertOamSendPid(
    VOS_UINT32                         *pulConvertedSendPid
);


#if (FEATURE_ON == FEATURE_PTM)
VOS_UINT32 NAS_MML_IsErrLogNeedRecord(VOS_UINT16 usLevel);
#endif


VOS_UINT32 NAS_MML_DecodeEmergencyNumList(
    VOS_UINT16                         *pusIndex,
    VOS_UINT8                          *pucRcvMsg,
    VOS_UINT16                          usMsgSize
);

VOS_UINT32 NAS_MML_IsNvimOplmnAvail(VOS_VOID);

VOS_VOID NAS_MML_Fill_IE_TddClassMark3(
    VOS_UINT8                          *pTddClassMark3
);

VOS_VOID NAS_MML_Fill_IE_FddClassMark3(
    VOS_UINT8                          *pFddClassMark3
);


VOS_VOID NAS_MML_DelEqualPlmnsInDisableRatList(
    VOS_UINT8                          *pucEPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstEPlmnList,
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enSpecRat,
    VOS_UINT32                          ulDisabledRatPlmnNum,
    NAS_MML_PLMN_WITH_RAT_STRU         *pstDisabledRatPlmnIdList
);
VOS_VOID NAS_MML_DelEqualPlmnNotInAllowLteRoamMccList(
    VOS_UINT8                          *pucEPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstEPlmnList,
    VOS_UINT32                          ulAllowLteRoamMccNum,
    VOS_UINT32                         *pulAllowLteRoamMccList
);
VOS_VOID NAS_MMC_DelDuplicatedPlmnInPlmnList(
    VOS_UINT8                          *pucPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstPlmnList
);
VOS_VOID NAS_MML_DelForbPlmnInPlmnList(
    VOS_UINT8                          *pucPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstPlmnList
);


VOS_UINT32 NAS_MML_GetCsOnlyDataServiceSupportFlg(VOS_VOID);

VOS_UINT32 NAS_MML_IsPlmnListSrchRslt(
    VOS_UINT32     enPlmnSrchRslt
);

VOS_UINT32  NAS_MML_IsCurrentRatHighestRat(VOS_VOID);

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

#endif /* end of NasMmlLib.h */
