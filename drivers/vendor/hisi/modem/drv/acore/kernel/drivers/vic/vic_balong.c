


#include "osl_types.h"
#include "osl_bio.h"
#include "osl_irq.h"
#include "osl_module.h"
#include "hi_vic.h"
#include "vic_balong.h"
#include "bsp_vic.h"
#include "drv_version.h"

#ifdef __KERNEL__
#define VIC_PRINT(a)   printk(a)
#else
#define VIC_PRINT(a)   logMsg(a, 0, 0, 0, 0, 0, 0)
#endif


bsp_vic_st gstVicInfo[BALONG_VIC_NUM];
u32 gGuVicFlag = 0;

void gu_vic_disable(u32 vicnum, int level);


OSL_IRQ_FUNC(irqreturn_t, bsp_vic_inthandler, intLvl, dev)
{
	int newLevel = 0;
	u32 isr = 0;
	bsp_vic_st* pVic = (bsp_vic_st*)dev;

	isr = readl(pVic->vic_base_addr + pVic->vic_final_status);

	if(isr == 0)
	{
		return IRQ_NONE;
	}

	for(;;)
	{
		if (0x1 == (isr & 0x1))
		{
			break;
		}
		isr = isr >> 1;
		newLevel++;
	}

	if(NULL != pVic->victable[newLevel].routine)
	{
		pVic->victable[newLevel].routine(pVic->victable[newLevel].arg);
	}

	return IRQ_HANDLED;
}


s32 __init bsp_vic_init(void)
{
    u32 i, j;

	if(BOARD_TYPE_GUL == bsp_get_board_mode_type())
	{
		gGuVicFlag = 1;
	}


		
	gstVicInfo[0].vic_base_addr = BSP_VIC_BASE;
	gstVicInfo[0].vic_irq_num = INT_LVL_VIC;
	gstVicInfo[0].vic_inten = 0x0;
	gstVicInfo[0].vic_intdisable = 0x0;
	gstVicInfo[0].vic_final_status = 0x30;
	
	
	if(1 == gGuVicFlag)
	{
	    gstVicInfo[1].vic_base_addr = BSP_GU_VIC_BASE;
	    gstVicInfo[1].vic_irq_num = INT_GU_LVL_VIC;
		gstVicInfo[1].vic_inten = 0x10;
		gstVicInfo[1].vic_intdisable = 0x14;
		gstVicInfo[1].vic_final_status = 0x0;		
	}
	
	for(j = 0; j <= gGuVicFlag; j++)
	{
		for(i=0; i<INT_VIC_MAX; i++)
		{
			gstVicInfo[j].victable[i].routine = NULL;
			gstVicInfo[j].victable[i].arg = 0;
		}
		writel(0x0, gstVicInfo[j].vic_base_addr + gstVicInfo[j].vic_inten);

		if(request_irq(gstVicInfo[j].vic_irq_num, (irq_handler_t)bsp_vic_inthandler, 0, "vic", &gstVicInfo[j]) != OK)
		{
			VIC_PRINT("bsp_vic_init error \n");
			return ERROR;
		}
	}

	VIC_PRINT("bsp_vic_init ok \n");
	return OK;
}


s32 bsp_vic_enable(int level)
{
    u32 ulValue = 0;
    int key = 0;
	u32 vicnum = 0;

	if((level < 0) || (level >= (INT_GU_VIC_BASE_LVL + gGuVicFlag * INT_VIC_MAX)))
	{
		VIC_PRINT("bsp_vic_enable input error \n");
		return 	ERROR;
	}
	if(level >= INT_GU_VIC_BASE_LVL)
	{
		vicnum = 1;
		level -= INT_GU_VIC_BASE_LVL;
	}

	local_irq_save(key);

	ulValue = readl(gstVicInfo[vicnum].vic_base_addr + gstVicInfo[vicnum].vic_inten);
    ulValue |= (1<<level);
	writel(ulValue, gstVicInfo[vicnum].vic_base_addr + gstVicInfo[vicnum].vic_inten);

	local_irq_restore(key);

    return OK;
}


s32 bsp_vic_disable (int level)
{

    u32 ulValue = 0;
    int key = 0;
	u32 vicnum = 0;
	
	if((level < 0) || (level >= (INT_GU_VIC_BASE_LVL + gGuVicFlag * INT_VIC_MAX)))
	{
		VIC_PRINT("bsp_vic_disable input error \n");
		return 	ERROR;
	}
	if(level >= INT_GU_VIC_BASE_LVL)
	{
		vicnum = 1;
		level -= INT_GU_VIC_BASE_LVL;
		gu_vic_disable(vicnum, level);
		return OK;	
	}

	local_irq_save(key);

	ulValue = readl(gstVicInfo[vicnum].vic_base_addr + gstVicInfo[vicnum].vic_intdisable);
    ulValue &= ~(1<<level);
	writel(ulValue, gstVicInfo[vicnum].vic_base_addr + gstVicInfo[vicnum].vic_intdisable);

	local_irq_restore(key);

    return OK;	
}

void gu_vic_disable(u32 vicnum, int level)
{
    u32 ulValue = 0;
    int key = 0;
	
	local_irq_save(key);

    ulValue = (1<<level);
	writel(ulValue, gstVicInfo[vicnum].vic_base_addr + gstVicInfo[vicnum].vic_intdisable);
}

s32 bsp_vic_connect(int level, vicfuncptr routine, s32 parameter)
{
	u32 vicnum = 0;

	if((level < 0) || (level >= (INT_GU_VIC_BASE_LVL + gGuVicFlag * INT_VIC_MAX)))
	{
		VIC_PRINT("bsp_vic_connect input error \n");
		return 	ERROR;
	}
	if(level >= INT_GU_VIC_BASE_LVL)
	{
		vicnum = 1;
		level -= INT_GU_VIC_BASE_LVL;
	}

    gstVicInfo[vicnum].victable[level].routine = routine;
    gstVicInfo[vicnum].victable[level].arg = parameter;

	return OK;
}



s32 bsp_vic_disconnect(int level)
{    
	u32 vicnum = 0;
	
	if((level < 0) || (level >= (INT_GU_VIC_BASE_LVL + gGuVicFlag * INT_VIC_MAX)))
	{
		VIC_PRINT("bsp_vic_disconnect input error \n");
		return 	ERROR;
	}
	if(level >= INT_GU_VIC_BASE_LVL)
	{
		vicnum = 1;
		level -= INT_GU_VIC_BASE_LVL;
	}

    gstVicInfo[vicnum].victable[level].routine = NULL;
    gstVicInfo[vicnum].victable[level].arg = 0;

	return OK;
}

module_init(bsp_vic_init);
EXPORT_SYMBOL(bsp_vic_enable);
EXPORT_SYMBOL(bsp_vic_disable);
EXPORT_SYMBOL(bsp_vic_connect);
EXPORT_SYMBOL(bsp_vic_disconnect);

