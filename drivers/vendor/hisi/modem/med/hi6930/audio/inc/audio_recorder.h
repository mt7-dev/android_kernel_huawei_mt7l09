

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "audio.h"
#include  "audio_pcm.h"

#ifndef __AUDIO_RECORDER_H__
#define __AUDIO_RECORDER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 获取录音控制结构体 */
#define AUDIO_RecorderGetObjPtr()                   (&g_stAudioRecorderObj)

/* 获取当前录音类型 */
#define AUDIO_RecorderGetCaptureType()              (g_stAudioRecorderObj.enRecordType)
/* 设置当前录音类型 */
#define AUDIO_RecorderSetCaptureType(enTpye)        (g_stAudioRecorderObj.enRecordType = enTpye)

#define AUDIO_RECORDER_BUFF_SIZE_320                (320)
#define AUDIO_RECORDER_BUFF_SIZE_640                (640)
#define AUDIO_RECORDER_BUFF_SIZE_1920               (1920)
#define AUDIO_RECORDER_BUFF_SIZE_3840               (3840)
#define ID_VOICE_AUDIO_RECORDER_UPDATE_BUFF_IND     (0xDDE6)

/****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_SAMPLERATE_ENUM
 功能描述  : 录音采样率枚举
*****************************************************************************/
enum AUDIO_RECORDER_SAMPLERATE_ENUM
{
    AUDIO_RECORDER_SAMPLERATE_8K              = 8000,
    AUDIO_RECORDER_SAMPLERATE_16K             = 16000,
    AUDIO_RECORDER_SAMPLERATE_48K             = 48000,
    AUDIO_RECORDER_SAMPLERATE_BUTT
};
typedef VOS_UINT32 AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_VOICE_TXRX_ENUM
 功能描述  : 通话录音语音上下行枚举
*****************************************************************************/
enum AUDIO_RECORDER_VOICE_TXRX_ENUM
{
    AUDIO_RECORDER_VOICE_TX             = 0,
    AUDIO_RECORDER_VOICE_RX             = 1,
    AUDIO_RECORDER_VOICE_TXRX_BUT
};
typedef VOS_UINT32 AUDIO_RECORDER_VOICE_TXRX_ENUM_UINT32;


/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_VOICE_BUFF_STRU
 功能描述  : Audio recorder 通话录音缓存结构体
*****************************************************************************/
 typedef struct
{
    VOS_UINT32                                  uwProtectWord1;
    VOS_INT16                                   ashwTxDataIn[AUDIO_RECORDER_BUFF_SIZE_1920];
    VOS_UINT32                                  uwProtectWord2;
    VOS_INT16                                   ashwTxDataOut[AUDIO_RECORDER_BUFF_SIZE_1920];

    VOS_UINT32                                  uwProtectWord3;
    VOS_INT16                                   ashwRxDataIn[AUDIO_RECORDER_BUFF_SIZE_1920];
    VOS_UINT32                                  uwProtectWord4;
    VOS_INT16                                   ashwRxDataOut[AUDIO_RECORDER_BUFF_SIZE_1920];
    VOS_UINT32                                  uwProtectWord5;

}AUDIO_RECORDER_VOICE_BUFF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_VOICE_STRU
 功能描述  : Audio recorder 通话录音结构体
*****************************************************************************/
 typedef struct
{
    VOS_UINT16                                  uwRefCnt;                       /* 通话录音引用计数 */
    VOS_UINT16                                  uhwReserved;
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwOrignalSmpRate;               /* 原始语音数据采样率 */
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwTargetSmpRate;                /* 目标语音数据采样率 */
    AUDIO_RECORDER_VOICE_BUFF_STRU              stRecordBuff;                   /* 录音数据 */

}AUDIO_RECORDER_VOICE_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_3RD_VOICE_STRU
 功能描述  : Audio recorder 第三方modem通话录音结构体，待完善
*****************************************************************************/
 typedef struct
{
    VOS_UINT16                                  uwRefCnt;                       /* 通话录音引用计数 */
    VOS_UINT16                                  uhwReserved;
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwOrignalSmpRate;               /* 原始语音数据采样率 */
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwTargetSmpRate;                /* 目标语音数据采样率 */

}AUDIO_RECORDER_3RD_VOICE_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_AUDIO_STRU
 功能描述  : Audio recorder 音频播放录音结构体，待完善
*****************************************************************************/
 typedef struct
{
    VOS_UINT16                                  uwRefCnt;                       /* 音频播放录音引用计数 */
    VOS_UINT16                                  uhwReserved;
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwOrignalSmpRate;               /* 原始音频数据采样率 */
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwTargetSmpRate;                /* 目标音频数据采样率 */

}AUDIO_RECORDER_AUDIO_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_FM_STRU
 功能描述  : Audio recorder FM播放录音结构体，待完善
*****************************************************************************/
 typedef struct
{
    VOS_UINT16                                  uwRefCnt;                       /* FM播放录音引用计数 */
    VOS_UINT16                                  uhwReserved;
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwOrignalSmpRate;               /* 原始音频数据采样率 */
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwTargetSmpRate;                /* 目标音频数据采样率 */

}AUDIO_RECORDER_FM_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_OBJ_STRU
 功能描述  : Audio recorder 录音控制结构体
*****************************************************************************/
 typedef struct
{
    VOS_UINT32                                  uwProtectWord1;
    VOS_UINT32                                  uwProtectWord2;

    AUDIO_RECORD_TYPE_ENUM_UINT16               enRecordType;                   /* 当前录音类型 */
    VOS_UINT16                                  uhwReserved;

    AUDIO_RECORDER_VOICE_STRU                   stVoiceRecord;                  /* CS通话录音 */
    AUDIO_RECORDER_3RD_VOICE_STRU               st3RDVoiceRecord;               /* 第三方modem通话录音 */
    AUDIO_RECORDER_AUDIO_STRU                   stAudioRecord;                  /* 音频播放录音 */
    AUDIO_RECORDER_FM_STRU                      stFMRecord;                     /* FM播放录音 */

    VOS_UINT32                                  uwProtectWord3;
    VOS_UINT32                                  uwProtectWord4;
}AUDIO_RECORDER_OBJ_STRU;

/*****************************************************************************
 实体名称  : AUDIO_RECORDER_UPDATE_BUFF_STRU
 功能描述  : Audio recorder 更新录音缓存消息结构体
*****************************************************************************/
 typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                                  uhwMsgId;
    VOS_UINT16                                  uhwReserved;
    VOS_INT16                                  *pshwBuff;
    VOS_UINT32                                  uwBuffSize;
    AUDIO_RECORDER_SAMPLERATE_ENUM_UINT32       uwVoiceSmpRate;
    AUDIO_RECORDER_VOICE_TXRX_ENUM_UINT32       enTxRxChn;
}AUDIO_RECORDER_UPDATE_BUFF_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern AUDIO_RECORDER_OBJ_STRU                 g_stAudioRecorderObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32  AUDIO_RECOEDER_Init(VOS_VOID);
extern VOS_VOID AUDIO_RECORDER_AudioCallBack(
                       VOS_VOID  *pshwBufOut,
                       VOS_VOID  *pshwBufIn,
                       VOS_UINT32 uwSampleRate,
                       VOS_UINT32 uwChannelNum,
                       VOS_UINT32 uwBufSize);
extern VOS_VOID AUDIO_RECORDER_FMCallBack(
                       VOS_VOID  *pshwBufOut,
                       VOS_VOID  *pshwBufIn,
                       VOS_UINT32 uwSampleRate,
                       VOS_UINT32 uwChannelNum,
                       VOS_UINT32 uwBufSize);
extern VOS_UINT32 AUDIO_RECORDER_MsgRecordStartCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_RECORDER_MsgRecordStopCmd(VOS_VOID *pvOsaMsg);
extern VOS_VOID AUDIO_RECORDER_SecondVoiceCallBack(
                       VOS_VOID  *pshwBufOut,
                       VOS_VOID  *pshwBufIn,
                       VOS_UINT32 uwSampleRate,
                       VOS_UINT32 uwChannelNum,
                       VOS_UINT32 uwBufSize);
extern VOS_VOID AUDIO_RECORDER_VoiceAudioPcmCB(
                       VOS_VOID  *pshwBufOut,
                       VOS_VOID  *pshwBufIn,
                       VOS_UINT32 uwTarSmpRate,
                       VOS_UINT32 uwChannelNum,
                       VOS_UINT32 uwTarBufSize);
extern VOS_UINT32  AUDIO_RECORDER_VoiceRcdStart(AUDIO_RECORD_START_STRU *pstRecStartMsg);
extern VOS_UINT32  AUDIO_RECORDER_VoiceRcdStop(VOS_VOID);
extern VOS_UINT32  AUDIO_RECORDER_VoiceVcMcCB(
                       VOS_INT16 *pshwTx,
                       VOS_INT16 *pshwRx,
                       VOS_UINT32 uwOrignalSmpRate);






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of audio_recorder.h */
