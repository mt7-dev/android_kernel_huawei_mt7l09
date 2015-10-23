# generic efuse driver support: platform drivers, dedicated expander chips, etc

-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

ifneq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
obj-y	   += efuse_balong.o
obj-$(CONFIG_ENABLE_TEST_CODE) += efuse_balong_test.o
endif

obj-y	   += efuse_comm.o

