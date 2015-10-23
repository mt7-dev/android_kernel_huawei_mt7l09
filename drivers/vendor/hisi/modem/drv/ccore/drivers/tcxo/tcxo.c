
#include "tcxo_balong.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifndef CONFIG_TCXO_BALONG
void bsp_pmu_tcxo1_en_enable(PMU_TCXO_EN bit_value)
{
	return;
}
void bsp_pmu_tcxo1_en_disable(PMU_TCXO_EN bit_value)
{
	return;
}
int bsp_tcxo_enable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType)
{
    return 0;
}
int bsp_tcxo_disable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType)
{
    return 0;
}

int bsp_tcxo_getstatus(PWC_COMM_MODEM_E enModemId)
{
    return PWRCTRL_COMM_ON;
}
int bsp_tcxo_timer_start(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType, PWC_TCXO_FUNCPTR routine, int arg, unsigned int timerValue)
{
    return 0;
}
int tcxo_init_configure()
{
    return 0;
}


s32 BSP_TCXO_RfclkStatus(PWC_COMM_MODEM_E enModemId)
{
    return 0;
}

s32 BSP_TCXO_RfclkEnable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType)
{
    return 0;
}

s32 BSP_TCXO_RfclkDisable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType)
{
    return 0;
}

#endif

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

