/*************************************************************************
 *
 *  FUNCTION:  agc
 *
 *  PURPOSE: Scales the postfilter output on a subframe basis by automatic
 *           control of the subframe gain.
 *
 *  DESCRIPTION:
 *   sig_out[n] = sig_out[n] * gain[n];
 *   where gain[n] is the gain at the nth sample given by
 *     gain[n] = agc_fac * gain[n-1] + (1 - agc_fac) g_in/g_out
 *   g_in/g_out is the square root of the ratio of energy at the input
 *   and output of the postfilter.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_cnst.h"

Word16 efr_past_gain;               /* initial value of past_gain = 1.0  */

void EFR_agc (
    Word16 *sig_in,             /* (i)     : postfilter input signal  */
    Word16 *sig_out,            /* (i/o)   : postfilter output signal */
    Word16 agc_fac,             /* (i)     : AGC factor               */
    Word16 l_trm                /* (i)     : subframe size            */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0, gain;
    Word32 s;

    /* calculate gain_out with exponent */

    s = CODEC_OpVecScaleMac(sig_out, l_trm, 0, 2);

    if (s == 0)
    {
        efr_past_gain = 0;
        return;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */

    s = CODEC_OpVecScaleMac(sig_in, l_trm, 0, 2);
    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = (1-agc_fac) * sqrt(gain_in/gain_out);       *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = EFR_Inv_sqrt (s);
        i = round (L_shl (s, 9));

        /* g0 = i * (1-agc_fac) */
        g0 = mult (i, sub (32767, agc_fac));
    }

    /* compute gain[n] = agc_fac * gain[n-1]
                        + (1-agc_fac) * sqrt(gain_in/gain_out) */
    /* sig_out[n] = gain[n] * sig_out[n]                        */

    gain = efr_past_gain;

    for (i = 0; i < l_trm; i++)
    {
        gain = mult (gain, agc_fac);
        gain = add (gain, g0);
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], gain), 3));

    }

    efr_past_gain = gain;

    return;
}

void EFR_agc2 (
 Word16 *sig_in,        /* (i)     : postfilter input signal  */
 Word16 *sig_out,       /* (i/o)   : postfilter output signal */
 Word16 l_trm           /* (i)     : subframe size            */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0;
    Word32 s;


    /* calculate gain_out with exponent */

    s = CODEC_OpVecScaleMac(sig_out, l_trm, 0, 2);

    if (s == 0)
    {
        return;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */

    s = CODEC_OpVecScaleMac(sig_in, l_trm, 0, 2);

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = EFR_Inv_sqrt (s);
        g0 = round (L_shl (s, 9));
    }

    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], g0), 3));

    }

    return;
}
