/*************************************************************************
 *
 *  FUNCTION:   Inv_sqrt
 *
 *  PURPOSE:   Computes 1/sqrt(L_x),  where  L_x is positive.
 *             If L_x is negative or zero, the result is 1 (3fff ffff).
 *
 *  DESCRIPTION:
 *       The function 1/sqrt(L_x) is approximated by a table and linear
 *       interpolation. The inverse square root is computed using the
 *       following steps:
 *          1- Normalization of L_x.
 *          2- If (30-exponent) is even then shift right once.
 *          3- exponent = (30-exponent)/2  +1
 *          4- i = bit25-b31 of L_x;  16<=i<=63  because of normalization.
 *          5- a = bit10-b24
 *          6- i -=16
 *          7- L_y = table[i]<<16 - (table[i] - table[i+1]) * a * 2
 *          8- L_y >>= exponent
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_inv_sqrt_tab.c" /* Table for inv_sqrt() */

Word32 EFR_Inv_sqrt (       /* (o) : output value   */
    Word32 L_x          /* (i) : input value    */
)
{
    Word16 exp, i, a, tmp;
    Word32 L_y;


    if (L_x <= (Word32) 0)
        return ((Word32) 0x3fffffffL);

    exp = norm_l (L_x);
    L_x = L_shl (L_x, exp);     /* L_x is normalize */

    exp = sub (30, exp);

    if ((exp & 1) == 0)         /* If exponent even -> shift right */
    {
        L_x = L_shr (L_x, 1);
    }
    exp = shr (exp, 1);
    exp = add (exp, 1);

    L_x = L_shr (L_x, 9);
    i = extract_h (L_x);        /* Extract b25-b31 */
    L_x = L_shr (L_x, 1);
    a = extract_l (L_x);        /* Extract b10-b24 */
    a = a & (Word16) 0x7fff;

    i = sub (i, 16);

    L_y = L_deposit_h (table[i]);       /* table[i] << 16          */
    tmp = sub (table[i], table[i + 1]); /* table[i] - table[i+1])  */
    L_y = L_msu (L_y, tmp, a);  /* L_y -=  tmp*a*2         */

    L_y = L_shr (L_y, exp);     /* denormalization */

    return (L_y);
}
