

#ifndef __DRV_RESET___
#define __DRV_RESET___

#include "drv_comm.h"

/*用于区分在Modem/HIFI复位前还是复位后调用回调函数*/
typedef enum
{
	DRV_RESET_CALLCBFUN_RESET_BEFORE,   /*Modem/HIFI 复位前*/
	DRV_RESET_CALLCBFUN_RESET_AFTER,    /*Modem/HIFI 复位后*/
	DRV_RESET_CALLCBFUN_RESETING,       /*Modem/HIFI 复位中*/
	DRV_RESET_CALLCBFUN_MOEMENT_INVALID
} DRV_RESET_CALLCBFUN_MOMENT;

/*****************************************************************************
 函 数 名  : pdrv_reset_cbfun
 功能描述  : 组件需要注册的回调函数定义.
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam,表示复位前还是复位后,
             int userdata,用户数据
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             非0：操作失败。
*****************************************************************************/
typedef int (*pdrv_reset_cbfun)(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);

/*****************************************************************************
 函 数 名  : ccorereset_regcbfunc
 功能描述  : 提供上层应用程序注册Modem单独复位前/后的回调接口函数。
 输入参数  : pname：上层组件注册的名字，注意不包括结束符最长9个字符，底软负责存储。
             pcbfun：回调函数指针。
             userdata:上层组件数据，在调用回调函数时，作为入参传给用户。
             priolevel: 回调函数调用优先级，enum DRV_RESET_CALLCBFUN_PIOR定义的值，值越小,优先级越高
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
#define DRV_CCORERESET_REGCBFUNC(pname,pcbfun, userdata, priolevel)\
                ccorereset_regcbfunc(pname,pcbfun, userdata, priolevel)

/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 提供上层应用程序注册HIFI单独复位的回调接口函数。
 输入参数  : pname：上层组件注册的名字，注意不包括结束符最长9个字符，底软负责存储。
             pcbfun：回调函数指针。
             puserdata:上层组件数据，在调用回调函数时，作为入参传给用户。
             priolevel: 回调函数调用优先级，0-49
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
#define DRV_HIFIRESET_REGCBFUNC(pname,pcbfun, userdata, priolevel)\
                hifireset_regcbfunc(pname,pcbfun, userdata, priolevel)

/*****************************************************************************
 函 数 名  : BSP_CCPU_Rest_Over
 功能描述  : C核单独复位成功后，COMM组件调用该函数进行通知ACPU。
 输入参数  : 无
 输出参数  : 无
 使用CPU   : CCPU
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_CCPU_Rest_Over(void);
#define DRV_CCPU_RESET_OVER()\
                BSP_CCPU_Rest_Over()

/*****************************************************************************
 函 数 名  : modem_state_set
 功能描述  : 提供ttf设置modem状态的API
 输入参数  : unsigned int state
 输出参数  : 无
 返 回 值  : <0     failed,invalid state
             =0     success
*****************************************************************************/
extern int modem_state_set(unsigned int state);
#define DRV_MODEM_STATE_SET(state)\
                modem_state_set(state)

#endif /* end of drv_reset.h */

