

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"

#ifndef __MED_AVC_H__
#define __MED_AVC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 基本信息 */
#define MED_AVC_MAX_OBJ_NUM             (1)                                     /* AVC最大实体数目 */
#define MED_AVC_PARAM_LEN               (9)                                     /* AVC有效的参数长度 */
#define MED_AVC_BUF_LEN                 (6)                                     /* 缓存的VAD信息长度 */
#define MED_AVC_NOISE_UPDATE_MAX_VOICE_ACTIVE (1)                               /* 噪声更新时最大允许语音激活状态数 */
#define MED_AVC_NOISE_UPDATE_ALPHA      (31456)                                 /* 噪声估计平滑系数 */
#define MED_AVC_NOISE_STEPS_NUM         (4)                                     /* 噪声级别数 */
#define MED_AVC_GAIN_UPDATE_ALPHA       (31456)                                 /* 增益平滑系数 */
#define MED_AVC_ANTI_SAT_ALPHA          (2949)                                  /* 防饱和处理平滑系数 */
#define MED_AVC_GAIN_INITIAL            (2048)                                  /* 增益初始化的值 */
#define MED_AVC_NOISE_LVL_INITIAL       (100)                                   /* 估计的噪声能量 */
#define MED_AVC_RX_DEV_GAIN_MIN         (4096)                                  /* 下行设备增益最小值 */
#define MED_AVC_GAIN_MIN                (2048)                                  /* 增益最小值 */

/* 获取全局变量 */
#define MED_AVC_GetNoiseStepLag(shwIdx) (g_ashwMedAvcStepLag[shwIdx])           /* 返回噪声分级迟滞 */
#define MED_AVC_GetGainFromTab(shwIdx)  (g_ashwMedAvcGainTab[shwIdx])           /* 根据索引查找增益对照表中的值 */
#define MED_AVC_GetObjPtr               (&g_astMedAvcObjPool[0])                /* 获取内存块全局变量地址 */
#define MED_AVC_GetObjInfoPtr           (&g_stMedAvcObjInfo)                    /* 获取内存块信息的全局变量地址 */

/* 函数、宏转定义 */
#define MED_AVC_Db2Linear(shwIdx)       MED_GAIN_Db2Linear(shwIdx)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : MED_AVC_FFT_LEN_ENUM
 功能描述  : AVC FFT长度枚举定义
*****************************************************************************/
enum MED_AVC_FFT_LEN_ENUM
{
    MED_AVC_FFT_LEN_8K                  = 256,
    MED_AVC_FFT_LEN_2MIC_8K             = 320,
    MED_AVC_FFT_LEN_16K                 = 512,
    MED_AVC_FFT_LEN_2MIC_16K            = 640,
    MED_AVC_FFT_LEN_BUTT
};
typedef VOS_UINT16 MED_AVC_FFT_LEN_ENUM_UINT16;

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
实体名称  : MED_AVC_NV_STRU
功能描述  : AVC NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enEnable;                               /* 配置项 使能开关 */
    VOS_INT16                           shwGainMaxInc;                          /* 配置项 最大上升 Q14 */
    VOS_INT16                           shwGainMaxDec;                          /* 配置项 最大下降 Q14 */
    VOS_INT16                           shwSensitivityOffset;                   /* 配置项 上行麦克风灵敏度偏置 */
    VOS_INT16                           shwAntiSatMaxLevel;                     /* 配置项 抗饱和处理最大增益 */
    VOS_INT16                           ashwNoiseStep[MED_AVC_NOISE_STEPS_NUM]; /* 配置项 噪声级别数 */
    VOS_INT16                           shwReserved;
}MED_AVC_NV_STRU;

/*****************************************************************************
实体名称  : MED_AVC_STRU
功能描述  : AVC维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AVC_NV_STRU                     stNv;                                   /* NV项 */

    /* 其他运算参数 */
    VOS_INT32                           swNoiseLvl;                             /* 当前噪声级别 */
    VOS_INT16                          *pshwWeightTab;                          /* 听觉加权表指针 */
    VOS_INT16                           shwFrmLen;                              /* 帧长 */
    VOS_INT16                           shwReserved2;

    /* 噪声分级,Q0 */
    VOS_INT32                           aswMedAvcStep[MED_AVC_NOISE_STEPS_NUM];

    /* 历史信息缓存维护 */
    CODEC_VAD_FLAG_ENUM_INT16        aenVadFlag[MED_AVC_BUF_LEN];            /* VAD历史信息的缓存 */
    VOS_INT32                           aswFrmLvl[MED_AVC_BUF_LEN];             /* 麦克风偏置修正后的帧能量的缓存 */

    /* 上一帧信息维护*/
    VOS_INT16                           shwLastStep;                            /* 目标增益级别 */
    VOS_INT16                           shwLastGain;                            /* 增益，Q11 */

    /* 上行信息维护 */
    VOS_INT32                           swTxFrmLvl;                             /* 上行帧信号的能量 */
    CODEC_VAD_FLAG_ENUM_INT16        enVadFlag;                              /* 上行帧的VAD信息 */
    MED_AVC_FFT_LEN_ENUM_UINT16         enFreqLen;                              /* 上行帧频域长度 */
} MED_AVC_OBJ_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOS_INT16 g_ashwMedAvcWeightTab8k[];
extern VOS_INT16 g_ashwMedAvcWeightTab16k[];

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID* MED_AVC_Create(VOS_VOID);
extern VOS_UINT32 MED_AVC_Destroy(VOS_VOID **ppstObj);
extern VOS_VOID MED_AVC_EstBgNoise(MED_AVC_OBJ_STRU *pstAvc);
extern VOS_VOID MED_AVC_GainRxVol(
                       MED_AVC_OBJ_STRU       *pstAvc,
                       VOS_INT16              *pshwRxFrm,
                       VOS_INT16               shwRxDevGain);
extern VOS_UINT32 MED_AVC_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AVC_Main(
                       VOS_VOID                *pstMedAvcObj,
                       VOS_INT16               *pshwRxFrm,
                       VOS_INT16                shwRxDevGain);
extern VOS_UINT32 MED_AVC_SetPara (
                       VOS_VOID                         *pstInstance,
                       VOS_INT16                        *pshwParam,
                       VOS_INT16                        shwParaLen,
                       MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum);
extern VOS_VOID MED_AVC_UpdateTx(
                       MED_AVC_OBJ_STRU       *pstAvc,
                       VOS_INT16              *pshwTxFreq,
                       VOS_INT16               enVadFlag);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_avc.h */

