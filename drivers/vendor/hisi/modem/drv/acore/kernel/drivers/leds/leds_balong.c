

#ifdef __cplusplus
extern "C"
{
#endif
/*lint --e{527,529,533,537,752}*/
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/types.h>
#include "bsp_leds.h"
#include "bsp_regulator.h"
#include "bsp_pmu.h"
#include "bsp_nvim.h"
#include "bsp_icc.h"
#include "bsp_softtimer.h"
#include "leds_balong.h"

struct softtimer_list led_softtimer;                /* soft timer */
struct nv_led g_nv_led;                             /* store nv */
LED_CONTROL_NV_STRU g_led_state_str_lte[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG; /* 不同的状态对应的闪灯方案，功能与led nv一样 */
unsigned int led_debug_level;                       /* for LED_TRACE level */

extern struct led_tled_arg g_arg;    

static int __devinit balong_led_probe(struct platform_device *dev);
static int __devexit balong_led_remove(struct platform_device *pdev);
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata);

/* LED resources */
struct led_info balong_led[] = {
    [0] = {
        .name = LED1_NAME,
    },
    [1] = {
        .name = LED2_NAME,
    },
    [2] = {
        .name = LED3_NAME,
    },
    [3] = {
        .name = LED4_NAME,
    },
    [4] = {
        .name = LED5_NAME,
    }
};

/* LED platform data */
 struct led_platform_data led_plat_data = {
    .num_leds   = ARRAY_SIZE(balong_led),
    .leds       = (struct led_info *)balong_led,
};

/* LED platform device */
 struct platform_device balong_led_dev = {
    .name	= "balong_led",
	.id	    = -1,
	.dev 	= {
		.platform_data	= &led_plat_data,
	}
};

unsigned long us2ms(unsigned long us)
{
    return us / 1000;
}

/*lint -save -e413 -e613*/
/************************************************************************
 * Name         : limit_to_brightness
 * Function     : limit to brightness
 * Arguments
 *      input   : @led_id  - led id
 *              : @limit   - limit value
 *      output  : null
 *
 * Return       : brightness
 * Decription   : null
 ************************************************************************/
 int limit_to_brightness(int led_id, int limit)
{
    int brightness, limit_max;

    limit_max = bsp_dr_list_current(led_id, DR_CUR_NUMS - 1);
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, limit %d, limit max %d\n", __FUNCTION__, led_id, limit, limit_max);

    brightness = limit * LED_FULL / limit_max;  //DEBUG
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, brightness %d\n", __FUNCTION__, led_id, brightness);

    return brightness;
}

/************************************************************************
 * Name         : brightness_to_limit
 * Function     : brightness to limit
 * Arguments
 *      input   : @led_id         - led id
 *              : @led_brightness - brightness
 *      output  : null
 *
 * Return       : limit
 * Decription   : 把最大电流到最小电流划分为(LED_FULL + 1)个间隔，据此将led_brightness转换为limit,
 *                再根据DR可接受的8个电流档位，选择最接近的一个档位。
 ************************************************************************/
 int brightness_to_limit(int led_id, int led_brightness)
{
    unsigned i;
    int limit, limit_min, limit_max, limit_bigger, limit_smaller;

    /* compute limit */
    if(LED_OFF == led_brightness)
    {
        return 0;
    }
    else
    {
        limit_min = bsp_dr_list_current(led_id, 0);
        limit_max = bsp_dr_list_current(led_id, DR_CUR_NUMS - 1);

        limit = led_brightness * limit_max / LED_FULL; //DEBUG
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, brightness %d, limit min %d, limit max %d, limit %d\n",
            __FUNCTION__, led_id, led_brightness, limit_min,limit_max, limit);
    }

    /* find nearest limit level to use */
    for(i = 0; i < DR_CUR_NUMS; i++)
    {
        limit_bigger = bsp_dr_list_current(led_id, i);
        if(limit < limit_bigger)
        {
            break;
        }
    }
    limit_smaller = (0 == i) ? 0 : bsp_dr_list_current(led_id, i - 1);
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] limit smaller %d, limit bigger %d\n", __FUNCTION__, limit_smaller, limit_bigger);

    /* find nearest limit level, if limit is just in the middle of limit_bigger and limit_smaller, chose limit_bigger */
    limit = ((limit - limit_smaller) < (limit_bigger - limit)) ? limit_smaller : limit_bigger;
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] limit level %d\n", __FUNCTION__, limit);

    return limit;
}


/************************************************************************
 * Name         : name_to_id
 * Function     : get id of led_cdev
 * Arguments
 *      input   : @led_cdev - which led
 *      output  : null
 *
 * Return       : led id
 * Decription   : null
 ************************************************************************/
 int name_to_id(struct led_classdev *led_cdev)
{
    /* reject null */
    if(!led_cdev || !led_cdev->name)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led name %s\n", __FUNCTION__, led_cdev->name);

    if(!strcmp(led_cdev->name, LED1_NAME))
    {
        return LED1_ID;
    }
    else if(!strcmp(led_cdev->name, LED2_NAME))
    {
        return LED2_ID;
    }
    else if(!strcmp(led_cdev->name, LED3_NAME))
    {
        return LED3_ID;
    }
    else if(!strcmp(led_cdev->name, LED4_NAME))
    {
        return LED4_ID;
    }
    else
    {
        return LED5_ID;
    }

}

/************************************************************************
 * Name         : set_bre_time_valid
 * Function     : set breath time aviled
 * Arguments
 *      input   : @led_id   - led id
 *                @bre_time_enum - 要设置的参数类型
 *                @p_breathtime_ms - 要设置的参数值
 *      output  : @p_breathtime_ms - 要设置的参数值
 *
 * Return       : default breath on time(ms)
 * Decription   : 默认值此处参考Hi6551用户手册对应寄存器的默认值
 ************************************************************************/
int set_bre_time_valid(int led_id, dr_bre_time_e bre_time_enum, unsigned long *p_breathtime_ms)
{
    unsigned long value_num, selector;
    unsigned long temp_bigger = 0, temp_smaller, breath_time;
    int ret = LED_ERROR;

    /* 检查参数 */
    if(!p_breathtime_ms)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        goto ERRO;
    }

    value_num = bsp_dr_bre_time_selectors_get((dr_id_e)led_id, bre_time_enum);
    if(BSP_PMU_PARA_ERROR == value_num)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get %d value num failed\n", __FUNCTION__, led_id, bre_time_enum);
        goto ERRO;
    }

    /* 选择可用的档位值 */
    breath_time = *p_breathtime_ms;
    for(selector = 0; selector < value_num; selector++)
    {
        temp_bigger = bsp_dr_bre_time_list((dr_id_e)led_id, bre_time_enum, selector);
        if(BSP_PMU_PARA_ERROR == temp_bigger)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get breath time failed\n", __FUNCTION__, led_id);
            goto ERRO;
        }
        if(temp_bigger >= breath_time)
        {
            LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d bigger breath time %d\n", __FUNCTION__, led_id, temp_bigger);
            break;
        }        
    }

    if(value_num == selector)   /* 传入的值超过最大档位，就使用最大档位 */
    {
        LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]%d breath time is larger than max, breath_time %d, max value %d\n",
            __FUNCTION__, led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else if(0 == selector)      /* 传入的值小于最小档位，就使用最小档位 */
    {
        LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]%d breath time is samller than min, breath_time %d, min value %d\n",
            __FUNCTION__, led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else                        /* 传入的值介于最大档位和最小档位之间，选择最近的档位。如果恰位于两档正中间，使用较小档位 */
    {
        temp_smaller = bsp_dr_bre_time_list((dr_id_e)led_id, bre_time_enum, selector - 1);
        if(BSP_PMU_PARA_ERROR == temp_smaller)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get breath time failed\n", __FUNCTION__, led_id);
            goto ERRO;
        }
        *p_breathtime_ms = ((temp_bigger - breath_time) > (breath_time - temp_smaller) ? temp_smaller : temp_bigger);
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d breath time bigger %d, smaller %d\n", 
            __FUNCTION__, led_id, temp_bigger, temp_smaller);
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d breath time %d, choice %d\n", 
            __FUNCTION__, led_id, breath_time, *p_breathtime_ms);
    }

    return LED_OK;
ERRO:
    return ret;   
}

/************************************************************************
 * Name         : set_bre_mode
 * Function     : set breath mode
 * Arguments
 *      input   : @led_id   - led id
 *
 * Return       : 0 - success; else - failed
 * Decription   : DR1/2和DR3/4/5的呼吸模式配置不一样
 ************************************************************************/
int set_bre_mode(int led_id)
{
    int ret = LED_ERROR;
    
    switch(led_id)
    {        
        case LED1_ID:
        case LED2_ID:
            ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE_FLASH);
            break;
        case LED3_ID:
        case LED4_ID:
        case LED5_ID:
            ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE);
            break;
        default:
            break;
    };
    
    return ret;
}
/************************************************************************
 * Name         : balong_brightness_set
 * Function     : set brightness
 * Arguments
 *      input   : @led_cdev - which led
 *                @brightness - brightness, should between 0~255
 *      output  : null
 *
 * Return       : null
 * Decription   : 这个函数不能阻塞，可考虑使用任务队列
 ************************************************************************/
void balong_led_brightness_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);
    int led_id, limit, ret;

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return;
    }

    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] %s, brightness %d\n", __FUNCTION__, led_cdev->name, brightness);

    mutex_lock(&led_dev->pdata->mlock);

    /* brightness can't be too big or too small */
    if(brightness >= LED_FULL)
    {
        brightness = LED_FULL;
    }

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* save brightness */
    led_cdev->brightness = brightness;
    
    /* set limit */
    limit = brightness_to_limit(led_id, brightness);
    if(0 == limit)
    {
        /* if enabled, disable */
        if(regulator_is_enabled(led_dev->pdata->pregulator))
        {
            ret = regulator_disable(led_dev->pdata->pregulator);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] %s regulator_disable failed, ret = %d\n", 
                    __FUNCTION__, led_cdev->name, ret);
                goto EXIT;
            }
        }
        goto EXIT;
    }
    regulator_set_current_limit(led_dev->pdata->pregulator, limit, limit);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] %s set brightness %d limit %d\n", 
        __FUNCTION__, led_cdev->name, brightness, limit);

    /* enable led */
    if(!regulator_is_enabled(led_dev->pdata->pregulator))
    {
        ret = regulator_enable(led_dev->pdata->pregulator);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] %s regulator_enable failed, ret = %d\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return;
}

/************************************************************************
 * Name         : brightness_get
 * Function     : get brightness
 * Arguments
 *      input   : @led_cdev - which led
 *      output  : null
 *
 * Return       : brightness
 * Decription   : null
 ************************************************************************/
/*lint -save -e82, -e110, -e533*/
enum led_brightness balong_led_brightness_get(struct led_classdev *led_cdev)
{/*lint !e64*/
    /* argument check */
    if(!led_cdev)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return (enum led_brightness)LED_ERROR;  
    }
    
    return (enum led_brightness)led_cdev->brightness;   
}
/*lint -restore*/
/************************************************************************
 * Name         : balong_blink_set
 * Function     : set blink
 * Arguments
 *      input   : @led_cdev - which led
 *              : @delay_on - delay time when led is on
 *              : @delay_off - delay time when led is off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_blink_set(struct led_classdev *led_cdev, unsigned long *delay_on, unsigned long *delay_off)
{
    int led_id, ret;
    unsigned long local_delayon, local_delayoff;
    DR_FLA_TIME dr_fla_time;
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set LED_VALUE_INVALID */
    if(!delay_on)
    {
        local_delayon = LED_VALUE_INVALID;
        delay_on = &local_delayon;
    }    

    if(!delay_off)
    {        
        local_delayoff = LED_VALUE_INVALID;
        delay_off = &local_delayoff;
    }
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%s blink feriod %d, blink on %d\n", 
        __FUNCTION__, led_cdev->name, *delay_on + *delay_off, *delay_on);

    /* set blink */
    dr_fla_time.fla_off_us = *delay_off;
    dr_fla_time.fla_on_us = *delay_on;
    ret = bsp_dr_fla_time_set(&dr_fla_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set flash time failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }
    
    /* set blink mode */
    ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_FLA_FLASH);      
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set flash mode failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }    

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;
    
EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;
}

/************************************************************************
 * Name         : balong_led_breath_set
 * Function     : set
 * Arguments
 *      input   : @led_cdev - which led
 *              : @full_on - delay time(us) when led is on
 *              : @full_off - delay time(us) when led is off
 *              : @fade_on - delay time(us) when led is breathing from off to on
 *              : @fade_off - delay time(us) when led is breathing form on to off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_breath_set(struct led_classdev *led_cdev, unsigned long *full_on, unsigned long *full_off, unsigned long *fade_on, unsigned long *fade_off)
{/*lint !e64*/
    DR_BRE_TIME dr_bre_time;
    int led_id, ret;
    unsigned long local_fullon, local_fulloff, local_breon, local_breoff;
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set default value */
    if(!full_on)
    {
        local_fullon = LED_VALUE_INVALID;
        full_on = &local_fullon;
    }
    else
    {
        local_fullon = us2ms(*full_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_ON_MS, &local_fullon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

    if(!full_off)
    {
        local_fulloff = LED_VALUE_INVALID;
        full_off = &local_fulloff;
    }
    else
    {
        local_fulloff = us2ms(*full_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_OFF_MS, &local_fulloff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }
    
    if(!fade_on)
    {
        local_breon = LED_VALUE_INVALID;
        fade_on = &local_breon;
    }
    else
    {
        local_breon = us2ms(*fade_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_RISE_MS, &local_breon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }
    
    if(!fade_off)
    {
        local_breoff = LED_VALUE_INVALID;
        fade_off = &local_breoff;
    }
    else
    {
        local_breoff = us2ms(*fade_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_FALL_MS, &local_breoff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

    /* set time */
    dr_bre_time.bre_fall_ms = local_breoff;
    dr_bre_time.bre_off_ms = local_fulloff;
    dr_bre_time.bre_on_ms = local_fullon;
    dr_bre_time.bre_rise_ms = local_breon;
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d full_on %d, full_off %d, fade_on %d, fade_off %d\n",
        __FUNCTION__, led_id, dr_bre_time.bre_on_ms, dr_bre_time.bre_off_ms, dr_bre_time.bre_rise_ms, dr_bre_time.bre_fall_ms);
    
    ret = bsp_dr_bre_time_set((dr_id_e)led_id, &dr_bre_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }

    /* set breath mode */
    ret = set_bre_mode(led_id);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath mode, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;
    
EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;

}
/************************************************************************
 * Name         : register_led_by_nr
 * Function     : register led by number
 * Arguments
 *      input   : nr - LED id, from 0 to 4
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata)
{/*lint !e64*/
    /*lint -save -e409*/ 
    led[nr].pdata->led_breath_set      = balong_led_breath_set;
	led[nr].cdev.brightness_set        = balong_led_brightness_set;
    led[nr].cdev.brightness_get        = balong_led_brightness_get;
    led[nr].cdev.blink_set             = balong_led_blink_set;
	led[nr].cdev.name                  = pdata->leds[nr].name;

    /* regist regulator device */
    led[nr].pdata->pregulator = regulator_get(NULL, led[nr].cdev.name);
    if (IS_ERR(led[nr].pdata->pregulator))
    {
        pr_err("[%s] led%d get regulator failed\n", __FUNCTION__, nr + 1);
        return LED_ERROR;
    }

    /*lint -save -e539*/ 
	led[nr].cdev.default_trigger       = pdata->leds[nr].default_trigger;
	led[nr].cdev.flags                 = 0;
    /*lint -restore*/  
    
    /* mutex lock init */
    mutex_init(&led[nr].pdata->mlock);
    return LED_OK;
    /*lint -restore*/  
}

/************************************************************************
 * Name         : balong_led_probe
 * Function     : probe when init
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int __devinit balong_led_probe(struct platform_device *dev)
{
    struct led_platform_data *pdata = dev->dev.platform_data;
	struct balong_led_device *led   = NULL;
	int ret = LED_OK;
    unsigned int i;

	led = (struct balong_led_device *)kzalloc(sizeof(struct balong_led_device) * ARRAY_SIZE(balong_led), GFP_KERNEL);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get buffer failed\n", __FUNCTION__);
        goto ERRO;
    }    

    /* get pdata, register led and register classdev for every led device */
    for(i = 0; i < (int)ARRAY_SIZE(balong_led); i++)
    {
        led[i].pdata = (struct balong_led_platdata *)kzalloc(sizeof(struct balong_led_platdata), GFP_KERNEL);
        if(!led[i].pdata)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get pdata buffer failed\n", __FUNCTION__);
            goto ERRO;
        }

        /* regulator register failed is possible, as not all DRs are for LED */
        ret = register_led_by_nr(led, (int)i, pdata);
        if(!ret)
        {
        	ret = led_classdev_register(&dev->dev, &(led[i].cdev));
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]led%d sysfs register failed\n", __FUNCTION__, i + 1);
            }
        }

        if(LED3_ID <= i + 1)
        {
            /* set start delay */
            ret = bsp_dr_start_delay_set((dr_id_e)(i + 1), 0);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]set led%d start delay ERROR, ret = %d.\n", 
                    __FUNCTION__, i + 1, ret);
                goto ERRO;
            }        
        }

        
    }

    /* save "led" as private data */
    /*lint -save -e539*/ 
	platform_set_drvdata(dev, led);
    /*lint -restore*/ 

    /* timer create for three-color-led */
	led_softtimer.func = (softtimer_func)do_led_threecolor_flush;
	led_softtimer.para = 0;                              
	led_softtimer.timeout = 0;                        /* 定时长度，单位ms */
	led_softtimer.wake_type = SOFTTIMER_WAKE;
    
	if (bsp_softtimer_create(&led_softtimer))
	{
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]ERROR: softtimer create failed\n", __FUNCTION__);
		goto ERRO;
	}  
    
    /* read NV and store for three-color-led*/    
    if(g_nv_led.g_already_read != LED_NV_ALREADY_READ)
    {
    	ret = (int)bsp_nvm_read(NV_ID_DRV_LED_CONTROL,(u8 *)g_nv_led.g_led_state_str_om, LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
    	if(NV_OK != ret)
    	{
    		LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]WARNING: read nv failed, use default, ret = %d!\n", __FUNCTION__, ret);

            /* 读nv失败，采用默认值 */
            memcpy(g_nv_led.g_led_state_str_om, g_led_state_str_lte, 
                LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
    	}
    }    

    /* icc register for three-color-led*/
    ret = bsp_icc_event_register(LED_ICC_CHN_ID, led_threecolor_flush, NULL, NULL, NULL);
	if(ret != LED_OK)
	{
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] register icc event failed, ret = 0x%x\n", __FUNCTION__, ret);
        goto ERRO;
	}
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]: succeed!!!!!register icc callback\n", __FUNCTION__);

    return ret;
    
ERRO:
    for(i = 0; i < ARRAY_SIZE(balong_led); i++)
    {
        if(!led)
        {
            continue;
        }
    
        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }
    if(led)
    {
        kfree(led);
        led = NULL;
    }
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"LED init failed\n");
    return LED_ERROR; /*lint !e438*/
}

/************************************************************************
 * Name         : balong_led_remove
 * Function     : remove
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int __devexit balong_led_remove(struct platform_device *pdev)
{
    int i;
    struct balong_led_device *led = platform_get_drvdata(pdev);
    if(!led)
    {
        return LED_ERROR;
    }
    
    if(&led->cdev)
    {
        led_classdev_unregister(&led->cdev);
    }

    for(i = 0; i < (int)(ARRAY_SIZE(balong_led)); i++)
    {
        if(!led[i].pdata)
        {
            continue;
        }
        
        regulator_put(led[i].pdata->pregulator);
        led_classdev_unregister(&(led[i].cdev));
        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }

    kfree(led);
    led = NULL;

    return LED_OK; /*lint !e438*/
}

/*****************************************************************************
 函 数 名  : drv_led_flash
 功能描述  : 三色灯设置，给上层提供的接口
 输入参数  : status：三色灯的状态
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int drv_led_flash(unsigned long state) 
{
    int ret = LED_ERROR;
	struct balong_led_device *led = NULL;
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"get balong led device failed.\n");
        return LED_ERROR;
    }    

    g_arg.new_state = (unsigned char)state;
    g_arg.ctl = MNTN_LED_STATUS_FLUSH;    

    ret = do_led_threecolor_flush();
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]three color led flush failed, ret = %d!\n", __FUNCTION__, ret);
        return LED_ERROR;
    }

    return LED_OK;
}


/*************************************************************************/
/* for power manager */
#ifdef CONFIG_PM
  int balong_led_prepare(struct device *dev)
{
    /* ok, return */
    return 0;
}

  void balong_led_complete(struct device *dev)
{
    /* ok, return */
    return;
}

  int balong_led_suspend(struct device *dev)
{
    /* ok, return */
    return 0;
}

  int balong_led_resume(struct device *dev)
{
    /* ok, return */
    return 0;
}

 const struct dev_pm_ops balong_led_dev_pm_ops =
{
    .prepare    =   balong_led_prepare,
    .complete   =   balong_led_complete,
    .suspend    =   balong_led_suspend,
    .resume     =   balong_led_resume,
};

#define BALONG_LED_PM_OPS (&balong_led_dev_pm_ops)
#else
#define BALONG_LED_PM_OPS NULL
#endif

/* LED platform driver */
 struct platform_driver balong_led_drv = {
	.probe		= balong_led_probe,
	.remove		= balong_led_remove,
	.driver		= {
		.name		= "balong_led",
		.owner		= THIS_MODULE,
	    .bus        = &platform_bus_type,
		.pm         = BALONG_LED_PM_OPS,
	},
};

/*******************************************************************************
 * FUNC NAME:
 * balong_led_init() - register dirver and device for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver initialization interface.
 *
 * CALL FUNC:
 *
 ********************************************************************************/
int __init bsp_led_init(void)
{
    int result;

    result = platform_driver_register(&balong_led_drv);
    if (result < 0)
    {
        return result;
    }

    result = platform_device_register(&balong_led_dev);
    if (result < 0)
    {
        platform_driver_unregister(&balong_led_drv);
        return result;
    }

    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"LED init OK\n");

    return result;
}

/*******************************************************************************
 * FUNC NAME:
 * led_module_exit() - unregister dirver for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver de_initialization interface.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
static void __exit bsp_led_exit (void)
{
    platform_driver_unregister(&balong_led_drv);
    platform_device_unregister(&balong_led_dev);
}

/*****************************************************************************/

module_init(bsp_led_init);   /*lint !e19 */
module_exit(bsp_led_exit);       /*lint !e19 */

/************************************************************************/
/*
 * Just for DEBUG
 * 控制打印级别
 */
void led_set_debug_level(void)
{
    led_debug_level = LED_DEBUG_ALL;
}

void led_get_debug_level(void)
{
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "INFO    0x%08x\n", (unsigned int)LED_DEBUG_INFO);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "WARNING 0x%08x\n", (unsigned int)LED_DEBUG_WARNING);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "ERROR   0x%08x\n", (unsigned int)LED_DEBUG_ERROR);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "ALWAYS  0x%08x\n", (unsigned int)LED_DEBUG_ALWAYS);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "Current level 0x%08x\n", led_debug_level);
} 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<lusuo@hisilicon.com>");
MODULE_DESCRIPTION("BalongV700R200 Hisilicon LED driver");

#ifdef __cplusplus
}
#endif
