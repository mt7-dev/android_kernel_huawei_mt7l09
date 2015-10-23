# Configuration for Linux on ARM.
# Generating binaries for the ARMv7-a architecture and higher
#
ARCH_ARM_HAVE_THUMB_SUPPORT     := true
ARCH_ARM_HAVE_FAST_INTERWORKING := true
ARCH_ARM_HAVE_64BIT_DATA        := true
ARCH_ARM_HAVE_HALFWORD_MULTIPLY := true
ARCH_ARM_HAVE_CLZ               := true
ARCH_ARM_HAVE_FFS               := true
ARCH_ARM_HAVE_ARMV7A            := true
ARCH_ARM_HAVE_TLS_REGISTER      := true

ifeq ($(strip $(TARGET_ARCH_VARIANT_FPU)),)
ARCH_ARM_HAVE_VFP               := false
else
ARCH_ARM_HAVE_VFP               := true
endif

ifeq ($(strip $(TARGET_ARCH_VARIANT_CPU)),)
TARGET_ARCH_VARIANT_CPU         := cortex-a8
endif

# Note: Hard coding the 'tune' value here is probably not ideal,
# and a better solution should be found in the future.
#
arch_variant_cflags := \
    -march=armv7-a \
	-mtune=$(TARGET_ARCH_VARIANT_CPU)

# vfp
ifeq ($(strip $(ARCH_ARM_HAVE_VFP)),true)
arch_variant_cflags += -mfloat-abi=softfp
arch_variant_cflags += -mfpu=$(TARGET_ARCH_VARIANT_FPU)
else
arch_variant_cflags += -mfloat-abi=soft
endif

ifeq ($(strip $(TARGET_ARCH_VARIANT_CPU)),cortex-a8)
arch_variant_ldflags := \
       -Wl,--fix-cortex-a8
else
arch_variant_ldflags :=
endif

	








