/*----------------------------------------------------------------------------
 *      balongv7r2 m3 pm
 *----------------------------------------------------------------------------
 *      Name:    pm.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of balongv7r2 PWR.
 *---------------------------------------------------------------------------*/

#include <ARMCM3.h>
#include "console.h"
#include "printk.h"

#include "osl_types.h"
#include "osl_bio.h"
#include "osl_irq.h"

#include "m3_pm.h"
#include "pm_api.h"
#include "drv_ipc_enum.h"
#include "ipc_balong.h"
#include "drv_nv_def.h"
#include "drv_nv_id.h"
#include "bsp_nvim.h"
#include "m3_cpufreq.h"
#include "bsp_dsp.h"
#include "bsp_icc.h"
#include "bsp_hardtimer.h"
#include "wdt_balong.h"
/*lint --e{40, 63, 409, 958} */
static void pm_nv_init(void);
static void pm_ipc_init(void);
static void pm_wsrc_init(void);
static void IntIpcAcoreHandler(u32 irq);
static void IntIpcCcoreHandler(u32 irq);
static void IntIpcHiFiHandler(u32 irq);
static irqreturn_t IntAcoreHandler(int irq, void* data);
static irqreturn_t IntCcoreHandler(int irq, void* data);
static void pm_appa9_nvic_enable(void);
static void pm_appa9_nvic_disable(void);
static void pm_mdma9_nvic_enable(void);
static void pm_mdma9_nvic_disable(void);
static void pm_hifi_nvic_enable(void);
static void pm_hifi_nvic_disable(void);
static void thread_pm (void const *arg);
s32 pm_wakeup_acore(void);
s32 pm_wakeup_ccore(void);
s32 pm_wsrc_set_init(void);


osMailQDef(pm_mail, 32, T_PM_MAIL);	/*lint !e133 !e64*/
osMailQId  pm_mail;

osThreadId thread_pm_id;
osThreadDef(thread_pm, osPriorityHigh, 1, 512);/*lint !e133 */


T_PM_ST gPmSt;
T_PM_BAKEUP_ST gPmBakeupSt;
T_PM_COUNT_ST gPmCountSt;
T_PM_ERROR_ST gPmErrorSt;
DRV_NV_PM_TYPE gPmNvSt;


u32 gAcoreWsrc[]={M3_IPCM_APP_INT0, M3_IPCM_APP_INT1, M3_IPF_INT1, \
				  M3_GPIO_GROUP0_INT, M3_RTC_INT, M3_TIMER1_INT, \
				  M3_TIMER18_INT, M3_PMU_INT};
u32 gCcoreWsrc[]={M3_IPCM_MDM_INT0, M3_IPCM_MDM_INT1, M3_IPF_INT0, \
				  M3_TIMER2_INT, M3_TIMER9_INT,M3_TIMER17_INT,  \
				  M3_TIMER19_INT, M3_TIMER21_INT, M3_TIMER22_INT, \
				  M3_TDS_DRX_ARM_WAKEUP_INT, M3_LTE_ARM_WAKEUP_INT, M3_W_ARM_INT_02, \
				  M3_G1_INT_BBP_TO_CPU_32K, M3_G2_INT_BBP_TO_CPU_32K};
u32 gMcoreWsrc[]={M3_TIMER10_INT};
u32 gHiFiWsrc[]={M3_IPCM_INT0_4};

#define PM_ACORE_WSRC_NUM          (sizeof(gAcoreWsrc) / sizeof(u32))
#define PM_CCORE_WSRC_NUM          (sizeof(gCcoreWsrc) / sizeof(u32))
#define PM_MCORE_WSRC_NUM          (sizeof(gMcoreWsrc) / sizeof(u32))
#define PM_HIFI_WSRC_NUM           (sizeof(gHiFiWsrc) / sizeof(u32))

void pm_init(void)
{
	memset((void*)&gPmSt, 0x0, sizeof(T_PM_ST));
	memset((void*)&gPmBakeupSt, 0x0, sizeof(DRV_NV_PM_TYPE));
	memset((void*)&gPmCountSt, 0x0, sizeof(T_PM_COUNT_ST));
	memset((void*)&gPmErrorSt, 0x0, sizeof(T_PM_ERROR_ST));
	memset((void*)&gPmNvSt, 0x0, sizeof(DRV_NV_PM_TYPE));
	memset((void*)PWR_SRAM_DEBUG_ADDR, 0x0, PWR_SRAM_DEBUG_SIZE);
	gPmSt.u32SleepFlag = 0;
	gPmSt.u32VoteMap = 0xFFFFFFFF;
	gPmSt.u32VoteMask = PM_SLEEP_VOTE_MASK;

	pm_nv_init();
	pm_config_init();

	pm_mail = osMailCreate(osMailQ(pm_mail), NULL);

	thread_pm_id = osThreadCreate (osThread (thread_pm), NULL);
	if (thread_pm_id == NULL)
	{
		printk(" thread_pm create error\n");
	}
	pm_ipc_init();

	pm_wsrc_init();
    pm_wsrc_set_init();
}

static void pm_nv_init(void)
{
	u32 ret = 0;
	ret = bsp_nvm_read(NV_ID_DRV_PM, (u8 *)&gPmNvSt, sizeof(DRV_NV_PM_TYPE));
	if(ret != NV_OK)
	{
		printk("pm_nv_init read error\n");
	}
}

static void pm_ipc_init(void)
{
	if(bsp_ipc_int_connect(IPC_MCU_INT_SRC_ACPU_PD, IntIpcAcoreHandler, 0) != OK)
	{
		printk("\r\npm_ipc_init connect acpu error\n");
	}

	if(bsp_ipc_int_enable(IPC_MCU_INT_SRC_ACPU_PD) != OK)
	{
		printk("\r\npm_ipc_init enable acpu error\n");
	}

	if(bsp_ipc_int_connect(IPC_MCU_INT_SRC_CCPU_PD, IntIpcCcoreHandler, 0) != OK)
	{
		printk("\r\npm_ipc_init connect ccpu error\n");
	}

	if(bsp_ipc_int_enable(IPC_MCU_INT_SRC_CCPU_PD) != OK)
	{
		printk("\r\npm_ipc_init enable ccpu error\n");
	}

	if(bsp_ipc_int_connect(IPC_MCU_INT_SRC_HIFI_PD, IntIpcHiFiHandler, 0) != OK)
	{
		printk("\r\npm_ipc_init connect hifi error\n");
	}

	if(bsp_ipc_int_enable(IPC_MCU_INT_SRC_HIFI_PD) != OK)
	{
		printk("\r\npm_ipc_init enable hifi error\n");
	}
}

static void pm_wsrc_init(void)
{
	u32 i = 0;

	for(i = 0; i < PM_ACORE_WSRC_NUM; i++)
	{
		pm_appa9_wsrc_set(gAcoreWsrc[i]);
	}

	for(i = 0; i < PM_CCORE_WSRC_NUM; i++)
	{
		pm_mdma9_wsrc_set(gCcoreWsrc[i]);
	}

	for(i = 0; i < PM_HIFI_WSRC_NUM; i++)
	{
		pm_hifi_wsrc_set(gHiFiWsrc[i]);
	}

	for(i = 0; i < PM_MCORE_WSRC_NUM; i++)
	{
		pm_mcu_wsrc_set(gMcoreWsrc[i]);
	}
}

static void IntIpcAcoreHandler(u32 irq)
{
	T_PM_MAIL  *smail;

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_ACORE_SLEEP;
	osMailPut(pm_mail, smail);
}

static void IntIpcCcoreHandler(u32 irq)
{
	T_PM_MAIL  *smail;

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_CCORE_SLEEP;
	osMailPut(pm_mail, smail);
}

static void IntIpcHiFiHandler(u32 irq)
{
	T_PM_MAIL  *smail;

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_HIFI_SLEEP;
	osMailPut(pm_mail, smail);
}

static irqreturn_t IntAcoreHandler(int irq, void* data)
{
	T_PM_MAIL  *smail;

	pm_appa9_nvic_disable();

	if(gPmCountSt.u32PrintIrqWsrcFlag)
	{
		printk("IntAcoreHandler irq = %d\n", get_irq());
	}

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_ACORE_WAKE;
	osMailPut(pm_mail, smail);

	return IRQ_HANDLED;
}

static irqreturn_t IntCcoreHandler(int irq, void* data)
{
	T_PM_MAIL  *smail;
    u32 tmp = 0;

	pm_mdma9_nvic_disable();

	if(gPmCountSt.u32PrintIrqWsrcFlag)
	{
		printk("IntCcoreHandler irq = %d\n", get_irq());
	}
    tmp = readl(SHM_TIMESTAMP_ADDR);
    if(tmp == 1)
    {
        writel(*(u32*)(PWR_SRAM_TIMER0), SHM_TIMESTAMP_ADDR + 0x10);
        writel(*(u32*)(PWR_SRAM_TIMER1), SHM_TIMESTAMP_ADDR + 0x14);
        writel(*(u32*)(PWR_SRAM_TIMER2), SHM_TIMESTAMP_ADDR + 0x18);
        writel(*(u32*)(PWR_SRAM_TIMER3), SHM_TIMESTAMP_ADDR + 0x1C);
        writel(*(u32*)(PWR_SRAM_TIMER4), SHM_TIMESTAMP_ADDR + 0x20);
        writel(*(u32*)(PWR_SRAM_TIMER5), SHM_TIMESTAMP_ADDR + 0x24);
        writel(*(u32*)(PWR_SRAM_TIMER6), SHM_TIMESTAMP_ADDR + 0x28);
        writel(*(u32*)(PWR_SRAM_TIMER7), SHM_TIMESTAMP_ADDR + 0x2C);
        writel(*(u32*)(PWR_SRAM_TIMER8), SHM_TIMESTAMP_ADDR + 0x30);
        writel(*(u32*)(PWR_SRAM_TIMER9), SHM_TIMESTAMP_ADDR + 0x34);
        writel(bsp_get_slice_value(), SHM_TIMESTAMP_ADDR + 0x38);
    }

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_CCORE_WAKE;
	osMailPut(pm_mail, smail);

	return IRQ_HANDLED;
}

static irqreturn_t IntHiFiHandler(int irq, void* data)
{
	T_PM_MAIL  *smail;

	pm_hifi_nvic_disable();

	if(gPmCountSt.u32PrintIrqWsrcFlag)
	{
		printk("IntHiFiHandler irq = %d\n", get_irq());
	}

	smail = osMailCAlloc(pm_mail, 0);
	smail->type = PM_TYEP_HIFI_WAKE;
	osMailPut(pm_mail, smail);

	return IRQ_HANDLED;
}


void pm_appa9_wsrc_set(u32 lvl)
{
	u32 groupnum = 0;
	u32 bitnum = 0;

	if(lvl < 16)
	{
		return;
	}

	groupnum = (lvl-16)/32;
	bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32AcoreWsrc[groupnum] |= (u32)0x1<<bitnum;

	pm_set_wakeup_reg(groupnum, bitnum);

	(void)request_irq(lvl, IntAcoreHandler, IRQF_DISABLED, "acore irq", 0);/*lint !e713*/
}
/*lint -save -e737*/
static void pm_appa9_nvic_enable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32AcoreWsrc[i];
		NVIC->ICPR[i] = tmp;
		NVIC->ISER[i] = tmp;
	}
}

static void pm_appa9_nvic_disable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32AcoreWsrc[i];
		NVIC->ICER[i] = tmp;
	}
}

void pm_mdma9_wsrc_set(u32 lvl)
{
	u32 groupnum = 0;
	u32 bitnum = 0;

	if(lvl < 16)
	{
		return;
	}

	groupnum = (lvl-16)/32;
	bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32CcoreWsrc[groupnum] |= (u32)0x1<<bitnum;

	pm_set_wakeup_reg(groupnum, bitnum);

	(void)request_irq(lvl, IntCcoreHandler, IRQF_DISABLED, "ccore irq", 0);/*lint !e713*/
}

static void pm_mdma9_nvic_enable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32CcoreWsrc[i];
		NVIC->ICPR[i] = tmp;
		NVIC->ISER[i] = tmp;
	}
}

static void pm_mdma9_nvic_disable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32CcoreWsrc[i];
		NVIC->ICER[i] = tmp;
	}
}

void pm_hifi_wsrc_set(u32 lvl)
{
	u32 groupnum = 0;
	u32 bitnum = 0;

	if(lvl < 16)
	{
		return;
	}

	groupnum = (lvl-16)/32;
	bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32HiFiWsrc[groupnum] |= (u32)0x1<<bitnum;

	pm_set_wakeup_reg(groupnum, bitnum);

	(void)request_irq(lvl, IntHiFiHandler, IRQF_DISABLED, "hifi irq", 0);/*lint !e713*/
}

static void pm_hifi_nvic_enable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32HiFiWsrc[i];
		NVIC->ICPR[i] = tmp;
		NVIC->ISER[i] = tmp;
	}
}

static void pm_hifi_nvic_disable(void)
{
	u32 tmp = 0;
	u32 i = 0;

	for(i = 0; i < 4; i++)
	{
		tmp = gPmSt.u32HiFiWsrc[i];
		NVIC->ICER[i] = tmp;
	}
}
/*lint -restore +e737*/
void pm_mcu_wsrc_set(u32 lvl)
{
	u32 groupnum = 0;
	u32 bitnum = 0;

	if(lvl < 16)
	{
		return;
	}

	groupnum = (lvl-16)/32;
	bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32McoreWsrc[groupnum] |= (u32)0x1<<bitnum;

	pm_set_wakeup_reg(groupnum, bitnum);
}

u32 pm_mcu_uart_wsrc_flag(void)
{
    u32 lvl = M3_UART0_INT;
    u32 groupnum = (lvl-16)/32;
	u32 bitnum = (lvl-16) % 32;

    if((gPmSt.u32McoreWsrc[groupnum] & ((u32)0x1<<bitnum)) > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void pm_appa9_wsrc_unset(u32 lvl)
{
	u32 groupnum = 0;
	u32 bitnum = 0;

	if(lvl < 16)
	{
		return;
	}

	groupnum = (lvl-16)/32;
	bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */

	gPmSt.u32AcoreWsrc[groupnum] &= ~((u32)0x1<<bitnum);
    pm_unset_wakeup_reg(groupnum, bitnum);

	free_irq(lvl, 0);/*lint !e713*/
}

void pm_mdma9_wsrc_unset(u32 lvl)
{
    u32 groupnum = 0;
    u32 bitnum = 0;

    if(lvl < 16)
    {
        return;
    }

    groupnum = (lvl-16)/32;
    bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32CcoreWsrc[groupnum] &= ~((u32)0x1<<bitnum);
    pm_unset_wakeup_reg(groupnum, bitnum);

	free_irq(lvl, 0);/*lint !e713*/
}

void pm_hifi_wsrc_unset(u32 lvl)
{
    u32 groupnum = 0;
    u32 bitnum = 0;

    if(lvl < 16)
    {
        return;
    }

    groupnum = (lvl-16)/32;
    bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32HiFiWsrc[groupnum] &= ~((u32)0x1<<bitnum);
    pm_unset_wakeup_reg(groupnum, bitnum);

	free_irq(lvl, 0);/*lint !e713*/
}

void pm_mcu_wsrc_unset(u32 lvl)
{
    u32 groupnum = 0;
    u32 bitnum = 0;

    if(lvl < 16)
    {
        return;
    }

    groupnum = (lvl-16)/32;
    bitnum = (lvl-16) % 32;

	/* 通过记录的全局变量的值，配置寄存器 */
	gPmSt.u32McoreWsrc[groupnum] &= ~((u32)0x1<<bitnum);
    pm_unset_wakeup_reg(groupnum, bitnum);
}


/* pm流程 */
static void thread_pm (void const *arg)
{
	T_PM_MAIL  *rmail;
	osEvent  evt;
	int irqlock;

	for(;;)
	{
		/*lint --e{569 } */
		evt = osMailGet(pm_mail, osWaitForever);        // wait for mail
		if (evt.status == osEventMail)
		{
      	    rmail = evt.value.p;
			local_irq_save(irqlock);

			/* votemap bit 1:no sleep     0:sleep */
			//printk("\r\nthread_pm get mail : 0x%x\n", rmail->type);
			switch(rmail->type)
			{
				case PM_TYEP_ACORE_SLEEP:
					if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_ACORE))
					{
						gPmSt.u32VoteMap &= ~(0x1 << PM_PWR_VOTE_ACORE);
						pm_appa9_pdown();
						bsp_wdt_stop_awdt();
						pm_appa9_nvic_enable();
                        cpufreq_set_sleepflag(CPUFREAQ_ACORE, 1);
						gPmCountSt.u32AcoreSleepTimes++;
					}
					break;
				case PM_TYEP_CCORE_SLEEP:
					if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_CCORE))
					{
						gPmSt.u32VoteMap &= ~(0x1 << PM_PWR_VOTE_CCORE);
					  	pm_mdma9_pdown();
						bsp_wdt_stop_cwdt();
						pm_mdma9_nvic_enable();
                        cpufreq_set_sleepflag(CPUFREAQ_CCORE, 1);
                        //cpufreq_change_ccorelockflag(0);
						gPmCountSt.u32CcoreSleepTimes++;
					}
					break;
				case PM_TYEP_HIFI_SLEEP:
					if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_HIFI))
					{
						gPmSt.u32VoteMap &= ~(0x1 << PM_PWR_VOTE_HIFI);
                        (void)bsp_hifi_stop();
					  	pm_hifi_pdown();
                        pm_hifi_pll_close();
						pm_hifi_nvic_enable();
						gPmCountSt.u32HiFiSleepTimes++;
					}
					break;
				case PM_TYEP_ASET_WSRC:
					pm_appa9_wsrc_set(rmail->addr);
					break;
				case PM_TYEP_CSET_WSRC:
					pm_mdma9_wsrc_set(rmail->addr);
					break;
				case PM_TYEP_ACORE_WAKE:
					if(!(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_ACORE)) )
					{
						gPmSt.u32VoteMap |= 0x1 << PM_PWR_VOTE_ACORE;
						bsp_wdt_restart_awdt();
						pm_appa9_pup();
                        cpufreq_set_sleepflag(CPUFREAQ_ACORE, 0);
					}
					break;
				case PM_TYEP_CCORE_WAKE:
					if(!(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_CCORE)) )
					{
						gPmSt.u32VoteMap |= 0x1 << PM_PWR_VOTE_CCORE;
						pm_dfs_bak();
                        //cpufreq_change_ccorelockflag(1);
                        bsp_wdt_restart_cwdt();
						pm_mdma9_pup();
                        cpufreq_set_sleepflag(CPUFREAQ_CCORE, 0);
					}
					break;
				case PM_TYEP_HIFI_WAKE:
					if(!(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_HIFI)) )
					{
						gPmSt.u32VoteMap |= 0x1 << PM_PWR_VOTE_HIFI;
                        pm_hifi_pll_open();
						pm_hifi_pup();
                        (void)bsp_hifi_restore();
                        (void)bsp_hifi_run();
					}
					break;
				case PM_TYEP_AUNSET_WSRC:
					pm_appa9_wsrc_unset(rmail->addr);
					break;
				case PM_TYEP_CUNSET_WSRC:
					pm_mdma9_wsrc_unset(rmail->addr);
					break;
				default:
					break;
			}
			local_irq_restore(irqlock);
			osMailFree(pm_mail, rmail);                      // free memory allocated for mail
		}
		else
		{
			printk("\r\n thread_pm no mail \n");
		}
	}
}
/* vote:ccore/acore/hifi    flag:  0 sleep/  other wakeup */
void pm_vote(u32 vote, u32 flag)
{
    int irqlock;
    local_irq_save(irqlock);

    if(0 == flag)
    {
        gPmSt.u32VoteMap &= ~((u32)0x1 << vote);
    }
    else
    {
        gPmSt.u32VoteMap |= (u32)0x1 << vote;
    }
    local_irq_restore(irqlock);

}
u32 pm_get_deepsleep_status(void)
{
    if((!(gPmSt.u32VoteMap & gPmSt.u32VoteMask)) && (1 == gPmNvSt.deepsleep_flag))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void pm_print_debug(void)
{
	printk("\rPWR_SRAM_DEBUG_ADDR: 0x%x\n", PWR_SRAM_DEBUG_ADDR);

	printk("\rnormalwfi_flag:%d\n", gPmNvSt.normalwfi_flag);
	printk("\rdeepsleep_flag:%d\n", gPmNvSt.deepsleep_flag);
	printk("\rbuck3off_flag:%d\n", gPmNvSt.buck3off_flag);
	printk("\rperidown_flag:%d\n\n", gPmNvSt.peridown_flag);

	printk("\rnormalwfi time:%d\n", gPmCountSt.u32NormalWfiTimes);
	printk("\rdeepsleep time:%d\n", gPmCountSt.u32DeepSleepTimes);
	printk("\rMcuTimerIntTimes:%d\n", gPmCountSt.u32McuTimerIntTimes);
	printk("\racore sleep time:%d\n", gPmCountSt.u32AcoreSleepTimes);
	printk("\rccore sleep time:%d\n", gPmCountSt.u32CcoreSleepTimes);
	printk("\rHiFi sleep time:%d\n", gPmCountSt.u32HiFiSleepTimes);
	printk("\rPwrRegbakFlag:%d\n", gPmCountSt.u32PwrRegbakFlag);
	printk("\rWakeupDebugFlag:%d\n", gPmCountSt.u32WakeupDebugFlag);
	printk("\rPrintIrqWsrcFlag:%d\n\n", gPmCountSt.u32PrintIrqWsrcFlag);

	printk("\rDpmSuspendErrTimes:%d\n", gPmErrorSt.u32DpmSuspendErrTimes);
	printk("\rDpmResumeErrTimes:%d\n", gPmErrorSt.u32DpmResumeErrTimes);
	printk("\rAxiBakeupErrTimes:%d\n", gPmErrorSt.u32AxiBakeupErrTimes);
	printk("\rAxiRestoreErrTimes:%d\n\n", gPmErrorSt.u32AxiRestoreErrTimes);

	if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_ACORE))
	{
		printk("a wake\n");
	}
	else
	{
		printk("a sleep\n");
	}

	if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_CCORE))
	{
		printk("c wake\n");
	}
	else
	{
		printk("c sleep\n");
	}

	if(gPmSt.u32VoteMap & (0x1 << PM_PWR_VOTE_HIFI))
	{
		printk("h wake\n");
	}
	else
	{
		printk("h sleep\n");
	}

	pm_print_wsrc();
}

/* shangmianyou */
s32 pm_wakeup_acore(void)
{
	u32 channel_id = (ICC_CHN_MCORE_ACORE << 16) | MCORE_ACORE_FUNC_WAKEUP;
	s32 ret = 0;
    u32 pm_msg;
	u32 msglen = sizeof(pm_msg);

	ret = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)(&pm_msg), msglen);
	if(ret != msglen)/*lint !e737*/
	{
		//printk("PM icc_send_acore error\n");
		return -1;
	}
    return 0;
}

s32 pm_wakeup_ccore(void)
{
	u32 channel_id = (ICC_CHN_MCORE_CCORE << 16) | MCORE_CCORE_FUNC_WAKEUP;
	s32 ret = 0;
    u32 pm_msg;
	u32 msglen = sizeof(pm_msg);

	ret = bsp_icc_send(ICC_CPU_MODEM, channel_id, (u8*)(&pm_msg), msglen);
	if(ret != msglen)/*lint !e737*/
	{
		return -1;
	}
    return 0;
}

static s32 pm_wsrc_icc_read(u32 id , u32 len, void* context)
{
	s32 ret = 0;
	u8 data[32];
	T_PM_MAIL *smail;

    if(len > 32)
	{
        printk("pm_wsrc_icc_read len is err\n");
		return -1;
	}
	
	ret = bsp_icc_read(id, data, len);
	if(len != ret)/*lint !e737*/
	{
		return -1;
	}
	/*lint --e{569 } */
	smail = osMailCAlloc(pm_mail, osWaitForever);
	memcpy(smail, data, len);
	osMailPut(pm_mail, smail);

	return 0;
}


s32 pm_wsrc_set_init(void)
{
	s32 ret;
	u32 channel_id_set = 0;

	/* m3 acore icc */
	channel_id_set = (ICC_CHN_MCORE_ACORE << 16) | MCU_ACORE_WSRC;
	ret = bsp_icc_event_register(channel_id_set, pm_wsrc_icc_read, NULL, (write_cb_func)NULL, (void *)NULL);
	if(ret != ICC_OK)/*lint !e737*/
	{
		return -1;
	}

	/* m3 ccore icc */
	channel_id_set = (ICC_CHN_MCORE_CCORE << 16) | MCU_CCORE_WSRC;
	ret = bsp_icc_event_register(channel_id_set, pm_wsrc_icc_read, NULL, (write_cb_func)NULL, (void *)NULL);
	if(ret != ICC_OK)/*lint !e737*/
	{
		return -1;
	}
	return 0;
}


