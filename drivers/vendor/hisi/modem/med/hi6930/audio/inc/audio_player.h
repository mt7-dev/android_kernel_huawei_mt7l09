/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : audio_player.h
  版 本 号   : 初稿
  作    者   : C00137131
  生成日期   : 2012年7月10日
  最近修改   :
  功能描述   : audio_player.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年7月10日
    作    者   : C00137131
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "audio.h"
#include  "xa_comm.h"
#include  "ucom_low_power.h"
#include  "xa_dts_pp.h"

#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define AUDIO_PLAYER_BUFF_POOL_SIZE                 (0x9c00)   /* 缓冲池深度为采样率为192K时两帧26ms的双通道数据，单位byte */
#define AUDIO_PLAYER_SRC_DOLBY_BUFF_SIZE            (0x3C00)   /* SRC处理之后缓冲池。4帧48K长为20ms双声道数据长度，单位:byte
                                                                * (4 * AUDIO_PLAYER_PCM_20MS_BUFF_SIZE) */
#define AUDIO_PLAYER_PLAY_CHN_NUM                   (2)        /* Audio player播放声道个数 */
#define AUDIO_PLAYER_CHUNK_SIZE                     (128)      /* SRC每次处理的chunksize */
#define AUDIO_PLAYER_BUFF_SIZE_512K                 (0x80000)  /* 待解码数据块大小512k */
#define AUDIO_PLAYER_BUFF_SIZE_200K                 (0x32000)  /* 待解码数据块大小200k */
#define AUDIO_PLAYER_BUFF_SIZE_50K                  (0xC800)   /* 待解码数据块大小50k */
#define AUDIO_PLAYER_PCM_WIDTH_SIZE                 (16)       /* PCM位宽 */
#define AUDIO_PLAYER_PCM_20MS_BUFF_SIZE             (3840)     /* 每20ms PCM需要搬运走的音频数据，单位byte
                                                                *计算方法:48000K * 20ms * 双声道 * sizeof(VOS_INT16) */
#define AUDIO_PLAYER_DTS_PROC_NUM                   (15)       /* DTS每20ms数据需要处理的次数, 每次处理64个样点，计算方法:
                                                                * AUDIO_PLAYER_PCM_20MS_BUFF_SIZE/双声道/sizeof(short)/64 */
#define AUDIO_PLAYER_DTS_CHUNK_SIZE                 (64)       /* DTS每次处理的chunksize */
#define AUDIO_PLAYER_OUTPUT_SAMPLE_RATE             (48000)    /* 音频输出的采样率，模拟codec配置为48K */
#define AUDIO_PLAYER_DTS_BUFF_SIZE                  (3840)     /* DTS buff大小 */

/* morphing volue while enable DTS */
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_0           (0)
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_5           (5)
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_6           (6)
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_10          (10)
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_11          (11)
#define AUDIO_PLAYER_MORPHING_VOL_FRAME_21          (21)

/* 以下XA related mem buff value are all temporary, as not provided exactly yet */
#define AUDIO_PLAYER_XA_API_OBJ_BUFF_SIZE           (0x2C)      /* XA api struct buffer size */
#define AUDIO_PLAYER_XA_PERSIST_MEM_SIZE            (0x30CC)   /* XA persist memory buffer size */
#define AUDIO_PLAYER_XA_SCRATCH_MEM_SIZE            (0x5C30)   /* XA scratch memory buffer size */
#define AUDIO_PLAYER_XA_INPUT_MEM_SIZE              (0x2FA8)   /* XA input memory buffer size */
#define AUDIO_PLAYER_XA_OUTPUT_MEM_SIZE             (0x3E80)   /* XA output memory buffer size */
#define AUDIO_PLAYER_XA_TABLE_MEM_SIZE              (0xA0)     /* XA table memory buffer size */

/* 得到播放器全局控制变量 */
#define AUDIO_PlayerGetPlayPtr()                    (&g_stAudioPlayerObj)

/* 得到播放器码流缓冲控制结构体 */
#define AUDIO_PlayerGetPlayBuf()                    (&g_stAudioPlayerBuff)
/* 得到播放器码流缓冲池buffer指针 */
#define AUDIO_PlayerGetPlayBufPoolPtr()             (g_stAudioPlayerBuff.scPlayBufPool)
/* 得到播放器码流缓冲池buffer深度 */
#define AUDIO_PlayerGetPlayBufPoolDepth()           (g_stAudioPlayerBuff.uwPlayBufPoolDepth)
/* 设置播放器码流缓冲池buffer深度 */
#define AUDIO_PlayerSetPlayBufPoolDepth(uwSize)\
                                                    (g_stAudioPlayerBuff.uwPlayBufPoolDepth = uwSize)
/* 获取解码输入缓冲结构体 */
#define AUDIO_PlayerGetInputBuf()                   (&g_stAudioPlayerInBuf)
/* 得到原始待解码存放buffer指针 */
#define AUDIO_PlayerGetInputBufPtr()                (g_stAudioPlayerInBuf.ascBuff)
/* 得到解码后存放buffer指针 */
#define AUDIO_PlayerGetOutputBufPtr()               (g_pvOutputBuff)

/* 获得audio player运行状态 */
#define AUDIO_PlayerGetState()                      (g_stAudioPlayerObj.enState)
/* 设置audio player运行状态 */
#define AUDIO_PlayerSetState(uhwVar)                (g_stAudioPlayerObj.enState = uhwVar)

/* 得到存放原始音频数据的DDR地址指针 */
#define AUDIO_PlayerGetBufAddr()                    (g_stAudioPlayerObj.uwDecBufAddr)
#define AUDIO_PlayerSetBufAddr(var)                 (g_stAudioPlayerObj.uwDecBufAddr = (var))

#define AUDIO_PlayerGetApBufAddr()                  (g_stAudioPlayerObj.uwApBufAddr)
#define AUDIO_PlayerSetApBufAddr(uwValue)           (g_stAudioPlayerObj.uwApBufAddr = (uwValue))

#define AUDIO_PlayerGetSwapBuf()                    (&g_stAudioPlayerSwapBuf)
#define AUDIO_PlayerGetSwapBufAddr()                (g_stAudioPlayerSwapBuf.ascBuff)

/* 获得SRC缓冲结构体 */
#define AUDIO_PlayerGetSrcBuff()                    (&g_stAudioPlayerSrcBuff)
/* SRC处理之后缓冲 */
#define AUDIO_PlayerGetSrcBuffPtr()                 (g_stAudioPlayerSrcBuff.scPlayBufPool)
/* 得到SRC处理之后缓冲池buffer深度 */
#define AUDIO_PlayerGetSrcBufPoolDepth()            (g_stAudioPlayerSrcBuff.uwPlayBufPoolDepth)
/* 设置SRC处理之后缓冲池buffer深度 */
#define AUDIO_PlayerSetSrcBufPoolDepth(uwSize)\
                                                    (g_stAudioPlayerSrcBuff.uwPlayBufPoolDepth = uwSize)
/* 获取DTS缓冲结构 */
#define AUDIO_PlayerGetDtsBuff()                    (&g_stAudioPlayerDtsBuff)
/* 获取DTS音效处理之后的缓冲buff指针 */
#define AUDIO_PlayerGetDtsBuffPtr()                 (g_stAudioPlayerDtsBuff.ascBuff)


#define AUDIO_PLAYER_FIR_INIT(uhwSampInBuf, uwLen) fir_init(samp_in_buf, len)
#define AUDIO_PLAYER_FIR_FIX(uhwPcmBuf, uhwSampInBuf, uwCoef, uwFilterLen, uwLen) \
                     fir_fix(PCM_buf, samp_in_buf, coef, filter_len,len)

#define PLAYER_RET_HANDLE(uwRet)   \
    if(uwRet != UCOM_RET_SUCC) \
    { \
        return uwRet; \
    }

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_PLAY_PART_ENUM_UINT16
 功能描述  : Audio Player读取数据所属分区
*****************************************************************************/
enum
{
    AUDIO_PLAY_PART_MAIN = 0,                               /* 主buffer 4920k */
    AUDIO_PLAY_PART_SUB,                                    /* 辅助buffer 200k */
    AUDIO_PLAY_PART_BUTT
};
typedef VOS_INT16 AUDIO_PLAY_PART_ENUM_UINT16;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_STATE_ENUM_UINT16
 功能描述  : Audio Player运行状态枚举
*****************************************************************************/
typedef enum
{
    AUDIO_PLAYER_STATE_IDLE = 0,                            /* IDLE状态 */
    AUDIO_PLAYER_STATE_RUNNING,                             /* RUNNING状态 */
    AUDIO_PLAYER_STATE_PAUSE,                               /* 暂停状态 */
    AUDIO_PLAYER_STATE_BUTT
} AUDIO_PLAYER_STATE_ENUM;
typedef VOS_UINT16  AUDIO_PLAYER_STATE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_BUFF_ENUM_UINT16
 功能描述  : Audio Player运行所涉及buff类型枚举
*****************************************************************************/
typedef enum
{
    AUDIO_PLAYER_DEC_BUFF = 0,                              /* 解码之后缓冲池 */
    AUDIO_PLAYER_SRC_BUFF,                                  /* 变采样之后缓冲池 */
    AUDIO_PLAYER_BUFF_BUTT
} AUDIO_PLAYER_BUFF_ENUM;
typedef VOS_UINT16  AUDIO_PLAYER_BUFF_ENUM_UINT16;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_VOL_ENUM_UINT32
 功能描述  : Audio Player设置音量类型枚举
*****************************************************************************/
typedef enum
{
    AUDIO_PLAYER_NORMAL_VOL = 0,                              /* 普通设置音量 */
    AUDIO_PLAYER_DTS_ENABLE_VOL,                              /* dts使能音量 */
    AUDIO_PLAYER_VOL_BUTT
} AUDIO_PLAYER_VOL_ENUM;
typedef VOS_UINT32  AUDIO_PLAYER_VOL_ENUM_UINT32;
/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
 结构名    : AUDIO_PLAYER_DECODE_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 开始/停止解码处理消息头
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;
    VOS_UINT16                          uhwReserve[3];
}AUDIO_PLAYER_DECODE_IND_STRU;

/*****************************************************************************
  5 消息定义
*****************************************************************************/

/* AudioPlayer 内部消息 */
enum AUDIO_PLAYER_MSG_ENUM
{
    ID_AUDIO_PLAYER_START_DECODE_IND    = 0xDD51,           /* 启动解码 */
    ID_AUDIO_PLAYER_STOP_DECODE_IND     = 0xDD53,           /* 停止解码 */
    ID_AUDIO_PLAYER_SET_DTS_ENABLE_IND  = 0xDD37,           /* audioplayer向dts模块转发的ap消息 */
    ID_AUDIO_PLAYER_MSG_BUTT,
};

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_VOL_STRU
 功能描述  : Audio Player 音量控制
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwTarget;                               /* 调节目标音量 Q15*/
    VOS_UINT32                          uwCurr;                                 /* 当前音量 Q15*/
    VOS_UINT32                          uwOriginal;                             /* 原始音量 Q15*/
    VOS_UINT32                          uwDtsEnableFrame;                       /* 为DTS使能音量渐变缓冲所处理的帧数 */
    AUDIO_PLAYER_VOL_ENUM_UINT32        enSetVolEnum;                           /* 设置音量类型 */
} AUDIO_PLAYER_VOL_STRU;


/*****************************************************************************
 实体名称  : AUDIO_PLAYER_OBJ_STRU
 功能描述  : Audio Player MP3/AAC音频播放通路整体控制结构体
*****************************************************************************/
 typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;

    AUDIO_PLAYER_STATE_ENUM_UINT16      enState;                                /* audio player运行状态 */
    AUDIO_FILE_FORMAT_ENUM_UINT16       enFormat;                               /* 音频格式 */
    VOS_UINT32                          uwStartPos;                             /* 开始位置 */
    VOS_UINT32                          uwFileHeaderRdy;                        /* 本文件头是否已读取 */
    VOS_UINT32                          uwSampleRate;                           /* 音频数据采样率 */
    VOS_UINT32                          uw20msDataSize;                         /* 20ms PCM码流数据 */

    AUDIO_PLAY_DONE_IND_ENUM_UINT16     enPlayStatus;                           /* 播放结果枚举 */
    VOS_UINT16                          uhwIsDataBak;                           /* 当前是否使用备份Buf作为解码数据源 */
    VOS_UINT32                          uwPlayedBytes;                          /* 当前已解码的数据，单位byte，含文件头 */
    VOS_UINT32                          uwDecBufAddr;                           /* 当前待解码数据地址 */
    VOS_UINT32                          uwDecDataLeft;                          /* 当前剩余待解码数据大小，单位byte */
    VOS_UINT32                          uwApBufAddr;                            /* AP侧待解码数据首地址 */
    VOS_UINT32                          uwApDataLeft;                           /* AP侧待解码Buff中剩余的待解码数据的大小，在解码过程中动态变化 */
    VOS_UINT32                          uwApBufSize;                            /* AP侧待解码Buff的总大小 */
    VOS_UINT32                          uwTotalFileSize;                        /* 整首MP3/AAC音乐的大小，单位byte，含文件头 */
    VOS_UINT32                          uwDecConsumed;
    VOS_UINT32                          uwDecInMemSize;                         /* XA decode input memory size */
    AUDIO_PLAYER_VOL_STRU               stVolume;
    XA_DTS_ENABLE_ENUM_UINT16           enDtsEnable;                            /* DTS是否使能标志位 */
    VOS_UINT16                          uhwReserve;

    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_OBJ_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_BUF_STRU
 功能描述  : audio player MP3/AAC文件处理数据BUFF结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwPlayBufPoolDepth;                     /* 解码后码流池深度，单位bytes数 */
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;
    VOS_CHAR                            scPlayBufPool[AUDIO_PLAYER_BUFF_POOL_SIZE];
    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_BUF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_SRC_BUF_STRU
 功能描述  : src解码之后的缓冲池结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwPlayBufPoolDepth;                     /* 解码后码流池深度，单位bytes数 */
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;
    VOS_CHAR                            scPlayBufPool[AUDIO_PLAYER_SRC_DOLBY_BUFF_SIZE];
    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_SRC_BUF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_IN_BUF_STRU
 功能描述  : audio player MP3/AAC文件解码输入
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;
    VOS_CHAR                            ascBuff[AUDIO_PLAYER_XA_INPUT_MEM_SIZE];
    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_IN_BUF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_SWAP_BUF_STRU
 功能描述  : audio player 备份交换
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;
    VOS_CHAR                            ascBuff[AUDIO_PLAYER_BUFF_SIZE_200K];
    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_SWAP_BUF_STRU;

/*****************************************************************************
 实体名称  : AUDIO_PLAYER_DTS_BUF_STRU
 功能描述  : audio player dts使用的缓冲
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;
    VOS_CHAR                            ascBuff[AUDIO_PLAYER_DTS_BUFF_SIZE];
    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
}AUDIO_PLAYER_DTS_BUF_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
/* 保存MP3/AAC等音频通路相关控制信息 */
extern AUDIO_PLAYER_OBJ_STRU                   g_stAudioPlayerObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 AUDIO_PLAYER_ApiInit(VOS_VOID);
extern VOS_VOID AUDIO_PLAYER_AudioCallBack(
                       VOS_VOID  *pshwBufOut,
                       VOS_VOID  *pshwBufIn,
                       VOS_UINT32 uwSampleRate,
                       VOS_UINT32 uwChannelNum,
                       VOS_UINT32 uwBufSize);
extern VOS_UINT32 AUDIO_PLAYER_CheckBuff( VOS_VOID );
extern VOS_INT32  AUDIO_PLAYER_DoDts(
                       VOS_INT16 *pshwInput,
                       VOS_INT16 *pshwOutput);
extern VOS_UINT32 AUDIO_PLAYER_DoPlay(VOS_VOID);
extern VOS_UINT32 AUDIO_PLAYER_DoSeek(VOS_UINT32 uwSkipSize, CODEC_SEEK_DERECT_ENUM_UINT16 enSeekDirect);
extern VOS_UINT32 AUDIO_PLAYER_DoSrc(
                                       VOS_INT16  *pshwBufIn,
                                       VOS_INT16  *pshwBufOut,
                                       VOS_UINT32  uwProcTime);
extern VOS_UINT32 AUDIO_PLAYER_DoStop(VOS_VOID);
extern VOS_VOID AUDIO_PLAYER_FillBuff(AUDIO_PLAYER_OBJ_STRU *pstPlayer);
extern VOS_UINT32  AUDIO_PLAYER_GetCurrDecPos(AUDIO_FILE_FORMAT_ENUM_UINT16 enFormat);
extern VOS_UINT16 AUDIO_PLAYER_GetCurrVol( VOS_VOID );
extern VOS_UINT32 AUDIO_PLAYER_HandleFile(CODEC_STATE_ENUM_UINT16 *penDecoderState);
extern VOS_UINT32 AUDIO_PLAYER_Init(VOS_VOID);
extern VOS_VOID AUDIO_PLAYER_InitBuff( VOS_VOID );
extern VOS_UINT32  AUDIO_PLAYER_IsIdle(VOS_VOID);
extern VOS_UINT32 AUDIO_PLAYER_MsgPlayDoneSignalReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgQueryStatusReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgQueryTimeReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgSeekReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgSetVolReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32  AUDIO_PLAYER_MsgStartDecodeInd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgStartReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32  AUDIO_PLAYER_MsgStopDecodeInd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgStopReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgUpdateApBuffCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_MsgUpdatePcmBuffCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_PLAYER_PcmStart(VOS_VOID);
extern VOS_VOID AUDIO_PLAYER_PcmStop(VOS_VOID);
extern VOS_VOID AUDIO_PLAYER_SendApAudioCnf(VOS_UINT16    uhwMsgId,
                                                         VOS_UINT32    uwRet,
                                                         VOS_VOID     *pvTail,
                                                         VOS_UINT32    uwTailLen);
extern VOS_UINT32 AUDIO_PLAYER_SendPcmSetBufCmd(VOS_UINT32 uwBufPoolAddr,
                                                               VOS_UINT32 uwBufSize);
extern VOS_UINT32 AUDIO_PLAYER_SendPlayDoneInd(AUDIO_PLAY_DONE_IND_ENUM_UINT16   enPlayDone);
extern VOS_VOID AUDIO_PLAYER_SendPlaySeekCnf(AUDIO_PLAY_RESULT_CNF_ENUM_UINT16 enSeekResult,
                                                         VOS_VOID               *pvTail,
                                                         VOS_UINT32              uwTailLen);
extern VOS_VOID AUDIO_PLAYER_SendPlayStopCnf(VOS_VOID *pvTail, VOS_UINT32 uwTailLen);
extern VOS_UINT32  AUDIO_PLAYER_ShiftBuff(
                       AUDIO_PLAYER_BUFF_ENUM_UINT16 enBuffType,
                       VOS_UINT32                    uwShiftSize);
extern VOS_UINT32  AUDIO_PLAYER_MsgSetDtsEnableCmd(VOS_VOID *pvOsaMsg);

extern VOS_UINT32  AUDIO_PLAYER_DtsMorphingVol(VOS_VOID);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of audio_player.h */
