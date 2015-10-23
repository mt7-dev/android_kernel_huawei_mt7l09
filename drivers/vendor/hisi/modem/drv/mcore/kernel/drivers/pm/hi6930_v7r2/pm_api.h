#ifndef __PM_API_H__
#define __PM_API_H__

#include "osl_types.h"
#include "soc_memmap.h"
#include "soc_clk.h"
#include "soc_interrupts.h"
#include <irq.h>
#include <bsp_sram.h>

#define CPUFREQ_MAX_PROFILE    4
#define CPUFREQ_MIN_PROFILE    0
#define CPUFREQ_MAX_PLL        1

#define PM_MCU_TIMER_BASE      HI_TIMER_10_REGBASE_ADDR
#define PM_MCU_TIMER_CLK       HI_TCXO_CLK
#define PM_MCU_TIMER_INT_LVL   M3_TIMER10_INT

#define CM3_DDRC_BASE_ADDR    0xE0046000
#define CM3_CRG_BASE_ADDR     0xE0045000

#define CRG_CLKEN2           (0x9000000c)
#define CRG_CLKDIS2          (0x90000010)
#define CRG_CLKEN3           (0x90000018)
#define CRG_CLKDIS3          (0x9000001c)
#define CRG_CLKEN4           (0x90000020)
#define CRG_CLKDIS4          (0x90000024)
#define CRG_SRSTEN1          (0x90000060)
#define CRG_SRSTDIS1         (0x90000064)
#define CRG_SRSTEN2          (0x9000006c)
#define CRG_SRSTDIS2         (0x90000070)

#define PWR_STAT1            (0x90000e04)

#define CM3_LP_ISO_EN        (0xE0044000)
#define CM3_LP_ISO_DIS       (0xE0044004)
#define CM3_LP_MTCMOS_EN     (0xE0044010)
#define CM3_LP_MTCMOS_DIS    (0xE0044014)
#define CM3_LP_PWRCTRL0      (0xE0044020)


#define PWR_CTRL0            (0x90000c00)
#define PWR_CTRL2            (0x90000c08)
#define PWR_CTRL4            (0x90000c10)
#define PWR_CTRL5            (0x90000c14)
#define PWR_CTRL6            (0x90000c18)
#define PWR_CTRL7            (0x90000c1c)
#define PWR_CTRL8            (0x90000c20)
#define PWR_CTRL9            (0x90000c24)
#define PWR_CTRL10           (0x90000c28)
#define PWR_CTRL11           (0x90000c2c)
#define PWR_CTRL12           (0x90000c30)
#define PWR_CTRL13           (0x90000c34)
#define PWR_CTRL14           (0x90000c38)
#define PWR_CTRL15           (0x90000c3c)
#define PWR_CTRL16           (0x90000c40)
#define PWR_CTRL17           (0x90000c44)
#define PWR_CTRL18           (0x90000c48)
#define PWR_CTRL19           (0x90000c4c)
#define PWR_STAT0            (0x90000e00)

#define MDDRC_SREFCTRL       (0x90020004)
#define MDDRC_STATUS         (0x90020000)

#define ACORE_BUCK3_ONOFF_FLAG   (((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_ACORE_ONOFF_FLAG)
#define CCORE_BUCK3_ONOFF_FLAG   (((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_CCORE_ONOFF_FLAG)

typedef struct {
	u32	timing0;
	u32 timing1;
	u32 timing2;
	u32 timing3;
    u32 zq0dr;
    u32 zq1dr;
}T_PM_DDRC;

typedef struct {
	u32	pll;
	u32 clkdiv;
}T_CPUFREQ_PROFILE;

typedef struct {
	u32	nxttiming0;
	u32 nxttiming1;
	u32 nxttiming2;
	u32 nxttiming3;
}T_CPUFREQ_DDRC_TIMING;

typedef struct {
	u32 ddr_reg_83c;
	u32 ddr_reg_abc;
	u32 ddr_reg_b3c;
	u32 ddr_reg_bbc;
	u32 ddr_reg_c3c;
}T_CPUFREQ_DDRC_CALC;

typedef struct {
	u32	dfs1ctrl1;
	u32 dfs1ctrl2;
}T_CPUFREQ_PLL_CONFIG;

extern void pm_set_wakeup_reg(u32 groupnum, u32 bitnum);
extern void pm_unset_wakeup_reg(u32 groupnum, u32 bitnum);
extern void pm_mdma9_pdown(void);
extern void pm_mdma9_pup(void);
extern void pm_appa9_pdown(void);
extern void pm_appa9_pup(void);
extern void pm_hifi_pll_close(void);
extern void pm_hifi_pll_open(void);
extern void pm_hifi_pdown(void);
extern void pm_hifi_pup(void);
extern void pm_bbe16_pdown(void);
extern void pm_bbe16_pup(void);
extern void pm_peri_usb_pdown(void);
extern void pm_peri_usb_pup(void);
extern void pm_config_init(void);
extern void timer_init(void);
extern void pm_print_wsrc(void);

extern void pm_dfs_bak(void);
extern void dfs_set_profile(u32 profile);
extern u32 dfs_get_profile(void);
extern void dfs_ddrc_calc(void);
//extern void dfs_to_max(void);
#endif