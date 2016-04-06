
#include <linux/rtc.h>
#include <linux/delay.h>
#include "rtc_balong.h"
#include "osl_types.h"
#include "hi_rtc.h"
#include "osl_bio.h"

#define RTC_TEST_OK 0
#define RTC_TEST_ERROR 1

s32 readtime_test(void)
{
    struct rtc_time tm;

    balong_rtc_readtime( NULL, &tm );

    rtc_dbg(" %4d-%02d-%02d %02d:%02d:%02d\n",
            RTC_BASE_YEAR + tm.tm_year, tm.tm_mon, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    return RTC_TEST_OK;
}

static int rtc_test_settime(int y,int mon,int d,int h,int minute,int s)
{
    struct rtc_time time;
    u32 temp = 0;
    u32 temp1 = 0;

    time.tm_year = y;
    time.tm_mon = mon;
    time.tm_mday = d;
    time.tm_hour = h;
    time.tm_min = minute;
    time.tm_sec = s;
    temp = readl((volatile unsigned *) (g_rtc_ctrl.rtc_base_addr + HI_RTC_CCVR_OFFSET));

    balong_rtc_settime(NULL,&time);
	msleep(2000);
    temp = readl((volatile unsigned *) (g_rtc_ctrl.rtc_base_addr + HI_RTC_CCVR_OFFSET));
    rtc_tm_to_time( &time, (unsigned long*)&temp1);
    if((((temp - temp1) < 3)&&((s32)(temp - temp1)>= 0)) || (((temp1 - temp)<3) && ((s32)(temp - temp1)>= 0)))
        return RTC_TEST_OK;
    else
        return RTC_TEST_ERROR;
}

s32 settime_test_case0(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(98, 7, 1, 13, 1, 25);
    if(RTC_TEST_OK == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("rtc_test_settime(98, 7, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
s32 settime_test_case1(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(98, 12, 1, 13, 1, 25);
    if(RTC_TEST_ERROR == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("rtc_test_settime(98, 12, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
s32 settime_test_case2(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(20, 12, 1, 13, 1, 25);
    if(RTC_TEST_ERROR == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("rtc_test_settime(20, 12, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
static int rtc_test_setalarm( int y,int mon,int d,int h,int minute,int s,char enable)
{
    struct rtc_wkalrm alarm;
    u32 alarmtime_reg = 0;
    u32 alarmtime_from = 0;

    alarm.time.tm_year = y;
    alarm.time.tm_mon = mon;
    alarm.time.tm_mday = d;
    alarm.time.tm_hour = h;
    alarm.time.tm_min = minute;
    alarm.time.tm_sec = s;
    alarm.enabled = enable;

    balong_rtc_setalarm(NULL, &alarm);

    alarmtime_reg = readl((volatile unsigned *)(g_rtc_ctrl.rtc_base_addr + HI_RTC_CMR_OFFSET));
	rtc_tm_to_time( &(alarm.time), (unsigned long*)&alarmtime_from);


    if( (alarmtime_from == alarmtime_reg)&&(enable == alarm.enabled))
        return RTC_TEST_OK;
    else
        return RTC_TEST_ERROR;
}


s32 setalarm_test_case0(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += rtc_test_setalarm(98, 7, 1, 13, 3, 0 ,ALARM_ENABLE);
    if(RTC_TEST_OK == ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 13, 3, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 setalarm_test_case1(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += rtc_test_setalarm(98, 7, 1, 24, 0, 0 ,ALARM_ENABLE);
    if(RTC_TEST_OK != ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 24, 0, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 setalarm_test_case2(void)
{
    s32 ret = 0;
    ret = rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += rtc_test_setalarm(90, 7, 1, 24, 0, 0 ,ALARM_ENABLE);
    if(0 != ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 24, 0, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 readalarm_test(void)
{
    struct rtc_wkalrm alarm;
    balong_rtc_readalarm(NULL, &alarm);

    return RTC_TEST_OK;
}

s32 alarmenable_test_case0(void)
{
    struct rtc_wkalrm alarm;

    balong_alarm_irq_enable(NULL, ALARM_ENABLE);
    balong_rtc_readalarm(NULL, &alarm);
    if(ALARM_ENABLE == alarm.enabled)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("Fail:alarm enable\r\n");
        return RTC_TEST_ERROR;
    }
}
s32 alarmenable_test_case1(void)
{
    struct rtc_wkalrm alarm;

    balong_alarm_irq_enable(NULL, ALARM_DISABLED);
    balong_rtc_readalarm(NULL, &alarm);
    if(ALARM_DISABLED == alarm.enabled)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("Fail:alarm disable\r\n");
        return RTC_TEST_ERROR;
    }
}
/********************test end**********************/
