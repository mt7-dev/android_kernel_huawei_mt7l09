/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  secBoot.h
*
*   作    者 :  wuzechun
*
*   描    述 :  secBoot.c的头文件，错误码定义及函数声明
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/


#ifndef _SECBOOT_H                                                                  
#define _SECBOOT_H

#include "OcrShare.h"

/*定义映像类型，0x01(RAM初始化程序)，0x02(安全启动下载映像)，03(USB自举下载映像) */
typedef enum _tagIMAGE_TYPE_E
{
    IMAGE_TYPE_RAM_INIT = 0x01,
    IMAGE_TYPE_DOWNLOAD,
    IMAGE_TYPE_BOOTLOAD
}IMAGE_TYPE_E;

void go(FUNCPTR entry);
int idioIdentify(UINT32 dataAddr, UINT32 dataLen, KEY_STRUCT *pubKey, UINT32* pIdio);
int secCheck(UINT32 ulImageAddr, IMAGE_TYPE_E eImageTye);

extern void BSP_UBOOT_StartExec(void);
 
#endif

