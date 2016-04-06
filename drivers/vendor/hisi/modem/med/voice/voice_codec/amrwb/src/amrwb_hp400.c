/*-----------------------------------------------------------------------*
 *                         HP400.C										 *
 *-----------------------------------------------------------------------*
 * 2nd order high pass filter with cut off frequency at 400 Hz.          *
 * Designed with cheby2 function in MATLAB.                              *
 * Optimized for fixed-point to get the following frequency response:    *
 *                                                                       *
 *  frequency:     0Hz   100Hz  200Hz  300Hz  400Hz  630Hz  1.5kHz  3kHz *
 *  dB loss:     -infdB  -30dB  -20dB  -10dB  -3dB   +6dB    +1dB    0dB *
 *                                                                       *
 * Algorithm:                                                            *
 *                                                                       *
 *  y[i] = b[0]*x[i] + b[1]*x[i-1] + b[2]*x[i-2]                         *
 *                   + a[1]*y[i-1] + a[2]*y[i-2];                        *
 *                                                                       *
 *  Word16 b[3] = {3660, -7320,  3660};       in Q12                     *
 *  Word16 a[3] = {4096,  7320, -3540};       in Q12                     *
 *                                                                       *
 *  float -->   b[3] = {0.893554687, -1.787109375,  0.893554687};        *
 *              a[3] = {1.000000000,  1.787109375, -0.864257812};        *
 *-----------------------------------------------------------------------*/

#include "amrwb_acelp.h"

/* filter coefficients  */

static Word16 b[3] = {915, -1830, 915};         /* Q12 (/4) */
static Word16 a[3] = {16384, 29280, -14160};    /* Q12 (x4) */


/* Initialization of static values */

void Init_HP400_12k8(Word16 mem[])
{
    CODEC_OpVecSet(mem, 6, 0);
}


void HP400_12k8(
     Word16 signal[],                      /* input signal / output is divided by 16 */
     Word16 lg,                            /* lenght of signal    */
     Word16 mem[]                          /* filter memory [6]   */
)
{
    Word16 x2;
    Word16 y2_hi, y2_lo, y1_hi, y1_lo, x0, x1;
    Word32 i, L_tmp;

    y2_hi = mem[0];
    y2_lo = mem[1];
    y1_hi = mem[2];
    y1_lo = mem[3];
    x0 = mem[4];
    x1 = mem[5];

    for (i = 0; i < lg; i++)
    {
        x2 = x1;
        x1 = x0;
        x0 = signal[i];

        /* y[i] = b[0]*x[i] + b[1]*x[i-1] + b140[2]*x[i-2]  */
        /* + a[1]*y[i-1] + a[2] * y[i-2];  */


        L_tmp = 16384L;                    /* rounding to maximise precision */
        L_tmp = L_mac(L_tmp, y1_lo, a[1]);
        L_tmp = L_mac(L_tmp, y2_lo, a[2]);
        L_tmp = L_shr(L_tmp, 15);
        L_tmp = L_mac(L_tmp, y1_hi, a[1]);
        L_tmp = L_mac(L_tmp, y2_hi, a[2]);
        L_tmp = L_mac(L_tmp, x0, b[0]);
        L_tmp = L_mac(L_tmp, x1, b[1]);
        L_tmp = L_mac(L_tmp, x2, b[2]);

        L_tmp = L_shl(L_tmp, 1);           /* coeff Q12 --> Q13 */

        y2_hi = y1_hi;
        y2_lo = y1_lo;
        L_Extract(L_tmp, &y1_hi, &y1_lo);

        /* signal is divided by 16 to avoid overflow in energy computation */
        signal[i] = round(L_tmp);
    }

    mem[0] = y2_hi;
    mem[1] = y2_lo;
    mem[2] = y1_hi;
    mem[3] = y1_lo;
    mem[4] = x0;
    mem[5] = x1;

    return;
}
