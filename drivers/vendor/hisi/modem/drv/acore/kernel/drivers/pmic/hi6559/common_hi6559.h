#ifndef __COMMON_HI6559_H
#define __COMMON_HI6559_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_pmu.h"

typedef unsigned long hi6559_commflags_t;

#define PMU_HI6559_POWER_KEY_MASK                   (1 << 5)   /* Bit 5 for Power key */
#define PMU_HI6559_USB_STATE_MASK                   (1 << 5)
#define PMU_HI6559_HRESET_STATE_MASK                (1 << 1)
#define PMU_HI6559_BACKBAT_CHARGER_ENABLE_OFFSET    2       /* 0x11F寄存器bit2 */
#define PMU_HI6559_BACKBAT_CHARGER_VOLT_NUM         3       /* 0x11F寄存器bit0~bit1 */
#define PMU_HI6559_LONGPOWERKEY_TIME_MAX            11      /* 0x11E寄存器bit2~bit0 */
#define PMU_HI6559_LONGPOWERKEY_TIME_MIN            6       /* 0x11E寄存器bit2~bit0 */
#define PMU_HI6559_LONGPOWERKEY_TIME_MASK           7       /* 0x11E寄存器bit2~bit0 */
#define PMU_HI6559_LONGPOWERKEY_OFF_MASK_OFS        3       /* 0x11E寄存器bit3 */
#define PMU_HI6559_LONGPOWERKEY_REBOOT_MASK_OFS     6       /* 0x11E寄存器bit6 */
#define PMU_HI6559_SIM_DEB_SEL_MASK                 0x1F    /* 0x43寄存器bit0~bit4 */
#define PMU_HI6559_SIM_DEB_TIME_MIN                 120     /* 0x43寄存器bit0~bit4，SIM卡去抖时间最小值，单位us */
#define PMU_HI6559_SIM_DEB_TIME_MAX                 600     /* 0x43寄存器bit0~bit4，SIM卡去抖时间最大值，单位us */
#define PMU_HI6559_SIM_DEB_TIME_STEP                30      /* SIM卡去抖时间间隔，单位us */

/* 这个宏在exc_hi6559.c/irq_hi6559.c中都用到 */
#define HI6559_NP_RECORD_REG_NUM                    10      /* 非下电状态记录寄存器数量，寄存器0x15~0x1E */
#define HI6559_NP_OCP_SCP_REG_NUM                   7       /* 非下电过温/短路记录寄存器数量，寄存器0x18~0x1E */

/*函数声明*/
#if defined(__KERNEL__)
s32  hi6559_32k_clk_enable(pmu_clk_e clk_id);
s32  hi6559_32k_clk_disable(pmu_clk_e clk_id);
s32  hi6559_32k_clk_is_enabled(pmu_clk_e clk_id);
bool hi6559_usb_state_get(void);
bool hi6559_power_key_state_get(void);
s32  hi6559_sim_deb_time_set(u32 uctime);
void hi6559_ldo22_res_enable(void);
void hi6559_ldo22_res_disable(void);
#endif

u8 hi6559_version_get(void);
void bsp_hi6559_common_init(void);

#ifdef __cplusplus
}
#endif
#endif
