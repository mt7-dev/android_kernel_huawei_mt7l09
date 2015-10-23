#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=nas_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=nas1_ccore
endif

#***********************************************************#
# compiler flags
#***********************************************************#
#COMM FLAGS
include $(BALONG_TOPDIR)/modem/ps/build/gu/MODEM_CORE/ps_comm_flags.mk

#USER FLAGS
CC_USER_FLAGS   ?= 
AS_USER_FLAGS   ?=

ifeq ($(CFG_THUMB_COMPILE),YES)
CC_USER_FLAGS += -march=armv6 -mthumb -mthumb-interwork -mlong-calls
else
CC_USER_FLAGS += -falign-arrays
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

#nas include directories
OBC_LOCAL_INC_DIR ?=
OBC_LOCAL_INC_DIR += \
	$(BALONG_TOPDIR)/config/log \
	$(BALONG_TOPDIR)/config/nvim/include/gu \
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
	$(BALONG_TOPDIR)/include/oam/lt/diag \
	$(BALONG_TOPDIR)/include/phy/gphy \
	$(BALONG_TOPDIR)/include/phy/wphy \
	$(BALONG_TOPDIR)/include/ps/gups \
	$(BALONG_TOPDIR)/include/ps/tlps \
	$(BALONG_TOPDIR)/include/ps/nas \
	$(BALONG_TOPDIR)/include/taf \
	$(BALONG_TOPDIR)/include/tools \
	$(BALONG_TOPDIR)/include/med \
	$(BALONG_TOPDIR)/include/nv/gu/codec \
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/nv/gu/gas \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/nv/gu/codec \
	$(BALONG_TOPDIR)/include/nv/tl/lps

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
	$(BALONG_TOPDIR)/modem/ps/comm/gu/zlib/Inc \
	$(BALONG_TOPDIR)/modem/ps/nas/inc \
	$(BALONG_TOPDIR)/modem/ps/nas/comm/mml/INC \
	$(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/INC \
	$(BALONG_TOPDIR)/modem/ps/nas/comm/NASUSIMMAPI/INC \
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
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Xml/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Log \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafDrvAgent/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Inc \
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/SDC/Inc \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Inc
	
#***********************************************************#
# source files
#***********************************************************#
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/lib/NasMsgEncDec.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcAirMsgProc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcAirMsgSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcCommon.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcEntityMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcMmccProc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcMmccSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcMnccProc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcMnccSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcMsgEncDec.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcTimer.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcUserConn.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcProcNvim.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Cc/Src/NasCcCtx.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmAttach.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasComm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasGsm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasSuspend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmCasTimer.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmComm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmDetach.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmRam.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmRau.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmService.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/GmmTimer.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_CellProc1.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_CellProc2.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_CellProc3.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Com.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Global.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Main.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Msg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Rcv.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MM_Snd.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MmAuth.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MmGsmDifMsg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/MmSuspend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmcomm/src/MM_Share.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Nasapi/src/AppInterfaceApi.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Nasapi/src/asnasapi.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Nasapi/src/NasCom.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmAgent.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/smram.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmRcvGmm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmRcvRabm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmRcvTaf.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/SmSend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smccom.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smcsmrapi.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smrcom.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smrtafmsg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/SmsCbMsgProc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smscssmprocess.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smsmain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smspssmprocess.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sms/src/smsram.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsDecode.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsEncode.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsEntityMng.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsRam.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsRcvAir.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsRcvMm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsRcvTaf.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsRcvTimer.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsSndAir.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsSndMm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsSndTimer.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Ss/Src/SsTaskEntry.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Tc/Src/tc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Fsm/SRC/NasFsm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/mml/SRC/NasMmlCtx.c \
    $(BALONG_TOPDIR)/modem/ps/nas/comm/mml/SRC/NasMmlLib.c \
    $(BALONG_TOPDIR)/modem/ps/nas/comm/mml/SRC/NasMmlMsgProc.c \
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlCommFunc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlCtx.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmMainTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlProcNvim.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmSyscfgTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmSwitchOnTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmModeChangeTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmPoweroffTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlMntn.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmModeChange.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmPlmnSelectionTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmPowerOff.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmSwitchOn.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmSyscfg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/UTRANCTRL/SRC/NasUtranCtrlFsmPlmnSelection.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/NASUSIMMAPI/SRC/NasUsimmApi.c\
    $(BALONG_TOPDIR)/modem/ps/nas/comm/NASUSIMMAPI/SRC/NasUsimmApiMntn.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcCtx.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcTimerMgmt.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcPreProcTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysOosTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysOos.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysCellResel.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcComFunc.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmSyscfgTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmSyscfg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysCcoTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysCco.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcProcSuspend.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysHoTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysCellReselTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcProcNvim.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/MmcMmaInterface.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmMain.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndInternalMsg.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPowerOff.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcPreProcAct.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPlmnSelection.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmAnyCellSearch.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndAps.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndCds.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndMma.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndMm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndOm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndGuAs.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPoweroffTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmSwitchOnTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmSwitchOn.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndGmm.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmMainTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPlmnSelectionTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmInterSysHo.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmAnyCellSearchTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcProcUsim.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcPlmnSelectionStrategy.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcProcRegRslt.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcMsgPrioCompare.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmBgPlmnSearch.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmBgPlmnSearchTbl.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPlmnList.c\
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcFsmPlmnListTbl.c

ifeq ($(CFG_RAT_MODE),RAT_GU)
OBC_LOCAL_SRC_FILE += \

endif

ifeq ($(CFG_RAT_MODE),RAT_GUL)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Gmm/Src/NasGmmProcLResult.c \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mm/Src/NasMmProcLResult.c \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Sm/Src/NasSmMultiMode.c \
    $(BALONG_TOPDIR)/modem/ps/nas/gu/src/Mmc/Src/NasMmcSndLmm.c
endif

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk
