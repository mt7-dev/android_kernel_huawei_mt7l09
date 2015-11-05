/*
 * efuse_balong.c - hisilicon balong efuse driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/printk.h>
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
#include "hi_base.h"
#include "hi_efuse.h"
#include "hi_efuse.h"
#include "bsp_memmap.h"

#include "bsp_clk.h"
#include "bsp_icc.h"
#include "bsp_hardtimer.h"
#include "bsp_om.h"
#include "bsp_efuse.h"


struct work_struct efuse_work;

EFUSE_DATA_STRU efuse_msg ;

int efuse_debug_flag = 1;

void efuse_debug_flag_ctrl(int flag)
{
    efuse_debug_flag = flag;
}

void bsp_efuse_show(void)
{
    unsigned int i = 0;
    unsigned int value = 0;
    
    for(i = 0;i < EFUSE_MAX_SIZE;i++)
    {
        if(OK == bsp_efuse_read(&value,i, 1))
        {
            efuse_print_info("efuse group%d value = 0x%x.\n ", i, value);
        }
        else
        {
            efuse_print_error("efuse group%d read fail.\n", i);
            return;
        }
        
    }

}

void efuse_handle_work(struct work_struct *work)
{
    u32 i = 0;
    int length = 0;
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_EFUSE;
    EFUSE_DATA_STRU *msg = &efuse_msg;

    if(EFUSE_READ == msg->opt)
    {
        if(1 == efuse_debug_flag)
        {
            pr_info("efuse read start group %d length %d.\n", msg->start, msg->len);
        }
        
        msg->ret = bsp_efuse_read(msg->buf, msg->start,msg->len);
        
        if(1 == efuse_debug_flag)
        {
            pr_info("efuse read end group %d length %d.\n", msg->start, msg->len);
            for(i = 0;i < msg->len;i++)
            {
                pr_info("efuse buf[%d] is 0x%x.\n", i, msg->buf[i]);
            }
            pr_info("efuse read ret %d\n", msg->ret);
        }
    }
    else if(EFUSE_WRITE == msg->opt)
    {
        if(1 == efuse_debug_flag)
        {
            pr_info("efuse write start group %d length %d.\n", msg->start, msg->len);
            for(i = 0;i < msg->len;i++)
            {
                pr_info("efuse buf[%d] is 0x%x.\n", i, msg->buf[i]);
            }
        }
        
        msg->ret = bsp_efuse_write(msg->buf, msg->start,msg->len);
        
        if(1 == efuse_debug_flag)
        {
            pr_info("efuse write ret %d.\n", msg->ret);
        }

    }
    else
    {
        msg->ret = EFUSE_ERROR;
    }

    length = bsp_icc_send(ICC_CPU_MODEM, channel_id, (unsigned char*)msg, sizeof(EFUSE_DATA_STRU));
    if(length != (int)sizeof(EFUSE_DATA_STRU))
    {
        efuse_print_error("send len(%x) != expected len(%x).\n", length, sizeof(EFUSE_DATA_STRU));
        return;
    }


}

void bsp_efuse_data_receive(void)
{	
    int length = 0;
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_EFUSE;
    
	length = bsp_icc_read(channel_id, (u8*)&efuse_msg, sizeof(EFUSE_DATA_STRU));
	if(length != (int)sizeof(EFUSE_DATA_STRU))
	{
		efuse_print_error("read len(%x) != expected len(%x).\n", length, sizeof(EFUSE_DATA_STRU));
		return;
	}
    
    schedule_work(&efuse_work);    
}

static int __init hi_efuse_comm_init(void)
{
    int ret = 0;
    u32 chan_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_EFUSE;
    
	INIT_WORK(&efuse_work, efuse_handle_work);
    
    ret = bsp_icc_event_register(chan_id, (read_cb_func)bsp_efuse_data_receive, NULL, NULL, NULL);
    if(0 != ret)
    {
        efuse_print_error("efuse icc register is error.\n");
        return EFUSE_ERROR;
    }

    efuse_print_info("efuse comm init ok.\n");
    
    return EFUSE_OK;
    
}

static void __exit hi_efuse_comm_exit(void)
{
    /* do nothing */
}

module_init(hi_efuse_comm_init);
module_exit(hi_efuse_comm_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("efuse comm driver for the Hisilicon anten plug in/out");
MODULE_LICENSE("GPL");


#ifdef __cplusplus
}
#endif

