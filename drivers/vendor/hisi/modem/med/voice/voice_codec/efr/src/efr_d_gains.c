/*************************************************************************
 *
 *  FILE NAME:   D_GAINS.C
 *
 *  FUNCTIONS DEFINED IN THIS FILE:
 *
 *        d_gain_pitch(), d_gain_code() and gmed5()
 *
 * MA prediction is performed on the innovation energy
 * ( in dB/(20*log10(2)) ) with mean removed.
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_gains_tb.h"
#include "efr_cnst.h"
#include "efr_dtx.h"

extern Word16 efr_gain_code_old_rx[4 * EFR_DTX_HANGOVER];

/* Static variables for bad frame handling */

/* Variables used by d_gain_pitch: */
Word16 efr_pbuf[5], efr_past_gain_pit, efr_prev_gp;

/* Variables used by d_gain_code: */
Word16 efr_gbuf[5], efr_past_gain_code, efr_prev_gc;

/* Static variables for CNI (used by d_gain_code) */

Word16 efr_gcode0_CN, efr_gain_code_old_CN, efr_gain_code_new_CN, efr_gain_code_muting_CN;

/* Memories of gain dequantization: */

/* past quantized energies.      */
/* initialized to -14.0/constant, constant = 20*Log10(2) */

Word16 efr_dec_past_qua_en[4];

/* MA prediction coeff   */
Word16 efr_dec_pred[4];

/*************************************************************************
 *
 *  FUNCTION:   gmed5
 *
 *  PURPOSE:    calculates 5-point median.
 *
 *  DESCRIPTION:
 *
 *************************************************************************/

Word16 EFR_gmed5 (        /* out      : index of the median value (0...4) */
    Word16 ind[]      /* in       : Past gain values                  */
)
{
    Word16 i, j, ix = 0, tmp[5];
    Word16 max, tmp2[5];

    CODEC_OpVecCpy(tmp2, ind, 5);

    for (i = 0; i < 5; i++)
    {

        max = -8192;

        for (j = 0; j < 5; j++)
        {
            if (sub (tmp2[j], max) >= 0)
            {
                max = tmp2[j];
                ix = j;
            }
        }

        tmp2[ix] = -16384;
        tmp[i] = ix;
    }

    return (ind[tmp[2]]);
}

/*************************************************************************
 *
 *  FUNCTION:   d_gain_pitch
 *
 *  PURPOSE:  decodes the pitch gain using the received index.
 *
 *  DESCRIPTION:
 *       In case of no frame erasure, the gain is obtained from the
 *       quantization table at the given index; otherwise, a downscaled
 *       past gain is used.
 *
 *************************************************************************/

Word16 EFR_d_gain_pitch ( /* out      : quantized pitch gain           */
    Word16 index,     /* in       : index of quantization          */
    Word16 bfi,       /* in       : bad frame indicator (good = 0) */
    Word16 state,
    Word16 prev_bf,
    Word16 rxdtx_ctrl
)
{
    static const Word16 pdown[7] =
    {
        32767, 32112, 32112, 26214,
        9830, 6553, 6553
    };

    Word16 gain, tmp;


    if (bfi == 0)
    {

        if ((rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
        {
            gain = shr (efr_qua_gain_pitch[index], 2);


            if (prev_bf != 0)
            {

                if (sub (gain, efr_prev_gp) > 0)
                {
                    gain = efr_prev_gp;
                }
            }
        }
        else
        {
            gain = 0;
        }
        efr_prev_gp = gain;
    }
    else
    {

        if ((rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
        {
            tmp = EFR_gmed5 (efr_pbuf);


            if (sub (tmp, efr_past_gain_pit) < 0)
            {
                efr_past_gain_pit = tmp;
            }
            gain = mult (pdown[state], efr_past_gain_pit);
        }
        else
        {
            gain = 0;
        }
    }

    efr_past_gain_pit = gain;


    if (sub (efr_past_gain_pit, 4096) > 0)  /* if (past_gain_pit > 1.0) */
    {
        efr_past_gain_pit = 4096;
    }

    CODEC_OpVecCpy(efr_pbuf, efr_pbuf + 1, 4);
    efr_pbuf[4] = efr_past_gain_pit;

    return gain;
}

/*************************************************************************
 *
 *  FUNCTION:  d_gain_code
 *
 *  PURPOSE:  decode the fixed codebook gain using the received index.
 *
 *  DESCRIPTION:
 *      The received index gives the gain correction factor gamma.
 *      The quantized gain is given by   g_q = g0 * gamma
 *      where g0 is the predicted gain.
 *      To find g0, 4th order MA prediction is applied to the mean-removed
 *      innovation energy in dB.
 *      In case of frame erasure, downscaled past gain is used.
 *
 *************************************************************************/

/* average innovation energy.                             */
/* MEAN_ENER = 36.0/constant, constant = 20*Log10(2)      */
#define MEAN_ENER  783741L      /* 36/(20*log10(2))       */

void EFR_d_gain_code (
    Word16 index,      /* input : received quantization index */
    Word16 code[],     /* input : innovation codevector       */
    Word16 lcode,      /* input : codevector length           */
    Word16 *gain_code, /* output: decoded innovation gain     */
    Word16 bfi,        /* input : bad frame indicator         */
    Word16 state,
    Word16 prev_bf,
    Word16 rxdtx_ctrl,
    Word16 i_subfr,
    Word16 rx_dtx_state
)
{
    static const Word16 cdown[7] =
    {
        32767, 32112, 32112, 32112,
        32112, 32112, 22937
    };

    Word16 i, tmp;
    Word16 gcode0, exp, frac, av_pred_en;
    Word32 ener, ener_code;


    if (((rxdtx_ctrl & EFR_RX_UPD_SID_QUANT_MEM) != 0) && (i_subfr == 0))
    {
        efr_gcode0_CN = EFR_update_gcode0_CN (efr_gain_code_old_rx);
        efr_gcode0_CN = shl (efr_gcode0_CN, 4);
    }

    /* Handle cases of comfort noise fixed codebook gain decoding in
       which past valid SID frames are repeated */


    if (((rxdtx_ctrl & EFR_RX_NO_TRANSMISSION) != 0)
        || ((rxdtx_ctrl & EFR_RX_INVALID_SID_FRAME) != 0)
        || ((rxdtx_ctrl & EFR_RX_LOST_SID_FRAME) != 0))
    {


        if ((rxdtx_ctrl & EFR_RX_NO_TRANSMISSION) != 0)
        {
            /* DTX active: no transmission. Interpolate gain values
            in memory */

            if (i_subfr == 0)
            {
                *gain_code = EFR_interpolate_CN_param (efr_gain_code_old_CN,
                                            efr_gain_code_new_CN, rx_dtx_state);

            }
            else
            {
                *gain_code = efr_prev_gc;
            }
        }
        else
        {                       /* Invalid or lost SID frame:
            use gain values from last good SID frame */
            efr_gain_code_old_CN = efr_gain_code_new_CN;
            *gain_code = efr_gain_code_new_CN;

            /* reset table of past quantized energies */
            CODEC_OpVecSet(efr_dec_past_qua_en, 4, -2381);
        }


        if ((rxdtx_ctrl & EFR_RX_DTX_MUTING) != 0)
        {
            /* attenuate the gain value by 0.75 dB in each subframe */
            /* (total of 3 dB per frame) */
            efr_gain_code_muting_CN = mult (efr_gain_code_muting_CN, 30057);
            *gain_code = efr_gain_code_muting_CN;
        }
        else
        {
            /* Prepare for DTX muting by storing last good gain value */
            efr_gain_code_muting_CN = efr_gain_code_new_CN;
        }

        efr_past_gain_code = *gain_code;

        CODEC_OpVecCpy(efr_gbuf, efr_gbuf + 1, 4);

        efr_gbuf[4] = efr_past_gain_code;
        efr_prev_gc = efr_past_gain_code;

        return;
    }

    /*----------------- Test erasure ---------------*/


    if (bfi != 0)
    {
        tmp = EFR_gmed5 (efr_gbuf);

        if (sub (tmp, efr_past_gain_code) < 0)
        {
            efr_past_gain_code = tmp;
        }
        efr_past_gain_code = mult (efr_past_gain_code, cdown[state]);
        *gain_code = efr_past_gain_code;

        av_pred_en = CODEC_OpVecSum(efr_dec_past_qua_en,4);

        /* av_pred_en = 0.25*av_pred_en - 4/(20Log10(2)) */
        av_pred_en = mult (av_pred_en, 8192);   /*  *= 0.25  */

        /* if (av_pred_en < -14/(20Log10(2))) av_pred_en = .. */

        if (sub (av_pred_en, -2381) < 0)
        {
            av_pred_en = -2381;
        }
        for (i = 3; i > 0; i--)
        {
            efr_dec_past_qua_en[i] = efr_dec_past_qua_en[i - 1];
        }
        efr_dec_past_qua_en[0] = av_pred_en;

        CODEC_OpVecCpy(efr_gbuf, efr_gbuf + 1, 4);

        efr_gbuf[4] = efr_past_gain_code;

        /* Use the most recent comfort noise fixed codebook gain value
           for updating the fixed codebook gain history */

        if (efr_gain_code_new_CN == 0)
        {
            tmp = efr_prev_gc;
        }
        else
        {
            tmp = efr_gain_code_new_CN;
        }

        EFR_update_gain_code_history_rx (tmp, efr_gain_code_old_rx);


        if (sub (i_subfr, (3 * EFR_L_SUBFR)) == 0)
        {
            efr_gain_code_old_CN = *gain_code;
        }
        return;
    }


    if ((rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {

        /*-------------- Decode codebook gain ---------------*/

        /*-------------------------------------------------------------------*
         *  energy of code:                                                   *
         *  ~~~~~~~~~~~~~~~                                                   *
         *  ener_code = 10 * Log10(energy/lcode) / constant                   *
         *            = 1/2 * Log2(energy/lcode)                              *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        /* ener_code = log10(ener_code/lcode) / (20*log10(2)) */
        ener_code = CODEC_OpVvMac(code, code, lcode, 0);

        /* ener_code = ener_code / lcode */
        ener_code = L_mult (round (ener_code), 26214);

        /* ener_code = 1/2 * Log2(ener_code) */
        EFR_Log2 (ener_code, &exp, &frac);
        ener_code = L_Comp (sub (exp, 30), frac);

        /* predicted energy */

        ener = CODEC_OpVvMac(efr_dec_past_qua_en, efr_dec_pred, 4, MEAN_ENER);
        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                           *
         *  gcode0     = Pow10( (ener*constant - ener_code*constant) / 20 )   *
         *             = Pow2(ener-ener_code)                                 *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        ener = L_shr (L_sub (ener, ener_code), 1);
        L_Extract (ener, &exp, &frac);

        gcode0 = extract_l (EFR_Pow2 (exp, frac));  /* predicted gain */

        gcode0 = shl (gcode0, 4);

        *gain_code = mult (efr_qua_gain_code[index], gcode0);


        if (prev_bf != 0)
        {

            if (sub (*gain_code, efr_prev_gc) > 0)
            {
                *gain_code = efr_prev_gc;
            }
        }
        /*-------------------------------------------------------------------*
         *  update table of past quantized energies                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           *
         *  past_qua_en      = 20 * Log10(qua_gain_code) / constant           *
         *                   = Log2(qua_gain_code)                            *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        for (i = 3; i > 0; i--)
        {
            efr_dec_past_qua_en[i] = efr_dec_past_qua_en[i - 1];
        }
        EFR_Log2 (L_deposit_l (efr_qua_gain_code[index]), &exp, &frac);

        efr_dec_past_qua_en[0] = shr (frac, 5);
        efr_dec_past_qua_en[0] = add (efr_dec_past_qua_en[0], shl (sub (exp, 11), 10));


        EFR_update_gain_code_history_rx (*gain_code, efr_gain_code_old_rx);

        if (sub (i_subfr, (3 * EFR_L_SUBFR)) == 0)
        {
            efr_gain_code_old_CN = *gain_code;
        }
    }
    else
    {

        if (((rxdtx_ctrl & EFR_RX_FIRST_SID_UPDATE) != 0) && (i_subfr == 0))
        {
            efr_gain_code_new_CN = mult (efr_gcode0_CN, efr_qua_gain_code[index]);

            /*---------------------------------------------------------------*
             *  reset table of past quantized energies                        *
             *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
             *---------------------------------------------------------------*/

            CODEC_OpVecSet(efr_dec_past_qua_en, 4, -2381);
        }

        if (((rxdtx_ctrl & EFR_RX_CONT_SID_UPDATE) != 0) && (i_subfr == 0))
        {
            efr_gain_code_old_CN = efr_gain_code_new_CN;
            efr_gain_code_new_CN = mult (efr_gcode0_CN, efr_qua_gain_code[index]);

        }

        if (i_subfr == 0)
        {
            *gain_code = EFR_interpolate_CN_param (efr_gain_code_old_CN,
                                               efr_gain_code_new_CN,
                                               rx_dtx_state);
        }
        else
        {
            *gain_code = efr_prev_gc;
        }
    }

    efr_past_gain_code = *gain_code;
    CODEC_OpVecCpy(efr_gbuf, efr_gbuf + 1, 4);
    efr_gbuf[4] = efr_past_gain_code;
    efr_prev_gc = efr_past_gain_code;

    return;
}
