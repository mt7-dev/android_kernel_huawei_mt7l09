/*-----------------------------------------------------------------------*
 *                         PITCH_F4.C									 *
 *-----------------------------------------------------------------------*
 * Find the closed loop pitch period with 1/4 subsample resolution.		 *
 *-----------------------------------------------------------------------*/

#include "amrwb_math_op.h"
#include "amrwb_acelp.h"
#include "amrwb_cnst.h"
#include "amrwb_op_hifi.h"

#ifndef _MED_C89_

#define UP_SAMP      4
#define L_INTERPOL1  4

/* Local functions */

static void Norm_Corr(
     Word16 exc[],                         /* (i)     : excitation buffer                     */
     Word16 xn[],                          /* (i)     : target vector                         */
     Word16 h[],                           /* (i) Q15 : impulse response of synth/wgt filters */
     Word16 t_min,                         /* (i)     : minimum value of pitch lag.           */
     Word16 t_max,                         /* (i)     : maximum value of pitch lag.           */
     Word16 corr_norm[]                    /* (o) Q15 : normalized correlation                */
);
static Word16 Interpol_4(                  /* (o)  : interpolated value  */
     Word16 * x,                           /* (i)  : input vector        */
     Word16 frac                           /* (i)  : fraction (-4..+3)   */
);


Word16 Pitch_fr4(                          /* (o)     : pitch period.                         */
     Word16 exc[],                         /* (i)     : excitation buffer                     */
     Word16 xn[],                          /* (i)     : target vector                         */
     Word16 h[],                           /* (i) Q15 : impulse response of synth/wgt filters */
     Word16 t0_min,                        /* (i)     : minimum value in the searched range.  */
     Word16 t0_max,                        /* (i)     : maximum value in the searched range.  */
     Word16 * pit_frac,                    /* (o)     : chosen fraction (0, 1, 2 or 3).       */
     Word16 i_subfr,                       /* (i)     : indicator for first subframe.         */
     Word16 t0_fr2,                        /* (i)     : minimum value for resolution 1/2      */
     Word16 t0_fr1,                        /* (i)     : minimum value for resolution 1        */
     Word16 L_subfr                        /* (i)     : Length of subframe                    */
)
{
    Word16 i;
    Word16 t_min, t_max;
    Word16 max, t0, fraction, step, temp;
    Word16 *corr;
    Word16 corr_v[40];                     /* Total length = t0_max-t0_min+1+2*L_inter */

    /* Find interval to compute normalized correlation */

    t_min = sub(t0_min, L_INTERPOL1);
    t_max = add(t0_max, L_INTERPOL1);

    corr = &corr_v[-t_min];


    /* Compute normalized correlation between target and filtered excitation */

    Norm_Corr(exc, xn, h, t_min, t_max, corr);

    /* Find integer pitch */

    /*max = corr[t0_min];

    t0 = t0_min;

    for (i = add(t0_min, 1); i <= t0_max; i++)
    {
        if (corr[i] >= max)//(sub(corr[i], max) >= 0)
        {
            max = corr[i];
            t0 = i;
        }
    }*/
    max = CODEC_OpVecMax(&corr[t0_min], t0_max - t0_min + 1, &t0);
    t0 = t0 + t0_min;

    /* If first subframe and t0 >= t0_fr1, do not search fractionnal pitch */
    if ((i_subfr == 0) && (sub(t0, t0_fr1) >= 0))
    {
        *pit_frac = 0;

        return (t0);
    }
    /*------------------------------------------------------------------*
     * Search fractionnal pitch with 1/4 subsample resolution.          *
     * Test the fractions around t0 and choose the one which maximizes  *
     * the interpolated normalized correlation.                         *
     *------------------------------------------------------------------*/

    step = 1;
                                  /* 1/4 subsample resolution */
    fraction = -3;

    if (((i_subfr == 0) && (sub(t0, t0_fr2) >= 0)) || (sub(t0_fr2, AMRWB_PIT_MIN) == 0))
    {
        step = 2;
                                  /* 1/2 subsample resolution */
        fraction = -2;

    }
    if (sub(t0, t0_min) == 0)
    {
        fraction = 0;

    }
    max = Interpol_4(&corr[t0], fraction);

    for (i = add(fraction, step); i <= 3; i = (Word16) (i + step))
    {
        temp = Interpol_4(&corr[t0], i);

        if (temp > max)/* (sub(temp, max) > 0) */
        {
            max = temp;

            fraction = i;

        }
    }

    /* limit the fraction value in the interval [0,1,2,3] */
    if (fraction < 0)
    {
        fraction = add(fraction, UP_SAMP);
        t0 = sub(t0, 1);
    }
    *pit_frac = fraction;


    return (t0);
}


/*--------------------------------------------------------------------------*
 * Function Norm_Corr()                                                     *
 * ~~~~~~~~~~~~~~~~~~~~                                                     *
 * Find the normalized correlation between the target vector and the        *
 * filtered past excitation.                                                *
 * (correlation between target and filtered excitation divided by the       *
 *  square root of energy of target and filtered excitation).               *
 *--------------------------------------------------------------------------*/
static void Norm_Corr(
     Word16 exc[],                         /* (i)     : excitation buffer                     */
     Word16 xn[],                          /* (i)     : target vector                         */
     Word16 h[],                           /* (i) Q15 : impulse response of synth/wgt filters */
     Word16 t_min,                         /* (i)     : minimum value of pitch lag.           */
     Word16 t_max,                         /* (i)     : maximum value of pitch lag.           */
     Word16 corr_norm[])                   /* (o) Q15 : normalized correlation                */
{
    Word16 k, t;
    Word16 corr, exp_corr, norm, exp_norm, exp, scale;
    Word32 excf32[AMRWB_L_SUBFR/2];
    Word16 *excf = (Word16 *)excf32;
    Word32 excf_mem[AMRWB_L_SUBFR/2];
    Word16 *excf_tmp = (Word16*)excf_mem;
    Word32 L_tmp, L_tmp0, i;
    Word32 xn_mem[AMRWB_L_SUBFR/2];
    ae_p24x2s  aep_exc, aep_h, aep_excf, aep_tmp1, aep_tmp2, aep_tmp3, aep_tmp4;
    ae_p24x2s  aep_excf1, aep_excf2,aep_xn1, aep_xn2;
    ae_q56s    aeq_tmp1, aeq_tmp2, aeq_tmp3, aeq_tmp4, aeq_L_tmp, aeq_L_tmp0;
    Word16 *ph, *pexcf;

    /* compute the filtered excitation for the first delay t_min */

    k = negate(t_min);
    AMRWB_Convolve(&exc[k], h, excf, AMRWB_L_SUBFR);

    if(((Word32)xn & 0x3) != 0 )
    {
        CODEC_OpVecCpy((Word16*)xn_mem, xn, AMRWB_L_SUBFR);
        xn = (Word16*)xn_mem;
    }

    /* Compute rounded down 1/sqrt(energy of xn[]) */

    L_tmp = 1L;

    /*for (i = 0; i < L_subfr; i++)
        L_tmp = L_mac(L_tmp, xn[i], xn[i]);*/
    L_tmp = CODEC_OpVvSelfMacAlignedQuan(xn, AMRWB_L_SUBFR, L_tmp);

    exp = norm_l(L_tmp);
    exp = sub(30, exp);

    exp = add(exp, 2);                     /* energy of xn[] x 2 + rounded up     */
    scale = negate(shr(exp, 1));           /* (1<<scale) < 1/sqrt(energy rounded) */

    /* loop for every possible period */

    for (t = t_min; t < t_max; t++)
    {
        /* Compute correlation between xn[] and excf[] */
        /* Compute 1/sqrt(energy of excf[]) */
        aeq_L_tmp  = AE_CVTQ48A32S(1L);
        aeq_L_tmp0 = AE_CVTQ48A32S(1L);

        for (i = 0; i < AMRWB_L_SUBFR>>2; i++)
        {
            aep_excf1  = *((ae_p16x2s *)&excf[4*i]);
            aeq_L_tmp  = AE_SATQ48S(aeq_L_tmp);

            aep_xn1    = *((ae_p16x2s *)&xn[4*i]);
            aeq_L_tmp0 = AE_SATQ48S(aeq_L_tmp0);

            aep_excf2  = *((ae_p16x2s *)&excf[4*i + 2]);
            aep_xn2    = *((ae_p16x2s *)&xn[4*i + 2]);

            AE_MULAAFP24S_HH_LL(aeq_L_tmp,  aep_excf1, aep_excf1);
            AE_MULAAFP24S_HH_LL(aeq_L_tmp0, aep_xn1, aep_excf1);
            AE_MULAAFP24S_HH_LL(aeq_L_tmp,  aep_excf2, aep_excf2);
            AE_MULAAFP24S_HH_LL(aeq_L_tmp0, aep_xn2, aep_excf2);
        }
        aeq_L_tmp0 = AE_SATQ48S(aeq_L_tmp0);
        aeq_L_tmp  = AE_SATQ48S(aeq_L_tmp);

        L_tmp0 = AE_TRUNCA32Q48(aeq_L_tmp0);
        L_tmp  = AE_TRUNCA32Q48(aeq_L_tmp);

        exp = norm_l(L_tmp0);
        L_tmp0 = L_shl(L_tmp0, exp);
        exp_corr = (30 - exp);
        corr = extract_h(L_tmp0);

        exp = norm_l(L_tmp);
        L_tmp = L_shl(L_tmp, exp);
        exp_norm = (30 - exp);

        Isqrt_n(&L_tmp, &exp_norm);
        norm = extract_h(L_tmp);

        /* Normalize correlation = correlation * (1/sqrt(energy)) */

        L_tmp = L_mult(corr, norm);
        L_tmp = L_shl(L_tmp, exp_corr + exp_norm + scale);

        corr_norm[t] = round(L_tmp);

        /* modify the filtered excitation excf[] for the next iteration */

        k--;

        aep_exc = *((ae_p16s *)&exc[k]);
        i     = (AMRWB_L_SUBFR - 1);
        ph    = &h[i];
        pexcf = &excf[i];

        aep_h    = *((ae_p16s *)ph--);
        aep_excf = *((ae_p16s *)(pexcf-1));

        for (i; i > 3; i-=4)
        {
            aeq_tmp1 = AE_MULFS32P16S_LL(aep_exc, aep_h);

            aep_h    = *((ae_p16s *)ph--);
            aep_tmp1  = AE_TRUNCP24Q48(aeq_tmp1);

            aep_tmp1  = AE_ADDSP24S(aep_excf, aep_tmp1);
            aep_excf = *((ae_p16s *)(pexcf-2));

            *((ae_p16s *)pexcf--) = aep_tmp1;
            aeq_tmp2 = AE_MULFS32P16S_LL(aep_exc, aep_h);

            aep_h    = *((ae_p16s *)ph--);
            aep_tmp2  = AE_TRUNCP24Q48(aeq_tmp2);

            aep_tmp2  = AE_ADDSP24S(aep_excf, aep_tmp2);
            aep_excf = *((ae_p16s *)(pexcf-2));

            *((ae_p16s *)pexcf--) = aep_tmp2;
            aeq_tmp3 = AE_MULFS32P16S_LL(aep_exc, aep_h);

            aep_h    = *((ae_p16s *)ph--);
            aep_tmp3  = AE_TRUNCP24Q48(aeq_tmp3);

            aep_tmp3  = AE_ADDSP24S(aep_excf, aep_tmp3);
            aep_excf = *((ae_p16s *)(pexcf-2));

            *((ae_p16s *)pexcf--) = aep_tmp3;
            aeq_tmp4 = AE_MULFS32P16S_LL(aep_exc, aep_h);

            aep_h    = *((ae_p16s *)ph--);
            aep_tmp4  = AE_TRUNCP24Q48(aeq_tmp4);

            aep_tmp4  = AE_ADDSP24S(aep_excf, aep_tmp4);
            aep_excf = *((ae_p16s *)(pexcf-2));

            *((ae_p16s *)pexcf--) = aep_tmp4;

        }

        aeq_tmp1 = AE_MULFS32P16S_LL(aep_exc, aep_h);

        aep_h    = *((ae_p16s *)ph--);
        aep_tmp1  = AE_TRUNCP24Q48(aeq_tmp1);

        aep_tmp1  = AE_ADDSP24S(aep_excf, aep_tmp1);
        aep_excf = *((ae_p16s *)(pexcf-2));

        *((ae_p16s *)pexcf--) = aep_tmp1;
        aeq_tmp2 = AE_MULFS32P16S_LL(aep_exc, aep_h);

        aep_h    = *((ae_p16s *)ph--);
        aep_tmp2  = AE_TRUNCP24Q48(aeq_tmp2);

        aep_tmp2  = AE_ADDSP24S(aep_excf, aep_tmp2);
        aep_excf = *((ae_p16s *)(pexcf-2));

        *((ae_p16s *)pexcf--) = aep_tmp2;
        aeq_tmp3 = AE_MULFS32P16S_LL(aep_exc, aep_h);

        aep_h    = *((ae_p16s *)ph--);
        aep_tmp3  = AE_TRUNCP24Q48(aeq_tmp3);

        aep_tmp3  = AE_ADDSP24S(aep_excf, aep_tmp3);
        aep_excf = *((ae_p16s *)(pexcf-2));

        *((ae_p16s *)pexcf--) = aep_tmp3;

        excf[0] = mult(exc[k], h[0]);
    }

    /* Compute correlation between xn[] and excf[] */
    /* Compute 1/sqrt(energy of excf[]) */
    aeq_L_tmp  = AE_CVTQ48A32S(1L);
    aeq_L_tmp0 = AE_CVTQ48A32S(1L);

    for (i = 0; i < AMRWB_L_SUBFR>>2; i++)
    {
        aep_excf1  = *((ae_p16x2s *)&excf[4*i]);
        aeq_L_tmp  = AE_SATQ48S(aeq_L_tmp);

        aep_xn1    = *((ae_p16x2s *)&xn[4*i]);
        aeq_L_tmp0 = AE_SATQ48S(aeq_L_tmp0);

        aep_excf2  = *((ae_p16x2s *)&excf[4*i + 2]);
        aep_xn2    = *((ae_p16x2s *)&xn[4*i + 2]);

        AE_MULAAFP24S_HH_LL(aeq_L_tmp,  aep_excf1, aep_excf1);
        AE_MULAAFP24S_HH_LL(aeq_L_tmp0, aep_xn1, aep_excf1);
        AE_MULAAFP24S_HH_LL(aeq_L_tmp,  aep_excf2, aep_excf2);
        AE_MULAAFP24S_HH_LL(aeq_L_tmp0, aep_xn2, aep_excf2);
    }
    aeq_L_tmp0 = AE_SATQ48S(aeq_L_tmp0);
    aeq_L_tmp  = AE_SATQ48S(aeq_L_tmp);

    L_tmp0 = AE_TRUNCA32Q48(aeq_L_tmp0);
    L_tmp  = AE_TRUNCA32Q48(aeq_L_tmp);

    exp = norm_l(L_tmp0);
    L_tmp0 = L_shl(L_tmp0, exp);
    exp_corr = (30 - exp);
    corr = extract_h(L_tmp0);

    exp = norm_l(L_tmp);
    L_tmp = L_shl(L_tmp, exp);
    exp_norm = (30 - exp);

    Isqrt_n(&L_tmp, &exp_norm);
    norm = extract_h(L_tmp);

    /* Normalize correlation = correlation * (1/sqrt(energy)) */

    L_tmp = L_mult(corr, norm);
    L_tmp = L_shl(L_tmp, exp_corr + exp_norm + scale);

    corr_norm[t] = round(L_tmp);

    return;
}

/*--------------------------------------------------------------------------*
 * Procedure Interpol_4()                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                   *
 * For interpolating the normalized correlation with 1/4 resolution.        *
 *--------------------------------------------------------------------------*/

/* 1/4 resolution interpolation filter (-3 dB at 0.791*fs/2) in Q14 */

static Word16 inter4_1[UP_SAMP * 2 * L_INTERPOL1] =
{
    -12, -26, 32, 206,
    420, 455, 73, -766,
    -1732, -2142, -1242, 1376,
    5429, 9910, 13418, 14746,
    13418, 9910, 5429, 1376,
    -1242, -2142, -1732, -766,
    73, 455, 420, 206,
    32, -26, -12, 0
};

static Word16 Interpol_4(                  /* (o)  : interpolated value  */
     Word16 * x,                           /* (i)  : input vector        */
     Word16 frac                           /* (i)  : fraction (-4..+3)   */
)
{
    Word16 sum;
    Word32 i, k, L_sum;

    if (frac < 0)
    {
        frac = add(frac, UP_SAMP);
        x--;

    }
    x = x - L_INTERPOL1 + 1;


    L_sum = 0L;
    for (i = 0, k = (UP_SAMP - 1 - frac)/*sub(sub(UP_SAMP, 1), frac)*/; i < 2 * L_INTERPOL1; i++, k += UP_SAMP)
    {
        L_sum = L_mac(L_sum, x[i], inter4_1[k]);
    }

    sum = round(L_shl(L_sum, 1));

    return (sum);
}

#endif
