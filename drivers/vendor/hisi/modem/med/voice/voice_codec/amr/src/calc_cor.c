/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : calc_cor.c
*      Purpose          : Calculate all correlations for prior the OL LTP
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "calc_cor.h"

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
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION: comp_corr
 *
 *  PURPOSE: Calculate all correlations of scal_sig[] in a given delay
 *           range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the all correlations
 *
 *************************************************************************/
void comp_corr (
    Word16 scal_sig[],  /* i   : scaled signal.                          */
    Word16 L_frame,     /* i   : length of frame to compute pitch        */
    Word16 lag_max,     /* i   : maximum lag                             */
    Word16 lag_min,     /* i   : minimum lag                             */
    Word32 corr[])      /* o   : correlation of selected lag             */
{
    Word32 i;
    Word16 *p, *p1;
    Word32 t0;

    for (i = lag_max; i >= lag_min; i--)
    {
       p = scal_sig;
       p1 = &scal_sig[-i];
       t0 = CODEC_OpVvMac( p, p1, L_frame, 0 );
       corr[-i] = t0;
    }

    return;
}
