####################################################################################################
#
####################################################################################################
include $(BALONG_TOPDIR)/build/scripts/make_base.mk
include $(BALONG_TOPDIR)/modem/oam/build/comm/APP_CORE/msp_code_comm_app_core.inc
include $(BALONG_TOPDIR)/modem/oam/build/gu/APP_CORE/msp_code_gu_app_core.inc
include $(BALONG_TOPDIR)/modem/oam/build/comm/APP_CORE/msp_code_comm_app_core.mk
include $(BALONG_TOPDIR)/modem/oam/build/gu/APP_CORE/msp_code_gu_app_core.mk


CC_INC_DIRS := $(foreach dir,$(MSP_INC_DIRS),-I$(dir))
GUMSP_SRC_FILES:= $(MSP_COMM_SRC_FILES) $(MSP_GU_SRC_FILES)
GUMSP_OBJS := $(subst .C,.o,$(subst .c,.o,$(GUMSP_SRC_FILES)))
GUMSP-objs := $(GUMSP_OBJS) 
obj-y  := GUMSP.o 

GUMSP_CMD_OBJS := $(foreach dir,$(dir $(GUMSP_SRC_FILES)),$(dir).*.o.cmd)

#产品相关的宏定义
EXTRA_CFLAGS += -DVERSION_V3R2 -DVERSION_V3R2_C00 

EXTRA_CFLAGS += -DBOARD_$(CFG_BOARD)
ifeq ($(PRODUCT_CFG_CHIP_BB_TYPE),CHIP_BB_6756CS)
EXTRA_CFLAGS += -D$(PRODUCT_CFG_CHIP_BB_TYPE) \
                -DBOARD_ASIC_BIGPACK
endif


#子系统相关的宏定义
ifneq ($(strip $(CFG_OS_ANDROID_AARCH64)),YES)
EXTRA_CFLAGS += -D__LINUX_ARM_ARCH__=7 -march=armv7-a -Uarm
endif

EXTRA_CFLAGS += -DVOS_LINUX=4 -DOSA_CPU_ACPU=1 -DVOS_OS_VER=VOS_LINUX -DVOS_OSA_CPU=OSA_CPU_ACPU -D__LOG_RELEASE__ \
                -DBSP_CORE_APP -DGU_MODE -DMSP_SUPPORT_USB_VCOM1 -DMSP_SUPPORT_USB_VCOM2 -DVOS_DOPRA_VER=2 -DNDIS_RELEASE \
                -DBALONG_PHY_V200=2 -DBALONG_PHY_V300=3 -DBALONG_PHY_VER=BALONG_PHY_V300 -DBALONG_CHIP_V200=2 \
				-DBALONG_CHIP_V300=3 -DBALONG_CHIP_V500=4 -DBALONG_CHIP_VER=BALONG_CHIP_V500 -DMSP_GUNAS_AT_UNITE -DVERSION_V3R2

#编译器相关的宏定义
EXTRA_CFLAGS += -fsigned-char 

#头文件目录
EXTRA_CFLAGS += $(CC_INC_DIRS)

export EXTRA_CFLAGS

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_linux_rules.mk
