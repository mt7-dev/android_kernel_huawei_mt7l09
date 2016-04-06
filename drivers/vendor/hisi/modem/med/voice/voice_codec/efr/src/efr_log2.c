/*************************************************************************
 *
 *   FUNCTION:   Log2()
 *
 *   PURPOSE:   Computes log2(L_x),  where   L_x is positive.
 *              If L_x is negative or zero, the result is 0.
 *
 *   DESCRIPTION:
 *        The function Log2(L_x) is approximated by a table and linear
 *        interpolation. The following steps are used to compute Log2(L_x)
 *
 *           1- Normalization of L_x.
 *           2- exponent = 30-exponent
 *           3- i = bit25-b31 of L_x;  32<=i<=63  (because of normalization).
 *           4- a = bit10-b24
 *           5- i -=32
 *           6- fraction = table[i]<<16 - (table[i] - table[i+1]) * a * 2
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_log2_tab.c"     /* Table for Log2() */

void EFR_Log2 (
    Word32 L_x,         /* (i) : input value                                 */
    Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1) */
)
{
    Word16 exp, i, a, tmp;
    Word32 L_y;


    if (L_x <= (Word32) 0)
    {
        *exponent = 0;
        *fraction = 0;
        return;
    }
    exp = norm_l (L_x);
    L_x = L_shl (L_x, exp);     /* L_x is normalized */

    *exponent = sub (30, exp);

    L_x = L_shr (L_x, 9);
    i = extract_h (L_x);        /* Extract b25-b31 */
    L_x = L_shr (L_x, 1);
    a = extract_l (L_x);        /* Extract b10-b24 of fraction */
    a = a & (Word16) 0x7fff;

    i = sub (i, 32);

    L_y = L_deposit_h (table[i]);       /* table[i] << 16        */
    tmp = sub (table[i], table[i + 1]); /* table[i] - table[i+1] */
    L_y = L_msu (L_y, tmp, a);  /* L_y -= tmp*a*2        */

    *fraction = extract_h (L_y);

    return;
}
