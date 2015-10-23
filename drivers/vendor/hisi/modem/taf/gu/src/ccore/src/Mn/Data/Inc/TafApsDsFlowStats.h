

#ifndef __TAFAPSDSFLOWSTATS_H__
#define __TAFAPSDSFLOWSTATS_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "Ps.h"
#include "TafApsApi.h"
#include "ApsCdsInterface.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 64位长整型运算，加法 : rst = argA + argB */
#define TAF_APS_BIT64_ADD(rstHi, rstLo, argAHi, argALo, argBHi, argBLo) \
    { \
        if ((argALo) > 0xFFFFFFFF - (argBLo)) \
        { \
            (rstHi) = (argAHi) + ((argBHi) + 1); \
        } \
        else \
        { \
            (rstHi) = (argAHi) + (argBHi); \
        } \
        (rstLo) = (argALo) + (argBLo); \
    }


/* 64位长整型运算，减法 : rst = argA - argB */
/* Notes: we assumed argA always great than or equal to argB */
#define TAF_APS_BIT64_SUB(rstHi, rstLo, argAHi, argALo, argBHi, argBLo) \
    { \
        if ((argALo) < (argBLo)) \
        { \
            (rstHi) = (argAHi) - ((argBHi) + 1); \
        } \
        else \
        { \
            (rstHi) = (argAHi) - (argBHi); \
        } \
        (rstLo) = (argALo) - (argBLo); \
    }

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

extern VOS_UINT32   VOS_GetTick(VOS_VOID);


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID  TAF_APS_StartDsFlowStats(
    VOS_UINT8                           ucRabId
);

VOS_VOID  TAF_APS_StopDsFlowStats(
    VOS_UINT8                           ucRabId
);

VOS_VOID  TAF_APS_QryDsFlowStats(
    TAF_DSFLOW_QUERY_INFO_STRU         *pstDsFlowQryInfo,
    VOS_UINT8                           ucRabId
);

VOS_VOID  TAF_APS_QryAllRabDsFlowStats(
    TAF_DSFLOW_QUERY_INFO_STRU         *pstTotalDsFlowQryInfo
);

VOS_VOID  TAF_APS_ClearDsFlowStats(
    VOS_UINT8                           ucRabId
);

VOS_VOID  TAF_APS_ClearAllRabDsFlowStats( VOS_VOID );

VOS_VOID  TAF_APS_StartDsFlowRpt( VOS_VOID );

VOS_VOID  TAF_APS_StopDsFlowRpt( VOS_VOID );

VOS_VOID TAF_APS_QryDsFlowReportInfo(
    VOS_UINT8                           ucRabId,
    TAF_DSFLOW_REPORT_STRU             *pstDsFlowRptInfo
);

VOS_VOID TAF_APS_QryAllRabDsFlowReportInfo(
    TAF_DSFLOW_REPORT_STRU             *pstTotalDsFlowRptInfo
);
VOS_VOID  TAF_APS_GetCurrentFlowInfo(
    VOS_UINT8                           ucRabId,
    TAF_DSFLOW_INFO_STRU               *pstCurrentFlowInfo
);

VOS_VOID  TAF_APS_UpdateAllRabCurrentFlowInfo( VOS_VOID );

VOS_UINT32  TAF_APS_CheckIfActiveRabIdExist( VOS_VOID );

VOS_UINT32  TAF_APS_CalcDsflowRate(
    VOS_UINT32                          ulStartHigh,
    VOS_UINT32                          ulStartLow,
    VOS_UINT32                          ulEndHigh,
    VOS_UINT32                          ulEndLow,
    VOS_UINT32                          ulPeriod,
    VOS_UINT32                         *pulRate
);

VOS_VOID TAF_APS_ReleaseDfs(VOS_VOID);
VOS_VOID TAF_APS_SwitchDdrRateByCurrentRate(
    VOS_UINT32                          ulDlRate,
    VOS_UINT32                          ulUlRate
);
VOS_UINT32 TAF_APS_FindDfsReqValue(
    VOS_UINT32                         *pulReqValue,
    VOS_UINT32                          ulDLRate,
    VOS_UINT32                          ulULRate
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

#endif /* end of TafApsDsFlowStats.h */
