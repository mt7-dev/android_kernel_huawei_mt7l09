#ifndef __SOC_TIMER_H__
#define __SOC_TIMER_H__

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_timer.h>
#include "soc_timer_enum.h"
#include <soc_interrupts.h>
#ifdef __KERNEL__
#include <soc_clk_app.h>
#elif defined(__VXWORKS__)
#include <soc_clk_mdm.h>
#elif defined(__CMSIS_RTOS)
#include <soc_clk.h>
#endif

/*封装udelay延时函数使用的timer基址*/
#define UDELAY_TIMER_ADDR   HI_AP_SYS_CNT_BASE_ADDR_VIRT
#define UDELAY_TIMER_CLK    19200000

#define CCORE_SYS_TIMER_CLK              HI_TIMER_CLK
#define CCORE_SYS_TIMER_BASE_ADDR        HI_TIMER_04_REGBASE_ADDR
#define CCORE_SYS_TIMER_INT_LVL          INT_LVL_TIMER4
#define UDELAY_CURTIME_ADDR   (UDELAY_TIMER_ADDR+0X1008)
/*k3 m3 softtimer*/
#define PM_MCU_TIMER_BASE      HI_TIMER_02_REGBASE_ADDR
#define PM_MCU_TIMER_CLK       HI_TCXO_CLK
#define PM_MCU_TIMER_INT_LVL   M3_TIMER21_INT

#ifndef __ASSEMBLY__
/*1代表非唤醒特性,k3只支持非唤醒*/
static inline int check_timer_type(unsigned int type)
{
	if(type!=1)
		return (-1);
	else
		return 0;
}

static inline unsigned int  __bsp_get_slice_value_hrt(void)
{
	return (*(volatile unsigned *)(UDELAY_CURTIME_ADDR));
}

#endif

#ifdef __KERNEL__
#define WAKE_TIMER_CLK                   1000
#define NOWAKE_TIMER_CLK                 HI_TIMER13_CLK
#define WAKE_TIMER_IRQ_ID                1000
#define NORMAL_TIMER_IRQ_ID              INT_LVL_TIMER13
#define wake_timer_array \
{\
	TIMER_ACPU_OSA_ID\
}


#define platform_timer_rest_time(i,j) bsp_get_timer_rest_time(i,j)


#elif defined(__VXWORKS__)
#define ST_WAKE_HARDTIMER_ADDR           1000
#define ST_NORMAL_HARDTIMER_ADDR         HI_TIMER_05_REGBASE_ADDR_VIRT
#define WAKE_TIMER_CLK                   1000
#define NOWAKE_TIMER_CLK                 HI_TIMER5_CLK
#define WAKE_TIMER_IRQ_ID                1000
#define NORMAL_TIMER_IRQ_ID              INT_LVL_TIMER5

#define wake_timer_array \
{\
	TIMER_CCPU_OSA_ID,\
	TIMER_CCPU_DSP_DRX_PROT_ID\
}

#define platform_timer_rest_time(i,j) bsp_get_timer_rest_time_k3(i,j)


#endif

#define WAKE_SOURCE_CLK      (32*1024)
#define NOWAKE_SOURCE_CLK    (32*1024)
#define SOFTTIMER_MAX_LENGTH             (0xFFFFFFFF>>15)
#define SOFTTIMER_MAX_LENGTH_NORMAL      (0xFFFFFFFF>>15)



#endif/*__SOC_TIMER_H__*/

