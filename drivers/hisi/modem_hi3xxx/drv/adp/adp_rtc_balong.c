
#include <product_config.h>

#if defined(CONFIG_BALONG_RTC)
#include "rtc_balong/rtc_balong.h"

unsigned int DRV_GET_RTC_VALUE (void)
{
    return balong_get_rtc_value();
}
#elif defined(CONFIG_HI6551_RTC)
#include "rtc_hi6551/rtc_hi6551.h"

unsigned int DRV_GET_RTC_VALUE (void)
{
    return hi6551_get_rtc_value();
}
#elif defined(CONFIG_HI6559_RTC)
#include "rtc_hi6559/rtc_hi6559.h"

unsigned int DRV_GET_RTC_VALUE (void)
{
    return hi6559_get_rtc_value();
}
#else
unsigned int DRV_GET_RTC_VALUE (void)
{
    return 0;
}
#endif

