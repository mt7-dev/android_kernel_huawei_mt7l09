# Makefile for staging directory

# fix for build system bug...
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

ifeq ($(strip $(CFG_CONFIG_BALONG_RTC)),YES)
obj-y	+= rtc_balong/
endif
ifeq ($(strip $(CFG_CONFIG_HI6551_RTC)),YES)
obj-y	+= rtc_hi6551/
endif
ifeq ($(strip $(CFG_CONFIG_HI6559_RTC)),YES)
obj-y	+= rtc_hi6559/
endif




