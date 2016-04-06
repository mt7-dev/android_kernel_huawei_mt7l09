/***************************************************************************
 *
 *   File Name: dtx.h
 *
 *   Purpose:   Contains the prototypes for all the functions of DTX.
 *              Also contains definitions of constants used in DTX functions.
 *
 **************************************************************************/

#define EFR_PN_INITIAL_SEED 0x70816958L   /* Pseudo noise generator seed value  */

#define EFR_CN_INT_PERIOD 24              /* Comfort noise interpolation period
                                         (nbr of frames between successive
                                         SID updates in the decoder) */

#define EFR_DTX_HANGOVER 7                /* Period when SP=1 although VAD=0.
                                         Used for comfort noise averaging */


/* Frame classification constants */

#define EFR_VALID_SID_FRAME          1
#define EFR_INVALID_SID_FRAME        2
#define EFR_GOOD_SPEECH_FRAME        3
#define EFR_UNUSABLE_FRAME           4

/* Encoder DTX control flags */

#define EFR_TX_SP_FLAG               0x0001
#define EFR_TX_VAD_FLAG              0x0002
#define EFR_TX_HANGOVER_ACTIVE       0x0004
#define EFR_TX_PREV_HANGOVER_ACTIVE  0x0008
#define EFR_TX_SID_UPDATE            0x0010
#define EFR_TX_USE_OLD_SID           0x0020

/* Decoder DTX control flags */

#define EFR_RX_SP_FLAG               0x0001
#define EFR_RX_UPD_SID_QUANT_MEM     0x0002
#define EFR_RX_FIRST_SID_UPDATE      0x0004
#define EFR_RX_CONT_SID_UPDATE       0x0008
#define EFR_RX_LOST_SID_FRAME        0x0010
#define EFR_RX_INVALID_SID_FRAME     0x0020
#define EFR_RX_NO_TRANSMISSION       0x0040
#define EFR_RX_DTX_MUTING            0x0080
#define EFR_RX_PREV_DTX_MUTING       0x0100
#define EFR_RX_CNI_BFI               0x0200
#define EFR_RX_FIRST_SP_FLAG         0x0400

void EFR_reset_tx_dtx (void);       /* Reset tx dtx variables */
void EFR_reset_rx_dtx (void);       /* Reset rx dtx variables */

void EFR_tx_dtx (
    Word16 VAD_flag,
    Word16 *txdtx_ctrl
);

void EFR_rx_dtx (
    Word16 *rxdtx_ctrl,
    Word16 TAF,
    Word16 bfi,
    Word16 SID_flag
);

void EFR_CN_encoding (
    Word16 params[],
    Word16 txdtx_ctrl
);

void EFR_sid_codeword_encoding (
    Word16 ser2[]
);

Word16 EFR_sid_frame_detection (
    Word16 ser2[]
);

void EFR_update_lsf_history (
    Word16 lsf1[EFR_M],
    Word16 lsf2[EFR_M],
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M]
);

void EFR_update_lsf_p_CN (
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M],
    Word16 lsf_p_CN[EFR_M]
);

void EFR_aver_lsf_history (
    Word16 lsf_old[EFR_DTX_HANGOVER][EFR_M],
    Word16 lsf1[EFR_M],
    Word16 lsf2[EFR_M],
    Word16 lsf_aver[EFR_M]
);

void EFR_update_gain_code_history_tx (
    Word16 new_gain_code,
    Word16 efr_gain_code_old_tx[4 * EFR_DTX_HANGOVER]
);

void EFR_update_gain_code_history_rx (
    Word16 new_gain_code,
    Word16 efr_gain_code_old_rx[4 * EFR_DTX_HANGOVER]
);

Word16 EFR_compute_CN_excitation_gain (
    Word16 res2[EFR_L_SUBFR]
);

Word16 EFR_update_gcode0_CN (
    Word16 efr_gain_code_old_tx[4 * EFR_DTX_HANGOVER]
);

Word16 EFR_aver_gain_code_history (
    Word16 efr_CN_excitation_gain,
    Word16 gain_code_old[4 * EFR_DTX_HANGOVER]
);

void EFR_build_CN_code (
    Word16 cod[],
    Word32 *seed
);

Word16 EFR_pseudonoise (
    Word32 *shift_reg,
    Word16 no_bits
);

Word16 EFR_interpolate_CN_param (
    Word16 old_param,
    Word16 new_param,
    Word16 rx_dtx_state
);

void EFR_interpolate_CN_lsf (
    Word16 lsf_old_CN[EFR_M],
    Word16 lsf_new_CN[EFR_M],
    Word16 lsf_interp_CN[EFR_M],
    Word16 rx_dtx_state
);
