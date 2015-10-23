

#ifndef __NASEMMCEMMINTERFACE_H__
#define __NASEMMCEMMINTERFACE_H__


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "NasLmmPublic.h"


#include  "NasLmmPubMIntraMsg.h"
#include  "AppMmInterface.h"
#include  "NasEmmPubUGlobal.h"
#include  "NasEmmcMain.h"

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
  2 Macro
*****************************************************************************/

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
enum    EMMC_EMM_MSG_ID_ENUM
{
    /* EMM发给MMC的消息原语 */
    ID_EMMC_EMM_START_REQ                = ID_NAS_LMM_INTRA_EMMC_EMM_START_REQ,
    ID_EMMC_EMM_STOP_REQ                 = ID_NAS_LMM_INTRA_EMMC_EMM_STOP_REQ,


    /*ID_EMMC_EMM_ACTION_RESULT_REQ        = ID_NAS_LMM_INTRA_EMMC_EMM_ACTION_RESULT_REQ,*/
    ID_EMMC_EMM_USIM_STATUS_IND          = ID_NAS_LMM_INTRA_EMMC_EMM_USIM_STATUS_IND,

    /* MMC发给EMM的消息原语 */
    ID_EMMC_EMM_START_CNF                = ID_NAS_LMM_INTRA_EMMC_EMM_START_CNF,
    ID_EMMC_EMM_STOP_CNF                 = ID_NAS_LMM_INTRA_EMMC_EMM_STOP_CNF,
    ID_EMMC_EMM_PLMN_IND                 = ID_NAS_LMM_INTRA_EMMC_EMM_PLMN_IND,
    ID_EMMC_EMM_SYS_INFO_IND             = ID_NAS_LMM_INTRA_EMMC_EMM_SYS_INFO_IND,
    ID_EMMC_EMM_COVERAGE_LOST_IND        = ID_NAS_LMM_INTRA_EMMC_EMM_COVERAGE_LOST_IND,

    ID_EMMC_EMM_MSG_ID_ENUM_BUTT         = ID_NAS_LMM_INTRA_MSG_TYPE_BUTT
};
typedef VOS_UINT32 EMMC_EMM_MSG_ID_ENUM_UINT32;

enum    EMMC_EMM_FORBIDDEN_INFO_ENUM
{
    EMMC_EMM_NO_FORBIDDEN                = 0x0000,   /* 不在任何禁止列表中*/
    EMMC_EMM_FORBIDDEN_PLMN              = 0x0001,   /* 在禁止PLMN列表内*/
    EMMC_EMM_FORBIDDEN_PLMN_FOR_GPRS     = 0X0002,   /* 在禁止PLMN FOR GPRS列表中*/
    EMMC_EMM_FORBIDDEN_TA_FOR_ROAMING    = 0x0003,   /* 在禁止TA FOR ROAMING列表中*/
    EMMC_EMM_FORBIDDEN_TA_FOR_RPOS       = 0x0004,   /* 在禁止TA FOR RPOS列表中*/
    EMMC_EMM_FORBIDDEN_INFO_BUTT
};
typedef VOS_UINT32 EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32;


enum    EMMC_EMM_CHANGE_INFO_ENUM
{
    EMMC_EMM_NO_CHANGE   = 0x0000,    /* 任何信息都没有改变              */
    EMMC_EMM_CHANGE_PLMN  = 0x0001,   /* PLMN ID 改变         */
    EMMC_EMM_CHANGE_TA       = 0x0002,   /* TA 改变 */
    EMMC_EMM_CHANGE_CELL  = 0x0004,   /* CELL ID 改变     */

    EMMC_EMM_CHANGE_INFO_BUTT
};
typedef VOS_UINT32 EMMC_EMM_CHANGE_INFO_ENUM_UINT32;

/*****************************************************************************
 枚举名    : EMMC_EMM_CELL_STATUS_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 小区驻留状态,EMMC通知EMM当前驻留小区是AnyCell状态还是Normal状态
*****************************************************************************/
enum EMMC_EMM_CELL_STATUS_ENUM
{
    EMMC_EMM_CELL_STATUS_NORMAL          = 0,                                    /* 正常驻留    */
    EMMC_EMM_CELL_STATUS_ANYCELL         = 1,                                    /* AnyCell驻留 */
    EMMC_EMM_CELL_STATUS_BUTT
};
typedef VOS_UINT32 EMMC_EMM_CELL_STATUS_ENUM_UINT32;


#if 0
enum    EMMC_EMM_ACTION_TYPE_ENUM
{
    EMMC_EMM_ACTION_TAU                  = 0,        /* TAU    */
    EMMC_EMM_ACTION_ATTACH               = 1,        /* ATTACH */
    EMMC_EMM_ACTION_DETACH               = 2,        /* DETACH */
    EMMC_EMM_ACTION_SERVICE              = 3,        /* SERVICE */
    EMMC_EMM_ACTION_AUTHENTICATION       = 4,        /* AUTHENTICATION */
    EMMC_EMM_ACTION_NO_ACTION            = 5,        /* other*/
    EMMC_EMM_ACTION_TYPE_BUTT
};
typedef VOS_UINT32 EMMC_EMM_ACTION_TYPE_ENUM_UINT32;

enum    EMMC_EMM_ACTION_RESULT_ENUM
{
    EMMC_EMM_ACTION_SUCCESS              = 0,        /* 操作成功 */
    EMMC_EMM_ACTION_FAILURE              = 1,        /* 操作失败 */

    EMMC_EMM_ACTION_RESULT_BUTT
};
typedef VOS_UINT32 EMMC_EMM_ACTION_RESULT_ENUM_UINT32;

typedef NAS_MM_CN_CAUSE_ENUM_UINT8 EMMC_EMM_ACTION_CAUSE_ENUM_UINT8;
#endif

/* EMM指示MMC搜网的原因值 */
enum EMMC_EMM_PLMN_REQ_CAUSE_ENUM
{
    EMMC_EMM_PLMN_REQ_CAUSE_PS_START     = 0,        /* 协议栈启动需要发起搜网 */
    EMMC_EMM_PLMN_REQ_CAUSE_PLMN_SELECT  = 1,        /* 需要进行PLMN选择 */
    EMMC_EMM_PLMN_REQ_CAUSE_BUTT
};
typedef VOS_UINT32 EMMC_EMM_PLMN_REQ_CAUSE_ENUM_UINT32;


enum EMMC_EMM_START_REQ_CAUSE_ENUM
{
    EMMC_EMM_START_REQ_CAUSE_NORMAL_START    = 0,    /* 正常启动 */
    EMMC_EMM_START_REQ_CAUSE_NEED_RESET      = 1,    /* 需要复位 */
    EMMC_EMM_START_REQ_CAUSE_BUTT
};
typedef VOS_UINT32 EMMC_EMM_START_REQ_CAUSE_ENUM_UINT32;


#if 0
/*****************************************************************************
 枚举名: EMMC_EMM_DOMAIN_ENUM
 协议格式:
 ASN.1描述:
 枚举说明:24301  9.9.3.10 / 24008  10.5.5.1
*****************************************************************************/
enum    EMMC_EMM_DOMAIN_ENUM
{
    EMMC_EMM_PS_ONLY                     = 0x00000000,
    EMMC_EMM_IMSI_ONLY,
    EMMC_EMM_COMBINED_PS_IMSI ,

    EMMC_EMM_DOMAIN_BUTT
};
typedef VOS_UINT32 EMMC_EMM_DOMAIN_ENUM_UINT32;

/*****************************************************************************
 枚举名:
 协议格式:
 ASN.1描述:
 枚举说明:
*****************************************************************************/
enum    EMMC_EMM_MT_DETACH_TYPE_ENUM
{

    EMMC_EMM_MT_DETACH_REATTACH_REQ      = 0x00000000,
    EMMC_EMM_MT_DETACH_REATTACH_NOT_REQ,
    EMMC_EMM_MT_DETACH_IMSI_DETACH,
    EMMC_EMM_MT_DETACH_TYPE_BUTT
};
typedef VOS_UINT32 EMMC_EMM_MT_DETACH_TYPE_ENUM_UINT32;
#endif


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : MMC_EMM_START_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_START_REQ_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;
    EMMC_EMM_START_REQ_CAUSE_ENUM_UINT32 enCause;
} EMMC_EMM_START_REQ_STRU;

/*****************************************************************************
 结构名    : MMC_EMM_START_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_START_CNF_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

} EMMC_EMM_START_CNF_STRU;

/*****************************************************************************
 结构名    : MMC_EMM_STOP_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_STOP_REQ_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;
    VOS_UINT32                          ulReserved;

} EMMC_EMM_STOP_REQ_STRU;

/*****************************************************************************
 结构名    : MMC_EMM_STOP_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_STOP_CNF_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

} EMMC_EMM_STOP_CNF_STRU;


/*****************************************************************************
 结构名    : MMC_EMM_PLMN_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_PLMN_IND_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

} EMMC_EMM_PLMN_IND_STRU;

/*****************************************************************************
 结构名    : NAS_EMMC_EMM_SYS_INFO_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : NAS_EMMC_EMM_SYS_INFO_IND_STRU的数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

    EMMC_EMM_CHANGE_INFO_ENUM_UINT32    ulChangeInfo;
    NAS_MM_PLMN_ID_STRU                 stPlmnId;           /* PLMN ID            */
    NAS_MM_TAC_STRU                     stTac;              /* Tracking Area Code */
    VOS_UINT32                          ulCellId;           /* Cell Identity      */
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  ulForbiddenInfo;    /* 禁止信息 */
    EMMC_EMM_CELL_STATUS_ENUM_UINT32    ulCellStatus;
} EMMC_EMM_SYS_INFO_IND_STRU;

/*****************************************************************************
 结构名    : MMC_EMM_COVERAGE_LOST_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_COVERAGE_LOST_IND_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

} EMMC_EMM_COVERAGE_LOST_IND_STRU;
#if 0
/*****************************************************************************
 结构名    : MMC_EMM_ACTION_RESULT_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_ACTION_RESULT_REQ_STRU消息结构定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;

    EMMC_EMM_ACTION_TYPE_ENUM_UINT32     ulEmmActionType; /* EMM进行操作的类型 */

    EMMC_EMM_ACTION_RESULT_ENUM_UINT32   ulActionResult;  /* 操作结果          */

    EMMC_EMM_ACTION_CAUSE_ENUM_UINT8     ucCause;         /* 操作失败原因      */

    VOS_UINT8                           aucRsv[3];

    VOS_UINT32                          ulAttemptCount;  /* ATTACH或TAU失败尝试次数 */

    NAS_MM_PLMN_LIST_STRU               stEplmn;         /* 等效的PLMN        */

    /* 发起业务时的类型:仅ATTACH,TAU和DETACH有效*/
    EMMC_EMM_DOMAIN_ENUM_UINT32         ulReqDomain;

    /*结果域类型:仅ATTACH和TAU有效*/
    EMMC_EMM_DOMAIN_ENUM_UINT32         ulRstDomain;

    /* CN MT DETACH 类型 */
    EMMC_EMM_MT_DETACH_TYPE_ENUM_UINT32 ulMtDetachType;
} EMMC_EMM_ACTION_RESULT_REQ_STRU;
#endif
/*****************************************************************************
 结构名    : MMC_EMM_USIM_STATUS_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MMC_EMM_USIM_STATUS_IND_STRU消息结构定义
*****************************************************************************/
#if 0
typedef struct
{
    VOS_MSG_HEADER
    EMMC_EMM_MSG_ID_ENUM_UINT32          enMsgId;
    USIMM_CARD_STATUS_ENUM_UINT32       enCardStatus;   /* USIM状态
 */
}EMMC_EMM_USIM_STATUS_IND_STRU;
#endif
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_UINT32  NAS_EMMC_MsgDistr( MsgBlock *pMsg );

extern VOS_VOID  NAS_EMMC_UpdateForbTaList( VOS_VOID );
extern VOS_VOID  NAS_EMMC_SendEmmCoverageLostInd( VOS_VOID );
extern VOS_VOID  NAS_EMM_AppRegStatInd( NAS_APP_REG_STAT_ENUM_UINT32   ulNasAppState,
                                           NAS_LMM_MAIN_STATE_ENUM_UINT16  enMainState);

extern  VOS_UINT32 * NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PARA_ENUM_UINT32 ulGlobleParaName);
/*extern VOS_UINT32  NAS_EMM_GetRrcConnState(VOS_VOID);*/
extern VOS_VOID  NAS_EMMC_SendRrcCellSelectionReq
(
    LRRC_LNAS_RESEL_TYPE_ENUM_UINT32 ulReselType
);
extern VOS_VOID    NAS_LMM_StartStateTimer(
                           NAS_LMM_STATE_TI_ENUM_UINT16             enStateTimerId );
extern VOS_UINT32  NAS_EMM_lteRatIsExist(VOS_VOID);
extern VOS_UINT32  NAS_EMM_IsLteDisabled( VOS_VOID );
extern VOS_UINT32  NAS_EMMC_GetUserSpecPlmnFlag(VOS_VOID);
extern VOS_VOID  NAS_EMMC_SetUserSpecPlmnFlag(NAS_EMMC_USER_SPEC_PLMN_FLAG_UINT32  ulUserFlag);
extern VOS_VOID  NAS_EMMC_AddPlmnInRej18PlmnList(const NAS_MM_PLMN_ID_STRU *pstPlmnId);
extern VOS_UINT32 NAS_EMMC_GetRejCause18Flag(VOS_VOID);
extern VOS_VOID NAS_EMMC_SetRejCause18Flag
(
    NAS_EMM_REJ_FLAG_ENUM_UINT32 ulRejFlag
);

extern VOS_UINT32 NAS_EMMC_ProcTimerMsgWaitPlmnSrchcnfTimerExp(MsgBlock *pMsg);
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

#endif /* end of EmmcEmmInterface.h */
