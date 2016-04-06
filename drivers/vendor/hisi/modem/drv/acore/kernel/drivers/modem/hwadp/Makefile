subdir-ccflags-y       += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu

KBUILD_CPPFLAGS +=      -Idrivers/hisi/modem_hi3630/drv/om/common \
                     -Idrivers/hisi/modem_hi3630/drv/om/dump \
                     -Idrivers/hisi/modem_hi3630/drv/om/oms\
                     -Idrivers/hisi/modem_hi3630/drv/om/log \
                     -Idrivers/hisi/modem_hi3630/drv/om/sys_view \
                     -Idrivers/hisi/modem_hi3630/drv/om/usbtrace

obj-$(CONFIG_HW_IP_BASE_ADDR)	+= hwadp_balong.o hwadp_memory.o
