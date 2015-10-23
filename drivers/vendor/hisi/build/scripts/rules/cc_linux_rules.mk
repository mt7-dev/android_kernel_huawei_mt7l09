include $(BALONG_TOPDIR)/build/scripts/make_base.mk
#****************************************************************************
# input parameters
#****************************************************************************
OBC_LOCAL_CORE_NAME		?=acore
OBC_LOCAL_MOUDLE_NAME	?=

OBC_COMPONENT_DIR		?=

KERNEL_FLAGS			?=
KERNEL_VERBOSE			?=

VERBOSE				?=2



#****************************************************************************
# rules
#****************************************************************************
.PHONY: all mkobj cpobj

all : mkobj cpobj
	
mkobj : 
	#$(Q)mkdir -p $(OUT_ACORE_DIR)
	@echo INCLUDE_FLAG=$(INCLUDE_FLAG) DRV_INCLUDE_FLAG=$(DRV_INCLUDE_FLAG) ASM_INCLUDE_FLAG=$(ASM_INCLUDE_FLAG) PLATFORM_FLAG=$(PLATFORM_FLAG) EXTRA_CFLAGS=$(EXTRA_CFLAGS)
ifeq ($(CFG_BOOT_OPTI_BUILDIN),YES)
	$(MAKE) -C $(LINUX_KERNEL) $(KERNEL_FLAGS) O=$(OUT_KERNEL) M=$(CURDIR) $(OBB_JOBS) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) $(DISTCC_KERNEL) 
else
	#$(MAKE) -C $(LINUX_KERNEL) $(KERNEL_FLAGS) M=$(CURDIR) modules $(OBB_JOBS) $(DISTCC_KERNEL) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C $(LINUX_KERNEL) $(KERNEL_FLAGS) O=$(OUT_KERNEL) M=$(CURDIR) $(OBB_JOBS) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) $(DISTCC_KERNEL) 
endif

cpobj : mkobj
	$(Q)cp -fp $(V) *.o $(OUT_ACORE_DIR)
	$(Q)rm -f $(V) $(OUT_ACORE_DIR)/built-in.o
	$(Q)rm -f $(V) $(OUT_ACORE_DIR)/*mod.o

.PHONY:clean
clean:
	if [ -d "$(OUT_KERNEL)" ]; then \
		$(MAKE) -s -C $(OUT_KERNEL)  $(KERNEL_FLAGS) M=$(CURDIR) clean V=$(VERBOSE); \
	fi;
	
	$(shell if [ "$(GUPS_CMD_OBJS)" ]; then \
		echo rm -f $(GUPS_CMD_OBJS); \
	fi;)	
	$(shell if [ "$(GUPS_OBJS)" ]; then \
		echo rm -f $(GUPS_OBJS); \
	fi;)
	$(shell if [ "$(GUMSP_CMD_OBJS)" ]; then \
		echo rm -f $(GUMSP_CMD_OBJS); \
	fi;)	
	$(shell if [ "$(GUMSP_OBJS)" ]; then \
		echo rm -f $(GUMSP_OBJS); \
	fi;)
	
	$(shell if [ "$(LMSPOBJ)" ]; then \
		echo rm -f $(LMSPOBJ); \
	fi;)	
	$(shell if [ "$(LMSP_CMD_OBJ)" ]; then \
		echo rm -f $(LMSP_CMD_OBJ); \
	fi;)
	
	$(shell if [ "$(LPSOBJ)" ]; then \
		echo rm -f $(LPSOBJ); \
	fi;)	
	$(shell if [ "$(LPS_CMD_OBJ)" ]; then \
		echo rm -f $(LPS_CMD_OBJ); \
	fi;)
