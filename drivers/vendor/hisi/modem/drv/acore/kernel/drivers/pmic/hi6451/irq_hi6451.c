/*
 * kernel/driver/mfd/hi6451_irq.c
 *
 * Copyright (C) 2013 Hisilicon
 * License terms: GNU General Public License (GPL) version 2
 *
 */
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <linux/slab.h>
#include <linux/irqnr.h>
#include <linux/err.h>
#include "osl_spinlock.h"
#include "bsp_om.h"

#include <hi_pmu.h>
#include <pmu_balong.h>

#include "irq_hi6451.h"

/*
 * HI6451 IRQ
 */
 #define NUM_IRQ_REG    3   /*num of hi6451 irq reg*/
 #define NUM_IRQM_REG   3   /*num of hi6451 no pwr irq reg*/
 /*存放中断处理回调函数的数组*/
 hi6451_irq_st Hi6451IrqInfo[HI6451_IRQ_NR];
 spinlock_t     hi6451_irq_lock;

 void hi6451_irq_mask(unsigned int irq)
{
    unsigned char reg_temp = 0,offset = 0;
    hi6451_irqflags_t flags;

    if ((irq < IRQ_HI6451_NR_BASE) || (irq > IRQ_HI6451_NR_BASE+HI6451_IRQ_NR)) {
        pmic_print_error("hi6451 irq is error,please check irq=%d\n", irq);
        return;
    }

    /*spin_lock(&hi6451_irq_data->irq_lock);*/
    spin_lock_irqsave(&hi6451_irq_lock,flags);

    if (((irq - IRQ_HI6451_NR_BASE) >> 3) == 0) {
        offset = irq - IRQ_HI6451_NR_BASE;
        bsp_hi6451_reg_read(HI_PMU_IRQM1_OFFSET,&reg_temp);
        reg_temp |= 1 << offset;
        bsp_hi6451_reg_write(HI_PMU_IRQM1_OFFSET,reg_temp);
    } else if(((irq - IRQ_HI6451_NR_BASE) >> 3) == 1) {
        offset = irq - IRQ_HI6451_NR_BASE - HI6451_IRQM1_BASE_OFFSET;
        bsp_hi6451_reg_read(HI_PMU_IRQM2_OFFSET,&reg_temp);
        reg_temp |= 1 << offset;
        bsp_hi6451_reg_write(HI_PMU_IRQM2_OFFSET,reg_temp);
    }else{
        offset = irq - IRQ_HI6451_NR_BASE - HI6451_IRQM2_BASE_OFFSET;
        bsp_hi6451_reg_read(HI_PMU_IRQM3_OFFSET,&reg_temp);
        reg_temp |= 1 << offset;
        bsp_hi6451_reg_write(HI_PMU_IRQM3_OFFSET,reg_temp);
    }
    spin_unlock_irqrestore(&hi6451_irq_lock,flags);
    /*spin_unlock(&hi6451_irq_data->irq_lock);*/
}
void hi6451_irq_unmask(unsigned int irq)
{
    unsigned char reg_temp = 0,offset = 0;
    hi6451_irqflags_t flags;


    if ((irq < IRQ_HI6451_NR_BASE) || (irq > IRQ_HI6451_NR_BASE+HI6451_IRQ_NR)) {
        pmic_print_error("hi6451 irq is error,please check irq=%d\n", irq);
        return;
    }

    /*spin_lock(&hi6451_irq_data->irq_lock);*/
    spin_lock_irqsave(&hi6451_irq_lock,flags);

    if (((irq - IRQ_HI6451_NR_BASE) >> 3) == 0) {
        offset = irq - IRQ_HI6451_NR_BASE;
        bsp_hi6451_reg_read(HI_PMU_IRQM1_OFFSET,&reg_temp);
        reg_temp &= ~(1 << offset);
        bsp_hi6451_reg_write(HI_PMU_IRQM1_OFFSET,reg_temp);
    } else if(((irq - IRQ_HI6451_NR_BASE) >> 3) == 1){
        offset = irq - IRQ_HI6451_NR_BASE - HI6451_IRQM1_BASE_OFFSET;
        bsp_hi6451_reg_read(HI_PMU_IRQM2_OFFSET,&reg_temp);
        reg_temp &= ~(1 << offset);
        bsp_hi6451_reg_write(HI_PMU_IRQM2_OFFSET,reg_temp);
    }else{
        offset = irq - IRQ_HI6451_NR_BASE - HI6451_IRQM2_BASE_OFFSET;
        bsp_hi6451_reg_read(HI_PMU_IRQM3_OFFSET,&reg_temp);
        reg_temp &= ~(1 << offset);
        bsp_hi6451_reg_write(HI_PMU_IRQM3_OFFSET,reg_temp);
    }

    /*spin_unlock(&hi6451_irq_data->irq_lock);*/
    spin_unlock_irqrestore(&hi6451_irq_lock,flags);
}

int hi6451_irq_callback_register(unsigned int irq, hi6451funcptr routine, void *data)
{

    if ((irq < IRQ_HI6451_NR_BASE) || (irq > IRQ_HI6451_NR_BASE+HI6451_IRQ_NR)) {
        pmic_print_error("hi6451 irq is error,please check irq=%d\n", irq);
        return  HI6451_IRQ_ERROR;
    }

    Hi6451IrqInfo[irq - IRQ_HI6451_NR_BASE].routine = routine;
    Hi6451IrqInfo[irq - IRQ_HI6451_NR_BASE].data = data;
    return  HI6451_IRQ_OK;
}
void hi6451_irq_wk_handler()
{
    unsigned int i = 0;
    u8 reg_tmp[NUM_IRQ_REG + NUM_IRQM_REG]  = {0};
    u32 ucIntStat = 0x00;

    pmic_print_error( "\n**********hi6451_irq_wk_handler**********\n");
    for(i = 0; i < NUM_IRQ_REG; i++)
    {
        bsp_hi6451_reg_read((u8)(HI_PMU_IRQ1_OFFSET + i), &(reg_tmp[i]));
        bsp_hi6451_reg_read((u8)(HI_PMU_NO_PWR_IRQ1_OFFSET + i), &(reg_tmp[3+i]));
    }
    /*清中断寄存器*/
    for(i = 0; i < NUM_IRQ_REG; i++)
    {
        bsp_hi6451_reg_write((u8)(HI_PMU_IRQ1_OFFSET + i), reg_tmp[i]);
    }
    /* 检查中断状态，执行对应子中断处理函数*/
    ucIntStat = reg_tmp[0] | (reg_tmp[1] << HI6451_IRQM1_BASE_OFFSET) | (reg_tmp[2]  << HI6451_IRQM2_BASE_OFFSET);
    if (ucIntStat) {
        for (i = 0; i < HI6451_IRQ_NR; i++) {
            if (ucIntStat & (1 << i)) {
                /* handle interrupt service */
                if (NULL != Hi6451IrqInfo[i].routine)
                    Hi6451IrqInfo[i].routine(Hi6451IrqInfo[i].data);
            }
        }
    }

    /* 清非下电寄存器*/
    for(i = 0; i < NUM_IRQM_REG; i++)
    {
        bsp_hi6451_reg_write((u8)(HI_PMU_NO_PWR_IRQ1_OFFSET + i), reg_tmp[3+i]);   /*lint !e534*/
    }
    /*使能中断*/
    (void)enable_irq(INT_LVL_PMU);
}

irqreturn_t hi6451_irq_handler(int irq, void *data)
{
    struct hi6451_irq_data *hi6451_irq_data = data;
    /*关中断*/
    (void)disable_irq_nosync(INT_LVL_PMU);
    pmic_print_error("\n**********hi6451_irq_handler**********\n");
    queue_work(hi6451_irq_data->hi6451_irq_wq, &hi6451_irq_data->hi6451_irq_wk);

    return IRQ_HANDLED;
}

static int __init hi6451_irq_probe(struct platform_device *pdev)
{
    int ret;
    unsigned int i;
    struct hi6451_irq_data  *hi6451_irq_data;

    hi6451_irq_data = kzalloc(sizeof(struct hi6451_irq_data), GFP_KERNEL);
    if (hi6451_irq_data == NULL) {
        pmic_print_error("hi6451 irq kzalloc is failed,please check!\n");
        return -ENOMEM;
    }

    spin_lock_init(&hi6451_irq_lock);

    hi6451_irq_data->irq = platform_get_irq(pdev, 0);
    if (hi6451_irq_data->irq < 0) {
        ret = -EINVAL;
        pmic_print_error("unable to get PMIC HI6451 IRQ\n");
        goto out;
    }

    /*init and set IRQ*/
    /*初始化工作队列*/
    hi6451_irq_data->hi6451_irq_wq = create_singlethread_workqueue("bsp_hi6451_irq");
    INIT_WORK(&hi6451_irq_data->hi6451_irq_wk, (void *)hi6451_irq_wk_handler);

    /*connect and enable irq*/
#if 0
    ret = request_irq(hi6451_irq_data->irq, hi6451_irq_handler, IRQF_DISABLED,
            "hi6451_irq", hi6451_irq_data);
    if (ret < 0) {
        pmic_print_error("unable to request PMIC HI6451 IRQ\n");
        goto out;
    }
    else
        pmic_print_error("hi6451_request_irq ok!\n");
#endif
    /*set hi6451_irq_data in order to get at last*/
    platform_set_drvdata(pdev, hi6451_irq_data);
    pmic_print_error("hi6451_irq_probe ok!\n");
    return 0;

    out:
     kfree(hi6451_irq_data);
    return ret;

}
static  int hi6451_irq_remove(struct platform_device *pdev)
{
    unsigned int i;
    struct hi6451_irq_data *hi6451_irq_data;
    /*get hi6451_irq_data*/
    hi6451_irq_data = platform_get_drvdata(pdev);
    if (NULL == hi6451_irq_data) {
        pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
        return -1;
    }

    /*release memory*/
    kfree(hi6451_irq_data);

    /* cleanup irqchip */
    return 0;
}
#ifdef CONFIG_PM
static int hi6451_irq_suspend(struct platform_device *pdev, pm_message_t state)
{
    struct hi6451_irq_data *hi6451_irq_data;

    pmic_print_error("hi6451 hi6451 irq entry suspend successfully");

    /*get hi6451_irq_data*/
    hi6451_irq_data = platform_get_drvdata(pdev);
    if (NULL == hi6451_irq_data) {
        pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
        return -1;
    }

    return 0;
}
static int hi6451_irq_resume(struct platform_device *pdev)
{
    int ret = 0;
    struct hi6451_irq_data *hi6451_irq_data;

    pmic_print_error("hi6451 hi6451 irq entry resume successfully");
    /*get hi6451_irq_data*/
    hi6451_irq_data = platform_get_drvdata(pdev);
    if (NULL == hi6451_irq_data) {
        pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
        return -1;
    }

    return ret;
}
#endif
/*HI6451 irq 设备注册*/
static struct resource hi6451_irq_resources[] = {
    {
    },
    {
        .start =  INT_LVL_PMU   ,
        .end =  INT_LVL_PMU   ,
        .flags   = IORESOURCE_IRQ,
    },
};
static struct platform_device hi6451_irq_device = {
    .name =  "hi6451-irq",
    .id = 0,
    .dev = {
        .platform_data  = NULL,
    },
    .num_resources = ARRAY_SIZE(hi6451_irq_resources),
    .resource = hi6451_irq_resources,
};
static struct platform_driver hi6451_irq_driver = {
    .probe = hi6451_irq_probe,
    .remove = hi6451_irq_remove,
    #ifdef CONFIG_PM
    .suspend = hi6451_irq_suspend,
    .resume = hi6451_irq_resume,
    #endif
    .driver     = {
        .name   = "hi6451-irq",
    },
};
static int __init hi6451_irq_init(void)
{
    int ret = 0;
    ret = platform_device_register(&hi6451_irq_device);
    ret |= platform_driver_register(&hi6451_irq_driver);
    if (ret != 0)
        pmic_print_error("Failed to register pmic_hi6451_irq driver or device: %d\n", ret);
    return ret;
}
static void __exit hi6451_irq_exit(void)
{
    platform_driver_unregister(&hi6451_irq_driver);
    platform_device_unregister(&hi6451_irq_device);
}
module_exit(hi6451_irq_exit);
module_init(hi6451_irq_init);
MODULE_LICENSE("GPL");
