/*lint --e{718,746}*/
#include <bsp_memmap.h>   
#include <osl_types.h>
#include <osl_bio.h>  
#include "drv_sysctrl.h" 

int BSP_SYSCTRL_REG_SET(unsigned int regAddr, unsigned int setData)
{
	writel(setData, HI_SYSCTRL_BASE_ADDR + regAddr);
	return 0;
} 

int BSP_SYSCTRL_REG_GET(unsigned int regAddr, unsigned int * getData)
{
	if (NULL == getData)
	{
		/*lint -save -e18*/
		printf("DRV_SYSCTRL_REG_GET:getData buffer is NULL!\n");
		/*lint -restore +e18*/
		return -1;
	}
	* getData = readl(HI_SYSCTRL_BASE_ADDR + regAddr);
	return 0;
} 

