/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : d_plsf.c
*      Purpose          : common part (init, exit, reset) of LSF decoder
*                         module (rest in d_plsf_3.c and d_plsf_5.c)
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "d_plsf.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"
#include "q_plsf_5_tab.c"

/*
*--------------------------------------------------*
* Constants (defined in cnst.h)                    *
*--------------------------------------------------*
*  M                    : LPC order
*--------------------------------------------------*
*/

/* D_plsfState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
D_plsfState g_stAmrDPlsfState;

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : D_plsf_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
int D_plsf_init (D_plsfState **state)
{
  D_plsfState* s;

  if (state == (D_plsfState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrDPlsfState;

  D_plsf_reset(s);
  *state = s;

  return 0;
}

/*
**************************************************************************
*
*  Function    : D_plsf_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int D_plsf_reset (D_plsfState *state)
{
  if (state == (D_plsfState *) AMR_NULL){
      return -1;
  }

  /* Past quantized prediction error */
  CODEC_OpVecSet(&(state->past_r_q[0]), M, 0);

  /* Past dequantized lsfs */
  CODEC_OpVecCpy(&(state->past_lsf_q[0]),
               &mean_lsf[0],
               M);

  return 0;
}

/*
**************************************************************************
*
*  Function    : D_plsf_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void D_plsf_exit (D_plsfState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}
