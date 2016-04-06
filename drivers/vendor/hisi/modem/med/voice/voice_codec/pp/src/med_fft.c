

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_fft.h"
#include "codec_op_lib.h"
#include "codec_op_cpx.h"
#include "voice_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _MED_C89_
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* FFT全局实体 */
MED_FFT_CORE_STRU                         g_stMedFftObj[12];

/* FFT计算临时缓存区 */
CODEC_OP_CMPX_STRU                          g_astBuf[MED_FFT_1024_CMPX_LEN];

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID MED_FFT_InitAllObjs()
{
    MED_FFT_CORE_STRU                  *pstCore = MED_NULL;


    /* 指定128阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[0];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_128_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_64_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft64RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft64SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft128Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft128SuperTwiddles[0]);

    /* 指定128阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[6];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_128_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_64_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft64RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft64SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft128Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft128SuperTwiddles[0]);

    /* 指定256阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[1];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_256_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_128_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft128RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft128SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft256Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft256SuperTwiddles[0]);

    /* 指定256阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[7];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_256_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_128_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft128RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft128SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft256Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft256SuperTwiddles[0]);

    /* 指定320阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[2];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_320_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_160_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft160RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft160SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft320Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft320SuperTwiddles[0]);

    /* 指定320阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[8];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_320_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_160_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft160RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft160SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft320Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft320SuperTwiddles[0]);

    /* 指定512阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[3];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_512_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_256_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft256RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft256SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft512Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft512SuperTwiddles[0]);

    /* 指定512阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[9];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_512_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_256_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft256RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft256SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft512Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft512SuperTwiddles[0]);

    /* 指定640阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[4];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_640_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_320_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft320RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft320SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft640Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft640SuperTwiddles[0]);

    /* 指定640阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[10];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_640_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_320_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft320RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft320SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft640Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft640SuperTwiddles[0]);
    /* 指定1024阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[5];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_1024_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_512_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft512RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft512SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft1024Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft1024SuperTwiddles[0]);

    /* 指定1024阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[11];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_1024_CMPX_LEN;
    pstCore->shwNumOfRadix  = MED_FFT_512_FACTOR_NUM;
    pstCore->pshwFactors    = (VOS_INT16*)&g_ashwMedFft512RadixAndStride[0];
    pstCore->pshwSortTable  = (VOS_INT16*)&g_ashwMedFft512SortTable[0];
    pstCore->pstTwiddles    = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft1024Twiddles[0]);
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft1024SuperTwiddles[0]);
}


VOS_VOID MED_FFT_Cmpx(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstIn,
                CODEC_OP_CMPX_STRU       *pstOut)
{
    VOS_INT32                           swCnt           = 0;
    VOS_INT32                           swCntK          = 0;
    VOS_INT32                           swCntM          = 0;
    VOS_INT16                           shwDblCnt       = 0;
    VOS_INT16                           shwRadix        = 0;                    /* 基数 */
    VOS_INT16                           shwStride       = 0;                    /* 步长 */
    VOS_INT16                           shwStageNum     = 0;                    /* 基数为shwRadix的运算数目 */
    VOS_INT16                           shwStageLen     = 0;                    /* 计算长度 */
    VOS_INT16                           shwNextStageNum = 0;                    /* 下一个shwStageNum */
    VOS_INT16                          *pshwFactors     = pstObj->pshwFactors;
    VOS_INT16                          *pshwSortTbl     = pstObj->pshwSortTable;
    CODEC_OP_CMPX_STRU                   *pstOutBeg;

    pstOutBeg = pstOut;

    /* 位序重排 */
    for (swCnt = pstObj->shwN - 1; swCnt >= 1; swCnt -= 2)
    {
        *(pstOut + swCnt)       = *(pstIn + *(pshwSortTbl + swCnt));
        *(pstOut + (swCnt - 1)) = *(pstIn + *(pshwSortTbl + (swCnt - 1)));
    }

    /* 混合基处理过程 */
    for (swCnt = pstObj->shwNumOfRadix - 1; swCnt >=0; swCnt--)
    {
        shwDblCnt   = (VOS_INT16)(swCnt << 1);
        shwRadix    = *(pshwFactors + shwDblCnt);                               /* 基数 */
        shwStageNum = *(pshwFactors + (shwDblCnt + 1));                         /* 基数为shwRadix的运算数目 */

        if (0 == swCnt)
        {
            shwNextStageNum = 1;
        }
        else
        {
            shwNextStageNum = *(pshwFactors + (shwDblCnt - 1));
        }

        shwStride  = 1;
        for (swCntM = 1; swCntM <= swCnt; swCntM++)
        {
            shwStride = (VOS_INT16)(shwStride * *(pshwFactors + (shwDblCnt - (swCntM * 2))));
        }
        shwStageLen = shwStride;

        pstOut= pstOutBeg;

        /* 根据基数选择对应的计算过程 */
        switch (shwRadix)
        {
            case MED_FFT_RADIX_TWO:
                MED_FFT_Bfly2(pstObj, pstOut, shwStride, shwStageNum, shwStageLen, shwNextStageNum);
                break;
            case MED_FFT_RADIX_FOUR:
                MED_FFT_Bfly4(pstObj, pstOut, shwStride, shwStageNum, shwStageLen, shwNextStageNum);
                break;
            case MED_FFT_RADIX_FIVE:
                for (swCntK = 0; swCntK < shwStageLen; swCntK++)
                {
                    pstOut= pstOutBeg + (swCntK * shwNextStageNum);
                    MED_FFT_Bfly5(pstObj, pstOut, shwStride, shwStageNum);
                }
                break;
            default:
                /* 不可能运行到这里 */
                break;
        }

    }
}
VOS_VOID MED_FFT_FftCore(MED_FFT_CORE_STRU *pstObj,VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    CODEC_OP_CMPX_STRU                *pstFreq = &(g_astBuf[0]);

    /* 以{实部 虚部}依次排列的方式, 把实序列构成复数序列进行复数FFT变换 */
    MED_FFT_Cmpx(pstObj, (CODEC_OP_CMPX_STRU *)pshwIn, pstFreq);

    /* 把输入的两个实数序列并行FFT结果转换为一个序列的FFT结果 */
    MED_FFT_UniteFreq(pstObj,
                      pstFreq,
                      (CODEC_OP_CMPX_STRU *)pshwOut);
}


VOS_VOID MED_FFT_IfftCore(MED_FFT_CORE_STRU *pstObj,  VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    CODEC_OP_CMPX_STRU                *pstFreq = &(g_astBuf[0]);

    /* 把输入的FFT结果转换为两个实数序列并行FFT的结果 */
    MED_FFT_SplitFreq(pstObj,
                      (CODEC_OP_CMPX_STRU *)pshwIn,
                      pstFreq);

    /* 以{实部 虚部}依次排列的方式, 把实序列构成复数序列进行复数IFFT变换 */
    MED_FFT_Cmpx(pstObj, pstFreq, (CODEC_OP_CMPX_STRU *)pshwOut);
}


VOS_VOID MED_FFT_Fft(VOS_INT16 shwFftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    MED_FFT_FftCore(&g_stMedFftObj[shwFftLenIndex], pshwIn, pshwOut);
}


VOS_VOID MED_FFT_Ifft(VOS_INT16 shwIfftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    MED_FFT_IfftCore(&g_stMedFftObj[shwIfftLenIndex], pshwIn, pshwOut);
}


VOS_INT16 MED_FFT_OpTriAdd(
                VOS_INT16               shwA,
                VOS_INT16               shwB,
                VOS_INT16               shwC)
{
    VOS_INT16       shwRet;
    shwRet          = CODEC_OpAdd(shwA, shwB);
    shwRet          = CODEC_OpAdd(shwRet, shwC);

    return          shwRet;
}


VOS_VOID MED_FFT_OpFixDiv(CODEC_OP_CMPX_STRU *pstCpx)
{
    pstCpx->shwR = CODEC_OpMult_r(pstCpx->shwR, MED_FFT_FIX_DIV5_PARAM);
    pstCpx->shwI = CODEC_OpMult_r(pstCpx->shwI, MED_FFT_FIX_DIV5_PARAM);
}


VOS_VOID MED_FFT_Bfly2(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstSn,
                VOS_INT16               shwStride,
                VOS_INT16               shwStageNum,
                VOS_INT16               shwStageLen,
                VOS_INT16               shwNextStageNum)
{
    VOS_INT16                           shwLenCnt;
    VOS_INT16                           shwNumCnt;
    CODEC_OP_CMPX_STRU                   *pstOutA             = pstSn;
    CODEC_OP_CMPX_STRU                   *pstOutB             = pstSn;
    CODEC_OP_CMPX_STRU                   *pstWn               = MED_NULL;
    CODEC_OP_CMPX_STRU                    stWnA;
    CODEC_OP_CMPX32_STRU                  stWnA32;
    CODEC_OP_CMPX32_STRU                  stTmp32;
    CODEC_OP_CMPX32_STRU                  stA32;

    if (VOS_TRUE == pstObj->shwIsInverted) /* IFFT运算 */
    {
        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstWn   = pstObj->pstTwiddles;
            pstOutA = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstOutB = pstOutA + shwStageNum;

            for(shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                /* 蝶形运算 Y(n) = A(n) +/- W*B(n) */
                stWnA = CODEC_OpCpxMulQ15(*pstOutB, *pstWn);
                CODEC_OpCpxSub(*pstOutB, *pstOutA, stWnA);
                CODEC_OpCpxAdd(*pstOutA, *pstOutA, stWnA);

                /* 更新下标 */
                pstOutA++;
                pstOutB++;
                pstWn += shwStride;
            }
        }
    }
    else /* FFT运算 */
    {
        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstWn   = pstObj->pstTwiddles;
            pstOutA = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstOutB = pstOutA + shwStageNum;

            /* 与IFFT一致, 精度提高 */
            for (shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                /* 蝶形运算 Y(n) = A(n) +/- W*B(n) */
                CODEC_OpCpxMul(stWnA32, *pstOutB, *pstWn);
                stWnA32.swR = stWnA32.swR >> 1;
                stWnA32.swI = stWnA32.swI >> 1;

                CODEC_OpCpx16To32(stA32, *pstOutA);
                CODEC_OpCpxShl32(stA32, MED_FFT_Q14);

                CODEC_OpCpxSub32(stTmp32, stA32, stWnA32);
                CODEC_OpCpxShr_r32(stTmp32, MED_FFT_Q15);
                CODEC_OpCpx32To16(*pstOutB, stTmp32);

                CODEC_OpCpxAdd32(stTmp32, stA32, stWnA32);
                CODEC_OpCpxShr_r32(stTmp32, MED_FFT_Q15);
                CODEC_OpCpx32To16(*pstOutA, stTmp32);

                /* 更新下标 */
                pstOutA++;
                pstOutB++;
                pstWn += shwStride;

            } /* end of for (shwNumCnt) */

        } /* end of for (shwLenCnt) */

    } /* end of if (VOS_TRUE == pstObj->shwIsInverted) */

}


VOS_VOID MED_FFT_Bfly4(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstSn,
                VOS_INT16               shwStride,
                VOS_INT16               shwStageNum,
                VOS_INT16               shwStageLen,
                VOS_INT16               shwNextStageNum)
{

    CODEC_OP_CMPX_STRU                   *pstWn1;
    CODEC_OP_CMPX_STRU                   *pstWn2;
    CODEC_OP_CMPX_STRU                   *pstWn3;
    CODEC_OP_CMPX_STRU                    astScratch[6];

    CODEC_OP_CMPX_STRU                   *pstOut;
    VOS_INT16                           shwNum1;
    VOS_INT16                           shwNum2;
    VOS_INT16                           shwNum3;
    VOS_INT16                           shwStride2;
    VOS_INT16                           shwStride3;
    VOS_INT16                           shwLenCnt;
    VOS_INT16                           shwNumCnt;

    shwNum1     = shwStageNum;
    shwNum2     = CODEC_OpShl(shwStageNum, 1);
    shwNum3     = (VOS_INT16)CODEC_OpL_mult0(shwStageNum, 3);

    shwStride2  = CODEC_OpShl(shwStride, 1);
    shwStride3  = (VOS_INT16)CODEC_OpL_mult0(shwStride, 3);

    if (VOS_TRUE == pstObj->shwIsInverted)                                      /* IFFT运算 */
    {
        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstOut = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstWn1 = pstObj->pstTwiddles;
            pstWn2 = pstWn1;
            pstWn3 = pstWn1;

            for (shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                astScratch[0]   = CODEC_OpCpxMulQ15(pstOut[shwNum1], *pstWn1);
                astScratch[1]   = CODEC_OpCpxMulQ15(pstOut[shwNum2], *pstWn2);
                astScratch[2]   = CODEC_OpCpxMulQ15(pstOut[shwNum3], *pstWn3);

                CODEC_OpCpxSub(astScratch[5], pstOut[0], astScratch[1]);
                CODEC_OpCpxAdd(pstOut[0], pstOut[0], astScratch[1]);

                CODEC_OpCpxAdd(astScratch[3], astScratch[0], astScratch[2]);
                CODEC_OpCpxSub(astScratch[4], astScratch[0], astScratch[2]);
                CODEC_OpCpxSub(pstOut[shwNum2], pstOut[0], astScratch[3]);
                CODEC_OpCpxAdd(pstOut[0], pstOut[0], astScratch[3]);

                pstOut[shwNum1].shwR = CODEC_OpSub(astScratch[5].shwR, astScratch[4].shwI);
                pstOut[shwNum1].shwI = CODEC_OpAdd(astScratch[5].shwI, astScratch[4].shwR);
                pstOut[shwNum3].shwR = CODEC_OpAdd(astScratch[5].shwR, astScratch[4].shwI);
                pstOut[shwNum3].shwI = CODEC_OpSub(astScratch[5].shwI, astScratch[4].shwR);

                pstWn1 += shwStride;
                pstWn2 += shwStride2;
                pstWn3 += shwStride3;
                pstOut++;
            }
        }
    }
    else                                                                        /* FFT运算 */
    {

        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstOut = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstWn1 = pstObj->pstTwiddles;
            pstWn2 = pstWn1;
            pstWn3 = pstWn1;

            for (shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                astScratch[0] = CODEC_OpCpxMulQ17(pstOut[shwNum1], *pstWn1);
                astScratch[1] = CODEC_OpCpxMulQ17(pstOut[shwNum2], *pstWn2);
                astScratch[2] = CODEC_OpCpxMulQ17(pstOut[shwNum3], *pstWn3);

                CODEC_OpCpxShr_r16(pstOut[0], 2);
                CODEC_OpCpxSub(astScratch[5], pstOut[0], astScratch[1]);
                CODEC_OpCpxAdd(pstOut[0], pstOut[0], astScratch[1]);

                CODEC_OpCpxAdd(astScratch[3], astScratch[0], astScratch[2]);
                CODEC_OpCpxSub(astScratch[4], astScratch[0], astScratch[2]);

                CODEC_OpCpxShr_r16(pstOut[shwNum2], 2);
                CODEC_OpCpxSub(pstOut[shwNum2], pstOut[0], astScratch[3]);
                CODEC_OpCpxAdd(pstOut[0], pstOut[0], astScratch[3]);

                pstOut[shwNum1].shwR = CODEC_OpAdd(astScratch[5].shwR, astScratch[4].shwI);
                pstOut[shwNum1].shwI = CODEC_OpSub(astScratch[5].shwI, astScratch[4].shwR);
                pstOut[shwNum3].shwR = CODEC_OpSub(astScratch[5].shwR, astScratch[4].shwI);
                pstOut[shwNum3].shwI = CODEC_OpAdd(astScratch[5].shwI, astScratch[4].shwR);

                pstWn1 += shwStride;
                pstWn2 += shwStride2;
                pstWn3 += shwStride3;
                pstOut++;

            }

        }

    }

}


VOS_VOID MED_FFT_Bfly5(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstSn,
                VOS_INT16               shwStride,
                VOS_INT16               shwStageNum)
{
    CODEC_OP_CMPX_STRU*  pstFout0;
    CODEC_OP_CMPX_STRU*  pstFout1;
    CODEC_OP_CMPX_STRU*  pstFout2;
    CODEC_OP_CMPX_STRU*  pstFout3;
    CODEC_OP_CMPX_STRU*  pstFout4;
    VOS_INT32          swCnt;
    CODEC_OP_CMPX_STRU   astScratch[13];
    CODEC_OP_CMPX_STRU*  pstTwiddles = pstObj->pstTwiddles;
    CODEC_OP_CMPX_STRU*  pstTw;
    CODEC_OP_CMPX_STRU   stYa  = pstTwiddles[shwStride * shwStageNum];
    CODEC_OP_CMPX_STRU   stYb  = pstTwiddles[shwStride * 2 * shwStageNum];

    pstFout0                 = pstSn;
    pstFout1                 = pstSn + shwStageNum;
    pstFout2                 = pstSn + (2 * shwStageNum);
    pstFout3                 = pstSn + (3 * shwStageNum);
    pstFout4                 = pstSn + (4 * shwStageNum);
    pstTw                    = pstObj->pstTwiddles;

    for (swCnt = 0; swCnt < shwStageNum; swCnt++)
    {
        if (VOS_FALSE == pstObj->shwIsInverted)
        {
            MED_FFT_OpFixDiv(pstFout0);
            MED_FFT_OpFixDiv(pstFout1);
            MED_FFT_OpFixDiv(pstFout2);
            MED_FFT_OpFixDiv(pstFout3);
            MED_FFT_OpFixDiv(pstFout4);
        }

        astScratch[0]        = *pstFout0;
        astScratch[1]        = CODEC_OpCpxMulQ15(*pstFout1, pstTw[swCnt * shwStride]);
        astScratch[2]        = CODEC_OpCpxMulQ15(*pstFout2, pstTw[2 * swCnt * shwStride]);
        astScratch[3]        = CODEC_OpCpxMulQ15(*pstFout3, pstTw[3 * swCnt * shwStride]);
        astScratch[4]        = CODEC_OpCpxMulQ15(*pstFout4, pstTw[4 * swCnt * shwStride]);
        CODEC_OpCpxAdd( astScratch[7], astScratch[1], astScratch[4]);
        CODEC_OpCpxSub( astScratch[10],astScratch[1], astScratch[4]);
        CODEC_OpCpxAdd( astScratch[8], astScratch[2], astScratch[3]);
        CODEC_OpCpxSub( astScratch[9], astScratch[2], astScratch[3]);

        pstFout0->shwR      = MED_FFT_OpTriAdd(pstFout0->shwR,
                                               astScratch[7].shwR,
                                               astScratch[8].shwR);
        pstFout0->shwI      = MED_FFT_OpTriAdd(pstFout0->shwI,
                                               astScratch[7].shwI,
                                               astScratch[8].shwI);
        astScratch[5].shwR  = MED_FFT_OpTriAdd(astScratch[0].shwR,
                                               CODEC_OpMult_r(astScratch[7].shwR, stYa.shwR),
                                               CODEC_OpMult_r(astScratch[8].shwR, stYb.shwR));
        astScratch[5].shwI  = MED_FFT_OpTriAdd(astScratch[0].shwI,
                                               CODEC_OpMult_r(astScratch[7].shwI, stYa.shwR),
                                               CODEC_OpMult_r(astScratch[8].shwI, stYb.shwR));
        astScratch[6].shwR  = CODEC_OpAdd(CODEC_OpMult_r(astScratch[10].shwI, stYa.shwI),
                                        CODEC_OpMult_r(astScratch[9].shwI,stYb.shwI));
        astScratch[6].shwI  = CODEC_OpNegate(CODEC_OpAdd(CODEC_OpMult_r(astScratch[10].shwR, stYa.shwI),
                                                     CODEC_OpMult_r(astScratch[9].shwR, stYb.shwI)));

        CODEC_OpCpxSub(*pstFout1, astScratch[5], astScratch[6]);
        CODEC_OpCpxAdd(*pstFout4, astScratch[5], astScratch[6]);

        astScratch[11].shwR = MED_FFT_OpTriAdd(astScratch[0].shwR,
                                               CODEC_OpMult_r(astScratch[7].shwR,stYb.shwR),
                                               CODEC_OpMult_r(astScratch[8].shwR,stYa.shwR));
        astScratch[11].shwI = MED_FFT_OpTriAdd(astScratch[0].shwI,
                                               CODEC_OpMult_r(astScratch[7].shwI,stYb.shwR),
                                               CODEC_OpMult_r(astScratch[8].shwI,stYa.shwR));
        astScratch[12].shwR = CODEC_OpSub(CODEC_OpMult_r(astScratch[9].shwI,stYa.shwI),
                                        CODEC_OpMult_r(astScratch[10].shwI,stYb.shwI));
        astScratch[12].shwI = CODEC_OpSub(CODEC_OpMult_r(astScratch[10].shwR,stYb.shwI),
                                        CODEC_OpMult_r(astScratch[9].shwR,stYa.shwI));

        CODEC_OpCpxAdd(*pstFout2, astScratch[11], astScratch[12]);
        CODEC_OpCpxSub(*pstFout3, astScratch[11], astScratch[12]);

        pstFout0++;
        pstFout1++;
        pstFout2++;
        pstFout3++;
        pstFout4++;
    }

}


VOS_VOID MED_FFT_UniteFreq(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstIn,
                CODEC_OP_CMPX_STRU       *pstOut)
{
    VOS_INT32                           swCnt;
    VOS_INT16                           shwLen;                                 /* 复数FFT阶数 */
    VOS_INT16                           shwHalfLen;                             /* 折半阶数    */
    VOS_INT32                           swEvenR;
    VOS_INT32                           swEvenI;
    VOS_INT32                           swWnR;                                  /* 临时实部 */
    VOS_INT32                           swWnI;                                  /* 临时虚部 */
    CODEC_OP_CMPX_STRU                    stDc;
    CODEC_OP_CMPX_STRU                   *pstOutDc;                               /* 输出的直流分量的位置 */
    CODEC_OP_CMPX_STRU                    stOdd;                                  /* 奇数元素 */
    CODEC_OP_CMPX_STRU                   *pstStart;                               /* 临时正向指针 */
    CODEC_OP_CMPX_STRU                   *pstEnd;                                 /* 临时反向指针 */
    CODEC_OP_CMPX_STRU                   *pstSupTwiddle;
    CODEC_OP_CMPX_STRU                    stStartTmp;                             /* 临时正向指针指向的数据 */
    CODEC_OP_CMPX_STRU                    stEndTmp;                               /* 临时反向指针指向的数据 */
    CODEC_OP_CMPX_STRU                    stSupTwiddle;

    shwLen                  = pstObj->shwN;
    pstOutDc                = pstOut;
    pstOut                  = pstOut + 1;

    /* pstIn[0]为直流分量, 实部为实序列偶数序列之和, 虚部为实序列奇数序列之和 */
    stDc                    = pstIn[0];
    stDc.shwR               = stDc.shwR >> 1;
    stDc.shwI               = stDc.shwI >> 1;

    pstOutDc->shwR          = stDc.shwR + stDc.shwI;
    pstOutDc->shwI          = stDc.shwR - stDc.shwI;

    pstStart                = &pstIn[1];
    pstEnd                  = &pstIn[shwLen - 1];
    pstSupTwiddle           = &(pstObj->pstSuperTwiddles[1]);

    shwHalfLen              = shwLen >> 1;
    for (swCnt = 1; swCnt <= shwHalfLen; swCnt++)
    {
        stStartTmp          = *pstStart;
        stEndTmp            = *pstEnd;
        stSupTwiddle        = *pstSupTwiddle;
        pstStart++;
        pstEnd--;
        pstSupTwiddle++;

        /* 偶数序列FFT结果, Q14 */
        swEvenR = (VOS_INT32)CODEC_OpAdd(stStartTmp.shwR, stEndTmp.shwR);
        swEvenI = (VOS_INT32)CODEC_OpSub(stStartTmp.shwI, stEndTmp.shwI);
        swEvenR = CODEC_OpL_shl(swEvenR, MED_FFT_Q13);
        swEvenI = CODEC_OpL_shl(swEvenI, MED_FFT_Q13);

        /* 奇数序列FFT结果, Q0 */
        stOdd.shwR = CODEC_OpShr(CODEC_OpSub(stStartTmp.shwR, stEndTmp.shwR), 1);
        stOdd.shwI = CODEC_OpShr_r(CODEC_OpAdd(stStartTmp.shwI, stEndTmp.shwI), 1);

        /* Wn*Xodd, Q14 */
        swWnR = CODEC_OpL_sub(CODEC_OpL_mult0(stOdd.shwR,stSupTwiddle.shwR),
                            CODEC_OpL_mult0(stOdd.shwI,stSupTwiddle.shwI));
        swWnI = CODEC_OpL_add(CODEC_OpL_mult0(stOdd.shwI,stSupTwiddle.shwR),
                            CODEC_OpL_mult0(stOdd.shwR,stSupTwiddle.shwI));

        swWnR = CODEC_OpL_shr(swWnR, 1);
        swWnI = CODEC_OpL_shr(swWnI, 1);

        /* Y = (Xeven + Wn*Xodd)/2, Q0 */

        pstOut[swCnt - 1].shwR
            = (VOS_INT16)CODEC_OpL_shr_r(swEvenR + swWnR, MED_FFT_Q15);

        pstOut[swCnt - 1].shwI
            = (VOS_INT16)CODEC_OpL_shr_r(swEvenI + swWnI, MED_FFT_Q15);

        pstOut[(shwLen - swCnt) - 1].shwR
            = (VOS_INT16)CODEC_OpL_shr_r(swEvenR - swWnR, MED_FFT_Q15);

        pstOut[(shwLen - swCnt) - 1].shwI
            = (VOS_INT16)CODEC_OpL_shr_r(swWnI - swEvenI, MED_FFT_Q15);

    }

}
VOS_VOID MED_FFT_SplitFreq(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstIn,
                CODEC_OP_CMPX_STRU       *pstOut)
{
    VOS_INT32                           swCnt;
    VOS_INT16                           shwLen;                                 /* 复数FFT阶数 */
    VOS_INT16                           shwHalfLen;                             /* 折半阶数 */
    CODEC_OP_CMPX_STRU                    stOdd;                                  /* 奇数元素 */
    CODEC_OP_CMPX_STRU                    stXk;
    CODEC_OP_CMPX_STRU                    stXkc;
    CODEC_OP_CMPX_STRU                    stEven;                                 /* 偶数元素 */
    CODEC_OP_CMPX_STRU                   *pstStart;                               /* 临时正向指针 */
    CODEC_OP_CMPX_STRU                   *pstEnd;                                 /* 临时反向指针 */
    CODEC_OP_CMPX_STRU                   *pstInDc;                                /* 输入的直流分量的位置 */

    pstInDc         = pstIn;
    pstIn           = pstIn + 1;

    shwLen          = pstObj->shwN;
    pstOut[0].shwR  = pstInDc->shwR + pstInDc->shwI;
    pstOut[0].shwI  = pstInDc->shwR - pstInDc->shwI;

    pstStart        = &pstIn[0];
    pstEnd          = &pstIn[shwLen - 2];

    shwHalfLen      = shwLen >> 1;
    for (swCnt = 1; swCnt <= shwHalfLen; swCnt++)
    {
        stXk        = *pstStart++;
        stXkc       = *pstEnd--;
        stXkc.shwI  = CODEC_OpNegate(stXkc.shwI);

        CODEC_OpCpxAdd(stEven, stXk, stXkc);
        CODEC_OpCpxSub(stXk, stXk, stXkc);
        stOdd       = CODEC_OpCpxMulQ15(stXk, pstObj->pstSuperTwiddles[swCnt]);

        CODEC_OpCpxAdd(pstOut[swCnt], stEven, stOdd);
        CODEC_OpCpxSub(pstOut[shwLen - swCnt], stEven, stOdd);
        pstOut[shwLen - swCnt].shwI = CODEC_OpNegate(pstOut[shwLen - swCnt].shwI);
    }

}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




