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

#ifndef __DRV_MD5_H__
#define __DRV_MD5_H__

#include "drv_comm.h"

/*************************MD5相关 START*******************************/

/*****************************************************************************
 函 数 名  : VerifySIMLock
 功能描述  : 判断当前解锁码是否正确 .
 输入参数  : imei       - 单板IMEI号
             unlockcode - 解锁码
 输出参数  : 无。
 返 回 值  : 1：   解锁成功
             0:    解锁失败
*****************************************************************************/
extern int VerifySIMLock(char* UnlockCode, char* Key);
#define DRV_CARDLOCK_MD5_VERIFY(unlockcode, imei)  VerifySL(unlockcode, imei)


extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*************************MD5相关 END*********************************/

#endif

