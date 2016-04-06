/**************************************************************************
 *
 *   File Name:  d_homing.c
 *
 *   Purpose:
 *      This file contains the following functions:
 *
 *      decoder_homing_frame_test()  checks if a frame of input speech
 *                                   parameters matches the Decoder Homing
 *                                   Frame pattern.
 *
 *      decoder_reset()              called by reset_dec() to reset all of
 *                                   the state variables for the decoder
 *
 *      reset_dec()                  calls functions to reset the state
 *                                   variables for the decoder, and for
 *                                   the receive DTX and Comfort Noise.
 *
 **************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"
#include "efr_dtx.h"
#include "efr_codec.h"
#include "efr_d_homing.h"
#include "efr_q_plsf_5_tab.c"

#define PRM_NO    57

/***************************************************************************
 *
 *   FUNCTION NAME:  decoder_homing_frame_test
 *
 *   PURPOSE:
 *      Checks if a frame of input speech parameters matches the Decoder
 *      Homing Frame pattern, which is:
 *
 *      parameter    decimal value    hexidecimal value
 *      ---------    -------------    -----------------
 *      LPC 1        4                0x0004
 *      LPC 2        47               0x002F
 *      LPC 3        180              0x00B4
 *      LPC 4        144              0x0090
 *      LPC 5        62               0x003E
 *      LTP-LAG 1    342              0x0156
 *      LTP-GAIN 1   11               0x000B
 *      PULSE1_1     0                0x0000
 *      PULSE1_2     1                0x0001
 *      PULSE1_3     15               0x000F
 *      PULSE1_4     1                0x0001
 *      PULSE1_5     13               0x000D
 *      PULSE1_6     0                0x0000
 *      PULSE1_7     3                0x0003
 *      PULSE1_8     0                0x0000
 *      PULSE1_9     3                0x0003
 *      PULSE1_10    0                0x0000
 *      FCB-GAIN 1   3                0x0003
 *      LTP-LAG 2    54               0x0036
 *      LTP-GAIN 2   1                0x0001
 *      PULSE2_1     8                0x0008
 *      PULSE2_2     8                0x0008
 *      PULSE2_3     5                0x0005
 *      PULSE2_4     8                0x0008
 *      PULSE2_5     1                0x0001
 *      PULSE2_6     0                0x0000
 *      PULSE2_7     0                0x0000
 *      PULSE2_8     1                0x0001
 *      PULSE2_9     1                0x0001
 *      PULSE2_10    0                0x0000
 *      FCB-GAIN 2   0                0x0000
 *      LTP-LAG 3    342              0x0156
 *      LTP-GAIN 3   0                0x0000
 *      PULSE3_1     0                0x0000
 *      PULSE3_2     0                0x0000
 *      PULSE3_3     0                0x0000
 *      PULSE3_4     0                0x0000
 *      PULSE3_5     0                0x0000
 *      PULSE3_6     0                0x0000
 *      PULSE3_7     0                0x0000
 *      PULSE3_8     0                0x0000
 *      PULSE3_9     0                0x0000
 *      PULSE3_10    0                0x0000
 *      FCB-GAIN 3   0                0x0000
 *      LTP-LAG 4    54               0x0036
 *      LTP-GAIN 4   11               0x000B
 *      PULSE4_1     0                0x0000
 *      PULSE4_2     0                0x0000
 *      PULSE4_3     0                0x0000
 *      PULSE4_4     0                0x0000
 *      PULSE4_5     0                0x0000
 *      PULSE4_6     0                0x0000
 *      PULSE4_7     0                0x0000
 *      PULSE4_8     0                0x0000
 *      PULSE4_9     0                0x0000
 *      PULSE4_10    0                0x0000
 *      FCB-GAIN 4   0                0x0000
 *
 *   INPUT:
 *      parm[]  one frame of speech parameters in parallel format
 *
 *      nbr_of_params
 *              the number of consecutive parameters in parm[] to match
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      0       input frame does not match the Decoder Homing Frame pattern.
 *      1       input frame matches the Decoder Homing Frame pattern.
 *
 **************************************************************************/

Word16 EFR_decoder_homing_frame_test (Word16 parm[], Word16 nbr_of_params)
{
    static const Word16 dhf_mask[PRM_NO] =
    {
        0x0004,                 /* LPC 1 */
        0x002f,                 /* LPC 2 */
        0x00b4,                 /* LPC 3 */
        0x0090,                 /* LPC 4 */
        0x003e,                 /* LPC 5 */

        0x0156,                 /* LTP-LAG 1 */
        0x000b,                 /* LTP-GAIN 1 */
        0x0000,                 /* PULSE 1_1 */
        0x0001,                 /* PULSE 1_2 */
        0x000f,                 /* PULSE 1_3 */
        0x0001,                 /* PULSE 1_4 */
        0x000d,                 /* PULSE 1_5 */
        0x0000,                 /* PULSE 1_6 */
        0x0003,                 /* PULSE 1_7 */
        0x0000,                 /* PULSE 1_8 */
        0x0003,                 /* PULSE 1_9 */
        0x0000,                 /* PULSE 1_10 */
        0x0003,                 /* FCB-GAIN 1 */

        0x0036,                 /* LTP-LAG 2 */
        0x0001,                 /* LTP-GAIN 2 */
        0x0008,                 /* PULSE 2_1 */
        0x0008,                 /* PULSE 2_2 */
        0x0005,                 /* PULSE 2_3 */
        0x0008,                 /* PULSE 2_4 */
        0x0001,                 /* PULSE 2_5 */
        0x0000,                 /* PULSE 2_6 */
        0x0000,                 /* PULSE 2_7 */
        0x0001,                 /* PULSE 2_8 */
        0x0001,                 /* PULSE 2_9 */
        0x0000,                 /* PULSE 2_10 */
        0x0000,                 /* FCB-GAIN 2 */

        0x0156,                 /* LTP-LAG 3 */
        0x0000,                 /* LTP-GAIN 3 */
        0x0000,                 /* PULSE 3_1 */
        0x0000,                 /* PULSE 3_2 */
        0x0000,                 /* PULSE 3_3 */
        0x0000,                 /* PULSE 3_4 */
        0x0000,                 /* PULSE 3_5 */
        0x0000,                 /* PULSE 3_6 */
        0x0000,                 /* PULSE 3_7 */
        0x0000,                 /* PULSE 3_8 */
        0x0000,                 /* PULSE 3_9 */
        0x0000,                 /* PULSE 3_10 */
        0x0000,                 /* FCB-GAIN 3 */

        0x0036,                 /* LTP-LAG 4 */
        0x000b,                 /* LTP-GAIN 4 */
        0x0000,                 /* PULSE 4_1 */
        0x0000,                 /* PULSE 4_2 */
        0x0000,                 /* PULSE 4_3 */
        0x0000,                 /* PULSE 4_4 */
        0x0000,                 /* PULSE 4_5 */
        0x0000,                 /* PULSE 4_6 */
        0x0000,                 /* PULSE 4_7 */
        0x0000,                 /* PULSE 4_8 */
        0x0000,                 /* PULSE 4_9 */
        0x0000,                 /* PULSE 4_10 */
        0x0000                  /* FCB-GAIN 4 */ };

    Word16 i, j;

    for (i = 0; i < nbr_of_params; i++)
    {
        j = parm[i] ^ dhf_mask[i];

        if (j)
            break;
    }

    return !j;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  decoder_reset
 *
 *   PURPOSE:
 *      resets all of the state variables for the decoder
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

void EFR_decoder_reset (void)
{
    /* External declarations for decoder variables which need to be reset */

    /* variable defined in decoder.c */
    /* ----------------------------- */
    extern Word16 efr_synth_buf[EFR_L_FRAME + EFR_M];

    /* variable defined in agc.c */
    /* -------------------------- */
    extern Word16 efr_past_gain;

    /* variables defined in d_gains.c */
    /* ------------------------------ */
    /* Error concealment */
    extern Word16 efr_pbuf[5], efr_past_gain_pit, efr_prev_gp, efr_gbuf[5], efr_past_gain_code,
     efr_prev_gc;

    /* CNI */
    extern Word16 efr_gcode0_CN, efr_gain_code_old_CN, efr_gain_code_new_CN;
    extern Word16 efr_gain_code_muting_CN;

    /* Memories of gain dequantization: */
    extern Word16 efr_dec_past_qua_en[4], efr_dec_pred[4];

    /* variables defined in d_plsf_5.c */
    /* ------------------------------ */
    /* Past quantized prediction error */
    extern Word16 efr_dec_past_r2_q[EFR_M];

    /* Past dequantized lsfs */
    extern Word16 efr_past_lsf_q[EFR_M];

    /* CNI */
    extern Word16 efr_lsf_p_CN[EFR_M], efr_lsf_new_CN[EFR_M], efr_lsf_old_CN[EFR_M];

    /* variables defined in dec_lag6.c */
    /* ------------------------------ */
    extern Word16 efr_old_T0;

    /* variable defined in preemph.c */
    /* ------------------------------ */
    extern Word16 efr_mem_pre;

    /* reset all the decoder state variables */
    /* ------------------------------------- */

    /* Variable in decoder.c: */
    CODEC_OpVecSet(efr_synth_buf, EFR_M, 0);

    /* Variables in dec_12k2.c: */
    EFR_Init_Decoder_12k2 ();

    /* Variable in agc.c: */
    efr_past_gain = 4096;

    /* Variables in d_gains.c: */

    CODEC_OpVecSet(efr_pbuf, 5, 410);
    CODEC_OpVecSet(efr_gbuf, 5, 1);

    efr_past_gain_pit = 0;          /* Error concealment */
    efr_prev_gp = 4096;             /* Error concealment */
    efr_past_gain_code = 0;         /* Error concealment */
    efr_prev_gc = 1;                /* Error concealment */
    efr_gcode0_CN = 0;              /* CNI */
    efr_gain_code_old_CN = 0;       /* CNI */
    efr_gain_code_new_CN = 0;       /* CNI */
    efr_gain_code_muting_CN = 0;    /* CNI */

    CODEC_OpVecSet(efr_dec_past_qua_en, 4, -2381);/* past quantized energies */

    efr_dec_pred[0] = 44;               /* MA prediction coeff */
    efr_dec_pred[1] = 37;               /* MA prediction coeff */
    efr_dec_pred[2] = 22;               /* MA prediction coeff */
    efr_dec_pred[3] = 12;               /* MA prediction coeff */

    /* Variables in d_plsf_5.c: */
    CODEC_OpVecSet(efr_dec_past_r2_q, EFR_M, 0);      /* Past quantized prediction error */
    CODEC_OpVecCpy(efr_past_lsf_q, (Word16 *)mean_lsf, EFR_M);/* Past dequantized lsfs */
    CODEC_OpVecCpy(efr_lsf_p_CN, (Word16 *)mean_lsf, EFR_M);/* CNI */
    CODEC_OpVecCpy(efr_lsf_new_CN, (Word16 *)mean_lsf, EFR_M);/* CNI */
    CODEC_OpVecCpy(efr_lsf_old_CN, (Word16 *)mean_lsf, EFR_M);/* CNI */

    /* Variable in dec_lag6.c: */
    efr_old_T0 = 40;                /* Old integer lag */

    /* Variable in preemph.c: */
    efr_mem_pre = 0;                /* Filter memory */

    /* Variables in pstfilt2.c: */
    EFR_Init_Post_Filter ();

    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  reset_dec
 *
 *   PURPOSE:
 *      resets all of the state variables for the decoder, and for the
 *      receive DTX and Comfort Noise.
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

void EFR_reset_dec (void)
{
    EFR_decoder_reset (); /* reset all the state variables in the speech decoder*/
    EFR_reset_rx_dtx ();  /* reset all the receive DTX and CN state variables */

    return;
}
