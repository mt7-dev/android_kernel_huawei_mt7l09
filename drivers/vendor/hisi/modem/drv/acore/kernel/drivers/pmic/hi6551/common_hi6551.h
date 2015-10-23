#ifndef __COMMON_HI6551_H
#define __COMMON_HI6551_H
#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */
#include "bsp_pmu.h"

typedef unsigned long hi6551_commflags_t;

#define PMU_HI6551_POWER_KEY_MASK         (1<<5)   /* Bit 5 for Power key */
#define PMU_HI6551_USB_STATE_MASK            (1<<5)
#define PMU_HI6551_HRESET_STATE_MASK            (1<<1)

/*º¯ÊýÉùÃ÷*/
#if defined(__KERNEL__)
int hi6551_32k_clk_enable(pmu_clk_e clk_id);
int hi6551_32k_clk_disable(pmu_clk_e clk_id);
int hi6551_32k_clk_is_enabled(pmu_clk_e clk_id);
bool hi6551_usb_state_get(void);
bool hi6551_power_key_state_get(void);
int hi6551_sim_deb_time_set(u32 uctime);
void hi6551_ldo22_res_enable(void);
void hi6551_ldo22_res_disable(void);
#endif
#if defined(__CMSIS_RTOS)
void hi6551_pastar_enable(void);
void hi6551_pastar_disable(void);
#endif
u8 hi6551_version_get(void);
void bsp_hi6551_common_init(void);
#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */
#endif
