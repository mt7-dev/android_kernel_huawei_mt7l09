

#ifndef __MMA_APPLOCAL_H__
#define __MMA_APPLOCAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "Ps.h"
#include "MnComm.h"
#include "Taf_Tafm_Remote.h"
#include "Taf_Common.h"
#include "Taf_Tafm_Local.h"
#include "TafAppMma.h"
#include "MmaForAtInc.h"
#include "NasUtranCtrlInterface.h"

/* 删除ExtAppMmcInterface.h*/

#include "MmcMmaItf.h"
#include "Taf_Status.h"
#include "Taf_Status_Itf.h"
#include "om.h"
#include "NVIM_Interface.h"
#include "UsimPsInterface.h"
#include "CardLockCipher.h"
#include "siappstk.h"
#include "MmaMmcInterface.h"
#include "TafMmaSndMmc.h"

#include "NasNvInterface.h"
#include "MmaAppLocal.h"

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
#include "MtcMmaInterface.h"
#endif

#pragma pack(4)
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MMA_SUCCESS     STA_SUCCESS
#define MMA_ERROR       STA_ERROR
#define MMA_FAILURE     TAF_FAILURE
/* BEGIN: Added by liuyang id:48197, 2006/3/13   PN:A32D02500*/
#define MMA_NORMAL      0X10
/* END:   Added by liuyang id:48197, 2006/3/13 */

#define MMA_TRUE     1
#define MMA_FALSE    0

#define MMA_INVALID_INDEX                (65535)


#define MMA_WRITE_NVIM_TRUE        0
#define MMA_WRITE_NVIM_FALSE       1

#define MMA_HS_ID                  12345678

#define MMA_HS_PROTOCOL_APP        0
#define MMA_HS_PROTOCOL_BOOT       1

#define MMA_HS_ONLINE              0
#define MMA_HS_OFFLINE             1
#define MMA_HS_LPM                 2

#define MMA_HS_PRODUCT_CLASS_GW            0
#define MMA_HS_PRODUCT_CLASS_CDMA_1X       1
#define MMA_HS_PRODUCT_CLASS_CDMA_EVDO     2

/*产品的ID，每一款产品华为都会分配一个唯一的ID, Balong没有产品，目前与终端E800A产品相同*/
#define MMA_HS_PRODUCT_ID                  56

#define MMA_PRINT_INFO    PS_PRINT_INFO

#define MMA_PRINT_NORMAL  PS_PRINT_NORMAL
/*normal: 对流程性的打印输出，例如某个模块收到××_REQ消息，然后发送了XX_CNF
  消息等，
  通过打印，可以直接了解流程的进展以及在各个模块的正常执行情况，以下内容是必
  须打印出来的:
  a. 状态下接收消息输出
  "PID名称:子模块名称(可选):实例标识(可选):STATE **state RECEIVE **msg\n"
  b. 状态下发送消息输出
  "PID名称:子模块名称(可选):实例标识(可选):STATE **state SEND **msg \n"
  c. 状态变迁输出
  "PID名称:子模块名称(可选):实例标识(可选):STATE **state CHANGE TO **state\n"

  b. 非状态类的输出
  "PID名称:子模块名称(可选):实例标识(可选):函数名:NORMAL:****\n"

  上述实例标识是指对于多实例的模块如APS，需要打印出当前处理的实例号即Ti值*/

#define MMA_PRINT_WARNING PS_PRINT_WARNING
/*warning: 一般是对一个已经估计到的异常流程发生后的处理，例如发出某个消息后
  ，由于定时器超时，
  而流程失败，则通过warning通过流程失败，或由于收到其他模块错误的参数，也通
  过warning输出:
  "PID名称:子模块名称(可选):实例标识(可选):函数名:WARNING:****\n"*/

#define MMA_PRINT_ERROR   PS_PRINT_ERROR
/*error: 一般是非常严重的错误发送，例如内存申请失败，启动定时器失败，状态迁
  移到未知状态等:
  "PID名称:子模块名称(可选):实例标识(可选):函数名:ERROR:****\n"*/

#define NAS_LOG( ModulePID, SubMod, Level, String )    PS_LOG(ModulePID, SubMod, Level, String)
#define NAS_LOG1( ModulePID, SubMod, Level, String, Para )     PS_LOG1(ModulePID, SubMod, Level, String, Para)
#define NAS_LOG2( ModulePID, SubMod, Level, String, Para1, Para2 )    PS_LOG2(ModulePID, SubMod, Level, String, Para1, Para2)
#define NAS_LOG3( ModulePID, SubMod, Level, String, Para1, Para2, Para3 ) PS_LOG3(ModulePID, SubMod, Level, String, Para1, Para2, Para3)
#define NAS_LOG4( ModulePID, SubMod, Level, String, Para1, Para2, Para3, Para4 )  PS_LOG4(ModulePID, SubMod, Level, String, Para1, Para2, Para3, Para4)


#define MMA_LOG( Level, String )        NAS_LOG( WUEPS_PID_MMA, TAF_MMA, Level, String )
#define MMA_LOG1( Level, String, Para ) NAS_LOG1( WUEPS_PID_MMA, TAF_MMA, Level, String, Para )
/* BEGIN: Added by liuyang id:48197, 2006/3/16   PN:A32D02412*/
#define MMA_INFOLOG( String )   MMA_LOG( MMA_PRINT_INFO, String)
#define MMA_INFOLOG1( String, Para )  MMA_LOG1( MMA_PRINT_INFO, String, Para)
/* END:   Added by liuyang id:48197, 2006/3/16 */
#define MMA_NORMAILLOG( String )    MMA_LOG( MMA_PRINT_NORMAL, String )
#define MMA_WARNINGLOG( String )    MMA_LOG( MMA_PRINT_WARNING, String )
#define MMA_WARNINGLOG1( String, Para )    MMA_LOG1( MMA_PRINT_WARNING, String, Para )
#define MMA_ERRORLOG( String )      MMA_LOG( MMA_PRINT_ERROR, String )
#define MMA_ERRORLOG1( String, Para )     MMA_LOG1(MMA_PRINT_ERROR, String, Para)

/* BEGIN: Added by liuyang id:48197, 2006/3/24   PN:A32D02755*/
/*内存管理相关的宏定义重载*/
#define MMA_ALLOC_MSG_WITHOUT_HEADER_LEN(pMsg, ulLen ) PS_ALLOC_MSG_WITHOUT_HEADER_LEN(pMsg, WUEPS_PID_MMA , ulLen)
#define MMA_ALLOC_MSG_WITHOUT_HEADER_LEN_RTN_FAIL(pMsg, ulLen) PS_ALLOC_MSG_RTN_FAIL_WITHOUT_HEADER_LEN(pMsg, WUEPS_PID_MMA , ulLen)
#define MMA_ALLOC_MSG_WITH_HEADER_LEN(pMsg, ulLen ) PS_ALLOC_MSG(pMsg, WUEPS_PID_MMA , ulLen)
#define MMA_ALLOC_MSG_WITH_HEADER_LEN_RTN_FAIL(pMsg, ulLen ) PS_ALLOC_MSG_RTN_FAIL(pMsg, WUEPS_PID_MMA , ulLen)
#define MMA_FREE_MSG(pMsg) PS_FREEMSG(WUEPS_PID_MMA, pMsg)
#define MMA_MEM_ALLOC(pMem, ulLen) PS_MEM_ALLOC_UNRTN(pMem, WUEPS_PID_MMA , ulLen)
#define MMA_MEM_ALLOC_RTN_FAIL(pMem, ulLen) PS_MEM_ALLOC_RTN_FAIL(pMem, WUEPS_PID_MMA , ulLen)
#define MMA_FREE_MEM(pBuffer) PS_MEM_FREE(WUEPS_PID_MMA,pBuffer)
#define MMA_MEM_CPY(pDestBuffer,pSrcBuffer,ulBufferLen) PS_MEM_CPY(pDestBuffer,pSrcBuffer,ulBufferLen)
#define MMA_MEM_SET(pBuffer,ucData,ulBufferLen) PS_MEM_SET(pBuffer,ucData,ulBufferLen)
/*调用系统信息函数重定义*/
#define MMA_SND_MSG(pMsg) PS_SEND_MSG(WUEPS_PID_MMA, pMsg)
/* END:   Added by liuyang id:48197, 2006/3/24 */

/* BEGIN: Added by liuyang id:48197, 2006/3/27   PN:A32D02372*/
#define MMA_PLMN_MCC_NONE       0x00
#define MMA_PLMN_MNC_NONE       0x00
/* END:   Added by liuyang id:48197, 2006/3/27 */

/*2G频段设置*/
typedef VOS_UINT16 GSMBANDTYPE;
#define GSM_BAND_NULL                   0x0
#define GSM_BAND_450                    0x0001        /* ARFCN所代表的频段:GSM 450 */
#define GSM_BAND_480                    0x0002        /* ARFCN所代表的频段:GSM 480 */
#define GSM_BAND_850                    0x0004        /* ARFCN所代表的频段:GSM 850 */
#define GSM_BAND_P900                   0x0008        /* ARFCN所代表的频段:P-GSM 900 */
#define GSM_BAND_R900                   0x0010        /* ARFCN所代表的频段:R-GSM 900 */
#define GSM_BAND_E900                   0x0020        /* ARFCN所代表的频段:E-GSM 900 */
#define GSM_BAND_1800                   0x0040        /* ARFCN所代表的频段:DCS 1800 */
#define GSM_BAND_1900                   0x0080        /* ARFCN所代表的频段:PCS 1900 */
#define GSM_BAND_700                    0x0100        /* ARFCN所代表的频段:GSM 700,不支持 */
#define GSM_BAND_ALL                   ( GSM_BAND_850|GSM_BAND_P900  \
                                        |GSM_BAND_R900|GSM_BAND_E900 \
                                        |GSM_BAND_1800|GSM_BAND_1900 )
/*3G频段设置*/
typedef VOS_UINT16 WCDMABANDTYPE;
#define WCDMA_BAND_NULL                 0x0000
#define WCDMA_BAND_I                    0x0001
#define WCDMA_BAND_II                   0x0002
#define WCDMA_BAND_III                  0x0004
#define WCDMA_BAND_IV                   0x0008
#define WCDMA_BAND_V                    0x0010
#define WCDMA_BAND_VI                   0x0020
#define WCDMA_BAND_VII                  0x0040
#define WCDMA_BAND_VIII                 0x0080
#define WCDMA_BAND_IX                   0x0100

#define WCDMA_BAND_ALL             ( WCDMA_BAND_I  \
                                    |WCDMA_BAND_II \
                                    |WCDMA_BAND_V )


#define MMA_PS_ATTACH              1
#define MMA_CS_ATTACH              2
#define MMA_PS_CS_ATTACH           3
#define MMA_UNKNOW_ATTACH          4

#define MMA_PS_DETACH              1
#define MMA_CS_DETACH              2
#define MMA_PS_CS_DETACH           3

#define MMA_NONEED_PIN1            0
#define MMA_NEED_PIN1              1
#define MMA_NEED_PUK1              2
#define MMA_ENABLE_PIN1_NONEED     3
#define MMA_PUK1_REMAINS_ZERO      4
#define MMA_PUK1_NONEED            5
#define MMA_PIN1_STATUS_FAIL     255

#define MMA_USIM_PRESENT           1
#define MMA_USIM_ABSENT            0
#define MMA_USIM_NOREADY           255

#define MMA_RSSI_UPPER_LIMIT       0
#define MMA_RSSI_LOWER_LIMIT       (-113)

#define MMA_USIM_FILE_EF_NETPAR_GSM_SIZE        72
#define MMA_USIM_FILE_EF_NETPAR_ID              0x6FC4

#define MMA_SIM_FILE_EF_NETPAR_GSM_SIZE         16
#define MMA_SIM_FILE_EF_NETPAR_ID               0x6F74
#define MMA_USIM_TYPE_SIM       0
#define MMA_USIM_TYPE_USIM      1
#define MMA_CLIENTID_NONE       (TAF_MAX_CLIENT_OF_ONE_PROC + 1)
#define MMA_OP_ID_INTERNAL      TAF_INVALID_TAB_INDEX
#define MMA_CLIENTID_BROADCAST  TAF_CLIENTID_BROADCAST
#define TAF_UICC_STATUS_CHANGE                  1
#define TAF_UICC_STATUS_UNCHANGE                0

#define NAS_MMA_AUTOMATIC_START  0
#define NAS_MMA_MANUAL_START     1


/*
  2 minimum functionality
  3 full functionality
  0 disable phone transmit RF circuits only
  1 ensable phone receive RF circuits only
*/
#define TAF_PH_MINI_FUN              2
#define TAF_PH_FULL_FUN              3

#define    MMA_QUICK_START_DISABLE    0x00000000
#define    MMA_QUICK_START_ENABLE     0x01000101
#define    MMA_AUTO_ATTACH_DISABLE    0x00000000
#define    MMA_AUTO_ATTACH_ENABLE     0x00000001
#define    MMA_BIT_ISVALID(a,bit) (((a)&(bit))==(bit))
#define    MMA_RF_AUTO_TEST_DISABLE   0x0
#define    MMA_RF_AUTO_TEST_ENABLE    0x1
#define    MMA_PS_DELAY_DISABLE       0x0                                       /* 设置AT+CFUN=6之后，PS不延迟5秒进入Onlin模式 */
#define    MMA_PS_DELAY_ENABLE        0x1                                       /* 设置AT+CFUN=6之后，PS延迟5秒进入Onlin模式 */
#define    MMA_NV_RESUME_DISABLE      0x0                                       /* 设置AT+CFUN=6之后，PS不延迟5秒进入Onlin模式 */
#define    MMA_NV_RESUME_ENABLE       0x1                                       /* 设置AT+CFUN=6之后，PS延迟5秒进入Onlin模式 */

#if (FEATURE_ON == FEATURE_LTE)
/* 获取用户设置的LTE频段 */
#define MN_MMA_GetUserSetLteBandAddr()        &(gstMmaValue.stLastSyscfgSet.stUserSetBand.stUserSetLteBand)

/* 获取UE支持的LTE频段 */
#define MN_MMA_GetUeSupportLteBandAddr()      &(gstMmaBandCapa.stUeSupportLteBand)
#endif

/* 获取上次用户通过syscfg设置成功的接入技术和优先级 */
#define MN_MMA_GetRatPrioListAddr()           &(gstMmaValue.stLastSyscfgSet.stRatPrioList)

/* 获取上次用户通过syscfg设置成功的参数值 */
#define MN_MMA_GetLastSyscfgSetAddr()         &(gstMmaValue.stLastSyscfgSet)

/* 获取上次用户通过syscfg设置成功的频段内容 */
#define MN_MMA_GetSyscfgUserSetBandAddr()     &(gstMmaValue.stLastSyscfgSet.stUserSetBand)



#define TAF_MMA_GetNotDisplayLocalNetworkNameFlg()     (gstMmaValue.ucNotDisplayLocalNetworkNameFlg)

#define MMA_UMTS_UEA0                                       (0)
#define MMA_UMTS_UEA1                                       (8)
#define MMA_UMTS_UEA2                                       (9)

/* 定义6F15文件的SERVICE GROUP CODE值 */
#define TAF_MMA_SERVICE_GROUP_CODE_IN_CUSTOMER_SERVICE_PROFILE_FILE    (0xC0)

/* 定义6F15文件的PLMN MODE位 */
#define TAF_MMA_PLMN_MODE_IN_SERVICE_GROUP_CODE                        (0x80)

/* 定义6F15文件的PLMN MODE位的偏移量 */
#define TAF_MMA_PLMN_MODE_OFFSET_IN_SERVICE_GROUP_CODE                 (7)

/* 定义6F15文件每个GROUP的长度 */
#define TAF_MMA_GROUP_LEN_IN_CUSTOMER_SERVICE_PROFILE_FILE             (2)


#define MMA_INVALID_MCC                                 (0xffffffff)            /* MCC的无效值 */
#define MMA_INVALID_MNC                                 (0xffffffff)            /* MNC的无效值 */
/*******************************************************************************
  3 枚举定义
*******************************************************************************/

enum MN_PH_REG_TIME_STATE_ENUM
{
    MN_PH_REG_TIME_STATE_BEGIN,
    MN_PH_REG_TIME_STATE_END,
    MN_PH_REG_TIME_STATE_PINVALID,
    MN_PH_REG_TIME_STATE_BUTT
} ;
typedef VOS_UINT8 MN_PH_REG_TIME_STATE_ENUM_U8;

typedef struct
{
    MN_PH_REG_TIME_STATE_ENUM_U8    enRegTimeState;
    VOS_UINT32                      ulBeginTick;
    VOS_UINT32                      ulEndTick;
    VOS_UINT32                      ulCostTime;
} MN_PH_REG_TIME_INFO_STRU;

typedef struct
{
    TAF_SDC_PLMN_ID_STRU            stPlmn;
    TAF_SDC_SYS_MODE_ENUM_UINT8     enSysMode;
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8 enServiceStatus;
    VOS_UINT16                      usLac;
    VOS_UINT32                      ulCellId;
}TAF_MMA_LOCATION_STATUS_EVENT_INFO_STRU;



/*****************************************************************************
 枚举名    : MMA_CIPHER_ALGOR_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 当前W下加密算法枚举
*****************************************************************************/
enum MMA_CIPHER_ALGOR_ENUM
{
    MMA_CIPHER_ALGOR_UEA0                 = 0,                /* 加密未打开 */
    MMA_CIPHER_ALGOR_UEA1                 = 1,
    MMA_CIPHER_ALGOR_UEA2                 = 2,
    MMA_CIPHER_ALGOR_BUTT
};
typedef VOS_UINT8  MMA_CIPHER_ALGOR_ENUM_UINT8;

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
    VOS_UINT32  BandSpare7                   : 7;
    VOS_UINT32  BandGsmDcs1800               : 1;
    VOS_UINT32  BandGsmEgsm900               : 1;
    VOS_UINT32  BandGsmPgsm900               : 1;
    VOS_UINT32  Spare6                       : 6;
    VOS_UINT32  BandWcdma_VII_2600           : 1;
    VOS_UINT32  BandWcdma_VIII_900           : 1;
    VOS_UINT32  BandWcdma_IX_1700            : 1;
    VOS_UINT32  BandGsm850                   : 1;
    VOS_UINT32  BandGsmRgsm900               : 1;
    VOS_UINT32  BandGsmPcs1900               : 1;
    VOS_UINT32  BandWcdma_I_Imt2k1           : 1;
    VOS_UINT32  BandWcdma_II_PCS_1900        : 1;
    VOS_UINT32  BandWcdma_III_1800           : 1;
    VOS_UINT32  BandWcdma_IV_1700            : 1;
    VOS_UINT32  BandWcdma_V_850              : 1;
    VOS_UINT32  BandWcdma_VI_800             : 1;
    VOS_UINT32  BandWCDMA_XIX_850            : 1;
    VOS_UINT32  BandWcdma_XI_1500            : 1;
    VOS_UINT32  Spare2                       : 2;
}MMA_USER_PREF_BAND_STRU;

typedef struct
{
    VOS_UINT32                          BandGsm450      :1;
    VOS_UINT32                          BandGsm480      :1;
    VOS_UINT32                          BandGsm850      :1;
    VOS_UINT32                          BandGsmP900     :1;
    VOS_UINT32                          BandGsmR900     :1;
    VOS_UINT32                          BandGsmE900     :1;
    VOS_UINT32                          BandGsm1800     :1;
    VOS_UINT32                          BandGsm1900     :1;
    VOS_UINT32                          BandGsm700      :1;
    VOS_UINT32                          BandSpare7      :23;
}MMA_GSM_PREF_BAND_STRU;

typedef struct
{
    VOS_UINT32                          BandWCDMA_I_2100      :1;
    VOS_UINT32                          BandWCDMA_II_1900     :1;
    VOS_UINT32                          BandWCDMA_III_1800    :1;
    VOS_UINT32                          BandWCDMA_IV_1700     :1;
    VOS_UINT32                          BandWCDMA_V_850       :1;
    VOS_UINT32                          BandWCDMA_VI_800      :1;
    VOS_UINT32                          BandWCDMA_VII_2600    :1;
    VOS_UINT32                          BandWCDMA_VIII_900    :1;
    VOS_UINT32                          BandWCDMA_IX_J1700    :1;
    VOS_UINT32                          BandSpare10           :1;
    VOS_UINT32                          BandWCDMA_XI_1500     :1;
    VOS_UINT32                          BandSpare7            :7;
    VOS_UINT32                          BandWCDMA_XIX_850     :1;
    VOS_UINT32                          BandSpare13           :13;
}MMA_WCDMA_PREF_BAND_STRU;


typedef union
{
    VOS_UINT32                          ulBand;
    MMA_GSM_PREF_BAND_STRU              BitBand;
}MMA_GSM_BAND_SET_UN;

typedef union
{
    VOS_UINT32                          ulBand;
    MMA_WCDMA_PREF_BAND_STRU            BitBand;
}MMA_WCDMA_BAND_SET_UN;
typedef union
{
    VOS_UINT32                          ulPrefBand;
    MMA_USER_PREF_BAND_STRU             BitBand;
}MMA_USER_BAND_SET_UN;

/*******************************************************************************
 结构名    : UE_SUPPORT_FREQ_BAND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 频段定义
*******************************************************************************/
typedef struct
{
    MMA_WCDMA_BAND_SET_UN                   unWcdmaBand;
    MMA_GSM_BAND_SET_UN                     unGsmBand;
    VOS_UINT8                               aucUeSupportWcdmaBand[NVIM_MAX_FDD_FREQ_BANDS_NUM];
    VOS_UINT8                               aucUeSupportGsmBand[NVIM_MAX_FDD_FREQ_BANDS_NUM];
}MMA_UE_SUPPORT_FREQ_BAND_STRU;


typedef VOS_UINT8 TAF_SERVICE_MODE;
#define  TAF_SERVICE_NULL     0       /*PS、CS业务都没有使能*/
#define  TAF_SERVICE_CS_PS    1       /*PS、CS业务都使能 */
#define  TAF_SERVICE_PS       2       /*仅PS业务使能 */
#define  TAF_SERVICE_CS       3       /*仅CS业务使能 */
#define  TAF_SERVICE_NOCHANGE 4       /*不改变服务域 */

typedef struct
{
    TAF_PH_PIN_DATA_STRU         stMmaPinData;
    VOS_UINT16                ClientId;
    VOS_UINT8                       OpId;
}MMA_PIN_OP_ST;

/*手机类型设置参数过程标志*/
typedef enum
{
    MMA_SET_MSCLASSTYPE_NO     = 0,   /*未处于设置过程*/
    MMA_SET_MSCLASSTYPE_YES,          /*处于设置过程*/
    MMA_SET_MSCLASSTYPE_FLAG_BUTT
}MMA_SET_MSCLASSTYPE_FLAG_ENUM;

typedef enum
{
    MMA_SET_CLASSTYPE_DE_ATTACH_DISABLE = 0,
    MMA_SET_CLASSTYPE_DETACH_ENABLE,
    MMA_SET_CLASSTYPE_ATTACH_ENABLE,
    MMA_SET_CLASSTYPE_DE_ATTACH_ENABLE,
    MMA_SET_CLASSTYPE_DE_ATTACH_FLG_BUTT
}MMA_SET_CLASSTYPE_DE_ATTACH_FLG_ENUM;

typedef struct
{
    TAF_PH_MS_CLASS_TYPE                     MsClassType;             /*手机类型设定*/
    TAF_PH_MS_CLASS_TYPE                     NewMsClassType;          /*新设置的手机类型*/
    MMA_SET_MSCLASSTYPE_FLAG_ENUM            enSetMsClassTypeflag;    /*是否设置手机类型标志*/
    MMA_SET_CLASSTYPE_DE_ATTACH_FLG_ENUM     enSetMsClassTypeDeAttFlg;/*开启是否允许detach,attach过程*/
}MMA_SET_MS_CLASS_TYPE_ST;
typedef VOS_UINT16   MMA_SYS_CFG_SET_FLG;
#define  MMA_SYS_CFG_NONE_SET                               0x0000 /*设置为空*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET                         0x0001 /*设置了服务域*/
#define  MMA_SYS_CFG_ROAM_SET                               0x0002 /*设置了漫游*/
#define  MMA_SYS_CFG_MODE_SET                               0x0004 /*设置了接入模式或接入次序*/
#define  MMA_SYS_CFG_BAND_SET                               0x0008 /*设置了频带*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH          0x0010 /*由于设置了服务域需要进行CS Detach*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH          0x0020 /*由于设置了服务域需要进行PS Detach*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH          0x0040 /*由于设置了服务域需要进行CS Attach*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH          0x0080 /*由于设置了服务域需要进行PS Attach*/
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_OFF        0x0100 /*由于设置了服务域需要通知STK/MSG 不支持CS域 */
#define  MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_SEND_CS_ON         0x0200 /*由于设置了服务域需要通知STK/MSG 支持CS域 */
#define  MMA_SYS_CFG_SRV_DOMAIN_NEED_ATTACH_DETACH                    \
                            (MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_DETACH\
                            |MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_DETACH\
                            |MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_CS_ATTACH\
                            |MMA_SYS_CFG_SRV_DOMAIN_SET_NEED_PS_ATTACH)
#define  MMA_SYS_CFG_SND_TO_MMC (MMA_SYS_CFG_ROAM_SET  \
                                |MMA_SYS_CFG_MODE_SET  \
                                |MMA_SYS_CFG_BAND_SET)
typedef struct
{
    VOS_UINT16                          usClientId;     /*Client Id*/
    VOS_UINT8                           ucOpId;         /*Operate Id*/
    TAF_PH_ATTACH_TYPE                  ucAttachType;   /*Attach类型*/
    TAF_PH_ATTACH_TYPE                  ucDetachType;   /*Detach类型*/
    MMA_SYS_CFG_SET_FLG                 usSetFlag;      /*SYSCFG命令设置标志*/


    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfgSetPara;/*SYSCFG命令设置参数*/

}MMA_SYS_CFG_SET_STRU;



typedef struct
{
    VOS_UINT16                          usClientId;     /*Client Id*/
    VOS_UINT8                           ucOpId;         /*Operate Id*/
    VOS_UINT8                           aucReserved[1];
}MMA_EOPLMN_SET_STRU;

/*表示初始化过程的状态*/
typedef enum
{
    MMA_INIT_POWERON  = 0,
    MMA_INIT_NOUSIM,
    MMA_INIT_NONVIM,
    MMA_INIT_FIN,
    MMA_INIT_STA_BUTT
}MMA_INIT_STA_ENUM;

/*启动初始化的参数类型*/
typedef enum
{
    MMA_INIT_DEFAULT,
    MMA_INIT_OUTSIDE,
    MMA_INIT_TYPE_BUTT
}MMA_INIT_TYPE_ENUM;

/*增加一个全局数据表,作为写入NVIM使用*/
typedef struct
{
    MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8    AccessMode;      /*手机接入模式*/
    TAF_PH_PLMN_PRIO PlmnPrio;        /*PLMN搜索优先级*/

}MMA_PARA_TAB_ST;

/*用于处理存放在nvim中的运营商信息*/

/*手机模式设置切换类型*/
typedef enum
{
    MMA_CC2CG   = (TAF_PH_MS_CLASS_CC<<4)|TAF_PH_MS_CLASS_CG,
    MMA_CC2A    = (TAF_PH_MS_CLASS_CC<<4)|TAF_PH_MS_CLASS_A,
    MMA_CC2NULL = (TAF_PH_MS_CLASS_CC<<4)|TAF_PH_MS_CLASS_NULL,
    MMA_CG2CC   = (TAF_PH_MS_CLASS_CG<<4)|TAF_PH_MS_CLASS_CC,
    MMA_CG2A    = (TAF_PH_MS_CLASS_CG<<4)|TAF_PH_MS_CLASS_A,
    MMA_CG2NULL = (TAF_PH_MS_CLASS_CG<<4)|TAF_PH_MS_CLASS_NULL,
    MMA_A2CC    = (TAF_PH_MS_CLASS_A<<4)|TAF_PH_MS_CLASS_CC,
    MMA_A2CG    = (TAF_PH_MS_CLASS_A<<4)|TAF_PH_MS_CLASS_CG,
    MMA_A2NULL  = (TAF_PH_MS_CLASS_A<<4)|TAF_PH_MS_CLASS_NULL,
    MMA_NULL2CC = (TAF_PH_MS_CLASS_NULL<<4)|TAF_PH_MS_CLASS_CC,
    MMA_NULL2CG = (TAF_PH_MS_CLASS_NULL<<4)|TAF_PH_MS_CLASS_CG,
    MMA_NULL2A  = (TAF_PH_MS_CLASS_NULL<<4)|TAF_PH_MS_CLASS_A,
    MMA_CLASS_CHANGE_INDEX_BUTT
}MMA_CLASS_CHANGE_INDEX_ENUM;

/*MMA内部使用的信号强度变量*/
typedef struct
{
    VOS_INT32        lRssiValue;
    VOS_UINT8        ucRssiValue4AT;
    VOS_UINT8        ucChannalQual;
}MMA_RSSI_VALUE_ST;

typedef struct
{
    MMA_RSSI_VALUE_ST       astRssi[TAF_PH_RSSI_MAX_NUM];
    VOS_UINT8               ucRssiNum;
}MMA_RSSI_VALUE_LIST_ST;

typedef struct
{
    TAF_PLMN_INFO_STRU    astPlmnInfo[TAF_PH_PLMN_LIST_MAX];
    VOS_UINT8             ucCurPlmnNum;
}MMA_PLMN_INFO_ST;

typedef VOS_UINT8   MMA_PLMN_SPECIALIZE_FLG;
#define MMA_PLMN_SPECIALIZE_NORMAL        0
#define MMA_PLMN_SPECIALIZE_MANUALAUTO    1

typedef struct
{
    TAF_PH_ICC_ID_STRU           stIccId;
    VOS_UINT16                   ClientId;
    VOS_UINT8                    OpId;
}MMA_GET_ICC_ID_ST;

typedef struct
{
    VOS_UINT16                          usCurrIndex;                    /* 本次需要读取的PNN list起始位index */
    VOS_UINT16                          usReadNum;                      /* 本次读取的PNN 数目 */
    TAF_PH_ICC_TYPE                     Icctype;                        /* 记录卡的类型 */
    VOS_UINT8                           ucAtPnnWaitCardCnfFlg;          /* 记录卡回复的PNN INDEX事件是否是由于AT^PNN查询引起的，规避与卡REFRESH消息的对冲 */
    VOS_UINT8                           aucReserve[2];
}MMA_READ_PNN_LIST_CTRL_STRU;


typedef struct
{
    VOS_UINT16                          usSumPnnNumInSim;               /* 卡中PNN总数目 */
    VOS_UINT16                          usEfLen;                        /* EF文件长度(对二进制来说是文件长度，对其它文件来说是记录长度)*/
    VOS_UINT8                          *pucPnnData;                       /* 记录从卡里读出来的PNN列表首地址 */
}MMA_READ_PNN_LIST_DATA_STRU;
typedef struct
{
    MMA_READ_PNN_LIST_CTRL_STRU         stPnnListCtrInfo;
    MMA_READ_PNN_LIST_DATA_STRU         stPnnListDataInfo;
}MMA_READ_PNN_LIST_INFO_STRU;

typedef struct
{
    VOS_UINT16                   ClientId;
    VOS_UINT8                    OpId;
    VOS_UINT8                    aucReserve[1];
    MMA_READ_PNN_LIST_INFO_STRU  stPnnListInfo;
}MMA_GET_PNN_ST;

typedef struct
{
    VOS_UINT16                   ClientId;
    VOS_UINT8                    OpId;
}MMA_GET_OPL_ST;
typedef struct
{
    VOS_UINT16                   usClientId;
    VOS_UINT8                    ucOpId;
    VOS_UINT8                    ucHplmnQryFlag;
}TAF_MMA_GET_HPLMN_STRU;

typedef struct
{
    VOS_UINT16                   ClientId;
    VOS_UINT8                    OpId;

    #define MMA_CPNN_WAIT_NULL            0
    #define MMA_CPNN_WAIT_PNN_FLAG        1
    #define MMA_CPNN_WAIT_OPL_FLAG        2
    VOS_UINT8                    ucWaitPnnOPLMask;

    VOS_UINT32                   ulPNNGetRslt;
    TAF_PH_USIM_CPNN_CNF_STRU    stCpnnCnf;
}MMA_GET_CPNN_ST;


typedef struct
{
    VOS_UINT8 ucEnableFlag;
    VOS_UINT8 ucForbidReg2GNetWork;
    VOS_UINT8 ucForbidSndMsg;
    VOS_UINT8 aucReserve[13];
}MMA_EFUST_SERVICE_CFG_STRU;



typedef struct
{
    MMA_USER_BAND_SET_UN                uUserSetGuBand;                         /* 上次用户设置成功当前生效的GU频段 */
    TAF_USER_SET_PREF_BAND64            stOrigUserSetGuBand;                    /* 上次用户设置成功的GU 64位原始频段,仅供查询使用 */
    MMA_UE_SUPPORT_FREQ_BAND_STRU       uUserSetUeFormatGuBand;                 /* 转换为UE支持格式的上次用户设置成功当前生效的GU频段 */
#if(FEATURE_ON == FEATURE_LTE)
    TAF_USER_SET_PREF_BAND64            stUserSetLteBand;                       /* 上次用户设置成功当前生效的LTE频段 */
#endif
}MN_MMA_SYSCFG_USER_SET_BAND_STRU;



typedef struct
{
    TAF_PH_RAT_ORDER_STRU               stRatPrioList;                          /* 上次用户设置成功当前生效的接入模式优先级*/
    VOS_UINT8                           ucRoam;                                 /* 上次用户设置成功当前生效的漫游能力 */
    TAF_MMA_USER_SET_PRIO_RAT_ENUM_U8   enPrioRat;                              /* 上次用户设置成功当前生效的接入优先级 */
    VOS_UINT8                           ucReserve;
    TAF_PH_SERVICE_DOMAIN               ucSrvDomain;                            /* 上次用户设置成功当前生效的服务域 */
    MN_MMA_SYSCFG_USER_SET_BAND_STRU    stUserSetBand;                          /* 上次用户设置成功当前生效的频段信息 */
}MN_MMA_LAST_SETTED_SYSCFG_SET_STRU;
typedef struct
{
    VOS_UINT16                   usClientId;
    VOS_UINT8                    ucOpId;

    VOS_UINT8                    ucReserve;
}MMA_NET_SCAN_STRU;


#define TI_TAF_MMA_NET_SCAN_TIMER_LEN                       (108 * 1000)
#define TI_TAF_MMA_ABORT_NET_SCAN_TIMER_LEN                 (10 * 1000)




typedef struct
{
    STATUS_CONTEXT_STRU          *pg_StatusContext;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*----------  MMA新增全局变量  -----------*/
/*=======================================================*/
    MMA_PIN_OP_ST                       stOpPinData;                /*MMA对PIN码操作数据*/

    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU  stLastSyscfgSet;                        /* 上次用户设置成功的syscfg参数 */

    MMA_SET_MS_CLASS_TYPE_ST            stSetMsClass;               /*手机类型设置相关标志与结果*/

    MMA_SYS_CFG_SET_STRU                stSysCfgSet;
    MMA_RSSI_VALUE_LIST_ST              stRssiValue;                /*手机信号强度*/
    TAF_PH_BATTERYPOWER_STRU            stBatteryPower;             /*手机电池管理*/
    TAF_PH_MEINFO_STRU                  stMeInfo;                   /*手机基本信息*/
    TAF_USIM_INFO_STRU                  stUsimInfo;                 /*USIM相关信息*/
    MMA_QUICK_START_STA_UINT32          ulQuickStartFlg;
    MMA_GET_ICC_ID_ST                   stGetIccId;
    MMA_GET_PNN_ST                      stPNN;                  /*读取PNN文件内容*/
    MMA_GET_OPL_ST                      stOPL;                  /*读取OPL文件内容*/
    TAF_MMA_GET_HPLMN_STRU              stGetHplmn;                  /*读取OPL文件内容*/
    VOS_UINT8                           aucReserved1[2];
    MMA_EFUST_SERVICE_CFG_STRU          stEfustServiceCfg;

    MMA_GET_CPNN_ST                     stCPNN;

    VOS_UINT8                           ucCopsNotDisplaySpnFlg;
    VOS_UINT8                           ucRoamFeatureStatus;
    VOS_UINT8                           ucRoamBrokerSwitch;         /* 是否打开roaming broker.VOS_TRUE:打开;VOS_FALSE:不打开。默认关闭 */



    VOS_UINT8                           ucReportPlmnFlg;

    VOS_UINT8                           ucNotDisplayLocalNetworkNameFlg;

    TAF_PH_RSSI_STRU                      stCerssiValue;                  /* 保存接入层主动上报的信号质量信息 */

    MMA_MMC_OPERATOR_NAME_INFO_STRU       stOperatorNameInfo;

    MMA_EOPLMN_SET_STRU                 stEOPlmnSet;
    MMA_NET_SCAN_STRU                   stNetScan;
}TAF_MMA_GLOBAL_VALUE_ST;

typedef enum
{
    MMA_STORED_CLIENT_OPID_OP_READ,
    MMA_STORED_CLIENT_OPID_OP_WRITE,
    MMA_STORED_CLIENT_OPID_OP_BUTT
}MMA_STORED_CLIENT_OPID_OP_ENUM;

typedef struct
{
    VOS_UINT16                          ausClientID[6];/* MMA中共6个client id 需要保存 */
    VOS_UINT8                           aucOPID[6];
}MMA_STORED_CLIENT_OPID_ST;


typedef struct
{
    VOS_UINT32                          ulUeGSptBand;
    VOS_UINT32                          ulUeWSptBand;
    VOS_UINT32                          ulAllUeBand;
    MMA_WCDMA_BAND_SET_UN               unWRFSptBand;
    MMA_GSM_BAND_SET_UN                 unGRFSptBand;

#if(FEATURE_ON == FEATURE_LTE)
    TAF_USER_SET_PREF_BAND64            stUeSupportLteBand;
#endif
}MMA_UE_BAND_CAPA_ST;


typedef struct
{
    VOS_UINT8                           ucActiveFlag;
    VOS_UINT8                           ucForbStatus;
    VOS_UINT8                           aucReserve[2];
    TAF_USER_SET_PREF_BAND64            stForbBand; /* NV项中原始的NV记录 */
    VOS_UINT32                          ulForbBand; /* 内部使用的频段信息 */
}MMA_UE_FORB_BAND_ST;

/*对PIN的操作所需的数据结构*/
typedef struct TAF_USIM_OPPIN_CNF_MSG
{
    VOS_MSG_HEADER
    TAF_MSG_TYPE                           MsgType;
    TAF_PH_PIN_CNF_STRU                    stPinCnf;
}TAF_USIM_OPPIN_CNF_MSG_ST;
typedef struct TAF_USIM_READ_SPN_CNF_MSG
{
    VOS_MSG_HEADER
    TAF_MSG_TYPE                           MsgType;
    TAF_PH_USIM_SPN_CNF_STRU               stSpnCnf;
}TAF_USIM_READ_SPN_CNF_MSG_ST;

#define TAF_SIM_TIMER_NUM 3
/* 3个Sim卡交换交互定时器: 0:用于pin操作
  1:用于SPN读取 2:用于 SIM卡 CPHS 6F14的文件读取*/
enum MN_PH_SIM_TIMER_ENUM
{
    MN_PH_SIM_TIMER_ENUM_PIN,
    MN_PH_SIM_TIMER_ENUM_SPN,
    MN_PH_SIM_TIMER_ENUM_CPHS_ONS,
    MN_PH_SIM_TIMER_ENUM_BUTT
} ;
typedef VOS_UINT8 MN_PH_SIM_TIMER_ENUM_U8;

/*增加对SPN文件读的宏*/
#define TAF_USIM_SPN_EFID  0x6f46  /*USIM EFspn的ID*/
#define TAF_SIM_SPN_EFID   0x9f46  /*SIM  EFspn的ID*/
#define TAF_CPHS_SIM_ONS_EFID   0x6F14  /*CPHS SIM OPER NAME STRING的ID*/

/* 6F15文件ID */
#define TAF_MMA_CUSTOMER_SERVICE_PROFILE_FILE_ID            (0x6F15)

/* 4F36文件ID */
#define TAF_MMA_RAT_MODE_FILE_ID                            (0x4F36)

/* 6F15文件Refresh触发自动搜网，周期性尝试定时器时长 */
#define TI_TAF_MMA_PERIOD_TRYING_NETWORK_SELECTION_MENU_LEN (5*1000)

/* 4F36文件Refresh触发内部SYSCFG，周期性尝试定时器时长 */
#define TI_TAF_MMA_PERIOD_TRYING_RAT_BALANCING_LEN          (5*1000)

/* 4F36文件长度 */
#define TAF_MMA_RAT_MODE_FILE_LEN                           (1)



#define MMA_INTERNAL_OP_NONE            0x0
#define MMA_INTERNAL_OP_CLASS_CHANGE    0x01


/* <==A32D12535 */
#define MMA_INTERNAL_SET_BAND_REQ       0x03

#define MMA_WAIT_PHY_READ_MAX_TIMES     0x01
#define MMA_SET_PREF_PLMN_REQ           0x06
#define MMA_WAIT_SET_PREF_PLMN_CNF_MAX_TIMES 0x14
#define MMA_GET_USIM_ICC_ID              0x07
#define MMA_WAIT_GET_USIM_ICC_ID_MAX_TIMES 0x10
#define MMA_GET_USIM_PNN                 0x08
#define MMA_WAIT_GET_USIM_FILE_MAX_TIMES 0x5
#define MMA_GET_USIM_OPL                 0x09
#define MMA_GET_USIM_CPNN                0x0A

#define MMA_GET_USIM_MNC_LENGTH_OP       0x0B

#define MMA_TIMER_FOR_PC_REPLAY_VAL      (60*1000)

#define TI_MN_PH_RESET_LENGTH            (50)



/* 将等待DETACH的时间长度由20秒为30秒，GMM/MM启动保护时长为25s */
#define TI_MN_MMA_SYSCFG_WAIT_DETACH_CNF_TIMER_LEN          (30 * 1000)     /* syscfg设置过程需要detach操作时等待detach cnf保护定时器时长，20秒*/

#define TI_TAF_MMA_PNN_LIST_MEM_PROTECT_TIMER_LEN     (15 * 1000)

#define TI_TAF_MMA_DELAY_POWER_DOWN_TIMER_LEN               (50)



#define TI_MN_PH_PIN_OPER_LENGTH         (1000)

#define MN_MMA_MNC_LENGTH_INDICATOR_POSITION               (4)                  /* 指示MNC长度的字节在EF文件中的位置 */
#define MN_MMA_MNC_LENGTH_TWO_BYTES_IN_IMSI                (2)                  /* MNC在IMSI中的长度是2 */
#define MN_MMA_MNC_LENGTH_THREE_BYTES_IN_IMSI              (3)                  /* MNC在IMSI中的长度是3 */

#define MN_MMA_OCTET_LOW_FOUR_BITS                         (0x0f)               /* 获取一个字节中的低4位 */

typedef struct MMA_INTERNAL_TIMER
{
    MMA_TIMER_ST  stTimer;
    VOS_UINT32    ulTimes;
    VOS_UINT32    ulOpType;
    VOS_UINT16 ClientId;
    VOS_UINT8        OpId;
}MMA_INTERNAL_TIMER_ST;
/* END:   Added by liuyang id:48197, 2006/3/2 */

/*TI记录表*/
typedef struct
{
    VOS_UINT16             ClientId;
    VOS_UINT8                    Id;               /*CallId, Smi, OpId*/
    VOS_UINT8                 ucUsed;
    TAF_PHONE_EVENT           PhoneEvent;       /*记录当前事件*/
}MMA_TI_TABLE;

/*启动定时器使用的数据结构*/
#define MMA_START_INITIAL_TIMER_ID  0x88
typedef struct
{
    MMA_TIMER_ST SetupTimer;
    VOS_UINT8    ucTimes;
}MMA_START_INITIAL_TIMER_ST;






/* BEGIN: Added by liuyang id: 48197, 2006/3/20   PN:A32D02500*/
/*启动标志变量结构定义，0：未启动，1：已启动*/
typedef enum
{
    MMA_PS_LOCAL_NOT_START,
    MMA_PS_LOCAL_STARTED,
    MMA_START_FLG_BUTT
}MMA_START_FLG_LOCAOL_ENUM;

typedef enum
{
    MMA_PHONE_NOT_READY,
    MMA_PHONE_READY
}MMA_PHONE_READY_ENUM;

/*内部启动过程的触发过程,
MMA_PS_NORMAL_START: 用户调用Taf_PhoneStart函数触发
MMA_PS_TAF_INIT_START:taf_Init时候，检查usim状态可用且PIN未使能触发内部过程
MMA_PS_PIN_OP_START:启动过程中PIN使能，用户验证PIN 通过启动内部过程。
*/
typedef enum
{
    MMA_PS_NORMAL_START,
    MMA_PS_TAF_INIT_START,
    MMA_PS_PIN_OP_START,
    MMA_START_LOCAL_PROC_BUTT
}MMA_START_LOCAL_PROC_ENUM;

/*启动标志结构体*/
typedef struct
{
    MMA_START_FLG_LOCAOL_ENUM MmaLocalStartFlg;
    MMA_START_LOCAL_PROC_ENUM MmaLocalStartProc;
}MMA_START_FLG_ST;

/* BEGIN: Added by liuyang id:48197, 2006/3/21   PN:A32D02602*/
typedef struct
{
    TAF_PH_PIN_CNF_STRU   stPinDataCnf;
    VOS_UINT8             ucPinDataFlg;
}MMA_PIN_DATA_REPORT_STRU;
/* END:   Added by liuyang id:48197, 2006/3/21 */
/* END:   Added by liuyang id: 48197, 2006/3/20 */

#define MMA_USIM_UPLMN_FILE_INDEX   0x6F60
#define MMA_SIM_PLMN_SEL_FILE_ID    0x6F30
#define MMA_USIM_ICC_ID_INDEX       0x2fe2
#define MMA_USIM_PNN_FILE_INDEX     0x6FC5
#define MMA_USIM_OPL_FILE_INDEX     0x6FC6
#define MMA_USIM_ICC_ID_LENGTH      10
#define MMA_USIM_IMSI_FILE_ID       0x6f07
#define MMA_PS_FLASH_ADDR           0x400

#define MMA_MNC_LENGTH_FILE_ID     (0x6FAD)


typedef VOS_UINT8  MMA_SIM_LOCK_STATUS;
#define MMA_SIM_IS_UNLOCK           0
#define MMA_SIM_IS_LOCK             1
#define MMA_SIM_LOCK_IS_VERIFY      2

typedef struct
{
    MMA_SIM_LOCK_STATUS                 SimLockStatus;
    TAF_OPERATOR_LOCK_INFO_STRU         stCardLockInfo;
}MMA_ME_PERSONALISATION_STATUS_STRU;

#define EVT_NAS_MMA_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY   0xaaaa          /* 为PC工程回放定义的消息 */
#define MMA_MAX_TI                      (TAF_MAX_STATUS_TI + 2)

#define ID_NAS_MMA_MNTN_GET_USIM_STATUS   0xbbbb                                /* USIM卡状态可维可测勾包 */







typedef struct
{
    VOS_UINT16    Mcc;
    VOS_UINT16    Mnc;
}TAF_PH_OPERATOR_PLMN_ID_STRU;


typedef struct
{
    TAF_PH_OPERATOR_PLMN_ID_STRU        PlmnId;
    TAF_UINT8                           ucMncLength;
    TAF_UINT8                           aucReserved1[3];
    TAF_CHAR*                           pStrOperatorNameShort ;
    TAF_CHAR*                           pStrOperatorNameLong;
}TAF_PH_OPERATOR_NAME_TBL_STRU;

typedef struct
{
    TAF_PLMN_ID_STRU                        stPlmnId;
    VOS_UINT16                              PlmnRat;
    VOS_UINT16                              usIndex;
}MMA_MODIFY_PLMN_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulUsimHotInFlg;
    VOS_UINT32                          ulUsimHotOutFlg;
    VOS_UINT32                          ulUsimHotInCount;
    VOS_UINT32                          ulUsimHotOutCount;
    VOS_UINT8                           ucUsimStatusFromUsim;

    VOS_UINT8                           ucMeLockStatus;
    VOS_UINT8                           ucMmaGlobalUsimStatus;
    MMA_START_FLG_ST                    stMmaStartFlg;
}NAS_MMA_USIM_STATUS_INFO_STRT;


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgID;
    NAS_MMA_USIM_STATUS_INFO_STRT       stStatusInfo;
}NAS_MMA_MNTN_GET_USIM_STATUS_STRU;


/*****************************************************************************
 Structure      : NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST
 Description    : PC回放工程，存储所有MMA相关的全局变量
 Message origin :
*****************************************************************************/

typedef struct
{
    VOS_CHAR                                acVersionBuffer[VER_MAX_LENGTH];
    VOS_UINT8                               aucReserve[2];

    VOS_UINT8                               ucSimType;
    VOS_UINT8                               pc_g_aucMmaImsi[9];
    VOS_UINT8                               pc_g_aucMmaImei[TAF_PH_IMEI_LEN-1];
    VOS_UINT8                               reserved[2];

    STATUS_CONTEXT_STRU                     pc_g_StatusContext;
    TAF_MMA_GLOBAL_VALUE_ST                 pc_gstMmaValue;

    MMA_ME_PERSONALISATION_STATUS_STRU      pc_g_stMmaMePersonalisationStatus;
    MMA_TI_TABLE                            pc_gastMmaTiTab[MMA_MAX_TI];

    MMA_INTERNAL_TIMER_ST                   pc_gstMmaInternalTimer;

    MMA_TIMER_ST                            pc_g_MmaSimTimer[TAF_SIM_TIMER_NUM];
} NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST;

/*****************************************************************************
 Structure      : NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST
 Description    : PC回放工程，所有MMA相关的全局变量通过以下消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                           usMsgID;
    VOS_UINT16                           usReserved;                            /* 在PACK(1)到PACK(4)调整中定义的保留字节 */
    NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST   stOutsideCtx;
}NAS_MMA_SDT_MSG_ST;

/* 以下消息只导出开机时读取NV项，或者USIM API获取的全局变量。用于从开机开始的回放 */
#define EVT_NAS_MMA_FIXED_PART_CONTEXT   0xaabb          /* PC回放工程固定变量导出消息 */


/*****************************************************************************
 Structure      : NAS_MMA_FIXED_PART_CONTEXT_ST
 Description    : PC回放工程，存储所有MMA相关的固定变量
 Message origin :
*****************************************************************************/

typedef struct
{
    VOS_UINT32                              ulVersion;                       /* increased when add new paras */

    /* ulVersion = 0 part */
    VOS_UINT8                               aucMmaImsi[9];
    VOS_UINT8                               aucSimType;
    VOS_UINT8                               aucReserved1[2];

    VOS_UINT32                              ulPrefBand;                      /* en_NV_Item_User_Set_Freqbands */
    MMA_UE_SUPPORT_FREQ_BAND_STRU           Band;                            /* en_NV_Item_Support_Freqbands */
    MMA_QUICK_START_STA_UINT32              ulQuickStartFlg;                 /* en_NV_Item_FollowOn_OpenSpeed_Flag */
    VOS_UINT16                              usRfAutoTestFlg;                 /* en_NV_Item_RF_Auto_Test_Flag */
    VOS_UINT8                               aucReserved5[2];
    MMA_EFUST_SERVICE_CFG_STRU              stEfustServiceCfg;               /* en_NV_Item_EFust_Service_Cfg */

    VOS_UINT8                               aucImeiBcd[TAF_PH_IMEI_LEN+1];   /* en_NV_Item_IMEI */
    VOS_UINT8                               aucReserved2[3];

    MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8         enAccessMode;                      /* en_NV_Item_MMA_AccessMode */
    TAF_PH_PLMN_PRIO                        PlmnPrio;
    TAF_PH_MS_CLASS_TYPE                    MsClassType;                     /* en_NV_Item_MMA_MsClass */
    TAF_PH_ROAM_MODE                        ucRoam;                          /* en_NV_Item_Roam_Capa */

    TAF_PH_MEINFO_STRU                      stMeInfo;
    VOS_UINT8                               aucReserved3[2];

    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU      stSimLockPlmnInfo;               /* en_NV_Item_CustomizeSimLockPlmnInfo */
    TAF_CUSTOM_CARDLOCK_STATUS_STRU         stCardLockStatus;                /* en_NV_Item_CardlockStatus */
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU      stSimLockMaxTimes;               /* en_NV_Item_CustomizeSimLockMaxTimes */

    VOS_UINT8                               aucPwd[TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX +1];    /* en_NV_Item_Sim_Personalisation_Pwd */
    VOS_UINT16                              usSimMePersonalisationActFlag;   /* en_NV_Item_Me_Personalisation_Active_Flag */
    VOS_UINT8                               aucReserved4[1];

    TAF_UINT8                               aucSimPersonalisationStr[TAF_MAX_IMSI_LEN + 1];      /* en_NV_Item_Sim_Personalisation_Imsi_Str */

    /* ulVersion = 1 part */
    MMA_UE_BAND_CAPA_ST                     gstMmaBandCapa;
    VOS_UINT8                               ucRoamFeatureStatus;
} NAS_MMA_FIXED_PART_CONTEXT_ST;

/*****************************************************************************
 Structure      : NAS_MMA_NEW_ADD_GLABAL_VARIABLE_FOR_PC_REPLAY_ST
 Description    : PC回放工程，新增全局变量保存读取的NV值。
 Message origin :
*****************************************************************************/

typedef struct
{
    VOS_UINT8                               ucSimType;
    VOS_UINT8                               aucImeiBcd[TAF_PH_IMEI_LEN+1];   /* en_NV_Item_IMEI */
    VOS_UINT8                               aucReserved3[2];

    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU      stSimLockPlmnInfo;               /* en_NV_Item_CustomizeSimLockPlmnInfo */
    TAF_CUSTOM_CARDLOCK_STATUS_STRU         stCardLockStatus;                /* en_NV_Item_CardlockStatus */
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU      stSimLockMaxTimes;               /* en_NV_Item_CustomizeSimLockMaxTimes */

    VOS_UINT8                               aucPwd[TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX +1];    /* en_NV_Item_Sim_Personalisation_Pwd */
    VOS_UINT16                              usSimMePersonalisationActFlag;   /* en_NV_Item_Me_Personalisation_Active_Flag */
    VOS_UINT8                               aucReserved4[1];

    TAF_UINT8                               aucSimPersonalisationStr[TAF_MAX_IMSI_LEN + 1];      /* en_NV_Item_Sim_Personalisation_Imsi_Str */

} NAS_MMA_NEW_ADD_GLABAL_VARIABLE_FOR_PC_REPLAY_ST;

/*****************************************************************************
 Structure      : NAS_MMA_FIXED_CONTEXT_MSG_ST
 Description    : PC回放工程，所有MMA相关的固定全局变量通过以下消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                           usMsgID;
    VOS_UINT16                           usReserved;                            /* 在PACK(1)到PACK(4)调整中定义的保留字节 */
    NAS_MMA_FIXED_PART_CONTEXT_ST        stFixedOutsideCtx;
}NAS_MMA_FIXED_CONTEXT_MSG_ST;
typedef struct
{
    VOS_UINT8                           ucGprsCipherAlg;                        /* GPRS加密算法 */
    VOS_UINT8                           ucGprsCipher;                           /* VOS_TRUE:加密启动,VOS_FALSE:加密不启动。*/
    MMA_CIPHER_ALGOR_ENUM_UINT8         enCipherAlgorForCSDomain;               /* CS域加密算法  0:uea0; 1:uea1; 2:uea2 */
    MMA_CIPHER_ALGOR_ENUM_UINT8         enCipherAlgorForPSDomain;               /* PS域加密算法  0:uea0; 1:uea1; 2:uea2 */
}MMA_CIPHER_INFO_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */
    VOS_UINT32                          ulFileLen;                              /* 文件长度 */
    VOS_UINT8                           aucFileContent[4];                      /* 文件内容 */
}TAF_MMA_GET_CACHE_FILE_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */
    VOS_UINT8                           ucDrxTimerStatus;                       /*  DRX定时器的状态 */
    VOS_UINT8                           aucReserve[3];
}TAF_MMA_DRX_TIMER_STAUTS_STRU;


#define TAF_MMA_LOG_NV_READ_WRITE_RSLT_MSG      0xdddd
#define TAF_MMA_MAX_NV_INFO_LEN                 0x20

typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头 */ /*_H2ASN_Skip*/
    VOS_UINT32                          ulNvId;
    VOS_UINT32                          ulRslt;/* VOS_TRUE:成功 ;VOS_FALSE:失败 */
    VOS_UINT8                           aucNvInfo[TAF_MMA_MAX_NV_INFO_LEN];/* NV内容 */
}TAF_MMA_LOG_READ_WRITE_NV_INFO_STRU;

typedef enum
{
   MMA_GET_REC_I,
   MMA_GET_REC_II,
   MMA_GET_REC_END
}MMA_GET_REC_PHASE;

typedef VOS_UINT32 MMA_GET_REC_PHASE_U32;

typedef struct
{
    USIMM_APP_TYPE_ENUM_UINT32          enAppType;
    MMA_GET_REC_PHASE_U32               enNextPhase;
}MMA_GET_REC_INFO_STRU;

enum TAF_MMA_PROC_PNN_PROTECT_TIMER_ENUM
{
   TAF_MMA_PROC_PNN_PROTECT_TIMER_START,
   TAF_MMA_PROC_PNN_PROTECT_TIMER_STOP,

   TAF_MMA_PROC_PNN_PROTECT_TIMER_BUTT
};
typedef VOS_UINT8 TAF_MMA_PROC_PNN_PROTECT_TIMER_ENUM_UINT8;
enum TAF_MMA_PROC_NET_SCAN_PROTECT_TIMER_ENUM
{
   TAF_MMA_PROC_NET_SCAN_PROTECT_TIMER_START,
   TAF_MMA_PROC_NET_SCAN_PROTECT_TIMER_STOP,

   TAF_MMA_PROC_NET_SCAN_PROTECT_TIMER_BUTT
};
typedef VOS_UINT8 TAF_MMA_PROC_NET_SCAN_PROTECT_TIMER_ENUM_UINT8;


enum TAF_MMA_PROC_ABORT_NET_SCAN_PROTECT_TIMER_ENUM
{
   TAF_MMA_PROC_ABORT_NET_SCAN_PROTECT_TIMER_START,
   TAF_MMA_PROC_ABORT_NET_SCAN_PROTECT_TIMER_STOP,

   TAF_MMA_PROC_ABORT_NET_SCAN_PROTECT_TIMER_BUTT
};
typedef VOS_UINT8 TAF_MMA_PROC_ABORT_NET_SCAN_PROTECT_TIMER_ENUM_UINT8;



enum MMA_DEACT_SIM_WHEN_POWEROFF_ENUM
{
   MMA_DEACT_SIM_WHEN_POWEROFF_DISABLE,
   MMA_DEACT_SIM_WHEN_POWEROFF_ENABLE,

   MMA_DEACT_SIM_WHEN_POWEROFF_BUTT
};
typedef VOS_UINT8 MMA_DEACT_SIM_WHEN_POWEROFF_ENUM_UINT8;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/



/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*函数定义*/
VOS_VOID MN_PH_GetCsqLvlExtPara(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
);

VOS_VOID MN_PH_GetCsqLvlPara(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
);


VOS_UINT8 TAF_MMA_IsNeedSndGetUsimPnnFileReq(
    VOS_UINT16        ClientId,
    VOS_UINT8         OpId,
    TAF_PH_QRY_USIM_INFO_STRU          *pstInfo
);

VOS_UINT32 TAF_MMA_PnnListMemProtectTimerProc(
    TAF_MMA_PROC_PNN_PROTECT_TIMER_ENUM_UINT8 enProcType
);

VOS_VOID TAF_MMA_AtQryPnnListProc(VOS_VOID);

VOS_VOID TAF_MMA_PnnListMemProtectTimerExpired(VOS_VOID);

VOS_VOID TAF_MMA_RcvDelayPowerDownTimerExpired(VOS_VOID);
    


VOS_VOID TAF_MMA_SndAtSubPnnList(
    VOS_UINT32                   ulSndNum
);

VOS_VOID TAF_MMA_ClearcPnnListQryCtrInfo(VOS_VOID);

VOS_VOID TAF_MMA_UsimGetFileRspPnnMsgProc(
    PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg
);



VOS_VOID MMA_GetAccessModePara(TAF_ACCESS_MODE_QUERY_PARA_STRU* pAccessMode);
VOS_VOID MMA_DbQuery(VOS_UINT16             ClientId,
                     VOS_UINT8                    OpId,
                     TAF_PARA_TYPE       ParaType);

/*
VOS_VOID Taf_DecodeMeInfo( TAF_PH_MEINFO_STRU  *pstMeInfo,
                            VOS_UINT8           **pucInputBuf );
VOS_VOID Taf_DecodeMsStatus( TAF_PH_MS_STAT_STRU      *pstMsStatus,
                             VOS_UINT8                **pucInputBuf );
VOS_VOID Taf_DecodeNetWorkName( TAF_PH_NETWORKNAME_STRU  *pstOperatorName,
                                VOS_UINT8                **pucInputBuf );
VOS_VOID Taf_DecodePinCnf( TAF_PH_PIN_CNF_STRU    *pPinCnf,
                                 VOS_UINT8             **pucInputBuf );
VOS_VOID Taf_DecodeRegStatus( TAF_PH_REG_STATE_STRU *pstRegStatus,
                                     VOS_UINT8            **pucInputBuf );
VOS_VOID Taf_DecodeRssiValue( TAF_PH_RSSI_STRU         *pstRssi,
                                    VOS_UINT8                **pucInputBuf );
VOS_VOID Taf_DecodeSpnInfo( TAF_PH_USIM_SPN_STRU  *pstSpnInfo,
                                  VOS_UINT8             **pucInputBuf );
VOS_VOID Taf_DecodeSystemMode( TAF_PH_SYSTEM_MODE_STRU  *pstSystemMode,
                                     VOS_UINT8                **pucInputBuf );
VOS_VOID Taf_DecodeUsimInfo( TAF_USIM_INFO_STRU  *pstUsimInfo,
                                   VOS_UINT8           **pucInputBuf);
VOS_VOID Taf_DecodeWholeInfo( TAF_AT_DATA_STRU *pstGobalInfo,
                                    VOS_UINT8        **pucInputBuf);
*/

VOS_UINT32 TAF_MMA_GetPidFromClientId(
    MN_CLIENT_ID_T                      usClientId
);

VOS_VOID MMA_DefPhAccessMode(VOS_UINT16      ClientId,
                             VOS_UINT8             OpId,
                             MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8    AccessMode,
                             TAF_PH_PLMN_PRIO PlmnPrio);
VOS_VOID MMA_DefPhClassType(VOS_UINT16           ClientId,
                            VOS_UINT8                  OpId,
                            TAF_PH_MS_CLASS_TYPE    MsClass);
VOS_VOID MMA_PhModeReport(VOS_UINT16           ClientId,
                                   VOS_UINT8             OpId,
                                   TAF_PH_OP_MODE_CNF_STRU   stPhMode,
                                   TAF_PH_ERR_CODE       usErrorCode);

/* MMA_PhModeSet MMA_PhModeQuery MMA_PhModeHandle 函数移动位置 */

NAS_STK_SERVICE_STATUS_ENUM_UINT8 NAS_MMA_ConvertSdcServStaToStkServSta(
    TAF_SDC_REPORT_SRVSTA_ENUM_UINT8    enSdcSrvSta
);


/* MMA_PhSysCfgHandle */

VOS_VOID MMA_PhSysCfgSet(VOS_UINT16     ClientId,
                       VOS_UINT8                 OpId,
                       TAF_MMA_SYS_CFG_PARA_STRU *pstSysCfg);

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32  MN_MMA_NeedDetachAttach_CampOnGUMode(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
);
VOS_UINT32  MN_MMA_NeedDetachAttach_CampOnLMode(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
);
VOS_VOID  MN_MMA_UpdateRatTrigedAttachPara(
    TAF_PH_MS_CLASS_TYPE                ucNewMsClassType,
    MMA_SYS_CFG_SET_FLG                *pusSetFlg,
    TAF_PH_ATTACH_TYPE                 *pucAttachType
);

VOS_VOID  MN_MMA_UpdateRatTrigedSrvDomianPara(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara,
    MMA_SYS_CFG_SET_FLG                *pusSetFlg,
    TAF_PH_ATTACH_TYPE                 *pucAttachType,
    TAF_PH_DETACH_TYPE                 *pucDetachType
);

#endif



VOS_UINT32 MMA_PhSysCfgSetCmdCheck(
                        TAF_MMA_SYS_CFG_PARA_STRU    *pstSysCfg,
                        MMA_SYS_CFG_SET_FLG     *pusSetFlg,
                        TAF_PH_ATTACH_TYPE      *pucAttachType,
                        TAF_PH_DETACH_TYPE      *pucDetachType);

VOS_VOID TAF_MMA_ReportSysCfgSetCnf(
    TAF_PH_ERR_CODE                     usErrorCode
);

VOS_VOID MMA_PhSysCfgQuery(VOS_UINT16     ClientId,
                                  VOS_UINT8            OpId);
VOS_VOID MMA_PhSysCfgReport(VOS_UINT16     ClientId,
                                  VOS_UINT8            OpId,
                                  TAF_PH_CMD_TYPE      ucCmdType,
                                  TAF_PH_ERR_CODE       usErrorCode);
VOS_VOID MMA_PhSysCfgSetSrvDomainCheck(
                                    TAF_PH_SERVICE_DOMAIN   ucSrvDomain,
                                    MMA_SYS_CFG_SET_FLG     *pusSetFlg,
                                    TAF_PH_ATTACH_TYPE      *pucAttachType,
                                    TAF_PH_DETACH_TYPE      *pucDetachType);

VOS_VOID MMA_ChangeSrv2Class(
                                        TAF_PH_SERVICE_DOMAIN   ucSrvDomain,
                                        TAF_PH_MS_CLASS_TYPE    *pucMsClass);
VOS_VOID MMA_ChangeClass2Srv(TAF_PH_SERVICE_DOMAIN  *pucSrvDomain,
                                     TAF_PH_MS_CLASS_TYPE    ucMsClass);
/* 删除MMA_FillWholeInfo() */
VOS_UINT32 MMA_GetIdByTi (VOS_UINT8         ucModule,
                          VOS_UINT8         ucTi,
                          VOS_UINT16    *pClientId,
                          VOS_UINT8           *pId,
                          TAF_PHONE_EVENT  *PhoneEvt);
VOS_VOID MMA_GetRssiLevelFromValue( VOS_INT32      lRssiValue, VOS_UINT8    *pucRssiLevel );
VOS_VOID MMA_HandleEventError(VOS_UINT16   ClientId,
                              VOS_UINT8          OpId,
                              VOS_UINT8       ucError,
                              TAF_PHONE_EVENT PhEvent);

VOS_VOID MMA_QryUsimInfo(
    VOS_UINT16        ClientId,
    VOS_UINT8         OpId,
    TAF_PH_QRY_USIM_INFO_STRU           *pstInfo
);

extern VOS_VOID Aps_PowerOff(VOS_VOID);
extern VOS_VOID NAS_RabmPowerOff(VOS_VOID);
extern VOS_VOID Sm_PowerOff(VOS_VOID);
extern VOS_VOID SSA_Init(VOS_VOID);
extern VOS_VOID  SSA_PowerOff(VOS_VOID);
extern VOS_VOID SMS_Poweroff(VOS_VOID);
extern VOS_VOID Cca_Poweroff(VOS_VOID);

extern VOS_UINT32 WAS_MNTN_QueryCSNR(VOS_INT16 *psCpichRscp,VOS_INT16 *psCpichEcNo);
extern VOS_VOID Aps_LoadDefault(VOS_VOID);
extern VOS_VOID Mma_LoadDefault(VOS_VOID);


extern VOS_VOID Tc_TaskInit(VOS_VOID);
extern VOS_VOID MN_CALL_PowerOff(VOS_VOID);
extern VOS_VOID MN_MSG_PowerOff(VOS_VOID);
extern VOS_VOID NAS_CC_PowerOff(VOS_VOID);
extern VOS_UINT32 sysIMEISet( VOS_UINT8 *pucStr,  VOS_UINT32 ulStrlen, VOS_UINT32 ulOffset);
VOS_VOID    MMA_InitCm(VOS_VOID);
VOS_VOID MMA_Init1(VOS_VOID);
VOS_VOID MMA_Init2(VOS_UINT8    ucUsimStatus);
VOS_VOID MMA_InitEventInfoOP(TAF_PHONE_EVENT_INFO_STRU *pstEvent);
VOS_VOID MMA_RdWt_StordedClientOpID(MMA_STORED_CLIENT_OPID_OP_ENUM  enOpType);
VOS_VOID MMA_GetForbBandFromNV(VOS_VOID);

VOS_VOID MMA_GetUESupportBandFromNV(VOS_VOID);

extern VOS_VOID MMA_InitOrigMeInfo( VOS_VOID );


VOS_VOID  MMA_InitUsimInfo(TAF_USIM_INFO_STRU       *pstUsimInfo,
                           MMA_INIT_TYPE_ENUM      enInitType,
                           TAF_PH_USIM_SPN_CNF_STRU *pstSpnCnfTmp);

TAF_PH_ROAM_STA MMA_IsRoam(TAF_SDC_PLMN_ID_STRU *pstCurPlmnId);

VOS_VOID MMA_OpPinExpired(VOS_VOID);
VOS_VOID*  MMA_ParaQueryReport(VOS_UINT16            ClientId,
                              VOS_UINT8                   OpId,
                              TAF_PARA_TYPE      QueryType,
                              VOS_UINT16         usErrorCode,
                              VOS_VOID                 *pucPara);

extern VOS_VOID MN_MMA_SetMsClassTypeDeAttFlg(VOS_VOID);

VOS_VOID MMA_PhoneAttach(VOS_UINT16 ClientId, VOS_UINT8 OpId,
                         TAF_PH_ATTACH_TYPE AttachType);

VOS_UINT32 TAF_MMA_RcvMmcSysInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_MMA_ProcMmcSysInfoInNormalService(
    VOS_UINT32                          ulCellChangeFlg
);

VOS_UINT32 TAF_MMA_RcvMmcAcInfoChangeInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_DbProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SetPlmnUserSel(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SetMtPowerDown(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_TestPrefPlmn(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_TestSyscfg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SetPrefPlmnType(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);


VOS_VOID MMA_PhoneDetach(VOS_UINT16         ClientId,
                         VOS_UINT8                OpId,
                         TAF_PH_DETACH_TYPE    DetachType,
                         TAF_MMA_DETACH_CAUSE_ENUM_UINT8     enDetachCause);

VOS_UINT32 MMA_PhoneGetAlphanumericNameByPlmnIdFromNV( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_UINT32 MMA_PhoneFindNtwkNameByPlmnId( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_VOID MMA_PhoneGetBatteryPower(VOS_UINT16     ClientId,
                                  VOS_UINT8            OpId);
VOS_VOID MMA_PhoneGetDomain(VOS_UINT16  ClientId,  VOS_UINT8  OpId);
VOS_VOID MMA_PhoneGetGmmRegStatus( VOS_UINT16    ClientId, VOS_UINT8      OpId );
VOS_VOID MMA_PhoneGetIdentificationInfo( VOS_UINT16 ClientId, VOS_UINT8    OpId );
VOS_VOID MMA_PhoneGetImsi(VOS_UINT16           ClientId,
                          VOS_UINT8                  OpId);
VOS_VOID MMA_PhoneGetMfrId(VOS_UINT16    ClientId, VOS_UINT8   OpId);
VOS_VOID MMA_PhoneGetModelId(VOS_UINT16           ClientId,
                             VOS_UINT8                  OpId);
VOS_VOID MMA_PhoneGetNetworkName(VOS_UINT16    ClientId,
                                 VOS_UINT8           OpId);
VOS_VOID MMA_PhoneGetNetworkNameFromUsim (VOS_UINT16   ClientId,
                                          VOS_UINT8          OpId);
VOS_UINT32 MMA_PhoneGetOperNameInfoByLongOperatorName( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_UINT32 MMA_PhoneGetOperInfoByLongNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_UINT32 MMA_PhoneGetOperInfoByShortNameFromOperTbl( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_UINT32 MMA_PhoneGetOperNameInfoByShortOperatorName( TAF_PH_OPERATOR_NAME_STRU   *pstOperName);
VOS_VOID MMA_PhoneGetProductName( VOS_UINT16     ClientId, VOS_UINT8    OpId );
VOS_VOID  MMA_PhoneGetRegStatus(VOS_UINT16      ClientId,
                                VOS_UINT8             OpId);
VOS_VOID MMA_PhoneGetRevisionId(VOS_UINT16           ClientId,
                                VOS_UINT8                  OpId);
VOS_VOID MMA_PhoneGetRoamSta(VOS_UINT16    ClientId,   VOS_UINT8 OpId);
VOS_VOID MMA_PhoneGetRssi (VOS_UINT16          ClientId,
                           VOS_UINT8                 OpId);
VOS_VOID MMA_PhoneGetSrvStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId);


VOS_VOID  MMA_PhonePlmnList (VOS_UINT16               ClientId,
                             VOS_UINT8                      OpId);
VOS_VOID MMA_PhonePlmnResel (VOS_UINT16                 ClientId,
                             VOS_UINT8                        OpId,
                             TAF_PLMN_RESEL_MODE_TYPE      ReselType);
VOS_VOID MMA_PhonePlmnUserSel (VOS_UINT16             ClientId,
                               VOS_UINT8                    OpId,
                               TAF_PLMN_ID_STRU          Plmn,
                               TAF_PH_RA_MODE            AccessMode,
                               TAF_PLMN_RESEL_MODE_TYPE  ReselMode);

VOS_UINT32 MMA_PhoneRssiInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_VOID MMA_DbQuery(VOS_UINT16             ClientId,
                     VOS_UINT8                    OpId,
                     TAF_PARA_TYPE       ParaType);


VOS_UINT32 MMA_PhoneUsimReady( VOS_UINT16           ClientId,
                               VOS_UINT8                  OpId,
                               TAF_PARA_TYPE     QueryType );
VOS_VOID MMA_PinDataReport( const TAF_PH_PIN_CNF_STRU *pstPinCnf);
VOS_VOID MMA_PinHandle (VOS_UINT16 ClientId, VOS_UINT8 OpId,
                        TAF_PH_PIN_DATA_STRU stPinData);
VOS_VOID MMA_PlmnListAbort( VOS_UINT16    ClientId, VOS_UINT8    OpId );
VOS_VOID MMA_QueryProc(VOS_UINT16 ClientId,
                       VOS_UINT8        OpId,
                       VOS_UINT8    *pMsg);
VOS_VOID MMA_GetUESupportCPHSFlgFromNV(VOS_VOID);
VOS_VOID MMA_ReadCPHSOperNameStrFileExpired(VOS_VOID);
VOS_VOID MMA_ReadCPHSOperNameStrFileCnf(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg);

VOS_VOID MMA_ReadSimCPHSOperNameStr(VOS_VOID);
VOS_BOOL MMA_IsNeedToDisplayCPHSOperNameStr(VOS_VOID);
VOS_VOID MMA_ReportPNNParaFromCPHSOperNameStr(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
);
VOS_UINT32 MMA_CovertCPHSOperNameStrToPNNFormat(
    TAF_PH_USIM_PNN_RECORD             *pstPNNRcd
);

VOS_VOID   MMA_ReportOPLParaForCPHSDisplay(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
);

VOS_VOID MMA_ReadSpnFileCnf(TAF_PH_USIM_SPN_CNF_STRU *pMsg,TAF_PH_SPN_TYPE enUpdateType);
VOS_VOID MMA_ReadSpnFileExpired(VOS_VOID);
VOS_VOID MMA_ReportParaQueryError( VOS_UINT16       ClientId,
                                   VOS_UINT8              OpId,
                                   VOS_UINT16           usErrorCode,
                                   TAF_PARA_TYPE ParaType);
VOS_VOID MMA_SyncMsClassType(VOS_UINT16        ClientId,
                             VOS_UINT8               OpId,
                             TAF_PH_MS_CLASS_TYPE MsClass);
VOS_UINT32  MMA_TiAlloc(VOS_UINT8        ucModule,
                        VOS_UINT16    ClientId,
                        VOS_UINT8           Id,
                        TAF_PHONE_EVENT  PhoneEvt,
                        VOS_UINT8       *pucTi);
VOS_VOID MMA_UpdateMsClassTypeGolbalValue(TAF_PH_MS_CLASS_TYPE MsClass);
VOS_VOID MMA_UpdateRegInfo( TAF_PH_REG_STATE_STRU *pstCurRegInf );
VOS_UINT32 MMA_WriteValue2Nvim(const NV_ID_ENUM_U16 enNVItemType,
                               VOS_VOID *pData, VOS_UINT16 usDataLen);
VOS_VOID  Taf_GetSpnName(VOS_UINT8*  aucSpnName,VOS_UINT32 ulMaxLen);
VOS_VOID TafPhoneGetNetworkNameForListPlmn(TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU *pstOperName);
VOS_UINT32 Taf_PhoneGetOperNameInfo( TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU *pstOperName );
VOS_VOID  Taf_PhoneEvent (TAF_PHONE_EVENT_INFO_STRU  *pEvent);
VOS_VOID MMA_SetPrefPlmnExpired(VOS_VOID);
VOS_VOID MMA_CleanTiVar(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_UINT32 MMA_ImsiBcd2Ascii(VOS_UINT8 ucLen, VOS_UINT8 *pucNum, VOS_UINT8 *pucAsciiStr);
VOS_UINT32 MMA_ImeiBcd2Ascii(VOS_UINT8 ucLen, VOS_UINT8 *pucNum, VOS_UINT8 *pucAsciiStr);

VOS_UINT32 MMA_AsciiNumToBcdNum(VOS_UINT8 *pucAsciiNum, VOS_UINT8 ucAsciiNumLen, VOS_UINT8 *pucBcdNum, VOS_UINT8 *pucBcdNumLen);

VOS_UINT32 MMA_InitAtMsgReq( VOS_UINT16   ClientId,
                             VOS_UINT8          OpId,
                             TAF_START_INFO_IND_STRU   stAtMsgReq);
VOS_VOID MMA_PhoneEventBroadcast(TAF_PHONE_EVENT_INFO_STRU  *pEvent);
VOS_UINT32 MMA_StartInitialTimer(VOS_VOID);

VOS_VOID MMA_PlmnId2Bcd(TAF_PLMN_ID_STRU *pstPlmnId);
VOS_VOID MMA_PlmnId2NasStyle(TAF_PLMN_ID_STRU *pstPlmnId);

VOS_UINT32 MMA_CheckPin1Satus(VOS_VOID);
VOS_VOID MMA_UsimMsgProc(struct MsgCB *pMsg);
VOS_UINT32 TAF_MMA_IsUsimStatusChanged(
    VOS_UINT32                          ulUsimSta
);

VOS_UINT32 TAF_MMA_IsImsiChanged(
    PS_USIM_STATUS_IND_STRU            *pstUsimMsg
);

VOS_VOID TAF_MMA_ChangeUsimType(VOS_UINT32  ulCardType);


/* MMA_UsimChangeProc移动位置 */
VOS_UINT32 MMA_PhoneGetUsimStatus( VOS_UINT8 *pucSimStatus );
VOS_UINT32 MMA_CheckPuk1Satus(VOS_VOID);
/* MMA_ResetMsClassFromNvim函数删除 */
VOS_UINT32 MMA_CheckPin1SatusSimple(VOS_VOID);
VOS_UINT32 MMA_StartInternalTimer(VOS_UINT32    ulTimes,
                                  VOS_UINT32    ulOpType,
                                  VOS_UINT16 ClientId,
                                  VOS_UINT8        OpId);
VOS_VOID MMA_ExpiredInternalTimer(VOS_VOID);
VOS_UINT32 Mma_CheckPin(TAF_PH_PIN_DATA_STRU stPinData);
VOS_UINT32 Mma_CheckPinTypes(TAF_PH_PIN_TYPE PinTypes);
VOS_UINT32 Mma_CheckPinCmdType(TAF_PH_PIN_CMD_TYPE PinCmdType);
VOS_UINT32 Mma_CheckPinLength(VOS_UINT8 *pucPin);
VOS_UINT32 Mma_CheckPinAvail(VOS_UINT8 *pucPin);
VOS_UINT32 Mma_CheckPinValue(VOS_UINT8 *pucPin);
VOS_VOID MMA_InitTi(VOS_VOID);


VOS_UINT32 MMA_OpPin1Success(const TAF_PH_PIN_CNF_STRU *pstPinCnf);
VOS_UINT32 MMA_Pin1Noneed(VOS_VOID);
VOS_UINT32 MMA_CheckUsimStatus(VOS_VOID);
TAF_PH_MS_CLASS_TYPE MMA_GetCurMsClass(VOS_VOID);
VOS_VOID MMA_ClearCurRegOp(TAF_PH_REG_STATE_STRU  *pstCurRegSta);
VOS_UINT32 MMA_GsmSetFreq(VOS_UINT32 ulGsmFreq);
VOS_UINT32 MMA_PhoneGetUsimType( VOS_UINT8 *pucSimType );
VOS_UINT32 MMA_WcdmaSetFreq(VOS_UINT32 ulDlFreqHighFreq,
                           VOS_UINT32 ulDlFreqLowFreq);
VOS_UINT32 MMA_SetFreq(VOS_UINT32 ulDlFreqHighFreq,
                       VOS_UINT32 ulDlFreqLowFreq,
                       VOS_UINT8  ucRadioAccessMode);
VOS_UINT32 MMA_StopInternalTimer(VOS_VOID);
VOS_VOID MMA_ResetInternalTimerFlg(VOS_VOID);
VOS_VOID MMA_TurnOnForFlightMode(VOS_VOID);
VOS_VOID MMA_TurnOffForFlightMode(VOS_VOID);


TAF_PH_PREF_BAND  MMA_ChangeBandType4Gsm( GSMBANDTYPE Band );
VOS_UINT32 MMA_ChangeBand4Wcdma( TAF_PH_PREF_BAND *pPhBand,
                                 NVIM_UE_SUPPORT_FREQ_BAND_STRU *pstBandSet );
VOS_VOID MMA_PhoneGetFreq(VOS_UINT16 ClientId, VOS_UINT8 OpId);

VOS_BOOL MN_PH_IsUsingPlmnSel(VOS_VOID);
VOS_UINT16 MN_PH_GetUpdateFileForPrefPlmn(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType
);
VOS_UINT32 MN_PH_GetPrefPlmnCnt(
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPLMNType
);

VOS_VOID MMA_PhoneGetUPlmn(TAF_USER_PLMN_LIST_STRU             *pstUPlmnInfo);
VOS_VOID MMA_PhoneGetOPlmn(TAF_USER_PLMN_LIST_STRU             *pstOPlmnInfo);
VOS_VOID MMA_PhoneGetHPlmn(TAF_USER_PLMN_LIST_STRU             *pstHPlmnInfo);
VOS_VOID MMA_SetPrefPlmnCnf(PS_USIM_SET_FILE_CNF_STRU *pstMsg);
VOS_UINT32 MMA_UpdateUsimPrePlmn(VOS_UINT16     ClientId,
                        VOS_UINT8      OpId,
                        TAF_USER_PLMN_LIST_STRU    *pstUPlmnInfo,
                        VOS_UINT16                  usUpdateFile);
VOS_UINT32 MMA_CheckSetUplmn();
VOS_VOID MMA_DefPhImei(TAF_UINT16 usClientId, TAF_UINT8 OpId, TAF_PH_IMEI_STRU* pstImei);
/* 删除MMA_OmMsg 和MMA_UsimMsg函数 */
VOS_VOID  MMA_PhoneGetIccId(VOS_UINT16           ClientId,
                            VOS_UINT8            OpId);
VOS_VOID MMA_GetUsimIccIdFileInd(VOS_VOID);
VOS_VOID MMA_GetUsimIccIdExpired(VOS_VOID);
VOS_VOID  MMA_UsimAccessCnfMsgProc(PS_USIM_GENERIC_ACCESS_CNF_STRU* pMsg);

VOS_UINT32 MMA_UsimRestrictedAccessCnfMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_UsimSetFileCnfMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_UsimGetFileRspMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_UsimRestrictedAccessCommandProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);


VOS_UINT32 MMA_PhoneProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SetCPin(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_GetDomainAttachState(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SetPrefPlmn(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_TimeExpired(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT8 MMA_GetUsimStatus(
    VOS_UINT32                          ulUsimStaChg
);


VOS_VOID MMA_GetPhIccType(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_GetPhIccStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId);
/* VOS_VOID MMA_IccStatusInd(VOS_VOID );
VOS_UINT8 MMA_GetUsimStatus(VOS_VOID); */
VOS_VOID MMA_DefQuickStartStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId,
                                 VOS_UINT32 ulSetValue);
VOS_VOID MMA_DefPhAutoAttachStatus(VOS_UINT16 ClientId, VOS_UINT8 OpId,
                                 VOS_UINT32 ulSetValue);
VOS_VOID MMA_GetQuickStartStatus(VOS_UINT16 ClientId,VOS_UINT8 OpId);
VOS_VOID MMA_FplmnQurey(VOS_UINT16  ClientId,VOS_UINT8 OpId);
VOS_VOID MMA_FplmnHandle(VOS_UINT16 ClientId,VOS_UINT8 OpId,
                                TAF_PH_FPLMN_OPERATE_STRU *pstFplmnOperate);

VOS_UINT32 MMA_LoadDefaultVaule(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_MMA_SndAtIccStatusInd(
    VOS_UINT8                           ucSimStatus,
    VOS_UINT8                           ucSimLockStatus
);

VOS_UINT8 MMA_GetMeLockStatus(VOS_VOID);
VOS_VOID MMA_CPnnQuery(VOS_UINT16 ClientId, VOS_UINT8 OpId,TAF_PH_ICC_TYPE  *pIccType);
VOS_VOID MMA_CPnnReport(VOS_VOID);
VOS_BOOL MMA_IsSimLockPlmnInfoValid(VOS_VOID);
VOS_UINT32 MMA_GetCardLockInfo(TAF_OPERATOR_LOCK_INFO_STRU *pstOperatorLockInfo);
VOS_VOID MMA_MePersonalisationHandle(VOS_UINT16 ClientId,
                                     VOS_UINT8 OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonlisationQuery(VOS_UINT16   ClientId,
                                  VOS_UINT8    OpId,
                                  TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonlisationRetrieve(VOS_UINT16   ClientId,
                                     VOS_UINT8    OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonlisationDeactive(VOS_UINT16   ClientId,
                                     VOS_UINT8    OpId,
                                     TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonlisationActive(VOS_UINT16   ClientId,
                                   VOS_UINT8    OpId,
                                   TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonlisationPwdChange(VOS_UINT16   ClientId,
                                      VOS_UINT8    OpId,
                                      TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID TAF_UsimmStatusCardUnlocked(VOS_VOID);
VOS_VOID MMA_MePersonlisationVerify(VOS_UINT16   ClientId,
                                   VOS_UINT8    OpId,
                                   TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_UINT32   MMA_OperatorPersonlisationVerify(
                                   TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_UINT32 MMA_VerifyOperatorLockPwd(VOS_UINT8 *pucPwd);
VOS_VOID MMA_MePersonlisationSet(VOS_UINT16 ClientId,
                                VOS_UINT8 OpId,
                                TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_UINT32 MMA_CheckMePersonalisationPwd(TAF_ME_PERSONALISATION_DATA_STRU stMePersonalData);
VOS_VOID MMA_MePersonalCnfReport(VOS_UINT16                        ClientId,
                                VOS_UINT8                         OpId,
                                TAF_PH_ME_PERSONLISATION_CNF_STRU stMePersonalCnf);
VOS_UINT32 MMA_CheckMePersonalisationStatus(VOS_VOID);
VOS_VOID MMA_HSQuery(VOS_UINT16 ClientId, VOS_UINT8 OpId);

VOS_VOID MMA_RegisterTimeQuery(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_AnQueryProc(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_HomePlmnProc(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_CsnrParaProc(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_SpnParaProc(VOS_UINT16 ClientId, VOS_UINT8 OpId);
VOS_VOID MMA_CrpnQuery(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MN_MMA_CRPN_QRY_REQ_STRU           *pstMnMmaCrpnQry
);
VOS_VOID MMA_PhCrpnQueryRsp(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    VOS_UINT32                          ulResult,
    MN_MMA_CRPN_QRY_CNF_STRU           *pStMnMmaCrpnQryCnf
);

VOS_VOID MMA_CmmTestCmdProc(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    MM_TEST_AT_CMD_STRU                *pStAtTestCmd
);

VOS_VOID MMA_PhUsimStatusInd(
    PS_USIM_STATUS_IND_STRU           *pStPsUsimStatusInd
);

extern TAF_UINT32 Mm_AtTest(VOS_VOID *pInput, VOS_VOID *pRslt);





VOS_UINT32 TAF_MMA_ConvertDetachTypeToMmc(
    VOS_UINT32                          ulDetachType,
    MMA_MMC_DETACH_TYPE_ENUM_UINT32    *pulMmcDetachType
);

VOS_UINT32 TAF_MMA_ConvertAttachTypeToMmc(
    VOS_UINT32                          ulAttachType,
    MMA_MMC_ATTACH_TYPE_ENUM_UINT32    *pulMmcAttachType
);


VOS_UINT32 TAF_MMA_ConvertPlmnPriorityClass(
    MMC_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                  enPrioClass,
    TAF_MMA_PLMN_PRIORITY_CLASS_ENUM_UINT8                 *penDestPrioClass
);
VOS_UINT32 TAF_MMA_ConvertCardType(
    USIMM_CARD_TYPE_ENUM_UINT32                             enCardType,
    TAF_MMA_USIMM_CARD_TYPE_ENUM_UINT32                    *penDestCardType
);
VOS_UINT32 TAF_MMA_ConvertCardStatus(
    USIMM_CARD_SERVIC_ENUM_UINT32       enCardStatus,
    TAF_MMA_CARD_STATUS_ENUM_UINT8     *penDestCardStatus
);

VOS_UINT32 TAF_MMA_ConvertServiceStatus(
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8                       enServiceStatus,
    TAF_PHONE_SERVICE_STATUS                               *penDestServiceStatus
);

VOS_UINT32 MMA_SndMmcDetachAttachReq(VOS_UINT32 ulMsgName,VOS_UINT32 ulDetachAttachType);
VOS_UINT32 MMA_PhSysCfgWriteNvim(VOS_VOID);
VOS_UINT32  MMA_SwitchUserSetBand2UESupportBand
( MMA_USER_BAND_SET_UN uUserSetBand,
  MMA_UE_SUPPORT_FREQ_BAND_STRU *pstUeBand
);

VOS_UINT32  MMA_SwitchUESupportBand2UserSetBand
(
    MMA_UE_SUPPORT_FREQ_BAND_STRU stUeBand,
    MMA_USER_BAND_SET_UN *puUserSetBand
);

/* MMA_TafMsgProc 函数删除 */

VOS_UINT32 MMA_GetUsimStatusFromUsim(VOS_UINT8 *pucCardStatus, VOS_UINT8 *pucCardType);
VOS_UINT8 MMA_CheckUsimStatusForPlmnSel(VOS_VOID);
VOS_UINT32 MMA_IsGsmForbidden(VOS_VOID );
VOS_UINT32 NAS_MMA_SndOutsideContextData(VOS_VOID);
VOS_UINT32 NAS_MMA_SndOutsideFixedContextData(VOS_VOID);
VOS_UINT8 MMA_GetCurrentPhoneMode(VOS_VOID);
VOS_UINT32 MMA_ClearEplmnFromNV(VOS_VOID);

#if (VOS_WIN32 == VOS_OS_VER)
VOS_VOID   NAS_MMA_SndNVData(VOS_VOID);
VOS_UINT32 NAS_MMA_SndNVFileData(const VOS_CHAR * aucNVFolder ,const VOS_CHAR * aucFileName);
#endif

VOS_VOID MN_PH_ResetTimerOutProc( VOS_VOID  );

VOS_VOID MN_PH_PinOperTimerOutProc( VOS_VOID );

VOS_VOID NAS_MMA_TransferSysSubMode2ActForAtCmdCreg(
    VOS_UINT8                           ucSysSubMode,
    TAF_PH_ACCESS_TECH_ENUM_UINT8       *pucAct
);
extern VOS_VOID NAS_MMA_ReportLociStatus(VOS_VOID);

extern VOS_VOID NAS_MMA_SndStkLociStatusEvent(VOS_VOID);
VOS_VOID TAF_MMA_SndStkLocationInfoInd (VOS_VOID);


extern VOS_BOOL NAS_MMA_CellIdChangeForStkLocSta(VOS_VOID);


VOS_VOID MN_PH_SimLockedRpt( VOS_VOID  );
VOS_VOID MN_PH_RoamingRpt( VOS_UINT8 ucTmpRoamStatus );
VOS_VOID MN_PH_AppRoamStatusRpt( VOS_VOID );



MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8 MN_MMA_CovertPlmnRatFromSimToUserFormat(VOS_UINT16 usSimPlmnRat);
VOS_UINT16 MN_MMA_CovertPlmnRatFromUserToSimFormat(MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8 ucPlmnRat);


VOS_UINT8 NAS_MMA_GetPhoneMode(VOS_VOID);

VOS_VOID MN_MMA_ConvertGUFrequencyBand(
    VOS_UINT32                         *pulPrefBand
);

VOS_UINT32 MN_MMA_IsGOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
);

VOS_UINT32 MN_MMA_IsWOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
);

VOS_UINT32 MN_MMA_IsGUDualMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
);


#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 MN_MMA_IsLOnlyMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
);

#endif

VOS_VOID MN_MMA_ReadNvimRoamingBrokerInfo(VOS_VOID);
VOS_UINT8 MN_MMA_GetRoamingBrokerFlg(VOS_VOID);
VOS_VOID MN_MMA_GetCpamGURatPrio(
   MN_MMA_CPAM_RAT_PRIO_ENUM_UINT8     *pucPlmnPrio,
   TAF_PH_RAT_ORDER_STRU               *pstAllRatOrder
);
VOS_VOID MN_MMA_GetCpamGUAccessRat(
   MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8     *penAccessMode,
   TAF_PH_RAT_ORDER_STRU               *pstAllRatOrder
);
VOS_VOID MMA_PrefPlmnHandle(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_PH_PREF_PLMN_OPERATE_STRU       *pstPrefPlmnOperate
);
VOS_UINT32 MN_MMA_IsSpecRatSupported(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enSpecRat
);

TAF_PH_RAT_TYPE_ENUM_UINT8 MN_MMA_GetUserPlmnSelPrioRat(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enPhRat,
    TAF_PLMN_ID_STRU                    stPlmn
);

VOS_UINT32 MN_MMA_IsSpecRatInRatList(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enSpecRat,
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
);


VOS_UINT32 MMA_FormatPlmnStr2PlmnId(TAF_PLMN_ID_STRU *pPlmn,
                                                 VOS_UINT32        usFormat,
                                                 VOS_UINT16        usOprNameLen,
                                                 VOS_CHAR         *pucOprName);

VOS_VOID Taf_InitEventInfoOP(TAF_PHONE_EVENT_INFO_STRU *pstEvent);
VOS_VOID Sta_PlmnListEventReport (VOS_UINT32 ulOpID);



#if(FEATURE_ON == FEATURE_LTE)

VOS_VOID NAS_MMA_GetEpsRegStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);
#endif

VOS_VOID TAF_MMA_RcvSimLockQry(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId
);

VOS_UINT32 MN_PH_QryLocinfo (
    VOS_UINT32                         *pulMcc,
    VOS_UINT32                         *pulMnc,
    VOS_UINT16                         *pusLac,
    VOS_UINT8                          *pucRac,
    VOS_UINT32                         *pulCellId
);
/* VOS_VOID MMA_SaveWCipherInfo(
    MMC_MMA_UMTS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd
);
VOS_VOID MMA_SaveGCipherInfo(
    MMC_MMA_GPRS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd
); */



VOS_VOID MMA_QryCipherInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_MMA_CIPHER_QRY_REQ_STRU         *pstInfo
);

VOS_VOID MMA_QryLocInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_MMA_LOCATION_INFO_REQ_STRU      *pstInfo
);


VOS_VOID TAF_MMA_GetUsimHplmnMncLenFileInd(PS_USIM_GET_FILE_CNF_STRU *pUsimTafMsg);

VOS_UINT32 TAF_MMA_RcvOamConnectStatusInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 TAF_MMA_RcvOamTraceCfgReq(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SaveWCipherInfo(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_SaveGCipherInfo(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);





VOS_UINT32  TAF_MMA_IsSysCfgSetValid(
    TAF_MMA_SYS_CFG_PARA_STRU           *pstSysCfgPara
);
VOS_UINT32  MN_MMA_IsUserSettedBandValid(
    TAF_MMA_SYS_CFG_PARA_STRU           *pstSysCfgPara
);


VOS_UINT32 NAS_MMA_MntnTraceGetUsimStatus(
    VOS_UINT8                           ucUsimStatus,
    VOS_UINT8                           ucMeLockStatus
);
/* VOS_VOID TAF_MMA_InitNetworkSelectionMenuCtx(VOS_VOID);
VOS_VOID TAF_MMA_InitRatBalancingCtx(VOS_VOID); */
VOS_VOID TAF_MMA_ReadAttEnsCustomNvim(VOS_VOID);
VOS_VOID TAF_MMA_SetNetworkSelectionMenuEnableFlg(VOS_UINT8 ucEnableFlg);
VOS_UINT8 TAF_MMA_GetNetworkSelectionMenuEnableFlg(VOS_VOID);
VOS_UINT32 TAF_MMA_ReadCustomerServiceProfileFile_SwitchOn(VOS_VOID);
VOS_UINT32 TAF_MMA_RcvCustomerServiceProfileFile_PreProc(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
);
VOS_VOID TAF_MMA_ProcCustomerServiceProfileFile(VOS_VOID);
VOS_VOID TAF_MMA_RcvCustomerServiceProfileFile(
    VOS_UINT32                          ulRslt,
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
);
/* VOS_VOID TAF_MMA_ReadPlmnSelectionModeNvim(VOS_VOID); */
VOS_VOID TAF_MMA_SetPlmnSelectionMode(VOS_UINT8 ucPlmnSelMode);
VOS_UINT8 TAF_MMA_GetPlmnSelectionMode(VOS_VOID);
VOS_VOID TAF_MMA_SetAutoPlmnSelUser(TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 enAutoPlmnSelUser);
TAF_MMA_AUTO_PLMN_SEL_USER_ENUM_UINT8 TAF_MMA_GetAutoPlmnSelUser(VOS_VOID);
VOS_UINT8 TAF_MMA_GetTiTryingPeriodRatBalancingStatus(VOS_VOID);
VOS_VOID TAF_MMA_SetTiTryingPeriodRatBalancingStatus(VOS_UINT8 ucStatus);
VOS_VOID TAF_MMA_SetSyscfgUser(TAF_MMA_SYSCFG_USER_ENUM_UINT8 enSyscfgUser);
TAF_MMA_SYSCFG_USER_ENUM_UINT8 TAF_MMA_GetSyscfgUser(VOS_VOID);
VOS_VOID TAF_MMA_NetworkSelectionMenuCtrlTimerExpired(VOS_VOID);
VOS_VOID TAF_MMA_RatBalancingCtrlTimerExpired(VOS_VOID);
VOS_VOID TAF_MMA_SetPlmnMode_Refresh(TAF_MMA_PLMN_MODE_ENUM_UINT8 enPlmnMode);
TAF_MMA_PLMN_MODE_ENUM_UINT8 TAF_MMA_GetPlmnMode_Refresh(VOS_VOID);
VOS_VOID TAF_MMA_ReadRatBalancingNvim(VOS_VOID);
VOS_UINT32 TAF_MMA_RcvRatModeFileRefresh_PreProc(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
);
VOS_VOID TAF_MMA_RcvRatModeFileRefresh(
    VOS_UINT16                          usEfLen,
    VOS_UINT8                          *pucEf
);
VOS_UINT8 TAF_MMA_GetTiTryingPeriodNetSelMenuStatus(VOS_VOID);
VOS_VOID TAF_MMA_SetTiTryingPeriodNetSelMenuStatus(VOS_UINT8 ucStatus);
VOS_VOID TAF_MMA_SetRatBalancingEnableFlg(VOS_UINT8 ucEnableFlg);
VOS_UINT8 TAF_MMA_GetRatBalancingEnableFlg(VOS_VOID);
VOS_VOID TAF_MMA_TransferRatMode(
    VOS_UINT8                           ucRatMode,
    TAF_PH_RAT_ORDER_STRU              *pstRatModeOrder
);
VOS_UINT32 TAF_MMA_ReadRatModeFile(VOS_VOID);
VOS_VOID TAF_MMA_ReadRatPrioListNvim(VOS_VOID);
VOS_VOID TAF_MMA_SetRatMode_Refresh(TAF_MMA_RAT_MODE_ENUM_U8 enRatMode);
TAF_MMA_RAT_MODE_ENUM_U8 TAF_MMA_GetRatMode_Refresh(VOS_VOID);
VOS_UINT32 TAF_MMA_ProcRatMode_EnableStatus(TAF_MMA_RAT_MODE_ENUM_U8 ucRatMode);
VOS_UINT32 TAF_MMA_ProcRatMode(TAF_MMA_RAT_MODE_ENUM_U8 enRatMode);
VOS_UINT32 TAF_MMA_CheckRatModePara(
    TAF_PH_RAT_ORDER_STRU              *pstCurRatModeOrder
);
VOS_VOID  TAF_MMA_SndOmGetCacheFile(
    VOS_UINT32                          ulFileId,
    VOS_UINT32                          ulFileLen,
    VOS_UINT8                          *pucFileContent
);

VOS_VOID  TAF_MMA_SndOmLocationStatus(VOS_VOID);
VOS_UINT32 TAF_MMA_RcvAppSyscfg_SysCfgSet(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);

VOS_UINT32 TAF_MMA_RcvAppSyscfg_PlmnResel(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg
);


VOS_VOID TAF_MMA_ReadNvimReportPlmnSupportFlg(VOS_VOID);
VOS_VOID TAF_MMA_PlmnIdReport(
    TAF_PLMN_ID_STRU                   *pstPlmn
);



VOS_VOID TAF_MMA_StopPlmnListTimer_UserAbort( VOS_VOID );

VOS_VOID  TAF_MMA_SndDrxTimerInfo( VOS_UINT8 ucDrxTimerStatus );

VOS_VOID  TAF_MMA_LogReadWriteNvInfo(
    VOS_UINT32                          ulNvId,
    VOS_UINT32                          ulRslt,
    VOS_UINT8                           *pucNVInfo,
    VOS_UINT32                          ulNvInfoLen
);

VOS_VOID TAF_MMA_ReadNvimRatPrioList(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
);

VOS_VOID MN_MMA_SetDefaultRatPrioList(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
);

VOS_UINT32 TAF_MMA_IsPlatformSupportSpecUserRat(
    TAF_PH_RAT_TYPE_ENUM_UINT8          enUserRat
);
VOS_VOID TAF_MMA_RcvTafMsgAcInfoQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_AC_INFO_QUERY_REQ_STRU     *pstAcInfoQuery
);


VOS_VOID TAF_MMA_SaveCellAcInfo(
    MMC_MMA_AC_INFO_CHANGE_IND_STRU    *pstAcInfoInd
);
VOS_VOID TAF_MMA_InitCellAcInfo(VOS_VOID);

/* VOS_VOID TAF_MMA_RcvMmcAcInfoChangeInd(
    MMC_MMA_AC_INFO_CHANGE_IND_STRU    *pstAcInfoChangeInd
); */


VOS_VOID TAF_MMA_DelPlatformUnsupportedRat(
    TAF_PH_RAT_ORDER_STRU              *pstRatPrioList
);

VOS_VOID TAF_MMA_QryMmPlmnInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);
VOS_VOID TAF_MMA_TranslateNtwkName2Str(
    VOS_UINT8                       *pucIeNtwkName,
    VOS_CHAR                        *pucNtwkName,
    VOS_UINT32                       ulLen
);
VOS_UINT8 NAS_MMA_TranslateNtwkName2UCS2(
    VOS_UINT8                   *pucSrcNtwkName,
    VOS_CHAR                    *pucDstNtwkName,
    VOS_UINT8                    ucDstLen
);

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_VOID TAF_MMA_SndMtcSysInfo(
    VOS_UINT8                           ucIsFobbiddenPlmnFlag,
    VOS_UINT8                           ucPsSupportFlg
);

VOS_VOID TAF_MMA_ConvertRatModeToMtc(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode,
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode,
    MTC_RATMODE_ENUM_UINT8             *pucRatMode
);


VOS_VOID TAF_MMA_SndMtcEplmnInfoInd(
    MMC_MMA_EPLMN_INFO_IND_STRU        *pstEplmnInfoIndMsg
);
#endif

/**********************************************************
 函 数 名  : TAF_MMA_QryPlmnPara
 功能描述  : 查询PH PLMN消息处理
 输入参数  : usClientId
             ucOpId
 输出参数  : 无
 返 回 值  :
*************************************************************/
VOS_VOID TAF_MMA_QryPlmnPara(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);

VOS_VOID TAF_MMA_RcvTafMsgCerssiInfoQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CERSSI_INFO_QUERY_REQ_STRU *pstCerssiInfoQuery
);

VOS_VOID TAF_MMA_InitRssiValue(VOS_VOID);

VOS_VOID NAS_MMA_JudgeServiceStatus(TAF_PHONE_EVENT_INFO_STRU *pstPhoneEvent);

VOS_UINT32 TAF_MMA_IsModeChangeNeedRpt(VOS_VOID);
VOS_UINT32 TAF_MMA_IsServStatusChangeNeedRpt(VOS_VOID);
VOS_UINT32 TAF_MMA_IsRssiChangeNeedRpt(VOS_VOID);
VOS_UINT32 TAF_MMA_IsTimeChangeNeedRpt(VOS_VOID);
VOS_UINT32 TAF_MMA_IsSimStaChangeNeedRpt(VOS_VOID);
VOS_UINT32 TAF_MMA_IsRegStatusChangeNeedRpt(
    TAF_PHONE_EVENT_INFO_STRU  *pEvent
);
MMA_MMC_ALLOWED_REG_DOMAIN_ENUM_UINT8 TAF_MMA_GetRegDomain(VOS_VOID);
VOS_UINT32 TAF_MMA_IsEventNeedRpt(
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
);


extern VOS_VOID TAF_MMA_DefaultAlphaToAscii(
    VOS_UINT8                          *pucDefAlpha,
    VOS_UINT8                           ucDefAlphaLen,
    VOS_UINT8                          *pucAsciiChar
);

VOS_VOID TAF_MMA_RcvTafMsgCopnQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_COPN_INFO_QUERY_REQ_STRU   *pstCopnInfo
);


VOS_UINT32 MMA_ProcHotInOutUsimStatusInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_USIMMaxRecordNumCnfProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_UINT32 MMA_UsimPINOperateMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);



VOS_VOID TAF_MMA_UsimRefreshIndAllFileChangedMsgProc(PS_USIM_REFRESH_IND_STRU *pUsimTafMsg);
VOS_VOID TAF_MMA_UsimRefreshIndFileListChangedMsgProc(PS_USIM_REFRESH_IND_STRU *pUsimTafMsg);

VOS_VOID TAF_MMA_QryUserSrvStatePara(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);
VOS_VOID MMA_WriteVersion(VOS_VOID);
VOS_VOID TAF_MMA_ReadSpnFile(VOS_VOID);

VOS_VOID MMA_ChangeUsimStatus(VOS_UINT32  ulUsimSta);

VOS_VOID  TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK(
    PWC_CLIENT_ID_E                     enPwcClientId
);

VOS_VOID  TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK(
    PWC_CLIENT_ID_E                     enPwcClientId
);

VOS_VOID TAF_MMA_RcvTafMsgSimInsertInd(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_MMA_SIM_INSERT_IND_STRU        *pstSimInsertState
);

VOS_VOID MN_MMA_Convert64BitBandTo32Bit(
    TAF_USER_SET_PREF_BAND64           *pstBand,
    VOS_UINT32                         *pulBand
);
VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceAbsent(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
);

VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceAvailable(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
);
VOS_UINT32 TAF_MMA_IsUsimStatusChange_UsimmCardServiceSimPin(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enPreSimStatus
);    

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_VOID TAF_MMA_SndMtcRatModeInd(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode,
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode
);

VOS_VOID TAF_MMA_SndMtcPowerStateInd(MTC_MODEM_POWER_STATE_ENUM_UINT8 enPowerState);

VOS_VOID TAF_MMA_SndMtcCurrCampPlmnInfoInd(
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU   *pstPlmnInfo
);
VOS_VOID TAF_MMA_SndMtcRegStatusInd(VOS_UINT8 ucIsUsimValidFlag);


#endif

#if (VOS_WIN32 == VOS_OS_VER)
VOS_VOID TAF_MMA_SetSimLockStatus(VOS_UINT8 ucSimLockStatus);
#endif
VOS_UINT32 TAF_MMA_RcvMmcEOPlmnSetCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_VOID TAF_MMA_RcvMsgEOPlmnSetReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnSetPara
);
VOS_VOID TAF_MMA_RcvMsgEOPlmnQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);

VOS_VOID TAF_MMA_UpdateSimRegStatus(VOS_VOID);

VOS_VOID TAF_MMA_NetScanTimerExpired(VOS_VOID);

VOS_VOID TAF_MMA_AbortNetScanTimerExpired(VOS_VOID);


VOS_VOID TAF_MMA_RcvTafMsgNetScanReq(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanSetPara
);

VOS_VOID TAF_MMA_RcvTafMsgAbortNetScanReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);


VOS_UINT32 TAF_MMA_StartNetScanProtectTimer(VOS_VOID);

VOS_UINT32 TAF_MMA_StopNetScanProtectTimer(VOS_VOID);

VOS_UINT32 TAF_MMA_StartAbortNetScanProtectTimer(VOS_VOID);

VOS_UINT32 TAF_MMA_StopAbortNetScanProtectTimer(VOS_VOID);

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_VOID TAF_MMA_SndMtcCurrCampPlmnInfoInd(
    MMA_MTC_CURR_CAMP_PLMN_INFO_IND_STRU   *pstPlmnInfo
);
#endif

VOS_VOID TAF_MMA_RcvTafMsgCpolQueryReq(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CPOL_INFO_QUERY_REQ_STRU   *pstCpolInfo
);
VOS_VOID TAF_MMA_BuildMmaCtrlInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CTRL_STRU                  *pstCtrlInfo
);

VOS_UINT32 TAF_MMA_IsPowerOnCLInterWork(VOS_VOID);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __MMA_APPLOCAL_H__ */

