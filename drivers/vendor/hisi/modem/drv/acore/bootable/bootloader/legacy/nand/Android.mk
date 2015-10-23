LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
	nandc/nandc_balong.c  \
	nandc/nandc_cfg.c  \
	nandc/nandc_ctrl.c  \
	nandc/nandc_data.c  \
	nandc/nandc_host.c  \
	nandc/nandc_nand.c  \
	nandc/nand_ids.c  \
	nandc/nandc_native.c \
	nandc/nandc_balong_test.c  \
	ptable/ptable_common.c  \
	ptable/ptable_product.c  \
	ptable/ptable_fastboot.c

ifdef CFG_NANDC_USE_V400
LOCAL_SRC_FILES += nandc/nandc_v400.c
else
LOCAL_SRC_FILES += nandc/nandc_v600.c
endif

# inc
LOCAL_C_INCLUDES := $(call include-path-for, bootloader)

# balong config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include



LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/boot
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/nandc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ptable

#LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_CFLAGS :=  -g -W -Wall
LOCAL_CFLAGS += -march=armv7-a

#
LOCAL_CFLAGS += -D__FASTBOOT__

#LOCAL_MODULE_TAGS := eng

LOCAL_MODULE := libnand

include $(BUILD_RAW_STATIC_LIBRARY)