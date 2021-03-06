
#ifndef _TAF_MMA_SND_IMSA_H_
#define _TAF_MMA_SND_IMSA_H_


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#if (FEATURE_IMS == FEATURE_ON)
#include "ImsaMmaInterface.h"
#endif
#include "MmaMmcInterface.h"
#include "TafSdcCtx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

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


#if (FEATURE_IMS == FEATURE_ON)
MMA_IMSA_SERVICE_STATUS_ENUM_UINT8 TAF_MMA_ConvertMmaPsServiceStatusToImsaFormat(
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32 enMmcPsServiceStatus
);

VOS_VOID TAF_MMA_SndImsaSrvInfoNotify(
    MMA_MMC_SERVICE_STATUS_ENUM_UINT32  enPsServiceStatus
);

VOS_VOID TAF_MMA_SndImsaCampInfoChangeInd(VOS_VOID);
VOS_VOID TAF_MMA_SndImsaStopReq(
    MMA_IMSA_STOP_TYPE_ENUM_UINT32      enStopType
);

VOS_UINT32 TAF_MMA_SndImsaStartReq(
    MMA_IMSA_START_TYPE_ENUM_UINT32     enStartType
);
VOS_VOID TAF_MMA_SndImsaVoiceDomainChangeInd(
    MMA_IMSA_VOICE_DOMAIN_ENUM_UINT32   enVoiceDomain
);
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_VOID TAF_MMA_SndImsaModem1InfoInd(
    MMA_IMSA_MODEM_POWER_STATE_ENUM_UINT8   enModem1PowerState);
#endif

#endif

VOS_UINT32 TAF_MMA_IsCGIInfoChanged(
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstOldCampInfo,
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstNewCampInfo
);

VOS_UINT32 TAF_MMA_IsNetworkCapInfoChanged(
    TAF_SDC_NETWORK_CAP_INFO_STRU       *pstNewNwCapInfo
);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif
#ifdef __cplusplus
}
#endif
/*****************************************************************************/

/*===========================================================================*/
#endif      /* __STATUS_H__*/

/***** End of the file *****/
