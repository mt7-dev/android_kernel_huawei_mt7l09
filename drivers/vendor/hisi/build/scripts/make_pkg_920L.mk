include $(BALONG_TOPDIR)/build/scripts/make_base.mk

PACK_HEAD_TOOL          :=$(BALONG_TOPDIR)/build/tools/utility/postlink/packHead/packHead.py

ifneq ($(strip $(OBB_CORE_NAME)),all)
PKG_DEPENDS             := $(OBB_CORE_NAME)_pkgs
else
#PKG_DEPENDS             := acore_pkgs ccore_pkgs mcore_pkgs
PKG_DEPENDS             := acore_pkgs  ccore_pkgs 
endif

VRL_TOOL_NAME           :=vrl_creater_for_k3v3
K3V3_VRL_TOOL_PATH      :=$(BALONG_TOPDIR)/../thirdparty/hisi/$(VRL_TOOL_NAME)
SEC_VRL_TOOL_PATH       :=$(OBB_PRODUCT_DELIVERY_DIR)/obj/vrl/$(VRL_TOOL_NAME)

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
acore_pkgs:

	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/*.img        $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-cp -fp $(BALONG_TOPDIR)/build/tools/idt/*.xml        $(OBB_PRODUCT_DELIVERY_DIR)/image/
	
	$(Q)-cp -fp $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_hi3630.bat        $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-cp -fp $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_sec_hi3630.bat        $(OBB_PRODUCT_DELIVERY_DIR)/image/

	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/*.elf       $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/LPM3_OBJ/lpm3.elf       $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/*.out       $(OBB_PRODUCT_DELIVERY_DIR)/lib/	
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/*.map       $(OBB_PRODUCT_DELIVERY_DIR)/lib/		
	$(Q)-cp -fp $(OBB_ANDROID_DIR)/out/target/product/$(CFG_OS_ANDROID_PRODUCT_NAME)/obj/KERNEL_OBJ/vmlinux      $(OBB_PRODUCT_DELIVERY_DIR)/lib/		

ccore_pkgs:sec_tool
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)/balong_modem.bin

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

	$(Q)-cp -f $(OBB_PRODUCT_DELIVERY_DIR)/image/balong_modem.bin $(SEC_VRL_TOOL_PATH)/balong_modem.bin
	$(Q)chmod 777 $(SEC_VRL_TOOL_PATH) -R && cd $(SEC_VRL_TOOL_PATH) && ./make_modem.sh
	$(Q)-mv -f $(SEC_VRL_TOOL_PATH)/sec_balong_modem.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/sec_balong_modem.bin
	
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)/balong_modem.bin
# clean
.PHONY: clean
clean:
	$(Q)-rm -rf $(SEC_VRL_TOOL_PATH)
	$(Q)-rm -f $(VRL_ENC_XLOADER_FILE)
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/image/*.img
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
