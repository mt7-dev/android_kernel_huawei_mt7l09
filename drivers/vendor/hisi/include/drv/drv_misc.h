
#ifndef _DRV_MISC_H_
#define _DRV_MISC_H_

/*lint -save -e537*/
#include "drv_comm.h"
/*lint -restore +e537*/

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* 函 数 名  : mdrv_misc_scbackup_ext_write
*
* 功能描述  : 本接口用于备份锁网锁卡文件
*
* 输入参数  : pRamAddr 源文件地址
*             len  写入长度
*
* 输出参数  : 无
*
* 返 回 值  : OK  写入成功 其他失败
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
int mdrv_misc_scbackup_ext_write(unsigned char* pRamAddr, unsigned int len);

/*****************************************************************************
* 函 数 名  : mdrv_misc_scbackup_ext_read
*
* 功能描述  : 本接口用于恢复锁网锁卡文件
*
* 输入参数  : pRamAddr 读出保存文件的地址
*             len  读出长度
*
* 输出参数  : 无
*
* 返 回 值  : OK  读出成功 其他失败
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
int mdrv_misc_scbackup_ext_read(unsigned char* pRamAddr, unsigned int len);




#ifdef __cplusplus
}
#endif

#endif  /*_DRV_MISC_H_*/




