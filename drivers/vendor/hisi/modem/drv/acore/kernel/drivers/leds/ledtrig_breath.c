/*
 * LED Kernel Timer Trigger
 *
 * Copyright 2005-2006 Openedhand Ltd.
 *
 * Author: Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
/*lint --e{438,527,529,537}*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/leds.h>
#include "bsp_leds.h"
#include "leds_balong.h"

static void led_breath_set(struct led_classdev *led_cdev, unsigned long *full_on, unsigned long *full_off, unsigned long *fade_on, unsigned long *fade_off)
{
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);

	del_timer_sync(&led_cdev->blink_timer);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",
        __FUNCTION__, led_cdev->name, *full_on, *full_off, *fade_on, *fade_off);

	if (led_dev && led_dev->pdata && led_dev->pdata->led_breath_set &&
	    !led_dev->pdata->led_breath_set(led_cdev, full_on, full_off, fade_on, fade_off))
		return;

    return;
}

static ssize_t led_fullon_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full on %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->full_on);

    if(led_dev && led_dev->pdata)
    {
        return snprintf(buf, LED_SYSFS_MAX_STR_LENTH, "%lu\n", led_dev->pdata->full_on);
    }

    return LED_OK;
}

static ssize_t led_fullon_store(struct device *dev,	struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
	int ret = -EINVAL;
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;

	if (isspace(*after))
		count++;

	if ((count == size) && led_cdev && led_dev && led_dev->pdata) {
        
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",
            __FUNCTION__, led_cdev->name, state, led_dev->pdata->full_off, led_dev->pdata->fade_on, led_dev->pdata->fade_off);

		led_breath_set(led_cdev, &state, &led_dev->pdata->full_off,
            &led_dev->pdata->fade_on, &led_dev->pdata->fade_off);
 		led_dev->pdata->full_on = state;
		ret = count;
	}

	return ret;
}

static ssize_t led_fulloff_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full off %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->full_off);

    if(led_dev && led_dev->pdata)
    {
        return snprintf(buf, LED_SYSFS_MAX_STR_LENTH, "%lu\n", led_dev->pdata->full_off);
    }

    return LED_OK;	
}

static ssize_t led_fulloff_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
	int ret = -EINVAL;
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;

	if (isspace(*after))
		count++;

	if ((count == size) && led_cdev && led_dev && led_dev->pdata) {
        
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->full_on, state,
            led_dev->pdata->fade_on, led_dev->pdata->fade_off);
        led_breath_set(led_cdev, &led_dev->pdata->full_on, &state,
            &led_dev->pdata->fade_on, &led_dev->pdata->fade_off);
        
		led_dev->pdata->full_off = state;
		ret = count;
	}

	return ret;
}

static ssize_t led_fadeon_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s fade on %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->fade_on);
    
    if(led_dev && led_dev->pdata)
    {
        return snprintf(buf, LED_SYSFS_MAX_STR_LENTH, "%lu\n", led_dev->pdata->fade_on);
    }

    return LED_OK;	
}

static ssize_t led_fadeon_store(struct device *dev,	struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
	int ret = -EINVAL;
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;

	if (isspace(*after))
		count++;

	if ((count == size) && led_cdev && led_dev && led_dev->pdata) {
        
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->full_on, 
            led_dev->pdata->full_off, state, led_dev->pdata->fade_off);
		led_breath_set(led_cdev, &led_dev->pdata->full_on, &led_dev->pdata->full_off,
            &state, &led_dev->pdata->fade_off);
        
		led_dev->pdata->fade_on = state;
		ret = count;
	}

	return ret;
}

static ssize_t led_fadeoff_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s fade off %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->fade_off);

    if(led_dev && led_dev->pdata)
    {
        return snprintf(buf, LED_SYSFS_MAX_STR_LENTH, "%lu\n", led_dev->pdata->fade_off);
    }
    
    return LED_OK;
}

static ssize_t led_fadeoff_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev);
	int ret = -EINVAL;
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;

	if (isspace(*after))
		count++;

	if ((count == size) && led_dev && led_dev->pdata) {
        
        LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",__FUNCTION__, led_cdev->name, led_dev->pdata->full_on, 
            led_dev->pdata->full_off, led_dev->pdata->fade_on, state);
		led_breath_set(led_cdev, &led_dev->pdata->full_on, &led_dev->pdata->full_off,
            &led_dev->pdata->fade_on, &state);
        
		led_dev->pdata->fade_off = state;
		ret = count;
	}

	return ret;
}

static DEVICE_ATTR(full_on, 0644, led_fullon_show, led_fullon_store);   /*lint !e24, !e26*/
static DEVICE_ATTR(full_off, 0644, led_fulloff_show, led_fulloff_store);
static DEVICE_ATTR(fade_on, 0644, led_fadeon_show, led_fadeon_store);
static DEVICE_ATTR(fade_off, 0644, led_fadeoff_show, led_fadeoff_store);

static void breath_trig_activate(struct led_classdev *led_cdev)
{
	int rc;
    struct balong_led_device *led_dev = (struct balong_led_device *)container_of(led_cdev, struct balong_led_device, cdev); 
    
    if(!led_cdev)
    {
        return;
    }

    /*lint -save -e539*/
	led_cdev->trigger_data = (void *)0;
    /*lint -restore */
    
	rc = device_create_file(led_cdev->dev, &dev_attr_full_on);
	if (rc)
		return;
	rc = device_create_file(led_cdev->dev, &dev_attr_full_off);
	if (rc)
		goto err_out_fulloff;
	rc = device_create_file(led_cdev->dev, &dev_attr_fade_on);
	if (rc)
		goto err_out_fadeon;
	rc = device_create_file(led_cdev->dev, &dev_attr_fade_off);
	if (rc)
		goto err_out_fadeoff;
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO), "[%s]%s full_on %d, full_off %d, fade_on %d, fade_off %d\n",__FUNCTION__, led_cdev->name, 
        led_dev->pdata->full_on, led_dev->pdata->full_off, led_dev->pdata->fade_on, led_dev->pdata->fade_off);

    if(led_dev && led_cdev && led_dev->pdata)
    {
        led_breath_set(led_cdev, &led_dev->pdata->full_on, &led_dev->pdata->full_off, &led_dev->pdata->fade_on, &led_dev->pdata->fade_off);
    }

    /*lint -save -e539*/
    led_cdev->trigger_data = (void *)1;
    /*lint -restore */
    
	return;

err_out_fadeoff:
	device_remove_file(led_cdev->dev, &dev_attr_fade_on);
err_out_fadeon:
	device_remove_file(led_cdev->dev, &dev_attr_full_off);
err_out_fulloff:
	device_remove_file(led_cdev->dev, &dev_attr_full_on);
}

static void breath_trig_deactivate(struct led_classdev *led_cdev)
{/*lint !e70*/
	if (led_cdev->trigger_data) {
		device_remove_file(led_cdev->dev, &dev_attr_full_on);
		device_remove_file(led_cdev->dev, &dev_attr_full_off);
		device_remove_file(led_cdev->dev, &dev_attr_fade_on);
		device_remove_file(led_cdev->dev, &dev_attr_fade_off);
	}

    /*lint -save -e539*/
	/* Stop breathing */
	led_brightness_set(led_cdev, LED_OFF);
    /*lint -restore */
}

static struct led_trigger breath_led_trigger = {
	.name     = "breath",
	.activate = breath_trig_activate,
	.deactivate = breath_trig_deactivate,
};

static int __init breath_trig_init(void)
{
	return led_trigger_register(&breath_led_trigger);
}

static void __exit breath_trig_exit(void)
{
	led_trigger_unregister(&breath_led_trigger);
}

module_init(breath_trig_init);  /*lint !e19 */
module_exit(breath_trig_exit);  /*lint !e19 */

MODULE_DESCRIPTION("Breath LED trigger");
MODULE_LICENSE("GPL");
