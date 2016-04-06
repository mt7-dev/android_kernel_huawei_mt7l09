/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : bgnscd.c
*      Purpose          : Background noise source charateristic detector (SCD)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "bgnscd.h"

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amr_comm.h"
#include "cnst.h"
#include "set_zero.h"
#include "gmed_n.h"
#include "sqrt_l.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/* Bgn_scdState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
Bgn_scdState g_stAmrBgnScdState;

/*-----------------------------------------------------------------*
 *   Decoder constant parameters (defined in "cnst.h")             *
 *-----------------------------------------------------------------*
 *   L_FRAME       : Frame size.                                   *
 *   L_SUBFR       : Sub-frame size.                               *
 *-----------------------------------------------------------------*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Bgn_scd_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
Word16 Bgn_scd_init (Bgn_scdState **state)
{
   Bgn_scdState* s;

   if (state == (Bgn_scdState **) AMR_NULL){
       return -1;
   }
   *state = AMR_NULL;

   /* allocate memory */
   s = &g_stAmrBgnScdState;

   Bgn_scd_reset(s);
   *state = s;

   return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
Word16 Bgn_scd_reset (Bgn_scdState *state)
{
   if (state == (Bgn_scdState *) AMR_NULL){
       return -1;
   }

   /* Static vectors to zero */
   Set_zero (state->frameEnergyHist, L_ENERGYHIST);

   /* Initialize hangover handling */
   state->bgHangover = 0;

   return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void Bgn_scd_exit (Bgn_scdState **state)
{
   if (state == AMR_NULL || *state == AMR_NULL)
      return;

   *state = AMR_NULL;

   return;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = no bgn, 1 = bgn
*
**************************************************************************
*/
Word16 Bgn_scd (Bgn_scdState *st,      /* i : State variables for bgn SCD */
                Word16 ltpGainHist[],  /* i : LTP gain history            */
                Word16 speech[],       /* o : synthesis speech frame      */
                Word16 *voicedHangover /* o : # of frames after last
                                              voiced frame                */
                )
{
   Word16 prevVoiced, inbgNoise;
   Word16 temp;
   Word16 ltpLimit, frameEnergyMin;
   Word16 currEnergy, noiseFloor, maxEnergy, maxEnergyLastPart;
   Word32 s;

   /* Update the inBackgroundNoise flag (valid for use in next frame if BFI) */
   /* it now works as a energy detector floating on top                      */
   /* not as good as a VAD.                                                  */

   currEnergy = 0;

   s = CODEC_OpVvMac( speech, speech, L_FRAME, 0 );

   s = L_shl(s, 2);

   currEnergy = extract_h (s);

   frameEnergyMin = CODEC_OpVecMin(&st->frameEnergyHist[0], L_ENERGYHIST, 0);

   noiseFloor = shl (frameEnergyMin, 4); /* Frame Energy Margin of 16 */

   maxEnergy = CODEC_OpVecMax(&st->frameEnergyHist[0],
                            L_ENERGYHIST-4,
                            AMR_NULL);

   maxEnergyLastPart = CODEC_OpVecMax(&st->frameEnergyHist[2*L_ENERGYHIST/3],
                                    L_ENERGYHIST-2*L_ENERGYHIST/3,
                                    AMR_NULL);

   inbgNoise = 0;        /* false */

   /* Do not consider silence as noise */
   /* Do not consider continuous high volume as noise */
   /* Or if the current noise level is very low */
   /* Mark as noise if under current noise limit */
   /* OR if the maximum energy is below the upper limit */

   /*if ( (sub(maxEnergy, LOWERNOISELIMIT) > 0) &&
        (sub(currEnergy, FRAMEENERGYLIMIT) < 0) &&
        (sub(currEnergy, LOWERNOISELIMIT) > 0) &&
        ( (sub(currEnergy, noiseFloor) < 0) ||
          (sub(maxEnergyLastPart, UPPERNOISELIMIT) < 0)))*/
   if (( maxEnergy  > (Word16)LOWERNOISELIMIT ) &&
       ( currEnergy < (Word16)FRAMEENERGYLIMIT ) &&
       ( currEnergy > (Word16)LOWERNOISELIMIT ) &&
       (( currEnergy < noiseFloor ) ||
       ( maxEnergyLastPart < (Word16)UPPERNOISELIMIT)))
   {
      /*if (sub(add(st->bgHangover, 1), 30) > 0)*/
      if ( add(st->bgHangover, 1) > (Word16)30 )
      {
         st->bgHangover = 30;
      } else
      {
         st->bgHangover = add(st->bgHangover, 1);
      }
   }
   else
   {
      st->bgHangover = 0;
   }

   /* make final decision about frame state , act somewhat cautiosly */

   /*if (sub(st->bgHangover,1) > 0)*/
   if ( st->bgHangover > (Word16)1 )
      inbgNoise = 1;       /* true  */

   CODEC_OpVecCpy(&st->frameEnergyHist[0],
                &st->frameEnergyHist[1],
                L_ENERGYHIST-1);

   st->frameEnergyHist[L_ENERGYHIST-1] = currEnergy;

   /* prepare for voicing decision; tighten the threshold after some
      time in noise */
   ltpLimit = 13926;             /* 0.85  Q14 */

   /*if (sub(st->bgHangover, 8) > 0)*/
   if ( st->bgHangover > (Word16)8 )
   {
      ltpLimit = 15565;          /* 0.95  Q14 */
   }

   /*if (sub(st->bgHangover, 15) > 0)*/
   if ( st->bgHangover > (Word16)15 )
   {
      ltpLimit = 16383;          /* 1.00  Q14 */
   }

   /* weak sort of voicing indication. */
   prevVoiced = 0;        /* false */

   /*if (sub(gmed_n(&ltpGainHist[4], 5), ltpLimit) > 0)*/
   if ( gmed_n(&ltpGainHist[4], 5) > ltpLimit )
   {
      prevVoiced = 1;     /* true  */
   }

   /*if (sub(st->bgHangover, 20) > 0) {*/
   if ( st->bgHangover > (Word16)20 ) {
      /*if (sub(gmed_n(ltpGainHist, 9), ltpLimit) > 0)*/
      if ( gmed_n(ltpGainHist, 9) > ltpLimit )
      {
         prevVoiced = 1;  /* true  */
      }
      else
      {
         prevVoiced = 0;  /* false  */
      }
   }

   if (prevVoiced)
   {
      *voicedHangover = 0;
   }
   else
   {
      temp = add(*voicedHangover, 1);

      /*if (sub(temp, 10) > 0)*/
      if ( temp > (Word16)10 )
      {
         *voicedHangover = 10;
      }
      else
      {
         *voicedHangover = temp;
      }
   }

   return inbgNoise;
}
