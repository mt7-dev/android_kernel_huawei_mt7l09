#ifndef __ANTEN_BALONG_H__
#define __ANTEN_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_anten.h"

#define ANTEN_OK               (0)
#define ANTEN_ERROR            (-1)


/*****************************************************************************
 * 函 数 名  :bsp_anten_init
 *
 * 功能描述  : 天线插拔核间通信，C核初始化函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : ANTAN_OK:    操作成功
 *             ANTAN_ERROR: 操作失败
 *
 * 其它说明  : 无
 *
 *****************************************************************************/
s32 bsp_anten_init(void);

/****************************************************
Function:   bsp_anten_int_install   
Description:      中断注册函数，用来获得当前的天线状态，打桩（ASIC平台下需要实现）
Input:        NA;
Output:       NA;
Return:       NA;
Others:       NA;
****************************************************/ 
void bsp_anten_int_install(PWC_COMM_MODEM_E modem_id, void* routine,int para);

int anten_nv_init(void);

#ifdef __cplusplus
}
#endif

#endif
