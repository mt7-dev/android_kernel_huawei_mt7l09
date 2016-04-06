/**************************************************************************
 *
 *   File Name:  e_homing.c
 *
 *   Purpose:
 *      This file contains the following functions:
 *
 *      encoder_homing_frame_test()  checks if a frame of input samples
 *                                   matches the Encoder Homing Frame pattern.
 *
 *      encoder_reset()              called by reset_enc() to reset all
 *                                   the state variables for the encoder.
 *
 *      reset_enc()                  calls functions to reset the state
 *                                   variables for the encoder and VAD, and
 *                                   for the transmit DTX and Comfort Noise.
 *
 **************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"
#include "efr_vad.h"
#include "efr_dtx.h"
#include "efr_codec.h"
#include "efr_sig_proc.h"
#include "efr_e_homing.h"

/***************************************************************************
 *
 *   FUNCTION NAME:  encoder_homing_frame_test
 *
 *   PURPOSE:
 *      Checks if a frame of input samples matches the Encoder Homing Frame
 *      pattern, which is 0x0008 for all 160 samples in the frame.
 *
 *   INPUT:
 *      input_frame[]    one frame of speech samples
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      0       input frame does not match the Encoder Homing Frame pattern.
 *      1       input frame matches the Encoder Homing Frame pattern.
 *
 **************************************************************************/

Word16 EFR_encoder_homing_frame_test (Word16 input_frame[])
{
    Word16 i, j;

    for (i = 0; i < EFR_L_FRAME; i++)
    {
        j = input_frame[i] ^ EFR_EHF_MASK;

        if (j)
            break;
    }

    return !j;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  encoder_reset
 *
 *   PURPOSE:
 *      resets all of the state variables for the encoder
 *
 *   INPUT:
 *      None
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      None
 *
 **************************************************************************/

void EFR_encoder_reset (void)
{
    /* External declarations for encoder variables which need to be reset */

    /* Variables defined in levinson.c */
    /* ------------------------------- */
    extern Word16 efr_old_A[EFR_M + 1]; /* Last A(z) for case of unstable filter */

    /* Variables defined in q_gains.c */
    /* ------------------------------- */
    /* Memories of gain quantization: */
    extern Word16 efr_enc_past_qua_en[4], efr_enc_pred[4];

    /* Variables defined in q_plsf_5.c */
    /* ------------------------------- */
    /* Past quantized prediction error */
    extern Word16 efr_enc_past_r2_q[EFR_M];

    /* reset all the encoder state variables */
    /* ------------------------------------- */

    /* Variables in cod_12k2.c: */
    EFR_Init_Coder_12k2 ();

    /* Variables in levinson.c: */
    efr_old_A[0] = 4096;            /* Last A(z) for case of unstable filter */

    CODEC_OpVecSet(efr_old_A + 1, EFR_M, 0);
    /* Variables in pre_proc.c: */
    EFR_Init_Pre_Process ();

    /* Variables in q_gains.c: */
    CODEC_OpVecSet(efr_enc_past_qua_en, 4, -2381);/* past quantized energies */

    efr_enc_pred[0] = 44;               /* MA prediction coeff */
    efr_enc_pred[1] = 37;               /* MA prediction coeff */
    efr_enc_pred[2] = 22;               /* MA prediction coeff */
    efr_enc_pred[3] = 12;               /* MA prediction coeff */

    /* Variables in q_plsf_5.c: */
    CODEC_OpVecSet(efr_enc_past_r2_q, EFR_M, 0);/* Past quantized prediction error  */

    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  reset_enc
 *
 *   PURPOSE:
 *      resets all of the state variables for the encoder and VAD, and for
 *      the transmit DTX and Comfort Noise.
 *
 *   INPUT:
 *      None
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      None
 *
 **************************************************************************/

void EFR_reset_enc (void)
{
    EFR_encoder_reset (); /* reset all the state variables in the speech encoder*/
    EFR_Vad_reset ();     /* reset all the VAD state variables */
    EFR_reset_tx_dtx ();  /* reset all the transmit DTX and CN variables */

    return;
}
