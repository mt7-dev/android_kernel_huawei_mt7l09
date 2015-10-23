

/*lint --e{537,958}*/
#include <osl_bio.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#if defined(__KERNEL__)
#include <osl_irq.h>
#include <osl_types.h>
#endif
#include <hi_bbstar.h>
#include <bsp_ipc.h>
#if defined(__KERNEL__)
#include <bsp_pmu.h>
#endif
#include "pmu_balong.h"
#include "common_hi6559.h"

struct hi6559_common_data{
    spinlock_t      lock;
};

struct hi6559_common_data g_hi6559_comm_st;

#if defined(__KERNEL__)

/* 32K时钟使能bit在寄存器内部的偏移 */
static u8 hi6559_32k_clk_offset[PMU_32K_CLK_MAX] = {7,6};
static u32 hi6559_backbat_volt[PMU_HI6559_BACKBAT_CHARGER_VOLT_NUM] ={2500,3000,3300};

/*****************************************************************************
* 函 数 名  : hi6559_32k_para_check
*
* 功能描述  : 检查32K CLK ID是否合法
*
* 输入参数  : @clk_id：待检查的CLK_ID
*
* 输出参数  : 无
*
* 返 回 值  : BSP_PMU_OK:合法;   BSP_PMU_ERROR:不合法
*****************************************************************************/
static __inline__ s32 hi6559_32k_para_check(pmu_clk_e clk_id)
{
    if(PMU_32K_CLK_MAX <= clk_id || PMU_32K_CLK_A > clk_id)
    {
        return BSP_PMU_ERROR;
    }
    else
    {
        return BSP_PMU_OK;
    }
}

/*****************************************************************************
 函 数 名  : hi6559_32k_clk_enable
 功能描述  : 开启对应路32k时钟
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : BSP_PMU_OK: 成功；else:失败
*****************************************************************************/
s32 hi6559_32k_clk_enable(pmu_clk_e clk_id)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    if(hi6559_32k_para_check(clk_id))
    {
        return BSP_PMU_ERROR;
    }
    
    /* 核内互斥 */
    /* lint --e{746,718} */
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_read(HI6559_ONOFF8_OFFSET, &regval);
    regval |= (u8)((u32)0x1 << hi6559_32k_clk_offset[clk_id]);
    bsp_hi6559_reg_write(HI6559_ONOFF8_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return BSP_PMU_OK;
}

/*****************************************************************************
 函 数 名  : hi6559_32k_clk_disable
 功能描述  : 关闭对应路32k时钟
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : 操作成功或失败
*****************************************************************************/
s32 hi6559_32k_clk_disable(pmu_clk_e clk_id)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    if(hi6559_32k_para_check(clk_id))
    {
        return BSP_PMU_ERROR;
    }

    if(PMU_32K_CLK_A == clk_id)
    {
        pmic_print_info("soc use,can not be closed!\n");
        return BSP_PMU_ERROR;
    }

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock,comm_flag);
    bsp_hi6559_reg_read(HI6559_ONOFF8_OFFSET, &regval);
    regval &= ~(u8)((u32)0x1 << hi6559_32k_clk_offset[clk_id]);
    bsp_hi6559_reg_write(HI6559_ONOFF8_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock,comm_flag);

    return BSP_PMU_OK;
}

/*****************************************************************************
 函 数 名  : hi6559_32k_clk_is_enabled
 功能描述  : 查询对应路32k时钟是否开启
 输入参数  : pmu 32k 时钟枚举值
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
s32 hi6559_32k_clk_is_enabled(pmu_clk_e clk_id)
{
    u8 regval = 0;

    if(hi6559_32k_para_check(clk_id))
    {
        return BSP_PMU_ERROR;
    }

    bsp_hi6559_reg_read(HI6559_ONOFF8_OFFSET, &regval);

    return (int)(regval & ((u32)0x1 << hi6559_32k_clk_offset[clk_id]));
}

/*****************************************************************************
 函 数 名  : hi6559_backbat_charge_enabled
 功能描述  : 备用电池充电使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_backbat_charge_enabled(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock,comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_CHG_OFFSET, &regval);
    regval |= ((u32)0x1 << PMU_HI6559_BACKBAT_CHARGER_ENABLE_OFFSET);
    bsp_hi6559_reg_write(HI6559_NP_REG_CHG_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock,comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_backbat_charge_disabled
 功能描述  : 备用电池充电禁止
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_backbat_charge_disabled(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock,comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_CHG_OFFSET, &regval);
    regval &= ~((u32)0x1 << PMU_HI6559_BACKBAT_CHARGER_ENABLE_OFFSET);
    bsp_hi6559_reg_write(HI6559_NP_REG_CHG_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock,comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_backbat_charge_volt_set
 功能描述  : 备用电池充电电压设置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_backbat_charge_volt_set(u32 volt)
{
    hi6559_commflags_t comm_flag = 0;
    s32 i = 0;

    for(i = 0; i < PMU_HI6559_BACKBAT_CHARGER_VOLT_NUM; i++)
    {
        if(hi6559_backbat_volt[i] == volt)
        {
            break;
        }
    }

    if(i >= PMU_HI6559_BACKBAT_CHARGER_VOLT_NUM)
    {
        /*没有复合要求的电压*/
        return ;
    }
    else
    {
        /*核内互斥*/
        spin_lock_irqsave(&g_hi6559_comm_st.lock,comm_flag);
        bsp_hi6559_reg_write_mask(HI6559_NP_REG_CHG_OFFSET, (u8)i, 0x3);
        spin_unlock_irqrestore(&g_hi6559_comm_st.lock,comm_flag);

        return;
    }
}

/*****************************************************************************
 函 数 名  : hi6559_long_powerkey_time_set
 功能描述  : 长按键时间设置
 输入参数  : @time:要设置的时间
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_long_powerkey_time_set(u32 time)
{
    hi6559_commflags_t comm_flag = 0;

    if((PMU_HI6559_LONGPOWERKEY_TIME_MIN > time) || (PMU_HI6559_LONGPOWERKEY_TIME_MAX < time))
    {
        /* unsupport time */
        return;
    }
    else
    {
        /* 核内互斥 */
        spin_lock_irqsave(&g_hi6559_comm_st.lock,comm_flag);

        /* 这里的time-6是因为实际时间和寄存器值之间的差值为6 */
        bsp_hi6559_reg_write_mask(HI6559_NP_REG_ADJ_OFFSET, (u8)(time - 6), PMU_HI6559_LONGPOWERKEY_TIME_MASK);
        spin_unlock_irqrestore(&g_hi6559_comm_st.lock,comm_flag);

        return;
    }
}

/*****************************************************************************
 函 数 名  : hi6559_long_powerkey_off_mask
 功能描述  : 屏蔽长按键关机功能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_long_powerkey_off_mask(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_ADJ_OFFSET, &regval);
    regval |= (0x1 << PMU_HI6559_LONGPOWERKEY_OFF_MASK_OFS);
    bsp_hi6559_reg_write(HI6559_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_long_powerkey_off_unmask
 功能描述  : 不屏蔽长按键关机功能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_long_powerkey_off_unmask(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_ADJ_OFFSET, &regval);
    regval &= ~(0x1 << PMU_HI6559_LONGPOWERKEY_OFF_MASK_OFS);
    bsp_hi6559_reg_write(HI6559_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_long_powerkey_reboot_mask
 功能描述  : 屏蔽长按键关机重启功能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_long_powerkey_reboot_mask(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_ADJ_OFFSET, &regval);
    regval |= (0x1 << PMU_HI6559_LONGPOWERKEY_REBOOT_MASK_OFS);
    bsp_hi6559_reg_write(HI6559_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_long_powerkey_reboot_unmask
 功能描述  : 不屏蔽长按键关机重启功能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_long_powerkey_reboot_unmask(void)
{
    hi6559_commflags_t comm_flag = 0;
    u8 regval = 0;

    /* 核内互斥 */
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_read(HI6559_NP_REG_ADJ_OFFSET, &regval);
    regval &= ~(0x1 << PMU_HI6559_LONGPOWERKEY_REBOOT_MASK_OFS);
    bsp_hi6559_reg_write(HI6559_NP_REG_ADJ_OFFSET, regval);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_power_key_state_get
 功能描述  : 获取按键状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : 1 - 按下；0 - 没有按下
*****************************************************************************/
bool hi6559_power_key_state_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_STATUS1_OFFSET, &regval);
    
    return  (regval & PMU_HI6559_POWER_KEY_MASK) ? 1: 0;
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_usb_state_get
 功能描述  : 获取usb状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : 1 - 插入；0 - 拔出
*****************************************************************************/
bool hi6559_usb_state_get(void)
{
    u8 regval = 0;

    bsp_hi6559_reg_read(HI6559_STATUS2_OFFSET, &regval);
    return  (regval & PMU_HI6559_USB_STATE_MASK) ? 1 : 0;
}

/*****************************************************************************
 函 数 名  : hi6559_sim_deb_time_set
 功能描述  : 设置SIM卡中断去抖时间，单位是us。范围是(120,600),step:30;
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 函数说明  : uctime在(120, 600)范围内时，配置档位为不超过uctime的最大值；否则返回
*****************************************************************************/
s32 hi6559_sim_deb_time_set(u32 uctime)
{
    hi6559_commflags_t comm_flag = 0;
    u32 time = 0;

    /* 参数合法性检查 */
    if((PMU_HI6559_SIM_DEB_TIME_MIN > uctime) && (PMU_HI6559_SIM_DEB_TIME_MAX < uctime))
    {
        pmic_print_error("sim debtime can not be set %d us,it's range is from 120 to 600 us,!\n", (s32)uctime);
        return BSP_PMU_ERROR;
    }

    /* 寻找可配置值 */
    for(time = PMU_HI6559_SIM_DEB_TIME_MIN; time < PMU_HI6559_SIM_DEB_TIME_MAX; time += PMU_HI6559_SIM_DEB_TIME_STEP)
    {
        if ((uctime >= time) && (uctime < time + PMU_HI6559_SIM_DEB_TIME_STEP))
        {
            break;
        }
    }

    /* 实际时间转换为寄存器值 */
    pmic_print_info("set sim debtime %d us!\n", (s32)time);
    time = time / PMU_HI6559_SIM_DEB_TIME_STEP - 1;   /* 实际时间和寄存器的值存在转换关系: 30 * (reg_val + 1) = time */    
    
    spin_lock_irqsave(&g_hi6559_comm_st.lock, comm_flag);
    bsp_hi6559_reg_write_mask(HI6559_SIM_DEB_OFFSET, time, PMU_HI6559_SIM_DEB_SEL_MASK);
    spin_unlock_irqrestore(&g_hi6559_comm_st.lock, comm_flag);

    return BSP_PMU_OK;
}

/*****************************************************************************
 函 数 名  : hi6559_ldo22_res_enable
 功能描述  : 设置电阻
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_ldo22_res_enable(void)
{
    /* 711 ldo22 不给sd供电，此处打桩 */
    return;
}

/*****************************************************************************
 函 数 名  : hi6559_ldo22_res_enable
 功能描述  : 设置电阻
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_ldo22_res_disable(void)
{
    /* 711 ldo22 不给sd供电，此处打桩 */
    return;
}

#endif

#if defined(__CMSIS_RTOS)
#define PASTAR_VCC PMIC_HI6559_LVS09
/*****************************************************************************
 函 数 名  : hi6559_pastar_enable
 功能描述  : 开启pastar电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
void hi6559_pastar_enable(void)
{
    bsp_hi6559_volt_enable(PASTAR_VCC);
}
/*****************************************************************************
 函 数 名  : hi6559_pastar_disable
 功能描述  : 关闭pastar电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : pmu adp文件调用
*****************************************************************************/
void hi6559_pastar_disable(void)
{
    bsp_hi6559_volt_disable(PASTAR_VCC);
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
u8 hi6559_version_get(void)
{
    u8 regval = 0;
    bsp_hi6559_reg_read(HI6559_VERSION_OFFSET, &regval);
    return  regval;
}
/*****************************************************************************
 函 数 名  : bsp_hi6559_common_init
 功能描述  : PMIC HI6559 common模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  : 初始化app互斥信号量
*****************************************************************************/
void bsp_hi6559_common_init(void)
{
    spin_lock_init(&g_hi6559_comm_st.lock); /* 中断只在Acore实现，多core */
}


