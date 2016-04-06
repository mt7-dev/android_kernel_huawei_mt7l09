
#ifndef _MED_AEC_NLP_H_
#define _MED_AEC_NLP_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "med_aec_comm.h"
#include "med_anr_2mic_comm.h"
#include "med_aec_dtd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 访问全局变量 */
#define MED_AEC_GetNlpWinNb()              (&g_ashwMedAecNlpWinNb[0])           /* 窄带下NLP时频变换窗系数 */
#define MED_AEC_GetNlpWinWb()              (&g_ashwMedAecNlpWinWb[0])           /* 宽带下NLP时频变换窗系数 */
#define MED_AEC_GetNlpGainModulTabNB()     (&g_ashwMedAecNlpGainModulTabNB[0])  /* 窄带下NLP子带抑制各频点增益调制系数 */
#define MED_AEC_Get2MicNlpGainModulTabNB() (&g_ashwMed2MicAecNlpGainModulTabNB[0])/* 窄带下2MIC NLP子带抑制各频点增益调制系数 */
#define MED_AEC_GetNlpGainModulTabWB()     (&g_ashwMedAecNlpGainModulTabWB[0])  /* 宽带下NLP子带抑制各频点增益调制系数 */
#define MED_AEC_Get2MicNlpGainModulTabWB() (&g_ashwMed2MicAecNlpGainModulTabWB[0])/* 宽带下2MIC NLP子带抑制各频点增益调制系数 */
#define MED_AEC_GetNlpBinTansTab()         (&g_ashwMedAecNlpDtd2NlpBinTransTab[0])/* NLP子带抑制频点映射表 DTD中FFT长度与NLP不同导致 */

/* 公共项 */
#define MED_AEC_1MIC_NLP_NB_FRM_LEN     (80)                                    /* 窄带帧长 */
#define MED_AEC_1MIC_NLP_WB_FRM_LEN     (160)                                   /* 宽带帧长 */
#define MED_AEC_2MIC_NLP_NB_FRM_LEN     (160)                                   /* 窄带帧长 */
#define MED_AEC_2MIC_NLP_WB_FRM_LEN     (320)                                   /* 宽带帧长 */
#define MED_AEC_NLP_NB_OVERLAP_LEN      (176)                                   /* 窄带窗叠长度 */
#define MED_AEC_NLP_WB_OVERLAP_LEN      (352)                                   /* 宽带窗叠长度 */
#define MED_AEC_NLP_MAX_FFT_LEN         (512)                                   /* NLP最大FFT长度 */
#define MED_AEC_2MIC_NLP_MAX_FFT_LEN    (640)                                   /* 2MIC_NLP最大FFT长度 */
#define MED_AEC_NLP_CNG_EST_LEN         (60)                                    /* 背景噪声估值缓冲长度 */
#define MED_AEC_NLP_GAMMA_MAX           (32767)                                 /* Gamma最大值, 表明完全双讲状态 */
#define MED_AEC_NLP_GAMMA_FIRST         (16384)                                 /* Gamma初始值 */
#define MED_AEC_NLP_GAMMA_MIN           (327)                                   /* Gamma最小值, 表明完全单讲状态 */
#define MED_AEC_NLP_GRAD_NUM            (20)                                    /* NLP抑制曲线最大长度 */
#define MED_AEC_NLP_SUB_FRAME_NUM       (2)                                     /* NLP子帧个数 */

/* 产生白噪声的种子 */
#define MED_AEC_NLP_SEED1               (100)                                   /* 随机种子1 */
#define MED_AEC_NLP_SEED2               (101)                                   /* 随机种子2 */
#define MED_AEC_NLP_SEED3               (102)                                   /* 随机种子3 */
#define MED_AEC_NLP_RNDOM_NUM           (3)                                     /* 随机数状态长度 */
#define MED_AEC_NLP_15200_Q0            (15200)                                 /* 15200 Q0  */

/* 同余法产生序列参数 */
#define MED_AEC_NLP_RAND_A1             (170)                                   /* 扩展因子a1 */
#define MED_AEC_NLP_RAND_A2             (171)                                   /* 扩展因子a2 */
#define MED_AEC_NLP_RAND_A3             (172)                                   /* 扩展因子a3 */
#define MED_AEC_NLP_RAND_M1             (30269)                                 /* 循环模m1 */
#define MED_AEC_NLP_RAND_M2             (30307)                                 /* 循环模m2 */
#define MED_AEC_NLP_RAND_M3             (30323)                                 /* 循环模m3 */

/* 同余序列线性组合系数 */
#define MED_AEC_NLP_RAND_ALPHA          (10825)                                 /* 线性组合系数1, Q15 */
#define MED_AEC_NLP_RAND_BETA           (10812)                                 /* 线性组合系数2, Q15 */
#define MED_AEC_NLP_RAND_GAMMA          (10806)                                 /* 线性组合系数3, Q15 */

/* Wiener滤波 */
#define MED_AEC_MAX_WIENER_DEC          (10)                                    /* NLP Wiener滤波弱化时刻。持续双讲中Wiener滤波作用开始弱化时刻，范围0 － 1000，单位10ms，默认19 */
#define MED_AEC_MAX_WIENER_OFF          (19)                                    /* NLP Wiener滤波停止时刻，持续双讲中Wiener滤波作用停止时刻，范围大于等于Wiener滤波弱化时刻，单位10ms，默认38 */
#define MED_AEC_NLP_WIENER_LVL          (1)                                     /* Wiener滤波级别 */
#define MED_AEC_NLP_WIENER_MIN_COEF     (5898)                                  /* Wiener滤波系数下限0.18, -15dB */
#define MED_AEC_NLP_PSD_SMOOTH          (9380)                                  /* 0.3 功率谱系数更新因子,值越小,更新越快 */
#define MED_AEC_NLP_SUPP_EN_THD         (10000)                                 /* 高频进一步抑制的能量阈值 */
#define MED_NLP_NLP_CNG_BIN_SHIFT       (10)
#define MED_AEC_NLP_RES_POW_THD         ((VOS_INT32)2000000000)                 /* Res高频能量阈值 */

/* 背景噪声估计 */
#define MED_AEC_NLP_BGN_MEAN_POW_MAX    ((VOS_INT32)2000*2000)
#define MED_AEC_NLP_BGN_AMP_ALPHA            (6553)                             /* 平滑系数0.2     Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_AMP_ALPHA    (26215)                            /* 平滑系数0.8     Q15 */
#define MED_AEC_NLP_BGN_FREQ_ALPHA           (655)                              /* 平滑系数0.02    Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_FREQ_ALPHA   (32113)                            /* 平滑系数0.98    Q15 */
#define MED_AEC_NLP_BGN_MIN_SMOOTH_ALPHA     (33)                               /* 平滑系数0.0001  Q15 */
#define MED_AEC_NLP_BGN_SMOOTH_COFE1         (22938)                            /* 平滑系数0.7     Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_SMOOTH_COFE1 (9830)                             /* 平滑系数0.3     Q15 */
#define MED_AEC_NLP_BGN_INIT_VALUE           (0)                                /* 背景噪声估值初始值 */
#define MED_AEC_NLP_MODIFIED_ALPHA           (1)                                /* 背景噪声估计修正系数初始值 */
/* 其他 */
#define MED_NLP_NLP_CNG_INSERT_COFE          (19660)                            /* 插入舒适噪声的系数 */
#define MED_NLP_NLP_CNG_SHIFT                (2)                                /* 双Mic会进一步抑制，插入的舒适噪声应该是抑制后的幅度。
                                                                                   通常抑制6-10倍，因此插入的频域信号减小4倍 */

/* 根据DTD子带概率进行回声抑制 */
#define MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_NB  (32)                               /* 根据DTD子带概率进行窄带回声抑制 截止子带 */
#define MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_WB  (40)                               /* 根据DTD子带概率进行宽带回声抑制 截止子带 */
#define MED_AEC_NLP_LOW_BIN_INDEX            (5)                                /* 根据DTD子带概率进行回声抑制 低频能量计算起始索引 */
#define MED_AEC_NLP_HIGIH_BIN_INDEX          (11)                               /* 根据DTD子带概率进行回声抑制 低频能量计算终止索引 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/* 前处理窗 */
extern VOS_INT16 g_ashwMedAecNlpWinNb[];
extern VOS_INT16 g_ashwMedAecNlpWinWb[];

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : MED_AEC_NLP_CONFIG_STRU
 功能描述  : 描述NLP配置的结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enNlpEnable;
    VOS_INT16                           shwDt2StLvl;                            /* DT -> ST 的切换速度级别 */
    VOS_INT16                           shwSt2DtLvl;                            /* ST -> DT 的切换速度级别 */
    VOS_INT16                           shwCngInsertLvl;                        /* 舒适噪声级别 */
    VOS_INT16                           shwNoiseFloorValue;                     /* 本底噪声值 */
    VOS_INT16                           shwMaxSuppress;                         /* 高频非线性最大抑制量 */
    VOS_INT16                           shwNonlinearThd;                        /* 高频非线性抑制门限 */
    VOS_INT16                           shwNlpBandProbSupFastAlpha;             /* NLP: 快滤波系数 */
    VOS_INT16                           shwNlpBandProbSupSlowAlpha;             /* NLP: 慢滤波系数 */
    VOS_INT16                           shwReserve;                             /* 保留 */
}MED_AEC_NLP_CONFIG_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_SWITCH_STRU
 功能描述  : 描述NLP gamma切换状态的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwGammaPre;
    VOS_INT16                           ashwDt2StGradsTab[MED_AEC_NLP_GRAD_NUM];/* Dt2St实际使用的handover曲线表 最大级别20 */
    VOS_INT16                           shwDt2StGradsLen;                       /* Dt2St实际使用的曲线表长度 */
    VOS_INT16                           ashwSt2DtGradsTab[MED_AEC_NLP_GRAD_NUM];/* St2Dt实际使用的handover曲线表 最大级别20 */
    VOS_INT16                           shwSt2DtGradsLen;                       /* St2Dt实际使用的曲线表长度 */
    VOS_INT16                           shwDt2StCnt;                            /* DT -> ST 计数 */
    VOS_INT16                           shwSt2DtCnt;                            /* ST -> DT 计数 */
    VOS_INT16                           shwReserve;
}MED_AEC_NLP_SWITCH_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述随机序列产生状态的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwSeed[MED_AEC_NLP_RNDOM_NUM];        /* 随机种子 */
    VOS_INT16                           ashwState[MED_AEC_NLP_RNDOM_NUM];       /* 随机序列状态 */
} MED_AEC_NLP_RANDOM_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述残留回声抑制状态的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswSme[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* Mic和残差互功率谱 */
    VOS_INT32                           aswSmm[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* Mic信号功率谱 */
    VOS_INT32                           aswSyy[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* 回声功率谱 */
    VOS_INT32                           aswSee[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* 残差功率谱 */
    VOS_INT16                           shwWienerFulEn;                         /* 完全使能阶段 */
    VOS_INT16                           shwWienerPrtEn;                         /* 部分使能阶段 */
    VOS_INT16                           shwDtCnt;                               /* 双讲计数 */
    VOS_INT16                           shwMaxSuppress;
    VOS_INT16                           shwPowThd;                               /* ST\DT过渡阶段抑制修正阈值 */
    VOS_INT16                           shwNonlinearThd;
}MED_AEC_NLP_RES_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述背景噪声估计状态的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwModifiedALpha;                       /* 背景噪声估计修正系数 */
    VOS_INT16                           shwBgnBinLen;                           /* 背景噪声频谱长度 */
    VOS_INT32                           aswBgnBin[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)]; /* 背景噪声频谱 */
    VOS_INT32                           swBgnValue;                             /* 背景噪声幅度估计值 */
    VOS_INT16                           ashwSubErrFrmLast[CODEC_FRAME_LENGTH_WB/2];/* 上一个子帧 */
    VOS_INT32                           swSmoothedPower;                        /* 平滑噪声能量 */
    VOS_INT32                           swMinNoisePower;                        /* 最小噪声能量 */
    VOS_INT32                           swPowerBuf[MED_AEC_NLP_CNG_EST_LEN];    /* 噪声能量缓存60*20ms */
}MED_AEC_NLP_BGN_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_BAND_SUPRESS_STRU
 功能描述  : 描述子带语音存在概率残差抑制的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwNlpFreqBinGain[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];  /* 频点增益 */
    VOS_INT16                           shwReserve;                             /* 保留 */
    VOS_INT32                           swPsdBinLo2Hi;                          /* 低频残差能量 */
    VOS_INT16                           shwNlpBandProbSupFastAlpha;             /* NLP: 快滤波系数 */
    VOS_INT16                           shwNlpBandProbSupSlowAlpha;             /* NLP: 慢滤波系数 */
}MED_AEC_NLP_BAND_SUPRESS_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述NLP状态的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwFftLen;                              /* NLP中FFT长度 */
    VOS_INT16                           shwFrameLen;                            /* NLP 1MIC:帧长10ms;2MIC:帧长20ms */
    MED_AEC_NLP_CONFIG_STRU             stConfig;                               /* NLP配置项 */
    MED_AEC_NLP_SWITCH_STRU             stSwCtrl;                               /* gamma曲线抑制状态 */
    MED_AEC_NLP_RANDOM_STRU             stRandSn;                               /* 随机序列生成状态 */
    MED_AEC_NLP_RES_STRU                stRes;                                  /* 残留回声抑制状态 */
    MED_AEC_NLP_BGN_STRU                stBgnStru;                              /* 背景噪声估计状态 */
    MED_AEC_NLP_BAND_SUPRESS_STRU       stBandSupress;                          /* 子带语音存在概率残差抑制状态 */
}MED_AEC_NLP_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述时频转换中重叠结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;
    VOS_INT16                           shwFrmLen;
    VOS_INT16                           ashwOverlap[MED_AEC_NLP_WB_OVERLAP_LEN*2];
}MED_AEC_OVERLAP_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_RANDOM_STRU
 功能描述  : 描述时频转换状态结构体
*****************************************************************************/
typedef struct
{
    MED_AEC_OVERLAP_STRU                stErrPreStatus;                         /* 残差信号前处理状态 */
    MED_AEC_OVERLAP_STRU                stMicPreStatus;                         /* 麦克风信号前处理状态 */
    MED_AEC_OVERLAP_STRU                stErrPostStatus;                        /* 残差信号后处理状态 */
    VOS_INT16                           shwFftLen;                              /* NLP 当前FFT IFFT的长度 */
    VOS_INT16                           shwReserve;
    VOS_INT16                          *pshwTrapeWin;                           /* 梯形窗系数指针 */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT长度索引枚举 */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT长度索引枚举 */
} MED_AEC_TRANS_STRU;


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/

extern VOS_VOID MED_AecNlpWienerCofe(
                       VOS_INT32          *paswSme,
                       VOS_INT32          *paswSmm,
                       VOS_INT32          *paswSyy,
                       VOS_INT16           shwPow,
                       VOS_INT16           ashwMaxH,
                       VOS_INT16           ashwMinH,
                       VOS_INT16           shwFftLen,
                       VOS_INT16          *pshwWienerCoef);
extern VOS_VOID MED_AEC_1MIC_NlpMain(
                       MED_AEC_NLP_STRU              *pstNlp,
                       MED_AEC_DTD_NLINE_STRU        *pstDtdNline,
                       VOS_INT16                     *pshwErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                       VOS_INT16                     *pshwMcBin,
                       VOS_INT16                     *pshwMcErrBinNorm,
                       VOS_INT16                      shwNormShift);
extern VOS_VOID MED_AEC_NlpBandProbSuppress(
                MED_AEC_DTD_NLINE_STRU         *pstDtdNline,
                MED_AEC_NLP_BAND_SUPRESS_STRU  *pstBandSupress,
                VOS_INT16                      *pshwErrBinNorm,
                VOS_INT16                       shwNormShift,
                VOS_INT16                       shwFftLen);
extern VOS_VOID MED_AEC_2MIC_Fft(
                       MED_2MIC_FREQ_STRU      *pstFreqOut,
                       MED_2MIC_COMM_STRU      *pstComCfg,
                       VOS_INT16               *pshwTimeIn,
                       CODEC_SWITCH_ENUM_UINT16   enNormEnable);
extern VOS_VOID MED_AEC_2MIC_Freq2Time(
                       MED_2MIC_DATAFLOW_MC_STRU         *pstFreqIn,
                       MED_2MIC_COMM_STRU                *pstComCfg,
                       VOS_INT16                         *pshwTimeOut);
extern VOS_VOID  MED_AEC_2MIC_NlpMain(
                       MED_2MIC_DATAFLOW_MC_STRU        *pstDataFlowMc,
                       MED_2MIC_DATAFLOW_RC_STRU        *pstDataFlowRc,
                       MED_AEC_NLP_STRU                 *pstNlp,
                       MED_AEC_DTD_NLINE_STRU           *pstDtdNline,
                       VOS_INT16                        *pshwMcErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16       enDtdFlag);
extern VOS_VOID MED_AEC_2MIC_NlpRc(
                       VOS_INT16              *pshwRcErrBinNorm,
                       VOS_INT16              *pshwRcsBinNorm,
                       VOS_INT16              *pshwWienerCoef,
                       VOS_INT16               shwFftLen );
extern VOS_VOID MED_AEC_2MIC_Time2Freq(
                       MED_2MIC_DATAFLOW_MC_STRU        *pstFreqMc,
                       MED_2MIC_DATAFLOW_RC_STRU        *pstFreqRc,
                       MED_2MIC_COMM_STRU                stComCfg,
                       VOS_INT16                        *pshwMcTd,
                       VOS_INT16                        *pshwRcTd,
                       VOS_INT16                        *pshwMcIn,
                       VOS_INT16                        *pshwRcCalib);
extern VOS_VOID MED_AEC_Freq2Time(
                       MED_AEC_TRANS_STRU          *pstTrans,
                       VOS_INT16                   *pshwMcErrBinNorm,
                       VOS_INT16                    shwNormShift,
                       VOS_INT16                   *pshwErrOut);
extern VOS_VOID MED_AEC_NlpBgnEst (
                       MED_AEC_NLP_BGN_STRU *pstBgnStru,
                       VOS_INT16            *pshwFrm,
                       VOS_INT16            *pshwBin,
                       VOS_INT16             shwFrmLen,
                       VOS_INT16             shwNoiseFloorValue);
extern VOS_VOID MED_AEC_NlpCngWhiteGen(
                       MED_AEC_NLP_RANDOM_STRU     *pstRandomState,
                       VOS_INT16                    shwGenLen,
                       VOS_INT16                   *pshwWhiteSn);
extern VOS_VOID MED_AEC_NlpCtrl(
                       MED_AEC_NLP_SWITCH_STRU     *pstSwCtrl,
                       VOS_INT16                   *pshwdErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16  enDtd,
                       VOS_INT16                   *pshwGamma);
extern VOS_VOID MED_AEC_NlpInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_NLP_STRU               *pstNlp,
                       MED_PP_MIC_NUM_ENUM_UINT16      enPpMicNum);
extern VOS_VOID MED_AEC_NlpInsertCn (
                       VOS_INT16               *pshwErrBin,
                       VOS_INT16                shwGamma,
                       VOS_INT16               *pshwWhiteNoise,
                       VOS_INT32                swBgnValue,
                       VOS_INT16                shwLen);
extern VOS_VOID MED_AEC_NlpRes(
                       MED_AEC_NLP_RES_STRU *pstRes,
                       VOS_INT16            *pshwMicBin,
                       VOS_INT16            *pshwErrBin,
                       VOS_INT16            *pshwErrBinNorm,
                       VOS_INT16            *pshwWienerCoef,
                       VOS_INT16             shwFftLen,
                       VOS_INT16             shwGamma);
extern VOS_VOID MED_AEC_NlpShapeCng (
                       VOS_INT16       *pshwWhiteNoise,
                       VOS_INT32       *pswNoiseBin,
                       VOS_INT16        shwFftLen);
extern VOS_VOID MED_AEC_NlpSlopSuppress(
                       VOS_INT16       *pshwInput,
                       VOS_INT16        shwFftLen,
                       VOS_INT16        shwMaxSuppress,
                       VOS_INT16       *pshwOutput);
extern VOS_INT16 MED_AEC_NlpSyncCnt(
                       VOS_INT16               shwGamma,
                       VOS_INT16              *pashwGrads,
                       VOS_INT16               shwGradsLen);
extern VOS_VOID MED_AEC_Time2Freq(
                       MED_AEC_TRANS_STRU       *pstTrans,
                       VOS_INT16                *pshwNear,
                       VOS_INT16                *pshwErr,
                       VOS_INT16                *pshwMcBin,
                       VOS_INT16                *pshwMcErrBinNorm,
                       VOS_INT16                *pshwNormShiftErr);
VOS_VOID MED_EAEC_Time2Freq(
                       MED_AEC_TRANS_STRU       *pstTrans,
                       VOS_INT16                *pshwNear,
                       VOS_INT16                *pshwErr,
                       VOS_INT16                *pshwMcBin,
                       VOS_INT16                *pshwMcErrBinNorm,
                       VOS_INT16                *pshwNormShiftNear,
                       VOS_INT16                *pshwNormShiftErr);
extern VOS_VOID MED_AEC_TransInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_TRANS_STRU             *pstTrans);
extern VOS_VOID MED_AEC_TransPostProcess(
                       MED_AEC_OVERLAP_STRU        *pstPostStatus,
                       VOS_INT16                   *pshwErrBin,
                       VOS_INT16                    shwNormShift,
                       VOS_INT16                   *pshwErrFrm,
                       VOS_INT16                    shwFftLen,
                       VOS_INT16                    shwIfftNumIndex,
                       VOS_INT16                   *pshwTrapeWin);
extern VOS_VOID MED_AEC_TransPreProcess(
                       MED_AEC_OVERLAP_STRU        *pstPreStatus,
                       VOS_INT16                   *pshwStream,
                       VOS_INT16                    shwNormEn,
                       VOS_INT16                   *pshwSpecBin,
                       VOS_INT16                   *pshwNormShift,
                       VOS_INT16                    shwFftNumIndex);
extern VOS_VOID MED_AEC_NlpGainModulTab(
                     CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enPpSampleRate,
                     MED_PP_MIC_NUM_ENUM_UINT16         enMicNum,
                     VOS_INT16                         *pshwNlpProbBandLen,
                     VOS_INT16                        **ppshwGainModulTab);

extern VOS_VOID MED_1MIC_ExtendBand2FreqBinGain(
                    VOS_INT16               *pshwBandGain,
                    VOS_INT16                shwFreqBinLen,
                    VOS_INT16                shwBandLen,
                    VOS_INT16               *pshwFreqBinGain);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_nlp.h*/

