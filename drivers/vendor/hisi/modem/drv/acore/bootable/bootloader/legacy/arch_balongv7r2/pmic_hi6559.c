
#include <boot/boot.h>
#include <soc_memmap.h>
#include <osl_types.h>

#include <hi_bbstar.h>
#include <bsp_shared_ddr.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>

//#include <bsp_hardtimer.h>
#include <nv_boot.h>
#include <pmu.h>
#include "pmic_hi6559.h"

/*PMU初始化配置nv值，只有fastboot中需要*/
PMU_INIT_NV_STRU    pmu_init_config;
extern void hi6559_volttable_copytoddr(void);

/* BUCK电压调节 */
typedef struct
{
    s32 volt_base;
    s32 volt_step;

}PMIC_HI6559_BUCK_ADJ;

/* 单位是uv */
static const PMIC_HI6559_BUCK_ADJ hi6559_buck_volt[PMIC_HI6559_BUCK_MAX + 1] =
{
    { 900000, 50000}, /* BUCK0 */
    { 700000,  8000}, /* BUCK3 */
    {1200000, 25000}, /* BUCK4 */
    {      0,     0}, /* BUCK5的为档位，此处打桩 */
    {1800000, 25000}, /* BUCK6 */
};

#define HI6559_LVS_VOLTAGE 1800000  /* LVS电压为固定值1800000uV */

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
static __inline__ s32 hi6559_volt_para_check(s32 volt_id)
{
    /* 有效性检查*/
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
    if((SHM_PMU_VOLTTABLE_MAGIC_START_DATA == hi6559_volt_table->magic_start) && \
        (SHM_PMU_VOLTTABLE_MAGIC_END_DATA == hi6559_volt_table->magic_end))
    {
        return BSP_PMU_OK;
    }
    else
    {
        pmic_print_error("ERROR: volt_table error !\n");
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
    writel((u32) value,(u32)(HI_PMUSSI0_REGBASE_ADDR_VIRT + (addr << 2)));
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
      *pValue = (u8)readl((u32)(HI_PMUSSI0_REGBASE_ADDR_VIRT + (addr << 2)));
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
    
    /* 写1使能，其中LDO9和LDO11的使能在同一个寄存器上，要先读再写，其余可直接写 */
    if(PMIC_HI6559_LDO09 == volt_id)
    {
        bsp_hi6559_reg_read(hi6559_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x3 << hi6559_volt->enable_bit_offset); /* 使能LDO9的同时，使能SIM卡转换电路 */
        bsp_hi6559_reg_write(hi6559_volt->enable_reg_addr, regval);
        
        /* 设置sim卡上拉电阻配置，放在开启电源后，否则有毛刺 */
        bsp_hi6559_reg_write(HI6559_SIM_CFG_OFFSET, 0x10);
    }
    else if(PMIC_HI6559_LDO11 == volt_id)
    {
        bsp_hi6559_reg_read(hi6559_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6559_volt->enable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->enable_reg_addr, regval);
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
        bsp_hi6559_reg_write_mask(HI6559_SIM_CFG_OFFSET,0x08,0x18);
        bsp_hi6559_reg_read(hi6559_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x3 << hi6559_volt->disable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->disable_reg_addr, regval);
    }
    else if(PMIC_HI6559_LDO11 == volt_id)
    {
        bsp_hi6559_reg_read(hi6559_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1 << hi6559_volt->disable_bit_offset);
        bsp_hi6559_reg_write(hi6559_volt->disable_reg_addr, regval);
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
 备注说明  : Acore、Ccore均提供该函数实现，但各核操作的电源不同(由regulator层保证)，即操作的寄存器不同，
             故不需要添加核间锁，只使用核内锁
*****************************************************************************/
s32 bsp_hi6559_volt_set_voltage(s32 volt_id, s32 min_uV, s32 max_uV,unsigned *selector)
{
    PMIC_HI6559_VLTGS_TABLE *hi6559_volt_table = (PMIC_HI6559_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6559_VLTGS_ATTR *hi6559_volt;
    
    s32 iret = BSP_PMU_OK;
    s32 volt_base = 0;
    s32 volt_step = 0;
    s32 valid = 0;
    s32 voltage = 0; /* 实际设置的电压值 */
    
    u8 volt_nums = 0;
    u8 i = 0;

    /* 参数有效性检查 */
    iret = hi6559_volt_para_check(volt_id);
    if(BSP_PMU_OK != iret)
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
                valid = HI6559_VALUE_VALID;   /* [min, max]是一个合法区间 */
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
                valid = HI6559_VALUE_VALID;
                break;
            }
        }
        voltage = hi6559_volt->voltage_list[i];
        *selector = i;
    }

    /* 如果设置值有效，设置寄存器 */
    if (valid)
    {
        bsp_hi6559_reg_write_mask(hi6559_volt->voltage_reg_addr, (u8)((u32)i << hi6559_volt->voltage_bit_offset), hi6559_volt->voltage_bit_mask);
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

    /* 判断该路是否有eco模式 */
    if((hi6559_volt->eco_force_reg_addr == PMU_INVAILD_ADDR) && (hi6559_volt->eco_follow_reg_addr == PMU_INVAILD_ADDR))
    {
        pmic_print_info("volt_id[%d] have no eco mode\n", volt_id);
        return PMU_MODE_NONE;           /* 没有eco模式 */
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
    return PMU_MODE_NORMAL;             /* 该路支持ECO模式，但是不处于ECO模式 */
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
 函 数 名  : hi6559_set_by_nv
 功能描述  : 根据nv项设置进行默认设置
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu默认设置(主要指与产品形态相关的)
*****************************************************************************/
void hi6559_set_by_nv(void)
{
    u32 selector=0;
    u32 iret = BSP_PMU_OK;
    u32 iret_temp = BSP_PMU_OK;
    s32 i = 0;

    /* 初始化为0 */
    memset((void*)&pmu_init_config, 0, sizeof(PMU_INIT_NV_STRU));
    
    /* 读NV */
    iret = bsp_nvm_read(NV_ID_DRV_NV_PMU_INIT,(u8 *)&pmu_init_config,sizeof(PMU_INIT_NV_STRU));
    if(NV_OK != iret)
    {
        pmic_print_error("pmu init read nv error,not set,use the default config!\n");
        return;
    }

    for(i = PMIC_HI6559_VOLT_MIN;i <= PMIC_HI6559_VOLT_MAX;i++)
    {
        if(pmu_init_config.InitConfig[i].IsNeedSet)
        {
            if(pmu_init_config.InitConfig[i].IsVoltSet)
            {
                iret_temp = bsp_hi6559_volt_set_voltage(pmu_init_config.InitConfig[i].VoltId,(s32)pmu_init_config.InitConfig[i].Voltage,\
                                            (s32)pmu_init_config.InitConfig[i].Voltage, &selector);
                if(iret_temp)
                {
                    pmic_print_error("ERROR: set voltage failed, iret %d!\n", iret_temp);
                    iret |= iret_temp;
                }
            }
            
            if(pmu_init_config.InitConfig[i].IsOnSet)
            {
                iret_temp = bsp_hi6559_volt_enable(pmu_init_config.InitConfig[i].VoltId);
                if(iret_temp)
                {
                    pmic_print_error("ERROR: voltage enable failed, iret %d!\n", iret_temp);
                    iret |= iret_temp;
                }
            }
            
            if(pmu_init_config.InitConfig[i].IsOffSet)
            {
                iret_temp = bsp_hi6559_volt_disable(pmu_init_config.InitConfig[i].VoltId);
                if(iret_temp)
                {
                    pmic_print_error("ERROR: voltage disable failed, iret %d!\n", iret_temp);
                    iret |= iret_temp;
                }
            }
            
            if(pmu_init_config.InitConfig[i].IsEcoSet)
            {
                iret_temp = bsp_hi6559_volt_set_mode(pmu_init_config.InitConfig[i].VoltId,pmu_init_config.InitConfig[i].EcoMod);
                if(iret_temp)
                {
                    pmic_print_error("ERROR: set mode failed, iret %d!\n", iret_temp);
                    iret |= iret_temp;
                }
            }
        }
    }
}

void hi6559_optreg_config(void)
{
   /* LVS07过流模式改为模式2 */ 
   bsp_hi6559_reg_write_mask(HI6559_OCP_MOD_CTRL8_OFFSET, 0x4, 0x4);

   /* buck0默认不使能apt */
   bsp_hi6559_reg_write(HI6559_BUCK0_REG16_ADJ_OFFSET, 0x3A);
   bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x02);
   bsp_hi6559_reg_write(HI6559_BUCK0_REG12_ADJ_OFFSET, 0x0);
   bsp_hi6559_reg_write(HI6559_BUCK0_REG4_ADJ_OFFSET, 0xFE);
   bsp_hi6559_reg_write(HI6559_BUCK0_REG9_ADJ_OFFSET, 0x42);

   /* buck3 */
   bsp_hi6559_reg_write(HI6559_BUCK3_REG7_ADJ_OFFSET, 0x02);
   bsp_hi6559_reg_write(HI6559_BUCK3_REG1_ADJ_OFFSET, 0x0D);
   bsp_hi6559_reg_write(HI6559_BUCK3_REG9_ADJ_OFFSET, 0x03);

   /* buck4 */
   bsp_hi6559_reg_write(HI6559_BUCK4_REG1_ADJ_OFFSET, 0x5C);
   bsp_hi6559_reg_write(HI6559_BUCK4_REG2_ADJ_OFFSET, 0x06);

   /* buck5 */
   bsp_hi6559_reg_write(HI6559_BUCK5_REG5_ADJ_OFFSET, 0x03);
   bsp_hi6559_reg_write(HI6559_BUCK5_REG10_ADJ_OFFSET, 0x40);

   /* buck6 */
   bsp_hi6559_reg_write(HI6559_BUCK6_REG1_ADJ_OFFSET, 0x5C);
   bsp_hi6559_reg_write(HI6559_BUCK6_REG2_ADJ_OFFSET, 0x03);
   bsp_hi6559_reg_write(HI6559_BUCK6_REG9_ADJ_OFFSET, 0x03);
}

/*通过开关函数配置需要默认打开的电源*/
void hi6559_volt_config(void)
{
    unsigned selector = 0;

    /* 给hkadc供电 */
    /* 配置hi6559 LDO24 3.0v on */
    bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO24,3000000,3000000,&selector);
    bsp_hi6559_volt_enable(PMIC_HI6559_LDO24);
    udelay(240);
    
    /* 配置hi6559 LDO1 2.5v on， LDO1要在LDO24之后打开 */
    bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO01,2500000,2500000,&selector);
    bsp_hi6559_volt_enable(PMIC_HI6559_LDO01);
    udelay(240);

    /* 回片默认打开PA/RF电源，其中LDO12默认on，不需要软件再打开 */
    bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO13, 2850000, 2850000, &selector);
    bsp_hi6559_volt_enable(PMIC_HI6559_LDO13);
    udelay(240);
    bsp_hi6559_volt_enable(PMIC_HI6559_LVS07);    
    udelay(240);
    bsp_hi6559_volt_enable(PMIC_HI6559_BUCK5);    
    udelay(240);
    bsp_hi6559_volt_set_voltage(PMIC_HI6559_BUCK0, 3300000, 3300000, &selector);
    bsp_hi6559_volt_enable(PMIC_HI6559_BUCK0);

    /* 防止BUCK5不满足要求，改用LDO11，电压应设为2.2V */
    /* bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO11, 2200000, 2200000, &selector); */
    /* bsp_hi6559_volt_enable(PMIC_HI6559_LDO11);    */
}

extern void hi6559_volttable_copytoddr(void);
/*****************************************************************************
 函 数 名  : pmu_hi6559_init
 功能描述  : pmu hi6559模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot中pmu模块初始化时调用
*****************************************************************************/
void pmu_hi6559_init(void)
{
    u32 *volt_need_off = (u64 *)SHM_PMU_OCP_INFO_ADDR;/*过流，需要关闭的过流源*/
    u8 iregVal;

    pmic_print_info("\r\npmu_hi6559_init!");

	*volt_need_off = 0;

    /* 将电压控制表拷贝到DDR中 */
    hi6559_volttable_copytoddr();
    pmic_print_info("\r\nhi6559_volttable_copytoddr ok!");

    /* 配置硬件提供的电源优化配置参数 */
    hi6559_optreg_config();
    
    /* 配置软硬件接口文档中需要的默认电源配置 */
    hi6559_volt_config();
    
    /* set dac */
    bsp_hi6559_reg_read(HI6559_DAC_CTRL_OFFSET,&iregVal);
    iregVal |=  0x01;
    bsp_hi6559_reg_write(HI6559_DAC_CTRL_OFFSET, iregVal);
    
    /* 配置buck3/LDO8/LVS9/LDO12受Peri_EN控制上下电 */
    bsp_hi6559_reg_read(HI6559_ENB3_ECO_MARK_OFFSET,&iregVal);
    iregVal |= 0x3c;
    bsp_hi6559_reg_write(HI6559_ENB3_ECO_MARK_OFFSET, iregVal);

    pmic_print_info("pmu_hi6559_init ok!\n");
}

/*以下函数是提供给其他模块使用的接口，还需要adp(pmu.c)封装*/
/*****************************************************************************
 函 数 名  : hi6559_power_key_state_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出(1:插入；0:拔出)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6559_power_key_state_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_STATUS1_OFFSET, &regval);
    return  (regval & PMU_HI6559_POWER_KEY_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : hi6559_usb_state_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出(1:插入；0:拔出)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6559_usb_state_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_STATUS2_OFFSET, &regval);
    return  (regval & PMU_HI6559_USB_STATE_MASK) ? 1 : 0;
}

/*****************************************************************************
 函 数 名  : hi6559_hreset_state_get
 功能描述  : 判断pmu是否为热复位
 输入参数  : void
 输出参数  : 无
 返 回 值     : pmu是热启动或冷启动(1:热启动；0:冷启动)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6559_hreset_state_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_NP_IRQ1_RECORD_OFFSET, &regval);
    return  (regval & PMU_HI6559_HRESET_STATE_MASK) ? 1 : 0;
}

/*****************************************************************************
 函 数 名  : hi6559_hreset_state_get
 功能描述  : 清除pmu热复位标志
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
void hi6559_hreset_state_clear(void)
{
    bsp_hi6559_reg_write(HI6559_NP_IRQ1_RECORD_OFFSET, PMU_HI6559_HRESET_STATE_MASK);
}

/*****************************************************************************
 函 数 名  : hi6559_version_get
 功能描述  : 获取pmu的版本号
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
u8 hi6559_version_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_VERSION_OFFSET, &regval);
    return  regval;
}

/*****************************************************************************
 函 数 名  : hi6559_get_boot_state
 功能描述  : 系统启动时检查pmu寄存器状态，确认是否是由pmu引起的重启
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  : 系统可维可测
*****************************************************************************/
s32 hi6559_get_boot_state(void)
{
    s32 iret = PMU_STATE_OK;
    s32 flag = 0;
    u32 j = 0;
    u8 hi6559_om_boot[12] = {0};

    /* 保存非下电寄存器 */
    for(j = 0; j < 11; j++)
    {
        bsp_hi6559_reg_read((HI6559_NP_IRQ1_RECORD_OFFSET + j), &hi6559_om_boot[j]);
    }

    /* 欠压 */
    if((hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET ))||
       (hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET )))
    {
        iret |= PMU_STATE_UNDER_VOL;
        pmic_print_error("hi6559 under voltage last time,check hardware!\n");
    }

    /* 过压 */
    if(hi6559_om_boot[0] & (0x01 << HI6559_VSYS_OVER_6P0_OFFSET ))
    {
        iret |=  PMU_STATE_OVER_VOL;
        pmic_print_error("hi6559 over voltage last time,check hardware!\n");
    }
    
    /* 过温 */
    if(hi6559_om_boot[1] & (0x01 << HI6559_OTMP_150_OFFSET ) )
    {
        iret |=  PMU_STATE_OVER_TEMP;
        pmic_print_error("hi6559 over temperature last time,check hardware!\n");
    }
    
    /* 过流,包括buck短路，各路过流，过压、短路 */
    for(j = 0; j < 8; j++)
    {
        if(hi6559_om_boot[j + 3])
        {
            flag = 1;
        }
    }
    
    if(flag)
    {
        iret|=  PMU_STATE_OVER_CUR;
        pmic_print_error("hi6559 over current last time,check hardware!\n");
    }

    return iret;
}

/*****************************************************************************
 函 数 名  : hi6559_ldo14_switch
 功能描述  : 开关ldo14电源
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : lcd模块调用
*****************************************************************************/
s32 hi6559_ldo14_switch(power_switch_e sw)
{
    s32 iret = BSP_PMU_OK;

    switch(sw)
    {
        case POWER_OFF:
            iret = bsp_hi6559_volt_disable(PMIC_HI6559_LDO14);
            break;
        case POWER_ON:
            iret = bsp_hi6559_volt_enable(PMIC_HI6559_LDO14);
            break;
        default:
            break;
    }
    pmic_print_info("hi6559_ldo14_switch ok!\n");
    return iret;
}
/*****************************************************************************
 函 数 名  : hi6559_ldo14_volt_set
 功能描述  : 设置ldo14电源电压
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : lcd模块调用
*****************************************************************************/
s32 hi6559_ldo14_volt_set(s32 voltage)
{
    unsigned selector = 0;
    s32 iret = BSP_PMU_OK;

     iret = bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO14,voltage,voltage,&selector);

     pmic_print_info("hi6559_ldo14_volt_set ok!\n");
     return iret;
}
/*****************************************************************************
 函 数 名  : hi6559_ldo23_switch
 功能描述  : 开关ldo23电源
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : efuse模块调用
*****************************************************************************/
s32 hi6559_ldo23_switch(power_switch_e sw)
{
    s32 iret = BSP_PMU_OK;

    switch(sw)
    {
        case POWER_OFF:
            iret = bsp_hi6559_volt_disable(PMIC_HI6559_LDO23);
            break;
        case POWER_ON:
            iret = bsp_hi6559_volt_enable(PMIC_HI6559_LDO23);
            break;
        default:
            break;
    }
    pmic_print_info("hi6559_ldo14_switch ok!\n");
    return iret;
}
/*****************************************************************************
 函 数 名  : hi6559_ldo23_volt_set
 功能描述  : 设置ldo23电源电压
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : efuse模块调用
*****************************************************************************/
s32 hi6559_ldo23_volt_set(s32 voltage)
{
    unsigned selector = 0;
    s32 iret = BSP_PMU_OK;

     iret = bsp_hi6559_volt_set_voltage(PMIC_HI6559_LDO23,voltage,voltage,&selector);

     pmic_print_info("hi6559_ldo23_volt_set ok!\n");
     return iret;
}

