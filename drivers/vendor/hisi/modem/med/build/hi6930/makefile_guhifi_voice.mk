#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
#src path define
#***********************************************************#
BALONG_SRC_PATH=$(BALONG_TOPDIR)
PRODUCT_CFG_PLATFORM_NEW=$(CFG_PLATFORM_HISI_BALONG)
PRODUCT_CFG_PRODUCT_FORM=$(OBB_PRODUCT_NAME)

MED_TOPDIR=$(BALONG_TOPDIR)/modem/med
MED_VOICE_DIR=$(MED_TOPDIR)/voice

CODEC_MK_FILE_PATH=$(BALONG_TOPDIR)/modem/med/build/hi6930
CODEC_SRC_FILE_PATH=$(BALONG_TOPDIR)/modem/med/hi6930

#***********************************************************#
#Directories for include files
#***********************************************************#

	
#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
OBC_LOCAL_MOUDLE_NAME	?=guhifi_voice

#****************************************************************************
# compiler configs
#****************************************************************************
#CFG_BOARD is defined in product_config.define
PRODUCT_CFG_BOARD_TYPE			:=BOARD_$(CFG_BOARD)

#CFG_CHIP_BB is defined in product_config.define
PRODUCT_CFG_CHIP_BB_TYPE		:=CHIP_BB_$(CFG_CHIP_BB) 

#PRODUCT_CFG_PRODUCT_VERSION is defined in product_config.define
OBC_USER_CC_DEFINES := -D$(PRODUCT_CFG_BOARD_TYPE) -D$(PRODUCT_CFG_CHIP_BB_TYPE)
OBC_USER_CC_FLAGS   :=


#****************************************************************************
# src & inc
#****************************************************************************

include $(CODEC_MK_FILE_PATH)/guhifi_voice.inc
	
include $(CODEC_MK_FILE_PATH)/codec_amrwb.mk
src_codec_amrwb := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/codec_amr.mk
src_codec_amr := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/codec_efr.mk
src_codec_efr := 	$(vob_src_files)

include $(CODEC_MK_FILE_PATH)/codec_fr.mk
src_codec_fr:=	$(vob_src_files)
	
include $(CODEC_MK_FILE_PATH)/codec_hr.mk
src_codec_hr :=	$(vob_src_files)

include $(CODEC_MK_FILE_PATH)/voice.mk
src_voice_main := $(vob_src_files)

OBC_LOCAL_SRC_FILE := $(src_codec_amr) $(src_codec_amrwb) $(src_codec_efr) $(src_codec_fr) $(src_codec_hr) $(src_voice_main) 

#include rules. must be droped at the bottom
include $(BALONG_SRC_PATH)/build/scripts/rules/xtensa_hifi_rules.mk
