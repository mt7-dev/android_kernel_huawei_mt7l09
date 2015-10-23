
#ifndef _NAS_CC_CTX_H_
#define _NAS_CC_CTX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"
#include "PsTypeDef.h"
#include "NasCcCommon.h"
#include "NasCcIe.h"
#include "NasCcAirMsg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  1 全局变量定义
*****************************************************************************/

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define NAS_CC_ONE_THOUSAND_MILLISECOND                    (1000)              /* 1000MS */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum NAS_CC_SS_SWITCH_STATE_ENUM
{
    NAS_CC_SS_SWITCH_IDLE               = 0,                                    /* 当前没有补充业务状态切换 */
    NAS_CC_SS_SWITCH_WAIT_CNF,                                                  /* 发送切换请求，等待网络的回复 */
    NAS_CC_SS_SWITCH_SUCCESS,                                                   /* 网侧已回复ACK消息 */
    NAS_CC_SS_SWITCH_FAILED,                                                    /* 网侧回复REJ或ERR */
    NAS_CC_SS_SWITCH_TIME_OUT,                                                  /* 网侧未回复导致超时 */

    NAS_CC_SS_SWITCH_LAST_CALL_RELEASED,

    NAS_CC_SS_SWITCH_STATE_BUTT
};
typedef VOS_UINT8 NAS_CC_SS_SWITCH_STATE_ENUM_UINT8;



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
    VOS_UINT8                           ucCcbsSupportFlg;                       /*CCBS(遇忙呼叫完成)业务*/
    VOS_UINT8                           aucReserved[3];
}NAS_CC_CUSTOM_CFG_INFO_STRU;
typedef struct
{
    NAS_CC_CUSTOM_CFG_INFO_STRU        stCustomCfg;                             /* NVIM中的定制信息 */
}NAS_CC_MS_CFG_INFO_STRU;
typedef struct
{
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enHoldState;                            /* 呼叫保持业务切换状态 */
    NAS_CC_CAUSE_VALUE_ENUM_U8          enHoldCause;                            /* 呼叫保持业务切换状态原因值 */
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enRetrieveState;                        /* 呼叫恢复业务切换状态 */
    NAS_CC_CAUSE_VALUE_ENUM_U8          enRetrieveCause;                        /* 呼叫恢复业务切换状态原因值 */
    NAS_CC_ENTITY_ID_T                  ulHoldEntityID;                         /* 呼叫保持业务实体ID */
    NAS_CC_ENTITY_ID_T                  ulRetrieveEntityID;                     /* 呼叫恢复业务实体ID */
    VOS_UINT8                           ucOpFacility;                           /* 是否保存了网络下发的FACILITY消息 */
    VOS_UINT8                           ucOpInvokeId;                           /* 是否保存了InvokeId */
    VOS_UINT8                           ucInvokeId;                             /* 记录InvokeId */
    VOS_UINT8                           aucReserved1[1];
    NAS_CC_MSG_FACILITY_MT_STRU         stFacility;                             /* 记录网侧下发的FACILITY消息 */
}NAS_CC_SS_SWITCH_INFO_STRU;


typedef struct
{
    NAS_CC_SS_SWITCH_INFO_STRU          stSwitchInfo;                           /* 通话补充业务状态切换信息 */
} NAS_CC_SS_CONTROL_STRU;
typedef struct
{
    NAS_CC_MS_CFG_INFO_STRU             stMsCfgInfo;                            /* MS的配置信息 */
    NAS_CC_SS_CONTROL_STRU              stSsControl;                            /* CC补充业务控制信息 */
} NAS_CC_CONTEXT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
NAS_CC_CUSTOM_CFG_INFO_STRU* NAS_CC_GetCustomCfgInfo( VOS_VOID );

NAS_CC_SS_SWITCH_STATE_ENUM_UINT8 NAS_CC_GetSsSwitchHoldState(VOS_VOID);

NAS_CC_SS_SWITCH_STATE_ENUM_UINT8 NAS_CC_GetSsSwitchRetrieveState(VOS_VOID);

NAS_CC_ENTITY_ID_T NAS_CC_GetSsSwitchHoldEntityID(VOS_VOID);

NAS_CC_ENTITY_ID_T NAS_CC_GetSsSwitchRetrieveEntityID(VOS_VOID);

NAS_CC_CAUSE_VALUE_ENUM_U8 NAS_CC_GetSsSwitchHoldCause(VOS_VOID);

NAS_CC_CAUSE_VALUE_ENUM_U8 NAS_CC_GetSsSwitchRetrieveCause(VOS_VOID);

VOS_VOID NAS_CC_SetSsSwitchHoldInfo(
    NAS_CC_ENTITY_ID_T                  ulEntityID,
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enState,
    NAS_CC_CAUSE_VALUE_ENUM_U8          enCause
);

VOS_VOID NAS_CC_SetSsSwitchRetrieveInfo(
    NAS_CC_ENTITY_ID_T                  ulEntityID,
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enState,
    NAS_CC_CAUSE_VALUE_ENUM_U8          enCause
);

VOS_UINT8 NAS_CC_GetSsSwitchOpFacility(VOS_VOID);

VOS_VOID NAS_CC_SetSsSwitchOpFacility(
    VOS_UINT32                          ulOpFlg,
    NAS_CC_MSG_FACILITY_MT_STRU        *pstFacilityMsg
);

NAS_CC_MSG_FACILITY_MT_STRU* NAS_CC_GetSsSwitchFacility(VOS_VOID);

VOS_VOID NAS_CC_SetSsSwitchInvokeId(
    VOS_UINT8                           ucFlg,
    VOS_UINT8                           ucInvokeId);

VOS_UINT8 NAS_CC_GetSsSwitchOpInvokeId(VOS_VOID);

VOS_UINT8 NAS_CC_GetSsSwitchInvokeId(VOS_VOID);

VOS_VOID NAS_CC_InitSsSwitchInfo(VOS_VOID);

VOS_VOID NAS_CC_SetSsSwitchHoldEntityID(NAS_CC_ENTITY_ID_T ulEntityID);

VOS_VOID NAS_CC_SetSsSwitchRetrieveEntityID(NAS_CC_ENTITY_ID_T ulEntityID);


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

#endif /* end of MnCallCtx.h */

