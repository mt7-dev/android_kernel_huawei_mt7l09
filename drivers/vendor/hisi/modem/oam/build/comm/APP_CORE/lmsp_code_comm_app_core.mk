# Core information
-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
MSPDIR = ../../../../oam/comm


MSP_COMM_SRC_FILES := \
	$(MSPDIR)/acore/osa/linux_sem.c\
	$(MSPDIR)/acore/osa/linux_task.c\
	$(MSPDIR)/acore/si/PcscProc.c\
	$(MSPDIR)/comm/osa/v_blkmem.c\
	$(MSPDIR)/comm/osa/v_int.c\
	$(MSPDIR)/comm/osa/rtc_timer.c\
	$(MSPDIR)/comm/osa/v_queue.c\
	$(MSPDIR)/comm/osa/v_timer.c\
	$(MSPDIR)/comm/osa/v_msg.c\
	$(MSPDIR)/comm/osa/v_lib.c\
	$(MSPDIR)/comm/osa/vos_Id.c\
	$(MSPDIR)/comm/osa/vos_main.c\
	$(MSPDIR)/comm/osa/vos_outside.c\
	$(MSPDIR)/comm/osa/watchdog.c\
	$(MSPDIR)/comm/osa/v_nsprintf.c\
	$(MSPDIR)/comm/osa/v_sprintf.c\
	$(MSPDIR)/comm/si/pbapi.c\
	$(MSPDIR)/comm/si/pbmsg.c\
	$(MSPDIR)/comm/si/pbdata.c\
	$(MSPDIR)/comm/si/stkapi.c\
	$(MSPDIR)/comm/si/pihapi.c\
	$(MSPDIR)/comm/scm/SCMProc.c
	
ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_ON)
MSP_COMM_SRC_FILES += \
	$(MSPDIR)/acore/cpm/cpm.c\
	$(MSPDIR)/acore/ppm/OmCommonPpm.c\
	$(MSPDIR)/acore/ppm/OmFsPpm.c\
	$(MSPDIR)/acore/ppm/OmHsicPpm.c\
	$(MSPDIR)/acore/ppm/OmSdPpm.c\
	$(MSPDIR)/acore/ppm/OmSocketPpm.c\
	$(MSPDIR)/acore/ppm/OmUsbPpm.c\
	$(MSPDIR)/acore/ppm/OmVcomPpm.c\
	$(MSPDIR)/comm/scm/SCMSoftDecode.c
endif
