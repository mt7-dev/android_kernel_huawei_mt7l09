/*------------------------------------------------------------------------*
 *                         P_MED_OL.C                                     *
 *------------------------------------------------------------------------*
 * Compute the open loop pitch lag.                                       *
 *------------------------------------------------------------------------*/

#include "amrwb_acelp.h"
#include "amrwb_math_op.h"

#ifndef _MED_C89_

static Word16 corrweight[199]= {

10772, 10794, 10816, 10839, 10862, 10885, 10908, 10932, 10955, 10980,
11004, 11029, 11054, 11079, 11105, 11131, 11157, 11183, 11210, 11238,
11265, 11293, 11322, 11350, 11379, 11409, 11439, 11469, 11500, 11531,
11563, 11595, 11628, 11661, 11694, 11728, 11763, 11798, 11834, 11870,
11907, 11945, 11983, 12022, 12061, 12101, 12142, 12184, 12226, 12270,
12314, 12358, 12404, 12451, 12498, 12547, 12596, 12647, 12699, 12751,
12805, 12861, 12917, 12975, 13034, 13095, 13157, 13221, 13286, 13353,
13422, 13493, 13566, 13641, 13719, 13798, 13880, 13965, 14053, 14143,
14237, 14334, 14435, 14539, 14648, 14761, 14879, 15002, 15130, 15265,
15406, 15554, 15710, 15874, 16056, 16384, 16384, 16384, 16384, 16384,
16384, 16384, 16056, 15874, 15710, 15554, 15406, 15265, 15130, 15002,
14879, 14761, 14648, 14539, 14435, 14334, 14237, 14143, 14053, 13965,
13880, 13798, 13719, 13641, 13566, 13493, 13422, 13353, 13286, 13221,
13157, 13095, 13034, 12975, 12917, 12861, 12805, 12751, 12699, 12647,
12596, 12547, 12498, 12451, 12404, 12358, 12314, 12270, 12226, 12184,
12142, 12101, 12061, 12022, 11983, 11945, 11907, 11870, 11834, 11798,
11763, 11728, 11694, 11661, 11628, 11595, 11563, 11531, 11500, 11469,
11439, 11409, 11379, 11350, 11322, 11293, 11265, 11238, 11210, 11183,
11157, 11131, 11105, 11079, 11054, 11029, 11004, 10980, 10955, 10932,
10908, 10885, 10862, 10839, 10816, 10794, 10772, 10750, 10728};

Word16 Pitch_med_ol_we(                   /* output: open loop pitch lag                             */
     Word16 wsp[],                         /* input : signal used to compute the open loop pitch      */
                                           /*         wsp[-pit_max] to wsp[-1] should be known        */
     Word16 L_min,                         /* input : minimum pitch lag                               */
     Word16 L_max,                         /* input : maximum pitch lag                               */
     Word16 L_frame,                       /* input : length of frame to compute pitch                */
     Word16 L_0,                           /* input : old_ open-loop pitch                            */
     Word16 * gain,                        /* output: normalize correlation of hp_wsp for the Lag     */
     Word16 * hp_wsp_mem,                  /* i:o   : memory of the hypass filter for hp_wsp[] (lg=9) */
     Word16 * old_hp_wsp                   /* i:o   : hypass wsp[]                                    */
)
{
    Word16 Tm;
    Word16 hi, lo;
    Word16 *ww, *we, *hp_wsp;
    Word16 exp_R0, exp_R1, exp_R2;
    Word32 i, j, max, R0, R1, R2;

    ae_p24x2s aep_wsp1, aep_wsp2, aep_wsp3, aep_wsp4;
    ae_p24x2s aep_ww, aep_we, aep_tm, aep_i, aep_hi, aep_lo, aep_16384, aep_tmp;
    ae_q56s   aeq_r0, aeq_tmp, aeq_max, aeq_r1, aeq_r2;
    xtbool    b;

    ww = &corrweight[198];
    we = &corrweight[98 + L_max - L_0];

    aeq_max   = AE_CVTQ48A32S(MIN_32);
    aep_tm    = AE_ZEROP48();
    aep_16384 = AE_CVTP24A16(16384);

    for (i = L_max; i > L_min; i--)
    {
        /* Compute the correlation */
        aeq_r0 = AE_ZEROQ56();

        for (j = 0; j < (L_frame>>1); j++)
        {
            /* R0 = L_mac(R0, wsp[j], wsp[j - i]); */
            aep_wsp1 = *((ae_p16s *)&wsp[2*j]);
            aep_wsp2 = *((ae_p16s *)&wsp[2*j - i]);

            AE_MULAFS32P16S_LL(aeq_r0, aep_wsp1, aep_wsp2);

            aep_wsp3 = *((ae_p16s *)&wsp[2*j + 1]);
            aep_wsp4 = *((ae_p16s *)&wsp[2*j + 1 - i]);

            AE_MULAFS32P16S_LL(aeq_r0, aep_wsp3, aep_wsp4);
        }

        /* Weighting of the correlation function.   */
        aep_ww = *((ae_p16s *)ww--);

        #ifdef _MED_AMRWB_FASTER_
        aeq_r0 = AE_MULFQ32SP16S_L (aeq_r0, aep_ww);
        aeq_r0 = AE_SATQ48S(aeq_r0);
        #else
        aep_hi = AE_TRUNCP24Q48(aeq_r0);
        aep_hi = AE_TRUNCP16(aep_hi);
        aeq_r0 = AE_SRAIQ56(aeq_r0, 1);
        AE_MULSFS32P16S_LL(aeq_r0, aep_hi, aep_16384);
        aeq_r0 = AE_SLLIQ56(aeq_r0, 16);
        aep_lo = AE_TRUNCP24Q48(aeq_r0);

        aeq_r0 = AE_MULFS32P16S_LL(aep_hi, aep_ww);
        aeq_tmp = AE_MULFS32P16S_LL(aep_lo, aep_ww);

        aep_tmp = AE_TRUNCP24Q48(aeq_tmp);
        aep_hi  = AE_CVTP24A16(1);
        AE_MULAFS32P16S_LL(aeq_r0, aep_hi, aep_tmp);
        #endif

        /* Weight the neighbourhood of the old lag. */
        aep_we = *((ae_p16s *)we--);
        #ifdef _MED_AMRWB_FASTER_
        aeq_r0 = AE_MULFQ32SP16S_L (aeq_r0, aep_we);
        aeq_r0 = AE_SATQ48S(aeq_r0);
        #else
        aep_hi = AE_TRUNCP24Q48(aeq_r0);
        aep_hi = AE_TRUNCP16(aep_hi);
        aeq_r0 = AE_SRAIQ56(aeq_r0, 1);
        AE_MULSFS32P16S_LL(aeq_r0, aep_hi, aep_16384);
        aeq_r0 = AE_SLLIQ56(aeq_r0, 16);
        aep_lo = AE_TRUNCP24Q48(aeq_r0);

        aeq_r0 = AE_MULFS32P16S_LL(aep_hi, aep_we);
        aeq_tmp = AE_MULFS32P16S_LL(aep_lo, aep_we);

        aep_tmp = AE_TRUNCP24Q48(aeq_tmp);
        aep_hi  = AE_CVTP24A16(1);
        AE_MULAFS32P16S_LL(aeq_r0, aep_hi, aep_tmp);
        #endif

        b = AE_LEQ56S(aeq_max, aeq_r0);
        aep_i = AE_CVTP24A16(i);
        AE_MOVTQ56(aeq_max, aeq_r0, b);
        AE_MOVTP48(aep_tm, aep_i, b);

    }
    Tm = AE_TRUNCA16P24S_L(aep_tm);

    /* Hypass the wsp[] vector */

    hp_wsp = old_hp_wsp + L_max;
    Hp_wsp(wsp, hp_wsp, L_frame, hp_wsp_mem);

    /* Compute normalize correlation at delay Tm */

    aeq_r0 = AE_ZEROQ56();
    aeq_r1 = AE_CVTQ48A32S(1L);
    aeq_r2 = AE_CVTQ48A32S(1L);

    for (j = 0; j < (L_frame>>1); j++)
    {
        i = 2*j - Tm;
        aep_wsp1 = *((ae_p16s *)&hp_wsp[2*j]);

        aep_wsp2 = *((ae_p16s *)&hp_wsp[i]);
        i = (2*j + 1) - Tm;

        AE_MULAFS32P16S_LL(aeq_r0, aep_wsp1, aep_wsp2);
        aep_wsp3 = *((ae_p16s *)&hp_wsp[2*j + 1]);

        AE_MULAFS32P16S_LL(aeq_r1, aep_wsp2, aep_wsp2);
        aep_wsp4 = *((ae_p16s *)&hp_wsp[i]);

        AE_MULAFS32P16S_LL(aeq_r2, aep_wsp1, aep_wsp1);

        AE_MULAFS32P16S_LL(aeq_r0, aep_wsp3, aep_wsp4);
        AE_MULAFS32P16S_LL(aeq_r1, aep_wsp4, aep_wsp4);
        AE_MULAFS32P16S_LL(aeq_r2, aep_wsp3, aep_wsp3);
    }

    exp_R0 = (AE_NSAQ56S(aeq_r0) - 8);
    aeq_r0 = AE_SLLASQ56S(aeq_r0, exp_R0);

    exp_R1 = (AE_NSAQ56S(aeq_r1) - 8);
    aeq_r1 = AE_SLLASQ56S(aeq_r1, exp_R1);

    exp_R2 = (AE_NSAQ56S(aeq_r2) - 8);
    aeq_r2 = AE_SLLASQ56S(aeq_r2, exp_R2);

    aep_wsp1 = AE_ROUNDSP16Q48ASYM(aeq_r1);
    aep_wsp2 = AE_ROUNDSP16Q48ASYM(aeq_r2);
    aeq_r1 = AE_MULFS32P16S_LL(aep_wsp1, aep_wsp2);

    i = (AE_NSAQ56S(aeq_r1) - 8);
    aeq_r1 = AE_SLLASQ56S(aeq_r1, i);

    exp_R1 += exp_R2;
    exp_R1 += i;
    exp_R1 = 62 - exp_R1;

    R0 = AE_TRUNCA32Q48(aeq_r0);
    R1 = AE_TRUNCA32Q48(aeq_r1);

    Isqrt_n(&R1, &exp_R1);

    R0 = L_mult(round(R0), round(R1));
    exp_R0 = sub(31, exp_R0);
    exp_R0 = add(exp_R0, exp_R1);

    *gain = round(L_shl(R0, exp_R0));


    /* Shitf hp_wsp[] for next frame */
    CODEC_OpVecCpy(old_hp_wsp, &old_hp_wsp[L_frame], L_max);

    return (Tm);
}

Word16 Pitch_med_ol_nwe(                   /* output: open loop pitch lag                             */
     Word16 wsp[],                         /* input : signal used to compute the open loop pitch      */
                                           /*         wsp[-pit_max] to wsp[-1] should be known        */
     Word16 L_min,                         /* input : minimum pitch lag                               */
     Word16 L_max,                         /* input : maximum pitch lag                               */
     Word16 L_frame,                       /* input : length of frame to compute pitch                */
     Word16 L_0,                           /* input : old_ open-loop pitch                            */
     Word16 * gain,                        /* output: normalize correlation of hp_wsp for the Lag     */
     Word16 * hp_wsp_mem,                  /* i:o   : memory of the hypass filter for hp_wsp[] (lg=9) */
     Word16 * old_hp_wsp                   /* i:o   : hypass wsp[]                                    */
)
{
    Word16 Tm;
    Word16 hi, lo;
    Word16 *ww, *we, *hp_wsp;
    Word16 exp_R0, exp_R1, exp_R2;
    Word32 i, j, max, R0, R1, R2;

    ae_p24x2s aep_wsp1, aep_wsp2, aep_wsp3, aep_wsp4;
    ae_p24x2s aep_ww, aep_tm, aep_i, aep_hi, aep_lo, aep_16384, aep_tmp;
    ae_q56s   aeq_r0, aeq_r1, aeq_r2, aeq_tmp, aeq_max;
    xtbool    b;

    ww = &corrweight[198];
    we = &corrweight[98 + L_max - L_0];

    aeq_max = AE_CVTQ48A32S(MIN_32);
    aep_tm  = AE_ZEROP48();
    aep_16384 = AE_CVTP24A16(16384);

    for (i = L_max; i > L_min; i--)
    {
        /* Compute the correlation */
        aeq_r0 = AE_ZEROQ56();

        for (j = 0; j < (L_frame>>1); j++)
        {
            /* R0 = L_mac(R0, wsp[j], wsp[j - i]); */
            aep_wsp1 = *((ae_p16s *)&wsp[2*j]);
            aep_wsp2 = *((ae_p16s *)&wsp[2*j - i]);

            AE_MULAFS32P16S_LL(aeq_r0, aep_wsp1, aep_wsp2);

            aep_wsp3 = *((ae_p16s *)&wsp[2*j + 1]);
            aep_wsp4 = *((ae_p16s *)&wsp[2*j + 1 - i]);

            AE_MULAFS32P16S_LL(aeq_r0, aep_wsp3, aep_wsp4);
        }

        /* Weighting of the correlation function.   */
        aep_ww = *((ae_p16s *)ww--);
        // aeq_r0 = AE_MULFQ32SP16S_L (aeq_r0, aep_ww);
        aep_hi = AE_TRUNCP24Q48(aeq_r0);
        aep_hi = AE_TRUNCP16(aep_hi);
        aeq_r0 = AE_SRAIQ56(aeq_r0, 1);
        AE_MULSFS32P16S_LL(aeq_r0, aep_hi, aep_16384);
        aeq_r0 = AE_SLLIQ56(aeq_r0, 16);
        aep_lo = AE_TRUNCP24Q48(aeq_r0);

        aeq_r0 = AE_MULFS32P16S_LL(aep_hi, aep_ww);
        aeq_tmp = AE_MULFS32P16S_LL(aep_lo, aep_ww);

        aep_tmp = AE_TRUNCP24Q48(aeq_tmp);
        aep_hi  = AE_CVTP24A16(1);
        AE_MULAFS32P16S_LL(aeq_r0, aep_hi, aep_tmp);

        b = AE_LEQ56S(aeq_max, aeq_r0);
        aep_i = AE_CVTP24A16(i);
        AE_MOVTQ56(aeq_max, aeq_r0, b);
        AE_MOVTP48(aep_tm, aep_i, b);
    }
    Tm = AE_TRUNCA16P24S_L(aep_tm);

    /* Hypass the wsp[] vector */

    hp_wsp = old_hp_wsp + L_max;
    Hp_wsp(wsp, hp_wsp, L_frame, hp_wsp_mem);

    /* Compute normalize correlation at delay Tm */

    aeq_r0 = AE_ZEROQ56();
    aeq_r1 = AE_CVTQ48A32S(1L);
    aeq_r2 = AE_CVTQ48A32S(1L);

    for (j = 0; j < (L_frame>>1); j++)
    {
        i = 2*j - Tm;
        aep_wsp1 = *((ae_p16s *)&hp_wsp[2*j]);

        aep_wsp2 = *((ae_p16s *)&hp_wsp[i]);
        i = (2*j + 1) - Tm;

        AE_MULAFS32P16S_LL(aeq_r0, aep_wsp1, aep_wsp2);
        aep_wsp3 = *((ae_p16s *)&hp_wsp[2*j + 1]);

        AE_MULAFS32P16S_LL(aeq_r1, aep_wsp2, aep_wsp2);
        aep_wsp4 = *((ae_p16s *)&hp_wsp[i]);

        AE_MULAFS32P16S_LL(aeq_r2, aep_wsp1, aep_wsp1);

        AE_MULAFS32P16S_LL(aeq_r0, aep_wsp3, aep_wsp4);
        AE_MULAFS32P16S_LL(aeq_r1, aep_wsp4, aep_wsp4);
        AE_MULAFS32P16S_LL(aeq_r2, aep_wsp3, aep_wsp3);
    }

    exp_R0 = (AE_NSAQ56S(aeq_r0) - 8);
    aeq_r0 = AE_SLLASQ56S(aeq_r0, exp_R0);

    exp_R1 = (AE_NSAQ56S(aeq_r1) - 8);
    aeq_r1 = AE_SLLASQ56S(aeq_r1, exp_R1);

    exp_R2 = (AE_NSAQ56S(aeq_r2) - 8);
    aeq_r2 = AE_SLLASQ56S(aeq_r2, exp_R2);

    aep_wsp1 = AE_ROUNDSP16Q48ASYM(aeq_r1);
    aep_wsp2 = AE_ROUNDSP16Q48ASYM(aeq_r2);
    aeq_r1 = AE_MULFS32P16S_LL(aep_wsp1, aep_wsp2);

    i = (AE_NSAQ56S(aeq_r1) - 8);
    aeq_r1 = AE_SLLASQ56S(aeq_r1, i);

    exp_R1 += exp_R2;
    exp_R1 += i;
    exp_R1 = 62 - exp_R1;

    R0 = AE_TRUNCA32Q48(aeq_r0);
    R1 = AE_TRUNCA32Q48(aeq_r1);

    Isqrt_n(&R1, &exp_R1);

    R0 = L_mult(round(R0), round(R1));
    exp_R0 = sub(31, exp_R0);
    exp_R0 = add(exp_R0, exp_R1);

    *gain = round(L_shl(R0, exp_R0));


    /* Shitf hp_wsp[] for next frame */
    CODEC_OpVecCpy(old_hp_wsp, &old_hp_wsp[L_frame], L_max);

    return (Tm);
    return (Tm);
}

/*____________________________________________________________________
 |
 |
 |  FUNCTION NAME median5
 |
 |      Returns the median of the set {X[-2], X[-1],..., X[2]},
 |      whose elements are 16-bit integers.
 |
 |  INPUT
 |      X[-2:2]   16-bit integers.
 |
 |  RETURN VALUE
 |      The median of {X[-2], X[-1],..., X[2]}.
 |_____________________________________________________________________
 */

Word16 median5(Word16 x[])
{
    Word16 x1, x2, x3, x4, x5;
    Word16 tmp;

    x1 = x[-2];
    x2 = x[-1];
    x3 = x[0];
    x4 = x[1];
    x5 = x[2];


    if (sub(x2, x1) < 0)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (sub(x3, x1) < 0)
    {
        tmp = x1;
        x1 = x3;
        x3 = tmp;
    }
    if (sub(x4, x1) < 0)
    {
        tmp = x1;
        x1 = x4;
        x4 = tmp;
    }
    if (sub(x5, x1) < 0)
    {
        x5 = x1;
    }
    if (sub(x3, x2) < 0)
    {
        tmp = x2;
        x2 = x3;
        x3 = tmp;
    }
    if (sub(x4, x2) < 0)
    {
        tmp = x2;
        x2 = x4;
        x4 = tmp;
    }
    if (sub(x5, x2) < 0)
    {
        x5 = x2;
    }
    if (sub(x4, x3) < 0)
    {
        x3 = x4;
    }
    if (sub(x5, x3) < 0)
    {
        x3 = x5;
    }
    return (x3);
}

/*____________________________________________________________________
 |
 |
 |  FUNCTION NAME med_olag
 |
 |
 |_____________________________________________________________________
 */


Word16 Med_olag(                           /* output : median of  5 previous open-loop lags       */
     Word16 prev_ol_lag,                   /* input  : previous open-loop lag                     */
     Word16 old_ol_lag[5]
)
{
    Word32 i;

    /* Use median of 5 previous open-loop lags as old lag */

    for (i = 4; i > 0; i--)
    {
        old_ol_lag[i] = old_ol_lag[i - 1];
    }

    old_ol_lag[0] = prev_ol_lag;

    i = median5(&old_ol_lag[2]);

    return i;

}
#endif
