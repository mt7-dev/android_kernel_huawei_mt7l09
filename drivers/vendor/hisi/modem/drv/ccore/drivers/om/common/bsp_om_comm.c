
/*lint -save -e537 -e451*/
#include <stdio.h>
#include <stdarg.h>
#include <logLib.h>
#include "osl_types.h"
#include "drv_comm.h"
#include "bsp_memmap.h"
#include "bsp_om_api.h"
#include "bsp_hardtimer.h"
#include <bsp_om.h>
/*lint -restore*/
#define OM_TIMER_STAMP_BASE HI_TIMER_10_REGBASE_ADDR		/*timer10*/

#if 0
u32 om_timer_start(void)
{
	u32 readValueTmp;
	u32 u32Times = 100;
	u32 i = 0;

	/* Set up in periodic mode */
	BSP_REG_WRITE(OM_TIMER_STAMP_BASE,0x8, 0x0);

	/* Load Timer Reload value into Timer registers */
	BSP_REG_WRITE (OM_TIMER_STAMP_BASE,0x0, 0xffffffff);

	/* 查询计数器使能是否生效 */
	do
	{
		BSP_REG_READ(OM_TIMER_STAMP_BASE,0x8, readValueTmp);
		readValueTmp = readValueTmp&0x10;
		i++;
	}while((!readValueTmp)&&(i<u32Times));

	BSP_REG_WRITE(OM_TIMER_STAMP_BASE,0x8, 0x5);

	return OK;
}
#endif
/*lint -save -e18*/
u32 om_timer_get(void)
{
	return bsp_get_slice_value();
}
/*lint -restore +e18*/
u32 om_timer_tick_get(void)
{
    u32 omTimerValue;
    /*lint -save -e958*/
	u64 omTick;

	omTimerValue = bsp_get_slice_value();
	omTick=((unsigned long long)omTimerValue*100)>>(15);
    /*lint -restore*/
	return (UINT32)omTick;
}

/*****************************************************************************
* 函 数 名  : bsp_int_lock_enter
*
* 功能描述  :用于锁中断统计
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : 锁中断后获得的key
*****************************************************************************/
void bsp_int_lock_enter(void)
{
    bsp_dump_int_lock((bsp_module_e)0);
}

/*****************************************************************************
* 函 数 名  : bsp_int_lock_out
*
* 功能描述  : 用于锁中断统计
*
* 输入参数  : 锁中断后获得的key
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_int_lock_out(void)
{
    bsp_dump_int_unlock((bsp_module_e)0);
}


void bsp_om_debug( u32 level, char *fmt,...)
{
        va_list arglist = (va_list)NULL;
        s32 _args[6];
        u32 i;
        va_start(arglist, fmt);
        for(i = 0; i < 6; i++)
        {
            _args[i] = va_arg(arglist, s32);
        }
        va_end(arglist);
        logMsg((char*)fmt, _args[0], _args[1],_args[2],_args[3],_args[4],_args[5]);
        return;
}


