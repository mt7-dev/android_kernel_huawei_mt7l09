
#ifndef __DRV_REGULATOR_H__
#define __DRV_REGULATOR_H__

#include "drv_comm.h"

#ifndef __CMSIS_RTOS

struct regulator;
/*************************REGULATOR BEGIN***********************************/


/*****************************************************************************
*函 数 名  : regulator_get
* 功能描述  :查找并获得regulator，用于后面对其进行操作，初始化时调用
* 输入参数  :dev:设备结构体，这里置为NULL
* 输出参数  :id:regulator的id或regulator提供的名字，通过id查找相对应的regulator
* 返回值：   A核:
*			 成功:获得的regulator结构体指针
*            失败:ID_ERR()能处理的errno
*			 C核:
*			 成功:获得的regulator结构体指针
*            失败:NULL
*注         :各模块初始化时获取regulator，注销时释放regulator
*            获取regulator失败需要各模块进行相应的处理，如初始化失败
*****************************************************************************/
/* regulator get and put */
#ifdef __KERNEL__
extern struct regulator *regulator_get(struct device *dev, const char *id);
#elif defined(__VXWORKS__)


extern struct regulator *regulator_get(unsigned char *name, char *supply);
#endif

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
*返回值	   ：成功: >= 0
*			 失败: < 0
*注		   : enable/disable 需要遵守成对使用约束 即需要先有使能，后方可去使能
*			 且使能、去使能总次数应相同
*			 特殊情况可先用regulator_is_enabled判断当前状态，后按约束使用
*****************************************************************************/
extern int regulator_enable(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_disable
*功能描述  : 去使能regulator，必须和regulator_enable()成对使用
*输入参数  : regulator资源
*返回值	   ：成功: >= 0
*			 失败: < 0
*注		   : enable/disable 需要遵守成对使用约束 即需要先有使能，后方可去使能
*			 且使能、去使能总次数应相同
*			 特殊情况可先用regulator_is_enabled判断当前状态，后按约束使用
*****************************************************************************/
extern int regulator_disable(struct regulator *regulator);
/*****************************************************************************
*函 数 名  : regulator_is_enabled
*功能描述  : regulator是否已使能
*输入参数  : regulator资源
*返回值	   ：成功: > 0
*			 失败: = 0
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
*功能描述  : 设置regulator电压
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

#endif /* __CMSIS_RTOS*/

#endif

