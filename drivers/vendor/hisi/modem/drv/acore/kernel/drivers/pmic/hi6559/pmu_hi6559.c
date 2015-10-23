/*
 * pmu_hi6559.c -- simple synchronous userspace interface to SSI devices
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
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <soc_memmap.h>
#include <hi_bbstar.h>
#include <bsp_shared_ddr.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#ifdef __KERNEL__
#include <drv_comm.h>
#endif
#ifdef __VXWORKS__
#include <drv_dpm.h>
#include <drv_pmu.h>
#include <bsp_regulator.h>
#endif

#include <bsp_version.h>
#include <bsp_nvim.h>
#include <bsp_ipc.h>

#ifdef __VXWORKS__
#include <bsp_regulator.h>
#include "pmu_hi6559.h"
#endif

#include "common_hi6559.h"
#include <pmu_balong.h>

#define HI6559_LVS_VOLTAGE 1800000  /* LVS电压为固定值1800000uV */

typedef unsigned long hi6559_pmuflags_t;
typedef struct /* BUCK电压调节 */
{
    s32 volt_base;
    s32 volt_step;

}PMIC_HI6559_BUCK_ADJ;

spinlock_t  hi6559_pmu_lock;
static u32 smart_pmu_base_addr;
static u8 volt_ocp_on_flag;
extern struct regulator *regulator_pmu[5];

static const PMIC_HI6559_BUCK_ADJ hi6559_buck_volt[PMIC_HI6559_BUCK_MAX + 1] =
{
    { 900000, 50000}, /* BUCK0 */
    { 700000,  8000}, /* BUCK3 */
    {1200000, 25000}, /* BUCK4 */
    {      0,     0}, /* BUCK5的为档位，此处打桩 */
    {1800000, 25000}, /* BUCK6 */
};


#if defined(__KERNEL__) || defined(__VXWORKS__)

/* 多核锁:核间锁+核内锁 */
#define pmu_hi6559_mutli_core_lock(flags)      \
    do {                            \
        /*lint --e{746,718}*/ \
        spin_lock_irqsave(&hi6559_pmu_lock,flags);     \
        (void)bsp_ipc_spin_lock(IPC_SEM_PMU_HI6559);   \
    } while (0)
    
#define pmu_hi6559_mutli_core_unlock(flags)        \
    do {                            \
        (void)bsp_ipc_spin_unlock(IPC_SEM_PMU_HI6559);   \
        spin_unlock_irqrestore(&hi6559_pmu_lock,flags);       \
    } while (0)
    
/* 单核锁:核内锁 */
#define pmu_hi6559_single_core_lock(flags)      \
    do {                            \
        spin_lock_irqsave(&hi6559_pmu_lock,flags);      \
    } while (0)
#define pmu_hi6559_single_core_unlock(flags)        \
    do {                            \
        spin_unlock_irqrestore(&hi6559_pmu_lock,flags);      \
    } while (0)

/* mcore中不需要加锁，为空 */
#elif defined(__CMSIS_RTOS)

#define pmu_hi6559_mutli_core_lock(flags) 
#define pmu_hi6559_mutli_core_unlock(flags) 
#define pmu_hi6559_single_core_lock(flags)  
#define pmu_hi6559_single_core_unlock(flags) 

#endif

/*****************************************************************************
* 函 数 名  : hi6559_volt_para_check
*
* 功能描述  : 检查电压源是否合法
*
* 输入参数  : @volt_id：待检查的电压源
*
* 输出参数  : 无
*
* 返 回 值  : BSP_PMU_OK:合法;   BSP_PMU_ERROR:不合法
*****************************************************************************/
s32 hi6559_volt_para_check(s32 volt_id)
{
    /* 有效性检查 */
    if ((PMIC_HI6559_VOLT_MAX < volt_id) || (PMIC_HI6559_VOLT_MIN > volt_id))
    {
        pmic_print_error("PMIC doesn't have volt %d!\n",volt_id);
        pmic_print_error("the volt you can use in this PMIC is from %d to %d,please check!\n", PMIC_HI6559_VOLT_MIN, PMIC_HI6559_VOLT_MAX);
        return BSP_PMU_ERROR;
    }
    else
    {
        return BSP_PMU_OK;
    }
}

/*****************************************************************************
* 函 数 名  : hi6559_volt_table_check
*
* 功能描述  : 检查电压源属性表是否存在
*
* 输入参数  : @hi6559_volt_table 电压源属性表地址
*
* 输出参数  : 无
*
* 返 回 值  : BSP_PMU_OK:存在;   BSP_PMU_ERROR:不存在
*****************************************************************************/
static __inline__ s32 hi6559_volt_table_check(PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table)
{
    if((SHM_PMU_VOLTTABLE_MAGIC_START_DATA == hi6559_volt_table->magic_start) && 
       (SHM_PMU_VOLTTABLE_MAGIC_END_DATA == hi6559_volt_table->magic_end))
    {
        return BSP_PMU_OK;
    }
    else
    {
        return BSP_PMU_ERROR;
    }
}

/*****************************************************************************
* 函 数 名  : hi6559_reg_write
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
void bsp_hi6559_reg_write( u16 addr, u8 value)
{
    writel((u32) value,(u32)(smart_pmu_base_addr + (addr << 2)));
}

/*****************************************************************************
* 函 数 名  : hi6559_reg_read
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
void  bsp_hi6559_reg_read( u16 addr, u8 *pValue)
{   
    /*lint !e958*/
    *pValue = (u8)readl((u32)(smart_pmu_base_addr + (addr << 2)));
}

/*****************************************************************************
* 函 数 名  : hi6559_reg_write_mask
*
* 功能描述  : 对pmu芯片寄存器的某些bit写值。如，向寄存器0x20的bit3写入1，则bsp_hi6559_reg_write_mask(0x20, 1, 8)
*
* 输入参数  : u16 addr：待写入的寄存器地址
*             u8 u8Data：读出的数据
*             u8 mask: 待写入bit的mask
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_reg_write_mask(u16 addr, u8 value, u8 mask)
{
    u8 reg_tmp = 0;

    bsp_hi6559_reg_read(addr, &reg_tmp);
    reg_tmp &= ~mask;
    reg_tmp |= value;
    bsp_hi6559_reg_write(addr, reg_tmp);
}

/*****************************************************************************
* 函 数 名  : hi6559_reg_write_mask
*
* 功能描述  : 对pmu芯片寄存器的某些bit写值。如，向寄存器0x20的bit3写入1，则bsp_hi6559_reg_write_mask(0x20, 1, 8)
*
* 输入参数  : u16 addr：待写入的寄存器地址
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
s32 bsp_hi6559_reg_show(u16 addr)
{
    u8 Value = 0;

    bsp_hi6559_reg_read(addr, &Value);
    pmic_print_info("pmuRead addr 0x%x value is 0x%x!!\n",addr,Value);
    return Value;
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_enable
 功能描述  : 使能电源volt_id
 输入参数  : @volt_id 电源id
 输出参数  : 无
 返 回 值  : BSP_PMU_OK: 成功； else:失败
*****************************************************************************/
s32 bsp_hi6559_volt_enable(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt = NULL;
    hi6559_pmuflags_t flags = 0;
    u32 *pmu_ocp_flag = (u32 *)SHM_PMU_OCP_INFO_ADDR;/*过流，需要关闭的过流源*/
    s32 iret = BSP_PMU_OK;
    u8 regval = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }
    
    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }

    /* 发生过流，则不打开 */
    if((*pmu_ocp_flag & ((u32)0x1 << volt_id))&& !volt_ocp_on_flag)
    {
        pmic_print_error("volt_id %d current overflow,can not open!\n",volt_id);
        return BSP_PMU_ERROR;
    }
    
    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];
    
    /* 写1使能，其中LDO9和LDO11的使能在同一个寄存器上，要先读再写，其余可直接写 */
    if(PMIC_HI6559_LDO09 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6559_mutli_core_lock(flags);
        bsp_hi6559_reg_read(hi6559_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x3 << hi6559_volt->enable_bit_offset); /* 使能LDO9的同时，使能SIM卡转换电路 */
        bsp_hi6559_reg_write(hi6559_volt->enable_reg_addr, regval);
        
        /*设置sim卡上拉电阻配置，放在开启电源后，否则有毛刺*/
        bsp_hi6559_reg_write(HI6559_SIM_CFG_OFFSET, 0x10);
        
        pmu_hi6559_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6559_LDO11 == volt_id)
    {
        /* 与LDO9操作在同一寄存器，故核间锁*/
        pmu_hi6559_mutli_core_lock(flags);
        bsp_hi6559_reg_read(hi6559_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6559_volt->enable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->enable_reg_addr, regval);
        pmu_hi6559_mutli_core_unlock(flags);
    }
    else
    {
        regval = (u8)((u32)0x1 << hi6559_volt->enable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->enable_reg_addr, regval);
    }    

    return BSP_PMU_OK;

}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_disable
 功能描述  : 禁止电源volt_id
 输入参数  : @volt_id 电源id
 输出参数  : 无
 返 回 值  : BSP_PMU_OK: 成功； else:失败
*****************************************************************************/
s32 bsp_hi6559_volt_disable(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt = NULL;
    hi6559_pmuflags_t flags = 0;
    s32 iret = BSP_PMU_OK;
    u8 regval = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }
    
    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }
    
    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    /* LDO9/11是写0关闭，且在同一个寄存器上，要先读再写，其余可直接写且都是写1关闭 */
    if(PMIC_HI6559_LDO09 == volt_id)
    {
        /*Acore中sim卡热插拔中断处理和Ccore中sim卡初始化时都需要操作，故核间锁+核内锁*/
        pmu_hi6559_mutli_core_lock(flags);
        bsp_hi6559_reg_write_mask(HI6559_SIM_CFG_OFFSET,0x08,0x18);
        bsp_hi6559_reg_read(hi6559_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x3 << hi6559_volt->disable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->disable_reg_addr, regval);
        pmu_hi6559_mutli_core_unlock(flags);
    }
    else if(PMIC_HI6559_LDO11 == volt_id)
    {
        /* 与LDO9操作在同一寄存器，故核间锁*/
        pmu_hi6559_mutli_core_lock(flags);
        bsp_hi6559_reg_read(hi6559_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6559_volt->disable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->disable_reg_addr, regval);
        pmu_hi6559_mutli_core_unlock(flags);
    }
    else
    {
        regval = (u8)((u32)0x1 << hi6559_volt->disable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->disable_reg_addr, regval);
    }

    return BSP_PMU_OK;

}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_is_enabled
 功能描述  : 查询某路电压源是否开启。
 输入参数  : volt_id:电压源id号
 输出参数  : 无
 返 回 值  : 0:未开启；else:开启
*****************************************************************************/
s32 bsp_hi6559_volt_is_enabled(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 iret = BSP_PMU_OK;
    u8 regval = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }
    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }
    
    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    bsp_hi6559_reg_read(hi6559_volt->is_enabled_reg_addr, &regval);

    /* 开关状态bit为1时，表示已使能 */
    return (regval & ((u32)0x1 << hi6559_volt->is_enabled_bit_offset));
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_get_voltage
 功能描述  : 获取电压源volt_id的电压值
 输入参数  : volt_id:电压源id号
 输出参数  : 无
 返 回 值  : 电压值
*****************************************************************************/
s32 bsp_hi6559_volt_get_voltage(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 voltage = 0;
    s32 iret  = BSP_PMU_OK;
    u8 regval = 0;
    u8 vltg = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }

    /*DDR中电压属性表安全性检查*/
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }

    /*LVS的电压默认是1.8v，不能改变*/
    if((PMIC_HI6559_LVS_MIN <= volt_id ) && (PMIC_HI6559_LVS_MAX >= volt_id))
    {
        pmic_print_info("volt_id %d's voltage is  %d uV!\n", volt_id, HI6559_LVS_VOLTAGE);
        return HI6559_LVS_VOLTAGE;
    }

    hi6559_volt = (PMIC_HI6559_VLTGS_ATTR *)&(hi6559_volt_table->hi6559_volt_attr[volt_id]);

    bsp_hi6559_reg_read(hi6559_volt->voltage_reg_addr, &regval);

    /* 获取电压档位值 */
    vltg = (regval & hi6559_volt->voltage_bit_mask) >> hi6559_volt->voltage_bit_offset;

    /* LDO和buck5的电压值为档位，其余buck单调 */
    if((PMIC_HI6559_BUCK_MAX >= volt_id) && (PMIC_HI6559_BUCK5 != volt_id))
    {
        voltage = (hi6559_buck_volt[volt_id].volt_base + vltg * hi6559_buck_volt[volt_id].volt_step);
    }
    else
    {
        voltage = hi6559_volt->voltage_list[vltg];
    }

    pmic_print_info("volt_id %d's voltage is  %d uV!\n", volt_id, voltage);

    return voltage;

}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_set_voltage
 功能描述  : 设置电压源volt_id的电压值，配置为[min_uV, max_uV]区间的值即可，如果区间内没有合法值，失败返回
 输入参数  : volt_id: 要设置的电源编号
             min_uV: 最小合法电压值
             max_uV: 最大合法电压值
 输出参数  : @selector: 实际设置电压值的档位
 返 回 值  : BSP_PMU_OK: 成功； else:失败
*****************************************************************************/
s32 bsp_hi6559_volt_set_voltage(s32 volt_id, s32 min_uV, s32 max_uV,unsigned *selector)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    hi6559_pmuflags_t flags = 0;
    s32 iret = BSP_PMU_OK;
    s32 volt_base = 0;
    s32 volt_step = 0;
    s32 valid = 0;
    s32 voltage = 0; /* 实际设置的电压值 */
    
    u8 volt_nums = 0;
    u8 i = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if((BSP_PMU_OK != iret) || (!selector))
    {
        return BSP_PMU_PARA_ERROR;
    }

    /* DDR中电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }

    /* LVS的电压默认是1.8v，不能改变 */
    if((PMIC_HI6559_LVS_MIN <= volt_id ) && (PMIC_HI6559_LVS_MAX >= volt_id))
    {
        pmic_print_error("volt_id %d's voltage can not be set!\n", volt_id);
        return BSP_PMU_ERROR;
    }

    hi6559_volt = (PMIC_HI6559_VLTGS_ATTR *)&(hi6559_volt_table->hi6559_volt_attr[volt_id]);

    /* buck电压值单调(除了buck5) */
    if((PMIC_HI6559_BUCK_MAX >= volt_id) && (PMIC_HI6559_BUCK5 != volt_id))
    {
        volt_base = hi6559_buck_volt[volt_id].volt_base;
        volt_step = hi6559_buck_volt[volt_id].volt_step;
        volt_nums = hi6559_volt->voltage_nums;

        for(i = 0; i <= volt_nums; i++ )
        {
            if((min_uV <= (volt_base + i * volt_step)) && (max_uV >= (volt_base + i * volt_step)))
            {
                valid = true;   /* [min, max]是一个合法区间 */
                break;
            }
        }

        /* [min, max]区间合法，寻找区间内的电压值 */
        if(valid)
        {
            voltage = (volt_base + i * volt_step);
            *selector = i;
        }
    }
    else /* LDO和buck5的电压值为档位 */
    {
        /* 检查 vltg 是否为该相应的电压源的有效值，并转换电压值为寄存器设置值 */
        for (i = 0; i <= hi6559_volt->voltage_nums; i++)
        {
            if ((min_uV <= hi6559_volt->voltage_list[i]) && (max_uV >= hi6559_volt->voltage_list[i]))
            {
                valid = true;
                break;
            }
        }
        voltage = hi6559_volt->voltage_list[i];
        *selector = i;
    }

    /* 如果设置值有效，设置寄存器 */
    if (valid)
    {
        pmu_hi6559_single_core_lock(flags);
        bsp_hi6559_reg_write_mask(hi6559_volt->voltage_reg_addr, (u8)((u32)i << hi6559_volt->voltage_bit_offset), hi6559_volt->voltage_bit_mask);
        pmu_hi6559_single_core_unlock(flags);
        pmic_print_info("voltage is set at %d uV!\n", voltage);
        return BSP_PMU_OK;
    }
    else
    {
        pmic_print_error("volt_id %d cann't support voltage between %d and %d uV!\n",volt_id,min_uV,max_uV);
        return BSP_PMU_ERROR;
    }

}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_list_voltage
 功能描述  : 获取电压源volt_id档位为selector的电压值
 输入参数  : volt_id: 要设置的电源编号
             selector: 电压档位
 返 回 值  : BSP_PMU_OK: 成功； else:失败
*****************************************************************************/
s32 bsp_hi6559_volt_list_voltage(s32 volt_id, unsigned selector)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 iret = BSP_PMU_OK;
    s32 voltage = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }
    
    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return  BSP_PMU_VOLTTABLE_ERROR;
    }

    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    /* 检查档位有效性 */
    if (selector > hi6559_volt->voltage_nums) 
    {
        pmic_print_error("selector is %d,not exist,-EINVAL,please input new\n", selector);
        return BSP_PMU_ERROR;
    }

    if((PMIC_HI6559_BUCK_MAX >= volt_id) && (PMIC_HI6559_BUCK5 != volt_id))/* buck为单调(除了buck5) */
    {
        voltage = (hi6559_buck_volt[volt_id].volt_base + (s32)selector * hi6559_buck_volt[volt_id].volt_step);
    }
    else if(PMIC_HI6559_LDO_MAX >= volt_id)/* LDO和buck5的电压值为档位 */
    {
        voltage =  hi6559_volt->voltage_list[selector];
    }
    else
    {
        voltage = HI6559_LVS_VOLTAGE; /* lvs为固定值 */
    }
    
    pmic_print_info("volt %d selector %d is %d uV!!\n",volt_id,selector,voltage);

    return voltage;

}
/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_list_show
 功能描述  : 显示当前所需路电源的所有档位值
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_volt_list_show(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 iret = BSP_PMU_OK;
    s32 voltage = 0;
    unsigned selector = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return;
    }
    
    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return;
    }

    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    for(selector = 0;selector <= hi6559_volt->voltage_nums;selector++)
    {
        voltage = bsp_hi6559_volt_list_voltage(volt_id,selector);
        pmic_print_error("volt %d selector %d is %d uV!!\n",volt_id,selector,voltage);
    }

    return;
}
/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_get_mode
 功能描述  : PMIC HI6559电源模式查询
            (支持normal和跟随PMU进入eco和强制进入eco三种模式,
             不支持模式的路返回PMU_MODE_NONE)
 输入参数  : volt_id:要查询的电源编号
 输出参数  : 无
 返 回 值  : 电压源具体的模式
 备注说明  : Acore、Ccore均提供该函数实现，只读，不需要添加锁
*****************************************************************************/
pmu_mode_e bsp_hi6559_volt_get_mode(s32 volt_id)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 iret = BSP_PMU_OK;
    u8 regval = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return PMU_MODE_BUTTOM;
    }

    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return PMU_MODE_BUTTOM;
    }

    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    /* 判断该路是否有eco模式，如果没有，返回NONE */
    if((hi6559_volt->eco_force_reg_addr == PMU_INVAILD_ADDR) && (hi6559_volt->eco_follow_reg_addr == PMU_INVAILD_ADDR))
    {
        pmic_print_info("volt_id[%d] have no eco mode\n", volt_id);
        return PMU_MODE_NONE;          
    }
    
    /* force 的优先级最高 */
    if(hi6559_volt->eco_force_reg_addr != PMU_INVAILD_ADDR)
    {
        bsp_hi6559_reg_read(hi6559_volt->eco_force_reg_addr, &regval);
        if(regval & ((u8)((u32)0x1 << hi6559_volt->eco_force_bit_offset)))
        {
            pmic_print_info("volt_id[%d] is in force eco mode\n", volt_id);
            return PMU_MODE_ECO_FORCE;  /* eco mode */
        }
    }

    if(hi6559_volt->eco_follow_reg_addr != PMU_INVAILD_ADDR)
    {
        bsp_hi6559_reg_read(hi6559_volt->eco_follow_reg_addr, &regval);
        if(regval & ((u8)((u32)0x1 << hi6559_volt->eco_follow_bit_offset)))
        {
            pmic_print_info("volt_id[%d] is in follow eco mode\n", volt_id);
            return PMU_MODE_ECO_FOLLOW; /* eco mode */
        }
    }

    pmic_print_info("volt_id[%d] is in normal mode\n", volt_id);

    /* 该路支持ECO模式，但是不处于ECO模式，返回NORMAL */
    return PMU_MODE_NORMAL;             
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_set_mode
 功能描述  : PMIC HI6559电源模式设置(支持normal/follow_eco/force_eco三种模式,只有特定路支持)
 输入参数  : @volt_id: 电压源id
             @mode:模式
 输出参数  : 无
 返 回 值  : BSP_PMU_OK: 成功； else:失败
 备注说明  : 只在Acore提供该函数实现，不再添加核间锁，只使用核内锁
*****************************************************************************/
s32 bsp_hi6559_volt_set_mode(s32 volt_id, pmu_mode_e mode)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    s32 iret = BSP_PMU_OK;
    u8 regval = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_PARA_ERROR;
    }

    /* 电压属性表安全性检查 */
    iret = hi6559_volt_table_check(hi6559_volt_table);
    if(BSP_PMU_OK != iret)
    {
        return BSP_PMU_VOLTTABLE_ERROR;
    }
    
    hi6559_volt = &hi6559_volt_table->hi6559_volt_attr[volt_id];

    /*判断该路是否有eco模式*/
    if(((PMU_INVAILD_ADDR == hi6559_volt->eco_force_reg_addr) && (PMU_MODE_ECO_FORCE== mode)) ||
       ((PMU_INVAILD_ADDR == hi6559_volt->eco_follow_reg_addr) && (PMU_MODE_ECO_FOLLOW== mode)))
    {
        pmic_print_error("PMIC HI6559 this volt doesn't have eco mode!\n");
        return BSP_PMU_ERROR;
    }

    /* 根据不同的模式进行配置 */
    switch(mode)
    {
        case PMU_MODE_NORMAL:
            pmic_print_info("volt_id[%d] will be set normal mode\n", volt_id);

            if(PMU_INVAILD_ADDR != hi6559_volt->eco_force_reg_addr)
            {
                bsp_hi6559_reg_read(hi6559_volt->eco_force_reg_addr, &regval);
                regval &= ~(u8)((u32)0x1 << hi6559_volt->eco_force_bit_offset);
                bsp_hi6559_reg_write(hi6559_volt->eco_force_reg_addr, regval);
            }

            if(PMU_INVAILD_ADDR != hi6559_volt->eco_follow_reg_addr)
            {
                bsp_hi6559_reg_read(hi6559_volt->eco_follow_reg_addr, &regval);
                regval &= ~(u8)((u32)0x1 << hi6559_volt->eco_follow_bit_offset);
                bsp_hi6559_reg_write(hi6559_volt->eco_follow_reg_addr, regval);
            }
            break;

        case PMU_MODE_ECO_FOLLOW:
            pmic_print_info("volt_id[%d] will be set eco FOLLOW mode\n", volt_id);
            
            if(PMU_INVAILD_ADDR != hi6559_volt->eco_follow_reg_addr)
            {
                bsp_hi6559_reg_read(hi6559_volt->eco_follow_reg_addr, &regval);
                regval |= (u8)((u32)0x1 << hi6559_volt->eco_follow_bit_offset);
                bsp_hi6559_reg_write(hi6559_volt->eco_follow_reg_addr, regval);
            }
            break;

        case PMU_MODE_ECO_FORCE:
            pmic_print_info("volt_id[%d] will be set eco FORCE mode\n", volt_id);
            
            if(PMU_INVAILD_ADDR != hi6559_volt->eco_force_reg_addr)
            {
                bsp_hi6559_reg_read(hi6559_volt->eco_force_reg_addr, &regval);
                regval |= (u8)((u32)0x1 << hi6559_volt->eco_force_bit_offset);
                bsp_hi6559_reg_write(hi6559_volt->eco_force_reg_addr, regval);
            }            
            break;

        default:
            pmic_print_error("ERROR: unkonwn mode %d\n", mode);
            return BSP_PMU_ERROR;
    }

    return BSP_PMU_OK;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_hi6559_init
 功能描述  : PMIC HI6559 PMU模块初始化
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
s32 bsp_pmu_hi6559_init(void)
{
    s32 iret = BSP_PMU_OK;

    smart_pmu_base_addr = HI_PMUSSI0_REGBASE_ADDR;

#ifdef __KERNEL__
    smart_pmu_base_addr = (u32)ioremap(smart_pmu_base_addr,HI_PMUSSI0_REG_SIZE);
    if(0 == smart_pmu_base_addr)
    {
        pmic_print_error("cannot map IO!\n");
        return BSP_PMU_ERROR;
    }
#endif

    /* 读NV，获取异常保护设置 */
    iret = (s32)bsp_nvm_readpart(NV_ID_DRV_NV_PMU_EXC_PRO, 0, &volt_ocp_on_flag, sizeof(u8));
    if(NV_OK != iret)
    {
        pmic_print_error("ERROR:pmu exc pro read nv error,not set,use the default config!\n");
    }

    spin_lock_init(&hi6559_pmu_lock);
    
    /* 初始化common模块的lock */
    bsp_hi6559_common_init();
    
    /* debug初始化 */
    bsp_hi6559_debug_init();

    return iret;
}

s32 hi6559_sim_upres_disable(u32 sim_id)
{
    return BSP_PMU_OK;
}

#ifdef __VXWORKS__
/*****************************************************************************
 函数	: bsp_pmu_hi6559_apt_enable
 功能	: 通信模块使能APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_hi6559_apt_enable(void)
{
    hi6559_pmuflags_t flags = 0;
    
    /* buck0 apt功能控制寄存器0x58功能单一，可直接写 */
    pmu_hi6559_single_core_lock(flags);

    /* 寄存器优化配置，由芯片和硬件给出 */
    bsp_hi6559_reg_write(HI6559_BUCK0_REG12_ADJ_OFFSET, 0x10);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG4_ADJ_OFFSET, 0xBE);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x02);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG16_ADJ_OFFSET, 0x6A);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x42);

    /* apt enable */
    bsp_hi6559_reg_write(HI6559_BUCK0_REG5_ADJ_OFFSET, 0x1);
    pmu_hi6559_single_core_unlock(flags);
    return BSP_PMU_OK;
}

/*****************************************************************************
 函数	: bsp_pmu_hi6559_apt_enable
 功能	: 通信模块使能APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_hi6559_apt_disable(void)
{
    hi6559_pmuflags_t flags = 0;

    /* buck0 apt功能控制寄存器0x58功能单一，可直接写 */
    pmu_hi6559_single_core_lock(flags);

    /* 寄存器优化配置，由芯片和硬件给出 */
    bsp_hi6559_reg_write(HI6559_BUCK0_REG16_ADJ_OFFSET, 0x3A);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x02);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG12_ADJ_OFFSET, 0x0);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG4_ADJ_OFFSET, 0xFE);
    bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x42);

    /* apt disable */
    bsp_hi6559_reg_write(HI6559_BUCK0_REG5_ADJ_OFFSET, 0x0);
    pmu_hi6559_single_core_unlock(flags);
    return BSP_PMU_OK;
}

/*****************************************************************************
 函数	: drv_pmu_hi6559_apt_status_get
 功能	: 通信模块获取当前APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_hi6559_apt_status_get(void)
{
    hi6559_pmuflags_t flags = 0;
    u8 reg_val = 0;
    
    pmu_hi6559_single_core_lock(flags);
    bsp_hi6559_reg_read(HI6559_BUCK0_REG5_ADJ_OFFSET, &reg_val);
    pmu_hi6559_single_core_unlock(flags);

    pmic_print_info("reg_val %d\n", reg_val);
    
    return reg_val;
}

#endif

#ifdef __KERNEL__
arch_initcall(bsp_pmu_hi6559_init);
static void __exit bsp_pmu_hi6559_exit(void)
{
    return;
}
module_exit(bsp_pmu_hi6559_exit);

EXPORT_SYMBOL(bsp_hi6559_reg_write);
EXPORT_SYMBOL(bsp_hi6559_reg_read);
EXPORT_SYMBOL(bsp_hi6559_reg_write_mask);
#endif


