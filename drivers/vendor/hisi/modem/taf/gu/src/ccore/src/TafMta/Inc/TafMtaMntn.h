

#ifndef __TAFMTAMMNTN_H__
#define __TAFMTAMMNTN_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "pslog.h"
#include "MtaRrcInterface.h"
#include "TafMtaPhy.h"



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

VOS_VOID TAF_MTA_RcvQryWrrAutotestReq(VOS_VOID *pMsg);
VOS_VOID TAF_MTA_RcvQryWrrCellInfoReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvQryWrrMeanrptReq( VOS_VOID  *pMsg );
VOS_VOID TAF_MTA_RcvSetWrrFreqlockReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvSetWrrRrcVersionReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvSetWrrCellsrhReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvQryWrrFreqlockReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvQryWrrRrcVersionReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvQryWrrCellsrhReq( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvWrrAutotestQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrCellInfoQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrMeanrptQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrFreqlockSetCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrVersionSetCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrCellsrhSetCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrFreqlockQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrVersionQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvWrrCellsrhQryCnf( VOS_VOID *pWrrCnfMsg );
VOS_VOID TAF_MTA_RcvTiReqAutotestQryExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqCellInfoQryExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqMeanrptQryExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqFreqlockSetExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqRrcVersionSetExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqCellsrhSetExpired( VOS_VOID * pMsg );
VOS_VOID TAF_MTA_RcvTiReqFreqlockQryExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqRrcVersionQryExpired( VOS_VOID *pMsg );
VOS_VOID TAF_MTA_RcvTiReqCellsrhQryExpired( VOS_VOID *pMsg );
VOS_UINT32 TAF_MTA_SndWrrQryAutotestReqMsg( MTA_WRR_AUTOTEST_PARA_STRU  *pAutotestQryPara );
VOS_UINT32 TAF_MTA_SndWrrQryCellinfoReqMsg(
    VOS_UINT32                          ulSetCellInfo
);
VOS_UINT32 TAF_MTA_SndWrrQryMeanrptReqMsg( VOS_VOID );
VOS_UINT32 TAF_MTA_SndWrrSetFreqlockReqMsg( MTA_WRR_FREQLOCK_CTRL_STRU *pFreqlockCtrlPara );
VOS_UINT32 TAF_MTA_SndWrrSetVersionReqMsg(
    VOS_UINT8                           ucRrcVersion
);
VOS_UINT32 TAF_MTA_SndWrrSetCellsrhReqMsg(
    VOS_UINT8                           ucCellSrh
);
VOS_UINT32 TAF_MTA_SndWrrQryFreqlockReqMsg(VOS_VOID);
VOS_UINT32 TAF_MTA_SndWrrQryRrcVersionReqMsg( VOS_VOID );
VOS_UINT32 TAF_MTA_SndWrrQryCellsrhReqMsg( VOS_VOID );

VOS_VOID TAF_MTA_RcvAtSetNCellMonitorReq(
    VOS_VOID                           *pMsg
);
VOS_VOID TAF_MTA_RcvAtQryNCellMonitorReq(
    VOS_VOID                           *pMsg
);
VOS_VOID TAF_MTA_RcvRrcSetNCellMonitorCnf(
    VOS_VOID                           *pMsg
);
VOS_VOID TAF_MTA_RcvRrcQryNCellMonitorCnf(
    VOS_VOID                           *pMsg
);
VOS_VOID TAF_MTA_RcvRrcNCellMonitorInd(
    VOS_VOID                           *pMsg
);
VOS_UINT32 TAF_MTA_SndGrrNCellMonitorSetReq(
    VOS_UINT8                         ucRptCmdStatus
);
VOS_UINT32 TAF_MTA_SndGrrNCellMonitorQryReq(VOS_VOID);

VOS_VOID TAF_MTA_RcvNasAreaLostInd(
    VOS_VOID                           *pMsg
);

VOS_VOID TAF_MTA_RcvTafAgentGetAntStateReq(
    VOS_VOID                           *pMsg
);

#if (FEATURE_ON == FEATURE_PTM)
VOS_VOID TAF_MTA_SndAcpuOmErrLogRptCnf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulBufUseLen
);
VOS_VOID TAF_MTA_RcvAcpuOmErrLogRptReq(
    VOS_VOID                           *pMsg
);
VOS_VOID TAF_MTA_RcvAcpuOmErrLogCtrlInd(
    VOS_VOID                           *pMsg
);
#endif

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_MTA_SndLrrcSetDpdtTestFlagNtfMsg(
    AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU  *pstSetDpdtFlagReq
);

VOS_UINT32 TAF_MTA_SndLrrcSetDpdtValueNtfMsg(
    AT_MTA_SET_DPDT_VALUE_REQ_STRU     *pstSetDpdtReq
);

VOS_UINT32 TAF_MTA_SndLrrcQryDpdtValueReqMsg(
    AT_MTA_QRY_DPDT_VALUE_REQ_STRU     *pstSetDpdtReq
);

VOS_VOID TAF_MTA_RcvLrrcDpdtValueQryCnf(VOS_VOID *pMsg);
#endif

VOS_VOID TAF_MTA_RcvAtSetJamDetectReq(
    VOS_VOID                           *pMsg
);

VOS_UINT32 TAF_MTA_SndGrrSetJamDetectReq(
    AT_MTA_SET_JAM_DETECT_REQ_STRU     *pstSetJdrReq
);

VOS_VOID TAF_MTA_RcvRrcJamDetectCnf(
    VOS_VOID                           *pMsg
);

VOS_VOID TAF_MTA_RcvRrcJamDetectInd(
    VOS_VOID                           *pMsg
);

VOS_VOID TAF_MTA_RcvTiWaitGrrSetJamDetectExpired(VOS_VOID *pMsg);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif


