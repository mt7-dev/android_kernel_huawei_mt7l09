
#include <bsp_lowpower_mntn.h>
#include "cpufreq_balong.h"
#ifdef __cplusplus
extern "C"
{
#endif


extern s32 g_cur_profile;
extern int g_cpufreq_lock_status_flag;
extern struct DFS_Qos_V7V9 g_v9_qos_list;
extern struct cpufreq_query balong_query_profile_table[];

extern int balong_cpufreq_icc_send(struct cpufreq_msg *msg);

/*分配的最后一个request ID*/
int g_last_req_id = 0;

struct DFS_Qos_V7V9 g_diffmode_with_vote[END_OF_DIFF_MODE] =
{
	[DIFF_MODE_OF_LT] = {
				.request_id = DIFF_MODE_OF_LT,
				.request_flag = 0,
				.request_profile = BALONG_FREQ_MIN,
	},
	[DIFF_MODE_OF_GU] = {
				.request_id = DIFF_MODE_OF_GU,
				.request_flag = 0,
				.request_profile = BALONG_FREQ_MIN,
	},
};

static int pwrctrl_find_min_profile(unsigned int a9freq, unsigned int ddrfreq, unsigned int slowfreq)
{
	unsigned int target_profile = DC_RESV;
	int i = 0;

	for (i = BALONG_FREQ_MIN; i < DC_RESV; i++)
	{
		if ((balong_query_profile_table[i].cpu_frequency >= a9freq) && (balong_query_profile_table[i].ddr_frequency >= ddrfreq)
					&& (balong_query_profile_table[i].sbus_frequency >= slowfreq))
		{
			if (target_profile >= balong_query_profile_table[i].profile)
			{
				target_profile = balong_query_profile_table[i].profile;
			}
		}
	}
	if (DC_RESV == target_profile)
	{
		target_profile = BALONG_FREQ_MAX;
	}
	return (int)target_profile;
}



/*
 * 获取当前profile
 */
int pwrctrl_dfs_get_profile(void)
{
	g_cur_profile = (s32)CPUFREQ_CUR_PROFILE;
	if ((g_cur_profile < BALONG_FREQ_MIN) || (g_cur_profile > BALONG_FREQ_MAX))
	{
		g_cur_profile = BALONG_FREQ_MAX;
		cpufreq_info("m3 cpufreq return right cur_profile value? %d\n", g_cur_profile);
	}
	if(g_lowpower_shared_addr)
	{
		writel(0xabababab, g_lowpower_shared_addr + 0x200);
		writel(g_cur_profile, g_lowpower_shared_addr + 0x204);
	}
	return g_cur_profile;
}
/*
 * 设置profile
 * success: return BSP_OK
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_profile(int profile)
{
	struct cpufreq_msg set_msg = {0,0,0,0};
	int cur_profile = 0;
	if ((profile < BALONG_FREQ_MIN) || (profile > BALONG_FREQ_MAX))
	{
		cpufreq_err("profile in right bound??%d\n", profile);
		return BSP_ERROR;
	}
	set_msg.msg_type = CPUFREQ_ADJUST_FREQ;
	set_msg.source = CPUFREQ_CCORE;
	cur_profile = pwrctrl_dfs_get_profile();
	if (cur_profile < profile)
	{
		set_msg.content = DFS_PROFILE_UP_TARGET;
	}
	else if (cur_profile > profile)
	{
		set_msg.content = DFS_PROFILE_DOWN_TARGET;
	}
	else
	{
		return BSP_OK;
	}

	set_msg.profile = (u32)profile;
	return balong_cpufreq_icc_send(&set_msg);
}
void pwrctrl_dfs_set_limitprofile_with_voteid(int baseprofile, int request_mode_id)
{
	int i = 0;
	unsigned long irqlock = 0;
	int target_profile = BALONG_FREQ_MIN;
	local_irq_save(irqlock);
	for (i = 0; i < END_OF_DIFF_MODE; i++)
	{
		if (request_mode_id == g_diffmode_with_vote[i].request_id)
		{
			g_diffmode_with_vote[i].request_flag = 1;
			g_diffmode_with_vote[i].request_profile = baseprofile;
		}
		if ((1 == g_diffmode_with_vote[i].request_flag) && (g_diffmode_with_vote[i].request_profile > target_profile))
		{
			target_profile = g_diffmode_with_vote[i].request_profile;
		}
	}
	local_irq_restore(irqlock);
	pwrctrl_dfs_set_baseprofile(target_profile);
}
/*
 * 设置profile下限
 * success: return BSP_OK
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_baseprofile(int baseprofile)
{
	struct cpufreq_msg set_msg = {0,0,0,0};
	if ((baseprofile < BALONG_FREQ_MIN) || (baseprofile > BALONG_FREQ_MAX))
	{
		cpufreq_err("profile in right bound??%d\n", baseprofile);
		return BSP_ERROR;
	}
	set_msg.msg_type = CPUFREQ_ADJUST_FREQ;
	set_msg.source = CPUFREQ_CCORE;
	set_msg.content = DFS_PROFILE_DOWN_LIMIT;
	set_msg.profile = (u32)baseprofile;
	return balong_cpufreq_icc_send(&set_msg);
}
/*
 * 锁定调频 DFS_PROFILE_LOCKFREQ=0锁定;DFS_PROFILE_LOCKFREQ=1解锁
 */
void pwrctrl_dfs_lock(u32 lock)
{
	struct cpufreq_msg set_msg = {0,0,0,0};
	cpufreq_debug("cpufreq lock status is: %d\n", g_cpufreq_lock_status_flag);
	g_cpufreq_lock_status_flag = (s32)lock;

	set_msg.content = lock;

	set_msg.msg_type = CPUFREQ_LOCK_MCORE_ACTION;
	set_msg.source = CPUFREQ_CCORE;
	balong_cpufreq_icc_send(&set_msg);
}
/*
 * 设置频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 * 入参分别为当前核的cpu频率，ddr频率，slowbus频率
 * 注:因C核profile4、5的频率相同，
 * 若想调至最高频请将ccpu频率设为最高频+1
 */
int pwrctrl_dfs_target(int a9freq, int ddrfreq, int slowfreq)
{
	int target_profile = 0;
	int a9_freq = 0;
	int ddr_freq = 0;
	int slow_freq = 0;
	
	a9_freq = (a9freq < 0) ? (0) : (a9freq /CPUFREQ_ARGV_KHZ2MHZ);
	ddr_freq = (ddrfreq < 0) ? (0) : (ddrfreq /CPUFREQ_ARGV_KHZ2MHZ);
	slow_freq = (slowfreq < 0) ? (0) : (slowfreq /CPUFREQ_ARGV_KHZ2MHZ);
	
	target_profile = pwrctrl_find_min_profile((u32)a9_freq, (u32)ddr_freq, (u32)slow_freq);
	cpufreq_debug("prolfie : %d\n", target_profile);
	pwrctrl_dfs_set_limitprofile_with_voteid(target_profile, DIFF_MODE_OF_LT);
	return BSP_OK;
}
/*
 * 获取当前频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 * 
 */
int pwrctrl_dfs_current(int *a9freq, int *ddrfreq, int *slowfreq)
{
	int cur_profile = 0;
	int ret = BSP_OK;
	if ((a9freq != NULL) && (ddrfreq != NULL) && (slowfreq != NULL))
	{
		cur_profile = pwrctrl_dfs_get_profile();
		*a9freq = (s32)balong_query_profile_table[cur_profile].cpu_frequency * CPUFREQ_ARGV_KHZ2MHZ;
		*ddrfreq = (s32)balong_query_profile_table[cur_profile].ddr_frequency * CPUFREQ_ARGV_KHZ2MHZ;
		*slowfreq = (s32)balong_query_profile_table[cur_profile].sbus_frequency * CPUFREQ_ARGV_KHZ2MHZ;
	}
	else
	{
		cpufreq_err("argv is NULL,check it\n");
		ret = BSP_ERROR;
	}
	return ret;
}
/*通知M3是否锁定lock_flag=0:锁定M3调频，lock_flag=1:解锁m3调频 */
void cpufreq_lock_mcore_freq(bool lock_flag)
{
	struct cpufreq_msg set_msg = {0,0,0,0};
	if ((lock_flag != 0) && (lock_flag != 1))
	{
		cpufreq_err("argv is error\n");
		return ;
	}
	set_msg.msg_type = CPUFREQ_LOCK_MCORE_ACTION;
	set_msg.source = CPUFREQ_CCORE;
	set_msg.content = (u32)lock_flag;
	balong_cpufreq_icc_send(&set_msg);
}
void cpufreq_set_ddrfreq(int ddrfreq)
{
	struct cpufreq_msg set_msg = {CPUFREQ_SET_DDR_LIMIT, CPUFREQ_CCORE, 1, 0};
	set_msg.profile = ddrfreq;
	if(g_lowpower_shared_addr)
	{
		writel(ddrfreq, g_lowpower_shared_addr + 0x208);
	}
	balong_cpufreq_icc_send(&set_msg);
}

/*提供pm流程调用，设置最大频率CPU DDR*/
void cpufreq_set_max_freq(void)
{
	pwrctrl_dfs_set_profile(BALONG_FREQ_MAX);
	//cpufreq_set_ddrfreq(BALONG_FREQ_MAX_DDR);
}
/******************************for v9r1 adp start**********************************/
/*balong_query_profile_table*/

/*获取大于等于请求频率的最小profile*/
int cpufreq_v9r1_get_ccore_profile(int request_freq)
{
	int target_profile = BALONG_FREQ_MAX;
	int i = 0;

	for (i = BALONG_FREQ_MIN; i < DC_RESV; i++)
	{
		if (balong_query_profile_table[i].cpu_frequency >= (unsigned int)request_freq)
		{
			target_profile = (s32)balong_query_profile_table[i].profile;
			goto ccore_out;
		}
	}
ccore_out:
	cpufreq_debug("ccore target_profile:%d\n", target_profile);
	return target_profile;
}
/*获取大于等于请求频率的最小profile*/
int cpufreq_v9r1_get_ddr_profile(int request_freq)
{
	int target_profile = BALONG_FREQ_MAX;
#ifdef CPUFREQ_IS_SYNC_DDR  /* 同步调频*/
	int i = 0;

	for (i = BALONG_FREQ_MIN; i < DC_RESV; i++)
	{
		if (balong_query_profile_table[i].ddr_frequency>= (unsigned int)request_freq)
		{
			target_profile = (s32)balong_query_profile_table[i].profile;
			goto ddr_out;
		}
	}
#else
	target_profile = request_freq;
	goto ddr_out;
#endif

ddr_out:
	cpufreq_debug("ddr target_profile:%d\n", target_profile);
	return target_profile;
}
/*获取大于等于请求频率的最小profile*/
int cpufreq_v9r1_get_bus_profile(int request_freq)
{
#if 0
	int target_profile = BALONG_FREQ_MAX;
	int i = 0;

	for (i = BALONG_FREQ_MIN; i < DC_RESV; i++)
	{
		if (balong_query_profile_table[i]>= request_freq)
		{
			target_profile = balong_query_profile_table[i].profile;
			goto bus_out;
		}
	}

bus_out:
	cpufreq_debug("ddr target_profile:%d\n", target_profile);
	return target_profile;
#endif
	return BSP_OK;
}

int cpufreq_v9r1_get_profile(int request_id, int request_freq)
{
	if (DFS_QOS_ID_CCPU_MINFREQ == request_id)
	{
		return cpufreq_v9r1_get_ccore_profile(request_freq);
	}
	else if (DFS_QOS_ID_DDR_MINFREQ == request_id)
	{
		return cpufreq_v9r1_get_ddr_profile(request_freq);
	}
	else
	{
		cpufreq_err("request_id is right?? %d\n", request_id);
		return BALONG_FREQ_MIN;
	}
}
#ifdef CPUFREQ_IS_SYNC_DDR  /* 同步调频*/
void cpufreq_qos_update_request(int qos_id)
{
	unsigned long irqlock = 0;
	struct DFS_Qos_V7V9 *qos_list = NULL;
	int target_profile = BALONG_FREQ_MIN;
	local_irq_save(irqlock);
	list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
	{
		if (qos_list->request_profile >= target_profile)
		{
			target_profile = qos_list->request_profile;
		}
	}
	local_irq_restore(irqlock);
	pwrctrl_dfs_set_limitprofile_with_voteid(target_profile, DIFF_MODE_OF_GU);
}
#else
void cpufreq_qos_update_request(int qos_id)
{
	unsigned long irqlock = 0;
	struct DFS_Qos_V7V9 *qos_list = NULL;
	int target_profile = BALONG_FREQ_MIN;
	
	if (qos_id == DFS_QOS_ID_CCPU_MINFREQ) /*cpu请求，发送profile*/
	{
		local_irq_save(irqlock);
		list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
		{
			if ((qos_list->request_type == DFS_QOS_ID_CCPU_MINFREQ) && (qos_list->request_profile >= target_profile))
			{
				target_profile = qos_list->request_profile;
			}
		}
		local_irq_restore(irqlock);
		pwrctrl_dfs_set_limitprofile_with_voteid(target_profile, DIFF_MODE_OF_GU);
	}
	else if (qos_id == DFS_QOS_ID_DDR_MINFREQ)/*ddr 请求，发送频率值*/
	{
		local_irq_save(irqlock);
		list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
		{
			if ((qos_list->request_type == DFS_QOS_ID_DDR_MINFREQ) && (qos_list->request_profile >= target_profile))
			{
				target_profile = qos_list->request_profile;
			}
		}
		local_irq_restore(irqlock);
		cpufreq_set_ddrfreq(target_profile);
	}
}
#endif

int cpufreq_find_rightid(int req_id)
{
	struct DFS_Qos_V7V9 *qos_list = NULL;
	list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
	{
		if (req_id == qos_list->request_id)
		{
			return BSP_ERROR;
		}
	}
	return BSP_OK;
}
/*返回req_id用于表示投票组件g_last_req_id 从1开始*/
int cpufreq_qos_request_with_reqid(int request_id, int request_freq)
{
	 /*lint --e{429 } */
	 int req_id = 0;
	 unsigned long irqlock = 0;
	 int tar_profile = BALONG_FREQ_MIN;
	 struct DFS_Qos_V7V9 *qos_request = NULL;
	 
 	 req_id = g_last_req_id + 1;
	 tar_profile = cpufreq_v9r1_get_profile(request_id, request_freq);
	 qos_request = (struct DFS_Qos_V7V9 *)malloc(sizeof(struct DFS_Qos_V7V9));/* [false alarm]:误报 */
	 if (NULL == qos_request)
	 {
		cpufreq_err("request:malloc failed\n");
		return BSP_ERROR;
	 }
 	local_irq_save(irqlock);
 	while((BSP_ERROR == cpufreq_find_rightid(req_id)))
 	{
		req_id = req_id + 1;
 	}
	qos_request->request_id = req_id;
	qos_request->request_type = request_id;
	qos_request->request_profile = tar_profile;
	qos_request->request_flag = 1;
	INIT_LIST_HEAD(&(qos_request->entry));
	list_add_tail(&(qos_request->entry), &(g_v9_qos_list.entry));
	g_last_req_id = req_id;
	local_irq_restore(irqlock);
	
	 return req_id;
}

void cpufreq_qos_release_with_reqid(int qos_id, int req_id)
{
	unsigned long irqlock = 0;
	struct DFS_Qos_V7V9 *qos_list = NULL;
	struct DFS_Qos_V7V9 *qos_release = NULL;
	local_irq_save(irqlock);
	list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
	{
		if (req_id == qos_list->request_id)
		{
			qos_release = qos_list;
			list_del(&(qos_list->entry));
			break;
		}
	}
 	local_irq_restore(irqlock);
	if(NULL != qos_release)
	{
		free(qos_release);
	}
}
/*****************************************************************************
 函 数 名  : PWRCTRL_DfsQosRequest
 功能描述  : 请求DFS QoS
 输入参数  : qos_id - QoS ID
             					req_value - 请求值
 输出参数  : req_id - 请求ID
 返 回 值  : 		DFS_RET_OK - Success
 调用函数  :
 被调函数  :
*****************************************************************************/
int PWRCTRL_DfsQosRequest(int qos_id, unsigned int req_value, int* req_id)
{
    if(!req_id)
        return RET_ERR_PARAM_NULL;

    bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MUDU_CPUFREQ, "qos_id:%d, req_value:%d\n", qos_id, req_value);

	*req_id = cpufreq_qos_request_with_reqid((int)qos_id, (int)req_value/1000);
	
	cpufreq_qos_update_request(qos_id);
    return RET_OK;
}


/*****************************************************************************
 函 数 名  : PWRCTRL_DfsQosRelease
 功能描述  : 释放DFS QoS
 输入参数  : qos_id - QoS ID
             req_id - 请求ID
 输出参数  : req_id = DFS_INVALID_ID if success
 返 回 值  : DFS_RET_OK - Success,
 调用函数  :
 被调函数  :
*****************************************************************************/
int PWRCTRL_DfsQosRelease(int qos_id, int* req_id)
{
	int req_release_id = 0;
    if(!req_id)
    {
        return RET_ERR_PARAM_NULL;
    }
    req_release_id = * req_id;
    cpufreq_err("qos_id:%d, req_id:%d\n", qos_id, *req_id);
    
	cpufreq_qos_release_with_reqid(qos_id, (int)req_release_id);

	cpufreq_qos_update_request(qos_id);
	
    *req_id = -1;

    return RET_OK;
}

/*****************************************************************************
 函 数 名  : PWRCTRL_DfsQosUpdate
 功能描述  : 更新DFS QoS
 输入参数  : qos_id - QoS ID
             req_id - 请求ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :
*****************************************************************************/
int PWRCTRL_DfsQosUpdate(int qos_id, int req_id, unsigned int req_value)
{
	unsigned long irqlock = 0;
	struct DFS_Qos_V7V9 *qos_list = NULL;
	
	bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MUDU_CPUFREQ,"qos_id:%d, req_value:%d, req_id:%d\n", qos_id, req_value, req_id);

	local_irq_save(irqlock);
	list_for_each_entry(qos_list, &(g_v9_qos_list.entry), entry)
	{
		if (req_id == qos_list->request_id)
		{
			qos_list->request_profile = cpufreq_v9r1_get_profile((int)qos_id, (int)req_value/1000);
			break;
		}
	}
    local_irq_restore(irqlock);

    cpufreq_qos_update_request(qos_id);
    return RET_OK;
}

/******************************for v9r1 adp end**********************************/

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


