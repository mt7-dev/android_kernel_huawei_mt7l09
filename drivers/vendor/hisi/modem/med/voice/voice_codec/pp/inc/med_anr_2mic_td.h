

#ifndef __MED_2MIC_TD_H__
#define __MED_2MIC_TD_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "med_anr_2mic_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_2MIC_CALIB_FLOOR_MIN_POWER            (1200)                        /* 电路噪声能量 */
#define MED_2MIC_CALIB_GAIN_DELTA_MAX             (3277)                        /* 增益调整步长最大值 int16(0.2 * 2^14) */
#define MED_2MIC_CALIB_GAIN_MAX                   (32767)                       /* 校准最大增益 int16(2 * 2^14) */
#define MED_2MIC_CALIB_GAIN_MIN                   (8192)                        /* 校准最小增益 int16(0.5 * 2^14) */
#define MED_2MIC_CALIB_ALPHA                      (32112)                       /* 校准增益平滑系数 int16(0.98 * 2^15) */

#define MED_2MIC_SPACESPLIT_MAX_DELAY             (15)                          /* 最大延迟 注意:该值 应大于等于 MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH -1 */
#define MED_2MIC_SPACESPLIT_MC_DELAY              (768)                         /* int16(3 * 2^8) */
#define MED_2MIC_SPACESPLIT_MC_NE_GAIN            (13472)                       /* 主麦克增益 */
#define MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH       (16)                          /* 分数延迟滤波器阶数 注意:该值-1 应小于等于 MED_2MIC_SPACESPLIT_MAX_DELAY */

#define MED_2MIC_ANC_VAD_PS_PN_RATIO_INIT         (4096)                        /* vad检测主辅麦克功率比初始值 */
#define MED_2MIC_ANC_VAD_FILTER_ORDER             (3)                           /* 滤波器阶数 */
#define MED_2MIC_ANR_VAD_ALPHA                    (16384)                       /* 双麦克VAD检测平滑系数,计算方法0.5 * 32767 */
#define MED_2MIC_ANR_OUTPUT_MAX_LEN               (640)                         /* ANR模块输出数据最大帧长，还应用于处理过程中开辟的局部变量 */

#define MED_2MIC_LAF_FILTER_ORDER_NB              (32)                          /* LAF窄带滤波器阶数 */
#define MED_2MIC_LAF_FILTER_ORDER_WB              (64)                          /* LAF宽带滤波器阶数 */
#define MED_2MIC_LAF_WEIGHT_UPDATE_LEN            (8)                           /* LAF滤波器系数更新频率(每8点更新一次)*/
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_NB    (15000)                       /* LAF滤波系数更新短时能量阈值 NB */
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_WB    (50000)                       /* LAF滤波系数更新短时能量阈值 WB */
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_ALPHA     (32767)                       /* LAF滤波系数更新短时能量计算平滑系数 int16(1 * 2^15)*/
#define MED_2MIC_LAF_RC_LEN_MAX                   (640)                         /*  2*(CODEC_SAMPLE_RATE_MODE_16000/50) */


#define MED_2MIC_LAF_STEP_INIT                    (3277)                        /* LAF步长初始值 int16(0.1 * 2^15) */
#define MED_2MIC_LAF_STEP_REF_POWER_THD           (500000)                      /* LAF步长更新能量阈值 */

#define MED_2MIC_ANC_MC_MAX_DELAY                 (64)                          /* ANC主Mic最大延迟 */
#define MED_2MIC_MAX_Q14                          (16384)                       /* 2^14 */

#define MED_2MIC_MIN_POWER_VALUE                  (100)

#define MED_2MIC_PSD_NORMSHIFT_8                  (256)                         /* 2^8 */

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdProcessPtr
 功能描述  : 获取TD模块结构体指针
*****************************************************************************/

#define MED_2MIC_GetTdProcessPtr()          (&g_stTdProcess)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdCalibPtr
 功能描述  : 获取校准模块结构体指针
*****************************************************************************/

#define MED_2MIC_GetTdCalibPtr()            (&g_stTdProcess.stCalib)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdSpaceSplitPtr
 功能描述  : 获取空间分离模块结构体指针
*****************************************************************************/

#define MED_2MIC_GetTdSpaceSplitPtr()       (&g_stTdProcess.stSpaceSplit)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdAncPtr
 功能描述  : 获取自适应滤波模块结构体指针
*****************************************************************************/

#define MED_2MIC_GetTdAncPtr()              (&g_stTdProcess.stAnc)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdVadPtr
 功能描述  : 获取VAD检测模块结构体指针
*****************************************************************************/

#define MED_2MIC_GetTdVadPtr()              (&g_stTdProcess.stAnc.stVad)

/*****************************************************************************
 宏名称    : MED_2MIC_SetTdSampleRate
 功能描述  : 设置VAD模块采样率
*****************************************************************************/

#define MED_2MIC_SetTdSampleRate(enSmpRate)     (g_stTdProcess.stAnc.stVad.enSampleRate = (enSmpRate))

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdSampleRate
 功能描述  : 获取VAD模块采样率
*****************************************************************************/

#define MED_2MIC_GetTdSampleRate()          (g_stTdProcess.stAnc.stVad.enSampleRate)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdFilterNumPtr
 功能描述  : 获取滤波器系数数组指针
*****************************************************************************/

#define MED_2MIC_GetTdFilterNumPtr()           (g_ashwFilterNum)

/*****************************************************************************
 宏名称    : MED_2MIC_GetTdFilterNumPtr
 功能描述  : 获取滤波器系数数组指针
*****************************************************************************/

#define MED_2MIC_GetTdFilterDenPtr()           (g_ashwFilterDen)

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
 实体名称  : MED_2MIC_CALIB_STRU
 功能描述  : 描述2MIC校准模块的结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16          enCalibEnable;                              /* 2MIC校准模块使能开关 */
    VOS_INT16                       shwLastGain;                                /* 上一帧的校准增益*/
} MED_2MIC_CALIB_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_SPACESPLIT_MC_STRU
 功能描述  : 描述2MIC空间分离主Mic的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                       ashwMainState[MED_2MIC_SPACESPLIT_MAX_DELAY];/* 主MiC历史信息 */
    VOS_INT16                       shwDelayValue;                               /* 主Mic延迟长度 */
} MED_2MIC_SPACESPLIT_MC_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_SPACESPLIT_RC_STRU
 功能描述  : 描述2MIC空间分离辅Mic的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                       ashwMainState[MED_2MIC_SPACESPLIT_MAX_DELAY];   /* 辅Mic空间分离的主Mic历史信息 */
    VOS_INT16                       ashwRefState[MED_2MIC_SPACESPLIT_MAX_DELAY];    /* 辅Mic空间分离的辅Mic历史信息 */
    VOS_INT16                       shwDelayValue;                                  /* 辅Mic延迟长度 */
    VOS_INT16                       shwNeGain;                                      /* 延迟增益 */
} MED_2MIC_SPACESPLIT_RC_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_SPACESPLIT_RC_STRU
 功能描述  : 描述2MIC空间分离的结构体
*****************************************************************************/
typedef struct
{
    MED_2MIC_SPACESPLIT_MC_STRU     stMainNormMode;                             /* 空间分离主Mic结构体 */
    MED_2MIC_SPACESPLIT_RC_STRU     stRefMode;                                  /* 空间分离辅Mic结构体 */
} MED_2MIC_SPACESPLIT_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_VAD_WB_STRU
 功能描述  : 双Mic 宽带信号的VAD检测结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                       shwFilterOrder;                             /* 滤波器阶数 */
    VOS_INT16                       shwReserve;
    VOS_INT16                      *pshwFilterNum;                              /* 滤波器系数 */
    VOS_INT16                      *pshwFilterDen;                              /* 滤波器系数 */
    VOS_INT16                       ashwMcXState[MED_2MIC_ANC_VAD_FILTER_ORDER];        /* 状态信息 */
    VOS_INT16                       ashwMcYState[MED_2MIC_ANC_VAD_FILTER_ORDER];
    VOS_INT16                       ashwRcXState[MED_2MIC_ANC_VAD_FILTER_ORDER];
    VOS_INT16                       ashwRcYState[MED_2MIC_ANC_VAD_FILTER_ORDER];
} MED_2MIC_VAD_WB_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_VAD_STRU
 功能描述  : 双Mic VAD检测结构体
*****************************************************************************/
typedef struct
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate;                               /* 采样率 */
    VOS_INT16                       shwVadRatioThd;                             /* VAD判断阈值 */
    VOS_INT16                       shwPsPnRatio;                               /* 信号与噪声的功率谱比值 */
    MED_2MIC_VAD_WB_STRU            stVadWb;
} MED_2MIC_VAD_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_LAF_STRU
 功能描述  : 双Mic LAF滤波结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                       shwN;                                       /* LAF滤波器长度 */
    VOS_INT16                       ashwWeight[MED_2MIC_LAF_FILTER_ORDER_WB];   /* LAF滤波器系数 */
    VOS_INT16                       shwStep;                                    /* 学习步长 */
    VOS_INT16                       ashwRcBuf[2*CODEC_FRAME_LENGTH_WB];         /* 辅麦克临时Buff */
    VOS_INT32                       swInShortPower;                             /* 短时能量 */
    VOS_INT16                       shwAlpha;                                   /* 短时能量平滑系数 */
    VOS_INT16                       shwMaxFar;                                  /* 参考信号最大幅度 */
    VOS_INT16                       shwFarMaxThd;                               /* 远端信号门限，低于此门限则进行LAF滤波 */
    VOS_INT16                       shwReserve;                                 /* 参考信号最大幅度 */
    VOS_INT32                       swInShortThd;                               /* 噪声判断短时能量阈值 */
    VOS_INT32                       swRefPowerThd;                              /* 辅麦克平均能量步长更新阈值 */
} MED_2MIC_LAF_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_ANC_STRU
 功能描述  : 双Mic 空间滤波结构体
*****************************************************************************/
typedef struct
{
    MED_2MIC_VAD_STRU               stVad;                                        /* 双Mic VAD检测结构体 */
    MED_2MIC_LAF_STRU               stLaf;                                        /* 双Mic LAF滤波结构体 */
    VOS_INT16                       shwMainDelayQ;                                /* 主麦克延迟 */
    VOS_INT16                       shwReserve;
    VOS_INT16                       ashwMainDelayState[MED_2MIC_ANC_MC_MAX_DELAY];/* 主麦克历史状态 */
} MED_2MIC_ANC_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_TDPROCESS_STRU
 功能描述  : 描述2MIC TD流程控制结构体
*****************************************************************************/
typedef struct
{
    MED_2MIC_CALIB_STRU             stCalib;                                    /* 2MIC校准模块结构体 */
    MED_2MIC_SPACESPLIT_STRU        stSpaceSplit;                               /* 2MIC空间分离模块结构体*/
    MED_2MIC_ANC_STRU               stAnc;                                      /* 2MIC自适应滤波模块结构体*/
}MED_2MIC_TDPROCESS_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_2MIC_TDPROCESS_STRU  g_stTdProcess;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_INT16 MED_ANR_2MIC_FirConvolute(
                       VOS_INT16               *pshwWeight,
                       VOS_INT16               *pshwRcIn,
                       VOS_INT16                shwLen );
extern VOS_UINT32 MED_ANR_2MIC_TdAncInit(
                       VOS_INT16                           shwVadRatioThld,
                       VOS_INT16                           shwFarMaxThd,
                       MED_2MIC_ANC_STRU                  *pstAnc);
extern VOS_UINT32 MED_ANR_2MIC_TdAncNormalMode(
                       MED_2MIC_ANC_STRU                       *pstAnc,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwRcIn,
                       VOS_INT16                               *pshwCalib,
                       VOS_INT16                               *pshwMcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdBlockMatrix(
                       MED_2MIC_SPACESPLIT_RC_STRU     *pstRefMode,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwRcOut);
extern VOS_INT32 MED_ANR_2MIC_TdCalcMeanPower(
                       VOS_INT16                       *pshwMicIn,
                       VOS_INT16                        shwFrameLength);
extern VOS_UINT32 MED_ANR_2MIC_TdCalib(
                       CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                       MED_2MIC_CALIB_STRU             *pstCalib,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdCalibInit(
                       CODEC_SWITCH_ENUM_UINT16       enEnable,
                       MED_2MIC_CALIB_STRU         *pstCalib);
extern VOS_UINT32 MED_ANR_2MIC_TdIIRLowPass(
                       MED_2MIC_VAD_WB_STRU                    *pstVadWb,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwMcXState,
                       VOS_INT16                               *pshwMcYState,
                       VOS_INT32                               *pswPowerMean);
extern VOS_UINT32 MED_ANR_2MIC_TdInit(
                       MED_2MIC_NV_STRU                    *pst2MICParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate);
extern VOS_UINT32 MED_ANR_2MIC_TdLaf(
                           MED_2MIC_LAF_STRU                       *pstLaf,
                           VOS_INT16                               *pshwMcIn,
                           VOS_INT16                               *pshwRcIn,
                           VOS_INT16                                shwVadFlag,
                           VOS_INT16                               *pshwMcOut);
extern CODEC_VAD_FLAG_ENUM_INT16 MED_ANR_2MIC_TdNoiseVadDetect(
                       MED_2MIC_VAD_STRU                       *pstVad,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwRcIn);
extern VOS_UINT32 MED_ANR_2MIC_TdProcess(
                       CODEC_SWITCH_ENUM_UINT16           enEnable,
                       CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut,
                       VOS_INT16                       *pshwRcCalib);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                        shwDelayValue,
                       VOS_INT16                       *pshwMainState,
                       VOS_INT16                       *pshwRefState,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceIntegerDelay(
                       VOS_INT16                       *pshwIn,
                       VOS_INT16                        shwDelay,
                       VOS_INT16                       *pshwState,
                       VOS_INT16                       *pshwOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceSplit(
                       MED_2MIC_SPACESPLIT_STRU        *pstSpaceSplit,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceSplitInit(
                       VOS_INT16                           shwRcDelay,
                       MED_2MIC_SPACESPLIT_STRU            *pstSpaceSplit);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_2mic_td.h */
