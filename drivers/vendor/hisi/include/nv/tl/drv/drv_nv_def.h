/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       :   drv_nv_def.h
  Description     :   DRV组件的NV 结构体定义
  History         :
******************************************************************************/

#ifndef __DRV_NV_DEF_H__
#define __DRV_NV_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




#include "drv_comm.h"

#define LDO_GPIO_MAX 2


/*******************************************************************/

/*****************************************************************************
 结构名    : nv_protocol_base_type
 结构说明  : nv_protocol_base_type结构 ID= en_NV_Item_Modem_Log_Path 148
 			MBB形态 modem log路径名，用于区分新老形态的modem log路径。
*****************************************************************************/
typedef struct
{
   BSP_CHAR_TL  ucModemLogPath[32];
   BSP_CHAR_TL  modemLogRsv[4];
}NV_MODEM_LOG_PATH;



/*NV ID = 0xd109*/
/*值为1则为打开，值为0则为关闭*/
typedef struct
{
	BSP_S32 buck2_switch;    /*[0, 1]*/
}NV_PASTAR_BUCK2_SWITCH_STRU;
/*end NV ID = 0xd109*/

/*NV ID  = 0xd10b*/

typedef struct ST_PWC_SWITCH_STRU_S {

	/*以下NV用于低功耗的整体控制，其中有些BIT暂时未用，做他用时，请更正为准确的名称*/
 	BSP_U32 deepsleep  :1; /*bit0*/
    BSP_U32 lightsleep :1; /*bit1*/
    BSP_U32 dfs        :1; /*bit2*/
    BSP_U32 hifi       :1; /*bit3*/
    BSP_U32 drxAbb     :1; /*bit4*/
    BSP_U32 drxZspCore :1; /*bit5*/
    BSP_U32 drxZspPll  :1; /*bit6*/
    BSP_U32 drxWLBbpPll  :1; /*bit7*/
    BSP_U32 drxGBbpPll   :1; /*bit8*/
    BSP_U32 drxRf      :1; /*bit9*/
    BSP_U32 drxPa      :1; /*bit10*/
    BSP_U32 drxGuBbpPd   :1; /*bit11*/
    BSP_U32 drxDspPd     :1; /*bit12*/
    BSP_U32 drxLBbpPd    :1; /*bit13*/
    BSP_U32 drxPmuEco    :1; /*bit14*/
    BSP_U32 drxPeriPd    :1; /*bit15*/
    BSP_U32 l2cache_mntn  :1; /*bit16*/
    BSP_U32 bugChk     :1; /*bit17*/
    BSP_U32 pmuSwitch     :1; /*bit18*/
    BSP_U32 drxLdsp      :1;  /*bit 19*/
    BSP_U32 matserTDSpd  :1; /*bit20*/
    BSP_U32 tdsClk    :1;  /*bit21*/
    BSP_U32 slaveTDSpd   :1; /*bit22*/
    BSP_U32 slow	     :1;/*bit23*/
    BSP_U32 reserved    :8; /*bit24-31*/

	/*以下NV用于DEBUG上下电和开关钟*/
	BSP_U32 drx_pa0_pd       :1; /*bit0 用于控制PA0的上下电*/
    BSP_U32 drx_pa1_pd       :1; /*bit1 用于控制PA1的上下电*/
    BSP_U32 drx_rfic0_pd     :1; /*bit2 用于控制RFIC0的上下电*/
    BSP_U32 drx_rfic1_pd     :1; /*bit3 用于控制RFIC1的上下电*/
    BSP_U32 drx_irm_pd       :1; /*bit4 用于控制BBP_IRM的上下电*/
    BSP_U32 drx_bbe16_pd     :1; /*bit5 用于控制BBE16的上下电*/
    BSP_U32 drx_abb_pd       :1; /*bit6 用于控制ABB的上下电*/
    BSP_U32 drx_g1bbp_pd     :1; /*bit7 用于控制G1_BBP的上下电*/
    BSP_U32 drx_g2bbp_pd     :1; /*bit8 用于控制G2_BBP的上下电*/
    BSP_U32 drx_wbbp_pd      :1; /*bit9 用于控制WBBP的上下电*/
    BSP_U32 drx_bbpcommon_pd :1; /*bit10 用于控制BBP_COMMON的上下电*/
    BSP_U32 drx_twbbp_pd     :1; /*bit11 用于控制TWBBP的上下电*/
    BSP_U32 drx_bbe16_pll    :1; /*bit12 用于控制BBE16_PLL的开关钟*/
    BSP_U32 drx_bbp_pll      :1; /*bit13 用于控制BBP_PLL的开关钟*/
    BSP_U32 drx_abb0_wpll    :1; /*bit14 用于控制ABB_CH0_WPLL的开关钟*/
    BSP_U32 drx_abb0_gpll    :1; /*bit15 用于控制ABB_CH0_GPLL的开关钟*/
    BSP_U32 drx_abb1_wpll    :1; /*bit16 用于控制ABB_CH1_WPLL的开关钟*/
    BSP_U32 drx_abb1_gpll    :1; /*bit17 用于控制ABB_CH1_GPLL的开关钟*/
    BSP_U32 reserved2        :14; /*bit18-31 未用*/
}ST_PWC_SWITCH_STRU;


/*NV ID = 0xd10c*/
typedef struct ST_PWC_DFS_STRU_S {
    BSP_U32 CcpuUpLimit;
	BSP_U32 CcpuDownLimit;
	BSP_U32 CcpuUpNum;
	BSP_U32 CcpuDownNum;
	BSP_U32 AcpuUpLimit;
	BSP_U32 AcpuDownLimit;
	BSP_U32 AcpuUpNum;
	BSP_U32 AcpuDownNum;
	BSP_U32 DFSTimerLen;
	BSP_U32 DFSHifiLoad;
 	BSP_U32 Strategy;/*使用何种策略bit0:1->200ms负载检测,bit0:0->4s负载检测;bit1:1/0打开/关闭辅助DDR调频*/
 	BSP_U32 DFSDdrUpLimit;
 	BSP_U32 DFSDdrDownLimit;
 	BSP_U32 DFSDdrprofile;
 	BSP_U32 reserved;
}ST_PWC_DFS_STRU;

/*NV ID = 0xd10f begin，配置PMU出现异常时的处理配置*/
typedef struct
{
    BSP_U8  VoltId;         /*需要特殊配置的电压源的id号*/
    BSP_U8  VoltOcpIsOff;   /*若该电压源过流的话是否关闭该路电压源*/
    BSP_U8  VoltOcpIsRst;   /*若该电压源过流的话是否重启系统*/
    BSP_U8  VoltOtpIsOff;   /*若PMU过热需要关闭非核心电源的话，是否能关闭该路电压源*/
} PMU_VOLT_PRO_STRU;
typedef struct
{
    BSP_U8    ulOcpIsOn;        /*过流的电源能否被重新打开:0:不能被重新打开；1:能被重新打开，默认为不能打开--0*/
    BSP_U8    ulOcpIsOff;       /*过流的电源是否关闭:0:不关闭；1:关闭，默认为关闭--1*/
    BSP_U8    ulOtpCurIsOff;    /*过温(超过温度预警值)时是否关闭非核心电源:0:不关闭非核心，1:关闭非核心电源。默认为关闭非核心电源--1*/
    BSP_U8    ulOtpIsRst;       /*过温(超过温度预警值)时是否重启系统:0:不重启，1:重启系统。默认为不重启系统--0*/

    BSP_U8    ulOtpIsOff;       /*PMU芯片结温超过150℃时PMU是否下电(测试时可配置):0:PMU不下电，1:PMU下电。默认为PMU下电--1*/
    BSP_U8    ulUvpIsRst;       /*欠压预警时是否重启系统，默认为0:不重启*/
    BSP_U16   reserved2;

    BSP_U16   ulOtpLimit;       /*温度预警阈值设置:105:105℃，115:115℃，125:125℃ ，135:135℃ (HI6559增加支持135)。默认为125℃--125 */
    BSP_U16   ulUvpLimit;       /*欠压预警阈值设置，单位mv:3000:3v; 2700:2.7v(2850:2.85v.HI6559支持2.85,HI6551支持2.7) ，默认为3v--3000*/

    PMU_VOLT_PRO_STRU VoltProConfig[50];/*每路电压源的异常保护策略配置，不同产品形态需要特殊配置的电压源配置*/
} PMU_EXC_PRO_NV_STRU;

/*NV ID = 0xd10f end*/

/*ID=0xd110*/
typedef struct
{
    BSP_U32    u32CalcTime;        /* 计算时间周期(10ms) */
    BSP_U32    u32PktNum;          /* 累计包个数 */
    BSP_U32    u32SwichFlag;       /* netif包上送适配开关 */
} NETIF_INIT_PARM_T;

/*ID=0xd111 begin */
typedef struct
{
    BSP_U32 dump_switch    : 2; /* 00: excdump, 01: usbdump, 1x: no dump */
    BSP_U32 ARMexc         : 1; /* 2 ARM异常检测开关*/
    BSP_U32 stackFlow      : 1; /* 3 堆栈溢出检测开关*/
    BSP_U32 taskSwitch     : 1; /* 4 任务切换记录开关*/
    BSP_U32 intSwitch      : 1; /* 5 中断记录开关*/
    BSP_U32 intLock        : 1; /* 6 锁中断记录开关*/
    BSP_U32 appRegSave1    : 1; /* 7 寄存器组1记录开关 */
    BSP_U32 appRegSave2    : 1; /* 8 寄存器组2记录开关*/
    BSP_U32 appRegSave3    : 1; /* 9 寄存器组3记录开关*/
    BSP_U32 commRegSave1   : 1; /* 10 寄存器组1记录开关 */
    BSP_U32 commRegSave2   : 1; /* 11 寄存器组2记录开关*/
    BSP_U32 commRegSave3   : 1; /* 12 寄存器组3记录开关*/
    BSP_U32 sysErrReboot   : 1; /* 13 systemError复位开关*/
    BSP_U32 reset_log      : 1; /* 14 强制记录开关，暂未使用*/
    BSP_U32 fetal_err      : 1; /* 15 强制记录开关，暂未使用*/
	BSP_U32 log_ctrl       : 2; /* bsp_trsce 输出控制*/
    BSP_U32 reserved1      : 14;
} DUMP_CFG_STRU;

typedef struct
{
    union
    {
        BSP_U32         uintValue;
        DUMP_CFG_STRU   cfg;
    } dumpCfg;

    BSP_U32 appRegAddr1;	/* ACORE保存寄存器组地址1*/
    BSP_U32 appRegSize1;	/* ACORE保存寄存器组长度1*/
    BSP_U32 appRegAddr2;	/* ACORE保存寄存器组地址2*/
    BSP_U32 appRegSize2;	/* ACORE保存寄存器组长度2*/
    BSP_U32 appRegAddr3;	/* ACORE保存寄存器组地址3*/
    BSP_U32 appRegSize3;	/* ACORE保存寄存器组长度3*/

    BSP_U32 commRegAddr1;	/* CCORE保存寄存器组地址1*/
    BSP_U32 commRegSize1;	/* CCORE保存寄存器组长度1*/
    BSP_U32 commRegAddr2;	/* CCORE保存寄存器组地址2*/
    BSP_U32 commRegSize2;	/* CCORE保存寄存器组长度2*/
    BSP_U32 commRegAddr3;	/* CCORE保存寄存器组地址3*/
    BSP_U32 commRegSize3;	/* CCORE保存寄存器组长度3*/

    BSP_U32 traceOnstartFlag;           /* 0:开机启动Trace, 非0:开机不启动Trace */
    BSP_U32 traceCoreSet;               /* 0:采集A核Trace, 1:采集C核Trace, 2:采集双核Trace */
    BSP_U32 BusErrorFlagSet;             /* 0:开机不启动防总线挂死功能, 非0:开机启动防总线挂死功能 */
} NV_DUMP_STRU;
/*ID=0xd111 end */

/*NV ID = 0xd114 begin,配置PMU初始化时应该由软件实现的基础配置*/
#define NUM_OF_PMU_NV  50
typedef struct
{
    BSP_U8  VoltId;     /*电源id号*/
    BSP_U8  IsNeedSet;  /*是否需要软件设置:0:不需要；1:需要*/
    BSP_U8  IsOnSet;    /*默认是否需要开启电压源:0:不需要；1:需要*/
    BSP_U8  IsOffSet;   /*默认是否需要关闭电压源:0:不需要；1:需要*/

    BSP_U8  IsVoltSet;  /*是否需要设置电压:0:不需要；1:需要*/
    BSP_U8  IsEcoSet;   /*是否需要设置ECO模式:0:不需要；1:需要*/
    BSP_U8  EcoMod;     /*需要设置的eco模式:0:normal;2:force_eco;3:follow_eco*/
    BSP_U8  reserved3;  /*默认*/

    BSP_U32 Voltage;    /*需要设置的默认电压*/
} PMU_INIT_CON_STRU;
typedef struct
{
    PMU_INIT_CON_STRU InitConfig[NUM_OF_PMU_NV];
} PMU_INIT_NV_STRU;
/*NV ID = 0xd114 end*/

/*NV ID = 0xd115 start*/

typedef struct {
    BSP_U32	index;           /*硬件版本号数值(大版本号1+大版本号2)，区分不同产品*/
    BSP_U32	hwIdSub;        /*硬件子版本号，区分产品的不同的版本*/
	BSP_CHAR_TL  name[32];           /*内部产品名*/
    BSP_CHAR_TL	namePlus[32];       /*内部产品名PLUS*/
    BSP_CHAR_TL	hwVer[32];          /*硬件版本名称*/
    BSP_CHAR_TL	dloadId[32];        /*升级中使用的名称*/
    BSP_CHAR_TL	productId[32];      /*外部产品名*/
}PRODUCT_INFO_NV_STRU;

/*NV ID =0xd115 end*/

/*NV ID =0xd116 start,mipi0_chn*/
typedef struct{
	BSP_U32 mipi_chn;
}MIPI0_CHN_STRU;
/*NV ID =0xd116 end,mipi0_chn*/

/*NV ID =0xd117 start,mipi1_chn*/
typedef struct{
	BSP_U32 mipi_chn;
}MIPI1_CHN_STRU;
/*NV ID =0xd117 end,mipi1_chn*/

/*NV ID =0xd12e start, rf power control, pastar config*/
typedef struct{
	BSP_U32 rfpower_m0;/*[0, 1,2]*//*modem0,物理通道0的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示LDO供电*/
	BSP_U32 rfpower_m1;/*[0, 1,2]*//*modem1,物理通道1的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示LDO供电*/
}NV_RFPOWER_UNIT_STRU;
/*NV ID =0xd12e start, pastar config*/

/* NV ID =0xd13A start, pa power control, pastar config */
typedef struct{
	BSP_U32 papower_m0;/*[0,1,2]*//*modem0,物理通道0的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示电池供电*/
	BSP_U32 papower_m1;/*[0,1,2]*//*modem1,物理通道1的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示电池供电*/
}NV_PAPOWER_UNIT_STRU;
/* NV ID =0xd13A start, pastar config */

typedef struct
{
    BSP_U32   nvSysTimeValue;   /* 获得单板系统运行时间 */
}SYS_TIME;

typedef struct
{
    BSP_U32  ulIsEnable;				/*温度保护使能*/
    BSP_U32    lCloseAdcThreshold;
    BSP_U32  ulTempOverCount;
}CHG_BATTERY_HIGH_TEMP_PROT_NV;

typedef struct
{
    BSP_U32  ulIsEnable;			/*温度保护使能*/
    BSP_U32    lCloseAdcThreshold;
    BSP_U32  ulTempLowCount;
}CHG_BATTERY_LOW_TEMP_PROTE_NV;

/* 工厂模式nv项,0 : 非工厂模式 1: 工厂模式**/
typedef struct
{
    BSP_U32 ulFactoryMode;
}FACTORY_MODE_TYPE;

/* 开机启动尝试次数 */
typedef struct
{
    BSP_U32 ulTryTimes;
}BOOT_TRY_TIMES_STRU;

/* 开机按键按下时间 */
typedef struct
{
    BSP_U32 ulPowKeyTime;
}POWER_KEY_TIME_STRU;

typedef struct
{
    BSP_U16 temperature;
    BSP_U16 voltage;
}CHG_TEMP_ADC_TYPE;

typedef struct
{
    CHG_TEMP_ADC_TYPE g_adc_batt_therm_map[31];
}NV_BATTERY_TEMP_ADC;


/*硬测版本nv项,1 : 硬测版本 0: 非硬测版本*/
typedef struct
{
    BSP_U32 ulHwVer;
}E5_HW_TEST_TYPE;

/*是否支持APT功能nv项,1 : 支持 0: 不支持*/
typedef struct
{
    BSP_U32 ulIsSupportApt;
}NV_SUPPORT_APT_TYPE;

/*PMU异常保护nv项*/
typedef struct
{
    BSP_U8 TemppmuLimit;  /*PMU温度阈值:0:105℃;1:115℃;2:125℃;3:135℃*/
	BSP_U8 ulCurIsOff;  /*过热时过流源是否下电：0：下电；1：不下电*/
	BSP_U8 ulOcpIsRst;  /*过热时是否进行软复位: 0:不软复位 1:软复位*/
	BSP_U8 PmuproIsOn;  /*PMU异常保护处理是否开启：0：不开启；1：开启*/
}PMU_PRO_NV;

typedef struct
{
    BSP_U32 u32SciGcfStubFlag;   /* 1: GCF测试使能；0：GCF测试不使能 */
}SCI_NV_GCF_STUB_FLAG;

/*快速开关机功能是否使能NV项*/
typedef struct
{
    BSP_U32 ulEnable;   /*快速开关机是否使能：0：不使能；1：使能*/
}NV_SHORT_ONOFF_ENABLE_TYPE;

/*快速开关机功能配置信息NV项*/
typedef struct NV_SHORT_ONOFF_TYPE_S
{
    BSP_U32 ulPowerOffMaxTimes;   /*支持的最大假关机次数*/
	BSP_U32 ulMaxTime;            /*假关机历史累加时间大于等于此时间自动真关机，单位小时*/
	BSP_U32 ulVoltLevel1;         /*过放保护第一档电压门限*/
	BSP_U32 ulVoltLevel2;         /*过放保护第二档电压门限*/
	BSP_U32 ulRTCLevel1;          /*小于第一档电压对应的RTC唤醒时间*/
	BSP_U32 ulRTCLevel2;          /*第一档和第二档之间电压对应的RTC唤醒时间*/
	BSP_U32 ulRTCLevel3;          /*大于等于第二档电压对应的RTC唤醒时间*/
}NV_SHORT_ONOFF_TYPE;

/*省电模式配置，标识各外设是否使能NV项*/
typedef struct
{
        BSP_U32 ulLEDEnable;   /*呼吸灯LED 是否使能：0：不使能；1：使能*/
        BSP_U32 ulReserved1;   /*此项预留  是否使能：0：不使能；1：使能*/
        BSP_U32 ulReserved2;   /*此项预留  是否使能：0：不使能；1：使能*/
}NV_POWER_SAVE_TYPE;

/*库仑计电压，电流校准参数*/
typedef struct
{
    BSP_U32 v_offset_a;         /* 电压校准线性参数 */
    BSP_S32 v_offset_b;         /* 电压校准线性偏移*/
    BSP_U32 c_offset_a;         /* 电流校准线性参数 */
    BSP_S32 c_offset_b;         /* 电流校准线性偏移 */
}COUL_CALI_NV_TYPE;

/*温度保护HKADC各个通道的配置 NV_ID_DRV_TEMP_HKADC_CONFIG            = 0xd120 */

typedef struct
{
    BSP_U32 out_config;         /* bit0-bit1 0:不输出 1:单次输出 2:循环输出 */
                                /* bit2 1:唤醒输出 0:非唤醒输出 */
                                /* bit3: 0:只输出电压 1:电压温度均输出 */
                                /* bit8: A核输出 */
                                /* bit9: C核输出 */
    BSP_U32 have_config;
    BSP_U16 out_period;         /* 循环输出时的循环周期，单位:秒 */
    BSP_U16 convert_list_len;   /* 温度转换表长度 */
    BSP_U16 convert_list_id;    /* 温度转换表NV标志，实际长度参见usTempDataLen */
    BSP_U16 reserved;
}TEMP_HKADC_CHAN_CONFIG;

typedef struct
{
    TEMP_HKADC_CHAN_CONFIG chan_config[16];

}TEMP_HKADC_CHAN_CONFIG_ARRAY;

/*温度保护HKADC各个通道的配置 NV_ID_DRV_TEMP_TSENS_CONFIG            = 0xd121 */
typedef struct
{
    BSP_U16 enable;         /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    BSP_U16 high_thres;     /* 芯片高温保护的电压阀值  */
    BSP_U16 high_count;     /* 芯片高温保护次数上限，系统关机 */
    BSP_U16 reserved;
    /*BSP_U32 low_thres;*/  /* 芯片低温保护的电压阀值 */
    /*BSP_U32 low_count;*/  /* 芯片低温保护次数上限，系统关机 */

}TEMP_TSENS_REGION_CONFIG;

typedef struct
{
    TEMP_TSENS_REGION_CONFIG region_config[3];

}TEMP_TSENS_REGION_CONFIG_ARRAY;



/*温度保护电池高低温保护  NV_ID_DRV_TEMP_BATTERY_CONFIG          = 0xd122 */
typedef struct
{
    BSP_U16 enable;        /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    BSP_U16 hkadc_id;      /* 系统放电温度保护的hkadc通道ID */
    BSP_U16 high_thres;    /* 系统放电高温保护的温度阀值 */
    BSP_U16 high_count;    /* 系统放电高温保护次数上限 */
    BSP_S16 low_thres;     /* 系统放电低温保护的温度阀值 */
    BSP_U16 low_count;     /* 系统放电低温保护次数 */

    BSP_U32 reserved[2];   /*保留*/
} DRV_SYS_TEMP_STRU;

/*温度保护物理逻辑转换表  NV_ID_DRV_TEMP_CHAN_MAP          = 0xd126 */
typedef struct
{
    BSP_U16 chan_map[16];

}TEMP_HKADC_PHY_LOGIC_ARRAY;

/* WIN7 Feature for usb id:50075 */
typedef struct
{
    BSP_U8 wwan_flag;
    BSP_U8 reserved1;
    BSP_U8 reserved2;
    BSP_U8 reserved3;
    BSP_U8 reserved4;
    BSP_U8 reserved5;
    BSP_U8 reserved6;
    BSP_U8 reserved7;
} NV_WIN7_FEATURE;

typedef struct
{
    BSP_U32 sci_dsda_select;        /* 0: sim0, 1:sim1*/
} DRV_SCI_DSDA_SELECT;
/* SIM gcf test flage*/
typedef struct
{
    BSP_U32 sci_gcf_sub_flag;        /* 0: open, 1:close */
} DRV_SCI_GCF_STUB_FLAG;

/*E5串口复用*/
typedef struct
{
	BSP_U32 wait_usr_sele_uart : 1;//[bit 0-0]1: await user's command for a moment; 0: do not wait
	BSP_U32 a_core_uart_num    : 2;//[bit 1-2]the number of uart used by a core
	BSP_U32 c_core_uart_num    : 2;//[bit 3-4]the number of uart used by c core
	BSP_U32 m_core_uart_num    : 2;//[bit 5-6]the number of uart used by m core
	BSP_U32 a_shell            : 1;//[bit 7-7]0:ashell is not used; 1:ashell is used
	BSP_U32 c_shell            : 1;//[bit 8-8]0:cshell is not used; 1:cshell is used
	BSP_U32 uart_at            : 1;//[bit 9-9]uart at control
	BSP_U32 extendedbits       : 22;//[b00]1:open cshell_auart; 0:close
}DRV_UART_SHELL_FLAG;

/* product support module nv define */
typedef struct
{
	BSP_U32 sdcard 		: 1;//1: support; 0: not support
	BSP_U32 charge 		: 1;
	BSP_U32 wifi    	: 1;
	BSP_U32 oled    	: 1;
	BSP_U32 hifi        : 1;
	BSP_U32 onoff       : 1;
	BSP_U32 hsic        : 1;
	BSP_U32 localflash  : 1;
	BSP_U32 reserved    : 24;
} DRV_MODULE_SUPPORT_STRU;

typedef struct
{
	BSP_U8 normalwfi_flag;
	BSP_U8 deepsleep_flag;
	BSP_U8 buck3off_flag;
	BSP_U8 peridown_flag;
	BSP_U32 deepsleep_Tth;
	BSP_U32 TLbbp_Tth;
}DRV_NV_PM_TYPE;

/* NV_ID_DRV_TSENS_TABLE = 0xd129*/
typedef struct
{
    BSP_S16 temp[256];

}DRV_TSENS_TEMP_TABLE;

typedef struct
{
    BSP_U8 ucABBSwitch; /*对应模式使用的ABB物理通道，0 通道0, 1: 通道1, 2: 同时使用两个通道*/
    BSP_U8 ucRFSwitch;  /*对应模式使用的RF物理通道，0 通道0, 1: 通道1, 2: 同时使用两个通道*/
    BSP_U8 ucTCXOSwitch; /*对应模式使用的TCXO ID 0 TCXO0, 1: TCXO1*/
    BSP_U8 reserve;
}NV_TLMODE_BASIC_PARA_STRU;

typedef struct
{
    NV_TLMODE_BASIC_PARA_STRU stModeBasicParam[2];/*下标为0:LTE, 1:TDS*/
}NV_TLMODE_CHAN_PARA_STRU;

typedef struct
{
    BSP_U32 clkdis1; /*初始关闭时钟寄存器，对应crg clk dis1*/
    BSP_U32 clkdis2; /*初始关闭时钟寄存器，对应crg clk dis2*/
    BSP_U32 clkdis3; /*初始关闭时钟寄存器，对应crg clk dis3*/
    BSP_U32 clkdis4;  /*初始关闭时钟寄存器，对应crg clk dis4*/
	BSP_U32 clkdis5;  /*初始关闭时钟寄存器，对应crg clk dis5*/
    BSP_U32 mtcmosdis; /*初始关闭MTCMOS寄存器，对应crg mtcmosdis*/
}DRV_NV_PM_CLKINIT_STRU;

typedef struct
{
    BSP_U32 ucBBPCh0TcxoSel:1; 	    /* bit 0,CH0 19.2M使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x90000148 */
    BSP_U32 ucBBPCh1TcxoSel:1;  	/* bit 1,CH1 19.2M使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x90000148 */
    BSP_U32 ucABBCh0TcxoSel:1; 		/* bit 2,ABB Ch0 使用的TCXO源，0 TCXO0, 1: TCXO1; 对应ABB寄存器 0x94 */
    BSP_U32 ucABBCh1TcxoSel:1; 		/* bit 3,ABB Ch1 使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x94 */
	BSP_U32 ucBbpPllTcxoSel:1; 		/* bit 4,BBP PLL 使用的TCXO源，0 TCXO0, 1: TCXO1;*/
	BSP_U32 ucG1bp104mTcxoSel:1; 	/* bit 5,G1BBP 104M 使用的ABB通道，0：通道0, 1: 通道1;*/
	BSP_U32 ucG2bp104mTcxoSel:1; 	/* bit 6,G1BBP 104M 使用的ABB通道，0：通道0, 1: 通道1;*/
	BSP_U32 reserve:25;             /* bit 7-31*/
}DRV_TCXO_SEL_PARA_STRU;

typedef struct
{
    BSP_U32 u32UsbDbg;   /* usb模块调试信息级别 */
}DRV_USB_DBG_STRU;

typedef struct
{
	BSP_U32 wdt_enable;
	BSP_U32 wdt_timeout;
	BSP_U32 wdt_keepalive_ctime;
	BSP_U32 wdt_suspend_timerout;
	BSP_U32 wdt_reserve;
}DRV_WDT_INIT_PARA_STRU;


/*温度保护物理逻辑转换表  NV_ID_DRV_TSENSOR_TRIM          = 0xd12f */
typedef struct
{
    BSP_U16 tsensor_trim[16];

}DRV_TSENSOR_TRIM_STRU;

/* axi monitor监控ID配置 */
typedef struct
{
    BSP_U32 reset_flag;     /* 复位标志，匹配到监控条件是否复位 */
    BSP_U32 opt_type;       /* 监控类型，01:读，10:写，11:读写，其余值:不监控 */
    BSP_U32 port;           /* 监控端口 */
    BSP_U32 master_id;      /* 监控masterid */
    BSP_U32 start_addr;     /* 监控起始地址 */
    BSP_U32 end_addr;       /* 监控结束地址 */
} AMON_CONFIG_T;

/* axi monitor配置NV项 NV_ID_DRV_AMON = 0xd130 */
typedef struct
{
    BSP_U32         en_flag;            /* 使能标志，00:去使能，01:SOC，10:CPUFAST，11:SOC,CPUFAST */
    AMON_CONFIG_T   soc_config[8];      /* SOC配置，8个监控ID */
    AMON_CONFIG_T   cpufast_config[8];  /* CPUFAST配置，8个监控ID */
} DRV_AMON_CONFIG_STRU;

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
    RF_MODEM_CONTROL modem_inside;	/*内置modem*/
    RF_MODEM_CONTROL modem_outside; /*外置modem*/
}RF_GPIO_CFG;

typedef struct
{
    RF_GPIO_CFG rf_switch_cfg[16];
}NV_RF_SWITCH_CFG_STRU;

/* SOCP按需开关特性配置NV项 = 0xd132 */
typedef struct
{
    BSP_U32         en_flag;            /* 使能标志，00:去使能，01:使能 */
} DRV_SOCP_ON_DEMAND_STRU;

/* drx delay flag */
typedef struct
{
    BSP_U8         lpm3_flag;         /* 0x11 代表lpm3 */
    BSP_U8         lpm3_0;            /* 1打开delay,其他关闭delay */
    BSP_U8         lpm3_1;
    BSP_U8         lpm3_2;
    BSP_U8         lpm3_3;
    BSP_U8         lpm3_4;
    BSP_U8         drv_flag;          /* 0x22 代表drv */
    BSP_U8         drv_0;             /* 1打开delay,其他关闭delay */
    BSP_U8         drv_1;
    BSP_U8         drv_2;
    BSP_U8         drv_3;
    BSP_U8         drv_4;
    BSP_U8         msp_flag;          /* 0x33 代表msp */
    BSP_U8         msp_0;             /* 1打开delay,其他关闭delay */
    BSP_U8         msp_1;
    BSP_U8         msp_2;
    BSP_U8         msp_3;
    BSP_U8         msp_4;
}DRV_DRX_DELAY_STRU;

/* C核单独复位按需开关特性配置NV项 = 0xd134 */
typedef struct
{
    BSP_U32 is_feature_on:1;         /* bit0, 整个C核单独复位功能是否打开 */
    BSP_U32 is_connected_ril:1;      /* bit1, 是否和RIL对接 */
	BSP_U32 reserve:30;              /* bit 2-31*/
} DRV_CCORE_RESET_STRU;

/* NV ID = 0xd135 */
/* 用于表示每路LDO和BUCK是否打开 */
typedef struct
{
	BSP_U32 ldo1_switch     :1;         /* 第0bit，值为1则为打开，值为0则为关闭 */
	BSP_U32 ldo2_switch     :1;         /* 第1bit，值为1则为打开，值为0则为关闭 */
	BSP_U32 buckpa_switch   :1;         /* 第2bit，值为1则为打开，值为0则为关闭 */
	BSP_U32 buck1_switch    :1;         /* 第3bit，值为1则为打开，值为0则为关闭 */
	BSP_U32 buck2_switch    :1;         /* 第4bit，值为1则为打开，值为0则为关闭 */
	BSP_U32 reserved        :27;        /* 第5~31bit，预留。默认值为0 */
}NV_PASTAR_SWITCH_STRU_BITS;

typedef struct
{
    union
    {
        BSP_U32 u32;
        NV_PASTAR_SWITCH_STRU_BITS bits;
    }cont;
}NV_PASTAR_SWITCH_STRU;
/* end NV ID = 0xd135 */


/* log2.0 2014-03-19 Begin:*/
typedef struct
{
    BSP_U32 ulSocpDelayWriteFlg;/* SOCP延迟写入功能启动标记 */
    BSP_U32 ulGuSocpLevel;      /* GU SOCP目的BUFFER发送水线 */
    BSP_U32 ulLSocpLevel;       /* L SOCP目的BUFFER发送水线 */
    BSP_U32 ulTimeOutValue;     /* SOCP目的BUFFER的超时时间 */
}DRV_NV_SOCP_LOG_CFG_STRU;
/* log2.0 2014-03-19 End*/
typedef struct
{
    BSP_U32  enUartEnableCfg;
	BSP_U32  AwaitReplyTimer;
	BSP_U32  AwakeTmer;
	BSP_U32  DoSleepTimer;
}DRV_DUAL_MODEM_STR;

/* GPIO控制LDO开关 NV项 = 0xd137 */
typedef struct
{
    BSP_U32 gpio;      /* LDO对应GPIO编号 */
    BSP_U32 used;      /* GPIO是否使用 */
} DRV_DRV_LDO_GPIO_CFG;

typedef struct
{
    DRV_DRV_LDO_GPIO_CFG ldo_gpio[2];
}DRV_DRV_LDO_GPIO_STRU;


/* GPIO控制LDO开关 NV项 = 0xd138 */
typedef struct
{
    BSP_U32 modem_id;      /* modem id默认从0开始依次增大 */
    BSP_U32 gpio;          /* ANTEN对应GPIO编号 */
    BSP_U32 used;          /* GPIO是否使用 */
} DRV_DRV_ANTEN_GPIO_CFG;

typedef struct
{
    DRV_DRV_ANTEN_GPIO_CFG anten_gpio[LDO_GPIO_MAX];
}DRV_DRV_ANTEN_GPIO_STRU;

/* RSE POWER GPIO控制 NV项 = 0xd139 */
typedef struct
{
    BSP_U32 ulRsePowerOnIds;
    BSP_U32 ulFemCtrlInfo;
    BSP_U32 ulRfGpioBitMask;
    BSP_U32 ulRfGpioOutValue;
    BSP_U16 usFemMipiCmdAddr;
    BSP_U16 usFemMipiCmdData;
}RF_NV_RSE_CFG_STRU;

/*天线开关不下电特性NV*/
/*NVID = 0xd13b，0为不使能该特性，1为使能*/
typedef struct
{
	BSP_U32 is_enable;/*Range:[0,1]*/
}DRV_ANT_SW_UNPD_ENFLAG;

typedef struct
{
	BSP_U32 gpio_num;	/*GPIO编号    */
	BSP_U32 is_used;	/*Range:[0,1]*//*是否使用*/
	BSP_U32 value;		/*配置值 */
}DRV_ANT_GPIO_CFG;

/*nvID = 0xd13c*/
/*主分集两组开关+副modem的一组天线开关的配置，每组最多8个，由zhaojun确认*/
/*0表示主分集1组，1表示主分集2组，2表示副modem天线开关*/
typedef struct
{
	DRV_ANT_GPIO_CFG 	antn_switch[8];	
}DRV_ANT_SW_UNPD_CFG;

typedef struct
{
	DRV_ANT_SW_UNPD_CFG all_switch[3];
}NV_DRV_ANT_SW_UNPD_CFG;
/*End 天线开关不下电特性NV*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

