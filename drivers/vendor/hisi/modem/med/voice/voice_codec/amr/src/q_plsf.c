/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : q_plsf.c
*      Purpose          : common part (init, exit, reset) of LSF quantization
*                         module (rest in q_plsf_3.c and q_plsf_5.c)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_plsf.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"

/* Q_plsfState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
Q_plsfState g_stAmrQPlsfState;

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*
**************************************************************************
*
*  Function    : Q_plsf_init
*  Purpose     : Allocates memory and initializes state variables
*
**************************************************************************
*/
int Q_plsf_init (Q_plsfState **state)
{
  Q_plsfState* s;

  if (state == (Q_plsfState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrQPlsfState;

  Q_plsf_reset(s);
  *state = s;

  return 0;
}

/*
**************************************************************************
*
*  Function    : Q_plsf_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int Q_plsf_reset (Q_plsfState *state)
{

  if (state == (Q_plsfState *) AMR_NULL){
      return -1;
  }

  CODEC_OpVecSet(&state->past_rq[0], M, 0);

  return 0;
}

/*
**************************************************************************
*
*  Function    : Q_plsf_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void Q_plsf_exit (Q_plsfState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}
