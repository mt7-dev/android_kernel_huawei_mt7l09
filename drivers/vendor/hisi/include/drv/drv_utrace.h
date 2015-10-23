

#ifndef _DRV_UTRACE_H
#define _DRV_UTRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"


/*****************************************************************************
 函 数 名  : DRV_UTRACE_START
 功能描述  : USB Trace启动接口, 开启Trace采集
 输入参数  : pData: Trace采集启动数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_UTRACE_START(BSP_U8 * pData);

/*****************************************************************************
 函 数 名  : DRV_UTRACE_TERMINATE
 功能描述  : USB Trace终止接口，停止或者异常导出
 输入参数  : pData  : Trace终止数据结构指针          
 输出参数  : 无
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_UTRACE_TERMINATE(BSP_U8 * pData);

#ifdef __cplusplus
}
#endif

#endif /* end of _DRV_UTRACE_H*/

