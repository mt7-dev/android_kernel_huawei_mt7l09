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
vob_src_dirs := $(CODEC_SRC_FILE_PATH)/codec/amr/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files :=$(vob_src_dirs)/a_refl.c         \
	$(vob_src_dirs)/agc.c            \
	$(vob_src_dirs)/amr_comm.c       \
	$(vob_src_dirs)/autocorr.c       \
	$(vob_src_dirs)/b_cn_cod.c       \
	$(vob_src_dirs)/bgnscd.c         \
	$(vob_src_dirs)/bitno_tab.c      \
	$(vob_src_dirs)/bits2prm.c       \
	$(vob_src_dirs)/c_g_aver.c       \
	$(vob_src_dirs)/c1035pf.c        \
	$(vob_src_dirs)/c2_11pf.c        \
	$(vob_src_dirs)/c2_11pf_tab.c    \
	$(vob_src_dirs)/c2_9pf.c         \
	$(vob_src_dirs)/c2_9pf_tab.c     \
	$(vob_src_dirs)/c4_17pf.c        \
	$(vob_src_dirs)/c8_31pf.c        \
	$(vob_src_dirs)/calc_en.c        \
	$(vob_src_dirs)/cbsearch.c       \
	$(vob_src_dirs)/cl_ltp.c         \
	$(vob_src_dirs)/cod_amr.c        \
	$(vob_src_dirs)/copy.c           \
	$(vob_src_dirs)/corrwght_tab.c   \
	$(vob_src_dirs)/d_gain_c.c       \
	$(vob_src_dirs)/d_gain_p.c       \
	$(vob_src_dirs)/d_homing.c       \
	$(vob_src_dirs)/d_homing_tab.c   \
	$(vob_src_dirs)/d_plsf.c         \
	$(vob_src_dirs)/d_plsf_3.c       \
	$(vob_src_dirs)/d_plsf_5.c       \
	$(vob_src_dirs)/d1035pf.c        \
	$(vob_src_dirs)/d2_11pf.c        \
	$(vob_src_dirs)/d2_9pf.c         \
	$(vob_src_dirs)/d3_14pf.c        \
	$(vob_src_dirs)/d4_17pf.c        \
	$(vob_src_dirs)/d8_31pf.c        \
	$(vob_src_dirs)/dec_amr.c        \
	$(vob_src_dirs)/dec_gain.c       \
	$(vob_src_dirs)/dec_lag3.c       \
	$(vob_src_dirs)/dec_lag6.c       \
	$(vob_src_dirs)/dtx_dec.c        \
	$(vob_src_dirs)/dtx_enc.c        \
	$(vob_src_dirs)/e_homing.c       \
	$(vob_src_dirs)/ec_gains.c       \
	$(vob_src_dirs)/enc_lag3.c       \
	$(vob_src_dirs)/enc_lag6.c       \
	$(vob_src_dirs)/ex_ctrl.c        \
	$(vob_src_dirs)/g_adapt.c        \
	$(vob_src_dirs)/g_code.c         \
	$(vob_src_dirs)/g_pitch.c        \
	$(vob_src_dirs)/gain_q.c         \
	$(vob_src_dirs)/gains_tab.c      \
	$(vob_src_dirs)/gc_pred.c        \
	$(vob_src_dirs)/gmed_n.c         \
	$(vob_src_dirs)/gray_tab.c       \
	$(vob_src_dirs)/grid_tab.c       \
	$(vob_src_dirs)/hp_max.c         \
	$(vob_src_dirs)/int_lpc.c        \
	$(vob_src_dirs)/int_lsf.c        \
	$(vob_src_dirs)/inter_36.c       \
	$(vob_src_dirs)/inter_36_tab.c   \
	$(vob_src_dirs)/inv_sqrt.c       \
	$(vob_src_dirs)/inv_sqrt_tab.c   \
	$(vob_src_dirs)/lag_wind.c       \
	$(vob_src_dirs)/lag_wind_tab.c   \
	$(vob_src_dirs)/levinson.c       \
	$(vob_src_dirs)/lflg_upd.c       \
	$(vob_src_dirs)/log2.c           \
	$(vob_src_dirs)/log2_tab.c       \
	$(vob_src_dirs)/lpc.c            \
	$(vob_src_dirs)/lsfwt.c          \
	$(vob_src_dirs)/lsp.c            \
	$(vob_src_dirs)/lsp_avg.c        \
	$(vob_src_dirs)/lsp_az.c         \
	$(vob_src_dirs)/lsp_lsf.c        \
	$(vob_src_dirs)/lsp_lsf_tab.c    \
	$(vob_src_dirs)/lsp_tab.c        \
	$(vob_src_dirs)/ol_ltp.c         \
	$(vob_src_dirs)/p_ol_wgh.c       \
	$(vob_src_dirs)/ph_disp.c        \
	$(vob_src_dirs)/ph_disp_tab.c    \
	$(vob_src_dirs)/pitch_ol.c       \
	$(vob_src_dirs)/post_pro.c       \
	$(vob_src_dirs)/pow2.c           \
	$(vob_src_dirs)/pow2_tab.c       \
	$(vob_src_dirs)/pre_big.c        \
	$(vob_src_dirs)/pre_proc.c       \
	$(vob_src_dirs)/pred_lt.c        \
	$(vob_src_dirs)/preemph.c        \
	$(vob_src_dirs)/prm2bits.c       \
	$(vob_src_dirs)/pstfilt.c        \
	$(vob_src_dirs)/q_gain_c.c       \
	$(vob_src_dirs)/q_gain_p.c       \
	$(vob_src_dirs)/q_plsf.c         \
	$(vob_src_dirs)/q_plsf_3.c       \
	$(vob_src_dirs)/q_plsf_3_tab.c   \
	$(vob_src_dirs)/q_plsf_5.c       \
	$(vob_src_dirs)/q_plsf_5_tab.c   \
	$(vob_src_dirs)/qgain475.c       \
	$(vob_src_dirs)/qgain475_tab.c   \
	$(vob_src_dirs)/qgain795.c       \
	$(vob_src_dirs)/qua_gain.c       \
	$(vob_src_dirs)/qua_gain_tab.c   \
	$(vob_src_dirs)/r_fft.c          \
	$(vob_src_dirs)/reorder.c        \
	$(vob_src_dirs)/s10_8pf.c        \
	$(vob_src_dirs)/set_zero.c       \
	$(vob_src_dirs)/sid_sync.c       \
	$(vob_src_dirs)/sp_dec.c         \
	$(vob_src_dirs)/sp_enc.c         \
	$(vob_src_dirs)/spreproc.c       \
	$(vob_src_dirs)/spstproc.c       \
	$(vob_src_dirs)/sqrt_l.c         \
	$(vob_src_dirs)/sqrt_l_tab.c     \
	$(vob_src_dirs)/strfunc.c        \
	$(vob_src_dirs)/ton_stab.c       \
	$(vob_src_dirs)/vad1.c           \
	$(vob_src_dirs)/vadname.c        \
	$(vob_src_dirs)/window_tab.c     \
	$(vob_src_dirs)/amr_convert.c    \
	$(vob_src_dirs)/set_sign.c       \
	$(vob_src_dirs)/vad2.c           \
	$(vob_src_dirs)/amr_interface.c  \
	$(vob_src_dirs)/az_lsp.c         \
	$(vob_src_dirs)/c3_14pf.c        \
	$(vob_src_dirs)/calc_cor.c       \
	$(vob_src_dirs)/convolve.c       \
	$(vob_src_dirs)/cor_h.c          \
	$(vob_src_dirs)/pitch_fr.c       \
	$(vob_src_dirs)/weight_a.c       \
	$(vob_src_dirs)/residu.c         \
	$(vob_src_dirs)/syn_filt.c
	

####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################