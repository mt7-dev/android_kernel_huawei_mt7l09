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
#ifdef __cplusplus
extern "C"
{
#endif
#include "osl_types.h"
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
#define    HI6561_STATUS1_OFFSET                            (0x21) /* status 1 register */
#define    HI6561_PERIPH_CTRL1_OFFSET                   (0x22) /* other ctrl 1 register*/
#define    HI6561_ONOFF1_OFFSET                              (0x23) /* ON OFF1 register */
#define    HI6561_ONOFF2_OFFSET                              (0x24) /* ON OFF2 register */
#define    HI6561_SET_BUCK2_OFFSET                         (0x26) /* BUCK2 voltage adjust register */
#define    HI6561_SET_BUCK1_OFFSET                         (0x27) /* BUCK1 voltage adjust register */
#define    HI6561_SET_BUCK0_OFFSET                         (0x28) /* BUCK0 voltage adjust register  */
#define    HI6561_BUCK0_ADJ1_OFFSET                       (0x29) /* BUCK0 adjust register  1 */
#define    HI6561_BUCK0_ADJ2_OFFSET                       (0x2A) /* BUCK0 adjust register  2 */
#define    HI6561_BUCK0_ADJ3_OFFSET                       (0x2B) /* BUCK0 adjust register  3 */
#define    HI6561_BUCK0_ADJ4_OFFSET                       (0x2C) /* BUCK0 adjust register  4 */
#define    HI6561_BUCK0_ADJ5_OFFSET                       (0x2D) /* BUCK0 adjust register  5 */
#define    HI6561_BUCK1_ADJ1_OFFSET                       (0x30) /* BUCK1 adjust register  1 */
#define    HI6561_BUCK1_ADJ2_OFFSET                       (0x31) /* BUCK1 adjust register  2 */
#define    HI6561_BUCK1_ADJ3_OFFSET                       (0x32) /* BUCK1 adjust register  3*/
#define    HI6561_BUCK2_ADJ1_OFFSET                       (0x36) /* BUCK2 adjust register  1 */
#define    HI6561_BUCK2_ADJ2_OFFSET                       (0x37) /* BUCK2 adjust register  2*/
#define    HI6561_BUCK2_ADJ3_OFFSET                       (0x38) /* BUCK2 adjust register  4 */
#define    HI6561_BUCK2_ADJ4_OFFSET                       (0x39) /* BUCK2 adjust register  5*/
#define    HI6561_BUCK_CTRL1_OFFSET                       (0x45) /* BUCK frequncy ctrl register 1 */
#define    HI6561_BUCK_CTRL2_OFFSET                       (0x46) /* BUCK frequncy ctrl register 2 */
#define    HI6561_SET_LDO1_OFFSET                           (0x48) /* LDO1 voltage adjust register */
#define    HI6561_SET_LDO2_OFFSET                           (0x49) /* LDO2 voltage adjust register */
#define    HI6561_SET_LDO_CTRL_OFFSET                   (0x4A) /* LDO ctrl register*/
#define    HI6561_PA_AMP_CTRL_OFFSET                   (0x4B) /* PA_AMPLIFIER register */
#define    HI6561_CLASSAB_CTRL_OFFSET                   (0x4C) /* CLASSAB ctrl register */
#define    HI6561_ILIM_CTRL1_OFFSET                         (0x4E) /* ILIM_CTRL ctrl register  1*/
#define    HI6561_ILIM_CTRL2_OFFSET                         (0x4F) /* ILIM_CTRL ctrl register 2 */
#define    HI6561_BANDGAP_OFFSET                           (0x55) /* BANDGAP ctrl register  */
#define    HI6561_ANA_IN_OFFSET                               (0x56) /* PMU simulate part  IO register */
#define    HI6561_RESERVED1_OFFSET                         (0x57) /* reserved register 1 */
#define    HI6561_RESERVED2_OFFSET                         (0x58) /* reserved register 2 */
#define    HI6561_RESERVED0_OFFSET                         (0x59) /* reserved register 3  */
#define    HI6561_SET_OTP_OFFSET                             (0x5A) /* OTP set register */
#define    HI6561_OTP_CTRL_OFFSET                           (0x5B) /* OTP ctrl register */
#define    HI6561_OTP_PDIN_OFFSET                           (0x5C) /* OTP set value register */
#define    HI6561_OTP_PDOB0_OFFSET                        (0x5D) /* OTP read value register 0*/
#define    HI6561_OTP_PDOB1_OFFSET                        (0x5E) /* OTP read value register 1 */
#define    HI6561_OTP_PDOB2_OFFSET                        (0x5F) /* OTP read value register 2 */
#define    HI6561_OTP_PDOB3_OFFSET                        (0x60) /* OTP read value register 3 */
#define    HI6561_NO_PWR_REG_RO_OFFSET             (0x6A) /* never power off register */

#define HI6561_TEST

#define HI6561_EN_LDO2_INIT_MASK ((u32)~(1<<3))
#define HI6561_EN_ECO_LDO2_MASK ((u32)~(1<<2))
#define HI6561_EN_LDO1_INIT_MASK ((u32)~(1<<1))
#define HI6561_EN_ECO_LDO1_MASK ((u32)~(1<<0))

#define HI6561_EN_LDO2_INIT_BIT	(3)
#define HI6561_EN_ECO_LDO2_BIT	(2)/*in low power cusumption ,pastar will be closed ,so never in eco mode*/
#define HI6561_EN_LDO1_INIT_BIT (1)
#define HI6561_EN_ECO_LDO1_BIT	(0)


#define HI6561_EN_BUCK2_INIT_MASK ((u8)~(1<<5))
#define HI6561_EN_BUCK1_INIT_MASK ((u8)~(1<<3))
#define HI6561_EN_BUCK0_INIT_MASK ((u8)~(1<<1))
#define HI6561_EN_BUCK0_APT_MASK  ((u8)~(1<<4))

#define HI6561_EN_BUCK2_INIT_BIT (5)
#define HI6561_EN_BUCK1_INIT_BIT (3)
#define HI6561_EN_BUCK0_INIT_BIT (1)
#define HI6561_EN_BUCK0_APT_BIT  (4)

#define HI6561_SET_VOLTAGE_MASK (0xf0)
#define HI6561_ILIM_CTRL1_MASK 	(0xf8)
#define HI6561_NO_PWR_REG_RO_MASK 	(0x80)
#define HI6561_VERSIN_BEFOR_MASK	(0xc0)

#define HI6561_IS_V100 (0x10)
#define HI6561_VERSION_BEFOR "HI6561V110"		/*befor ec version*/
#define HI6561_VERSION_AFTER "HI6561V100"		/*after ec version*/

#define VOLTAGE_BIT_OFFSET 	(0)
#define VOLTAGE_BIT_MASK	(0xf0)

/********************************************************************************/
/*    hi6561 data type      */
/********************************************************************************/

typedef enum POWER_PROC_ENUM_E
{
	PA_STAR_POWER_OFF   = 0x0,
	PA_STAR_POWER_ON,
	PA_STAR_POWER_BUTT
}POWER_PROC_ENUM;

/*voltage type*/
typedef  enum VOLTAGE_TYPE{
	VOLTAGE0=0,	/*BUCK1:1.00V  LDO:1.5V*/
	VOLTAGE1,	/*BUCK1:1.10V  LDO:1.6V*/
	VOLTAGE2,	/*BUCK1:1.15V  LDO:1.7V*/
	VOLTAGE3,	/*BUCK1:1.20V  LDO:1.8V*/
	VOLTAGE4,	/*BUCK1:1.25V  LDO:2.2V*/
	VOLTAGE5,	/*BUCK1:1.30V  LDO:2.5V*/
	VOLTAGE6,	/*BUCK1:1.35V  LDO:2.6V*/
	VOLTAGE7,	/*BUCK1:1.40V  LDO:2.7V*/
	VOLTAGE8,	/*BUCK1:1.45V  LDO:2.8V*/
	VOLTAGE9,	/*BUCK1:1.50V  LDO:2.9V*/
	VOLTAGEA,	/*BUCK1:1.55V  LDO:3.0V*/
	VOLTAGEB,	/*BUCK1:1.60V  LDO:3.1V*/
	VOLTAGEC,	/*BUCK1:1.70V  LDO:3.2V*/
	VOLTAGED,	/*BUCK1:1.75V  LDO:3.3V*/
	VOLTAGEE,	/*BUCK1:1.80V  LDO:3.4V*/
	VOLTAGEF,	/*BUCK1:1.85V  LDO:3.5V*/
	VOLTAGEMAX
}VOLTAGE;


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
typedef struct LIMIT_CURRENT_TABLE{
	enum LIMIT_CURRENT_TABLE_TYPE current_id;
	u32 current_ma;
}LIMIT_CURRENT_TABLE_E;

/*exception type*/
typedef enum EXCEPTION_TYPE{
	LDO1CUR=0,
	LDO2CUR,
	BUCK0,
	BUCK1,
	BUCK2,
	TEMPERATURE150,
	TEMPERATURE125,
	EXCEPTION_MAX
}EXCEPTION_TYPE_E;

/*hi6561 ctrl param*/
typedef struct
{
	u16 *voltage_list;
	u8 onoff_reg_addr;
	u8 onoff_bit_offset;
	u8 voltage_reg_addr;
	u8 reserved;
}HI6561_VLTGS_ATTR;


int pmu_hi6561_write_debug(u8 reg_addr,u8 reg_data,HI6561_ID_ENUM chip_id);

int pmu_hi6561_read_debug(HI6561_ID_ENUM chip_id,u8 reg_addr);


int pmu_hi6561_debug(HI6561_ID_ENUM chip_id);


int pmu_hi6561_buck1_2_phony_exception_clear(HI6561_ID_ENUM chip_id);


int pmu_hi6561_buck1_2_real_exception_clear(HI6561_ID_ENUM chip_id);
#ifdef __cplusplus
}
#endif
#endif // __HI_HI6561_H__


