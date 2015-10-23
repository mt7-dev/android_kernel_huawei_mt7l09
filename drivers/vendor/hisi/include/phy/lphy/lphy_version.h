/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : lphy_version.h
  版 本 号   : 初稿
  作    者   : tuzhiguo(52067)
  生成日期   : 2011年11月08日
  最近修改   :
  功能描述   :

        一. 目前存在的芯片有, 如下:
            1) HI6920_ES: 对应MPW版本
                1.1> HI6920ES_FPGA_P500: MPW FPGA验证平台
                1.2> HI6920ES_ASIC:      MPW ASIC芯片
            2) HI6920_CS: 对应PILOT版本
                2.1> HI6920CS_FPGA_P500: PILOT FPGA验证平台
                2.2> HI6920CS_ASIC:      PILOT ASIC芯片

        二. 针对当前的平台特性, 分别加上LPHY作为物理层的版本特性, 如下:
            1) HI6920_ES: 对应MPW版本
                1.1> LPHY_HI6920ES_FPGA_P500: MPW FPGA验证平台
                1.2> LPHY_HI6920ES_ASIC:      MPW ASIC芯片
            2) HI6920_CS: 对应PILOT版本
                2.1> LPHY_HI6930CS_FPGA_P500: PILOT FPGA验证平台
                2.2> LPHY_HI6930CS_ASIC:      PILOT ASIC芯片

        三. 该文件先包含外部产品头文件, 根据产品宏决定当前的版本号

******************************************************************************/

#ifndef __LPHY_VERSION_H__
#define __LPHY_VERSION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "TLPhyInterface.h"
/***********************************************************

一. 产品版本宏定义, 目前存在的芯片有, 如下:
    1.1) HI6920_ES: 对应MPW版本
        1.1.1> HI6920ES_FPGA_P500: MPW FPGA验证平台
        1.1.2> HI6920ES_ASIC:      MPW ASIC芯片
    1.2) HI6920_CS: 对应PILOT版本
        1.2.1> HI6920CS_FPGA_P500: PILOT FPGA验证平台
        1.2.2> HI6920CS_ASIC:      PILOT ASIC芯片

二). 针对当前的产品版本, 分别加上LPHY作为物理层的版本特性, 如下:
    1) HI6920_ES: 对应MPW版本
        1.1> LPHY_HI6920ES_FPGA_P500: MPW FPGA验证平台
        1.2> LPHY_HI6920ES_ASIC:      MPW ASIC芯片
    2) HI6920_CS: 对应PILOT版本
        2.1> LPHY_HI6930CS_FPGA_P500: PILOT FPGA验证平台
        2.2> LPHY_HI6930CS_ASIC:      PILOT ASIC芯片

三). 物理层版本版本宏定义
    有的代码只区分ES/CS, 或者只区分FPGA_P500/ASIC 也定义这些相关的宏供引用

    1) 芯片型号: ES or CS
        1.1> LPHY_HI6920ES           0xE0       //mpw
        1.2> LPHY_HI6930CS           0xC0       //pilot
    2) 芯片平台
        2.1> LPHY_FPGA_P500           0xF0       //FPGA P500验证平台
        2.2> LPHY_ASIC                0xA0       //ASIC样片平台

****************************************************************/

/*
   将版本号组成一个32位数, 主要分三段如下
    2)下面分别定义组成版本号的宏:
        MAJOR   : LPHY_HI6920ES(0xE0) or LPHY_HI6930CS(0xC0)
        MINOR   : LPHY_FPGA_P500(0xF0) or LPHY_ASIC(0xA0)
        NUMBER  : B001,B002,.....,B060,.....
    2)例如LPHY_HI6920_CS_FPGA_P500用数字表示为:
        0xC0F0B001----> CS版本FPGA平台B001版本
*/

#define PHY_ON		(0x1)
#define PHY_OFF		(0x0)


#define LPHY_VERSION(MAJOR,MINOR,NUMBER)  (((MAJOR)<<24) | ((MINOR)<<16) | NUMBER)
#define LPHY_MAJOR_VERSION(x)             (((x) >> 24) & 0x00FF)
#define LPHY_MINOR_VERSION(x)             (((x) >> 16) & 0x00FF)
#define LPHY_NUMBER_VERSION(x)            (((x) >>  0) & 0xFFFF)

/*芯片类型*/
#if 	defined(LPHY_CHIP_BB_6930ES)
    #define LPHY_HI6930CS           (0xE0)
    #define CHIP_BB_VERSION         LPHY_HI6930CS
#elif 	defined(LPHY_CHIP_BB_6930CS)
    #define LPHY_HI6930CS           (0xC0)
    #define CHIP_BB_VERSION         LPHY_HI6930CS
#else
    #error "sorry, this is not a supported baseband version, please include product_config.h"
#endif

/*单板类型*/
#if defined(TL_PHY_FPGA_P500)
    #define LPHY_FPGA_P500          		(0xF0)
    #define LPHY_BOARD_VERSION      		(LPHY_FPGA_P500)
	#error "sorry, not support P500 board"
#elif defined(TL_PHY_FPGA_P531)
    #define LPHY_FPGA_P530          		(0xF3)
    #define LPHY_BOARD_VERSION      		(LPHY_FPGA_P530)
	#define LPHY_FEATURE_LCS_SWITCH			PHY_OFF
	#define LPHY_FEATURE_EMBMS_SWITCH		PHY_OFF
	#define LPHY_FEATURE_ONE_XO_SWITCH		PHY_ON
	#define LPHY_FEATURE_MAX_CARRIER_NUM	1	
#elif (defined(TL_PHY_ASIC_HI6930) || defined(BOARD_ASIC_BIGPACK) ||defined(TL_PHY_SFT))
    #define LPHY_ASIC_B720
    #define LPHY_ASIC               		(0xA0)
    #define LPHY_BOARD_VERSION      		(LPHY_ASIC)
	#define LPHY_FEATURE_LCS_SWITCH			PHY_OFF
	#define LPHY_FEATURE_EMBMS_SWITCH		PHY_ON
	#define LPHY_FEATURE_ONE_XO_SWITCH		PHY_OFF
	#define LPHY_FEATURE_MAX_CARRIER_NUM	2	
    #define BALONG_FEATURE_LTE_DT
    #define SG_REPORT
    
	
	#if defined(TL_PHY_ASIC_HI6930)
		#define LPHY_IN_FACT_ASIC_BOARD
	#endif
#elif (defined(TL_PHY_ASIC_K3V3) || defined(TL_PHY_K3V3_SFT))
	#define LPHY_ASIC_K3V3
    #define LPHY_ASIC               		(0xA3)
    #define LPHY_BOARD_VERSION      		(LPHY_ASIC)

	#define LPHY_FEATURE_LCS_SWITCH			PHY_OFF
	#define LPHY_FEATURE_EMBMS_SWITCH		PHY_ON
	#define LPHY_FEATURE_ONE_XO_SWITCH		PHY_OFF
	#define LPHY_FEATURE_MAX_CARRIER_NUM	2	
    #define BALONG_FEATURE_LTE_DT
    #define SG_REPORT
    #if defined(TL_PHY_ASIC_K3V3)
        #define LPHY_IN_FACT_ASIC_BOARD
    #endif
	
#elif (defined(TL_PHY_ASIC_V810) || defined(TL_PHY_V810_SFT))

	#define LPHY_ASIC_V810
    #define LPHY_ASIC               		(0xA8)
    #define LPHY_BOARD_VERSION      		(LPHY_ASIC)

	#define LPHY_FEATURE_LCS_SWITCH			PHY_OFF
	#define LPHY_FEATURE_EMBMS_SWITCH		PHY_OFF
	#define LPHY_FEATURE_ONE_XO_SWITCH		PHY_ON
	#define LPHY_FEATURE_MAX_CARRIER_NUM	1

    #if defined(TL_PHY_ASIC_V810)
		#define LPHY_IN_FACT_ASIC_BOARD
	#endif
	#define SG_REPORT
#elif (defined(TL_PHY_ASIC_V711) || defined(TL_PHY_V711_SFT))

	#define LPHY_ASIC_V711
	#define LPHY_ASIC               		(0xA7)
    #define LPHY_BOARD_VERSION      		(LPHY_ASIC)

	#define LPHY_FEATURE_LCS_SWITCH			PHY_OFF
	#define LPHY_FEATURE_EMBMS_SWITCH		PHY_OFF
	#define LPHY_FEATURE_ONE_XO_SWITCH		PHY_OFF
	#define LPHY_FEATURE_MAX_CARRIER_NUM	1
	#define SG_REPORT

#else
    #error "sorry, this is not a supported asic version, please include product_config.h"
#endif

#if (PHY_ON == LPHY_FEATURE_LCS_SWITCH)
	#define BALONGV7_FEATURE_LCS_ENABLE
	#define BALONGV7_FEATURE_LCS
#else
	#define BALONGV7_FEATURE_LCS_DISABLE	
#endif

#if (PHY_ON == LPHY_FEATURE_EMBMS_SWITCH)
	#define BALONGV7_FEATURE_EMBMS_ENABLE
	#define BALONGV7_FEATURE_EMBMS
#else
	#define BALONGV7_FEATURE_EMBMS_DISABLE
#endif

#if (PHY_ON == LPHY_FEATURE_ONE_XO_SWITCH)
	#define LPHY_FEATURE_ONE_XO_ENABLE
	#define LPHY_FEATURE_ONE_XO
#else
	#define LPHY_FEATURE_ONE_XO_DISABLE
#endif

#if   (1 == LPHY_FEATURE_MAX_CARRIER_NUM)
	#define LPHY_FEATURE_CA_DISABLE
#elif (2 <= LPHY_FEATURE_MAX_CARRIER_NUM)
	#define LPHY_FEATURE_CA_ENABLE
#else 
	#error "sorry, error sopport carrier number"
#endif


/*RF类型定义*/

#define LPHY_MAIN_VERSION      LPHY_VERSION(CHIP_BB_VERSION, LPHY_BOARD_VERSION, 0xB001)

/*由于CHIP_BB_6920CS porting版本黑盒包的不够全，导致软件要做异常处理，排除一部分寄存器的配置*/#if (defined(CHIP_BB_6920CS) && defined(BOARD_SFT))
    #define LPHY_HI6930CS_PORTING
#endif


/*
    LPHY内部强制禁止使用定义过的产品宏
*/
#undef BOARD_FPGA_P500
#undef BOARD_ASIC
#undef BOARD_ASIC_BIGPACK
#undef BOARD_SFT
#undef CHIP_BB_6920CS
#undef CHIP_BB_6920ES
#undef CHIP_RFIC_6360V200
#undef CHIP_RFIC_6360V210
#undef CHIP_RFIC_9361


/*版本帮助字符串*/
#define LPHY_TIP "Hisilicon balongV7R2 GUTL lphy develop group!"


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __LPHY_VERSION_H__ */
