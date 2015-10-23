/******************************************************************************

  Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : PhyNvDefine.h
  Description     : PhyNvDefine.h header file
  History         :

******************************************************************************/
#include "uphy_type_define.h"
#include "ucom_nvdefine_union.h"


#ifndef __PHYNVDEFINE_H__
#define __PHYNVDEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(1)


/*****************************************************************************
  2 宏定义
*****************************************************************************/


/* xiongjiangjiang Begine */
#define UCOM_NV_EDGE_TX_TEMP_COMP_NUM           ( 16 )
#define UCOM_NV_EDGE_TX_RF_GAIN_CTRL_NUM        ( 20 )


#define UCOM_NV_G_RAMP_PWR_LEVEL_NUM        ( 7 )                               /* 依次以10dBm,20dBm划分为3个爬坡功率区间 */
#define UCOM_NV_G_TX_REF_CHAN_NUM_MAX       ( 32 )                              /* TX频率补偿校准频点最大个数 */


#define UCOM_NV_G_TX_8PSK_PWR_CONT_NUM      ( 30 )                              /* (自研)8PSK需要校正的发射功率点为30个 */
#define UCOM_NV_G_TX_PWR_CONT_NUM           ( 60 )                              /* 需要校正的发射功率点为60个 */


#define UCOM_NV_G_TEMP_LEVEL_NUM            ( 16 )                              /* [温度档位,共5档:-20~0,0~20,20~40,40~60,60~85] */
#define UCOM_NV_G_AGC_LEVEL_MAX_NUM         ( 8 )
#define UCOM_NV_G_GAIN_THRESHOLD_MAX_NUM    ( UCOM_NV_G_AGC_LEVEL_MAX_NUM - 1 )  /* UCOM_NV_G_AGC_LEVEL_MAX_NUM - 1 */
#define UCOM_G_RX_OTHER_GAIN_DCR_CHAN_NUM   ( UCOM_NV_G_AGC_LEVEL_MAX_NUM - 1 ) /* 本频其它档位DCR校准结果 */


#define UCOM_NV_G_CHAN_GAIN_MAX_NUM         ( 8 )                               /* 接收增益档数，与AGC档位呈对应关系 */

#define UCOM_NV_G_TX_POWER_LEVEL_NUM        ( 3 )                               /* 将发射功率划分为3个等级 */
#define UCOM_NV_G_TX_CHAN_COMP_NUM          ( 32 )                              /* 8PSK TX频率补偿参考补偿点个数 */
#define UCOM_NV_G_PCL_NUM                   ( 32 )                              /* 功率等级个数 */
#define UCOM_NV_G_TX_SLOT_MAX_NUM           ( 4 )                               /* 功率等级个数 */

#define UCOM_NV_G_TX_ATTEN_NUM              ( 55 )                              /* 8PSK 衰减个数 */

#define UCOM_NV_G_RX_CHAN_COMP_NUM          ( 16 )                              /* 低频段参考补偿点个数 */
#define UCOM_NV_G_HIGH_BAND_CHAN_COMP_NUM   ( 16 )                              /* 高频段参考补偿点个数 */

#define UCOM_NV_G_RX_GSM850_GAIN0_DCR_NUM   ( 42 )
#define UCOM_NV_G_RX_GSM900_GAIN0_DCR_NUM   ( 65 )
#define UCOM_NV_G_RX_DCS1800_GAIN0_DCR_NUM  ( 125 )
#define UCOM_NV_G_RX_PCS1900_GAIN0_DCR_NUM  ( 100 )
#define UCOM_G_RX_GAIN0_DCR_MAX_NUM         ( 125 )

#if ( FEATURE_ON == FEATURE_VIRTUAL_BAND )
#define UCOM_NV_G_BAND_NUM                  ( 5 )                               /* GSM Narrow Dcs共五个频段 */
#else
#define UCOM_NV_G_BAND_NUM                  ( 4 )                               /* GSM共四个频段 */
#endif
#define UCOM_NV_G_BAND_NUMBER               ( 4 )                               /* GSM共四个频段 */
#define UCOM_NV_G_HIGH_BAND_NUM             ( 2 )                               /* 高频段包括1800/1900两个频段 */
#define UCOM_NV_G_LOW_BAND_NUM              ( 2 )                               /* 低频段包括850/900两个频段 */
#define UCOM_NV_G_HIGH_BAND_INDEX           ( 0 )                               /* 高频段包括1800/1900两个频段 */
#define UCOM_NV_G_LOW_BAND_INDEX            ( 1 )                               /* 低频段包括850/900两个频段 */
#define UCOM_NV_G_HIGH_LOW_BUTT             ( 2 )                               /* G模区分高低BAND */
#define UCOM_NV_G_SUBBAND_NUM               ( 3 )                               /* 每频段内子频段个数 */
#define UCOM_NV_G_RAMP_COEF_NUM             ( 16 )                              /* 爬坡点数 */

/* xiongjiangjiang End */

#define UCOM_NV_W_BAND_SECTION_NUM                  ( 3 )                       /* W BAND SECTION */
#define UCOM_NV_WCDMA_PA_EN_LINE_NUM        ( 4 )                               /* w pa 4 line */

/* 最大HSUPA ETFCI 表格个数 */
#define UCOM_NV_HSUPA_ETFCI_TABLE_MAX_NUM   ( 6 )

#define UCOM_NV_W_TX_TEMP_COMP_NUM                  ( 16 )                      /* 每个RF Gain档位的温度补偿数目 */

#define UCOM_NV_W_TX_RF_GAIN_CTRL_NUM               ( 20 )                      /* RF增益档位理论数目 */

#define UCOM_W_MAX_RF_GAIN_CTRL_NUM             ( 20 )                          /* rf gain ctrl最大档位数 */

#define UCOM_W_MAX_RF_BIAS_NUM                  ( 16 )                          /* rf BIAS最大档位数 */
#define UCOM_W_MAX_RF_BB_GAIN_NUM               ( 32 )                          /* rf BB GAIN最大档位数 */

#define UCOM_NV_MAX_RF_VCO_CONFIG_NUM               ( 4 )                       /* RF VCO CONFIG */
#define UCOM_NV_W_TX_RF_BIAS_NUM                    ( 0x4 )                     /* RF TX BIAS */
#define UCOM_NV_W_TX_DPA_IQ_REDUCTION_NUM           ( 15 )                      /* DPA业务数字回退 */
#define UCOM_NV_W_TX_SCUPA_IQ_REDUCTION_NUM         ( 26 )                      /* SC-UPA业务数字回退 */
#define UCOM_NV_W_TX_DPA_MPR_NUM                    ( 15 )                      /* DPA业务的功率回退 */
#define UCOM_NV_MAX_MMMBPA_CMD_NUMBER                ( 8 )                       /* 控制字命令最大数目 */
#define UCOM_NV_MAX_MIPI_ANT_CMD_NUMBER              ( 4 )                       /* ant switch控制字命令最大数目 */
#define UCOM_NV_W_ANT_NUM                            ( 2 )                        /* 2天线 */
#define UCOM_NV_W_TX_SCUPA_MPR_NUM                   ( 26 )                      /* SC-UPA业务的功率回退 */
#define UCOM_NV_W_TX_DCUPA_MPR_NUM                   ( 7 )                       /* SC-UPA业务的功率回退 */
#define UCOM_NV_SAR_BACKOF_NUM                       ( 9 )                               /* 降SAR共分9个等级 */
#define UCOM_NV_W_PA_GAIN_SWITCH_COMP_BOUNDARY_NUM  ( 5 )                       /* W TX PA增益切换点补偿,每个BAND区分5个区间 */
#define UCOM_NV_APT_MAX_NUM                 ( 32 )                              /* 每个档位下APT补偿的最大个数 */
#define UCOM_NV_PA_GAIN_MAX_NUM             ( 3 )                               /* PA档位个数 */
#define UCOM_WCDMA_UARFCN_NUM               ( 0x10 )                            /* 每个频段下UARFCN个数 */
#define UCOM_NV_W_TX_PA_MODE_NUM            ( 3 )                               /* PA 增益个数:高中低 */
#define UCOM_NV_W_TEMP_NUM                  ( 0x10  )                           /* 温度补偿设置的温度点，前5个分别代表 -20,0,20,40,60，第6个用于字节对齐 */
#define UCOM_NV_W_TX_PA_TEMP_NUM            ( 0x10 )                            /* TX PA的温度补偿设置的温度点，由工具下发，共16个 */
#define UCOM_NV_W_TX_FREQ_COMP_NUM          ( 0x10 )                            /* TX PA的温度补偿设置的温度点，由工具下发，共16个 */


#define UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM             ( 0x8 )
#define UCOM_NV_W_AGC_GAIN_NOBLK_THRESHOLD_NUM        ( UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM -1 )  /* 6361中AGC GAIN档位切换门限的个数
                                                         该值依赖于档位=(UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM - 1) */

#define UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM               ( 0x8 )                                           /* NV项中block档位个数 */
#define UCOM_NV_W_AGC_GAIN_BLK_THRESHOLD_NUM          ( UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM - 1 )    /* 6361中AGC GAIN档位切换门限的个数
                                                                  UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM - 1 */

#define UCOM_NV_W_PA_NONLINEAR_NUM                  ( 32 )                      /* PA非线性补偿 */
#define UCOM_NV_W_TX_HDET_DPA_CM_COMP_NUM           ( 15 )                      /* DPA业务PD的CM补偿 */
#define UCOM_NV_W_TX_HDET_SCUPA_CM_COMP_NUM         ( 26 )                      /* SC-UPA业务PD的CM补偿 */

#define UCOM_NV_W_APT_FREQ_COMP_NUM                 ( 16 )
#define UCOM_NV_W_APT_TEMP_COMP_NUM                 ( 16 )

#define UCOM_NV_W_CHANNEL_NUM               ( 3 )                               /* NV项每个BAND区分高中低三个信道 */
#define UCOM_NV_W_PA_SWITCH_COMP_SLOT       ( 4 )                               /* Pa切换补偿时隙数 */
#define UCOM_NV_W_APC_ATTEN_NUM             ( 0x58 )
#define UCOM_NV_W_TX_HDET_PV_NUM            ( 0x10 )                            /* PA的工作电压个数 */
#define UCOM_NV_W_PA_GAIN_SWITCH_COMP_CHAN_NUM      ( 4 )                       /* W TX PA增益切换点补偿的4个分界频点 */
#define UCOM_NV_W_TX_APT_SCUPA_CM_COMP_NUM          ( 26 )                      /* SC-UPA业务APT的CM补偿 */

#define UCOM_NV_W_TX_APT_DPA_CM_COMP_NUM            ( 15 )                     /* DPA业务APT的CM补偿 */


#define UCOM_NV_W_TOTAL_BAND_NUM            ( 0x06 )
#define UCOM_NV_RFABB_INIT_REG_NUM          ( 60 )                             /* 初始化定义了60个 */

/* tuner MIPI接口初始化寄存器最大个数 */
#define UCOM_NV_TUNER_MIPI_COMM_INIT_REG_NUM    ( 6 )

/* AP+Sensor手持状态个数,目前支持5种状态 */
#define UCOM_NV_TUNER_HANDLE_STATUS_NUM         ( 5 )

/* idle时，tuner支持的最大MIPI个数 */
#define UCOM_NV_TUNER_MIPI_IDLE_CMD_MAX_NUM     ( 6 )

/* active时，w tuner支持的最大MIPI个数 */
#define UCOM_NV_W_TUNER_MIPI_CMD_MAX_NUM        ( 10 )
#define UCOM_NV_W_TUNER_MIPI_DATA_MAX_NUM       ( UCOM_NV_W_TUNER_MIPI_CMD_MAX_NUM )

/* active时,G tuner支持的最大MIPI个数 */
#define UCOM_NV_GSM_TUNER_MIPI_CMD_MAX_NUM      ( 6 )
#define UCOM_NV_GSM_TUNER_MIPI_DATA_MAX_NUM     ( UCOM_NV_GSM_TUNER_MIPI_CMD_MAX_NUM )

#define UCOM_G_RX_GAIN0_DCR_CHAN_NUM            ( 16 )                          /*DCR channel NUM */
/* DCXO多项式系数的个数 */
#define UCOM_DCXO_POLY_COEFF_NUM            ( 4 )

#define UCOM_NV_XO_TEMP_COMP_NUM            ( 32 )
#define UCOM_NV_BATT_TEMP_SENSOR_NUM        ( 8 )
/* 存放 144组温度-频率样本点:存数时按照 FIFO原则 */
#define UCOM_DCXO_TEMP_AFC_MAX_NUM          ( 144 )
#define UCOM_NV_PA_TEMP_SENSOR_NUM          ( 32 )

/* AFC CFIX折算曲线取点个数 */
#define UCOM_NV_CFIX_CONVERT_AFC_NUM        ( 5 )


#define UCOM_NV_GSM_LINEAR_PA_GAIN_NUM                ( 4 )                       /* GSM 线性PA增益个数 */

#define UCOM_NV_GSM_LINEAR_PA_SWITCH_THRESHOLD_NUM    ( UCOM_NV_GSM_LINEAR_PA_GAIN_NUM - 1 )  /* GSM 线性PA增益个数 */

#define UCOM_NV_DCXO_SEARCH_AFC_OFFSET_SAMPLE_NUM       ( 5 )                   /* 三次搜网AFC偏置计算曲线样本个数 */

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
 枚举名    :UCOM_NV_DC_SC_MODE_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  :载波模式
*****************************************************************************/
enum UCOM_NV_DC_SC_MODE_ENUM
{
    UCOM_NV_SC_MODE                     = 0,                                    /* 单载波模式 */
    UCOM_NV_DC_MODE                     = 1,                                    /* 双载波模式 */
    UCOM_NV_DC_SC_MODE_BUTT             = 2
};
typedef PHY_UINT16 UCOM_NV_DC_SC_MODE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : UCOM_NV_CHAN_CONNECT_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : A/B通道的标识
*****************************************************************************/
enum UCOM_NV_CHAN_CONNECT_ENUM
{
    UCOM_NV_CHAN_CONNECT_A                  = 0,                                /* A通道 */
    UCOM_NV_CHAN_CONNECT_B                  = 1,                                   /* B通道 */
    UCOM_NV_CHAN_CONNECT_BUTT               = 2
};
typedef PHY_UINT16 UCOM_NV_CHAN_CONNECT_ENUM_UINT16;


/*****************************************************************************
 枚举名    : UCOM_NV_WCDMA_SPUR_TYPE_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 谐波点类型
*****************************************************************************/
enum UCOM_NV_WCDMA_SPUR_TYPE_ENUM
{
    UCOM_NV_WCDMA_SPUR_1920                  = 0,                               /* 19.20M */
    UCOM_NV_WCDMA_SPUR_1536                  = 1,                                  /* 15.36M */
    UCOM_NV_WCDMA_SPUR_6144                  = 2,                                  /* 61.44M */
    UCOM_NV_WCDMA_SPUR_BUTT                  = 3
};
typedef PHY_UINT16 UCOM_NV_WCDMA_SPUR_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    :UCOM_NV_XO_TYPE_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  :晶体类型区分,0表示 DCXO,1表示 TCXO
*****************************************************************************/
enum UCOM_NV_XO_TYPE_ENUM
{
    UCOM_NV_XO_TYPE_DCXO                = 0,                                    /* 表示使用 DCXO */                                                           /* 上行仅存在DPCH */
    UCOM_NV_XO_TYPE_TCXO                = 1,                                    /* 表示使用 TCXO */                                                             /* 上行存在RACH */
    UCOM_NV_XO_TYPE_BUTT                = 2
};
typedef PHY_UINT16 UCOM_NV_XO_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    :UCOM_NV_DCXO_TEMP_CIRCUIT_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  :DCXO中有两个温度采集电路，一个用于采集常温和高温，另一个用于采集低温和部分常温
*****************************************************************************/
enum UCOM_NV_DCXO_TEMP_CIRCUIT_ENUM
{
    UCOM_NV_DCXO_TEMP_CIRCUIT_HIGH              = 0,                            /* 高温电路  */
    UCOM_NV_DCXO_TEMP_CIRCUIT_NORMAL            = 0,                            /* 常温和高温电路是一个 */                                                           /* 上行仅存在DPCH */
    UCOM_NV_DCXO_TEMP_CIRCUIT_LOW               = 1,                            /* 低温电路 */                                                             /* 上行存在RACH */
    UCOM_NV_DCXO_TEMP_CIRCUIT_BUTT              = 2
};
typedef PHY_UINT16 UCOM_NV_DCXO_TEMP_CIRCUIT_ENUM_UINT16;
/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
*                                                                            *
*                           参数设置消息结构                                 *
*                                                                            *
******************************************************************************/
/* xiongjiangjiang Begine */
/*****************************************************************************
 结构名    : UCOM_NV_NV_ITEM_XO_DEFINE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    UCOM_NV_XO_TYPE_ENUM_UINT16             enXoDefine;
}UCOM_NV_NV_ITEM_XO_DEFINE_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_RFIC_TEMP_COMP_WEIGH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : rf reg12 温度因子NV_RFIC_TEMP_COMP_WEIGTH
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwNvRficTempCompWeigh;
}UCOM_NV_RFIC_TEMP_COMP_WEIGH_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_XO_INIT_FREQUENCY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :2.2表示普通晶体的初始频偏和温度
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwInitFrequency;                       /* 普通晶体的初始频偏 */
    PHY_INT16                           shwInitTemperature;                     /* 普通晶体的初始温度 */
}UCOM_NV_XO_INIT_FREQUENCY_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DYNAMIC_FREQUENCY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :2.2表示平台上次AFC锁定值
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwDymamicFrequency;                    /* 平台上次AFC锁定值 */
    PHY_INT16                           shwDymamicTemperature;                  /* 平台上次AFC的温度 */
}UCOM_NV_DYNAMIC_FREQUENCY_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_TX_TEMP_DEFINE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.3 3G和2G上行发射功率相对温度变化范围
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTxTempDefine[UCOM_NV_W_TX_PA_TEMP_NUM];
}UCOM_NV_WG_TX_TEMP_DEFINE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_TEMP_SENCOR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :for UCOM_NV_PA_TEMP_SENSOR_STRU
                UCOM_NV_XO_TEMP_SENSOR_STRU
                UCOM_NV_USIM_BATT_TEMP_SENSOR_STRU
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwPaTempTemp;                          /* 温度:0.1度 */
    PHY_UINT16                          uhwPaTempVol;                           /* 热敏电阻检测电压 */
}UCOM_NV_TEMP_SENCOR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_PA_TEMP_SENSOR_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.1 PA的温度检测表
*****************************************************************************/
typedef struct
{
    UCOM_NV_TEMP_SENCOR_STRU            astPaTempSensor[UCOM_NV_PA_TEMP_SENSOR_NUM];
}UCOM_NV_PA_TEMP_SENSOR_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_XO_TEMP_SENSOR_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.1 普通晶体的常(高)温电压-温度表
*****************************************************************************/
typedef struct
{
    UCOM_NV_TEMP_SENCOR_STRU            astXoTempSensor[UCOM_NV_XO_TEMP_COMP_NUM];
}UCOM_NV_XO_TEMP_SENSOR_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_XO_LOW_TEMP_SENSOR_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.1 普通晶体的低温电压-温度表
*****************************************************************************/
typedef struct
{
    UCOM_NV_TEMP_SENCOR_STRU            astXoLowTempSensor[UCOM_NV_XO_TEMP_COMP_NUM];
}UCOM_NV_XO_LOW_TEMP_SENSOR_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_BATT_TEMP_SENSOR_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.1 电池的温度检测表
*****************************************************************************/
typedef struct
{
    UCOM_NV_TEMP_SENCOR_STRU            astBattTempSensor[UCOM_NV_BATT_TEMP_SENSOR_NUM];
}UCOM_NV_BATT_TEMP_SENSOR_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY
 协议表格  :
 ASN.1描述 :
 结构说明  : 产品类型NV。不同产品，Log保存路径不一样
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwProductType;                         /* 产品类型 */
    PHY_UINT16                          uhwReserve;                             /* 保留 */
}UCOM_NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY;

/*****************************************************************************
 结构名    : UCOM_NV_XO_LOW_TEMP_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : XO低温检测特性相关NV,包括特性开关与电路切换门限
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwLowTempSupport;                      /* 硬件是否支持低温检测，0-不支持，1-支持 */
    PHY_INT16                           shwSwitchDeltaTemp;                     /* 低温电路与原有电路的切换门限为(校准温度+该值)，
                                                                                   当低于切换门限温度时使用低温电路，默认为0，
                                                                                   即切换门限为校准温度(常温电路)，单位0.1度 */
    PHY_UINT16                          uhwTempDeltaThreshold;                  /* 两个电路校准值的偏差门限，默认为10，单位0.1度 */
    PHY_UINT16                          uhwReserve;                             /* 保留 */
}UCOM_NV_XO_LOW_TEMP_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_XO_INIT_LOW_TEMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 校准时，低温电路的检测温度，备份恢复项
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwInitLowTemp;                         /* 校准时，低温电路的检测温度，单位0.1度 */
    PHY_UINT16                          uhwReserve;                             /* 保留 */
}UCOM_NV_XO_INIT_LOW_TEMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_DCXO_AFC_CONVERT_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AFC折算上限/下限对应的Cfix-Afc折算表，Cfix-Afc折算关系与AFC,CFIX本身相关
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwAfcUpperThr;                                         /* AFC折算上限,当AFC大于此值时把超出部分折成cfix,默认值14384 */
    PHY_INT16                           shwAfcLowerThr;                                         /* AFC折算下限,当AFC小于此值时把小于部分折成cfix,默认值2000 */
    PHY_INT16                           ashwCfixTable[UCOM_NV_CFIX_CONVERT_AFC_NUM];           /* Cfix-Afc折算表对应的cfix表 */
    PHY_INT16                           ashwCfix2AfcUpperTable[UCOM_NV_CFIX_CONVERT_AFC_NUM];  /* AFC折算上限对应的Cfix-Afc折算表,实验室提取 */
    PHY_INT16                           ashwCfix2AfcLowerTable[UCOM_NV_CFIX_CONVERT_AFC_NUM];  /* AFC折算下限对应的Cfix-Afc折算表,实验室提取 */
}UCOM_NV_DCXO_AFC_CONVERT_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_IQ_REV_CONTROL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IQ接反配置
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRxAIRevSel;                          /* Rx A I路接反 */
    PHY_UINT16                          uhwRxAQRevSel;                          /* Rx A Q路接反 */
    PHY_UINT16                          uhwRxBIRevSel;                          /* Rx B I路接反 */
    PHY_UINT16                          uhwRxBQRevSel;                          /* Rx B Q路接反 */
    PHY_UINT16                          uhwTxIRevSel;                           /* Tx I路接反 */
    PHY_UINT16                          uhwTxQRevSel;                           /* Tx Q路接反 */
}UCOM_NV_WG_IQ_REV_CONTROL_STRU;


/*****************************************************************************
 结构名    : UCOM_DCXO_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存DCXO多项式的 4个系数和温度的最大最小值
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          auwMantissaDefault[UCOM_DCXO_POLY_COEFF_NUM];  /* 依次存放a0,a1,a2,a3的尾数 */
    PHY_UINT16                          auhwExponentDefault[UCOM_DCXO_POLY_COEFF_NUM]; /* 依次存放a0,a1,a2,a3的指数 */
    /* 特别说明:shwTmax/shwTmin实际上是和样本邦定的,和系数定义在一起不合理.因为不合理,所以DSP内部
       只使用pstAfcContext->stDcxoPara.shwSampleTempMax/shwSampleTempMin */
    PHY_INT16                           shwTmax;                                /* 样本温度最大值 */
    PHY_INT16                           shwTmin;                                /* 样本温度最小值 */
}UCOM_NV_DCXO_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_DCXO_AFC_VS_TEMP_VALUE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 存放一组DCXO的温度-频率(实际为AFC控制值)样本
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTemp;                                /* 温度 */
    PHY_UINT16                          uhwAfc;                                 /* 上面温度下对应的AFC控制值 */
}UCOM_NV_DCXO_AFC_VS_TEMP_VALUE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DCXO_TEMP_READ_PERIOD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DCXO读取温度的周期,区分IDLE态和DCH态
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwIdlePeriod;                          /* IDLE态下读取周期 */
    PHY_UINT16                          uhwDchPeriod;                           /* IDLE态下读取周期 */
}UCOM_NV_DCXO_TEMP_READ_PERIOD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DCXO_TEMP_COMP_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DCXO多项式系数更新门限
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          auwLowMantissa[UCOM_DCXO_POLY_COEFF_NUM];
    PHY_UINT16                          auhwLowExponent[UCOM_DCXO_POLY_COEFF_NUM];
    PHY_UINT32                          auwHighMantissa[UCOM_DCXO_POLY_COEFF_NUM];
    PHY_UINT16                          auhwHighExponent[UCOM_DCXO_POLY_COEFF_NUM];
}UCOM_NV_DCXO_TEMP_COMP_THRESHOLD_STRU;


/*****************************************************************************
 结构名    : UCOM_DCXO_FREQ_VS_TEMP_ARRAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 存放DCXO的温度-频率(实际为AFC控制值)样本集合
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidNum;                             /* 样本的有效个数 */
    PHY_INT16                           shwTmax;                                /* 样本温度最大值 */
    PHY_INT16                           shwTmin;                                /* 样本温度最小值 */
    UCOM_NV_DCXO_AFC_VS_TEMP_VALUE_STRU astAfcTempValue[UCOM_DCXO_TEMP_AFC_MAX_NUM]; /* 样本数据 */
}UCOM_NV_DCXO_FREQ_VS_TEMP_ARRAY_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DCXO_SAMPLE_UPDATE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DCXO样本更新参数定义
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwWcdmaPeriod;                          /* W DCXO样本写入NV周期，单位帧,默认30000 */
    PHY_UINT32                          uwGsmPeriod;                            /* G DCXO样本写入NV周期，单位帧,默认65000 */
    PHY_UINT16                          uhwUpdateLowThr;                        /* 样本更新下门限,默认234 AFC */
    PHY_UINT16                          uhwUpdateHighThr;                       /* 样本更新上门限,默认975 AFC */
}UCOM_NV_DCXO_SAMPLE_UPDATE_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_SAR_BACKOFF_TEST_MODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 降SAR测试模式
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwSarBackoffTestModeFlg;               /* 降SAR测试模式 */
}UCOM_NV_SAR_BACKOFF_TEST_MODE_STRU;


/*****************************************************************************
 枚举名    : UCOM_NV_WG_DCOFFSET_THRESHOLD_STRU
 协议表格  :
 枚举说明  : DC offset的直流校准门限
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwDciLimit;                            /* I路的直流门限,单位mV */
    PHY_INT16                           shwDcqLimit;                            /* Q路的直流门限,单位mV */
}UCOM_NV_WG_DCOFFSET_THRESHOLD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DCXO_CTRIM_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 降SAR测试模式
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwDcxoCTrim;                           /*Range:[-1,63]*/
}UCOM_NV_DCXO_CTRIM_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_DCXO_SEARCH_AFC_OFFSET_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : ashwSearchAfc2和ashwSearchAfcDelta2构成第二次搜网的校准曲线
             ashwSearchAfc3和ashwSearchAfcDelta3构成第三次搜网的校准曲线
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwSearchAfc2[UCOM_NV_DCXO_SEARCH_AFC_OFFSET_SAMPLE_NUM];      /* 第二次搜索AFC值 */
    PHY_INT16                           ashwSearchAfcDelta2[UCOM_NV_DCXO_SEARCH_AFC_OFFSET_SAMPLE_NUM]; /* 第二次搜索AFC偏置值 */
    PHY_INT16                           ashwSearchAfc3[UCOM_NV_DCXO_SEARCH_AFC_OFFSET_SAMPLE_NUM];      /* 第三次搜索AFC值 */
    PHY_INT16                           ashwSearchAfcDelta3[UCOM_NV_DCXO_SEARCH_AFC_OFFSET_SAMPLE_NUM]; /* 第三次搜索AFC偏置值 */
}UCOM_NV_DCXO_SEARCH_AFC_OFFSET_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_BUCK_VOLT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : BUCK电压值
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwBuck1Volt;                           /* buck1电压值,单位mV */
    PHY_UINT16                          uhwBuck2Volt;                           /* buck2电压值,单位mV */
}UCOM_NV_BUCK_VOLT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_RFIC_SSI_WAIT_TIME_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : BUCK电压值
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRficSsiWaitTime;
}UCOM_NV_RFIC_SSI_WAIT_TIME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwNumber;
    PHY_INT16                           ashwGain[UCOM_NV_GSM_LINEAR_PA_GAIN_NUM];
}UCOM_NV_GSM_LINEAR_PA_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_EDGE_LINEAR_PA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益
*****************************************************************************/
/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwNumber;
    PHY_INT16                           ashwGain[UCOM_NV_GSM_LINEAR_PA_GAIN_NUM];
}UCOM_NV_EDGE_LINEAR_PA_GAIN_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_PCL_PA_GAIN_INDEX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益索引
*****************************************************************************/
typedef struct
{
    PHY_UINT16                           auhwPaGainIndex[UCOM_NV_G_PCL_NUM];     /* 32个功率等级对应的PA增益索引 */
}UCOM_NV_GSM_TX_PCL_PA_GAIN_INDEX_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_PCL_PA_GAIN_INDEX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益索引
*****************************************************************************/
typedef struct
{
    PHY_UINT16                           auhwPaGainIndex[UCOM_NV_G_PCL_NUM];     /* 32个功率等级对应的PA增益索引 */
}UCOM_NV_EDGE_TX_PCL_PA_GAIN_INDEX_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_TX_REF_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidChanNum;                                /* 有效频点个数 */
    PHY_UINT16                          auhwTxChan[UCOM_NV_G_TX_REF_CHAN_NUM_MAX];      /* 参考频点列表 */
}UCOM_NV_TX_CAL_CHAN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RAMP_INDEX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwRampIndex[UCOM_NV_G_PCL_NUM];
}UCOM_NV_GSM_RAMP_INDEX_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_RAMP_INDEX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwRampIndex[UCOM_NV_G_PCL_NUM];
}UCOM_NV_EDGE_RAMP_INDEX_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RAMP_COEFF_STRU
 协议表格  :
 结构说明  : GSM/EDGE上下行爬坡表NV项参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwRampUp[UCOM_NV_G_RAMP_COEF_NUM];    /* 上坡表 */
    PHY_UINT16                          auhwRampDown[UCOM_NV_G_RAMP_COEF_NUM];  /* 下坡表 */
}UCOM_NV_GSM_RAMP_COEFF_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RAMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM爬坡系数结构
*****************************************************************************/
typedef struct
{
    UCOM_NV_GSM_RAMP_COEFF_STRU         astRampCoeff[UCOM_NV_G_RAMP_PWR_LEVEL_NUM];     /* [依次以10dBm,20dBm划分为3个区间] */
}UCOM_NV_GSM_RAMP_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_EDGE_RAMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : EDGE爬坡系数结构
*****************************************************************************/
typedef struct
{
    UCOM_NV_GSM_RAMP_COEFF_STRU         astRampCoeff[UCOM_NV_G_RAMP_PWR_LEVEL_NUM];     /* [依次以10dBm,20dBm划分为3个区间] */
}UCOM_NV_EDGE_RAMP_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    /* 注意电压和发射功率的对应关系 */
    PHY_UINT16                          auhwTxPower[UCOM_NV_G_TX_PWR_CONT_NUM];         /* 需要校正的发射功率 */
    PHY_UINT16                          auhwContVolt[UCOM_NV_G_TX_PWR_CONT_NUM];        /* 需要校正的发射功率对应的控制电压 */
}UCOM_NV_GSM_PA_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

/* 自研RF，8PSK同样使用电压字控制 */
typedef struct
{
    PHY_UINT16                          auhwContVolt[UCOM_NV_G_TX_8PSK_PWR_CONT_NUM];   /* 需要校正的发射功率对应的控制电压 */
    PHY_UINT16                          auhwTxPower[UCOM_NV_G_TX_8PSK_PWR_CONT_NUM];    /* 需要校正的发射功率 */
}UCOM_NV_EDGE_PA_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwGsmMaxTxVolt;
    PHY_UINT16                          uhwGsmMaxTxPower;
}UCOM_NV_GSM_MAX_TX_VOLT_POWER;


/*****************************************************************************
 结构名    : UCOM_NV_G_TIMING_CTRL_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTimingCtrl;
}UCOM_NV_G_TIMING_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_RAMP_POINTS_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 爬坡点数结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRampPoints;                          /* 爬坡点数 */
}UCOM_NV_G_RAMP_POINTS_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_PREMOD_SYMBOL_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 预调制符号结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwPreModSymbol;                        /* 预调制符号 */
}UCOM_NV_G_PREMOD_SYMBOL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_TX_BURST_OFFSET_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  : burset offset结构
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTxBurstOffset;                       /* GSM burst offset */
}UCOM_NV_G_TX_BURST_OFFSET_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_TX_PA_LENGTH_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PA保持最大电压的长度结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTxPaLength;                          /* PA保持最大电压的长度，单位为Qb */
}UCOM_NV_G_TX_PA_LENGTH_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_PA_PRECHG_VOLT_CTRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PA保持最大电压的长度结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwPaPrechgVolt;                        /*Range:[0,1023]*//* 校准后的预充电压 */
}UCOM_NV_G_PA_PRECHG_VOLT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_G_PA_FACTORY_PRECHARGE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwInit;
    PHY_INT16                           shwDelta;
}UCOM_NV_G_PA_FACTORY_PRECHARGE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_RX_IP2_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :I和Q通道直流，为NV项中通用的I和Q通道偏置
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwIReg;                                /* I路寄存器取值，拼接方式 = SN[0]+DATA[6:0] */
    PHY_INT16                           shwQReg;                                /* Q路寄存器取值，拼接方式 = SN[0]+DATA[6:0] */
}UCOM_NV_RX_IP2_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_IP2_CAL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :5.2 每个BAND在高中低频点下，对I和Q路的电路参数补偿
*****************************************************************************/
typedef struct
{
    UCOM_NV_RX_IP2_STRU                 astRxIp2Cal[3];
}UCOM_NV_GSM_RX_IP2_CAL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_AGC_FREQ_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :5.4 在AGC增益在统一个频段内因频点不同而引起的增益变化
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxAgcFreqComp[UCOM_NV_G_RX_CHAN_COMP_NUM];  /*Rang:[-64,64]*//* 一个频段内16个频点 */
}UCOM_NV_GSM_RX_AGC_FREQ_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_APC_RF_ATTEN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.1 RF芯片对基带信号的衰减，用于配置RF芯片寄存器
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwEdgeTxApcRfAtten[UCOM_NV_G_TX_ATTEN_NUM];
}UCOM_NV_EDGE_TX_APC_RF_ATTEN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_APC_DBB_ATTEN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.1 DBB芯片对基带信号的补偿，
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwEdgeTxApcDbbAtten[UCOM_NV_G_TX_ATTEN_NUM];
}UCOM_NV_EDGE_TX_APC_DBB_ATTEN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_VBIAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.3 在不同功率等级下，因为PA供电电压(或者偏置电压)可以下降到的最低电压
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwPaVbias[UCOM_NV_G_PCL_NUM];     /* 32个DA变换前的值 */
}UCOM_NV_GSM_PA_VBIAS_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_PA_VBIAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.3 在不同功率等级下，因为PA供电电压(或者偏置电压)可以下降到的最低电压
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwPaVbias[UCOM_NV_G_PCL_NUM];     /* 32个DA变换前的值 */
}UCOM_NV_EDGE_PA_VBIAS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_VBIAS_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.3 在不同功率等级下，因为PA供电电压(或者偏置电压)下降，而引起PA增益变化的大小。
*****************************************************************************/
typedef struct
{
    PHY_INT16                           auhwPaVccVbiasComp[UCOM_NV_G_PCL_NUM];  /* 32个功率补偿值 */
}UCOM_NV_GSM_PA_VCC_VBIAS_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_PA_VBIAS_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :6.3 在不同功率等级下，因为PA供电电压(或者偏置电压)下降，而引起PA增益变化的大小。
*****************************************************************************/
typedef struct
{
    PHY_INT16                           auhwPaVccVbiasComp[UCOM_NV_G_PCL_NUM];  /* 32个功率补偿值 */
}UCOM_NV_EDGE_PA_VCC_VBIAS_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GMSK(线性PA) TX相对于温度变化的补偿
*****************************************************************************/
typedef struct
{
    /* 温度范围的取值取决于UCOM_NV_WG_TX_TEMP_DEFINE_STRU */
    PHY_INT16                           ashwGsmTxTempComp[UCOM_NV_G_TEMP_LEVEL_NUM];   /*Range:[-100,100]*/
}UCOM_NV_GSM_TX_TEMP_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.7 8PSK TX相对于温度变化的补偿
*****************************************************************************/
typedef struct
{
    /* 温度范围的取值取决于UCOM_NV_WG_TX_TEMP_DEFINE_STRU */
    PHY_INT16                           ashwEdgeTxTempComp[UCOM_NV_G_TEMP_LEVEL_NUM];   /*Range:[-100,100]*/
}UCOM_NV_EDGE_TX_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_CAL_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :需要校准的GSM的频点个数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidCalChanNum;                     /*Range:[0,16]*/
    PHY_UINT16                          auhwGsmRxCalChan[UCOM_NV_G_RX_CHAN_COMP_NUM];
}UCOM_NV_GSM_RX_CAL_CHAN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_AGC_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :5.3 AGC每个状态的增益值，总共分成8档
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxAgcGain[UCOM_NV_G_CHAN_GAIN_MAX_NUM];
}UCOM_NV_GSM_RX_AGC_GAIN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_ EDGE_TX_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示每档档RF Gain对应的温度补偿值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTempComp10th[UCOM_NV_EDGE_TX_TEMP_COMP_NUM];      /* 表示某一档RF Gain在16个采样温度下的温度补偿值，单位为0.1dB */
}UCOM_NV_EDGE_TEMP_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_RF_GAIN_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示15档RF Gain对应的温度补偿值
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCount;
    UCOM_NV_EDGE_TEMP_COMP_STRU         astTempComp[UCOM_NV_EDGE_TX_RF_GAIN_CTRL_NUM];      /* 表示15档RF Gain在16个采样温度下的温度补偿值 */
}UCOM_NV_EDGE_RF_GAIN_TEMP_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_PA_VCC_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 在不同功率等级下，GSM对应的VCC电压控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwGsmPaVcc[UCOM_NV_G_PCL_NUM];        /* 32个DA变换前的值 */
}UCOM_NV_GSM_PA_VCC_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_PA_VCC_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 在不同功率等级下，EDGE对应的VCC电压控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwEdgePaVcc[UCOM_NV_G_PCL_NUM];       /* 32个DA变换前的值 */
}UCOM_NV_EDGE_PA_VCC_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_CLOSE_PA_VCC_CONTROL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : close vcc control
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwClosePaVccMipiCmdFrame;              /* 关闭PaVcc的mipi指令帧 */
    PHY_UINT16                          uhwClosePaVccMipiDataFrame;             /* 关闭PaVcc的Mipi数据帧 */
}UCOM_NV_GSM_CLOSE_PA_VCC_CONTROL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_DEV_AVOID_GSM_SLOT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GPS避让GSM时隙
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwPowerThreshold;                      /* 功率门限，大于该门限时启动避让模式，单位为0.1dbm */
    PHY_UINT16                          uhwBandMask;                            /* 用于避让的频段掩码，BIT0/1/2/3对应850/900/1800/1900M */
    PHY_UINT32                          uwIoMask;                               /* 用于避让的管脚 */
}UCOM_NV_DEV_AVOID_GSM_SLOT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_REDUCE_CURRENT_MIPI_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 二级限流(MIPI)
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwReduceCurrentEn;                     /* 二级限流使能 */
    PHY_UINT16                          uhwCurrentLimitAddr;                    /* 二级限流对应的PMU地址 */
    PHY_UINT16                          uhwDefaultCurrentValue;                 /* 缺省时的电流值(第一个时隙) */
    PHY_UINT16                          uhwLimitCurrentValue;                   /* 限流时的配置值(非第一个时隙) */
}UCOM_NV_GSM_REDUCE_CURRENT_MIPI_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_REDUCE_CURRENT_GPIO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 二级限流(GPIO)
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwReduceCurrentEn;                     /* 二级限流使能 */
    PHY_UINT32                          uwIoMask;                               /* 二级限流使用的管脚 */
}UCOM_NV_GSM_REDUCE_CURRENT_GPIO_STRU;



/*****************************************************************************
 结构名    : UCOM_GSM_TX_APC_CAL_TEMP
 协议表格  :
 ASN.1描述 :
 结构说明  : Gsm下APC校准温度
*****************************************************************************/
typedef struct
{
    PHY_INT16                               shwGsmTxApcCalTemp;                 /*Range:[-300,1350]*/
}UCOM_GSM_TX_APC_CAL_TEMP_STRU;


/*****************************************************************************
 结构名    : UCOM_EDGE_TX_APC_CAL_TEMP
 协议表格  :
 ASN.1描述 :
 结构说明  : Edge下APC校准温度
*****************************************************************************/
typedef struct
{
    PHY_INT16                               shwEdgeTxApcCalTemp;                /*Range:[-300,1350]*/
}UCOM_EDGE_TX_APC_CAL_TEMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_START_BYPASS_THRESHOLD
 协议表格  :
 ASN.1描述 :
 结构说明  : G模的Bypass门限
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwGsmStartBypassThreshold;
}UCOM_NV_GSM_START_BYPASS_THRESHOLD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_POWER
 协议表格  :
 ASN.1描述 :
 结构说明  : 需要校正的发射功率
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmTxPower[UCOM_NV_G_TX_PWR_CONT_NUM];  /*Range:[-400,400]*//* 需要校正的发射功率 */
}UCOM_NV_GSM_TX_POWER_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_POWER
 协议表格  :
 ASN.1描述 :
 结构说明  : 需要校正的发射功率对应的控制电压
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwGsmContVolt[UCOM_NV_G_TX_PWR_CONT_NUM]; /*Range:[0,1023]*//* 需要校正的发射功率对应的控制电压 */
}UCOM_NV_GSM_CONT_VOLT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_POWER
 协议表格  :
 ASN.1描述 :
 结构说明  : 不同频点的发射功率的补偿值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmTxFreqComp[UCOM_NV_G_TX_CHAN_COMP_NUM];  /*Range:[-100,100]*//* [频段][不同频点的发射功率的补偿值] ，单位0.1dBm*/
}UCOM_NV_GSM_TX_FREQ_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_FREQ_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : Edge校准发射功率的补偿值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmTxFreqComp[UCOM_NV_G_TX_CHAN_COMP_NUM];  /*Range:[-100,100]*//* [频段][不同频点的发射功率的补偿值] ，单位0.1dBm*/
}UCOM_NV_EDGE_TX_FREQ_COMP_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_TX_APC_DBB_ATTEN
 协议表格  :
 ASN.1描述 :
 结构说明  : gsm linear pa apc dbb atten
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmLinearPaTxApcDbbAtten[UCOM_NV_G_TX_ATTEN_NUM];   /*Range:[-40,10]*//* gsm linear pa apc dbb atten */
}UCOM_NV_GSM_LINEAR_PA_TX_APC_DBB_ATTEN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_RF_ATTEN
 协议表格  :
 ASN.1描述 :
 结构说明  : RF衰减
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwGsmTxRfAtten;   /* RF衰减 */
}UCOM_NV_GSM_TX_RF_ATTEN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_POWER_LEVEL
 协议表格  :
 ASN.1描述 :
 结构说明  : 4频段功率等级到dBm的映射表
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmTxPowerLevel[UCOM_NV_G_PCL_NUM]; /* 4频段功率等级到dBm的映射表 */
}UCOM_NV_GSM_TX_POWER_LEVEL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_MAX_POWER_REDUCTION
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM最大发射功率回退
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwGsmMaxPowerReduction[UCOM_NV_G_TX_SLOT_MAX_NUM];
}UCOM_NV_GSM_MAX_POWER_REDUCTION_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_WIRELESS_MAX_POWER
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM无线最大发射功率
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmWirelessMaxPower[UCOM_NV_G_TX_SLOT_MAX_NUM]; /* 无线 *//* 4频段GMSK空口发射时的功率衰减 */
}UCOM_NV_GSM_WIRELESS_MAX_POWER_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_SAR_BACK_OFF_POWER
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM下各时隙SAR回退值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwGsmSarBackoffPower[UCOM_NV_SAR_BACKOF_NUM];
}UCOM_NV_GSM_SAR_BACK_OFF_POWER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_PA_GAIN
 协议表格  :
 ASN.1描述 :
 结构说明  : 传统PA的增益
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwEdgeTxPaGain;                        /* 传统PA的增益 */
}UCOM_NV_EDGE_TX_PA_GAIN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_APC_RF_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GMSK(线性PA)下APC RF控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwGsmLinearPaApcRfCtrl[UCOM_NV_G_TX_ATTEN_NUM];    /* apc rf ctrl */
}UCOM_NV_GSM_LINEAR_PA_APC_RF_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_APC_RF_CTRL
 协议表格  :
 ASN.1描述 :
 结构说明  : EDGE下APC RF控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwEdgeTxApcRfCtrl[UCOM_NV_G_TX_ATTEN_NUM];    /* apc rf ctrl */
}UCOM_NV_EDGE_TX_APC_RF_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_RF_GAIN_STATE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GMSK(线性PA)下APC DBB衰减
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwGsmLinearPaRfGainStateIndex[UCOM_NV_G_TX_ATTEN_NUM]; /* rf gain 索引 */
}UCOM_NV_GSM_LINEAR_PA_RF_GAIN_STATE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_NONLINEAR_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GMSK(线性PA)下APC DBB衰减
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwComp[UCOM_NV_G_PCL_NUM];
}UCOM_NV_GSM_LINEAR_PA_NONLINEAR_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_EDGE_LINEAR_PA_NONLINEAR_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : EDGE(线性PA)下APC DBB衰减
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwComp[UCOM_NV_G_PCL_NUM];
}UCOM_NV_EDGE_LINEAR_PA_NONLINEAR_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_EDGE_TX_RF_GAIN_STATE_INDEX
 协议表格  :
 ASN.1描述 :
 结构说明  : EDGE下APC DBB衰减
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwEdgeTxRfGainStateIndex[UCOM_NV_G_TX_ATTEN_NUM]; /* rf gain 索引 */
}UCOM_NV_EDGE_TX_RF_GAIN_STATE_INDEX_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_RSSI_ARITHMETIC_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : G模通道固定增益
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRssiArithmeticPara;
}UCOM_NV_G_RSSI_ARITHMETIC_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_RX_PLL_ADVANCE_TIMING_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : rx pll提前打开时间
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRxPllAdvanceTiming;
}UCOM_NV_G_RX_PLL_ADVANCE_TIMING_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_AFC_FE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwAfcFeDelayFn;                        /* FE延迟帧数 */
    PHY_UINT16                          uhwAfcFePowerDis;                       /*  */
    PHY_UINT16                          uhwAfcFeDchFcut0;                       /*  */
    PHY_UINT16                          uhwAfcFeDchFcut1;                       /*  */
}UCOM_NV_GSM_AFC_FE_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_G_ADJ_CHAN_INTERFERE_EN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 邻频抑制使能标志
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwAdjChanInterfereEn;                  /* 邻频抑制使能 */
}UCOM_NV_G_ADJ_CHAN_INTERFERE_EN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_LS_DCR_EN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : ls dcr使能标志
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwLsDcrEn;                             /* ls dcr使能 */
}UCOM_NV_G_LS_DCR_EN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_SRB_STUB_FOR_GCF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 由于射频存在问题，GCF测试时，必须对SRB环回进行打桩
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwGsmSrbStubForGcfEn;                   /* 由于射频存在问题，GCF测试时，必须对SRB环回进行打桩 */
}UCOM_NV_GSM_SRB_STUB_FOR_GCF_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_LINEAR_PA_ENABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LinearPA使能标志
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwLinearPaEnable;                      /* linear Pa使能 */
}UCOM_NV_G_LINEAR_PA_ENABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_LINEAR_PA_GAIN_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 线性PA的增益控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          auwWord[UCOM_NV_GSM_LINEAR_PA_GAIN_NUM];
}UCOM_NV_GSM_LINEAR_PA_GAIN_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_G_APT_ENABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LinearPA使能标志
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwGsmAptEnable;                        /* 2G APT使能标志 */
}UCOM_NV_G_APT_ENABLE_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_GSM_SPUR_ACPR_OPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM SPUR ACPR优化
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwGsmSpurAcprOpt;                      /* 2G GSM SPUR 优化 */
}UCOM_NV_GSM_SPUR_ACPR_OPT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_BY_PASS_GPIO_MASK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : Bypass导通时配置GPIO口的bit位
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwBypassGpioMask;                       /* Bypass导通时配置GPIO口的bit位 */
}UCOM_NV_G_BY_PASS_GPIO_MASK_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_G_PA_VCC_MIPI_CMD_FRAME_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 打开PaVcc的mipi指令帧
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwOpenPaVccMipiCmdFrame;               /* 打开PaVcc的mipi指令帧 */
}UCOM_NV_G_PA_VCC_MIPI_CMD_FRAME_STRU;



/*****************************************************************************
 枚举名    : UCOM_NV_GSM_RX_AGC_CTRL_STRU
 协议表格  :
 枚举说明  : 5.3G下行AGC增益控制字
*****************************************************************************/
typedef struct
{
    UCOM_NV_RX_AGC_CTRL_UNION           aunRxAgcCtrl[UCOM_NV_G_AGC_LEVEL_MAX_NUM];
}UCOM_NV_GSM_RX_AGC_CTRL_STRU;


/*****************************************************************************
 枚举名    : UCOM_NV_GSM_AGC_SWITCH_THRESHOLD_STRU
 协议表格  :
 枚举说明  : AGC增益切换门限
*****************************************************************************/

typedef struct
{
    PHY_INT16                           shwIncDelay;
    PHY_INT16                           ashwDec[UCOM_NV_G_GAIN_THRESHOLD_MAX_NUM];
}UCOM_NV_GSM_AGC_SWITCH_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_ACPR_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTempThreshold;                       /* 温度门限，单位为度 */
    PHY_INT16                           shwPowerComp;                           /* 功率补偿，单位为0.1dbm */
}UCOM_NV_GSM_ACPR_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_EDGE_RF_TX_BIAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwBiasHighTemp;                       /* 温度门限，单位为度 */
    PHY_INT16                           shwBiasRoomTemp;                           /* 功率补偿，单位为0.1dbm */
}UCOM_NV_EDGE_RF_TX_BIAS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_DCR_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :DCR 需要校准的GSM的频点个数
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uhwValidDcrChanNum;
    PHY_UINT16                          auhwGsmRxCalChan[ UCOM_G_RX_GAIN0_DCR_CHAN_NUM ];
}UCOM_NV_GSM_RX_DCR_CHAN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_DC_OFFSET_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM DC Offset值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwInitDcoci;                           /* 初始值,RFIC控制值为0x80对应的I路DC值 */
    PHY_INT16                           shwInitDcocq;                           /* 初始值,RFIC控制值为0x80对应的Q路DC值 */
    PHY_INT16                           shwRfDci;                               /* 校准后,RFIC I路DC控制值 */
    PHY_INT16                           shwRfDcq;                               /* 校准后,RFIC Q路DC控制值 */
    PHY_INT16                           shwRemainDcoci;                         /* 校准后,I路残余DC值 */
    PHY_INT16                           shwRemainDcocq;                         /* 校准后,Q路残余DC值 */
}UCOM_NV_GSM_DC_OFFSET_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_DC_OFFSET_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM DC Offset值
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCount;
    UCOM_NV_GSM_DC_OFFSET_STRU          astGain0DcOffset[ UCOM_G_RX_GAIN0_DCR_CHAN_NUM ];
}UCOM_NV_GSM_RX_DC_OFFSET_GAIN0_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RX_DC_OFFSET_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM DC Offset值
*****************************************************************************/
typedef struct
{
    UCOM_NV_GSM_DC_OFFSET_STRU           astGain0[UCOM_G_RX_GAIN0_DCR_CHAN_NUM ];             /* 本频Gain0档位，DCR校准结果 */
    UCOM_NV_GSM_DC_OFFSET_STRU           astOtherGain[UCOM_G_RX_OTHER_GAIN_DCR_CHAN_NUM];  /*本频其它档位DCR校准结果:UCOM_NV_G_AGC_LEVEL_MAX_NUM -1 */
    UCOM_NV_GSM_DC_OFFSET_STRU           astCrossBandGain0[UCOM_G_RX_GAIN0_DCR_CHAN_NUM];     /* 异频Gain0档位，DCR校准结果 */
    UCOM_NV_GSM_DC_OFFSET_STRU           astCrossBandOtherGain[UCOM_G_RX_OTHER_GAIN_DCR_CHAN_NUM]; /*异频其它档位DCR校准结果:UCOM_NV_G_AGC_LEVEL_MAX_NUM -1 */
}UCOM_NV_GSM_RX_DC_OFFSET_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RF_GPIO_MASK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF 控制结构
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwGsmAntMask;                           /* 该值表示W的BAND或者G模的BAND标识 */
    PHY_UINT32                          uwPaEnMask;                             /* RF所在的通道 */
    PHY_UINT32                          uwPaBandMask;                           /* 天线1的开关值 */
    PHY_UINT32                          uwPaModeMask;                           /* 天线2的开关值 */
}UCOM_NV_GSM_RF_GPIO_MASK_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_DSDA_POWER_LIMIT_STRU
 结构说明  : 双卡发射功率限制NV结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                      uhwPowerLmtEnable;                          /* 限制功率开关 */
    PHY_UINT16                      auhwPowerLmtLow[UCOM_NV_G_HIGH_LOW_BUTT];   /* 限制功率最小值，单位0.1dBm */
    PHY_UINT16                      auhwPowerLmtHigh[UCOM_NV_G_HIGH_LOW_BUTT];  /* 限制功率稳态值，单位0.1dBm */
    PHY_UINT16                      auhwPowerLmtMax[UCOM_NV_G_HIGH_LOW_BUTT];   /* 限制功率最大值，单位0.1dBm */
}UCOM_NV_GSM_DSDA_POWER_LIMIT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RAMP_DAC_REV_STRU
 结构说明  : 双卡发射功率限制NV结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                      uhwGsmRampDacRev;                           /* gsm爬坡dac是否交叉,0:不交叉,1:交叉 */
}UCOM_NV_GSM_RAMP_DAC_REV_STRU;


/*****************************************************************************
 枚举名    : UCOM_NV_G_ANT_TUNER_CODE_STRU
 协议表格  :
 枚举说明  : 天线调谐G模默认值
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxCode;                          /* 天线调谐Tx默认值 */
    PHY_UINT16                              uhwReserve1;
    PHY_UINT16                              uhwRxCode;                          /* 天线调谐Rx默认值 */
    PHY_UINT16                              uhwReserve2;
}UCOM_NV_G_ANT_TUNER_DEFULT_CODE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_ANT_SWITCH_MIPI_USID_CHANGE_STRU
 结构说明  : 用于当主分集天线开关USID相同时，修改天线开关USID的MIPI指令
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwValidNum;					    /* 指令有效个数 */
    PHY_UINT16                              uhwReserve;
    PHY_UINT16                              uhwCmdFrame[9];                     /* mipi配置命令 */
    PHY_UINT16                              uhwDataFrame[9];                    /* mipi配置数据 */
} UCOM_NV_WG_ANT_SWITCH_MIPI_USID_CHANGE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_MIPI_WR_FORMAT_STRU
 结构说明  : MIPI的控制结构体
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwDataFrame;                       /* mipi配置值 */
    PHY_UINT16                              uhwCmdFrame;                        /* mipi配置命令 */
} UCOM_NV_MIPI_WR_FORMAT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_TUNER_MIPI_IDLE_REG_STRU
 协议表格  :
 结构说明  : WG共用,配置tuner为IDLE态或低功耗态的控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwValidNum;
    PHY_UINT16                              auhwCmdFrame[UCOM_NV_TUNER_MIPI_IDLE_CMD_MAX_NUM];
    PHY_UINT16                              auhwDataFrame[UCOM_NV_TUNER_MIPI_IDLE_CMD_MAX_NUM];
}UCOM_NV_TUNER_MIPI_IDLE_REG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_TUNER_MIPI_COMM_INIT_REG_STRU
 协议表格  :
 结构说明  : WG共用,tuner的公共初始化控制字，用于一些寄存器只需要模式加载配置一次
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwValidNum;
    PHY_UINT16                              auhwCmdFrame[UCOM_NV_TUNER_MIPI_COMM_INIT_REG_NUM];
    PHY_UINT16                              auhwDataFrame[UCOM_NV_TUNER_MIPI_COMM_INIT_REG_NUM];
}UCOM_NV_TUNER_MIPI_COMM_INIT_REG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_ANT_TUNER_CTRL_INFO_STRU
 结构说明  : 天线调谐的控制结构体,MIPI型tuner,考虑兼容多种型号，提供最多6个控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwAntTunerCtrl;                    /* 硬件是否支持天线调谐，0不支持，1支持 */
    PHY_UINT16                              uhwAntTunerCtrlMode;                /* 天线调谐是通过MIPI控制还是GPIO,0表示GPIO,1表示MIPI */
    UCOM_NV_TUNER_MIPI_COMM_INIT_REG_STRU   stTunerCommReg;                     /* tuner的公共初始化控制字，用于一些寄存器只需要模式加载配置一次 */
    UCOM_NV_TUNER_MIPI_IDLE_REG_STRU        stTunerIdleReg;                     /* 配置tuner为IDLE态或低功耗态的控制字 */
} UCOM_NV_WG_ANT_TUNER_CTRL_INFO_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RF_DIV_BAND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 是否支持分集
*****************************************************************************/
typedef struct
{
    PHY_UINT32          uwWBand;
    PHY_UINT32          uwWbandExtRsv;
}UCOM_NV_W_RF_DIV_BAND_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_RF_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF 控制结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwBand;                                /* 该值表示W的BAND或者G模的BAND标识 */
    PHY_UINT16                          uhwRfChannel;                           /* RF所在的通道 */
    PHY_UINT32                          uwAntSwitch;                            /* 天线1的开关值 */
    PHY_UINT32                          uwOtherSwitch;                          /* 其它天线开关值 */
    PHY_UINT32                          uwPaControlWord;                        /* W模PA使能 */
}UCOM_NV_RF_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_BAND_RF_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :2.1表示支持的3G和2G频段、3G分集支持情况、每个频段与RF
            通道对应关系天线开关切换选择、DBB对3GPA的控制
*****************************************************************************/
typedef struct
{
    UCOM_NV_RF_CONFIG_STRU              stRxMainRfBandCofig;                    /* 包括了2G和3G的所有频道 */

    UCOM_NV_RF_CONFIG_STRU              stRxDvtyRfBandCofig;                    /* 包括了2G和3G的所有频道 */

    UCOM_NV_RF_CONFIG_STRU              stTxRfBandCofig;                        /* 包括了2G和3G的所有频道 */
}UCOM_NV_W_BAND_RF_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_RF_BAND_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :2.1表示支持的3G和2G频段、3G分集支持情况、每个频段与RF
            通道对应关系天线开关切换选择、DBB对3GPA的控制
*****************************************************************************/
typedef struct
{
    UCOM_NV_RF_CONFIG_STRU              astGsmRxRfBandConfig[UCOM_NV_G_BAND_NUMBER];        /* 包括了2G RX 所有频段 */

    UCOM_NV_RF_CONFIG_STRU              astGsmTxRfBandConfig[UCOM_NV_G_BAND_NUMBER];        /* 包括了2G TX 频段 */

    UCOM_NV_W_BAND_RF_CONFIG_STRU       astWcdmaRfBandConfig[UCOM_NV_W_TOTAL_BAND_NUM];     /* 包括了3G 频段 */
}UCOM_NV_WG_RF_BAND_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_RF_INIT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF初始化结果
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwRfAddr;
    PHY_UINT16          uhwRfAddrValue;
}UCOM_NV_RF_INIT_INFO_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_RF_INIT_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF初始化结果
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwRfCount;
    UCOM_NV_RF_INIT_INFO_STRU               stConfigInfo[UCOM_NV_RFABB_INIT_REG_NUM];
}UCOM_NV_RF_INIT_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_ABB_INIT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF初始化结果
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwAbbAddr;
    PHY_UINT16          uhwAbbValue;
}UCOM_NV_ABB_INIT_INFO_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_ABB_INIT_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : ABB初始化结果
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwABBCount;
    UCOM_NV_ABB_INIT_INFO_STRU              stABBConfigInfo[UCOM_NV_RFABB_INIT_REG_NUM];
}UCOM_NV_ABB_INIT_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_PHY_SW_CFG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 解决前端差异性的NV项。这里是时钟类的属性
*****************************************************************************/
typedef struct
{
    UCOM_NV_PHY_SW_CFG_DRX_SYSCLK_STRU  stDrxTSysclk;
    PHY_UINT32                          uwReservedWords[7];         /* 保留用于以后自行扩展 */
} UCOM_NV_PHY_SW_CFG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_MMMB_PA_SEL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MIPI新增NV项,PA控制模式
*****************************************************************************/
typedef struct
{
    PHY_UINT32          uwWBand;
    PHY_UINT32          uwWbandExtRsv;
    PHY_UINT32          uwGeBand;
}UCOM_NV_WG_MMMB_PA_SEL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_ANT_SWITCH_CONTROL_MODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : MIPI新增NV项,天线控制模式
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwWcdmaBandMainAnt;                    /* [0]-BAND1;[1]-BAND2;以此类推 */
    PHY_UINT32                          uwWcdmaBandDvtyAnt;                    /* [0]-BAND1;[1]-BAND2;以此类推 */
    PHY_UINT32                          uwWbandExtRsv;                        /* [0]-GSM450;[1]-GSM480;... */
    PHY_UINT32                          uwWbandExtRsv2;                        /* [0]-GSM450;[1]-GSM480;... */
    PHY_UINT32                          uwGsmBandRxAnt;                        /* [0]-GSM450;[1]-GSM480;... */
    PHY_UINT32                          uwGsmBandTxAnt;                        /* [0]-GSM450;[1]-GSM480;... */
}UCOM_NV_ANT_SWITCH_CONTROL_MODE_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_DRX_RESUME_TIME_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : WG DRX 睡眠唤醒ABB、RF等外设稳定和软件恢复时间
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwAbbPwrRsmTime;                        /* ABB AD/DA数字部分电源稳定时间,单位us */
    PHY_UINT32                          uwRfPwrRsmTime;                         /* RF 模拟域电源稳定时间,单位us */
    PHY_UINT32                          uwAntSwitchPwrRsmTime;                  /* 天线开关电源稳定时间,单位us */
    PHY_UINT32                          uwAbbWpllRsmTime;                       /* ABB WPLL稳定时间,单位us */
    PHY_UINT32                          uwAbbGpllRsmTime;                       /* ABB GPLL稳定时间 ,单位us*/
    PHY_UINT32                          uwBbpPllRsmTime;                        /* SOC BBP PLL稳定时间,单位us */
    PHY_UINT32                          uwPaStarRsmTime;                            /* 保留字段 */
    PHY_UINT32                          uwSysProtectTime;                       /* 系统恢复保护时间 */

    PHY_UINT32                          uwTcxoRsmTime;                          /* TCXO稳定时间,单位us */
    PHY_UINT32                          uwDcxoRsmTime;                          /* DCXO稳定时间,单位us */
    PHY_UINT32                          uwSlowToMcpuRsmTime;                    /* 从SLOW到MCPU上电恢复时间,单位us */
    PHY_UINT32                          uwWphyRsmTime;                          /* W物理层唤醒恢复时间,单位us */
    PHY_UINT32                          uwGphyRsmTime;                          /* G物理层唤醒恢复时间,单位us */
    PHY_UINT32                          uwTaskSwitchTime;                       /* 操作系统任务切换时间,单位us */
    PHY_UINT32                          uwPaPwrRsmTime;                         /* PA电源稳定时间,单位us */
}UCOM_NV_WG_DRX_RESUME_TIME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_SUPPORT_RATMODE_MASK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : modem支持的所有模式信息
             bit0:是否支持GSM;
             bit1:是否支持WCDMA;
             bit2:是否支持LTE;
             bit3:是否支持TDS;
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwSupportRatMask;                      /* Modem支持的所有mode信息 */
}UCOM_NV_SUPPORT_RATMODE_MASK_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_PA_DEFAULT_VOLT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PA默认电压值,电压单位:1mV
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwWcdmaPaVolt;                         /* WCDMA PA默认电压 */
    PHY_UINT16                          uhwGsmPaVolt;                           /* GSM PA默认电压 */
}UCOM_NV_PA_DEFAULT_VOLT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_ABB_PLL_STABLE_TIME_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PLL稳定时间,稳定时间单位:1us
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwWcdmaPllStableTime;                  /* WCDMA PLL稳定时间 */
    PHY_UINT16                          uhwGsmPllStableTime;                    /* GSM PLL稳定时间 */
}UCOM_NV_WG_ABB_PLL_STABLE_TIME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_MMMBPA_OFF_REG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mmmb pa off寄存器
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRegAddr;
    PHY_UINT16                          uhwValue;
}UCOM_NV_WG_MMMBPA_OFF_REG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_MMMBPA_OFF_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mmmb pa off控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidNumber;
    UCOM_NV_WG_MMMBPA_OFF_REG_STRU      stCmdReg[UCOM_NV_MAX_MMMBPA_CMD_NUMBER];
}UCOM_NV_WG_MMMBPA_OFF_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GUPHY_FUNC_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : v7r1c50 低功耗控制结构16位
*****************************************************************************/
typedef struct
{
    PHY_UINT16                  CsOnlyAntCtrl           :1;                     /* CS ONLY关分集控制 */
    PHY_UINT16                  ScAbbCtrl               :1;                     /* SC ABB控制 */
    PHY_UINT16                  AbbCtrl                 :1;                     /* CS ONLY abb控制 */
    PHY_UINT16                  RficRegCtrl             :1;                     /* CS ONLY RF调寄存器控制 */
    PHY_UINT16                  RficVolCtrl             :1;                     /* CS ONLY RF调电压控制 */
    PHY_UINT16                  GudspPowerDownCtrl      :1;                     /* GU 从模下电控制 */
    PHY_UINT16                  SimpleSlotIntCtrl       :1;                     /* W 精简时隙控制 */
    PHY_UINT16                  LoopRfLowpowerCtrl      :1;                     /* 环回允许RF低功耗控制 1允许 0不允许，默认不允许 */

    PHY_UINT16                  LoopRxAnt2Ctrl          :1;                     /* 环回允许rx动态分集控制 1允许 0不允许，默认不允许 */
    PHY_UINT16                  PsR99RxAnt2Ctrl         :1;                     /* ps r99 rx关分集控制 DCM需求*/
    PHY_UINT16                  PsRxAnt2Ctrl            :1;                     /* ps R99和DPA rx关分集控制为1相当于动态分集相关代码全部关闭 */
    PHY_UINT16                  PsVoltLPCtrl            :1;                     /* PS 调压低功耗特性代码控制位, 1打开 0不打开，默认打开 */
    PHY_UINT16                  PsCsAntCtrl             :1;                     /* PS CS并发下关分集控制 */
    PHY_UINT16                  reserved                :1;
    PHY_UINT16                  DpaOpenIpc              :1;                     /* dpa业务下E5776、860产品形态打开IPC中断开关控制，默认不打开 */
    PHY_UINT16                  GphyCsInterferennceCtrl :1;                     /* GSM CS同频干扰掉话问题规避方案控制:1-表示打开该功能,否则表示关闭 */
}UCOM_NV_GUPHY_FUNC_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_LP_RFIC_FUNC_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : v7r1c50 低功耗RFIC控制结构
*****************************************************************************/
typedef struct
{
    UCOM_NV_LP_RFIC_REGTHD_STRU         stRegThd;
    UCOM_NV_LP_RFIC_REGOPT_STRU         stRegOpt;
    UCOM_NV_LP_RFIC_REGDEFAULT_STRU     stRegDefault;

    PHY_UINT16                          uhwReserved;
}UCOM_NV_LP_RFIC_FUNC_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WG_TEMPPROTECT_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTempTypeId;                          /* 0-13 */
    PHY_UINT16                          uhwCloseAnt2Flag;                       /* 温保是否开关分集 */
    PHY_INT16                           shwCloseAnt2TempThresh;                 /* 高温关闭分集门限 */
    PHY_INT16                           shwOpenAnt2TempThresh;                  /* 低温打开分集门限 */
    PHY_UINT16                          uhwMaxPowLimitFlag;                     /* 温保是否限制最大发射功率 */
    PHY_INT16                           shwMaxPowLimitAlarmThresh;              /* 温保限制最大发射功率,高温门限 */
    PHY_INT16                           shwMaxPowLimitResumeThresh;             /* 温保限制最大发射功率,恢复门限 */
    PHY_INT16                           shwWMaxPowLimitValue;                   /* 温保限制W最大发射功率 */
    PHY_INT16                           shwGMaxPowLimitValue;                   /* 温保限制G最大发射功率 */
}UCOM_NV_WG_TEMPPROTECT_CTRL_STRU;

/* xiongjiangjiang Edn */

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_RX_BIAS_DELTA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF RX DELTA GAIN
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxChanDelay;
}UCOM_NV_W_RF_TX_CHAN_DELAY_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ANT_PERFORM_TEST_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwAntPerformanceTest;
}UCOM_NV_W_ANT_PERFORM_TEST_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_CQI_ANT2_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwCQICtrlFlag;
    PHY_UINT16                          uhwCQIThresh1;                          /* CQI高门限 */
    PHY_UINT16                          uhwCQIThresh2;                          /* CQI低门限 */
    PHY_UINT16                          uhwCQITime1;                            /* CQI高于高门限关分集延迟时间 */
    PHY_UINT16                          uhwCQITime2;                            /* CQI低于低门限开分集延迟时间 */
}UCOM_NV_W_CQI_ANT2_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_UL_RF_GATE0_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwUlRfGate0;
}UCOM_NV_W_UL_RF_GATE0_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_UL_RF_GATE1_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwUlRfGate1;
}UCOM_NV_W_UL_RF_GATE1_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_INIT_DELAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwPaInitDelay;
}UCOM_NV_W_PA_INIT_DELAY_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_OFF_DELAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwPaOffDelay;
}UCOM_NV_W_PA_OFF_DELAY_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_INIT_DELAY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxInitDelay;
}UCOM_NV_W_TX_INIT_DELAY_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_OFF_DELAY_STRUS
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxOffDelay;
}UCOM_NV_W_TX_OFF_DELAY_STRUS;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_IDLE_ONE_DELAY_STRUS
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxIdleOnDelay;
}UCOM_NV_W_TX_IDLE_ONE_DELAY_STRUS;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_IDLE_OFF_DELAY_STRUS
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxIdleOffDelay;
}UCOM_NV_W_TX_IDLE_OFF_DELAY_STRUS;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_ABB_INIT_DELAY_STRUS
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxAbbInitDelay;
}UCOM_NV_W_TX_ABB_INIT_DELAY_STRUS;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_ABB_OFF_DELAY_STRUS
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwTxAbbOffDelay;
}UCOM_NV_W_TX_ABB_OFF_DELAY_STRUS;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_GAIN_CTRL_TIMING_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          shwTxGainOpenTime;                      /* 提前打开上行RF增益的chip数 */
}UCOM_NV_W_TX_GAIN_CTRL_TIMING_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_DRX_CS_PERIOD_MEAS_CLASS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 影响小区搜索周期的测量等级因子
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwClass1Factor;                        /* 除以下两种情况外的其它所有 */
    PHY_UINT16                          uhwClass2Factor;                        /* 满足测量下移准则，且只有本频或异频 */
    PHY_UINT16                          uhwClass3Factor;                        /* 只有主小区测量 */
    PHY_UINT16                          uhwRsv;
}UCOM_NV_W_DRX_CS_PERIOD_MEAS_CLASS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_DRX_MEAS_CORR_LENTH_MEAS_CALSS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 影响测量相关长度的测量等级因子
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwClass1Factor;                        /* 除以下两种情况外的其它所有 */
    PHY_UINT16                          uhwClass2Factor;                        /* 满足测量下移准则，且只有本频或异频 */
    PHY_UINT16                          uhwClass3Factor;                        /* 只有主小区测量 */
    PHY_UINT16                          uhwRsv;
}UCOM_NV_W_DRX_MEAS_CORR_LENTH_MEAS_CALSS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_DRX_STRONG_CELL_CTRL_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 强邻区控制信息
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwIntraStrongCellNum;                  /* 本频强邻区个数 */
    PHY_UINT16                          uhwIntraMeasReserveCellNum;             /* 每次本频测量保留的强小区个数 */
    PHY_UINT16                          uhwInterStrongCellNum;                  /* 异频强邻区个数 */
    PHY_UINT16                          uhwInterMeasReserveCellNum;             /* 每次异频测量保留的强小区个数 */
}UCOM_NV_W_DRX_STRONG_CELL_CTRL_INFO_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_DRX_CTRL_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwDrxMaxCsPeriodLenth;                 /* DRX态下最大的小区搜索周期 */
    PHY_UINT16                          uhwDrxMinMeasCorrLenth;                 /* DRX态下最小的测量相关长度 */
    PHY_UINT16                          uhwDrxMaxMeasCorrLenth;                 /* DRX态下最大的测量相关长度 */
    PHY_UINT16                          uhwDrxMaxCsStep1Lenth;                  /* DRX态下最大的小区搜索step1长度 */
    PHY_UINT16                          uhwDrxMinCsStep1Lenth;                  /* DRX态下最小的小区搜索step1长度 */
    PHY_UINT16                          uhwRsv;
    UCOM_NV_W_DRX_CS_PERIOD_MEAS_CLASS_STRU         stDrxCsPeriodMeasClass;         /* 影响小区搜索周期的测量等级因子 */
    UCOM_NV_W_DRX_MEAS_CORR_LENTH_MEAS_CALSS_STRU   stDrxMeasCorrLenthMeasClass;    /* 影响测量相关长度的测量等级因子 */
    UCOM_NV_W_DRX_STRONG_CELL_CTRL_INFO_STRU        stDrxStrongCellCtrlInfo;        /* 强邻区控制信息 */
}UCOM_NV_W_DRX_CTRL_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_CQI_CORRECT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwCqiCorrectEnable;                     /* CQI动态补偿使能开关,0 关闭；1 打开 */
    PHY_UINT32                          uwSblerHighThreshold;                   /* SBLER 高控制门限,单位0.01 */
    PHY_UINT32                          uwSblerLowThreshold;                    /* SBLER 低控制门限,单位0.01 */
    PHY_UINT32                          uwScheduleRateThreshold;                /* 调度率控制门限,单位0.01 */
}UCOM_NV_W_CQI_CORRECT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_PSCH_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwPschLen;                             /* PSCH搜索的相干长度 */
    PHY_UINT16                          uhwPschThd;                             /* PSCH搜索的噪声门限 */
}UCOM_NV_W_PSCH_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TOTOLPC_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwOpenTotolpcFlg;                  /* 是否启动Tot Olpc算法标志，默认值1 */
    PHY_UINT16                              uhwOutageProbability;               /* Totolpc算法中的参数，默认值15*/
    PHY_UINT16                              uhwSirErrorStep;                    /* Totolpc算法中的参数，默认值1*/
    PHY_UINT16                              uhwCorrectionFactor;                /* Totolpc算法中的参数，默认值10*/
    PHY_UINT16                              uhwSirOutageOffset;                 /* Totolpc算法中的参数，默认值0*/
    PHY_INT16                               shwOutageIncrementThresUp;          /*TOT OLPC算法中的参数OutageIncrementThresholdUp，默认值26*/
    PHY_INT16                               shwOutageIncrementThresDown;        /*TOT OLPC算法中的参数OutageIncrementThresholdDown，默认值-19*/
    PHY_INT16                               shwOutagetargetStepUp;              /*TOT OLPC算法中的参数OutagetargetStepUp，默认值10*/
    PHY_INT16                               shwOutagetargetStepDown;            /*TOT OLPC算法中的参数OutagetargetStepDown，默认值-10*/
    PHY_INT16                               shwReserve;                             /*保留位，默认值0*/
} UCOM_NV_W_TOTOLPC_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_CPC_DRX_STRU
 协议表格  :
 枚举说明  :CPC DRX项目使用的NV结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRfWorkMode;                          /* RF进入低功耗状态时，进入Idle态还是进入StandBy态*/
    PHY_UINT16                          uhwCpcDrxCtrlFlag;                      /* 控制CPC DRX相关功能开关的标志*/
}UCOM_NV_W_CPC_DRX_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_CAL_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :IP2校准门限,主集分集共用同一个结构
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwIp2TxRefPower;                  /* 校准时上行通道的功率，单位为0.1dbm */
    PHY_INT16                           shwIp2Imd2Threshold;                /* 校准后IMD2的门限，单位db */
}UCOM_NV_W_RX_IP2_CAL_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_IQ_MISMATCH_CAL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :TX IQ MISMATCH校准初始功率和门限
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTxIqMismatch2RefPower;           /* 校准时上行通道的功率，单位为0.1dbm */
    PHY_INT16                           shwAmpCorrThreshold;                /* 校准后IMD2的门限，单位db */
    PHY_INT16                           shwDcCorrThreshold;                 /* 校准后IMD2的门限，单位db */
}UCOM_NV_W_TX_IQ_MISMATCH_CAL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RSSI_ARITHMETIC_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwRssiArithmeticPara;
}UCOM_NV_W_RSSI_ARITHMETIC_PARA_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_APT_ENABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwWBand;                      /* bit位是1代表支持APT */
    PHY_UINT32                          uwWBandExtRsv;                /* bit位是1代表支持APT */
}UCOM_NV_W_TX_PA_APT_ENABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_EEIC_EN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwEEICEn;
}UCOM_NV_W_EEIC_EN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_IPS_FLG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwIpsFlg;
}UCOM_NV_W_IPS_FLG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_SPUR_REMOVE_ALG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 谐波消除算法
*****************************************************************************/
typedef struct
{
    UCOM_NV_WCDMA_SPUR_TYPE_ENUM_UINT16 enSpurType;                             /* 谐波点类型 */
    PHY_UINT16                          uhwMainRemoveEnable;                    /* 主集使能 */
    PHY_UINT16                          uhwDivRemoveEnable;                     /* 分集使能 */
    PHY_INT16                           shwRssiThreshold;                       /* 启动谐波消除算法的rssi门限 */
    PHY_UINT16                          uhwFilterCoeff;                         /* 滤波因子 */
}UCOM_NV_W_RX_SPUR_REMOVE_ALG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_M_VALUE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              auhwRfMValue[2];                    /* 0: usRfScMDaValue 1:usRfDcMDaValue */
}UCOM_NV_W_RF_M_VALUE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_LOWPOWER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : W低功耗特性所需门限以及开关
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwAllocToUseExtraMpUnit;       /* 允许使用10~19单元,范围 [0,1] ,推荐 0 */
    PHY_UINT16                          uhwAllocToUseExtraMpUnitInCPC;  /* CPC 允许使用10~15单元,范围 [0,1],推荐 1 */
    PHY_UINT16                          uhwIntraFMSRUsing5msPerFrEn;    /* 本频5ms方案, 范围 [0,1]，推荐 1 */

    /* 关闭 EEIC */
    PHY_UINT16                          uhwThrptUppLmtPerCrNMThd;       /* 范围0~2000,精度1(100Kbps),推荐 130 */
    PHY_UINT16                          uhwThrptUppLmtPerCrNonMimoEn;   /* 范围[0,1],推荐 1 */

    /* 关闭分集 */
    PHY_UINT16                          uhwCorrCoefAccFrm;              /* 比较统计帧数，单位10帧，范围0~1024,推荐 10 */
    PHY_UINT32                          uwCorrCoefShutDownThd;          /* 关闭门限，为原值平方后放大10000倍,范围0~1000000,默认 400, */
    PHY_UINT16                          uhwAntRssiShutDownThd;          /* 关闭门限，单位1dBm，范围0~100，推荐20 推荐 10 */
    PHY_UINT16                          uhwCorrCoefCalEn;               /* 范围[0,1],推荐 1 */
    PHY_UINT16                          uhwAntRssiCmprEn;               /* 范围[0,1],推荐 1 */
    PHY_UINT16                          uhwAntRssiCmprAccFrm;           /* 范围0~1024, 推荐 10 */
    PHY_UINT16                          uhwCorrCoefReCalDelayFrm;       /* 范围0~1024, 推荐 10 */
    PHY_UINT16                          uhwAntRssiReOpenDelayFrm;       /* 范围0~1024, 推荐 10 */

    /* Wd静态信道按时隙更新 */
    PHY_UINT16                          uhwWdCalPerSlotEn;              /* 范围 [0,1],    推荐 1 */
    PHY_UINT16                          uhwWdCalPerSlotThd;             /* 范围0~511,   推荐 251 */

}UCOM_NV_W_LOWPOWER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_ENABLE_SELECT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : w pa en 4线选择
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwLineSel[UCOM_NV_WCDMA_PA_EN_LINE_NUM];
}UCOM_NV_W_PA_ENABLE_SELECT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_HSUPA_MAX_POWER_TEST_MODE_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : HSUPA最大发射功率测试模式控制
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwEnableFlag;
    PHY_UINT16                          auhwMinSetEtfci[UCOM_NV_HSUPA_ETFCI_TABLE_MAX_NUM];
}UCOM_NV_W_HSUPA_MAX_POWER_TEST_MODE_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_HDET_TIMING_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 功率检测中使用的NV项结构
*****************************************************************************/
typedef struct
{
    PHY_INT16                              shwPaStableTiming;                   /* 发射功率变化后，RF与PA的稳定时间，单位为chip*/
    PHY_INT16                              shwPdHkadcChanSelTiming;             /*授权有效到发送HKADC通道控制字时刻间隔，单位为chip*/
    PHY_INT16                              shwPdStartInterval;                  /* ABB从bypass为低到启动start（ABB开始转换）的时间间隔，单位为chip*/
    PHY_INT16                              shwPdInterval;                       /* 两次功率检测之间的时间间隔，单位为chip*/
    PHY_INT16                              shwPdsampleTiming;                   /* HKADC采样信号处理时间，单位为chip*/
    PHY_INT16                              shwDcOffsetSampleNum;                /*WHKADC上行每个时隙进行dc offset检测的次数*/
    PHY_INT16                              shwPdSampleNum;                      /*W上行每个时隙进行PD检测的次数*/
} UCOM_NV_W_HDET_TIMING_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示每档RF Gain对应的温度补偿值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTempComp[UCOM_NV_W_TX_TEMP_COMP_NUM];       /* 表示某一档RF Gain在16个采样温度下的温度补偿值，单位为0.1dB */
}UCOM_NV_W_TX_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_RF_GAIN_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示15档RF Gain对应的温度补偿值
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCount;                                   /* 有效的档位数 */
    UCOM_NV_W_TX_TEMP_COMP_STRU         astTempComp[UCOM_NV_W_TX_RF_GAIN_CTRL_NUM];     /* 表示15档RF Gain在16个采样温度下的温度补偿值 */
}UCOM_NV_W_TX_RF_GAIN_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_VOICE_APC_SIG_ENABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwVoiceApcSigEnable;               /* 表示在信令模式下是否激活Voice模式的APC表 */
}UCOM_NV_W_VOICE_APC_SIG_ENABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_VOICE_APC_CAL_ENABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwVoiceApcCalEnable;               /* 表示在校准模式下是否校准Voice模式的APC表 */
}UCOM_NV_W_VOICE_APC_CAL_ENABLE_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_RX_AGC_GAIN_VALID_COUNT_STRU
 协议表格  :
 枚举说明  : AGC档位个数的NV项，包括BLOCK和非BLOCK
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwAgcGainNoBlockNum;                   /* 非Block模式的AGC控制字 */
    PHY_UINT16                          uhwAgcGainBlockNum;                     /* Block模式的AGC控制字 */
}UCOM_NV_W_RX_AGC_GAIN_VALID_COUNT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_RF_GAIN_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : rf gain ctrl 档位数
*****************************************************************************/
typedef struct
{
    PHY_UINT32          uwValidCount;
    PHY_UINT16          auhwRfGainCtrl[UCOM_W_MAX_RF_GAIN_CTRL_NUM];
}UCOM_NV_W_TX_RF_GAIN_CTRL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TX_RF_BIAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : rf BIAS 档位数
*****************************************************************************/
typedef struct
{
    PHY_UINT32          uwValidCount;
    PHY_UINT16          auhwRfBias[UCOM_W_MAX_RF_BIAS_NUM];
}UCOM_NV_W_TX_RF_BIAS_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_BB_GAIN_VS_ATTEN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : BB每个档位对应的衰减值
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwBbGainCtrl;
    PHY_INT16           shwBbAtten10th;
}UCOM_NV_W_BB_GAIN_VS_ATTEN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_BB_GAIN_TABLE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : BB的档位数
*****************************************************************************/
typedef struct
{
    PHY_UINT32                             uwValidCount;
    UCOM_NV_W_BB_GAIN_VS_ATTEN_STRU        astBbGain[UCOM_W_MAX_RF_BB_GAIN_NUM];
}UCOM_NV_W_BB_GAIN_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ECIOR_CTRL_STRU
 结构说明  : ec/ior的控制结构体
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwCqiLoadSwitchThreshold;          /* CQI切换门限，默认配置为15;*/
    PHY_UINT16                          uhwLoadRateThreshold1;              /* 调度率门限1，默认配置为6;*/
    PHY_UINT16                          uhwLoadRateThreshold2;              /* 调度率门限2，默认配置为10; */
} UCOM_NV_W_ECIOR_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_ANT_TUNER_CTRL_WORD_STRU
 结构说明  : 驻波检测前向反向控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwForWardWord;                      /* 驻波检测前向配置 */
    PHY_UINT32                              uwReverseWord;                      /* 驻波检测反向配置 */
} UCOM_NV_ANT_TUNER_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ANT_TUNER_CTRL_PARA_STRU
 结构说明  : 天线调谐的控制结构体
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwCodeSearchStep;                  /* 调谐时码字搜索步长 */
    PHY_UINT16                              uhwBypassMaxCode;                   /* Bypass时的最大码字，例RF1108为0x40 */
    PHY_UINT16                              uhwBypassMinCode;                   /* Bypass时的最小码字，例RF1108为0x5F */
    PHY_UINT16                              uhwUpperCode;                       /* 天线调谐码字上限，例RF1108为0 */
    PHY_UINT16                              uhwLowerCode;                       /* 天线调谐码字下限，例RF1108为31 */
    PHY_UINT16                              auhwReserve[3];                     /* 不确定码字格式和大小，先保留 */
} UCOM_NV_W_ANT_TUNER_CODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ANT_TUNER_CTRL_PARA_STRU
 结构说明  : 天线调谐的控制结构体
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwBandSupport;                      /* 对应bit为1表示这个band支持动态天线调谐,否则不支持 */
    PHY_UINT32                              uwBandExten;                        /* 以后band ID可能超出32 */
    PHY_UINT16                              uhwActiveTime;                      /* 天线调谐配置提前帧头配置量，当上行打开后，WBBP每次在帧头提前cpu_tx_sw_active_time进行配置，单位为chip */
    PHY_UINT16                              uhwSampleTime;                      /* 天线调谐平均检测次数,默认配置为4 */
    PHY_UINT16                              uhwDelayFrame;                      /* 完成一轮调谐后的等待时间，单位帧 */
    PHY_UINT16                              uhwEchoLossThreshold;               /* 天线调谐回波损耗门限,精度0.1dBm */
    UCOM_NV_ANT_TUNER_CTRL_WORD_STRU        stCtrlWord;                         /* 驻波检测前向反向控制字 */
    UCOM_NV_W_ANT_TUNER_CODE_STRU           stAntTunerCode;                     /* 天线调谐码字信息 */
} UCOM_NV_W_ANT_TUNER_CTRL_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RF_VCO_REG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF VCO REG
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRegAddress;
    PHY_UINT16                          uhwRegValue;
}UCOM_NV_W_RF_VCO_REG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_VCO_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF VCO CONFIG
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCounter;
    UCOM_NV_W_RF_VCO_REG_STRU           astRegConfig[UCOM_NV_MAX_RF_VCO_CONFIG_NUM];
}UCOM_NV_W_RF_VCO_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_RF_TX_BIAS_REG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF TX BIAS
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwRegAddress;
    PHY_UINT16                          uhwRegValue;
}UCOM_NV_W_RF_TX_BIAS_REG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_TX_BIAS_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF TX BIAS
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCounter;
    UCOM_NV_W_RF_TX_BIAS_REG_STRU       astBias[UCOM_NV_W_TX_RF_BIAS_NUM];
}UCOM_NV_W_RF_TX_BIAS_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_RFIC_BIAS_CFG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF TX BIAS
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwRfRxBiasAddress;                                     /* TX BIAS地址 */
}UCOM_NV_W_RX_RFIC_BIAS_CFG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_RX_BIAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF RX BIAS
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwNoBlockStateBias[UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM]; /* no block bias的NV项 */
}UCOM_NV_W_RF_RX_BIAS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_RX_BIAS_DELTA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RF RX DELTA GAIN
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwNoBlockStateDeltaGain[UCOM_NV_W_ANT_NUM][UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM]; /* no block delta gain的NV项 */
}UCOM_NV_W_RF_RX_BIAS_DELTA_GAIN_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_RX_AGC_SWITCH_VOICE_MODE_STRU
 协议表格  :
 枚举说明  : VOICE模式下的AGC档位切换门限的NV项，只包括非BLOCK状态的
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwNoBlockAgcThresholdInc[UCOM_NV_W_AGC_GAIN_NOBLK_THRESHOLD_NUM];           /* NoBlock方式下档位增加 */
    PHY_INT16                           ashwNoBlockAgcThresholdDec[UCOM_NV_W_AGC_GAIN_NOBLK_THRESHOLD_NUM];           /* NoBlock方式下档位增加 */
}UCOM_NV_W_RX_AGC_SWITCH_VOICE_MODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_MMMBPA_CMD_ADDRESS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mmmb pa 寄存器地址
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidNumber;
    PHY_UINT16                          auhwCmdAddress[UCOM_NV_MAX_MMMBPA_CMD_NUMBER];
}UCOM_NV_WG_MMMBPA_CMD_ADDRESS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_MIPI_ANT_CMD_ADDRESS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mipi ant 寄存器地址
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidNumber;
    PHY_UINT16                          auhwCmdAddress[UCOM_NV_MAX_MIPI_ANT_CMD_NUMBER];
}UCOM_NV_WG_MIPI_ANT_CMD_ADDRESS_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_HEET_HKADC_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwSeperatePaHdetChannel;           /* 分立PA HDET通道 */
    PHY_UINT16                          uhwMmmbPaHdetChannel;               /* MMMB PA HDET通道 */
}UCOM_NV_W_HEET_HKADC_CHAN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_DPA_IQ_REDUCTION_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DPA数字回退
*****************************************************************************/

typedef struct
{
    PHY_INT16                           ashwDpaIqReduction[UCOM_NV_W_TX_DPA_IQ_REDUCTION_NUM];
}UCOM_NV_W_TX_DPA_IQ_REDUCTION_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_SCUPA_IQ_REDUCTION_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : SC-UPA数字回退
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwScUpaIqReduction[UCOM_NV_W_TX_SCUPA_IQ_REDUCTION_NUM];
}UCOM_NV_W_TX_SCUPA_IQ_REDUCTION_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_DPA_MPR_STRU_HI6361
 协议表格  :
 ASN.1描述 :
 结构说明  : DPA功率回退
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwDpaMpr[UCOM_NV_W_TX_DPA_MPR_NUM];
}UCOM_NV_W_TX_DPA_MPR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_SCUPA_MPR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DPA功率回退
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwScUpaMpr[UCOM_NV_W_TX_SCUPA_MPR_NUM];
}UCOM_NV_W_TX_SCUPA_MPR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_DCUPA_MPR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DPA功率回退
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwDcUpaMpr[UCOM_NV_W_TX_DCUPA_MPR_NUM];
}UCOM_NV_W_TX_DCUPA_MPR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_HKADC_SSI_WAIT_TIME_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwHkadcSsiWaitTime;
}UCOM_NV_W_HKADC_SSI_WAIT_TIME_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_DRX_LTE_EVALUATE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LTE测量下移门限参数结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwLteEvaluateServCellReportCorr;       /* Lte测量不下移时服务小区测量上报周期系数默认6，范围0，30 */
    PHY_INT16                           shwLteEvaluateServCellRscpThres;        /* Lte测量下移判决的服务小区RSCP绝对值门限，单位0.125db，默认-640，范围-1040，-320 */
    PHY_UINT16                          uhwLteEvaluateServCellDeltaThres;       /* Lte测量下移判决的服务小区rscp变化值门限，单位0.125db，默认160，范围0，480 */
}UCOM_NV_W_DRX_LTE_EVALUATE_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_VOLT_LOWPOWER_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 调压低功耗控制NV
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwLowVoltDeltaPower;                   /* 降压最大发射功率差值门限,默认2dBm,,范围0~24dBm */
    PHY_UINT16                          uhwDataFrameThreshold;                  /* 连续数据帧个数门限,默认50,单位10msOr2ms */
    PHY_UINT16                          uhwNoDataFrameThreshold;                /* 连续无数据帧个数门限,默认80,单位10ms */
    PHY_INT16                           shwLowRssiThreshold;                    /* 升压RSSI门限,默认-95dBm */
    PHY_INT16                           shwHighRssiThreshold;                   /* 降压RSSI门限,默认-85dBm */
    PHY_UINT16                          uhwTxPowerCntThreshold;                 /* 升压发射功率统计个数门限,默认8,单位10ms */
    PHY_UINT16                          uhwHsupaRemainPowerCntThreshold;        /* 升压HSUPA剩余功率不足统计个数门限,默认8,单位10ms */
    PHY_UINT16                          uhwR99RemainPowerCntThreshold;          /* 升压R99剩余功率不足统计个数门限,默认8,单位10ms */
    PHY_UINT16                          uhwForceConfigLowPowerThreshold;        /* 强制降压计时门限,默认100,单位10ms */
}UCOM_NV_VOLT_LOWPOWER_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ANT2_OFF_EVA_THRESH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 天线2关闭的评估门限参数结构
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTimeRxNoDataDpa;                     /* 80、160、320、640、1000单位ms。默认160ms */
    PHY_UINT16                          uhwTimeRxNoDataR99Turbo;                /* 80、160、320、640、1000单位ms。默认160ms */
    PHY_UINT16                          uhwTimeRxNoDataR99Vtb;                  /* 80、160、320、640、1000单位ms。默认160ms */
}UCOM_NV_W_ANT2_OFF_EVA_THRESH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_RX_CAL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :需要校准的WCDMA的频点个数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidCalChanNum;
    PHY_UINT16                          auhwRxCalChan[UCOM_WCDMA_UARFCN_NUM];        /* _H2ASN_Replace PHY_UINT16_ARRAY2_16 auhwRxCalChan[UCOM_WCDMA_BAND_NUM][UCOM_NV_W_UARFCN_NUM]; */ /* RX频点区间，3个频段，每个频段16个UARFCN */
}UCOM_NV_WCDMA_RX_CAL_CHAN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_CAL_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :3.3需要进行IP2校准的频点
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwHighFreq;
    PHY_UINT16                          uhwMidFreq;
    PHY_UINT16                          uhwLowFreq;
}UCOM_NV_W_RX_IP2_CAL_CHAN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_CAL_AT1_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :主集RX通道每个BAND在高中低频点分集关闭和打开两种情况下，对I和Q路的电路参数补偿
*****************************************************************************/
typedef struct
{
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetHDivyOn;                   /* 高频点分集开启 */
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetHDivyOff;                  /* 高频点分集关闭 */
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetMDivyOn;                   /* 中频点分集开启 */
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetMDivyOff;                  /* 中频点分集关闭 */
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetLDivyOn;                   /* 低频点分集开启 */
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetLDivyOff;                  /* 低频点分集关闭 */
}UCOM_NV_W_RX_IP2_CAL_AT1_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_CAL_AT2_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :分集RX通道每个BAND在高中低频点下，对I和Q路的电路参数补偿
*****************************************************************************/
typedef struct
{
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetH;
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetM;
    UCOM_NV_RX_IP2_STRU                 stRxIp2OffsetL;
}UCOM_NV_W_RX_IP2_CAL_AT2_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :IP2校准门限,主集分集共用同一个结构
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwIp2StopThreshold;                    /* 停止门限 */
    PHY_INT16                           shwIp2FailThreshold;                    /* 失败门限 */
}UCOM_NV_W_RX_IP2_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_IP2_CAL_ENABLE_AT2_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwRxIp2CalEnableAt2;           /* 0表示启动分集天线IP2校准，1表示不启动 */
}UCOM_NV_W_RX_IP2_CAL_ENABLE_AT2_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_FREQ_COMP_ANT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :3.5 AGC增益在统一个频段内因频点不同而引起的增益变化
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxAgcFreqComp[UCOM_WCDMA_UARFCN_NUM];
}UCOM_NV_W_RX_AGC_FREQ_COMP_ANT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_ANY_CARRIER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain校准值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxAgcGainNoBlock[UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM];
    PHY_INT16                           ashwRxAgcGainBlock[UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM];
}UCOM_NV_W_RX_AGC_GAIN_ANY_CARRIER_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain校准值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_GAIN_ANY_CARRIER_STRU            astAgcGain[UCOM_NV_DC_SC_MODE_BUTT];    /* AGC Gain,区分主辅载波 */
}UCOM_NV_W_RX_AGC_GAIN_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain校准值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_GAIN_STRU            astRxAgcGainAnt[UCOM_NV_W_ANT_NUM];    /* AGC Gain,区分主辅载波 */
}UCOM_NV_W_RX_AGC_GAIN_DANT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_PRE_ANY_CARRIER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain Predefined值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxAgcGainPreNoBlock[UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM];
    PHY_INT16                           ashwRxAgcGainPreBlock[UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM];
}UCOM_NV_W_RX_AGC_GAIN_PRE_ANY_CARRIER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_PRE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain Predefined值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_GAIN_PRE_ANY_CARRIER_STRU
                                        astRxAgcGainPreCarrier[UCOM_NV_DC_SC_MODE_BUTT];         /* AGC Gain Pre值,区分主辅载波 */
}UCOM_NV_W_RX_AGC_GAIN_PRE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_GAIN_PRE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Gain Predefined值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_GAIN_PRE_STRU
                                        astRxAgcGainPreAnt[UCOM_NV_W_ANT_NUM];         /* AGC Gain Pre值,区分主辅载波 */
}UCOM_NV_W_RX_AGC_GAIN_PRE_DANT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_FRONT_END_GAIN_PRE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Front End Predefined值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwRxFrontEndGainPre[UCOM_NV_DC_SC_MODE_BUTT];   /* Front End Predefined */
}UCOM_NV_W_RX_AGC_FRONT_END_GAIN_PRE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RX_AGC_FRONT_END_GAIN_PRE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : AGC Front End Predefined值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_FRONT_END_GAIN_PRE_STRU         astRxFrontEndGainPre[UCOM_NV_W_ANT_NUM];   /* Front End Predefined */
}UCOM_NV_W_RX_AGC_FRONT_END_GAIN_PRE_DANT_STRU;


/*****************************************************************************
 枚举名    : RX_AGC_GAIN_SWITCH_THRESHOLD_STRU
 协议表格  :
 枚举说明  : AGC档位切换门限的NV项，包括BLOCK和非BLOCK
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwNoBlockAgcThresholdInc[UCOM_NV_W_AGC_GAIN_NOBLK_THRESHOLD_NUM];           /* NoBlock方式下档位增加 */
    PHY_INT16                           ashwNoBlockAgcThresholdDec[UCOM_NV_W_AGC_GAIN_NOBLK_THRESHOLD_NUM];           /* NoBlock方式下档位增加 */
    PHY_INT16                           ashwBlockAgcThresholdInc[UCOM_NV_W_AGC_GAIN_BLK_THRESHOLD_NUM];             /* Block方式下档位增加 */
    PHY_INT16                           ashwBlockAgcThresholdDec[UCOM_NV_W_AGC_GAIN_BLK_THRESHOLD_NUM];             /* Block方式下档位增加 */
}UCOM_NV_W_RX_AGC_GAIN_SWITCH_THRESHOLD_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_RX_AGC_GAIN_PREDEFINED_ANYCARRIER_STRU
 协议表格  :
 枚举说明  : AGC档位切换门限的NV项，包括BLOCK和非BLOCK
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_GAIN_SWITCH_THRESHOLD_STRU      astRxAgcGainSwitchThreshold[UCOM_NV_DC_SC_MODE_BUTT];
}UCOM_NV_W_RX_AGC_GAIN_SWITCH_THRESHOLD_DCARR_STRU;


/*****************************************************************************
 枚举名    : UCOM_NV_W_RX_AGC_CTRL_STRU
 协议表格  :
 枚举说明  : AGC控制字的NV项，包括BLOCK和非BLOCK
*****************************************************************************/
typedef struct
{
    UCOM_NV_RX_AGC_CTRL_UNION           unAgcCtrlNoBlock[UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM];                   /* 非Block模式的AGC控制字 */
    UCOM_NV_RX_AGC_CTRL_UNION           unAgcCtrlBlock[UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM];                     /* Block模式的AGC控制字 */
}UCOM_NV_W_RX_AGC_CTRL_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_RX_AGC_CTRL_STRU
 协议表格  :
 枚举说明  : AGC控制字的NV项，包括BLOCK和非BLOCK
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_RX_AGC_CTRL_STRU  astRxAgcCtrl[UCOM_NV_DC_SC_MODE_BUTT];
}UCOM_NV_W_RX_AGC_CTRL_DCARR_STRU;



/*****************************************************************************
 枚举名    : UCOM_NV_W_DCOFFSET_STRU
 协议表格  :
 枚举说明  : DC offset的结构体类型
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwDci;                                 /* I路的直流值 */
    PHY_INT16                           shwDcq;                                 /* Q路的直流值 */
}UCOM_NV_W_DCOFFSET_STRU;


/*****************************************************************************
 枚举名    : UCOM_NV_W_DCOFFSET_ANYCARRIER_STRU
 协议表格  :
 枚举说明  : DC offset的结构体类型
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_DCOFFSET_STRU             astNoBlockDcOffset[UCOM_NV_W_AGC_GAIN_BLK_STEP_NUM];       /* block模式下各档位的DC offset */
    UCOM_NV_W_DCOFFSET_STRU             astBlockDcOffset[UCOM_NV_W_AGC_GAIN_NOBLK_STEP_NUM];         /* 非block模式下各档位的DC offset */
}UCOM_NV_W_DCOFFSET_ANYCARRIER_STRU;


/*****************************************************************************
 枚举名    : W_DCOFFSET_ANT_STRU
 协议表格  :
 枚举说明  : DC offset的结构体类型
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_DCOFFSET_ANYCARRIER_STRU              astAnyCarrierDcOffset[UCOM_NV_DC_SC_MODE_BUTT];   /* SC\DC载波模式下的DC值 */
}UCOM_NV_W_DCOFFSET_ANT_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_DCOFFSET_ANT_STRU
 协议表格  :
 枚举说明  : DC offset的结构体类型
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_DCOFFSET_ANT_STRU              astDcOffsetAnt[UCOM_NV_W_ANT_NUM];
}UCOM_NV_W_DCOFFSET_DANT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_FEM_CHAN_CONNECT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : A/B通道对等选择,前端Ch0/CH1通道选择
*****************************************************************************/
typedef struct
{
    UCOM_NV_CHAN_CONNECT_ENUM_UINT16    enRfConnect;                            /* RF~RFIC之间的连接:0:主集连接在RF_A，1:主集连接在RF_B */
    UCOM_NV_CHAN_CONNECT_ENUM_UINT16    enRfAbbConnect;                         /* RFIC与ABB之间的连接,硬件连线,不需要软件配置 */
    UCOM_NV_CHAN_CONNECT_ENUM_UINT16    enAbbBbpConnect;                        /* ABB与BBP COM1之间的连接:0:主集连接在ABB_A，1:主集连接在ABB_B */
    PHY_UINT16                          uhwChanSwitch;                          /* 定义每个band选择通道0或者1,目前V9R1上默认主卡通道都选择0,副卡通道都选择1. */

}UCOM_NV_FEM_CHAN_CONNECT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_MIPI_ANT_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mipi ant 寄存器控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwCtrlWord[UCOM_NV_MAX_MIPI_ANT_CMD_NUMBER];
}UCOM_NV_WG_MIPI_ANT_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_CAL_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :需要校准的WCDMA的频点个数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwValidCalChanNum;
    PHY_UINT16                          auhwTxCalChan[UCOM_WCDMA_UARFCN_NUM];        /* _H2ASN_Replace PHY_UINT16_ARRAY2_16 auhwRxCalChan[UCOM_WCDMA_BAND_NUM][UCOM_NV_W_UARFCN_NUM]; */ /* RX频点区间，3个频段，每个频段16个UARFCN */
}UCOM_NV_WCDMA_TX_CAL_CHAN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_RACH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxWiredMaxPowerRach;                             /* 三个频段，上行仅存在DPCH或存在RACH时UE可允许的最大发射功率，单位0.1dbm;第0列为RACH,第1列为DPCH, */
}UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_RACH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_DPCH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxWiredMaxPowerDpch;                             /* 三个频段，上行仅存在DPCH或存在RACH时UE可允许的最大发射功率，单位0.1dbm;第0列为RACH,第1列为DPCH, */
}UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_DPCH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwTxWiredMaxPowerTempComp[UCOM_NV_W_TEMP_NUM];    /* 最大发射功率补偿增益，单位0.1dbm;3个频段，5个预先定义的温度点:-20,0,20,40,60,第6个点用于对齐，不使用 */
}UCOM_NV_WCDMA_TX_WIRED_MAX_POWER_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_MIN_POWER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxMinPower;                                      /* UE允许的最小发射功率，单位0.1dbm;3个频段，5个预先定义的温度点:-20,0,20,40,60,第6个点用于对齐，不使用 */
}UCOM_NV_WCDMA_TX_MIN_POWER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_MIN_POWER_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwTxMinPowerTempComp[UCOM_NV_W_TEMP_NUM];         /* 最小发射功率温度补偿 */
}UCOM_NV_WCDMA_TX_MIN_POWER_TEMP_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_VGA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTxHdetVga;                                                   /* 表示PD的前级衰减 和VGA值*/
}UCOM_NV_W_TX_HDET_VGA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_DCOFFSET_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwTxHdetDcOffset;                                              /* 表示DCOFFSET */
}UCOM_NV_W_TX_HDET_DCOFFSET_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_POWER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTxHdetContPower[UCOM_NV_W_TX_HDET_PV_NUM];                  /* 高增益模式下，测试功率和电压关系的功率测试点 */
}UCOM_NV_W_TX_HDET_POWER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_HKADC_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTxHdetVolt[UCOM_NV_W_TX_HDET_PV_NUM];                       /* 高增益模式下，测试功率和电压关系时电压测试值 */
}UCOM_NV_W_TX_HDET_HKADC_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_FREQ_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTxHdetFreqComp[UCOM_WCDMA_UARFCN_NUM];                       /* 不同频点对功率器件影响不同，频率补偿值 */
}UCOM_NV_W_TX_HDET_FREQ_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwTxHdetTempComp[UCOM_NV_W_TEMP_NUM];  /* 不同温度对功率器件影响不同，温度补偿值.区分高中低信道,顺序为中高低 */
}UCOM_NV_W_TX_HDET_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_START_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTxHdetStart;                                                 /* TX 发射功率超过该值时，BBP自动启动功率检测 */
}UCOM_NV_W_TX_HDET_START_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_STOPT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTxHdetStop;                                                  /* TX 发射功率小于该值时，BBP自动停止功率检测 */
}UCOM_NV_W_TX_HDET_STOPT_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRE_MAXPOWER_RACH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxWirelessMaxPowerRach;                          /* 无线 *//* 空口发射功率:_UCOM_TX_PWR_CTRL_SWITCH */
}UCOM_NV_WCDMA_TX_WIRE_MAXPOWER_RACH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRE_MAXPOWER_DPCH_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxWirelessMaxPowerDpch;                          /* 无线 *//* 空口发射功率:_UCOM_TX_PWR_CTRL_SWITCH */
}UCOM_NV_WCDMA_TX_WIRE_MAXPOWER_DPCH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_WIRELESS_POWER_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwTxWirelessPowerTempComp[UCOM_NV_W_TEMP_NUM];    /* 空口功率下温度补偿 */
}UCOM_NV_WCDMA_TX_WIRELESS_POWER_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_GAIN_COMP_VS_PA_CHAN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwHighFreq;                            /* 用于判断高频点区间的频点 */
    PHY_UINT16                          uhwLowFreq;                             /* 用于判断低频点区间的频点 */
}UCOM_NV_W_TX_GAIN_COMP_VS_PA_CHAN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_GAIN_COMP_VS_PA_CH_STRU
 协议表格  :
 结构说明  : W TX PA增益切换点补偿的区间分界点
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidNum;                             /* 表示以下频点几个有效 */
    PHY_UINT16                          auhwFreqChan[UCOM_NV_W_PA_GAIN_SWITCH_COMP_CHAN_NUM];
}UCOM_NV_W_TX_GAIN_COMP_VS_PA_CH_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_GAIN_SWITCH_COMP_STRU
 协议表格  :
 结构说明  : W TX PA增益切换时功率补偿值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwH2MComp;                             /* PA增益从高切到中的门限补偿,单位0.1db */
    PHY_INT16                           shwM2HComp;                             /* PA增益从中切到高的门限补偿,单位0.1db */
    PHY_INT16                           shwM2LComp;                             /* PA增益从中切到低的门限补偿,单位0.1db */
    PHY_INT16                           shwL2MComp;                             /* PA增益从低切到中的门限补偿,单位0.1db */
}UCOM_NV_W_PA_GAIN_SWITCH_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_GAIN_SWITCH_COMP_STRU
 协议表格  :
 结构说明  : W TX 4个时隙的PA增益切换时功率补偿值
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_PA_GAIN_SWITCH_COMP_STRU  astSlotComp[UCOM_NV_W_PA_SWITCH_COMP_SLOT];
}UCOM_NV_W_TX_PA_GAIN_SWITCH_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_APC_TEMP_STRU
 协议表格  :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   shwTxApcTemp;                                       /* 保存校准时的温度值:本项保存中信道下,每个BAND的高中低PA增益模式下校准时候的温度值,列表示高中低 */
}UCOM_NV_W_TX_APC_TEMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_GAIN_SWITH_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_INT16                           shwPaGainThresH2M;                      /* PA从高增益到中增益的切换门限 */
    PHY_INT16                           shwPaGainThresM2H;                      /* PA从中增益到高增益的切换门限 */
    PHY_INT16                           shwPaGainThresM2L;                      /* PA从中增益到低增益的切换门限 */
    PHY_INT16                           shwPaGainThresL2M;                      /* PA从低增益到中增益的切换门限 */
}UCOM_NV_W_PA_GAIN_SWITH_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_RF_PA_GAIN_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT32                          uwRfPaCtrlHighMode;                     /* PA高增益模式控制字 */
    PHY_UINT32                          uwRfPaCtrlMediumMode;                   /* PA中增益模式控制字 */
    PHY_UINT32                          uwRfPaCtrlLowMode;                      /* PA低增益模式控制字 */
}UCOM_NV_W_RF_PA_GAIN_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_GAIN_SWITH_TIMING_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_INT16                           shwPaGainSwitchH2M;                     /* PA增益高增益到中增益切换时序 */
    PHY_INT16                           shwPaGainSwitchM2H;                     /* PA增益中增益到高增益切换时序 */
    PHY_INT16                           shwPaGainSwitchM2L;                     /* PA增益中增益到低增益切换时序 */
    PHY_INT16                           shwPaGainSwitchL2M;                     /* PA增益低增益到中增益切换时序 */
}UCOM_NV_W_PA_GAIN_SWITH_TIMING_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_TX_IQ_MISMATCH_DCR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :2.3 每组NV项目表示上行IQ mismatch和DC offset
            正常工作时，将此参数配置给DBB，对TX通道进行预畸变
            以弥补上行通道的IQ mismatch和残余DC。
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwAmplitude;                           /* 幅值校正因子 */
    PHY_INT16                           shwPhase;                               /* 相位校正因子 */
    PHY_INT16                           shwDCI;                                 /* 直流校正实部 */
    PHY_INT16                           shwDCQ;                                 /* 直流校正虚部 */
}UCOM_NV_TX_IQ_MISMATCH_DCR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示RFIC和DBB不衰减的情况下，高中低3档增益的增益值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwPaGain[UCOM_NV_W_TX_PA_MODE_NUM];           /* 非VOICE模式下，表示RFIC和DBB不衰减的情况下，高中低3档增益的增益值，单位为0.1dB */
}UCOM_NV_W_TX_PA_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_GAIN_VOICE_MODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 表示VOICE模式下，RFIC和DBB不衰减的情况下，高中低3档增益的增益值
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwPaGain[UCOM_NV_W_TX_PA_MODE_NUM];           /* 表示VOICE模式下，RFIC和DBB不衰减的情况下，高中低3档增益的增益值，单位为0.1dB */
}UCOM_NV_W_TX_PA_GAIN_VOICE_MODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PHASE_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :4.2 PA模式从高增益切换到中增益、中增益切换到低增益、低增益切换到
                中增益、中增益切换到高增益时，由于TX和PA会对基带IQ信号产生相位
                旋转。
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwPaH2M;
    PHY_INT16                           shwPaM2H;
    PHY_INT16                           shwPaM2L;
    PHY_INT16                           shwPaL2M;

}UCOM_NV_W_TX_PHASE_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PHASE_COMP_TIMING_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwTxPhaseCompTiming;
}UCOM_NV_W_TX_PHASE_COMP_TIMING_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_GAIN_FREQCOMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwTxGainFreqComp[UCOM_NV_W_TX_FREQ_COMP_NUM];     /* _H2ASN_Replace PHY_UINT16_ARRAY2_16 ashwHigh[W_BAND_NUM][W_UARFCN_NUM];*/ /* TX高增益频率补偿值 */
}UCOM_NV_W_TX_GAIN_FREQCOMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_GAIN_TEMP_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :7.4 PA处于高中低增益模式下，频段内高、中、低信道下，TX发射功率相对于温度变化的补偿
*****************************************************************************/
typedef struct
{
    /* 温度范围的取值取决于UCOM_NV_WG_TX_TEMP_DEFINE_STRU */
    PHY_INT16                           ashwTxGainTempComp[UCOM_NV_W_TX_PA_TEMP_NUM];
}UCOM_NV_W_TX_GAIN_TEMP_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_HSDPCCH_DELTA_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwTpcAlgo1UlPowGain;
    PHY_INT16                           shwTpcAlgo1HsDpaPowMode;
    PHY_INT16                           shwTpcAlgo2UlPowGain;
    PHY_INT16                           shwTpcAlgo2HsDpaPowMode;
}UCOM_NV_W_HSDPCCH_DELTA_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_VBIAS_TIMING_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwTxVbiasTiming;              /*表示PA VBIAS切换相对于时隙边界的提前量*/
}UCOM_NV_W_TX_VBIAS_TIMING_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_SAR_BACKOFF_POWER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwWcdmaSarBackoffPower[UCOM_NV_SAR_BACKOF_NUM];
}UCOM_NV_W_SAR_BACKOFF_POWER_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_RFIC_CTRL_GAIN_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  auhwApcRficCtrlGain[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_RFIC_CTRL_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_DBB_ATTEN_GAIN_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwApcDbbAttenGain[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_DBB_ATTEN_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_GAIN_STATE_INDEX_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  auhwRfGainStateIndexGain[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_GAIN_STATE_INDEX_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_RFIC_CTRL_GAIN_VOICE_MODE_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  auhwApcRficCtrlGainVoiceMode[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_RFIC_CTRL_GAIN_VOICE_MODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_DBB_ATTEN_GAIN_MODE_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_INT16                                   ashwApcDbbAttenGainVoiceMode[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_DBB_ATTEN_GAIN_VOICE_MODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APC_STATE_INDEX_GAIN_VOICE_MODE_STRU
 协议表格  :
 结构说明  : APC table表
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  auhwRfGainStateIndexGainVoiceMode[UCOM_NV_W_APC_ATTEN_NUM];

}UCOM_NV_W_APC_STATE_INDEX_GAIN_VOICE_MODE_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_APT_STRU
 协议表格  :
 结构说明  : APT发射功率的结构体类型
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwPower10th;                           /* 每个档位下APT发射功率 */

    PHY_UINT16                          uhwPaVcc;                               /* 每个档位下APT电压控制字 */
    PHY_UINT16                          uhwPaVbias;                             /* 每个档位下MmmbPaVbias电压控制字 */
    PHY_UINT16                          uhwPaVbias2;                            /* 每个档位下MmmbPaVbias2 */
    PHY_UINT16                          uhwPaVbias3;                            /* 每个档位下MmmbPaVbias3 */
}UCOM_NV_W_TX_PA_APT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_PA_APT_TABLE_STRU
 协议表格  :
 结构说明  : APT发射功率的结构体类型
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCount;

    UCOM_NV_W_TX_PA_APT_STRU            astAptTable[UCOM_NV_APT_MAX_NUM];

}UCOM_NV_W_TX_PA_APT_TABLE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_ANT_TUNER_BAND_PARA_STRU
 协议表格  :
 结构说明  : 天线调谐BAND控制结构体类型
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwAntTunerCode;                   /* 当前BAND天线调谐默认值 */
    PHY_UINT16                              uhwReserve;
    PHY_UINT16                              uhwAntTunerStart;                  /* 当前BAND驻波检测启动门限,BBP未提供接口，暂和PD打开门限一致 */
    PHY_INT16                               shwAntTunerComp;                   /* 反向功率补偿，精度0.1dbm */
}UCOM_NV_W_ANT_TUNER_BAND_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TUNER_MIPI_CMD_FRAME_STRU
 协议表格  :
 结构说明  : Tuner W模活动态时的命令帧，最多支持10个, uhwNormalNum表示当前型号
             tuner所需要配置的非扩展寄存器个数。uhwExtendedNum表示当前型号tuner
             所需要配置的扩展寄存器个数（扩展配置的寄存器肯定是连续的）。两者之
             和应小于等于10。非扩展配置的寄存器需要写完整的命令，扩展配置的寄存
             器只需要写地址既可。
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwNormalNum;
    PHY_UINT16                              uhwExtenedNum;
    PHY_UINT16                              auhwCmdFrame[UCOM_NV_W_TUNER_MIPI_CMD_MAX_NUM];
}UCOM_NV_W_TUNER_MIPI_CMD_FRAME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TUNER_MIPI_DATA_FRAME_STRU
 协议表格  :
 结构说明  : tuner默认配置数据，最多支持10个,对应CmdFrame,索引方式同CmdFrame
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              auhwDataFrame[UCOM_NV_W_TUNER_MIPI_DATA_MAX_NUM];
}UCOM_NV_W_TUNER_MIPI_DATA_FRAME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TUNER_MIPI_CMD_FRAME_STRU
 协议表格  :
 结构说明  : Tuner的命令帧，最多支持6个,uhwNormalNum表示当前型号
             tuner所需要配置的非扩展寄存器个数。uhwExtendedNum表示当前型号tuner
             所需要配置的扩展寄存器个数（扩展配置的寄存器肯定是连续的）。两者之
             和应小于等于10。非扩展配置的寄存器需要写完整的命令，扩展配置的寄存
             器只需要写地址既可。
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwNormalNum;
    PHY_UINT16                              uhwExtenedNum;
    PHY_UINT16                              auhwCmdFrame[UCOM_NV_GSM_TUNER_MIPI_CMD_MAX_NUM];
}UCOM_NV_GSM_TUNER_MIPI_CMD_FRAME_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TUNER_MIPI_DATA_FRAME_STRU
 协议表格  :
 结构说明  : tuner默认配置数据，最多支持6个,对应CmdFrame,索引方式同CmdFrame
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              auhwDataFrame[UCOM_NV_GSM_TUNER_MIPI_DATA_MAX_NUM];
}UCOM_NV_GSM_TUNER_MIPI_DATA_FRAME_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_TUNER_MIPI_DATA_GROUP_STRU
 协议表格  :
 枚举说明  : W配置数据,最多支持五种状态，每个状态支持10个控制字，对应CmdFrame,
             索引方式同CmdFrame
*****************************************************************************/
typedef struct
{
    UCOM_NV_W_TUNER_MIPI_DATA_FRAME_STRU         astDataFrame[UCOM_NV_TUNER_HANDLE_STATUS_NUM];
}UCOM_NV_W_TUNER_MIPI_DATA_GROUP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TUNER_MIPI_DATA_TX_STRU
 协议表格  :
 结构说明  : G模Tx默认配置数据,最多支持6个，有效个数由UCOM_NV_TUNER_CMD_ADDR
             uhwValidNum决定，各Band数据在实验室提取
*****************************************************************************/
typedef struct
{
    UCOM_NV_GSM_TUNER_MIPI_DATA_FRAME_STRU         astDataFrame[UCOM_NV_TUNER_HANDLE_STATUS_NUM];
}UCOM_NV_GSM_TUNER_MIPI_DATA_TX_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_TUNER_MIPI_DATA_RX_STRU
 协议表格  :
 结构说明  : G模Rx默认配置数据,最多支持6个，有效个数由UCOM_NV_TUNER_CMD_ADDR
             uhwValidNum决定，各Band数据在实验室提取
*****************************************************************************/
typedef struct
{
    UCOM_NV_GSM_TUNER_MIPI_DATA_FRAME_STRU         astDataFrame[UCOM_NV_TUNER_HANDLE_STATUS_NUM];
}UCOM_NV_GSM_TUNER_MIPI_DATA_RX_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_WG_MMMBPA_GAIN_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mmmb pa 寄存器控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          auhwCtrlWord[UCOM_NV_MAX_MMMBPA_CMD_NUMBER];
}UCOM_NV_WG_MMMBPA_GAIN_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_SCUPA_CM_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : SC-UPA的CM值补偿
*****************************************************************************/

typedef struct
{
    PHY_INT16                           ashwScUpaPdComp[UCOM_NV_W_TX_HDET_SCUPA_CM_COMP_NUM];
}UCOM_NV_W_TX_HDET_SCUPA_CM_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_HDET_DPA_CM_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DPA的CM值补偿
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwDpaPdComp[UCOM_NV_W_TX_HDET_DPA_CM_COMP_NUM];
}UCOM_NV_W_TX_HDET_DPA_CM_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_TX_APT_CM_SCUPA_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : SC-UPA业务的APT模式scupa补偿
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwAptCmScupaComp[UCOM_NV_W_TX_APT_SCUPA_CM_COMP_NUM];
}UCOM_NV_W_TX_APT_CM_SCUPA_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_APT_CM_DPA_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : SC-UPA业务的APT模式dpa补偿
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwAptCmDpaComp[UCOM_NV_W_TX_APT_DPA_CM_COMP_NUM];
}UCOM_NV_W_TX_APT_CM_DPA_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_APT_TEMP_COMP_STRU_HI6361
 协议表格  :
 ASN.1描述 :
 结构说明  : APT频率补偿
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwAptTempComp[UCOM_NV_W_APT_FREQ_COMP_NUM][UCOM_NV_W_APT_TEMP_COMP_NUM];                                /* 该值表示W的BAND或者G模的BAND标识 */
}UCOM_NV_W_APT_TEMP_COMP_STRU;

/*****************************************************************************
 枚举名    : UCOM_NV_W_APT_COMP_STRU
 协议表格  :
 枚举说明  : APT电压补偿的结构体类型
*****************************************************************************/
typedef struct
{
    PHY_INT16                           ashwAptComp[UCOM_NV_APT_MAX_NUM];       /* 每个档位下APT补偿 */
}UCOM_NV_W_APT_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_NONLINEAR_STRU_HI6361
 协议表格  :
 ASN.1描述 :
 结构说明  : PA非线性补偿
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwPower;
    PHY_INT16                           shwNonLinearComp;
}UCOM_NV_W_PA_NONLINEAR_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_PA_NONLINEAR_COMP_STRU_HI6361
 协议表格  :
 ASN.1描述 :
 结构说明  : PA非线性补偿
*****************************************************************************/
typedef struct
{
    PHY_UINT32                          uwValidCount;
    UCOM_NV_W_PA_NONLINEAR_STRU         astNonLinear[UCOM_NV_W_PA_NONLINEAR_NUM];
}UCOM_NV_W_PA_NONLINEAR_COMP_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_LOW_VOLT_SWITCH_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_INT16                           shwStartPower;                          /* 降压门限,默认16dBm,范围0~22dBm */
    PHY_INT16                           shwStopPower;                           /* 升压门限,默认18dBm,范围0~22dBm */
}UCOM_NV_W_TX_LOW_VOLT_SWITCH_THRESHOLD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_MAIL_BOX_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : W邮箱配置
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwWcdmaMailBoxFlag;                 /* W邮箱方案，1:邮箱方案有效；0:无效 */
    PHY_UINT16                          uhwReserved;                /* 保留字节 */
}UCOM_NV_W_MAIL_BOX_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_TX_LOW_VOLT_SWITCH_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/

typedef struct
{
    PHY_UINT16                          uhwSlaveSampleNum;                      /* 从模分集模式切换的采样点数,默认为1 */
    PHY_UINT16                          uhwDrxIntraSampleNum;                   /* DRX本频分集模式切换的采样点数,默认为1 */
    PHY_UINT16                          uhwDrxInterSampleNum;                   /* DRX异频分集模式切换的采样点数,默认为1 */
    PHY_UINT16                          uhwFachIntraSampleNum;                  /* FACH本频分集模式切换的采样点数,默认为2 */
    PHY_UINT16                          uhwFachInterSampleNum;                  /* FACH异频分集模式切换的采样点数,默认为1 */
    PHY_UINT16                          uhwDchIntraSampleNum;                   /* DCH本频分集模式切换的采样点数,默认为10 */
    PHY_UINT16                          uhwDchInterSampleNum;                   /* DCH异频分集模式切换的采样点数,默认为1 */
    PHY_INT16                           shwMain2ScndThreshold;                  /* 发射主集切发射分集的门限，TX_MAIN_RSCP - TX_SCND_RSCP <= 门限，切换到发射分集，-48(-6dB,1/8dB精度) */
    PHY_INT16                           shwScnd2MainThreshold;                  /* 发射主集切发射分集的门限，TX_MAIN_RSCP - TX_SCND_RSCP > 门限，切换到发射分集，-16(-2dB,1/8dB精度) */
    PHY_INT16                           shwMeasValidThreshold;                  /* 测量值有效门限，-960(-120dB,1/8dB精度) */
    PHY_INT16                           uhwMeasNonEqualThreshold;               /* 测量值有效门限，|TX_MAIN_RSCP - TX_SCND_RSCP| > 门限，说明两个发射天线能量不均等,24(3dB,1/8dB精度) */
    PHY_UINT16                          uhwEvaTxEnable;                         /* 评估发射分集使能的标记，1表示使能、0表示不使能 */
}UCOM_NV_W_TX_DIV_MODE_EVALUATE_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_W_MAIN_ANT_ESTIMATE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 主天线评估NV参数
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwRssiThreshold;                       /* 启动双天线测量的RSSI门限(单位为0.125dbm) */
    PHY_INT16                           shwRscpThreshold;                       /* 启动双天线测量的RSCP门限(单位为0.125dbm) */
    PHY_UINT16                          uhwEstimateTime;                        /* 评估时间(单位为1ms) */
}UCOM_NV_W_MAIN_ANT_ESTIMATE_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_DUAL_ANT_COMPARE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线比较状态NV参数
*****************************************************************************/
typedef struct
{
    PHY_INT16                           shwRscpThreshold[3];                    /* 各个阶段天线切换的RSCP门限(单位为0.125dbm) */
    PHY_UINT16                          uhwCompareTime[3];                      /* 各个阶段的双天线比较时间(单位为1ms) */
}UCOM_NV_W_DUAL_ANT_COMPARE_PARA_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_ANT_PROTECT_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 天线保护状态NV参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                          uhwFrozenTime;                          /* 进行主天线评估状态的冻结时间(单位为1ms) */
}UCOM_NV_W_ANT_PROTECT_PARA_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_W_DUAL_ANT_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 天线交换控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwWcdmaAntDefaultCtrlWord;          /* 天线不交换时的GPIO控制字 */
    PHY_UINT32                              uwWcdmaAntSwapCtrlWord;             /* 天线交换时的GPIO控制字 */
}UCOM_NV_W_DUAL_ANT_CTRL_WORD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_DUAL_ANT_MEAS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线测量的控制参量
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwMeasEnable;                       /* 测量使能 */
    PHY_UINT16                              uhwAlpha;                           /* alpha滤波因子 */
    UCOM_NV_W_DUAL_ANT_CTRL_WORD_STRU       stDualAntCtrlWord;                  /* 天线切换控制字 */
    UCOM_NV_W_MAIN_ANT_ESTIMATE_PARA_STRU   stAntEstimate;                      /* 主天线评估参量 */
    UCOM_NV_W_DUAL_ANT_COMPARE_PARA_STRU    stAntCompare;                       /* 双天线比较参量 */
    UCOM_NV_W_ANT_PROTECT_PARA_STRU         stAntProtect;                       /* 天线保护阶段参量 */
}UCOM_NV_W_DUAL_ANT_MEAS_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_GCF_INIT_GAIN_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线测量的控制参量
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwGsmRssiAgcSwitch;                /* GSM RSSI配置AGC初始增益开关 */
    PHY_UINT16                              uhwGsmRssiAgcGain;                  /* GSM RSSI AGC初始档位 */
}UCOM_NV_GSM_GCF_INIT_GAIN_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_VIRTUAL_BAND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM 虚拟频段使能标志和临界频点
*****************************************************************************/
typedef struct
{
    PHY_UINT16                           uhwGsmVirtualBand;                     /* 0-不支持虚拟频段方案; 1-旧的虚拟频段方案；2-新的虚拟频段方案 */
    PHY_UINT16                           uhwGsmFreqBound;                       /* B3L和B3H频点号分界点 */
}UCOM_NV_GSM_VIRTUAL_BAND_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_CTUNE_LUT_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM发射Ctune调试Lut参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                           uhwGsmTxCtuneLutCtrl;
}UCOM_NV_GSM_TX_CTUNE_LUT_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_EXT_STRONG_NCELL_THRESHOLD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM邻区进入六强邻区的电平门限
*****************************************************************************/
typedef struct
{
    PHY_INT16                            shwGsmExtStrongNcellThresh;            /* GSM邻区进入六强邻区的电平门限 */
}UCOM_NV_GSM_EXT_STRONG_NCELL_THRESHOLD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_TX_CTUNE_LUT_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : WCDMA发射Ctune调试Lut参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                           uhwWcdmaTxCtuneLutCtrl;
}UCOM_NV_WCDMA_TX_CTUNE_LUT_CTRL_STRU;
/*****************************************************************************
 结构名    : UCOM_NV_CHANNEL_WBBP_GAP_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RFIC新增NV项,WBBP Gap操作通道配置参数,对应WBBP接口说明书"跨通道测量"
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwT1ForIdlePch;                                         /* idlepch态T1延迟配置,单位chip */
    PHY_UINT16          uhwT2ForIdlePch;                                         /* idlepch态T2延迟配置,单位chip */
    PHY_UINT16          uhwT3ForIdlePch;                                         /* idlepch态T3延迟配置,单位chip */
    PHY_UINT16          uhwT4ForIdlePch;                                         /* idlepch态T4延迟配置,单位chip */
    PHY_UINT16          uhwT1ForFach;                                            /* FACH态T1延迟配置,单位chip */
    PHY_UINT16          uhwT2ForFach;                                            /* FACH态T2延迟配置,单位chip */
    PHY_UINT16          uhwT3ForFach;                                            /* FACH态T3延迟配置,单位chip */
    PHY_UINT16          uhwT4ForFach;                                            /* FACH态T4延迟配置,单位chip */
    PHY_UINT16          uhwT1ForDch;                                             /* DCH态T1延迟配置,单位chip */
    PHY_UINT16          uhwT2ForDch;                                             /* DCH态T2延迟配置,单位chip */
    PHY_UINT16          uhwT3ForDch;                                             /* DCH态T3延迟配置,单位chip */
    PHY_UINT16          uhwT4ForDch;                                             /* DCH态T4延迟配置,单位chip */

}UCOM_NV_CHANNEL_WBBP_GAP_CONFIG_STRU;



/*****************************************************************************
 结构名    : UCOM_NV_WCDMA_CHANNEL_SWITCH_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RFIC新增NV项,通道交叉配置
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwRfSignalSwitch;                                      /* RF信号交叉 */
    PHY_UINT16          uhwMipiSignalSwitch;                                    /* MIPI信号交叉 */
    PHY_UINT16          uhwAbbTxSignalSwitch;                                   /* ABB TX信号交叉 */
    PHY_UINT16          uhwAbbRxSignalSwitch;                                   /* ABB RX信号交叉 */
    PHY_UINT16          uhwPmuSignalSwitch;                                     /* PMU信号交叉 */
    PHY_UINT16          uhwAptSignalSwitch;                                     /* APT信号交叉 */
}UCOM_NV_CHANNEL_SWITCH_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_CHANNEL_UNBIND_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RFIC新增NV项,通道BAND不绑定配置
*****************************************************************************/
typedef struct
{
    PHY_UINT16          uhwMipiUnbindEn;                                         /* MIPI非绑定使能 */
    PHY_UINT16          uhwMipiCh0Select;                                        /* MIPI通道0选择 */
    PHY_UINT16          uhwMipiCh1Select;                                        /* MIPI通道1选择 */
    PHY_UINT16          uhwAfcCh0Select;                                         /* AFC通道0选择 */
    PHY_UINT16          uhwAfcCh1Select;                                         /* AFC通道1选择 */
    PHY_UINT16          uhwAptCh0Select;                                         /* APT通道0选择 */
    PHY_UINT16          uhwAptCh1Select;                                         /* APT通道1选择 */
    PHY_UINT16          uhwPmuCh0Select;                                         /* PMU通道0选择 */
    PHY_UINT16          uhwPmuCh1Select;                                         /* PMU通道1选择 */
    PHY_UINT16          uhwAbbTxCh0Select;                                       /* ABB TX通道0选择,6363使用 */
    PHY_UINT16          uhwAbbTxCh1Select;                                       /* ABB TX通道1选择 6363使用*/

}UCOM_NV_CHANNEL_UNBIND_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_MIPI_NOTCH_ADDRESS_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mipi NOTCH 寄存器地址
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwNotchMipiNum;                                     /* Nothc Mipi命令字个数，uhwNotchControlMode=2或者3时有效 */
    PHY_UINT16                              auhwNotchMipiCmd[UCOM_NV_MAX_MIPI_ANT_CMD_NUMBER];   /* Nothc MipiCmd，uhwNotchControlMode=2或者3时有效 */
}UCOM_NV_GSM_MIPI_NOTCH_ADDRESS_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_MIPI_NOTCH_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : mipi NOTCH 寄存器控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              auhwNotchMipiData[UCOM_NV_MAX_MIPI_ANT_CMD_NUMBER];     /* Nothc MipiData，uhwNotchControlMode=2或者3时有效 */
}UCOM_NV_GSM_MIPI_NOTCH_CTRL_WORD_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_NOTCH_POWER_COMP_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :  NOTCH POWER COMP
*****************************************************************************/
typedef struct
{
    PHY_INT16                               shwNotchPowerThreshold;             /* Notch发射功率大于该值时不进行发射功率补偿 */
    PHY_INT16                               shwNotchPowerComp;                  /* Notch发射功率补偿值 */
}UCOM_NV_GSM_NOTCH_POWER_COMP_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_NOTCH_CHAN_CTRL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 天线开关滤波器通道的控制
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwNotchSupportFlag;                /* DCS1800 TX NOTCH开关支持标志，1表示支持，0表示不支持 */
    PHY_UINT16                              uhwNotchControlMode;                /* NOTCH控制方式:1表示GPIO模式，2表示Mipi方式，3表示GPIO+MIPI组合方式 */
    PHY_UINT32                              uwNotchGpioValue;                   /* Nothc GPIO控制值，uhwNotchControlMode=1或者3时有效 */
    PHY_UINT16                              uhwReserved;
    UCOM_NV_GSM_MIPI_NOTCH_ADDRESS_STRU     stNotchMipiCmd;                     /* 天线NOTCH通道MIPI接口控制地址 */
    UCOM_NV_GSM_MIPI_NOTCH_CTRL_WORD_STRU   stNotchMipiCtrlWord;                /* 天线NOTCH通道MIPI接口控制字 */
    UCOM_NV_GSM_NOTCH_POWER_COMP_STRU       stNotchPowerComp;                   /* Notch发射功率补偿 */
}UCOM_NV_GSM_NOTCH_CHAN_CTRL_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DFS新增NV项,动态调频参数,只修改合入V7R2和V9R1
*****************************************************************************/
typedef struct
{
    PHY_UINT32          uwDdrFreq;                                        /* DDR频率配置(单位KHz)  */
    PHY_UINT32          uwCcpuFreq;                                       /* CCPU频率配置(单位KHz) */
    PHY_UINT32          uwBusFreq;                                        /* 总线频率配置(单位KHz) */

}UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_MODEM_DFS_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DFS新增NV项,只修改合入V7R2和V9R1
*****************************************************************************/
typedef struct
{
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stCsCfg;                                          /* 语音模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stDrxCfg;                                         /* 待机模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stFachCfg;                                        /* FACH态下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stHspaCfg;                                        /* HSPA数据业务模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stR99PsCfg;                                       /* R99数据业务模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stSlaveWorkCfg;                                   /* 从模工作状态下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stSlaveSleepCfg;                                  /* 从模睡眠状态下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stMinCfg;                                         /* 最小的Profile设置值 */

}UCOM_NV_W_MODEM_DFS_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_W_CS_INTRA_STEP23_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 本频三步法小区搜索参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwCsStep23SlotNum;                 /* DRX态本频三步法小区搜索step23的时隙头个数，初始值为1 */
    PHY_UINT16                              uhwCsStep23Period;                  /* DRX态本频三步法小区搜索周期，初始值为3072帧，即30.72s */
}UCOM_NV_W_CS_INTRA_STEP23_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_QPC_REGDEFAULT_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : QPC NV项
*****************************************************************************/
typedef struct
{
    PHY_UINT16                      uhwDefaultCloseTime;                 /* 默认关闭时间     默认80(单位 帧)，0代表QPC未使能 */
    PHY_UINT16                      uhwDefaultGuardTime;                 /* 默认保护开启时长 默认20(单位 帧) */
    PHY_UINT16                      uhwDefaultOpenTime;                 /* 默认数据触发时长 默认40(单位 帧) */

    PHY_UINT16                      uhwReserve;
}UCOM_NV_QPC_REGDEFAULT_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_G_MODEM_DFS_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DFS新增NV项,只修改合入V7R2和V9R1
*****************************************************************************/
typedef struct
{
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stCsCfg;                                          /* 语音业务模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stPsCfg;                                          /* 数据业务模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stDrxCfg;                                         /* 待机模式下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stTuningCfg;                                      /* tuning dch状态下Profile设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stSearchingCfg;                                   /* 搜网模式下设置值 */
    UCOM_NV_SYS_MODEM_DFS_CONFIG_STRU          stMinCfg;                                         /* 最小的Profile设置值 */

}UCOM_NV_G_MODEM_DFS_CONFIG_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_RF_INTERFERE_CONTROL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 控制TDS/LTE的天线开关,目的是避免B39和B3H的干扰
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwFuncOn;                      /* 0-不使能，1-使能 */
    PHY_UINT32                                  uwGpioMask;                     /* 线控清除 */
    PHY_UINT32                                  uwGpioRxValue;                  /* 关闭TDS/LTE发射的线控 */
    PHY_UINT32                                  uwGpioDefaultValue;             /* TDS/LTE发射的线控默认值 */
}UCOM_NV_GSM_RF_INTERFERE_CONTROL_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_PHASE_ERROR_STANDARD_MODE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : CMU使用standard模式测试gmsk phase error时，需要配置次NV项为1。
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwMode;                        /* CMU使用standard模式测试gmsk phase error时，需要配置次NV项为1 */
}UCOM_NV_GSM_PHASE_ERROR_STANDARD_MODE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_TX_ANT_SWITCH_GPIO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : TX ANT SWITCH（扩展），与rf band config一起使用，在天线开关配值的时刻生效。
*****************************************************************************/
typedef struct
{
    PHY_UINT32                                  uwGpio;                         /* 对应前端线控 */
    PHY_UINT32                                  uwResvered;
}UCOM_NV_GSM_TX_ANT_SWITCH_GPIO_STRU;


/*****************************************************************************
 结构名    : UCOM_NV_GSM_DUAL_ANT_CTR_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : GSM双天线切换GPIO控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT32                           uwGsmDualAntClearMask;                 /* 天线控制清除掩码 */
    PHY_UINT32                           uwGsmDualAntPriCtrlWord;               /* 主天线的GPIO控制字 */
    PHY_UINT32                           uwGsmDualAntSecCtrlWord;               /* 辅天线的GPIO控制字 */
}UCOM_NV_GSM_DUAL_ANT_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_PRI_ANT_ESTIMATE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线切换主天线能量评估阶段参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwGsmAntEstimatePeriodNum;         /* 统计周期个数 */
    PHY_INT16                               shwGsmAntEstimatePowerCmpThreshold; /* 能量判决门限 */
}UCOM_NV_GSM_PRI_ANT_ESTIMATE_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_DUAL_ANT_COMPARE_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线切换主辅天线能量比较阶段参数
*****************************************************************************/
typedef struct
{
    PHY_UINT32                              uwGsmAntCompareCollectTime;         /* 采集时间 */
    PHY_UINT16                              uhwGsmAntCompareFirstPeriodNum;     /* 状态1的统计周期个数 */
    PHY_UINT16                              uhwGsmAntCompareSecondPeriodNum;    /* 状态2的统计周期个数 */
    PHY_UINT16                              uhwGsmAntCompareThirdPeriodNum;     /* 状态3的统计周期个数 */
    PHY_INT16                               shwGsmAntCompareFirstPowerCmpThreshold;  /* 状态1的能量判决门限 */
    PHY_INT16                               shwGsmAntCompareSecondPowerCmpThreshold; /* 状态2的能量判决门限 */
    PHY_INT16                               shwGsmAntCompareThirdPowerCmpThreshold;  /* 状态3的能量判决门限 */
}UCOM_NV_GSM_DUAL_ANT_COMPARE_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_DUAL_ANT_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 双天线切换参数
*****************************************************************************/
typedef struct
{
    PHY_UINT16                              uhwGsmDualAntEn;                    /* 双天线切换使能标志 */
    PHY_UINT16                              uhwGsmDualAntAlphaPara;             /* 双天线切换平滑滤波系数 */
    UCOM_NV_GSM_PRI_ANT_ESTIMATE_PARA_STRU  stGsmPriAntEstimatePara;            /* 双天线切换主天线能量评估阶段参数 */
    UCOM_NV_GSM_DUAL_ANT_COMPARE_PARA_STRU  stGsmDualAntComparePara;            /* 双天线切换主辅天线能量比较阶段参数 */
    PHY_UINT32                              uwGsmAntHandleDelayPara;            /* 双天线切换后的迟滞参数 */
    UCOM_NV_GSM_DUAL_ANT_CTRL_WORD_STRU     stGsmDualAntCtrlWord;               /* 双天线切换GPIO控制字 */
}UCOM_NV_GSM_DUAL_ANT_PARA_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_ANT_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 共天线使能标志
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwAntShareEn;                  /* 0-不支持; 1-支持 */
}UCOM_NV_ANT_SHARE_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_CLOSE_ANT_MIPI_CTRL_WORD_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 关闭天线Mipi控制字
*****************************************************************************/
typedef struct
{
    PHY_UINT16                                  uhwCloseAntEn;                  /* 关闭天线使能 */
    UCOM_NV_WG_MIPI_ANT_CMD_ADDRESS_STRU        stCloseAntMipiAddress;          /* Mipi控制地址 */
    UCOM_NV_WG_MIPI_ANT_CTRL_WORD_STRU          stCloseAntMipiCtrlWord;         /* Mipi控制字 */
}UCOM_NV_GSM_CLOSE_ANT_MIPI_CTRL_WORD_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_CLOSE_ANT_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 关闭天线开关的线控配置和MIPI配置
*****************************************************************************/
typedef struct
{
    UCOM_NV_RF_CONFIG_STRU                      stGsmCloseAntRfConfig;          /* 关闭天线线控配置 */
    UCOM_NV_GSM_CLOSE_ANT_MIPI_CTRL_WORD_STRU   stGsmCloseAntMipiCtrlWord;      /* 关闭天线Mipi控制字 */
}UCOM_NV_GSM_CLOSE_ANT_CONFIG_STRU;

/*****************************************************************************
 结构名    : UCOM_NV_GSM_VIRTUAL_BAND_ANT_CONFIG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 虚拟频段的天线开关配置
*****************************************************************************/
typedef struct
{
    UCOM_NV_RF_CONFIG_STRU                      stGsmVirtualBandRfConfig;       /* 虚拟频段的天线开关配置 */
}UCOM_NV_GSM_VIRTUAL_BAND_ANT_CONFIG_STRU;

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


#endif /* end of PhyNvDefine.h */


