#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=gas_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=gas1_ccore
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

#gas include directories
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
	$(BALONG_TOPDIR)/include/nv/gu/gas \
	$(BALONG_TOPDIR)/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/nv/gu/phy \
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
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Inc \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Inc \
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
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Inc
endif

ifeq ($(CFG_RAT_MODE),RAT_GUL)
OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Inc
endif

#***********************************************************#
# source files
#***********************************************************#
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/fsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/GasMnTn.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/GasErrLog.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/GasComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GASM/Src/GasGasm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GASM/Src/GasGasmfunc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Src/GasGcom.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Src/GasGcomCapability.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Src/GasGcomToGrrParams.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Src/GasGcomToRrParams.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcBuildMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcCmpMsgPri.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcPreProcess.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcFsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcFsmTbl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL1Fsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmBgWSearchG.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcGprsCellReselection.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcGsmCellReselection.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcMsgDistribution.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcPlmnSearch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcPublicCellReselection.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcUtranCellReselection.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmBgGSearchG.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmBgGSearchW.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmBgPlmnListSearch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmGNetScanG.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmGNetScanW.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmWNetScanG.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmSysCfg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmNasSusp.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmPwrOff.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmSlaveModeLowPower.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmReselG2W.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmCCOG2W.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndGphyMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndInternalMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndNasMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndOmMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndWrrMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndWphyMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommCommon.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommGprsNcMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommGprsPulbic.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommGprsReselMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommGsmDedicatedMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommGsmIdleMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommMain.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommFsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommFsmTbl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiMeasInformation.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsi.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiL2FsmInactiveSCellBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiL2FsmInactiveSearchingBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSndInternalMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiFsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiNCellBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiNCellNormal.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiProcessPsi.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiProcessSi.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellBcchGprsPeriod.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellBcchGsmPeriod.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellDedicate.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellInactive.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellNormal.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellPbcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellPacketSiStatus.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiSCellBcchENHReceive.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiNetCtrlMeasParam.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiInactiveBgNCellBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiScellNormalBgNCellBcch.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiCmpMsgPri.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/Src/GasGcomMain.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/Src/GasGcomPublic.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrBuildMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL1Proc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL2Abnormal.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL2Acs.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL2InitAcs.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL2Transf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmL3Transf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrFsmTbl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrImmedAssign.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrMain.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrPktDownlinkAssign.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrPktTimeSlotReCfg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrPktUplinkAssign.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrPreProcess.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrTbfCfg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GRR/Src/GasGrrVar.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrAbnormalRelease.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrAssignCmd.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrBuildMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrChannelModeModify.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrChannelRelease.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrCipherModeCmd.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrCommFunc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrConnection.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrContextHandler.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrFreqRedef.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrFsmTable.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrHandover.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrHandoverToGSM.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrHandoverToUtran.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrMain.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrMmAbort.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrMmChannelRelease.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrMsgValid.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrPhyRadioLinkFailure.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrRadioLinkFailure.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrSap3Est.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrCmpMsgPri.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrPreProcess.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsCommFunc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsFsmActive.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsFsmIdle.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsFsmInactive.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsFsmL2Active.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsFsmTbl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCBS/Src/GasGcbsMain.c

OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/comm/ASN1/Codec/per_main_gas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/COMMON/Src/GasDynBuf.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrAppInfo.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsComm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsSaveRrlpMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsFillRrlpMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsSndMtaMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsSndRrMsg.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsCtx.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsFsmL1Idle.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsFsmL1Location.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsFsmL1AssistDataDelivery.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsFsmTbl.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/LCS/Src/GasLcsMain.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasAsn1Api.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasMsgLstRomDec.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasMsgLstRomDynEnc.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasRRLPV920DecPerRom.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/ASN1/Src/GasRRLPV920DynEncPerRom.c

ifeq ($(CFG_RAT_MODE),RAT_GU)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/CSN1DataTypes.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/csn1lib.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/Downlink_RLC_MAC.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/ed_c.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/ed_c_known_ie.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/ed_c_recog.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/ed_lib.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/L3Downlink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/L3Uplink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/PLENDownlink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/ts04.18_support.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/Uplink_RLC_MAC.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R6/Src/UserDefinedDataTypes.c
endif	

ifeq ($(CFG_RAT_MODE),RAT_GUL)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/CSN1DataTypes.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/csn1lib.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/Downlink_RLC_MAC.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/ed_c.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/ed_c_known_ie.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/ed_c_recog.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/ed_lib.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/L3Downlink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/L3Uplink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/PLENDownlink.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/ts04.18_support.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/Uplink_RLC_MAC.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/Encodix/R9/Src/UserDefinedDataTypes.c

OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/DATA/Src/GasGcomCtxLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmBgGSearchL.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmLteAnrGsm.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcCtxLte.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcBuildMsgLte.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmReselG2L.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmReselL2G.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcLteCellReselComm.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommL2FsmLteConnectMeas.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommL2FsmLteIdleMeas.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiProcessSiR8.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiProcessSiR9.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmRedirG2L.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmRedirG2T.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmRedirL2G.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmCCOG2L.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmCCOL2G.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndLrrcMsg.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcCmpMsgPriLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndOmMsgLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommCommonLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommCtxLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommSndOmMsgLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiCommLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMSI/Src/GasGcomsiCtxLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/Src/GasGcomPublicLte.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmCCOG2T.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmCCOT2G.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmReselG2T.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmReselT2G.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmSlaveModeCtrl.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcSndTrrcMsg.c \
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMM/Src/GasGcommL2FsmTdsMeas.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrL2FsmHandoverT2G.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/RR/Src/GasRrL2FsmHandoverL2G.c\
    $(BALONG_TOPDIR)/modem/ps/as/gu/gas/GCOM/GCOMC/Src/GasGcomcL2FsmUtranMode.c
endif

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk
