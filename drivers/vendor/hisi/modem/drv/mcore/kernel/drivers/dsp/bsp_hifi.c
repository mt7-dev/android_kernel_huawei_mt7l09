

/*lint --e{537} */

#include <cmsis_os.h>

#include <product_config.h>

#include <osl_types.h>
#include <osl_bio.h>

#include <hi_base.h>
#include <hi_syssc.h>
#include <hi_syscrg.h>
#include <hi_pwrctrl.h>
#include <hi_dsp.h>
#include <drv_ipc_enum.h>
#include <bsp_sram.h>

#include <bsp_om.h>
#include <bsp_ipc.h>
#include <bsp_dsp.h>
#include <bsp_version.h>
#include <bsp_hardtimer.h>

#include <m3_pm.h>



#ifdef CONFIG_HIFI

extern void pm_hifi_pll_open(void);

int bsp_hifi_run(void)
{
    set_hi_sc_ctrl12_hifi_runstall(0);
    return 0;
}

int bsp_hifi_stop(void)
{
    set_hi_sc_ctrl12_hifi_runstall(1);
    return 0;
}

int bsp_hifi_power_on(void)
{
#ifdef BSP_DSP_BBE16
    /* power on HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_ctrl(1);
    while (!get_hi_pwr_stat1_hifi_mtcmos_rdy()) ;

    udelay(30);

    /* disable ISO clamp */
    set_hi_pwr_ctrl6_hifi_iso_ctrl(0);
#else
    /* power on HiFi */
    set_hi_pwr_ctrl6_hifi_mtcmos_en(1);
    while(!get_hi_pwr_stat1_hifi_mtcmos_rdy()) ;

    udelay(30);

    /* disable ISO clamp */
    set_hi_pwr_ctrl5_hifi_iso_dis(1);
#endif

    return 0;
}

int bsp_hifi_power_off(void)
{
#ifdef BSP_DSP_BBE16
    /* enable ISO clamp */
    set_hi_pwr_ctrl6_hifi_iso_ctrl(1);

    /* power down HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_ctrl(0);
#else
    /* enable ISO clamp */
    set_hi_pwr_ctrl4_hifi_iso_en(1);

    /* power down HiFi */
    set_hi_pwr_ctrl7_hifi_mtcmos_dis(1);
#endif

    return 0;
}

int bsp_hifi_clock_enable(void)
{
#ifdef BSP_DSP_BBE16
    /* enable clock */
    set_hi_crg_ctrl9_tensi_hifi_clk_en(1);
#else
    /* enable clock */
    set_hi_crg_clken3_hifi_clk_en(1);
    set_hi_crg_clken3_hifi_dbg_clk_en(1);
#endif

    return 0;
}

int bsp_hifi_clock_disable(void)
{
#ifdef BSP_DSP_BBE16
    /* disable clock */
    set_hi_crg_ctrl10_tensi_hifi_clk_dis(1);
#else
    /* disable clock */
    set_hi_crg_clkdis3_hifi_clk_dis(1);
    set_hi_crg_clkdis3_hifi_dbg_clk_dis(1);
#endif

    return 0;
}

int bsp_hifi_unreset(void)
{
#ifdef BSP_DSP_BBE16
    set_hi_crg_ctrl15_hifi_srst_req(0);
#else
	set_hi_crg_srstdis2_hifi_core_srst_dis(1);
    set_hi_crg_srstdis2_hifi_pd_srst_dis(1);
    set_hi_crg_srstdis2_hifi_dbg_srst_dis(1);
#endif

    return 0;
}

int bsp_hifi_reset(void)
{
#ifdef BSP_DSP_BBE16
    set_hi_crg_ctrl15_hifi_srst_req(1);
#else
	set_hi_crg_srsten2_hifi_core_srst_en(1);
    set_hi_crg_srsten2_hifi_pd_srst_en(1);
    set_hi_crg_srsten2_hifi_dbg_srst_en(1);
#endif

    return 0;
}

int bsp_hifi_restore(void)
{
    u32 i;
    u32 flag;
    u32 length;
    struct drv_hifi_sec_load_info *section_info = (struct drv_hifi_sec_load_info *)HIFI_SHARE_MEMORY_ADDR;

    if (section_info->sec_magic != HIFI_MEM_BEGIN_CHECK32_DATA)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "DSP image not found\r\n");
        return -1;
    }

    for (i = 0; i < section_info->sec_num; i++)
    {
        for (length = 0; length < section_info->sec_addr_info[i].sec_length; length += 4)
        {
            flag = readl(section_info->sec_addr_info[i].sec_source_addr + length);
            writel(flag, section_info->sec_addr_info[i].sec_dest_addr + length);
        }
    }

    return 0;
}

int bsp_hifi_store(void)
{
    return 0;
}

void bsp_hifi_resume(u32 value)
{
    int ret = 0;

    pm_hifi_pll_open();

    ret |= bsp_ipc_int_disable(IPC_MCU_INT_SRC_HIFI_PU);

    ret |= bsp_hifi_power_on();
    ret |= bsp_hifi_clock_enable();
    ret |= bsp_hifi_unreset();
    ret |= bsp_hifi_restore();
    ret |= bsp_hifi_run();

    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to resume hifi\r\n");
    }
    else
    {
        pm_vote(PM_PWR_VOTE_HIFI, 1);
    }
}

void bsp_hifi_suspend(u32 value)
{
    int ret = 0;

    ret |= bsp_hifi_stop();
    ret |= bsp_hifi_reset();
    ret |= bsp_hifi_clock_enable();
    ret |= bsp_hifi_power_off();

    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to suspend hifi\r\n");
    }
}

void bsp_dsp_init(void)
{
    int ret = 0;

    pm_vote(PM_PWR_VOTE_HIFI, 0);

    ret |= bsp_ipc_int_connect(IPC_MCU_INT_SRC_HIFI_PU, bsp_hifi_resume, 0);

    ret |= bsp_ipc_int_enable(IPC_MCU_INT_SRC_HIFI_PU);
    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to init hifi\r\n");
        return;
    }
}

#else

void bsp_dsp_init(void)
{
    pm_vote(PM_PWR_VOTE_HIFI, 0);
}

int bsp_hifi_restore(void)
{
    return 0;
}

int bsp_hifi_run(void)
{
    return 0;
}

int bsp_hifi_stop(void)
{
    return 0;
}

#endif


