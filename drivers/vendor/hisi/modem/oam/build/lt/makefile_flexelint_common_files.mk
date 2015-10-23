
include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

OAM_DIR := $(BALONG_TOPDIR)/modem/oam
TAF_DIR := $(BALONG_TOPDIR)/modem/taf

#specific optionss
PCLINT_OPTION +=

#header file dir
PCLINT_INC_VAR += \
	-I$(BALONG_TOPDIR)/config/product/OBB_PRODUCT_NAME/config \
	-I$(BALONG_TOPDIR)/config/product/OBB_PRODUCT_NAME/include_gu \
	-I$(BALONG_TOPDIR)/config/nvim/include/gu \
	-I$(BALONG_TOPDIR)/include/drv \
	-I$(BALONG_TOPDIR)/include/nv/tl/drv \
	-I$(BALONG_TOPDIR)/include/nv/tl/oam \
	-I$(BALONG_TOPDIR)/include/nv/tl/lps \
	-I$(BALONG_TOPDIR)/include/phy/lphy \
	-I$(BALONG_TOPDIR)/platform/CFG_PLATFORM \
	-I$(BALONG_TOPDIR)/platform/CFG_PLATFORM/soc \
	-I$(BALONG_TOPDIR)/modem/drv/common/include \
	-I$(BALONG_TOPDIR)/include/oam/comm/err.c \
	-I$(BALONG_TOPDIR)/include/oam/comm/nvim \
	-I$(BALONG_TOPDIR)/include/oam/comm/om \
	-I$(BALONG_TOPDIR)/include/oam/comm/osa \
	-I$(BALONG_TOPDIR)/include/oam/comm/scm \
	-I$(BALONG_TOPDIR)/include/oam/comm/si \
	-I$(BALONG_TOPDIR)/include/oam/comm/socp \
	-I$(BALONG_TOPDIR)/include/oam/comm/usimm \
	-I$(BALONG_TOPDIR)/include/oam/lt/diag \
	-I$(BALONG_TOPDIR)/include/oam/lt/msp \
	-I$(BALONG_TOPDIR)/include/taf 

#source files which should be checked
OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/lt/ccore/bbpds/bbp_datalog.c \
	$(OAM_DIR)/lt/ccore/diag/diag_agent.c \
	$(OAM_DIR)/lt/ccore/sleep/lte_sleepflow.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/lt/comm/diag/diag_api.c \
	$(OAM_DIR)/lt/comm/diag/diag_cfg.c \
	$(OAM_DIR)/lt/comm/diag/diag_debug.c \
	$(OAM_DIR)/lt/comm/diag/diag_buf_ctrl.c \
	$(OAM_DIR)/lt/comm/diag/diag_common.c \
	$(OAM_DIR)/lt/comm/diag/diag_fs_proc.c \
	$(OAM_DIR)/lt/comm/drx/drx_msp_api.c \
	$(OAM_DIR)/lt/comm/list/blist.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/ccore/dh/aes_core.c \
	$(OAM_DIR)/comm/ccore/dh/aes_util.c \
	$(OAM_DIR)/comm/ccore/dh/md5c.c \
	$(OAM_DIR)/comm/ccore/dh/nn.c \
	$(OAM_DIR)/comm/ccore/dh/rand.c \
	$(OAM_DIR)/comm/ccore/dh/r_dh.c \
	$(OAM_DIR)/comm/ccore/om/spysystem.c \
	$(OAM_DIR)/comm/ccore/osa/monitordsp.c \
	$(OAM_DIR)/comm/ccore/osa/nucleus_sock.c \
	$(OAM_DIR)/comm/ccore/osa/v_sem.c \
	$(OAM_DIR)/comm/ccore/osa/v_task.c \
	$(OAM_DIR)/comm/ccore/osa/v_sock.c \
	$(OAM_DIR)/comm/ccore/si/pbbase.c \
	$(OAM_DIR)/comm/ccore/si/StkAtprintf.c \
	$(OAM_DIR)/comm/ccore/si/stkgobal.c \
	$(OAM_DIR)/comm/ccore/si/stkproc.c \
	$(OAM_DIR)/comm/ccore/si/pihproc.c \
	$(OAM_DIR)/comm/ccore/si/stkcomm.c \
	$(OAM_DIR)/comm/ccore/si/stkispecialproc.c 
	
OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/ccore/usimm/usimmapdu.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmbase.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmgobal.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmvsimauth.c \
	$(OAM_DIR)/comm/ccore/usimm/UsimmApi.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmcommon.c \
	$(OAM_DIR)/comm/ccore/usimm/UsimmInstanceApi.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmvsim.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmapicnf.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmdl.c \
	$(OAM_DIR)/comm/ccore/usimm/usimmpool.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/comm/osa/rtc_timer.c \
	$(OAM_DIR)/comm/comm/osa/v_blkmem.c \
	$(OAM_DIR)/comm/comm/osa/v_int.c \
	$(OAM_DIR)/comm/comm/osa/v_lib.c \
	$(OAM_DIR)/comm/comm/osa/v_msg.c \
	$(OAM_DIR)/comm/comm/osa/v_nsprintf.c \
	$(OAM_DIR)/comm/comm/osa/vos_Id.c \
	$(OAM_DIR)/comm/comm/osa/vos_main.c \
	$(OAM_DIR)/comm/comm/osa/vos_outside.c \
	$(OAM_DIR)/comm/comm/osa/vos_queue.c \
	$(OAM_DIR)/comm/comm/osa/v_sprintf.c \
	$(OAM_DIR)/comm/comm/osa/v_timer.c \
	$(OAM_DIR)/comm/comm/osa/watchdog.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/comm/scm/SCMproc.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(OAM_DIR)/comm/comm/si/pbapi.c \
	$(OAM_DIR)/comm/comm/si/pbdata.c \
	$(OAM_DIR)/comm/comm/si/pbmsg.c \
	$(OAM_DIR)/comm/comm/si/pihapi.c \
	$(OAM_DIR)/comm/comm/si/stkapi.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_base_cmd.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_comm_cmd.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_common_api.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_ct_core.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_lte_def.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_lte_nonsignal_bt.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_lte_quick_ct.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_mailbox_proc.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_tds_cmd.c \
	$(TAF_DIR)/lt/src/ccore/ftm/ftm_tdscom.c 

OBC_LOCAL_PCLINT_SRC_FILE += \
	$(TAF_DIR)/lt/src/ccore/l4a/l4a_fun.c \
	$(TAF_DIR)/lt/src/ccore/l4a/l4a_proc.c 
