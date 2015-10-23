



#ifdef __cplusplus
extern "C"
{
#endif
#include "drv_comm.h"
#include "sec_balong.h"

#ifndef BSP_HAS_SEC_FEATURE
BSP_S32 BSP_SEC_Check()
{
    return 0;
}

BSP_S32 BSP_SEC_Start()
{
    return 0;
}

BSP_S32 BSP_SEC_AlreadyUse(BSP_U8 * p_data)
{
	if(p_data)
		*p_data = 0;
    return 0;
}
#endif

#ifdef __cplusplus
}
#endif

