# Copyright 2005 The Android Open Source Project
#
# Android.mk for adb
#

#LOCAL_PATH := $(call my-dir)
#include $(CLEAR_VARS)

#LOCAL_SRC_FILES := \
#	dload.c \
#	hdlc.c \
#	at.c \
#	dload_comm.c \
#	dload_lib.c \
#	dload_show.c \
#	usb_acm.c

#LOCAL_CFLAGS += -D__RECOVERY_IMG__

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../external/oeminfo/lib $(LOCAL_PATH)/.. 
#LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/drv/common/include
#LOCAL_CFLAGS := -O2 -g -DADB_HOST=0 -Wall -Wno-unused-parameter
#LOCAL_CFLAGS += -D_XOPEN_SOURCE -D_GNU_SOURCE

#LOCAL_MODULE := libdload

#include $(BUILD_STATIC_LIBRARY)
