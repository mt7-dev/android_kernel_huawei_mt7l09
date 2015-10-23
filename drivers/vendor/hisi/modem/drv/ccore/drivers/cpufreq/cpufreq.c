
#include <bsp_cpufreq.h>
#include <drv_comm.h>

#ifdef __cplusplus
extern "C"
{
#endif


void  cpufreq_init(void)
{
    return ;
}
/*提供pm流程调用，设置最大频率CPU/DDR*/
void cpufreq_set_max_freq(void)
{
	
}
/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCcpuLoadCB
 功能描述  : arm 提供给TTF的回调函数
 输入参数  : pFunc:TTF函数指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
 void BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc )
 {
	(void)(*pFunc)(1);
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
void cpufreq_checkload_in_sysint(void)
{

}
/*for v9r1*/
BSP_S32 PWRCTRL_DfsQosRequest(BSP_S32 qos_id, BSP_U32 req_value, BSP_S32 *req_id)
{
	return 0;
}
BSP_S32 PWRCTRL_DfsQosRelease(BSP_S32 qos_id, BSP_S32 *req_id)
{
	return 0;
}
BSP_S32 PWRCTRL_DfsQosUpdate(BSP_S32 qos_id, BSP_S32 req_id, BSP_U32 req_value)
{
	return 0;
}

/********************************adp interface start**************************************/

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
 unsigned int DRV_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad)
 {
	return 0;
 }


/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_SET_PRF_CCPU
Description:Set the System Min Profile
Input:      ulClientID: The Vote Module Client
            enProfile:  The Min Profile Value
Output:     None
Return:     The Vote Result
Others:
*****************************************************************************/
 BSP_U32  DRV_PWRCTRL_DFS_SET_PRF_CCPU(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile)
 {
	return 0;
 }

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_RLS_PRF_CCPU
Description:Release the Vote Result
Input:      ulClientID: The Vote Module Client
Output:     None;
Return:     The Vote Result
Others:
*****************************************************************************/
BSP_U32 DRV_PWRCTRL_DFS_RLS_PRF_CCPU(PWC_DFS_ID_E ulClientID)
{
	return 0;
}
 /*****************************************************************************
 函 数 名  : BSP_DFS_GetCurCpuLoad
 功能描述  : 查询当前CPU
 输入参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 输出参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
BSP_U32 BSP_DFS_GetCurCpuLoad(BSP_U32 *pu32AcpuLoad,BSP_U32 *pu32CcpuLoad)
{
    return ERROR;
}
/********************************adp interface end***************************************/

#ifdef __cplusplus
}
#endif

