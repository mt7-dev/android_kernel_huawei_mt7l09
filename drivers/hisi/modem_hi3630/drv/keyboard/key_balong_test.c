/*
 * gpio_balong_test.c - hisilicon balong gpio driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/input/key_balong.h>
#include <linux/pm.h>


int test_key_int_disable(void)
{
    key_int_disable(POWER_KEY);
    key_int_disable(MENU_KEY);   
    key_int_disable(WIFI_KEY);
    key_int_disable(RESET_KEY);

    return 0;
}

int test_key_int_enable(void)
{
    key_int_enable(POWER_KEY);
    key_int_enable(MENU_KEY);
    key_int_enable(WIFI_KEY);
    key_int_enable(RESET_KEY);
    
    return 0;
}


void key_test(void)
{
    test_key_int_enable();
    test_key_int_disable();
}



