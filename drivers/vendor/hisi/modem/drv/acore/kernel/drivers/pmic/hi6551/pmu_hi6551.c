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
#include <asm/uaccess.h>
#endif

#include <product_config.h>
#include <osl_bio.h>
#include <osl_thread.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <soc_memmap.h>
#include <hi_smartstar.h>
#ifdef __KERNEL__
#include <drv_comm.h>
#endif
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_shared_ddr.h>


#include <bsp_version.h>
#include <bsp_nvim.h>
#include <bsp_ipc.h>
#include <pmu_balong.h>

#include "common_hi6551.h"


/*BUCK电压调节*/
typedef struct
{
    int volt_base;
    int step;

}PMIC_HI6551_BUCK_ADJ;
/*单位是0.1uv*/
static const PMIC_HI6551_BUCK_ADJ hi6551_buck_volt[PMIC_HI6551_BUCK_MAX + 1] =
{
    { 7020000, 79365},
    { 7020000, 79365},
    { 7000000, 80000},
    { 7500000, 250000},
    {12000000, 250000},
    { 7000000, 80000},
    {18000000, 250000},
};

#define HI6551_LVS_VOLTAGE 1800000  /*LVS电压为固定值*/
#define HI6551_BUCK2_5_VOLTAGE_1 1604000 /*BUCK2/5固定电压档1*/
#define HI6551_BUCK2_5_STEP_1 400000 /*BUCK2/5固定电压档1*/
#define HI6551_BUCK2_5_VOLTAGE_2 1804000 /*BUCK2/5固定电压档2*/
#define HI6551_BUCK2_5_STEP_2 200000 /*BUCK2/5固定电压档1*/
/*----------------------------全局变量\核内锁---------------------------------------------*/
typedef unsigned long hi6551_pmuflags_t;
spinlock_t  hi6551_pmu_lock;
#if defined(__KERNEL__) || defined(__VXWORKS__)
/*多核锁:核间锁+核内锁*/
#define pmu_hi6551_mutli_core_lock(flags)      \
    do {                            \
        /*lint --e{746,718}*/ \
        spin_lock_irqsave(&hi6551_pmu_lock,flags);     \
        (void)bsp_ipc_spin_lock(IPC_SEM_PMU_HI6551);   \
    } while (0)
#define pmu_hi6551_mutli_core_unlock(flags)        \
    do {                            \
        (void)bsp_ipc_spin_unlock(IPC_SEM_PMU_HI6551);   \
        spin_unlock_irqrestore(&hi6551_pmu_lock,flags);       \
    } while (0)
/*单核锁:核内锁*/
#define pmu_hi6551_single_core_lock(flags)      \
    do {                            \
        spin_lock_irqsave(&hi6551_pmu_lock,flags);      \
    } while (0)
#define pmu_hi6551_single_core_unlock(flags)        \
    do {                            \
        spin_unlock_irqrestore(&hi6551_pmu_lock,flags);      \
    } while (0)

/*mcore中不需要加锁，为空*/
#elif defined(__CMSIS_RTOS)
#define pmu_hi6551_mutli_core_lock(flags)      \
    do {                            \
    } while (0)
#define pmu_hi6551_mutli_core_unlock(flags)        \
    do {                            \
    } while (0)
/*单核锁:核内锁*/
#define pmu_hi6551_single_core_lock(flags)      \
    do {                            \
    } while (0)
#define pmu_hi6551_single_core_unlock(flags)        \
    do {                            \
    } while (0)
#endif

/*函数声明*/
int bsp_hi6551_volt_set_voltage_test(int volt_id, int min_uV, int max_uV);
void bsp_hi6551_volt_list_show(int volt_id);

/*para check func*/
static __inline__ int hi6551_volt_para_check(int volt_id)
{
    /* 有效性检查*/
    if ((PMIC_HI6551_VOLT_MAX < volt_id) || (PMIC_HI6551_VOLT_MIN > volt_id))
    {
        pmic_print_error("PMIC doesn't have volt %d!\n",volt_id);
        pmic_print_error("the volt you can use in this PMIC is from %d to %d,please check!\n",PMIC_HI6551_VOLT_MIN,PMIC_HI6551_VOLT_MAX);

        return BSP_PMU_ERROR;
    }
    else
        return BSP_PMU_OK;
}
static __inline__ int hi6551_volt_table_check(PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table)
{
    if((SHM_PMU_VOLTTABLE_MAGIC_START_DATA == hi6551_volt_table->magic_start) && \
        (SHM_PMU_VOLTTABLE_MAGIC_END_DATA == hi6551_volt_table->magic_end))
        return BSP_PMU_OK;
    else
        return BSP_PMU_ERROR;
}

static u32 smart_pmu_base_addr;
static u8 volt_ocp_on_flag;
/*----------------------------------基本寄存器操作接口---------------------------------------*/
/*****************************************************************************
* 函 数 名  : hi6551_reg_write
*
* 功能描述  : 对pmu芯片寄存器的写函数
*
* 输入参数  : u16 addr：待写入的寄存器地址
*             u8 u8Data：待写入的数据
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_hi6551_reg_write( u16 addr, u8 value)
{
    writel((u32) value,(u32)(smart_pmu_base_addr + (addr << 2)));
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
void  bsp_hi6551_reg_read( u16 addr, u8 *pValue)
{/*lint !e958*/
      *pValue = (u8)readl((u32)(smart_pmu_base_addr + (addr << 2)));
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
void bsp_hi6551_reg_write_mask(u16 addr, u8 value, u8 mask)
{
    u8 reg_tmp = 0;

    bsp_hi6551_reg_read(addr, &reg_tmp);
    reg_tmp &= ~mask;
    reg_tmp |= value;
    bsp_hi6551_reg_write(addr, reg_tmp);
}

int bsp_hi6551_reg_show(u16 addr)
{
    u8 Value = 0;

    bsp_hi6551_reg_read(addr, &Value);
    pmic_print_info("pmuRead addr 0x%x value is 0x%x!!\n",addr,Value);
    return Value;
}
/*-------------------------------电源开关接口------------------------------------------*/
/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_is_enabled
 功能描述  : 查询某路电压源是否开启。
 输入参数  : volt_id:电压源id号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 只在Acore、Ccore提供该函数实现，
*****************************************************************************/
int bsp_hi6551_volt_is_enabled(int volt_id)
{
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    bsp_hi6551_reg_read(hi6551_volt->is_enabled_reg_addr, &regval);

    return (regval & (u8)((u32)0x1 << hi6551_volt->is_enabled_bit_offset));
}
int bsp_hi6551_volt_enable(int volt_id)
{
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    hi6551_pmuflags_t flags = 0;
    u64 *pmu_ocp_flag = (u64 *)SHM_PMU_OCP_INFO_ADDR;/*过流，需要关闭的过流源*/
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    if((*pmu_ocp_flag & ((u64)0x1 << volt_id))&& !volt_ocp_on_flag)
    {
        pmic_print_error("volt_id %d current overflow,can not open!\n",volt_id);
        return BSP_PMU_ERROR;
    }

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];
     /*使能reg，都是写1有效，由pmussi总线互斥保证互斥，软件上不用再做互斥。boost、LDO9和LDO11除外*/
    if(PMIC_HI6551_LDO09 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        /*开启电源和转换电路，跟硬件确认可以同时开启*/
        regval |= (u8)((u32)0x3 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
        /*设置sim卡上拉电阻配置，放在开启电源后，否则有毛刺*/
        bsp_hi6551_reg_write(HI6551_SIM0_CFG_OFFSET, 0x10);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6551_LDO11 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        /*开启电源和转换电路，跟硬件确认可以同时开启*/
        regval |= (u8)((u32)0x3 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
        /*设置sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM1_CFG_OFFSET, 0x10);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6551_BOOST == volt_id)/*boost，写1开启，其他bit位不能修改*/
    {
        pmu_hi6551_single_core_lock(flags);/*只在acore中使用，使用核内锁即可*/

        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);

        pmu_hi6551_single_core_unlock(flags);
    }
    else
    {
        regval = (u8)((u32)0x1 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
    }

    return BSP_PMU_OK;

}
int bsp_hi6551_volt_disable(int volt_id)
{
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    hi6551_pmuflags_t flags = 0;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    /*BOOST和LDO9/11是写0关闭，其他都是写1关闭*/
    /*禁能reg，都是写1有效，由pmussi总线互斥保证互斥，软件上不用再做互斥。boost、LDO9和LDO11除外*/
    if(PMIC_HI6551_LDO09 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        /*sim卡上拉电阻配置,关闭快速上拉和sim卡侧上拉，bb侧上配置由sim卡模块去激活流程控制*/
        /*0x41,bit4为0，bit3为1*/
        bsp_hi6551_reg_write_mask(HI6551_SIM0_CFG_OFFSET,0x08,0x18);
        /*关闭电源和转换电路，跟硬件确认可以同时关闭*/
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6551_LDO11 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        /*sim卡上拉电阻配置,关闭快速上拉和sim卡侧上拉，bb侧上配置由sim卡模块去激活流程控制*/
        /*0x42,bit4为0，bit3为1*/
        bsp_hi6551_reg_write_mask(HI6551_SIM1_CFG_OFFSET,0x08,0x18);
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        /*关闭电源和转换电路，跟硬件确认可以同时关闭*/
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6551_BOOST == volt_id)/*boost，写0关闭，其他bit位不能修改*/
    {
        pmu_hi6551_single_core_lock(flags);/*只在acore中使用，使用核内锁即可*/

        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1<<hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);

        pmu_hi6551_single_core_unlock(flags);
    }
    else
    {
        regval = (u8)((u32)0x1 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);
    }

    return BSP_PMU_OK;
}
/*-------------------------------电源电压接口------------------------------------------*/
/*#if defined(__KERNEL__) || defined(__VXWORKS__)*/
int bsp_hi6551_volt_get_voltage(int volt_id)
{
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    int voltage = 0;
    u8 vltg = 0;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    /*LVS的电压默认是1.8v，不能改变*/
    if ((PMIC_HI6551_LVS_MIN <= volt_id ) && (PMIC_HI6551_LVS_MAX >= volt_id))
    {
        pmic_print_info("volt_id %d's voltage is  %d uV!\n", volt_id,1800000);
        return HI6551_LVS_VOLTAGE;
    }

    hi6551_volt = (PMIC_HI6551_VLTGS_ATTR *)&(hi6551_volt_table->hi6551_volt_attr[volt_id]);/*lint !e826*/

    bsp_hi6551_reg_read(hi6551_volt->voltage_reg_addr, &regval);

    /* 获取电压档位值 */
    vltg = (regval & hi6551_volt->voltage_bit_mask) >> hi6551_volt->voltage_bit_offset;

    /* buck的电压源单调，BOOST和LDO的为档位*/
    if(volt_id <= PMIC_HI6551_BUCK_MAX)
    {
        if((vltg & 0xc0) && ((volt_id == PMIC_HI6551_BUCK2) || (volt_id == PMIC_HI6551_BUCK5)))
        {
            if(vltg & (0x1 << 6))
                voltage += HI6551_BUCK2_5_STEP_1;
            if(vltg & (0x1 << 7))
                voltage += HI6551_BUCK2_5_STEP_2;
            vltg = vltg & (~0xc0);
        }
        voltage += (hi6551_buck_volt[volt_id].volt_base + vltg * hi6551_buck_volt[volt_id].step)/10;
    }
    else
    {
        voltage = hi6551_volt->voltage_list[vltg];
    }

    pmic_print_info("volt_id %d's voltage is  %d uV!\n", volt_id,voltage);

    return voltage;

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_get_mode
 功能描述  : PMIC HI6551电源电压
 输入参数  : volt_id:要查询的电源编号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : Acore、Ccore均提供该函数实现，但各核操作的电源不同(由regulator层保证)，
             故不需要添加核间锁，只使用核内锁
*****************************************************************************/
int bsp_hi6551_volt_set_voltage(int volt_id, int min_uV, int max_uV, unsigned *selector)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    hi6551_pmuflags_t flags = 0;
    int iret = BSP_PMU_OK;

    int volt_base = 0;
    int step = 0;

    int valid = 0;
    int voltage = 0; /*actual voltage be setted*/
    u8 voltage_nums = 0;
    u8 i = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    /*LVS的电压默认是1.8v，不能改变*/
    if ((PMIC_HI6551_LVS_MIN <= volt_id ) && (PMIC_HI6551_LVS_MAX >= volt_id))
    {
        pmic_print_error("volt_id %d's voltage can not be set!\n", volt_id);
        return BSP_PMU_ERROR;
    }

    hi6551_volt = (PMIC_HI6551_VLTGS_ATTR *)&(hi6551_volt_table->hi6551_volt_attr[volt_id]);/*lint !e826*/

    /* buck的电压源单调，BOOST和LDO的为档位*/
    if(PMIC_HI6551_BUCK_MAX >= volt_id)
    {
        volt_base = hi6551_buck_volt[volt_id].volt_base;
        step = hi6551_buck_volt[volt_id].step;
        voltage_nums = hi6551_volt->voltage_nums;

        if((PMIC_HI6551_BUCK2 == volt_id) || (PMIC_HI6551_BUCK5 == volt_id))
            voltage_nums -= 2;/*buck2/5 0~63档单调，64、65固定*/
        for(i = 0; i <= voltage_nums; i++ )
        {
            if ((min_uV * 10 <= (volt_base + i * step))&&(max_uV * 10 >= (volt_base + i * step)))
            {
                valid = 1;
                break;
            }
        }
        if(valid)
        {
            voltage = (volt_base + i * step)/10;
            *selector = i;
        }
        else
        {
            /*buck2和buck5低6bit单调，高2bit不单调，需要特殊处理*/
            if((PMIC_HI6551_BUCK2 == volt_id) || (PMIC_HI6551_BUCK5 == volt_id))
            {
                if((min_uV <= HI6551_BUCK2_5_VOLTAGE_1) && (max_uV >= HI6551_BUCK2_5_VOLTAGE_1))
                {
                    valid = 1;
                    voltage = HI6551_BUCK2_5_VOLTAGE_1;
                    i = 127;
                    *selector = 64;
                }
                else if((min_uV <= HI6551_BUCK2_5_VOLTAGE_2) && (max_uV >= HI6551_BUCK2_5_VOLTAGE_2))
                {
                    valid = 1;
                    voltage = HI6551_BUCK2_5_VOLTAGE_2;
                    i = 255;
                    *selector = 65;
                }
            }
        }
    }
    else
    {
        /* 参数检查2: 检查 vltg 是否为该相应的电压源的有效值，并转换电压值为寄存器设置值 */
        for (i = 0; i <= hi6551_volt->voltage_nums; i++)
        {
            if ((min_uV <= hi6551_volt->voltage_list[i])&&(max_uV >= hi6551_volt->voltage_list[i]))
            {
                valid = 1;
                break;
            }
        }
        voltage =hi6551_volt->voltage_list[i];
        *selector = i;
    }
    /* 如果设置值有效，设置寄存器 */
    if (valid)
    {
        /*核内互斥*/
        pmu_hi6551_single_core_lock(flags);
        bsp_hi6551_reg_write_mask(hi6551_volt->voltage_reg_addr, (u8)((u32)i << hi6551_volt->voltage_bit_offset), hi6551_volt->voltage_bit_mask);
        pmu_hi6551_single_core_unlock(flags);

        pmic_print_info("voltage is set at %d uV!\n", voltage);
        return BSP_PMU_OK;
    }
    else
    {
        pmic_print_error("volt_id %d cann't support voltage between %d and %d uV!\n",volt_id,min_uV,max_uV);

        return BSP_PMU_ERROR;
    }

}

int bsp_hi6551_volt_set_voltage_test(int volt_id, int min_uV, int max_uV)
{
    unsigned selector = 0;
    int iret = BSP_PMU_OK;

    iret = bsp_hi6551_volt_set_voltage(volt_id,min_uV,max_uV,&selector);
    if(iret)
        return iret;
    else
    {
        pmic_print_info("selector value is %d!!",selector);
        return (int)selector;
    }
}

int bsp_hi6551_volt_list_voltage(int volt_id, unsigned selector)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    int iret = BSP_PMU_OK;
    int voltage = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    /*检查档位有效性*/
    if (selector > hi6551_volt->voltage_nums) {
        pmic_print_error("selector is %d,not exist,-EINVAL,please input new\n", selector);
        return BSP_PMU_ERROR;
    }

    if(PMIC_HI6551_BUCK_MAX >= volt_id)/*buck为单调*/
    {
        /*BUCK2/5的64/65档为固定值，其他档为单调*/
        if((selector & 0x40) && ((volt_id == PMIC_HI6551_BUCK2) || (volt_id == PMIC_HI6551_BUCK5)))
        {
            if(64 == selector)
                voltage = HI6551_BUCK2_5_VOLTAGE_1;
            else if(65 == selector)
                voltage = HI6551_BUCK2_5_VOLTAGE_2;
        }
        else
            voltage = (hi6551_buck_volt[volt_id].volt_base + (int)selector * hi6551_buck_volt[volt_id].step)/10;
    }
    else if(PMIC_HI6551_LDO_MAX >= volt_id)/*BOOST,LDO为档位*/
    {
        voltage =  hi6551_volt->voltage_list[selector];
    }
    else
    {
        voltage = HI6551_LVS_VOLTAGE; /*lvs为固定值*/
    }
    pmic_print_info("volt %d selector %d is %d uV!!\n",volt_id,selector,voltage);

    return voltage;

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_list_show
 功能描述  : 显示当前所需路电源的所有档位值
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 调试使用
*****************************************************************************/
void bsp_hi6551_volt_list_show(int volt_id)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    int iret = BSP_PMU_OK;
    int voltage = 0;
    unsigned selector = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    for(selector = 0;selector <= hi6551_volt->voltage_nums;selector++)
    {
        voltage = bsp_hi6551_volt_list_voltage(volt_id,selector);
        pmic_print_error("volt %d selector %d is %d uV!!\n",volt_id,selector,voltage);
    }
}
/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_get_mode
 功能描述  : PMIC HI6551电源模式查询
            (支持normal和跟随PMU进入eco和强制进入eco三种模式,
             不支持模式的路返回PMU_MODE_NONE)
 输入参数  : volt_id:要查询的电源编号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : Acore、Ccore均提供该函数实现，只读，不需要添加锁
*****************************************************************************/
pmu_mode_e bsp_hi6551_volt_get_mode(int volt_id)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    int iret = BSP_PMU_OK;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return PMU_MODE_BUTTOM;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return PMU_MODE_BUTTOM;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    /*判断该路是否有eco模式*/
    if((hi6551_volt->eco_force_reg_addr == PMU_INVAILD_ADDR)&&(hi6551_volt->eco_follow_reg_addr == PMU_INVAILD_ADDR))
    {
        pmic_print_info("volt_id[%d] have no eco mode\n", volt_id);
        return PMU_MODE_NONE;/*没有eco模式*/
    }
    /*force 的优先级最高*/
    bsp_hi6551_reg_read(hi6551_volt->eco_force_reg_addr, &regval);
    if(regval & ((u8)((u32)0x1 << hi6551_volt->eco_force_bit_offset)))
    {
        pmic_print_info("volt_id[%d] is in force eco mode\n", volt_id);
        return PMU_MODE_ECO_FORCE;/*eco mode*/
    }

    bsp_hi6551_reg_read(hi6551_volt->eco_follow_reg_addr, &regval);
    if(regval & ((u8)((u32)0x1 << hi6551_volt->eco_follow_bit_offset)))
    {
        pmic_print_info("volt_id[%d] is in follow eco mode\n", volt_id);
        return PMU_MODE_ECO_FOLLOW;/*eco mode*/
    }
    else
    {
        pmic_print_info("volt_id[%d] is in normal mode\n", volt_id);
        return PMU_MODE_NORMAL;
    }

}
/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_set_mode
 功能描述  : PMIC HI6551电源模式设置
            (支持normal和跟随PMU进入eco和强制进入eco三种模式,只有特定路支持)
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 只在Acore提供该函数实现，不再添加核间锁，只使用核内锁
*****************************************************************************/
int bsp_hi6551_volt_set_mode(int volt_id, pmu_mode_e mode)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    hi6551_pmuflags_t flags = 0;
    int iret = BSP_PMU_OK;
    u8 regval = 0;

    /*参数有效性检查*/
    iret = hi6551_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
        return BSP_PMU_PARA_ERROR;
    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[volt_id];

    /*判断该路是否有eco模式*/
    if((hi6551_volt->eco_force_reg_addr == PMU_INVAILD_ADDR)&&(hi6551_volt->eco_follow_reg_addr == PMU_INVAILD_ADDR))
    {
        pmic_print_error("PMIC HI6551 this volt doesn't have eco mode!\n");
        return PMU_MODE_NONE;
    }

    pmu_hi6551_mutli_core_lock(flags);

    switch(mode){
    case PMU_MODE_NORMAL:
        pmic_print_info("volt_id[%d] will be set normal mode\n", volt_id);
        bsp_hi6551_reg_read(hi6551_volt->eco_force_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_volt->eco_force_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->eco_force_reg_addr, regval);

        bsp_hi6551_reg_read(hi6551_volt->eco_follow_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6551_volt->eco_follow_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->eco_follow_reg_addr, regval);

        break;

    case PMU_MODE_ECO_FOLLOW:
        pmic_print_info("volt_id[%d] will be set eco FOLLOW mode\n", volt_id);
        bsp_hi6551_reg_read(hi6551_volt->eco_follow_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_volt->eco_follow_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->eco_follow_reg_addr, regval);
        break;

    case PMU_MODE_ECO_FORCE:
        pmic_print_info("volt_id[%d] will be set eco FORCE mode\n", volt_id);
        bsp_hi6551_reg_read(hi6551_volt->eco_force_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_volt->eco_force_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->eco_force_reg_addr, regval);
        break;

    default:
        pmic_print_error("cann't support the mode\n");
        iret = BSP_PMU_ERROR;
        break;
    }

    pmu_hi6551_mutli_core_unlock(flags);

    return iret;

}
/*#endif*/

/*****************************************************************************
 函 数 名  : bsp_pmu_hi6551_init
 功能描述  : PMIC HI6551 PMU模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : fastboot中完成将电压控制表拷贝到ddr中，
             设置nv项中设定的默认电压及eco模式。
             其他核目前均为打桩。
*****************************************************************************/
int bsp_pmu_hi6551_init(void)
{
    u32 iret;

    smart_pmu_base_addr = HI_PMUSSI0_REGBASE_ADDR;

#ifdef __KERNEL__
    smart_pmu_base_addr = (u32)ioremap(smart_pmu_base_addr,HI_PMUSSI0_REG_SIZE);
    if(0 == smart_pmu_base_addr)
    {
        pmic_print_error("cannot map IO!\n");
        return BSP_PMU_ERROR;
    }
#endif

  /*read nv,get the exc protect setting*/
    iret = bsp_nvm_readpart(NV_ID_DRV_NV_PMU_EXC_PRO,0,&volt_ocp_on_flag ,1);
    if(NV_OK != iret)
    {
        pmic_print_error("pmu exc pro read nv error,not set,use the default config!\n");
    }

    spin_lock_init(&hi6551_pmu_lock);
    /*初始化common模块的lock*/
    bsp_hi6551_common_init();
    /*debug初始化*/
    bsp_hi6551_debug_init();

    return BSP_PMU_OK;
}
/*bb侧上拉电阻与关电彻底解绑*/
#if 0
void bsp_hi6551_sim_volt_disable(u32 sim_id)
{
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    hi6551_pmuflags_t flags = 0;
    u8 regval = 0;

    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return;

    /*BOOST和LDO9/11是写0关闭，其他都是写1关闭*/
    /*禁能reg，都是写1有效，由pmussi总线互斥保证互斥，软件上不用再做互斥。boost、LDO9和LDO11除外*/
    if(0 == sim_id)
    {
        hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[PMIC_HI6551_LDO09];
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        /*恢复sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM0_CFG_OFFSET, 0x08);
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        /*关闭电源和转换电路，跟硬件确认是否有顺序要求*/
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else if(1 == sim_id)
    {
        hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[PMIC_HI6551_LDO11];
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6551_mutli_core_lock(flags);

        /*恢复sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM1_CFG_OFFSET, 0x08);
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        /*关闭电源和转换电路，跟硬件确认是否有顺序要求*/
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);

        pmu_hi6551_mutli_core_unlock(flags);
    }
    else
    {
        pmic_print_error("don't support this sim!\n");
    }
    
    return;

}
#endif
int hi6551_sim_upres_disable(u32 sim_id)
{
    hi6551_pmuflags_t flags = 0;
    int iret = BSP_PMU_OK;
    u8 regval = 0;

    pmu_hi6551_mutli_core_lock(flags);
    if(0 == sim_id)
    {
        bsp_hi6551_reg_read(HI6551_SIM0_CFG_OFFSET, &regval);
        /*关闭sim0卡bb侧上拉电阻*/
        regval |= (0x1 << 2);
        bsp_hi6551_reg_write(HI6551_SIM0_CFG_OFFSET, regval);
    }
    else if(1 == sim_id)
    {
        bsp_hi6551_reg_read(HI6551_SIM1_CFG_OFFSET, &regval);
        /*关闭sim0卡bb侧上拉电阻*/
        regval |= (0x1 << 2);
        bsp_hi6551_reg_write(HI6551_SIM1_CFG_OFFSET, regval);
    }
    else
    {
        pmic_print_error("don't support this sim!\n");
        iret = BSP_PMU_ERROR;
    }
    pmu_hi6551_mutli_core_unlock(flags);
    
    return iret;
}


#ifdef __KERNEL__
/*subsys_initcall(bsp_pmu_hi6551_init);*/
arch_initcall(bsp_pmu_hi6551_init);
static void __exit bsp_pmu_hi6551_exit(void)
{
}
module_exit(bsp_pmu_hi6551_exit);

EXPORT_SYMBOL(bsp_hi6551_reg_write);
EXPORT_SYMBOL(bsp_hi6551_reg_read);
EXPORT_SYMBOL(bsp_hi6551_reg_write_mask);
#endif

