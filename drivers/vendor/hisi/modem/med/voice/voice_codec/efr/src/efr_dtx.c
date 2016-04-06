/***************************************************************************
 *
 *   File Name: dtx.c
 *
 *   Purpose:   Contains functions for performing DTX operation and comfort
 *              noise generation.
 *
 *     Below is a listing of all the functions appearing in the file.
 *     The functions are arranged according to their purpose.  Under
 *     each heading, the ordering is hierarchical.
 *
 *     Resetting of static variables of TX DTX:
 *       reset_tx_dtx()
 *     Resetting of static variables of RX DTX:
 *       reset_rx_dtx()
 *
 *     TX DTX handler (called by the speech encoder):
 *       tx_dtx()
 *     RX DTX handler (called by the speech decoder):
 *       rx_dtx()
 *     Encoding of comfort noise parameters into SID frame:
 *       CN_encoding()
 *     Encoding of SID codeword into SID frame:
 *       sid_codeword_encoding()
 *     Detecting of SID codeword from a frame:
 *       sid_frame_detection()
 *     Update the LSF parameter history:
 *       update_lsf_history()
 *     Update the reference LSF parameter vector:
 *       update_lsf_p_CN()
 *     Compute the averaged LSF parameter vector:
 *       aver_lsf_history()
 *     Update the fixed codebook gain parameter history of the encoder:
 *       update_gain_code_history_tx()
 *     Update the fixed codebook gain parameter history of the decoder:
 *       update_gain_code_history_rx()
 *     Compute the unquantized fixed codebook gain:
 *       compute_CN_excitation_gain()
 *     Update the reference fixed codebook gain:
 *       update_gcode0_CN()
 *     Compute the averaged fixed codebook gain:
 *       aver_gain_code_history()
 *     Compute the comfort noise fixed codebook excitation:
 *       build_CN_code()
 *       Generate a random integer value:
 *         pseudonoise()
 *     Interpolate a comfort noise parameter value over the comfort noise
 *       update period:
 *       interpolate_CN_param()
 *     Interpolate comfort noise LSF pparameter values over the comfort
 *       noise update period:
 *       interpolate_CN_lsf()
 *         interpolate_CN_param()
 *
 **************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"
#include "efr_sig_proc.h"
#include "efr_dtx.h"

/* Inverse values of DTX hangover period and DTX hangover period + 1 */

#define INV_DTX_HANGOVER (0x7fff / EFR_DTX_HANGOVER)
#define INV_DTX_HANGOVER_P1 (0x7fff / (EFR_DTX_HANGOVER+1))

#define NB_PULSE 10 /* Number of pulses in fixed codebook excitation */

/* SID frame classification thresholds */

#define VALID_SID_THRESH 2
#define INVALID_SID_THRESH 16

/* Constant DTX_ELAPSED_THRESHOLD is used as threshold for allowing
   SID frame updating without hangover period in case when elapsed
   time measured from previous SID update is below 24 */

#define DTX_ELAPSED_THRESHOLD (24 + EFR_DTX_HANGOVER - 1)

/* Index map for encoding and detecting SID codeword */

static const Word16 efr_SID_codeword_bit_idx[95] =
{
     45,  46,  48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,  64,  65,
     66,  67,  68,  94,  95,  96,  98,  99, 100, 101,
    102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
    171, 196, 197, 198, 199, 200, 201, 202, 203, 204,
    205, 206, 207, 208, 209, 212, 213, 214, 215, 216,
    217, 218, 219, 220, 221
};

Word16 efr_txdtx_ctrl;              /* Encoder DTX control word                */
Word16 efr_rxdtx_ctrl;              /* Decoder DTX control word                */
Word16 efr_CN_excitation_gain;      /* Unquantized fixed codebook gain         */
Word32 efr_L_pn_seed_tx;            /* PN generator seed (encoder)             */
Word32 efr_L_pn_seed_rx;            /* PN generator seed (decoder)             */
Word16 efr_rx_dtx_state;            /* State of comfort noise insertion period */

static Word16 efr_txdtx_hangover;   /* Length of hangover period (VAD=0, SP=1) */
static Word16 efr_rxdtx_aver_period;/* Length of hangover period (VAD=0, SP=1) */
static Word16 efr_txdtx_N_elapsed;  /* Measured time from previous SID frame   */
static Word16 efr_rxdtx_N_elapsed;  /* Measured time from previous SID frame   */
static Word16 efr_old_CN_mem_tx[6]; /* The most recent CN parameters are stored*/
static Word16 efr_prev_SID_frames_lost; /* Counter for lost SID frames         */
static Word16 efr_buf_p_tx;         /* Circular buffer pointer for gain code
                                   history  update in tx                   */
static Word16 efr_buf_p_rx;         /* Circular buffer pointer for gain code
                                   history update in rx                    */

Word16 efr_lsf_old_tx[EFR_DTX_HANGOVER][EFR_M]; /* Comfort noise LSF averaging buffer  */
Word16 efr_lsf_old_rx[EFR_DTX_HANGOVER][EFR_M]; /* Comfort noise LSF averaging buffer  */

Word16 efr_gain_code_old_tx[4 * EFR_DTX_HANGOVER]; /* Comfort noise gain averaging
                                              buffer                       */
Word16 efr_gain_code_old_rx[4 * EFR_DTX_HANGOVER]; /* Comfort noise gain averaging
                                              buffer                       */

/*************************************************************************
 *
 *   FUNCTION NAME: reset_tx_dtx
 *
 *   PURPOSE:  Resets the static variables of the TX DTX handler to their
 *             initial values
 *
 *************************************************************************/

void EFR_reset_tx_dtx ()
{
    Word16 i;

    /* suppose infinitely long speech period before start */

    efr_txdtx_hangover = EFR_DTX_HANGOVER;
    efr_txdtx_N_elapsed = 0x7fff;
    efr_txdtx_ctrl = EFR_TX_SP_FLAG | EFR_TX_VAD_FLAG;

    CODEC_OpVecSet(efr_old_CN_mem_tx, 6, 0);

    for (i = 0; i < EFR_DTX_HANGOVER; i++)
    {
        efr_lsf_old_tx[i][0] = 1384;
        efr_lsf_old_tx[i][1] = 2077;
        efr_lsf_old_tx[i][2] = 3420;
        efr_lsf_old_tx[i][3] = 5108;
        efr_lsf_old_tx[i][4] = 6742;
        efr_lsf_old_tx[i][5] = 8122;
        efr_lsf_old_tx[i][6] = 9863;
        efr_lsf_old_tx[i][7] = 11092;
        efr_lsf_old_tx[i][8] = 12714;
        efr_lsf_old_tx[i][9] = 13701;
    }

    CODEC_OpVecSet(efr_gain_code_old_tx, 4 * EFR_DTX_HANGOVER, 0);

    efr_L_pn_seed_tx = EFR_PN_INITIAL_SEED;

    efr_buf_p_tx = 0;
    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: reset_rx_dtx
 *
 *   PURPOSE:  Resets the static variables of the RX DTX handler to their
 *             initial values
 *
 *************************************************************************/

void EFR_reset_rx_dtx ()
{
    Word16 i;

    /* suppose infinitely long speech period before start */

    efr_rxdtx_aver_period = EFR_DTX_HANGOVER;
    efr_rxdtx_N_elapsed = 0x7fff;
    efr_rxdtx_ctrl = EFR_RX_SP_FLAG;

    for (i = 0; i < EFR_DTX_HANGOVER; i++)
    {
        efr_lsf_old_rx[i][0] = 1384;
        efr_lsf_old_rx[i][1] = 2077;
        efr_lsf_old_rx[i][2] = 3420;
        efr_lsf_old_rx[i][3] = 5108;
        efr_lsf_old_rx[i][4] = 6742;
        efr_lsf_old_rx[i][5] = 8122;
        efr_lsf_old_rx[i][6] = 9863;
        efr_lsf_old_rx[i][7] = 11092;
        efr_lsf_old_rx[i][8] = 12714;
        efr_lsf_old_rx[i][9] = 13701;
    }

    CODEC_OpVecSet(efr_gain_code_old_rx, 4 * EFR_DTX_HANGOVER, 0);

    efr_L_pn_seed_rx = EFR_PN_INITIAL_SEED;
    efr_rx_dtx_state = EFR_CN_INT_PERIOD - 1;

    efr_prev_SID_frames_lost = 0;
    efr_buf_p_rx = 0;

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: tx_dtx
 *
 *   PURPOSE: DTX handler of the speech encoder. Determines when to add
 *            the hangover period to the end of the speech burst, and
 *            also determines when to use old SID parameters, and when
 *            to update the SID parameters. This function also initializes
 *            the pseudo noise generator shift register.
 *
 *            Operation of the TX DTX handler is based on the VAD flag
 *            given as input from the speech encoder.
 *
 *   INPUTS:      VAD_flag      Voice activity decision
 *                *txdtx_ctrl   Old encoder DTX control word
 *
 *   OUTPUTS:     *txdtx_ctrl   Updated encoder DTX control word
 *                L_pn_seed_tx  Initialized pseudo noise generator shift
 *                              register (global variable)
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_tx_dtx (
    Word16 VAD_flag,
    Word16 *txdtx_ctrl
)
{

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    efr_txdtx_N_elapsed = add (efr_txdtx_N_elapsed, 1);

    /* If voice activity was detected, reset hangover counter */


    if (sub (VAD_flag, 1) == 0)
    {
        efr_txdtx_hangover = EFR_DTX_HANGOVER;
        *txdtx_ctrl = EFR_TX_SP_FLAG | EFR_TX_VAD_FLAG;
    }
    else
    {

        if (efr_txdtx_hangover == 0)
        {
            /* Hangover period is over, SID should be updated */

            efr_txdtx_N_elapsed = 0;

            /* Check if this is the first frame after hangover period */

            if ((*txdtx_ctrl & EFR_TX_HANGOVER_ACTIVE) != 0)
            {
                *txdtx_ctrl = EFR_TX_PREV_HANGOVER_ACTIVE
                    | EFR_TX_SID_UPDATE;
                efr_L_pn_seed_tx = EFR_PN_INITIAL_SEED;
            }
            else
            {
                *txdtx_ctrl = EFR_TX_SID_UPDATE;
            }
        }
        else
        {
            /* Hangover period is not over, update hangover counter */
            efr_txdtx_hangover = sub (efr_txdtx_hangover, 1);

            /* Check if elapsed time from last SID update is greater than
               threshold. If not, set SP=0 (although hangover period is not
               over) and use old SID parameters for new SID frame.
               N_elapsed counter must be summed with hangover counter in order
               to avoid erroneus SP=1 decision in case when N_elapsed is grown
               bigger than threshold and hangover period is still active */


            if (sub (add (efr_txdtx_N_elapsed, efr_txdtx_hangover),
                     DTX_ELAPSED_THRESHOLD) < 0)
            {
                /* old SID frame should be used */
                *txdtx_ctrl = EFR_TX_USE_OLD_SID;
            }
            else
            {

                if ((*txdtx_ctrl & EFR_TX_HANGOVER_ACTIVE) != 0)
                {
                    *txdtx_ctrl = EFR_TX_PREV_HANGOVER_ACTIVE
                        | EFR_TX_HANGOVER_ACTIVE
                        | EFR_TX_SP_FLAG;
                }
                else
                {
                    *txdtx_ctrl = EFR_TX_HANGOVER_ACTIVE
                        | EFR_TX_SP_FLAG;
                }
            }
        }
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: rx_dtx
 *
 *   PURPOSE: DTX handler of the speech decoder. Determines when to update
 *            the reference comfort noise parameters (LSF and gain) at the
 *            end of the speech burst. Also classifies the incoming frames
 *            according to SID flag and BFI flag
 *            and determines when the transmission is active during comfort
 *            noise insertion. This function also initializes the pseudo
 *            noise generator shift register.
 *
 *            Operation of the RX DTX handler is based on measuring the
 *            lengths of speech bursts and the lengths of the pauses between
 *            speech bursts to determine when there exists a hangover period
 *            at the end of a speech burst. The idea is to keep in sync with
 *            the TX DTX handler to be able to update the reference comfort
 *            noise parameters at the same time instances.
 *
 *   INPUTS:      *rxdtx_ctrl   Old decoder DTX control word
 *                TAF           Time alignment flag
 *                bfi           Bad frame indicator flag
 *                SID_flag      Silence descriptor flag
 *
 *   OUTPUTS:     *rxdtx_ctrl   Updated decoder DTX control word
 *                rx_dtx_state  Updated state of comfort noise interpolation
 *                              period (global variable)
 *                L_pn_seed_rx  Initialized pseudo noise generator shift
 *                              register (global variable)
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_rx_dtx (
    Word16 *rxdtx_ctrl,
    Word16 TAF,
    Word16 bfi,
    Word16 SID_flag
)
{
    Word16 frame_type;

    /* Frame classification according to bfi-flag and ternary-valued
       SID flag. The frames between SID updates (not actually trans-
       mitted) are also classified here; they will be discarded later
       and provided with "NO TRANSMISSION"-flag */




    if ((sub (SID_flag, 2) == 0) && (bfi == 0))
    {
        frame_type = EFR_VALID_SID_FRAME;
    }
    else if ((SID_flag == 0) && (bfi == 0))
    {
        frame_type = EFR_GOOD_SPEECH_FRAME;
    }
    else if ((SID_flag == 0) && (bfi != 0))
    {
        frame_type = EFR_UNUSABLE_FRAME;
    }
    else
    {
        frame_type = EFR_INVALID_SID_FRAME;
    }

    /* Update of decoder state */
    /* Previous frame was classified as a speech frame */

    if ((*rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {

        if (sub (frame_type, EFR_VALID_SID_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_FIRST_SID_UPDATE;
        }
        else if (sub (frame_type, EFR_INVALID_SID_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_FIRST_SID_UPDATE
                        | EFR_RX_INVALID_SID_FRAME;
        }
        else if (sub (frame_type, EFR_UNUSABLE_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_SP_FLAG;
        }
        else if (sub (frame_type, EFR_GOOD_SPEECH_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_SP_FLAG;
        }
    }
    else
    {

        if (sub (frame_type, EFR_VALID_SID_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_CONT_SID_UPDATE;
        }
        else if (sub (frame_type, EFR_INVALID_SID_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_CONT_SID_UPDATE
                        | EFR_RX_INVALID_SID_FRAME;
        }
        else if (sub (frame_type, EFR_UNUSABLE_FRAME) == 0)
        {
            *rxdtx_ctrl = EFR_RX_CNI_BFI;
        }
        else if (sub (frame_type, EFR_GOOD_SPEECH_FRAME) == 0)
        {
            /* If the previous frame (during CNI period) was muted,
               raise the EFR_RX_PREV_DTX_MUTING flag */

            if ((*rxdtx_ctrl & EFR_RX_DTX_MUTING) != 0)
            {
                *rxdtx_ctrl = EFR_RX_SP_FLAG | EFR_RX_FIRST_SP_FLAG
                            | EFR_RX_PREV_DTX_MUTING;

            }
            else
            {
                *rxdtx_ctrl = EFR_RX_SP_FLAG | EFR_RX_FIRST_SP_FLAG;

            }
        }
    }



    if ((*rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {
        efr_prev_SID_frames_lost = 0;
        efr_rx_dtx_state = EFR_CN_INT_PERIOD - 1;
    }
    else
    {
        /* First SID frame */

        if ((*rxdtx_ctrl & EFR_RX_FIRST_SID_UPDATE) != 0)
        {
            efr_prev_SID_frames_lost = 0;
            efr_rx_dtx_state = EFR_CN_INT_PERIOD - 1;
        }

        /* SID frame detected, but not the first SID */

        if ((*rxdtx_ctrl & EFR_RX_CONT_SID_UPDATE) != 0)
        {
            efr_prev_SID_frames_lost = 0;


            if (sub (frame_type, EFR_VALID_SID_FRAME) == 0)
            {
                efr_rx_dtx_state = 0;
            }
            else if (sub (frame_type, EFR_INVALID_SID_FRAME) == 0)
            {

                if (sub(efr_rx_dtx_state, (EFR_CN_INT_PERIOD - 1)) < 0)
                {
                    efr_rx_dtx_state = add(efr_rx_dtx_state, 1);
                }
            }
        }

        /* Bad frame received in CNI mode */

        if ((*rxdtx_ctrl & EFR_RX_CNI_BFI) != 0)
        {

            if (sub (efr_rx_dtx_state, (EFR_CN_INT_PERIOD - 1)) < 0)
            {
                efr_rx_dtx_state = add (efr_rx_dtx_state, 1);
            }

            /* If an unusable frame is received during CNI period
               when TAF == 1, the frame is classified as a lost
               SID frame */

            if (sub (TAF, 1) == 0)
            {
                *rxdtx_ctrl = *rxdtx_ctrl | EFR_RX_LOST_SID_FRAME;

                efr_prev_SID_frames_lost = add (efr_prev_SID_frames_lost, 1);
            }
            else /* No transmission occurred */
            {
                *rxdtx_ctrl = *rxdtx_ctrl | EFR_RX_NO_TRANSMISSION;

            }


            if (sub (efr_prev_SID_frames_lost, 1) > 0)
            {
                *rxdtx_ctrl = *rxdtx_ctrl | EFR_RX_DTX_MUTING;

            }
        }
    }

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    efr_rxdtx_N_elapsed = add (efr_rxdtx_N_elapsed, 1);


    if ((*rxdtx_ctrl & EFR_RX_SP_FLAG) != 0)
    {
        efr_rxdtx_aver_period = EFR_DTX_HANGOVER;
    }
    else
    {

        if (sub (efr_rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
        {
            *rxdtx_ctrl |= EFR_RX_UPD_SID_QUANT_MEM;
            efr_rxdtx_N_elapsed = 0;
            efr_rxdtx_aver_period = 0;
            efr_L_pn_seed_rx = EFR_PN_INITIAL_SEED;
        }
        else if (efr_rxdtx_aver_period == 0)
        {
            efr_rxdtx_N_elapsed = 0;
        }
        else
        {
            efr_rxdtx_aver_period = sub (efr_rxdtx_aver_period, 1);
        }
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: CN_encoding
 *
 *   PURPOSE:  Encoding of the comfort noise parameters into a SID frame.
 *             Use old SID parameters if necessary. Set the parameter
 *             indices not used by comfort noise parameters to zero.
 *
 *   INPUTS:      params[0..56]  Comfort noise parameter frame from the
 *                               speech encoder
 *                txdtx_ctrl     TX DTX handler control word
 *
 *   OUTPUTS:     params[0..56]  Comfort noise encoded parameter frame
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_CN_encoding (
    Word16 params[],
    Word16 txdtx_ctrl
)
{
    Word16 i;


    if ((txdtx_ctrl & EFR_TX_SID_UPDATE) != 0)
    {
        /* Store new CN parameters in memory to be used later as old
           CN parameters */

        /* LPC parameter indices */
        CODEC_OpVecCpy(efr_old_CN_mem_tx, params, 5);
      /* Codebook index computed in last subframe */
        efr_old_CN_mem_tx[5] = params[56];
    }

    if ((txdtx_ctrl & EFR_TX_USE_OLD_SID) != 0)
    {
        /* Use old CN parameters previously stored in memory */
        CODEC_OpVecCpy(params, efr_old_CN_mem_tx, 5);
        params[17] = efr_old_CN_mem_tx[5];
        params[30] = efr_old_CN_mem_tx[5];
        params[43] = efr_old_CN_mem_tx[5];
        params[56] = efr_old_CN_mem_tx[5];
    }
    /* Set all the rest of the parameters to zero (SID codeword will
       be written later) */
    for (i = 0; i < 12; i++)
    {
        params[i + 5] = 0;
        params[i + 18] = 0;
        params[i + 31] = 0;
        params[i + 44] = 0;
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: sid_codeword_encoding
 *
 *   PURPOSE:  Encoding of the SID codeword into the SID frame. The SID
 *             codeword consists of 95 bits, all set to '1'.
 *
 *   INPUTS:      ser2[0..243]  Serial-mode speech parameter frame before
 *                              writing SID codeword into it
 *
 *   OUTPUTS:     ser2[0..243]  Serial-mode speech parameter frame with
 *                              SID codeword written into it
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_sid_codeword_encoding (
    Word16 ser2[]
)
{
    Word16 i;

    for (i = 0; i < 95; i++)
    {
        ser2[efr_SID_codeword_bit_idx[i]] = 1;
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: sid_frame_detection
 *
 *   PURPOSE:  Detecting of SID codeword from a received frame. The frames
 *             are classified into three categories based on how many bit
 *             errors occur in the SID codeword:
 *                 - VALID SID FRAME
 *                 - INVALID SID FRAME
 *                 - SPEECH FRAME
 *
 *   INPUTS:      ser2[0..243]  Received serial-mode speech parameter frame
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Ternary-valued SID classification flag
 *
 *************************************************************************/

Word16 EFR_sid_frame_detection (
    Word16 ser2[]
)
{
    Word16 i, nbr_errors, sid;

    /* Search for bit errors in SID codeword */
    nbr_errors = 0;
    for (i = 0; i < 95; i++)
    {

        if (ser2[efr_SID_codeword_bit_idx[i]] == 0)
        {
            nbr_errors = add (nbr_errors, 1);
        }
    }

    /* Frame classification */

    if (sub (nbr_errors, VALID_SID_THRESH) < 0)
    {                                                   /* Valid SID frame */
        sid = 2;
    }
    else if (sub (nbr_errors, INVALID_SID_THRESH) < 0)
    {                                                   /* Invalid SID frame */
        sid = 1;
    }
    else
    {                                                   /* Speech frame */
        sid = 0;
    }

    return sid;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_lsf_history
 *
 *   PURPOSE: Update the LSF parameter history. The LSF parameters kept
 *            in the buffer are used later for computing the reference
 *            LSF parameter vector and the averaged LSF parameter vector.
 *
 *   INPUTS:      lsf1[0..9]    LSF vector of the 1st half of the frame
 *                lsf2[0..9]    LSF vector of the 2nd half of the frame
 *                lsf_old[0..EFR_DTX_HANGOVER-1][0..EFR_M-1]
 *                              Old LSF history
 *
 *   OUTPUTS:     lsf_old[0..EFR_DTX_HANGOVER-1][0..EFR_M-1]
 *                              Updated LSF history
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_update_lsf_history (
    Word16 lsf1[EFR_M],
    Word16 lsf2[EFR_M],
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M]
)
{
    Word16 i;

    /* shift LSF data to make room for LSFs from current frame */
    /* This can also be implemented by using circular buffering */

    for (i = EFR_DTX_HANGOVER - 1; i > 0; i--)
    {
        CODEC_OpVecCpy(&lsf_old[i][0], &lsf_old[i-1][0], EFR_M);
    }

    /* Store new LSF data to lsf_old buffer */

    CODEC_OpVvFormWithDimi(lsf1, lsf2, EFR_M, &lsf_old[0][0]);
    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_lsf_p_CN
 *
 *   PURPOSE: Update the reference LSF parameter vector. The reference
 *            vector is computed by averaging the quantized LSF parameter
 *            vectors which exist in the LSF parameter history.
 *
 *   INPUTS:      lsf_old[0..EFR_DTX_HANGOVER-1][0..EFR_M-1]
 *                                 LSF parameter history
 *
 *   OUTPUTS:     lsf_p_CN[0..9]   Computed reference LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_update_lsf_p_CN (
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M],
    Word16 lsf_p_CN[EFR_M]
)
{
    Word16 i, j;
    Word32 L_temp;

    for (j = 0; j < EFR_M; j++)
    {
        L_temp = L_mult (INV_DTX_HANGOVER, lsf_old[0][j]);
        for (i = 1; i < EFR_DTX_HANGOVER; i++)
        {
            L_temp = L_mac (L_temp, INV_DTX_HANGOVER, lsf_old[i][j]);
        }
        lsf_p_CN[j] = round (L_temp);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: aver_lsf_history
 *
 *   PURPOSE: Compute the averaged LSF parameter vector. Computation is
 *            performed by averaging the LSF parameter vectors which exist
 *            in the LSF parameter history, together with the LSF
 *            parameter vectors of the current frame.
 *
 *   INPUTS:      lsf_old[0..EFR_DTX_HANGOVER-1][0..EFR_M-1]
 *                                   LSF parameter history
 *                lsf1[0..EFR_M-1]       LSF vector of the 1st half of the frame
 *                lsf2[0..EFR_M-1]       LSF vector of the 2nd half of the frame
 *
 *   OUTPUTS:     lsf_aver[0..EFR_M-1]   Averaged LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_aver_lsf_history (
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M],
    Word16 lsf1[EFR_M],
    Word16 lsf2[EFR_M],
    Word16 lsf_aver[EFR_M]
)
{
    Word16 i, j;
    Word32 L_temp;

    for (j = 0; j < EFR_M; j++)
    {
        L_temp = L_mult (0x3fff, lsf1[j]);
        L_temp = L_mac (L_temp, 0x3fff, lsf2[j]);
        L_temp = L_mult (INV_DTX_HANGOVER_P1, extract_h (L_temp));

        for (i = 0; i < EFR_DTX_HANGOVER; i++)
        {
            L_temp = L_mac (L_temp, INV_DTX_HANGOVER_P1, lsf_old[i][j]);
        }

        lsf_aver[j] = extract_h (L_temp);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gain_code_history_tx
 *
 *   PURPOSE: Update the fixed codebook gain parameter history of the
 *            encoder. The fixed codebook gain parameters kept in the buffer
 *            are used later for computing the reference fixed codebook
 *            gain parameter value and the averaged fixed codebook gain
 *            parameter value.
 *
 *   INPUTS:      new_gain_code   New fixed codebook gain value
 *
 *                gain_code_old_tx[0..4*EFR_DTX_HANGOVER-1]
 *                                Old fixed codebook gain history of encoder
 *
 *   OUTPUTS:     gain_code_old_tx[0..4*EFR_DTX_HANGOVER-1]
 *                                Updated fixed codebook gain history of encoder
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_update_gain_code_history_tx (
    Word16 new_gain_code,
    Word16 efr_gain_code_old_tx[4 * EFR_DTX_HANGOVER]
)
{

    /* Circular buffer */
    efr_gain_code_old_tx[efr_buf_p_tx] = new_gain_code;


    if (sub (efr_buf_p_tx, (4 * EFR_DTX_HANGOVER - 1)) == 0)
    {
        efr_buf_p_tx = 0;
    }
    else
    {
        efr_buf_p_tx = add (efr_buf_p_tx, 1);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gain_code_history_rx
 *
 *   PURPOSE: Update the fixed codebook gain parameter history of the
 *            decoder. The fixed codebook gain parameters kept in the buffer
 *            are used later for computing the reference fixed codebook
 *            gain parameter value.
 *
 *   INPUTS:      new_gain_code   New fixed codebook gain value
 *
 *                gain_code_old_tx[0..4*EFR_DTX_HANGOVER-1]
 *                                Old fixed codebook gain history of decoder
 *
 *   OUTPUTS:     gain_code_old_tx[0..4*EFR_DTX_HANGOVER-1]
 *                                Updated fixed codebk gain history of decoder
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_update_gain_code_history_rx (
    Word16 new_gain_code,
    Word16 efr_gain_code_old_rx[4 * EFR_DTX_HANGOVER]
)
{

    /* Circular buffer */
    efr_gain_code_old_rx[efr_buf_p_rx] = new_gain_code;


    if (sub (efr_buf_p_rx, (4 * EFR_DTX_HANGOVER - 1)) == 0)
    {
        efr_buf_p_rx = 0;
    }
    else
    {
        efr_buf_p_rx = add (efr_buf_p_rx, 1);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: compute_CN_excitation_gain
 *
 *   PURPOSE: Compute the unquantized fixed codebook gain. Computation is
 *            based on the energy of the Linear Prediction residual signal.
 *
 *   INPUTS:      res2[0..39]   Linear Prediction residual signal
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Unquantized fixed codebook gain
 *
 *************************************************************************/

Word16 EFR_compute_CN_excitation_gain (
    Word16 res2[EFR_L_SUBFR]
)
{
    Word16 i, norm, norm1, temp, overfl;
    Word32 L_temp;

    /* Compute the energy of the LP residual signal */

    norm = 0;
    do
    {
        overfl = 0;

        L_temp = 0L;
        for (i = 0; i < EFR_L_SUBFR; i++)
        {
            temp = shr (res2[i], norm);
            L_temp = L_mac (L_temp, temp, temp);
        }


        if (L_sub (L_temp, MAX_32) == 0)
        {
            norm = add (norm, 1);
            overfl = 1;                 /* Set the overflow flag */
        }

    }
    while (overfl != 0);

    L_temp = L_add (L_temp, 1L);             /* Avoid the case of all zeros */

    /* Take the square root of the obtained energy value (sqroot is a 2nd
       order Taylor series approximation) */

    norm1 = norm_l (L_temp);
    temp = extract_h (L_shl (L_temp, norm1));
    L_temp = L_mult (temp, temp);
    L_temp = L_sub (805306368L, L_shr (L_temp, 3));
    L_temp = L_add (L_temp, L_mult (24576, temp));

    temp = extract_h (L_temp);

    if ((norm1 & 0x0001) != 0)
    {
        temp = mult_r (temp, 23170);
        norm1 = sub (norm1, 1);
    }
    /* Divide the result of sqroot operation by sqroot(10) */

    temp = mult_r (temp, 10362);

    /* Re-scale to get the final value */

    norm1 = shr (norm1, 1);
    norm1 = sub (norm1, norm);


    if (norm1 >= 0)
    {
        temp = shr (temp, norm1);
    }
    else
    {
        temp = shl (temp, abs_s (norm1));
    }

    return temp;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gcode0_CN
 *
 *   PURPOSE: Update the reference fixed codebook gain parameter value.
 *            The reference value is computed by averaging the quantized
 *            fixed codebook gain parameter values which exist in the
 *            fixed codebook gain parameter history.
 *
 *   INPUTS:      gain_code_old[0..4*EFR_DTX_HANGOVER-1]
 *                              fixed codebook gain parameter history
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Computed reference fixed codebook gain
 *
 *************************************************************************/

Word16 EFR_update_gcode0_CN (
    Word16 gain_code_old[4 * EFR_DTX_HANGOVER]
)
{
    Word16 i;
    Word32 L_temp, L_ret;

    L_ret = 0L;
    for (i = 0; i < EFR_DTX_HANGOVER; i++)
    {
        L_temp = CODEC_OpVcMac(&gain_code_old[4 * i], 4, 0x1fff, 0);
        L_ret = L_mac (L_ret, INV_DTX_HANGOVER, extract_h (L_temp));
    }

    return extract_h (L_ret);
}

/*************************************************************************
 *
 *   FUNCTION NAME: aver_gain_code_history
 *
 *   PURPOSE: Compute the averaged fixed codebook gain parameter value.
 *            Computation is performed by averaging the fixed codebook
 *            gain parameter values which exist in the fixed codebook
 *            gain parameter history, together with the fixed codebook
 *            gain parameter value of the current subframe.
 *
 *   INPUTS:      CN_excitation_gain
 *                              Unquantized fixed codebook gain value
 *                              of the current subframe
 *                gain_code_old[0..4*DTX_HANGOVER-1]
 *                              fixed codebook gain parameter history
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Averaged fixed codebook gain value
 *
 *************************************************************************/

Word16 EFR_aver_gain_code_history (
    Word16 efr_CN_excitation_gain,
    Word16 gain_code_old[4 * EFR_DTX_HANGOVER]
)
{
    Word32 L_ret;

    L_ret = L_mult (0x470, efr_CN_excitation_gain);
    L_ret = CODEC_OpVcMac(gain_code_old, 4 * EFR_DTX_HANGOVER, 0x470, L_ret);
    return extract_h (L_ret);
}

/*************************************************************************
 *
 *   FUNCTION NAME: build_CN_code
 *
 *   PURPOSE: Compute the comfort noise fixed codebook excitation. The
 *            gains of the pulses are always +/-1.
 *
 *   INPUTS:      *seed         Old CN generator shift register state
 *
 *   OUTPUTS:     cod[0..39]    Generated comfort noise fixed codebook vector
 *                *seed         Updated CN generator shift register state
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_build_CN_code (
    Word16 cod[],
    Word32 *seed
)
{
    Word16 i, j, k;

    CODEC_OpVecSet(cod, EFR_L_SUBFR, 0);
    for (k = 0; k < NB_PULSE; k++)
    {
        i = EFR_pseudonoise (seed, 2);      /* generate pulse position */
        i = shr (extract_l (L_mult (i, 10)), 1);
        i = add (i, k);

        j = EFR_pseudonoise (seed, 1);      /* generate sign           */


        if (j > 0)
        {
            cod[i] = 4096;
        }
        else
        {
            cod[i] = -4096;
        }
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: pseudonoise
 *
 *   PURPOSE: Generate a random integer value to use in comfort noise
 *            generation. The algorithm uses polynomial x^31 + x^3 + 1
 *            (length of PN sequence is 2^31 - 1).
 *
 *   INPUTS:      *shift_reg    Old CN generator shift register state
 *
 *
 *   OUTPUTS:     *shift_reg    Updated CN generator shift register state
 *
 *   RETURN VALUE: Generated random integer value
 *
 *************************************************************************/

Word16 EFR_pseudonoise (
    Word32 *shift_reg,
    Word16 no_bits
)
{
    Word16 noise_bits, Sn, i;

    noise_bits = 0;
    for (i = 0; i < no_bits; i++)
    {
        /* State n == 31 */

        if ((*shift_reg & 0x00000001L) != 0)
        {
            Sn = 1;
        }
        else
        {
            Sn = 0;
        }

        /* State n == 3 */

        if ((*shift_reg & 0x10000000L) != 0)
        {
            Sn = Sn ^ 1;
        }
        else
        {
            Sn = Sn ^ 0;
        }

        noise_bits = shl (noise_bits, 1);
        noise_bits = noise_bits | (extract_l (*shift_reg) & 1);


        *shift_reg = L_shr (*shift_reg, 1);

        if (Sn & 1)
        {
            *shift_reg = *shift_reg | 0x40000000L;
        }
    }

    return noise_bits;
}

/*************************************************************************
 *
 *   FUNCTION NAME: interpolate_CN_param
 *
 *   PURPOSE: Interpolate a comfort noise parameter value over the comfort
 *            noise update period.
 *
 *   INPUTS:      old_param     The older parameter of the interpolation
 *                              (the endpoint the interpolation is started
 *                              from)
 *                new_param     The newer parameter of the interpolation
 *                              (the endpoint the interpolation is ended to)
 *                rx_dtx_state  State of the comfort noise insertion period
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Interpolated CN parameter value
 *
 *************************************************************************/

Word16 EFR_interpolate_CN_param (
    Word16 old_param,
    Word16 new_param,
    Word16 rx_dtx_state
)
{
    static const Word16 interp_factor[EFR_CN_INT_PERIOD] =
    {
        0x0555, 0x0aaa, 0x1000, 0x1555, 0x1aaa, 0x2000,
        0x2555, 0x2aaa, 0x3000, 0x3555, 0x3aaa, 0x4000,
        0x4555, 0x4aaa, 0x5000, 0x5555, 0x5aaa, 0x6000,
        0x6555, 0x6aaa, 0x7000, 0x7555, 0x7aaa, 0x7fff};
    Word16 temp;
    Word32 L_temp;

    L_temp = L_mult (interp_factor[rx_dtx_state], new_param);
    temp = sub (0x7fff, interp_factor[rx_dtx_state]);
    temp = add (temp, 1);
    L_temp = L_mac (L_temp, temp, old_param);
    temp = round (L_temp);

    return temp;
}

/*************************************************************************
 *
 *   FUNCTION NAME:  interpolate_CN_lsf
 *
 *   PURPOSE: Interpolate comfort noise LSF parameter vector over the comfort
 *            noise update period.
 *
 *   INPUTS:      lsf_old_CN[0..9]
 *                              The older LSF parameter vector of the
 *                              interpolation (the endpoint the interpolation
 *                              is started from)
 *                lsf_new_CN[0..9]
 *                              The newer LSF parameter vector of the
 *                              interpolation (the endpoint the interpolation
 *                              is ended to)
 *                rx_dtx_state  State of the comfort noise insertion period
 *
 *   OUTPUTS:     lsf_interp_CN[0..9]
 *                              Interpolated LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void EFR_interpolate_CN_lsf (
    Word16 lsf_old_CN[EFR_M],
    Word16 lsf_new_CN[EFR_M],
    Word16 lsf_interp_CN[EFR_M],
    Word16 rx_dtx_state
)
{
    Word16 i;

    for (i = 0; i < EFR_M; i++)
    {
        lsf_interp_CN[i] = EFR_interpolate_CN_param (lsf_old_CN[i],
                                                 lsf_new_CN[i],
                                                 rx_dtx_state);
    }

    return;
}
