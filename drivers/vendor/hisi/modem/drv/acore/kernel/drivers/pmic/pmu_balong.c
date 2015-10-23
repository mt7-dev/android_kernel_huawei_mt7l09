
#ifdef __VXWORKS__
#include <drv_dpm.h>
#include <drv_pmu.h>
#include <bsp_pmu.h>
#include <bsp_regulator.h>
#include "bsp_pmu_hi6561.h"
#endif
#include "pmu_balong.h"

#ifdef __KERNEL__
#ifdef CONFIG_PMIC_HI6451
#include "hi6451/irq_hi6451.h"
#endif
#ifdef CONFIG_PMIC_HI6551
#include "hi6551/irq_hi6551.h"
#include "hi6551/dr_hi6551.h"
#endif
#ifdef CONFIG_PMIC_HI6559
#include "hi6559/irq_hi6559.h"
#include "hi6559/dr_hi6559.h"
#endif
#endif

#ifdef CONFIG_PMIC_HI6451
#include "hi6451/common_hi6451.h"
#endif
#ifdef CONFIG_PMIC_HI6551
#include "hi6551/common_hi6551.h"
#endif
#ifdef CONFIG_PMIC_HI6559
#include "hi6559/common_hi6559.h"
#endif

struct pmu_adp_ops{
#if defined(__KERNEL__)
    int (*get_boot_state)(void);
    int (*clk_32k_enable)(pmu_clk_e clk_id);
    int (*clk_32k_disable)(pmu_clk_e clk_id);
    int (*clk_32k_is_enabled)(pmu_clk_e clk_id);
    bool (*usb_state_get)(void);
    bool (*key_state_get)(void);
    void (*irq_mask)(unsigned int irq);
    void (*irq_unmask)(unsigned int irq);
    int (*irq_is_masked)(unsigned int irq);
    int (*irq_callback_register)(unsigned int irq,pmufuncptr routine,void *data);
    int (*dr_list_current)(int dr_id, unsigned selector);
    int (*dr_set_mode)(int dr_id, dr_mode_e mode);
    dr_mode_e (*dr_get_mode)(int dr_id);
    int (*dr_fla_time_set)(DR_FLA_TIME *dr_fla_time_st);
    int (*dr_fla_time_get)(DR_FLA_TIME *dr_fla_time_st);
    int (*dr_bre_time_set)(dr_id_e dr_id, DR_BRE_TIME *dr_bre_time_st);
    int (*dr_bre_time_get)(dr_id_e dr_id, DR_BRE_TIME *dr_bre_time_st);
    unsigned (*dr_bre_time_list)(dr_id_e dr_id, dr_bre_time_e bre_time_enum ,unsigned selector);
    unsigned (*dr_bre_time_selectors_get)(dr_id_e dr_id, dr_bre_time_e bre_time_enum);
    int (*dr_start_delay_set)(dr_id_e dr_id, unsigned delay_ms);
    int (*sim_debtime_set)(u32 uctime);
    void (*ldo22_res_enable)(void);
    void (*ldo22_res_disable)(void);
#endif
#ifdef __VXWORKS__
    int (*pmu_init)(void);
    s32 (*apt_enable)(void);
    s32 (*apt_disable)(void);
    s32 (*apt_status_get)(void);
#endif
    u8 (*version_get)(void);
    int (*sim_upres_disable)(u32 sim_id);

};

/*函数声明*/
#if defined(__VXWORKS__)
u8 dummy_info_get_handler(void);
#endif

#if defined(__KERNEL__)
/*****************************************************************************
 函 数 名  : dummy_32k_clk_enable
 功能描述  : 开启、关闭、查询pmu中32k时钟打桩函数
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int dummy_32k_clk_handler(pmu_clk_e clk_id)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
/*****************************************************************************
 函 数 名  : dummy_irq_handler
 功能描述  : mask,unmask pmu中包含的模块的irq打桩函数
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void dummy_irq_mask_handler(unsigned int irq)
{
    pmic_print_error("no pmic macro defined,use dummy!");
}
/*****************************************************************************
 函 数 名  : dummy_irq_handler
 功能描述  : 查询pmu中包含模块中断是否屏蔽打桩函数
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int dummy_irq_state_handler(unsigned int irq)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_irq_callback_register
 功能描述  : 注册中断处理回调函数默认打桩函数
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 包含在pmu内部的中断模块
*****************************************************************************/
int dummy_irq_callback_register(unsigned int irq,pmufuncptr routine,void *data)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
/*****************************************************************************
 函 数 名  : dummy_get_boot_state
 功能描述  :系统启动时检查pmu寄存器状态，默认打桩函数
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  :系统可维可测
*****************************************************************************/
int dummy_get_boot_state(void)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
/*****************************************************************************
 函 数 名  : dummy_get_part_state
 功能描述  :系统启动时检查pmu内usb\热启动等状态，默认打桩函数
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  :充放电、开关机
*****************************************************************************/
bool dummy_get_part_state(void)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
/*****************************************************************************
* 函 数 名  : dummy_dr_list_current
* 功能描述  :查询某路电流源指定档位电流打桩。
* 输入参数  :dr_id:电流源id号；selector：要查询的档位；
* 输出参数  :无
* 返回值：   所查询的电流源指定档位的电流值（单位：uA）
*
*****************************************************************************/
int dummy_dr_list_current(int dr_id, unsigned selector)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
* 函 数 名  : dummy_dr_set_mode
* 功能描述  :设置某路电流源模式打桩。
* 输入参数  :dr_id:电流源id号；mode：设置的模式；
* 输出参数  :无
* 返回值：   成功或失败
*
*****************************************************************************/
int dummy_dr_set_mode(int dr_id, dr_mode_e mode)
{
    pmic_print_info("dr_id is %d,mode is %d!\n",dr_id,mode);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
* 函 数 名  : dummy_dr_get_mode
* 功能描述  :获取某路电流源模式打桩。
* 输入参数  :dr_id:电流源id号；
* 输出参数  :无
* 返回值：   电流源当前模式
*
*****************************************************************************/
dr_mode_e dummy_dr_get_mode(int dr_id)
{
    pmic_print_info("dr_id is %d!\n",dr_id);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  PMU_DRS_MODE_BUTTOM;
}
/*****************************************************************************
 函 数 名  : dummy_dr_fla_time_set
 功能描述  : DR闪烁时间配置，默认打桩函数
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
int dummy_dr_fla_time_set(DR_FLA_TIME* dr_fla_time_st)
{
    pmic_print_info("fla_on_us is %d,fla_off_us is %d!\n",\
        dr_fla_time_st->fla_on_us,dr_fla_time_st->fla_off_us);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
 函 数 名  : dummy_dr_bre_time_set
 功能描述  : DR呼吸时间配置，默认打桩函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
int dummy_dr_bre_time_set(dr_id_e dr_id, DR_BRE_TIME* dr_bre_time_st)
{
    pmic_print_info("on_ms is %d,off_ms is %d,rise_ms is %d,fall_ms is %d!\n",\
        dr_bre_time_st->bre_on_ms,dr_bre_time_st->bre_off_ms,dr_bre_time_st->bre_rise_ms,dr_bre_time_st->bre_fall_ms);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
 函 数 名  : dummy_dr_bre_time_list
 功能描述  : DR呼吸时间档位值获取，默认打桩函数
 输入参数  : void
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
unsigned dummy_dr_bre_time_list(dr_id_e dr_id, dr_bre_time_e bre_time_enum ,unsigned selector)
{
    pmic_print_info("no pmic macro defined,use dummy,dr_id %d,bre_time_enum %d,selector %d!\n",\
        dr_id,bre_time_enum,selector);
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
 函 数 名  : dummy_dr_bre_time_list
 功能描述  : DR呼吸时间档位数获取，默认打桩函数
 输入参数  : void
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
unsigned dummy_dr_bre_time_selectors_get(dr_id_e dr_id, dr_bre_time_e bre_time_enum)
{
    pmic_print_info("no pmic macro defined,use dummy,dr_id %d,bre_time_enum %d,selector %d!",dr_id,bre_time_enum);
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
 函 数 名  : dummy_dr_bre_time_set
 功能描述  : DR呼吸时间配置，默认打桩函数
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
int dummy_dr_start_delay_set(dr_id_e dr_id, unsigned delay_ms)
{
    pmic_print_info("dr_id is %d,delay_ms is %d!",dr_id,delay_ms);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
/*****************************************************************************
 函 数 名  : dummy_sim_debtime_set
 功能描述  : 设置SIM卡中断去抖时间的打桩函数
 输入参数  : para:参数
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int dummy_sim_debtime_set(u32 para)
{
    pmic_print_info("para is %d",para);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}
void dummy_no_para_handler(void)
{
    pmic_print_info("no pmic macro defined,use dummy!");
}
#endif

#if defined(__VXWORKS__)
/*****************************************************************************
 函 数 名  : dummy_pmu_init
 功能描述  : 初始化默认打桩函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无 
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
s32 dummy_pmu_init(void)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return BSP_PMU_OK;
}

#endif
/*****************************************************************************
 函 数 名  : dummy_info_get_handler
 功能描述  : 查询PMU版本号的打桩函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
u8 dummy_info_get_handler(void)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  0;
}
int dummy_sim_upres_disable(u32 para)
{
    pmic_print_info("para is %d",para);
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}

#if defined (CONFIG_PMIC_HI6451)
struct pmu_adp_ops pmu_adp_ops = {
#if defined(__KERNEL__)
    .get_boot_state = hi6451_get_boot_state,
    .clk_32k_enable = hi6451_32k_clk_enable,
    .clk_32k_disable = hi6451_32k_clk_disable,
    .clk_32k_is_enabled = hi6451_32k_clk_is_enabled,
    .usb_state_get = hi6451_usb_state_get,
    .key_state_get = hi6451_power_key_state_get,
    .irq_mask = dummy_irq_mask_handler,
    .irq_unmask = dummy_irq_mask_handler,
    .irq_is_masked = dummy_irq_state_handler,
    .irq_callback_register = hi6451_irq_callback_register,
    .dr_list_current = dummy_dr_list_current,
    .dr_set_mode = dummy_dr_set_mode,
    .dr_get_mode = dummy_dr_get_mode,
    .dr_fla_time_set = dummy_dr_fla_time_set,
    .dr_fla_time_get = dummy_dr_fla_time_set,
    .dr_bre_time_set = dummy_dr_bre_time_set,
    .dr_bre_time_get = dummy_dr_bre_time_set,
    .dr_bre_time_list = dummy_dr_bre_time_list,
    .dr_bre_time_selectors_get = dummy_dr_bre_time_selectors_get,
    .dr_start_delay_set = dummy_dr_start_delay_set,
    .sim_debtime_set = dummy_sim_debtime_set,
    .ldo22_res_enable = dummy_no_para_handler,
    .ldo22_res_disable = dummy_no_para_handler,
    .pmu_init = bsp_pmu_hi6451_init,
#endif
    .version_get = hi6451_version_get,
    .sim_upres_disable = dummy_sim_upres_disable,
#if defined(__VXWORKS__)
    .pmu_init = bsp_pmu_hi6451_init,
#endif
};

#elif defined (CONFIG_PMIC_HI6551)
struct pmu_adp_ops pmu_adp_ops = {
#if defined(__KERNEL__)
    .get_boot_state = hi6551_get_boot_state,
    .clk_32k_enable = hi6551_32k_clk_enable,
    .clk_32k_disable = hi6551_32k_clk_disable,
    .clk_32k_is_enabled = hi6551_32k_clk_is_enabled,
    .usb_state_get = hi6551_usb_state_get,
    .key_state_get = hi6551_power_key_state_get,
    .irq_mask = hi6551_irq_mask,
    .irq_unmask = hi6551_irq_unmask,
    .irq_is_masked = hi6551_irq_is_masked,
    .irq_callback_register = hi6551_irq_callback_register,
    .dr_list_current = bsp_hi6551_dr_list_current,
    .dr_set_mode = bsp_hi6551_dr_set_mode,
    .dr_get_mode = bsp_hi6551_dr_get_mode,
    .dr_fla_time_set = hi6551_dr_fla_time_set,
    .dr_fla_time_get = hi6551_dr_fla_time_get,
    .dr_bre_time_set = hi6551_dr_bre_time_set,
    .dr_bre_time_get = hi6551_dr_bre_time_get,
    .dr_bre_time_list = hi6551_dr_bre_time_list,
    .dr_bre_time_selectors_get = hi6551_dr_bre_time_selectors_get,
    .dr_start_delay_set = hi6551_dr_start_delay_set,
    .sim_debtime_set = hi6551_sim_deb_time_set,
    .ldo22_res_enable = hi6551_ldo22_res_enable,
    .ldo22_res_disable = hi6551_ldo22_res_disable,
#endif
    .version_get = hi6551_version_get,
    .sim_upres_disable = hi6551_sim_upres_disable,
#if defined(__VXWORKS__)
    .pmu_init = bsp_pmu_hi6551_init
#endif
};

#elif defined (CONFIG_PMIC_HI6559)
struct pmu_adp_ops pmu_adp_ops = {
#if defined(__KERNEL__)
    .get_boot_state = hi6559_get_boot_state,
    .clk_32k_enable = hi6559_32k_clk_enable,
    .clk_32k_disable = hi6559_32k_clk_disable,
    .clk_32k_is_enabled = hi6559_32k_clk_is_enabled,
    .usb_state_get = hi6559_usb_state_get,
    .key_state_get = hi6559_power_key_state_get,
    .irq_mask = hi6559_irq_mask,
    .irq_unmask = hi6559_irq_unmask,
    .irq_is_masked = hi6559_irq_is_masked,
    .irq_callback_register = hi6559_irq_callback_register,
    .dr_list_current = bsp_hi6559_dr_list_current,
    .dr_set_mode = bsp_hi6559_dr_set_mode,
    .dr_get_mode = bsp_hi6559_dr_get_mode,
    .dr_fla_time_set = hi6559_dr_fla_time_set,
    .dr_fla_time_get = hi6559_dr_fla_time_get,
    .dr_bre_time_set = hi6559_dr_bre_time_set,
    .dr_bre_time_get = hi6559_dr_bre_time_get,
    .dr_bre_time_list = hi6559_dr_bre_time_list,
    .dr_bre_time_selectors_get = hi6559_dr_bre_time_selectors_get,
    .dr_start_delay_set = hi6559_dr_start_delay_set,
    .sim_debtime_set = hi6559_sim_deb_time_set,
    .ldo22_res_enable = hi6559_ldo22_res_enable,
    .ldo22_res_disable = hi6559_ldo22_res_disable,
#endif
    .version_get = hi6559_version_get,
    .sim_upres_disable = hi6559_sim_upres_disable,
#if defined(__VXWORKS__)
    .pmu_init = bsp_pmu_hi6559_init,
    .apt_enable = bsp_pmu_hi6559_apt_enable,
    .apt_disable = bsp_pmu_hi6559_apt_disable,
    .apt_status_get = bsp_pmu_hi6559_apt_status_get,
#endif
};

#else
struct pmu_adp_ops pmu_adp_ops = {
#if defined(__KERNEL__)
    .get_boot_state = dummy_get_boot_state,
    .clk_32k_enable = dummy_32k_clk_handler,
    .clk_32k_disable = dummy_32k_clk_handler,
    .clk_32k_is_enabled = dummy_32k_clk_handler,
    .usb_state_get = dummy_get_part_state,
    .key_state_get = dummy_get_part_state,
    .irq_mask = dummy_irq_mask_handler,
    .irq_unmask = dummy_irq_mask_handler,
    .irq_is_masked = dummy_irq_state_handler,
    .irq_callback_register = dummy_irq_callback_register,
    .dr_list_current = dummy_dr_list_current,
    .dr_set_mode = dummy_dr_set_mode,
    .dr_get_mode = dummy_dr_get_mode,
    .dr_fla_time_set = dummy_dr_fla_time_set,
    .dr_fla_time_get = dummy_dr_fla_time_set,
    .dr_bre_time_set = dummy_dr_bre_time_set,
    .dr_bre_time_get = dummy_dr_bre_time_set,
    .dr_bre_time_list = dummy_dr_bre_time_list,
    .dr_bre_time_selectors_get = dummy_dr_bre_time_selectors_get,
    .dr_start_delay_set = dummy_dr_start_delay_set,
    .sim_debtime_set = dummy_sim_debtime_set,
    .ldo22_res_enable = dummy_no_para_handler,
    .ldo22_res_disable = dummy_no_para_handler,
#endif
    .version_get = dummy_info_get_handler,
    .sim_upres_disable = dummy_sim_upres_disable,
#if defined(__VXWORKS__)
    .pmu_init = dummy_pmu_init,
#endif
};
#endif /* #if defined (CONFIG_PMIC_HI6451) */

/*以下函数只在Acore中提供*/
#if defined(__KERNEL__)
/*****************************************************************************
 函 数 名  : bsp_pmu_get_boot_state
 功能描述  :系统启动时检查pmu寄存器状态，
                确认是否是由pmu引起的重启
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  :系统可维可测
*****************************************************************************/
int bsp_pmu_get_boot_state(void)
{
    return  pmu_adp_ops.get_boot_state();
}
/*****************************************************************************
 函 数 名  : bsp_pmu_32k_clk_enable
 功能描述  : 开启pmu中32k时钟
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  : 开启成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_pmu_32k_clk_enable(pmu_clk_e clk_id)
{
    return  pmu_adp_ops.clk_32k_enable(clk_id);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_32k_clk_disable
 功能描述  : 关闭pmu中32k时钟
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  : 关闭成功或失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_pmu_32k_clk_disable(pmu_clk_e clk_id)
{
    return  pmu_adp_ops.clk_32k_disable(clk_id);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_32k_clk_is_enabled
 功能描述  : 查询pmu中32k时钟是否开启
 输入参数  : clk_id:32k时钟编号
 输出参数  : 无
 返 回 值  : 开启或关闭
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_pmu_32k_clk_is_enabled(pmu_clk_e clk_id)
{
    return  pmu_adp_ops.clk_32k_is_enabled(clk_id);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_version_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool bsp_pmu_usb_state_get(void)
{
    return  pmu_adp_ops.usb_state_get();
}
/*****************************************************************************
 函 数 名  : bsp_pmu_key_state_get
 功能描述  : 获取power按键是否按下状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool bsp_pmu_key_state_get(void)
{
    return  pmu_adp_ops.key_state_get();
}
/*****************************************************************************
 函 数 名  : bsp_pmu_irq_callback_register
 功能描述  : 注册中断处理回调函数
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 包含在pmu内部的中断模块
*****************************************************************************/
void bsp_pmu_irq_mask(unsigned int irq)
{
    pmu_adp_ops.irq_mask(irq);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_irq_callback_register
 功能描述  : 注册中断处理回调函数
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 包含在pmu内部的中断模块
*****************************************************************************/
void bsp_pmu_irq_unmask(unsigned int irq)
{
    pmu_adp_ops.irq_unmask(irq);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_irq_callback_register
 功能描述  : 注册中断处理回调函数
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 包含在pmu内部的中断模块
*****************************************************************************/
int bsp_pmu_irq_is_masked(unsigned int irq)
{
    return  pmu_adp_ops.irq_is_masked(irq);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_irq_callback_register
 功能描述  : 注册中断处理回调函数
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 包含在pmu内部的中断模块
*****************************************************************************/
int bsp_pmu_irq_callback_register(unsigned int irq,pmufuncptr routine,void *data)
{
    return  pmu_adp_ops.irq_callback_register(irq,routine,data);
}
/*****************************************************************************
* 函 数 名  : bsp_dr_list_current
* 功能描述  :查询某路电流源指定档位电流。
* 输入参数  :dr_id:电流源id号；selector：要查询的档位；
* 输出参数  :无
* 返回值：   所查询的电流源指定档位的电流值（单位：uA）
*
*****************************************************************************/
int bsp_dr_list_current(int dr_id, unsigned selector)
{
    return  pmu_adp_ops.dr_list_current(dr_id,selector);
}
/*****************************************************************************
* 函 数 名  : bsp_pmu_dr_set_mode
* 功能描述  :设置某路电流源模式。
* 输入参数  :dr_id:电流源id号；mode：设置的模式；
* 输出参数  :无
* 返回值：   成功或失败
*
*****************************************************************************/
int bsp_dr_set_mode(int dr_id, dr_mode_e mode)
{
    return  pmu_adp_ops.dr_set_mode(dr_id,mode);
}
/*****************************************************************************
* 函 数 名  : bsp_hi6551_dr_get_mode
* 功能描述  :获取某路电流源模式。
* 输入参数  :dr_id:电流源id号；
* 输出参数  :无
* 返回值：   电流源当前模式
*
*****************************************************************************/
dr_mode_e bsp_dr_get_mode(int dr_id)
{
    return  pmu_adp_ops.dr_get_mode(dr_id);
}
/*****************************************************************************
 函 数 名  : bsp_dr_fla_time_set
 功能描述  : 设置dr的闪烁点亮时间 和 暗灭时间
 输入参数  : dr_fla_time_st:闪烁时间参数结构体;单位:us
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
*****************************************************************************/
/*呼吸时间配置dr1/2*/
int bsp_dr_fla_time_set(DR_FLA_TIME *dr_fla_time_st)
{
    return  pmu_adp_ops.dr_fla_time_set(dr_fla_time_st);
}
/*****************************************************************************
 函 数 名  : bsp_dr_fla_time_set
 功能描述  : 获取dr的闪烁点亮时间 和 暗灭时间
 输入参数  : dr_fla_time_st:闪烁时间参数结构体;单位:us
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
*****************************************************************************/
/*呼吸时间配置dr1/2*/
int bsp_dr_fla_time_get(DR_FLA_TIME *dr_fla_time_st)
{
    return  pmu_adp_ops.dr_fla_time_get(dr_fla_time_st);
}
/*****************************************************************************
 函 数 名  : bsp_dr_bre_time_set
 功能描述  : 设置dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)
 输入参数  : dr_id:要设置的电流源编号;dr_bre_time_st:呼吸时间参数结构体;
            没有找到设置的时间，就设置为长亮
 输出参数  : 无
 返 回 值  : 设置成功或者失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁
*****************************************************************************/
int bsp_dr_bre_time_set(dr_id_e dr_id, DR_BRE_TIME *dr_bre_time_st)
{
    return  pmu_adp_ops.dr_bre_time_set(dr_id,dr_bre_time_st);
}
/*****************************************************************************
 函 数 名  : bsp_dr_bre_time_list
 功能描述  : 陈列dr的呼吸时间(包括长亮，长暗、渐亮、渐暗时间)各档位的时间值
 输入参数  : dr_id:要查询的电流源编号;bre_time_enum:所要查询的时间类型;
             selector，时间档位
 输出参数  : 无
 返 回 值  : 档位时间
 调用函数  :
 备注说明  :
*****************************************************************************/
unsigned bsp_dr_bre_time_list(dr_id_e dr_id, dr_bre_time_e bre_time_enum ,unsigned selector)
{
    return  pmu_adp_ops.dr_bre_time_list(dr_id,bre_time_enum,selector);
}
/*****************************************************************************
 函 数 名  : bsp_dr_bre_time_selectors_get
 功能描述  : 获取dr 呼吸时间总共档位值
 输入参数  : dr_id:要设置的电流源编号;bre_time_enum:所要查询的时间类型;
 输出参数  : 无
 返 回 值  : 档位时间
 调用函数  :
 备注说明  :
*****************************************************************************/
unsigned bsp_dr_bre_time_selectors_get(dr_id_e dr_id, dr_bre_time_e bre_time_enum)
{
    return  pmu_adp_ops.dr_bre_time_selectors_get(dr_id,bre_time_enum);
}
/*****************************************************************************
 函 数 名  : bsp_dr_start_delay_set
 功能描述  : 设置dr的启动延时时间
 输入参数  : dr_id:要设置的电流源编号;delay_ms:启动延时的时间，单位:ms
             设置范围:[0 , 32768]ms
 输出参数  : 无
 返 回 值  : 设置成功或失败
 调用函数  :
 备注说明  : 对应寄存器只有设置时间的功能，不需要互斥锁，只有DR3/4/5有此功能
*****************************************************************************/
int bsp_dr_start_delay_set(dr_id_e dr_id, unsigned delay_ms)
{
    return  pmu_adp_ops.dr_start_delay_set(dr_id,delay_ms);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_sim_debtime_set
 功能描述  : 设置sim卡去抖时间
 输入参数  : uctime:去抖时间
 输出参数  : 无
 返 回 值  : 设置成功或失败
 调用函数  :
 备注说明  :
*****************************************************************************/
int bsp_pmu_sim_debtime_set(u32 uctime)
{
   return  pmu_adp_ops.sim_debtime_set(uctime);
}

void bsp_pmu_ldo22_res_enable(void)
{
   pmu_adp_ops.ldo22_res_enable();
}
void bsp_pmu_ldo22_res_disable(void)
{
   pmu_adp_ops.ldo22_res_disable();
}
#endif

/*以下函数只在Ccore\Mcore和FASTBOOT中提供*/
#if defined(__VXWORKS__)

/*****************************************************************************
 函 数 名  : bsp_pmu_init
 功能描述  :系统启动初始化pmu相关信号量
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :ccore系统初始化函数
*****************************************************************************/
void bsp_pmu_init(void)
{
    pmu_adp_ops.pmu_init();
}

int bsp_sim_upres_disable(u32 sim_id)
{
    return  pmu_adp_ops.sim_upres_disable(sim_id); 
}
 
/*****************************************************************************
 函数	: bsp_pmu_apt_enable
 功能	: 通信模块使能APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_apt_enable(void)
{
    return pmu_adp_ops.apt_enable();
}

/*****************************************************************************
 函数	: bsp_pmu_apt_enable
 功能	: 通信模块使能APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_apt_disable(void)
{
    return pmu_adp_ops.apt_disable();
}

/*****************************************************************************
 函数	: bsp_pmu_apt_status_get
 功能	: 通信模块获取当前APT状态接口
 输入	: 无
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 bsp_pmu_apt_status_get(void)
{
    return pmu_adp_ops.apt_status_get();
}

#endif
/*以下函数在Acore\Ccore\Mcore\FASTBOOT中均提供*/
/*****************************************************************************
 函 数 名  : bsp_pmu_version_get
 功能描述  : 获取pmu版本号
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : 集成hso，msp调用
*****************************************************************************/
u8 bsp_pmu_version_get(void)
{
    return pmu_adp_ops.version_get();
}

