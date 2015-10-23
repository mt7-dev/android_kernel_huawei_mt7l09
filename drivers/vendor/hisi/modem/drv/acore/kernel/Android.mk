#Android makefile to build kernel as a part of Android Build

KERNEL_DEFCONFIG := balongv7r2_defconfig
KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
KERNEL_CONFIG := $(KERNEL_OUT)/.config
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage
BALONG_ARCH := arm 
BALONG_CROSS_COMPILE :=$(shell pwd)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-

ifneq ($(USE_CCACHE),)
BALONG_CROSS_COMPILE :="$(shell pwd)/prebuilts/misc/linux-x86/ccache/ccache $(BALONG_CROSS_COMPILE)"
endif

ifeq ($(distcc),true)
BALONG_CROSS_COMPILE :=/opt/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
export DISTCC_ANDRIOD := 'CC=distcc $(BALONG_CROSS_COMPILE)gcc'
endif

ifeq ($(FORTIFY),true)
export FORTIFY_CC := 'CC=$(SOURCEANALYZER) $(BALONG_CROSS_COMPILE)gcc'
export FORTIFY_AR := 'AR=$(SOURCEANALYZER) $(BALONG_CROSS_COMPILE)ar'
export FORTIFY_LD := 'LD=$(SOURCEANALYZER) $(BALONG_CROSS_COMPILE)ld'
export FORTIFY_AS := 'AS=$(SOURCEANALYZER) $(BALONG_CROSS_COMPILE)as'
endif

INSTALLED_KERNEL_TARGET :=$(PRODUCT_OUT)/kernel

$(KERNEL_OUT):
	mkdir -p $(KERNEL_OUT)

$(KERNEL_CONFIG): $(KERNEL_OUT)
#	$(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(BALONG_ARCH) CROSS_COMPILE=$(BALONG_CROSS_COMPILE) distclean
	$(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(BALONG_ARCH)  CROSS_COMPILE=$(BALONG_CROSS_COMPILE) $(DISTCC_ANDRIOD) $(KERNEL_DEFCONFIG) \
	$(FORTIFY_CC) $(FORTIFY_AS) $(FORTIFY_AR) $(FORTIFY_LD)

$(TARGET_PREBUILT_KERNEL): $(KERNEL_CONFIG)
#	$(hide) $(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(BALONG_ARCH) CROSS_COMPILE=$(BALONG_CROSS_COMPILE)  -j 18
	$(hide) $(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(BALONG_ARCH) CROSS_COMPILE=$(BALONG_CROSS_COMPILE) $(DISTCC_ANDRIOD) \
	$(FORTIFY_CC) $(FORTIFY_AS) $(FORTIFY_AR) $(FORTIFY_LD) -j 18 zImage
ifeq ($(CFG_CONFIG_ARM_ENABLE_DTS),YES)	
	./$(TOP)/kernel/dts.sh $(KERNEL_OUT) $(TOP) $(CFG_DTS_NAME)
endif

kernelconfig: $(KERNEL_OUT)
	$(MAKE) -C kernel O=../$(KERNEL_OUT) $(KERNEL_DEFCONFIG) menuconfig
