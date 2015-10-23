#include <vxWorks.h>
#include <stdlib.h>
#include <taskLib.h>
#include <cacheLib.h>
#include "hi_base.h"
#include "hi_syssc.h"
#include "hi_syscrg.h"
#include "DrvInterface.h"
#include "drv_comm.h"
#include "bsp_busstress.h"

BSP_BOOL g_bA9MemCpyTaskRun = BSP_FALSE;
BSP_U32 ccpu_busstress_count = 0;
BSP_S32 stress_test_rate = 2;
BSP_S32 cipher_stress_test_rate = 8;
extern BSP_S32 edma_stress_test_start(BSP_S32 task_priority, BSP_S32 test_rate);
extern BSP_S32 edma_stress_test_stop();
extern BSP_S32 acc_stress_test_start(BSP_S32 task_priority,BSP_S32 test_type,BSP_S32 test_rate);
extern BSP_S32 acc_stress_test_stop();
extern BSP_S32 cipher_stress_test_start(BSP_S32 task_priority,BSP_S32 test_rate);
extern BSP_S32 cipher_stress_test_stop();
extern BSP_S32 kdf_stress_test_start(BSP_S32 task_priority,BSP_S32 test_rate);
extern BSP_S32 kdf_stress_test_stop();
extern BSP_S32 ipf_dl_stress_test_start(BSP_S32 task_priority,BSP_S32 dtime);
extern BSP_S32 ipf_dl_stress_test_stop();

extern int bsp_bbe_stress_test_start(void);
extern int gu_master_stress_test_start(int task_priority,int delayticks);
extern int gu_master_stress_test_stop();


/*test process*/
BSP_S32 a9MemcpyTaskFunc(BSP_S32 s32TestSence,BSP_S32 s32DelayVal)
{
	BSP_U32 u32Src = 0;
	BSP_U32 u32Dst = 0;
	BSP_S32 s32Size = 0;
	BSP_U32 u32Cnt = 0;

	switch(s32TestSence)
	{
	case DDR:
		s32Size = A9_DATA_CACHE_SIZE;
		u32Src = (BSP_U32)malloc(s32Size);
		if(0 == u32Src)
		{
			printf("malloc %s buffer fail.\n","src");
			return ERROR;
		}
		
		u32Dst = (BSP_U32)malloc(s32Size);
		if(0 == u32Dst)
		{
			printf("malloc %s buffer fail.\n","dst");
			free((void*)u32Src);
			return ERROR;
		}
		break;
	case AXIMEM:
		s32Size = AXI_MEM_SIZE_FOR_A9;
		u32Src = (BSP_U32)alloc(s32Size);
		if(0 == u32Src)
		{
			printf("malloc %s buffer fail.\n","src");
			return ERROR;
		}
		
		u32Dst = (BSP_U32)alloc(s32Size);
		if(0 == u32Dst)
		{
			printf("malloc %s buffer fail.\n","dst");
			return ERROR;
		}
		break;
	default:
		printf("s32TestSence invalid.\n");
		return ERROR;			
	}

	while(g_bA9MemCpyTaskRun)
	{
		memcpyTestProcess(u32Src,u32Dst,s32Size);
		if(DDR == s32TestSence)
		{
			(BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)u32Dst, s32Size);
		}
		u32Cnt++;
		if(0 == (u32Cnt%5))
		{
			taskDelay(s32DelayVal);
		}
	}

	if(DDR == s32TestSence)
	{
		free((void*)u32Src);
		free((void*)u32Dst);
	}
	else	{}
	
	return OK;
}
BSP_S32 g_A9MemCpy_TaskPriority = 150;
BSP_S32 a9_mem_stress_test_start(BSP_S32 s32TestSence,BSP_S32 s32DelayValue)
{
	char taskName[30] = {0};
	BSP_S32 s32Ret = 0;
    BSP_S32 s32LocalDelayVal = s32DelayValue;
	
	if(0 == s32LocalDelayVal)
	{
		s32LocalDelayVal = 1;
	}

	sprintf(taskName,"a9S%dMemcpyTask",s32TestSence);
	g_bA9MemCpyTaskRun = BSP_TRUE;
	s32Ret = taskSpawn(taskName,g_A9MemCpy_TaskPriority,0,2000,(FUNCPTR)a9MemcpyTaskFunc,(int)s32TestSence,(int)s32LocalDelayVal,0,0,0,0,0,0,0,0);
	if(ERROR == s32Ret)
	{
		printf("taskSpawn %s fail.\n","a9S1MemcpyTask");
		return ERROR;		
	}
	
	return OK;
}

BSP_S32 a9_mem_stress_test_stop(BSP_S32 s32TestSence)
{
	BSP_S32 taskId = 0;
	char taskName[30] = {0};

	sprintf(taskName,"a9S%dMemcpyTask",s32TestSence);
	g_bA9MemCpyTaskRun = BSP_FALSE;
	taskId = taskNameToId(taskName);
	while(OK == taskIdVerify(taskId))
	{
		taskDelay(500);
	}	
	printf("delete %s OK.\n","a9S1MemcpyTask");
	return OK;
}

/*For A9 Access HIFI Local Mem and bbe16 Local Mem*/
BSP_BOOL g_bA9MemCpyTaskRunExt = BSP_TRUE;
BSP_S32 a9MemcpyTestFuncExt(BSP_U32 u32Src,BSP_U32 u32Dst,BSP_S32 s32Size,BSP_S32 s32DelayVal)
{
	BSP_U32 u32Cnt = 0;

	while(g_bA9MemCpyTaskRunExt)
	{
		if(0 != memcpyTestProcess(u32Src,u32Dst,s32Size))
		{
			printf("u32Src:0x%x,u32Dst:%x.\n",u32Src,u32Dst);
			break;
		}
	        (BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)u32Src, s32Size);
	        (BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)u32Dst, s32Size);
		u32Cnt++;
		if(0 == (u32Cnt%5))
		{
			taskDelay(s32DelayVal);
		}
	}
	return BSP_OK;
}
BSP_S32 a9MemcpyTaskSpawnExt(BSP_U32 u32Src,BSP_U32 u32Dst,BSP_S32 s32Size,BSP_S32 s32DelayValue)
{
	char taskName[30] = {0};
	BSP_S32 s32Ret = 0;
    BSP_S32 s32LocalDelayVal = s32DelayValue;
	
	if(0 == s32LocalDelayVal)
	{
		s32LocalDelayVal = 1;
	}

	sprintf(taskName,"a9MemcpyTask%x",u32Src);
	g_bA9MemCpyTaskRunExt = BSP_TRUE;
	s32Ret = taskSpawn(taskName,g_A9MemCpy_TaskPriority,0,2000,(FUNCPTR)a9MemcpyTestFuncExt,(int)u32Src,(int)u32Dst,(int)s32Size,(int)s32LocalDelayVal,0,0,0,0,0,0);
	if(ERROR == s32Ret)
	{
		printf("taskSpawn %s fail.\n",taskName);
		return ERROR;		
	}
	
	return OK;
}

BSP_S32 a9MemcpyTaskDeleteExt(BSP_U32 u32Src)
{
	BSP_S32 taskId = 0;
	char taskName[30] = {0};

	sprintf(taskName,"a9MemcpyTask%x",u32Src);
	g_bA9MemCpyTaskRunExt = BSP_FALSE;
	taskId = taskNameToId(taskName);
	while(OK == taskIdVerify(taskId))
	{
		taskDelay(500);
	}	
	printf("delete %s OK.\n",taskName);
	return OK;
}

/*
BSP_S32 a9_mem_stress_test_start_use_ddr(BSP_S32 s32DelayValue)
{
	void *src_addr;
	void *dst_addr;
	src_addr = malloc(DDR_MEM_SIZE_FOR_A9);
	if(NULL == src_addr)
	{
		return ERROR;
	}
	dst_addr = malloc(DDR_MEM_SIZE_FOR_A9);
	if(NULL == dst_addr)
	{
		return ERROR;
	}

	a9MemcpyTaskSpawnExt(src_addr, dst_addr,DDR_MEM_SIZE_FOR_A9,s32DelayValue);
	return OK;
}
*/

BSP_S32 a9_mem_stress_test_start_use_bbe16(BSP_S32 s32DelayValue)
{
	a9MemcpyTaskSpawnExt(BBE_TCM_ADDR,BBE_TCM_ADDR + BBE16_LOCAL_MEM_SIZE_FOR_A9 ,BBE16_LOCAL_MEM_SIZE_FOR_A9,s32DelayValue);
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_bbe16()
{
	a9MemcpyTaskDeleteExt(BBE_TCM_ADDR);
	return OK;
}

BSP_S32 a9_mem_stress_test_start_use_hifi(BSP_S32 s32DelayValue)
{
	BSP_S32 reg = 0x1C0000;  
	/*?a?¡ä??*/
 	writel(reg, 0x90000070);  
	a9MemcpyTaskSpawnExt(HIFI_TCM_ADDR,HIFI_LOCAL_MEM_FOR_MCORE_DST_ADDR,HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE,s32DelayValue);
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_hifi()
{
	a9MemcpyTaskDeleteExt(HIFI_TCM_ADDR);
	return OK;
}
#if 0
BSP_S32 a9_mem_stress_test_start_use_m3(BSP_S32 s32DelayValue)
{
	a9MemcpyTaskSpawnExt(M3_LOCAL_MEM_SRC_FOR_A9_MCORE,M3_LOCAL_MEM_DST_FOR_A9_MCORE,M3_LOCAL_MEM_FOR_MEMCPY_SIZE,s32DelayValue);
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_m3()
{
	a9MemcpyTaskDeleteExt(M3_LOCAL_MEM_SRC_FOR_A9_MCORE);
	return OK;
}
#endif
BSP_S32 a9_mem_stress_test_start_use_aximem64(BSP_S32 s32DelayValue)
{
	a9MemcpyTaskSpawnExt(AXI_MEM_64_SRC_FOR_A9_CCORE,AXI_MEM_64_DST_FOR_A9_CCORE,AXI_MEM_FOR_MEMCPY_SIZE,s32DelayValue);
	return OK;
}

BSP_S32 a9_mem_stress_test_start_use_aximem64_count()
{
	printk("ccpu busstress count: %u", ccpu_busstress_count);
	return OK;
}

BSP_S32 a9_mem_stress_test_stop_use_aximem64()
{
	a9MemcpyTaskDeleteExt(AXI_MEM_64_SRC_FOR_A9_CCORE);
	return OK;
}

/*bus stress test switch*/
BSP_S32 busstress_test_start(void)
{
	if(ERROR == edma_stress_test_start(150, stress_test_rate))
	logMsg("ccore edma stress test fail\n",0,0,0,0,0,0);
		
	if(0 != acc_stress_test_start(150, 0, cipher_stress_test_rate))
	logMsg("ccore acc stress test0 fail\n",0,0,0,0,0,0);
	
	if(0 != cipher_stress_test_start(150, cipher_stress_test_rate))
	logMsg("ccore cipher stress test fail\n",0,0,0,0,0,0);

	if(0 != kdf_stress_test_start(150, cipher_stress_test_rate))
	logMsg("ccore kdf stress test fail\n",0,0,0,0,0,0);

	if(0 != ipf_dl_stress_test_start(150, stress_test_rate))
	logMsg("ccore ipf dl stress test fail\n",0,0,0,0,0,0);

//	if(0 != a9_mem_stress_test_start_use_bbe16(stress_test_rate))
//	logMsg("ccore use bbe16 stress test fail\n",0,0,0,0,0,0);

	if(0 != a9_mem_stress_test_start_use_aximem64(stress_test_rate))
	logMsg("ccore use aximem64 stress test fail\n",0,0,0,0,0,0);
	
	if(-1 == gu_master_stress_test_start(150,stress_test_rate))
	logMsg("gu_master_stress_test_start stress test fail\n",0,0,0,0,0,0);

	if(0 != bsp_bbe_stress_test_start())
	logMsg("bsp_bbe_stress_test_start stress test fail\n",0,0,0,0,0,0);

	if(0 != bbp_stress_test_start(150,stress_test_rate))
	logMsg("bsp_bbe_stress_test_start stress test fail\n",0,0,0,0,0,0);

	return OK;
}

BSP_S32 busstress_test_stop(void)
{
	if(0 != edma_stress_test_stop())
	logMsg("ccore edma stress test stop fail\n",0,0,0,0,0,0);

	if(0 != acc_stress_test_stop())
	logMsg("ccore acc stress test0 stop fail\n",0,0,0,0,0,0);

	if(0 != cipher_stress_test_stop())
	logMsg("ccore cipher stress test stop fail\n",0,0,0,0,0,0);

	if(0 != kdf_stress_test_stop())
	logMsg("ccore kdf stress test stop fail\n",0,0,0,0,0,0);

	if(0 != ipf_dl_stress_test_stop())
	logMsg("ccore ipf dl stress test fail\n",0,0,0,0,0,0);

//	if(0 != a9_mem_stress_test_stop_use_bbe16())
//	logMsg("ccore use bbe16 stress test fail\n",0,0,0,0,0,0);

//	if(0 != a9_mem_stress_test_stop_use_hifi())
//	logMsg("ccore use hifi stress test fail\n",0,0,0,0,0,0);

	if(0 != a9_mem_stress_test_stop_use_aximem64())
	logMsg("ccore use aximem64 stress test fail\n",0,0,0,0,0,0);
	
	if(0 != gu_master_stress_test_stop())
	logMsg("gu_master_stress_test_stop stress test fail\n",0,0,0,0,0,0);
	
	return OK;
}



