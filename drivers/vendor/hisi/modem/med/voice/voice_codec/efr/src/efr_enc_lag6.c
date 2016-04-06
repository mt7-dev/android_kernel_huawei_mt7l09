/*************************************************************************
 *
 *   FUNCTION:  Enc_lag6
 *
 *   PURPOSE:  Encoding of fractional pitch lag with 1/6 resolution.
 *
 *   DESCRIPTION:
 *                    First and third subframes:
 *                    --------------------------
 *   The pitch range is divided as follows:
 *           17 3/6  to   94 3/6   resolution 1/6
 *           95      to   143      resolution 1
 *
 *   The period is encoded with 9 bits.
 *   For the range with fractions:
 *     index = (T-17)*6 + frac - 3;
 *                         where T=[17..94] and frac=[-2,-1,0,1,2,3]
 *   and for the integer only range
 *     index = (T - 95) + 463;        where T=[95..143]
 *
 *                    Second and fourth subframes:
 *                    ----------------------------
 *   For the 2nd and 4th subframes a resolution of 1/6 is always used,
 *   and the search range is relative to the lag in previous subframe.
 *   If t0 is the lag in the previous subframe then
 *   t_min=t0-5   and  t_max=t0+4   and  the range is given by
 *       (t_min-1) 3/6   to  (t_max) 3/6
 *
 *   The period in the 2nd (and 4th) subframe is encoded with 6 bits:
 *     index = (T-(t_min-1))*6 + frac - 3;
 *                 where T=[t_min-1..t_max] and frac=[-2,-1,0,1,2,3]
 *
 *   Note that only 61 values are used. If the decoder receives 61, 62,
 *   or 63 as the relative pitch index, it means that a transmission
 *   error occurred and the pitch from previous subframe should be used.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

Word16 EFR_Enc_lag6 (      /* output: Return index of encoding     */
    Word16 T0,         /* input : Pitch delay                  */
    Word16 *T0_frac,   /* in/out: Fractional pitch delay       */
    Word16 *T0_min,    /* in/out: Minimum search delay         */
    Word16 *T0_max,    /* in/out: Maximum search delay         */
    Word16 pit_min,    /* input : Minimum pitch delay          */
    Word16 pit_max,    /* input : Maximum pitch delay          */
    Word16 pit_flag    /* input : Flag for 1st or 3rd subframe */
)
{
    Word16 index, i;


    if (pit_flag == 0)          /* if 1st or 3rd subframe */
    {
        /* encode pitch delay (with fraction) */


        if (sub (T0, 94) <= 0)
        {
            /* index = T0*6 - 105 + *T0_frac */
            i = add (add (T0, T0), T0);
            index = add (sub (add (i, i), 105), *T0_frac);
        } else
        {   /* set fraction to 0 for delays > 94 */
            *T0_frac = 0;
            index = add (T0, 368);
        }

        /* find T0_min and T0_max for second (or fourth) subframe */

        *T0_min = sub (T0, 5);

        if (sub (*T0_min, pit_min) < 0)
        {
            *T0_min = pit_min;
        }
        *T0_max = add (*T0_min, 9);

        if (sub (*T0_max, pit_max) > 0)
        {
            *T0_max = pit_max;
            *T0_min = sub (*T0_max, 9);
        }
    } else
        /* if second or fourth subframe */
    {
        /* index = 6*(T0-*T0_min) + 3 + *T0_frac  */
        i = sub (T0, *T0_min);
        i = add (add (i, i), i);
        index = add (add (add (i, i), 3), *T0_frac);
    }

    return index;
}
