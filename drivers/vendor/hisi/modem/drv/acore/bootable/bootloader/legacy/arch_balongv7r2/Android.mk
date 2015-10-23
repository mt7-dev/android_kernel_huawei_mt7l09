LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

LOCAL_ARM_MODE := arm

# nand
NAND_ROOT_DIR :=kernel/drivers/mtd/nand
LINUX_KERNEL_ROOT_DIR :=kernel

# src
LOCAL_SRC_FILES := \
	board.c \
	serial.c \
	nand.c \
	dwc_usb3.c \
	spinLockamp.S \
	p531_fpga_board.c \
	console.c \
	ios.c \
	spi.c \
	gpio.c \
	efuse.c\
	efuse_nandc.c \
	i2c.c \
	version.c \
	boardid.c \
	preboot.c  \
	display.c \
	ddm_phase.c \
	ios_list.c \
	nv_boot.c\
	clk_fastboot.c \
	cipher.c \
	nnex.c \
	mtcmos_fastboot.c

#tsensor
ifeq ($(strip $(CFG_CONFIG_TSENSOR)),YES)
LOCAL_SRC_FILES += tsensor_balong.c
endif

ifeq ($(strip $(CFG_BSP_CONFIG_EDA)),YES)
LOCAL_SRC_FILES += bsp_abb.c
endif


ifeq ($(strip $(CFG_CONFIG_PCIE_CFG)),YES)
LOCAL_SRC_FILES += pcie_balong_fastboot.c
endif

#pmu
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6451)),YES)
LOCAL_SRC_FILES += pmic_hi6451.c
endif
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6551)),YES)
LOCAL_SRC_FILES += pmic_hi6551_regtable.c
LOCAL_SRC_FILES += pmic_hi6551.c
LOCAL_SRC_FILES += dr_hi6551.c
endif
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6559)),YES)
LOCAL_SRC_FILES += pmic_hi6559_regtable.c
LOCAL_SRC_FILES += pmic_hi6559.c
LOCAL_SRC_FILES += dr_hi6559.c
endif
LOCAL_SRC_FILES += pmu.c

ifeq ($(strip $(CFG_CONFIG_DSP)),YES)
LOCAL_SRC_FILES += bsp_dsp.c
endif

#hkadc
ifeq ($(strip $(CFG_CONFIG_HKADC)),YES)
LOCAL_SRC_FILES += bsp_hkadc.c
endif

#coul
LOCAL_SRC_FILES += coul.c

#lcd
ifeq ($(strip $(CFG_CONFIG_FB_SPI_BALONG)),YES)
LOCAL_SRC_FILES += \
	lcd_spi.c
endif
ifeq ($(strip $(CFG_CONFIG_FB_EMI_BALONG)),YES)
LOCAL_SRC_FILES += \
	lcd_emi.c
endif

# inc
LOCAL_C_INCLUDES := $(call include-path-for, bootloader)

# balong config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/include/nv/tl/drv
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc
LOCAL_C_INCLUDES += $(BALONG_TOPDIR)/modem/drv/common/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/balongv7r2/gpio_config
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nand/nandc $(LOCAL_PATH)/../nand/ptable
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../usb

LOCAL_CFLAGS := -O2 -g -W 
LOCAL_CFLAGS += -march=armv7-a

#
LOCAL_CFLAGS += -D__FASTBOOT__

#LOCAL_MODULE_TAGS := eng

LOCAL_MODULE := libboot_arch_balongv7r2

include $(BUILD_RAW_STATIC_LIBRARY)

