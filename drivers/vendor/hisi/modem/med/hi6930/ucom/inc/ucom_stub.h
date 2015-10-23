/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ucom_stub.h
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年6月22日
  最近修改   :
  功能描述   : ucom_stub.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月22日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"

#ifdef _MED_TEST_UT
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef __UCOM_STUB_H__
#define __UCOM_STUB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#if (VOS_CPU_TYPE != VOS_HIFI)
/* UT工程需要对地址进行打桩 */
#define CODEC_PC_VOICE_OM_CODEC_ADDR        ((VOS_UINT32)&g_auwRegStub[0x0038])
#define CODEC_PC_VOICE_CODEC_OM_ADDR        ((VOS_UINT32)&g_auwRegStub[0x0048])
#define CODEC_PC_VOICE_SET_HOOK           ((VOS_UINT32)&g_auwRegStub[0x0058])

#define UCOM_NV_GetPhyAddr(addr)    (addr)
#define UCOM_NV_MAICNUM_ADDR            ((VOS_UINT32)(&g_aucNvStub[0]))
#define UCOM_NV_DATA_BASE_ADDR          ((VOS_UINT32)(&g_aucNvStub[400]))
#define UCOM_NV_FILEHANDLE_ADDR         ((VOS_UINT32)(&g_aucNvStub[144]))
#define UCOM_NV_CTRLFILE_ADDR           ((VOS_UINT32)(&g_aucNvStub[16]))
#ifndef SHM_MEM_NV_SIZE
#define SHM_MEM_NV_SIZE                 (0x00100000)
#endif
#endif
/* UT工程默认具备PC Voice功能 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
#if (VOS_CPU_TYPE != VOS_HIFI)
extern VOS_UCHAR                        g_aucNvStub[0xffff];
extern VOS_UINT32                       g_auwRegStub[0xffff];
extern MAILBOX_HEAD_STRU                g_astMailboxHeader[6];
extern VOS_UCHAR                        g_aucMailbox[0xffff];
extern VOS_UCHAR                        g_aucStubSocpBuff[0x2000];
extern VOS_UINT32 g_uwDrvMailboxAddrTbl[][3];
#endif

/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if (VOS_CPU_TYPE != VOS_HIFI)
extern unsigned xthal_get_ccount(void);
extern void     xthal_set_ccompare(int, unsigned);
extern unsigned xthal_get_ccompare(int);
#endif

#ifdef _MED_TEST_UT
extern FILE *MED_TstOpenFile(char *pscFileName);
extern VOS_INT32 MED_TstCmpSn(
                VOS_INT16              *pshwSn1,
                VOS_INT16              *pshwSn2,
                VOS_INT32               swLen,
                VOS_INT16               shwErr,
                VOS_INT16              *pshwMaxErr,
                VOS_INT32              *pswMaxErrCnt);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ucom_stub.h */
