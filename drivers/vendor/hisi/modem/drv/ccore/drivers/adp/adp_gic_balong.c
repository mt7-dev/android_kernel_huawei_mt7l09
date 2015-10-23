/*lint --e{732}*/
#include <drv_int.h>
#include <intLib.h>
#include <iv.h>
#include <osl_types.h>

BSP_S32 BSP_INT_Connect(VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    return intConnect(vector,routine,parameter);
}
BSP_S32 BSP_INT_Disable (BSP_S32 s32Lvl)
{
    return intDisable(s32Lvl);
}
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl)
{
    return intEnable(s32Lvl);
}
/*lint -save -e18 -e64*/

unsigned int DRV_INT_GET_CONTEXT(BSP_VOID)
{
    return (unsigned int)INT_CONTEXT();
}
/*lint -restore +e18 +e64*/

