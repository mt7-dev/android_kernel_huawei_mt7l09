
#include <linux/rtc.h>
#include <linux/delay.h>
#include "rtc_hi6559.h"
#include "osl_types.h"
#include "osl_bio.h"
#include <linux/platform_device.h>

#define RTC_TEST_OK 0
#define RTC_TEST_ERROR 1

extern struct platform_device hi6559_rtc_device[HI6559_RTC_DEVICE_NUM];
extern struct rtc_control g_hi6559_rtc_ctrl;
s32 hi6559_readtime_test(void)
{
    struct rtc_time tm;

    hi6559_rtc_readtime( NULL, &tm );

    hi6559_rtc_print_dbg(" %4d-%02d-%02d %02d:%02d:%02d\n",
            RTC_BASE_YEAR + tm.tm_year, tm.tm_mon, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    return RTC_TEST_OK;
}

static int hi6559_rtc_test_settime(int y,int mon,int d,int h,int minute,int s)
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
    temp = hi6559_rtc_read_reg(HI6559_RTCCR_A0_OFFSET);

    hi6559_rtc_settime(NULL,&time);
	msleep(2);
    temp = hi6559_rtc_read_reg(HI6559_RTCCR_A0_OFFSET);
    rtc_tm_to_time( &time, (unsigned long*)&temp1);
    if((((temp - temp1) < 3)&&((temp - temp1)>= 0)) || (((temp1 - temp)<3) && ((temp - temp1)>= 0)))
        return RTC_TEST_OK;
    else
        return RTC_TEST_ERROR;
}

s32 hi6559_settime_test_case0(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(98, 7, 1, 13, 1, 25);
    if(RTC_TEST_OK == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("hi6559_rtc_test_settime(98, 7, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
s32 hi6559_settime_test_case1(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(98, 12, 1, 13, 1, 25);
    if(RTC_TEST_ERROR == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("hi6559_rtc_test_settime(98, 12, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
s32 hi6559_settime_test_case2(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(20, 12, 1, 13, 1, 25);
    if(RTC_TEST_ERROR == ret)
    {
        return RTC_TEST_OK;
    }
    else
    {
        printk("hi6559_rtc_test_settime(20, 12, 1, 13, 1, 25)\r\n");
        return RTC_TEST_ERROR;
    }
}
static int hi6559_rtc_test_setalarm(struct platform_device rtc, int y,int mon,int d,int h,int minute,int s,char enable)
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

    hi6559_rtc_setalarm(&rtc.dev, &alarm);

    udelay(3);

    alarmtime_reg = hi6559_rtc_read_reg(g_hi6559_rtc_ctrl.alarm_base_addr[rtc.id]);

	rtc_tm_to_time( &(alarm.time), (unsigned long*)&alarmtime_from);


    if( (alarmtime_from == alarmtime_reg)&&(enable == alarm.enabled))
        return RTC_TEST_OK;
    else
        return RTC_TEST_ERROR;
}


s32 hi6559_setalarm_test_case0(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += hi6559_rtc_test_setalarm(hi6559_rtc_device[0],98, 7, 1, 13, 3, 0 ,ALARM_ENABLE);
    if(RTC_TEST_OK == ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 13, 3, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 hi6559_setalarm_test_case1(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += hi6559_rtc_test_setalarm(hi6559_rtc_device[1],98, 7, 1, 24, 0, 0 ,ALARM_ENABLE);
    if(RTC_TEST_OK != ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 24, 0, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 hi6559_setalarm_test_case2(void)
{
    s32 ret = 0;
    ret = hi6559_rtc_test_settime(98, 7, 1, 13, 1, 25);
    ret += hi6559_rtc_test_setalarm(hi6559_rtc_device[2],90, 7, 1, 24, 0, 0 ,ALARM_ENABLE);
    if(0 != ret)
        return RTC_TEST_OK;
    else
    {
        printk("Fail:RTC time:98, 7, 1, 13, 1, 25 Alarm is:98, 7, 1, 24, 0, 0 ,1\r\n");
        return RTC_TEST_ERROR;
    }
}

s32 hi6559_readalarm_test(void)
{
    struct rtc_wkalrm alarm;
    hi6559_rtc_readalarm(&hi6559_rtc_device[0].dev, &alarm);

    hi6559_rtc_print_dbg("alarm time is %d.%d.%d-%d:%d:%d", alarm.time.tm_year, alarm.time.tm_mon, alarm.time.tm_mday, alarm.time.tm_hour, alarm.time.tm_min, alarm.time.tm_sec);
    hi6559_rtc_print_dbg("alarm enable is %d ", alarm.enabled);
    hi6559_rtc_print_dbg("alarm pending is %d ", alarm.pending);
    return RTC_TEST_OK;
}

s32 hi6559_alarmenable_test_case0(void)
{
    struct rtc_wkalrm alarm;

    hi6559_alarm_irq_enable(&hi6559_rtc_device[0].dev, ALARM_ENABLE);
    hi6559_rtc_readalarm(&hi6559_rtc_device[0].dev, &alarm);
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
s32 hi6559_alarmenable_test_case1(void)
{
    struct rtc_wkalrm alarm;

    hi6559_alarm_irq_enable(&hi6559_rtc_device[1].dev, ALARM_DISABLED);
    hi6559_rtc_readalarm(&hi6559_rtc_device[1].dev, &alarm);
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
