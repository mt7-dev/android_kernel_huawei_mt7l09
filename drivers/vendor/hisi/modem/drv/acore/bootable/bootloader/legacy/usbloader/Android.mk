LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := $(call include-path-for, bootloader mkbootimg)

# balong config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/nv/tl/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include 

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/boot
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nand/ptable
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nandwrite


LOCAL_SRC_FILES := $(TARGET_BOOTLOADER_INIT_FILE) main.c usbloader.c uartloader.c time.c boot.c

#LOCAL_CFLAGS := -O2 -g -W -Wall -fno-common
LOCAL_CFLAGS :=  -g -W -Wall -fno-common
#LOCAL_CFLAGS += -march=armv6
LOCAL_CFLAGS += -march=armv7-a
LOCAL_CFLAGS += -DPRODUCTNAME='"$(strip $(TARGET_BOOTLOADER_BOARD_NAME))"'

# must be deleted!!
LOCAL_CFLAGS += -D__FASTBOOT__

LOCAL_MODULE := fastboot.img 

LOCAL_MODULE_PATH := $(PRODUCT_OUT)
LOCAL_STATIC_LIBRARIES := $(TARGET_BOOTLOADER_LIBS) libboot libboot_c libboot_arch_balongv7r2 libnand libusb nandwrite

include $(BUILD_RAW_EXECUTABLE)

$(LOCAL_BUILT_MODULE) : PRIVATE_LINK_SCRIPT := $(TARGET_BOOTLOADER_LINK_SCRIPT)

# onchip.img
# =====================================
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := $(call include-path-for, bootloader mkbootimg)

# balong config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/nv/tl/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include 

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/boot
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nand/ptable
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nandwrite


LOCAL_SRC_FILES := $(TARGET_BOOTLOADER_INIT_FILE) main.c usbloader.c uartloader.c time.c boot.c

#LOCAL_CFLAGS := -O2 -g -W -Wall -fno-common
LOCAL_CFLAGS :=  -g -W -Wall -fno-common
#LOCAL_CFLAGS += -march=armv6
LOCAL_CFLAGS += -march=armv7-a
LOCAL_CFLAGS += -DPRODUCTNAME='"$(strip $(TARGET_BOOTLOADER_BOARD_NAME))"'

# must be deleted!!
LOCAL_CFLAGS += -D__FASTBOOT__

LOCAL_MODULE := onchip.img 

LOCAL_MODULE_PATH := $(PRODUCT_OUT)
LOCAL_STATIC_LIBRARIES := $(TARGET_BOOTLOADER_LIBS) libboot libboot_c libboot_arch_balongv7r2 libnand libusb nandwrite

include $(BUILD_RAW_EXECUTABLE)

$(LOCAL_BUILT_MODULE) : PRIVATE_LINK_SCRIPT := $(TARGET_ONCHIP_LINK_SCRIPT)

