LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := $(call include-path-for, bootloader mkbootimg)
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config 
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include 
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc

LOCAL_SRC_FILES := init.S jtag.c nandwrite.c

LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_CFLAGS += -march=armv6

LOCAL_MODULE := nandwrite

LOCAL_MODULE_PATH := $(PRODUCT_OUT)
LOCAL_STATIC_LIBRARIES := $(TARGET_BOOTLOADER_LIBS) libboot libboot_c 

include $(BUILD_RAW_STATIC_LIBRARY)

$(LOCAL_BUILT_MODULE) : PRIVATE_LINK_SCRIPT := $(TARGET_BOOTLOADER_LINK_SCRIPT)
