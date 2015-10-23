

#ifndef __IMSAPROCSPMMSG_H__
#define __IMSAPROCSPMMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"


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

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
    枚举名    : IMSA_SRVCC_ABNORMAL_TYPE_ENUM
    枚举说明  : SRVCC异常类型
*****************************************************************************/
enum    IMSA_SRVCC_ABNORMAL_TYPE_ENUM
{
    IMSA_SRVCC_ABNORMAL_STOP_REQ        = 1,            /* SRVCC过程中关机 */
    IMSA_SRVCC_ABNORMAL_DEREG_REQ       = 2,            /* SRVCC过程中DETACH */
    IMSA_SRVCC_ABNORMAL_STATUS_CHANGE   = 3,            /* SRVCC过程中状态迁离CONN+REG */
    IMSA_SRVCC_ABNORMAL_BUTT
};
typedef VOS_UINT8 IMSA_SRVCC_ABNORMAL_TYPE_ENUM_UINT32;




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
extern VOS_UINT32 IMSA_CallProcSpmMsgGetClprInfo(const SPM_IMSA_CALL_GET_CLPR_REQ_STRU  *pstAppMsg);

extern VOS_UINT32 IMSA_CallSendSpmStartDtmfCnfMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
);
extern VOS_UINT32 IMSA_CallSendSpmStopDtmfCnfMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
);
extern VOS_UINT32 IMSA_CallProcSpmMsgOrig(SPM_IMSA_CALL_ORIG_REQ_STRU *pstAppMsg);
extern VOS_UINT32 IMSA_CallProcSpmMsgSups(const SPM_IMSA_CALL_SUPS_CMD_REQ_STRU *pstAppMsg);
extern VOS_VOID IMSA_CallProcSpmMsgStartDtmf(const SPM_IMSA_CALL_START_DTMF_REQ_STRU *pstAppMsg);
extern VOS_VOID IMSA_CallProcSpmMsgStopDtmf(const SPM_IMSA_CALL_STOP_DTMF_REQ_STRU *pstAppMsg);
extern VOS_UINT32 IMSA_CallProcSpmMsgGetInfo(const SPM_IMSA_CALL_GET_CALL_INFO_REQ_STRU *pstAppMsg);
extern VOS_VOID   IMSA_ProcSpmCallMsg(const VOS_VOID *pRcvMsg);
extern VOS_UINT32 IMSA_CallSendSpmOrigCnfMsg(VOS_UINT16 usClientId,
                                             VOS_UINT32 ulOpId,
                                             VOS_UINT32 ulCallId,
                                             TAF_CS_CAUSE_ENUM_UINT32 ulResult);
extern VOS_UINT32 IMSA_CallSendSpmSupsCnfMsg(VOS_UINT16 usClientId,
                                             VOS_UINT32 ulOpId,
                                             VOS_UINT32 ulCallId,
                                             TAF_CS_CAUSE_ENUM_UINT32 ulResult);

extern VOS_VOID IMSA_USSD_SndImsUssdReqMsg
(
    IMSA_IMS_USSD_ENCTYPE_ENUM_UINT8   encType,
    VOS_UINT16   usLength,
    const VOS_UINT8   *pucMessage
);
extern VOS_VOID IMSA_USSD_SndImsUssdDisconnectMsg
(
    VOS_VOID
);
extern VOS_VOID IMSA_USSD_Init(VOS_VOID);
extern VOS_VOID IMSA_SMS_ProcTimerMsgWaitNetRsp(const VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_SMS_ProcTimerMsgWaitAppRsp(const VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_SrvccFailBuffProc( VOS_VOID );
extern VOS_VOID IMSA_SrvccSuccBuffProc( VOS_VOID );
extern VOS_VOID IMSA_SrvccAbormalClearBuff
(
    IMSA_SRVCC_ABNORMAL_TYPE_ENUM_UINT32    enAbnormalType
);

/*xiongxianghui00253310 add for conference 20140210 begin */
extern VOS_UINT32 IMSA_CallProcSpmMsgInviteNewPtpt(const SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ_STRU *pstAppMsg);
extern VOS_UINT32 IMSA_CallSendSpmInviteNewPtptCnfMsg
(
    VOS_UINT16                          usClientId,
    VOS_UINT32                          ulOpId,
    VOS_UINT32                          ulCallId,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
);

/*xiongxianghui00253310 add for conference 20140210 end */


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

#endif /* end of ImsaProcSpmMsg.h */




