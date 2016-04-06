

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_anr_2mic_comm.h"
#include "med_anr_2mic_fd.h"
#include "om.h"
#include "med_aec_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 2MIC频域后滤波结构体 */
MED_2MIC_POSTFILTER_STRU                g_stAnr2MicFdProcess;

/* 宽带信号频率补偿系数 */
const VOS_INT16 g_ashwAnr2MicCompCoef[321]     =
    {8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     20577, 20577, 20577, 20577, 20577, 20577, 20577, 20577, 20577,
     29066, 29066, 29066, 29066, 29066, 29066, 29066, 29066, 29066,
     29066, 29066, 29066, 29066, 29066, 32613, 32613, 32613, 32613,
     32613, 32613, 32613, 32613, 32613, 32613, 32613, 32613, 32613,
     32613, 32613, 20577, 20577, 20577, 20577, 20577, 20577, 20577,
     20577, 20577, 20577, 20577, 20577, 20577};

/* 2Mic子带频带划分 */
const VOS_INT16 g_ashwMedAnr2MicChTbl[MED_2MIC_NUM_CHAN_16k][2] =
{
    {1,     2},
    {3,     4},
    {5,     6},
    {7,     8},
    {9,    11},
    {12,   14},
    {15,   17},
    {18,   20},
    {21,   23},
    {24,   26},
    {27,   29},
    {30,   32},
    {33,   35},
    {36,   38},
    {39,   41},
    {42,   44},
    {45,   49},
    {50,   54},
    {55,   59},
    {60,   64},
    {65,   69},
    {70,   74},
    {75,   79},
    {80,   86},
    {87,   93},
    {94,  101},
    {102, 110},
    {111, 119},
    {120, 128},
    {129, 158},
    {159, 159},
    {160, 161},
    {162, 176},
    {177, 191},
    {192, 210},
    {211, 229},
    {230, 250},
    {251, 273},
    {274, 296},
    {297, 321}
};

/* 修正语音不存在概率函数中主辅麦克功率谱之比的平滑系数 */
const VOS_INT16 g_ashwAnr2MicPsdRatioFreqAlpha[5] =
    {3277, 6553, 13107, 6553, 3277};

/* 计算OM_LSA增益时的子带间平滑系数 */
const VOS_INT16 g_ashwAnr2MicBandAlpha[3]         =
    {6553, 19660, 6553};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 MED_ANR_2MIC_FdInit(
                MED_2MIC_NV_STRU                 *pst2MICParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate,
                VOS_INT16                         shwFftLen)
{
    MED_2MIC_POSTFILTER_STRU            *pstPostFilter       = MED_2MIC_GetFdProcessPtr();
    MED_2MIC_COMPFREQAP_STRU            *pstCompFreqAp       = &(pstPostFilter->stCompFreqAp);
    MED_2MIC_SNPSDMIC_STRU              *pstSnPsdMc          = &(pstPostFilter->stSnPsdMc);
    MED_2MIC_SNPSDMIC_STRU              *pstSnPsdRc          = &(pstPostFilter->stSnPsdRc);
    MED_2MIC_PROBSPABSEN_STRU           *pstProbSpAbsen      = &(pstPostFilter->stProbSpAbsen);
    MED_2MIC_MODFYPROBSPABSEN_STRU      *pstModfyProbSpAbsen = &(pstPostFilter->stModfyProbSpAbsen);
    MED_2MIC_PROBSPPRESEN_STRU          *pstProbSpPresen     = &(pstPostFilter->stProbSpPresen);
    MED_2MIC_LSAGAIN_STRU               *pstLsaGain          = &(pstPostFilter->stLsaGain);
    VOS_INT16                            shwBandLen;
    VOS_INT16                            shwCntI;

    /* 异常入参检查 */
    if (   (MED_NULL == pst2MICParam)
        || (enSampleRate >= CODEC_SAMPLE_RATE_MODE_BUTT)
        || (shwFftLen <= 0))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 将2MIC结构体先初始化为0 */
    UCOM_MemSet(pstPostFilter, 0, sizeof(MED_2MIC_POSTFILTER_STRU));

    /* 功率谱长度 */
    pstPostFilter->shwFreqBinLen                  = (shwFftLen/2) + 1;

    /* 第一帧的信号功率谱和噪声功率谱*/
    pstPostFilter->enFirstFrameInitFlag = CODEC_SWITCH_OFF;

    /* 计算平滑功率谱时*/
    pstPostFilter->shwTimeAlpha = pst2MICParam->shwTimeAlpha;

    /* 远端信号门限，超过此门限则在FD将参考信号置零 */
    pstPostFilter->shwFarMaxThd = pst2MICParam->shwFarMaxThd2;

    /* 窄带时将功率谱划分为32个子带，宽带分为40个子带 */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        /* 窄带时将功率谱划分为32个子带 */
        pstPostFilter->shwBandLen                 = MED_2MIC_NUM_CHAN_8k;

        /* 语音频谱的低带和高带 */
        pstProbSpAbsen->shwSpBandLow              = MED_2MIC_SP_LO_BAND_8k;
        pstProbSpAbsen->shwSpBandHigh             = MED_2MIC_SP_HI_BAND_8k;

        pstProbSpPresen->shwTmosExitBandLow       = MED_2MIC_TMOS_EXIT_LOWBIN_8k;
        pstProbSpPresen->shwTmosExitBandHi        = MED_2MIC_TMOS_EXIT_HIGHBIN_8k;

        pstModfyProbSpAbsen->shwDifIndex          = MED_2MIC_Mid_BAND_8k;
    }
    else
    {
        /* 宽带时将功率谱划分为40个子带 */
        pstPostFilter->shwBandLen                 = MED_2MIC_NUM_CHAN_16k;

        /* 语音频谱的低带和高带 */
        pstProbSpAbsen->shwSpBandLow              = MED_2MIC_SP_LO_BAND_16k;
        pstProbSpAbsen->shwSpBandHigh             = MED_2MIC_SP_HI_BAND_16k;

        pstProbSpPresen->shwTmosExitBandLow       = MED_2MIC_TMOS_EXIT_LOWBIN_16k;
        pstProbSpPresen->shwTmosExitBandHi        = MED_2MIC_TMOS_EXIT_HIGHBIN_16k;

        pstModfyProbSpAbsen->shwDifIndex          = MED_2MIC_Mid_BAND_16k;
    }
    shwBandLen = pstPostFilter->shwBandLen;

    /* 频响补偿模块初始化 */
    pstCompFreqAp->pshwCompCoef                   = (VOS_INT16 *)MED_2MIC_GetCompCoefPtr();
    pstCompFreqAp->shwSampleRate                  = (VOS_INT16)enSampleRate;

    /* 初始化主辅麦克功率谱 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstSnPsdMc->aswMinPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
        pstSnPsdMc->aswTmpPsd[shwCntI] = MED_2MIC_SNPSD_MAX;

        pstSnPsdRc->aswMinPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
        pstSnPsdRc->aswTmpPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
    }

    /* 全带语音存在概率阈值 */
    pstProbSpAbsen->shwSumProbThld = pst2MICParam->shwSumProbThld;


    /* 修正语音不存在概率模块初始化 */
    pstModfyProbSpAbsen->shwPowerRatioLowLast     = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    pstModfyProbSpAbsen->shwPowerRatioHighLast    = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    pstModfyProbSpAbsen->shwFarMuteCnt            = MED_2MIC_MODFY_ABSEN_FAR_MUTE_INIT;

    pstModfyProbSpAbsen->shwFarMuteCntThd         = pst2MICParam->shwFarMuteCntThd;

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstModfyProbSpAbsen->ashwPsdRatioLast[shwCntI] = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    }

    pstModfyProbSpAbsen->pshwPsdRatioFreqAlpha    = (VOS_INT16 *)MED_2MIC_GetPsdRatioFreqAlphaPtr();
    pstModfyProbSpAbsen->shwFreqSmoothLen         = MED_2MIC_MODFY_FREQ_SMOOTH_LEN;

    /* 计算语音存在概率模块初始化 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstProbSpPresen->aswPostSNR[shwCntI] = MED_2MIC_SPPRENSEN_POSTSNR_INIT;
    }

    pstProbSpPresen->shwSpLowTh = pst2MICParam->shwSpLowTh;
    pstProbSpPresen->shwNsLowTh = pst2MICParam->shwNsLowTh;
    pstProbSpPresen->shwNsHighTh = pst2MICParam->shwNsHighTh;

    /* 计算增益模块初始化 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstPostFilter->ashwLsaGain[shwCntI] = CODEC_OP_INT16_MAX;
        pstPostFilter->ashwOmLsaGain[shwCntI] = CODEC_OP_INT16_MAX;
    }

    pstLsaGain->shwBandLow                        = MED_2MIC_LSA_LOW_BAND;
    pstLsaGain->shwGainMin                        = pst2MICParam->shwGainMin;
    pstLsaGain->pshwBandAlpha                     = (VOS_INT16 *)MED_2MIC_GetBandAlphaPtr();
    pstLsaGain->shwSmoothLen                      = MED_2MIC_LSAGAIN_FREQ_SMOOTH_LEN;

    return UCOM_RET_SUCC;
}

VOS_UINT32 MED_ANR_2MIC_FdProcess(
                CODEC_SWITCH_ENUM_UINT16           enEnable,
                MED_2MIC_DATAFLOW_MC_STRU         *pstDataFlowMc,
                MED_2MIC_DATAFLOW_RC_STRU         *pstDataFlowRc,
                VOS_INT16                          shwMaxFar)
{
    VOS_INT32                 *pswPsdBinMc        = MED_NULL;
    VOS_INT32                 *pswPsdBinRc        = MED_NULL;
    VOS_INT32                 *pswPsdBinOriRc     = MED_NULL;

    VOS_INT32                  aswBandMc[MED_2MIC_NUM_CHAN_16k];
    VOS_INT32                  aswBandRc[MED_2MIC_NUM_CHAN_16k];

    MED_2MIC_POSTFILTER_STRU  *pstPostFilter      =  MED_2MIC_GetFdProcessPtr();
    VOS_INT16                  shwFreqBinLen      =  pstPostFilter->shwFreqBinLen;
    MED_2MIC_FREQ_STRU        *pstMc              = &(pstDataFlowMc->stMc);
    MED_2MIC_FREQ_STRU        *pstRc              = &(pstDataFlowRc->stRc);
    MED_2MIC_FREQ_STRU        *pstOriRc           = &(pstDataFlowRc->stOriRc);

    MED_2MIC_COMPFREQAP_STRU  *pstCompFreqAp      = &(pstPostFilter->stCompFreqAp);
    VOS_INT16                  shwBandLen         =  pstPostFilter->shwBandLen;

    VOS_INT16                  shwTimeAlpha       =  pstPostFilter->shwTimeAlpha;
    VOS_INT32                 *pswBandSY          =  pstPostFilter->aswBandSY;
    VOS_INT32                 *pswBandSU          =  pstPostFilter->aswBandSU;
    VOS_INT32                 *pswBandMY          =  pstPostFilter->aswBandMY;
    VOS_INT32                 *pswBandMU          =  pstPostFilter->aswBandMU;
    MED_2MIC_SNPSDMIC_STRU    *pstSnPsdMc         = &(pstPostFilter->stSnPsdMc);
    MED_2MIC_SNPSDMIC_STRU    *pstSnPsdRc         = &(pstPostFilter->stSnPsdRc);

    VOS_INT16                  ashwEstiProb[MED_2MIC_NUM_CHAN_16k];
    MED_2MIC_PROBSPABSEN_STRU *pstProbSpAbsen     = &(pstPostFilter->stProbSpAbsen);
    VOS_INT16                 *pshwProbSpAbsen    =  pstPostFilter->ashwProbSpAbsen;

    MED_2MIC_MODFYPROBSPABSEN_STRU      *pstModfyProbSpAbsen
                                                  = &(pstPostFilter->stModfyProbSpAbsen);

    VOS_INT16                 *pshwLsaGain        =  pstPostFilter->ashwLsaGain;
    MED_2MIC_PROBSPPRESEN_STRU          *pstProbSpPresen
                                                  = &(pstPostFilter->stProbSpPresen);
    VOS_INT16                 *pshwProbSpPresen   =  pstPostFilter->ashwProbSpPresen;
    VOS_INT16                  ashwSNRFactor[MED_2MIC_NUM_CHAN_16k];

    VOS_INT32                 *pswPrioriSNR       =  pstProbSpPresen->aswPrioriSNR;
    MED_2MIC_LSAGAIN_STRU     *pstLsaGain         = &(pstPostFilter->stLsaGain);
    VOS_INT16                 *pshwOmLsaGain      =  pstPostFilter->ashwOmLsaGain;

    VOS_INT16                 *pshwFreqBinGain    = MED_NULL;

    /* 异常入参检查 */
    if (   (MED_NULL == pstDataFlowMc)
        || (MED_NULL == pstDataFlowRc))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 如果ANR不使能 */
    if (CODEC_SWITCH_OFF == enEnable)
    {
        return UCOM_RET_SUCC;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_2MIC_FdProcess);

    /* 内存申请 */
    pswPsdBinMc     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinRc     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinOriRc  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pshwFreqBinGain = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT16));

    if (shwMaxFar > pstPostFilter->shwFarMaxThd)
    {
        /* 如果有回声，将辅Mic信号置0，避免将回声学成噪声 */
        CODEC_OpVecSet(
                    pstRc->ashwFreq,
                    shwFreqBinLen,
                    0);
        CODEC_OpVecSet(
                    pstOriRc->ashwFreq,
                    shwFreqBinLen,
                    0);
    }

    /* 计算每根功率谱 */
    MED_2MIC_CalcPsdBin(pstMc, shwFreqBinLen, pswPsdBinMc);
    MED_2MIC_CalcPsdBin(pstRc, shwFreqBinLen, pswPsdBinRc);
    MED_2MIC_CalcPsdBin(pstOriRc, shwFreqBinLen, pswPsdBinOriRc);

    /* 对辅麦克中的噪声功率谱进行修正 */
    MED_2MIC_CompFreqAp(pswPsdBinMc,
                        pswPsdBinOriRc,
                        shwFreqBinLen,
                        pstCompFreqAp,
                        pswPsdBinRc);

    /* 频谱子带合并，计算每个子带的功率谱 */
    MED_2MIC_CalcBandPsd(pswPsdBinMc, shwBandLen, aswBandMc);
    MED_2MIC_CalcBandPsd(pswPsdBinRc, shwBandLen, aswBandRc);

    /* 第一帧初始化 */
    if (CODEC_SWITCH_OFF == pstPostFilter->enFirstFrameInitFlag)
    {
        MED_2MIC_FreqProcessFirstFrame(aswBandMc, aswBandRc, pstPostFilter);
        pstPostFilter->enFirstFrameInitFlag = CODEC_SWITCH_ON;
    }

    /* 计算主辅麦克各子带的平滑功率谱 */
    MED_2MIC_CalcSmoothPsd(aswBandMc, shwBandLen, shwTimeAlpha, pswBandSY);
    MED_2MIC_CalcSmoothPsd(aswBandRc, shwBandLen, shwTimeAlpha, pswBandSU);

    /* 计算主辅麦克各子带的噪声功率谱 */
    MED_2MIC_CalcSnPsd(pswBandSY,
                       aswBandMc,
                       shwBandLen,
                       pswBandMY,
                       pstSnPsdMc);
    MED_2MIC_CalcSnPsd(pswBandSU,
                       aswBandRc,
                       shwBandLen,
                       pswBandMU,
                       pstSnPsdRc);

    /* 估计各子带语音存在概率*/
    MED_2MIC_EstiProbSpPresen(pswBandSY,
                              pswBandSU,
                              pswBandMY,
                              pswBandMU,
                              shwBandLen,
                              ashwEstiProb);

    /* 计算主麦克各子带的语音不存在概率    */
    MED_2MIC_CalcProbSpAbsen(ashwEstiProb,
                             aswBandMc,
                             pswBandMY,
                             shwBandLen,
                             pstProbSpAbsen,
                             pshwProbSpAbsen);

    /* 修正主麦克各子带的语音不存在概率 */
    MED_2MIC_ModifyProbSpAbsen(pswBandSY,
                               pswBandSU,
                               shwBandLen,
                               aswBandMc,
                               aswBandRc,
                               pshwProbSpAbsen,
                               pstModfyProbSpAbsen);

    /* 计算主麦克各子带的语音存在概率 */
    MED_2MIC_CalcProbSpPresen(aswBandMc,
                              pshwProbSpAbsen,
                              pshwLsaGain,
                              shwBandLen,
                              pstProbSpPresen,
                              pshwProbSpPresen,
                              ashwSNRFactor);

    /* 计算主麦克各子带的增益 */
    MED_2MIC_CalcLsaGain(pswPrioriSNR,
                         ashwSNRFactor,
                         pshwProbSpPresen,
                         pstLsaGain,
                         shwBandLen,
                         pshwOmLsaGain,
                         pshwLsaGain);

    /* 将子带扩充到各个频谱带 */
    MED_2MIC_ExtendBand2FreqBinGain(aswBandMc,
                                    pshwOmLsaGain,
                                    shwFreqBinLen,
                                    shwBandLen,
                                    pshwFreqBinGain,
                                    pstProbSpPresen,
                                    pshwProbSpPresen);

    /* 对主麦克各频带施加增益来消除噪声 */
    MED_2MIC_FilterFrm(shwFreqBinLen,
                       pshwFreqBinGain,
                       pstMc);

    /* 内存释放 */
    UCOM_MemFree(pswPsdBinMc);
    UCOM_MemFree(pswPsdBinRc);
    UCOM_MemFree(pswPsdBinOriRc);
    UCOM_MemFree(pshwFreqBinGain);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_2MIC_FdProcess);

    return UCOM_RET_SUCC;
}
#ifndef _MED_C89_

VOS_VOID MED_2MIC_CalcPsdBin(
                MED_2MIC_FREQ_STRU      *pstFreq,
                VOS_INT16                shwFreqBinLen,
                VOS_INT32               *pswPsdBin)
{
    VOS_INT16  shwCntK;
    VOS_INT16  shwNormQ8;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1, aeqS2;

    VOS_INT16 *pshwFreq     = pstFreq->ashwFreq;
    VOS_INT16  shwNormShift = pstFreq->shwNormShift;

    /* 功率谱计算  */
    UCOM_MemSet(pswPsdBin, 0, (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));

    /* 计算第一根功率谱和最后一根功率谱 */
    pswPsdBin[0] = CODEC_OpL_mult(pshwFreq[0], pshwFreq[0]);
    pswPsdBin[shwFreqBinLen - 1] = CODEC_OpL_mult(pshwFreq[1], pshwFreq[1]);

    /* 计算其它功率谱 */
    for (shwCntK = 1; shwCntK < (shwFreqBinLen - 1)>>1; shwCntK++)
    {
        aepX1 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK - 2]);
        aepX2 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK - 2]);

        aepY1 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK]);
        aepY2 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK]);

        aeqS1 = AE_MULZAAFP24S_HH_LL(aepX1, aepX2);
        aeqS2 = AE_MULZAAFP24S_HH_LL(aepY1, aepY2);

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);
        aeqS2 = AE_SATQ48S(aeqS2);

        /* aeqS1[bit9 - bit47] -> pswPsdBin */
        pswPsdBin[2 * shwCntK - 1] = AE_TRUNCA32Q48(aeqS1);
        pswPsdBin[2 * shwCntK] = AE_TRUNCA32Q48(aeqS2);
    }

    aepX1 = *((ae_p16x2s *)&pshwFreq[2*(shwFreqBinLen - 2)]);
    aepX2 = *((ae_p16x2s *)&pshwFreq[2*(shwFreqBinLen - 2)]);

    aeqS1 = AE_MULZAAFP24S_HH_LL(aepX1, aepX2);
    aeqS1 = AE_SATQ48S(aeqS1);

    pswPsdBin[shwFreqBinLen - 2] = AE_TRUNCA32Q48(aeqS1);

    /* 将功率谱归一化到Q8 */
    shwNormQ8 = (2 * shwNormShift) - MED_PP_SHIFT_BY_7;
    /*CODEC_OpVecShr_r32(pswPsdBin,
                   shwFreqBinLen,
                   shwNormQ8,
                   pswPsdBin);*/
    CODEC_OpVecShr32(pswPsdBin,
        shwFreqBinLen,
        shwNormQ8,
        pswPsdBin);
}
#else

VOS_VOID MED_2MIC_CalcPsdBin(
                MED_2MIC_FREQ_STRU      *pstFreq,
                VOS_INT16                shwFreqBinLen,
                VOS_INT32               *pswPsdBin)
{
    VOS_INT16  shwCntK;
    VOS_INT16  shwNormQ8;

    VOS_INT16 *pshwFreq     = pstFreq->ashwFreq;
    VOS_INT16  shwNormShift = pstFreq->shwNormShift;

    /* 功率谱计算  */
    UCOM_MemSet(pswPsdBin, 0, (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));

    /* 计算第一根功率谱和最后一根功率谱 */
    pswPsdBin[0] = CODEC_OpL_mult(pshwFreq[0], pshwFreq[0]);
    pswPsdBin[shwFreqBinLen - 1] = CODEC_OpL_mult(pshwFreq[1], pshwFreq[1]);

    /* 计算其它功率谱 */
    for (shwCntK = 1; shwCntK < (shwFreqBinLen - 1); shwCntK++)
    {
        pswPsdBin[shwCntK] = CODEC_OpL_mac(pswPsdBin[shwCntK],
                                   pshwFreq[2 * shwCntK],
                                   pshwFreq[2 * shwCntK]);
        pswPsdBin[shwCntK] = CODEC_OpL_mac(pswPsdBin[shwCntK],
                                   pshwFreq[(2 * shwCntK) + 1],
                                   pshwFreq[(2 * shwCntK) + 1]);
    }

    /* 将功率谱归一化到Q8 */
    shwNormQ8 = (VOS_INT16)(2 * shwNormShift) - MED_PP_SHIFT_BY_7;
    /*CODEC_OpVecShr_r32(pswPsdBin,
                   shwFreqBinLen,
                   shwNormQ8,
                   pswPsdBin);*/
    CODEC_OpVecShr32(pswPsdBin,
        shwFreqBinLen,
        shwNormQ8,
        pswPsdBin);
}
#endif

VOS_VOID MED_2MIC_CompFreqAp(
                VOS_INT32                         *pswPsdBinMc,
                VOS_INT32                         *pswPsdBinOriRc,
                VOS_INT16                          shwFreqBinLen,
                MED_2MIC_COMPFREQAP_STRU          *pstCompFreqAp,
                VOS_INT32                         *pswPsdBinRc)
{
    VOS_INT16  shwCntI;
    VOS_INT32  swTemp1;
    VOS_INT32  swTemp2;
    VOS_INT16  shwTemp;

    VOS_INT16  shwSampleRate = pstCompFreqAp->shwSampleRate;
    VOS_INT16 *pshwCompCoef  = pstCompFreqAp->pshwCompCoef;

    /* 低频校正 */
    for (shwCntI = (MED_2MIC_COMP_FREQ_LOW - 1); shwCntI < MED_2MIC_COMP_FREQ_HIGH; shwCntI++)
    {
        /* 为了后面判断是否需要修正 */
        swTemp1 = CODEC_OpL_mpy_32_16(pswPsdBinOriRc[shwCntI],
                                    MED_2MIC_COMP_FREQ_THD1);
        swTemp1 = CODEC_OpL_shl(swTemp1, MED_PP_SHIFT_BY_2);
        swTemp2 = CODEC_OpL_mpy_32_16(pswPsdBinOriRc[shwCntI],
                                    MED_2MIC_COMP_FREQ_THD2);

        /* 对被抑制掉的辅麦克噪声功率谱修正 */
        if ( (pswPsdBinMc[shwCntI] < swTemp1)
          && (pswPsdBinRc[shwCntI] < swTemp2) )
        {
            pswPsdBinRc[shwCntI] = pswPsdBinOriRc[shwCntI];
        }
     }

    /* 高频频响补偿 */
    if (CODEC_SAMPLE_RATE_MODE_16000 == shwSampleRate)
    {
        for (shwCntI = 0; shwCntI < shwFreqBinLen; shwCntI++)
        {
            shwTemp = CODEC_OpMult(pshwCompCoef[shwCntI],
                                 pshwCompCoef[shwCntI]);
            pswPsdBinRc[shwCntI] = CODEC_OpL_mpy_32_16(pswPsdBinRc[shwCntI],
                                                     shwTemp);
            pswPsdBinRc[shwCntI] = CODEC_OpL_shl(pswPsdBinRc[shwCntI],
                                               MED_PP_SHIFT_BY_4);
        }
    }
}


VOS_VOID MED_2MIC_CalcBandPsd(
                 VOS_INT32               *pswPsdBin,
                 VOS_INT16                shwBandLen,
                 VOS_INT32               *pswBandPsd)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwCntJ;

    VOS_INT16 shwChLo;
    VOS_INT16 shwChHi;

    /* 子带功率谱 */
    UCOM_MemSet(pswBandPsd, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 子带范围 */
        shwChLo = g_ashwMedAnr2MicChTbl[shwCntI][0];
        shwChHi = g_ashwMedAnr2MicChTbl[shwCntI][1];

        /* 子带范围内的功率谱求和 */
        for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
        {
            pswBandPsd[shwCntI] = CODEC_OpL_add(pswBandPsd[shwCntI],
                                              pswPsdBin[shwCntJ]);
        }

        /* 子带内功率谱取平均 */
        pswBandPsd[shwCntI] /= ((shwChHi - shwChLo) + 1);

        /* 限定最小值 */
        pswBandPsd[shwCntI] = CODEC_OpL_max(pswBandPsd[shwCntI],
                                          MED_2MIC_MIN_BIN_ENRG);
    }
}
VOS_VOID MED_2MIC_FreqProcessFirstFrame(
                VOS_INT32                         *pswBandMc,
                VOS_INT32                         *pswBandRc,
                MED_2MIC_POSTFILTER_STRU          *pstPostFilter)
{
    /* 主麦克平稳信号功率谱 */
    UCOM_MemCpy(pstPostFilter->aswBandSY,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* 主麦克平稳噪声功率谱 */
    UCOM_MemCpy(pstPostFilter->aswBandMY,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* 噪声功率谱 */
    UCOM_MemCpy(pstPostFilter->stProbSpPresen.aswNsPsd,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

     /* 辅麦克信号功率谱 */
    UCOM_MemCpy(pstPostFilter->aswBandSU,
                pswBandRc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* 辅麦克平稳噪声功率谱 */
    UCOM_MemCpy(pstPostFilter->aswBandMU,
                pswBandRc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

}
VOS_VOID MED_2MIC_CalcSmoothPsd(
                VOS_INT32               *pswBandPsd,
                VOS_INT16                shwBandLen,
                VOS_INT16                shwTimeAlpha,
                VOS_INT32               *pswSmoothPsd)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwSubAlpha;

    shwSubAlpha = CODEC_OP_INT16_MAX - shwTimeAlpha;
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 帧间平滑 */
        pswSmoothPsd[shwCntI] = CODEC_OpLSmooth(pswSmoothPsd[shwCntI],
                                              shwTimeAlpha,
                                              pswBandPsd[shwCntI],
                                              shwSubAlpha);

        /* 设置下限，防止除0 */
        pswSmoothPsd[shwCntI] = CODEC_OpL_max(pswSmoothPsd[shwCntI],
                                           MED_2MIC_MIN_BIN_ENRG);
    }
}


VOS_VOID MED_2MIC_CalcSnPsd(
                VOS_INT32               *pswSmoothPsd,
                VOS_INT32               *pswCurrPsd,
                VOS_INT16                shwBandLen,
                VOS_INT32               *pswSnPsd,
                MED_2MIC_SNPSDMIC_STRU  *pstSnPsdMic)
{
    VOS_INT16 shwCntI;
    VOS_INT32 swPsdThd;
    VOS_INT16 shwTmpAlpha;
    VOS_INT16 ashwSpFlag[MED_2MIC_NUM_CHAN_16k];

    VOS_INT16  shwFrameNum      = pstSnPsdMic->shwFrameNum;
    VOS_INT32 *pswMinPsd        = pstSnPsdMic->aswMinPsd;
    VOS_INT32 *pswTmpPsd        = pstSnPsdMic->aswTmpPsd;
    VOS_INT16 *pshwProbSpPresen = pstSnPsdMic->ashwProbSpPresen;

    /* 更新估计的最小功率谱 目前算法的跟踪速度为50帧 */
    if (0 != (shwFrameNum % MED_2MIC_SNPSD_STATE_LEN))
    {
        /* 跟踪当前50帧内的最小功率谱 */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            pswMinPsd[shwCntI] = CODEC_OpL_min(pswMinPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
            pswTmpPsd[shwCntI] = CODEC_OpL_min(pswTmpPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
        }
    }
    else
    {
        /* 到达50帧，更新最小功率谱 */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            pswMinPsd[shwCntI] = CODEC_OpL_min(pswTmpPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
            pswTmpPsd[shwCntI] = pswSmoothPsd[shwCntI];
        }
    }

    /* 记录跟踪的帧数 */
    pstSnPsdMic->shwFrameNum = CODEC_OpAdd(pstSnPsdMic->shwFrameNum, 1);

    UCOM_MemSet(ashwSpFlag, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    /* 计算每个子带的语音存在概率 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swPsdThd = CODEC_OpL_mpy_32_16(pswMinPsd[shwCntI], MED_2MIC_SNPSD_PROB_THD);
        swPsdThd = CODEC_OpL_shl(swPsdThd, MED_PP_SHIFT_BY_2);

        /* 信噪比较大 */
        if (pswCurrPsd[shwCntI] > swPsdThd)
        {
            ashwSpFlag[shwCntI] = CODEC_OP_INT16_MAX;
        }
    }

    /* 计算语音存在概率 */
    CODEC_OpVvFormWithCoef(pshwProbSpPresen,
                         ashwSpFlag,
                         shwBandLen,
                         MED_2MIC_SNPSD_PROB_ALPHA,
                         MED_2MIC_SNPSD_PROB_SUB_ALPHA,
                         pshwProbSpPresen);

    /* 计算平稳噪声功率谱 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* MED_2MIC_SNPSD_ALPHA代表平稳噪声功率谱的更新速度，越大更新越慢 */
        shwTmpAlpha = CODEC_OpAdd(MED_2MIC_SNPSD_ALPHA,
                                CODEC_OpMult(MED_2MIC_SNPSD_SUB_ALPHA,
                                           pshwProbSpPresen[shwCntI]));

        /* 时域平滑 */
        pswSnPsd[shwCntI] = CODEC_OpLSmooth(pswSnPsd[shwCntI],
                                          shwTmpAlpha,
                                          pswCurrPsd[shwCntI],
                                         (CODEC_OP_INT16_MAX - shwTmpAlpha));

        /* 限制最小值，防止后面除0 */
        pswSnPsd[shwCntI] = CODEC_OpL_max(pswSnPsd[shwCntI],
                                        MED_2MIC_MIN_BIN_ENRG);
    }
}
VOS_VOID MED_2MIC_EstiProbSpPresen(
                VOS_INT32                         *pswBandSY,
                VOS_INT32                         *pswBandSU,
                VOS_INT32                         *pswBandMY,
                VOS_INT32                         *pswBandMU,
                VOS_INT16                          shwBandLen,
                VOS_INT16                         *pshwProbSpPresen)
{
    VOS_INT16 shwCntK;
    VOS_INT16 ashwTransRatio[MED_2MIC_NUM_CHAN_16k];
    VOS_INT32 swTemp1;
    VOS_INT32 swTemp2;
    VOS_INT32 swPsdMcThd;
    VOS_INT32 swPsdRcThd;

    /* 计算主辅麦克局部非平稳性和主辅麦克瞬时变化率 */
    UCOM_MemSet(ashwTransRatio, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
    {
        swTemp1 = CODEC_OpL_sub(pswBandSY[shwCntK], pswBandMY[shwCntK]);
        swTemp2 = CODEC_OpL_sub(pswBandSU[shwCntK], pswBandMU[shwCntK]);

        /* 限制分母的最小值，防止除0 */
        swTemp2 = CODEC_OpL_max(swTemp2, MED_2MIC_MIN_BIN_ENRG);

        /* 这种情况时信噪比很低，语音存在概率为0。而且可以降低除法判断正负的计算量 */
        if ((swTemp1 <= 0)
         || (swTemp2 < 0))
         {
             ashwTransRatio[shwCntK] = 0;
         }
         else
         {
             ashwTransRatio[shwCntK] = CODEC_OpRound(CODEC_OpNormDiv_32(swTemp1,
                                                                    swTemp2,
                                                                    MED_PP_SHIFT_BY_29));
         }
    }

    /* 估计各子带语音存在概率 */
    UCOM_MemSet(pshwProbSpPresen, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
    {
        swPsdMcThd = CODEC_OpL_mpy_32_16(pswBandMY[shwCntK],
                                       MED_2MIC_ABSEN_LNS_RATIO_THD);
        swPsdMcThd = CODEC_OpL_shl(swPsdMcThd,
                                 MED_PP_SHIFT_BY_2);
        swPsdRcThd = CODEC_OpL_mpy_32_16(pswBandMU[shwCntK],
                                       MED_2MIC_ABSEN_LNS_RATIO_THD);
        swPsdRcThd = CODEC_OpL_shl(swPsdRcThd,
                                 MED_PP_SHIFT_BY_2);

        /* 主麦克信噪比小，判为噪声 */
        if (pswBandSY[shwCntK] <= swPsdMcThd)
        {
            pshwProbSpPresen[shwCntK] = 0;
        }
        /* 辅麦克信噪比小，判为语音 */
        else if (pswBandSU[shwCntK] <= swPsdRcThd)
        {
            pshwProbSpPresen[shwCntK] = MED_2MIC_SPABSEN_Q13;
        }
        /* 根据主辅麦克瞬时变化率来计算 */
        else
        {
            if (ashwTransRatio[shwCntK] < MED_2MIC_ABSEN_TRANS_RATIO_LOW)
            {
                pshwProbSpPresen[shwCntK] = 0;
            }
            else if (ashwTransRatio[shwCntK] > MED_2MIC_ABSEN_TRANS_RATIO_HIGH)
            {
                pshwProbSpPresen[shwCntK] = MED_2MIC_SPABSEN_Q13;
            }
            else
            {
                pshwProbSpPresen[shwCntK] = CODEC_OpShr(CODEC_OpSub(ashwTransRatio[shwCntK],
                                                                MED_2MIC_ABSEN_TRANS_RATIO_LOW),
                                                      MED_PP_SHIFT_BY_1);
            }
        }
    }
}
VOS_VOID MED_2MIC_CalcProbSpAbsen(
                VOS_INT16                         *pshwProbSpPresen,
                VOS_INT32                         *pswBandPsd,
                VOS_INT32                         *pswBandMY,
                VOS_INT16                          shwBandLen,
                MED_2MIC_PROBSPABSEN_STRU         *pstProbSpAbsen,
                VOS_INT16                         *pshwProbSpAbsen)
{
    VOS_INT16 shwCntK;
    VOS_INT16 shwSumProbThld;
    VOS_INT16 shwSpBandLow;
    VOS_INT16 shwSpBandHigh;
    VOS_INT16 shwSumProb;
    VOS_INT16 ashwPostSNR[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16 shwTemp1;
    VOS_INT16 shwTemp2;

    /* 全带语音不存在判定阈值 */
    shwSumProbThld = pstProbSpAbsen->shwSumProbThld;

    /* 语音存在范围的最低子带和最高子带 */
    shwSpBandLow  = pstProbSpAbsen->shwSpBandLow;
    shwSpBandHigh = pstProbSpAbsen->shwSpBandHigh;



    /* 计算全带语音存在概率 */
    shwSumProb = 0;
    for (shwCntK = (shwSpBandLow - 1); shwCntK < shwSpBandHigh; shwCntK++)
    {
        shwSumProb = CODEC_OpAdd(shwSumProb, pshwProbSpPresen[shwCntK]);
    }

    /* 虽然shwSumProb定标为[16bit Q15]会溢出，但是不会影响后面的判断。 */
    shwSumProb =  CODEC_OpSaturate(CODEC_OpNormDiv_32((VOS_INT32)shwSumProb,
                                              (VOS_INT32)((shwSpBandHigh - shwSpBandLow) + 1),
                                              MED_PP_SHIFT_BY_2));

    /* 计算各子带语音不存在概率 */
    UCOM_MemSet(ashwPostSNR, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    /* 存在语音的可能性较大 */
    if (shwSumProb > shwSumProbThld)
    {
        for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
        {
            ashwPostSNR[shwCntK] = CODEC_OpRound(CODEC_OpNormDiv_32(pswBandPsd[shwCntK],
                                                                pswBandMY[shwCntK],
                                               MED_PP_SHIFT_BY_28));
          /* 后验信噪比小，语音不存在 */
          if (ashwPostSNR[shwCntK] <= MED_2MIC_ABSEN_POST_SNR_THLD)
          {
              pshwProbSpAbsen[shwCntK] = CODEC_OP_INT16_MAX;
          }
          else
          {
              shwTemp1 = CODEC_OpSub(MED_2MIC_ABSEN_POST_SNR_ALPHA,
                                   ashwPostSNR[shwCntK]);

              /* 利用语音存在概率估计语音不存在概率*/
              pshwProbSpAbsen[shwCntK] = CODEC_OpSub(CODEC_OP_INT16_MAX,
                                                   CODEC_OpShl(pshwProbSpPresen[shwCntK],
                                                             MED_PP_SHIFT_BY_2));
              if (shwTemp1 > 0)
              {
                  /* 利用后验信噪比估计语音不存在概率 */
                  shwTemp2 = CODEC_OpSaturate(CODEC_OpNormDiv_32((VOS_INT32)shwTemp1,
                                                         (VOS_INT32)MED_2MIC_ABSEN_POST_SNR_SUB_ALPHA,
                                                         MED_PP_SHIFT_BY_15));
                  pshwProbSpAbsen[shwCntK] = CODEC_OpMax(shwTemp2,
                                                       pshwProbSpAbsen[shwCntK]);
              }
          }
        }
    }
    /* 噪声帧 */
    else
    {
        for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
        {
            pshwProbSpAbsen[shwCntK] = CODEC_OP_INT16_MAX;
        }
    }
}



VOS_VOID MED_2MIC_ModifyProbSpAbsen(
                VOS_INT32                         *pswBandSY,
                VOS_INT32                         *pswBandSU,
                VOS_INT16                          shwBandLen,
                VOS_INT32                         *pswPsdBinMc,
                VOS_INT32                         *pswPsdBinRc,
                VOS_INT16                         *pshwProbSpAbsence,
                MED_2MIC_MODFYPROBSPABSEN_STRU    *pstModifyProb)
{
    VOS_INT16  shwCntI;
    VOS_INT16  shwPowerRatioLowLast;
    VOS_INT16  shwPowerRatioHighLast;
    VOS_INT16 *pshwPsdRatioLast         = pstModifyProb->ashwPsdRatioLast;
    VOS_INT16 *pshwPsdRatioFreqAlpha    = pstModifyProb->pshwPsdRatioFreqAlpha;
    VOS_INT16  shwFreqSmoothLen;
    VOS_INT16  ashwMainToRefPsdRatio[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  ashwTmpMainToRefPsdRatio[(MED_2MIC_NUM_CHAN_16k + MED_2MIC_MODFY_FREQ_SMOOTH_LEN) - 1];
    VOS_INT16  shwHalfSmoothLen;
    VOS_INT16  shwSubAlpha;
    VOS_INT16  shwDifIndex;
    VOS_INT32  swSumMc;
    VOS_INT32  swSumRc;
    VOS_INT16  shwTemp;
    VOS_INT16  shwMainToRefPowerRatioLow;
    VOS_INT16  shwMainToRefPowerRatioHigh;
    VOS_INT16  ashwTmpPsdRatio[MED_2MIC_NUM_CHAN_16k];

    MED_AEC_OBJ_STRU *pstAECObj = MED_AEC_GetObjPtr();


    /* 获取历史信息 */
    shwPowerRatioLowLast  =  pstModifyProb->shwPowerRatioLowLast;
    shwPowerRatioHighLast =  pstModifyProb->shwPowerRatioHighLast;
    pshwPsdRatioLast      =  pstModifyProb->ashwPsdRatioLast;

    pshwPsdRatioFreqAlpha =  pstModifyProb->pshwPsdRatioFreqAlpha;
    shwFreqSmoothLen      =  pstModifyProb->shwFreqSmoothLen;

    /* 计算各个子带的主辅麦克平滑功率谱比率 */
    UCOM_MemSet(ashwMainToRefPsdRatio, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    UCOM_MemSet(ashwTmpPsdRatio, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        ashwMainToRefPsdRatio[shwCntI] = CODEC_OpRound(CODEC_OpNormDiv_32(pswBandSY[shwCntI],
                                                                       pswBandSU[shwCntI],
                                                                       MED_PP_SHIFT_BY_26));
    }

    /* 平滑系数长度必须为奇数 */
    UCOM_MemSet(ashwTmpMainToRefPsdRatio, 0,
                ((MED_2MIC_NUM_CHAN_16k + MED_2MIC_MODFY_FREQ_SMOOTH_LEN) - 1) * sizeof(VOS_INT16));

    shwHalfSmoothLen = CODEC_OpShr((shwFreqSmoothLen - 1), MED_PP_SHIFT_BY_1);

    /* 补齐开头和结尾，为频域平滑作准备 */
    for (shwCntI = 0; shwCntI < shwHalfSmoothLen; shwCntI++)
    {
        ashwTmpMainToRefPsdRatio[shwCntI] = ashwMainToRefPsdRatio[0];
        ashwTmpMainToRefPsdRatio[shwCntI + shwBandLen + shwHalfSmoothLen]
                                          = ashwMainToRefPsdRatio[shwBandLen - 1];
    }

    /* 拷贝中间元素 */
    CODEC_OpVecCpy(&ashwTmpMainToRefPsdRatio[shwCntI], ashwMainToRefPsdRatio, shwBandLen);

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwTemp = 0;
        /* 子带间平滑 */
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI],
                                                pshwPsdRatioFreqAlpha[0]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 1],
                                                pshwPsdRatioFreqAlpha[1]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 2],
                                                pshwPsdRatioFreqAlpha[2]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 3],
                                                pshwPsdRatioFreqAlpha[3]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 4],
                                                pshwPsdRatioFreqAlpha[4]));
        ashwTmpPsdRatio[shwCntI] = shwTemp;

    }

    /* 帧间平滑 */
    shwSubAlpha = CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA;
    CODEC_OpVvFormWithCoef(pshwPsdRatioLast,
                         ashwTmpPsdRatio,
                         shwBandLen,
                         MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA,
                         shwSubAlpha,
                         ashwMainToRefPsdRatio);


    /* 高低带分界点为全频带的中间值 */
    shwDifIndex = pstModifyProb->shwDifIndex;

    /* 计算低频子带主辅麦克能量比 */
    swSumMc = 0;
    swSumRc = 0;
    for (shwCntI = 0; shwCntI < shwDifIndex; shwCntI++)
    {
        swSumMc = CODEC_OpL_add(swSumMc, pswPsdBinMc[shwCntI]);
        swSumRc = CODEC_OpL_add(swSumRc, pswPsdBinRc[shwCntI]);
    }

    shwTemp = CODEC_OpMult(CODEC_OpRound(CODEC_OpNormDiv_32(swSumMc,
                                                       swSumRc,
                                                       MED_PP_SHIFT_BY_26)),
                         (CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA));
    shwMainToRefPowerRatioLow = CODEC_OpAdd(CODEC_OpMult(shwPowerRatioLowLast,
                                                     MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA),
                                          shwTemp);

    /* 计算高频子带主辅麦克能量比 */
    swSumMc = 0;
    swSumRc = 0;
    for (shwCntI = shwDifIndex; shwCntI < shwBandLen; shwCntI++)
    {
        swSumMc = CODEC_OpL_add(swSumMc, pswPsdBinMc[shwCntI]);
        swSumRc = CODEC_OpL_add(swSumRc, pswPsdBinRc[shwCntI]);
    }

    shwTemp = CODEC_OpMult(CODEC_OpRound(CODEC_OpNormDiv_32(swSumMc,
                                                       swSumRc,
                                                       MED_PP_SHIFT_BY_26)),
                         (CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA));
    shwMainToRefPowerRatioHigh = CODEC_OpAdd(CODEC_OpMult(shwPowerRatioHighLast,
                                                       MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA),
                                           shwTemp);

    /* 修正先验语音不存在概率 */
    if (pstAECObj->stDtd.enDtdFlag < MED_AEC_DTD_FLAG_PASS)
    {
        pstModifyProb->shwFarMuteCnt = 0;
    }
    else
    {
        pstModifyProb->shwFarMuteCnt = pstModifyProb->shwFarMuteCnt + 1;
    }

    if (pstModifyProb->shwFarMuteCnt >= pstModifyProb->shwFarMuteCntThd)
    {
        pstModifyProb->shwFarMuteCnt = pstModifyProb->shwFarMuteCntThd;

        /* 低带主辅麦克能量比较小，可能存在误判 */
        if (shwMainToRefPowerRatioLow < MED_2MIC_MODFY_ABSEN_POWER_RATIO_LOW)
        {
            for (shwCntI = 0; shwCntI < shwDifIndex; shwCntI++)
            {
                /* 当前子带的主辅麦克功率谱比小，修正为噪声 */
                if (ashwMainToRefPsdRatio[shwCntI] < MED_2MIC_MODFY_ABSEN_PSD_RATIO_LOW)
                {
                    pshwProbSpAbsence[shwCntI] = CODEC_OP_INT16_MAX;
                }
            }
        }

        /* 高带主辅麦克能量比较小，可能存在误判 */
        if (shwMainToRefPowerRatioHigh < MED_2MIC_MODFY_ABSEN_POWER_RATIO_HIGH)
        {
            for (shwCntI = shwDifIndex; shwCntI < shwBandLen; shwCntI++)
            {
                /* 当前子带的主辅麦克功率谱比小，修正为噪声 */
                if (ashwMainToRefPsdRatio[shwCntI] < MED_2MIC_MODFY_ABSEN_PSD_RATIO_HIGH)
                {
                    pshwProbSpAbsence[shwCntI] = CODEC_OP_INT16_MAX;
                }
            }
        }
    }

    /* 更新历史信息 */
    pstModifyProb->shwPowerRatioLowLast  = shwMainToRefPowerRatioLow;
    pstModifyProb->shwPowerRatioHighLast = shwMainToRefPowerRatioHigh;
    UCOM_MemCpy(pstModifyProb->ashwPsdRatioLast, ashwMainToRefPsdRatio, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));
}



VOS_INT16 MED_2MIC_WeightSum(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwSmoothAlpha,
                VOS_INT16                shwSmoothLen,
                VOS_INT16                shwCntI)
{
    VOS_INT16 shwCntK;
    VOS_INT16 shwSmoothOut;
    VOS_INT16 shwTemp;

    shwSmoothOut = 0;
    for (shwCntK = 0; shwCntK < shwSmoothLen; shwCntK++)
    {
        shwTemp = CODEC_OpMult(pshwIn[shwCntI + shwCntK ],
                             pshwSmoothAlpha[shwCntK]);
        shwSmoothOut = CODEC_OpAdd(shwSmoothOut,
                                      shwTemp);
    }
    return shwSmoothOut;
}



VOS_VOID MED_2MIC_CalcProbSpPresen(
                VOS_INT32                         *pswBandPsd,
                VOS_INT16                         *pshwProbSpAbsen,
                VOS_INT16                         *pshwLsaGain,
                VOS_INT16                          shwBandLen,
                MED_2MIC_PROBSPPRESEN_STRU        *pstProbSpPresen,
                VOS_INT16                         *pshwProbSpPresen,
                VOS_INT16                         *pshwSNRFactor)
{
    VOS_INT16  shwCntI;
    VOS_INT32 *pswNsPsd       = MED_NULL;
    VOS_INT32 *pswPostSNR     = MED_NULL;
    VOS_INT32 *pswPostSNRLast = MED_NULL;
    VOS_INT32 *pswPrioriSNR   = MED_NULL;
    VOS_INT16  shwTemp1;
    VOS_INT32  swTemp1;
    VOS_INT32  swTemp2;
    VOS_INT32  swExp;
    VOS_INT16  ashwNsPsdAlphaD[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  ashwGainTmp[MED_2MIC_NUM_CHAN_16k];

    /* 内存申请 */
    pswPostSNRLast = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    UCOM_MemSet(pswPostSNRLast , 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));


    /* 获取历史信息 */
    pswPrioriSNR    = pstProbSpPresen->aswPrioriSNR;
    pswNsPsd        = pstProbSpPresen->aswNsPsd;
    pswPostSNR      = pstProbSpPresen->aswPostSNR;

    UCOM_MemCpy(pswPostSNRLast, pswPostSNR, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    /* 计算各子带后验信噪比 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pswPostSNR[shwCntI] =  CODEC_OpNormDiv_32(pswBandPsd[shwCntI],
                                                pswNsPsd[shwCntI],
                                                MED_PP_SHIFT_BY_8);
        /* 限制最小值，防止除0 */
        pswPostSNR[shwCntI] = CODEC_OpL_max(pswPostSNR[shwCntI],
                                          MED_2MIC_SPPRENSEN_POSTSNR_MIN);
    }

    /* 计算先验信噪比和信噪比因子 */
    UCOM_MemSet(pswPrioriSNR , 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    UCOM_MemSet(pshwSNRFactor, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    CODEC_OpVvMult(pshwLsaGain, pshwLsaGain, shwBandLen, ashwGainTmp);
    CODEC_OpVcMult(ashwGainTmp, shwBandLen, MED_2MIC_SPPRENSEN_PRIORSNR_ALPHA, ashwGainTmp);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp1 = CODEC_OpL_mpy_32_16(pswPostSNRLast[shwCntI], ashwGainTmp[shwCntI]);
        swTemp2 = CODEC_OpL_sub(pswPostSNR[shwCntI], MED_2MIC_SPPRENSEN_Q8);
        swTemp2 = CODEC_OpL_max(swTemp2,0);
        swTemp2 = CODEC_OpL_mpy_32_16(swTemp2, MED_2MIC_SPPRENSEN_PRIORSNR_SUB_ALPHA);

        pswPrioriSNR[shwCntI] = CODEC_OpL_add(swTemp1, swTemp2);

        /* 限制最小值，很重要 */
        pswPrioriSNR[shwCntI] = CODEC_OpL_max(pswPrioriSNR[shwCntI],
                                            MED_2MIC_SPPRENSEN_PRIORSNR_MIN);

        swTemp1 = CODEC_OpL_add(MED_2MIC_SPPRENSEN_Q8, pswPrioriSNR[shwCntI]);
        swTemp2 = CODEC_OpNormDiv_32(pswPrioriSNR[shwCntI], swTemp1, MED_PP_SHIFT_BY_31);
        shwTemp1 = CODEC_OpRound(swTemp2);

        swTemp2 = CODEC_OpL_mpy_32_16(pswPostSNR[shwCntI], shwTemp1);
        swTemp1 = CODEC_OpL_shl(swTemp2, MED_PP_SHIFT_BY_3);
        pshwSNRFactor[shwCntI] = CODEC_OpSaturate(swTemp1);

        /* 限定信噪比因子的最大值 */
        pshwSNRFactor[shwCntI] = CODEC_OpMin(pshwSNRFactor[shwCntI],
                                           MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX);
    }

    /* 计算各子带语音存在概率 */
    UCOM_MemSet(pshwProbSpPresen, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 语音不存在概率小，语音存在可能性大 */
        if (pshwProbSpAbsen[shwCntI] < MED_2MIC_SPPRENSEN_PROBZEROTHD)
        {
            /* 语音帧 */
            if ((0 == pshwProbSpAbsen[shwCntI])
             || (MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX == pshwSNRFactor[shwCntI]))
             {
                 pshwProbSpPresen[shwCntI] = CODEC_OP_INT16_MAX;
             }
             /* 根据信噪比因子计算语音存在概率 */
             else
             {
                 swTemp1 = CODEC_OpNormDiv_32((VOS_INT32)pshwProbSpAbsen[shwCntI],
                                            (VOS_INT32)(CODEC_OP_INT16_MAX - pshwProbSpAbsen[shwCntI]),
                                            MED_PP_SHIFT_BY_11);
                 swTemp2 = CODEC_OpL_mpy_32_16(CODEC_OpL_shl(CODEC_OpL_add(MED_2MIC_SPPRENSEN_Q8,
                                                                      pswPrioriSNR[shwCntI]),
                                                          MED_PP_SHIFT_BY_4),
                                             (VOS_INT16)swTemp1);

                 swExp = CODEC_OpL_Exp(pshwSNRFactor[shwCntI],
                                     MED_PP_SHIFT_BY_11,
                                     MED_PP_SHIFT_BY_8);
                 swTemp2 = CODEC_OpL_add(swTemp2, swExp);
                 pshwProbSpPresen[shwCntI] = CODEC_OpRound(CODEC_OpNormDiv_32(swExp,
                                                                          swTemp2,
                                                                          MED_PP_SHIFT_BY_31));
             }
        }
    }

    /* 更新噪声功率谱 */
    CODEC_OpVecSet(ashwNsPsdAlphaD, shwBandLen, MED_2MIC_SPPRENSEN_NSPSD_ALPHA);
    CODEC_OpVcMultQ15Add(ashwNsPsdAlphaD,
                       pshwProbSpPresen,
                       shwBandLen,
                       MED_2MIC_SPPRENSEN_NSPSD_SUB_ALPHA,
                       ashwNsPsdAlphaD);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp1 = CODEC_OpLSmooth(pswNsPsd[shwCntI],
                                ashwNsPsdAlphaD[shwCntI],
                                pswBandPsd[shwCntI],
                                (CODEC_OP_INT16_MAX - ashwNsPsdAlphaD[shwCntI]));
        pswNsPsd[shwCntI] = CODEC_OpL_mpy_32_16(swTemp1,
                                              MED_2MIC_SPPRENSEN_NSPSD_BETA);
    }

    /* 内存释放 */
    UCOM_MemFree(pswPostSNRLast);
}
VOS_VOID MED_2MIC_CalcLsaGain(
                VOS_INT32                 *pswPrioriSNR,
                VOS_INT16                 *pshwSNRFactor,
                VOS_INT16                 *pshwProbSpPresen,
                MED_2MIC_LSAGAIN_STRU     *pstLsaGain,
                VOS_INT16                  shwBandLen,
                VOS_INT16                 *pshwOmLsaGain,
                VOS_INT16                 *pshwLsaGain)
{
    VOS_INT16  shwCntI;
    VOS_INT16  shwCntJ;
    VOS_INT16  shwBandHigh;
    VOS_INT16  ashwOmLsaGain[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwBandLow;
    VOS_INT16 *pshwBandAlpha = MED_NULL;
    VOS_INT16  shwSmoothLen;
    VOS_INT16  shwGainMin;
    VOS_INT16  ashwExpIntegral[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwExp;
    VOS_INT16  ashwSubProbSpPresen[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwPower1;
    VOS_INT16  shwPower2;
    VOS_INT16  shwDelta;
    VOS_INT32  swTemp;
    VOS_INT32  swSumTmp;
    VOS_INT16  shwMultTmp;
    VOS_INT16  shwGainMinTmp;
    VOS_INT16  shwMeanGain;
    VOS_INT16  shwOmLsaGainMin;

    shwBandLow    = pstLsaGain->shwBandLow;
    pshwBandAlpha = pstLsaGain->pshwBandAlpha;
    shwSmoothLen  = pstLsaGain->shwSmoothLen;
    shwGainMin    = pstLsaGain->shwGainMin;

    /* 计算积分 */
    UCOM_MemSet(ashwExpIntegral, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 计算积分 */
        swTemp = CODEC_OpL_Integral(pshwSNRFactor[shwCntI]);
        ashwExpIntegral[shwCntI] = CODEC_OpRound(swTemp);
    }

    /* 计算 LSA gain */
    UCOM_MemSet(pshwLsaGain,
                0,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwExp = CODEC_OpSaturate(CODEC_OpL_Exp(ashwExpIntegral[shwCntI],
                                            MED_PP_SHIFT_BY_12,
                                            MED_PP_SHIFT_BY_13));
        swTemp = CODEC_OpNormDiv_32(pswPrioriSNR[shwCntI],
                                  (pswPrioriSNR[shwCntI] + MED_2MIC_LSAGAIN_Q8),
                                  MED_PP_SHIFT_BY_31);
        pshwLsaGain[shwCntI] = CODEC_OpRound(CODEC_OpL_mpy_32_16(swTemp, shwExp));
        pshwLsaGain[shwCntI] = CODEC_OpShl(pshwLsaGain[shwCntI], MED_PP_SHIFT_BY_2);
    }

    /* 限定LSA gain的最小值 */
    shwMultTmp           = CODEC_OpMult(shwGainMin, MED_2MIC_LSAGAIN_MIN_ALPHA);
    for (shwCntI = 0; shwCntI < shwBandLow; shwCntI++)
    {
        pshwLsaGain[shwCntI] = CODEC_OpMax(pshwLsaGain[shwCntI], shwMultTmp);
    }

    for (shwCntI = shwBandLow; shwCntI < shwBandLen; shwCntI++)
    {
        pshwLsaGain[shwCntI] = CODEC_OpMax(pshwLsaGain[shwCntI], shwGainMin);
    }

    /* 计算 OM-LSA gain */
    UCOM_MemSet(ashwOmLsaGain,
                0,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    CODEC_OpVecSet(ashwSubProbSpPresen,
                 shwBandLen,
                 CODEC_OP_INT16_MAX);

    /* 1 - pshwProbSpPresen */
    CODEC_OpVvSub(ashwSubProbSpPresen, pshwProbSpPresen, shwBandLen, ashwSubProbSpPresen);

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp = CODEC_OpL_Power(pshwLsaGain[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               pshwProbSpPresen[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               MED_PP_SHIFT_BY_31);
        shwPower1 = CODEC_OpRound(swTemp);
        swTemp = CODEC_OpL_Power(shwGainMin,
                               MED_PP_SHIFT_BY_15,
                               ashwSubProbSpPresen[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               MED_PP_SHIFT_BY_15);
        shwPower2 = CODEC_OpSaturate(swTemp);
        ashwOmLsaGain[shwCntI] = CODEC_OpMult(shwPower1, shwPower2);
    }

    /* 平滑OM-LSA gain */
    MED_2MIC_SmoothLsaGain(pshwOmLsaGain,
                           pshwBandAlpha,
                           shwSmoothLen,
                           shwBandLow,
                           shwBandLen,
                           ashwOmLsaGain);

    /* 限定ashwOmLsaGain的最小值 */
    shwDelta = MED_2MIC_LSA_DELTA;
    shwBandHigh = shwBandLen - shwDelta;
    shwGainMinTmp = CODEC_OpShl(CODEC_OpMult(MED_2MIC_LSAGAIN_MIN_BETA,
                                                         shwGainMin),
                                                         MED_PP_SHIFT_BY_3);

    for (shwCntI = shwBandLow; shwCntI < shwBandHigh; shwCntI++)
    {
        swSumTmp = 0;
        shwCntJ = shwCntI - shwDelta;
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 2]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 3]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 4]));

        /* 求平均增益 */
        shwMeanGain = CODEC_OpExtract_l(CODEC_OpL_mpy_32_16(swSumTmp,
                                                       MED_2MIC_LSA_MEAN_COEF));

        shwOmLsaGainMin = CODEC_OpMin(shwMeanGain, shwGainMinTmp);
        ashwOmLsaGain[shwCntI] = CODEC_OpMax(ashwOmLsaGain[shwCntI],
                                           shwOmLsaGainMin);
    }

    /* 更新历史信息，以便下一帧使用 */
    CODEC_OpVecCpy(pshwOmLsaGain, ashwOmLsaGain, shwBandLen);
}


VOS_VOID MED_2MIC_SmoothLsaGain(
                VOS_INT16               *pshwLsaGainLast,
                VOS_INT16               *pshwBandAlpha,
                VOS_INT16                shwSmoothLen,
                VOS_INT16                shwBandLow,
                VOS_INT16                shwBandLen,
                VOS_INT16               *pshwLsaGain)
{
    VOS_INT16 shwCntI;
    VOS_INT16 ashwLsaGainExtend[MED_2MIC_NUM_CHAN_16k + 2];
    VOS_INT16 ashwLsaGainTmp[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16 shwDelta;
    VOS_INT16 shwTemp1;
    VOS_INT16 shwTemp2;
    VOS_INT16 shwSubAlpha;
    VOS_INT32 swSumTmp;
    VOS_INT16 shwMeanTmp;

    /* 子带间平滑 */
    ashwLsaGainExtend[0]              = pshwLsaGain[0];
    ashwLsaGainExtend[shwBandLen + 1] = pshwLsaGain[shwBandLen - 1];
    UCOM_MemCpy(ashwLsaGainExtend + 1,
                pshwLsaGain,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    UCOM_MemCpy(ashwLsaGainTmp,
                pshwLsaGain,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    shwDelta = MED_2MIC_LSA_DELTA;

    for (shwCntI = shwBandLow; shwCntI < (shwBandLen - shwDelta); shwCntI++)
    {
        /* 计算当前5个子带的平均增益 */
        swSumTmp = 0;
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI - 2]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI - 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI + 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI + 2]));
    /*    for (shwCntJ = (shwCntI - shwDelta); shwCntJ < (shwCntI + shwDelta + 1); shwCntJ++)
        {
            swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntJ]));
        }*/

        shwMeanTmp = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(swSumTmp,
                                                      MED_2MIC_LSA_MEAN_COEF));

        /* 对当前子带增益平滑 */
        if (shwMeanTmp < MED_2MIC_LSAGAIN_SMOOTH_THD)
        {
            pshwLsaGain[shwCntI] = MED_2MIC_WeightSum(ashwLsaGainExtend,
                                                      pshwBandAlpha,
                                                      shwSmoothLen,
                                                      shwCntI);
        }
    }


    /* 帧间平滑 */
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_2MIC_LSAGAIN_TIME_ALPHA);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwTemp1 = CODEC_OpMult(MED_2MIC_LSAGAIN_TIME_ALPHA, pshwLsaGainLast[shwCntI]);
        shwTemp2 = CODEC_OpMult(shwSubAlpha, pshwLsaGain[shwCntI]);
        pshwLsaGain[shwCntI] = CODEC_OpAdd(shwTemp1, shwTemp2);
    }
}


VOS_VOID MED_2MIC_ExtendBand2FreqBinGain(
                VOS_INT32               *pswBandMc,
                VOS_INT16               *pshwBandGain,
                VOS_INT16                shwFreqBinLen,
                VOS_INT16                shwBandLen,
                VOS_INT16               *pshwFreqBinGain,
                MED_2MIC_PROBSPPRESEN_STRU *pstProbSpPresen,
                VOS_INT16                  *pshwProbSpPrese)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwCntJ;
    VOS_INT16 shwChLo;
    VOS_INT16 shwChHi;
    VOS_INT32 swNsPow                   = 0;
    VOS_INT16 shwAllPassInd             = 0;
    VOS_INT16 shwSpechProb              = 0;

    VOS_INT16 *pshwMedAnr2MicChTbl  = (VOS_INT16 *)MED_2MIC_GetAnr2MicChTblPtr();
    MED_AEC_OBJ_STRU *pstAECObj     = MED_AEC_GetObjPtr();

    for(shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
       swNsPow = CODEC_OpL_add(swNsPow,
                               CODEC_OpL_shr(pstProbSpPresen->aswNsPsd[shwCntI],
                                             MED_PP_SHIFT_BY_5));
    }

    if (swNsPow <= pstProbSpPresen->shwNsLowTh)
    {
        pstProbSpPresen->shwNsIndex = pstProbSpPresen->shwNsIndex + 1;
        if (pstProbSpPresen->shwNsIndex >= MED_2MIC_TMOS_SILENCETh)
        {
            pstProbSpPresen->shwAllPassNum = MED_2MIC_TMOS_CLOSEANR_LEN;
        }
    }
    else
    {
        pstProbSpPresen->shwNsIndex = 0;
    }

    swNsPow     = 0;
    shwChLo     = pstProbSpPresen->shwTmosExitBandLow;
    shwChHi     = pstProbSpPresen->shwTmosExitBandHi;
    for (shwCntI = shwChLo; shwCntI < shwChHi; shwCntI++)
    {
        swNsPow = CODEC_OpL_add(swNsPow,
                                CODEC_OpL_mpy_32_16(pswBandMc[shwCntI],
                                                    CODEC_OpSub(CODEC_OP_INT16_MAX, pshwProbSpPrese[shwCntI])));
        shwSpechProb = CODEC_OpAdd(shwSpechProb,
                                   CODEC_OpShr(pshwProbSpPrese[shwCntI], MED_PP_SHIFT_BY_5));
    }

    UCOM_MemSet(pshwFreqBinGain,
                0,
                (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT16));

    pstProbSpPresen->shwAllPassNum = CODEC_OpMax(0, (pstProbSpPresen->shwAllPassNum - 1));

    if (pstProbSpPresen->shwAllPassNum > 0)
    {
        shwAllPassInd = 1;
    }
    if ((CODEC_OpL_shr(swNsPow, MED_PP_SHIFT_BY_5) > pstProbSpPresen->shwNsHighTh)
        && (shwSpechProb < pstProbSpPresen->shwSpLowTh))
    {
        shwAllPassInd = 0;
    }

    if ((0 == shwAllPassInd) || (pstAECObj->stDtd.enDtdFlag == MED_AEC_DTD_FLAG_ST))
    {
        /* 计算每个子带的增益 */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            /* 子带范围 */
            shwChLo = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 0);
            shwChHi = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 1);

            for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
            {
                pshwFreqBinGain[shwCntJ] = pshwBandGain[shwCntI];
            }
        }
    }
    else
    {
        /* 计算每个子带的增益 */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            /* 子带范围 */
            shwChLo = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 0);
            shwChHi = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 1);

            for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
            {
                pshwFreqBinGain[shwCntJ] = CODEC_OP_INT16_MAX;
            }
        }
    }
}


VOS_VOID MED_2MIC_FilterFrm(
                VOS_INT16                shwFreqBinLen,
                VOS_INT16               *pshwFreqBinGain,
                MED_2MIC_FREQ_STRU      *pstMc)
{
    VOS_INT16 shwCntI;
    VOS_INT16 *pshwFreqMc = MED_NULL;

    /* 主麦克频谱 */
    pshwFreqMc = pstMc->ashwFreq;

    /* 施加增益 */
    pshwFreqMc[0] = CODEC_OpMult(pshwFreqMc[0], pshwFreqBinGain[0]);
    pshwFreqMc[1] = CODEC_OpMult(pshwFreqMc[1], pshwFreqBinGain[shwFreqBinLen - 1]);

    for (shwCntI = 1; shwCntI < (shwFreqBinLen - 1); shwCntI++)
    {
        pshwFreqMc[2 * shwCntI]       = CODEC_OpMult(pshwFreqMc[2 * shwCntI],
                                                pshwFreqBinGain[shwCntI]);
        pshwFreqMc[(2 * shwCntI) + 1] = CODEC_OpMult(pshwFreqMc[(2 * shwCntI) + 1],
                                                    pshwFreqBinGain[shwCntI]);
    }

}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

