
include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

OAM_DIR := $(BALONG_TOPDIR)/modem/oam
TAF_DIR := $(BALONG_TOPDIR)/modem/taf

#specific options
PCLINT_OPTION +=

#include dir
PCLINT_INC_VAR += \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/ccore/sleep \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/acore/diag \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/diag \
    -I$(BALONG_TOPDIR)/modem/oam/inc/comm/comm/si \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/diag \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/drx \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/list \
    -I$(BALONG_TOPDIR)/modem/oam/inc/lt/comm/type \
    -I$(BALONG_TOPDIR)/include/ps/tlps \
    -I$(BALONG_TOPDIR)/modem/taf/comm/src/acore/at/inc \
    -I$(BALONG_TOPDIR)/include/taf \
    -I$(BALONG_TOPDIR)/include/oam/lt/msp \
    -I$(BALONG_TOPDIR)/include/oam/gu/om \
    -I$(BALONG_TOPDIR)/include/oam/comm/errno \
	-I$(BALONG_TOPDIR)/include/oam/comm/osa \
	-I$(BALONG_TOPDIR)/include/drv \
	-I$(BALONG_TOPDIR)/include/oam/lt/diag \
	-I$(BALONG_TOPDIR)/include/oam/comm/nvim \
	-I$(BALONG_TOPDIR)/include/oam/comm/scm \
    -I$(BALONG_TOPDIR)/include/oam/comm/socp \
    -I$(BALONG_TOPDIR)/modem/system/android/android_4.2_r1/kernel/include/linux/
    
OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/acore/osa/linux_sem.c \
	$(OAM_DIR)/comm/acore/osa/linux_task.c \
	$(OAM_DIR)/lt/acore/diag/diag_app_agent.c \
	$(OAM_DIR)/lt/acore/diag/diag_fw.c \
	$(OAM_DIR)/lt/acore/diag/diag_port.c \
	$(OAM_DIR)/lt/acore/diag/diag_tcp.c \
	$(OAM_DIR)/lt/acore/diag/diag_vcom.c \
	$(OAM_DIR)/lt/acore/diag/diag_hisc.c \
	$(OAM_DIR)/lt/acore/diag/diag_sd.c \
	$(OAM_DIR)/lt/acore/diag/diag_usb.c \
	$(OAM_DIR)/lt/acore/diag/socp_lfix_chan.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(TAF_DIR)/lt/src/acore/at/at_comm_cmd.c \
	$(TAF_DIR)/lt/src/acore/at/AtDeviceCmdTL.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_common.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_ct_ex2_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_ct_ex_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_ct_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_eventreport.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_ms_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_set_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_st_proc.c \
	$(TAF_DIR)/lt/src/acore/at/at_lte_upgrade_proc.c 
