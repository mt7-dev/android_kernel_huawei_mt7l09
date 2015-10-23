
#ifndef __AUDIO_ENHANCE_H__
#define __AUDIO_ENHANCE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "audio.h"
#include "audio_pcm.h"
#include "xa_src_pp.h"
#include "hifidrvinterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* ENHANCE帧长最大值(单声道) */
#define AUDIO_ENHANCE_MAX_FRAME_LENGTH  (AUDIO_PCM_MAX_FRAME_LENGTH/2)

/* 默认处理的采样率 */
#define AUDIO_ENHANCE_DEFAULT_SAMPLE_RATE   (48000)

/* 获取控制结构体指针 */
#define AUDIO_ENHANCE_GetCtrl()             (&g_stAudioEnhanceCtrl)
/* 获取模块控制指针 */
#define AUDIO_ENHANCE_GetModuleCtrl(enMod)  (&g_stAudioEnhanceCtrl.astModule[enMod])
/* 获取模块注册指针 */
#define AUDIO_ENHANCE_GetModuleReg(enMod)   (&g_astAudioEnhanceModuleTab[enMod])

/* 获取缓冲结构体指针 */
#define AUDIO_ENHANCE_GetBuff()             (&g_stAudioEnhanceBuff)
/* 获取MicIn左声道 */
#define AUDIO_ENHANCE_GetMicInL(enBuff)     (g_stAudioEnhanceBuff.ashwMicInL[enBuff])
/* 获取MicIn右声道 */
#define AUDIO_ENHANCE_GetMicInR(enBuff)     (g_stAudioEnhanceBuff.ashwMicInR[enBuff])
/* 获取SpkOut左声道 */
#define AUDIO_ENHANCE_GetSpkOutL(enBuff)    (g_stAudioEnhanceBuff.ashwSpkOutL[enBuff])
/* 获取SpkOut右声道 */
#define AUDIO_ENHANCE_GetSpkOutR(enBuff)    (g_stAudioEnhanceBuff.ashwSpkOutR[enBuff])

/* 计算单声道帧长(Byte) */
#define AUDIO_ENHANCE_GetMonoFrmSize(Rate)  (((Rate) / 8000) * 320)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 模块运行状态 */
enum AUDIO_ENHANCE_STATUS_ENUM
{
    AUDIO_ENHANCE_STATUS_IDLE           = 0,
    AUDIO_ENHANCE_STATUS_RUNNING,
    AUDIO_ENHANCE_STATUS_BUTT,
};
typedef VOS_UINT32 AUDIO_ENHANCE_STATUS_ENUM_UINT32;

/* 缓存枚举 */
enum AUDIO_ENHANCE_BUFF_ENUM
{
    AUDIO_ENHANCE_BUFF_IN               = 0,                /* 输入缓冲 */
    AUDIO_ENHANCE_BUFF_OUT,                                 /* 输出缓冲 */
    AUDIO_ENHANCE_BUFF_SWAP,                                /* 交换缓冲(做为临时缓冲使用) */
    AUDIO_ENHANCE_BUFF_BUTT,
};
typedef VOS_UINT32 AUDIO_ENHANCE_BUFF_ENUM_UINT32;

/* 外部输入状态(SAMRT_PA) */
enum AUDIO_ENHANCE_EX_REF_ENUM
{
    AUDIO_ENHANCE_EX_REF_OFF            = 0,                /* 不使用外部信号反馈 */
    AUDIO_ENHANCE_EX_REF_ON,                                /* 使用外部信号反馈 */
    AUDIO_ENHANCE_EX_REF_BUTT
};
typedef VOS_UINT32  AUDIO_ENHANCE_EX_REF_ENUM_UINT32;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/* ENHANCE 模块控制结构体 */
typedef struct
{
    VOS_UINT32                          uwRefCnt;                               /* 引用计数 */
    AUDIO_ENHANCE_STATUS_ENUM_UINT32    enStatus;                               /* 声学处理工作状态 */
    VOS_UINT32                          uwSampleRate;                           /* 采样率 */

} AUDIO_ENHANCE_MODULE_CTRL_STRU;

/* ENHANCE 控制结构体 */
typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;

    AUDIO_ENHANCE_DEVICE_ENUM_UINT32    enDevice;                               /* 设备模式 */
    AUDIO_ENHANCE_EX_REF_ENUM_UINT32    enExRef;                                /* 外部参考输入 */
    VOS_UINT32                          uwSrcRefFlag;                           /* 变采样引用标记 */

    AUDIO_ENHANCE_MODULE_ENUM_UINT32    enCurrModule;                           /* 当前工作模块 */
    AUDIO_ENHANCE_MODULE_CTRL_STRU      astModule[AUDIO_ENHANCE_MODULE_BUTT];   /* 模块工作信息 */

    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
} AUDIO_ENHANCE_CTRL_STRU;

/* ENHANCE 缓存结构体 */
typedef struct
{
    VOS_UINT32                          uwProtectWord1;
    VOS_UINT32                          uwProtectWord2;

    VOS_INT16                           ashwMicInL[AUDIO_ENHANCE_BUFF_BUTT][AUDIO_ENHANCE_MAX_FRAME_LENGTH];
    VOS_INT16                           ashwMicInR[AUDIO_ENHANCE_BUFF_BUTT][AUDIO_ENHANCE_MAX_FRAME_LENGTH];

    VOS_INT16                           ashwSpkOutL[AUDIO_ENHANCE_BUFF_BUTT][AUDIO_ENHANCE_MAX_FRAME_LENGTH];
    VOS_INT16                           ashwSpkOutR[AUDIO_ENHANCE_BUFF_BUTT][AUDIO_ENHANCE_MAX_FRAME_LENGTH];

    VOS_UINT32                          uwProtectWord3;
    VOS_UINT32                          uwProtectWord4;
} AUDIO_ENHANCE_BUFF_STRU;

/* ENHANCE 模块注册结构 */
typedef struct
{
    /* MIC处理 */
    VOS_UINT32                          uwMicInPathID;
    /* SPK处理 */
    VOS_UINT32                          uwSpkOutPathID;

} AUDIO_ENHANCE_MODULE_REG_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32  AUDIO_ENHANCE_Init( VOS_VOID );
extern VOS_UINT32  AUDIO_ENHANCE_InitSrc(
                        AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule,
                        VOS_UINT32                          uwSampleRate );
extern VOS_UINT32 AUDIO_ENHANCE_MsgOmGetNvReq(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 AUDIO_ENHANCE_MsgOmSetNvReq(VOS_VOID *pstOsaMsg);
extern VOS_UINT32  AUDIO_ENHANCE_MsgSetDeviceInd( VOS_VOID *pvOsaMsg );
extern VOS_UINT32  AUDIO_ENHANCE_MsgSetDeviceReqRt( VOS_VOID *pvOsaMsg );
extern VOS_UINT32 AUDIO_ENHANCE_MsgSetParaInd(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 AUDIO_ENHANCE_MsgSetParaIndRt(VOS_VOID *pstOsaMsg);
extern VOS_UINT32  AUDIO_ENHANCE_MsgStartInd( VOS_VOID *pvOsaMsg );
extern VOS_UINT32  AUDIO_ENHANCE_MsgStartReqRt( VOS_VOID *pvOsaMsg );
extern VOS_UINT32  AUDIO_ENHANCE_MsgStopInd( VOS_VOID *pvOsaMsg );
extern VOS_UINT32  AUDIO_ENHANCE_MsgStopReqRt( VOS_VOID *pvOsaMsg );
extern VOS_VOID AUDIO_ENHANCE_MsgSyncConfirm(VOS_UINT16 uhwMsgId,
                                                           VOS_UINT32 uwRet,
                                                           VOS_VOID   *pvTail,
                                                           VOS_UINT32  uwTailLen);
extern VOS_UINT32 AUDIO_ENHANCE_ProcClear( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule );
extern VOS_UINT32 AUDIO_ENHANCE_ProcInit( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule );
extern VOS_UINT32 AUDIO_ENHANCE_ProcMicIn(
                       VOS_VOID               *pshwBufOut,
                       VOS_VOID               *pshwBufIn,
                       VOS_UINT32              uwSampleRate,
                       VOS_UINT32              uwChannelNum,
                       VOS_UINT32              uwBufSize);
extern VOS_UINT32 AUDIO_ENHANCE_ProcSetPara(
                       AUDIO_ENHANCE_MODULE_ENUM_UINT32            enModule,
                       AUDIO_ENHANCE_DEVICE_ENUM_UINT32            enDevice );
extern VOS_UINT32 AUDIO_ENHANCE_ProcSpkOut(
                       VOS_VOID               *pshwBufOut,
                       VOS_VOID               *pshwBufIn,
                       VOS_UINT32              uwSampleRate,
                       VOS_UINT32              uwChannelNum,
                       VOS_UINT32              uwBufSize);
extern VOS_VOID AUDIO_ENHANCE_RegisterPcmCallback( VOS_VOID );
extern VOS_UINT32 AUDIO_ENHANCE_SetDevice( AUDIO_ENHANCE_DEVICE_ENUM_UINT32 enDevice );
extern VOS_UINT32 AUDIO_ENHANCE_Start(
                       AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule,
                       VOS_UINT32                          uwSampleRate );
extern VOS_UINT32 AUDIO_ENHANCE_Stop( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule );
extern VOS_UINT32  AUDIO_ENHANCE_DoSrc(
                XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                VOS_VOID                  *pInputBuff,
                VOS_VOID                  *pOutputBuff,
                VOS_UINT32                *puwOutputSize);
extern VOS_VOID  AUDIO_ENHANCE_CloseSrc( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule );
extern VOS_VOID  AUDIO_ENHANCE_SetExRefPort( AUDIO_ENHANCE_DEVICE_ENUM_UINT32 enDevice);
extern VOS_VOID AUDIO_ENHANCE_ProcEcRef( VOS_UINT32 uwSampleRate );
extern VOS_UINT32  AUDIO_ENHANCE_GetCurrentModule( VOS_VOID );


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of audio_voip.h */
