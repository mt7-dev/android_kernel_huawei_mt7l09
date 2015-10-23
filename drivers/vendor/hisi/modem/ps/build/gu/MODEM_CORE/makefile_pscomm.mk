#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=pscomm_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=pscomm1_ccore
endif

#***********************************************************#
# compiler flags
#***********************************************************#
#COMM FLAGS
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm_flags.mk

#USER FLAGS
CC_USER_FLAGS   ?=
AS_USER_FLAGS   ?=

CC_USER_FLAGS += -falign-arrays

ifeq ($(CFG_THUMB_COMPILE),YES)
CC_USER_FLAGS += -mlong-calls
endif

#***********************************************************#
# compiler defines
#***********************************************************#
#COMM DEFINES
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm_defines.mk

#USER DEFINES
CC_USER_DEFINES ?=

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
CC_USER_DEFINES	+=-DINSTANCE_1
endif
				
#***********************************************************#
# include Directories
#***********************************************************#
#common include directories
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm.inc

#ps common include directories
OBC_LOCAL_INC_DIR ?=
OBC_LOCAL_INC_DIR += \
	$(BALONG_TOPDIR)/config/log \
	$(BALONG_TOPDIR)/config/nvim/include/gu \
    $(BALONG_TOPDIR)/include/nv/gu/codec \
	$(BALONG_TOPDIR)/config/osa \
	$(BALONG_TOPDIR)/include/drv \
	$(BALONG_TOPDIR)/include/oam/comm/om \
	$(BALONG_TOPDIR)/include/oam/comm/osa \
	$(BALONG_TOPDIR)/include/oam/comm/scm \
	$(BALONG_TOPDIR)/include/oam/comm/si \
	$(BALONG_TOPDIR)/include/oam/comm/socp \
	$(BALONG_TOPDIR)/include/oam/comm/usimm \
	$(BALONG_TOPDIR)/include/oam/comm/errno \
	$(BALONG_TOPDIR)/include/oam/comm/dms \
	$(BALONG_TOPDIR)/include/oam/comm/nvim \
	$(BALONG_TOPDIR)/include/oam/gu/log \
	$(BALONG_TOPDIR)/include/oam/gu/om \
	$(BALONG_TOPDIR)/include/oam/gu/nvim \
	$(BALONG_TOPDIR)/include/oam/gu/hpa \
	$(BALONG_TOPDIR)/include/oam/lt/msp \
	$(BALONG_TOPDIR)/include/phy/gphy \
	$(BALONG_TOPDIR)/include/phy/wphy \
	$(BALONG_TOPDIR)/include/ps/gups \
	$(BALONG_TOPDIR)/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/ps/tlps \
	$(BALONG_TOPDIR)/include/ps/nas \
	$(BALONG_TOPDIR)/include/taf \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/med

OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/inc/gu \
	$(BALONG_TOPDIR)/modem/ps/inc/lt \
	$(BALONG_TOPDIR)/modem/ps/comm/inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/DICC/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/LINK/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEM/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEMCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/STL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/NFEXT \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/NFEXT/Interpeak \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/IMSNIC/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/Pstool/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Inc \
	$(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Pscfg \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Comm \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Gtf \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Wtf \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Gas \
    $(BALONG_TOPDIR)/modem/ps/as/gu/inc/Was \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Cipher/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Frmwk/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/comm/Pscfg/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GASM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/was/Inc \
	$(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CF/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/TOOLS \
	$(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/MUX/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/DL/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GMAC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRLC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/PDCP/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/BMC/Inc \
	$(BALONG_TOPDIR)/modem/ps/nas/inc \
	$(BALONG_TOPDIR)/modem/ps/nas/comm/mml/INC \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/lib \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Fsm/INC \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmcomm/inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Nasapi/inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Rabm/inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Inc \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Tc/Inc \
	$(BALONG_TOPDIR)/modem/taf/inc \
	$(BALONG_TOPDIR)/modem/taf/comm/inc \
	$(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/Lib \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Fsm/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Log \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafDrvAgent/Inc

ifeq ($(CFG_RAT_MODE),RAT_GU)
OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ND/IPCOMM/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ND/NDCLIENT/Inc
endif

#***********************************************************#
# source files
#***********************************************************#
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/adler32.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/compress.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/crc32.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/deflate.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/inffast.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/inflate.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/inftrees.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/trees.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/uncompr.c \
    $(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Src/zutil.c	

ifeq ($(INSTANCE_ID), INSTANCE_0)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/LINK/Src/TTFLink.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/LINK/Src/TTFUtil.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEM/Src/TTFMem.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEM/Src/TTFMemRB.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEMCTRL/Src/TTFMemCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/DICC/Src/Dicc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Src/R_ITF_FlowCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Src/Fc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Src/FcCCore.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/IMSNIC/Src/ImsNic.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/Pstool/Src/TtfErrlog.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/NFEXT/IpsMntnCCore.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/NFEXT/Interpeak/InterpeakMntn.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/Pstool/Src/cpu_view_adapter.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/Pstool/Src/Ccpu_view.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/Rrm.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/RrmDebug.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/RrmInit.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/RrmLog.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/RrmCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/RRM/Src/RrmTimer.c        
	 
ifeq ($(CFG_RAT_MODE),RAT_GU)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/STL/Src/LUPQueue.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsDebug.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsDlProc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsEntity.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsIpfCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsMsgProc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsUlProc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsIpFragmentProc.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/CdsSoftFilter.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/FcFlowCtrlMana.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/QosFcChannelFlowCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/QosFcOm.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/QosFcRabStatus.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Src/QosFcStatus.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ND/IPCOMM/Src/IpComm.c \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/ND/NDCLIENT/Src/IpNdClient.c 
endif
endif

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk

