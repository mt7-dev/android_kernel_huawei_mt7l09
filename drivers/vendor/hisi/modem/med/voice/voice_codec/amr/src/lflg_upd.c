/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : lflg_upd.c
*      Purpose          : LTP_flag update for AMR VAD option 2
*
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "cnst.h"

#include "vad2.h"
#include "mode.h"

/***************************************************************************
 *
 *   FUNCTION NAME: LTP_flag_update
 *
 *   PURPOSE:
 *     Set LTP_flag if the LTP gain > LTP_THRESHOLD, where the value of
 *     LTP_THRESHOLD depends on the LTP analysis window length.
 *
 *   INPUTS:
 *
 *     mode
 *                     AMR mode
 *     vadState->L_R0
 *                     LTP energy
 *     vadState->L_Rmax
 *                     LTP maximum autocorrelation
 *   OUTPUTS:
 *
 *     vadState->LTP_flag
 *                     Set if LTP gain > LTP_THRESHOLD
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *************************************************************************/

void LTP_flag_update (vadState2 * st, Word16 mode)
{
	Word16 thresh;
	Word16 hi1;
	Word16 lo1;
	Word32 Ltmp;

	if ((MR475 == mode) || (MR515 == mode))
	{
		thresh = (Word16)(32768.0*0.55);
	}
	else if (MR102 == mode)
	{
		thresh = (Word16)(32768.0*0.60);
	}
	else
	{
		thresh = (Word16)(32768.0*0.65);
	}

	L_Extract (st->L_R0, &hi1, &lo1);
	Ltmp = Mpy_32_16(hi1, lo1, thresh);
	if ( st->L_Rmax > Ltmp )
	{
		st->LTP_flag = TRUE;
	}
	else
	{
		st->LTP_flag = FALSE;
	}

	return;
}
