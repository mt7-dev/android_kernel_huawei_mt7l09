

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_fft.h"
#include "codec_op_lib.h"
#include "codec_op_cpx.h"
#include "ucom_mem_dyn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e50*/

#ifndef _MED_C89_
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
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft256SuperTwiddles[0]);

    /* 指定256阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[7];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_256_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft256SuperTwiddles[0]);

    /* 指定320阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[2];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_320_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft320SuperTwiddles[0]);

    /* 指定320阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[8];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_320_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft320SuperTwiddles[0]);

    /* 指定512阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[3];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_512_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft512SuperTwiddles[0]);

    /* 指定512阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[9];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_512_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedIfft512SuperTwiddles[0]);

    /* 指定640阶FFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[4];
    pstCore->shwIsInverted  = VOS_FALSE;
    pstCore->shwN           = MED_FFT_640_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
    pstCore->pstSuperTwiddles = (CODEC_OP_CMPX_STRU*)(&g_auhwMedFft640SuperTwiddles[0]);

    /* 指定640阶IFFT实体的旋转因子，复数FFT阶数、逆变换的标志 */
    pstCore                 = &g_stMedFftObj[10];
    pstCore->shwIsInverted  = VOS_TRUE;
    pstCore->shwN           = MED_FFT_640_CMPX_LEN;
    pstCore->shwNumOfRadix  = 0;
    pstCore->pshwFactors    = MED_NULL;
    pstCore->pshwSortTable  = (VOS_INT16*)&g_shwDigperm[0];
    pstCore->pstTwiddles    = MED_NULL;
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

    VOS_INT16                         i;
    VOS_INT16                *pshwBufIn;
    CODEC_OP_CMPX_STRU         *pstFreq;
    VOS_UINT32                uswMemSize;
    VOS_INT16                 shwFftLen;

    shwFftLen   = pstObj->shwN << 1;
    uswMemSize  = (VOS_UINT16)shwFftLen * sizeof(VOS_INT16);
    pshwBufIn   = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    pstFreq     = (CODEC_OP_CMPX_STRU*)UCOM_MemAlloc(uswMemSize);
    UCOM_MemCpy(pshwBufIn, pshwIn, uswMemSize);

    /* 如果为320、640阶实数FFT，采用优化后的 */
    if (   (MED_FFT_320_REAL_LEN == shwFftLen)
        || (MED_FFT_640_REAL_LEN == shwFftLen) )
    {
        MED_FFT_cfft_dit_160_320(pstObj, pshwBufIn, (VOS_INT16 *)pstFreq);
    }
    /* 如果为256、512阶实数FFT，采用优化后的 */
    else if (  (MED_FFT_256_REAL_LEN == shwFftLen)
            || (MED_FFT_512_REAL_LEN == shwFftLen) )
    {
        MED_FFT_cfft_dif_128_256(pstObj, pshwBufIn, (VOS_INT16 *)pstFreq);
    }
    else
    {
        /* 以{实部 虚部}依次排列的方式, 把实序列构成复数序列进行复数FFT变换 */
        MED_FFT_Cmpx(pstObj, (CODEC_OP_CMPX_STRU *)pshwBufIn, pstFreq);
    }

    /* 把输入的两个实数序列并行FFT结果转换为一个序列的FFT结果 */
    MED_FFT_UniteFreq(pstObj,
                      pstFreq,
                      (CODEC_OP_CMPX_STRU *)pshwOut);

    UCOM_MemFree(pshwBufIn);
    UCOM_MemFree(pstFreq);

}


VOS_VOID MED_FFT_IfftCore(MED_FFT_CORE_STRU *pstObj,  VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    CODEC_OP_CMPX_STRU                *pstFreq = &(g_astBuf[0]);

    /* 把输入的FFT结果转换为两个实数序列并行FFT的结果 */
    MED_FFT_SplitFreq(pstObj,
                      (CODEC_OP_CMPX_STRU *)pshwIn,
                      pstFreq);

    /* 如果为复数320、640阶逆FFT，采用优化后的 */
    if (   (&g_stMedFftObj[MED_IFFT_NUM_320] == pstObj)
        || (&g_stMedFftObj[MED_IFFT_NUM_640] == pstObj))
    {
        MED_FFT_cifft_dit_160_320(pstObj, (VOS_INT16 *)pstFreq, pshwOut);
    }
    /* 如果为复数256、512阶逆FFT，采用优化后的 */
    else if (   (&g_stMedFftObj[MED_IFFT_NUM_256] == pstObj)
             || (&g_stMedFftObj[MED_IFFT_NUM_512] == pstObj))
    {
        MED_FFT_cifft_dif_128_256(pstObj, (VOS_INT16 *)pstFreq, pshwOut);
    }
    else
    {
        /* 以{实部 虚部}依次排列的方式, 把实序列构成复数序列进行复数IFFT变换 */
        MED_FFT_Cmpx(pstObj, pstFreq, (CODEC_OP_CMPX_STRU *)pshwOut);
    }

}


VOS_VOID MED_FFT_Fft(VOS_INT16 shwFftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    MED_FFT_FftCore(&g_stMedFftObj[shwFftLenIndex], pshwIn, pshwOut);
}


VOS_VOID MED_FFT_Ifft(VOS_INT16 shwIfftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut)
{
    MED_FFT_IfftCore(&g_stMedFftObj[shwIfftLenIndex], pshwIn, pshwOut);
}


static inline VOS_VOID MED_FFT_OpFixDiv(CODEC_OP_CMPX_STRU *pstCpx)
{
    ae_p24x2s  aepCpx, aepConst;
    ae_q56s    aeqR, aeqI;

    VOS_INT16  shwConst = MED_FFT_FIX_DIV5_PARAM;

    aepCpx   = *((ae_p16x2s *)pstCpx);
    aepConst = AE_CVTP24A16(shwConst);

    /* pstCpx * MED_FFT_FIX_DIV5_PARAM */
    aeqR = AE_MULFS32P16S_HL(aepCpx, aepConst);
    aeqI = AE_MULFS32P16S_LL(aepCpx, aepConst);

    /* 进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1 */
    aepCpx = AE_TRUNCP24Q48X2(aeqR, aeqI);

    aepCpx = AE_ROUNDSP16ASYM(aepCpx);

    *((ae_p16x2s *)pstCpx) = aepCpx;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitBfly2Fft
 功能描述  : 运算单元: FFT2级蝶形运算单元运算
             A   |  A = A + BW
             B   |  B = A - BW
             A(n)>>1 +/- W*B(n)>>16
 输入参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB
             CODEC_OP_CMPX_STRU   *pstW
 输出参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月5日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitBfly2Fft(
                CODEC_OP_CMPX_STRU   *pstA,
                CODEC_OP_CMPX_STRU   *pstB,
                CODEC_OP_CMPX_STRU   *pstWn)
{
    ae_p24x2s  aepA, aepB, aepWn, aepWnB, aepAY, aepBY;
    ae_q56s    aeqR, aeqI;

    aepA = *((ae_p16x2s *)pstA);
    aepB = *((ae_p16x2s *)pstB);
    aepWn = *((ae_p16x2s *)pstWn);

    /* W*B(n) */
    aeqR = AE_MULZASP24S_HH_LL(aepWn, aepB);
    aeqI = AE_MULZAAP24S_HL_LH(aepWn, aepB);

    /* W*B(n)>>16 */
    aepWnB = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* A(n)>>1 */
    aepA = AE_SRAIP24(aepA, 1);

    /* A(n)>>1 +/- W*B(n)>>16 */
    aepAY = AE_ADDSP24S(aepA, aepWnB);
    aepBY = AE_SUBSP24S(aepA, aepWnB);

    aepAY = AE_ROUNDSP16ASYM(aepAY);
    aepBY = AE_ROUNDSP16ASYM(aepBY);

    *((ae_p16x2s *)pstA) = aepAY;
    *((ae_p16x2s *)pstB) = aepBY;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitBfly2Ifft
 功能描述  : 运算单元: IFFT2级蝶形运算单元运算
             A   |  A = A + BW
             B   |  B = A - BW
             A(n) +/- W*B(n)>>15
 输入参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB
             CODEC_OP_CMPX_STRU   *pstW
 输出参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月5日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitBfly2Ifft(
                CODEC_OP_CMPX_STRU   *pstA,
                CODEC_OP_CMPX_STRU   *pstB,
                CODEC_OP_CMPX_STRU   *pstWn)
{
    ae_p24x2s  aepA, aepB, aepWn, aepWnB, aepAY, aepBY;
    ae_q56s    aeqR, aeqI;

    aepA = *((ae_p16x2s *)pstA);
    aepB = *((ae_p16x2s *)pstB);
    aepWn = *((ae_p16x2s *)pstWn);

    /* W*B(n)>>15 */
    aeqR = AE_MULZASFP24S_HH_LL(aepWn, aepB);
    aeqI = AE_MULZAAFP24S_HL_LH(aepWn, aepB);

    aeqR   = AE_SATQ48S(aeqR);
    aeqI   = AE_SATQ48S(aeqI);

    aepWnB = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* A(n) +/- W*B(n)>>15 */
    aepAY = AE_ADDSP24S(aepA, aepWnB);
    aepBY = AE_SUBSP24S(aepA, aepWnB);

    aepAY = AE_ROUNDSP16ASYM(aepAY);
    aepBY = AE_ROUNDSP16ASYM(aepBY);

    *((ae_p16x2s *)pstA) = aepAY;
    *((ae_p16x2s *)pstB) = aepBY;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitBfly4Fft
 功能描述  : 运算单元: 4级蝶形运算单元运算
             A     |    A=A+B2W2+B1W1+B3W3
             B1    |    B1.R=(A-B2W2).R+(B1W1-B3W3).I; B1.I=(A-B2W2).I-(B1W1-B3W3).R
             B2    |    B2=A+B2W2-B1W1-B3W3
             B3    |    B3.R=(A-B2W2).R-(B1W1-B3W3).I; B3.I=(A-B2W2).I+(B1W1-B3W3).R
             Y = A>>2 +\- B2W2>>17 +\- B1W1>>17 +\- B3W3>>17
 输入参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
             CODEC_OP_CMPX_STRU   *pstWn1
             CODEC_OP_CMPX_STRU   *pstWn2
             CODEC_OP_CMPX_STRU   *pstWn3
 输出参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月5日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitBfly4Fft(
                CODEC_OP_CMPX_STRU   *pstA,
                CODEC_OP_CMPX_STRU   *pstB1,
                CODEC_OP_CMPX_STRU   *pstB2,
                CODEC_OP_CMPX_STRU   *pstB3,
                CODEC_OP_CMPX_STRU   *pstWn1,
                CODEC_OP_CMPX_STRU   *pstWn2,
                CODEC_OP_CMPX_STRU   *pstWn3)
{
    ae_p24x2s  aepA, aepB1, aepWn1, aepB2, aepWn2, aepB3, aepWn3, aepY;
    ae_q56s    aeqR, aeqI;

    aepA  = *((ae_p16x2s *)pstA);
    aepB1 = *((ae_p16x2s *)pstB1);
    aepB2 = *((ae_p16x2s *)pstB2);
    aepB3 = *((ae_p16x2s *)pstB3);

    aepWn1 = *((ae_p16x2s *)pstWn1);
    aepWn2 = *((ae_p16x2s *)pstWn2);
    aepWn3 = *((ae_p16x2s *)pstWn3);

    /* B1*W1-->aepWn1 */
    aeqR   = AE_MULZASP24S_HH_LL(aepWn1, aepB1);
    aeqI   = AE_MULZAAP24S_HL_LH(aepWn1, aepB1);
    aepWn1 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* B2*W2-->aepWn2 */
    aeqR   = AE_MULZASP24S_HH_LL(aepWn2, aepB2);
    aeqI   = AE_MULZAAP24S_HL_LH(aepWn2, aepB2);
    aepWn2 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* B3*W3-->aepWn3 */
    aeqR   = AE_MULZASP24S_HH_LL(aepWn3, aepB3);
    aeqI   = AE_MULZAAP24S_HL_LH(aepWn3, aepB3);
    aepWn3 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* A>>2(因24bit，精度不损失)，避免溢处 */
    aepA = AE_SRAIP24(aepA, 2);

    /* B1W1>>1(因24bit，精度不损失)，避免溢处；B1*W1实则右移1位，此处累加共右移2位 */
    aepWn1 = AE_SRAIP24(aepWn1, 1);

    /* B2W2>>1(因24bit，精度不损失)，避免溢处；B2*W2实则右移1位，此处累加共右移2位 */
    aepWn2 = AE_SRAIP24(aepWn2, 1);

    /* B3W3>>1(因24bit，精度不损失)，避免溢处；B3*W3实则右移1位，此处累加共右移2位 */
    aepWn3 = AE_SRAIP24(aepWn3, 1);

    /* (A+B2W2)-->aepB2 + (B1W1+B3W3)-->aepB1    -->aepY   -->pstA */
    aepB2 = AE_ADDSP24S(aepA, aepWn2);
    aepB1 = AE_ADDSP24S(aepWn1, aepWn3);
    aepY  = AE_ADDSP24S(aepB2, aepB1);
    aepY  = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstA) = aepY;

    /* (A+B2W2)-->aepB2 - (B1W1+B3W3)-->aepB1    -->aepY   -->pstB2 */
    aepY  = AE_SUBSP24S(aepB2, aepB1);
    aepY  = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB2) = aepY;

    /* B1.R=(A-B2W2).R+(B1W1-B3W3).I; B1.I=(A-B2W2).I-(B1W1-B3W3).R *
     * B3.R=(A-B2W2).R-(B1W1-B3W3).I; B3.I=(A-B2W2).I+(B1W1-B3W3).R */
    /* (A-B2W2).R-->aepB2.H (A-B2W2).I-->aepB2.L */
    aepB2 = AE_SUBSP24S(aepA, aepWn2);

    /* (B1W1-B3W3).I-->aepB3.H (B1W1-B3W3).R-->aepB3.L */
    aepB1 = AE_SUBSP24S(aepWn1, aepWn3);
    aepB3 = AE_SELP24_LH(aepB1, aepB1);

    /* B1.R-->aepWn1.H B3.I-->aepWn1.L; B3.R-->aepWn3.H B1.I-->aepWn3.L */
    aepWn1 = AE_ADDSP24S(aepB2, aepB3);
    aepWn3 = AE_SUBSP24S(aepB2, aepB3);

    /* B1.R-->aepY.H B1.I-->aepY.L */
    aepY = AE_SELP24_HL(aepWn1, aepWn3);
    aepY = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB1) = aepY;

    /* B3.R-->aepY.H B3.I-->aepY.L */
    aepY = AE_SELP24_HL(aepWn3, aepWn1);
    aepY = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB3) = aepY;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitBfly4Ifft
 功能描述  : 运算单元: 4级蝶形运算单元运算
             A     |    A=A+B2W2+B1W1+B3W3
             B1    |    B1.R=(A-B2W2).R+(B1W1-B3W3).I; B1.I=(A-B2W2).I-(B1W1-B3W3).R
             B2    |    B2=A+B2W2-B1W1-B3W3
             B3    |    B3.R=(A-B2W2).R-(B1W1-B3W3).I; B3.I=(A-B2W2).I+(B1W1-B3W3).R
             Y = A +\- B2W2>>15 +\- B1W1>>15 +\- B3W3>>15
 输入参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
             CODEC_OP_CMPX_STRU   *pstWn1
             CODEC_OP_CMPX_STRU   *pstWn2
             CODEC_OP_CMPX_STRU   *pstWn3
 输出参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月5日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitBfly4Ifft(
                CODEC_OP_CMPX_STRU   *pstA,
                CODEC_OP_CMPX_STRU   *pstB1,
                CODEC_OP_CMPX_STRU   *pstB2,
                CODEC_OP_CMPX_STRU   *pstB3,
                CODEC_OP_CMPX_STRU   *pstWn1,
                CODEC_OP_CMPX_STRU   *pstWn2,
                CODEC_OP_CMPX_STRU   *pstWn3)
{
    ae_p24x2s  aepA, aepB1, aepWn1, aepB2, aepWn2, aepB3, aepWn3, aepY;
    ae_q56s    aeqR, aeqI;

    aepA  = *((ae_p16x2s *)pstA);
    aepB1 = *((ae_p16x2s *)pstB1);
    aepB2 = *((ae_p16x2s *)pstB2);
    aepB3 = *((ae_p16x2s *)pstB3);

    aepWn1 = *((ae_p16x2s *)pstWn1);
    aepWn2 = *((ae_p16x2s *)pstWn2);
    aepWn3 = *((ae_p16x2s *)pstWn3);

    /* B1*W1-->aepWn1 */
    aeqR   = AE_MULZASFP24S_HH_LL(aepWn1, aepB1);
    aeqI   = AE_MULZAAFP24S_HL_LH(aepWn1, aepB1);

    aeqR   = AE_SATQ48S(aeqR);
    aeqI   = AE_SATQ48S(aeqI);

    aepWn1 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* B2*W2-->aepWn2 */
    aeqR   = AE_MULZASFP24S_HH_LL(aepWn2, aepB2);
    aeqI   = AE_MULZAAFP24S_HL_LH(aepWn2, aepB2);

    aeqR   = AE_SATQ48S(aeqR);
    aeqI   = AE_SATQ48S(aeqI);

    aepWn2 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* B3*W3-->aepWn3 */
    aeqR   = AE_MULZASFP24S_HH_LL(aepWn3, aepB3);
    aeqI   = AE_MULZAAFP24S_HL_LH(aepWn3, aepB3);

    aeqR   = AE_SATQ48S(aeqR);
    aeqI   = AE_SATQ48S(aeqI);

    aepWn3 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* (A+B2W2)-->aepB2 + (B1W1+B3W3)-->aepB1    -->aepY   -->pstA */
    aepB2 = AE_ADDSP24S(aepA, aepWn2);
    aepB1 = AE_ADDSP24S(aepWn1, aepWn3);
    aepY  = AE_ADDSP24S(aepB2, aepB1);
    aepY  = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstA) = aepY;

    /* (A+B2W2)-->aepB2 - (B1W1+B3W3)-->aepB1    -->aepY   -->pstB2 */
    aepY  = AE_SUBSP24S(aepB2, aepB1);
    aepY  = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB2) = aepY;

    /* B1.R=(A-B2W2).R+(B1W1-B3W3).I; B1.I=(A-B2W2).I-(B1W1-B3W3).R *
     * B3.R=(A-B2W2).R-(B1W1-B3W3).I; B3.I=(A-B2W2).I+(B1W1-B3W3).R */
    /* (A-B2W2).R-->aepB2.H (A-B2W2).I-->aepB2.L */
    aepB2 = AE_SUBSP24S(aepA, aepWn2);

    /* (B1W1-B3W3).I-->aepB3.H (B1W1-B3W3).R-->aepB3.L */
    aepB1 = AE_SUBSP24S(aepWn1, aepWn3);
    aepB3 = AE_SELP24_LH(aepB1, aepB1);

    /* B1.R-->aepWn1.H B3.I-->aepWn1.L; B3.R-->aepWn3.H B1.I-->aepWn3.L */
    aepWn1 = AE_SUBSP24S(aepB2, aepB3);
    aepWn3 = AE_ADDSP24S(aepB2, aepB3);

    /* B1.R-->aepY.H B1.I-->aepY.L */
    aepY = AE_SELP24_HL(aepWn1, aepWn3);
    aepY = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB1) = aepY;

    /* B3.R-->aepY.H B3.I-->aepY.L */
    aepY = AE_SELP24_HL(aepWn3, aepWn1);
    aepY = AE_ROUNDSP16ASYM(aepY);
    *((ae_p16x2s *)pstB3) = aepY;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitBfly5
 功能描述  : 运算单元: 5级蝶形运算单元运算
             A     |    A=A+B2W2+B1W1+B3W3
             B1    |    B1.R=(A-B2W2).R+(B1W1-B3W3).I; B1.I=(A-B2W2).I-(B1W1-B3W3).R
             B2    |    B2=A+B2W2-B1W1-B3W3
             B3    |    B3.R=(A-B2W2).R-(B1W1-B3W3).I; B3.I=(A-B2W2).I+(B1W1-B3W3).R
             Y = A +\- B2W2>>15 +\- B1W1>>15 +\- B3W3>>15
 输入参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
             CODEC_OP_CMPX_STRU   *pstWn1
             CODEC_OP_CMPX_STRU   *pstWn2
             CODEC_OP_CMPX_STRU   *pstWn3
 输出参数  : CODEC_OP_CMPX_STRU   *pstA
             CODEC_OP_CMPX_STRU   *pstB1
             CODEC_OP_CMPX_STRU   *pstB2
             CODEC_OP_CMPX_STRU   *pstB3
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月5日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitBfly5(
                CODEC_OP_CMPX_STRU   *pstFout0,
                CODEC_OP_CMPX_STRU   *pstFout1,
                CODEC_OP_CMPX_STRU   *pstFout2,
                CODEC_OP_CMPX_STRU   *pstFout3,
                CODEC_OP_CMPX_STRU   *pstFout4,
                CODEC_OP_CMPX_STRU   *pstTw1,
                CODEC_OP_CMPX_STRU   *pstTw2,
                CODEC_OP_CMPX_STRU   *pstTw3,
                CODEC_OP_CMPX_STRU   *pstTw4,
                CODEC_OP_CMPX_STRU   *pstYa,
                CODEC_OP_CMPX_STRU   *pstYb)
{
    CODEC_OP_CMPX_STRU   stScratchTmp7, stScratchTmp8, stScratchTmp9, stScratchTmp10;
    ae_p24x2s  aepA1, aepA2, aepA3, aepA4, aepTmp1, aepTmp2, aepTmp3, aepTmp4;
    ae_q56s    aeqR, aeqI;

    /* astScratch[1]        = CODEC_OpCpxMulQ15(*pstFout1, *pstTw1); */
    aepTmp1 = *((ae_p16x2s *)pstFout1);
    aepTmp2 = *((ae_p16x2s *)pstTw1);
    aeqR    = AE_MULZASFP24S_HH_LL(aepTmp1, aepTmp2);
    aeqI    = AE_MULZAAFP24S_HL_LH(aepTmp1, aepTmp2);
    aepTmp3 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* astScratch[4]        = CODEC_OpCpxMulQ15(*pstFout4, *pstTw4); */
    aepTmp1 = *((ae_p16x2s *)pstFout4);
    aepTmp2 = *((ae_p16x2s *)pstTw4);
    aeqR    = AE_MULZASFP24S_HH_LL(aepTmp1, aepTmp2);
    aeqI    = AE_MULZAAFP24S_HL_LH(aepTmp1, aepTmp2);
    aepTmp4 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* CODEC_OpCpxAdd( astScratch[7], astScratch[1], astScratch[4]); */
    aepA1   = AE_ADDSP24S(aepTmp3, aepTmp4);
    /* CODEC_OpCpxSub( astScratch[10],astScratch[1], astScratch[4]); */
    aepA2   = AE_SUBSP24S(aepTmp3, aepTmp4);

    /* astScratch[2]        = CODEC_OpCpxMulQ15(*pstFout2, *pstTw2); */
    aepTmp1 = *((ae_p16x2s *)pstFout2);
    aepTmp2 = *((ae_p16x2s *)pstTw2);
    aeqR    = AE_MULZASFP24S_HH_LL(aepTmp1, aepTmp2);
    aeqI    = AE_MULZAAFP24S_HL_LH(aepTmp1, aepTmp2);
    aepTmp3 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* astScratch[3]        = CODEC_OpCpxMulQ15(*pstFout3, *pstTw3); */
    aepTmp1 = *((ae_p16x2s *)pstFout3);
    aepTmp2 = *((ae_p16x2s *)pstTw3);
    aeqR    = AE_MULZASFP24S_HH_LL(aepTmp1, aepTmp2);
    aeqI    = AE_MULZAAFP24S_HL_LH(aepTmp1, aepTmp2);
    aepTmp4 = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* CODEC_OpCpxAdd( astScratch[8], astScratch[2], astScratch[3]); */
    aepA3   = AE_ADDSP24S(aepTmp3, aepTmp4);
    /* CODEC_OpCpxSub( astScratch[9], astScratch[2], astScratch[3]); */
    aepA4   = AE_SUBSP24S(aepTmp3, aepTmp4);

    /* 保存astScratch[7] astScratch[8] astScratch[10] astScratch[9] */
    aepA1 = AE_ROUNDSP16ASYM(aepA1);
    aepA3 = AE_ROUNDSP16ASYM(aepA3);
    aepA2 = AE_ROUNDSP16ASYM(aepA2);
    aepA4 = AE_ROUNDSP16ASYM(aepA4);
    *((ae_p16x2s *)&stScratchTmp7)  = aepA1;
    *((ae_p16x2s *)&stScratchTmp8)  = aepA3;
    *((ae_p16x2s *)&stScratchTmp10) = aepA2;
    *((ae_p16x2s *)&stScratchTmp9)  = aepA4;

    /* astScratch[0]        = *pstFout0; */
    aepTmp1 = *((ae_p16x2s *)pstFout0);

    /*
    pstFout0->shwR      = MED_FFT_OpTriAdd(pstFout0->shwR,
                                           astScratch[7].shwR,
                                           astScratch[8].shwR);
    pstFout0->shwI      = MED_FFT_OpTriAdd(pstFout0->shwI,
                                           astScratch[7].shwI,
                                           astScratch[8].shwI);
    */
    aepTmp2 = AE_ADDSP24S(aepTmp1, aepA1);
    aepTmp2 = AE_ADDSP24S(aepTmp2, aepA3);

    /* 输出 pstFout0 */
    aepTmp2 = AE_ROUNDSP16ASYM(aepTmp2);
    *((ae_p16x2s *)pstFout0) = aepTmp2;

    /* sload pstYa pstYb */
    aepTmp3 = *((ae_p16x2s *)pstYa);
    aepTmp4 = *((ae_p16x2s *)pstYb);

    /*
    astScratch[5].shwR  = MED_FFT_OpTriAdd(astScratch[0].shwR,
                                           CODEC_OpMult_r(astScratch[7].shwR, pstYa->shwR),    A2 R
                                           CODEC_OpMult_r(astScratch[8].shwR, pstYb->shwR));   A4 R
    astScratch[5].shwI  = MED_FFT_OpTriAdd(astScratch[0].shwI,
                                           CODEC_OpMult_r(astScratch[7].shwI, pstYa->shwR),    A2 I
                                           CODEC_OpMult_r(astScratch[8].shwI, pstYb->shwR));   A4 I
    */
    aeqR    = AE_MULFS32P16S_HH(aepA1, aepTmp3);
    aeqI    = AE_MULFS32P16S_LH(aepA1, aepTmp3);
    aepA2   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aeqR    = AE_MULFS32P16S_HH(aepA3, aepTmp4);
    aeqI    = AE_MULFS32P16S_LH(aepA3, aepTmp4);
    aepA4   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aepTmp2 = AE_ADDSP24S(aepA2, aepA4);
    aepTmp2 = AE_ADDSP24S(aepTmp1, aepTmp2);

    /* load astScratch[10] astScratch[9] */
    aepA1   = *((ae_p16x2s *)&stScratchTmp10);
    aepA3   = *((ae_p16x2s *)&stScratchTmp9);

    /*
    astScratch[6].shwR  = CODEC_OpAdd(CODEC_OpMult_r(astScratch[10].shwI, pstYa->shwI),                 A2 R
                                    CODEC_OpMult_r(astScratch[9].shwI,pstYb->shwI));                  A4 R
    astScratch[6].shwI  = CODEC_OpNegate(CODEC_OpAdd(CODEC_OpMult_r(astScratch[10].shwR, pstYa->shwI),    A2 I
                                                 CODEC_OpMult_r(astScratch[9].shwR, pstYb->shwI)));   A4 I
    */
    aeqR    = AE_MULFS32P16S_LL(aepA1, aepTmp3);
    aeqI    = AE_MULFS32P16S_HL(aepA1, aepTmp3);
    aeqI    = AE_NEGSQ56S(aeqI);
    aepA2   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aeqR    = AE_MULFS32P16S_LL(aepA3, aepTmp4);
    aeqI    = AE_MULFS32P16S_HL(aepA3, aepTmp4);
    aeqI    = AE_NEGSQ56S(aeqI);
    aepA4   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aepA2   = AE_ADDSP24S(aepA2, aepA4);

    /* CODEC_OpCpxSub(*pstFout1, astScratch[5], astScratch[6]); */
    aepA4   = AE_SUBSP24S(aepTmp2, aepA2);
    /* CODEC_OpCpxAdd(*pstFout4, astScratch[5], astScratch[6]); */
    aepA2   = AE_ADDSP24S(aepTmp2, aepA2);

    //输出 pstFout1 pstFout4 */
    aepA4 = AE_ROUNDSP16ASYM(aepA4);
    aepA2 = AE_ROUNDSP16ASYM(aepA2);
    *((ae_p16x2s *)pstFout1) = aepA4;
    *((ae_p16x2s *)pstFout4) = aepA2;

    /*
    astScratch[12].shwR = CODEC_OpSub(CODEC_OpMult_r(astScratch[9].shwI,pstYa->shwI),     A2 R
                                    CODEC_OpMult_r(astScratch[10].shwI,pstYb->shwI));   A4 R
    astScratch[12].shwI = CODEC_OpSub(CODEC_OpMult_r(astScratch[10].shwR,pstYb->shwI),    A2 I
                                    CODEC_OpMult_r(astScratch[9].shwR,pstYa->shwI));    A4 I
    */
    aeqR    = AE_MULFS32P16S_LL(aepA3, aepTmp3);
    aeqI    = AE_MULFS32P16S_HL(aepA1, aepTmp4);
    aepA2   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aeqR    = AE_MULFS32P16S_LL(aepA1, aepTmp4);
    aeqI    = AE_MULFS32P16S_HL(aepA3, aepTmp3);
    aepA4   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aepTmp2 = AE_SUBSP24S(aepA2, aepA4);

    /* load astScratch[7] astScratch[8] */
    aepA1   = *((ae_p16x2s *)&stScratchTmp7);
    aepA3   = *((ae_p16x2s *)&stScratchTmp8);

    /*
    astScratch[11].shwR = MED_FFT_OpTriAdd(astScratch[0].shwR,
                                           CODEC_OpMult_r(astScratch[7].shwR,pstYb->shwR),    A2 R
                                           CODEC_OpMult_r(astScratch[8].shwR,pstYa->shwR));   A4 R
    astScratch[11].shwI = MED_FFT_OpTriAdd(astScratch[0].shwI,
                                           CODEC_OpMult_r(astScratch[7].shwI,pstYb->shwR),    A2 I
                                           CODEC_OpMult_r(astScratch[8].shwI,pstYa->shwR));   A4 I
    */
    aeqR    = AE_MULFS32P16S_HH(aepA1, aepTmp4);
    aeqI    = AE_MULFS32P16S_LH(aepA1, aepTmp4);
    aepA2   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aeqR    = AE_MULFS32P16S_HH(aepA3, aepTmp3);
    aeqI    = AE_MULFS32P16S_LH(aepA3, aepTmp3);
    aepA4   = AE_TRUNCP24Q48X2(aeqR, aeqI);
    aepA2   = AE_ADDSP24S(aepA2, aepA4);
    aepA2   = AE_ADDSP24S(aepA2, aepTmp1);

    /* CODEC_OpCpxAdd(*pstFout2, astScratch[11], astScratch[12]); */
    aepA4   = AE_ADDSP24S(aepA2, aepTmp2);
    /* CODEC_OpCpxSub(*pstFout3, astScratch[11], astScratch[12]); */
    aepA2   = AE_SUBSP24S(aepA2, aepTmp2);

    /* 输出 pstFout2 pstFout3 */
    aepA4 = AE_ROUNDSP16ASYM(aepA4);
    aepA2 = AE_ROUNDSP16ASYM(aepA2);
    *((ae_p16x2s *)pstFout2) = aepA4;
    *((ae_p16x2s *)pstFout3) = aepA2;

}

/*****************************************************************************
 函 数 名  : MED_PpUnitFreqUnite
 功能描述  : 运算单元: 将复数频谱转换为实数频谱
 输入参数  : CODEC_OP_CMPX_STRU               *pstSta
             CODEC_OP_CMPX_STRU               *pstEnd
             CODEC_OP_CMPX_STRU               *pstTwi
             CODEC_OP_CMPX_STRU               *pstOutF
             CODEC_OP_CMPX_STRU               *pstOutR
 输出参数  : CODEC_OP_CMPX_STRU               *pstOutF
             CODEC_OP_CMPX_STRU               *pstOutR
 返 回 值  : static inline VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月6日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitFreqUnite(
                CODEC_OP_CMPX_STRU               *pstSta,
                CODEC_OP_CMPX_STRU               *pstEnd,
                CODEC_OP_CMPX_STRU               *pstTwi,
                CODEC_OP_CMPX_STRU               *pstOutF,
                CODEC_OP_CMPX_STRU               *pstOutR)
{
    ae_p24x2s  aepSta, aepEnd, aepTwi, aepEvn, aepOdd, aepYe, aepYo, aepY;
    ae_q56s    aeqR, aeqI;

    aepSta  = *((ae_p16x2s *)pstSta);
    aepEnd  = *((ae_p16x2s *)pstEnd);
    aepTwi  = *((ae_p16x2s *)pstTwi);

    /* Evn((S.R+E.R)>>2, (S.I-E.I)>>2); Odd((S.R-E.R)>>1, (S.I+E.I)>>1) */
    aepYe  = AE_ADDSP24S(aepSta, aepEnd);
    aepYo  = AE_SUBSP24S(aepSta, aepEnd);

    aepEvn = AE_SELP24_HL(aepYe, aepYo);
    aepOdd = AE_SELP24_HL(aepYo, aepYe);

    aepEvn = AE_SRAIP24(aepEvn, 2);
    aepOdd = AE_SRAIP24(aepOdd, 1);

    /* Ye = Odd * Twi */
    aeqR   = AE_MULZASP24S_HH_LL(aepOdd, aepTwi);
    aeqI   = AE_MULZAAP24S_HL_LH(aepOdd, aepTwi);

    aepYe  = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* OutF = Evn + Odd * Twi */
    aepY   = AE_ADDSP24S(aepEvn, aepYe);
    aepY   = AE_ROUNDSP16ASYM(aepY);

    *((ae_p16x2s *)pstOutF) = aepY;

    /* OutR.R = Evn.R - (Odd * Twi).R; OutR.I = (Odd * Twi).I - Evn.I */
    aepYo  = AE_SUBSP24S(aepEvn, aepYe);
    aepYe  = AE_SUBSP24S(aepYe, aepEvn);
    aepY   = AE_SELP24_HL(aepYo, aepYe);
    aepY   = AE_ROUNDSP16ASYM(aepY);

    *((ae_p16x2s *)pstOutR) = aepY;
}

/*****************************************************************************
 函 数 名  : MED_PpUnitFreqSplit
 功能描述  : 运算单元: 将实数频谱转为复数频谱
 输入参数  : CODEC_OP_CMPX_STRU               *pstSta
             CODEC_OP_CMPX_STRU               *pstEnd
             CODEC_OP_CMPX_STRU               *pstTwi
             CODEC_OP_CMPX_STRU               *pstOutF
             CODEC_OP_CMPX_STRU               *pstOutR
 输出参数  : CODEC_OP_CMPX_STRU               *pstOutF
             CODEC_OP_CMPX_STRU               *pstOutR
 返 回 值  : static inline VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月6日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline VOS_VOID MED_PpUnitFreqSplit(
                CODEC_OP_CMPX_STRU               *pstSta,
                CODEC_OP_CMPX_STRU               *pstEnd,
                CODEC_OP_CMPX_STRU               *pstTwi,
                CODEC_OP_CMPX_STRU               *pstOutF,
                CODEC_OP_CMPX_STRU               *pstOutR)
{
    ae_p24x2s  aepSta, aepEnd, aepTwi, aepEvn, aepOdd, aepYe, aepYo, aepY;
    ae_q56s    aeqR, aeqI;

    aepSta  = *((ae_p16x2s *)pstSta);
    aepEnd  = *((ae_p16x2s *)pstEnd);
    aepTwi  = *((ae_p16x2s *)pstTwi);

    /* Evn.R=Sta.R+End.R, Evn.I=Sta.I-End.I */
    /* Odd.R=Sta.R-End.R, Odd.I=Sta.I+End.I */
    aepYe  = AE_ADDSP24S(aepSta, aepEnd);
    aepYo  = AE_SUBSP24S(aepSta, aepEnd);

    aepEvn = AE_SELP24_HL(aepYe, aepYo);
    aepOdd = AE_SELP24_HL(aepYo, aepYe);

    /* Odd=Odd*Twi */
    aeqR   = AE_MULZASFP24S_HH_LL(aepOdd, aepTwi);
    aeqI   = AE_MULZAAFP24S_HL_LH(aepOdd, aepTwi);

    aeqR   = AE_SATQ48S(aeqR);
    aeqI   = AE_SATQ48S(aeqI);

    aepOdd = AE_TRUNCP24Q48X2(aeqR, aeqI);

    /* OutF=Evn+Odd */
    aepY   = AE_ADDSP24S(aepEvn, aepOdd);
    aepY   = AE_ROUNDSP16ASYM(aepY);

    *((ae_p16x2s *)pstOutF) = aepY;

    /* OutR.R=Evn.R-Odd.R; OutR.I=Odd.I-Evn.I */
    aepYe  = AE_SUBSP24S(aepEvn, aepOdd);
    aepYo  = AE_SUBSP24S(aepOdd, aepEvn);
    aepY   = AE_SELP24_HL(aepYe, aepYo);
    aepY   = AE_ROUNDSP16ASYM(aepY);

    *((ae_p16x2s *)pstOutR) = aepY;

}


/*****************************************************************************
 函 数 名  : MED_FFT_Bfly2
 功能描述  : 基2 FFT/IFFT变换运算
 输入参数  : pstObj    -- 复数FFT/IFFT变换对象
             pstSn     -- 待变换序列, 长度为MED_AEC_FFT_CMPX_LEN
             shwStride -- 步长
             shwStageNum   -- 本次蝴形运算的数目
             shwStageLen   -- 本次蝴形运算长度
             shwNextStageNum  -- 下次运算的数目
 输出参数  : pstSn     -- 运算结果, 长度为MED_AEC_FFT_CMPX_LEN
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月17日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MED_FFT_Bfly2(
                MED_FFT_CORE_STRU   *pstObj,
                CODEC_OP_CMPX_STRU   *pstSn,
                VOS_INT16               shwStride,
                VOS_INT16               shwStageNum,
                VOS_INT16               shwStageLen,
                VOS_INT16               shwNextStageNum)
{
    VOS_INT16                           shwLenCnt;
    VOS_INT16                           shwNumCnt;
    CODEC_OP_CMPX_STRU               *pstOutA             = pstSn;
    CODEC_OP_CMPX_STRU               *pstOutB             = pstSn;
    CODEC_OP_CMPX_STRU               *pstWn               = MED_NULL;

    if (VOS_TRUE == pstObj->shwIsInverted) /* IFFT运算 */
    {
        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstWn   = pstObj->pstTwiddles;
            pstOutA = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstOutB = pstOutA + shwStageNum;

            for(shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                /* 2级蝶形运算单元运算 Y(n) = A(n) +/- W*B(n)>>15 */
                MED_PpUnitBfly2Ifft(pstOutA, pstOutB, pstWn);

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
                /* 2级蝶形运算单元运算 Y(n) = A(n)>>1 +/- W*B(n)>>16 */
                MED_PpUnitBfly2Fft(pstOutA, pstOutB, pstWn);

                /* 更新下标 */
                pstOutA++;
                pstOutB++;
                pstWn += shwStride;

            } /* end of for (shwNumCnt) */

        } /* end of for (shwLenCnt) */

    } /* end of if (VOS_TRUE == pstObj->shwIsInverted) */

}


/*****************************************************************************
 函 数 名  : MED_PpFftBfly4
 功能描述  : 基4 FFT/IFFT变换运算
 输入参数  : pstObj    -- 复数FFT/IFFT变换对象
             pstSn     -- 待变换序列, 长度为MED_AEC_FFT_CMPX_LEN
             shwStride -- 步长
             shwStageNum   -- 本次蝴形运算的数目
             shwStageLen   -- 本次蝴形运算长度
             shwNextStageNum  -- 下次运算的数目
 输出参数  : pstSn     -- 运算结果, 长度为MED_AEC_FFT_CMPX_LEN
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月17日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MED_FFT_Bfly4(
                MED_FFT_CORE_STRU   *pstObj,
                CODEC_OP_CMPX_STRU   *pstSn,
                VOS_INT16               shwStride,
                VOS_INT16               shwStageNum,
                VOS_INT16               shwStageLen,
                VOS_INT16               shwNextStageNum)
{

    CODEC_OP_CMPX_STRU               *pstWn1;
    CODEC_OP_CMPX_STRU               *pstWn2;
    CODEC_OP_CMPX_STRU               *pstWn3;

    CODEC_OP_CMPX_STRU               *pstOut;
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

    if (VOS_TRUE == pstObj->shwIsInverted) /* IFFT运算 */
    {
        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstOut = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstWn1 = pstObj->pstTwiddles;
            pstWn2 = pstWn1;
            pstWn3 = pstWn1;

            for (shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {
                /* 4级蝶形运算单元运算 Y = A + B2W2>>15 + B1W1>>15 + B3W3>>15 */
                MED_PpUnitBfly4Ifft(&pstOut[0],
                                    &pstOut[shwNum1],
                                    &pstOut[shwNum2],
                                    &pstOut[shwNum3],
                                     pstWn1,
                                     pstWn2,
                                     pstWn3);

                pstWn1 += shwStride;
                pstWn2 += shwStride2;
                pstWn3 += shwStride3;
                pstOut++;
            }
        }
    }
    else /* FFT运算 */
    {

        for (shwLenCnt = 0; shwLenCnt < shwStageLen; shwLenCnt++)
        {
            pstOut = pstSn + CODEC_OpL_mult0(shwLenCnt, shwNextStageNum);
            pstWn1 = pstObj->pstTwiddles;
            pstWn2 = pstWn1;
            pstWn3 = pstWn1;

            for (shwNumCnt = 0; shwNumCnt < shwStageNum; shwNumCnt++)
            {

                /* 4级蝶形运算单元运算 Y = A>>2 + B2W2>>17 + B1W1>>17 + B3W3>>17 */
                MED_PpUnitBfly4Fft(&pstOut[0],
                                   &pstOut[shwNum1],
                                   &pstOut[shwNum2],
                                   &pstOut[shwNum3],
                                    pstWn1,
                                    pstWn2,
                                    pstWn3);

                pstWn1 += shwStride;
                pstWn2 += shwStride2;
                pstWn3 += shwStride3;
                pstOut++;

            } /* end of for (shwNumCnt) */

        } /* end of for (shwLenCnt) */

    } /* end of if (VOS_TRUE == pstObj->shwIsInverted) */

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
    CODEC_OP_CMPX_STRU*  pstTwiddles = pstObj->pstTwiddles;
    CODEC_OP_CMPX_STRU*  pstTw;
    CODEC_OP_CMPX_STRU*  pstYa    = &pstTwiddles[shwStride * shwStageNum];
    CODEC_OP_CMPX_STRU*  pstYb    = &pstTwiddles[shwStride * 2 * shwStageNum];

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
        /* 5级蝶形运算单元运算 */
        MED_PpUnitBfly5(pstFout0,
                        pstFout1,
                        pstFout2,
                        pstFout3,
                        pstFout4,
                        &pstTw[swCnt * shwStride],
                        &pstTw[2 * swCnt * shwStride],
                        &pstTw[3 * swCnt * shwStride],
                        &pstTw[4 * swCnt * shwStride],
                        pstYa,
                        pstYb);

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
    VOS_INT32                           swLen;                                  /* 复数FFT阶数 */
    VOS_INT32                           swHalfLen;                              /* 折半阶数    */
    VOS_INT32                           swTmp;

    CODEC_OP_CMPX_STRU                stDc;
    CODEC_OP_CMPX_STRU               *pstOutDc;

    CODEC_OP_CMPX_STRU               *pstStart;
    CODEC_OP_CMPX_STRU               *pstEnd;
    CODEC_OP_CMPX_STRU               *pstSupTwiddle;

    CODEC_OP_CMPX_STRU                stOutFTmp;                                  /* 用于4字节对齐中转 */
    CODEC_OP_CMPX_STRU                stOutRTmp;

    swLen                   = pstObj->shwN;
    swTmp                   = swLen - 1;
    pstOutDc                = pstOut;
    pstOut                  = pstOut + 1;

    /* pstIn[0]为直流分量, 实部为实序列偶数序列之和, 虚部为实序列奇数序列之和 */
    stDc                    = pstIn[0];
    stDc.shwR               = stDc.shwR >> 1;
    stDc.shwI               = stDc.shwI >> 1;
    pstOutDc->shwR          = stDc.shwR + stDc.shwI;
    pstOutDc->shwI          = stDc.shwR - stDc.shwI;

    pstStart                = &pstIn[1];
    pstEnd                  = &pstIn[swTmp];
    pstSupTwiddle           = &(pstObj->pstSuperTwiddles[1]);

    swHalfLen               = swLen >> 1;

    for (swCnt = 1; swCnt <= swHalfLen; swCnt++)
    {
        MED_PpUnitFreqUnite(pstStart,
                            pstEnd,
                            pstSupTwiddle,
                           &stOutFTmp,
                           &stOutRTmp);

        pstOut[swCnt - 1].shwR = stOutFTmp.shwR;
        pstOut[swCnt - 1].shwI = stOutFTmp.shwI;

        pstOut[swTmp - swCnt].shwR = stOutRTmp.shwR;
        pstOut[swTmp - swCnt].shwI = stOutRTmp.shwI;

        pstStart++;
        pstEnd--;
        pstSupTwiddle++;

    }

}



VOS_VOID MED_FFT_SplitFreq(
                MED_FFT_CORE_STRU      *pstObj,
                CODEC_OP_CMPX_STRU       *pstIn,
                CODEC_OP_CMPX_STRU       *pstOut)
{
    VOS_INT32                           swCnt;
    VOS_INT16                           shwLen;                                 /* 复数FFT阶数 */
    VOS_INT16                           shwHalfLen;                             /* 折半阶数    */

    CODEC_OP_CMPX_STRU               *pstStart;
    CODEC_OP_CMPX_STRU               *pstEnd;
    CODEC_OP_CMPX_STRU               *pstInDc;

    CODEC_OP_CMPX_STRU                stStartTmp;                                 /* 用于4字节对齐中转 */
    CODEC_OP_CMPX_STRU                stEndTmp;

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

        stStartTmp.shwR = pstStart->shwR;
        stStartTmp.shwI = pstStart->shwI;
        stEndTmp.shwR   = pstEnd->shwR;
        stEndTmp.shwI   = pstEnd->shwI;

        MED_PpUnitFreqSplit(&stStartTmp,
                            &stEndTmp,
                            &(pstObj->pstSuperTwiddles[swCnt]),
                            &pstOut[swCnt],
                            &pstOut[shwLen - swCnt]);

        pstStart++;
        pstEnd--;

    }

}

VOS_VOID MED_FFT_cfft_dit_160_320(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y)
{
    /* 获取必须的信息 */

    /* 复数FFT点数 */
    VOS_INT32   N = pstObj->shwN;

    /* 旋转因子,同阶正逆FFT共用一个表 */
    VOS_INT32   *twd_fft160 = (VOS_INT32 *)g_aswMedFft320TwiddlesDit;

    VOS_INT32   *twd_fft320 = (VOS_INT32 *)g_aswMedFft640TwiddlesDit;

    /* digital permatation的获取 */
    VOS_INT16   *digperm    = (VOS_INT16 *)g_shwDigperm;


    // py0 ~ py3 for radix-4
    // py0 ~ py4 for radix-5
    VOS_INT16 *py0;
    VOS_INT16 *py1;
    VOS_INT16 *py2;
    VOS_INT16 *py3;
    //VOS_INT16 *py4;
    VOS_INT32  i;
    VOS_INT32  j;
    VOS_INT32  k;
    VOS_INT32  stride;
    VOS_INT32  quarterN;
    //VOS_INT32  pentaN;
    VOS_INT32  incr;
    VOS_INT32  decr;

    ae_p24x2s tw1;
    ae_p24x2s tw2;
    ae_p24x2s tw3;
    ae_p24x2s tw4;
    ae_p24x2s a0;
    ae_p24x2s a1;
    ae_p24x2s a2;
    ae_p24x2s a3;
    ae_p24x2s a4;
    ae_p24x2s a14; // add *ptr1 + *ptr4
    ae_p24x2s s14; // sub *ptr1 - *ptr4
    ae_p24x2s a23; // add *ptr2 + *ptr3
    ae_p24x2s s23; // sub *ptr2 - *ptr3

    ae_p24x2s b0;
    ae_p24x2s b1;
    ae_p24x2s b2;
    ae_p24x2s b3;
    ae_p24x2s b4;

    ae_p24x2s wa;
    ae_p24x2s wb;
    ae_p24x2s wc;

    ae_q56s   re;
    ae_q56s   im;

    ae_q56s   q56;
    VOS_INT32   norm;

    // px00 ~ px09 for complex FFT160
    // px00 ~ px19 for complex FFT320
    VOS_INT16  *px00;
    VOS_INT16  *px01;
    VOS_INT16  *px02;
    VOS_INT16  *px03;
    VOS_INT16  *px04;
    VOS_INT16  *px05;
    VOS_INT16  *px06;
    VOS_INT16  *px07;
    VOS_INT16  *px08;
    VOS_INT16  *px09;

    VOS_INT16  *px10;
    VOS_INT16  *px11;
    VOS_INT16  *px12;
    VOS_INT16  *px13;
    VOS_INT16  *px14;
    VOS_INT16  *px15;
    VOS_INT16  *px16;
    VOS_INT16  *px17;
    VOS_INT16  *px18;
    VOS_INT16  *px19;
    VOS_INT32   offset;
    VOS_INT32   StageNumRadix4;
    VOS_INT32   GrpNum;

    // int scale = 0;
    const VOS_INT32 *twd;

    q56 = AE_CVTQ48A32S(N);
    norm = AE_NSAQ56S(q56);



    // 1个复数对应4字节，相当于2个VOS_INT16，32表示32个实数，即16个复数间隔
    py0  = (VOS_INT16 *)y - 2;
    px00 = (VOS_INT16 *)x;
    px01 = px00 + 32;
    px02 = px01 + 32;
    px03 = px02 + 32;
    px04 = px03 + 32;
    px05 = px04 + 32;
    px06 = px05 + 32;
    px07 = px06 + 32;
    px08 = px07 + 32;
    px09 = px08 + 32;

    // complex FFT160
    if (norm & 1)
    {
        //-------------------------------------------------------------
        // The stage for digital permutation and radix-5
        //-------------------------------------------------------------

        // 读取基5旋转因子wa,wb和归一化旋转因子wc
        twd = twd_fft160;
        AE_LP16X2F_IU(wa, (const ae_p16x2s *)twd, 0 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wb, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wc, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

        for (i = 0; i < 16; i++)
        {

            offset = digperm[i];
            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px00, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px02, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px04, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px06, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px08, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px01, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px03, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px05, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px07, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px09, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
        }

        // 到这里，160点复数FFT第1级的DIT的基5输出符合预期 2013.03.29
//      printf("The output of first-stage for radix-5 DIT FFT\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }
    }
    // complex FFT320
    else
    {
        //-------------------------------------------------------------
        // The stage for digital permutation and radix-5
        //-------------------------------------------------------------

        px10 = px09 + 32;
        px11 = px10 + 32;
        px12 = px11 + 32;
        px13 = px12 + 32;
        px14 = px13 + 32;
        px15 = px14 + 32;
        px16 = px15 + 32;
        px17 = px16 + 32;
        px18 = px17 + 32;
        px19 = px18 + 32;

        twd = twd_fft320;
        wa = AE_LP16X2F_I((const ae_p16x2s *)twd, 0 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wb, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wc, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];
            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px00, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px04, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px08, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px12, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px16, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px01, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px05, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px09, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px13, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px17, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px02, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px06, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px10, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px14, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px18, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px03, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px07, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px11, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px15, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px19, offset * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
//#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_SUBSP24S(b1, b2);
            a4 = AE_ADDSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_ADDSP24S(b3, b4);
            a3 = AE_SUBSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
        }

        // 到这里，320点复数FFT第1级的DIT的基5输出符合预期 2013.03.30
//      printf("The output of first-stage for DIT CFFT320\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }
    }

    // complex FFT160
    if (norm & 1)
    {
        //-------------------------------------------------------------
        // The stage for radix-2
        //-------------------------------------------------------------

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        incr  = 10;

        // 同一级中相同蝶形参与计算完毕后，回退的长度，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, (-150+1-10)*2 = (-160+1)*2 = -318, (-120+1-40)*2 = -318
        decr  = 318;

        // 同一蝶形中各个节点，
        // 减法为预操作，保证地址更新后指向期望数据
        // 加法数量是同一蝶形相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        py0 = (VOS_INT16 *)y - 20;
        py1 = py0 + 10;

        // 遍历同一级中不同蝶形结构
        for (i = 0; i < 5; i++)
        {
            AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

            // 遍历同一级中的不同组
            for (j = 0; j < 16; j++)
            {
                AE_LP16X2F_XU(a0, (ae_p16x2s *)py0, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a1, (ae_p16x2s *)py1, incr * sizeof(ae_p16x2s));

                re = AE_MULZASP24S_HH_LL(a1, tw1);
                im = AE_MULZAAP24S_HL_LH(a1, tw1);
                a1 = AE_TRUNCP24Q48X2(re, im);
                a1 = AE_SLLISP24S(a1, 1);

//#ifdef NORM_RADIX2
                a0 = AE_SRAIP24(a0, 1);
                a1 = AE_SRAIP24(a1, 1);
//#endif

                b0 = AE_ADDSP24S(a0, a1);
                b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
                b0 = AE_ROUNDSP16ASYM(b0);
                b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
                b0 = AE_ROUNDSP16SYM(b0);
                b1 = AE_ROUNDSP16SYM(b1);
#endif

                AE_SP16X2F_I(b0, (ae_p16x2s *)py0, 0 * sizeof(ae_p16x2s));
                AE_SP16X2F_I(b1, (ae_p16x2s *)py1, 0 * sizeof(ae_p16x2s));
            }

            py0 -= decr;
            py1 -= decr;
        }

        // 到这里，复数FFT160第2级的DIT的基2输出符合预期, 2013.03.29
//      printf("The output of second-stage for radix-2 DIT FFT\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }

        // 配置后续的基4参数
        // 基4的级数
        StageNumRadix4 = 2;

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 40, 160
        incr           = 40;

        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 10, 40
        quarterN       = 10;

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 20, 80
        stride         = 20;

        // 当前级中的组(group)数
        // dit_cfft160, 4, 1
        GrpNum         = 4;
    }
    // complex FFT320
    else
    {
        // 基4的级数
        StageNumRadix4 = 3;

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft320, 20, 80, 320
        incr           = 20;

        // 同一级中相同蝶形参与计算完毕后，回退的长度，单位：实数，一个实数占用VOS_INT16
        // dit_cfft320, (-315+1-5)*2 = (-320+1)*2 = -638, (-300+1-20)*2 = -638, (-240+1-80)*2 = -638
        decr           = 638;

        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft320, 5, 20, 80
        quarterN       = 5;

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 10, 40, 160
        stride         = 10;

        // 当前级中的组(group)数
        // dit_cfft320, 16, 4, 1
        GrpNum         = 16;
    }

    //--------------------------------------------------------------------------
    // The stage for radix-4
    //--------------------------------------------------------------------------

    // 遍历基4的各级
    // dit_cfft160, 共2级基4
    // dit_cfft160, 共3级基4
    for (k = 0; k < StageNumRadix4; k++)
    {
        // 同一蝶形中各个节点，减法为预操作，保证地址更新后指向期望数据
        py0      = (VOS_INT16 *)y - (incr << 1);
        py1      = py0 + stride;
        py2      = py1 + stride;
        py3      = py2 + stride;

        // 遍历同一级中不同蝶形结构
        for (i = 0; i < quarterN; i++)
        {
            AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
            AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
            AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

            // 遍历同一级中的不同组
            for (j = 0; j < GrpNum; j++)
            {
                AE_LP16X2F_XU(a0, (ae_p16x2s *)py0, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a1, (ae_p16x2s *)py1, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a2, (ae_p16x2s *)py2, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a3, (ae_p16x2s *)py3, incr * sizeof(ae_p16x2s));

                re = AE_MULZASP24S_HH_LL(a2, tw2);
                im = AE_MULZAAP24S_HL_LH(a2, tw2);
                a2 = AE_TRUNCP24Q48X2(re, im);
                a2 = AE_SLLISP24S(a2, 1);

                re = AE_MULZASP24S_HH_LL(a1, tw1);
                im = AE_MULZAAP24S_HL_LH(a1, tw1);
                a1 = AE_TRUNCP24Q48X2(re, im);
                a1 = AE_SLLISP24S(a1, 1);

                re = AE_MULZASP24S_HH_LL(a3, tw3);
                im = AE_MULZAAP24S_HL_LH(a3, tw3);
                a3 = AE_TRUNCP24Q48X2(re, im);
                a3 = AE_SLLISP24S(a3, 1);

//#ifdef NORM_RADIX4
                a0 = AE_SRAIP24(a0, 2);
                a1 = AE_SRAIP24(a1, 2);
                a2 = AE_SRAIP24(a2, 2);
                a3 = AE_SRAIP24(a3, 2);
//#endif

                b0 = AE_ADDSP24S(a0, a2);
                b2 = AE_ADDSP24S(a1, a3);
                b1 = AE_SUBSP24S(a0, a2);
                b3 = AE_SUBSP24S(a1, a3);

                a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
                a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
                a0 = AE_ROUNDSP16SYM(a0);
#endif

                AE_SP16X2F_I(a0, (ae_p16x2s *)py0, 0 * sizeof(ae_p16x2s));

                a2 = AE_SUBSP24S(b0, b2);


#ifdef ROUND_ASYM_ENABLE
                a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
                a2 = AE_ROUNDSP16SYM(a2);
#endif
                AE_SP16X2F_I(a2, (ae_p16x2s *)py2, 0 * sizeof(ae_p16x2s));

                a3 = AE_SELP24_LH(b3, b3);
                b0 = AE_ADDSP24S(b1, a3);
                b2 = AE_SUBSP24S(b1, a3);
                a1 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
                a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
                a1 = AE_ROUNDSP16SYM(a1);
#endif
                AE_SP16X2F_I(a1, (ae_p16x2s *)py1, 0 * sizeof(ae_p16x2s));

                a3 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
                a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
                a3 = AE_ROUNDSP16SYM(a3);
#endif

                AE_SP16X2F_I(a3, (ae_p16x2s *)py3, 0 * sizeof(ae_p16x2s));
            }

            py0 -= decr;
            py1 -= decr;
            py2 -= decr;
            py3 -= decr;
        }

        if (2 == k)
        {
            // 到这里，复数FFT160第3级的DIT的基4输出符合预期, 2013.03.29
            // 到这里，复数FFT160第4级的DIT的基4输出符合预期, 2013.03.29
//          printf("The output of radix-4 DIT FFT\n");
//          for (i = 0; i < N; i++)
//          {
//              printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//          }
        }


        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 10, 40
        // dit_cfft320, 5, 20, 80
        quarterN = (quarterN << 2);

        // 当前级中的组(group)数
        // dit_cfft160, 4, 1
        // dit_cfft320, 16, 4, 1
        GrpNum   = (GrpNum >> 2);

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 40, 160
        // dit_cfft320, 20, 80, 320
        incr     = (incr << 2);

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 40, 160
        // dit_cfft320, 20, 80
        stride   = (stride << 2);
    }
}


VOS_VOID MED_FFT_cifft_dit_160_320(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y)
{
    /* 获取必须的信息 */

    /* 复数FFT点数 */
    VOS_INT32   N = pstObj->shwN;

    /* 旋转因子,同阶正逆FFT共用一个表 */
    VOS_INT32   *twd_fft160 = (VOS_INT32 *)g_aswMedFft320TwiddlesDit;

    VOS_INT32   *twd_fft320 = (VOS_INT32 *)g_aswMedFft640TwiddlesDit;

    /* digital permatation的获取 */
    VOS_INT16   *digperm    = (VOS_INT16 *)g_shwDigperm;

    // py0 ~ py3 for radix-4
    // py0 ~ py4 for radix-5
    VOS_INT16 *py0;
    VOS_INT16 *py1;
    VOS_INT16 *py2;
    VOS_INT16 *py3;
    //VOS_INT16 *py4;
    VOS_INT32  i;
    VOS_INT32  j;
    VOS_INT32  k;
    VOS_INT32  stride;
    VOS_INT32  quarterN;
    //VOS_INT32  pentaN;
    VOS_INT32  incr;
    VOS_INT32  decr;

    ae_p24x2s tw1;
    ae_p24x2s tw2;
    ae_p24x2s tw3;
    ae_p24x2s tw4;
    ae_p24x2s a0;
    ae_p24x2s a1;
    ae_p24x2s a2;
    ae_p24x2s a3;
    ae_p24x2s a4;
    ae_p24x2s a14; // add *ptr1 + *ptr4
    ae_p24x2s s14; // sub *ptr1 - *ptr4
    ae_p24x2s a23; // add *ptr2 + *ptr3
    ae_p24x2s s23; // sub *ptr2 - *ptr3

    ae_p24x2s b0;
    ae_p24x2s b1;
    ae_p24x2s b2;
    ae_p24x2s b3;
    ae_p24x2s b4;

    ae_p24x2s wa;
    ae_p24x2s wb;
    ae_p24x2s wc;

    ae_q56s   re;
    ae_q56s   im;

    ae_q56s   q56;
    VOS_INT32   norm;

    // px00 ~ px09 for complex FFT160
    // px00 ~ px19 for complex FFT320
    VOS_INT16  *px00;
    VOS_INT16  *px01;
    VOS_INT16  *px02;
    VOS_INT16  *px03;
    VOS_INT16  *px04;
    VOS_INT16  *px05;
    VOS_INT16  *px06;
    VOS_INT16  *px07;
    VOS_INT16  *px08;
    VOS_INT16  *px09;

    VOS_INT16  *px10;
    VOS_INT16  *px11;
    VOS_INT16  *px12;
    VOS_INT16  *px13;
    VOS_INT16  *px14;
    VOS_INT16  *px15;
    VOS_INT16  *px16;
    VOS_INT16  *px17;
    VOS_INT16  *px18;
    VOS_INT16  *px19;
    VOS_INT32   offset;
    VOS_INT32   StageNumRadix4;
    VOS_INT32   GrpNum;

    // int scale = 0;
    const VOS_INT32 *twd;

    q56 = AE_CVTQ48A32S(N);
    norm = AE_NSAQ56S(q56);



    // 1个复数对应4字节，相当于2个VOS_INT16，32表示32个实数，即16个复数间隔
    py0  = (VOS_INT16 *)y - 2;
    px00 = (VOS_INT16 *)x;
    px01 = px00 + 32;
    px02 = px01 + 32;
    px03 = px02 + 32;
    px04 = px03 + 32;
    px05 = px04 + 32;
    px06 = px05 + 32;
    px07 = px06 + 32;
    px08 = px07 + 32;
    px09 = px08 + 32;

    // complex FFT160
    if (norm & 1)
    {
        //-------------------------------------------------------------
        // The stage for digital permutation and radix-5
        //-------------------------------------------------------------

        // 读取基5旋转因子wa,wb和归一化旋转因子wc
        twd = twd_fft160;
        AE_LP16X2F_IU(wa, (const ae_p16x2s *)twd, 0 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wb, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wc, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

        for (i = 0; i < 16; i++)
        {

            offset = digperm[i];
            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px00, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px02, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px04, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px06, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px08, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px01, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px03, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px05, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px07, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px09, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
        }

        // 到这里，160点复数FFT第1级的DIT的基5输出符合预期 2013.03.29
//      printf("The output of first-stage for radix-5 DIT FFT\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }
    }
    // complex FFT320
    else
    {
        //-------------------------------------------------------------
        // The stage for digital permutation and radix-5
        //-------------------------------------------------------------

        px10 = px09 + 32;
        px11 = px10 + 32;
        px12 = px11 + 32;
        px13 = px12 + 32;
        px14 = px13 + 32;
        px15 = px14 + 32;
        px16 = px15 + 32;
        px17 = px16 + 32;
        px18 = px17 + 32;
        px19 = px18 + 32;

        twd = twd_fft320;
        wa = AE_LP16X2F_I((const ae_p16x2s *)twd, 0 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wb, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
        AE_LP16X2F_IU(wc, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];
            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px00, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px04, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px08, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px12, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px16, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px01, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px05, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px09, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px13, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px17, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px02, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px06, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px10, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px14, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px18, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            //--------------------------------------------------------------
            // Butterfly for radix-5
            //--------------------------------------------------------------
            a0 = AE_LP16X2F_X((ae_p16x2s *)px03, offset * sizeof(ae_p16x2s));
            a1 = AE_LP16X2F_X((ae_p16x2s *)px07, offset * sizeof(ae_p16x2s));
            a2 = AE_LP16X2F_X((ae_p16x2s *)px11, offset * sizeof(ae_p16x2s));
            a3 = AE_LP16X2F_X((ae_p16x2s *)px15, offset * sizeof(ae_p16x2s));
            a4 = AE_LP16X2F_X((ae_p16x2s *)px19, offset * sizeof(ae_p16x2s));

#ifdef NORM_RADIX5
            re = AE_MULFS32P16S_HH(a0, wc);
            im = AE_MULFS32P16S_LH(a0, wc);
            a0 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a1, wc);
            im = AE_MULFS32P16S_LH(a1, wc);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a2, wc);
            im = AE_MULFS32P16S_LH(a2, wc);
            a2 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a3, wc);
            im = AE_MULFS32P16S_LH(a3, wc);
            a3 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a4, wc);
            im = AE_MULFS32P16S_LH(a4, wc);
            a4 = AE_TRUNCP24Q48X2(re, im);
#endif

            a14 = AE_ADDSP24S(a1, a4);
            s14 = AE_SUBSP24S(a1, a4);

            a23 = AE_ADDSP24S(a2, a3);
            s23 = AE_SUBSP24S(a2, a3);
            // a1, a2, a3, a4 are now available

            b0 = AE_ADDSP24S(a14, a23);
            b0 = AE_ADDSP24S(b0, a0);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
#endif

            AE_SP16X2F_IU(b0, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

            re = AE_MULFS32P16S_HH(a14, wa);
            im = AE_MULFS32P16S_LH(a14, wa);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wb);
            im = AE_MULFS32P16S_LH(a23, wb);
            a2 = AE_TRUNCP24Q48X2(re, im);

            a1 = AE_ADDSP24S(a1, a2);
            b1 = AE_ADDSP24S(a1, a0);

            re = AE_MULFS32P16S_LL(s14, wa);
            im = AE_MULFS32P16S_HL(s14, wa);
            im = AE_NEGSQ56S(im);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s23, wb);
            im = AE_MULFS32P16S_HL(s23, wb);
            im = AE_NEGSQ56S(im);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b2 = AE_ADDSP24S(a1, a2);

            a1 = AE_ADDSP24S(b1, b2);
            a4 = AE_SUBSP24S(b1, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_IU(a1, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a4 = AE_ROUNDSP16ASYM(a4);
#endif

#ifdef ROUND_SYM_ENABLE
            a4 = AE_ROUNDSP16SYM(a4);
#endif

            //

            re = AE_MULFS32P16S_LL(s23, wa);
            im = AE_MULFS32P16S_HL(s14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_LL(s14, wb);
            im = AE_MULFS32P16S_HL(s23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b4 = AE_SUBSP24S(a1, a2);

            re = AE_MULFS32P16S_HH(a14, wb);
            im = AE_MULFS32P16S_LH(a14, wb);
            a1 = AE_TRUNCP24Q48X2(re, im);

            re = AE_MULFS32P16S_HH(a23, wa);
            im = AE_MULFS32P16S_LH(a23, wa);
            a2 = AE_TRUNCP24Q48X2(re, im);

            b3 = AE_ADDSP24S(a1, a2);
            b3 = AE_ADDSP24S(b3, a0);

            a2 = AE_SUBSP24S(b3, b4);
            a3 = AE_ADDSP24S(b3, b4);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_IU(a2, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_IU(a3, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
            AE_SP16X2F_IU(a4, (ae_p16x2s *)py0, 1 * sizeof(ae_p16x2s));
        }

        // 到这里，320点复数FFT第1级的DIT的基5输出符合预期 2013.03.30
//      printf("The output of first-stage for DIT CFFT320\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }
    }

    // complex FFT160
    if (norm & 1)
    {
        //-------------------------------------------------------------
        // The stage for radix-2
        //-------------------------------------------------------------

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        incr  = 10;

        // 同一级中相同蝶形参与计算完毕后，回退的长度，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, (-150+1-10)*2 = (-160+1)*2 = -318, (-120+1-40)*2 = -318
        decr  = 318;

        // 同一蝶形中各个节点，
        // 减法为预操作，保证地址更新后指向期望数据
        // 加法数量是同一蝶形相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        py0 = (VOS_INT16 *)y - 20;
        py1 = py0 + 10;

        // 遍历同一级中不同蝶形结构
        for (i = 0; i < 5; i++)
        {
            AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

            // 遍历同一级中的不同组
            for (j = 0; j < 16; j++)
            {
                AE_LP16X2F_XU(a0, (ae_p16x2s *)py0, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a1, (ae_p16x2s *)py1, incr * sizeof(ae_p16x2s));

                re = AE_MULZAAP24S_HH_LL(tw1, a1);
                im = AE_MULZASP24S_HL_LH(tw1, a1);
                a1 = AE_TRUNCP24Q48X2(re, im);
                a1 = AE_SLLISP24S(a1, 1);

#ifdef NORM_RADIX2
                a0 = AE_SRAIP24(a0, 1);
                a1 = AE_SRAIP24(a1, 1);
#endif

                b0 = AE_ADDSP24S(a0, a1);
                b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
                b0 = AE_ROUNDSP16ASYM(b0);
                b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
                b0 = AE_ROUNDSP16SYM(b0);
                b1 = AE_ROUNDSP16SYM(b1);
#endif

                AE_SP16X2F_I(b0, (ae_p16x2s *)py0, 0 * sizeof(ae_p16x2s));
                AE_SP16X2F_I(b1, (ae_p16x2s *)py1, 0 * sizeof(ae_p16x2s));
            }

            py0 -= decr;
            py1 -= decr;
        }

        // 到这里，复数FFT160第2级的DIT的基2输出符合预期, 2013.03.29
//      printf("The output of second-stage for radix-2 DIT FFT\n");
//      for (i = 0; i < N; i++)
//      {
//          printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//      }

        // 配置后续的基4参数
        // 基4的级数
        StageNumRadix4 = 2;

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 40, 160
        incr           = 40;

        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 10, 40
        quarterN       = 10;

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 20, 80
        stride         = 20;

        // 当前级中的组(group)数
        // dit_cfft160, 4, 1
        GrpNum         = 4;
    }
    // complex FFT320
    else
    {
        // 基4的级数
        StageNumRadix4 = 3;

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft320, 20, 80, 320
        incr           = 20;

        // 同一级中相同蝶形参与计算完毕后，回退的长度，单位：实数，一个实数占用VOS_INT16
        // dit_cfft320, (-315+1-5)*2 = (-320+1)*2 = -638, (-300+1-20)*2 = -638, (-240+1-80)*2 = -638
        decr           = 638;

        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft320, 5, 20, 80
        quarterN       = 5;

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 10, 40, 160
        stride         = 10;

        // 当前级中的组(group)数
        // dit_cfft320, 16, 4, 1
        GrpNum         = 16;
    }

    //--------------------------------------------------------------------------
    // The stage for radix-4
    //--------------------------------------------------------------------------

    // 遍历基4的各级
    // dit_cfft160, 共2级基4
    // dit_cfft160, 共3级基4
    for (k = 0; k < StageNumRadix4; k++)
    {
        // 同一蝶形中各个节点，减法为预操作，保证地址更新后指向期望数据
        py0      = (VOS_INT16 *)y - (incr << 1);
        py1      = py0 + stride;
        py2      = py1 + stride;
        py3      = py2 + stride;

        // 遍历同一级中不同蝶形结构
        for (i = 0; i < quarterN; i++)
        {
            AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
            AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));
            AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1 * sizeof(VOS_INT32));

            // 遍历同一级中的不同组
            for (j = 0; j < GrpNum; j++)
            {
                AE_LP16X2F_XU(a0, (ae_p16x2s *)py0, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a1, (ae_p16x2s *)py1, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a2, (ae_p16x2s *)py2, incr * sizeof(ae_p16x2s));
                AE_LP16X2F_XU(a3, (ae_p16x2s *)py3, incr * sizeof(ae_p16x2s));

                re = AE_MULZAAP24S_HH_LL(tw2, a2);
                im = AE_MULZASP24S_HL_LH(tw2, a2);
                a2 = AE_TRUNCP24Q48X2(re, im);
                a2 = AE_SLLISP24S(a2, 1);

                re = AE_MULZAAP24S_HH_LL(tw1, a1);
                im = AE_MULZASP24S_HL_LH(tw1, a1);
                a1 = AE_TRUNCP24Q48X2(re, im);
                a1 = AE_SLLISP24S(a1, 1);

                re = AE_MULZAAP24S_HH_LL(tw3, a3);
                im = AE_MULZASP24S_HL_LH(tw3, a3);
                a3 = AE_TRUNCP24Q48X2(re, im);
                a3 = AE_SLLISP24S(a3, 1);

#ifdef NORM_RADIX4
                a0 = AE_SRAIP24(a0, 2);
                a1 = AE_SRAIP24(a1, 2);
                a2 = AE_SRAIP24(a2, 2);
                a3 = AE_SRAIP24(a3, 2);
#endif

                b0 = AE_ADDSP24S(a0, a2);
                b2 = AE_ADDSP24S(a1, a3);
                b1 = AE_SUBSP24S(a0, a2);
                b3 = AE_SUBSP24S(a1, a3);

                a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
                a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
                a0 = AE_ROUNDSP16SYM(a0);
#endif

                AE_SP16X2F_I(a0, (ae_p16x2s *)py0, 0 * sizeof(ae_p16x2s));

                a2 = AE_SUBSP24S(b0, b2);


#ifdef ROUND_ASYM_ENABLE
                a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
                a2 = AE_ROUNDSP16SYM(a2);
#endif
                AE_SP16X2F_I(a2, (ae_p16x2s *)py2, 0 * sizeof(ae_p16x2s));

                a3 = AE_SELP24_LH(b3, b3);
                b0 = AE_ADDSP24S(b1, a3);
                b2 = AE_SUBSP24S(b1, a3);
                a1 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
                a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
                a1 = AE_ROUNDSP16SYM(a1);
#endif
                AE_SP16X2F_I(a1, (ae_p16x2s *)py1, 0 * sizeof(ae_p16x2s));

                a3 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
                a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
                a3 = AE_ROUNDSP16SYM(a3);
#endif

                AE_SP16X2F_I(a3, (ae_p16x2s *)py3, 0 * sizeof(ae_p16x2s));
            }

            py0 -= decr;
            py1 -= decr;
            py2 -= decr;
            py3 -= decr;
        }

        if (2 == k)
        {
            // 到这里，复数FFT160第3级的DIT的基4输出符合预期, 2013.03.29
            // 到这里，复数FFT160第4级的DIT的基4输出符合预期, 2013.03.29
//          printf("The output of radix-4 DIT FFT\n");
//          for (i = 0; i < N; i++)
//          {
//              printf("y[%3d] = %6d + %6di\n", i, y[2*i], y[2*i + 1]);
//          }
        }


        // 当前级同一蝶形中两个相邻节点之间的距离, 即同一级中不同蝶形结构的数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 10, 40
        // dit_cfft320, 5, 20, 80
        quarterN = (quarterN << 2);

        // 当前级中的组(group)数
        // dit_cfft160, 4, 1
        // dit_cfft320, 16, 4, 1
        GrpNum   = (GrpNum >> 2);

        // 当前级相邻组(group)中同一蝶形结构之间的距离，即每组元素数量，单位：复数，一个复数占用VOS_INT32
        // dit_cfft160, 40, 160
        // dit_cfft320, 20, 80, 320
        incr     = (incr << 2);

        // 同一蝶形中两个相邻节点之间的距离，单位：实数，一个实数占用VOS_INT16
        // dit_cfft160, 40, 160
        // dit_cfft320, 20, 80
        stride   = (stride << 2);
    }
}


VOS_VOID MED_FFT_cfft_dif_128_256(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y)
{
    /* 获取必须的信息 */

    /* 复数FFT点数 */
    VOS_INT32   N = pstObj->shwN;

    /* 旋转因子,同阶正逆FFT共用一个表 */
    VOS_INT32   *twd_fft128_dif = (VOS_INT32 *)g_aswMedFft256Twiddles;

    VOS_INT32   *twd_fft256_dif = (VOS_INT32 *)g_aswMedFft512Twiddles;

    /* digital permatation的获取 */
    VOS_INT16   *digperm    = (VOS_INT16 *)g_shwDigperm;

    // px0 ~ px3 for radix-4
    // px0 ~ px4 for radix-5
    VOS_INT16 *px0;
    VOS_INT16 *px1;
    VOS_INT16 *px2;
    VOS_INT16 *px3;
    VOS_INT16 *px4;
    VOS_INT32  i;
    VOS_INT32  j;
    VOS_INT32  stride;
    VOS_INT32  quarterN;
    VOS_INT32  pentaN;
    VOS_INT32  incr;
    VOS_INT32  decr;

    ae_p24x2s tw1;
    ae_p24x2s tw2;
    ae_p24x2s tw3;
    ae_p24x2s tw4;
    ae_p24x2s a0;
    ae_p24x2s a1;
    ae_p24x2s a2;
    ae_p24x2s a3;
    ae_p24x2s a4;
    ae_p24x2s a14; // add *ptr1 + *ptr4
    ae_p24x2s s14; // sub *ptr1 - *ptr4
    ae_p24x2s a23; // add *ptr2 + *ptr3
    ae_p24x2s s23; // sub *ptr2 - *ptr3

    ae_p24x2s b0;
    ae_p24x2s b1;
    ae_p24x2s b2;
    ae_p24x2s b3;
    ae_p24x2s b4;

    ae_p24x2s wa;
    ae_p24x2s wb;
    ae_p24x2s wc;

    ae_q56s   re;
    ae_q56s   im;

    ae_q56s   q56;
    VOS_INT32   norm;

    // py00 ~ py09 for complex FFT160
    // py00 ~ py19 for complex FFT320
    VOS_INT16  *py00;
    VOS_INT16  *py01;
    VOS_INT16  *py02;
    VOS_INT16  *py03;
    VOS_INT16  *py04;
    VOS_INT16  *py05;
    VOS_INT16  *py06;
    VOS_INT16  *py07;
    VOS_INT16  *py08;
    VOS_INT16  *py09;

    VOS_INT16  *py10;
    VOS_INT16  *py11;
    VOS_INT16  *py12;
    VOS_INT16  *py13;
    VOS_INT16  *py14;
    VOS_INT16  *py15;
    VOS_INT16  *py16;
    VOS_INT16  *py17;
    VOS_INT16  *py18;
    VOS_INT16  *py19;
    VOS_INT32   offset;

    // int scale = 0;
    const VOS_INT32 *twd;

    q56 = AE_CVTQ48A32S(N);
    norm = AE_NSAQ56S(q56);

    // complex FFT128
    if (norm & 1)
    {
        // 减1为预操作，保证地址更新时指向期望数据
        twd = twd_fft128_dif - 1;
    }
    // complex FFT256
    else
    {
        twd = twd_fft256_dif - 1;
    }

    // 第1级基4蝶形的相邻节点的距离，单位为VOS_INT16
    stride   = (N >> 1);
    // 第1级基4蝶形的相邻节点的距离，单位为VOS_INT32
    quarterN = (N >> 2);

    //-----------------------------------------------------------------
    // The first stage for DIF radix-4
    //-----------------------------------------------------------------
    incr = 1;
    px0  = (VOS_INT16 *)x - 2;
    px1  = px0 + stride;
    px2  = px1 + stride;
    //px2  = px0 + N;  // 另一种写法，两种写法对总体cycle数的影响相同
    px3  = px2 + stride;

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX4
        a0 = AE_SRAIP24(a0, 2);
        a1 = AE_SRAIP24(a1, 2);
        a2 = AE_SRAIP24(a2, 2);
        a3 = AE_SRAIP24(a3, 2);
//#endif

        b0 = AE_ADDSP24S(a0, a2);
        b2 = AE_ADDSP24S(a1, a3);
        b1 = AE_SUBSP24S(a0, a2);
        b3 = AE_SUBSP24S(a1, a3);

        a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
        a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
        a0 = AE_ROUNDSP16SYM(a0);
#endif

        AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a2 = AE_SUBSP24S(b0, b2);
        re = AE_MULZASP24S_HH_LL(a2, tw2);
        im = AE_MULZAAP24S_HL_LH(a2, tw2);
        a2 = AE_TRUNCP24Q48X2(re, im);
        a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
        a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
        a2 = AE_ROUNDSP16SYM(a2);
#endif
        AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a3 = AE_SELP24_LH(b3, b3);
        b0 = AE_ADDSP24S(b1, a3);
        b2 = AE_SUBSP24S(b1, a3);
        a1 = AE_SELP24_HL(b0, b2);
        re = AE_MULZASP24S_HH_LL(a1, tw1);
        im = AE_MULZAAP24S_HL_LH(a1, tw1);
        a1 = AE_TRUNCP24Q48X2(re, im);
        a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
        a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
        a1 = AE_ROUNDSP16SYM(a1);
#endif
        AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a3 = AE_SELP24_HL(b2, b0);
        re = AE_MULZASP24S_HH_LL(a3, tw3);
        im = AE_MULZAAP24S_HL_LH(a3, tw3);
        a3 = AE_TRUNCP24Q48X2(re, im);
        a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
        a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
        a3 = AE_ROUNDSP16SYM(a3);
#endif

        AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
    }

    // 到这里，160点复数FFT第1级的DIF的基4输出符合预期 2013.03.
    // 到这里，320点复数FFT第1级的DIF的基4输出符合预期 2013.03.
//  printf("The output of first-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }

    //-----------------------------------------------------------------
    // The second stage for DIF radix-4
    //-----------------------------------------------------------------
    // complex FFT128
    if (norm & 1)
    {
        incr = 32;
        decr = 254; // (-96+1-32)*2 = -254
        quarterN = 8;

        px0 = (VOS_INT16 *)x - 64;
        px1 = px0 + 16;
        px2 = px1 + 16;
        px3 = px2 + 16;
    }
    // complex FFT256
    else
    {
        incr = 64;
        decr = 510; // (-192+1-64)*2 = -510
        quarterN = 16;

        px0 = (VOS_INT16 *)x - 128;
        px1 = px0 + 32;
        px2 = px1 + 32;
        px3 = px2 + 32;
    }

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));

        // 同一级中的不同组数
        for (j = 0; j < 4; j++)
        {
            AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX4
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);
            re = AE_MULZASP24S_HH_LL(a2, tw2);
            im = AE_MULZAAP24S_HL_LH(a2, tw2);
            a2 = AE_TRUNCP24Q48X2(re, im);
            a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif
            AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);
            re = AE_MULZASP24S_HH_LL(a1, tw1);
            im = AE_MULZAAP24S_HL_LH(a1, tw1);
            a1 = AE_TRUNCP24Q48X2(re, im);
            a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif
            AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);
            re = AE_MULZASP24S_HH_LL(a3, tw3);
            im = AE_MULZAAP24S_HL_LH(a3, tw3);
            a3 = AE_TRUNCP24Q48X2(re, im);
            a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
        }

        px0 -= decr;
        px1 -= decr;
        px2 -= decr;
        px3 -= decr;
    }

    // 到这里，复数FFT160第2级的DIF的基4输出符合预期, 2013.03.
    // 到这里，复数FFT320第2级的DIF的基4输出符合预期, 2013.03.
//  printf("The output of second-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }

    //-----------------------------------------------------------------
    // The third stage for DIF radix-4
    //-----------------------------------------------------------------
    // complex FFT128
    if (norm & 1)
    {
        incr = 8;
        //decr = 254; // (-120+1-8)*2 = -254
        quarterN = 2;

        px0 = (VOS_INT16 *)x - 16;
        px1 = px0 + 4;
        px2 = px1 + 4;
        px3 = px2 + 4;
    }
    // complex FFT256
    else
    {
        incr = 16;
        //decr = 510; // (-240+1-16)*2 = -510
        quarterN = 4;

        px0 = (VOS_INT16 *)x - 32;
        px1 = px0 + 8;
        px2 = px1 + 8;
        px3 = px2 + 8;
    }

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));

        // 同一级中的不同组数
        for (j = 0; j < 16; j++)
        {
            AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

//#ifdef NORM_RADIX4
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);
            re = AE_MULZASP24S_HH_LL(a2, tw2);
            im = AE_MULZAAP24S_HL_LH(a2, tw2);
            a2 = AE_TRUNCP24Q48X2(re, im);
            a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif
            AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);
            re = AE_MULZASP24S_HH_LL(a1, tw1);
            im = AE_MULZAAP24S_HL_LH(a1, tw1);
            a1 = AE_TRUNCP24Q48X2(re, im);
            a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif
            AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);
            re = AE_MULZASP24S_HH_LL(a3, tw3);
            im = AE_MULZAAP24S_HL_LH(a3, tw3);
            a3 = AE_TRUNCP24Q48X2(re, im);
            a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
        }

        px0 -= decr;
        px1 -= decr;
        px2 -= decr;
        px3 -= decr;
    }

    // 到这里，复数FFT160第2级的DIF的基4输出符合预期, 2013.03.
    // 到这里，复数FFT320第2级的DIF的基4输出符合预期, 2013.03.
//  printf("The output of third-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }


    //--------------------------------------------------------------------------
    // The last stage for radix-2 or radix-4
    //--------------------------------------------------------------------------
    px0 = (VOS_INT16 *)x - 2;
    py00 = (VOS_INT16 *)y;
    // 1个复数对应4字节，相当于2个VOS_INT16，32表示32个实数，即16个复数间隔
    py01 = py00 + 32;
    py02 = py01 + 32;
    py03 = py02 + 32;
    py04 = py03 + 32;
    py05 = py04 + 32;
    py06 = py05 + 32;
    py07 = py06 + 32;

    //--------------------------------------------------------------------------
    // last stage is RADIX2 !!!
    //--------------------------------------------------------------------------
    // complex FFT 128
    if (norm & 1)
    {
        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
//#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py00, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py04, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
//#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py01, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py05, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
//#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py02, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py06, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
//#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py03, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py07, offset*sizeof(ae_p16x2s));
        }
    }

    //--------------------------------------------------------------------------
    // last stage is RADIX4 !!!
    //--------------------------------------------------------------------------
    // complex FFT 320
    else
    {
        py08 = py07 + 32;
        py09 = py08 + 32;
        py10 = py09 + 32;
        py11 = py10 + 32;
        py12 = py11 + 32;
        py13 = py12 + 32;
        py14 = py13 + 32;
        py15 = py14 + 32;

        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py00, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py08, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py04, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py12, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py01, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py09, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py05, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py13, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py02, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py10, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py06, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py14, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

//#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
//#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py03, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py11, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py07, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py15, offset*sizeof(ae_p16x2s));
        }
    }
}


VOS_VOID MED_FFT_cifft_dif_128_256(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y)
{
    /* 获取必须的信息 */

    /* 复数FFT点数 */
    VOS_INT32   N = pstObj->shwN;

    /* 旋转因子,同阶正逆FFT共用一个表 */
    VOS_INT32   *twd_fft128_dif = (VOS_INT32 *)g_aswMedFft256Twiddles;

    VOS_INT32   *twd_fft256_dif = (VOS_INT32 *)g_aswMedFft512Twiddles;

    /* digital permatation的获取 */
    VOS_INT16   *digperm    = (VOS_INT16 *)g_shwDigperm;

    // px0 ~ px3 for radix-4
    // px0 ~ px4 for radix-5
    VOS_INT16 *px0;
    VOS_INT16 *px1;
    VOS_INT16 *px2;
    VOS_INT16 *px3;
    VOS_INT16 *px4;
    VOS_INT32  i;
    VOS_INT32  j;
    VOS_INT32  stride;
    VOS_INT32  quarterN;
    VOS_INT32  pentaN;
    VOS_INT32  incr;
    VOS_INT32  decr;

    ae_p24x2s tw1;
    ae_p24x2s tw2;
    ae_p24x2s tw3;
    ae_p24x2s tw4;
    ae_p24x2s a0;
    ae_p24x2s a1;
    ae_p24x2s a2;
    ae_p24x2s a3;
    ae_p24x2s a4;
    ae_p24x2s a14; // add *ptr1 + *ptr4
    ae_p24x2s s14; // sub *ptr1 - *ptr4
    ae_p24x2s a23; // add *ptr2 + *ptr3
    ae_p24x2s s23; // sub *ptr2 - *ptr3

    ae_p24x2s b0;
    ae_p24x2s b1;
    ae_p24x2s b2;
    ae_p24x2s b3;
    ae_p24x2s b4;

    ae_p24x2s wa;
    ae_p24x2s wb;
    ae_p24x2s wc;

    ae_q56s   re;
    ae_q56s   im;

    ae_q56s   q56;
    VOS_INT32   norm;

    // py00 ~ py07 for complex FFT128
    // py00 ~ py19 for complex FFT256
    VOS_INT16  *py00;
    VOS_INT16  *py01;
    VOS_INT16  *py02;
    VOS_INT16  *py03;
    VOS_INT16  *py04;
    VOS_INT16  *py05;
    VOS_INT16  *py06;
    VOS_INT16  *py07;

    VOS_INT16  *py08;
    VOS_INT16  *py09;
    VOS_INT16  *py10;
    VOS_INT16  *py11;
    VOS_INT16  *py12;
    VOS_INT16  *py13;
    VOS_INT16  *py14;
    VOS_INT16  *py15;
    //VOS_INT16  *py16;
    //VOS_INT16  *py17;
    //VOS_INT16  *py18;
    //VOS_INT16  *py19;
    VOS_INT32   offset;

    // int scale = 0;
    const VOS_INT32 *twd;

    q56 = AE_CVTQ48A32S(N);
    norm = AE_NSAQ56S(q56);

    // complex FFT128
    if (norm & 1)
    {
        // 减1为预操作，保证地址更新时指向期望数据
        twd = twd_fft128_dif - 1;
    }
    // complex FFT256
    else
    {
        twd = twd_fft256_dif - 1;
    }

    // 第1级基4蝶形的相邻节点的距离，单位为VOS_INT16
    stride   = (N >> 1);
    // 第1级基4蝶形的相邻节点的距离，单位为VOS_INT32
    quarterN = (N >> 2);

    //-----------------------------------------------------------------
    // The first stage for DIF radix-4
    //-----------------------------------------------------------------
    incr = 1;
    px0  = (VOS_INT16 *)x - 2;
    px1  = px0 + stride;
    px2  = px1 + stride;
    //px2  = px0 + N;  // 另一种写法，两种写法对总体cycle数的影响相同
    px3  = px2 + stride;

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
        AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

#ifdef NORM_RADIX4
        a0 = AE_SRAIP24(a0, 2);
        a1 = AE_SRAIP24(a1, 2);
        a2 = AE_SRAIP24(a2, 2);
        a3 = AE_SRAIP24(a3, 2);
#endif

        b0 = AE_ADDSP24S(a0, a2);
        b2 = AE_ADDSP24S(a1, a3);
        b1 = AE_SUBSP24S(a0, a2);
        b3 = AE_SUBSP24S(a1, a3);

        a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
        a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
        a0 = AE_ROUNDSP16SYM(a0);
#endif

        AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a2 = AE_SUBSP24S(b0, b2);
        re = AE_MULZAAP24S_HH_LL(tw2, a2);
        im = AE_MULZASP24S_HL_LH(tw2, a2);
        a2 = AE_TRUNCP24Q48X2(re, im);
        a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
        a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
        a2 = AE_ROUNDSP16SYM(a2);
#endif
        AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a3 = AE_SELP24_LH(b3, b3);
        b0 = AE_ADDSP24S(b1, a3);
        b2 = AE_SUBSP24S(b1, a3);
        a1 = AE_SELP24_HL(b2, b0);
        re = AE_MULZAAP24S_HH_LL(tw1, a1);
        im = AE_MULZASP24S_HL_LH(tw1, a1);
        a1 = AE_TRUNCP24Q48X2(re, im);
        a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
        a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
        a1 = AE_ROUNDSP16SYM(a1);
#endif
        AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        a3 = AE_SELP24_HL(b0, b2);
        re = AE_MULZAAP24S_HH_LL(tw3, a3);
        im = AE_MULZASP24S_HL_LH(tw3, a3);
        a3 = AE_TRUNCP24Q48X2(re, im);
        a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
        a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
        a3 = AE_ROUNDSP16SYM(a3);
#endif

        AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
    }

    // 到这里，128点复数FFT第1级的DIF的基4输出符合预期 2013.03.
    // 到这里，256点复数FFT第1级的DIF的基4输出符合预期 2013.03.
//  printf("The output of first-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }

    //-----------------------------------------------------------------
    // The second stage for DIF radix-4
    //-----------------------------------------------------------------
    // complex FFT128
    if (norm & 1)
    {
        incr = 32;
        decr = 254; // (-96+1-32)*2 = -254
        quarterN = 8;

        px0 = (VOS_INT16 *)x - 64;
        px1 = px0 + 16;
        px2 = px1 + 16;
        px3 = px2 + 16;
    }
    // complex FFT256
    else
    {
        incr = 64;
        decr = 510; // (-192+1-64)*2 = -510
        quarterN = 16;

        px0 = (VOS_INT16 *)x - 128;
        px1 = px0 + 32;
        px2 = px1 + 32;
        px3 = px2 + 32;
    }

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));

        // 同一级中的不同组数
        for (j = 0; j < 4; j++)
        {
            AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

#ifdef NORM_RADIX4
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);
            re = AE_MULZAAP24S_HH_LL(tw2, a2);
            im = AE_MULZASP24S_HL_LH(tw2, a2);
            a2 = AE_TRUNCP24Q48X2(re, im);
            a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif
            AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);
            re = AE_MULZAAP24S_HH_LL(tw1, a1);
            im = AE_MULZASP24S_HL_LH(tw1, a1);
            a1 = AE_TRUNCP24Q48X2(re, im);
            a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif
            AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);
            re = AE_MULZAAP24S_HH_LL(tw3, a3);
            im = AE_MULZASP24S_HL_LH(tw3, a3);
            a3 = AE_TRUNCP24Q48X2(re, im);
            a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
        }

        px0 -= decr;
        px1 -= decr;
        px2 -= decr;
        px3 -= decr;
    }

    // 到这里，复数FFT128第2级的DIF的基4输出符合预期, 2013.03.
    // 到这里，复数FFT256第2级的DIF的基4输出符合预期, 2013.03.
//  printf("The output of second-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }

    //-----------------------------------------------------------------
    // The third stage for DIF radix-4
    //-----------------------------------------------------------------
    // complex FFT128
    if (norm & 1)
    {
        incr = 8;
        //decr = 254; // (-120+1-8)*2 = -254
        quarterN = 2;

        px0 = (VOS_INT16 *)x - 16;
        px1 = px0 + 4;
        px2 = px1 + 4;
        px3 = px2 + 4;
    }
    // complex FFT256
    else
    {
        incr = 16;
        //decr = 510; // (-240+1-16)*2 = -510
        quarterN = 4;

        px0 = (VOS_INT16 *)x - 32;
        px1 = px0 + 8;
        px2 = px1 + 8;
        px3 = px2 + 8;
    }

    for (i = 0; i < quarterN; i++)
    {
        AE_LP16X2F_IU(tw2, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw1, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));
        AE_LP16X2F_IU(tw3, (const ae_p16x2s *)twd, 1*sizeof(VOS_INT32));

        // 同一级中的不同组数
        for (j = 0; j < 16; j++)
        {
            AE_LP16X2F_XU(a0, (ae_p16x2s *)px0, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a1, (ae_p16x2s *)px1, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a2, (ae_p16x2s *)px2, incr * sizeof(ae_p16x2s));
            AE_LP16X2F_XU(a3, (ae_p16x2s *)px3, incr * sizeof(ae_p16x2s));

#ifdef NORM_RADIX4
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_I(a0, (ae_p16x2s *)px0, 0*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);
            re = AE_MULZAAP24S_HH_LL(tw2, a2);
            im = AE_MULZASP24S_HL_LH(tw2, a2);
            a2 = AE_TRUNCP24Q48X2(re, im);
            a2 = AE_SLLISP24S(a2, 1);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif
            AE_SP16X2F_I(a2, (ae_p16x2s *)px2, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);
            re = AE_MULZAAP24S_HH_LL(tw1, a1);
            im = AE_MULZASP24S_HL_LH(tw1, a1);
            a1 = AE_TRUNCP24Q48X2(re, im);
            a1 = AE_SLLISP24S(a1, 1);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif
            AE_SP16X2F_I(a1, (ae_p16x2s *)px1, 0*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);
            re = AE_MULZAAP24S_HH_LL(tw3, a3);
            im = AE_MULZASP24S_HL_LH(tw3, a3);
            a3 = AE_TRUNCP24Q48X2(re, im);
            a3 = AE_SLLISP24S(a3, 1);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_I(a3, (ae_p16x2s *)px3, 0*sizeof(ae_p16x2s));
        }

        px0 -= decr;
        px1 -= decr;
        px2 -= decr;
        px3 -= decr;
    }

    // 到这里，复数FFT128第2级的DIF的基4输出符合预期, 2013.03.
    // 到这里，复数FFT256第2级的DIF的基4输出符合预期, 2013.03.
//  printf("The output of third-stage DIF FFT\n");
//    for (i = 0; i < N; i++)
//    {
//      printf("x[%3d] = %6d + %6di\n", i, x[2*i], x[2*i + 1]);
//    }


    //--------------------------------------------------------------------------
    // The last stage for radix-2 or radix-4
    //--------------------------------------------------------------------------
    px0 = (VOS_INT16 *)x - 2;
    py00 = (VOS_INT16 *)y;
    // 1个复数对应4字节，相当于2个VOS_INT16，32表示32个实数，即16个复数间隔
    py01 = py00 + 32;
    py02 = py01 + 32;
    py03 = py02 + 32;
    py04 = py03 + 32;
    py05 = py04 + 32;
    py06 = py05 + 32;
    py07 = py06 + 32;

    //--------------------------------------------------------------------------
    // last stage is RADIX2 !!!
    //--------------------------------------------------------------------------
    // complex FFT 128
    if (norm & 1)
    {
        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py00, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py04, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py01, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py05, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py02, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py06, offset*sizeof(ae_p16x2s));

            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 1);
            a1 = AE_SRAIP24(a1, 1);
#endif

            b0 = AE_ADDSP24S(a0, a1);
            b1 = AE_SUBSP24S(a0, a1);

#ifdef ROUND_ASYM_ENABLE
            b0 = AE_ROUNDSP16ASYM(b0);
            b1 = AE_ROUNDSP16ASYM(b1);
#endif

#ifdef ROUND_SYM_ENABLE
            b0 = AE_ROUNDSP16SYM(b0);
            b1 = AE_ROUNDSP16SYM(b1);
#endif

            AE_SP16X2F_X (b0, (ae_p16x2s *)py03, offset*sizeof(ae_p16x2s));
            AE_SP16X2F_X (b1, (ae_p16x2s *)py07, offset*sizeof(ae_p16x2s));
        }
    }

    //--------------------------------------------------------------------------
    // last stage is RADIX4 !!!
    //--------------------------------------------------------------------------
    // complex FFT 256
    else
    {
        py08 = py07 + 32;
        py09 = py08 + 32;
        py10 = py09 + 32;
        py11 = py10 + 32;
        py12 = py11 + 32;
        py13 = py12 + 32;
        py14 = py13 + 32;
        py15 = py14 + 32;

        for (i = 0; i < 16; i++)
        {
            offset = digperm[i];

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py00, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py08, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py04, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py12, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py01, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py09, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py05, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py13, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py02, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py10, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py06, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py14, offset*sizeof(ae_p16x2s));

            //----------------------------------------------------------------
            // Read eight inputs, and perform radix4 decomposition
            //----------------------------------------------------------------
            AE_LP16X2F_IU(a0, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a1, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a2, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));
            AE_LP16X2F_IU(a3, (ae_p16x2s *)px0, 1*sizeof(ae_p16x2s));

#ifdef NORM_LAST_STAGE
            a0 = AE_SRAIP24(a0, 2);
            a1 = AE_SRAIP24(a1, 2);
            a2 = AE_SRAIP24(a2, 2);
            a3 = AE_SRAIP24(a3, 2);
#endif

            b0 = AE_ADDSP24S(a0, a2);
            b2 = AE_ADDSP24S(a1, a3);
            b1 = AE_SUBSP24S(a0, a2);
            b3 = AE_SUBSP24S(a1, a3);

            a0 = AE_ADDSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a0 = AE_ROUNDSP16ASYM(a0);
#endif

#ifdef ROUND_SYM_ENABLE
            a0 = AE_ROUNDSP16SYM(a0);
#endif

            AE_SP16X2F_X(a0, (ae_p16x2s *)py03, offset*sizeof(ae_p16x2s));

            a2 = AE_SUBSP24S(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a2 = AE_ROUNDSP16ASYM(a2);
#endif

#ifdef ROUND_SYM_ENABLE
            a2 = AE_ROUNDSP16SYM(a2);
#endif

            AE_SP16X2F_X(a2, (ae_p16x2s *)py11, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_LH(b3, b3);
            b0 = AE_ADDSP24S(b1, a3);
            b2 = AE_SUBSP24S(b1, a3);
            a1 = AE_SELP24_HL(b2, b0);

#ifdef ROUND_ASYM_ENABLE
            a1 = AE_ROUNDSP16ASYM(a1);
#endif

#ifdef ROUND_SYM_ENABLE
            a1 = AE_ROUNDSP16SYM(a1);
#endif

            AE_SP16X2F_X(a1, (ae_p16x2s *)py07, offset*sizeof(ae_p16x2s));

            a3 = AE_SELP24_HL(b0, b2);

#ifdef ROUND_ASYM_ENABLE
            a3 = AE_ROUNDSP16ASYM(a3);
#endif

#ifdef ROUND_SYM_ENABLE
            a3 = AE_ROUNDSP16SYM(a3);
#endif

            AE_SP16X2F_X(a3, (ae_p16x2s *)py15, offset*sizeof(ae_p16x2s));
        }
    }
}

#endif

/*lint +e50*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




