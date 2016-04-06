/*************************************************************************
 *
 *  FUNCTION:   D_plsf_5()
 *
 *  PURPOSE: Decodes the 2 sets of LSP parameters in a frame using the
 *           received quantization indices.
 *
 *  DESCRIPTION:
 *           The two sets of LSFs are quantized using split by 5 matrix
 *           quantization (split-MQ) with 1st order MA prediction.
 *
 *   See "q_plsf_5.c" for more details about the quantization procedure
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"

#include "efr_q_plsf_5_tab.c"         /* Codebooks of LSF prediction residual */

#include "efr_cnst.h"
#include "efr_dtx.h"

/* EFR_M  ->order of linear prediction filter                      */
/* LSF_GAP  -> Minimum distance between LSF after quantization */
/*             50 Hz = 205                                     */
/* PRED_FAC -> Prediction factor = 0.65                        */
/* ALPHA    ->  0.9                                            */
/* ONE_ALPHA-> (1.0-ALPHA)                                     */

#define LSF_GAP   205
#define PRED_FAC  21299
#define ALPHA     31128
#define ONE_ALPHA 1639

/* Past quantized prediction error */

Word16 efr_dec_past_r2_q[EFR_M];

/* Past dequantized lsfs */

Word16 efr_past_lsf_q[EFR_M];

/* Reference LSF parameter vector (comfort noise) */

Word16 efr_lsf_p_CN[EFR_M];

/*  LSF memories for comfort noise interpolation */

Word16 efr_lsf_old_CN[EFR_M], efr_lsf_new_CN[EFR_M];

 /* LSF parameter buffer */

extern Word16 efr_lsf_old_rx[EFR_DTX_HANGOVER][EFR_M];

void EFR_D_plsf_5 (
    Word16 *indice,       /* input : quantization indices of 5 submatrices */
    Word16 *lsp1_q,       /* output: quantized 1st LSP vector              */
    Word16 *lsp2_q,       /* output: quantized 2nd LSP vector              */
    Word16 bfi,           /* input : bad frame indicator (set to 1 if a bad
                                     frame is received)                    */
    Word16 rxdtx_ctrl,    /* input : RX DTX control word                   */
    Word16 rx_dtx_state   /* input : state of the comfort noise insertion
                                     period                                */
)
{
    Word16 i;
    const Word16 *p_dico;
    Word16 temp, sign;
    Word16 lsf1_r[EFR_M], lsf2_r[EFR_M];
    Word16 lsf1_q[EFR_M], lsf2_q[EFR_M];

    /* Update comfort noise LSF quantizer memory */

    if ((rxdtx_ctrl & EFR_RX_UPD_SID_QUANT_MEM) != 0)
    {
        EFR_update_lsf_p_CN (efr_lsf_old_rx, efr_lsf_p_CN);
    }
    /* Handle cases of comfort noise LSF decoding in which past
    valid SID frames are repeated */




    if (((rxdtx_ctrl & EFR_RX_NO_TRANSMISSION) != 0)
        || ((rxdtx_ctrl & EFR_RX_INVALID_SID_FRAME) != 0)
        || ((rxdtx_ctrl & EFR_RX_LOST_SID_FRAME) != 0))
    {


        if ((rxdtx_ctrl & EFR_RX_NO_TRANSMISSION) != 0)
        {
            /* DTX active: no transmission. Interpolate LSF values in memory */
            EFR_interpolate_CN_lsf (efr_lsf_old_CN, efr_lsf_new_CN, lsf2_q, rx_dtx_state);
        }
        else
        {                       /* Invalid or lost SID frame: use LSFs
                                   from last good SID frame */
            CODEC_OpVecCpy(efr_lsf_old_CN, efr_lsf_new_CN, EFR_M);
            CODEC_OpVecCpy(lsf2_q, efr_lsf_new_CN, EFR_M);
            CODEC_OpVecSet(efr_dec_past_r2_q, EFR_M, 0);
        }

        CODEC_OpVecCpy(efr_past_lsf_q, lsf2_q, EFR_M);

        /*  convert LSFs to the cosine domain */
        EFR_Lsf_lsp (lsf2_q, lsp2_q, EFR_M);

        return;
    }


    if (bfi != 0)                               /* if bad frame */
    {
        /* use the past LSFs slightly shifted towards their mean */

        for (i = 0; i < EFR_M; i++)
        {
            /* lsfi_q[i] = ALPHA*past_lsf_q[i] + ONE_ALPHA*mean_lsf[i]; */

            lsf1_q[i] = add (mult (efr_past_lsf_q[i], ALPHA),
                             mult (mean_lsf[i], ONE_ALPHA));


            lsf2_q[i] = lsf1_q[i];
        }

        /* estimate past quantized residual to be used in next frame */

        for (i = 0; i < EFR_M; i++)
        {
            /* temp  = mean_lsf[i] +  past_r2_q[i] * PRED_FAC; */

            temp = add (mean_lsf[i], mult (efr_dec_past_r2_q[i], PRED_FAC));

            efr_dec_past_r2_q[i] = sub (lsf2_q[i], temp);

        }
    }
    else
        /* if good LSFs received */
    {
        /* decode prediction residuals from 5 received indices */

        p_dico = &dico1_lsf[shl (indice[0], 2)];
        lsf1_r[0] = *p_dico++;
        lsf1_r[1] = *p_dico++;
        lsf2_r[0] = *p_dico++;
        lsf2_r[1] = *p_dico++;

        p_dico = &dico2_lsf[shl (indice[1], 2)];
        lsf1_r[2] = *p_dico++;
        lsf1_r[3] = *p_dico++;
        lsf2_r[2] = *p_dico++;
        lsf2_r[3] = *p_dico++;

        sign = indice[2] & 1;
        i = shr (indice[2], 1);
        p_dico = &dico3_lsf[shl (i, 2)];


        if (sign == 0)
        {
            lsf1_r[4] = *p_dico++;
            lsf1_r[5] = *p_dico++;
            lsf2_r[4] = *p_dico++;
            lsf2_r[5] = *p_dico++;
        }
        else
        {
            lsf1_r[4] = negate (*p_dico++);
            lsf1_r[5] = negate (*p_dico++);
            lsf2_r[4] = negate (*p_dico++);
            lsf2_r[5] = negate (*p_dico++);
        }

        p_dico = &dico4_lsf[shl (indice[3], 2)];
        lsf1_r[6] = *p_dico++;
        lsf1_r[7] = *p_dico++;
        lsf2_r[6] = *p_dico++;
        lsf2_r[7] = *p_dico++;

        p_dico = &dico5_lsf[shl (indice[4], 2)];
        lsf1_r[8] = *p_dico++;
        lsf1_r[9] = *p_dico++;
        lsf2_r[8] = *p_dico++;
        lsf2_r[9] = *p_dico++;

        /* Compute quantized LSFs and update the past quantized residual */
        /* Use lsf_p_CN as predicted LSF vector in case of no speech
           activity */


        if ((rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
        {
            for (i = 0; i < EFR_M; i++)
            {
                temp = add (mean_lsf[i], mult (efr_dec_past_r2_q[i], PRED_FAC));
                lsf1_q[i] = add (lsf1_r[i], temp);

                lsf2_q[i] = add (lsf2_r[i], temp);

                efr_dec_past_r2_q[i] = lsf2_r[i];
            }
        }
        else
        {                       /* Valid SID frame */
            CODEC_OpVvAdd(lsf2_r, efr_lsf_p_CN, EFR_M, lsf2_q);
            CODEC_OpVecCpy(lsf1_q, lsf2_q, EFR_M);
            CODEC_OpVecSet(efr_dec_past_r2_q, EFR_M, 0);
        }
    }

    /* verification that LSFs have minimum distance of LSF_GAP Hz */

    EFR_Reorder_lsf (lsf1_q, LSF_GAP, EFR_M);
    EFR_Reorder_lsf (lsf2_q, LSF_GAP, EFR_M);


    if ((rxdtx_ctrl & EFR_RX_FIRST_SID_UPDATE) != 0)
    {
        CODEC_OpVecCpy(efr_lsf_new_CN, lsf2_q, EFR_M);
    }

    if ((rxdtx_ctrl & EFR_RX_CONT_SID_UPDATE) != 0)
    {
        CODEC_OpVecCpy(efr_lsf_old_CN, efr_lsf_new_CN, EFR_M);
        CODEC_OpVecCpy(efr_lsf_new_CN, lsf2_q, EFR_M);
    }

    if ((rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {
        /* Update lsf history with quantized LSFs
           when speech activity is present. If the current frame is
           a bad one, update with most recent good comfort noise LSFs */

        if (bfi==0)
        {
            EFR_update_lsf_history (lsf1_q, lsf2_q, efr_lsf_old_rx);
        }
        else
        {
            EFR_update_lsf_history (efr_lsf_new_CN, efr_lsf_new_CN, efr_lsf_old_rx);
        }

        CODEC_OpVecCpy(efr_lsf_old_CN, lsf2_q, EFR_M);
    }
    else
    {
        EFR_interpolate_CN_lsf (efr_lsf_old_CN, efr_lsf_new_CN, lsf2_q, rx_dtx_state);
    }

    CODEC_OpVecCpy(efr_past_lsf_q, lsf2_q, EFR_M);
    /*  convert LSFs to the cosine domain */

    EFR_Lsf_lsp (lsf1_q, lsp1_q, EFR_M);
    EFR_Lsf_lsp (lsf2_q, lsp2_q, EFR_M);

    return;
}
