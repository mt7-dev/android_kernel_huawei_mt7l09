
/*lint --e{537}*/
#include "drv_comm.h"
#include "bsp_bbp.h"
#include "drv_bbp.h"
#include "product_config.h"

#ifdef CONFIG_BBP_INT
/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 被PS调用，用来获取系统精确时间
* 输入  : void
* 输出  : u64 *pcurtime
* 返回  : u32
*****************************************************************************/
BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime)
{
    return bsp_bbp_getcurtime(pCurTime);
}

/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 获取BBP定时器的值。用于OAM 时戳
* 输入  : void
* 输出  :
			pulLow32bitValue指针参数不能为空，否则会返回失败。
			pulHigh32bitValue如果为空， 则只返回低32bit的值。
* 返回  : int
*****************************************************************************/
int DRV_GET_BBP_TIMER_VALUE(unsigned int  *pulHigh32bitValue,  unsigned int  *pulLow32bitValue)
{
	BSP_U64 CurTime;

	if(BSP_NULL == pulLow32bitValue)
	{
		return -1;
	}

	bsp_bbp_getcurtime(&CurTime);

	if(BSP_NULL != pulHigh32bitValue)
	{
		*pulHigh32bitValue = (CurTime>>32);
	}

	*pulLow32bitValue = CurTime & 0xffffffff;

	return 0;
}
#else
BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime)
{
    return (BSP_U32)0;
}

/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 获取BBP定时器的值。用于OAM 时戳
* 输入  : void
* 输出  :
			pulLow32bitValue指针参数不能为空，否则会返回失败。
			pulHigh32bitValue如果为空， 则只返回低32bit的值。
* 返回  : int
*****************************************************************************/
int DRV_GET_BBP_TIMER_VALUE(unsigned int  *pulHigh32bitValue,  unsigned int  *pulLow32bitValue)
{
	return (int)0;
}
/*****************************************************************************
* 函 数 : bsp_bbp_getcurtime
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
u32  bsp_bbp_getcurtime(u64 *pcurtime)
{
	return 0;
}
#endif
