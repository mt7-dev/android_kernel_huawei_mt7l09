/*
 * kernel/driver/pmic/hi6559/irq_hi6559.c
 *
 * Copyright (C) 2013 Hisilicon
 * License terms: GNU General Public License (GPL) version 2
 *
 */
/*lint --e{537,958}*/
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
#include <linux/kernel.h>
#include "osl_spinlock.h"

#ifdef CONFIG_MODULE_VIC
#include "bsp_vic.h"
#endif
#include "pmu_balong.h"
#include "common_hi6559.h"
#include <hi_bbstar.h>
#include "irq_hi6559.h"


/*存放中断处理回调函数的数组*/
hi6559_irq_st Hi6559IrqInfo[HI6559_IRQ_NR];
struct hi6559_irq_data  *hi6559_irq_data_st;/*中断全局结构体*/

#ifdef CONFIG_MODULE_VIC
static __inline__ void hi6559_irq_enable(void)
{
    bsp_vic_enable(INT_LVL_SMART);
}
static __inline__ void hi6559_irq_disable(void)
{
    bsp_vic_disable(INT_LVL_SMART);
}

#else
static __inline__ void hi6559_irq_enable(void)
{
    enable_irq(INT_LVL_PMU);
}
static __inline__ void hi6559_irq_disable(void)
{
    disable_irq_nosync(INT_LVL_PMU);
}
#endif

/*****************************************************************************
 函 数 名  : hi6559_irq_cnt_show
 功能描述  : 显示中断号
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_irq_cnt_show(void)
{
    s32 i = 0;

    for (i = 0; i < HI6559_IRQ_NR; i++)
    {
      pmic_print_error("hi6559 irq %d is %d!\n", i, Hi6559IrqInfo[i].cnt);
    }
}

/*****************************************************************************
 函 数 名  : hi6559_irq_mask
 功能描述  : 屏蔽中断
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_irq_mask(u32 irq)
{
    hi6559_irqflags_t irq_flags = 0;
    u32 offset = 0;
    u8 reg_temp = 0;

    /* 参数合法性检查 */
    if (irq > IRQ_HI6559_NR_BASE + HI6559_IRQ_NR) 
    {
        pmic_print_error("hi6559 irq is error,please check irq=%d\n", (s32)irq);
        return;
    }
    
    /*lint --e{746,718}*/
    spin_lock_irqsave(&hi6559_irq_data_st->irq_lock, irq_flags);

    offset = irq - IRQ_HI6559_NR_BASE;
    if (offset < HI6559_IRQM2_BASE_OFFSET) 
    {
        bsp_hi6559_reg_read(HI6559_IRQ1_MASK_OFFSET, &reg_temp);
        reg_temp |= (u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ1_MASK_OFFSET, reg_temp);
    } 
    else if(offset < HI6559_IRQM3_BASE_OFFSET)
    {
        offset -= HI6559_IRQM2_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ2_MASK_OFFSET, &reg_temp);
        reg_temp |= (u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ2_MASK_OFFSET, reg_temp);
    }
    else if(offset < HI6559_IRQM4_BASE_OFFSET)
    {
        offset -= HI6559_IRQM3_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ3_MASK_OFFSET, &reg_temp);
        reg_temp |= (u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ3_MASK_OFFSET, reg_temp);
    }
    else 
    {
        offset -= HI6559_IRQM4_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ4_MASK_OFFSET, &reg_temp);
        reg_temp |= (u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ4_MASK_OFFSET, reg_temp);
    }

    spin_unlock_irqrestore(&hi6559_irq_data_st->irq_lock, irq_flags);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_irq_unmask
 功能描述  : 不屏蔽中断
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_irq_unmask(u32 irq)
{
    hi6559_irqflags_t irq_flags = 0;
    u32 offset = 0;
    u8 reg_temp = 0;

    /* 参数合法性检查 */
    if (irq > IRQ_HI6559_NR_BASE + HI6559_IRQ_NR) 
    {
        pmic_print_error("hi6559 irq is error,please check irq=%d\n", (s32)irq);
        return ;
    }
    spin_lock_irqsave(&hi6559_irq_data_st->irq_lock, irq_flags);

    offset = irq - IRQ_HI6559_NR_BASE;
    if (offset < HI6559_IRQM2_BASE_OFFSET) 
    {
        bsp_hi6559_reg_read(HI6559_IRQ1_MASK_OFFSET, &reg_temp);
        reg_temp &= ~(u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ1_MASK_OFFSET, reg_temp);
    } 
    else if(offset < HI6559_IRQM3_BASE_OFFSET)
    {
        offset -= HI6559_IRQM2_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ2_MASK_OFFSET, &reg_temp);
        reg_temp &= ~(u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ2_MASK_OFFSET, reg_temp);
    }
    else if(offset < HI6559_IRQM4_BASE_OFFSET)
    {
        offset -= HI6559_IRQM3_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ3_MASK_OFFSET, &reg_temp);
        reg_temp &= ~(u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ3_MASK_OFFSET, reg_temp);
    }
    else 
    {
        offset -= HI6559_IRQM4_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ4_MASK_OFFSET, &reg_temp);
        reg_temp &= ~(u8)(1 << offset);
        bsp_hi6559_reg_write(HI6559_IRQ4_MASK_OFFSET, reg_temp);
    }

    spin_unlock_irqrestore(&hi6559_irq_data_st->irq_lock,irq_flags);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_irq_is_masked
 功能描述  : 检查某个中断是否被屏蔽
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
s32 hi6559_irq_is_masked(u32 irq)
{
    u32 offset = 0;
    u8 reg_temp = 0;

    /* 参数合法性检查 */
    if (irq > IRQ_HI6559_NR_BASE+HI6559_IRQ_NR) 
    {
        pmic_print_error("hi6559 irq is error,please check irq=%d\n", (s32)irq);
        return HI6559_IRQ_ERROR;
    }

    offset = irq - IRQ_HI6559_NR_BASE;
    if (offset < HI6559_IRQM2_BASE_OFFSET) 
    {
        bsp_hi6559_reg_read(HI6559_IRQ1_MASK_OFFSET, &reg_temp);
    } 
    else if(offset < HI6559_IRQM3_BASE_OFFSET)
    {
        offset -= HI6559_IRQM2_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ2_MASK_OFFSET, &reg_temp);
    }
    else if(offset < HI6559_IRQM4_BASE_OFFSET)
    {
        offset = HI6559_IRQM3_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ3_MASK_OFFSET, &reg_temp);
    }
    else 
    {
        offset = HI6559_IRQM4_BASE_OFFSET;
        bsp_hi6559_reg_read(HI6559_IRQ4_MASK_OFFSET, &reg_temp);
    }    

    return  (reg_temp & (1 << offset)) ;
}

/*****************************************************************************
 函 数 名  : hi6559_irq_callback_register
 功能描述  : 回调函数注册
 输入参数  : @irq: 中断号
             @routine: 回调函数
             @data: 回调函数的参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
s32 hi6559_irq_callback_register(u32 irq, hi6559funcptr routine, void *data)
{
    /* 参数合法性检查 */
    if (irq >= IRQ_HI6559_NR_BASE + HI6559_IRQ_NR) 
    {
        pmic_print_error("hi6559 irq is error,please check irq=%d\n", (s32)irq);
        return  HI6559_IRQ_ERROR;
    }

    Hi6559IrqInfo[irq].routine = routine;
    Hi6559IrqInfo[irq].data = data;
    return  HI6559_IRQ_OK;
}

/*****************************************************************************
 函 数 名  : hi6559_irq_wk_handler
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_irq_wk_handler(void)
{
    u32 reg = 0;
    u8 i = 0, reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg_tmp = 0;
    u8 flag = 0; /*power 键按下和抬起中断同时过来的标志*/

    /* 读所有中断 */
    bsp_hi6559_reg_read(HI6559_IRQ1_OFFSET, &reg1);
    bsp_hi6559_reg_read(HI6559_IRQ2_OFFSET, &reg2);
    bsp_hi6559_reg_read(HI6559_IRQ3_OFFSET, &reg3);
    bsp_hi6559_reg_read(HI6559_IRQ4_OFFSET, &reg4);
    reg = ((u32)reg1 | 
          ((u32)reg2 << HI6559_IRQM2_BASE_OFFSET) | 
          ((u32)reg3 << HI6559_IRQM3_BASE_OFFSET) | 
          ((u32)reg4 << HI6559_IRQM4_BASE_OFFSET));

    /* 按键中断press和release中断同时发生，则flag置1 */
    if (unlikely(0x60 == (0x60 & reg1))) {
        flag = 1;
    }

    
    /* 逐个寄存器清中断 */
    if (reg1) 
    {
        bsp_hi6559_reg_write(HI6559_IRQ1_OFFSET, reg1);
    }

    if (reg2) 
    {
        if (reg2 & 0x01)    /* 发生了过流/短路中断 */
        {
            for(i = 0; i < HI6559_NP_OCP_SCP_REG_NUM; i++)
            {
                /* 清除短路/过流中断状态寄存器 */
                bsp_hi6559_reg_read(HI6559_IRQ2_OFFSET, &reg_tmp);
                if(reg_tmp)
                {
                    bsp_hi6559_reg_write((HI6559_SCP_RECORD1_OFFSET + i), 0xFF);
                }
            }
        }
        bsp_hi6559_reg_write(HI6559_IRQ2_OFFSET, reg2);
    }

    if (reg3) 
    {
        bsp_hi6559_reg_write(HI6559_IRQ3_OFFSET, reg3);
    }

    if (reg4)
    {
        /* sim拔出中断，使用拔出的下跳沿 */
        if(reg4 & 0x04)
        {
            /* 先关闭电源再清中断，否则热插拔功能失效,修改为任务队列，故可以使用锁 */
            /* 后续需分析是否会引起任务处理时间长，影响中断响应 */
            bsp_hi6559_volt_disable(PMIC_HI6559_LDO09);
        }

        bsp_hi6559_reg_write(HI6559_IRQ4_OFFSET, reg4);
    }


    if (reg) 
    {
        for (i = 0; i < HI6559_IRQ_NR; i++) 
        {
            if ((1 == flag) && (5 == i))
            {
                /* 按键中断press和release中断同时发生，先处理press中断，这样更符合逻辑 */
                Hi6559IrqInfo[6].cnt++;    
                if (NULL != Hi6559IrqInfo[6].routine)
                    Hi6559IrqInfo[6].routine(Hi6559IrqInfo[6].data);
                Hi6559IrqInfo[5].cnt++;
                if (NULL != Hi6559IrqInfo[5].routine)
                    Hi6559IrqInfo[5].routine(Hi6559IrqInfo[5].data);
                i = i + 2;
            }
            
            if (reg & ((u32)1 << i)) 
            {
                Hi6559IrqInfo[i].cnt++;
                /* 中断处理 */
                if (NULL != Hi6559IrqInfo[i].routine)
                {
                    Hi6559IrqInfo[i].routine(Hi6559IrqInfo[i].data);
                }
            }
        }
    }
    pmic_print_info("**********hi6559_irq_wk_handler**********\n");
    hi6559_irq_enable();

    return;
}

#ifdef CONFIG_MODULE_VIC

/*****************************************************************************
 函 数 名  : hi6559_irq_handler
 功能描述  : 中断处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_irq_handler(u32 irq)
{
    /* 关闭中断,中断处理任务完成后打开 */
    hi6559_irq_disable();
    pmic_print_info("**********hi6559_irq_handler**********\n");
    queue_work(hi6559_irq_data_st->hi6559_irq_wq, &hi6559_irq_data_st->hi6559_irq_wk);

    return;
}
#else
/*****************************************************************************
 函 数 名  : hi6559_irq_handler
 功能描述  : 中断处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
irqreturn_t hi6559_irq_handler(s32 irq, void *data)
{
    struct hi6559_irq_data *hi6559_irq_data = data;
    
    /* 关闭中断,中断处理任务完成后打开 */
    hi6559_irq_disable();
    pmic_print_info("**********hi6559_irq_handler**********\n");
    queue_work(hi6559_irq_data->hi6559_irq_wq, &hi6559_irq_data->hi6559_irq_wk);

    return IRQ_HANDLED;
}
#endif


/*****************************************************************************
 函 数 名  : hi6559_irq_probe
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static s32 __init hi6559_irq_probe(struct platform_device *pdev)
{
    s32 ret = BSP_PMU_OK;
    u16 i = 0;
    u8 regvalue = 0;

    hi6559_irq_data_st = kzalloc(sizeof(*hi6559_irq_data_st), GFP_KERNEL);
    if (hi6559_irq_data_st == NULL) 
    {
        pmic_print_error("hi6559 irq kzalloc is failed,please check!\n");
        return -ENOMEM;
    }
    hi6559_irq_data_st->irq = (unsigned)platform_get_irq(pdev, 0);

    spin_lock_init(&hi6559_irq_data_st->irq_lock);/*中断只在Acore实现，多core*/

    /* 初始化工作队列 */
    hi6559_irq_data_st->hi6559_irq_wq = create_singlethread_workqueue("bsp_hi6559_irq");
    INIT_WORK(&hi6559_irq_data_st->hi6559_irq_wk, (void *)hi6559_irq_wk_handler);

    /* 处理usb拔出误报,若usb不在位，多报usb拔出无影响 */
    bsp_hi6559_reg_read(HI6559_IRQ2_OFFSET, &regvalue);
    if((regvalue & (0x01 << 2)) && hi6559_usb_state_get())
    {
        bsp_hi6559_reg_write(HI6559_IRQ2_OFFSET, 0x04);
    }
    /* 清除所有下电中断寄存器 */
    for(i = 0;i < HI6559_IRQ_REG_NUM; i++)
    {
        bsp_hi6559_reg_read(HI6559_IRQ1_OFFSET + i,&regvalue);
        bsp_hi6559_reg_write(HI6559_IRQ1_OFFSET + i, regvalue);
    }
    /* 清除短路过流中断寄存器，这样过流中断位才能消除 */
    for(i = 0; i < HI6559_NP_OCP_SCP_REG_NUM; i++)
    {
        bsp_hi6559_reg_read(HI6559_SCP_RECORD1_OFFSET + i, &regvalue);
        bsp_hi6559_reg_write(HI6559_SCP_RECORD1_OFFSET + i, regvalue);
    }

    /* 屏蔽所有中断 */
    for(i = 0;i < HI6559_IRQ_REG_NUM;i++)
    {
        bsp_hi6559_reg_write(HI6559_IRQ1_MASK_OFFSET + i, 0xff);
    }
    
    /* 中断注册 */
#ifdef CONFIG_MODULE_VIC
    ret = bsp_vic_connect(hi6559_irq_data_st->irq, hi6559_irq_handler,(s32)(hi6559_irq_data_st->irq));
    if (ret < 0) 
    {
        pmic_print_error("unable to connect PMIC HI6559 IRQ!\n");
        goto out;
    }
    ret = bsp_vic_enable(hi6559_irq_data_st->irq);
    if (ret < 0) {
        pmic_print_error("enable PMIC HI6559 IRQ error!\n");
        goto out;
    }
#else
    ret = request_irq((hi6559_irq_data_st->irq), hi6559_irq_handler, IRQF_DISABLED,
            "hi6559_irq", hi6559_irq_data_st);
    if (ret < 0) 
    {
        pmic_print_error("unable to request PMIC HI6559 IRQ!\n");
        goto out;
    }
#endif

    /* 打开所有中断 */
    for(i = 0; i < HI6559_IRQ_REG_NUM; i++)
    {
        bsp_hi6559_reg_write(HI6559_IRQ1_MASK_OFFSET + i, 0x0);
    }
    
    platform_set_drvdata(pdev, hi6559_irq_data_st);
    pmic_print_error("hi6559_irq_probe ok !\n");

    return ret;

out:
    kfree(hi6559_irq_data_st);
    return ret;

}

/*****************************************************************************
 函 数 名  : hi6559_irq_remove
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static  s32 hi6559_irq_remove(struct platform_device *pdev)
{
    struct hi6559_irq_data *hi6559_irq_data_pt;
     
    /* get hi6559_irq_data */
    hi6559_irq_data_pt = platform_get_drvdata(pdev);
    if (NULL == hi6559_irq_data_pt) {
        pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
        return BSP_PMU_ERROR;
    }

    /*release memory*/
    kfree(hi6559_irq_data_pt);
    return BSP_PMU_OK;
}

/*HI6559 irq 设备注册*/
static struct resource hi6559_irq_resources[] = {
    [0] = {
    },
    [1] = {
#ifdef CONFIG_MODULE_VIC
        .start  =   INT_LVL_SMART   ,
        .end    =   INT_LVL_SMART   ,
#else
        .start  =   INT_LVL_PMU   ,
        .end    =   INT_LVL_PMU    ,
#endif
        .flags  =   IORESOURCE_IRQ,
    },
};

static struct platform_device hi6559_irq_device = {
    .name =  "hi6559-irq",
    .id = 0,
    .dev = 
    {
        .platform_data  = NULL,
    },
    .num_resources = ARRAY_SIZE(hi6559_irq_resources),
    .resource = hi6559_irq_resources,
};

static struct platform_driver hi6559_irq_driver = {
    .probe = hi6559_irq_probe,
    .remove = hi6559_irq_remove,
    .driver     = 
    {
        .name   = "hi6559-irq",
    },
};

/*****************************************************************************
 函 数 名  : hi6559_irq_init
 功能描述  : 初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static s32 __init hi6559_irq_init(void)
{
    s32 ret = 0;
    ret = platform_device_register(&hi6559_irq_device);
    ret |= platform_driver_register(&hi6559_irq_driver);
    if (ret != 0)
    {
        pmic_print_error("Failed to register hi6559_hi6559_irq driver or device: %d\n", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : hi6559_irq_exit
 功能描述  : 注销
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static void __exit hi6559_irq_exit(void)
{
    platform_driver_unregister(&hi6559_irq_driver);
    platform_device_unregister(&hi6559_irq_device);
}

module_exit(hi6559_irq_exit);
subsys_initcall(hi6559_irq_init);
MODULE_LICENSE("GPL");
