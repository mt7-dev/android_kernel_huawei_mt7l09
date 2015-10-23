
/*lint --e{537} */

#include <stdio.h>

#include <product_config.h>

#include <osl_types.h>
#include <osl_bio.h>
#include <osl_sem.h>
#include <osl_wait.h>
#include <osl_spinlock.h>


#include <hi_base.h>
#include <hi_syssc.h>
#include <hi_syscrg.h>
#include <hi_pwrctrl.h>
#include <hi_dsp.h>

#include <bsp_sram.h>
#include <bsp_shared_ddr.h>

#include <bsp_om.h>
#include <bsp_edma.h>
#include <bsp_dpm.h>
#include <bsp_dsp.h>
#include <bsp_icc.h>
#include <bsp_version.h>
#include <bsp_hardtimer.h>
#include <bsp_memrepair.h>
#include <bsp_hw_spinlock.h>



/* #define BSP_BBE_STATE_CHECK */
#define BSP_DSP_WITH_EDMA

struct dsp_state
{
    spinlock_t spin_lock;
    u32 store_edma_channel_id;
    u32 restore_edma_channel_id_lte;
    u32 restore_edma_channel_id_tds;
    u32 dsp_power_on_count;
    u32 dsp_unreset_count;
    u32 dsp_clock_enable_count;
    u32 dsp_suspend_count;
    u32 dsp_resume_count;
    u32 dsp_sem_up_count;
    u32 dsp_sem_down_count;
    u32 dsp_run_count;
    u32 dsp_stop_count;
    u32 is_dsp_power_on:1;      /* 1 for power on */
    u32 is_dsp_clock_enable:1;  /* 1 for clock enable */
    u32 is_dsp_unreset: 1;      /* 1 for unreset */
    u32 is_bbe_power_on:1;      /* 1 for power on */
    u32 is_bbe_clock_enable:1;  /* 1 for clock enable */
    u32 is_bbe_unreset: 1;      /* 1 for unreset */
    u32 is_image_loaded:1;
    u32 is_image_dirty: 1;  /* is the image in DTCM different form DDR */
    u32 is_dsp_running: 1;
    u32 is_dsp_deep_sleep: 1;
    u32 is_image_need_store: 1;
    u32 is_image_has_store: 1;
    u32 is_msp_restoring:   1;
};

static struct dsp_state g_bbe16_state =
{
    .store_edma_channel_id    = 0,
    .restore_edma_channel_id_lte    = 0,
    .restore_edma_channel_id_tds    = 0,
    .dsp_power_on_count     = 1,
    .dsp_unreset_count  = 1,
    .dsp_clock_enable_count = 1,
    .dsp_suspend_count  = 0,
    .dsp_resume_count   = 0,
    .dsp_sem_up_count   = 0,
    .dsp_sem_down_count = 0,
    .dsp_run_count      = 0,
    .dsp_stop_count     = 0,
    .is_dsp_power_on    = 0,
    .is_dsp_clock_enable= 0,
    .is_dsp_unreset     = 0,
    .is_bbe_power_on    = 0,
    .is_bbe_clock_enable= 0,
    .is_bbe_unreset     = 0,
    .is_image_loaded    = 0,
    .is_image_dirty     = 0,
    .is_dsp_running     = 0,
    .is_dsp_deep_sleep  = 1,
    .is_image_need_store= 0,
    .is_image_has_store = 0,
    .is_msp_restoring   = 0,
};

int bsp_bbe_load_muti(void)
{
	/* bbe从深睡中恢复 */
    int ret = 0;
    u32 flag;

    flag = readl(SHM_MEM_DSP_FLAG_ADDR);
    if (flag != DSP_IMAGE_STATE_OK)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "DSP image not found\r\n");
        return -1;
    }

    ret |= bsp_bbe_clock_enable();
    ret |= bsp_bbe_unreset();

    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to setup bbe16\r\n");
        return -1;
    }

    ret = bsp_bbe_restore();

    ret |= bsp_bbe_wait_restore_ok();

    g_bbe16_state.is_dsp_deep_sleep = 0;

    return ret;
}

int bsp_bbe_run(void)
{
	/* 让bbe开始工作 */
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

#if defined(BSP_DSP_BBE16)
    set_hi_sc_ctrl13_bbe16_runstall(0);
#elif defined(BSP_DSP_DSP0)
    set_hi_sc_ctrl13_dsp0_runstall(0);
#elif defined(BSP_DSP_K3V3)
    set_hi_sc_ctrl13_runstall(0);
#endif

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_stop(void)
{
	/* 让bbe   停止工作 */
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

#if defined(BSP_DSP_BBE16)
    set_hi_sc_ctrl13_bbe16_runstall(1);
#elif defined(BSP_DSP_DSP0)
    set_hi_sc_ctrl13_dsp0_runstall(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_sc_ctrl13_runstall(1);
#endif

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_store(void)
{
    int ret = 0;
    /* save public image's data */
#ifdef BSP_DSP_WITH_EDMA
    g_bbe16_state.store_edma_channel_id =
        (u32)bsp_edma_channel_init(EDMA_PWC_LDSP_TCM, NULL, 0, 0);

    ret |= bsp_edma_channel_set_config(g_bbe16_state.store_edma_channel_id, EDMA_M2M,
        EDMA_TRANS_WIDTH_64, EDMA_BUR_LEN_16);

    ret |= bsp_edma_channel_2vec_start(g_bbe16_state.store_edma_channel_id,
        LPHY_BBE16_PUB_DTCM_LOAD_ADDR,
        BBE_DDR_PUB_DTCM_ADDR,
        LPHY_BBE16_PUB_DTCM_LOAD_SIZE, SZ_4K);
#else
    memcpy((void*)BBE_DDR_PUB_DTCM_ADDR,
           (void*)LPHY_BBE16_PUB_DTCM_LOAD_ADDR,
           LPHY_BBE16_PUB_DTCM_LOAD_SIZE);
#endif
    return ret;
}

int bsp_bbe_wait_store_ok(void)
{
    int ret = 0;
#ifdef BSP_DSP_WITH_EDMA
        while(!bsp_edma_channel_is_idle(g_bbe16_state.store_edma_channel_id)) ;

        ret = bsp_edma_channel_free(g_bbe16_state.store_edma_channel_id);
#endif
    return ret;
}

int bsp_bbe_restore(void)
{
    int ret = 0;
#ifdef BSP_DSP_WITH_EDMA
        g_bbe16_state.restore_edma_channel_id_lte =
            (u32)bsp_edma_channel_init(EDMA_PWC_LDSP_TCM, NULL, 0, 0);

        ret |= bsp_edma_channel_set_config(g_bbe16_state.restore_edma_channel_id_lte, EDMA_M2M,
            EDMA_TRANS_WIDTH_64, EDMA_BUR_LEN_16);

        ret |= bsp_edma_channel_2vec_start(g_bbe16_state.restore_edma_channel_id_lte,
            BBE_DDR_PUB_DTCM_ADDR,
            LPHY_BBE16_PUB_DTCM_LOAD_ADDR,
            LPHY_BBE16_PUB_DTCM_LOAD_SIZE, SZ_4K);

        g_bbe16_state.restore_edma_channel_id_tds =
            (u32)bsp_edma_channel_init(EDMA_PWC_TDSP_TCM, NULL, 0, 0);

        ret |= bsp_edma_channel_set_config(g_bbe16_state.restore_edma_channel_id_tds, EDMA_M2M,
            EDMA_TRANS_WIDTH_64, EDMA_BUR_LEN_16);

        ret |= bsp_edma_channel_2vec_start(g_bbe16_state.restore_edma_channel_id_tds,
            BBE_DDR_PUB_ITCM_ADDR,
            LPHY_BBE16_PUB_ITCM_LOAD_ADDR,
            LPHY_BBE16_PUB_ITCM_LOAD_SIZE, SZ_4K);
#else
        memcpy((void*)LPHY_BBE16_PUB_DTCM_LOAD_ADDR,
               (void*)BBE_DDR_PUB_DTCM_ADDR,
               LPHY_BBE16_PUB_DTCM_LOAD_SIZE);

        memcpy((void*)LPHY_BBE16_PUB_ITCM_LOAD_ADDR,
               (void*)BBE_DDR_PUB_ITCM_ADDR,
               LPHY_BBE16_PUB_ITCM_LOAD_SIZE);
#endif

    return ret;
}

int bsp_bbe_wait_restore_ok(void)
{
    int ret = 0;

#ifdef BSP_DSP_WITH_EDMA
        while(!bsp_edma_channel_is_idle(g_bbe16_state.restore_edma_channel_id_lte)) ;
        while(!bsp_edma_channel_is_idle(g_bbe16_state.restore_edma_channel_id_tds)) ;

        ret = bsp_edma_channel_free(g_bbe16_state.restore_edma_channel_id_lte);
        ret |= bsp_edma_channel_free(g_bbe16_state.restore_edma_channel_id_tds);
        if (ret)
        {
            ret = -1;
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to free edma channel\r\n");
        }
#endif

    return ret;
}

int bsp_bbe_clock_enable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* enable clock */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl9_tensi_bbe16_clk_en(1);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_clken4_dsp0_core_clk_en(1);
    set_hi_crg_clken4_dsp0_dbg_clk_en(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_clken4_bbe_core_clk_en(1);
    /* set_hi_crg_clken4_bbe_dbg_clk_en(1); */
#endif

    g_bbe16_state.is_bbe_clock_enable = 1;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_clock_disable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* disable clock */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl10_tensi_bbe16_clk_dis(1);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_clkdis4_dsp0_core_clk_dis(1);
    set_hi_crg_clkdis4_dsp0_dbg_clk_dis(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_clkdis4_dsp0_core_clk_dis(1);
    set_hi_crg_clkdis4_dsp0_dbg_clk_dis(1);
#endif

    g_bbe16_state.is_bbe_clock_enable = 0;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_unreset(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* unreset BBE16 */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl15_tensi_bbe16_srst_req(0);
    set_hi_crg_ctrl15_tensi_dbg_bbe16_srst_req(0);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_srstdis2_tensi_dsp0_core_srst_dis(1);
    set_hi_crg_srstdis2_tensi_dsp0_dbg_srst_dis(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_srstdis1_bbe_core_srst_dis(1);
    set_hi_crg_srstdis1_bbe_dbg_srst_dis(1);
#endif

    g_bbe16_state.is_bbe_unreset = 1;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_reset(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* reset BBE16 */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl15_tensi_bbe16_srst_req(1);
    set_hi_crg_ctrl15_tensi_dbg_bbe16_srst_req(1);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_srsten2_tensi_dsp0_core_srst_en(1);
    set_hi_crg_srsten2_tensi_dsp0_dbg_srst_en(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_srsten1_bbe_core_srst_en(1);
    set_hi_crg_srsten1_bbe_dbg_srst_en(1);
#endif

    g_bbe16_state.is_bbe_unreset = 0;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_power_on(void)
{
    unsigned long irq_flags;
    u32 board_type;
	int ret = 0;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    board_type = bsp_version_get_board_chip_type();
	if((HW_VER_PRODUCT_PORTING != board_type)
		&& (HW_VER_PRODUCT_SFT != board_type)
		&& (HW_VER_K3V3_FPGA != board_type))
	{
#if defined(BSP_DSP_BBE16)
        /* power on BBE16 */
        set_hi_pwr_ctrl7_bbe16_mtcmos_ctrl(1);
        while (!get_hi_pwr_stat1_bbe16_mtcmos_rdy()) ;

        udelay(30);

        /* disable ISO clamp */
        set_hi_pwr_ctrl6_bbe16_iso_ctrl(0);
#elif defined(BSP_DSP_DSP0)
        /* power on DSP0 */
        set_hi_pwr_ctrl6_dsp0_mtcmos_en(1);
        while(!get_hi_pwr_stat1_dsp0_mtcmos_rdy()) ;

        udelay(30);

        /* disable ISO clamp */
        set_hi_pwr_ctrl5_dsp0_iso_dis(1);
#elif defined(BSP_DSP_K3V3)
        /* power on BBE16 */
        set_hi_pwr_ctrl6_bbe16_mtcmos_ctrl_en(1);
        while (!get_hi_pwr_stat1_bbe16_mtcmos_rdy_stat()) ;

        udelay(30);

        /* disable ISO clamp */
        set_hi_pwr_ctrl5_bbe16_iso_ctrl_dis(1);

		/*memrepair*/
#ifdef MODEM_MEM_REPAIR

	ret = hwspin_lock_timeout(HW_MEMREPAIR_LOCKID,HW_SPINLOCK_WAIT_FOREVER);
	if(BSP_OK!=ret){
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "hwspin lock timeout !\n");
	}

	ret = bsp_modem_memrepair(MODEM_MEMREPAIR_DSP);
	if(BSP_OK!=ret){
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "memrepair fail !\n");
	}

	ret = hwspin_unlock(HW_MEMREPAIR_LOCKID);
	if(BSP_OK!=ret){
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "hwspin unlock timeout !\n");
	}
#endif

#endif
	}

    g_bbe16_state.is_dsp_power_on = 1;
    g_bbe16_state.is_bbe_power_on = 1;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return ret;
}

int bsp_bbe_power_off(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

#if defined(BSP_DSP_BBE16)
    /* enable ISO clamp */
    set_hi_pwr_ctrl6_bbe16_iso_ctrl(1);

    /* power down BBE16 */
    set_hi_pwr_ctrl7_bbe16_mtcmos_ctrl(0);
#elif defined(BSP_DSP_DSP0)
    /* enable ISO clamp */
    set_hi_pwr_ctrl4_dsp0_iso_en(1);

    /* power down DSP0 */
    set_hi_pwr_ctrl7_dsp0_mtcmos_dis(1);
#elif defined(BSP_DSP_K3V3)
    /* enable ISO clamp */
    set_hi_pwr_ctrl4_bbe16_iso_ctrl_en(1);

    /* power down DSP0 */
    set_hi_pwr_ctrl7_bbe16_mtcmos_ctrl_dis(1);
#endif

    g_bbe16_state.is_dsp_power_on = 0;
    g_bbe16_state.is_bbe_power_on = 0;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_bbe_is_clock_enable(void)
{
    return (int)g_bbe16_state.is_bbe_clock_enable;
}

int bsp_bbe_is_power_on(void)
{
    return (int)g_bbe16_state.is_bbe_power_on;
}

int bsp_bbe_is_tcm_accessible(void)
{
    int ret = 0;

    ret = (int)(g_bbe16_state.is_dsp_power_on &&
                g_bbe16_state.is_dsp_clock_enable &&
                g_bbe16_state.is_dsp_unreset &&
                g_bbe16_state.is_bbe_power_on &&
                g_bbe16_state.is_bbe_clock_enable &&
                g_bbe16_state.is_bbe_unreset);

    return ret;
}

int bsp_dsp_clock_enable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* enable clock */
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_clken4_dsp0_pd_clk_en(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_clken4_bbe_pd_clk_en(1);
#endif

    g_bbe16_state.is_dsp_clock_enable = 1;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_dsp_clock_disable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* disable clock */
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_clkdis4_dsp0_pd_clk_dis(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_clkdis4_dsp0_pd_clk_dis(1);
#endif

    g_bbe16_state.is_dsp_clock_enable = 0;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_dsp_unreset(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* unreset BBE16 */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl15_tensi_dps0_srst_req(0);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_srstdis2_tensi_dps0_pd_srst_dis(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_srstdis1_bbe_pd_srst_dis(1);
#endif

    g_bbe16_state.is_dsp_unreset = 1;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_dsp_reset(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

    /* reset BBE16 */
#if defined(BSP_DSP_BBE16)
    set_hi_crg_ctrl15_tensi_dps0_srst_req(1);
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_srsten2_tensi_dps0_pd_srst_en(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_srsten1_bbe_pd_srst_en(1);
#endif

    g_bbe16_state.is_dsp_unreset = 0;

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_dsp_pll_status(void)
{
#if defined(BSP_DSP_K3V3)
    return (int)get_hi_crg_dsppll_cfg0_pll_lock();
#else
    return 1;
#endif
}

int bsp_dsp_pll_enable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

	/* open dsp pll */
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_dfs2_ctrl3_pd(0);

    while (!get_hi_crg_dfs2_ctrl3_lock()) ;
#elif defined(BSP_DSP_K3V3)
    if(!get_hi_crg_dsppll_cfg0_pll_lock())
    {
        set_hi_crg_dsppll_cfg0_pll_en(1);
        set_hi_crg_dsppll_cfg0_pll_bp(0);

        while (!get_hi_crg_dsppll_cfg0_pll_lock()) ;

        set_hi_crg_dsppll_cfg1_pll_clk_gt(1);
        //set_hi_crg_clken3_bbe_refclk_en(1);
    }
#endif

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

int bsp_dsp_pll_disable(void)
{
    unsigned long irq_flags;

    spin_lock_irqsave(&(g_bbe16_state.spin_lock), irq_flags);

	/* close dsp dfs */
#if defined(BSP_DSP_BBE16)
    /* do nothing */
#elif defined(BSP_DSP_DSP0)
    set_hi_crg_dfs2_ctrl3_pd(1);
#elif defined(BSP_DSP_K3V3)
    set_hi_crg_dsppll_cfg1_pll_clk_gt(0);
    set_hi_crg_dsppll_cfg0_pll_en(0);
    set_hi_crg_dsppll_cfg0_pll_bp(1);
#endif

    spin_unlock_irqrestore(&(g_bbe16_state.spin_lock), irq_flags);

    return 0;
}

void bsp_bbe_refclk_enable(void)
{
#if defined(BSP_DSP_K3V3)
    set_hi_crg_clken3_bbe_refclk_en(1);
#endif
}

void bsp_bbe_refclk_disable(void)
{
#if defined(BSP_DSP_K3V3)
    set_hi_crg_clkdis3_bbe_refclk_dis(1);
#endif
}

/* 0:dsp pll   1:ccpu pll*/
void bsp_bbe_chose_pll(u32 flag)
{
#if defined(BSP_DSP_K3V3)
    u32 tmp = 0;
    if(0 == flag)
    {
        tmp = readl(HI_LP_PERI_CRG_REG_ADDR + 0xDC);
        /* first: bbe sel dsp pll */
        tmp &= ~0x30;
        tmp |= 0x300020;
        writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
        /* first :div [3:0] div=1 */
        tmp &= ~0xF;
        tmp |= 0xF0000;
        writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
    }
    else
    {
        tmp = readl(HI_LP_PERI_CRG_REG_ADDR + 0xDC);
        /* first :div [3:0] div=2 */
        tmp &= ~0xF;
        tmp |= 0xF0001;
        writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
        udelay(1);
        /* bbe sel ccpu pll */
        tmp &= ~0x30;
        tmp |= 0x300010;
        writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
    }
#endif
}
int bsp_msp_bbe_store(void)
{
    g_bbe16_state.is_image_need_store = 1;
    g_bbe16_state.is_dsp_deep_sleep = 1;
    g_bbe16_state.is_image_has_store = 0;

    return 0;
}
u32 bbe_restore_print = 0;
int bsp_msp_bbe_restore(void)
{
    int ret = 0;
    u32 stamp_start = 0;
    u32 stamp_end = 0;
    if(g_bbe16_state.is_image_has_store)
    {
        stamp_start = bsp_get_slice_value();
        ret = bsp_bbe_restore();
        g_bbe16_state.is_msp_restoring = 1;
        stamp_end = bsp_get_slice_value();
        if(bbe_restore_print)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "bbe restore: %d .\r\n",stamp_end-stamp_start);
        }
    }

    g_bbe16_state.is_image_need_store = 0;
    g_bbe16_state.is_dsp_deep_sleep = 0;

    return ret;
}

int bsp_msp_wait_edma_ok(void)
{
    int ret = 0;

    if (g_bbe16_state.is_msp_restoring)
    {
        ret = bsp_bbe_wait_restore_ok();
        g_bbe16_state.is_image_has_store = 0;
        g_bbe16_state.is_msp_restoring   = 0;
    }

    return ret;
}

#ifdef CONFIG_CCORE_PM
int bsp_bbe_dpm_prepare(struct dpm_device *dev)
{
	/* bbe下电前存储 */
    int ret = 0;

    if (g_bbe16_state.is_image_need_store && !g_bbe16_state.is_image_has_store)
    {
        ret = bsp_bbe_clock_enable();
        ret |= bsp_bbe_unreset();

        /* save public image's data */
        ret |= bsp_bbe_store();
    }
    return ret;
}

int bsp_bbe_dpm_suspend_late(struct dpm_device *dev)
{
	/* 在设备进入睡眠模式之前判断是否真的存储完成，如果完成，则将bbe下电 */
    int ret = 0;

    if (g_bbe16_state.is_image_need_store && !g_bbe16_state.is_image_has_store)
    {
        ret = bsp_bbe_wait_store_ok();
        g_bbe16_state.is_image_has_store = 1;
        ret |= bsp_bbe_reset();
        ret |= bsp_bbe_clock_disable();
    }

    ret |= bsp_dsp_reset();
    ret |= bsp_dsp_clock_disable();
#if defined(BSP_DSP_K3V3)
    bsp_bbe_refclk_disable();
#else
    ret |= bsp_dsp_pll_disable();
#endif

    if (g_bbe16_state.is_dsp_deep_sleep)
    {
        ret |= bsp_bbe_power_off();
    }

    return ret;
}

int bsp_bbe_dpm_resume_early(struct dpm_device *dev)
{
	/* dsp上电 */
    int ret = 0;

    if (g_bbe16_state.is_dsp_deep_sleep)
    {
        ret |= bsp_bbe_power_on();
    }

#if defined(BSP_DSP_K3V3)
    bsp_bbe_refclk_enable();
#else
    ret |= bsp_dsp_pll_enable();
#endif
    ret |= bsp_dsp_clock_enable();
    ret |= bsp_dsp_unreset();

    return ret;
}

int bsp_bbe_dpm_complete(struct dpm_device *dev)
{
	/* dsp上电后关bbe */
    int ret = 0;

    if (g_bbe16_state.is_image_need_store && !g_bbe16_state.is_image_has_store)
    {
        ret = bsp_bbe_wait_store_ok();
        ret |= bsp_bbe_reset();
        ret |= bsp_bbe_clock_disable();
    }

    return ret;
}

static struct dpm_device g_bbe_dpm_device={
    .device_name = "bbe_dpm",
    .prepare = bsp_bbe_dpm_prepare,
    .suspend_early = NULL,
    .suspend = NULL,
    .suspend_late = bsp_bbe_dpm_suspend_late,
    .resume_early = bsp_bbe_dpm_resume_early,
    .resume = NULL,
    .resume_late = NULL,
    .complete = bsp_bbe_dpm_complete,
};
#endif

void bsp_dsp_init(void)
{
    int ret = 0;

    spin_lock_init(&(g_bbe16_state.spin_lock));

    ret |= bsp_bbe_power_on();
    ret |= bsp_dsp_clock_enable();
    ret |= bsp_dsp_unreset();

    ret |= bsp_bbe_reset();
    ret |= bsp_bbe_clock_disable();

#ifdef CONFIG_CCORE_PM
    ret |= bsp_device_pm_add(&g_bbe_dpm_device);
#endif
    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to init dsp\n");
        return;
    }
}

/* print the statistics info of dsp */
int bsp_dsp_info(void)
{
    printf("BBE16 state:\r\n");

    printf("image state: %d %d %d\r\n", g_bbe16_state.is_image_loaded,
        g_bbe16_state.dsp_resume_count, g_bbe16_state.dsp_suspend_count);

    printf("isrun state: %d %d %d\r\n", g_bbe16_state.is_dsp_running,
        g_bbe16_state.dsp_run_count, g_bbe16_state.dsp_stop_count);

    printf("sem state: %d %d\r\n",
        g_bbe16_state.dsp_sem_up_count, g_bbe16_state.dsp_sem_down_count);

    printf("dirty state: %d\r\n", g_bbe16_state.is_image_dirty);

    return 0;
}


