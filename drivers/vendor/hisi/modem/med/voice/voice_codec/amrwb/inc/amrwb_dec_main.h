/*--------------------------------------------------------------------------*
 *                         amrwb_dec_main.h                                       *
 *--------------------------------------------------------------------------*
 *       Static memory in the decoder                                       *
 *--------------------------------------------------------------------------*/

#include "amrwb_cnst.h"                          /* coder constant parameters */
#include "amrwb_dtx.h"

/*****************************************************************************
 实体名称  : AMRWB_SID_RESET_FLG_ENUM
 功能描述  : AMRWB SID帧是否已经重置标记枚举
*****************************************************************************/
enum AMRWB_SID_RESET_FLG_ENUM
{
    AMRWB_SID_RESET_FLG_NO   = 0,
    AMRWB_SID_RESET_FLG_YES,
    AMRWB_SID_RESET_FLG_BUTT
};

typedef struct
{
    Word16 old_exc[AMRWB_PIT_MAX + AMRWB_L_INTERPOL];  /* old excitation vector */
    Word16 ispold[AMRWB_M];                      /* old isp (immittance spectral pairs) */
    Word16 isfold[AMRWB_M];                      /* old isf (frequency domain) */
    Word16 isf_buf[L_MEANBUF * AMRWB_M];         /* isf buffer(frequency domain) */
    Word16 past_isfq[AMRWB_M];                   /* past isf quantizer */
    Word16 tilt_code;                      /* tilt of code */
    Word16 Q_old;                          /* old scaling factor */
    Word16 Qsubfr[4];                      /* old maximum scaling factor */
    Word32 L_gc_thres;                     /* threshold for noise enhancer */
    Word16 mem_syn_hi[AMRWB_M];                  /* modified synthesis memory (MSB) */
    Word16 mem_syn_lo[AMRWB_M];                  /* modified synthesis memory (LSB) */
    Word16 mem_deemph;                     /* speech deemph filter memory */
    Word16 mem_sig_out[6];                 /* hp50 filter memory for synthesis */
    Word16 mem_oversamp[2 * L_FILT];       /* synthesis oversampled filter memory */
    Word16 mem_syn_hf[M16k];               /* HF synthesis memory */
    Word16 mem_hf[2 * L_FILT16k];          /* HF band-pass filter memory */
    Word16 mem_hf2[2 * L_FILT16k];         /* HF band-pass filter memory */
    Word16 mem_hf3[2 * L_FILT16k];         /* HF band-pass filter memory */
    Word16 seed;                           /* random memory for frame erasure */
    Word16 seed2;                          /* random memory for HF generation */
    Word16 old_T0;                         /* old pitch lag */
    Word16 old_T0_frac;                    /* old pitch fraction lag */
    Word16 lag_hist[5];
    Word16 dec_gain[23];                   /* gain decoder memory */
    Word16 seed3;                          /* random memory for lag concealment */
    Word16 disp_mem[8];                    /* phase dispersion memory */
    Word16 mem_hp400[6];                   /* hp400 filter memory for synthesis */

    Word16 prev_bfi;
    Word16 state;
    Word16 first_frame;
    AMRWB_dtx_decState *dtx_decSt;
    Word16 vad_hist;
    Word16 shwOldResetFlag;
    UWord16 prev_mode;
    Word16 shwRxSidResetFlg;               /* 声码器重启后，SID标记是否重置位。收到的第一个SID_FIRST或者SID_UPDATE帧需要修改为SID_BAD帧 */
} Decoder_State;
