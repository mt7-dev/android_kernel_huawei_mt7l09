/*lint --e{537}*/
#include <bsp_vic.h>
#include <drv_int.h>
#include <drv_hw_adp.h>
#include <soc_interrupts.h>
BSP_S32	adp_int_enable(int ulLvl)
{
	if(ulLvl<INT_LVL_MAX && ulLvl>=0)
		return BSP_INT_Enable(ulLvl);
	else if(ulLvl>=INT_LVL_MAX && ulLvl<INT_LVL_MAX+64)
	{
	#ifdef CONFIG_MODULE_VIC
		return bsp_vic_enable(ulLvl-INT_LVL_MAX);
	#else
		return -1;
	#endif
	}
	else
		return -1;
}
BSP_S32	adp_int_disable(int ulLvl)
{
	if(ulLvl<INT_LVL_MAX && ulLvl>=0)
		return BSP_INT_Disable(ulLvl);
	else if(ulLvl>=INT_LVL_MAX && ulLvl<INT_LVL_MAX+64)
	{
	#ifdef CONFIG_MODULE_VIC
		return bsp_vic_disable(ulLvl-INT_LVL_MAX);
	#else
		return -1;
	#endif
	}
	else
		return -1;
}
BSP_S32 adp_int_connect(int ulLvl,FUNCPTR_1 routine,void* parameter)
{

	if(ulLvl<INT_LVL_MAX && ulLvl>=0)
		return BSP_INT_Connect(ulLvl, (VOIDFUNCPTR)routine, parameter);
	else if(ulLvl>=INT_LVL_MAX && ulLvl<INT_LVL_MAX+64)
	{
	#ifdef CONFIG_MODULE_VIC
		return bsp_vic_connect(ulLvl-INT_LVL_MAX,(vicfuncptr)routine,parameter);
	#else
		return -1;
	#endif
	}
	else
		return -1;
}