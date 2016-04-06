# Need definition 
# 1. CFG_GU_PLATFORM  from make_base.mk
# 2. OBB_PRODUCT_NAME from build command line parameters
# 3. BALONG_TOPDIR    from build_balong_acore.sh
# 4. CFG_BOARD        from build command line parameters 
# 5. CFG_CHIP_BB      from build command line parameters

#Include Common Path
CURR_WORKDIR :=../../../../oam
TAF_WORKDIR :=../../../../taf
LT_AT_DIR = ../../../../taf/lt/src/acore/at
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

INCLUDE_COMM_DIR_FLAG := \
	-I$(LINUX_KERNEL)/arch/arm/mach-balong/include/mach \
	-I$(LINUX_KERNEL)/include/linux \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/ \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu/ \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/ \
	-I$(BALONG_TOPDIR)/config/product/include/$(CFG_GU_PLATFORM_NEW)/ \
	-I$(BALONG_TOPDIR)/config/nvim/include/gu/ \
	-I$(BALONG_TOPDIR)/config/nvim/include/lt/tlps \
	-I$(BALONG_TOPDIR)/include/ps/tlps \
	-I$(BALONG_TOPDIR)/include/app \
	-I$(BALONG_TOPDIR)/include/drv/ \
	-I$(BALONG_TOPDIR)/include/nv/tl/drv/ \
	-I$(BALONG_TOPDIR)/include/nv/tl/oam/ \
	-I$(BALONG_TOPDIR)/include/nv/tl/lps \
	-I$(BALONG_TOPDIR)/include/med/ \
	-I$(BALONG_TOPDIR)/include/oam/gu/log \
	-I$(BALONG_TOPDIR)/include/oam/gu/om \
	-I$(BALONG_TOPDIR)/include/oam/gu/nvim \
	-I$(BALONG_TOPDIR)/include/oam/lt/msp \
	-I$(BALONG_TOPDIR)/include/oam/lt/diag \
	-I$(BALONG_TOPDIR)/include/oam/comm/errno \
	-I$(BALONG_TOPDIR)/include/oam/comm/nvim \
	-I$(BALONG_TOPDIR)/include/oam/comm/om \
	-I$(BALONG_TOPDIR)/include/oam/comm/osa \
	-I$(BALONG_TOPDIR)/include/oam/comm/scm \
	-I$(BALONG_TOPDIR)/include/oam/comm/si \
	-I$(BALONG_TOPDIR)/include/oam/comm/usimm \
	-I$(BALONG_TOPDIR)/include/oam/comm/socp \
	-I$(BALONG_TOPDIR)/include/phy/gphy \
	-I$(BALONG_TOPDIR)/include/phy/wphy \
	-I$(BALONG_TOPDIR)/include/phy/lphy \
	-I$(BALONG_TOPDIR)/include/ps/gups \
	-I$(BALONG_TOPDIR)/include/ps/tlps \
	-I$(BALONG_TOPDIR)/include/ps/nas \
	-I$(BALONG_TOPDIR)/include/taf \
	-I$(BALONG_TOPDIR)/include/tools \
	-I$(BALONG_TOPDIR)/config/log \
	-I$(BALONG_TOPDIR)/config/osa \
	-I$(BALONG_TOPDIR)/config/nvim/include/tl/tlps
	

#Inclue Your Own Path
INCLUDE_OWN_DIR_FLAG += -I$(BALONG_TOPDIR)/modem/oam/inc/comm/comm/si/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/comm/ccore/om/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/list/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/acore/diag/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/cfg/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/diag/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/drx/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/type/ \
	-I$(BALONG_TOPDIR)/modem/oam/inc/gu/acore/om/ \
	-I$(BALONG_TOPDIR)/modem/taf/comm/src/acore/at/inc/ \
	-I$(BALONG_TOPDIR)/modem/taf/lt/inc/acore/at/ \
	
include $(BALONG_TOPDIR)/modem/oam/build/comm/APP_CORE/lmsp_code_comm_app_core.inc
include $(BALONG_TOPDIR)/modem/oam/build/comm/APP_CORE/lmsp_code_comm_app_core.mk

#LMSP.O dependencies
LMSPOBJ := $(CURR_WORKDIR)/lt/comm/diag/diag_api.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_test.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_common.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_buf_ctrl.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_cfg.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_debug.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_fw.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_app_agent.o \
	$(CURR_WORKDIR)/lt/comm/diag/diag_fs_proc.o\
	$(CURR_WORKDIR)/lt/comm/list/blist.o \
	$(CURR_WORKDIR)/lt/comm/drx/drx_msp_api.o\
	$(CURR_WORKDIR)/lt/comm/drx/drx_msp_test.o\
	$(LT_AT_DIR)/at_lte_common.o \
	$(LT_AT_DIR)/at_lte_ct_ex_proc.o \
	$(LT_AT_DIR)/at_lte_ct_ex2_proc.o \
	$(LT_AT_DIR)/at_lte_ct_proc.o \
	$(LT_AT_DIR)/at_lte_eventreport.o \
	$(LT_AT_DIR)/at_lte_ms_proc.o \
	$(LT_AT_DIR)/at_lte_set_proc.o \
	$(LT_AT_DIR)/at_lte_st_proc.o \
	$(LT_AT_DIR)/at_lte_upgrade_proc.o \
	$(LT_AT_DIR)/AtDeviceCmdTL.o \
	$(LT_AT_DIR)/at_comm_cmd.o
ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_OFF)
LMSPOBJ +=$(CURR_WORKDIR)/lt/acore/diag/diag_vcom.o\
	$(CURR_WORKDIR)/lt/acore/diag/diag_usb.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_port.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_sd.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_tcp.o \
	$(CURR_WORKDIR)/lt/acore/diag/diag_hisc.o 
endif		

LMSP_CMD_OBJ := $(foreach dir,$(dir $(LMSPOBJ)),$(dir).*.o.cmd)


# Target
CC_INC_DIRS := $(foreach dir,$(MSP_INC_DIRS),-I$(dir))
LMSP_SRC_FILES:= $(MSP_COMM_SRC_FILES)
LMSP_OBJS := $(subst .C,.o,$(subst .c,.o,$(LMSP_SRC_FILES)))
LMSP-objs := $(LMSP_OBJS) 

LMSP-objs += $(LMSPOBJ)
#obj-$(OBB_CFG_BUILDIN) += LMSP.o
obj-y += LMSP.o

# User definition
BUILD_V3R2 := FALSE
MSP_SDM_MACRO    := -DHAL_DIAG_SDM_USE
MSP_PRODUCT_CFG  := 
TARGET_DIR       := $(OUT_KERNEL)/drivers/lmsp
# User definition
ifeq ($(BUILD_V3R2),FALSE)

endif
EXTRA_CFLAGS += $(MSP_SDM_MACRO) $(MSP_PRODUCT_CFG)
EXTRA_CFLAGS += -fsigned-char -DVOS_LINUX=4 -DOSA_CPU_ACPU=1 -DVOS_OS_VER=VOS_LINUX -DVOS_OSA_CPU=OSA_CPU_ACPU -DPRODUCT_CFG_MULTIMODE_LTE -D__LOG_RELEASE__ 
#Defined in top shell script(Comm Definition)
EXTRA_CFLAGS += $(INCLUDE_FLAG) $(DRV_INCLUDE_FLAG) $(ASM_INCLUDE_FLAG) $(PLATFORM_FLAG)
EXTRA_CFLAGS += $(INCLUDE_COMM_DIR_FLAG) $(INCLUDE_OWN_DIR_FLAG) $(CC_INC_DIRS) -Wno-pointer-to-int-cast

export KBUILD_CFLAGS := $(filter-out -Werror,$(KBUILD_CFLAGS))

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/cc_linux_rules.mk

export KBUILD_CFLAGS := $(filter-out -Werror,$(KBUILD_CFLAGS))

#pclint
#PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/android.lnt
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/modem/oam/build/lt/APP_CORE/android.lnt
PCLINT_INC_VAR			:=
PCLINT_INC_FILE			:=$(BALONG_TOPDIR)/modem/oam/build/lt/pclint_inc.lnt
PCLINT_SRC_FILE			:=$(BALONG_TOPDIR)/modem/oam/build/lt/APP_CORE/pclint_src.lnt
PCLINT_CC_OPTION		:=$(EXTRA_CFLAGS)
OBC_LOCAL_MOUDLE_NAME		=lmsp_acore
.PHONY: $(maction)
$(maction): do_$(maction)
	@echo lmsp_acore $(maction) is ready.

ifneq ($(strip $(maction)),)
OBC_USE_NEW_PCLINT := true
ifeq ($(strip $(OBC_USE_NEW_PC_LINT)),false)
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules.mk
else
PCLINT_SYSTEM_INC_FILE := $(BALONG_TOPDIR)/build/tools/pc_lint/lint/android.lnt
include makefile_flexelint_common_files.mk
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules_new.mk
endif
endif
subdir-ccflags-y := -Werror
