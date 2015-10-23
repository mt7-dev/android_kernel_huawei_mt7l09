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

#ifndef __DRV_SYSCTRL_H__
#define __DRV_SYSCTRL_H__

#include "drv_comm.h"


/*************************SYSCTRL START*******************************/


/*****************************************************************************
 函 数 名  : DRV_SYSCTRL_REG_SET
 功能描述  : sysctrl写接口供GU物理层调用。
 输入参数  : regAddr:SC寄存器偏移地址;setData:SC寄存器值。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_SYSCTRL_REG_SET(unsigned int regAddr, unsigned int setData);
#define DRV_SYSCTRL_REG_SET(regAddr, setData)	BSP_SYSCTRL_REG_SET(regAddr, setData)

/*****************************************************************************
 函 数 名  : DRV_SYSCTRL_REG_GET
 功能描述  : sysctrl读接口供GU物理层调用。
 输入参数  : regAddr:SC寄存器偏移地址;getData:返回的SC寄存器值。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_SYSCTRL_REG_GET(unsigned int regAddr, unsigned int * getData);
#define DRV_SYSCTRL_REG_GET(regAddr, getData)	BSP_SYSCTRL_REG_GET(regAddr, getData)

/*************************SYSCTRL START*******************************/

#endif

