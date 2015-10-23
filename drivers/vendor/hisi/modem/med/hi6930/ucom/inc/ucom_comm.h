/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ucom_comm.h
  版 本 号   : 初稿
  作    者   : 陈超群
  生成日期   : 2011年5月31日
  最近修改   :
  功能描述   : ucom_comm.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年5月31日
    作    者   : 陈超群
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "ucom_config.h"
#include "ucom_stub.h"  /* 测试打桩 */
#include <string.h>

#ifndef __UCOM_COMM_H__
#define __UCOM_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/

#if (VOS_CPU_TYPE == VOS_HIFI)

/*****************************************************************************
 宏 名 称  : UCOM_SECTION
 功能描述  : 为紧跟后面的变量定义或者函数定义指定段名.
 输入参数  : szSecName - 字符串,段名,规范为.模块.功能.后缀
                         其中, 后缀取值为data/text/rodata/bss
                         szSecName 取值例如".osa_stack.bss"
 使用示例  :
             (1)全局变量, UCOM_SECTION放置在变量定义前面, 为保证代码阅读时SI能
             够解析变量, 建议如下:
                 UCOM_SECTION(".osa_stack.bss")
                 VOS_UINT32 g_auwOsaTaskStack[10];

             (2)函数, UCOM_SECTION放置在函数实现前面, 为保证代码阅读时SI能够解
             析函数, 建议如下:
                 #define OSA_SEC_INTR   UCOM_SECTION(".osa_intr.text")
                 OSA_SEC_INTR
                 void interrupt1(void)
                 {
                    ......
                 }
*****************************************************************************/
#define UCOM_SECTION(szSecName)         __attribute__((section(szSecName)))

/*****************************************************************************
 宏 名 称  : UCOM_ALIGN
 功能描述  : 为紧跟后面的变量定义指定对齐的字节数.
 输入参数  : bytes - 要求指定对齐的字节数
 使用示例  :
             UCOM_ALIGN放置在变量定义前面, 为保证代码阅读时SI能够解析变量,
             建议如下:
                 UCOM_ALIGN(32)
                 VOS_UINT32 g_auwOsaTaskStack[10];
*****************************************************************************/
#define UCOM_ALIGN(bytes)               __attribute__((aligned(bytes)))

#define UCOM_INLINE                     static __inline__

/*转定义32比特读函数*/
#define UCOM_RegRd(uwAddr)              (*((volatile VOS_UINT32 *)(uwAddr)))

/*转定义32比特写函数*/
#define UCOM_RegWr(uwAddr, uwValue)     (*((volatile VOS_UINT32 *)(uwAddr)) = uwValue)

#else

#define UCOM_SECTION(szSecName)
#define UCOM_ALIGN(bytes)
#define UCOM_INLINE                     __inline

/*转定义32比特读函数*/
#define UCOM_RegRd(uwAddr)              /*lint -e(778)*/(g_auwRegStub[(uwAddr)&0xffff])

/*转定义32比特写函数*/
#define UCOM_RegWr(uwAddr, uwValue)     /*lint -e(778)*/(g_auwRegStub[(uwAddr)&0xffff] = uwValue)

#endif /* #if (VOS_CPU_TYPE == VOS_HIFI) */

/*****************************************************************************
 函 数 名  : UCOM_COMM_CycSub
 功能描述  : 考虑给定范围内循环, 计算给定两个量的差
 输入参数  : Cur    - 被减数
             Pre    - 减数
             CycLen - 环形队列线性空间的长度
 输出参数  : 无
 返 回 值  : (Cur - Pre) 或者(CycLen - Pre + Cur)
*****************************************************************************/
#define UCOM_COMM_CycSub(Cur, Pre, CycLen)                    \
    (((Cur) < (Pre)) ? (((CycLen) - (Pre)) + (Cur)) : ((Cur) - (Pre)))


/*转定义按比特读函数*/
#define UCOM_RegBitRd(uwAddr, uhwStartBit, uhwEndBit)               \
    UCOM_COMM_RegBitRd(uwAddr, uhwStartBit, uhwEndBit)

/*转定义按比特写函数*/
#define UCOM_RegBitWr(uwAddr, uhwStartBit, uhwEndBit, swContent)    \
    UCOM_COMM_RegBitWr(uwAddr, uhwStartBit, uhwEndBit, swContent)

/*通过VOS通道发送OSA消息*/
#define UCOM_SendOsaMsg(uwSenderPid, uwReceiverPid, pvMsg, uhwLen)  \
    UCOM_COMM_SendMsg((uwSenderPid), (uwReceiverPid), DRV_MAILBOX_CHANNEL_VOS_CH, (((MsgBlock *)pvMsg)->aucValue), (uhwLen - VOS_MSG_HEAD_LENGTH))

/*通过音频专用通道发送跨核消息*/
#define UCOM_SendAudioMsg(uwSenderPid, uwReceiverPid, pvMsg, uhwLen) \
    UCOM_COMM_SendMsg((uwSenderPid), (uwReceiverPid), DRV_MAILBOX_CHANNEL_AUDIO_CH, (pvMsg), (uhwLen))

/*通过AP、hifi专用通道发送跨核消息*/
#define UCOM_SendDrvMsg(uwSenderPid, uwReceiverPid, pvMsg, uhwLen) \
    UCOM_COMM_SendMsg((uwSenderPid), (uwReceiverPid), DRV_MAILBOX_CHANNEL_DRV_CH, (pvMsg), (uhwLen))

/*通过DSP专用通道发送跨核消息*/
#define UCOM_SendDspMsg(uwSenderPid, uwReceiverPid, pvMsg, uhwLen)  \
    UCOM_COMM_SendMsg((uwSenderPid), (uwReceiverPid), DRV_MAILBOX_CHANNEL_DSP_CH, (pvMsg), (uhwLen))

/*通过测试通道发送跨核消息*/
#define UCOM_SendTestMsg(uwSenderPid, uwReceiverPid, pvMsg, uhwLen) \
    UCOM_COMM_SendMsg((uwSenderPid), (uwReceiverPid), DRV_MAILBOX_CHANNEL_AP_DEFAULT_CH, (pvMsg), (uhwLen))

/*使用宏控制常用内存操作函数为库函数或自定义函数*/
#ifdef _UCOM_USE_STD_LIB

#define UCOM_MemCpy(pvDes, pvSrc, uwCount) /*lint -e(419,420)*/\
    memcpy((pvDes), (const void *)(pvSrc), (uwCount))

#define UCOM_MemSet(pvDes, ucData, uwCount)     \
    memset((pvDes), (int)(ucData), (uwCount))

#else

#define UCOM_MemCpy(pvDes, pvSrc, uwCount)      \
    UCOM_COMM_CopyMem((pvDes), (pvSrc), (uwCount))

#define UCOM_MemSet(pvDes, ucData, uwCount)     \
    UCOM_COMM_SetMem((pvDes), (ucData), (uwCount))

#endif /* #ifdef _UCOM_USE_STD_LIB */

#define UCOM_COMM_UINT32_MAX            (0xffffffff)        /*32bit无符号数最大值*/

/* 如下宏定义需要在PID确定后进行转定义 */
#define UCOM_PID_PS_NAS                 WUEPS_PID_VC
#define UCOM_PID_PS_WTTF                WUEPS_PID_MAC
#define UCOM_PID_PS_OM                  WUEPS_PID_OM
#define UCOM_PID_PS_TDTTF               TPS_PID_MAC
#define UCOM_PID_DSP_GPHY               DSP_PID_GPHY
#define UCOM_PID_DSP_WPHY               DSP_PID_WPHY
#define UCOM_PID_DSP_TDPHY              DSP_PID_TDPHY
#define UCOM_PID_DSP_IMSA               PS_PID_IMSVA

#if (VOS_CPU_TYPE == VOS_HIFI)
/* 对于V7R2，地址映射及虚实地址如下
   0x50000000 - 0x5fffffff : 实际地址; cached   address
   0xf0000000 - 0xffffffff : 映射地址; uncached address
   地址的映射关系在reset.S中配置  */
#define UCOM_MEMMAP_OFFSET              ((-1)*(0xa0000000))                     /* 地址映射偏移，HIFI可通过地址映射方式实现cache地址范围的uncache访问 */
#define UCOM_CACHED2PHY_OFFSET          (0x00000000)                            /* cached的地址到非映射地址(物理地址)的偏移 */
#else
#define UCOM_MEMMAP_OFFSET              (0x00000000)                            /* 地址映射偏移，非HIFI平台暂设定不进行cache地址向uncache映射 */
#define UCOM_CACHED2PHY_OFFSET          (0x00000000)                            /* cached的地址到非映射地址(物理地址)的偏移 */
#endif

#define UCOM_SEC_TCMBSS                 UCOM_SECTION(".dram0.bss")              /* 指定TCM bss段 */
#define UCOM_SEC_TCMDATA                UCOM_SECTION(".dram0.data")             /* 指定TCM data段 */
#define UCOM_SEC_TCMTEXT                UCOM_SECTION(".dram0.text")             /* 指定TCM text段 */
#define UCOM_SET_SRAMSHARE              UCOM_SECTION(".sram.shareaddr")         /* 指定SRAM地址共享段 */
#define UCOM_SET_UNINIT                 UCOM_SECTION(".sram.uninit")            /* 指定不初始化段 */

#define UCOM_PROTECT_WORD1              (0x5A5A5A5A)
#define UCOM_PROTECT_WORD2              (0x5A5A5A5A)
#define UCOM_PROTECT_WORD3              (0xA5A5A5A5)
#define UCOM_PROTECT_WORD4              (0xA5A5A5A5)

#define UCOM_HIFI_FREQ                  (360000)                                /* Hifi主频360MHz */
#define UCOM_COMM_LOOP_CLOCK_PERIOD     ( 1 )                                   /* 循环代码消耗时钟周期  */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum UCOM_BIT_ENUM
{
    UCOM_BIT0 = 0,
    UCOM_BIT1 ,
    UCOM_BIT2 ,
    UCOM_BIT3 ,
    UCOM_BIT4 ,
    UCOM_BIT5 ,
    UCOM_BIT6 ,
    UCOM_BIT7 ,
    UCOM_BIT8 ,
    UCOM_BIT9 ,
    UCOM_BIT10,
    UCOM_BIT11,
    UCOM_BIT12,
    UCOM_BIT13,
    UCOM_BIT14,
    UCOM_BIT15,
    UCOM_BIT16,
    UCOM_BIT17,
    UCOM_BIT18,
    UCOM_BIT19,
    UCOM_BIT20,
    UCOM_BIT21,
    UCOM_BIT22,
    UCOM_BIT23,
    UCOM_BIT24,
    UCOM_BIT25,
    UCOM_BIT26,
    UCOM_BIT27,
    UCOM_BIT28,
    UCOM_BIT29,
    UCOM_BIT30,
    UCOM_BIT31,
    UCOM_BIT_BUTT
};
/*****************************************************************************
 实 体 名  : DRV_MAILBOX_CHANNEL_ENUM
 功能描述  : 定义核间通讯的数据类型
*****************************************************************************/
enum DRV_MAILBOX_CHANNEL_ENUM
{
   DRV_MAILBOX_CHANNEL_VOS_CH = 0,
   DRV_MAILBOX_CHANNEL_VOS_CH_URGENT = 1,
   DRV_MAILBOX_CHANNEL_AUDIO_CH = 2,
   DRV_MAILBOX_CHANNEL_DSP_CH = 3,
   DRV_MAILBOX_CHANNEL_DRV_CH = 4,
   DRV_MAILBOX_CHANNEL_AP_DEFAULT_CH = 5,
   DRV_MAILBOX_CHANNEL_BUTT
};
typedef unsigned short DRV_MAILBOX_CHANNEL_ENUM_UINT16;
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
 实体名称  : UCOM_DATA_BLK_STRU
 功能描述  : 数据块结构
*****************************************************************************/
typedef struct
{
    VOS_UCHAR      *pucData;            /* 数据块地址  */
    VOS_UINT32      uwSize;             /* pucData长度 */
}UCOM_DATA_BLK_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID UCOM_COMM_CopyMem(
                       VOS_VOID               *pvDes,
                       VOS_VOID               *pvSrc,
                       VOS_UINT32              uwCount);
extern VOS_VOID UCOM_COMM_IdleHook(VOS_UINT32 ulVosCtxSw);
extern VOS_UINT32 UCOM_COMM_LowFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
extern VOS_UINT32 UCOM_COMM_NormalFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
extern VOS_UINT32 UCOM_COMM_RegBitRd(
                       VOS_UINT32              uwAddr,
                       VOS_UINT16              uhwStartBit,
                       VOS_UINT16              uhwEndBit);
extern VOS_VOID UCOM_COMM_RegBitWr(
                       VOS_UINT32              uwAddr,
                       VOS_UINT16              uhwStartBit,
                       VOS_UINT16              uhwEndBit,
                       VOS_UINT32              uwContent);
extern VOS_UINT32 UCOM_COMM_RtFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
extern VOS_VOID UCOM_COMM_SendMsg(
                       VOS_UINT32              uwSenderPid,
                       VOS_UINT32              uwReceiverPid,
                       VOS_UINT32              uwChannel,
                       VOS_VOID               *pvMsg,
                       VOS_UINT32              uwLen);
extern VOS_VOID UCOM_COMM_SetMem(
                       VOS_VOID               *pvDes,
                       VOS_UCHAR               ucData,
                       VOS_UINT32              uwCount);
extern VOS_VOID UCOM_COMM_SendDspMsg(
                       VOS_UINT32              uwSenderPid,
                       VOS_UINT32              uwReceiverPid,
                       VOS_VOID               *pvMsg,
                       VOS_UINT32              uwLen,
                       VOS_VOID               *pvTail,
                       VOS_UINT32              uwTailLen);
extern VOS_VOID UCOM_COMM_Sleep(VOS_UINT32 ulSleepTimeLen);
extern VOS_VOID UCOM_COMM_GetChnBit16(
                       VOS_VOID               *pvDes,
                       VOS_UINT32              uwSize,
                       VOS_UINT32              uwChnID,
                       VOS_VOID               *pvSrc,
                       VOS_UINT32              uwChnNum);
extern VOS_VOID UCOM_COMM_SetChnBit16(
                       VOS_VOID               *pvDes,
                       VOS_UINT32              uwChnNum,
                       VOS_VOID               *pvSrc,
                       VOS_UINT32              uwSize,
                       VOS_UINT32              uwChnID);

extern VOS_VOID UCOM_COMM_TimeDelayFor( VOS_UINT32 uwTimeLen );
extern VOS_UINT32 UCOM_GetUncachedAddr(VOS_UINT32 uwAddr);
extern VOS_UINT32 UCOM_GetUnmappedAddr(VOS_UINT32 uwAddr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ucom_comm.h */
