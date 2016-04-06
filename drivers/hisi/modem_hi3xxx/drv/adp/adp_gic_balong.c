#include <drv_int.h>
#include <osl_types.h>
#include <osl_irq.h>

BSP_S32 BSP_INT_Connect(int vector,VOIDFUNCPTR routine, void* parameter)
{
    return request_irq(vector,(irq_handler_t) routine, 0, "other", parameter);
}
BSP_S32 BSP_INT_Disable (BSP_S32 s32Lvl)
{
    return 0;
}
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl)
{
    return 0;
}
