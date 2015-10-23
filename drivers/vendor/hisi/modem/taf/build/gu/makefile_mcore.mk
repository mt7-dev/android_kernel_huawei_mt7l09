#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore

ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_MOUDLE_NAME	?=taf_ccore
endif

ifeq ($(INSTANCE_ID) ,INSTANCE_1)
OBC_LOCAL_MOUDLE_NAME	?=taf1_ccore
endif

#***********************************************************#
#thumb compile switch:必须放在compiler FLAGS之前
#***********************************************************#
OBC_THUMB_COMPILE :=ON

#***********************************************************#
# compiler flags
#***********************************************************#
CC_USER_FLAGS   ?= 
AS_USER_FLAGS   ?=


CC_USER_FLAGS += -c  -O2 -t7 -msoft-float -fdollars-in-identifiers \
				    -fno-builtin -fvolatile -fsigned-char -fno-feature-proxy -Wall -fno-zero-initialized-in-bss
					
ifeq ($(CFG_THUMB_COMPILE),YES)
CC_USER_FLAGS += -mlong-calls
else
OBC_THUMB_COMPILE := OFF
endif
					
ifeq ($(OBC_THUMB_COMPILE),ON)
CC_USER_FLAGS += -march=armv6 -mthumb -mthumb-interwork
else
CC_USER_FLAGS += -falign-arrays
endif

AR_USER_FLAGS  +=-r -o

#***********************************************************#
# compiler defines
#***********************************************************#
CC_USER_DEFINES ?=

CC_USER_DEFINES += -DUSP_2_0 -D__LOG_RELEASE__ -DVOS_VXWORKS=3 -DVOS_OS_VER=VOS_VXWORKS -DVOS_HARDWARE_PLATFORM=8 \
				-DVOS_CPU_TYPE=8 -DV2R1_USIM \
				-DVOS_OSA_CPU=OSA_CPU_CCPU -DBALONG_CHIP_V200=2 \
				-DBALONG_CHIP_V300=3 -DBALONG_CHIP_V500=4 -DBALONG_CHIP_VER=BALONG_CHIP_V500 -DDSP_A17_ENABLE \
				-DVERSION_V3R2_C00 -D$(CFG_GU_PRODUCT_VERSION) -D__LDF_FUNCTION__ -DBOARD_$(CFG_BOARD) \
				-DFEATURE_DSP2ARM -DBSP_CORE_MODEM -DMSP_GUNAS_AT_UNITE
				
ifeq ($(INSTANCE_ID) ,INSTANCE_1)
CC_USER_DEFINES	+=-DINSTANCE_1
endif

#***********************************************************#
#thumb compile switch
#***********************************************************#
PS_CFG_THUMB_COMPILE := OFF

#***********************************************************#
# include Directories
#***********************************************************#

#taf include directories
OBC_LOCAL_INC_DIR := \
    $(BALONG_TOPDIR)/config/product/include/$(OBB_PRODUCT_NAME) \
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
	$(BALONG_TOPDIR)/include/oam/comm/cbpa \
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
	$(BALONG_TOPDIR)/include/nv/gu/nas \
	$(BALONG_TOPDIR)/include/nv/gu/gas \
	$(BALONG_TOPDIR)/include/nv/gu/was \
	$(BALONG_TOPDIR)/include/nv/gu/ttf \
	$(BALONG_TOPDIR)/include/nv/gu/oam \
	$(BALONG_TOPDIR)/include/nv/tl/lps \
    $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM_HISI_BALONG)

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
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Inc\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafSpm/Inc\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Inc

ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_INC_DIR += \
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Inc

endif
endif

####################################################################################################
#Source files
####################################################################################################
OBC_LOCAL_SRC_FILE := \
    $(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Src/Taf_MmiStrParse.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Src/MnMsgDecode.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Src/MnMsgEncode.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Src/MnMsgCbEncDec.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/mcore/TAF/Lib/Src/hi_list.c\
    ${BALONG_TOPDIR}/modem/taf/comm/src/mcore/TAF/Lib/Src/TafMmiEncode.c\
    ${BALONG_TOPDIR}/modem/taf/comm/src/mcore/TAF/Lib/Src/TafApsApi.c\
    ${BALONG_TOPDIR}/modem/taf/comm/src/mcore/TAF/Lib/Src/TafStdlib.c\
    ${BALONG_TOPDIR}/modem/taf/comm/src/mcore/TAF/Lib/Src/TafMmaApi.c\
    ${BALONG_TOPDIR}/modem/taf/comm/src/mcore/TAF/Lib/Src/MnCommApi.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaCtx.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaCtxPkt.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaInit.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaMain.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaMeasReselMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaMntn.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaParseCmd.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaPktMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaRegMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/CMMCA/Src/CmmcaTimerMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Fsm/Src/TafFsm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Lib/Xml/Src/XmlComm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafDrvAgent/Src/TafDrvAgentMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafDrvAgent/Src/TafDrvAgentMsgProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCall.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallBcProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallFacilityDecode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallFacilityEncode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallMnccProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallReqProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallSendCc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallProcNvim.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallCstProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallTimer.c\
	$(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Call/Src/MnCallImsaProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/MnApsMultiMode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsAssistant.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealApiFlowMsgPara.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealApiFlowMsgSmAct.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealApiParaMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealRabmMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealSmMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/Taf_ApsDealSndcpMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsComFunc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsDecode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsDsFlowStats.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsEncode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMainTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsActivating.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsActivatingTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsDeactivating.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsDeactivatingTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsModifying.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmMsModifyingTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmNwActivating.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsFsmNwActivatingTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsGetPdpIdList.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsPreproc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsPreProcTbl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsProcIpFilter.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsProcNvim.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsProcTft.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndAt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndAds.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndInternalMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndNd.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndRabm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndSm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndSndcp.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsTimerMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsMntn.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafMd5.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgComm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgReport.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgReqProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgSendSms.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgSmCommProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgSmsProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgTimerProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgUsimProc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgProcNvim.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Msg/Src/MnMsgSendSpm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Src/CardLockCipher.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Src/MmaAppLocal.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaSndMmc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Phone/Src/Taf_Status.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaNetworkNameTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaCtx.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaFsmMain.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaFsmMainTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaFsmPhoneMode.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaFsmPhoneModeTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaMain.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaMntn.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaMsgPrioCompare.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaPreProcAct.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaPreProcTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaProcNvim.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaSndImsa.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaSndInternalMsg.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaTimerMgmt.c\
	${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaSndTaf.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/Mn/Phone/Src/TafMmaSndApp.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Src/Ssa_App.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Src/Ssa_Common.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Src/Ssa_Decode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Src/Ssa_Encode.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Sups/Src/Ssa_Ps.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Src/MnComm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Src/MnTask.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Src/Taf_Tafm_AppLocal.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Task/Src/Taf_Tafm_Db.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Src/VcMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Src/VcSendMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Src/VcCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/VC/Src/VcProcNvim.c \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaComFunc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcAs.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcAt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcEtwsPrimNtf.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcNvim.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaMntn.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcUsim.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcMmc.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcMma.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/CBA/Src/TafCbaProcTimer.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/SDC/Src/TafSdcCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/SDC/Src/TafSdcLib.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaAgps.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaAgpsMsgConvertXml.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaAgpsXmlFillMsg.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaComm.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaCtx.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaMain.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaModemControl.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaPhy.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaMntn.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaTimerMgmt.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaSecure.c\
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/TafMta/Src/TafMtaUnSolicitedReport.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmSndInternalMsg.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmFsmMainTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmFsmServiceCtrlTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmTimerMgmt.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmFsmMain.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmMntn.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmMain.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmCtx.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmFsmServiceCtrl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmProcNvim.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmSndUsim.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmServiceDomainSelProc.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmSndImsa.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmComFunc.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmPreProcAct.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmPreProcTbl.c\
    ${BALONG_TOPDIR}/modem/taf/gu/src/ccore/src/TafSpm/Src/TafSpmRedial.c    

ifeq ($(CFG_RAT_MODE),RAT_GU)
OBC_LOCAL_SRC_FILE += \

endif
   
ifeq ($(CFG_RAT_MODE),RAT_GUL)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndEsm.c \
    $(BALONG_TOPDIR)/modem/taf/gu/src/ccore/src/Mn/Data/Src/TafApsSndL4a.c 
endif

ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
ifeq ($(INSTANCE_ID) ,INSTANCE_0)
OBC_LOCAL_SRC_FILE += \
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcComm.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcCtx.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcDebug.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcInit.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcIntrusion.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcMain.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcPsTransfer.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcCalcFreq.c\
    $(BALONG_TOPDIR)/modem/taf/comm/src/ccore/MTC/Src/MtcRfLcdIntrusion.c
endif
endif

#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk

