
#ifndef __BSP_WDT_H__
#define __BSP_WDT_H__

#include <soc_interrupts.h>
#include <hi_syssc_interface.h>
#include <soc_clk.h>
#include <bsp_memmap.h>
#include <bsp_om.h>
#include <bsp_hardtimer.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include <bsp_softtimer.h>



#define  wdt_err(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MUDU_WDT, "[wdt]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  wdt_pinfo(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MUDU_WDT, "[wdt]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  wdt_debug(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MUDU_WDT, "[wdt]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/(sizeof((a)[0])))
#endif

/* 整理后 */
#define WDT_NUM                  				(2)
#define WDT_INDEX                			(0)
#define TIMER_INDEX              			(1)
#define WDT_RESET_TIMEOUT        	(60)
#define WDT_OK                   				(0)
#define WDT_ERROR                			(-1)
#define WDT_NULL            					(void*)0
#define WDT_RES_NUM              			(4)
#define ACORE_WDT_TIMEOUT    		(1)
#define CCORE_WDT_TIMEOUT    		(2)
#define MCORE_WDT_TIMEOUT    		(3)
/*硬狗使用*/
#define WDT_UNLOCK               				(0x1ACCE551)
#define WDT_LOCK                 					(0x0)
#define WDT_COUNT_DEFAULT       		(0xf0000)
#ifdef CONFIG_HI3630_CCORE_WDT
#define WDT_RST_INT_EN 						(0x1)
#else
#define WDT_RST_INT_EN 						(0x3)
#endif
#define WDT_DEF_CLK_FREQ         		(32768)                  /* 32khz */
#define WDT_KEEPALIVE_TIME				(15)





#ifdef __KERNEL__
#define WATCHDOG_TIMEOUT_SEC				(HI_TIMER3_CLK * 30)
#define WDT_HI_TIMER_CLK									(HI_TIMER3_CLK)
//4fe1fe00
#define STOP_WDT_TRACR_RUN_FLAG			(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_AM_FLAG)
#elif defined( __VXWORKS__)
#define WATCHDOG_TIMEOUT_SEC				(HI_TIMER4_CLK * 30)
#define WDT_HI_TIMER_CLK									(HI_TIMER4_CLK)
#define STOP_WDT_TRACR_RUN_FLAG			(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_CM_FLAG)
#elif defined(__CMSIS_RTOS)
#endif /* end of __KERNEL__ */


#ifndef BSP_WDT_SUSPEND_TIMEROUT
#define BSP_WDT_SUSPEND_TIMEROUT	(30)
#endif
#ifndef BSP_WDT_SOFTTIMEROUT
#define	 BSP_WDT_SOFTTIMEROUT			(1000)
#endif


#define BSP_SW_WDT_PERIOD       			(1500)
#ifdef BSP_FALSE
#define BSP_FALSE               						(0)
#endif
#ifdef BSP_TRUE
#define BSP_TRUE                						(1)
#endif

struct wdt_info{
		s32  lowtaskid;
		s32  init;
		s32  enable;
		u32  task_delay_value;
		u32  wdt_timeout;
		u32  wdt_suspend_timeout;
#if defined(__KERNEL__) || defined(__VXWORKS__)
		struct bsp_hardtimer_control my_timer;
#endif
		struct softtimer_list      wdt_timer_list;
};

typedef void(*wdt_timeout_cb)(int core);/*A、C核共用*/

/*functions*/
/*A C core functions*/
s32 hi6930_wdt_init(void);
s32 bsp_wdt_keepalive(void);
s32 bsp_wdt_start(void);
s32 bsp_wdt_stop(void);
s32 bsp_wdt_set_timeout(u32 timeout);
s32 bsp_wdt_get_timeleft(u32 *timeleft);
s32 bsp_wdt_register_hook(void *hook);
s32  bsp_wdt_unregister_hook(void);
s32 bsp_wdt_reboot_register_hook(void *hook);
s32  bsp_wdt_reboot_unregister_hook(void);
s32 bsp_wdt_suspend(u32 timeout);
void bsp_wdt_resume(void);


#endif /*__BSP_WDT_H__*/
