LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
	modem.c \
	nv_balong.c \
	nv_xml_dec.c \
	balong_modem.l2cache.bin.S
	
ifneq ($(strip $(CFG_BALONG_MODEM)),true)
LOCAL_SRC_FILES := 	dummy.c 
endif

LOCAL_C_INCLUDES := $(call include-path-for, legacy)

LOCAL_C_INCLUDES +=\
    bootable/bootloader/legacy/include\


LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
                    $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu \
                    $(BALONG_TOPDIR)/config/nvim/include/gu \
                    $(BALONG_TOPDIR)/include/drv \
                    $(BALONG_TOPDIR)/include/nv/tl/drv \
                    $(BALONG_TOPDIR)/include/nv/tl/oam \
                    $(BALONG_TOPDIR)/include/nv/tl/lps \
                    $(BALONG_TOPDIR)/include/phy/lphy \
                    $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
                    $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc \
                    $(BALONG_TOPDIR)/modem/drv/common/include

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_ASFLAGS := $(COMMON_ASFLAGS)


LOCAL_MODULE := lib_modem
LOCAL_MODULE_TAGS := optional
include $(BUILD_RAW_STATIC_LIBRARY)


