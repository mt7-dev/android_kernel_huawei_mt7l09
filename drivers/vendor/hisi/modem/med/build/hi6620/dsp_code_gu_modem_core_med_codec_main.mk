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
vob_src_dirs := \
    $(CODEC_SRC_FILE_PATH)

####################################################################################################
#Source files
vob_src_files := \
	$(vob_src_dirs)/codec/common/src/xa_comm.c\
	$(vob_src_dirs)/codec/mp3/src/mp3_dec.c\
	$(vob_src_dirs)/audio/src/audio_comm.c\
	$(vob_src_dirs)/audio/src/audio_pcm.c\
	$(vob_src_dirs)/audio/src/audio_debug.c\
	$(vob_src_dirs)/audio/src/audio_player.c\
	$(vob_src_dirs)/audio/src/audio_recorder.c\
	$(vob_src_dirs)/audio/src/audio_voip.c\
	$(vob_src_dirs)/audio/src/audio_enhance.c\
	$(vob_src_dirs)/voice/src/voice_amr_mode.c\
	$(vob_src_dirs)/voice/src/voice_api.c\
	$(vob_src_dirs)/voice/src/voice_debug.c\
	$(vob_src_dirs)/voice/src/voice_log.c\
	$(vob_src_dirs)/voice/src/voice_mc.c\
	$(vob_src_dirs)/voice/src/voice_pcm.c\
	$(vob_src_dirs)/voice/src/voice_proc.c\
	$(vob_src_dirs)/codec/common/src/codec_com_codec.c       \
	$(vob_src_dirs)/codec/common/src/codec_op_cpx.c          \
	$(vob_src_dirs)/codec/common/src/codec_op_etsi.c         \
	$(vob_src_dirs)/codec/common/src/codec_op_etsi_hifi.c    \
	$(vob_src_dirs)/codec/common/src/codec_op_float.c        \
	$(vob_src_dirs)/codec/common/src/codec_op_lib.c          \
	$(vob_src_dirs)/codec/common/src/codec_op_netsi.c        \
	$(vob_src_dirs)/codec/common/src/codec_op_netsi_hifi.c   \
	$(vob_src_dirs)/codec/common/src/codec_op_vec.c          \
	$(vob_src_dirs)/codec/common/src/codec_op_vec_hifi.c     \
	$(vob_src_dirs)/codec/common/src/fir_bk_hifi2.c        \
	$(vob_src_dirs)/codec/common/src/vec_divide_hifi2.c    \
	$(vob_src_dirs)/codec/pp/src/med_aec_af_hifi.c         \
	$(vob_src_dirs)/codec/pp/src/med_aec_dtd.c             \
	$(vob_src_dirs)/codec/pp/src/med_aec_main.c            \
	$(vob_src_dirs)/codec/pp/src/med_aec_nlp.c             \
	$(vob_src_dirs)/codec/pp/src/med_agc.c                 \
	$(vob_src_dirs)/codec/pp/src/med_anr.c                 \
	$(vob_src_dirs)/codec/pp/src/med_anr_tab.c             \
	$(vob_src_dirs)/codec/pp/src/med_avc.c                 \
	$(vob_src_dirs)/codec/pp/src/med_avc_tab.c             \
	$(vob_src_dirs)/codec/pp/src/med_fft_hifi.c            \
	$(vob_src_dirs)/codec/pp/src/med_fft_tab.c             \
	$(vob_src_dirs)/codec/pp/src/med_fft_tab_hifi.c        \
	$(vob_src_dirs)/codec/pp/src/med_filt.c                \
	$(vob_src_dirs)/codec/pp/src/med_gain.c                \
	$(vob_src_dirs)/codec/pp/src/med_pp_comm.c             \
	$(vob_src_dirs)/codec/pp/src/med_vad_hifi.c            \
	$(vob_src_dirs)/codec/pp/src/med_vad_tab.c             \
	$(vob_src_dirs)/codec/pp/src/med_anr_2mic_fd.c         \
	$(vob_src_dirs)/codec/pp/src/med_anr_2mic_interface.c  \
	$(vob_src_dirs)/codec/pp/src/med_anr_2mic_td.c         \
	$(vob_src_dirs)/codec/pp/src/med_pp_main.c             \
	$(vob_src_dirs)/codec/pp/src/med_aig.c                 \
	$(vob_src_dirs)/codec/pp/src/med_mbdrc.c               \
	$(vob_src_dirs)/codec/dolby/src/dm3.c                  \
	$(vob_src_dirs)/codec/resample/src/xa_src_pp.c         \
	$(vob_src_dirs)/codec/aac/src/xa_aac_dec.c             \
	$(vob_src_dirs)/custom/src/mlib_main.c      			\
	$(vob_src_dirs)/custom/src/module_balong_pp.c			\
	$(vob_src_dirs)/custom/src/module_sample.c				\
	
####################################################################################################
#general make rules
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################