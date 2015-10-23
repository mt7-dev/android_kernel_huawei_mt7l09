
#ifndef __BSP_REGULATOR_H__
#define __BSP_REGULATOR_H__
#include "osl_common.h"
#include "bsp_sram.h"
#include "drv_regulator.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
 * struct regulator
 *
 * One for each consumer device.
 */

#define VOLT_NAME_PA_VCC "PA-vcc"
#define VOLT_NAME_PA_VBIA2_VCC "PA_Vbias2-vcc"
#define VOLT_NAME_RFIC_FRONT_VCC "RFIC_front-vcc"
#define VOLT_NAME_RF_FEM_VCC "RF_FEM-vcc"
#define VOLT_NAME_RFIC_VCC "RFIC-vcc"

 #ifdef __KERNEL__

#define H6551_DR1_NM "Balong_dr1"
#define H6551_DR2_NM "Balong_dr2"
#define H6551_DR3_NM "Balong_dr3"
#define H6551_DR4_NM "Balong_dr4"
#define H6551_DR5_NM "Balong_dr5"
#define HI6551_SD_DR  "SD_DR-vcc"
#define HI6551_LCD_DR "BL-vcc"

#include <linux/regulator/consumer.h>

#elif defined(__VXWORKS__)

struct regulator{
	const char *name;
	int id;
	unsigned int  num_consumer_supplies;
	struct regulator_consumer_supply *consumer_supplies;
	int use_count;
};
#else

#endif /* __KERNEL__ */
/*for k3 regulator init*/
void regulator_init(void);

#ifdef __KERNEL__
#define BUCK3_ONOFF_FLAG			(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_ACORE_ONOFF_FLAG)
#elif defined( __VXWORKS__)
#define BUCK3_ONOFF_FLAG			(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_CCORE_ONOFF_FLAG)
#elif defined(__CMSIS_RTOS)
#endif /* end of __KERNEL__ */
/*****************************************************************************
* 函 数 名  : regulator_get
* 功能描述  :查找并获得regulator，用于后面对其进行操作，初始化时调用
* 输入参数  :dev:设备结构体，这里置为NULL
* 输出参数  :id:regulator的id或regulator提供的名字，通过id查找相对应的regulator
* 返回值：   A核:
*			 成功:获得的regulator结构体指针
*            失败:ID_ERR()能处理的errno
*			C核:
*			成功:获得的regulator结构体指针
*           失败:NULL
* 注        :各模块初始化时获取regulator，注销时释放regulator
*            获取regulator失败需要各模块进行相应的处理，如初始化失败
*****************************************************************************/
/* regulator get and put */
#ifdef __VXWORKS__
extern struct regulator *regulator_get(unsigned char *name, char *supply);


/*****************************************************************************
*函 数 名  : regulator_put
*功能描述  : 释放regulator资源。释放前需确认regulator_enable(),regulator_disable()
*			 成对使用
*输入参数  : regulator资源
*返回值	   ：
*
*****************************************************************************/
extern void regulator_put(struct regulator *regulator);

/*****************************************************************************
*函 数 名  : regulator_enable
*功能描述  : 使能regulator，必须和regulator_disable()成对使用
*输入参数  : regulator资源
*返回值	   ：成功: = 0
*			 失败: !0
*注		   : enable/disable 需要成对使用
*****************************************************************************/
/* regulator output control and status */
extern int regulator_enable(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_disable
*功能描述  : 去使能regulator，必须和regulator_enable()成对使用
*输入参数  : regulator资源
*返回值	   ：成功: = 0
*			 失败: !0
*注		   : enable/disable 需要成对使用
*****************************************************************************/
extern int regulator_disable(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_is_enabled
*功能描述  : regulator是否已使能
*输入参数  : regulator资源
*返回值	   ：成功: > 0
*			 失败: = 0
*
*****************************************************************************/
extern int regulator_is_enabled(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_get_voltage
*功能描述  : 获取regulator当前电压
*输入参数  : regulator资源
*返回值	   ：成功:当前电压值
*			 失败:负数
*****************************************************************************/
extern int regulator_get_voltage(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_set_voltage
*功能描述  : 设置regulator电压，如果设置成功会立即生效。
*输入参数  : regulator资源，min_uV，max_uV要设置的电压值
*返回值	   ：成功:0
*			 失败:负数
*****************************************************************************/
extern int regulator_set_voltage(struct regulator *regulator, int min_uV, int max_uV);

/*模式设置 NORMAL :正常供电 IDLE :该regulator进入eco模式*/
#ifndef REGULATOR_MODE_NORMAL
#define REGULATOR_MODE_NORMAL		0x2
#endif
#ifndef REGULATOR_MODE_IDLE
#define REGULATOR_MODE_IDLE			0x4
#endif
/*****************************************************************************
*函 数 名  : regulator_set_mode/regulator_get_mode
*功能描述  : 设置获取regulator模式
*输入参数  : regulator资源，mode要设置的模式
*返回值	   ：成功:0
*			 失败:负数
*注        : regulator暂时只提供开关、设置电压等功能，设置获取mode功能暂不实现
*****************************************************************************/
extern int regulator_set_mode(struct regulator *regulator, unsigned int mode);

extern unsigned int regulator_get_mode(struct regulator *regulator);

#endif
/******************************for regulator interface end****************************/

/******************************for regulator count start****************************/
enum REGULATOR_TYPE_NUM_ENUM
{
	REGULATOR_TYPE_HI6451 = 1,
	REGULATOR_TYPE_HI6551,
	REGULATOR_TYPE_HI6561,
	REGULATOR_TYPE_P531_MTCMOS,
	REGULATOR_TYPE_V7R2_MTCMOS = 5,
	REGULATOR_TYPE_HI6551_DR,
	REGULATOR_TYPE_K3 = 7,
	REGULATOR_TYPE_V711_MTCMOS,
	REGULATOR_TYPE_HI6559,
	REGULATOR_TYPE_HI6559_DR,
	REGULATOR_TYPE_OBSOLETE,
	REGULATOR_TYPE_END,
};

#define REGULATOR_TYPE_FLAG         (0xffff)
#define REGULATOR_SHIFT				(16)
#define HI6451_FLAG 				(REGULATOR_TYPE_HI6451 << REGULATOR_SHIFT)
#define HI6551_FLAG					(REGULATOR_TYPE_HI6551 << REGULATOR_SHIFT)
#define HI6561_FLAG					(REGULATOR_TYPE_HI6561 << REGULATOR_SHIFT)
#define P531_MTCMOS_FLAG			(REGULATOR_TYPE_P531_MTCMOS << REGULATOR_SHIFT)
#define V7R2_MTCMOS_FLAG			(REGULATOR_TYPE_V7R2_MTCMOS << REGULATOR_SHIFT)
#define V711_MTCMOS_FLAG			(REGULATOR_TYPE_V711_MTCMOS << REGULATOR_SHIFT)
#define HI6551_DR_FLAG				(REGULATOR_TYPE_HI6551_DR << REGULATOR_SHIFT)
#define K3_REGULATOR_FLAG			(REGULATOR_TYPE_K3 << REGULATOR_SHIFT)
#define HI6559_FLAG					(REGULATOR_TYPE_HI6559 << REGULATOR_SHIFT)
#define HI6559_DR_FLAG				(REGULATOR_TYPE_HI6559_DR << REGULATOR_SHIFT)
#define HI6559_OBSOLETE_FLAG			(REGULATOR_TYPE_OBSOLETE << REGULATOR_SHIFT)
enum P531_REGULATOR_COUNT_NUM
{
	P531_MTCMOS_COUNT1 = 1,
	P531_MTCMOS_COUNT2,
	P531_MTCMOS_COUNT3,
	P531_MTCMOS_COUNT4,
	P531_MTCMOS_COUNT5 = 5,
	P531_MTCMOS_COUNT6,
	P531_MTCMOS_COUNT7,
	P531_MTCMOS_COUNT8,
	P531_MTCMOS_COUNT9,
	P531_MTCMOS_COUNT10 = 10,
	P531_MTCMOS_COUNT_END
};
enum V7R2_REGULATOR_COUNT_NUM
{
	V7R2_MTCMOS_COUNT1 = 1,
	V7R2_MTCMOS_COUNT2,
	V7R2_MTCMOS_COUNT3,
	V7R2_MTCMOS_COUNT4,
	V7R2_MTCMOS_COUNT5 = 5,
	V7R2_MTCMOS_COUNT6,
	V7R2_MTCMOS_COUNT7,
	V7R2_MTCMOS_COUNT8,
	V7R2_MTCMOS_COUNT9,
	V7R2_MTCMOS_COUNT10 = 10,
	V7R2_MTCMOS_COUNT11,
	V7R2_MTCMOS_COUNT12,
	V7R2_MTCMOS_COUNT13,
	V7R2_MTCMOS_COUNT_END
};

enum V711_REGULATOR_COUNT_NUM
{
    V711_MTCMOS_COUNT_START,
	V711_MTCMOS_COUNT1 = 1,
	V711_MTCMOS_COUNT2,
	V711_MTCMOS_COUNT3,
	V711_MTCMOS_COUNT4,
	V711_MTCMOS_COUNT5 = 5,
	V711_MTCMOS_COUNT6,
	V711_MTCMOS_COUNT7,
	V711_MTCMOS_COUNT8,
	V711_MTCMOS_COUNT9,
	V711_MTCMOS_COUNT10 = 10,
	V711_MTCMOS_COUNT11,
	V711_MTCMOS_COUNT12,
	V711_MTCMOS_COUNT_END
};

/*
	HI6451 REGULATOR NUM
*/
#define HI6451_BASE_NUM				(0)
#define HI6451_BUCK1  				(HI6451_BASE_NUM + 1)
#define HI6451_BUCK2  				(HI6451_BASE_NUM + 2)
#define HI6451_BUCK3  				(HI6451_BASE_NUM + 3)
#define HI6451_BUCK4  				(HI6451_BASE_NUM + 4)
#define HI6451_BUCK5  				(HI6451_BASE_NUM + 5)
#define HI6451_BUCK6  				(HI6451_BASE_NUM + 6)
#define HI6451_LDO1   				(HI6451_BASE_NUM + 7)
#define HI6451_LDO2   				(HI6451_BASE_NUM + 8)
#define HI6451_LDO3   				(HI6451_BASE_NUM + 9)
#define HI6451_LDO4   				(HI6451_BASE_NUM + 10)
#define HI6451_LDO5   				(HI6451_BASE_NUM + 11)
#define HI6451_LDO6   				(HI6451_BASE_NUM + 12)
#define HI6451_LDO7   				(HI6451_BASE_NUM + 13)
#define HI6451_LDO8   				(HI6451_BASE_NUM + 14)
#define HI6451_LDO9   				(HI6451_BASE_NUM + 15)
#define HI6451_LDO10  				(HI6451_BASE_NUM + 16)
#define HI6451_LDO11  				(HI6451_BASE_NUM + 17)
#define HI6451_LDO12  				(HI6451_BASE_NUM + 18)
#define HI6451_LDO13  				(HI6451_BASE_NUM + 19)
#define HI6451_LDO14  				(HI6451_BASE_NUM + 20)
#define HI6451_LDO15  				(HI6451_BASE_NUM + 21)
#define HI6451_DR1    				(HI6451_BASE_NUM + 22)
#define HI6451_DR2    				(HI6451_BASE_NUM + 23)
#define HI6451_DR3    				(HI6451_BASE_NUM + 24)
#define NUM_OF_HI6451_REGULATOR 	(24)
#define HI6451_REGULATOR_MIN		HI6451_BUCK1
#define HI6451_REGULATOR_MAX		HI6451_DR3

//P531_MTCMOS_BASE_NUM ->24
#define P531_MTCMOS_BASE_NUM		(100)
#define P531_MTCMOS1				(P531_MTCMOS_BASE_NUM + 1)
#define P531_MTCMOS2				(P531_MTCMOS_BASE_NUM + 2)
#define P531_MTCMOS3				(P531_MTCMOS_BASE_NUM + 3)
#define P531_MTCMOS4				(P531_MTCMOS_BASE_NUM + 4)
#define P531_MTCMOS5				(P531_MTCMOS_BASE_NUM + 5)
#define P531_MTCMOS6				(P531_MTCMOS_BASE_NUM + 6)
#define P531_MTCMOS7				(P531_MTCMOS_BASE_NUM + 7)
#define P531_MTCMOS8				(P531_MTCMOS_BASE_NUM + 8)
#define P531_MTCMOS9				(P531_MTCMOS_BASE_NUM + 9)
#define P531_MTCMOS10				(P531_MTCMOS_BASE_NUM + 10)
#define NUM_OF_P531_MTCMOS			(10)
//HI6551_BASE_NUM ->34
#define HI6551_BASE_NUM				(200)
#define HI6551_BUCK1				(HI6551_BASE_NUM + 1)
#define HI6551_BUCK2				(HI6551_BASE_NUM + 2)
#define HI6551_BUCK3				(HI6551_BASE_NUM + 3)
#define HI6551_BUCK4				(HI6551_BASE_NUM + 4)
#define HI6551_BUCK5				(HI6551_BASE_NUM + 5)
#define HI6551_BUCK6				(HI6551_BASE_NUM + 6)
#define HI6551_BOOST				(HI6551_BASE_NUM + 7)
#define HI6551_LDO1					(HI6551_BASE_NUM + 8)
#define HI6551_LDO2					(HI6551_BASE_NUM + 9)
#define HI6551_LDO3					(HI6551_BASE_NUM + 10)
#define HI6551_LDO4					(HI6551_BASE_NUM + 11)
#define HI6551_LDO5					(HI6551_BASE_NUM + 12)
#define HI6551_LDO6					(HI6551_BASE_NUM + 13)
#define HI6551_LDO7					(HI6551_BASE_NUM + 14)
#define HI6551_LDO8					(HI6551_BASE_NUM + 15)
#define HI6551_LDO9					(HI6551_BASE_NUM + 16)
#define HI6551_LDO10				(HI6551_BASE_NUM + 17)
#define HI6551_LDO11				(HI6551_BASE_NUM + 18)
#define HI6551_LDO12				(HI6551_BASE_NUM + 19)
#define HI6551_LDO13				(HI6551_BASE_NUM + 20)
#define HI6551_LDO14				(HI6551_BASE_NUM + 21)
#define HI6551_LDO15				(HI6551_BASE_NUM + 22)
#define HI6551_LDO16				(HI6551_BASE_NUM + 23)
#define HI6551_LDO17				(HI6551_BASE_NUM + 24)
#define HI6551_LDO18				(HI6551_BASE_NUM + 25)
#define HI6551_LDO19				(HI6551_BASE_NUM + 26)
#define HI6551_LDO20				(HI6551_BASE_NUM + 27)
#define HI6551_LDO21				(HI6551_BASE_NUM + 28)
#define HI6551_LDO22				(HI6551_BASE_NUM + 29)
#define HI6551_LDO23				(HI6551_BASE_NUM + 30)
#define HI6551_LDO24				(HI6551_BASE_NUM + 31)
//#define HI6551_LVS1					(HI6551_BASE_NUM + 32)
#define HI6551_LVS2					(HI6551_BASE_NUM + 32)
#define HI6551_LVS3					(HI6551_BASE_NUM + 33)
#define HI6551_LVS4					(HI6551_BASE_NUM + 34)
#define HI6551_LVS5					(HI6551_BASE_NUM + 35)
#define HI6551_LVS6					(HI6551_BASE_NUM + 36)
#define HI6551_LVS7					(HI6551_BASE_NUM + 37)
#define HI6551_LVS8					(HI6551_BASE_NUM + 38)
#define HI6551_LVS9					(HI6551_BASE_NUM + 39)
#define HI6551_LVS10				(HI6551_BASE_NUM + 40)

#define NUM_OF_HI6551_REGULATOR		(40)
#define HI6551_REGULATOR_MIN		HI6551_BUCK1
#define HI6551_REGULATOR_MAX		HI6551_LVS10
//HI6561_BASE_NUM ->74
#define HI6561_BASE_NUM				(300)
#define HI6561_BUCK0				(HI6561_BASE_NUM + 1)
#define HI6561_BUCK1				(HI6561_BASE_NUM + 2)
#define HI6561_BUCK2				(HI6561_BASE_NUM + 3)
#define HI6561_LDO1					(HI6561_BASE_NUM + 4)
#define HI6561_LDO2					(HI6561_BASE_NUM + 5)

#define NUM_OF_HI6561_REGULATOR		(5)
#define HI6561_REGULATOR_MIN		HI6561_BUCK0
#define HI6561_REGULATOR_MAX		HI6561_LDO2
//V7R2_MTCMOS_BASE_NUM -> 79
#define V7R2_MTCMOS_BASE_NUM		(400)
#define V7R2_MTCMOS1				(V7R2_MTCMOS_BASE_NUM + 1)
#define V7R2_MTCMOS2				(V7R2_MTCMOS_BASE_NUM + 2)
#define V7R2_MTCMOS3				(V7R2_MTCMOS_BASE_NUM + 3)
#define V7R2_MTCMOS4				(V7R2_MTCMOS_BASE_NUM + 4)
#define V7R2_MTCMOS5				(V7R2_MTCMOS_BASE_NUM + 5)
#define V7R2_MTCMOS6				(V7R2_MTCMOS_BASE_NUM + 6)
#define V7R2_MTCMOS7				(V7R2_MTCMOS_BASE_NUM + 7)
#define V7R2_MTCMOS8				(V7R2_MTCMOS_BASE_NUM + 8)
#define V7R2_MTCMOS9				(V7R2_MTCMOS_BASE_NUM + 9)
#define V7R2_MTCMOS10				(V7R2_MTCMOS_BASE_NUM + 10)
#define V7R2_MTCMOS11				(V7R2_MTCMOS_BASE_NUM + 11)
#define V7R2_MTCMOS12				(V7R2_MTCMOS_BASE_NUM + 12)
#define V7R2_MTCMOS13				(V7R2_MTCMOS_BASE_NUM + 13)

#define NUM_OF_HI6551_MOCMOS		(13)

#define V711_MTCMOS_BASE_NUM		(450)
#define V711_MTCMOS1				(V711_MTCMOS_BASE_NUM + 1)
#define V711_MTCMOS2				(V711_MTCMOS_BASE_NUM + 2)
#define V711_MTCMOS3				(V711_MTCMOS_BASE_NUM + 3)
#define V711_MTCMOS4				(V711_MTCMOS_BASE_NUM + 4)
#define V711_MTCMOS5				(V711_MTCMOS_BASE_NUM + 5)
#define V711_MTCMOS6				(V711_MTCMOS_BASE_NUM + 6)
#define V711_MTCMOS7				(V711_MTCMOS_BASE_NUM + 7)
#define V711_MTCMOS8				(V711_MTCMOS_BASE_NUM + 8)
#define V711_MTCMOS9				(V711_MTCMOS_BASE_NUM + 9)
#define V711_MTCMOS10				(V711_MTCMOS_BASE_NUM + 10)
#define V711_MTCMOS11				(V711_MTCMOS_BASE_NUM + 11)
#define V711_MTCMOS12				(V711_MTCMOS_BASE_NUM + 12)

#define NUM_OF_V711_MTCMOS		    (12)

#define HI6551_DR_BASE_NUM		(500)
#define HI6551_DR1							(HI6551_DR_BASE_NUM + 1)
#define HI6551_DR2							(HI6551_DR_BASE_NUM + 2)
#define HI6551_DR3							(HI6551_DR_BASE_NUM + 3)
#define HI6551_DR4							(HI6551_DR_BASE_NUM + 4)
#define HI6551_DR5							(HI6551_DR_BASE_NUM + 5)
#define NUM_OF_HI6551_DR		(5)

#define K3_REGULATOR_BASE_NUM			(600)
#define K3_REGU_EFUSE								(K3_REGULATOR_BASE_NUM + 1)
#define K3_REGU_SIM0								(K3_REGULATOR_BASE_NUM + 2)
#define K3_REGU_SIM1								(K3_REGULATOR_BASE_NUM + 3)
#define K3_REGU_PASTAR							(K3_REGULATOR_BASE_NUM + 4)
#define NUM_OF_K3_REGULATOR				(4)

#define HI6559_REGULATOR_BASE_NUM			(700)
#define HI6559_BUCK0  										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_BUCK0)
#define HI6559_BUCK3  										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_BUCK3)
#define HI6559_BUCK4   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_BUCK4) 
#define HI6559_BUCK5   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_BUCK5) 
#define HI6559_BUCK6   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_BUCK6) 
#define HI6559_LDO01   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO01) 
#define HI6559_LDO03   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO03) 
#define HI6559_LDO06   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO06) 
#define HI6559_LDO07   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO07) 
#define HI6559_LDO08   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO08) 
#define HI6559_LDO09   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO09) 
#define HI6559_LDO10   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO10) 
#define HI6559_LDO11   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO11) 
#define HI6559_LDO12   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO12) 
#define HI6559_LDO13   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO13) 
#define HI6559_LDO14   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO14) 
#define HI6559_LDO22   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO22) 
#define HI6559_LDO23   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO23) 
#define HI6559_LDO24   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LDO24) 
#define HI6559_LVS07   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LVS07) 
#define HI6559_LVS09   										(HI6559_REGULATOR_BASE_NUM + PMIC_HI6559_LVS09) 
#define NUM_OF_HI6559_REGULATOR				(21)
#define HI6559_DR_BASE_NUM		(750)
#define HI6559_DR1							(HI6559_DR_BASE_NUM + PMIC_HI6559_DR01)
#define HI6559_DR2							(HI6559_DR_BASE_NUM + PMIC_HI6559_DR02)
#define HI6559_DR3							(HI6559_DR_BASE_NUM + PMIC_HI6559_DR03)
#define HI6559_DR4							(HI6559_DR_BASE_NUM + PMIC_HI6559_DR04)
#define HI6559_DR5							(HI6559_DR_BASE_NUM + PMIC_HI6559_DR05)
#define NUM_OF_HI6559_DR		(5)
/*for useless consumer*/
#define HI6559_OBSOLETE                     (9999)
#define NUM_OF_REGUALTORS			(NUM_OF_HI6451_REGULATOR + NUM_OF_P531_MTCMOS \
									+ NUM_OF_HI6551_REGULATOR + NUM_OF_HI6561_REGULATOR \
									+ NUM_OF_HI6551_MOCMOS + NUM_OF_K3_REGULATOR + NUM_OF_V711_MTCMOS \
									+ NUM_OF_HI6559_REGULATOR + NUM_OF_HI6559_DR + 1)

enum HI6421_REGULATOR_OPS_ID
{
	HI6421_OPS_ENABLE = 0,
	HI6421_OPS_DISABLE = 1,
	HI6421_OPS_IS_ENABLED = 2,
	HI6421_OPS_GET_VOLT = 3,
	HI6421_OPS_SET_VOLT = 4,
};
/******************************for regulator count end****************************/

#ifdef __cplusplus
}
#endif

#endif
