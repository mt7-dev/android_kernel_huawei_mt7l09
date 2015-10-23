LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
	flash.c \
	poll.c \
	tags_partition.c \
	tags_revision.c \
	tags_serialno.c \
	tags_cmdline.c \
	gpio_keypad.c \
	init.c \
	tags.c

LOCAL_C_INCLUDES := $(call include-path-for, bootloader)

# balong config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv \
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) 
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/boot
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2

LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_CFLAGS += -march=armv6
# must be deleted!!
LOCAL_CFLAGS += -D__FASTBOOT__

LOCAL_MODULE := libboot

include $(BUILD_RAW_STATIC_LIBRARY)
