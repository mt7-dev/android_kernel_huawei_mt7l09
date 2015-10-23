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
#include <vxWorks.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>

#include "bsp_memmap.h"
#include <osl_bio.h>
#include <osl_spinlock.h>
#include "bbp_mipi_balong.h"
#include "hi_bbp_mipi.h"
#include "bsp_version.h"
#include "bsp_hardtimer.h"

/*****************************************************************************
*
*      全局变量
*
*****************************************************************************/

u32 BBP_MIPI_BASE_ADDR[MIPI_BUTT];
static MIPI_INIT_STRU mipi_init;

static spinlock_t mipi_ctrl_lock[MIPI_BUTT];

static u32 mipi_init_flag = 0;
/*****************************************************************************
*
*      函数声明
*
*****************************************************************************/

int bsp_mipi_config_frame(u64 trans_type, u64 slave_addr, u64 reg_addr, u64 data,MIPI_CMD_STRU *cmd);



static u32 bsp_mipi_parity_check(u32 check_data )
{
	/*lint -save -e958*/
    u32 bit=0;
    u32 mask=0;
    u32 count = 0;
	/*lint -restore*/
    /* 统计每个bit位置上多少为1 */
    for( bit = 0 ; bit < 32 ; ++bit)
    {
        mask = (unsigned int)1 << bit;
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


/*lint -save -e958*/
int bsp_mipi_data_rev(u8 type, u8 slave_addr, u8 reg_addr,u8 *data,MIPI_CTRL_ENUM mipi_id)
/*lint -restore*/
{
	/*lint -save -e958*/
	u64 reg_addr_temp = reg_addr;
	u64 trans_type = type;
    u64 trans_addr = slave_addr;
	u64 data_temp = 0;
	unsigned long flags;
	MIPI_CMD_STRU mipi_cmd;
	u32 count=0;
	u32 reg_val=0;
	u32 data_high;
	u32 data_low;
	/*lint -restore*/

	if(NULL == data){
		mipi_print_error("param [data] is null pointer! \n");
		return MIPI_ERROR;
	}

	*data=0xff;

	/*配置帧数据格式*/
	(void)bsp_mipi_config_frame(trans_type,trans_addr,reg_addr_temp,data_temp, &mipi_cmd);

	/*加锁保护寄存器*/
	spin_lock_irqsave(&mipi_ctrl_lock[mipi_id],flags);

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
		    spin_unlock_irqrestore(&mipi_ctrl_lock[mipi_id],flags);
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
			spin_unlock_irqrestore(&mipi_ctrl_lock[mipi_id],flags);
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
	spin_unlock_irqrestore(&mipi_ctrl_lock[mipi_id],flags);

    /*lint -save -e734*/
	*data=MIPI_EXTENDED_WR_FIRST_DATA_FRAME_GET(data_high, data_low);
    /*lint -restore*/
	return MIPI_OK;
}


/*lint -save -e958*/
int bsp_mipi_data_send(u8 type, u8 slave_addr, u8 reg_addr,u8 data,MIPI_CTRL_ENUM mipi_id)
/*lint -restore*/
{
	/*lint -save -e958*/
	u64 data_temp = data;
	u64 reg_addr_temp = reg_addr;
	u64 trans_type = type;
    u64 trans_addr = slave_addr;
	MIPI_CMD_STRU mipi_cmd;
	unsigned long flag;
	u32 count=0;
	u32 reg_val=0;
	/*lint -restore*/

	if((0x0 == reg_addr))
	{
		mipi_print_error("ERROR:para is invalid!\n");
		return MIPI_ERROR;
	}

	/*配置帧数据格式*/
	(void)bsp_mipi_config_frame(trans_type, trans_addr, reg_addr_temp, data_temp, &mipi_cmd);

	/*加锁保护寄存器*/
	spin_lock_irqsave(&mipi_ctrl_lock[mipi_id],flag);

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
	    	spin_unlock_irqrestore(&mipi_ctrl_lock[mipi_id],flag);
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
    spin_unlock_irqrestore(&mipi_ctrl_lock[mipi_id],flag);

	return MIPI_OK;

}



int bsp_mipi_config_frame(u64 trans_type, u64 slave_addr, u64 reg_addr, u64 data,MIPI_CMD_STRU *cmd)
{
	/*lint -save -e958*/
	u64 mipi_cmd;
	u64 parity1;
	u64 parity2;
	u64 parity3;
	u64 end_flag=0;
	u8 cmd_type;
	/*lint -restore*/

	if(MIPI_EXTENDED_READ == trans_type)
	{
		cmd_type = MIPI_CMD_TYPE_READ;
		end_flag = MIPI_READ_END_FLAG;
	}
	else{
		cmd_type = MIPI_CMD_TYPE_WRITE;
		end_flag = MIPI_WRITE_END_FLAG;
	}

	parity1 = bsp_mipi_parity_check((u32)((slave_addr << 8) | (cmd_type << MIPI_CMD_TYPE_PARITY1_OFFSET) | mipi_init.byte_cnt));
	parity2 = bsp_mipi_parity_check((u32)reg_addr);
	parity3 = bsp_mipi_parity_check((u32)data);

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
				   |((slave_addr)<<MIPI_SLAVE_ADDR_OFFSET)
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

static int bsp_mipi_chn_set(void)
{
	u32 ret;
	MIPI0_CHN_STRU mipi0_chn={0xff};/*记录mipi0控制器走哪个通道*/
	MIPI1_CHN_STRU mipi1_chn={0xff};/*记录mipi1控制器走哪个通道*/

    /*read nv,get the exc protect setting*/
    ret = bsp_nvm_read(NV_ID_DRV_NV_MIPI_0_CHN,(u8*)&mipi0_chn,sizeof(MIPI0_CHN_STRU));
    if(NV_OK != ret||mipi0_chn.mipi_chn>1)
    {
        ver_print_error("mipi0 init read nv error,not set mipi chn ,use the default config!\n");
        return VER_ERROR;
    }

	ret = bsp_nvm_read(NV_ID_DRV_NV_MIPI_1_CHN,(u8*)&mipi1_chn,sizeof(MIPI1_CHN_STRU));
    if(NV_OK != ret||mipi1_chn.mipi_chn>1)
    {
        ver_print_error("mipi1 init read nv error,not set mipi chn ,use the default config!\n");
        return VER_ERROR;
    }
	/*和逻辑确认两个mipi是否可以绑定同一个通道*/
	/*配置MIPI0的通道*/
	writel(mipi0_chn.mipi_chn,BBP_MIPI_BASE_ADDR[MIPI_0]+MIPI_CH_SEL0);
	/*配置MIPI1的通道*/
	writel(mipi1_chn.mipi_chn,BBP_MIPI_BASE_ADDR[MIPI_0]+MIPI_CH_SEL1);

	/*配置通道绑定,0为绑定，1为不绑定*/
	writel(1,BBP_MIPI_BASE_ADDR[MIPI_0]+MIPI_UNBIND_EN);

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

	/*创建信号量*/
	spin_lock_init(&mipi_ctrl_lock[MIPI_0]);
	spin_lock_init(&mipi_ctrl_lock[MIPI_1]);


	BBP_MIPI_BASE_ADDR[MIPI_0] = HI_CTU_BASE_ADDR;


	BBP_MIPI_BASE_ADDR[MIPI_1] =BBP_MIPI_BASE_ADDR[MIPI_0]+0x300;

	/*通道绑定*/
	/*当前通道默认配置均为0通道，其他根据特性NV识别*/
	bsp_mipi_chn_set();

	return MIPI_OK;
}

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

