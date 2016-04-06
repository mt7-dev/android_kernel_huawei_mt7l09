#
# Makefile for the memory technology device drivers.
#
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
# Core functionality.

COMM_INC:=../../../../../../include
KBUILD_CPPFLAGS +=	-Idrivers/om \
			-Idrivers/om/dump \
			-I$(COMM_INC)/om/lt/diag 

obj-y				+= bsp_om_sysview.o
obj-y				+= bsp_task_view.o
ifeq ($(strip $(CFG_ENABLE_BUILD_CPUVIEW)),YES)
obj-y				+= bsp_cpu_view.o
endif
ifeq ($(strip $(CFG_ENABLE_BUILD_MEMVIEW)),YES)
obj-y				+= bsp_mem_view.o
endif


