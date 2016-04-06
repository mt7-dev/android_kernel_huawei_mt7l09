
#include "UsimmApi.h"
#include "usimmbase.h"
#include "usimmapdu.h"
#include "usimmdl.h"
#include "usimmt1dl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/*消息队列记数器*/
VOS_UINT32                          gulUSIMMAPIMessageNum = 0;

/*互斥操作*/
VOS_SEM                             gulUSIMMApiSmId       = VOS_NULL_LONG;

/*当前的快速开机状态*/
VOS_UINT32                          gulUSIMMOpenSpeed     = 0;

/*当前卡的基本信息*/
USIMM_BASE_INFO_ST                  gstUSIMMBaseInfo;

/* ISIM卡信息 */
USIMM_CARD_APP_INFO_ST              gastUSIMMCardAppInfo[USIMM_UICC_BUTT];

/* 标记USIM是否支持IMS */
VOS_UINT32                          gulUsimSupportIMS;

USIMM_ISIM_USIM_CONVERSION_ST       gastIsimUsimEFConTBL[USIMM_ISIMEFTOUSIMEF_TBL_SIZE] =
                                               {{EFISIMAD,      EFAD},
                                               {EFISIMGBABP,    EFGBABP},
                                               {EFISIMGBANL,    EFGBANL},
                                               {EFISIMNAFKCA,   EFNAFKCA},
                                               {EFISIMUICCIARI, EFUICCIARI}};

/*底软相关的基本信息*/
USIMM_DRV_INFO_ST                   gstUSIMMDrvInfo;

/*当前访问的文件信息，如果需要支持逻辑通道则定义为数组即可*/
USIMM_CURFILEINFO_ST                gstUSIMMCurFileInfo;

/*当前ADF信息*/
USIMM_ADF_INFO_STRU                 gstUSIMMADFInfo;

/*AID信息*/
USIMM_AID_INFO_STRU                 g_astAidInfo[USIMM_AID_TYPE_BUTT];

/*当前的APDU命令的内容*/
USIMM_APDU_ST                       gstUSIMMAPDU = {0};

/*当前链路层的状态*/
USIMMDL_CONTROL_ST                  gstUSIMMDLControl;

/*文件缓存定义*/
USIMM_POOL_ST                       gstUSIMMPOOL;

/*保存Log信息的控制变量*/
USIMM_LOG_DATA_STRU                 g_stUSIMMLogData;

VOS_UINT32                          g_ulUSIMMExcLogLenBK;

USIMM_LOG_DATA_STRU                 g_stUSIMMExcLogData;

/* 保存逻辑通道信息 */
USIMM_CHANNEL_INFO_STRU             g_astUSIMMChCtrl[USIMM_CHANNEL_NUM_MAX];

/* 保存逻辑通道上GetResponse信息 */
USIMM_CHANNEL_GET_RSP_STRU          g_astUSIMMChGetRsp[USIMM_CHANNEL_NUM_MAX];

/* 保存逻辑通道上AID FCP信息 */
USIMM_CHANNEL_AIDFCPINFO_STRU       g_astUSIMMChAIDFCP[USIMM_CHANNEL_NUM_MAX];

USIMM_CARD_INIT_INFO_LOG_STRU       g_stUSIMMInitInfo;

/* 获取卡状态时所起的callback定时器所使用的TIMER ID */
HTIMER                              g_ulUSIMMGetCardStatTimerID = VOS_NULL_PTR;

/* 获取卡状态时所起的callback定时器所使用的信号量 */
VOS_SEM                             g_ulUSIMMGetCardStatSemID;

/* 指示支持T=1还是T=0 */
USIMM_DL_T_MODE_ENUM_UINT32         g_enUSIMMTMode = USIMM_DL_T_MODE_T0;

/* T=1相关参数备份，用于卡复位场景后恢复相关的T=1上下文 */
USIMM_DL_T1_PARA_STRU               g_stUSIMMT1Para;

/* 指示是否发送了IFS REQ */
VOS_BOOL                            g_bSendIFSReqFlag = VOS_FALSE;

#if(FEATURE_ON == FEATURE_PTM)
USIMM_MNTN_STATE_ERROR_EVENT_STRU   g_stCardErrorLog;

NV_ID_ERR_LOG_CTRL_INFO_STRU        g_stErrLogCtrlInfo;
#endif

/* TaskDelay时所起的callback定时器所使用的TIMER ID */
HTIMER                              g_ulUSIMMTaskDelayTimerID = VOS_NULL_PTR;

/* TaskDelay时所起的callback定时器所使用的信号量 */
VOS_SEM                             g_ulUSIMMTaskDelaySemID;

USIMM_CARD_VERSIONTYPE_STRU         g_stUSIMMCardVersionType;

#if (FEATURE_VSIM == FEATURE_ON)
VOS_UINT8                           *g_pucUSIMMVsimData = VOS_NULL_PTR;
#endif


USIMM_FEATURE_CFG_STRU              g_stUsimmFeatureCfg;

VOS_UINT32                          g_ulATTSpecErrSWCnt;    /* ATT特殊处理的SW计数 */

/*初始化第一步时候读取的不受PIN保护的文件ID*/
VOS_UINT16 gausInitFileOne[USIMM_INITFIRST_FILENUM]   = {0x2FE2,0x2F05};

/*SIM卡需要初始化读取缓存的Mandatory文件ID*/
VOS_UINT16 gausSimInitMFile[USIMM_INITSIM_MFILENUM]   = {0x6F07,0x6FAD,0x6F78,0x6F31,0x6F7E,0x6F20,0x6F74,0x6F7B};

/*USIM卡需要初始化读取缓存的Mandatory文件ID*/
VOS_UINT16 gausUsimInitMFile[USIMM_INITUSIM_MFILENUM] = {0x6F07,0x6FC4,0x6FAD,0x6F31,0x6F7E,0x6F78,0x6F73,0x6F08,0x6F09,0x6F7B,0x6F5B,0x6F5C};

/* 如果有新增加的OP文件读取，需要在下面的表里增加对应的服务。如果不确定或者在6F38找不到对应的服务的就写USIM_SVR_NONE */
/*SIM卡需要初始化读取缓存的Optional文件ID*/
USIMM_OPFILE_LIST_STRU gastSimInitOFile[USIMM_INITSIM_OFILENUM] = {{0x6F15, (VOS_UINT16)UICC_SVR_NONE},
                                                                   {0x6F52, (VOS_UINT16)SIM_SVR_GPRS},
                                                                   {0x6F53, (VOS_UINT16)SIM_SVR_GPRS},
                                                                   {0x6F60, (VOS_UINT16)SIM_SVR_USER_CTRL_PLMN},
                                                                   {0x6F61, (VOS_UINT16)SIM_SVR_OPERATOR_CTRL_PLMN},
                                                                   {0x6F62, (VOS_UINT16)SIM_SVR_HPLMN},
                                                                   {0x6F45, (VOS_UINT16)SIM_SVR_CBMID},
                                                                   {0x6F48, (VOS_UINT16)SIM_SVR_DD_VIA_SMS_CB},
                                                                   {0x6F50, (VOS_UINT16)SIM_SVR_CBMIR},
                                                                   {0x6F46, (VOS_UINT16)SIM_SVR_SERVICE_PRO_NAME},
                                                                   {0x6F3E, (VOS_UINT16)SIM_SVR_GIDL1},
                                                                   {0x6F30, (VOS_UINT16)SIM_SVR_PLMN_SELECTOR}};
/*USIM卡需要初始化读取缓存的Optional文件ID*/
USIMM_OPFILE_LIST_STRU gastUsimInitOFile[USIMM_INITUSIM_OFILENUM] = {{0x6F15, (VOS_UINT16)UICC_SVR_NONE},
                                                                     {0x6F60, (VOS_UINT16)USIM_SVR_UPLMN_SEL_WACT},
                                                                     {0x6F61, (VOS_UINT16)USIM_SVR_OPLMN_SEL_WACT},
                                                                     {0x6F62, (VOS_UINT16)USIM_SVR_HPLMN_WACT},
                                                                     {0x6F46, (VOS_UINT16)USIM_SVR_SPN},
                                                                     {0x6F3E, (VOS_UINT16)USIM_SVR_GID1},
                                                                     {0x6FD9, (VOS_UINT16)USIM_SVR_EHPLMN},
                                                                     {0x6FDB, (VOS_UINT16)USIM_SVR_EHPLMN_IND},
                                                                     {0x6FDC, (VOS_UINT16)USIM_SVR_RPLMN_LACT},
                                                                     {0x6F56, (VOS_UINT16)UICC_SVR_NONE},
                                                                     {0x6F45, (VOS_UINT16)USIM_SVR_CBMI},
                                                                     {0x6F48, (VOS_UINT16)USIM_SVR_DATA_DL_SMSCB},
                                                                     {0x6F50, (VOS_UINT16)USIM_SVR_CBMIR},
                                                                     {0x4F20, (VOS_UINT16)USIM_SVR_GSM_ACCESS_IN_USIM},
                                                                     {0x4F52, (VOS_UINT16)USIM_SVR_GSM_ACCESS_IN_USIM}};
/* ATT USIM卡需要初始化的文件列表 */
VOS_UINT16 g_ausAttUsimInitFile[USIMM_USIM_ATT_INITFILE_NUM]={EFACTINGHPLMN, EFRATMODE};

/* ATT SIM卡需要初始化的文件列表 */
VOS_UINT16 g_ausAttSimInitFile [USIMM_SIM_ATT_INITFILE_NUM]={EFACTINGHPLMN};

VOS_UINT16 gausSimDFTELFile[15] = {0x6F3A,0x6F3B,0x6F3C,0x6F3D,0x6F40,0x6F42,0x6F43,0x6F44,0x6F47,0x6F49,0x6F4A,0x6F4B,0x6F4C,0x6F4D,0x6F4E};

VOS_UINT16 gausSimDFGSMFile[46] = {0x6F05,0x6F07,0x6F11,0x6F12,0x6F13,0x6F14,0x6F15,0x6F16,0x6F17,0x6F18,0x6F19,0x6F20,0x6F30,0x6F31,0x6F37,0x6F38,0x6F39,0x6F3E,0x6F3F,0x6F41,0x6F45,0x6F46,0x6F48,0x6F74,0x6F78,0x6F7B,0x6F7E,0x6FAD,0x6FAE,0x6FB1,0x6FB2,0x6FB3,0x6FB4,0x6FB5,0x6FB6,0x6FB7,0x6F50,0x6F51,0x6F52,0x6F53,0x6F54,0x6F60,0x6F61,0x6F62,0x6F63,0x6F64};

VOS_UINT16 gausUsimDFGSMFile[39] = {0x6F05,0x6F07,0x6F20,0x6F30,0x6F31,0x6F37,0x6F38,0x6F39,0x6F3E,0x6F3F,0x6F41,0x6F45,0x6F46,0x6F48,0x6F74,0x6F78,0x6F7B,0x6F7E,0x6FAD,0x6FAE,0x6FB1,0x6FB2,0x6FB3,0x6FB4,0x6FB5,0x6FB6,0x6FB7,0x6F50,0x6F51,0x6F52,0x6F53,0x6F54,0x6F60,0x6F61,0x6F62,0x6F63,0x6F64,0x6FC5,0x6FC6};

VOS_UINT16 gausUsimDFTELFile[17] = {0x6F06,0x6F3A,0x6F3B,0x6F3C,0x6F40,0x6F42,0x6F43,0x6F44,0x6F47,0x6F49,0x6F4A,0x6F4B,0x6F4C,0x6F4D,0x6F4E,0x6F4F,0x6F54};

VOS_UINT16 gausUsimADFFile[88] = {0x6F05,0x6F06,0x6F07,0x6F08,0x6F09,0x6F15,0x6F2C,0x6F31,0x6F32,0x6F37,0x6F38,0x6F39,0x6F3B,0x6F3C,0x6F3E,0x6F3F,0x6F40,0x6F41,0x6F42,0x6F43,0x6F45,0x6F46,0x6F47,0x6F48,0x6F49,0x6F4B,0x6F4C,0x6F4D,0x6F4E,0x6F50,0x6F55,0x6F56,0x6F57,0x6F58,0x6F5B,0x6F5C,0x6F60,0x6F61,0x6F62,0x6FD9,
                            0x6F73,0x6F78,0x6F7B,0x6F7E,0x6F80,0x6F81,0x6F82,0x6F83,0x6FAD,0x6FB5,0x6FB6,0x6FB7,0x6FC3,0x6FC4,0x6FC5,0x6FC6,0x6FC7,0x6FC8,0x6FC9,0x6FCA,0x6FCB,0x6FCC,0x6FCD,0x6FCE,0x6FCF,0x6FD0,0x6FD1,0x6FD2,0x6FD3,0x6F4F,0x6FB1,0x6FB2,0x6FB3,0x6FB4,0x6FD4,0x6FD5,0x6FD6,0x6FD7,0x6FD8,0x6FDA,
                            0x6FDB,0x6FDC,0x6FDD,0x6FDE,0x6FDF,0x6FE2,0x6FE3,0x6FE4};


VOS_UINT16 gausSimDFGRAPHFile[1] = {0x4F20};

VOS_UINT16 gausSimDFSoLSAFile[2] = {0x4F30,0x4F31};

VOS_UINT16 gausSimDFMExEFile[4] = {0x4F40,0x4F41,0x4F42,0x4F43};

VOS_UINT16 gausUsimDFMMFile[2] = {0x4F47,0x4F48};

VOS_UINT16 gausUsimDFGSMAFile[4] = {0x4F20,0x4F52,0x4F63,0x4F64};

VOS_UINT16 gausUsimDFWLANFile[6] = {0x4F41,0x4F42,0x4F43,0x4F44,0x4F45,0x4F46};

USIMM_CARD_FILEMAP_ST   g_stUSIMMCardFileMap[10];

/*SIM卡的DF和子DF关系*/
USIMM_FILE_CONTENT_ST g_stSimDFGSMContent[USIMM_SIMDFGSMGDF_MAX] =
       {{DFMExE,     ARRAYSIZE(gausSimDFMExEFile),   gausSimDFMExEFile},
        {DFSoLSA,    ARRAYSIZE(gausSimDFSoLSAFile),  gausSimDFSoLSAFile}};

USIMM_FILE_CONTENT_ST g_stSimDFTELContent[USIMM_SIMDFTELGDF_MAX] =
       {{DFGRAPHICS, ARRAYSIZE(gausSimDFGRAPHFile),  gausSimDFGRAPHFile}};

USIMM_DF_CONTENT_ST g_stSimDFContent[USIMM_SIMDFCONTENT_MAX] =
       {{{DFGSM,      ARRAYSIZE(gausSimDFGSMFile),    gausSimDFGSMFile},   USIMM_SIMDFGSMGDF_MAX,  g_stSimDFGSMContent},
        {{DFTELCOM,   ARRAYSIZE(gausSimDFTELFile),    gausSimDFTELFile},   USIMM_SIMDFTELGDF_MAX,  g_stSimDFTELContent}};

/*Usim卡的DF和子DF的关系*/
USIMM_FILE_CONTENT_ST g_astUsimADFContent[USIMM_USIMADFGDF_MAX] =
       {{DFGSMACCESS,ARRAYSIZE(gausUsimDFGSMAFile),  gausUsimDFGSMAFile},
        {DFMExE,     ARRAYSIZE(gausSimDFMExEFile),   gausSimDFMExEFile},
        {DFWLAN,     ARRAYSIZE(gausUsimDFWLANFile),  gausUsimDFWLANFile},
        {DFSoLSA,    ARRAYSIZE(gausSimDFSoLSAFile),  gausSimDFSoLSAFile}};

USIMM_FILE_CONTENT_ST g_astUsimDFTELContent[USIMM_USIMADFTELGDF_MAX] =
       {{DFUGRAPHICS,ARRAYSIZE(gausSimDFGRAPHFile),  gausSimDFGRAPHFile},
        {DFMULTIMEDIA,ARRAYSIZE(gausUsimDFMMFile),   gausUsimDFMMFile}};

USIMM_DF_CONTENT_ST g_stUsimDFContent[USIMM_USIMDFCONTENT_MAX] =
       {{{ADF,        ARRAYSIZE(gausUsimADFFile),     gausUsimADFFile},    USIMM_USIMADFGDF_MAX,   g_astUsimADFContent},
        {{DFTELCOM,   ARRAYSIZE(gausUsimDFTELFile),   gausUsimDFTELFile},  USIMM_USIMADFTELGDF_MAX,g_astUsimDFTELContent},
        {{DFGSM,      ARRAYSIZE(gausUsimDFGSMFile),   gausUsimDFGSMFile},  0,                      0}};

/*SIM INS字节检查*/
/* 0x90, 0x00 */
VOS_UINT8 g_aucSim9000Ins[] = { CMD_INS_STATUS,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_READ_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_SLEEP,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_GET_RESPONSE,
                                CMD_INS_END};

/* 0x91, 0xFF */
VOS_UINT8 g_aucSim91XXIns[] = { CMD_INS_STATUS,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_GET_RESPONSE,
                                CMD_INS_END};

/* 0x93, 0x00 */
VOS_UINT8 g_aucSim9300Ins[] = { CMD_INS_ENVELOPE,
                                CMD_INS_END};

/* 0x9E, 0xFF */
VOS_UINT8 g_aucSim9EXXIns[] = { CMD_INS_ENVELOPE,
                                CMD_INS_END};

/* 0x9F, 0xFF */
VOS_UINT8 g_aucSim9FFFIns[] = { CMD_INS_SELECT,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_END};

/* 0x92, 0x40 */
VOS_UINT8 g_aucSim9240Ins[] = { CMD_INS_SELECT,
                                CMD_INS_STATUS,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_GET_RESPONSE,
                                CMD_INS_END};

/* 0x92, 0xFF */
VOS_UINT8 g_aucSim92FFIns[] = { CMD_INS_UPDATE_BINARY,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_END};

/* 0x94, 0x00 */
VOS_UINT8 g_aucSim9400Ins[] = { CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_END};

/* 0x94, 0x02 */
VOS_UINT8 g_aucSim9402Ins[] = { CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_END};

/* 0x94, 0x04 */
VOS_UINT8 g_aucSim9404Ins[] = { CMD_INS_SELECT,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_END};

/* 0x94, 0x08 */
VOS_UINT8 g_aucSim9408Ins[] = { CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_END};

/* 0x98, 0x02 */
VOS_UINT8 g_aucSim9802Ins[] = { CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_END};

/* 0x98, 0x04 */
VOS_UINT8 g_aucSim9804Ins[] = { CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_END};

/* 0x98, 0x08 */
VOS_UINT8 g_aucSim9808Ins[] = { CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_END};

/* 0x98, 0x10 */
VOS_UINT8 g_aucSim9810Ins[] = { CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_END};

/* 0x98, 0x40 */
VOS_UINT8 g_aucSim9840Ins[] = { CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_END};

/* 0x98, 0x50 */
VOS_UINT8 g_aucSim9850Ins[] = { CMD_INS_INCREASE,
                                CMD_INS_END};
/* 0x67, 0x00    0x67, 0xFF    0x6B, 0xFF    0x6E, 0xFF    0x6F, 0x00    0x6F, 0xFF */
VOS_UINT8 g_aucSimSwAnyIns[] = {CMD_INS_ANY};

/*USIM INS字节检查*/
/*0x90,0x00  0x61,0xFF  0x91,0xFF  0x62,0xFF  0x63,0xFF  0x64,0x00  0x67,0x00  0x67,0xFF  0x68,0x00  0x68,0x81  0x68,0x82  0x6A,0x81  0x6A,0x86  0x6B,0x00  0x6C,0xFF  0x6E,0x00  0x6F,0x00  0x6F,0xFF*/
VOS_UINT8 g_aucUsimSwAnyIns[] = {CMD_INS_ANY};

    /* 0x93, 0x00 */
VOS_UINT8 g_aucUsim9300Ins[] = {CMD_INS_ENVELOPE,
                                CMD_INS_END};

    /* 0x98, 0x50 */
VOS_UINT8 g_aucUsim9850Ins[] = {CMD_INS_INCREASE,
                                CMD_INS_END};

    /* 0x98, 0x62 */
VOS_UINT8 g_aucUsim9862Ins[] = {CMD_INS_AUTHENTICATE,
                                CMD_INS_END};

    /* 0x62, 0x00 */
VOS_UINT8 g_aucUsim6200Ins[] = {CMD_INS_SELECT,
                                CMD_INS_STATUS,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_GET_CHALLENGE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_TERMINAL_CAPABILITY,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x62, 0x81 */
VOS_UINT8 g_aucUsim6281Ins[] = {CMD_INS_READ_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_END};

    /* 0x62, 0x82 */
VOS_UINT8 g_aucUsim6282Ins[] = {CMD_INS_READ_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_END};

    /* 0x62, 0x83 */
VOS_UINT8 g_aucUsim6283Ins[] = {CMD_INS_SELECT,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_END};

    /* 0x62, 0x85 */
VOS_UINT8 g_aucUsim9285Ins[] = {CMD_INS_SELECT,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_END};

    /* 0x62, 0xF1 */
VOS_UINT8 g_aucUsim62F1Ins[] = {CMD_INS_AUTHENTICATE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x62, 0xF2 */
VOS_UINT8 g_aucUsim62F2Ins[] = {CMD_INS_RETRIEVE_DATA,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x62, 0xF3 */
VOS_UINT8 g_aucUsim62F3Ins[] = {CMD_INS_AUTHENTICATE,
                                CMD_INS_END};

    /* 0x63, 0xC0 */
VOS_UINT8 g_aucUsim63CxIns[] = {CMD_INS_UPDATE_BINARY,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_END};

    /* 0x63,0xF1 */
VOS_UINT8 g_aucUsim63F1Ins[] = {CMD_INS_AUTHENTICATE,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x63, 0xF2 */
VOS_UINT8 g_aucUsim63F2Ins[] = {CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x65, 0x00 */
VOS_UINT8 g_aucUsim6500Ins[] = {CMD_INS_UPDATE_BINARY,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_SET_DATA,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_END};

    /* 0x65, 0x81 */
VOS_UINT8 g_aucUsim6581Ins[] = {CMD_INS_UPDATE_BINARY,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_SET_DATA,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_END};

    /* 0x69, 0x00 */
VOS_UINT8 g_aucUsim6900Ins[] = {CMD_INS_SELECT,
                                CMD_INS_STATUS,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_GET_CHALLENGE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x69, 0x81 */
VOS_UINT8 g_aucUsim6981Ins[] = {CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x69, 0x82 */
VOS_UINT8 g_aucUsim6982Ins[] = {CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x69, 0x83 */
VOS_UINT8 g_aucUsim6983Ins[] = {CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_END};

    /* 0x69, 0x84 */
VOS_UINT8 g_aucUsim6984Ins[] ={CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x69, 0x85 */
VOS_UINT8 g_aucUsim6985Ins[] = {CMD_INS_SELECT,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x69, 0x86 */
VOS_UINT8 g_aucUsim6986Ins[] = {CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x69, 0x89 */
VOS_UINT8 g_aucUsim6989Ins[] = {CMD_INS_SELECT,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_GET_CHALLENGE,
                                CMD_INS_TERMINAL_PROFILE,
                                CMD_INS_ENVELOPE,
                                CMD_INS_FETCH,
                                CMD_INS_TERMINAL_RESPONSE,
                                CMD_INS_MANAGE_CHANNEL,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_TERMINAL_CAPABILITY,
                                CMD_INS_END};

    /* 0x6A, 0x80 */
VOS_UINT8 g_aucUsim6A80Ins[] = {CMD_INS_SEARCH_RECORD,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_TERMINAL_CAPABILITY,
                                CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

    /* 0x6A, 0x82 */
VOS_UINT8 g_aucUsim6A82Ins[] = {CMD_INS_SELECT,
                                CMD_INS_READ_BINARY,
                                CMD_INS_UPDATE_BINARY,
                                CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_INCREASE,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x6A, 0x83 */
VOS_UINT8 g_aucUsim6A83Ins[] = {CMD_INS_READ_RECORD,
                                CMD_INS_UPDATE_RECORD,
                                CMD_INS_SEARCH_RECORD,
                                CMD_INS_END};

    /* 0x6A, 0x84 */
VOS_UINT8 g_aucUsim6A84Ins[] = {CMD_INS_SET_DATA,
                                CMD_INS_END};

    /* 0x6A, 0x87 */
VOS_UINT8 g_aucUsim6A87Ins[] = {CMD_INS_SELECT,
                                CMD_INS_DEACTIVATE_FILE,
                                CMD_INS_ACTIVATE_FILE,
                                CMD_INS_END};

/* 0x6A, 0x88 */
VOS_UINT8 g_aucUsim6A88Ins[] = {CMD_INS_VERIFY,
                                CMD_INS_CHANGE_PIN,
                                CMD_INS_DISABLE_PIN,
                                CMD_INS_ENABLE_PIN,
                                CMD_INS_UNBLOCK_PIN,
                                CMD_INS_AUTHENTICATE,
                                CMD_INS_RETRIEVE_DATA,
                                CMD_INS_END};

    /* 0x98, 0x63 */
VOS_UINT8 g_aucUsim9863Ins[] = {CMD_INS_MANAGE_SECURE_CHANNEL,
                                CMD_INS_TRANSACT_DATA,
                                CMD_INS_END};

/*Sim SW字节检查*/
USIMM_SWCHECK_ST gausSimSWCheck[USIMM_SIMSW_MAXNUMBER] =
                            {
                                {0x90, 0x00, USIMM_SW_OK,              },
                                {0x91, 0xFF, USIMM_SW_OK_WITH_SAT,     },
                                {0x93, 0x00, USIMM_SW_CARD_BUSY,       },
                                {0x9E, 0xFF, USIMM_SW_WARNING,         },
                                {0x9F, 0xFF, USIMM_SW_OK_WITH_RESOPNSE,},
                                {0x92, 0x40, USIMM_SW_MEMORY_ERROR,    },
                                {0x92, 0xFF, USIMM_SW_OK,              },
                                {0x94, 0x00, USIMM_SW_NOFILE_SELECT,   },
                                {0x94, 0x02, USIMM_SW_OUTOF_RANGE,     },
                                {0x94, 0x04, USIMM_SW_NOFILE_FOUND,    },
                                {0x94, 0x08, USIMM_SW_FILETYPE_ERROR,  },
                                {0x98, 0x02, USIMM_SW_SECURITY_ERROR,  },
                                {0x98, 0x04, USIMM_SW_SECURITY_ERROR,  },
                                {0x98, 0x08, USIMM_SW_PINSTATUS_ERROR, },
                                {0x98, 0x10, USIMM_SW_FILESTATUS_ERROR,},
                                {0x98, 0x40, USIMM_SW_PIN_BLOCK,       },
                                {0x98, 0x50, USIMM_SW_INCREACE_ERROR,  },
                                {0x67, 0x00, USIMM_SW_CMD_ERROR,       },
                                {0x67, 0xFF, USIMM_SW_P3_ERROR,        },
                                {0x6B, 0xFF, USIMM_SW_CMD_ERROR,       },
                                {0x6E, 0xFF, USIMM_SW_CLA_ERROR,       },
                                {0x6F, 0x00, USIMM_SW_TECH_ERROR,      },
                                {0x6F, 0xFF, USIMM_SW_OTHER_ERROR      }
                            };

/*Usim SW字节检查*/
USIMM_SWCHECK_ST gausUsimSWCheck[USIMM_USIMSW_MAXNUMBER] =
                            {
                                {0x90, 0x00, USIMM_SW_OK,                },
                                {0x61, 0xFF, USIMM_SW_OK_WITH_RESOPNSE,  },
                                {0x91, 0xFF, USIMM_SW_OK_WITH_SAT,       },
                                {0x93, 0x00, USIMM_SW_CARD_BUSY,         },
                                {0x98, 0x50, USIMM_SW_INCREACE_ERROR,    },
                                {0x98, 0x62, USIMM_SW_AUTN_ERROR ,       },
                                {0x62, 0xFF, USIMM_SW_WARNING,           },
                                {0x63, 0xC0, USIMM_SW_CMD_REMAINTIME,    },
                                {0x63, 0xFF, USIMM_SW_WARNING,           },
                                {0x64, 0x00, USIMM_SW_MEMORY_ERROR,      },
                                {0x65, 0x00, USIMM_SW_MEMORY_ERROR,      },
                                {0x65, 0x81, USIMM_SW_MEMORY_ERROR,      },
                                {0x67, 0x00, USIMM_SW_CMD_ERROR,         },
                                {0x67, 0xFF, USIMM_SW_P3_ERROR,          },
                                {0x68, 0x00, USIMM_SW_CLA_ERROR,         },
                                {0x68, 0x81, USIMM_SW_CHANNEL_NOSUPPORT, },
                                {0x68, 0x82, USIMM_SW_SECURITY_ERROR,    },
                                {0x69, 0x00, USIMM_SW_OTHER_ERROR,       },
                                {0x69, 0x81, USIMM_SW_FILETYPE_ERROR,    },
                                {0x69, 0x82, USIMM_SW_SECURITY_ERROR,    },
                                {0x69, 0x83, USIMM_SW_PIN_BLOCK,         },
                                {0x69, 0x84, USIMM_SW_PINNUMBER_ERROR,   },
                                {0x69, 0x85, USIMM_SW_FILESTATUS_ERROR,  },
                                {0x69, 0x86, USIMM_SW_NOFILE_SELECT,     },
                                {0x69, 0x89, USIMM_SW_COMMAND_ERROR,     },
                                {0x6A, 0x80, USIMM_SW_CMD_ERROR,         },
                                {0x6A, 0x81, USIMM_SW_CMD_ERROR,         },
                                {0x6A, 0x82, USIMM_SW_NOFILE_FOUND,      },
                                {0x6A, 0x83, USIMM_SW_NORECORD_FOUND,    },
                                {0x6A, 0x84, USIMM_SW_CMD_ERROR,         },
                                {0x6A, 0x86, USIMM_SW_CMD_ERROR,         },
                                {0x6A, 0x87, USIMM_SW_CMD_ERROR,         },
                                {0x6A, 0x88, USIMM_SW_CMD_ERROR,         },
                                {0x6B, 0x00, USIMM_SW_CMD_ERROR,         },
                                {0x6C, 0xFF, USIMM_SW_P3_ERROR,          },
                                {0x6E, 0x00, USIMM_SW_CLA_ERROR,         },
                                {0x6F, 0x00, USIMM_SW_TECH_ERROR,        },
                                {0x6F, 0xFF, USIMM_SW_OTHER_ERROR        },
                                {0x98, 0x63, USIMM_SW_EXPIRED_ERROR      }
                            };

/* ISIM服务状态位对应到USIM UST上的索引 */

/*lint -e656*/
const VOS_UINT8 g_aucServIsimToUsim[] =
/*lint +e656*/
                            {
                                0,                  /* ISIM_SVR_PCSCF_ADDR */
                                USIM_SVR_GBA,       /* ISIM_SVR_GBA */
                                0,                  /* ISIM_SVR_HTTP_DIGEST */
                                USIM_SVR_GBAKEY_EM, /* ISIM_SVR_LOCAL_GBA */
                                0,                  /* ISIM_SVR_IMS_PCSCF */
                                USIM_SVR_SMS,       /* ISIM_SVR_SMS */
                                USIM_SVR_SMSR,      /* ISIM_SVR_SMSR */
                                0,                  /* ISIM_SVR_SOI */
                                USIM_SVR_CTRL_IMS,  /* ISIM_SVR_COMM_CTRL */
                                USIM_SVR_ACCESS_IMS /* ISIM_SVR_UICC_IMS */
                            };


/*Usim 卡初始化过程函数列表*/
USIMM_CARD_INIT_FUNC g_aUsimInitCardProcFuncTbl[USIMM_INIT_USIM_CARD_NUM]=
{
    {VOS_NULL_PTR                   },
    {USIMM_InitUsimGlobal           },
    {USIMM_InsertNoNeedPinFileToPool},
    {USIMM_UsimProfileDownload      },
    {USIMM_Select3GApp              },
    {USIMM_SelectUiccADF            },
    {USIMM_InitCardPINInfo          },
    {USIMM_AdjudgeCardState         },
    {USIMM_InitUsimEccFile          },
    {USIMM_InsertLPFileToPool       },
    {USIMM_InitUsimCardFileMap      }
};

/*Sim 卡初始化过程函数列表*/
USIMM_CARD_INIT_FUNC g_aSimInitCardProcFuncTbl[USIMM_INIT_SIM_CARD_NUM]=
{
    {VOS_NULL_PTR                   },
    {USIMM_InitSimGlobal            },
    {USIMM_Select2GApp              },
    {USIMM_InsertNoNeedPinFileToPool},
    {USIMM_InitCardPINInfo          },
    {USIMM_AdjudgeCardState         },
    {USIMM_InitSimEccFile           },
    {USIMM_InsertLPFileToPool       },
    {USIMM_InitSimCardFileMap       }
};

/*初始化卡的过程函数入口控制*/
USIMM_CARD_INIT_TBL_ST g_aCardInitTbl[USIMM_CARD_INIT_TBL_SIZE]=
{
    {g_aUsimInitCardProcFuncTbl, USIMM_INIT_USIM_CARD_NUM},
    {g_aSimInitCardProcFuncTbl,  USIMM_INIT_SIM_CARD_NUM}
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


