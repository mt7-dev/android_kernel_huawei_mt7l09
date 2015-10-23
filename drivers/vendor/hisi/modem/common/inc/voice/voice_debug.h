/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_debug.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年7月4日
  最近修改   :
  功能描述   : VOICE_debug.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年7月4日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

#ifndef __VOICE_DEBUG_H__
#define __VOICE_DEBUG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "OmCodecInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 2.2 全局变量封装 */
#define VOICE_DbgGetLoopFLagPtr()       (&g_stVoiceDbgLoop)
#define VOICE_DbgGetHookPtr()           (&g_stVoiceDbgHookObj)

/* 2.3 数据钩取位置与对应内容*/
#define VOICE_HOOK_NONE                 (0x0000)
#define VOICE_HOOK_TX_STRM              (0x0001)                                  /*bit0:上行码流*/
#define VOICE_HOOK_RX_STRM              (0x0002)                                  /*bit1:下行码流*/
#define VOICE_HOOK_MICIN_PCM            (0x0004)                                  /*bit2:从Mic输入的PCM*/
#define VOICE_HOOK_SPKOUT_PCM           (0x0008)                                  /*bit3:输出到Spk的PCM*/
#define VOICE_HOOK_CODECIN_PCM          (0x0010)                                  /*bit4:编码器输入的PCM*/
#define VOICE_HOOK_CODECOUT_PCM         (0x0020)                                  /*bit5:解码器输出的PCM*/
#define VOICE_HOOK_AECIN_PCM            (0x0040)                                  /*bit6:AEC输入的PCM*/
#define VOICE_HOOK_AECREF_PCM           (0x0080)                                  /*bit7:AEC参考PCM*/
#define VOICE_HOOK_JB_PARA              (0x0100)                                  /*bit8:JitterBuffer上报参数*/
#define VOICE_HOOK_TSM_DATA             (0x0400)                                  /*bit10:AJB TSM输出数据*/
#define VOICE_HOOK_BUTT                 (0x0800)                                  /*所有数据*/

/* 2.4 数据钩取特征字 */
#define VOICE_DBG_CODEC_TITLE_MAGIC_NUM                  (0x55AA55AA)
#define VOICE_DBG_CODEC_ENCODE_MAGIC_NUM                 (0x5A5A5A5A)
#define VOICE_DBG_CODEC_DECODE_MAGIC_NUM                 (0xA5A5A5A5)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 编解码器类型枚举 */
enum VOICE_DBG_CODEC_MODE_ENUM
{
    VOICE_DBG_CODEC_MODE_ENCODE           = 0,
    VOICE_DBG_CODEC_MODE_DECODE,
    VOICE_DBG_CODEC_MODE_BUTT
};
typedef VOS_UINT16 VOICE_DBG_CODEC_MODE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* 数据钩取结构体*/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enHookEnable;                           /* 钩取数据是否打开 */
    VOS_UINT16                          usHookTarget;                           /* 钩取数据目标 */
    VOS_UINT32                          ulFrameTick;                            /*当前语音帧帧号*/
} VOICE_HOOK_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern CODEC_LOOP_STRU g_stVoiceDbgLoop;
extern VOICE_HOOK_STRU g_stVoiceDbgHookObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID VOICE_DbgCheckAndLoop(CODEC_LOOP_ENUM_UINT16 uhwPos);
extern VOS_VOID VOICE_DbgInfoHookCfg(VOS_VOID);
extern VOS_VOID VOICE_DbgInit( CODEC_NV_PARA_VOICE_TRACE_CFG_STRU *pstVoiceTraceCfg );
extern VOS_VOID VOICE_DbgSendHook(
                       VOS_UINT16              uhwPos,
                       VOS_VOID               *pvData,
                       VOS_UINT16              uhwLen);
extern VOS_UINT32 VOICE_MsgOmSetHookReqIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgOmSetLoopReqIRP(VOS_VOID *pstOsaMsg);
extern VOS_VOID VOICE_DbgSaveCodecObj(
                       VOS_INT16                           *pshwCodedBuf,
                       VOICE_DBG_CODEC_MODE_ENUM_UINT16  uhwCodecMode,
                       VOS_VOID                            *pstCodec);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of VOICE_debug.h */
