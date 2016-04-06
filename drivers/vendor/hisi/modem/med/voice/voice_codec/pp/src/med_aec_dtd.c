
/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "med_aec_dtd.h"
#include "med_aec_nlp.h"
#include "med_anr_2mic_fd.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif
/*lint -e(813)*/
/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
/* SER计算语音存在概率P时子带阈值加权值 16bit Q12 */
VOS_INT16 g_ashwDtdBandSERThdModulTab[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB] =
{
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047
};

/* 语音存在概率P带间平滑系数 */
VOS_INT16 g_ashwSpBandProbAlphaCoeff[MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN] =
{
    3640, 7280, 10920, 7280, 3640
};

/*****************************************************************************
   3 函数实现
*****************************************************************************/


VOS_VOID MED_AEC_DtdInit(
                MED_AEC_NV_STRU                *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_DTD_STRU               *pstDtd)
{
    VOS_INT16        shwCnt;
    VOS_INT16        shwStFlrThd = MED_AEC_DTD_ST_FLR_THD;

    UCOM_MemSet((VOS_INT16*)pstDtd, 0, sizeof(MED_AEC_DTD_STRU));

    pstDtd->shwFrameLen              = MED_PP_GetFrameLength();
    pstDtd->shwMutePowerThd          = pstAecParam->shwDtdMutePowerThd;

    /* 采样率对应参数初始化 包含线性和非线性DTD */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstDtd->stDtdLine.enFftLen             = MED_AEC_MAX_FFT_LEN / 2;
        pstDtd->stDtdLine.shwMeanAlpha         = MED_AEC_DTD_MEAN_ALPHA_NB;
        pstDtd->stDtdNline.enFftLen            = MED_AEC_MAX_FFT_LEN / 2;
        pstDtd->stDtdNline.shwFreqBinLen       = MED_AEC_DTD_NLINE_FREQ_BIN_LEN_NB;
        pstDtd->stDtdNline.shwBandLen          = MED_AEC_DTD_NLINE_PSD_BAND_LEN_NB;
    }
    else
    {
        pstDtd->stDtdLine.enFftLen             = MED_AEC_MAX_FFT_LEN;
        pstDtd->stDtdLine.shwMeanAlpha         = MED_AEC_DTD_MEAN_ALPHA_WB;
        pstDtd->stDtdNline.enFftLen            = MED_AEC_MAX_FFT_LEN;
        pstDtd->stDtdNline.shwFreqBinLen       = MED_AEC_DTD_NLINE_FREQ_BIN_LEN_WB;
        pstDtd->stDtdNline.shwBandLen          = MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB;
    }

    /* 线性DTD结构体初始化 */
    pstDtd->stDtdLine.shwFrameLen     = MED_PP_GetFrameLength();
    pstDtd->stDtdLine.shwStModiThd    = pstAecParam->shwDtdStModiThd;
    pstDtd->stDtdLine.shwDtModiThd    = pstAecParam->shwDtdDtModiThd;
    pstDtd->stDtdLine.shwStFlrThd     = shwStFlrThd;

    /* 非线性DTD结构体初始化 */
    pstDtd->stDtdNline.shwSpProbMean        = MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE;
    pstDtd->stDtdNline.enNearFarRatioEnable = pstAecParam->enNearFarRatioEnable;
    pstDtd->stDtdNline.shwNearFarRatioActiveFrmNum = pstAecParam->shwNearFarRatioActiveFrmNum;
    pstDtd->stDtdNline.shwNearFarRatioGain  = pstAecParam->shwNearFarRatioGain;
    pstDtd->stDtdNline.shwSpSerThd          = pstAecParam->shwSpSerThd;
    pstDtd->stDtdNline.shwEchoSerThd        = pstAecParam->shwEchoSerThd;
    pstDtd->stDtdNline.shwBandPsdMuteThd    = pstAecParam->shwBandPsdMuteThd;
    pstDtd->stDtdNline.shwSpThdInit         = pstAecParam->shwSpThdInit;
    pstDtd->stDtdNline.shwSpThd             = pstAecParam->shwSpThdInit;
    pstDtd->stDtdNline.shwSpThdMax          = pstAecParam->shwSpThdMax;
    pstDtd->stDtdNline.shwSpThdMin          = pstAecParam->shwSpThdMin;
    pstDtd->stDtdNline.swSumPsdThd          = CODEC_OpL_mult0(pstAecParam->shwSumPsdThd,
                                                            MED_AEC_DTD_NLINE_SUM_PSD_THD_BASE); /* 32bit Q8 */
    for (shwCnt = 0; shwCnt < pstDtd->stDtdNline.shwBandLen; shwCnt++)
    {
        pstDtd->stDtdNline.ashwSpBandProb[shwCnt] = MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE;
    }

    /* 单讲增强相关元素初始化 牺牲双讲时使用 */
    pstDtd->enEstFlag                = pstAecParam->enEstFlag;
    pstDtd->shwEstPowerThd           = pstAecParam->shwEstPowerThd;
    pstDtd->shwEstAmplThd            = pstAecParam->shwEstAmplThd;
    pstDtd->shwEstHangOver           = MED_AEC_DTD_EST_HANG_OVER_LEN;

    /* 单双讲判别结果初始化为bypass */
    pstDtd->enDtdFlag                = MED_AEC_DTD_FLAG_PASS;

}


VOS_VOID MED_AEC_DtdMuteDetect(
                MED_AEC_DTD_STRU              *pstDtd,
                VOS_INT16                     *pshwFar)
{
    VOS_INT16                     shwMeanFar  = 0;
    VOS_INT16                     shwMaxFar   = 0;
    VOS_INT32                     swPowerThd  = 0;
    VOS_INT32                     swCurrPower = 0;
    VOS_INT32                     swAddPara   = 0;
    VOS_INT16                     shwFrameLen = 0;
    VOS_INT16                     ashwFarTmp[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT32                     swFarSum = 0;
    VOS_INT32                     swCnt    = 0;

    MED_AEC_DTD_FLAG_ENUM_INT16   enDtdFlag;

    shwFrameLen = pstDtd->shwFrameLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdMuteDetect);

    /* 将单双讲标志初始化为单讲 */
    enDtdFlag             = MED_AEC_DTD_FLAG_ST;

    /* 计算平滑的信号能量 */
    CODEC_OpVecAbs(pshwFar, shwFrameLen, ashwFarTmp);

    shwMaxFar = CODEC_OpVecMax(ashwFarTmp, shwFrameLen, 0);

    for (swCnt = 0; swCnt < shwFrameLen; swCnt++)
    {
        swFarSum = CODEC_OpL_add(swFarSum, ashwFarTmp[swCnt]);
    }

    shwMeanFar  = (VOS_INT16)(swFarSum / shwFrameLen);

    swCurrPower = CODEC_OpL_mult0(shwMeanFar, shwMeanFar);

    swCurrPower = CODEC_OpL_mpy_32_16(swCurrPower, MED_AEC_DTD_MUTE_ALPHA);
    swAddPara   = CODEC_OpL_mpy_32_16(pstDtd->swPowerLast, ((CODEC_OP_INT16_MAX - MED_AEC_DTD_MUTE_ALPHA) + 1));
    swCurrPower = CODEC_OpL_add(swCurrPower, swAddPara);

    pstDtd->swPowerLast = swCurrPower;

    /* 根据能量判断是否有回声 */
    if( (swCurrPower < (VOS_INT32)(pstDtd->shwMutePowerThd)) )
    {
        pstDtd->shwMuteHandover = CODEC_OpSub(pstDtd->shwMuteHandover, 1);

        if(pstDtd->shwMuteHandover <= 0)
        {
            enDtdFlag               = MED_AEC_DTD_FLAG_PASS;
            pstDtd->shwMuteHandover = 0;
        }
    }
    else
    {
        enDtdFlag               = MED_AEC_DTD_FLAG_ST;
        pstDtd->shwMuteHandover = CODEC_OpAdd(pstDtd->shwMuteHandover, 1);

        if(pstDtd->shwMuteHandover> MED_AEC_DTD_MUTE_HO_THD)
        {
            pstDtd->shwMuteHandover = MED_AEC_DTD_MUTE_HO_THD;
        }
    }

    /* 增强单讲使能时计算是否强制判断为ST标志 */
    if ( CODEC_SWITCH_ON == pstDtd->enEstFlag)
    {
        swPowerThd = CODEC_OpL_mult0(pstDtd->shwEstPowerThd, MED_AEC_DTD_EST_POWER_BASE);

        if ((swCurrPower > swPowerThd)
         || (shwMaxFar   > pstDtd->shwEstAmplThd))
        {
            pstDtd->shwFarLargeFlag = 1;
            pstDtd->shwEstHangOver  = 0;
        }
        else
        {
            pstDtd->shwEstHangOver = CODEC_OpAdd(pstDtd->shwEstHangOver, 1);

            if (pstDtd->shwEstHangOver > MED_AEC_DTD_EST_HANG_OVER_LEN)
            {
                pstDtd->shwFarLargeFlag = 0;
            }
            else
            {
                pstDtd->shwFarLargeFlag = 1;
            }
        }
    }

    pstDtd->enDtdFlag = enDtdFlag;

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdMuteDetect);

}
#ifndef _MED_C89_

static inline VOS_VOID MED_AEC_DtdUnit(
                VOS_INT32    shwFftLen,
                VOS_INT16   *pshwErrFreq,
                VOS_INT16   *pshwEchoFreq,
                VOS_INT16   *pshwNearFreq,
                VOS_INT32   *pswPdy,
                VOS_INT32   *pswPdd,
                VOS_INT32   *pswPdySum,
                VOS_INT32   *pswPddSum)
{
    VOS_INT32   swCntI;
    VOS_INT16   shwSpecAlpha  = MED_AEC_DTD_SPEC_ALPHA;
    VOS_INT16   shwSmoothCoef = MED_AEC_NLP_BGN_SMOOTH_COFE1;

    ae_p24x2s   aepEr, aepEc, aepNe, aepSa, aepSc;
    ae_q56s     aeqSum1, aeqSum2, aeqTmp1, aeqTmp2;

    /* 4字节对齐保护 */
    if (((((VOS_INT32)&pshwErrFreq[0]) & 0x03) != 0)
      || ((((VOS_INT32)&pshwEchoFreq[0]) & 0x03) != 0)
      || ((((VOS_INT32)&pshwNearFreq[0]) & 0x03) != 0))
    {
        return;
    }

    aepSa = AE_CVTP24A16(shwSpecAlpha);
    aepSc = AE_CVTP24A16(shwSmoothCoef);

    aeqSum1 = *((ae_q32s *)pswPdySum);
    aeqSum2 = *((ae_q32s *)pswPddSum);

    for (swCntI = 0; swCntI < shwFftLen>>1; swCntI++)
    {
        aepEr = *((ae_p16x2s *)&pshwErrFreq[2*swCntI]);
        aepEc = *((ae_p16x2s *)&pshwEchoFreq[2*swCntI]);

        aepEr = AE_ABSSP24S(aepEr);
        aepEc = AE_ABSSP24S(aepEc);
        aepNe = AE_ADDSP24S(aepEr, aepEc);

        *((ae_p16x2s *)&pshwNearFreq[2*swCntI]) = aepNe;

        aeqTmp1 = *((ae_q32s *)&pswPdy[2*swCntI]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_HH(aepEc, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum1 = AE_ADDSQ56S(aeqTmp2, aeqSum1);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdy[2*swCntI] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdy[2*swCntI+1]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_LL(aepEc, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum1 = AE_ADDSQ56S(aeqTmp2, aeqSum1);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdy[2*swCntI+1] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdd[2*swCntI]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_HH(aepNe, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum2 = AE_ADDSQ56S(aeqTmp2, aeqSum2);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdd[2*swCntI] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdd[2*swCntI+1]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_LL(aepNe, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum2 = AE_ADDSQ56S(aeqTmp2, aeqSum2);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdd[2*swCntI+1] = AE_TRUNCA32Q48(aeqTmp2);

    }

    (*pswPdySum) = AE_TRUNCA32Q48(aeqSum1);
    (*pswPddSum) = AE_TRUNCA32Q48(aeqSum2);

}
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swValue;
    VOS_INT16                           shwTmp;
    VOS_INT16                           shwCorr;
    VOS_INT32                           swPdySum = 0;
    VOS_INT32                           swPddSum = 0;
    VOS_INT32                           swSumPara1;
    VOS_INT32                           swSumPara2;
    VOS_INT16                           shwTempMultPara;

    VOS_INT16                          *pshwNearFreq;
    VOS_INT16                          *pshwTmp;
    VOS_INT32                           swTmpSum = 0;
    VOS_INT32                           swAvgAbsErr;
    VOS_INT16                           shwBeta;
    VOS_INT32                           swAvgAbsNear;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;

    shwTmp       = MED_AEC_NLP_BGN_SMOOTH_COFE1;
    pshwNearFreq = MED_AEC_GetshwVecTmp640Ptr1();

    /* pass态 */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagPre)
    {
        return enDtdFlagPre;
    }

    MED_AEC_DtdUnit((VOS_INT32)pstDtd->enFftLen,
                    pshwErrFreq,
                    pshwEchoFreq,
                    pshwNearFreq,
                    pstDtd->aswPdy,
                    pstDtd->aswPdd,
                    &swPdySum,
                    &swPddSum);

    /* 计算归一化互相关 */
    swPdySum = CODEC_OpL_max(swPdySum, 0);
    swPddSum = CODEC_OpL_max(swPddSum, 1);
    shwCorr  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swPdySum, swPddSum, MED_PP_SHIFT_BY_30));

    /* 由于小信号消除回声效果会变弱，因此判为单讲的阈值适当降低。*/
    pshwTmp = pshwNearFreq;
    CODEC_OpVecAbs(pshwError, pstDtd->shwFrameLen, pshwTmp);

    for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
    {
        swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
    }

    swAvgAbsErr = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);

    if (swAvgAbsErr < pstDtd->shwStModiThd)
    {
        shwBeta = CODEC_OpSaturate(CODEC_OpNormDiv_32(swAvgAbsErr,
                                                  pstDtd->shwStModiThd,
                                                  MED_PP_SHIFT_BY_15));
        shwBeta = CODEC_OpMax(shwBeta, MED_AEC_DTD_BETA_MIN);
    }
    else
    {
        shwBeta = MED_AEC_DTD_BETA_MAX;
    }

    /* 单双讲判断 */
    shwTmp =    (shwCorr >= CODEC_OpMult(MED_AEC_DTD_CORR_THD, shwBeta))
             || (swAvgAbsErr < pstDtd->shwStFlrThd);
    if (1== shwTmp)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
    }

    /* 当单讲时部分情况修正为双讲，条件为：残留回声幅度大于单讲的阈值且AF后残差信号幅度大于MIC信号的一半 */
    if (MED_AEC_DTD_FLAG_ST == enDtdFlag)
    {
        CODEC_OpVecAbs(pshwNear, pstDtd->shwFrameLen, pshwTmp);
        swTmpSum = 0;

        for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
        {
            swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
        }

        swAvgAbsNear = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);
        swValue = CODEC_OpMult(pstDtd->shwDtModiThd, (VOS_INT16)swAvgAbsNear);

        if (   (swAvgAbsErr > pstDtd->shwStModiThd)
            && (swAvgAbsErr > swValue))
        {
            enDtdFlag = MED_AEC_DTD_FLAG_DT;
        }
    }

    return enDtdFlag;

}

#else
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swValue;
    VOS_INT16                           shwTmp;
    VOS_INT16                           shwCorr;
    VOS_INT32                           swPdySum = 0;
    VOS_INT32                           swPddSum = 0;
    VOS_INT32                           swSumPara1;
    VOS_INT32                           swSumPara2;
    VOS_INT16                           shwTempMultPara;

    VOS_INT16                          *pshwNearFreq;
    VOS_INT16                          *pshwTmp;
    VOS_INT32                           swTmpSum = 0;
    VOS_INT32                           swAvgAbsErr;
    VOS_INT16                           shwBeta;
    VOS_INT32                           swAvgAbsNear;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdLine);

    shwTmp       = MED_AEC_NLP_BGN_SMOOTH_COFE1;
    pshwNearFreq = MED_AEC_GetshwVecTmp640Ptr1();

    /* pass态 */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagPre)
    {
        return enDtdFlagPre;
    }

    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        pshwNearFreq[swCntI] = CODEC_OpAdd(CODEC_OpAbs_s(pshwErrFreq[swCntI]),
                                          CODEC_OpAbs_s(pshwEchoFreq[swCntI]));
    }

    /* 近端和残差信号的互功率谱，帧间平滑 */
    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pstDtd->aswPdy[swCntI], MED_AEC_DTD_SPEC_ALPHA);
        shwTempMultPara  = CODEC_OpMult_r(shwTmp, CODEC_OpAbs_s(pshwEchoFreq[swCntI]));
        swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[swCntI], shwTempMultPara);
        pstDtd->aswPdy[swCntI] = CODEC_OpL_add(swSumPara1, swSumPara2);
        swPdySum         = CODEC_OpL_add(pstDtd->aswPdy[swCntI], swPdySum);
    }

    /* 近端自功率谱，帧间平滑 */
    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pstDtd->aswPdd[swCntI], MED_AEC_DTD_SPEC_ALPHA);
        shwTempMultPara  = CODEC_OpMult_r(shwTmp, pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_abs(CODEC_OpL_mult0(pshwNearFreq[swCntI], shwTempMultPara));
        pstDtd->aswPdd[swCntI] = CODEC_OpL_add(swSumPara1, swSumPara2);
        swPddSum = CODEC_OpL_add(pstDtd->aswPdd[swCntI], swPddSum);
    }

    /* 计算归一化互相关 */
    swPdySum = CODEC_OpL_max(swPdySum, 0);
    swPddSum = CODEC_OpL_max(swPddSum, 1);
    shwCorr  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swPdySum, swPddSum, MED_PP_SHIFT_BY_30));

    /* 由于小信号消除回声效果会变弱，因此判为单讲的阈值适当降低。*/
    pshwTmp = pshwNearFreq;
    CODEC_OpVecAbs(pshwError, pstDtd->shwFrameLen, pshwTmp);

    for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
    {
        swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
    }

    swAvgAbsErr = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);

    if (swAvgAbsErr < pstDtd->shwStModiThd)
    {
        shwBeta = CODEC_OpSaturate(CODEC_OpNormDiv_32(swAvgAbsErr,
                                                  pstDtd->shwStModiThd,
                                                  MED_PP_SHIFT_BY_15));
        shwBeta = CODEC_OpMax(shwBeta, MED_AEC_DTD_BETA_MIN);
    }
    else
    {
        shwBeta = MED_AEC_DTD_BETA_MAX;
    }

    /* 单双讲判断 */
    shwTmp =    (shwCorr >= CODEC_OpMult(MED_AEC_DTD_CORR_THD, shwBeta))
             || (swAvgAbsErr < pstDtd->shwStFlrThd);
    if (1== shwTmp)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
    }

    /* 当单讲时部分情况修正为双讲，条件为：残留回声幅度大于单讲的阈值且AF后残差信号幅度大于MIC信号的一半 */
    if (MED_AEC_DTD_FLAG_ST == enDtdFlag)
    {
        CODEC_OpVecAbs(pshwNear, pstDtd->shwFrameLen, pshwTmp);
        swTmpSum = 0;

        for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
        {
            swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
        }

        swAvgAbsNear = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);
        swValue = CODEC_OpMult(pstDtd->shwDtModiThd, (VOS_INT16)swAvgAbsNear);

        if (   (swAvgAbsErr > pstDtd->shwStModiThd)
            && (swAvgAbsErr > swValue))
        {
            enDtdFlag = MED_AEC_DTD_FLAG_DT;
        }
    }

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdLine);

    return enDtdFlag;

}
#endif
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdNline(
                MED_AEC_DTD_NLINE_STRU        *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagLine,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq)
{
    VOS_INT16          shwCnt;
    VOS_INT16          shwFreqBinLen;
    VOS_INT16          shwBandLen;
    VOS_INT16          shwAlpha;
    VOS_INT16          shwSubAlpha;

    VOS_INT16         *pshwNearFreq;
    VOS_INT16         *pshwSpBandProb;

    VOS_INT32          swSpProbSum;
    VOS_INT16          shwSpProbMean;
    VOS_INT16          shwSpProbMeanLast;

    VOS_INT32         *pswPsdBandNearLast;
    VOS_INT32         *pswPsdBandFarLast;
    VOS_INT32         *pswPsdBandEchoLast;
    VOS_INT32         *pswPsdBinNear;
    VOS_INT32         *pswPsdBinFar;
    VOS_INT32         *pswPsdBinEcho;
    VOS_INT32         *pswPsdBandNear;
    VOS_INT32         *pswPsdBandFar;
    VOS_INT32         *pswPsdBandEcho;

    VOS_INT16          shwSpThdMax;
    VOS_INT16          shwSpThdMin;
    VOS_INT16          shwSpThd;

    VOS_INT32          swSumPsd;
    VOS_INT32          swSumPsdLast;
    VOS_INT32          swSumPsdThd;

    MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag;

    shwFreqBinLen = pstDtd->shwFreqBinLen;
    shwBandLen    = pstDtd->shwBandLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdNline);

    /* 静音帧处理 相应判断参数初始化 */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagLine)
    {
        for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
        {
            pstDtd->ashwSpBandProb[shwCnt]  = 0;
            pstDtd->aswPsdBandNear[shwCnt]  = 0;
            pstDtd->aswPsdBandFar[shwCnt]   = 0;
            pstDtd->aswPsdBandEcho[shwCnt]  = 0;
        }

        pstDtd->shwSpProbMean = 0;
        pstDtd->swSumPsd = 0;

        pstDtd->shwSpThd = pstDtd->shwSpThdInit;

        return enDtdFlagLine;
    }

    /* 动态内存申请 */
    pshwNearFreq   = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)pstDtd->enFftLen * sizeof(VOS_INT16));

    pswPsdBinNear  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinFar   = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinEcho  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBandNear = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    pswPsdBandFar  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    pswPsdBandEcho = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    pshwSpBandProb = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    /* 计算近端信号频谱 */
    CODEC_OpVvAdd(pshwErrFreq,
                pshwEchoFreq,
                pstDtd->enFftLen,
                pshwNearFreq);

    /* 计算近端信号和远端信号子带功率谱 */
    /* 1. 计算近端信号和远端信号功率谱 32bit Q6 */
    MED_PP_CalcPsdBin(pshwNearFreq, shwFreqBinLen, pswPsdBinNear);
    MED_PP_CalcPsdBin(pshwFarFreq, shwFreqBinLen, pswPsdBinFar);
    MED_PP_CalcPsdBin(pshwEchoFreq, shwFreqBinLen, pswPsdBinEcho);

    /* 2. 初步计算近端信号和远端信号子带功率谱 32bit Q6 */
    MED_PP_CalcBandPsd(pswPsdBinNear, shwBandLen, pswPsdBandNear);
    MED_PP_CalcBandPsd(pswPsdBinFar, shwBandLen, pswPsdBandFar);
    MED_PP_CalcBandPsd(pswPsdBinEcho, shwBandLen, pswPsdBandEcho);

    /* 3. 平滑计算的子带功率谱  32bit Q6 */
    pswPsdBandNearLast = pstDtd->aswPsdBandNear;
    pswPsdBandFarLast  = pstDtd->aswPsdBandFar;
    pswPsdBandEchoLast = pstDtd->aswPsdBandEcho;

    MED_PP_CalcSmoothPsd(pswPsdBandNearLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandNear);

    MED_PP_CalcSmoothPsd(pswPsdBandFarLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandFar);

    MED_PP_CalcSmoothPsd(pswPsdBandEchoLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandEcho);

    /* 4. 更新结构体子带功率谱，用于下帧计算  32bit Q6 */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        pstDtd->aswPsdBandNear[shwCnt] = pswPsdBandNear[shwCnt];
        pstDtd->aswPsdBandFar[shwCnt]  = pswPsdBandFar[shwCnt];
        pstDtd->aswPsdBandEcho[shwCnt] = pswPsdBandEcho[shwCnt];
    }

    /* 计算各子带语音存在概率  16bit Q15 */
    MED_AEC_DtdSpProb(pstDtd, pswPsdBandNear, pswPsdBandFar, pswPsdBandEcho, pshwSpBandProb);

    /* 计算子带语音存在概率平均值  16bit Q15 */
    swSpProbSum = 0;

    for (shwCnt = 0; shwCnt < MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN; shwCnt++)
    {
        swSpProbSum = CODEC_OpL_add(swSpProbSum, pshwSpBandProb[shwCnt]);
    }

    shwSpProbMean = CODEC_OpRound(CODEC_OpNormDiv_32(swSpProbSum,
                                                 MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN,
                                                 16));  /* Q15=Q15-Q0+Q16-Q16 */

    /* 时域平滑子带语音存在概率平均值 16bit Q15 */
    shwSpProbMeanLast = pstDtd->shwSpProbMean;

    shwAlpha    = MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA);

    shwSpProbMean = CODEC_OpAdd(CODEC_OpMult(shwSpProbMeanLast, shwSubAlpha),
                              CODEC_OpMult(shwSpProbMean, shwAlpha));

    pstDtd->shwSpProbMean = shwSpProbMean;


    /* 计算单双讲标志(根据子带语音存在概率平均值)*/
    /* 1. 计算子带语音存在概率平均值判断阈值 16bit Q15 */
    shwSpThdMax  = pstDtd->shwSpThdMax;
    shwSpThdMin  = pstDtd->shwSpThdMin;
    shwSpThd     = pstDtd->shwSpThd;

    /* 2. 根据子带语音存在概率平均值计算单双讲标志 */
    if (shwSpProbMean < shwSpThd)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
        shwSpThd  = CODEC_OpAdd(shwSpThd, MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL); /* 当判为单讲时，调大阈值，使其利于单讲判别 */
        shwSpThd  = CODEC_OpMin(shwSpThd, shwSpThdMax);
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
        shwSpThd  = CODEC_OpSub(shwSpThd, MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL); /* 当判为双讲时，调小阈值，使其利于双讲判别 */
        if (shwSpProbMean > shwSpThdMax)
        {
            shwSpThd = shwSpThd - MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_LARGE;        /* 当判为双讲时，特征值较大即有明显双讲特性时，大步调小阈值，保证连片双讲 */
        }
        shwSpThd = CODEC_OpMax(shwSpThd, shwSpThdMin);
    }

    /* 3. 更新子带语音存在概率平均值判断阈值 */
    pstDtd->shwSpThd = shwSpThd;

    /* 4. 修正单双讲标志(根据子带施加子带存在概率后剩余能量)*/
    /* 4.1 计算剩余能量 32bit Q6 */
    swSumPsd = 0;
    for (shwCnt = 0; shwCnt < MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN; shwCnt++)
    {
        swSumPsd = CODEC_OpL_add(swSumPsd,
                               CODEC_OpL_mpy_32_16(pswPsdBandNear[shwCnt], pshwSpBandProb[shwCnt])); /* Q6=Q6+Q15-Q15 */
    }

    /* 4.2 时域平滑剩余能量 32bit Q6 */
    swSumPsdLast = pstDtd->swSumPsd;

    shwAlpha    = MED_AEC_DTD_NLINE_SUM_PSD_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SUM_PSD_ALPHA);

    swSumPsd = CODEC_OpL_add(CODEC_OpL_mpy_32_16(swSumPsdLast, shwSubAlpha),
                           CODEC_OpL_mpy_32_16(swSumPsd, shwAlpha));

    pstDtd->swSumPsd = swSumPsd;


    /* 4.3 根据剩余能量阈值修正 */
    swSumPsdThd  = pstDtd->swSumPsdThd;

    if (swSumPsd < swSumPsdThd)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }

    /* 释放内存 */
    UCOM_MemFree(pshwNearFreq);
    UCOM_MemFree(pswPsdBinNear);
    UCOM_MemFree(pswPsdBinFar);
    UCOM_MemFree(pswPsdBinEcho);
    UCOM_MemFree(pswPsdBandNear);
    UCOM_MemFree(pswPsdBandFar);
    UCOM_MemFree(pswPsdBandEcho);
    UCOM_MemFree(pshwSpBandProb);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdNline);

    return enDtdFlag;

}
VOS_VOID MED_AEC_DtdSpProb(
                MED_AEC_DTD_NLINE_STRU            *pstDtd,
                VOS_INT32                         *pswPsdBandNear,
                VOS_INT32                         *pswPsdBandFar,
                VOS_INT32                         *pswPsdBandEcho,
                VOS_INT16                         *pshwSpBandProb)
{
    VOS_INT16          shwCnt;
    VOS_INT16          shwBandLen;
    VOS_INT16          shwBandLenExt;
    VOS_INT16          shwAlpha;
    VOS_INT16          shwSubAlpha;

    VOS_INT16          shwNearFarRatioActiveFrmCnt;
    VOS_INT16          shwNearFarRatioActiveFrmNum;
    VOS_INT16          shwSpSERThd;
    VOS_INT16          shwEchoSERThd;
    VOS_INT16         *pshwSpBandProbLast;
    VOS_INT16         *pshwSpBandProbTmp;
    VOS_INT32          swBandPsdMuteThd;

    VOS_INT16         *pshwSERModulTab    = MED_AEC_DtdNlineGetSerModulTabPtr();
    VOS_INT16         *pshwProbAlphaCoeff = MED_AEC_DtdNlineGetProbAlphaCoeffPtr();

    VOS_INT16          *pshwPostSER;
    VOS_INT16          *pshwPostNFR;

    /* 获取结构体参数配置 */
    shwBandLen        = pstDtd->shwBandLen;
    shwBandLenExt     = pstDtd->shwBandLen + MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN;

    swBandPsdMuteThd  = CODEC_OpL_shl((VOS_INT32)pstDtd->shwBandPsdMuteThd, MED_PP_SHIFT_BY_6);  /* [16bit Q6] */

    shwNearFarRatioActiveFrmNum = pstDtd->shwNearFarRatioActiveFrmNum;
    shwNearFarRatioActiveFrmCnt = pstDtd->shwNearFarRatioActiveFrmCnt;
    /* 动态分配内存 */
    pshwPostSER       = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    pshwPostNFR       = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    pshwSpBandProbTmp = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLenExt * sizeof(VOS_INT16));

    /* 1. 计算后验信回比SER */

    /* 根据近端/估计回声计算SER */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        pshwPostSER[shwCnt] = CODEC_OpRound(CODEC_OpNormDiv_32(pswPsdBandNear[shwCnt],
                                                           pswPsdBandEcho[shwCnt],
                                                           MED_PP_SHIFT_BY_27));         /* [16bit Q11=Q6-Q6+Q27-Q16] */
    }

    shwNearFarRatioActiveFrmCnt         = CODEC_OpAdd(shwNearFarRatioActiveFrmCnt, 1);
    pstDtd->shwNearFarRatioActiveFrmCnt = shwNearFarRatioActiveFrmCnt;

    /* 若使能远端信号联合计算SER，且初始单双讲一定帧数时，更新SER ashwPostNFR */
    if ((CODEC_SWITCH_ON == pstDtd->enNearFarRatioEnable)
     && (shwNearFarRatioActiveFrmCnt < shwNearFarRatioActiveFrmNum))
    {
        for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
        {
            /* 根据NV参数由远端信号估计回声大小*/
            pswPsdBandFar[shwCnt] = CODEC_OpL_shl(CODEC_OpL_mpy_32_16(pswPsdBandFar[shwCnt],
                                                                  pstDtd->shwNearFarRatioGain),
                                                MED_PP_SHIFT_BY_4);
            /* 防止NV参数配0导致除0 */
            pswPsdBandFar[shwCnt] = CODEC_OpL_max(pswPsdBandFar[shwCnt],
                                                MED_PP_MIN_PSD_VALUE);
            /* 计算近端/远端信号比值NFR */
            pshwPostNFR[shwCnt] = CODEC_OpRound(CODEC_OpNormDiv_32(pswPsdBandNear[shwCnt],
                                                               pswPsdBandFar[shwCnt],
                                                               MED_PP_SHIFT_BY_27));     /* [16bit Q11=Q6-Q6+Q27-Q16] */

            pshwPostSER[shwCnt] = CODEC_OpMin(pshwPostSER[shwCnt], pshwPostNFR[shwCnt]);
        }
    }

    /* 2. 根据各子带后验信回比SER计算子带语音存在概率 */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* 计算各子带后验信回比阈值 [16bit Q11=Q11+Q11-Q15+4]*/
        shwSpSERThd   = CODEC_OpShl(CODEC_OpMult(pshwSERModulTab[shwCnt],
                                             pstDtd->shwSpSerThd),
                                  MED_PP_SHIFT_BY_4);

        shwEchoSERThd = CODEC_OpShl(CODEC_OpMult(pshwSERModulTab[shwCnt],
                                             pstDtd->shwEchoSerThd),
                                  MED_PP_SHIFT_BY_4);

        /* 低能量时，直接语音存在概率P置0 */
        if (pswPsdBandNear[shwCnt] < swBandPsdMuteThd)
        {
            pshwSpBandProb[shwCnt] = 0;
        }
        else
        {
            if (pshwPostSER[shwCnt] > shwSpSERThd)
            {
                /* SER大时，语音存在概率P置1 */
                pshwSpBandProb[shwCnt] = CODEC_OP_INT16_MAX;   /* 16bit Q15 */
            }
            else if (pshwPostSER[shwCnt] < shwEchoSERThd)
            {
                /* SER小时，语音存在概率P置0 */
                pshwSpBandProb[shwCnt] = 0;
            }
            else
            {
                /* SER其他值，语音存在概率P置0-1，线性映射 */
                pshwSpBandProb[shwCnt] = CODEC_OpDiv_s(pshwPostSER[shwCnt]-shwEchoSERThd,  /* [16bit Q15=Q11-Q11+Q15] */
                                                     shwSpSERThd-shwEchoSERThd);
            }
        }
    }

    /* 3. 减小语音小概率子带，保留语音大概率子带 */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* 利用power(x,alpha)，x在0-1间变化，alpha选大于1整数，函数曲线） 这里alpha选2 */
        pshwSpBandProb[shwCnt] = CODEC_OpMult(pshwSpBandProb[shwCnt],
                                            pshwSpBandProb[shwCnt]); /* 16bit Q15=Q15+Q15-Q15 */
    }

    /* 4. 带间平滑子带概率 */
    UCOM_MemCpy(pshwSpBandProbTmp,
                pshwSpBandProb,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCnt = shwBandLen; shwCnt < shwBandLenExt; shwCnt++)
    {
        pshwSpBandProbTmp[shwCnt] = pshwSpBandProb[shwBandLen-1];
    }

    for (shwCnt = MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_INDEX; shwCnt < shwBandLen; shwCnt++)
    {
        /* 16bit Q15=Q15+Q15-Q15 */
        pshwSpBandProb[shwCnt] = MED_2MIC_WeightSum(pshwSpBandProbTmp,
                                                    pshwProbAlphaCoeff,
                                                    MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN,
                                                    shwCnt-2);
    }

    /* 5. 时域平滑子带概率 */
    pshwSpBandProbLast = pstDtd->ashwSpBandProb;
    shwAlpha    = MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA);
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* 时域平滑 */
        pshwSpBandProb[shwCnt] =  CODEC_OpAdd(CODEC_OpMult(pshwSpBandProbLast[shwCnt], shwSubAlpha),
                                            CODEC_OpMult(pshwSpBandProb[shwCnt], shwAlpha));

        pstDtd->ashwSpBandProb[shwCnt] =  pshwSpBandProb[shwCnt];
    }

    /* 动态内存释放 */
    UCOM_MemFree(pshwPostSER);
    UCOM_MemFree(pshwPostNFR);
    UCOM_MemFree(pshwSpBandProbTmp);

}
VOS_VOID MED_AEC_Dtd(
                MED_AEC_DTD_STRU              *pstDtd,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT16                           shwDtConLen;
    VOS_INT16                           shwDtHangLen;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagPre;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagLine;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagNline;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagFix;

    MED_AEC_DTD_LINE_STRU               *pstDtdLine;
    MED_AEC_DTD_NLINE_STRU              *pstDtdNline;

    enDtdFlagPre = pstDtd->enDtdFlag;
    pstDtdLine   = &(pstDtd->stDtdLine);
    pstDtdNline  = &(pstDtd->stDtdNline);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_Dtd);

    /* 1. 线性DTD判断 */
    enDtdFlagLine = MED_AEC_DtdLine(pstDtdLine,
                                    enDtdFlagPre,
                                    pshwErrFreq,
                                    pshwEchoFreq,
                                    pshwError,
                                    pshwNear);


    /* 2. 非线性DTD判断 */
    enDtdFlagNline = MED_AEC_DtdNline(pstDtdNline,
                                      enDtdFlagLine,
                                      pshwErrFreq,
                                      pshwEchoFreq,
                                      pshwFarFreq);

    /* 3. 仅对线性DTD的双讲进行修正 */
    if (MED_AEC_DTD_FLAG_DT == enDtdFlagLine)
    {
        enDtdFlagFix = enDtdFlagNline;
    }
    else
    {
        enDtdFlagFix = enDtdFlagLine;
    }

    /* 4. 双讲hangover过程:连续双讲，则进行双讲hangover */
    shwDtConLen = pstDtd->shwDtConLen;

    if (enDtdFlagFix == MED_AEC_DTD_FLAG_DT)
    {
        shwDtConLen = shwDtConLen + 1;
    }
    else
    {
        shwDtConLen = 0;
    }

    /* 若连续双讲帧数超过阈值，则进行双讲hangover */
    shwDtHangLen = pstDtd->shwDtHangLen;
    if (shwDtConLen > MED_AEC_DTD_NLINE_DT_CONTINUOUS_LEN)
    {
        shwDtHangLen = MED_AEC_DTD_NLINE_DT_HANGOVER_LEN;
    }

    if (enDtdFlagFix == MED_AEC_DTD_FLAG_PASS)
    {
        shwDtHangLen = 0;
    }
    else
    {
        if (shwDtHangLen > 0)
        {
            enDtdFlagFix = MED_AEC_DTD_FLAG_DT;
            shwDtHangLen = shwDtHangLen - 1;
        }
    }


    /* 更新双讲hangover结构体参数 */
    pstDtd->shwDtConLen = shwDtConLen;
    pstDtd->shwDtHangLen = shwDtHangLen;

    /* 5. 增强单讲功能打开时,根据远端能量大小标志强制判断单讲 */
    if (CODEC_SWITCH_ON == pstDtd->enEstFlag)
    {
        if (1 == pstDtd->shwFarLargeFlag)
        {
            enDtdFlagFix = MED_AEC_DTD_FLAG_ST;
        }
    }

    /* 6. 最终判断标志赋值 */
    pstDtd->enDtdFlag = enDtdFlagFix;

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_Dtd);

}


#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif

