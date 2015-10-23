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

#ifndef __DRV_EFUSE_H__
#define __DRV_EFUSE_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"


#define     BUF_ERROR          (-55)
#define     LEN_ERROR          (-56)
#define     READ_EFUSE_ERROR   (-57)




extern int efuseWriteHUK(char *pBuf,unsigned int len);

#define EFUSE_WRITE_HUK(pBuf,len) efuseWriteHUK(pBuf,len)



extern int CheckHukIsValid(void);
#define DRV_CHECK_HUK_IS_VALID() CheckHukIsValid()


extern int DRV_GET_DIEID(unsigned char* buf,int length);


extern int DRV_GET_CHIPID(unsigned char* buf,int length);

#ifdef __cplusplus
}
#endif

#endif

