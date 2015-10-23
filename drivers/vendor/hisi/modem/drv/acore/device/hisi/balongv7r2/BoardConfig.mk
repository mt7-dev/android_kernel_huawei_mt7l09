# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk


TARGET_ARCH :=arm

# The generic product target doesn't have any hardware-specific pieces.
TARGET_BOARD_PLATFORM := balongv7r2
TARGET_BOOTLOADER_BOARD_NAME := balongv7r2
TARGET_BOOTLOADER_LINK_SCRIPT := bootable/bootloader/legacy/usbloader/boot_fastboot_temp.ld
TARGET_ONCHIP_LINK_SCRIPT := bootable/bootloader/legacy/usbloader/boot_onchip_temp.ld
TARGET_BOOTLOADER_INIT_FILE := init_fastboot.S

TARGET_NO_BOOTLOADER := false
TARGET_NO_KERNEL := false
KERNEL_DEFCONFIG := hisi_balongv7r2_defconfig

BOARD_KERNEL_CMDLINE :=root=/dev/ram0 rw console=ttyAMA0,115200 console=uw_tty0,115200 rdinit=/init loglevel=5 mem=$(CFG_DDR_ACORE_SIZE)

ifdef CFG_FLASH_PAGE_SIZE
BOARD_MKBOOTIMG_ARGS :=--pagesize $(CFG_FLASH_PAGE_SIZE)
endif


TARGET_CPU_ABI := armeabi
BOARD_USES_GENERIC_AUDIO := true



# Set /system/bin/sh to mksh, not ash, to test the transition.
#TARGET_SHELL := mksh

#BUILD_TINY_ANDROID ?= true
#TARGET_CPU_ABI := armeabi-v7a
#TARGET_CPU_ABI2 := armeabi

# arch combo
TARGET_ARCH_VARIANT := armv7-a
TARGET_CPU_SMP :=false
ARCH_ARM_HAVE_TLS_REGISTER      := true
#TARGET_ARCH_VARIANT_FPU :=
TARGET_ARCH_VARIANT_CPU :=cortex-a9


# define usb function
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	persist.sys.usb.config=ptp

# firmware
PRODUCT_COPY_FILES += \

    
# busybox copy to sytem/bin
PRODUCT_COPY_FILES += \
	external/busybox/busybox:system/bin/busybox \
	external/iperf/iperf:system/bin/iperf \
	device/hisi/balongv7r2/mdev.conf:system/etc/mdev.conf \
	device/hisi/balongv7r2/mdev.sh:system/etc/mdev.sh \
	device/hisi/balongv7r2/mass_boot.sh:system/etc/mass_boot.sh \
	external/iperf/perf:system/bin/perf \
	external/dialup/dialup:system/bin/dialup \
	external/mtd-utils/mtd-utils/flash_eraseall:system/bin/flash_eraseall \
	external/iptables/iptables.bin:system/bin/iptables 	
	
# kernel
PRODUCT_COPY_FILES += \
	


PRODUCT_BRAND := balongv7r2
PRODUCT_LOCALES := hdpi mdpi
PRODUCT_LOCALES += zh_CN en_US es_US

USE_CAMERA_STUB := true

BOARD_KERNEL_BASE := $(CFG_PRODUCT_CFG_KERNEL_ENTRY)  # $(ACORE_BOOTLOADER_REL)  #"OS_RAM_START+OS_LOAD_OFFSET + ACPU_OFFSET" in fastboot
#$(warning ****************************$(BOARD_KERNEL_BASE)**********************************)
BOARD_KERNEL_PAGESIZE := 2048


