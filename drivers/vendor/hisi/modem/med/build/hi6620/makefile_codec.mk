
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

CODEC_MK_FILE_PATH=$(BALONG_TOPDIR)/modem/med/build/$(CFG_PLATFORM_CHIP_TYPE)
CODEC_SRC_FILE_PATH=$(BALONG_TOPDIR)/modem/med/$(CFG_PLATFORM_CHIP_TYPE)/src/hifi

#***********************************************************#
#Directories for include files
#***********************************************************#
-include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med.inc

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
OBC_LOCAL_MOUDLE_NAME	?=guhifi_mcore

#****************************************************************************
# compiler configs
#****************************************************************************
#CFG_BOARD is defined in product_config.define
PRODUCT_CFG_BOARD_TYPE			:=BOARD_$(CFG_BOARD)

#CFG_CHIP_BB is defined in product_config.define
PRODUCT_CFG_CHIP_BB_TYPE		:=CHIP_BB_$(CFG_CHIP_BB) 

#PRODUCT_CFG_PRODUCT_VERSION is defined in product_config.define
PRODUCT_CFG_CFLAGS	:= -D$(PRODUCT_CFG_PRODUCT_VERSION) -D$(PRODUCT_CFG_BOARD_TYPE) -D$(PRODUCT_CFG_CHIP_BB_TYPE)
SUBSYS_CFG_CFLAGS   :=


#****************************************************************************
# src & inc
#****************************************************************************

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med.inc

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_common.mk
src_comm :=$(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_amrwb.mk
src_codec_amrwb := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_amr.mk
src_codec_amr := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_efr.mk
src_codec_efr := 	$(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_fr.mk
src_codec_fr:=	$(vob_src_files)
	
include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_hr.mk
src_codec_hr :=	$(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_codec_main.mk
src_codec_main := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_drv.mk
src_drv := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_om.mk
src_om := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med_osa.mk
src_osa := $(vob_src_files)

OBC_LOCAL_SRC_FILE := $(src_comm) $(src_codec_amr) $(src_codec_amrwb) $(src_codec_efr) $(src_codec_fr) $(src_codec_hr) $(src_codec_main) $(src_drv) $(src_om) $(src_osa)

OBC_LINK_SCRIPT:=$(CODEC_MK_FILE_PATH)/$(CFG_HIFI_LINK_DIR_NAME)

#include rules. must be droped at the bottom
include $(BALONG_SRC_PATH)/build/scripts/rules/xtensa_hifi_rules.mk
