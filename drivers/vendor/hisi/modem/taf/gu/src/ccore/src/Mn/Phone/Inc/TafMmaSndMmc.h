
#ifndef _TAF_MMA_SND_MMC_H_
#define _TAF_MMA_SND_MMC_H_
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "MmaMmcInterface.h"

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
VOS_UINT32 TAF_MMA_SndMmcStartReq(
    MMA_MMC_CARD_STATUS_ENUM_UINT8      ucCardStatus,
    MMA_MMC_PLMN_RAT_PRIO_STRU         *pstPlmnRatPrio
);

VOS_UINT32 TAF_MMA_SndMmcSignalReportReq(
    VOS_UINT8                           ucActionType,
    VOS_UINT8                           ucRrcMsgType,
    VOS_UINT8                           ucSignThreshold,
    VOS_UINT8                           ucMinRptTimerInterval
);

VOS_UINT32 TAF_MMA_SndMmcModeChangeReq(
    MMA_MMC_MS_MODE_ENUM_UINT32         enMsMode
);

VOS_UINT32 TAF_MMA_SndMmcAttachReq(
    VOS_UINT32                                              ulOpID,
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32                         enAttachType,
    TAF_MMA_EPS_ATTACH_REASON_ENUM_UINT8                    enAttachReason
);

VOS_UINT32 TAF_MMA_SndMmcDetachReq(
    VOS_UINT32                                 ulOpID,
    MMA_MMC_DETACH_TYPE_ENUM_UINT32            enDetachType,
    TAF_MMA_DETACH_CAUSE_ENUM_UINT8            enDetachCause
);

VOS_UINT32 TAF_MMA_SndMmcPlmnListReq(VOS_VOID);
VOS_UINT32 TAF_MMA_SndMmcPlmnListAbortReq(VOS_VOID);
VOS_UINT32 TAF_MMA_SndMmcPlmnUserReselReq(MMA_MMC_PLMN_SEL_MODE_ENUM_UINT32 enPlmnSelMode);

VOS_UINT32 TAF_MMA_SndMmcPlmnSpecialReq(
    MMA_MMC_PLMN_ID_STRU               *pstPlmnId,
    MMA_MMC_NET_RAT_TYPE_ENUM_UINT8     enAccessMode
);

VOS_UINT32 TAF_MMA_SndMmcPowerOffReq(
    MMA_MMC_POWER_OFF_CAUSE_ENUM_UINT32 enCause
);

VOS_UINT32  TAF_MMA_SndMmcSysCfgReq(
    TAF_MMA_SYS_CFG_PARA_STRU          *pSysCfgReq,
    VOS_UINT16                          usSetFlg
);

VOS_UINT32 TAF_MMA_SndMmcNetScanMsgReq(
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanReq
);

VOS_UINT32 TAF_MMA_SndMmcAbortNetScanMsgReq(VOS_VOID);





VOS_UINT32 TAF_MMA_SndMmcPlmnSearchReq(VOS_VOID);

VOS_UINT32 TAF_MMA_SndMmcSpecPlmnSearchAbortReq(VOS_VOID);

VOS_UINT32 TAF_MMA_SndMmcOmMaintainInfoInd(
    VOS_UINT8                           ucOmConnectFlg,
    VOS_UINT8                           ucOmPcRecurEnableFlg
);

VOS_UINT32 TAF_MMA_SndMmcUpdateUplmnNtf( VOS_VOID );

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_UINT32 TAF_MMA_SndMmcOtherModemInfoNotify(
    struct MsgCB                       *pstMsg
);

VOS_UINT32 TAF_MMA_SndMmcNcellInfoInd(
    struct MsgCB                       *pstMsg
);

VOS_UINT32 TAF_MMA_SndMmcPsTransferInd(
    struct MsgCB                       *pstMsg
);


#endif

VOS_VOID TAF_MMA_SndMmcEOPlmnSetReq(
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnSetPara
);

VOS_VOID TAF_MMA_SndMmcImsVoiceCapInd(
    VOS_UINT8                           ucImsVoiceAvail
);

VOS_UINT32 TAF_MMA_SndMmcAcqReq(
    TAF_MMA_ACQ_PARA_STRU              *pstMmaAcqPara
);

VOS_UINT32 TAF_MMA_SndMmcRegReq(
    TAF_MMA_REG_PARA_STRU              *pstMmaRegPara
);

VOS_UINT32 TAF_MMA_SndMmcPowerSaveReq(
    TAF_MMA_POWER_SAVE_PARA_STRU       *pstMmaPowerSavePara
);

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

#endif /* end of TafSdcLib.h */
