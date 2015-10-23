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
include $(BALONG_SRC_PATH)/modem/oam/build/gu/MODEM_CORE/msp_code_gu_modem_core.inc

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs :=

MSP_MCORE_DIR			:=$(BALONG_SRC_PATH)/modem/oam/gu/comm
MSP_CCORE_DIR  			:=$(BALONG_SRC_PATH)/modem/oam/gu/ccore
PRJ_CONFIG_DIR          :=$(BALONG_SRC_PATH)/config
PRJ_INCLUDE_DIR         :=$(BALONG_SRC_PATH)/include
PRJ_PLATFORM_DIR        :=$(BALONG_SRC_PATH)/platform
####################################################################################################
#Source files
####################################################################################################
vob_src_files  := $(MSP_CCORE_DIR)/sleep/sleepflow.c
ifeq ($(INSTANCE_ID), INSTANCE_0)
vob_src_files  +=                       $(MSP_CCORE_DIR)/led/lightled.c\
					$(MSP_CCORE_DIR)/pslog/pslog.c\
					$(MSP_CCORE_DIR)/pslog/pslogadapter.c\
					$(MSP_CCORE_DIR)/tf/updatefromtf.c\
					$(MSP_CCORE_DIR)/hpa/apmprocess.c\
					$(MSP_CCORE_DIR)/hpa/hpaentity.c\
					$(MSP_CCORE_DIR)/hpa/hpaglobalvar.c\
					$(MSP_CCORE_DIR)/hpa/hpainit.c\
					$(MSP_CCORE_DIR)/hpa/hpaisr.c\
					$(MSP_CCORE_DIR)/hpa/hpaoperatertt.c\
					$(MSP_CCORE_DIR)/hpa/hparingbuffer.c\
					$(MSP_CCORE_DIR)/pstool/psregrpt.c\
					$(MSP_CCORE_DIR)/sleep/sleepinit.c\
					$(MSP_MCORE_DIR)/om/OmHdlc.c\
					$(MSP_MCORE_DIR)/om/omringbuffer.c\
					$(MSP_MCORE_DIR)/om/ombufmngr.c\
					$(MSP_CCORE_DIR)/om/rfa.c\
					$(MSP_CCORE_DIR)/om/om.c\
					$(MSP_CCORE_DIR)/om/omrl.c\
					$(MSP_CCORE_DIR)/om/omoutside.c\
					$(MSP_CCORE_DIR)/om/omfilesystem.c\
					$(MSP_CCORE_DIR)/om/ommodemcommrx.c\
					$(MSP_CCORE_DIR)/om/monitor.c\
					$(MSP_CCORE_DIR)/om/agingtest.c\
					$(MSP_CCORE_DIR)/om/omoperator.c\
					$(MSP_CCORE_DIR)/om/omnosig.c\
					$(MSP_CCORE_DIR)/om/omagent.c\
					$(MSP_CCORE_DIR)/om/omlittleimagelog.c\
					$(MSP_CCORE_DIR)/sc/ScApSec.c\
					$(MSP_CCORE_DIR)/sc/ScComm.c\
					$(MSP_CCORE_DIR)/sc/ScCtx.c\
					$(MSP_CCORE_DIR)/sc/ScFactory.c\
					$(MSP_CCORE_DIR)/sc/ScPersonalization.c\
					$(MSP_CCORE_DIR)/bbplog/ombbplog.c
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