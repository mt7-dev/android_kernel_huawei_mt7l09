
/*lint --e{537 } */
#include <intLib.h>
#include <logLib.h>
#include <vxWorks.h>
#include <msgQLib.h>
#include <product_config.h>
#include <osl_thread.h>
#include <bsp_hardtimer.h>
#include <bsp_icc.h>
#include <bsp_nvim.h>
#include <bsp_dpm.h>
#include <bsp_lowpower_mntn.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <drv_comm.h>
#include "cpufreq_balong.h"

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************/
/*读取NV*/
ST_PWC_DFS_STRU g_stDfsSwitch={0};

DFS_CONFIGINFO_STRU g_stDfsCpuConfigInfo;
DFS_CONTROL_STRU g_stDfsCpuControl;
/*调频模块开启声明该宏，用于和CPUIDLE解耦*/
#ifdef CONFIG_CPUFREQ
/*task idle time, unit:slice*/
unsigned int g_ulDfsCcpuIdleTime = 1;
unsigned int g_ulDfsCcpuIdleTime_long = 1;
unsigned int g_cpufreq_start_time = 0;
#else
extern unsigned int g_ulDfsCcpuIdleTime;
extern unsigned int g_ulDfsCcpuIdleTime_long;
#endif
/*当前profile*/
s32 g_cur_profile = BALONG_FREQ_MAX;
/*当前占用率*/
u32 g_ulCCpuload=1;
/*cpufreq 锁定标志 1:未锁，0: 锁定(锁定后不会主动调频)*/
int g_cpufreq_lock_status_flag = 0;
/*主动调频是否发送icc消息标志 1:可发送*/
static int g_icc_run_flag = 1;

static struct cpufreq_msg debug_msg = {0,0,0,0};

static PWRCTRLFUNCPTR FlowCtrlCallBack = NULL;

struct DFS_Qos_V7V9 g_v9_qos_list;

SEM_ID g_sem_calccpu_flag  = NULL;

unsigned int g_in_interr_times = 0;
unsigned int g_flowctrl_in_interr_times = 0;
unsigned int g_next_freq = 0;
unsigned int g_calccpu_load_result = DFS_PROFILE_NOCHANGE;
unsigned int g_test_in_interr_times = 20;

/*K3 DDR request qos id*/
int g_ddr_request_id = 0;
int g_last_ddr_value_id = 0;
/*******************************************************/
int balong_cpufreq_icc_send(struct cpufreq_msg *msg);


/*频率表*/
static struct cpufreq_frequency_table balong_clockrate_table[DC_RESV + 1];

#ifdef CPUFREQ_PLATFORM_HI6930  /* V7R2 ?*/
/*CPUFREQ_ENTRY_INVALID*/
struct cpufreq_query balong_query_profile_table[] = {
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO0, BALONG_CPUFREQUENCY_100, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO1, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_222, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO2, BALONG_CPUFREQUENCY_666, BALONG_DDRFREQUENCY_333, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO3, BALONG_CPUFREQUENCY_666, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO4, BALONG_CPUFREQUENCY_666, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
};
#elif defined(CPUFREQ_PLATFORM_HI6930_V711)
struct cpufreq_query balong_query_profile_table[] = {
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO0, BALONG_CPUFREQUENCY_100, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO1, BALONG_CPUFREQUENCY_200, BALONG_DDRFREQUENCY_400, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO2, BALONG_CPUFREQUENCY_400, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO3, BALONG_CPUFREQUENCY_400, BALONG_DDRFREQUENCY_400, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO4, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO5, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO6, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO7, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_600, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO8, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO9, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_600, BALONG_SBUSFREQUENCY_150, 0),
};
#else  /*3630*/
struct cpufreq_query balong_query_profile_table[] = {
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO0, BALONG_CPUFREQUENCY_200, 0, BALONG_SBUSFREQUENCY_200, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO1, BALONG_CPUFREQUENCY_260, 120, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO2, BALONG_CPUFREQUENCY_333, 240, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO3, BALONG_CPUFREQUENCY_370, 360, BALONG_SBUSFREQUENCY_200, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO4, BALONG_CPUFREQUENCY_400, 400, BALONG_SBUSFREQUENCY_133, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO5, BALONG_CPUFREQUENCY_430, 667, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO6, BALONG_CPUFREQUENCY_460, 800, BALONG_SBUSFREQUENCY_133, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO7, BALONG_CPUFREQUENCY_490, BALONG_DDRFREQUENCY_222, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO8, BALONG_CPUFREQUENCY_520, BALONG_DDRFREQUENCY_333, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO9, BALONG_CPUFREQUENCY_550, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_133, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO10, BALONG_CPUFREQUENCY_580, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO11, BALONG_CPUFREQUENCY_610, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO12, BALONG_CPUFREQUENCY_640, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_133, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO13, BALONG_CPUFREQUENCY_666, BALONG_DDRFREQUENCY_222, BALONG_SBUSFREQUENCY_166, 0),
	//QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO14, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_333, BALONG_SBUSFREQUENCY_166, 0),
	//QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO15, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
	//QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO16, BALONG_CPUFREQUENCY_400, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
};
#endif

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCcpuLoadCB
 功能描述  : arm 提供给TTF的回调函数,该接口中回调不可再中断中使用
 输入参数  : pFunc:TTF函数指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
 void BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc )
 {
	 if (NULL == pFunc)
    {
        cpufreq_err("BSP_PWRCTRL_FlowCtrlCallBackRegister param is Null,pls check\n");
        return;
    }
    else
    {
        FlowCtrlCallBack = pFunc;
    }
 }

static void balong_check_profile_limit(int *max_limit, int *min_limit)
{
	if (CPUFREQ_MAX_PROFILE_LIMIT < CPUFREQ_MIN_PROFILE_LIMIT)
	{
		*max_limit = BALONG_FREQ_MAX;
		*min_limit = BALONG_FREQ_MIN;
		return;
	}
	if ((CPUFREQ_MAX_PROFILE_LIMIT > BALONG_FREQ_MAX) || ((s32)CPUFREQ_MAX_PROFILE_LIMIT < BALONG_FREQ_MIN))
	{
		*max_limit = BALONG_FREQ_MAX;
	}
	else
	{
		*max_limit = (s32)CPUFREQ_MAX_PROFILE_LIMIT;
	}
	/* coverity[unsigned_compare] */
	if ((CPUFREQ_MIN_PROFILE_LIMIT > BALONG_FREQ_MAX) || ((s32)CPUFREQ_MIN_PROFILE_LIMIT < BALONG_FREQ_MIN))
	{
		*min_limit = BALONG_FREQ_MIN;
	}
	else
	{
		*min_limit = (s32)CPUFREQ_MIN_PROFILE_LIMIT;
	}
}

/*检查调频请求是否符合限制*/
static int balong_check_msg(struct cpufreq_msg *msg)
{
	int ret = BSP_OK;
	int max_limit = 0;
	int min_limit = 0;
	balong_check_profile_limit(&max_limit, &min_limit);
	int cur_profile = pwrctrl_dfs_get_profile();
	if (CPUFREQ_ADJUST_FREQ == msg->msg_type)
	{
		switch(msg->content)
		{
			case DFS_PROFILE_UP:
			case DFS_PROFILE_UP_TARGET:
				if (max_limit == cur_profile)
				{
					ret = BSP_ERROR;
				}
				if (msg->profile > (u32)max_limit)
				{
					msg->profile = (u32)max_limit;
				}
				break;
			case DFS_PROFILE_DOWN:
			case DFS_PROFILE_DOWN_TARGET:
				if (min_limit == cur_profile)
				{
					ret = BSP_ERROR;
				}
				if (msg->profile < (u32)min_limit)
				{
					msg->profile = (u32)min_limit;
				}
#ifdef CPUFREQ_IS_SYNC_DDR
			/*判断本次请求是否和上次重复，重复则停止再次发送*/
			if ((1 == CPUFREQ_DOWN_FLAG(msg->source)) 
					&& ((msg->profile) == CPUFREQ_DOWN_PROFILE(msg->source)))
			{
				ret = BSP_ERROR;
			}
#endif
				break;
			case DFS_PROFILE_DOWN_LIMIT:
				if ((u32)min_limit == msg->profile)
				{
					ret =BSP_ERROR;
				}
				break;
			case DFS_PROFILE_UP_LIMIT:
				if ((u32)max_limit == msg->profile)
				{
					ret =BSP_ERROR;
				}
				break;
			default:
				break;
		}
	}/*CPUFREQ_ADJUST_FREQ == msg->msg_type*/
	return ret;
}
/*
 * 发送消息 BSP_ERROR 发送失败;BSP_OK 发送成功
 */
int balong_cpufreq_icc_send(struct cpufreq_msg *msg)
{
	u32 channel_id = ICC_CHN_MCORE_CCORE << 16 | MCU_CCORE_CPUFREQ;
	s32 ret = 0;
	u32 time_value = 0;
	u32 msg_len = sizeof(struct cpufreq_msg);
	if (!g_cpufreq_lock_status_flag)
	{
		return BSP_ERROR;
	}
	ret = balong_check_msg(msg);
	if (BSP_ERROR == ret)
	{
		bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MUDU_CPUFREQ,"msg is: msg_type:%d source:%d content:%d profile:%d\n",
																								msg->msg_type, msg->source, msg->content, msg->profile);
		return BSP_OK;
	}
	if(g_lowpower_shared_addr)
	{
		time_value=  bsp_get_slice_value();
		writel(time_value, g_lowpower_shared_addr + 0x210);
	}
	debug_msg.msg_type = msg->msg_type;
	debug_msg.source = msg->source;
	debug_msg.content = msg->content;
	debug_msg.profile = msg->profile;
	
	ret = bsp_icc_send(ICC_CPU_MCU, channel_id, (u8 *)msg, msg_len);

	if((ret < 0) && (ret != BSP_ERR_ICC_CCORE_RESETTING))
	{
		cpufreq_err("mcore return an ERROR please check m3 %d\n", ret);
		return BSP_ERROR;
	}

    return BSP_OK;
}
static s32 balong_cpufreq_cb_getprofile(u32 channel_id , u32 len, void* context)
{
	s32 ret = 0;
	struct cpufreq_msg cm = {0};
	ret = bsp_icc_read(channel_id, (u8*)&cm, len);

	if(len != (u32)ret)
	{
		cpufreq_err("balong_cpufreq_cb_getload error \r\n");
		return -1;
	}
	g_stDfsCpuControl.enCurProfile = cm.profile;
	cpufreq_info("get icc from Mcore msg_type->>%d, source->>%d, profile->>%d\n", cm.msg_type, cm.source, cm.profile);
	return 0;
}
static void register_icc_for_cpufreq(void)
{
	s32 ret;
	u32 channel_id_set = ICC_CHN_MCORE_CCORE << 16 | MCU_CCORE_CPUFREQ;
	ret = bsp_icc_event_register(channel_id_set, (read_cb_func)balong_cpufreq_cb_getprofile, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
	if (ret != BSP_OK)
	{
		cpufreq_err("icc register failed %d\n", ret);
	}
	cpufreq_info("register icc %d\n", ret);
}

/*
	调频策略
*/
static inline u32  cpufreq_calccpu_result_insysint(u32 *next_freq)
{
    u32 ulSleepTime;
    u32 ulCpuLoad_C;
    u32 ulProTime;
    u32 ulEndTime;

    /*这里是读取时间,暂时大桩*/
	ulEndTime = bsp_get_slice_value();
  	ulProTime = get_timer_slice_delta(g_stDfsCpuControl.ulStartTime, ulEndTime);
	ulSleepTime = g_ulDfsCcpuIdleTime;
	g_ulDfsCcpuIdleTime = 0;
	g_stDfsCpuControl.ulStartTime = ulEndTime;
	ulCpuLoad_C= (ulProTime == 0) ? (0) : (((ulProTime -ulSleepTime)* 100) / (ulProTime));    /*Calc the Cpu load Value*/
	if (ulCpuLoad_C > 100)
	{
		ulCpuLoad_C = 100;
	}
	g_ulCCpuload = ulCpuLoad_C;
	if(g_lowpower_shared_addr)
	{
		writel(g_ulCCpuload, g_lowpower_shared_addr + 0x20c);
	}
    if ((ulCpuLoad_C > g_stDfsCpuConfigInfo.astThresHold[0].usProfileUpLimit) || (ulCpuLoad_C >= g_stDfsSwitch.DFSDdrUpLimit))
    {
        return DFS_PROFILE_UP_TARGET;
	}
    else if ((ulCpuLoad_C < g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit) || (ulCpuLoad_C <= g_stDfsSwitch.DFSDdrDownLimit))
    {
        return DFS_PROFILE_DOWN;
    }
    else /*The System Load is Normal Value*/
    {
        return DFS_PROFILE_NOCHANGE;
    }
}

/*
	调频策略
*/
static u32  cpufreq_calccpu_result(u32 *next_freq)
{
    u32 ulSleepTime;
    u32 ulCpuFree;
    u32 ulCpuLoad_C;
    u32 ulProTime;
    u32 ulEndTime;
    int cur_profile = 0;
    PWRCTRLFUNCPTR pRoutine = NULL;

    /*这里是读取时间,暂时大桩*/
	ulEndTime = bsp_get_slice_value();
  	ulProTime = ulEndTime - g_stDfsCpuControl.ulStartTime;
	ulSleepTime = g_ulDfsCcpuIdleTime;
	g_ulDfsCcpuIdleTime = 0;
	g_stDfsCpuControl.ulStartTime = ulEndTime;
	ulCpuFree= (ulSleepTime* 100) / (ulProTime);    /*Calc the Cpu Free Value*/

	if (ulCpuFree > 100)
    {
		cpufreq_err("calc cpuload error!\n");
        return DFS_PROFILE_NOCHANGE;
    }
	ulCpuLoad_C= 100 - ulCpuFree;
	g_ulCCpuload = ulCpuLoad_C;
	 /*调用ttf回调函数*/
    if (NULL != FlowCtrlCallBack)
    {
        pRoutine = FlowCtrlCallBack;
        (void)(*pRoutine)(g_ulCCpuload);
    }
    else
    {
	}
    if (ulCpuLoad_C > g_stDfsCpuConfigInfo.astThresHold[0].usProfileUpLimit)
    {
        /*Clean the Value of the System Down Counter*/
        g_stDfsCpuControl.ulCurSysDownTime = 0;
		g_stDfsCpuControl.ulCurSysUpTime++;
	}
    else if (ulCpuLoad_C < g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit)
    {
        /*Clean the Value of the System Over Load Counter*/
		g_stDfsCpuControl.ulCurSysUpTime = 0;
		g_stDfsCpuControl.ulCurSysDownTime++;
    }
    else /*The System Load is Normal Value*/
    {
		g_stDfsCpuControl.ulCurSysDownTime = 0;
        g_stDfsCpuControl.ulCurSysUpTime = 0;

        return DFS_PROFILE_NOCHANGE;
    }
	cur_profile = pwrctrl_dfs_get_profile();
    if (g_stDfsCpuControl.ulCurSysDownTime >= g_stDfsCpuConfigInfo.usProfileDownTime)
    {
		g_stDfsCpuControl.ulCurSysDownTime = 0;
        g_stDfsCpuControl.ulCurSysUpTime = 0;

        *next_freq =( ulCpuLoad_C * (balong_query_profile_table[cur_profile].cpu_frequency))
        							/ (g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit);
        return DFS_PROFILE_DOWN;
    }

    if (g_stDfsCpuControl.ulCurSysUpTime >= g_stDfsCpuConfigInfo.usProfileUpTime)
    {
		 g_stDfsCpuControl.ulCurSysDownTime = 0;
        g_stDfsCpuControl.ulCurSysUpTime = 0;
        *next_freq = balong_query_profile_table[BALONG_FREQ_MAX].cpu_frequency;/*max cpufreq*/
        return DFS_PROFILE_UP_TARGET;
    }

	return DFS_PROFILE_NOCHANGE;

}


/*find the first profile that is the target*/
static void cpufreq_frequency_target_profile( unsigned int target_freq,
				   unsigned int relation,  unsigned int *new_profile)
{
	int i = 0;
	int cur_profile = pwrctrl_dfs_get_profile();
	switch (relation)
	{
		case DFS_PROFILE_UP:
			*new_profile = balong_clockrate_table[BALONG_FREQ_MAX].index;
			return;
		case DFS_PROFILE_DOWN:
			cpufreq_debug("down to frequency\n");
			break;
		default :
			break;
	}

	for (i = cur_profile; i >= BALONG_FREQ_MIN; i--)
	{
		if (target_freq != balong_query_profile_table[i].cpu_frequency)
		{
			continue;
		}
		if ((u32)cur_profile == balong_query_profile_table[i].profile)
		{
			continue;
		}
		*new_profile = balong_query_profile_table[i].profile;
		goto out;
	}
	if (cur_profile != BALONG_FREQ_MIN)
	{
		*new_profile = balong_query_profile_table[cur_profile - 1].profile;
	}
	else
	{
		*new_profile = balong_query_profile_table[BALONG_FREQ_MIN].profile;
	}
out:
	return;
}
/*find a freq in all table*/
static int cpufreq_frequency_table_target(struct cpufreq_frequency_table *table,
				   unsigned int target_freq,
				   unsigned int relation,
				   unsigned int *index)
{/*lint !e578 */
	struct cpufreq_frequency_table optimal = {
		.index = ~0,
		.frequency = 0,
	};
	struct cpufreq_frequency_table suboptimal = {
		.index = ~0,
		.frequency = 0,
	};
	unsigned int i;
	/*lint --e{744} */
	switch (relation) {
	case DFS_PROFILE_UP:
		suboptimal.frequency = ~0;
		break;
	case DFS_PROFILE_DOWN:
		optimal.frequency = ~0;
		break;
	}

	for (i = 0; (table[i].frequency != (u32)CPUFREQ_TABLE_END); i++) {
		unsigned int freq = table[i].frequency;
		if (freq == (u32)CPUFREQ_ENTRY_INVALID)
			continue;
		if ((freq < balong_query_profile_table[BALONG_FREQ_MIN].cpu_frequency) || (freq > balong_query_profile_table[BALONG_FREQ_MAX].cpu_frequency))
			continue;
		switch (relation) {
		case DFS_PROFILE_UP:
			if (freq <= target_freq) {
				if (freq >= optimal.frequency) {
					optimal.frequency = freq;
					optimal.index = i;
				}
			} else {
				if (freq <= suboptimal.frequency) {
					suboptimal.frequency = freq;
					suboptimal.index = i;
				}
			}
			break;
		case DFS_PROFILE_DOWN:
			if (freq >= target_freq) {
				if (freq <= optimal.frequency) {
					optimal.frequency = freq;
					optimal.index = i;
				}
			} else {
				if (freq >= suboptimal.frequency) {
					suboptimal.frequency = freq;
					suboptimal.index = i;
				}
			}
			break;
		}
	}
	if (optimal.index > i) {
		if (suboptimal.index > i)
			return BSP_ERROR;
		*index = suboptimal.index;
	} else
		*index = optimal.index;

	return BSP_OK;
}
/*****************************************************************************
Function:   PWRCTRL_DfsMgrExcuteVoteResultCpu
Description:Handle the Profile Vote Result
Input:      enResult:   The Vote Value
Output:     None
Return:     None
Others:
*****************************************************************************/
static int  cpufreq_excute_result_cpu(u32 relation, u32 target_freq)
{
	u32 result = 2;
	u32 new_index = 0;
	int cur_profile = 0;
	struct cpufreq_msg task_msg = {0,0,0,0};

	cpufreq_frequency_table_target(balong_clockrate_table,
					   target_freq, relation, &new_index);

	cpufreq_debug("target_freq %d new_index%d\n", target_freq, new_index);

	cur_profile = pwrctrl_dfs_get_profile();
	if ((DFS_PROFILE_UP == relation) && (BALONG_FREQ_MAX != cur_profile))
	{
		result = DFS_PROFILE_UP_TARGET;
	}
	else if ((DFS_PROFILE_DOWN == relation) && (BALONG_FREQ_MIN != cur_profile))
	{
		result = DFS_PROFILE_DOWN_TARGET;
	}
	else
	{
		cpufreq_err("set target relation %d, cur pro %d\n", relation, cur_profile);
		return BSP_ERROR;
	}

	cpufreq_frequency_target_profile(balong_clockrate_table[new_index].frequency, relation, &new_index);

	task_msg.msg_type = CPUFREQ_ADJUST_FREQ;
	task_msg.source = CPUFREQ_CCORE;
	task_msg.content = result;
	task_msg.profile = new_index;
	balong_cpufreq_icc_send(&task_msg);
	g_stDfsCpuControl.enCurProfile = (u32)cur_profile;
	return BSP_OK;

}

unsigned int cpufreq_calccpu_cpuload(void)
{
	u32 end_time = 0;
	u32 idle_time = 0;
	u32 wall_time = 0;
	u32 cpu_load = 0;
	unsigned long irqlock = 0;
	local_irq_save(irqlock);
	end_time = bsp_get_slice_value();
	wall_time = get_timer_slice_delta(g_cpufreq_start_time, end_time);
	idle_time = g_ulDfsCcpuIdleTime_long;
	g_cpufreq_start_time = end_time;
	g_ulDfsCcpuIdleTime_long = 0;
	g_flowctrl_in_interr_times = 0;
	cpu_load = (wall_time == 0) ? (0) : (((wall_time - idle_time) * 100) / wall_time);
	local_irq_restore(irqlock);
	if (cpu_load > 100)
	{
		cpu_load = g_ulCCpuload;
		cpufreq_info("cpuload: %d, wall:%d, idle:%d\n", cpu_load, wall_time, idle_time);
	}
	return cpu_load;
}
/*
 * 该接口负责cpu负载检测，
 * 并根据预设阈值判决是否需要向M3请求调频
 */
void cpufreq_update_frequency(void)
{
	u32 cpuload = 0;
	int cur_profile = 0;
	struct cpufreq_msg task_msg = {CPUFREQ_ADJUST_FREQ, CPUFREQ_CCORE, 0, BALONG_FREQ_MAX};
	cpuload = cpufreq_calccpu_cpuload();
	cur_profile = pwrctrl_dfs_get_profile();
	if (cpuload > g_stDfsCpuConfigInfo.astThresHold[0].usProfileUpLimit)
   {
		task_msg.content = DFS_PROFILE_UP_TARGET;
	}
    else if (cpuload < g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit)
    {
		task_msg.profile = (cur_profile != BALONG_FREQ_MIN) ? (cur_profile - 1) : (BALONG_FREQ_MIN);
    	task_msg.content = DFS_PROFILE_DOWN;
    }
    else
    {
		return;
    }
    balong_cpufreq_icc_send(&task_msg);
}
void cpufreq_checkload_in_sysint(void)
{
	if (!g_cpufreq_lock_status_flag)
	{
		return;
	}
	if (NULL != g_sem_calccpu_flag)
	{
		if (g_in_interr_times >= g_test_in_interr_times)
		{
			g_calccpu_load_result = cpufreq_calccpu_result_insysint(&g_next_freq);

			g_in_interr_times = 0;
			if (DFS_PROFILE_NOCHANGE != g_calccpu_load_result)
			{
				semGive(g_sem_calccpu_flag);
			}
		}
		else
		{
			g_in_interr_times++;
		}
		/*2s释放信号量，回调流控接口*/
		if (g_flowctrl_in_interr_times == 200)
		{
			semGive(g_sem_calccpu_flag);
		}
		g_flowctrl_in_interr_times++;
	}
}
/*dfs 辅助DDR 调频接口V7不需要*/
void cpufreq_assistant_regulate_ddr(int cur_profile)
{
#ifndef CPUFREQ_IS_SYNC_DDR  /* 异步调频*/
	unsigned int ddr_value = 0;
	int ddr_value_id = 0;
	unsigned long irqlock = 0;
	ddr_value_id = g_last_ddr_value_id;
	if ((ddr_value_id > BALONG_DDRFREQUENCY_MAX) || (ddr_value_id < BALONG_DDRFREQUENCY_MIN))
	{
		ddr_value_id = (int)g_stDfsSwitch.DFSDdrprofile;
	}
	/*bit2表示是否打开ddr辅助调频*/
	if ((g_stDfsSwitch.Strategy) & (0x1<<1))
	{
		/*直接将DDR调制最高或降一档*/
		if ((cur_profile >= BALONG_FREQ_MAX) && (g_ulCCpuload >= g_stDfsSwitch.DFSDdrUpLimit)
		&& (ddr_value_id < (int)g_stDfsSwitch.DFSDdrprofile))
		{
			ddr_value = balong_query_profile_table[g_stDfsSwitch.DFSDdrprofile].ddr_frequency;
			(void)PWRCTRL_DfsQosUpdate(DFS_QOS_ID_DDR_MINFREQ, g_ddr_request_id, ddr_value * 1000);
			local_irq_save(irqlock);
			g_last_ddr_value_id = (int)g_stDfsSwitch.DFSDdrprofile;
			g_calccpu_load_result = DFS_PROFILE_NOCHANGE;
			local_irq_restore(irqlock);
		}
		else if ((g_ulCCpuload <= g_stDfsSwitch.DFSDdrDownLimit) && (ddr_value_id > BALONG_DDRFREQUENCY_MIN))
		{
			ddr_value_id = ((ddr_value_id -1) > BALONG_DDRFREQUENCY_MIN) ? (ddr_value_id -1) : BALONG_DDRFREQUENCY_MIN;
			ddr_value = balong_query_profile_table[ddr_value_id].ddr_frequency;
			(void)PWRCTRL_DfsQosUpdate(DFS_QOS_ID_DDR_MINFREQ, g_ddr_request_id, ddr_value * 1000);
			local_irq_save(irqlock);
			g_last_ddr_value_id = ddr_value_id;
			g_calccpu_load_result = DFS_PROFILE_NOCHANGE;
			local_irq_restore(irqlock);
		}
	}
#endif
}
static void  pwrctrl_dfs_mgrmsg_task(void)
{
    int cur_profile = 0;
    unsigned int flowctrl_cpuload = 0;
    PWRCTRLFUNCPTR pRoutine = NULL;
    struct cpufreq_msg task_msg = {0,0,0,0};
	g_stDfsCpuControl.ulStartTime = bsp_get_slice_value();
	/* coverity[INFINITE_LOOP] */
	/* coverity[no_escape] */
    for (;;)
    {
		if (NULL != g_sem_calccpu_flag)
		{	
			semTake(g_sem_calccpu_flag, DFS_WAIT_FOREVER);
			 /*调用ttf回调函数*/
		    if ((NULL != FlowCtrlCallBack) && (g_flowctrl_in_interr_times >= 200))
		    {
		    	 flowctrl_cpuload = cpufreq_calccpu_cpuload();
		        pRoutine = FlowCtrlCallBack;
		        (void)(*pRoutine)(flowctrl_cpuload);
		    }
		}
		else
		{
			taskDelay((int)g_stDfsCpuConfigInfo.ulTimerLen);
			g_calccpu_load_result = cpufreq_calccpu_result(&g_next_freq);
		}
		if (!g_cpufreq_lock_status_flag)
		{
			continue;
		}
		
		cur_profile = pwrctrl_dfs_get_profile();
		cpufreq_assistant_regulate_ddr(cur_profile);
		if (DFS_PROFILE_NOCHANGE != g_calccpu_load_result)
		{
			if (g_icc_run_flag == 1)
			{
				task_msg.msg_type = CPUFREQ_ADJUST_FREQ;
				task_msg.source = CPUFREQ_CCORE;
				task_msg.content = g_calccpu_load_result;
				if (DFS_PROFILE_UP_TARGET == g_calccpu_load_result)
				{
					cur_profile = DC_RESV;
				}
				else if ((u32)cur_profile == CPUFREQ_MIN_PROFILE_LIMIT)
				{
					continue;
				}
				task_msg.profile = (unsigned int)cur_profile - 1;
				balong_cpufreq_icc_send(&task_msg);
			}
			else if (g_icc_run_flag == 2)
			{
				cpufreq_excute_result_cpu(g_calccpu_load_result, g_next_freq);
			}
        }

    }
}




static void cpufreq_table_init(void)
{
	int i = 0;
	for (i = BALONG_FREQ_MIN; i < DC_RESV; i++)
	{
		balong_clockrate_table[i].index = balong_query_profile_table[i].profile;
		balong_clockrate_table[i].frequency = balong_query_profile_table[i].cpu_frequency;
	}
	balong_clockrate_table[DC_RESV].index = DC_RESV;
	balong_clockrate_table[DC_RESV].frequency = CPUFREQ_TABLE_END;
}

static int balong_cpufreq_resume(struct dpm_device *cpufreq_resume)
{
	g_stDfsCpuControl.ulCurSysDownTime = 0;
    g_stDfsCpuControl.ulCurSysUpTime = 0;
    g_in_interr_times = 0;
    return BSP_OK;
}
#ifdef CONFIG_CCORE_PM
static struct dpm_device g_cpufreq_dpm_device={
    .device_name = "cpufreq_dpm",
    .resume = balong_cpufreq_resume,
};
#endif
/*****************************************************************************
Function:   cpufreq_init
Description:DFS Module Initialiation Process
Input:      None
Output:     None
Return:     None
Others:
*****************************************************************************/

void  cpufreq_init(void)
{
	/*lint --e{516}*/
	u32 i = 0;
	u32 retValue = 0;
	ST_PWC_SWITCH_STRU cpufreq_control_nv = {0} ;
    retValue = bsp_nvm_read(NV_ID_DRV_NV_DFS_SWITCH,(u8*)&g_stDfsSwitch,sizeof(ST_PWC_DFS_STRU));
    if (NV_OK != retValue)
    {
    	cpufreq_err("read nv failed use default value\n");
		g_stDfsSwitch.CcpuDownLimit = 60;
		g_stDfsSwitch.CcpuDownNum = 3;
		g_stDfsSwitch.CcpuUpLimit = 85;
		g_stDfsSwitch.CcpuUpNum = 1;
		g_stDfsSwitch.DFSTimerLen = 400;
		g_stDfsSwitch.Strategy = 0;/*使用4s检测一次的策略*/
		g_stDfsSwitch.DFSDdrUpLimit = 85;
		g_stDfsSwitch.DFSDdrDownLimit = 60;
		g_stDfsSwitch.DFSDdrprofile = 5;
		g_stDfsSwitch.reserved = 0;
    }
	retValue = bsp_nvm_read(NV_ID_DRV_NV_PWC_SWITCH,(u8*)&cpufreq_control_nv,sizeof(ST_PWC_SWITCH_STRU));
	if (NV_OK == retValue)
	{
		g_cpufreq_lock_status_flag = cpufreq_control_nv.dfs;
	}
	else
	{
		cpufreq_err("read nv failed %d\n", retValue);
	}

    memset(&g_stDfsCpuConfigInfo, 0, sizeof(g_stDfsCpuConfigInfo));
    memset(&g_stDfsCpuControl, 0, sizeof(g_stDfsCpuControl));
	g_stDfsCpuControl.enCurProfile = BALONG_FREQ_MAX;

	register_icc_for_cpufreq();
	cpufreq_table_init();
#ifdef CONFIG_CCORE_PM
    if (bsp_device_pm_add(&g_cpufreq_dpm_device))
    {
		cpufreq_err("register dpm failed,check it\n");
    }
#endif
    /*bit1 使用MS级调频策略*/
    if (g_stDfsSwitch.Strategy & 0x1)
    {
		  g_sem_calccpu_flag = semBCreate(SEM_Q_PRIORITY, SEM_FULL); /*lint !e64 */
		  if (NULL == g_sem_calccpu_flag)
		  {
		      cpufreq_err("Create g_sem_k3get_volt Failed %d\n", g_sem_calccpu_flag);
		  }
	}

    g_stDfsCpuConfigInfo.ulDFSFunEnableFlag = DFS_TRUE;
    g_stDfsCpuConfigInfo.usProfileDownTime = g_stDfsSwitch.CcpuDownNum;
    g_stDfsCpuConfigInfo.usProfileUpTime = g_stDfsSwitch.CcpuUpNum;
    g_stDfsCpuConfigInfo.ulProfileNum = DC_RESV;
    g_stDfsCpuConfigInfo.ulTimerLen = g_stDfsSwitch.DFSTimerLen;
    g_test_in_interr_times = g_stDfsSwitch.DFSTimerLen;

    for (i = 0; i < g_stDfsCpuConfigInfo.ulProfileNum; i++)
    {
        g_stDfsCpuConfigInfo.astThresHold[i].usProfileUpLimit   = g_stDfsSwitch.CcpuUpLimit;
        g_stDfsCpuConfigInfo.astThresHold[i].usProfileDownLimit = g_stDfsSwitch.CcpuDownLimit;
    }
    INIT_LIST_HEAD(&(g_v9_qos_list.entry));
    /*添加DDR调频请求*/
    (void)PWRCTRL_DfsQosRequest(DFS_QOS_ID_DDR_MINFREQ, BALONG_DDRFREQUENCY_MIN, &g_ddr_request_id);
    
    taskSpawn("dfs_task", 1, 0, 4096, (FUNCPTR)pwrctrl_dfs_mgrmsg_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);/*lint !e64 !e119 */
	g_cpufreq_start_time = bsp_get_slice_value();
	cpufreq_err("cpufreq init ok\n");
    return;
}
/******************************test for cpufreq start***********************************/
/*设置调频阈值 不用配置的项输入负值*/
void cpufreq_exc_change_limit(u32 sample, u32 up_cpuloadlimit, u32 down_cpuloadlimit, u32 down_times, u32 up_times)
{
	unsigned long flags = 0;
	local_irq_save(flags);
	if (up_cpuloadlimit <= down_cpuloadlimit)
	{
		cpufreq_err("ERROE: up_cpuloadlimit <= down_cpuloadlimit")	;
		goto out;
	}
	g_stDfsCpuConfigInfo.ulTimerLen = sample;
	
	g_test_in_interr_times = sample;

	g_stDfsCpuConfigInfo.astThresHold[0].usProfileUpLimit = up_cpuloadlimit;

	g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit = down_cpuloadlimit;

	g_stDfsCpuConfigInfo.usProfileDownTime = down_times;

	g_stDfsCpuConfigInfo.usProfileUpTime = up_times;

out:
	local_irq_restore(flags);
}


/*
 * g_dfs_flag控制是否计算占用
 * g_icc_flag_run控制是否调用icc通知CCORE
 */
void calccpu_flag(int flag, int flag1)
{
	g_cpufreq_lock_status_flag = flag;
 	g_icc_run_flag = flag1;
}
/*1 tick 3000000*/
unsigned int busy_time = 10;
unsigned int idle_time = 1;
unsigned long start_time = 0;
#define TIMER_COUNT_TICK_TO_MS	32768
void test_for_adjust_cpuload(int busytime, int idletime)
{
	if (busytime)
	busy_time = (u32)busytime;
	if (idletime)
	idle_time = (u32)idletime;
}
void test_for_pmuevent(void)
{/*lint --e{716 } */
	while (1)
	{
		start_time = bsp_get_elapse_ms();
		while (bsp_get_elapse_ms()-start_time <= busy_time)
		{
			;
		}
		taskDelay((int)idle_time);
		if (!idle_time)
		{
			break;
		}
	}
}

void test_for_cpufreq_longtime(void)
{/*lint --e{716 } */
	int which_load_bound = 0;
	/* coverity[INFINITE_LOOP] */
	/* coverity[no_escape] */
	while(1)
	{
		/*每30s改变cpu占用，测试长时间调频是否正常*/
		taskDelay(4000 * (which_load_bound + 1));
		switch(which_load_bound)
		{
			case 0:
				test_for_adjust_cpuload(3500, 500);//90
				break;
			case 1:
				test_for_adjust_cpuload(2000,200);//50
				break;
			case 2:
				test_for_adjust_cpuload(1000,300);//8-11
				break;
			default:
				break;
		}
		which_load_bound = (which_load_bound + 1) % 3;
	}
}

void test_for_pmu_task(void)
{
	taskSpawn("testPmuEvent", 2, 0, 4096, (FUNCPTR)test_for_pmuevent, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); /*lint !e64 !e119 */
	//if (!idle_time)
	//taskSpawn("testPmuEvent", 2, 0, 4096, (FUNCPTR)test_for_cpufreq_longtime, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); /*lint !e64 !e119 */
}

void cpufreq_print_debug(void)
{
	pwrctrl_dfs_get_profile();
	cpufreq_err("cur\t profile: %d\n", g_cur_profile);
	cpufreq_err("ccore\t load: %d\n", g_ulCCpuload);
	cpufreq_err("lase\t ddr: %d\n", g_last_ddr_value_id);
	cpufreq_err("up      times: %d\n", g_stDfsCpuControl.ulCurSysUpTime);
	cpufreq_err("down  times: %d\n", g_stDfsCpuControl.ulCurSysDownTime);
	cpufreq_err("up_threshold: %d\n", g_stDfsCpuConfigInfo.astThresHold[0].usProfileUpLimit);
	cpufreq_err("up_threshold_times: %d\n",  g_stDfsCpuConfigInfo.usProfileUpTime);
	cpufreq_err("down_threshold: %d\n", g_stDfsCpuConfigInfo.astThresHold[0].usProfileDownLimit);
	cpufreq_err("down_threshold_times: %d\n",  g_stDfsCpuConfigInfo.usProfileDownTime);
	cpufreq_err("sampling_rate: %d\n", g_stDfsCpuConfigInfo.ulTimerLen);
	cpufreq_err("last icc msg\n");
	cpufreq_err("icc msg_type: %d\n", debug_msg.msg_type);
	cpufreq_err("icc source: %d\n", debug_msg.source);
	cpufreq_err("icc content: %d\n", debug_msg.content);
	cpufreq_err("icc profile: %d\n", debug_msg.profile);
	cpufreq_err("cur max limit:%d\n", CPUFREQ_MAX_PROFILE_LIMIT);
	cpufreq_err("cur min limit:%d\n", CPUFREQ_MIN_PROFILE_LIMIT);
}

void test_send_msg(unsigned int msg_type, unsigned int source, unsigned int content, unsigned int profile)
{
	struct cpufreq_msg task_msg = {0,0,0,0};
	task_msg.msg_type = msg_type;
	task_msg.source = source;
	task_msg.content = content;
	task_msg.profile = profile;
	balong_cpufreq_icc_send(&task_msg);
}


void test_for_v9r1_interface(int inter_id, int request_id, int request_freq, int req_id)
{
	int req_request_id = 0;
	switch(inter_id)
	{
		case 0://request
			if (PWRCTRL_DfsQosRequest(request_id, (u32)request_freq, &req_request_id))
			{
				cpufreq_err("QosRequest error\n");
			}
			break;
		case 1://release
			if (PWRCTRL_DfsQosRelease(request_id, &req_request_id))
			{
				cpufreq_err("QosRelease error\n");
			}
			break;
		case 2: //update
			if (PWRCTRL_DfsQosUpdate(request_id, req_id, (u32)request_freq))
			{
				cpufreq_err("QosUpdate error\n");
			}
			break;
		default:
			break;
	}
}
/******************************test for cpufreq end***********************************/
#ifdef __cplusplus
}
#endif
