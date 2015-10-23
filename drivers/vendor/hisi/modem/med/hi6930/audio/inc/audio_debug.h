/******************************************************************************

                  版权所有 (C), 2012-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   : audio_debug.h
  版 本 号   : 初稿
  作    者   : 王贵林 W164657
  生成日期   : 2012年7月26日
  最近修改   :
  功能描述   : AUDIO_debug.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年7月26日
    作    者   : 王贵林 W164657
    修改内容   : 创建文件

******************************************************************************/

#ifndef __AUDIO_DEBUG_H__
#define __AUDIO_DEBUG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "OmCodecInterface.h"
#include "audio_pcm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 2.2 全局变量封装 */
#define AUDIO_UtilGetHookPtr()       (&g_stAudioDbgHookObj)

/* 2.3 数据钩取位置与对应内容 */
enum AUDIO_HOOK_POS_ENUM
{
    AUDIO_HOOK_NONE                         = 0x0000,
    AUDIO_HOOK_APIN_PCM                     = 0x0001,   /* 0 bit */
    AUDIO_HOOK_CODECOUT_PCM                 = 0x0002,   /* 1 bit */
    AUDIO_HOOK_MICIN_PCM                    = 0x0004,   /* 2 bit */
    AUDIO_HOOK_ENHANCE_MICIN                = 0x0008,   /* 3 bit */
    AUDIO_HOOK_ENHANCE_SPKOUT               = 0x0010,   /* 4 bit,16 */
    AUDIO_HOOK_ENHANCE_MLIB_MICIN_16K_L     = 0x0020,   /* 5 bit,32 */
    AUDIO_HOOK_ENHANCE_MLIB_MICIN_16K_R     = 0x0040,   /* 6 bit,64 */
    AUDIO_HOOK_ENHANCE_MLIB_MICIN_48K_L     = 0x0080,   /* 7 bit,128 */
    AUDIO_HOOK_ENHANCE_MLIB_MICIN_48K_R     = 0x0100,   /* 8 bit,256 */
    AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_16K_L    = 0x0200,   /* 9 bit,512 */
    AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_16K_R    = 0x0400,   /* 10 bit,1024 */
    AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_48K_L    = 0x0800,   /* 11 bit,2048 */
    AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_48K_R    = 0x1000,   /* 12 bit,4096 */
    AUDIO_HOOK_ENHANCE_MLIB_SMARTPA_16K_L   = 0x2000,   /* 13 bit,8192 */
    AUDIO_HOOK_ENHANCE_MLIB_SMARTPA_16K_R   = 0x4000,   /* 14 bit,16384 */
    AUDIO_HOOK_BUTT
};
typedef VOS_UINT16 AUDIO_HOOK_POS_ENUM_UINT16;

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 编解码器类型枚举 */
enum AUDIO_DBG_CODEC_MODE_ENUM
{
    AUDIO_DBG_CODEC_MODE_ENCODE           = 0,
    AUDIO_DBG_CODEC_MODE_DECODE,
    AUDIO_DBG_CODEC_MODE_BUTT
};
typedef VOS_UINT16 AUDIO_DBG_CODEC_MODE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* 数据钩取结构体 */
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enHookEnable;                           /* 钩取数据是否打开 */
    VOS_UINT16                          usHookTarget;                           /* 钩取数据目标 */
}AUDIO_HOOK_STRU;

/*****************************************************************************
 实体名称  : AUDIO_LOOP_CNF_STRU
 功能描述  : 该消息通知AP音频环回打开或关闭是否成功
*****************************************************************************/
 typedef struct
{
    unsigned short                          uhwMsgId;           /*_H2ASN_Skip */
    AUDIO_PLAY_RESULT_CNF_ENUM_UINT16       enAudioLoopCnf;
} AUDIO_LOOP_CNF_STRU;

typedef struct
{
    unsigned short   usMsgId;           /*_H2ASN_Skip */
    unsigned short   usReserve;           /*_H2ASN_Skip */
    unsigned long    ulSenderPid;       /* SenderPid */
    unsigned long    ulReceiverPid;     /* ReceiverPid */
    unsigned long    ulChannel;         /* 通道号 */
    unsigned long    ulMsgLen;          /* 消息长度 */
}TEST_HIFI_MSG_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

extern AUDIO_HOOK_STRU g_stAudioDbgHookObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID AUDIO_DEBUG_Init( CODEC_NV_PARA_AUDIO_TRACE_CFG_STRU *pstAudioTraceCfg );
extern VOS_UINT32 AUDIO_DEBUG_MsgAudioLoopCloseReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_DEBUG_MsgAudioLoopOpenReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_DEBUG_MsgQueryStatusReq(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 AUDIO_DEBUG_MsgSetHookReq(VOS_VOID *pstOsaMsg);
extern VOS_VOID AUDIO_DEBUG_SendHookInd(
                       VOS_UINT16              uhwPos,
                       VOS_VOID               *pvData,
                       VOS_UINT32              uwLen);
extern VOS_UINT32 AUDIO_DEBUG_MsgTestCmd(VOS_VOID *pvOsaMsg);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of audio_debug.h */

