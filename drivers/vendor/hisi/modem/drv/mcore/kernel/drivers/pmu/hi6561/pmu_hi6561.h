/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : pmu_hi6561.h */
/* Version       : 2.0 */
/* Created       : 2013-03-14*/
/* Last Modified : */
/* Description   :  The C union definition file for the module hi6561*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __PMU_HI6561_H__
#define __PMU_HI6561_H__

#include "bsp_pmu_hi6561.h"
/*
 * Project: hi
 * Module : pmu
 * chip : hi6561
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

#ifndef PASTAR_FALSE
#define PASTAR_FALSE                0
#endif

#ifndef PASTAR_TRUE
#define PASTAR_TRUE                 1
#endif

/********************************************************************************/
/*    hi6561 register offset define£¨project name_module name_register name_OFFSET)        */
/********************************************************************************/
#define    HI6561_VERSION_OFFSET                            (0x20) /* version register */
#define    HI6561_ONOFF1_OFFSET                              (0x23) /* ON OFF1 register */
#define    HI6561_ONOFF2_OFFSET                              (0x24) /* ON OFF2 register */

#define HI6561_TEST

#define HI6561_EN_LDO2_INIT_BIT	(3)
#define HI6561_EN_ECO_LDO2_BIT	(2)/*in low power cusumption ,pastar will be closed ,so never in eco mode*/
#define HI6561_EN_LDO1_INIT_BIT (1)
#define HI6561_EN_ECO_LDO1_BIT	(0)


#define HI6561_EN_BUCK0_APT_MASK  ((u8)~(1<<4))

#define HI6561_EN_BUCK2_INIT_BIT (5)
#define HI6561_EN_BUCK1_INIT_BIT (3)
#define HI6561_EN_BUCK0_INIT_BIT (1)
#define HI6561_EN_BUCK0_APT_BIT (4)

#define HI6561_SET_VOLTAGE_MASK (0xf0)
#define HI6561_ILIM_CTRL1_MASK 	(0xf8)
#define HI6561_NO_PWR_REG_RO_MASK 	(0xc0)
#define HI6561_VERSIN_BEFOR_MASK	(0xc0)

#define HI6561_IS_V100 (0x10)
#define HI6561_VERSION_BEFOR "HI6561V110"		/*befor ec version*/
#define HI6561_VERSION_AFTER "HI6561V100"		/*after ec version*/

#define VOLTAGE_BIT_OFFSET 	(0)

/********************************************************************************/
/*    hi6561 data type      */
/********************************************************************************/

typedef enum POWER_PROC_ENUM_E
{
	PA_STAR_POWER_OFF   = 0x0,
	PA_STAR_POWER_ON,
	PA_STAR_POWER_BUTT
}POWER_PROC_ENUM;


/*limit current id*/
enum LIMIT_CURRENT_TABLE_TYPE{
	LIMIT_CURRENT_500MA=0,
	LIMIT_CURRENT_300MA,
	LIMIT_CURRENT_200MA,
	LIMIT_CURRENT_NO,
	LIMIT_CURRENT_1800MA,
	LIMIT_CURRENT_1500MA,
	LIMIT_CURRENT_1200MA,
	LIMIT_CURRENT_900MA
};

/*limit contrl type*/
struct LIMIT_CURRENT_TABLE{
	enum LIMIT_CURRENT_TABLE_TYPE current_id;
	u32 current_ma;
};

/*exception type*/
enum EXCEPTION_TYPE{
	LDO1CUR=0,
	LDO2CUR,
	BUCK0,
	BUCK1,
	BUCK2,
	TEMPERATURE150,
	TEMPERATURE125,
	EXCEPTION_MAX
};

#endif // __HI_HI6561_H__


