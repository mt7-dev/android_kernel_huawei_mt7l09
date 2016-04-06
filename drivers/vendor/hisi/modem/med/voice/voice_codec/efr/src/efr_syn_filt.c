/*************************************************************************
 *
 *  FUNCTION:  Syn_filt:
 *
 *  PURPOSE:  Perform synthesis filtering through 1/A(z).
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

/* m = LPC order == 10 */
#define m 10

void EFR_Syn_filt (
    Word16 a[],     /* (i)     : a[m+1] prediction coefficients   (m=10)  */
    Word16 x[],     /* (i)     : input signal                             */
    Word16 y[],     /* (o)     : output signal                            */
    Word16 lg,      /* (i)     : size of filtering                        */
    Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
    Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
    Word16 i;
    Word32 s;
    Word16 tmp[80];   /* This is usually done by memory allocation (lg+m) */
    Word16 *yy;

    /* Copy mem[] to yy[] */

    yy = tmp;

    CODEC_OpVecCpy(yy, mem, m);
    yy = yy + m;
    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        s = L_mult (x[i], a[0]);
        /*
        for (j = 1; j <= m; j++)
        {
            s = L_msu (s, a[j], yy[-j]);
        }*/
    	s = CODEC_OpVvMsuD(&a[1], &yy[-1], m, s);

    	s = L_shl (s, 3);
        *yy++ = round (s);
    }

    CODEC_OpVecCpy(y, tmp + m, lg);

    /* Update of memory if update==1 */


    if (update != 0)
    {
        CODEC_OpVecCpy(mem, y + lg - m, m);
    }
    return;
}
