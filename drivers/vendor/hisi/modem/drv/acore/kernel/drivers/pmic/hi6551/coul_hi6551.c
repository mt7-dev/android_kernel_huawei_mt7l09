/*lint --e{537,958}*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/mach/irq.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <drv_comm.h>
#include <asm/uaccess.h>
/*#include <osl_math64.h>*/
#include <osl_spinlock.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include <hi_smartstar.h>
#include <bsp_coul.h>
#include "coul_hi6551.h"

struct coul_data{
    spinlock_t      lock;
};
struct coul_data g_coul_st;

#define COUL_BEFORE_TIMES_MAX    19
#define R_COUL_MOHM 10/*参考电阻:10 mohm*/
/*COUL校准配置nv值*/
COUL_CALI_NV_TYPE    coul_cali_config={1000,0,1000,0};/*默认值，无校准*/

void coul_reg_write(u16 reg, u8 val)
{
    bsp_hi6551_reg_write(reg, val);
}

u8 coul_reg_read(u16 reg)
{
    unsigned char val = 0;

    bsp_hi6551_reg_read(reg, &val);

    return val;
}

void coul_reg_mask(u16 reg,u8 val,u8 mask)
{
    bsp_hi6551_reg_write_mask(reg, val, mask);
}
#if 0
/*****************************************************************************
 函 数 名  : coul_battery_voltage
 功能描述  : 寄存器值转换成电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2uv
 * 1 bit = 225.1196 uv = 4.8/21322 V = 4.8/21322 * 1000 * 1000 uV = 24 * 1000 * 100/ 10661 uV
 * convert regval to uv
 */
u64 coul_convert_regval2uv(unsigned short reg_val)
{
    u64 temp;

    if (reg_val & 0x8000)
    {
        return -1;
    }

    temp = (u64)((u64)(reg_val)  * (u64)(24 * 1000 * 100));

    temp = div_u64(temp, 10661);

#if 1 /* for debug */
    temp = (u64) coul_cali_config.v_offset_a *temp;
/*    temp = div_u64(temp, 1000000);*/
    temp += coul_cali_config.v_offset_b;
#endif

    return temp;
}
/*****************************************************************************
 函 数 名  : coul_convert_regval2ua
 功能描述  : 电流转换函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2ua
 * 10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA
 * 20 mohm resistance: 1 bit = 10 mohm / 2
 * 30 mohm resistance: 1 bit = 10 mohm / 3
 * ...
 * high bit = 0 is in, 1 is out
 * convert regval to ua
 */
s64 coul_convert_regval2ua(short reg_val)
{
    int ret;
    s64 temp;

    ret = reg_val;
    temp = (s64)(ret) * (s64)(1000 * 1000 * 5);
    temp = div_s64(temp, 10661);

    ret = temp / (R_COUL_MOHM/10);

    ret = -ret;/*何意?*/

#if 1 /* for debug */
    temp = (s64) coul_cali_config.c_offset_a *ret;
 /*   ret = div_s64(temp, 1000000);*/
    ret += coul_cali_config.c_offset_b;
#endif

    return ret;
}
/*****************************************************************************
 函 数 名  : coul_battery_voltage
 功能描述  : 电量转换函数(单位:uC)
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2uc
 * 1 bit = 2.345*110*10^(-6) C = 2.5795*10^(-5) C = 25795 / 1000 uC
 * convert regval to uv
 */
u64 coul_convert_regval2uc(unsigned short reg_val)
{
    int ret;
    s64 temp;

    ret = reg_val;
    temp = (s64)(ret) * (s64)(25795);
    temp = div_s64(temp, 1000);

    return temp;
}
#endif
/*****************************************************************************
 函 数 名  : coul_convert_regval2mv
 功能描述  : 寄存器值转换成电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2uv
 * 1 bit = 225.1196 uv = 4.8/21322 V = 4.8/21322 * 1000 * 1000 uV = 24 * 1000 * 100/ 10661 uV
 * convert regval to mv
 */
unsigned int coul_convert_regval2mv(unsigned short reg_val)
{
    unsigned int temp;

    if (reg_val & 0x8000)/*电压不会是负值*/
    {
        return BSP_COUL_UNVALID;
    }

    temp = (unsigned int)(reg_val*225);

    temp = temp / 1000;/*单位转化为mV*/

    return temp;
}
/*****************************************************************************
 函 数 名  : coul_convert_regval2ua
 功能描述  : 电流转换函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2ua
 * 10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA = 469uA
 * 20 mohm resistance: 1 bit = 10 mohm / 2
 * 30 mohm resistance: 1 bit = 10 mohm / 3
 * ...
 * high bit = 0 is in, 1 is out
 * convert regval to ma
 */
signed int coul_convert_regval2ma(signed short reg_val)
{
    signed curr;
    s32 temp;

    temp = (s32)reg_val * 469;/*uA*/

    curr = temp / (R_COUL_MOHM/10);/*uA*/

    curr = curr / 1000;/*转化为mA*/

    return curr;
}
/*****************************************************************************
 函 数 名  : coul_battery_voltage
 功能描述  : 电量转换函数(单位:uC)
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned long long
 调用函数  :
 被调函数  :
*****************************************************************************/
/**
 * convert_regval2uc
 * 1 bit = 2.345*110*10^(-6) C = 2.5795*10^(-5) C = 25.795  uC
 * convert regval to uv
 */
unsigned long long coul_convert_regval2uc(unsigned int reg_val)
{
    unsigned long long temp;

    temp = (unsigned long long)reg_val * 26;/*uc*/
 /*   temp = (temp / 1000);*//*mC**/

    return temp;
}
/*****************************************************************************
 函 数 名  : coul_cali_voltage
 功能描述  : 获取校准后的电压
 输入参数  : voltage:未经校准的电压
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_cali_voltage(unsigned voltage)
{
    int calied_volt;

    calied_volt = (int)(coul_cali_config.v_offset_a *voltage);
    calied_volt = calied_volt / 1000;/*消除由于校准斜率计算产生的数量级误差*/
    calied_volt += coul_cali_config.v_offset_b;

    return (unsigned int)calied_volt;
}
/*****************************************************************************
 函 数 名  : coul_cali_current
 功能描述  : 获取校准后的电流
 输入参数  : cur:未经校准的电流
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int coul_cali_current(signed cur)
{
    signed int calied_cur;

    calied_cur = (signed int)coul_cali_config.c_offset_a * cur;
    calied_cur = calied_cur / 1000;/*消除由于校准斜率计算产生的数量级误差*/
    calied_cur += coul_cali_config.c_offset_b;

    return calied_cur;
}
/*****************************************************************************
 函 数 名  : coul_convert_regval2mv_calied
 功能描述  : 由寄存器值获取校准后的电压
 输入参数  : voltage:未经校准的电压
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_convert_regval2mv_calied(unsigned short reg_val)
{
    unsigned int uncali_volt;
    unsigned int calied_volt;

    uncali_volt = coul_convert_regval2mv(reg_val);
    calied_volt = coul_cali_voltage(uncali_volt);

    return calied_volt;
}
/*****************************************************************************
 函 数 名  : coul_convert_regval2calivolt
 功能描述  : 由寄存器值获取校准后的电流
 输入参数  : cur:未经校准的电流
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int coul_convert_regval2ma_calied(signed short reg_val)
{
    signed int uncali_cur;
    signed int calied_cur;

    uncali_cur = coul_convert_regval2ma(reg_val);
    calied_cur = coul_cali_current(uncali_cur);

    return calied_cur;
}
/*****************************************************************************
 函 数 名  : coul_voltage_regval
 功能描述  : 当前电压寄存器获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned short
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned short coul_voltage_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned short val = 0;

    val1 = coul_reg_read(HI6551_V_OUT0_PRE0_OFFSET);
    val2 = coul_reg_read(HI6551_V_OUT1_PRE0_OFFSET);

    val = (unsigned short)(val1 | ((u32)(val2 & 0x7f)<< 8));/*电压为正值，最高位为符号位，不关注*/
    return val;
}
/*****************************************************************************
 函 数 名  : coul_voltage_before_regval
 功能描述  : 获取当前电压前times次电压寄存器值，单位:uv
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned short
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned short coul_voltage_before_regval(unsigned int times)
{
    unsigned char val1;
    unsigned char val2;
    unsigned short val = 0;

    val1 = coul_reg_read((u16)(HI6551_V_OUT0_PRE0_OFFSET + 2 * times));
    val2 = coul_reg_read((u16)(HI6551_V_OUT1_PRE0_OFFSET + 2 * times ));

    val = (unsigned short)(val1 | ((u32)(val2 & 0xef)<< 8));/*电压为正值，最高位为符号位，不关注*/
    return val;
}
/*****************************************************************************
 函 数 名  : coul_battery_current_regval
 功能描述  : 当前电流寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed short
 调用函数  :
 被调函数  :
*****************************************************************************/
signed short coul_current_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    signed short val = 0;

    val1 = coul_reg_read(HI6551_CURRENT0_PRE0_OFFSET);
    val2 = coul_reg_read(HI6551_CURRENT1_PRE0_OFFSET);

    val = (signed short)(val1 | ((u32)val2 << 8));

    return val;
}
/*****************************************************************************
 函 数 名  : coul_current_before_regval
 功能描述  : 获取当前电压前times次电流寄存器值
 输入参数  : times:要获取的前几次的电流值([范围[1,20])
 输出参数  : 无
 返 回 值  : signed short
 调用函数  :
 被调函数  :
*****************************************************************************/
signed short coul_current_before_regval(unsigned int times)
{
    unsigned char val1;
    unsigned char val2;
    signed short val = 0;

    val1 = coul_reg_read((u16)(HI6551_CURRENT0_PRE0_OFFSET + 2 * times));
    val2 = coul_reg_read((u16)(HI6551_CURRENT1_PRE0_OFFSET + 2 * times));/*最高位代表符号位*/

    val = (signed short)(val1 | ((u32)val2 << 8));
    return val;
}
/*****************************************************************************
 函 数 名  : coul_in_capacity_regval
 功能描述  : 流入电量寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_in_capacity_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
    unsigned char val4;
    unsigned int in;

    val1 = coul_reg_read(HI6551_CL_IN0_OFFSET);
    val2 = coul_reg_read(HI6551_CL_IN1_OFFSET);
    val3 = coul_reg_read(HI6551_CL_IN2_OFFSET);
    val4 = coul_reg_read(HI6551_CL_IN3_OFFSET);
    in = (unsigned int)(val1 | ((u32)val2 << 8) | ((u32)val3 << 16) | ((u32)val4 << 24));

    return in;
}
/*****************************************************************************
 函 数 名  : coul_out_capacity_regval
 功能描述  : 流出电量寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_out_capacity_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
    unsigned char val4;
    unsigned int out;

    val1 = coul_reg_read(HI6551_CL_OUT0_OFFSET);
    val2 = coul_reg_read(HI6551_CL_OUT1_OFFSET);
    val3 = coul_reg_read(HI6551_CL_OUT2_OFFSET);
    val4 = coul_reg_read(HI6551_CL_OUT3_OFFSET);

    out = (unsigned int)(val1 | ((u32)val2 << 8) | ((u32)val3 << 16) | ((u32)val4 << 24));

    return out;
}
/*****************************************************************************
 函 数 名  : coul_ocv_data_regval
 功能描述  : 电池电压开路电压数值寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned short
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned short coul_ocv_data_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned short val = 0;

    val1 = coul_reg_read(HI6551_OCV_DATA1_OFFSET);
    val2 = coul_reg_read(HI6551_OCV_DATA2_OFFSET);

    val = (unsigned short)(val1 | ((u32)val2 << 8));
    return val;
}
/*****************************************************************************
 函 数 名  : coul_ocv_offset_regval
 功能描述  : 电池电压开路电压误差寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned short
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned short coul_ocv_offset_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned short val = 0;

    val1 = coul_reg_read(HI6551_OFFSET_VOLTAGE0_OFFSET);
    val2 = coul_reg_read(HI6551_OFFSET_VOLTAGE1_OFFSET);

    val = (unsigned short)(val1 | ((u32)val2 << 8));
    return val;
}
/*****************************************************************************
 函 数 名  : coul_battery_charge_time_regval
 功能描述  : 库仑计充电时间计数寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_charge_time_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
    unsigned char val4;

    val1 = coul_reg_read(HI6551_LOAD_TIMER0_OFFSET);
    val2 = coul_reg_read(HI6551_LOAD_TIMER1_OFFSET);
    val3 = coul_reg_read(HI6551_LOAD_TIMER2_OFFSET);
    val4 = coul_reg_read(HI6551_LOAD_TIMER3_OFFSET);

    return (unsigned int)val1 | ((u32)val2 << 8) | ((u32)val3 << 16) | ((u32)val4 << 24);
}

/*****************************************************************************
 函 数 名  : coul_battery_discharge_time_regval
 功能描述  : 放电时间寄存器值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int coul_discharge_time_regval(void)
{
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
    unsigned char val4;

    val1 = coul_reg_read(HI6551_CHG_TIMER0_OFFSET);
    val2 = coul_reg_read(HI6551_CHG_TIMER1_OFFSET);
    val3 = coul_reg_read(HI6551_CHG_TIMER2_OFFSET);
    val4 = coul_reg_read(HI6551_CHG_TIMER3_OFFSET);

    return (unsigned int)val1 | ((u32)val2 << 8) | ((u32)val3 << 16) | ((u32)val4 << 24);
}

/*****************************************************************************
 函 数 名  : coul_set_vbat_value
 功能描述  : 设置电池低电值(需要校准转化后的数据)
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void coul_voltage_low_regval_set(unsigned short value)
{
    unsigned char val1 = (value & 0xff);
    unsigned char val2 = (value >> 8) & 0xff;

    coul_reg_write(HI6551_V_INT0_OFFSET, val1);
    coul_reg_write(HI6551_V_INT1_OFFSET, val2);
}

/*****************************************************************************
 函 数 名  : coul_power_ctrl
 功能描述  :
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void coul_power_ctrl(COUL_POWER_CTRL ctrl)
{
    unsigned char val;
    coul_irqflags_t coul_flags = 0;

    spin_lock_irqsave(&g_coul_st.lock,coul_flags);
    val = coul_reg_read(HI6551_CLJ_CTRL_REG_OFFSET);
    if(COUL_POWER_ON == ctrl){
        val |= COUL_CTRL_ONOFF_MASK;
    }else{
        val &= ~COUL_CTRL_ONOFF_MASK;
    }
    coul_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, val);
    spin_unlock_irqrestore(&g_coul_st.lock,coul_flags);
}

/*****************************************************************************
 函 数 名  : coul_cali_ctrl
 功能描述  : 是否强制进入校准状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void coul_cali_ctrl(COUL_CALI_CTRL ctrl)
{
    unsigned char val;
    coul_irqflags_t coul_flags = 0;

    spin_lock_irqsave(&g_coul_st.lock,coul_flags);
    val = coul_reg_read(HI6551_CLJ_CTRL_REG_OFFSET);
    if(COUL_CALI_ON == ctrl){
        val |= COUL_CTRL_CALI_MASK;
    }else{
        val &= ~COUL_CTRL_CALI_MASK;
    }
    coul_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, val);
    spin_unlock_irqrestore(&g_coul_st.lock,coul_flags);
}

/*****************************************************************************
 函 数 名  : coul_reflash_ctrl
 功能描述  : 刷新控制
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void coul_reflash_ctrl(COUL_REFLASH_CTRL ctrl)
{
    unsigned char val;
    coul_irqflags_t coul_flags = 0;

    spin_lock_irqsave(&g_coul_st.lock,coul_flags);
    val = coul_reg_read(HI6551_CLJ_CTRL_REG_OFFSET);
    if(COUL_REFLASH_ECO == ctrl){
        val |= COUL_CTRL_REFLASH_MASK;
    }else{
        val &= ~COUL_CTRL_REFLASH_MASK;
    }
    coul_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, val);
    spin_unlock_irqrestore(&g_coul_st.lock,coul_flags);
}

/*****************************************************************************
 函 数 名  : coul_eco_ctrl
 功能描述  :
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void coul_eco_ctrl(COUL_ECO_CTRL ctrl)
{
    unsigned char val;
    coul_irqflags_t coul_flags = 0;

    spin_lock_irqsave(&g_coul_st.lock,coul_flags);
    val = coul_reg_read(HI6551_CLJ_CTRL_REG_OFFSET);
    if(COUL_ECO_FORCE == ctrl){
        val |= COUL_CTRL_ECO_MASK;
    }else{
        val &= ~COUL_CTRL_ECO_MASK;
    }
    coul_reg_write(HI6551_CLJ_CTRL_REG_OFFSET, val);
    spin_unlock_irqrestore(&g_coul_st.lock,coul_flags);
}

/*****************************************************************************
 函 数 名  : bsp_coul_init
 功能描述  : 库仑计模块初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 初始化成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_coul_init(void)
{
    u32 iret = BSP_COUL_OK;

    /*默认在fastboot中已配置*/
#if 0
    unsigned int low_batt_thres = 3400;

    /* 库仑计默认已开启,其他功能后续提供*/

    coul_power_ctrl(COUL_POWER_ON);

    coul_cali_ctrl(COUL_CALI_ON);

    coul_reflash_ctrl(COUL_REFLASH_ECO);

    coul_eco_ctrl(COUL_REFLASH_ECO);

    coul_set_vbat_value(low_batt_thres);
#endif
    spin_lock_init(&g_coul_st.lock);/*中断只在Acore实现，多core互斥锁*/

    /*读取nv值，获取电压电流计算参数*/
    /*read nv,get the exc protect setting*/
    iret = bsp_nvm_read(NV_ID_DRV_COUL_CALI,(unsigned char *)&coul_cali_config,sizeof(COUL_CALI_NV_TYPE));

    if(NV_OK != iret)
    {
        coul_err("coul cali read nv error,not set,use the default config!\n");
    }
    else
        coul_err("coul init ok!\n");

    return BSP_COUL_OK;
}
/*********************以下接口思考是否有接口需要使用**************************************/
/*****************************************************************************
 函 数 名  : bsp_coul_enable
 功能描述  : 开启库仑计
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_enable(void)
{
    coul_power_ctrl(COUL_POWER_ON);
}
/*****************************************************************************
 函 数 名  : bsp_coul_disable
 功能描述  : 关闭库仑计
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_disable(void)
{
    coul_power_ctrl(COUL_POWER_OFF);
}
/*********************以下接口提供给产品线使用**************************************/
/*采样校准参数时调用*/
/*****************************************************************************
 函 数 名  : bsp_coul_voltage_uncali
 功能描述  : 获取未经校准当前电压，单位:mV
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_voltage_uncali(void)
{
    unsigned short  val = 0;
    unsigned int voltage = 0;

    val = coul_voltage_regval();

    voltage = coul_convert_regval2mv(val);

    coul_dbg("uncali:coul_battery_voltage is %d mV\n",voltage);

    return voltage;
}
/*****************************************************************************
 函 数 名  : bsp_coul_current_uncali
 功能描述  : 获取未经校准当前电流,单位:mA
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int  bsp_coul_current_uncali(void)
{
    signed short  val = 0;
    signed int bat_current = 0;

    val = coul_current_regval();

    bat_current = coul_convert_regval2ma(val);

    coul_dbg("uncali:coul_battery_current is %d mA\n",bat_current);

    return bat_current;
}
/*****************************************************************************
 函 数 名  : bsp_coul_voltage
 功能描述  : 当前电压获取，单位:mV
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_voltage(void)
{
    unsigned short  val = 0;
    unsigned int voltage = 0;

    val = coul_voltage_regval();

    voltage = coul_convert_regval2mv_calied(val);

    coul_dbg("calied:coul_battery_voltage is %d mV\n",voltage);

    return voltage;
}
/*****************************************************************************
 函 数 名  : bsp_coul_voltage_before
 功能描述  : 获取当前电压第times次电压值，单位:mV
 输入参数  : times:要获取最近的第n次的电压值([范围[1,20])
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int  bsp_coul_voltage_before(unsigned int times)
{
    unsigned short  val = 0;
    unsigned int  voltage = 0;

    if(COUL_BEFORE_TIMES_MAX < times)
    {
        coul_err("coul can only support 19 times before!");
        return BSP_COUL_UNVALID;/*怎样区分错误码和电压值?*/
    }

    val = coul_voltage_before_regval(times);

    voltage = coul_convert_regval2mv_calied(val);

    coul_dbg("calied:before %d times voltage is %d mV\n",times,voltage);

    return voltage;
}
/*****************************************************************************
 函 数 名  : bsp_coul_current
 功能描述  : 当前电流获取,单位:mA
 输入参数  : void
 输出参数  : 无
 返 回 值  : signed long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int  bsp_coul_current(void)
{
    signed short  val = 0;
    signed int bat_current = 0;

    val = coul_current_regval();

    bat_current = coul_convert_regval2ma_calied(val);

    coul_dbg("calied:coul_battery_current is %d mA\n",bat_current);

    return bat_current;
}
/*****************************************************************************
 函 数 名  : bsp_coul_current_before
 功能描述  : 获取当前电流前times次电流值，单位:mA
 输入参数  : times:要获取的前几次的电流值([范围[1,20])
 输出参数  : 无
 返 回 值  : unsigned long long
 调用函数  :
 被调函数  :
*****************************************************************************/
signed int bsp_coul_current_before(unsigned int times)
{
    signed short  val = 0;
    signed int  bat_current = 0;

    if(COUL_BEFORE_TIMES_MAX < times)
    {
        coul_err("coul can only support 19 times before!");
        return BSP_COUL_ERR;/*怎样区分错误码和电压值?*/
    }

    val = coul_current_before_regval(times);

    bat_current = coul_convert_regval2ma_calied(val);

    coul_dbg("calied:before %d times current is %d mA\n",times,bat_current);

    return bat_current;
}
/*****************************************************************************
 函 数 名  : bsp_coul_in_capacity
 功能描述  : 电池流入电量获取,单位:uC
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned long long
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned long long bsp_coul_in_capacity(void)
{
    unsigned int  val = 0;
    unsigned long long capacity = 0;

    val = coul_in_capacity_regval();

    capacity = coul_convert_regval2uc(val);

    coul_dbg("in capacity is %lld uC\n",capacity);

    return capacity;
}
/*****************************************************************************
 函 数 名  : bsp_coul_out_capacity
 功能描述  : 电池流出电量获取,单位:mC
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned long long
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned long long bsp_coul_out_capacity(void)
{
    unsigned int  val = 0;
    unsigned long long capacity = 0;

    val = coul_out_capacity_regval();

    capacity = coul_convert_regval2uc(val);

    coul_dbg("out capacity is %lld uC\n",capacity);

    return capacity;
}
/*****************************************************************************
 函 数 名  : bsp_coul_charge_time
 功能描述  : 充电时间获取,单位:s
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_charge_time(void)
{
    unsigned int  time = 0;

    time = coul_charge_time_regval();

    coul_dbg("charge_time is %d s\n",time);

    return time;
}
/*****************************************************************************
 函 数 名  : bsp_coul_charge_time
 功能描述  : 充电时间获取,单位:s
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_discharge_time(void)
{
    unsigned int  time = 0;

    time = coul_discharge_time_regval();

    coul_dbg("discharge_time is %d s\n",time);

    return time;
}
/*****************************************************************************
 函 数 名  : bsp_coul_eco_filter_time
 功能描述  : 设置库仑计eco滤波时间
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_eco_filter_time(COUL_FILETR_TIME filter_time)
{
    coul_irqflags_t coul_flags = 0;

    spin_lock_irqsave(&g_coul_st.lock,coul_flags);
    coul_reg_mask(HI6551_CLJ_CTRL_REG_OFFSET,filter_time << COUL_CTRL_FILETR_OFFSET ,COUL_CTRL_FILETR_MASK);
    spin_unlock_irqrestore(&g_coul_st.lock,coul_flags);
}
/*****************************************************************************
 函 数 名  : bsp_coul_int_register
 功能描述  : 注册库仑计中断回调函数
 输入参数  : int:中断号；func:中断处理回调函数，data:回调函数参数指针
 输出参数  : 无
 返 回 值  : 注册成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_coul_int_register(COUL_INT_TYPE irq,COUL_INT_FUNC func,void *data)
{
    /*注册给pmu中断处理*/
    if(bsp_pmu_irq_callback_register(irq,func,data))
        return BSP_COUL_ERR;
    else
        return BSP_COUL_OK;
}
/*****************************************************************************
 函 数 名  : coul_ocv_get
 功能描述  : 电池开路电压采样值获取
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_coul_ocv_get(void)
{
    unsigned short val = 0;
    unsigned short data_val = 0;
    unsigned short offset_val = 0;
    unsigned int voltage = 0;

    data_val = coul_ocv_data_regval();
    offset_val = coul_ocv_offset_regval();
    val = data_val - offset_val;
    voltage = coul_convert_regval2mv_calied(val);

    coul_dbg("calied:coul_ocv_voltage is %d mv!\n",voltage);

    return voltage;
}

/*后续提供*/
#if 0
/*****************************************************************************
 函 数 名  : bsp_coul_vbat_set
 功能描述  : 设置电池低电值,中断的阈值
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_coul_vbat_set(unsigned short value)
{
    unsigned char val1 = (value & 0xff);
    unsigned char val2 = (value >> 8) & 0xff;

    coul_reg_write(HI6551_V_INT0_OFFSET, val1);
    coul_reg_write(HI6551_V_INT1_OFFSET, val2);
}
#endif
static void __exit bsp_coul_exit(void)
{
}
module_exit(bsp_coul_exit);
module_init(bsp_coul_init);
MODULE_LICENSE("GPL");

