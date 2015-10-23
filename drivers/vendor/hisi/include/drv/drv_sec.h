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

#ifndef __DRV_SEC_H__
#define __DRV_SEC_H__

#include "drv_comm.h"


/*************************SEC START***********************************/

/*************************************************
 函 数 名   : BSP_SEC_Support
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值   : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_Support(BSP_U8 *pu8Data);
#define DRV_SECURE_SUPPORT(pu8Data) BSP_SEC_Support(pu8Data)

/*************************************************
 函 数 名   : BSP_SEC_AlreadyUse
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值   : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_AlreadyUse(BSP_U8 *pu8Data);
#define DRV_SECURE_ALREADY_USE(pu8Data) BSP_SEC_AlreadyUse(pu8Data)

/*************************************************
 函 数 名   : BSP_SEC_Start
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值   : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_Start(void);
#define DRV_START_SECURE() BSP_SEC_Start()


extern BSP_S32 BSP_SEC_Check(void);
#define DRV_SEC_CHECK() BSP_SEC_Check()

/*************************SEC END*************************************/

#endif

