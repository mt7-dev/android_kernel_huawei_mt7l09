# Makefile for staging directory

# fix for build system bug...
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

ifeq ($(strip $(CFG_CONFIG_PMIC_HI6451)),YES)
obj-$(CONFIG_POWER_PMIC_HI6451)	+= hi6451/
endif
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6551)),YES)
obj-$(CONFIG_POWER_PMIC_HI6551)	+= hi6551/
endif

ifeq ($(strip $(CFG_CONFIG_PMIC_HI6559)),YES)
obj-$(CONFIG_POWER_PMIC_HI6559)	+= hi6559/
endif

KBUILD_CPPFLAGS += -Idrivers/pmic

obj-y += pmu_balong.o
obj-y += pmu_debug.o
obj-$(CONFIG_ENABLE_TEST_CODE) += pmu_test.o




