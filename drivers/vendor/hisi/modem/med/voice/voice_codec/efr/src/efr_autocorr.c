/*************************************************************************
 *
 *  FUNCTION:  autocorr
 *
 *  PURPOSE:   Compute autocorrelations of signal with windowing
 *
 *  DESCRIPTION:
 *       - Windowing of input speech:   s'[n] = s[n] * w[n]
 *       - Autocorrelations of input speech:
 *             r[k] = sum_{i=k}^{239} s'[i]*s'[i-k]    k=0,...,10
 *         The autocorrelations are expressed in normalized double precision
 *         format.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"

Word16 EFR_Autocorr (
    Word16 x[],         /* (i)    : Input signal                    */
    Word16 m,           /* (i)    : LPC order                       */
    Word16 r_h[],       /* (o)    : Autocorrelations  (msb)         */
    Word16 r_l[],       /* (o)    : Autocorrelations  (lsb)         */
    Word16 wind[]       /* (i)    : window for LPC analysis         */
)
{
    Word16 i, norm;
    Word16 y[EFR_L_WINDOW];
    Word32 sum;
    Word16 overfl, overfl_shft;

    /* Windowing of signal */

    CODEC_OpVvMultR(x, wind, EFR_L_WINDOW, y);

    /* Compute r[0] and test for overflow */

    overfl_shft = 0;

    do
    {
        overfl = 0;

        sum = CODEC_OpVvMac(y, y, EFR_L_WINDOW, 0L);
        /* If overflow divide y[] by 4 */


        if (L_sub (sum, MAX_32) == 0L)
        {
            overfl_shft = add (overfl_shft, 4);
            overfl = 1;                /* Set the overflow flag */

            CODEC_OpVecShr(y, EFR_L_WINDOW, 2, y);
        }

    }
    while (overfl != 0);

    sum = L_add (sum, 1L);             /* Avoid the case of all zeros */

    /* Normalization of r[0] */

    norm = norm_l (sum);
    sum = L_shl (sum, norm);
    L_Extract (sum, &r_h[0], &r_l[0]); /* Put in DPF format (see oper_32b) */

    /* r[1] to r[m] */

    for (i = 1; i <= m; i++)
    {
        sum = CODEC_OpVvMac(y, y + i, EFR_L_WINDOW - i, 0L);

        sum = L_shl (sum, norm);
        L_Extract (sum, &r_h[i], &r_l[i]);
    }

    norm = sub (norm, overfl_shft);

    return norm;
}
