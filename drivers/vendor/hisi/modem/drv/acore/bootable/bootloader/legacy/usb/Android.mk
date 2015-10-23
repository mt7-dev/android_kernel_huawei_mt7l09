LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := usb3_drv.c usb3_pcd.c usb3_intr.c usb3_pcd_intr.c

LOCAL_C_INCLUDES := $(call include-path-for, bootloader)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/boot
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2
LOCAL_C_INCLUDES += $(LOCAL_PATH)

LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc

LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_CFLAGS += -march=armv6

LOCAL_MODULE := libusb

LOCAL_CFLAGS += -D__FASTBOOT__

include $(BUILD_RAW_STATIC_LIBRARY)