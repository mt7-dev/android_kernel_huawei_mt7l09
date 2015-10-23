
/*lint --e{537, 713, 732 } */
#include <vxWorks.h>
#include <iv.h>
#include <stdio.h>
#include <intLib.h>
#include <logLib.h>
#include <taskLib.h>
#include <tickLib.h>
#include <excLib.h>
#include <cacheLib.h>
#include <osl_types.h>
#include <osl_bio.h>
#include <bsp_sram.h>
#include <bsp_ipc.h>
#include <bsp_dpm.h>
#include <sysTimer.h>
#include <bsp_wdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CONFIG_HI3630_CCORE_WDT

/*lint --e{527, 529, 123, 40, 63, 551 } */
#define ARM_DBG_READ(M, OP2, VAL) do { \
			asm volatile("mrc p14, 0, %0, c0," #M ", " #OP2 : "=r" (VAL)); \
			} while (0)

#define ARM_DSCR_HDBGEN		(1 << 14)

void hi6930_wdt_lowtask(void);
void bsp_wdt_reboot_direct(void);
void bsp_wdt_print_debug(void);

unsigned int current_timeout = 0;
wdt_timeout_cb g_pSoftFeedFunc = WDT_NULL;
wdt_timeout_cb g_wdt_rebootfunc = WDT_NULL;
struct wdt_info hi6930_wdt_ident;


static void  hi6930_wdt_ccore_irq(u32 para)
{
	u32 dscr = 0;

	ARM_DBG_READ(c1, 0, dscr); /*lint !e40 */

	STOP_WDT_TRACR_RUN_FLAG = 0;
	/* Ensure that halting mode is disabled. */
	if (dscr & ARM_DSCR_HDBGEN)
	{
		STOP_WDT_TRACR_RUN_FLAG = 1;
	}
	else
	{
		STOP_WDT_TRACR_RUN_FLAG = 0;
	}
	wdt_err("c:%d flag:%d\n", dscr & ARM_DSCR_HDBGEN, STOP_WDT_TRACR_RUN_FLAG);
	return;
}


void hi6930_wdt_lowtask(void)
{ /*lint !e527 */
	wdt_pinfo("enter wdt lowtask\n");
	/* coverity[INFINITE_LOOP] */
	/* coverity[no_escape] */
	for(;;)
    {
        taskDelay((s32)hi6930_wdt_ident.task_delay_value);

        (void)bsp_wdt_keepalive();
    }
}
#if 0
int balong_wdt_resume(struct dpm_device *wdt_resume)/*lint !e527 */
{
	bsp_wdt_resume();
	return WDT_OK;
}

int balong_wdt_suspend(struct dpm_device *wdt_suspend) /*lint !e26 */
{
	int ret = 0;
	ret = bsp_wdt_suspend(ret);
	return WDT_OK;
}



//def CONFIG_CCORE_PM
static struct dpm_device wdt_dpm_device={
    .device_name = "wdt_dpm",
    .suspend = balong_wdt_suspend,
    .resume = balong_wdt_resume,
};

void wdt_timer_event(u32 agrv)
{
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		wdt_err("wdt nv enabled??\n");
		return;
	}
	bsp_wdt_keepalive();
}
#endif
s32 hi6930_wdt_init(void)
{
	int ret = 0;
	s32 retValue = -1;
	DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
    retValue = bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
    if (NV_OK != retValue)
    {
    	wdt_err("read nv failed use default value\n");
		wdt_nv_param.wdt_enable = BSP_FALSE;
		wdt_nv_param.wdt_keepalive_ctime = 15;
    }
	hi6930_wdt_ident.lowtaskid = 0;
	hi6930_wdt_ident.enable = wdt_nv_param.wdt_enable;
	hi6930_wdt_ident.task_delay_value = wdt_nv_param.wdt_keepalive_ctime * 100;

    hi6930_wdt_ident.lowtaskid =  taskSpawn("tWDTLowTask", 120, 0, 0x1000, (FUNCPTR)hi6930_wdt_lowtask, 0,0,0,0,0,0,0,0,0,0);/*lint !e64 !e119 !e40 */
    if(WDT_ERROR == hi6930_wdt_ident.lowtaskid)
    {
    	wdt_err("task create error\n");
        return WDT_ERROR;
    }
    /*for running trace stop wdt*/
     if (bsp_ipc_int_connect(IPC_CCPU_INT_SRC_MCPU_WDT,hi6930_wdt_ccore_irq,0))
     {
		wdt_err("bsp_ipc_int_connect failed\n");
		return WDT_ERROR;
	}
	 if (bsp_ipc_int_enable(IPC_CCPU_INT_SRC_MCPU_WDT))
	 {
		wdt_err("bsp_ipc_int_enable failed\n");
		return WDT_ERROR;
	}
#ifdef CONFIG_CCORE_PM
    //bsp_device_pm_add(&wdt_dpm_device);
#endif
    ret = bsp_wdt_start();
    if (ret == WDT_ERROR)
    {
		wdt_err("wdt start error\n");
        return WDT_ERROR;
    }

	wdt_err("wdt init ok\n");
    return WDT_OK;
}

s32 bsp_wdt_start(void)
{
    u32 reg = 0;
    s32 retValue = -1;
	DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
    retValue = bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
    if (NV_OK != retValue)
    {
    	wdt_err("read nv failed use default value\n");
		wdt_nv_param.wdt_enable = BSP_FALSE;
		wdt_nv_param.wdt_timeout = 30;
		wdt_nv_param.wdt_suspend_timerout = 120;
    }
    hi6930_wdt_ident.enable = wdt_nv_param.wdt_enable;
    hi6930_wdt_ident.my_timer.func = NULL;
    hi6930_wdt_ident.my_timer.mode = TIMER_PERIOD_COUNT;
    hi6930_wdt_ident.my_timer.timeout = WDT_HI_TIMER_CLK * wdt_nv_param.wdt_timeout;
    hi6930_wdt_ident.my_timer.timerId = CCORE_WDT_TIMER_ID;
    hi6930_wdt_ident.my_timer.unit = TIMER_UNIT_NONE;
    hi6930_wdt_ident.wdt_suspend_timeout = wdt_nv_param.wdt_suspend_timerout;

	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		wdt_err("wdt nv enabled??\n");
		return WDT_OK;
	}
    reg = bsp_hardtimer_start(&hi6930_wdt_ident.my_timer);
    if (reg)
    {
		wdt_err("wdt start hardtimer failed\n");
		return WDT_ERROR;
    }
    current_timeout = hi6930_wdt_ident.my_timer.timeout /WDT_HI_TIMER_CLK;

    /*A-M 核标志for 仿真器停狗*/
    STOP_WDT_TRACR_RUN_FLAG = 0;
    wdt_debug("exit wdt start\n");
    return WDT_OK;
}

s32 bsp_wdt_stop(void)
{
	u32 reg = 0;
	reg = bsp_hardtimer_disable(CCORE_WDT_TIMER_ID);
	hi6930_wdt_ident.enable = BSP_FALSE;
	STOP_WDT_TRACR_RUN_FLAG = 1;
	wdt_debug("exit wdt stop %d\n", reg);
	return WDT_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : BSP_U8 u8WdtId :看门狗ID
* 输出参数  : 无
*
* 返 回 值  : WDT_OK:    操作成功
*             WDT_ERROR: 操作失败
* 其它说明  :
*
*****************************************************************************/
s32 bsp_wdt_keepalive(void)
{
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_pinfo("wdt is not enabled \n");
		return WDT_OK;
	}
	bsp_hardtimer_disable(CCORE_WDT_TIMER_ID);
	bsp_hardtimer_enable(CCORE_WDT_TIMER_ID);

    wdt_debug("exit wdt keepalive\n");
    return WDT_OK;
}


/*****************************************************************************
* 函 数 名  : BSP_WDT_TimerReboot
*
* 功能描述  : 通过设置timer中断，通知mcore重启。
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : WDT_OK:    操作成功
*             WDT_ERROR: 操作失败
* 其它说明  :
*
*****************************************************************************/
void bsp_wdt_reboot_direct(void)
{
	//hi_syssc_wdt_reboot();
}
s32 bsp_wdt_set_timeout(u32 timeout)/*s*/
{
	u32 reg;
	u32 value;
	value = timeout * WDT_HI_TIMER_CLK;
	hi6930_wdt_ident.my_timer.timeout = value;
	wdt_pinfo("wdt set timeout : %d\n", timeout);
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_err("wdt is not enabled \n");
		return WDT_OK;
	}
	reg = bsp_hardtimer_start(&hi6930_wdt_ident.my_timer);
    if (reg)
    {
		wdt_err("wdt start hardtimer failed\n");
		return WDT_ERROR;
    }
    current_timeout = timeout;
	wdt_debug("exit wdt set timeout: %d\n", value);
	return WDT_OK;
}

 s32 bsp_wdt_get_timeleft(u32 *timeleft)/*s*/
{
	u32 reg;
	if(WDT_NULL == timeleft)
	{
		return WDT_ERROR;
	}
	reg = bsp_get_timer_current_value(CCORE_WDT_TIMER_ID);

	*timeleft = reg/WDT_HI_TIMER_CLK;
	wdt_pinfo("exit wdt get timeleft: %d\n", *timeleft);
	return WDT_OK;
}
s32 bsp_wdt_register_hook(void *hook)
{
    if(WDT_NULL == hook)
    {
		 wdt_err("hook is NULL\n");
        return WDT_ERROR;
    }

	g_pSoftFeedFunc = hook;

	return WDT_OK;
}

s32  bsp_wdt_unregister_hook(void)
{
	g_pSoftFeedFunc = WDT_NULL;
	return WDT_OK;
}

s32 bsp_wdt_reboot_register_hook(void *hook)
{
    if(WDT_NULL == hook)
    {
		 wdt_err("hook is NULL\n");
        return WDT_ERROR;
    }

	g_wdt_rebootfunc = hook;

	return WDT_OK;
}

s32  bsp_wdt_reboot_unregister_hook(void)
{
	g_wdt_rebootfunc = WDT_NULL;
	wdt_pinfo("wdt unregister_hook\n");
	return WDT_OK;
}
s32 bsp_wdt_suspend(u32 timeout)
{
	 bsp_wdt_set_timeout(timeout);
	 return WDT_OK;
 }
void bsp_wdt_resume(void)
{
	/*设置WDT 时间30s*/
	bsp_wdt_set_timeout(30);/*时间还需修改*/
}



void bsp_wdt_print_debug(void)
{
    unsigned int timeleft = 0;
    bsp_wdt_get_timeleft(&timeleft);
    wdt_err("*******************wdt debug  start*******************\n");
    wdt_err("timeleft: %d\n", timeleft);
    wdt_err("cu_timeout: %d\n", current_timeout);
    wdt_err("*******************wdt debug  end*******************\n");
}

#elif defined(CONFIG_HI3630_CCORE_WDT)
/*K3 ccore wdt*/
#include <hi_wdt.h>
#include <bsp_ipc.h>
#include "wdt_balong.h"


struct wdt_info hi6930_wdt_ident;

unsigned int current_timeout = 0;
struct wdt_control g_wdt_ctrl={
	.wdt_cb = WDT_NULL,
};
wdt_timeout_cb g_wdt_rebootfunc = WDT_NULL;

static int hi6930_wdt_stop(void);
static int hi6930_wdt_keepalive(void);
static int hi6930_wdt_set_timeout(u32 timeout);
static u32 hi6930_wdt_get_timeleft(void);


/* functions */
static int hi6930_wdt_keepalive(void)
{
	unsigned long irqlock = 0;
	local_irq_save(irqlock);
	/* 寄存器解锁 */
	writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	/*中断清除*/
	writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_INTCLR_OFFSET);
	/*给Watchdog上锁*/
	writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

	local_irq_restore(irqlock);

	return WDT_OK;
}

static int hi6930_wdt_stop(void)
{
    unsigned long irqlock = 0;
    local_irq_save(irqlock);
    /* 寄存器解锁 */
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    writel(0x0, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    /*给Watchdog上锁*/
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    local_irq_restore(irqlock);

    return WDT_OK;
}

static int hi6930_wdt_start(void)
{
    unsigned long irqlock = 0;
    DRV_WDT_INIT_PARA_STRU wdt_nv_param = {0};
	u32 retValue = 0;
    retValue = bsp_nvm_read(NV_ID_DRV_WDT_INIT_PARAM, (u8*)&wdt_nv_param, sizeof(DRV_WDT_INIT_PARA_STRU));
    if (NV_OK != retValue)
    {
    	wdt_err("read nv failed use default value\n");
		wdt_nv_param.wdt_enable = 0;
		wdt_nv_param.wdt_timeout = 30;
		wdt_nv_param.wdt_keepalive_ctime = 15;
		wdt_nv_param.wdt_suspend_timerout = 120;
    }
	/*读取系统控制寄存器0x33c bit[0]为0打开看门狗，1关闭看门狗*/
	retValue = readl(HI_AP_SYSCTRL_BASE_ADDR + HI_WDG_SYSCTRL_ENABLE_OFFSET) & 0x1;
	/*NV 、寄存器共同判别是否可开狗*/
	if ((0 == retValue) && (wdt_nv_param.wdt_enable != 0))
	{
		wdt_nv_param.wdt_enable = 1;
	}
	else
	{
		wdt_nv_param.wdt_enable = 0;
	}
	hi6930_wdt_ident.enable = (s32)wdt_nv_param.wdt_enable;
	hi6930_wdt_ident.wdt_timeout = wdt_nv_param.wdt_timeout;
	hi6930_wdt_ident.wdt_suspend_timeout = wdt_nv_param.wdt_suspend_timerout;
	hi6930_wdt_ident.task_delay_value = wdt_nv_param.wdt_keepalive_ctime * 100;

	retValue = WDT_DEF_CLK_FREQ * hi6930_wdt_ident.wdt_timeout ;
    if (BSP_FALSE == hi6930_wdt_ident.enable)
    {
		wdt_err("nv enabled?\n");
		hi6930_wdt_stop();
		return WDT_OK;
    }
    local_irq_save(irqlock);

    hi_syssc_wdt_enble();
    /* 寄存器解锁 */
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
    /*设置看门狗计数器初值*/
    writel(retValue, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOAD_OFFSET);
    /* 使能复位和中断 */
    writel(WDT_RST_INT_EN, HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    /*给Watchdog上锁*/
    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	current_timeout = retValue / WDT_DEF_CLK_FREQ;
    local_irq_restore(irqlock);

    return WDT_OK;
}

static int hi6930_wdt_set_timeout(u32 timeout) /*输入参数单位是秒*/
{
    unsigned long irqlock = 0;
    local_irq_save(irqlock);
    writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

    writel(timeout * WDT_DEF_CLK_FREQ, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOAD_OFFSET);

    writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

    local_irq_restore(irqlock);
    current_timeout = timeout;
    return WDT_OK;
}

static u32 hi6930_wdt_get_timeleft(void)
{
	u32 reg = 0;
	u32 timeleft = 0;
	unsigned long irqlock = 0;

	local_irq_save(irqlock);

	writel(WDT_UNLOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);
	reg = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_VALUE_OFFSET);
	writel(WDT_LOCK, HI_WDT_BASE_ADDR_VIRT + HI_WDG_LOCK_OFFSET);

	local_irq_restore(irqlock);

	timeleft = reg/WDT_DEF_CLK_FREQ;

	return timeleft;
}


void hi6930_wdt_lowtask(void)
{ /*lint !e527 */
	wdt_pinfo("enter wdt lowtask\n");
	/* coverity[INFINITE_LOOP] */
	/* coverity[no_escape] */
	for(;;)
    {
        taskDelay((int)hi6930_wdt_ident.task_delay_value);

        (void)bsp_wdt_keepalive();
    }
}

s32 hi6930_wdt_init(void)
{
	hi6930_wdt_ident.lowtaskid =  taskSpawn("tWDTLowTask", 120, 0, 0x1000, (FUNCPTR)hi6930_wdt_lowtask, 0,0,0,0,0,0,0,0,0,0);/*lint !e64 !e119 !e40 */
	if(WDT_ERROR == hi6930_wdt_ident.lowtaskid)
	{
		wdt_err("task create error\n");
	    return WDT_ERROR;
	}
    bsp_wdt_start();
    wdt_err("wdt init ok\n");
    return WDT_OK;
}


s32 bsp_wdt_start(void)
{
	hi6930_wdt_start();
	return WDT_OK;
}

s32 bsp_wdt_stop(void)
{
    hi6930_wdt_stop();
    return WDT_OK;
}

s32 bsp_wdt_keepalive(void)
{
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_pinfo("wdt is not enabled \n");
		return WDT_OK;
	}
    hi6930_wdt_keepalive();
    return WDT_OK;
}

s32 bsp_wdt_get_timeleft(u32 *timeleft)
{
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_pinfo("wdt is not enabled \n");
		return WDT_OK;
	}
	if(WDT_NULL == timeleft)
	{
		wdt_err("timeleft is NULL\n");
		return WDT_ERROR;
	}
    *timeleft = hi6930_wdt_get_timeleft();
    return WDT_OK;
}

/*系统重启*/
void bsp_wdt_reboot_direct(void)
{
    //bsp_drv_power_reboot_direct();
}

signed int bsp_wdt_register_hook(void *hook)
{
    if(NULL == hook)
    {
    	wdt_err("hook is NULL\n");
       return WDT_ERROR;
    }

	g_wdt_ctrl.wdt_cb = hook;
    return WDT_OK;
}

signed int  bsp_wdt_unregister_hook()
{
	g_wdt_ctrl.wdt_cb = NULL;
	return WDT_OK;
}
s32 bsp_wdt_reboot_register_hook(void *hook)
{
    if(WDT_NULL == hook)
    {
        return WDT_ERROR;
    }
	g_wdt_rebootfunc = hook;

	return WDT_OK;
}

s32  bsp_wdt_reboot_unregister_hook(void)
{
	g_wdt_rebootfunc = WDT_NULL;
	return WDT_OK;
}

s32 bsp_wdt_set_timeout(u32 timeout)
{
	s32 result;
	if(hi6930_wdt_ident.enable == BSP_FALSE)
	{
		wdt_pinfo("wdt is not enabled \n");
		return WDT_OK;
	}
	result = hi6930_wdt_set_timeout(timeout);
	return result;
}

s32 bsp_wdt_suspend(u32 timeout)
{
	u32 suspend_timeout = hi6930_wdt_ident.wdt_suspend_timeout;

	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		return WDT_OK;
	}
	bsp_wdt_set_timeout(suspend_timeout);
	return WDT_OK;
}

void bsp_wdt_resume(void)
{
	if (BSP_FALSE == hi6930_wdt_ident.enable)
	{
		return;
	}
    bsp_wdt_set_timeout(hi6930_wdt_ident.wdt_timeout);
}

void bsp_wdt_print_debug(void)
{
    unsigned int timeleft = 0;
    bsp_wdt_get_timeleft(&timeleft);
    wdt_err("*******************wdt debug  start*******************\n");
    wdt_err("timeleft: %d\n", timeleft);
    wdt_err("cur_timeout: %d\n", current_timeout);
    timeleft = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_CONTROL_OFFSET);
    wdt_err("ctr_reg: 0x%x\n", timeleft);
    timeleft = readl(HI_WDT_BASE_ADDR_VIRT + HI_WDG_VALUE_OFFSET);
    wdt_err("wdt_value: 0x%x\n", timeleft);
    wdt_err("*******************wdt debug  end*******************\n");
}

#endif

#ifdef __cplusplus
}
#endif
