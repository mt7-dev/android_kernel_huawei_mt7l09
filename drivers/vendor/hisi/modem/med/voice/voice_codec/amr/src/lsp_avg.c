/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : lsp_avg.c
*      Purpose:         : LSP averaging and history
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "lsp_avg.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_vec.h"
#include "q_plsf_5_tab.c"
#include "cnst.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/* lsp_avgState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
lsp_avgState g_stAmrLspAvgState;
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : lsp_avg_init
*  Purpose     : Allocates memory and initializes state variables
*
**************************************************************************
*/
int lsp_avg_init (lsp_avgState **state)
{
  lsp_avgState* s;

  if (state == (lsp_avgState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrLspAvgState;

  lsp_avg_reset(s);
  *state = s;

  return 0;
}

/*
**************************************************************************
*
*  Function    : lsp_avg_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int lsp_avg_reset (lsp_avgState *st)
{
  if (st == (lsp_avgState *) AMR_NULL){
      return -1;
  }

  CODEC_OpVecCpy(&st->lsp_meanSave[0], mean_lsf, M);

  return 0;
}

/*
**************************************************************************
*
*  Function    : lsp_avg_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void lsp_avg_exit (lsp_avgState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*
**************************************************************************
*
*  Function    : lsp_avg
*  Purpose     : Calculate the LSP averages
*
**************************************************************************
*/

void lsp_avg (
    lsp_avgState *st,         /* i/o : State struct                 Q15 */
    Word16 *lsp               /* i   : state of the state machine   Q15 */
)
{
    Word16 i;
    Word32 L_tmp;            /* Q31 */

    for (i = 0; i < M; i++) {

       /* mean = 0.84*mean */
       L_tmp = L_deposit_h(st->lsp_meanSave[i]);
       L_tmp = L_msu(L_tmp, EXPCONST, st->lsp_meanSave[i]);

       /* Add 0.16 of newest LSPs to mean */
       L_tmp = L_mac(L_tmp, EXPCONST, lsp[i]);

       /* Save means */
       st->lsp_meanSave[i] = round(L_tmp);             /* Q15 */
    }

    return;
}
