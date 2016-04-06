/*************************************************************************
 *
 *  FUNCTION:  Pitch_ol
 *
 *  PURPOSE: Compute the open loop pitch lag.
 *
 *  DESCRIPTION:
 *      The open-loop pitch lag is determined based on the perceptually
 *      weighted speech signal. This is done in the following steps:
 *        - find three maxima of the correlation <sw[n],sw[n-T]> in the
 *          follwing three ranges of T : [18,35], [36,71], and [72, 143]
 *        - divide each maximum by <sw[n-t], sw[n-t]> where t is the delay at
 *          that maximum correlation.
 *        - select the delay of maximum normalized correlation (among the
 *          three candidates) while favoring the lower delay ranges.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"

#define THRESHOLD 27853

/* local function */

static Word16 EFR_Lag_max (   /* output: lag found                              */
    Word16 scal_sig[],    /* input : scaled signal                          */
    Word16 scal_fac,      /* input : scaled signal factor                   */
    Word16 L_frame,       /* input : length of frame to compute pitch       */
    Word16 lag_max,       /* input : maximum lag                            */
    Word16 lag_min,       /* input : minimum lag                            */
    Word16 *cor_max);     /* output: normalized correlation of selected lag */

Word16 EFR_Pitch_ol (      /* output: open loop pitch lag                        */
    Word16 signal[],   /* input : signal used to compute the open loop pitch */
                       /*     signal[-pit_max] to signal[-1] should be known */
    Word16 pit_min,    /* input : minimum pitch lag                          */
    Word16 pit_max,    /* input : maximum pitch lag                          */
    Word16 L_frame     /* input : length of frame to compute pitch           */
)
{
    Word16 i, j;
    Word16 max1, max2, max3;
    Word16 p_max1, p_max2, p_max3;
    Word32 t0;

    /* Scaled signal                                                */
    /* Can be allocated with memory allocation of(pit_max+L_frame)  */

    Word16 scaled_signal[512];
    Word16 *scal_sig, scal_fac;

    scal_sig = &scaled_signal[pit_max];

    t0 = CODEC_OpVvMac(signal - pit_max, signal - pit_max, L_frame + pit_max, 0L);
    /*--------------------------------------------------------*
     * Scaling of input signal.                               *
     *                                                        *
     *   if Overflow        -> scal_sig[i] = signal[i]>>2     *
     *   else if t0 < 1^22  -> scal_sig[i] = signal[i]<<2     *
     *   else               -> scal_sig[i] = signal[i]        *
     *--------------------------------------------------------*/

    /*--------------------------------------------------------*
     *  Verification for risk of overflow.                    *
     *--------------------------------------------------------*/


    if (L_sub (t0, MAX_32) == 0L)               /* Test for overflow */
    {
        CODEC_OpVecShr(signal - pit_max, L_frame + pit_max, 3, scal_sig - pit_max);
        scal_fac = 3;
    }
    else if (L_sub (t0, (Word32) 1048576L) < (Word32) 0)
        /* if (t0 < 2^20) */
    {
        CODEC_OpVecShl(signal - pit_max, L_frame + pit_max, 3, scal_sig - pit_max);
        scal_fac = -3;
    }
    else
    {
        CODEC_OpVecCpy(scal_sig - pit_max, signal - pit_max, L_frame + pit_max);
        scal_fac = 0;
    }

    /*--------------------------------------------------------------------*
     *  The pitch lag search is divided in three sections.                *
     *  Each section cannot have a pitch multiple.                        *
     *  We find a maximum for each section.                               *
     *  We compare the maximum of each section by favoring small lags.    *
     *                                                                    *
     *  First section:  lag delay = pit_max     downto 4*pit_min          *
     *  Second section: lag delay = 4*pit_min-1 downto 2*pit_min          *
     *  Third section:  lag delay = 2*pit_min-1 downto pit_min            *
     *-------------------------------------------------------------------*/

    j = shl (pit_min, 2);
    p_max1 = EFR_Lag_max (scal_sig, scal_fac, L_frame, pit_max, j, &max1);

    i = sub (j, 1);
    j = shl (pit_min, 1);
    p_max2 = EFR_Lag_max (scal_sig, scal_fac, L_frame, i, j, &max2);

    i = sub (j, 1);
    p_max3 = EFR_Lag_max (scal_sig, scal_fac, L_frame, i, pit_min, &max3);

    /*--------------------------------------------------------------------*
     * Compare the 3 sections maximum, and favor small lag.               *
     *-------------------------------------------------------------------*/


    if (sub (mult (max1, THRESHOLD), max2) < 0)
    {
        max1 = max2;
        p_max1 = p_max2;
    }

    if (sub (mult (max1, THRESHOLD), max3) < 0)
    {
        p_max1 = p_max3;
    }
    return (p_max1);
}

/*************************************************************************
 *
 *  FUNCTION:  Lag_max
 *
 *  PURPOSE: Find the lag that has maximum correlation of scal_sig[] in a
 *           given delay range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the maximum correlation after normalization
 *      and the corresponding lag.
 *
 *************************************************************************/

static Word16 EFR_Lag_max ( /* output: lag found                               */
    Word16 scal_sig[],  /* input : scaled signal.                          */
    Word16 scal_fac,    /* input : scaled signal factor.                   */
    Word16 L_frame,     /* input : length of frame to compute pitch        */
    Word16 lag_max,     /* input : maximum lag                             */
    Word16 lag_min,     /* input : minimum lag                             */
    Word16 *cor_max)    /* output: normalized correlation of selected lag  */
{
    Word16 i;
    Word16 *p, *p1;
    Word32 max, t0;
    Word16 max_h, max_l, ener_h, ener_l;
    Word16 p_max;

    max = MIN_32;

    for (i = lag_max; i >= lag_min; i--)
    {
        p = scal_sig;
        p1 = &scal_sig[-i];

        t0 = CODEC_OpVvMac(p, p1, L_frame, 0);
        if (L_sub (t0, max) >= 0)
        {
            max = t0;
            p_max = i;
        }
    }

    /* compute energy */

    p = &scal_sig[-p_max];

    t0 = CODEC_OpVvMac(p, p, L_frame, 0);
    /* 1/sqrt(energy) */

    t0 = EFR_Inv_sqrt (t0);
    t0 = L_shl (t0, 1);

    /* max = max/sqrt(energy)  */

    L_Extract (max, &max_h, &max_l);
    L_Extract (t0, &ener_h, &ener_l);

    t0 = Mpy_32 (max_h, max_l, ener_h, ener_l);
    t0 = L_shr (t0, scal_fac);

    *cor_max = extract_h (L_shl (t0, 15));      /* divide by 2 */

    return (p_max);
}
