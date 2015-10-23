/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_apb_mipi.c */
/* Version       : 2.0 */
/* Created       : 2013-03-13*/
/* Last Modified : */
/* Description   :  The C union definition file for the module apb_mipi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
/*lint --e{537}*/
#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include "soc_memmap.h"
#include <osl_bio.h>
#include "bbp_mipi_balong.h"
#include "hi_bbp_mipi.h"
#include <osl_irq.h>
#include "bsp_version.h"
#include "bsp_hardtimer.h"
/*****************************************************************************
*
*      全局变量
*
*****************************************************************************/

u32 BBP_MIPI_BASE_ADDR[MIPI_BUTT];
static MIPI_INIT_STRU mipi_init;
static u32 mipi_init_flag = 0;
/*****************************************************************************
*
*      函数声明
*
*****************************************************************************/

int bsp_mipi_config_frame( u64 trans_type,u64 reg_addr, u64 data,MIPI_CMD_STRU *cmd);


u32 bsp_mipi_parity_check(u8 check_data )
{
    u32 bit=0;
    u32 mask=0;
    u32 count = 0;

    /* 统计每个bit位置上多少为1 */
    for( bit = 0 ; bit < sizeof(u8)*BIT_CNT+1 ; ++bit)
    {
        mask = (u16)0x1 << bit;
        if(mask == (check_data & mask))
        {
            count++;
        }
    }

    /* 若1的个数为奇数个，则奇偶校验位为0，否则奇偶校验位1 */
    if(1 == (count%2))
    {
        return 0;
    }
    return 1;
}


int bsp_mipi_data_rev(u8 reg_addr,u8 *data,MIPI_CTRL_ENUM mipi_id)
{
	MIPI_CMD_STRU mipi_cmd;
	u32 count=0;
	u32 reg_val=0;
	u32 data_high;
	u32 data_low;
	u64 reg_addr_temp=reg_addr;
	*data=0xff;
	unsigned long flag=0;

	if((0x0 == reg_addr)||(0x0 == data))
	{
		mipi_print_error("ERROR:para is invalid!\n");
		return MIPI_ERROR;
	}

	/*配置帧数据格式*/
	bsp_mipi_config_frame(MIPI_EXTENDED_READ,reg_addr_temp,0, &mipi_cmd);

	/*加锁保护寄存器*/
	local_irq_save(flag);

	/*拉高DSP_MIPI_EN_IMI，屏蔽xbbp的其他请求*/
	set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(1,mipi_id);

	/*写入数据帧*/
	set_bbp_mipi_dsp_mipi0_wdata_low_dsp_mipi0_wdata_low(mipi_cmd.mipi_cmd_low,mipi_id);
	set_bbp_mipi_dsp_mipi0_wdata_high_dsp_mipi0_wdata_high(mipi_cmd.mipi_cmd_high,mipi_id);

	/*循环等待MIPI_GRANT_DSP为高，mipi_grant_dsp为0时表示mipi正在响应任务，响应完毕将其置为1*/
	count=0;
	reg_val= get_bbp_mipi_mipi0_grant_dsp_mipi0_grant_dsp(mipi_id);

	while(MIPI_GRANT_DSP_FLAG_IS_HIGH != reg_val)
	{
        if(++count > MIPI_REG_WAIT_TIMEOUT)
        {
            /*拉低DSP_MIPI_EN_IMI*/
            set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(0,mipi_id);
            mipi_print_error("wait for MIPI_GRANT_DSP timeout: %d\n",count);
	    	local_irq_restore(flag);
            return MIPI_ERROR;
        }

    	reg_val= get_bbp_mipi_mipi0_grant_dsp_mipi0_grant_dsp(mipi_id);
	}

	/*向DSP_MIPI_CFG_IND_IMI中置1*/
	set_bbp_mipi_dsp_mipi0_cfg_ind_imi_dsp_mipi0_cfg_ind_imi(1,mipi_id);

	/*循环等待mipi_rd_end_flag_mipi0_soft_rd_end_flag为1，为1时表示读数据寄存器有效*/
	count=0;
	reg_val= get_bbp_mipi_rd_end_flag_mipi0_soft_rd_end_flag_mipi0_soft(mipi_id);

	while(RD_END_FLAG_MIPI_SOFT_FLAG_IS_HIGH != reg_val)
	{
		if(++count > MIPI_REG_WAIT_TIMEOUT)
		{
			/*拉低DSP_MIPI_EN_IMI*/
			set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(0,mipi_id);
			mipi_print_error("wait for RD_END_FLAG_MIPI_SOFT timeout: %d\n",count);
			local_irq_restore(flag);
			return MIPI_ERROR;
		}
		reg_val= get_bbp_mipi_rd_end_flag_mipi0_soft_rd_end_flag_mipi0_soft(mipi_id);

	}
    /*当状态位为1时*/
    /*从MIPI_RD_DATA_LOW/HIGH_SOFT[31:0]中获取数据，赋值给输出参数*/
	data_low=get_bbp_mipi_mipi0_rd_data_low_soft_mipi0_rd_data_low_soft(mipi_id);
	data_high=get_bbp_mipi_mipi0_rd_data_high_soft_mipi0_rd_data_high_soft(mipi_id);

    /*向DSP_MIPI_RD_CLR写入1，清RD_END_FLAG_MIPI_SOFT标志位*/
    set_bbp_mipi_dsp_mipi0_rd_clr_dsp_mipi0_rd_clr(1,mipi_id);

	/*拉低DSP_MIPI_EN_IMI*/
    set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(0,mipi_id);

	/*释放互斥锁*/
    local_irq_restore(flag);

	*data=MIPI_EXTENDED_WR_FIRST_DATA_FRAME_GET(data_high, data_low);
	return MIPI_OK;
}


int bsp_mipi_data_send(u8 reg_addr,u8 data,MIPI_CTRL_ENUM mipi_id)
{
	MIPI_CMD_STRU mipi_cmd;
	u32 count=0;
	u32 reg_val=0;
	unsigned long flag = 0;
	if((0x0 == reg_addr))
	{
		mipi_print_error("ERROR:para is invalid!\n");
		return MIPI_ERROR;
	}

	/*配置帧数据格式*/
	bsp_mipi_config_frame(MIPI_EXTENDED_WRITE,(u64)reg_addr,(u64)data, &mipi_cmd);

	/*加锁保护寄存器*/
	local_irq_save(flag);

	/*拉高DSP_MIPI_EN_IMI,屏蔽xbbp的其他信号*/
	set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(1,mipi_id);


	/*写入数据帧*/
	set_bbp_mipi_dsp_mipi0_wdata_low_dsp_mipi0_wdata_low(mipi_cmd.mipi_cmd_low,mipi_id);
	set_bbp_mipi_dsp_mipi0_wdata_high_dsp_mipi0_wdata_high(mipi_cmd.mipi_cmd_high,mipi_id);

	/*循环等待MIPI_GRANT_DSP为高，其为0表示正在执行任务，为1表示任务执行完毕*/
	count=0;
	reg_val= get_bbp_mipi_mipi0_grant_dsp_mipi0_grant_dsp(mipi_id);

	while(MIPI_GRANT_DSP_FLAG_IS_HIGH != reg_val)
	{
        if(++count > MIPI_REG_WAIT_TIMEOUT)
        {
            /*拉低DSP_MIPI_EN_IMI*/
            set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(0,mipi_id);
            mipi_print_error("wait for MIPI_GRANT_DSP timeout: %d\n",count);
	    	local_irq_restore(flag);
            return MIPI_ERROR;
        }
    	reg_val= get_bbp_mipi_mipi0_grant_dsp_mipi0_grant_dsp(mipi_id);
	}

	/*向DSP_MIPI_CFG_IND_IMI中置1*/
	set_bbp_mipi_dsp_mipi0_cfg_ind_imi_dsp_mipi0_cfg_ind_imi(1,mipi_id);

	/*按照芯片要求，延时1us
	while循环按照4条指令算， CPU频率按400M计算*/
	udelay(1);

	/*拉低DSP_MIPI_EN_IMI*/
    set_bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi(0,mipi_id);

  	/*释放互斥锁*/
    local_irq_restore(flag);

	return MIPI_OK;

}



int bsp_mipi_config_frame( u64 trans_type,u64 reg_addr, u64 data,MIPI_CMD_STRU *cmd)
{
	u64 mipi_cmd;
	u64 parity1;
	u64 parity2;
	u64 parity3;
	u8 cmd_type;
	u64 end_flag=0;

	if(MIPI_EXTENDED_READ == trans_type)
	{
		cmd_type = MIPI_CMD_TYPE_READ;
		end_flag = MIPI_READ_END_FLAG;
	}
	else{
		cmd_type = MIPI_CMD_TYPE_WRITE;
		end_flag = MIPI_WRITE_END_FLAG;
	}

	parity1 = bsp_mipi_parity_check(((cmd_type)<<MIPI_CMD_TYPE_PARITY1_OFFSET)|mipi_init.byte_cnt);
	parity2 = bsp_mipi_parity_check(reg_addr);
	parity3 = bsp_mipi_parity_check((u16)data);

	/*MIPI 帧格式:
	(63bit-61bit)3bit,指令类型
	(60bit-57bit)4bit,slave地址
	(56bit-53bit)4bit,保留
	(52bit-49bit)4bit,传送字节数
	(48bit-48bit)1bit,奇偶校验
	(47bit-40bit)8bit,slave地址
	(39bit-39bit)1bit,奇偶校验
	(38bit-03bit)36bit,数据
	(34bit-34bit)1bit,end_flag
	(02bit-00bit)3bit,保留
	*/
	mipi_cmd = ((trans_type << MIPI_TRANS_TYPE_OFFSET)
				   |((u64)(mipi_init.slave_addr)<<MIPI_SLAVE_ADDR_OFFSET)
				   |((u64)(cmd_type)<<MIPI_CMD_TYPE_OFFSET)
				   |((u64)(mipi_init.byte_cnt)<<MIPI_BYTE_CNT_OFFSET)
				   |(((u64)parity1)<<MIPI_PARITY1_OFFSET)
				   |(reg_addr<<MIPI_REG_ADDR_OFSET)
				   |(((u64)parity2)<<MIPI_PARITY2_OFFSET)
				   |(end_flag<<MIPI_END_FLAG_OFFSET)
				   |(data<<MIPI_DATA_OFFSET)
				   |(((u64)parity3)<<MIPI_PARITY3_OFFSET));
	cmd->mipi_cmd_high= (mipi_cmd>>MIPI_HIGH_OFFSET) & (LOW_32BIT_MASK);
	cmd->mipi_cmd_low= mipi_cmd & (LOW_32BIT_MASK);

	return MIPI_OK;

}



int bsp_mipi_init(void)
{
	if(MIPI_IS_INITIALIZED_FLAG==mipi_init_flag)
	{
		mipi_print_info("bsp mipi init OK!\n");
		return MIPI_OK;
	}

	/*初始化全局变量*/
	mipi_init.byte_cnt=0;	/*实际的字节数为byte_cnt+1*/
	mipi_init.slave_addr=MIPI_PASTAR_ADDR;

	BBP_MIPI_BASE_ADDR[MIPI_0] = HI_CTU_BASE_ADDR;

	BBP_MIPI_BASE_ADDR[MIPI_1] =BBP_MIPI_BASE_ADDR[MIPI_0]+0x300;

	return MIPI_OK;

}

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */


