
#include <intLib.h>
#include <logLib.h>
#include <vxWorks.h>
#include <msgQLib.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <osl_thread.h>
#include "cpufreq_balong.h"
#include <bsp_hardtimer.h>
#include <bsp_icc.h>
#include <bsp_nvim.h>
#include <bsp_dpm.h>
#include <product_config.h>
#include <drv_comm.h>


int stress_req_id[DC_RESV] = {0};
extern struct cpufreq_query balong_query_profile_table[];
int stress_test_timeout = 1;
extern void test_for_v9r1_interface(int inter_id, int request_id, int request_freq, int req_id);
void test_for_v9interface(void)
{
	int i = BALONG_FREQ_MAX;
	while (stress_test_timeout != 0)
	{
		test_for_v9r1_interface(2, DFS_QOS_ID_DDR_MINFREQ, balong_query_profile_table[i].ddr_frequency * 1000, stress_req_id[i]);
		taskDelay(stress_test_timeout);
		test_for_v9r1_interface(2, DFS_QOS_ID_DDR_MINFREQ, balong_query_profile_table[0].ddr_frequency * 1000, stress_req_id[i]);
		test_for_v9r1_interface(2, DFS_QOS_ID_DDR_MINFREQ, balong_query_profile_table[1].ddr_frequency * 1000, stress_req_id[i]);
		taskDelay(stress_test_timeout);
	}
	// release??
	for (i = 0; i < DC_RESV; i++)
	{
		if (i != BALONG_FREQ_MAX)
		{
			PWRCTRL_DfsQosRelease(DFS_QOS_ID_DDR_MINFREQ, &stress_req_id[i]);
		}
		else
		{
			PWRCTRL_DfsQosRelease(DFS_QOS_ID_DDR_MINFREQ, &stress_req_id[i]);
		}
	}
}
void cpufreq_stresstest_for_v9_interface(void)
{
	int i = 0;
	for (i = 0; i < DC_RESV; i++)
	{
		if (i != BALONG_FREQ_MAX)
		{
			PWRCTRL_DfsQosRequest(DFS_QOS_ID_DDR_MINFREQ, balong_query_profile_table[i].cpu_frequency * 1000 , &stress_req_id[i]);
		}
		else
		{
			PWRCTRL_DfsQosRequest(DFS_QOS_ID_DDR_MINFREQ, balong_query_profile_table[i].cpu_frequency * 1000 + 1 , &stress_req_id[i]);
		}
	}
	taskSpawn("stresstest_for_v9_inter", 5, 0, 4096, (FUNCPTR)test_for_v9interface, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); /*lint !e64 !e119 */
}
int g_test_icc_action = 2;
void test_for_iccsend(void)
{
	int i = BALONG_FREQ_MAX;
	while (stress_test_timeout != 0)
	{
		taskDelay(stress_test_timeout);
		test_send_msg(1,1,g_test_icc_action,i);
		test_send_msg(1,0,g_test_icc_action,i);
		if (i == BALONG_FREQ_MAX)
		{
			test_send_msg(1,0,4,i);
		}
		i = (i + 1) % DC_RESV;
	}
}
void cpufreq_stresstest_for_iccsend(void)
{
	taskSpawn("stresstest_for_iccsend", 5, 0, 4096, (FUNCPTR)test_for_iccsend, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); /*lint !e64 !e119 */
}
