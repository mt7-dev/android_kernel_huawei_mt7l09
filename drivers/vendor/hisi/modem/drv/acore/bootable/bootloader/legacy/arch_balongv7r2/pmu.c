
#include <product_config.h>
#include <pmu.h>
#include <boot/boot.h>

#ifdef CONFIG_PMIC_HI6451
#include <balongv7r2/pmic_hi6451.h>
#endif
#ifdef CONFIG_PMIC_HI6551
#include <balongv7r2/pmic_hi6551.h>
#include <balongv7r2/dr_hi6551.h>
#endif
#ifdef CONFIG_PMIC_HI6559
#include <balongv7r2/pmic_hi6559.h>
#include <balongv7r2/dr_hi6559.h>
#endif

struct pmu_adp_ops{
    int (*lcd_io_power_switch)(power_switch_e sw);
    int (*lcd_analog_power_switch)(power_switch_e sw);
    int (*lcd_analog_volt_set)(int);
    int (*lcd_bl_cur_set)(int);
    int (*lcd_bl_power_switch)(power_switch_e sw);
    int (*efuse_power_switch)(power_switch_e sw);
    int (*efuse_volt_set)(int);
    bool (*hreset_state_get)(void);
    void (*hreset_state_clear)(void);
    bool (*usb_state_get)(void);
    bool (*key_state_get)(void);
    u8 (*version_get)(void);
    int (*get_boot_state)(void);
    void (*set_by_nv)(void);
    int (*dr_bre_time_set)(dr_id_e dr_id, DR_BRE_TIME *dr_bre_time_st);
    void (*pmu_init)(void);
};
/*****************************************************************************
 函 数 名  : dummy_set_volt_dr
 功能描述  : 设置PMU电压电流源打桩函数
 输入参数  : para:参数
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int dummy_volt_switch(power_switch_e sw)
{
    sw = sw;
    pmic_print_info("no pmic macro defined or not need onoff,use dummy!");
    return  0;
}
/*****************************************************************************
 函 数 名  : dummy_set_volt_dr
 功能描述  : 设置PMU电压电流源打桩函数
 输入参数  : para:参数
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int dummy_volt_set(int para)
{
    para = para;
    pmic_print_info("no pmic macro defined,use dummy!");
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
 函 数 名  : dummy_default_set_handler
 功能描述  : 通过nv项控制默认设置的打桩函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void dummy_clear_part_state(void)
{
    pmic_print_info("pmic macro hi6551 not defined,use dummy!");
}
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
/*****************************************************************************
 函 数 名  : dummy_default_set_handler
 功能描述  : 通过nv项控制默认设置的打桩函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void dummy_set_by_nv_handler(void)
{
    pmic_print_info("pmic macro hi6551 not defined,use dummy!");
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
    dr_fla_time_st = dr_fla_time_st;
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
    dr_bre_time_st = dr_bre_time_st;
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}

/*****************************************************************************
 函 数 名  : dummy_pmu_init
 功能描述  : 初始化默认打桩函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无 
 调用函数  :
 被调函数  : LED模块
*****************************************************************************/
void dummy_pmu_init(void)
{
    pmic_print_error("no pmic macro defined,use dummy!");
    return  BSP_PMU_NO_PMIC;
}

#if defined (CONFIG_PMIC_HI6451)
struct pmu_adp_ops pmu_adp_ops = {
    .lcd_io_power_switch = dummy_volt_switch,
    .lcd_analog_power_switch= hi6451_ldo14_switch,
    .lcd_analog_volt_set = hi6451_ldo14_volt_set,
    .lcd_bl_cur_set = dummy_volt_set,
    .lcd_bl_power_switch = dummy_volt_switch,
    .efuse_power_switch= dummy_volt_switch,
    .efuse_volt_set = dummy_volt_set,
    .hreset_state_get = hi6451_hreset_state_get,
    .hreset_state_clear = dummy_clear_part_state,
    .usb_state_get = hi6451_usb_state_get,
    .key_state_get = hi6451_power_key_state_get,
    .version_get = hi6451_version_get,
    .get_boot_state = hi6451_get_boot_state,
    .set_by_nv = dummy_set_by_nv_handler,
    .dr_bre_time_set = dummy_dr_bre_time_set,
    .pmu_init = pmu_hi6451_init,
    };
#elif defined (CONFIG_PMIC_HI6551)
struct pmu_adp_ops pmu_adp_ops = {
    .lcd_io_power_switch  = hi6551_lvs4_switch,
    .lcd_analog_power_switch  = hi6551_ldo14_switch,
    .lcd_analog_volt_set = hi6551_ldo14_volt_set,
    .lcd_bl_cur_set = hi6551_lcd_bl_current_set,
    .lcd_bl_power_switch = hi6551_lcd_bl_power_switch,
    .efuse_power_switch= hi6551_ldo23_switch,
    .efuse_volt_set = hi6551_ldo23_volt_set,
    .hreset_state_get = hi6551_hreset_state_get,
    .hreset_state_clear = hi6551_hreset_state_clear,
    .usb_state_get = hi6551_usb_state_get,
    .key_state_get = hi6551_power_key_state_get,
    .version_get = hi6551_version_get,
    .get_boot_state = hi6551_get_boot_state,
    .set_by_nv = hi6551_set_by_nv,
    .dr_bre_time_set = hi6551_dr_bre_time_set,
    .pmu_init = pmu_hi6551_init,
    };
#elif defined (CONFIG_PMIC_HI6559)
struct pmu_adp_ops pmu_adp_ops = {
    .lcd_io_power_switch  = dummy_volt_switch, /* 711上BUCK6 always on，不能操作 */
    .lcd_analog_power_switch  = hi6559_ldo14_switch,
    .lcd_analog_volt_set = hi6559_ldo14_volt_set,
    .lcd_bl_cur_set = hi6559_lcd_bl_current_set,
    .lcd_bl_power_switch = hi6559_lcd_bl_power_switch,
    .efuse_power_switch= hi6559_ldo23_switch,
    .efuse_volt_set = hi6559_ldo23_volt_set,
    .hreset_state_get = hi6559_hreset_state_get,
    .hreset_state_clear = hi6559_hreset_state_clear,
    .usb_state_get = hi6559_usb_state_get,
    .key_state_get = hi6559_power_key_state_get,
    .version_get = hi6559_version_get,
    .get_boot_state = hi6559_get_boot_state,
    .set_by_nv = hi6559_set_by_nv,
    .dr_bre_time_set = hi6559_dr_bre_time_set,
    .pmu_init = pmu_hi6559_init,
    };
#else
struct pmu_adp_ops pmu_adp_ops = {
    .lcd_io_power_switch = dummy_volt_switch,
    .lcd_analog_power_switch = dummy_volt_switch,
    .lcd_analog_volt_set = dummy_volt_set,
    .lcd_bl_cur_set = dummy_volt_set,
    .lcd_bl_power_switch = dummy_volt_switch,
    .efuse_power_switch= dummy_volt_switch,
    .efuse_volt_set = dummy_volt_set,
    .hreset_state_get = dummy_get_part_state,
    .hreset_state_clear = dummy_clear_part_state,
    .usb_state_get = dummy_get_part_state,
    .key_state_get = dummy_get_part_state,
    .version_get = dummy_info_get_handler,
    .get_boot_state = dummy_get_boot_state,
    .set_by_nv = dummy_set_by_nv_handler,
    .dr_bre_time_set = dummy_dr_bre_time_set,
    .pmu_init = dummy_pmu_init,
    };
#endif
/*****************************************************************************
 函 数 名  : bsp_pmu_hreset_state_get
 功能描述  : 判断pmu是否为热启动
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu是热启动或冷启动(1:热启动；0:冷启动)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool bsp_pmu_hreset_state_get(void)
{
    return  pmu_adp_ops.hreset_state_get();
}
/*****************************************************************************
 函 数 名  : bsp_pmu_hreset_state_get
 功能描述  : 清除pmu热启动标志
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
void bsp_pmu_hreset_state_clear(void)
{
    return  pmu_adp_ops.hreset_state_clear();
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
 功能描述  : 获取按键是否按下状态
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
 函 数 名  : bsp_pmu_lcd_io_power_switch
 功能描述  : 操作lcdio的电压源(lvs04)
 输入参数  : power_switch_e sw：POWER_ON:打开；POWER_OFF:关闭；
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : 无
*****************************************************************************/
int bsp_pmu_lcd_io_power_switch(power_switch_e sw)
{
    return  pmu_adp_ops.lcd_io_power_switch(sw);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_lcd_analog_power_switch
 功能描述  : 开关lcd模拟电源(ldo14)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int bsp_pmu_lcd_analog_power_switch(power_switch_e sw)
{
    return  pmu_adp_ops.lcd_analog_power_switch(sw);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_lcd_analog_volt_set
 功能描述  : 设置lcd模拟电源(ldo14)电压
 输入参数  : int voltage:要设置的电压值，单位uV!
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int bsp_pmu_lcd_analog_volt_set(int voltage)
{
    return  pmu_adp_ops.lcd_analog_volt_set(voltage);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_lcd_bl_power_switch
 功能描述  : 开关lcd背光
 输入参数  :
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int bsp_pmu_lcd_bl_power_switch(power_switch_e sw)
{
    return  pmu_adp_ops.lcd_bl_power_switch(sw);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_lcd_analog_volt_set
 功能描述  : 设置lcd背光的电流
 输入参数  : int cur:要设置的电流值，单位uA!
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int bsp_pmu_lcd_bl_current_set(int cur)
{
    return  pmu_adp_ops.lcd_bl_cur_set(cur);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_efuse_power_switch
 功能描述  : 开关efuse电源(ldo23)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中efuse文件调用
*****************************************************************************/
int bsp_pmu_efuse_power_switch(power_switch_e sw)
{
    return  pmu_adp_ops.efuse_power_switch(sw);
}
/*****************************************************************************
 函 数 名  : bsp_pmu_efuse_volt_set
 功能描述  : 设置lcd模拟电源(ldo14)电压
 输入参数  : int voltage:要设置的电压值，单位uV!
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int bsp_pmu_efuse_volt_set(int voltage)
{
    return  pmu_adp_ops.efuse_volt_set(voltage);
}
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
    u8 version = 0;
    version = pmu_adp_ops.version_get();
    return version;
}
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
 函 数 名  : bsp_pmu_default_set
 功能描述  : PMU模块根据nv项设置配置默认电压
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot main
*****************************************************************************/
void bsp_pmu_set_by_nv(void)
{
    pmu_adp_ops.set_by_nv();
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
 函 数 名  : bsp_pmu_init
 功能描述  : pmu模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot中pmu模块初始化时调用
*****************************************************************************/
void bsp_pmu_init(void)
{
    pmu_adp_ops.pmu_init();
}

/*热启动标志测试*/
void bsp_pmu_hreset_test(void)
{
    bool state = 0;
    state = bsp_pmu_hreset_state_get();
    pmic_print_error("bsp_pmu_hreset_state is %d!",state);
    bsp_pmu_hreset_state_clear();
}