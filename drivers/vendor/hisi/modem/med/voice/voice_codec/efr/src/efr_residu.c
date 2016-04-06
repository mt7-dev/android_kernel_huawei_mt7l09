/*************************************************************************
 *
 *  FUNCTION:  Residu
 *
 *  PURPOSE:  Computes the LP residual.
 *
 *  DESCRIPTION:
 *     The LP residual is computed by filtering the input speech through
 *     the LP inverse filter A(z).
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

/* m = LPC order == 10 */
#define m 10

void EFR_Residu (
    Word16 a[], /* (i)     : prediction coefficients                      */
    Word16 x[], /* (i)     : speech signal                                */
    Word16 y[], /* (o)     : residual signal                              */
    Word16 lg   /* (i)     : size of filtering                            */
)
{
    Word16 i;
    Word32 s;

    for (i = 0; i < lg; i++)
    {
    	/*
        s = L_mult (x[i], a[0]);
        for (j = 1; j <= m; j++)
        {
            s = L_mac (s, a[j], x[i - j]);
        }*/
    	s = CODEC_OpVvMacD(a, &x[i], m+1, 0);

        s = L_shl (s, 3);
        y[i] = round (s);
    }
    return;
}
