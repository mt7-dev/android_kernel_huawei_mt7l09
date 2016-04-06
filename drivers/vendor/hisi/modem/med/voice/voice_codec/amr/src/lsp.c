/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : lsp.c
*      Purpose          : From A(z) to lsp. LSP quantization and interpolation
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lsp.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "q_plsf.h"
#include "az_lsp.h"
#include "int_lpc.h"
#include "cnst.h"
#include "lsp_tab.c"

/* lspState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
lspState g_stAmrLspState;

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : lsp_init
*
**************************************************************************
*/
int lsp_init (lspState **st)
{
  lspState* s;

  if (st == (lspState **) AMR_NULL){
      return -1;
  }

  *st = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrLspState;

  /* Initialize quantization state */
   Q_plsf_init(&s->qSt);

  lsp_reset(s);
  *st = s;

  return 0;
}

/*
**************************************************************************
*
*  Function    : lsp_reset
*
**************************************************************************
*/
int lsp_reset (lspState *st)
{

  if (st == (lspState *) AMR_NULL){
      return -1;
  }

  /* Init lsp_old[] */
  CODEC_OpVecCpy(&st->lsp_old[0], lsp_init_data, M);

  /* Initialize lsp_old_q[] */
  CODEC_OpVecCpy(&st->lsp_old_q[0], st->lsp_old, M);

  /* Reset quantization state */
   Q_plsf_reset(st->qSt);

  return 0;
}

/*
**************************************************************************
*
*  Function    : lsp_exit
*
**************************************************************************
*/
void lsp_exit (lspState **st)
{
  if (st == AMR_NULL || *st == AMR_NULL)
      return;

  /* Deallocate members */
  Q_plsf_exit(&(*st)->qSt);

  *st = AMR_NULL;

  return;
}

/*************************************************************************
 *
 *   FUNCTION:  lsp()
 *
 ************************************************************************/
int lsp(lspState *st,        /* i/o : State struct                            */
        enum Mode req_mode,  /* i   : requested coder mode                    */
        enum Mode used_mode, /* i   : used coder mode                         */
        Word16 az[],         /* i/o : interpolated LP parameters Q12          */
        Word16 azQ[],        /* o   : quantization interpol. LP parameters Q12*/
        Word16 lsp_new[],    /* o   : new lsp vector                          */
        Word16 **anap        /* o   : analysis parameters                     */)
{
   Word16 lsp_new_q[M];    /* LSPs at 4th subframe           */
   Word16 lsp_mid[M], lsp_mid_q[M];    /* LSPs at 2nd subframe           */

   Word16 pred_init_i; /* init index for MA prediction in DTX mode */

   if ( MR122 == req_mode)
   {
       Az_lsp (&az[MP1], lsp_mid, st->lsp_old);
       Az_lsp (&az[MP1 * 3], lsp_new, lsp_mid);

       /*--------------------------------------------------------------------*
        * Find interpolated LPC parameters in all subframes (both quantized  *
        * and unquantized).                                                  *
        * The interpolated parameters are in array A_t[] of size (M+1)*4     *
        * and the quantized interpolated parameters are in array Aq_t[]      *
        *--------------------------------------------------------------------*/
       Int_lpc_1and3_2 (st->lsp_old, lsp_mid, lsp_new, az);

       if ( used_mode != MRDTX )
       {
          /* LSP quantization (lsp_mid[] and lsp_new[] jointly quantized) */
          Q_plsf_5 (st->qSt, lsp_mid, lsp_new, lsp_mid_q, lsp_new_q, *anap);

          Int_lpc_1and3 (st->lsp_old_q, lsp_mid_q, lsp_new_q, azQ);

          /* Advance analysis parameters pointer */
          (*anap) += add(0,5);
       }
   }
   else
   {
       Az_lsp(&az[MP1 * 3], lsp_new, st->lsp_old);  /* From A(z) to lsp  */

       /*--------------------------------------------------------------------*
        * Find interpolated LPC parameters in all subframes (both quantized  *
        * and unquantized).                                                  *
        * The interpolated parameters are in array A_t[] of size (M+1)*4     *
        * and the quantized interpolated parameters are in array Aq_t[]      *
        *--------------------------------------------------------------------*/

       Int_lpc_1to3_2(st->lsp_old, lsp_new, az);

       if ( used_mode != MRDTX )
       {
          /* LSP quantization */
          Q_plsf_3(st->qSt, req_mode, lsp_new, lsp_new_q, *anap, &pred_init_i);

          Int_lpc_1to3(st->lsp_old_q, lsp_new_q, azQ);

          /* Advance analysis parameters pointer */
          (*anap) += add (0, 3);
       }
   }

   /* update the LSPs for the next frame */
   CODEC_OpVecCpy(st->lsp_old, lsp_new, M);
   CODEC_OpVecCpy(st->lsp_old_q, lsp_new_q, M);

   return 0;
}

