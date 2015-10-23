

#ifndef __IMSAPROCIMSCALLMSG_H__
#define __IMSAPROCIMSCALLMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "ImsaImsEvent.h"

#include    "MnCallApi.h"
#include    "ImsaEntity.h"

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
extern VOS_VOID IMSA_CallClearLocalAlertInfo(VOS_VOID );

extern VOS_VOID IMSA_ImsMsgCallEvent(VOS_VOID *pCallEvent);

extern VOS_VOID IMSA_CallProcImsSupsrvInfo(const IMSA_IMS_CALL_SUPSRV_INFO_STRU  *pstSupsrvInfo);


/* xiongxianghui00253310 add for DTMF 20131205 begin */
extern VOS_VOID IMSA_CallDtmfInfoInit(VOS_VOID);

extern VOS_VOID IMSA_CallDeleteSavedDtmfInfo(VOS_UINT8 ucIndex);

extern VOS_VOID IMSA_CallClearDtmfInfo(TAF_CS_CAUSE_ENUM_UINT32 enCause);
extern VOS_VOID IMSA_CallSrvccSuccClearDtmfInfo(TAF_CS_CAUSE_ENUM_UINT32 enCause);

extern TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallSaveDtmfInfo(VOS_UINT8 ucCallId,
                                                             VOS_CHAR  cKey,
                                                             VOS_UINT16 usDuration,
                                                             VOS_UINT16 usClientId,
                                                             VOS_UINT8  ucSpmOpid);

/* xiongxianghui00253310 add for DTMF 20131205 end */
extern TAF_CS_CAUSE_ENUM_UINT32 IMSA_CallTransImsErr2SpmErr
(
    VOS_UINT16                          usImsErr
);
extern VOS_VOID IMSA_CallReleaseCallCommonProc
(
    IMSA_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_CS_CAUSE_ENUM_UINT32            ulResult
);


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

#endif /* end of ImsaProcImsCallMsg.h */




