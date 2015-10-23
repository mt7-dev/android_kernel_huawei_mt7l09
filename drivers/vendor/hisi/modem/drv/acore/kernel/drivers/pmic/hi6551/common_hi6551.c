

/*lint --e{537,958}*/
#if defined(__KERNEL__)
#include <osl_irq.h>
#endif
#include <osl_bio.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <hi_smartstar.h>
#include <bsp_ipc.h>
#include "pmu_balong.h"
#include "common_hi6551.h"

struct hi6551_common_data{
    spinlock_t      lock;
};
struct hi6551_common_data g_hi6551_comm_st;

#if defined(__KERNEL__)

#include<bsp_pmu.h>

/*32K时钟偏移*/
static u8 hi6551_32k_clk_offset[PMU_32K_CLK_MAX + 1] = {7,6,5};
static u32 hi6551_backbat_volt[3] ={2500,3000,3300};

/*para check func*/
static __inline__ int hi6551_32k_para_check(pmu_clk_e clk_id)
{
    if(PMU_32K_CLK_MAX <= clk_id || PMU_32K_CLK_A > clk_id)
        return BSP_PMU_ERROR;
    else
        return BSP_PMU_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_32k_clk_enable
 功能描述  : 开启对应路32k时钟
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
int hi6551_32k_clk_enable(pmu_clk_e clk_id)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;
    u8 bit_offset;

    if(hi6551_32k_para_check(clk_id))
        return BSP_PMU_ERROR;

    bit_offset = hi6551_32k_clk_offset[clk_id];     /*BIT偏移*/

    /*核内互斥*/
    /*lint --e{746,718}*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_ONOFF8_OFFSET, &regval);
    regval |= (u8)((u32)0x1 << bit_offset);
    bsp_hi6551_reg_write(HI6551_ONOFF8_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);

    return BSP_PMU_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_32k_clk_disable
 功能描述  : 关闭对应路32k时钟
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
int hi6551_32k_clk_disable(pmu_clk_e clk_id)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;
    u8 bit_offset;

    if(hi6551_32k_para_check(clk_id))
        return BSP_PMU_ERROR;

    if(PMU_32K_CLK_A == clk_id)
    {
        pmic_print_info("soc use,can not be closed!\n");
        return BSP_PMU_ERROR;
    }

    bit_offset = hi6551_32k_clk_offset[clk_id];     /*BIT偏移*/

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_ONOFF8_OFFSET, &regval);
    regval &= ~(u8)((u32)0x1 << bit_offset);
    bsp_hi6551_reg_write(HI6551_ONOFF8_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);

    return BSP_PMU_OK;
}
/*****************************************************************************
 函 数 名  : hi6551_32k_clk_is_enabled
 功能描述  : 查询对应路32k时钟是否开启
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
int hi6551_32k_clk_is_enabled(pmu_clk_e clk_id)
{
    u8 regval = 0;
    u8 bit_offset;

    if(hi6551_32k_para_check(clk_id))
        return BSP_PMU_ERROR;

    bit_offset = hi6551_32k_clk_offset[clk_id];     /*BIT偏移*/
    bsp_hi6551_reg_read(HI6551_ONOFF8_OFFSET, &regval);

    return (int)(regval & ((u32)0x1<<bit_offset));
}

/*-------------------------纽扣电池模块接口-----------------------------------------------*/
void hi6551_backbat_charge_enabled(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_NP_REG_CHG_OFFSET, &regval);
    regval |= ((u32)0x1 << 2);
    bsp_hi6551_reg_write(HI6551_NP_REG_CHG_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
void hi6551_backbat_charge_disabled(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_NP_REG_CHG_OFFSET, &regval);
    regval &= ~((u32)0x1 << 2);
    bsp_hi6551_reg_write(HI6551_NP_REG_CHG_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
/*单位mv*/
void hi6551_backbat_charge_volt_set(u32 volt)
{
    int i = 0;
    hi6551_commflags_t comm_flag = 0;

    for(i=0; i<3; i++)
    {
        if(hi6551_backbat_volt[i] == volt)
            break;
    }

    if(i >= 3)
        /*没有复合要求的电压*/
        return ;
    else
    {
        /*核内互斥*/
        spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
        bsp_hi6551_reg_write_mask(HI6551_NP_REG_CHG_OFFSET,(u8)i,0x3);
        spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
    }

}

/*----------------------------------长按键关机模块---------------------------------------*/
void hi6551_long_powerkey_time_set(u32 time)
{
    hi6551_commflags_t comm_flag = 0;

    if((6 > time) ||(11 < time))
    {
        /*no this time*/
            return;
    }
    else
    {
        /*核内互斥*/
        spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
        bsp_hi6551_reg_write_mask(HI6551_NP_REG_ADJ_OFFSET,(u8)(time - 6),0x7);
        spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
    }
}

void hi6551_long_powerkey_off_mask(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_NP_REG_ADJ_OFFSET, &regval);
    regval |= (0x1 << 3);
    bsp_hi6551_reg_write(HI6551_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
void hi6551_long_powerkey_off_unmask(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_NP_REG_ADJ_OFFSET, &regval);
    regval &= ~(0x1 << 3);
    bsp_hi6551_reg_write(HI6551_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
/*****************************************************************************
 函 数 名  : bsp_hi6551_usb_state_get
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
 函 数 名  : bsp_hi6551_usb_state_get
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
 函 数 名  : hi6551_sim_deb_time_set
 功能描述  : 设置SIM卡中断去抖时间，单位是us。
             范围是(120,600),step:30;
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
int hi6551_sim_deb_time_set(u32 uctime)
{
    hi6551_commflags_t comm_flag = 0;
    u32 time = 0;

    /* 参数合法性检查 */
    if((120 > uctime) && (600 < uctime))
    {
        pmic_print_error("sim debtime can not be set %d us,it's range is from 120 to 600 us,!\n", (s32)uctime);
        return BSP_PMU_ERROR;
    }

    /* 寻找可配置值 */
    for(time = 120; time < 600; time += 30)
    {
        if ((uctime >= time) && (uctime < time + 30))
        {
            break;
        }
    }

    pmic_print_info("sim debtime is set at %d us!\n",(int)time);
    
    /* 实际时间转换为寄存器值 */
    time = time / 30 - 1;   /* 实际时间和寄存器的值存在转换关系: 30 * (reg_val + 1) = time */     
    
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_write_mask(HI6551_SIM_DEB_OFFSET, time, 0x1f);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
    return BSP_PMU_OK;

}
void hi6551_ldo22_res_enable(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_LDO22_REG_ADJ_OFFSET, &regval);

    /* 210/220设为b'11，配成最大电阻值；230/240设为b'00，配成1.5k */
    if((BSP_PMU_V210 == bsp_pmu_version_get()) || (BSP_PMU_V220 == bsp_pmu_version_get()))
    {
        regval |= (0x3 << 4);
    }
    else /* BSP_PMU_V230/BSP_PMU_V240 */
    {
        regval &= ~(0x3 << 4);
    }
    
    bsp_hi6551_reg_write(HI6551_LDO22_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
void hi6551_ldo22_res_disable(void)
{
    hi6551_commflags_t comm_flag = 0;
    u8 regval = 0;

    /*去使能 bit5 4-10*/
    /*核内互斥*/
    spin_lock_irqsave(&g_hi6551_comm_st.lock,comm_flag);
    bsp_hi6551_reg_read(HI6551_LDO22_REG_ADJ_OFFSET, &regval);
    regval &= ~(0x1 << 4);
    regval |= (0x1 << 5);
    bsp_hi6551_reg_write(HI6551_LDO22_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6551_comm_st.lock,comm_flag);
}
#endif
#if defined(__CMSIS_RTOS)
#define PASTAR_VCC PMIC_HI6551_LVS05
/*****************************************************************************
 函 数 名  : hi6551_pastar_enable
 功能描述  : 开启pastar电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
void hi6551_pastar_enable(void)
{
    bsp_hi6551_volt_enable(PASTAR_VCC);
}
/*****************************************************************************
 函 数 名  : hi6551_pastar_disable
 功能描述  : 关闭pastar电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
void hi6551_pastar_disable(void)
{
    bsp_hi6551_volt_disable(PASTAR_VCC);
}
#endif
/*****************************************************************************
 函 数 名  : hi6451_version_get
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
 函 数 名  : bsp_hi6551_common_init
 功能描述  : PMIC HI6551 common模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 初始化app互斥信号量
*****************************************************************************/
void bsp_hi6551_common_init(void)
{
    spin_lock_init(&g_hi6551_comm_st.lock);/*中断只在Acore实现，多core*/
}


