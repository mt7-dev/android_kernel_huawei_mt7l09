
#ifndef _TAF_SDC_LIB_H_
#define _TAF_SDC_LIB_H_
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "TafSdcCtx.h"

#include "TafOamInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define PRE_SECOND_TO_TICK                      (100)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
#define TAF_SDC_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_PTR()    (&g_aenTafSdcCurcRptCmdTable[0])
#define TAF_SDC_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_SIZE()   (sizeof(g_aenTafSdcCurcRptCmdTable)/sizeof(TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8))

#define TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_PTR()    (&g_aenTafSdcUnsolicitedRptCmdTable[0])
#define TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_SIZE()   (sizeof(g_aenTafSdcUnsolicitedRptCmdTable)/sizeof(TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8))

#define TAF_SDC_MIN(n1,n2)                                  (((n1) > (n2)) ? (n2):(n1))


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
extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID* pMsg);

VOS_UINT32  TAF_SDC_IsPlatformRatSupported(
    TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8                    enSpecRat
);

VOS_UINT32  TAF_SDC_IsPlatformSupportLte( VOS_VOID );

VOS_UINT32  TAF_SDC_IsPlatformSupportUtran( VOS_VOID );
VOS_UINT32  TAF_SDC_IsPlatformSupportUtranFdd( VOS_VOID );
#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_UINT32  TAF_SDC_IsPlatformSupportUtranTdd( VOS_VOID );
#endif

VOS_UINT32  TAF_SDC_IsPlatformSupportGsm( VOS_VOID );

VOS_UINT32 TAF_SDC_CheckRptCmdStatus(
    VOS_UINT8                                  *pRptCfg,
    TAF_SDC_CMD_RPT_CTRL_TYPE_ENUM_UINT8        enRptCtrlType,
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex
);
VOS_UINT32 TAF_SDC_UpdateRptCmdStatus(
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex,
    VOS_UINT32                                  ulRptCmdStatus
);

VOS_UINT32 TAF_SDC_ConvertOamSendPid(
    VOS_UINT32                          ulConvertedSendPid
);

VOS_UINT32 TAF_SDC_GetCsSrvExistFlg(VOS_VOID);



VOS_UINT32 TAF_SDC_GetImsSupportFlag(VOS_VOID);

#if (FEATURE_ON == FEATURE_PTM)
VOS_UINT32 TAF_SDC_IsErrLogNeedRecord(VOS_UINT16 usLevel);
#endif

VOS_UINT32  TAF_CalcTime(
                VOS_UINT32  ulFirTime,
                VOS_UINT32  ulSecTime,
                VOS_UINT32 *pulTotalTime
);

VOS_UINT32 TAF_SDC_IsUsimStausValid(VOS_VOID);


#if (FEATURE_ON == FEATURE_IMS)
VOS_UINT32 TAF_SDC_IsImsVoiceSupportedInUE(VOS_VOID);
VOS_UINT32 TAF_SDC_IsImsVoiceSupportedInNW(VOS_VOID);
#endif

VOS_VOID TAF_SDC_PlmnId2Bcd(
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId
);

VOS_UINT32 TAF_SDC_IsConfigCLInterWork(VOS_VOID);

VOS_UINT32 TAF_SDC_IsLteOnlyMode(
    TAF_SDC_RAT_PRIO_STRU              *pstRatOrder
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
