
#define VXWORKS 1
#include <osl_bio.h>
#include <soc_memmap.h>
#include <hi_base.h>
#include <hi_syssc.h>
#include "osl_spinlock.h"
#include "osl_sem.h"
#include "osl_thread.h"
#include "osl_irq.h"
#include "osl_cache.h"
#include "osl_malloc.h"


void test_osl_bio(void)
{
	unsigned  reg1;
	unsigned  reg2;
	writel(123, (unsigned)&reg1);
	reg2 = readl((unsigned)&reg1);
	if (reg1 != reg2)	{
		printf("test_osl_bio:fail! %d!=%d\n", reg1, reg2);
	}
	else {
		printf("test_osl_bio:ok\n");
	}


}

void test_platform_getset()
{
	unsigned int val;

	val = readl(HI_SYSSC_BASE_ADDR + HI_SC_CTRL2_OFFSET);
	printf("test_platform_getset: 0x%x+0x%x=0x%x\n", HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET, val);

	/*wdt_en_ctrl*/
	val = get_hi_sc_ctrl2_wdt_en_ctrl();
	printf("test_platform_getset:  get_hi_sc_ctrl2_wdt_en_ctrl=0x%x\n",  val);
	val = 6920;
	set_hi_sc_ctrl2_wdt_en_ctrl(val);
	val = get_hi_sc_ctrl2_wdt_en_ctrl();
	printf("test_platform_getset: get_hi_sc_ctrl2_wdt_en_ctrl=0x%x\n", val);

	/*wdt_en_ctrl*/
	val = get_hi_sc_ctrl2_wdt_clk_en();
	printf("test_platform_getset: get_hi_sc_ctrl2_wdt_clk_en=0x%x\n", val);
	val = 0;
	set_hi_sc_ctrl2_wdt_clk_en(val);
	val = get_hi_sc_ctrl2_wdt_clk_en();
	printf("test_platform_getset: get_hi_sc_ctrl2_wdt_clk_en=0x%x\n", val);

	val = readl(HI_SYSSC_BASE_ADDR + HI_SC_CTRL2_OFFSET);
	printf("test_platform_getset: 0x%x+0x%x=0x%x\n", HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET, val);

}

void test_irq_handle(void)
{
	logMsg("test_irq_handle ok\n",0,0,0,0,0,0);
}

void test_thread()
{
	while(1)
	{
		logMsg("test_thread \n",0,0,0,0,0,0);

		taskDelay(100000);
	}
}

void test_osl_spinlock(void)
{
	spinlock_t lock;
	unsigned long flag = 0;

	spin_lock_init(&lock);
	spin_lock(&lock);
	spin_unlock(&lock);
	spin_lock_irqsave(&lock, flag);
	spin_unlock_irqrestore(&lock, flag);
}

void test_osl_sem(void)
{
	struct semaphore *sem;

	sema_init(sem, 0);

	up(sem);
	down(sem);
}

void test_osl_cache(void)
{
	__dma_single_cpu_to_dev(0x53c00000,512,DMA_TO_DEVICE);
	__dma_single_dev_to_cpu(0x53c00000,512,DMA_TO_DEVICE);
}

void test_osl_irq(void)
{
	int flag = 0;

	local_irq_save(flag);

	local_irq_restore(flag);

	request_irq(35, (irq_handler_t)test_irq_handle, 0, NULL, NULL);
}

void test_osl_thread(void)
{
	int taskid = 0;
	u32* mem = NULL;
	taskid = kthread_run(test_thread, 0, "testTask");

	taskDelay(150000);

	kthread_stop(taskid);

	if(osl_task_init("test-lt", 150, 8192, test_thread, NULL, &taskid)!= OK)
	{
		logMsg(" osl_task_init error \n",0,0,0,0,0,0);
	}

	mem = osl_malloc(32);
}



