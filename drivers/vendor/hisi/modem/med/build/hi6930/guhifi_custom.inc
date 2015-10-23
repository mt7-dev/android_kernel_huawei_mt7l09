####################################################################################################
#
####################################################################################################

####################################################################################################
#Directories for include files
####################################################################################################
OBC_LOCAL_INC_DIR := \
    $(BALONG_SRC_PATH)/config/nvim/include/gu \
	$(BALONG_SRC_PATH)/config/osa \
    $(BALONG_SRC_PATH)/config/product/$(PRODUCT_CFG_PRODUCT_FORM)/include_gu/ \
    $(BALONG_SRC_PATH)/include/drv \
    $(BALONG_SRC_PATH)/include/drv/MODULE_H \
    $(BALONG_SRC_PATH)/include/nv/gu/codec \
    $(BALONG_SRC_PATH)/include/med \
    $(BALONG_SRC_PATH)/include/oam/gu/log \
    $(BALONG_SRC_PATH)/include/oam/comm/om \
	$(BALONG_SRC_PATH)/include/oam/comm/nvim \
    $(BALONG_SRC_PATH)/platform/$(PRODUCT_CFG_PLATFORM_NEW) \
	$(BALONG_SRC_PATH)/platform/$(PRODUCT_CFG_PLATFORM_NEW)/soc \
	$(BALONG_SRC_PATH)/modem/med/custom/hi6930/inc \
	$(TENSILICA_TOOLS_HIFI)/XtensaTools/xtensa-elf/include/xtensa\