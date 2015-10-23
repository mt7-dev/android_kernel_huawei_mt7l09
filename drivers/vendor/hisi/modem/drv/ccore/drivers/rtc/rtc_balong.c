
/*lint -save -e537*/
#include <taskLib.h>
#include <osl_bio.h>
#include <osl_wait.h>
#include <osl_types.h>
#include <drv_comm.h>
#include <hi_rtc.h>
#include <bsp_memmap.h>
#if defined(CONFIG_HI6551_RTC)&&defined(CONFIG_PMIC_HI6551)
#include <hi_smartstar.h>
#endif
#if defined(CONFIG_HI6559_RTC)&&defined(CONFIG_PMIC_HI6559)
#include <hi_bbstar.h>
#endif
#include <bsp_icc.h>
#include "rtc_balong.h"


/*lint -restore*/
struct rtc_control g_rtc_ctrl={
	HI_RTC_REGBASE_ADDR_VIRT,
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

#define LEAPS_THRU_END_OF(y) (((y)/4 - (y)/100) + (y)/400)


static s32 is_leap_year(u32 year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}
/*
 * The number of days in the month.
 */
static s32 rtc_month_days(u32 month, u32  year)
{
	return g_rtc_ctrl.rtc_days_in_month[month] + (s32)(is_leap_year(year) && (month == 1));
}

/*
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
static void rtc_time_to_tm(u32 time, struct rtc_time *tm)
{
	unsigned int month, year;
	int days;

	days = (int)(time / 86400);
	time -= (unsigned int) days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = (unsigned int)(1970 + days / 365);
	days -= (s32)((year - 1970) * 365
		+ LEAPS_THRU_END_OF(year - 1)
		- LEAPS_THRU_END_OF(1970 - 1));
	if (days < 0) {
		year -= 1;
		days += 365 + is_leap_year(year);
	}
	tm->tm_year = (int)(year - 1900);
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = (int)(month);
	tm->tm_mday = days + 1;

	tm->tm_hour = (int)(time / 3600);
	time -= (u32)(tm->tm_hour * 3600);
	tm->tm_min = (int)(time / 60);
	tm->tm_sec = (int)((int)time - tm->tm_min * 60);

	tm->tm_isdst = 0;
}
#ifndef CONFIG_HI3630_RTC0
#if defined(CONFIG_HI6551_RTC)
/*****************************************************************************
 函 数 名  : hi6551_rtc_read_reg
 功能描述  : 读取RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint -save -e958 -e701*/
u32 rtc_read_reg(u16 addr)
{
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;
    u32 value = 0;

    bsp_hi6551_reg_read(addr + 0, &val1);
    bsp_hi6551_reg_read(addr + 1, &val2);
    bsp_hi6551_reg_read(addr + 2, &val3);
    bsp_hi6551_reg_read(addr + 3, &val4);

    value = (u32)(val1 | (val2 << 8) | (val3 << 16) | (val4 << 24));

    return value;
}
/*lint -restore*/
/*****************************************************************************
 函 数 名  : hi6551_rtc_write_reg
 功能描述  : 写RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : RTC_OK
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint -save -e958*/
u32 rtc_write_reg(u16 addr, u32 pValue)
{
/*lint -restore*/
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;

    val1 = ((pValue)>>0)&(0xFF);
    val2 = ((pValue)>>8)&(0xFF);
    val3 = ((pValue)>>16)&(0xFF);
    val4 = ((pValue)>>24)&(0xFF);

    bsp_hi6551_reg_write( addr + 0, val1);
    bsp_hi6551_reg_write( addr + 1, val2);
    bsp_hi6551_reg_write( addr + 2, val3);
    bsp_hi6551_reg_write( addr + 3, val4);

    return BSP_OK;
}
#endif
#if defined(CONFIG_HI6559_RTC)
/*****************************************************************************
 函 数 名  : hi6559_rtc_read_reg
 功能描述  : 读取RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint -save -e958 -e701*/
u32 rtc_read_reg(u16 addr)
{
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;
    u32 value = 0;

    bsp_hi6559_reg_read(addr + 0, &val1);
    bsp_hi6559_reg_read(addr + 1, &val2);
    bsp_hi6559_reg_read(addr + 2, &val3);
    bsp_hi6559_reg_read(addr + 3, &val4);

    value = (u32)(val1 | (val2 << 8) | (val3 << 16) | (val4 << 24));

    return value;
}
/*lint -restore*/
/*****************************************************************************
 函 数 名  : hi6559_rtc_write_reg
 功能描述  : 写RTC寄存器
 输入参数  : void
 输出参数  : 无
 返 回 值  : RTC_OK
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint -save -e958*/
u32 rtc_write_reg(u16 addr, u32 pValue)
{
/*lint -restore*/
    u8 val1 = 0;
    u8 val2 = 0;
    u8 val3 = 0;
    u8 val4 = 0;

    val1 = ((pValue)>>0)&(0xFF);
    val2 = ((pValue)>>8)&(0xFF);
    val3 = ((pValue)>>16)&(0xFF);
    val4 = ((pValue)>>24)&(0xFF);

    bsp_hi6559_reg_write( addr + 0, val1);
    bsp_hi6559_reg_write( addr + 1, val2);
    bsp_hi6559_reg_write( addr + 2, val3);
    bsp_hi6559_reg_write( addr + 3, val4);

    return BSP_OK;
}
#endif


s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm)
{
    u32 value = 0;

    dev = dev;
#if defined(CONFIG_BALONG_RTC)
    value = readl(g_rtc_ctrl.rtc_base_addr + HI_RTC_CCVR_OFFSET);
#elif defined(CONFIG_HI6551_RTC)
    value = rtc_read_reg(HI6551_RTCCR_A0_OFFSET);
#elif defined(CONFIG_HI6559_RTC)
    value = rtc_read_reg(HI6559_RTCCR_A0_OFFSET);
#endif
    rtc_time_to_tm(value, tm);
    tm->tm_year += RTC_BASE_YEAR;
	rtc_dbg( "%4d-%02d-%02d %02d:%02d:%02d\n",
		tm->tm_year, tm->tm_mon, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
    /*lint -save -e438*/
	return BSP_OK;
    /*lint -restore*/
}
/*
* set RTC time
* tm: the time to be set
*/
s32 balong_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	u32 channel_id_settime = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_RTC_SETTIME;
	s32 ret = 0;
    u32 i = 0;
    u32 len = sizeof(*tm);
    dev = dev;

    for(i = 0;i<RTC_ICC_SEND_COUNT;i++)
    {
        ret = bsp_icc_send(ICC_CPU_APP, channel_id_settime, (u8 *)tm, len);
        if((s32)ICC_INVALID_NO_FIFO_SPACE == ret)/*消息队列满,则50ms之后重新发送*/
        {
            taskDelay(5);
            continue;
        }
        else if(ret != (s32)len)
        {
            rtc_print_error("ret :0x%x,len 0x%x\n",ret,len);
            return BSP_ERROR;
        }
        else
        {
            return BSP_OK;
        }
    }
    /*lint -save -e438*/
    return BSP_OK;
    /*lint -restore*/
}

/*
* adapt v9r1
*/
u32 balong_get_rtc_value (void)
{
    u32 rtc_value = 0;
#if defined(CONFIG_BALONG_RTC)
	rtc_value = readl(g_rtc_ctrl.rtc_base_addr + HI_RTC_CCVR_OFFSET);
#elif defined(CONFIG_HI6551_RTC)
	rtc_value = rtc_read_reg(HI6551_RTCCR_A0_OFFSET);
#elif defined(CONFIG_HI6559_RTC)
	rtc_value = rtc_read_reg(HI6559_RTCCR_A0_OFFSET);
#endif

    return rtc_value;
}
#else
s32 balong_rtc_readtime(struct device *dev, struct rtc_time *tm)
{
    u32 value = 0;

    dev = dev;
    value = readl(g_rtc_ctrl.rtc_base_addr + HI_RTC_CCVR_OFFSET);
    rtc_time_to_tm(value + DELTA_BejingAndGregorian, tm);

    tm->tm_year += RTC_BASE_YEAR;

    /*lint -save -e438*/
	return BSP_OK;
    /*lint -restore*/
}

#endif
