
/*lint --e{537}*/
#include "drv_comm.h"
#include "bsp_bbp.h"
#include "bsp_version.h"
#include "bsp_memmap.h"
#include "osl_bio.h"
#include "product_config.h"
#define BBP_CTU_MEAS_FLAG_L_RD  0x640
#define BBP_CTU_MEAS_FLAG_W_RD  0x644
#define BBP_CTU_MEAS_FLAG_T_RD  0x648
#define BBP_CTU_MEAS_FLAG_GM_RD 0x64c
#define BBP_CTU_MEAS_FLAG_GS_RD 0x650

#ifdef CONFIG_BBP_INT
/*****************************************************************************
* 函数  : BSP_BBPIntTimerRegCb
* 功能  : 被PS调用，用来向底软注册1ms定时中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc)
{
    bsp_bbp_timerintregcb(pFunc);
}

/*****************************************************************************
* 函数  : bsp_bbp_dltbintregcb
* 功能  : 被PS调用，用来向底软注册下行数据译码完成中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc)
{
    bsp_bbp_dltbintregcb(pFunc);
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerClear
* 功 能: 被PS调用，用来清除1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerClear(void)
{
    bsp_bbp_timerintclear();
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerEnable
* 功 能: 被PS调用，用来打开1ms定时中断
* 输入 : void
* 输出 : void
* 返回 : int
*****************************************************************************/
BSP_S32 BSP_BBPIntTimerEnable(void)
{
    return bsp_bbp_timerintenable();
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerDisable
* 功 能: 被PS调用，用来关闭1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerDisable(void)
{
    bsp_bbp_timerintdisable();
}

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
/*****************************************************************************
* 函数  : BSP_GetSysFrame
* 功能  : get system frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysFrame(BSP_VOID)
{
    return bsp_bbp_getsysframe();
}

/*****************************************************************************
* 函数  : BSP_GetSysSubFrame
* 功能  : get system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysSubFrame(BSP_VOID)
{
    return bsp_bbp_getsyssubframe();
}

/*****************************************************************************
* 函数  : BSP_GetTdsSubFrame
* 功能  : get tds system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetTdsSubFrame(BSP_VOID)
{
	return bsp_bbp_get_tds_subframe();
}
/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_SetTdsSubFrameOffset(BSP_U16 usOffset)
{
	return bsp_bbp_set_tds_subframoffset(usOffset);
}
/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U32 BSP_GetTdsSleepTime(BSP_VOID)
{
	return bsp_bbp_get_tds_sleep_time();
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFRegCb
* 功能  : regist callback
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

BSP_VOID BSP_BBPIntTdsTFRegCb(BSPBBPIntTdsFunc pFunc)
{
    bsp_bbp_tdstf_regcb(pFunc);
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFEnable
* 功能  : enable tdstf
* 输入  : void
* 输出  : viod
* 返回  : u32
*****************************************************************************/

BSP_S32 BSP_BBPIntTdsTFEnable(BSP_VOID)
{
    return bsp_bbp_tdstf_enable();
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFDisable
* 功能  : disable tdstf
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

BSP_VOID BSP_BBPIntTdsTFDisable(BSP_VOID)
{
    bsp_bbp_tdstf_disable();
}
/*****************************************************************************
* 函数  : BSP_GetTdsClkSwitchStatus
* 功能  : get tdsclk switch
* 输入  : void
* 输出  : 0表示切换到系统时钟，1表示切换到32k时钟
* 返回  : void
*****************************************************************************/
BSP_S32 BSP_GetTdsClkSwitchStatus(BSP_VOID)
{
	return bsp_bbp_get_tdsclk_switch();
}

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_ENABLE
* 功能  : enable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_ENABLE(PWC_COMM_MODE_E mode)
{
    bbp_wakeup_int_enable(mode);
    return;
}

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_DISABLE
* 功能  : disable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_DISABLE(PWC_COMM_MODE_E mode)
{
    bbp_wakeup_int_disable(mode);
    return;
}

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_CLEAR
* 功能  : clear bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_CLEAR(PWC_COMM_MODE_E mode)
{
    bbp_wakeup_int_clear(mode);
    return;
}

/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
 BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode)
 {

	u32 pwc_meas_offset=0;
	u32 meas_value=0xffffffff;

	switch(enCommMode){

	case PWC_COMM_MODE_WCDMA:
		pwc_meas_offset = BBP_CTU_MEAS_FLAG_W_RD;
		break;
	case PWC_COMM_MODE_GSM:
		pwc_meas_offset = BBP_CTU_MEAS_FLAG_GM_RD;
		break;
	case PWC_COMM_MODE_LTE:
		pwc_meas_offset = BBP_CTU_MEAS_FLAG_L_RD;
		break;
	case PWC_COMM_MODE_TDS:
		pwc_meas_offset = BBP_CTU_MEAS_FLAG_T_RD;
		break;
	default:
		return -1;
	}

	meas_value = readl(HI_CTU_BASE_ADDR+pwc_meas_offset);
	return (BSP_S32)meas_value;
 }
/*****************************************************************************
* 函 数 : BSP_LPS_Get_Ta
* 功 能 : 被lps调用,查看上行子帧头相对系统子帧头的提前量
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
BSP_U16 BSP_LPS_GetTa(void)
{
	return bsp_bbp_lps_get_ta();
}
/*****************************************************************************
* 函 数 : BSP_BBP_GET_WAKEUP_TIME
* 功 能 : 获取dsp唤醒时间
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/

BSP_U32 BSP_BBP_GET_WAKEUP_TIME(PWC_COMM_MODE_E mode)
{
    return bbp_get_wakeup_time(mode);
}

#else
BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc)
{
    return ;
}

/*****************************************************************************
* 函数  : bsp_bbp_dltbintregcb
* 功能  : 被PS调用，用来向底软注册下行数据译码完成中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc)
{
    return ;
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerClear
* 功 能: 被PS调用，用来清除1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerClear(void)
{
    return ;
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerEnable
* 功 能: 被PS调用，用来打开1ms定时中断
* 输入 : void
* 输出 : void
* 返回 : int
*****************************************************************************/
BSP_S32 BSP_BBPIntTimerEnable(void)
{
    return (BSP_S32)-1;
}

/*****************************************************************************
* 函 数: BSP_BBPIntTimerDisable
* 功 能: 被PS调用，用来关闭1ms定时中断
* 输 入: void
* 输 出: void
* 返 回: void
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerDisable(void)
{
    return ;
}

/*****************************************************************************
* 函数  : BSP_BBPGetCurTime
* 功能  : 被PS调用，用来获取系统精确时间
* 输入  : void
* 输出  : u64 *pcurtime
* 返回  : u32
*****************************************************************************/
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
* 函数  : BSP_GetSysFrame
* 功能  : get system frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysFrame(BSP_VOID)
{
    return (BSP_U32)0;
}

/*****************************************************************************
* 函数  : BSP_GetSysSubFrame
* 功能  : get system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetSysSubFrame(BSP_VOID)
{
    return (BSP_U32)0;
}

/*****************************************************************************
* 函数  : BSP_GetTdsSubFrame
* 功能  : get tds system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_GetTdsSubFrame(BSP_VOID)
{
	return 0;
}
/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U16 BSP_SetTdsSubFrameOffset(BSP_U16 usOffset)
{
	return 0;
}
/*****************************************************************************
* 函数  : BSP_SetTdsSubFrameOffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
BSP_U32 BSP_GetTdsSleepTime(BSP_VOID)
{
	return 0;
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFRegCb
* 功能  : regist callback
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

BSP_VOID BSP_BBPIntTdsTFRegCb(BSPBBPIntTdsFunc pFunc)
{
    return ;
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFEnable
* 功能  : enable tdstf
* 输入  : void
* 输出  : viod
* 返回  : u32
*****************************************************************************/

BSP_S32 BSP_BBPIntTdsTFEnable(BSP_VOID)
{
    return (BSP_S32)0;
}

/*****************************************************************************
* 函数  : BSP_BBPIntTdsTFDisable
* 功能  : disable tdstf
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

BSP_VOID BSP_BBPIntTdsTFDisable(BSP_VOID)
{
    return ;
}
/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_ENABLE
* 功能  : enable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_ENABLE(PWC_COMM_MODE_E mode)
{

    return ;
}

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_DISABLE
* 功能  : disable bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_DISABLE(PWC_COMM_MODE_E mode)
{
    return ;
}

/*****************************************************************************
* 函数  : DRV_BBPWAKE_INT_CLEAR
* 功能  : clear bbp wakeup int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
BSP_VOID DRV_BBPWAKE_INT_CLEAR(PWC_COMM_MODE_E mode)
{
    return ;
}

/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
 BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode)
 {
	return (BSP_S32)0;
 }

/*****************************************************************************
* 函 数 : bsp_bbp_get_wakeup_time
* 功 能 : 被低功耗调用，用来查询睡眠超时时刻
* 输 入 : void
* 输 出 : void
* 返 回 : bbp睡眠剩余时间，单位为32.768KHz时钟计数
* 说 明 : 低功耗不关心哪个模还有多长时间被告唤醒，
		    只需要返回最小的时间即可
*****************************************************************************/
u32 bsp_bbp_get_wakeup_time(void)
{
	return 0xffffffff;
}

/*****************************************************************************
* 函 数 : bbp_pwrctrl_irm_poweron
* 功 能 : 被低功耗调用，用来给bbp公共部分上电
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
int bbp_pwrctrl_irm_poweron(void)
{
	return 0;
}

/*****************************************************************************
* 函 数 : bbp_pwrctrl_irm_poweroff
* 功 能 : 被低功耗调用，用来给bbp公共部分下电
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
int bbp_pwrctrl_irm_poweroff()
{
	return 0;
}
PWC_COMM_STATUS_E bbp_pwrctrl_irm_status(void)
{
    return 0x10;
}

/*****************************************************************************
* 函 数 : bbp_int_init
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
s32 bbp_int_init(void)
{
	return 0;
}

/*****************************************************************************
* 函 数 : bsp_bbp_timerintregcb
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_timerintregcb(BSPBBPIntTimerFunc pfunc)
{
	return ;
}

/*****************************************************************************
* 函 数 : bsp_bbp_dltbintregcb
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_dltbintregcb(BSPBBPIntDlTbFunc pfunc)
{
	return ;
}

/*****************************************************************************
* 函 数 : bsp_bbp_timerintclear
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_timerintclear(void)
{
	return ;
}
/*****************************************************************************
* 函 数 : bsp_bbp_timerintenable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
int  bsp_bbp_timerintenable(void)
{
	return 0;
}
/*****************************************************************************
* 函 数 : bsp_bbp_timerintdisable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_timerintdisable(void)
{
	return ;
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

/*****************************************************************************
* 函 数 : bsp_bbp_getsysframe
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
u16  bsp_bbp_getsysframe(void)
{
	return 0;
}
/*****************************************************************************
* 函 数 : bsp_bbp_timerintdisable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
u16  bsp_bbp_getsyssubframe(void)
{
	return 0;
}

/*****************************************************************************
* 函 数 : bsp_bbp_tdstf_regcb
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_tdstf_regcb(BSPBBPIntTdsFunc pFunc)
{
	return ;
}
/*****************************************************************************
* 函 数 : bsp_bbp_tdstf_enable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
int  bsp_bbp_tdstf_enable()
{
	return ;
}

/*****************************************************************************
* 函 数 : bsp_bbp_tdstf_disable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bsp_bbp_tdstf_disable()
{
	return ;
}
/*****************************************************************************
* 函数  : BSP_GetTdsClkSwitchStatus
* 功能  : get tdsclk switch
* 输入  : void
* 输出  : 0表示切换到系统时钟，1表示切换到32k时钟
* 返回  : void
*****************************************************************************/
BSP_S32 BSP_GetTdsClkSwitchStatus(BSP_VOID)
{
	return -1;
}

/*****************************************************************************
* 函 数 : bsp_bbp_tdstf_disable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bbp_wakeup_int_enable(PWC_COMM_MODE_E mode)
{
	return ;
}
/*****************************************************************************
* 函 数 : bbp_wakeup_int_disable
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bbp_wakeup_int_disable(PWC_COMM_MODE_E mode)
{
	return ;
}
/*****************************************************************************
* 函 数 : bbp_wakeup_int_clear
* 功 能 : bbp 初始化
* 输 入 : void
* 输 出 : void
* 返 回 : 模块裁剪时，打桩
* 说 明 :
*****************************************************************************/
void bbp_wakeup_int_clear(PWC_COMM_MODE_E mode)
{
	return ;
}
/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweron
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweron(void)
{

	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweroff(void)
{
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g1_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g1bbp_status(void)
{
    return PWC_COMM_STATUS_BUTT;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweron
* 功 能 : 被低功耗调用，用来给G2 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweron(void)
{
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweroff(void)
{
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g2_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g2bbp_status(void)
{
    return PWC_COMM_STATUS_BUTT;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweron
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweron(void)
{
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweroff(void)
{

	return BSP_OK;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_wbbp_status(void)
{
    return PWC_COMM_STATUS_BUTT;
}

/*****************************************************************************
* 函 数 : bsp_bbp_twbbp_poweron
* 功 能 : 被低功耗调用，用来给TW BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweron(void)
{

	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweroff(void)
{

	return BSP_OK;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_twbbp_status(void)
{
    return PWC_COMM_STATUS_BUTT;
}
/*****************************************************************************
* 函 数 : BSP_LPS_Get_Ta
* 功 能 : 被lps调用,查看上行子帧头相对系统子帧头的提前量
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
BSP_U16 BSP_LPS_Get_Ta(void)
{
	return 0xffff;
}

/*****************************************************************************
* 函 数 : BSP_BBP_GET_WAKEUP_TIME
* 功 能 : 获取dsp唤醒时间
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
BSP_U32 BSP_BBP_GET_WAKEUP_TIME(PWC_COMM_MODE_E mode)
{
    return 0xffffffff;
}

#endif
/*****************************************************************************
* 函 数 : BSP_BBPIntTimerSwitchStatus
* 功 能 : 用于获取LBBP TIMER中断使能状态,打桩函数返回值设为-1，适配V9R1
* 输 入 : BSP_BOOL
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
BSP_S32 BSP_BBPIntTimerSwitchStatus(BSP_BOOL *pbStat)
{
	*pbStat = 0;
	return -1;
}

