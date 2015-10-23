

#ifndef __CMMCAMNTN_H__
#define __CMMCAMNTN_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "CmmcaCmdComm.h"


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

enum CMMCA_MNTN_MSG_ID_ENUM
{
    CMMCA_MNTN_BUFF_FULL_IND                                , /*_H2ASN_MsgChoice CMMCA_MNTN_BUFF_FULL_IND_STRU */
    CMMCA_MNTN_MSG_ID_BUT
};
typedef VOS_UINT32 CMMCA_MNTN_MSG_ID_ENUM_UINT32;


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

typedef struct
{
    VOS_MSG_HEADER                                                              /* 消息头 */    /* _H2ASN_Skip */
    CMMCA_MNTN_MSG_ID_ENUM_UINT32       enMsgId;                                /* 消息ID */    /* _H2ASN_Skip */
}CMMCA_MNTN_BUFF_FULL_IND_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_VOID CMMCA_Debug_SetParseDbg(VOS_UINT8 ucParseDbg);

VOS_VOID CMMCA_Debug_PrintPdnId(VOS_UINT8 ucPdnId);

VOS_VOID CMMCA_Debug_PrintPdnType(VOS_UINT8 ucPdnType);

VOS_VOID CMMCA_Debug_PrintApn(
    VOS_UINT8                          *pucApn,
    VOS_UINT16                          usApnLen
);

VOS_VOID CMMCA_Debug_PrintRatId(
    CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16    enCmdId,
    CMMCA_MMC_RAT_ID_ENUM_UINT8         enRatId
);

VOS_VOID CMMCA_Debug_PrintDataIndLenErr(
    CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16    enCmdId,
    VOS_UINT32                          ulDataLen
);

VOS_VOID  CMMCA_LogBuffFullInd(VOS_VOID);

extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID *pMsg);


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

#endif /* end of CmmcaMntn.h */
