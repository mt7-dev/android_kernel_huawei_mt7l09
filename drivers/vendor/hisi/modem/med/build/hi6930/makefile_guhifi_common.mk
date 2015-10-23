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
OBC_LOCAL_MOUDLE_NAME	?=guhifi_common

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

include $(CODEC_MK_FILE_PATH)/guhifi_comm.inc

include $(CODEC_MK_FILE_PATH)/ucom.mk
src_comm :=$(vob_src_files)

# include $(CODEC_MK_FILE_PATH)/audio.mk
# src_codec_main := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/drv.mk
src_drv := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/om.mk
src_om := $(vob_src_files)

include $(CODEC_MK_FILE_PATH)/osa.mk
src_osa := $(vob_src_files)

OBC_LOCAL_SRC_FILE := $(src_comm)  $(src_drv) $(src_om) $(src_osa)

#include rules. must be droped at the bottom
include $(BALONG_SRC_PATH)/build/scripts/rules/xtensa_hifi_rules.mk
