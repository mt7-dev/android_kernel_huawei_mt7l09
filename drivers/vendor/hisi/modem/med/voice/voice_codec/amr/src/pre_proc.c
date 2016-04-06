/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : pre_proc.c
*      Purpose          : Preprocessing of input speech.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pre_proc.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/* Pre_ProcessState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
Pre_ProcessState g_stAmrPreProcessState;

/* filter coefficients (fc = 80 Hz, coeff. b[] is divided by 2) */
static const Word16 b[3] = {1899, -3798, 1899};
static const Word16 a[3] = {4096, 7807, -3733};

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Pre_Process_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int Pre_Process_init (Pre_ProcessState **state)
{
  Pre_ProcessState* s;

  if (state == (Pre_ProcessState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrPreProcessState;

  Pre_Process_reset(s);
  *state = s;

  return 0;
}

/*************************************************************************
*
*  Function:   Pre_Process_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int Pre_Process_reset (Pre_ProcessState *state)
{
  if (state == (Pre_ProcessState *) AMR_NULL){
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
*  Function:   Pre_Process_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void Pre_Process_exit (Pre_ProcessState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*************************************************************************
 *
 *  FUNCTION:  Pre_Process()
 *
 *  PURPOSE: Preprocessing of input speech.
 *
 *  DESCRIPTION:
 *     - 2nd order high pass filtering with cut off frequency at 80 Hz.
 *     - Divide input by two.
 *
 *
 * Algorithm:
 *
 *  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b[2]*x[i-2]/2
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];
 *
 *
 *  Input is divided by two in the filtering process.
 *
 *************************************************************************/
int Pre_Process (
    Pre_ProcessState *st,
    Word16 signal[], /* input/output signal */
    Word16 lg)       /* lenght of signal    */
{
    Word16 i, x2;
    Word32 L_tmp;

    for (i = 0; i < lg; i++)
    {
        x2 = st->x1;
        st->x1 = st->x0;
        st->x0 = signal[i];

        /*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b140[2]*x[i-2]/2  */
        /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

        L_tmp = Mpy_32_16 (st->y1_hi, st->y1_lo, a[1]);
        L_tmp = L_add (L_tmp, Mpy_32_16 (st->y2_hi, st->y2_lo, a[2]));
        L_tmp = L_mac (L_tmp, st->x0, b[0]);
        L_tmp = L_mac (L_tmp, st->x1, b[1]);
        L_tmp = L_mac (L_tmp, x2, b[2]);
        L_tmp = L_shl (L_tmp, 3);
        signal[i] = round (L_tmp);

        st->y2_hi = st->y1_hi;
        st->y2_lo = st->y1_lo;
        L_Extract (L_tmp, &st->y1_hi, &st->y1_lo);
    }
    return 0;
}
