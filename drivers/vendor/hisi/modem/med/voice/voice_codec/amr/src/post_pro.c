/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : post_pro.c
*      Purpose          : Postprocessing of output speech.
*
*                         - 2nd order high pass filtering with cut
*                           off frequency at 60 Hz.
*                         - Multiplication of output by two.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "post_pro.h"
#include "cnst.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/* Post_ProcessState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
Post_ProcessState g_stAmrPostProcessState;

/* filter coefficients (fc = 60 Hz) */
static const Word16 b[3] = {7699, -15398, 7699};
static const Word16 a[3] = {8192, 15836, -7667};

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Post_Process_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int Post_Process_init (Post_ProcessState **state)
{
  Post_ProcessState* s;

  if (state == (Post_ProcessState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrPostProcessState;

  Post_Process_reset(s);
  *state = s;

  return 0;
}

/*************************************************************************
*
*  Function:   Post_Process_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int Post_Process_reset (Post_ProcessState *state)
{
  if (state == (Post_ProcessState *) AMR_NULL){
      return -1;
  }

  state->y2_hi = 0;
  state->y2_lo = 0;
  state->y1_hi = 0;
  state->y1_lo = 0;
  state->x0 = 0;
  state->x1 = 0;

  return 0;
}

/*************************************************************************
*
*  Function:   Post_Process_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void Post_Process_exit (Post_ProcessState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*************************************************************************
 *
 *  FUNCTION:  Post_Process()
 *
 *  PURPOSE: Postprocessing of input speech.
 *
 *  DESCRIPTION:
 *     - 2nd order high pass filtering with cut off frequency at 60 Hz.
 *     - Multiplication of output by two.
 *
 * Algorithm:
 *
 *  y[i] = b[0]*x[i]*2 + b[1]*x[i-1]*2 + b[2]*x[i-2]*2
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];
 *
 *
 *************************************************************************/
int Post_Process (
    Post_ProcessState *st,  /* i/o : post process state                   */
    Word16 signal[],        /* i/o : signal                               */
    Word16 lg               /* i   : length of signal                     */
    )
{
    Word16 i, x2;
    Word32 L_tmp;

    for (i = 0; i < lg; i++)
    {
        x2 = st->x1;
        st->x1 = st->x0;
        st->x0 = signal[i];

        /*  y[i] = b[0]*x[i]*2 + b[1]*x[i-1]*2 + b140[2]*x[i-2]/2  */
        /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

        L_tmp = Mpy_32_16 (st->y1_hi, st->y1_lo, a[1]);
        L_tmp = L_add (L_tmp, Mpy_32_16 (st->y2_hi, st->y2_lo, a[2]));
        L_tmp = L_mac (L_tmp, st->x0, b[0]);
        L_tmp = L_mac (L_tmp, st->x1, b[1]);
        L_tmp = L_mac (L_tmp, x2, b[2]);
        L_tmp = L_shl (L_tmp, 2);

        /* Multiplication by two of output speech with saturation. */
        signal[i] = round(L_shl(L_tmp, 1));

        st->y2_hi = st->y1_hi;
        st->y2_lo = st->y1_lo;
        L_Extract (L_tmp, &st->y1_hi, &st->y1_lo);
    }

    return 0;
}
