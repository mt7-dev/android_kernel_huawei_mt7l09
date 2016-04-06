

#ifndef _MED_AEC_DTD_H_
#define _MED_AEC_DTD_H_

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
/* 静音检测和线性DTD宏定义 */
#define MED_AEC_DTD_MEAN_ALPHA_NB       (205)                                   /* 窄带初始MeanAlpha */
#define MED_AEC_DTD_MEAN_ALPHA_WB       (102)                                   /* 宽带初始MeanAlpha */
#define MED_AEC_DTD_MUTE_ALPHA          (24576)                                 /* 静音检测平滑系数 */
#define MED_AEC_DTD_MUTE_HO_THD         (30)                                    /* 静音最大计数值 */
#define MED_AEC_DTD_ST_FLR_THD          (16)                                    /* 底噪阈值，对于绝对小的信号, 低于此值直接判为单讲 */
#define MED_AEC_DTD_SPEC_ALPHA          (9830)                                  /* 功率谱平滑系数 */
#define MED_AEC_DTD_CORR_THD            (31130)                                 /* 单双讲相关系数阈值 */
#define MED_AEC_DTD_BETA_MIN            (328)                                   /* beta 最小值 */
#define MED_AEC_DTD_BETA_MAX            (32767)                                 /* beta 最大值 */

/* 非线性DTD判别宏定义 */
#define MED_AEC_DTD_NLINE_FREQ_BIN_LEN_NB            (161)                      /* 窄带频谱根数 */
#define MED_AEC_DTD_NLINE_FREQ_BIN_LEN_WB            (321)                      /* 宽带频谱根数 */
#define MED_AEC_DTD_NLINE_PSD_BAND_LEN_NB            (32)                       /* 窄带子带个数 */
#define MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB            (40)                       /* 宽带子带个数 */
#define MED_AEC_DTD_NLINE_DT_CONTINUOUS_LEN          (6)                        /* 双讲hangover连续帧数阈值 6bit Q0 6 */
#define MED_AEC_DTD_NLINE_DT_HANGOVER_LEN            (6)                        /* 双讲hangover帧数 16bit Q0 6 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE    (16384)                    /* 子带近端语音存在概率初始值 16bit Q15 0.5 */
#define MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN       (15)                       /* 计算子带近端语音存在概率使用子带数 16bit Q0 31 */
#define MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA      (26214)                    /* 计算子带功率谱平滑系数 16bit Q15 0.8 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN    (5)                        /* 计算子带近端语音存在概率带间平滑长度 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_INDEX  (19)                       /* 计算子带近端语音存在概率带间平滑起始子带索引 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA  (32767)                    /* 计算子带近端语音存在概率平滑系数 16bit Q15 1 */
#define MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA  (3276)                     /* 计算子带近端语音存在概率平均值平滑系数 16bit Q15 0.7*/
#define MED_AEC_DTD_NLINE_SP_SER_THD                 (32767)                    /* 近端语音存在信回比 16bit Q8 8 */
#define MED_AEC_DTD_NLINE_ECHO_SER_THD               (8192)                     /* 近端语音不存在信回比 16bit Q8 2 */
#define MED_AEC_DTD_NLINE_BAND_PSD_MUTE_THD          (25600)                    /* 子带功率谱静音帧判断阈值 32bit Q8 100*/
#define MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL     (82)                       /* 近端语音判断语音概率阈值小步长 16bit Q15 0.0025 */
#define MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_LARGE     (655)                      /* 近端语音判断语音概率阈值大步长 16bit Q15 0.02*/
#define MED_AEC_DTD_NLINE_SUM_PSD_ALPHA              (3277)                     /* 近端语音判断能量计算平滑系数 16bit Q15 0.1 */
#define MED_AEC_DTD_NLINE_SUM_PSD_THD_BASE           (100)                      /* 近端语音判断能量阈值NV项与真实参数倍数 32bit Q8 100 */


#define MED_AEC_DtdNlineGetSerModulTabPtr()          (&g_ashwDtdBandSERThdModulTab[0])
#define MED_AEC_DtdNlineGetProbAlphaCoeffPtr()       (&g_ashwSpBandProbAlphaCoeff[0])

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
/* DTD状态 线性判断结构体 */
typedef struct
{
    VOS_INT16                           shwFrameLen;                            /* 帧长 */
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT长度 */
    VOS_INT16                           shwMeanAlpha;                           /* 均值计算平滑系数 */
    VOS_INT16                           shwStModiThd;                           /* 单讲修正门限 */
    VOS_INT16                           shwDtModiThd;                           /* 双讲修正门限 */
    VOS_INT16                           shwStFlrThd;                            /* 单讲本底能量门限 */
    VOS_INT32                           aswPdy[CODEC_FRAME_LENGTH_WB * 2];  /* 近端和残差信号的平滑互功率谱 */
    VOS_INT32                           aswPdd[CODEC_FRAME_LENGTH_WB * 2];  /* 近端平滑自功率谱 */
}MED_AEC_DTD_LINE_STRU;

/* DTD状态 非线性判断结构体 */
typedef struct
{
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT长度 */
    VOS_INT16                           shwFreqBinLen;                          /* 频谱根数 */
    VOS_INT16                           shwBandLen;                             /* 子带个数 */
    VOS_INT16                           shwReserved;
    VOS_INT32                           aswPsdBandNear[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* 近端信号子带功率谱 */
    VOS_INT32                           aswPsdBandFar[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB];  /* 远端信号子带功率谱 */
    VOS_INT32                           aswPsdBandEcho[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* 估计回声信号子带功率谱 */
    VOS_INT16                           ashwSpBandProb[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* 语音子带存在概率 */
    VOS_INT16                           shwSpProbMean;                          /* 语音子带存在概率平均值 */
    CODEC_SWITCH_ENUM_UINT16            enNearFarRatioEnable;                   /* 近端与远端功率谱密度比值使能 */
    VOS_INT16                           shwNearFarRatioActiveFrmNum;            /* DTD下行数据参考帧数*/
    VOS_INT16                           shwNearFarRatioActiveFrmCnt;
    VOS_INT16                           shwNearFarRatioGain;                    /* 远端功率谱密度的增益 */
    VOS_INT16                           shwSpSerThd;                            /* 语音存在概率为1的SER门限 */
    VOS_INT16                           shwEchoSerThd;                          /* 语音存在概率为0的SER门限 */
    VOS_INT16                           shwBandPsdMuteThd;                      /* 近端功率谱密度判为静音的门限 */
    VOS_INT16                           shwSpThdInit;                           /* 近端语音存在判别阈值初始值 */
    VOS_INT16                           shwSpThd;                               /* 近端语音存在判别阈值 */
    VOS_INT16                           shwSpThdMax;                            /* 近端语音存在判别阈值最大值 */
    VOS_INT16                           shwSpThdMin;                            /* 近端语音存在判别阈值最小值 */
    VOS_INT32                           swSumPsdThd;                            /* 近端语音存在判别能量阈值 */
    VOS_INT32                           swSumPsd;                               /* 近端语音存在判别能量 */

}MED_AEC_DTD_NLINE_STRU;

/* DTD状态 总结构体 */
typedef struct
{
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;                              /*单双讲标志 */
    VOS_INT16                           shwFrameLen;                            /*帧长 */
    VOS_INT16                           shwDtConLen;                            /*连续双讲帧数 */
    VOS_INT16                           shwDtHangLen;                           /*双讲hangover帧数 */
    VOS_INT16                           shwMutePowerThd;                        /*静音检测能量门限*/
    VOS_INT16                           shwMuteHandover;                        /*静音检测切换计数器 */
    VOS_INT32                           swPowerLast;                            /*上一帧能量 */
    MED_AEC_DTD_LINE_STRU               stDtdLine;                              /*线性DTD判别结构体 */
    MED_AEC_DTD_NLINE_STRU              stDtdNline;                             /*非线性DTD判别结构体 */
    CODEC_SWITCH_ENUM_UINT16            enEstFlag;                              /*DTD增强单讲(EST)使能开关*/
    VOS_INT16                           shwEstPowerThd;                         /*DTD增强单讲(EST)能量阈值*/
    VOS_INT16                           shwEstAmplThd;                          /*DTD增强单讲(EST)幅度阈值*/
    VOS_INT16                           shwEstHangOver;                         /*DTD增强单讲(EST)hangover帧长*/
    VOS_INT16                           shwFarLargeFlag;                        /*DTD增强单讲(EST)下行能量大标志*/
    VOS_INT16                           shwReserved;
}MED_AEC_DTD_STRU;


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear);
extern MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdNline(
                MED_AEC_DTD_NLINE_STRU        *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagLine,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq);
extern VOS_VOID MED_AEC_DtdSpProb(
                MED_AEC_DTD_NLINE_STRU            *pstDtd,
                VOS_INT32                         *pswPsdBandNear,
                VOS_INT32                         *pswPsdBandFar,
                VOS_INT32                         *pswPsdBandEcho,
                VOS_INT16                         *pshwSpBandProb);
extern VOS_VOID MED_AEC_Dtd(
                        MED_AEC_DTD_STRU              *pstDtd,
                        VOS_INT16                     *pshwErrFreq,
                        VOS_INT16                     *pshwEchoFreq,
                        VOS_INT16                     *pshwFarFreq,
                        VOS_INT16                     *pshwError,
                        VOS_INT16                     *pshwNear);
extern VOS_VOID MED_AEC_DtdInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_DTD_STRU               *pstDtd);
extern VOS_VOID MED_AEC_DtdMuteDetect(
                       MED_AEC_DTD_STRU              *pstDtd,
                       VOS_INT16                     *pshwFar);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_dtd.h*/

