#include <intLib.h>
#include <logLib.h>
#include <vxWorks.h>
#include <msgQLib.h>

#include <bsp_hardtimer.h>
#include <osl_thread.h>

void pmu_analysis_start(void);
void axi_mon_analysis_start(void);
void axi_mon_set_config(int start_addr, int end_addr, int congf_id, int check_time);
void pmu_set_config(int event_num, int pmu_event1, int pmu_event2, int check_time);

enum perf_type_enum
{
	PERF_AXI_ANALYSE = 0,
	PERF_PMU_ANALYSE = 1,
	PERF_ANALYSE_END
};

struct perf_ops
{
	void (* perf_config)(int argv1, int argv2, int argv3, int check_time);
	void (* perf_start)(void);
	void (*perf_dump)(void);
	int check_time;
};

struct perf_ops g_perf_ops;
int g_perf_type = PERF_ANALYSE_END;
void perf_analysis_config(int perf_type, int check_time, int argv1, int argv2, int argv3)
{
	switch (perf_type)
	{
		case PERF_AXI_ANALYSE:
			g_perf_ops.perf_config = axi_mon_set_config;
			g_perf_ops.perf_start = axi_mon_analysis_start;
			break;
		case PERF_PMU_ANALYSE:
			g_perf_ops.perf_config = pmu_set_config;
			g_perf_ops.perf_start = pmu_analysis_start;
			break;
		default:
			printf("perf_tyep is right? %d\n", perf_type);
			break;
	}
	if (NULL != g_perf_ops.perf_config)
	{
		g_perf_ops.perf_config(argv1, argv2, argv3, check_time);
		g_perf_ops.check_time = check_time;
	}
}

/*启动ddr test task*/
int g_ddr_size = 10000;
int g_ddr_time = 100;
int g_ddr_stop_flag = 1;/*是否停止DDR任务标志0:停止，1:继续执行*/
void perf_analysis_ddr_task(void)
{
	int ddr_num = 0;
	int ddr_size = sizeof(int)*g_ddr_size;
	char *ddr_src = (char *)malloc(ddr_size);
	char *ddr_des =(char *)malloc(ddr_size);
	if (ddr_src == NULL || ddr_des == NULL)
	{
		printf("malloc failed\n");
		free(ddr_src);
		free(ddr_des);
		return;
	}
	while(g_ddr_stop_flag)
	{
		memset(ddr_src, ddr_num++, ddr_size);
		memcpy(ddr_des, ddr_src, ddr_size);
		udelay(g_ddr_time);
		memset(ddr_des, ddr_num++, ddr_size);
		memcpy(ddr_src, ddr_des, ddr_size);
		udelay(2);
	}
	free(ddr_src);
	free(ddr_des);
}
void perf_analysis_start_test_task(void)
{
	g_ddr_stop_flag = 1;
	taskSpawn("perf_ddr_test", 240, 0, 4096, (FUNCPTR)perf_analysis_ddr_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
void perf_analysis_stop_test_task(void)
{
	g_ddr_stop_flag = 0;
}
void perf_analysis_start(void)
{
	/*开始进行监测.结束后输出*/
	if (NULL != g_perf_ops.perf_start)
	{
		g_perf_ops.perf_start();
	}	
}

void perf_analysis_init(void)
{
	g_perf_ops.perf_config = NULL;
	g_perf_ops.perf_start = NULL;
	g_perf_ops.perf_dump = NULL;
}



void perf_help(viod)
{
	printf("\n");
	printf("*************************************\n");
	printf("*perf_analysis_init 初始化perf模块\n");
	printf("*perf_analysis_config perf配置接口入参见文档\n");
	printf("*perf_analysis_start_test_task 测试任务接口 \n");
	printf("*perf_analysis_start 启动perf监测数据并输出\n");
	printf("*************************************\n");
}

