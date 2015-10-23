

#ifndef __IMSAPROCMMAMSG_H__
#define __IMSAPROCMMAMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "ImsaMmaInterface.h"

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


extern VOS_VOID IMSA_ProcMmaMsg(const VOS_VOID *pRcvMsg );

extern VOS_VOID IMSA_ProcMmaMsgStartReq(VOS_VOID  );

extern VOS_VOID IMSA_SndMmaMsgStartCnf(VOS_VOID  );

extern VOS_VOID IMSA_ProcMmaMsgStopReq(VOS_VOID  );

extern VOS_VOID IMSA_SndMmaMsgStopCnf(VOS_VOID  );

extern VOS_VOID IMSA_ProcMmaMsgDeregReq(VOS_VOID );

extern VOS_VOID IMSA_SndMmaMsgDeregCnf(VOS_VOID );

extern VOS_VOID IMSA_ProcMmaMsgServiceChangeInd(const VOS_VOID *pRcvMsg  );
extern VOS_VOID IMSA_ProcMmaMsgCampInfoChangeInd
(
    const VOS_VOID                     *pRcvMsg
);
extern VOS_VOID IMSA_ProcImsMsgStartOrStopCnf(VOS_VOID);
extern VOS_VOID IMSA_SndMmaMsgImsVoiceCapNotify(MMA_IMSA_IMS_VOICE_CAP_ENUM_UINT8 enImsVoiceCap);

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

#endif /* end of ImsaProcMmaMsg.h */




