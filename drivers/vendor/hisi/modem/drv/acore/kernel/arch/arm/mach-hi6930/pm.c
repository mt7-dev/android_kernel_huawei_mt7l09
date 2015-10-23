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
/*lint ***--e{537,713,732,737,701,438,830}*/
#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/wakelock.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/outercache.h>
#include <asm/hardware/cache-l2x0.h>
#endif
#include <osl_types.h>
#include <osl_bio.h>
#include <hi_syssc_interface.h>
#include <soc_interrupts_m3.h>
#include <drv_nv_def.h>
#include <drv_nv_id.h>
#include <drv_pm.h>

#include <bsp_lowpower_mntn.h>
#include <bsp_nvim.h>
#include "bsp_om.h"
#include <bsp_hardtimer.h>
#include <bsp_gic_pm.h>
#include "bsp_utrace.h"
#include <bsp_icc.h>
#include <bsp_pm.h>

#include "pm.h"

ST_PWC_SWITCH_STRU g_nv_pwc_switch = {0}; // NV_ID_DRV_NV_PWC_SWITCH
DRV_NV_PM_TYPE  g_nv_pm_config  = {0};

/*lint -save -e550*/ /* 550: 函数内变量没有访问，仅仅只有赋值，自增，自减等运算.但为了程序可读性需要这种变量 */
void pm_set_trace_level(u32 level)
{
    bsp_mod_level_set(BSP_MODU_PM, level);
    pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_mod_level_set(BSP_MODU_PM=%d, %d)\n",BSP_MODU_PM,level);
}


#ifdef CONFIG_CACHE_L2X0
static u32 l2x0_cache_bak[3];
/*
 *  hi6930_pm_disable_l2x0()/hi6930_pm_enable_l2x0() is designed to
 *  disable and enable l2-cache during Suspend-Resume phase
 */
void hi6930_pm_disable_l2x0(void)
{
    u32 l2x0_base = HI_APPA9_L2_REGBASE_ADDR_VIRT;
	/* backup aux control register value */
	l2x0_cache_bak[0] = readl_relaxed(l2x0_base + L2X0_AUX_CTRL);
	l2x0_cache_bak[1] = readl_relaxed(l2x0_base + L2X0_TAG_LATENCY_CTRL);
	l2x0_cache_bak[2] = readl_relaxed(l2x0_base + L2X0_DATA_LATENCY_CTRL);
	outer_disable();
	pm_printk(BSP_LOG_LEVEL_DEBUG, "l2x0 Cache disabled.\r\n");
}

void hi6930_pm_enable_l2x0(void)
{
    u32 l2x0_base = HI_APPA9_L2_REGBASE_ADDR_VIRT;
	/* disable cache */
	writel_relaxed(0, l2x0_base + L2X0_CTRL);
	/* restore aux control register */
	writel_relaxed(l2x0_cache_bak[0], l2x0_base + L2X0_AUX_CTRL);
	writel_relaxed(l2x0_cache_bak[1], l2x0_base + L2X0_TAG_LATENCY_CTRL);
	writel_relaxed(l2x0_cache_bak[2], l2x0_base + L2X0_DATA_LATENCY_CTRL);
	/* invalidate l2x0 cache */
	outer_inv_all();
	/* enable l2x0 cache */
	writel_relaxed(1, l2x0_base + L2X0_CTRL);
	mb();
	pm_printk(BSP_LOG_LEVEL_DEBUG, "L2 Cache enabled\r\n");
}

#endif

static inline void pm_timer_stamp(u32 stamp_addr)
{
    u32 curr_time;
    curr_time = readl(PM_STAMP_ADDR);/* [false alarm]:屏蔽Fortify错误*/
    writel(curr_time, stamp_addr);    
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
    u32 i = 0;

    for(i = 0; i < CARM_GIC_ICDABR_NUM; i++)
	{
		*gic_int_set_reg = gic_disable_bak[i];
		gic_int_set_reg++;
	}
}

static int balong_pm_valid_state(suspend_state_t state)
{
	pm_printk(BSP_LOG_LEVEL_INFO,">>>>>>>enter valid state  %d<<<<<<<\n", state);
	switch (state){
	case PM_SUSPEND_ON:
	case PM_SUSPEND_STANDBY:
	case PM_SUSPEND_MEM:
		return 1;
	default:
		return 0;
	}
}

struct pm_info
{
    u32 pm_enter_count;
    u32 pm_enter_asm_count;
    u32 pm_enter_wfi_count;
    u32 pm_out_count;
    u32 stamp_addr;
    u32 gic_flag;   /* 若不禁GIC 直接从wfi出来，测试整个流程。 在汇编中读取，要放到结构体第一位 */
    u32 sleep_flag; /* 可以走 suspend;0 - 不进深睡；奇数 - 重复进入; 偶数 - 只进一次 ;*/
    u32 deepsleep_nv;
};
/*lint -save -e34*/

struct pm_info pm_ctrl_info =
{
    .pm_enter_count = 0,
    .pm_enter_asm_count = 0,
    .pm_enter_wfi_count = 0,
    .pm_out_count = 0,
    .stamp_addr = DUMP_EXT_OM_DRX_ACORE_ADDR,  /* pclint Non-constant Initializer*/
    .gic_flag = 1,
    .sleep_flag = 1,
    .deepsleep_nv = 0,
};
/*lint -restore*/
 int balong_pm_enter(suspend_state_t state)
{
	unsigned long flags = 0;
	u32 phy_addrs,virt_addrs;
	local_irq_save(flags);
    pm_timer_stamp(STAMP_PM_ENTER_START);

    pm_ctrl_info.pm_enter_count++;
    if(pm_ctrl_info.sleep_flag)
    {
        if (0 == pm_check_irq_pending_status()) /*中断状态查询*/
        {
    	    if(pm_ctrl_info.gic_flag)
                pm_disable_all_gic();
            pm_timer_stamp(STAMP_AFTER_DISABLE_GIC);

    		/*提前做虚实地址映射，将深睡的那段代码的虚拟地址映射成跟实际地址相同*/
    		phy_addrs = (u32)pm_create_idmap();
    		virt_addrs = (u32)__phys_to_virt(phy_addrs);/*lint !e737*//* [false alarm]:屏蔽Fortify错误*/
            writel(virt_addrs, STORE_REMAP_ADDR1);

            #ifdef CONFIG_CACHE_L2X0
            hi6930_pm_disable_l2x0();
            #endif

            gic_suspend();
            pm_timer_stamp(STAMP_AFTER_BAK_GIC);

            pm_ctrl_info.pm_enter_asm_count++;

            bsp_utrace_suspend();

            pm_asm_cpu_go_sleep();
            pm_timer_stamp(STAMP_SLEEP_ASM_OUT);

            bsp_utrace_resume();

            gic_resume();
            pm_timer_stamp(STAMP_AFTER_RSTR_GIC);

            #ifdef CONFIG_CACHE_L2X0
            hi6930_pm_enable_l2x0();
            #endif

            if(pm_ctrl_info.gic_flag)
                pm_enable_all_gic();
            pm_timer_stamp(STAMP_AFTER_ENABLE_GIC);

            /*debug: 如果此flag为偶数，则pm流程只走一次*/
            pm_ctrl_info.sleep_flag = pm_ctrl_info.sleep_flag&1;
        }
    }
    else
    {
        pm_ctrl_info.pm_enter_wfi_count++;
        WFI();
    }
    pm_ctrl_info.pm_out_count++;
    pm_timer_stamp(STAMP_PM_ENTER_END);

     /*更新醒来的时间戳*/
      update_awake_time_stamp();
	local_irq_restore(flags);
	return 0;
}

static const struct platform_suspend_ops balong_pm_ops = {
	.enter = balong_pm_enter,
	.valid = balong_pm_valid_state,
};


extern u32 pm_asm_gic_flag; /* debug, 1:gic suspend ,0:gic not suspend,and ipc not send */
void pm_wakeup_init(void);
static int __init balong_pm_init(void)
{
    u32 ret;
    NV_ID_DRV_ENUM nv_id;
    u32 pm_code_begin,pm_code_size;
    nv_id = NV_ID_DRV_NV_PWC_SWITCH;
    ret = bsp_nvm_read(nv_id, (u8 *)(&g_nv_pwc_switch), sizeof(ST_PWC_SWITCH_STRU));
    if(ret!=0)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_nvm_read FAILED id=0x%x,ret=%d \n",nv_id,ret);
        return (int)ret;
    }
    nv_id = NV_ID_DRV_PM;
    ret = bsp_nvm_read(nv_id, (u8 *)(&g_nv_pm_config), sizeof(DRV_NV_PM_TYPE));
    if(ret!=0)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR,"bsp_nvm_read FAILED id=0x%x,ret=%d \n",nv_id,ret);
        return (int)ret;
    }
    pm_ctrl_info.deepsleep_nv = g_nv_pwc_switch.deepsleep;

    /* NV control , system will not suspend if ops is not set */
    if(0==pm_ctrl_info.deepsleep_nv)
    {
        pm_printk(BSP_LOG_LEVEL_ERROR," core pm not inited \n");
        return -1;
    }
    pm_asm_gic_flag = pm_ctrl_info.gic_flag; /* flag for pm_asm_sleep.s */
    pm_code_begin = (u32)pm_asm_boot_code_begin;
    pm_code_size = (u32)pm_asm_sleep_end-pm_code_begin;

    /*FOR creat id map */
    writel(0, STORE_REMAP_ADDR1);
    writel(0, STORE_REMAP_ADDR2);
    writel(0, STORE_REMAP_ADDR3);

	/* copy the pm_asm_sleep code to PM MEMORY */
	memcpy((void*)PM_MEM_ACORE_BASE_ADDR, (void*)pm_asm_boot_code_begin, pm_code_size+PM_ASM_CODE_COPY_OFFSET);
	/*set_appa9_boot_addr  -   phy*/
    hi_syssc_set_appa9_boot_addr(SHD_DDR_V2P(PM_MEM_ACORE_BASE_ADDR));
    pm_wakeup_init();
    suspend_set_ops(&balong_pm_ops);
	return 0;
}

void debug_balong_pm_init(void)
{
    u32 pm_code_begin,pm_code_size;
    pm_ctrl_info.deepsleep_nv = 1;

    pm_asm_gic_flag = pm_ctrl_info.gic_flag; /* flag for pm_asm_sleep.s */
    pm_code_begin = (u32)pm_asm_boot_code_begin;
    pm_code_size = (u32)pm_asm_sleep_end-pm_code_begin;

    /*FOR creat id map */
    writel(0, STORE_REMAP_ADDR1);
    writel(0, STORE_REMAP_ADDR2);
    writel(0, STORE_REMAP_ADDR3);

	/* copy the pm_asm_sleep code to PM MEMORY */
    pm_printk(BSP_LOG_LEVEL_ERROR,"pm_boot_addr, virt:0x%x, phy:0x%x\n",\
            PM_MEM_ACORE_BASE_ADDR,SHD_DDR_V2P(PM_MEM_ACORE_BASE_ADDR));
	memcpy((void*)PM_MEM_ACORE_BASE_ADDR, (void*)pm_asm_boot_code_begin, pm_code_size+PM_ASM_CODE_COPY_OFFSET);
	/*set_appa9_boot_addr  -   phy*/
    hi_syssc_set_appa9_boot_addr(SHD_DDR_V2P(PM_MEM_ACORE_BASE_ADDR));
    suspend_set_ops(&balong_pm_ops);
}

module_init(balong_pm_init); /*lint !e19*/


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


    //pm_printk(BSP_LOG_LEVEL_ERROR,"  %d\n", pm_ctrl_info.);
}

void pm_debug_clear_stamp(void)
{
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_START_ADDR =0x%x \n",STAMP_START_ADDR);
/*start form pm_suspend()*/
    *(u32*)STAMP_PM_SUSPEND_START = 0;
    *(u32*)STAMP_AFTER_DPM_SUSPEND = 0;
    *(u32*)STAMP_AFTER_UART_SUSPEND = 0;

/* pm enter */
    *(u32*)STAMP_PM_ENTER_START = 0;
    *(u32*)STAMP_AFTER_DISABLE_GIC = 0;
    *(u32*)STAMP_AFTER_BAK_GIC = 0;

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
    *(u32*)STAMP_AFTER_ENABLE_GIC = 0;
    *(u32*)STAMP_AFTER_RSTR_GIC = 0;
    *(u32*)STAMP_PM_ENTER_END = 0;

/* out from pm enter, in pm suspend*/
    *(u32*)STAMP_AFTER_UART_RESUME = 0;
    *(u32*)STAMP_AFTER_DPM_RESUME = 0;
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
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_RSTR_GIC       =0x%x \n",  *(u32*)STAMP_AFTER_RSTR_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_ENABLE_GIC     =0x%x \n",  *(u32*)STAMP_AFTER_ENABLE_GIC);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_PM_ENTER_END         =0x%x \n",  *(u32*)STAMP_PM_ENTER_END);

    /* out from pm enter, in pm suspend*/
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_UART_RESUME    =0x%x \n",  *(u32*)STAMP_AFTER_UART_RESUME);
    pm_printk(BSP_LOG_LEVEL_ERROR,"STAMP_AFTER_DPM_RESUME     =0x%x \n",  *(u32*)STAMP_AFTER_DPM_RESUME);
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
	u32 channel_id_set = ((ICC_CHN_MCORE_ACORE << 16) | MCORE_ACORE_FUNC_WAKEUP);
    wake_lock_init(&pm_wakelock, WAKE_LOCK_SUSPEND, "pm_wakeup");
	ret = (u32)bsp_icc_event_register(channel_id_set, (read_cb_func)pm_wakeup_icc_read, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
    if(ret)
        pm_printk(BSP_LOG_LEVEL_DEBUG,"register icc to mcore error: %d\n", ret);
}

/*
动态设置唤醒源
参数 wake_src 对应 drv_pm.h 中的 enum pm_wake_src
*/
#define ENABLE_WAKE_SRC     (PM_TYEP_ASET_WSRC)
#define DISABLE_WAKE_SRC    (PM_TYEP_AUNSET_WSRC)

void pm_set_wake_src(enum pm_wake_src wake_src, T_PM_TYPE pm_type)
{
    T_PM_MAIL pm_msg;
    u32 channel_id = (ICC_CHN_MCORE_ACORE << 16) | MCU_ACORE_WSRC;
	u32 ret = 0;
    u32 int_id = 0;
    u32 msglen = 0;

    switch(wake_src)
    {
        case DRV_WAKE_SRC_UART0:
            int_id = M3_UART0_INT;
            break;
        case DRV_WAKE_SRC_SOCP:
            int_id = M3_SOCP_INT0;
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
/*lint -restore +e550*/



