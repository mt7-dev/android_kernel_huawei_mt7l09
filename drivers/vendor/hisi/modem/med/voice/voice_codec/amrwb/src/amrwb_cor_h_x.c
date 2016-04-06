/*-------------------------------------------------------------------*
 *                         cor_h_x.C                                 *
 *-------------------------------------------------------------------*
 * Compute correlation between target "x[]" and "h[]".               *
 * Designed for codebook search (24 pulses, 4 tracks, 4 pulses per   *
 *    track, 16 positions in each track) to avoid saturation.        *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_math_op.h"
#include "amrwb_cnst.h"

/*#define L_SUBFR   64*/
#define NB_TRACK  4
#define STEP      4


void AMRWB_cor_h_x(
     Word16 h[],                           /* (i) Q12 : impulse response of weighted synthesis filter */
     Word16 x[],                           /* (i) Q0  : target vector                                 */
     Word16 dn[]                           /* (o) <12bit : correlation between target and h[]         */
)
{
    Word32 i, j, k, L_tmp, y32[AMRWB_L_SUBFR], L_max, L_tot;

    /* first keep the result on 32 bits and find absolute maximum */

    L_tot = 1L;

    for (k = 0; k < (NB_TRACK>>1); k++)
    {
        L_max = 0;
        for (i = 2*k; i < AMRWB_L_SUBFR; i += STEP)
        {
            L_tmp = 1L;                      /* 1 -> to avoid null dn[] */

            /*for (j = i; j < L_SUBFR; j++)
                L_tmp = L_mac(L_tmp, x[j], h[j - i]);*/
            L_tmp = CODEC_OpVvMacAlignedEven(&x[i], h, (AMRWB_L_SUBFR - i), L_tmp);

            y32[i] = L_tmp;
            L_tmp = L_abs(L_tmp);
            if (L_tmp > L_max)/*(L_sub(L_tmp, L_max) > (Word32) 0)*/
            {
                L_max = L_tmp;
            }
        }
        /* tot += 3*max / 8 */
        L_max = L_shr(L_max, 2);
        L_tot = L_add(L_tot, L_max);       /* +max/4 */
        L_tot = L_add(L_tot, L_shr(L_max, 1));  /* +max/8 */
    }

    for (k = 0; k < (NB_TRACK>>1); k++)
    {
        L_max = 0;
        for (i = 2*k + 1; i < AMRWB_L_SUBFR; i += STEP)
        {
            L_tmp = 1L;                      /* 1 -> to avoid null dn[] */

            /*for (j = i; j < L_SUBFR; j++)
                L_tmp = L_mac(L_tmp, x[j], h[j - i]);*/
            L_tmp = CODEC_OpVvMacAlignless(&x[i], h, (AMRWB_L_SUBFR - i), L_tmp);

            y32[i] = L_tmp;
            L_tmp = L_abs(L_tmp);
            if (L_tmp > L_max)/*(L_sub(L_tmp, L_max) > (Word32) 0)*/
            {
                L_max = L_tmp;
            }
        }
        /* tot += 3*max / 8 */
        L_max = L_shr(L_max, 2);
        L_tot = L_add(L_tot, L_max);       /* +max/4 */
        L_tot = L_add(L_tot, L_shr(L_max, 1));  /* +max/8 */
    }

    /* Find the number of right shifts to do on y32[] so that    */
    /* 6.0 x sumation of max of dn[] in each track not saturate. */

    j = sub(norm_l(L_tot), 4);             /* 4 -> 16 x tot */

    for (i = 0; i < AMRWB_L_SUBFR; i++)
    {
        dn[i] = round(L_shl(y32[i], j));
    }

    return;
}
