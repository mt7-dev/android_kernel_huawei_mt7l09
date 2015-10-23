
#ifndef __TEMPERATURE_BALONG_H__
#define __TEMPERATURE_BALONG_H__

#define TEMPERATURE_OK                   0
#define TEMPERATURE_ERROR                -1

/*****************************************************************************
* 函 数 名  : TEM_FUNCPTR
*
* 功能描述  : 用于回调POWER_SUPPLY事件上报函数
*
* 输入参数  : u32 device_id        DEVICE_ID_TEMP
*             u32 event_id         POWER_SUPPLY_HEALTH_COLD或者POWER_SUPPLY_HEALTH_DEAD
*
* 输出参数  : 无
*
* 返 回 值  : 0 成功 -1失败
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
typedef void (*TEM_FUNCPTR)(unsigned int device_id, unsigned int event_id);

s32 bsp_tem_protect_callback_register(TEM_FUNCPTR p);



#endif
