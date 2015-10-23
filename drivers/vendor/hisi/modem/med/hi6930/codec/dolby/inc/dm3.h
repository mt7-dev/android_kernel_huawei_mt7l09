/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xa_dm3.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年12月20日
  最近修改   :
  功能描述   : xa_dm3.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   :
    作    者   : twx144307
    修改内容   : 创建文件

******************************************************************************/
#ifndef __XA_DM3_H__
#define __XA_DM3_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "xa_apicmd_standards.h"
#include  "xa_error_handler.h"
#include  "xa_error_standards.h"
#include  "xa_memory_standards.h"
#include  "xa_type_def.h"
#include  "xa_dm3plus_ak_pp_api.h"

#include  "xa_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 双声道 */
#define AUDIO_PCM_CHANNEL_NUM           (2)
#define AUDIO_PCM_BYTES_PER_SAMPLE      (2)

//#define AUDIO_EFFECT_MAX_CHANNLE_NUM    (8)
#define AUDIO_EFFECT_MAX_SAMPLE_RATE    (48000)

/* Number of samples to process per exec call (must be multiple of 32). */
#define DM3_N_SAMPLES                   (512)

/* 图形均衡(Graphic EQ)最大增益级数 */
#define AUDIO_EFFECT_GEQ_MAX_GAIN_SCALE (48)

/* 图形均衡(Graphic EQ)最大增益级数 */
#define AUDIO_EFFECT_GEQ_MIN_GAIN_SCALE (-48)

/* 位宽信息 */
#define AUDIO_EFFECT_PCM_BIT_WIDTH      (16)

#define AUDIO_EFFECT_GetDm3HeadphoneCfgPtr() (&g_stDm3HeadphoneParams)
#define AUDIO_EFFECT_GetDm3SpkCfgPtr()       (&g_stDm3SpkParams)
#define AUDIO_EFFECT_GetDm3MemCfgPtr()       (&g_stDm3MemoryCfg)
#define AUDIO_EFFECT_GetDtsMemCfgPtr()       (&g_stDtsMemoryCfg)
#define AUDIO_EFFECT_GetDm3ApiPtr()          (g_pstDm3ApiObj)

/* 输入输出采样率保持不变 */
#define AUDIO_EFFECT_DM3_IO_SAMP_EQU    (1)

/* DM3中输入输出采样率变换功能关闭 */
#define AUDIO_EFFECT_DM3_UPSAMP_DISABLE (0)

/* 参数已经初始化 */
#define AUDIO_EFFECT_PARA_INITIALIZED   (2)

/* 只有参数需要初始化，内存不需要重新分配 */
#define AUDIO_EFFECT_PARA_RESET_ONLY    (1)

/* 参数没有初始化，需要重新分配内存空间并初始化 */
#define AUDIO_EFFECT_PARA_UNINITIALIZED (0)

/* 当前DM3音效库需要初始化 */
#define AUDIO_EFFECT_GetDm3InitStatus()       (g_uwDm3InitStatus)
#define AUDIO_EFFECT_SetDm3InitStatus(uwPara) (g_uwDm3InitStatus = (uwPara))

#define AUDIO_EFFECT_GetUserCfgPtr()          (&g_stAudioEffectUserCfg)

#define AUDIO_EFFECT_Dm3ApiValid        (0)
#define AUDIO_EFFECT_Dm3ApiInvalid      (1)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 设置音效参数 */
enum AUDIO_EFFECT_DM3_CHAN_ORDER_ENUM
{
    AUDIO_EFFECT_DM3_CHANS_ORDER_C   = 1,      /* 单声道 */
    AUDIO_EFFECT_DM3_CHANS_ORDER_L_R = 2,      /* 双声道 */
    AUDIO_EFFECT_DM3_CHANS_ORDER_BUT
};
typedef VOS_UINT16 AUDIO_EFFECT_DM3_CHAN_ORDER_ENUM_UINT16;


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
 实体名称  : AUDIO_EFFECT_VALUE_STRU
 功能描述  : 配置数据结构定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32  uwIsSetted;
    VOS_INT32   swValue;
} AUDIO_EFFECT_VALUE_STRU;

/*****************************************************************************
 实体名称  : AUDIO_EFFECT_DOBLY_PARAMS
 功能描述  : DOBLY音效配置参数
*****************************************************************************/
typedef struct
{
    VOS_INT32 swInMatrix;
    VOS_INT32 swLfeMixLevel;

    VOS_INT32 swByPass; /* 0 disable, 1 soft mode, 2 hard mode*/

    /* 单声道2双声道 */
    VOS_INT32 swM2sEnable;
    VOS_INT32 swM2sDetecotr;/* tensilca 默认开启 */

    /* 音域增强 */
    VOS_INT32 swSSEEnable;
    VOS_INT32 swSSEWidth;
    VOS_INT32 swSSESpkMode;

    /* 声音级别控制模块 */
    VOS_INT32 swSlcEnable;
    VOS_INT32 swSlcLevel;
    VOS_INT32 swSlcDepth;

    /* 低音模块 */
    VOS_INT32 swNBEnable;
    VOS_INT32 swNBCutoff;
    VOS_INT32 swNBBoost;
    VOS_INT32 swNBLevel;

    /* 均衡模块 */
    VOS_INT32 swGeqEnable;
    VOS_INT32 swGeqNbands;
    VOS_INT32 swGeqPreamp;
    VOS_INT32 swGeqMaxBoost;
    VOS_INT32 aswGeqBands[GEQ_N_BANDS_MAX];

    /* 移动环绕 */
    VOS_INT32 swMsrMaxProfile;
    VOS_INT32 swMsrEnable;
    VOS_INT32 swMsrRoot;
    VOS_INT32 swMsrBright;

    /* 高频增强模块 */
    VOS_INT32 swHfeEnable;
    VOS_INT32 swHfeDepth;

    /* Speaker Equalizer Module */
    VOS_INT32 swSpkEQEnable;
    VOS_INT32 swSpkEQChgainDB1;
    VOS_INT32 swSpkEQChgainDB2;

    /* XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE */
    VOS_INT32 swPortableMode;
    VOS_INT32 swPortableModeGain;
    VOS_INT32 swPortableModeOrl;

} AUDIO_EFFECT_DOBLY_PARAMS_STRU;
/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern xa_codec_handle_t                g_pstDm3ApiObj;
extern AUDIO_EFFECT_DOBLY_PARAMS_STRU   g_stDm3HeadphoneParams; /* dolby headphone音效配置全局数据,不能初始化 */
extern AUDIO_EFFECT_DOBLY_PARAMS_STRU   g_stDm3SpkParams;
extern XA_COMM_MEMORY_CFG_STRU          g_stDm3MemoryCfg;       /* 内存管理 */
extern XA_COMM_MEMORY_CFG_STRU          g_stDtsMemoryCfg;       /* 内存管理 */
extern VOS_UINT32                       g_uwDm3InitStatus;
extern XA_AUDIO_EFFECT_USER_CFG_STRU    g_stAudioEffectUserCfg;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_INT32 AUDIO_EFFECT_CharArrayToInt16(VOS_CHAR cLb, VOS_CHAR cHb);
extern VOS_INT32 AUDIO_EFFECT_CharArrayToInt32(VOS_CHAR* cBa);
extern VOS_VOID AUDIO_EFFECT_Dm3Close(VOS_VOID);
extern VOS_UINT32 AUDIO_EFFECT_Dm3Execute(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg);
extern VOS_UINT32  AUDIO_EFFECT_Dm3Init(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg);
extern VOS_VOID AUDIO_EFFECT_Dm3ParseParams(VOS_INT32 swParamId, VOS_INT32 swValue, AUDIO_EFFECT_DOBLY_PARAMS_STRU* pstDoblyParams);
extern VOS_UINT32 AUDIO_EFFECT_Dm3SetPara(AUDIO_EFFECT_DOBLY_PARAMS_STRU* pstDoblyParams);
extern VOS_UINT32 AUDIO_EFFECT_Execute(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg);
extern VOS_UINT32 AUDIO_EFFECT_MsgSetEnableReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 AUDIO_EFFECT_MsgSetParamReq(VOS_VOID *pvOsaMsg);
extern VOS_VOID AUDIO_EFFECT_Dm3SetParaAsDefault(AUDIO_EFFECT_DOBLY_PARAMS_STRU *pstPara);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of audio_pcm.h */




