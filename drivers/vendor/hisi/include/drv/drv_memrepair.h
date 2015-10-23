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

#ifndef __DRV_MEM_REPAIR_H__
#define __DRV_MEM_REPAIR_H__

#include "drv_comm.h"

extern int bsp_get_memrepair_time(void);

#define DRV_GET_MEM_REPAIR_TIME() bsp_get_memrepair_time()

#endif

