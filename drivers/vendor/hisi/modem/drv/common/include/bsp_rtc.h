
#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__
#ifdef __cplusplus
extern "C" 
{
#endif

#include "osl_types.h"

#ifdef __VXWORKS__
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
struct device
{
    int count;
};
#endif
#ifndef CONFIG_HI3630_RTC0
s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm);
s32 balong_rtc_settime(struct device *dev, struct rtc_time *tm);
#else
s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm);
#endif
#ifdef __cplusplus
}
#endif

#endif    /*  __BSP_RTC_H__ */
