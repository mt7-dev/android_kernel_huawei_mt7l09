/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : convolve.c
*      Purpose          : Perform the convolution between two vectors x[]
*                       : and h[] and write the result in the vector y[].
*                       : All vectors are of length L and only the first
*                       : L samples of the convolution are computed.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "convolve.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"

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
void Convolve (
    Word16 x[],        /* (i)     : input vector                           */
    Word16 h[],        /* (i)     : impulse response                       */
    Word16 y[],        /* (o)     : output vector                          */
    Word16 L           /* (i)     : vector size                            */
)
{
    Word32 n;
    Word32 s;

    for (n = 0; n < L; n++)
    {

        /*s = 0;
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
