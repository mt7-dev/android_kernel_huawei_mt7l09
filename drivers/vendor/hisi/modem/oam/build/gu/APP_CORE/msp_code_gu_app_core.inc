# Core information
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MSP_INC_DIRS := \
	$(LINUX_KERNEL)/include/linux \
	$(LINUX_KERNEL)/include/generated \
	$(LINUX_KERNEL)/arch/arm/mach-balong/include/mach \
	$(BALONG_TOPDIR)/config/product/define/$(OBB_PRODUCT_NAME)/config/ \
	$(BALONG_TOPDIR)/config/product/include/$(OBB_PRODUCT_NAME)/ \
	$(BALONG_TOPDIR)/config/log \
	$(BALONG_TOPDIR)/config/nvim/include/gu \
	$(BALONG_TOPDIR)/config/osa \
	$(BALONG_TOPDIR)/config/nvim/include/lt/tlps \
	$(BALONG_TOPDIR)/include/app/ \
	$(BALONG_TOPDIR)/include/drv/ \
	$(BALONG_TOPDIR)/include/med/ \
	$(BALONG_TOPDIR)/include/oam/comm/om \
	$(BALONG_TOPDIR)/include/oam/comm/osa \
	$(BALONG_TOPDIR)/include/oam/comm/scm \
	$(BALONG_TOPDIR)/include/oam/comm/si \
	$(BALONG_TOPDIR)/include/oam/comm/socp \
	$(BALONG_TOPDIR)/include/oam/comm/usimm \
	$(BALONG_TOPDIR)/include/oam/comm/errno \
	$(BALONG_TOPDIR)/include/oam/comm/dms \
	$(BALONG_TOPDIR)/include/oam/comm/nvim \
	$(BALONG_TOPDIR)/include/oam/comm/cpm \
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/nv/gu/codec \
	$(BALONG_TOPDIR)/include/nv/gu/drv \
	$(BALONG_TOPDIR)/include/nv/gu/gas \
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/nv/gu/phy \
	$(BALONG_TOPDIR)/include/nv/gu/tool \
	$(BALONG_TOPDIR)/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/oam/gu/log \
	$(BALONG_TOPDIR)/include/oam/gu/om \
	$(BALONG_TOPDIR)/include/oam/gu/nvim \
	$(BALONG_TOPDIR)/include/oam/gu/hpa \
	$(BALONG_TOPDIR)/include/oam/lt/msp \
	$(BALONG_TOPDIR)/include/oam/lt/diag \
	$(BALONG_TOPDIR)/include/phy/gphy \
	$(BALONG_TOPDIR)/include/phy/wphy \
	$(BALONG_TOPDIR)/include/ps/gups \
	$(BALONG_TOPDIR)/include/ps/tlps \
	$(BALONG_TOPDIR)/include/ps/nas \
	$(BALONG_TOPDIR)/include/taf \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM_HISI_BALONG)/ \

	
MSP_INC_DIRS += \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/acore/dms \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/ccore/om \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/ccore/usimm \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/comm/si \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/comm/nvim \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/acore/om \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/hpa \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/nvim \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/om \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/pstool \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/sc \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/ccore/sleep \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/comm/om \
    $(BALONG_TOPDIR)/modem/oam/inc/gu/comm/bbplog \
	
MSP_INC_DIRS += \
	$(BALONG_TOPDIR)/modem/taf/inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/at/inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/tafapi/inc \
	$(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/TAFAGENT/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/APPVCOM/Inc/ \
	
		
ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_ON)
MSP_INC_DIRS += \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/acore/cbtcpm \
	$(BALONG_TOPDIR)/modem/oam/inc/gu/acore/cbtppm \
	$(BALONG_TOPDIR)/modem/oam/inc/comm/comm/ppm
endif
