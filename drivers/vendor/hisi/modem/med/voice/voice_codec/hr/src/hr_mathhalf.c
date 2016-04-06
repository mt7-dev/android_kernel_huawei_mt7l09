/***************************************************************************
 *
 *   File Name:  hr_mathhalf.c
 *
 *   Purpose:  Contains functions which implement the primitive
 *     arithmetic operations.
 *
 *      The functions in this file are listed below.  Some of them are
 *      defined in terms of other basic operations.  One of the
 *      routines, saturate() is static.  This is not a basic
 *      operation, and is not referenced outside the scope of this
 *      file.
 *
 *       L_shift_r()
 *       shift_r()
 *       isLwLimit()
 *       isSwLimit()
 *       L_mpy_ll()
 *       L_mpy_ls()
 *
 **************************************************************************/

/*_________________________________________________________________________
 |                                                                         |
 |                            Include Files                                |
 |_________________________________________________________________________|
*/

#include "hr_typedefs.h"
#include "hr_mathhalf.h"


/***************************************************************************
 *
 *   FUNCTION NAME: L_shift_r
 *
 *   PURPOSE:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (Longword) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *     var2
 *                     16 bit short signed integer (Shortword) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_var1
 *                     32 bit long signed integer (Longword) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.  This is just like shift_r above except that the
 *     input/output is 32 bits as opposed to 16.
 *
 *     if var2 is positve perform a arithmetic left shift
 *     with saturation (see L_shl() above).
 *
 *     If var2 is zero simply return L_var1.
 *
 *     If var2 is negative perform a arithmetic right shift (L_shr)
 *     of L_var1 by (-var2)+1.  Add the LS bit of the result to
 *     L_var1 shifted right (L_shr) by -var2.
 *
 *     Note that there is no constraint on var2, so if var2 is
 *     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
 *     This is the reason the L_shl function is used.
 *
 *
 *   KEYWORDS:
 *
 *************************************************************************/

Longword L_shift_r(Longword L_var1, Shortword var2)
{
  Longword L_Out,
         L_rnd;

  if (var2 < -31)
  {
    L_Out = 0;
  }
  else if (var2 < 0)
  {
    /* right shift */
    L_rnd = L_shl(L_var1, var2 + 1) & 0x1;
    L_Out = L_add(L_shl(L_var1, var2), L_rnd);
  }
  else
    L_Out = L_shl(L_var1, var2);

  return (L_Out);
}


/***************************************************************************
 *
 *   FUNCTION NAME: shift_r
 *
 *   PURPOSE:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (Shortword) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (Shortword) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (Shortword) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *     If var2 is positive perform a arithmetic left shift
 *     with saturation (see shl() above).
 *
 *     If var2 is zero simply return var1.
 *
 *     If var2 is negative perform a arithmetic right shift (shr)
 *     of var1 by (-var2)+1.  Add the LS bit of the result to var1
 *     shifted right (shr) by -var2.
 *
 *     Note that there is no constraint on var2, so if var2 is
 *     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
 *     This is the reason the shl function is used.
 *
 *
 *   KEYWORDS:
 *
 *************************************************************************/

Shortword shift_r(Shortword var1, Shortword var2)
{
  Shortword swOut,
         swRnd;

  if (var2 >= 0)
    swOut = shl(var1, var2);
  else
  {

    /* right shift */

    if (var2 < -15)
    {

      swOut = 0;

    }
    else
    {

      swRnd = shl(var1, var2 + 1) & 0x1;
      swOut = add(shl(var1, var2), swRnd);

    }
  }
  return (swOut);
}


/****************************************************************************
 *
 *     FUNCTION NAME: isLwLimit
 *
 *     PURPOSE:
 *
 *        Check to see if the input Longword is at the
 *        upper or lower limit of its range.  i.e.
 *        0x7fff ffff or -0x8000 0000
 *
 *        Ostensibly this is a check for an overflow.
 *        This does not truly mean an overflow occurred,
 *        it means the value reached is the
 *        maximum/minimum value representable.  It may
 *        have come about due to an overflow.
 *
 *     INPUTS:
 *
 *       L_In               A Longword input variable
 *
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        1 if input == 0x7fff ffff or -0x8000 0000
 *                          0 otherwise
 *
 *     KEYWORDS: saturation, limit
 *
 ***************************************************************************/

Shortword  isLwLimit(Longword L_In)
{

  Longword   L_ls;
  Shortword  siOut;

  if (L_In != 0)
  {
    L_ls = L_shl(L_In, 1);
    if (L_sub(L_In, L_ls) == 0)
      siOut = 1;
    else
      siOut = 0;
  }
  else
  {
    siOut = 0;
  }
  return (siOut);
}

/****************************************************************************
 *
 *     FUNCTION NAME: isSwLimit
 *
 *     PURPOSE:
 *
 *        Check to see if the input Shortword is at the
 *        upper or lower limit of its range.  i.e.
 *        0x7fff or -0x8000
 *
 *        Ostensibly this is a check for an overflow.
 *        This does not truly mean an overflow occurred,
 *        it means the value reached is the
 *        maximum/minimum value representable.  It may
 *        have come about due to an overflow.
 *
 *     INPUTS:
 *
 *       swIn               A Shortword input variable
 *
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        1 if input == 0x7fff or -0x8000
 *                          0 otherwise
 *
 *     KEYWORDS: saturation, limit
 *
 ***************************************************************************/

Shortword  isSwLimit(Shortword swIn)
{

  Shortword swls;
  Shortword siOut;

  if (swIn != 0)
  {
    swls = shl(swIn, 1);
    if (sub(swIn, swls) == 0)          /* logical compare outputs 1/0 */
      siOut = 1;
    else
      siOut = 0;
  }
  else
  {
    siOut = 0;
  }
  return (siOut);

}

/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ll
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var1) and a 32 bit number
 *                 (L_var2), and return a 32 bit result.
 *
 *     INPUTS:
 *
 *       L_var1             A Longword input variable
 *
 *       L_var2             A Longword input variable
 *
 *     OUTPUTS:             none
 *
 *     IMPLEMENTATION:
 *
 *        Performs a 31x31 bit multiply, Complexity=24 Ops.
 *
 *        Let x1x0, or y1y0, be the two constituent halves
 *        of a 32 bit number.  This function performs the
 *        following:
 *
 *        low = ((x0 >> 1)*(y0 >> 1)) >> 16     (low * low)
 *        mid1 = [(x1 * (y0 >> 1)) >> 1 ]       (high * low)
 *        mid2 = [(y1 * (x0 >> 1)) >> 1]        (high * low)
 *        mid =  (mid1 + low + mid2) >> 14      (sum so far)
 *        output = (y1*x1) + mid                (high * high)
 *
 *
 *     RETURN VALUE:        A Longword value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/

Longword L_mpy_ll(Longword L_var1, Longword L_var2)
{
  Shortword swLow1,
         swLow2,
         swHigh1,
         swHigh2;
  Longword L_varOut,
         L_low,
         L_mid1,
         L_mid2,
         L_mid;


  swLow1 = shr(extract_l(L_var1), 1);
  swLow1 = HR_SW_MAX & swLow1;

  swLow2 = shr(extract_l(L_var2), 1);
  swLow2 = HR_SW_MAX & swLow2;
  swHigh1 = extract_h(L_var1);
  swHigh2 = extract_h(L_var2);

  L_low = L_mult(swLow1, swLow2);
  L_low = L_shr(L_low, 16);

  L_mid1 = L_mult(swHigh1, swLow2);
  L_mid1 = L_shr(L_mid1, 1);
  L_mid = L_add(L_mid1, L_low);

  L_mid2 = L_mult(swHigh2, swLow1);
  L_mid2 = L_shr(L_mid2, 1);
  L_mid = L_add(L_mid, L_mid2);

  L_mid = L_shr(L_mid, 14);
  L_varOut = L_mac(L_mid, swHigh1, swHigh2);

  return (L_varOut);
}



