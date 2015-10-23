

#ifndef __NASEMMLPPGPROC_H__
#define __NASEMMLPPGPROC_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
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

#include "NasEmmPubUCnMsgDecode.h"

/*****************************************************************************
  2 Macro
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
typedef struct
{
    VOS_UINT32                          bitOpAdditionalInfo :1;
    VOS_UINT32                          bitRsv              :31;

    VOS_UINT8                           ucContainerType;
    VOS_UINT8                           aucRsv[3];
    NAS_EMM_GENERIC_MESSAGE_CONTAINER_STRU stContainer;
    NAS_EMM_ADDITIONAL_INFOR_STRU       stAdditionalInfo;
} NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID NAS_EMM_CompUplinkGenericNasTransportMsg(VOS_UINT8 *pMsgBuf,
                                                         VOS_UINT32 *pulIndex,
                                                         const NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT_STRU *pMsgStruct);

extern VOS_VOID NAS_EMM_SendMrrcDataReq_UplinkGenericNasTransport(const NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT_STRU *pstUplinkMsg);
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


#endif /* end of NasEmmLppMsgProc.h */
