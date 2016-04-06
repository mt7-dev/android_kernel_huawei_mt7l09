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
    NatureDSP_Signal library. VEC part
    C code with generic optimization
    Integrit, 2006-2010
*/

/*-------------------------------------------------------------------------
  This routine performs pair wise division of vectors written in Q31 format.
  It returns the fractional and exponential portion of the division result.
  Since the division may generate result greater than 1, it returns
  fractional portion frac in Q(31-exp) format and exponent exp so true
  division result in the Q0.31 may be found by shifting fractional part
  left by exponent value.
  For division to 0, the result is not defined

  Accuracy is 10LSB (5*10-7) of fractional part

  Input:
  x[N]      - input data (dividend).
  y[N]      - input data (divisor).
  N         - length of vectors
  Output:
  frac[N]   - fractional part of result
  exp[N]    - exponent part of result
  returned value:
  none

  Restrictions:
  1. x,y,frac,exp should not be overlapping
  2. divisors should not be zero (otherwise result is not defined)


  PERFORMANCE NOTE:
  for optimum performance follow rules:
  frac,x,y   - should be aligned by 8 bytes
  N   - divisible by 2 and >2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
#include "hifi2_common.h"
// Newton-rafson
// tabulated 1/x in range 0.5...1 in steps 1/256
const int32_t tab_invQ30[128] = {
    0x7f807f80, 0x7e8472a8, 0x7d8c42b2, 0x7c97d910, 0x7ba71fe1, 0x7aba01ea, 0x79d06a96, 0x78ea45e7,
    0x78078078, 0x77280772, 0x764bc88c, 0x7572b201, 0x749cb28f, 0x73c9b971, 0x72f9b658, 0x722c996b,
    0x71625344, 0x709ad4e4, 0x6fd60fba, 0x6f13f596, 0x6e5478ac, 0x6d978b8e, 0x6cdd212b, 0x6c252cc7,
    0x6b6fa1fe, 0x6abc74be, 0x6a0b9944, 0x695d041d, 0x68b0aa1f, 0x68068068, 0x675e7c5d, 0x66b893a9,
    0x6614bc36, 0x6572ec2f, 0x64d319fe, 0x64353c48, 0x639949eb, 0x62ff3a01, 0x626703d8, 0x61d09ef3,
    0x613c0309, 0x60a92806, 0x60180601, 0x5f889545, 0x5eface48, 0x5e6ea9ae, 0x5de42046, 0x5d5b2b08,
    0x5cd3c315, 0x5c4de1b6, 0x5bc9805b, 0x5b46989a, 0x5ac5242a, 0x5a451cea, 0x59c67cd8, 0x59493e14,
    0x58cd5ae2, 0x5852cda0, 0x57d990d0, 0x57619f0f, 0x56eaf319, 0x567587c4, 0x56015805, 0x558e5ee9,
    0x551c979a, 0x54abfd5a, 0x543c8b84, 0x53ce3d8b, 0x53610efb, 0x52f4fb76, 0x5289feb5, 0x52201488,
    0x51b738d1, 0x514f678b, 0x50e89cc2, 0x5082d499, 0x501e0b44, 0x4fba3d0a, 0x4f576646, 0x4ef58364,
    0x4e9490e1, 0x4e348b4d, 0x4dd56f47, 0x4d77397e, 0x4d19e6b3, 0x4cbd73b5, 0x4c61dd63, 0x4c0720ab,
    0x4bad3a87, 0x4b542804, 0x4afbe639, 0x4aa4724b, 0x4a4dc96e, 0x49f7e8e2, 0x49a2cdf3, 0x494e75fa,
    0x48fade5c, 0x48a8048a, 0x4855e601, 0x48048048, 0x47b3d0f1, 0x4763d59c, 0x47148bf0, 0x46c5f19f,
    0x46780467, 0x462ac20e, 0x45de2864, 0x45923543, 0x4546e68f, 0x44fc3a34, 0x44b22e27, 0x4468c066,
    0x441feef8, 0x43d7b7ea, 0x43901956, 0x43491158, 0x43029e1a, 0x42bcbdc8, 0x42776e9a, 0x4232aecd,
    0x41ee7ca6, 0x41aad671, 0x4167ba81, 0x41252730, 0x40e31ade, 0x40a193f1, 0x406090d9, 0x40201008,
};

/*lint -e50*/

  // Newton-rafson algorithm
static void vec_divide_x2 (f24 * restrict frac, short *exp, const f24 * restrict x, const f24 * restrict y, int N)
{
          ae_p24f *pfrac = (      ae_p24f *)frac;
          short   *pexp = exp;
    const ae_p24f *px = (const ae_p24f *)x;
    const ae_p24f *py = (const ae_p24f *)y;
    int i;

    if(IS_ALIGN(frac) && IS_ALIGN(x) && IS_ALIGN(y) ) {
        ae_p24x2s x0, y0;

        // load data and prepare pointers for pre-increment
        LD_P24X2F_IU(x0, px, 0);
        LD_P24X2F_IU(y0, py, 0);
        pfrac -= 2;

        // iterate N/2 times
        for(i = 0; i < N>>1; i++ ) {
            ae_q56s q_y0h, q_y0l;
            ae_p24x2s s = AE_XORP48(x0, y0);

            //
            // normalize input and calculate result exponent:
            //
            // x0, y0  -> normalized(abs(x0)), normalized(abs(y0))
            //
            {
                ae_p24x2s x0_abs  = AE_ABSSP24S(x0),          y0_abs = AE_ABSSP24S(y0);
                ae_q56s   q_x0h, q_x0l;
                int       exp_x0h, exp_y0h, exp_x0l, exp_y0l, exp_rh, exp_rl;

                q_x0h   = AE_CVTQ48P24S_H(x0_abs),  q_y0h   = AE_CVTQ48P24S_H(y0_abs);
                q_x0l   = AE_CVTQ48P24S_L(x0_abs),  q_y0l   = AE_CVTQ48P24S_L(y0_abs);
                exp_x0h = AE_NSAQ56S(q_x0h)-8,      exp_y0h = AE_NSAQ56S(q_y0h)-8;
                exp_x0l = AE_NSAQ56S(q_x0l)-8,      exp_y0l = AE_NSAQ56S(q_y0l)-8;

                q_x0h   = AE_SLLAQ56(q_x0h, exp_x0h);         q_y0h   = AE_SLLAQ56(q_y0h, exp_y0h);
                q_x0l   = AE_SLLAQ56(q_x0l, exp_x0l);         q_y0l   = AE_SLLAQ56(q_y0l, exp_y0l);
                x0      = AE_TRUNCP24Q48X2(q_x0h, q_x0l);     y0      = AE_TRUNCP24Q48X2(q_y0h, q_y0l);

                if(exp_x0h > 23) { // norm(0) == 55
                    exp_x0h = 23;
                }

                if(exp_x0l > 23) { // norm(0) == 55
                    exp_x0l = 23;
                }

                // exp(x0_h/y0_h)                           exp(x0_l/y0_l)
                exp_rh = -exp_x0h + exp_y0h+1;
                exp_rl = -exp_x0l + exp_y0l+1;

                *pexp++ = exp_rh;
                *pexp++ = exp_rl;
            }

            // inverse of y0 (already normalized on previous step)
            {
                ae_p24x2s x0 = y0;
                ae_q56s   t56h, t56l;
                ae_p24x2s z0, z0h, z0l, t;
                //
                // Newton朢aphson division ( see http://en.wikipedia.org/wiki/Division_(digital) )
                //
                // The steps of Newton朢aphson are:
                //
                //  1. Calculate an estimate for the reciprocal of the divisor (D): X0
                //  2. Compute successively more accurate estimates of the reciprocal: (X_1, ... , X_k)
                //
                // The Newton朢aphson iteration is:
                //
                //     X_(i+1) = 2*X_i - D*X_i*X_i
                //
                // This implemented as the following:
                //
                //     1. D    <- y0 (normalized input value)
                //     2. 2*X0 <- read from the table based on y0's MSB value
                //     3. Do two algorithm iterations.

                #define N_MSB_BITS INV_TBL_BITS
                {
                    const int32_t *pTbl   = tab_invQ30;
                    const int32_t *pTbl_h = pTbl + (((unsigned int)AE_TRUNCA32Q48(q_y0h)<<2)>>(32-N_MSB_BITS));
                    const int32_t *pTbl_l = pTbl + (((unsigned int)AE_TRUNCA32Q48(q_y0l)<<2)>>(32-N_MSB_BITS));
                    // read X0
                    LD_P24F_I(z0h, pTbl_h, 0);
                    LD_P24F_I(z0l, pTbl_l, 0);
                    z0 = AE_SELP24_HH(z0h, z0l);
                }

                // 1'st iteration
                // t=D*X0 with extended precision
                t56h = AE_MULFP24S_HH(z0h, x0); t56h = AE_SLLIQ56(t56h,  8);
                t56l = AE_MULFP24S_HL(z0l, x0); t56l = AE_SLLIQ56(t56l,  8); t = AE_TRUNCP24Q48X2(t56h, t56l);
                // t=D*X0*X0 with correct binary point position
                t56h = AE_MULP24S_HH (z0h, t);  t56h = AE_SRAIQ56(t56h,  6);
                t56l = AE_MULP24S_HL (z0l, t);  t56l = AE_SRAIQ56(t56l,  6); t = AE_TRUNCP24Q48X2(t56h, t56l);
                // z0=2*X0 - D*X0*X0
                z0  = AE_SUBSP24S(z0, t);

                // 2'nd iteration
                // t=D*X1 with extended precision
                t56h = AE_MULFP24S_HH(z0, x0); t56h = AE_SLLIQ56(t56h, 15);
                t56l = AE_MULFP24S_LL(z0, x0); t56l = AE_SLLIQ56(t56l, 15); t = AE_TRUNCP24Q48X2(t56h, t56l);
                // t=D*X1*X1 with correct binary point position
                t56h = AE_MULP24S_HH (z0, t);  t56h = AE_SRAIQ56(t56h, 13);
                t56l = AE_MULP24S_LL (z0, t);  t56l = AE_SRAIQ56(t56l, 13); t = AE_TRUNCP24Q48X2(t56h, t56l);
                // z0=2*X1 - D*X1*X1
                y0  = AE_SUBSP24S(z0, t);
            }

            // set sign and store the result
            {
                ae_q56s t56h = AE_MULFP24S_HH(x0, y0);
                ae_q56s t56l = AE_MULFP24S_LL(x0, y0);
                ae_p24x2s zero = AE_ZEROP48();
                xtbool2 bhl = AE_LTP24S(s, zero);
                ae_p24x2s z0 = AE_TRUNCP24Q48X2(t56h, t56l);
                ae_p24x2s z0_neg = AE_NEGP24(z0);

                AE_MOVTP24X2(z0, z0_neg, bhl);

                LD_P24X2F_IU(x0, px, 1);
                LD_P24X2F_IU(y0, py, 1);

                ST_P24X2F_IU(z0, pfrac, 1);
            }
        }
    } else {
        // prepare pointers for pre-increment
        px -= 1;
        py -= 1;
        pfrac -= 1;

        // iterate N times
        for(i = 0; i < N; i++ ) {
            ae_p24x2s x0, y0;
            ae_p24x2s s;

            // load data
            LD_P24F_IU(x0, px, 1);
            LD_P24F_IU(y0, py, 1);
            s = AE_XORP48(x0, y0);

            //
            // normalize input and calculate result exponent:
            //
            // x0, y0  -> normalized(abs(x0)), normalized(abs(y0))
            //
            {
                ae_p24x2s x0_abs  = AE_ABSSP24S(x0),          y0_abs = AE_ABSSP24S(y0);

                ae_q56s   q_x0h   = AE_CVTQ48P24S_H(x0_abs),  q_y0h   = AE_CVTQ48P24S_H(y0_abs);
                int       exp_x0h = AE_NSAQ56S(q_x0h)-8,      exp_y0h = AE_NSAQ56S(q_y0h)-8;
                int exp_rh;
                q_x0h   = AE_SLLAQ56(q_x0h, exp_x0h);         q_y0h   = AE_SLLAQ56(q_y0h, exp_y0h);
                x0      = AE_TRUNCP24Q48(q_x0h);              y0      = AE_TRUNCP24Q48(q_y0h);

                if(exp_x0h > 23) { // norm(0) == 55
                    exp_x0h = 23;
                }

                exp_rh = -exp_x0h + exp_y0h+1;
                *pexp++ = exp_rh;
            }

            // inverse of y0 (already normalized on previous step)
            {
                ae_p24x2s x0 = y0;
                ae_q56s   t56;
                ae_p24x2s z0, t;

                //
                // See comment for vectorized loop for details
                //
                #define N_MSB_BITS INV_TBL_BITS
                {
                    const int32_t *pTbl   = tab_invQ30;
                    const int32_t *pTbl_h = pTbl + (((unsigned int)AE_CVTA32P24_H(x0)<<2)>>(32-N_MSB_BITS));
                    // read X0
                    LD_P24F_I(z0, pTbl_h, 0);
                }

                // 1'st iteration
                // t=D*X0 with extended precision
                t56 = AE_MULFP24S_HH(z0, x0); t56 = AE_SLLIQ56(t56,  8); t = AE_TRUNCP24Q48(t56);
                // t=D*X0*X0 with correct binary point position
                t56 = AE_MULP24S_HH (z0, t);  t56 = AE_SRAIQ56(t56,  6); t = AE_TRUNCP24Q48(t56);
                // z0=2*X0 - D*X0*X0
                z0  = AE_SUBSP24S(z0, t);

                // 2'nd iteration
                // t=D*X1 with extended precision
                t56 = AE_MULFP24S_HH(z0, x0); t56 = AE_SLLIQ56(t56, 15); t = AE_TRUNCP24Q48(t56);
                // t=D*X1*X1 with correct binary point position
                t56 = AE_MULP24S_HH (z0, t);  t56 = AE_SRAIQ56(t56, 13); t = AE_TRUNCP24Q48(t56);
                // z0=2*X1 - D*X1*X1
                y0  = AE_SUBSP24S(z0, t);
            }

            // set sign and store the result
            {
                ae_q56s t56 = AE_MULFP24S_HH(x0, y0);
                ae_p24x2s zero = AE_ZEROP48();
                xtbool2 bhl = AE_LTP24S(s, zero);
                ae_p24x2s y0 = AE_TRUNCP24Q48(t56);
                ae_p24x2s y0_neg = AE_NEGP24(y0);

                AE_MOVTP24X2(y0, y0_neg, bhl);
                ST_P24F_IU(y0, pfrac, 1);
            }
        }
    }
}

void vec_divide (
          f24   *restrict frac, // [N]
          short *restrict exp,  // [N]
    const f24   *restrict x,    // [N]
    const f24   *restrict y,    // [N]
    int N
)
{
    // process even number of samples
    if(N > 1) {
        vec_divide_x2 (frac, exp, x, y, N);
    }

    // process the rest
    if(N&0x1) {
        f24 ALIGN(8)frac_tmp[2];
        short exp_tmp[2];
        vec_divide_x2 (frac_tmp, exp_tmp, x+N-1, y+N-1, 2);
        frac[N-1] = frac_tmp[0];
        exp[N-1] = exp_tmp[0];
    }
}
int32_t scl_divide(int32_t x, int32_t y)
{
    ae_p24x2s x0, y0;
    ae_p24x2s s;
    int exp_rh;

    /*  * 取x, x的 高 24bit放置AE_PR寄存器的H和L
         * x -> H
         * x -> L   */
    x0 = AE_TRUNCP24A32X2(x, x);
    y0 = AE_TRUNCP24A32X2(y, y);
    s = AE_XORP48(x0, y0);

    //
    // normalize input and calculate result exponent:
    //
    // x0, y0  -> normalized(abs(x0)), normalized(abs(y0))
    //
    {
        ae_p24x2s x0_abs  = AE_ABSSP24S(x0),          y0_abs = AE_ABSSP24S(y0);

        ae_q56s   q_x0h   = AE_CVTQ48P24S_H(x0_abs),  q_y0h   = AE_CVTQ48P24S_H(y0_abs);
        int       exp_x0h = AE_NSAQ56S(q_x0h)-8,      exp_y0h = AE_NSAQ56S(q_y0h)-8;

        q_x0h   = AE_SLLAQ56(q_x0h, exp_x0h);         q_y0h   = AE_SLLAQ56(q_y0h, exp_y0h);
        x0      = AE_TRUNCP24Q48(q_x0h);              y0      = AE_TRUNCP24Q48(q_y0h);

        if(exp_x0h > 23) { // norm(0) == 55
            exp_x0h = 23;
        }

        exp_rh = -exp_x0h + exp_y0h+1;
    }

    // inverse of y0 (already normalized on previous step)
    {
        ae_p24x2s x0 = y0;
        ae_q56s   t56;
        ae_p24x2s z0, t;

        //
        // See comment for vectorized loop for details
        //
        #define N_MSB_BITS INV_TBL_BITS
        {
            const int32_t *pTbl   = tab_invQ30;
            const int32_t *pTbl_h = pTbl + (((unsigned int)AE_CVTA32P24_H(x0)<<2)>>(32-N_MSB_BITS));
            // read X0
            LD_P24F_I(z0, pTbl_h, 0);
        }

        // 1'st iteration
        // t=D*X0 with extended precision
        t56 = AE_MULFP24S_HH(z0, x0); t56 = AE_SLLIQ56(t56,  8); t = AE_TRUNCP24Q48(t56);
        // t=D*X0*X0 with correct binary point position
        t56 = AE_MULP24S_HH (z0, t);  t56 = AE_SRAIQ56(t56,  6); t = AE_TRUNCP24Q48(t56);
        // z0=2*X0 - D*X0*X0
        z0  = AE_SUBSP24S(z0, t);

        // 2'nd iteration
        // t=D*X1 with extended precision
        t56 = AE_MULFP24S_HH(z0, x0); t56 = AE_SLLIQ56(t56, 15); t = AE_TRUNCP24Q48(t56);
        // t=D*X1*X1 with correct binary point position
        t56 = AE_MULP24S_HH (z0, t);  t56 = AE_SRAIQ56(t56, 13); t = AE_TRUNCP24Q48(t56);
        // z0=2*X1 - D*X1*X1
        y0  = AE_SUBSP24S(z0, t);
    }

    // set sign and store the result
    {
        ae_q56s t56 = AE_MULFP24S_HH(x0, y0);
        ae_p24x2s zero = AE_ZEROP48();
        xtbool2 bhl = AE_LTP24S(s, zero);
        ae_p24x2s y0 = AE_TRUNCP24Q48(t56);
        ae_p24x2s y0_neg = AE_NEGP24(y0);

        AE_MOVTP24X2(y0, y0_neg, bhl);
        return (((int)exp_rh)<<24) | (((unsigned int)AE_CVTA32P24_H(y0))>>8);
    }
}
/*lint +e50*/
