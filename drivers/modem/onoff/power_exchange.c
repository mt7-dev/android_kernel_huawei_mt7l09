/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2011, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: power_exchange.c                                                */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2013-5-3                                                            */
/*                                                                           */
/* Description: power on information exchange managerment                    */
/*              with misc and sram                                           */
/*                                                                           */
/* Others:                                                                   */
/*****************************************************************************/
#include <hi_onoff.h>
#ifndef HI_ONOFF_PHONE
/*lint --e{537} */
#include <linux/kernel.h>
#include "bsp_sram.h"
#include "drv_onoff.h"
#include "power_com.h"
#include "power_exchange.h"
#include "power_inner.h"
#include "bsp_nandc.h"

/*****************************************************************************
 函 数 名  : clear_misc_message
 功能描述  : clear boot message in normal mode,
           : clear misc when exit recovery mode
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
int clear_misc_message(void)
{
	char* str_misc = "misc";
	int rt;

    rt = bsp_nand_erase(str_misc, 0);

    return rt;
}

/*****************************************************************************
 函 数 名  : power_on_wdt_cnt_set
 功能描述  : 清除狗复位计数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_on_wdt_cnt_set( void )
{
    power_info_s * power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->wdg_rst_cnt = 0;
}

/*****************************************************************************
 函 数 名  : power_on_wdt_cnt_get
 功能描述  : 获取狗复位计数值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int power_on_wdt_cnt_get( void )
{
    power_info_s * power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    return power_info->wdg_rst_cnt;
}

/*****************************************************************************
 函 数 名  : power_on_reboot_flag_set
 功能描述  : 设置重启标志
 输入参数  : power_off_reboot_flag 重启原因
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_on_reboot_flag_set( power_off_reboot_flag enFlag )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->last_shut_reason = (unsigned int)(enFlag);

    pr_dbg( "#########  power_on_reboot_flag_set = 0x%08X ######## \r\n", (unsigned int)enFlag);
}

/*****************************************************************************
 函 数 名  : power_on_reboot_flag_get
 功能描述  : 获取重启原因
 输入参数  : 无
 输出参数  : 无
 返 回 值  : power_off_reboot_flag 重启原因
 调用函数  :
 被调函数  :
*****************************************************************************/
power_off_reboot_flag power_on_reboot_flag_get( void )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    pr_dbg( "#########  power_on_reboot_flag_get = 0x%08X ######## \r\n", power_info->last_shut_reason );
    return (power_off_reboot_flag)power_info->last_shut_reason;
}

/*****************************************************************************
 函 数 名  : power_on_start_reason_set
 功能描述  : 设置开机原因
 输入参数  : power_on_start_reason 开机原因
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_on_start_reason_set( power_on_start_reason enReason )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->power_on_reason = (unsigned int)(enReason);

    pr_dbg( "#########  power_on_start_reason_set = 0x%08X ######## \r\n", enReason );
}

/*****************************************************************************
 函 数 名  : power_on_start_reason_get
 功能描述  : 获取开机原因
 输入参数  : 无
 输出参数  : 无
 返 回 值  : power_on_start_reason 开机原因
 调用函数  :
 被调函数  :
*****************************************************************************/
power_on_start_reason power_on_start_reason_get( void )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    pr_dbg("#########  power_on_start_reason_get = 0x%08X ######## \r\n", power_info->power_on_reason );
    return (power_on_start_reason)(power_info->power_on_reason);
}

/*****************************************************************************
 函 数 名  : power_on_start_reason_set
 功能描述  : 设置开机原因
 输入参数  : power_on_start_reason 开机原因
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_reboot_cmd_set( power_reboot_cmd cmd )
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    power_info->reboot_cmd = (unsigned int)(cmd);

    pr_dbg( "#########  power_reboot_cmd_set = 0x%08X ######## \r\n", cmd );
}

/*****************************************************************************
 函 数 名  : power_on_c_status_get
 功能描述  : 获取C核状态
 输入参数  : power_on_c_status_get 开机原因
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
c_power_st_e power_on_c_status_get(void)
{
    power_info_s *power_info = (power_info_s *)(SRAM_REBOOT_ADDR);

    pr_dbg( "#########  power_on_c_status_get = 0x%08X ######## \r\n", power_info->c_power_state );

    return (c_power_st_e)(power_info->c_power_state);

}

/*****************************************************************************
 函 数 名  : print_exchange_addr
 功能描述  : 打印sram区数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void print_exchange_addr( void )
{
    unsigned int * ulPtr = (unsigned int *)(SRAM_REBOOT_ADDR);

    pr_dbg("############ (Linux) Addr = 0x%08X \r\n    0x00 = 0x%08X, 0x04 = 0x%08X, 0x08 = 0x%08X, 0x0C = 0x%08X\r\n",
         SRAM_REBOOT_ADDR, *(ulPtr+0), *(ulPtr+1), *(ulPtr+2), *(ulPtr+3));
}
#endif

