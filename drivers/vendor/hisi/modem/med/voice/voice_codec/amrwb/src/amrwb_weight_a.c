/*-------------------------------------------------------------------*
 *                         WB_VAD.C                                  *
 *-------------------------------------------------------------------*
 * Weighting of LPC coefficients.                                    *
 *   ap[i]  =  a[i] * (gamma ** i)                                   *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"

void Weight_a(
     Word16 a[],                           /* (i) Q12 : a[m+1]  LPC coefficients             */
     Word16 ap[],                          /* (o) Q12 : Spectral expanded LPC coefficients   */
     Word16 gamma,                         /* (i) Q15 : Spectral expansion factor.           */
     Word16 m                              /* (i)     : LPC order.                           */
)
{
    Word16 fac;
    Word32 i;

    ap[0] = a[0];
    fac = gamma;
    for (i = 1; i < m; i++)
    {
        ap[i] = round(L_mult(a[i], fac));
        fac = round(L_mult(fac, gamma));
    }
    ap[m] = round(L_mult(a[m], fac));

    return;
}
