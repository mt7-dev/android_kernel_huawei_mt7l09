#ifndef __DRV_LEDS_H__
#define __DRV_LEDS_H__

#include "drv_comm.h"

/* 三色灯状态编号*/
enum
{
    LED_LIGHT_SYSTEM_STARTUP,
    LED_LIGHT_POWER_ON,
    LED_LIGHT_G_REGISTERED,
    LED_LIGHT_W_REGISTERED,
    LED_LIGHT_G_CONNECTED,
    LED_LIGHT_W_CONNNECTED,
    LED_LIGHT_H_CONNNECTED,
    LED_LIGHT_OFFLINE,
    LED_LIGHT_FORCE_UPDATING,
    LED_LIGHT_NORMAL_UPDATING,
    LED_LIGHT_UPDATE_FAIL,
    LED_LIGHT_UPDATE_SUCCESS,
    LED_LIGHT_UPDATE_FILEFAIL,
    LED_LIGHT_UPDATE_NVFAIL,
    LED_LIGHT_SIM_ABSENT,
    LED_LIGHT_SHUTDOWN,
    LED_LIGHT_G_PLMN_SEARCH,
    LED_LIGHT_W_PLMN_SEARCH,
    LED_LIGHT_L_REGISTERED,
    LED_LIGHT_STATE_MAX = 32
};/*the state of LED */


/****************************************************************************
 函 数 名  : drv_led_state_func_reg
 功能描述  : 本接口为提供给协议栈的钩子函数，用于获取协议栈的点灯状态。
 输入参数  : 协议栈获取点灯状态函数的指针。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项  : 无。

******************************************************************************/
void drv_led_state_func_reg (pFUNCPTR p);

#define DRV_LED_STATE_FUNREG(p)  drv_led_state_func_reg(p)

/*****************************************************************************
 函 数 名  : drv_led_flash
 功能描述  : 三色灯设置。
 输入参数  : status：三色灯的状态
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int drv_led_flash(unsigned long state);

#define DRV_LED_FLUSH(state)  drv_led_flash(state)  

/*****************************************************************************
 函 数 名  : BSP_MNTN_LedControlAddressGet
 功能描述  : 得到定制的点灯数据
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 点灯控制数据的全局变量地址
*****************************************************************************/
static INLINE unsigned int BSP_MNTN_LedControlAddressGet(void)
{
    return 0;
}
#define DRV_LED_GET_ADDRESS()   BSP_MNTN_LedControlAddressGet()

#endif /* #ifndef __DRV_LEDS_H__ */
