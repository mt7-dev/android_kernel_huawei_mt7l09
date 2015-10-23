#ifndef __PM_API_H__
#define __PM_API_H__

#include "osl_types.h"
#include "soc_memmap.h"
#include "soc_clk.h"
#include "soc_interrupts.h"
#include <irq.h>

#define CPUFREQ_MAX_PROFILE    1
#define CPUFREQ_MIN_PROFILE    0

#define PM_MCU_TIMER_BASE      HI_TIMER_10_REGBASE_ADDR
#define PM_MCU_TIMER_CLK       HI_TCXO_CLK
#define PM_MCU_TIMER_INT_LVL   M3_TIMER10_INT

#define CRG_CTRL3        (0x9000000c)
#define CRG_CTRL4        (0x90000010)
#define CRG_CTRL9        (0x90000024)
#define CRG_CTRL10       (0x90000028)
#define CRG_CTRL14       (0x90000038)
#define CRG_CTRL15       (0x9000003C)
#define CRG_CTRL16       (0x90000040)
#define CRG_CTRL18       (0x90000048)
#define CRG_CTRL22       (0x90000058)
#define CRG_CTRL25       (0x90000064)
#define CRG_CTRL28       (0x90000070)
#define CRG_CTRL34       (0x90000088)
#define CRG_CTRL39       (0x9000009c)
#define CRG_CTRL40       (0x900000A0)

#define PWR_CTRL0        (0x90000c00)
#define PWR_CTRL2        (0x90000c08)
#define PWR_CTRL4        (0x90000c10)
#define PWR_CTRL6        (0x90000c18)
#define PWR_CTRL7        (0x90000c1c)
#define PWR_CTRL8        (0x90000c20)
#define PWR_CTRL9        (0x90000c24)
#define PWR_CTRL10       (0x90000c28)
#define PWR_CTRL11       (0x90000c2c)
#define PWR_CTRL12       (0x90000c30)
#define PWR_CTRL13       (0x90000c34)
#define PWR_CTRL15       (0x90000c3c)
#define PWR_CTRL16       (0x90000c40)
#define PWR_CTRL17       (0x90000c44)
#define PWR_CTRL18       (0x90000c48)
#define PWR_STAT0        (0x90000e00)
#define PWR_STAT1        (0x90000e04)

#define MDDRC_SREFCTRL   (0x90020004)
#define MDDRC_STATUS     (0x90020000)

typedef struct {
	u32	pll;
	u32 clkdiv;
}T_CPUFREQ_PROFILE;


extern void pm_set_wakeup_reg(u32 groupnum, u32 bitnum);
extern void pm_mdma9_pdown(void);
extern void pm_mdma9_pup(void);
extern void pm_appa9_pdown(void);
extern void pm_appa9_pup(void);
extern void pm_hifi_pdown(void);
extern void pm_hifi_pup(void);
extern void pm_bbe16_pdown(void);
extern void pm_bbe16_pup(void);
extern void pm_dsp0_pdown(void);
extern void pm_dsp0_pup(void);
extern void pm_a15_pdown(void);
extern void pm_a15_pup(void);
extern void pm_config_init(void);
extern void pm_config_init_m3on(void);
extern void pm_config_init_m3off(void);
extern void pm_mcu_bakeup_debug(void);
extern void pm_mcu_default_pdown(void);
extern void pm_mcu_default_pup(void);
extern void timer_init(void);
extern void pm_print_wsrc(void);

extern void dfs_set_profile(u32 profile);
extern u32 dfs_get_profile(void);

#endif