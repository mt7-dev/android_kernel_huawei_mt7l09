/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : g_code.c
*      Purpose          : Compute the innovative codebook gain.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_code.h"

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
 *  FUNCTION:   G_code
 *
 *  PURPOSE:  Compute the innovative codebook gain.
 *
 *  DESCRIPTION:
 *      The innovative codebook gain is given by
 *
 *              g = <x[], y[]> / <y[], y[]>
 *
 *      where x[] is the target vector, y[] is the filtered innovative
 *      codevector, and <> denotes dot product.
 *
 *************************************************************************/
Word16 G_code (         /* out   : Gain of innovation code         */
    Word16 xn2[],       /* in    : target vector                   */
    Word16 y2[]         /* in    : filtered innovation vector      */
)
{
    Word16 i;
    Word16 xy, yy, exp_xy, exp_yy, gain;
    Word16 scal_y2[L_SUBFR];
    Word32 s;

    /* Scale down Y[] by 2 to avoid overflow */

    CODEC_OpVecShr(&y2[0], L_SUBFR, 1, &scal_y2[0]);

    /* Compute scalar product <X[],Y[]> */
    s = CODEC_OpVvMac(xn2, scal_y2, L_SUBFR, 1);

    exp_xy = norm_l (s);
    xy = extract_h (L_shl (s, exp_xy));

    /* If (xy < 0) gain = 0  */

    if (xy <= 0)
        return ((Word16) 0);

    /* Compute scalar product <Y[],Y[]> */
    s = CODEC_OpVvMac(scal_y2, scal_y2, L_SUBFR, 0);

    exp_yy = norm_l (s);
    yy = extract_h (L_shl (s, exp_yy));

    /* compute gain = xy/yy */

    xy = shr (xy, 1);                 /* Be sure xy < yy */
    gain = div_s (xy, yy);

    /* Denormalization of division */
    i = add (exp_xy, 5);              /* 15-1+9-18 = 5 */
    i = sub (i, exp_yy);

    gain = shl (shr (gain, i), 1);    /* Q0 -> Q1 */

    return (gain);
}
