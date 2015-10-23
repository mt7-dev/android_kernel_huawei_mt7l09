#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include "DrvInterface.h"
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include "bsp_sram.h"

#define SENCE_NUM (4)
#define MODULE_NUM (20)

char* g_testSenceName[SENCE_NUM] = {"S1","S2","S3","S4"};
int   module_test_switch_stat[MODULE_NUM] = {0};

static unsigned long next = 1;
static unsigned __alloc_next = SRAM_BUSSTRESS_ADDR;

void *alloc(signed sz)
{
    void *ptr;
    
    ptr = (void*) __alloc_next;
    __alloc_next = (__alloc_next + sz + 31) & (~31);

    return ptr;
}

/* RAND_MAX assumed to be 32767 */
static int myrand(void) {
   next = next * 1103515245 + 12345;
   return((unsigned)(next/65536) % 32768);
}

static void mysrand(unsigned seed) {
   next = seed;
}

int memcpyTestProcess(unsigned int uiSrc,unsigned int uiDst,int iSize)
{
	int iRet = 0;

	mysrand(uiSrc);
	memset((void*)uiSrc,myrand(),iSize);
	memset((void*)uiDst,0,iSize);
	memcpy((void*)uiDst,(void*)uiSrc,iSize);
	iRet = memcmp((void*)uiSrc,(void*)uiDst,iSize);
	if(0 != iRet)
	{
		printk("memcmp fail,iRet:0x%x\n",iRet);
		return iRet;
	}

	return 0;
}


int get_test_switch_stat(int module_id)
{
	return module_test_switch_stat[module_id];
}

int set_test_switch_stat(int module_id, int run_or_stop)
{
	module_test_switch_stat[module_id] = run_or_stop;
	return 0;
}
