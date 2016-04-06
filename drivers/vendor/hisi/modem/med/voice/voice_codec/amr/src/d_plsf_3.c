/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : d_plsf_3.c
*      Purpose          : Decodes the LSP parameters using the received
*                         quantization indices. 1st order MA prediction and
*                         split by 3 vector quantization (split-VQ)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d_plsf.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "lsp_lsf.h"
#include "reorder.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "q_plsf_3_tab.c"    /* Codebooks of LSF prediction residual */

/* ALPHA    ->  0.9                                            */
/* ONE_ALPHA-> (1.0-ALPHA)                                     */

#define ALPHA     29491
#define ONE_ALPHA 3277

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:   D_plsf_3()
 *
 *  PURPOSE: Decodes the LSP parameters using the received quantization
 *           indices.1st order MA prediction and split by 3 vector
 *           quantization (split-VQ)
 *
 *************************************************************************/

void D_plsf_3(
    D_plsfState *st,   /* i/o: State struct                               */
    enum Mode mode,    /* i  : coder mode                                 */
    Word16 bfi,        /* i  : bad frame indicator (set to 1 if a         */
                       /*      bad frame is received)                     */
    Word16 * indice,   /* i  : quantization indices of 3 submatrices, Q0  */
    Word16 * lsp1_q    /* o  : quantized 1st LSP vector,              Q15 */
)
{
    Word16 i, index;
    Word16 *p_cb1, *p_cb2, *p_cb3, *p_dico, temp;
    Word16 lsf1_r[M];
    Word16 lsf1_q[M];

    if (bfi != 0)   /* if bad frame */
    {
        /* use the past LSFs slightly shifted towards their mean */
        CODEC_OpVvFormWithCoef(&st->past_lsf_q[0],
                             &mean_lsf[0],
                             M,
                             ALPHA,
                             ONE_ALPHA,
                             &lsf1_q[0]);

        /* estimate past quantized residual to be used in next frame */

    if ((enum Mode)MRDTX != mode) {
      for (i = 0; i < M; i++) {
            /* temp  = mean_lsf[i] +  past_r2_q[i] * PRED_FAC; */

            temp = add(mean_lsf[i], mult(st->past_r_q[i], pred_fac[i]));

            st->past_r_q[i] = sub(lsf1_q[i], temp);
      }
    } else {
      for (i = 0; i < M; i++) {
            /* temp  = mean_lsf[i] +  past_r2_q[i]; */

            temp = add(mean_lsf[i], st->past_r_q[i]);
            st->past_r_q[i] = sub(lsf1_q[i], temp);
      }
    }
    }
    else  /* if good LSFs received */
    {

       if ( (enum Mode)MR475 == mode || (enum Mode)MR515  == mode)
       {   /* MR475, MR515 */
          p_cb1 = dico1_lsf;
          p_cb2 = dico2_lsf;
          p_cb3 = mr515_3_lsf;
       }
       else if ((enum Mode)MR795 == mode)
       {   /* MR795 */

          p_cb1 = mr795_1_lsf;
          p_cb2 = dico2_lsf;
          p_cb3 = dico3_lsf;
       }
       else
       {   /* MR59, MR67, MR74, MR102, MRDTX */

          p_cb1 = dico1_lsf;
          p_cb2 = dico2_lsf;
          p_cb3 = dico3_lsf;
       }

       /* decode prediction residuals from 3 received indices */

        index = *indice++;
        p_dico = &p_cb1[add(index, add(index, index))];
        lsf1_r[0] = *p_dico++;
        lsf1_r[1] = *p_dico++;
        lsf1_r[2] = *p_dico++;

        index = *indice++;

        if (((enum Mode)MR475 == mode) || ((enum Mode)MR515 == mode))
        {   /* MR475, MR515 only using every second entry */
            index = shl(index,1);
        }

        p_dico = &p_cb2[add(index, add(index, index))];
        lsf1_r[3] = *p_dico++;
        lsf1_r[4] = *p_dico++;
        lsf1_r[5] = *p_dico++;

        index = *indice++;
        p_dico = &p_cb3[shl(index, 2)];
        lsf1_r[6] = *p_dico++;
        lsf1_r[7] = *p_dico++;
        lsf1_r[8] = *p_dico++;
        lsf1_r[9] = *p_dico++;

        /* Compute quantized LSFs and update the past quantized residual */

    if ((enum Mode)MRDTX != mode)
           for (i = 0; i < M; i++) {
              temp = add(mean_lsf[i], mult(st->past_r_q[i], pred_fac[i]));
              lsf1_q[i] = add(lsf1_r[i], temp);
              st->past_r_q[i] = lsf1_r[i];
           }
        else
           for (i = 0; i < M; i++) {
              temp = add(mean_lsf[i], st->past_r_q[i]);
              lsf1_q[i] = add(lsf1_r[i], temp);
              st->past_r_q[i] = lsf1_r[i];
           }
    }

    /* verification that LSFs has minimum distance of LSF_GAP Hz */

    Reorder_lsf(lsf1_q, LSF_GAP, M);

    CODEC_OpVecCpy(&st->past_lsf_q[0],
                 &lsf1_q[0],
                 M);

    /*  convert LSFs to the cosine domain */

    Lsf_lsp(lsf1_q, lsp1_q, M);

    return;
}

void Init_D_plsf_3(D_plsfState *st,  /* i/o: State struct                */
           Word16 index      /* i  : past_rq_init[] index [0, 7] */)
{
  CODEC_OpVecCpy(&st->past_r_q[0],
               &past_rq_init[index * M],
               M);
}
