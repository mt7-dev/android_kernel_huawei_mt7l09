/*-------------------------------------------------------------------*
 *                         PRED_LT4.C								 *
 *-------------------------------------------------------------------*
 * Compute the result of long term prediction with fractionnal       *
 * interpolation of resolution 1/4.                                  *
 *                                                                   *
 * On return exc[0..L_subfr-1] contains the interpolated signal      *
 *   (adaptive codebook excitation)                                  *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_op_hifi.h"

#ifdef _MED_C89_

#define UP_SAMP      4
#define L_INTERPOL2  16

/* 1/4 resolution interpolation filter (-3 dB at 0.856*fs/2) in Q14 */

static Word16 inter4_2[UP_SAMP * 2 * L_INTERPOL2] =
{
    0, 1, 2, 1,
    -2, -7, -10, -7,
    4, 19, 28, 22,
    -2, -33, -55, -49,
    -10, 47, 91, 92,
    38, -52, -133, -153,
    -88, 43, 175, 231,
    165, -9, -209, -325,
    -275, -60, 226, 431,
    424, 175, -213, -544,
    -619, -355, 153, 656,
    871, 626, -16, -762,
    -1207, -1044, -249, 853,
    1699, 1749, 780, -923,
    -2598, -3267, -2147, 968,
    5531, 10359, 14031, 15401,
    14031, 10359, 5531, 968,
    -2147, -3267, -2598, -923,
    780, 1749, 1699, 853,
    -249, -1044, -1207, -762,
    -16, 626, 871, 656,
    153, -355, -619, -544,
    -213, 175, 424, 431,
    226, -60, -275, -325,
    -209, -9, 165, 231,
    175, 43, -88, -153,
    -133, -52, 38, 92,
    91, 47, -10, -49,
    -55, -33, -2, 22,
    28, 19, 4, -7,
    -10, -7, -2, 1,
    2, 1, 0, 0
};

void Pred_lt4(
     Word16 exc[],                         /* in/out: excitation buffer */
     Word16 T0,                            /* input : integer pitch lag */
     Word16 frac,                          /* input : fraction of lag   */
     Word16 L_subfr                        /* input : subframe size     */
)
{
    Word16 *x;
    Word32 i, j, k, L_sum;
    Word32 buf[L_INTERPOL2];
    Word16 *pshwBuf = (Word16 *)buf;

    x = &exc[-T0];


    frac = negate(frac);
    if (frac < 0)
    {
        frac = add(frac, UP_SAMP);
        x--;

    }
    x = x - L_INTERPOL2 + 1;


    for (j = 0; j < 64 + 1; j++)
    {
        L_sum = 0L;
        for (i = 0, k = (UP_SAMP - 1 - frac); i < 2 * L_INTERPOL2; i++, k += UP_SAMP)
        {
            /*L_sum = L_mac(L_sum, x[i], inter4_2[k]);*/
            pshwBuf[i] = inter4_2[k];
        }
        L_sum = CODEC_OpVvMac(x, pshwBuf, 2 * L_INTERPOL2, L_sum);

        /*L_sum = L_shl(L_sum, 1);

        exc[j] = round(L_sum);*/
        exc[j] = L_shl_round(L_sum, 1);

        x++;
    }

    return;
}
#endif
