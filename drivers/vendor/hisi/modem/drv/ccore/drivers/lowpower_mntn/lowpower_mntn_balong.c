/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  lowpower_mntn_balong.c
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于低功耗可谓可测信息的记录和上报
*
*   修改记录 :  2013年6月10日  v1.00 xujingcui创建
*************************************************************************/
/*lint -save -e537*/
#include <bsp_lowpower_mntn.h>
#include <bsp_wakelock.h>
#include <string.h>
#include <vxWorks.h>
#include <mmuLib.h>
#include <errno.h>
#include <cacheLib.h>
#include <private/classLibP.h>
#include <private/objLibP.h>
#include <vmLib.h>
#include <cacheLib.h>
#include <private/memPartLibP.h>
#include <private/vmLibP.h>
#include <stdlib.h>
#include <osl_irq.h>
/*lint -restore +e537*/

/* 系统头文件和vxworks的源码pc-lint找到 */
/*lint --e{18, 40, 115, 652, 661, 831, 830, 752} */
extern int snprintf(char *str, int sz, const char *fmt, ...);

#define  mntn_printf(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_WAKELOCK, "[Lowpower_mntn]: <%s> "fmt"", __FUNCTION__, ##__VA_ARGS__))

/*此全局变量用于记录超过2s未进入休眠的投票时间戳*/
static unsigned int time_stamp = 0;
#define AWAKE_TIME     (32768*2)

#ifndef VIRT_ADDR
#define VIRT_ADDR	UINT32
#endif

struct lock_info hi6930_lockinfo;
/*低功耗控制和DEBUG NV --NV_ID_DRV_NV_PWC_SWITCH */
ST_PWC_SWITCH_STRU g_nv_pwc_switch = {0};

/*记录低功耗共享地址*/
unsigned int g_lowpower_shared_addr = 0;

extern MMU_LIB_FUNCS	mmuLibFuncs;		  /* initialized by mmuLib.c */
#define MMU_ATTR_NORMAL_NONCACHEABLE_MSK (MMU_ATTR_SPL_MSK | MMU_ATTR_CACHE_MSK) /*mmuArmArch6PalLib.h*/
#define MMU_ATTR_NORMAL_NONCACHEABLE     (MMU_ATTR_SPL_0 | MMU_ATTR_CACHE_OFF) /*mmuArmArch6PalLib.h*/

#define LP_REG_MNTN_NULL   0xFFFFFFFF
struct lp_dump_mem_info lowpower_dump_mem = {
	0,
	0,
	0,
	{}
};
struct lp_dump_mem_info lowpower_dump_bus_error = {
	0,
	0,
	0,
	{}
};

/*记录需要保存的低功耗寄存器,寄存器为可以直接readl的寄存器*/
#ifdef CONFIG_V7R2_CLK_CRG /* CONFIG_V7R2_CLK_CRG */
unsigned int lowpower_register[]={
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT9_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT10_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT22_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT1_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT2_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT1_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT2_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLK_SEL3_OFFSET,       /*BBP SEL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS2_CTRL3_OFFSET,   /*DSP PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS3_CTRL3_OFFSET,   /*HIFI PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS4_CTRL3_OFFSET,   /*BBP PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS5_CTRL3_OFFSET,   /*PERI PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_CTRL2_OFFSET,            /*TCXO*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_STAT4_OFFSET,            /*MTCOMS*/
	LP_REG_MNTN_NULL
};

/*记录需要保存的防挂死寄存器的值*/
unsigned int lowpower_bus_error_register[]={
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT41_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT42_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT43_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT44_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT45_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT46_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT47_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT48_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT49_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT50_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT51_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT52_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT53_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT54_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT55_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT56_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT57_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT58_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT59_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT60_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT61_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT62_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT63_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT64_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT65_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT66_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT67_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT68_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT69_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT1_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT2_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT1_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT2_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLK_SEL3_OFFSET,       /*BBP SEL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS2_CTRL3_OFFSET,   /*DSP PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS3_CTRL3_OFFSET,   /*HIFI PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS4_CTRL3_OFFSET,   /*BBP PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DFS5_CTRL3_OFFSET,   /*PERI PLL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_CTRL2_OFFSET,            /*TCXO*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_STAT4_OFFSET,            /*MTCOMS*/
	LP_REG_MNTN_NULL
};
#elif defined(CONFIG_K3V3_CLK_CRG) /* CONFIG_K3V3_CLK_CRG */
unsigned int lowpower_register[]={
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT1_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLK_SEL3_OFFSET,  /*BBP SEL*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DSPPLL_CFG0_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_BBPPLL_CFG0_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_CTRL2_OFFSET,  /*TCXO*/
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_STAT4_OFFSET,  /*MTCOMS*/
	LP_REG_MNTN_NULL
};

/*记录需要保存的防挂死寄存器的值*/
unsigned int lowpower_bus_error_register[]={
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT41_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT42_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT43_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT44_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT46_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT47_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT48_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT49_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT50_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT51_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT52_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT53_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT54_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT55_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT56_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT57_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT62_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT63_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT64_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT65_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT66_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT67_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT68_OFFSET,
	HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT69_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT3_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT1_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_CLK_SEL3_OFFSET,  /*BBP SEL*/
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_DSPPLL_CFG0_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_CRG_BBPPLL_CFG0_OFFSET,
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_CTRL2_OFFSET,  /*TCXO*/
    HI_SYSCTRL_BASE_ADDR_VIRT + HI_PWR_STAT4_OFFSET,  /*MTCOMS*/
	LP_REG_MNTN_NULL
};
#else
unsigned int lowpower_register[]={
	LP_REG_MNTN_NULL
};

/*记录需要保存的防挂死寄存器的值*/
unsigned int lowpower_bus_error_register[]={
	LP_REG_MNTN_NULL
};
#endif /* CONFIG_V7R2_CLK_CRG */

/*此数值用于保存最后一次保存的信息*/
#define LP_SAVE_REG_NUM   50
unsigned int lowpower_current_reg_status[LP_SAVE_REG_NUM] = {0};

/*记录最后一次挂死寄存器的值*/
#define LP_SAVE_BUS_ERROR_REG_NUM   80
unsigned int lowpower_last_bus_error_register[LP_SAVE_BUS_ERROR_REG_NUM] = {0};

void bsp_save_lowpower_status(void)
{
	unsigned long flags = 0;
	unsigned int i = 0;
	unsigned int regvalue = 0;
	if(lowpower_register[0] == LP_REG_MNTN_NULL){
		mntn_printf("Has nothing to save!\n");
		return;
	}
	spin_lock_irqsave(&lowpower_dump_mem.spinlock, flags);

	/*标记记录的组号*/
	if(lowpower_dump_mem.lp_current_legth > lowpower_dump_mem.lp_dump_base_addr_legth){
		lowpower_dump_mem.lp_current_legth = 0;
	}
	*((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = 0x88888888;/* [false alarm]:误报 */
	lowpower_dump_mem.lp_current_legth++;

	/*标记记录的时间戳*/
	if(lowpower_dump_mem.lp_current_legth > lowpower_dump_mem.lp_dump_base_addr_legth){
		lowpower_dump_mem.lp_current_legth = 0;
	}
	*((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = bsp_get_slice_value();/* [false alarm]:误报 */
	lowpower_dump_mem.lp_current_legth++;

	/*读取寄存器的值存入DUMP MEM*/
	do{
        /*记录寄存器地址到内存中*/
        if(lowpower_dump_mem.lp_current_legth > lowpower_dump_mem.lp_dump_base_addr_legth){
            lowpower_dump_mem.lp_current_legth = 0;
        }
        *((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = lowpower_register[i];/* [false alarm]:误报 */
        lowpower_dump_mem.lp_current_legth++;

        /*记录寄存器值到内存中*/
		if(lowpower_dump_mem.lp_current_legth > lowpower_dump_mem.lp_dump_base_addr_legth){
			lowpower_dump_mem.lp_current_legth = 0;
		}
		regvalue = readl(lowpower_register[i]);
        *((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = regvalue;/* [false alarm]:误报 */

	        /*保存寄存器值到数组里*/
		lowpower_current_reg_status[i] = regvalue;
	    lowpower_dump_mem.lp_current_legth++;
		i++;
	}while(lowpower_register[i] != LP_REG_MNTN_NULL);

	/*用于保存那些不能直接读取的寄存器值*/

	spin_unlock_irqrestore(&lowpower_dump_mem.spinlock, flags);
	return;
}

void bsp_dump_lowpower_status(void)
{
	unsigned int i = 0;
	unsigned long flags = 0;
	if(lowpower_register[0] == LP_REG_MNTN_NULL){
		mntn_printf("Has nothing to dump!\n");
		return;
	}

	spin_lock_irqsave(&lowpower_dump_mem.spinlock, flags);
	do{
		mntn_printf("d 0x%x = 0x%x\n",lowpower_register[i], lowpower_current_reg_status[i]);
		i++;
	}while(lowpower_register[i] != LP_REG_MNTN_NULL);

	/*dump 不能直接readl的寄存器值*/

	mntn_printf("print lowpower dump base Address: 0x%x,Total length: 0x%x\n\n",lowpower_dump_mem.lp_dump_base_addr, lowpower_dump_mem.lp_dump_base_addr_legth);
	spin_unlock_irqrestore(&lowpower_dump_mem.spinlock, flags);
	return;
}

void bsp_show_lowpower_status(void)
{
	unsigned long flags = 0;
	unsigned int i = 0;
	unsigned int regvalue = 0;
	if(lowpower_register[0] == LP_REG_MNTN_NULL){
		mntn_printf("Has nothing to save!\n");
		return;
	}
	spin_lock_irqsave(&lowpower_dump_mem.spinlock, flags);
	do{
		regvalue = readl(lowpower_register[i]);
		mntn_printf("d 0x%x = 0x%x\n",lowpower_register[i], regvalue);
		i++;
	}while(lowpower_register[i] != LP_REG_MNTN_NULL);

	/*show不能直接readl的寄存器值*/

	spin_unlock_irqrestore(&lowpower_dump_mem.spinlock, flags);
	return;
}

void* dmem_malloc(unsigned int bytes, unsigned int *refcount)
{
	if(refcount == NULL){
		 mntn_printf("you input refcount = NULL!\n");
		 return NULL;
	}
	*refcount = 0;
	return KMEM_ALIGNED_ALLOC(ROUND_UP(bytes, MMU_PAGE_SIZE_GET()), MMU_PAGE_SIZE_GET());/*lint !e737*/
}

void dmem_free(void* p)
{
	free(p);
}

void* dmem_cache_dma_malloc(unsigned int bytes, unsigned int *refcount)
{
	STATUS state =0;
	unsigned long flags = 0;
	/*lint -save -e502*/
	unsigned int alloc_size = ((bytes +  MMU_PAGE_SIZE_GET() -1) & ~(MMU_PAGE_SIZE_GET() -1));

	void* p = KMEM_ALIGNED_ALLOC(alloc_size, MMU_PAGE_SIZE_GET());
	if(refcount == NULL){
		 mntn_printf("you input refcount = NULL!\n");
		 return NULL;
	}
	if (p)
	{
		local_irq_save(flags);
		*refcount = 0;
		state = VM_STATE_SET(NULL, p, alloc_size, MMU_ATTR_VALID_MSK | MMU_ATTR_SPL_MSK | MMU_ATTR_PROT_MSK | MMU_ATTR_NORMAL_NONCACHEABLE_MSK, MMU_ATTR_VALID | MMU_ATTR_NO_BLOCK | MMU_ATTR_SUP_RW | MMU_ATTR_NORMAL_NONCACHEABLE); /*lint !e119*/
		local_irq_restore(flags);
		if (state != OK )
		{
			free(p);
			return NULL;
		}
	}
	return p;
}

STATUS dmem_cache_dma_free(void* p)
{
	STATUS state = OK;
	unsigned long flags = 0;
	if(NULL != p){

		local_irq_save(flags);
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_CACHE_MSK | MMU_ATTR_PROT_MSK, MMU_ATTR_CACHE_DEFAULT | MMU_ATTR_SUP_RW);/*lint !e119*/
		local_irq_restore(flags);
	}
	free(p);
	return state;
}

STATUS dmem_lock(void* p, unsigned int *refcount)
{
	STATUS state = OK;
	unsigned long flags = 0;
	if(p == NULL){
		mntn_printf("you input p = NULL!\n");
		return ERROR;
	}
	if(refcount == NULL){
		 mntn_printf("you input refcount = NULL!\n");
		 return ERROR;
	}
	local_irq_save(flags);
	if((*refcount) == 1){
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_PROT_MSK, MMU_ATTR_PROT_SUP_READ);/*lint !e119*/
	}
	if((*refcount) == 0){
		mntn_printf("please call dmem_unlock first!\n");
		local_irq_restore(flags);
		return state;
	}
	(*refcount)--;
	local_irq_restore(flags);
	return state;
}

STATUS dmem_unlock(void* p, unsigned int *refcount)
{
	STATUS state = OK;
	unsigned long flags = 0;
	if(p == NULL){
		mntn_printf("you input p = NULL!\n");
		return ERROR;
	}
	if(refcount == NULL){
		 mntn_printf("you input refcount = NULL!\n");
		 return ERROR;
	}
	local_irq_save(flags);
	if((*refcount) == 0){
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_PROT_MSK, MMU_ATTR_SUP_RWX);/*lint !e119*/
	}
	(*refcount)++;
	local_irq_restore(flags);
	return state;
}
void bsp_dump_bus_error_status(void)
{
	unsigned int i = 0;
	if(lowpower_bus_error_register[0] == LP_REG_MNTN_NULL){
		mntn_printf("Has nothing to dump!\n");
		return;
	}
	do{
		mntn_printf("d 0x%x = 0x%x\n", lowpower_bus_error_register[i], lowpower_last_bus_error_register[i]);
		i++;
	}while(lowpower_bus_error_register[i] != LP_REG_MNTN_NULL);

	mntn_printf("print bus error dump base Address: 0x%x,Total length: 0x%x\n\n", lowpower_dump_bus_error.lp_dump_base_addr, lowpower_dump_bus_error.lp_dump_base_addr_legth);
	return;
}

static irqreturn_t bsp_prevent_bus_error_reg_save(int irq, void* dev)
{
	unsigned int i = 0;
	unsigned int regvalue = 0;
#ifdef CONFIG_V7R2_CLK_CRG
	/*清中断*/
	regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
	regvalue |= (unsigned int)0x1 << 7;
	writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
	regvalue &= ~((unsigned int)0x1 << 7);
	writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
#elif defined(CONFIG_K3V3_CLK_CRG) /* CONFIG_K3V3_CLK_CRG */
    /*清中断*/
    regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
    regvalue |= (unsigned int)0x1 << 7;
    writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
    regvalue &= ~((unsigned int)0x1 << 7);
    writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
#endif
	if(lowpower_bus_error_register[0] == LP_REG_MNTN_NULL){
		return IRQ_HANDLED;
	}
	/*标记记录的组号*/
	if(lowpower_dump_bus_error.lp_current_legth > lowpower_dump_bus_error.lp_dump_base_addr_legth){
		lowpower_dump_bus_error.lp_current_legth = 0;
	}
	*((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = 0x88888888;/* [false alarm]:误报 */
	lowpower_dump_bus_error.lp_current_legth++;

	/*标记记录的时间戳*/
	if(lowpower_dump_bus_error.lp_current_legth > lowpower_dump_bus_error.lp_dump_base_addr_legth){
		lowpower_dump_bus_error.lp_current_legth = 0;
	}
	*((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = bsp_get_slice_value();/* [false alarm]:误报 */
	lowpower_dump_bus_error.lp_current_legth++;

	/*读取寄存器的值存入DUMP MEM*/
	do{
        /*记录寄存器地址到内存中*/
        if(lowpower_dump_bus_error.lp_current_legth > lowpower_dump_bus_error.lp_dump_base_addr_legth){
            lowpower_dump_bus_error.lp_current_legth = 0;
        }
        *((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = lowpower_bus_error_register[i]; /* [false alarm]:误报 */
        lowpower_dump_bus_error.lp_current_legth++;

        /*记录寄存器值到内存中*/
		if(lowpower_dump_bus_error.lp_current_legth > lowpower_dump_bus_error.lp_dump_base_addr_legth){
			lowpower_dump_bus_error.lp_current_legth = 0;
		}
		regvalue = readl(lowpower_bus_error_register[i]);
               *((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = regvalue;/* [false alarm]:误报 */

	        /*保存寄存器值到数组里*/
		lowpower_last_bus_error_register[i] = regvalue;
	         lowpower_dump_bus_error.lp_current_legth++;
		i++;
	}while(lowpower_bus_error_register[i] != LP_REG_MNTN_NULL);
	return IRQ_HANDLED;
}

void bsp_lowpower_mntn_init(void)
{
	int ret = 0;
	char* dump_base = NULL;
	unsigned int dump_length = 0;
	NV_DUMP_STRU g_dump_nv_lp;
    unsigned int regvalue = 0;


	/*获取DUMP 低功耗共享内存地址*/
	if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_DRX_CCORE, &dump_base, &dump_length)){
		mntn_printf("*******!get dump buffer failed!******* \n\n");
		return;
	}
    g_lowpower_shared_addr = (unsigned int)dump_base;
    mntn_printf("pm shared mem addr: g_lowpower_shared_addr = 0x %x!\n",g_lowpower_shared_addr);
	ret = (int)bsp_nvm_read(NV_ID_DRV_NV_PWC_SWITCH, (u8 *)(&g_nv_pwc_switch), sizeof(ST_PWC_SWITCH_STRU));
	if(ret!=0)
	{
	    mntn_printf("NV_ID_DRV_NV_PWC_SWITCH:D10B read failure!\n");
	}else{
	    mntn_printf("NV_ID_DRV_NV_PWC_SWITCH:D10B read success!\n");
	}
	bsp_adp_dpm_debug_init();

	/*获取DUMP的内存用于记录低功耗相关寄存器*/
	if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_LP_CCORE, &dump_base, &dump_length)){
		mntn_printf("*******!get dump buffer failed!******* \n\n");
		return;
	}
	lowpower_dump_mem.lp_dump_base_addr = (unsigned int)dump_base;
	lowpower_dump_mem.lp_dump_base_addr_legth = dump_length/4 - 1;
	spin_lock_init(&lowpower_dump_mem.spinlock);
         memset((void *)(lowpower_dump_mem.lp_dump_base_addr),0,(4 * lowpower_dump_mem.lp_dump_base_addr_legth));

	/* EXC DUMP注册 */
	if(bsp_dump_register_hook(DUMP_SAVE_MOD_LP_CCORE, (dump_save_hook)bsp_save_lowpower_status) != BSP_OK)
	{
	    mntn_printf("*****dump register fail!*****\n\n");
	}

	/*根据NV配置芯片防挂死功能是否打开*/
	ret = (int)bsp_nvm_read(NV_ID_DRV_DUMP, (u8 *)(&g_dump_nv_lp), sizeof(NV_DUMP_STRU));
	if(ret!=0)
	{
	    mntn_printf("NV_ID_DRV_DUMP:D111 read failure!\n");
	    return;
	}else{
	    mntn_printf("NV_ID_DRV_DUMP:D111 read success!\n");
	}
	if(g_dump_nv_lp.BusErrorFlagSet){
#ifdef CONFIG_V7R2_CLK_CRG

		/*获取DUMP的内存用于记录总线挂死相关寄存器*/
		if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_LP_CCORE_BUSERROR, &dump_base, &dump_length)){
			mntn_printf("*******!get dump buffer failed!******* \n\n");
			return;
		}
		lowpower_dump_bus_error.lp_dump_base_addr = (unsigned int)dump_base;
		lowpower_dump_bus_error.lp_dump_base_addr_legth = dump_length/4 -1;
		memset((void *)(lowpower_dump_bus_error.lp_dump_base_addr),0,(4 * lowpower_dump_bus_error.lp_dump_base_addr_legth));
        /*清中断*/
        regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
        regvalue |= (unsigned int)0x1 << 7;
        writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
        regvalue &= ~((unsigned int)0x1 << 7);
        writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
		ret = request_irq(INT_LVL_X2H_X2P_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"ccore_bus_error129",NULL);
		if(ret)
		{
			mntn_printf("INT_LVL_X2H_X2P_ERR request_irq failed!\n");
			return;
		}
		ret = request_irq(INT_LVL_AXI_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"ccore_bus_error130",NULL);
		if(ret)
		{
			mntn_printf("INT_LVL_AXI_ERR request_irq failed!\n");
			return;
		}
#elif defined(CONFIG_K3V3_CLK_CRG) /* CONFIG_K3V3_CLK_CRG */

        /*获取DUMP的内存用于记录总线挂死相关寄存器*/
        if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_LP_CCORE_BUSERROR, &dump_base, &dump_length)){
            mntn_printf("*******!get dump buffer failed!******* \n\n");
            return;
        }
        lowpower_dump_bus_error.lp_dump_base_addr = (unsigned int)dump_base;
        lowpower_dump_bus_error.lp_dump_base_addr_legth = dump_length/4 - 1;
        memset((void *)(lowpower_dump_bus_error.lp_dump_base_addr),0,(4 * lowpower_dump_bus_error.lp_dump_base_addr_legth));
        /*清中断*/
        regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
        regvalue |= (unsigned int)0x1 << 7;
        writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
        regvalue &= ~((unsigned int)0x1 << 7);
        writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
        ret = request_irq(INT_LVL_X2H_X2P_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"ccore_bus_error129",NULL);
        if(ret)
        {
            mntn_printf("INT_LVL_X2H_X2P_ERR request_irq failed!\n");
            return;
        }
        ret = request_irq(INT_LVL_AXI_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"ccore_bus_error130",NULL);
        if(ret)
        {
            mntn_printf("INT_LVL_AXI_ERR request_irq failed!\n");
            return;
        }

        /*配置防挂死寄存器配置*/
		writel(0x0, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL68_OFFSET);
		writel(0x69, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL69_OFFSET);
		writel(0x20000, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL103_OFFSET);
		writel(0x0, HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_CTRL105_OFFSET);
#else
		mntn_printf("This version don't support prevent bus error identity!");
#endif
	}
    return;
}
static void bsp_lowpower_mntn_lock_info(void)
{
   unsigned int i = 0;
   unsigned int lockmap = has_wake_lock(0);
   for (i = 0; i < 9 ; i++){
           if (lockmap & ((unsigned int)0x1 << i)){
                    hi6930_lockinfo.lock_lookup[i + 2].status = 1;
                    //vote_printf("awake_time > 2s --lock: %s ,status : 1 \n",ccore_lock[i].name);
           }
           else{
                    hi6930_lockinfo.lock_lookup[i + 2].status = 0;
           }
		  //snprintf(hi6930_lockinfo.lock_lookup[i + 2].name, sizeof(hi6930_lockinfo.lock_lookup[i + 2].name), "%s", ccore_lock[i].name); /*lint !e119*/ /* [false alarm]:误报 */
   }
    bsp_log_bin_ind(0x5220, &hi6930_lockinfo, sizeof(hi6930_lockinfo));
    return;
}

void update_awake_time_stamp(void)
{
        time_stamp = bsp_get_slice_value();
        return;
}

void check_awake_time_limit(void)
{
      unsigned int newslice = 0;
      newslice = bsp_get_slice_value();
      if(get_timer_slice_delta(time_stamp, newslice) > AWAKE_TIME){
             bsp_lowpower_mntn_lock_info();
             update_awake_time_stamp();
      }
      return;
}

/*以下测试函数用于测试dmem_malloc\dmem_free\dmem_cache_dma_malloc\dmem_cache_dma_free\dmem_lock\demem_unlock*/
#if 0
static unsigned int* phead1;
static unsigned int* phead2;
static unsigned int count_flags;

void dmem_init_space(void)
{
	phead1 = (unsigned int*) dmem_cache_dma_malloc(1000, &count_flags);
	return;
}

void test_dmem_case_1(void)
{
	dmem_unlock(phead1, &count_flags);
	mntn_printf("dmem_unlock, count_flags = %d\n",count_flags);
	*phead1=0x1;
	dmem_lock(phead1, &count_flags);
	mntn_printf("dmem_lock, count_flags = %d\n",count_flags);
}
void test_dmem_case_2(void)
{
	dmem_unlock(phead1, &count_flags);
	mntn_printf("dmem_unlock, count_flags = %d\n",count_flags);
	test_dmem_case_1();
	*phead1=0x1;
	dmem_lock(phead1, &count_flags);
	mntn_printf("dmem_lock, count_flags = %d\n",count_flags);
}
#endif
/*lint -restore*/

