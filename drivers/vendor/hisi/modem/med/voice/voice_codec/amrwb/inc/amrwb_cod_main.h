/*--------------------------------------------------------------------------*
 *                         amrwb_cod_main.h                                       *
 *--------------------------------------------------------------------------*
 *       Static memory in the encoder				                        *
 *--------------------------------------------------------------------------*/

#include "amrwb_cnst.h"                          /* coder constant parameters */
#include "amrwb_wb_vad.h"
#include "amrwb_dtx.h"

typedef struct
{
    Word16 mem_decim[2 * L_FILT16k];       /* speech decimated filter memory */
    Word16 mem_sig_in[6];                  /* hp50 filter memory */
    Word16 mem_preemph;                    /* speech preemph filter memory */
    Word16 old_speech[AMRWB_L_TOTAL - AMRWB_L_FRAME];  /* old speech vector at 12.8kHz */
    Word16 old_wsp[AMRWB_PIT_MAX / OPL_DECIM];   /* old decimated weighted speech vector */
    Word16 old_exc[AMRWB_PIT_MAX + AMRWB_L_INTERPOL];  /* old excitation vector */
    Word16 mem_levinson[AMRWB_M + 2];            /* levinson routine memory */
    Word16 ispold[AMRWB_M];                      /* old isp (immittance spectral pairs) */
    Word16 ispold_q[AMRWB_M];                    /* quantized old isp */
    Word16 past_isfq[AMRWB_M];                   /* past isf quantizer */
    Word16 mem_wsp;                        /* wsp vector memory */
    Word16 mem_decim2[3];                  /* wsp decimation filter memory */
    Word16 mem_w0;                         /* target vector memory */
    Word16 mem_syn[AMRWB_M];                     /* synthesis memory */
    Word16 tilt_code;                      /* tilt of code */
    Word16 old_wsp_max;                    /* old wsp maximum value */
    Word16 old_wsp_shift;                  /* old wsp shift */
    Word16 Q_old;                          /* old scaling factor */
    Word16 Q_max[2];                       /* old maximum scaling factor */
    Word16 gp_clip[2];                     /* gain of pitch clipping memory */
    Word16 qua_gain[4];                    /* gain quantizer memory */

    Word16 old_T0_med;
    Word16 ol_gain;
    Word16 ada_w;
    Word16 ol_wght_flg;
    Word16 old_ol_lag[5];
    Word16 hp_wsp_mem[9];
    Word16 old_hp_wsp[(AMRWB_L_FRAME / OPL_DECIM) + (AMRWB_PIT_MAX / OPL_DECIM)];
    VadVars *vadSt;
    AMRWB_dtx_encState *dtx_encSt;
    Word16 first_frame;

    Word16 isfold[AMRWB_M];                      /* old isf (frequency domain) */
    Word32 L_gc_thres;                     /* threshold for noise enhancer */
    Word16 mem_syn_hi[AMRWB_M];                  /* modified synthesis memory (MSB) */
    Word16 mem_syn_lo[AMRWB_M];                  /* modified synthesis memory (LSB) */
    Word16 mem_deemph;                     /* speech deemph filter memory */
    Word16 mem_sig_out[6];                 /* hp50 filter memory for synthesis */
    Word16 mem_hp400[6];                   /* hp400 filter memory for synthesis */
    Word16 mem_oversamp[2 * L_FILT];       /* synthesis oversampled filter memory */
    Word16 mem_syn_hf[AMRWB_M];                  /* HF synthesis memory */
    Word16 mem_hf[2 * L_FILT16k];          /* HF band-pass filter memory */
    Word16 mem_hf2[2 * L_FILT16k];         /* HF band-pass filter memory */
    Word16 seed2;                          /* random memory for HF generation */
    Word16 vad_hist;

    Word16 gain_alpha;

    Word16 *pfCallBack;                    /* Frame type call-back function */

} Coder_State;
