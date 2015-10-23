include $(BALONG_TOPDIR)/build/scripts/make_base.mk

ANDROID_DIR :=$(BALONG_TOPDIR)/ANDROID
PRODUCT_TARGET_DIR=$(ANDROID_DIR)/local_bin/$(OBB_PRODUCT_NAME)

all:
	$(Q)-rm -rf ${Q} $(PRODUCT_TARGET_DIR)/bin/system
	$(Q)cd $(PRODUCT_TARGET_DIR)/bin/ && unzip system.zip
	$(Q)-cp -fp $(V) $(PRODUCT_TARGET_DIR)/bin/*.ko $(PRODUCT_TARGET_DIR)/bin/system/bin/
	$(Q)cd $(PRODUCT_TARGET_DIR)/bin/ && ./mkyaffs2image ./system ./system.img
	
clean:
	$(Q)-rm -rf ${Q} $(PRODUCT_TARGET_DIR)/bin/system
	$(Q)-rm -rf ${Q} $(PRODUCT_TARGET_DIR)/bin/system.img

	
