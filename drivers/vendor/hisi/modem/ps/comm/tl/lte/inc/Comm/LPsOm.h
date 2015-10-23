

#ifndef __PSOM_H__
#define __PSOM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "product_config.h"
#include  "vos.h"
#include  "PsTypeDef.h"
#include  "PsLogdef.h"
#include  "PsCommonDef.h"
#include  "msp.h"
#include  "LPSCommon.h"
#include  "OmCommon.h"
#include  "AppRrcInterface.h"
#include  "LPsNvInterface.h"

#if 0   /*deleted by hujianbo 20130809:产品线裁剪代码时不能包含非开放头文件，故将其去掉*/
#include  "LHpaMsgDebug.h"
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/

#define  MAX_HMAC_HASH_SIZE              32  /* KDF算法用到的最大HMAC长度 */
#define  LPS_GAS_OM_OTA_IND                 0xA025

#define  LPS_GNAS_OM_OTA_IND                0xC103

#define  LPS_WAS_OM_OTA_IND                 0XB022

#define  EN_NV_ID_TEST_MODE_FLAG            6656

#define TLRRC_VERSION_V9R1                  0x91

#define TLRRC_VERSION_V7R2                  0x72

#define TLRRC_VERSION_V7R1                  0x71

#define TLRRC_FEATURE_ON                    1
#define TLRRC_FEATURE_OFF                   0
#define TLRRC_FEATURE_STK_V9R1              TLRRC_FEATURE_ON

#define  TLRRC_VERSION                      TLRRC_VERSION_V7R2

/*g_astMsgIdToPidTbl表格里最少的message id的界限个数*/
#define LPS_OM_MSG_TABLE_MIN_NUM             2

/*VOS_PID_BUTT无效的PID*/
#define LPS_PID_NULL                         VOS_PID_BUTT

#define LPS_OM_SPECIFY_FLG                   (0xF0000000UL)

#define TLRRC_FEATURE_ON                    1
#define TLRRC_FEATURE_OFF                   0

#define TLRRC_FAST_CSFB_FEATURE             TLRRC_FEATURE_ON
#define LHPA_DATA_ABORT_LAST_MSG           16


#if (VOS_OS_VER != VOS_WIN32)
#define PS_LOG_RETURN() \
            if(0 != g_ulTimeDelaySwitch) \
            {\
              return;\
            }

#define PS_LOG_RETURN_SUCC() \
            if(0 != g_ulTimeDelaySwitch) \
            {\
              return VOS_OK;\
            }

#define PS_HOOK_LOG_RETURN_SUCC() \
            if(0 != g_ulTimeDelayHookSwitch) \
            {\
              return VOS_OK;\
            }

#else
#define PS_LOG_RETURN()

#define PS_LOG_RETURN_SUCC()

#define PS_HOOK_LOG_RETURN_SUCC()
#endif

/*LOG打印*/
#define LPS_OM_INFO_LOG(pcString)\
        LPS_LOG(UE_MODULE_PS_OM, 0, LOG_LEVEL_INFO, pcString)

#define LPS_OM_INFO_LOG1(pcString, lPara1)\
        LPS_LOG1(UE_MODULE_PS_OM, 0, LOG_LEVEL_INFO, pcString, lPara1)

#define LPS_OM_INFO_LOG2(pcString, lPara1, lPara2)\
        LPS_LOG2(UE_MODULE_PS_OM, 0, LOG_LEVEL_INFO, pcString, lPara1,lPara2)

#define LPS_OM_WARNING_LOG(pcString)\
        LPS_LOG(UE_MODULE_PS_OM, 0, LOG_LEVEL_WARNING, pcString)

#define LPS_OM_WARNING_LOG1(pcString, lPara1)\
        LPS_LOG1(UE_MODULE_PS_OM, 0, LOG_LEVEL_WARNING, pcString, lPara1)

#define LPS_OM_WARNING_LOG2(pcString, lPara1, lPara2)\
        LPS_LOG2(UE_MODULE_PS_OM, 0, LOG_LEVEL_WARNING, pcString, lPara1,lPara2)

#define LPS_OM_ERROR_LOG(pcString)\
        LPS_LOG(UE_MODULE_PS_OM, 0, LOG_LEVEL_ERROR, pcString)

#define LPS_OM_ERROR_LOG1(pcString, lPara1)\
        LPS_LOG1(UE_MODULE_PS_OM, 0, LOG_LEVEL_ERROR, pcString, lPara1)

#define LPS_OM_ERROR_LOG2(pcString, lPara1, lPara2)\
        LPS_LOG2(UE_MODULE_PS_OM, 0, LOG_LEVEL_ERROR, pcString, lPara1,lPara2)



/*PS处理完APP发送来的消息后，把消息的空间释放*/
#define LPS_OM_FREE_APP_MSG_MEM(pAddr)   PS_MEM_FREE(PS_PID_OM, pAddr);

/* 获得时延统计信息 */
#define LPS_OM_Get_LATENCY_INFO() (&g_stPsLatencyStat)

#if (VOS_OS_VER != VOS_WIN32)
/* 设置HO流程时延 */
#define LPS_HO_SET_LATENCY_TIME(ucLatencyType) \
{\
    LPS_OM_SetLatency(g_stPsLatencyStat.stHoLatency.aulHoLatency, ucLatencyType, LPS_HO_LATENCY_BUFF);\
}
/* 设置建链接时延 */
#define LPS_EST_SET_LATENCY_TIME(ucLatencyType) \
{\
    LPS_OM_SetLatency(g_stPsLatencyStat.stEstLatency.aulEstLatency, ucLatencyType, LPS_EST_LATENCY_BUFF);\
}
/* 设置重建时延 */
#define LPS_REEST_SET_LATENCY_TIME(ucLatencyType) \
{\
    LPS_OM_SetLatency(g_stPsLatencyStat.stReestLatency.aulReestLatency, ucLatencyType, LPS_REEST_LATENCY_BUFF);\
}

/* 获得建链控制面时延是否有效标志 */
#define LPS_EST_GET_LATENCY_FLAG() (g_stPsLatencyStat.stEstLatency.enContralPlaneValidFlag)

#define LPS_EST_SET_LATENCY_FLAG(ulFlag) (g_stPsLatencyStat.stEstLatency.enContralPlaneValidFlag = ulFlag)

/* 获得重建时延是否有效标志 */
#define LPS_REEST_GET_LATENCY_FLAG() (g_stPsLatencyStat.stReestLatency.enReestValidFlag)

#define LPS_REEST_SET_LATENCY_FLAG(ulFlag) (g_stPsLatencyStat.stReestLatency.enReestValidFlag = ulFlag)



/* mod by guojiyu 可维可测切换时延 begin */
#define LPS_GET_HO_LatencyInfo()            (&g_stPsHoInfo)

#define LPS_HO_GET_HO_TYPE()                (g_stPsHoInfo.enHoType)
#define LPS_HO_SET_HO_TYPE(enHoType)        (LPS_HO_GET_HO_TYPE() = (enHoType))

#define LPS_GET_HO_LatestHOLatency()        (g_stPsHoInfo.usLatestHOLatency)
#define LPS_GET_HO_HoLatencyInfo(enHoType)  (g_stPsHoInfo.astHOLatencyInfo + (enHoType))

#define LPS_GET_HO_AllSuccTime(enHoType)    (g_stPsHoInfo.aulAllHoSuccTime[enHoType])
/* mod by guojiyu 可维可测切换时延 end */
#else
#define LPS_HO_SET_LATENCY_TIME(ucLatencyType)
/* 设置建链接时延 */
#define LPS_EST_SET_LATENCY_TIME(ucLatencyType)
/* 设置重建时延 */
#define LPS_REEST_SET_LATENCY_TIME(ucLatencyType)


/* 获得建链控制面时延是否有效标志 */
#define LPS_EST_GET_LATENCY_FLAG()                                  PS_FALSE

#define LPS_EST_SET_LATENCY_FLAG(ulFlag)

/* 获得重建时延是否有效标志 */
#define LPS_REEST_GET_LATENCY_FLAG()                                PS_FALSE

#define LPS_REEST_SET_LATENCY_FLAG(ulFlag)

/* mod by guojiyu 可维可测切换时延 begin */
#define LPS_GET_HO_LatencyInfo()

#define LPS_HO_GET_HO_TYPE()
#define LPS_HO_SET_HO_TYPE(enHoType)

#define LPS_GET_HO_LatestHOLatency()
#define LPS_GET_HO_HoLatencyInfo(enHoType)

#define LPS_GET_HO_AllSuccTime(enHoType)
/* mod by guojiyu 可维可测切换时延 end */

#endif
/*TDS begin*/
#if 0
#define tra01_10tdynamicTrace DIAG_PrintfV_TDS
#define tgl01_21dynamicTraceToLog DIAG_PrintfV_TDS
#define fpv01_30SpvDynamicTraceToLog DIAG_PrintfV_TDS
#define fpv01_32AtrDynamicTraceToLog DIAG_PrintfV_TDS
#define fpv01_33AtcDynamicTraceToLog DIAG_PrintfV_TDS
#define fpv01_31SimDynamicTraceToLog DIAG_PrintfV_TDS
#define tra06_40dynamicTrace DIAG_PrintfV_TDS1
#endif
#define tra01_10tdynamicTrace TDS_OM_DiagPrint
#define tgl01_21dynamicTraceToLog TDS_OM_DiagPrint
#define fpv01_30SpvDynamicTraceToLog TDS_OM_DiagPrint
#define fpv01_32AtrDynamicTraceToLog TDS_OM_DiagPrint
#define fpv01_33AtcDynamicTraceToLog TDS_OM_DiagPrint
#define fpv01_31SimDynamicTraceToLog TDS_OM_DiagPrint
#define tra06_40dynamicTrace TDS_OM_DiagPrint1
/*TDS end*/


#define LPS_GET_NV_MEAS_THRESHOLD()                      (&g_LPsNvCellMeasThredhold)

#define LPS_GET_NV_RSRP_THRESHOLD()                      (g_LPsNvCellMeasThredhold.sRsrpHoldThreshold) /* PHY上报的测量值小于某值 ucRsrpLowCount++*/
#define LPS_SET_NV_RSRP_THRESHOLD(sRsrpHoldThreshold)    (LPS_GET_NV_RSRP_THRESHOLD() = (sRsrpHoldThreshold)) /* PHY上报的测量值小于某值 ucRsrpLowCount++*/

#define LPS_GET_NV_RSRP_LOW_CONT()                       (g_LPsNvCellMeasThredhold.ucRsrpLowCount)  /* PHY上报的测量值小于sRsrpHoldThreshold达到两次。发起重建流程 */
#define LPS_SET_NV_RSRP_LOW_CONT(ucRsrpLowCount)         (LPS_GET_NV_RSRP_LOW_CONT() = (ucRsrpLowCount))  /* PHY上报的测量值小于sRsrpHoldThreshold达到两次。发起重建流程 */

#define LPS_GET_NV_SYNSSWITCH()                           (g_LPsNvCellMeasThredhold.ucSynsSwitch) /*是否启动测量值掉底发起重建流程 */
#define LPS_SET_NV_SYNSSWITCH(ucSynsSwitch)               (LPS_GET_NV_SYNSSWITCH() = (ucSynsSwitch)) /*是否启动测量值掉底发起重建流程 */


#define LPS_VERSION_V_NO                     (700)                                 /* V版本号 */
#define LPS_VERSION_R_NO                     (  1)                                 /* R版本号 */
#define LPS_VERSION_C_NO                     ( 10)                                 /* C版本号 */
#define LPS_VERSION_B_NO                     ( 60)                                 /* B版本号 */
#define LPS_VERSION_SPC_NO                   (000)                                 /* SPC版本号 */
#define LPS_VERSION_PRODUCT_NO               (1)                                   /* Product Version */
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
/* mod by guojiyu 0xffff begin */
/*****************************************************************************
 结构名    : LPS_HO_TYPE_ENUM
 结构说明  : 切换类型枚举
*****************************************************************************/
enum LPS_HO_TYPE_ENUM
{
    LPS_INTRA_FREQ_HO = 0,
    LPS_INTER_FREQ_HO,
    LPS_INTER_RAT_HO,
    LPS_HO_TYPE_BUFF
};
typedef VOS_UINT8 LPS_HO_TYPE_ENUM_UINT8;
/* mod by guojiyu 0xffff END */

/*****************************************************************************
 结构名    : LPS_HO_LATENCY_ENUM
 结构说明  : 切换时延阶段枚举
*****************************************************************************/
enum LPS_HO_LATENCY_ENUM
{
    LPS_HO_LATENCY_LRRC_SND_MEAS_RPT = 0,/* mod by guojiyu 0xffff begin */
    LPS_HO_LATENCY_LRRC_RCV_HO_RECFG,
    LPS_HO_LATENCY_LRRC_RCV_NAS_RABM_RSP,
    LPS_HO_LATENCY_LRRC_RCV_CMM_HO_CNF,
    LPS_HO_LATENCY_LRRC_RCV_STOP_PDCP_CNF,
    LPS_HO_LATENCY_LRRC_SND_DSP_HO_REQ,
    LPS_HO_LATENCY_LRRC_RCV_DSP_HO_CNF,
    LPS_HO_LATENCY_LRRC_SND_MAC_RA_REQ,
    LPS_HO_LATENCY_LMAC_LPHY_ACCESS_REQ,
    LPS_HO_LATENCY_LMAC_RCV_RAR,
    LPS_HO_LATENCY_LRRC_RCV_MAC_RA_CNF,
    LPS_HO_LLATENCY_RRC_SND_SMC_SECU_CFG,
    LPS_HO_LATENCY_LRRC_SND_RECFG_CMP,
    LPS_HO_LATENCY_LRRC_RCV_AM_DATA_CNF,
    LPS_HO_LATENCY_LRRC_RCV_CQI_SRS_CNF,
    LPS_HO_LATENCY_LRRC_RCV_PDCP_CONTINUE_CNF,
    LPS_HO_LATENCY_LRRC_RCV_RABM_STATUS_RSP,
    LPS_HO_LATENCY_BUFF
};

/*****************************************************************************
 结构名    : LPS_REEST_LATENCY_ENUM
 结构说明  : 重建时延阶段枚举
*****************************************************************************/
enum LPS_REEST_LATENCY_ENUM
{
    LPS_REEST_LATENCY_LRRC_RCV_REESTIND,
    LPS_REEST_LATENCY_LRRC_SND_CELL_SEARCH_REQ,
    LPS_REEST_LATENCY_LRRC_RCV_CELLSEARCH_IND,
    LPS_REEST_LATENCY_LRRC_RCV_SI,
    LPS_REEST_LATENCY_LRRC_CAMP_REQ,
    LPS_REEST_LATENCY_LRRC_CAMP_CNF,
    LPS_REEST_LATENCY_LRRC_SND_LMAC_RA_REQ ,
    LPS_REEST_LATENCY_LMAC_LPHY_ACCESS_REQ,
    LPS_REEST_LATENCY_LMAC_RCV_RAR,
    LPS_REEST_LATENCY_LRRC_RCV_LMAC_RA_CNF,
    LPS_REEST_LATENCY_LMAC_SND_MSG3,
    LPS_REEST_LATENCY_LRRC_RCV_REEST,
    LPS_REEST_LATENCY_LRRC_SND_REEST_CMPL,
    LPS_REEST_LATENCY_BUFF
};
typedef VOS_UINT8 LPS_REEST_LATENCY_ENUM_UINT8;

/*****************************************************************************
 结构名    : LPS_EST_LATENCY_ENUM
 结构说明  : 控制面时延阶段枚举
*****************************************************************************/
enum LPS_EST_LATENCY_ENUM
{
    LPS_EST_LATENCY_LRRC_SND_LMAC_RA_REQ = 0,
    LPS_EST_LATENCY_LMAC_LPHY_ACCESS_REQ,
    LPS_EST_LATENCY_LMAC_RCV_RAR,
    LPS_EST_LATENCY_LRRC_RCV_LMAC_RA_CNF,
    LPS_EST_LATENCY_LMAC_SND_MSG3,
    LPS_EST_LATENCY_LRRC_RCV_SETUP,
    LPS_EST_LATENCY_LRRC_SND_SETUP_CMP,
    LPS_EST_LATENCY_LRRC_RCV_CAP_ENQ,
    LPS_EST_LATENCY_LRRC_SND_CAP_INFO,
    LPS_EST_LATENCY_LRRC_RCV_SEC_CMD,
    LPS_EST_LATENCY_LRRC_RCV_RECFG,
    LPS_EST_LATENCY_LRRC_SND_SEC_CMD_CMP,
    LPS_EST_LATENCY_LRRC_SND_RECFG_CMP,
    LPS_EST_LATENCY_BUFF
};
typedef VOS_UINT8 LPS_EST_LATENCY_ENUM_UINT8;


enum LPS_REBOOT_MOD_ID_ENUM
{
    LPS_REBOOT_MOD_ID_MML   = 0Xa0000000,
    LPS_REBOOT_MOD_ID_BUTT  = 0X6FFFFFFF
};
typedef VOS_UINT32 LPS_REBOOT_MOD_ID_ENUM_UINT32;


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
 /*****************************************************************************
 结构名    : LPS_HO_LATENCY_STRU
 结构说明  : PS切换时延统计信息数据结构(
*****************************************************************************/
typedef struct
{
    PS_BOOL_ENUM_UINT8                  enHoValidFlag;                          /* 本次测量值是否有效 */
    VOS_UINT8                           aucReservered[3];
    VOS_UINT32                          aulHoLatency[LPS_HO_LATENCY_BUFF];        /* 记录每个关键点的测试值 */
}LPS_HO_LATENCY_STRU;
/*****************************************************************************
 结构名    : LPS_REEST_LATENCY_STRU
 结构说明  : PS重建时延统计信息数据结构(
*****************************************************************************/
typedef struct
{
    PS_BOOL_ENUM_UINT8                  enReestValidFlag;                       /* 本次测量值是否有效 */
    VOS_UINT8                           aucReservered[3];
    VOS_UINT32                          aulReestLatency[LPS_REEST_LATENCY_BUFF];  /* 记录每个关键点的测试值 */
}LPS_REEST_LATENCY_STRU;

/*****************************************************************************
 结构名    : LPS_EST_LATENCY_STRU
 结构说明  : PS控制面时延统计信息数据结构(
*****************************************************************************/
typedef struct
{
    PS_BOOL_ENUM_UINT8                  enContralPlaneValidFlag;                /* 本次测量值是否有效 */
    VOS_UINT8                           aucReservered[3];
    VOS_UINT32                          aulEstLatency[LPS_EST_LATENCY_BUFF];      /* 记录每个关键点的测试值 */
}LPS_EST_LATENCY_STRU;

/*****************************************************************************
 结构名    : LPS_LATENCY_STAT_STRU
 结构说明  : PS时延统计信息数据结构(
*****************************************************************************/
typedef struct
{
    PS_BOOL_ENUM_UINT8                   enReportFlg;                            /* 时延信息是否上报标志 */
    VOS_UINT8                            aucReserved[3];
    LPS_HO_LATENCY_STRU                  stHoLatency;                         /* 切换时延统计信息 */
    LPS_REEST_LATENCY_STRU               stReestLatency;                      /* 重建时延统计信息 */
    LPS_EST_LATENCY_STRU                 stEstLatency;                          /* 连接建立时延统计信息 */
}LPS_LATENCY_STAT_STRU;
/*****************************************************************************
 结构名    : LPS_OM_MSG_PID_PAIR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 消息ID的取值段起始值和PID的对应关系对
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulMsgIdStartVal;
    VOS_PID                         ulPid;
}LPS_OM_MSG_PID_PAIR_STRU;


/*****************************************************************************
 结构名    : LPS_OM_MSG_ID_SEARCH_COMM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : ulMsgId的取值和ulSpare的对应关系对
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulMsgId;
    VOS_UINT32                      ulSpare;
}LPS_OM_MSG_ID_SEARCH_COMM_STRU;

/*added by guojiyu*/
/*****************************************************************************
 结构名    : LPS_HANDOVER_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          aulAllHoSuccTime[LPS_HO_TYPE_BUFF];
    VOS_UINT16                          usLatestHOLatency;      /*最近一次切换时延*/
    VOS_UINT16                          usAverageHOLatency;     /*平均切换时延*/
    LPS_HO_TYPE_ENUM_UINT8              enHoType;
    VOS_UINT8                           ucResvd[3];
    LPS_HO_STAT_INFO_STRU               astHOLatencyInfo[LPS_HO_TYPE_BUFF];
}LPS_HANDOVER_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER                   /* VOS消息头 */
    VOS_UINT16    usTransPrimId;     /* 固定值 0x5001 */
    VOS_UINT16    usRsv1;            /* 保留 */
    VOS_UINT8     ucFuncType;        /* 消息跟踪，固定值 2 */
    VOS_UINT8     ucReserve;         /* 保留 */
    VOS_UINT16    usLength;          /* 下一字节至包尾的长度，单位为字节 */
    VOS_UINT32    ulSn;              /* 上报消息的序列号 */
    VOS_UINT32    ulTimeStamp;       /* 消息的时间戳 */
    VOS_UINT16    usPrimId;          /* 原语ID */
    VOS_UINT16    usRsv2;             /* 保留 */
    VOS_UINT16    usOtaMsgID;
    VOS_UINT8     ucUpDown;
    VOS_UINT8     ucRsv;
    VOS_UINT32    ulLengthASN;
    VOS_UINT8     aucData[4];
}LTE_GAS_OM_OTA_IND_STRUCT;

/*****************************************************************************
 结构名    : T_DlRecvMsgStat
 协议表格  :
 ASN.1描述 :
 结构说明  : 记录TDS下行接收数据消息的统计量
*****************************************************************************/
typedef struct
{
    VOS_UINT32    ulSccpchReadIndNum;
    VOS_UINT32    ulDpchReadIndNum;
    VOS_UINT32    ulHspdschReadIndNum;
    VOS_UINT32    ulHsUpaInfoIndNum;
}T_DlRecvMsgStat;
typedef struct
{
    VOS_UINT32      ulTimeStamp;
    VOS_UINT32      ulPid;
    VOS_UINT32      ulOpId;
    VOS_UINT32      MsgId;
}LHPA_DATA_ABORT_MSG_STRU;

typedef struct
{
    VOS_UINT32                    ulDownCurrCnt;
    VOS_UINT32                    ulUpCurrCnt;
    LHPA_DATA_ABORT_MSG_STRU      ulForMailBoxDownDataAbortMsg[LHPA_DATA_ABORT_LAST_MSG];
    LHPA_DATA_ABORT_MSG_STRU      ulForMailBoxUpDataAbortMsg[LHPA_DATA_ABORT_LAST_MSG];
    VOS_UINT32                    ulCurrMailBoxLen;
}LHPA_MAILBOX_DATAABORT_STRU;

extern LHPA_MAILBOX_DATAABORT_STRU  g_stHpaMailBoxDataAbortMsg;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
/*#if (VOS_OS_VER != VOS_WIN32)
 */
extern LPS_LATENCY_STAT_STRU               g_stPsLatencyStat;
/*#endif
 */
#if (VOS_OS_VER != VOS_WIN32)
extern UE_SW_BUILD_VER_INFO_STRU g_stPSVersionInfo;
#endif

/* mod by guojiyu 可维可测切换时延 begin */
/* 切换时延统计全局变量 */
extern LPS_HANDOVER_INFO_STRU          g_stPsHoInfo;
/* mod by guojiyu 可维可测切换时延 end */

/* gaojishan-remove-kdf-hmac-sha */
#if 0
/* 硬件KDF算法使用功能开关，0为关闭，0x10为打开。默认为关闭 */
extern VOS_UINT32                       g_ulHardKdf;
/* 混合硬软件KDF算法使用功能开关，0为关闭，0x20为打开。默认为关闭 */
extern VOS_UINT32                       g_ulHardKdfHybri;
#endif
extern VOS_UINT32                       g_ulPsBandwidthSupportFlg;
/* modify by lishangfeng freq all begin */
extern VOS_UINT32                       g_ulPsFreqInAllBandWidthFlg;
/* modify by lishangfeng freq all end */

extern LPS_NV_MEAS_THRESHOLD_STRU g_LPsNvCellMeasThredhold;

extern VOS_UINT32 g_ulNasPlainRejMsgFlag;
/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_UINT32  LPS_OM_PidMsgProc(const MsgBlock *pMsg  );
extern VOS_UINT32 LPS_OM_SearchMsgIdTbl( VOS_UINT32 ulMsgId,
                                                 CONST LPS_OM_MSG_ID_SEARCH_COMM_STRU *pstTbl,
                                                 VOS_UINT32 ulTblSize,
                                                 VOS_UINT32 *pulTblIdx);
extern VOS_UINT32 LPS_OM_SearchMsgIdTblForHpa( VOS_UINT32 ulMsgId,
                                          CONST LPS_OM_MSG_ID_SEARCH_COMM_STRU *pstTbl,
                                          VOS_UINT32 ulTblSize,
                                          VOS_UINT32 *pulTblIdx);
extern VOS_UINT32 LPS_OM_TraceMsgHook( VOS_VOID *pMsg );

extern VOS_VOID LPS_OM_SetLatency(VOS_UINT32* pulLatency,
                            VOS_UINT8 ucLatencyType,VOS_UINT32 ulLatencyBuff);

extern VOS_VOID LNAS_LPSOM_GetNvRaMod ( VOS_UINT32       *pulPsCfgDspRatMod);
extern     VOS_VOID LNAS_LPSOM_GetCampedCellInfo(LPS_NAS_MM_TA_LIST_STRU *pstRegPlmnTaList,
        LPS_NAS_IMSI_STRU *pstImsiInfo,LPS_NAS_MM_GUTI_STRU *pstGutiInfo);
extern  VOS_VOID LNAS_LPSOM_GetIpAddrInfo(APP_LPS_CAMPED_CELL_INFO_STRU *pstCampedCellInfo);
extern VOS_VOID LRRC_LPSOM_GetCampedCellInfo(APP_LPS_CAMPED_CELL_INFO_STRU *pstCampedCellInfo);
extern VOS_VOID LPS_OM_GetHoInfo(APP_LPS_HANDOVER_INFO_STRU *psPsOmGetHoinfo);
extern  VOS_VOID L2_LPSOM_GetMacPrachInfo(APP_LPS_PRACH_SR_INFO_STRU *pstLMacOmInfo);
extern  VOS_VOID L2_LPSOM_GetMacBsrInfo(APP_LPS_BSR_INFO_STRU *pstLMacOmBsrInfo);

VOS_VOID LPS_OM_CampedCellInfoInd(VOS_VOID);
VOS_VOID LPS_OM_CellHOInfoInd(VOS_VOID);
VOS_VOID LPS_OM_CampedCellInfoReq(APP_OM_MSG_STRU *pMsg);
VOS_VOID LPS_OM_CellHOInfoReq(APP_OM_MSG_STRU *pMsg);
VOS_VOID LPS_OM_PrachSrInfoReq(APP_OM_MSG_STRU *pMsg);
VOS_VOID LPS_OM_BsrInfoReq(APP_OM_MSG_STRU *pMsg);
extern VOS_UINT32 LTE_MsgHook( VOS_VOID *pMsg );

extern VOS_VOID  LPS_OM_StatInit(VOS_VOID);
/* gaojishan-Dug-mod-Begin */
/* gaojishan-reboot-save-lrrc-para-for-nas-reboot */
extern VOS_VOID LPS_SoftReBoot_SaveLRrc( VOS_VOID );
extern VOS_UINT32 LPS_SoftReBoot_WithLineNoAndFileID(VOS_UINT32 ulDbgNum,VOS_UINT32 ulLineNO, VOS_UINT32 ulFileID);
/* gaojishan-Dug-mod-End */

extern VOS_VOID LHPA_ProcGetSelfAdjustPara( VOS_VOID );

extern VOS_UINT32 LPS_OM_GetTmode(FTM_TMODE_ENUM* penFtmMode);


extern VOS_UINT32 LPS_OM_IsTestMode(VOS_VOID);

#if (PLATFORM != hi6930_v7r2)
extern VOS_VOID TLPS_ExcLog_Save(VOS_VOID);
#endif
extern VOS_UINT32 TLPS_TaskDelay(VOS_UINT32 ulLength);

/*封装取得时间戳函数*/
/*由于低软封装的宏DRV_32K_GET_TICK()无法使用，所以只能这样定义，FPGA无法使用32K所以不用*/
#if ((VOS_VXWORKS == VOS_OS_VER)\
    && (defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
    && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
    extern BSP_U32 BSP_32K_GetTick( BSP_VOID );
    #define PS_GET_TICK()    BSP_32K_GetTick()
#else
    #define PS_GET_TICK()    VOS_GetTick()
#endif

extern VOS_UINT32 TDS_OM_DiagPrint(const VOS_CHAR *pcString, ...);
extern VOS_UINT32 TDS_OM_DiagPrint1(VOS_UINT32 ulId, const VOS_CHAR *pcString, ...);

extern VOS_VOID LPS_OM_StatInfoMsgProc(MsgBlock CONST *pMsg);
/*****************************************************************************
  9 OTHERS
*****************************************************************************/







#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of LPsOm.h */
