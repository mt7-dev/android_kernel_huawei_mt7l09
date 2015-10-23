
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the Top dir
BALONG_TOPDIR ?= $(patsubst %/drv/build/mcore,%,$(CURDIR))

# Make sure that there are no spaces in the absolute path; the build system can't deal with them.
ifneq ($(words $(BALONG_TOPDIR)),1)
$(warning ************************************************************)
$(warning You are building in a directory whose absolute path contains a space character)
$(warning "$(BALONG_TOPDIR)")
$(warning Please move your source tree to a path that does not contain any spaces.)
$(warning ************************************************************)
$(error Directory names containing spaces not supported)
endif

# include the define at the top
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

OBC_SYNC_FILE_LIST        := $(BALONG_TOPDIR)/modem/drv/build/mcore/$(CFG_OS_ANDROID_SYNC_FILE_LIST)
OBC_LPM3_TOOLCHAIN        := $(OBB_ANDROID_DIR)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
OBC_LPM3_SRC_PATH         := $(BALONG_TOPDIR)/lowpowerM3
# process
.PHONY:all mcore raminit bootrom bootloader
all: mcore raminit bootrom bootloader
	@echo do [$@]

mcore :
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
ifeq ($(OBC_LPM3_SRC_PATH),$(wildcard $(OBC_LPM3_SRC_PATH)))
	$(Q)if [ ! -h $(OBC_LPM3_SRC_PATH)/driver/modem ]; then rsync -rtL --files-from=$(OBC_SYNC_FILE_LIST) $(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/ $(OBC_LPM3_SRC_PATH)/driver/modem/; fi
	MAKEFLAGS= ./build_k3lpm3.sh $(OBB_ANDROID_DIR) $(CFG_OS_ANDROID_PRODUCT_NAME) $(CFG_OS_ANDROID_MODE)
	$(Q)cp -f $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/lpm3.img $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)cp -f $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/LPM3_OBJ/lpm3* $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	$(Q)cp -f $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/lpm3.img $(OBB_ANDROID_DIR)/device/hisi/keylibs/
else
	@echo nothing to be done for lpm3 build, no access right to lpm3 repository.
endif
else
	$(MAKE)  $(OBB_JOBS)  -f makefile_mcore.mk
endif
$(maction):
	$(MAKE)  $(OBB_JOBS)  -f makefile_mcore.mk $(maction)
	@echo mcore $(maction) end

raminit : 
ifeq ($(strip $(CFG_BSP_USB_BURN)),YES)
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/drv/mcore/ramInit/
endif
bootrom :
	@echo do [$@]

bootloader : 
ifneq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(MAKE)  $(OBB_JOBS)  -f makefile_bootloader.mk
endif

# clean
.PHONY: clean
clean:
	$(MAKE) $(OBB_JOBS) -f makefile_mcore.mk $@
	$(MAKE) $(OBB_JOBS) -f makefile_bootloader.mk $@
	-$(MAKE) $(OBB_JOBS) -C $(OBC_LPM3_SRC_PATH)/ $@
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/lpm3*
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/librtx.a
	$(Q)-rm -f $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/LPM3_OBJ
	$(Q)-cd $(OBB_ANDROID_DIR)/device/hisi/keylibs/ && git checkout .
	$(Q)echo "COMM_CODE clean complete."
