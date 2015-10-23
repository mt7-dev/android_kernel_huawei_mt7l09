# Core information



PS_INC_DIRS := \
	$(LINUX_KERNEL)/arch/arm/mach-balong/include/mach \
	$(LINUX_KERNEL)/arch/arm/include/asm \
	$(LINUX_KERNEL)/include/linux \
	$(LINUX_KERNEL)/include/generated 


PS_INC_DIRS += \
	$(BALONG_TOPDIR)/config/product/include/$(OBB_PRODUCT_NAME)/ \
	$(BALONG_TOPDIR)/config/product/define/$(OBB_PRODUCT_NAME)/config/ \
        $(BALONG_TOPDIR)/include/nv/gu/codec \
	$(BALONG_TOPDIR)/include/app/ \
	$(BALONG_TOPDIR)/include/drv/ \
	$(BALONG_TOPDIR)/include/med/ \
	$(BALONG_TOPDIR)/include/phy/gphy \
	$(BALONG_TOPDIR)/include/phy/wphy \
	$(BALONG_TOPDIR)/include/ps/gups \
	$(BALONG_TOPDIR)/include/ps/tlps \
	$(BALONG_TOPDIR)/include/ps/nas \
	$(BALONG_TOPDIR)/include/taf \
	$(BALONG_TOPDIR)/include/sdio \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/config/log \
	$(BALONG_TOPDIR)/config/nvim/include/gu \
	$(BALONG_TOPDIR)/config/nvim/include/tl/tlps \
	$(BALONG_TOPDIR)/config/osa \
	$(BALONG_TOPDIR)/include/oam/comm/om \
	$(BALONG_TOPDIR)/include/oam/comm/osa \
	$(BALONG_TOPDIR)/include/oam/comm/scm \
	$(BALONG_TOPDIR)/include/oam/comm/si \
	$(BALONG_TOPDIR)/include/oam/comm/socp \
	$(BALONG_TOPDIR)/include/oam/comm/usimm \
	$(BALONG_TOPDIR)/include/oam/comm/errno \
	$(BALONG_TOPDIR)/include/oam/comm/nvim \
	$(BALONG_TOPDIR)/include/oam/gu/log \
	$(BALONG_TOPDIR)/include/oam/gu/om \
	$(BALONG_TOPDIR)/include/oam/gu/nvim \
	$(BALONG_TOPDIR)/include/oam/gu/hpa \
	$(BALONG_TOPDIR)/include/oam/lt/msp \
	$(BALONG_TOPDIR)/include/oam/lt/diag \
	$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM_HISI_BALONG)/ \
	
PS_INC_DIRS += \
        $(BALONG_TOPDIR)/modem/ps/inc/gu/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/LINK/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/MEMCTRL/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/DICC/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/DIPC/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/MUX/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/IMM/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/STL/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/NDIS/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/PPP/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/ND/IPCOMM/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/comm/comm/ND/NDSERVER/Inc/ \
	$(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/ADS/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/CSD/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/RNIC/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/at/inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/tafapi/inc \
	$(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/TAFAGENT/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/comm/src/acore/APPVCOM/Inc/ \
	$(BALONG_TOPDIR)/modem/taf/lt/inc/ccore/gen/ 





