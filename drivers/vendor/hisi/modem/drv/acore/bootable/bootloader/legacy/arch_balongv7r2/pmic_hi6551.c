
#include <boot/boot.h>
#include <soc_memmap.h>
#include <hi_smartstar.h>
#include <bsp_shared_ddr.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <nv_boot.h>
#include <pmu.h>
#include "pmic_hi6551.h"

/*PMU初始化配置nv值，只有fastboot中需要*/
/*PMU_INIT_NV_STRU    pmu_init_config={{0,0,0,0,0,0,0,0,0}};*//*赋值有warning，init中使用memset*/
PMU_INIT_NV_STRU    pmu_init_config;
extern void hi6551_volttable_copytoddr(void);

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
    writel((u32) value,(u32)(HI_PMUSSI0_REGBASE_ADDR_VIRT + (addr << 2)));
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
{
      *pValue = (u8)readl((u32)(HI_PMUSSI0_REGBASE_ADDR_VIRT + (addr << 2)));
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

/*****************************************************************************
 函 数 名  : bsp_hi6551_volt_enable
 功能描述  : pmu hi6551模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot中pmu模块初始化时调用
*****************************************************************************/
int bsp_hi6551_volt_enable(int volt_id)
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
     /*使能reg，都是写1有效，由pmussi总线互斥保证互斥，软件上不用再做互斥。boost、LDO9和LDO11除外*/
    if(PMIC_HI6551_LDO09 == volt_id)
    {
        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        /*开启电源和转换电路，跟硬件确认是否有顺序要求*/
        regval |= (u8)((u32)0x3 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
        /*设置sim卡上下拉电阻配置，放在开启电源后，否则有毛刺*/
        bsp_hi6551_reg_write(HI6551_SIM0_CFG_OFFSET, 0x10);
    }
    else if(PMIC_HI6551_LDO11 == volt_id)
    {
        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        /*开启电源和转换电路，跟硬件确认是否有顺序要求*/
        regval |= (u8)((u32)0x3 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
        /*设置sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM1_CFG_OFFSET, 0x10);
    }
    else if(PMIC_HI6551_BOOST == volt_id)/*boost，写1开启，其他bit位不能修改*/
    {
        bsp_hi6551_reg_read(hi6551_volt->enable_reg_addr, &regval);
        regval |= (u8)((u32)0x1 << hi6551_volt->enable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->enable_reg_addr, regval);
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
        /*恢复sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM0_CFG_OFFSET, 0x0c);
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        /*关闭电源和转换电路，跟硬件确认是否有顺序要求*/
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);
    }
    else if(PMIC_HI6551_LDO11 == volt_id)
    {
        /*恢复sim卡上下拉电阻配置*/
        bsp_hi6551_reg_write(HI6551_SIM1_CFG_OFFSET, 0x0c);
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        /*关闭电源和转换电路，跟硬件确认是否有顺序要求*/
        regval &= ~(u8)((u32)0x3 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);
    }
    else if(PMIC_HI6551_BOOST == volt_id)/*boost，写0关闭，其他bit位不能修改*/
    {
        bsp_hi6551_reg_read(hi6551_volt->disable_reg_addr, &regval);
        regval &= ~(u8)((u32)0x1<<hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);
    }
    else
    {
        regval = (u8)((u32)0x1 << hi6551_volt->disable_bit_offset);
        bsp_hi6551_reg_write(hi6551_volt->disable_reg_addr, regval);
    }

    return BSP_PMU_OK;

}
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
    u8 regval = 0;
    int iret = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;

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

    return (regval & ((u32)0x1 << hi6551_volt->is_enabled_bit_offset));
}
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

    hi6551_volt = (PMIC_HI6551_VLTGS_ATTR *)&(hi6551_volt_table->hi6551_volt_attr[volt_id]);

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
int bsp_hi6551_volt_set_voltage(int volt_id, int min_uV, int max_uV,unsigned *selector)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
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

    hi6551_volt = (PMIC_HI6551_VLTGS_ATTR *)&(hi6551_volt_table->hi6551_volt_attr[volt_id]);

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
        voltage = hi6551_volt->voltage_list[i];
        *selector = i;
    }
    /* 如果设置值有效，设置寄存器 */
    if (valid)
    {
        bsp_hi6551_reg_write_mask(hi6551_volt->voltage_reg_addr, (u8)((u32)i << hi6551_volt->voltage_bit_offset), hi6551_volt->voltage_bit_mask);

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

    return iret;
}

/*硬件提示boost没有使用时需要关闭，否则有烧片可能，在init中将该电源关闭，若需要使用，可以通过配置nv打开*/
void hi6551_set_by_nv(void)
{
    int i = 0;
    u32 iret;
    unsigned selector=0;

    /*init default nv config,all is 0*/
    memset((void*)&pmu_init_config, 0, sizeof(PMU_INIT_NV_STRU));
    /*read nv,get the exc protect setting*/
    iret = bsp_nvm_read(NV_ID_DRV_NV_PMU_INIT,(u8 *)&pmu_init_config,sizeof(PMU_INIT_NV_STRU));

    if(NV_OK != iret)
    {
        pmic_print_error("pmu init read nv error,not set,use the default config!\n");
        return;
    }

    for(i = PMIC_HI6551_VOLT_MIN;i <= PMIC_HI6551_VOLT_MAX;i++)
    {
        if(pmu_init_config.InitConfig[i].IsNeedSet)
        {
            if(pmu_init_config.InitConfig[i].IsVoltSet)
                bsp_hi6551_volt_set_voltage(pmu_init_config.InitConfig[i].VoltId,(int)pmu_init_config.InitConfig[i].Voltage,\
                                            (int)pmu_init_config.InitConfig[i].Voltage,&selector);
            if(pmu_init_config.InitConfig[i].IsOnSet)
                bsp_hi6551_volt_enable(pmu_init_config.InitConfig[i].VoltId);
            if(pmu_init_config.InitConfig[i].IsOffSet)
                bsp_hi6551_volt_disable(pmu_init_config.InitConfig[i].VoltId);
            if(pmu_init_config.InitConfig[i].IsEcoSet)
                bsp_hi6551_volt_set_mode(pmu_init_config.InitConfig[i].VoltId,pmu_init_config.InitConfig[i].EcoMod);
        }
    }
}

void hi6551_optreg_config(void)
{
    /*BUCK2*/
    bsp_hi6551_reg_write(HI6551_BUCK2_REG1_ADJ_OFFSET,0x3c);/*调节LX反馈电阻电容，R第二大*/
    bsp_hi6551_reg_write(HI6551_BUCK2_REG8_ADJ_OFFSET,0x00);/*比较器电流最小*/
    bsp_hi6551_reg_write(HI6551_BUCK2_REG6_ADJ_OFFSET,0x12);/*PG_drver加强1档             */
    bsp_hi6551_reg_write(HI6551_BUCK2_REG7_ADJ_OFFSET,0x03);/*NG_drver最强                */
    bsp_hi6551_reg_write(HI6551_BUCK2_REG4_ADJ_OFFSET,0x20);/*DMD减小1档、OCP减小1档      */
    bsp_hi6551_reg_write(HI6551_BUCK2_REG5_ADJ_OFFSET,0x01);/*N2P 老死区                  */
    /*BUCK3*/
    bsp_hi6551_reg_write(HI6551_BUCK3_REG7_ADJ_OFFSET,0x00);/*比较器电流最小*/
    bsp_hi6551_reg_write(HI6551_BUCK3_REG6_ADJ_OFFSET,0x03);/*NG_drver最强*/
    bsp_hi6551_reg_write(HI6551_BUCK3_REG5_ADJ_OFFSET,0x12);/*PG_drver加强1档*/
    bsp_hi6551_reg_write(HI6551_BUCK3_REG4_ADJ_OFFSET,0x11);/*死区控制方式调节*/
    /*BUCK4*/
    bsp_hi6551_reg_write(HI6551_BUCK4_REG5_ADJ_OFFSET,0x12);/*PG_drver加强1档*/
    bsp_hi6551_reg_write(HI6551_BUCK4_REG7_ADJ_OFFSET,0x01);/*比较器电流保持默认*/
    bsp_hi6551_reg_write(HI6551_BUCK4_REG6_ADJ_OFFSET,0x03);/*NG_drver最强*/
    bsp_hi6551_reg_write(HI6551_BUCK4_REG3_ADJ_OFFSET,0x00);/*DMD减小2档*/
    bsp_hi6551_reg_write(HI6551_BUCK4_REG4_ADJ_OFFSET,0x01);/*死区控制方式调节*/
    /*BUCK5*/
    bsp_hi6551_reg_write(HI6551_BUCK5_REG5_ADJ_OFFSET,0x12);/*PG_drver加强1档*/
    bsp_hi6551_reg_write(HI6551_BUCK5_REG4_ADJ_OFFSET,0x03);/*比较器电流保持默认*/
    bsp_hi6551_reg_write(HI6551_BUCK5_REG7_ADJ_OFFSET,0x00);/*比较器电流最小*/
    bsp_hi6551_reg_write(HI6551_BUCK5_REG6_ADJ_OFFSET,0x03);/*NG_drver最强*/
    bsp_hi6551_reg_write(HI6551_BUCK5_REG3_ADJ_OFFSET,0x01);/*DMD减小1档*/

    /*BUCK6*/
    bsp_hi6551_reg_write(HI6551_BUCK6_REG7_ADJ_OFFSET,0x00);/*比较器电流最小*/
    bsp_hi6551_reg_write(HI6551_BUCK6_REG5_ADJ_OFFSET,0x12);/*PG_drver加强1档*/
    bsp_hi6551_reg_write(HI6551_BUCK6_REG4_ADJ_OFFSET,0x01);/*死区控制方式调节*/
    bsp_hi6551_reg_write(HI6551_BUCK6_REG6_ADJ_OFFSET,0x03);/*NG_drver最强*/
    bsp_hi6551_reg_write(HI6551_BUCK6_REG9_ADJ_OFFSET,0x13);/*regulator 动态范围调整*/

    /*BOOST*/
    bsp_hi6551_reg_write(HI6551_BOOST_ADJ1_OFFSET,0x0e);/*关闭OVP、OCP*/
    bsp_hi6551_reg_write(HI6551_BOOST_ADJ2_OFFSET,0x32);/*关闭DMD*/
    bsp_hi6551_reg_write(HI6551_BOOST_ADJ3_OFFSET,0x3b);/*调整更换电感后环路特性*/
    /*LDO*/
    if((BSP_PMU_V230 == bsp_pmu_version_get()) ||
        (BSP_PMU_V240 == bsp_pmu_version_get()))
    {
        //bsp_hi6551_reg_write(HI6551_LDO22_REG_ADJ_OFFSET,0x01);/*LDO22下拉电阻设置为1.5K*/
        bsp_hi6551_reg_write(HI6551_BUCK4_REG9_ADJ_OFFSET,0x13);/*BUCK4 regulator 动态范围调整*/
        bsp_hi6551_reg_write(HI6551_OCP_DEB_OFFSET,0x45);/*BUCK0-6 SCP滤波*/
    }
    else if((BSP_PMU_V210 == bsp_pmu_version_get())||(BSP_PMU_V220 == bsp_pmu_version_get()))
    {
        bsp_hi6551_reg_write(HI6551_RESERVED1_OFFSET,0x02);     /*bandgap:将基准中buffer的偏置电流减半*/

        /*BUCK2*/
        bsp_hi6551_reg_write(HI6551_BUCK2_REG12_ADJ_OFFSET,0x01);/*发生DMD时不增加ton时间*/
        /*BUCK3*/
        bsp_hi6551_reg_write(HI6551_BUCK3_REG12_ADJ_OFFSET,0x01);/*发生DMD时不增加ton时间*/
        /*BUCK4*/
        bsp_hi6551_reg_write(HI6551_BUCK4_REG9_ADJ_OFFSET,0x15);/*BUCK4 regulator 动态范围调整*/
        bsp_hi6551_reg_write(HI6551_BUCK4_REG12_ADJ_OFFSET,0x01);/*发生DMD时不增加ton时间*/
        /*BUCK5*/
        bsp_hi6551_reg_write(HI6551_BUCK5_REG12_ADJ_OFFSET,0x01);/*发生DMD时不增加ton时间*/
        /*BUCK6*/
        bsp_hi6551_reg_write(HI6551_BUCK6_REG12_ADJ_OFFSET,0x01);/*发生DMD时不增加ton时间*/
        //bsp_hi6551_reg_write(HI6551_LDO22_REG_ADJ_OFFSET,0x31); /*LDO22下拉电阻设置为150欧姆*/
    }
    /*配置ocp*/
    bsp_hi6551_reg_write_mask(HI6551_OCP_MOD_CTRL4_OFFSET,0x0,(0x1<<0));/*LDO5 OCP模式配置为模式二*/
    bsp_hi6551_reg_write_mask(HI6551_OCP_SCP_MOD_CTRL1_OFFSET,0x0,(0x1<<0));/*BUCK01 OCP模式配置为模式二*/
    bsp_hi6551_reg_write_mask(HI6551_OCP_MOD_CTRL7_OFFSET,0x0,(0x1<<4));/*LDO19 OCP模式配置为模式二*/
    bsp_hi6551_reg_write_mask(HI6551_OCP_MOD_CTRL10_OFFSET,0x0,(0x1<<2));/*LVS6 OCP模式配置为模式二*/
}
/*通过reg配置需要默认打开的电源*/
void hi6551_volt_config_by_reg(void)
{
    /*配置hi6551 LDO24 3.0v on，LDO1 2.5v on*/
    bsp_hi6551_reg_write_mask(HI6551_LDO24_REG_ADJ_OFFSET, 0x04,0x07);
    bsp_hi6551_reg_write(HI6551_ENABLE4_OFFSET, (0x01 << 5));

    bsp_hi6551_reg_write_mask(HI6551_LDO1_REG_ADJ_OFFSET, 0x01,0x07);
    bsp_hi6551_reg_write(HI6551_ENABLE2_OFFSET, 0x01);

    /*打开LVS05,给pastar供电*/
    bsp_hi6551_reg_write(HI6551_ENABLE5_OFFSET, (0x01 << 3));

    /*给lcd供电*/
    /*打开LVS04*/
    bsp_hi6551_reg_write(HI6551_ENABLE5_OFFSET, (0x01 << 2));
    /*配置hi6551 LDO14 2.85v on*/
    bsp_hi6551_reg_write_mask(HI6551_LDO14_REG_ADJ_OFFSET, 0x05,0x07);
    bsp_hi6551_reg_write(HI6551_ENABLE3_OFFSET, (0x01 << 3));
}
/*通过开关函数配置需要默认打开的电源*/
void hi6551_volt_config(void)
{
    unsigned selector = 0;

    /*硬件提示boost没有使用时需要关闭，否则有烧片可能,需要使用该电源的平台可以通过nv配置打开，或者注掉该代码*/
    bsp_hi6551_volt_disable(PMIC_HI6551_BOOST);
    /*给hkadc供电*/
    /*配置hi6551 LDO24 3.0v on*/
    bsp_hi6551_volt_set_voltage(PMIC_HI6551_LDO24,3000000,3000000,&selector);
    bsp_hi6551_volt_enable(PMIC_HI6551_LDO24);
    /*配置hi6551 LDO1 2.5v on*/
    bsp_hi6551_volt_set_voltage(PMIC_HI6551_LDO01,2500000,2500000,&selector);
    bsp_hi6551_volt_enable(PMIC_HI6551_LDO01);

#if 0
    /*给lcd供电，留给模块中自己实现*/
    /*打开LVS04*/
    bsp_hi6551_volt_enable(PMIC_HI6551_LVS04);
    /*配置hi6551 LDO14 2.85v on*/
    bsp_hi6551_volt_set_voltage(PMIC_HI6551_LDO14,2850000,2850000,&selector);
    bsp_hi6551_volt_enable(PMIC_HI6551_LDO14);
    /*打开LVS05,给pastar供电*/
    bsp_hi6551_volt_enable(PMIC_HI6551_LVS05);
#endif
}
extern void hi6551_volttable_copytoddr(void);
/*****************************************************************************
 函 数 名  : pmu_hi6551_init
 功能描述  : pmu hi6551模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot中pmu模块初始化时调用
*****************************************************************************/
void pmu_hi6551_init(void)
{
    u64 *volt_need_off = (u64 *)SHM_PMU_OCP_INFO_ADDR;/*过流，需要关闭的过流源*/
    u8 iregVal;

    pmic_print_info("\r\npmu_hi6551_init!");

	*volt_need_off = 0;

    /*将电压控制表拷贝到DDR中*/
    hi6551_volttable_copytoddr();
    pmic_print_info("\r\nhi6551_volttable_copytoddr ok!");

    /*初始化默认配置*/

    /*配置硬件提供的电源优化配置参数*/
    hi6551_optreg_config();
    /*配置软硬件接口文档中需要的默认电源配置*/
    hi6551_volt_config();

    /* set dac */
    bsp_hi6551_reg_read(HI6551_DAC_CTRL_OFFSET,&iregVal);
    iregVal |=  0x01;
    bsp_hi6551_reg_write(HI6551_DAC_CTRL_OFFSET, iregVal);
    /*配置 buck3受Peri_EN控制上下电,bit5*/
    bsp_hi6551_reg_read(HI6551_ENB3_ECO_MARK_OFFSET,&iregVal);
    iregVal |= 0x3c;
    bsp_hi6551_reg_write(HI6551_ENB3_ECO_MARK_OFFSET, iregVal);

    /*没有库仑计的产品形态，关闭库仑计，有库仑计的显式打开一下*/
#ifdef CONFIG_COUL
    bsp_hi6551_reg_read(HI6551_CLJ_CTRL_REG_OFFSET,&iregVal);
    iregVal |= (0x01 << 7);
    bsp_hi6551_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, iregVal);
#else
    bsp_hi6551_reg_read(HI6551_CLJ_CTRL_REG_OFFSET,&iregVal);
    iregVal &=  ~(0x01 << 7);
    bsp_hi6551_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, iregVal);
#endif

    pmic_print_info("pmu_hi6551_init ok!\n");
}

/*以下函数是提供给其他模块使用的接口，还需要adp(pmu.c)封装*/
/*****************************************************************************
 函 数 名  : hi6551_power_key_state_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出(1:插入；0:拔出)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6551_power_key_state_get(void)
{
    u8 regval = 0;

    bsp_hi6551_reg_read(HI6551_STATUS1_OFFSET, &regval);
    return  (regval & PMU_HI6551_POWER_KEY_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : hi6551_usb_state_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出(1:插入；0:拔出)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6551_usb_state_get(void)
{
    u8 regval = 0;

    bsp_hi6551_reg_read(HI6551_STATUS2_OFFSET, &regval);
    return  (regval & PMU_HI6551_USB_STATE_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : hi6551_hreset_state_get
 功能描述  : 判断pmu是否为热复位
 输入参数  : void
 输出参数  : 无
 返 回 值     : pmu是热启动或冷启动(1:热启动；0:冷启动)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6551_hreset_state_get(void)
{
    u8 regval = 0;

    bsp_hi6551_reg_read(HI6551_NP_IRQ1_RECORD_OFFSET, &regval);
    return  (regval & PMU_HI6551_HRESET_STATE_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : hi6551_hreset_state_get
 功能描述  : 清除pmu热复位标志
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
void hi6551_hreset_state_clear(void)
{
    bsp_hi6551_reg_write(HI6551_NP_IRQ1_RECORD_OFFSET, PMU_HI6551_HRESET_STATE_MASK);
}
/*****************************************************************************
 函 数 名  : hi6551_version_get
 功能描述  : 获取pmu的版本号
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
u8 hi6551_version_get(void)
{
    u8 regval = 0;

    bsp_hi6551_reg_read(HI6551_VERSION_OFFSET, &regval);
    return  regval;
}
/*****************************************************************************
 函 数 名  : hi6551_get_boot_state
 功能描述  : 系统启动时检查pmu寄存器状态，
                确认是否是由pmu引起的重启
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  : 系统可维可测
*****************************************************************************/
int hi6551_get_boot_state(void)
{

    u8 hi6551_om_boot[12] = {0};
    u32 j = 0;
    int iret = PMU_STATE_OK;
    int flag = 0;

    for(j = 0; j < 11; j++)
    {
        bsp_hi6551_reg_read((HI6551_NP_IRQ1_RECORD_OFFSET+j), &hi6551_om_boot[j]);
    }

    /*欠压，过压*/
    if((hi6551_om_boot[0] & (0x01 << HI6551_VSYS_UNDER_2P5_OFFSET ))||(hi6551_om_boot[0] & (0x01 << HI6551_VSYS_UNDER_2P7_OFFSET )))
    {
        iret|= PMU_STATE_UNDER_VOL;
        pmic_print_error("hi6551 under voltage last time,check hardware!\n");
    }
    if(hi6551_om_boot[0] & (0x01 << HI6551_VSYS_OVER_6P0_OFFSET ))
    {
        iret|=  PMU_STATE_OVER_VOL;
        pmic_print_error("hi6551 over voltage last time,check hardware!\n");
    }
    /*过温*/
    if(hi6551_om_boot[1] & (0x01 << HI6551_OTMP_150_OFFSET ) )
    {
        iret|=  PMU_STATE_OVER_TEMP;
        pmic_print_error("hi6551 over temperature last time,check hardware!\n");
    }
    /*过流,包括buck短路，各路过流，boost欠压、过压、短路*/
    for(j= 0; j < 8; j++)
    {
        if(hi6551_om_boot[j+3])
            flag = 1;
    }
    if(flag)
    {
        iret|=  PMU_STATE_OVER_CUR;
        pmic_print_error("hi6551 over current last time,check hardware!\n");
    }

    return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_lvs4_switch
 功能描述  : 开关lvs04电源
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : lcd模块调用
*****************************************************************************/
int hi6551_lvs4_switch(power_switch_e sw)
{
    int iret = BSP_PMU_OK;
    switch(sw)
    {
        case POWER_OFF:
            iret = bsp_hi6551_volt_disable(PMIC_HI6551_LVS04);
            break;
        case POWER_ON:
            iret = bsp_hi6551_volt_enable(PMIC_HI6551_LVS04);
            break;
        default:
            break;
    }
    pmic_print_info("hi6551_lvs4_switch ok!\n");
    return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_ldo14_switch
 功能描述  : 开关ldo14电源
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : lcd模块调用
*****************************************************************************/
int hi6551_ldo14_switch(power_switch_e sw)
{
    int iret = BSP_PMU_OK;

    switch(sw)
    {
        case POWER_OFF:
            iret = bsp_hi6551_volt_disable(PMIC_HI6551_LDO14);
            break;
        case POWER_ON:
            iret = bsp_hi6551_volt_enable(PMIC_HI6551_LDO14);
            break;
        default:
            break;
    }
    pmic_print_info("hi6551_ldo14_switch ok!\n");
    return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_ldo14_volt_set
 功能描述  : 设置ldo14电源电压
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : lcd模块调用
*****************************************************************************/
int hi6551_ldo14_volt_set(int voltage)
{
    unsigned selector = 0;
    int iret = BSP_PMU_OK;

     iret = bsp_hi6551_volt_set_voltage(PMIC_HI6551_LDO14,voltage,voltage,&selector);

     pmic_print_info("hi6551_ldo14_volt_set ok!\n");
     return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_ldo23_switch
 功能描述  : 开关ldo23电源
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : efuse模块调用
*****************************************************************************/
int hi6551_ldo23_switch(power_switch_e sw)
{
    int iret = BSP_PMU_OK;

    switch(sw)
    {
        case POWER_OFF:
            iret = bsp_hi6551_volt_disable(PMIC_HI6551_LDO23);
            break;
        case POWER_ON:
            iret = bsp_hi6551_volt_enable(PMIC_HI6551_LDO23);
            break;
        default:
            break;
    }
    pmic_print_info("hi6551_ldo14_switch ok!\n");
    return iret;
}
/*****************************************************************************
 函 数 名  : hi6551_ldo23_volt_set
 功能描述  : 设置ldo23电源电压
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  : efuse模块调用
*****************************************************************************/
int hi6551_ldo23_volt_set(int voltage)
{
    unsigned selector = 0;
    int iret = BSP_PMU_OK;

     iret = bsp_hi6551_volt_set_voltage(PMIC_HI6551_LDO23,voltage,voltage,&selector);

     pmic_print_info("hi6551_ldo23_volt_set ok!\n");
     return iret;
}
