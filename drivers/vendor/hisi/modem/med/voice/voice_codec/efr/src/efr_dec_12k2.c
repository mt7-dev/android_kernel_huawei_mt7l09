/***************************************************************************
 *
 *  FILE NAME:    dec_12k2.c
 *
 *  FUNCTIONS DEFINED IN THIS FILE:
 *                   Init_Decoder_12k2   and  Decoder_12k2
 *
 *
 *  Init_Decoder_12k2():
 *      Initialization of variables for the decoder section.
 *
 *  Decoder_12k2():
 *      Speech decoder routine operating on a frame basis.
 *
 ***************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_codec.h"
#include "efr_cnst.h"

#include "efr_dtx.h"

extern Word16 efr_dtx_mode;

/*---------------------------------------------------------------*
 *   Decoder constant parameters (defined in "cnst.h")           *
 *---------------------------------------------------------------*
 *   EFR_L_FRAME     : Frame size.                                   *
 *   EFR_L_FRAME_BY2 : Half the frame size.                          *
 *   EFR_L_SUBFR     : Sub-frame size.                               *
 *   EFR_M           : LPC order.                                    *
 *   EFR_MP1         : LPC order+1                                   *
 *   EFR_PIT_MIN     : Minimum pitch lag.                            *
 *   EFR_PIT_MAX     : Maximum pitch lag.                            *
 *   EFR_L_INTERPOL  : Length of filter for interpolation            *
 *   EFR_PRM_SIZE    : size of vector containing analysis parameters *
 *---------------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/

 /* Excitation vector */

static Word16 efr_old_exc[EFR_L_FRAME + EFR_PIT_MAX + EFR_L_INTERPOL];
static Word16 *efr_exc;

 /* Lsp (Line spectral pairs) */

static Word16 efr_lsp_old[EFR_M];

 /* Filter's memory */

static Word16 efr_mem_syn[EFR_M];

 /* Memories for bad frame handling */

static Word16 efr_prev_bf;
static Word16 efr_state;

/***************************************************************************
 *
 *   FUNCTION:  Init_Decoder_12k2
 *
 *   PURPOSE: Initialization of variables for the decoder section.
 *
 ***************************************************************************/

void EFR_Init_Decoder_12k2 (void)
{
    /* Initialize static pointer */

    efr_exc = efr_old_exc + EFR_PIT_MAX + EFR_L_INTERPOL;

    /* Static vectors to zero */

    CODEC_OpVecSet(efr_old_exc, EFR_PIT_MAX + EFR_L_INTERPOL, 0);
    CODEC_OpVecSet(efr_mem_syn, EFR_M, 0);
    /* Initialize efr_lsp_old [] */

    efr_lsp_old[0] = 30000;
    efr_lsp_old[1] = 26000;
    efr_lsp_old[2] = 21000;
    efr_lsp_old[3] = 15000;
    efr_lsp_old[4] = 8000;
    efr_lsp_old[5] = 0;
    efr_lsp_old[6] = -8000;
    efr_lsp_old[7] = -15000;
    efr_lsp_old[8] = -21000;
    efr_lsp_old[9] = -26000;

    /* Initialize memories of bad frame handling */

    efr_prev_bf = 0;
    efr_state = 0;

    return;
}

/***************************************************************************
 *
 *   FUNCTION:  Decoder_12k2
 *
 *   PURPOSE:   Speech decoder routine.
 *
 ***************************************************************************/

void EFR_Decoder_12k2 (
    Word16 parm[], /* input : vector of synthesis parameters
                      parm[0] = bad frame indicator (bfi)       */
    Word16 synth[],/* output: synthesis speech                  */
    Word16 A_t[],  /* output: decoded LP filter in 4 subframes  */
    Word16 TAF,
    Word16 SID_flag
)
{

    /* LPC coefficients */

    Word16 *Az;                 /* Pointer on A_t */

    /* LSPs */

    Word16 lsp_new[EFR_M];
    Word16 lsp_mid[EFR_M];

    /* Algebraic codevector */

    Word16 code[EFR_L_SUBFR];

    /* excitation */

    Word16 excp[EFR_L_SUBFR];

    /* Scalars */

    Word16 i, i_subfr;
    Word16 T0, T0_frac, index;
    Word16 gain_pit, gain_code, bfi, pit_sharp;
    Word16 temp;
    Word32 L_temp;

    extern Word16 efr_rxdtx_ctrl, efr_rx_dtx_state;
    extern Word32 efr_L_pn_seed_rx;

    /* Test bad frame indicator (bfi) */

    bfi = *parm++;

    /* Set efr_state machine */


    if (bfi != 0)
    {
        efr_state = add (efr_state, 1);
    }
    else if (sub (efr_state, 6) == 0)
    {
        efr_state = 5;
    }
    else
    {
        efr_state = 0;
    }


    if (sub (efr_state, 6) > 0)
    {
        efr_state = 6;
    }
    EFR_rx_dtx (&efr_rxdtx_ctrl, TAF, bfi, SID_flag);

    /* If this frame is the first speech frame after CNI period,     */
    /* set the BFH state machine to an appropriate state depending   */
    /* on whether there was DTX muting before start of speech or not */
    /* If there was DTX muting, the first speech frame is muted.     */
    /* If there was no DTX muting, the first speech frame is not     */
    /* muted. The BFH state machine starts from state 5, however, to */
    /* keep the audible noise resulting from a SID frame which is    */
    /* erroneously interpreted as a good speech frame as small as    */
    /* possible (the decoder output in this case is quickly muted)   */

    if ((efr_rxdtx_ctrl & EFR_RX_FIRST_SP_FLAG) != 0)
    {

        if ((efr_rxdtx_ctrl & EFR_RX_PREV_DTX_MUTING) != 0)
        {
            efr_state = 5;
            efr_prev_bf = 1;
        }
        else
        {
            efr_state = 5;
            efr_prev_bf = 0;
        }
    }

#if (WMOPS)
    fwc ();                     /* function worst case */

    /* Note! The following test is performed only for determining
       whether or not DTX mode is active, in order to switch off
       worst worst case complexity printout when DTX mode is active
    */
    if ((efr_rxdtx_ctrl & EFR_RX_SP_FLAG) == 0)
    {
        efr_dtx_mode = 1;
    }
#endif

    EFR_D_plsf_5 (parm, lsp_mid, lsp_new, bfi, efr_rxdtx_ctrl, efr_rx_dtx_state);

    /* Advance synthesis parameters pointer */
    parm += 5;


    if ((efr_rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {
        /* Interpolation of LPC for the 4 subframes */

        EFR_Int_lpc (efr_lsp_old, lsp_mid, lsp_new, A_t);
    }
    else
    {
        /* Comfort noise: use the same parameters in each subframe */
        EFR_Lsp_Az (lsp_new, A_t);

        CODEC_OpVecCpy(A_t + EFR_MP1, A_t, EFR_MP1);
        CODEC_OpVecCpy(A_t + 2 * EFR_MP1, A_t, EFR_MP1);
        CODEC_OpVecCpy(A_t + 3 * EFR_MP1, A_t, EFR_MP1);

    }

    /* update the LSPs for the next frame */
    CODEC_OpVecCpy(efr_lsp_old, lsp_new, EFR_M);
    /*---------------------------------------------------------------------*
     *       Loop for every subframe in the analysis frame                 *
     *---------------------------------------------------------------------*
     * The subframe size is EFR_L_SUBFR and the loop is repeated               *
     * EFR_L_FRAME/L_SUBFR times                                               *
     *     - decode the pitch delay                                        *
     *     - decode algebraic code                                         *
     *     - decode pitch and codebook gains                               *
     *     - find the excitation and compute synthesis speech              *
     *---------------------------------------------------------------------*/

    /* pointer to interpolated LPC parameters */
    Az = A_t;

    for (i_subfr = 0; i_subfr < EFR_L_FRAME; i_subfr += EFR_L_SUBFR)
    {

        index = *parm++;                /* pitch index */


        if ((efr_rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
        {
            T0 = EFR_Dec_lag6 (index, EFR_PIT_MIN, EFR_PIT_MAX, i_subfr, EFR_L_FRAME_BY2,
                           &T0_frac, bfi);
            /*-------------------------------------------------*
             * - Find the adaptive codebook vector.            *
             *-------------------------------------------------*/

            EFR_Pred_lt_6 (&efr_exc[i_subfr], T0, T0_frac, EFR_L_SUBFR);
        }
        else
        {
            T0 = EFR_L_SUBFR;
        }

        /*-------------------------------------------------------*
         * - Decode pitch gain.                                  *
         *-------------------------------------------------------*/

        index = *parm++;

        gain_pit = EFR_d_gain_pitch (index, bfi, efr_state, efr_prev_bf, efr_rxdtx_ctrl);

        /*-------------------------------------------------------*
         * - Decode innovative codebook.                         *
         *-------------------------------------------------------*/


        if ((efr_rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
        {
            EFR_Dec_10i40_35bits (parm, code);
        }
        else
        {   /* Use pseudo noise for excitation when SP_flag == 0 */
            EFR_build_CN_code (code, &efr_L_pn_seed_rx);
        }

        parm += 10;

        /*-------------------------------------------------------*
         * - Add the pitch contribution to code[].               *
         *-------------------------------------------------------*/

        /* pit_sharp = gain_pit;                   */
        /* if (pit_sharp > 1.0) pit_sharp = 1.0;   */

        pit_sharp = shl (gain_pit, 3);

        /* This loop is not entered when SP_FLAG is 0 */

        CODEC_OpVcMultQ15Add(code + T0, code, EFR_L_SUBFR - T0, pit_sharp, code + T0);
        /* post processing of excitation elements */

        /* This test is not passed when SP_FLAG is 0 */
        if (sub (pit_sharp, 16384) > 0)
        {
            for (i = 0; i < EFR_L_SUBFR; i++)
            {
                temp = mult (efr_exc[i + i_subfr], pit_sharp);
                L_temp = L_mult (temp, gain_pit);
                L_temp = L_shl (L_temp, 1);
                excp[i] = round (L_temp);

            }
        }
        /*-------------------------------------------------*
         * - Decode codebook gain.                         *
         *-------------------------------------------------*/

        index = *parm++;                /* index of energy VQ */

        EFR_d_gain_code (index, code, EFR_L_SUBFR, &gain_code, bfi, efr_state, efr_prev_bf,
                     efr_rxdtx_ctrl, i_subfr, efr_rx_dtx_state);

        /*-------------------------------------------------------*
         * - Find the total excitation.                          *
         * - Find synthesis speech corresponding to efr_exc[].       *
         *-------------------------------------------------------*/
        for (i = 0; i < EFR_L_SUBFR; i++)
        {
            /* efr_exc[i] = gain_pit*efr_exc[i] + gain_code*code[i]; */

            L_temp = L_mult (efr_exc[i + i_subfr], gain_pit);
            L_temp = L_mac (L_temp, code[i], gain_code);
            L_temp = L_shl (L_temp, 3);

            efr_exc[i + i_subfr] = round (L_temp);

        }

        if (sub (pit_sharp, 16384) > 0)
        {
            CODEC_OpVvAdd(excp, efr_exc + i_subfr, EFR_L_SUBFR, excp);
            EFR_agc2 (&efr_exc[i_subfr], excp, EFR_L_SUBFR);
            EFR_Syn_filt (Az, excp, &synth[i_subfr], EFR_L_SUBFR, efr_mem_syn, 1);
        }
        else
        {
            EFR_Syn_filt (Az, &efr_exc[i_subfr], &synth[i_subfr], EFR_L_SUBFR, efr_mem_syn, 1);
        }

        /* interpolated LPC parameters for next subframe */
        Az += EFR_MP1;
    }

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> shift to the left by EFR_L_FRAME  efr_exc[]           *
     *--------------------------------------------------*/

    CODEC_OpVecCpy(&efr_old_exc[0], &efr_old_exc[EFR_L_FRAME], EFR_PIT_MAX + EFR_L_INTERPOL);

    efr_prev_bf = bfi;

    return;
}
