/*
 *  Copyright (C), 2011-2013, Hisilicon Technologies Co., Ltd.
 *
 * File name     : rtc_balong.h
 * Author        : fuxin 0021597
 * Created       : 2012-12-26
 * Last Modified :
 * Description   : the RTC head file
 * Modification  : Create file
 *
 */
#ifndef __RTC_BALONG_H__
#define __RTC_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <bsp_om.h>

#define RTC_BIT_AI	(1 << 0) /* Alarm interrupt bit */
#define RTC_BIT_IM	(1 << 1) /* Interrupt mask and set bit */

#define ALARM_ENABLE    (1)
#define ALARM_DISABLED  (0)

#define RTC_INT_ID 36
#define RTC_BASE_YEAR   1900 /*RTC base year*/

/*RTC 全局变量结构体*/
struct rtc_control
{
	u32 rtc_base_addr;/*RTC 寄存器基地址*/
};

extern struct rtc_control g_rtc_ctrl;

#define  rtc_print_error(fmt,...)      (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RTC, "[rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  rtc_dbg(fmt,...)		       (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_RTC, "[rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))


int balong_rtc_readtime(struct device *dev, struct rtc_time *tm);
int balong_rtc_settime(struct device *dev, struct rtc_time *tm);
int balong_alarm_irq_enable(struct device *dev, unsigned int enabled);
int balong_rtc_readalarm(struct device *dev, struct rtc_wkalrm *alarm);
int balong_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm);
int  balong_rtc_alarmhandler(int irq, void *data);
u32 balong_get_rtc_value (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
