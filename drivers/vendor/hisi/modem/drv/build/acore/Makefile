include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# wifi

ifeq ($(strip $(CFG_FEATURE_WIFI_ON)),YES)
  WIFI_TARGETS := bsp_wifi 
  ifeq ($(strip $(CFG_FEATURE_BCM_VERSION)),43241)
    BCM_VERSION := bcm43241
  else
    ifeq ($(strip $(CFG_FEATURE_BCM_VERSION)),43239)
	  BCM_VERSION := bcm43239
	else
	  $(error bcm version error)
	endif
  endif
endif

export ARCH=arm
export DHDARCH=arm
export VERBOSE=1
export LINUXDIR=$(LINUX_KERNEL)
WIFI_PATH :=$(LINUX_KERNEL)/drivers/$(BCM_VERSION)
WIFI_BRCM_PATH_DRIVER :=$(WIFI_PATH)/wifi_brcm
WIFI_PATH_OUT :=$(LINUX_KERNEL)/../out/target/product/balongv7r2/obj/KERNEL_OBJ/drivers
WIFI_BRCM_PTAH := $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system/bin/wifi_brcm
RFILE_PTAH := $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system/rfile
export LINUXVER=3.4.5

#链接脚本需要使用, define文件里的内容替换生成的temp文件
ORG_FASTBOOT_LINK_FILE  := $(BALONG_TOPDIR)/modem/drv/acore/bootable/bootloader/legacy/usbloader/boot_fastboot.ld
ORG_ONCHIP_LINK_FILE    := $(BALONG_TOPDIR)/modem/drv/acore/bootable/bootloader/legacy/usbloader/boot_onchip.ld
ORG_MAKE_BOOT_FILE      := $(BALONG_TOPDIR)/modem/drv/acore/kernel/arch/arm/mach-hi6930/Makefile.boot
USE_FASTBOOT_LINK_FILE  := $(OBB_ANDROID_DIR)/bootable/bootloader/legacy/usbloader/boot_fastboot_temp.ld
USE_ONCHIP_LINK_FILE    := $(OBB_ANDROID_DIR)/bootable/bootloader/legacy/usbloader/boot_onchip_temp.ld
USE_MAKE_BOOT_FILE      := $(OBB_ANDROID_DIR)/kernel/arch/arm/mach-hi6930/Makefile_temp.boot

OBJCPY = $(OBB_ANDROID_DIR)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy

# todo: 如果V7R2的android和K3的一样，使用"normal" 这个android_mode是无法区分编译K3还是V7R2的
ifeq ($(strip $(CFG_OS_ANDROID_MODE)),normal)
OBC_MAKE_ANDROID_CHOISE :=K3_ANDROID
else
OBC_MAKE_ANDROID_CHOISE :=V7R2_ANDROID
endif

.PHONY:all pre_build bsp_kernel bsp_android bsp_wifi
all: bsp_system_zip
	@echo do [$@]
# nv
pre_build:
	$(Q)tar -cf $(OBB_PRODUCT_DELIVERY_DIR)/lib/scripts.tar  -C $(BALONG_TOPDIR)/modem/drv/common/scripts .

bsp_kernel:pre_build
ifneq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)sed -e 's/CFG_FASTBOOT_ENTRY/$(CFG_FASTBOOT_ENTRY)/g' -e 's/CFG_FASTBOOT_DDR_ENTRY/$(CFG_FASTBOOT_DDR_ENTRY)/g' $(ORG_FASTBOOT_LINK_FILE) > $(USE_FASTBOOT_LINK_FILE) 
	$(Q)sed -e 's/CFG_ONCHIP_FASTBOOT_ADDR/$(CFG_ONCHIP_FASTBOOT_ADDR)/g' -e 's/CFG_PRODUCT_CFG_FLASH_RECOVERY_LEN/$(CFG_PRODUCT_CFG_FLASH_RECOVERY_LEN)/g' $(ORG_ONCHIP_LINK_FILE) > $(USE_ONCHIP_LINK_FILE) 
	$(Q)sed 's/CFG_PRODUCT_CFG_KERNEL_ENTRY/$(CFG_PRODUCT_CFG_KERNEL_ENTRY)/g' $(ORG_MAKE_BOOT_FILE) > $(USE_MAKE_BOOT_FILE)
endif

$(WIFI_TARGETS):pre_build 
	$(Q)$(MAKE) -s -C $(WIFI_PATH)/open-src/src/dhd/linux/ dhd-cdc-usb-hsic-gpl $(OBB_JOBS) $(DISTCC_KERNEL) LINUXVER=3.4.5
	$(Q)cp -pf $(WIFI_PATH)/open-src/src/dhd/linux/dhd-cdc-usb-hsic-gpl-$(LINUXVER)/dhd.ko $(WIFI_PATH)/wifi_brcm/driver/dhd.ko
	$(Q)mkdir -p $(WIFI_PATH_OUT)
	$(Q)cp -rf $(WIFI_PATH)/ $(WIFI_PATH_OUT)
	
bsp_wifi_testmode:pre_build 
	$(Q)$(MAKE) -s -C $(WIFI_PATH)/open-src_test_mode/src/dhd/linux/ dhd-cdc-usb-hsic-gpl $(OBB_JOBS) $(DISTCC_KERNEL)
	$(Q)cp -pf $(WIFI_PATH)/open-src_test_mode/src/dhd/linux/dhd-cdc-usb-hsic-gpl-$(LINUXVER)/dhd.ko $(WIFI_PATH)/wifi_brcm/driver/dhd_test_mode.ko
	$(Q)cp -pf $(WIFI_PATH)/open-src_test_mode/src/dhd/linux/dhd-cdc-usb-hsic-gpl-$(LINUXVER)/dhd.ko $(PRODUCT_TARGET_DIR)/bin/dhd_test_mode.ko
	
bsp_android: bsp_kernel
	MAKEFLAGS= ./build_android.sh $(OBB_ANDROID_DIR) $(CFG_OS_ANDROID_PRODUCT_NAME) $(OBC_MAKE_ANDROID_CHOISE)
	$(Q)-rm -rf $(USE_FASTBOOT_LINK_FILE)
	$(Q)-rm -rf $(USE_ONCHIP_LINK_FILE)
	$(Q)-rm -rf $(USE_MAKE_BOOT_FILE)

bsp_system_zip:bsp_android $(WIFI_TARGETS)
	echo do [$@]
	$(Q)mkdir -p $(RFILE_PTAH)
	$(Q)mkdir -p $(WIFI_BRCM_PTAH)
	$(Q)-cp -rf $(WIFI_BRCM_PATH_DRIVER) $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system/bin/
ifneq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)$(OBB_ANDROID_DIR)/out/host/linux-x86/bin/mkyaffs2image $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system.img
endif

ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)cd $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/ && mkdir -p modem_fs/yaffs0/SC/Pers
	$(Q)$(OBB_ANDROID_DIR)/out/host/linux-x86/bin/make_ext4fs -s -l 12582912 -a modemom $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/modem_fs.img $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/modem_fs 
endif

ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)cd $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/ && mkdir -p modem_nv/modem_nv modem_nv/SC/Pers
	$(Q)$(OBB_ANDROID_DIR)/out/host/linux-x86/bin/make_ext4fs -s -l 8388608 -b 1024 -a modemnv $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/modem_nv.img $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/modem_nv 
endif

ifneq ($(strip $(tiny)),true)
	$(Q)cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system.img $(OBB_PRODUCT_DELIVERY_DIR)/image/system.img
endif


# android independent image
%image %.img : bsp_kernel
	@echo do [$@]
	MAKEFLAGS= ./build_android.sh $(OBB_ANDROID_DIR) $(CFG_OS_ANDROID_PRODUCT_NAME) $(OBC_MAKE_ANDROID_CHOISE) $@

%.mk %akefile :
	@echo nothing to be done for [$@]

force:
	@echo nothing to be done for [$@]
# 
%:: bsp_kernel
	$(warning do[$@])
	MAKEFLAGS= ./build_android.sh $(OBB_ANDROID_DIR) $(CFG_OS_ANDROID_PRODUCT_NAME) $(OBC_MAKE_ANDROID_CHOISE) $@

.PHONY:clean
clean: do_clean
	$(Q)$(MAKE) -s -C $(OBB_ANDROID_DIR) clean 
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/system.img 
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/scripts.tar

do_clean:
	@echo nothing to be done for [$@]

#pclint
ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/android_hi3630.lnt
PCLINT_INC_FILE				:=$(BALONG_TOPDIR)/modem/drv/build/acore/pclint_inc_hi3630.lnt
PCLINT_SRC_PLATFORM_FILE	:=$(BALONG_TOPDIR)/modem/drv/build/acore/pclint_src_hi3630.lnt
else
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/android.lnt
PCLINT_SRC_PLATFORM_FILE	:=$(BALONG_TOPDIR)/modem/drv/build/acore/pclint_src_hi6930.lnt
PCLINT_INC_FILE				:=$(BALONG_TOPDIR)/modem/drv/build/acore/pclint_inc.lnt
endif
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/android.lnt
PCLINT_INC_VAR				:=
PCLINT_SRC_FILE				:=$(BALONG_TOPDIR)/modem/drv/build/acore/pclint_src.lnt
PCLINT_CC_OPTION			:=
OBC_LOCAL_MOUDLE_NAME		?=bsp_acore
.PHONY: $(maction)
$(maction) : do_$(maction)
	@echo acore $(maction) is ready.

ifneq ($(strip $(maction)),)
OBC_USE_NEW_PC_LINT            :=true
ifeq ($(strip $(OBC_USE_NEW_PC_LINT)),false)
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules.mk
else
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/android.lnt
include makefile_flexelint_common_files.mk
include makefile_flexelint_files_$(CFG_PLATFORM).mk
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules_new.mk
endif
endif
