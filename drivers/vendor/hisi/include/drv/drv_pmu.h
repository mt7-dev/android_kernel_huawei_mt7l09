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

#ifndef __DRV_PMU_H__
#define __DRV_PMU_H__

#include "drv_comm.h"
#include "drv_dpm.h"

/*通信模块供电的 CONSUMER的id枚举*/
typedef enum EM_MODEM_CONSUMER_ID_E
{
    MODEM_PA0,
    MODEM_RFIC0_ANALOG0,
    MODEM_RFIC0_ANALOG1,
    MODEM_FEM0,
    MODEM_PA_VBIAS0,
    MODEM_PA1,
    MODEM_RFIC1_ANALOG0,
    MODEM_RFIC1_ANALOG1,
    MODEM_FEM1,
    MODEM_PA_VBIAS1,
    MODEM_CONSUMER_ID_BUTT
}EM_MODEM_CONSUMER_ID;

/*PA动态电压枚举*/
typedef enum EM_MODEM_PA_DYNAMIC_VOLTAGE_ID_E
{
    MODEM_PA_LOW_POWER_VOLTAGE,
}EM_MODEM_PA_DYNAMIC_VOLTAGE_ID;


/*************************PMU BEGIN***********************************/

/*****************************************************************************
 函 数 名  : DRV_GET_PMU_STATE
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
static INLINE unsigned int DRV_GET_PMU_STATE(void*  Pmu_State)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : BSP_PMU_UsbEndRegActionFunc
 功能描述  : 本接口是USB插入/拔出回调注册函数。
 输入参数  : srcFunc：注册的USB插入或拔出动作发生时的回调函数指针。
             actionIndex：动作指示。
                          0：参数srcFunc是注册的USB插入动作的回调函数；
                          1：参数srcFunc是注册的USB拔出动作的回调函数。

 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：调用此接口，将所要执行的函数指针传入，即在USB插入或拔出时调用所要执行的函数。
*****************************************************************************/
extern int BSP_PMU_UsbEndRegActionFunc(pFUNCPTR srcFunc, unsigned char actionIndex);
#define DRV_USB_ENDREGACTION_FUNC(srcFunc,actionIndex) \
                BSP_PMU_UsbEndRegActionFunc(srcFunc,actionIndex)

/************************************
函 数 名  : BSP_PMU_AptEnable
功能描述  : 使能PMU的APT功能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_OK，设置成功
		其他值：设置失败
**************************************/
static INLINE BSP_S32 BSP_PMU_AptEnable(BSP_VOID)
{
	return 0;
}
#define DRV_PMU_APT_ENABLE()    BSP_PMU_AptEnable()

/************************************
函 数 名  : BSP_PMU_AptDisable
功能描述  : 去使能PMU的APT功能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_OK，设置成功
		其他值：设置失败
**************************************/
static INLINE BSP_S32 BSP_PMU_AptDisable(BSP_VOID)
{
	return 0;
}
#define DRV_PMU_APT_DISABLE()    BSP_PMU_AptDisable()

/************************************
函 数 名  : BSP_PMU_AptIsEnable
功能描述  : 查询PMU的APT功能是否使能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_TURE，使能APT
		    BSP_FALSE,未使能APT
**************************************/
static INLINE BSP_BOOL  BSP_PMU_AptIsEnable(BSP_VOID)
{
	return 1;
}
#define DRV_PMU_APT_ISENABLE()    BSP_PMU_AptIsEnable()

/*****************************************************************************
 函 数 名  : BSP_MNTN_PmuSelfCheck
 功能描述  : PMU自检。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int BSP_MNTN_PmuSelfCheck(void)
{
	return 0;
}
#define DRV_PMU_SELFCHECK()    BSP_MNTN_PmuSelfCheck()


/*for dsda*/
typedef enum
{
    RF_ID0 = 0,
    RF_ID1 = 1,
    RF_ID_BUTTOM
}RF_ID;

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoDown
 功能描述  : 本接口实现WCDMA和GSM RF LDO下电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
static INLINE void DRV_RF_LDODOWN(RF_ID rfId)
{
    return;     /* 打桩 */
}

#define WCDMA_MODE      0
#define GSM_MODE        1
#define LTE_MODE        2
#define TDS_MODE        3
/*****************************************************************************
 函 数 名  : BSP_GUSYS_ModeSwitchs
 功能描述  : 本接口实现WCDMA/GSM系统模式切换。
 输入参数  : uiSysMode：0：切换到WCDMA模式；1：切换到GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int  DRV_SYSMODE_SWITCH (unsigned int uiSysMode)
{
    if(WCDMA_MODE == uiSysMode)
    {
        return OK;
    }
    else if(GSM_MODE == uiSysMode)
    {
        return OK;
    }
    else if(LTE_MODE == uiSysMode || TDS_MODE == uiSysMode)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

/*************************PMU END*************************************/
/*****************************************************************************
 函 数 名  : DRV_PASTAR_EXC_CHECK
 功能描述  : 通信模块检查PASTAR是否有异常接口
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : 0          没有异常
             -1       存在异常
*****************************************************************************/
extern BSP_S32 DRV_PASTAR_EXC_CHECK(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_SET
 功能描述  : 通信模块设置电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      设置的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          设置成功
             -1       设置失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_VOLTAGE_SET( EM_MODEM_CONSUMER_ID consumer_id, BSP_U32 voltage_mv );

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_GET
 功能描述  : 通信模块获取电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      获得的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          获取成功
             -1       获取失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_VOLTAGE_GET( EM_MODEM_CONSUMER_ID consumer_id, BSP_U32 *voltage_mv );

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_LIST
 功能描述  : 通信模块获取电压设置范围接口
 输入参数  : consumer_id     用户id
             list            电压范围数组
             size            数组大小
 输出参数  : 无
 返 回 值  : 0          获取成功
             -1       获取失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_VOLTAGE_LIST(EM_MODEM_CONSUMER_ID consumer_id,BSP_U16 **list, BSP_U32 *size);

/*****************************************************************************
 函 数 名  : DRV_MODEM_APT_ENABLE
 功能描述  : 通信模块使能APT接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         设置成功
             -1      设置失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_APT_ENABLE(PWC_COMM_MODEM_E modem_id,PWC_COMM_MODE_E mode_id);

/*****************************************************************************
 函 数 名  : DRV_MODEM_APT_DISABLE
 功能描述  : 通信模块去使能APT接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         设置成功
             -1      设置失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_APT_DISABLE(PWC_COMM_MODEM_E modem_id,PWC_COMM_MODE_E mode_id);

/*****************************************************************************
 函 数 名  : DRV_MODEM_APT_STATUS_GET
 功能描述  : 通信模块获取当前APT状态接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : PWRCTRL_COMM_ON     APT使能
             PWRCTRL_COMM_OFF    APT未使能
             -1           获取失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_APT_STATUS_GET(PWC_COMM_MODEM_E modem_id,PWC_COMM_MODE_E mode_id);

/*****************************************************************************
 函 数 名  : DRV_MODEM_MODE_CONFIG
 功能描述  : 通信模块配置G模或W模接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         配置成功
             -1      配置失败
*****************************************************************************/
extern BSP_S32 DRV_MODEM_MODE_CONFIG(PWC_COMM_MODEM_E modem_id,PWC_COMM_MODE_E mode_id);



#endif

