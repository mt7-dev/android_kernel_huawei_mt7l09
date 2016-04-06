#include "codec_op_vec.h"
#include "codec_op_etsi.h"

void EFR_Init_Coder_12k2 (void);

void EFR_Coder_12k2 (
    Word16 ana[],      /* output  : Analysis parameters */
    Word16 synth[]     /* output  : Local synthesis     */
);

void EFR_Init_Decoder_12k2 (void);

void EFR_Decoder_12k2 (
    Word16 parm[],     /* input : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi) */
    Word16 synth[],    /* output: synthesis speech                    */
    Word16 A_t[],      /* output: decoded LP filter in 4 subframes    */
    Word16 TAF,
    Word16 SID_flag
);

void EFR_Init_Post_Filter (void);

void EFR_Post_Filter (
    Word16 *syn,       /* in/out: synthesis speech (postfiltered is output) */
    Word16 *Az_4       /* input : interpolated LPC parameters in all subfr. */
);

void EFR_Code_10i40_35bits (
    Word16 x[],        /* (i)   : target vector                             */
    Word16 cn[],       /* (i)   : residual after long term prediction       */
    Word16 h[],        /* (i)   : impulse response of weighted synthesis
                                  filter                                    */
    Word16 cod[],      /* (o)   : algebraic (fixed) codebook excitation     */
    Word16 y[],        /* (o)   : filtered fixed codebook excitation        */
    Word16 indx[]      /* (o)   : index of 10 pulses (sign + position)      */
);
void EFR_Dec_10i40_35bits (
    Word16 index[],    /* (i)   : index of 10 pulses (sign+position)        */
    Word16 cod[]       /* (o)   : algebraic (fixed) codebook excitation     */
);
Word16 EFR_Dec_lag6 (      /* output: return integer pitch lag                  */
    Word16 index,      /* input : received pitch index                      */
    Word16 pit_min,    /* input : minimum pitch lag                         */
    Word16 pit_max,    /* input : maximum pitch lag                         */
    Word16 i_subfr,    /* input : subframe flag                             */
    Word16 L_frame_by2,/* input : speech frame size divided by 2            */
    Word16 *T0_frac,   /* output: fractional part of pitch lag              */
    Word16 bfi         /* input : bad frame indicator                       */
);
Word16 EFR_d_gain_pitch (  /* out      : quantized pitch gain                   */
    Word16 index,      /* in       : index of quantization                  */
    Word16 bfi,        /* in       : bad frame indicator (good = 0)         */
    Word16 state,      /* in       : state of the state machine             */
    Word16 prev_bf,    /* Previous bf                                       */
    Word16 rxdtx_ctrl

);
void EFR_d_gain_code (
    Word16 index,      /* input : received quantization index               */
    Word16 code[],     /* input : innovation codevector                     */
    Word16 lcode,      /* input : codevector length                         */
    Word16 *gain_code, /* output: decoded innovation gain                   */
    Word16 bfi,        /* input : bad frame indicator                       */
    Word16 state,      /* in    : state of the state machine                */
    Word16 prev_bf,    /* Previous bf                                       */
    Word16 rxdtx_ctrl,
    Word16 i_subfr,
    Word16 rx_dtx_state

);
void EFR_D_plsf_5 (
    Word16 *indice,    /* input : quantization indices of 5 submatrices     */
    Word16 *lsp1_q,    /* output: quantized 1st LSP vector                  */
    Word16 *lsp2_q,    /* output: quantized 2nd LSP vector                  */
    Word16 bfi,        /* input : bad frame indicator (set to 1 if a bad
                                  frame is received)                        */
    Word16 rxdtx_ctrl,
    Word16 rx_dtx_state
);
Word16 EFR_Enc_lag6 (      /* output: Return index of encoding                  */
    Word16 T0,         /* input : Pitch delay                               */
    Word16 *T0_frac,   /* in/out: Fractional pitch delay                    */
    Word16 *T0_min,    /* in/out: Minimum search delay                      */
    Word16 *T0_max,    /* in/out: Maximum search delay                      */
    Word16 pit_min,    /* input : Minimum pitch delay                       */
    Word16 pit_max,    /* input : Maximum pitch delay                       */
    Word16 pit_flag    /* input : Flag for 1st or 3rd subframe              */
);

Word16 EFR_q_gain_pitch (  /* Return index of quantization                      */
    Word16 *gain       /* (i)    :  Pitch gain to quantize                  */
);

Word16 EFR_q_gain_code (   /* Return quantization index                         */
    Word16 code[],     /* (i)      : fixed codebook excitation              */
    Word16 lcode,      /* (i)      : codevector size                        */
    Word16 *gain,      /* (i/o)    : quantized fixed codebook gain          */
    Word16 txdtx_ctrl,
    Word16 i_subfr
);

Word16 EFR_G_pitch (       /* (o)     : Gain of pitch lag saturated to 1.2      */
    Word16 xn[],       /* (i)     : Pitch target.                           */
    Word16 y1[],       /* (i)     : Filtered adaptive codebook.             */
    Word16 L_subfr     /*         : Length of subframe.                     */
);
Word16 EFR_G_code (        /* out      : Gain of innovation code.               */
    Word16 xn[],       /* in       : target vector                          */
    Word16 y2[]        /* in       : filtered inovation vector              */
);

Word16 EFR_Interpol_6 (    /* (o)  : interpolated value                         */
    Word16 *x,         /* (i)  : input vector                               */
    Word16 frac        /* (i)  : fraction                                   */
);
void EFR_Int_lpc (
    Word16 lsp_old[],  /* input: LSP vector at the 4th subfr. of past frame */
    Word16 lsp_mid[],  /* input: LSP vector at the 2nd subfr. of
                          present frame                                     */
    Word16 lsp_new[],  /* input: LSP vector at the 4th subfr. of
                          present frame                                     */
    Word16 Az[]        /* output: interpolated LP parameters in all subfr.  */
);
void EFR_Int_lpc2 (
    Word16 lsp_old[],  /* input: LSP vector at the 4th subfr. of past frame */
    Word16 lsp_mid[],  /* input: LSP vector at the 2nd subframe of
                          present frame                                     */
    Word16 lsp_new[],  /* input: LSP vector at the 4th subframe of
                          present frame                                     */
    Word16 Az[]        /* output:interpolated LP parameters
                          in subframes 1 and 3                              */
);
Word16 EFR_Pitch_fr6 (     /* (o)     : pitch period.                           */
    Word16 exc[],      /* (i)     : excitation buffer                       */
    Word16 xn[],       /* (i)     : target vector                           */
    Word16 h[],        /* (i)     : impulse response of synthesis and
                                    weighting filters                       */
    Word16 L_subfr,    /* (i)     : Length of subframe                      */
    Word16 t0_min,     /* (i)     : minimum value in the searched range.    */
    Word16 t0_max,     /* (i)     : maximum value in the searched range.    */
    Word16 i_subfr,    /* (i)     : indicator for first subframe.           */
    Word16 *pit_frac   /* (o)     : chosen fraction.                        */
);
Word16 EFR_Pitch_ol (      /* output: open loop pitch lag                       */
    Word16 signal[],   /* input: signal used to compute the open loop pitch */
                       /* signal[-pit_max] to signal[-1] should be known    */
    Word16 pit_min,    /* input : minimum pitch lag                         */
    Word16 pit_max,    /* input : maximum pitch lag                         */
    Word16 L_frame     /* input : length of frame to compute pitch          */
);
void EFR_Pred_lt_6 (
    Word16 exc[],      /* in/out: excitation buffer                         */
    Word16 T0,         /* input : integer pitch lag                         */
    Word16 frac,       /* input : fraction of lag                           */
    Word16 L_subfr     /* input : subframe size                             */
);
void EFR_Q_plsf_5 (
    Word16 *lsp1,      /* input : 1st LSP vector                            */
    Word16 *lsp2,      /* input : 2nd LSP vector                            */
    Word16 *lsp1_q,    /* output: quantized 1st LSP vector                  */
    Word16 *lsp2_q,    /* output: quantized 2nd LSP vector                  */
    Word16 *indice,    /* output: quantization indices of 5 matrices        */
    Word16 txdtx_ctrl  /* input : tx dtx control word                       */
);
void EFR_Bits2prm_12k2 (
    Word16 bits[],     /* input : serial bits                               */
    Word16 prm[]       /* output: analysis parameters                       */
);
void EFR_Prm2bits_12k2 (
    Word16 prm[],      /* input : analysis parameters                       */
    Word16 bits[]      /* output: serial bits                               */
);
