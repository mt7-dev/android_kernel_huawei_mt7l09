

#ifndef _MED_AEC_AF_H_
#define _MED_AEC_AF_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "med_aec_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* AF 宏定义*/
#define MED_AEC_AF_FILT_NB              (320)                                   /* 窄带MDF滤波块长度 */
#define MED_AEC_AF_FILT_WB              (640)                                   /* 宽带MDF滤波块长度 */
#define MED_AEC_AF_M_MAX                (3)                                     /* MDF滤波最大块数, 最大支持44ms滤波长度 */
#define MED_AEC_AF_POWX_DITH            (10)                                    /* 远端信号能量平滑扰动 */
#define MED_AEC_AF_XPOW_THR             (11469)                                 /* 远端信号功率谱平滑因子 */
#define MED_AEC_AF_RESET_THR            ((VOS_INT32)1000000)                    /* AF复位判决阈值 */
#define MED_AEC_AF_RESET_NUM            (50)                                    /* AF复位判决次数阈值 */
#define MED_AEC_AF_WEIGHT_POWER_THD     (8)

/* 滤波器权值更新 */
#define MED_AEC_AF_W_QN_FILT            (12)                                    /* Filter函数中W 原始定标精度 */
#define MED_AEC_AF_W_QN_PROP            (18)                                    /* UpdateProp函数中W 原始定标精度 */


/* 计算泄漏率的系数 */
#define MED_AEC_AF_LEAK_BETA0           (1311)                                  /* (2.0*frame_size)/sampling_rate */
#define MED_AEC_AF_LEAK_BETA_MAX        (328)                                   /* (0.5*frame_size)/sampling_rate */
#define MED_AEC_AF_LEAK_SMOOTH_ALPHA    (655)                                   /* 计算残差与回波估计均值的平滑系数 */
#define MED_AEC_AF_LEAK_MIN_LEAK        (164)                                   /* 泄漏因子最小值 */
#define MED_AEC_AF_LEAK_THR             (983)                                   /* 泄漏因子阈值 */

/* 对比两个滤波器采用两种时间窗，对应系数 */
#define MED_AEC_AF_DAVG1_COEF1          (19661)                                 /* Davg1滤波系数1，0.6 Q15 */
#define MED_AEC_AF_DAVG1_COEF2          (13107)                                 /* Davg1滤波系数2，0.4 Q15 */
#define MED_AEC_AF_DAVG2_COEF1          (27853)                                 /* Davg2滤波系数1，0.85 Q15 */
#define MED_AEC_AF_DAVG2_COEF2          (4915)                                  /* Davg2滤波系数1，0.15 Q15 */
#define MED_AEC_AF_DVAR1_COEF1_M        (11796)                                 /* Dvar1滤波系数1，0.36 Q15 */
#define MED_AEC_AF_DVAR1_COEF1_E        (-15)
#define MED_AEC_AF_DVAR1_COEF2          (5243)                                  /* Dvar1滤波系数2，0.16 Q15 */
#define MED_AEC_AF_DVAR2_COEF1_M        (23675)                                 /* Dvar2滤波系数1，0.7225 Q15 */
#define MED_AEC_AF_DVAR2_COEF1_E        (-15)
#define MED_AEC_AF_DVAR2_COEF2          (737)                                   /* Dvar2滤波系数1，0.0225 Q15 */
#define MED_AEC_AF_VAR1_THD_M           (16384)                                 /* 双滤波器选择阈值1 0.5 Q15 */
#define MED_AEC_AF_VAR1_THD_E           (-15)
#define MED_AEC_AF_VAR2_THD_M           (16384)                                 /* 双滤波器选择阈值2 0.25 Q15 */
#define MED_AEC_AF_VAR2_THD_E           (-16)
#define MED_AEC_AF_BACKTRACK_THD_M      (16384)                                 /* 双滤波器选择阈值3 4 Q15 */
#define MED_AEC_AF_BACKTRACK_THD_E      (-12)
#define MED_AEC_AF_DBF_FLOOR            (10)                                    /* 差方和能量扰动 */

/* 计算步长 */
#define MED_AEC_AF_STEP_COEF            (26214)                                 /* 滤波器步长平滑系数 */
#define MED_AEC_AF_STEP_COEF_N          (6554)                                  /* 滤波器步长平滑系数 */
#define MED_AEC_AF_STEP_QN              (27)                                    /* 滤波器步长定标位数 */
#define MED_AEC_AF_STEP_TMP_COFE        (8192)                                  /* 临时步长系数 0.25 */

/* 数值 */
#define MED_AEC_AF_0_POINT_1_Q15        (3277)                                  /* 0.1   Q15 */
#define MED_AEC_AF_0_POINT_99_Q15       (32440)                                 /* 0.99  Q15 */
#define MED_AEC_AF_10000                (10000)                                 /* 10000 */
#define MED_AEC_AF_1000                 (1000)                                  /* 1000 */
#define MED_AEC_AF_100                  (100)                                   /* 100 */
#define MED_AEC_AF_DIV_DITH             (10)                                    /* 扰动 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 滤波器对比结果 */
enum MED_AEC_AF_CMP_ENUM
{
    MED_AEC_AF_CMP_SOSO,                                                        /* 滤波更新还行 */
    MED_AEC_AF_CMP_OK,                                                          /* 滤波更新很好 */
    MED_AEC_AF_CMP_BAD,                                                         /* 滤波更新很差 */
    MED_AEC_AF_CMP_BUTT
};
typedef VOS_UINT16  MED_AEC_AF_CMP_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
 /* 延迟补偿数据实体 */
 typedef struct
 {
     VOS_INT16                          shwOffsetLen;                           /* 固定延迟偏移长度, 单位样点数, 0-480 */
     VOS_INT16                          shwReserve;
     VOS_INT16                          ashwOffsetBuf[MED_AEC_MAX_OFFSET + CODEC_FRAME_LENGTH_WB]; /* 延迟缓存 */
 } MED_AEC_OFFSET_OBJ_STRU;

/* AF配置结构体 */
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enAfEnable;
    VOS_INT16                           shwTailLen;                             /*回声尾端长度, 单位样点数*/
}MED_AEC_AF_CONFIG_STRU;

/* 前后滤波器比较状态 */
typedef struct
{
    VOS_INT32                           swDiffAvg1;                             /* 均值1 */
    VOS_INT32                           swDiffAvg2;                             /* 均值2 */
    CODEC_OP_FLOAT_STRU                   stFloatDiffVar1;                        /* 方差1 */
    CODEC_OP_FLOAT_STRU                   stFloatDiffVar2;                        /* 方差2 */
}MED_AEC_AF_KEEPER_STRU;

/* 泄漏因子状态*/
typedef struct
{
    VOS_INT32                           swPyy;                                  /* y/y相关值  */
    VOS_INT32                           swPey;                                  /* e/y相关值 */
    VOS_INT16                           shwAvgErrAbs;                           /* |e|平均值 */
    VOS_INT16                           shwAvgEchoAbs;                          /* |y|平均值 */
}MED_AEC_AF_LEAK_STRU;

/* 步长计算结构体 */
typedef struct
{
    VOS_INT32                           aswStep[1+(MED_AEC_MAX_FFT_LEN/2)];  /* 自适应步长,32bit Q27 */
    VOS_INT32                           swSumAdapt;                             /* 32bit Q27 */
    VOS_INT16                           shwAdapted;                             /* 步长计算中标志 */
    VOS_INT16                           shwRER;                                 /* 最优学习率 */
}MED_AEC_AF_STEP_STRU;

/* AF状态 */
typedef struct
{
    MED_AEC_AF_CONFIG_STRU              stConfig;
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT长度 */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT长度索引枚举 */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT长度索引枚举 */
    VOS_INT16                           shwSampleRate;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                           shwM;                                   /* MDF分块数,上限为MED_AEC_AF_MAX_BLOCK_NUM*/

    /* 时频信号 */
    VOS_INT16                           ashwInput[CODEC_FRAME_LENGTH_WB];   /* 近端信号 */
    VOS_INT16                           ashwFar[CODEC_FRAME_LENGTH_WB];     /* 远端信号 */
    VOS_INT16                           ashwLastFar[CODEC_FRAME_LENGTH_WB*2];/* 前MED_AEC_MAX_FRM_LEN个采样点为前一帧远端信号，后MED_AEC_MAX_FRM_LEN个采样点为当前帧远端信号  */
    VOS_INT16                           ashwFarFreq[MED_AEC_AF_M_MAX+1][MED_AEC_MAX_FFT_LEN]; /* 远端信号频域信号 */
    VOS_INT16                           ashwForeEcho[CODEC_FRAME_LENGTH_WB];/* 前滤波器回声估计信号 */
    VOS_INT16                           ashwEcho[CODEC_FRAME_LENGTH_WB];    /* 后滤波器回声估计信号 */
    VOS_INT16                           ashwForeErr[CODEC_FRAME_LENGTH_WB]; /* 前滤波器残差信号 */
    VOS_INT16                           ashwErr[CODEC_FRAME_LENGTH_WB];     /* 后滤波器残差信号 */
    VOS_INT16                           ashwErrFreq[MED_AEC_MAX_FFT_LEN];    /* 后滤波器残差频域信号 */
    VOS_INT16                           ashwEchoFreq[MED_AEC_MAX_FFT_LEN];   /* 后滤波器回声估计频域信号 */

    /* 能量 */
    VOS_INT32                           swFarPow;                               /* 远端信号能量 */
    VOS_INT32                           swEchoPow;                              /* 后滤波器回声估计信号能量 */
    VOS_INT32                           swErrPow;                               /* 后滤波器残差信号能量*/
    VOS_INT32                           swForeErrPow;                           /* 前滤波器残差信号能量 */
    VOS_INT32                           swBEchoErrPow;                          /* 后滤波器估计信号和后滤波器残差信号互相关功率谱 */
    VOS_INT32                           aswFarSmoothPsd[(MED_AEC_MAX_FFT_LEN/2) + 1]; /* 远端信号平滑功率谱 */
    VOS_INT32                           swNearPow;                              /* 近端信号功率谱 */
    VOS_INT32                           aswErrPsd[(MED_AEC_MAX_FFT_LEN/2) + 1];/* 后端残差信号功率谱 */
    VOS_INT32                           aswEchoPsd[(MED_AEC_MAX_FFT_LEN/2) + 1];/* 后端回声估计信号功率谱 */

    /* 前后滤波器 */
    VOS_INT32                           aswForeWeight[MED_AEC_AF_M_MAX][MED_AEC_MAX_FFT_LEN]; /* 前滤波器滤波系数,32bit Q27 */
    VOS_INT32                           aswWeight[MED_AEC_AF_M_MAX][MED_AEC_MAX_FFT_LEN];     /* 后滤波器滤波系数,32bit Q27 */

    /* 计数器 */
    VOS_INT32                           swCancelCount;                          /* 帧计数器 */

    /* 自适应率 */
    VOS_INT16                           ashwProp[MED_AEC_AF_M_MAX];

    VOS_INT16                           shwScrewedUp;                           /* AF复位判断计数器 */

    /* 前后滤波器比较 */
    MED_AEC_AF_KEEPER_STRU              stKeeper;

    /* 泄漏因子相关变量 */
    MED_AEC_AF_LEAK_STRU                stCompLeakState;
    VOS_INT16                           shwLeakEstimate;                        /* 泄漏率 */

    VOS_INT16                           shwFarPowerAlpha;                       /* 远端能量平滑系数 */

    /* 步长相关变量 */
    MED_AEC_AF_STEP_STRU                stStepState;

} MED_AEC_AF_STRU;


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_VOID MED_AEC_AfCmpTwoFilter(
                       MED_AEC_AF_KEEPER_STRU  *pstKeeper,
                       VOS_INT32                swCurrDiffVar,
                       VOS_INT32                swForeErrPow,
                       VOS_INT32                swErrPow,
                       VOS_INT16               *pshwUpdateFore,
                       VOS_INT16               *pshwUpdateBack);
extern VOS_VOID MED_AEC_AfCompLeak(MED_AEC_AF_STRU *pstAf);
extern VOS_VOID MED_AEC_AfCompRER(
                       VOS_INT32  swFarPow,
                       VOS_INT32  swErrPow,
                       VOS_INT32  swEchoPow ,
                       VOS_INT16  shwLeakEstimate,
                       VOS_INT32  swBEchoErrPow,
                       VOS_INT16 *pshwRER);
extern VOS_VOID MED_AEC_AfCompSmoothPower(
                       VOS_INT16 *pshwX,
                       VOS_INT32  *paswPower,
                       VOS_INT16  shwAlpha,
                       VOS_INT16  shwLen);
extern VOS_VOID  MED_AEC_AfCompStep(MED_AEC_AF_STRU *pstAf);
extern VOS_VOID MED_AEC_AfFilt(
                       VOS_INT16  ashwFarFreq[][MED_AEC_AF_FILT_WB],
                       VOS_INT32  aswWeight[][MED_AEC_AF_FILT_WB],
                       VOS_INT16 *pshwInput,
                       VOS_INT16  shwM,
                       VOS_INT16  enFftLen,
                       VOS_INT16  shwIfftNumIndex,
                       VOS_INT16  enFrameLen,
                       VOS_INT16 *pshwEcho,
                       VOS_INT16 *pshwErr,
                       VOS_INT32 *pswErrPow);
extern VOS_INT32 MED_AEC_AfFrmPower(
                       VOS_INT16 *pshwX,
                       VOS_INT16 *pshwY,
                       VOS_INT16  shwFrameSize,
                       VOS_INT16  shwShift);
extern VOS_VOID MED_AEC_AfInit(
                       MED_AEC_NV_STRU                *pstPara,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_AF_STRU                *pstAf);
extern VOS_VOID MED_AEC_AfMain (
                       MED_AEC_AF_STRU               *pstAfObj,
                       VOS_INT16                     *pshwNear,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                       VOS_INT16                     *pshwErr);
extern VOS_VOID MED_AEC_AfMainUpdate(
                       MED_AEC_AF_STRU               *pstAfObj,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag);
extern VOS_VOID MED_AEC_AfPowerSpectrum(
                       VOS_INT16 *pshwInFreq,
                       VOS_INT16  shwLen,
                       VOS_INT32 *pswInPsd);
extern VOS_VOID MED_AEC_AfResetJudge(MED_AEC_AF_STRU *pstAf, VOS_INT16 *pshwOut);
extern  VOS_VOID MED_AEC_AfSpectralMulAccum(
                       VOS_INT16   ashwFarFreq[][MED_AEC_AF_FILT_WB],
                       VOS_INT32   aswFilterCoeff[][MED_AEC_AF_FILT_WB],
                       VOS_INT16   shwM,
                       VOS_INT16   enFftLen,
                       VOS_INT16  *pshwEchoFreqOut);
extern VOS_VOID MED_AEC_AfStateReset(MED_AEC_AF_STRU  *pstAf);
extern VOS_VOID MED_AEC_AfUpdateProp(
                       VOS_INT32  aswW[][MED_AEC_AF_FILT_WB],
                       VOS_INT16  shwM,
                       VOS_INT16  shwWeightLen,
                       VOS_INT16 *pshwProp);
extern VOS_VOID MED_AEC_AfUpdateTwoFilter(
                       MED_AEC_AF_STRU *pstMedAecAf,
                       VOS_INT16        shwUpdateFore,
                       VOS_INT16        shwUpdateBack);
extern VOS_VOID MED_AEC_AfUpdateWeight(
                       MED_AEC_AF_STRU         *pstAf);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_af.h*/

