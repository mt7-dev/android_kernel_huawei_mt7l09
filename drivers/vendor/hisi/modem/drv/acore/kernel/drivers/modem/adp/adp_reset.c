
#include <product_config.h>
#include <bsp_om.h>
#include <drv_reset.h>
#include <bsp_reset.h>

#ifndef CONFIG_BALONG_MODEM_RESET /* ´ò×®!CONFIG_BALONG_MODEM_RESET */

int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
    return 0;
}

s32 bsp_reset_ccpu_status_get(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return 1;
}

s32 bsp_reset_cb_func_register(const char *name, pdrv_reset_cbfun func, int user_data, int prior)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return 0;
}

void bsp_modem_power_off(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return;
}

void bsp_modem_power_on(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return;
}

int bsp_modem_reset(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return -1;
}

u32 bsp_reset_is_feature_on(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return 0;
}

u32 bsp_reset_is_connect_ril(void)
{
	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_RESET, "[reset]: <%s> is stub\n", __FUNCTION__);
	return 0;
}


#else  /* ÊµÏÖ */

int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    return bsp_reset_cb_func_register(pname, pcbfun, userdata, priolevel);
}

#endif /* end of CONFIG_BALONG_MODEM_RESET */

#ifndef CONFIG_HIFI_RESET
int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
	return 0;
}

#endif /* end of CONFIG_HIFI_RESET */

