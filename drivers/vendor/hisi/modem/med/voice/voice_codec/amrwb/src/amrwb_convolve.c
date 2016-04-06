/*------------------------------------------------------------------------*
 *                         CONVOLVE.C                                     *
 *------------------------------------------------------------------------*
 * Perform the convolution between two vectors x[] and h[] and            *
 * write the result in the vector y[].                                    *
 * All vectors are of length L.                                           *
 *------------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_op_hifi.h"

//#ifdef _MED_C89_

void AMRWB_Convolve(
     Word16 x[],                           /* (i)        : input vector                           */
     Word16 h[],                           /* (i) Q15    : impulse response                       */
     Word16 y[],                           /* (o) 12 bits: output vector                          */
     Word16 L                              /* (i)        : vector size                            */
)
{
    Word32 n, L_sum;
    Word32 a32[32];
    Word16 *a16 = (Word16 *)a32;
    Word32 h32[32+1];
    Word16 *h16 = (Word16 *)h32;
    Word16 *p1, *p2;

    if (0 != (((Word32)x) & 0x03))
    {
        CODEC_OpVecCpy(a16, x, 64);
    }
    else
    {
        a16 = x;
    }

    if (0 != (((Word32)h) & 0x03))
    {
        CODEC_OpVecCpy(h16, h, 64);
        p1 = h;
        p2 = &h16[1];
    }
    else
    {
        CODEC_OpVecCpy(&h16[1], h, 64);
        p1 = &h16[1];
        p2 = &h[1];
    }

    for (n = 0; n < 64; n+=2)
    {
        /*for (i = 0; i <= n; i++)
            L_sum = L_mac(L_sum, x[i], h[n - i]);*/
        L_sum = CODEC_OpVvMacD(a16, &p1[n], (n + 1), 0);

        y[n] = round(L_sum);

        L_sum = CODEC_OpVvMacD(a16, &p2[n], (n + 2), 0);

        y[n+1] = round(L_sum);
    }

    return;
}

//#endif
