
#include <product_config.h>


#ifndef CONFIG_CPUFREQ

#include <bsp_cpufreq.h>
#include <drv_comm.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*提供pm流程调用，设置最大频率CPU/DDR*/
void cpufreq_set_max_freq(void)
{
	
}
/*
 * 获取当前profile
 */
int pwrctrl_dfs_get_profile()
{
	return 0;
}
/*
 * 设置profile
 * success: return BSP_OK
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_profile(int profile)
{
	return BSP_OK;
}

/*
 * 设置profile下限
 * success: return BSP_OK	
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_baseprofile(int baseprofile)
{
	return BSP_OK;
}

/*
 * 锁定调频 lock=0锁定;lock=1解锁
 */
void pwrctrl_dfs_lock(u32 lock)
{
	
}

/*
 * 调试接口，设置频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 *
 */
int pwrctrl_dfs_target(int a9freq, int ddrfreq, int slowfreq)
{
	return BSP_OK;
}

int pwrctrl_dfs_current(int *a9freq, int *ddrfreq, int *slowfreq)
{
	return BSP_OK;
}

void cpufreq_update_frequency(void)
{
	
}
unsigned int cpufreq_calccpu_load_netif(void)
{
	return BSP_OK;
}

#ifdef __cplusplus
}
#endif
#endif
