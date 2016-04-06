-include $(BALONG_TOPDIR)/build/scripts/make_base.mk
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
DRIVER_SRCS_MODEM += \
	$(DRIVER_DIR_MODEM)/ipf/ipf_balong_pm.c \
	$(DRIVER_DIR_MODEM)/ipc/ipc_lpm3_k3.c \
	$(DRIVER_DIR_MODEM)/icc/icc_balong.c \
	$(DRIVER_DIR_MODEM)/icc/icc_balong_debug.c \
	$(DRIVER_DIR_MODEM)/timer/softtimer_lpm3_k3.c \
	$(DRIVER_DIR_MODEM)/nvim/nv_base.c \
	$(DRIVER_DIR_MODEM)/nvim/nv_comm.c \
	$(DRIVER_DIR_MODEM)/modem/m3_modem.c \
	$(DRIVER_DIR_MODEM)/pm-k3/m3_pm_modem.c \
	$(DRIVER_DIR_MODEM)/pm-k3/m3_dpm_modem.c \
	$(DRIVER_DIR_MODEM)/pm-k3/m3_cpufreq_modem.c \
	$(DRIVER_DIR_MODEM)/watchdog/wdt_balong.c \
	$(DRIVER_DIR_MODEM)/regulator/regulator_balong.c \
	$(DRIVER_DIR_MODEM)/dpm/dpm_lpm3_k3.c \
	$(DRIVER_DIR_MODEM)/temperature/temperature.c \
	$(DRIVER_DIR_MODEM)/socp/socp_balong.c \
	$(DRIVER_DIR_MODEM)/edma/edma_balong.c \
	$(DRIVER_DIR_MODEM)/hkadc/bsp_hkadc.c \
	$(DRIVER_DIR_MODEM)/start-k3/modem_start.c \
	$(DRIVER_DIR_MODEM)/reset/reset_balong.c \
	$(DRIVER_DIR_MODEM)/pmu/hi6561/pmu_hi6561.c \
	$(DRIVER_DIR_MODEM)/dual_modem/dual_modem_wakeup.c
CFLAGS  += \
	-I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
	-I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
	-I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc \
	-I$(BALONG_TOPDIR)/include/drv \
	-I$(BALONG_TOPDIR)/include/nv/tl/drv \
	-I$(BALONG_TOPDIR)/include/nv/tl/oam \
	-I$(BALONG_TOPDIR)/modem/drv/common/include \
	-I$(BALONG_TOPDIR)/include/tools \
	-I$(DRIVER_DIR_MODEM)/modem \
	-I$(DRIVER_DIR_MODEM)/pm-k3 \
	-I$(DRIVER_DIR_MODEM)/icc \
	-I$(DRIVER_DIR_MODEM)/ipc \
	-I$(DRIVER_DIR_MODEM)/dpm \
	-I$(DRIVER_DIR_MODEM)/watchdog \
	-I$(DRIVER_DIR_MODEM)/tsensor \
	-I$(DRIVER_DIR_MODEM)/edma \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/om \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/socp \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/regulator \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/temperature \
	-I$(DRIVER_DIR_MODEM)/start-k3 \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/reset \
	-I$(BALONG_TOPDIR)/modem/drv/mcore/kernel/drivers/pmu/hi6561

ifneq ($(wildcard $(TOP_DIR)/vendor/hisi/lpm3_hi3630/src),)
CFLAGS  += -I$(SYS_DIR) 
endif
CFLAGS  += -DK3V3_LPM3_HAS_MODEM_FEATURE

ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
ifeq ($(strip $(CFG_ENABLE_BUILD_OM)),YES)
DRIVER_SRCS_MODEM += \
	$(DRIVER_DIR_MODEM)/om/om_balong.c 
endif
endif
	
$(waring ******CFLAGS=$(CFLAGS))
endif
