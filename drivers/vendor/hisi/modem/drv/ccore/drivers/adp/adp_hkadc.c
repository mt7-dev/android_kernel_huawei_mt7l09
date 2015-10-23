#include <osl_types.h>

#include <bsp_hkadc.h>


s32 BSP_HKADC_PaValueGet(u16 *value);
s32 DRV_HKADC_BAT_VOLT_GET(s32 *ps32Data);
s32 DRV_GET_BATTERY_ADC(s32 * pslData);


s32 BSP_HKADC_PaValueGet(u16 *value)
{
    return bsp_hkadc_convert(HKADC_CHANNEL_3, value);
}

s32 DRV_HKADC_BAT_VOLT_GET(s32 *ps32Data)
{
    s32 ret;
    u16 value = 0;

    if (NULL == ps32Data)
        return -1;

    ret = bsp_hkadc_convert(HKADC_CHANNEL_8, &value);
    *ps32Data = value * 2;

    return ret;
}

s32 DRV_GET_BATTERY_ADC(s32 * pslData)
{
    s32 ret;
    u16 value = 0;

    if (NULL == pslData)
        return -1;

    ret = bsp_hkadc_convert(HKADC_CHANNEL_8, &value);
    *pslData = value;

    return ret;
}


