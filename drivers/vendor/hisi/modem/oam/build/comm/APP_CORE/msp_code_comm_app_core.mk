# Core information

MSPDIR = ../../../../oam/comm
ifneq ($(vos_comm_compile),)

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
endif