/*************************************************************************
 *
 *  FUNCTION:   Convolve
 *
 *  PURPOSE:
 *     Perform the convolution between two vectors x[] and h[] and
 *     write the result in the vector y[]. All vectors are of length L
 *     and only the first L samples of the convolution are computed.
 *
 *  DESCRIPTION:
 *     The convolution is given by
 *
 *          y[n] = sum_{i=0}^{n} x[i] h[n-i],        n=0,...,L-1
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

void EFR_Convolve (
    Word16 x[],        /* (i)     : input vector                           */
    Word16 h[],        /* (i)     : impulse response                       */
    Word16 y[],        /* (o)     : output vector                          */
    Word16 L           /* (i)     : vector size                            */
)
{
    Word16 n;
    Word32 s;

    for (n = 0; n < L; n++)
    {
    	/*
        s = 0;
        for (i = 0; i <= n; i++)
        {
            s = L_mac (s, x[i], h[n - i]);
        }*/
    	s = CODEC_OpVvMacD(x, &h[n], n+1, 0);

        s = L_shl (s, 3);
        y[n] = extract_h (s);
    }

    return;
}
