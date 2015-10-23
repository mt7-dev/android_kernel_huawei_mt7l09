# Need definition 
# 1. CFG_GU_PLATFORM  from make_base.mk
# 2. OBB_PRODUCT_NAME from build command line parameters
# 3. BALONG_TOPDIR    from build_balong_acore.sh
# 4. CFG_BOARD        from build command line parameters 
# 5. CFG_CHIP_BB      from build command line parameters

#Include Common Path
CURR_WORKDIR :=../../../comm/comm
LPS_INC_DIR =$(BALONG_TOPDIR)/modem/ps/comm/comm
PS_COMM_CODE = ps/comm/comm
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

INCLUDE_COMM_DIR_FLAG += \
	-I$(LINUX_KERNEL)/arch/arm/mach-balong/include/mach \
	-I$(LINUX_KERNEL)/include/linux \
	-I$(BALONG_TOPDIR)/config/product/include/$(CFG_GU_PLATFORM_NEW)/ \
	-I$(BALONG_TOPDIR)/config/nvim/include/gu/ \
	-I$(BALONG_TOPDIR)/config/nvim/include/tl/tlps \
	-I$(BALONG_TOPDIR)/config/log \
	-I$(BALONG_TOPDIR)/config/osa \
	-I$(BALONG_TOPDIR)/include/ps/tlps \
	-I$(BALONG_TOPDIR)/include/app \
	-I$(BALONG_TOPDIR)/include/drv \
	-I$(BALONG_TOPDIR)/include/med \
	-I$(BALONG_TOPDIR)/include/oam/lt/diag \
	-I$(BALONG_TOPDIR)/include/oam/lt/msp  \
	-I$(BALONG_TOPDIR)/include/oam/mux \
	-I$(BALONG_TOPDIR)/include/oam/comm/om \
	-I$(BALONG_TOPDIR)/include/oam/comm/osa \
	-I$(BALONG_TOPDIR)/include/oam/gu/nvim \
	-I$(BALONG_TOPDIR)/include/oam/comm/nvim \
	-I$(BALONG_TOPDIR)/include/oam/comm/usimm \
	-I$(BALONG_TOPDIR)/include/phy/gphy \
	-I$(BALONG_TOPDIR)/include/phy/wphy \
	-I$(BALONG_TOPDIR)/include/nv/tl/lps \
	-I$(BALONG_TOPDIR)/include/nv/tl/phy \
	-I$(BALONG_TOPDIR)/include/nv/gu/nas \
	-I$(BALONG_TOPDIR)/include/ps/gups \
	-I$(BALONG_TOPDIR)/include/ps/tlps \
	-I$(BALONG_TOPDIR)/include/ps/nas \
	-I$(BALONG_TOPDIR)/include/taf \
	-I$(BALONG_TOPDIR)/include/tools \
	-I$(BALONG_TOPDIR)/platform/$(CFG_GU_PLATFORM_NEW)/ \
	-I$(BALONG_TOPDIR)/platform/$(CFG_GU_PLATFORM_NEW)/soc \
	-I$(BALONG_TOPDIR)/$(CFG_GU_FEATURE_CONFIG_PATH)/ \
	-I$(BALONG_TOPDIR)/config/product/define/$(OBB_PRODUCT_NAME)/config/ \
	-I$(BALONG_TOPDIR)/config/product/define/$(OBB_PRODUCT_NAME)/ \


#Inclue Your Own Path
INCLUDE_OWN_DIR_FLAG += -I$(LPS_INC_DIR)/LINK/Inc/ \
	-I$(LPS_INC_DIR)/STL/Inc/ \
	-I$(LPS_INC_DIR)/MEM/Inc/ \
	-I$(LPS_INC_DIR)/NDIS/Inc/ \
	-I$(LPS_INC_DIR)/PPP/Inc/ \
	-I$(LPS_INC_DIR)/ND/IPCOMM/Inc/ \
	-I$(LPS_INC_DIR)/ND/NDSERVER/Inc/ 

#LPS.O dependencies CURR_WORKDIR
LPSOBJ := $(CURR_WORKDIR)/STL/Src/LUPQueue.o\
	$(CURR_WORKDIR)/NDIS/Src/PsNdis.o\
	$(CURR_WORKDIR)/NDIS/Src/Ipv4DhcpServer.o\
	$(CURR_WORKDIR)/ND/NDSERVER/Src/IpNdServer.o\
	$(CURR_WORKDIR)/ND/IPCOMM/Src/IpComm.o

LPS_CMD_OBJ := $(foreach dir,$(dir $(LPSOBJ)),$(dir).*.o.cmd)
# Target
LPS-objs     += $(LPSOBJ)
obj-$(OBB_CFG_BUILDIN)        += LPS.o

# User definition
EXTRA_CFLAGS += -fsigned-char -DVOS_LINUX=4 -DOSA_CPU_ACPU=1 -DVOS_OS_VER=VOS_LINUX -DVOS_OSA_CPU=OSA_CPU_ACPU -D__LOG_RELEASE__ -DBSP_CORE_APP -DGU_MODE -DMSP_SUPPORT_USB_VCOM1 -DMSP_SUPPORT_USB_VCOM2 -DVOS_DOPRA_VER=2 -DNDIS_RELEASE -DBALONG_PHY_V200=2 -DBALONG_PHY_V300=3 -DBALONG_PHY_VER=BALONG_PHY_V300 -DBALONG_CHIP_V200=2 -DBALONG_CHIP_V300=3 -DBALONG_CHIP_V500=4 -DBALONG_CHIP_VER=BALONG_CHIP_V500 -DVERSION_V7R1_C010 -DMSP_GUNAS_AT_UNITE -DVERSION_V3R2
#Defined in top shell script(Comm Definition)
EXTRA_CFLAGS += $(INCLUDE_FLAG) $(DRV_INCLUDE_FLAG) $(ASM_INCLUDE_FLAG) $(PLATFORM_FLAG)
EXTRA_CFLAGS += $(INCLUDE_COMM_DIR_FLAG) $(INCLUDE_OWN_DIR_FLAG)

TARGET_DIR       := $(OUT_KERNEL)/drivers/lps

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_linux_rules.mk