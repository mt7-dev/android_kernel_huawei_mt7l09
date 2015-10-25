


#ifndef __RTC_BALONG_H__
#define __RTC_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <linux/rtc.h>
#include <product_config.h>
#include <hi_smartstar.h>
#include <bsp_om.h>
#include <bsp_pmu.h>



#define ALARM_ENABLE   (1)
#define ALARM_DISABLED (0)

#define RTC_INT_ID      36
#define RTC_BASE_YEAR   1900 /*RTC base year*/
#define HI6551_RTC_DEVICE_NUM   6
#define RTC_OK          0
#define RTC_ERROR       -1

struct hi6551_alarm_para
{
    s32 alarm_id;
    struct rtc_device *rtc_dev;
};

/*RTC 全局变量结构体*/
struct rtc_control
{
    u16 rtc_cur_addr;               /*当前值寄存器*/
    u16 rtc_clr_addr;               /*加载值寄存器*/
    u32 alarm_irq[HI6551_RTC_DEVICE_NUM];
    u16 alarm_base_addr[HI6551_RTC_DEVICE_NUM];
};

#define  hi6551_rtc_print_error(fmt,...)      (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HI6551_RTC, "[hi6551_rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  hi6551_rtc_print_dbg(fmt,...)		       (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_HI6551_RTC, "[hi6551_rtc]: <%s> <%d>"fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__))


int hi6551_rtc_readtime(struct device *dev, struct rtc_time *tm);
int hi6551_rtc_settime(struct device *dev, struct rtc_time *tm);
int hi6551_alarm_irq_enable(struct device *dev, unsigned int enabled);
int hi6551_rtc_readalarm(struct device *dev, struct rtc_wkalrm *alarm);
int hi6551_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm);
void hi6551_rtc_alarmhandler(void *data);
u32 hi6551_get_rtc_value (void);
u32 hi6551_rtc_write_reg(u16 addr, u32 pValue);
u32 hi6551_rtc_read_reg(u16 addr);
u32 hi6551_rtc_show_level(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
