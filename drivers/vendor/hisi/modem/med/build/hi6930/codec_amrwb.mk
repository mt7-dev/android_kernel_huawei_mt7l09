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

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs := $(MED_VOICE_DIR)/voice_codec/amrwb/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files := 	$(vob_src_dirs)/amrwb_agc2.c               \
	$(vob_src_dirs)/amrwb_autocorr.c           \
	$(vob_src_dirs)/amrwb_az_isp.c             \
	$(vob_src_dirs)/amrwb_bits.c               \
	$(vob_src_dirs)/amrwb_c2t64fx.c            \
	$(vob_src_dirs)/amrwb_c4t64fx_hifi.c       \
	$(vob_src_dirs)/amrwb_cod_main.c           \
	$(vob_src_dirs)/amrwb_convert.c            \
	$(vob_src_dirs)/amrwb_convolve.c      \
	$(vob_src_dirs)/amrwb_cor_h_x.c            \
	$(vob_src_dirs)/amrwb_d2t64fx.c            \
	$(vob_src_dirs)/amrwb_d4t64fx.c            \
	$(vob_src_dirs)/amrwb_d_gain2.c            \
	$(vob_src_dirs)/amrwb_dec_main.c           \
	$(vob_src_dirs)/amrwb_decim54_hifi.c       \
	$(vob_src_dirs)/amrwb_deemph.c             \
	$(vob_src_dirs)/amrwb_dtx.c                \
	$(vob_src_dirs)/amrwb_g_pitch.c            \
	$(vob_src_dirs)/amrwb_gpclip.c             \
	$(vob_src_dirs)/amrwb_homing.c             \
	$(vob_src_dirs)/amrwb_hp6k.c               \
	$(vob_src_dirs)/amrwb_hp7k.c               \
	$(vob_src_dirs)/amrwb_hp50.c               \
	$(vob_src_dirs)/amrwb_hp400.c              \
	$(vob_src_dirs)/amrwb_hp_wsp_hifi.c        \
	$(vob_src_dirs)/amrwb_int_lpc.c            \
	$(vob_src_dirs)/amrwb_interface.c          \
	$(vob_src_dirs)/amrwb_isfextrp.c           \
	$(vob_src_dirs)/amrwb_isp_az.c             \
	$(vob_src_dirs)/amrwb_isp_isf.c            \
	$(vob_src_dirs)/amrwb_lag_wind.c           \
	$(vob_src_dirs)/amrwb_lagconc.c            \
	$(vob_src_dirs)/amrwb_levinson.c           \
	$(vob_src_dirs)/amrwb_log2.c               \
	$(vob_src_dirs)/amrwb_lp_dec2.c            \
	$(vob_src_dirs)/amrwb_math_op.c            \
	$(vob_src_dirs)/amrwb_p_med_ol_hifi.c      \
	$(vob_src_dirs)/amrwb_ph_disp.c            \
	$(vob_src_dirs)/amrwb_pit_shrp.c           \
	$(vob_src_dirs)/amrwb_pitch_f4_hifi.c      \
	$(vob_src_dirs)/amrwb_pred_lt4_hifi.c      \
	$(vob_src_dirs)/amrwb_preemph.c            \
	$(vob_src_dirs)/amrwb_q_gain2.c            \
	$(vob_src_dirs)/amrwb_q_gain2_tab.c        \
	$(vob_src_dirs)/amrwb_q_pulse.c            \
	$(vob_src_dirs)/amrwb_qisf_ns.c            \
	$(vob_src_dirs)/amrwb_qpisf_2s_hifi.c      \
	$(vob_src_dirs)/amrwb_random.c             \
	$(vob_src_dirs)/amrwb_residu.c        \
	$(vob_src_dirs)/amrwb_scale_hifi.c         \
	$(vob_src_dirs)/amrwb_syn_filt_hifi.c      \
	$(vob_src_dirs)/amrwb_updt_tar.c           \
	$(vob_src_dirs)/amrwb_voicefac.c           \
	$(vob_src_dirs)/amrwb_wb_vad_hifi.c        \
	$(vob_src_dirs)/amrwb_weight_a.c 

	

####################################################################################################
#general make rules
####################################################################################################

####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################