/*
 * pmu_hi6551.c -- simple synchronous userspace interface to SSI devices
 *
 * Copyright (C) 2006 SWAPP
 *  Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*lint --e{537,958}*/
#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <drv_comm.h>
#include <asm/uaccess.h>
#endif

#include <product_config.h>
#include <osl_bio.h>
#include <osl_thread.h>
#include <bsp_nvim.h>

#include <osl_sem.h>
#include <osl_spinlock.h>
#include <hi_smartstar.h>
#include "dr_hi6551.h"

typedef unsigned long hi6551_drflags_t;
struct hi6551_dr_data{
    spinlock_t      lock;
};
struct hi6551_dr_data g_hi6551_dr_st;

int dr12_current_list[8] = {3000,6000,9000,12000,15000,18000,21000,24000};
int dr345_current_list[8] = {1000,1500,2000,2500,3000,3500,4000,4500};
/*单位:ms*/
unsigned int dr12_onoff_time_list[8] = {1,250,500,1000,2000,4000,4000,ALWAYS_ON_OFF_TIME_DR12};
unsigned int dr12_risefall_time_list[8] = {0,250,500,1000,2000,2500,3000,4000};
unsigned int dr345_onoff_time_list[16] = {0,500,1000,2000,4000,6000,8000,12000,14000,16000,ALWAYS_ON_OFF_TIME_DR345};
unsigned int dr345_risefall_time_list[16] = {0,250,500,1000,2000,4000};
/*dr属性表*/
PMIC_HI6551_DRS_ATTR hi6551_dr_attr[PMIC_HI6551_DR_MAX + 1] =
{
    [PMIC_HI6551_DR01] = {
        .fla_onoff_reg_addr      = HI6551_DR_FLA_CTRL2_OFFSET,
        .bre_onoff_reg_addr       = HI6551_DR_BRE_CTRL_OFFSET,
        .mode_sel_reg_addr   = HI6551_DR_MODE_SEL_OFFSET,
        .fla_mode_reg_addr   = HI6551_DR_FLA_CTRL2_OFFSET,
        .bre_mode_reg_addr      = HI6551_DR_BRE_CTRL_OFFSET,
        .bre_onoff_time_reg_adrr      = HI6551_DR1_TIM_CONF0_OFFSET,
        .bre_risefall_time_reg_adrr       = HI6551_DR1_TIM_CONF1_OFFSET,
        .current_reg_addr   = HI6551_DR1_ISET_OFFSET,
        .start_del_reg_addr = DR_INVAILD_ADDR,
        .fla_onoff_bit_offset     = 0,
        .bre_onoff_bit_offset    = 0,
        .mode_sel_bit_offset    = 0,
        .fla_mode_bit_offset = 1,
        .bre_mode_bit_offset = 1,
        .current_bit_offset      = 0x0,
        .current_bit_mask    = 0x07,
        .size_of_onoff_time_list = 8,
        .size_of_risefall_time_list = 8,
        .current_list          = dr12_current_list,
        .bre_onoff_time_list = dr12_onoff_time_list,
        .bre_risefall_time_list = dr12_risefall_time_list,
    },
    [PMIC_HI6551_DR02] = {
        .fla_onoff_reg_addr      = HI6551_DR_FLA_CTRL2_OFFSET,
        .bre_onoff_reg_addr       = HI6551_DR_BRE_CTRL_OFFSET,
        .mode_sel_reg_addr   = HI6551_DR_MODE_SEL_OFFSET,
        .fla_mode_reg_addr   = HI6551_DR_FLA_CTRL2_OFFSET,
        .bre_mode_reg_addr      = HI6551_DR_BRE_CTRL_OFFSET,
        .bre_onoff_time_reg_adrr      = HI6551_DR2_TIM_CONF0_OFFSET,
        .bre_risefall_time_reg_adrr       = HI6551_DR2_TIM_CONF1_OFFSET,
        .current_reg_addr   = HI6551_DR2_ISET_OFFSET,
        .start_del_reg_addr = DR_INVAILD_ADDR,
        .fla_onoff_bit_offset     = 4,
        .bre_onoff_bit_offset    = 4,
        .mode_sel_bit_offset    = 1,
        .fla_mode_bit_offset = 5,
        .bre_mode_bit_offset = 5,
        .current_bit_offset      = 0x0,
        .current_bit_mask    = 0x07,
        .size_of_onoff_time_list = 8,
        .size_of_risefall_time_list = 8,
        .current_list          = dr12_current_list,
        .bre_onoff_time_list = dr12_onoff_time_list,
        .bre_risefall_time_list = dr12_risefall_time_list,
    },
    [PMIC_HI6551_DR03] = {
        .fla_onoff_reg_addr      = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_onoff_reg_addr       = HI6551_DR_LED_CTRL_OFFSET,
        .mode_sel_reg_addr   = HI6551_DR_MODE_SEL_OFFSET,
        .fla_mode_reg_addr   = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_mode_reg_addr      = DR_INVAILD_ADDR,
        .bre_onoff_time_reg_adrr      = HI6551_DR3_TIM_CONF0_OFFSET,
        .bre_risefall_time_reg_adrr       = HI6551_DR3_TIM_CONF1_OFFSET,
        .current_reg_addr   = HI6551_DR3_ISET_OFFSET,
        .start_del_reg_addr = HI6551_DR3_START_DEL_OFFSET,
        .fla_onoff_bit_offset     = 0,
        .bre_onoff_bit_offset    = 0,
        .mode_sel_bit_offset    = 2,
        .fla_mode_bit_offset = 1,
        .bre_mode_bit_offset = DR_INVAILD_OFFSET,
        .current_bit_offset      = 0x0,
        .current_bit_mask    = 0x07,
        .size_of_onoff_time_list = 11,
        .size_of_risefall_time_list = 6,
        .current_list          = dr345_current_list,
        .bre_onoff_time_list = dr345_onoff_time_list,
        .bre_risefall_time_list = dr345_risefall_time_list,
    },
    [PMIC_HI6551_DR04] = {
        .fla_onoff_reg_addr      = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_onoff_reg_addr       = HI6551_DR_LED_CTRL_OFFSET,
        .mode_sel_reg_addr   = HI6551_DR_MODE_SEL_OFFSET,
        .fla_mode_reg_addr   = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_mode_reg_addr      = DR_INVAILD_ADDR,
        .bre_onoff_time_reg_adrr      = HI6551_DR4_TIM_CONF0_OFFSET,
        .bre_risefall_time_reg_adrr       = HI6551_DR4_TIM_CONF1_OFFSET,
        .current_reg_addr   = HI6551_DR4_ISET_OFFSET,
        .start_del_reg_addr = HI6551_DR4_START_DEL_OFFSET,
        .fla_onoff_bit_offset     = 2,
        .bre_onoff_bit_offset    = 1,
        .mode_sel_bit_offset    = 3,
        .fla_mode_bit_offset = 3,
        .bre_mode_bit_offset = DR_INVAILD_OFFSET,
        .current_bit_offset      = 0x0,
        .current_bit_mask    = 0x07,
        .size_of_onoff_time_list = 11,
        .size_of_risefall_time_list = 6,
        .current_list          = dr345_current_list,
        .bre_onoff_time_list = dr345_onoff_time_list,
        .bre_risefall_time_list = dr345_risefall_time_list,
    },
    [PMIC_HI6551_DR05] = {
        .fla_onoff_reg_addr      = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_onoff_reg_addr       = HI6551_DR_LED_CTRL_OFFSET,
        .mode_sel_reg_addr   = HI6551_DR_MODE_SEL_OFFSET,
        .fla_mode_reg_addr   = HI6551_DR_FLA_CTRL1_OFFSET,
        .bre_mode_reg_addr      = DR_INVAILD_ADDR,
        .bre_onoff_time_reg_adrr      = HI6551_DR5_TIM_CONF0_OFFSET,
        .bre_risefall_time_reg_adrr       = HI6551_DR5_TIM_CONF1_OFFSET,
        .current_reg_addr   = HI6551_DR5_ISET_OFFSET,
        .start_del_reg_addr = HI6551_DR5_START_DEL_OFFSET,
        .fla_onoff_bit_offset     = 4,
        .bre_onoff_bit_offset    = 2,
        .mode_sel_bit_offset    = 4,
        .fla_mode_bit_offset = 5,
        .bre_mode_bit_offset = DR_INVAILD_OFFSET,
        .current_bit_offset      = 0x0,
        .current_bit_mask    = 0x07,
        .size_of_onoff_time_list = 11,
        .size_of_risefall_time_list = 6,
        .current_list          = dr345_current_list,
        .bre_onoff_time_list = dr345_onoff_time_list,
        .bre_risefall_time_list = dr345_risefall_time_list,
    },
};
/*para check func*/
static __inline__ int hi6551_dr_para_check(int dr_id)
{
    /* 有效性检查*/
    if ((PMIC_HI6551_DR_MAX < dr_id) || (PMIC_HI6551_DR_MIN > dr_id))
    {
        pmic_print_error("PMIC doesn't have dr%d!\n",dr_id);
        pmic_print_error("the dr you can use in this PMIC is from %d to %d,please check!\n",PMIC_HI6551_DR_MIN,PMIC_HI6551_DR_MAX);

        return BSP_PMU_ERROR;
    }
    else
        return BSP_PMU_OK;
}

/*----------------------------------基本寄存器操作接口---------------------------------------*/
/*****************************************************************************
* 函 数 名  : hi6551_reg_read
*
* 功能描述  : 对pmu芯片寄存器的读函数
*
* 输入参数  : u16 addr：待写入的寄存器地址
*             u8 u8Data：读出的数据
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_dr_reg_write( u16 addr, u8 value)
{
    bsp_hi6551_reg_write(addr,value);
}
/*****************************************************************************
* 函 数 名  : hi6551_reg_read
*
* 功能描述  : 对pmu芯片寄存器的读函数
*
* 输入参数  : u16 addr：待写入的寄存器地址
*             u8 u8Data：读出的数据
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void  bsp_dr_reg_read( u16 addr, u8 *pValue)
{
    bsp_hi6551_reg_read(addr,pValue);
}
/*****************************************************************************
* 函 数 名  : hi6551_reg_write_mask
*
* 功能描述  : 对pmu芯片寄存器的某些bit置位
*
* 输入参数  : u16 addr：待写入的寄存器地址
*             u8 u8Data：读出的数据
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_dr_reg_write_mask(u16 addr, u8 value, u8 mask)
{
    bsp_hi6551_reg_write_mask(addr, value, mask);
}

int bsp_dr_reg_show(u16 addr)
{
    u8 Value = 0;

    bsp_dr_reg_read(addr, &Value);
    pmic_print_info("pmuRead addr 0x%x value is 0x%x!!\n",addr,Value);
    return Value;
}
/*----------------电流源设置相关接口,给regulator设置的接口-------------------*/

/*****************************************************************************
 函 数 名  : bsp_hi6551_dr_set_mode
 功能描述  : PMIC HI6551电源模式设置
            (支持normal和跟随PMU进入eco和强制进入eco三种模式,只有特定路支持)
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 只在Acore提供该函数实现，不再添加核间锁，只使用核内锁
*****************************************************************************/
int bsp_hi6551_dr_set_mode(int dr_id, dr_mode_e mode)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    u8 regval = 0;
    hi6551_drflags_t flags = 0;
    int iret = BSP_PMU_OK;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];
    /*lint --e{746,718}*/
    spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);

    switch(mode){
    case PMU_DRS_MODE_FLA_FLASH:
        /*先设置为闪烁模式*/
        pmic_print_info("dr_id[%d] will be set fla-flash mode\n", dr_id);

        if(dr_id >= PMU_DR03)
            bsp_dr_reg_write_mask(HI6551_DR_LED_CTRL_OFFSET, (0<<3),0x08);

        bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset);
        bsp_dr_reg_write(hi6551_dr->mode_sel_reg_addr, regval);
        /*开启闪亮模式*/
        bsp_dr_reg_read(hi6551_dr->fla_mode_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_dr->fla_mode_bit_offset);
        bsp_dr_reg_write(hi6551_dr->fla_mode_reg_addr, regval);
        break;
    case PMU_DRS_MODE_FLA_LIGHT:
        pmic_print_info("dr_id[%d] will be set fla-light mode\n", dr_id);

        if(dr_id >= PMU_DR03)
            bsp_dr_reg_write_mask(HI6551_DR_LED_CTRL_OFFSET, (0<<3),0x08);

        /*先设置为闪烁模式*/
        bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset);
        bsp_dr_reg_write(hi6551_dr->mode_sel_reg_addr, regval);
        /*关闭闪亮模式*/
        bsp_dr_reg_read(hi6551_dr->fla_mode_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->fla_mode_bit_offset);
        bsp_dr_reg_write(hi6551_dr->fla_mode_reg_addr, regval);
        break;
    case PMU_DRS_MODE_BRE:
        pmic_print_info("dr_id[%d] will be set normal bre mode\n", dr_id);
        bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset);
        bsp_dr_reg_write(hi6551_dr->mode_sel_reg_addr, regval);
        break;

    case PMU_DRS_MODE_BRE_FLASH:
        pmic_print_info("dr_id[%d] will be set bre-flash mode\n", dr_id);
        /*先设置为呼吸模式*/
        bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset);
        bsp_dr_reg_write(hi6551_dr->mode_sel_reg_addr, regval);
        /*再设置为呼吸闪烁模式*/
        if(DR_INVAILD_ADDR == hi6551_dr->bre_mode_reg_addr)/*dr3/4/5没有该模式*/
        {
            pmic_print_error("dr_id[%d] cann't support bre-flash mode,will be set normal bre mode \n", dr_id);
            iret = BSP_PMU_ERROR;
        }
        else
        {
            bsp_dr_reg_read(hi6551_dr->bre_mode_reg_addr, &regval);
            regval |= (u8)((u32)0x1 << hi6551_dr->bre_mode_bit_offset);
            bsp_dr_reg_write(hi6551_dr->bre_mode_reg_addr, regval);
        }
        break;

    case PMU_DRS_MODE_BRE_LIGHT:
        pmic_print_info("dr_id[%d] will be set breath-light mode\n", dr_id);
        /*先设置为呼吸模式*/
        bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset);
        bsp_dr_reg_write(hi6551_dr->mode_sel_reg_addr, regval);
        /*再设置为呼吸闪烁模式*/
        if(DR_INVAILD_ADDR == hi6551_dr->bre_mode_reg_addr)/*dr3/4/5没有该模式*/
        {
            pmic_print_error("dr_id[%d] cann't support breath-light mode,will be set normal bre mode \n", dr_id);
        }
        else
        {
            bsp_dr_reg_read(hi6551_dr->bre_mode_reg_addr, &regval);
            regval &= ~(u8)((u32)0x1 << hi6551_dr->bre_mode_bit_offset);
            bsp_dr_reg_write(hi6551_dr->bre_mode_reg_addr, regval);
        }
        break;

    default:
        pmic_print_error("cann't support the mode\n");
        iret = BSP_PMU_ERROR;
        break;
    }

    spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);

    return iret;

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_dr_get_mode
 功能描述  : PMIC HI6551电源模式查询
            (支持闪烁和呼吸)
            (DR1/2支持普通闪烁，呼吸长亮，呼吸闪烁( 高精度闪烁 )
             DR3/4/5只支持闪烁和呼吸)
 输入参数  : dr_id:要查询的电流源编号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : Acore提供该函数实现
*****************************************************************************/
dr_mode_e bsp_hi6551_dr_get_mode(int dr_id)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int iret = BSP_PMU_OK;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return PMU_DRS_MODE_BUTTOM;

    hi6551_dr = &hi6551_dr_attr[dr_id];
    /*先判断是闪烁模式还是呼吸模式*/
    bsp_dr_reg_read(hi6551_dr->mode_sel_reg_addr, &regval);
    if(regval & ((u8)((u32)0x1 << hi6551_dr->mode_sel_bit_offset)))
    {
        /*判断是闪烁长亮还是闪烁闪亮*/
        bsp_dr_reg_read(hi6551_dr->fla_mode_reg_addr, &regval);
        if(regval & ((u8)((u32)0x1 << hi6551_dr->fla_mode_bit_offset)))
        {
            pmic_print_info("dr_id[%d] is in fla-flash mode\n", dr_id);
            return PMU_DRS_MODE_FLA_FLASH;/*闪烁闪动低精度模式*/
        }
        else
        {
            pmic_print_info("dr_id[%d] is in fla-light mode\n", dr_id);
            return PMU_DRS_MODE_FLA_LIGHT;/*闪烁长亮持续输出模式*/
        }
    }
    else
    {
        /*if(PMIC_HI6551_DR03 <= dr_id),DR3,4,5不区分呼吸长亮和呼吸闪烁*/
        if(DR_INVAILD_ADDR == hi6551_dr->bre_mode_reg_addr)
        {
           pmic_print_info("dr_id[%d] is in normal bre mode\n", dr_id);
           return PMU_DRS_MODE_BRE;/*呼吸模式*/
        }
        else
        {
            /*判断是呼吸长亮还是呼吸闪烁*/
            bsp_dr_reg_read(hi6551_dr->bre_mode_reg_addr, &regval);
            if(regval & ((u8)((u32)0x1 << hi6551_dr->bre_mode_bit_offset)))
            {
                pmic_print_info("dr_id[%d] is in breath-flash mode\n", dr_id);
                return PMU_DRS_MODE_BRE_FLASH;/*呼吸闪烁高精度模式*/
            }
            else
            {
                pmic_print_info("dr_id[%d] is in breath-light mode\n", dr_id);
                return PMU_DRS_MODE_BRE_LIGHT;/*呼吸长亮模式*/
            }
        }

    }
}
/*****************************************************************************
 函 数 名  : bsp_hi6551_dr_is_enabled
 功能描述  : 查询某路电流源是否开启。
 输入参数  : dr_id:电流源id号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 只在Acore提供该函数实现，
*****************************************************************************/
int bsp_hi6551_dr_is_enabled(int dr_id)
{
    u8 regval,regval2 = 0;
    int iret = BSP_PMU_OK;
    PMIC_HI6551_DRS_ATTR *hi6551_dr;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    if(PMU_DRS_MODE_FLA_FLASH == bsp_hi6551_dr_get_mode(dr_id) || PMU_DRS_MODE_FLA_LIGHT == bsp_hi6551_dr_get_mode(dr_id))/*FLA MODE*/
    {
        bsp_dr_reg_read(hi6551_dr->fla_onoff_reg_addr, &regval);
        if (PMU_DR03 <= dr_id)/*dr3/4/5闪烁模式下开启还需要判断bre开关寄存器*/
        {
            bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval2);
            return ((regval2 & (u8)((u32)0x1<<hi6551_dr->bre_onoff_bit_offset))&&(regval & (u8)((u32)0x1<<hi6551_dr->fla_onoff_bit_offset)));
        }
        else
            return (regval & (u8)((u32)0x1<<hi6551_dr->fla_onoff_bit_offset));
    }
    else/*BRE MODE*/
    {
        bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval);
        return (regval & (u8)((u32)0x1<<hi6551_dr->bre_onoff_bit_offset));
    }

}
int bsp_hi6551_dr_enable(int dr_id)
{
    u8 regval = 0;
    int iret = BSP_PMU_OK;
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    hi6551_drflags_t flags = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    if(PMU_DRS_MODE_FLA_FLASH == bsp_hi6551_dr_get_mode(dr_id) || PMU_DRS_MODE_FLA_LIGHT == bsp_hi6551_dr_get_mode(dr_id))/*FLA MODE*/
    {
        spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);
        bsp_dr_reg_read(hi6551_dr->fla_onoff_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_dr->fla_onoff_bit_offset);
        bsp_dr_reg_write(hi6551_dr->fla_onoff_reg_addr, regval);
        if (PMU_DR03 <= dr_id)/*dr3/4/5闪烁模式下开启还需要操作bre开关寄存器*/
        {
            bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval);
            regval |= (u8)((u32)0x1 << hi6551_dr->bre_onoff_bit_offset);
            bsp_dr_reg_write(hi6551_dr->bre_onoff_reg_addr, regval);
        }
        spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);
    }
    else/*BRE MODE*/
    {
        spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);
        bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_dr->bre_onoff_bit_offset);
        bsp_dr_reg_write(hi6551_dr->bre_onoff_reg_addr, regval);
        spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);
    }

    return BSP_PMU_OK;

}
int bsp_hi6551_dr_disable(int dr_id)
{
    u8 regval = 0;
    int iret = BSP_PMU_OK;
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    hi6551_drflags_t flags = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    if(PMU_DRS_MODE_FLA_FLASH == bsp_hi6551_dr_get_mode(dr_id) || PMU_DRS_MODE_FLA_LIGHT == bsp_hi6551_dr_get_mode(dr_id))/*FLA MODE*/
    {
        spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);
        bsp_dr_reg_read(hi6551_dr->fla_onoff_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->fla_onoff_bit_offset);
        bsp_dr_reg_write(hi6551_dr->fla_onoff_reg_addr, regval);
        if (PMU_DR03 <= dr_id)/*dr3/4/5闪烁模式下关闭还需要操作bre开关寄存器*/
        {
            bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval);
            regval &= ~(u8)((u32)0x1 << hi6551_dr->bre_onoff_bit_offset);
            bsp_dr_reg_write(hi6551_dr->bre_onoff_reg_addr, regval);
        }
        spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);
    }
    else/*BRE MODE*/
    {
        spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);
        bsp_dr_reg_read(hi6551_dr->bre_onoff_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_dr->bre_onoff_bit_offset);
        bsp_dr_reg_write(hi6551_dr->bre_onoff_reg_addr, regval);
        spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);
    }

    return BSP_PMU_OK;
}

int bsp_hi6551_dr_get_current(int dr_id)
{
    u8 regval = 0;
    int iret = BSP_PMU_OK;
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int cur = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    bsp_hi6551_reg_read(hi6551_dr->current_reg_addr, &regval);

    /* 获取电压档位值 */
    cur = (regval & hi6551_dr->current_bit_mask) >> hi6551_dr->current_bit_offset;

    cur = hi6551_dr->current_list[cur];

    pmic_print_info("dr_id %d's current is  %d uA!\n", dr_id,cur);

    return cur;

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_dr_set_current
 功能描述  : PMIC HI6551电源电压
 输入参数  : dr_id:要查询的电流源编号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : Acore、Ccore均提供该函数实现，但各核操作的电源不同(由regulator层保证)，
             故不需要添加核间锁，只使用核内锁
*****************************************************************************/
int bsp_hi6551_dr_set_current(int dr_id, int min_uA, int max_uA, unsigned *selector)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    hi6551_drflags_t flags = 0;
    int iret = BSP_PMU_OK;
    unsigned i = 0;
    int valid = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    /**/
    for(i = 0; i < DR_CUR_NUMS; i++ )
    {
        if ((min_uA <= hi6551_dr->current_list[i])&&(max_uA >= hi6551_dr->current_list[i]))
        {
            valid = 1;
            break;
        }
    }

    /* 如果设置值有效，设置寄存器 */
    if (valid)
    {
        *selector = i;
        /*核内互斥*/
        /*lint --e{746,718}*/
        spin_lock_irqsave(&g_hi6551_dr_st.lock,flags);
        bsp_dr_reg_write_mask(hi6551_dr->current_reg_addr, (u8)(i << hi6551_dr->current_bit_offset), hi6551_dr->current_bit_mask);
        spin_unlock_irqrestore(&g_hi6551_dr_st.lock,flags);

        pmic_print_info("current is set at %d uA!\n", hi6551_dr->current_list[i]);
        return BSP_PMU_OK;
    }
    else
    {
        pmic_print_error("dr_id %d cann't support current between %d and %d uA!\n",dr_id,min_uA,max_uA);

        return BSP_PMU_ERROR;
    }

}
int bsp_hi6551_dr_set_current_test(int dr_id, int min_uA, int max_uA)
{
    unsigned selector = 0;
    int iret = BSP_PMU_OK;

    iret = bsp_hi6551_dr_set_current(dr_id,min_uA,max_uA,&selector);
    if(iret)
        return iret;
    else
    {
        pmic_print_info("selector value is %d!\n",selector);
        return (int)selector;
    }
}

int bsp_hi6551_dr_list_current(int dr_id, unsigned selector)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int iret = BSP_PMU_OK;
    int cur = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;


    hi6551_dr = &hi6551_dr_attr[dr_id];

    /*检查档位有效性*/
    if (selector > NUM_OF_DR_CURRENT) {
        pmic_print_error("selector is %d,not exist,-EINVAL,please input new\n", selector);
        return BSP_PMU_ERROR;
    }

    cur =  hi6551_dr->current_list[selector];

    pmic_print_info("volt %d selector %d is %d uA!!\n",dr_id,selector,cur);

    return cur;

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_dr_init
 功能描述  : PMIC HI6551 dr模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 初始化app互斥信号量
*****************************************************************************/
int bsp_hi6551_dr_init(void)
{
    spin_lock_init(&g_hi6551_dr_st.lock);/*中断只在Acore实现，多core*/

    return 0;
}

/*----------------电流源其他设置相关接口,给led模块调用的接口-------------------*/
/*****************************************************************************
 函 数 名  : hi6551_dr_fla_time_set
 功能描述  : 设置dr的闪烁周期时间和点亮时间
 输入参数  : dr_fla_time_st:闪烁时间参数结构体;单位:us
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁
             闪烁周期:reg_value*31.25ms;点亮时间:reg_value*7.8125ms
*****************************************************************************/
int hi6551_dr_fla_time_set(DR_FLA_TIME *dr_fla_time_st)
{
    u8 feriod = 0;/*闪烁周期寄存器配置值*/
    u8 on_time = 0;/*点亮时间设置值*/
    DR_FLA_TIME dr_fla_time_current={0,0};
    unsigned int fla_on_time,fla_off_time,feriod_time;

    if(!dr_fla_time_st)
    {
        return BSP_DR_ERROR;
    }
    /*获取当前闪烁参数配置值*/
    hi6551_dr_fla_time_get(&dr_fla_time_current);

    if(DR_VALUE_INVALIED != dr_fla_time_st->fla_on_us)
        fla_on_time = dr_fla_time_st->fla_on_us;
    else
        fla_on_time = dr_fla_time_current.fla_on_us;

    if(DR_VALUE_INVALIED != dr_fla_time_st->fla_off_us)
        fla_off_time = dr_fla_time_st->fla_off_us;
    else
        fla_off_time = dr_fla_time_current.fla_off_us;

    feriod_time = fla_on_time + fla_off_time;

    /*feriod时间配置*/
    feriod=(u8)(feriod_time / FLA_PERIOD_STEP);
    bsp_dr_reg_write(HI6551_FLASH_PERIOD_OFFSET,feriod);

    /*闪烁点亮时间配置*/
    on_time=(u8)(fla_on_time / FLA_ON_STEP);
    bsp_dr_reg_write(HI6551_FLASH_ON_OFFSET,on_time);

    return BSP_DR_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_fla_time_get
 功能描述  : 获取dr的闪烁周期时间和点亮时间
 输入参数  : dr_fla_time_st:闪烁时间参数结构体;单位:us
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁
             闪烁周期:reg_value*31.25ms;点亮时间:reg_value*7.8125ms
*****************************************************************************/
int hi6551_dr_fla_time_get(DR_FLA_TIME* dr_fla_time_st)
{
    u8 feriod = 0;/*闪烁周期寄存器配置值*/
    u8 on_time = 0;/*点亮时间设置值*/
    unsigned feriod_time_us = 0;

    /*feriod时间获取*/
    bsp_dr_reg_read(HI6551_FLASH_PERIOD_OFFSET,&feriod);
    feriod_time_us = feriod * FLA_PERIOD_STEP;
    pmic_print_info("dr fla feriod is %d us!!\n",feriod_time_us);

    /*闪烁点亮时间获取*/
    bsp_dr_reg_read(HI6551_FLASH_ON_OFFSET,&on_time);
    dr_fla_time_st->fla_on_us = on_time * FLA_ON_STEP;
    pmic_print_info("dr fla on_time is %d us!!\n",dr_fla_time_st->fla_on_us);

    /*闪烁灯灭时间获取*/
    dr_fla_time_st->fla_off_us = feriod_time_us - dr_fla_time_st->fla_on_us;
    pmic_print_info("dr fla off_time is %d us!!\n",dr_fla_time_st->fla_off_us);

    return BSP_DR_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_fla_time_show
 功能描述  : 显示dr的闪烁周期时间和点亮时间
 输入参数  : dr_fla_time_st:闪烁时间参数结构体;单位:us
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
 备注说明  :
*****************************************************************************/
void hi6551_dr_fla_time_show(void)
{
    DR_FLA_TIME dr_fla_time_st={0,0};

    hi6551_dr_fla_time_get(&dr_fla_time_st);
    pmic_print_error("dr fla feriod is %d us!!\n",dr_fla_time_st.fla_on_us + dr_fla_time_st.fla_off_us);
    pmic_print_error("dr fla on_time is %d us!!\n",dr_fla_time_st.fla_on_us);
    pmic_print_error("dr fla off_time is %d us!!\n",dr_fla_time_st.fla_off_us);
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_set
 功能描述  : 设置dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)
 输入参数  : dr_id:要设置的电流源编号;dr_bre_time_st:呼吸时间参数结构体;
            没有找到设置的时间，就设置为长亮
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁
*****************************************************************************/
/*呼吸时间配置dr1/2*/
int hi6551_dr_bre_time_set(dr_id_e dr_id, DR_BRE_TIME *dr_bre_time_st)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    unsigned int bre_on_time,bre_off_time,bre_rise_time,bre_fall_time = 0;
    u8 bre_on,bre_off,bre_rise,bre_fall = 0;/*设置的寄存器对应值*/
    u8 i = 0;
    bool valid = 0;
    int iret = BSP_PMU_OK;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    if(!dr_bre_time_st)
    {
        return BSP_DR_ERROR;
    }

    hi6551_dr = &hi6551_dr_attr[dr_id];

    bre_on_time = dr_bre_time_st->bre_on_ms;
    bre_off_time = dr_bre_time_st->bre_off_ms;
    bre_rise_time = dr_bre_time_st->bre_rise_ms;
    bre_fall_time = dr_bre_time_st->bre_fall_ms;

    /*计算出设置的值*/
    if(bre_on_time != DR_VALUE_INVALIED)
    {
        for(i = 0; i < hi6551_dr->size_of_onoff_time_list; i++ )
        {
            /*if ((bre_on_time >= hi6551_dr->bre_onoff_time_list[i-1])&&(bre_on_time < hi6551_dr->bre_onoff_time_list[i]))*/
            if(bre_on_time == hi6551_dr->bre_onoff_time_list[i])
            {
                valid = 1;
                break;
            }
        }
        if(valid)
        {
            valid = 0;

            if(i == hi6551_dr->size_of_onoff_time_list - 1)
                bre_on = ALWAYS_ON_OFF;/*长亮*/
            else
                bre_on = i;

            bsp_dr_reg_write_mask(hi6551_dr->bre_onoff_time_reg_adrr, (u8)(bre_on << bre_on_offset), 0xF0);
        }
        else
        {
            pmic_print_info("can not support bre_on_time you want! \n");
            iret = BSP_DR_ERROR;
        }
    }

    if(bre_off_time != DR_VALUE_INVALIED)
    {
        for(i = 0; i < hi6551_dr->size_of_onoff_time_list; i++ )
        {
            /*if ((bre_off_time >= hi6551_dr->bre_onoff_time_list[i])&&(bre_off_time < hi6551_dr->bre_onoff_time_list[i+1]))*/
            if(bre_off_time == hi6551_dr->bre_onoff_time_list[i])
            {
                valid = 1;
                break;
            }
        }
        if(valid)
        {
            valid = 0;

            if(i == hi6551_dr->size_of_onoff_time_list - 1)
                bre_off = ALWAYS_ON_OFF;/*长暗*/
            else
                bre_off = i;

            bsp_dr_reg_write_mask(hi6551_dr->bre_onoff_time_reg_adrr, (u8)bre_off, 0x0F);
        }
        else
        {
            pmic_print_info("can not support bre_off_time you want! \n");
            iret = BSP_DR_ERROR;
        }
    }

    if(bre_rise_time != DR_VALUE_INVALIED)
    {
        for(i = 0; i < hi6551_dr->size_of_risefall_time_list; i++ )
        {
            /*if ((bre_rise_time >= hi6551_dr->bre_risefall_time_list[i])&&(bre_rise_time < hi6551_dr->bre_risefall_time_list[i+1]))*/
            if(bre_rise_time == hi6551_dr->bre_risefall_time_list[i])
            {
                valid = 1;
                break;
            }
        }
        if(valid)
        {
            valid = 0;
            bre_rise = i;
            bsp_dr_reg_write_mask(hi6551_dr->bre_risefall_time_reg_adrr, (u8)bre_rise , 0x0F);
        }
        else
        {
            pmic_print_info("can not support bre_rise_time you want! \n");
            iret = BSP_DR_ERROR;
        }
    }

    if(bre_fall_time != DR_VALUE_INVALIED)
    {
        for(i = 0; i < hi6551_dr->size_of_risefall_time_list; i++ )
        {
            /*if ((bre_fall_time >= hi6551_dr->bre_risefall_time_list[i])&&(bre_fall_time < hi6551_dr->bre_risefall_time_list[i+1]))*/
            if(bre_fall_time == hi6551_dr->bre_risefall_time_list[i])
            {
                valid = 1;
                break;
            }
        }
        if(valid)
        {
            bre_fall = i;
            bsp_dr_reg_write_mask(hi6551_dr->bre_risefall_time_reg_adrr, (u8)(bre_fall << bre_fall_offset) , 0xF0);
        }
        else
        {
            pmic_print_info("can not support bre_fall_time you want! \n");
            iret = BSP_DR_ERROR;
        }
    }

    return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_get
 功能描述  : 获取dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)
 输入参数  : dr_id:要获取呼吸时间的电流源编号;dr_bre_time_st:呼吸时间参数结构体;
 输出参数  : ALWAYS_ON_OFF表示长亮或长暗,BRE_TIME_NOT_SURE表示为其他设置值，
            与设置生效时亮度不变，故无法确认
 返 回 值  : 获取成功或者失败
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_dr_bre_time_get(dr_id_e dr_id, DR_BRE_TIME* dr_bre_time_st)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int bre_on,bre_off,bre_rise,bre_fall = 0;/*设置的寄存器对应值*/
    u8 onoff_time = 0;/*设置的时间*/
    u8 risefall_time = 0;/*设置的时间*/
    int iret = BSP_PMU_OK;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check(dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    /*onoff时间获取*/
    bsp_dr_reg_read(hi6551_dr->bre_onoff_time_reg_adrr,&onoff_time);
    bre_on = (onoff_time & 0xf0) >> bre_on_offset;
    bre_off = onoff_time & 0x0f;
    /*risefall时间获取*/
    bsp_dr_reg_read(hi6551_dr->bre_risefall_time_reg_adrr,&risefall_time);
    bre_fall = (risefall_time & 0xf0) >> bre_fall_offset;
    bre_rise = risefall_time & 0x0f;

    if (dr_id< PMU_DR03)
    {
        if(bre_on < hi6551_dr->size_of_onoff_time_list)
            dr_bre_time_st->bre_on_ms = hi6551_dr->bre_onoff_time_list[bre_on];
        else
            dr_bre_time_st->bre_on_ms = BRE_TIME_NOT_SURE;

        if(bre_off < hi6551_dr->size_of_onoff_time_list)
            dr_bre_time_st->bre_off_ms= hi6551_dr->bre_onoff_time_list[bre_off];
        else
            dr_bre_time_st->bre_off_ms = BRE_TIME_NOT_SURE;
    }
    else
    {
        if(ALWAYS_ON_OFF == bre_on)
            dr_bre_time_st->bre_on_ms = ALWAYS_ON_OFF_TIME_DR345;
        else if(bre_on < hi6551_dr->size_of_onoff_time_list - 1)
            dr_bre_time_st->bre_on_ms = hi6551_dr->bre_onoff_time_list[bre_on];
        else
            dr_bre_time_st->bre_on_ms = BRE_TIME_NOT_SURE;

        if(ALWAYS_ON_OFF == bre_off)
            dr_bre_time_st->bre_off_ms = ALWAYS_ON_OFF_TIME_DR345;
        else if(bre_off < hi6551_dr->size_of_onoff_time_list - 1)
            dr_bre_time_st->bre_off_ms= hi6551_dr->bre_onoff_time_list[bre_off];
        else
            dr_bre_time_st->bre_off_ms = BRE_TIME_NOT_SURE;
    }

    if(bre_rise < hi6551_dr->size_of_risefall_time_list)
        dr_bre_time_st->bre_rise_ms = hi6551_dr->bre_risefall_time_list[bre_rise];
    else
        dr_bre_time_st->bre_rise_ms = BRE_TIME_NOT_SURE;

    if(bre_fall < hi6551_dr->size_of_risefall_time_list)
        dr_bre_time_st->bre_fall_ms = hi6551_dr->bre_risefall_time_list[bre_fall];
    else
        dr_bre_time_st->bre_fall_ms = BRE_TIME_NOT_SURE;

    pmic_print_info("dr_id %d bre_on_time is %d ms!!\n",dr_id,dr_bre_time_st->bre_on_ms);
    pmic_print_info("dr_id %d bre_off_time is %d ms!!\n",dr_id,dr_bre_time_st->bre_off_ms);
    pmic_print_info("dr_id %d bre_rise_time is %d ms!!\n",dr_id,dr_bre_time_st->bre_rise_ms);
    pmic_print_info("dr_id %d bre_fall_time is %d ms!!\n",dr_id,dr_bre_time_st->bre_fall_ms);

    return BSP_DR_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_get
 功能描述  : 显示dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)
 输入参数  : dr_id:要设置的电流源编号;dr_bre_time_st:呼吸时间参数结构体;
            没有找到设置的时间，就设置为长亮
 输出参数  : 无
 返 回 值  : 获取成功或者失败
 调用函数  :
 备注说明  :
*****************************************************************************/
void hi6551_dr_bre_time_show(dr_id_e dr_id)
{
    int iret = BSP_PMU_OK;
    DR_BRE_TIME dr_bre_time_st ={0,0,0,0};

    iret = hi6551_dr_bre_time_get(dr_id, &dr_bre_time_st);

    if(BSP_DR_OK == iret)
    {
        pmic_print_error("dr_id %d bre_on_time is %d ms!!\n",dr_id,dr_bre_time_st.bre_on_ms);
        pmic_print_error("dr_id %d bre_off_time is %d ms!!\n",dr_id,dr_bre_time_st.bre_off_ms);
        pmic_print_error("dr_id %d bre_rise_time is %d ms!!\n",dr_id,dr_bre_time_st.bre_rise_ms);
        pmic_print_error("dr_id %d bre_fall_time is %d ms!!\n",dr_id,dr_bre_time_st.bre_fall_ms);
    }
    else
        pmic_print_error("dr_id %d bre_time get error!!\n",dr_id);
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_list
 功能描述  : 陈列dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)各档位的时间值
 输入参数  : dr_id:要设置的电流源编号;bre_time_enum:所要查询的时间类型;
             selector，时间档位
 输出参数  : 无
 返 回 值  : 档位时间
 调用函数  :
 备注说明  :
*****************************************************************************/
unsigned hi6551_dr_bre_time_list(dr_id_e dr_id, dr_bre_time_e bre_time_enum ,unsigned selector)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int iret = BSP_PMU_OK;
    unsigned dr_bre_time = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check((int)dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    switch(bre_time_enum)
    {
        case PMU_DRS_BRE_ON_MS:
        case PMU_DRS_BRE_OFF_MS:
            if(selector > hi6551_dr->size_of_onoff_time_list)
            {
                pmic_print_error("dr_id[%d] bre_onoff_time don't support selector %d!\n",dr_id,selector);
                pmic_print_error("max selector is %d!\n",hi6551_dr->size_of_onoff_time_list);
                return BSP_PMU_PARA_ERROR;
            }
            else
                dr_bre_time = hi6551_dr->bre_onoff_time_list[selector];
            break;
        case PMU_DRS_BRE_RISE_MS:
        case PMU_DRS_BRE_FALL_MS:
            if(selector > hi6551_dr->size_of_risefall_time_list)
            {
                pmic_print_error("dr_id[%d] bre_risefall_time don't support selector %d!\n",dr_id,selector);
                pmic_print_error("max selector is %d!\n",hi6551_dr->size_of_risefall_time_list);
                return BSP_PMU_PARA_ERROR;
            }
            else
                dr_bre_time = hi6551_dr->bre_risefall_time_list[selector];
            break;
        default:
            pmic_print_error("don't support this bre time!\n");
            return BSP_PMU_PARA_ERROR;
    }
    pmic_print_info("dr_id[%d],enum %d 's selector %d time is %dms!\n",dr_id,bre_time_enum,selector,dr_bre_time);
    return dr_bre_time;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_list_show
 功能描述  : 设置dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)
 输入参数  : dr_id:要设置的电流源编号;dr_bre_time_st:呼吸时间参数结构体;
            没有找到设置的时间，就设置为长亮
 输出参数  : 无
 返 回 值  : 档位时间
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁
*****************************************************************************/
void hi6551_dr_bre_time_list_show(void)
{
    dr_id_e dr_id;
    unsigned dr_bre_time,selector = 0;
    PMIC_HI6551_DRS_ATTR *hi6551_dr;

    for(dr_id = PMU_DR01;dr_id <= PMU_DR05; dr_id++)
    {
        hi6551_dr = &hi6551_dr_attr[dr_id];

        for(selector = 0;selector < hi6551_dr->size_of_onoff_time_list;selector++)
        {
            dr_bre_time = hi6551_dr_bre_time_list(dr_id,PMU_DRS_BRE_ON_MS,selector);
            pmic_print_error("dr_id[%d],BRE_ON_MS,selector %d , %dms!\n",dr_id,selector,dr_bre_time);
        }
        for(selector = 0;selector < hi6551_dr->size_of_onoff_time_list;selector++)
        {
            dr_bre_time = hi6551_dr_bre_time_list(dr_id,PMU_DRS_BRE_OFF_MS,selector);
            pmic_print_error("dr_id[%d],BRE_OFF_MS,selector %d , %dms!\n",dr_id,selector,dr_bre_time);
        }

        for(selector = 0;selector < hi6551_dr->size_of_risefall_time_list;selector++)
        {
            dr_bre_time = hi6551_dr_bre_time_list(dr_id,PMU_DRS_BRE_RISE_MS,selector);
            pmic_print_error("dr_id[%d],BRE_RISE_MS, selector %d , %dms!\n",dr_id,selector,dr_bre_time);
        }
        for(selector = 0;selector < hi6551_dr->size_of_risefall_time_list;selector++)
        {
            dr_bre_time = hi6551_dr_bre_time_list(dr_id,PMU_DRS_BRE_FALL_MS,selector);
            pmic_print_error("dr_id[%d],BRE_FALL_MS, selector %d , %dms!\n",dr_id,selector,dr_bre_time);
        }
    }
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_time_selector_get
 功能描述  : 获取dr 呼吸时间总共档位值
 输入参数  : dr_id:要设置的电流源编号;bre_time_enum:所要查询的时间类型;
 输出参数  : 无
 返 回 值  : 档位时间
 调用函数  :
 备注说明  :
*****************************************************************************/
unsigned hi6551_dr_bre_time_selectors_get(dr_id_e dr_id, dr_bre_time_e bre_time_enum)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int iret = BSP_PMU_OK;
    unsigned selectors = 0;

    /*参数有效性检查*/
    iret = hi6551_dr_para_check((int)dr_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;

    hi6551_dr = &hi6551_dr_attr[dr_id];

    switch(bre_time_enum)
    {
        case PMU_DRS_BRE_ON_MS:
        case PMU_DRS_BRE_OFF_MS:
            selectors = hi6551_dr->size_of_onoff_time_list;
            break;
        case PMU_DRS_BRE_RISE_MS:
        case PMU_DRS_BRE_FALL_MS:
            selectors = hi6551_dr->size_of_risefall_time_list;
            break;
        default:
            pmic_print_error("don't support this bre time!\n");
            return BSP_PMU_PARA_ERROR;
    }
    pmic_print_info("dr_id[%d],enum %d 's selector %d time is %d!\n",dr_id,bre_time_enum,selectors);
    return selectors;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_start_delay_set
 功能描述  : 设置dr的启动延时时间
 输入参数  : dr_id:要设置的电流源编号;delay_ms:启动延时的时间，单位:ms
             设置范围:[0 , 32768]ms
 输出参数  : 无
 返 回 值  : 设置成功或失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁，只有DR3/4/5有此功能
*****************************************************************************/
int hi6551_dr_start_delay_set(dr_id_e dr_id, unsigned delay_ms)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    /* 有效性检查*/
    if (dr_id< PMU_DR03)
    {
        pmic_print_error("This DR %d can not set start delay time!\n",dr_id);
        return BSP_PMU_PARA_ERROR;
    }
    if (DR345_START_DELAY_MAX < delay_ms)
    {
        pmic_print_error("This dr start delay time can be set form 0 to 32768ms!\n");
        return BSP_PMU_PARA_ERROR;
    }

    hi6551_dr = &hi6551_dr_attr[dr_id];
    bsp_dr_reg_write(hi6551_dr->start_del_reg_addr,(u8)(delay_ms/DR345_START_DELAY_STEP));

    return BSP_DR_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_start_delay_get
 功能描述  : 获取dr的启动延时时间
 输入参数  : dr_id:要设置的电流源编号;delay_ms:启动延时的时间，单位:ms
             设置范围:[0 , 32768]ms
 输出参数  : 无
 返 回 值  : 设置成功或失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁，只有DR3/4/5有此功能
*****************************************************************************/
int hi6551_dr_start_delay_get(dr_id_e dr_id, unsigned *delay_ms)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    u8 regval = 0;
    /* 有效性检查*/
    if (dr_id< PMU_DR03)
    {
        pmic_print_error("This DR %d can not support start delay time!\n",dr_id);
        return BSP_PMU_PARA_ERROR;
    }

    hi6551_dr = &hi6551_dr_attr[dr_id];
    bsp_dr_reg_read(hi6551_dr->start_del_reg_addr,&regval);
    *delay_ms = (unsigned)regval * DR345_START_DELAY_STEP;
    pmic_print_info("dr_id[%d] start delay time is %d!\n", dr_id, *delay_ms);

    return BSP_DR_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_start_delay_show
 功能描述  : 获取dr的启动延时时间
 输入参数  : dr_id:要设置的电流源编号;delay_ms:启动延时的时间，单位:ms
             设置范围:[0 , 32768]ms
 输出参数  : 无
 返 回 值  : 设置成功或失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁，只有DR3/4/5有此功能
*****************************************************************************/
unsigned hi6551_dr_start_delay_show(dr_id_e dr_id)
{
    int iret = BSP_DR_OK;
    unsigned delay_ms = 0;
    /* 有效性检查*/
    iret = hi6551_dr_start_delay_get(dr_id,&delay_ms);
    if(BSP_DR_OK == iret)
        pmic_print_error("dr_id[%d] start delay time is %d!\n", dr_id, delay_ms);
    else
        pmic_print_error("dr_id[%d] start delay time get error!\n", dr_id);

    return delay_ms;
}

subsys_initcall(bsp_hi6551_dr_init);
static void __exit bsp_hi6551_dr_exit(void)
{
}
module_exit(bsp_hi6551_dr_exit);