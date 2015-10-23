####################################################################################################
#
####################################################################################################
CODEC_INC_PATH =$(CODEC_SRC_FILE_PATH)
####################################################################################################
#Directories for include files
# $(CODEC_INC_PATH)/osa/inc  must before  $(BALONG_SRC_PATH)/config/osa 
####################################################################################################
OBC_LOCAL_INC_DIR := \
    $(CODEC_INC_PATH)/osa/inc 
	
OBC_LOCAL_INC_DIR += \
    $(BALONG_SRC_PATH)/config/nvim/include/gu \
	$(BALONG_SRC_PATH)/config/osa \
    $(BALONG_SRC_PATH)/config/product/$(PRODUCT_CFG_PRODUCT_FORM)/include_gu/ \
    $(BALONG_SRC_PATH)/include/drv \
    $(BALONG_SRC_PATH)/include/drv/MODULE_H \
    $(BALONG_SRC_PATH)/include/nv/gu/codec \
    $(BALONG_SRC_PATH)/include/med \
    $(BALONG_SRC_PATH)/include/oam/gu/log \
    $(BALONG_SRC_PATH)/include/oam/comm/om \
	$(BALONG_SRC_PATH)/include/oam/comm/nvim \
    $(BALONG_SRC_PATH)/platform/$(PRODUCT_CFG_PLATFORM_NEW) \
	$(BALONG_SRC_PATH)/platform/$(PRODUCT_CFG_PLATFORM_NEW)/soc \
	$(BALONG_SRC_PATH)/modem/med/custom/hi6930/inc \
	$(TENSILICA_TOOLS_HIFI)/XtensaTools/xtensa-elf/include/xtensa\
	
OBC_LOCAL_INC_DIR += \
    $(CODEC_INC_PATH)/codec/xa_common/inc\
    $(CODEC_INC_PATH)/codec/mp3/inc\
    $(CODEC_INC_PATH)/audio/inc\
    $(CODEC_INC_PATH)/ucom/inc\
    $(CODEC_INC_PATH)/drv/inc\
    $(CODEC_INC_PATH)/om/inc\
    $(CODEC_INC_PATH)/codec/aac/inc\
    $(CODEC_INC_PATH)/codec/dolby/inc\
    $(CODEC_INC_PATH)/codec/resample/inc\
	$(CODEC_INC_PATH)/codec/dts/inc\
	$(MED_TOPDIR)/common/inc/codec \
	$(MED_TOPDIR)/common/inc/om \
	$(MED_TOPDIR)/common/inc/voice
	
OBC_LOCAL_INC_DIR += \
    $(MED_VOICE_DIR)/voice_codec/amr/inc\
    $(MED_VOICE_DIR)/voice_codec/common/inc\
    $(MED_VOICE_DIR)/voice_codec/efr/inc\
    $(MED_VOICE_DIR)/voice_codec/fr/inc\
    $(MED_VOICE_DIR)/voice_codec/hr/inc\
    $(MED_VOICE_DIR)/voice_codec/amrwb/inc\
    $(MED_VOICE_DIR)/voice_codec/pp/inc	
	