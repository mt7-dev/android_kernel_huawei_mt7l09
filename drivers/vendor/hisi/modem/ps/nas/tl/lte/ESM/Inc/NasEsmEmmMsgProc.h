

#ifndef _NASESMEMMMSGPROC_H
#define _NASESMEMMMSGPROC_H


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include    "vos.h"
#include    "NasEsmPublic.h"

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
#define ESM_MAX_SND_MSG_LEN              (256)               /*空口消息占用的最大空间*/
#define ESM_MIN_SND_MSG_LEN              (4)                 /*空口消息占用的最小空间*/
#define ESM_MAX_MSG_CONTAINER_CNTNTS_LEN (1024)              /*ESM message container contents最大长度*/


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/



/*****************************************************************************
  5 STRUCT
*****************************************************************************/


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID NAS_ESM_EmmMsgDistr( VOS_VOID *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEmmDetachMsg( VOS_VOID );
extern VOS_VOID NAS_ESM_RcvEsmEmmBearerStatusInd(const EMM_ESM_BEARER_STATUS_IND_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEsmEmmDataInd( EMM_ESM_DATA_IND_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEsmEmmEstCnf(const EMM_ESM_EST_CNF_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEsmEmmPdnConInd(const EMM_ESM_PDN_CON_IND_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEsmEmmStatusInd(const EMM_ESM_STATUS_IND_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_RcvEsmEmmDeactAllNonEmcBearerInd(VOS_VOID);
extern VOS_VOID NAS_ESM_SndEsmEmmBearerStatusReqMsg(
                    EMM_ESM_BEARER_CNTXT_MOD_ENUM_UINT32 enBearerCntxtChangeMod);
extern VOS_VOID  NAS_ESM_SndEsmEmmDataReqMsg( VOS_UINT32 ulOpId,
                                                VOS_UINT8  ulIsEmcPdnType,
                                                VOS_UINT32 ulLength,
                                                const VOS_UINT8 *pucSendMsg);
extern VOS_VOID NAS_ESM_SndEsmEmmEstReqMsg(VOS_UINT32 ulOpId,
                                             VOS_UINT8 ulIsEmcPdnType,
                                             VOS_UINT32 ulLength,
                                             const VOS_UINT8 *pucSendMsg);
extern VOS_VOID NAS_ESM_SndEsmEmmPdnConRspMsg( EMM_ESM_PDN_CON_RSLT_ENUM_UINT32        ulRst,
                                                                 VOS_UINT32 ulLength, const VOS_UINT8 *pucSendMsg);
extern VOS_VOID NAS_ESM_SndEsmEmmRelReqMsg( VOS_VOID );
extern VOS_VOID  NAS_ESM_ClearStateTable( VOS_UINT32 ulAppErrType, VOS_UINT32 ulKeepEmc );
extern VOS_VOID  NAS_ESM_SndEsmEmmPdnConSuccReqMsg
(
     VOS_UINT32                         ulLength,
     const VOS_UINT8                   *pucSendMsg
);
extern VOS_VOID  NAS_ESM_BufferDecodedNwMsg
(
    const EMM_ESM_INTRA_DATA_IND_STRU  *pstEmmEsmIntraDataIndMsg,
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause
);
extern VOS_VOID NAS_ESM_BufferedNwMsgProc();
extern VOS_VOID  NAS_ESM_RcvEsmEmmSuspendInd(const EMM_ESM_SUSPEND_IND_STRU *pRcvMsg );
extern VOS_VOID  NAS_ESM_RcvEsmEmmResumeInd(const EMM_ESM_RESUME_IND_STRU *pRcvMsg );
extern VOS_VOID NAS_ESM_SndEsmEmmSuspendRsp
(
    EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
);
extern VOS_VOID NAS_ESM_SndEsmEmmResumeRsp
(
    EMM_ESM_RSLT_TYPE_ENUM_UINT32       enRslt
);

extern VOS_VOID  NAS_ESM_ProcPfPrecedenceCollision
(
    const NAS_ESM_NW_MSG_STRU                *pstNwMsg
);
extern VOS_UINT32  NAS_ESM_IsNeedPreferrenceCollision
(
    const NAS_ESM_NW_MSG_STRU          *pstNwMsg
);
/*leili modify for isr begin*/
extern VOS_VOID NAS_ESM_SndEsmEmmBearerModifyReq( VOS_UINT32 ulEpsId);
/*leili modify for isr end*/
extern VOS_UINT32 NAS_ESM_GetRegistCid( VOS_VOID );
extern VOS_VOID NAS_ESM_ReestablishAttachBearer( VOS_VOID );

extern VOS_VOID NAS_ESM_SndEsmEmmClrEsmProcResNtyMsg(VOS_UINT32 ulOpId, VOS_UINT8 ucIsEmcPdnType);



/*****************************************************************************
  9 OTHERS
*****************************************************************************/


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasEsmEmmMsgProc.h*/
