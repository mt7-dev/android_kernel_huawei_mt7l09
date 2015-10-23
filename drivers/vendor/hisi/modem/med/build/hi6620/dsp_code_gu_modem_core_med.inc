####################################################################################################
#
####################################################################################################

CODEC_PRODUCT_INC_PATH = -I$(BALONG_SRC_PATH)
CODEC_INC_PATH = -I$(CODEC_SRC_FILE_PATH)
CODEC_SYSTEM_INC_PATH = -I$(TENSILICA_TOOLS_HIFI)/XtensaTools/xtensa-elf/include/xtensa
####################################################################################################
#Directories for include files
####################################################################################################
vob_include_dirs := \
    $(CODEC_PRODUCT_INC_PATH)/config/nvim/include/gu \
	$(CODEC_INC_PATH)/osa/inc \
	$(CODEC_PRODUCT_INC_PATH)/config/osa \
    $(CODEC_PRODUCT_INC_PATH)/config/product/include/$(PRODUCT_CFG_PRODUCT_FORM) \
    $(CODEC_PRODUCT_INC_PATH)/include/drv \
    $(CODEC_PRODUCT_INC_PATH)/include/drv/MODULE_H \
    $(CODEC_PRODUCT_INC_PATH)/include/nv/gu/codec \
    $(CODEC_PRODUCT_INC_PATH)/include/med \
    $(CODEC_PRODUCT_INC_PATH)/include/oam/gu/log \
    $(CODEC_PRODUCT_INC_PATH)/include/oam/comm/om \
	$(CODEC_PRODUCT_INC_PATH)/include/oam/gu/nvim \
    $(CODEC_PRODUCT_INC_PATH)/platform/$(PRODUCT_CFG_PRODUCT_FORM) \
    $(CODEC_SYSTEM_INC_PATH) \
	
vob_include_dirs += \
    $(CODEC_INC_PATH)/codec/amr/inc\
    $(CODEC_INC_PATH)/codec/common/inc\
    $(CODEC_INC_PATH)/codec/efr/inc\
    $(CODEC_INC_PATH)/codec/fr/inc\
    $(CODEC_INC_PATH)/codec/hr/inc\
    $(CODEC_INC_PATH)/codec/amrwb/inc\
    $(CODEC_INC_PATH)/codec/pp/inc\
    $(CODEC_INC_PATH)/codec/xa_common/inc\
    $(CODEC_INC_PATH)/codec/mp3/inc\
    $(CODEC_INC_PATH)/voice/inc\
    $(CODEC_INC_PATH)/audio/inc\
    $(CODEC_INC_PATH)/common/inc\
    $(CODEC_INC_PATH)/drv/inc\
    $(CODEC_INC_PATH)/om/inc\
    $(CODEC_INC_PATH)/codec/aac/inc\
    $(CODEC_INC_PATH)/codec/dolby/inc\
    $(CODEC_INC_PATH)/codec/resample/inc\
    $(CODEC_INC_PATH)/osa/inc\
	$(CODEC_INC_PATH)/custom/inc