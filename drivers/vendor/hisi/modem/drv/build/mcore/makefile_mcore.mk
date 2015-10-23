
#***********************************************************
# include the define at the top
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************
# root dir
#***********************************************************
OBC_LOCAL_MOUDLE_NAME ?=bsp_mcore
BSP_DIR :=$(BALONG_TOPDIR)/modem/drv
BSP_MCORE_DIR :=$(BALONG_TOPDIR)/modem/drv/mcore
CLIB_DIR:=$(PATH_GNU_ARM_TOOLCHAIN)
CMSIS_RTOS_RTX_DIR :=$(BALONG_TOPDIR)/modem/system/external/cmsis_rtos_rtx_v4p70
CMSIS_ORG_DIR :=$(BALONG_TOPDIR)/modem/system/external/CMSIS-SP-00300-r3p2-00rel1

#*************************************************************************
# Compilter-Specific flags & Configuration
#*************************************************************************
USER_LIB:=
SYS_LIB_PATH:=
OS_LIB_PATH :=
LINKER_SCRIPT :=$(BSP_DIR)/build/mcore/ARMCMx.ld
USER_CC_DEFINES :=-D__M3_OS__
USER_AS_DEFINES :=-D__M3_OS__
USER_CC_FLAGS :=-mcpu=cortex-m3 -mthumb -gdwarf-2 -MD -Wall -fno-builtin -mlong-calls -ffunction-sections -D__RTX_CM3__ -D__CORTEX_M3 -D__CMSIS_RTOS -Werror -Werror-implicit-function-declaration
USER_AS_FLAGS :=-mcpu=cortex-m3 -mthumb -gdwarf-2 -MD -Wall -fno-builtin -mlong-calls -ffunction-sections -D__RTX_CM3__ -D__CORTEX_M3 -D__CMSIS_RTOS -D__ASSEMBLY__
USER_C_OPTIM_FLAGS:=
USER_AS_OPTIM_FLAGS :=
USER_LD_FLAGS := -T $(LINKER_SCRIPT) --gc-sections -nostartfiles -nostdlib

ifeq ($(strip $(OBB_BUILD_TYPE)),RELEASE)
USER_C_OPTIM_FLAGS:=-Os
USER_AS_OPTIM_FLAGS :=
else
USER_C_OPTIM_FLAGS:= -g -Os
USER_AS_OPTIM_FLAGS :=
endif

# enable symbol
ENABLE_SHELL_SYM :=false


OBC_LOCAL_SRC_FILE_SYM :=
#***********************************************************
# source files
#***********************************************************
#rtx
RTX_SRC_FILE := \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_CMSIS.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Event.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_List.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Mailbox.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_MemBox.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Memory.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Mutex.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Robin.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Semaphore.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_System.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Task.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Time.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/rt_Timer.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/HAL_CM.c \
    $(CMSIS_RTOS_RTX_DIR)/SRC/GCC/HAL_CM3.s \
    $(CMSIS_RTOS_RTX_DIR)/SRC/GCC/SVC_Table.s

#libc
OBC_LOCAL_SRC_FILE := \
    $(BSP_MCORE_DIR)/kernel/libc/malloc.c \
    $(BSP_MCORE_DIR)/kernel/libc/memset.c \
    $(BSP_MCORE_DIR)/kernel/libc/strlen.c \
    $(BSP_MCORE_DIR)/kernel/libc/strcmp.c \
    $(BSP_MCORE_DIR)/kernel/libc/memcpy.c

OBC_LOCAL_INC_DIR:= \
	$(BSP_MCORE_DIR)/kernel/libc

# startup
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/sys/startup_balong_cm3.S

OBC_LOCAL_INC_DIR+= \
	$(BSP_MCORE_DIR)/boot

# system
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/sys/main.c \
	$(BSP_MCORE_DIR)/kernel/sys/rtx_conf_cm.c \
	$(BSP_MCORE_DIR)/kernel/sys/system_balong_cm3.c \
	$(BSP_MCORE_DIR)/kernel/sys/irq.c

OBC_LOCAL_INC_DIR+= \
	$(BSP_MCORE_DIR)/kernel/sys \
	$(BSP_MCORE_DIR)/kernel/include

OBC_LOCAL_INC_DIR+= \
	$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
	$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
	$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc \
	$(BALONG_TOPDIR)/include/drv \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/include/nv/tl/drv \
	$(BALONG_TOPDIR)/include/nv/tl/oam \
	$(BALONG_TOPDIR)/include/nv/comm \
	$(BALONG_TOPDIR)/modem/drv/common/include

OBC_LOCAL_INC_DIR+= \
	$(BSP_MCORE_DIR)/kernel/drivers/serial \
	$(BSP_MCORE_DIR)/kernel/drivers/icc \
	$(BSP_MCORE_DIR)/kernel/drivers/ipc \
	$(BSP_MCORE_DIR)/kernel/drivers/dpm \
	$(BSP_MCORE_DIR)/kernel/drivers/aximonitor \
	$(BSP_MCORE_DIR)/kernel/drivers/version \
	$(BSP_MCORE_DIR)/kernel/drivers/mipi \
	$(BSP_MCORE_DIR)/kernel/drivers/spi \
	$(BSP_MCORE_DIR)/kernel/drivers/pmu \
	$(BSP_MCORE_DIR)/kernel/drivers/tsensor \
 	$(BSP_MCORE_DIR)/kernel/drivers/temperature \
	$(BSP_MCORE_DIR)/kernel/drivers/ios \
	$(BSP_MCORE_DIR)/kernel/drivers/socp \
	$(BSP_MCORE_DIR)/kernel/drivers/om \
	$(BSP_MCORE_DIR)/kernel/drivers/watchdog \
	$(BSP_MCORE_DIR)/kernel/drivers/edma \
	$(BSP_MCORE_DIR)/kernel/drivers/start

# drivers
#start
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/start/ccpu_start.c

#serail
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/serial/console.c \
	$(BSP_MCORE_DIR)/kernel/drivers/serial/uart.c \
	$(BSP_MCORE_DIR)/kernel/drivers/serial/printk.c \
	$(BSP_MCORE_DIR)/kernel/drivers/serial/symbol.c

OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/ipc/ipc_balong.c \
	$(BSP_MCORE_DIR)/kernel/drivers/edma/edma_balong.c


#timer
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/timer/softtimer_balong.c
#dpm
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/dpm/dpm_balong.c

#wdt
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/watchdog/wdt_balong.c
#	$(BSP_MCORE_DIR)/kernel/drivers/watchdog/adp_wdt.c

#ipf
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/ipf/ipf_balong_pm.c \
#$(BSP_MCORE_DIR)/kernel/drivers/ipf/ipf_balong_test.c

#ios
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/ios/ios_list.c \
	$(BSP_MCORE_DIR)/kernel/drivers/ios/ios_pd.c

#tsensor
ifeq ($(strip $(CFG_CONFIG_TSENSOR)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/tsensor/tsensor_balong.c
endif
	
#nvim
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/nvim/nv_base.c \
	$(BSP_MCORE_DIR)/kernel/drivers/nvim/nv_comm.c

#temperature
ifeq ($(strip $(CFG_CONFIG_TEMPERATURE_PROTECT)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/temperature/temperature.c
endif

#hifi
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/dsp/bsp_hifi.c

#icc
ifeq ($(strip $(CFG_CONFIG_ICC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/icc/icc_balong.c \
	$(BSP_MCORE_DIR)/kernel/drivers/icc/icc_balong_debug.c
#OBC_LOCAL_SRC_FILE += $(BSP_MCORE_DIR)/kernel/drivers/icc/icc_balong_test.c
endif

#hkadc
ifeq ($(strip $(CFG_CONFIG_HKADC)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/hkadc/bsp_hkadc.c
endif

#onoff
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/onoff/bsp_onoff.c

#aximonitor
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/aximonitor/amon_balong.c

ifneq ($(ENABLE_SHELL_SYM),true)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/serial/symbol_dummy.c
endif
#om
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/om/om_balong.c \
	$(BSP_MCORE_DIR)/kernel/drivers/om/m3_dump.c

#socp
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/socp/socp_balong.c
#	$(BSP_MCORE_DIR)/kernel/drivers/socp/socp_balong_test.c

#spi
#OBC_LOCAL_SRC_FILE += \
#	$(BSP_MCORE_DIR)/kernel/drivers/spi/spi_balong.c
#pmu
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6451)),YES)
OBC_LOCAL_SRC_FILE += \
#	$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6451/pmu_hi6451.c
#	$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6451/pmu_hi6451_test.c
endif
ifeq ($(strip $(CFG_CONFIG_PMIC_HI6551)),YES)
OBC_LOCAL_SRC_FILE += \
#	$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6551/pmu_hi6551.c \
#    $(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6551/common_hi6551.c \
#    $(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6551/debug_hi6551.c \
#$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6551/pmu_hi6551_test.c
endif

OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/pmu/pmu.c \
#	$(BSP_MCORE_DIR)/kernel/drivers/pmu/pmu_adp.c \
#    $(BSP_MCORE_DIR)/kernel/drivers/pmu/pmu_debug.c \
#$(BSP_MCORE_DIR)/kernel/drivers/pmu/pmu_test.c

#rfile
#OBC_LOCAL_SRC_FILE += \
#   $(BSP_MCORE_DIR)/kernel/drivers/rfile/rfile_balong.c
# pm
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/pm/m3_pm.c \
	$(BSP_MCORE_DIR)/kernel/drivers/pm/m3_cpufreq.c \
	$(BSP_MCORE_DIR)/kernel/drivers/pm/$(CFG_PLATFORM)/pm_api.c

OBC_LOCAL_INC_DIR+= \
	$(BSP_MCORE_DIR)/kernel/drivers/pm \
	$(BSP_MCORE_DIR)/kernel/drivers/pm/$(CFG_PLATFORM)

#mipi&pastar
#ifeq ($(strip $(CFG_CONFIG_PASTAR)),YES)
#OBC_LOCAL_SRC_FILE += \
#	$(BSP_MCORE_DIR)/kernel/drivers/mipi/bbp_mipi_balong.c

#OBC_LOCAL_SRC_FILE += \
#	$(BSP_MCORE_DIR)/kernel/drivers/version/version_balong.c
#	$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6561/pmu_hi6561.c
#endif

#M3 pastar dpm
ifeq ($(strip $(CFG_CONFIG_PASTAR_DPM_M3)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/pmu/hi6561/pmu_hi6561.c
endif

#version
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/version/version_balong.c
	
#lowpower mntn
ifeq ($(strip $(CFG_CONFIG_PWC_MNTN_CCORE)),YES)
OBC_LOCAL_SRC_FILE += \
	$(BSP_MCORE_DIR)/kernel/drivers/lowpower_mntn/lowpower_mntn_balong.c
endif

SYM_FULL_SRC_FILES :=$(OBC_LOCAL_SRC_FILE_SYM)
OBC_LOCAL_SRC_FILE += $(OBC_LOCAL_SRC_FILE_SYM)

LOCAL_OBJ_PATH      := $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
RTX_C_SRCS          := $(filter %.c,$(RTX_SRC_FILE))
RTX_C_OBJS          := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(RTX_C_SRCS)))
RTX_C_DEPENDS       := $(patsubst %.o,%.d,$(RTX_C_OBJS))
RTX_AS_SRCS         := $(filter %.s,$(RTX_SRC_FILE))
RTX_AS_OBJS         := $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(RTX_AS_SRCS)))
RTX_AS_DEPENDS      := $(patsubst %.o,%.d,$(RTX_AS_OBJS))
LIB_RTX             := $(OBB_PRODUCT_DELIVERY_DIR)/lib/librtx.a
ARFLAGS             := cr
#include rules. must be droped at the bottom
include $(BALONG_TOPDIR)/build/scripts/rules/cc_arm_gnu_rules.mk

# rtx compile
USER_LIB+= $(LIB_RTX)
do_lib_before : $(LIB_RTX)
$(LIB_RTX):$(RTX_C_OBJS) $(RTX_AS_OBJS)
	$(AR) $(ARFLAGS) $@ $(RTX_AS_OBJS) $(RTX_C_OBJS)

ifneq ($(strip $(RTX_C_OBJS)),)
# *.c增量编译
ifneq ($(MAKECMDGOALS),clean)
sinclude $(RTX_C_DEPENDS)
endif
$(RTX_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c
	@echo [CC] $@
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@
endif

ifneq ($(strip $(RTX_AS_OBJS)),)
#*.s增量编译
ifneq ($(MAKECMDGOALS),clean)
sinclude $(RTX_AS_DEPENDS)
endif
$(RTX_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.s
	@echo [AS] $@
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@
endif

# symbol
ifeq ($(ENABLE_SHELL_SYM),true)

SYM_FULL_OBJ_FILES := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(SYM_FULL_SRC_FILES)))
SYM_SRC_FILE:= $(BSP_MCORE_DIR)/kernel/drivers/serial/symbol_table.c
SYM_SRC_FILE:= $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.c, $(subst \,/,$(SYM_SRC_FILE)))
SYM_OBJ_FILE:= $(patsubst %.c,%.o, $(subst \,/,$(SYM_SRC_FILE)))
SYM_FULL_C_OBJS := $(patsubst %.o,if=%.o , $(subst \,/,$(SYM_FULL_OBJ_FILES)))

USER_LIB+= $(SYM_OBJ_FILE)
do_lib_before : $(SYM_OBJ_FILE)

$(SYM_SRC_FILE) : $(FULL_C_OBJS)
	@echo [make_symbols] [$@]
	$(Q)python make_symbols.py nm=$(NM) $(SYM_FULL_C_OBJS) of=$(SYM_SRC_FILE)
$(SYM_OBJ_FILE) : $(SYM_SRC_FILE)
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@

endif

#pclint
PCLINT_SYSTEM_INC_FILE		:=$(BALONG_TOPDIR)/build/tools/pc_lint/lint/rtx.lnt
PCLINT_INC_VAR                  :=$(addprefix -I,$(sort $(dir $(OBC_LOCAL_SRC_FILE)))) $(addsuffix /,$(FULL_SRC_INC_PATH))
ifeq ($(strip $(CFG_BSP_CONFIG_HI3630)),YES)
PCLINT_SRC_PLATFORM_FILE	:=$(BALONG_TOPDIR)/modem/drv/build/mcore/pclint_src_hi3630.lnt
else
PCLINT_SRC_PLATFORM_FILE	:=$(BALONG_TOPDIR)/modem/drv/build/mcore/pclint_src_hi6930.lnt
endif
PCLINT_SRC_FILE				:=$(BALONG_TOPDIR)/modem/drv/build/mcore/pclint_src.lnt
PCLINT_CC_OPTION			:=$(LOCAL_CC_OPTION)

.PHONY: $(maction)
$(maction): do_$(maction)
	@echo mcore $(maction) is ready.

ifneq ($(strip $(maction)),)
OBC_USE_NEW_PC_LINT            :=true
ifeq ($(strip $(OBC_USE_NEW_PC_LINT)),false)
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules.mk
else
include makefile_flexelint_common_files.mk
include makefile_flexelint_files_$(CFG_PLATFORM).mk
include $(BALONG_TOPDIR)/build/scripts/rules/pclint_rules_new.mk
endif
endif
