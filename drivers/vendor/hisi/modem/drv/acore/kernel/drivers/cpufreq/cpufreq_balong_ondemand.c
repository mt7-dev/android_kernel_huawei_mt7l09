
/*lint --e{537 } */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/tick.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/percpu.h>
#include <osl_thread.h>
#include <osl_irq.h>
#include <osl_types.h>
#include <bsp_hardtimer.h>
#include <bsp_icc.h>
#include <bsp_nvim.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <drv_comm.h>
#include "cpufreq_balong.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*lint --e{24,34,35,43,50,63,64,110,78, 728,745,752, 958,808 } */


struct dbs_tuners dbs_tuners_ins = {0};
static DEFINE_PER_CPU(struct cpu_dbs_info_s, g_acpu_dbs_info);/*lint !e49 !e601 !e808 !e762 !e830 */
static DEFINE_PER_CPU(struct cpu_dbs_info_s, g_netif_dbs_info);/*lint !e49 !e601 !e808 !e762 !e830 */
static DEFINE_MUTEX(dbs_mutex); /*lint !e651 !e120 !e156 !e121 !e133 */
static DEFINE_MUTEX(info_mutex);/*lint !e651 */
/*读取NV*/
ST_PWC_DFS_STRU g_stDfsSwitch={0};

static u32 dbs_enable = 0;	/* number of CPUs using this policy */
/*当前profile*/
extern s32 g_cur_profile;
/*当前占用率*/
static u32 g_ulACpuload = 0;
/*cpufreq 锁定标志 1:未锁，0: 锁定(锁定后不会主动调频)*/
extern int g_cpufreq_lock_status_flag;
/*主动调频是否发送icc消息标志 1:可发送*/
static int g_icc_run_flag = 1;

extern struct cpufreq_msg debug_msg;

extern s32 balong_cpufreq_cb_getprofile(u32 channel_id , u32 len, void* context);
extern int balong_cpufreq_icc_send(struct cpufreq_msg *msg);
/*******************************************************/

static inline u64 get_cpu_idle_time_jiffy(unsigned int cpu, u64 *wall)
{
	u64 idle_time;
	u64 cur_wall_time;
	u64 busy_time;
	/*lint --e{718, 746, 737, 732, 712, 747 } */
	cur_wall_time = jiffies64_to_cputime64(get_jiffies_64());
	/*lint --e{409,550}*/
	busy_time  = kcpustat_cpu(cpu).cpustat[CPUTIME_USER];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_IRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SOFTIRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_STEAL];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_NICE];

	idle_time = cur_wall_time - busy_time;
	if (wall)
		*wall = jiffies_to_usecs(cur_wall_time);

	return jiffies_to_usecs(idle_time);
}
/*lint --e{551,713}*/
static cputime64_t get_cpu_idle_time(unsigned int cpu, cputime64_t *wall)/*lint !e551*/
{

	u64 idle_time = get_cpu_idle_time_us((int)cpu, NULL);/*lint !e530 !e712 */
	/*lint --e{501} */
	if (idle_time == -1ULL)
	{
		return get_cpu_idle_time_jiffy(cpu, wall);
	}
	else
	{
		idle_time += get_cpu_iowait_time_us((int)cpu, wall);
	}

	return idle_time;
}
/*lint --e{718,746}*/
unsigned int cpufreq_calccpu_load_netif(void)
{
    struct cpu_dbs_info_s *dbs_info;
    cputime64_t cur_wall_time = 0;
	cputime64_t cur_idle_time = 0;
	u32 idle_time = 0, wall_time = 0;
	unsigned int load = 0;

    dbs_info = &per_cpu(g_netif_dbs_info, 0);
    cur_idle_time = get_cpu_idle_time(0, &cur_wall_time);

    idle_time = (u32)(cur_idle_time - dbs_info->prev_cpu_idle);
	wall_time = (u32)(cur_wall_time - dbs_info->prev_cpu_wall);

	dbs_info->prev_cpu_idle = cur_idle_time;
	dbs_info->prev_cpu_wall = cur_wall_time;

	load = (wall_time == 0) ?
	    0 : (unsigned int)(100 * (wall_time - idle_time) / wall_time);
	return load;
}

/*lint -save -e438*/
/*lint --e{550}*/
unsigned int cpufreq_calccpu_cpuload(void)
{
	u32 idle_time = 0;
	u32 wall_time = 0;
	unsigned int cpu_load = 0;
	cputime64_t cur_wall_time = 0;
	cputime64_t cur_idle_time = 0;
	struct cpu_dbs_info_s *dbs_info;
	dbs_info = &per_cpu(g_acpu_dbs_info, 0); 
	
	cur_idle_time = get_cpu_idle_time(0, &cur_wall_time);
	idle_time = (u32)(cur_idle_time - dbs_info->prev_cpu_idle);
	wall_time = (u32)(cur_wall_time - dbs_info->prev_cpu_wall);

	cpu_load = (100 * (wall_time - idle_time) / wall_time);

	return cpu_load;
}
/*lint -restore +e438*/
/*
 * 该接口负责cpu负载检测，
 * 并根据预设阈值判决是否需要向M3请求调频
 */
void cpufreq_update_frequency(void)
{
	u32 cpuload = 0;
	int cur_profile = 0;
	struct cpufreq_msg task_msg = {CPUFREQ_ADJUST_FREQ, CPUFREQ_ACORE, 0, BALONG_FREQ_MAX};
	cpuload = (u32)cpufreq_calccpu_cpuload();
	cur_profile = (int)pwrctrl_dfs_get_profile();
	if (cpuload > dbs_tuners_ins.up_threshold)
   {
		task_msg.content = DFS_PROFILE_UP_TARGET;
	}
    else if (cpuload < dbs_tuners_ins.down_threshold)
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

/*
 *计算CPU占用率及算出将要调整的频率值
 */
unsigned int cpufreq_calccpu_result(u32 *nextfreq)
{
	u32 max_load_cpu = 0;
	struct cpufreq_policy *policy;
	u32 idle_time = 0, wall_time = 0;
	cputime64_t cur_wall_time = 0;
	cputime64_t cur_idle_time = 0;
	struct cpu_dbs_info_s *dbs_info;
	dbs_info = &per_cpu(g_acpu_dbs_info, 0);
	policy = dbs_info->cur_policy;

	cur_idle_time = get_cpu_idle_time(0, &cur_wall_time);

	idle_time = (u32)(cur_idle_time - dbs_info->prev_cpu_idle);
	wall_time = (u32)(cur_wall_time - dbs_info->prev_cpu_wall);

	dbs_info->prev_cpu_idle = cur_idle_time;
	dbs_info->prev_cpu_wall = cur_wall_time;

	/*获取cpu占用率*/
	max_load_cpu = 100 * (wall_time - idle_time) / wall_time;
	g_ulACpuload = max_load_cpu;

	/* Check for frequency increase or decrease*/
	if (max_load_cpu > dbs_tuners_ins.up_threshold)
	{
		dbs_info->cpu_up_time++;
		if (dbs_tuners_ins.up_threshold_times == dbs_info->cpu_up_time)
		{
			dbs_info->cpu_down_time = 0;
			dbs_info->cpu_up_time = 0;
			*nextfreq = policy->max;
			return CPUFREQ_RELATION_H;
		}
		return DFS_PROFILE_NOCHANGE;
	}
 	if (max_load_cpu < dbs_tuners_ins.down_threshold)
 	{
		dbs_info->cpu_down_time++;
		if (dbs_tuners_ins.down_threshold_times == dbs_info->cpu_down_time)
		{
			dbs_info->cpu_down_time = 0;
			dbs_info->cpu_up_time = 0;
			if (0 == max_load_cpu)
			{
				max_load_cpu = 1;
			}
			*nextfreq = (max_load_cpu * policy->cur)/	(dbs_tuners_ins.down_threshold);
			return CPUFREQ_RELATION_L;
		}
		return DFS_PROFILE_NOCHANGE;
	}
	*nextfreq = 0;
	dbs_info->cpu_down_time = 0;
	dbs_info->cpu_up_time = 0;
	return DFS_PROFILE_NOCHANGE;
}


void register_icc_for_cpufreq(void)
{
	s32 ret;
	u32 channel_id_set = ICC_CHN_MCORE_ACORE << 16 | MCU_ACORE_CPUFREQ;
	ret = bsp_icc_event_register(channel_id_set, (read_cb_func)balong_cpufreq_cb_getprofile, (void *)NULL, (write_cb_func)NULL, (void *)NULL);
	if (ret != BSP_OK)
	{
		cpufreq_err("icc register failed %d\n", ret);
	}
	else
	{
		;
	}
	cpufreq_debug("register icc to mcore %d\n", ret);
}/*lint !e533 */


/*
 *	变为两个函数，一个计算占用率，并返回要之后要如何操作
 *	一个根据返回完成接下来的操作
 */
void balong_dbs_check_cpu(void)
{
	u32 result = 2;
	u32 nextfreq = 0;
	struct cpu_dbs_info_s *dbs_info = &per_cpu(g_acpu_dbs_info, 0);
	struct cpufreq_policy *policy;

	policy = dbs_info->cur_policy;

	if (!g_cpufreq_lock_status_flag)
	{
		return;
	}
	result = cpufreq_calccpu_result(&nextfreq);
	if (result != DFS_PROFILE_NOCHANGE)
	{
		if (g_icc_run_flag)
		{
			__cpufreq_driver_target(policy, nextfreq, result);
		}
	}
	
}


/***********************************************************
*调频任务
***********************************************************/
void balong_do_dbs_timer(struct work_struct *work)
{
	struct cpu_dbs_info_s *dbs_info = &per_cpu(g_acpu_dbs_info, 0);
	s32 cpu = 0;
	unsigned long delay = 0;
	if (dbs_info == NULL){
		cpufreq_err("dbs_info error\n");
		return;
	}
	cpu = dbs_info->cpu;
	mutex_lock(&info_mutex);
	/*检查CPU占用率，调频*/
	balong_dbs_check_cpu();

	delay = usecs_to_jiffies(dbs_tuners_ins.sampling_rate);

	schedule_delayed_work_on(cpu, &dbs_info->work, delay);
	mutex_unlock(&info_mutex);
}

/***********************************************************
*调频任务初始化
***********************************************************/
static inline void dbs_timer_init(struct cpu_dbs_info_s *dbs_info)
{
	/* We want all CPUs to do sampling nearly on same jiffy */
	unsigned long delay = usecs_to_jiffies(dbs_tuners_ins.sampling_rate);
	if (NULL == dbs_info){
		cpufreq_err("!!!!!!dbs_timer_init!!!!!!error\n");
		return;
	}

	INIT_DELAYED_WORK_DEFERRABLE(&(dbs_info->work), balong_do_dbs_timer);/*lint !e613*/
	schedule_delayed_work_on(dbs_info->cpu, &(dbs_info->work), delay);/*lint !e613*/
}

static inline void dbs_timer_exit(struct cpu_dbs_info_s *dbs_info)
{
	cancel_delayed_work_sync(&dbs_info->work);
}

static s32 cpufreq_governor_dbs(struct cpufreq_policy *policy, u32 event)
{
	s32 cpu = (s32)policy->cpu;
	struct cpu_dbs_info_s *dbs_info = NULL;
	u32 retValue = 0;
	ST_PWC_SWITCH_STRU cpufreq_control_nv = {0} ;
	/*cpu 信息*/
	dbs_info = &per_cpu(g_acpu_dbs_info, (u32)cpu);
	/*lint --e{744 } */
	switch (event) {
	case CPUFREQ_GOV_START:
		cpufreq_debug("CPUFREQ_GOV_START\n");
		mutex_lock(&dbs_mutex);

		dbs_enable++;

		/*cpu 信息初始化  函数??idle_time*/
		dbs_info->prev_cpu_idle = get_cpu_idle_time(0,
						&dbs_info->prev_cpu_wall);
		dbs_info->cur_policy = policy;
		dbs_info->cpu = cpu;
		dbs_info->freq_table = cpufreq_frequency_get_table((u32)cpu);
		dbs_info->cpu_down_time = 0;
		dbs_info->cpu_up_time = 0;
		retValue = bsp_nvm_read(NV_ID_DRV_NV_PWC_SWITCH,(u8*)&cpufreq_control_nv,sizeof(ST_PWC_SWITCH_STRU));
		if (NV_OK == retValue)
		{
			g_cpufreq_lock_status_flag = cpufreq_control_nv.dfs;
		}
		else
		{
			cpufreq_err("read nv failed %d\n", retValue);
		}

		if (1 == dbs_enable){
			retValue = bsp_nvm_read(NV_ID_DRV_NV_DFS_SWITCH,(u8*)&g_stDfsSwitch,sizeof(ST_PWC_DFS_STRU));
		    if (NV_OK != retValue)
		    {
		    	cpufreq_err("read nv failed use default value\n");
				g_stDfsSwitch.AcpuDownLimit = 20;
				g_stDfsSwitch.AcpuDownNum = 3;
				g_stDfsSwitch.AcpuUpLimit = 80;
				g_stDfsSwitch.AcpuUpNum = 1;
				g_stDfsSwitch.DFSTimerLen = 400;
		    }
		    
			dbs_tuners_ins.up_threshold = g_stDfsSwitch.AcpuUpLimit;
			dbs_tuners_ins.down_threshold = g_stDfsSwitch.AcpuDownLimit;
			dbs_tuners_ins.down_threshold_times = g_stDfsSwitch.AcpuDownNum;
			dbs_tuners_ins.up_threshold_times = g_stDfsSwitch.AcpuUpNum;
			dbs_tuners_ins.sampling_rate = g_stDfsSwitch.DFSTimerLen * 10000; /*unit:us*/
			/*
			 * Start the timerschedule work, when this governor
			 * is used for first time
			 */

			register_icc_for_cpufreq();

			dbs_timer_init(dbs_info);
		}
		mutex_unlock(&dbs_mutex);
		break;

	case CPUFREQ_GOV_STOP:
		dbs_timer_exit(dbs_info);

		mutex_lock(&dbs_mutex);
		dbs_enable--;
		mutex_unlock(&dbs_mutex);
		break;

	case CPUFREQ_GOV_LIMITS:
			
			mutex_lock(&info_mutex);
			dbs_info->cpu_down_time = 0;
			dbs_info->cpu_up_time = 0;
			mutex_unlock(&info_mutex);
		if (policy->max < dbs_info->cur_policy->cur)
			__cpufreq_driver_target(dbs_info->cur_policy,
				policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > dbs_info->cur_policy->cur)
			__cpufreq_driver_target(dbs_info->cur_policy,
				policy->min, CPUFREQ_RELATION_L);

		break;
	}
	return 0;
}

struct cpufreq_governor cpufreq_balong_ondemand = {
       .name                   = "balong_ondemand",
       .governor               = cpufreq_governor_dbs,
       .max_transition_latency = TRANSITION_LATENCY_LIMIT,/*遗留:功能见注释*/
       .owner                  = THIS_MODULE,
};

static s32 __init cpufreq_gov_dbs_init(void)
{
	return cpufreq_register_governor(&cpufreq_balong_ondemand);
}

static void __exit cpufreq_gov_dbs_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_balong_ondemand);
}


fs_initcall(cpufreq_gov_dbs_init);
module_exit(cpufreq_gov_dbs_exit);
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
	dbs_tuners_ins.sampling_rate = sample;

	dbs_tuners_ins.up_threshold = up_cpuloadlimit;
	
	dbs_tuners_ins.down_threshold = down_cpuloadlimit;

	dbs_tuners_ins.down_threshold_times = down_times;

	dbs_tuners_ins.up_threshold_times = up_times;
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
static unsigned int busy_time = 10;
static int idle_time = 10;
static unsigned long start_time = 0;
void test_for_adjust_cpuload(int busytime, int idletime)
{
	if (busytime)
	busy_time = (unsigned int)busytime;
	if (idletime)
	idle_time = idletime;
}
#define TIMER_COUNT_TICK_TO_MS	(32768)

void test_for_pmuevent(void)
{/*lint --e{732, 716 } */
	while (1)
	{
		start_time = bsp_get_elapse_ms();
		while (bsp_get_elapse_ms()-start_time <= busy_time)
		{
			;
		}
		msleep(idle_time);
		if (!idle_time)
		{
			break;
		}
	}
}
void test_for_cpufreq_longtime(void)
{/*lint --e{732, 716 } */
	unsigned int which_load_bound = 0;
	/* coverity[INFINITE_LOOP] */
	/* coverity[no_escape] */
	while(1)
	{
		/*每30s改变cpu占用，测试长时间调频是否正常*/
		msleep(40000 * (which_load_bound + 1));
		switch(which_load_bound)
		{
			case 0:
				test_for_adjust_cpuload(1000,10);//90
				break;
			case 1:
				test_for_adjust_cpuload(1000,1000);//50
				break;
			case 2:
				test_for_adjust_cpuload(100,1000);//8-11
				break;
			default:
				break;
		}
		which_load_bound = (which_load_bound + 1) % 3;
	}
}
void test_for_pmu_task(void)
{
	struct task_struct*  task_id = 0;
	osl_task_init("pmu_test", 16, 0x1000 ,(void *)test_for_pmuevent, NULL, &task_id);
	//if (!idle_time)
	//osl_task_init("pmu_test", 16, 0x1000 ,(void *)test_for_cpufreq_longtime, NULL, &task_id);
}


void cpufreq_print_debug(void)
{
	struct cpu_dbs_info_s *dbs_info;
	dbs_info = &per_cpu(g_acpu_dbs_info, 0);
	pwrctrl_dfs_get_profile();
	cpufreq_err("cur    profile: %d\n", g_cur_profile);
	cpufreq_err("acore    load: %d\n", g_ulACpuload);
	cpufreq_err("up      times: %d\n", dbs_info->cpu_up_time);
	cpufreq_err("down  times: %d\n", dbs_info->cpu_down_time);
	cpufreq_err("up_threshold: %d\n", dbs_tuners_ins.up_threshold);
	cpufreq_err("up_threshold_times: %d\n", dbs_tuners_ins.up_threshold_times);
	cpufreq_err("down_threshold: %d\n", dbs_tuners_ins.down_threshold);
	cpufreq_err("down_threshold_times: %d\n", dbs_tuners_ins.down_threshold_times);
	cpufreq_err("sampling_rate: %d\n", dbs_tuners_ins.sampling_rate);
	cpufreq_err("last icc msg\n");
	cpufreq_err("icc msg_type: %d\n", debug_msg.msg_type);
	cpufreq_err("icc source: %d\n", debug_msg.source);
	cpufreq_err("icc content: %d\n", debug_msg.content);
	cpufreq_err("icc profile: %d\n", debug_msg.profile);
	cpufreq_err("cur max limit:%d\n", CPUFREQ_MAX_PROFILE_LIMIT);
	cpufreq_err("cur min limit:%d\n", CPUFREQ_MIN_PROFILE_LIMIT);
}



/******************************test for cpufreq end***********************************/
#ifdef __cplusplus
}
#endif

