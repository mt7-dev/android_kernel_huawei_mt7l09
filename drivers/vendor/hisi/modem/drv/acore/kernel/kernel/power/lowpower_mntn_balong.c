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
#include <linux/string.h>
#include <bsp_lowpower_mntn.h>
#include "wakelock_debug.h"
#define  mntn_printf(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_WAKELOCK, "[Lowpower_mntn]: <%s> "fmt"", __FUNCTION__, ##__VA_ARGS__))

/*此全局变量用于记录超过2s未进入休眠的投票时间戳*/
static unsigned int time_stamp = 0;
#define AWAKE_TIME     (32768*2)

struct lock_info hi6930_lockinfo;

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
	LP_REG_MNTN_NULL
};

/*记录需要保存的防挂死寄存器的值*/
unsigned int lowpower_bus_error_register[]={
	LP_REG_MNTN_NULL
};
#else
unsigned int lowpower_register[]={
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
	*((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = 0x88888888;
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
        *((u32 *)lowpower_dump_mem.lp_dump_base_addr + lowpower_dump_mem.lp_current_legth) = lowpower_register[i]; /* [false alarm]:误报 */
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

/*This function for sending struct lock_info to MSP*/
static void bsp_lowpower_mntn_lock_info(void)
{
    int i = 0;
    for (i = 0; i < wakelock_count; i++){
        if(hi6930_wakelock[i].lock){
                int ret = ((unsigned int)hi6930_wakelock[i].lock->flags >> 9) & 0x3;
                if (ret){
                    hi6930_lockinfo.lock_lookup[i + 2].status = 1;
                    //vote_printf("awake_time > 2s --lock: %s ,status : 1 \n ", hi6930_wakelock[i].lock->name);
                }
                else{
                    hi6930_lockinfo.lock_lookup[i + 2].status = 0;
                }
				snprintf(hi6930_lockinfo.lock_lookup[i + 2].name, sizeof(hi6930_lockinfo.lock_lookup[i + 2].name), "%s", hi6930_wakelock[i].lock->name);
        }
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
	regvalue &= ~((unsigned int)0x1 << 7);	/* [false alarm]:误报 */
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
	*((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = bsp_get_slice_value();	/* [false alarm]:误报 */
	lowpower_dump_bus_error.lp_current_legth++;

	/*读取寄存器的值存入DUMP MEM*/
	do{
        /*记录寄存器地址到内存中*/
        if(lowpower_dump_bus_error.lp_current_legth > lowpower_dump_bus_error.lp_dump_base_addr_legth){
            lowpower_dump_bus_error.lp_current_legth = 0;
        }
        *((u32 *)lowpower_dump_bus_error.lp_dump_base_addr + lowpower_dump_bus_error.lp_current_legth) = lowpower_bus_error_register[i];/* [false alarm]:误报 */
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

static int __init bsp_lowpower_mntn_init_acore(void)
{
	char* dump_base = NULL;
	unsigned int dump_length = 0;
	NV_DUMP_STRU g_dump_nv_lp;
	int ret = 0;

	/*获取DUMP的内存用于记录低功耗相关寄存器*/
	if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_LP_ACORE, &dump_base, &dump_length)){
		mntn_printf("*******!get dump buffer failed!******* \n\n");
		return 0;
	}
	lowpower_dump_mem.lp_dump_base_addr = (unsigned int)dump_base;
	lowpower_dump_mem.lp_dump_base_addr_legth = dump_length/4;
	spin_lock_init(&lowpower_dump_mem.spinlock);
         memset((void *)(lowpower_dump_mem.lp_dump_base_addr),0,(4 * lowpower_dump_mem.lp_dump_base_addr_legth));

	/* EXC DUMP注册 */
	if(bsp_dump_register_hook(DUMP_SAVE_MOD_LP_ACORE, (dump_save_hook)bsp_save_lowpower_status) != BSP_OK)
	{
	    mntn_printf("*****dump register fail!*****\n\n");
	}

	/*根据NV配置芯片防挂死功能是否打开*/
	ret = (int)bsp_nvm_read(NV_ID_DRV_DUMP, (u8 *)(&g_dump_nv_lp), sizeof(NV_DUMP_STRU));
	if(ret!=0)
	{
		mntn_printf("NV_ID_DRV_DUMP:D111 read failure!\n");
		return 0;
	}else{
		mntn_printf("NV_ID_DRV_DUMP:D111 read success!\n");
	}
	if(g_dump_nv_lp.BusErrorFlagSet){
#ifdef CONFIG_V7R2_CLK_CRG

		/*获取DUMP的内存用于记录总线挂死相关寄存器*/
		if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_LP_ACORE_BUSERROR, &dump_base, &dump_length)){
			mntn_printf("*******!get dump buffer failed!******* \n\n");
			return 0;
		}
		lowpower_dump_bus_error.lp_dump_base_addr = (unsigned int)dump_base;
		lowpower_dump_bus_error.lp_dump_base_addr_legth = dump_length/4;
		memset((void *)(lowpower_dump_bus_error.lp_dump_base_addr),0,(4 * lowpower_dump_bus_error.lp_dump_base_addr_legth));
		ret = request_irq(INT_LVL_X2H_X2P_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"acore_bus_error129",NULL);
		if(ret)
		{
			mntn_printf("INT_LVL_X2H_X2P_ERR request_irq failed!\n");
			return 0;
		}
		ret = request_irq(INT_LVL_AXI_ERR, (irq_handler_t)bsp_prevent_bus_error_reg_save,0,"acore_bus_error130",NULL);
		if(ret)
		{
			mntn_printf("INT_LVL_AXI_ERR request_irq failed!\n");
			return 0;
		}
#else
		mntn_printf("This version don't support prevent bus error identity!");
#endif
	}
	return 0;
}
module_init(bsp_lowpower_mntn_init_acore);

