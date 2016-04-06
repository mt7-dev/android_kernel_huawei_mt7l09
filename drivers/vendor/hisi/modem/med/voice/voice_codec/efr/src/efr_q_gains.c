/*--------------------------------------------------------------------------*
 * Function q_gain_pitch(), q_gain_code()                                  *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                  *
 * Scalar quantization of the pitch gain and the innovative codebook gain.  *
 *                                                                          *
 * MA prediction is performed on the innovation energy                      *
 * (in dB/(20*log10(2))) with mean removed.                                 *
 *-------------------------------------------------------------------------*/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_gains_tb.h"
#include "efr_cnst.h"
#include "efr_dtx.h"

/* past quantized energies.                               */
/* initialized to -14.0/constant, constant = 20*Log10(2)   */
Word16 efr_enc_past_qua_en[4];

/* MA prediction coeff */
Word16 efr_enc_pred[4];

extern Word16 efr_CN_excitation_gain, efr_gain_code_old_tx[4 * EFR_DTX_HANGOVER];

Word16 EFR_q_gain_pitch (   /* Return index of quantization */
    Word16 *gain        /* (i)  :  Pitch gain to quantize  */
)
{
    Word16 i, index, gain_q14, err, err_min;

    gain_q14 = shl (*gain, 2);

    err_min = abs_s (sub (gain_q14, efr_qua_gain_pitch[0]));
    index = 0;

    for (i = 1; i < NB_QUA_PITCH; i++)
    {
        err = abs_s (sub (gain_q14, efr_qua_gain_pitch[i]));


        if (sub (err, err_min) < 0)
        {
            err_min = err;
            index = i;
        }
    }

    *gain = shr (efr_qua_gain_pitch[index], 2);

    return index;
}

/* average innovation energy.                             */
/* MEAN_ENER  = 36.0/constant, constant = 20*Log10(2)     */

#define MEAN_ENER  783741L      /* 36/(20*log10(2))       */

Word16 EFR_q_gain_code (    /* Return quantization index                  */
    Word16 code[],      /* (i)      : fixed codebook excitation       */
    Word16 lcode,       /* (i)      : codevector size                 */
    Word16 *gain,       /* (i/o)    : quantized fixed codebook gain   */
    Word16 txdtx_ctrl,
    Word16 i_subfr
)
{
    Word16 i, index=0;
    Word16 gcode0, err, err_min, exp, frac;
    Word32 ener, ener_code;
    Word16 aver_gain;
    static Word16 efr_gcode0_CN;


    if ((txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
    {

        /*-------------------------------------------------------------------*
         *  energy of code:                                                   *
         *  ~~~~~~~~~~~~~~~                                                   *
         *  ener_code(Q17) = 10 * Log10(energy/lcode) / constant              *
         *                 = 1/2 * Log2(energy/lcode)                         *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        /* ener_code = log10(ener_code/lcode) / (20*log10(2))       */
        ener_code = CODEC_OpVvMac(code, code, lcode, 0);

        /* ener_code = ener_code / lcode */
        ener_code = L_mult (round (ener_code), 26214);

        /* ener_code = 1/2 * Log2(ener_code) */
        EFR_Log2 (ener_code, &exp, &frac);
        ener_code = L_Comp (sub (exp, 30), frac);

        /* predicted energy */

        ener = MEAN_ENER;
        ener = CODEC_OpVvMac(efr_enc_past_qua_en, efr_enc_pred, 4, MEAN_ENER);
        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                           *
         *  gcode0(Qx) = Pow10( (ener*constant - ener_code*constant) / 20 )   *
         *             = Pow2(ener-ener_code)                                 *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        ener = L_shr (L_sub (ener, ener_code), 1);
        L_Extract (ener, &exp, &frac);

        gcode0 = extract_l (EFR_Pow2 (exp, frac));  /* predicted gain */

        gcode0 = shl (gcode0, 4);

        /*-------------------------------------------------------------------*
         *                   Search for best quantizer                        *
         *-------------------------------------------------------------------*/

        err_min = abs_s (sub (*gain, mult (gcode0, efr_qua_gain_code[0])));
        index = 0;

        for (i = 1; i < NB_QUA_CODE; i++)
        {
            err = abs_s (sub (*gain, mult (gcode0, efr_qua_gain_code[i])));


            if (sub (err, err_min) < 0)
            {
                err_min = err;
                index = i;
            }
        }

        *gain = mult (gcode0, efr_qua_gain_code[index]);


        /*------------------------------------------------------------------*
         *  update table of past quantized energies                         *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                         *
         *  past_qua_en(Q12) = 20 * Log10(qua_gain_code) / constant         *
         *                   = Log2(qua_gain_code)                          *
         *                                           constant = 20*Log10(2) *
         *------------------------------------------------------------------*/

        for (i = 3; i > 0; i--)
        {
            efr_enc_past_qua_en[i] = efr_enc_past_qua_en[i - 1];
        }
        EFR_Log2 (L_deposit_l (efr_qua_gain_code[index]), &exp, &frac);

        efr_enc_past_qua_en[0] = shr (frac, 5);
        efr_enc_past_qua_en[0] = add (efr_enc_past_qua_en[0], shl (sub (exp, 11), 10));


        EFR_update_gain_code_history_tx (*gain, efr_gain_code_old_tx);
    }
    else
    {

        if ((txdtx_ctrl & EFR_TX_PREV_HANGOVER_ACTIVE) != 0 && (i_subfr == 0))
        {
            efr_gcode0_CN = EFR_update_gcode0_CN (efr_gain_code_old_tx);
            efr_gcode0_CN = shl (efr_gcode0_CN, 4);
        }
        *gain = efr_CN_excitation_gain;


        if ((txdtx_ctrl & EFR_TX_SID_UPDATE) != 0)
        {
            aver_gain = EFR_aver_gain_code_history (efr_CN_excitation_gain,
                                                efr_gain_code_old_tx);

            /*---------------------------------------------------------------*
             *                   Search for best quantizer                    *
             *---------------------------------------------------------------*/

            err_min = abs_s (sub (aver_gain,
                                  mult (efr_gcode0_CN, efr_qua_gain_code[0])));
            index = 0;

            for (i = 1; i < NB_QUA_CODE; i++)
            {
                err = abs_s (sub (aver_gain,
                                  mult (efr_gcode0_CN, efr_qua_gain_code[i])));


                if (sub (err, err_min) < 0)
                {
                    err_min = err;
                    index = i;
                }
            }
        }
        EFR_update_gain_code_history_tx (*gain, efr_gain_code_old_tx);

        /*-------------------------------------------------------------------*
         *  reset table of past quantized energies                            *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                            *
         *-------------------------------------------------------------------*/

        CODEC_OpVecSet(efr_enc_past_qua_en, 4, -2381);
    }

    return index;
}
