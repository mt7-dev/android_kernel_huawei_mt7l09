/*************************************************************************
 *
 *  FUNCTION:   Pitch_fr6()
 *
 *  PURPOSE: Find the pitch period with 1/6 subsample resolution (closed loop).
 *
 *  DESCRIPTION:
 *        - find the normalized correlation between the target and filtered
 *          past excitation in the search range.
 *        - select the delay with maximum normalized correlation.
 *        - interpolate the normalized correlation at fractions -3/6 to 3/6
 *          with step 1/6 around the chosen delay.
 *        - The fraction which gives the maximum interpolated value is chosen.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_codec.h"

 /* L_inter = Length for fractional interpolation = nb.coeff/2 */

#define L_inter 4

 /* Local functions */

void EFR_Norm_Corr (Word16 exc[], Word16 xn[], Word16 h[], Word16 L_subfr,
                Word16 t_min, Word16 t_max, Word16 corr_norm[]);

Word16 EFR_Pitch_fr6 (    /* (o)     : pitch period.                          */
    Word16 exc[],     /* (i)     : excitation buffer                      */
    Word16 xn[],      /* (i)     : target vector                          */
    Word16 h[],       /* (i)     : impulse response of synthesis and
                                    weighting filters                     */
    Word16 L_subfr,   /* (i)     : Length of subframe                     */
    Word16 t0_min,    /* (i)     : minimum value in the searched range.   */
    Word16 t0_max,    /* (i)     : maximum value in the searched range.   */
    Word16 i_subfr,   /* (i)     : indicator for first subframe.          */
    Word16 *pit_frac  /* (o)     : chosen fraction.                       */
)
{
    Word16 i;
    Word16 t_min, t_max;
    Word16 max, lag, frac;
    Word16 *corr;
    Word16 corr_int;
    Word16 corr_v[40];          /* Total length = t0_max-t0_min+1+2*L_inter */

    /* Find interval to compute normalized correlation */

    t_min = sub (t0_min, L_inter);
    t_max = add (t0_max, L_inter);

    corr = &corr_v[-t_min];

    /* Compute normalized correlation between target and filtered excitation */

    EFR_Norm_Corr (exc, xn, h, L_subfr, t_min, t_max, corr);

    /* Find integer pitch */

    max = corr[t0_min];

    max = CODEC_OpVecMax(corr+ t0_min, t0_max - t0_min + 1, &lag);
    lag = lag + t0_min;
    /* If first subframe and lag > 94 do not search fractional pitch */


    if ((i_subfr == 0) && (sub (lag, 94) > 0))
    {
        *pit_frac = 0;
        return (lag);
    }
    /* Test the fractions around T0 and choose the one which maximizes   */
    /* the interpolated normalized correlation.                          */

    max = EFR_Interpol_6 (&corr[lag], -3);
    frac = -3;

    for (i = -2; i <= 3; i++)
    {
        corr_int = EFR_Interpol_6 (&corr[lag], i);

        if (sub (corr_int, max) > 0)
        {
            max = corr_int;
            frac = i;
        }
    }

    /* Limit the fraction value in the interval [-2,-1,0,1,2,3] */


    if (sub (frac, -3) == 0)
    {
        frac = 3;
        lag = sub (lag, 1);
    }
    *pit_frac = frac;

    return (lag);
}

/*************************************************************************
 *
 *  FUNCTION:   Norm_Corr()
 *
 *  PURPOSE: Find the normalized correlation between the target vector
 *           and the filtered past excitation.
 *
 *  DESCRIPTION:
 *     The normalized correlation is given by the correlation between the
 *     target and filtered past excitation divided by the square root of
 *     the energy of filtered excitation.
 *                   corr[k] = <x[], y_k[]>/sqrt(y_k[],y_k[])
 *     where x[] is the target vector and y_k[] is the filtered past
 *     excitation at delay k.
 *
 *************************************************************************/

void
EFR_Norm_Corr (Word16 exc[], Word16 xn[], Word16 h[], Word16 L_subfr,
           Word16 t_min, Word16 t_max, Word16 corr_norm[])
{
    Word32 i, j, k;
    Word16 corr_h, corr_l, norm_h, norm_l;
    Word32 s;

    /* Usally dynamic allocation of (L_subfr) */
    Word16 excf[80];
    Word16 scaling, h_fac, *s_excf, scaled_excf[80];

    k = -t_min;

    /* compute the filtered excitation for the first delay t_min */

    EFR_Convolve (&exc[k], h, excf, L_subfr);

    /* scale "excf[]" to avoid overflow */

    CODEC_OpVecShr(excf, L_subfr, 2, scaled_excf);

    /* Compute 1/sqrt(energy of excf[]) */

    s = CODEC_OpVvMac(excf, excf, L_subfr, 0);

    if (L_sub (s, 67108864L) <= 0)             /* if (s <= 2^26) */
    {
        s_excf = excf;
        h_fac = 15 - 12;
        scaling = 0;
    }
    else
    {
        /* "excf[]" is divided by 2 */
        s_excf = scaled_excf;
        h_fac = 15 - 12 - 2;
        scaling = 2;
    }

    /* loop for every possible period */

    for (i = t_min; i <= t_max; i++)
    {
        /* Compute 1/sqrt(energy of excf[]) */

        s = CODEC_OpVvMac(s_excf, s_excf, L_subfr, 0);

        s = EFR_Inv_sqrt (s);
        L_Extract (s, &norm_h, &norm_l);

        /* Compute correlation between xn[] and excf[] */

        s = CODEC_OpVvMac(xn, s_excf, L_subfr, 0);

        L_Extract (s, &corr_h, &corr_l);

        /* Normalize correlation = correlation * (1/sqrt(energy)) */

        s = Mpy_32 (corr_h, corr_l, norm_h, norm_l);

        corr_norm[i] = extract_h (L_shl (s, 16));


        /* modify the filtered excitation excf[] for the next iteration */


        if (sub (i, t_max) != 0)
        {
            k--;
            for (j = L_subfr - 1; j > 0; j--)
            {
                s = L_mult (exc[k], h[j]);
                s = L_shl (s, h_fac);
                s_excf[j] = add (extract_h (s), s_excf[j - 1]);
            }
            s_excf[0] = shr (exc[k], scaling);
        }
    }
    return;
}
