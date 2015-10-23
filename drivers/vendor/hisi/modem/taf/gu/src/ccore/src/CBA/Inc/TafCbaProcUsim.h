

#ifndef __TAFCBAPROCUSIM_H__
#define __TAFCBAPROCUSIM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "TafCbaCtx.h"
#include  "TafCbsInterface.h"
#include  "MnMsgApi.h"
#include  "UsimPsInterface.h"
#include  "NasUsimmApi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))


/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define  TAF_CBA_CBMI_UNIT_LEN                              (2)
#define  TAF_CBA_CBMIR_UNIT_LEN                             (4)

#define  TAF_CBA_EF_MSGID_PADDING                           (0xffff)


#define  TAF_CBA_EF_CBMI_ID                                 (0x6F45)            /* EFCBMI  */
#define  TAF_CBA_EF_CBMIR_ID                                (0x6F50)            /* EFCBMIR */
#define  TAF_CBA_EF_CBMID_ID                                (0x6F48)            /* EFCBMID */


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

typedef struct
{
    VOS_UINT16                          usMsgIdFrom;                            /*Cell broadcast message id value range from  */
    VOS_UINT16                          usMsgIdTo;                              /*Cell broadcast message id value range to    */
}TAF_CBA_USIM_CBMIR_STRU;

typedef struct
{
    VOS_UINT8                          ucTafCbaCbmiFlag;
    VOS_UINT8                          ucTafCbaCbmirFlag;
    VOS_UINT8                          ucTafCbaCbmidFlag;
}TAF_CBA_FILE_NEED_REFRESH_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32 TAF_CBA_IsNeedActAsCbs(VOS_VOID);

extern VOS_VOID TAF_CBA_SndAsActiveCbsReq(
    TAF_CBS_CFG_ENUM_UINT32      enCfgFlg
);

VOS_VOID TAF_CBA_RcvUsimGetEfCbmiRsp(
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_CBA_RcvUsimGetEfCbmirRsp(
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_CBA_RcvUsimGetEfCbmidRsp(
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_CBA_ParseUsimFileEfCbmiData(
    VOS_UINT8                           ucEfCbmiExistFlg,
    VOS_UINT8                          *pucEf,
    VOS_UINT16                          usEfLen
);

VOS_VOID TAF_CBA_ParseUsimFileEfCbmirData(
    VOS_UINT8                           ucEfCbmirExistFlg,
    VOS_UINT8                          *pucEf,
    VOS_UINT16                          usEfLen
);

VOS_VOID TAF_CBA_ParseUsimFileEfCbmidData(
    VOS_UINT8                           ucEfCbmidExistFlg,
    VOS_UINT8                          *pucEf,
    VOS_UINT16                          usEfLen
);

VOS_UINT16 TAF_CBA_CountCbmiNum(
    VOS_UINT16                         *pusCbmi,
    VOS_UINT32                          ulLength
);

VOS_UINT16 TAF_CBA_CountCbmirNum(
    VOS_UINT16                         *pusCbmir,
    VOS_UINT32                          ulLength
);

VOS_VOID TAF_CBA_BuildUsimEfCbmiContent(
    VOS_UINT16                         *pusMsgId
);

VOS_VOID TAF_CBA_BuildUsimEfCbmirContent(
    TAF_CBA_USIM_CBMIR_STRU            *pstMsgRange
);

VOS_VOID TAF_CBA_UpdateUsimFileEfCbmiReq(VOS_VOID);

VOS_VOID TAF_CBA_UpdateUsimFileEfCbmirReq(VOS_VOID);

VOS_VOID  TAF_CBA_UpdateUsimFile(VOS_VOID);

VOS_UINT32  TAF_CBA_ProcUsimSetFileRsp(
    PS_USIM_SET_FILE_CNF_STRU           *pstSetFileRsp
);

VOS_VOID TAF_CBA_ReadSpecificFileListRefreshFile(
    VOS_UINT16                          usFileId,
    NAS_USIMM_SERVICES_TYPE_ENUM_UINT32 ulUsimmService,
    PS_USIM_REFRESH_IND_STRU           *pstRefreshInd
);


VOS_VOID TAF_CBA_ParseSpecUsimFileData (
    VOS_UINT16                          usFileId,
    VOS_UINT8                           ucFileExistFlg,
    VOS_UINT8                          *pucEf,
    VOS_UINT16                          ulEfLen
);



VOS_VOID TAF_CBA_SndUsimCbMsg(
    VOS_UINT32                          ulSendPID,
    MN_OPERATION_ID_T                   opId,
    MN_MSG_CBRAW_TS_DATA_STRU          *pstCbMsg
);

VOS_VOID TAF_CBA_ReadUsimFile(
    VOS_UINT16                          usFileId,
    NAS_USIMM_SERVICES_TYPE_ENUM_UINT32 ulUsimmService
);

VOS_VOID TAF_CBA_ApplyUsimInfo(VOS_VOID);

VOS_VOID TAF_CBA_ProcRefreshInd(PS_USIM_REFRESH_IND_STRU *pstRefreshInd);


#endif

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

#endif /* end of TafCbaProcUsim.h */
