

#ifndef __MTCCTX_H__
#define __MTCCTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "Ps.h"
#include "pslog.h"
#include "MtcRrcInterface.h"
#include "MtcMmaInterface.h"
#include "TafMtcInterface.h"
#include "MtaMtcInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#if (VOS_WIN32 == VOS_OS_VER)
#define MTC_WARNING_LOG(string)                 vos_printf("%s \r\n", string)
#define MTC_WARNING1_LOG(string, para1)         vos_printf("%s %d.\r\n", string, para1)
#define MTC_WARNING2_LOG(string, para1, para2)  vos_printf("%s %d, %d.\r\n", string, para1, para2)

#define MTC_ERROR_LOG(string)                   vos_printf("%s \r\n", string)
#define MTC_ERROR1_LOG(string, para1)           vos_printf("%s %d.\r\n", string, para1)
#define MTC_ERROR2_LOG(string, para1, para2)    vos_printf("%s %d, %d.\r\n", string, para1, para2)
#else
#define MTC_WARNING_LOG(string)                 PS_LOG(UEPS_PID_MTC, 0, PS_PRINT_WARNING,  string)
#define MTC_WARNING1_LOG(string, para1)         PS_LOG1(UEPS_PID_MTC, 0, PS_PRINT_WARNING,  string, (VOS_INT32)para1)
#define MTC_WARNING2_LOG(string, para1, para2)  PS_LOG2(UEPS_PID_MTC, 0, PS_PRINT_WARNING,  string, (VOS_INT32)para1, (VOS_INT32)para2)

#define MTC_ERROR_LOG(string)                   PS_LOG(UEPS_PID_MTC, 0, PS_PRINT_ERROR,  string)
#define MTC_ERROR1_LOG(string, para1)           PS_LOG1(UEPS_PID_MTC, 0, PS_PRINT_ERROR,  string, (VOS_INT32)para1)
#define MTC_ERROR2_LOG(string, para1, para2)    PS_LOG2(UEPS_PID_MTC, 0, PS_PRINT_ERROR,  string, (VOS_INT32)para1, (VOS_INT32)para2)
#endif

/* 将BIT n置位 */
#define MTC_SET_BIT(n)                  (0x01 << n)
#define MTC_SET_BIT64(n)                ((VOS_UINT64)1 << (n))

#define MTC_INTRUSION_CFG_BIT           MTC_SET_BIT(0)                          /* NV中标识干扰控制的bit位 */

#define MTC_NOTCH_CFG_BIT               MTC_SET_BIT(1)                          /* NV中标识notch控制的bit位 */

#define MTC_RSE_CFG_BIT                 MTC_SET_BIT(4)                          /* NV中标识RSE控制的bit位 */


#define MTC_PS_AUTOMATIC_START              (0)                                 /* 值与NAS_MMA_AUTOMATIC_START保持一致 */
#define MTC_PS_MMA_MANUAL_START             (1)                                 /* 值与NAS_MMA_MANUAL_START保持一致  */




#define MTC_NARROW_BAND_DCS_CFG_BIT     MTC_SET_BIT(2)                          /* NV中标识narrow band dcs控制的bit位 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum MTC_CFG_ENUM
{
    MTC_CFG_DISABLE                     = 0x00,                                 /* 配置不使能 */
    MTC_CFG_ENABLE                      = 0x01,                                 /* 配置使能 */
    MTC_CFG_BUTT
};
typedef VOS_UINT8  MTC_CFG_ENUM_UINT8;


enum MTC_CS_SERVICE_STATE_ENUM
{
    MTC_CS_NO_SERVICE         = 0x00,                                           /* CS域无业务 */
    MTC_CS_IN_SERVICE,                                                          /* CS域存在业务 */

    MTC_CS_SERVICE_STATE_BUTT
};
typedef VOS_UINT8 MTC_CS_SERVICE_STATE_ENUM_UINT8;


enum MTC_OPERATOR_CUST_SOLUTION_ENUM
{
    MTC_OPERATOR_CUST_NONE          = 0x00,                                     /* 无运营商定制方案 */
    MTC_OPERATOR_CUST_CMCC_SVLTE    = 0x01,                                     /* 中移动定制SVLTE特性 */
    MTC_OPERATOR_CUST_CT_LC         = 0x02,                                     /* 中国电信C+L方案 */

    MTC_OPERATOR_CUST_BUTT
};
typedef VOS_UINT8 MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8;
enum MTC_PS_TRANSFER_CAUSE_ENUM
{
    MTC_PS_TRANSFER_CAUSE_AREA_LOST                 = 0,

    MTC_PS_TRANSFER_CAUSE_AREA_AVALIABLE            = 1,

    MTC_PS_TRANSFER_CAUSE_BUTT
};
typedef VOS_UINT8 MTC_PS_TRANSFER_CAUSE_ENUM_UINT8;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


typedef struct
{
    VOS_UINT8                           ucIntrusionCfg      : 1;
    VOS_UINT8                           ucNotchBypass       : 1;
    VOS_UINT8                           ucNarrowBandDcsCfg  : 1;
    VOS_UINT8                           ucTlRfCtrlCfg       : 1;
    VOS_UINT8                           ucReserved          : 4;

    VOS_UINT8                           aucAdditonCfg[3];

} MTC_RF_INTRUSION_CFG_STRU;


typedef struct
{
    MTC_NV_RF_INTRUSION_CFG_STRU        stIntrusionCfg;                         /* 保存RF INTRUSION NV配置信息 */
    PS_BOOL_ENUM_UINT8                  enNotchEnableFlag;

    PS_BOOL_ENUM_UINT8                  enNarrowDcsEnableFlag;                  /* 记录NarrowDcs状态 */

    PS_BOOL_ENUM_UINT8                  enTlRfCtrlEnableFlag;

    PS_BOOL_ENUM_UINT8                  enRseCfg;                               /* 记录已经下发过的RSE配置 */
}MTC_INTRUSION_CTX_STRU;
typedef struct
{
    VOS_UINT8                           ucCallSrvExistFlg;                      /* Modem是否存在呼叫服务 */
    VOS_UINT8                           ucSmsSrvExistFlg;                       /* Modem是否存在短信业务 */
    VOS_UINT8                           ucSsSrvExistFlg;                        /* Modem是否存在补充业务 */
    VOS_UINT8                           aucReserved1[1];
}MTC_MODEM_SERVICE_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucPsSupportFlg;                         /* 当前PS域是否支持GPRS接入 */
    VOS_UINT8                           ucPsAttachAllowFlg;                     /* PS域是否允许注册,VOS_TRUE:允许PS域注册,VOS_FALSE:不允许PS域注册 */

    VOS_UINT8                           aucReserved1[2];
}MTC_MODEM_PS_DOMAIN_INFO_STRU;


typedef struct
{
    MTC_MMA_PLMN_ID_STRU                stPlmnId;                               /* Modem当前驻留网络 */
    MTC_MODEM_PS_DOMAIN_INFO_STRU       stPsDomainInfo;                         /* Modem当前PS域信息 */

    VOS_UINT8                           ucIsForbiddenPlmnFlag;                  /* Modem当前驻留网络是否为禁止网络标识 */
    VOS_UINT8                           aucReserved1[3];
}MTC_MODEM_NETWORK_INFO_STRU;
typedef struct
{
    PS_BOOL_ENUM_UINT8                  enCsExistFlag;
    PS_BOOL_ENUM_UINT8                  enPsExistFlag;

    VOS_UINT16                          usScellBitMap;
    VOS_UINT16                          usHoppingBitMap;
    VOS_UINT16                          usNcellBitMap;
}MTC_MODEM_MIPICLK_BITMAP_STRU;

typedef struct
{
    VOS_UINT8                           ucRatCap;                               /* Modem的接入技术能力值 */
    MTC_MODEM_POWER_STATE_ENUM_UINT8    enPowerState;                           /* Modem的开关机状态 */
    MTC_RATMODE_ENUM_UINT8              enRatMode;                              /* Modem当前的模式类型 */
    VOS_UINT8                           ucUsimValidFlag;                        /* modem当前卡是否有效，1:卡有效；0:卡无效 */

    MTC_MODEM_SERVICE_INFO_STRU         stSrvInfo;                              /* Modem当前服务信息 */
    RRC_MTC_MS_BAND_INFO_STRU           stCurBandInfo;                          /* Modem当前的频段信息 */
    RRC_MTC_MS_BAND_INFO_STRU           stSerCellBandInfo;                      /* Modem当前驻留到的频段信息 */

    MTC_MODEM_NETWORK_INFO_STRU         stNetworkInfo;                          /* Modem当前驻留网络信息 */

    MTC_MODEM_MIPICLK_BITMAP_STRU       stRfCellInfo;                           /* Modem当前的BITMAP状态 */

}MTC_MODEM_INFO_STRU;
typedef struct
{
    MTC_NV_PS_TRANSFER_CFG_STRU         stPsTransferCfg;                        /* 保存PS TRANSFER NV配置信息 */

    MTC_PS_TRANSFER_CAUSE_ENUM_UINT8    enRrcCause;
    MTC_PS_TRANSFER_CAUSE_ENUM_UINT8    enReportCause;
    VOS_UINT8                           aucRsve[2];
}MTC_PS_TRANSFER_CTX_STRU;
typedef struct
{
    VOS_UINT32                          ulAvailNum;                             /* 有效频率个数 */
    VOS_UINT32                          ulMipiClk;                              /* MipiClk值 */
    VOS_UINT32                          aulFreq[MTC_RF_LCD_MIPICLK_FREQ_MAX_NUM];/* ulMipiClk值影响的频率表 */
}MTC_RF_LCD_MIPICLK_FREQ_STRU;
typedef struct
{
    VOS_UINT8                           ucAvailFreqListNum;                     /* 有效的astRfLcdFreqList个数 */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT16                          usMipiClkBitMap;                        /* MIPICLK位图 */
    VOS_UINT32                          ulMipiClkReportModemId;                 /* 保存MipiClk主动上报到那个modem的MTA */
    VOS_UINT16                          usFreqWidth;                            /* 带宽 单位100KHZ */
    VOS_UINT16                          usEnableBitMap;
    MTC_RF_LCD_MIPICLK_FREQ_STRU        astRfLcdFreqList[MTC_RF_LCD_MIPICLK_MAX_NUM];
}MTC_RF_LCD_INTRUSION_CTX_STRU;


typedef struct
{
    MTC_MODEM_INFO_STRU                 astModemInfo[MODEM_ID_BUTT];            /* MTC模块保存Modem状态信息 */

    MTC_INTRUSION_CTX_STRU              stIntrusionCtx;                         /* 干扰控制上下文 */

    MTC_PS_TRANSFER_CTX_STRU            stPsTransferCtx;                        /* PS域迁移上下文 */

    MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8   enOperatorCustSolution;             /* 运营商定制方案 */

    VOS_UINT8                           aucReserved1[3];

    MTC_RF_LCD_INTRUSION_CTX_STRU       stRfLcdIntrusionCtx;

    /* 后续可能会扩展其它功能 */

}MTC_CONTEXT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
MTC_CONTEXT_STRU* MTC_GetCtxAddr(VOS_VOID);

MTC_INTRUSION_CTX_STRU* MTC_GetIntrusionCtxAddr(VOS_VOID);

MTC_MODEM_INFO_STRU* MTC_GetModemCtxAddr(MODEM_ID_ENUM_UINT16 enModemId);

VOS_VOID MTC_InitIntrusionCtx(VOS_VOID);

VOS_UINT32 MTC_IsSupportRatType(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_RATMODE_ENUM_UINT8              enRatMode
);

VOS_VOID MTC_InitModemCtx(VOS_VOID);

VOS_VOID MTC_InitCtx(VOS_VOID);

MTC_CFG_ENUM_UINT8 MTC_GetIntrusionCfg(VOS_VOID);

VOS_VOID MTC_SetModemPowerState(MODEM_ID_ENUM_UINT16 enModemId, MTC_MODEM_POWER_STATE_ENUM_UINT8 enState);

MTC_MODEM_POWER_STATE_ENUM_UINT8 MTC_GetModemPowerState(MODEM_ID_ENUM_UINT16 enModemId);

VOS_VOID MTC_SetModemRatMode(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_RATMODE_ENUM_UINT8              enRatMode
);

MTC_RATMODE_ENUM_UINT8 MTC_GetModemRatMode(MODEM_ID_ENUM_UINT16 enModemId);

VOS_VOID MTC_SetModemCallSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
);

VOS_VOID MTC_SetModemSmsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
);

VOS_VOID MTC_SetModemSsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
);

MTC_CS_SERVICE_STATE_ENUM_UINT8 MTC_GetModemCsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId
);

RRC_MTC_MS_BAND_INFO_STRU* MTC_GetModemCurBandInfo(
    MODEM_ID_ENUM_UINT16                enModemId
);

VOS_UINT8 MTC_GetModemUsimValidFlag(MODEM_ID_ENUM_UINT16 enModemId);
VOS_VOID MTC_SetModemUsimValidFlag(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucUsimValidStatus
);

VOS_VOID MTC_ResetModemInfo(MODEM_ID_ENUM_UINT16 enModemId);

VOS_VOID MTC_SetNotchEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag);
PS_BOOL_ENUM_UINT8 MTC_GetNotchEnalbeFlag(VOS_VOID);
MTC_CFG_ENUM_UINT8 MTC_GetNotchCfg(VOS_VOID);

RRC_MTC_MS_BAND_INFO_STRU* MTC_GetModemSerCellBandInfo(
    MODEM_ID_ENUM_UINT16                enModemId
);
MTC_CFG_ENUM_UINT8 MTC_GetTlRfCtrlCfg(VOS_VOID);
VOS_VOID MTC_SetTlRfCtrlEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag);
PS_BOOL_ENUM_UINT8 MTC_GetTlRfCtrlEnalbeFlag(VOS_VOID);

VOS_VOID MTC_InitPsTransferCtx(VOS_VOID);

VOS_VOID MTC_InitOperatorCustSolution(VOS_VOID);

MTC_PS_TRANSFER_ENUM_UINT8 MTC_GetPsTransferCfg(VOS_VOID);

MTC_PS_TRANSFER_CTX_STRU* MTC_GetPsTransferCtxAddr(VOS_VOID);

MTC_MODEM_NETWORK_INFO_STRU* MTC_GetModemNetworkInfoAddr(
    MODEM_ID_ENUM_UINT16                enModemId);

MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8 MTC_GetOperatorCustSolution(VOS_VOID);

VOS_VOID MTC_SetOperatorCustSolution(
    MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8   enOperatorCustSolution);


VOS_VOID MTC_SetNarrowDcsEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag);
PS_BOOL_ENUM_UINT8 MTC_GetNarrowDcsEnalbeFlag(VOS_VOID);
MTC_CFG_ENUM_UINT8 MTC_GetNarrowBandDcsCfg(VOS_VOID);

MTC_RF_LCD_INTRUSION_CTX_STRU* MTC_GetRfLcdIntrusionCtxAddr(VOS_VOID);
MTC_CFG_ENUM_UINT8 MTC_GetRfLcdIntrusionCfg(VOS_VOID);
VOS_VOID MTC_InitRfLcdIntrusionCtx(VOS_VOID);
VOS_VOID MTC_ReadRfLcdCfgNv(VOS_VOID);
VOS_UINT32 MTC_GetMipiClkRcvPid(VOS_VOID);
MTC_MODEM_MIPICLK_BITMAP_STRU* MTC_GetRfCellInfoAddr(
    MODEM_ID_ENUM_UINT16                enModemId
);
VOS_UINT16 MTC_GetMipiClkBitMap(VOS_VOID);
VOS_VOID MTC_InitRfCellInfo(MODEM_ID_ENUM_UINT16 enModemId);
MTC_RF_LCD_INTRUSION_CTX_STRU* MTC_GetRfLcdIntrusionCtxAddr(VOS_VOID);

MTC_CFG_ENUM_UINT8 MTC_GetRseSupportCfg(VOS_VOID);

PS_BOOL_ENUM_UINT8 MTC_GetRseCfgValue(VOS_VOID);

VOS_VOID MTC_SetRseCfgValue(PS_BOOL_ENUM_UINT8 enValue);





#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of MtcCtx.h */
