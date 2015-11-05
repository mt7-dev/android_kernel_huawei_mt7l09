/*
 * efuse_balong.c - hisilicon balong efuse driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
/*lint -save -e537*/
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/regulator/consumer.h>

#include "product_config.h"
#include "osl_bio.h"
#include "osl_sem.h"
#include "osl_thread.h"
#include "osl_wait.h"
#include "osl_spinlock.h"
#include "soc_clk.h"
#include "bsp_memmap.h"
#include "hi_base.h"
#include "hi_efuse.h"

#include "bsp_clk.h"
#include "bsp_icc.h"
#include "bsp_hardtimer.h"
#include "bsp_om.h"
#include "bsp_efuse.h"


/*lint -restore*/

#define TIMEMAXDELAY   (0x1000000)       /*最大延迟时间*/

static struct spinlock efuse_lock;

typedef unsigned int (*punptr)(void);
/*********************************************************************************************  
Function:           test_bit_value
Description:        测试寄存器中的某位，是否等于指定的值
Calls:              无
Data Accessed:      无
Data Updated:       无
Input:              addr：  寄存器地址
                    bitMask：需要检测的位置
                    bitValue：指定的值
Output:             无
Return:             成功则返回TRUE， 失败则返回FALSE
Others:             无
*********************************************************************************************/
int test_bit_value(punptr pfun,  u32 bitValue)
{
    u32 time;                       /*延时时间*/
    
    for(time = 0; time < TIMEMAXDELAY; time++)
    {
        if(bitValue == (*pfun)())
        {
            return EFUSE_OK;
        }
    }
    
    return EFUSE_ERROR;
}

/*lint -save -e718 -e746*/
/*****************************************************************************
* 函 数 名  : bsp_efuse_read
*
* 功能描述  : 按组读取EFUSE中的数据
*
* 输入参数  : group  起始group
*                   num  数组长度(word数,不超过16)
* 输出参数  : pBuf ：EFUSE中的数据
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int bsp_efuse_read(u32* pBuf, const u32 group, const u32 num)
{
    u32* pSh = 0;
    u32 cnt = 0;
    
    unsigned long flags = 0;
    struct clk *efuse_clk = NULL;
       
    if((0 == num) || (group + num > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        efuse_print_error(" parameter error, group=%d, num=%d, pBuf=0x%x.\n", group, num, pBuf);        
        return EFUSE_ERROR;
    }
 
    efuse_clk = (struct clk *)clk_get(NULL,"efuse_clk");
	if(IS_ERR(efuse_clk))
    {
		efuse_print_error("efuse clk cannot get, 0x%x.\n", efuse_clk);
		return EFUSE_ERROR;
	}

    spin_lock_irqsave(&efuse_lock, flags);
    
    if(0 != clk_enable(efuse_clk))
    {
		efuse_print_error("efuse clk enable is fail, 0x%x.\n", efuse_clk);
		goto clk_enable_error;
    }
    
    /*判断是否处在power_down状态*/
    if(get_hi_efusec_status_pd_status())
    {
       /*退出powerd_down状态*/
        set_hi_efusec_cfg_pd_en(0);
       
        /* 等待powerd_down退出标志设置成功，否则超时返回错误 */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            efuse_print_error(" powerd_down disable is fail .\n");
            goto efuse_rd_error;
        }
    }

#ifdef HI_RF_STATUS_OFFSET
    /*设置efuse接口时序为内部产生*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*选择信号为apb操作efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    pSh = pBuf;
    
    /* 循环读取efuse值 */
    for(cnt = 0; cnt < num; cnt++)
    {
        /* 设置读取地址 */
        set_hi_efuse_group_efuse_group( group+cnt);
        
        /* 使能读 */
        set_hi_efusec_cfg_rden(1);
        
        /* 等待读完成 */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_rd_status, 1))
        {
            efuse_print_error(" read finish is fail .\n");
            goto efuse_rd_error;
        }
        /* 读取数据 */
        *pSh = get_hi_efusec_data_efusec_data();
        pSh++;
        
    }
    
    clk_disable(efuse_clk);
    
    spin_unlock_irqrestore(&efuse_lock, flags);
    
    return EFUSE_OK;

efuse_rd_error:
    clk_disable(efuse_clk);
    
clk_enable_error:
    spin_unlock_irqrestore(&efuse_lock, flags);
    clk_put(efuse_clk);

    return EFUSE_ERROR;
}    
/*lint -restore*/

/*****************************************************************************
* 函 数 名  : bsp_efuse_write
*
* 功能描述  : 烧写Efsue
*
* 输入参数  : pBuf:待烧写的EFUSE值
*                 group,Efuse地址偏移
*                 len,烧写长度
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int bsp_efuse_write( u32 *pBuf, const u32 group, const u32 len )
{
    u32 i = 0;
    u32 u32Grp = 0;
    u32 *pu32Value = 0;

    unsigned long flags = 0;
    struct clk *efuse_clk = NULL;
    struct regulator *efuse_regulator = NULL;
    
    if((0 == len) || (group + len > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        efuse_print_error(" parameter error, group=%d, len=%d, pBuf=0x%x.\n", group, len, pBuf);
        return EFUSE_ERROR;
    }

    spin_lock_irqsave(&efuse_lock, flags);

#ifndef HI_RF_STATUS_OFFSET
    efuse_regulator = regulator_get(NULL, "EFUSE-vcc");
    if(NULL == efuse_regulator)
    {
        efuse_print_error("efuse regulator cannot get. \n");        
        goto regu_get_error;
    }

    if(0 != regulator_enable(efuse_regulator))
    {
        efuse_print_error("efuse regulator enable is fail. \n");
        goto regulator_error;
    }
    
#endif

    efuse_clk = (struct clk *)clk_get(NULL,"efuse_clk");
	if(IS_ERR(efuse_clk))
    {
		efuse_print_error("efuse clk cannot get, 0x%x.\n", efuse_clk);
		goto clk_get_error;
	}
    
    if(0 != clk_enable(efuse_clk))
    {
		efuse_print_error("efuse clk enable is fail, 0x%x.\n", efuse_clk);
		goto clk_enable_error;
    }
    
    udelay(1000);/*lint !e737*/
    
    /*判断是否处在power_down状态*/
    if(get_hi_efusec_status_pd_status())
    {
       /*退出powerd_down状态*/
        set_hi_efusec_cfg_pd_en(0);
        /* 等待powerd_down退出标志设置成功，否则超时返回错误 */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            efuse_print_error(" power_down disable is fail .\n");
            goto efuse_wt_error;
        }
    }
    
    /* 判断是否允许烧写,为1禁止烧写,直接退出 */
    if(get_hi_hw_cfg_hw_cfg() & 0x1)
    {
        efuse_print_error(" burn write is disable .\n");
        goto efuse_wt_error;
    }

#ifdef HI_RF_STATUS_OFFSET
    /*设置efuse接口时序为内部产生*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*选择信号为apb操作efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    /* 配置时序要求 */
    set_hi_efusec_count_efusec_count(EFUSE_COUNT_CFG);
    set_hi_pgm_count_pgm_count(PGM_COUNT_CFG);
    
    /* 使能预烧写 */
    set_hi_efusec_cfg_pre_pg(1);
    
    /*查询是否使能置位 */
    if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pgenb_status, 1))
    {
        efuse_print_error(" pre write enable is fail .\n");
        goto efuse_wt_error;
    }
    
    /* 循环烧写 */
    u32Grp = group;
    pu32Value = pBuf;
    for (i = 0; i < len; i++)
    {
        /* 设置group */
        set_hi_efuse_group_efuse_group(u32Grp);
        
        /* 设置value */
        set_hi_pg_value_pg_value(*pu32Value);
        
        /* 使能烧写 */
        set_hi_efusec_cfg_pgen(1);
        
        /* 查询烧写完成 */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pg_status, 1))
        {
            efuse_print_error(" burn write finish is fail .\n");
            goto efuse_wt_error;
        }
        
        /* 烧写下一组 */
        u32Grp++;
        pu32Value++;
    }

    /*去使能预烧写*/
    set_hi_efusec_cfg_pre_pg(0);
    
    clk_disable(efuse_clk);

#ifndef HI_RF_STATUS_OFFSET
    regulator_put(efuse_regulator);
#endif

    spin_unlock_irqrestore(&efuse_lock, flags);
     
    return EFUSE_OK;

efuse_wt_error:
    clk_disable(efuse_clk);
    
clk_enable_error:
    clk_put(efuse_clk);
    
clk_get_error:
    (void)regulator_disable(efuse_regulator);

regulator_error:
    regulator_put(efuse_regulator);
regu_get_error:
    spin_unlock_irqrestore(&efuse_lock, flags);
    
    return EFUSE_ERROR;
}


static int __init hi_efuse_init(void)
{    
    spin_lock_init(&efuse_lock);    
    
    efuse_print_info("efuse init ok.\n");
    
    return EFUSE_OK;
    
}

static void __exit hi_efuse_exit(void)
{
    /* do nothing */
}

module_init(hi_efuse_init);
module_exit(hi_efuse_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("anten driver for the Hisilicon anten plug in/out");
MODULE_LICENSE("GPL");


#ifdef __cplusplus
}
#endif

