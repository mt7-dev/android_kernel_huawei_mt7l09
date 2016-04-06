/*
 * gpio_balong.c - hisilicon balong gpio driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
/*lint --e{64, 527, 533, 537}*/
#include <linux/errno.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include "bsp_memmap.h"
#include "soc_interrupts.h"
#include <hi_gpio.h>
#include <bsp_ipc.h>
#include <bsp_icc.h>
#include "bsp_om.h"



#define DRIVER_NAME    "balongv7r2-gpio"
#define LABEL_NAME  "balongv7r2"

#define HI_GPIO_REG_OFFSET                      (0x1000)

/*irq num*/
#ifdef HI_GPIO4_REGBASE_ADDR
#define GPIO_IRQ_NUM                           (6)
#else
#define GPIO_IRQ_NUM                           (4)
#endif

#define  gpio_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_GPIO, "[gpio]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  gpio_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_GPIO, "[gpio]: "fmt, ##__VA_ARGS__))


//extern struct gpio_chip *gpio_to_chip(unsigned gpio);

struct v7r2_gpio
{/*lint --e{959, 958}*/
    struct gpio_chip gc[GPIO_MAX_BANK_NUM];
    int irq[GPIO_MAX_BANK_NUM];/*每组GPIO对应中断号*/
    unsigned bank_reg_addr[GPIO_MAX_BANK_NUM];/*每组GPIO基址*/

    struct clk *gpio_clk[GPIO_MAX_BANK_NUM];

    unsigned int dir_reg;
    unsigned int out_value_reg;
    unsigned int in_value_reg;
    unsigned int gpio_sys_lock;/*核间锁保护读写方向、数值*/

    unsigned int inten_reg;
    unsigned int intmask_reg;
    unsigned int inttype_reg;
    unsigned int intploarity_reg;
    unsigned int intstate_reg;
    unsigned int intrawstate_reg;
    unsigned int intclear_reg;
    spinlock_t gpio_lock;/*核内锁保护读写方向、数值*/

    #ifdef CONFIG_PM
    unsigned int suspended;
    unsigned int reg_value[GPIO_DPM_BANK_NUM][GPIO_DPM_REG_NUM];/*低功耗时，保存GPIO所有寄存器数值*/
    #endif
};

struct hi_platform_gpio{
    int irq_start;/*GPIO第一个中断号*/
    int irq_num;/*GPIO中断号数目*/

    unsigned bank_reg_addr[GPIO_MAX_BANK_NUM];/*每组GPIO基址*/
    unsigned int bank_addr_length;/*每组GPIO地址空间长度*/

    unsigned int gpio_sys_lock;/*核间锁保护读写方向、数值*/

    unsigned int dir_reg;/*方向寄存器偏移地址*/
    unsigned int out_value_reg;/*输出数据寄存器偏移地址*/
    unsigned int in_value_reg;/*输入数据寄存器偏移地址*/

    unsigned int inten_reg;/*中断模式寄存器偏移地址*/
    unsigned int intmask_reg;/*中断屏蔽寄存器偏移地址*/
    unsigned int inttype_reg;/*中断类型寄存器偏移地址*/
    unsigned int intploarity_reg;/*中断极性寄存器偏移地址*/
    unsigned int intstate_reg;/*中断状态寄存器偏移地址*/
    unsigned int intrawstate_reg;/*原始中断状态寄存器偏移地址*/
    unsigned int intclear_reg;/*清中断寄存器偏移地址*/
};

int gpio_reg_value_get(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin)
{
    unsigned gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->in_value_reg);

    return (int)((gpio_value & ((unsigned int)0x1 << bank_pin)) >> bank_pin);

}

int balongv7r2_gpio_get(struct gpio_chip *chip, unsigned int bank_pin)
{
    int value = 0;
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = (unsigned int)chip->base / GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(chip, struct v7r2_gpio, gc[bank_num]);

    if( GPIO_OUTPUT == gpio_direction_get((unsigned int)(chip->base) + bank_pin))
    {
        gpio_print_error( " gpio direction error,please make sure gpio input.\n");
        return -EINVAL;
    }

    /*if(gpio->debounce[chip->base + bank_pin])
    {
        udelay(gpio->debounce[chip->base + bank_pin]);
    }*/

    value = gpio_reg_value_get(gpio,bank_num, bank_pin);

    return value;
}


void gpio_reg_value_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin, int value)
{
/*lint --e{718, 746}*/
    unsigned gpio_value;
    unsigned long flags = 0;

    spin_lock_irqsave(&gpio->gpio_lock, flags);
    bsp_ipc_spin_lock(gpio->gpio_sys_lock);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->out_value_reg);/* [false alarm]:屏蔽Fortify错误 */
    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->out_value_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->out_value_reg);
    }

    bsp_ipc_spin_unlock(gpio->gpio_sys_lock);
    spin_unlock_irqrestore(&gpio->gpio_lock, flags);

}

void balongv7r2_gpio_set(struct gpio_chip *chip, unsigned int bank_pin,int value)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = (unsigned int)chip->base / GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(chip, struct v7r2_gpio, gc[bank_num]);

    if( GPIO_INPUT == gpio_direction_get((unsigned)(chip->base) + bank_pin))
    {
        gpio_print_error( " gpio direction error,please make sure gpio output.\n");
        return;
    }

    gpio_reg_value_set(gpio,bank_num, bank_pin,value);

}

void gpio_reg_direction_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin,unsigned int value)
{
    unsigned gpio_value;

    unsigned long flags = 0;

    spin_lock_irqsave(&gpio->gpio_lock, flags);

    bsp_ipc_spin_lock(gpio->gpio_sys_lock);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->dir_reg);/* [false alarm]:屏蔽Fortify错误 */
    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->dir_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->dir_reg);
    }

    bsp_ipc_spin_unlock(gpio->gpio_sys_lock);

    spin_unlock_irqrestore(&gpio->gpio_lock, flags);


}

int balongv7r2_gpio_direction_output(struct gpio_chip *chip, unsigned int bank_pin,int value)
{
/*lint --e{533, 830}*/
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = (unsigned int)chip->base / GPIO_MAX_PINS;
	
    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(chip, struct v7r2_gpio, gc[bank_num]);

    gpio_reg_direction_set(gpio,bank_num, bank_pin,GPIO_OUTPUT);
    gpio_reg_value_set(gpio,bank_num, bank_pin,value);

    return GPIO_OK;

}

int balongv7r2_gpio_direction_input(struct gpio_chip *chip, unsigned int bank_pin)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = (unsigned int)chip->base / GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(chip, struct v7r2_gpio, gc[bank_num]);

    gpio_reg_direction_set(gpio,bank_num, bank_pin,GPIO_INPUT);

    return GPIO_OK;

}

int balongv7r2_gpio_to_irq(struct gpio_chip *chip,unsigned int bank_pin)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = (unsigned int)chip->base / GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(chip, struct v7r2_gpio, gc[bank_num]);

    return gpio->irq[bank_num];/*lint !e413*/
}

int gpio_reg_direction_get(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin)
{
    unsigned gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->dir_reg);

    return (int)((gpio_value & ((unsigned int)0x1 << bank_pin)) >> bank_pin);

}

int balongv7r2_gpio_direction_get(unsigned gpio_num)
{
    int value = 0;
    struct v7r2_gpio *gpio = NULL;

    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return GPIO_ERROR;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    value = gpio_reg_direction_get(gpio,bank_num, bank_pin);

    return value;

}

int gpio_direction_get(unsigned gpio)
{
    return balongv7r2_gpio_direction_get(gpio);
}

EXPORT_SYMBOL_GPL(gpio_direction_get);

void gpio_reg_int_mark_or_unmask(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin,int value)
{
    unsigned gpio_value;
    unsigned long flags = 0;

    spin_lock_irqsave(&gpio->gpio_lock, flags);
    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->intmask_reg);/* [false alarm]:屏蔽Fortify错误 */

    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->intmask_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->intmask_reg);
    }
    spin_unlock_irqrestore(&gpio->gpio_lock, flags);
}


static void balongv7r2_gpio_int_mask_set(unsigned gpio_num)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    gpio_reg_int_mark_or_unmask(gpio,bank_num,bank_pin,GPIO_INT_DISABLE);

}

void gpio_int_mask_set(unsigned gpio)
{
    balongv7r2_gpio_int_mask_set(gpio);
}
EXPORT_SYMBOL_GPL(gpio_int_mask_set);

void balongv7r2_gpio_int_unmask_set(unsigned gpio_num)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    gpio_reg_int_mark_or_unmask(gpio,bank_num,bank_pin,GPIO_INT_ENABLE);

}

void gpio_int_unmask_set(unsigned gpio)
{
    balongv7r2_gpio_int_unmask_set(gpio);
}
EXPORT_SYMBOL_GPL(gpio_int_unmask_set);

void gpio_reg_fun_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin,int value)
{
    unsigned gpio_value;
    unsigned long flags = 0;

    spin_lock_irqsave(&gpio->gpio_lock, flags);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->inten_reg);/* [false alarm]:屏蔽Fortify错误 */

    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->inten_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->inten_reg);
    }

    spin_unlock_irqrestore(&gpio->gpio_lock, flags);

}


void balongv7r2_gpio_func_set(unsigned gpio_num,int value)
{
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    gpio_reg_fun_set(gpio,bank_num,bank_pin,value);

}

void gpio_set_function(unsigned gpio, int mode)
{
    balongv7r2_gpio_func_set(gpio,mode);
}
EXPORT_SYMBOL_GPL(gpio_set_function);

int gpio_reg_int_state(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin)
{
    unsigned gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->intstate_reg);

    return (int)((gpio_value & ((unsigned)0x1 << bank_pin)) >> bank_pin);
}

int balongv7r2_gpio_int_state(unsigned gpio_num)
{
/*lint --e{533}*/
    int value = 0;
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    value = gpio_reg_int_state(gpio,bank_num, bank_pin);


    return value;

}

int gpio_int_state_get(unsigned gpio)
{
    return balongv7r2_gpio_int_state(gpio);

}
EXPORT_SYMBOL_GPL(gpio_int_state_get);

void gpio_reg_intclear_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin)
{

    unsigned gpio_value;
    unsigned long flags = 0;
    spin_lock_irqsave(&gpio->gpio_lock, flags);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->intclear_reg);/* [false alarm]:屏蔽Fortify错误 */

    writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->intclear_reg);

    spin_unlock_irqrestore(&gpio->gpio_lock, flags);
}

void balongv7r2_gpio_int_clear(unsigned gpio_num)
{
    struct v7r2_gpio *gpio = NULL;

    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    gpio_reg_intclear_set(gpio,bank_num,bank_pin);

    }

void gpio_int_state_clear(unsigned gpio)
{
    balongv7r2_gpio_int_clear(gpio);

}
EXPORT_SYMBOL_GPL(gpio_int_state_clear);

int gpio_reg_int_raw_state(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin)
{
    unsigned gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->intrawstate_reg);

    return (int)((gpio_value & ((unsigned int)0x1 << bank_pin)) >> bank_pin);
}


int balongv7r2_gpio_int_raw_state(unsigned gpio_num)
{
	/*lint --e{533}*/
    int value = 0;
    struct v7r2_gpio *gpio = NULL;
    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return -EINVAL;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    value = gpio_reg_int_raw_state(gpio,bank_num, bank_pin);

    return value;

}


int gpio_raw_int_state_get(unsigned gpio)
{
    return balongv7r2_gpio_int_raw_state(gpio);

}
EXPORT_SYMBOL_GPL(gpio_raw_int_state_get);

void gpio_reg_int_type_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin,int value)
{
    unsigned gpio_value;
    unsigned long flags = 0;

    spin_lock_irqsave(&gpio->gpio_lock, flags);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->inttype_reg);/* [false alarm]:屏蔽Fortify错误 */

    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->inttype_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->inttype_reg);
    }

    spin_unlock_irqrestore(&gpio->gpio_lock, flags);
}

void gpio_reg_int_trigger_set(struct v7r2_gpio *gpio,unsigned int bank_num, unsigned int bank_pin,int value)
{

    unsigned gpio_value;
    unsigned long flags = 0;
    spin_lock_irqsave(&gpio->gpio_lock, flags);

    gpio_value = readl(gpio->bank_reg_addr[bank_num] + gpio->intploarity_reg);/* [false alarm]:屏蔽Fortify错误 */

    if(!!value)
    {
        writel(gpio_value | ((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->intploarity_reg);
    }
    else
    {
        writel(gpio_value & ~((unsigned int)0x1 << bank_pin), gpio->bank_reg_addr[bank_num] + gpio->intploarity_reg);
    }

    spin_unlock_irqrestore(&gpio->gpio_lock, flags);

}

void balongv7r2_gpio_int_trigger_set(unsigned gpio_num,int value)
{
    int level_t = 0;
    int polarity_t = 0;

    struct v7r2_gpio *gpio = NULL;

    unsigned int bank_num = gpio_num / GPIO_MAX_PINS;
    unsigned int bank_pin = gpio_num % GPIO_MAX_PINS;

    if(bank_num >= GPIO_MAX_BANK_NUM || (bank_pin >= GPIO_MAX_PINS))
    {
        gpio_print_error( " parameter error.\n");
        return;
    }

    gpio = container_of(gpio_to_chip(gpio_num), struct v7r2_gpio, gc[bank_num]);

    switch(value)
    {
        case IRQ_TYPE_EDGE_RISING:
            level_t = GPIO_INT_TYPE_EDGE;
            polarity_t = GPIO_INT_POLARITY_RIS_HIGH;
            break;

        case IRQ_TYPE_EDGE_FALLING:
            level_t = GPIO_INT_TYPE_EDGE;
            polarity_t = GPIO_INT_POLARITY_FAL_LOW;
            break;

        case IRQ_TYPE_LEVEL_HIGH:
            level_t = GPIO_INT_TYPE_LEVEVL;
            polarity_t = GPIO_INT_POLARITY_RIS_HIGH;
            break;

        case IRQ_TYPE_LEVEL_LOW:
            level_t = GPIO_INT_TYPE_LEVEVL;
            polarity_t = GPIO_INT_POLARITY_FAL_LOW;
            break;

        default:
            gpio_print_error( " trigger level error.\n");
            return;

    }

    gpio_reg_int_type_set(gpio,bank_num,bank_pin,level_t);
    gpio_reg_int_trigger_set(gpio,bank_num,bank_pin,polarity_t);

}

void gpio_int_trigger_set(unsigned gpio, int trigger_type)
{
    balongv7r2_gpio_int_trigger_set(gpio,trigger_type);

}
EXPORT_SYMBOL_GPL(gpio_int_trigger_set);


/*GPIO驱动探测函数*/
static int __devinit hi_gpio_probe(struct platform_device *pdev)
{
    int i = 0;
    int i_irq = 0;
    int i_mem = 0;
    int i_chip = 0;
    int ret = 0;

    unsigned int bank_addr = 0;
    char clk_name[24] = "";

    struct v7r2_gpio *gpio = NULL;
    struct hi_platform_gpio *pdata = NULL;

    gpio_print_info( " gpio init in acore.\n");

    pdata = pdev->dev.platform_data; /*获取gpio平台数据*/
    if(NULL == pdata){
        dev_err(&pdev->dev,"no platform data.\n");
        return -EINVAL;
    }

    gpio = kzalloc(sizeof(*gpio),GFP_KERNEL);/*为v7r2_gpio申请内核内存*/
    if(!gpio) {
        dev_err(&pdev->dev,"no memory for state.\n");
        return -ENOMEM;
    }

    gpio->gpio_sys_lock = pdata->gpio_sys_lock;/*获取核间锁*/

    gpio->dir_reg = pdata->dir_reg;/*获取方向寄存器偏移地址*/
    gpio->out_value_reg = pdata->out_value_reg;/*获取输出数据寄存器偏移地址*/
    gpio->in_value_reg = pdata->in_value_reg;/*获取输入数据寄存器偏移地址*/

    gpio->inten_reg = pdata->inten_reg;/*获取中断模式寄存器偏移地址*/
    gpio->intmask_reg = pdata->intmask_reg;/*获取中断屏蔽寄存器偏移地址*/
    gpio->inttype_reg = pdata->inttype_reg;/*获取中断类型寄存器偏移地址*/
    gpio->intploarity_reg = pdata->intploarity_reg;/*获取中断极性寄存器偏移地址*/
    gpio->intstate_reg = pdata->intstate_reg;/*获取中断状态寄存器偏移地址*/
    gpio->intrawstate_reg = pdata->intrawstate_reg;/*获取原始中断状态寄存器偏移地址*/
    gpio->intclear_reg = pdata->intclear_reg;/*获取清中断寄存器偏移地址*/

    spin_lock_init(&gpio->gpio_lock);

    /*获取中断号*/
    for(i_irq = 0;i_irq < pdata->irq_num;i_irq++)
    {
        gpio->irq[i_irq] = pdata->irq_start+ i_irq;
    }

    /*获取IO资源对应的虚拟地址*/
    for(i_mem = 0;i_mem < GPIO_MAX_BANK_NUM;i_mem++)
    {
        bank_addr = pdata->bank_reg_addr[i_mem];
        gpio->bank_reg_addr[i_mem] = IO_ADDRESS(bank_addr);
    }

    /*为每组gpio chip赋值，并添加到内核gpio_desc结构体*/
    for(i_chip = 0; i_chip < GPIO_MAX_BANK_NUM; i_chip++)
    {
        gpio->gc[i_chip].dev = &pdev->dev;
        gpio->gc[i_chip].label = LABEL_NAME;
        gpio->gc[i_chip].owner = THIS_MODULE;
        gpio->gc[i_chip].base = (i_chip * GPIO_MAX_PINS);
        gpio->gc[i_chip].ngpio = GPIO_MAX_PINS;
        gpio->gc[i_chip].direction_input = balongv7r2_gpio_direction_input;
        gpio->gc[i_chip].get = balongv7r2_gpio_get;
        gpio->gc[i_chip].direction_output = balongv7r2_gpio_direction_output;
        gpio->gc[i_chip].set = balongv7r2_gpio_set;
        gpio->gc[i_chip].to_irq = balongv7r2_gpio_to_irq;

        /*gpio->gc[i_chip].set_debounce = balongv7r2_gpio_debounce_set;*/
        ret |= gpiochip_add(&gpio->gc[i_chip]);
    }
    if(ret)
    {
        gpio_print_error( "gpiochip add error = %d.\n", ret);
        goto err_addchip;
    }
    else
    {
        gpio_print_info( "gpiochip_add over.\n");
    }

    /*打开gpio1~3时钟*/
    for(i = 1;i < GPIO_MAX_BANK_NUM;i++)
    {
        snprintf(clk_name, 20, "gpio%d_clk", i);

        gpio->gpio_clk[i] = (struct clk *)clk_get(NULL,clk_name);
    	if(IS_ERR(gpio->gpio_clk[i])){
    		gpio_print_error("gpio clk%d cannot get, 0x%x.\n", i, gpio->gpio_clk[i]);
    		goto err_getclk;
    	}

        if(0 != clk_enable(gpio->gpio_clk[i]))
        {
    		gpio_print_error("gpio clk enable is fail, 0x%x.\n", gpio->gpio_clk[i]);
    		goto err_enableclk;
        }

    }


    /*将v7r2_gpio数据存入设备drvdata中*/
    platform_set_drvdata(pdev, gpio);

    /*清除中断，并屏蔽中断*/
   for(i = 0; i < GPIO_MAX_BANK_NUM * GPIO_MAX_PINS; i++)
   {
        gpio_int_state_clear((unsigned int)i);

        gpio_int_mask_set((unsigned int)i);
    }

#ifdef CONFIG_PM
    gpio->suspended = 0;
#endif

    gpio_print_info( " gpio init over.\n");
    return ret;


err_enableclk:
    clk_put(gpio->gpio_clk[i]);

err_getclk:
    while(--i > 0)
    {
        clk_disable(gpio->gpio_clk[i]);
        clk_put(gpio->gpio_clk[i]);
    }

err_addchip:

    kfree(gpio);

    return (ret);
}


/*移除GPIO设备函数*/
static int __devexit hi_gpio_remove(struct platform_device *pdev)
{
    struct  v7r2_gpio *gpio = platform_get_drvdata(pdev);

    platform_set_drvdata(pdev,NULL);
    kfree(gpio);

    return 0;

}

#ifdef CONFIG_PM
static int hi_gpio_prepare(struct device *pdev)
{
    return GPIO_OK;
}

static void hi_gpio_complete(struct device *pdev)
{
    return ;
}

static int hi_gpio_suspend(struct device *dev)
{
    return GPIO_OK;

}

static int hi_gpio_resume(struct device *dev)
{
    return GPIO_OK;
}


static const struct dev_pm_ops balong_gpio_dev_pm_ops ={
	.suspend = hi_gpio_suspend,
	.resume = hi_gpio_resume,
	.prepare = hi_gpio_prepare,
	.complete = hi_gpio_complete,
};

#define BALONG_DEV_PM_OPS (&balong_gpio_dev_pm_ops)

#else

#define BALONG_DEV_PM_OPS NULL

#endif

/*GPIO驱动模型*/
static struct platform_driver hi_gpio_driver = {
    .probe           = hi_gpio_probe,
    .remove          = __devexit_p(hi_gpio_remove),
    .driver          =
    {
        .name        = DRIVER_NAME,
        .owner       = THIS_MODULE,
        .pm		= BALONG_DEV_PM_OPS

    },
};


/*GPIO设备拥有平台数据*/
static struct hi_platform_gpio gpio_config = {
    .irq_start = INT_LVL_APP_GPIO0,
    .irq_num = GPIO_IRQ_NUM,
    .bank_reg_addr[0] = HI_GPIO0_REGBASE_ADDR,
    .bank_reg_addr[1] = HI_GPIO1_REGBASE_ADDR,
    .bank_reg_addr[2] = HI_GPIO2_REGBASE_ADDR,
    .bank_reg_addr[3] = HI_GPIO3_REGBASE_ADDR,
#if (GPIO_MAX_BANK_NUM == 6)
    .bank_reg_addr[4] = HI_GPIO4_REGBASE_ADDR,
    .bank_reg_addr[5] = HI_GPIO5_REGBASE_ADDR,
#endif

    .gpio_sys_lock = IPC_SEM_GPIO,
    .bank_addr_length = HI_GPIO_REG_OFFSET,
    .dir_reg = HI_GPIO_SWPORT_DDR_OFFSET,
    .out_value_reg = HI_GPIO_SWPORT_DR_OFFSET,
    .in_value_reg = HI_GPIO_EXT_PORT_OFFSET,
    .inten_reg = HI_GPIO_INTEN_OFFSET,
    .intmask_reg = HI_GPIO_INTMASK_OFFSET,
    .inttype_reg = HI_GPIO_INTTYPE_LEVEL_OFFSET,
    .intploarity_reg = HI_GPIO_INT_PLOARITY_OFFSET,
    .intstate_reg = HI_GPIO_INTSTATUS_OFFSET,
    .intrawstate_reg = HI_GPIO_RAWINTSTATUS_OFFSET,
    .intclear_reg = HI_GPIO_PORT_EOI_OFFSET,
};

/*GPIO设备模型*/
static struct platform_device hi_gpio_device = {
    .name           = DRIVER_NAME,
    .id             = 1,
    .dev ={
        .platform_data = &gpio_config,
    },
};

static int __init hi_gpio_init(void)
{
    int ret = 0;

    ret = platform_device_register(&hi_gpio_device);
    if(ret)
    {

        gpio_print_error( "gpio device register is failed!");
        return ret;
    }

    ret = platform_driver_register(&hi_gpio_driver);

    if(ret)
    {
        platform_device_unregister(&hi_gpio_device);

        gpio_print_error( "gpio driver register is failed!");
        return ret;
    }
    return ret;
}

static void __exit hi_gpio_exit(void)
{
    platform_driver_unregister(&hi_gpio_driver);
    platform_device_unregister(&hi_gpio_device);

}

subsys_initcall(hi_gpio_init);
module_exit(hi_gpio_exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Balong GPIO driver for Hisilicon");
MODULE_LICENSE("GPL");

