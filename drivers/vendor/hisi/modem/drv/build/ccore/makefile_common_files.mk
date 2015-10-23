
include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

DRV_DIR                  :=$(BALONG_TOPDIR)/modem/drv
BSP_DIR                  :=$(DRV_DIR)/ccore
OS_SRC_DIR               :=$(BALONG_TOPDIR)/build/delivery/$(OBB_PRODUCT_NAME)/os/$(OBC_LOCAL_CORE_NAME)
ASIC_DIR_NAME            :=$(CFG_BSP_ASIC_DIR_NAME)

CORE_INDEX_MODEM         :=1
CORE_INDEX_APP           :=2
CORE_INDEX_BOOTROM       :=3
CORE_INDEX_BOOTLOADER    :=4
#*******************************************************************
# src:OBC_LOCAL_SRC_FILE
#*******************************************************************

# sys :  sysALib.s 必须放到第一个
ifeq ($(INSTANCE_ID),INSTANCE_0)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/sys/sysALib.s \
	$(BSP_DIR)/sys/sysLib_$(CFG_PLATFORM).c \
	$(BSP_DIR)/sys/usrAppInit.c \
	$(BSP_DIR)/sys/spinLockAmp.s
ifeq ($(strip $(CFG_PLATFORM)),hi3630)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/sys/sysL2Cache.c
endif
ifeq ($(strip $(CFG_CONFIG_BALONG_MODEM_RESET)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/sys/excArchLib.c
endif
# add protection for text section .
OBC_LOCAL_SRC_FILE += \
	$(BALONG_TOPDIR)/build/libs/os/ccore/dataSegPad.c

ifeq ($(strip $(CFG_ROM_COMPRESS)),YES)
OBC_LOCAL_SRC_ROM_COMPRESS_FILE = \
	$(BSP_DIR)/sys/romInit.s \
	$(BSP_DIR)/sys/romMmu.c \
	$(BSP_DIR)/sys/romStart.c	
endif

# gic
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/gic/armGenIntCtlr.c \
	$(BSP_DIR)/drivers/gic/gic_pm.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/gic/gic_test.c

# systimer
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/systimer/sysTimer.c

# watchdog
ifeq ($(strip $(CFG_CONFIG_CCORE_WDT)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/watchdog/wdt_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/watchdog/wdt_balong_test.c
endif

#analysis
ifeq ($(strip $(CFG_CONFIG_MODEM_PERF)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/analysis/perf_pmu_analyse.c \
	$(BSP_DIR)/drivers/analysis/perf_axi_analyse.c \
	$(BSP_DIR)/drivers/analysis/perf_analyse.c
endif

# busstress
ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/busstress/busstress_balong.c \
	$(BSP_DIR)/drivers/busstress/a9_stress_balong.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_comm/BasicFunc.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_comm/common.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_comm/dmac1.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_comm/mem_fun.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_hdlc/master.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_hdlc/hdlcv200_frm_def_test_bbit.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_cicom/cicom.c \
	$(BSP_DIR)/drivers/busstress/gu/uft_upacc/upacc.c
endif

# uart
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/uart/balongv7r2_uart.c \
	$(BSP_DIR)/drivers/uart/sysSerial.c \
	$(BSP_DIR)/drivers/uart/printksync.c

#dual_modem
ifeq ($(strip $(CFG_CONFIG_DUAL_MODEM)),YES)
OBC_LOCAL_SRC_FILE +=  \
	$(BSP_DIR)/drivers/dual_modem/dual_modem.c
else
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_dual_modem_uart.c
endif
#ipc
ifeq ($(strip $(CFG_CONFIG_MODULE_IPC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/ipc/ipc_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/ipc/ipc_balong_test.c

endif

# gpio
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/gpio/gpio_balong.c

# ios
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/ios/ios_balong.c \
	$(BSP_DIR)/drivers/ios/ios_list.c


OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/gpio/gpio_balong_test.c


# anten
ifeq ($(strip $(CFG_CONFIG_ANTEN)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/anten/anten_balong.c
endif

# ddm
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/ddm/ddm_phase.c

# icc
ifeq ($(strip $(CFG_CONFIG_ICC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/icc/icc_balong.c \
	$(BSP_DIR)/drivers/icc/icc_balong_debug.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/icc/icc_balong_test.c

endif

# efuse
ifeq ($(strip $(CFG_CONFIG_EFUSE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/efuse/efuse_balong.c
endif

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/efuse/efuse_balong_test.c \
	$(BSP_DIR)/drivers/efuse/efuse_rfile_test.c

#rtc
ifeq ($(strip $(CFG_CONFIG_BALONG_RTC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong_test.c
endif
ifeq ($(strip $(CFG_CONFIG_HI6551_RTC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong_test.c
endif
ifeq ($(strip $(CFG_CONFIG_HI6559_RTC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong_test.c
endif
ifeq ($(strip $(CFG_CONFIG_HI3630_RTC0)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/rtc/rtc_balong_test.c
endif


#mem
ifeq ($(strip $(CFG_CONFIG_MEM)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/mem/mem_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/mem/mem_balong_test.c
else
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/mem/adp_mem_balong.c
endif

# sio
ifeq ($(strip $(CFG_CONFIG_SIO)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/sio/balong_sio.c
endif

# wm8990
ifeq ($(strip $(CFG_CONFIG_WM8990)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/wm8990/wm8990_plat.c \
	$(BSP_DIR)/drivers/wm8990/wm8990.c
endif

# audio
ifeq ($(strip $(CFG_CONFIG_AUDIO)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/audio/audio_nv.c \
	$(BSP_DIR)/drivers/audio/audiodev.c

#正常版本SIO在HIFI控制，底软单编才使能并验证SIO
ifeq ($(strip $(CFG_CONFIG_SIO)),YES)
ifeq ($(strip $(CFG_CONFIG_WM8990)),YES)
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/audio/audio_test.c
endif
endif

else
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_audio.c
endif

#udi
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/udi/udi_balong.c

#usb
#OBC_LOCAL_SRC_FILE += \
#	$(BSP_DIR)/drivers/usb/adp_usb.c

ifeq ($(strip $(CFG_CONFIG_IPF)),YES)
#ipf
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/ipf/ipf_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/ipf/ipf_balong_test.c
endif

# nvim
ifeq ($(strip $(CFG_CONFIG_NVIM)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/nvim/nv_comm.c \
        $(BSP_DIR)/drivers/nvim/nv_base.c  \
	$(BSP_DIR)/drivers/nvim/NVIM_ResumeId.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/nvim/nv_test.c
endif

#vic
ifeq ($(strip $(CFG_CONFIG_MODULE_VIC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/vic/vic_balong.c
endif

#softtimer hardtimer
ifeq ($(strip $(CFG_CONFIG_MODULE_TIMER)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/balong_timer/hardtimer_balong.c \
	$(BSP_DIR)/drivers/balong_timer/softtimer_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/balong_timer/hardtimer_balong_test.c \
	$(BSP_DIR)/drivers/balong_timer/softtimer_balong_test.c

ifeq ($(strip $(CFG_K3_TIMER_FEATURE)),YES)
	OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/balong_timer/hardtimer_k3.c
	OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/balong_timer/hardtimer_k3_test.c
endif
endif

# sci
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/sci/sci_balong.c \
	$(BSP_DIR)/drivers/sci/sci_pl131.c  \
	$(BSP_DIR)/drivers/sci/sci_debug.c  \
	$(BSP_DIR)/drivers/sci/sci_cfg.c
# dsda add sci_cfg.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/sci/sci_balong_test.c



#borad
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/board/board_fpga.c


#cipher
ifeq ($(strip $(CFG_CONFIG_CIPHER)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cipher/cipher_balong.c \
	$(BSP_DIR)/drivers/cipher/acc_balong.c


ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cipher/cipher_kdf_bus_stress.c \
	$(BSP_DIR)/drivers/cipher/acc_bus_stress.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/cipher/acc_balong_test.c \
	$(BSP_DIR)/drivers/cipher/cipher_balong_test.c

endif
endif

#sec
ifneq ($(strip $(CFG_BSP_HAS_SEC_FEATURE)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/sec/sec_balong.c
endif

#security
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/security/aes_alg.c \
	$(BSP_DIR)/drivers/security/rsann.c \
	$(BSP_DIR)/drivers/security/rsa_cipher.c \
	$(BSP_DIR)/drivers/security/aes.c \
	$(BSP_DIR)/drivers/security/ran.c \
	$(BSP_DIR)/drivers/security/sha2.c \
	$(BSP_DIR)/drivers/security/hash_md5.c \
	$(BSP_DIR)/drivers/security/security2.c

# spi
ifeq ($(strip $(CFG_CONFIG_SPI)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/spi/spi_balong.c
endif
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/spi/spi.c
# pmu
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6451)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pmu/hi6451/pmu_hi6451.c \
    $(BSP_DIR)/drivers/pmu/hi6451/common_hi6451.c \
    $(BSP_DIR)/drivers/pmu/hi6451/debug_hi6451.c
OBC_LOCAL_SRC_TEST_FILE += \
    $(BSP_DIR)/drivers/pmu/hi6451/pmu_hi6451_test.c
endif
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6551)),YES)
OBC_LOCAL_SRC_FILE += \
    $(BSP_DIR)/drivers/pmu/hi6551/pmu_hi6551.c \
    $(BSP_DIR)/drivers/pmu/hi6551/common_hi6551.c \
    $(BSP_DIR)/drivers/pmu/hi6551/debug_hi6551.c
OBC_LOCAL_SRC_TEST_FILE += \
    $(BSP_DIR)/drivers/pmu/hi6551/pmu_hi6551_test.c
endif

ifeq ($(strip $(CFG_CONFIG_PMIC_HI6559)),YES)
OBC_LOCAL_SRC_FILE += \
    $(BSP_DIR)/drivers/pmu/hi6559/pmu_hi6559.c \
    $(BSP_DIR)/drivers/pmu/hi6559/common_hi6559.c \
    $(BSP_DIR)/drivers/pmu/hi6559/debug_hi6559.c
OBC_LOCAL_SRC_TEST_FILE += \
    $(BSP_DIR)/drivers/pmu/hi6559/pmu_hi6559_test.c
endif

OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pmu/pmu_balong.c \
    $(BSP_DIR)/drivers/pmu/pmu_debug.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/pmu/pmu_test.c

# regulator
ifeq ($(strip $(CFG_CONFIG_REGULATOR)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/regulator/regulator_balong.c \
	$(BSP_DIR)/drivers/regulator/regulator_test_byid.c \
	$(BSP_DIR)/drivers/regulator/regulator_balong_table.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/regulator/regulator_balong_test.c
endif

ifeq ($(strip $(CFG_CONFIG_REGULATOR)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/regulator/regulator.c
endif

# cpufreq
ifeq ($(strip $(CFG_CONFIG_CPUFREQ)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cpufreq/cpufreq_balong.c \
	$(BSP_DIR)/drivers/cpufreq/cpufreq_balong_api.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/cpufreq/cpufreq_balong_test.c
endif
ifeq ($(strip $(CFG_CONFIG_CPUFREQ)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cpufreq/cpufreq.c
endif

#mipi
ifeq ($(strip $(CFG_CONFIG_MIPI)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/mipi/bbp_mipi_balong.c 
endif

#pastar
ifeq ($(strip $(CFG_CONFIG_PASTAR)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pmu/hi6561/pmu_hi6561.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/pmu/hi6561/pmu_hi6561_test.c
endif
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_pmu_hi6561.c

OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pa_rf/pa_rf_balong.c
	
# os
OBC_LOCAL_SRC_FILE += \
	$(OS_SRC_DIR)/linkSyms.c \
	$(OS_SRC_DIR)/prjConfig.c

# osl
OBC_LOCAL_SRC_FILE += \
	$(DRV_DIR)/common/osl/osl_wait.c


OBC_LOCAL_SRC_TEST_FILE += \
#	$(DRV_DIR)/common/osl/io_test.c \
	$(DRV_DIR)/common/osl/test_osl_wait.c \
	$(DRV_DIR)/common/osl/test_osl_list.c

#HW_SPINLOCK
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/hwspinlock/bsp_hw_spinlock.c

#dsp
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_dsp.c
ifeq ($(strip $(CFG_CONFIG_DSP)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/dsp/bsp_dsp.c
ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/dsp/bsp_dsp_test.c
endif
endif

#tuner
ifeq ($(strip $(CFG_CONFIG_TUNER)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/tuner/bsp_tuner.c
endif

#hkadc
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_hkadc.c
ifeq ($(strip $(CFG_CONFIG_HKADC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/hkadc/bsp_hkadc.c
endif

#bbp_int
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_bbp_balong.c
ifeq ($(strip $(CFG_CONFIG_BBP_INT)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/bbp/bbp_balong.c
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/bbp/bbp_balong_test.c
endif

#modem memrepair
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/memrepair/memrepair_balong.c
#abb
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/abb/bsp_abb.c

#edma
ifeq ($(strip $(CFG_CONFIG_BALONG_EDMA)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/edma/edma_balong.c


OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/edma/edma_balong_test.c
endif

ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
# edma busstress
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/edma/edma_balong_busstress.c
endif

# pm
ifeq ($(strip $(CFG_CONFIG_CCORE_BALONG_PM)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pm/pm.c  \
	$(BSP_DIR)/drivers/pm/pm_asm_deepsleep.s

endif

# om
ifeq ($(strip $(CFG_ENABLE_BUILD_OM)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/common/bsp_om_comm.c \
	$(BSP_DIR)/drivers/om/common/bsp_dmesg.c \
	$(BSP_DIR)/drivers/om/common/regcpy.s \
	$(BSP_DIR)/drivers/om/common/bsp_om_save.c \
	$(BSP_DIR)/drivers/om/oms/bsp_om_server.c \
	$(BSP_DIR)/drivers/om/log/bsp_om_log.c \
	$(BSP_DIR)/drivers/om/test/bsp_dump_test.c

ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
OBC_LOCAL_SRC_FILE += $(BSP_DIR)/drivers/om/dump/bsp_dump_k3.c
else
OBC_LOCAL_SRC_FILE += $(BSP_DIR)/drivers/om/dump/bsp_dump.c
endif

# sysview
ifeq ($(strip $(CFG_ENABLE_BUILD_SYSVIEW)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/sys_view/bsp_om_sysview.c \
	$(BSP_DIR)/drivers/om/sys_view/bsp_task_view.c

# memview
ifeq ($(strip $(CFG_ENABLE_BUILD_MEMVIEW)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/sys_view/bsp_mem_view.c
endif
endif

# cpuview
ifeq ($(strip $(CFG_ENABLE_BUILD_CPUVIEW)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/sys_view/bsp_cpu_view.c
endif

# amon
ifeq ($(strip $(CFG_ENABLE_BUILD_AMON)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/aximonitor/amon_balong.c \
	$(BSP_DIR)/drivers/om/aximonitor/amon_addr_balong.c
endif

# utrace
ifeq ($(strip $(CFG_ENABLE_BUILD_UTRACE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/om/usbtrace/utrace_balong.c
endif

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/om/test/bsp_om_test.c \
	$(BSP_DIR)/drivers/om/test/bsp_dump_test.c

ifeq ($(strip $(CFG_ENABLE_BUILD_AMON)),YES)
OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/om/test/amon_balong_test.c
endif
endif
# dpm
ifeq ($(strip $(CFG_CONFIG_CCORE_PM)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/dpm/dpm_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/dpm/dpm_balong_test.c
endif
# version
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_version.c \
	$(BSP_DIR)/drivers/version/version_balong.c

# socp
ifeq ($(strip $(CFG_ENABLE_BUILD_SOCP)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/socp/socp_balong.c \

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/socp/socp_balong_test.c
endif
#cshell
ifeq ($(strip $(CFG_CONFIG_CSHELL)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cshell/cshell.c
endif
#clk
ifeq ($(strip $(CFG_CONFIG_BALONG_CCLK)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/clk/clock.c
endif
ifeq ($(strip $(CFG_CONFIG_BALONG_CCLK)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/clk/clock_balong.c \
	$(BSP_DIR)/drivers/clk/clock_balong_ops.c \
	$(BSP_DIR)/drivers/clk/clock_balong_hi6930.c\
	$(BSP_DIR)/drivers/clk/clock_balong_debug.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/clk/clock_balong_test.c
endif


# rfile
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/rfile/rfile_balong.c \
	$(BSP_DIR)/drivers/adp/adp_rfile.c

OBC_LOCAL_SRC_TEST_FILE += \
#	$(BSP_DIR)/drivers/rfile/rfile_test.c

# mailbox
ifeq ($(strip $(CFG_FEATURE_TLPHY_MAILBOX)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/mailbox/mailbox_commom.c \
	$(BSP_DIR)/drivers/mailbox/mailbox_commsg.c \
	$(BSP_DIR)/drivers/mailbox/mailbox_spmsg.c
endif

# mbxhifi
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_debug.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_gut.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_msg.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_port_vxworks.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_test_vxworks.c \
	$(BSP_DIR)/drivers/mbxhifi/drv_mailbox_table.c

# get_ip_base_addr
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/hwadp/hwadp_balong.c \
	$(BSP_DIR)/drivers/hwadp/hwadp_memory.c
#cpuidle
ifeq ($(strip $(CFG_CONFIG_CCORE_CPU_IDLE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/cpuidle/cpuidle_balong.c \
	$(BSP_DIR)/drivers/cpuidle/sleep_balong.s
endif

#i2c
#当前不编，若需要编译，必须在product_config.define填加此项
ifeq ($(strip $(CFG_CONFIG_CCORE_I2C)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/i2c/i2c_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/i2c/i2c_balong_test.c
endif

#sync
ifeq ($(strip $(CFG_CONFIG_MODULE_SYNC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/sync/sync_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/sync/sync_balong_test.c
endif

#wakelock
ifeq ($(strip $(CFG_CONFIG_HAS_CCORE_WAKELOCK)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/wakelock/wakelock_balong.c
endif

ifeq ($(strip $(CFG_CONFIG_HAS_CCORE_WAKELOCK)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/wakelock/wakelock.c
endif

#onoff
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_onoff.c
ifeq ($(strip $(CFG_CONFIG_ONOFF)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/onoff/bsp_onoff.c
endif

#lowpower mntn
ifeq ($(strip $(CFG_CONFIG_PWC_MNTN_CCORE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/lowpower_mntn/lowpower_mntn_balong.c
endif
#lowpower mntn
ifeq ($(strip $(CFG_CONFIG_PWC_MNTN_CCORE)),NO)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_lowpower_mntn.c
endif

# modem reset
ifeq ($(strip $(CFG_CONFIG_BALONG_MODEM_RESET)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/reset/fiq_lib_balong.s \
	$(BSP_DIR)/drivers/reset/reset_balong.c
endif

#adp
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/adp/adp_gic_balong.c \
	$(BSP_DIR)/drivers/adp/adp_int.c \
	$(BSP_DIR)/drivers/adp/adp_wdt.c \
	$(BSP_DIR)/drivers/adp/adp_ipc.c  \
	$(BSP_DIR)/drivers/adp/adp_icc.c \
	$(BSP_DIR)/drivers/adp/adp_timer.c \
	$(BSP_DIR)/drivers/adp/adp_sysctrl.c \
	$(BSP_DIR)/drivers/adp/adp_edma.c \
	$(BSP_DIR)/drivers/adp/adp_wakelock.c \
	$(BSP_DIR)/drivers/adp/adp_clock.c \
	$(BSP_DIR)/drivers/adp/adp_dpm.c\
	$(BSP_DIR)/drivers/adp/adp_cipher.c\
	$(BSP_DIR)/drivers/adp/adp_rtc_balong.c\
	$(BSP_DIR)/drivers/adp/adp_nvim.c\
	$(BSP_DIR)/drivers/adp/adp_sci.c \
	$(BSP_DIR)/drivers/adp/adp_om.c \
	$(BSP_DIR)/drivers/adp/adp_dump.c \
	$(BSP_DIR)/drivers/adp/adp_socp.c\
	$(BSP_DIR)/drivers/adp/adp_amon.c\
	$(BSP_DIR)/drivers/adp/adp_sec.c\
	$(BSP_DIR)/drivers/adp/adp_udi.c\
	$(BSP_DIR)/drivers/adp/adp_gpio.c\
	$(BSP_DIR)/drivers/adp/adp_anten.c\
	$(BSP_DIR)/drivers/adp/adp_efuse.c\
	$(BSP_DIR)/drivers/adp/adp_temperature.c\
	$(BSP_DIR)/drivers/adp/adp_pm.c\
	$(BSP_DIR)/drivers/adp/adp_usb.c \
	$(BSP_DIR)/drivers/adp/adp_reset.c \
	$(BSP_DIR)/drivers/adp/adp_cshell.c
#	$(BSP_DIR)/drivers/adp/adp_mem_balong.c\






ifeq ($(HUTAF_HLT_COV),true)
OBC_LOCAL_SRC_FILE += \
    $(BSP_DIR)/drivers/lltcov/ltcov_ccore.c
endif

ifeq ($(CONFIG_ENABLE_TEST_CODE),y)
OBC_LOCAL_SRC_TEST_FILE_TEMP := $(filter-out $(OBC_LOCAL_SRC_FILE),$(OBC_LOCAL_SRC_TEST_FILE))
OBC_LOCAL_SRC_FILE += $(OBC_LOCAL_SRC_TEST_FILE_TEMP)
endif

# rse
OBC_LOCAL_SRC_FILE += \
    $(BSP_DIR)/drivers/rse/rse_balong.c


#tcxo
ifeq ($(strip $(CFG_CONFIG_TCXO_BALONG)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/tcxo/tcxo_balong.c
endif
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/tcxo/tcxo.c

endif

#leds
ifeq ($(strip $(CFG_CONFIG_LEDS_CCORE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/leds/leds_balong.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/leds/balong_led_test.c

else
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/leds/leds_stub.c
endif

ifeq ($(INSTANCE_ID),INSTANCE_1)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/sci/sci_balong.c \
	$(BSP_DIR)/drivers/sci/sci_pl131.c  \
	$(BSP_DIR)/drivers/sci/sci_debug.c  \
        $(BSP_DIR)/drivers/adp/adp_sci.c

OBC_LOCAL_SRC_TEST_FILE += \
	$(BSP_DIR)/drivers/sci/sci_balong_test.c
endif
ifeq ($(CFG_CONFIG_MODEM_PINTRL),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_DIR)/drivers/pintrl/pintrl_balong.c

endif


#*******************************************************************
# inc:OBC_LOCAL_INC_DIR
#*******************************************************************
# include
OBC_LOCAL_INC_DIR += \
	$(BALONG_TOPDIR)/include/drv \
	$(BALONG_TOPDIR)/include/ps/gups \
	$(BALONG_TOPDIR)/include/nv/tl/drv \
	$(BALONG_TOPDIR)/include/nv/gu/drv \
	$(BALONG_TOPDIR)/include/nv/comm \
	$(BALONG_TOPDIR)/include/nv/tl/oam \
	$(BALONG_TOPDIR)/include/phy/lphy \
	$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/os/ccore \
	$(BALONG_TOPDIR)/config/product/nvim/include/gu


# sys
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/sys

#ccore include
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/include \
	$(BALONG_TOPDIR)/include/nv/tl/drv \
	$(BALONG_TOPDIR)/include/nv/tl/oam \
	$(BALONG_TOPDIR)/include/nv/tl/lps \
	$(BALONG_TOPDIR)/config/nvim/include/gu

# driver
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/gic \
	$(BSP_DIR)/drivers/uart \
	$(BSP_DIR)/drivers/systimer \
	$(BSP_DIR)/drivers/ipc \
	$(BSP_DIR)/drivers/icc \
	$(BSP_DIR)/drivers/spi \
	$(BSP_DIR)/drivers/pmu \
   	$(BSP_DIR)/drivers/pmu/hi6451 \
   	$(BSP_DIR)/drivers/pmu/hi6551 \
	$(BSP_DIR)/drivers/pmu/hi6559 \
	$(BSP_DIR)/drivers/regulator \
	$(BSP_DIR)/drivers/clk \
	$(BSP_DIR)/drivers/nvim \
	$(BSP_DIR)/drivers/om \
	$(BSP_DIR)/drivers/om/dump \
	$(BSP_DIR)/drivers/om/log \
	$(BSP_DIR)/drivers/om/common \
	$(BSP_DIR)/drivers/om/sys_view \
	$(BSP_DIR)/drivers/om/oms \
	$(BSP_DIR)/drivers/om/aximonitor \
	$(BSP_DIR)/drivers/dpm \
	$(BSP_DIR)/drivers/socp \
	$(BSP_DIR)/drivers/i2c \
	$(BSP_DIR)/drivers/wakelock\
	$(BSP_DIR)/drivers/rfile\
	$(BSP_DIR)/drivers/sec\
	$(BSP_DIR)/drivers/udi\
	$(BSP_DIR)/drivers/tcxo\
	$(BSP_DIR)/drivers/bbp


OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/rtc

ifeq ($(strip $(CFG_CONFIG_IPF)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/ipf
endif

ifeq ($(strip $(CFG_CONFIG_MODULE_VIC)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/vic
endif


# busstress
#ifeq ($(strip $(CFG_CONFIG_MODULE_BUSSTRESS)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/busstress \
	$(BSP_DIR)/drivers/busstress/gu/uft_comm \
	$(BSP_DIR)/drivers/busstress/gu/uft_hdlc \
	$(BSP_DIR)/drivers/busstress/gu/uft_cicom \
	$(BSP_DIR)/drivers/busstress/gu/uft_upacc
#endif

OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/board

# osl
OBC_LOCAL_INC_DIR += \
	$(DRV_DIR)/common/include

# ddm
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/om/ddm

# anten
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/anten

# efuse
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/efuse

# gpio
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/gpio

# rse
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/rse

# ios
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/ios
# pintrl
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/pintrl

# temperature
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/temperature

#cpuidle
ifeq ($(strip $(CFG_CONFIG_CCORE_CPU_IDLE)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/cpuidle
endif

#lowpower mntn
ifeq ($(strip $(CFG_CONFIG_PWC_MNTN_CCORE)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/lowpower_mntn
endif

#leds
ifeq ($(strip $(CFG_CONFIG_LEDS_CCORE)),YES)
OBC_LOCAL_INC_DIR += \
	$(BSP_DIR)/drivers/leds
endif

#dual_modem
ifeq ($(strip $(CFG_CONFIG_DUAL_MODEM)),YES)
OBC_LOCAL_INC_DIR +=  \
	$(BSP_DIR)/drivers/dual_modem
endif

# os
OBC_LOCAL_INC_DIR += \
	$(OS_SRC_DIR) \
	$(WIND_HOME)/vxworks-6.8/target/h \
	$(WIND_HOME)/vxworks-6.8/target/h/wrn/coreip \
	$(WIND_HOME)/vxworks-6.8/target/config/comps/src \
	$(WIND_HOME)/vxworks-6.8/target/src/drv \
	$(WIND_HOME)/vxworks-6.8/target/src/hwif \
	$(WIND_HOME)/vxworks-6.8/target/src/hwif/h \
	$(WIND_HOME)/components/windml-5.3/h \
	$(WIND_HOME)/components/webservices-1.7/h \
	$(WIND_HOME)/components/webcli-4.8/target/h \
	$(WIND_HOME)/components/opc-3.2/h \
	$(WIND_HOME)/components/dcom-2.3/h \
	$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src \
	$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src \
	$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h \
	$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h

#ipcom
OBC_LOCAL_INC_DIR  += \
	$(WIND_HOME)/components/ip_net2-6.8/ipcom \
	$(WIND_HOME)/components/ip_net2-6.8/ipcom/port/vxworks/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipcom/port/vxworks/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipcom/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipcom/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipnet2/src \
	$(WIND_HOME)/components/ip_net2-6.8/ipnet2/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipnet2/config  \
	$(WIND_HOME)/components/ip_net2-6.8/ipappl/config \
	$(WIND_HOME)/components/ip_net2-6.8/iprip/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipsntp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcps/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcpr/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipppp/config \
	$(WIND_HOME)/components/ip_net2-6.8/iptcp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmcrypto/config\
	$(WIND_HOME)/components/ip_net2-6.8/vxmux/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipsctp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipwps/config \
	$(WIND_HOME)/components/ip_net2-6.8/vxcoreip/config \
	$(WIND_HOME)/components/ip_net2-6.8/vxcompat/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipappl/include \
	$(WIND_HOME)/components/ip_net2-6.8/iprip/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipsntp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcps/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcpr/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipppp/include \
	$(WIND_HOME)/components/ip_net2-6.8/iptcp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmcrypto/include \
	$(WIND_HOME)/components/ip_net2-6.8/vxmux/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipsctp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipwps/include \
	$(WIND_HOME)/components/ip_net2-6.8/vxcoreip/include \
	$(WIND_HOME)/components/ip_net2-6.8/vxcompat/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipripng/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcpc6/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcps6/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipripng/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcpc6/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipdhcps6/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipwlan/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmcp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipipsec2/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipike/config \
	$(WIND_HOME)/components/ip_net2-6.8/iphwcrypto/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipcrypto/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipssh/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipfa/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipha/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipmn/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmpls/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipeap/config \
	$(WIND_HOME)/components/ip_net2-6.8/ip8021x/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipradius/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipfirewall/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipssl2/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipl2tp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipcavium/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipdiameter/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipfreescale/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip4/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmippm/config \
	$(WIND_HOME)/components/ip_net2-6.8/iprohc/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipwlan/config \
	$(WIND_HOME)/components/ip_net2-6.8/vxbridge/config \
	$(WIND_HOME)/components/ip_net2-6.8/vxdot1x/config \
	$(WIND_HOME)/components/ip_net2-6.8/wrsnmp/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmcp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipipsec2/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipike/include \
	$(WIND_HOME)/components/ip_net2-6.8/iphwcrypto/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipcrypto/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipssh/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipfa/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipha/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmipmn/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmpls/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipeap/include \
	$(WIND_HOME)/components/ip_net2-6.8/ip8021x/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipradius/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipfirewall/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipssl2/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipl2tp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipcavium/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipdiameter/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipfreescale/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip4/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmippm/include \
	$(WIND_HOME)/components/ip_net2-6.8/iprohc/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipwlan/include \
	$(WIND_HOME)/components/ip_net2-6.8/vxbridge/include \
	$(WIND_HOME)/components/ip_net2-6.8/vxdot1x/include \
	$(WIND_HOME)/components/ip_net2-6.8/wrsnmp/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6mn/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6ha/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6pm/config \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6mn/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6ha/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipmip6pm/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipcrypto/openssl-0_9_8/include \
	$(WIND_HOME)/components/ip_net2-6.8/ipssl2/openssl-0_9_8/include








