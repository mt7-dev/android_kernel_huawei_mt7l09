##########################################################################
# ptable module compiler process.
# Output:ptable.obj
##########################################################################
BSP_CUR_DIR = $(BSP_DIR)/services/ptable
INCLUDE_DIR  = $(BSP_DIR)/bootloader/code/bootloader

INCLUDE_DIR  += $(BSP_DIR)/bootloader

ptable.obj: $(OBJFILE_PTABLE) 
	$(LD) $(LD_PARTIAL_FLAGS) $(OBJFILE_PTABLE) -o $@
	$(RM) $(OBJFILE_PTABLE)

$(OBJFILE_PTABLE):%.o:$(addprefix $(BSP_CUR_DIR)/,%.c)
	$(RM) $@
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(CFLAGS_OPTIONS_ALL) -c  $<
