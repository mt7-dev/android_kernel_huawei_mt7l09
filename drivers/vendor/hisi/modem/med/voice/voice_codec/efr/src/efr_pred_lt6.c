/*************************************************************************
 *
 *  FUNCTION:   Pred_lt_6()
 *
 *  PURPOSE:  Compute the result of long term prediction with fractional
 *            interpolation of resolution 1/6. (Interpolated past excitation).
 *
 *  DESCRIPTION:
 *       The past excitation signal at the given delay is interpolated at
 *       the given fraction to build the adaptive codebook excitation.
 *       On return exc[0..L_subfr-1] contains the interpolated signal
 *       (adaptive codebook excitation).
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"

#define UP_SAMP      6
#define FIR_SIZE     (UP_SAMP*EFR_L_INTERPOL+1)

/* 1/6 resolution interpolation filter  (-3 dB at 3600 Hz) */

static const Word16 efr_inter_6[FIR_SIZE] =
{
    29443,
    28346, 25207, 20449, 14701, 8693, 3143,
    -1352, -4402, -5865, -5850, -4673, -2783,
    -672, 1211, 2536, 3130, 2991, 2259,
    1170, 0, -1001, -1652, -1868, -1666,
    -1147, -464, 218, 756, 1060, 1099,
    904, 550, 135, -245, -514, -634,
    -602, -451, -231, 0, 191, 308,
    340, 296, 198, 78, -36, -120,
    -163, -165, -132, -79, -19, 34,
    73, 91, 89, 70, 38, 0
};

void EFR_Pred_lt_6 (
    Word16 exc[],     /* in/out: excitation buffer */
    Word16 T0,        /* input : integer pitch lag */
    Word16 frac,      /* input : fraction of lag   */
    Word16 L_subfr    /* input : subframe size     */
)
{
    Word16 i, j, k;
    Word16 *x0, *x1, *x2;
    const Word16 *c1, *c2;
    Word32 s;

    x0 = &exc[-T0];

    frac = negate (frac);

    if (frac < 0)
    {
        frac = add (frac, UP_SAMP);
        x0--;
    }
    for (j = 0; j < L_subfr; j++)
    {
        x1 = x0++;
        x2 = x0;
        c1 = &efr_inter_6[frac];
        c2 = &efr_inter_6[sub (UP_SAMP, frac)];

        s = 0;
        for (i = 0, k = 0; i < EFR_L_INTERPOL; i++, k += UP_SAMP)
        {
            s = L_mac (s, x1[-i], c1[k]);
            s = L_mac (s, x2[i], c2[k]);
        }

        exc[j] = round (s);
    }

    return;
}
