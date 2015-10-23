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
vob_src_dirs := \
    $(MED_VOICE_DIR)

####################################################################################################
#Source files
vob_src_files := \
	$(vob_src_dirs)/voice/src/voice_amr_mode.c\
	$(vob_src_dirs)/voice/src/voice_api.c\
	$(vob_src_dirs)/voice/src/voice_debug.c\
	$(vob_src_dirs)/voice/src/voice_log.c\
	$(vob_src_dirs)/voice/src/voice_mc.c\
	$(vob_src_dirs)/voice/src/voice_sjb.c\
	$(vob_src_dirs)/voice/src/voice_jb_interface.c\
	$(vob_src_dirs)/voice/src/voice_pcm.c\
	$(vob_src_dirs)/voice/src/voice_proc.c\
	$(vob_src_dirs)/voice/src/voice_diagnose.c\
	$(vob_src_dirs)/voice_codec/common/src/codec_com_codec.c       \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_cpx.c          \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_etsi.c         \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_etsi_hifi.c    \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_float.c        \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_lib.c          \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_netsi.c        \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_netsi_hifi.c   \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_vec.c          \
	$(vob_src_dirs)/voice_codec/common/src/codec_op_vec_hifi.c     \
	$(vob_src_dirs)/voice_codec/common/src/fir_bk_hifi2.c        \
	$(vob_src_dirs)/voice_codec/common/src/vec_divide_hifi2.c    \
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_af_hifi.c         \
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_hf_af.c         	\
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_hf_nlp.c         	\
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_dtd.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_main.c            \
	$(vob_src_dirs)/voice_codec/pp/src/med_aec_nlp.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_agc.c                 \
	$(vob_src_dirs)/voice_codec/pp/src/med_eanr.c                \
	$(vob_src_dirs)/voice_codec/pp/src/med_avc.c                 \
	$(vob_src_dirs)/voice_codec/pp/src/med_avc_tab.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_fft_hifi.c            \
	$(vob_src_dirs)/voice_codec/pp/src/med_fft_tab.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_fft_tab_hifi.c        \
	$(vob_src_dirs)/voice_codec/pp/src/med_filt.c                \
	$(vob_src_dirs)/voice_codec/pp/src/med_gain.c                \
	$(vob_src_dirs)/voice_codec/pp/src/med_pp_comm.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_vad_hifi.c            \
	$(vob_src_dirs)/voice_codec/pp/src/med_vad_tab.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_anr_2mic_fd.c         \
	$(vob_src_dirs)/voice_codec/pp/src/med_anr_2mic_interface.c  \
	$(vob_src_dirs)/voice_codec/pp/src/med_anr_2mic_td.c         \
	$(vob_src_dirs)/voice_codec/pp/src/med_pp_main.c             \
	$(vob_src_dirs)/voice_codec/pp/src/med_aig.c                 \
	$(vob_src_dirs)/voice_codec/pp/src/med_mbdrc.c              \
	$(vob_src_dirs)/custom/src/module_balong_pp.c               \
	$(vob_src_dirs)/custom/src/mlib_utility.c

	
####################################################################################################
#general make rules
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################