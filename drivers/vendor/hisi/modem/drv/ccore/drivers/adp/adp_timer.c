/*lint --e{537}*/
#include <drv_timer.h>
#include <bsp_hardtimer.h>
#include <bsp_memmap.h>
#include <soc_interrupts.h>
#include <bsp_om.h>
#include <osl_module.h>
#include <bsp_bbp.h>
#include "../balong_timer/hardtimer_balong.h"
#include <soc_clk_mdm.h>

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
	s32       args;
	u32       int_num;
	u32       clk;
	u32       mode;
	/*for debug*/
	struct adp_timer_enable_dbg       stEnable  ;
	struct adp_timer_callback_dbg    stCallback;
	struct adp_timer_get_dbg            stGet;
};
/*lint -save -e651*/

struct adp_timer_control  adp_timer_ctrl[TIMER_NUM] = {
	{NULL,0,INT_LVL_TIMER0,HI_TIMER0_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER1,HI_TIMER1_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER2,HI_TIMER2_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER3,HI_TIMER3_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER4,HI_TIMER4_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER5,HI_TIMER5_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER6,HI_TIMER6_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER7,HI_TIMER7_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER8,HI_TIMER8_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER9,HI_TIMER9_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER10,HI_TIMER10_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER11,HI_TIMER11_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER12,HI_TIMER12_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER13,HI_TIMER13_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER14,HI_TIMER14_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER15,HI_TIMER15_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER16,HI_TIMER16_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER17,HI_TIMER17_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER18,HI_TIMER18_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER19,HI_TIMER19_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER20,HI_TIMER20_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER21,HI_TIMER21_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER22,HI_TIMER22_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}},
	{NULL,0,INT_LVL_TIMER23,HI_TIMER23_CLK,0,{0,{0},{0},{0}},{0,{0}},{0,{{0,0}}}}};
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
void timer_callback_stamp_dbg(u32 clkid)
{
	struct adp_timer_callback_dbg *pstCallback_dbg = &adp_timer_ctrl[clkid].stCallback;
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
	timer_id_phy =(u32)para;
	timer_callback_stamp_dbg(timer_id_phy);
	#ifdef K3_TIMER_FEATURE
	/*k3 形态，modem侧8个timer，timer枚举号大于8个的，自动调用k3接口*/
		if(timer_id_phy >= 8)
			ret_value = bsp_hardtimer_int_status_k3(timer_id_phy);
		else
	ret_value = bsp_hardtimer_int_status(timer_id_phy);
	#else
		ret_value = bsp_hardtimer_int_status(timer_id_phy);
	#endif
	if(0x0 !=ret_value)
	{
		#ifdef K3_TIMER_FEATURE
			if(timer_id_phy >= 8)
				bsp_hardtimer_int_clear_k3(timer_id_phy);
			else
				bsp_hardtimer_int_clear(timer_id_phy);
		#else
			bsp_hardtimer_int_clear(timer_id_phy);
			if(adp_timer_ctrl[timer_id_phy].mode == TIMER_ONCE_COUNT)
				(void)bsp_hardtimer_disable(timer_id_phy);
		#endif
		if(NULL!=adp_timer_ctrl[timer_id_phy].routine)
		{
			/*lint -save -e522*/
			adp_timer_ctrl[timer_id_phy].routine(adp_timer_ctrl[timer_id_phy].args);
			/*lint -restore +e522*/
		}
	}
	return IRQ_HANDLED;
}
/*lint -save -e18*/
/*lint -save -e532*/

#ifdef __KERNEL__
/*TIMER_ACPU_OSA_ID:和caixi确认，后续可拿掉*/
void BSP_StartHardTimer(u32 value)
{
    writel(HARD_TIMER_DISABLE,TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    writel(value,TIMER_LOADCOUNT(HI_TIMER_18_REGBASE_ADDR_VIRT));
    writel(HARD_TIMER_ENABLE,TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    return;
}
BSP_VOID BSP_StopHardTimer(BSP_VOID)
{
    writel(HARD_TIMER_DISABLE,TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    return;
}
BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID)
{
    u32 ret = 0;
    ret = readl(TIMER_CONTROLREG(HI_TIMER_18_REGBASE_ADDR_VIRT));
    if((ret&0x1)!=0x1)/*如果计数器没打开*/
    {
        return 0;
    }
    else
    {
        ret = readl(TIMER_CURRENTVALUE(HI_TIMER_18_REGBASE_ADDR_VIRT));
        return ret;
    }
}

void BSP_ClearTimerINT(void)
{
    (void)readl(TIMER_EOI(HI_TIMER_18_REGBASE_ADDR_VIRT));
}
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

u32 omTimerGet(void)
{
    return om_timer_get();
}
u32 omTimerTickGet(void)
{
	return om_timer_tick_get();
}
/*lint -save -e516*/

int DRV_TIMER_START(unsigned int usrClkId,FUNCPTR_1 routine,int argument,unsigned int timerValue,unsigned int mode,unsigned int unitType)
{
    s32 ret = 0;
    struct bsp_hardtimer_control my_hardtimer;
    u32 intLev = adp_timer_ctrl[usrClkId].int_num;
    if(usrClkId==TIMER_CCPU_CPUVIEW_ID)
    {
        if(adp_timer_ctrl[usrClkId].routine)
        {
            osl_free_irq(intLev,(VOIDFUNCPTR)adp_timer_handler,(int)usrClkId);/*lint !e64*/
            adp_timer_ctrl[usrClkId].routine = NULL;
        }
   }
    my_hardtimer.func = NULL;
    my_hardtimer.para = 0;
    my_hardtimer.mode = mode;
    my_hardtimer.timerId = usrClkId;
    adp_timer_ctrl[usrClkId].routine = routine;
    adp_timer_ctrl[usrClkId].args = argument;
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
                /* 对于不大于31us的定时，直接往load寄存器写1 */
                 my_hardtimer.timeout = 1;
            }
        }
    }
    else if(TIMER_UNIT_NONE == unitType)/*直接操作寄存器*/
    {
        my_hardtimer.timeout = timerValue;
    }
    #ifdef K3_TIMER_FEATURE
    if(usrClkId >= 8)
        ret = bsp_hardtimer_alloc_k3(&my_hardtimer);
    else
        ret = bsp_hardtimer_alloc(&my_hardtimer);
    #else
        ret = bsp_hardtimer_alloc(&my_hardtimer);
    #endif
    if(ret)
    {
         return ERROR;
    }
    if(usrClkId==TIMER_CCPU_CPUVIEW_ID)
    {
        ret = request_irq( adp_timer_ctrl[usrClkId].int_num,(irq_handler_t)adp_timer_handler,0,"timer callback",(void*)usrClkId);
        if(ret)
        {
            return ERROR;
        }
    }
    timer_enable_stamp_dbg(usrClkId,my_hardtimer.timeout);
    #ifdef K3_TIMER_FEATURE
    if(usrClkId >= 8)
        (void)bsp_hardtimer_enable_k3(usrClkId);
    else
    (void)bsp_hardtimer_enable(usrClkId);
    #else
    (void)bsp_hardtimer_enable(usrClkId);
    #endif
    return 0;
}
/*lint -restore +e516*/

int DRV_TIMER_STOP(unsigned int usrClkId)
{
	timer_disable_stamp_dbg(usrClkId);
	#ifdef K3_TIMER_FEATURE
	if(usrClkId >= 8)
		return bsp_hardtimer_disable_k3(usrClkId);
	else
		return bsp_hardtimer_disable(usrClkId);
	#else
	return bsp_hardtimer_disable(usrClkId);
    #endif
}

int DRV_TIMER_GET_REST_TIME(unsigned int usrClkId,unsigned int unitType,unsigned int * pRestTime)
{
	u32 ret=0;
	#ifdef K3_TIMER_FEATURE
	if(usrClkId >= 8)
		ret = bsp_get_timer_current_value_k3(usrClkId);
	else
	ret = bsp_get_timer_current_value(usrClkId);
	#else
	ret = bsp_get_timer_current_value(usrClkId);
	#endif
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
void adp_timer_init(void)
{
	s32 ret = 0;
	ret = request_irq(adp_timer_ctrl[TIMER_CCPU_DSP_DRX_PROT_ID].int_num, (irq_handler_t)adp_timer_handler,0,"ccpu osa ",(void*)TIMER_CCPU_DSP_DRX_PROT_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_CCPU_DSP_DRX_PROT_ID request_irq failed\n");
		return;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_DSP_TIMER2_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu dsp2",(void*)TIMER_DSP_TIMER2_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_DSP_TIMER2_ID request_irq failed\n");
		return;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_DSP_TIMER1_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu dsp1",(void*)TIMER_DSP_TIMER1_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_DSP_TIMER1_ID request_irq failed\n");
		return;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_CCPU_OSA_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu osa",(void*)TIMER_CCPU_OSA_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_CCPU_OSA_ID request_irq failed\n");
		return;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_CCPU_DRX1_STABLE_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu drx1",(void*)TIMER_CCPU_DRX1_STABLE_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_CCPU_DRX1_STABLE_ID request_irq failed\n");
		return;
	}
	ret = request_irq(adp_timer_ctrl[TIMER_CCPU_DRX_TIMER_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu drx1",(void*)TIMER_CCPU_DRX_TIMER_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_CCPU_DRX_TIMER_ID request_irq failed\n");
		return;
	}
	#ifndef K3_TIMER_FEATURE
	ret = request_irq(adp_timer_ctrl[TIMER_CCPU_DRX2_STABLE_ID].int_num, (irq_handler_t)adp_timer_handler,0," ccpu drx1",(void*)TIMER_CCPU_DRX2_STABLE_ID);
	if(ret)
	{
		hardtimer_print_error("TIMER_CCPU_DRX2_STABLE_ID request_irq failed\n");
		return;
	}
	#endif
	hardtimer_print_error("adp_timer_init ok\n");
}
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
	/*lint -restore +e504*/
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
/*lint -restore +e18*/
/*lint -restore +e532*/


