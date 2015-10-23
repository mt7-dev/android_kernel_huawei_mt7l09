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

CODEC_MK_FILE_PATH=$(BALONG_TOPDIR)/modem/med/build/hi6930
CODEC_SRC_FILE_PATH=$(BALONG_TOPDIR)/modem/med/hi6930
CUSTOM_SRC_FILE_PATH=$(BALONG_TOPDIR)/modem/med/custom/hi6930/src

#***********************************************************#
#Directories for include files
#***********************************************************#

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
OBC_LOCAL_MOUDLE_NAME	?=guhifi_custom
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

include $(CODEC_MK_FILE_PATH)/guhifi_custom.inc	

include $(CODEC_MK_FILE_PATH)/codec_custom.mk

dir_custom :=$(vob_src_dirs)
	
OBC_LOCAL_SRC_DIR := $(dir_custom)

OBC_LINK_SCRIPT:=$(CODEC_MK_FILE_PATH)/$(CFG_HIFI_LINK_DIR_NAME)

#include rules. must be droped at the bottom
include $(BALONG_SRC_PATH)/build/scripts/rules/xtensa_hifi_rules.mk
