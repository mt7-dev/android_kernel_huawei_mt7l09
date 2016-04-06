
#ifndef _APP_VC_MAIN_H_
#define _APP_VC_MAIN_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "PsTypeDef.h"
#include "AppVcApi.h"
#include "VcComm.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "VcImsaInterface.h"
#endif
#include "VcCallInterface.h"


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
#define VC_MAX_BUFF_MSG_NUM                 (8)

/*内部宏定义*/
#define APP_VC_GetState()                   (g_stVcStateMgmt.enState)
#define APP_VC_GetCurrDevMode()             (g_stVcStateMgmt.enCurrDevMode)
#define APP_VC_GetPreDevMode()              (g_stVcStateMgmt.enPreDevMode)
#define APP_VC_GetDevHandle()               (g_stVcStateMgmt.iDevHandle)
#define APP_VC_GetRadioMode()               (g_stVcStateMgmt.enRadioMode)
#define APP_VC_GetCurrVolume()              (g_stVcStateMgmt.sCurrVolume)
#define APP_VC_GetPreVolume()               (g_stVcStateMgmt.sPreVolume)
#define APP_VC_GetPcVoiceSupportFlg()       (g_stVcStateMgmt.ucPcVoiceSupportFlag)
#define APP_VC_GetPcVoicePort()             (g_stVcStateMgmt.ucVoicePort)
#define APP_VC_GetPreMuteStatus()           (g_stVcStateMgmt.enPreMuteStatus)
#define APP_VC_GetCurrMuteStatus()          (g_stVcStateMgmt.enCurrMuteStatus)

#define APP_VC_MUTED_VOLUME                 (-80)
#define APP_VC_UNMUTED_VOLUME               (0)

#define APP_VC_GetCallStatus()              (g_stVcStateMgmt.bInCall)

#define APP_VC_GetStartHifiOrig()           (g_stVcStateMgmt.enStartHifiOrig)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum APP_VC_STATE_ENUM
{
    APP_VC_S_NULL,                                                              /* 空状态 */
    APP_VC_S_WAIT_INTERNAL_SET_DEV_RSLT,                                        /* 等待DEVICE设置结果状态 */
    APP_VC_S_WAIT_INTERNAL_SET_START_RSLT,                                      /* 等待START设置结果状态 */
    APP_VC_S_WAIT_INTERNAL_SET_CLOSE_RSLT,                                      /* 等待CLOSE设置结果状态 */
    APP_VC_S_WAIT_INTERNAL_SET_CODEC_RSLT,                                      /* 等待CODEC设置结果状态 */
    APP_VC_S_WAIT_AT_SET_DEV_RSLT,                                              /* 等待AT设置DEVICE结果状态 */
    APP_VC_S_WAIT_AT_SET_VOLUME_RSLT,                                           /* 等待AT设置VOLUME结果状态 */
    APP_VC_S_WAIT_AT_SET_MUTE_STATUS_RSLT,                                      /* 等待AT设置静音结果状态 */

    APP_VC_S_WAIT_SET_FOREGROUND_RSLT,                                          /* 等待设置前台模式结果状态 */
    APP_VC_S_WAIT_SET_BACKGROUND_RSLT,                                          /* 等待设置后台模式结果状态 */
    APP_VC_S_WAIT_QRY_GROUND_RSLT,                                              /* 等待查询前后台模式结果状态 */

    APP_VC_S_BUTT
};
typedef VOS_UINT16  APP_VC_GLOBAL_STATE_ENUM_U16;

enum APP_VC_MSG_ORIGIN_ENUM
{
    APP_VC_MSG_ORIGIN_AT,                                                           /* 来自AT设置的消息 */
    APP_VC_MSG_ORIGIN_INTERNAL,                                                     /* 来自内部消息 */
    APP_VC_MSG_ORIGIN_IMSA,                                                         /* 来自IMSA的消息 */
    APP_VC_MSG_ORIGIN_BUTT
};
typedef VOS_UINT16  APP_VC_MSG_ORIGIN_ENUM_U16;

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


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_VOID APP_VC_BuffMsgProc(VOS_VOID);

VOS_UINT32 APP_VC_SaveCurVolume(VOS_VOID);
VOS_UINT32 APP_VC_GetCurVolume(VOS_INT16 *pCurVolume);

VOS_VOID  APP_VC_AppQryModeReqProc(APP_VC_REQ_MSG_STRU  *pstAppMsg);
VOS_VOID  APP_VC_AppSetPortReqProc(APP_VC_REQ_MSG_STRU *pstAppMsg);
VOS_VOID  APP_VC_AppQryPortReqProc(APP_VC_REQ_MSG_STRU *pstAppMsg);
VOS_UINT32 APP_VC_GetVoiceTestFlag(VOS_UINT32 *pVoiceTestFlag);

#if (FEATURE_ON == FEATURE_IMS)
VC_IMSA_EXCEPTION_CAUSE_ENUM_UINT32  APP_VC_ConvertVcOpenChannelFailCauseToImsaExceptionCause(
    APP_VC_OPEN_CHANNEL_FAIL_CAUSE_ENUM_UINT32              enVcCause
);
CALL_VC_CODEC_TYPE_ENUM_U8  APP_VC_ConvertImsaCodeTypeToCallCodeType(
    IMSA_VC_CODEC_TYPE_ENUM_UINT8       enImsaCodeType
);
VOS_VOID  APP_VC_ImsaStartHifiNtfProc(IMSA_VC_START_HIFI_NTF_STRU* pstMsg);
VOS_VOID  APP_VC_ImsaStopHifiNtfProc(IMSA_VC_STOP_HIFI_NTF_STRU* pstMsg);
VOS_VOID  APP_VC_ImsaHifiParaChangeNtfProc(IMSA_VC_HIFI_PARA_CHANGED_NTF_STRU* pstMsg);
VOS_VOID  APP_VC_ImsaMsgProc(VOS_VOID *pMsg);

#endif
 #if (FEATURE_ON == FEATURE_PTM)
VOS_VOID APP_VC_AppVCFailErrRecord(
    VOS_UINT16                          usName,
    VOS_UINT16                          usCause
);
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

#endif /* end of MnCallCtx.h */

