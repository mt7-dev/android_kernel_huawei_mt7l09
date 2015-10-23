
#ifndef __RTC_BALONG_H__
#define __RTC_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include <product_config.h>
#include <osl_types.h>
#include <bsp_om.h>
#include <bsp_pmu.h>

/*这里的定义和 Linux是保持一致的*/
 struct rtc_time
{
     int tm_sec;
     int tm_min;
     int tm_hour;
     int tm_mday;
     int tm_mon;
     int tm_year;
     int tm_wday;
     int tm_yday;
     int tm_isdst;
};
/*全局变量结构体定义*/
struct rtc_control
{
	u32 rtc_base_addr;/*RTC 寄存器基地址*/
    s32 rtc_days_in_month[12];
};
struct device
{
    int count;
};
extern struct rtc_control g_rtc_ctrl;

#define  rtc_print_error(fmt, ...)      (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RTC, "[rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  rtc_dbg(fmt,...)               (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_RTC, "[rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))

#ifndef RTC_BASE_YEAR
#define RTC_BASE_YEAR 1900
#endif
#ifndef WAIT_FOREVER
#define  WAIT_FOREVER   (-1)
#endif
#ifndef RTC_ICC_SEND_COUNT
#define  RTC_ICC_SEND_COUNT   (5)
#endif
#ifndef DELTA_BejingAndGregorian
#define DELTA_BejingAndGregorian   (8*60*60)
#endif

#ifndef CONFIG_HI3630_RTC0
s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm);
s32 balong_rtc_settime(struct device *dev, struct rtc_time *tm);
u32 balong_get_rtc_value (void);
#if defined(CONFIG_HI6551_RTC)
u32 rtc_read_reg(u16 addr);
u32 rtc_write_reg(u16 addr, u32 pValue);
#endif
#else
s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


