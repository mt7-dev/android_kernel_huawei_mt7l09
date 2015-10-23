

#ifndef __IMSAREGMANAGEMENT_H__
#define __IMSAREGMANAGEMENT_H__

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
#define IMSA_REG_FAILURE_RETRY_INTERVEL_5_MIN   (5*60)

#define IMSA_REG_GetRegCntxt()          (&pgstImsaEntity->stImsaRegManager)
#define IMSA_REG_GetNormalRegEntity()   (&pgstImsaEntity->stImsaRegManager.stNormalRegEntity)
#define IMSA_REG_GetEmcRegEntity()      (&pgstImsaEntity->stImsaRegManager.stEmcRegEntity)

#ifdef PS_ITT_PC_TEST_NAS
#define IMSA_REG_TIMER_LEN_PROTECT              (8*1000)
#define IMSA_REG_TIMER_LEN_RETRY                (6*1000)
#else
#define IMSA_REG_TIMER_LEN_PROTECT              (30*1000)
#define IMSA_REG_TIMER_LEN_RETRY                (5*60*1000)
#endif

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
    枚举名    : IMSA_REG_EVT_ENUM
    枚举说明  : 将外部调用和下层通知结果/事件统一编址为事件，用于内部状态机的处理
*****************************************************************************/
enum IMSA_REG_EVT_ENUM
{
    IMSA_REG_EVT_USER_REG_REQ,
    IMSA_REG_EVT_USER_DEREG_REQ,

    IMSA_REG_EVT_IMS_REG_SUCCESS,
    IMSA_REG_EVT_IMS_REG_FAILURE,

    IMSA_REG_EVT_TIMEOUT_PROTECT,
    IMSA_REG_EVT_TIMEOUT_RETRY,

    IMSA_REG_EVT_BUTT
};
typedef VOS_UINT32 IMSA_REG_EVT_ENUM_UINT32;


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : IMSA_FSM_PROC
 协议表格  :
 ASN.1描述 :
 结构说明  : 注册状态机处理函数原型定义
*****************************************************************************/
typedef VOS_UINT32 (*IMSA_FSM_PROC)(IMSA_REG_ENTITY_STRU *pstEntity, VOS_UINT32 ulEvt, VOS_VOID* pData);


/*****************************************************************************
 结构名    : IMSA_REG_FSM_ITEM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 状态机处理项结构体定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulState;
    VOS_UINT32 ulEvent;
    IMSA_FSM_PROC ProcFun;
} IMSA_REG_FSM_ITEM_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
/**
 * Declaration for global entity context
 */
extern IMSA_ENTITY_STRU *pgstImsaEntity;
/*extern HICSM_ImsError g_ImsErrorInfo;*/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
#define IMSA_RegCtxGet()    (&pgstImsaEntity->stImsaRegManager)
#define IMSA_CtxGet()       (pgstImsaEntity)

extern VOS_VOID   IMSA_RegMgrInit(VOS_VOID);
extern VOS_VOID   IMSA_RegMgrDeinit(VOS_VOID);

extern VOS_VOID IMSA_RegEntityInit(IMSA_REG_ENTITY_STRU *pstRegEntity);
extern IMSA_REG_ENTITY_STRU* IMSA_RegEntityGetByType(IMSA_REG_TYPE_ENUM_UINT8 enRegType);

extern VOS_UINT32 IMSA_RegProcUserRegReqMsg(const VOS_VOID *pRcvMsg);
extern VOS_UINT32 IMSA_RegProcUserDeregReqMsg(const VOS_VOID *pRcvMsg);

extern VOS_VOID IMSA_RegProcTimeoutProtect(const VOS_VOID *pTimerMsg);
extern VOS_VOID IMSA_RegProcTimeoutRetry(const VOS_VOID *pTimerMsg);

extern VOS_UINT32 IMSA_RegSendIntraMsg(VOS_UINT32 ulMsgId, const VOS_UINT8 *pucMsg, VOS_UINT32 ulLen);
extern VOS_VOID IMSA_RegEntityDeinit(IMSA_REG_ENTITY_STRU *pstRegEntity);
extern VOS_VOID IMSA_RegPrintEmcRegState( VOS_VOID );
extern VOS_VOID IMSA_RegPrintNrmRegState( VOS_VOID );
extern VOS_VOID IMSA_SndD2MsgServiceRegInfo(IMSA_IMS_INPUT_SERVICE_REASON_ENUM_UINT32   enInputServeReason);
extern VOS_VOID IMSA_RegSaveRegedPara
(
    IMSA_REG_TYPE_ENUM_UINT8            ulRegType,
    VOS_UINT8                           ucCid,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);
/* lihong00150010 lte pclint 717 begin */
#define IMSA_RegTimerStart(timer, para)   \
{                               \
    (timer)->usPara = (para);   \
    IMSA_StartTimer(timer);     \
}


#define IMSA_RegTimerStop(timer)          \
{                               \
    IMSA_StopTimer(timer);      \
}

#define IMSA_RegTimerRefresh(timer, para) \
{                               \
    if (IMSA_IsTimerRunning(timer)) \
    {                           \
        IMSA_StopTimer(timer);  \
    }                           \
    (timer)->usPara = (para);   \
    IMSA_StartTimer(timer);     \
}

#define IMSA_REG_FSM_STATE_TRANS(pstEntity, state)      \
{                                                       \
    if (IMSA_REG_TYPE_NORMAL == (pstEntity)->enRegType) \
    {                                                   \
        IMSA_INFO_LOG2("Normal REG entity state changed: ", (pstEntity)->enStatus, (state));    \
    }                                                   \
    else                                                \
    {                                                   \
        IMSA_INFO_LOG2("Emergency REG entity state changed: ", (pstEntity)->enStatus, (state)); \
    }                                                   \
                                                        \
    ((pstEntity)->enStatus = (state));                  \
}
/* lihong00150010 lte pclint 717 end */

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

#endif /* end of ImsaRegManagement.h */




