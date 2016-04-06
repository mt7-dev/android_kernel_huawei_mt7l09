
#ifndef __MED_ANR_2MIC_FD_H__
#define __MED_ANR_2MIC_FD_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "med_anr_2mic_comm.h"
#include "codec_com_codec.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_2MIC_MIN_BIN_ENRG           (1)                                     /* 语音子带能量下限 */

#define MED_2MIC_NUM_CHAN_8k            (32)                                    /* 8K采样率的频域子带数 */
#define MED_2MIC_NUM_CHAN_16k           (40)                                    /* 16K采样率的频域子带数 */
#define MED_2MIC_SP_LO_BAND_8k          (5)                                     /* 语音频谱下限频带(8k) */
#define MED_2MIC_SP_HI_BAND_8k          (30)                                    /* 语音频谱上限频带(8k) */
#define MED_2MIC_SP_LO_BAND_16k         (5)                                     /* 语音频谱下限频带(16k) */
#define MED_2MIC_SP_HI_BAND_16k         (37)                                    /* 语音频谱上限频带(16k) */
#define MED_2MIC_Mid_BAND_8k            (23)                                    /* 中频位置频带(8k) */
#define MED_2MIC_Mid_BAND_16k           (32)                                    /* 中频位置频带(16k) */

#define MED_2MIC_COMP_COEF1             (20577)                                 /* 频谱校正系数 */
#define MED_2MIC_COMP_COEF2             (29066)                                 /* 频谱校正系数 */
#define MED_2MIC_COMP_COEF3             (32613)                                 /* 频谱校正系数 */
#define MED_2MIC_COMP_FREQ_LOW          (9)                                     /* 频谱校正的下限频带 */
#define MED_2MIC_COMP_FREQ_HIGH         (21)                                    /* 频谱校正的上限频带 */
#define MED_2MIC_COMP_FREQ_THD1         (17514)                                 /* 噪声子带判断阈值 */
#define MED_2MIC_COMP_FREQ_THD2         (8231)                                  /* 辅麦克信号频谱抑制程度判断阈值 */

#define MED_2MIC_SNPSD_MAX              (2048000000)                            /* 噪声功率谱的最大值 */
#define MED_2MIC_SNPSD_STATE_LEN        (50)                                    /* 跟踪最小功率谱速度 */
#define MED_2MIC_SNPSD_PROB_ALPHA       (6553)                                  /* 噪声功率谱平滑系数 */
#define MED_2MIC_SNPSD_PROB_SUB_ALPHA   (26214)                                 /* 32767- MED_2MIC_SNPSD_PROB_ALPHA */
#define MED_2MIC_SNPSD_PROB_THD         (24576)                                 /* 语音存在判别阈值 */
#define MED_2MIC_SNPSD_ALPHA            (31129)                                 /* 平稳噪声功率谱的平滑系数 */
#define MED_2MIC_SNPSD_SUB_ALPHA        (1638)                                  /* 32767 - MED_2MIC_SNPSD_ALPHA */

#define MED_2MIC_ABSEN_TRANS_RATIO_LOW  (8192)                                  /* 主辅麦克瞬时变化率低阈值 */
#define MED_2MIC_ABSEN_TRANS_RATIO_HIGH (24576)                                 /* 主辅麦克瞬时变化率高阈值 */
#define MED_2MIC_ABSEN_LNS_RATIO_THD    (12616)                                 /* 根据非平稳性判别语音存在阈值  */
#define MED_2MIC_ABSEN_POST_SNR_ALPHA   (18842)                                 /* 后验信噪比平滑系数 */
#define MED_2MIC_ABSEN_POST_SNR_THLD    (4096)                                  /* 判断语音不存在的后验信噪比阈值*/
#define MED_2MIC_ABSEN_POST_SNR_SUB_ALPHA (14746)                               /* 后验信噪比平滑系数与后验信噪比阈值之差*/

/* 修正各子带语音不存在概率 */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA    (16384)                       /* 能量比平滑系数 */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA      (16384)                       /* 功率谱之比平滑系数 */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_LOW      (3072)                        /* 能量比下限 */
#define MED_2MIC_MODFY_ABSEN_RATIO_INIT           (1024)                        /* 能量比初始值 */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_HIGH     (2048)                        /* 能量比上限 */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_LOW        (3072)                        /* 功率谱之比下限 */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_HIGH       (3072)                        /* 功率谱之比上限 */
#define MED_2MIC_MODFY_FREQ_SMOOTH_LEN            (5)                           /* 平滑长度 */
#define MED_2MIC_SPABSEN_Q13                      (8192)                        /* 2^13 */
#define MED_2MIC_MODFY_ABSEN_FAR_MUTE_INIT        (3200)                        /* 无远端信号帧计数初始值 */

/* 计算各子带语音存在概率 */
#define MED_2MIC_SPPRENSEN_POSTSNR_MIN            (3)                           /* 后验信噪比最小值 */
#define MED_2MIC_SPPRENSEN_PRIORSNR_MIN           (3)                           /* 先验信噪比最小值 */
#define MED_2MIC_SPPRENSEN_POSTSNR_INIT           (256)                         /* 后验信噪比初始值 */
#define MED_2MIC_SPPRENSEN_PRIORSNR_ALPHA         (30146)                       /* 先验信噪比平滑系数 */
#define MED_2MIC_SPPRENSEN_PRIORSNR_SUB_ALPHA     (2621)                        /* 1 - 先验信噪比平滑系数 */
#define MED_2MIC_SPPRENSEN_NSPSD_ALPHA            (27852)                       /* 噪声功率谱平滑系数 */
#define MED_2MIC_SPPRENSEN_NSPSD_SUB_ALPHA        (4915)                        /* 1 - 噪声功率谱平滑系数 */
#define MED_2MIC_SPPRENSEN_NSPSD_BETA             (32767)                       /* 噪声功率谱平滑系数 */
#define MED_2MIC_SPPRENSEN_PROBZEROTHD            (29490)                       /* 语音存在判别阈值 */
#define MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX   (32072)                       /* 信噪比因子上限 */
#define MED_2MIC_SPPRENSEN_Q8                     (256)                         /* 2^8 */


#define MED_2MIC_LSAGAIN_MIN_ALPHA      (9830)                                  /* 最小增益平滑系数 */
#define MED_2MIC_LSAGAIN_MIN_BETA       (24576)                                 /* 最小增益平滑系数 */
#define MED_2MIC_LSAGAIN_TIME_ALPHA     (13107)                                 /* 帧间平滑系数 */
#define MED_2MIC_LSAGAIN_SMOOTH_THD     (9830)                                  /* 平滑增益的判断阈值 */
#define MED_2MIC_LSA_LOW_BAND           (5)                                     /* 低频带起始位置 */
#define MED_2MIC_LSA_DELTA              (2)                                     /* 平滑区间 */
#define MED_2MIC_LSA_MEAN_COEF          (6553)                                  /* 求平均(除以5相当于乘以2)*/
#define MED_2MIC_LSAGAIN_FREQ_SMOOTH_LEN (3)
#define MED_2MIC_LSAGAIN_Q8             (256)                                   /* 2^8 */

#define MED_2MIC_TMOS_SILENCETh         (50)                                    /* 关闭降噪单元前静默帧数门限 */
#define MED_2MIC_TMOS_CLOSEANR_LEN      (150)                                   /* 关闭降噪单元帧数 */
#define MED_2MIC_TMOS_EXIT_LOWBIN_8k    (4)                                     /* 退出关闭降噪时统计噪声能量窄带低频瓣索引 */
#define MED_2MIC_TMOS_EXIT_HIGHBIN_8k   (16)                                    /* 退出关闭降噪时统计噪声能量窄带高频瓣索引 */
#define MED_2MIC_TMOS_EXIT_LOWBIN_16k   (8)                                     /* 退出关闭降噪时统计噪声能量宽带低频瓣索引 */
#define MED_2MIC_TMOS_EXIT_HIGHBIN_16k  (32)                                    /* 退出关闭降噪时统计噪声能量宽带高频瓣索引 */
/*****************************************************************************
 宏名称    : MED_2MIC_GetFdProcessPtr
 功能描述  : 获取FD模块结构体指针
*****************************************************************************/
#define MED_2MIC_GetFdProcessPtr()                (&g_stAnr2MicFdProcess)

/* 获取常量数组指针 */
#define MED_2MIC_GetCompCoefPtr()                 (g_ashwAnr2MicCompCoef)
#define MED_2MIC_GetPsdRatioFreqAlphaPtr()        (g_ashwAnr2MicPsdRatioFreqAlpha)
#define MED_2MIC_GetBandAlphaPtr()                (g_ashwAnr2MicBandAlpha)
#define MED_2MIC_GetAnr2MicChTblPtr()             (g_ashwMedAnr2MicChTbl)

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
 实体名称  : MED_2MIC_COMPFREQAP_STRU
 功能描述  : 辅麦克噪声功率谱补偿结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                          *pshwCompCoef;                           /* 宽带信号频率补偿系数 */
    VOS_INT16                           shwSampleRate;                          /* 采样率 */
    VOS_INT16                           shwReserve;
} MED_2MIC_COMPFREQAP_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_PROBSPABSEN_STRU
 功能描述  : 计算语音不存在概率结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];  /* 各子带语音不存在概率 */
    VOS_INT16                           shwSumProbThld;                           /* 全带语音存在概率阈值 */
    VOS_INT16                           shwSpBandLow;                             /* 语音存在的起始子带 */
    VOS_INT16                           shwSpBandHigh;                            /* 语音存在的截止子带 */
    VOS_INT16                           shwReserve;
} MED_2MIC_PROBSPABSEN_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_MODFYPROBSPABSEN_STRU
 功能描述  : 修正语音不存在概率结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                            shwPowerRatioLowLast;                   /* 上一帧低频段能量比 */
    VOS_INT16                            shwPowerRatioHighLast;                  /* 上一帧高频段能量比 */
    VOS_INT16                            ashwPsdRatioLast[MED_2MIC_NUM_CHAN_16k];/* 上一帧各子带功率谱之比 */
    VOS_INT16                           *pshwPsdRatioFreqAlpha;                  /* 功率谱之比平滑系数 */
    VOS_INT16                            shwFreqSmoothLen;                       /* 平滑长度 */
    VOS_INT16                            shwDifIndex;                            /* 中频位置 */
    VOS_INT16                            shwFarMuteCnt;
    VOS_INT16                            shwFarMuteCntThd;
} MED_2MIC_MODFYPROBSPABSEN_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_PROBSPPRESEN_STRU
 功能描述  : 计算语音存在概率结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswPostSNR[MED_2MIC_NUM_CHAN_16k];      /* 各子带后验信噪比 */
    VOS_INT32                           aswPrioriSNR[MED_2MIC_NUM_CHAN_16k];    /* 各子带先验信噪比 */
    VOS_INT32                           aswNsPsd[MED_2MIC_NUM_CHAN_16k];        /* 各子带噪声功率谱 */
    VOS_INT16                           shwNsIndex;
    VOS_INT16                           shwAllPassNum;
    VOS_INT16                           shwNsLowTh;                             /* 噪声判决低门限 */
    VOS_INT16                           shwNsHighTh;                            /* 噪声判决高门限 */
    VOS_INT16                           shwSpLowTh;                             /* 语音存在概率低门限 */
    VOS_INT16                           shwReserve;
    VOS_INT16                           shwTmosExitBandLow;                     /* Tmos方案推出统计量起始子带 */
    VOS_INT16                           shwTmosExitBandHi;                      /* Tmos方案推出统计量截止子带 */
} MED_2MIC_PROBSPPRESEN_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_SNPSDMIC_STRU
 功能描述  : 计算主麦克平稳噪声功率谱结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswMinPsd[MED_2MIC_NUM_CHAN_16k];       /* 各子带最小功率谱 */
    VOS_INT32                           aswTmpPsd[MED_2MIC_NUM_CHAN_16k];       /* 各子带功率谱 */
    VOS_INT16                           shwFrameNum;                            /* 跟踪帧数 */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];/* 语音存在概率 */
} MED_2MIC_SNPSDMIC_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_LSAGAIN_STRU
 功能描述  : 计算各子带增益的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                            shwBandLow;                             /* 低子带位置 */
    VOS_INT16                            shwGainMin;                             /* 最小增益 */
    VOS_INT16                           *pshwBandAlpha;                       /* 子带间平滑系数 */
    VOS_INT16                            shwSmoothLen;                           /* 平滑长度 */
    VOS_INT16                            shwReserve;
} MED_2MIC_LSAGAIN_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_POSTFILTER_STRU
 功能描述  : 描述2MIC频域后处理的结构体
 *****************************************************************************/
typedef struct
{
    VOS_INT16                           shwBandLen;                             /* 子带长度 */
    CODEC_SWITCH_ENUM_UINT16            enFirstFrameInitFlag;                   /* 第一帧是否初始化标志 */
    VOS_INT16                           shwFreqBinLen;                          /* 频谱长度 */
    VOS_INT16                           shwTimeAlpha;                           /* 计算平滑功率谱时的平滑系数 */
    VOS_INT16                           shwFarMaxThd;                           /* 远端信号门限，超过此门限则在FD将参考信号置零 */
    VOS_INT16                           shwReserve;
    VOS_INT32                           aswBandSY[MED_2MIC_NUM_CHAN_16k];       /* 主麦克各子带语音信号功率谱 */
    VOS_INT32                           aswBandSU[MED_2MIC_NUM_CHAN_16k];       /* 辅麦克各子带语音信号功率谱 */
    VOS_INT32                           aswBandMY[MED_2MIC_NUM_CHAN_16k];       /* 主麦克各子带平稳噪声功率谱 */
    VOS_INT32                           aswBandMU[MED_2MIC_NUM_CHAN_16k];       /* 辅麦克各子带平稳噪声功率谱 */
    VOS_INT16                           ashwProbSpAbsen[MED_2MIC_NUM_CHAN_16k]; /* 主麦克各子带语音不存在概率 */
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];/* 主麦克各子带语音存在概率 */
    VOS_INT16                           ashwLsaGain[MED_2MIC_NUM_CHAN_16k];     /* 主麦克各子带增益 */
    VOS_INT16                           ashwOmLsaGain[MED_2MIC_NUM_CHAN_16k];   /* 调整后各子带增益 */
    MED_2MIC_COMPFREQAP_STRU            stCompFreqAp;                           /* 辅麦克噪声功率谱修正结构体 */
    MED_2MIC_SNPSDMIC_STRU              stSnPsdMc;                              /* 主麦克噪声功率谱结构体 */
    MED_2MIC_SNPSDMIC_STRU              stSnPsdRc;                              /* 辅麦克噪声功率谱结构体 */
    MED_2MIC_PROBSPABSEN_STRU           stProbSpAbsen;                          /* 计算语音不存在概率结构体 */
    MED_2MIC_MODFYPROBSPABSEN_STRU      stModfyProbSpAbsen;                     /* 修正语音不存在概率结构体 */
    MED_2MIC_PROBSPPRESEN_STRU          stProbSpPresen;                         /* 计算语音存在概率结构体 */
    MED_2MIC_LSAGAIN_STRU               stLsaGain;                              /* 计算各子带增益结构体 */
} MED_2MIC_POSTFILTER_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/




/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern const VOS_INT16 g_ashwMedAnr2MicChTbl[MED_2MIC_NUM_CHAN_16k][2];
extern MED_2MIC_POSTFILTER_STRU g_stAnr2MicFdProcess;
/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID MED_2MIC_CalcBandPsd(
                        VOS_INT32               *pswPsdBin,
                        VOS_INT16                shwBandLen,
                        VOS_INT32               *pswBandPsd);
extern VOS_VOID MED_2MIC_CalcLsaGain(
                       VOS_INT32                 *pswPrioriSNR,
                       VOS_INT16                 *pshwSNRFactor,
                       VOS_INT16                 *pshwProbSpPresen,
                       MED_2MIC_LSAGAIN_STRU     *pstLsaGain,
                       VOS_INT16                  shwBandLen,
                       VOS_INT16                 *pshwOmLsaGain,
                       VOS_INT16                 *pshwLsaGain);
extern VOS_VOID MED_2MIC_CalcProbSpAbsen(
                       VOS_INT16                         *pshwProbSpPresen,
                       VOS_INT32                         *pswBandPsd,
                       VOS_INT32                         *pswBandMY,
                       VOS_INT16                          shwBandLen,
                       MED_2MIC_PROBSPABSEN_STRU         *pstProbSpAbsen,
                       VOS_INT16                         *pshwProbSpAbsen);
extern VOS_VOID MED_2MIC_CalcProbSpPresen(
                       VOS_INT32                         *pswBandPsd,
                       VOS_INT16                         *pshwProbSpAbsen,
                       VOS_INT16                         *pshwLsaGain,
                       VOS_INT16                          shwBandLen,
                       MED_2MIC_PROBSPPRESEN_STRU        *pstProbSpPresen,
                       VOS_INT16                         *pshwProbSpPresen,
                       VOS_INT16                         *pshwSNRFactor);
extern VOS_VOID MED_2MIC_CalcPsdBin(
                       MED_2MIC_FREQ_STRU      *pstFreq,
                       VOS_INT16                shwFreqBinLen,
                       VOS_INT32               *pswPsdBin);
extern VOS_VOID MED_2MIC_CalcSmoothPsd(
                       VOS_INT32               *pswBandPsd,
                       VOS_INT16                shwBandLen,
                       VOS_INT16                shwTimeAlpha,
                       VOS_INT32               *pswSmoothPsd);
extern VOS_VOID MED_2MIC_CalcSnPsd(
                       VOS_INT32               *pswSmoothPsd,
                       VOS_INT32               *pswCurrPsd,
                       VOS_INT16                shwBandLen,
                       VOS_INT32               *pswSnPsd,
                       MED_2MIC_SNPSDMIC_STRU  *pstSnPsdMic);
extern VOS_VOID MED_2MIC_CompFreqAp(
                       VOS_INT32                         *pswPsdBinMc,
                       VOS_INT32                         *pswPsdBinOriRc,
                       VOS_INT16                          shwFreqBinLen,
                       MED_2MIC_COMPFREQAP_STRU          *pstCompFreqAp,
                       VOS_INT32                         *pswPsdBinRc);
extern VOS_VOID MED_2MIC_EstiProbSpPresen(
                       VOS_INT32                         *pswBandSY,
                       VOS_INT32                         *pswBandSU,
                       VOS_INT32                         *pswBandMY,
                       VOS_INT32                         *pswBandMU,
                       VOS_INT16                          shwBandLen,
                       VOS_INT16                         *pshwProbSpPresen);
extern VOS_VOID MED_2MIC_ExtendBand2FreqBinGain(
                        VOS_INT32               *pswBandMc,
                        VOS_INT16               *pshwBandGain,
                        VOS_INT16                shwFreqBinLen,
                        VOS_INT16                shwBandLen,
                        VOS_INT16               *pshwFreqBinGain,
                        MED_2MIC_PROBSPPRESEN_STRU *pstProbSpPresen,
                        VOS_INT16                  *pshwProbSpPrese);
extern VOS_VOID MED_2MIC_FilterFrm(
                       VOS_INT16                shwFreqBinLen,
                       VOS_INT16               *pshwFreqBinGain,
                       MED_2MIC_FREQ_STRU      *pstMc);
extern VOS_VOID MED_2MIC_FreqProcessFirstFrame(
                       VOS_INT32                         *pswBandMc,
                       VOS_INT32                         *pswBandRc,
                       MED_2MIC_POSTFILTER_STRU          *pstPostFilter);
extern VOS_VOID MED_2MIC_ModifyProbSpAbsen(
                       VOS_INT32                         *pswBandSY,
                       VOS_INT32                         *pswBandSU,
                       VOS_INT16                          shwBandLen,
                       VOS_INT32                         *pswPsdBinMc,
                       VOS_INT32                         *pswPsdBinRc,
                       VOS_INT16                         *pshwProbSpAbsence,
                       MED_2MIC_MODFYPROBSPABSEN_STRU    *pstModifyProb);
extern VOS_VOID MED_2MIC_SmoothLsaGain(
                       VOS_INT16               *pshwLsaGainLast,
                       VOS_INT16               *pshwBandAlpha,
                       VOS_INT16                shwSmoothLen,
                       VOS_INT16                shwBandLow,
                       VOS_INT16                shwBandLen,
                       VOS_INT16               *pshwLsaGain);
extern VOS_INT16 MED_2MIC_WeightSum(
                       VOS_INT16               *pshwIn,
                       VOS_INT16               *pshwSmoothAlpha,
                       VOS_INT16                shwSmoothLen,
                       VOS_INT16                shwCntI);
extern VOS_UINT32 MED_ANR_2MIC_FdInit(
                       MED_2MIC_NV_STRU                 *pst2MICParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate,
                       VOS_INT16                         shwFftLen);
extern VOS_UINT32 MED_ANR_2MIC_FdProcess(
                       CODEC_SWITCH_ENUM_UINT16           enEnable,
                       MED_2MIC_DATAFLOW_MC_STRU         *pstDataFlowMc,
                       MED_2MIC_DATAFLOW_RC_STRU         *pstDataFlowRc,
                       VOS_INT16                          shwMaxFar);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_anr_2mic_fd.h */
