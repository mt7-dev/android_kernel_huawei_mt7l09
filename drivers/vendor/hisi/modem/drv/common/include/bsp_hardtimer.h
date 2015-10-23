#ifndef __BSP_HARDTIMER_H__
#define __BSP_HARDTIMER_H__
#include <hi_base.h>
#include <bsp_memmap.h>
#include <hi_timer.h>
#include <hi_syscrg.h>
#include <hi_bbp_systime.h>

#ifdef CONFIG_USE_TIMER_STAMP
#define TIMER_STAMP_ADDR        (HI_TIMER_05_REGBASE_ADDR_VIRT+HI_TIMER_CURRENTVALUE_OFFSET)
#define TIMER_STAMP_ADDR_PHY    (HI_TIMER_05_REGBASE_ADDR+HI_TIMER_CURRENTVALUE_OFFSET)
#else
#define TIMER_STAMP_ADDR        (HI_BBP_SYSTIME_BASE_ADDR_VIRT+HI_BBP_SYSTIME_ABS_TIMER_L_OFFSET)
#define TIMER_STAMP_ADDR_PHY    (HI_BBP_SYSTIME_BASE_ADDR +HI_BBP_SYSTIME_ABS_TIMER_L_OFFSET)
#endif

#define TIMER_STAMP_FREQ        (0x8000)

#ifndef __ASSEMBLY__
#include <osl_common.h>
#include <osl_math64.h>
#include <drv_timer.h>
#include <soc_clk.h>
#include <product_config.h>
#include <soc_timer.h>
#ifdef __KERNEL__
#define timer_func irq_handler_t
#elif defined(__VXWORKS__)
#define timer_func VOIDFUNCPTR
#endif
#define  hardtimer_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HARDTIMER, "[hardtimer]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#if defined(__KERNEL__)||defined(__VXWORKS__)
struct bsp_hardtimer_control
{
	u32 timerId;
	u32 mode;
	u32 timeout;
	timer_func func;
	u32 para;
	DRV_TIMER_UNIT unit; /*计数单位*/
};
#endif
/*****************************************************************************
* 函 数 名  :bsp_get_slice_value
*
* 功能描述  : 获取时间戳
*
* 输入参数  :  无
* 输出参数  : 无
*
* 返 回 值  : 时间戳定时器计数值，只在p531 asic上返回timer时间戳，
*                        其他返回系统控制器32k 时间戳，递增
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

static inline u32 bsp_get_slice_value(void)
{
#ifdef CONFIG_USE_TIMER_STAMP
	return 0xFFFFFFFF-readl((const volatile void *)TIMER_STAMP_ADDR);
#else
#ifdef __KERNEL__
	return readl((const volatile void *)TIMER_STAMP_ADDR);
#else
	return readl(TIMER_STAMP_ADDR);
#endif
#endif
}
/*****************************************************************************
* 函 数 名  :bsp_get_slice_value_hrt
*
* 功能描述  : 获取19.2M 高精度时间戳
*
* 输入参数  :  无
* 输出参数  : 无
*
* 返 回 值  : 19.2M 时间戳，递增
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

static inline u32 bsp_get_slice_value_hrt(void)
{
	return __bsp_get_slice_value_hrt();
}

/*****************************************************************************
* 函 数 名  :get_timer_slice_delta
*
* 功能描述  : 打点计时，直接获取时间戳前后差值
*
* 输入参数  :  begin:开始时间戳
					     end  :结束时间戳
* 输出参数  : 无
*
* 返 回 值  : 时间戳前后差值，单位为1
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

#define get_timer_slice_delta(begin,end) ((end>=begin)?(end-begin):(0xFFFFFFFF-begin+end))

/*****************************************************************************
* 函 数 名  :bsp_get_elapse_ms
*
* 功能描述  : 打点计时，以毫秒为单位返回系统启动到当前时间
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 以毫秒为单位的从系统启动到当前所流逝的时间
*
* 修改记录  :  2013年6月20日   lixiaojie
*****************************************************************************/

static inline u32 bsp_get_elapse_ms(void)
{
	u64 tmp=0;
	unsigned long timer_get = 0;
	timer_get = bsp_get_slice_value();
	tmp = (u64)(timer_get & 0xFFFFFFFF);
	tmp = tmp*1000;
	tmp = div_u64(tmp,HI_TIMER_STAMP_CLK);
	return (u32)tmp;
}
/*****************************************************************************
* 函 数 名  :udelay
*
* 功能描述  : 微秒延时
*
* 输入参数  : delay_us
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  :  2013年6月20日   lixiaojie
*****************************************************************************/
/*K3 LPM3有自带的udelay*/
#ifdef OS_K3V3_USE_LPM3_API
#if defined(__VXWORKS__)
static inline void udelay(u32 delay_us )
{
	u32 begin = 0,end=0,delta = 0;
	delta = delay_us*(UDELAY_TIMER_CLK/1000000);
	begin = readl(UDELAY_CURTIME_ADDR);
	begin = readl(UDELAY_CURTIME_ADDR);
	do{
		end = readl(UDELAY_CURTIME_ADDR);
	}while(get_timer_slice_delta(begin,end)<delta);
	return;
}
#endif
#else
#if defined(__VXWORKS__)||defined(__CMSIS_RTOS)
static inline void udelay(u32 delay_us )
{
	u32 begin = 0,end=0,delta = 0;
	delta = delay_us*(UDELAY_TIMER_CLK/1000000);
	begin = readl(UDELAY_CURTIME_ADDR);
	do{
		end = readl(UDELAY_CURTIME_ADDR);
	}while(get_timer_slice_delta(end,begin)<delta);
	return;
}
#endif
#endif
#ifdef CONFIG_MODULE_TIMER
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_load_value
*
* 功能描述  : 载入timer计数初始值
*
*输入参数  :  timer_id,
						value:计数值
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  :  2013年9月16日   lixiaojie
*****************************************************************************/


void bsp_hardtimer_load_value(u32 timer_id, u32 value);
void bsp_hardtimer_load_value_k3(u32 timer_id, u32 value);
#ifndef __CMSIS_RTOS

/*****************************************************************************
* 函 数 名  :bsp_hardtimer_alloc
*
* 功能描述  : 载入timer计数模式、计数初始值、挂接用户回调函数
*
*输入参数  :  struct bsp_hardtimer_control *my_hardtimer
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

s32 bsp_hardtimer_alloc(struct bsp_hardtimer_control *my_hardtimer);
s32 bsp_hardtimer_alloc_k3(struct bsp_hardtimer_control *my_hardtimer);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_start
*
* 功能描述  : 载入timer计数模式、计数初始值、挂接用户回调函数
					   同时使能定时器开始计数。
*输入参数  :  struct bsp_hardtimer_control *my_hardtimer  定时器配置参数结构体
                             unit  单位:0 毫秒，
                                                 1微秒 ，
                                                 2单位为1，即直接操作寄存器
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

s32 bsp_hardtimer_start(struct bsp_hardtimer_control  *timer_ctrl);
#endif
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_enable
*
* 功能描述  : 使能某hardtimer开始计数
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

s32 bsp_hardtimer_enable(u32 timer_id);
s32 bsp_hardtimer_enable_k3(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_disable
*
* 功能描述  : 去使能某hardtimer开始计数
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

s32 bsp_hardtimer_disable(u32 timer_id);
s32 bsp_hardtimer_disable_k3(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_free
*
* 功能描述  : disable掉硬timer，取消注册的回调函数
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

s32 bsp_hardtimer_free(u32 timer_id);
s32 bsp_hardtimer_free_k3(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_clear
*
* 功能描述  : 超时后会产生一个中断，本函数用于清除本次产生
*                           的中断
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  :
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

void bsp_hardtimer_int_clear(u32 timer_id);
void bsp_hardtimer_int_clear_k3(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_mask
*
* 功能描述  : 屏蔽中断
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

void bsp_hardtimer_int_mask(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_unmask
*
* 功能描述  : 不屏蔽中断
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

void bsp_hardtimer_int_unmask(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_hardtimer_int_status
*
* 功能描述  : 查询中断状态
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : 寄存器中断状态
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/

u32 bsp_hardtimer_int_status(u32 timer_id);
u32 bsp_hardtimer_int_status_k3(u32 timer_id);
/*****************************************************************************
* 函 数 名  :bsp_get_timer_current_value
*
* 功能描述  : 获取定时器计数寄存器当前值
*
* 输入参数  :  timer_id
* 输出参数  : 无
*
* 返 回 值  : 计数寄存器当前值
*
* 修改记录  :  2013年1月8日   lixiaojie
*****************************************************************************/
u32 bsp_get_timer_current_value(u32 timer_id);
u32 bsp_get_timer_current_value_k3(u32 timer_id);

/*****************************************************************************
* 函 数 名  :bsp_get_timer_rest_time
*
* 功能描述  : 根据传入参数，获取指定定时器以指定单位为单位的
*                           剩余时间
*
* 输入参数  : timer_id:定时器id
*                           unit:获取剩余时间单位
* 输出参数  : 无
*
* 返 回 值  : 剩余时间
*
* 修改记录  :  2013年6月20日   lixiaojie
*****************************************************************************/

u32 bsp_get_timer_rest_time(u32 timer_id, DRV_TIMER_UNIT unit);
u32 bsp_get_timer_rest_time_k3(u32 timer_id, DRV_TIMER_UNIT unit);


/*****************************************************************************
* 函 数 名  :get_next_schedule_time
*
* 功能描述  : 低功耗使用，用于计算唤醒源最近中断到来时间
*
* 输入参数  :  无
* 输出参数  : 无
*
* 返 回 值  : 毫秒
*
* 修改记录  :  2013年3月21日   lixiaojie
*****************************************************************************/

u32 get_next_schedule_time(void);
void timer_dpm_init(void);

#else
static inline void bsp_hardtimer_load_value(u32 timer_id, u32 value){}
static inline void bsp_hardtimer_load_value_k3(u32 timer_id, u32 value){}
static inline s32 bsp_hardtimer_alloc(struct bsp_hardtimer_control *my_hardtimer) {return 0;}
static inline s32 bsp_hardtimer_alloc_k3(struct bsp_hardtimer_control *my_hardtimer) {return 0;}
static inline s32 bsp_hardtimer_start(struct bsp_hardtimer_control  *timer_ctrl) {return 0;}
static inline s32 bsp_hardtimer_enable(u32 timer_id) {return 0;}
static inline s32 bsp_hardtimer_enable_k3(u32 timer_id) {return 0;}
static inline s32 bsp_hardtimer_disable(u32 timer_id) {return 0;}
static inline s32 bsp_hardtimer_disable_k3(u32 timer_id) {return 0;}
static inline s32 bsp_hardtimer_free(u32 timer_id) {return 0;}
static inline s32 bsp_hardtimer_free_k3(u32 timer_id) {return 0;}
static inline void bsp_hardtimer_int_clear(u32 timer_id) {}
static inline void bsp_hardtimer_int_clear_k3(u32 timer_id) {}
static inline void bsp_hardtimer_int_mask(u32 timer_id) {}
static inline void bsp_hardtimer_int_unmask(u32 timer_id) {}
static inline u32 bsp_hardtimer_int_status(u32 timer_id) {return 0;}
static inline u32 bsp_hardtimer_int_status_k3(u32 timer_id) {return 0;}
static inline u32 bsp_get_timer_current_value(u32 timer_id) {return 0;}
static inline u32 bsp_get_timer_current_value_k3(u32 timer_id) {return 0;}
static inline u32 bsp_get_timer_rest_time(u32 timer_id, DRV_TIMER_UNIT unit) {return 0;}
static inline u32 bsp_get_timer_rest_time_k3(u32 timer_id, DRV_TIMER_UNIT unit) {return 0;}
static inline u32 get_next_schedule_time(void) {return 0;}
static inline void timer_dpm_init(void){}
#endif

#endif /*__ASSEMBLY__*/

#endif /*__BSP_HARDTIMER_H__*/


