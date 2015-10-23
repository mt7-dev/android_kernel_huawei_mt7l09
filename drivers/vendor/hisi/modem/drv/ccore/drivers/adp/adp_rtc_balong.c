
#include <product_config.h>

#if defined(CONFIG_BALONG_RTC) || defined(CONFIG_HI6551_RTC)
#include "rtc_balong.h"

unsigned int DRV_GET_RTC_VALUE (void)
{
    return balong_get_rtc_value();
}
#else
unsigned int DRV_GET_RTC_VALUE (void)
{
    return 0;
}
#endif

