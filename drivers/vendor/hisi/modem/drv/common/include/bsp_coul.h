

#ifndef _BSP_COUL_H_
#define _BSP_COUL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "product_config.h"
#include "osl_types.h"
#ifdef __FASTBOOT__
#include "types.h"
#include <boot/boot.h>
#endif
#include "bsp_pmu.h"

/*中断号，后续考虑如何用pmu的宏*/
#if 0
typedef enum{
    COUL_INT = PMU_INT_COUL_INT,   /*cl_out>cl_int时上报中断*/
    COUL_OUT = PMU_INT_COUL_OUT,    /*cl_out计数到81.25%上报中断*/
    COUL_IN = PMU_INT_COUL_IN ,      /*cl_in计数到81.25%上报中断*/
    COUL_CHG_TIMER = PMU_INT_CHG_TIMER,  /*chg_timer计数到81.25%上报中断*/
    COUL_LOAD_TIMER = PMU_INT_LOAD_TIMER,/*load_timer计数到81.25%上报中断*/
    COUL_VBAT_INT = PMU_INT_VBAT_INT,    /*vbat电压<设定的vbat_int值*/
    COUL_INT_BUILT
}COUL_INT_TYPE;
#endif
typedef enum{
    COUL_INT = 25,   /*cl_out>cl_int时上报中断*/
    COUL_OUT = 26,    /*cl_out计数到81.25%上报中断*/
    COUL_IN = 27 ,      /*cl_in计数到81.25%上报中断*/
    COUL_CHG_TIMER = 28,  /*chg_timer计数到81.25%上报中断*/
    COUL_LOAD_TIMER = 29,/*load_timer计数到81.25%上报中断*/
    COUL_VBAT_INT = 30,    /*vbat电压<设定的vbat_int值*/
    COUL_INT_BUILT
}COUL_INT_TYPE;
/*eco mode去抖时间*/
typedef enum{
    COUL_FILETR_20MS = 0,
    COUL_FILETR_25MS,
    COUL_FILETR_30MS,
    COUL_FILETR_35MS,
    COUL_FILETR_BUILT
}COUL_FILETR_TIME;

/*函数声明*/
/*****************************************************************************
 函 数 名  : bsp_coul_voltage
 功能描述  : 当前电压获取，单位:mV
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_voltage(void);
/*****************************************************************************
 函 数 名  : bsp_coul_voltage_before
 功能描述  : 获取当前电压第times次电压值，单位:mV
 输入参数  : times:要获取的前第几次的电压值([范围[1,20])
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_voltage_before(unsigned int times);
/*****************************************************************************
 函 数 名  : bsp_coul_current
 功能描述  : 当前电流获取,单位:mA
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed int
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int  bsp_coul_current(void);
/*****************************************************************************
 函 数 名  : bsp_coul_current_before
 功能描述  : 获取当前电流前times次电流值，单位:mA
 输入参数  : times:要获取的前第几次的电流值([范围[1,20])
 输出参数  : 无
 返 回 值  : signed int
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int bsp_coul_current_before(unsigned int times);
/*****************************************************************************
 函 数 名  : bsp_coul_in_capacity
 功能描述  : 电池流入电量获取,单位:uC
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed long long
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned long long bsp_coul_in_capacity(void);
/*****************************************************************************
 函 数 名  : bsp_coul_out_capacity
 功能描述  : 电池流入电量获取,单位:uC
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed long long
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned long long bsp_coul_out_capacity(void);
/*****************************************************************************
 函 数 名  : bsp_coul_charge_time
 功能描述  : 充电时间获取,单位:s
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_charge_time(void);
/*****************************************************************************
 函 数 名  : bsp_coul_charge_time
 功能描述  : 充电时间获取,单位:s
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_discharge_time(void);
/*****************************************************************************
 函 数 名  : bsp_coul_eco_filter_time
 功能描述  : 设置库仑计eco滤波时间
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_eco_filter_time(COUL_FILETR_TIME filter_time);
/*****************************************************************************
 函 数 名  : coul_ocv_get
 功能描述  : 电池开路电压采样值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_ocv_get(void);
/*采样校准参数时用*/
/*****************************************************************************
 函 数 名  : bsp_coul_voltage_uncali
 功能描述  : 获取未经校准当前电压，单位:mV
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_voltage_uncali(void);
/*****************************************************************************
 函 数 名  : bsp_coul_current_uncali
 功能描述  : 获取未经校准当前电流,单位:mA
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int  bsp_coul_current_uncali(void);

#ifdef __KERNEL__
typedef void (*COUL_INT_FUNC)(void *);
/*****************************************************************************
 函 数 名  : bsp_coul_int_register
 功能描述  : 注册库仑计中断回调函数
 输入参数  : int:中断号；func:中断处理回调函数，data:回调函数参数指针
 输出参数  : 无
 返 回 值  : 注册成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_coul_int_register(COUL_INT_TYPE irq,COUL_INT_FUNC func,void *data);
#endif

#ifdef __FASTBOOT__
/*****************************************************************************
 函 数 名  : bsp_coul_init
 功能描述  : 库仑计模块初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 初始化成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_coul_init(void);
/*****************************************************************************
 函 数 名  : bsp_coul_ready
 功能描述  : 等待库仑计稳定
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_ready(void);
#endif

/*错误码*/
#define BSP_COUL_OK (0)
#define BSP_COUL_ERR (-1)
#define BSP_COUL_UNVALID (0xffffffff)

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_COUL_H_*/


