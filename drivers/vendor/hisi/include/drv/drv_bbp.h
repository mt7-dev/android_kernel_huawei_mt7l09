/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_BBP_H__
#define __DRV_BBP_H__

#include "drv_comm.h"
#include "drv_dpm.h"

/*用于bbp掉电时， 查询子帧号等接口的返回值*/
#define BBP_POWER_DOWN_BACK 0xffff
#define BBP_TDS_STU_SFN_MASK 0x1fff

#define BSP_BBP_POWER_DOWN BBP_POWER_DOWN_BACK

typedef BSP_VOID (*BSPBBPIntDlTbFunc)(BSP_VOID);
typedef BSP_VOID (*BSPBBPIntTimerFunc)(BSP_VOID);
typedef BSP_VOID (*BSPBBPIntTdsFunc)(BSP_VOID);

/*****************************************************************************
* 函数  : BSP_BBPIntTimerRegCb
* 功能  : 被PS调用，用来向底软注册1ms定时中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc);

/*****************************************************************************
* 函数  : bsp_bbp_dltbintregcb
* 功能  : 被PS调用，用来向底软注册下行数据译码完成中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc);

/*****************************************************************************
* 函 数: BSP_BBPIntTimerClear
* 功 能: 被PS调用，用来清除1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerClear(void);

/*****************************************************************************
* 函 数: BSP_BBPIntTimerEnable
* 功 能: 被PS调用，用来打开1ms定时中断
* 输入 : void
* 输出 : void
* 返回 : int
*****************************************************************************/
BSP_S32 BSP_BBPIntTimerEnable(void);

/*****************************************************************************
* 函 数: BSP_BBPIntTimerDisable
* 功 能: 被PS调用，用来关闭1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerDisable(void);

/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 被PS调用，用来获取系统精确时间
* 输入  : void
* 输出  : u64 *pcurtime
* 返回  : u32
*****************************************************************************/
BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime);

/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 获取BBP定时器的值。用于OAM 时戳
* 输入  : void
* 输出  :
			pulLow32bitValue指针参数不能为空，否则会返回失败。
			pulHigh32bitValue如果为空， 则只返回低32bit的值。
* 返回  : int
*****************************************************************************/
int DRV_GET_BBP_TIMER_VALUE(unsigned int  *pulHigh32bitValue,  unsigned int  *pulLow32bitValue);

/*****************************************************************************
* 函数  : bsp_bbp_getsysframe
* 功能  : get system frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysFrame(BSP_VOID);

/*****************************************************************************
* 函数  : bsp_bbp_getsyssubframe
* 功能  : get system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysSubFrame(BSP_VOID);

/*****************************************************************************
* 函数  : BSP_GetTdsSubFrame
* 功能  : get tds system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetTdsSubFrame(BSP_VOID);
#define DRV_GET_TDS_SUB_FRAME()  BSP_GetTdsSubFrame()

/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_SetTdsSubFrameOffset(BSP_U16 usOffset);
#define DRV_SET_TDS_SUB_FRAME_OFFSET(usOffset)  BSP_SetTdsSubFrameOffset(usOffset)

/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U32 BSP_GetTdsSleepTime(BSP_VOID);
#define DRV_GET_TDS_SLEEP_TIME()  BSP_GetTdsSleepTime()

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFRegCb
* 功能  : regist callback
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntTdsTFRegCb(BSPBBPIntTdsFunc pFunc);

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFEnable 
* 功能  : enable tdstf
* 输入  : void
* 输出  : viod
* 返回  : u32
*****************************************************************************/
BSP_S32 BSP_BBPIntTdsTFEnable(BSP_VOID);

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFDisable
* 功能  : disable tdstf
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntTdsTFDisable(BSP_VOID);

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_ENABLE
* 功能  : enable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_ENABLE(PWC_COMM_MODE_E mode);

/*****************************************************************************
* 函数  : BSP_GetTdsClkSwitchStatus
* 功能  : get tdsclk switch
* 输入  : void
* 输出  : 0表示切换到系统时钟，1表示切换到32k时钟
* 返回  : void
*****************************************************************************/
BSP_S32 BSP_GetTdsClkSwitchStatus(BSP_VOID);
#define DRV_GET_TDS_CLK_STATUS()   BSP_GetTdsClkSwitchStatus()

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_DISABLE
* 功能  : disable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_DISABLE(PWC_COMM_MODE_E mode);

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_CLEAR
* 功能  : clear bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_CLEAR(PWC_COMM_MODE_E mode);

/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
extern BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode);
#define DRV_PWC_GET_MEANSFLAG(enCommMode) BSP_PWC_GetMeansFlag(enCommMode)


/*****************************************************************************
* 函 数 : bbp_timer_int_init
* 功 能 : 挂接BBP子帧中断(T 和L)
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
extern int bsp_bbp_timer_int_init(PWC_COMM_MODE_E mode);
#define DRV_BBP_TIMER_INT_INIT(mode) bsp_bbp_timer_int_init(mode)

/*****************************************************************************
* 函 数 : bbp_dma_int_init
* 功 能 : 挂接bbp dma 中断，并使能
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_dma_int_init(void);
#define DRV_BBP_DMA_INT_INIT() bsp_bbp_dma_int_init()

/*****************************************************************************
* 函 数 : bsp_bbp_dma_int_enable
* 功 能 : 使能bbp dma 中断
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_dma_int_enable(void);
#define DRV_BBP_DMA_INT_ENABLE() bsp_bbp_dma_int_enable()

/*****************************************************************************
* 函 数 : BSP_LPS_Get_Ta
* 功 能 : 被lps调用,查看上行子帧头相对系统子帧头的提前量
* 输 入 : void
* 输 出 : void
* 返 回 : 
* 说 明 : 
*****************************************************************************/
BSP_U16 BSP_LPS_GetTa(void);

/*****************************************************************************
* 函 数 : BSP_BBPIntTimerSwitchStatus
* 功 能 : 用于获取LBBP TIMER中断使能状态
* 输 入 : BSP_BOOL
* 输 出 : void
* 返 回 : 
* 说 明 : 
*****************************************************************************/
BSP_S32 BSP_BBPIntTimerSwitchStatus(BSP_BOOL *pbStat);
#define DRV_BBP_INT_TIMER_SWITCH_STATUS(pbStat) BSP_BBPIntTimerSwitchStatus(pbStat)

#endif

