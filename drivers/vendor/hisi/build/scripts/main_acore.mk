
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).$(BALONG_TOPDIR))

# guess the Top dir
BALONG_TOPDIR ?= $(patsubst %/build,%,$(CURDIR))

# Make sure that there are no spaces in the absolute path; the build system can't deal with them.
ifneq ($(words $(BALONG_TOPDIR)),1)
$(warning ************************************************************)
$(warning You are building in a directory whose absolute path contains a space character)
$(warning "$(BALONG_TOPDIR)")
$(warning Please move your source tree to a path that does not contain any spaces.)
$(warning ************************************************************)
$(error Directory names containing spaces not supported)
endif

export BALONG_TOPDIR

# include the define at the top
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

ifeq ($(HUTAF_HLT_COV), true)
include $(BALONG_TOPDIR)/build/tools/linux_avatar_64/HLLT_init_acore.mk
endif

# Used to find valid targets.
MOUDULES_DIR := $(filter-out system,$(shell ls $(BALONG_TOPDIR)/modem))
MOUDULES_FULL_DIR := $(addprefix $(BALONG_TOPDIR)/modem/,$(MOUDULES_DIR))
$(warning $(MOUDULES_DIR))

MOUDULES :=
MOUDULES_NO_APPS_DIR:= $(filter-out apps,$(MOUDULES_DIR))
# group 1, null
# group 2, distcc not suppored
# group 3
MOUDULES += $(if $(findstring ps,$(MOUDULES_NO_APPS_DIR)), gups, )
MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), gumsp, )
MOUDULES += $(if $(findstring drv,$(MOUDULES_DIR)), drv, )

MOUDULES += $(if $(findstring ps,$(MOUDULES_NO_APPS_DIR)), lps, )
MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), lmsp, )

ifeq ($(strip $(OBB_SEPARATE)),true)
MOUDULES := drv
endif

# linux path
OBB_ANDROID_DIR = $(BALONG_TOPDIR)/../../../..
export LINUX_KERNEL  := $(OBB_ANDROID_DIR)/kernel
export OUT_ANDROID   := $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj
export OUT_KERNEL    := $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/KERNEL_OBJ



ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
else
endif

ifeq ($(distcc),true)
export CROSS_COMPILE := /opt/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
export DISTCC_KERNEL := 'CC=distcc $(CROSS_COMPILE)gcc'
endif

ifeq ($(FORTIFY),true)
export CROSS_COMPILE := '$(SOURCEANALYZER) $(CROSS_COMPILE)'
endif


export ARCH          :=arm
COMMON_HEAD          += $(BALONG_TOPDIR)/include/drv/
COMMON_HEAD          += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/
export OUT_ACORE_DIR :=$(OUT_KERNEL)/drivers/modem/balong_oam_ps
#soc only used for drv this moment
COMMON_HEAD          += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc
COMMON_HEAD          += $(BALONG_TOPDIR)/config/product/include/$(CFG_GU_PLATFORM_NEW)/
BALONG_INC           := $(patsubst %,-I%,$(COMMON_HEAD))

ifneq ($(USE_CCACHE),)
BALONG_CROSS_COMPILE :="$(shell pwd)/prebuilts/misc/linux-x86/ccache/ccache $(BALONG_CROSS_COMPILE)"
endif

TTIME ?=time 

#rsync config 
OBC_CLEAN_SYNC_FILE_SH    := $(BALONG_TOPDIR)/modem/drv/build/acore/clean_sync_files.sh
OBC_SYNC_FILE_LIST        := $(BALONG_TOPDIR)/modem/drv/build/acore/$(CFG_OS_ANDROID_SYNC_FILE_LIST)
ifneq ($(CFG_OS_ANDROID_SYNC_FILE_LIST),)
OBC_SYNC                  := rsync -rtL --files-from=$(OBC_SYNC_FILE_LIST)
else
OBC_SYNC                  := rsync -rtL
endif

ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
OBC_LINUX_DEFCONFIG       := $(CFG_OS_ANDROID_TEMP_DECONFIG)
else
OBC_LINUX_DEFCONFIG       := $(CFG_OS_LINUX_PRODUCT_NAME)
endif

OUT_MODEM_DIR             :=$(OUT_KERNEL)/drivers/modem

# This is the default target.  It must be the first declared target.
.PHONY: balong
DEFAULT_GOAL := balong
$(DEFAULT_GOAL):

# componnets
.PHONY: prebuild comm acore lps gups lmsp gumsp drv oam_ps
prebuild :
	$(Q)mkdir -p $(OUT_KERNEL)
	$(Q)mkdir -p $(OUT_ANDROID)
	$(Q)mkdir -p $(OUT_ACORE_DIR)
	$(Q)mkdir -p $(OUT_MODEM_DIR)
comm : prebuild
	$(Q) echo do $@
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)if [ ! -h $(OBB_ANDROID_DIR)/kernel/drivers/modem ]; then $(OBC_SYNC) $(BALONG_TOPDIR)/modem/drv/acore/ $(OBB_ANDROID_DIR)/	; fi
	$(Q)sed -i 's/-Werror[-?]/-W/g' $(LINUX_KERNEL)/Makefile
	$(Q)sed -i 's/-Werror//g' $(LINUX_KERNEL)/Makefile
	$(Q)cat  $(LINUX_KERNEL)/arch/arm/configs/$(CFG_OS_LINUX_PRODUCT_NAME)      > $(LINUX_KERNEL)/arch/arm/configs/$(OBC_LINUX_DEFCONFIG)
	$(Q)cat  $(BALONG_TOPDIR)/$(CFG_OS_PATH)/acore/$(CFG_OS_ANDROID_MODEM_DECONFIG) >> $(LINUX_KERNEL)/arch/arm/configs/$(OBC_LINUX_DEFCONFIG)
	$(Q)sed -i "s/CONFIG_WATCHDOG=y/# CONFIG_WATCHDOG is not set/g" $(LINUX_KERNEL)/arch/arm/configs/$(OBC_LINUX_DEFCONFIG) 
	$(Q)echo  > $(OUT_MODEM_DIR)/Makefile
else
	$(Q)$(OBC_SYNC) $(BALONG_TOPDIR)/modem/drv/acore/ $(OBB_ANDROID_DIR)/	
	$(Q)rsync -rt $(BALONG_TOPDIR)/$(CFG_OS_PATH)/acore/*  $(LINUX_KERNEL)/arch/arm/configs
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/busybox/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/btools/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/ecall/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/exfat-fuse/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/iperf/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/iptables/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/dialup/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/lrz/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/lsz/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/mtd-utils/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/gtr-lcd/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/yaffs2/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/yaffs2_test/ $(OBB_ANDROID_DIR)/external
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/bcm43239/ $(LINUX_KERNEL)/drivers
	$(Q)cp -rf $(BALONG_TOPDIR)/modem/system/external/bcm43241/ $(LINUX_KERNEL)/drivers
endif
	$(Q)$(MAKE) -C $(LINUX_KERNEL) O=$(OUT_KERNEL) ARCH=arm  CROSS_COMPILE=$(CROSS_COMPILE) $(DISTCC_KERNEL) $(OBC_LINUX_DEFCONFIG)
	$(Q)$(MAKE) -C $(LINUX_KERNEL) O=$(OUT_KERNEL) ARCH=arm  CROSS_COMPILE=$(CROSS_COMPILE) $(DISTCC_KERNEL) modules_prepare BALONG_INC="$(BALONG_INC)"
	$(Q)$(MAKE) -C $(LINUX_KERNEL) O=$(OUT_KERNEL) ARCH=arm  CROSS_COMPILE=$(CROSS_COMPILE) $(DISTCC_KERNEL) scripts BALONG_INC="$(BALONG_INC)"

# lte
lps : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/ps/build/tl/APP_CORE 
lmsp : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/oam/build/lt/APP_CORE $(maction)

ifeq ($(OBB_SEPARATE),true)
oam_ps :
	@echo nothing to be done for [$@]
else
oam_ps : lps lmsp gups gumsp
	@echo nothing to be done for [$@]
endif

drv : comm oam_ps
	$(TTIME) $(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/acore $(maction)

# gu
gups : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/ps/build/gu/APP_CORE 
gumsp : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/oam/build/gu/APP_CORE 

$(warning The moudles are $(MOUDULES).)

android : $(MOUDULES)
	@echo nothing to be done for [$@]

# this is the goal.
balong : android 
	@echo balong acore is Ready.

# android independent image
%image %.img : comm
	@echo do [$@]
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/acore $@

%.mk %akefile :
	@echo nothing to be done for [$@]

# 
%:: comm
	$(warning do[$@])
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/acore $@
# clean is so.....................................................
CLEAN_MOUDULES :=
# group 1, null

CLEAN_MOUDULES += $(if $(findstring ps,$(MOUDULES_NO_APPS_DIR)), clean-gups, )
CLEAN_MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), clean-gumsp, )
CLEAN_MOUDULES += $(if $(findstring drv,$(MOUDULES_DIR)), clean-drv, )
ifneq ($(CFG_RAT_MODE),RAT_GU)	
CLEAN_MOUDULES += $(if $(findstring ps,$(MOUDULES_NO_APPS_DIR)), clean-lps, )
CLEAN_MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), clean-lmsp, )
endif
# group 3

ifeq ($(strip $(OBB_SEPARATE)),true)
CLEAN_MOUDULES := $(if $(findstring drv,$(MOUDULES_DIR)), clean-drv, )
endif

.PHONY:clean clean-comm clean-lps clean-gups clean-lmsp clean-gumsp clean-drv clean-android

clean-android:
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR)/acore/android clean
clean-system_img:
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR)/acore/system_img clean

clean-app: clean-system_img
	@echo nothing to be done for [$@] 

clean-drv: clean-system_img
ifneq ($(strip $(CFG_OS_ANDROID_SYNC_FILE_LIST)),)
#	cd $(OBB_ANDROID_DIR) && $(OBC_CLEAN_SYNC_FILE_SH) $(OBC_SYNC_FILE_LIST)
endif
	$(Q)-rm -f $(LINUX_KERNEL)/arch/arm/configs/$(CFG_OS_ANDROID_MODEM_DECONFIG)
	$(Q)-rm -f $(LINUX_KERNEL)/arch/arm/configs/$(OBC_LINUX_DEFCONFIG)
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
#	@-mkdir -p $(LINUX_KERNEL)/drivers/modem
#	@echo > $(LINUX_KERNEL)/drivers/modem/Kconfig
endif
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/drv/build/acore clean

clean-lmsp: clean-app
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/oam/build/lt/APP_CORE clean
clean-lps: clean-app
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/ps/build/tl/APP_CORE clean
clean-gups: clean-app
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/ps/build/gu/APP_CORE clean
clean-gumsp: clean-app
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/oam/build/gu/APP_CORE clean
	
clean-comm:
	@echo complete $@

clean: $(CLEAN_MOUDULES)
	@echo "Clean the targets: $(CLEAN_MOUDULES)."

distclean: clean-comm clean 
	@echo "Entire build directory removed."

.PHONY: help
help:
	@echo make product=hi6930cs_p531_fpga acore
	@echo make product=hi6930cs_p531_fpga clean-acore

.PHONY:cmd
cmd:
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/$(mdir) -f $(mfile) $(goals)
.PHONY: showcommands
showcommands:
	@echo >/dev/null
	
# Used to force goals to build.  Only use for conditionally defined goals.
.PHONY: FORCE
pc-lint-msp:
	$(MAKE) -C $(BALONG_TOPDIR)/modem/oam/build/lt/APP_CORE $(maction)
pc-lint-drv:
	$(MAKE) -C $(BALONG_TOPDIR)/modem/drv/build/acore $(maction)

.PHONY: $(maction)
$(maction): pc-lint-msp pc-lint-drv
	@echo [$@] is done!
FORCE:;
