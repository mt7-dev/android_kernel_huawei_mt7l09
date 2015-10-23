####################################################################################################
#
####################################################################################################


####################################################################################################
#Directories for library files 
####################################################################################################
vob_lib_dirs :=

####################################################################################################
#library files
####################################################################################################
vob_lib_files :=

####################################################################################################
#Directories for include files
####################################################################################################
include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med.inc

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs := $(CODEC_SRC_FILE_PATH)/codec/efr/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files :=$(vob_src_dirs)/efr_lag_wind.c       \
	$(vob_src_dirs)/efr_lag_wind_tab.c   \
	$(vob_src_dirs)/efr_levinson.c       \
	$(vob_src_dirs)/efr_log2.c           \
	$(vob_src_dirs)/efr_log2_tab.c       \
	$(vob_src_dirs)/efr_lsp_az.c         \
	$(vob_src_dirs)/efr_lsp_lsf.c        \
	$(vob_src_dirs)/efr_lsp_lsf_tab.c    \
	$(vob_src_dirs)/efr_pitch_ol.c       \
	$(vob_src_dirs)/efr_pow2.c           \
	$(vob_src_dirs)/efr_pow2_tab.c       \
	$(vob_src_dirs)/efr_pre_proc.c       \
	$(vob_src_dirs)/efr_pred_lt6.c       \
	$(vob_src_dirs)/efr_preemph.c        \
	$(vob_src_dirs)/efr_prm2bits.c       \
	$(vob_src_dirs)/efr_pstfilt2.c       \
	$(vob_src_dirs)/efr_q_gains.c        \
	$(vob_src_dirs)/efr_q_plsf_5.c       \
	$(vob_src_dirs)/efr_q_plsf_5_tab.c   \
	$(vob_src_dirs)/efr_reorder.c        \
	$(vob_src_dirs)/efr_weight_a.c       \
	$(vob_src_dirs)/efr_window2_tab.c    \
	$(vob_src_dirs)/efr_agc.c            \
	$(vob_src_dirs)/efr_autocorr.c       \
	$(vob_src_dirs)/efr_az_lsp.c         \
	$(vob_src_dirs)/efr_bits2prm.c       \
	$(vob_src_dirs)/efr_cod_12k2.c       \
	$(vob_src_dirs)/efr_d_1035pf.c       \
	$(vob_src_dirs)/efr_d_gains.c        \
	$(vob_src_dirs)/efr_d_homing.c       \
	$(vob_src_dirs)/efr_d_plsf_5.c       \
	$(vob_src_dirs)/efr_dec_12k2.c       \
	$(vob_src_dirs)/efr_dec_lag6.c       \
	$(vob_src_dirs)/efr_dtx.c            \
	$(vob_src_dirs)/efr_e_homing.c       \
	$(vob_src_dirs)/efr_enc_lag6.c       \
	$(vob_src_dirs)/efr_grid_tab.c       \
	$(vob_src_dirs)/efr_int_lpc.c        \
	$(vob_src_dirs)/efr_inter_6.c        \
	$(vob_src_dirs)/efr_inv_sqrt_tab.c   \
	$(vob_src_dirs)/efr_inv_sqrt.c       \
	$(vob_src_dirs)/efr_interface.c      \
	$(vob_src_dirs)/efr_vad.c            \
	$(vob_src_dirs)/efr_g_code.c         \
	$(vob_src_dirs)/efr_g_pitch.c        \
	$(vob_src_dirs)/efr_pitch_f6.c       \
	$(vob_src_dirs)/efr_c1035pf.c        \
	$(vob_src_dirs)/efr_residu.c         \
	$(vob_src_dirs)/efr_syn_filt.c       \
	$(vob_src_dirs)/efr_convolve.c


####################################################################################################
#general make rules
####################################################################################################

####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################