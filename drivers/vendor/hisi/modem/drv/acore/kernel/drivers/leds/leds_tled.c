
#include <linux/platform_device.h>
#include <drv_leds.h>
#include <bsp_nvim.h>
#include <bsp_icc.h>
#include <bsp_softtimer.h>
#include <bsp_leds.h>
#include "leds_balong.h"

struct led_param g_led_para = {0};                  /* led parameter */
struct led_tled_arg g_arg;                          /* 专门为do_led_threecolor_flush()传参 */
unsigned int g_led_old_state = LED_LIGHT_OFFLINE;   /* to store old status of three-color-light */

int led_threecolor_flush(u32 channel_id , u32 len, void* context);
int do_led_threecolor_flush(void);

extern struct platform_device balong_led_dev;
extern struct softtimer_list led_softtimer;        /* soft timer */
extern struct nv_led g_nv_led;                      /* store nv */
extern LED_CONTROL_NV_STRU g_led_state_str_lte[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE]; /* 不同的状态对应的闪灯方案，功能与led nv一样 */

/*******************************************************************************
 * FUNC NAME:
 * led_on() 
 * Function     : turn on a led
 * Arguments
 *      input   : @led_id - led id
 *      output  : null
 *
 * Return       : null
 * Decription   : 
 ************************************************************************/
void led_on(unsigned int led_id)
{
    unsigned long full_on = ALWAYS_ON_OFF_TIME_DR345* 1000; 
    unsigned long full_off = 0;
    unsigned long fade_on = 0;
    unsigned long fade_off = 0;
    
    struct balong_led_device *led = platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get drvdata failed\n", __FUNCTION__);
        return;
    }
    
    balong_led_breath_set(&led[led_id].cdev, &full_on, &full_off, &fade_on, &fade_off);
    balong_led_brightness_set(&led[led_id].cdev, LED_HALF);
    
    return;
}
/*******************************************************************************
 * FUNC NAME:
 * led_on() 
 * Function     : turn on a led
 * Arguments
 *      input   : @led_id - led id
 *      output  : null
 *
 * Return       : null
 * Decription   : 
 ************************************************************************/
void led_off(unsigned int led_id)
{
    unsigned long full_on = 0; 
    unsigned long full_off = ALWAYS_ON_OFF_TIME_DR345 * 1000;
    unsigned long fade_on = 0;
    unsigned long fade_off = 0;    

    struct balong_led_device *led = platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get drvdata failed\n", __FUNCTION__);
        return;
    }

    balong_led_breath_set(&led[led_id].cdev, &full_on, &full_off, &fade_on, &fade_off);
    
    return;
}

/*******************************************************************************
 * FUNC NAME:
 * led_threecolor_state_switch() - three color light status change
 * Function     : update three color light status
 * Arguments
 *      input   : @new_color - new color
 *              : @old_color - old color
 *      output  : null
 *
 * Return       : null
 * Decription   : before call this function, make sure that new_state is ligal and nv is gotten
 ************************************************************************/
void led_threecolor_state_switch(unsigned char new_color, unsigned char old_color)
{
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]tled status change, %d(old) to %d(new)\n", __FUNCTION__, old_color, new_color);
    
    if ((old_color & LED_RED) ^ (new_color & LED_RED))
    {
        if (new_color & LED_RED)
        {
            RED_ON;
        }
        else
        {
            RED_OFF;
        }
    }

    if ((old_color & LED_GREEN) ^ (new_color & LED_GREEN)) 
    {
        if (new_color & LED_GREEN)
        {
            GREEN_ON;
        }
        else
        {
            GREEN_OFF;
        }
    }

    if ((old_color & LED_BLUE) ^ (new_color & LED_BLUE)) 
    {
        if (new_color & LED_BLUE)
        {
            BLUE_ON;
        }
        else
        {
            BLUE_OFF;
        }
    }
    return;
}

/*******************************************************************************
 * FUNC NAME:
 * led_softtimer_modify()
 * Function     : modify softtimer time
 * Arguments
 *      input   : new_time  -   new time to set
 *      output  : null
 *
 * Return       : null
 * Decription   : null
 ************************************************************************/
void led_softtimer_modify_and_add(unsigned int new_time)
{
    int ret = LED_OK;
    static int i = 0;
    
    g_arg.ctl = MNTN_LED_TIMER_OCCURE;
    
    ret = bsp_softtimer_modify(&led_softtimer, new_time * LED_TIME_BASE_UNIT);
    i++;
    if(ret != LED_OK)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]ERROR: softtimer modify failed, ret = 0x%x, i = %d\n", 
            __FUNCTION__, ret, i);
        dump_stack();
        return;
    }
    bsp_softtimer_add(&led_softtimer);

    return;
}

/*******************************************************************************
 * FUNC NAME:
 * led_softtimer_delete()
 * Function     : delete softtimer
 * Arguments
 *      input   : null
 *      output  : null
 *
 * Return       : null
 * Decription   : null
 ************************************************************************/
void led_softtimer_delete(void)
{
    int ret;
    ret = bsp_softtimer_delete(&led_softtimer);
    if (ret != LED_OK)
    {
        LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]ERROR: softtimer delete failed, ret = %d\n", __FUNCTION__, ret);
	    return;
    }
}

/*******************************************************************************
 * FUNC NAME:
 * tled_need_flash() - 
 * Function     : 
 * Arguments
 *      input   : null,该函数使用g_led_param为参数
 *      output  : null
 *
 * Return       : 0 - false
 *              : else - true
 * Decription   : 判断特定的闪灯方案需不需要启动timer
 ************************************************************************/
int tled_need_flash(void)
{
    unsigned char new_state = g_led_para.led_state_id;

    /* 常暗/常亮时，闪灯序列的第2项时间和颜色都为0 */
    if(0 == g_nv_led.g_led_state_str_om[new_state][1].ucTimeLength
        && 0 == g_nv_led.g_led_state_str_om[new_state][1].ucLedColor)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*******************************************************************************
 * FUNC NAME:
 * do_led_threecolor_flush() - three color light status flush
 * Function     : update three color light status
 * Arguments
 *      input   : null,该函数使用g_arg和g_led_param为参数
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : before call this function, make sure that new_state is ligal and NV has been gotten
 ************************************************************************/
int do_led_threecolor_flush(void)
{
    unsigned char old_color = g_led_para.led_color_id;
    unsigned char new_state = g_arg.new_state;
    unsigned int ctl = g_arg.ctl;
    int ret = LED_ERROR;

    if(MNTN_LED_STATUS_FLUSH == ctl)    /* 接受从c核发来的新状态，并根据NV值确定新的闪灯方案 */
    {
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]tled status flash\n", __FUNCTION__);
        
        g_led_para.led_state_id = new_state;
        g_led_para.led_config_id = 0;
        g_led_para.led_color_id = g_nv_led.g_led_state_str_om[new_state][0].ucLedColor;
        g_led_para.led_time = g_nv_led.g_led_state_str_om[new_state][0].ucTimeLength;

        /* timer can't be on-list before modified*/
        if(!list_empty(&led_softtimer.entry))
        {
            ret = bsp_softtimer_delete(&led_softtimer);
            if (ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]ERROR: softtimer delete failed, ret = %d\n", __FUNCTION__, ret);
        	    return LED_ERROR;
            }
        }

        /* 闪灯方案中有亮灭的变化，才启动timer */
        if(tled_need_flash())
        {
            led_softtimer_modify_and_add(g_led_para.led_time);
        }
               
    }
    else if(MNTN_LED_TIMER_OCCURE == ctl)/* 同一个闪灯方案里，timer触发灯的状态(颜色、亮灭等)改变 */
    {
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]tled timer occure\n", __FUNCTION__);

        if((g_led_para.led_config_id >= (LED_CONFIG_MAX_LTE - 1))
            || (0 == g_nv_led.g_led_state_str_om[g_led_para.led_state_id][g_led_para.led_config_id + 1].ucTimeLength))
        {
            g_led_para.led_config_id = 0;
        }else{

            /* save old color */
            old_color = g_nv_led.g_led_state_str_om[g_led_para.led_state_id][g_led_para.led_config_id].ucLedColor;

            g_led_para.led_config_id++;
        }
        
        g_led_para.led_color_id = g_nv_led.g_led_state_str_om[g_led_para.led_state_id][g_led_para.led_config_id].ucLedColor;
        g_led_para.led_time = g_nv_led.g_led_state_str_om[g_led_para.led_state_id][g_led_para.led_config_id].ucTimeLength;

        led_softtimer_modify_and_add(g_led_para.led_time);
        
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]new color %d, new time(*100 ms) %d\n", 
            __FUNCTION__, g_led_para.led_color_id, g_led_para.led_time);
        
    }else{    
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]unknown ctrl: %d\n", __FUNCTION__, ctl);
        return LED_ERROR;
    }
    
    led_threecolor_state_switch(g_led_para.led_color_id, old_color);
    
    return LED_OK;
}

/*******************************************************************************
 * FUNC NAME:
 * led_threecolor_flush() - three color light flush
 * Function     : update three color light status
 * Arguments
 *      input   : @newstatus - new status
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : prepare for update led status
 ************************************************************************/
int led_threecolor_flush(u32 channel_id , u32 len, void* context)
{
    int ret = LED_ERROR;
    int length = 0;
    unsigned char new_state;
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "LED acore begin to flush state\n");

    /* get new status from c core */
    length = bsp_icc_read(LED_ICC_CHN_ID, &new_state, sizeof(unsigned char));
	if(length != (int)sizeof(unsigned char))
	{
		LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]icc read failed, length = 0x%x\n", __FUNCTION__, length);
		return LED_ERROR;
	}

    LED_TRACE(LED_DEBUG_LEVEL(INFO), "LED icc read OK, new state %d\n", new_state);

    /* check whether new status is illigal */
    if(new_state >= LED_LIGHT_STATE_MAX)
    {
		LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]illigal new status, new_state = %d!\n", __FUNCTION__, new_state);
		return LED_ERROR;
    }    
    
    /* update led status */
    /* 这里do_led_threecolor_flush要作为timer的注册函数，只能有一个unsigned int 类型的参数，故将参数封装一个结构体 */
    g_arg.new_state = new_state;
    g_arg.ctl = MNTN_LED_STATUS_FLUSH;
    ret = do_led_threecolor_flush();
    if(ret)
    {
		LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]three color led status update failed, ret = 0x%x\n", __FUNCTION__, ret);
		return LED_ERROR;
    }

    /* return */
    return ret;
}

