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

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"


/*************************GPIO START**********************************/

typedef unsigned int GPIO_OPRT_ENUM_UINT32;
#define DRV_NOT_START_UMTS (0x55aa5a5a)
#define GPIO_OPRT_ERROR    (0xFFFFFFFF)

#define OUTER_RFSWITCH_ON   1
#define OUTER_RFSWITCH_OFF  0

enum GPIO_OPRT_ENUM
{
    GPIO_OPRT_SET = 0,
    GPIO_OPRT_GET,
    GPIO_OPRT_BUTT
};

typedef enum tagGPIO_ANT_MODESET_E
{
    ANT_LTE     =  0,/*lte分集天线*/
    ANT_GSM     =  1,/*GSM副卡天线*/
    ANT_MAX
} GPIO_ANT_MODESET_E;

/*****************************************************************************
 函 数 名  : BSP_GPIO_Oprt
 功能描述  : at^GPIOPL,设置和查询GPIO的电平
 输入参数  : 操作类型 ulOp:
             0  设置各GPIO的PL值
             1  查询当前各GPIO的PL值

             pucPL 对应20Byte的数组,每个BYTE代表一个8个管脚的值

             设置操作时,pucPL为设置的20的Byte
             查询操作时,pucPL为当前获取到的PL的实际值组成的16进制数据
             例如用户设置at^GPIOPL = ABCDEF,则对应pucPL的数组值为{A,B,C,D,E,F,0,0,...}

 输出参数  : 无
 返 回 值  :  0 ：OK  非 0 ：Error

*****************************************************************************/
extern unsigned long drv_gpio_oprt(unsigned long ulOp, unsigned char *pucPL);
#define DRV_GPIO_OPRT(ulOp, pucPL) drv_gpio_oprt(ulOp, pucPL)


/* from v9r1, 根据GPIO 拨码开关判断是否启动协议栈代码, V7R2先打桩实现 */
static INLINE unsigned long DRV_BBP_GPIO_GET(void)
{
	return ~DRV_NOT_START_UMTS;
}
static INLINE int BSP_Modem_OS_Status_Switch(int enable)
{
    return 0;
}
#define DRV_OS_STATUS_SWITCH(enable) BSP_Modem_OS_Status_Switch(enable)

/*****************************************************************************
 函 数 名  : DRV_OUTER_RFSWITCH_SET
 功能描述  : NAS专用接口
             外接Modem的天线选择控制接口
             当前单板不支持此功能（对应NV项为全0），或者其他异常 返回-1
 输入参数  : status 1:外接Modem天线功能，0：非外接Modem天线功能
 输出参数  : 无
 返 回 值  : 0：成功，-1：失败
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年12月30日
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
extern int outer_rfswitch_set(unsigned int status);
#define  DRV_OUTER_RFSWITCH_SET(status) outer_rfswitch_set(status)
/*****************************************************************************
 函 数 名  : DRV_OUTER_RFSWITCH_GET
 功能描述  : NAS专用接口
             外接Modem的天线选择查询接口
             当前单板不支持此功能（对应NV项为全0），或者其他异常 返回-1
 输入参数  : 无
 输出参数  : status 1:外接Modem天线功能，0：非外接Modem天线功能
 返 回 值  : 0：成功，-1：失败
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年12月30日
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
extern int outer_rfswitch_get(unsigned int *status);
#define  DRV_OUTER_RFSWITCH_GET(status) outer_rfswitch_get(status)


extern int bsp_ant_modem_set(GPIO_ANT_MODESET_E mode);
#define  DRV_ANT_MODE_SET(mode) bsp_ant_modem_set(mode)


extern int bsp_gpio_rf_pin_set(unsigned int mask, unsigned int is_hz, unsigned int value);
#define DRV_RF_PIN_HIGHZ_SET(ulPinMask,ulPinHz,ulPinValue) bsp_gpio_rf_pin_set(ulPinMask,ulPinHz,ulPinValue)




/*************************GPIO END************************************/

#ifdef __cplusplus
}
#endif

#endif

