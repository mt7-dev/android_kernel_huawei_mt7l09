
#include "drv_comm.h"
#include "osl_types.h"
#include "rtc_balong.h"
#include <taskLib.h>

#define RTC_TEST_OK (0)
#define RTC_TEST_ERROR (1)
#ifndef CONFIG_HI3630_RTC0
s32 settime_test_case0(void);
s32 settime_test_case1(void);
s32 settime_test_case2(void);
s32 readtime_test(void);


#endif
s32 readtime_test(void)
{
	struct rtc_time	tm = {0};/*lint !e133 !e43*/
	balong_rtc_readtime( NULL, &tm);
    rtc_dbg("%4d-%02d-%02d %02d:%02d:%02d\n",
        tm.tm_year, tm.tm_mon, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return RTC_TEST_OK;
}
#ifndef CONFIG_HI3630_RTC0
static s32 compare_rtctime(struct rtc_time tm0, struct rtc_time tm1)
{
    if((tm0.tm_year == tm1.tm_year)
        &&(tm0.tm_mon == tm1.tm_mon)
        &&(tm0.tm_mday == tm1.tm_mday)
        &&(tm0.tm_hour == tm1.tm_hour)
        &&(tm0.tm_min == tm1.tm_min)
        &&((((tm0.tm_sec- tm1.tm_sec) < 3)&&((tm0.tm_sec - tm1.tm_sec)>= 0))
            || (((tm1.tm_sec - tm0.tm_sec)<3) && ((tm1.tm_sec - tm0.tm_sec) >=0))))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
s32 settime_test_case0(void)
{
	struct rtc_time tm = {0};
	struct rtc_time current_tm = {0};
    s32 ret = 0;
	tm.tm_year = 98;
	tm.tm_mon = 7;
	tm.tm_mday = 1;
	tm.tm_hour = 13;
	tm.tm_min = 1;
	tm.tm_sec = 25;

    balong_rtc_settime(NULL, &tm );
#ifdef CONFIG_BALONG_RTC
	taskDelay(200);
#else
	taskDelay(2);
#endif
	balong_rtc_readtime( NULL, &current_tm);
    ret = compare_rtctime(tm, current_tm);
    if(ret == 0)
    {
        return RTC_TEST_OK;
    }
    else
    {
        rtc_dbg(" %4d-%02d-%02d %02d:%02d:%02d",
                RTC_BASE_YEAR + current_tm.tm_year, current_tm.tm_mon, current_tm.tm_mday,
                current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
        return RTC_TEST_ERROR;
    }
}


s32 settime_test_case1(void)
{
	struct rtc_time tm = {0};
	struct rtc_time current_tm = {0};
    s32 ret = 0;
	tm.tm_year = 98;
	tm.tm_mon = 0;
	tm.tm_mday = 1;
	tm.tm_hour = 13;
	tm.tm_min = 1;
	tm.tm_sec = 25;

    balong_rtc_settime(NULL, &tm );
#ifdef CONFIG_BALONG_RTC
        taskDelay(200);
#else
        taskDelay(2);
#endif
	balong_rtc_readtime( NULL, &current_tm);
    ret = compare_rtctime(tm, current_tm);
    if(ret == 0)
    {
        return RTC_TEST_OK;
    }
    else
    {
        rtc_dbg("ccore settme : %4d-%02d-%02d %02d:%02d:%02d",
                RTC_BASE_YEAR + current_tm.tm_year, current_tm.tm_mon, current_tm.tm_mday,
                current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
        return RTC_TEST_ERROR;
    }
}

s32 settime_test_case2(void)
{
	struct rtc_time tm = {0};
	struct rtc_time current_tm = {0};
    s32 ret = 0;
	tm.tm_year = 98;
	tm.tm_mon = 1;
	tm.tm_mday = 1;
	tm.tm_hour = 25;
	tm.tm_min = 1;
	tm.tm_sec = 25;

    balong_rtc_settime(NULL, &tm );
#ifdef CONFIG_BALONG_RTC
        taskDelay(200);
#else
        taskDelay(2);
#endif
	balong_rtc_readtime( NULL, &current_tm);
    ret = compare_rtctime(tm, current_tm);
    if(ret == 1)
    {
        return RTC_TEST_OK;
    }
    else
    {
        rtc_dbg("ccore settme : %4d-%02d-%02d %02d:%02d:%02d",
                RTC_BASE_YEAR + current_tm.tm_year, current_tm.tm_mon, current_tm.tm_mday,
                current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
        return RTC_TEST_ERROR;
    }
}
#endif
