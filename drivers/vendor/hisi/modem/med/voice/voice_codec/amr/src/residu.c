/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : residu.c
*      Purpose          : Computes the LP residual.
*      Description      : The LP residual is computed by filtering the input
*                       : speech through the LP inverse filter A(z).
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "residu.h"

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
void Residu (
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

        /*s = L_mult (x[i], a[0]);
        for (j = 1; j <= M; j++)
        {
            s = L_mac (s, a[j], x[i - j]);
        }*/
    	s = CODEC_OpVvMacD(a,&x[i],M+1, 0);
        s = L_shl (s, 3);
        y[i] = round (s);
    }
    return;
}
