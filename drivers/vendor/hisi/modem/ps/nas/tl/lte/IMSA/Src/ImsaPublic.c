/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaPublic.c
  Description     : 该C文件实现公共的函数，如NV读写，USIM等
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaPublic.h"
#include "ImsaEntity.h"
#include "LNvCommon.h"
#include "LPsNvInterface.h"
#include "UsimPsInterface.h"
#include "ImsaNvInterface.h"
#include "SysNvId.h"
#include "NVIM_Interface.h"
#include "ImsaImsInterface.h"
#include "ImsaImsAdaption.h"
#include "NasCommPrint.h"
#include "VcImsaInterface.h"
#include "ImsaCdsInterface.h"
#include "ImsaImsInterface.h"
#include "MsgImsaInterface.h"
#include "ImsaRegManagement.h"
#include "ScInterface.h"
#include "CallImsaInterface.h"
#include "LNasNvInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID      PS_FILE_ID_IMSAPUBLIC_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
/*IMSA主动发送给IMS消息的OPID*/
static VOS_UINT32 gulImsaImsOpId = 0;

/*IMS主动发送给IMSA，需要IMSA回复消息的OPID*/
static VOS_UINT32 gulImsaRcvImsOpId = 0;

extern VOS_UINT32  IMSA_EncodeIsimFile(const IMSA_ISIM_DATA_STRU *pstIsimData);
extern VOS_UINT32  IMSA_DecodeIsimDomain(const  IMSA_ISIM_DATA_STRU *pstIsimData);
extern VOS_UINT32  IMSA_DecodeIsimImpi(const  IMSA_ISIM_DATA_STRU *pstIsimData);
extern VOS_UINT32  IMSA_DecodeIsimImpu(const  IMSA_ISIM_DATA_STRU *pstIsimData);
extern VOS_VOID    IMSA_ReadImsaNvImsRatSupportConfig( VOS_VOID);
extern VOS_VOID    IMSA_ReadImsaNvImsaConfig( VOS_VOID);
extern VOS_VOID    IMSA_ReadImsaNvImsCapability( VOS_VOID);
extern VOS_VOID    IMSA_ReadImsaNvSipPortConfig( VOS_VOID);
extern VOS_VOID     IMSA_ReadImei( VOS_VOID );
extern VOS_VOID     IMSA_ReadNvVoiceDomain(VOS_VOID );
extern VOS_VOID     IMSA_ReadPcscfDiscoveryPolicy(VOS_VOID);

extern VOS_VOID IMSA_ReadNvIpv6FallBackExtCause(VOS_VOID);

extern VOS_VOID  IMSA_ReadImsaNvImsConfig( VOS_VOID);
extern VOS_VOID     IMSA_SndD2AuthSyncFailure(const USIMM_IMS_AUTH_CNF_STRU  *pstIMSCnf);
extern VOS_VOID     IMSA_SndD2AuthNetworkFailure(VOS_VOID);
extern VOS_VOID IMSA_SndD2AuthSuccRsp(const USIMM_IMS_AUTH_CNF_STRU  *pstIMSCnf);
extern VOS_UINT32  IMSA_BcdToAsciiCode
(
    VOS_UINT8                           ucBcdCode,
    VOS_CHAR                            *pcAsciiCode
);
extern VOS_UINT32  IMSA_AsciiToBcdCode
(
    VOS_CHAR                            cAsciiCode,
    VOS_UINT8                           *pucBcdCode
);
extern VOS_INT32   IMSA_PrintImsaIntraMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    IMSA_INTRA_MSG_ID_ENUM_UINT32       enMsgId
);
extern VOS_INT32   IMSA_PrintUsimMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
);
extern VOS_INT32   IMSA_PrintVcMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
);
extern VOS_INT32   IMSA_PrintCdsMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
);
extern VOS_INT32  IMSA_PrintImsaTimer
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulTimerType,
    VOS_UINT32                          ulTimerPara
);
extern VOS_INT32   IMSA_PrintMmaMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
);
extern VOS_INT32   IMSA_PrintApsMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const TAF_PS_EVT_STRU              *pstApsEvt
);
extern VOS_INT32  IMSA_PrintSpmMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintAtMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintMsgMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintCallMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintImsMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintRcvTafMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IMSA_PrintSendTafMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
);
extern IMSA_IMS_NW_ACCESS_TYPE_ENUM_UINT8 IMSA_ConverterAccessType2Ims
(
    MMA_IMSA_ACCESS_TYPE_ENUM_UINT8     enAccessType
);
extern VOS_VOID IMSA_ConverterCgiParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_ConverterImeiParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_ConverterRetyrTimeParam2Ims
(
    VOS_UINT32                          ulRetryTimerLen,
    VOS_UINT32                          ulPeriodRergisterTimerLen,
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_ConverterUeCapParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_ConverterNetCapParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_ConverterImsiParam2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
);

IMSA_ISIM_ACT_STRU g_astIsimDataMap[] =
{
    {IMSA_ISIM_FILE_ID_IMPI,    IMSA_DecodeIsimImpi,     IMSA_EncodeIsimFile},
    {IMSA_ISIM_FILE_ID_DOMAIN,  IMSA_DecodeIsimDomain,     IMSA_EncodeIsimFile},
    {IMSA_ISIM_FILE_ID_IMPU,    IMSA_DecodeIsimImpu,     IMSA_EncodeIsimFile}

};

VOS_UINT32 g_ulIsimDataMapNum
            = sizeof(g_astIsimDataMap)/sizeof(IMSA_ISIM_ACT_STRU);


VOS_CHAR                                g_acImsaPrintBuf[IMSA_PRINT_BUFF_LEN] = "";

/* IMSA内部消息打印数组 */
NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaIntraMsgIdArray[] =
{
    {   ID_IMSA_REG_REG_REQ                   ,
            "MSG:  ID_IMSA_REG_REG_REQ                               "},
    {   ID_IMSA_REG_DEREG_REQ                   ,
            "MSG:  ID_IMSA_REG_DEREG_REQ                             "},
    {   ID_IMSA_REG_REG_IND                    ,
            "MSG:  ID_IMSA_REG_REG_IND                               "},
    {   ID_IMSA_REG_DEREG_IND                    ,
            "MSG:  ID_IMSA_REG_DEREG_IND                             "},
    {   ID_IMSA_CONN_SETUP_REQ                    ,
            "MSG:  ID_IMSA_CONN_SETUP_REQ                            "},
    {   ID_IMSA_CONN_REL_REQ                ,
            "MSG:  ID_IMSA_CONN_REL_REQ                              "},
    {   ID_IMSA_CONN_SETUP_IND           ,
            "MSG:  ID_IMSA_CONN_SETUP_IND                            "},
    {   ID_IMSA_CONN_REL_IND             ,
            "MSG:  ID_IMSA_CONN_REL_IND                              "},
    {   ID_IMSA_CONN_MODIFY_IND             ,
            "MSG:  ID_IMSA_CONN_MODIFY_IND                           "},
    {   ID_IMSA_NRM_CALL_SRV_STATUS_IND                       ,
            "MSG:  ID_IMSA_NRM_CALL_SRV_STATUS_IND                   "},
    {   ID_IMSA_EMC_CALL_SRV_STATUS_IND                        ,
            "MSG:  ID_IMSA_EMC_CALL_SRV_STATUS_IND                   "},
    {   ID_IMSA_CALL_RESULT_ACTION_IND                        ,
            "MSG:  ID_IMSA_CALL_RESULT_ACTION_IND                    "}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaUsimMsgIdArray[] =
{
    {   PS_USIM_GET_FILE_RSP                   ,
            "MSG:  PS_USIM_GET_FILE_RSP                              "},
    {   PS_USIM_AUTHENTICATION_CNF                   ,
            "MSG:  PS_USIM_AUTHENTICATION_CNF                        "},
    {   PS_USIM_GET_STATUS_IND                    ,
            "MSG:  PS_USIM_GET_STATUS_IND                            "},
    {   PS_USIM_REFRESH_CNF                    ,
            "MSG:  PS_USIM_REFRESH_CNF                               "}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaVcMsgIdArray[] =
{
    {   ID_IMSA_VC_START_HIFI_NTF                   ,
            "MSG:  ID_IMSA_VC_START_HIFI_NTF                         "},
    {   ID_IMSA_VC_STOP_HIFI_NTF                   ,
            "MSG:  ID_IMSA_VC_STOP_HIFI_NTF                          "},
    {   ID_IMSA_VC_HIFI_PARA_CHANGED_NTF                   ,
            "MSG:  ID_IMSA_VC_HIFI_PARA_CHANGED_NTF                  "},
    {   ID_VC_IMSA_HIFI_EXCEPTION_NTF                   ,
            "MSG:  ID_VC_IMSA_HIFI_EXCEPTION_NTF                     "}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaCdsMsgIdArray[] =
{
    {   ID_IMSA_CDS_SET_IMS_BEARER_REQ                   ,
            "MSG:  ID_IMSA_CDS_SET_IMS_BEARER_REQ                    "},
    {   ID_IMSA_CDS_SET_IMS_BEARER_CNF                   ,
            "MSG:  ID_IMSA_CDS_SET_IMS_BEARER_CNF                    "}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaTimerArray[] =
{
    {   TI_IMSA_START_OR_STOP,
            "STATE TI:  TI_IMSA_START_OR_STOP                       "},
    {   TI_IMSA_PERIOD_TRY_IMS_SRV     ,
            "STATE TI:  TI_IMSA_PERIOD_TRY_IMS_SRV                  "},
    {   TI_IMSA_PERIOD_TRY_IMS_EMC_SRV     ,
            "STATE TI:  TI_IMSA_PERIOD_TRY_IMS_EMC_SRV                  "},
    {   TI_IMSA_SIP_SIGAL_PDP_ORIG               ,
            "STATE TI:  TI_IMSA_SIP_SIGAL_PDP_ORIG                  "},
    {   TI_IMSA_SIP_SIGAL_PDP_END               ,
            "STATE TI:  TI_IMSA_SIP_SIGAL_PDP_END                   "},
    {   TI_IMSA_WAIT_IPV6_INFO             ,
            "STATE TI:  TI_IMSA_WAIT_IPV6_INFO                      "},
    {   TI_IMSA_REG_PROTECT               ,
            "STATE TI:  TI_IMSA_REG_PROTECT                         "},
    {   TI_IMSA_REG_RETRY               ,
            "STATE TI:  TI_IMSA_REG_RETRY                           "},
    {   TI_IMSA_CALL_PROTECT,
            "STATE TI:  TI_IMSA_CALL_PROTECT                        "},
    {   TI_IMSA_CALL_RES_READY,
            "STATE TI:  TI_IMSA_CALL_RES_READY                      "},
    {   TI_IMSA_SMS_TR1M,
            "STATE TI:  TI_IMSA_SMS_TR1M                            "} ,
    {   TI_IMSA_SMS_TR2M,
            "STATE TI:  TI_IMSA_SMS_TR2M                            "},
    {   TI_IMSA_BACK_OFF_TX,
            "STATE TI:  TI_IMSA_BACK_OFF_TX                            "},
    {   TI_IMSA_BACK_OFF_TY,
            "STATE TI:  TI_IMSA_BACK_OFF_TY                            "},
    {   TI_IMSA_USSD_WAIT_NET_RSP,
            "STATE TI:  TI_IMSA_USSD_WAIT_NET_RSP                    "},
    {   TI_IMSA_USSD_WAIT_APP_RSP,
            "STATE TI:  TI_IMSA_USSD_WAIT_APP_RSP                    "},

};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaMmaMsgIdArray[] =
{
    {   ID_MMA_IMSA_START_REQ,
            "MSG:  ID_MMA_IMSA_START_REQ                             "},
    {   ID_MMA_IMSA_STOP_REQ                   ,
            "MSG:  ID_MMA_IMSA_STOP_REQ                              "},
    {   ID_MMA_IMSA_DEREG_REQ                    ,
            "MSG:  ID_MMA_IMSA_DEREG_REQ                             "},
    {   ID_MMA_IMSA_SERVICE_CHANGE_IND                    ,
            "MSG:  ID_MMA_IMSA_SERVICE_CHANGE_IND                    "},
    {   ID_MMA_IMSA_CAMP_INFO_CHANGE_IND                    ,
            "MSG:  ID_MMA_IMSA_CAMP_INFO_CHANGE_IND                  "},
    {   ID_IMSA_MMA_START_CNF                    ,
            "MSG:  ID_IMSA_MMA_START_CNF                             "},
    {   ID_IMSA_MMA_STOP_CNF                    ,
            "MSG:  ID_IMSA_MMA_STOP_CNF                              "},
    {   ID_IMSA_MMA_DEREG_CNF                    ,
            "MSG:  ID_IMSA_MMA_DEREG_CNF                             "},
    {   ID_IMSA_MMA_IMS_VOICE_CAP_NOTIFY                    ,
            "MSG:  ID_IMSA_MMA_IMS_VOICE_CAP_NOTIFY                  "}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astImsaApsEvtIdArray[] =
{
    {   ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF               "},
    {   ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ                   ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ               "},
    {   ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_IND                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_IND               "},
    {   ID_EVT_TAF_PS_CALL_PDP_MODIFY_IND                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_MODIFY_IND                 "},
    {   ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF             "},
    {   ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_IND                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_IND             "},
    {   ID_EVT_TAF_PS_CALL_PDP_IPV6_INFO_IND                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_PDP_IPV6_INFO_IND              "},
    {   ID_EVT_TAF_PS_CALL_ORIG_CNF                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_ORIG_CNF                       "},
    {   ID_EVT_TAF_PS_CALL_END_CNF                    ,
            "MSG:  ID_EVT_TAF_PS_CALL_END_CNF                        "},
    {   ID_EVT_TAF_PS_SRVCC_CANCEL_NOTIFY_IND                    ,
            "MSG:  ID_EVT_TAF_PS_SRVCC_CANCEL_NOTIFY_IND             "}
};

NAS_COMM_PRINT_LIST_STRU g_astImsaSpmMsgIdArray[] =
{
    {   ID_SPM_IMSA_CALL_ORIG_REQ               ,
            "MSG:  ID_SPM_IMSA_CALL_ORIG_REQ                         ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_CALL_SUPS_CMD_REQ     ,
            "MSG:  ID_SPM_IMSA_CALL_SUPS_CMD_REQ                     ",
        IMSA_PrintSpmImsaCallSupsCmdReq},
    {   ID_SPM_IMSA_CALL_GET_CALL_INFO_REQ               ,
            "MSG:  ID_SPM_IMSA_CALL_GET_CALL_INFO_REQ                ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_CALL_START_DTMF_REQ               ,
            "MSG:  ID_SPM_IMSA_CALL_START_DTMF_REQ                   ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_CALL_STOP_DTMF_REQ             ,
            "MSG:  ID_SPM_IMSA_CALL_STOP_DTMF_REQ                    ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_CALL_GET_CUDR_REQ               ,
            "MSG:  ID_SPM_IMSA_CALL_GET_CUDR_REQ                     ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_CALL_GET_CLPR_REQ               ,
            "MSG:  ID_SPM_IMSA_CALL_GET_CLPR_REQ                     ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_REGISTER_SS_REQ     ,
            "MSG:  ID_SPM_IMSA_REGISTER_SS_REQ                       ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_ERASE_SS_REQ               ,
            "MSG:  ID_SPM_IMSA_ERASE_SS_REQ                          ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_ACTIVATE_SS_REQ               ,
            "MSG:  ID_SPM_IMSA_ACTIVATE_SS_REQ                       ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_DEACTIVATE_SS_REQ             ,
            "MSG:  ID_SPM_IMSA_DEACTIVATE_SS_REQ                     ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_INTERROGATE_SS_REQ               ,
            "MSG:  ID_SPM_IMSA_INTERROGATE_SS_REQ                    ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_PROCESS_USSD_REQ     ,
            "MSG:  ID_SPM_IMSA_PROCESS_USSD_REQ                      ",
        VOS_NULL_PTR},
    {   ID_SPM_IMSA_RELEASE_REQ               ,
            "MSG:  ID_SPM_IMSA_RELEASE_REQ                           ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_MSG               ,
            "MSG:  ID_IMSA_SPM_CALL_MSG                              ",
        IMSA_PrintImsaSpmCallMsg},
    {   ID_IMSA_SPM_CALL_GET_CALL_INFO_CNF             ,
            "MSG:  ID_IMSA_SPM_CALL_GET_CALL_INFO_CNF                ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CHANNEL_INFO_IND               ,
            "MSG:  ID_IMSA_SPM_CHANNEL_INFO_IND                      ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_GET_CLPR_CNF               ,
            "MSG:  ID_IMSA_SPM_CALL_GET_CLPR_CNF                     ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_START_DTMF_CNF               ,
            "MSG:  ID_IMSA_SPM_CALL_START_DTMF_CNF                   ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_START_DTMF_RSLT_IND             ,
            "MSG:  ID_IMSA_SPM_CALL_START_DTMF_RSLT_IND              ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_STOP_DTMF_CNF               ,
            "MSG:  ID_IMSA_SPM_CALL_STOP_DTMF_CNF                    ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_STOP_DTMF_RSLT_IND               ,
            "MSG:  ID_IMSA_SPM_CALL_STOP_DTMF_RSLT_IND               ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_SS_MSG,
            "MSG:  ID_IMSA_SPM_SS_MSG                                ",
        VOS_NULL_PTR},
    /* xiongxianghui00253310 add for conference 20140210 begin */
    {   ID_SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ,
            "MSG:  ID_SPM_IMSA_CALL_INVITE_NEW_PTPT_REQ              ",
        VOS_NULL_PTR},
    {   ID_IMSA_SPM_CALL_INVITE_NEW_PTPT_CNF,
            "MSG:  ID_IMSA_SPM_CALL_INVITE_NEW_PTPT_CNF              ",
        VOS_NULL_PTR}
    /* xiongxianghui00253310 add for conference 20140210 end */
};

NAS_COMM_PRINT_LIST_STRU g_astImsaMsgMsgIdArray[] =
{
    {   ID_MSG_IMSA_RP_DATA_REQ                 ,
            "MSG:  ID_MSG_IMSA_RP_DATA_REQ                           ",
        VOS_NULL_PTR},
    {   ID_MSG_IMSA_REPORT_REQ                  ,
            "MSG:  ID_MSG_IMSA_REPORT_REQ                            ",
        VOS_NULL_PTR},
    {   ID_MSG_IMSA_SMMA_REQ                    ,
            "MSG:  ID_MSG_IMSA_SMMA_REQ                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_MSG_DATA_IND                    ,
            "MSG:  ID_IMSA_MSG_DATA_IND                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_MSG_REPORT_IND                  ,
            "MSG:  ID_IMSA_MSG_REPORT_IND                            ",
        VOS_NULL_PTR},
    {   ID_IMSA_MSG_MT_ERR_IND                  ,
            "MSG:  ID_IMSA_MSG_MT_ERR_IND                            ",
        VOS_NULL_PTR}
};

NAS_COMM_PRINT_LIST_STRU g_astImsaAtMsgIdArray[] =
{
    {   ID_AT_IMSA_CIREG_SET_REQ                 ,
            "MSG:  ID_AT_IMSA_CIREG_SET_REQ                           ",
        VOS_NULL_PTR},
    {   ID_AT_IMSA_CIREG_QRY_REQ                  ,
            "MSG:  ID_AT_IMSA_CIREG_QRY_REQ                            ",
        VOS_NULL_PTR},
    {   ID_AT_IMSA_CIREP_SET_REQ                    ,
            "MSG:  ID_AT_IMSA_CIREP_SET_REQ                              ",
        VOS_NULL_PTR},
    {   ID_AT_IMSA_CIREP_QRY_REQ                    ,
            "MSG:  ID_AT_IMSA_CIREP_QRY_REQ                              ",
        VOS_NULL_PTR},
    {   ID_AT_IMSA_VOLTEIMPU_QRY_REQ                    ,
            "MSG:  ID_AT_IMSA_VOLTEIMPU_QRY_REQ                              ",
        VOS_NULL_PTR},
    {   ID_AT_IMSA_CCWAI_SET_REQ                    ,
            "MSG:  ID_AT_IMSA_CCWAI_SET_REQ                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREG_SET_CNF                  ,
            "MSG:  ID_IMSA_AT_CIREG_SET_CNF                            ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREG_QRY_CNF                  ,
            "MSG:  ID_IMSA_AT_CIREG_QRY_CNF                            ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREP_SET_CNF                  ,
            "MSG:  ID_IMSA_AT_CIREP_SET_CNF                            ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREP_QRY_CNF                    ,
            "MSG:  ID_IMSA_AT_CIREP_QRY_CNF                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_VOLTEIMPU_QRY_CNF                    ,
            "MSG:  ID_IMSA_AT_VOLTEIMPU_QRY_CNF                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CCWAI_SET_CNF                    ,
            "MSG:  ID_IMSA_AT_CCWAI_SET_CNF                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREGU_IND                    ,
            "MSG:  ID_IMSA_AT_CIREGU_IND                              ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREPH_IND                  ,
            "MSG:  ID_IMSA_AT_CIREPH_IND                            ",
        VOS_NULL_PTR},
    {   ID_IMSA_AT_CIREPI_IND                  ,
            "MSG:  ID_IMSA_AT_CIREPI_IND                            ",
        VOS_NULL_PTR}

};


NAS_COMM_PRINT_LIST_STRU g_astImsaCallMsgIdArray[] =
{
    {   ID_CALL_IMSA_SRVCC_STATUS_NOTIFY         ,
            "MSG:  ID_CALL_IMSA_SRVCC_STATUS_NOTIFY                  ",
        IMSA_PrintCallImsaSrvccStatusNotify},
    {   ID_IMSA_CALL_SRVCC_CALL_INFO_NOTIFY      ,
            "MSG:  ID_IMSA_CALL_SRVCC_CALL_INFO_NOTIFY               ",
        VOS_NULL_PTR},
    {   ID_IMSA_CALL_MSG_SYNC_IND      ,
            "MSG:  ID_IMSA_CALL_MSG_SYNC_IND                         ",
        VOS_NULL_PTR}
};

NAS_COMM_PRINT_LIST_STRU g_astImsaImsMsgIdArray[] =
{
    {   ID_IMSA_IMS_INPUT_CALL_MSG               ,
            "MSG:  ID_IMSA_IMS_INPUT_CALL_MSG                      ",
        IMSA_PrintImsaImsInputCall},
    {   ID_IMSA_IMS_INPUT_SMS_MSG     ,
            "MSG:  ID_IMSA_IMS_INPUT_SMS_MSG                       ",
        IMSA_PrintImsaImsInputSms},
    {   ID_IMSA_IMS_INPUT_SERVICE_MSG               ,
            "MSG:  ID_IMSA_IMS_INPUT_SERVICE_MSG                   ",
        IMSA_PrintImsaImsInputService},
    {   ID_IMSA_IMS_INPUT_SYSTEM_MSG               ,
            "MSG:  ID_IMSA_IMS_INPUT_SYSTEM_MSG                     ",
        IMSA_PrintImsaImsInputSystem},
    {   ID_IMSA_IMS_INPUT_PARA_MSG             ,
            "MSG:  ID_IMSA_IMS_INPUT_PARA_MSG                    ",
        IMSA_PrintImsaImsInputPara},
    {   ID_IMSA_IMS_INPUT_USSD_MSG             ,
            "MSG:  ID_IMSA_IMS_INPUT_USSD_MSG                    ",
        IMSA_PrintImsaImsInputPara},
    {   ID_IMSA_IMS_INPUT_NV_INFO_MSG             ,
            "MSG:  ID_IMSA_IMS_INPUT_NV_INFO_MSG                    ",
        VOS_NULL_PTR},

    {   ID_IMS_IMSA_OUTPUT_CALL_MSG     ,
            "MSG:  ID_IMS_IMSA_OUTPUT_CALL_MSG                     ",
        IMSA_PrintImsaImsOutputCall},
    {   ID_IMS_IMSA_OUTPUT_SMS_MSG               ,
            "MSG:  ID_IMS_IMSA_OUTPUT_SMS_MSG                      ",
        IMSA_PrintImsaImsOutputSms},
    {   ID_IMS_IMSA_OUTPUT_SERVICE_MSG               ,
            "MSG:  ID_IMS_IMSA_OUTPUT_SERVICE_MSG                  ",
        IMSA_PrintImsaImsOutputService},
    {   ID_IMS_IMSA_OUTPUT_SYSTEM_MSG             ,
            "MSG:  ID_IMS_IMSA_OUTPUT_SYSTEM_MSG                    ",
        IMSA_PrintImsaImsOutputSystem},
    {   ID_IMS_IMSA_OUTPUT_PARA_MSG               ,
            "MSG:  ID_IMS_IMSA_OUTPUT_PARA_MSG                   ",
        IMSA_PrintImsaImsOutputPara},
    {   ID_IMS_IMSA_OUTPUT_USSD_MSG               ,
            "MSG:  ID_IMS_IMSA_OUTPUT_USSD_MSG                   ",
        IMSA_PrintImsaImsOutputPara},
    {   ID_IMS_IMSA_OUTPUT_NV_INFO_MSG             ,
            "MSG:  IID_IMS_IMSA_OUTPUT_NV_INFO_MSG                    ",
        VOS_NULL_PTR}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astCallSupsCmdArray[] =
{
    {   MN_CALL_SUPS_CMD_REL_HELD_OR_UDUB                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_HELD_OR_UDUB                 \r\n"},
    {   MN_CALL_SUPS_CMD_REL_ACT_ACPT_OTH                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_ACT_ACPT_OTH                 \r\n"},
    {   MN_CALL_SUPS_CMD_REL_CALL_X                    ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_CALL_X                       \r\n"},
    {   MN_CALL_SUPS_CMD_REL_ALL_CALL                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_ALL_CALL                     \r\n"},
    {   MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH                \r\n"},
    {   MN_CALL_SUPS_CMD_HOLD_ALL_EXCPT_X                    ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_HOLD_ALL_EXCPT_X                 \r\n"},
    {   MN_CALL_SUPS_CMD_BUILD_MPTY                    ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_BUILD_MPTY                       \r\n"},
    {   MN_CALL_SUPS_CMD_ECT                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_ECT                              \r\n"},
    {   MN_CALL_SUPS_CMD_DEFLECT_CALL                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_DEFLECT_CALL                     \r\n"},
    {   MN_CALL_SUPS_CMD_ACT_CCBS                    ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_ACT_CCBS                         \r\n"},
    {   MN_CALL_SUPS_CMD_REL_ALL_EXCEPT_WAITING_CALL                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_ALL_EXCEPT_WAITING_CALL      \r\n"},
    {   MN_CALL_SUPS_CMD_REL_HELD                   ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_HELD                         \r\n"},
    {   MN_CALL_SUPS_CMD_REL_ACTIVE                    ,
            "enCallSupsCmd:  MN_CALL_SUPS_CMD_REL_ACTIVE                       \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astCallMsgEvtArray[] =
{
    {   MN_CALL_EVT_ORIG                   ,
            "enEventType:  MN_CALL_EVT_ORIG                                  \r\n"},
    {   MN_CALL_EVT_CALL_PROC                   ,
            "enEventType:  MN_CALL_EVT_CALL_PROC                             \r\n"},
    {   MN_CALL_EVT_ALERTING                    ,
            "enEventType:  MN_CALL_EVT_ALERTING                              \r\n"},
    {   MN_CALL_EVT_CONNECT                   ,
            "enEventType:  MN_CALL_EVT_CONNECT                               \r\n"},
    {   MN_CALL_EVT_RELEASED                   ,
            "enEventType:  MN_CALL_EVT_RELEASED                              \r\n"},
    {   MN_CALL_EVT_INCOMING                    ,
            "enEventType:  MN_CALL_EVT_INCOMING                              \r\n"},
    {   MN_CALL_EVT_SS_CMD_PROGRESS                    ,
            "enEventType:  MN_CALL_EVT_SS_CMD_PROGRESS                       \r\n"},
    {   MN_CALL_EVT_SS_CMD_RSLT                    ,
            "enEventType:  MN_CALL_EVT_SS_CMD_RSLT                           \r\n"},
    {   MN_CALL_EVT_START_DTMF_CNF                   ,
            "enEventType:  MN_CALL_EVT_START_DTMF_CNF                        \r\n"},
    {   MN_CALL_EVT_STOP_DTMF_CNF                    ,
            "enEventType:  MN_CALL_EVT_STOP_DTMF_CNF                         \r\n"},
    {   MN_CALL_EVT_HOLD                   ,
            "enEventType:  MN_CALL_EVT_HOLD                                  \r\n"},
    {   MN_CALL_EVT_RETRIEVE                   ,
            "enEventType:  MN_CALL_EVT_RETRIEVE                              \r\n"},
    {   MN_CALL_EVT_ERR_IND                    ,
            "enEventType:  MN_CALL_EVT_ERR_IND                               \r\n"},
    {   MN_CALL_EVT_CALL_ORIG_CNF                    ,
            "enEventType:  MN_CALL_EVT_CALL_ORIG_CNF                         \r\n"},
    {   MN_CALL_EVT_SUPS_CMD_CNF                    ,
            "enEventType:  MN_CALL_EVT_SUPS_CMD_CNF                          \r\n"},
    {   MN_CALL_EVT_ALL_RELEASED                    ,
            "enEventType:  MN_CALL_EVT_ALL_RELEASED                          \r\n"},
    {   MN_CALL_EVT_START_DTMF_RSLT                    ,
            "enEventType:  MN_CALL_EVT_START_DTMF_RSLT                       \r\n"},
    {   MN_CALL_EVT_STOP_DTMF_RSLT                    ,
            "enEventType:  MN_CALL_EVT_STOP_DTMF_RSLT                        \r\n"}

};

NAS_COMM_PRINT_MSG_LIST_STRU g_astInputCallReasonArray[] =
{
    {   IMSA_IMS_INPUT_CALL_REASON_DIAL                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_DIAL                           \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_REPORT                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_REPORT                         \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_ANSWER                    ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_ANSWER                         \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_HANGUP                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_HANGUP                         \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_SWAP                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_SWAP                           \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_REL_ACTIVE_AND_ACCEPT_OTH                    ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_REL_ACTIVE_AND_ACCEPT_OTH      \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_REL_HELD_OR_WAITING                    ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_REL_HELD_OR_WAITING            \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_HOLD_ALL_EXCEPT_X                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_HOLD_ALL_EXCEPT_X              \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_DTMF                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_DTMF                           \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_RELEASE_AT_X                    ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_RELEASE_AT_X                   \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_CONFERENCE                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_CONFERENCE                     \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_SRVCC_START                    ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_SRVCC_START                    \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_SRVCC_SUCCESS                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_SRVCC_SUCCESS                  \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_SRVCC_FAILED                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_SRVCC_FAILED                   \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_SRVCC_CANCELED                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_SRVCC_CANCELED                  \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_RESOURCE_READY                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_RESOURCE_READY                 \r\n"},
    {   IMSA_IMS_INPUT_CALL_REASON_RESOURCE_FAILED                   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_RESOURCE_FAILED                \r\n"},
    /* xiongxianghui00253310 add for conference 20140214 begin */
    {   IMSA_IMS_INPUT_CALL_REASON_CONFERENCE_INVITE_NEW_PARTICIPANT   ,
            "REASON:  IMSA_IMS_INPUT_CALL_REASON_CONFERENCE_INVITE_NEW_PARTICIPANT  \r\n"}
    /* xiongxianghui00253310 add for conference 20140214 end */
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astInputSmsReasonArray[] =
{
    {   IMSA_IMS_INPUT_SMS_REASON_SEND_MESSAGE                   ,
            "REASON:  IMSA_IMS_INPUT_SMS_REASON_SEND_MESSAGE                    \r\n"},
    {   IMSA_IMS_INPUT_SMS_REASON_TR1M_EXP                   ,
            "REASON:  IMSA_IMS_INPUT_SMS_REASON_TR1M_EXP                        \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astInputServiceReasonArray[] =
{
    {   IMSA_IMS_INPUT_SERVICE_REASON_REGISTER                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_REGISTER                    \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_REGISTER_EMERGENCY                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_REGISTER_EMERGENCY          \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_DEREGISTER                    ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_DEREGISTER                  \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_LOCAL_DEREGISTER                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_LOCAL_DEREGISTER            \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_LOCAL_DEREGISTER_EMERGENCY                    ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_LOCAL_DEREGISTER_EMERGENCY  \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SUCCESS                    ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SUCCESS        \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_NETWORK_FAILURE                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_NETWORK_FAILURE\r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SYNC_FAILURE                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SYNC_FAILURE   \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_NRM_SRV                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_NRM_SRV   \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV   \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_EMC_SRV                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_EMC_SRV   \r\n"},
    {   IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV                   ,
            "REASON:  IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV   \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astInputSystemReasonArray[] =
{
    {   IMSA_IMS_INPUT_SYSTEM_REASON_START                   ,
            "REASON:  IMSA_IMS_INPUT_SYSTEM_REASON_START                        \r\n"},
    {   IMSA_IMS_INPUT_SYSTEM_REASON_STOP                    ,
            "REASON:  IMSA_IMS_INPUT_SYSTEM_REASON_STOP                         \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astInputParaReasonArray[] =
{
    {   IMAS_IMS_INPUT_PARA_REASON_SET_CGI                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_CGI                        \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_NORMAL_IP                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_NORMAL_IP                  \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_EMC_IP                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_EMC_IP                     \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_PORTS                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_PORTS                      \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_IPSEC                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_IPSEC                      \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_NORMAL_ACCOUNT                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_NORMAL_ACCOUNT             \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_EMC_ACCOUNT                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_EMC_ACCOUNT                \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_UE_CAPABILITIES                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_UE_CAPABILITIES            \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_NETWORK_CAPABILITIES                   ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_NETWORK_CAPABILITIES       \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_TIMER_LENGTH                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_TIMER_LENGTH               \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_IMEI                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_IMEI                       \r\n"},
    {   IMAS_IMS_INPUT_PARA_REASON_SET_CALL_WAITING                    ,
            "REASON:  IMAS_IMS_INPUT_PARA_REASON_SET_CALL_WAITING               \r\n"}
};


NAS_COMM_PRINT_MSG_LIST_STRU g_astOutputCallReasonArray[] =
{
    {   IMSA_IMS_OUTPUT_CALL_REASON_OK                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_OK                            \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_ERROR                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_ERROR                         \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_CALL_LIST                    ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_CALL_LIST                     \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_DISCONNECT_EVENT                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_DISCONNECT_EVENT              \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_INCOMING_EVENT                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_INCOMING_EVENT                \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_WAITING_EVENT                    ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_WAITING_EVENT                 \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_INITIALIZING_EVENT                    ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_INITIALIZING_EVENT            \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_SRVCC_CALL_LIST                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_SRVCC_CALL_LIST               \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_EARLY_MEDIA                    ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_EARLY_MEDIA                   \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_EMERGENCY_INDICATION                   ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_EMERGENCY_INDICATION          \r\n"},
    {   IMSA_IMS_OUTPUT_CALL_REASON_EXTRA_INFO                    ,
            "REASON:  IMSA_IMS_OUTPUT_CALL_REASON_EXTRA_INFO                    \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astOutputSmsReasonArray[] =
{
    {   IMSA_IMS_OUTPUT_SMS_REASON_OK                   ,
            "REASON:  IMSA_IMS_OUTPUT_SMS_REASON_OK                             \r\n"},
    {   IMSA_IMS_OUTPUT_SMS_REASON_ERROR                   ,
            "REASON:  IMSA_IMS_OUTPUT_SMS_REASON_ERROR                          \r\n"},
    {   IMSA_IMS_OUTPUT_SMS_REASON_SMS_RECEIVED                   ,
            "REASON:  IMSA_IMS_OUTPUT_SMS_REASON_SMS_RECEIVED                   \r\n"},
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astOutputServiceReasonArray[] =
{
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_OK                   ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_OK                          \r\n"},
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_ERROR                   ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_ERROR                       \r\n"},
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_STATE                    ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_STATE                       \r\n"},
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_AUTH_CHALLENGE                   ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_AUTH_CHALLENGE              \r\n"},
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_SETUP                    ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_SETUP                 \r\n"},
    {   IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_RELEASE                    ,
            "REASON:  IMSA_IMS_OUTPUT_SEVICE_REASON_IPSEC_RELEASE               \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astOutputSystemReasonArray[] =
{
    {   IMSA_IMS_OUTPUT_SYSTM_REASON_OK                   ,
            "REASON:  IMSA_IMS_OUTPUT_SYSTM_REASON_OK                           \r\n"},
    {   IMSA_IMS_OUTPUT_SYSTM_REASON_ERROR                    ,
            "REASON:  IMSA_IMS_OUTPUT_SYSTM_REASON_ERROR                        \r\n"}
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astOutputParaReasonArray[] =
{
    {   IMSA_IMS_OUTPUT_PARA_REASON_SET_OK                   ,
            "REASON:  IMSA_IMS_OUTPUT_PARA_REASON_SET_OK                        \r\n"},
    {   IMSA_IMS_OUTPUT_PARA_REASON_SET_PORTS_ERROR                    ,
            "REASON:  IMSA_IMS_OUTPUT_PARA_REASON_SET_PORTS_ERROR               \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_CGI_ERROR                   ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_CGI_ERROR                 \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_NORMAL_IP_ERROR                   ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_NORMAL_IP_ERROR           \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_EMC_IP_ERROR                    ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_EMC_IP_ERROR              \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_IPSEC_ERROR                   ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_IPSEC_ERROR               \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_NORMAL_ACCOUNT_ERROR                    ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_NORMAL_ACCOUNT_ERROR      \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_EMC_ACCOUNT_ERROR                   ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_EMC_ACCOUNT_ERROR         \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_TIMER_LENGTH_ERROR                   ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_TIMER_LENGTH_ERROR        \r\n"},
    {   IMAS_IMS_OUTPUT_PARA_REASON_SET_REG_CAPABILITIES_ERROR                    ,
            "REASON:  IMAS_IMS_OUTPUT_PARA_REASON_SET_REG_CAPABILITIES_ERROR    \r\n"},
    {   IMSA_IMS_OUTPUT_PARA_REASON_QUERY_OK                    ,
            "REASON:  IMSA_IMS_OUTPUT_PARA_REASON_QUERY_OK                        \r\n"}
};



/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/

VOS_VOID  IMSA_SndOmImsaTimerStatus(
    IMSA_TIMER_RUN_STA_ENUM_UINT32          enTimerStatus,
    VOS_UINT16                          enTimerId,
    VOS_UINT32                          ulTimerRemainLen
)
{
    IMSA_TIMER_INFO_STRU                     *pstMsg = VOS_NULL_PTR;

    pstMsg = (IMSA_TIMER_INFO_STRU*)IMSA_ALLOC_MSG(sizeof(IMSA_TIMER_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        IMSA_ERR_LOG("IMSA_SndOmImsaTimerStatus: mem alloc fail!.");
        return;
    }

    IMSA_WRITE_INTRA_MSG_HEAD(  pstMsg,
                                enTimerId + PS_MSG_ID_IMSA_TO_IMSA_OM_BASE,
                                sizeof(IMSA_TIMER_INFO_STRU));

    pstMsg->enTimerStatus               = enTimerStatus;
    pstMsg->usTimerId                   = enTimerId;
    pstMsg->usRsv                       = 0;

    pstMsg->ulTimerRemainLen            = ulTimerRemainLen;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    IMSA_FREE_MSG(pstMsg);

}



/*****************************************************************************
 Function Name  : IMSA_StartTimer()
 Description    : 启动定时器函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_StartTimer(IMSA_TIMER_STRU *pstTimer)
{
    if(pstTimer == VOS_NULL_PTR)
    {
        return;
    }

    /*判断定时器是否打开，已打开则关闭*/
    if(VOS_NULL_PTR != pstTimer->phTimer)
    {
        /*关闭失败，则报警返回*/
        if (VOS_OK != PS_STOP_REL_TIMER(&(pstTimer->phTimer)))
        {
            /*打印异常信息*/
            IMSA_WARN_LOG("IMSA_StartTimer:WARN: stop reltimer error!");
            return;
        }

        /*打印异常信息*/
        IMSA_WARN_LOG1("(TimerType) Timer not close!",pstTimer->usName);
    }

    if (pstTimer->ulTimerLen == 0)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_StopTimer:ERROR: start unreasonable reltimer.");
        return;
    }

    /*设定定时器NAME，打开失败则报警返回*/
    if (VOS_OK !=\
            PS_START_REL_TIMER(&(pstTimer->phTimer),PS_PID_IMSA,\
                                pstTimer->ulTimerLen,(VOS_UINT32)pstTimer->usName,\
                                pstTimer->usPara,pstTimer->ucMode))
    {
          /*打印异常信息*/
          IMSA_WARN_LOG("IMSA_StartTimer:WARN: start reltimer error!");
          return;
    }

    /*钩出当前的定时器信息 */
    /*bsun*/
    IMSA_SndOmImsaTimerStatus(IMSA_TIMER_RUNNING, pstTimer->usName, pstTimer->ulTimerLen);

}

/*****************************************************************************
 Function Name  : IMSA_StopTimer()
 Description    : 停止定时器函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_StopTimer(IMSA_TIMER_STRU *pstTimer)
{
    VOS_UINT32                          ulTimerRemainLen;

    if(pstTimer == VOS_NULL_PTR)
    {
        return;
    }

    /*判断定时器是否打开，已打开则关闭*/
    if(VOS_NULL_PTR != pstTimer->phTimer)
    {
        /* 获取当前定时器的剩余时间 */
        if (VOS_OK != VOS_GetRelTmRemainTime(&(pstTimer->phTimer), &ulTimerRemainLen ))
        {
            ulTimerRemainLen = 0;
        }

        /*关闭失败，则报警返回*/
        if (VOS_OK != PS_STOP_REL_TIMER(&(pstTimer->phTimer)))
        {
            /*打印异常信息*/
            IMSA_WARN_LOG("IMSA_StopTimer:WARN: stop reltimer error!");
            return;
        }

        pstTimer->phTimer = VOS_NULL_PTR;

        IMSA_INFO_LOG1("(TimerType) Timer closed!",pstTimer->usName);
    }

    /*钩出当前的定时器信息 */
    /*bsun*/
    /* modify by jiqiang 2014.03.24 pclint fix error 644 begin */
    /*lint -e644*/
    IMSA_SndOmImsaTimerStatus(IMSA_TIMER_STOPED, pstTimer->usName, ulTimerRemainLen);
    /*lint +e644*/
    /* modify by jiqiang 2014.03.24 pclint fix error 644 end */


}



/*****************************************************************************
 Function Name  : IMSA_IsTimerRunning()
 Description    : 查询定时器是否正在运行
 Input          : IMSA_TIMER_STRU *pstTimer
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-26  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_IsTimerRunning(const IMSA_TIMER_STRU *pstTimer)
{
    /*入参错误*/
    if(pstTimer == VOS_NULL_PTR)
    {
        return VOS_FALSE;
    }

    /*判断定时器是否打开，已打开则关闭*/
    if(VOS_NULL_PTR != pstTimer->phTimer)
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}


VOS_VOID IMSA_GetRemainTimeLen
(
    IMSA_TIMER_STRU                    *pstTimer,
    VOS_UINT32                         *pulRemainTimeLen
)
{
    VOS_UINT32                          ulRemainTick = IMSA_NULL;

    if (VOS_FALSE == IMSA_IsTimerRunning(pstTimer))
    {
        *pulRemainTimeLen = 0;
    }

    if (VOS_OK != VOS_GetRelTmRemainTime(&(pstTimer->phTimer), &ulRemainTick))
    {
        *pulRemainTimeLen = 0;
    }

    *pulRemainTimeLen = ulRemainTick * IMSA_TIMER_1_TICK_10_MS;
}



/*****************************************************************************
 Function Name  : IMSA_ProcReadIsimFileResult()
 Description    : 处理读卡结果
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-21  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_ProcReadIsimFileResult(VOS_VOID *pRcvMsg)
{
    PS_USIM_GET_FILE_CNF_STRU  *pstUsimCnf;
    IMSA_ISIM_ACTION_FUN        pActFun   = VOS_NULL_PTR;
    VOS_UINT32                  i;
    IMSA_ISIM_DATA_STRU         stIsimData;
    IMSA_CONTROL_MANAGER_STRU   *pstControlManager;

    pstUsimCnf          = (PS_USIM_GET_FILE_CNF_STRU*)pRcvMsg;
    pstControlManager   = IMSA_GetControlManagerAddress();

    if(VOS_OK != pstUsimCnf->ulResult )
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        IMSA_WARN_LOG1("IMSA_ProcReadIsimFileResult: Read File err. UsimFileId =  ",
                                        pstUsimCnf->usEfId);
        return VOS_ERR;
    }

    stIsimData.pData    = pstUsimCnf->aucEf;
    stIsimData.usDataLen= pstUsimCnf->usEfLen;

    for(i = 0; i < g_ulIsimDataMapNum; i++)
    {
        /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
        if(pstUsimCnf->usEfId == g_astIsimDataMap[i].enIsimFileId)
        {
           pActFun            = g_astIsimDataMap[i].pfDecodeActionFun;
           break;
        }
    }

    if (VOS_NULL_PTR != pActFun)
    {
        (VOS_VOID)(*pActFun) (&stIsimData);

        if ((IMSA_STATUS_STARTED == pstControlManager->enImsaStatus )
            && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi)
            && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu)
            && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName))
        {
            /* config account info to ims */
            IMSA_ConfigAccoutInfo2Ims(IMSA_REG_TYPE_NORMAL);

            /* config account info to ims */
            IMSA_ConfigAccoutInfo2Ims(IMSA_REG_TYPE_EMC);
        }

        return VOS_OK;
    }
    else
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        IMSA_WARN_LOG1("IMSA_ProcReadIsimFileResult: no fun deal with UsimFileId =  ",
                                        pstUsimCnf->usEfId);
        return  VOS_ERR;
    }

}

/*****************************************************************************
 Function Name  : IMSA_DecodeIsimImpu()
 Description    : IMPU译码
 Input          : IMSA_ISIM_DATA_STRU *pstIsimData
 Output         : VOS_VOID
 Return Value   : VOS_UINT32

 History        :
      1.sunbing 49683      2013-06-27  Draft Enact
*****************************************************************************/
VOS_UINT32  IMSA_DecodeIsimImpu(const IMSA_ISIM_DATA_STRU *pstIsimData)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    /*31.103 4.2.4描述卡中IMPU的格式为TLV，tag = '80'*/
    if( 0x80 != pstIsimData->pData[0])
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimImpu:tag = ", pstIsimData->pData[0]);
    }

    /*长度不合法，则设置为最大值*/
    if(pstIsimData->pData[1]> IMSA_MAX_TIMPU_LEN)
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimImpu:lenth = ", pstIsimData->pData[1]);

        pstIsimData->pData[1] = IMSA_MAX_TIMPU_LEN;
    }

    pstControlManager = IMSA_GetControlManagerAddress();

    if (VOS_FALSE == pstControlManager->stImsaConfigPara.ucUserInfoFlag)
    {
        pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu = IMSA_OP_TRUE;

        IMSA_MEM_SET(&pstControlManager->stImsaCommonInfo.stImsaUeId.stTImpu,\
                  0,\
                  IMSA_MAX_TIMPU_LEN+1);

        IMSA_MEM_CPY(&pstControlManager->stImsaCommonInfo.stImsaUeId.stTImpu, \
                 &pstIsimData->pData[2], \
                  pstIsimData->pData[1]);
    }

    return VOS_TRUE;
}

/*****************************************************************************
 Function Name  : IMSA_DecodeIsimImpi()
 Description    : IMPI译码
 Input          : IMSA_ISIM_DATA_STRU *pstIsimData
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-27  Draft Enact
*****************************************************************************/
VOS_UINT32  IMSA_DecodeIsimImpi(const IMSA_ISIM_DATA_STRU *pstIsimData)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    /*31.103 4.2.2描述卡中IMPI的格式为TLV，tag = '80'*/
    if( 0x80 != pstIsimData->pData[0])
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimImpu:tag = ", pstIsimData->pData[0]);
    }

    /*长度不合法，则设置为最大值*/
    if(pstIsimData->pData[1]> IMSA_MAX_IMPI_LEN)
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimImpi:lenth = ", pstIsimData->pData[1]);

        pstIsimData->pData[1] = IMSA_MAX_IMPI_LEN;
    }

    pstControlManager = IMSA_GetControlManagerAddress();

    if (VOS_FALSE == pstControlManager->stImsaConfigPara.ucUserInfoFlag)
    {
        pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi = IMSA_OP_TRUE;

        IMSA_MEM_SET(&pstControlManager->stImsaCommonInfo.stImsaUeId.stImpi,\
                  0,\
                  IMSA_MAX_IMPI_LEN +1);

        IMSA_MEM_CPY(&pstControlManager->stImsaCommonInfo.stImsaUeId.stImpi, \
                 &pstIsimData->pData[2], \
                  pstIsimData->pData[1]);
    }


    return VOS_TRUE;

}

/*****************************************************************************
 Function Name  : IMSA_DecodeIsimDomain()
 Description    : Domain译码
 Input          : IMSA_ISIM_DATA_STRU *pstIsimData
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-27  Draft Enact
*****************************************************************************/
VOS_UINT32  IMSA_DecodeIsimDomain(const IMSA_ISIM_DATA_STRU *pstIsimData)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    /*31.103 4.2.3描述卡中DOMAIN的格式为TLV，tag = '80'*/
    if( 0x80 != pstIsimData->pData[0])
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimDomain:tag = ", pstIsimData->pData[0]);
    }

    /*长度不合法，则设置为最大值*/
    if(pstIsimData->pData[1]> IMSA_MAX_HOME_NET_DOMAIN_NAME_LEN)
    {
        IMSA_INFO_LOG1("IMSA_DecodeIsimDomain:lenth = ", pstIsimData->pData[1]);

        pstIsimData->pData[1] = IMSA_MAX_HOME_NET_DOMAIN_NAME_LEN;
    }

    pstControlManager = IMSA_GetControlManagerAddress();

    if (VOS_FALSE == pstControlManager->stImsaConfigPara.ucUserInfoFlag)
    {
        pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName = IMSA_OP_TRUE;

        IMSA_MEM_SET(&pstControlManager->stImsaCommonInfo.stHomeNetDomainName,\
                  0,\
                  IMSA_MAX_HOME_NET_DOMAIN_NAME_LEN +1);

        IMSA_MEM_CPY(&pstControlManager->stImsaCommonInfo.stHomeNetDomainName, \
                 &pstIsimData->pData[2], \
                  pstIsimData->pData[1]);
    }

    return VOS_TRUE;

}

/*****************************************************************************
 Function Name  : IMSA_DecodeIsimImpu()
 Description    : IMPU译码
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-27  Draft Enact
*****************************************************************************/
VOS_UINT32  IMSA_EncodeIsimFile(const IMSA_ISIM_DATA_STRU *pstIsimData)
{
    (VOS_VOID)pstIsimData;

    return 0;
}
/*lint -e732*/
/*lint -e718*/
/*lint -e516*/
/*****************************************************************************
 Function Name  : IMSA_ReadImsaNvImsRatSupportConfig()
 Description    : 读取不同接入技术下IMS的支持配置信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-27  Draft Enact
*****************************************************************************/
VOS_VOID  IMSA_ReadImsaNvImsRatSupportConfig( VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    IMSA_NV_IMS_RAT_SUPPORT_STRU    stNvImsRatSupport;
    VOS_UINT32                      ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_RAT_SUPPORT,&stNvImsRatSupport,\
                 sizeof(IMSA_NV_IMS_RAT_SUPPORT_STRU));

    if(ulRslt == VOS_OK)
    {
        /*根据NV内容设置*/
        pstControlManager->stImsaConfigPara.ucGsmEmsSupportFlag = stNvImsRatSupport.ucGsmEmsSupportFlag;
        pstControlManager->stImsaConfigPara.ucGsmImsSupportFlag = stNvImsRatSupport.ucGsmImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucUtranEmsSupportFlag= stNvImsRatSupport.ucUtranEmsSupportFlag;
        pstControlManager->stImsaConfigPara.ucUtranImsSupportFlag= stNvImsRatSupport.ucUtranImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucLteEmsSupportFlag= stNvImsRatSupport.ucLteEmsSupportFlag;
        pstControlManager->stImsaConfigPara.ucLteImsSupportFlag= stNvImsRatSupport.ucLteImsSupportFlag;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadImsaNvImsRatSupportConfig: read NV err!");

        /*设置初始值*/
        pstControlManager->stImsaConfigPara.ucGsmEmsSupportFlag = VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucGsmImsSupportFlag = VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucUtranEmsSupportFlag= VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucUtranImsSupportFlag= VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucLteEmsSupportFlag= VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucLteImsSupportFlag= VOS_TRUE;
    }

}


VOS_VOID  IMSA_ReadImsaNvImsaConfig( VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;
    IMSA_REG_MANAGER_STRU              *pstImsaRegManager   = IMSA_RegCtxGet();
    IMSA_NV_IMSA_CONFIG_STRU            stNvImsaConfig;
    VOS_UINT32                          ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMSA_CONFIG,&stNvImsaConfig,\
                 sizeof(IMSA_NV_IMSA_CONFIG_STRU));

    if(ulRslt == VOS_OK)
    {
        /*根据NV内容设置*/
        pstControlManager->stImsaConfigPara.ucNoCardEmcCallSupportFlag
                    = stNvImsaConfig.ucNoCardEmcCallSupportFlag;
        pstControlManager->stImsaConfigPara.ucImsLocDeregSigPdpRelFlag
                    = stNvImsaConfig.ucImsLocDeregSigPdpRelFlag;
        pstControlManager->stImsaConfigPara.ucUeRelMediaPdpFLag
                    = stNvImsaConfig.ucUeRelMediaPdpFLag;
        pstImsaRegManager->ulMaxTime    = stNvImsaConfig.ulMaxTime;
        pstImsaRegManager->ulBaseTime   = stNvImsaConfig.ulBaseTime;
        pstImsaRegManager->ulRetryPeriod= stNvImsaConfig.ulRegFailRetryIntervel;
        pstImsaRegManager->ulSaveRetryTimes = stNvImsaConfig.ucRetryTimes;
        IMSA_MEM_CPY(   &pstControlManager->stImsaConfigPara.stImsRedialCfg,
                        &stNvImsaConfig.stImsRedialCfg,
                        sizeof(IMSA_NV_IMS_REDIAL_CFG_STRU));
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadImsaNvImsaConfig: read NV err!");

        /*设置初始值*/
        pstControlManager->stImsaConfigPara.ucNoCardEmcCallSupportFlag
                    = VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucImsLocDeregSigPdpRelFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucUeRelMediaPdpFLag
                    = VOS_FALSE;
        pstImsaRegManager->ulMaxTime    = 1800;
        pstImsaRegManager->ulBaseTime   = 30;
        pstImsaRegManager->ulRetryPeriod= 0;

        IMSA_MEM_SET(   &pstControlManager->stImsaConfigPara.stImsRedialCfg,
                        0,
                        sizeof(IMSA_NV_IMS_REDIAL_CFG_STRU));
    }
}


VOS_VOID  IMSA_ReadImsaNvImsCapability( VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    IMS_NV_IMS_CAP_STRU             stNvImsCapability;
    VOS_UINT32                      ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_CAPABILITY,&stNvImsCapability,\
                 sizeof(IMS_NV_IMS_CAP_STRU));

    if(ulRslt == VOS_OK)
    {
        /*根据NV内容设置*/
        pstControlManager->stImsaConfigPara.ucVoiceCallOnImsSupportFlag
                    = stNvImsCapability.ucVoiceCallOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucVideoCallOnImsSupportFlag
                    = stNvImsCapability.ucVideoCallOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSmsOnImsSupportFlag
                    = stNvImsCapability.ucSmsOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSrvccOnImsSupportFlag
                    = stNvImsCapability.ucSrvccOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSrvccMidCallOnImsSupportFlag
                    = stNvImsCapability.ucSrvccMidCallOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSrvccAlertingOnImsSupportFlag
                    = stNvImsCapability.ucSrvccAlertingOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSrvccPreAlertingOnImsSupportFlag
                    = stNvImsCapability.ucSrvccPreAlertingOnImsSupportFlag;
        pstControlManager->stImsaConfigPara.ucSrvccTiFlag
                    = stNvImsCapability.ucSrvccTiFlag;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadImsaNvImsCapability: read NV err!");

        /*设置初始值*/
        pstControlManager->stImsaConfigPara.ucVoiceCallOnImsSupportFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucVideoCallOnImsSupportFlag
                    = VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucSmsOnImsSupportFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucSrvccOnImsSupportFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucSrvccMidCallOnImsSupportFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucSrvccAlertingOnImsSupportFlag
                    = VOS_TRUE;
        pstControlManager->stImsaConfigPara.ucSrvccPreAlertingOnImsSupportFlag
                    = VOS_FALSE;
        pstControlManager->stImsaConfigPara.ucSrvccTiFlag
                    = VOS_TRUE;
    }

}



VOS_VOID  IMSA_ReadImsaNvSipPortConfig( VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    IMSA_NV_SIP_PORT_CONFIG_STRU    stNvSipPort;
    VOS_UINT32                      ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_SIP_PORT_CONFIG,&stNvSipPort,\
                 sizeof(IMSA_NV_SIP_PORT_CONFIG_STRU));

    if ((ulRslt == VOS_OK)
        && (stNvSipPort.usImsMaxPort >= stNvSipPort.usImsMinPort))
    {
        /*根据NV内容设置*/
        pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort
                    = stNvSipPort.usImsMinPort;
        pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort
                    = stNvSipPort.usImsMaxPort;
        #if 0
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulSip
                    = stNvSipPort.ulSip;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulAudio
                    = stNvSipPort.ulAudio;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulAudioPoolSize
                    = stNvSipPort.ulAudioPoolSize;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulVideo
                    = stNvSipPort.ulVideo;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulVideoPoolSize
                    = stNvSipPort.ulVideoPoolSize;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulProtectedSip
                    = stNvSipPort.ulProtectedSip;
        pstControlManager->stImsaConfigPara.stImsPortConfig.ulProtectedSipPoolSize
                    = stNvSipPort.ulProtectedSipPoolSize;
        #endif

    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadImsaNvSipPortConfig: read NV err!");

        /*设置初始值*/
        pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort = 5000;
        pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort = 32000;
        /*IMSA_MEM_SET(&pstControlManager->stImsaConfigPara.stImsPortConfig,
                     0, sizeof(IMSA_IMS_PORT_CONFIG_STRU));*/
    }

}


VOS_VOID  IMSA_ReadImsaNvImsConfig( VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    IMSA_NV_IMS_CONFIG_STRU         stNvImsConfig;
    VOS_UINT32                      ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_CONFIG,&stNvImsConfig,\
                 sizeof(IMSA_NV_IMS_CONFIG_STRU));

    if(ulRslt == VOS_OK)
    {
        /*根据NV内容设置*/
        pstControlManager->stImsaConfigPara.ucUserInfoFlag
                = stNvImsConfig.ucUserInfoFlag;

        pstControlManager->stImsaConfigPara.ucAuthType
                = stNvImsConfig.ucAuthType;
        IMSA_MEM_CPY(pstControlManager->stImsaConfigPara.aucPassWord,
                stNvImsConfig.aucPassWord, IMSA_PASSWORD_LEN);

        if (VOS_TRUE == stNvImsConfig.ucUserInfoFlag)
        {

            pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi = IMSA_OP_TRUE;
            IMSA_MEM_CPY(pstControlManager->stImsaCommonInfo.stImsaUeId.stImpi.acImpi,
                    stNvImsConfig.acImpi, IMSA_IMPI_LEN);

            pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu = IMSA_OP_TRUE;
            IMSA_MEM_CPY(pstControlManager->stImsaCommonInfo.stImsaUeId.stTImpu.acTImpu,
                    stNvImsConfig.acTImpu, IMSA_IMPU_LEN);

            pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName = IMSA_OP_TRUE;
            IMSA_MEM_CPY(pstControlManager->stImsaCommonInfo.stHomeNetDomainName.acHomeNetDomainName,
                    stNvImsConfig.acHomeNetDomainName, IMSA_HOME_NET_DOMAIN_NAME_LEN);
        }


    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadImsaNvSipPortConfig: read NV err!");

        /*设置初始值*/
        pstControlManager->stImsaConfigPara.ucUserInfoFlag = VOS_FALSE;

        pstControlManager->stImsaConfigPara.ucAuthType = VOS_FALSE;

        IMSA_MEM_SET(pstControlManager->stImsaConfigPara.aucPassWord,
                0, IMSA_PASSWORD_LEN);

    }

}


VOS_VOID  IMSA_ReadImei( VOS_VOID )
{
    IMSA_COMMON_INFO_STRU          *pstCommInfo;
    VOS_UINT8                       aucBuf[NV_ITEM_IMEI_SIZE] = {0};
    VOS_UINT32                      i = 0;

    /* 调用SC接口读取IMEI号码 */
    if (SC_ERROR_CODE_NO_ERROR != SC_PERS_NvRead(   MODEM_ID_0,
                                                    en_NV_Item_IMEI,
                                                    aucBuf,
                                                    NV_ITEM_IMEI_SIZE))
    {
        IMSA_WARN_LOG("IMSA_ReadImeiAndImeisv: Read IMEI from NV fail!");

        aucBuf[0] = 0x3;
        aucBuf[1] = 0x5;
        aucBuf[2] = 0x6;
        aucBuf[3] = 0x6;
        aucBuf[4] = 0x2;
        aucBuf[5] = 0x6;
        aucBuf[6] = 0x0;
        aucBuf[7] = 0x0;
        aucBuf[8] = 0x7;
        aucBuf[9] = 0x7;
        aucBuf[10] = 0x3;
        aucBuf[11] = 0x1;
        aucBuf[12] = 0x7;
        aucBuf[13] = 0x4;
        aucBuf[14] = 0x0;
    }

    pstCommInfo = IMSA_GetCommonInfoAddress();
    pstCommInfo->stImsaUeId.bitOpImei  = IMSA_OP_TRUE;

    /* 将NV中格式转变为IMSA内部IMEI存放格式 */
    for (i = 0; i < IMSA_IMS_IMEI_LEN; i++)
    {
        pstCommInfo->stImsaUeId.acImei[i] = IMSA_ConverterDigit2Chacter(aucBuf[i]);
    }
    pstCommInfo->stImsaUeId.acImei[IMSA_IMS_IMEI_LEN] = '\0';
}


VOS_VOID  IMSA_ReadNvVoiceDomain(VOS_VOID )
{

    LNAS_LMM_NV_VOICE_DOMAIN_STRU   stNvVoiceDomain;
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    VOS_UINT32                      ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_UE_VOICE_DOMAIN,&stNvVoiceDomain,\
                 sizeof(LNAS_LMM_NV_VOICE_DOMAIN_STRU));

    /*NV内容准确*/
    if((VOS_OK == ulRslt) && (stNvVoiceDomain.bitOpVoicDomain == 1))
    {
        switch(stNvVoiceDomain.enVoicDomain)
        {
            case NAS_LMM_VOICE_DOMAIN_CS_ONLY:
                pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_CS_ONLY;
                break;

            case NAS_LMM_VOICE_DOMAIN_IMS_PS_ONLY:
                pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_IMS_PS_ONLY;
                break;

            case NAS_LMM_VOICE_DOMAIN_CS_PREFERRED:
                pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_CS_PREFERRED;
                break;

            case NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED:
                pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_IMS_PS_PREFERRED;
                break;

            default:
                IMSA_WARN_LOG("IMSA_ReadNvVoiceDomain: Para is err!");
                pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_IMS_PS_PREFERRED;
                break;
        }
        pstControlManager->stImsaConfigPara.enVoiceDomain = stNvVoiceDomain.enVoicDomain;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadNvVoiceDomain: read NV err.");

        pstControlManager->stImsaConfigPara.enVoiceDomain = IMSA_VOICE_DOMAIN_IMS_PS_PREFERRED;
    }

    return;
}
VOS_VOID IMSA_ReadPcscfDiscoveryPolicy(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU                   *pstControlManager;
    IMSA_NV_PCSCF_DISCOVERY_POLICY_STRU         stPcscfPolicy;
    VOS_UINT32                                  ulRslt;

    pstControlManager = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_NV_Read(EN_NV_ID_PCSCF_DISCOVERY_POLICY,&stPcscfPolicy,\
                 sizeof(IMSA_NV_PCSCF_DISCOVERY_POLICY_STRU));

    if (VOS_OK == ulRslt)
    {
        if ((IMSA_PCSCF_DISCOVERY_POLICY_PCO == stPcscfPolicy.enPcscfDiscoverPolicy) ||
            (IMSA_PCSCF_DISCOVERY_POLICY_NV == stPcscfPolicy.enPcscfDiscoverPolicy))
        {
            pstControlManager->stPcscfDiscoveryPolicyInfo.enPcscfDiscoverPolicy
                = stPcscfPolicy.enPcscfDiscoverPolicy;

            IMSA_MEM_CPY(&pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf,
                         &stPcscfPolicy.stIpv4Pcscf, sizeof(IMSA_NV_PDP_IPV4_PCSCF_STRU));

            IMSA_MEM_CPY(&pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf,
                &stPcscfPolicy.stIpv6Pcscf, sizeof(IMSA_NV_PDP_IPV6_PCSCF_STRU));
        }
        else
        {
            pstControlManager->stPcscfDiscoveryPolicyInfo.enPcscfDiscoverPolicy
                = IMSA_PCSCF_DISCOVERY_POLICY_PCO;
        }
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadPcscfDiscoveryPolicy: read NV err.");

        pstControlManager->stPcscfDiscoveryPolicyInfo.enPcscfDiscoverPolicy
                = IMSA_PCSCF_DISCOVERY_POLICY_PCO;
    }

    return;
}


VOS_VOID IMSA_ReadNvIpv6FallBackExtCause(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU              *pstConnManager;
    TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU stNvFallBackExtCause;
    VOS_UINT32                          ulCauseNum = 0;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulCnt;

    pstConnManager  =  IMSA_CONN_GetConnManagerAddr();

    ulRslt = IMSA_NV_Read(en_NV_Item_IPV6_BACKPROC_EXT_CAUSE,&stNvFallBackExtCause,\
                 sizeof(TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU));

    if ((VOS_OK == ulRslt) && (VOS_TRUE == stNvFallBackExtCause.ulActiveFlag))
    {
        for (ulCnt = 0 ; ulCnt < TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM; ulCnt++)
        {
            if (!TAF_PS_IS_SM_CAUSE_VALID(stNvFallBackExtCause.aucSmCause[ulCnt]))
            {
                continue;
            }
            else
            {
                pstConnManager->stIpv6FallBackExtCause.aenPsCause[ulCauseNum] = TAF_PS_CONVERT_SM_CAUSE_TO_PS_CAUSE(stNvFallBackExtCause.aucSmCause[ulCnt]);
                ulCauseNum++;
            }
        }

        pstConnManager->stIpv6FallBackExtCause.ulCauseNum = ulCauseNum;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_ReadNvIpv6FallBackExtCause: read NV err.");
    }
}


/*****************************************************************************
 Function Name  : IMSA_ReadImsaNV()
 Description    : 读取NV信息，配置信息等
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ReadImsaNV(VOS_VOID)
{
    IMSA_ReadImsaNvImsRatSupportConfig();

    IMSA_ReadImsaNvImsaConfig();

    IMSA_ReadImsaNvImsCapability();

    IMSA_ReadImsaNvSipPortConfig();

    IMSA_ReadImsaNvImsConfig();

    /*读取IMEI*/
    IMSA_ReadImei();

    IMSA_ReadNvVoiceDomain();

    IMSA_ReadPcscfDiscoveryPolicy();

    IMSA_ReadNvIpv6FallBackExtCause();
}

/*****************************************************************************
 Function Name  : IMSA_ReadIsimFile()
 Description    : 读取卡文件信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ReadIsimFile(VOS_VOID)
{
    USIMM_GET_FILE_INFO_STRU         stGetFileInfo ={0};

    /*读取必选文件*/
    stGetFileInfo.enAppType = USIMM_ISIM_APP;
    stGetFileInfo.ucRecordNum  = 1;
    stGetFileInfo.usEfId =IMSA_ISIM_FILE_ID_IMPI;
    (VOS_VOID)USIMM_GetFileReq(PS_PID_IMSA, 0 ,&stGetFileInfo);

    stGetFileInfo.enAppType = USIMM_ISIM_APP;
    stGetFileInfo.ucRecordNum  = 1;
    stGetFileInfo.usEfId =IMSA_ISIM_FILE_ID_DOMAIN;
    (VOS_VOID)USIMM_GetFileReq(PS_PID_IMSA, 0 ,&stGetFileInfo);

    stGetFileInfo.enAppType = USIMM_ISIM_APP;
    stGetFileInfo.ucRecordNum  = 1;
    stGetFileInfo.usEfId =IMSA_ISIM_FILE_ID_IMPU;
    (VOS_VOID)USIMM_GetFileReq(PS_PID_IMSA, 0 ,&stGetFileInfo);

}
VOS_UINT32 IMSA_AllocImsOpId(VOS_VOID)
{
    do
    {
        gulImsaImsOpId++;
    } while(0 == gulImsaImsOpId);

    IMSA_INFO_LOG1("IMSA_AllocImsOpId: ", gulImsaImsOpId);

    return gulImsaImsOpId;
}


VOS_VOID IMSA_ResetImsOpId(VOS_VOID)
{
    gulImsaImsOpId = 0;
}


VOS_UINT32 IMSA_GetImsOpId(VOS_VOID)
{
    return gulImsaImsOpId;
}

/*****************************************************************************
 Function Name  : IMSA_SaveRcvImsOpid()
 Description    : 保存IMS发送消息的OPID，IMSA回复该消息时，需要使用
 Input          : VOS_UINT32 ulImsOpid
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-03  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SaveRcvImsOpid(VOS_UINT32 ulImsOpid)
{
    gulImsaRcvImsOpId = ulImsOpid;
}

/*****************************************************************************
 Function Name  : IMSA_GetRcvImsOpid()
 Description    : IMSA回复IMS消息时，需要使用IMS先前发送消息的OPID
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32 ulImsOpid

 History        :
      1.sunbing 49683      2013-07-03  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_GetRcvImsOpid(VOS_VOID)
{
    return gulImsaRcvImsOpId;
}

/*****************************************************************************
 Function Name  : IMSA_ProcIsimStatusInd()
 Description    : 处理ISIM卡状态指示
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcIsimStatusInd(const VOS_VOID *pRcvMsg)
{
    PS_USIM_STATUS_IND_STRU *pstIsimStatusInd;
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    pstIsimStatusInd = (PS_USIM_STATUS_IND_STRU*)pRcvMsg;

    if(pstIsimStatusInd->enCardType != USIMM_CARD_ISIM)
    {
        /*非ISIM信息*/
        IMSA_WARN_LOG("IMSA_ProcIsimStatusInd: not ISIM!");

        return;
    }

    pstControlManager = IMSA_GetControlManagerAddress();

    /*卡在位可用*/
    if(pstIsimStatusInd->enCardStatus == USIMM_CARD_SERVIC_AVAILABLE)
    {
        /*此前卡状态不在位，且已开机，则读卡信息*/
        if((pstControlManager->enImsaIsimStatus == IMSA_ISIM_STATUS_ABSENT)
            &&(pstControlManager->enImsaStatus != IMSA_STATUS_NULL))
        {
            IMSA_ReadIsimFile();
        }

        /*更新卡状态*/
        pstControlManager->enImsaIsimStatus = IMSA_ISIM_STATUS_AVAILABLE;
    }
    else/*卡不在位*/
    {
        /*更新卡状态，删除卡文件信息*/
        pstControlManager->enImsaIsimStatus = IMSA_ISIM_STATUS_ABSENT;
        pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName = 0;
        pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi   = 0;
        pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu  = 0;

        vos_printf("IMSA_ProcIsimStatusInd: card isnot avail");
    }

}

/*****************************************************************************
 Function Name  : IMSA_ProcIsimRefreshInd()
 Description    : 处理ISIM卡文件刷新指示
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-31  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcIsimRefreshInd(const VOS_VOID *pRcvMsg)
{
    /*暂无文件刷新需求，不实现*/
    (VOS_VOID)pRcvMsg;
}


/*****************************************************************************
 Function Name  : IMSA_SndD2AuthSyncFailure()
 Description    : 处理ISIM卡鉴权同步失败信息
 Input          : pstIMSCnf   卡鉴权结果
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-17  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndD2AuthSyncFailure(const USIMM_IMS_AUTH_CNF_STRU  *pstIMSCnf)
{

    IMSA_IMS_INPUT_EVENT_STRU   *pstHiInputEvent;

    /*分配空间并检验分配是否成功*/
    pstHiInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstHiInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndD2AuthSyncFailure:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstHiInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstHiInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SERVICE;

    pstHiInputEvent->evt.stInputServiceEvent.enInputServeReason = IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SYNC_FAILURE;

    pstHiInputEvent->evt.stInputServiceEvent.ulOpId = IMSA_GetRcvImsOpid();

    pstHiInputEvent->evt.stInputServiceEvent.bitOpAka = IMSA_OP_TRUE;

    /*SYNC错误填写*/
    IMSA_MEM_CPY(pstHiInputEvent->evt.stInputServiceEvent.stAka.aucAuts,
                 &pstIMSCnf->aucAuts[1],
                 pstIMSCnf->aucAuts[0]);

    IMSA_SndImsMsgServiceEvent(pstHiInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstHiInputEvent);

}
VOS_VOID IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo
(
    IMSA_IMS_INPUT_SERVICE_REASON_ENUM_UINT32   enInputServeReason
)
{
    IMSA_IMS_INPUT_EVENT_STRU   *pstHiInputEvent;

    /*分配空间并检验分配是否成功*/
    pstHiInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstHiInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstHiInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstHiInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SERVICE;

    pstHiInputEvent->evt.stInputServiceEvent.enInputServeReason = enInputServeReason;

    pstHiInputEvent->evt.stInputServiceEvent.ulOpId = IMSA_AllocImsOpId();

    IMSA_SndImsMsgServiceEvent(pstHiInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstHiInputEvent);

    return;

}

/*****************************************************************************
 Function Name  : IMSA_SndD2AuthNetworkFailure()
 Description    : 处理ISIM卡鉴权网络失败信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-17  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndD2AuthNetworkFailure(VOS_VOID)
{
    IMSA_IMS_INPUT_EVENT_STRU               *pstHiInputEvent;

    /*分配空间并检验分配是否成功*/
    pstHiInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstHiInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndD2AuthNetworkFailure:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstHiInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstHiInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SERVICE;

    pstHiInputEvent->evt.stInputServiceEvent.enInputServeReason = IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_NETWORK_FAILURE;

    pstHiInputEvent->evt.stInputServiceEvent.ulOpId = IMSA_GetRcvImsOpid();

    IMSA_SndImsMsgServiceEvent(pstHiInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstHiInputEvent);

}

/*****************************************************************************
 Function Name  : IMSA_SndD2AuthSuccRsp()
 Description    : 处理ISIM卡鉴权成功信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-07-01  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndD2AuthSuccRsp(const USIMM_IMS_AUTH_CNF_STRU  *pstIMSCnf)
{
    IMSA_IMS_INPUT_EVENT_STRU               *pstHiInputEvent;

    /*分配空间并检验分配是否成功*/
    pstHiInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstHiInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndD2AuthSuccRsp:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstHiInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    pstHiInputEvent->enEventType = IMSA_IMS_EVENT_TYPE_SERVICE;

    pstHiInputEvent->evt.stInputServiceEvent.enInputServeReason = IMSA_IMS_INPUT_SERVICE_REASON_AKA_RESPONSE_SUCCESS;

    pstHiInputEvent->evt.stInputServiceEvent.ulOpId = IMSA_GetRcvImsOpid();

    pstHiInputEvent->evt.stInputServiceEvent.bitOpAka = 1;

    IMSA_MEM_CPY(pstHiInputEvent->evt.stInputServiceEvent.stAka.aucCk,
                 &pstIMSCnf->aucCK[1],
                 pstIMSCnf->aucCK[0]);

    IMSA_MEM_CPY(pstHiInputEvent->evt.stInputServiceEvent.stAka.aucIk,
                 &pstIMSCnf->aucIK[1],
                 pstIMSCnf->aucIK[0]);

    pstHiInputEvent->evt.stInputServiceEvent.stAka.ulResLength = pstIMSCnf->aucAuthRes[0];

    IMSA_MEM_CPY(pstHiInputEvent->evt.stInputServiceEvent.stAka.aucResponse,
                 &pstIMSCnf->aucAuthRes[1],
                 pstIMSCnf->aucAuthRes[0]);


    IMSA_SndImsMsgServiceEvent(pstHiInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstHiInputEvent);

}

/*****************************************************************************
 Function Name  : IMSA_ProcIsimAuthRsp()
 Description    : 处理ISIM卡鉴权信息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.sunbing 49683      2013-06-24  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_ProcIsimAuthRsp(const VOS_VOID *pRcvMsg)
{
    USIMM_AUTH_CNF_STRU         *pstAuthCnf;

    IMSA_NORM_LOG("IMSA_ProcIsimAuthRsp: ENTER!");

    pstAuthCnf = (USIMM_AUTH_CNF_STRU*)pRcvMsg;

    if(USIMM_ISIM_APP != pstAuthCnf->enAppType )
    {
        IMSA_WARN_LOG("IMSA_ProcIsimAuthRsp: Not IMS auth!");
        return;
    }

    switch(pstAuthCnf->enResult)
    {
    case USIMM_AUTH_IMS_SUCCESS:
        IMSA_SndD2AuthSuccRsp(&pstAuthCnf->cnfdata.stIMSCnf);
        break;

    case USIMM_AUTH_MAC_FAILURE:
        IMSA_SndD2AuthNetworkFailure();
        break;

    case USIMM_AUTH_SYNC_FAILURE:
        IMSA_SndD2AuthSyncFailure(&pstAuthCnf->cnfdata.stIMSCnf);
        break;

    case USIMM_AUTH_IMS_OTHER_FAILURE:
        IMSA_SndD2AuthNetworkFailure();
        break;

    default:/*其他结果，异常处理*/
        IMSA_WARN_LOG("IMSA_ProcIsimAuthRsp: Not IMS auth result!");
        break;
    }


}


VOS_UINT32  IMSA_BcdToAsciiCode(
    VOS_UINT8                           ucBcdCode,
    VOS_CHAR                            *pcAsciiCode
)
{
    VOS_CHAR                            cAsciiCode;

    if (VOS_NULL_PTR == pcAsciiCode)
    {
        IMSA_ERR_LOG("IMSA_BcdToAsciiCode: Parameter of the function is null.");
        return VOS_FALSE;
    }

    if (ucBcdCode <= 0x09)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x30);
    }
    else if (0x0A == ucBcdCode)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x20);    /*字符'*'*/
    }
    else if (0x0B == ucBcdCode)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x18);    /*字符'#'*/
    }
    else if ((0x0C == ucBcdCode)
          || (0x0D == ucBcdCode)
          || (0x0E == ucBcdCode))
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x55);    /*字符'a', 'b', 'c'*/
    }
    else
    {
        IMSA_ERR_LOG("IMSA_BcdToAsciiCode: Parameter of the function is invalid.");
        return VOS_FALSE;
    }

    *pcAsciiCode = cAsciiCode;

    return VOS_TRUE;
}

VOS_UINT32  IMSA_AsciiToBcdCode(
    VOS_CHAR                            cAsciiCode,
    VOS_UINT8                           *pucBcdCode
)
{
    if (VOS_NULL_PTR == pucBcdCode)
    {
        IMSA_ERR_LOG("IMSA_AsciiToBcdCode: Parameter of the function is null.");
        return VOS_FALSE;
    }

    if ((cAsciiCode >= '0')
     && (cAsciiCode <= '9'))
    {
        *pucBcdCode = (VOS_UINT8)(cAsciiCode - '0');
    }
    else if ('*' == cAsciiCode)
    {
        *pucBcdCode = 0x0a;
    }
    else if ('#' == cAsciiCode)
    {
        *pucBcdCode = 0x0b;
    }
    else if (('a' == cAsciiCode)
          || ('b' == cAsciiCode)
          || ('c' == cAsciiCode))
    {
        *pucBcdCode = (VOS_UINT8)((cAsciiCode - 'a') + 0x0c);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_AsciiToBcdCode: Parameter of the function is invalid.");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32  IMSA_AsciiToDtmfKeyEnum
(
    VOS_CHAR                            cAsciiCode,
    IMSA_IMS_DTMF_KEY_ENUM_UINT8       *penDtmfKey
)
{
    VOS_CHAR                            cKey = cAsciiCode;

    /* 转换为小写字符 */
    if ( (cKey >= 'A') && (cKey <= 'D'))
    {
        cKey = cKey + 0x20;
    }

    if ((cKey >= '0') && (cKey <= '9'))
    {
        *penDtmfKey = (IMSA_IMS_DTMF_KEY_ENUM_UINT8)(cKey - '0');
    }
    else if ('*' == cKey)
    {
        *penDtmfKey = IMSA_IMS_DTMF_KEY_STAR;
    }
    else if ('#' == cKey)
    {
        *penDtmfKey = IMSA_IMS_DTMF_KEY_POUND;
    }
    else if (('a' == cKey)
          || ('b' == cKey)
          || ('c' == cKey)
          || ('d' == cKey))
    {
        *penDtmfKey = (IMSA_IMS_DTMF_KEY_ENUM_UINT8)((cKey - 'a') + 0x0c);
    }
    else
    {
        IMSA_ERR_LOG("IMSA_AsciiToDtmfKeyEnum: Parameter of the function is invalid.");
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_UINT32  IMSA_UtilAsciiNumberToBcd(
    const VOS_CHAR                      *pcAsciiNumber,
    VOS_UINT8                           *pucBcdNumber,
    VOS_UINT8                           *pucBcdLen
)
{
    VOS_UINT8                           ucLoop = 0;
    VOS_UINT8                           ucBcdCode;
    VOS_UINT32                          ulRet;

    if ((VOS_NULL_PTR == pcAsciiNumber)
     || (VOS_NULL_PTR == pucBcdNumber)
     || (VOS_NULL_PTR == pucBcdLen))
    {
        IMSA_ERR_LOG("IMSA_UtilAsciiNumberToBcd: Parameter of the function is null.");
        return VOS_FALSE;
    }

    for (ucLoop = 0; pcAsciiNumber[ucLoop] != '\0'; ucLoop++)
    {
        ulRet = IMSA_AsciiToBcdCode(pcAsciiNumber[ucLoop], &ucBcdCode);
        if (VOS_TRUE != ulRet)
        {
            return ulRet;
        }

        /*将当前需要填入的空间清0*/
        pucBcdNumber[(ucLoop / 2)] &= ((ucLoop % 2) == 1) ? 0x0F : 0xF0;

        /*将数字填入相应的空间*/
        pucBcdNumber[(ucLoop / 2)] |= (((ucLoop % 2) == 1) ? ((ucBcdCode << 4) & 0xF0) : (ucBcdCode & 0x0F));
    }

    /*如果长度为奇数，则最后一个字符需要填 F */
    if (1 == (ucLoop % 2))
    {
        pucBcdNumber[(ucLoop / 2)] |= 0xF0;
    }

    *pucBcdLen = (ucLoop + 1) / 2;

    return VOS_TRUE;
}

VOS_UINT32  IMSA_UtilBcdNumberToAscii(
    const VOS_UINT8                     *pucBcdNumber,
    VOS_UINT8                           ucBcdLen,
    VOS_CHAR                            *pcAsciiNumber
)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucLen;
    VOS_UINT8                           ucBcdCode;
    VOS_UINT32                          ulRet;

    if ((VOS_NULL_PTR == pucBcdNumber)
     || (VOS_NULL_PTR == pcAsciiNumber))
    {
        IMSA_ERR_LOG("IMSA_BcdNumberToAscii: Parameter of the function is null.");
        return VOS_FALSE;
    }

    /*整理号码字符串，去除无效的0XFF数据*/
    while (ucBcdLen > 1)
    {
        if (0xFF == pucBcdNumber[ucBcdLen - 1])
        {
            ucBcdLen--;
        }
        else
        {
            break;
        }
    }

    /*判断pucBcdAddress所指向的字符串的最后一个字节的高位是否为1111，
    如果是，说明号码位数为奇数，否则为偶数*/
    if ((pucBcdNumber[ucBcdLen - 1] & 0xF0) == 0xF0)
    {
        ucLen = (VOS_UINT8)((ucBcdLen * 2) - 1);
    }
    else
    {
        ucLen = (VOS_UINT8)(ucBcdLen * 2);
    }

    /*解析号码*/
    for (ucLoop = 0; ucLoop < ucLen; ucLoop++)
    {
        /*判断当前解码的是奇数位号码还是偶数位号码，从0开始，是偶数*/
        if (1 == (ucLoop % 2))
        {
            /*如果是奇数位号码，则取高4位的值*/
            ucBcdCode = ((pucBcdNumber[(ucLoop / 2)] >> 4) & 0x0F);
        }
        else
        {
            /*如果是偶数位号码，则取低4位的值*/
            ucBcdCode = (pucBcdNumber[(ucLoop / 2)] & 0x0F);
        }

        /*将二进制数字转换成Ascii码形式*/
        ulRet = IMSA_BcdToAsciiCode(ucBcdCode, &(pcAsciiNumber[ucLoop]));
        if (VOS_TRUE != ulRet)
        {
            return ulRet;
        }
    }

    pcAsciiNumber[ucLoop] = '\0';      /*字符串末尾为0*/

    return VOS_TRUE;
}



VOS_VOID IMSA_UtilStrNCpy(VOS_CHAR *pDst, const VOS_CHAR *pSrc, VOS_UINT32 ulLen)
{
    if (pDst && pSrc)
    {
        (VOS_VOID)VOS_StrNCpy(pDst, (VOS_CHAR *)pSrc, ulLen);
        /*pDst[ulLen - 1] = 0;*/
    }
}
VOS_INT32  IMSA_PrintImsaImsOutputService
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulOutputServiceReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMS_IMSA_OUTPUT_SERVICE_MSG_STRU   *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMS_IMSA_OUTPUT_SERVICE_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    ulOutputServiceReasonNum = sizeof(g_astOutputServiceReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulOutputServiceReasonNum; ulLoop++)
    {
        if (g_astOutputServiceReasonArray[ulLoop].ulId == pstRcvMsg->stOutputService.enOutputServiceReason)
        {
            break;
        }
    }

    if ( ulLoop < ulOutputServiceReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astOutputServiceReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsOutputService, Invalid OutputService Reason: ",
                              pstRcvMsg->stOutputService.enOutputServiceReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMS_IMSA_OUTPUT_SERVICE_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsOutputCall
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulOutputCallReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMS_IMSA_OUTPUT_CALL_MSG_STRU      *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMS_IMSA_OUTPUT_CALL_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    ulOutputCallReasonNum = sizeof(g_astOutputCallReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulOutputCallReasonNum; ulLoop++)
    {
        if (g_astOutputCallReasonArray[ulLoop].ulId == pstRcvMsg->stOutputCall.enOutputCallReason)
        {
            break;
        }
    }

    if ( ulLoop < ulOutputCallReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astOutputCallReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsOutputCall, Invalid OutputCall Reason: ",
                              pstRcvMsg->stOutputCall.enOutputCallReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMS_IMSA_OUTPUT_CALL_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsOutputSms
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulOutputSmsReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMS_IMSA_OUTPUT_SMS_MSG_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMS_IMSA_OUTPUT_SMS_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    ulOutputSmsReasonNum = sizeof(g_astOutputSmsReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulOutputSmsReasonNum; ulLoop++)
    {
        if (g_astOutputSmsReasonArray[ulLoop].ulId == pstRcvMsg->stOutputSms.enOutputSmsReason)
        {
            break;
        }
    }

    if ( ulLoop < ulOutputSmsReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astOutputSmsReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsOutputSms, Invalid OutputSms Reason: ",
                              pstRcvMsg->stOutputSms.enOutputSmsReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMS_IMSA_OUTPUT_SMS_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsOutputSystem
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulOutputSystemReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMS_IMSA_OUTPUT_SYSTEM_MSG_STRU    *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMS_IMSA_OUTPUT_SYSTEM_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    ulOutputSystemReasonNum = sizeof(g_astOutputSystemReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulOutputSystemReasonNum; ulLoop++)
    {
        if (g_astOutputSystemReasonArray[ulLoop].ulId == pstRcvMsg->stOutputSystem.enOutputSysReason)
        {
            break;
        }
    }

    if ( ulLoop < ulOutputSystemReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astOutputSystemReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsOutputSystem, Invalid OutputSystem Reason: ",
                              pstRcvMsg->stOutputSystem.enOutputSysReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMS_IMSA_OUTPUT_SYSTEM_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsOutputPara
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulOutputParaReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMS_IMSA_OUTPUT_PARA_MSG_STRU      *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMS_IMSA_OUTPUT_PARA_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    ulOutputParaReasonNum = sizeof(g_astOutputParaReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulOutputParaReasonNum; ulLoop++)
    {
        if (g_astOutputParaReasonArray[ulLoop].ulId == pstRcvMsg->stOutputPara.enOutputParaReason)
        {
            break;
        }
    }

    if ( ulLoop < ulOutputParaReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astOutputParaReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsOutputSystem, Invalid OutputPara Reason: ",
                              pstRcvMsg->stOutputPara.enOutputParaReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMS_IMSA_OUTPUT_PARA_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintCallImsaSrvccStatusNotify
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    CALL_IMSA_SRVCC_STATUS_NOTIFY_STRU *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_PrintCallImsaSrvccStatusNotify****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印消息的每个数据*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enSrvccStatus = %d \r\n",
                       pstRcvMsg->enSrvccStatus,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_PrintCallImsaSrvccStatusNotify****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}
VOS_INT32  IMSA_PrintImsaImsInputSystem
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulInputRadioReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_IMS_INPUT_SYSTEM_MSG_STRU     *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_IMS_INPUT_SYSTEM_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulInputRadioReasonNum = sizeof(g_astInputSystemReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulInputRadioReasonNum; ulLoop++)
    {
        if (g_astInputSystemReasonArray[ulLoop].ulId == pstRcvMsg->stInputSystem.enInputSysReason)
        {
            break;
        }
    }

    if ( ulLoop < ulInputRadioReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astInputSystemReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsInputRadio, Invalid InputSystem Reason: ",
                              pstRcvMsg->stInputSystem.enInputSysReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_IMS_INPUT_SYSTEM_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsInputPara
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulInputRadioReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_IMS_INPUT_PARA_MSG_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_IMS_INPUT_PARA_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulInputRadioReasonNum = sizeof(g_astInputParaReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulInputRadioReasonNum; ulLoop++)
    {
        if (g_astInputParaReasonArray[ulLoop].ulId == pstRcvMsg->stInputPara.enInputParaReason)
        {
            break;
        }
    }

    if ( ulLoop < ulInputRadioReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astInputParaReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsInputRadio, Invalid InputPara Reason: ",
                              pstRcvMsg->stInputPara.enInputParaReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_IMS_INPUT_PARA_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  IMSA_PrintImsaImsInputService
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulInputServiceReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_IMS_INPUT_SERVICE_MSG_STRU    *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_IMS_INPUT_SERVICE_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulInputServiceReasonNum = sizeof(g_astInputServiceReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulInputServiceReasonNum; ulLoop++)
    {
        if (g_astInputServiceReasonArray[ulLoop].ulId == pstRcvMsg->stInputService.enInputServeReason)
        {
            break;
        }
    }

    if ( ulLoop < ulInputServiceReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astInputServiceReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsInputService, Invalid InputService Reason: ",
                              pstRcvMsg->stInputService.enInputServeReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_IMS_INPUT_SERVICE_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32  IMSA_PrintImsaImsInputSms
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulInputSmsReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_IMS_INPUT_SMS_MSG_STRU        *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_IMS_INPUT_SMS_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulInputSmsReasonNum = sizeof(g_astInputSmsReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulInputSmsReasonNum; ulLoop++)
    {
        if (g_astInputSmsReasonArray[ulLoop].ulId == pstRcvMsg->stInputSms.enInputSmsReason)
        {
            break;
        }
    }

    if ( ulLoop < ulInputSmsReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astInputSmsReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsInputSms, Invalid InputSms Reason: ",
                              pstRcvMsg->stInputSms.enInputSmsReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_IMS_INPUT_SMS_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32  IMSA_PrintImsaImsInputCall
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulInputCallReasonNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_IMS_INPUT_CALL_MSG_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_IMS_INPUT_CALL_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulInputCallReasonNum = sizeof(g_astInputCallReasonArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulInputCallReasonNum; ulLoop++)
    {
        if (g_astInputCallReasonArray[ulLoop].ulId == pstRcvMsg->stInputCall.enInputCallReason)
        {
            break;
        }
    }

    if ( ulLoop < ulInputCallReasonNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astInputCallReasonArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaImsInputCall, Invalid InputCall Reason: ",
                              pstRcvMsg->stInputCall.enInputCallReason);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_IMS_INPUT_CALL_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32  IMSA_PrintImsaSpmCallMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulCallMsgEvtNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    IMSA_SPM_CALL_MSG_STRU             *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :IMSA_SPM_CALL_MSG****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulCallMsgEvtNum = sizeof(g_astCallMsgEvtArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulCallMsgEvtNum; ulLoop++)
    {
        if (g_astCallMsgEvtArray[ulLoop].ulId == pstRcvMsg->enEventType)
        {
            break;
        }
    }

    if ( ulLoop < ulCallMsgEvtNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astCallMsgEvtArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaSpmCallMsg, Invalid enEventType: ",
                              pstRcvMsg->enEventType);
    }

    /* 打印消息的每个数据*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "CallId = %d \r\n",
                       pstRcvMsg->stCallInfo.callId,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印消息的每个数据*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "ClientId = %d \r\n",
                       pstRcvMsg->stCallInfo.clientId,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印消息的每个数据*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "OpId = %d \r\n",
                       pstRcvMsg->stCallInfo.opId,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :IMSA_SPM_CALL_MSG****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}
VOS_INT32  IMSA_PrintSpmImsaCallSupsCmdReq
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulCallSupsCmdNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    SPM_IMSA_CALL_SUPS_CMD_REQ_STRU    *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* 打印消息结构标题 */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :SPM_IMSA_CALL_SUPS_CMD_REQ****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    ulCallSupsCmdNum = sizeof(g_astCallSupsCmdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulCallSupsCmdNum; ulLoop++)
    {
        if (g_astCallSupsCmdArray[ulLoop].ulId == pstRcvMsg->stCallMgmtCmd.enCallSupsCmd)
        {
            break;
        }
    }

    if ( ulLoop < ulCallSupsCmdNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astCallSupsCmdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintSpmImsaCallSupsCmdReq, Invalid enCallSupsCmd: ",
                              pstRcvMsg->stCallMgmtCmd.enCallSupsCmd);
    }

    /* 打印消息的结束标题*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :SPM_IMSA_CALL_SUPS_CMD_REQ****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32   IMSA_PrintImsaIntraMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    IMSA_INTRA_MSG_ID_ENUM_UINT32       enMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaIntraMsgNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaIntraMsgNum = sizeof(g_astImsaIntraMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaIntraMsgNum; ulLoop++)
    {
        if (g_astImsaIntraMsgIdArray[ulLoop].ulId == enMsgId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaIntraMsgNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaIntraMsgIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaIntraMsg, Invalid enMsgId: ",
                              enMsgId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32   IMSA_PrintUsimMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaUsimMsgNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaUsimMsgNum = sizeof(g_astImsaUsimMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaUsimMsgNum; ulLoop++)
    {
        if (g_astImsaUsimMsgIdArray[ulLoop].ulId == ulMsgId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaUsimMsgNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaUsimMsgIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintUsimMsg, Invalid enMsgId: ",
                              ulMsgId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32   IMSA_PrintVcMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaVcMsgNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaVcMsgNum = sizeof(g_astImsaVcMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaVcMsgNum; ulLoop++)
    {
        if (g_astImsaVcMsgIdArray[ulLoop].ulId == ulMsgId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaVcMsgNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaVcMsgIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintVcMsg, Invalid enMsgId: ",
                              ulMsgId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32   IMSA_PrintCdsMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaCdsMsgNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaCdsMsgNum = sizeof(g_astImsaCdsMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaCdsMsgNum; ulLoop++)
    {
        if (g_astImsaCdsMsgIdArray[ulLoop].ulId == ulMsgId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaCdsMsgNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaCdsMsgIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintVcMsg, Invalid enMsgId: ",
                              ulMsgId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32  IMSA_PrintImsaTimer
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulTimerType,
    VOS_UINT32                          ulTimerPara
)

{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaTimerNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulImsaTimerNum = sizeof(g_astImsaTimerArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaTimerNum; ulLoop++)
    {
        if (g_astImsaTimerArray[ulLoop].ulId == ulTimerType)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulImsaTimerNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaTimerArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        /* 打印消息的每个数据*/
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " ulPara = %d \r\n",
                           ulTimerPara,
                          &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsaTimer, Invalid Timer Type: ",ulTimerType);
    }

    return (usTotalLen - usOffset);

}
VOS_INT32   IMSA_PrintMmaMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulMsgId
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaMmaMsgNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaMmaMsgNum = sizeof(g_astImsaMmaMsgIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaMmaMsgNum; ulLoop++)
    {
        if (g_astImsaMmaMsgIdArray[ulLoop].ulId == ulMsgId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaMmaMsgNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaMmaMsgIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintMmaMsg, Invalid enMsgId: ",
                              ulMsgId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32   IMSA_PrintApsMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const TAF_PS_EVT_STRU              *pstApsEvt
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsaApsEvtNum;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* delay begin */
    PS_LOG_RETURN_SUCC();
    /* delay end */

    ulImsaApsEvtNum = sizeof(g_astImsaApsEvtIdArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    for (ulLoop = 0; ulLoop < ulImsaApsEvtNum; ulLoop++)
    {
        if (g_astImsaApsEvtIdArray[ulLoop].ulId == pstApsEvt->ulEvtId)
        {
            break;
        }
    }

    if ( ulLoop < ulImsaApsEvtNum )
    {
        NAS_COMM_nsprintf(
                    pcBuff,
                    usTotalLen,
                    (VOS_CHAR *)(g_astImsaApsEvtIdArray[ulLoop].aucPrintString),
                    &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                               usTotalLen,
                               "[ TICK : %ld ]",
                               PS_GET_TICK(),
                              &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintApsMsg, Invalid enMsgId: ",
                              pstApsEvt->ulEvtId);
    }

    return (usTotalLen - usOffset);
}


VOS_INT32  IMSA_PrintSpmMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulSpmImsaMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulSpmImsaMsgNum = sizeof(g_astImsaSpmMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulSpmImsaMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astImsaSpmMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulSpmImsaMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaSpmMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astImsaSpmMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astImsaSpmMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintSpmMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}
VOS_INT32  IMSA_PrintAtMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulAtImsaMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulAtImsaMsgNum = sizeof(g_astImsaAtMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulAtImsaMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astImsaAtMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulAtImsaMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaAtMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astImsaAtMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astImsaAtMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintAtMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}


/*****************************************************************************
 Function Name   : IMSA_PrintMsgMsg
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.sunbing 49683      2013-10-15  Draft Enact

*****************************************************************************/
VOS_INT32  IMSA_PrintMsgMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulSpmImsaMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulSpmImsaMsgNum = sizeof(g_astImsaMsgMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulSpmImsaMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astImsaMsgMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulSpmImsaMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaMsgMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astImsaMsgMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astImsaMsgMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintSpmMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}

/*****************************************************************************
 Function Name   : IMSA_PrintCallMsg
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.sunbing 49683      2013-10-15  Draft Enact

*****************************************************************************/
VOS_INT32  IMSA_PrintCallMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulSpmImsaMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulSpmImsaMsgNum = sizeof(g_astImsaCallMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulSpmImsaMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astImsaCallMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulSpmImsaMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaCallMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astImsaCallMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astImsaCallMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintSpmMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}
VOS_INT32  IMSA_PrintImsMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)

{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulImsImsaMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* 获得消息表的长度 */
    ulImsImsaMsgNum = sizeof(g_astImsaImsMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* 查找对应的消息 */
    for (ulLoop = 0; ulLoop< ulImsImsaMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astImsaImsMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* 打印对应的消息 */
    if ( ulLoop < ulImsImsaMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astImsaImsMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astImsaImsMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astImsaImsMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        IMSA_WARN_LOG1("IMSA_PrintImsMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}

/*****************************************************************************
 Function Name   : IMSA_PrintRcvTafMsg
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.sunbing 49683      2013-10-15  Draft Enact

*****************************************************************************/
VOS_INT32  IMSA_PrintRcvTafMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    if (MN_CALLBACK_PS_CALL == pstMsg->ulMsgName)
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "APS-->IMSA\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;
        /*lint -e826*/
        ilOutPutLen = IMSA_PrintApsMsg( pcBuff,
                                        usTotalLen,
                                        (const TAF_PS_EVT_STRU*)pstMsg);
        /*lint +e826*/
    }

    if((pstMsg->ulMsgName >= ID_SPM_IMSA_CALL_ORIG_REQ)
     && (pstMsg->ulMsgName < ID_SPM_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "SPM-->IMSA\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintSpmMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }

    if((pstMsg->ulMsgName >= ID_MSG_IMSA_RP_DATA_REQ)
     && (pstMsg->ulMsgName < ID_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "MSG-->IMSA\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintMsgMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }



    if((pstMsg->ulMsgName >= ID_CALL_IMSA_SRVCC_STATUS_NOTIFY)
        &&(pstMsg->ulMsgName <= ID_CALL_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "CALL-->IMSA\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintCallMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset);

}



VOS_VOID IMSA_PrintImsaRevMsg
(
    const PS_MSG_HEADER_STRU           *pstMsg,
    VOS_CHAR                           *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "IMSA Receive Message:",
                     &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        IMSA_WARN_LOG("IMSA_PrintImsaRevMsg, Print receive msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印各个PID的消息 */
    switch ( pstMsg->ulSenderPid )
    {
        case WUEPS_PID_TAF:
            ilOutPutLen = IMSA_PrintRcvTafMsg( pcBuff,
                                                usTotalLen,
                                                pstMsg);
            break;

        case WUEPS_PID_MMA:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "MMA-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintMmaMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case PS_PID_IMSA:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMSA-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintImsaIntraMsg(pcBuff,
                                              usTotalLen,
                                              pstMsg->ulMsgName);
            break;

        case PS_PID_IMSVA  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMS-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintImsMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg);
            break;

        case WUEPS_PID_AT  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "AT-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintAtMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg);
            break;

        case WUEPS_PID_USIM:
        case MAPS_PIH_PID:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "USIM-->IMSA\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintUsimMsg(pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case WUEPS_PID_VC:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "VC-->IMSA\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintVcMsg(  pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case UEPS_PID_CDS:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "CDS-->IMSA\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintCdsMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case VOS_PID_TIMER  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TIMER-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintImsaTimer(pcBuff,
                                            usTotalLen,
                                            ((REL_TIMER_MSG *)(VOS_VOID *)pstMsg)->ulName,
                                            ((REL_TIMER_MSG *)(VOS_VOID *)pstMsg)->ulPara);
            break;

        default:
            IMSA_WARN_LOG2("IMSA_PrintImsaRevMsg,Invalid Pid, MsgId: ",
                          pstMsg->ulSenderPid,
                          pstMsg->ulMsgName);

            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        IMSA_WARN_LOG2("IMSA_PrintImsaRevMsg, print return zero length.SenderPid, MsgId: ",
                      pstMsg->ulSenderPid,
                      pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff, usTotalLen);

    return ;

}

/*****************************************************************************
 Function Name   : IMSA_PrintSendTafMsg
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.sunbing 49683      2013-10-15  Draft Enact

*****************************************************************************/
VOS_INT32  IMSA_PrintSendTafMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    if (MN_CALLBACK_PS_CALL == pstMsg->ulMsgName)
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "IMSA-->APS\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        /*lint -e826*/
        ilOutPutLen = IMSA_PrintApsMsg( pcBuff,
                                        usTotalLen,
                                        (TAF_PS_EVT_STRU*)pstMsg);
        /*lint +e826*/
    }

    if((pstMsg->ulMsgName >= ID_SPM_IMSA_CALL_ORIG_REQ)
     && (pstMsg->ulMsgName < ID_SPM_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "IMSA-->SPM\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintSpmMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }

    if((pstMsg->ulMsgName >= ID_MSG_IMSA_RP_DATA_REQ)
     && (pstMsg->ulMsgName < ID_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "IMSA-->MSG\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintMsgMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }

    if((pstMsg->ulMsgName >= ID_CALL_IMSA_SRVCC_STATUS_NOTIFY)
        &&(pstMsg->ulMsgName <= ID_CALL_IMSA_MSG_TYPE_BUTT))
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "IMSA-->CALL\t",
                        &ilOutPutLenHead);

        usTotalLen += (VOS_UINT16)ilOutPutLenHead;

        ilOutPutLen = IMSA_PrintCallMsg( pcBuff,
                                        usTotalLen,
                                        pstMsg);
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset);

}


/*****************************************************************************
 Function Name   : IMSA_PrintImsaSendMsg
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.lihong00150010    2013-08-08  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_PrintImsaSendMsg
(
    const PS_MSG_HEADER_STRU               *pstMsg,
    VOS_CHAR                               *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    /* delay begin */
    PS_LOG_RETURN();
    /* delay end */

    /* 打印标题*/
    NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         "IMSA Send Message:",
                         &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        IMSA_WARN_LOG("IMSA_PrintImsaSendMsg, Print send msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* 打印各个PID的消息 */
    switch ( pstMsg->ulReceiverPid )
    {
        case WUEPS_PID_TAF:
            ilOutPutLen = IMSA_PrintSendTafMsg(pcBuff,
                                               usTotalLen,
                                               pstMsg);
            break;

        case WUEPS_PID_MMA:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMSA-->MMA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintMmaMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case PS_PID_IMSA:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMSA-->IMSA\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintImsaIntraMsg(pcBuff,
                                              usTotalLen,
                                              pstMsg->ulMsgName);
            break;

        case PS_PID_IMSVA  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMSA-->IMS\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintImsMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg);
            break;

        case WUEPS_PID_AT  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "IMSA-->AT\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintAtMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg);
            break;

        case WUEPS_PID_VC:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "IMSA-->VC\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintVcMsg(  pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        case UEPS_PID_CDS:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "IMSA-->CDS\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = IMSA_PrintCdsMsg( pcBuff,
                                            usTotalLen,
                                            pstMsg->ulMsgName);
            break;

        default:
            IMSA_WARN_LOG2("IMSA_PrintImsaSendMsg,Invalid Pid, MsgId: ",
                          pstMsg->ulReceiverPid,
                          pstMsg->ulMsgName);

            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        IMSA_WARN_LOG2("IMSA_PrintImsaSendMsg, print return zero length. ReceiverPid, MsgId: ",
                              pstMsg->ulReceiverPid,
                              pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff,usTotalLen);


    /*打印当前安全上下文状态和空口消息原始码流*/
    IMSA_NORM_LOG("=============OUT MSG CONTENT is :====================");
    NAS_COMM_PrintArray(             IMSA_GET_IMSA_PRINT_BUF(),
                                     (VOS_UINT8 *)pstMsg,
                                     (pstMsg->ulLength + VOS_MSG_HEAD_LENGTH));
    IMSA_NORM_LOG("====================================================");

    return ;
}
IMSA_IMS_NW_ACCESS_TYPE_ENUM_UINT8 IMSA_ConverterAccessType2Ims
(
    MMA_IMSA_ACCESS_TYPE_ENUM_UINT8     enAccessType
)
{
    IMSA_IMS_NW_ACCESS_TYPE_ENUM_UINT8  enImsAccessType = IMSA_IMS_NW_ACCESS_TYPE_BUTT;

    switch (enAccessType)
    {
        case MMA_IMSA_ACCESS_TYPE_GERAN:
            enImsAccessType = IMSA_IMS_NW_ACCESS_TYPE_3GPP_GERAN;
            break;
        case MMA_IMSA_ACCESS_TYPE_UTRAN_TDD:
            enImsAccessType = IMSA_IMS_NW_ACCESS_TYPE_3GPP_UTRAN_TDD;
            break;
        case MMA_IMSA_ACCESS_TYPE_UTRAN_FDD:
            enImsAccessType = MMA_IMSA_ACCESS_TYPE_UTRAN_FDD;
            break;
        case MMA_IMSA_ACCESS_TYPE_EUTRAN_TDD:
            enImsAccessType = IMSA_IMS_NW_ACCESS_TYPE_3GPP_EUTRAN_TDD;
            break;
        case MMA_IMSA_ACCESS_TYPE_EUTRAN_FDD:
            enImsAccessType = IMSA_IMS_NW_ACCESS_TYPE_3GPP_EUTRAN_FDD;
            break;
        default:
            IMSA_WARN_LOG("IMSA_ConverterAccessType2Ims:illegal type");
            break;
    }

    return enImsAccessType;
}


VOS_CHAR IMSA_ConverterDigit2Chacter
(
    VOS_UINT8                           ucDigit
)
{
    return (VOS_CHAR)(ucDigit + 0x30);
}


VOS_VOID IMSA_ConverterCgiParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo   = VOS_NULL_PTR;
    VOS_UINT8                           ucDigit     = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_ConverterCgiParam2Ims is entered!");

    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();
    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_CGI;

    pstNwInfo   = IMSA_GetNetInfoAddress();
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.enNwAccessType
                = IMSA_ConverterAccessType2Ims(pstNwInfo->enAccessType);

    /* 取MCC第一DIGIT */
    ucDigit     = (pstNwInfo->stPlmnId.ulMcc & 0xff);
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMcc[0]
                = IMSA_ConverterDigit2Chacter(ucDigit);

    /* 取MCC第二DIGIT */
    ucDigit     = (pstNwInfo->stPlmnId.ulMcc & 0xff00) >> IMSA_MOVEMENT_8;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMcc[1]
                = IMSA_ConverterDigit2Chacter(ucDigit);

    /* 取MCC第三DIGIT */
    ucDigit     = (pstNwInfo->stPlmnId.ulMcc & 0xff0000) >> IMSA_MOVEMENT_16;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMcc[2]
                = IMSA_ConverterDigit2Chacter(ucDigit);

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMcc[3] = '\0';

    /* 取MNC第一DIGIT */
    ucDigit     = (pstNwInfo->stPlmnId.ulMnc & 0xff);
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMnc[0]
                = IMSA_ConverterDigit2Chacter(ucDigit);

    /* 取MNC第二DIGIT */
    ucDigit     = (pstNwInfo->stPlmnId.ulMnc & 0xff00) >> IMSA_MOVEMENT_8;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMnc[1]
                = IMSA_ConverterDigit2Chacter(ucDigit);

    /* 取MNC第三DIGIT */
    if ((pstNwInfo->stPlmnId.ulMnc & 0xff0000) != 0x0f0000)
    {
        ucDigit = (pstNwInfo->stPlmnId.ulMnc & 0xff0000) >> IMSA_MOVEMENT_16;
        pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMnc[2]
                = IMSA_ConverterDigit2Chacter(ucDigit);

        pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMnc[3] = '\0';
    }
    else
    {
        pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.cMnc[2] = '\0';
    }

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.usLac
                = pstNwInfo->usLac;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.usTac
                = pstNwInfo->usTac;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stCgi.ulCellId
                = pstNwInfo->ulCellId;
}


VOS_VOID IMSA_ConverterImeiParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMSA_ENTITY_STRU                   *pstImsaEntity = IMSA_CtxGet();

    IMSA_INFO_LOG("IMSA_RegConverterImeiParam2Ims is entered!");

    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();
    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_IMEI;

    IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputParaEvent.u.cImei,
                    pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stImsaUeId.acImei,
                    IMSA_IMS_IMEI_LEN + 1);
}

VOS_UINT32 IMSA_ConverterSipInfo2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMS_PARM_SIP_STRU     stNvImsConfig;
    VOS_UINT32                      ulRslt = VOS_ERR;

    IMSA_INFO_LOG("IMSA_ConverterSipInfo2Ims is entered!");

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_SIP_CONFIG,&stNvImsConfig,\
                 sizeof(IMS_PARM_SIP_STRU));


    if(ulRslt == VOS_OK)
    {
        pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_NV_INFO;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulOpId = IMSA_AllocImsOpId();

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.enInputNvInfoReason = IMSA_IMS_INPUT_NV_INFO_REASON_SIP;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulNvInfoLen = sizeof(IMS_PARM_SIP_STRU);

        /*lint -e419*/
        IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputNvInfoEvent.aucNvInfo,
                        &stNvImsConfig,
                        sizeof(IMS_PARM_SIP_STRU));
        /*lint +e419*/
    }
    return ulRslt;

}


VOS_UINT32 IMSA_ConverterVoipInfo2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMS_PARM_VOIP_STRU    stNvImsVoipConfig;
    VOS_UINT32                      ulRslt = VOS_ERR;

    IMSA_INFO_LOG("IMSA_ConverterVoipInfo2Ims is entered!");

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_VOIP_CONFIG,&stNvImsVoipConfig,\
                 sizeof(IMS_PARM_VOIP_STRU));


    if (ulRslt == VOS_OK)
    {
        pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_NV_INFO;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulOpId = IMSA_AllocImsOpId();

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.enInputNvInfoReason = IMSA_IMS_INPUT_NV_INFO_REASON_VOIP;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulNvInfoLen = sizeof(IMS_PARM_VOIP_STRU);

        /*lint -e419*/
        IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputNvInfoEvent.aucNvInfo,
                        &stNvImsVoipConfig,
                        sizeof(IMS_PARM_VOIP_STRU));
        /*lint +e419*/
    }
    return ulRslt;

}

VOS_UINT32 IMSA_ConverterCodeInfo2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMS_PARM_VOIP_CODE_STRU    stNvImsCodeConfig;
    VOS_UINT32                      ulRslt = VOS_ERR;

    IMSA_INFO_LOG("IMSA_ConverterCodeInfo2Ims is entered!");

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_CODE_CONFIG,&stNvImsCodeConfig,\
                 sizeof(IMS_PARM_VOIP_CODE_STRU));

    if(ulRslt == VOS_OK)
    {
        pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_NV_INFO;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulOpId = IMSA_AllocImsOpId();

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.enInputNvInfoReason = IMSA_IMS_INPUT_NV_INFO_REASON_CODE;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulNvInfoLen = sizeof(IMS_PARM_VOIP_CODE_STRU);

        /*lint -e419*/
        IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputNvInfoEvent.aucNvInfo,
                        &stNvImsCodeConfig,
                        sizeof(IMS_PARM_VOIP_CODE_STRU));
        /*lint +e419*/
    }

    return ulRslt;
}

VOS_UINT32 IMSA_ConverterSsConfInfo2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMS_PARM_SS_CONF_STRU stNvImsSsConfConfig;
    VOS_UINT32                      ulRslt = VOS_ERR;

    IMSA_INFO_LOG("IMSA_ConverterSsConfInfo2Ims is entered!");

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_SS_CONF_CONFIG,&stNvImsSsConfConfig,\
                 sizeof(IMS_PARM_SS_CONF_STRU));

    if(ulRslt == VOS_OK)
    {
        pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_NV_INFO;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulOpId = IMSA_AllocImsOpId();

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.enInputNvInfoReason = IMSA_IMS_INPUT_NV_INFO_REASON_SS_CONF;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulNvInfoLen = sizeof(IMS_PARM_SS_CONF_STRU);

        /*lint -e419*/
        IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputNvInfoEvent.aucNvInfo,
                        &stNvImsSsConfConfig,
                        sizeof(IMS_PARM_SS_CONF_STRU));
        /*lint +e419*/

    }

    return ulRslt;
}

VOS_UINT32 IMSA_ConverterSecurityInfo2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMS_PARM_SECURITY_STRU        stNvImsSecurityConfig;
    VOS_UINT32                      ulRslt = VOS_ERR;

    IMSA_INFO_LOG("IMSA_ConverterSecurityInfo2Ims is entered!");

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_SECURITY_CONFIG,&stNvImsSecurityConfig,\
                 sizeof(IMS_PARM_SECURITY_STRU));

    if(ulRslt == VOS_OK)
    {
        pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_NV_INFO;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulOpId = IMSA_AllocImsOpId();

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.enInputNvInfoReason = IMSA_IMS_INPUT_NV_INFO_REASON_SECURITY;

        pstImsaImsInputEvt->evt.stInputNvInfoEvent.ulNvInfoLen = sizeof(IMS_PARM_SECURITY_STRU);

        /*lint -e419*/
        IMSA_MEM_CPY(   pstImsaImsInputEvt->evt.stInputNvInfoEvent.aucNvInfo,
                        &stNvImsSecurityConfig,
                        sizeof(IMS_PARM_SECURITY_STRU));
        /*lint +e419*/
    }

    return ulRslt;
}
/*lint +e516*/
/*lint +e718*/
/*lint +e732*/

VOS_VOID IMSA_ConverterRetyrTimeParam2Ims
(
    VOS_UINT32                          ulRetryTimerLen,
    VOS_UINT32                          ulPeriodRergisterTimerLen,
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvt
)
{
    IMSA_INFO_LOG("IMSA_RegConverterRetyrTimeParam2Ims is entered!");

    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();
    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_TIMER_LENGTH;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stTimerLength.ulRetryTimerLength
                = ulRetryTimerLen;
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stTimerLength.ulPeriodRergisterTimerLength
                = ulPeriodRergisterTimerLen;
}



VOS_VOID IMSA_ConfigCgi2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_ConfigCgi2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_ConfigCgi2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterCgiParam2Ims(pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigImei2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConfigImei2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigImei2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterImeiParam2Ims(pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigSipInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU           *pstImsaImsInputEvent = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = VOS_ERR;
    VOS_UINT32                          ulInputEventLen = 0;

    IMSA_INFO_LOG("IMSA_RegConfigImei2Ims is entered!");

    ulInputEventLen = sizeof(IMSA_IMS_INPUT_EVENT_STRU) + sizeof(IMS_PARM_SIP_STRU)-IMSA_NV_INFO_LEN ;

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(ulInputEventLen);

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigImei2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    /*lint --e{669}*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, ulInputEventLen);

    ulRslt = IMSA_ConverterSipInfo2Ims(pstImsaImsInputEvent);

    /* 只有读取NV成功，才给IMS 配置 */
    if (VOS_OK == ulRslt)
    {
        IMSA_SndImsMsgNvInfoEvent(pstImsaImsInputEvent,sizeof(IMS_PARM_SIP_STRU));
    }

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}

VOS_VOID IMSA_ConfigVoipInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU           *pstImsaImsInputEvent = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = VOS_ERR;

    VOS_UINT32                          ulInputEventLen = 0;

    IMSA_INFO_LOG("IMSA_ConfigVoipInfo2Ims is entered!");

    ulInputEventLen = sizeof(IMSA_IMS_INPUT_EVENT_STRU) + sizeof(IMS_PARM_VOIP_STRU)-IMSA_NV_INFO_LEN;

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(ulInputEventLen);

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_ConfigVoipInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/

    /*lint --e{669}*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, ulInputEventLen);

    ulRslt = IMSA_ConverterVoipInfo2Ims(pstImsaImsInputEvent);

    /* 只有读取NV成功，才给IMS 配置 */
    if (VOS_OK == ulRslt)
    {
        IMSA_SndImsMsgNvInfoEvent(pstImsaImsInputEvent,sizeof(IMS_PARM_VOIP_STRU));
    }

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigCodeInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU           *pstImsaImsInputEvent = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = VOS_ERR;
    VOS_UINT32                          ulInputEventLen = 0;

    IMSA_INFO_LOG("IMSA_ConfigCodeInfo2Ims is entered!");

    ulInputEventLen = sizeof(IMSA_IMS_INPUT_EVENT_STRU) + sizeof(IMS_PARM_VOIP_CODE_STRU)-IMSA_NV_INFO_LEN ;

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(ulInputEventLen);

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_ConfigCodeInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    /*lint --e{669}*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, ulInputEventLen);

    ulRslt = IMSA_ConverterCodeInfo2Ims(pstImsaImsInputEvent);

    /* 只有读取NV成功，才给IMS 配置 */
    if (VOS_OK == ulRslt)
    {
        IMSA_SndImsMsgNvInfoEvent(pstImsaImsInputEvent,sizeof(IMS_PARM_VOIP_CODE_STRU));
    }

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}

VOS_VOID IMSA_ConfigSsConfInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU           *pstImsaImsInputEvent = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = VOS_ERR;
    VOS_UINT32                          ulInputEventLen = 0;

    IMSA_INFO_LOG("IMSA_ConfigSsConfInfo2Ims is entered!");


    ulInputEventLen = sizeof(IMSA_IMS_INPUT_EVENT_STRU) + sizeof(IMS_PARM_SS_CONF_STRU)-IMSA_NV_INFO_LEN ;

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(ulInputEventLen);

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_ConfigSsConfInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    /*lint --e{669}*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, ulInputEventLen);

    ulRslt = IMSA_ConverterSsConfInfo2Ims(pstImsaImsInputEvent);

    /* 只有读取NV成功，才给IMS 配置 */
    if (VOS_OK == ulRslt)
    {
        IMSA_SndImsMsgNvInfoEvent(pstImsaImsInputEvent,sizeof(IMS_PARM_SS_CONF_STRU));
    }

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}

VOS_VOID IMSA_ConfigSecurityInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU           *pstImsaImsInputEvent = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = VOS_ERR;
    VOS_UINT32                          ulInputEventLen = 0;

    IMSA_INFO_LOG("IMSA_ConfigSecurityInfo2Ims is entered!");


    ulInputEventLen = sizeof(IMSA_IMS_INPUT_EVENT_STRU) + sizeof(IMS_PARM_SECURITY_STRU)-IMSA_NV_INFO_LEN ;

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = (IMSA_IMS_INPUT_EVENT_STRU *)IMSA_MEM_ALLOC(ulInputEventLen);

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_ConfigSecurityInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    /*lint --e{669}*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, ulInputEventLen);

    ulRslt = IMSA_ConverterSecurityInfo2Ims(pstImsaImsInputEvent);

    /* 只有读取NV成功，才给IMS 配置 */
    if (VOS_OK == ulRslt)
    {
        IMSA_SndImsMsgNvInfoEvent(pstImsaImsInputEvent,sizeof(IMS_PARM_SECURITY_STRU));
    }

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigTimerLength2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConfigRetryTime2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigRetryTime2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterRetyrTimeParam2Ims(0, 0, pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigSipPort2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_ConfigSipPort2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigRetryTime2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterSipPort2Ims(pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}

VOS_VOID IMSA_ConverterSipPort2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU          *pstImsaImsInputEvt
)
{
    IMSA_ENTITY_STRU *pstImsaEntity = IMSA_CtxGet();

    IMSA_INFO_LOG("IMSA_RegConverterRetyrTimeParam2Ims is entered!");

    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();
    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_PORTS;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stPortInfo.usMinPort
                = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMinPort;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stPortInfo.usMaxPort
                = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMaxPort;
}



VOS_VOID IMSA_ConverterUeCapParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMSA_ENTITY_STRU *pstImsaEntity = IMSA_CtxGet();

    IMSA_INFO_LOG("IMSA_RegConverterUeCapParam2Ims is entered!");

    /* 设置输入事件类型 */
    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    /* 设置输入Reason和opid */
    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();

    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_UE_CAPABILITIES;

    /* 填充UeCapability各域 */
    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucVoiceCall =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucVoiceCallOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucVideoCall =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucVideoCallOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSms =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSmsOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSrvcc =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSrvccOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSrvccMidCall =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSrvccMidCallOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSrvccAlerting =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSrvccAlertingOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSrvccPreAlerting =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSrvccPreAlertingOnImsSupportFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucSrvccTiFlag =
        pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucSrvccTiFlag;

    pstImsaImsInputEvt->evt.stInputParaEvent.u.stUeCapability.ucCallWaiting = VOS_TRUE;
}
VOS_VOID IMSA_ConverterNetCapParam2Ims
(
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConverterNetCapParam2Ims is entered!");

    pstNwInfo                               = IMSA_GetNetInfoAddress();

    /* 设置输入事件类型 */
    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    /* 设置输入Reason和opid */
    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();

    pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_NETWORK_CAPABILITIES;

    /* 填充Net Capability各域 */
    if (IMSA_IMS_VOPS_STATUS_SUPPORT == pstNwInfo->enImsaImsVoPsStatus)
    {
        pstImsaImsInputEvt->evt.stInputParaEvent.u.stNetworkCapability.ucVoice = VOS_TRUE;
    }
    else
    {
        pstImsaImsInputEvt->evt.stInputParaEvent.u.stNetworkCapability.ucVoice = VOS_FALSE;
    }
}
VOS_VOID IMSA_ConfigUeCapabilityInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConfigUeCapabilityInfo2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigUeCapabilityInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterUeCapParam2Ims(pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConfigNetCapInfo2Ims( VOS_VOID )
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConfigNetCapInfo2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigNetCapInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterNetCapParam2Ims(pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_ConverterImsiParam2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_IMS_INPUT_EVENT_STRU                   *pstImsaImsInputEvt
)
{
    IMSA_ENTITY_STRU *pstImsaEntity = IMSA_CtxGet();

    IMSA_INFO_LOG("IMSA_RegConverterImsiParam2Ims is entered!");

    /* 设置输入事件类型 */
    pstImsaImsInputEvt->enEventType = IMSA_IMS_EVENT_TYPE_PARA;

    /* 设置输入Reason和opid */
    pstImsaImsInputEvt->evt.stInputParaEvent.ulOpId = IMSA_AllocImsOpId();
    if (IMSA_REG_TYPE_NORMAL == enRegType)
    {
        pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_NORMAL_ACCOUNT;

        /* 填充Account各域 */
        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.acImpi,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stImsaUeId.stImpi.acImpi, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.acImpu,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stImsaUeId.stTImpu.acTImpu, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.acDomain,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stHomeNetDomainName.acHomeNetDomainName, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.acPassword,
            pstImsaEntity->stImsaControlManager.stImsaConfigPara.aucPassWord, IMSA_IMS_EVENT_STRING_SZ);

        if (IMSA_AUTH_TYPE_AKA == pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucAuthType)
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_AKA;
        }
        else if (IMSA_AUTH_TYPE_DIGIST == pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucAuthType)
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_DIGIST;
        }
        else
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stNormalAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_AKA_IPSEC;
        }
    }
    else
    {
        pstImsaImsInputEvt->evt.stInputParaEvent.enInputParaReason = IMAS_IMS_INPUT_PARA_REASON_SET_EMC_ACCOUNT;

        /* 填充Account各域 */
        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.acImpi,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stImsaUeId.stImpi.acImpi, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.acImpu,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stImsaUeId.stTImpu.acTImpu, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.acDomain,
            pstImsaEntity->stImsaControlManager.stImsaCommonInfo.stHomeNetDomainName.acHomeNetDomainName, IMSA_IMS_EVENT_STRING_SZ);

        IMSA_UtilStrNCpy(pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.acPassword,
            pstImsaEntity->stImsaControlManager.stImsaConfigPara.aucPassWord, IMSA_IMS_EVENT_STRING_SZ);

        if (IMSA_AUTH_TYPE_AKA == pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucAuthType)
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_AKA;
        }
        else if (IMSA_AUTH_TYPE_DIGIST == pstImsaEntity->stImsaControlManager.stImsaConfigPara.ucAuthType)
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_DIGIST;
        }
        else
        {
            pstImsaImsInputEvt->evt.stInputParaEvent.u.stEmcAccount.enAuthType
                                    = IMSA_IMS_AUTH_TYPE_AKA_IPSEC;
        }
    }
}


VOS_VOID IMSA_ConfigAccoutInfo2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType
)
{
    IMSA_IMS_INPUT_EVENT_STRU                    *pstImsaImsInputEvent = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_RegConfigAccoutInfo2Ims is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsaImsInputEvent = IMSA_MEM_ALLOC(sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    if ( VOS_NULL_PTR == pstImsaImsInputEvent )
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_RegConfigAccoutInfo2Ims:ERROR:Alloc Mem fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET(pstImsaImsInputEvent, 0, \
                 sizeof(IMSA_IMS_INPUT_EVENT_STRU));

    IMSA_ConverterImsiParam2Ims(enRegType, pstImsaImsInputEvent);

    IMSA_SndImsMsgParaEvent(pstImsaImsInputEvent);

    /*释放消息空间*/
    IMSA_MEM_FREE(pstImsaImsInputEvent);
}


VOS_VOID IMSA_Send_Intra_Msg(VOS_VOID* pIntraMsg)
{
    IMSA_PrintImsaSendMsg((const PS_MSG_HEADER_STRU *)pIntraMsg,
                              IMSA_GET_IMSA_PRINT_BUF());
    if(VOS_OK !=  IMSA_SndIntraMsg((pIntraMsg)))
    {
        IMSA_ERR_LOG("IMSA_SEND_INTRA_MSG  ERR!!");
    }
}

VOS_VOID IMSA_Send_Msg(VOS_VOID* pMsg)
{
    IMSA_PrintImsaSendMsg((const PS_MSG_HEADER_STRU *)pMsg,IMSA_GET_IMSA_PRINT_BUF());
    if(VOS_OK != PS_SND_MSG_ALL_CHECK(PS_PID_IMSA , pMsg))
    {
        IMSA_ERR_LOG("IMSA_SND_MSG:Msg Snd Err!");
    }
}



VOS_UINT32 IMSA_IsImsVoiceContidionSatisfied( VOS_VOID )
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG2("IMSA_IsImsVoiceContidionSatisfied:NV IMS = ,IMS voice = ",
                pstControlMagnaer->stImsaConfigPara.ucLteImsSupportFlag,
                pstControlMagnaer->stImsaConfigPara.ucVoiceCallOnImsSupportFlag);

    IMSA_INFO_LOG2("IMSA_IsImsVoiceContidionSatisfied:voice domain = ,nw IMS voice = ",
                pstControlMagnaer->stImsaConfigPara.enVoiceDomain,
                IMSA_GetNwImsVoiceCap());

    IMSA_INFO_LOG1("IMSA_IsImsVoiceContidionSatisfied:IMSA REG STATE = ",
                IMSA_SRV_GetNormalSrvStatus());

    if ((VOS_TRUE == pstControlMagnaer->stImsaConfigPara.ucLteImsSupportFlag)
        && (VOS_TRUE == pstControlMagnaer->stImsaConfigPara.ucVoiceCallOnImsSupportFlag)
        && (IMSA_VOICE_DOMAIN_CS_ONLY != pstControlMagnaer->stImsaConfigPara.enVoiceDomain)
        && (IMSA_IMS_VOPS_STATUS_SUPPORT == IMSA_GetNwImsVoiceCap())
        && (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_IsRegParaAvailable
(
    IMSA_CONN_TYPE_ENUM_UINT32      enConnType,
    VOS_CHAR                        *pacUeAddr,
    VOS_CHAR                        *pacPcscfAddr
)
{
    IMSA_REG_ENTITY_STRU               *pstRegEntity = VOS_NULL_PTR;
    IMSA_REG_ADDR_PAIR_STRU            *pstIpAddr;

    IMSA_INFO_LOG("IMSA_IsRegParaAvailable is entered!");

    pstRegEntity = IMSA_RegEntityGetByType(enConnType);

    pstIpAddr = pstRegEntity->stPairMgrCtx.pstIpv6List;

    /* 遍历所有IPV6地址对，查看是否备份的注册信息是否有效 */
    while (VOS_NULL_PTR != pstIpAddr)
    {
        if ((0 ==VOS_StrCmp(pacUeAddr, pstIpAddr->acUeAddr))
            && (0 ==VOS_StrCmp(pacPcscfAddr, pstIpAddr->acPcscfAddr)))
        {
            return IMSA_TRUE;
        }
        pstIpAddr = pstIpAddr->pstNext;
    }

    pstIpAddr = pstRegEntity->stPairMgrCtx.pstIpv4List;

    /* 遍历所有IPV4地址对，查看是否备份的注册信息是否有效 */
    while (VOS_NULL_PTR != pstIpAddr)
    {
        if ((0 ==VOS_StrCmp(pacUeAddr, pstIpAddr->acUeAddr))
            && (0 ==VOS_StrCmp(pacPcscfAddr, pstIpAddr->acPcscfAddr)))
        {
            return IMSA_TRUE;
        }
        pstIpAddr = pstIpAddr->pstNext;
    }

    return IMSA_FALSE;

}


VOS_UINT32 IMSA_SetCurrentPara
(
    IMSA_CONN_TYPE_ENUM_UINT32      enConnType,
    VOS_CHAR                        *pacUeAddr,
    VOS_CHAR                        *pacPcscfAddr
)
{
    IMSA_REG_ENTITY_STRU               *pstRegEntity        = IMSA_RegEntityGetByType(enConnType);
    IMSA_REG_ADDR_PAIR_STRU            *pstIpAddr;

    IMSA_INFO_LOG("IMSA_SetCurrentPara is entered!");

    pstIpAddr = pstRegEntity->stPairMgrCtx.pstIpv6List;

    /* 遍历所有IPV6地址对，查看是否备份的注册信息是否有效 */
    while (VOS_NULL_PTR != pstIpAddr)
    {
        if ((0 ==VOS_StrCmp(pacUeAddr, pstIpAddr->acUeAddr))
            && (0 ==VOS_StrCmp(pacPcscfAddr, pstIpAddr->acPcscfAddr)))
        {
            pstRegEntity->stPairMgrCtx.pstCurrent = pstIpAddr;
            return IMSA_TRUE;
        }
        pstIpAddr = pstIpAddr->pstNext;
    }

    pstIpAddr = pstRegEntity->stPairMgrCtx.pstIpv4List;

    /* 遍历所有IPV4地址对，查看是否备份的注册信息是否有效 */
    while (VOS_NULL_PTR != pstIpAddr)
    {
        if ((0 ==VOS_StrCmp(pacUeAddr, pstIpAddr->acUeAddr))
            && (0 ==VOS_StrCmp(pacPcscfAddr, pstIpAddr->acPcscfAddr)))
        {
            pstRegEntity->stPairMgrCtx.pstCurrent = pstIpAddr;
            return IMSA_TRUE;
        }
        pstIpAddr = pstIpAddr->pstNext;
    }

    return IMSA_FALSE;
}



/*lint +e961*/
/*lint +e960*/


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaPublic.c */



