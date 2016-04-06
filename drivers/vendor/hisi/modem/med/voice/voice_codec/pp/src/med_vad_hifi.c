

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_fft.h"
#include "med_vad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef _MED_C89_

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_VAD_OBJ_STRU    g_astMedVadObjPool[MED_VAD_MAX_OBJ_NUM];                    /* VAD实体资源池 */
MED_OBJ_INFO        g_stMedVadObjInfo;                                          /* 相关的信息 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_VAD_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_VAD_GetObjInfoPtr(),
                    MED_VAD_GetObjPtr(),
                    MED_VAD_MAX_OBJ_NUM,
                    sizeof(MED_VAD_OBJ_STRU)));
}


VOS_UINT32 MED_VAD_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_VAD_OBJ_STRU       *pstObj;
    pstObj      = (MED_VAD_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_VAD_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_VAD_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    MED_VAD_OBJ_STRU         *pstObj    = (MED_VAD_OBJ_STRU*)pstInstance;       /* 临时VAD指针 */
    MED_VAD_NV_STRU          *pstNv     = &(pstObj->stNv);                      /* NV项 */
    VOS_UINT32                uwRet;                                            /* 返回值 */

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (shwParaLen < MED_VAD_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取使能项、滤波系数的信息 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = pstNv->shwAlpha;                                  /* 判断门限系数 */
            pshwParam[1]    = pstNv->shwArtiSpeechThd;                          /* 人工语音判决门限 */
            pshwParam[2]    = pstNv->shwChopNumThd;                             /* 人工语音中，频带能量跳变次数 */
        }
    }

    return uwRet;

}


VOS_UINT32 MED_VAD_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    MED_VAD_OBJ_STRU    *pstVad         = (MED_VAD_OBJ_STRU*)pstInstance;       /* 临时VAD指针 */
    MED_VAD_NV_STRU     *pstNv          = &(pstVad->stNv);                      /* NV项 */
    VOS_UINT32           uwRet;                                                 /* 返回值 */

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstVad);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (shwParaLen < MED_VAD_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 因为需要赋值为0的数比较多，需要先将整个结构体数据清零 */
            UCOM_MemSet(pstVad, 0, sizeof(MED_VAD_OBJ_STRU));

            pstVad->enIsUsed                   = CODEC_OBJ_USED;

            /* 赋值门限判决系数、人工语音检测门限，门限平滑系数 */
            pstNv->shwAlpha                    = pshwParam[0];                          /* 判断门限系数 */
            pstNv->shwArtiSpeechThd            = pshwParam[1];                          /* 人工语音判决门限 */
            pstNv->shwChopNumThd               = pshwParam[2];                          /* 人工语音中，频带能量跳变次数 */

            /* 获取帧长信息 */
            pstVad->shwFrmLen                   = MED_PP_GetFrameLength();

            /* 根据帧长，确定成员变量的值 */
            /* 窄带 */
            if (CODEC_FRAME_LENGTH_NB == pstVad->shwFrmLen)
            {
                pstVad->enFftLen                = MED_VAD_FFT_LEN_8K;                   /* FFT的长度 */
                pstVad->enPrevFrmLen            = MED_VAD_PREVFRM_LEN_8K;               /* 上一帧缓存信号的长度 */
                pstVad->shwHalfFftLen           = MED_VAD_HALFFFT_8K;                   /* FFT长度的一半，数值上等于 CODEC_OpShr(stSV.shwFftLen, 1); */
                pstVad->shwHalfFftShift         = MED_VAD_HALFFFT_SHIFT_8K;             /* FFT长度一半用二进制表示，所占的位数 floor(log2(double(stSV.shwHalfFftLen))); */
                pstVad->swMaxFreqEngThd         = MED_VAD_MAX_FREQ_ENG_8K;              /* 最大频域能量门限，数值上等于 CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
                pstVad->swMaxStandardDeviThd    = MED_VAD_MAX_STD_DEVI_THD_8K;          /* 最大频域能量均方差门限，数值上等于 CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
                pstVad->shwFreqEngNormPara      = MED_VAD_FREQ_ENG_NORM_PARA_8K;        /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
                pstVad->shwStdDeviNormPara      = MED_VAD_STD_DEVI_NORM_PARA_8K;        /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
                pstVad->pshwHammingWindow       = &(g_ashwMedVadHammingWin8k[0]);       /* 指向8k(256阶)汉明窗 */
            }

            /* 宽带 */
            else
            {
                pstVad->enFftLen                = MED_VAD_FFT_LEN_16K;                  /* FFT的长度 */
                pstVad->enPrevFrmLen            = MED_VAD_PREVFRM_LEN_16K;              /* 上一帧缓存信号的长度 */
                pstVad->shwHalfFftLen           = MED_VAD_HALFFFT_16K;                  /* FFT长度的一半，数值上等于 CODEC_OpShr(stSV.shwFftLen, 1); */
                pstVad->shwHalfFftShift         = MED_VAD_HALFFFT_SHIFT_16K;            /* FFT长度一半用二进制表示，所占的位数 floor(log2(double(stSV.shwHalfFftLen))); */
                pstVad->swMaxFreqEngThd         = MED_VAD_MAX_FREQ_ENG_16K;             /* 最大频域能量门限，数值上等于 CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
                pstVad->swMaxStandardDeviThd    = MED_VAD_MAX_STD_DEVI_THD_16K;         /* 最大频域能量均方差门限，数值上等于 CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
                pstVad->shwFreqEngNormPara      = MED_VAD_FREQ_ENG_NORM_PARA_16K;       /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
                pstVad->shwStdDeviNormPara      = MED_VAD_STD_DEVI_NORM_PARA_16K;       /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
                pstVad->pshwHammingWindow       = &(g_ashwMedVadHammingWin16k[0]);      /* 指向16k(512阶)汉明窗 */
            }

            /* 其他成员变量初始化 */
            pstVad->shwFrmCount                 = 1;                                    /* 前5帧计数初始化 */
            pstVad->shwModifiedAlpha            = CODEC_OP_INT16_MAX;                     /* 平滑参数 */
        }
    }

    return uwRet;

}


VOS_VOID MED_VAD_UpdateThd(
                MED_VAD_OBJ_STRU       *pstVad,
                CODEC_OP_FLOAT_STRU      *pstFloatCurrTarget)
{
    /* 初始化临时变量 */
    CODEC_OP_FLOAT_STRU   stFloatTemp1;
    CODEC_OP_FLOAT_STRU   stFloatTemp2;
    VOS_INT16           shwTemp;
    VOS_INT32           swTemp;
    VOS_INT16           shwTmpAlpha;                                            /* 修正平滑系数计算参数 */
    VOS_INT32           shwCnt;                                                 /* 计数器 */
    VOS_INT16           shwOptAlpha;                                            /* 最优平滑系数 */
    VOS_INT16           shwLastSmoothAlpha;                                     /* 最终平滑系数 */
    CODEC_OP_FLOAT_STRU   stFloatLastSmoothAlpha;                                 /* 最终平滑系数 (伪浮点)*/

    /* 计算最优平滑系数 */
    stFloatTemp1    = CODEC_OpFloatAdd(pstVad->stFloatMinTarget, CODEC_OP_FLOAT_ONE);
    stFloatTemp1    = CODEC_OpFloatDivU(pstVad->stFloatSmoothedTarget, stFloatTemp1);
    stFloatTemp1.shwE = CODEC_OpAdd(stFloatTemp1.shwE, MED_PP_SHIFT_BY_9);
    shwTemp        = CODEC_OpFloatTo16(stFloatTemp1);                             /* 16bit Q9 */
    shwTemp        = CODEC_OpSub(shwTemp, MED_VAD_1_Q9);                          /* 16bit Q9 */
    swTemp         = CODEC_OpL_add(CODEC_OpL_mult(shwTemp, shwTemp), MED_VAD_1_Q19);/* 32bit Q19 */
    shwOptAlpha    = CODEC_OpSaturate(CODEC_OpNormDiv_32(MED_VAD_1_Q19,
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_15));     /* 16bit Q15 */

    /* 计算修正平滑系数 */
    stFloatTemp1    = CODEC_OpFloatAdd(*pstFloatCurrTarget, CODEC_OP_FLOAT_ONE);
    stFloatTemp1    = CODEC_OpFloatDivU(pstVad->stFloatSmoothedTarget, stFloatTemp1);
    stFloatTemp1.shwE = CODEC_OpAdd(stFloatTemp1.shwE, MED_PP_SHIFT_BY_9);
    shwTemp        = CODEC_OpFloatTo16(stFloatTemp1);                             /* 16bit Q9 */
    shwTemp        = CODEC_OpSub(shwTemp, MED_VAD_1_Q9);                          /* 16bit Q9 */
    swTemp         = CODEC_OpL_add(CODEC_OpL_mult(shwTemp, shwTemp), MED_VAD_1_Q19);/* 32bit Q19 */
    shwTmpAlpha    = CODEC_OpSaturate(CODEC_OpNormDiv_32(MED_VAD_1_Q19,
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_15));     /* 16bit Q15 */
    shwTmpAlpha    = CODEC_OpMax(shwTmpAlpha, MED_VAD_0P7_Q15);
    pstVad->shwModifiedAlpha = CODEC_OpAdd(CODEC_OpMult_r(MED_VAD_0P7_Q15,
                                                      pstVad->shwModifiedAlpha),
                                         CODEC_OpMult_r(MED_VAD_0P3_Q15,
                                                      shwTmpAlpha));

    /* 计算最终平滑系数 */
    shwLastSmoothAlpha = CODEC_OpMult_r(shwOptAlpha, pstVad->shwModifiedAlpha);
    shwLastSmoothAlpha = CODEC_OpMax(MIN_SMOOTH_ALPHA, shwLastSmoothAlpha);

    stFloatLastSmoothAlpha = CODEC_OpToFloat((VOS_INT32)shwLastSmoothAlpha);
    stFloatLastSmoothAlpha.shwE = CODEC_OpSub(stFloatLastSmoothAlpha.shwE,
                                            MED_PP_SHIFT_BY_15);
    stFloatTemp1           = CODEC_OpFloatMul(pstVad->stFloatSmoothedTarget, stFloatLastSmoothAlpha);
    stFloatTemp2           = CODEC_OpFloatMul(*pstFloatCurrTarget,
                                            CODEC_OpFloatSub(CODEC_OP_FLOAT_ONE ,stFloatLastSmoothAlpha));
    pstVad->stFloatSmoothedTarget = CODEC_OpFloatAdd(stFloatTemp1, stFloatTemp2);


    /* 将最终平滑系数赋给环形队列的当前值 */
    UCOM_MemCpy(&(pstVad->astFloatTargetBuf[pstVad->shwRingCnt]),
                &(pstVad->stFloatSmoothedTarget),
                sizeof(CODEC_OP_FLOAT_STRU));


    /* 更新目标最小值，采用环形BUFFER */
    pstVad->shwRingCnt      = pstVad->shwRingCnt + 1;
    if (pstVad->shwRingCnt >= MED_VAD_FRM_NUM_1P5_SECOND)
    {
        pstVad->shwRingCnt  = 0;
    }

    /* 搜索最小值 */
    UCOM_MemCpy(&(pstVad->stFloatMinTarget),
                &(pstVad->astFloatTargetBuf[0]),
                sizeof(CODEC_OP_FLOAT_STRU));

    for (shwCnt = 1; shwCnt < MED_VAD_FRM_NUM_1P5_SECOND; shwCnt++)
    {
        if (CODEC_OpFloatLT(pstVad->astFloatTargetBuf[shwCnt], pstVad->stFloatMinTarget))
        {
            UCOM_MemCpy(&(pstVad->stFloatMinTarget),
                        &(pstVad->astFloatTargetBuf[shwCnt]),
                        sizeof(CODEC_OP_FLOAT_STRU));
        }
    }
}


VOS_INT16 MED_VAD_ArtiSpchDetect(
                    MED_VAD_OBJ_STRU   *pstVad,
                    VOS_INT16           shwAveCurrFrm,
                    VOS_INT32          *aswFreqEng)
{
    MED_VAD_NV_STRU *pstNv              = &(pstVad->stNv);                      /* NV项 */
    VOS_INT32       swFreqEngDiff;                                              /* 能量差 */
    VOS_INT16       shwChopNum          = 0;                                    /* 跳变次数 */
    VOS_INT32       shwCnt, i;                                                     /* 计数器 */
    VOS_INT32       swFreqEng0, swFreqEng1;

    if (shwAveCurrFrm > MED_VAD_ARTI_VOICE_DETECT_AMP_THD)
    {
        for(i = 0; i < MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF; i++)
        {
            swFreqEng0 = aswFreqEng[MED_VAD_ARTI_SPCH_FREQ_BAND_MIN + i];

            for (   shwCnt = MED_VAD_ARTI_SPCH_FREQ_BAND_MIN + i;
                    shwCnt < (pstVad->shwHalfFftLen - MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF);
                    shwCnt += MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF)
            {
                swFreqEng1 = aswFreqEng[shwCnt + MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF];

                swFreqEngDiff = swFreqEng0 / (CODEC_OpL_max(swFreqEng1, 1));

                swFreqEng0 = swFreqEng1;

                if (swFreqEngDiff > pstNv->shwArtiSpeechThd)
                {
                    shwChopNum = shwChopNum + 1;
                }
            }

            if(shwChopNum > pstNv->shwChopNumThd)
            {
                break;
            }

        }
    }

    return shwChopNum;
}


VOS_VOID MED_VAD_FreqEngCal(
                MED_VAD_OBJ_STRU         *pstVad,
                VOS_INT16                *pshwCurrFrm,
                VOS_INT32                *pswFreqEng)
{
    /* 1. 临时变量赋值 */
    VOS_INT16              *pshwFreqDat;                                       /* 频域信息 */
    VOS_INT32               shwCnt;                                            /* 计数器 */
    ae_p24x2s               aepDat1, aepDat2;
    ae_q56s                 aeqEng1, aeqEng2;

    /* 2. FFT */
    pshwFreqDat     = (VOS_INT16*)(&(pstVad->aswBuff[0]));
    if (CODEC_FRAME_LENGTH_NB == pstVad->shwFrmLen)
    {
        /* 窄带 */
        MED_FFT_Fft(MED_FFT_NUM_256 ,pshwCurrFrm, pshwFreqDat);
    }
    else
    {
        /* 宽带 */
        MED_FFT_Fft(MED_FFT_NUM_512 ,pshwCurrFrm, pshwFreqDat);
    }

    /* 3. 计算谱带能量 */
    /* 谱带能量所占用的内存空间和谱带信息占用的空间复用 */
    pswFreqEng[0] = CODEC_OpL_mult0(pshwFreqDat[0], pshwFreqDat[0]);

    aepDat1 = *((ae_p16x2s *)&pshwFreqDat[2]);
    aeqEng1 = AE_MULZAAP24S_HH_LL(aepDat1, aepDat1);
    aeqEng1 = AE_SATQ48S(aeqEng1);
    *((ae_q32s *) &pswFreqEng[1]) = aeqEng1;

    for (shwCnt = 1; shwCnt < (pstVad->shwHalfFftLen>>1); shwCnt++)
    {
        aepDat1 = *((ae_p16x2s *)&pshwFreqDat[4 * shwCnt]);
        aepDat2 = *((ae_p16x2s *)&pshwFreqDat[4 * shwCnt + 2]);

        aeqEng1 = AE_MULZAAP24S_HH_LL(aepDat1, aepDat1);
        aeqEng1 = AE_SATQ48S(aeqEng1);

        aeqEng2 = AE_MULZAAP24S_HH_LL(aepDat2, aepDat2);
        aeqEng2 = AE_SATQ48S(aeqEng2);

        *((ae_q32s *) &pswFreqEng[2 * shwCnt])     = aeqEng1;
        *((ae_q32s *) &pswFreqEng[2 * shwCnt + 1]) = aeqEng2;
    }

}



VOS_UINT32 MED_VAD_Main(
                VOS_VOID               *pstVadObj,
                VOS_INT16              *pshwIn,
                VOS_INT16              *penVadFlag)
{
    /* 1. 临时变量赋值 */
    MED_VAD_NV_STRU         *pstNv;                                             /* NV项 */
    VOS_INT16               *pshwCurrFrm;                                       /* 输入帧和历史帧的结合 */
    VOS_INT32               *pswFreqEng;                                        /* 频带能量 */
    VOS_INT16               *pshwNextFrmHead;                                   /* 下一帧的开头部分，也就是当前帧未加窗前的结尾部分 */
    VOS_INT32                shwCnt;                                            /* 计数器 */
    VOS_INT16                shwShift1         = 0;                             /* 频带能量的归一化系数 */
    VOS_INT16                shwShift2         = 0;                             /* 中间值aswX的归一化系数 */
    VOS_INT32               *pswX;                                              /* 能量差 */
    VOS_INT32                swXbar;                                            /* 归一化之后的频带能量的平均值 */
    VOS_INT32                swMaxX;                                            /* 最大能量差 */
    VOS_INT32                swMaxFreqEng;                                      /* 最大频带能量 */
    VOS_INT32                swD               = 0;                             /* 谱方差 */
    VOS_INT16                shwAveCurrFrm;                                     /* 当前帧绝对值的平均值 */
    VOS_INT16                shwChopNum;                                        /* 幅度跳变计数 */
    CODEC_OP_FLOAT_STRU        stFloatD;                                          /* 谱方差(伪浮点) */
    CODEC_OP_FLOAT_STRU        stFloatTs;                                         /* 谱方差门限 */
    VOS_INT16                shwCheck;                                          /* 逻辑运算结果 */
    VOS_INT32                swSum;                                             /* 向量和，求平均值用 */
    MED_VAD_OBJ_STRU        *pstVad;

    pstVad = (MED_VAD_OBJ_STRU*)pstVadObj;
    pstNv  = &(pstVad->stNv);

    /* 2. 将本帧的信号加到上一帧信号的末尾 */
    pshwCurrFrm     = &(pstVad->ashwFrmDat[0]);
    CODEC_OpVecCpy(&(pshwCurrFrm[pstVad->enPrevFrmLen]),
                 pshwIn,
                 (VOS_INT32)(pstVad->shwFrmLen));

    /* 3. 加Hamming窗 */
    CODEC_OpVvMultR(pshwCurrFrm, pstVad->pshwHammingWindow, pstVad->shwHalfFftLen, pshwCurrFrm);
    CODEC_OpVvMultRD(&(pshwCurrFrm[pstVad->shwHalfFftLen]),
                   &(pstVad->pshwHammingWindow[(pstVad->shwHalfFftLen) - 1]),
                   pstVad->shwHalfFftLen,
                   &(pshwCurrFrm[pstVad->shwHalfFftLen]));

    /* 4. 求当前帧绝对幅度的平均值 */
    shwAveCurrFrm = CODEC_OpVecMeanAbsAlignedQuan(pshwIn, pstVad->shwFrmLen);

    /* 5. 计算当前帧的频谱和能量 */
    pswFreqEng  = &(pstVad->aswBuff[0]);
    MED_VAD_FreqEngCal(pstVad, pshwCurrFrm, pswFreqEng);

    /* 6. 计算谱方差 */
    /* 6.1 求最大频带能量 */
    swMaxFreqEng    = CODEC_OpVecMax32(pswFreqEng, (VOS_INT32)(pstVad->shwHalfFftLen));

    /* 6.2 求频带能量的归一化系数 */
    if (swMaxFreqEng > pstVad->swMaxFreqEngThd)
    {
        shwShift1   = CODEC_OpAdd(CODEC_OpSub(pstVad->shwFreqEngNormPara, CODEC_OpNorm_l(swMaxFreqEng)), 1);/* 归一化系数 */
    }

    /* 6.3 频带能量归一化 */
    if(shwShift1 != 0)
    {
        CODEC_OpVecShr_r32(pswFreqEng,
                         (VOS_INT32)(pstVad->shwHalfFftLen),
                         shwShift1,
                         pswFreqEng);
    }

    /* 6.4 求归一化之后的频带能量的平均值 */
    swSum  = 0;
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        swSum = CODEC_OpL_add(pswFreqEng[shwCnt], swSum);
    }
    swXbar = CODEC_OpL_shr_r(swSum, pstVad->shwHalfFftShift);

    /* 7. 从语音中检测出人工语音 */
    shwChopNum = MED_VAD_ArtiSpchDetect(pstVad, shwAveCurrFrm, pswFreqEng);

    /* 6.5 求能量差aswX并求它的最大值 */
    pswX            = &(pstVad->aswBuff[0]);
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        pswX[shwCnt]= CODEC_OpL_sub(pswFreqEng[shwCnt], swXbar);
    }
    swMaxX = CODEC_OpVecMax32(pswX, pstVad->shwHalfFftLen);

    /* 6.6 求频带能量差的进一步归一化系数 */
    if (swMaxX > (pstVad->swMaxStandardDeviThd))
    {
        shwShift2   = CODEC_OpAdd(CODEC_OpSub(pstVad->shwStdDeviNormPara, CODEC_OpNorm_l(swMaxX)), 1);  /* 归一化系数2 */
    }

    /* 6.7 频带能量差归一化 */
    if(shwShift2 != 0)
    {
        CODEC_OpVecShr_r32(pswX,
                         (VOS_INT32)(pstVad->shwHalfFftLen),
                         shwShift2,
                         pswX);
    }

    /* 6.8 计算谱方差 */
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        swD = CODEC_OpL_add(swD,
                          CODEC_OpL_mult0((VOS_INT16)(pswX[shwCnt]),
                                        (VOS_INT16)(pswX[shwCnt])));
    }
    swD = CODEC_OpL_shr_r(swD, pstVad->shwHalfFftShift);

    /* 6.9 将谱方差转换为伪浮点 */
    stFloatD      = CODEC_OpToFloat(swD);
    stFloatD.shwE = CODEC_OpAdd(stFloatD.shwE,
                              CODEC_OpAdd(shwShift1, CODEC_OpShl(shwShift2, 1)));

    /* 8. 门限初始化和更新 */
    if (pstVad->shwFrmCount <= MED_VAD_STARTING_FRM_LEN)
    {
        /* 该部分代码用于计算前5帧的语音判决门限 */

        pstVad->stFloatEmbryonicThd     = CODEC_OpFloatAdd(pstVad->stFloatEmbryonicThd,
                                                    stFloatD);
        pstVad->shwFrmCount             = pstVad->shwFrmCount + 1;
        stFloatTs.shwM                  = CODEC_OP_INT16_MAX;                     /* 判决门限设为最大值 */
        stFloatTs.shwE                  = CODEC_OP_INT16_MAX / 2;
    }
    else if (pstVad->shwFrmCount == (MED_VAD_STARTING_FRM_LEN + 1))
    {
        pstVad->stFloatEmbryonicThd     = CODEC_OpFloatDivU(pstVad->stFloatEmbryonicThd,
                                                          CODEC_OpToFloat(MED_VAD_STARTING_FRM_LEN));
        for (shwCnt = 0; shwCnt < MED_VAD_FRM_NUM_1P5_SECOND; shwCnt++)
        {
            UCOM_MemCpy(&(pstVad->astFloatTargetBuf[shwCnt]),
                        &(pstVad->stFloatEmbryonicThd),
                        sizeof(CODEC_OP_FLOAT_STRU));
        }

        stFloatTs                       = pstVad->stFloatEmbryonicThd;
        pstVad->stFloatSmoothedTarget   = stFloatTs;
        pstVad->shwFrmCount             = pstVad->shwFrmCount + 1;
    }
    else
    {
        MED_VAD_UpdateThd(pstVad, &stFloatD);
        stFloatTs                       = pstVad->stFloatMinTarget;
    }

    /* 9. 将当前帧的一部分或者全部保存为下一帧加窗时的“前一帧”数据，*/
    /* 因为存在 shwFrmLen <= shwFftLen  的约束，因此这里不需要进行判断 */
    pshwNextFrmHead = &(pstVad->ashwFrmDat[0]);
    CODEC_OpVecCpy(pshwNextFrmHead,
                 &(pshwIn[(2 * pstVad->shwFrmLen) - pstVad->enFftLen]),
                 (VOS_INT32)(pstVad->enPrevFrmLen));

    /* 10. 语音判决 */
    if (shwAveCurrFrm < MED_VAD_SMOOTH_AMP_UPDATE_THD)
    {
        pstVad->shwSmoothedValue        = CODEC_OpAdd(
                                          CODEC_OpMult(MED_VAD_SMOOTH_FRM_AMP_ALPHA,
                                                     pstVad->shwSmoothedValue),
                                          CODEC_OpMult(MED_VAD_CURR_FRM_ALPHA,
                                                     shwAveCurrFrm));
    }

    shwCheck =    (CODEC_OpFloatLT(CODEC_OpFloatMul(stFloatTs, CODEC_OpToFloat((VOS_INT32)pstNv->shwAlpha)), stFloatD)
                         || (shwChopNum > pstNv->shwChopNumThd))
               && ((pstVad->shwSmoothedValue > MED_VAD_SMOOTH_AMP_THD)
                         || (shwAveCurrFrm > MED_VAD_AVE_AMP_CURR_FRM_THD));

    if (VOS_TRUE == shwCheck)
    {
        *penVadFlag                     = CODEC_VAD_FLAG_SPEECH;             /* 有语音 */
    }
    else
    {
        *penVadFlag                     = CODEC_VAD_FLAG_NOISE;              /* 无语音 */
    }

    return UCOM_RET_SUCC;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

