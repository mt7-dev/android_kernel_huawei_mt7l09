include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# Packet Tool Configuration
PACK_HEAD_TOOL          :=$(BALONG_TOPDIR)/build/tools/utility/postlink/packHead/packHead.py
USBLOADER_TOOL          :=$(BALONG_TOPDIR)/build/tools/utility/postlink/merge/merge.py
K3V3_VRL_TOOL_PATH      :=$(BALONG_TOPDIR)/../thirdparty/tools/vrl_creater_for_k3v3
SEC_VRL_TOOL_PATH       :=$(OBB_PRODUCT_DELIVERY_DIR)/obj/vrl/vrl_creater_for_k3v3
VRL_XLOADER_TBL         :=$(BALONG_TOPDIR)/build/tools/vrl_creater_for_k3v3_xloader/xloader.tbl
VRL_RAW_XLOADER_FILE    :=$(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/xloader.img
VRL_ENC_XLOADER_FILE    :=$(OBB_PRODUCT_DELIVERY_DIR)/image/sec_xloader.img
OBJCPY = $(OBB_ANDROID_DIR)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-objcopy
ifneq ($(strip $(OBB_CORE_NAME)),all)
PKG_DEPENDS             := $(OBB_CORE_NAME)_pkgs
else
PKG_DEPENDS             := acore_pkgs ccore_pkgs mcore_pkgs
endif
# rules
#*******************************************************************************
.PHONY:all $(PKG_DEPENDS)
all: $(PKG_DEPENDS)
	@echo do [$@]
sec_tool:
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	@echo rebuiling the secure signature tool
	$(Q)rm -rf $(SEC_VRL_TOOL_PATH)
	$(Q)mkdir -p $(SEC_VRL_TOOL_PATH)
	$(Q)-cp -rfp $(K3V3_VRL_TOOL_PATH)/* $(SEC_VRL_TOOL_PATH)/
	$(Q) cd $(SEC_VRL_TOOL_PATH)/utils/src/secure_boot_utils  && make clean && make
else
	@echo do nothing
endif

acore_pkgs:sec_tool
#K3V3和v7r2差异
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)-cp -fp $(VRL_RAW_XLOADER_FILE) $(SEC_VRL_TOOL_PATH)/
	$(Q) cd $(SEC_VRL_TOOL_PATH) && ./make_xloader.sh
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/*.img        $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-cp -fp $(BALONG_TOPDIR)/build/tools/idt/*.xml        $(OBB_PRODUCT_DELIVERY_DIR)/image/

else
#对镜像包头进行处理
ifeq ($(CFG_BSP_ENBALE_PACK_IMAGE),YES)
	$(Q)python $(PACK_HEAD_TOOL) kernel $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/boot.img $(OBB_PRODUCT_DELIVERY_DIR)/image/boot.img $(CFG_PRODUCT_CFG_KERNEL_ENTRY)
else
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/boot.img $(OBB_PRODUCT_DELIVERY_DIR)/image/
endif
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/onchip.img $(OBB_PRODUCT_DELIVERY_DIR)/lib/onchip.img
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/EXECUTABLES/onchip.img_intermediates/onchip.img.elf $(OBB_PRODUCT_DELIVERY_DIR)/lib/onchip.img.elf
	$(Q)$(OBJCPY) -O binary -g $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/KERNEL_OBJ/drivers/mtd/nand/ptable/ptable_product.o $(OBB_PRODUCT_DELIVERY_DIR)/image/ptable.bin
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/host/windows-x86/obj/EXECUTABLES/fastboot_intermediates/fastboot.exe $(OBB_PRODUCT_DELIVERY_DIR)/lib/fastboot.exe
endif
	$(Q)-cp -f  $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_$(CFG_PLATFORM).bat $(OBB_PRODUCT_DELIVERY_DIR)/image/burn.bat
	$(Q)-cp -f  $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_sec_$(CFG_PLATFORM).bat $(OBB_PRODUCT_DELIVERY_DIR)/image/burn_sec.bat
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/recovery.img $(OBB_PRODUCT_DELIVERY_DIR)/image/recovery.img
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/EXECUTABLES/fastboot.img_intermediates/fastboot.img $(OBB_PRODUCT_DELIVERY_DIR)/image/fastboot.img
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/EXECUTABLES/fastboot.img_intermediates/fastboot.img.elf $(OBB_PRODUCT_DELIVERY_DIR)/lib/fastboot.img.elf
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/system.img $(OBB_PRODUCT_DELIVERY_DIR)/image/system.img
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/KERNEL_OBJ/vmlinux $(OBB_PRODUCT_DELIVERY_DIR)/lib/vmlinux
#先贴后烧镜像处理
ifeq ($(strip $(CFG_BSP_USB_BURN)),YES)
	$(Q)-python $(USBLOADER_TOOL)  $(OBB_PRODUCT_DELIVERY_DIR)/lib/ramInit.bin  0x0  $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/onchip.img  $(CFG_ONCHIP_FASTBOOT_ADDR) \
		$(OBB_PRODUCT_DELIVERY_DIR)/lib/usbloader.bin
endif
	@echo Do [$@] complete

ccore_pkgs:sec_tool
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)/balong_modem.bin
#对镜像包头进行处理
ifeq ($(CFG_BSP_ENBALE_PACK_IMAGE),NO)
$(Q)-mv -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin
else
ifeq ($(strip $(CFG_CONFIG_COMPRESS_CCORE_IMAGE)),YES)
ifeq ($(strip $(CFG_ROM_COMPRESS)),YES)
	$(Q)-python $(PACK_HEAD_TOOL) VXWORKS $(OBB_PRODUCT_DELIVERY_DIR)/lib/balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(CFG_MCORE_TEXT_START_ADDR_COMPRESSED) DEFLATE
else
	$(Q)-python $(PACK_HEAD_TOOL) VXWORKS $(OBB_PRODUCT_DELIVERY_DIR)/lib/balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(CFG_MCORE_TEXT_START_ADDR) DEFLATE
endif
else
ifeq ($(strip $(CFG_ROM_COMPRESS)),YES)
	$(Q)-python $(PACK_HEAD_TOOL) VXWORKS $(OBB_PRODUCT_DELIVERY_DIR)/lib/balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(CFG_MCORE_TEXT_START_ADDR_COMPRESSED)
else
	$(Q)-python $(PACK_HEAD_TOOL) VXWORKS $(OBB_PRODUCT_DELIVERY_DIR)/lib/balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(CFG_MCORE_TEXT_START_ADDR)
endif	
endif
endif

ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	$(Q)-cp -f $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(SEC_VRL_TOOL_PATH)/balong_modem.bin
	$(Q)cd $(SEC_VRL_TOOL_PATH) && ./make_modem.sh
	$(Q)-mv -f $(SEC_VRL_TOOL_PATH)/sec_balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/sec_balong_modem.bin
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)/balong_modem.bin
endif
	@echo Do [$@] complete

mcore_pkgs:sec_tool
#K3V3和v7r2差异
ifneq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
ifeq ($(CFG_BSP_ENBALE_PACK_IMAGE),NO)
#对镜像包头进行处理
	$(Q)-mv -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/bsp_mcore.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/bsp_mcore.bin
else
	$(Q)-python $(PACK_HEAD_TOOL) m3image $(OBB_PRODUCT_DELIVERY_DIR)/lib/bsp_mcore.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/bsp_mcore.bin $(CFG_RTX_KERNEL_ENTRY)
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/m3boot.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
endif
endif
	@echo Do [$@] complete

# clean
.PHONY: clean
clean:
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)
	$(Q)-rm -f $(VRL_ENC_XLOADER_FILE)
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/dt.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/lpm3.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/sensorhub.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/trustedcore.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/xloader.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/boot.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/bsp_mcore.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/usbloader.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/recovery.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/fastboot.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/system.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/userdata.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/ptable.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/onchip.img
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/ptable.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/fastboot.img.elf
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/vmlinux
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/lphy.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/nv.bin
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/ucos.bin
