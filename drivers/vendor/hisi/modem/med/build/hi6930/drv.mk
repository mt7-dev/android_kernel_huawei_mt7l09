####################################################################################################
#
####################################################################################################


####################################################################################################
#Directories for library files 
####################################################################################################
vob_lib_dirs :=

####################################################################################################
#library files
####################################################################################################
vob_lib_files :=

####################################################################################################
#Directories for include files
####################################################################################################

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs := $(CODEC_SRC_FILE_PATH)/drv/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files :=$(vob_src_dirs)/med_drv_dma.c        \
	$(vob_src_dirs)/med_drv_ipc.c        \
	$(vob_src_dirs)/med_drv_mailbox.c    \
	$(vob_src_dirs)/med_drv_sio.c        \
	$(vob_src_dirs)/med_drv_timer.c      \
	$(vob_src_dirs)/med_drv_watchdog.c   \
	$(vob_src_dirs)/med_drv_uart.c       \
	

####################################################################################################
#general make rules
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################
