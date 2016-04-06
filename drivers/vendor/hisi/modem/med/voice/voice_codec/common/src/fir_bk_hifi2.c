/* ------------------------------------------------------------------------ */
/*  IntegrIT, Ltd.   www.integrit.com, info@integrit.com                    */
/*                                                                          */
/*  NatureDSP Signal Processing Library                                     */
/*                                                                          */
/* This library contains copyrighted materials, trade secrets and other     */
/* proprietary information of IntegrIT, Ltd. This software is licensed for  */
/* use with Tensilica HiFi2 core only and must not be used for any other    */
/* processors and platforms. The license to use these sources was given to  */
/* Tensilica, Inc. under Terms and Condition of a Software License Agreement*/
/* between Tensilica, Inc. and IntegrIT, Ltd. Any third party must not use  */
/* this code in any form unless such rights are given by Tensilica, Inc.    */
/* Compiling, linking, executing or using this library in any form you      */
/* unconditionally accept these terms.                                      */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2010 IntegrIT, Limited.                           */
/*                          All Rights Reserved.                            */
/* ------------------------------------------------------------------------ */
/*
    NatureDSP_Signal library. FIR part
    C code with generic optimization
    Integrit, 2006-2010
*/

/*-------------------------------------------------------------------------
  Real FIR filter.
  Computes a real FIR filter (direct-form) using IR stored in vector h.
  The real data input is stored in vector x. The filter output result is
  stored in vector r. The filter calculates N output samples using M
  coefficients and requires last M+N-1 samples on the input.

  Input:
  x[N+M-1]  - input data. First in time corresponds to x[0], Q31
  h[M]      - filter coefficients in normal order, Q31
  N         - length of sample block
  M         - length of filter
  Output:
  y[N]      - output data, Q31

  Restriction:
  x,h,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
-------------------------------------------------------------------------*/
#include "hifi2_common.h"

static void fir_bk_x4 (
                    int16_t *pshwOut,
                    int16_t *pshwIn,
                    int16_t *pshwCoef,
                    int N,
                    int M)
{
    //
    // The block FIR function is defined as: out[i] = sum in0[i-j] * in1[j], i=0...N-1, j = 0...M-1
    //
    // This implemented in a form of outer and inner loop. The outer loop increments data
    // offset by 4. The inner loop calculates four convolutions:
    //          out[0] = sum in0[-j]*in1[j+offset+0]
    //          out[1] = sum in0[-j]*in1[j+offset+1]
    //          out[2] = sum in0[-j]*in1[j+offset+2]
    //          out[3] = sum in0[-j]*in1[j+offset+3]
    //
    //  unrolled by factor of two:
    //
    //          out[0] = sum {in0[-j_even]*in1[j_even+offset+0] + in0[-j_odd]*in1[j_odd+offset+0]}
    //          out[1] = sum {in0[-j_even]*in1[j_even+offset+1] + in0[-j_odd]*in1[j_odd+offset+1]}
    //          out[2] = sum {in0[-j_even]*in1[j_even+offset+2] + in0[-j_odd]*in1[j_odd+offset+2]}
    //          out[3] = sum {in0[-j_even]*in1[j_even+offset+3] + in0[-j_odd]*in1[j_odd+offset+3]}
    //
    //  the same but in vector form:
    //
    //          out[0] = sum (in0[-j_even],in0[-j_odd]) * (in1[offset+j_even+0],in1[offset+j_odd+0])
    //          out[1] = sum (in0[-j_even],in0[-j_odd]) * (in1[offset+j_even+1],in1[offset+j_odd+1])
    //          out[2] = sum (in0[-j_even],in0[-j_odd]) * (in1[offset+j_even+2],in1[offset+j_odd+2])
    //          out[3] = sum (in0[-j_even],in0[-j_odd]) * (in1[offset+j_even+3],in1[offset+j_odd+3])
    //
    //  the same with shorten notation:
    //
    //          out[0] = sum H[-j] * X0[j]      where Xi[j] = (in1[offset+j_even+i],in1[offset+j_odd+i]),
    //          out[1] = sum H[-j] * X1[j]            H[-j] = (in0[-j_even],in0[-j_odd])
    //          out[2] = sum H[-j] * X2[j]
    //          out[3] = sum H[-j] * X3[j]
    //
    //  Existing implementation unrolls inner loop by factor of four (factor two unrolled twice by j):
    //
    //          out[0] = sum H[-j] * X0[j] + H[-j-2] * X0[j+2]
    //          out[1] = sum H[-j] * X1[j] + H[-j-2] * X1[j+2]
    //          out[2] = sum H[-j] * X2[j] + H[-j-2] * X2[j+2]
    //          out[3] = sum H[-j] * X3[j] + H[-j-2] * X3[j+2]
    //
    //  This is follows from Xi data structure
    //
    //          | X0 |  | X2 |  | X4 |
    //          a0, a1, a2, a3, a4, a5, a6, ...
    //              | X1 |  | X3 |
    //
    //  that X0[j+2] = X2[j] and X_odd = pack(X_even_prev_low, X_even_next_hi),
    //  thus 4 times unroll has the form of:
    //
    //          out[0]  = sum H[-j-0] * X0[j]    where j = 0, 4, 8, ...
    //          out[1]  = sum H[-j-0] * X1[j]
    //          out[2]  = sum H[-j-0] * X2[j]
    //          out[3]  = sum H[-j-0] * X3[j]
    //          out[0] += sum H[-j-2] * X2[j]
    //          out[1] += sum H[-j-2] * X3[j]
    //          out[2] += sum H[-j-2] * X4[j]
    //          out[3] += sum H[-j-2] * X5[j]
    //

    int i, j;
    int16_t *pshwXTmp;

    ae_p24x2s x0, x1, x2;
    ae_q56s y0, y1, y2, y3 ;

    // iterate N/4 times 4个点并行计算
    for (i = 0; i < N>>2; i++)
    {
        y0 = AE_ZEROQ56();
        y1 = AE_ZEROQ56();
        y2 = AE_ZEROQ56();
        y3 = AE_ZEROQ56();

        pshwXTmp = pshwIn + 4*i;

        /* 加载待滤波信号 */
        x0 = *((ae_p16x2s *)pshwXTmp); pshwXTmp += 2;
        x2 = *((ae_p16x2s *)pshwXTmp); pshwXTmp += 2;

        x1 = AE_SELP24_LH(x0, x2); //x1.H = x0.L; x1.L = x2.H;

        for(j = 0; j < M>>2; j++)
        { // 9 cycles loop
            ae_p24x2s x3, x4, x5, x6, h0, h1;

            // load H[-j] and H[-j-2]
            h0 = *((ae_p16x2s *)&pshwCoef[M - 4*j - 2]);
            h1 = *((ae_p16x2s *)&pshwCoef[M - 4*j - 4]);

            // load X4[j], X6[j] and construct X3[j], X5[j]
            x4 = *((ae_p16x2s *)pshwXTmp); pshwXTmp += 2;
            x3 = AE_MOVPA24X2(AE_MOVAP24S_L(x2), AE_MOVAP24S_H(x4));
            x6 = *((ae_p16x2s *)pshwXTmp); pshwXTmp += 2;
            x5 = AE_SELP24_LH(x4, x6);

            // do multiplications
            AE_MULAAP24S_HL_LH(y0, x0, h0); // out[0] += H[-j-0] * X0[j]
            AE_MULAAP24S_HL_LH(y1, x1, h0); // out[1] += H[-j-0] * X1[j]
            AE_MULAAP24S_HL_LH(y2, x2, h0); // out[2] += H[-j-0] * X2[j]
            AE_MULAAP24S_HL_LH(y3, x3, h0); // out[3] += H[-j-0] * X3[j]
            AE_MULAAP24S_HL_LH(y0, x2, h1); // out[0] += H[-j-2] * X2[j]
            AE_MULAAP24S_HL_LH(y1, x3, h1); // out[1] += H[-j-2] * X3[j]
            AE_MULAAP24S_HL_LH(y2, x4, h1); // out[2] += H[-j-2] * X4[j]
            AE_MULAAP24S_HL_LH(y3, x5, h1); // out[3] += H[-j-2] * X5[j]

            // advance to next iter
            x0 = x4;
            x1 = x5;
            x2 = x6;
        }

   //     assert((M%2) == 0);
        // 滤波器系数长度除4的余数
        if(M&0x3)
        {
                ae_p24x2s x3, x4, h0;
            //    LD_P24X2F_IU(h0, ph0, -1);
                h0 = *((ae_p16x2s *)&pshwCoef[M - 4*j - 2]);
            //     LD_P24X2F_IU(x4, px0,  1);
                x4 = *((ae_p16x2s *)pshwXTmp);
                x3 = AE_MOVPA24X2(AE_MOVAP24S_L(x2), AE_MOVAP24S_H(x4));

                AE_MULAAP24S_HL_LH(y0, x0, h0);
                AE_MULAAP24S_HL_LH(y1, x1, h0);
                AE_MULAAP24S_HL_LH(y2, x2, h0);
                AE_MULAAP24S_HL_LH(y3, x3, h0);

                /* 滤波器系数长度为奇数 */
                if(M&0x1)
                {
                    h0 = *((ae_p16s *)&pshwCoef[M - 4*j - 3]);
                    AE_MULAP24S_LL(y0, x1, h0);
                    AE_MULAP24S_LL(y1, x2, h0);
                    AE_MULAP24S_LL(y2, x3, h0);
                    AE_MULAP24S_LL(y3, x4, h0);
                }
        }

        // output result
        // y0 << 1; 对称带饱和Round低32bit; y0[bit32 - bit47] -> y
        y0 = AE_SLLISQ56S(y0, 1);
        *((ae_p16s *)pshwOut) = AE_ROUNDSP16Q48SYM(y0);
        pshwOut++;

        y1 = AE_SLLISQ56S(y1, 1);
        *((ae_p16s *)pshwOut) = AE_ROUNDSP16Q48SYM(y1);
        pshwOut++;

        y2 = AE_SLLISQ56S(y2, 1);
        *((ae_p16s *)pshwOut) = AE_ROUNDSP16Q48SYM(y2);
        pshwOut++;

        y3 = AE_SLLISQ56S(y3, 1);
        *((ae_p16s *)pshwOut) = AE_ROUNDSP16Q48SYM(y3);
        pshwOut++;
    }
}


void fir_bk (
            int16_t *pshwOut,   // [N]
            int16_t *pshwIn,    // [M+N-1]
            int16_t *pshwCoef,  // [M]
            int N,
            int M)
{
    /* 4字节对齐 */
 //   if(CODEC_OpCheck4ByteAligned( (int32_t)pshwIn | (int32_t)pshwCoef )  )
    if(IS_ALIGN_4((int32_t)pshwIn | (int32_t)pshwCoef))
    {

        // process 4*n samples
        fir_bk_x4(pshwOut, pshwIn, pshwCoef, N, M);

        // process the rest
        if(N&0x3)
        {
            int16_t y_tmp[4];
            int offset = 4*(N>>2);

            fir_bk_x4(y_tmp, pshwIn + offset, pshwCoef, 4, M);

            pshwOut[offset] = y_tmp[0];

            /* 剩余第2个点 */
            if((N&0x3) > 1)
            {
                pshwOut[offset+1] = y_tmp[1];
            }

            /* 剩余第3个点 */
            if((N&0x3) > 2)
            {
                pshwOut[offset+2] = y_tmp[2];
            }
        }
    }
    else
    {

        // non-optimized implementation for non-aligned input
        int i, j;

        for (i = 0; i < N; i++)
        {
            ae_q56s y0 = AE_ZEROQ56();
            for(j = 0; j < M; j++)
            {
                ae_p24x2s x0 = *((ae_p16s *)&pshwIn[i+j]);
                ae_p24x2s h0 = *((ae_p16s *)&pshwCoef[M-1-j]);
                AE_MULAP24S_LL(y0, x0, h0);
            }
            y0 = AE_SLLISQ56S(y0, 1);
            *((ae_p16s *)&pshwOut[i]) = AE_ROUNDSP16Q48SYM(y0);
        }
    }
}
