#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=ttf_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=ttf1_ccore
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

#ttf include directories
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
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/nv/gu/gas \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/ps/tlps \
	$(BALONG_TOPDIR)/include/ps/nas \
	$(BALONG_TOPDIR)/include/taf \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/include/med

OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/inc/gu \
	$(BALONG_TOPDIR)/modem/ps/inc/lt \
	$(BALONG_TOPDIR)/modem/ps/comm/inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/DICC/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/FLOWCTRL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/LINK/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/MEM/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/STL/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/CDS/Inc \
    $(BALONG_TOPDIR)/modem/ps/comm/comm/Pstool/Inc \
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


#***********************************************************#
# source files
#***********************************************************#
#以下文件全部编译两次
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/TTFStub/CF_Stub.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfGrmMntn.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfGrmComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfGtm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfMacComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfMacDlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfMacUlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfRlcDlCtrl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfRlcDlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfRlcUlCtrl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfRlcUlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GRM/Src/GttfT4Func.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/DL/Src/DL.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/DL/Src/DLData.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/DL/Src/DLPhy.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/DL/Src/DLStateCtrl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GTFCOM/PsGtfHelp.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GTFCOM/GTTFComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/GTFCOM/GTTFQueue.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LL.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLAbm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLAdm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLE.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLGea.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLME.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLMux.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/LL/Src/LLXid.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Src/SNComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Src/SNData.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Src/SNNsapi.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Src/SNSapi.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Gtf/SN/Src/SNXid.c

#以下文件只有modem 0 编译
ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstCstm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstL2R.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstNtM.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstRa0.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstRa1m.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstRingBuf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstRlpData.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstRlpE.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/CstTransEntity.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/FaxBuf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/FaxMgr.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/FaxGfa.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/CST/Src/FaxT31.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/TOOLS/TTFMemChk.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/TOOLS/TTFMsgChk.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/TOOLS/TTFTrace.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/TTFComm/IP/TtfIpComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/BMC/Src/WttfBmcEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/BMC/Src/WttfBmcEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacCUlEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacCDlEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacDUlEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacDDlEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacEEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacIEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacDlEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacHsEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacEhsEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacMntn.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacTrafficMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacAmrSwitch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacUlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacDlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/MAC/Src/WttfMacMsgProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/PDCP/Src/WttfPdcpDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/PDCP/Src/WttfPdcpEntProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/PDCP/Src/WttfPdcpEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcAmUlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcAmUlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcAmDlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcAmDlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcAmQueueMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUlCipher.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcDlCipher.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcCommon.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcDlEntry.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcDlProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcMntn.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUlResetProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUlStatusProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcDlResetProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcDlStatusProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcTmDlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcTmDlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcTmUlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcTmUlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUmUlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUmUlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUmDlDataProc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcUmDlEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/ttf/Wtf/RLC/Src/WttfRlcMsgProc.c
endif

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk
