-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

obj-$(CONFIG_DSP_BALONG)	+= bsp_dsp.o
ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
obj-y	+= bsp_dsp_stress.o
endif