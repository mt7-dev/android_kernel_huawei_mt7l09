/*----------------------------------------------------------------------------
 *      balongv7r2 m3 pm
 *----------------------------------------------------------------------------
 *      Name:    pm.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of balongv7r2 PWR.
 *---------------------------------------------------------------------------*/
#include <hi_base.h>
#include <ARMCM3.h>
#include "console.h"
#include "printk.h"

#include "osl_types.h"
#include "osl_irq.h"

#include "bsp_shared_ddr.h"
#include "hi_syscrg.h"
#include "hi_syssc.h"
#include "hi_timer.h"

#include "m3_pm.h"
#include "pm_api.h"
#include "bsp_hardtimer.h"
#include "drv_edma_enum.h"
#include "edma_balong.h"
#include "dpm_balong.h"
#include "drv_nv_def.h"
#include "drv_nv_id.h"
#include "bsp_nvim.h"
#include "ios_pd.h"
#include "wdt_balong.h"

/*lint --e{40, 63, 409, 438} */
extern T_PM_ST gPmSt;
extern T_PM_BAKEUP_ST gPmBakeupSt;
extern T_PM_COUNT_ST gPmCountSt;
extern T_PM_ERROR_ST gPmErrorSt;
extern DRV_NV_PM_TYPE gPmNvSt;
extern u32 sys_initial;
extern u32 uart_delay_flag;

u32 tcxo = 0;
u32 pclk = 0;
u32 m3_record_num = 0;
u32 m3_wakeup_flag = 0;
u32 g_buck3on_txco_on = 1;

T_PM_DDRC gddrreg;

static void pm_config_init_buck3off(void);
static void pm_config_init_buck3on(void);
static void pm_mcu_bakeup_debug(void);
static void pm_mcu_wakeup_debug(void);
static void pm_mcu_process(void);
static void pm_debug_clr_timer(void);
static void pm_set_peri_down(u32 flag);
static void pm_print_wakeup_debug(void);

void pm_set_wakeup_reg(u32 groupnum, u32 bitnum)
{
	int irqlock;
	u32 tmp = 0;

	local_irq_save(irqlock);
	tmp = readl(PWR_CTRL16 + groupnum*4);
	tmp |= (u32)0x1 << bitnum;
	writel(tmp, PWR_CTRL16 + groupnum*4);
	local_irq_restore(irqlock);
}

void pm_unset_wakeup_reg(u32 groupnum, u32 bitnum)
{
	int irqlock;
	u32 tmp = 0;

	local_irq_save(irqlock);
	tmp = readl(PWR_CTRL16 + groupnum*4);
	tmp &= ~((u32)0x1 << bitnum);
	writel(tmp, PWR_CTRL16 + groupnum*4);
	local_irq_restore(irqlock);
}


/* pm */
void pm_mdma9_pdown(void)
{
	/* disable clk */
	writel((0x1<<27), CRG_CLKDIS3);

	/* iso ctrl enable */
	writel(0x2, PWR_CTRL4);

	/* reset */
	writel(0x8080, CRG_SRSTEN1);

	/* mtcmos power down */
	writel(0x2, PWR_CTRL7);
}
void pm_mdma9_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	writel(0x2, PWR_CTRL6);
	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 1) & 0x1;
	}while(!tmp);
    udelay(30);
	/* enable clk */
	writel((0x1<<27), CRG_CLKEN3);

	/* iso ctrl disable */
	writel(0x2, PWR_CTRL5);

	/* unreset */
	writel(0x8080, CRG_SRSTDIS1);
}

void pm_appa9_pdown(void)
{
	/* disable clk */
	writel((0x1<<26), CRG_CLKDIS3);

	/* iso ctrl enable */
	writel(0x1, PWR_CTRL4);

	/* reset */
	writel(0x4002, CRG_SRSTEN1);

	/* mtcmos power down */
	writel(0x1, PWR_CTRL7);
}
void pm_appa9_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	writel(0x1, PWR_CTRL6);
	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 0) & 0x1;
	}while(!tmp);
    udelay(30);

	/* enable clk */
	writel((0x1<<26), CRG_CLKEN3);

	/* iso ctrl disable */
	writel(0x1, PWR_CTRL5);

	/* unreset */
	writel(0x4002, CRG_SRSTDIS1);
}

void pm_hifi_pll_close(void)
{
    u32 tmp = 0;

	tmp = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_DFS3_CTRL3_OFFSET);
	tmp |= 0x1;
	writel(tmp, HI_SYSCRG_BASE_ADDR + HI_CRG_DFS3_CTRL3_OFFSET);
}

void pm_hifi_pll_open(void)
{
    u32 tmp = 0;

	tmp = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_DFS3_CTRL3_OFFSET);
	tmp &= ~0x1;
	writel(tmp, HI_SYSCRG_BASE_ADDR + HI_CRG_DFS3_CTRL3_OFFSET);

    do{
		tmp = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_DFS3_CTRL3_OFFSET);
		tmp &= ((u32)0x1<<31);
	}while(!tmp);
}

void pm_hifi_pdown(void)
{
	/* disable clk */
	writel((u32)0x1<<31, CRG_CLKDIS3);

	/* iso ctrl enable */
	writel(0x20, PWR_CTRL4);

	/* reset */
	writel((0x3<<19), CRG_SRSTEN2);

	/* mtcmos power down */
	writel(0x20, PWR_CTRL7);
}
void pm_hifi_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	writel(0x20, PWR_CTRL6);
	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 5) & 0x1;
	}while(!tmp);

	/* unreset */
	writel((0x3<<19), CRG_SRSTDIS2);

	/* iso ctrl disable */
	writel(0x20, PWR_CTRL5);

	/* enable clk */
	writel((u32)0x1<<31, CRG_CLKEN3);
}

void pm_bbe16_pdown(void)
{
	/* disable clk */
	writel(0x7, CRG_CLKDIS4);

	/* iso ctrl enable */
	writel(0x10, PWR_CTRL4);

	/* reset */
	writel(0x7, CRG_SRSTEN2);

	/* mtcmos power down */
	writel(0x10, PWR_CTRL7);
}
void pm_bbe16_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	writel(0x10, PWR_CTRL6);
	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 4) & 0x1;
	}while(!tmp);

	/* unreset */
	writel(0x7, CRG_SRSTDIS2);

	/* iso ctrl disable */
	writel(0x10, PWR_CTRL5);

	/* enable clk */
	writel(0x7, CRG_CLKEN4);
}

void pm_peri_usb_pdown(void)
{
	/* disable clk */
	writel(0x1<<12, CRG_CLKDIS2);

	/* iso ctrl enable */
	writel(0x1000, PWR_CTRL4);

	/* reset */
	writel(0x1<<5, CRG_SRSTEN2);

	/* mtcmos power down */
	writel(0x1<<12, PWR_CTRL7);
}
void pm_peri_usb_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	writel(0x1<<12, PWR_CTRL6);
	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 12) & 0x1;
	}while(!tmp);

	/* unreset */
	writel(0x1<<5, CRG_SRSTDIS2);

	/* iso ctrl disable */
	writel(0x1<<12, PWR_CTRL5);

	/* enable clk */
	writel(0x1<<12, CRG_CLKEN2);
}

static void pm_mddrc_bak()
{
    gddrreg.timing0 = readl(HI_MDDRC_REGBASE_ADDR + 0x50);
    gddrreg.timing1 = readl(HI_MDDRC_REGBASE_ADDR + 0x54);
    gddrreg.timing2 = readl(HI_MDDRC_REGBASE_ADDR + 0x58);
    gddrreg.timing3 = readl(HI_MDDRC_REGBASE_ADDR + 0x5c);
    gddrreg.zq0dr = readl(HI_MDDRC_REGBASE_ADDR + 0xa48);
    gddrreg.zq1dr = readl(HI_MDDRC_REGBASE_ADDR + 0xa58);
}

static void pm_mddrc_restore()
{
	/* Base Configuration of MDDRC */
	writel(0x1, HI_MDDRC_REGBASE_ADDR + 0x10);
	writel(0x01e00210, HI_MDDRC_REGBASE_ADDR + 0x1c);
	writel(0x00620000, HI_MDDRC_REGBASE_ADDR + 0x14);
	writel(0x00010004, HI_MDDRC_REGBASE_ADDR + 0x18);
	writel(0x30007300, HI_MDDRC_REGBASE_ADDR + 0x20);
	writel(0x131, HI_MDDRC_REGBASE_ADDR + 0x2c);
	writel(0x50000000, HI_MDDRC_REGBASE_ADDR + 0x40);
	writel(gddrreg.timing0, HI_MDDRC_REGBASE_ADDR + 0x50);
	writel(gddrreg.timing1 , HI_MDDRC_REGBASE_ADDR + 0x54);
	writel(gddrreg.timing3, HI_MDDRC_REGBASE_ADDR + 0x5c);
	writel(0x0, HI_MDDRC_REGBASE_ADDR + 0xf4);
	writel(gddrreg.timing2&0xFFFFF800, HI_MDDRC_REGBASE_ADDR + 0x58);
	writel(0x0f000501, HI_MDDRC_REGBASE_ADDR + 0xac);

	/* Configuration of PUB for DDR PHY ready for initilization */
	writel(0x80000000, HI_MDDRC_REGBASE_ADDR + 0x804);
	while((readl(HI_MDDRC_REGBASE_ADDR+0x818) & 0x1) != 0x1)
	{};
#ifdef BSP_CONFIG_NOT_DDR_BYPASSPLL
	writel(0x001b8000, HI_MDDRC_REGBASE_ADDR + 0x820);
#else
	writel(0xe01b8000, HI_MDDRC_REGBASE_ADDR + 0x820);
#endif

	writel(0x408, HI_MDDRC_REGBASE_ADDR + 0x888);
	writel(0x07008580, HI_MDDRC_REGBASE_ADDR + 0x80c);
	writel(0x04000085, HI_MDDRC_REGBASE_ADDR + 0xa80);
	writel(0x04000085, HI_MDDRC_REGBASE_ADDR + 0xb00);
	writel(0x04000085, HI_MDDRC_REGBASE_ADDR + 0xb80);
	writel(0x04000085, HI_MDDRC_REGBASE_ADDR + 0xc00);
	writel(0x04000084, HI_MDDRC_REGBASE_ADDR + 0xc80);
	writel(0x04000084, HI_MDDRC_REGBASE_ADDR + 0xd00);
	writel(0x04000084, HI_MDDRC_REGBASE_ADDR + 0xd80);
	writel(0x04000084, HI_MDDRC_REGBASE_ADDR + 0xe00);
	writel(0x04000084, HI_MDDRC_REGBASE_ADDR + 0xe80);

	writel(0x01001586, HI_MDDRC_REGBASE_ADDR + 0x8b0);
	writel(0x006c64fe, HI_MDDRC_REGBASE_ADDR + 0x884);

	/*PUB_DX0GTR*/
	writel(0x00055002, HI_MDDRC_REGBASE_ADDR + 0xac8);
	writel(0x00055002, HI_MDDRC_REGBASE_ADDR + 0xb48);
	writel(0x00055002, HI_MDDRC_REGBASE_ADDR + 0xbc8);
	writel(0x00055002, HI_MDDRC_REGBASE_ADDR + 0xc48);

	/* Configuration of PUB about Timing */
	writel(0x20010010, HI_MDDRC_REGBASE_ADDR + 0x824);
	writel(0x19000900, HI_MDDRC_REGBASE_ADDR + 0x828);
	writel(0x0321046b, HI_MDDRC_REGBASE_ADDR + 0x830);
	writel(0x05380e53, HI_MDDRC_REGBASE_ADDR + 0x834);
	writel(0x5d0e8943, HI_MDDRC_REGBASE_ADDR + 0x88c);
	writel(0x22816295, HI_MDDRC_REGBASE_ADDR + 0x890);
	writel(0x10030c32, HI_MDDRC_REGBASE_ADDR + 0x894);
	writel(0x00000021, HI_MDDRC_REGBASE_ADDR + 0x898);
	writel(0x00000062, HI_MDDRC_REGBASE_ADDR + 0x8a0);
	writel(0x00000004, HI_MDDRC_REGBASE_ADDR + 0x8a4);
	writel(0x00000001, HI_MDDRC_REGBASE_ADDR + 0x8a8);
	writel(0x00181884, HI_MDDRC_REGBASE_ADDR + 0x880);
	writel(0x0000000c, HI_MDDRC_REGBASE_ADDR + 0xa44);

	writel(0x0000000d, HI_MDDRC_REGBASE_ADDR + 0xa54);/*PUB_ZQ1PR, for DX parts*/

	writel(0x0000aaaa, HI_MDDRC_REGBASE_ADDR + 0xa88);
	writel(0x0000aaaa, HI_MDDRC_REGBASE_ADDR + 0xb08);
	writel(0x0000aaaa, HI_MDDRC_REGBASE_ADDR + 0xb88);
	writel(0x0000aaaa, HI_MDDRC_REGBASE_ADDR + 0xc08);
	writel(0x00002000, HI_MDDRC_REGBASE_ADDR + 0xa8c);
	writel(0x00002000, HI_MDDRC_REGBASE_ADDR + 0xb0c);
	writel(0x00002000, HI_MDDRC_REGBASE_ADDR + 0xb8c);
	writel(0x00002000, HI_MDDRC_REGBASE_ADDR + 0xc0c);

	/* Start initilization */
#ifdef BSP_CONFIG_NOT_DDR_BYPASSPLL
	writel(0x000000f3, HI_MDDRC_REGBASE_ADDR + 0x804);
#else
	writel(0x000600e1, HI_MDDRC_REGBASE_ADDR + 0x804);
#endif
	while((readl(HI_MDDRC_REGBASE_ADDR+0x818) & 0x00000001))
	{
	};
#ifdef BSP_CONFIG_NOT_DDR_BYPASSPLL
	while(!((readl(HI_MDDRC_REGBASE_ADDR+0x818) & 0x80000001) == 0x80000001))
	{};
#else
	while(!((readl(HI_MDDRC_REGBASE_ADDR+0x818) & 0x0000000F) == 0x0000000F))
	{};
#endif
	writel(gddrreg.timing2, HI_MDDRC_REGBASE_ADDR + 0x58);
	writel(0x30107700, HI_MDDRC_REGBASE_ADDR + 0x20);
	writel(0x0, HI_MDDRC_REGBASE_ADDR + 0xf8);

	writel(0x00058906, HI_MDDRC_REGBASE_ADDR + 0xa40);/*power down ZQ w00*/
	writel(0x00002008, HI_MDDRC_REGBASE_ADDR + 0xa8c);
	writel(0x00002008, HI_MDDRC_REGBASE_ADDR + 0xb0c);
	writel(0x00002008, HI_MDDRC_REGBASE_ADDR + 0xb8c);
	writel(0x00002008, HI_MDDRC_REGBASE_ADDR + 0xc0c);
	writel(0x01e70210, HI_MDDRC_REGBASE_ADDR + 0x1c);
	writel(0x0, HI_MDDRC_REGBASE_ADDR + 0x8e4);

    writel(gddrreg.zq0dr, HI_MDDRC_REGBASE_ADDR + 0xa48);
    writel(gddrreg.zq1dr, HI_MDDRC_REGBASE_ADDR + 0xa58);
}

static void pm_ddr_in_sref()
{
    u32 tmp = 0;
    u32 times = 20;

    writel(0x1, MDDRC_SREFCTRL);
	do{
		tmp = readl(MDDRC_STATUS);
		tmp &= 0x4;
	}while(!tmp);

    while(times)
    {
        times--;
    }
}

static void pm_ddr_out_sref()
{
    u32 tmp = 0;
    u32 times = 20;

    writel(0x0, MDDRC_SREFCTRL);
	do{
	tmp = readl(MDDRC_STATUS);
	tmp &= 0x4;
	}while(tmp);

    while(times)
    {
        times--;
	}
}

static void pm_set_bbpwakeup_flag(void)
{
    u32 tmp = 0;

    tmp = (NVIC->ICPR[0]) & (NVIC->ISER[0]);
    tmp = tmp&0x100;
    if(tmp)
    {
        writel(1, SHM_TIMESTAMP_ADDR);
    }
    else
    {
        writel(0, SHM_TIMESTAMP_ADDR);
    }

    writel(readl(HI_SYSCRG_BASE_ADDR + 0x8), PWR_SRAM_CRG_REG);
    writel(readl(HI_SYSCRG_BASE_ADDR + 0x14), PWR_SRAM_CRG_REG + 0x4);
    writel(readl(HI_SYSCRG_BASE_ADDR + 0x20), PWR_SRAM_CRG_REG + 0x8);
    writel(readl(HI_SYSCRG_BASE_ADDR + 0x2c), PWR_SRAM_CRG_REG + 0xC);
    writel(readl(HI_SYSCRG_BASE_ADDR + 0x38), PWR_SRAM_CRG_REG + 0x10);
    writel(readl(HI_SYSCRG_BASE_ADDR + 0xe04), PWR_SRAM_CRG_REG + 0x14);
}

////////////////////////////////////
void pm_config_init(void)
{
    /* wakeup src */
	writel(0x0, PWR_CTRL15);
	writel(0x0, PWR_CTRL16);
	writel(0x0, PWR_CTRL17);
	writel(0x0, PWR_CTRL18);
	writel(0x0, PWR_CTRL19);
}
static void pm_config_init_buck3off(void)
{
	u32 tmp = 0;

	/* 1 */
	tmp = readl(PWR_CTRL0);
	tmp |= 0x2c00400;
	writel(tmp, PWR_CTRL0);

	/* 2 */
	writel(0x1FFF0FFF, PWR_CTRL8);

	/* 3 tcxo*/
    tmp = readl(PWR_CTRL2);
    tmp &= 0xA00000;
   	tmp = tmp | 0x1404FFCD;
    writel(tmp, PWR_CTRL2);

	/* 4 */
	writel(0x2089B, PWR_CTRL12);
	writel(0x1FFFFFF, PWR_CTRL10);
	writel(0x1FFFFFF, PWR_CTRL11);

	/* 5 */
	writel(0x10FE1, PWR_CTRL13);

	/* 6 peri power down */
	writel(0x20000000, PWR_CTRL9);
}

static void pm_config_init_buck3on(void)
{
	u32 tmp = 0;

	/* 1 */
	tmp = readl(PWR_CTRL0);
	tmp |= 0x2c00400;
	writel(tmp, PWR_CTRL0);

	/* 2 */
	writel(0x1FFF0FFF, PWR_CTRL8);

	/* 3 tcxo*/
	tmp = readl(PWR_CTRL2);
    tmp &= 0xA00000;
	if(1==g_buck3on_txco_on)
	{
		tmp = tmp | 0x1400FFFF;
	}
	else
	{
		tmp = tmp | 0x1404FFCD;
	}
    writel(tmp, PWR_CTRL2);

	/* 4 */
	writel(0x2089B, PWR_CTRL12);
	writel(0x1FFFFFF, PWR_CTRL10);
	writel(0x1FFFFFF, PWR_CTRL11);

	/* 5 */
	writel(0x0, PWR_CTRL13);

	/* 6 peri power down */
	if(1 == gPmNvSt.peridown_flag)
	{
		writel(0x20000000, PWR_CTRL9);
	}
}
/*lint -save -e737*/
void pm_mcu_buck3off_pdown(void)
{
	u32 tmp = 0;
	u32 i = 0;

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER1);

	for(i = 0; i < 8; i++)
	{
		gPmBakeupSt.u32NvicEnable[i] = NVIC->ISER[i];
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	/* ªΩ–—‘¥Œ¥≈‰÷√ */
	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32AcoreWsrc[i]|gPmSt.u32CcoreWsrc[i]|gPmSt.u32McoreWsrc[i]|gPmSt.u32HiFiWsrc[i];
		NVIC->ISER[i] = tmp;
	}

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER2);
    ios_suspend();

	/* 3 ddr */
    pm_mddrc_bak();
	pm_ddr_in_sref();

	/* 4 ddrio and normal io retention */
	writel((0xF1<<22), PWR_CTRL4);
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER3);

    if(gPmCountSt.u32PwrRegbakFlag)
	{
		pm_mcu_bakeup_debug();
	}

    /* sram retention  0x0 or 0xF */
    tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);
    tmp &= ~0x300000;
    writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);

    pclk = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);
    if(1==g_buck3on_txco_on)
    {
		tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_DFS5_CTRL3_OFFSET);
		tmp |=0x100;
		writel(tmp,HI_SYSCTRL_BASE_ADDR + HI_CRG_DFS5_CTRL3_OFFSET);
    }
    /* slow */
    tmp = readl(PWR_CTRL0);
	tmp &= 0xFFFFFFF8;
    tmp |= 0x2;
	writel(tmp, PWR_CTRL0);
    do
    {
        tmp = readl(PWR_CTRL0);
        tmp = (tmp >> 3)&0x7;
    }while(tmp != 0x2);

    writel(0x0, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	/* 8 cpu sleep */
	tmp = readl(PWR_CTRL0);
	tmp &= 0xFFFFFFF8;
	writel(tmp, PWR_CTRL0);

	/* 9 make sure */
	tmp = readl(PWR_CTRL0);

	ISB();

	/* wfi */
	WFI();
}

void pm_mcu_buck3on_pdown(void)
{
	u32 tmp = 0;
	u32 i = 0;

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER1);

	for(i = 0; i < 8; i++)
	{
		gPmBakeupSt.u32NvicEnable[i] = NVIC->ISER[i];
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	/* ªΩ–—‘¥Œ¥≈‰÷√ */
	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32AcoreWsrc[i]|gPmSt.u32CcoreWsrc[i]|gPmSt.u32McoreWsrc[i]|gPmSt.u32HiFiWsrc[i];
		NVIC->ISER[i] = tmp;
	}

   // if(1 == gPmNvSt.peridown_flag)
   // {
	    gpio_suspend();
        writel((0xF0<<22), PWR_CTRL4);
   // }

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER2);

	/* 4 ddr */
    if(1 == gPmNvSt.peridown_flag)
    {
	    pm_mddrc_bak();
    }
	pm_ddr_in_sref();

	if(gPmCountSt.u32PwrRegbakFlag)
	{
		pm_mcu_bakeup_debug();
	}
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER3);

	/* sram retention  0x0 or 0xF */
	tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);
	tmp &= ~0x300000;
	writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);

    pclk = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);
    /* slow */
    tmp = readl(PWR_CTRL0);
	tmp &= 0xFFFFFFF8;
    tmp |= 0x2;
	writel(tmp, PWR_CTRL0);
    do
    {
        tmp = readl(PWR_CTRL0);
        tmp = (tmp >> 3)&0x7;
    }while(tmp != 0x2);

    writel(0x0, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	/* 8 cpu sleep */
	tmp = readl(PWR_CTRL0);
	tmp &= 0xFFFFFFF8;
	writel(tmp, PWR_CTRL0);

	/* 9 make sure */
	tmp = readl(PWR_CTRL0);

	ISB();

	/* wfi */
	WFI();
}

void pm_mcu_buck3off_pup(void)
{
	u32 tmp = 0;
	u32 tmp1 = 0;
	u32 i = 0;
	u32 arm_boot_type = 0;
	u32 times = 0;

    writel(pclk, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	tmp = readl(PWR_CTRL0);
	tmp &= ~0x7;
	tmp |= 0x4;
    do{
	writel(tmp, PWR_CTRL0);
		tmp1 = readl(PWR_CTRL0);
		tmp1 = (tmp1 & 0x78) >> 3;
	}while((tmp1 != 0x4));

    times = 1000*5;
    while(times--)
    {
        writel(times, PWR_SRAM_TIMER81);
    }

	/* sram out retention  0x0 or 0xF */
	tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);
	tmp |= 0x300000;
	writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER4);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER5);
	/* ddrc init  ddrc sref out */
	/* ddr io */
	writel((0x1<<22), PWR_CTRL5);
	arm_boot_type = readl(PWR_CTRL0);
	if((arm_boot_type & 0x40000000)>>30)
	{
        arm_boot_type &= ~(0x40000000);
        writel(arm_boot_type, PWR_CTRL0);
		pm_mddrc_restore();
		writel(0x87654321, PWR_SRAM_TIMER51);
	}
	pm_ddr_out_sref();

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER6);
	ios_resume();
	writel((0xF0<<22), PWR_CTRL5);
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER61);

	pm_mcu_wakeup_debug();

    pm_set_bbpwakeup_flag();
	for(i = 0; i < 8; i++)
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
		NVIC->ISER[i] = gPmBakeupSt.u32NvicEnable[i];
	}
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER7);
	writel(*(u32*)0x900A4000, PWR_SRAM_TIMER71);

}

void pm_mcu_buck3on_pup(void)
{
	u32 tmp = 0;
	u32 tmp1 = 0;
	u32 i = 0;
	u32 arm_boot_type = 0;
    u32 times = 0;

    writel(pclk, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	/* cpu normal */
	tmp = readl(PWR_CTRL0);
	tmp &= ~0x7;
	tmp |= 0x4;
    do{
	writel(tmp, PWR_CTRL0);
		tmp1 = readl(PWR_CTRL0);
		tmp1 = (tmp1 & 0x78) >> 3;
	}while((tmp1 != 0x4));

    times = 1000*5;
    while(times--)
    {
    writel(times, PWR_SRAM_TIMER81);
    }

	/* sram out retention  0x0 or 0xF */
	tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);
	tmp |= 0x300000;
	writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL68_OFFSET);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER4);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER5);
	/* ddrc init  ddrc sref out */
	arm_boot_type = readl(PWR_CTRL0);
	if(((arm_boot_type & 0x40000000)>>30) && (1 == gPmNvSt.peridown_flag))
	{
        arm_boot_type &= ~(0x40000000);
        writel(arm_boot_type, PWR_CTRL0);
		pm_mddrc_restore();
        writel(0x87654321, PWR_SRAM_TIMER51);
	}
	pm_ddr_out_sref();
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER6);

   // if(1 == gPmNvSt.peridown_flag)
   // {
	    gpio_resume();
        writel((0xF0<<22), PWR_CTRL5);
   // }

	pm_mcu_wakeup_debug();

	pm_set_bbpwakeup_flag();
	for(i = 0; i < 8; i++)
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
		NVIC->ISER[i] = gPmBakeupSt.u32NvicEnable[i];
	}
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER7);
}

static void pm_mcu_bakeup_debug(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i=0;i<22;i++)
	{
		tmp = readl(PWR_CTRL0+i*4);
		writel(tmp, PWR_SRAM_PWRCTRL_REG+i*4);
	}

	for(i=0;i<22;i++)
	{
		tmp = readl(PWR_STAT0+i*4);
		writel(tmp, PWR_SRAM_PWRCTRL_REG+25*4+i*4);
	}
}

static void pm_mcu_wakeup_debug(void)
{
	u32 tmp = 0;
	u32 i = 0;

	if(gPmCountSt.u32WakeupDebugFlag)
	{
        writel(m3_record_num, PWR_SRAM_WAKEUP_NUM_REG);
        writel(m3_record_num, PWR_SRAM_WAKEUP_REG+m3_record_num*0x20);

        for(i=0;i<4;i++)
        {
            tmp = (NVIC->ISER[i]) & (NVIC->ICPR[i]);
            writel(tmp, PWR_SRAM_WAKEUP_REG+m3_record_num*0x20+0x10+i*4);
        }

        if(m3_record_num < 19)
        {
            m3_record_num++;
        }
        else
        {
            m3_record_num = 0;
        }
	}
}
/*lint -restore +e737*/
static void pm_mcu_process(void)
{
    u32 buck3offflag = ACORE_BUCK3_ONOFF_FLAG | CCORE_BUCK3_ONOFF_FLAG;
	s32 ret = 0;

    pm_dfs_bak();

    if((1 == gPmNvSt.buck3off_flag)&&(buck3offflag == 0))
	{
        writel(0x11111111, PWR_SRAM_TIMER10);
		ret = dpm_suspend_buck3off();
		if(ret == 0 )
		{
	        pm_config_init_buck3off();
	        pm_mcu_buck3off_pdown();
	        pm_mcu_buck3off_pup();
			ret = dpm_resume_buck3off();
			if(ret != 0 )
			{
				writel(0x11110001, PWR_SRAM_TIMER10);
			}
		}
		else
		{
			writel(0x11110000, PWR_SRAM_TIMER10);
		}
    }
    else
	{
        writel(0x22222222, PWR_SRAM_TIMER10);
		pm_config_init_buck3on();
        pm_mcu_buck3on_pdown();
        pm_mcu_buck3on_pup();
	}
}

static void pm_debug_clr_timer(void)
{
	writel(0x0, PWR_SRAM_TIMER0);
	writel(0x0, PWR_SRAM_TIMER1);
	writel(0x0, PWR_SRAM_TIMER2);
	writel(0x0, PWR_SRAM_TIMER21);
	writel(0x0, PWR_SRAM_TIMER22);
	writel(0x0, PWR_SRAM_TIMER3);
	writel(0x0, PWR_SRAM_TIMER4);
	writel(0x0, PWR_SRAM_TIMER5);
	writel(0x0, PWR_SRAM_TIMER51);
	writel(0x0, PWR_SRAM_TIMER6);
	writel(0x0, PWR_SRAM_TIMER61);
	writel(0x0, PWR_SRAM_TIMER62);
	writel(0x0, PWR_SRAM_TIMER63);
	writel(0x0, PWR_SRAM_TIMER7);
	writel(0x0, PWR_SRAM_TIMER71);
	writel(0x0, PWR_SRAM_TIMER8);
	writel(0x0, PWR_SRAM_TIMER81);
    writel(0x0, PWR_SRAM_TIMER9);
    writel(0x0, PWR_SRAM_TIMER10);
}
/*lint --e{718,732,746}*/
void pm_normal_wfi(void)
{
	int irqlock;
	u32 sleeptime = 0;
	u32 tc = 0;
	u32 tmp = 0;
    u32 btime = 0;
    u32 ftime = 0;

	local_irq_save(irqlock);

	tc = rt_suspend();
    if(tc>1)
	{
		gPmCountSt.u32NormalWfiTimes++;
        btime = bsp_get_slice_value();

		/* pwr_ctrl0 bit25 bit26 set 0*/
		tmp = readl(PWR_CTRL0);
		tmp &= ~0x6000000;
		writel(tmp, PWR_CTRL0);

		WFI();

		/* pwr_ctrl0 bit25 bit26 set 1*/
		tmp = readl(PWR_CTRL0);
		tmp |= 0x6000000;
		writel(tmp, PWR_CTRL0);

		ftime = bsp_get_slice_value();
        sleeptime = (ftime - btime)* 1000 / PM_MCU_TIMER_CLK;
	}
	else
	{
		sleeptime = 0;
	}
	rt_resume(sleeptime);

	local_irq_restore(irqlock);

}

void pm_deepsleep(void)
{
	int irqlock;
	u32 sleeptime = 0;
	u32 tc = 0;
	s32 tmp = 0;
    u32 btime = 0;
    u32 ftime = 0;

	local_irq_save(irqlock);
	tmp = dpm_suspend();
	if(tmp == DPM_OK)
	{
		gPmSt.u32SleepFlag = 1;
		tc = rt_suspend();
        if(tc>1)
    	{
    		gPmCountSt.u32DeepSleepTimes++;
            pm_debug_clr_timer();
            btime = bsp_get_slice_value();
            writel(btime, PWR_SRAM_TIMER0);

    		pm_mcu_process();

    		ftime = bsp_get_slice_value();
            sleeptime = (ftime - btime)* 1000 / PM_MCU_TIMER_CLK;
            writel(ftime, PWR_SRAM_TIMER8);
    	}
		else
		{
			sleeptime = 0;
		}
		rt_resume(sleeptime);
		gPmSt.u32SleepFlag = 0;
		tmp = dpm_resume();
		if(tmp != DPM_OK)
		{
			gPmErrorSt.u32DpmResumeErrTimes++;
		}
        writel(bsp_get_slice_value(), PWR_SRAM_TIMER9);
	}
	else
	{
		gPmErrorSt.u32DpmSuspendErrTimes++;
	}
	local_irq_restore(irqlock);

}
void idle_task(void)
{
	bsp_wdt_feed();
	/* uart0 wakeup */
    if(uart_delay_flag == 1)
    {
        udelay(10000000);
        uart_delay_flag = 0;
    }

	/* deepsleep 0Õ∂∆±ÀØ√ﬂ£¨1∑¥∂‘ÀØ√ﬂ */
	if((!(gPmSt.u32VoteMap & gPmSt.u32VoteMask)) && (1 == gPmNvSt.deepsleep_flag)
	   && (*(u32*)SRAM_RTT_SLEEP_FLAG_ADDR != 1))
	{
		pm_deepsleep();
	}
	else if((sys_initial==1) && ((1 == gPmNvSt.normalwfi_flag)))
	{
		pm_normal_wfi();
	}

}

void pm_print_wsrc(void)
{
	u32 i = 0;
	u32 j = 0;
	u32 tmp = 0;

	printk("wsrc 0x%x  0x%x  0x%x  0x%x\n", readl(PWR_CTRL16), readl(PWR_CTRL17), readl(PWR_CTRL18), readl(PWR_CTRL19));
	printk("ISER 0x%x  0x%x  0x%x  0x%x\n", NVIC->ISER[0], NVIC->ISER[1], NVIC->ISER[2], NVIC->ISER[3]);

	for(j = 0; j < 4; j++)
	{
		printk("wsrc num%d:",j);
		tmp = readl(PWR_CTRL16+j*4);
		for(i = 0; i < 32; i++)
		{
			if(tmp & ((u32)0x1<<i))
			{
				printk(" %d ",i+j*32+16);
			}
		}
		printk("\n");
	}

	pm_print_wakeup_debug();
}

static void pm_print_wakeup_debug(void)
{
	u32 i = 0;
	u32 j = 0;
	u32 tmp = 0;

	if(gPmCountSt.u32WakeupDebugFlag)
	{
		for(j = 0; j < 4; j++)
		{
			printk("wakeup num%d:",j);
			tmp = readl(PWR_SRAM_WAKEUP_REG+m3_record_num*0x20+0x10+j*4);
			for(i = 0; i < 32; i++)
			{
				if(tmp & ((u32)0x1<<i))
				{
					printk(" %d ",i+j*32+16);
				}
			}
			printk("\n");
		}
	}
}

static void pm_set_peri_down(u32 flag)
{
	if(flag)
	{
		writel(0x20000000, PWR_CTRL9);
	}
	else
	{
		writel(0x40000000, PWR_CTRL9);
	}
}

void pm_set_debug(u32 type, u32 flag)
{
	switch(type)
	{
		case 0:
			printk(" input error\n");
			break;
		case 1:
			gPmNvSt.normalwfi_flag = (BSP_U8)flag;
			break;
		case 2:
			gPmNvSt.deepsleep_flag = (BSP_U8)flag;
			break;
		case 3:
			if(0 == gPmNvSt.buck3off_flag)
			{
				gPmNvSt.peridown_flag = (BSP_U8)flag;
				pm_set_peri_down(flag);
			}
			else
			{
				printk("buck3off peridown flag cannot change\n");
			}
			break;
		case 4:
			gPmCountSt.u32PwrRegbakFlag = flag;
			break;
		case 5:
			gPmCountSt.u32WakeupDebugFlag = flag;
			break;
		case 6:
			gPmCountSt.u32PrintIrqWsrcFlag = flag;
			break;
		default:
			printk(" input error\n");
			break;
	}
}

/**************************dfs**************************/
#include "m3_cpufreq.h"

extern T_CPUFREQ_ST g_cpufreq;

static void dfs_get_ddrc_timing(u32 fbus, u32 pll, T_CPUFREQ_DDRC_TIMING* ptiming);
static void dfs_config_reg(u32 cur, u32 new);

T_CPUFREQ_PROFILE gCpufrqProfile[CPUFREQ_MAX_PROFILE+1] =
    {//{0, 0x40003733},
	 //{0, 0x40007733},
     {0, 0x40003333},
    // {0, 0x40001313},
     //{0, 0x40003300},
     {0, 0x40001311},
     //{0, 0x40001310},
     //{0, 0x40001101},
     //{0, 0x40001100},
     //{1, 0x40005555},
     //{1, 0x40005522},
     //{1, 0x40005b55},
     //{1, 0x40007733},
     //{1, 0x40003733},
     //{1, 0x40003333},
     //{1, 0x40005511},
     //{1, 0x40003311},
     {1, 0x40003301},
     //{1, 0x40001310},
     {1, 0x40001301},
     {1, 0x40001300}};

/* only support 666M and 400M */
T_CPUFREQ_PLL_CONFIG gCpufrqPll[CPUFREQ_MAX_PLL + 1] =
	{{0x800000, 0x310103e},
	 {0x600000, 0x2101045}};

T_CPUFREQ_DDRC_CALC gDdrCalc[2];
/* dfs */
void pm_dfs_bak(void)
{
    u32 cur = M3_CUR_CPUFREQ_PROFILE;
    u32 max = M3_MAX_CPUFREQ_PROFILE;

    if(cur < max)
    {
        dfs_config_reg(cur, max);
        g_cpufreq.curprof = M3_MAX_CPUFREQ_PROFILE;
        M3_CUR_CPUFREQ_PROFILE = M3_MAX_CPUFREQ_PROFILE;
    }
}

void dfs_set_profile(u32 profile)
{
	dfs_config_reg(g_cpufreq.curprof, profile);
}

u32 dfs_get_profile(void)
{
	u32 profile = CPUFREQ_INVALID_PROFILE;
	T_CPUFREQ_PLL_CONFIG curconf;
	u32 invalid = 0xFFFFFFFF;
	u32 curpll = invalid;
	u32 curclkdiv = 0;
	u32 i = 0;

	/* find right pll */
	curconf.dfs1ctrl1 = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_DFS1_CTRL1_OFFSET);
	curconf.dfs1ctrl2 = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_DFS1_CTRL2_OFFSET);

	for(i = 0;i <= CPUFREQ_MAX_PLL;i++)
	{
		if(curconf.dfs1ctrl1 == gCpufrqPll[i].dfs1ctrl1)
		{
			if(curconf.dfs1ctrl2 == gCpufrqPll[i].dfs1ctrl2)
			{
				curpll = i;
				break;
			}
		}
	}
	if(curpll == invalid)
	{
		printk("dfs_get_profile pll error\n");
		return CPUFREQ_INVALID_PROFILE;
	}

	/* find right profile */
	curclkdiv = readl(HI_SYSCRG_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	for(i = 0;i <= CPUFREQ_MAX_PROFILE;i++)
	{
		if(curpll == gCpufrqProfile[i].pll)
		{
			if(curclkdiv == gCpufrqProfile[i].clkdiv)
			{
				profile = i;
				break;
			}
		}
	}

	return profile;
}

void dfs_get_ddrc_timing(u32 fbus, u32 pll, T_CPUFREQ_DDRC_TIMING* ptiming)
{
	/* 400M */
	if(0 == pll)
	{
		switch(fbus)
		{
			case 1:
				ptiming->nxttiming0 = 0x52650f09;
				ptiming->nxttiming1 = 0x0ea7461a;
				ptiming->nxttiming2 = 0x6250c030;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			case 3:
				ptiming->nxttiming0 = 0x52330805;
				ptiming->nxttiming1 = 0x08a7460f;
				ptiming->nxttiming2 = 0x6250801e;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			case 7:
				ptiming->nxttiming0 = 0x52330603;
				ptiming->nxttiming1 = 0x04a74607;
				ptiming->nxttiming2 = 0x6250800c;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			default:
				break;
		}
	}
	else if(1 == pll)/* 666M */
	{
		switch(fbus)
		{
			case 1:
				ptiming->nxttiming0 = 0x5498170e;
				ptiming->nxttiming1 = 0x18a7462c;
				ptiming->nxttiming2 = 0x64514051;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			case 3:
				ptiming->nxttiming0 = 0x52540c07;
				ptiming->nxttiming1 = 0x0ca74616;
				ptiming->nxttiming2 = 0x6250a028;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			case 5:
				ptiming->nxttiming0 = 0x52330805;
				ptiming->nxttiming1 = 0x08a7460f;
				ptiming->nxttiming2 = 0x6250801e;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			case 7:
				ptiming->nxttiming0 = 0x52330704;
				ptiming->nxttiming1 = 0x06a7460b;
				ptiming->nxttiming2 = 0x62508014;
				ptiming->nxttiming3 = 0xffd4e033;
				break;
			default:
				break;
		}
	}
	else
	{
		printk("dfs_get_ddrc_timing error\n");
	}
}

void dfs_ddrc_config(u32 fbus, u32 pll)
{
    /* 400M */
	if(0 == pll)
	{
		switch(fbus)
		{
			case 1:
				writel(gDdrCalc[1].ddr_reg_83c, CM3_DDRC_BASE_ADDR + 0x83c);
		        writel(gDdrCalc[1].ddr_reg_abc, CM3_DDRC_BASE_ADDR + 0xabc);
		        writel(gDdrCalc[1].ddr_reg_b3c, CM3_DDRC_BASE_ADDR + 0xb3c);
		        writel(gDdrCalc[1].ddr_reg_bbc, CM3_DDRC_BASE_ADDR + 0xbbc);
		        writel(gDdrCalc[1].ddr_reg_c3c, CM3_DDRC_BASE_ADDR + 0xc3c);
				break;
			case 3:
			case 7:
                writel(gDdrCalc[0].ddr_reg_83c, CM3_DDRC_BASE_ADDR + 0x83c);
		        writel(gDdrCalc[0].ddr_reg_abc, CM3_DDRC_BASE_ADDR + 0xabc);
		        writel(gDdrCalc[0].ddr_reg_b3c, CM3_DDRC_BASE_ADDR + 0xb3c);
		        writel(gDdrCalc[0].ddr_reg_bbc, CM3_DDRC_BASE_ADDR + 0xbbc);
		        writel(gDdrCalc[0].ddr_reg_c3c, CM3_DDRC_BASE_ADDR + 0xc3c);
				break;
			default:
				break;
		}
	}
	else if(1 == pll)/* 666M */
	{
		switch(fbus)
		{
			case 1:
				writel(gDdrCalc[1].ddr_reg_83c, CM3_DDRC_BASE_ADDR + 0x83c);
		        writel(gDdrCalc[1].ddr_reg_abc, CM3_DDRC_BASE_ADDR + 0xabc);
		        writel(gDdrCalc[1].ddr_reg_b3c, CM3_DDRC_BASE_ADDR + 0xb3c);
		        writel(gDdrCalc[1].ddr_reg_bbc, CM3_DDRC_BASE_ADDR + 0xbbc);
		        writel(gDdrCalc[1].ddr_reg_c3c, CM3_DDRC_BASE_ADDR + 0xc3c);
				break;
			case 3:
			case 5:
			case 7:
				writel(gDdrCalc[0].ddr_reg_83c, CM3_DDRC_BASE_ADDR + 0x83c);
		        writel(gDdrCalc[0].ddr_reg_abc, CM3_DDRC_BASE_ADDR + 0xabc);
		        writel(gDdrCalc[0].ddr_reg_b3c, CM3_DDRC_BASE_ADDR + 0xb3c);
		        writel(gDdrCalc[0].ddr_reg_bbc, CM3_DDRC_BASE_ADDR + 0xbbc);
		        writel(gDdrCalc[0].ddr_reg_c3c, CM3_DDRC_BASE_ADDR + 0xc3c);
				break;
			default:
				break;
		}
	}
	else
	{
		printk("dfs_ddrc_config error\n");
	}

}
/* ddr init 166M  [0]<=166  [1]>166 */
void dfs_ddrc_calc(void)
{
    SRAM_SMALL_SECTIONS * sram_mem = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR);
    gDdrCalc[0].ddr_reg_83c = sram_mem->SRAM_DFS_DDRC_CFG[0].ddr_reg_83c;
	gDdrCalc[1].ddr_reg_83c = sram_mem->SRAM_DFS_DDRC_CFG[1].ddr_reg_83c;

	gDdrCalc[0].ddr_reg_abc = sram_mem->SRAM_DFS_DDRC_CFG[0].ddr_reg_abc ;
	gDdrCalc[1].ddr_reg_abc = sram_mem->SRAM_DFS_DDRC_CFG[1].ddr_reg_abc ;

	gDdrCalc[0].ddr_reg_b3c = sram_mem->SRAM_DFS_DDRC_CFG[0].ddr_reg_b3c ;
	gDdrCalc[1].ddr_reg_b3c = sram_mem->SRAM_DFS_DDRC_CFG[1].ddr_reg_b3c ;

	gDdrCalc[0].ddr_reg_bbc = sram_mem->SRAM_DFS_DDRC_CFG[0].ddr_reg_bbc ;
	gDdrCalc[1].ddr_reg_bbc = sram_mem->SRAM_DFS_DDRC_CFG[1].ddr_reg_bbc ;

	gDdrCalc[0].ddr_reg_c3c = sram_mem->SRAM_DFS_DDRC_CFG[0].ddr_reg_c3c ;
	gDdrCalc[1].ddr_reg_c3c = sram_mem->SRAM_DFS_DDRC_CFG[1].ddr_reg_c3c ;
}
void dfs_config_reg(u32 cur, u32 new)
{
	u32 tmp = 0;
	u32 curfbus = 0;
	u32 newfbus = 0;
	u32 curpll = 0;
	u32 newpll = 0;
	u32 ddrcnochange = 0;
    u32 times = 0;
	T_CPUFREQ_DDRC_TIMING timing = {0};

	/* 1 */
	tmp = readl(CM3_LP_PWRCTRL0);
	tmp |= 0xFF000000;
	writel(tmp, CM3_LP_PWRCTRL0);

	/* 2 */
	curpll = gCpufrqProfile[cur].pll;
	newpll = gCpufrqProfile[new].pll;
	curfbus = (gCpufrqProfile[cur].clkdiv >> 12) & 0xf;
	newfbus = (gCpufrqProfile[new].clkdiv >> 12) & 0xf;
	if(curpll == newpll)
	{
		if(curfbus == newfbus)
		{
			/* ddrc no change */
			ddrcnochange = 1;
			goto step6;
		}
	}

	/* 3 */
	//tmp = readl(CM3_DDRC_BASE_ADDR + 0x20);
	//tmp |= 0x100000;
	//writel(tmp, CM3_DDRC_BASE_ADDR + 0x20);

	/* 4 */
	dfs_get_ddrc_timing(newfbus, newpll, &timing);
	writel(timing.nxttiming0, CM3_DDRC_BASE_ADDR + 0x70);
	writel(timing.nxttiming1, CM3_DDRC_BASE_ADDR + 0x74);
	writel(timing.nxttiming2, CM3_DDRC_BASE_ADDR + 0x78);
	writel(timing.nxttiming3, CM3_DDRC_BASE_ADDR + 0x7c);

	/* 5 */
	writel(0x1, CM3_DDRC_BASE_ADDR + 0x4);
	do{
		tmp = readl(CM3_DDRC_BASE_ADDR);
		tmp &= 0x4;
	}while(!tmp);
    times = 20;
    while(times)
    {
        times--;
    }

	dfs_ddrc_config(newfbus, newpll);
	/* 6 */
step6:
	if(curpll == newpll)
	{
		writel(gCpufrqProfile[new].clkdiv, CM3_CRG_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);
		/* pll no change */
		goto step12;
	}

	/* 7 */
	//tmp = readl(CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);
	//tmp |= 0x80;
	//writel(tmp, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);

	writel(0x40001300, CM3_CRG_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

	tmp = readl(CM3_CRG_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);
	tmp &= ~0x300;
	tmp |= 0x200;
	writel(tmp, CM3_CRG_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);

	/* 8 */
	writel(0x129, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);
	writel(gCpufrqPll[newpll].dfs1ctrl1, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL1_OFFSET);
	writel(gCpufrqPll[newpll].dfs1ctrl2, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL2_OFFSET);
    times = 30;
    while(times)
    {
        times--;
    }
	writel(0x128, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);

	/* 9 */
	do
	{
		tmp = readl(CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);
		tmp = (tmp >> 31) & 0x1;
	}while(!tmp);

	/* 10 */
	tmp = readl(CM3_CRG_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);
	tmp &= ~0x300;
	tmp |= 0x100;
	writel(tmp, CM3_CRG_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);

	/* 11 */
	writel(gCpufrqProfile[new].clkdiv, CM3_CRG_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);
	writel(0x28, CM3_CRG_BASE_ADDR + HI_CRG_DFS1_CTRL3_OFFSET);

	/* 12 */
step12:
	if(ddrcnochange)
	{
		goto step14;
	}

	/* 13 */
	writel(0x0, CM3_DDRC_BASE_ADDR + 0x4);
	do{
		tmp = readl(CM3_DDRC_BASE_ADDR);
		tmp &= 0x4;
	}while(tmp);
    times = 20;
    while(times)
    {
        times--;
    }

	/* 14 */
step14:
	tmp = readl(CM3_LP_PWRCTRL0);
	tmp &= ~0xFF000000;
	writel(tmp, CM3_LP_PWRCTRL0);
}
