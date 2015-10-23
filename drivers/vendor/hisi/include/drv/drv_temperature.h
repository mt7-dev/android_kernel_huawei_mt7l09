

#ifndef __DRV_TEMPERATURE_H__
#define __DRV_TEMPERATURE_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef enum tagHKADC_CONV_MODE_E
{
	HKADC_CONV_DELAY =0,
	HKADC_CONV_NO_DELAY,
	HKADC_CONV_BUTT
}HKADC_CONV_MODE_E;
typedef enum HKADC_TEMP_PROTECT
{
    HKADC_TEMP_TOP = 0,
    HKADC_TEMP_BATTERY = HKADC_TEMP_TOP,
    HKADC_TEMP_PA0,
    HKADC_TEMP_PA1,
    HKADC_TEMP_DCXO0,
    HKADC_TEMP_DCXO1,
    HKADC_TEMP_LCD,
    HKADC_TEMP_SIM_CARD,
    HKADC_TEMP_DCXO0_LOW,
    HKADC_TEMP_DCXO1_LOW,
    HKADC_TEMP_BUTT,
}HKADC_TEMP_PROTECT_E;


/* HKADC获取温度信息 */
extern int drv_hkadc_get_temp(HKADC_TEMP_PROTECT_E enMode, unsigned short* pusVolt, short* psTemp, HKADC_CONV_MODE_E ulMode);
#define DRV_HKADC_GET_TEMP(enMode, pusVolt, psTemp,ulMode) drv_hkadc_get_temp(enMode, pusVolt, psTemp,ulMode)

#ifdef __cplusplus
}
#endif

#endif

