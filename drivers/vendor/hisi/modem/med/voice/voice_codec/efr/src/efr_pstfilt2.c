/*************************************************************************
 *
 *  FILE NAME:   pstfilt2.c
 *
 * Performs adaptive postfiltering on the synthesis speech
 *
 *  FUNCTIONS INCLUDED:  Init_Post_Filter()  and Post_Filter()
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_codec.h"
#include "efr_cnst.h"

/*---------------------------------------------------------------*
 *    Postfilter constant parameters (defined in "cnst.h")       *
 *---------------------------------------------------------------*
 *   EFR_L_FRAME     : Frame size.                                   *
 *   EFR_L_SUBFR     : Sub-frame size.                               *
 *   EFR_M           : LPC order.                                    *
 *   EFR_MP1         : LPC order+1                                   *
 *   EFR_MU          : Factor for tilt compensation filter           *
 *   EFR_AGC_FAC     : Factor for automatic gain control             *
 *---------------------------------------------------------------*/

#define L_H 22  /* size of truncated impulse response of A(z/g1)/A(z/g2) */

/*------------------------------------------------------------*
 *   static vectors                                           *
 *------------------------------------------------------------*/

 /* inverse filtered synthesis */

static Word16 efr_res2[EFR_L_SUBFR];

 /* memory of filter 1/A(z/0.75) */

static Word16 efr_mem_syn_pst[EFR_M];

 /* Spectral expansion factors */

const Word16 efr_F_gamma3[EFR_M] =
{
    22938, 16057, 11240, 7868, 5508,
    3856, 2699, 1889, 1322, 925
};
const Word16 efr_F_gamma4[EFR_M] =
{
    24576, 18432, 13824, 10368, 7776,
    5832, 4374, 3281, 2461, 1846
};

/*************************************************************************
 *
 *  FUNCTION:   Init_Post_Filter
 *
 *  PURPOSE: Initializes the postfilter parameters.
 *
 *************************************************************************/

void EFR_Init_Post_Filter (void)
{
    CODEC_OpVecSet(efr_mem_syn_pst, EFR_M, 0);

    CODEC_OpVecSet(efr_res2, EFR_L_SUBFR, 0);

    return;
}

/*************************************************************************
 *  FUNCTION:  Post_Filter()
 *
 *  PURPOSE:  postfiltering of synthesis speech.
 *
 *  DESCRIPTION:
 *      The postfiltering process is described as follows:
 *
 *          - inverse filtering of syn[] through A(z/0.7) to get efr_res2[]
 *          - tilt compensation filtering; 1 - EFR_MU*k*z^-1
 *          - synthesis filtering through 1/A(z/0.75)
 *          - adaptive gain control
 *
 *************************************************************************/

void EFR_Post_Filter (
    Word16 *syn,    /* in/out: synthesis speech (postfiltered is output)    */
    Word16 *Az_4    /* input: interpolated LPC parameters in all subframes  */
)
{
    /*-------------------------------------------------------------------*
     *           Declaration of parameters                               *
     *-------------------------------------------------------------------*/

    Word16 syn_pst[EFR_L_FRAME];    /* post filtered synthesis speech   */
    Word16 Ap3[EFR_MP1], Ap4[EFR_MP1];  /* bandwidth expanded LP parameters */
    Word16 *Az;                 /* pointer to Az_4:                 */
                                /*  LPC parameters in each subframe */
    Word16 i_subfr;             /* index for beginning of subframe  */
    Word16 h[L_H];

    Word16 temp1, temp2;
    Word32 L_tmp;

    /*-----------------------------------------------------*
     * Post filtering                                      *
     *-----------------------------------------------------*/

    Az = Az_4;

    for (i_subfr = 0; i_subfr < EFR_L_FRAME; i_subfr += EFR_L_SUBFR)
    {
        /* Find weighted filter coefficients Ap3[] and ap[4] */

        EFR_Weight_Ai (Az, efr_F_gamma3, Ap3);
        EFR_Weight_Ai (Az, efr_F_gamma4, Ap4);

        /* filtering of synthesis speech by A(z/0.7) to find efr_res2[] */

        EFR_Residu (Ap3, &syn[i_subfr], efr_res2, EFR_L_SUBFR);

        /* tilt compensation filter */

        /* impulse response of A(z/0.7)/A(z/0.75) */

        CODEC_OpVecCpy(h, Ap3, EFR_M + 1);
        CODEC_OpVecSet(&h[EFR_M + 1], L_H - EFR_M - 1, 0);
        EFR_Syn_filt (Ap4, h, h, L_H, &h[EFR_M + 1], 0);

        /* 1st correlation of h[] */

        L_tmp = CODEC_OpVvMac(h, h, L_H, 0L);
        temp1 = extract_h (L_tmp);

        L_tmp = CODEC_OpVvMac(h, h + 1, L_H - 1, 0L);
        temp2 = extract_h (L_tmp);


        if (temp2 <= 0)
        {
            temp2 = 0;
        }
        else
        {
            temp2 = mult (temp2, EFR_MU);
            temp2 = div_s (temp2, temp1);
        }

        EFR_preemphasis (efr_res2, temp2, EFR_L_SUBFR);

        /* filtering through  1/A(z/0.75) */

        EFR_Syn_filt (Ap4, efr_res2, &syn_pst[i_subfr], EFR_L_SUBFR, efr_mem_syn_pst, 1);

        /* scale output to input */

        EFR_agc (&syn[i_subfr], &syn_pst[i_subfr], EFR_AGC_FAC, EFR_L_SUBFR);

        Az += EFR_MP1;
    }

    /* update syn[] buffer */

    CODEC_OpVecCpy(&syn[-EFR_M], &syn[EFR_L_FRAME - EFR_M], EFR_M);

    /* overwrite synthesis speech by postfiltered synthesis speech */

    CODEC_OpVecCpy(syn, syn_pst, EFR_L_FRAME);

    return;
}
