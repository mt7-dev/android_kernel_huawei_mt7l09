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
#include <linux/irq.h>
#include <linux/pm.h>

#include "hi_gpio.h"
#include "osl_bio.h"
#include "soc_memmap.h"
#include "bsp_memmap.h"
#include "soc_interrupts.h"

/*for GPIO UT by liuwenhui*/
#define GPIO_TEST_NUM                      (GPIO_0_13)
#define GPIO_BANK_BITS                     (0)
#define GPIO_PIN_BITS                      (13)
#define GPIO_TEST_IRQ                      (INT_LVL_APP_GPIO0)
#define GPIO_TEST_ADDR                     (HI_GPIO0_REGBASE_ADDR_VIRT)

#define GPIO_INVALID_NUM                   (10000)

#define gpio_reg_set_bits(base, reg, pos, bits, val) (writel((readl(base + reg) & (~(((0x1 << bits) - 1) << pos))) | ((val & ((0x1 << bits) - 1)) << pos), base + reg))
#define gpio_reg_get_bits(base, reg, pos) ((readl(base + reg) & (0x1 << pos)) >> pos)

int test_gpio_init(void);

int test_gpio_value_get_valid_value(void);
int test_gpio_value_set_valid_value(void);
int test_gpio_direction_get_valid_value(void);
int test_gpio_direction_output_valid_value(void);
int test_gpio_direction_input_valid_value(void);
int test_gpio_to_irq_valid_value(void);
int test_gpio_int_mask_set_valid_value(void);
int test_gpio_int_unmask_set_valid_value(void);
int test_gpio_set_function_valid_value(void);
int test_gpio_int_state_get_valid_value(void);
int test_gpio_int_state_clear_valid_value(void);
int test_gpio_raw_int_state_get_valid_value(void);
int test_gpio_int_trigger_set_valid_value(void);
int test_gpio_direction_output(void);
int test_gpio_output_get_value(void);
int test_gpio_set_value(void);
int test_gpio_to_irq(void);
int test_gpio_direction_get(void);
int test_gpio_direction_input(void);
int test_gpio_input_set_value(void);
int test_gpio_get_value(void);
int test_gpio_int_mask_set(void);
int test_gpio_int_trigger_set(void);
int test_gpio_set_function(void);
int test_gpio_int_unmask_set(void);
int test_gpio_int_state_get(void);
int test_gpio_int_state_clear(void);
int test_gpio_raw_int_state_get(void);

int test_gpio_direction_input(void)
{
    gpio_direction_input(GPIO_TEST_NUM);

    if(GPIO_INPUT == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_direction_input is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_direction_input is fail!!\n");
        return GPIO_ERROR;
    }

}


int test_gpio_direction_output(void)
{
    if(GPIO_ERROR == gpio_direction_output(GPIO_TEST_NUM,0))
    {
        printk(KERN_INFO "gpio_direction_output is fail!!\n");
        return GPIO_ERROR;
    }

    if(GPIO_OUTPUT == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_direction_output is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_direction_output is fail!!\n");
        return GPIO_ERROR;
    }

}

int test_gpio_get_value(void)
{
    if(gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_EXT_PORT_OFFSET,GPIO_PIN_BITS) == gpio_get_value(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_get_value is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_get_value is fail!!\n");
        return GPIO_ERROR;
    }

}


int test_gpio_set_value(void)
{
    gpio_set_value(GPIO_TEST_NUM,0);

    if(1 == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DR_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_set_value is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_set_value(GPIO_TEST_NUM,1);
    if(0 == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DR_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_set_value is fail!!\n");
        return GPIO_ERROR;
    }

    printk(KERN_INFO "test_gpio_set_value is passed!!\n");
    return GPIO_OK;
}


int test_gpio_to_irq(void)
{
    if(GPIO_TEST_IRQ == gpio_to_irq(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_to_irq is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_to_irq is fail!!\n");
        return GPIO_ERROR;
    }

}



int test_gpio_direction_get(void)
{
    gpio_reg_set_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS,1,GPIO_INPUT);

    if(GPIO_OUTPUT == gpio_direction_get(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_direction_get is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_reg_set_bits(GPIO_TEST_ADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS,1,GPIO_OUTPUT);
    if(GPIO_INPUT == gpio_direction_get(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_direction_get is fail!!\n");
        return GPIO_ERROR;
    }
    printk(KERN_INFO "test_gpio_direction_get is passed!!\n");
    return GPIO_OK;
}


int test_gpio_int_mask_set(void)
{
    gpio_int_mask_set(GPIO_TEST_NUM);

    if(GPIO_INT_ENABLE == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTMASK_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_mask_set is fail!!\n");
        return GPIO_ERROR;
    }
    printk(KERN_INFO "test_gpio_int_mask_set is passed!!\n");
    return GPIO_OK;
}


int test_gpio_int_unmask_set(void)
{
    gpio_int_unmask_set(GPIO_TEST_NUM);

    if(GPIO_INT_DISABLE == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTMASK_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_unmask_set is fail!!\n");
        return GPIO_ERROR;
    }
    printk(KERN_INFO "test_gpio_int_unmask_set is passed!!\n");
    return GPIO_OK;
}


int test_gpio_set_function(void)
{
    gpio_set_function(GPIO_TEST_NUM,GPIO_INTERRUPT);
    if(GPIO_NORMAL == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTEN_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_set_function is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_set_function(GPIO_TEST_NUM,GPIO_NORMAL);

    if(GPIO_INTERRUPT == gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTEN_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_set_function is fail!!\n");
        return GPIO_ERROR;
    }

    printk(KERN_INFO "test_gpio_set_function is passed!!\n");
    return GPIO_OK;
}


int test_gpio_int_state_get(void)
{
    if(gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTSTATUS_OFFSET,GPIO_PIN_BITS) == gpio_int_state_get(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_int_state_get is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_int_state_get is fail!!\n");
        return GPIO_ERROR;
    }


}


int test_gpio_int_state_clear(void)
{
    printk(KERN_INFO "gpio_int_state_clear is passed!!\n");
    return GPIO_OK;
}


int test_gpio_raw_int_state_get(void)
{
    if(gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_RAWINTSTATUS_OFFSET,GPIO_PIN_BITS) == gpio_raw_int_state_get(GPIO_TEST_NUM))
    {
        printk(KERN_INFO "test_gpio_int_state_get is passed!!\n");
        return GPIO_OK;
    }
    else
    {
        printk(KERN_INFO "test_gpio_int_state_get is fail!!\n");
        return GPIO_ERROR;
    }

}


int test_gpio_int_trigger_set(void)
{
    gpio_int_trigger_set(GPIO_TEST_NUM,IRQ_TYPE_EDGE_RISING);
    if(GPIO_INT_TYPE_EDGE != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
        && GPIO_INT_POLARITY_RIS_HIGH != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_trigger_set is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_int_trigger_set(GPIO_TEST_NUM,IRQ_TYPE_EDGE_FALLING);
    if(GPIO_INT_TYPE_EDGE != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
        && GPIO_INT_POLARITY_FAL_LOW != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_trigger_set is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_int_trigger_set(GPIO_TEST_NUM,IRQ_TYPE_LEVEL_HIGH);
    if(GPIO_INT_TYPE_LEVEVL != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
        && GPIO_INT_POLARITY_RIS_HIGH != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_trigger_set is fail!!\n");
        return GPIO_ERROR;
    }

    gpio_int_trigger_set(GPIO_TEST_NUM,IRQ_TYPE_LEVEL_LOW);
    if(GPIO_INT_TYPE_LEVEVL != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
        && GPIO_INT_POLARITY_FAL_LOW != gpio_reg_get_bits(GPIO_TEST_ADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
    {
        printk(KERN_INFO "test_gpio_int_trigger_set is fail!!\n");
        return GPIO_ERROR;
    }

    printk(KERN_INFO "test_gpio_int_trigger_set is passed!!\n");

    return GPIO_OK;

}

extern struct gpio_chip *gpio_to_chip(unsigned gpio);

int balongv7r2_gpio_get(struct gpio_chip *chip, unsigned int bank_pin);
int test_gpio_value_get_valid_value()
{
    if(-EINVAL == balongv7r2_gpio_get(gpio_to_chip(GPIO_TEST_NUM), GPIO_INVALID_NUM))
    {
        return GPIO_OK;
    }

    return GPIO_ERROR;
}

extern void balongv7r2_gpio_set(struct gpio_chip *chip, unsigned int bank_pin,int value);
int test_gpio_value_set_valid_value(void)
{
    balongv7r2_gpio_set(gpio_to_chip(GPIO_TEST_NUM), GPIO_INVALID_NUM, 0);

    return GPIO_OK;
}

int balongv7r2_gpio_direction_output(struct gpio_chip *chip, unsigned int bank_pin,int value);
int test_gpio_direction_output_valid_value(void)
{
    if(-EINVAL == balongv7r2_gpio_direction_output(gpio_to_chip(GPIO_TEST_NUM), GPIO_INVALID_NUM, 0))
    {
        return GPIO_OK;
    }
    return GPIO_ERROR;
}

extern int balongv7r2_gpio_direction_input(struct gpio_chip *chip, unsigned int bank_pin);
int test_gpio_direction_input_valid_value(void)
{
    if(-EINVAL == balongv7r2_gpio_direction_input(gpio_to_chip(GPIO_TEST_NUM), GPIO_INVALID_NUM))
    {
        return GPIO_OK;
    }
    return GPIO_ERROR;
}

extern int balongv7r2_gpio_to_irq(struct gpio_chip *chip,unsigned int bank_pin);
int test_gpio_to_irq_valid_value(void)
{
    if(-EINVAL == balongv7r2_gpio_to_irq(gpio_to_chip(GPIO_TEST_NUM), GPIO_INVALID_NUM))
    {
        return GPIO_OK;
    }
    return GPIO_ERROR;
}

extern int balongv7r2_gpio_direction_get(unsigned gpio_num);
int test_gpio_direction_get_valid_value(void)
{
    balongv7r2_gpio_direction_get(GPIO_INVALID_NUM);

    return GPIO_OK;
}

int test_gpio_int_mask_set_valid_value(void)
{
    gpio_int_mask_set(GPIO_INVALID_NUM);

    return GPIO_OK;
}

int test_gpio_int_unmask_set_valid_value(void)
{
    gpio_int_unmask_set(GPIO_INVALID_NUM);

    return GPIO_OK;
}

int test_gpio_set_function_valid_value(void)
{
    gpio_set_function(GPIO_INVALID_NUM, 0);

    return GPIO_OK;
}

int test_gpio_int_state_get_valid_value(void)
{
    if(-EINVAL == gpio_int_state_get(GPIO_INVALID_NUM))
    {
        return GPIO_OK;
    }
    return GPIO_ERROR;
}

int test_gpio_int_state_clear_valid_value(void)
{
    gpio_int_state_clear(GPIO_INVALID_NUM);

    return GPIO_OK;
}

int test_gpio_raw_int_state_get_valid_value(void)
{
    gpio_raw_int_state_get(GPIO_INVALID_NUM);

    return GPIO_OK;
}

int test_gpio_int_trigger_set_valid_value(void)
{
    gpio_int_trigger_set(GPIO_INVALID_NUM, 0);

    return GPIO_OK;
}

int test_gpio_output_get_value(void)
{
    gpio_get_value(GPIO_TEST_NUM);

    return GPIO_OK;
}

int test_gpio_input_set_value(void)
{
    gpio_set_value(GPIO_TEST_NUM, 0);

    return GPIO_OK;
}


int test_gpio_init(void)
{
    gpio_free(GPIO_TEST_NUM);
    if(gpio_request(GPIO_TEST_NUM,"GPIO_TEST"))
    {
        printk(KERN_INFO "request gpio fail!!");
        return GPIO_ERROR;
    }

    return GPIO_OK;
}

void gpio_test(void)
{

    test_gpio_init();

    test_gpio_value_get_valid_value();
    test_gpio_value_set_valid_value();
    test_gpio_direction_get_valid_value();
    test_gpio_direction_output_valid_value();
    test_gpio_direction_input_valid_value();
    test_gpio_to_irq_valid_value();
    test_gpio_int_mask_set_valid_value();
    test_gpio_int_unmask_set_valid_value();
    test_gpio_set_function_valid_value();
    test_gpio_int_state_get_valid_value();
    test_gpio_int_state_clear_valid_value();
    test_gpio_raw_int_state_get_valid_value();
    test_gpio_int_trigger_set_valid_value();

    test_gpio_direction_output();
    test_gpio_output_get_value();
    test_gpio_set_value();

    test_gpio_to_irq();
    test_gpio_direction_get();

    test_gpio_direction_input();
    test_gpio_input_set_value();
    test_gpio_get_value();

    test_gpio_int_mask_set();
    test_gpio_int_trigger_set();
    test_gpio_set_function();
    test_gpio_int_unmask_set();

    test_gpio_int_state_get();
    test_gpio_int_state_clear();
    test_gpio_raw_int_state_get();


}

