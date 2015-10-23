/* Copyright (c) 2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc. and its licensors, and are licensed to the recipient
 * under the terms of a separate license agreement.  They may be
 * adapted and modified by bona fide purchasers under the terms of the
 * separate license agreement for internal use, but no adapted or
 * modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the
 * prior written consent of Tensilica Inc.
 */

#ifndef __XA_DM3PLUS_AK_PP_API_H__
#define __XA_DM3PLUS_AK_PP_API_H__

/*****************************************************************************/
/* DMS3 Postprocessing specific API definitions                              */
/*****************************************************************************/

/* DMS3 PP-specific configuration parameters */
enum xa_config_param_dm3plus_ak_pp {

    /* TDAS Bundle */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_IN_MATRIX         = 0,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_IN_CHAN_FORMAT,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_BYPASS,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_DOWNMIX_CONFIG,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_LFE_MIX_LEVEL,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_MSR_MAX_PROFILE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_INP_SAMP_FREQ,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_OUT_SAMP_FREQ,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_INP_NUM_CHANS,

    /* Resampler */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_RESAMP_MAX_OUT_SAMP_FREQ,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_RESAMP_MIN_OUT_SAMP_FREQ,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_RESAMP_UPSAMPLE_RATE,

    /* Two Up Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_M2S_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_M2S_DETECTOR,

    /* StereoWrapper Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_WIDTH,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_SPK_MODE,

    /* MobileUpmixer Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MUP_ENABLE,

    /* MobileSurround Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ROOM,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_BRIGHT,

    /* LateNight Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_LEVEL,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_DEPTH,

    /* Volume Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_GAIN_TYPE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_GAIN_INTERNAL,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_GAIN_EXTERNAL,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_BALANCE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_VOL_MUTE,

    /* Rumbler Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_CUTOFF,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_BOOST,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_LEVEL,

    /* Sparkler Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_DEPTH,

    /* Fade Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_FADE_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_FADE_TARGET,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_FADE_TIME,

    /* Graphics Equalizer Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_MAXBOOST,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_PREAMP,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_NBANDS,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND2,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND3,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND4,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND5,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND6,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND7,

    /* Speaker Equalizer Module */
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_ENABLE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB1,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB2,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_SET_COEFS_24K,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_SET_COEFS_32K,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_SET_COEFS_44K,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_SET_COEFS_48K,

    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_INPUT_BUF_PTR,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_OUTPUT_BUF_PTR,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_NUM_SAMPLES,

    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_GAIN,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_ORL,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PULSEDOWNMIX,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PULSEDOWNMIX_ATTEN,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_LIMITER_TESTMODE,
    XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_OUT_CHANS

};

/* commands */
#include "xa_apicmd_standards.h"

/* dm3plus_ak_pp-specific commands */
/* (none) */

/* dm3plus_ak_pp-specific command types */
/* (none) */

/* error codes */
#include "xa_error_standards.h"

/* TOFO_DMS3 - Must change following number. */
#define XA_CODEC_DM3PLUS_AK_PP	20

/* dm3plus_ak_pp-specific error codes */
/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Fatal Errors */
/* none */

/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_config_dm3plus_ak_pp {
  XA_DM3PLUS_AK_PP_CONFIG_NONFATAL_CMD_NOT_SUPPORTED = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_DM3PLUS_AK_PP, 0),
  XA_DM3PLUS_AK_PP_CONFIG_NONFATAL_INVALID_PARAMETER = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_DM3PLUS_AK_PP, 1),
  XA_DM3PLUS_AK_PP_CONFIG_NONFATAL_INVALID_INP_SAMP_FREQ = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_config, XA_CODEC_DM3PLUS_AK_PP, 2),
};

/* Fatal Errors */
/* none */

/*****************************************************************************/
/* Class 2: Execution Errors                                                 */
/*****************************************************************************/
/* Nonfatal Errors */
enum xa_error_nonfatal_execute_dm3plus_ak_pp {
	XA_DM3PLUS_AK_PP_EXECUTE_CMD_NOT_SUPPORTED = XA_ERROR_CODE(xa_severity_nonfatal, xa_class_execute, XA_CODEC_DM3PLUS_AK_PP, 0),
};

/* Fatal Errors */
enum xa_error_fatal_execute_dm3plus_ak_pp {
	XA_DM3PLUS_AK_PP_EXECUTE_IO_BUF_PTR_ERROR = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_CODEC_DM3PLUS_AK_PP, 0),
};
/* none */

#include "xa_type_def.h"

#if defined(__cplusplus)
extern "C" {
#endif  /* __cplusplus */
xa_codec_func_t xa_dm3plus_ak_pp;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */


/************************ DMS3 PP Specific Defines ************************/

/* Note - the ordering of the channel configurations in this
 * enum has been deliberately kept consistent with those in
 * Dolby Digital and Dolby Digital Plus.
 */
#define CHANS_RESERVED                 0
#define CHANS_C	                       1
#define CHANS_L_R                      2
#define CHANS_L_C_R                    3
#define CHANS_L_R_S                    4
#define CHANS_L_C_R_S                  5
#define CHANS_L_R_Ls_Rs                6
#define CHANS_L_C_R_Ls_Rs              7
#define CHANS_L_R_Ls_Rs_Cs             8
#define CHANS_L_R_Ls_Rs_Lb_Rb          9
#define CHANS_L_C_R_Ls_Rs_Cs           10
#define CHANS_L_R_Ls_Rs_Ts_Cs          11
#define CHANS_L_C_R_Ls_Rs_Cvh_Cs       12
#define CHANS_L_C_R_Ls_Rs_Ts_Cs        13
#define CHANS_L_C_R_Ls_Rs_Lb_Rb        14
#define CHANS_RESERVED_2               15
#define CHANS_C_LFE                    16
#define CHANS_L_R_LFE                  17
#define CHANS_L_C_R_LFE                18
#define CHANS_L_R_S_LFE                19
#define CHANS_L_C_R_S_LFE              20
#define CHANS_L_R_Ls_Rs_LFE            21
#define CHANS_L_C_R_Ls_Rs_LFE          22
#define CHANS_L_R_Ls_Rs_Cs_LFE         23
#define CHANS_L_R_Ls_Rs_Lb_Rb_LFE      24
#define CHANS_L_C_R_Ls_Rs_Cs_LFE       25
#define CHANS_L_R_Ls_Rs_Ts_Cs_LFE      26
#define CHANS_L_C_R_Ls_Rs_Cvh_Cs_LFE   27
#define CHANS_L_C_R_Ls_Rs_Ts_Cs_LFE    28
#define CHANS_L_C_R_Ls_Rs_Lb_Rb_LFE    29
#define TDAS_N_IN_CHANS                30

#define TDAS_MAX_IN_CHANS              8  /* Maximum is 7.1 channel input */

/*
 * TDAS_BUNDLE_IN_MATRIX - Determines, for 2-channel input (TDAS_BUNDLE_INP_NUM_CHANS = CHANS_L_R),
 *                         how they are mixed to form the 2-channel signal processed by PP library.
 *      0: stereo - left and right input channels are treated normally.
 *                  i.e. PP library processes L, R
 *      1: channel one mono - right channel is replaced by left input channel internally.
 *                  i.e. PP library processes L, L
 *      2: channel two mono - left channel is replaced by the right input channel internally.
 *                  i.e. PP library processes R, R
 *      3: dual mono - both channels are summed and replicated to both channels internally.
 *                  i.e. PP library processes (L+R), (L+R)
 *      4: Dolby Pro-Logic II Encoded stereo content- left and right channels are treated as
 *                  Pro-Logic II encoded. This is only applicable for use with mobile surround.
 */
#define MATRIX_STEREO                  0
#define MATRIX_LEFT                    1
#define MATRIX_RIGHT                   2
#define MATRIX_DUALMONO	               3
#define MATRIX_PL2                     4

#define TDAS_N_IN_MATRICES             5

/*
 * TDAS_BUNDLE_IN_CHAN_FORMAT - 0: interlaced channels
 *                              1: de-interlaced channels
 */
#define CHAN_FORMAT_INTERLACED         0
#define CHAN_FORMAT_DEINTERLACED       1
#define TDAS_N_CHAN_FORMATS            2

/* Identifiers for the valid internal TDAS channel configurations.
 * They are also used to specify the TDAS_DOWNMIX_CONFIG bundle
 * parameter.
 *
 * TDAS_DOWNMIX_CONFIG - 0: downmix to shuffled L/R channels (2 channels)
 *                       1: downmix to L, R channels (2 channels)
 *                       2: downmix to shuffled L/R, shuffled Ls/Rs channels (4 channels)
 *                       3: downmix to L, R, Ls, Rs channels (4 channels)
 *                       4: downmix to shuffled L/R, shuffled Ls/Rs, unshuffled C (5 channels)
 *                       5: downmix to L, C, R, Ls, Rs (5 channels)
 *                       6: downmix to shuffled L/R, shuffled Ls/Rs, unshuffled C, shuffled Lb/Rb (7 channels)
 *                       7: downmix to L, C, R, Ls, Rs, Lb, Rb (7 channels)
 */
#define INTERNAL_CHANS_SHUFFLED_L_R                  0
#define INTERNAL_CHANS_UNSHUFFLED_L_R                1
#define INTERNAL_CHANS_SHUFFLED_L_R_Ls_Rs            2
#define INTERNAL_CHANS_UNSHUFFLED_L_R_Ls_Rs          3
#define INTERNAL_CHANS_SHUFFLED_L_C_R_Ls_Rs          4
#define INTERNAL_CHANS_UNSHUFFLED_L_C_R_Ls_Rs        5
#define INTERNAL_CHANS_SHUFFLED_L_C_R_Ls_Rs_Lb_Rb    6
#define INTERNAL_CHANS_UNSHUFFLED_L_C_R_Ls_Rs_Lb_Rb  7
#define TDAS_N_INTERNAL_CHAN_CONFIGS                 8

/*
 * Downmixer LFE limit mixing values
 * Min mix level is -21 dB
 * Max mix level is +10 dB
 * Actual mix level is calculated as (10 - DMX_LFE_MIX_LEVEL) dB
 * Note: there's a systematic 4.5dB attenuation
 */
#define DMX_LFE_MIX_MIN             31
#define DMX_LFE_MIX_MAX             0
#define DMX_LFE_MIX_DEFAULT         10      /* Default LFE downmix level */


/* TDAS_BUNDLE_BYPASS -
 * 0 : bypass defeated (no bypass - normal operation)
 * 1 : soft bypass on  (all features bypassed except resampler and downmixer)
 * 2 : hard bypass on  (all features bypassed including resampler and downmixer)
 */
#define TDAS_N_BYPASSES         3
#define TDAS_DEF_BYPASS         0   /* Default bypass off */

/*
 * Mobile Surround Maximum Profile settings
 */
#define TDAS_MSR_MAX_PROFILE_STEREO  0
#define TDAS_MSR_MAX_PROFILE_FOUR    1
#define TDAS_MSR_MAX_PROFILE_FIVE    2
#define TDAS_MSR_MAX_PROFILE_SEVEN   3
#define TDAS_MSR_N_MAX_PROFILES      4
#define TDAS_MSR_DEF_MAX_PROFILE     TDAS_MSR_MAX_PROFILE_FIVE

/*---------------------------------------------------------------------------*/
/* TwoUp */
/* M2S_ENABLE - 0 is feature off, 1 is feature on */
#define M2S_N_ENABLES       2

/*---------------------------------------------------------------------------*/
/* StereoWrapper */
/* SSE_ENABLE - 0 is feature off, 1 is feature on */
#define SSE_N_ENABLES       2

/* SSE_WIDTH - 0 is half width, 1 is full width */
#define SSE_N_WIDTHS        2

/* SSE_SPK_MODE - 0 is narrow, 1 is normal and 2 is binarual mode */
#define SSE_N_SPK_MODES 3

/*---------------------------------------------------------------------------*/
/* LateNight */
/* SLC_ENABLE - 0 is feature off, 1 is feature on */
#define SLC_N_ENABLES       2

/* SLC_LEVEL - discrete input audio levels, 0-6, 0 is really loud music,
 *             6 is quiet film */
#define SLC_N_LEVELS        7

/* SLC_DEPTH - depth of effect, 0 is light, 1 is medium, 2 is comprehensive */
#define SLC_N_DEPTHS        3

/*---------------------------------------------------------------------------*/
/* Volume */
/* VOL_ENABLE - 0 is feature off, 1 is feature on */
#define VOL_N_ENABLES       2

/* Volume is defined as 2^(VOL_BITS_IN_GAIN-1) discrete steps. */
#define VOL_BITS_IN_GAIN        8
#define VOL_N_GAINS_TDAS        (1<<(VOL_BITS_IN_GAIN))
#define VOL_N_GAINS_EXTERNAL    (1<<(VOL_BITS_IN_GAIN))

/* VOL_GAIN_TYPE - 0 is linear gain, 1 is log gain */
#define VOL_N_GAIN_TYPES        2
#define VOL_LINEAR              0
#define VOL_LOGARITHMIC         1

/* VOL_BALANCE
 * This is specified as a signed integer value from -128 to 127.
 * -128 corresponds with audio fully panned to the left,
 * 127 corresponds with audio fully panned to the right.
 */
#define VOL_BITS_IN_BALANCE (VOL_BITS_IN_GAIN-1)
#define VOL_BALANCE_MIN     (-1<<VOL_BITS_IN_BALANCE)
#define VOL_BALANCE_MAX     ((1<<VOL_BITS_IN_BALANCE)-1)


/* VOL_MUTE - 0 is not muted, 1 is muted */
#define VOL_N_MUTES     2

/*---------------------------------------------------------------------------*/
/* Rumbler */
/* NB_ENABLE - 0 is feature off, 1 is feature on */
#define NB_N_ENABLES        2

/* NB_CUTOFF - discrete cutoff frequencies, as described in the system integration manual */
#define NB_N_CUTOFFS        24

/* NB_BOOST - discrete boost levels, as described in the system integration manual */
#define NB_N_BOOSTS     7

/* NB_LEVEL - defines the output capabilities of your speakers, as described in the system
 * integration manual */
#define NB_N_LEVELS     7

/*---------------------------------------------------------------------------*/
/* Graphics Equalizer */

#define GEQ_STEP_BITS       2
#define GEQ_STEP            (1<<GEQ_STEP_BITS)	  /* 0.25 dB step size */

#define GEQ_MINGAIN         (-12<<GEQ_STEP_BITS)  /* minimum allowed gain in dB */
#define GEQ_MAXGAIN         (12<<GEQ_STEP_BITS)   /* maximum allowed gain in dB */

/* limits are applied to the range of preamp and maxboost */
#define GEQ_MIN_PREAMP      GEQ_MINGAIN
#define GEQ_MAX_PREAMP      0

#define GEQ_MIN_MAXBOOST    0
#define GEQ_MAX_MAXBOOST    GEQ_MAXGAIN

/* Default GEQ settings */
#define GEQ_DEFAULT_PREAMP  0                   /* default to 0dB */
#define GEQ_DEFAULT_MAXBOOST (6<<GEQ_STEP_BITS) /* default to 6dB */

/* GEQ_ENABLE - 0 is feature off, 1 is feature on */
#define GEQ_N_ENABLES       2

/* GEQ_NBANDS */
#define GEQ_N_BANDS_MIN     2
#define GEQ_N_BANDS_MAX     7

/*---------------------------------------------------------------------------*/
/*Mobile Surround Upmixer */

/* Number of MUP enable modes - 0 is feature off, 1 is feature on */
#define MUP_N_ENABLES       2

/*---------------------------------------------------------------------------*/
/* Mobile Surround Feature Parameters */

/* Number of MSR enable modes - 0 is feature off, 1 is feature on */
#define MSR_N_ENABLES       2

/* Number of MSR room modes */
#define MSR_N_ROOMS         3

/* Number of MSR brightness modes */
#define MSR_N_BRIGHTS       3

/* Default MSR room mode */
#define MSR_DEF_ROOM        1

/* Default MSR brightness */
#define MSR_DEF_BRIGHT      1

/*---------------------------------------------------------------------------*/
/* High Frequency Enhancer (Sparkler) */
/* HFE_ENABLE - 0 is feature off, 1 is feature on */
#define HFE_N_ENABLES       2

#define HFE_N_DEPTHS        7

/*---------------------------------------------------------------------------*/
/* Fade */

/* FADE_ENABLE - 0 is feature off, 1 is feature on */
#define FADE_N_ENABLES      2

#define FADE_TIME_LUT_SCALE_SHIFT   5                                               /* 1 << 5 = 32 msec increments */
#define FADE_TIME_LUT_MAX_INDEX     31

#define FADE_TIME_MIN (1 << FADE_TIME_LUT_SCALE_SHIFT)                              /* 32   msec */
#define FADE_TIME_MAX ((FADE_TIME_LUT_MAX_INDEX << FADE_TIME_LUT_SCALE_SHIFT) * 4)  /* 3968 msec */

/*---------------------------------------------------------------------------*/
/* Speaker Equalizer */

#define SPKEQ_STEP_BITS         2   /* 2 bits -> 4 steps -> 0.25 dB per step */
#define SPKEQ_STEP              (1<<SPKEQ_STEP_BITS)    /* 0.25 dB step size */

/* SPKEQ Min/Max settings */
#define SPKEQ_MIN_CHGAIN_DB     (-6<<SPKEQ_STEP_BITS)  /* minimum allowed gain in dB */
#define SPKEQ_MAX_CHGAIN_DB     (6<<SPKEQ_STEP_BITS)   /* maximum allowed gain in dB */

/* Default SPKEQ settings */
#define SPKEQ_DEFAULT_CHGAIN_DB 0   /* defaults to 0 dB for both channels */

/* SPKEQ_ENABLE - 0 is feature off, 1 is feature on */
#define SPKEQ_N_ENABLES         2

/* Maximum allowed number of biquads is 4 */
#define SPKEQ_MAX_N_FILTERS     4

/* Coefs table size, one such table for each sampling frequency */
#define SPKEQ_COEF_TABLE_SZ     (2+2*SPKEQ_MAX_N_FILTERS*(5+1))

typedef struct SPEQ_COEF_INFO_S {
    unsigned int spkeq_coef_size;
    char *spkeq_coef_ptr;
} SPEQ_COEF_INFO;

#define TDAS_PORTABLEMODE_DISABLE      0     /* Default, Portable Mode Normalisation Disabled. */
#define TDAS_PORTABLEMODE_ENABLE       1     /* Portable Mode Normalisation Enabled */
#define TDAS_PORTABLEMODE_GAIN_SCALE   2     /* Right shifts used to represent a dB integer for PORTABLEMODE_GAIN */
#define TDAS_PORTABLEMODE_GAIN_MIN     -44   /* -11dB is the minimum portable mode gain */
#define TDAS_PORTABLEMODE_GAIN_MAX     160   /* 40dB is the maximum portable mode gain */
#define TDAS_PORTABLEMODE_GAIN_DEF     0     /* 0dB is the default portable mode gain */
#define TDAS_PORTABLEMODE_ORL_8DB      0     /* -8dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_11DB     1     /* Default, -11dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_14DB     2     /* -14dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_DEF      1     /* Default, -11dB Portable Mode ORL */
#define TDAS_PULSEDOWNMIX_DISABLE      0     /* Default, account for Pulse Downmixer Attenuation disabled. */
#define TDAS_PULSEDOWNMIX_ENABLE       1     /* Account for Pulse Downmixer Attenuation enabled. */
#define TDAS_PULSEDOWNMIX_ATTEN_SCALE  2     /* Left shifts used to represent a dB integer for PULSEDOWNMIX_ATTEN */
#define TDAS_PULSEDOWNMIX_ATTEN_MAX    -40   /* -10dB is the maximum Pulse Downmixer attenuation */
#define TDAS_PULSEDOWNMIX_ATTEN_MIN    0     /* 0dB is the minimum Pulse Downmixer attenuation */
#define TDAS_PULSEDOWNMIX_ATTEN_DEF    0     /* 0dB is the default Pulse Downmixer attenuation */
#define TDAS_LIMITER_TESTMODE_DISABLE    0     /* DIAGNOSTIC USE. Default (disabled). */
#define TDAS_LIMITER_TESTMODE_ENABLE     1     /* DIAGNOSTIC USE. Enabled */
#endif /* __XA_DM3PLUS_AK_PP_API_H__ */

