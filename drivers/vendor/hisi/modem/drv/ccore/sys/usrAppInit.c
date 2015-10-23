/* usrAppInit.c - stub application initialization routine */

/* Copyright (c) 1998,2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01b,16mar06,jmt  Add header file to find USER_APPL_INIT define
01a,02jun98,ms   written
*/

/*
DESCRIPTION
Initialize user application code.
*/

#include <vxWorks.h>
#include <taskLib.h>
#include <selectLib.h>
#include <logLib.h>
#include "shellLib.h"
#include "dosFsLib.h"
#include "usrFsLib.h"
#include "private/shellLibP.h"

#include "product_config.h"
#include "config.h"
#include "bsp_ipc.h"
#include <bsp_icc.h>
#include "bsp_nvim.h"
#include "gpio_balong.h"
#include "osl_bio.h"
#include "bsp_abb.h"
#include "hi_uart.h"
#include "bsp_sram.h"
#include "bsp_softtimer.h"
#include "bsp_sci.h"
#include "bsp_om.h"
#include "bsp_i2c.h"
#include <bsp_dpm.h>
#include "ddm_phase.h"
#include "power_com.h"
#include "bsp_onoff.h"
#include "bsp_spi.h"
#include "bsp_pmu.h"
#include "bsp_pmu_hi6561.h"
#include "bsp_edma.h"
#include <bsp_dsp.h>
#include <bsp_tuner.h>
#include <bsp_hkadc.h>
#include "efuse_balong.h"
#include <bsp_cpufreq.h>
#include <bsp_regulator.h>
#include <bsp_wdt.h>
#include <bsp_version.h>
#include <bsp_audio.h>
#include <bsp_ipf.h>
#include <bsp_socp.h>
#include <bsp_hardtimer.h>
#include <bsp_leds.h>
#include "board_fpga.h"
#include "bsp_dump.h"
#include "bsp_amon.h"
#include <bsp_reset.h>
#include "bsp_dual_modem.h"
#include "bsp_pmu.h"
#include "rse_balong.h"

#ifdef CONFIG_MODULE_VIC
#include "vic_balong.h"
#endif

#ifdef CONFIG_BBP_INT
#include "bsp_bbp.h"
#endif

#ifdef CONFIG_CIPHER
extern s32 cipher_init();
extern s32 bsp_acc_init();
#endif
#ifdef CONFIG_CCORE_CPU_IDLE
#include "cpuidle_balong.h"
#endif
#ifdef CONFIG_BALONG_CCLK
#include "bsp_clk.h"
#endif
#ifdef CONFIG_ANTEN
#include "anten_balong.h"
#endif
#include <bsp_lowpower_mntn.h>

#ifdef K3_TIMER_FEATURE
extern void k3_timer_init();
#endif
#include <bsp_rfile.h>

extern long mailbox_init(void);
#ifdef FEATURE_TLPHY_MAILBOX
extern void bsp_mailbox_init(void);
#endif

extern int cshell_init();
extern BSP_S32 BSP_UDI_Init(VOID);
extern STATUS shellInterpPromptSet (SHELL_ID shellId,  const char * interp,  const char * promptFmt);
extern int tcxo_init_configure();
extern void adp_timer_init();
LOCAL void cpu_idle(void)
{
    while(1)
    {
#ifdef CONFIG_CCORE_CPU_IDLE
        cpuidle_idle_management();
#endif
    }
    return;
}

void BSP_DRV_Init()
{
#ifdef CONFIG_MODULE_VIC
    s32 ret = 0;
#endif

/***********************基础模块初始化***************************/
#ifdef CONFIG_BALONG_CCLK
    hi6930_clock_init();
#endif
#ifdef CONFIG_CCORE_PM
		 bsp_dpm_init();
#endif
#ifdef K3_TIMER_FEATURE
	k3_timer_init();
#endif
    adp_timer_init();
    timer_dpm_init();

    if(0 != BSP_UDI_Init())
        logMsg("BSP_UDI_Init fail\n",0,0,0,0,0,0);

    bsp_ipc_init();

	bsp_icc_init();

#ifdef CONFIG_K3V3_CLK_CRG /*CONFIG_K3V3_CLK_CRG*/
    gps_refclk_icc_read_cb_init();
#endif
	/* Cshell init if magic number is set to PRT_FLAG_EN_MAGIC_M */
#ifdef CONFIG_CSHELL
    if(0 != cshell_init())
    {
            logMsg("cshell init fail\n",0,0,0,0,0,0);
    }
#endif

#ifdef CONFIG_NVIM
     if(0 != bsp_nvm_init())
        logMsg("nv init fail\n",0,0,0,0,0,0);
#endif

    /* axi monitor监控初始化 */
    (void)bsp_amon_init();

	/*此初始化必须放置在MSP/OAM/PS初始化之前，请不要随意改动顺序*/
    tcxo_init_configure();

    if(0 != bsp_rfile_init())
        logMsg("rfile init fail\n",0,0,0,0,0,0);

	/* version inits */
    bsp_productinfo_init();

    hwspinlock_init();

    bsp_hkadc_init();

    bsp_version_init();
    bsp_lowpower_mntn_init();

#ifdef CONFIG_MODULE_VIC
    ret = bsp_vic_init();
    if(ret != OK)
    {
        logMsg("bsp_vic_init error\n", 0, 0, 0, 0, 0, 0);
    }
#endif

	(void)bsp_softtimer_init();

#ifdef CONFIG_BALONG_EDMA
    if(0 != bsp_edma_init())
    {
        logMsg("edma init fail \n",0,0,0,0,0,0);
    }
#endif

    /*C core init ipc module*/
    if(0 != socp_init())
        logMsg("socp init fail\n",0,0,0,0,0,0);

     if(0 != bsp_om_server_init())
        logMsg("om init fail\n",0,0,0,0,0,0);
	 if(0 != bsp_dual_modem_init())
	     logMsg("dual modem uart init fail\n",0,0,0,0,0,0);

/***********************外设模块初始化***************************/
    bsp_dsp_init();

#ifdef CONFIG_BBP_INT
	bbp_int_init();/*此处需要放在dsp初始化之后，放在pastar/abb之前*/
#endif

    bsp_spi_init();
    bsp_pmu_init();
	regulator_init();

#if defined(CONFIG_PMIC_HI6559)
    if(bsp_pa_rf_init())   /* 依赖于regulator_init */
    {
        logMsg("bsp_pa_rf_init fail\n",0,0,0,0,0,0);
    }
#endif

	/*init mipi*/
#ifdef CONFIG_MIPI
	bsp_mipi_init();
#endif

#ifdef CONFIG_TUNER
    bsp_tuner_init();
#endif

#ifdef CONFIG_PASTAR
	/*此函数的位置不可以向后移动，为pastar上电后，提供足够的稳定时间*/
    pmu_hi6561_init_phase1();
#endif

     if(0 != hi6930_wdt_init())
        logMsg("wdt init fail\n",0,0,0,0,0,0);

#ifdef CONFIG_CCORE_I2C

	if(0!=bsp_i2c_initial())
		logMsg("i2c init fail\n",0,0,0,0,0,0);
#endif


    if(0 != bsp_gpio_init())
        logMsg("gpio init fail\n",0,0,0,0,0,0);
#ifdef CONFIG_EFUSE
	if(0 != efuse_init())
	{
		logMsg("efuse init fail \n",0,0,0,0,0,0);
    }
#endif

#ifdef CONFIG_LEDS_CCORE
    if(0 != bsp_led_init())
    {
        logMsg("led init fail\n",0,0,0,0,0,0);
    }
#endif

/***********************通信支撑模块初始化***************************/
#ifdef CONFIG_CIPHER
    if(0 != cipher_init())
    {
        logMsg("cipher init fail \n",0,0,0,0,0,0);
    }
	if(0 != bsp_acc_init())
	{
		logMsg("acc init fail \n",0,0,0,0,0,0);
	}
#endif

#ifdef CONFIG_IPF
    if(0 != ipf_init())
        logMsg("ipf init fail\n",0,0,0,0,0,0);
#endif

#ifdef CONFIG_MODULE_BUSSTRESS
	 ipf_ul_stress_test_start(10);
#endif

#ifdef FEATURE_TLPHY_MAILBOX
    bsp_mailbox_init();
#endif

    mailbox_init();

#ifdef CONFIG_ANTEN
    if(0 != bsp_anten_init())
        logMsg("anten init fail.\n",0,0,0,0,0,0);
#endif

	bsp_sci_cfg_init();

    bsp_abb_init();

    bsp_on_off_init();

	cpufreq_init();

    /*初始化醒来的时间戳*/
    update_awake_time_stamp();

#ifdef CONFIG_CCORE_BALONG_PM
    balong_pm_init();
#endif

#ifdef CONFIG_AUDIO
    audio_init();
#endif

#ifdef CONFIG_BALONG_MODEM_RESET
	bsp_reset_init();
#endif

    (void)bsp_rf_rse_init();
#ifdef CONFIG_PASTAR
	/*勿动!此处需要放置在该函数最后，确保pastar上电后稳定后，进行初始化配置*/
	pmu_hi6561_init_phase2();
#endif

	(void)bsp_antn_sw_init();

}

void BSP_PromptSet()
{
    SHELL_ID    shellId;
    int taskId;

    /* Get the shell context */
    taskId = taskNameToId ("tShell0");
    shellId = shellFirst ();

    while (shellId != (SHELL_ID)0)
    {
        if (((SHELL_CTX *)shellId)->taskId == taskId)
            break;
        shellId = shellNext (shellId);
    }

    shellInterpPromptSet(shellId,"C","[C]->");
    taskPrioritySet(taskId, 9);/*优先级与V9一致*/
}

#ifdef CONFIG_K3V3_CLK_CRG /* CONFIG_K3V3_CLK_CRG */

/*MODEM时钟非用即关策略下的初始化状态*/
void modem_clk_init(void)
{
    /*hpm clk*/
    writel((unsigned int)0x1 << 26, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS3_OFFSET);

    /*aximon clk*/
    writel((unsigned int)0x1 << 22, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*hsuart clk*/
    writel((unsigned int)0x1 << 20, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*dsp vic clk*/
    //writel((unsigned int)0x1 << 16, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*uicc clk*/
    //writel((unsigned int)0x1 << 14, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*sci1 clk*/
    writel((unsigned int)0x1 << 13, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*sci0 clk*/
    writel((unsigned int)0x1 << 12, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*upacc clk*/
    //writel((unsigned int)0x1 << 11, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*cicom1 clk*/
    //writel((unsigned int)0x1 << 9, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*cicom0 clk*/
    //writel((unsigned int)0x1 << 8, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*mcpu mbist clk*/
    //writel((unsigned int)0x1 << 6, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);

    /*mcpu dbg clk*/
    //writel((unsigned int)0x1 << 5, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIS4_OFFSET);
}
#endif
/******************************************************************************
*
* usrAppInit - initialize the users application
*/
void usrAppInit (void)
{
#ifdef BSP_CONFIG_HI3630
#ifdef CONFIG_K3V3_CLK_CRG /* CONFIG_K3V3_CLK_CRG */
    modem_clk_init();
#endif
    bsp_utrace_init();
    bsp_utrace_resume();
#endif

	power_on_c_status_set(POWER_OS_OK);

    board_fpga_init();

#if defined (BSP_CONFIG_P531_FPGA) || defined(BSP_CONFIG_V7R2_SFT)
    //p531_fpga_test_init();
    p531_fpga_multi_mode_init();
    ddm_phase_boot_score("end p531_fpga_multi_mode_init",__LINE__);
#endif

    ddm_phase_boot_score("start BSP_DRV_Init",__LINE__);
    BSP_DRV_Init();

	power_on_c_status_set(POWER_BSP_OK);
    ddm_phase_boot_score("end BSP_DRV_Init",__LINE__);

#ifndef DRV_BUILD_SEPARATE
    ddm_phase_boot_score("start root",__LINE__);
    root();
	power_on_c_status_set(POWER_MSP_OK);   /* 根据返回值设置 */
    ddm_phase_boot_score("end root",__LINE__);
#endif

	/*原来taskdelay(2)，但是porting板上启动时会在BSP_PromptSet中没有退出，所以暂时修改为20*/
    taskDelay(20);
    BSP_PromptSet();

    taskSpawn ("cpuidle_task", 255, 0, 4096, (FUNCPTR)cpu_idle, 0,0,0,0,0,0,0,0,0,0);

    bsp_dump_init_task_name();
}


