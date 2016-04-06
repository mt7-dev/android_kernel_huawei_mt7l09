/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#ifdef _MED_C89_
#include <stdio.h>
#endif

#include "fr_etsi_op.h"
#include "fr_long_term.h"
#include "fr_table.h"
#include "codec_op_vec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Prevent improper operation for 16-bit systems */
#if defined(MSDOS) || defined(__MSDOS__)
#ifdef USE_TABLE_MUL
#undef USE_TABLE_MUL
#endif
#endif

#ifdef USE_TABLE_MUL

unsigned int umul_table[ 513 ][ 256 ];

init_umul_table()
{
    int i, j;
    int n;
    unsigned int     * p = &umul_table[0][0];

    for (i = 0; i < 513; i++) {
        n = 0;
        for (j = 0; j < 256; j++) {
            *p++ = n;
            n += i;
        }
    }
}

#define umul(x9, x15)  \
    ((int)(umul_table[x9][x15 & 0x0FF] + (umul_table[x9][ x15 >> 8 ] << 8)))

#define table_mul(a, b)    \
    ( (a < 0)  ? ((b < 0) ? umul(-a, -b) : -umul(-a, b))    \
           : ((b < 0) ? -umul(a, -b) :  umul(a, b)))

#endif /* USE_TABLE_MUL */



/*
 *  4.2.11 .. 4.2.12 LONG TERM PREDICTOR (LTP) SECTION
 */


/*
 * This procedure computes the LTP gain (bc) and the LTP lag (Nc)
 * for the long term analysis filter.   This is done by calculating a
 * maximum of the cross-correlation function between the current
 * sub-segment short term residual signal d[0..39] (output of
 * the short term analysis filter; for simplification the index
 * of this array begins at 0 and ends at 39 for each sub-segment of the
 * RPE-LTP analysis) and the previous reconstructed short term
 * residual signal dp[ -120 .. -1 ].  A dynamic scaling must be
 * performed to avoid overflow.
 */

 /* This procedure exists in four versions.  First, the two integer
  * versions with or without table-multiplication (as one function);
  * then, the two floating point versions (as another function), with
  * or without scaling.
  */

#ifndef  USE_FLOAT_MUL

#ifndef _MED_C89_
/*****************************************************************************
 函 数 名  : FR_OpVvMac0
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月25日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline Word32 FR_OpVvMac0(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    if(swLen < 0)
    {
        return swSum;
    }

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned(pshwX) && CODEC_OpCheck4ByteAligned(pshwY))
    {

        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            AE_MULAAP24S_HH_LL(aeqS1, aepX1, aepY1);
            AE_MULAAP24S_HH_LL(aeqS1, aepX2, aepY2);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            AE_MULAAP24S_HH_LL(aeqS1, aepX1, aepY1);
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

            AE_MULAS56P24S_LL(aeqS1, aepX1, aepY1);
            AE_MULAS56P24S_LL(aeqS1, aepX2, aepY2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        AE_MULAS56P24S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : FR_OpVecScaleMac0
 功能描述  : temp = shr(X[i], bits) ， Bits>=0
             sum = L_mac(sum, temp, temp)
 输入参数  : const Word16 *pshwX    - 向量
             const Word32 swLen     - 向量长度
             Word32 swSum           - 累加和初始值
             Word16 shwBits         - 缩放右移位数
 输出参数  : 无
 返 回 值  : Word32  累加结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月25日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
static inline Word32 FR_OpVecScaleMac0(
                const Word16 *pshwX,
                const Word32  swLen,
                Word32        swSum,
                Word16        shwBits)
{
    Word32    k = 0;

    ae_p24x2s aepX1, aepX2;
    ae_q56s   aeqS1;

    if(swLen < 0)
    {
        return swSum;
    }

    /* 限制右移比特数在 0到16 */
    shwBits = shwBits < 16 ? shwBits : 16;

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(shwBits);

    /* swSum --> aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepX1 = AE_SRASP24(aepX1);
            aepX1 = AE_TRUNCP16(aepX1);

            aepX2 = AE_SRASP24(aepX2);
            aepX2 = AE_TRUNCP16(aepX2);

            AE_MULAAP24S_HH_LL(aeqS1, aepX1, aepX1);
            AE_MULAAP24S_HH_LL(aeqS1, aepX2, aepX2);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aepX1 = AE_SRASP24(aepX1);
            aepX1 = AE_TRUNCP16(aepX1);

            AE_MULAAP24S_HH_LL(aeqS1, aepX1, aepX1);
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

            aepX1 = AE_SRASP24(aepX1);
            aepX1 = AE_TRUNCP16(aepX1);
            aepX2 = AE_SRASP24(aepX2);
            aepX2 = AE_TRUNCP16(aepX2);

            AE_MULAP24S_LL(aeqS1, aepX1, aepX1);
            AE_MULAP24S_LL(aeqS1, aepX2, aepX2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aepX1 = AE_SRASP24(aepX1);
        aepX1 = AE_TRUNCP16(aepX1);

        AE_MULAP24S_LL(aeqS1, aepX1, aepX1);
    }

    return AE_TRUNCA32Q48(aeqS1);

}
#endif

static void Calculation_of_the_LTP_parameters(
     Word16   * d,        /* [0..39]  IN  */
     Word16   * dp,       /* [-120..-1]   IN  */
    Word16        * bc_out,   /*      OUT */
    Word16        * Nc_out    /*      OUT */
)
{
    Word32        k, lambda;
    Word16        Nc, bc;
    Word16        wt[40];

    Word32    L_max, L_power;
    Word16        R, S, dmax, scal;
     Word16   temp;

    /*  Search of the optimum scaling of d[0..39].
     */
    dmax = CODEC_OpVecMaxAbs(d, 40, 0);

    temp = 0;
    if (dmax == 0) scal = 0;
    else {
        temp = gsm_norm( (Word32)dmax << 16 );
    }

    if (temp > 6) scal = 0;
    else scal = 6 - temp;


    /*  Initialization of a working array wt
     */
    CODEC_OpVecShr(&d[0], 40, scal, &wt[0]);

    /* Search for the maximum cross-correlation and coding of the LTP lag
     */
    L_max = 0;
    Nc    = 40; /* index for the maximum cross-correlation */

    for (lambda = 40; lambda <= 120; lambda++)
    {
       Word32 L_result;

#ifdef _MED_C89_
#ifdef STEP
#undef STEP
#endif

#ifdef USE_TABLE_MUL
#define STEP(k) (table_mul(wt[k], dp[k - lambda]))
#else
#define STEP(k) (wt[k] * dp[k - lambda])
#endif

        L_result  = STEP(0)  ; L_result += STEP(1) ;
        L_result += STEP(2)  ; L_result += STEP(3) ;
        L_result += STEP(4)  ; L_result += STEP(5)  ;
        L_result += STEP(6)  ; L_result += STEP(7)  ;
        L_result += STEP(8)  ; L_result += STEP(9)  ;
        L_result += STEP(10) ; L_result += STEP(11) ;
        L_result += STEP(12) ; L_result += STEP(13) ;
        L_result += STEP(14) ; L_result += STEP(15) ;
        L_result += STEP(16) ; L_result += STEP(17) ;
        L_result += STEP(18) ; L_result += STEP(19) ;
        L_result += STEP(20) ; L_result += STEP(21) ;
        L_result += STEP(22) ; L_result += STEP(23) ;
        L_result += STEP(24) ; L_result += STEP(25) ;
        L_result += STEP(26) ; L_result += STEP(27) ;
        L_result += STEP(28) ; L_result += STEP(29) ;
        L_result += STEP(30) ; L_result += STEP(31) ;
        L_result += STEP(32) ; L_result += STEP(33) ;
        L_result += STEP(34) ; L_result += STEP(35) ;
        L_result += STEP(36) ; L_result += STEP(37) ;
        L_result += STEP(38) ; L_result += STEP(39) ;
#else
        L_result = FR_OpVvMac0(&wt[0], &dp[-lambda], 40, 0);
#endif
        if (L_result > L_max)
        {
            Nc    = lambda;
            L_max = L_result;
        }
    }

    *Nc_out = Nc;

    L_max <<= 1;

    /*  Rescaling of L_max
     */
    L_max = L_max >> (6 - scal);    /* sub(6, scal) */


    /*   Compute the power of the reconstructed short term residual
     *   signal dp[..]
     */
#ifdef _MED_C89_
    L_power = 0;
    for (k = 0; k <= 39; k++)
    {
         Word32 L_temp;

        L_temp   = SASR( (Word32)(dp[k - Nc]), 3 );
        L_power += L_temp * L_temp;
    }
#else
    L_power = FR_OpVecScaleMac0(&dp[0 - Nc], 40, 0, 3);
#endif

    L_power <<= 1;  /* from L_MULT */

    /*  Normalization of L_max and L_power
     */

    if (L_max <= 0)  {
        *bc_out = 0;
        return;
    }
    if (L_max >= L_power) {
        *bc_out = 3;
        return;
    }

    temp = gsm_norm( L_power );

    R = SASR( L_max   << temp, 16 );
    S = SASR( L_power << temp, 16 );

    /*  Coding of the LTP gain
     */

    /*  Table 4.3a must be used to obtain the level DLB[i] for the
     *  quantization of the LTP gain b to get the coded version bc.
     */
    for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
    *bc_out = bc;
}

#else   /* USE_FLOAT_MUL */

static void Calculation_of_the_LTP_parameters(
     Word16   * d,        /* [0..39]  IN  */
     Word16   * dp,       /* [-120..-1]   IN  */
    Word16        * bc_out,   /*      OUT */
    Word16        * Nc_out    /*      OUT */
)
{
  /*     UWord32 utmp;    / * for L_ADD */

     int    k, lambda;
    Word16        Nc, bc;

    float       wt_float[40];
    float       dp_float_base[120], * dp_float = dp_float_base + 120;

    Word32    L_max, L_power;
    Word16        R, S, dmax, scal;
     Word16   temp;

    /*  Search of the optimum scaling of d[0..39].
     */
    dmax = 0;

    for (k = 0; k <= 39; k++) {
        temp = d[k];
        temp = GSM_ABS( temp );
        if (temp > dmax) dmax = temp;
    }

    temp = 0;
    if (dmax == 0) scal = 0;
    else {
        temp = gsm_norm( (Word32)dmax << 16 );
    }

    if (temp > 6) scal = 0;
    else scal = 6 - temp;


    /*  Initialization of a working array wt
     */

    for (k =    0; k < 40; k++) wt_float[k] =  SASR( d[k], scal );
    for (k = -120; k <  0; k++) dp_float[k] =  dp[k];

    /* Search for the maximum cross-correlation and coding of the LTP lag
     */
    L_max = 0;
    Nc    = 40; /* index for the maximum cross-correlation */

    for (lambda = 40; lambda <= 120; lambda += 9) {

        /*  Calculate L_result for l = lambda .. lambda + 9.
         */
         float *lp = dp_float - lambda;

         float  W;
         float  a = lp[-8], b = lp[-7], c = lp[-6],
                d = lp[-5], e = lp[-4], f = lp[-3],
                g = lp[-2], h = lp[-1];
         float  E;
         float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
                S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#ifdef STEP
#undef STEP
#endif

#define  STEP(K, a, b, c, d, e, f, g, h) \
            W = wt_float[K];        \
            E = W * a; S8 += E;     \
            E = W * b; S7 += E;     \
            E = W * c; S6 += E;     \
            E = W * d; S5 += E;     \
            E = W * e; S4 += E;     \
            E = W * f; S3 += E;     \
            E = W * g; S2 += E;     \
            E = W * h; S1 += E;     \
            a  = lp[K];         \
            E = W * a; S0 += E

#define  STEP_A(K)   STEP(K, a, b, c, d, e, f, g, h)
#define  STEP_B(K)   STEP(K, b, c, d, e, f, g, h, a)
#define  STEP_C(K)   STEP(K, c, d, e, f, g, h, a, b)
#define  STEP_D(K)   STEP(K, d, e, f, g, h, a, b, c)
#define  STEP_E(K)   STEP(K, e, f, g, h, a, b, c, d)
#define  STEP_F(K)   STEP(K, f, g, h, a, b, c, d, e)
#define  STEP_G(K)   STEP(K, g, h, a, b, c, d, e, f)
#define  STEP_H(K)   STEP(K, h, a, b, c, d, e, f, g)

        STEP_A( 0);
        STEP_B( 1);
        STEP_C( 2);
        STEP_D( 3);
        STEP_E( 4); STEP_F( 5); STEP_G( 6); STEP_H( 7);

        STEP_A( 8); STEP_B( 9); STEP_C(10); STEP_D(11);
        STEP_E(12); STEP_F(13); STEP_G(14); STEP_H(15);

        STEP_A(16); STEP_B(17); STEP_C(18); STEP_D(19);
        STEP_E(20); STEP_F(21); STEP_G(22); STEP_H(23);

        STEP_A(24); STEP_B(25); STEP_C(26); STEP_D(27);
        STEP_E(28); STEP_F(29); STEP_G(30); STEP_H(31);

        STEP_A(32); STEP_B(33); STEP_C(34); STEP_D(35);
        STEP_E(36); STEP_F(37); STEP_G(38); STEP_H(39);

        if (S0 > L_max) { L_max = S0; Nc = lambda;     }
        if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
        if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
        if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
        if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
        if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
        if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
        if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
        if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
    }
    *Nc_out = Nc;

    L_max <<= 1;

    /*  Rescaling of L_max
     */
    L_max = L_max >> (6 - scal);    /* sub(6, scal) */


    /*   Compute the power of the reconstructed short term residual
     *   signal dp[..]
     */
    L_power = 0;
    for (k = 0; k <= 39; k++) {

         Word32 L_temp;

        L_temp   = SASR( dp[k - Nc], 3 );
        L_power += L_temp * L_temp;
    }
    L_power <<= 1;  /* from L_MULT */

    /*  Normalization of L_max and L_power
     */

    if (L_max <= 0)  {
        *bc_out = 0;
        return;
    }
    if (L_max >= L_power) {
        *bc_out = 3;
        return;
    }

    temp = gsm_norm( L_power );

    R = SASR( L_max   << temp, 16 );
    S = SASR( L_power << temp, 16 );

    /*  Coding of the LTP gain
     */

    /*  Table 4.3a must be used to obtain the level DLB[i] for the
     *  quantization of the LTP gain b to get the coded version bc.
     */
    for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
    *bc_out = bc;
}

#ifdef  FAST

static void Fast_Calculation_of_the_LTP_parameters(
     Word16   * d,        /* [0..39]  IN  */
     Word16   * dp,       /* [-120..-1]   IN  */
    Word16        * bc_out,   /*      OUT */
    Word16        * Nc_out    /*      OUT */
)
{
     UWord32 utmp;    /* for L_ADD */

     int    k, lambda;
    Word16        Nc, bc;

    float       wt_float[40];
    float       dp_float_base[120], * dp_float = dp_float_base + 120;

     float  L_max, L_power;

    for (k = 0; k < 40; ++k) wt_float[k] = (float)d[k];
    for (k = -120; k <= 0; ++k) dp_float[k] = (float)dp[k];

    /* Search for the maximum cross-correlation and coding of the LTP lag
     */
    L_max = 0;
    Nc    = 40; /* index for the maximum cross-correlation */

    for (lambda = 40; lambda <= 120; lambda += 9) {

        /*  Calculate L_result for l = lambda .. lambda + 9.
         */
         float *lp = dp_float - lambda;

         float  W;
         float  a = lp[-8], b = lp[-7], c = lp[-6],
                d = lp[-5], e = lp[-4], f = lp[-3],
                g = lp[-2], h = lp[-1];
         float  E;
         float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
                S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#ifdef STEP
#undef STEP
#endif

#define  STEP(K, a, b, c, d, e, f, g, h) \
            W = wt_float[K];        \
            E = W * a; S8 += E;     \
            E = W * b; S7 += E;     \
            E = W * c; S6 += E;     \
            E = W * d; S5 += E;     \
            E = W * e; S4 += E;     \
            E = W * f; S3 += E;     \
            E = W * g; S2 += E;     \
            E = W * h; S1 += E;     \
            a  = lp[K];         \
            E = W * a; S0 += E

#define  STEP_A(K)   STEP(K, a, b, c, d, e, f, g, h)
#define  STEP_B(K)   STEP(K, b, c, d, e, f, g, h, a)
#define  STEP_C(K)   STEP(K, c, d, e, f, g, h, a, b)
#define  STEP_D(K)   STEP(K, d, e, f, g, h, a, b, c)
#define  STEP_E(K)   STEP(K, e, f, g, h, a, b, c, d)
#define  STEP_F(K)   STEP(K, f, g, h, a, b, c, d, e)
#define  STEP_G(K)   STEP(K, g, h, a, b, c, d, e, f)
#define  STEP_H(K)   STEP(K, h, a, b, c, d, e, f, g)

        STEP_A( 0); STEP_B( 1); STEP_C( 2); STEP_D( 3);
        STEP_E( 4); STEP_F( 5); STEP_G( 6); STEP_H( 7);

        STEP_A( 8); STEP_B( 9); STEP_C(10); STEP_D(11);
        STEP_E(12); STEP_F(13); STEP_G(14); STEP_H(15);

        STEP_A(16); STEP_B(17); STEP_C(18); STEP_D(19);
        STEP_E(20); STEP_F(21); STEP_G(22); STEP_H(23);

        STEP_A(24); STEP_B(25); STEP_C(26); STEP_D(27);
        STEP_E(28); STEP_F(29); STEP_G(30); STEP_H(31);

        STEP_A(32); STEP_B(33); STEP_C(34); STEP_D(35);
        STEP_E(36); STEP_F(37); STEP_G(38); STEP_H(39);

        if (S0 > L_max) { L_max = S0; Nc = lambda;     }
        if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
        if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
        if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
        if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
        if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
        if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
        if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
        if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
    }
    *Nc_out = Nc;

    if (L_max <= 0.)  {
        *bc_out = 0;
        return;
    }

    /*  Compute the power of the reconstructed short term residual
     *  signal dp[..]
     */
    dp_float -= Nc;
    L_power = 0;
    for (k = 0; k < 40; ++k) {
         float f = dp_float[k];
        L_power += f * f;
    }

    if (L_max >= L_power) {
        *bc_out = 3;
        return;
    }

    /*  Coding of the LTP gain
     *  Table 4.3a must be used to obtain the level DLB[i] for the
     *  quantization of the LTP gain b to get the coded version bc.
     */
    lambda = L_max / L_power * 32768.;
    for (bc = 0; bc <= 2; ++bc) if (lambda <= gsm_DLB[bc]) break;
    *bc_out = bc;
}

#endif  /* FAST      */
#endif  /* USE_FLOAT_MUL */


/* 4.2.12 */

static void Long_term_analysis_filtering(
    Word16        bc, /*                  IN  */
    Word16        Nc, /*                  IN  */
     Word16   * dp,   /* previous d   [-120..-1]      IN  */
     Word16   * d,    /* d        [0..39]         IN  */
     Word16   * dpp,  /* estimate [0..39]         OUT */
     Word16   * e /* long term res. signal [0..39]    OUT */
)
/*
 *  In this part, we have to decode the bc parameter to compute
 *  the samples of the estimate dpp[0..39].  The decoding of bc needs the
 *  use of table 4.3b.  The long term residual signal e[0..39]
 *  is then calculated to be fed to the RPE encoding section.
 */
{
  /*     Word16     bp; Was reported as unused */
     int      k;

#ifdef STEP
#undef STEP
#endif
/*
#define STEP(BP)        \
    for (k = 0; k <= 39; k++) {         \
        dpp[k]  = GSM_MULT_R( BP, dp[k - Nc]);  \
        e[k]    = GSM_SUB( d[k], dpp[k] );  \
    }
*/

#define STEP(BP)        \
    for (k = 0; k <= 39; k++) {         \
        dpp[k]  = GSM_MULT_R( BP, dp[k - Nc]);  \
    }\
    CODEC_OpVvSub(&d[0], &dpp[0], 40, &e[0]);

    switch (bc) {
    case 0: STEP(  3277 ); break;
    case 1: STEP( 11469 ); break;
    case 2: STEP( 21299 ); break;
    case 3: STEP( 32767 ); break;
    }
}

void Gsm_Long_Term_Predictor(
    Word8    fast,
    Word16    * d,    /* [0..39]   residual signal    IN  */
    Word16    * dp,   /* [-120..-1] d'        IN  */
    Word16    * e,    /* [0..39]          OUT */
    Word16    * dpp,  /* [0..39]          OUT */
    Word16    * Nc,   /* correlation lag      OUT */
    Word16    * bc    /* gain factor          OUT */
)
{
#if defined(FAST) && defined(USE_FLOAT_MUL)
    if (fast)
        Fast_Calculation_of_the_LTP_parameters( d, dp, bc, Nc );
    else
#endif
        Calculation_of_the_LTP_parameters( d, dp, bc, Nc );

    Long_term_analysis_filtering( *bc, *Nc, dp, d, dpp, e );
}

/* 4.3.2 */
void Gsm_Long_Term_Synthesis_Filtering(
    Word16            *pshwNrp,
    Word16            Ncr,
    Word16            bcr,
     Word16       * erp,      /* [0..39]        IN */
     Word16       * drp       /* [-120..-1] IN, [0..40] OUT */
)
/*
 *  This procedure uses the bcr and Ncr parameter to realize the
 *  long term synthesis filtering.  The decoding of bcr needs
 *  table 4.3b.
 */
{
     int        k;
    Word16            brp, drpp, Nr;

    /*  Check the limits of Nr.
     */
    Nr = Ncr < 40 || Ncr > 120 ? *pshwNrp : Ncr;
    *pshwNrp = Nr;

    /*  Decoding of the LTP gain bcr
     */
    brp = gsm_QLB[ bcr ];

    /*  Computation of the reconstructed short term residual
     *  signal drp[0..39]
     */

    for (k = 0; k <= 39; k++) {
        drpp   = GSM_MULT_R( brp, drp[ k - Nr ] );
        drp[k] = GSM_ADD( erp[k], drpp );
    }

    /*
     *  Update of the reconstructed short term residual signal
     *  drp[ -1..-120 ]
     */

    for (k = 0; k <= 119; k++) drp[ -120 + k ] = drp[ -80 + k ];
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

