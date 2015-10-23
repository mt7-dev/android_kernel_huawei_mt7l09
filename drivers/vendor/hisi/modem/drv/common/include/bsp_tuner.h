
#ifndef __BSP_TUNER_H__
#define __BSP_TUNER_H__


#ifdef __cplusplus
extern "C" {
#endif

#define NV_GU_RF_FEND_MIPI_INIT_CONFIG_ID (10046)


void bsp_tuner_init(void);
int bsp_tuner_resume(struct dpm_device *dev);


#ifdef __cplusplus
}
#endif

#endif

