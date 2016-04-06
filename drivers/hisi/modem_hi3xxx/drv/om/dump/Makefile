#
# Makefile for the memory technology device drivers.
#
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
# Core functionality.
ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
obj-y				+= bsp_dump_k3.o 
else
obj-y				+= bsp_dump.o 
endif
