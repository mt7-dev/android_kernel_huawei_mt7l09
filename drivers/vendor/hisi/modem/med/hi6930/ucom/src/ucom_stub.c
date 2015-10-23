/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ucom_stub.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年6月22日
  最近修改   :
  功能描述   : 各类桩函数实现
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月22日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_stub.h"
#include "med_drv_mb_hifi.h"

#ifdef _MED_TEST_UT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codec_op_lib.h"
#endif

#ifdef VOS_VENUS_TEST_STUB
#include "venus_stub.h"
#include "om_log.h"
#include "HifiOmInterface_new.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#if (VOS_CPU_TYPE != VOS_HIFI)
VOS_UCHAR                               g_aucNvStub[0xffff];
VOS_UINT32                              g_auwRegStub[0xffff];
VOS_UINT32                              g_uwSoftIntInfo;
MAILBOX_HEAD_STRU						g_astMailboxHeader[DRV_MAILBOX_ID_BUTT];
VOS_UCHAR                               g_aucMailbox[0xffff];
VOS_UCHAR                               g_aucStubSocpBuff[0x2000];

VOS_UINT32 g_uwDrvMailboxAddrTbl[DRV_MAILBOX_ID_BUTT][DRV_MAILBOX_INFO_NO] =
{
    {(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_HIFI2CCPU], (VOS_UINT32)&g_aucMailbox[0],                           MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)},
    {(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_CCPU2HIFI], (VOS_UINT32)&g_aucMailbox[MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)],  MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)},
    {(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_HIFI2ACPU], (VOS_UINT32)&g_aucMailbox[MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)], MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG)},
    {(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_ACPU2HIFI], (VOS_UINT32)&g_aucMailbox[MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG)], MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG)},
	{(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_HIFI2BBE16], (VOS_UINT32)&g_aucMailbox[MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG)\
	                                                                                      +MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG)], MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG)},
	{(VOS_UINT32)&g_astMailboxHeader[DRV_MAILBOX_ID_BBE162HIFI], (VOS_UINT32)&g_aucMailbox[MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)\
                                                                                          +MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG)\
	                                                                                      +MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG)\
	                                                                                      +MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG)], MAILBOX_QUEUE_SIZE(BBE16, HIFI, MSG)}};

#endif

#ifdef _MED_TEST_UT
char    g_szMedTstDataPath[] = ".\\data\\";
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#ifdef _MED_TEST_UT

FILE *MED_TstOpenFile(char *pscFileName)
{
    char  szFileName[256];
    FILE *pfOpened = NULL;

    /* 打开指定文件 */
    strcpy(szFileName, g_szMedTstDataPath);
    strcat(szFileName, pscFileName);
    pfOpened  = fopen(szFileName, "rb");

    if (NULL == pfOpened)
    {
        printf("Failed to open %s\r\n", szFileName);
    }

    return pfOpened;
}
VOS_INT32 MED_TstCmpSn(
                VOS_INT16              *pshwSn1,
                VOS_INT16              *pshwSn2,
                VOS_INT32               swLen,
                VOS_INT16               shwErr,
                VOS_INT16              *pshwMaxErr,
                VOS_INT32              *pswMaxErrCnt)
{
    VOS_INT32 swCnt     = 0;
    VOS_INT16 shwMaxErr = 0;
    VOS_INT16 shwCurErr = 0;
    VOS_INT32 swMaxErrCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        shwCurErr = CODEC_OpSub(pshwSn1[swCnt], pshwSn2[swCnt]);
        shwCurErr = CODEC_OpAbs_s(shwCurErr);

        if (shwCurErr > shwMaxErr)
        {
            shwMaxErr       = shwCurErr;
            swMaxErrCnt     = swCnt;
        }

    }

    if (pshwMaxErr != VOS_NULL)
    {
        if (shwMaxErr > *pshwMaxErr)
        {
            *pshwMaxErr     = shwMaxErr;

            if (pswMaxErrCnt != VOS_NULL)
            {
                *pswMaxErrCnt  = swMaxErrCnt;
            }
        }
    }

    if (shwMaxErr > shwErr)
    {
        return 1;
    }

    return 0;
}
#endif

#ifdef VOS_VENUS_TEST_STUB

VOS_UINT32 DRV_MAILBOX_SendMsg(
                VOS_UINT32                  uwCpuId,
                VOS_UINT32                  uwChannel,
                VOS_UINT32                  uwPriority,
                VOS_UCHAR                  *pucMsg,
                VOS_UINT32                  uwMsgSize)

{
    #if VENUS_ST_OS
    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_MSG_INTER_CORE);
    VOS_DEBUG_LOG_CPU_INFO(uwCpuId);
    VOS_DEBUG_LOG_CPU_INFO(uwMsgSize);
    #endif

    VOS_DEBUG_LOG_CPU_INFO_A(pucMsg, uwMsgSize/4);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_LOG_LogReport
 功能描述  : 上报调试日志
 输入参数  : OM_LOG_RPT_LEVEL_ENUM_UINT16        enLevel    - 日志级别
             OM_FILE_ID_ENUM_UINT16              enFileNo   - 日志文件号
             VOS_UINT16                          uhwLineNo  - 日志行号
             OM_LOG_ID_ENUM_UINT32               enLogId    - 日志编号
             VOS_UINT16                          uhwLogFreq - 日志频度
             VOS_INT32                           swValue1   - 附带值1
             VOS_INT32                           swValue2   - 附带值2
             VOS_INT32                           swValue3   - 附带值3
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_LOG_LogReport(
                OM_LOG_RPT_LEVEL_ENUM_UINT16        enLevel,
                OM_FILE_ID_ENUM_UINT16              enFileNo,
                VOS_UINT16                          uhwLineNo,
                OM_LOG_ID_ENUM_UINT32               enLogId,
                VOS_UINT16                          uhwLogFreq,
                VOS_INT32                           swValue1,
                VOS_INT32                           swValue2,
                VOS_INT32                           swValue3)

{
    VOS_UINT32  uwDataLen = 1;
    VOS_INT32   aswValue[4];

    aswValue[0] = enLogId;
    aswValue[1] = swValue1;
    aswValue[2] = swValue2;
    aswValue[3] = swValue3;

    VOS_VenusLogInfo((VOS_UINT32 *)aswValue, 4);

    return UCOM_RET_SUCC;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

