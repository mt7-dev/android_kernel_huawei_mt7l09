
#ifndef _VC_CTX_H_
#define _VC_CTX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "AppVcApi.h"
#include  "VcMain.h"
#include  "VcCallInterface.h"
#include  "PsTypeDef.h"


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
#define APP_VC_MAX_MSG_BUFF_LEN         (100)

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
/* APP_VC_BUFF_MSG_UNION删除 */

typedef struct
{
    VOS_UINT16                           usVcPcvoiceSupportFlag;                /*Pcvoice业务*/
}APP_VC_MS_CFG_INFO_STRU;

typedef struct
{
    APP_VC_MSG_ORIGIN_ENUM_U16              enMsgOrigin;
    VOS_UINT16                              usReserved;
    /* IMSA消息分为START_HIFI、STOP_HIFI和HIFI_PARA_CHANGE三个结构体，与原来不一致
       统一改成VOS_UINT8 */
    VOS_UINT8                               aucBufMsg[APP_VC_MAX_MSG_BUFF_LEN];
} APP_VC_BUFFER_MSG_STRU;

typedef struct
{
    VC_PHY_DEVICE_MODE_ENUM_U16             enCurrDevMode;
    VC_PHY_DEVICE_MODE_ENUM_U16             enPreDevMode;
    VOS_INT16                               sCurrVolume;
    VOS_INT16                               sPreVolume;
    APP_VC_GLOBAL_STATE_ENUM_U16            enState;                            /* VC的全局状态 */
    MN_CLIENT_ID_T                          clientId;
    MN_OPERATION_ID_T                       opId;
    APP_VC_VOICE_PORT_ENUM_U8               ucVoicePort;
    VOS_UINT8                               ucPcVoiceSupportFlag;
    APP_VC_START_HIFI_ORIG_ENUM_UINT8       enStartHifiOrig;                    /* 区分是IMSA还是CALL要启动HIFI */
    VOS_BOOL                                bParaUpdate;
    CALL_VC_RADIO_MODE_ENUM_U8              enRadioMode;                        /* 接入模式 */
    CALL_VC_CODEC_TYPE_ENUM_U8              enCodecType;                        /* codec type */
    APP_VC_MUTE_STATUS_ENUM_UINT8           enPreMuteStatus;
    APP_VC_MUTE_STATUS_ENUM_UINT8           enCurrMuteStatus;
    VOS_UINT32                              ulBuffLen;
    APP_VC_BUFFER_MSG_STRU                  astBufMsg[VC_MAX_BUFF_MSG_NUM];
    HTIMER                                  protectTimer;
    VOS_INT32                               iDevHandle;
    VOS_BOOL                                bInCall;                            /* 当前是否在呼叫过程中 */
    APP_VC_MS_CFG_INFO_STRU                 stMsCfgInfo;                      /* MS的配置信息 */
    VOS_UINT32                              ulVoiceTestFlag;
} APP_VC_STATE_MGMT_STRU;

/* 任意点回放导出全局变量使用*/
typedef struct
{
    APP_VC_STATE_MGMT_STRU                   pc_g_g_stVcStateMgmt;
} NAS_VC_OUTSIDE_RUNNING_CONTEXT_ST;



/*****************************************************************************
 Structure      : NAS_CC_SDT_MSG_ST
 Description    : PC回放工程，CC相关的全局变量通过以下消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                        MsgHeader;                                          /* 消息头                                   */ /*_H2ASN_Skip*/
    NAS_VC_OUTSIDE_RUNNING_CONTEXT_ST      stOutsideCtx;
}NAS_VC_SDT_MSG_ST;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
APP_VC_MS_CFG_INFO_STRU* APP_VC_GetCustomCfgInfo( VOS_VOID );
APP_VC_STATE_MGMT_STRU*  APP_VC_GetCtx( VOS_VOID );


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

#endif /* end of VCCtx.h */

