/*-----------------------------------------------------------------------*
 *                         RESIDU.C										 *
 *-----------------------------------------------------------------------*
 * Compute the LPC residual by filtering the input speech through A(z)   *
 *-----------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"
#include "amrwb_op_hifi.h"

//#ifdef _MED_C89_

void AMRWB_Residu(
     Word16 a[],                           /* (i) Q12 : prediction coefficients                     */
     Word16 m,                             /* (i)     : order of LP filter                          */
     Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed         */
     Word16 y[],                           /* (o) x2  : residual signal                             */
     Word16 lg                             /* (i)     : size of filtering                           */
)
{
    Word32 i, s;
    Word32 a32[AMRWB_M/2];
    Word16 *a16 = (Word16 *)a32;

    if (0 == (((Word32)a) & 0x03))
    {
        CODEC_OpVecCpy(a16, &a[1], AMRWB_M);
    }
    else
    {
        a16 = &a[1];
    }

    for (i = 0; i < lg; i += 2)
    {
        s = L_mult(x[i], a[0]);

        /*for (j = 1; j <= m; j++)
            s = L_mac(s, a[j], x[i - j]);*/
        s = CODEC_OpVvMacD(a16, &x[i-1], AMRWB_M, s);

        y[i] = L_shl_round(s, 4);

        s = L_mult(x[i+1], a[0]);

        s = CODEC_OpVvMacD(a16, &x[i], AMRWB_M, s);

        y[i+1] = L_shl_round(s, 4);
    }

    return;
}
//#endif
