

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "TafFsm.h"
#include "TafMmaPreProcAct.h"
#include "TafMmaFsmMainTbl.h"
#include "MmaMmcInterface.h"
#include "Taf_Tafm_Remote.h"
#include "MmaAppLocal.h"
#include "Taf_Status.h"
#include "TafMmaSndInternalMsg.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "ImsaMmaInterface.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_PREPROC_TBL_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 预处理状态机 */
TAF_FSM_DESC_STRU                       g_stTafMmaPreFsmDesc;


/*新增状态动作处理表 */

/* 不进状态机处理的消息 动作表 */
TAF_ACT_STRU        g_astTafMmaPreProcessActTbl[]   =
{
#ifdef __PS_WIN32_RECUR__   
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      EVT_NAS_MMA_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY,
                      NAS_MMA_RestoreContextData),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      EVT_NAS_MMA_FIXED_PART_CONTEXT,
                      NAS_MMA_RestoreFixedContextData),

#endif

    /* MMA的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_INTER_USIM_STATUS_CHANGE_IND,
                      TAF_MMA_RcvMmaInterUsimStatusChangeInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMA,
                      MMA_MMA_SIM_LOCK_STATUS_CHANGE_IND,
                      TAF_MMA_RcvMmaSimLockStatusChangeInd_PreProc ),

    /* OM的消息处理 */

    TAF_ACT_TBL_ITEM( WUEPS_PID_OM,
                      MMA_EVT_OM_SDT_CONNECTED_IND,
                      TAF_MMA_RcvOamConnectStatusInd),

    TAF_ACT_TBL_ITEM( WUEPS_PID_OM,
                      OAM_MMA_TRACE_CONFIG_REQ,
                      TAF_MMA_RcvOamTraceCfgReq),   
    
    TAF_ACT_TBL_ITEM( WUEPS_PID_OM,
                      OAM_MMA_PHONE_LOADDEFAULT_REQ,
                      MMA_LoadDefaultVaule),

    TAF_ACT_TBL_ITEM( WUEPS_PID_OM,
                      OAM_MMA_PHONE_MODE_SET_REQ,
                      TAF_MMA_RcvOmPhoneModeSetReq_PreProc),

    /* USIM的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_GET_STATUS_IND,
                      TAF_MMA_RcvPihUsimStatusInd_PreProc),

    TAF_ACT_TBL_ITEM( MAPS_PIH_PID,                      
                      PS_USIM_GET_STATUS_IND,
                      TAF_MMA_RcvPihUsimStatusInd_PreProc),
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_RESTRICTED_ACCESS_CNF,
                      MMA_UsimRestrictedAccessCnfMsgProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_HOT_IN_OUT_USIM_STATUS_IND,
                      MMA_ProcHotInOutUsimStatusInd),

    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_EF_MAX_RECORD_NUM_CNF,
                      MMA_USIMMaxRecordNumCnfProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_PIN_OPERATE_RSP,
                      MMA_UsimPINOperateMsgProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_SET_FILE_RSP,
                      MMA_UsimSetFileCnfMsgProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_USIM,                      
                      PS_USIM_GET_FILE_RSP,
                      MMA_UsimGetFileRspMsgProc),

    TAF_ACT_TBL_ITEM( MAPS_PIH_PID,                      
                      PS_USIM_REFRESH_IND,
                      TAF_MMA_RcvPihUsimRefreshIndMsgProc),                     

    /* MMC的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_RSSI_IND,
                      MMA_PhoneRssiInd),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_MM_INFO_IND,
                      Sta_MmInfo), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_PLMN_SELECTION_RLST_IND,
                      MN_MMA_ProcPlmnSelectionRlstInd),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_PLMN_SELE_START_IND,
                      TAF_MMA_ProcPlmnSelectStartInd),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_EOPLMN_SET_CNF,
                      TAF_MMA_RcvMmcEOPlmnSetCnf),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_UMTS_CIPHER_INFO_IND,
                      MMA_SaveWCipherInfo),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_GPRS_CIPHER_INFO_IND,
                      MMA_SaveGCipherInfo),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_AC_INFO_CHANGE_IND,
                      TAF_MMA_RcvMmcAcInfoChangeInd), 
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_SERVICE_STATUS_IND,
                      Sta_ServiceStatusInd),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_SYS_INFO_IND,
                      TAF_MMA_RcvMmcSysInfoInd), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_CAMP_ON_IND,
                      TAF_MMA_RcvMmcCampOnInd_PreProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_DATATRAN_ATTRI_IND,
                      Sta_DataTranAttri),
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_COVERAGE_AREA_IND,
                      Sta_CoverageAreaInd),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,                      
                      ID_MMC_MMA_REG_RESULT_IND,
                      TAF_MMA_RcvMmcRegResultInd_PreProc), 
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_NETWORK_CAPABILITY_INFO_IND,
                      TAF_MMA_RcvMmcNetworkCapabilityInfoInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_NET_SCAN_CNF,
                      TAF_MMA_RcvMmcNetScanCnf_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_ABORT_NET_SCAN_CNF,
                      TAF_MMA_RcvMmcAbortNetScanCnf_PreProc ),


    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_EPLMN_INFO_IND,
                      TAF_MMA_RcvMmcEplmnInfoInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_USIM_AUTH_FAIL_IND,
                      TAF_MMA_RcvMmcUsimAuthFailInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_CS_SERVICE_CONN_STATUS_IND,
                      TAF_MMA_RcvMmcCsServiceConnStatusInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_SRV_REJ_IND,
                      TAF_MMA_RcvMmcServRejInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_DETACH_IND,
                      TAF_MMA_RcvMmcDetachInd_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_ACQ_CNF,
                      TAF_MMA_RcvMmcAcqCnf_PreProc ),

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_ACQ_IND,
                      TAF_MMA_RcvMmcAcqInd_PreProc ),
#endif

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_REG_CNF,
                      TAF_MMA_RcvMmcRegCnf_PreProc ),

    TAF_ACT_TBL_ITEM( WUEPS_PID_MMC,
                      ID_MMC_MMA_POWER_SAVE_CNF,
                      TAF_MMA_RcvMmcPowerSaveCnf_PreProc ),

    /* AT的消息处理 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_USIM_RESTRICTED_ACCESS,
                      MMA_UsimRestrictedAccessCommandProc), 
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_PARA_READ,
                      TAF_MMA_RcvAtParaReadReq_PreProc),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_SYSCFG_QUERY_REQ,
                      TAF_MMA_RcvAtSyscfgHandle_PreProc),


    /* 收到AT的cfun查询请求 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,
                      TAF_MSG_MMA_FUN_QUERY,
                      TAF_MMA_RcvPhoneModeQuery_PreProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_LOADDEFAULT,
                      MMA_DbProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SIMINSERT_IND,
                      MMA_DbProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_QUICKSTART,
                      MMA_DbProc),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_FPLMN_HANDLE,
                      MMA_DbProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_HANDLE_PREF_PLMN_INFO,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_USIM_INFO,
                      MMA_DbProc), 
                          
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_CPNN_INFO,
                      MMA_DbProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_CIPHER_INFO,
                      MMA_DbProc), 
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_LOCATION_INFO,
                      MMA_DbProc),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_CRPN_HANDLE,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MM_TEST_HANDLE,
                      MMA_DbProc), 
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_AC_INFO_QUERY_REQ,
                      MMA_DbProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_CERSSI_INFO_QUERY_REQ,
                      MMA_DbProc),   
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_COPN_INFO_QUERY_REQ,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_EOPLMN_SET_REQ,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_EOPLMN_QUERY_REQ,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_PARA_QUERY,
                      MMA_PhoneProc),


    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_NET_SCAN_REQ,
                      MMA_DbProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_ABORT_NET_SCAN_REQ,
                      MMA_DbProc),


    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_GET_CURRENT_ATTACH_STATUS,
                      MMA_GetDomainAttachState),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_PREF_PLMN,
                      MMA_SetPrefPlmn),  
                     
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_TEST_PREF_PLMN,
                      MMA_TestPrefPlmn),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_TEST_SYSCFG,
                      MMA_TestSyscfg),  
                             
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_PREF_PLMN_TYPE,
                      MMA_SetPrefPlmnType),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_MT_POWER_DOWN,
                      MMA_SetMtPowerDown),   

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_INFO_REPORT,
                      MMA_PhoneProc), 

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_OP_PIN_REQ,
                      MMA_PhoneProc),  
                      
    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_AUTOATTACH,
                      MMA_DbProc),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_ME_PERSONAL_REQ,
                      MMA_PhoneProc),  

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_SET_PIN,
                      MMA_SetCPin), 

#if (FEATURE_ON == FEATURE_IMS)
    TAF_ACT_TBL_ITEM( PS_PID_IMSA,                      
                      ID_IMSA_MMA_IMS_VOICE_CAP_NOTIFY,
                      TAF_MMA_RcvImsaImsVoiceCapNtf_PreProc), 

    TAF_ACT_TBL_ITEM( VOS_PID_TIMER,                      
                      TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY,
                      TAF_MMA_RcvTiWaitImsaImsVoiceCapNtfExpired_PreProc), 

#endif

    /* 定时器消息 */
    TAF_ACT_TBL_ITEM( VOS_PID_TIMER,                      
                      TAF_MMA,
                      MMA_TimeExpired),  
    
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    TAF_ACT_TBL_ITEM( UEPS_PID_MTC,
                      ID_MTC_MMA_OTHER_MODEM_INFO_NOTIFY,
                      TAF_MMA_RcvMtcOtherModemInfoNotify_PreProc),
    TAF_ACT_TBL_ITEM( UEPS_PID_MTC,
                      ID_MTC_MMA_NCELL_INFO_IND,
                      TAF_MMA_RcvMtcNcellInfoInd_PreProc),

    TAF_ACT_TBL_ITEM( UEPS_PID_MTC,
                      ID_MTC_MMA_PS_TRANSFER_IND,
                      TAF_MMA_RcvMtcPsTransferInd_PreProc),


#endif

    TAF_ACT_TBL_ITEM( WUEPS_PID_AT,                      
                      TAF_MSG_MMA_CPOL_INFO_QUERY_REQ,
                      MMA_DbProc),  

/* TAF的消息处理 */


    /* 收到模式设置请求 */
    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_PHONE_MODE_SET_REQ,
                      TAF_MMA_ProcTafPhoneModeSetReq_PreProc),



    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_ACQ_BEST_NETWORK_REQ,
                      TAF_MMA_RcvMmaAcqReq_PreProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_REG_REQ,
                      TAF_MMA_RcvMmaRegReq_PreProc),

    TAF_ACT_TBL_ITEM( WUEPS_PID_TAF,
                      ID_TAF_MMA_POWER_SAVE_REQ,
                      TAF_MMA_RcvMmaPowerSaveReq_PreProc),

    TAF_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_TAF_MMA_WAIT_MMC_ACQ_CNF,
                      TAF_MMA_RcvTiWaitMmcAcqCnfExpired_PreProc),

    TAF_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_TAF_MMA_WAIT_MMC_REG_CNF,
                      TAF_MMA_RcvTiWaitMmcRegCnfExpired_PreProc),

    TAF_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF,
                      TAF_MMA_RcvTiWaitMmcPowerSaveExpired_PreProc),

};

/* 不进状态机处理的消息 状态表 */
TAF_STA_STRU        g_astTafMmaPreProcessFsmTbl[]   =
{
    TAF_STA_TBL_ITEM( TAF_MMA_MAIN_STA_PREPROC,
                      g_astTafMmaPreProcessActTbl )
};
VOS_UINT32 TAF_MMA_GetPreProcessStaTblSize( VOS_VOID  )
{
    return (sizeof(g_astTafMmaPreProcessFsmTbl)/sizeof(TAF_STA_STRU));
}

/*****************************************************************************
 函 数 名  : TAF_MMA_GetPreFsmDescAddr
 功能描述  : 获取预处理状态机的描述表
 输入参数  : 无
 输出参数  : 无
 返 回 值  : TAF_FSM_DESC_STRU:指向预处理状态机的描述表
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月9日
    作    者   : zhoujun 40661
    修改内容   : 新生成函数

*****************************************************************************/
TAF_FSM_DESC_STRU * TAF_MMA_GetPreFsmDescAddr(VOS_VOID)
{
    return (&g_stTafMmaPreFsmDesc);
}




/*****************************************************************************
  3 函数实现
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

