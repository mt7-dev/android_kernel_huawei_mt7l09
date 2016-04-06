#
# created by y000184236 for k3v3 fpga
#
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/icc
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/rtc
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/nvim
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/mem
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/balong_timer
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/om \
                   -Idrivers/hisi/modem_hi3630/drv/om/common \
                   -Idrivers/hisi/modem_hi3630/drv/om/dump \
                   -Idrivers/hisi/modem_hi3630/drv/om/log \
                   -Idrivers/hisi/modem_hi3630/drv/om/sys_view \
                   -Idrivers/hisi/modem_hi3630/drv/om/usbtrace \
                   -Idrivers/hisi/modem_hi3630/drv/om/aximonitor \
                   -Idrivers/hisi/modem_hi3630/drv/om/oms
KBUILD_CPPFLAGS += -Idrivers/hisi/modem_hi3630/drv/udi\
                   -Idrivers/usb/gadget
obj-y           += adp_ipc.o
obj-y           += adp_icc.o
#obj-y           += adp_mem_balong.o
obj-y           += adp_nvim.o
obj-y           += adp_bbp_balong.o
obj-y           += adp_dpm.o
obj-y           += adp_wifi.o
obj-y           += adp_socp.o
obj-y           += adp_rtc_balong.o
obj-y           += adp_om.o
obj-y           += adp_dump.o
obj-y           += adp_utrace.o
obj-y           += adp_charger.o
obj-y           += adp_version.o
obj-y           += adp_timer.o
obj-y           += adp_udi.o
obj-y           += adp_mmc.o
obj-y           += adp_acm.o
obj-y           += adp_sec.o
obj-n           += adp_int.o
obj-n           += adp_gic_balong.o
obj-y           += adp_wdt.o
obj-y           += adp_rfile.o
obj-y           += adp_onoff.o
obj-y           += adp_efuse.o
ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
obj-y           += adp_onoff_k3.o
obj-y           += adp_nandc_k3.o
obj-y           += adp_ncm_k3.o
obj-y           += adp_usb_k3.o
obj-y           += adp_pm_k3.o
else
obj-y           += adp_ncm.o
obj-y           += adp_usb.o
obj-y           += adp_pm.o
endif

obj-y           += adp_reset.o
obj-y           += adp_cshell.o
ifeq ($(strip $(CFG_OS_ANDROID_AARCH64)),YES)
obj-y           += easy_shell.o
endif
ifeq ($(strip $(CFG_CONFIG_PASTAR_DSM)),YES)
obj-y			+= adp_pmu_hi6561.o
endif