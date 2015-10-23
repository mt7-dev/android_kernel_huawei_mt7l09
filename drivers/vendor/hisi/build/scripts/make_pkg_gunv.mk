
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# user target: make yaffs0.img		
LOCAL_ROOT	?=$(OBB_PRODUCT_DELIVERY_DIR)/lib

GU_NV_DIR         := $(BALONG_TOPDIR)/$(CFG_GU_NV_PATH)
GU_NV_FILES       := $(shell ls $(GU_NV_DIR))
GU_NV_XMLS			:=$(shell ls $(BALONG_TOPDIR)/$(CFG_GU_NV_XML_PATH))

CFG_YAFFS0_IMG_FILES	?= $(GU_NV_FILES) $(GU_NV_XMLS)
CFG_YAFFS0_IMG_FILES	:=$(addprefix $(LOCAL_ROOT)/,$(CFG_YAFFS0_IMG_FILES))

# do user target
gunv:$(LOCAL_ROOT)/yaffs0.img

#pragma runlocal
$(LOCAL_ROOT)/yaffs0.img : $(CFG_YAFFS0_IMG_FILES)
	$(Q)mkdir -p  $(OBB_PRODUCT_DELIVERY_DIR)/lib/yaffs0/Nvim 
	$(Q)cp $(CFG_YAFFS0_IMG_FILES) $(OBB_PRODUCT_DELIVERY_DIR)/lib/yaffs0/Nvim
	$(Q)$(BALONG_TOPDIR)/modem/system/android/android_4.2_r1/out/host/linux-x86/bin/mkyaffs2image $(OBB_PRODUCT_DELIVERY_DIR)/lib/yaffs0/Nvim $(LOCAL_ROOT)/yaffs0.img
