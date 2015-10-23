
include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

BSP_DIR       := $(BALONG_TOPDIR)/modem/drv
#*******************************************************************
# specific pc-lint options
#*******************************************************************
PCLINT_OPTION +=

#*******************************************************************
# header file dir
#*******************************************************************
PCLINT_INC_VAR += \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/ \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu/ \
	-I$(BALONG_TOPDIR)/include/drv \
	-I$(BALONG_TOPDIR)/include/nv/tl/drv \
	-I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
	-I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc \
	-I$(BALONG_TOPDIR)/modem/drv/common/include/ \
	-I$(BALONG_TOPDIR)/modem/system/external/CMSIS-SP-00300-r3p2-00rel1/CMSIS/Include/ \
	-I$(BALONG_TOPDIR)/modem/system/external/cmsis_rtos_rtx_v4p70/INC/ \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/libc/ \
	-I$(BALONG_TOPDIR)/lowpowerM3/include/

#*******************************************************************
# pc-lint src files: OBC_LOCAL_SRC_FILE
#*******************************************************************
OBC_LOCAL_PCLINT_SRC_FILE +=
