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
#include "osl_bio.h"
#include "osl_irq.h"

#include "soc_memmap.h"
#include "hi_syscrg.h"
#include "hi_syssc.h"
#include "hi_timer.h"

#include "m3_pm.h"
#include "pm_api.h"
#include <irq.h>
#include "bsp_hardtimer.h"
#include "dpm_balong.h"
#include "drv_nv_def.h"
#include "drv_nv_id.h"
#include "bsp_nvim.h"


extern T_PM_ST gPmSt;
extern T_PM_BAKEUP_ST gPmBakeupSt;
extern T_PM_COUNT_ST gPmCountSt;
extern T_PM_ERROR_ST gPmErrorSt;
extern DRV_NV_PM_TYPE gPmNvSt;
extern u32 sys_initial;

static irqreturn_t IntTimerHandler(void);

void pm_set_wakeup_reg(u32 groupnum, u32 bitnum)
{
	int irqlock;
	u32 tmp = 0;

	local_irq_save(irqlock);
	tmp = readl(PWR_CTRL15 + groupnum*4);
	tmp |= 0x1 << bitnum;
	writel(tmp, PWR_CTRL15 + groupnum*4);
	local_irq_restore(irqlock);
}

/* pm */
void pm_mdma9_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x200, CRG_CTRL4);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x10000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL14);
	tmp |= 0x2080;
	writel(tmp, CRG_CTRL14);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x10000;
	writel(tmp, PWR_CTRL7);
}
void pm_mdma9_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x10000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 16) & 0x1;
	}while(!tmp);

	/* enable clk */
	writel(0x200, CRG_CTRL3);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x10000;
	writel(tmp, PWR_CTRL6);

	/* unreset */
	tmp = readl(CRG_CTRL14);
	tmp &= ~0x2080;
	writel(tmp, CRG_CTRL14);
}

void pm_appa9_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x100, CRG_CTRL4);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x100000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL14);
	tmp |= 0x1002;
	writel(tmp, CRG_CTRL14);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x100000;
	writel(tmp, PWR_CTRL7);
}
void pm_appa9_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x100000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 20) & 0x1;
	}while(!tmp);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x100000;
	writel(tmp, PWR_CTRL6);

	/* enable clk */
	writel(0x100, CRG_CTRL3);

	/* unreset */
	tmp = readl(CRG_CTRL14);
	tmp &= ~0x1002;
	writel(tmp, CRG_CTRL14);
}

void pm_hifi_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x200000, CRG_CTRL10);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x80000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL15);
	tmp |= 0x180000;
	writel(tmp, CRG_CTRL15);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x80000;
	writel(tmp, PWR_CTRL7);
}
void pm_hifi_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x80000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 19) & 0x1;
	}while(!tmp);

	/* unreset */
	tmp = readl(CRG_CTRL15);
	tmp &= ~0x180000;
	writel(tmp, CRG_CTRL15);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x80000;
	writel(tmp, PWR_CTRL6);

	/* enable clk */
	writel(0x200000, CRG_CTRL9);
}

void pm_bbe16_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x100000, CRG_CTRL10);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x40000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL15);
	tmp |= 0x30000;
	writel(tmp, CRG_CTRL15);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x40000;
	writel(tmp, PWR_CTRL7);
}
void pm_bbe16_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x40000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 18) & 0x1;
	}while(!tmp);

	/* unreset */
	tmp = readl(CRG_CTRL15);
	tmp &= ~0x30000;
	writel(tmp, CRG_CTRL15);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x40000;
	writel(tmp, PWR_CTRL6);

	/* enable clk */
	writel(0x100000, CRG_CTRL9);
}

void pm_dsp0_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x1, CRG_CTRL10);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x2000000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL15);
	tmp |= 0x5;
	writel(tmp, CRG_CTRL15);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x2000000;
	writel(tmp, PWR_CTRL7);
}
void pm_dsp0_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x2000000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 25) & 0x1;
	}while(!tmp);

	/* unreset */
	tmp = readl(CRG_CTRL15);
	tmp &= ~0x5;
	writel(tmp, CRG_CTRL15);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x2000000;
	writel(tmp, PWR_CTRL6);

	/* enable clk */
	writel(0x1, CRG_CTRL9);
}

void pm_a15_pdown(void)
{
	u32 tmp = 0;

	/* disable clk */
	writel(0x1000000, CRG_CTRL40);

	/* iso ctrl enable */
	tmp = readl(PWR_CTRL6);
	tmp |= 0x1000000;
	writel(tmp, PWR_CTRL6);

	/* reset */
	tmp = readl(CRG_CTRL16);
	tmp |= 0x1110000;
	writel(tmp, CRG_CTRL16);

	/* mtcmos power down */
	tmp = readl(PWR_CTRL7);
	tmp &= ~0x1000000;
	writel(tmp, PWR_CTRL7);
}
void pm_a15_pup(void)
{
	u32 tmp = 0;

	/* mtcmos power up and wait for complete*/
	tmp = readl(PWR_CTRL7);
	tmp |= 0x1000000;
	writel(tmp, PWR_CTRL7);

	do
	{
		tmp = readl(PWR_STAT1);
		tmp = (tmp >> 24) & 0x1;
	}while(!tmp);

	/* iso ctrl disable */
	tmp = readl(PWR_CTRL6);
	tmp &= ~0x1000000;
	writel(tmp, PWR_CTRL6);

	/* enable clk */
	writel(0x1000000, CRG_CTRL39);

	/* unreset */
	tmp = readl(CRG_CTRL16);
	tmp &= ~0x1110000;
	writel(tmp, CRG_CTRL16);
}

/* 低功耗相关的一些寄存器提前配置 */
void pm_config_init(void)
{
	pm_config_init_m3on();
}

void pm_config_init_m3on(void)
{
	u32 tmp = 0;

	/* tcxo */
	writel(0x4FFCD, PWR_CTRL2);

	/* 1 */
	writel(0x200149B, PWR_CTRL4);
	writel(0x1FFFFFF, PWR_CTRL10);
	writel(0x1FFFFFF, PWR_CTRL11);

	/* 2 */
	tmp = readl(PWR_CTRL0);
	tmp |= 0x6000C80;
	tmp &= 0xFFFFFBFF;
	writel(tmp, PWR_CTRL0);

	/* 3 a15 power down */
	pm_a15_pdown();

	/* 4 close a15 dfs */
	tmp = readl(CRG_CTRL28);
	tmp |= 0x1;
	writel(tmp, CRG_CTRL28);

	/* wakeup src */
	writel(0x0, PWR_CTRL12);
	writel(0x0, PWR_CTRL13);
	writel(0x0, PWR_CTRL15);
	writel(0x0, PWR_CTRL16);
	writel(0x0, PWR_CTRL17);
	writel(0x0, PWR_CTRL18);

}

void pm_mcu_bakeup_debug(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i=0;i<25;i++)
	{
		tmp = readl(PWR_CTRL0+i*4);
		writel(tmp, PWR_SRAM_PWRCTRL_REG+i*4);
	}

	for(i=0;i<8;i++)
	{
		tmp = readl(PWR_STAT0+i*4);
		writel(tmp, PWR_SRAM_PWRCTRL_REG+25*4+i*4);
	}
}

void pm_mcu_m3on_pdown(void)
{
	u32 tmp = 0;
	u32 i = 0;

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER1);
	/* bakeup reg */
	gPmBakeupSt.u32ClkDiv = readl(CRG_CTRL18);

	for(i = 0; i < 8; i++)
	{
		gPmBakeupSt.u32NvicEnable[i] = NVIC->ISER[i];
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	/* 唤醒源未配置 */
	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32AcoreWsrc[i]|gPmSt.u32CcoreWsrc[i]|gPmSt.u32McoreWsrc[i];
		NVIC->ISER[i] = tmp;
	}

	if(gPmCountSt.u32PwrRegbakFlag)
	{
		pm_mcu_bakeup_debug();
	}

	/* 2 dsp dfs */
	tmp = readl(CRG_CTRL25);
	tmp |= 0x1;
	writel(tmp, CRG_CTRL25);

	/* 3 ddrc sref */
	writel(0x1, MDDRC_SREFCTRL);
	do{
		tmp = readl(MDDRC_STATUS);
		tmp &= 0x4;
	}while(!tmp);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER2);

	/* sram retention  0x0 or 0xF */
	writel(0x0, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL66_OFFSET);

	/* 4 cpu slow */
	tmp = readl(PWR_CTRL0);
	tmp &= ~0x7;
	tmp |= 0x2;
	writel(tmp, PWR_CTRL0);
	do{
		tmp = readl(PWR_CTRL0);
		tmp = (tmp & 0x78) >> 3;
	}while((tmp != 0x2));

	/* 5 */
	writel(0x11111, CRG_CTRL18);

	/* 6 close peri dfs */
	tmp = readl(CRG_CTRL34);
	tmp |= 0x1;
	writel(tmp, CRG_CTRL34);
	/* 7 close a9 dfs */
	tmp = readl(CRG_CTRL22);
	tmp |= 0x1;
	writel(tmp, CRG_CTRL22);

	/* 8 cpu sleep */
	tmp = readl(PWR_CTRL0);
	tmp &= 0xFFFFFFF8;
	writel(tmp, PWR_CTRL0);

	/* 9 make sure */
	tmp = readl(PWR_CTRL0);

	/* wfi */
	asm volatile ( "wfi;");
}

void pm_mcu_m3on_pup(void)
{
	u32 tmp = 0;
	u32 tmp1 = 0;
	u32 i = 0;

	/* open a9 dfs */
	tmp = readl(CRG_CTRL22);
	tmp &= ~0x1;
	writel(tmp, CRG_CTRL22);

	/* open peri dfs */
	tmp = readl(CRG_CTRL34);
	tmp &= ~0x1;
	writel(tmp, CRG_CTRL34);

	writel(gPmBakeupSt.u32ClkDiv, CRG_CTRL18);

	/* cpu normal */
	tmp1 = readl(PWR_CTRL0);
	tmp1 &= ~0x7;
	tmp1 |= 0x4;
	do{
		writel(tmp1, PWR_CTRL0);
		tmp = readl(PWR_CTRL0);
		writel(tmp, 0x4fe1e00c);
		tmp = (tmp & 0x78) >> 3;
	}while((tmp != 0x4));

	/* sram out retention  0x0 or 0xF */
	writel(0xFFFF, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL66_OFFSET);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER3);

    /* ddrc sref out */
    writel(0x11, 0x90020804);
    do{
		tmp = readl(0x90020818);
		tmp &= 0x1;
	}while(tmp);
    do{
		tmp = readl(0x90020818);
		tmp &= 0x80000003;
	}while(tmp != 0x80000003);
	writel(0x0, MDDRC_SREFCTRL);
	do{
		tmp = readl(MDDRC_STATUS);
		tmp &= 0x4;
	}while(tmp);

	writel(bsp_get_slice_value(), PWR_SRAM_TIMER4);

	/* open dsp dfs */
	tmp = readl(CRG_CTRL25);
	tmp &= ~0x1;
	writel(tmp, CRG_CTRL25);

	for(i = 0; i < 8; i++)
	{
		NVIC->ISER[i] = gPmBakeupSt.u32NvicEnable[i];
	}
	writel(bsp_get_slice_value(), PWR_SRAM_TIMER5);
}

/* mcu不掉电 peri掉电(只关peri dfs) tcm retention */
void pm_mcu_default_pdown(void)
{
	pm_mcu_m3on_pdown();
}

void pm_mcu_default_pup(void)
{
	pm_mcu_m3on_pup();
}

void timer_init(void)
{
	writel(0x0, PM_MCU_TIMER_BASE + HI_TIMER_CONTROLREG_OFFSET);

	(void)request_irq(PM_MCU_TIMER_INT_LVL, (irq_handler_t)IntTimerHandler, 0, "timer10 irq", 0);

}

static void timer_set_value(u32 value)
{
	writel(0x0, PM_MCU_TIMER_BASE + HI_TIMER_CONTROLREG_OFFSET);
	writel(value, PM_MCU_TIMER_BASE + HI_TIMER_LOADCOUNT_OFFSET);
	writel(0x3, PM_MCU_TIMER_BASE + HI_TIMER_CONTROLREG_OFFSET);
}

static u32 __inline__ timer_get_value(void)
{
	u32 tmp = 0;

	/* to make sure timer loadcount is set ok */
	do
	{
		tmp = readl(PM_MCU_TIMER_BASE + HI_TIMER_CURRENTVALUE_OFFSET);
	}while(tmp == 0xFFFFFFFF);

	return tmp;
}

static void __inline__ timer_close()
{
	writel(0x0, PM_MCU_TIMER_BASE + HI_TIMER_CONTROLREG_OFFSET);
}

static irqreturn_t IntTimerHandler(void)
{
	u32 tmp = 0;

	UNUSED(tmp);
	tmp = readl(PM_MCU_TIMER_BASE + HI_TIMER_EOI_OFFSET);
	printk("timer10 \n");
	gPmCountSt.u32McuTimerIntTimes++;
	return IRQ_HANDLED;
}

static void pm_debug_clr_timer(void)
{
	writel(0x0, PWR_SRAM_TIMER0);
	writel(0x0, PWR_SRAM_TIMER1);
	writel(0x0, PWR_SRAM_TIMER2);
	writel(0x0, PWR_SRAM_TIMER3);
	writel(0x0, PWR_SRAM_TIMER4);
	writel(0x0, PWR_SRAM_TIMER5);
	writel(0x0, PWR_SRAM_TIMER6);
	writel(0x0, PWR_SRAM_TIMER7);
}

void pm_normal_wfi(void)
{
	int irqlock;
	u32 sleeptime = 0;
	u32 del = 0;
	u32 tc = 0;
	u32 tmp = 0;

	local_irq_save(irqlock);

	sleeptime = rt_suspend();
	if(sleeptime>1)
	{
		tc = sleeptime * PM_MCU_TIMER_CLK / 1000;
		timer_set_value(tc);
		gPmCountSt.u32NormalWfiTimes++;

		/* pwr_ctrl0 bit25 bit26 set 0*/
		tmp = readl(PWR_CTRL0);
		tmp &= ~0x6000000;
		writel(tmp, PWR_CTRL0);

		asm volatile ( "wfi;");

		/* pwr_ctrl0 bit25 bit26 set 1*/
		tmp = readl(PWR_CTRL0);
		tmp |= 0x6000000;
		writel(tmp, PWR_CTRL0);

		if(NVIC_GetPendingIRQ(PM_MCU_TIMER_INT_LVL - 16))
		{
			sleeptime -= 1;
		}
		else
		{
			del = timer_get_value();
			sleeptime = (tc - del) * 1000 / PM_MCU_TIMER_CLK;
		}
		timer_close();
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
	u32 del = 0;
	u32 tc = 0;
	u32 tmp = 0;

	local_irq_save(irqlock);
	tmp = dpm_suspend();
	if(tmp == DPM_OK)
	{
		gPmSt.u32SleepFlag = 1;
		sleeptime = rt_suspend();
		if(sleeptime > 1)
		{
			pm_debug_clr_timer();
			writel(bsp_get_slice_value(), PWR_SRAM_TIMER0);

			tc = sleeptime * PM_MCU_TIMER_CLK / 1000;
			timer_set_value(tc);
			gPmCountSt.u32DeepSleepTimes++;
			pm_mcu_default_pdown();
			pm_mcu_default_pup();
			if(NVIC_GetPendingIRQ(PM_MCU_TIMER_INT_LVL - 16))
			{
				sleeptime -= 1;
			}
			else
			{
				del = timer_get_value();
				sleeptime = (tc - del) * 1000 / PM_MCU_TIMER_CLK;
			}
			timer_close();
			writel(bsp_get_slice_value(), PWR_SRAM_TIMER6);
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
	}
	else
	{
		gPmErrorSt.u32DpmSuspendErrTimes++;
	}
	local_irq_restore(irqlock);

}

void idle_task(void)
{
	/* deepsleep 0投票睡眠，1反对睡眠 */
	if((!(gPmSt.u32VoteMap & gPmSt.u32VoteMask)) && (1 == gPmNvSt.deepsleep_flag))
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

	printk("wsrc 0x%x  0x%x  0x%x  0x%x\n", readl(PWR_CTRL15), readl(PWR_CTRL16), readl(PWR_CTRL17), readl(PWR_CTRL18));
	printk("ISER 0x%x  0x%x  0x%x  0x%x\n", NVIC->ISER[0], NVIC->ISER[1], NVIC->ISER[2], NVIC->ISER[3]);

	for(j = 0; j < 4; j++)
	{
		printk("wsrc num%d:",j);
		for(i = 0; i < 32; i++)
		{
			tmp = readl(PWR_CTRL15+j*4);
			if(tmp & (0x1<<i))
			{
				printk(" %d ",i+j*32+16);
			}
		}
		printk("\n");
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
			gPmNvSt.normalwfi_flag = flag;
			break;
		case 2:
			gPmNvSt.deepsleep_flag = flag;
			break;
		case 3:
			gPmNvSt.peridown_flag = flag;
			pm_set_peri_down(flag);
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

T_CPUFREQ_PROFILE gCpufrqProfile[CPUFREQ_MAX_PROFILE+1] =
	{{0, 0x22421},
	 {0, 0x22411}};

/* dfs */
void dfs_set_profile(u32 profile)
{
	u32 tmp = 0;

	tmp = readl(CRG_CTRL18) & 0xFFF00000;
	tmp |=  gCpufrqProfile[profile].clkdiv;
	writel(tmp, CRG_CTRL18);
}

u32 dfs_get_profile(void)
{
	u32 tmp = 0;
	u32 i = 0;
	u32 profile = CPUFREQ_INVALID_PROFILE;

	tmp = readl(CRG_CTRL18) & 0x000FFFFF;
	for(i = 0;i <= CPUFREQ_MAX_PROFILE;i++)
	{
		if(tmp == gCpufrqProfile[i].clkdiv)
		{
			profile = i;
			break;
		}
	}

	return profile;
}

