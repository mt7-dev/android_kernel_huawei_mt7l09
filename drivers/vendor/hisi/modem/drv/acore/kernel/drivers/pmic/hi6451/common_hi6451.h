#ifndef __COMMON_HI6451_H
#define __COMMON_HI6451_H

#include "osl_types.h"
#include "bsp_pmu.h"

#define PMU_HI6451_POWER_KEY_MASK         (1<<4)   /* Bit 4 for Power key */
#define PMU_HI6451_USB_STATE_MASK            (1<<2)
#define PMU_HI6451_HRESET_STATE_MASK            (1<<1)

typedef unsigned long hi6451_commflags_t;
/*º¯ÊýÉùÃ÷,need move */
#if defined(__KERNEL__)
int hi6451_32k_clk_enable(pmu_clk_e clk_id);
int hi6451_32k_clk_disable(pmu_clk_e clk_id);
int hi6451_32k_clk_is_enabled(pmu_clk_e clk_id);
bool hi6451_power_key_state_get(void);
bool hi6451_usb_state_get(void);
#endif

u8  hi6451_version_get(void);
void bsp_hi6451_common_init(void);

#endif
