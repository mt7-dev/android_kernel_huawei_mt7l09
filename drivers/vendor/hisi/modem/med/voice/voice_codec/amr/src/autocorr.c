/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : autocorr.c
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "autocorr.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amr_comm.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : autocorr
*  Purpose     : Compute autocorrelations of signal with windowing
*
**************************************************************************
*/
Word16 Autocorr (
    Word16 x[],            /* (i)    : Input signal (L_WINDOW)            */
    Word16 m,              /* (i)    : LPC order                          */
    Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
    Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
    const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINDOW) */
)
{
    Word16 i, norm;
    Word16 y[L_WINDOW];
    Word32 sum;
    Word16 overfl, overfl_shft;

    /* Windowing of signal */
    CODEC_OpVvMultR(x, wind, L_WINDOW, y);

    /* Compute r[0] and test for overflow */

    overfl_shft = 0;

    do
    {
        overfl = 0;

        sum = CODEC_OpVvMac(y, y, L_WINDOW, 0);

        /* If overflow divide y[] by 4 */
        if ( MAX_32 == sum)
        {
            overfl_shft = add (overfl_shft, 4);
            overfl = 1;                 /* Set the overflow flag */

            CODEC_OpVecShr(&y[0], L_WINDOW, 2, &y[0]);
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
        sum = CODEC_OpVvMac(&y[0],
                          &y[i],
                          L_WINDOW - i,
                          0);

        sum = L_shl (sum, norm);
        L_Extract (sum, &r_h[i], &r_l[i]);
    }

    norm = sub (norm, overfl_shft);

    return norm;
}
