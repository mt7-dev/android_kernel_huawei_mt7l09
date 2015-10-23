BSP_CUR_DIR = $(BSP_DIR)/drivers/multicore/udi
OBJFILE		= BSP_UDI.o BSP_UDI_adp.o

$(OBJFILE):%.o:$(addprefix $(BSP_CUR_DIR)/,%.c)
	$(RM) $@
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $<