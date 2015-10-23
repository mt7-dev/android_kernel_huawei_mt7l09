# Core information

GUPSDIR = ../../..
TAFDIR = ../../../../taf

PS_COMM_SRC_FILES        := \
	$(GUPSDIR)/comm/comm/LINK/Src/TTFLink.c\
	$(GUPSDIR)/comm/comm/LINK/Src/TTFUtil.c\
	$(GUPSDIR)/comm/comm/DICC/Src/Dicc.c\
	$(GUPSDIR)/comm/comm/DIPC/Src/PsDipc.c\
	$(GUPSDIR)/comm/comm/MUX/Src/PsMux.c\
	$(GUPSDIR)/comm/comm/MEMCTRL/Src/TTFMemCtrl.c\
	$(GUPSDIR)/comm/comm/FLOWCTRL/Src/ACpuLoad.c\
	$(GUPSDIR)/comm/comm/FLOWCTRL/Src/Fc.c\
	$(GUPSDIR)/comm/comm/FLOWCTRL/Src/FcACore.c\
	$(GUPSDIR)/comm/comm/FLOWCTRL/Src/FcACoreCReset.c\
	$(GUPSDIR)/comm/comm/IMM/Src/IMMmemMntn.c\
	$(GUPSDIR)/comm/comm/IMM/Src/IMMmemRB.c\
	$(GUPSDIR)/comm/comm/IMM/Src/IMMmemZC.c\
	$(GUPSDIR)/comm/comm/PPP/Src/acf.c\
	$(GUPSDIR)/comm/comm/PPP/Src/async.c\
	$(GUPSDIR)/comm/comm/PPP/Src/auth.c\
	$(GUPSDIR)/comm/comm/PPP/Src/chap.c\
	$(GUPSDIR)/comm/comm/PPP/Src/hdlc.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ipcp.c\
	$(GUPSDIR)/comm/comm/PPP/Src/lcp.c\
	$(GUPSDIR)/comm/comm/PPP/Src/link.c\
	$(GUPSDIR)/comm/comm/PPP/Src/pap.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_atcmd.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_fsm.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_init.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_input.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_mbuf.c\
	$(GUPSDIR)/comm/comm/PPP/Src/ppp_public.c\
	$(GUPSDIR)/comm/comm/PPP/Src/pppid.c\
	$(GUPSDIR)/comm/comm/PPP/Src/proto.c\
	$(GUPSDIR)/comm/comm/PPP/Src/throughput.c\
	$(GUPSDIR)/comm/comm/PPP/Src/hdlc_hardware.c\
	$(GUPSDIR)/comm/comm/PPP/Src/hdlc_software.c\
	$(GUPSDIR)/comm/comm/NFEXT/IpsMntn.c\
	$(GUPSDIR)/comm/comm/NFEXT/NetfilterEx.c\
	$(GUPSDIR)/comm/comm/NFEXT/NetfilterExCtrl.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsCtx.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsDownLink.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsInit.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsTimerMgmt.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsUpLink.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsDebug.c\
	$(TAFDIR)/comm/src/acore/ADS/Src/AdsFilter.c\
	$(TAFDIR)/comm/src/acore/APPVCOM/Src/AppVcomDev.c\
	$(TAFDIR)/comm/src/acore/CSD/Src/CsdCtx.c\
	$(TAFDIR)/comm/src/acore/CSD/Src/CsdDebug.c\
	$(TAFDIR)/comm/src/acore/CSD/Src/CsdDownLink.c\
	$(TAFDIR)/comm/src/acore/CSD/Src/CsdUpLink.c\
	$(TAFDIR)/comm/src/acore/CSD/Src/CsdInit.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicCtx.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicEntity.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicInit.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicProcMsg.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicTimerMgmt.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicDebug.c\
	$(TAFDIR)/comm/src/acore/RNIC/Src/RnicDemandDialFileIO.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/Taf_MmiStrParse.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/MnMsgDecode.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/MnMsgEncode.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/MnMsgCbEncDec.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/TafApsApi.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/TafMmaApi.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/MnCommApi.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/hi_list.c\
	$(TAFDIR)/comm/src/mcore/TAF/Lib/Src/TafStdlib.c\
	$(TAFDIR)/comm/src/acore/TAFAGENT/Src/TafAgent.c\
	$(TAFDIR)/comm/src/acore/TAFAGENT/Src/TafAgentCtx.c\
	$(TAFDIR)/comm/src/acore/at/src/AtBasicCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCheckFunc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCmdMsgProc.c\
	$(TAFDIR)/comm/src/acore/at/src/ATCmdProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCmdSimProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCombinecmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCmdImsProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtDataProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtDeviceCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtEventReport.c\
	$(TAFDIR)/comm/src/acore/at/src/AtExtendCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtExtendPrivateCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtInputProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtMntn.c\
	$(TAFDIR)/comm/src/acore/at/src/AtMsgPrint.c\
	$(TAFDIR)/comm/src/acore/at/src/AtOutputProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseBasicCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseCore.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseDCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseDMCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseExtendCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParsePara.c\
	$(TAFDIR)/comm/src/acore/at/src/AtParseSCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtQueryParaCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtSetParaCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtTestParaCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtTimer.c\
	$(TAFDIR)/comm/src/acore/at/src/ATSndMsg.c\
	$(TAFDIR)/comm/src/acore/at/src/AtAbortParaCmd.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCtx.c\
	$(TAFDIR)/comm/src/acore/at/src/AtInit.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCmdPacketProc.c\
	$(TAFDIR)/comm/src/acore/at/src/AtCmdFtmProc.c\
	$(TAFDIR)/comm/src/acore/tafapi/src/MnCallApi.c\
	$(TAFDIR)/comm/src/acore/tafapi/src/MnMsgApi.c\
	$(TAFDIR)/comm/src/acore/tafapi/src/Ssa_App_Remote.c\
	$(TAFDIR)/comm/src/acore/tafapi/src/Taf_Tafm_AppRemote.c\
	$(TAFDIR)/comm/src/acore/tafapi/src/VcApi.c\
	$(TAFDIR)/comm/src/acore/dms/src/dms_acm_at_rx.c\
	$(TAFDIR)/comm/src/acore/dms/src/dms_acm_at_tx.c\
	$(TAFDIR)/comm/src/acore/dms/src/dms_core.c\
	$(TAFDIR)/comm/src/acore/dms/src/dms_debug.c\
	$(TAFDIR)/comm/src/acore/dms/src/dms_ndis.c\
	$(GUPSDIR)/comm/comm/HISOCKET/Src/hisocket.c\


ifeq ($(CFG_RAT_MODE),RAT_GU)
PS_COMM_SRC_FILES        += \
	$(GUPSDIR)/comm/comm/STL/Src/LUPQueue.c\
	$(GUPSDIR)/comm/comm/NDIS/Src/PsNdis.c\
	$(GUPSDIR)/comm/comm/NDIS/Src/Ipv4DhcpServer.c\
	$(GUPSDIR)/comm/comm/ND/IPCOMM/Src/IpComm.c\
	$(GUPSDIR)/comm/comm/ND/NDSERVER/Src/IpNdServer.c\
    
endif

ifeq ($(CFG_TTF_SKB_EXP),NO)
PS_COMM_SRC_FILES        += \
	$(GUPSDIR)/comm/comm/IMM/Src/IMMmem_PS.c\
	
    
endif

	

