/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       :   comm_nv_def.h
  Description     :   gutl多模公共nv结构体定义
  History         :
******************************************************************************/

#ifndef __COMM_NV_DEF_H__
#define __COMM_NV_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NV_WLCOUNTRY_CODE_LEN     (5)
#define NV_WLMODE_LEN             (5)
#define NV_WLSSID_LEN             (33)
#define NV_FACTORY_INFO_I_SIZE    (78)
/* 版本信息每条的最大长度 */
#define VER_MAX_LENGTH            (30)

#define KPD_NV_UNIT               (16)  /* total 16 * 32Bit = 64bytes */
#define KPD_EVENT_MAX             (KPD_NV_UNIT-3)


/*NV ID = 50018*/
#pragma pack(1)
typedef struct
{
    BSP_S32        nvStatus;
    BSP_S8         nv_version_info[30];
}NV_SW_VER_STRU;
#pragma pack(4)

/*END NV ID = 50018*/

/*Resume_Flag   ID = 4*/
typedef struct
{
    BSP_U16                              Resume_Flag;    /*[0, 1]*/
}RESUME_FLAG_STRU;


/*LED_Control   ID = 7*/

typedef struct
{
	BSP_U8  Color;
	BSP_U8  Time;
}LED_STRU;

typedef struct
{
    LED_STRU                                     stLED[10];
}LED_CONTROL_STRU;
typedef struct
{
    LED_CONTROL_STRU                             stLED_Control[32];
}LED_CONTROL_STRU_ARRAY;





/*UE_Point_Ctrl   ID = 15*//*未使用*/
/*****************************************************************************
 结构名    : UE_POINT_CTRL_STRU
 结构说明  : UE_POINT_CTRL结构 ID=15
*****************************************************************************/
typedef struct
{
    BSP_U32     Status;
    BSP_U32     ProductForm;
    BSP_U16     Pid;
    BSP_U16     FakePid;
    BSP_U8      Mac[8];
}UE_POINT_CTRL_STRU;

/*ExceptionRecord 临终遗言设置   ID = 17*/
/*****************************************************************************
 结构名    : EXCEPTION_RECORD_STRU
 结构说明  : EXCEPTION_RECORD结构
*****************************************************************************/
typedef struct
{
    BSP_U32     IsEnable;
    BSP_U32     Config;
    BSP_U32     Reserve;
}EXCEPTION_RECORD_STRU;
/*sysPrtTask   ID = 20*/
/*数据类型：32bit unsigned，长度为4byte*/
/*****************************************************************************
 结构名    : NV_SYS_PRT_STRU
 结构说明  : NV_SYS_PRT结构
*****************************************************************************/
typedef struct
{
    BSP_U32    uintValue;
}NV_SYS_PRT_STRU;


/*usbEnumStatus ID = 21*/
/*****************************************************************************
 结构名    : USB_ENUM_STATUS_STRU
 结构说明  : USB_ENUM_STATUS结构
*****************************************************************************/
typedef struct
{
    BSP_U32    status;
    BSP_U32    value;
    BSP_U32    reserve1;
    BSP_U32    reserve2;
}USB_ENUM_STATUS_STRU;


/*配置单板上电后WatchDog存活记录?ID =22*/
/*****************************************************************************
 结构名    : LIVE_TIME_CONTROL_STRU
 结构说明  : LIVE_TIME_CONTROL结构
*****************************************************************************/
typedef struct
{
    BSP_U32    ulEnable;
    BSP_U32    ulCycle;
} LIVE_TIME_CONTROL_STRU;



/*LiveTime   ID = 23*/
/*****************************************************************************
 结构名    : LIVE_TIME_STRU
 结构说明  : LIVE_TIME结构
*****************************************************************************/
typedef struct
{
    BSP_U32    ulLiveTime;
}LIVE_TIME_STRU;


/*NV_CUSTOMIZE_REWIND_DELAY_TIME_I   ID = 25*/ /*未使用*/
/*****************************************************************************
 结构名    : NV_CUSTOMIZE_REWIND_DELAY_TIME_I_STRU
 结构说明  : NV_CUSTOMIZE_REWIND_DELAY_TIME_I结构 ID=25
*****************************************************************************/
typedef struct
{
    BSP_U32     Status;    /*Range:[0,1]*/
    BSP_U16     Timeout;
}NV_CUSTOMIZE_REWIND_DELAY_TIME_I_STRU;


/*配置UE上报固定SN号*/   /*ID = 26 长度 16*/
/*****************************************************************************
 结构名    : USB_SN_NV_INFO_STRU
 结构说明  : USB_SN_NV_INFO结构
*****************************************************************************/
typedef struct
{
    BSP_U32    usbSnNvStatus;
    BSP_U32    usbSnNvSucFlag;
    BSP_U32    usbSnReserved1;
    /*BSP_U32    usbSnReserved2;*/
}USB_SN_NV_INFO_STRU;




/*硬件版本号。ID = 30 长度 12*/
/*****************************************************************************
 结构名    : NVHWVER
 结构说明  : NVHWVER结构
*****************************************************************************/
typedef struct
{
    BSP_U32    NV_HW_VER_Flag;
    BSP_U8     NV_HW_VER_VAL[8];
} NVHWVER;


/*PID优化方案开关 ID = 31 长度 4*/
/*****************************************************************************
 结构名    : nv_pid_enable_type
 结构说明  : nv_pid_enable_type结构
*****************************************************************************/
typedef struct
{
    BSP_U32    pid_enabled;
}nv_pid_enable_type;


/*NV_AT_SHELL_OPEN_FLAG   ID = 33*/
/*****************************************************************************
 结构名    : NV_AT_SHELL_OPEN_FLAG_STRU
 结构说明  : NV_AT_SHELL_OPEN_FLAG结构
*****************************************************************************/
typedef struct
{
    BSP_U32    NV_AT_SHELL_OPEN_FLAG;
}NV_AT_SHELL_OPEN_FLAG_STRU;


/*用于电池的温度查询。温度量纲为1℃为基准单元 ID = 35*/

/*****************************************************************************
 结构名    : TEMP_ADC_STRU
 结构说明  : TEMP_ADC结构
*****************************************************************************/
typedef struct
{
    BSP_S16    sTemp;
    BSP_U16    usADC;
}TEMP_ADC_STRU;

/*****************************************************************************
 结构名    : TEMP_ADC_STRU_ARRAY
 结构说明  : TEMP_ADC_STRU_ARRAY结构
*****************************************************************************/
typedef struct
{
    TEMP_ADC_STRU    stTempAdc[28];
}TEMP_ADC_STRU_ARRAY;


/*DRV_CUSTOMIZE_TYPE   ID = 41 长度*/
/*****************************************************************************
 结构名    : nv_drv_customize_type
 结构说明  : nv_drv_customize_type结构
*****************************************************************************/
typedef struct DRV_CUSTOMIZE_TYPE
{
    BSP_U32 drv_customize_type;
} nv_drv_customize_type;

/*NV_OLED_TEMP_ADC   ID = 49*/
/*****************************************************************************
 结构名    : NV_OLED_TEMP_ADC_STRU
 结构说明  : NV_OLED_TEMP_ADC结构
*****************************************************************************/
typedef struct
{
    BSP_S16       sTemp;
    BSP_S16       sADC;
}NV_OLED_TEMP_ADC_STRU;

/*****************************************************************************
 结构名    : NV_OLED_TEMP_ADC_STRU_ARRAY
 结构说明  : NV_OLED_TEMP_ADC_STRU_ARRAY结构  ID=49
*****************************************************************************/
typedef struct
{
    NV_OLED_TEMP_ADC_STRU          stNV_OLED_TEMP_ADC[28];
}NV_OLED_TEMP_ADC_STRU_ARRAY;

/*NV_ITEM_MODIFY_LINKINFO_FLAG   ID = 50*/
/*****************************************************************************
 结构名    : NV_ITEM_MODIFY_LINKINFO_FLAG_STRU
 结构说明  : NV_ITEM_MODIFY_LINKINFO_FLAG结构
*****************************************************************************/
typedef struct
{
    BSP_U16    NV_ITEM_MODIFY_LINKINFO_FLAG;  /*Range:[0,1]*/
}NV_ITEM_MODIFY_LINKINFO_FLAG_STRU;

/*NV_WEBNAS_SD_WORKMODE   ID = 51*/
/*****************************************************************************
 结构名    : NV_WEBNAS_SD_WORKMODE_STRU
 结构说明  : NV_WEBNAS_SD_WORKMODE结构
*****************************************************************************/
typedef struct
{
    BSP_U32 ulSDWorkMode;
}NV_WEBNAS_SD_WORKMODE_STRU;

/*HW_WDT_FLAG   ID = 52*/
/*****************************************************************************
 结构名    : HW_WDT_FLAG_STRU
 结构说明  : HW_WDT_FLAG结构
*****************************************************************************/
typedef struct
{
    BSP_U32   HW_WDT_FLAG;
}HW_WDT_FLAG_STRU;

/*POWER_LED_ENABLE   ID = 54*/
/*****************************************************************************
 结构名    : POWER_LED_ENABLE_STRU
 结构说明  : POWER_LED_ENABLE结构
*****************************************************************************/
typedef struct
{
    BSP_U32   POWER_LED_ENABLE;
}POWER_LED_ENABLE_STRU;

/*****************************************************************************
 结构名    : NV_HW_CONFIG_STRU
 结构说明  : NV_HW_CONFIG结构 ID = 55
*****************************************************************************/
/* PMU每一路配置,8Byte */
typedef struct
{
	BSP_U8 PowerEn;
	BSP_U8 ModuleID;
	BSP_U8 DevID;
	BSP_U8 PowerID;
	BSP_U8 ConsumerID;
	BSP_U8 Reserve;
	BSP_U16 Voltage;
}PMU_POWER_CFG;

/* PMU配置,16路 */
typedef struct
{
	PMU_POWER_CFG RF0_PA;
	PMU_POWER_CFG RFIC0_TX;
	PMU_POWER_CFG RFIC0_RX;
	PMU_POWER_CFG FEM0;
	PMU_POWER_CFG RF0_VBIAS;

	PMU_POWER_CFG RF1_PA;
	PMU_POWER_CFG RFIC1_TX;
	PMU_POWER_CFG RFIC1_RX;
	PMU_POWER_CFG FEM1;
	PMU_POWER_CFG RF1_VBIAS;

	PMU_POWER_CFG PMU_RESERVE0;
	PMU_POWER_CFG PMU_RESERVE1;
	PMU_POWER_CFG PMU_RESERVE2;
	PMU_POWER_CFG PMU_RESERVE3;
	PMU_POWER_CFG PMU_RESERVE4;
	PMU_POWER_CFG PMU_RESERVE5;
}PMU_CFG_STRU;

/*过热保护设置*/
typedef struct
{
	BSP_U8 OTMP_TMP;          /*过热保护温度设置*/
	BSP_U8 OTMP_PWR_DOWN;     /*过热后，非核心电源是否下点*/
	BSP_U8 OTMP_RESET;        /*过热后，是否进行软复位*/
	BSP_U8 RESERVE;
}PMU_PROTECT_STRU;

typedef struct
{
	BSP_U32 RESERVE0;           	/* 保留,4Byte */
	BSP_U16 W_PA_V;            		/* W PA电压 2Byte */
	BSP_U16 G_PA_V;             	/* G PA电压 2Byte */
	BSP_U32 USB_EYE_CFG;        	/* USB眼图 4Byte */
	BSP_U32 APT_MODE;           	/* APT 模式 4Byte，参见DR_APT_CFG_ENUM */
	PMU_PROTECT_STRU PMU_PROTECT;   /* PMU 过热保护 4Byte*/
	BSP_U32 DRV_LOG_CTRL;       	/* DRVLOG打印开关 4Byte */
	BSP_U32 DRV_LOG_LEVEL;      	/* DRVLOG打印等级 4Byte */
	BSP_U32 RESERVE7;           	/* 保留,4Byte */

	/*PMU NV项配置*/
	PMU_CFG_STRU PMU_CFG;
	BSP_U32 PMU_CRC32;	     /* PMU CRC校验,4Byte */

	/* 保留,剩余23(*)4Byte */
	BSP_U32 RESERVE41;
	BSP_U32 RESERVE42;
	BSP_U32 RESERVE43;
	BSP_U32 RESERVE44;
	BSP_U32 RESERVE45;
	BSP_U32 RESERVE46;
	BSP_U32 RESERVE47;
	BSP_U32 RESERVE48;
	BSP_U32 RESERVE49;
	BSP_U32 RESERVE50;
	BSP_U32 RESERVE51;
	BSP_U32 RESERVE52;
	BSP_U32 RESERVE53;
	BSP_U32 RESERVE54;
	BSP_U32 RESERVE55;
	BSP_U32 RESERVE56;
	BSP_U32 RESERVE57;
	BSP_U32 RESERVE58;
	BSP_U32 RESERVE59;
	BSP_U32 RESERVE60;
	BSP_U32 RESERVE61;
	BSP_U32 RESERVE62;
	BSP_U32 RESERVE63;
}NV_HW_CONFIG_STRU;



/*通过该NV项控制各按键的触发时间和响应时间。此时间为产品线定制，不得随意修改。ID = 56 长度 64*/
/*****************************************************************************
 结构名    : KPD_NV_VERSION_1_1_T
 结构说明  : KPD_NV_VERSION_1_1_T结构 ID = 56
*****************************************************************************/
typedef struct
{
    BSP_U32  ulversion;                    /* Should be 0x0001_0000. major + minor */
    BSP_U32  ulkeyupdatetime;              /* 0 means not support. non-zero is the update check time */
    BSP_U32  ultickunit;                   /* the tick unit. ms */
    BSP_U32  ulitemtable[ KPD_EVENT_MAX ]; /* sizeof(versio_1_1) = 64bytes */
}KPD_NV_VERSION_1_1_T;                     /* version 1.1 structure */


/*WIFI_TARGET_ASSERT_ENABLE   ID = 57*/
/*****************************************************************************
 结构名    : HWIFI_TARGET_ASSERT_ENABLE_STRU
 结构说明  : HWIFI_TARGET_ASSERT_ENABLE结构
*****************************************************************************/
typedef struct
{
    BSP_U32   WIFI_TARGET_ASSERT_ENABLE;
}HWIFI_TARGET_ASSERT_ENABLE_STRU;

/*HILINK_AUTORUN_FLAG   ID = 61 长度 2*/
/*****************************************************************************
 结构名    : HILINK_AUTORUN_FLAG_STRU
 结构说明  : HILINK_AUTORUN_FLAG结构
*****************************************************************************/
typedef struct
{
    BSP_U16   HILINK_AUTORUN_FLAG;
}HILINK_AUTORUN_FLAG_STRU;

/*****************************************************************************
 结构名    : NV_FACTORY_INFO_I_STRU
 结构说明  : NV_FACTORY_INFO_I结构 ID=114
*****************************************************************************/
typedef struct
{
    BSP_CHAR aucFactoryInfo[NV_FACTORY_INFO_I_SIZE];
}NV_FACTORY_INFO_I_STRU;

/*NV_Battery ID = 90*/
/*****************************************************************************
 结构名    : VBAT_CALIBART_TYPE
 结构说明  : 电池校准参数数据结构
*****************************************************************************/
typedef struct
{
    BSP_U16 min_value;
    BSP_U16 max_value;
}VBAT_CALIBART_TYPE;

/*NV_USB_LOG_SAVE_LEV   ID = 118*/

/*****************************************************************************
 结构名    : NV_USB_LOG_SAVE_LEV
 结构说明  : NV_USB_LOG_SAVE_LEV结构
*****************************************************************************/
typedef struct
{
    BSP_U32 NV_USB_LOG_SAVE_LEV;
}NV_USB_LOG_SAVE_LEV_STRU;

/*NV_Usb_Mntn_Flag   ID = 119 长度 2*/
/*****************************************************************************
 结构名    : NV_ITEM_USB_PKT_HOOK_STRU
 结构说明  : NV_ITEM_USB_PKT_HOOK结构 ID=119
*****************************************************************************/
typedef struct
{
    BSP_U16 usUsbPktHookFlag;
}NV_ITEM_USB_PKT_HOOK_STRU;   /*USB勾包NV控制项*/


/*****************************************************************************
 结构名    : NV_SCI_CFG_STRU
 结构说明  : NV_SCI_CFG结构 ID=128
*****************************************************************************/
typedef struct
{
    BSP_U32 value;
} NV_SCI_CFG_STRU;

/*存储不同网络下的速率 ID = 50027*/
/*****************************************************************************
 结构名    : nv_huawei_connect_display_rate_type
 结构说明  : nv_huawei_connect_display_rate_type结构     ID=50027
*****************************************************************************/
typedef struct
{
    BSP_U8 gsm_connect_rate;
    BSP_U8 gprs_connect_rate;
    BSP_U8 edge_connect_rate;
    BSP_U8 wcdma_connect_rate;
    BSP_U8 hspda_connect_rate;
    BSP_U8 reserved;           /*对齐使用*/
}nv_huawei_connect_display_rate_type;

/*设备枚举过程中上报的PID信息 ID = 50071*/
/*****************************************************************************
 结构名    : nvi_cust_pid_type
 结构说明  : nvi_cust_pid_type结构
*****************************************************************************/
typedef struct
{
    BSP_U32  nv_status;
    BSP_U16  cust_first_pid;
    BSP_U16  cust_rewind_pid;
}nvi_cust_pid_type;

/*WIFI的SSID，产品线使用，试制时写入 ID=  52000*/
/*****************************************************************************
 结构名    : nv_wifibs_type
 结构说明  : nv_wifibs_type结构
*****************************************************************************/
typedef struct
{
    BSP_U8   aucwlSsid[NV_WLSSID_LEN];
    BSP_U32  ulwlChannel;
    BSP_U32  ulwlHide;
    BSP_U8   aucwlCountry[NV_WLCOUNTRY_CODE_LEN];
    BSP_U8   aucwlMode[NV_WLMODE_LEN];
    BSP_U32  ulwlRate;
    BSP_U32  ulwlTxPwrPcnt;
    BSP_U32  ulwlMaxAssoc;
    BSP_U8   ucwlEnbl;
    BSP_U32  ulwlFrgThrshld;
    BSP_U32  ulwlRtsThrshld;
    BSP_U32  ulwlDtmIntvl;
    BSP_U32  ulwlBcnIntvl;
    BSP_U32  ulwlWme;
    BSP_U32  ulwlPamode;
    BSP_U32  ulwlIsolate;
    BSP_U32  ulwlProtectionmode;
    BSP_U32  ulwloffenable;
    BSP_U32  ulwlofftime;
    BSP_U8   aucwlExtends[12];
}nv_wifibs_type;


/*低温保护 ID = 52005*/
/*****************************************************************************
 结构名    : CHG_BATTERY_LOW_TEMP_PROTECT_NV
 结构说明  : CHG_BATTERY_LOW_TEMP_PROTECT_NV结构 ID=52005
*****************************************************************************/
typedef struct
{
    BSP_U32  ulIsEnable;
    BSP_S32  lCloseAdcThreshold;
    BSP_U32  ulTempLowCount;
}CHG_BATTERY_LOW_TEMP_PROTECT_NV;

/*****************************************************************************
 结构名    : nv_wifi_info
 结构说明  : nv_wifi_info结构
*****************************************************************************/
typedef struct
{
    BSP_U32 ulOpSupport;
    BSP_U8  usbHighChannel;
    BSP_U8  usbLowChannel;
    BSP_U8  aucRsv1[2];
    BSP_U16 ausbPower[2];
    BSP_U8  usgHighsChannel;
    BSP_U8  usgLowChannel;
    BSP_U8  aucRsv2[2];
    BSP_U16 ausgPower[2];
    BSP_U8  usnHighsChannel;
    BSP_U8  usnLowChannel;
    BSP_U8  ausnRsv3[2];
    BSP_U16 ausnPower[2];
}nv_wifi_info;


/*设备枚举过程中上报的端口形态类型 ID = 50091*/
typedef struct PACKED_POST
{
    BSP_U32  nv_status;
    BSP_U8  first_port_style[17];
    BSP_U8  rewind_port_style[17];
    BSP_U8  reserved[22];
} nv_huawei_dynamic_pid_type;


/*NV_SEC_BOOT_ENABLE_FLAG   ID = 50201*/
/*****************************************************************************
 结构名    : NV_SECBOOT_ENABLE_FLAG
 结构说明  : NV_SECBOOT_ENABLE_FLAG结构 ID=50201
*****************************************************************************/
typedef struct
{
    BSP_U16 usNVSecBootEnableFlag; /*Range:[0,1]*/
}NV_SECBOOT_ENABLE_FLAG;


/* NV_ID_DRV_PPP_DIAL_ERR_CODE            = 50061, */
typedef struct
{
    BSP_U8                               Ppp_Dial_Err_Code[2];
}PPP_DIAL_ERR_CODE_STRU;


/*****************************************************************************
 结构名    : PROIDNvInfo
 结构说明  : PROIDNvInfo结构     ID=50018
*****************************************************************************/
typedef struct _productIDNvInfo
{
    BSP_U32   NvStatus;
    BSP_U8    ucCompVer[VER_MAX_LENGTH];
} PROIDNvInfo;


/*****************************************************************************
 结构名    : DR_NV_ENHANCE_SIMCARD_LOCK_STATUS_STRU
 结构说明  : 增强的锁卡状态结构体   ID=8517
*****************************************************************************/
typedef struct
{
    BSP_U8  EnhanceSimcardStatus;     /* 增强的锁卡特性是否激活 0:不激活, 1: 激活 */
    BSP_U8  SimcardStatus;            /* SimCardLockStatus各种状态 1: 锁卡版本, 2: 非锁卡版本 */
}DR_NV_ENHANCE_SIMCARD_LOCK_STATUS_STRU;

/*****************************************************************************
 结构名    : nv_protocol_base_type
 结构说明  : nv_protocol_base_type结构 ID= 52110
*****************************************************************************/
typedef struct
{
    BSP_U32  nv_status;
    BSP_U32  protocol_base;
}nv_protocol_base_type;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


