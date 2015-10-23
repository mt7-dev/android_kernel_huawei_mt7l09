
/*lint --e{537 } */
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/sched.h>	/* set_cpus_allowed() */
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <osl_types.h>
#include <osl_sem.h>
#include <bsp_icc.h>
#include <cpufreq_balong.h>
#include <drv_cpufreq.h>

#ifdef __cplusplus
extern "C"
{
#endif

u32 g_cur_freq = BALONG_CPUFREQUENCY_666;
s32 g_cur_profile = BALONG_FREQ_MAX;
/*cpufreq 锁定标志 1:未锁，0: 锁定(锁定后不会主动调频)*/
int g_cpufreq_lock_status_flag = 0;
/*for debug*/
struct cpufreq_msg debug_msg = {0,0,0,0};

/*频率表*/
struct cpufreq_frequency_table balong_clockrate_table[DC_RESV + 1];

#ifdef CPUFREQ_PLATFORM_HI6930  /* V7R2 ?*/
/*CPUFREQ_ENTRY_INVALID*/
struct cpufreq_query balong_query_profile_table[] = {
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO0, BALONG_CPUFREQUENCY_100, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO1, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_222, BALONG_SBUSFREQUENCY_111, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO2, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_333, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO3, BALONG_CPUFREQUENCY_333, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO4, BALONG_CPUFREQUENCY_666, BALONG_DDRFREQUENCY_666, BALONG_SBUSFREQUENCY_166, 0),
};

#elif defined(CPUFREQ_PLATFORM_HI6930_V711)
struct cpufreq_query balong_query_profile_table[] = {
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO0, BALONG_CPUFREQUENCY_100, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO1, BALONG_CPUFREQUENCY_200, BALONG_DDRFREQUENCY_400, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO2, BALONG_CPUFREQUENCY_200, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO3, BALONG_CPUFREQUENCY_400, BALONG_DDRFREQUENCY_400, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO4, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_200, BALONG_SBUSFREQUENCY_100, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO5, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO6, BALONG_CPUFREQUENCY_300, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO7, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_600, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO8, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_300, BALONG_SBUSFREQUENCY_150, 0),
	QUERY_PROFILE_TABLE_INIT(BALONG_FREQ_PRO9, BALONG_CPUFREQUENCY_600, BALONG_DDRFREQUENCY_600, BALONG_SBUSFREQUENCY_150, 0),
};
#endif

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

int balong_cpufreq_icc_send(struct cpufreq_msg *msg);
/********************************************************************/
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
	if ((profile < BALONG_FREQ_MIN) || (profile > BALONG_FREQ_MAX))
	{
		cpufreq_err("profile in right bound??%d\n", profile);
		return BSP_ERROR;
	}
	set_msg.msg_type = CPUFREQ_ADJUST_FREQ;
	set_msg.source = CPUFREQ_ACORE;
	if (pwrctrl_dfs_get_profile() < profile)
	{
		set_msg.content = DFS_PROFILE_UP_TARGET;
	}
	else if (pwrctrl_dfs_get_profile() > profile)
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
	set_msg.source = CPUFREQ_ACORE;
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
	set_msg.source = CPUFREQ_ACORE;
	balong_cpufreq_icc_send(&set_msg);
}
/*
 * 调试接口，设置频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 *
 */
int pwrctrl_dfs_target(int a9freq, int ddrfreq, int slowfreq)
{
	int target_profile = 0;
	target_profile = pwrctrl_find_min_profile((u32)a9freq, (u32)ddrfreq, (u32)slowfreq);
	cpufreq_debug("prolfie : %d\n", target_profile);
	pwrctrl_dfs_set_baseprofile(target_profile);
	return target_profile;
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
		*a9freq = (s32)balong_query_profile_table[cur_profile].cpu_frequency;
		*ddrfreq = (s32)balong_query_profile_table[cur_profile].ddr_frequency;
		*slowfreq = (s32)balong_query_profile_table[cur_profile].sbus_frequency;
	}
	else
	{
		cpufreq_err("argv is NULL,check it\n");
		ret = BSP_ERROR;
	}
	return ret;
}

static void balong_check_profile_limit(int *max_limit, int *min_limit)
{
	if (CPUFREQ_MAX_PROFILE_LIMIT < CPUFREQ_MIN_PROFILE_LIMIT)
	{
		*max_limit = BALONG_FREQ_MAX;
		*min_limit = BALONG_FREQ_MIN;
		return;
	}
	if ((CPUFREQ_MAX_PROFILE_LIMIT > (unsigned int)BALONG_FREQ_MAX) || ((CPUFREQ_MAX_PROFILE_LIMIT) < (unsigned int)BALONG_FREQ_MIN))
	{
		*max_limit = BALONG_FREQ_MAX;
	}
	else
	{
		*max_limit = (s32)CPUFREQ_MAX_PROFILE_LIMIT;
	}
	if ((CPUFREQ_MIN_PROFILE_LIMIT > (unsigned int)BALONG_FREQ_MAX) || ((CPUFREQ_MIN_PROFILE_LIMIT) < (unsigned int)BALONG_FREQ_MIN))
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
			/*判断本次请求是否和上次重复，重复则停止再次发送*/
			if ((1 == CPUFREQ_DOWN_FLAG(msg->source)) 
					&& (msg->profile == CPUFREQ_DOWN_PROFILE(msg->source)))
			{
				ret = BSP_ERROR;
			}
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
	return ret;
}
int balong_cpufreq_icc_send(struct cpufreq_msg *msg)
{

	u32 channel_id = ICC_CHN_MCORE_ACORE << 16 | MCU_ACORE_CPUFREQ;
	s32 ret = 0;
	u32 msg_len = sizeof(struct cpufreq_msg);
	if (!g_cpufreq_lock_status_flag)
	{
		return BSP_ERROR;
	}
	ret = balong_check_msg(msg);
	if (BSP_ERROR == ret)
	{
		return BSP_OK;
	}
	if (msg->msg_type != CPUFREQ_GET_FREQ_FROM_M)
	{
		debug_msg.msg_type = msg->msg_type;
		debug_msg.source = msg->source;
		debug_msg.content = msg->content;
		debug_msg.profile = msg->profile;
	}

	ret = bsp_icc_send(ICC_CPU_MCU, channel_id, (u8 *)msg, msg_len);

	if(ret != (s32)msg_len)
	{
		cpufreq_err("mcore return an ERROR please check m3 %d\n", ret);
		return BSP_ERROR;
	}
	
    return BSP_OK;
}

s32 balong_cpufreq_cb_getprofile(u32 channel_id , u32 len, void* context)
{

	s32 ret = 0;
	struct cpufreq_msg cm = {0};
	ret = bsp_icc_read(channel_id, (u8*)&cm, len);

	if((s32)len != ret)
	{
		cpufreq_err("balong_cpufreq_cb_getload error \r\n");
		return BSP_ERROR;
	}

	cpufreq_info("get icc from Mcore msg_type->>%d, source->>%d, profile->>%d\n", cm.msg_type, cm.source, cm.profile);

	return BSP_OK;
}


static s32 balong_cpu_freq_notifier(struct notifier_block *nb,
					unsigned long val, void *data)
{
	if (val == CPUFREQ_POSTCHANGE)
	{
		;//current_cpu_data.udelay_val = loops_per_jiffy;
	}
	else
	{
		;//for pclint
	}
	
	return 0;
}

static struct notifier_block balong_cpufreq_notifier_block = {
	.notifier_call = balong_cpu_freq_notifier,
};


/*
 *获取当前的频率
 *
 */
u32 balong_cpufreq_get(u32 cpu)
{
	return g_cur_freq;
}

static void cpufreq_frequency_target_profile( unsigned int target_freq,
				   unsigned int relation,  unsigned int *new_profile)
{
	int i = 0;
	int cur_profile = pwrctrl_dfs_get_profile();
	switch (relation)
	{
		case CPUFREQ_RELATION_H:
			*new_profile = balong_clockrate_table[BALONG_FREQ_MAX].index;
			return;
		case CPUFREQ_RELATION_L:
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
		if (cur_profile == (s32)balong_query_profile_table[i].profile)
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


/*
 * Here we notify other drivers of the proposed change and the final change.
 *
 *
 *根据relation做相应动作
 */
static s32 balong_cpufreq_target(struct cpufreq_policy *policy,
				     u32 target_freq,
				     u32 relation)
{
	u32 result = 2;
	u32 new_index = 0;
	int cur_profile = 0;
	struct cpufreq_msg task_msg = {0,0,0,0};
	
	
	cpufreq_frequency_table_target(policy, balong_clockrate_table,
					   target_freq, relation, &new_index);
					   
	cpufreq_debug("target_freq %d new_index%d\n", target_freq, new_index);

	cur_profile = pwrctrl_dfs_get_profile();
	
	if ((CPUFREQ_RELATION_H == relation) && (BALONG_FREQ_MAX != cur_profile))
	{
		result = DFS_PROFILE_UP_TARGET;
	}
	else if ((CPUFREQ_RELATION_L == relation) && (BALONG_FREQ_MIN != cur_profile))
	{
		result = DFS_PROFILE_DOWN_TARGET;
	}
	else
	{
		policy->cur = balong_clockrate_table[cur_profile].frequency;
		g_cur_freq = policy->cur;
		cpufreq_info("set target relation %d, cur pro %d\n", relation, policy->cur);
		return BSP_ERROR;
	}
	
	cpufreq_frequency_target_profile(balong_clockrate_table[new_index].frequency, relation, &new_index);

	task_msg.msg_type = CPUFREQ_ADJUST_FREQ;
	task_msg.source = CPUFREQ_ACORE;
	task_msg.content = result;
	task_msg.profile = new_index;
	balong_cpufreq_icc_send(&task_msg);
	
	policy->cur = balong_clockrate_table[cur_profile].frequency;
	g_cur_freq = policy->cur;
	return BSP_OK;
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
static s32 balong_cpufreq_cpu_init(struct cpufreq_policy *policy)
{
	/*cpu_online 这里的作用是 ? */
	if (!cpu_online(policy->cpu))
		return -ENODEV;

	pr_info("cpufreq: balong_cpufreq_cpu_init.\n");
	
	cpufreq_table_init();
	policy->governor = &cpufreq_balong_ondemand;
	policy->max = policy->cpuinfo.max_freq = BALONG_CPUFREQUENCY_666;
	policy->min = policy->cpuinfo.min_freq = BALONG_CPUFREQUENCY_100;
	policy->cur = BALONG_CPUFREQUENCY_666;
	g_cur_freq = policy->cur;
	cpufreq_frequency_table_get_attr(&balong_clockrate_table[0],
					 policy->cpu);

	return cpufreq_frequency_table_cpuinfo(policy,
					    &balong_clockrate_table[0]);
}

static s32 balong_cpufreq_verify(struct cpufreq_policy *policy)
{
	return cpufreq_frequency_table_verify(policy, &balong_clockrate_table[0]);
}

static s32 balong_cpufreq_exit(struct cpufreq_policy *policy)
{
	//clk_put(cpuclk);
	return 0;
}

static struct freq_attr *balong_table_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	NULL,
};

static struct cpufreq_driver balong_cpufreq_driver = {
	.owner = THIS_MODULE,
	.name = "balong_cpufreq",
	.init = balong_cpufreq_cpu_init,
	.verify = balong_cpufreq_verify,
	.target = balong_cpufreq_target,
	.get = balong_cpufreq_get,
	.exit = balong_cpufreq_exit,
	.attr = balong_table_attr,
};

static struct platform_device_id platform_device_ids[] = {
	{
		.name = "balong_cpufreq",
	},
	{}
};

MODULE_DEVICE_TABLE(platform, platform_device_ids);

static struct platform_driver platform_driver = {
	.driver = {
		.name = "balong_cpufreq",
		.owner = THIS_MODULE,
	},
	.id_table = platform_device_ids,
};

static s32 __init cpufreq_init(void)
{
	s32 ret;

	/* Register platform stuff ?????*/
	ret = platform_driver_register(&platform_driver);
	if (ret)
		return ret;

	pr_info("cpufreq: balongv7r2 CPU frequency driver.\n");

	cpufreq_register_notifier(&balong_cpufreq_notifier_block,
				  CPUFREQ_TRANSITION_NOTIFIER);

	ret = cpufreq_register_driver(&balong_cpufreq_driver);

	return ret;
}

static void __exit cpufreq_exit(void)
{

	cpufreq_unregister_driver(&balong_cpufreq_driver);
	cpufreq_unregister_notifier(&balong_cpufreq_notifier_block,
				    CPUFREQ_TRANSITION_NOTIFIER);

	platform_driver_unregister(&platform_driver);
}

module_init(cpufreq_init);
module_exit(cpufreq_exit);

//module_param(nowait, uint, 0644);

MODULE_AUTHOR("YQ ");
MODULE_DESCRIPTION("cpufreq driver for Balong");
MODULE_LICENSE("GPL");


/*test for send */
void test_send_msg(unsigned int msg_type, unsigned int source, unsigned int content, unsigned int profile)
{
	struct cpufreq_msg task_msg = {0,0,0,0};
	task_msg.msg_type = msg_type;
	task_msg.source = source;
	task_msg.content = content;
	task_msg.profile = profile;
	balong_cpufreq_icc_send(&task_msg);
}

void test_for_profile(int target_freq, int relation, int new_profile)
{
	unsigned int profile_test = (unsigned int)new_profile;
	cpufreq_frequency_target_profile((u32)target_freq, (u32)relation, &profile_test);
	cpufreq_err("profile_test %d\n", profile_test);
}

#ifdef __cplusplus
}
#endif