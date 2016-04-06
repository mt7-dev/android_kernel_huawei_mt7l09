
 /******************************************************************************
  文 件 名   : amrwb_op_hifi.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2012年2月1日
  最近修改   :
  功能描述   : AMR-WB声码器优化时对ETSI基本运算函数的HIFI内联拓展
  函数列表   :
  修改历史   :
  1.日    期   : 2012年2月1日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/
#ifndef __AMRWB_OP_HIFI_H__
#define __AMRWB_OP_HIFI_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "codec_op_etsi.h"
#include  "codec_op_vec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef _MED_C89_
XT_INLINE Word16 shl_round_add_mult(Word32 swVar1, Word16 shwVar2, Word16 shwVar3)
{
    ae_q56s  q1;
    ae_p24x2s p1, p2, p3;

    /* L_shl */
    q1 = AE_CVTQ48A32S(swVar1);
    q1 = AE_SLLASQ56S(q1, 1);
    p2 = AE_CVTP24A16(shwVar2);
    q1 = AE_SATQ48S(q1);

    /* round */
    p1 = AE_ROUNDSP16Q48ASYM(q1);
    p3 = AE_CVTP24A16(shwVar3);

    /* mult */
    q1 = AE_MULFS32P16S_LL(p1, p2);
    p1 = AE_TRUNCP24Q48(q1);
    p1 = AE_ADDSP24S(p1, p3);

    return AE_TRUNCA16P24S_L(p1);
}

XT_INLINE Word16 add_mult(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1 + p2 --> p3 24bit饱和加 */
    p1 = AE_ADDSP24S(p1, p2);

    /* p1(1.24)*p2(1.24)*2 --> q1 */
    q1 = AE_MULFS32P16S_LL(p1, p1);

    /* q1(25.48) --> p1(1.24) */
    p1 = AE_TRUNCP24Q48(q1);

    return AE_TRUNCA16P24S_L(p1);
}

XT_INLINE Word16 L_mac_mac_extract(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2, p3, p4;
    ae_q56s     q1;
    Word32 tmp;
    Word16 shwTmp;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(4096);

    /* L_var3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar2);
    p2 = AE_CVTP24A16(8192);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    p1 = AE_TRUNCP24Q48(q1);
    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);
}

XT_INLINE Word16 L_shl_round(Word32 swVar1, Word16 shwVar2)
{
    ae_q56s  q1;
    ae_p24x2s  p1;
    Word32 L_rounded;

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    /* q1做56bit带饱和左移 --> q2 */
    q1 = AE_SLLASQ56S(q1, shwVar2);

    /* q1做48比特饱和处理 */
    q1 = AE_SATQ48S(q1);

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1*/
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);
}

XT_INLINE Word16 L_mult_mac_shl_extract(Word16 shwVar1, Word16 shwVar2, Word16 shwVar3, Word16 shwVar4, Word16 shwVar5)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1, q2;
    Word32 tmp;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1(1.24)*p2(1.24)*2 --> q1 */
    q1 = AE_MULFS32P16S_LL(p1, p2);

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar3);
    p2 = AE_CVTP24A16(shwVar4);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* q1做56bit带饱和左移 --> q2 */
    q2 = AE_SLLASQ56S(q1, shwVar5);

    /* q2做48比特饱和处理 */
    q2 = AE_SATQ48S(q2);

    p1 = AE_TRUNCP24Q48(q2);

    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);
}

XT_INLINE Word16 L_deposit_shl_round(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1;
    ae_q56s  q1;
    Word32	swVar1;


    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    /* p1(9.24)-->swVarOut(17.32); 0-->swVarOut(1.16) */
    swVar1 = AE_CVTA32P24_H(p1);

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 32 ? 32 : shwVar2;

        /* q1做56bit带饱和左移 --> q2 */
        q1 = AE_SLLASQ56S(q1, shwVar2);

        /* q2做48比特饱和处理 */
        q1 = AE_SATQ48S(q1);
    }
    else
    {
        /* q1做56bit算术右移 --> q2 */
        q1 = AE_SRAAQ56(q1, -shwVar2);
    }

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1*/
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);

}

XT_INLINE Word32 Mpy_mac_mac_shl_msu (Word16 shwHi, Word16 shwLo, Word16 shwN,
                    Word16 shwVar1, Word16 shwVar2, Word16 shwVar3, Word16 shwVar4,
                    Word16 shwVar5, Word16 shwVar6, Word16 shwVar7)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1, q2;

    p1 = AE_CVTP24A16(shwHi);
    p2 = AE_CVTP24A16(shwN);
    q1 = AE_MULFS32P16S_LL(p1, p2);

    p1 = AE_CVTP24A16(shwLo);
    q2 = AE_MULFS32P16S_LL(p1, p2);

    p2 = AE_TRUNCP24Q48(q2);
    p1 = AE_CVTP24A16(1);
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar3);
    p2 = AE_CVTP24A16(shwVar4);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* q1做56bit带饱和左移 --> q2 */
    q1 = AE_SLLASQ56S(q1, shwVar5);

    /* q2做48比特饱和处理 */
    q1 = AE_SATQ48S(q1);

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar6);
    p2 = AE_CVTP24A16(shwVar7);

    /* q1-p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULSFS32P16S_LL(q1, p1, p2);

    /* q1(17.48) --> swVarOut */
    return  AE_TRUNCA32Q48(q1);
}

XT_INLINE Word32 sub_mac(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    q1 = AE_CVTQ48A32S(swVar3);
    p1 = AE_SUBSP24S(p1, p2);

    AE_MULAFS32P16S_LL(q1, p1, p1);

    return AE_TRUNCA32Q48(q1);
}

XT_INLINE void AMRWB_OpVvvvAdd(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word16 *pshwP,
                const Word16 *pshwQ,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2, aepW1, aepW2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            aepX1 = *((ae_p16x2s *)&pshwP[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwP[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwQ[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwQ[4*k + 2]);

            aepW1 = AE_ADDSP24S(aepX1, aepY1);
            aepW2 = AE_ADDSP24S(aepX2, aepY2);

            aepZ1 = AE_ADDSP24S(aepW1, aepZ1);
            aepZ2 = AE_ADDSP24S(aepW2, aepZ2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);
            aepZ1 = AE_ADDSP24S(aepX1, aepY1);

            aepX1 = *((ae_p16x2s *)&pshwP[k]);
            aepY1 = *((ae_p16x2s *)&pshwQ[k]);
            aepW1 = AE_ADDSP24S(aepX1, aepY1);

            aepZ1 = AE_ADDSP24S(aepZ1, aepW1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            aepX1 = *((ae_p16s *)&pshwP[2*k]);
            aepX2 = *((ae_p16s *)&pshwP[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwQ[2*k]);
            aepY2 = *((ae_p16s *)&pshwQ[2*k + 1]);

            aepW1 = AE_ADDSP24S(aepX1, aepY1);
            aepW2 = AE_ADDSP24S(aepX2, aepY2);

            aepZ1 = AE_ADDSP24S(aepZ1, aepW1);
            aepZ2 = AE_ADDSP24S(aepZ2, aepW2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);
        aepZ1 = AE_ADDSP24S(aepX1, aepY1);

        aepX1 = *((ae_p16s *)&pshwP[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwQ[swLen - 1]);
        aepW1 = AE_ADDSP24S(aepX1, aepY1);

        aepZ1 = AE_ADDSP24S(aepZ1, aepW1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}

XT_INLINE Word32 AMRWB_OpVvMacSub(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;
    ae_q56s aeqS1;

    if(swLen < 0)
    {
        return;
    }

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY2);

            AE_MULAAFP24S_HH_LL(aeqS1, aepZ1, aepZ1);
            AE_MULAAFP24S_HH_LL(aeqS1, aepZ2, aepZ2);

            /* 饱和到48bit */
            aeqS1 = AE_SATQ48S(aeqS1);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);

            AE_MULAAFP24S_HH_LL(aeqS1, aepZ1, aepZ1);
        }

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY2);

            AE_MULAFS32P16S_LL(aeqS1, aepZ1, aepZ1);
            AE_MULAFS32P16S_LL(aeqS1, aepZ2, aepZ2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        aepZ1 = AE_SUBSP24S(aepX1, aepY1);

        AE_MULAFS32P16S_LL(aeqS1, aepZ1, aepZ1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

#else

XT_INLINE Word16 shl_round_add_mult(Word32 swVar1, Word16 shwVar2, Word16 shwVar3)
{
    Word32 L_sum;
    Word16 corr, cor;

    L_sum = L_shl(swVar1, 1);

    corr = round(L_sum);

    cor = add(mult(corr, shwVar2), shwVar3);

    return cor;
}

XT_INLINE Word16 add_mult(Word16 shwVar1, Word16 shwVar2)
{
    Word16 ps2, sq;

    ps2 = add(shwVar1, shwVar2);
    sq = mult(ps2, ps2);

    return sq;
}

XT_INLINE Word16 L_mac_mac_extract(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    Word32 alp2;
    Word16 alp_16;

    alp2 = L_mac(swVar3, shwVar1, 4096);
    alp2 = L_mac(alp2, shwVar2, 8192);
    alp_16 = extract_h(alp2);

    return alp_16;
}

XT_INLINE Word16 L_shl_round(Word32 swVar1, Word16 shwVar2)
{
    Word32 s;
    Word16 y;

    s = L_shl(swVar1, shwVar2);               /* saturation can occur here */
    y = round(s);

    return y;
}

XT_INLINE Word16 L_mult_mac_shl_extract(Word16 shwVar1, Word16 shwVar2, Word16 shwVar3, Word16 shwVar4, Word16 shwVar5)
{
    Word32 s;
    Word16 dn2;

    s = L_mac(L_mult(shwVar1, shwVar2), shwVar3, shwVar4);
    dn2 = extract_h(L_shl(s, shwVar5));

    return dn2;
}

XT_INLINE Word16 L_deposit_shl_round(Word16 shwVar1, Word16 shwVar2)
{
    Word32 L_tmp;
    Word16 x;

    L_tmp = L_deposit_h(shwVar1);
    L_tmp = L_shl(L_tmp, shwVar2);         /* saturation can occur here */
    x = round(L_tmp);

    return x;

}

XT_INLINE Word32 Mpy_mac_mac_shl_msu (Word16 shwHi, Word16 shwLo, Word16 shwN,
                    Word16 shwVar1, Word16 shwVar2, Word16 shwVar3, Word16 shwVar4,
                    Word16 shwVar5, Word16 shwVar6, Word16 shwVar7)
{
    Word32 t0;

    t0 = Mpy_32_16(shwHi, shwLo, shwN);     /* t0 = 2.0*x*b1              */

    t0 = L_mac(t0, shwVar1, shwVar2);
    t0 = L_mac(t0, shwVar3, shwVar4);
    t0 = L_shl(t0, shwVar5);
    t0 = L_msu(t0, shwVar6, shwVar7);           /* t0 = 2.0*x*b1 - b2 + f[i]; */

    return t0;
}

XT_INLINE void AMRWB_OpVvvvAdd(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word16 *pshwP,
                const Word16 *pshwQ,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(add(add(*pshwX++, *pshwY++), *pshwP++), *pshwQ++);
    }
}

XT_INLINE Word32 AMRWB_OpVvMacSub(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i;
    Word16 tmp;

    for (i = 0; i < swLen; i++)
    {
        tmp = sub(pshwX[i], pshwY[i]);
        swSum = L_mac(swSum, tmp, tmp);
    }

    return swSum;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif

