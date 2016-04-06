# Core information
include $(BALONG_TOPDIR)/build/scripts/make_base.mk
MSPDIR = ../../../../oam/gu

MSP_GU_SRC_FILES = \
	$(MSPDIR)/acore/pslog/pslogapp.c\
	$(MSPDIR)/acore/pslog/pslogadapterapp.c\
	$(MSPDIR)/acore/om/omappcommrx.c\
	$(MSPDIR)/acore/om/omapp.c\
	$(MSPDIR)/acore/om/omapprl.c\
	$(MSPDIR)/acore/om/omappoutside.c\
	$(MSPDIR)/acore/om/omappagent.c\
	$(MSPDIR)/acore/om/omprintf.c\
	$(MSPDIR)/acore/om/ompcvoice.c\
	$(MSPDIR)/acore/om/omsdlog.c\
	$(MSPDIR)/acore/om/omerrorlog.c\
	$(MSPDIR)/acore/om/fastlz.c\
	$(MSPDIR)/acore/bbplog/omappbbplog.c\
	$(MSPDIR)/acore/om/Omappoperator.c\
	$(MSPDIR)/comm/om/OmHdlc.c\
	$(MSPDIR)/comm/om/omringbuffer.c\
	$(MSPDIR)/comm/om/ombufmngr.c

ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_ON)
MSP_GU_SRC_FILES += \
	$(MSPDIR)/acore/cbtcpm/CbtCpm.c\
	$(MSPDIR)/acore/cbtppm/CbtPpm.c\
	$(MSPDIR)/acore/cbtscm/CBTSCMSoftDecode.c
endif

ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_OFF)
MSP_GU_SRC_FILES += \
	$(MSPDIR)/acore/om/cpm.c\
	$(MSPDIR)/comm/om/omsock.c
endif
