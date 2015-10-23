-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

obj-$(CONFIG_MTD_NAND_BALONG_NANDC)    += nandc_mtd.o nandc_nand.o \
										nandc_native.o nandc_cfg.o nandc_ctrl.o nandc_data.o \
										nandc_host.o  nand_ids.o  nandc_balong.o
obj-$(CONFIG_ENABLE_TEST_CODE) += nandc_balong_test.o
ifdef CFG_NANDC_USE_V400
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)    += nandc_v400.o
else
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)    += nandc_v600.o
endif