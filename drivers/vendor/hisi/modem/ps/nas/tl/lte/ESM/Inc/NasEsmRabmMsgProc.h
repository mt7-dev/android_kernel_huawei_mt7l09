

#ifndef _NASESMRABMMSGPROC_H
#define _NASESMRABMMSGPROC_H

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
extern VOS_VOID NAS_ESM_CopyRabmTftInfo( ESM_ERABM_TFT_PF_STRU* pstRabmTftInfo,
                                                const NAS_ESM_CONTEXT_TFT_STRU* pstCntxtTftInfo,
                                                VOS_UINT32 ulSdfPfNum);
extern VOS_VOID NAS_ESM_RabmMsgDistr( VOS_VOID *pRcvMsg );
extern VOS_VOID NAS_ESM_SndEsmRabmActIndMsg( VOS_UINT32          ulEpsbId );
extern VOS_VOID NAS_ESM_SndEsmRabmDeactAllMsg( VOS_VOID );
extern VOS_VOID NAS_ESM_SndEsmRabmDeactIndMsg(VOS_UINT32          ulEpsbIdNum,
                                                         const VOS_UINT32         *pulEpsbId);
extern VOS_VOID NAS_ESM_SndEsmRabmMdfIndMsg( VOS_UINT32          ulEpsbId );
extern VOS_VOID NAS_ESM_SndEsmRabmRelIndMsg( VOS_VOID );

extern VOS_VOID  NAS_ESM_RcvEsmRabmBearerStatusReq(const ESM_ERABM_BEARER_STATUS_REQ_STRU *pRcvMsg );
extern VOS_VOID  NAS_ESM_GetPdnAddr(NAS_ESM_CONTEXT_IP_ADDR_STRU *pstPdnAddr,VOS_UINT32 ulEpsbId);
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

#endif /* end of NasEsmRabmMsgProc.h*/
