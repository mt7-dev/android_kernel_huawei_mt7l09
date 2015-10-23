
#ifndef _TAF_SDC_CTX_H_
#define _TAF_SDC_CTX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "PsTypeDef.h"
#include  "ComInterface.h"

#include  "product_config.h"
#include  "NVIM_Interface.h"

#include  "NasNvInterface.h"

#include "omringbuffer.h"

#include "TafNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TAF_SDC_PLATFORM_MAX_RAT_NUM                        (7)                 /* 平台支持的最大接入技术数目 */

#if (FEATURE_ON == FEATURE_LTE)
#define TAF_SDC_MAX_RAT_NUM                             (3)                     /* 当前支持的接入技术个数 */
#else
#define TAF_SDC_MAX_RAT_NUM                             (2)                     /* 当前支持的GU接入技术个数 */
#endif


#define TAF_SDC_MAX_IMSI_LEN                                (9)                 /* IMSI的最大长度 */
#define TAF_SDC_IMSI_INVALID                                (0xFF)              /* IMSI的无效值的宏定义     */
#define TAF_SDC_INVALID_MCC                                 (0xFFFFFFFF)        /* MCC的无效值 */
#define TAF_SDC_INVALID_MNC                                 (0xFFFFFFFF)        /* MNC的无效值 */
#define TAF_SDC_RAC_INVALID                                 (0xFF)              /* RAC的为无效值 */
#define TAF_SDC_CELLID_INVALID                              (0xFFFFFFFF)
#define TAF_SDC_LAC_INVALID                                 (0xFFFF)
#define TAF_SDC_IMSI_INVALID                                (0xFF)                  /* IMSI的无效值的宏定义     */

#define TAF_SDC_RPT_CFG_MAX_SIZE                            (8)                 /* 主动上报标识最大的长度 */
#define TAF_SDC_MAX_SIGN_THRESHOLD                          (5)                 /* RSSI/CERSSI最大上报门限值:5DB */


#define TAF_SDC_ASCII2BCDLENGTH(ulAsciiLength)              ((ulAsciiLength + 1) / 2)
#define TAF_SDC_BITS_PER_OCTET                              (8)
#define TAF_SDC_BITS_PER_SEPTET                             (7)
#define TAF_SDC_USSD_7BIT_PAD                               (0x1a)

#define TAF_SDC_EMER_NUM_MAX_LENGTH                         (46)                             /* Emergency Number information的最大字节数 */
#define TAF_SDC_EMER_NUM_LISTS_MAX_NUMBER                   (16)
#define TAF_SDC_NUMOF_EMER_NUMS_NO_USIM                     (8)
#define TAF_SDC_NUMOF_EMER_NUMS_WITH_USIM                   (2)
#define TAF_SDC_MAX_CUSTOM_ECC_NUM                          (20)                             /* 用户定制的紧急呼号码最大条数 */
#define TAF_SDC_MAX_BCD_NUM_LEN                             (20)                             /* 该宏定义的取值与MN_CALL_MAX_BCD_NUM_LEN的取值保持一致 */

#define TI_TAF_SDC_WAIT_IMSA_IMS_VOICE_AVAIL_IND_LEN         (35*1000)
#define TAF_SDC_ONE_THOUSAND_MILLISECOND                     (1000)                         /* 1000MS */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


enum TAF_SDC_USSD_TRAN_MODE_ENUM
{
    TAF_SDC_USSD_NON_TRAN_MODE,                                     /* 非透传:单板支持编解码 */
    TAF_SDC_USSD_TRAN_MODE,                                         /* 透传:  单板不编解码，只是透传，由后台来编解码 */
    TAF_SDC_USSD_TRAN_MODE_BUTT
};
typedef VOS_UINT8 TAF_SDC_USSD_TRAN_MODE_ENUM_UINT8;


enum TAF_SDC_ALPHA_TO_ASCII_TRAN_TABLE_ENUM
{
    TAF_SDC_ALPHA_TO_ASCII_TRAN_TABLE_DEF   = 0X0,                          /* 默认的字符表转换，*/
    TAF_SDC_ALPHA_TO_ASCII_TRAN_TABLE_TME,                                  /* TEM字符表转换*/
    TAF_SDC_ALPHA_TO_ASCII_TRAN_TABLE_BUTT
};
typedef VOS_UINT8 TAF_SDC_ALPHA_TO_ASCII_TRAN_TABLE_ENUM_UINT8;


enum TAF_SDC_PLATFORM_RAT_TYPE_ENUM
{
    TAF_SDC_PLATFORM_RAT_GSM   = 0X0,
    TAF_SDC_PLATFORM_RAT_WCDMA,
    TAF_SDC_PLATFORM_RAT_LTE,
    TAF_SDC_PLATFORM_RAT_TDS,
    TAF_SDC_PLATFORM_RAT_BUTT
};
typedef VOS_UINT8 TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8;


enum TAF_SDC_INIT_CTX_TYPE_ENUM
{
    TAF_SDC_INIT_CTX_STARTUP                    = 0,
    TAF_SDC_INIT_CTX_POWEROFF                   = 1,
    TAF_SDC_INIT_CTX_BUTT
};
typedef VOS_UINT8 TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8;



enum TAF_SDC_SYS_SUBMODE_ENUM
{
    TAF_SDC_SYS_SUBMODE_NONE                = 0,                                /* 无服务 */
    TAF_SDC_SYS_SUBMODE_GSM                 = 1,                                /* GSM模式 */
    TAF_SDC_SYS_SUBMODE_GPRS                = 2,                                /* GPRS模式 */
    TAF_SDC_SYS_SUBMODE_EDGE                = 3,                                /* EDGE模式 */
    TAF_SDC_SYS_SUBMODE_WCDMA               = 4,                                /* WCDMA模式 */
    TAF_SDC_SYS_SUBMODE_HSDPA               = 5,                                /* HSDPA模式 */
    TAF_SDC_SYS_SUBMODE_HSUPA               = 6,                                /* HSUPA模式 */
    TAF_SDC_SYS_SUBMODE_HSDPA_HSUPA         = 7,                                /* HSDPA+HSUPA模式 */
    TAF_SDC_SYS_SUBMODE_TD_SCDMA            = 8,                                /* TD_SCDMA模式 */
    TAF_SDC_SYS_SUBMODE_HSPA_PLUS           = 9,                                /* HSPA+模式 */
    TAF_SDC_SYS_SUBMODE_LTE                 = 10,                               /* LTE模式 */
    TAF_SDC_SYS_SUBMODE_DC_HSPA_PLUS        = 17,                               /* DC-HSPA+模式 */
    TAF_SDC_SYS_SUBMODE_DC_MIMO             = 18,                               /* MIMO-HSPA+模式 */
    TAF_SDC_SYS_SUBMODE_BUTT
};
typedef VOS_UINT8  TAF_SDC_SYS_SUBMODE_ENUM_UINT8;
enum TAF_SDC_SYS_MODE_ENUM
{
    TAF_SDC_SYS_MODE_GSM,
    TAF_SDC_SYS_MODE_WCDMA,
    TAF_SDC_SYS_MODE_LTE,
    TAF_SDC_SYS_MODE_BUTT
};
typedef VOS_UINT8  TAF_SDC_SYS_MODE_ENUM_UINT8;


enum TAF_SDC_SERVICE_STATUS_ENUM
{
    TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE           = 0,                        /* 正常服务 */
    TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE          = 1,                        /* 限制服务 */
    TAF_SDC_SERVICE_STATUS_LIMITED_SERVICE_REGION   = 2,                        /* 有限制区域服务 */
    TAF_SDC_SERVICE_STATUS_NO_IMSI                  = 3,                        /* 无imsi */
    TAF_SDC_SERVICE_STATUS_NO_SERVICE               = 4,                        /* 无服务 */
    TAF_SDC_SERVICE_STATUS_NO_CHANGE                = 5,
    TAF_SDC_SERVICE_STATUS_DEEP_SLEEP               = 6,
    TAF_SDC_SERVICE_STATUS_BUTT
};
typedef VOS_UINT8 TAF_SDC_SERVICE_STATUS_ENUM_UINT8;

/*****************************************************************************
    枚举名    : TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM
    枚举说明  : ims的普通注册状态
*****************************************************************************/
enum TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM
{
    TAF_SDC_IMS_NORMAL_REG_STATUS_DEREG        = 0x00,
    TAF_SDC_IMS_NORMAL_REG_STATUS_DEREGING     = 0x01,
    TAF_SDC_IMS_NORMAL_REG_STATUS_REGING       = 0x02,
    TAF_SDC_IMS_NORMAL_REG_STATUS_REG          = 0x03,
    
    TAF_SDC_IMS_NORMAL_REG_STATUS_BUTT         = 0xFF
};
typedef VOS_UINT8 TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM_UINT8;



enum TAF_SDC_RPT_SRVSTA_ENUM
{
    TAF_SDC_REPORT_SRVSTA_NO_SERVICE               = 0,                         /* 无服务 */
    TAF_SDC_REPORT_SRVSTA_LIMITED_SERVICE          = 1,                         /* 限制服务 */
    TAF_SDC_REPORT_SRVSTA_NORMAL_SERVICE           = 2,                         /* 正常服务 */
    TAF_SDC_REPORT_SRVSTA_REGIONAL_LIMITED_SERVICE = 3,                         /* 有限的区域服务 */
    TAF_SDC_REPORT_SRVSTA_DEEP_SLEEP               = 4,                         /* 省电和深睡眠状态 */
    TAF_SDC_REPORT_SRVSTA_BUTT
};
typedef VOS_UINT8  TAF_SDC_REPORT_SRVSTA_ENUM_UINT8;
enum  TAF_SDC_CELL_RESTRICTION_TYPE_ENUM
{
    TAF_SDC_CELL_RESTRICTION_TYPE_NONE                   = 0,                   /* 当前小区不限制任何业务 */
    TAF_SDC_CELL_RESTRICTION_TYPE_NORMAL_SERVICE         = 1,                   /* 当前小区限制正常业务 */
    TAF_SDC_CELL_RESTRICTION_TYPE_EMERGENCY_SERVICE      = 2,                   /* 当前小区限制紧急业务 */
    TAF_SDC_CELL_RESTRICTION_TYPE_ALL_SERVICE            = 3,                   /* 当前小区限制正常业务和紧急业务 */
    TAF_SDC_CELL_RESTRICTION_TYPE_BUTT
};
typedef VOS_UINT8  TAF_SDC_CELL_RESTRICTION_TYPE_ENUM_UINT8;


enum  TAF_SDC_USIM_STATUS_ENUM
{
    TAF_SDC_USIM_STATUS_UNVALID         = 0,                                    /* 初始化卡状态 */
    TAF_SDC_USIM_STATUS_VALID           = 1,                                    /* 卡存在且CS/PS卡都有效 */
    TAF_SDC_USIM_STATUS_CS_UNVALID      = 2,
    TAF_SDC_USIM_STATUS_PS_UNVALID      = 3,
    TAF_SDC_USIM_STATUS_CS_PS_UNVALID   = 4,
    TAF_SDC_USIM_STATUS_SIM_PIN         = 5,                                    /* SIM卡服务由于PIN码原因不可用 */
    TAF_SDC_USIM_STATUS_SIM_PUK         = 6,                                    /* SIM卡服务由于PUK码原因不可用 */
    TAF_SDC_USIM_STATUS_NET_LCOK        = 7,                                    /* SIM卡服务由于网络锁定原因不可用 */
    TAF_SDC_USIM_STATUS_IMSI_LCOK       = 8,                                    /* SIM卡服务由于IMSI锁定原因不可用 */

    TAF_SDC_USIM_STATUS_READY_TIMEROUT  = 9,                                    /* 等待usim定时器超时原因不可用 */

    TAF_SDC_USIM_STATUS_ROMSIM          = 240,
    TAF_SDC_USIM_STATUS_NO_PRESENT      = 255,
    TAF_SDC_USIM_STATUS_BUTT            = 254
};

typedef VOS_UINT8  TAF_SDC_USIM_STATUS_ENUM_UINT8;
enum TAF_SDC_SIM_TYPE_ENUM
{
    TAF_SDC_SIM_TYPE_SIM                , /* SIM卡 */
    TAF_SDC_SIM_TYPE_USIM               , /* USIM卡 */
    TAF_SDC_SIM_TYPE_ROM_SIM,
    TAF_SDC_SIM_TYPE_BUTT
};
typedef VOS_UINT8 TAF_SDC_SIM_TYPE_ENUM_UINT8;




enum TAF_SDC_SERVICE_DOMAIN_ENUM
{
    TAF_SDC_SERVICE_DOMAIN_NO_DOMAIN = 0,                                       /* 无服务 */
    TAF_SDC_SERVICE_DOMAIN_CS        = 1,                                       /* 仅CS服务 */
    TAF_SDC_SERVICE_DOMAIN_PS        = 2,                                       /* 仅PS服务 */
    TAF_SDC_SERVICE_DOMAIN_CS_PS     = 3,                                       /* PS+CS服务 */
    TAF_SDC_SERVICE_DOMAIN_SEARCHING = 4,                                       /* CS、PS均未注册，并处于搜索状态 */
    TAF_SDC_SERVICE_DOMAIN_BUTT
};
typedef VOS_UINT8  TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8;
enum TAF_SDC_REG_STATUS_ENUM
{
    TAF_SDC_REG_NOT_REGISTERED_NOT_SEARCH,                                      /* 未注册未搜索 */
    TAF_SDC_REG_REGISTERED_HOME_NETWORK,                                        /* 注册，注册在 HOME PLMN */
    TAF_SDC_REG_NOT_REGISTERED_SEARCHING,                                       /* 未注册，正在搜索PLMN或者正在附着过程 */
    TAF_SDC_REG_REGISTERED_DENIED,                                              /* 注册被拒绝，当前用户不可以启动注册，启动服务被禁止 */
    TAF_SDC_REG_UNKNOWN,                                                        /* 其余情况 */
    TAF_SDC_REG_REGISTERED_ROAM,                                                /* 注册，注册在非 HOME PLMN */
    TAF_SDC_REG_STATUS_BUTT
};
typedef VOS_UINT8 TAF_SDC_REG_STATUS_ENUM_UINT8;



enum TAF_SDC_RPT_CMD_INDEX_ENUM
{
    TAF_SDC_RPT_CMD_MODE                     = 0,
    TAF_SDC_RPT_CMD_RSSI,
    TAF_SDC_RPT_CMD_REJINFO,
    TAF_SDC_RPT_CMD_SRVST,
    TAF_SDC_RPT_CMD_PLMNSELEINFO,
    TAF_SDC_RPT_CMD_SIMST,
    TAF_SDC_RPT_CMD_TIME,
    TAF_SDC_RPT_CMD_SMMEMFULL,
    TAF_SDC_RPT_CMD_CTZV,
    TAF_SDC_RPT_CMD_DSFLOWRPT,
    TAF_SDC_RPT_CMD_ORIG,
    TAF_SDC_RPT_CMD_CONF,
    TAF_SDC_RPT_CMD_CONN,
    TAF_SDC_RPT_CMD_CEND,
    TAF_SDC_RPT_CMD_STIN,
    TAF_SDC_RPT_CMD_CERSSI,
    TAF_SDC_RPT_CMD_ANLEVEL,
    TAF_SDC_RPT_CMD_LWCLASH,
    TAF_SDC_RPT_CMD_XLEMA,
    TAF_SDC_RPT_CMD_ACINFO,
    TAF_SDC_RPT_CMD_PLMN,
    TAF_SDC_RPT_CMD_CALLSTATE,

    /* 补充单个命令主动上报的枚举 */
    TAF_SDC_RPT_CMD_CREG,
    TAF_SDC_RPT_CMD_CGREG,
    TAF_SDC_RPT_CMD_CEREG,
    TAF_SDC_RPT_CMD_CTZR,
    TAF_SDC_RPT_CMD_CUSD,
    TAF_SDC_RPT_CMD_CSSI,
    TAF_SDC_RPT_CMD_CSSU,
    TAF_SDC_RPT_CMD_LWURC,
    TAF_SDC_RPT_CMD_CUUS1I,
    TAF_SDC_RPT_CMD_CUUS1U,
    TAF_SDC_RPT_CMD_NCELLMONITOR,

    TAF_SDC_RPT_CMD_MIPICLK,

    TAF_SDC_RPT_CMD_BUTT
};
typedef VOS_UINT8 TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8;
enum TAF_SDC_CALL_STATE_RPT_STATUS_ENUM
{
    TAF_SDC_CALL_STATE_RPT_STATUS_DISABLE,                 /* 不允许主动上报 */
    TAF_SDC_CALL_STATE_RPT_STATUS_ENABLE,                  /* 允许主动上报 */
    TAF_SDC_CALL_STATE_RPT_STATUS_BUTT
};
typedef VOS_UINT8 TAF_SDC_CALL_STATE_RPT_STATUS_ENUM_UINT8;


enum TAF_SDC_CMD_RPT_CTRL_TYPE_ENUM
{
    TAF_SDC_CMD_RPT_CTRL_BY_CURC,               /* 主动上报受CURC控制 */
    TAF_SDC_CMD_RPT_CTRL_BY_UNSOLICITED,        /* 主动上报受单独的命令控制 */
    TAF_SDC_CMD_RPT_CTRL_BUTT
};
typedef VOS_UINT8 TAF_SDC_CMD_RPT_CTRL_TYPE_ENUM_UINT8;


enum TAF_SDC_STATUS_RPT_GENERAL_CTRL_TYPE_ENUM
{
    TAF_SDC_STATUS_RPT_GENERAL_CONTROL_TYPE_NO_REPORT = 0,
    TAF_SDC_STATUS_RPT_GENERAL_CONTROL_TYPE_REPORT    = 1,
    TAF_SDC_STATUS_RPT_GENERAL_CONTROL_TYPE_CUSTOM    = 2,
    TAF_SDC_STATUS_RPT_GENERAL_CONTROL_TYPE_BUTT
};
typedef VOS_UINT8 TAF_SDC_STATUS_RPT_GENERAL_CTRL_TYPE_ENUM_UINT8;


/*****************************************************************************
 枚举名    : TAF_SDC_NW_IMS_VOICE_CAP_ENUM_UINT8
 结构说明  : IMS voice capability
 1.日    期   : 2013年7月13日
   作    者   : y0024524
   修改内容   : 新建
*****************************************************************************/
enum TAF_SDC_NW_IMS_VOICE_CAP_ENUM
{
    TAF_SDC_NW_IMS_VOICE_NOT_SUPPORTED    = 0,
    TAF_SDC_NW_IMS_VOICE_SUPPORTED        = 1,

    TAF_SDC_NW_IMS_VOICE_BUTT
};
typedef VOS_UINT8 TAF_SDC_NW_IMS_VOICE_CAP_ENUM_UINT8;


enum TAF_SDC_NW_EMC_BS_CAP_ENUM
{
    TAF_SDC_NW_EMC_BS_NOT_SUPPORTED       = 0,
    TAF_SDC_NW_EMC_BS_SUPPORTED           = 1,

    TAF_SDC_NW_EMC_BS_BUTT
};
typedef VOS_UINT8 TAF_SDC_NW_EMC_BS_CAP_ENUM_UINT8;


enum TAF_SDC_LTE_CS_CAPBILITY_ENUM
{
    TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO  = 0,
    TAF_SDC_LTE_CS_CAPBILITY_CSFB_NOT_PREFER   = 1,
    TAF_SDC_LTE_CS_CAPBILITY_SMS_ONLY          = 2,
    TAF_SDC_LTE_CS_CAPBILITY_NOT_SUPPORTED     = 3,

    TAF_SDC_LTE_CS_CAPBILITY_BUTT
};
typedef VOS_UINT8 TAF_SDC_LTE_CS_CAPBILITY_ENUM_UINT8;


enum TAF_SDC_VOICE_DOMAIN_ENUM
{
    TAF_SDC_VOICE_DOMAIN_CS_ONLY            = 0,                                /**< CS voice only */
    TAF_SDC_VOICE_DOMAIN_IMS_PS_ONLY        = 1,                                /**< IMS PS voice only */
    TAF_SDC_VOICE_DOMAIN_CS_PREFERRED       = 2,                                /**< CS voice preferred, IMS PS Voice as secondary */
    TAF_SDC_VOICE_DOMAIN_IMS_PS_PREFERRED   = 3,                                /**< IMS PS voice preferred, CS Voice as secondary */

    TAF_SDC_VOICE_DOMAIN_BUTT
};
typedef VOS_UINT32 TAF_SDC_VOICE_DOMAIN_ENUM_UINT32;


enum TAF_SDC_SMS_DOMAIN_ENUM
{
    TAF_SDC_SMS_DOMAIN_NOT_USE_SMS_OVER_IP                  = 0,                /**< not to use SMS over ip */
    TAF_SDC_SMS_DOMAIN_PREFER_TO_USE_SMS_OVER_IP            = 1,                /**< prefer to use SMS over ip, CS/PS SMS as secondary */

    TAF_SDC_SMS_DOMAIN_BUTT
};
typedef VOS_UINT8 TAF_SDC_SMS_DOMAIN_ENUM_UINT8;


enum TAF_SDC_LTE_CS_SERVICE_ENUM
{
    TAF_SDC_LTE_SUPPORT_CSFB_AND_SMS_OVER_SGS = 1,                              /* 支持cs fallback和sms over sgs*/
    TAF_SDC_LTE_SUPPORT_SMS_OVER_SGS_ONLY,                                      /* 支持sms over sgs only*/
    TAF_SDC_LTE_SUPPORT_1XCSFB,                                                 /* 支持1XCSFB */
    TAF_SDC_LTE_SUPPORT_BUTT
};
typedef VOS_UINT8 TAF_SDC_LTE_CS_SERVICE_ENUM_UINT8;
enum TAF_SDC_UE_USAGE_SETTING_ENUM
{
    TAF_SDC_UE_USAGE_VOICE_CENTRIC      = 0,                                    /* 语音中心 */
    TAF_SDC_UE_USAGE_DATA_CENTRIC,                                              /* 数据中心 */

    TAF_SDC_UE_USAGE_BUTT
};
typedef VOS_UINT8 TAF_SDC_UE_USAGE_SETTING_ENUM_UINT8;


enum TAF_SDC_PHONE_MODE_ENUM
{
    TAF_SDC_PHONE_MODE_MINI                = 0,   /* minimum functionality*/
    TAF_SDC_PHONE_MODE_FULL                = 1,   /* full functionality */
    TAF_SDC_PHONE_MODE_TXOFF               = 2,   /* disable phone transmit RF circuits only */
    TAF_SDC_PHONE_MODE_RXOFF               = 3,   /* disable phone receive RF circuits only */
    TAF_SDC_PHONE_MODE_RFOFF               = 4,   /* disable phone both transmit and receive RF circuits */
    TAF_SDC_PHONE_MODE_FT                  = 5,   /* factory-test functionality */
    TAF_SDC_PHONE_MODE_RESET               = 6,   /* reset */
    TAF_SDC_PHONE_MODE_VDFMINI             = 7,   /* mini mode required by VDF*/
    TAF_SDC_PHONE_MODE_POWEROFF            = 8,   /* 关机下电模式 */
    TAF_SDC_PHONE_MODE_LOWPOWER            = 9,
    TAF_SDC_PHONE_MODE_BUTT 
};
typedef VOS_UINT8 TAF_SDC_PHONE_MODE_ENUM_UINT8;


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
    VOS_UINT8                                     ucRatNum;
    TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8          aenRatList[TAF_SDC_PLATFORM_MAX_RAT_NUM];
}TAF_SDC_PLATFORM_RAT_CAP_STRU;


typedef struct
{
    VOS_UINT8                           ucRatNum;                          
    TAF_SDC_SYS_MODE_ENUM_UINT8         aenRatPrio[TAF_SDC_MAX_RAT_NUM];    
}TAF_SDC_RAT_PRIO_STRU;



typedef struct
{
    VOS_UINT8                           ucGsmImsSupportFlag;    /**< GSM IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUtranImsSupportFlag;  /**< UNTRAN IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucLteImsSupportFlag;    /**< LTE IMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucGsmEmsSupportFlag;    /**< GSM EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUtranEmsSupportFlag;  /**< UNTRAN EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucLteEmsSupportFlag;    /**< LTE EMS使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           aucReserved[2];
}TAF_SDC_IMS_RAT_SUPPORT_STRU;


typedef struct
{
    VOS_UINT8                           ucVoiceCallOnImsSupportFlag;            /* IMS语音使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucSmsOnImsSupportFlag;                  /* IMS短信使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucVideoCallOnImsSupportFlag;            /* IMS域VIDEO CALL使能项,VOS_TRUE :支持，VOS_FALSE :不支持 */
    VOS_UINT8                           ucUssdOnImsSupportFlag;                 /* IMS USSD业务使能项，VOS_TRUE :支持，VOS_FALSE :不支持 */
}TAF_SDC_IMS_CAPABILITY_STRU;


typedef struct
{
    VOS_UINT8                           ucCallRedial;                           /* call重拨flag */
    VOS_UINT8                           ucSmsRedial;                            /* SMS 重拨flag */
    VOS_UINT8                           ucSsRedial;                             /* SS  重拨flag */
    VOS_UINT8                           aucReserve[1];
}TAF_SDC_SWITCH_DOMAIN_REDIAL_CONFIG_STRU;
typedef struct
{
    TAF_SDC_SWITCH_DOMAIN_REDIAL_CONFIG_STRU                stRedialImsToCs;                        /* IMS to CS重拨数据结构  */
    TAF_SDC_SWITCH_DOMAIN_REDIAL_CONFIG_STRU                stRedialCsToIms;                        /* CS to IMS 重拨数据结构 */
}TAF_SDC_SWITCH_DOMAIN_REDIAL_STRU;


typedef struct
{
    TAF_SDC_IMS_RAT_SUPPORT_STRU        stImsRatSupport;
    TAF_SDC_IMS_CAPABILITY_STRU         stImsCapability;
    TAF_SDC_LTE_CS_SERVICE_ENUM_UINT8   enLteCsServiceCfg;                      /* L模支持的cs域业务能力的类型 */
    TAF_SDC_SMS_DOMAIN_ENUM_UINT8       enSmsDomain;                            /* sms domain preferrece */
    VOS_UINT8                           ucRoamingSupport;
    VOS_UINT8                           aucReserved[1];
    TAF_SDC_SWITCH_DOMAIN_REDIAL_STRU   stRedial;                               /* IMS<-->CS换域重拨标志 */
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32    enVoiceDomain;                          /* voice domain preferrece */

    VOS_UINT32                          ulWaitImsVoiceAvailTimerLen;            /* 等待IMS VOICE 可用的定时器时长 */    
}TAF_SDC_IMS_CONFIG_PARA_STRU;
typedef struct
{
    VOS_UINT8                      ucCategoryValue;                                 /* Emergency Service Category Value         */
    VOS_UINT8                      ucEmcNumLen;                                     /* 紧急呼号码长度，即aucEmergencyList的有效长度*/
    VOS_UINT8                      aucEmergencyList[TAF_SDC_EMER_NUM_MAX_LENGTH];   /* 紧急呼列表 */
}TAF_SDC_EMERGENCY_CONTENT_STRU;
typedef struct
{
    VOS_UINT32                         ulMcc;                                               /* MCC */
    VOS_UINT8                          ucEmergencyNumber;                                   /* 紧急呼个数 */
    VOS_UINT8                          aucReserved[3];
    TAF_SDC_EMERGENCY_CONTENT_STRU     astEmergencyLists[TAF_SDC_EMER_NUM_LISTS_MAX_NUMBER];/* 紧急呼列表 */
}TAF_SDC_MM_EMERGENCY_LIST_STRU;
typedef struct
{
    VOS_UINT8                           ucCategory;                             /* 紧急呼号码类型 */
    VOS_UINT8                           ucValidSimPresent;                      /* 紧急呼号码是有卡时有效还是无卡时有效，1有卡时有效，0无卡时有效 */
    VOS_UINT8                           ucReserved;
    VOS_UINT8                           ucEccNumLen;                            /* 紧急呼号码长度 */
    VOS_UINT8                           aucEccNum[TAF_SDC_MAX_BCD_NUM_LEN];     /* 紧急呼号码 */
    VOS_UINT32                          ulMcc;                                  /* MCC,3 bytes */
} TAF_SDC_CUSTOM_ECC_NUM_STRU;
typedef struct
{
    VOS_UINT8                           ucEccNumCount;                                  /* 紧急号个数 */
    VOS_UINT8                           aucReseve[3];                                   
    TAF_SDC_CUSTOM_ECC_NUM_STRU         astCustomEccNumList[TAF_SDC_MAX_CUSTOM_ECC_NUM];/* 紧急呼列表 */
} TAF_SDC_CUSTOM_ECC_NUM_LIST_STRU;


typedef struct
{
    VOS_UINT8                               ucCustomSetFlg;                     /* VOS_TRUE,标书APP正在设置定制紧急呼号码，VOS_FALSE,表示没有开始定制 */
    VOS_UINT8                               aucReserve[3];
    TAF_SDC_CUSTOM_ECC_NUM_LIST_STRU        stCustomEccNumList;                 /* APP定制的紧急呼列表 */
} TAF_SDC_CUSTOM_ECC_CTX_STRU;


typedef struct
{
    VOS_UINT8                                     ucLCEnableFlg;
    TAF_NVIM_LC_RAT_COMBINED_ENUM_UINT8           enRatCombined;
    TAF_NVIM_LC_WORK_CFG_ENUM_UINT8               enLCWorkCfg;
    VOS_UINT8                                     aucReserved[1];
}TAF_SDC_LC_CONFIG_PARA_STRU;


typedef struct
{
    TAF_SDC_PLATFORM_RAT_CAP_STRU       stPlatformRatCap;                       /* 平台接入技术能力信息 */

    VOS_UINT8                           ucSvlteSupportFlag;                     /* 是否支持SVLTE标志 */
    VOS_UINT8                           aucReserved[3];

    TAF_SDC_IMS_CONFIG_PARA_STRU       stImsConfigPara;                       /* IMS相关配置信息 */

    TAF_SDC_LC_CONFIG_PARA_STRU        stLcConfigPara;

}TAF_SDC_MS_CAP_INFO_STRU;
typedef struct
{
    VOS_UINT32                          ulMcc;
    VOS_UINT32                          ulMnc;
} TAF_SDC_PLMN_ID_STRU;


typedef struct
{
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode;
    TAF_SDC_SYS_SUBMODE_ENUM_UINT8      enSysSubMode;
    VOS_UINT16                          usLac;
    TAF_SDC_PLMN_ID_STRU                stPlmnId;
    VOS_UINT8                           ucRac;
    VOS_UINT8                           ucRoamFlag;                     /* 当前驻留网络是否漫游 VOS_TRUE:漫游网络 VOS_FALSE:非漫游网络 */
    VOS_UINT8                           ucCampOnFlg;                    /* 当前是否驻留,收到系统消息认为驻留,收到搜网或丢网指示认为未驻留 */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulCellId;
} TAF_SDC_CAMP_PLMN_INFO_STRU;


typedef struct
{
    TAF_SDC_CELL_RESTRICTION_TYPE_ENUM_UINT8    enCellAcType;                   /* 小区受限类型 */
    VOS_UINT8                                   ucRestrictRegister;             /* 是否限制注册VOS_TRUE:限制；VOS_FALSE:不限制 */
    VOS_UINT8                                   ucRestrictPagingRsp;            /* 是否限制响应寻呼VOS_TRUE:限制；VOS_FALSE:不限制 */
    VOS_UINT8                                   ucReserved;
}TAF_SDC_ACCESS_RESTRICTION_STRU;
typedef struct
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8      enCsServiceStatus;                   /* cs服务状态 */
    TAF_SDC_REG_STATUS_ENUM_UINT8          enCsRegStatus;                       /* cs注册状态 */
    VOS_UINT8                              ucSimCsRegStatus;    /* SIM卡CS域的注册结果导致的卡是否有效VOS_TRUE:CS域的卡有效,VOS_FALSE:CS域的卡无效*/

    VOS_UINT8                              ucCsAttachAllowFlg;                  /* cs是否允许注册标识 */

    TAF_SDC_ACCESS_RESTRICTION_STRU        stCsAcRestriction;                   /* 当前CS域接入受限情况 */
}TAF_SDC_CS_DOMAIN_INFO_STRU;


typedef struct
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8      enPsServiceStatus;                                                            /* ps服务状态 */
    TAF_SDC_REG_STATUS_ENUM_UINT8          enPsRegStatus;                       /* ps注册状态 */
    VOS_UINT8                              ucSimPsRegStatus;                    /* SIM卡PS域的注册结果导致的卡是否有效VOS_TRUE:CS域的卡有效,VOS_FALSE:CS域的卡无效*/
    VOS_UINT8                              ucPsAttachAllowFlg;                  /* ps是否允许注册标识 */
	TAF_SDC_ACCESS_RESTRICTION_STRU        stPsAcRestriction;                   /* 当前PS域接入受限情况 */

}TAF_SDC_PS_DOMAIN_INFO_STRU;
typedef struct
{
    TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM_UINT8   enImsNormalRegSta;
    VOS_UINT8                                  ucImsAvail;
    VOS_UINT8                                  aucReserved[2];
}TAF_SDC_IMS_DOMAIN_INFO_STRU;




typedef struct
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSimStatus;
    VOS_UINT8                           aucReserved[3];
}TAF_SDC_SIM_STATUS_STRU;


typedef struct
{
    VOS_UINT8                           aucImsi[TAF_SDC_MAX_IMSI_LEN];
	
	VOS_UINT8                           aucLastImsi[TAF_SDC_MAX_IMSI_LEN];
    VOS_UINT8                           aucReserved[2];
}TAF_SDC_SIM_MS_IDENTITY_STRU;



typedef struct
{
    TAF_SDC_SIM_STATUS_STRU             stSimStatus;                            /* SIM卡状态 */
    TAF_SDC_SIM_MS_IDENTITY_STRU        stMsIdentity;                           /* MS Identity信息 */
    TAF_SDC_SIM_TYPE_ENUM_UINT8         enSimType;                              /* SIM卡类型 */
    VOS_UINT8                           aucRsv[3];
}TAF_SDC_SIM_INFO_STRU;
typedef struct
{
    TAF_SDC_STATUS_RPT_GENERAL_CTRL_TYPE_ENUM_UINT8   enStatusRptGeneralControl; /* 控制是否允许所有私有命令的主动上报 */
    VOS_UINT8                                         aucReserved[3];
    VOS_UINT8                                         aucRptCfg[TAF_SDC_RPT_CFG_MAX_SIZE]; /* 64bit主动上报标识 */
}TAF_SDC_CURC_RPT_CTRL_STRU;


typedef struct
{
    VOS_UINT8                                         aucRptCfg[TAF_SDC_RPT_CFG_MAX_SIZE]; /* 64bit主动上报标识 */
}TAF_SDC_UNSOLICITED_RPT_CTRL_STRU;
typedef struct
{
    TAF_SDC_CURC_RPT_CTRL_STRU              stCurcRptCtrl;                      /* CURC控制主动上报 */
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU       stUnsolicitedRptCtrl;               /* 单个命令控制主动上报 */
}TAF_SDC_RPT_CTRL_STRU;



typedef struct
{
    VOS_UINT8                           ucUssdTransMode;
    VOS_UINT8                           ucAlphaTransMode;
    VOS_UINT8                           aucRsv[2];
}TAF_SDC_USSD_CFG_STRU;



typedef struct
{
    VOS_UINT8                                   ucUtranSkipWPlmnSearchFlag;
    VOS_UINT8                                   aucReserved[3];
}TAF_SDC_DSDA_PLMN_SEARCH_ENHANCED_CFG_STRU;

typedef struct
{
    TAF_SDC_RPT_CTRL_STRU               stRptCtrl;                              /* 控制主动上报信息 */
    TAF_SDC_USSD_CFG_STRU               stUssdCfg;                              /* USSD配置信息 */
    VOS_UINT16                          usAppCfgSupportType;                    /*控制应用版本*/
    TAF_SDC_UE_USAGE_SETTING_ENUM_UINT8 enUeUsageSetting;                       /* UE's usage setting */
    VOS_UINT8                           aucReserve[1];
    
    TAF_SDC_RAT_PRIO_STRU               stPrioRatList;                          /* 接入技术以及优先级 */
	
    TAF_SDC_DSDA_PLMN_SEARCH_ENHANCED_CFG_STRU  stDsdaPlmnSearchEnhancedCfg;
}TAF_SDC_MS_SYS_CFG_INFO_STRU;
typedef struct
{
    TAF_SDC_MS_CAP_INFO_STRU            stMsCapInfo;                            /* MS的能力信息 */
    TAF_SDC_MS_SYS_CFG_INFO_STRU        stMsSysCfgInfo;                         /* 系统配置信息 */
}TAF_SDC_MS_CFG_INFO_STRU;


typedef struct
{
    TAF_SDC_NW_IMS_VOICE_CAP_ENUM_UINT8   enNwImsVoCap;
    TAF_SDC_NW_EMC_BS_CAP_ENUM_UINT8      enNwEmcBsCap;
    TAF_SDC_LTE_CS_CAPBILITY_ENUM_UINT8   enLteCsCap;
    VOS_UINT8                             aucReserved[1];
}
TAF_SDC_NETWORK_CAP_INFO_STRU;


typedef struct
{
    TAF_SDC_CAMP_PLMN_INFO_STRU         stCampPlmnInfo;                         /* 当前驻留PLMN信息 */
    TAF_SDC_CS_DOMAIN_INFO_STRU         stCsDomainInfo;                         /* 当前CS域信息 */
    TAF_SDC_PS_DOMAIN_INFO_STRU         stPsDomainInfo;                         /* 当前PS域信息 */
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enReportSrvsta;                         /* UE服务状态^srvst上报的服务状态 */
    TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8   enServiceDomain;                        /* UE服务域sysinfo查询时上报的服务域 */
    VOS_UINT8                           aucReserved[2];
    TAF_SDC_NETWORK_CAP_INFO_STRU       stGuNwCapInfo;                          /* GU下网络业务能力信息 */
    TAF_SDC_NETWORK_CAP_INFO_STRU       stLteNwCapInfo;                         /* LTE下网络业务能力信息 */

    TAF_SDC_IMS_DOMAIN_INFO_STRU        stImsDomainInfo;                        /* IMS域的网络信息 */    

}TAF_SDC_NETWORK_INFO_STRU;



typedef struct
{
    VOS_UINT8                           ucCsCallExistFlg;                       /* 当前是否处于呼叫过程中:VOS_TRUE  当前处于呼叫过程中
                                                                                                  VOS_FALSE 当前不处于呼叫过程中  */
    VOS_UINT8                           ucImsCallExistFlg;                      /* 当前是否处于呼叫过程中:VOS_TRUE  当前处于呼叫过程中
                                                                                                  VOS_FALSE 当前不处于呼叫过程中  */
    /* 删除SRVCC标志，移到CALL模块 */
    VOS_UINT8                           aucReserved[2];

    TAF_SDC_MM_EMERGENCY_LIST_STRU      stMmCallEmerNumList;                    /* MM带下来的紧急呼列表 */
    TAF_SDC_CUSTOM_ECC_CTX_STRU         stCustomCallEccNumCtx;                  /* 运营商定制的紧急呼列表 */

}TAF_SDC_CALL_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucCsSmsSrvExistFlg;                     /* CS域的短信业务存在标识:VOS_TRUE  当前有CS域的短信业务
                                                                                   VOS_FALSE 当前没有CS域的短信业务  */
    VOS_UINT8                           ucPsSmsSrvExistFlg;                     /* PS域的短信业务存在标识:VOS_TRUE  当前有PS域的短信业务
                                                                                   VOS_FALSE 当前没有PS域的短信业务  */
    VOS_UINT8                           ucImsSmsSrvExistFlg;                    /* IMS域的短信业务存在标识:VOS_TRUE  当前有IMS域的短信业务
                                                                                   VOS_FALSE 当前没有IMS域的短信业务  */
    VOS_UINT8                           aucReserved[1];
}TAF_SDC_SMS_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucCsSsSrvExistFlg;                      /* CS域的补充业务存在标识:VOS_TRUE  当前有CS域的补充业务
                                                                                   VOS_FALSE 当前没有CS域的补充业务  */
    VOS_UINT8                           ucImsSsSrvExistFlg;                     /* IMS域的补充业务存在标识:VOS_TRUE  当前有IMS域的补充业务
                                                                                   VOS_FALSE 当前没有IMS域的补充业务  */
    VOS_UINT8                           aucReserved[2];
}TAF_SDC_SS_INFO_STRU;



typedef struct
{
    VOS_UINT8                           ucCsServiceConnStatusFlag;              /* cs业务信令连接是否存在标识 */
    VOS_UINT8                           ucReserved[3];                                                                                             VOS_UINT8                           aucReserved[2];
}TAF_SDC_CS_INFO_STRU;
typedef struct
{
    TAF_SDC_CALL_INFO_STRU              stCallInfo;                               /* CC相关信息 */
    TAF_SDC_SMS_INFO_STRU               stSmsInfo;                              /* SMS相关信息 */
    TAF_SDC_SS_INFO_STRU                stSsInfo;                               /* SS相关信息 */

    TAF_SDC_CS_INFO_STRU                stCsInfo;                                                                                      VOS_UINT8                           aucReserved[2];
}TAF_SDC_SERVICE_INFO_STRU;



typedef struct
{
    /* TAF层的共享缓存 */
    OM_RING_ID                          pstRingBuffer;
    VOS_UINT8                           ucErrLogCtrlFlag;                       /* ERRLOG打开标识 */
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          usAlmLevel;                             /* 故障告警级别 */
}TAF_SDC_ERRLOG_INFO_STRU;
typedef struct
{
    TAF_SDC_PHONE_MODE_ENUM_UINT8       enPhMode;
    VOS_UINT8                           aucRsv[3];
}TAF_SDC_PHONE_INFO_STRU;


typedef struct
{
    TAF_SDC_SIM_INFO_STRU               stSimInfo;                              /* 卡相关信息 */
    TAF_SDC_MS_CFG_INFO_STRU            stMsCfgInfo;                            /* MS的配置信息 */
    TAF_SDC_NETWORK_INFO_STRU           stNetworkInfo;                          /* 当前驻留的网络信息 */
    TAF_SDC_SERVICE_INFO_STRU           stServiceInfo;                          /* 当前业务相关信息 */

    TAF_SDC_PHONE_INFO_STRU             stPhoneInfo;

    TAF_SDC_ERRLOG_INFO_STRU            stErrlogInfo;
}TAF_SDC_CTX_STRU;


typedef struct
{
    VOS_UINT8                           ucSignThreshold;                        /* 信号变化门限,当RSSI变化超过该值，接入层需要主动上报信号质量，取值0表示接入层按默认值处理 */
    VOS_UINT8                           ucMinRptTimerInterval;                  /* 间隔上报的时间   */
} TAF_SDC_CELL_SIGN_REPORT_CFG_STRU;

/* call state NV删除，call state的上报由curc统一控制 */


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
TAF_SDC_CTX_STRU*  TAF_SDC_GetSdcCtx( VOS_VOID );

TAF_SDC_MS_CAP_INFO_STRU*  TAF_SDC_GetMsCapInfo( VOS_VOID );

TAF_SDC_PLATFORM_RAT_CAP_STRU*  TAF_SDC_GetPlatformRatCap( VOS_VOID );

TAF_SDC_CURC_RPT_CTRL_STRU*  TAF_SDC_GetCurcRptCtrl( VOS_VOID );
TAF_SDC_UNSOLICITED_RPT_CTRL_STRU*  TAF_SDC_GetUnsolicitedRptCtrl( VOS_VOID );


TAF_SDC_NETWORK_CAP_INFO_STRU *TAF_SDC_GetGuNwCapInfo(VOS_VOID);


TAF_SDC_NETWORK_CAP_INFO_STRU *TAF_SDC_GetLteNwCapInfo(VOS_VOID);


VOS_VOID TAF_SDC_SetGuNwCapInfo(TAF_SDC_NETWORK_CAP_INFO_STRU *pstNwCapInfo);


VOS_VOID TAF_SDC_SetLteNwCapInfo(TAF_SDC_NETWORK_CAP_INFO_STRU *pstNwCapInfo);

VOS_UINT32 TAF_SDC_IsPlatformRatCapNvimValid(
    PLATAFORM_RAT_CAPABILITY_STRU      *pstNvPlatformRatCap
);

TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8 TAF_SDC_ConvertNvPlatformRatToTafType(
    PLATFORM_RAT_TYPE_ENUM_UINT16       enNvPlatformRatCap
);

VOS_VOID TAF_SDC_InitPlatformRatCap(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    TAF_SDC_PLATFORM_RAT_CAP_STRU       *pstPlatformRatCap
);

VOS_VOID TAF_SDC_InitMsCapInfo(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    TAF_SDC_MS_CAP_INFO_STRU            *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitCtx(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    TAF_SDC_CTX_STRU                    *pstSdcCtx
);

TAF_SDC_SYS_MODE_ENUM_UINT8 TAF_SDC_GetSysMode( VOS_VOID );
VOS_VOID TAF_SDC_SetSysMode(TAF_SDC_SYS_MODE_ENUM_UINT8 enSysMode);
TAF_SDC_SYS_SUBMODE_ENUM_UINT8 TAF_SDC_GetSysSubMode( VOS_VOID );
VOS_VOID TAF_SDC_SetSysSubMode(TAF_SDC_SYS_SUBMODE_ENUM_UINT8 enSysSubMode);
TAF_SDC_CAMP_PLMN_INFO_STRU* TAF_SDC_GetCampPlmnInfo( VOS_VOID );
TAF_SDC_SERVICE_STATUS_ENUM_UINT8 TAF_SDC_GetCsServiceStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetCsServiceStatus(TAF_SDC_SERVICE_STATUS_ENUM_UINT8 enCsServiceStatus);
TAF_SDC_SERVICE_STATUS_ENUM_UINT8 TAF_SDC_GetPsServiceStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetPsServiceStatus(TAF_SDC_SERVICE_STATUS_ENUM_UINT8 enPsServiceStatus);
TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8 TAF_SDC_GetServiceDomain( VOS_VOID );
VOS_VOID TAF_SDC_SetServiceDomain(TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8 enServiceDomain);
TAF_SDC_REG_STATUS_ENUM_UINT8 TAF_SDC_GetCsRegStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetCsRegStatus(TAF_SDC_REG_STATUS_ENUM_UINT8 enCsRegStatus);
TAF_SDC_REG_STATUS_ENUM_UINT8 TAF_SDC_GetPsRegStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetPsRegStatus(TAF_SDC_REG_STATUS_ENUM_UINT8 enPsRegStatus);
VOS_UINT8* TAF_SDC_GetSimImsi( VOS_VOID );
VOS_VOID TAF_SDC_SetSimImsi( VOS_UINT8 *pstImsi );

VOS_UINT8* TAF_SDC_GetLastSimImsi( VOS_VOID );
VOS_VOID TAF_SDC_SetLastSimImsi( VOS_UINT8 *pstImsi );

TAF_SDC_USIM_STATUS_ENUM_UINT8 TAF_SDC_GetSimStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetSimStatus(TAF_SDC_USIM_STATUS_ENUM_UINT8 enSimStatus);
VOS_VOID TAF_SDC_SetSimType(TAF_SDC_SIM_TYPE_ENUM_UINT8 enSimType);
TAF_SDC_SIM_TYPE_ENUM_UINT8 TAF_SDC_GetSimType( VOS_VOID );
VOS_VOID TAF_SDC_SetCurrCampPlmnId(TAF_SDC_PLMN_ID_STRU *pstPlmnId);
TAF_SDC_PLMN_ID_STRU *TAF_SDC_GetCurrCampPlmnId(VOS_VOID);


VOS_UINT16 TAF_SDC_GetCurrCampLac(VOS_VOID);

VOS_UINT8 TAF_SDC_GetCurrCampRac(VOS_VOID);
VOS_UINT32 TAF_SDC_GetCurrCampCellId(VOS_VOID);

TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 TAF_SDC_GetServiceStatus(VOS_VOID);

VOS_VOID TAF_SDC_SetServiceStatus(TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 enReportSrvsta);
VOS_VOID TAF_SDC_SetCurrCampLac(VOS_UINT16 usLac);
VOS_VOID TAF_SDC_SetCurrCampRac(VOS_UINT8 ucRac);
VOS_VOID TAF_SDC_SetCurrCampCellId(VOS_UINT32 ulCellId);
VOS_VOID TAF_SDC_SetCsAcRestriction(TAF_SDC_ACCESS_RESTRICTION_STRU *pstCsAcRetriction);
VOS_VOID TAF_SDC_SetPsAcRestriction(TAF_SDC_ACCESS_RESTRICTION_STRU *pstPsAcRetriction);
TAF_SDC_ACCESS_RESTRICTION_STRU *TAF_SDC_GetCsAcRestriction(VOS_VOID);

TAF_SDC_ACCESS_RESTRICTION_STRU *TAF_SDC_GetPsAcRestriction(VOS_VOID);


VOS_VOID TAF_SDC_InitCurcRptCtrlInfo(VOS_VOID);
VOS_VOID TAF_SDC_InitUnsolicitedRptCtrlInfo(VOS_VOID);
VOS_VOID TAF_SDC_ReadCellSignReportCfgNV( VOS_VOID );

VOS_UINT8   TAF_SDC_GetCsCallExistFlg(VOS_VOID);
VOS_VOID    TAF_SDC_SetCsCallExistFlg(
    VOS_UINT8                           ucInCall
);
VOS_VOID    TAF_SDC_SetImsCallExistFlg(
    VOS_UINT8                           ucImsCallExistFlg
);
VOS_UINT8   TAF_SDC_GetImsCallExistFlg(VOS_VOID);

/* 删除SRVCC标志，移到CALL模块 */

TAF_SDC_CALL_INFO_STRU* TAF_SDC_GetCallInfo(VOS_VOID);
VOS_VOID TAF_SDC_SetMmCallEmerNumList(
    TAF_SDC_MM_EMERGENCY_LIST_STRU     *pstMmCallEmerNumList
);
TAF_SDC_MM_EMERGENCY_LIST_STRU* TAF_SDC_GetMmCallEmerNumList(VOS_VOID);
VOS_VOID TAF_SDC_SetCustomCallEccNumCtx(
    TAF_SDC_CUSTOM_ECC_CTX_STRU        *pstCustomCallEccNumCtx
);
TAF_SDC_CUSTOM_ECC_CTX_STRU* TAF_SDC_GetCustomCallEccNumCtx(VOS_VOID);
VOS_CHAR** TAF_SDC_GetCallEmerNumsNoUsimTblAddr(VOS_VOID);
VOS_CHAR** TAF_SDC_GetCallEmerNumsWithUsimTblAddr(VOS_VOID);



VOS_VOID   TAF_SDC_SetUssdTransMode(
    VOS_UINT8                           ucTransMode
);
VOS_UINT8   TAF_SDC_GetUssdTransMode(VOS_VOID);

VOS_VOID TAF_SDC_InitUssdCfgInfo(VOS_VOID);

VOS_VOID TAF_SDC_ReadUssdTransModeNvim(VOS_VOID);
VOS_VOID TAF_SDC_ReadUssdTransTableNvim(VOS_VOID);

VOS_VOID TAF_SDC_InitServiceInfo(
    TAF_SDC_SERVICE_INFO_STRU          *pstServiceInfo
);

VOS_UINT8 TAF_SDC_GetCsServiceConnStatusFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetCsServiceConnStatusFlag(
    VOS_UINT8                           ucCsServiceConnStatusFlag
);


VOS_VOID TAF_SDC_InitCallInfo(
    TAF_SDC_CALL_INFO_STRU             *pstCallInfo
);
VOS_VOID TAF_SDC_ReadCustomEccNumListNvim(VOS_VOID);



VOS_VOID TAF_SDC_InitAppCfgSupportType(VOS_VOID);

VOS_VOID TAF_SDC_InitGuNwCapInfo(
    TAF_SDC_NETWORK_CAP_INFO_STRU *pstGuNwCapInfo
);
VOS_VOID TAF_SDC_InitLteNwCapInfo(
    TAF_SDC_NETWORK_CAP_INFO_STRU *pstLteNwCapInfo
);
VOS_VOID TAF_SDC_InitNetWorkInfo(
    TAF_SDC_NETWORK_INFO_STRU *pstNetworkInfo
);


VOS_VOID TAF_SDC_InitUeUsageSetting(VOS_VOID);

VOS_UINT8   TAF_SDC_Get7BitToAssicTableType(VOS_VOID);
VOS_VOID    TAF_SDC_Set7BitToAssicTableType(
    VOS_UINT8                           ucType
);

VOS_UINT8*   TAF_SDC_GetCurr7BitToAsciiTableAddr(VOS_VOID);

VOS_VOID TAF_SDC_GetCurrentLai(
    VOS_UINT8                          *aucLai,
    VOS_UINT32                         *pulLaiLen
);



VOS_VOID    TAF_SDC_SetSvlteSupportFlag(
    VOS_UINT8                           ucSvlteSupportFlag
);
VOS_UINT8   TAF_SDC_GetSvlteSupportFlag(VOS_VOID);
VOS_VOID    TAF_SDC_SetCsSmsSrvExistFlg(
    VOS_UINT8                           ucCsSmsSrvExistFlg
);
VOS_UINT8   TAF_SDC_GetCsSmsSrvExistFlg(VOS_VOID);
VOS_VOID TAF_SDC_SetPsSmsSrvExistFlg(
    VOS_UINT8                           ucPsSmsSrvExistFlg
);
VOS_UINT8 TAF_SDC_GetPsSmsSrvExistFlg(VOS_VOID);
VOS_VOID TAF_SDC_SetImsSmsSrvExistFlg(
    VOS_UINT8                           ucImsSmsSrvExistFlg
);
VOS_UINT8 TAF_SDC_GetImsSmsSrvExistFlg(VOS_VOID);
VOS_VOID    TAF_SDC_SetImsSsSrvExistFlg(
    VOS_UINT8                           ucImsSsSrvExistFlg
);
VOS_UINT8   TAF_SDC_GetImsSsSrvExistFlg(VOS_VOID);
VOS_VOID    TAF_SDC_SetCsSsSrvExistFlg(
    VOS_UINT8                           ucCsSsSrvExistFlg
);
VOS_UINT8   TAF_SDC_GetCsSsSrvExistFlg(VOS_VOID);
VOS_VOID TAF_SDC_InitSvlteSupportFlag(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);


VOS_VOID TAF_SDC_InitImsRatSupport(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);
VOS_VOID TAF_SDC_InitImsCapability(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);
VOS_VOID TAF_SDC_InitVoiceDomain(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);
VOS_VOID TAF_SDC_InitSmsDomain(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitImsConfigPara(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);
VOS_VOID TAF_SDC_InitLteCsServiceCfg(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitRedialCfgInfo(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitImsRoamingCfgInfo(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitImsUssdCfgInfo(
    TAF_SDC_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    TAF_SDC_MS_CAP_INFO_STRU           *pstMsCapInfo
);

VOS_VOID TAF_SDC_InitLcConfigPara(
    TAF_SDC_LC_CONFIG_PARA_STRU        *pstLcConfigPara
);



VOS_VOID TAF_SDC_SetGsmImsSupportFlag(
    VOS_UINT8                          ucGsmImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetGsmImsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetUtranImsSupportFlag(
    VOS_UINT8                          ucUtranImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetUtranImsSupportFlag(VOS_VOID);

VOS_VOID TAF_SDC_SetLteImsSupportFlag(
    VOS_UINT8                           ucImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetLteImsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetAppCfgSupportType(
    VOS_UINT16                          usAppCfgSupportType
);
VOS_UINT16 TAF_SDC_GetAppCfgSupportType(VOS_VOID);


VOS_VOID TAF_SDC_SetCsAttachAllowFlg(VOS_UINT8 ucCsAttachAllowFlg);
VOS_UINT8 TAF_SDC_GetCsAttachAllowFlg(VOS_VOID);


#if (FEATURE_ON == FEATURE_PTM)
OM_RING_ID TAF_SDC_GetErrLogRingBufAddr(VOS_VOID);
VOS_VOID TAF_SDC_SetErrLogRingBufAddr(OM_RING_ID pRingBuffer);
VOS_UINT8 TAF_SDC_GetErrlogCtrlFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetErrlogCtrlFlag(VOS_UINT8 ucFlag);
VOS_UINT16 TAF_SDC_GetErrlogAlmLevel(VOS_VOID);
VOS_VOID TAF_SDC_SetErrlogAlmLevel(VOS_UINT16 usAlmLevel);
VOS_VOID TAF_SDC_ReadErrlogCtrlInfoNvim(VOS_VOID);
VOS_VOID TAF_SDC_InitErrLogInfo(
    TAF_SDC_ERRLOG_INFO_STRU           *pstErrLogInfo
);
extern VOS_UINT32 TAF_SDC_PutErrLogRingBuf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
);
extern VOS_UINT32 TAF_SDC_GetErrLogRingBufContent(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
);
extern VOS_UINT32 TAF_SDC_GetErrLogRingBufferUseBytes(VOS_VOID);

extern VOS_VOID TAF_SDC_CleanErrLogRingBuf(VOS_VOID);
#endif

VOS_VOID TAF_SDC_SetVoiceCallOnImsSupportFlag(
    VOS_UINT8                          ucVoiceCallOnImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetVoiceCallOnImsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetSmsOnImsSupportFlag(
    VOS_UINT8                          ucSmsOnImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetSmsOnImsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetVideoCallOnImsSupportFlag(
    VOS_UINT8                          ucVideoCallOnImsSupportFlag
);
VOS_UINT8 TAF_SDC_GetVideoCallOnImsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetVoiceDomain(
    TAF_SDC_VOICE_DOMAIN_ENUM_UINT32    enVoiceDomain
);
TAF_SDC_VOICE_DOMAIN_ENUM_UINT32 TAF_SDC_GetVoiceDomain(VOS_VOID);
VOS_VOID TAF_SDC_SetSmsDomain(
    TAF_SDC_SMS_DOMAIN_ENUM_UINT8      enSmsDomain
);
TAF_SDC_SMS_DOMAIN_ENUM_UINT8 TAF_SDC_GetSmsDomain(VOS_VOID);
VOS_VOID TAF_SDC_SetLteEmsSupportFlag(
    VOS_UINT8                           ucLteEmsSupportFlag
);
VOS_UINT8 TAF_SDC_GetLteEmsSupportFlag(VOS_VOID);
VOS_VOID TAF_SDC_SetLteCsSeviceCap(
    TAF_SDC_LTE_CS_SERVICE_ENUM_UINT8   enLteCsServiceCap
);
TAF_SDC_LTE_CS_SERVICE_ENUM_UINT8 TAF_SDC_GetLteCsSeviceCap(VOS_VOID);
TAF_SDC_UE_USAGE_SETTING_ENUM_UINT8 TAF_SDC_GetUeUsageSetting(VOS_VOID);
VOS_VOID TAF_SDC_SetUeUsageSetting(
    TAF_SDC_UE_USAGE_SETTING_ENUM_UINT8   enUeUsageSetting
);

VOS_UINT8 TAF_SDC_GetRoamFlag(VOS_VOID);
VOS_VOID  TAF_SDC_SetRoamFlag(VOS_UINT8 ucRoamFlag);

VOS_UINT8 TAF_SDC_GetCampOnFlag( VOS_VOID );
VOS_VOID  TAF_SDC_SetCampOnFlag(VOS_UINT8 ucCampOnFlg);

TAF_SDC_SYS_MODE_ENUM_UINT8 TAF_SDC_GetCampSysMode( VOS_VOID );

VOS_UINT8 TAF_SDC_GetSimCsRegStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetSimCsRegStatus(
    VOS_UINT8       ucSimCsRegStatus
);


VOS_UINT8 TAF_SDC_GetSimPsRegStatus( VOS_VOID );
VOS_VOID TAF_SDC_SetSimPsRegStatus(
    VOS_UINT8       ucSimPsRegStatus
);

TAF_SDC_RAT_PRIO_STRU* TAF_SDC_GetMsPrioRatList( VOS_VOID );
VOS_VOID TAF_SDC_SetMsPrioRatList(
    TAF_SDC_RAT_PRIO_STRU              *pstPrioRatList
);


VOS_UINT8 TAF_SDC_GetCallRedailFromImsToCsSupportFlag(VOS_VOID);

VOS_VOID TAF_SDC_SetCallRedailFromImsToCsSupportFlag(VOS_UINT8 ucCallRedialFromImsToCs);

VOS_UINT8 TAF_SDC_GetSmsRedailFromImsToCsSupportFlag(VOS_VOID);

VOS_VOID TAF_SDC_SetSmsRedailFromImsToCsSupportFlag(VOS_UINT8 ucSmsRedialFromImsToCs);

VOS_UINT8 TAF_SDC_GetRoamingSupportFlag(VOS_VOID);

VOS_VOID TAF_SDC_SetRoamingSupportFlag(VOS_UINT8 ucRoamingSupport);

VOS_VOID    TAF_SDC_SetImsNormalRegStatus(
    TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM_UINT8    enRegStatus
);

TAF_SDC_IMS_NORMAL_REG_STATUS_ENUM_UINT8   TAF_SDC_GetImsNormalRegStatus(VOS_VOID);

VOS_VOID    TAF_SDC_SetImsVoiceAvailFlg(
    VOS_UINT8       ucAvail
);

VOS_UINT8   TAF_SDC_GetImsVoiceAvailFlg(VOS_VOID);

VOS_VOID  TAF_SDC_SetCurPhoneMode(
    TAF_SDC_PHONE_MODE_ENUM_UINT8       enPhoneMode
);

TAF_SDC_PHONE_MODE_ENUM_UINT8  TAF_SDC_GetCurPhoneMode(VOS_VOID);

VOS_VOID TAF_SDC_InitPhoneInfo(
    TAF_SDC_PHONE_INFO_STRU            *pstPhInfo
);

VOS_VOID TAF_SDC_SetWaitImsVoiceAvailTimerLen(
    VOS_UINT32                          ulTimerLen
);

VOS_UINT32 TAF_SDC_GetWaitImsVoiceAvailTimerLen(VOS_VOID);

VOS_VOID TAF_SDC_InitDsdaPlmnSearchEnhancedCfg(VOS_VOID);

VOS_UINT8 TAF_SDC_GetPsAttachAllowFlg(VOS_VOID);
VOS_VOID TAF_SDC_SetPsAttachAllowFlg(VOS_UINT8 ucPsAttachAllowFlg);


VOS_VOID  TAF_SDC_SetUtranSkipWPlmnSearchFlag(VOS_UINT8  ucUtranSkipWPlmnSearchFlag);
VOS_UINT8  TAF_SDC_GetUtranSkipWPlmnSearchFlag(VOS_VOID);


VOS_UINT8 TAF_SDC_GetUssdOnImsSupportFlag(VOS_VOID);

VOS_VOID TAF_SDC_SetUssdOnImsSupportFlag(VOS_UINT8 ucUssdOnImsSupportFlag);
VOS_VOID TAF_SDC_SetLCEnableFlg(
    VOS_UINT8                           ucLCEnableFlg
);

VOS_UINT8 TAF_SDC_GetLCEnableFlg(VOS_VOID);

VOS_UINT8 TAF_SDC_GetLCWorkCfg(VOS_VOID);
VOS_VOID   TAF_SDC_SetLCWorkCfg(
    TAF_NVIM_LC_WORK_CFG_ENUM_UINT8     enLCWorkCfg
);

VOS_UINT8 TAF_SDC_GetLCRatCombined(VOS_VOID);

TAF_SDC_LC_CONFIG_PARA_STRU* TAF_SDC_GetLcConfigPara(VOS_VOID);

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

#endif /* end of TafSdcCtx.h */
