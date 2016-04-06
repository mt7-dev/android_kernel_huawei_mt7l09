#ifndef __RSE_BALONG_H__
#define __RSE_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif


int bsp_rf_rse_init(void);
int bsp_rse_mipi_base_config(PWC_COMM_MODEM_E modem_id);
int bsp_rse_mipi_config_on(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

