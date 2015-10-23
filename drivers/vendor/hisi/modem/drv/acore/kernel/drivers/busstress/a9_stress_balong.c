#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include "hi_base.h"
#include "hi_syssc.h"
#include "hi_syscrg.h"
#include "drv_comm.h"
#include "drv_memory.h"
#include "bsp_busstress.h"

typedef struct a9_kthread_stru_tag
{
	int testsence;
	int delayval;
	int srcaddr;
	int dstaddr;
	int size;
} a9_kthread_stru;

BSP_BOOL g_bA9MemCpyTaskRun = BSP_FALSE;
a9_kthread_stru g_a9_kthread_para;
BSP_S32 stress_test_rate = 2;
BSP_U32 acpu_busstress_count = 0;

extern BSP_S32 SOCP_ST_ENCODE_STRESS(void);
extern BSP_S32 SOCP_ST_DECODE_STRESS(void);
extern BSP_S32 ipf_ul_stress_test_start(BSP_S32 task_priority);
extern BSP_S32 ipf_dl_stress_test_start();
extern BSP_VOID ipf_ul_stress_test_stop();
extern int nandc_stress_test_start();
extern int nandc_stress_test_stop();
extern s32 lcd_emi_edma_stress_test_start(u32 interval);
extern s32 lcd_emi_edma_stress_test_stop(void);


/*test process*/
BSP_S32 a9MemcpyTaskFunc(int para)
{
	BSP_U32 u32Src = 0;
	BSP_U32 u32Dst = 0;
	BSP_S32 s32Size = 0;
	BSP_U32 u32Cnt = 0;
	a9_kthread_stru* p_a9_kthread_stru = (a9_kthread_stru*)para;
	BSP_S32 s32TestSence = p_a9_kthread_stru->testsence;
	BSP_S32 s32DelayVal = p_a9_kthread_stru->delayval;
	
	switch(s32TestSence)
	{
	case DDR:
		s32Size = A9_DATA_CACHE_SIZE;
		u32Src = (BSP_U32)kmalloc(s32Size,GFP_KERNEL);
		if(0 == u32Src)
		{
			printk("malloc %s buffer fail.\n","src");
			return ERROR;
		}
		
		u32Dst = (BSP_U32)kmalloc(s32Size,GFP_KERNEL);
		if(0 == u32Dst)
		{
			printk("malloc %s buffer fail.\n","dst");
			kfree((void*)u32Src);
			return ERROR;
		}
		break;
	case AXIMEM:
		s32Size = AXI_MEM_SIZE_FOR_A9;
		u32Src = (BSP_U32)alloc(s32Size);
		if(0 == u32Src)
		{
			printk("malloc %s buffer fail.\n","src");
			return ERROR;
		}
		
		u32Dst = (BSP_U32)alloc(s32Size);
		if(0 == u32Dst)
		{
			printk("malloc %s buffer fail.\n","dst");
			return ERROR;
		}
		break;
	default:
		printk("s32TestSence invalid.\n");
		return ERROR;			
	}

	while(g_bA9MemCpyTaskRun)
	{
		memcpyTestProcess(u32Src,u32Dst,s32Size);
		if(DDR == s32TestSence)
		{
			__dma_single_cpu_to_dev((const void *)(u32Dst), s32Size, DMA_TO_DEVICE);
		}
		u32Cnt++;
		if(0 == (u32Cnt%5))
		{
			msleep(s32DelayVal);
		}
	}

	if(DDR == s32TestSence)
	{
		kfree((void*)u32Src);
		kfree((void*)u32Dst);
	}
	else {}
	
	return OK;
}

BSP_S32 a9_mem_stress_test_start(BSP_S32 s32TestSence,BSP_S32 s32DelayValue)
{
	BSP_U8 taskName[30] = {0};
	BSP_S32 s32Ret = 0;
       BSP_S32 s32LocalDelayVal = s32DelayValue;
	   
	if(0 == s32LocalDelayVal)
	{
		s32LocalDelayVal = 1;
	}

       g_a9_kthread_para.testsence = s32TestSence;
	g_a9_kthread_para.delayval = s32LocalDelayVal;
	
	sprintf(taskName,"a9S%dMemcpyTask",s32TestSence);
	g_bA9MemCpyTaskRun = BSP_TRUE;
	
	kthread_run(a9MemcpyTaskFunc,&g_a9_kthread_para,taskName,0,0);	
	return OK;
}

BSP_S32 a9_mem_stress_test_stop()
{
	g_bA9MemCpyTaskRun = BSP_FALSE;
	msleep(200);
	return OK;
}

/*For A9 Access HIFI Local Mem and bbe16 Local Mem*/
BSP_BOOL g_bA9MemCpyTaskRunExt = BSP_TRUE;
BSP_S32 a9MemcpyTestFuncExt(int para)
{
	BSP_U32 u32Cnt = 0;
	a9_kthread_stru* p_a9_kthread_stru = (a9_kthread_stru*)para;
       BSP_U32 u32Src = p_a9_kthread_stru->srcaddr;
	BSP_U32 u32Dst = p_a9_kthread_stru->dstaddr;
	BSP_S32 s32Size = p_a9_kthread_stru->size;
	BSP_S32 s32DelayVal = p_a9_kthread_stru->delayval;

	while(g_bA9MemCpyTaskRunExt)
	{
		//memcpyTestProcess(u32Src,u32Dst,s32Size);
		if(0 != memcpyTestProcess(u32Src,u32Dst,s32Size))
		{
			printk("u32Src:0x%x,u32Dst:0x%x.\n",u32Src,u32Dst);
			break;
		}
	        //(BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)u32Src, s32Size);
	        //(BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)u32Dst, s32Size);
	        __dma_single_cpu_to_dev_noverify((const void *)(u32Src), s32Size, DMA_TO_DEVICE);
	        __dma_single_cpu_to_dev_noverify((const void *)(u32Dst), s32Size, DMA_TO_DEVICE);
		acpu_busstress_count++;
		u32Cnt++;
		if(0 == (u32Cnt%5))
		{
			msleep(s32DelayVal);
		}
	}
	return BSP_OK;
}
BSP_S32 a9MemcpyTaskSpawnExt(BSP_U32 u32Src,BSP_U32 u32Dst,BSP_S32 s32Size,BSP_S32 s32DelayValue)
{
	BSP_U8 taskName[30] = {0};
	BSP_S32 s32Ret = 0;
       BSP_S32 s32LocalDelayVal = s32DelayValue;

	if(0 == s32LocalDelayVal)
	{
		s32LocalDelayVal = 1;
	}

	sprintf(taskName,"a9MemcpyTask%x",u32Src);
	g_bA9MemCpyTaskRunExt = BSP_TRUE;
	
	g_a9_kthread_para.srcaddr = u32Src;
	g_a9_kthread_para.dstaddr = u32Dst;
	g_a9_kthread_para.size = s32Size;
	g_a9_kthread_para.delayval = s32LocalDelayVal;
	
	kthread_run(a9MemcpyTestFuncExt,&g_a9_kthread_para,taskName,0,0);
	
	return OK;
}

BSP_S32 a9MemcpyTaskDeleteExt(void)
{
	g_bA9MemCpyTaskRunExt = BSP_FALSE;
	msleep(1000);
	return OK;
}
void* g_bbe16_src_v = NULL;
void* g_bbe16_dst_v   = NULL;

BSP_S32 a9_mem_stress_test_start_use_ddr(BSP_S32 s32DelayValue)
{
	void *src_addr;
	void *dst_addr;

	src_addr = kmalloc(DDR_MEM_SIZE_FOR_A9, GFP_KERNEL);
	if(NULL == src_addr)
	{
		return ERROR;
	}
	dst_addr = kmalloc(DDR_MEM_SIZE_FOR_A9, GFP_KERNEL);
	if(NULL == dst_addr)
	{
		return ERROR;
	}

	a9MemcpyTaskSpawnExt(src_addr, dst_addr,DDR_MEM_SIZE_FOR_A9,s32DelayValue);
	return OK;
}


BSP_S32 a9_mem_stress_test_start_use_bbe16(BSP_S32 s32DelayValue)
{
	/*�⸴λ*/
#ifdef BSP_DSP_BBE16
	set_hi_crg_ctrl15_tensi_dps0_srst_req(0);
	set_hi_crg_ctrl15_tensi_bbe16_srst_req(0);
#else
	set_hi_crg_srstdis2_tensi_dps0_pd_srst_dis(1);
	set_hi_crg_srstdis2_tensi_dsp0_core_srst_dis(1);
#endif

	g_bbe16_src_v   = ioremap(BBE16_LOCAL_MEM_START_ADDR_FOR_A9_A,2*BBE16_LOCAL_MEM_SIZE_FOR_A9);
	if(!g_bbe16_src_v)
	{
		return -1;
	}
	g_bbe16_dst_v   = g_bbe16_src_v + BBE16_LOCAL_MEM_SIZE_FOR_A9;
	a9MemcpyTaskSpawnExt(g_bbe16_src_v,g_bbe16_dst_v,BBE16_LOCAL_MEM_SIZE_FOR_A9,s32DelayValue);
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_bbe16()
{
	a9MemcpyTaskDeleteExt();
	return OK;
}
BSP_S32 a9_mem_stress_test_start_use_hifi(BSP_S32 s32DelayValue)
{

	BSP_U32 hifi2_tcm_addr; 
	hifi2_tcm_addr = ioremap_nocache(HI_HIFI2DMEM0_BASE_ADDR, HI_HIFI2DMEM0_SIZE); 
	a9MemcpyTaskSpawnExt(hifi2_tcm_addr + 2*HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE,hifi2_tcm_addr + 3*HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE,HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE,s32DelayValue); 
	
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_hifi()
{
	a9MemcpyTaskDeleteExt();
	return OK;
}

#if 0
BSP_S32 a9_mem_stress_test_start_use_m3(BSP_S32 s32DelayValue)
{
	BSP_U32 m3_tcm_addr;
	m3_tcm_addr = ioremap_nocache(M3_LOCAL_MEM_SRC_FOR_A9_MCORE, 16*M3_LOCAL_MEM_FOR_MEMCPY_SIZE); 
	a9MemcpyTaskSpawnExt(m3_tcm_addr + 2*M3_LOCAL_MEM_FOR_MEMCPY_SIZE,m3_tcm_addr + 3*M3_LOCAL_MEM_FOR_MEMCPY_SIZE,M3_LOCAL_MEM_FOR_MEMCPY_SIZE,s32DelayValue); 
	return OK;
}

BSP_S32 a9_mem_stress_test_stop_use_m3()  
{
	a9MemcpyTaskDeleteExt();
	return OK;
}
#endif
BSP_S32 a9_mem_stress_test_start_use_aximem64(BSP_S32 s32DelayValue)
{
	a9MemcpyTaskSpawnExt(AXI_MEM_64_VSRC_FOR_A9_ACORE,AXI_MEM_64_VDST_FOR_A9_ACORE,AXI_MEM_FOR_MEMCPY_SIZE,s32DelayValue);
	return OK;
}
BSP_S32 a9_mem_stress_test_stop_use_aximem64()
{
	a9MemcpyTaskDeleteExt();
	return OK;
}
BSP_S32 a9_mem_stress_test_start_use_aximem64_count()
{
	printk("acpu busstress count: %u", acpu_busstress_count);
	return OK;
}

/*bus stress test switch*/
BSP_S32 busstress_test_start(void)
{
	if(0 != SOCP_ST_ENCODE_STRESS())
	printk("acore socp encode stress test fail\n");

	if(0 != SOCP_ST_DECODE_STRESS())
	printk("acore socp decode stress test fail\n");

	ipf_ul_stress_test_start(stress_test_rate);

//	if(0 != nandc_stress_test_start())
//	printk("acore nandc stress test fail\n");
	
	if(0 != a9_mem_stress_test_start_use_ddr(stress_test_rate))
	printk("acore use bbe16 stress test fail\n");

	if(0 != a9_mem_stress_test_start_use_aximem64(stress_test_rate))
	printk("acore use aximem64 stress test fail\n");

	if(0 != lcd_emi_edma_stress_test_start(stress_test_rate))
	printk("ccore use lcd_emi stress test fail\n");

	return OK;
}


BSP_S32 busstress_test_stop(void)
{
	ipf_ul_stress_test_stop();

//	if(0 != nandc_stress_test_stop())
//	printk("acore nandc stress test fail\n"); 


	if(0 != a9_mem_stress_test_stop_use_aximem64())
	printk("ccore use aximem64 stress test fail\n");

	if(0 != lcd_emi_edma_stress_test_stop())
	printk("ccore use lcd_emi stress test fail\n");

	return OK;
}

module_init(ipf_dl_stress_test_start);



