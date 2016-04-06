/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fr_vad.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年8月15日
  最近修改   :
  功能描述   : FR VAD模块函数实现
  函数列表   :
              FR_VadAdaptByPvadWithFac
              FR_VadAdaptByPvadWithMargin
              FR_VadAdaptThreshold
              FR_VadAverAcf
              FR_VadCompareFloat
              FR_VadCompareSpectral
              FR_VadComputeEnergy
              FR_VadComputeRav1
              FR_VadDecideAdapt
              FR_VadDeciVad
              FR_VadDetect
              FR_VadDetectPeriod
              FR_VadHangover
              FR_VadInit
              FR_VadPredictValues
              FR_VadSchurRecursion
              FR_VadUpdatePeriod
              FR_VadUpdateStep

  修改历史   :
  1.日    期   : 2011年8月15日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "fr_codec.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : FR_VadInit
 功能描述  : FR VAD模块初始化
             初始化值参见协议3GPP TS 46.032中Talbe3.1
 输入参数  : FR_VAD_STATE_STRU *pstVadState  --VAD结构体指针
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月6日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadInit(FR_VAD_STATE_STRU *pstVadState)
{
    Word32                              swCnt = 0;

    /*初始化rvad状态变量*/
    pstVadState->ashwRvad[0]            = FR_VAD_INIT_VALUE_RVAD0;
    pstVadState->ashwRvad[1]            = FR_VAD_INIT_VALUE_RVAD1;
    pstVadState->ashwRvad[2]            = FR_VAD_INIT_VALUE_RVAD2;

    CODEC_OpVecSet(&(pstVadState->ashwRvad[3]), 6, 0);

    /*初始化rvad比例因子*/
    pstVadState->shwNormRvad            = FR_VAD_INIT_VALUE_NORM_RVAD;

    /*初始化自相关系数*/
    for (swCnt = 0; swCnt <= 26; swCnt++)
    {
        pstVadState->aswAcf[swCnt]     = 0;
    }

    /*初始化自相关系数均值*/
    for (swCnt = 0; swCnt <= 35; swCnt++)
    {
        pstVadState->aswSav0[swCnt]    = 0;
    }

    /*初始化sacf延迟点数*/
    pstVadState->shwPtSacf              = 0;

    /*初始化sav0延迟点数*/
    pstVadState->shwPtSav0              = 0;

    /*初始化距离测量值*/
    pstVadState->swLastDm               = 0;

    /*初始化周期累计值*/
    pstVadState->shwOldLagCnt           = 0;
    pstVadState->shwVeryOldLagCnt       = 0;

    /*初始化LTP周期*/
    pstVadState->shwOldLag              = FR_VAD_INIT_VALUE_OLD_LAG;

    /*初始化VAD判定自适应阈值*/
    pstVadState->stThvad.shwExp         = FR_VAD_INIT_VALUE_THVAD_E;
    pstVadState->stThvad.shwMnts        = FR_VAD_INIT_VALUE_THVAD_M;

    /*初始化自适应帧数累计值*/
    pstVadState->shwAdaptCnt            = 0;

    /*初始化语音Burst帧数累计*/
    pstVadState->shwBurstCnt            = 0;

    /*初始化hangover帧数累计*/
    pstVadState->shwHangCnt             = FR_VAD_INIT_VALUE_HANG_COUNT;

    /* 周期检测标志初始化为1，与ZSP500上一致 */
    pstVadState->shwPtch                = 1;
}

/*****************************************************************************
 函 数 名  : FR_VadDetect
 功能描述  : FR VAD检测主调函数
 输入参数  : Word32                *pswAcf      --自相关系数指针
             Word16                shwScalauto  --自相关系数的缩放系数
             Word16                *pshwNc      --4个子块的LTP延迟指针
             FR_VAD_STATE_STRU *pstVadState --VAD结构体状态
 输出参数  : FR_VAD_STATE_STRU *pstVadState --VAD结构体状态
 返 回 值  : FR_VAD_STATUS_ENUM_UINT16          --VAD检测标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadDetect(
                Word32                 *pswAcf,
                Word16                  shwScalauto,
                Word16                 *pshwNc,
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word16                              shwScalVad, shwNormRav1;
    Word16                              ashwRav1[FR_AR_LEN];
    Word16                              shwState;
    FR_VAD_STATUS_ENUM_UINT16           enVvad, enVadStatus;
    Word32                              aswAv0[FR_AR_LEN], aswAv1[FR_AR_LEN];
    FR_FLOAT_STRU                       stVad, stAcf0;

    /* 协议3GPP TS 46.032-- section 6.1 --，自适应滤波和能量计算 */
    shwScalVad = FR_VadComputeEnergy(pswAcf, shwScalauto, &stAcf0, &stVad, pstVadState);

    /* 协议3GPP TS 46.032-- section 6.2 --，计算ACF均值 */
    FR_VadAverAcf(pswAcf, shwScalVad, aswAv0, aswAv1, pstVadState);

    /* 协议3GPP TS 46.032 -- section 6.3 -- 计算自相关预测值 */
    FR_VadPredictValues(aswAv1, ashwRav1, &shwNormRav1);

    /* 协议3GPP TS 46.032 -- section 6.4 -- 谱比较，计算谱特性标志 */
    shwState = FR_VadCompareSpectral(ashwRav1,
                                     shwNormRav1,
                                     aswAv0,
                                     pstVadState);

    /* 协议3GPP TS 46.032 -- 6.5 -- */
    /* FR_VadDetectPeriod(pVadState); 该函数滞后，与zsp500保持一致，但与协议不一致 */

    /* 协议3GPP TS 46.032 -- section 6.6 -- 对VAD判定阈值进行调整 */
    FR_VadAdaptThreshold(shwState,
                         0,
                         shwNormRav1,
                         stAcf0,
                         stVad,
                         ashwRav1,
                         pstVadState);

    /* 协议3GPP TS 46.032 -- section 6.7 -- 进行VAD判决 */
    enVvad = FR_VadDeciVad(stVad, pstVadState->stThvad);

    /* 协议3GPP TS 46.032 -- section 6.8 -- 进行hangover处理 */
    enVadStatus = FR_VadHangover(enVvad,
                                 &(pstVadState->shwBurstCnt),
                                 &(pstVadState->shwHangCnt));

    /* 协议3GPP TS 46.032 -- section 6.9 -- 进行周期性更新 */
    FR_VadUpdatePeriod(pshwNc, pstVadState);

    /* 协议3GPP TS 46.032 -- section 6.5 -- 更新周期检测标志 */
    FR_VadDetectPeriod(pstVadState);

    return enVadStatus;
}

/*****************************************************************************
 函 数 名  : FR_VadComputeEnergy
 功能描述  : 协议3GPP TS 46.032-- section 6.1 --，自适应滤波和能量计算
 输入参数  : const Word32          aswAcf[]      --自相关系数
             Word16                shwScalauto   --自相关系数的缩放系数
             FR_FLOAT_STRU         *pstAcf0      --信号帧能量
             FR_FLOAT_STRU         *pstVadState  --滤波后能量
             FR_VAD_STATE_STRU *pstVadState  --VAD状态结构体
 输出参数  : 无
 返 回 值  : Word16                              --比例系数
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
Word16 FR_VadComputeEnergy(
                const Word32            aswAcf[],
                Word16                  shwScalauto,
                FR_FLOAT_STRU          *pstAcf0,
                FR_FLOAT_STRU          *pstPvad,
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              swCnt;
    Word16                              ashwSacf[FR_AR_LEN];
    Word16                              shwScalVad, shwNormAcf, shwNormProd;
    Word32                              swTemp = 0;

    /* 判别aswAcf[0]是否为零；计算比例系数用于aswAcf均值计算 */
    if (shwScalauto < 0)
    {
        shwScalVad       = 0;
    }
    else
    {
        shwScalVad       = shwScalauto;
    }

    /* 对ACF[0..8]进行归一化处理 */
    shwNormAcf           = norm_l(aswAcf[0]);

    for(swCnt = 0; swCnt <= 8; swCnt++)
    {
        ashwSacf[swCnt] =(Word16)(L_shl(aswAcf[swCnt], shwNormAcf) >> 19);
    }

    /* 计算信号能量acf0的指数和尾数部分 */
    pstAcf0->shwExp  = add(32, shl(shwScalVad,1));
    pstAcf0->shwExp  = sub(pstAcf0->shwExp, shwNormAcf);
    pstAcf0->shwMnts = shl(ashwSacf[0],3);

    /* 计算滤波能量pvad的指数和尾数部分 */
    pstPvad->shwExp  = add(pstAcf0->shwExp, 14);
    pstPvad->shwExp  = sub(pstPvad->shwExp, pstVadState->shwNormRvad);

    for (swCnt = 1; swCnt <= 8; swCnt++)
    {
        swTemp = L_add(swTemp, L_mult(ashwSacf[swCnt], pstVadState->ashwRvad[swCnt]));
    }

    swTemp     = L_add(swTemp, L_shr(L_mult(ashwSacf[0],pstVadState->ashwRvad[0]),1));

    if (swTemp <= 0)
    {
        swTemp = 1;
    }

    shwNormProd      = norm_l(swTemp);

    /* 更新滤波能量pvad的值 */
    pstPvad->shwExp  = sub(pstPvad->shwExp, shwNormProd);
    pstPvad->shwMnts = (Word16)(L_shl(swTemp, shwNormProd) >> 16);

    return shwScalVad;
}


/*****************************************************************************
 函 数 名  : FR_VadAverAcf
 功能描述  : 协议3GPP TS 46.032-- section 6.2 --，计算ACF均值
 输入参数  : const Word32          aswAcf        --自相关系数
             const Word16          shwScalvad    --比例因子
             FR_VAD_STATE_STRU *pstVadState  --vad结构体状态
 输出参数  : Word32                aswAv0[]      --自相关系数均值0
             Word32                aswAv1[]      --自相关系数均值1
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadAverAcf(
                const Word32            aswAcf[],
                const Word16            shwScalvad,
                Word32                  aswAv0[],
                Word32                  aswAv1[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              swCnt;
    Word16                              shwScal;
    Word32                              swTemp;

    /* 计算缩放因子shwScal */
    shwScal            = sub(10, shl(shwScalvad,1));

    /* 计算aswAv0[0..8] 和 aswAv1[0..8] */
    for (swCnt = 0; swCnt < FR_AR_LEN; swCnt++)
    {
        swTemp         = L_shr(aswAcf[swCnt], shwScal);

        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt], swTemp);
        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt+9], aswAv0[swCnt]);
        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt+18], aswAv0[swCnt]);

        /* 检查数组下标是否越界 */
        if ((pstVadState->shwPtSacf + swCnt) >= 27)
        {
            return;
        }

        pstVadState->aswAcf[pstVadState->shwPtSacf + swCnt] = swTemp;

        aswAv1[swCnt] = pstVadState->aswSav0[pstVadState->shwPtSav0 + swCnt];

        pstVadState->aswSav0[pstVadState->shwPtSav0 + swCnt] = aswAv0[swCnt];
    }

    /* 更新shwPtSacf */
    if (18 == pstVadState->shwPtSacf)
    {
        pstVadState->shwPtSacf = 0;
    }
    else
    {
        pstVadState->shwPtSacf = add(pstVadState->shwPtSacf, 9);
    }

    /* 更新shwPtSav0 */
    if (27 == pstVadState->shwPtSav0)
    {
        pstVadState->shwPtSav0 = 0;
    }
    else
    {
        pstVadState->shwPtSav0 = add(pstVadState->shwPtSav0, 9);
    }
}


/*****************************************************************************
 函 数 名  : FR_VadSchurRecursion
 功能描述  : 协议3GPP TS 46.032 -- section 6.3.1 -- 计算反射系数
 输入参数  : Word32 aswAv1[]    --自相关系数均值1
 输出参数  : Word16 ashwVpar[]  --反射系数
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadSchurRecursion(Word32 aswAv1[], Word16 ashwVpar[])
{
    Word32                              i, j;
    Word16                              shwTemp;
    Word16                              ashwValueP[FR_AR_LEN], ashwValueK[FR_AR_LEN];
    Word16                              ashwSacf[FR_AR_LEN];

    /* 如果自相关系数为0，将反射系数置0后返回 */
    if (0 == aswAv1[0])
    {
        CODEC_OpVecSet(&ashwVpar[0], 9, 0);

        return;
    }

    /* 以aswAvl[0]对自相关系数进行归一化，移位生成新的自相关系数ashwSacf[] */
    shwTemp                 = norm_l(aswAv1[0]);

    for (i = 0; i <= 8; i++)
    {
        ashwSacf[i]         = (Word16)(L_shl(aswAv1[i], shwTemp) >> 16);
    }

    /* 初始化数组ashwValueK[]和ashwValueP[]用于递推算法 */
    for (i = 1; i <= 7; i++)
    {
        ashwValueK[9-i]     = ashwSacf[i];
    }

    CODEC_OpVecCpy(&ashwValueP[0], &ashwSacf[0], 9);

    /* 计算反射系数 */
    for (j = 1; j <= 8; j++)
    {
        if ((ashwValueP[0] < abs_s(ashwValueP[1])) || (0 == ashwValueP[0]))
        {
            CODEC_OpVecSet(&ashwVpar[j], (9 - j), 0);

            return;
        }

        ashwVpar[j]         = div_s(abs_s(ashwValueP[1]), ashwValueP[0]);

        if (ashwValueP[1] > 0)
        {
            ashwVpar[j]     = sub(0, ashwVpar[j]);
        }

        if (8 == j)
        {
            break;
        }

        /* Schur递推算法 */
        ashwValueP[0]       = add(ashwValueP[0], mult_r(ashwValueP[1], ashwVpar[j]));

        for (i = 1; i <= (8-j); i++)
        {
            ashwValueP[i]   = add(ashwValueP[i+1], mult_r(ashwValueK[9-i], ashwVpar[j]));
            ashwValueK[9-i] = add(ashwValueK[9-i], mult_r(ashwValueP[i+1], ashwVpar[j]));
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_VadUpdateStep
 功能描述  : 协议3GPP TS 46.032 -- section 6.3.2 -- 计算滤波器系数ashwAav1[]
 输入参数  : Word16 ashwVpar[]   --反射系数
 输出参数  : Word16 ashwAav1[]   --滤波器系数
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadUpdateStep(Word16 ashwVpar[], Word16 ashwAav1[])
{
    Word32                              i, m;
    Word16                              shwTemp;
    Word32                              aswCoef[9] = {0};
    Word32                              aswWork[9] = {0};

    /* 初始化step-up 递归 */
    aswCoef[0]         = L_shl(16384, 15);

    aswCoef[1]         = L_shl(ashwVpar[1], 14);

    /* LPC分析滤波 */
    for (m = 2; m <= 8; m++)
    {
        for (i = 1; i <= (m-1); i++)
        {
            shwTemp    = (Word16)(aswCoef[m-i] >> 16);
            aswWork[i] = L_add(aswCoef[i], L_mult(ashwVpar[m], shwTemp));
        }

        for (i = 1; i <= (m-1); i++)
        {
            aswCoef[i] = aswWork[i];
        }

        aswCoef[m]     = L_shl(ashwVpar[m],14);
    }

    /* 保存aav1[0..8] */
    for (i = 0; i <= 8; i++)
    {
        ashwAav1[i]    = (Word16)L_shr(aswCoef[i],19);
    }

    return;
}

/*****************************************************************************
 函 数 名  : FR_VadComputeRav1
 功能描述  : 协议3GPP TS 46.032 -- section 6.3.3 -- 计算自相关预测值
 输入参数  : Word16 ashwAav1[]     --滤波器系数
 输出参数  : Word16 ashwRav1[]     --反射系数
             Word16 *pshwNormRav1  --反射系数缩放因子
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadComputeRav1(Word16 ashwAav1[], Word16 ashwRav1[], Word16 *pshwNormRav1)
{
    Word32                              i, k;
    Word32                              aswWork[9];
    Word16                              shwNormRav1;

    /* 计算反射系数缩放因子 */
    for (i = 0; i <= 8; i++)
    {
        aswWork[i]     = 0;

        for (k = 0; k <= (8-i); k++)
        {
            aswWork[i] = L_add(aswWork[i], L_mult(ashwAav1[k], ashwAav1[k+i]));
        }
    }

    if (0 == aswWork[0])
    {
        /* 反射系数缩放因子置0 */
        shwNormRav1    = 0;
    }
    else
    {
        /* 反射系数缩放因子置aswWork[0]归一化 */
        shwNormRav1    = norm_l(aswWork[0]);
    }

    /* 计算反射系数 */
    for (i = 0; i <= 8; i++)
    {
        ashwRav1[i]    =(Word16)(L_shl(aswWork[i], shwNormRav1) >> 16);
    }

    *pshwNormRav1      = shwNormRav1;
}

/*****************************************************************************
 函 数 名  : FR_VadPredictValues
 功能描述  : 协议3GPP TS 46.032 -- section 6.3 -- 计算自相关预测值
 输入参数  : Word32 aswAv1[]       --自相关系数均值1
 输出参数  : Word32 aswAv1[]       --自相关系数均值1
             Word16 ashwRav1[]     --自相关预测值
             Word16 *pshwNormrav1  --缩放因子
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadPredictValues(Word32 aswAv1[], Word16 ashwRav1[], Word16 *pshwNormrav1)
{
    Word16                              ashwVpar[9], ashwAav1[9];

    /* 协议3GPP TS 46.032 -- section 6.3.1 -- 计算反射系数 */
    FR_VadSchurRecursion(aswAv1, ashwVpar);

    /* 协议3GPP TS 46.032 -- section 6.3.2 -- 计算滤波器系数ashwAav1[] */
    FR_VadUpdateStep(ashwVpar, ashwAav1);

    /* 协议3GPP TS 46.032 -- section 6.3.3 -- 计算自相关预测值 */
    FR_VadComputeRav1(ashwAav1, ashwRav1, pshwNormrav1);
}

/*****************************************************************************
 函 数 名  : FR_VadCompareSpectral
 功能描述  : 协议3GPP TS 46.032 -- section 6.4 -- 谱比较，计算谱特性标志
 输入参数  : Word16                ashwRav1[]     --自相关预测值
             Word16                shwNormRav1    --自相关预测值缩放因子
             Word32                aswAv0[]       --自相关系数均值0
             FR_VAD_STATE_STRU *pstVadState   --VAD状态
 输出参数  : FR_VAD_STATE_STRU *pstVadState   --VAD状态
 返 回 值  : Word16    --谱特性标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
Word16 FR_VadCompareSpectral(
                Word16                  ashwRav1[],
                Word16                  shwNormRav1,
                Word32                  aswAv0[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              i;
    Word16                              shwStatus;
    Word16                              ashwSav0[9], shwShift, shwDivShift, shwTemp;
    Word32                              swSump, swTemp, swDm;

    /* 归一化aswAv0[0..8] */
    if (0 == aswAv0[0])
    {
        CODEC_OpVecSet(&ashwSav0[0], 9, FR_VAD_MAX_VALUE_13BITS);
    }
    else
    {
        shwShift              = norm_l(aswAv0[0]);
        for (i = 0; i <= 8; i++)
        {
            ashwSav0[i]       = (Word16)(L_shl(aswAv0[i],shwShift-3) >> 16);
        }
    }

    /* 计算公式分子中求和部分swSump */

    swSump                    = 0;

    for (i = 1; i <= 8; i++)
    {
        swSump                = L_add(swSump, L_mult(ashwRav1[i], ashwSav0[i]));
    }

    /* 取swSump绝对值赋给swTemp */
    swTemp                    = L_abs(swSump);

    /* 计算分子中求和部分与分母的比值swDm */
    if (0 == swTemp)
    {
        swDm                  = 0;
        shwShift              = 0;
    }
    else
    {
        ashwSav0[0]           = shl(ashwSav0[0],3);

        shwShift              = norm_l(swTemp);

        shwTemp               = (Word16)(L_shl(swTemp, shwShift) >> 16);

        if (ashwSav0[0] >= shwTemp)
        {
            shwDivShift       = 0;
            shwTemp           = div_s(shwTemp, ashwSav0[0]);
        }
        else
        {
            shwDivShift       = 1;
            shwTemp           = sub(shwTemp, ashwSav0[0]);
            shwTemp           = div_s(shwTemp, ashwSav0[0]);
        }

        if (1 == shwDivShift)
        {
            swDm              = FR_VAD_SET_VALUE_DM;
        }
        else
        {
            swDm              = 0;
        }

        swDm                  = L_shl(L_add(swDm, shwTemp), 1);

        if (swSump < 0)
        {
            swDm              = L_sub(0, swDm);
        }
    }

    /* 归一化并计算swDm */
    swDm                      = L_shl(swDm, 14);
    swDm                      = L_shr(swDm, shwShift);
    swDm                      = L_add(swDm, L_shl(ashwRav1[0],11));
    swDm                      = L_shr(swDm, shwNormRav1);

    /* 计算当前帧swDm与上帧swLastDm差值并保存本帧swDm */
    swTemp                    = L_sub(swDm, pstVadState->swLastDm);
    pstVadState->swLastDm     = swDm;

    if (swTemp < 0)
    {
        swTemp                = L_sub(0, swTemp);
    }

    swTemp                    = L_sub(swTemp, 3277);

    /* 计算谱特性标志 */
    shwStatus                 = (swTemp < 0) ? 1 : 0;

    return shwStatus;
}

/*****************************************************************************
 函 数 名  : FR_VadDetectPeriod
 功能描述  : 协议3GPP TS 46.032 -- section 6.5 -- 更新周期检测标志
 输入参数  : FR_VAD_STATE_STRU *pstVadState   --VAD结构体状态
 输出参数  : FR_VAD_STATE_STRU *pstVadState   --VAD结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadDetectPeriod(FR_VAD_STATE_STRU *pstVadState)
{
    Word16                              shwTemp;
    Word16                              shwPtch;

    /* 计算周期累计值 */
    shwTemp = add(pstVadState->shwOldLagCnt, pstVadState->shwVeryOldLagCnt);

    /* 根据周期累计值大小更新周期检测标志 */
    if (shwTemp >= 4)
    {
        shwPtch               = 1;
    }
    else
    {
        shwPtch               = 0;
    }

    pstVadState->shwPtch      = shwPtch;
}


/*****************************************************************************
 函 数 名  : FR_VadAdaptThreshold
 功能描述  : 协议3GPP TS 46.032 -- section 6.6 -- 对VAD判定阈值进行调整
 输入参数  : Word16                shwSpecStat   --频谱稳定性标志
             Word16                shwTone       --tone音标志
             Word16                shwNormRav1   --反射系数缩放因子
             FR_FLOAT_STRU         stAcf0        --信号能量
             FR_FLOAT_STRU         stPvad        --滤波后能量
             Word16                ashwRav1[]    --反射系数
             FR_VAD_STATE_STRU *pstVadState  --VAD状态
 输出参数  : FR_VAD_STATE_STRU *pstVadState  --VAD状态
 返 回 值  : void
 调用函数  : FR_VadCompareFloat
             FR_VadAdaptByPvadWithFac
             FR_VadAdaptByPvadWithMargin
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadAdaptThreshold(
                Word16                  shwSpecStat,
                Word16                  shwTone,
                Word16                  shwNormRav1,
                FR_FLOAT_STRU           stAcf0,
                FR_FLOAT_STRU           stPvad,
                Word16                  ashwRav1[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word16                              shwComp;
    FR_COMPARE_ENUM_UINT16              uhwComp;
    FR_FLOAT_STRU                       stThesh;

    stThesh.shwExp                      = FR_VAD_THRESH_E_PTH;
    stThesh.shwMnts                     = FR_VAD_THRESH_M_PTH;

    /* 比较信号能量与pth调整值大小 */
    uhwComp = FR_VadCompareFloat(stAcf0, stThesh);
    if (FR_COMPARE_SMALL == uhwComp)
    {
        pstVadState->stThvad.shwExp    = FR_VAD_THRESH_E_PLEV;
        pstVadState->stThvad.shwMnts   = FR_VAD_THRESH_M_PLEV;
        return;
    }

    /* 判断是否需要阈值调整 */
    shwComp = FR_VadDecideAdapt(pstVadState->shwPtch, shwSpecStat, shwTone);
    if (1 == shwComp)
    {
        pstVadState->shwAdaptCnt        = 0;  /* 自适应帧数累计置0 */
        return;
    }

    pstVadState->shwAdaptCnt            = add(pstVadState->shwAdaptCnt, 1);
    if (pstVadState->shwAdaptCnt <= 8)
    {
        return;
    }

    /* 计算当前VAD判定阈值 */
    pstVadState->stThvad.shwMnts        = sub(pstVadState->stThvad.shwMnts,
                                              shr(pstVadState->stThvad.shwMnts, 5));
    if ( pstVadState->stThvad.shwMnts < 16384)
    {
        pstVadState->stThvad.shwMnts    = shl(pstVadState->stThvad.shwMnts,1);
        pstVadState->stThvad.shwExp     = sub(pstVadState->stThvad.shwExp, 1);
    }

    /* 根据滤波后能量加权进行调整 */
    FR_VadAdaptByPvadWithFac(stPvad, &pstVadState->stThvad);

    /* 根据滤波后能量叠加边缘调整值进行调整 */
    FR_VadAdaptByPvadWithMargin(stPvad, &pstVadState->stThvad);

    /* 更新自相关系数 */
    pstVadState->shwNormRvad            = shwNormRav1;

    CODEC_OpVecCpy(&(pstVadState->ashwRvad[0]), &ashwRav1[0], 9);

    /* 更新自适应帧数累计为9 */
    pstVadState->shwAdaptCnt            = 9;
}

/*****************************************************************************
 函 数 名  : FR_VadCompareFloat
 功能描述  : 比较浮点x与浮点y大小，若x<y则返回1
 输入参数  : FR_FLOAT_STRU stFloatx  --浮点x
             FR_FLOAT_STRU stFloaty  --浮点y
 输出参数  : 无
 返 回 值  : Word16    --比较标志
 调用函数  :
 被调函数  : FR_VadAdaptThreshold

 修改历史      :
  1.日    期   : 2011年5月6日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_COMPARE_ENUM_UINT16 FR_VadCompareFloat(FR_FLOAT_STRU stFloatx, FR_FLOAT_STRU stFloaty)
{
    FR_COMPARE_ENUM_UINT16                              enComp = FR_COMPARE_BIG;

    /* 如果stPloatx的指数小于stPloaty的指数，比较标志置1 */
    if (stFloatx.shwExp < stFloaty.shwExp)
    {
        enComp     = FR_COMPARE_SMALL;
    }

    /* 如果stPloatx的指数等于stPloaty的指数且stPloatx的尾数小于stPloaty的尾数，比较标志置1 */
    if (stFloatx.shwExp == stFloaty.shwExp)
    {
        if (stFloatx.shwMnts < stFloaty.shwMnts)
        {
            enComp = FR_COMPARE_SMALL;
        }
    }

    return enComp;
}

/*****************************************************************************
 函 数 名  : FR_VadDecideAdapt
 功能描述  : 判断是否需要进行VAD阈值调整
 输入参数  : Word16 shwPtch      --周期标志
             Word16 shwSpecStat  --谱稳定特性标志
             Word16 shwTone      --tone音标志
 输出参数  : 无
 返 回 值  : Word16      --是否需要VAD阈值调整标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月6日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
Word16 FR_VadDecideAdapt(Word16 shwPtch, Word16 shwSpecStat, Word16 shwTone)
{
    Word16                              shwComp = 0;

    if (1 == shwPtch)
    {
        shwComp = 1;
    }

    if (0 == shwSpecStat)
    {
        shwComp = 1;
    }

    if (1 == shwTone)
    {
        shwComp = 1;
    }

    return shwComp;
}

/*****************************************************************************
 函 数 名  : FR_VadAdaptByPvadWithFac
 功能描述  : 根据滤波后能量加权对VAD判定阈值进行调整
 输入参数  : FR_FLOAT_STRU stPvad       --滤波后能量值
             FR_FLOAT_STRU *pstThvad    --VAD判定阈值指针
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月6日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadAdaptByPvadWithFac(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad)
{
    Word32                              swTemp;
    FR_FLOAT_STRU                       stTemp;
    FR_COMPARE_ENUM_UINT16              uhwComp, uhwComp2;

    /* 计算滤波后能量加权值 */
    swTemp                    = L_add(stPvad.shwMnts, stPvad.shwMnts);
    swTemp                    = L_add(swTemp, stPvad.shwMnts);
    swTemp                    = L_shr(swTemp,1);
    stTemp.shwExp             = add(stPvad.shwExp, 1);

    if (swTemp > FR_VAD_MAX_VALUE_16BITS)
    {
        swTemp                = L_shr(swTemp,1);
        stTemp.shwExp         = add(stTemp.shwExp, 1);
    }

    stTemp.shwMnts            = (Word16)swTemp;

    /* 比较域值与滤波后能量加权值 */
    uhwComp = FR_VadCompareFloat(*pstThvad, stTemp);

    /* 当shwComp为1时 计算pstThvad +( pstThvad /inc)和stPvad*fac中最小值 */
    if (FR_COMPARE_SMALL == uhwComp)
    {
        swTemp                = L_add(pstThvad->shwMnts, shr(pstThvad->shwMnts, 4));

        if (swTemp > FR_VAD_MAX_VALUE_16BITS)
        {
            pstThvad->shwMnts = (Word16)L_shr(swTemp,1);
            pstThvad->shwExp  = add(pstThvad->shwExp,1);
        }
        else
        {
            pstThvad->shwMnts = (Word16)swTemp;
        }

        uhwComp2 = FR_VadCompareFloat(stTemp, *pstThvad);
        if (FR_COMPARE_SMALL == uhwComp2)
        {
            pstThvad->shwExp  = stTemp.shwExp;
            pstThvad->shwMnts = stTemp.shwMnts;
        }
    }
}

/*****************************************************************************
 函 数 名  : FR_VadAdaptByPvadWithMargin
 功能描述  : 根据滤波后能量叠加边缘调整值对VAD判决阈值进行调整
 输入参数  : FR_FLOAT_STRU stPvad      --滤波后能量值
             FR_FLOAT_STRU *pstThvad   --VAD判定阈值指针
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadAdaptByPvadWithMargin(
                FR_FLOAT_STRU           stPvad,
                FR_FLOAT_STRU          *pstThvad)
{
    Word16                              shwTemp;
    Word32                              swTempL;
    FR_FLOAT_STRU                       stTemp;
    Word16                              shwComp = 0;

    /* 计算 stPvad + margin */
    if (FR_VAD_THRESH_E_MARGIN == stPvad.shwExp)
    {
        swTempL                = L_add(stPvad.shwMnts, FR_VAD_THRESH_M_MARGIN);
        stTemp.shwMnts         = (Word16)L_shr(swTempL,1);
        stTemp.shwExp          = add(stPvad.shwExp, 1);
    }
    else
    {
        if (stPvad.shwExp > FR_VAD_THRESH_E_MARGIN)
        {
            shwTemp            = sub(stPvad.shwExp, FR_VAD_THRESH_E_MARGIN);
            shwTemp            = shr(FR_VAD_THRESH_M_MARGIN, shwTemp);
            swTempL            = L_add(stPvad.shwMnts, shwTemp);

            if (swTempL > FR_VAD_MAX_VALUE_16BITS)
            {
                stTemp.shwExp  = add(stPvad.shwExp, 1);
                stTemp.shwMnts = (Word16)L_shr(swTempL, 1);
            }
            else
            {
                stTemp.shwExp  = stPvad.shwExp;
                stTemp.shwMnts = (Word16)swTempL;
            }
        }
        else
        {
            shwTemp            = sub(FR_VAD_THRESH_E_MARGIN, stPvad.shwExp);
            shwTemp            = shr(stPvad.shwMnts, shwTemp);
            swTempL            = L_add(FR_VAD_THRESH_M_MARGIN, shwTemp);

            if (swTempL > FR_VAD_MAX_VALUE_16BITS)
            {
                stTemp.shwExp  = add(FR_VAD_THRESH_E_MARGIN, 1);
                stTemp.shwMnts = (Word16)L_shr(swTempL,1);
            }
            else
            {
                stTemp.shwExp  = FR_VAD_THRESH_E_MARGIN;
                stTemp.shwMnts = (Word16)swTempL;
            }
        }
    }

    /* 判断 pstThvad 是否大于 stPvad + margin */
    if (pstThvad->shwExp > stTemp.shwExp)
    {
        shwComp = 1;
    }

    if (pstThvad->shwExp == stTemp.shwExp)
    {
        if (pstThvad->shwMnts > stTemp.shwMnts)
        {
            shwComp            = 1;
        }
    }

    if (1 == shwComp)
    {
        pstThvad->shwExp       = stTemp.shwExp;
        pstThvad->shwMnts      = stTemp.shwMnts;
    }
}

/*****************************************************************************
 函 数 名  : FR_VadDeciVad
 功能描述  : 协议3GPP TS 46.032 -- section 6.7 -- 进行VAD判决
 输入参数  : FR_FLOAT_STRU stPvad       --滤波后能量
             FR_FLOAT_STRU stThvad      --VAD判定阈值
 输出参数  : 无
 返 回 值  : FR_VAD_STATUS_ENUM_UINT16  --VAD初步判定标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadDeciVad(
                FR_FLOAT_STRU           stPvad,
                FR_FLOAT_STRU           stThvad)
{

    FR_VAD_STATUS_ENUM_UINT16           enVadStatus = FR_VAD_STATUS_NOT;

    /* 滤波后能量大于判定阈值时判定标志置1*/
    if (stPvad.shwExp >  stThvad.shwExp)
    {
        enVadStatus                     = FR_VAD_STATUS_YES;
    }

    if (stPvad.shwExp == stThvad.shwExp)
    {
        if (stPvad.shwMnts > stThvad.shwMnts)
        {
            enVadStatus                 = FR_VAD_STATUS_YES;
        }
    }

    return enVadStatus;
}

/*****************************************************************************
 函 数 名  : FR_VadHangover
 功能描述  : 协议3GPP TS 46.032 -- section 6.8 -- 进行hangover处理
 输入参数  : FR_VAD_STATUS_ENUM_UINT16 enVvad   --VAD初步判定结果
             Word16 *pshwBurstCount             --语音burst帧计数
             Word16 *pshwHangCount              --hangover帧计数
 输出参数  : Word16 *pshwBurstCount             --语音burst帧计数
             Word16 *pshwHangCount              --hangover帧计数
 返 回 值  : FR_VAD_STATUS_ENUM_UINT16          --VAD判定标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadHangover(
                FR_VAD_STATUS_ENUM_UINT16                   enVvad,
                Word16                                     *pshwBurstCount,
                Word16                                     *pshwHangCount)
{
    FR_VAD_STATUS_ENUM_UINT16           enVadStatus;
    Word16                              shwBurstCount, shwHangCount;

    shwBurstCount                       = *pshwBurstCount;
    shwHangCount                        = *pshwHangCount;

    if (FR_VAD_STATUS_YES == enVvad)
    {
        shwBurstCount                   = add(shwBurstCount, 1);                /* 语音burst帧计数加1 */
    }
    else
    {
        shwBurstCount                   = 0;
    }

    if (shwBurstCount >= 3)
    {
        shwHangCount                    = 5;                                   /* hangover帧计数置5 */
        shwBurstCount                   = 3;
    }

    enVadStatus                         = enVvad;                               /* VAD判定标志置为VAD初步判定标志 */
    if (shwHangCount >= 0)
    {
        enVadStatus                     = FR_VAD_STATUS_YES;                    /* VAD判定标志置1 */
        shwHangCount                    = sub(shwHangCount, 1);
    }

    *pshwBurstCount                     = shwBurstCount;
    *pshwHangCount                      = shwHangCount;

    return enVadStatus;
}

/*****************************************************************************
 函 数 名  : FR_VadUpdatePeriod
 功能描述  : 协议3GPP TS 46.032 -- section 6.9 -- 进行周期性更新
 输入参数  : const Word16 *pshwLags               --RPE-LTP计算获得的4个子块的延迟
             FR_VAD_STATE_STRU *pstVadState   --VAD结构体状态
 输出参数  : FR_VAD_STATE_STRU *pstVadState   --VAD结构体状态
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
void FR_VadUpdatePeriod(Word16 *pshwLags, FR_VAD_STATE_STRU *pstVadState)
{
    Word32                              i, j;
    Word16                              shwMinLag, shwMaxLag, shwSmalLag;
    Word16                              shwLagCount, shwTemp;

    shwLagCount                         = 0;

    /* 4个子块循环 */
    for (i = 0; i <= 3; i++)
    {
        /* 搜索连续延迟中最大值shwMaxLag和最小值shwMinLag */
        if (pstVadState->shwOldLag > pshwLags[i])
        {
            shwMinLag                   = pshwLags[i];
            shwMaxLag                   = pstVadState->shwOldLag;
        }
        else
        {
            shwMinLag                   = pstVadState->shwOldLag;
            shwMaxLag = pshwLags[i] ;
        }

        /* 计算shwSmalLag*/
        shwSmalLag = shwMaxLag;

        for (j = 0; j <= 2; j++)
        {
            if (shwSmalLag >= shwMinLag)
            {
                shwSmalLag              = sub(shwSmalLag, shwMinLag);
            }
        }

        /* 计算smallag与minlag差值赋予smallag */
        shwTemp = sub(shwMinLag, shwSmalLag);
        if (shwTemp < shwSmalLag)
        {
            shwSmalLag                  = shwTemp;
        }

        if (shwSmalLag < 2)
        {
            shwLagCount                 = add(shwLagCount, 1);
        }

        /* 保存当前LTP延迟 */
        pstVadState->shwOldLag          = pshwLags[i];
    }

    /* 更新结构体状态变量周期累计值shwOldLagCnt和shwVeryOldLagCnt用于下帧vad检测 */
    pstVadState->shwVeryOldLagCnt       = pstVadState->shwOldLagCnt;
    pstVadState->shwOldLagCnt           = shwLagCount;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

