/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : cbsearch.c
*      Purpose          : Inovative codebook search (find index and gain)
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "cbsearch.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "c2_9pf.h"
#include "c2_11pf.h"
#include "c3_14pf.h"
#include "c4_17pf.h"
#include "c8_31pf.h"
#include "c1035pf.h"
#include "mode.h"
#include "cnst.h"

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
int cbsearch(Word16 x[],    /* i : target vector, Q0                       */
             Word16 h[],    /* i : impulse response of weighted synthesis  */
                            /*     filter h[-L_subfr..-1] must be set to   */
                            /*     zero. Q12                               */
             Word16 T0,     /* i : Pitch lag                               */
             Word16 pitch_sharp, /* i : Last quantized pitch gain, Q14     */
             Word16 gain_pit,    /* i : Pitch gain, Q14                    */
             Word16 res2[], /* i : Long term prediction residual, Q0       */
             Word16 code[], /* o : Innovative codebook, Q13                */
             Word16 y[],    /* o : filtered fixed codebook excitation, Q12 */
             Word16 **anap, /* o : Signs of the pulses                     */
             enum Mode mode,/* i : coder mode                              */
             Word16 subNr)  /* i : subframe number                         */
             {
   Word16 index;
   Word16 pit_sharpTmp;

   /* For MR74, the pre and post CB pitch sharpening is included in the
    * codebook search routine, while for MR122 is it not.
    */

   if (( (enum Mode)MR475 == mode) || ((enum Mode)MR515 == mode))
   {   /* MR475, MR515 */
      *(*anap)++ = code_2i40_9bits(subNr, x, h, T0, pitch_sharp,
                                   code, y, &index);
      *(*anap)++ = index;    /* sign index */
   }
   else if ( (enum Mode)MR59 == mode )
   {   /* MR59 */

      *(*anap)++ = code_2i40_11bits(x, h, T0, pitch_sharp, code, y, &index);
      *(*anap)++ = index;    /* sign index */
   }
   else if ((enum Mode)MR67 == mode )
   {   /* MR67 */

      *(*anap)++ = code_3i40_14bits(x, h, T0, pitch_sharp, code, y, &index);
      *(*anap)++ = index;    /* sign index */
   }
   else if (((enum Mode)MR74 == mode ) || ((enum Mode)MR795 == mode))
   {   /* MR74, MR795 */

      *(*anap)++ = code_4i40_17bits(x, h, T0, pitch_sharp, code, y, &index);
      *(*anap)++ = index;    /* sign index */
   }
   else if ((enum Mode)MR102 == mode )
   {   /* MR102 */

      /*-------------------------------------------------------------*
       * - include pitch contribution into impulse resp. h1[]        *
       *-------------------------------------------------------------*/
      /* pit_sharpTmp = pit_sharp;                     */
      /* if (pit_sharpTmp > 1.0) pit_sharpTmp = 1.0;   */
      pit_sharpTmp = shl (pitch_sharp, 1);

      CODEC_OpVcMultQ15Add(&h[T0],
                         &h[0],
                         L_SUBFR - T0,
                         pit_sharpTmp,
                         &h[T0]);

      /*--------------------------------------------------------------*
       * - Innovative codebook search (find index and gain)           *
       *--------------------------------------------------------------*/
      code_8i40_31bits (x, res2, h, code, y, *anap);
      *anap += 7;                                              add(0,0);

      /*-------------------------------------------------------*
       * - Add the pitch contribution to code[].               *
       *-------------------------------------------------------*/
      CODEC_OpVcMultQ15Add(&code[T0],
                         &code[0],
                         L_SUBFR - T0,
                         pit_sharpTmp,
                         &code[T0]);

   }
   else
   {  /* MR122 */
      /*-------------------------------------------------------------*
       * - include pitch contribution into impulse resp. h1[]        *
       *-------------------------------------------------------------*/

      /* pit_sharpTmp = gain_pit;                      */
      /* if (pit_sharpTmp > 1.0) pit_sharpTmp = 1.0;   */
      pit_sharpTmp = shl (gain_pit, 1);

      CODEC_OpVcMultQ15Add(&h[T0],
                         &h[0],
                         L_SUBFR - T0,
                         pit_sharpTmp,
                         &h[T0]);

      /*--------------------------------------------------------------*
       * - Innovative codebook search (find index and gain)           *
       *--------------------------------------------------------------*/

      code_10i40_35bits (x, res2, h, code, y, *anap);
      *anap += 10;      	                                   add(0,0);

      /*-------------------------------------------------------*
       * - Add the pitch contribution to code[].               *
       *-------------------------------------------------------*/
      CODEC_OpVcMultQ15Add(&code[T0],
                         &code[0],
                         L_SUBFR - T0,
                         pit_sharpTmp,
                         &code[T0]);

   }

   return 0;
}
