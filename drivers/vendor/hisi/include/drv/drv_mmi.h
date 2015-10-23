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

#ifndef __DRV_MMI_H__
#define __DRV_MMI_H__

#include "drv_comm.h"

/*************************MMI START*****************************/

/***************************************************************
*  函数名        :   BSP_MMI_TestResultSet
*  函数功能      :   本接口是将MMI的测试标志写入NV
                     若ulFlag == 1 则写入 “ST P”
                     若ulFlag == 0 则写入 “ST F”
*  输入参数      :   BSP_U32 ulFlag
*  输出参数      :   无
*  返回值        :   成功(0)  失败(-1)
***************************************************************/
int BSP_MMI_TestResultSet(unsigned int ulFlag);
#define DVR_MMI_TEST_RESULT_SET(ulFlag) BSP_MMI_TestResultSet(ulFlag)

/***************************************************************
*  函数名        :   BSP_MMI_TestResultGet
*  函数功能      :   本接口比较MMI NV中的是否是“ST P”
                                  是返回1   不是返回0
*  输入参数      :   无
*  输出参数      :   无
*  返回值        :   成功(1)  失败(0)
***************************************************************/
unsigned int BSP_MMI_TestResultGet(void);
#define DVR_MMI_TEST_RESULT_GET() BSP_MMI_TestResultGet()

/*************************MMI END*****************************/

#endif

