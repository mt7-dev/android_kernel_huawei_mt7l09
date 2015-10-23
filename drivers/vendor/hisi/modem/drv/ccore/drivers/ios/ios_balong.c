#include "product_config.h"
#include "bsp_ipc.h"
#include "bsp_version.h"
#include "bsp_om.h"
#include "bsp_reg_def.h"
#include "gpio_balong.h"
#include "soc_memmap.h"


#if defined(BSP_CONFIG_P531_ASIC)
#include "ios_drv_macro.h"

#elif (defined(BSP_CONFIG_V7R2_SFT) || defined(BSP_CONFIG_V7R2_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"

#endif
#include "osl_bio.h"
#include "ios_list.h"
#include "ios_balong.h"


void rf_reset_to_gpio_mux(void)
{
#ifdef BSP_CONFIG_V7R2_ASIC
	bsp_gpio_direction_output(GPIO_1_13,1);
	bsp_gpio_direction_output(GPIO_1_19,1);

    bsp_ipc_spin_lock(IPC_SEM_GPIO);
	/*配置RF线控：CH0 FEM(6PIN）*/
    /*gpio1[13]管脚复用配置*/
    SET_IOS_GPIO1_13_CTRL1_1;
    CLR_IOS_CH0_RF_CTRL1_1;

    /*gpio1[19]管脚复用配置*/
    SET_IOS_GPIO1_19_CTRL1_1;
    CLR_IOS_CH1_RF_CTRL1_1;

	/*gpio1[13]管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL5);
    add_ios_list(IOS_PD_AF_CTRL8);

    /*gpio1[19]管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL6);
    add_ios_list(IOS_PD_AF_CTRL10);

    bsp_ipc_spin_unlock(IPC_SEM_GPIO);
#endif
}
// k3 如何处理
void gpio_to_rf_reset_mux(void)
{
#ifdef BSP_CONFIG_V7R2_ASIC
    bsp_ipc_spin_lock(IPC_SEM_GPIO);
    
	SET_IOS_CH0_RF_CTRL1_1;
	SET_IOS_CH1_RF_CTRL1_1;
	
	CLR_IOS_GPIO1_13_CTRL1_1;
	CLR_IOS_GPIO1_19_CTRL1_1;
	
    bsp_ipc_spin_unlock(IPC_SEM_GPIO);
#endif
}


