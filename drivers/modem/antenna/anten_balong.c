/*
 * anten_balong.c - hisilicon a core anten plug in/out detect driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
/*lint -save -e537*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/timer.h>

#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/gpio.h>

#include "bsp_om.h"
#include "bsp_nvim.h"
#include "drv_nv_id.h"
#include "drv_nv_def.h"
#include "bsp_memmap.h"
#include "bsp_icc.h"
#include "hi_anten.h"
#include "drv_anten.h"
/*lint -restore */

#define ANTEN_OK                       (0)
#define ANTEN_ERROR                    (-1)

#ifdef HI_K3_ANTEN
//#define ANTEN_MODEM0_GPIO              (158)
//#define ANTEN_MODEM1_GPIO              (159)
#define ANTEN_MAX_NUM                  (2)
#define IRQ_TRIGGER_TYPE               (IRQF_SHARED | IRQF_TRIGGER_PROBE)
#define gpio_int_mask_set(v)
#define gpio_int_state_clear(v)
#define gpio_int_trigger_set(n,v)
#define gpio_set_function(n,v)
#define gpio_int_unmask_set(v)
#define gpio_int_state_get(v)			1
#else
//#define ANTEN_MODEM0_GPIO              (GPIO_0_15)
#define ANTEN_MAX_NUM                  (1)
#define IRQ_TRIGGER_TYPE               (IRQF_SHARED)
#endif

#define DRIVER_NAME                    "anten_balong"

#define anten_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ANTEN, "[anten]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define anten_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_ANTEN, "[anten]: "fmt, ##__VA_ARGS__))

struct gpio_anten;

struct anten_data
{
    unsigned int modem_id;
    unsigned int gpio;
    unsigned int status;
    unsigned int is_debounce;
	unsigned int timer_debounce;	/* in msecs */

    struct gpio_anten *anten;
	struct timer_list timer;
    //struct work_struct anten_work;
};

struct gpio_anten
{
    unsigned int chan_id;
    //struct workqueue_struct  *anten_workque;

    unsigned int n_anten;
    struct anten_data data[0];

};

struct anten_msg_stru
{
    unsigned int modem_id;
    unsigned int status;
};

struct anten_platform_data{

    unsigned int modem_gpio[ANTEN_MAX_NUM];

};

void anten_handle_work(struct anten_data *data)
{
    int len = 0;

    struct gpio_anten *anten = NULL;
    struct anten_msg_stru anten_msg ;

    anten = data->anten;/*lint !e413*/

    /*lint -save -e413*/
    anten_msg.modem_id = data->modem_id;
    anten_msg.status = data->status;
    anten_print_info("modem id = 0x%x, status = %d.\n", anten_msg.modem_id, anten_msg.status);

    /*通过IFC将消息发出*/
    len = bsp_icc_send(ICC_CPU_MODEM, anten->chan_id, (unsigned char*)&anten_msg, (u32)sizeof(struct anten_msg_stru));
    if(len != sizeof(struct anten_msg_stru))
    {
        anten_print_error("send len(%x) != expected len(%x).\n", len, sizeof(struct anten_msg_stru));
        return;
    }
    /*lint -restore*/
}


static void gpio_antens_gpio_timer(unsigned long _data)
{
	struct anten_data *bdata = (struct anten_data *)_data;
    int status = gpio_get_value((unsigned)(bdata->gpio));

    if(((0 == bdata->status) && (1 == status)) || ((1 == bdata->status) && (0 == status)))
    {
        bdata->status = status;
        gpio_int_trigger_set((unsigned)(bdata->gpio), status ? IRQ_TYPE_LEVEL_LOW : IRQ_TYPE_LEVEL_HIGH);
        anten_handle_work(bdata);
    }  
    
    bdata->is_debounce = 0;

    gpio_int_unmask_set((unsigned)(bdata->gpio));

}


static irqreturn_t anten_irq_handle(int irq, void *dev_id)
{
    struct anten_data *data = (struct anten_data *)dev_id;
    int status = 0;

	if(!gpio_int_state_get(data->gpio))
	{
		return IRQ_NONE;
	}
    
	gpio_int_mask_set(data->gpio);
	gpio_int_state_clear(data->gpio);
	status = gpio_get_value(data->gpio);

    //queue_work(anten->anten_workque, &(data->anten_work));
    
	if(data->timer_debounce && (data->status != status) && (0 == data->is_debounce))
    {
        data->is_debounce = 1;
		mod_timer(&data->timer, jiffies + msecs_to_jiffies(data->timer_debounce));/*考虑fiffies是否会溢出*/
    }
    else
    {
        gpio_int_unmask_set((unsigned)(data->gpio));

    }

	return IRQ_HANDLED;
}

static int setup_gpio_anten(DRV_DRV_ANTEN_GPIO_STRU *pdata,
                            unsigned int i,
        					struct anten_data *bdata,
        					struct gpio_anten *anten)
{
    int value = 0;

    if(1 != pdata->anten_gpio[i].used)
    {
        return 0;
    }
    
	//INIT_WORK(&(bdata->anten_work), anten_handle_work);

    bdata->gpio = pdata->anten_gpio[i].gpio;
	if(gpio_request(bdata->gpio, DRIVER_NAME))
	{
		anten_print_error("request gpio%d is error!\n", bdata->gpio);
		return -1;
	}

    gpio_direction_input(bdata->gpio);

    bdata->is_debounce = 0;
    bdata->timer_debounce = 20UL;

	gpio_int_mask_set(bdata->gpio);
    gpio_int_state_clear(bdata->gpio);

	value = gpio_get_value(bdata->gpio);
    if (value)
    {
        bdata->status = 1;
		gpio_int_trigger_set(bdata->gpio, IRQ_TYPE_LEVEL_LOW);
    }
    else
    {
        bdata->status = 0;
        gpio_int_trigger_set(bdata->gpio, IRQ_TYPE_LEVEL_HIGH);
    }

    gpio_set_function(bdata->gpio, 1);

    bdata->modem_id = pdata->anten_gpio[i].modem_id;

    bdata->anten = anten;

	setup_timer(&bdata->timer, gpio_antens_gpio_timer, (unsigned long)bdata);
    
    /*conncet anten interrupt*/
	if(request_irq(gpio_to_irq(bdata->gpio), anten_irq_handle, IRQ_TRIGGER_TYPE, DRIVER_NAME, bdata))
	{
	    anten_print_error("request anten gpio irq is error!\n");
	    goto irq_error;
	}

    gpio_int_unmask_set(bdata->gpio);

    return 0;

irq_error:
    gpio_free(bdata->gpio);

    return -1;

}


#if 0

#ifdef CONFIG_PM
static int hi_anten_prepare(struct device *pdev)
{
    return 0;
}

static void hi_anten_complete(struct device *pdev)
{
    return ;
}

static int hi_anten_suspend(struct device *dev)
{

    return 0;
}

static int hi_anten_resume(struct device *dev)
{

    return 0;
}

static const struct dev_pm_ops balong_anten_dev_pm_ops ={
	.suspend  = hi_anten_suspend,
	.resume   = hi_anten_resume,
	.prepare  = hi_anten_prepare,
	.complete = hi_anten_complete,
};

#define BALONG_DEV_PM_OPS (&balong_anten_dev_pm_ops)

#else

#define BALONG_DEV_PM_OPS NULL

#endif

/*ANTEN驱动模型*/
static struct platform_driver hi_anten_driver = {
    .probe           = hi_anten_probe,
    .remove          = hi_anten_remove,
    .driver          =
    {
        .name        = DRIVER_NAME,
        .owner       = THIS_MODULE,
        .pm		= BALONG_DEV_PM_OPS

    },
};


/*ANTEN设备拥有平台数据*/
static struct anten_platform_data anten_plat_data = {
#ifdef HI_K3_ANTEN
    .modem_gpio[0] = ANTEN_MODEM0_GPIO,
    .modem_gpio[1] = ANTEN_MODEM1_GPIO,

#else
    .modem_gpio[0] = ANTEN_MODEM0_GPIO,

#endif
};

/*ANTEN设备模型*/
static struct platform_device hi_anten_device = {
    .name           = DRIVER_NAME,
    .id             = 1,
    .dev ={
        .platform_data = &anten_plat_data,
    },
};
#endif

static int __init hi_anten_init(void)
{
    int ret = 0;
    unsigned int i = 0;

    struct gpio_anten *anten;
    DRV_DRV_ANTEN_GPIO_STRU anten_stru;

    memset((void*)&anten_stru, 0, sizeof(DRV_DRV_ANTEN_GPIO_STRU));

    /* Get ANTEN NV data by id.*/
    ret = bsp_nvm_read(NV_ID_DRV_ANTEN_CFG,(u8*)&anten_stru,sizeof(DRV_DRV_ANTEN_GPIO_STRU));
    if (ret !=  0)
    {
        anten_print_error("anten_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_ANTEN_CFG, ret);
        return -1;
    }

	anten = kzalloc((size_t)(sizeof(struct gpio_anten) + ANTEN_MAX_NUM * sizeof(struct anten_data)), GFP_KERNEL);
    if(!anten) {
        anten_print_error("no memory for alloc.\n");
        ret = ANTEN_ERROR;
        goto mem_error;
    }

    anten->chan_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ANTEN;

    /*anten->anten_workque = create_singlethread_workqueue(DRIVER_NAME);
	if(NULL == anten->anten_workque)
	{
	    anten_print_error("workqueue create failed!\n");
        ret = ANTEN_ERROR;
	    goto workque_error;
	}*/

    for(i = 0;i < ANTEN_MAX_NUM;i++)
    {
        if(0 != setup_gpio_anten(&anten_stru, i, &(anten->data[i]), anten))
        {
            goto setup_error;
        }
    }


    anten_print_info( " anten init over.\n");
    return ANTEN_OK;

setup_error:
    while(0 != i)
    {
        --i;
        free_irq(gpio_to_irq(anten_stru.anten_gpio[i].gpio), &(anten->data[i]));
        gpio_free(anten_stru.anten_gpio[i].gpio);

    }
    //destroy_workqueue(anten->anten_workque);

//workque_error:
    kfree(anten);

mem_error:
    return ret;

}


static void __exit hi_anten_exit(void)
{
    //platform_driver_unregister(&hi_anten_driver);
    //platform_device_unregister(&hi_anten_device);
}

module_init(hi_anten_init);
module_exit(hi_anten_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("anten driver for the Hisilicon anten plug in/out");
MODULE_LICENSE("GPL");

