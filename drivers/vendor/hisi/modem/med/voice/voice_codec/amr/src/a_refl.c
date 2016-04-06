/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : a_refl.c
*      Purpose          : Convert from direct form coefficients to
*                         reflection coefficients
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "a_refl.h"

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
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : A_Refl
*
**************************************************************************
*/
void A_Refl(
   Word16 a[],	      /* i   : Directform coefficients */
   Word16 refl[]      /* o   : Reflection coefficients */
)
{
   /* local variables */
   Word16 i,j;
   Word16 aState[M];
   Word16 bState[M];
   Word16 normShift;
   Word16 normProd;
   Word32 L_acc;
   Word16 scale;
   Word32 L_temp;
   Word16 temp;
   Word16 mult;

   /* initialize states */
   CODEC_OpVecCpy(&aState[0], &a[0], (Word32)M);

   /* backward Levinson recursion */
   for (i = M-1; i >= 0; i--)
   {
      /* if (sub(abs_s(aState[i]), 4096) >= 0) */
      if ( abs_s(aState[i]) >=  (Word16)4096 )
      {
          goto ExitRefl;
      }

      refl[i] = shl(aState[i], 3);

      L_temp = L_mult(refl[i], refl[i]);
      L_acc = L_sub(MAX_32, L_temp);

      normShift = norm_l(L_acc);
      scale = sub(15, normShift);

      L_acc = L_shl(L_acc, normShift);
      normProd = round(L_acc);

      mult = div_s(16384, normProd);

      for (j = 0; j < i; j++)
      {
         L_acc = L_deposit_h(aState[j]);
         L_acc = L_msu(L_acc, refl[i], aState[i-j-1]);

         temp = round(L_acc);
         L_temp = L_mult(mult, temp);
         L_temp = L_shr_r(L_temp, scale);

         if ( L_abs(L_temp) > (Word32)32767 )
         {
            goto ExitRefl;
         }

         bState[j] = extract_l(L_temp);
      }

      CODEC_OpVecCpy(&aState[0], &bState[0], (Word32)i);

   }
   return;

ExitRefl:
   CODEC_OpVecSet(&refl[0], M, 0);
}
