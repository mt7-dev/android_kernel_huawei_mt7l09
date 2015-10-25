#
# Makefile for the memory technology device drivers.
#

# Core functionality.
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
			
obj-y				+= common/
obj-y				+= dump/
obj-y				+= log/
obj-y				+= oms/
obj-y				+= mem_save/
ifeq ($(strip $(CFG_ENABLE_BUILD_SYSVIEW)),YES)
obj-y				+= sys_view/
endif
ifeq ($(strip $(CFG_ENABLE_BUILD_UTRACE)),YES)
obj-y				+= usbtrace/
endif
obj-$(CONFIG_ENABLE_TEST_CODE)				+= test/

KBUILD_CPPFLAGS +=	-Idrivers/modem/om/common \
                    -Idrivers/modem/om/dump \
                    -Idrivers/modem/om/oms\
                    -Idrivers/modem/om/log \
                    -Idrivers/modem/om/sys_view \
		    -Idrivers/modem/om/usbtrace

