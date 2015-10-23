

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_leds.h"

#ifdef __KERNEL__
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include "leds_balong.h"

#define LED_NUMBER 5
#define LED_BLINK_PERIOD_MAX 1968750
#define LED_BLINK_FULLON_MAX 242203
#define LED_BREATH_FADEONOFF_MAX 4000000
#define LED_BREATH_FULLONOFF_MAX_DR12 4000000
#define LED_BREATH_FULLONOFF_MAX_DR345 16000000

#define sleep(s) msleep(s*1000)

extern LED_CONTROL_NV_STRU g_led_state_str_lte[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE];
extern struct platform_device balong_led_dev;
extern struct nv_led g_nv_led;
extern struct led_tled_arg g_arg;
extern void led_on(unsigned int led_id);
extern void led_off(unsigned int led_id);
extern void led_threecolor_state_switch(unsigned char new_color, unsigned char old_color);

/************************************************************************
 * Name         : led_test_case_001
 * Function     :
 * Arguments
 *      input   : led_id:0~4
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试id为led_id灯的亮度控制功能。
 ************************************************************************/
int led_test_case_001(int led_id)
{
    int ret = LED_ERROR;
    enum led_brightness brightness;
	struct balong_led_device *led = NULL;

    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"get balong led device failed.\n");
        return LED_ERROR;
    }

    /* test every brightness for a certain led, including two illegal brightness*/
    /*lint -save -e685*/ 
    for(brightness = LED_OFF; brightness <= LED_FULL ; brightness++)
    {
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"\n[%s] ===TEST=== led id %d, brightness %d\n",
                __FUNCTION__, led_id, brightness);

        led[led_id].cdev.brightness_set(&(led[led_id].cdev), brightness);   /* set brightness */            
        ret = led[led_id].cdev.brightness_get(&(led[led_id].cdev));
        if(ret != brightness)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, brightness set %d, get %d\n",
                    __FUNCTION__, led_id + 1, brightness, ret);
            return LED_ERROR;
        }
    }
    /*lint -restore*/  

    return LED_OK;
}

/************************************************************************
 * Name         : led_test_case_002
 * Function     :
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试每个led灯的亮度设置
 ************************************************************************/
int led_test_case_002(void)
{
    int led_id;
    int ret;
    for(led_id = 0; led_id < LED_ID_MAX; led_id++)
    {
        ret = led_test_case_001(led_id);
        if(ret != LED_OK)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, ret %d\n",
                    __FUNCTION__, led_id + 1, ret);
            return LED_ERROR;
        }
    }
    return LED_OK;
}

/************************************************************************
 * Name         : led_test_case_003
 * Function     :
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试led_id灯的闪烁模式控制功能
 ************************************************************************/
int led_test_case_003(int led_id)
{
    int ret = LED_ERROR;
    unsigned long full_on = LED_BLINK_FULLON_MAX, full_off = LED_BLINK_PERIOD_MAX - LED_BLINK_FULLON_MAX;
	struct balong_led_device *led = NULL;

    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get balong led device failed.\n");
        return LED_ERROR;
    }

    /* set brightness */
    ret = led[led_id].cdev.blink_set(&(led[led_id].cdev), &full_on, &full_off);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, set blink error, ret %d\n",
                __FUNCTION__, led_id + 1, ret);
        return LED_ERROR;
    }

    return LED_OK;
}
/************************************************************************
 * Name         : led_test_case_004
 * Function     :
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试每个led灯的呼吸模式控制功能
 ************************************************************************/
int led_test_case_004(void)
{
    int led_id;
    int ret;
    for(led_id = 0; led_id < LED_ID_MAX; led_id++)
    {
        ret = led_test_case_003(led_id);
        if(ret != LED_OK)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, ret %d\n",
                    __FUNCTION__, led_id + 1, ret);
            return LED_ERROR;
        }
    }
    
    return LED_OK;
}


/************************************************************************
 * Name         : led_test_case_005
 * Function     :
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试led_id灯的呼吸模式控制功能
 ************************************************************************/
int led_test_case_005(int led_id)
{
    int ret = LED_ERROR;
    unsigned long full_onoff, fade_onoff = LED_BREATH_FADEONOFF_MAX;
    
	struct balong_led_device *led = NULL;

    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get balong led device failed.\n");
        return LED_ERROR;
    }

    /* set brightness */
    if((0 == led_id) || (1 == led_id))
    {
        full_onoff = LED_BREATH_FULLONOFF_MAX_DR12;
    }
    else
    {
        full_onoff = LED_BREATH_FULLONOFF_MAX_DR345;
    }
    
    ret = led[led_id].pdata->led_breath_set(&(led[led_id].cdev), 
        &full_onoff, &full_onoff, &fade_onoff, &fade_onoff);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, set blink error, ret %d\n",
                __FUNCTION__, led_id + 1, ret);
        return LED_ERROR;
    }

    return LED_OK;
}

/************************************************************************
 * Name         : led_test_case_006
 * Function     :
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 测试每个led灯的呼吸模式控制功能
 ************************************************************************/
int led_test_case_006(void)
{
    int led_id;
    int ret;
    for(led_id = 0; led_id < LED_ID_MAX; led_id++)
    {
        ret = led_test_case_005(led_id);
        if(ret != LED_OK)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"\n[%s]ERROR:led id %d, ret %d\n",
                    __FUNCTION__, led_id + 1, ret);
            return LED_ERROR;
        }
    }
    
    return LED_OK;
}

/************************************************************************
 * Name         : do_led_tled_test_case
 * Function     : 测试A核三色灯闪烁功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 不涉及核间通信，只测试A核三色灯闪灯方案
 ************************************************************************/
int do_led_tled_test_case(unsigned long new_state)
{
    int ret = LED_ERROR;
	struct balong_led_device *led = NULL;
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"get balong led device failed.\n");
        goto ERRO;
    }    

    g_arg.new_state = (unsigned char)new_state;
    g_arg.ctl = MNTN_LED_STATUS_FLUSH;    

    ret = do_led_threecolor_flush();
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]three color led flush failed, ret = %d!\n", __FUNCTION__, ret);
        return ret;
    }

    return ret;
    
ERRO:
    return ret;  
}

/************************************************************************
 * Name         : led_tled_test_case_001
 * Function     : 测试A核三色灯绿灯双闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_001(void)
{
    return do_led_tled_test_case(LED_LIGHT_SYSTEM_STARTUP);
}

/************************************************************************
 * Name         : led_tled_test_case_002
 * Function     : 测试A核三色灯绿灯单闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_002(void)
{
    return do_led_tled_test_case(LED_LIGHT_G_REGISTERED);
}

/************************************************************************
 * Name         : led_tled_test_case_003
 * Function     : 测试A核三色灯绿灯频闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_003(void)
{
    return do_led_tled_test_case(LED_LIGHT_FORCE_UPDATING);
}

/************************************************************************
 * Name         : led_tled_test_case_004
 * Function     : 测试A核三色灯绿灯常亮功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_004(void)
{
    return do_led_tled_test_case(LED_LIGHT_G_CONNECTED);
}

/************************************************************************
 * Name         : led_tled_test_case_005
 * Function     : 测试A核三色灯蓝灯单闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_005(void)
{
    return do_led_tled_test_case(LED_LIGHT_W_REGISTERED);
}

/************************************************************************
 * Name         : led_tled_test_case_006
 * Function     : 测试A核三色灯蓝灯频闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_006(void)
{
    return do_led_tled_test_case(LED_LIGHT_UPDATE_NVFAIL);
}

/************************************************************************
 * Name         : led_tled_test_case_007
 * Function     : 测试A核三色灯蓝灯常亮功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_007(void)
{
    return do_led_tled_test_case(LED_LIGHT_W_CONNNECTED);
}

/************************************************************************
 * Name         : led_tled_test_case_008
 * Function     : 测试A核三色灯青灯单闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 在三色灯nv没有用过青灯单闪功能，这里把g_led_state_str_lte[18]拷贝到nv，用以测试
 ************************************************************************/
int led_tled_test_case_008(void)
{
    memcpy((void *)g_nv_led.g_led_state_str_om[LED_LIGHT_STATE_MAX - 1], (void *)g_led_state_str_lte[18], 
        (unsigned int)(LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU)));
    
    return do_led_tled_test_case(LED_LIGHT_STATE_MAX - 1);
}

/************************************************************************
 * Name         : led_tled_test_case_009
 * Function     : 测试A核三色灯青灯常亮功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_009(void)
{
    return do_led_tled_test_case(LED_LIGHT_H_CONNNECTED);
}

/************************************************************************
 * Name         : led_tled_test_case_010
 * Function     : 测试A核三色灯蓝绿交替闪烁功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_010(void)
{
    return do_led_tled_test_case(LED_LIGHT_UPDATE_FILEFAIL);
}

/************************************************************************
 * Name         : led_tled_test_case_011
 * Function     : 测试A核三色灯红灯双闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_011(void)
{
    LED_CONTROL_NV_STRU temp[LED_CONFIG_MAX_LTE] = LED_RED_LIGHTING_DOUBLE;

    memcpy((void *)g_nv_led.g_led_state_str_om[LED_LIGHT_STATE_MAX - 1], (void *)temp, 
        (unsigned int)(LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU)));

    return do_led_tled_test_case(LED_LIGHT_STATE_MAX - 1);
}

/************************************************************************
 * Name         : led_tled_test_case_012
 * Function     : 测试A核三色灯红灯单闪功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_012(void)
{
    LED_CONTROL_NV_STRU temp[LED_CONFIG_MAX_LTE] = LED_RED_LIGHTING_SIGNAL;

    memcpy((void *)g_nv_led.g_led_state_str_om[LED_LIGHT_STATE_MAX - 1], (void *)temp, 
        (unsigned int)(LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU)));

    return do_led_tled_test_case(LED_LIGHT_STATE_MAX - 1);
}

/************************************************************************
 * Name         : led_tled_test_case_013
 * Function     : 测试A核三色灯红灯常亮功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_013(void)
{
    LED_CONTROL_NV_STRU temp[LED_CONFIG_MAX_LTE] = LED_RED_LIGHTING_ALWAYS;

    memcpy((void *)g_nv_led.g_led_state_str_om[LED_LIGHT_STATE_MAX - 1], (void *)temp, 
        (unsigned int)(LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU)));

    return do_led_tled_test_case(LED_LIGHT_STATE_MAX - 1);
}

/************************************************************************
 * Name         : led_tled_test_case_014
 * Function     : 测试A核三色灯常暗功能
 * Arguments
 *      input   : new_state - new status
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 
 ************************************************************************/
int led_tled_test_case_014(void)
{
    return do_led_tled_test_case(LED_LIGHT_SHUTDOWN);
}

/************************************************************************
 * Name         : led_tled_test_case_015
 * Function     : 测试NV读取失败时，三色灯能否按照默认闪灯方案闪烁
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : 需要设置nv读失败的场景
 ************************************************************************/
int led_tled_test_case_015(void)
{
    return do_led_tled_test_case(LED_LIGHT_SYSTEM_STARTUP);
}

/************************************************************************
 * Name         : led_tled_test_case_017
 * Function     : 测试三色灯部分红灯点亮功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_017(void)
{
    RED_ON;
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_018
 * Function     : 测试三色灯部分红灯熄灭功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_018(void)
{
    RED_OFF;
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_019
 * Function     : 测试三色灯部分绿灯点亮功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_019(void)
{
    GREEN_ON;
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_020
 * Function     : 测试三色灯部分绿灯熄灭功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_020(void)
{
    GREEN_OFF;
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_021
 * Function     : 测试三色灯部分蓝灯点亮功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_021(void)
{
    BLUE_ON;
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_022
 * Function     : 测试三色灯部分蓝灯熄灭功能
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_022(void)
{
    BLUE_OFF;
    return LED_OK;
}

/*
 * led_tled_test_case_023分为4个小用例，可覆盖颜色切换的所有场景
 */

/************************************************************************
 * Name         : led_tled_test_case_023_1
 * Function     : 测试三色灯部分颜色变化功能 - 从白色到灭
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_023_1(void)
{
    /* power off */
    RED_OFF;
    GREEN_OFF;
    BLUE_OFF;
    led_threecolor_state_switch(LED_WHITE, LED_NULL);
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_023_2
 * Function     : 测试三色灯部分颜色变化功能 - 从灭到白色
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_023_2(void)
{
    /* power on */
    RED_ON;
    GREEN_ON;
    BLUE_ON;
    led_threecolor_state_switch(LED_NULL, LED_WHITE);
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_023_3
 * Function     : 测试三色灯部分颜色变化功能 - 从灭到灭
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_023_3(void)
{
    /* power off */
    RED_OFF;
    GREEN_OFF;
    BLUE_OFF;
    led_threecolor_state_switch(LED_NULL, LED_NULL);
    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_023_4
 * Function     : 测试三色灯部分颜色变化功能 - 从白色到白色
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_023_4(void)
{
    /* power on */
    RED_ON;
    GREEN_ON;
    BLUE_ON;
    led_threecolor_state_switch(LED_WHITE, LED_WHITE);
    return LED_OK;
}
#else /* __VXWORKS__ */

#include <drv_leds.h>
#include <bsp_om.h>

extern pFUNCPTR p_func_tled_state_get;
extern unsigned char g_new_work_state;

pFUNCPTR g_led_test_state = NULL;


/* 给上层注册的钩子函数会影响测试效果，测试之前先使钩子函数无效，待测试完成后再恢复，
 * 所以测试之前要先调用led_save_for_test()，待测试完成后调用led_resume_for_test()
 */
void led_save_for_test(void)
{
    if(g_led_test_state)
    {
        return;
    }

    g_led_test_state = p_func_tled_state_get;
    p_func_tled_state_get = NULL;
    return;
}

void led_resume_for_test(void)
{
    if(p_func_tled_state_get)
    {
        return;
    }
    
    p_func_tled_state_get = g_led_test_state;
    g_led_test_state = NULL;
    return;
}

/************************************************************************
 * Name         : do_led_tled_test_case
 * Function     : 
 * Arguments
 *      input   : @state - 工作状态
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int do_led_tled_test_case(unsigned long state)
{
    if(drv_led_flash(state))
    {
        return LED_ERROR;
    }

    return LED_OK;
}

/************************************************************************
 * Name         : led_tled_test_case_016
 * Function     : 测试三色灯部分颜色变化功能 - 从白色到白色
 * Arguments
 *      input   : @state - 工作状态
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 需要测试人员观察灯的状态来判断是否测试ok
 ************************************************************************/
int led_tled_test_case_016(unsigned long state)
{
    led_save_for_test();
    
    if(state < LED_LIGHT_STATE_MAX)
    {
        return do_led_tled_test_case(state);
    }
    else
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"ERROR: invalid state %d\n", state);
        return LED_ERROR;
    }            
}

/************************************************************************
 * Name         : led_tled_test_case_024
 * Function     : 测试三色灯部分颜色变化功能 - 从白色到白色
 * Arguments
 *      input   : @state - 工作状态
 *      output  : null
 *
 * Return       : 0 - ok
 * Decription   : 三色灯的状态应该不发生变化
 ************************************************************************/
int led_tled_test_case_024(void)
{
    led_save_for_test();
    return do_led_tled_test_case((unsigned long)g_new_work_state);
}


#endif
#ifdef __cplusplus
}
#endif
