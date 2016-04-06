/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : syn_filt.c
*      Purpose          : Perform synthesis filtering through 1/A(z).
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "syn_filt.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------*
* Constants (defined in cnst.h         *
*--------------------------------------*
*  M         : LPC order               *
*--------------------------------------*
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
void Syn_filt (
    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
    Word16 x[],     /* (i)     : input signal                             */
    Word16 y[],     /* (o)     : output signal                            */
    Word16 lg,      /* (i)     : size of filtering                        */
    Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
    Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
    Word32 i;
    Word32 s;
    Word16 tmp[80];   /* This is usually done by memory allocation (lg+M) */
    Word16 *yy;

    /* Copy mem[] to yy[] */

    yy = tmp;

    CODEC_OpVecCpy(&yy[0], &mem[0], M);

    yy += M;

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        s = L_mult (x[i], a[0]);
        /*
        for (j = 1; j <= M; j++)
        {
            s = L_msu (s, a[j], yy[-j]);
        }*/
        s = CODEC_OpVvMsuD(&a[1], &yy[-1], M, s);
        s = L_shl (s, 3);
        *yy++ = round (s);
    }

    CODEC_OpVecCpy(&y[0], &tmp[M], lg);

    /* Update of memory if update==1 */

    if (update != 0)
    {
        CODEC_OpVecCpy(&mem[0], &y[lg - M], M);
    }
    return;
}
