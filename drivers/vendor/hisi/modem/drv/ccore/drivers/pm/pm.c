/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : pm.c */
/* Version       : 2.0 */
/* Created       : 2013-04-04*/
/* Last Modified : */
/* Description   :  pm drv*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
/*lint --e{537,438}*/
/* Warning 537: (Warning -- Repeated include  */
/* Warning 438: (Warning -- Last value assigned  not used) */
/*lint *** --e{537,713,732,701,438,830}*/
#include <taskLib.h>
#include <cacheLib.h>
#include <arch/arm/cacheArmArch7.h>
#include <string.h>
#include <tickLib.h>

#include "config.h"

#include <osl_types.h>
#include <osl_irq.h>
#include <osl_bio.h>

#include <hi_base.h>
#include <hi_syssc_interface.h>
#include <hi_syscrg_interface.h>
#include <hi_pwrctrl_interface.h>
#include <soc_interrupts_m3.h>
#include <drv_nv_def.h>
#include <drv_nv_id.h>
#include <drv_pm.h>

#include <bsp_om.h>
#include <bsp_gic_pm.h>
#include <bsp_dpm.h>
#include <bsp_cpuidle.h>
#include <bsp_wakelock.h>
#include <bsp_uart.h>
#include <bsp_dpm.h>
#include <bsp_lowpower_mntn.h>
#include <bsp_nvim.h>
#include <bsp_hardtimer.h>
#include <bsp_utrace.h>
#include <bsp_icc.h>
#include <bsp_wakelock.h>
#include <bsp_shared_ddr.h>
#include <bsp_pm.h>
#include <bsp_cpufreq.h>
#include <bsp_bbp.h>

#include <pintrl_balong.h>
#include <tcxo_balong.h>

#include "pm.h"


DRV_NV_PM_TYPE  g_nv_pm_config  = {0};      /* NV_ID_DRV_PM */
DRV_DRX_DELAY_STRU g_nv_drx_delay = {0};

typedef int suspend_state_t;
void pm_set_trace_level(u32 level);
s32 pm_check_irq_pending_status(void);
void pm_disable_all_gic(void);
void pm_enable_all_gic(void);
void set_pm_debug_flag(u32 gic_flag, u32 sleep_flag);
void balong_pm_enter(suspend_state_t state);
void balong_pm_init(void);
int pm_suspend(suspend_state_t state);
void pm_debug_clear_stamp(void);
void pm_debug_print_stamp(void);
void pm_wake_lock(void);
void pm_wake_unlock(void);
void pm_wakeup_init(void);
void pm_set_wake_src(enum pm_wake_src wake_src, T_PM_TYPE pm_type);
void pm_save_drx_timer_stamp(void);
int bsp_suspend(void);
void debug_balong_pm_init(void);
void set_pm_threshold(u32 deepsleep_Tth,u32 TLbbp_Tth);
void pm_print_debug_info(void);

void pm_set_trace_level(u32 level)
{
    (void)bsp_mod_level_set(BSP_MODU_PM, level);
    pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_mod_level_set(BSP_MODU_PM=%d, %d)\n",BSP_MODU_PM,level);
}

static inline void pm_timer_stamp(u32 stamp_addr)
{
    u32 curr_time;
    curr_time = readl(PM_STAMP_ADDR);
    writel(curr_time, stamp_addr);
}
#define DRX_PM_FLAG_FROM_M3     (SHM_TIMESTAMP_ADDR)        /* m3 置的 drx 标志位 , 读到 1 后置为 2 */
#define DRX_PM_FLAG_FOR_MSP     (SHM_TIMESTAMP_ADDR+0x4)    /* 给msp的标志位，msp读到后马上清*/
#define DRX_PM_TIMESTAMP_ADDR   (SHM_TIMESTAMP_ADDR+0x100)
#define DRX_PM_TIMESTAMP_SIZE   (4*16)

/* record drx timer if waked up by bbp_wakeup */
void pm_save_drx_timer_stamp(void)
{
    u32* time_save_addr;
    u32* drx_time_save_addr;

    time_save_addr = (u32*)STAMP_PWRUP_CODE_BEGIN;
    drx_time_save_addr = (u32*)DRX_PM_TIMESTAMP_ADDR;

    if(readl(DRX_PM_FLAG_FROM_M3)==1)
    {
        *(u32*)DRX_PM_FLAG_FROM_M3 = 2;
        *(u32*)DRX_PM_FLAG_FOR_MSP = 1;
        for(;time_save_addr<=(u32*)STAMP_AFTER_DPM_RESUME;)
        {
            *drx_time_save_addr = *time_save_addr;
            time_save_addr++;
            drx_time_save_addr++;
        }
        if(g_nv_drx_delay.drv_4 == 1)
        {
            pm_wake_lock();
        }
    }
}

/* save clk/mtcmos status before sleep */
void pm_save_clk_mtcmos_status(void)
{
    *(u32*)CHECK_STATUS_START = (u32)PM_STAMP_START_FLAG;
    *(u32*)CHECK_CRG_CLKSTAT1 = (u32)get_hi_crg_clkstat1();
    *(u32*)CHECK_CRG_CLKSTAT2 = (u32)get_hi_crg_clkstat2();
    *(u32*)CHECK_CRG_CLKSTAT3 = (u32)get_hi_crg_clkstat3();
    *(u32*)CHECK_CRG_CLKSTAT4 = (u32)get_hi_crg_clkstat4();
    *(u32*)CHECK_CRG_CLKSTAT5 = (u32)get_hi_crg_clkstat5();
    *(u32*)CHECK_PWR_STAT1 = (u32)get_hi_pwr_stat1();
}
/* print clk/mtcmos status saved */
void pm_print_clk_mtcmos_status(void)
{
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_STATUS_START =0x%x \n",  CHECK_STATUS_START);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_CRG_CLKSTAT1 =0x%x \n",  *(u32*)CHECK_CRG_CLKSTAT1);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_CRG_CLKSTAT2 =0x%x \n",  *(u32*)CHECK_CRG_CLKSTAT2);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_CRG_CLKSTAT3 =0x%x \n",  *(u32*)CHECK_CRG_CLKSTAT3);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_CRG_CLKSTAT4 =0x%x \n",  *(u32*)CHECK_CRG_CLKSTAT4);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_CRG_CLKSTAT5 =0x%x \n",  *(u32*)CHECK_CRG_CLKSTAT5);
    pm_printk(BSP_LOG_LEVEL_ERROR,"CHECK_PWR_STAT1    =0x%x \n",  *(u32*)CHECK_PWR_STAT1);
}

/* do not clear stamps. only set some start_flag  */
void pm_stamp_init(void)
{
    *(u32*)STAMP_START_ADDR = (u32)PM_STAMP_START_FLAG;
    *(u32*)STAMP_AFTER_WFI_NOP = (u32)PM_STAMP_START_FLAG;
    *(u32*)CHECK_STATUS_START = (u32)PM_STAMP_START_FLAG;
    writel(0, PM_ENTER_COUNT);
    writel(0, PM_DPM_FAIL_COUNT);
}

/*****************************************************************************
 函 数 名  : pm_check_irq_pending_status
 功能描述  : 检查中断状态
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
s32 pm_check_irq_pending_status(void)
{
    u32 i;
    u32 gic_icdispr = 0;
    unsigned long flags = 0;
    local_irq_save(flags);
    for (i = 0; i < CARM_GIC_ICDABR_NUM; i++)
    {
        gic_icdispr = (u32)readl(CARM_GIC_ICDISPR_OFFSET(i));/*lint !e572*/
        if (0 != gic_icdispr)
        {
            local_irq_restore(flags);
            return -1;
        }
    }
	local_irq_restore(flags);
    return 0;
}
/*****************************************************************************
 Function   : pm_disable_all_gic
 Description: disable gic, called in balong_pm_enter
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
static u32 gic_disable_bak[CARM_GIC_ICDABR_NUM];
void pm_disable_all_gic(void)
{
	u32 *gic_int_set_reg = (u32 *)(CARM_GIC_ICDISER);/*lint !e572*/
	u32 *gic_int_clear_reg = (u32 *)(CARM_GIC_ICDICER);/*lint !e572*/
    u32 i;
    for(i = 0; i < CARM_GIC_ICDABR_NUM; i++)
	{
        gic_disable_bak[i] = *gic_int_set_reg;
		*gic_int_clear_reg = 0xffffffff;
		gic_int_set_reg++;
		gic_int_clear_reg++;
	}
}

/*****************************************************************************
 Function   : pm_enable_all_gic
 Description: enable gic, called in balong_pm_enter
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
void pm_enable_all_gic(void)
{
	u32 *gic_int_set_reg = (u32 *)(CARM_GIC_ICDISER);/*lint !e572*/
    u32 i;

    for(i = 0; i < CARM_GIC_ICDABR_NUM; i++)
	{
		*gic_int_set_reg = gic_disable_bak[i];
		gic_int_set_reg++;
	}
}

struct pm_info
{
    u32 dpm_fail_count;
    u32 pm_enter_count;
    u32 pm_enter_asm_count;
    u32 pm_enter_wfi_count;
    u32 pm_out_count;
    u32 stamp_addr;
    u32 gic_flag;   /* 若不禁GIC 直接从wfi出来，测试整个流程。 */
    u32 sleep_flag; /* 0 - 走dpm cpu不睡眠；奇数 - cpu睡眠; 偶数 - cpu睡眠一次后置为0 ;*/
    u32 deepsleep_nv;
    u32 deepsleep_Tth_nv;
    u32 TLbbp_sleeptimer_Tth_nv;    /* 默认 10ms */
};

struct pm_info pm_ctrl_info =
{
    .dpm_fail_count = 0,
    .pm_enter_count = 0,
    .pm_enter_asm_count = 0,
    .pm_enter_wfi_count = 0,
    .pm_out_count = 0,
    .stamp_addr = DUMP_EXT_OM_DRX_CCORE_ADDR,
    .gic_flag = 1,
    .sleep_flag = 1,
    .deepsleep_nv = 0,
    .deepsleep_Tth_nv = 100,        /* 参考v7r1 105 ,v7r2可能比较小    */
    .TLbbp_sleeptimer_Tth_nv = 320, /* 32k */
};


/* ccore should close auxiliary-tcxo before sleep,
 or usb can not wakeup mcore from deepsleep */
/*set pwrctrl2  bit 21 bit23  zero*/

struct tcxo_suspend_ctrl
{
    u32 tcxo_is_suspended;
    u32 pwc2_abb_ch1_tcxo_en;
    u32 pwc2_pmu_ch1_tcxo_en;
};

static struct tcxo_suspend_ctrl tcxo_ctrl = {0};

static void tcxo_suspend(void)
{
    if(1==*(u32*)SRAM_RTT_SLEEP_FLAG_ADDR)
    { /*if dsp light sleep, auxiliary-tcxo should not be suspended*/
        tcxo_ctrl.tcxo_is_suspended = false;
        return;
    }
    else
    {
        tcxo_ctrl.tcxo_is_suspended = true;
        /*save pwrctrl2 and set bit 21 bit23  zero*/
        tcxo_ctrl.pwc2_abb_ch1_tcxo_en = hi_pwrctrl_get_pwr_ctrl2_abb_ch1_tcxo_en();
        tcxo_ctrl.pwc2_pmu_ch1_tcxo_en = hi_pwrctrl_get_pwr_ctrl2_pmu_ch1_tcxo_en();
        hi_pwrctrl_set_pwr_ctrl2_abb_ch1_tcxo_en(0);
        hi_pwrctrl_set_pwr_ctrl2_pmu_ch1_tcxo_en(0);
    }
}
static void tcxo_resume(void)
{
    if(false == tcxo_ctrl.tcxo_is_suspended)
    {
        return;
    }
    else
    {
        /*resume pwrctrl2 --- bit 21 bit23 */
        hi_pwrctrl_set_pwr_ctrl2_abb_ch1_tcxo_en(tcxo_ctrl.pwc2_abb_ch1_tcxo_en);
        hi_pwrctrl_set_pwr_ctrl2_pmu_ch1_tcxo_en(tcxo_ctrl.pwc2_pmu_ch1_tcxo_en);
    }
}

void drv_udelay (u32 ms, u32 flag)
{
    u32 start = 0;
    u32 end = 0;
    u32 tmp = 0;

    if(flag == 1)
    {
        start = bsp_get_slice_value();
        do
        {
            end = bsp_get_slice_value();
            tmp = end - start;
        }while(tmp<(ms*32));
    }
}

#ifdef INCLUDE_L2_CACHE
#define L2X0_CTRL	                0x100
#define L2X0_AUX_CTRL               0x104
#define L2X0_TAG_LATENCY_CTRL       0x108
#define L2X0_DATA_LATENCY_CTRL      0x10C
#define L2X0_PREFETCH_CTRL          0xF60
#define L2X0_POWER_CTRL             0xF80
unsigned int has_l2cache_flag;


struct l2x0_cache_regs
{
    u32 AUX_CTRL;
    u32 TAG_LATENCY_CTRL;
    u32 DATA_LATENCY_CTRL;
    u32 PREFETCH_CTRL;
    u32 POWER_CTRL;
};
struct l2x0_cache_regs l2x0_cache_back;


#include <arm_pbxa9.h>

#define L2_CTRL_BASE             HI_MDMA9_L2_REGBASE_ADDR_VIRT
#define L2_CACHE_ID              (L2_CTRL_BASE + 0x0)
#define L2_CACHE_TYPE            (L2_CTRL_BASE + 0x4)
#define L2_CONTROL               (L2_CTRL_BASE + 0x100)
#define L2_AUX_CONTROL           (L2_CTRL_BASE + 0x104)
#define L2_INT_MASK              (L2_CTRL_BASE + 0x214)
#define L2_INT_CLEAR             (L2_CTRL_BASE + 0x220)

#define L2_CACHE_SYNC            (L2_CTRL_BASE + 0x730)
#define L2_INVALIDATE_PA         (L2_CTRL_BASE + 0x770)
#define L2_INVALIDATE_WAY        (L2_CTRL_BASE + 0x77c)
#define L2_CLEAN_PA              (L2_CTRL_BASE + 0x7b0)
#define L2_CLEAN_WAY             (L2_CTRL_BASE + 0x7bc)
#define L2_CLEAN_INVALIDATE_PA   (L2_CTRL_BASE + 0x7f0)
#define L2_CLEAN_INVALIDATE_WAY  (L2_CTRL_BASE + 0x7fc)

#define L2_CONTROL_ENABLE        (0x1)
#define L2_WAY_ALL               (0xff) /* 8-ways */
#define L2_OPERATION_IN_PROGRESS (0x1)
#define L2_INT_MASK_ALL          (0x0)
#define L2_INT_CLEAR_ALL         (0x1ff)

#define L2_CACHE_SIZE            (0x80000)
#define L2_CACHE_LINE_SIZE       (0x20)
#define L2_CACHE_LINE_MASK       (L2_CACHE_LINE_SIZE - 1)


#define BIT_A9_FULL_LINE 3
#define BIT_A9_L1_PREFETCH 2
#define BIT_A9_L2_PREFETCH_HINTS 1

static void  set_a9_aux_ctrl( unsigned int bit , unsigned int bool)
{
/* c1 - 0 - c0 - 1 */
    unsigned int reg = 0;
    __asm__ __volatile__  ("mrc p15, 0, %0, c1, c0, 1" : "=r" (reg) );

    if(bool==1)
        { reg = reg|(0x1<<bit);}
    else
        { reg = reg &(~(0x1<<bit)) ;}
    __asm__ __volatile__  ("mcr p15, 0, %0, c1, c0, 1" : : "r" (reg));
    
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 

}

#define SCU_CTRL_OFFSET 0x0
#define SCU_IC_STANDBY      (0x1<<6)
#define SCU_STANDBY     (0x1<<5)
#define SCU_SPECUL_LINEFILL (0x1<<3)
#define SCU_SCU_ENABLE      (0x1<<0)

#define SCU_CTRL_CONFIG (SCU_IC_STANDBY|SCU_STANDBY|SCU_SPECUL_LINEFILL|SCU_SCU_ENABLE)

u32 scu_ctrl_reg_bak = 0;
extern void (* cache_sync_ops)(void);
void (* cache_sync_ops_save)(void);

LOCAL void scu_suspend(void)
{
    unsigned int scu_reg_base = 0;
    __asm__ __volatile__  ("mrc p15, 4, %0, c15, c0, 0" : "=r" (scu_reg_base) );
    scu_reg_base &= 0xffffe000; /* get scu reg base - bit[31:13]*/
    
    scu_ctrl_reg_bak = ARMA9CTX_REGISTER_READ(scu_reg_base+SCU_CTRL_OFFSET);

    ARMA9CTX_REGISTER_WRITE(scu_reg_base+SCU_CTRL_OFFSET, (scu_ctrl_reg_bak&(~SCU_CTRL_CONFIG)));
    __asm__ __volatile__ ("dsb" : : : "memory");
    __asm__ __volatile__ ("isb" : : : "memory"); 
   
}
LOCAL void scu_resume(void)
{
    unsigned int scu_reg_base = 0;
    __asm__ __volatile__  ("mrc p15, 4, %0, c15, c0, 0" : "=r" (scu_reg_base) );
    scu_reg_base &= 0xffffe000; /* get scu reg base - bit[31:13]*/

    ARMA9CTX_REGISTER_WRITE(scu_reg_base+SCU_CTRL_OFFSET, scu_ctrl_reg_bak);
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 
}




#define  L2CACHE_WITH_L1
/*
 *  hi6930_pm_disable_l2x0()/hi6930_pm_enable_l2x0() is designed to
 *  disable and enable l2-cache during Suspend-Resume phase
 */
void hi6930_pm_disable_l2x0(void)
{
#ifdef L2CACHE_WITH_L1
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 0); /*AUX_FULL_LINE_OF_ZERO*/
    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 0); /*  */
#endif
#if 1
    u32 l2x0_base = HI_MDMA9_L2_REGBASE_ADDR_VIRT;
	/* backup aux control register value */
    l2x0_cache_back.AUX_CTRL = readl(l2x0_base + L2X0_AUX_CTRL);
    l2x0_cache_back.TAG_LATENCY_CTRL = readl(l2x0_base + L2X0_TAG_LATENCY_CTRL) ; 
    l2x0_cache_back.DATA_LATENCY_CTRL = readl(l2x0_base + L2X0_DATA_LATENCY_CTRL);
    l2x0_cache_back.PREFETCH_CTRL  = readl(l2x0_base + L2X0_PREFETCH_CTRL);
    l2x0_cache_back.POWER_CTRL = readl(l2x0_base + L2X0_POWER_CTRL);

#endif
    //cacheArchL2CacheDisable();

	/* clear */
	ARMA9CTX_REGISTER_WRITE(L2_CLEAN_INVALIDATE_WAY, L2_WAY_ALL);
	
	/* poll state until the background clear operation	is complete */
	while (ARMA9CTX_REGISTER_READ(L2_CLEAN_INVALIDATE_WAY) & L2_WAY_ALL)
		__asm__ __volatile__ ("" ::: "memory");

	/* cache sync */
        ARMA9CTX_REGISTER_WRITE( l2x0_base + 0x730,0);

	while (ARMA9CTX_REGISTER_READ(L2_CACHE_SYNC) & 1)
		__asm__ __volatile__ ("" ::: "memory");

	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");

	 /* disable L2 cache */
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, 0x0);
	__asm__ __volatile__ ("dsb" : : : "memory");

    cache_sync_ops_save = cache_sync_ops;
    cache_sync_ops = NULL;
    
    scu_suspend();

}

void l2x0_test_print_regbaks(void)
{    
    pm_printk(BSP_LOG_LEVEL_ERROR,"l2cache regs: 0x%x,0x%x,0x%x,0x%x,0x%x,\n"\
        ,l2x0_cache_back.AUX_CTRL \
        ,l2x0_cache_back.TAG_LATENCY_CTRL \
        ,l2x0_cache_back.DATA_LATENCY_CTRL\
        ,l2x0_cache_back.PREFETCH_CTRL\
        ,l2x0_cache_back.POWER_CTRL );
}
void l2cache_test_set_regbaks(u32 arg1,u32 arg2,u32 arg3,u32 arg4,u32 arg5)
{
        l2x0_cache_back.AUX_CTRL  = arg1;
        l2x0_cache_back.TAG_LATENCY_CTRL  = arg2;
        l2x0_cache_back.DATA_LATENCY_CTRL  = arg3;
        l2x0_cache_back.PREFETCH_CTRL  = arg4;
        l2x0_cache_back.POWER_CTRL   = arg5;
}

LOCAL void sysL2CacheResume(void)
{

    u32 l2x0_base = HI_MDMA9_L2_REGBASE_ADDR_VIRT;

    /*     * 512KB L2 cache RAM. 8-way associativity. way-size is 64KB     */
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, 0x0); /* disable L2 cache */

    scu_resume();

	/* restore aux control register */
    writel( l2x0_cache_back.AUX_CTRL , l2x0_base + L2X0_AUX_CTRL);
    writel(l2x0_cache_back.TAG_LATENCY_CTRL , l2x0_base + L2X0_TAG_LATENCY_CTRL);
    writel(l2x0_cache_back.DATA_LATENCY_CTRL , l2x0_base + L2X0_DATA_LATENCY_CTRL);
    writel(l2x0_cache_back.PREFETCH_CTRL,l2x0_base + L2X0_PREFETCH_CTRL);
    writel(l2x0_cache_back.POWER_CTRL , l2x0_base + L2X0_POWER_CTRL);

    /*     * We use the default configuration, so here we do not need use the
     * auxiliary control register to configure associativity, way size and
     * latency of RAM access.     */

    /* invalidate the whole L2 cache through way operation  */
    ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_WAY, L2_WAY_ALL);

    /* poll state until the background invalidate operation  is complete */
    while (ARMA9CTX_REGISTER_READ(L2_INVALIDATE_WAY) & L2_WAY_ALL)
		__asm__ __volatile__ ("" ::: "memory");

    /* clear all pending interrupts */
    //ARMA9CTX_REGISTER_WRITE(L2_INT_CLEAR, L2_INT_CLEAR_ALL);
    /* mask all interrupts */
    //ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);
	
	/* cache sync */
        ARMA9CTX_REGISTER_WRITE( l2x0_base + 0x730,0);
    
	while (ARMA9CTX_REGISTER_READ(L2_CACHE_SYNC) & 1)
		__asm__ __volatile__ ("" ::: "memory");

	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");
	
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, L2_CONTROL_ENABLE); /* enable L2 cache */  
	
	while (ARMA9CTX_REGISTER_READ(L2_CACHE_SYNC) & 1)
		__asm__ __volatile__ ("" ::: "memory");
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");     

#ifdef L2CACHE_WITH_L1
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 1); /*AUX_FULL_LINE_OF_ZERO*/
    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 1); /*  */

#endif
    
    cache_sync_ops = cache_sync_ops_save;

}

void hi6930_pm_enable_l2x0(void)
{
#if 0
    u32 l2x0_base = HI_MDMA9_L2_REGBASE_ADDR_VIRT;
	/* disable cache */
	writel(0, l2x0_base + L2X0_CTRL);
	/* restore aux control register */
	writel(l2x0_cache_bak[0], l2x0_base + L2X0_AUX_CTRL);
	writel(l2x0_cache_bak[1], l2x0_base + L2X0_TAG_LATENCY_CTRL);
	writel(l2x0_cache_bak[2], l2x0_base + L2X0_DATA_LATENCY_CTRL);

	/* invalidate l2x0 cache */
	/* enable l2x0 cache */
	cacheArchL2CacheEnable();
	//mb();
	pm_printk(BSP_LOG_LEVEL_DEBUG, "L2 Cache enabled\r\n");
#endif
	//l2cache_test_set_regbaks(0x32040000,0x111,0x111,0x70000007,0);
	sysL2CacheResume();
}
#endif

void balong_pm_enter(suspend_state_t state)
{
	unsigned long flags = 0;

    UNUSED(state);

	local_irq_save(flags);

    pm_timer_stamp(STAMP_PM_ENTER_START);
    pm_ctrl_info.pm_enter_count++;
    writel(pm_ctrl_info.pm_enter_count, PM_ENTER_COUNT);

    if(pm_ctrl_info.sleep_flag)
    {
        if (0 == pm_check_irq_pending_status())
        {
    	    if(pm_ctrl_info.gic_flag)
                pm_disable_all_gic();
            pm_timer_stamp(STAMP_AFTER_DISABLE_GIC);

#ifdef INCLUDE_L2_CACHE
            if(has_l2cache_flag){
                hi6930_pm_disable_l2x0();
            }
#endif

            gic_suspend();
            pm_timer_stamp(STAMP_AFTER_BAK_GIC);

            bsp_utrace_suspend();
            pm_timer_stamp(STAMP_AFTER_UTRACE_SUSPEND);

            tcxo_suspend();
            pm_timer_stamp(STAMP_AFTER_TCXO_SUSPEND);

            pm_save_clk_mtcmos_status();

            bsp_dpm_powerdown_antn_config();
    		(void)modem_pintrl_config(MODEM_PIN_POWERDOWN);
            pm_timer_stamp(STAMP_AFTER_PIN_POWERDOWN);

            pm_ctrl_info.pm_enter_asm_count++;
            pm_asm_cpu_go_sleep();
            pm_timer_stamp(STAMP_SLEEP_ASM_OUT);

			(void)modem_pintrl_config(MODEM_PIN_NORMAL);
            pm_timer_stamp(STAMP_AFTER_PIN_NORMAL);

            tcxo_resume();
            pm_timer_stamp(STAMP_AFTER_TCXO_RESUME);

            bsp_utrace_resume();
            pm_timer_stamp(STAMP_AFTER_UTRACE_RESUME);

#ifdef INCLUDE_L2_CACHE
            if(has_l2cache_flag){
                hi6930_pm_enable_l2x0();
            }
#endif

            gic_resume();
            pm_timer_stamp(STAMP_AFTER_RSTR_GIC);
            if(pm_ctrl_info.gic_flag)
                pm_enable_all_gic();
            pm_timer_stamp(STAMP_AFTER_ENABLE_GIC);

            /*debug: 如果此flag为偶数，则pm流程只走一次*/
            pm_ctrl_info.sleep_flag = pm_ctrl_info.sleep_flag&1;
            drv_udelay(5, g_nv_drx_delay.drv_0);
        }
        else
        {
            cpufreq_set_max_freq();
        }
    }
    else
    {
        pm_ctrl_info.pm_enter_wfi_count++;
        WFI();
    }

    pm_ctrl_info.pm_out_count++;
    pm_timer_stamp(STAMP_PM_ENTER_END);
	local_irq_restore(flags);
}


/* 低功耗主流程 */

extern int sysClkRateGet(void);

int pm_suspend(suspend_state_t state)/**/
{
    u32 enter_time_stamp = 0;
    u32 out_time_stamp = 0;
    u32 pm_time_tick = 0;

    pm_timer_stamp(STAMP_PM_SUSPEND_START);

    if(0 == pm_ctrl_info.dpm_fail_count)
        writel(0, STAMP_DPM_SUSPEND_FAIL);
    if(bsp_dpm_suspend())
    {
        pm_ctrl_info.dpm_fail_count++;
        pm_timer_stamp(STAMP_DPM_SUSPEND_FAIL);
        writel(pm_ctrl_info.dpm_fail_count, PM_DPM_FAIL_COUNT);
        return -1;
    }

    pm_timer_stamp(STAMP_AFTER_DPM_SUSPEND);
    /*计时作为 MNTN  time repair*/
    enter_time_stamp = bsp_get_slice_value();

    (void)balongv7r2_uart_suspend(); /*lint !e746*/
    pm_timer_stamp(STAMP_AFTER_UART_SUSPEND);

    balong_pm_enter(state);

    (void)balongv7r2_uart_resume(); /*lint !e746*/
    pm_timer_stamp(STAMP_AFTER_UART_RESUME);

    out_time_stamp = bsp_get_slice_value();
    pm_time_tick = \
        (get_timer_slice_delta(enter_time_stamp,out_time_stamp)*(u32)sysClkRateGet() + TIMER_STAMP_FREQ/2)
                  /TIMER_STAMP_FREQ;
    //if(pm_time_tick>1)
        //pm_printk(BSP_LOG_LEVEL_ERROR,"PM: pm_time_tick = %d \n",pm_time_tick);
    /*time repair*/
    tickSet(tickGet()+pm_time_tick);

    pm_timer_stamp(STAMP_BEFORE_DPM_RESUME);
    #if defined(CONFIG_K3V3_CLK_CRG)
    bsp_tcxo1_set_flag(1);
    #endif
    bsp_dpm_resume();
    pm_timer_stamp(STAMP_AFTER_DPM_RESUME);

    pm_save_drx_timer_stamp();
    drv_udelay(5, g_nv_drx_delay.drv_1);

    return 0;
}

u32 debug_pm_threshold = 0;
int bsp_suspend(void)
{
    u32 idle_time_ms=0;;
	unsigned long flags = 0;
    u32 BBP_sleeptimer=0;
    s32 ret = -1;

	local_irq_save(flags);
    if(taskLock()==ERROR) /*In fact, taskLock only return OK*/
    {}

    idle_time_ms = get_next_schedule_time();
    BBP_sleeptimer = bsp_bbp_get_wakeup_time();

    if( (idle_time_ms >= pm_ctrl_info.deepsleep_Tth_nv)\
        &&(BBP_sleeptimer>=pm_ctrl_info.TLbbp_sleeptimer_Tth_nv)\
        &&(bsp_bbp_get_gubbp_wakeup_status()==0) )
    {
        ret = pm_suspend(0);
    }
    else
    {
        if(debug_pm_threshold)
            pm_printk(BSP_LOG_LEVEL_ERROR,"idle_time_ms=%d ms, BBP_sleeptimer=%d slice \n",idle_time_ms,BBP_sleeptimer);
    }
    if(taskUnlock()==ERROR)/*In fact, taskLock only return OK*/
    {}
    local_irq_restore(flags);
    return ret;
}

/* pm_asm_gic_flag: set by pm init ,used in asm */
extern u32 pm_asm_gic_flag; /* debug, 1:gic suspend ,0:gic not suspend,and ipc not send */

void balong_pm_init(void)
{
    u32 ret;
    NV_ID_DRV_ENUM nv_id;
    u32 pm_code_begin,pm_code_size;
    nv_id = NV_ID_DRV_PM;
    ret = bsp_nvm_read(nv_id, (u8 *)(&g_nv_pm_config), sizeof(DRV_NV_PM_TYPE));
    if(ret!=0)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_nvm_read FAILED id=0x%x,ret=%d \n",nv_id,ret);
        return;
    }

    nv_id = NV_ID_DRV_DRX_DELAY;
    ret = bsp_nvm_read(nv_id, (u8 *)&g_nv_drx_delay, sizeof(DRV_DRX_DELAY_STRU));
    if(ret != 0)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_nvm_read FAILED id=0x%x,ret=%d \n",nv_id,ret);
        return;
    }
    pm_ctrl_info.deepsleep_nv = g_nv_pwc_switch.deepsleep;
    pm_ctrl_info.deepsleep_Tth_nv = g_nv_pm_config.deepsleep_Tth;
    pm_ctrl_info.TLbbp_sleeptimer_Tth_nv = g_nv_pm_config.TLbbp_Tth;

    /* NV control , system will not suspend if ops is not set */
    if(0==pm_ctrl_info.deepsleep_nv)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR," core pm not inited \n");
        return;
    }
    else
    {
        pm_stamp_init();
        //pm_asm_gic_flag = pm_ctrl_info.gic_flag; /* flag for pm_asm_sleep.s */
        pm_code_begin = (u32)pm_asm_boot_code_begin;
        pm_code_size = (u32)pm_asm_sleep_end-pm_code_begin;
    	/* copy the pm_asm_sleep code to PM MEMORY */
    	pm_printk(BSP_LOG_LEVEL_DEBUG,"cpuMemoryCopy addr 0x%x .\n",PM_MEM_CCORE_BASE_ADDR);
        memcpy((void*)PM_MEM_CCORE_BASE_ADDR,(void*)pm_code_begin,(pm_code_size+PM_ASM_CODE_COPY_OFFSET));

        hi_syssc_set_mdma9_boot_addr(PM_MEM_CCORE_BASE_ADDR);

        pm_wakeup_init();

        cpuidle_set_deepsleep_ops(bsp_suspend);
#ifdef INCLUDE_L2_CACHE
        has_l2cache_flag = *(u32 *)(SHM_MEM_CCORE_L2CACHE_ADDR);
#endif
    }

}


void debug_balong_pm_init(void)
{
    u32 pm_code_begin,pm_code_size;
    pm_ctrl_info.deepsleep_nv = 1;
    pm_ctrl_info.deepsleep_Tth_nv = 100;
    pm_ctrl_info.TLbbp_sleeptimer_Tth_nv = 320;

    //pm_asm_gic_flag = pm_ctrl_info.gic_flag; /* flag for pm_asm_sleep.s */
    pm_code_begin = (u32)pm_asm_boot_code_begin;
    pm_code_size = (u32)pm_asm_sleep_end-pm_code_begin;
	/* copy the pm_asm_sleep code to PM MEMORY */
	pm_printk(BSP_LOG_LEVEL_DEBUG,"cpuMemoryCopy addr 0x%x .\n",PM_MEM_CCORE_BASE_ADDR);
    memcpy((void*)PM_MEM_CCORE_BASE_ADDR,(void*)pm_code_begin,(pm_code_size+PM_ASM_CODE_COPY_OFFSET));

    hi_syssc_set_mdma9_boot_addr(PM_MEM_CCORE_BASE_ADDR);

    cpuidle_set_deepsleep_ops(bsp_suspend);
}


void set_pm_threshold(u32 deepsleep_Tth,u32 TLbbp_Tth)
{
    pm_ctrl_info.deepsleep_Tth_nv = deepsleep_Tth;
    pm_ctrl_info.TLbbp_sleeptimer_Tth_nv = TLbbp_Tth;
}


void set_pm_debug_flag(u32 gic_flag,u32 sleep_flag)
{
    pm_ctrl_info.gic_flag= gic_flag;
    pm_asm_gic_flag = pm_ctrl_info.gic_flag;
    pm_ctrl_info.sleep_flag= sleep_flag;
}
void pm_print_debug_info(void)
{
    pm_printk(BSP_LOG_LEVEL_ERROR,"============= pm_ctrl_info ============\n");
    pm_printk(BSP_LOG_LEVEL_ERROR," pm_enter_count      %d\n", pm_ctrl_info.pm_enter_count);
    pm_printk(BSP_LOG_LEVEL_ERROR," pm_enter_asm_count  %d\n", pm_ctrl_info.pm_enter_asm_count);
    pm_printk(BSP_LOG_LEVEL_ERROR," pm_enter_wfi_count  %d\n", pm_ctrl_info.pm_enter_wfi_count);
    pm_printk(BSP_LOG_LEVEL_ERROR," pm_out_count        %d\n", pm_ctrl_info.pm_out_count);
    pm_printk(BSP_LOG_LEVEL_ERROR," stamp_addr 0x%x\n", pm_ctrl_info.stamp_addr);
    pm_printk(BSP_LOG_LEVEL_ERROR," gic_flag        %d\n", pm_ctrl_info.gic_flag);
    pm_printk(BSP_LOG_LEVEL_ERROR," sleep_flag      %d\n", pm_ctrl_info.sleep_flag);
    pm_printk(BSP_LOG_LEVEL_ERROR," deepsleep_nv    %d\n", pm_ctrl_info.deepsleep_nv);
    pm_printk(BSP_LOG_LEVEL_ERROR," deepsleep_Tth_nv        %d\n", pm_ctrl_info.deepsleep_Tth_nv);
    pm_printk(BSP_LOG_LEVEL_ERROR," TLbbp_sleeptimer_Tth_nv %d\n", pm_ctrl_info.TLbbp_sleeptimer_Tth_nv);
    //pm_printk(BSP_LOG_LEVEL_ERROR,"  %d\n", pm_ctrl_info.);
}

void pm_debug_clear_stamp(void)
{
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_START_ADDR =0x%x \n",STAMP_START_ADDR);
/*start form pm_suspend()*/
    *(u32*)STAMP_PM_SUSPEND_START = 0;
    *(u32*)STAMP_DPM_SUSPEND_FAIL = 0;
    *(u32*)STAMP_AFTER_DPM_SUSPEND = 0;
    *(u32*)STAMP_AFTER_UART_SUSPEND = 0;
/* pm enter */
    *(u32*)STAMP_PM_ENTER_START = 0;
    *(u32*)STAMP_AFTER_DISABLE_GIC = 0;
    *(u32*)STAMP_AFTER_BAK_GIC = 0;
    *(u32*)STAMP_AFTER_UTRACE_SUSPEND = 0;
    *(u32*)STAMP_AFTER_TCXO_SUSPEND = 0;
/*in sleep ASM power down*/
    *(u32*)STAMP_SLEEP_ASM_ENTER = 0;
    *(u32*)STAMP_BAK_COREG_BEGIN = 0;
    *(u32*)STAMP_BAK_COREG_END = 0;
    *(u32*)STAMP_BAK_MMUREG_BEGIN = 0;
    *(u32*)STAMP_BAK_MMUREG_END = 0;
    *(u32*)STAMP_BEFORE_SEND_IPC = 0;
    *(u32*)STAMP_AFTER_SEND_IPC = 0;
/* after wfi, should not be stamped */
    *(u32*)STAMP_AFTER_WFI_NOP = 0;
/*in sleep ASM power up*/
    *(u32*)STAMP_PWRUP_CODE_BEGIN = 0;
    *(u32*)STAMP_RSTR_MMUREG_BEGIN = 0;
    *(u32*)STAMP_RSTR_MMUREG_END = 0;
    *(u32*)STAMP_RSTR_COREG_BEGIN = 0;
    *(u32*)STAMP_RSTR_COREG_END = 0;
/* out from sleep ASM,in pm enter */
    *(u32*)STAMP_SLEEP_ASM_OUT = 0;
    *(u32*)STAMP_AFTER_TCXO_RESUME = 0;
    *(u32*)STAMP_AFTER_UTRACE_RESUME = 0;
    *(u32*)STAMP_AFTER_ENABLE_GIC = 0;
    *(u32*)STAMP_AFTER_RSTR_GIC = 0;
    *(u32*)STAMP_PM_ENTER_END = 0;
/* out from pm enter, in pm suspend*/
    *(u32*)STAMP_AFTER_UART_RESUME = 0;
    *(u32*)STAMP_BEFORE_DPM_RESUME = 0;
    *(u32*)STAMP_AFTER_DPM_RESUME = 0;
/* stamp for debug     */
    *(u32*)STAMP_DEBUG_1= 0;
    *(u32*)STAMP_DEBUG_2= 0;
    *(u32*)STAMP_DEBUG_3= 0;
    *(u32*)STAMP_DEBUG_4= 0;
    *(u32*)STAMP_DEBUG_5= 0;
    *(u32*)STAMP_DEBUG_6= 0;
}

void pm_debug_print_stamp(void)
{
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_START_ADDR =0x%x \n",STAMP_START_ADDR);
    /*start form pm_suspend()*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_PM_SUSPEND_START     =0x%x \n",  *(u32*)STAMP_PM_SUSPEND_START);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_DPM_SUSPEND    =0x%x \n",  *(u32*)STAMP_AFTER_DPM_SUSPEND);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_UART_SUSPEND   =0x%x \n",  *(u32*)STAMP_AFTER_UART_SUSPEND);
    /* pm enter */
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_PM_ENTER_START       =0x%x \n",  *(u32*)STAMP_PM_ENTER_START);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_DISABLE_GIC    =0x%x \n",  *(u32*)STAMP_AFTER_DISABLE_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_BAK_GIC        =0x%x \n",  *(u32*)STAMP_AFTER_BAK_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_UTRACE_SUSPEND =0x%x \n",  *(u32*)STAMP_AFTER_UTRACE_SUSPEND);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_TCXO_SUSPEND   =0x%x \n",  *(u32*)STAMP_AFTER_TCXO_SUSPEND);
    /*in sleep ASM power down*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_SLEEP_ASM_ENTER      =0x%x \n",  *(u32*)STAMP_SLEEP_ASM_ENTER);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BAK_COREG_BEGIN      =0x%x \n",  *(u32*)STAMP_BAK_COREG_BEGIN);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BAK_COREG_END        =0x%x \n",  *(u32*)STAMP_BAK_COREG_END);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BAK_MMUREG_BEGIN     =0x%x \n",  *(u32*)STAMP_BAK_MMUREG_BEGIN);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BAK_MMUREG_END       =0x%x \n",  *(u32*)STAMP_BAK_MMUREG_END);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BEFORE_SEND_IPC      =0x%x \n",  *(u32*)STAMP_BEFORE_SEND_IPC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_SEND_IPC       =0x%x \n",  *(u32*)STAMP_AFTER_SEND_IPC);
    /* after wfi, should not be stamped */
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_WFI_NOP        =0x%x \n",  *(u32*)STAMP_AFTER_WFI_NOP);
    /*in sleep ASM power up*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_PWRUP_CODE_BEGIN     =0x%x \n",  *(u32*)STAMP_PWRUP_CODE_BEGIN);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_RSTR_MMUREG_BEGIN    =0x%x \n",  *(u32*)STAMP_RSTR_MMUREG_BEGIN);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_RSTR_MMUREG_END      =0x%x \n",  *(u32*)STAMP_RSTR_MMUREG_END);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_RSTR_COREG_BEGIN     =0x%x \n",  *(u32*)STAMP_RSTR_COREG_BEGIN);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_RSTR_COREG_END       =0x%x \n",  *(u32*)STAMP_RSTR_COREG_END);
    /* out from sleep ASM,in pm enter */
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_SLEEP_ASM_OUT        =0x%x \n",  *(u32*)STAMP_SLEEP_ASM_OUT);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_TCXO_RESUME    =0x%x \n",  *(u32*)STAMP_AFTER_TCXO_RESUME);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_UTRACE_RESUME  =0x%x \n",  *(u32*)STAMP_AFTER_UTRACE_RESUME);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_RSTR_GIC       =0x%x \n",  *(u32*)STAMP_AFTER_RSTR_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_ENABLE_GIC     =0x%x \n",  *(u32*)STAMP_AFTER_ENABLE_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_PM_ENTER_END         =0x%x \n",  *(u32*)STAMP_PM_ENTER_END);
    /* out from pm enter, in pm suspend*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_UART_RESUME    =0x%x \n",  *(u32*)STAMP_AFTER_UART_RESUME);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_BEFORE_DPM_RESUME    =0x%x \n",  *(u32*)STAMP_BEFORE_DPM_RESUME);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_DPM_RESUME     =0x%x \n",  *(u32*)STAMP_AFTER_DPM_RESUME);


    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_DEBUG_1->     =0x%x 0x%x 0x%x 0x%x \n",  *(u32*)STAMP_DEBUG_1,
                    *(u32*)STAMP_DEBUG_2,*(u32*)STAMP_DEBUG_3,*(u32*)STAMP_DEBUG_4);
}


struct wake_lock pm_wakelock;

void pm_wake_lock(void)
{
    wake_lock(&pm_wakelock);
}

static s32 pm_wakeup_icc_read(u32 id , u32 len, void* context)
{
	u32 ret = 0;
	u8 data[32];
    if(len > 32)
	{
        pm_printk(BSP_LOG_LEVEL_ERROR,"pm_wakeup_icc_read len is err\n");
		return -1;
	}

    UNUSED(context);

    pm_wake_lock();

	if(len == 0)
        pm_printk(BSP_LOG_LEVEL_ALERT,"pm_appa9_read_cb len is 0 \n");
	ret = (u32)bsp_icc_read(id, data, len);
	if(len != ret)
	{
		pm_printk(BSP_LOG_LEVEL_ERROR,"pm_appa9_read_cb read error \r\n");
		return -1;
	}
	return 0;
}

void pm_wake_unlock(void)
{
    wake_unlock(&pm_wakelock);
}

void pm_wakeup_init(void)
{
	u32 ret;
	u32 channel_id_set = ((ICC_CHN_MCORE_CCORE << 16) | MCORE_CCORE_FUNC_WAKEUP);
    wake_lock_init(&pm_wakelock, PWRCTRL_SLEEP_TEST, "pm_wakeup");
	ret = (u32)bsp_icc_event_register(channel_id_set, (read_cb_func)pm_wakeup_icc_read, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
    if(ret)
        pm_printk(BSP_LOG_LEVEL_DEBUG,"register icc to mcore error: %d\n", ret);
}

/*
动态设置唤醒源
参数 wake_src 对应 drv_pm.h 中的 enum pm_wake_src
*/
#define ENABLE_WAKE_SRC     (PM_TYEP_CSET_WSRC)
#define DISABLE_WAKE_SRC    (PM_TYEP_CUNSET_WSRC)

void pm_set_wake_src(enum pm_wake_src wake_src, T_PM_TYPE pm_type)
{
    T_PM_MAIL pm_msg;
    u32 channel_id = (ICC_CHN_MCORE_CCORE << 16) | MCU_CCORE_WSRC;
	u32 ret = 0;
    u32 int_id = 0;
    u32 msglen = 0;

    switch(wake_src)
    {
        case DRV_WAKE_SRC_DRX_TIMER:
            int_id = M3_DRX_TIMER_INT;
            break;
        default:
		    pm_printk(BSP_LOG_LEVEL_ERROR," wake_src %d error \n",wake_src);
            return;
    }
    pm_msg.type = pm_type;
    pm_msg.addr = int_id;
	msglen = sizeof(T_PM_MAIL);

	ret = (u32)bsp_icc_send(ICC_CPU_MCU, channel_id, (u8*)(&pm_msg), msglen);
	if(ret != msglen)
	{
		pm_printk(BSP_LOG_LEVEL_ERROR,"PM icc_send_mcore error\n");
		return;
	}
    return;
}
void pm_enable_wake_src(enum pm_wake_src wake_src)
{
    pm_set_wake_src(wake_src,ENABLE_WAKE_SRC);
}

void pm_disable_wake_src(enum pm_wake_src wake_src)
{
    pm_set_wake_src(wake_src,DISABLE_WAKE_SRC);
}


