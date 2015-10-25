/*
 * Filename:kernel/drivers/input/keyboard/gpio_balong.c
 *
 * Discription:using gpio realizing reset-key and wps-key 
 * instead of KPC in kernel, only support simple key-press
 * at currunt version, not support combo-keys.
 *
 * Copyright (C) 2011 Hisilicon
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *
 * Revision history:
 */
/*lint -save -e537*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/pm.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/input/key_balong.h>

#include "linux/wakelock.h"
#include "bsp_om.h"
#include "bsp_pmu.h"
/*lint -restore */

/*gpio1处于掉电域，不带按键唤醒*/
#define WIFI_KEY_GPIO     (GPIO_1_0)
#define MENU_KEY_GPIO     (GPIO_0_16)
#define RESET_KEY_GPIO    (GPIO_0_18)

/*延时锁为100ms*/
#define WAKE_LOCK_TIME    (100)

#define  key_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_KEY, "[key]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  key_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_KEY, "[key]: "fmt, ##__VA_ARGS__))

typedef enum 
{
    RELEASE = 0,
    PRESS = 1,
    BUTT
}KEY_EVENT_TYPE;

/*lint -save -e958*/
struct gpio_button_data {
	const struct gpio_keys_button *button;
    struct wake_lock *key_wake_lock;
	struct input_dev *input;
	struct timer_list timer;
	unsigned int timer_debounce;	/* in msecs */
	unsigned int irq;
	spinlock_t lock;
	int disabled;
	int key_pressed;
};
/*lint -restore*/

struct v7r2_gpio_key {
	struct input_dev *input;
    struct wake_lock key_wake_lock;
    
    unsigned int suspended;
    unsigned int event_type;
    
    unsigned int n_buttons;
    struct gpio_button_data data[0];/*lint !e43*/
};

/******************************************************************************
*  Function:  key_int_disable
*  Description: 假开机或假关机时去使能menu键和Reset键的gpio中断
*  Called by:   
*  Input: item  :
*  Output:None
*  Return:None
*  Note  :      
*  History:    
********************************************************************************/
void key_int_disable(KEY_ENUM key)
{
    if(MENU_KEY == key)
    {
        gpio_int_mask_set(MENU_KEY_GPIO);
		gpio_int_state_clear(MENU_KEY_GPIO);
    }
    else if(RESET_KEY == key)
    {
        gpio_int_mask_set(RESET_KEY_GPIO);
		gpio_int_state_clear(RESET_KEY_GPIO);
    }
    else if(WIFI_KEY == key)
    {
        gpio_int_mask_set(WIFI_KEY_GPIO);
		gpio_int_state_clear(WIFI_KEY_GPIO);
    }
}

/******************************************************************************
*  Function:  key_int_enable
*  Description: 假开机或假关机时使能menu键和Reset键的gpio中断
*  Called by:   
*  Input: item  :
*  Output:None
*  Return:None
*  Note  :      
*  History:
********************************************************************************/
void key_int_enable(KEY_ENUM key)
{
    if(MENU_KEY == key)
    {
        gpio_int_state_clear(MENU_KEY_GPIO);
		gpio_int_unmask_set(MENU_KEY_GPIO);
    }
    else if(RESET_KEY == key)
    {
        gpio_int_state_clear(RESET_KEY_GPIO);
		gpio_int_unmask_set(RESET_KEY_GPIO);
    }
    else if(WIFI_KEY == key)
    {
        gpio_int_state_clear(WIFI_KEY_GPIO);
		gpio_int_unmask_set(WIFI_KEY_GPIO);
    }
}

void report_power_key_up(void *data)
{
    struct v7r2_gpio_key *gpio_key = (struct v7r2_gpio_key*)data;

	input_event(gpio_key->input, EV_KEY, KEY_POWER, 0);
	input_sync(gpio_key->input);
    
    wake_lock_timeout(&(gpio_key->key_wake_lock), (long)msecs_to_jiffies(WAKE_LOCK_TIME));
    key_print_info("power key is %s. \n", "on");   

}

void report_power_key_down(void *data)
{
    struct v7r2_gpio_key *gpio_key = (struct v7r2_gpio_key*)data;

    if(1 == gpio_key->suspended)
    {
        gpio_key->event_type = PRESS;
    }
    else
    {
    	input_event(gpio_key->input, EV_KEY, KEY_POWER, 1);
    	input_sync(gpio_key->input);
        
        wake_lock_timeout(&(gpio_key->key_wake_lock), (long)msecs_to_jiffies(WAKE_LOCK_TIME));
        key_print_info("power key is %s. \n", "down");
    }
}

static int v7r2_gpio_key_open(struct input_dev *dev)
{
	return 0;
}

static void v7r2_gpio_key_close(struct input_dev *dev)
{
	return;
}

static void gpio_keys_gpio_report_event(struct gpio_button_data *bdata)
{
	const struct gpio_keys_button *button = bdata->button;
	struct input_dev *input = bdata->input;
	unsigned int type = EV_KEY;

	input_event(input, type, button->code, bdata->key_pressed);

	input_sync(input);

    wake_lock_timeout(bdata->key_wake_lock, (long)msecs_to_jiffies(WAKE_LOCK_TIME));    
    key_print_info("%s is %s. \n", bdata->button->desc, bdata->key_pressed ? "down":"on");

}

static void gpio_keys_gpio_timer(unsigned long _data)
{
	struct gpio_button_data *bdata = (struct gpio_button_data *)_data;
    
    if((!gpio_get_value((unsigned)(bdata->button->gpio))) && bdata->key_pressed)
    {
        gpio_int_trigger_set((unsigned)(bdata->button->gpio), IRQ_TYPE_LEVEL_HIGH);
        gpio_keys_gpio_report_event(bdata);
        
    }
    else
    {
        bdata->key_pressed = 0;        
    }

    gpio_int_unmask_set((unsigned)(bdata->button->gpio));
}

static irqreturn_t gpio_keys_gpio_isr(int irq, void *dev_id)
{
	struct gpio_button_data *bdata = (struct gpio_button_data *)dev_id;
	
	if(!gpio_int_state_get((unsigned)(bdata->button->gpio)))
	{
		return IRQ_NONE;
	}

	gpio_int_mask_set((unsigned)(bdata->button->gpio));
	gpio_int_state_clear((unsigned)(bdata->button->gpio));

    if(!gpio_get_value((unsigned)(bdata->button->gpio)))
    {
        bdata->key_pressed = 1;
    }
    else
    {
        bdata->key_pressed = 0;
        gpio_int_trigger_set((unsigned)(bdata->button->gpio), IRQ_TYPE_LEVEL_LOW);
        gpio_keys_gpio_report_event(bdata);
    	gpio_int_unmask_set((unsigned)(bdata->button->gpio));
    }

	if (bdata->timer_debounce && bdata->key_pressed)
    {
		mod_timer(&bdata->timer, jiffies + msecs_to_jiffies(bdata->timer_debounce));/*考虑fiffies是否会溢出*/
    }

	return IRQ_HANDLED;
}

static void gpio_remove_key(struct gpio_button_data *bdata)
{
	free_irq(bdata->irq, bdata);
    
	if (bdata->timer_debounce)
		del_timer_sync(&bdata->timer);
    
	if (gpio_is_valid(bdata->button->gpio))
		gpio_free((unsigned)(bdata->button->gpio));
}

static int __devinit gpio_keys_setup_key(struct platform_device *pdev,
					 struct input_dev *input,
					 struct gpio_button_data *bdata,
					 const struct gpio_keys_button *button)
{
	const char *desc = button->desc ? button->desc : "v7r2_gpio_key";
	struct device *dev = &pdev->dev;
	int error;

	bdata->input = input;
	bdata->button = button;
	spin_lock_init(&bdata->lock);

	error = gpio_request((unsigned)(button->gpio), desc);
	if (error < 0) {
		dev_err(dev, "Failed to request GPIO %d, error %d\n",
			button->gpio, error);
		goto err_request_gpio;
	}

	gpio_direction_input((unsigned)(button->gpio));
	
    bdata->timer_debounce = (unsigned int)(button->debounce_interval);
    
    gpio_int_mask_set((unsigned)(button->gpio));
    
	/*根据gpio电平高低设置触发方式，高电平为抬起，低电平为按下*/
	if(gpio_get_value((unsigned)(button->gpio)))
    {
        bdata->key_pressed = 0;
    	gpio_int_trigger_set((unsigned)(button->gpio), IRQ_TYPE_LEVEL_LOW); /* 判断设置低电平触发*/
    }
    else
    {
        bdata->key_pressed = 1;
     	gpio_int_trigger_set((unsigned)(button->gpio), IRQ_TYPE_LEVEL_HIGH); /* 判断设置高电平触发*/
    }
	
	gpio_set_function((unsigned)(button->gpio), GPIO_INTERRUPT);
	
	error = request_irq((unsigned int)gpio_to_irq((unsigned)(button->gpio)), gpio_keys_gpio_isr, IRQF_NO_SUSPEND | IRQF_SHARED, bdata->button->desc, bdata);
	if (error) {
		dev_err(&pdev->dev, "Failed to request press interupt handler!\n");
		goto err_request_irq;
	}

	gpio_int_state_clear((unsigned)(button->gpio));
	gpio_int_unmask_set((unsigned)(button->gpio));
    
	setup_timer(&bdata->timer, gpio_keys_gpio_timer, (unsigned long)bdata);
    
	return 0;

err_request_irq:
    gpio_free((unsigned)(button->gpio));

err_request_gpio:
    
	return error;
}

/*lint -save -e830 -e438*/
static int __devinit v7r2_gpio_key_probe(struct platform_device* pdev)
{
	struct v7r2_gpio_key *gpio_key = NULL;
	struct input_dev *input = NULL;
    struct gpio_keys_platform_data *pdata = NULL;
	int err =0;
    int i = 0;
    
	key_print_info("v7r2 key driver probes start!\n");
    
	if (NULL == pdev) {
		key_print_error("parameter error!\n");
		err = -EINVAL;
		return err;
	}

    pdata = pdev->dev.platform_data; /*获取key平台数据*/
	if(NULL == pdata){
		dev_err(&pdev->dev,"Failed to get no platform data!\n");
		return -EINVAL;
	}

	gpio_key = kzalloc(sizeof(struct v7r2_gpio_key) +
			(unsigned int)(pdata->nbuttons) * sizeof(struct gpio_button_data), GFP_KERNEL);
	if (!gpio_key) {/*内存申请的大小打印*/
		dev_err(&pdev->dev, "Failed to allocate struct v7r2_gpio_key!\n");
		err = -ENOMEM;
		return err;
	}

    gpio_key->suspended = 0;

    gpio_key->event_type = BUTT;

	input = input_allocate_device();
	if (!input) {
		dev_err(&pdev->dev, "Failed to allocate struct input_dev!\n");
		err = -ENOMEM;
		goto err_alloc_input_device;
	}

	input->name = pdev->name;
	input->id.bustype = BUS_HOST;
	input->dev.parent = &pdev->dev;
	input_set_drvdata(input, gpio_key);
	set_bit(EV_KEY, input->evbit);
	set_bit(EV_SYN, input->evbit);
    set_bit(KEY_MENU, input->keybit);
    set_bit(KEY_F24, input->keybit);
    set_bit(KEY_POWER, input->keybit);
    
	input->open = v7r2_gpio_key_open;
	input->close = v7r2_gpio_key_close;

	gpio_key->input = input;

	for (i = 0; i < pdata->nbuttons; i++) {
		const struct gpio_keys_button *button = &pdata->buttons[i];
		struct gpio_button_data *bdata = &gpio_key->data[i];
        bdata->key_wake_lock = &(gpio_key->key_wake_lock);
        
		err = gpio_keys_setup_key(pdev, input, bdata, button);
		if (err)
			goto err_gpio_key;
	}

	err = input_register_device(gpio_key->input);
	if (err) {
		dev_err(&pdev->dev, "Failed to register input device!\n");
		goto err_register_device;
	}

    wake_lock_init(&(gpio_key->key_wake_lock), WAKE_LOCK_SUSPEND, "V7R2 KEY");

	/* get current state of buttons that are connected to GPIOs */
	for (i = 0; i < pdata->nbuttons; i++) {
		struct gpio_button_data *bdata = &gpio_key->data[i];
		if (gpio_is_valid(bdata->button->gpio))
			gpio_keys_gpio_report_event(bdata);
	}

    /*挂载PMU中断处理函数*/
    if(0 != bsp_pmu_irq_callback_register(PMU_INT_POWER_KEY_20MS_PRESS, report_power_key_down, gpio_key))
    {
		dev_err(&pdev->dev, "Failed to register pmu down irq!\n");
		goto err_register_device;
    }

    if(0 != bsp_pmu_irq_callback_register(PMU_INT_POWER_KEY_20MS_RELEASE, report_power_key_up, gpio_key))
    {
		dev_err(&pdev->dev, "Failed to register pmu up irq!\n");
		goto err_register_device;
    }
    
    /*power键初始状态上报Input事件*/
    if(bsp_pmu_key_state_get())
    {
        report_power_key_down(gpio_key);
    }
    else
    {
        report_power_key_up(gpio_key);
    }
    
	device_init_wakeup(&pdev->dev, (bool)1);
	platform_set_drvdata(pdev, gpio_key);

	key_print_info("v7r2 gpio key driver probes end!\n");

	return 0;

err_register_device:
    input_free_device(input);

err_gpio_key:
    while (--i >= 0)
		gpio_remove_key(&gpio_key->data[i]);

err_alloc_input_device:
	kfree(gpio_key);
	gpio_key = NULL;
	key_print_error("v7r2 gpio key probe failed! ret = %d.\n", err);
	return err;
}
/*lint -restore*/

static int __devexit v7r2_gpio_key_remove(struct platform_device* pdev)
{
	unsigned int i = 0;
    struct input_dev *input = NULL;
    
	struct v7r2_gpio_key *gpio_key = platform_get_drvdata(pdev);
    if(NULL == gpio_key)
    {
        key_print_error("platform_get_drvdata is fail.\n");
        return -1;
    }
    
	input = gpio_key->input;

	device_init_wakeup(&pdev->dev, (bool)0);

	for (i = 0; i < gpio_key->n_buttons; i++)
		gpio_remove_key(&gpio_key->data[i]);

	input_unregister_device(input);

	/*
	 * If we had no platform_data, we allocated buttons dynamically, and
	 * must free them here. ddata->data[0].button is the pointer to the
	 * beginning of the allocated array.
	 */
	if (!pdev->dev.platform_data)
		kfree(gpio_key->data[0].button);

	kfree(gpio_key);

	return 0;
}

#ifdef CONFIG_PM

static int v7r2_gpio_key_prepare(struct device *dev)
{
    return GPIO_OK;
}

static void v7r2_gpio_key_complete(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct v7r2_gpio_key *gpio_key = platform_get_drvdata(pdev);

	if(NULL == gpio_key){
		key_print_error("gpio_key is null pointer.\n");
		return ;
	}
    
	gpio_key->suspended = 0;
    
    if(PRESS == gpio_key->event_type)
    {
        /* keyboard event report to app */
        report_power_key_down(gpio_key);
        
        gpio_key->event_type = BUTT;
    }
    
    return ;
}

static int v7r2_gpio_key_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct v7r2_gpio_key *gpio_key = platform_get_drvdata(pdev);

	if(NULL == gpio_key){
		key_print_error("gpio_key is null pointer.\n");
		return -1;
	}
    
	gpio_key->suspended = 1;
	return 0;
}

static int v7r2_gpio_key_resume(struct device *dev)
{
   
	return 0;
}

static const struct dev_pm_ops v7r2_gpio_key_dev_pm_ops ={
	.suspend  = v7r2_gpio_key_suspend,
	.resume   = v7r2_gpio_key_resume,
	.prepare  = v7r2_gpio_key_prepare,
	.complete = v7r2_gpio_key_complete,
};

#define BALONG_DEV_PM_OPS (&v7r2_gpio_key_dev_pm_ops)

#else

#define BALONG_DEV_PM_OPS NULL

#endif

struct platform_driver v7r2_gpio_key_driver = {
	.probe = v7r2_gpio_key_probe,
	.remove = __devexit_p(v7r2_gpio_key_remove),
	.driver = {
		.name  = "v7r2_gpio_key",
		.owner = THIS_MODULE,
		.pm	   = BALONG_DEV_PM_OPS
	},

};

static struct gpio_keys_button gpio_keys_buttons[] = {
		{
			.code			= KEY_MENU,
			.gpio			= MENU_KEY_GPIO,
			.active_low		= 0,
			.desc			= "menu key",
			.type			= EV_KEY,
			.wakeup			= 1,
	 		.debounce_interval	= 20,
		},
		{
			.code			= KEY_F24,
			.gpio			= RESET_KEY_GPIO,
			.active_low		= 0,
			.desc			= "reset key",
	 		.type			= EV_KEY,
			.wakeup			= 1,
	 		.debounce_interval	= 20,
		},

/*udp单板wifi按键*/
#if 0
		{
			.code			= KEY_WLAN,
			.gpio			= WIFI_KEY_GPIO,
			.active_low		= 0,
			.desc			= "wifi key",
	 		.type			= EV_KEY,
			.wakeup			= 1,
	 		.debounce_interval	= 20,
		},
#endif

};

static struct gpio_keys_platform_data gpio_keys_data = {
	.buttons	= gpio_keys_buttons,
	.nbuttons	= ARRAY_SIZE(gpio_keys_buttons),/*lint !e30 !e806 !e84*/
	.rep		= 0,
};

static struct platform_device v7r2_gpio_key_device = {
	.name	= "v7r2_gpio_key",
	.id	= -1,
	.dev 	= {
		.platform_data	= &gpio_keys_data,
	}
};


static int __init v7r2_gpio_key_init(void)
{
    int ret;
    
	key_print_info("v7r2 gpio key init!\n");
    
	ret = platform_device_register(&v7r2_gpio_key_device);
	if(ret)
	{
	    key_print_error( "failed to register platform device!\n");
	    goto failed;
	}
	
    ret = platform_driver_register(&v7r2_gpio_key_driver);
    if(ret)
    {
	    platform_device_unregister(&v7r2_gpio_key_device);
        key_print_error( "failed to register platform driver!\n");
        goto failed;
    }

failed:
    
    return ret;
}

static void __exit v7r2_gpio_key_exit(void)
{
	key_print_info("v7r2 gpio key exit!\n");
    
	platform_driver_unregister(&v7r2_gpio_key_driver);
	platform_device_unregister(&v7r2_gpio_key_device);

}

module_init(v7r2_gpio_key_init);
module_exit(v7r2_gpio_key_exit);
MODULE_AUTHOR("Hisilicon V7r2 Driver Group");
MODULE_DESCRIPTION("v7r2 keypad platform driver");
MODULE_LICENSE("GPL");

