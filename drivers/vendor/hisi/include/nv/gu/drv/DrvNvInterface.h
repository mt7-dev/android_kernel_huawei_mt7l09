/******************************************************************************

  Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : DrvNvInterface.h
  Description     : DrvNvInterface.h header file
  History         :

******************************************************************************/

#ifndef __DRVNVINTERFACE_H__
#define __DRVNVINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
/*#include "MODULE_H/drv_global.h"*/

/*****************************************************************************
  2 Macro
*****************************************************************************/

#define DYN_MAX_PORT_CNT          (17)

/* 设备枚举最大端口个数 */
//#define DYNAMIC_PID_MAX_PORT_NUM  (17)

#define NV_HW_CFG_LENTH           (0x40)

#ifndef VER_MAX_LENGTH
#define VER_MAX_LENGTH                  30
#endif
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
*                                                                            *
*                           参数设置消息结构                                 *
*                                                                            *
******************************************************************************/


/*****************************************************************************
 结构名    : NV_WDT_TIMEOUT_STRU
 结构说明  : NV_WDT_TIMEOUT结构 ID=11
*****************************************************************************/
typedef struct
{
    BSP_U32   ulWdtTimeOut;
}NV_WDT_TIMEOUT_STRU;


/*****************************************************************************
 结构名    : USIM_TEMP_SENSOR_TABLE
 结构说明  : USIM_TEMP_SENSOR_TABLE结构
*****************************************************************************/
typedef struct
{
    BSP_S16   Temperature;
    BSP_U16   Voltage;
}USIM_TEMP_SENSOR_TABLE;

/*****************************************************************************
 结构名    : USIM_TEMP_SENSOR_TABLE_STRU
 结构说明  : USIM_TEMP_SENSOR_TABLE结构
*****************************************************************************/
typedef struct
{
    USIM_TEMP_SENSOR_TABLE UsimTempSensorTable[19];
}USIM_TEMP_SENSOR_TABLE_STRU;



/*****************************************************************************
 结构名    : CHG_TEMP_ADC_TYPE
 结构说明  : CHG_TEMP_ADC_TYPE结构
*****************************************************************************/
typedef struct 
{
    BSP_S16   temperature;
    BSP_U16   voltage;
}CHG_TEMP_ADC_TYPE;

/*****************************************************************************
 结构名    : FACTORY_MODE_TYPE
 结构说明  : FACTORY_MODE_TYPE结构 
             工厂模式nv项,0 : 工厂模式 1: 非工厂模式 36
*****************************************************************************/
typedef struct
{
    BSP_U32 factory_mode;
}FACTORY_MODE_TYPE;



/*****************************************************************************
 结构名    : NV_BREATH_LED_STR
 结构说明  : NV_BREATH_LED结构     ID=9051
*****************************************************************************/
typedef struct
{
    BSP_U8 ucBreathOnTime;
    BSP_U8 ucBreathOffTime;
    BSP_U8 ucBreathRiseTime;
    BSP_U8 ucBreathFallTime;
}NV_BREATH_LED_STR;

/*****************************************************************************
 结构名    : CHG_BATTERY_TEMP_PROTECT_NV_TYPE
 结构说明  : CHG_BATTERY_TEMP_PROTECT_NV_TYPE结构     ID=50016
*****************************************************************************/
typedef struct 
{
    BSP_U32  ulIsEnable;
    BSP_S32  lCloseAdcThreshold;
    BSP_U32  ulTempOverCount;
}CHG_BATTERY_TEMP_PROTECT_NV_TYPE;


/*****************************************************************************
 结构名    : PROIDNvInfo
 结构说明  : PROIDNvInfo结构     ID=50018,ID=50048
*****************************************************************************/
typedef struct _productIDNvInfo
{
    BSP_U32   NvStatus;
    BSP_U8    ucCompVer[VER_MAX_LENGTH];
} PROIDNvInfo;

/*****************************************************************************
 结构名    : NV_HUAWEI_PCCW_HS_HSPA_BLUE_STRU
 结构说明  : NV_HUAWEI_PCCW_HS_HSPA_BLUE结构 ID=50032
*****************************************************************************/
typedef struct
{
    BSP_U32  NVhspa_hs_blue; /*Range:[0,3]*/
}NV_HUAWEI_PCCW_HS_HSPA_BLUE_STRU;




/*****************************************************************************
 结构名    : nv_huawei_dynamic_pid_type
 结构说明  : nv_huawei_dynamic_pid_type结构 ID=50091 端口形态
*****************************************************************************/
typedef struct PACKED_POST
{
    BSP_U32 nv_status;
    BSP_U8  first_port_style[DYNAMIC_PID_MAX_PORT_NUM];
    BSP_U8  rewind_port_style[DYNAMIC_PID_MAX_PORT_NUM];
    BSP_U8  reserved[22];
} nv_huawei_dynamic_pid_type;


/*****************************************************************************
 结构名    : USB_NV_SN_INFO_T
 结构说明  : USB_NV_SN_INFO_T结构
*****************************************************************************/
typedef struct tagUSB_NV_SN_INFO_T
{
    BSP_U32 u32USBNvState;
    BSP_U32 u32USBReserved1;
    BSP_U32 u32USBReserved2;
    BSP_U32 u32USBNvResult;
} USB_NV_SN_INFO_T;

/*****************************************************************************
 结构名    : USB_NV_DEV_INFO_T
 结构说明  : USB_NV_DEV_INFO_T结构
*****************************************************************************/
typedef struct tagUSB_NV_DEV_INFO_T
{
    BSP_U32 u32USBNvState;
    BSP_U32 u32USBDevTypeIdx;
    BSP_U32 u32USBReserved1;
    BSP_U32 u32USBReserved2;
/*    BSP_U32 u32USBNvResult;*/
} USB_NV_DEV_INFO_T;

/*****************************************************************************
 结构名    : USB_NV_SERIAL_NUM_T
 结构说明  : USB_NV_SERIAL_NUM_T结构
*****************************************************************************/
typedef struct tagUSB_NV_SERIAL_NUM_T
{
    BSP_U32 u32USBSerialNumSupp;
    BSP_U32 u32USBReserved1;
    BSP_U32 u32USBReserved2;
    BSP_U32 u32USBNvResult;
} USB_NV_SERIAL_NUM_T;

/*****************************************************************************
 结构名    : USB_NV_NEW_PID_INFO_T
 结构说明  : USB_NV_NEW_PID_INFO_T结构
*****************************************************************************/
typedef struct tagUSB_NV_NEW_PID_INFO_T
{
    BSP_U32 u32USBNewPidSupp;
} USB_NV_NEW_PID_INFO_T;

/*****************************************************************************
 结构名    : USB_NV_PID_UNION_T
 结构说明  : USB_NV_PID_UNION_T结构
*****************************************************************************/
typedef struct tagUSB_NV_PID_UNION_T
{
    BSP_U32 u32USBNvState;
    BSP_U32 u32USBProtOfs;   /* change from "u32USBVDFCust" to "u32USBProtOfs" */
    BSP_U32 u32USBCdromPid;
    BSP_U32 u32USBUniquePid;
} USB_NV_PID_UNION_T;

/*****************************************************************************
 结构名    : USB_NV_DEV_PROFILE_T
 结构说明  : USB_NV_DEV_PROFILE_T结构
*****************************************************************************/
typedef struct tagUSB_NV_DEV_PROFILE_T
{
    BSP_U32 u32USBNvState;
    BSP_U8  u8USBFirstPortSeq[DYN_MAX_PORT_CNT];
    BSP_U8  u8USBMultiPortSeq[DYN_MAX_PORT_CNT];
    BSP_U8  u8USBPortReserved[2];
} USB_NV_DEV_PROFILE_T;


/*****************************************************************************
 结构名    : NV_GCF_TYPE_CONTENT_STRU
 结构说明  : NV_GCF_TYPE_CONTENT结构 ID=8250
*****************************************************************************/
typedef struct
{
    BSP_U8 GCFTypeContent[4];
}NV_GCF_TYPE_CONTENT_STRU;



/*****************************************************************************
 结构名    : TEM_VOLT_TABLE
 结构说明  : TEM_VOLT_TABLE结构
*****************************************************************************/
typedef struct tem_volt_table
{
    BSP_S16   temperature;
    BSP_U16   voltage;
}TEM_VOLT_TABLE;



/*****************************************************************************
 结构名    : NV_TCXO_CFG_STRU
 结构说明  : NV_TCXO_CFG结构 ID=9217
*****************************************************************************/
typedef struct
{
    BSP_U32 tcxo_cfg;
}NV_TCXO_CFG_STRU;

/*****************************************************************************
 结构名    : NV_KADC_PHYTOLOGICAL_CONFIGTCXO_CFG_STRU
 结构说明  : NV_KADC_PHYTOLOGICAL_CONFIGTCXO_CFG结构 ID=9216
*****************************************************************************/
typedef struct
{
    BSP_U16 hkadc[14];
}NV_KADC_PHYTOLOGICAL_CONFIGTCXO_CFG_STRU;

/*****************************************************************************
 结构名    : NV_THERMAL_HKADC_CONFIG
 结构说明  : NV_THERMAL_HKADC_CONFIG结构 ID=9215
*****************************************************************************/
typedef struct
{
    BSP_U16 hkadc[32];
}NV_KADC_CHANNEL_CFG_STRU;
typedef struct
{
    BSP_U32   outconfig;       
    BSP_U16   outperiod;   
    BSP_U16   convertlistlen;  
    BSP_U32   reserved[2];    
}NV_THERMAL_HKADC_CONFIG;

/*****************************************************************************
 结构名    : NV_THERMAL_HKADC_CONFIG_STRU
 结构说明  : NV_THERMAL_HKADC_CONFIG结构 ID=9215
*****************************************************************************/
typedef struct
{
    NV_THERMAL_HKADC_CONFIG   hkadcCfg[14];    
}NV_THERMAL_HKADC_CONFIG_STRU;

/*****************************************************************************
 结构名    : NV_THERMAL_BAT_CONFIG_STRU
 结构说明  : NV_THERMAL_BAT_CONFIG结构 ID=9214
*****************************************************************************/
typedef struct
{
    BSP_U16       enable;       
    BSP_U16       hkadcid;      
    BSP_S16       highthres;        
    BSP_U16       highcount;     
    BSP_S16       lowthres;             
    BSP_U16       lowcount;    
    BSP_U32       reserved[2]; 
  
}NV_THERMAL_BAT_CONFIG_STRU;

/*****************************************************************************
 结构名    : NV_THERMAL_TSENSOR_CONFIG_STRU
 结构说明  : NV_THERMAL_TSENSOR_CONFIG结构 ID=9213
*****************************************************************************/
typedef struct
{
    BSP_U32       enable;          
    BSP_U32       lagvalue0;
    BSP_U32       lagvalue1;
    BSP_U32       thresvalue0;
    BSP_U32       thresvalue1;
    BSP_U32       rstthresvalue0;
    BSP_U32       rstthresvalue1;
    BSP_U32       alarmcount1;
    BSP_U32       alarmcount2;
    BSP_U32       resumecount;
    BSP_U32       acpumaxfreq;
    BSP_U32       gpumaxfreq;
    BSP_U32       ddrmaxfreq;
    BSP_U32       reserved[4]; 
}NV_THERMAL_TSENSOR_CONFIG_STRU;

/*****************************************************************************
 结构名    : NV_TUNER_MIPI_INIT_CONFIG_STRU
 结构说明  : NV_TUNER_MIPI_INIT_CONFIG_STRU结构 ID=10046
*****************************************************************************/
#define TUNER_USER_DATA_MAX_NUM (9)
#define TUNER_MAX_NUM           (2)
typedef struct
{
    BSP_U8 mipiChan;
    BSP_U8 validNum;
    BSP_U16 cmdFrame[TUNER_USER_DATA_MAX_NUM];
    BSP_U16 dataFrame[TUNER_USER_DATA_MAX_NUM];
}NV_TUNER_MIPI_FRAME_CONFIG_STRU;

typedef struct
{
    NV_TUNER_MIPI_FRAME_CONFIG_STRU RFInitReg[TUNER_MAX_NUM];
}NV_GU_RF_FEND_MIPI_INIT_CONFIG_STRU;
/*****************************************************************************
 结构名    : WG_DRX_RESUME_TIME_STRU
 结构说明  : WG_DRX_RESUME_TIME_STRU结构 ID=10031
*****************************************************************************/
typedef struct
{
    unsigned long ulAbbPwrRsmTime;
    unsigned long ulRfPwrRsmTime;
    unsigned long ulAntSwitchPwrRsmTime;
    unsigned long ulAbbWpllRsmTime;
    unsigned long ulAbbGpllRsmTime;
    unsigned long ulBbpPllRsmTime;
    unsigned long ulPaStarRsmTime;
    unsigned long ulSysProtectTime;
    unsigned long ulTcxoRsmTime;
    unsigned long ulDcxoRsmTime;
    unsigned long ulSlowToMcpuRsmTime;
    unsigned long ulWphyRsmTime;
    unsigned long ulGphyRsmTime;
    unsigned long ulTaskSwitchRsmTime;
    unsigned long ulPaPwrRsmTime;
}WG_DRX_RESUME_TIME_STRU;

/*****************************************************************************
 结构名    : NV_NPNP_CONFIG_INFO
 结构说明  : NV_NPNP_CONFIG_INFO
*****************************************************************************/
typedef struct
{
    BSP_U32 npnp_open_flag;                           /* NPNP 特性一级NV开关, 0为未开启不可以使用, 1为开启可以使用 */
    BSP_U32 npnp_enable_flag;                         /* NPNP 特性二级NV项, 0为特性未使能, 1为特性使能             */
}NV_NPNP_CONFIG_INFO;
/*****************************************************************************
 结构名    : NV_RF_GPIO_CFG_STRU
 结构说明  : RF_GPIO中有若干可以复用为DRV_GPIO
             在一些射频功能场景需要改变这些RF_GPIO引脚的复用功能及输出状态
             通过该NV配置 ID=10047
*****************************************************************************/
typedef struct
{
    BSP_U16 is_used;    /*Range:[0,1]*/
	BSP_U16 gpio_level; /*Range:[0,1]*/
}RF_MODEM_CONTROL;
typedef struct
{
    BSP_U32 rf_gpio_num;
    RF_MODEM_CONTROL modem_inside;
    RF_MODEM_CONTROL modem_outside;
}RF_GPIO_CFG;
typedef struct
{
    RF_GPIO_CFG rf_switch_cfg[16];
}NV_RF_SWITCH_CFG_STRU;
/*****************************************************************************
 结构名    : NV_RF_ANT_OTG_CFG_STRU
 结构说明  : 主、副卡的天线热拔插功能需要根据不同的产品形态决定是否打开，能够
             通过该NV配置，并且通过该NV配置使用哪一根GPIO引脚作为中断输入
             ID=10051
*****************************************************************************/
typedef struct
{
    BSP_U16 ANT0_GPIO_NUM;
    BSP_U16 ANT1_GPIO_NUM;
}NV_RF_ANT_OTG_CFG_STRU;

/*****************************************************************************
 结构名    : NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY
 结构说明  : NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY ID=148
*****************************************************************************/
typedef struct
{
    BSP_U16         usProductType;/*0:MBB V3R3 stick/E5,etc.; 1:V3R3 M2M & V7R2; 2:V9R1 phone; 3:K3V3&V8R1;*/
    BSP_U16         usRsv;
}NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/








#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of DrvNvInterface.h */
