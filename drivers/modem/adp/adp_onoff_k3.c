#include <drv_onoff.h>
#include <bsp_reset.h>
#include <product_config.h>

#ifndef CONFIG_BALONG_ONOFF
int bsp_start_mode_get(void)
{
    return 1;
}

void bsp_drv_power_off(void)
{
    return;
}

void drv_shut_down( DRV_SHUTDOWN_REASON_ENUM enReason )
{
    return;
}

/******************************************************************************
*  Function:  bsp_drv_power_reboot_direct
*  Description:
*  Input:
*         None
*  Output:
*         None
*  Return:
*         None
*  Note  : ÷±Ω”÷ÿ∆Ù
********************************************************************************/
void bsp_drv_power_reboot_direct( void )
{
    return;
}
#endif

