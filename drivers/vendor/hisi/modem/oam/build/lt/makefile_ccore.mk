
#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=lmsp_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=lmsp1_ccore
endif

OAM_DIR :=$(BALONG_TOPDIR)/modem/oam
BALONG_SRC_PATH         ?=$(BALONG_TOPDIR)

OBC_LOCAL_USER_OBJ_FILE =

#***********************************************************#
# compiler flags
#***********************************************************#

CC_USER_FLAGS   :=  -fno-strict-aliasing\
					-DPROCESS_TRACE -DVOS_OSA_CPU=0 -DPRODUCT_CFG_CORE_TYPE_MCORE \
					-DVOS_VXWORKS=3 -DVOS_OS_VER=VOS_VXWORKS -DMSP_GUNAS_AT_UNITE \
					-DVERSION_V7R1_C010 -DBSP_CORE_MODEM -DFEATURE_DSP2ARM -D__LOG_RELEASE__ 

CC_USER_FLAGS   += -Werror

AS_USER_FLAGS   := 

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
CC_USER_FLAGS	+= -DINSTANCE_1
endif

#***********************************************************#
# Source File to compile
#***********************************************************#
MSP_LT_COMM_DIR			:=$(BALONG_TOPDIR)/modem/oam/lt/comm
MSP_COMM_INC_DIR        :=$(BALONG_TOPDIR)/modem/oam/inc/lt/comm
MSP_LT_CCORE_DIR   		:=$(BALONG_TOPDIR)/modem/oam/lt/ccore
MSP_CCORE_INC_DIR       :=$(BALONG_TOPDIR)/modem/oam/inc/lt/ccore
TAF_CCORE_DIR   		:=$(BALONG_TOPDIR)/modem/taf/lt/src/ccore
TAF_CCORE_INC_DIR       :=$(BALONG_TOPDIR)/modem/taf/lt/inc/ccore
TAF_CCORE_COMM_INC_DIR  :=$(BALONG_TOPDIR)/modem/taf/comm
PRJ_INCLUDE_DIR      	:=$(BALONG_TOPDIR)/include
PRJ_CONFIG_DIR          :=$(BALONG_TOPDIR)/config
PRJ_PLATFORM_DIR        :=$(BALONG_TOPDIR)/platform

#include $(OAM_DIR)/build/comm/MODEM_CORE/lmsp_code_comm_modem_core.inc
include $(OAM_DIR)/build/comm/MODEM_CORE/lmsp_code_comm_modem_core.mk


OBC_LOCAL_SRC_FILE := 
OBC_LOCAL_SRC_FILE +=$(vob_src_files)
OBC_LOCAL_SRC_DIR :=$(MSP_LT_COMM_DIR)/diag \
   $(MSP_LT_COMM_DIR)/list \
   $(MSP_LT_COMM_DIR)/sda \
   $(MSP_LT_COMM_DIR)/drx \
   $(MSP_LT_CCORE_DIR)/bbpds \
   $(MSP_LT_CCORE_DIR)/diag \
   $(MSP_LT_CCORE_DIR)/sleep \
   $(MSP_LT_CCORE_DIR)/sdm \
   $(TAF_CCORE_DIR)/ftm \
   $(TAF_CCORE_DIR)/l4a \
   $(TAF_CCORE_DIR)/sym 

OBC_LOCAL_INC_DIR :=$(vob_include_dirs)
OBC_LOCAL_INC_DIR +=$(PRJ_INCLUDE_DIR)/ps/gups \
   $(PRJ_INCLUDE_DIR)/ps/tlps \
   $(PRJ_INCLUDE_DIR)/ps/nas \
   $(PRJ_INCLUDE_DIR)/phy/lphy \
   $(PRJ_INCLUDE_DIR)/drv \
   $(PRJ_INCLUDE_DIR)/nv/tl/lps \
   $(PRJ_INCLUDE_DIR)/nv/tl/drv \
   $(PRJ_INCLUDE_DIR)/nv/tl/oam \
   $(PRJ_INCLUDE_DIR)/oam/comm/usimm \
   $(PRJ_INCLUDE_DIR)/oam/comm/osa \
   $(PRJ_INCLUDE_DIR)/oam/comm/om \
   $(PRJ_INCLUDE_DIR)/oam/comm/scm \
   $(PRJ_INCLUDE_DIR)/oam/comm/nvim \
   $(PRJ_INCLUDE_DIR)/oam/comm/errno \
   $(PRJ_INCLUDE_DIR)/oam/lt/msp \
   $(PRJ_INCLUDE_DIR)/oam/lt/diag \
   $(PRJ_INCLUDE_DIR)/oam/gu/log \
   $(PRJ_INCLUDE_DIR)/taf \
   $(MSP_COMM_INC_DIR)/cfg \
   $(MSP_COMM_INC_DIR)/diag \
   $(MSP_COMM_INC_DIR)/list \
   $(MSP_COMM_INC_DIR)/type \
   $(MSP_COMM_INC_DIR)/drx \
   $(MSP_CCORE_INC_DIR)/bbpds \
   $(MSP_CCORE_INC_DIR)/diag \
   $(MSP_CCORE_INC_DIR)/sleep \
   $(TAF_CCORE_INC_DIR)/ftm \
   $(TAF_CCORE_INC_DIR)/l4a \
   $(TAF_CCORE_COMM_INC_DIR)/src/acore/at/inc\
   $(PRJ_CONFIG_DIR)/osa \
   $(PRJ_CONFIG_DIR)/log \
   $(PRJ_CONFIG_DIR)/nvim/include/gu \
   $(PRJ_CONFIG_DIR)/nvim/include/tl/tlps \
   $(PRJ_CONFIG_DIR)/product/define/$(OBB_PRODUCT_NAME) \
   $(WIND_BASE)/target/h \
   $(WIND_BASE)/target/h/wrn/coreip

#include rules. must be droped at the bottom
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/cc_vxworks6.8_rules.mk

#pclint
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/vxworks.lnt

PCLINT_INC_VAR				:=$(addprefix -I,$(sort $(dir $(OBC_LOCAL_SRC_FILE)))) $(FULL_SRC_INC_PATH) -I$(BALONG_TOPDIR)/build/tools/pc_lint/lint/external
PCLINT_INC_FILE				:=$(BALONG_TOPDIR)/modem/oam/build/lt/pclint_inc.lnt
PCLINT_SRC_FILE				:=$(BALONG_TOPDIR)/modem/oam/build/lt/pclint_src.lnt
PCLINT_CC_OPTION			:=$(LOCAL_CC_OPTION)

.PHONY: $(maction)
$(maction): do_$(maction)
	@echo lmsp $(maction) is ready.

#To use flexelint

ifneq ($(strip $(maction)),)
OBC_USE_NEW_PC_LINT := true
ifeq ($(strip $(OBC_USE_NEW_PC_LINT)),false)
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules.mk
else
include makefile_flexelint_common_files.mk
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules_new.mk
endif
endif
