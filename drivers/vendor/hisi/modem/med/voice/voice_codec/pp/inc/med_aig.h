
#ifndef __MED_AIG_H__
#define __MED_AIG_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 基本信息 */
#define MED_AIG_MAX_OBJ_NUM             (2)                                     /* AIG最大实体数目 */
#define MED_AIG_PARAM_LEN               (21)                                    /* 有效的配置项个数 */
#define MED_AIG_RX_0_DBFS               (32767)
#define MED_AIG_ALPHA_NORM              (128)
#define MED_AIG_SLOP_NORM               (128)
#define MED_AIG_GAIN_30DB_NORM          (7706)

/* 获取全局变量 */
#define MED_AIG_GetObjPtr               (&g_astMedAigObjPool[0])                /* 获取内存块全局变量地址 */
#define MED_AIG_GetObjInfoPtr           (&g_stMedAigObjInfo)                    /* 获取内存块信息的全局变量地址 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

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
实体名称  : MED_AIG_NV_STRU
功能描述  : AIG NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 配置项 使能项 */
    VOS_INT16                           shwDnCompThr;                       /* 下压缩区门限（大于此门限为下压缩区），单位为dBfs */
    VOS_INT16                           shwUpCompThr;                       /* 上压缩区门限（小于此门限为上压缩区），单位为dBfs */
    VOS_INT16                           shwDnExpThr;                        /* 下扩展区门限（小于此门限为下扩展区），单位为dBfs */
    VOS_INT16                           shwDnCompSlop;                      /* 下压缩区增益曲线斜率 */
    VOS_INT16                           shwUpCompSlop;                      /* 上压缩区增益曲线斜率 */
    VOS_INT16                           shwDnExpSlop;                       /* 下扩展区增益曲线斜率 */
    VOS_INT16                           shwMindB;                           /* 最小dB值 */
    VOS_INT16                           shwGainAlphaattack;
    VOS_INT16                           shwGainAlpharelease;                /* 增益alpha滤波系数 */
    VOS_INT16                           shwAlphaThd;                        /* alpha门限值 */
    VOS_INT16                           shwLastPowerIndBfsInit;             /*上一帧输入功率初始值 */
    VOS_INT16                           shwLastPowerOutdBfsInit;            /*上一帧输出功率初始值 */
    VOS_INT16                           shwGainMindB;                       /* 增益最小dB值 */
    VOS_INT16                           shwLastGainInit;                    /*上一帧增益初始值 */
    VOS_INT16                           shwMaxIn;                           /* 最大输入值 */
    VOS_INT16                           shwExpectMax;                       /* 期望最大值 */
    VOS_INT16                           shwMaxGainDelta;                    /* 最大增益Delta系数 */
    VOS_INT16                           shwSpeechCntInit;
    VOS_INT16                           shwVADholdlenth;
    VOS_INT16                           shwMakeupGain;
    VOS_INT16                           shwReserved;
}MED_AIG_NV_STRU;

/*****************************************************************************
实体名称  : MED_AIG_STRU
功能描述  : AIG项维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AIG_NV_STRU                     stNv;                                   /* NV项结构体 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           shwGainMaxRefThd;                       /* 最大增益参考阈值 */
    VOS_INT16                           shwGainMax;                             /* 最大增益值 */
    VOS_INT16                           shwMaxIn;                               /* 输入最大值 */
    VOS_INT16                           shwSpeechCnt;
    VOS_INT16                           shwLastPowerIndBfs;                     /* 上一帧输入功率dB值 */
    VOS_INT16                           shwLastGain;                            /* 上一帧增益 */
    VOS_INT16                           shwLastGaindB;                          /* 上一帧增益 */
    VOS_INT16                           shwLastPowerOutdBfs;                    /* 上一帧输出功率dB值 */
    VOS_INT16                           swdBfs0;                                /* DRC的RMS增益折线和输出信号限幅门限的0dBfs */
    VOS_INT16                           shwMakeupGain;
}MED_AIG_OBJ_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_AIG_OBJ_STRU    g_astMedAigObjPool[MED_AIG_MAX_OBJ_NUM];             /* AIG实体资源池 */
extern MED_OBJ_INFO        g_stMedAigObjInfo;                                   /* AIG信息 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID* MED_AIG_Create(MED_VOID);
extern VOS_UINT32 MED_AIG_Destroy(MED_VOID **ppstObj);
extern VOS_UINT32 MED_AIG_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AIG_SetPara (
                MED_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen,
                VOS_INT32               swSampleRate);
extern VOS_UINT32 MED_AIG_Main(
                 MED_AIG_OBJ_STRU    *pstAig,
                 VOS_INT16           *pshwIn,
                 VOS_INT16           *pshwRef,
                 VOS_INT16           *pshwOut,
                 VOS_INT16            shwVadFlag);
extern VOS_VOID MED_AIG_PowerCal(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwPowerRMS,
                VOS_INT16               *pshwNormShift,
                VOS_INT16                shwFrameLen);
extern VOS_VOID MED_AIG_GainCal(
                MED_AIG_OBJ_STRU        *pstAig,
                VOS_INT16                shwPowerRMS,
                VOS_INT16                shwNormShift,
                VOS_INT16                shwPowerRMSRef,
                VOS_INT16                shwNormShiftRef,
                VOS_INT16               *pshwGainAIG,
                VOS_INT16               *pshwAlpha);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_agc.h */
