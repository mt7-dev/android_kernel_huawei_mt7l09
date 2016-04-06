
/*lint --e{537}*/
#include "hi_base.h"
#include "bsp_memmap.h"
#include "hi_bbp_systime.h"

#include "bsp_bbp.h"
//#include "osl_types.h"
/*****************************************************************************
* 函数  : bsp_bbp_getcurtime
* 功能  : 被PS调用，用来获取系统精确时间
* 输入  : void
* 输出  : u64 *pcurtime
* 返回  : u32
*****************************************************************************/
u32 bsp_bbp_getcurtime(unsigned long long *pcurtime)
{
	/*lint -save -e958*/
    unsigned long long  timervalue[4];
	/*lint -restore*/
/*遗留*/
#if 0
    timervalue[0] = get_hi_bbp_systime_abs_timer_l_abs_timer_l();
    timervalue[1] = get_hi_bbp_systime_abs_timer_h_abs_timer_h();
    timervalue[2] = get_hi_bbp_systime_abs_timer_l_abs_timer_l();
    timervalue[3] = get_hi_bbp_systime_abs_timer_h_abs_timer_h();
#endif

    timervalue[0] = (u64)readl((void *)((unsigned long)(HI_BBP_SYSTIME_BASE_ADDR_VIRT)+HI_BBP_SYSTIME_ABS_TIMER_L_OFFSET));
    timervalue[1] = (u64)readl((void *)((unsigned long)(HI_BBP_SYSTIME_BASE_ADDR_VIRT)+HI_BBP_SYSTIME_ABS_TIMER_H_OFFSET));
    timervalue[2] = (u64)readl((void *)((unsigned long)(HI_BBP_SYSTIME_BASE_ADDR_VIRT)+HI_BBP_SYSTIME_ABS_TIMER_L_OFFSET));
    timervalue[3] = (u64)readl((void *)((unsigned long)(HI_BBP_SYSTIME_BASE_ADDR_VIRT)+HI_BBP_SYSTIME_ABS_TIMER_H_OFFSET));

    if(timervalue[2] < timervalue[0])
    {
        (*pcurtime) = ((timervalue[3] - 1) << 32) | timervalue[0];
    }
    else
    {
        (*pcurtime) = (timervalue[1] << 32) | timervalue[0];
    }

    return 0;
}


