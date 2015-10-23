#include <intLib.h>
#include <logLib.h>
#include <vxWorks.h>
#include <msgQLib.h>

#include <bsp_hardtimer.h>
#include <bsp_icc.h>
#include <osl_thread.h>

#include <drv_amon.h>
#include <amon_balong.h>

extern AXI_MON_CONFIG_CNF_STRU * bsp_axi_mon_config(u8 * data, u32 * out_len);
extern s32 bsp_axi_mon_start(u8 * data);
extern AXI_MON_TERMINATE_CNF_STRU * bsp_axi_mon_terminate(u8 * data, u32 * out_len);
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
extern int bsp_socp_vote_to_mcore(unsigned int type);
#endif

axi_mon_config_t g_axi_mon_conf = {0};
AXI_MON_TERMINATE_CNF_STRU *g_axi_mon_term_info = NULL;
int axi_idle_time = 500;
int axi_idle_times = 1;
int g_config_id = 0;

/*配置axi信息*/
int g_opt_type = 0;
void axi_mon_set_config(int start_addr, int end_addr, int congf_id, int check_time)
{
	g_config_id = congf_id;
	g_axi_mon_conf.win_en = 0;
	g_axi_mon_conf.mode = AXI_SOC_CONFIG;
	g_axi_mon_conf.id_config[congf_id].id_en = 1;
	g_axi_mon_conf.id_config[congf_id].port = 3;   /*modem a9 需要查表*/
	g_axi_mon_conf.id_config[congf_id].id_mon_en = 1;
	g_axi_mon_conf.id_config[congf_id].id_value = 0;
	g_axi_mon_conf.id_config[congf_id].mask = 0xffff; /*mask 取反和表中监控ID信息相与得到得值为监控ID*/
	g_axi_mon_conf.id_config[congf_id].addr_en = 1;
	g_axi_mon_conf.id_config[congf_id].addr_start = start_addr;
	g_axi_mon_conf.id_config[congf_id].addr_end = end_addr;
	memset(&g_axi_mon_conf.id_config[congf_id].opt_type, g_opt_type, sizeof(axi_opt_type_t));

	axi_idle_time = check_time;
	/*请求给socp上电*/
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
	bsp_socp_vote_to_mcore(1);
#endif
}

extern void perf_analysis_stop_test_task(void);
void axi_mon_analysis_task(void)
{
	int i = 0, ret = 0;
	u32 axi_mon_size = 0;
	u32 axi_mon_term_size = 0;
	axi_ctrl_t axi_ctrl = {0};
	
	axi_ctrl.mode = AXI_SOC_CONFIG;
	axi_ctrl.ctrl_type = AXI_CTRL_START;
	
	axi_mon_size = sizeof(axi_mon_config_t);
	axi_mon_term_size = sizeof(AXI_MON_TERMINATE_CNF_STRU);
	
	(void)bsp_axi_mon_config((u8 *)&g_axi_mon_conf, &axi_mon_size);
	
	//printf("start axi mon %d\n", axi_mon_size);
	
	for (i = 0; i < axi_idle_times; i++)
	{
		axi_ctrl.ctrl_type = AXI_CTRL_START;
		ret = bsp_axi_mon_start((u8 *)&axi_ctrl);
		if (ret == BSP_ERROR)
		{
			printf("mon start error\n");
			break;
		}
		
		taskDelay(axi_idle_time);
		
		axi_ctrl.ctrl_type = AXI_CTRL_STOP;
		g_axi_mon_term_info = bsp_axi_mon_terminate((u8 *)&axi_ctrl, &axi_mon_term_size);
	}
	if (g_axi_mon_term_info == NULL)
	{
		printf("axi mon term is NULL\n");
		return;
	}
	printf("\n");
	printf("ulIncr1Cnt:%d, ulIncr2Cnt: %d, ulIncr4Cnt: %d\n", 
		g_axi_mon_term_info->idStat[g_config_id].ulIncr1Cnt, g_axi_mon_term_info->idStat[g_config_id].ulIncr2Cnt, g_axi_mon_term_info->idStat[g_config_id].ulIncr4Cnt);
	printf("ulIncr8Cnt:%d, ulIncr16Cnt:%d, ulWrapCnt:%d\n",
		g_axi_mon_term_info->idStat[g_config_id].ulIncr8Cnt, g_axi_mon_term_info->idStat[g_config_id].ulIncr16Cnt, g_axi_mon_term_info->idStat[g_config_id].ulWrapCnt);
	printf("ulBurstSendCnt:%d, ulBurstFinCnt:%d\n",
		g_axi_mon_term_info->idStat[g_config_id].ulBurstSendCnt, g_axi_mon_term_info->idStat[g_config_id].ulBurstFinCnt);
	
	cacheDmaFree(g_axi_mon_term_info);
	memset(&g_axi_mon_conf, 0, sizeof(axi_mon_config_t));
	/*请求给socp下电*/
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
	bsp_socp_vote_to_mcore(0);
#endif
	perf_analysis_stop_test_task();
}

void axi_mon_analysis_start(void)
{
	taskDelay(1);
	taskSpawn("axi_test", 100, 0, 4096, (FUNCPTR)axi_mon_analysis_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

