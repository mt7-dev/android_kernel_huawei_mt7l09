/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : sp_enc.c
*      Purpose          : Pre filtering and encoding of one speech frame.
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"

#include "cnst.h"
#include "set_zero.h"
#include "pre_proc.h"
#include "prm2bits.h"
#include "mode.h"
#include "cod_amr.h"

#ifdef MMS_IO
#include "frame.h"
#include "bitno_tab.c"
#endif

#include "sp_enc.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/* Speech_Encode_FrameState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
Speech_Encode_FrameState g_stAmrEncFrameState;

/*---------------------------------------------------------------*
 *    Constants (defined in "cnst.h")                            *
 *---------------------------------------------------------------*
 * L_FRAME     :
 * M           :
 * PRM_SIZE    :
 * AZ_SIZE     :
 * SERIAL_SIZE :
 *---------------------------------------------------------------*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/

/*************************************************************************
*
*  Function:   Speech_Encode_Frame_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int Speech_Encode_Frame_init (Speech_Encode_FrameState **state,
                              Flag dtx,
                              char *id)
{
  Speech_Encode_FrameState* s;

  if (state == (Speech_Encode_FrameState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrEncFrameState;

  s->pre_state = AMR_NULL;
  s->cod_amr_state = AMR_NULL;
  s->dtx = dtx;

  if (Pre_Process_init(&s->pre_state) ||
      cod_amr_init(&s->cod_amr_state, s->dtx)) {
      Speech_Encode_Frame_exit(&s);
      return -1;
  }

  Speech_Encode_Frame_reset(s);
  *state = s;

  return 0;
}

/*************************************************************************
*
*  Function:   Speech_Encode_Frame_reset
*  Purpose:    Resetses state memory
*
**************************************************************************
*/
int Speech_Encode_Frame_reset (Speech_Encode_FrameState *state)
{
  if (state == (Speech_Encode_FrameState *) AMR_NULL){
      return -1;
  }

  Pre_Process_reset(state->pre_state);
  cod_amr_reset(state->cod_amr_state);

  return 0;
}

/*************************************************************************
*
*  Function:   Speech_Encode_Frame_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void Speech_Encode_Frame_exit (Speech_Encode_FrameState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  Pre_Process_exit(&(*state)->pre_state);
  cod_amr_exit(&(*state)->cod_amr_state);

  *state = AMR_NULL;

  return;
}

int Speech_Encode_Frame (
    Speech_Encode_FrameState *st, /* i/o : post filter states          */
    enum Mode mode,               /* i   : speech coder mode           */
    Word16 *new_speech,           /* i   : speech input                */
    Word16 *serial,               /* o   : serial bit stream           */
    enum Mode *usedMode,          /* o   : used speech coder mode      */
    enum TXFrameType *penTxFrmType /* o :Output frametype              */
    )
{
  Word16 prm[MAX_PRM_SIZE];   /* Analysis parameters.                  */
  Word16 syn[L_FRAME];        /* Buffer for synthesis speech           */

  /* initialize the serial output frame to zero */
  CODEC_OpVecSet(&serial[0], MAX_SERIAL_SIZE, 0);

#if !defined(NO13BIT)
  /* Delete the 3 LSBs (13-bit input) */
  CODEC_OpVcAnd(&new_speech[0], L_FRAME, (Word16)0xfff8, &new_speech[0]);
#endif

  /* filter + downscaling */
  Pre_Process (st->pre_state, new_speech, L_FRAME);

  /* Call the speech encoder */
  cod_amr(st->cod_amr_state, mode, new_speech, prm, usedMode, syn, penTxFrmType);

  /* Parameters to serial bits */
  Prm2bits (*usedMode, prm, &serial[0]);

  return 0;
}

#ifdef MMS_IO

/*************************************************************************
 *
 *  FUNCTION:    PackBits
 *
 *  PURPOSE:     Sorts speech bits according decreasing subjective importance
 *               and packs into octets according to AMR file storage format
 *               as specified in RFC 3267 (Sections 5.1 and 5.3).
 *
 *  DESCRIPTION: Depending on the mode, different numbers of bits are
 *               processed. Details can be found in specification mentioned
 *               above and in file "bitno.tab".
 *
 *************************************************************************/
Word16 PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    Word16 bits[],             /* i : serial bits                 */
    UWord8 packed_bits[]       /* o : sorted&packed bits          */
)
{
   Word16 i;
   UWord8 temp;
   UWord8 *pack_ptr;

   temp = 0;
   pack_ptr = (UWord8*)packed_bits;

   /* file storage format can handle only speech frames, AMR SID frames and NO_DATA frames */
   /* -> force NO_DATA frame */
   if (used_mode < 0 || used_mode > 15 || (used_mode > 8 && used_mode < 15))
   {
	   used_mode = 15;
   }

   /* mark empty frames between SID updates as NO_DATA frames */
   if (used_mode == MRDTX && fr_type == TX_NO_DATA)
   {
	   used_mode = 15;
   }

   /* insert table of contents (ToC) byte at the beginning of the frame */
   *pack_ptr = toc_byte[used_mode];
   pack_ptr++;

   /* note that NO_DATA frames (used_mode==15) do not need further processing */
   if (used_mode == 15)
   {
	   return 1;
   }

   temp = 0;

   /* sort and pack speech bits */
   for (i = 1; i < unpacked_size[used_mode] + 1; i++)
   {
       if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
	   {
		   temp++;
	   }

	   if (i % 8)
	   {
		   temp <<= 1;
	   }
	   else
	   {
		   *pack_ptr = temp;
		   pack_ptr++;
		   temp = 0;
	   }
   }

   /* insert SID type indication and speech mode in case of SID frame */
   if (used_mode == MRDTX)
   {
	   if (fr_type == TX_SID_UPDATE)
	   {
		   temp++;
	   }
	   temp <<= 3;

	   temp += ((mode & 0x4) >> 2) | (mode & 0x2) | ((mode & 0x1) << 2);

	   temp <<= 1;
   }

   /* insert unused bits (zeros) at the tail of the last byte */
   temp <<= (unused_size[used_mode] - 1);
   *pack_ptr = temp;

   return packed_size[used_mode];
}

#endif
