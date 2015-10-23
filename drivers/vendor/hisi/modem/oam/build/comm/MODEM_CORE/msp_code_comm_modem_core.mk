####################################################################################################
#
####################################################################################################
USER_CCFLAGS :=

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
include $(BALONG_SRC_PATH)/modem/oam/build/comm/MODEM_CORE/msp_code_comm_modem_core.inc
ifneq ($(vos_comm_compile),)
####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs :=

MSP_MCORE_DIR			:=$(BALONG_SRC_PATH)/modem/oam/comm/comm
MSP_CCORE_DIR  			:=$(BALONG_SRC_PATH)/modem/oam/comm/ccore
PRJ_CONFIG_DIR          :=$(BALONG_SRC_PATH)/config
PRJ_INCLUDE_DIR         :=$(BALONG_SRC_PATH)/include
PRJ_PLATFORM_DIR        :=$(BALONG_SRC_PATH)/platform
####################################################################################################
#Source files
####################################################################################################
vob_src_files :=    $(MSP_MCORE_DIR)/osa/rtc_timer.c\
					$(MSP_MCORE_DIR)/osa/v_blkmem.c\
					$(MSP_MCORE_DIR)/osa/v_int.c\
					$(MSP_MCORE_DIR)/osa/v_lib.c\
					$(MSP_MCORE_DIR)/osa/v_msg.c\
					$(MSP_MCORE_DIR)/osa/v_nsprintf.c\
					$(MSP_MCORE_DIR)/osa/v_queue.c\
					$(MSP_MCORE_DIR)/osa/v_sprintf.c\
					$(MSP_MCORE_DIR)/osa/v_timer.c\
					$(MSP_MCORE_DIR)/osa/vos_Id.c\
					$(MSP_MCORE_DIR)/osa/vos_main.c\
					$(MSP_MCORE_DIR)/osa/vos_outside.c\
					$(MSP_MCORE_DIR)/osa/watchdog.c\
					$(MSP_CCORE_DIR)/osa/monitordsp.c\
					$(MSP_CCORE_DIR)/osa/nucleus_sem.c\
					$(MSP_CCORE_DIR)/osa/nucleus_sock.c\
					$(MSP_CCORE_DIR)/osa/nucleus_task.c\
					$(MSP_CCORE_DIR)/osa/v_sem.c\
					$(MSP_CCORE_DIR)/osa/v_sock.c\
					$(MSP_CCORE_DIR)/osa/v_task.c\
					$(MSP_CCORE_DIR)/om/spysystem.c\
                    			$(MSP_CCORE_DIR)/usimm/UsimmInstanceApi.c\
					$(MSP_CCORE_DIR)/usimm/usimmapdu.c\
					$(MSP_CCORE_DIR)/usimm/UsimmApi.c\
					$(MSP_CCORE_DIR)/usimm/usimmapicnf.c\
					$(MSP_CCORE_DIR)/usimm/usimmbase.c\
					$(MSP_CCORE_DIR)/usimm/usimmcommon.c\
					$(MSP_CCORE_DIR)/usimm/usimmdl.c\
					$(MSP_CCORE_DIR)/usimm/usimmgobal.c\
					$(MSP_CCORE_DIR)/usimm/usimmpool.c\
					$(MSP_CCORE_DIR)/usimm/usimmt1dl.c\
					$(MSP_CCORE_DIR)/usimm/usimmvsim.c\
					$(MSP_CCORE_DIR)/usimm/usimmvsimauth.c\
					$(MSP_MCORE_DIR)/scm/SCMProc.c\
					$(MSP_MCORE_DIR)/si/pbapi.c\
					$(MSP_MCORE_DIR)/si/pbdata.c\
					$(MSP_MCORE_DIR)/si/pbmsg.c\
					$(MSP_MCORE_DIR)/si/pihapi.c\
					$(MSP_MCORE_DIR)/si/stkapi.c\
					$(MSP_CCORE_DIR)/si/pbbase.c\
					$(MSP_CCORE_DIR)/si/pihproc.c\
					$(MSP_CCORE_DIR)/si/stkcomm.c\
					$(MSP_CCORE_DIR)/si/stkgobal.c\
					$(MSP_CCORE_DIR)/si/stkproc.c\
					$(MSP_CCORE_DIR)/si/stkispecialproc.c\
					$(MSP_CCORE_DIR)/si/StkAtprintf.c \
					$(MSP_CCORE_DIR)/si/csimagent.c\
                                        $(MSP_CCORE_DIR)/cbpa/cbpacommagent.c\
					$(MSP_CCORE_DIR)/xml/OamXmlComm.c\
					$(MSP_CCORE_DIR)/dh/aes_core.c\
					$(MSP_CCORE_DIR)/dh/aes_util.c\
					$(MSP_CCORE_DIR)/dh/md5c.c\
					$(MSP_CCORE_DIR)/dh/nn.c\
					$(MSP_CCORE_DIR)/dh/r_dh.c\
					$(MSP_CCORE_DIR)/dh/rand.c\

endif   

####################################################################################################
#general make rules
####################################################################################################
# ifeq ($(PRODUCT_CFG_PRODUCT_VERSION),VERSION_V3R2)
# include $(BALONG_SRC_PATH)\build\Build\MODEM_CORE\Rule\rules.msp.mk
# endif

####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################