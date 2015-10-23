#include <asm/mach/irq.h>
#include <linux/interrupt.h>
#include "bsp_pmu.h"
#include "hi_smartstar.h"
#include "irq_hi6551.h"
#include "coul_hi6551.h"

extern unsigned short coul_voltage(void);
extern unsigned short coul_current(void);
extern unsigned int coul_capacity(void);

/*****************************************************************************
 函 数 名  : coul_battery_voltage
 功能描述  : 当前电压获取，单位:uV/1000
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned long long
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned long long coul_battery_voltage_test(void)
{
    unsigned short  val = 0;
    unsigned long long voltage = 0;

    val = coul_voltage();

    voltage = val * 225120;

    coul_dbg("coul_battery_voltage is %lld\n",voltage);

    return voltage;
}

/*****************************************************************************
 函 数 名  : coul_battery_current
 功能描述  : 当前电流获取,单位:uA/1000
 输入参数  : void
 输出参数  : 无
 返 回 值  : long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed long long  coul_battery_current_test(void)
{
    unsigned int  val = 0;
    signed long long  bat_current = 0;

    val = coul_current();

    bat_current = val * 468999;

    coul_dbg("coul_battery_current is %lld\n",bat_current);

    return bat_current;
}

/*****************************************************************************
 函 数 名  : coul_battery_capacity
 功能描述  : 电池电量获取,单位:uAh/1000
 输入参数  : void
 输出参数  : 无
 返 回 值  : long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed long long coul_battery_in_capacity_test(void)
{
    unsigned int  val = 0;
    signed long long capacity = 0;

    val = coul_in_capacity();

    capacity = val * 14330;

    coul_dbg("coul_battery_capacity is %lld\n",capacity);

    return capacity;
}
/*****************************************************************************
 函 数 名  : coul_battery_capacity
 功能描述  : 电池电量获取,单位:uAh/1000
 输入参数  : void
 输出参数  : 无
 返 回 值  : long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed long long coul_battery_out_capacity_test(void)
{
    unsigned int  val = 0;
    signed long long capacity = 0;

    val = coul_out_capacity();

    capacity = val * 14330;

    coul_dbg("coul_battery_capacity is %lld\n",capacity);

    return capacity;
}

