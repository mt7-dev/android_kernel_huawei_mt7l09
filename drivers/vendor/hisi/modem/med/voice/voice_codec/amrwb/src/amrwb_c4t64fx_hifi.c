/*------------------------------------------------------------------------*
 *                         C4T64FX.C                                      *
 *------------------------------------------------------------------------*
 * Performs algebraic codebook search for higher modes                    *
 *------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*
 * Function  ACELP_4t64_fx()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~                                             *
 * 20, 36, 44, 52, 64, 72, 88 bits algebraic codebook.                   *
 * 4 tracks x 16 positions per track = 64 samples.                       *
 *                                                                       *
 * 20 bits --> 4 pulses in a frame of 64 samples.                        *
 * 36 bits --> 8 pulses in a frame of 64 samples.                        *
 * 44 bits --> 10 pulses in a frame of 64 samples.                       *
 * 52 bits --> 12 pulses in a frame of 64 samples.                       *
 * 64 bits --> 16 pulses in a frame of 64 samples.                       *
 * 72 bits --> 18 pulses in a frame of 64 samples.                       *
 * 88 bits --> 24 pulses in a frame of 64 samples.                       *
 *                                                                       *
 * All pulses can have two (2) possible amplitudes: +1 or -1.            *
 * Each pulse can have sixteen (16) possible positions.                  *
 *-----------------------------------------------------------------------*/

#include "amrwb_math_op.h"
#include "amrwb_acelp.h"
#include "amrwb_cnst.h"

#include "amrwb_q_pulse.h"
#include "amrwb_op_hifi.h"

#ifndef _MED_C89_

static Word16 tipos[36] = {
    0, 1, 2, 3,                            /* starting point &ipos[0], 1st iter */
    1, 2, 3, 0,                            /* starting point &ipos[4], 2nd iter */
    2, 3, 0, 1,                            /* starting point &ipos[8], 3rd iter */
    3, 0, 1, 2,                            /* starting point &ipos[12], 4th iter */
    0, 1, 2, 3,
    1, 2, 3, 0,
    2, 3, 0, 1,
    3, 0, 1, 2,
    0, 1, 2, 3};                           /* end point for 24 pulses &ipos[35], 4th iter */

#define NB_PULSE_MAX  24

/*#define L_SUBFR   64*/
#define NB_TRACK  4
#define STEP      4
#define DSTEP     8
#define NB_POS    16
#define MSIZE     256
#define NB_MAX    8
#define NPMAXPT   ((NB_PULSE_MAX+NB_TRACK-1)/NB_TRACK)


/* locals functions */

static void cor_h_vec(
     Word16 h[],                           /* (i) scaled impulse response                 */
     Word16 vec[],                         /* (i) scaled vector (/8) to correlate with h[] */
     Word16 track,                         /* (i) track to use                            */
     Word16 sign[],                        /* (i) sign vector                             */
     Word16 rrixix[][NB_POS],              /* (i) correlation of h[x] with h[x]      */
     Word16 cor[]                          /* (o) result of correlation (NB_POS elements) */
);

static void search_ixiy(
     Word16 nb_pos_ix,                     /* (i) nb of pos for pulse 1 (1..8)       */
     Word16 track_x,                       /* (i) track of pulse 1                   */
     Word16 track_y,                       /* (i) track of pulse 2                   */
     Word16 * ps,                          /* (i/o) correlation of all fixed pulses  */
     Word16 * alp,                         /* (i/o) energy of all fixed pulses       */
     Word16 * ix,                          /* (o) position of pulse 1                */
     Word16 * iy,                          /* (o) position of pulse 2                */
     Word16 dn[],                          /* (i) corr. between target and h[]       */
     Word16 dn2[],                         /* (i) vector of selected positions       */
     Word16 cor_x[],                       /* (i) corr. of pulse 1 with fixed pulses */
     Word16 cor_y[],                       /* (i) corr. of pulse 2 with fixed pulses */
     Word16 rrixiy[][MSIZE]                /* (i) corr. of pulse 1 with pulse 2   */
);

void ACELP_4t64_fx(
     Word16 dn[],                          /* (i) <12b : correlation between target x[] and H[]      */
     Word16 cn[],                          /* (i) <12b : residual after long term prediction         */
     Word16 H[],                           /* (i) Q12: impulse response of weighted synthesis filter */
     Word16 code[],                        /* (o) Q9 : algebraic (fixed) codebook excitation         */
     Word16 y[],                           /* (o) Q9 : filtered fixed codebook excitation            */
     Word16 nbbits,                        /* (i) : 20, 36, 44, 52, 64, 72 or 88 bits                */
     Word16 ser_size,                      /* (i) : bit rate                                         */
     Word16 _index[]                       /* (o) : index (20): 5+5+5+5 = 20 bits.                   */
                                           /* (o) : index (36): 9+9+9+9 = 36 bits.                   */
                                           /* (o) : index (44): 13+9+13+9 = 44 bits.                 */
                                           /* (o) : index (52): 13+13+13+13 = 52 bits.               */
                                           /* (o) : index (64): 2+2+2+2+14+14+14+14 = 64 bits.       */
                                           /* (o) : index (72): 10+2+10+2+10+14+10+14 = 72 bits.     */
                                           /* (o) : index (88): 11+11+11+11+11+11+11+11 = 88 bits.   */
)
{
    Word32 st, pos, index, track, nb_pulse, nbiter;
    Word16 ix, iy, psk, ps, alpk, alp, val, k_cn, k_dn, exp;
    Word16 *p0, *p1, *p2, *p3, *psign;
    Word16 *h, *h_inv, *ptr_h1, *ptr_h2, *ptr_hf, h_shift;
    Word32 i, j, k, s, cor, L_tmp, L_index;

    ae_p24x2s   aep_tmp, aep_h1, aep_h2, aep_p0, aep_p1, aep_p2, aep_p3, aep_sign, aep_vec;
    ae_p24x2s   aep_k_cn, aep_cn, aep_k_dn, aep_dn, aep_dn2;
    ae_q56s     aeq_cor, aeq_tmp, aeq_tmp2;

    Word16 dn2[AMRWB_L_SUBFR], sign[AMRWB_L_SUBFR], ashwValue[AMRWB_L_SUBFR];
    Word32 vec2[AMRWB_L_SUBFR/2];
    Word16 *vec = (Word16 *)vec2;
    Word16 ind[NPMAXPT * NB_TRACK];
    Word16 codvec[NB_PULSE_MAX], nbpos[10];
    Word16 cor_x[NB_POS], cor_y[NB_POS], pos_max[NB_TRACK];
    Word32 h_buf[2 * AMRWB_L_SUBFR];
    Word16 *pshwh_buf = (Word16 *)h_buf;
    Word32 rrixix32[NB_TRACK][NB_POS/2], rrixiy32[NB_TRACK][MSIZE/2];
    Word16 (*rrixix)[NB_POS];
    Word16 (*rrixiy)[MSIZE];

    Word32 ipos32[NB_PULSE_MAX];
    Word16 *ipos = (Word16*)ipos32;

    rrixix = (Word16(*)[NB_POS])rrixix32;
    rrixiy = (Word16(*)[MSIZE])rrixiy32;

    switch (nbbits)
    {
    case 20:                               /* 20 bits, 4 pulses, 4 tracks */
        nbiter = 4;                          /* 4x16x16=1024 loop */
        alp = 8192;                          /* alp = 2.0 (Q12) */
        nb_pulse = 4;
        nbpos[0] = 4;
        nbpos[1] = 8;
        break;
    case 36:                               /* 36 bits, 8 pulses, 4 tracks */
        nbiter = 4;                          /* 4x20x16=1280 loop */
        alp = 4096;                          /* alp = 1.0 (Q12) */
        nb_pulse = 8;
        nbpos[0] = 4;
        nbpos[1] = 8;
        nbpos[2] = 8;
        break;
    case 44:                               /* 44 bits, 10 pulses, 4 tracks */
        nbiter = 4;                          /* 4x26x16=1664 loop */
        alp = 4096;                          /* alp = 1.0 (Q12) */
        nb_pulse = 10;
        nbpos[0] = 4;
        nbpos[1] = 6;
        nbpos[2] = 8;
        nbpos[3] = 8;
        break;
    case 52:                               /* 52 bits, 12 pulses, 4 tracks */
        nbiter = 4;                          /* 4x26x16=1664 loop */
        alp = 4096;                          /* alp = 1.0 (Q12) */
        nb_pulse = 12;
        nbpos[0] = 4;
        nbpos[1] = 6;
        nbpos[2] = 8;
        nbpos[3] = 8;
        break;
    case 64:                               /* 64 bits, 16 pulses, 4 tracks */
        nbiter = 3;                          /* 3x36x16=1728 loop */
        alp = 3277;                          /* alp = 0.8 (Q12) */
        nb_pulse = 16;
        nbpos[0] = 4;
        nbpos[1] = 4;
        nbpos[2] = 6;
        nbpos[3] = 6;
        nbpos[4] = 8;
        nbpos[5] = 8;
        break;
    case 72:                               /* 72 bits, 18 pulses, 4 tracks */
        nbiter = 3;                          /* 3x35x16=1680 loop */
        alp = 3072;                          /* alp = 0.75 (Q12) */
        nb_pulse = 18;
        nbpos[0] = 2;
        nbpos[1] = 3;
        nbpos[2] = 4;
        nbpos[3] = 5;
        nbpos[4] = 6;
        nbpos[5] = 7;
        nbpos[6] = 8;
        break;
    case 88:                               /* 88 bits, 24 pulses, 4 tracks */
        if (sub(ser_size, 462) > 0)
            nbiter = 1;
        else
            nbiter = 2;                    /* 2x53x16=1696 loop */

        alp = 2048;                          /* alp = 0.5 (Q12) */
        nb_pulse = 24;
        nbpos[0] = 2;
        nbpos[1] = 2;
        nbpos[2] = 3;
        nbpos[3] = 4;
        nbpos[4] = 5;
        nbpos[5] = 6;
        nbpos[6] = 7;
        nbpos[7] = 8;
        nbpos[8] = 8;
        nbpos[9] = 8;
        break;
    default:
        nbiter = 0;
        alp = 0;
        nb_pulse = 0;
    }

    for (i = 0; i < nb_pulse; i++)
    {
        codvec[i] = i;
    }

    /*----------------------------------------------------------------*
     * Find sign for each pulse position.                             *
     *----------------------------------------------------------------*/

    /* calculate energy for normalization of cn[] and dn[] */

    /* set k_cn = 32..32767 (ener_cn = 2^30..256-0) */
    s = Dot_product12(cn, cn, AMRWB_L_SUBFR, &exp);
    Isqrt_n(&s, &exp);
    s = L_shl(s, add(exp, 5));             /* saturation can occur here */
    k_cn = round(s);

    /* set k_dn = 32..512 (ener_dn = 2^30..2^22) */
    s = Dot_product12(dn, dn, AMRWB_L_SUBFR, &exp);
    Isqrt_n(&s, &exp);
    k_dn = round(L_shl(s, add(exp, 5 + 3)));    /* k_dn = 256..4096 */
    k_dn = mult_r(alp, k_dn);              /* alp in Q12 */

    /* mix normalized cn[] and dn[] */
    aep_k_cn = AE_CVTP24A16(k_cn);
    aep_k_dn = AE_CVTP24A16(k_dn);

    aep_cn = *((ae_p16s *)cn);
    aep_dn = *((ae_p16s *)dn);

    for (i = 0; i < (AMRWB_L_SUBFR>>1); i++)
    {
        aeq_tmp = AE_MULFS32P16S_LL(aep_k_cn, aep_cn);
        aep_cn = *((ae_p16s *)&cn[2*i+1]);

        AE_MULAFS32P16S_LL(aeq_tmp, aep_k_dn, aep_dn);
        aep_dn = *((ae_p16s *)&dn[2*i+1]);

        aeq_tmp = AE_SLLASQ56S(aeq_tmp, 8);
        aeq_tmp2 = AE_MULFS32P16S_LL(aep_k_cn, aep_cn);
        aep_cn = *((ae_p16s *)&cn[2*i+2]);

        aeq_tmp = AE_SATQ48S(aeq_tmp);
        AE_MULAFS32P16S_LL(aeq_tmp2, aep_k_dn, aep_dn);
        aep_dn = *((ae_p16s *)&dn[2*i+2]);

        aeq_tmp2 = AE_SLLASQ56S(aeq_tmp2, 8);
        aep_tmp = AE_TRUNCP24Q48(aeq_tmp);

        aeq_tmp2 = AE_SATQ48S(aeq_tmp2);
        *((ae_p16s *)&dn2[2*i]) = aep_tmp;

        aep_tmp = AE_TRUNCP24Q48(aeq_tmp2);
        *((ae_p16s *)&dn2[2*i+1]) = aep_tmp;
    }

    /* set sign according to dn2[] = k_cn*cn[] + k_dn*dn[]    */

    CODEC_OpVecSet(sign, AMRWB_L_SUBFR,  32767);
    CODEC_OpVecSet(vec,  AMRWB_L_SUBFR, -32768);

    for (k = 0; k < NB_TRACK; k++)
    {
        for (i = k; i < AMRWB_L_SUBFR; i += STEP)
        {
            if(dn2[i] < 0)
            {
                sign[i] = -32768;            /* sign = -1 (Q12) */
                vec[i]  = 32767;

                aep_dn = *((ae_p16s *)&dn[i]);
                aep_dn = AE_NEGSP24S(aep_dn);
                *((ae_p16s *)&dn[i]) = aep_dn;

                aep_dn2 = *((ae_p16s *)&dn2[i]);
                aep_dn2 = AE_NEGSP24S(aep_dn2);
                *((ae_p16s *)&dn2[i]) = aep_dn2;

            }
        }
    }

    /*----------------------------------------------------------------*
     * Select NB_MAX position per track according to max of dn2[].    *
     *----------------------------------------------------------------*/

    pos = 0;
    for (i = 0; i < NB_TRACK; i++)
    {

        ps = -1;
        for (j = i; j < AMRWB_L_SUBFR; j += STEP)
        {
            if (dn2[j] > ps)
            {
                ps = dn2[j];
                pos = j;
            }
        }

        dn2[pos] = 0 - NB_MAX;/*sub(k, NB_MAX); */    /* dn2 < 0 when position is selected */
        pos_max[i] = pos;

        for (k = 1; k < NB_MAX; k++)
        {
            ps = -1;
            for (j = i; j < AMRWB_L_SUBFR; j += STEP)
            {
                if (dn2[j] > ps)/* (sub(dn2[j], ps) > 0) */
                {
                    ps = dn2[j];
                    pos = j;
                }
            }

            dn2[pos] = k - NB_MAX;/* sub(k, NB_MAX); */    /* dn2 < 0 when position is selected */
        }
    }

    /*--------------------------------------------------------------*
     * Scale h[] to avoid overflow and to get maximum of precision  *
     * on correlation.                                              *
     *                                                              *
     * Maximum of h[] (h[0]) is fixed to 2048 (MAX16 / 16).         *
     *  ==> This allow addition of 16 pulses without saturation.    *
     *                                                              *
     * Energy worst case (on resonant impulse response),            *
     * - energy of h[] is approximately MAX/16.                     *
     * - During search, the energy is divided by 8 to avoid         *
     *   overflow on "alp". (energy of h[] = MAX/128).              *
     *  ==> "alp" worst case detected is 22854 on sinusoidal wave.  *
     *--------------------------------------------------------------*/

    /* impulse response buffer for fast computation */

    h = pshwh_buf;
    h_inv = pshwh_buf + (2 * AMRWB_L_SUBFR);

    CODEC_OpVecSet(h, AMRWB_L_SUBFR, 0);
    CODEC_OpVecSet(h_inv, AMRWB_L_SUBFR, 0);

    h += AMRWB_L_SUBFR;
    h_inv += AMRWB_L_SUBFR;

    /* scale h[] down (/2) when energy of h[] is high with many pulses used */

    /*for (i = 0; i < L_SUBFR; i++)
        L_tmp = L_mac(L_tmp, H[i], H[i]);*/
    L_tmp = CODEC_OpVvSelfMacAlignedQuan(H, AMRWB_L_SUBFR, 0);

    val = extract_h(L_tmp);

    h_shift = 0;

    if ((sub(nb_pulse, 12) >= 0) && (sub(val, 1024) > 0))
    {
        h_shift = 1;
    }

    CODEC_OpVecShr(H, AMRWB_L_SUBFR, h_shift, h);

    for (i = 0; i < (AMRWB_L_SUBFR>>1); i++)
    {
        aep_h1 = *((ae_p16x2s *)&h[2*i]);
        aep_h1 = AE_NEGSP24S(aep_h1);
        *((ae_p16x2s *)&h_inv[2*i]) = aep_h1;
    }

    /*------------------------------------------------------------*
     * Compute rrixix[][] needed for the codebook search.         *
     * This algorithm compute impulse response energy of all      *
     * positions (16) in each track (4).       Total = 4x16 = 64. *
     *------------------------------------------------------------*/

    /* storage order --> i3i3, i2i2, i1i1, i0i0 */

    /* Init pointers to last position of rrixix[] */
    p0 = &rrixix[0][NB_POS - 1];
    p1 = &rrixix[1][NB_POS - 1];
    p2 = &rrixix[2][NB_POS - 1];
    p3 = &rrixix[3][NB_POS - 1];

    ptr_h1 = h;

    aeq_cor = AE_CVTQ48A32S(0x00008000L);

    for (i = 0; i < NB_POS; i++)
    {
        aep_h1 = *((ae_p16x2s *)ptr_h1++);
        AE_MULAFS32P16S_HH(aeq_cor, aep_h1, aep_h1);
        /* cor = L_mac(cor, *ptr_h1, *ptr_h1);*/

        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p3--) = aep_tmp;
        /* *p3-- = extract_h(cor);*/

        ptr_h1++;
        AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h1);
        /* cor = L_mac(cor, *ptr_h1, *ptr_h1);*/

        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p2--) = aep_tmp;
        /* *p2-- = extract_h(cor);*/

        aep_h1 = *((ae_p16x2s *)ptr_h1++);
        AE_MULAFS32P16S_HH(aeq_cor, aep_h1, aep_h1);
        /* cor = L_mac(cor, *ptr_h1, *ptr_h1);*/

        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p1--) = aep_tmp;
        /* *p2-- = extract_h(cor);*/

        ptr_h1++;
        AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h1);
        /* cor = L_mac(cor, *ptr_h1, *ptr_h1);*/

        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p0--) = aep_tmp;
        /* *p0-- = extract_h(cor);*/
    }

    /*------------------------------------------------------------*
     * Compute rrixiy[][] needed for the codebook search.         *
     * This algorithm compute correlation between 2 pulses        *
     * (2 impulses responses) in 4 possible adjacents tracks.     *
     * (track 0-1, 1-2, 2-3 and 3-0).     Total = 4x16x16 = 1024. *
     *------------------------------------------------------------*/

    /* storage order --> i2i3, i1i2, i0i1, i3i0 */

    pos = MSIZE - 1;
    ptr_hf = h + 1;

    for (k = 0; k < NB_POS; k++)
    {
        p3 = &rrixiy[2][pos];
        p2 = &rrixiy[1][pos];
        p1 = &rrixiy[0][pos];
        p0 = &rrixiy[3][pos - NB_POS];

        ptr_h1 = h;
        ptr_h2 = ptr_hf;
        aeq_cor = AE_CVTQ48A32S(0x00008000L);

        for (i = (k + 1); i < NB_POS; i++)
        {
            aep_h1 = *((ae_p16x2s *)ptr_h1++);
            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_HL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p3) = aep_tmp;

            ptr_h1++;
            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p2) = aep_tmp;

            aep_h1 = *((ae_p16x2s *)ptr_h1++);
            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_HL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p1) = aep_tmp;

            ptr_h1++;
            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p0) = aep_tmp;

            p3 -= (NB_POS + 1);
            p2 -= (NB_POS + 1);
            p1 -= (NB_POS + 1);
            p0 -= (NB_POS + 1);
        }

        aep_h1 = *((ae_p16x2s *)ptr_h1++);
        aep_h2 = *((ae_p16s *)ptr_h2++);
        AE_MULAFS32P16S_HL(aeq_cor, aep_h1, aep_h2);
        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p3) = aep_tmp;

        ptr_h1++;
        aep_h2 = *((ae_p16s *)ptr_h2++);
        AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p2) = aep_tmp;

        aep_h1 = *((ae_p16s *)ptr_h1++);
        aep_h2 = *((ae_p16s *)ptr_h2++);
        AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p1) = aep_tmp;

        pos -= NB_POS;
        ptr_hf += STEP;
    }

    /* storage order --> i3i0, i2i3, i1i2, i0i1 */

    pos = MSIZE - 1;
    ptr_hf = h + 3;

    for (k = 0; k < NB_POS; k++)
    {
        p3 = &rrixiy[3][pos];
        p2 = &rrixiy[2][pos - 1];
        p1 = &rrixiy[1][pos - 1];
        p0 = &rrixiy[0][pos - 1];

        ptr_h1 = h;
        ptr_h2 = ptr_hf;
        aeq_cor = AE_CVTQ48A32S(0x00008000L);

        for (i = (k + 1); i < NB_POS; i++)
        {
            aep_h1 = *((ae_p16x2s *)ptr_h1++);
            ptr_h1++;
            aep_h2 = *((ae_p16s *)ptr_h2++);

            AE_MULAFS32P16S_HL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p3) = aep_tmp;

            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p2) = aep_tmp;

            aep_h1 = *((ae_p16x2s *)ptr_h1++);
            ptr_h1++;
            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_HL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p1) = aep_tmp;

            aep_h2 = *((ae_p16s *)ptr_h2++);
            AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
            aep_tmp = AE_TRUNCP24Q48(aeq_cor);
            *((ae_p16s *)p0) = aep_tmp;

            p3 -= (NB_POS + 1);
            p2 -= (NB_POS + 1);
            p1 -= (NB_POS + 1);
            p0 -= (NB_POS + 1);
        }

        aep_h1 = *((ae_p16s *)ptr_h1++);
        aep_h2 = *((ae_p16s *)ptr_h2++);
        AE_MULAFS32P16S_LL(aeq_cor, aep_h1, aep_h2);
        aep_tmp = AE_TRUNCP24Q48(aeq_cor);
        *((ae_p16s *)p3) = aep_tmp;

        pos--;
        ptr_hf += STEP;
    }

    /*------------------------------------------------------------*
     * Modification of rrixiy[][] to take signs into account.     *
     *------------------------------------------------------------*/

    p0 = &rrixiy[0][0];

    for (i = 0; i < AMRWB_L_SUBFR; i += STEP)
    {
        psign = sign;
        if (psign[i] < 0)
        {
            psign = vec;
        }
        for (j = 1; j < AMRWB_L_SUBFR; j += DSTEP)
        {
            aep_p0   = *((ae_p16x2s *)p0);
            aep_sign = *((ae_p16s *)&psign[j]);

            aeq_tmp  = AE_MULFS32P16S_HL(aep_p0, aep_sign);

            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);

            aep_sign = *((ae_p16s *)&psign[j+STEP]);

            *((ae_p16s *)p0) = aep_tmp;

            aeq_tmp  = AE_MULFS32P16S_LL(aep_p0, aep_sign);

            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);

            *((ae_p16s *)(p0+1)) = aep_tmp;

            p0 += 2;
        }
    }

    for (i = 1; i < AMRWB_L_SUBFR; i += STEP)
    {
        psign = sign;
        if (psign[i] < 0)
        {
            psign = vec;
        }
        for (j = 2; j < AMRWB_L_SUBFR; j += DSTEP)
        {
            aep_p0   = *((ae_p16x2s *)p0);
            aep_sign = *((ae_p16s *)&psign[j]);

            aeq_tmp  = AE_MULFS32P16S_HL(aep_p0, aep_sign);

            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            aep_sign = *((ae_p16s *)&psign[j+STEP]);

            *((ae_p16s *)p0) = aep_tmp;
            aeq_tmp  = AE_MULFS32P16S_LL(aep_p0, aep_sign);
            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            *((ae_p16s *)(p0+1)) = aep_tmp;

            p0 += 2;
        }
    }

    for (i = 2; i < AMRWB_L_SUBFR; i += STEP)
    {
        psign = sign;
        if (psign[i] < 0)
        {
            psign = vec;
        }
        for (j = 3; j < AMRWB_L_SUBFR; j += DSTEP)
        {
            aep_p0   = *((ae_p16x2s *)p0);
            aep_sign = *((ae_p16s *)&psign[j]);

            aeq_tmp  = AE_MULFS32P16S_HL(aep_p0, aep_sign);

            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            aep_sign = *((ae_p16s *)&psign[j+STEP]);

            *((ae_p16s *)p0) = aep_tmp;
            aeq_tmp  = AE_MULFS32P16S_LL(aep_p0, aep_sign);
            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            *((ae_p16s *)(p0+1)) = aep_tmp;

            p0 += 2;
        }
    }

    for (i = 3; i < AMRWB_L_SUBFR; i += STEP)
    {
        psign = sign;
        if (psign[i] < 0)
        {
            psign = vec;
        }
        for (j = 0; j < AMRWB_L_SUBFR; j += DSTEP)
        {
            aep_p0   = *((ae_p16x2s *)p0);
            aep_sign = *((ae_p16s *)&psign[j]);

            aeq_tmp  = AE_MULFS32P16S_HL(aep_p0, aep_sign);

            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            aep_sign = *((ae_p16s *)&psign[j+STEP]);

            *((ae_p16s *)p0) = aep_tmp;
            aeq_tmp  = AE_MULFS32P16S_LL(aep_p0, aep_sign);
            aep_tmp  = AE_TRUNCP24Q48(aeq_tmp);
            *((ae_p16s *)(p0+1)) = aep_tmp;

            p0 += 2;
        }
    }

    /*-------------------------------------------------------------------*
     *                       Deep first search                           *
     *-------------------------------------------------------------------*/

    psk = -1;
    alpk = 1;

    for (k = 0; k < nbiter; k++)
    {
        /*for (i = 0; i < nb_pulse; i++)
            ipos[i] = tipos[(k * 4) + i];*/
        CODEC_OpVecCpy(ipos, &tipos[(k * 4)], nb_pulse);

        if (nbbits == 20)
        {
            pos = 0;
            ps = 0;
            alp = 0;
            CODEC_OpVecSet(vec, AMRWB_L_SUBFR, 0);
        }
        else if ((nbbits == 36) || (nbbits == 44))
        {
            /* first stage: fix 2 pulses */
            pos = 2;

            ix = ind[0] = pos_max[ipos[0]];
            iy = ind[1] = pos_max[ipos[1]];
            ps = add(dn[ix], dn[iy]);
            i = ix>>2;                /* ix / STEP */
            j = iy>>2;                /* iy / STEP */
            s = L_mult(rrixix[ipos[0]][i], 4096);
            s = L_mac(s, rrixix[ipos[1]][j], 4096);
            i = (i<<4) + j;         /* (ix/STEP)*NB_POS + (iy/STEP) */
            s = L_mac(s, rrixiy[ipos[0]][i], 8192);
            alp = round(s);

            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;

            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            for (i = 0; i < (AMRWB_L_SUBFR>>2); i++)
            {
                aep_p0  = *((ae_p16s *)p0++);
                aep_p1  = *((ae_p16s *)p1++);

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i+1]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i+2]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                *((ae_p16s *)&vec[4*i+3]) = aep_tmp;

            }

            if (nbbits == 44)
            {
                ipos[8] = 0;
                ipos[9] = 1;
            }
        }
        else
        {
            /* first stage: fix 4 pulses */
            pos = 4;

            ix = ind[0] = pos_max[ipos[0]];
            iy = ind[1] = pos_max[ipos[1]];
            i =  ind[2] = pos_max[ipos[2]];
            j =  ind[3] = pos_max[ipos[3]];
            ps = add(add(add(dn[ix], dn[iy]), dn[i]), dn[j]);

            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;

            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            if (sign[i] < 0)
                p2 = h_inv - i;
            else
                p2 = h - i;

            if (sign[j] < 0)
                p3 = h_inv - j;
            else
                p3 = h - j;

            for (i = 0; i < (AMRWB_L_SUBFR>>2); i++)
            {
                aep_p0  = *((ae_p16s *)p0++);
                aep_p1  = *((ae_p16s *)p1++);
                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p2  = *((ae_p16s *)p2++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p2);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p3  = *((ae_p16s *)p3++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p3);
                aep_p1  = *((ae_p16s *)p1++);
                *((ae_p16s *)&vec[4*i]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p2  = *((ae_p16s *)p2++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p2);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p3  = *((ae_p16s *)p3++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p3);
                aep_p1  = *((ae_p16s *)p1++);
                *((ae_p16s *)&vec[4*i+1]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p2  = *((ae_p16s *)p2++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p2);
                aep_p0  = *((ae_p16s *)p0++);
                aep_p3  = *((ae_p16s *)p3++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p3);
                aep_p1  = *((ae_p16s *)p1++);
                *((ae_p16s *)&vec[4*i+2]) = aep_tmp;

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p2  = *((ae_p16s *)p2++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p2);
                aep_p3  = *((ae_p16s *)p3++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_p3);
                *((ae_p16s *)&vec[4*i+3]) = aep_tmp;

            }

            L_tmp = CODEC_OpVvSelfMacAlignedQuan(vec, AMRWB_L_SUBFR, 0);

            alp = round(L_shr(L_tmp, 3));

            if (nbbits == 72)
            {
                ipos[16] = 0;
                ipos[17] = 1;
            }
        }
        /* other stages of 2 pulses */

        for (j = pos, st = 0; j < nb_pulse; j += 2, st++)
        {
            /*--------------------------------------------------*
            * Calculate correlation of all possible positions  *
            * of the next 2 pulses with previous fixed pulses. *
            * Each pulse can have 16 possible positions.       *
            *--------------------------------------------------*/
            cor_h_vec(h, vec, ipos[j], sign, rrixix, cor_x);
            cor_h_vec(h, vec, ipos[j + 1], sign, rrixix, cor_y);

            /*--------------------------------------------------*
            * Find best positions of 2 pulses.                 *
            *--------------------------------------------------*/

            search_ixiy(nbpos[st], ipos[j], ipos[j + 1], &ps, &alp,
                &ix, &iy, dn, dn2, cor_x, cor_y, rrixiy);

            ind[j] = ix;
            ind[j + 1] = iy;

            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;
            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            i = 0;

            aep_p0  = *((ae_p16s *)p0++);
            aep_p1  = *((ae_p16s *)p1++);

            for (i; i < (AMRWB_L_SUBFR>>2); i++)
            {
                aep_vec = *((ae_p16s *)&vec[4*i]);

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);

                aep_tmp = AE_ADDSP24S(aep_tmp, aep_vec);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i]) = aep_tmp;

                aep_vec = *((ae_p16s *)&vec[4*i+1]);

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);

                aep_tmp = AE_ADDSP24S(aep_tmp, aep_vec);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i+1]) = aep_tmp;

                aep_vec = *((ae_p16s *)&vec[4*i+2]);

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_vec);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i+2]) = aep_tmp;

                aep_vec = *((ae_p16s *)&vec[4*i+3]);

                aep_tmp = AE_ADDSP24S(aep_p0, aep_p1);
                aep_p0  = *((ae_p16s *)p0++);
                aep_tmp = AE_ADDSP24S(aep_tmp, aep_vec);
                aep_p1  = *((ae_p16s *)p1++);

                *((ae_p16s *)&vec[4*i+3]) = aep_tmp;

            }
        }

        /* memorise the best codevector */

        ps = mult(ps, ps);
        s = L_msu(L_mult(alpk, ps), psk, alp);
        if (s > 0)
        {
            psk = ps;
            alpk = alp;
            /*for (i = 0; i < nb_pulse; i++)
            {
                codvec[i] = ind[i];
            }*/
            CODEC_OpVecCpy(codvec, ind, nb_pulse);

            /*for (i = 0; i < L_SUBFR; i++)
            {
                y[i] = vec[i];
            }*/
            CODEC_OpVecCpy(y, vec, AMRWB_L_SUBFR);
        }
    }

    /*-------------------------------------------------------------------*
     * Build the codeword, the filtered codeword and index of codevector.*
     *-------------------------------------------------------------------*/

    /*for (i = 0; i < NPMAXPT * NB_TRACK; i++)
    {
        ind[i] = -1;
    }*/
    CODEC_OpVecSet(ind, NPMAXPT * NB_TRACK, -1);

    /*for (i = 0; i < L_SUBFR; i++)
    {
        code[i] = 0;
        y[i] = shr_r(y[i], 3);               // Q12 to Q9
    }*/
    CODEC_OpVecSet(code, AMRWB_L_SUBFR, 0);
    CODEC_OpVecShr_r(y, AMRWB_L_SUBFR, 3, y);

    val = shr(512, h_shift);               /* codeword in Q9 format */

    for (k = 0; k < nb_pulse; k++)
    {
        i = codvec[k];                       /* read pulse position */
        j = sign[i];                         /* read sign           */

        index = shr(i, 2);                 /* index = pos of pulse (0..15) */
        track = (Word16) (i & 0x03);         /* track = i % NB_TRACK (0..3)  */

        if (j > 0)
        {
            code[i] = add(code[i], val);
            codvec[k] = add(codvec[k], (2 * AMRWB_L_SUBFR));
        } else
        {
            code[i] = sub(code[i], val);
            index = add(index, NB_POS);
        }

        i = extract_l(L_shr(L_mult(track, NPMAXPT), 1));

        while (ind[i] >= 0)
        {
            i++;/* = add(i, 1); */
        }
        ind[i] = index;
    }

    k = 0;

    /* Build index of codevector */
    switch(nbbits)
    {
        case 20:
        {
            for (track = 0; track < NB_TRACK; track++)
            {
                _index[track] = extract_l(quant_1p_N1(ind[k], 4));
                k += NPMAXPT;
            }

            break;
        }

        case 36:
        {
            for (track = 0; track < NB_TRACK; track++)
            {
                _index[track] = extract_l(quant_2p_2N1(ind[k], ind[k + 1], 4));
                k += NPMAXPT;
            }

            break;
        }

        case 44:
        {
            for (track = 0; track < NB_TRACK - 2; track++)
            {
                _index[track] = extract_l(quant_3p_3N1(ind[k], ind[k + 1], ind[k + 2], 4));
                k += NPMAXPT;
            }

            for (track = 2; track < NB_TRACK; track++)
            {
                _index[track] = extract_l(quant_2p_2N1(ind[k], ind[k + 1], 4));
                k += NPMAXPT;
            }

            break;
        }

        case 52:
        {
            for (track = 0; track < NB_TRACK; track++)
            {
                _index[track] = extract_l(quant_3p_3N1(ind[k], ind[k + 1], ind[k + 2], 4));
                k += NPMAXPT;
            }

            break;
        }

        case 64:
        {
            for (track = 0; track < NB_TRACK; track++)
            {
                L_index = quant_4p_4N(&ind[k], 4);
                _index[track] = extract_l(L_shr(L_index, 14) & 3);
                _index[track + NB_TRACK] = extract_l(L_index & 0x3FFF);
                k += NPMAXPT;
            }

            break;
        }

        case 72:
        {
            for (track = 0; track < NB_TRACK - 2; track++)
            {
                L_index = quant_5p_5N(&ind[k], 4);
                _index[track] = extract_l(L_shr(L_index, 10) & 0x03FF);
                _index[track + NB_TRACK] = extract_l(L_index & 0x03FF);
                k += NPMAXPT;
            }

            for (track = 2; track < NB_TRACK; track++)
            {
                L_index = quant_4p_4N(&ind[k], 4);
                _index[track] = extract_l(L_shr(L_index, 14) & 3);
                _index[track + NB_TRACK] = extract_l(L_index & 0x3FFF);
                k += NPMAXPT;
            }

            break;
        }

        case 88:
        {
            for (track = 0; track < NB_TRACK; track++)
            {
                L_index = quant_6p_6N_2(&ind[k], 4);
                _index[track] = extract_l(L_shr(L_index, 11) & 0x07FF);
                _index[track + NB_TRACK] = extract_l(L_index & 0x07FF);
                k += NPMAXPT;
            }

            break;
        }

        default:
            break;
    }
    return;
}

/*-------------------------------------------------------------------*
 * Function  cor_h_vec()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~                                             *
 * Compute correlations of h[] with vec[] for the specified track.   *
 *-------------------------------------------------------------------*/
static void cor_h_vec(
     Word16 h[],                           /* (i) scaled impulse response                 */
     Word16 vec[],                         /* (i) scaled vector (/8) to correlate with h[] */
     Word16 track,                         /* (i) track to use                            */
     Word16 sign[],                        /* (i) sign vector                             */
     Word16 rrixix[][NB_POS],              /* (i) correlation of h[x] with h[x]      */
     Word16 cor[]                          /* (o) result of correlation (NB_POS elements) */
)
{
    Word16 pos;
    Word16 *p0, *p1;
    Word32 i, k, L_sum, len;
    Word32 swBuf[AMRWB_L_SUBFR/2];
    Word16 *pshwBuf = (Word16 *)swBuf;

    ae_p24x2s aep_h_1, aep_p1_1, aep_h_2, aep_p1_2;
    ae_p24x2s aep_sign0, aep_corr0, aep_sign1, aep_corr1, aep_p0;
    ae_p24x2s aep_h_buf1, aep_h_buf2;
    ae_q56s aeq_L_sum0, aeq_L_sum1;

    p0 = rrixix[track];
    pos = track;
    p1 = &vec[pos];
    len = AMRWB_L_SUBFR - pos;

    /* vector alignment */
    if (0 != ((Word32)p1 & 0x03))
    {
        aep_h_1 = *((ae_p16s *)&p1[0]);
        aep_h_2 = *((ae_p16s *)&p1[1]);

        for (k=0; k < (len)>>1; k++)
        {
            AE_MOVTP24X2(aep_p0, aep_h_1, 0x2);
            AE_MOVTP24X2(aep_p0, aep_h_2, 0x1);

            *((ae_p16x2s *)&pshwBuf[2*k]) = aep_p0;

            aep_h_1 = *((ae_p16s *)&p1[2*k + 2]);
            aep_h_2 = *((ae_p16s *)&p1[2*k + 3]);

        }

        if (CODEC_OpCheckBit0Set(len))
        {
            *((ae_p16s *)&pshwBuf[len - 1]) = aep_h_1;
        }

        p1 = pshwBuf;
    }

    /* vector calc */
    for (i = 0; i < (NB_POS>>1); i++)
    {
        aeq_L_sum0 = AE_ZEROQ56();
        aeq_L_sum1 = AE_ZEROQ56();

        /* STEP */
        aep_h_buf1  = *((ae_p16x2s *)&h[0]);
        aep_p1_1 = *((ae_p16x2s *)&p1[0]);

        aep_h_buf2 = *((ae_p16x2s *)&h[2]);
        aep_p1_2 = *((ae_p16x2s *)&p1[2]);

        AE_MULAAFP24S_HH_LL(aeq_L_sum0, aep_h_buf1, aep_p1_1);
        AE_MULAAFP24S_HH_LL(aeq_L_sum0, aep_h_buf2, aep_p1_2);
        // aeq_L_sum0 = AE_SATQ48S(aeq_L_sum0);

        p1+=STEP;

        len = (AMRWB_L_SUBFR -STEP) - pos;

        for (k = 0; k < len>>2; k++)
        {
            aep_h_1  = *((ae_p16x2s *)&h[4*k + STEP]);
            aep_p1_1 = *((ae_p16x2s *)&p1[4*k]);
            aeq_L_sum0 = AE_SATQ48S(aeq_L_sum0);

            aep_h_2 = *((ae_p16x2s *)&h[4*k + STEP+ 2]);
            aep_p1_2 = *((ae_p16x2s *)&p1[4*k + 2]);
            aeq_L_sum1 = AE_SATQ48S(aeq_L_sum1);

            AE_MULAAFP24S_HH_LL(aeq_L_sum0, aep_h_1, aep_p1_1);
            AE_MULAAFP24S_HH_LL(aeq_L_sum0, aep_h_2, aep_p1_2);

            AE_MULAAFP24S_HH_LL(aeq_L_sum1, aep_h_buf1, aep_p1_1);
            aep_h_buf1 = AE_MOVP48(aep_h_1);
            AE_MULAAFP24S_HH_LL(aeq_L_sum1, aep_h_buf2, aep_p1_2);
            aep_h_buf2 = AE_MOVP48(aep_h_2);
        }
        // aeq_L_sum0 = AE_SATQ48S(aeq_L_sum0);
        // aeq_L_sum1 = AE_SATQ48S(aeq_L_sum1);

        if (CODEC_OpCheckBit1Set(len))
        {
            k = (len & 0x7FFFFFFC);

            aep_h_1  = *((ae_p16x2s *)&h[k+STEP]);
            aep_p1_1 = *((ae_p16x2s *)&p1[k]);

            AE_MULAAFP24S_HH_LL(aeq_L_sum0, aep_h_1, aep_p1_1);
            AE_MULAAFP24S_HH_LL(aeq_L_sum1, aep_h_buf1, aep_p1_1);
            aep_h_buf1 = AE_MOVP48(aep_h_buf2);
        }

        //aeq_L_sum0 = AE_SATQ48S(aeq_L_sum0);
        //aeq_L_sum1 = AE_SATQ48S(aeq_L_sum1);

        if (CODEC_OpCheckBit0Set(len))
        {
            aep_h_1  = *((ae_p16s *)&h[len+(STEP-1)]);
            aep_p1_1 = *((ae_p16s *)&p1[len - 1]);

            AE_MULAFS32P16S_LL(aeq_L_sum0, aep_h_1, aep_p1_1);
            AE_MULAFS32P16S_HL(aeq_L_sum1, aep_h_buf1, aep_p1_1);
        }

        aeq_L_sum0 = AE_SLLASQ56S(aeq_L_sum0, 1);
        aep_sign0  = *((ae_p16s *)&sign[pos]);
        aeq_L_sum0 = AE_SATQ48S(aeq_L_sum0);

        aep_corr0 = AE_ROUNDSP16Q48ASYM(aeq_L_sum0);
        aep_p0  = *((ae_p16s *)p0++);

        aeq_L_sum0 = AE_MULFS32P16S_LL(aep_corr0, aep_sign0);
        aep_corr0 = AE_TRUNCP24Q48(aeq_L_sum0);
        aep_corr0 = AE_ADDSP24S(aep_corr0, aep_p0);

        *((ae_p16s *)&cor[2*i]) = aep_corr0;

        aeq_L_sum1 = AE_SLLASQ56S(aeq_L_sum1, 1);
        aep_sign1  = *((ae_p16s *)&sign[pos+STEP]);
        aeq_L_sum1 = AE_SATQ48S(aeq_L_sum1);

        aep_corr1 = AE_ROUNDSP16Q48ASYM(aeq_L_sum1);
        aep_p0  = *((ae_p16s *)p0++);

        aeq_L_sum1 = AE_MULFS32P16S_LL(aep_corr1, aep_sign1);
        aep_corr1 = AE_TRUNCP24Q48(aeq_L_sum1);
        aep_corr1 = AE_ADDSP24S(aep_corr1, aep_p0);

        *((ae_p16s *)&cor[2*i+1]) = aep_corr1;

        p1 += STEP;
        pos += DSTEP;
    }

    return;
}

/*-------------------------------------------------------------------*
 * Function  search_ixiy()                                           *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                           *
 * Find the best positions of 2 pulses in a subframe.                *
 *-------------------------------------------------------------------*/
// #define _MED_AMRWB_PRECISE_

static void search_ixiy(
     Word16 nb_pos_ix,                     /* (i) nb of pos for pulse 1 (1..8)       */
     Word16 track_x,                       /* (i) track of pulse 1                   */
     Word16 track_y,                       /* (i) track of pulse 2                   */
     Word16 * ps,                          /* (i/o) correlation of all fixed pulses  */
     Word16 * alp,                         /* (i/o) energy of all fixed pulses       */
     Word16 * ix,                          /* (o) position of pulse 1                */
     Word16 * iy,                          /* (o) position of pulse 2                */
     Word16 dn[],                          /* (i) corr. between target and h[]       */
     Word16 dn2[],                         /* (i) vector of selected positions       */
     Word16 cor_x[],                       /* (i) corr. of pulse 1 with fixed pulses */
     Word16 cor_y[],                       /* (i) corr. of pulse 2 with fixed pulses */
     Word16 rrixiy[][MSIZE]                /* (i) corr. of pulse 1 with pulse 2   */
)
{
    Word16 x, y, pos, thres_ix;
    Word16 ps1, ps2, sq, sqk;
    Word16 alp_16, alpk;
    Word16 *p0, *p1, *p2;
    Word32 s, alp0, alp1, alp2;

    ae_p24x2s aep_sqk, aep_alpk, aep_ps, aep_ps1, aep_dnx, aep_p0, aep_4096;
    ae_p24x2s aep_dny, aep_ps2, aep_p1, aep_8192, aep_p2, aep_alp_16, aep_sq;
    ae_p24x2s aep_y, aep_pos, aep_x, aep_0, aep_ix, aep_iy;
    ae_q56s   aeq_alp0, aeq_alp1, aeq_alp2, aeq_tmp, aeq_0;
    xtbool b;
    xtbool2 b2;

    p0 = cor_x;
    p1 = cor_y;
    p2 = rrixiy[track_x];

    thres_ix = sub(nb_pos_ix, NB_MAX);

    alp0 = L_deposit_h(*alp);
    alp0 = L_add(alp0, 0x00008000L);       /* for rounding */

    /*
    sqk = -1;
    alpk = 1;
    */
    aep_sqk  = AE_CVTP24A16(-1);
    aep_alpk = AE_CVTP24A16(1);
    aeq_alp0 = AE_CVTQ48A32S(alp0);
    aeq_0    = AE_ZEROQ56();

    for (x = track_x; x < AMRWB_L_SUBFR; x += STEP)
    {
        aep_ps  = *((ae_p16s *)ps);
        aep_dnx = *((ae_p16s *)&dn[x]);
        /* ps1 = add(*ps, dn[x]); */
        aep_ps1 = AE_ADDSP24S(aep_ps, aep_dnx);
        aep_ps1 = AE_TRUNCP16(aep_ps1);

        aep_p0   = *((ae_p16s *)p0++);
        aep_4096 = AE_CVTP24A16(4096);
        aep_8192 = AE_CVTP24A16(8192);
        aeq_alp1 = AE_MOVQ56(aeq_alp0);
        /* alp1 = L_mac(alp0, *p0++, 4096); */
        AE_MULAFS32P16S_LL(aeq_alp1, aep_p0, aep_4096);

        if (dn2[x] < thres_ix)
        {
            /* pos = -1;*/
            aep_pos = AE_CVTP24A16(-1);

            for (y = track_y; y < AMRWB_L_SUBFR; y += DSTEP)
            {
                /* Round 1 */
                aep_dny = *((ae_p16s *)&dn[y]);
                /* ps2 = add(ps1, dn[y]);*/
                aep_ps2 = AE_ADDSP24S(aep_ps1, aep_dny);
                #ifdef _MED_AMRWB_PRECISE_
                aep_ps2 = AE_TRUNCP16(aep_ps2);
                #endif

                aep_p1   = *((ae_p16s *)p1++);
                aeq_alp2 = AE_MOVQ56(aeq_alp1);
                /* alp2 = L_mac(alp1, *p1++, 4096);*/
                AE_MULAFS32P16S_LL(aeq_alp2, aep_p1, aep_4096);

                aep_p2   = *((ae_p16s *)p2++);
                /* alp2 = L_mac(alp2, *p2++, 8192);*/
                AE_MULAFS32P16S_LL(aeq_alp2, aep_p2, aep_8192);

                /* alp_16 = extract_h(alp2);*/
                aep_alp_16 = AE_TRUNCP24Q48(aeq_alp2);
                #ifdef _MED_AMRWB_PRECISE_
                aep_alp_16 = AE_TRUNCP16(aep_alp_16);
                #endif

                /* sq = mult(ps2, ps2); */
                aeq_tmp = AE_MULFS32P16S_LL(aep_ps2, aep_ps2);
                aep_sq  = AE_TRUNCP24Q48(aeq_tmp);
                #ifdef _MED_AMRWB_PRECISE_
                aep_sq  = AE_TRUNCP16(aep_sq);
                #endif

                /* s = L_msu(L_mult(alpk, sq), sqk, alp_16);*/
                aeq_tmp = AE_MULFS32P16S_LL(aep_alpk, aep_sq);
                AE_MULSFS32P16S_LL(aeq_tmp, aep_sqk, aep_alp_16);

                b = AE_LTQ56S(aeq_0, aeq_tmp);
                /* if (s > 0)
                {
                    sqk = sq;
                    alpk = alp_16;
                    pos = y;
                }*/
                aep_y = AE_CVTP24A16(y);
                AE_MOVTP48(aep_sqk,  aep_sq, b);
                AE_MOVTP48(aep_alpk, aep_alp_16, b);
                AE_MOVTP48(aep_pos, aep_y, b);

                /* Round 2 */

                aep_dny = *((ae_p16s *)&dn[y+STEP]);
                 /* ps2 = add(ps1, dn[y]);*/
                aep_ps2 = AE_ADDSP24S(aep_ps1, aep_dny);
                #ifdef _MED_AMRWB_PRECISE_
                aep_ps2 = AE_TRUNCP16(aep_ps2);
                #endif

                aep_p1   = *((ae_p16s *)p1++);
                aeq_alp2 = AE_MOVQ56(aeq_alp1);
                /* alp2 = L_mac(alp1, *p1++, 4096);*/
                AE_MULAFS32P16S_LL(aeq_alp2, aep_p1, aep_4096);

                aep_p2   = *((ae_p16s *)p2++);
                /* alp2 = L_mac(alp2, *p2++, 8192);*/
                AE_MULAFS32P16S_LL(aeq_alp2, aep_p2, aep_8192);

                /* alp_16 = extract_h(alp2);*/
                aep_alp_16 = AE_TRUNCP24Q48(aeq_alp2);
                #ifdef _MED_AMRWB_PRECISE_
                aep_alp_16 = AE_TRUNCP16(aep_alp_16);
                #endif

                /* sq = mult(ps2, ps2); */
                aeq_tmp = AE_MULFS32P16S_LL(aep_ps2, aep_ps2);
                aep_sq  = AE_TRUNCP24Q48(aeq_tmp);
                #ifdef _MED_AMRWB_PRECISE_
                aep_sq  = AE_TRUNCP16(aep_sq);
                #endif

                /* s = L_msu(L_mult(alpk, sq), sqk, alp_16);*/
                aeq_tmp = AE_MULFS32P16S_LL(aep_alpk, aep_sq);
                AE_MULSFS32P16S_LL(aeq_tmp, aep_sqk, aep_alp_16);

                b = AE_LTQ56S(aeq_0, aeq_tmp);
                /* if (s > 0)
                {
                    sqk = sq;
                    alpk = alp_16;
                    pos = y;
                }*/
                aep_y = AE_CVTP24A16(y+STEP);
                AE_MOVTP48(aep_sqk,  aep_sq, b);
                AE_MOVTP48(aep_alpk, aep_alp_16, b);
                AE_MOVTP48(aep_pos, aep_y, b);
            }
            p1 -= NB_POS;

            pos = AE_TRUNCA16P24S_H(aep_pos);

            if (pos >= 0)
            {
                *ix = x;
                *iy = pos;
            }
            /*
            aep_x = AE_CVTP24A16(x);
            aep_0 = AE_ZEROP48();
            b2 = AE_LEP24S(aep_0, aep_pos);
            AE_MOVTP24X2(aep_ix, aep_x, b2);
            AE_MOVTP24X2(aep_iy, aep_pos, b2);
            */

        } else
        {
            p2 += NB_POS;
        }
    }

    /*
    *((ae_p16s *)ix) = aep_ix;
    *((ae_p16s *)iy) = aep_iy;
    */

    *ps = add(*ps, add(dn[*ix], dn[*iy]));

    /* *alp = alpk; */
     *((ae_p16s *)alp) = aep_alpk;
    return;
}

#endif
