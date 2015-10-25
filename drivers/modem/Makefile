#
# created by y000184236 for k3v3 fpga
#
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

obj-y				+= analysis/
ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
obj-y				+= ioremap/
endif

obj-y				+= udi/
ifeq ($(strip $(CFG_ENABLE_BUILD_SOCP)),YES)
obj-y				+= socp/
endif
obj-$(CONFIG_BALONG_EARLY_FS)	+= balong_fs/
obj-y				+= nvim/
ifeq ($(strip $(CFG_CONFIG_BALONG_RTC)),YES)
obj-y	+= rtc/
endif
ifeq ($(strip $(CFG_CONFIG_HI6551_RTC)),YES)
obj-y	+= rtc/
endif
ifeq ($(strip $(CFG_CONFIG_HI6559_RTC)),YES)
obj-y	+= rtc/
endif
# 温度保护需要放在ccore加载之前，请勿移动。 
ifeq ($(strip $(CFG_CONFIG_TEMPERATURE_PROTECT)),YES)
obj-y				+= temperature/
endif

obj-$(CONFIG_IMAGE_LOAD) 	+= loadm/
obj-$(CONFIG_SEC_CHECK) += sec/
obj-$(CONFIG_KDF_HASH) += cipher/
obj-y				+= ipc/
obj-y				+= icc/
obj-$(CONFIG_DSP_BALONG) 	+= dsp/
ifeq ($(strip $(CFG_CONFIG_BBP_INT)),YES)
obj-y				+= bbp/
endif
obj-y				+= version/
ifeq ($(strip $(CFG_CONFIG_HKADC)),YES)
obj-$(CONFIG_HKADC_BALONG) 	+= hkadc/
endif
obj-y 	+= onoff/
obj-$(CONFIG_GPIO_BALONG) 	+= gpio/
obj-$(CONFIG_IOS_BALONG) 	+= ios/
ifeq ($(strip $(CFG_CONFIG_EFUSE)),YES)
obj-y 	+= efuse/
endif
obj-$(CONFIG_BALONG_KEYBOARD_GPIO) += keyboard/
ifeq ($(strip $(CFG_CONFIG_ANTEN)),YES)
obj-y				+= antenna/
endif

obj-y				+= adp/
obj-y				+= udi/
ifeq ($(strip $(CFG_ENABLE_BUILD_OM)),YES)
obj-y				+= om/
endif
obj-$(CONFIG_RFILE_SUPPORT)	+= rfile/
obj-y				+= mbxhifi/
ifeq ($(strip $(CFG_CONFIG_MODULE_TIMER)),YES)
obj-y				+= balong_timer/
endif
obj-$(CONFIG_MEM_BALONG)	+= mem/
ifeq ($(strip $(CFG_CONFIG_CSHELL)),YES)
obj-y               += cshell/
endif
obj-y				+= adp/
obj-y				+= sync/
obj-y				+= hwadp/
obj-y				+= reset/
obj-y				+= reset_hifi/
ifeq ($(strip $(CFG_CONFIG_IPF)),YES)
obj-y				+= ipf/
endif
obj-y				+= lowpower_mntn/
obj-$(CONFIG_BALONG_OAM_PS)  += $(if $(findstring .o,$(shell ls $(CURDIR)/drivers/modem/balong_oam_ps)), balong_oam_ps/, )
#ifeq ($(strip $(CFG_CONFIG_DUAL_MODEM)),YES)
#obj-y				+= dual_modem/
#endif
subdir-ccflags-y := -Werror

#HSUART_M2M
ifeq ($(strip $(CFG_CONFIG_M2M_HSUART)),YES)
obj-y  += dialup_hsuart/
endif
KBUILD_CPPFLAGS +=	-I$(BALONG_TOPDIR)/include/nv/comm
KBUILD_CPPFLAGS +=	-I$(BALONG_TOPDIR)/include/med