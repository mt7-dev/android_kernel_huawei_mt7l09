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

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"

Word16 EFR_G_code (         /* out   : Gain of innovation code         */
    Word16 xn2[],       /* in    : target vector                   */
    Word16 y2[]         /* in    : filtered innovation vector      */
)
{
    Word16 i;
    Word16 xy, yy, exp_xy, exp_yy, gain;
    Word16 scal_y2[EFR_L_SUBFR];
    Word32 s;

    /* Scale down Y[] by 2 to avoid overflow */

    CODEC_OpVecShr(y2, EFR_L_SUBFR, 1, scal_y2);
    /* Compute scalar product <X[],Y[]> */

    s = CODEC_OpVvMac(xn2, scal_y2, EFR_L_SUBFR, 1L);/* Avoid case of all zeros */
    exp_xy = norm_l (s);
    xy = extract_h (L_shl (s, exp_xy));

    /* If (xy < 0) gain = 0  */


    if (xy <= 0)
        return ((Word16) 0);

    /* Compute scalar product <Y[],Y[]> */

    s = CODEC_OpVvMac(scal_y2, scal_y2, EFR_L_SUBFR, 1L);/* Avoid case of all zeros */
    exp_yy = norm_l (s);
    yy = extract_h (L_shl (s, exp_yy));

    /* compute gain = xy/yy */

    xy = shr (xy, 1);                 /* Be sure xy < yy */
    gain = div_s (xy, yy);

    /* Denormalization of division */
    i = add (exp_xy, 5);              /* 15-1+9-18 = 5 */
    i = sub (i, exp_yy);

    gain = shr (gain, i);

    return (gain);
}
