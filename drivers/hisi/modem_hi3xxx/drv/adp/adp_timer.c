/*lint --e{537,648 }*/
#include <linux/syscore_ops.h>
#include <drv_timer.h>
#include <bsp_hardtimer.h>
#include <osl_bio.h>
#include <bsp_memmap.h>
#include <soc_clk_app.h>
#include <soc_interrupts.h>
#include <bsp_om.h>
#include <osl_module.h>
#include <bsp_bbp.h>
#include "hardtimer_balong.h"
#define TIMER_32K_US_BOUNDARY           31
/* begin: add for timer debug*/
#define ADP_TIMER_ENABLE_TIMES 30
#define ADP_TIMER_CALLBACK_TIMES 30
#define ADP_TIMER_GET_TIMES 50

struct adp_timer_enable_dbg {
	u32  enable_cnt;
	u32  enable_slice[ADP_TIMER_ENABLE_TIMES];
	u32  disable_slice[ADP_TIMER_ENABLE_TIMES];
	u32  init_time[ADP_TIMER_ENABLE_TIMES];
};

struct adp_timer_callback_dbg{
	u32  callback_slice_cnt;
	u32  callback_slice[ADP_TIMER_CALLBACK_TIMES];
};

struct adp_timer_get_dbg{
	u32  get_cnt;
	u32  get_times[ADP_TIMER_GET_TIMES][2];
};
struct adp_timer_control{
	FUNCPTR_1 routine;
	int       args;
	u32       int_num;
	u32       clk;
	u32       mode;
	/*for debug*/
	struct adp_timer_enable_dbg       stEnable  ;
	struct adp_timer_callback_dbg    stCallback;
	struct adp_timer_get_dbg            stGet;
	FUNCPTR_1 debug_routine;
	int       debug_args;
};
/*lint -save -e651*/
struct adp_timer_control  adp_timer_ctrl[TIMER_NUM] = {
	{NULL,0,INT_LVL_TIMER0,HI_TIMER0_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER1,HI_TIMER1_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER2,HI_TIMER2_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER3,HI_TIMER3_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER4,HI_TIMER4_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER5,HI_TIMER5_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER6,HI_TIMER6_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER7,HI_TIMER7_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER8,HI_TIMER8_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER9,HI_TIMER9_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER10,HI_TIMER10_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER11,HI_TIMER11_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER12,HI_TIMER12_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER13,HI_TIMER13_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER14,HI_TIMER14_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER15,HI_TIMER15_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER16,HI_TIMER16_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER17,HI_TIMER17_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER18,HI_TIMER18_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER19,HI_TIMER19_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER20,HI_TIMER20_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER21,HI_TIMER21_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER22,HI_TIMER22_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0},
	{NULL,0,INT_LVL_TIMER23,HI_TIMER23_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}},NULL,0}};
/*lint -restore*/

void timer_enable_stamp_dbg(u32 clkid, u32 init_time)
{
      struct adp_timer_enable_dbg *pstEnable_dbg = &adp_timer_ctrl[clkid].stEnable;
      pstEnable_dbg->enable_slice[pstEnable_dbg->enable_cnt] = bsp_get_slice_value();
      pstEnable_dbg->init_time[pstEnable_dbg->enable_cnt] = init_time;
}
void timer_disable_stamp_dbg(u32 clkid)
{
	struct adp_timer_enable_dbg *pstEnable_dbg = &adp_timer_ctrl[clkid].stEnable;
	pstEnable_dbg->disable_slice[pstEnable_dbg->enable_cnt] = bsp_get_slice_value();
	pstEnable_dbg->enable_cnt++;
	if (pstEnable_dbg->enable_cnt >=ADP_TIMER_ENABLE_TIMES)
		pstEnable_dbg->enable_cnt = 0;
}

static void timer_callback_stamp_dbg(struct adp_timer_control* pAdp_timer_ctrl)
{
	struct adp_timer_callback_dbg *pstCallback_dbg = &pAdp_timer_ctrl->stCallback;
	pstCallback_dbg->callback_slice[pstCallback_dbg->callback_slice_cnt] = bsp_get_slice_value();
	pstCallback_dbg->callback_slice_cnt++;
	if (pstCallback_dbg->callback_slice_cnt >= ADP_TIMER_CALLBACK_TIMES)
	{
		pstCallback_dbg->callback_slice_cnt = 0;
	}
}

void timer_get_stamp_dbg(u32 clkid, u32 curtime)
{
	struct adp_timer_get_dbg *pstGet_dbg = &adp_timer_ctrl[clkid].stGet;
	pstGet_dbg->get_times[pstGet_dbg->get_cnt][0]= bsp_get_slice_value();
	pstGet_dbg->get_times[pstGet_dbg->get_cnt][1]= curtime;
	pstGet_dbg->get_cnt++;
	if (pstGet_dbg->get_cnt >= ADP_TIMER_GET_TIMES)
	{
	   	pstGet_dbg->get_cnt = 0;
	}
}
OSL_IRQ_FUNC(static irqreturn_t,adp_timer_handler,irq,para)
{
	u32 ret_value = 0;
	u32 timer_id_phy;
	struct adp_timer_control *pAdp_timer_control;

	pAdp_timer_control = (struct adp_timer_control *)para;
	/* performance with divide operation is low, had better save a pointer
	 * in structure adp_timer_control, which pointers to bsp_hardtimer_ctl
         */
	timer_id_phy = ((unsigned long)pAdp_timer_control - (unsigned long)adp_timer_ctrl)/sizeof(struct adp_timer_control);

	timer_callback_stamp_dbg(pAdp_timer_control);
	ret_value = bsp_hardtimer_int_status(timer_id_phy);
	if(0x0 !=ret_value)
	{
		bsp_hardtimer_int_clear(timer_id_phy);
		if(pAdp_timer_control->mode == TIMER_ONCE_COUNT)
		(void)bsp_hardtimer_disable(timer_id_phy);
		if(NULL!=pAdp_timer_control->routine)
		{
			pAdp_timer_control->routine(adp_timer_ctrl[timer_id_phy].args);
		}
	}

	return IRQ_HANDLED;
}

#ifdef __KERNEL__
#if 0
/*TIMER_ACPU_OSA_ID:和caixi确认，后续可拿掉*/
void BSP_StartHardTimer(unsigned int value)
{
    writel(HARD_TIMER_DISABLE, (volatile void *)TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    writel(value,(volatile void *)TIMER_LOADCOUNT(HI_TIMER_18_REGBASE_ADDR_VIRT));/*lint !e516 */
    writel(HARD_TIMER_ENABLE,(volatile void *)TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    return;
}
BSP_VOID BSP_StopHardTimer(BSP_VOID)
{
    writel(HARD_TIMER_DISABLE,(volatile void *)TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    return;
}
/*lint --e{732}*/
BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID)
{
    u32 ret = 0;
    ret = readl((const volatile void *)TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));/*lint !e746*/
    if((ret&0x1)!=0x1)/*如果计数器没打开*/
    {
        return 0;
    }
    else
    {
        ret = readl((const volatile void *)TIMER_CURRENTVALUE(HI_TIMER_18_REGBASE_ADDR_VIRT));
        return ret;
    }
}

void BSP_ClearTimerINT(void)
{
    (void)readl((const volatile void *)TIMER_EOI(HI_TIMER_18_REGBASE_ADDR_VIRT));
}
#endif
#elif defined(__VXWORKS__)
/*TIMER_ACPU_OSA_ID:和caixi确认，后续可拿掉*/
void BSP_StartHardTimer(u32 value)
{
    return;
}
BSP_VOID BSP_StopHardTimer(BSP_VOID)
{
    return;
}
BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID)
{
    return 0;
}

void BSP_ClearTimerINT(void)
{
    return;
}
#endif

unsigned int omTimerGet(void)
{
    return om_timer_get();
}
/*lint --e{18} */
u32 omTimerTickGet(void)
{
	return om_timer_tick_get();
}

int DRV_TIMER_START(unsigned int usrClkId,FUNCPTR_1 routine,int arg,unsigned int timerValue,unsigned int mode,unsigned int unitType)
{
    s32 ret = 0;
    struct bsp_hardtimer_control my_hardtimer;
    u32 intLev = adp_timer_ctrl[usrClkId].int_num;
    if(TIMER_ACPU_CPUVIEW_ID==usrClkId)
   {
       if(adp_timer_ctrl[usrClkId].routine)
       {
          osl_free_irq(intLev,adp_timer_handler, &adp_timer_ctrl[usrClkId]);
          adp_timer_ctrl[usrClkId].routine = NULL;
       }
   }
    my_hardtimer.func = NULL;
    my_hardtimer.para = 0;
    my_hardtimer.mode = mode;
    my_hardtimer.timerId = usrClkId;
    adp_timer_ctrl[usrClkId].routine = routine;
    adp_timer_ctrl[usrClkId].args = arg;
	adp_timer_ctrl[usrClkId].mode = mode;
    if (TIMER_UNIT_MS==unitType)/*ms*/
    {
        if(HI_TCXO_CLK == adp_timer_ctrl[usrClkId].clk)
        {
            my_hardtimer.timeout = adp_timer_ctrl[usrClkId].clk*timerValue/Second_To_Millisecond;
        }
        else
        {
        my_hardtimer.timeout = adp_timer_ctrl[usrClkId].clk/Second_To_Millisecond*timerValue;
    }
    }
    else if (TIMER_UNIT_US==unitType)/*us*/
    {
        if(HI_TIMER_CLK == adp_timer_ctrl[usrClkId].clk)
        {
            my_hardtimer.timeout = adp_timer_ctrl[usrClkId].clk/Second_To_Millisecond*timerValue/Second_To_Millisecond;
        }
        else
        {
            if (timerValue > TIMER_32K_US_BOUNDARY)
            {
                 my_hardtimer.timeout = (timerValue * adp_timer_ctrl[usrClkId].clk) / Second_To_Millisecond / Second_To_Millisecond;
            }
            else
            {
                 my_hardtimer.timeout = 1;
            }
        }
    }
    else if(TIMER_UNIT_NONE == unitType)/*直接操作寄存器*/
    {
        my_hardtimer.timeout = timerValue;
    }
    ret = bsp_hardtimer_alloc(&my_hardtimer);
    if(ret)
    {
        return ERROR;
    }
    if(TIMER_ACPU_CPUVIEW_ID==usrClkId)
    {
        ret = request_irq( adp_timer_ctrl[usrClkId].int_num,(irq_handler_t)adp_timer_handler,0,"timer callback", &adp_timer_ctrl[usrClkId]);
        if(ret)
        {
            return ERROR;
        }
    }
    timer_enable_stamp_dbg(usrClkId,my_hardtimer.timeout);
    (void)bsp_hardtimer_enable(usrClkId);
    return 0;
}
int DRV_TIMER_STOP(unsigned int usrClkId)
{
	timer_disable_stamp_dbg(usrClkId);
    return bsp_hardtimer_disable(usrClkId);
}

int DRV_TIMER_GET_REST_TIME(unsigned int usrClkId,unsigned int unitType,unsigned int * pRestTime)
{
    u32 ret=0;
    ret = bsp_get_timer_current_value(usrClkId);
    if(TIMER_UNIT_MS==unitType)/*ms*/
    {
        *pRestTime=ret/(adp_timer_ctrl[usrClkId].clk/Second_To_Millisecond);
    }
    else if(TIMER_UNIT_US==unitType)
    {
       *pRestTime=(ret*Second_To_Millisecond)/(adp_timer_ctrl[usrClkId].clk/Second_To_Millisecond);
    }
    else if(TIMER_UNIT_NONE==unitType)
    {
       *pRestTime = ret;
    }
    timer_get_stamp_dbg(usrClkId,ret);
    return 0;

}
BSP_VOID DRV_TIMER_DEBUG_REGISTER(unsigned int timer_id,FUNCPTR_1 routine, int arg)
{
	adp_timer_ctrl[timer_id].debug_routine = routine;
	adp_timer_ctrl[timer_id].debug_args = arg;
}

static void get_timer_int_stat(void)
{
	unsigned int i = 0;
	for(i = 0;i <TIMER_NUM;i++){
		if(adp_timer_ctrl[i].debug_routine)
		{
			if(bsp_hardtimer_int_status(i))
				adp_timer_ctrl[i].debug_routine(adp_timer_ctrl[i].debug_args);
		}
	}
}

static struct syscore_ops debug_timer_dpm_ops = {
	.resume = get_timer_int_stat,
	};

static int adp_timer_init(void)
{
	int ret = 0;
	ret = request_irq(adp_timer_ctrl[TIMER_ACPU_OSA_ID].int_num,adp_timer_handler,IRQF_NO_SUSPEND," acpu osa", &adp_timer_ctrl[TIMER_ACPU_OSA_ID]);
	if(ret)
	{
		hardtimer_print_error("TIMER_ACPU_OSA_ID request_irq failed\n");
		return -1;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_ACPU_OM_TCXO_ID].int_num,adp_timer_handler,0," acpu osa", &adp_timer_ctrl[TIMER_ACPU_OM_TCXO_ID]);
	if(ret)
	{
		hardtimer_print_error("TIMER_ACPU_OSA_ID request_irq failed\n");
		return -1;
	}
	register_syscore_ops(&debug_timer_dpm_ops);
	hardtimer_print_error("adp_timer_init ok\n");
	return 0;
}
core_initcall(adp_timer_init);

/*v7r2中,时间戳返回系统控制器定时器，在p531 asic中，时间戳由timer5提供*/
unsigned int BSP_GetSliceValue(void)
{
    return bsp_get_slice_value();
}
unsigned int bsp_get_32k_ms(void)
{
	u64 curtime;
	(void)bsp_bbp_getcurtime(&curtime);
	/*lint -save -e504*/
	return (u32)(curtime*1000>>15);
	/*lint -restore  +e504*/
}
/*****************************************************************************
 函 数 名  : BSP_32K_GetTick
 功能描述  : 32K时钟对应Tick查询接口
 输入参数  : None
 输出参数  : None
 返 回 值  : 32K时钟对应Tick值
*****************************************************************************/
 BSP_U32 BSP_32K_GetTick( BSP_VOID )
 {
	return 0;
 }

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: 设置timer4作为唤醒源
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
 VOID BSP_PWC_SetTimer4WakeSrc(VOID)
 {
 }
/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: 设置timer4不作为唤醒源
 Input      :
            :
 Return     : void
 Other      :
*****************************************************************************/
 VOID BSP_PWC_DelTimer4WakeSrc(VOID)
 {
 }
/*lint -save -e19*/

EXPORT_SYMBOL(BSP_GetSliceValue);
/*lint -restore +e19*/


