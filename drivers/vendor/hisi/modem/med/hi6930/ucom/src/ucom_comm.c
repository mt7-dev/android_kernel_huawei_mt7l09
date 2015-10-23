/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ucom_comm.c
  版 本 号   : 初稿
  作    者   : 陈超群
  生成日期   : 2011年5月12日
  最近修改   :
  功能描述   : 提供公共操作函数
  函数列表   :
              UCOM_COMM_CopyMem
              UCOM_COMM_IdleHook
              UCOM_COMM_NormalFidInit
              UCOM_COMM_RegBitRd
              UCOM_COMM_RegBitWr
              UCOM_COMM_RtFidInit
              UCOM_COMM_SetMem
  修改历史   :
  1.日    期   : 2011年5月12日
    作    者   : 陈超群
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "ucom_comm.h"
#include "ucom_mem_dyn.h"
#include "om.h"
#include "med_drv_dma.h"
#include "med_drv_ipc.h"
#include "med_drv_mb_hifi.h"
#include "med_drv_timer_hifi.h"
#include "med_drv_uart.h"
#include "med_drv_sio.h"
#include "med_drv_watchdog.h"
#include "ucom_low_power.h"
#include "voice_api.h"
#include "ucom_nv.h"
#include "audio_comm.h"
#include "om_cpuview.h"
#include "ucom_share.h"

#ifdef VOS_VENUS_TEST_STUB
#include "venus_stub.h"
#endif

#ifndef __UCOM_COMM_C__
#define __UCOM_COMM_C__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                  OM_FILE_ID_UCOM_COMM_C

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/*****************************************************************************
  3 函数声明
*****************************************************************************/


VOS_UINT32 UCOM_COMM_LowFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    VOS_UINT32  uwRet = VOS_OK;
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* 调用VOS_RegisterPIDInfo注册DSP_PID_HIFI_OM及消息入口函数 */
            uwRet = VOS_RegisterPIDInfo(DSP_PID_HIFI_OM, OM_COMM_PidInit, OM_COMM_PidProc);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

             /* 调用VOS_RegisterPIDInfo注册DSP_FID_LOW任务优先级为低 */
            uwRet = VOS_RegisterMsgTaskPrio(DSP_FID_LOW, VOS_PRIORITY_P4);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

           break;
        }
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    return uwRet;

}


/*****************************************************************************
 函 数 名  : UCOM_NormalFidInit
 功能描述  : 普通优先级FID初始化函数，其中会进行DSP_PID_VOICE/DSP_PID_OM的PID初始化动作
 输入参数  : enum VOS_INIT_PHASE_DEFINE enInitPhrase
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月28日
    作    者   : 陈超群
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 UCOM_COMM_NormalFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    VOS_UINT32  uwRet = VOS_OK;
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* 调用VOS_RegisterPIDInfo注册DSP_PID_VOICE及消息入口函数 */
            uwRet = VOS_RegisterPIDInfo(DSP_PID_VOICE, VOICE_ApiPidInit, VOICE_ApiPidProc);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

            /* 调用VOS_RegisterPIDInfo注册DSP_PID_AUDIO及消息入口函数 */
            /*uwRet = VOS_RegisterPIDInfo(DSP_PID_AUDIO, AUDIO_COMM_PidInit, AUDIO_COMM_PidProc);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }*/

            /* 调用VOS_RegisterPIDInfo注册DSP_FID_NORMAL任务优先级为低 */
            uwRet = VOS_RegisterMsgTaskPrio(DSP_FID_NORMAL, VOS_PRIORITY_P5);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

            break;
        }
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    return uwRet;

}

/*****************************************************************************
 函 数 名  : UCOM_COMM_RtFidInit
 功能描述  : 高优先级FID初始化函数，其中会进行DSP_PID_VOICE_RT的PID初始化动作
 输入参数  : enum VOS_INIT_PHASE_DEFINE enInitPhrase
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月28日
    作    者   : 陈超群
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 UCOM_COMM_RtFidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    VOS_UINT32  uwRet = VOS_OK;
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* 0.调用VOS_RegisterPIDInfo注册DSP_PID_VOICE_RT及消息入口函数 */
            uwRet = VOS_RegisterPIDInfo(DSP_PID_VOICE_RT, VOICE_ApiRtPidInit, VOICE_ApiRtPidProc);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

            /* 0.调用VOS_RegisterPIDInfo注册DSP_PID_AUDIO_RT及消息入口函数 */
            /*uwRet = VOS_RegisterPIDInfo(DSP_PID_AUDIO_RT, AUDIO_COMM_RtPidInit, AUDIO_COMM_RtPidProc);
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }*/

            /* 0.调用VOS_RegisterPIDInfo注册DSP_FID_RT任务优先级为高 */
            uwRet = VOS_RegisterMsgTaskPrio(DSP_FID_RT, VOS_PRIORITY_P6 );
            if( VOS_OK != uwRet )
            {
                return uwRet;
            }

            /* 0.进行IDLE任务函数注册 */
            VOS_RegIdleHook(UCOM_COMM_IdleHook);

            /* 1.优先初始化OM模块 */
            OM_COMM_Init();

            /* 内存管理模块初始化 */
            UCOM_MEM_DYN_Init();

            /* HIFI内部共享段初始化 */
            UCOM_SHARE_Init();

            /* 2.相关外设初始化 */
            DRV_IPC_Init();
            DRV_MAILBOX_Init();
            DRV_SOCP_Init();
            DRV_DMA_Init();
            DRV_SIO_Init();
            DRV_TIMER_Init();

            #ifdef _OM_UART_PRINT
            DRV_UART_Init();
            #endif
#if (VOS_HIFI == VOS_CPU_TYPE)
            UCOM_NV_Init();
#endif
            UCOM_LOW_PowerInit();

            /* 8.进行ST测试时挂接中断2和7，在中断7中发送测试消息 */
            #ifdef VOS_VENUS_TEST_STUB
            xthal_set_intset(0x80000002);
            xthal_set_intclear(0x80000002);
            xthal_set_intenable(0x00000000);

            VOS_ConnectInterrupt(DRV_IPC_INT_NO_HIFI, VOS_IntHandlers_02);
            VOS_ConnectInterrupt(OS_INTR_CONNECT_07, VOS_IntHandlers_07);

            VOS_EnableInterrupt(DRV_IPC_INT_NO_HIFI);
            VOS_EnableInterrupt(OS_INTR_CONNECT_07);
            #endif

            /* 9.启动看门狗 */
            if(UCOM_GET_WATCHDOG_ENABLE())
            {
                //DRV_WATCHDOG_Start(UCOM_GET_WD_TIMEOUT_LEN());
            }

            break;
        }
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    return uwRet;

}


VOS_VOID UCOM_COMM_CopyMem(
                VOS_VOID               *pvDes,
                VOS_VOID               *pvSrc,
                VOS_UINT32              uwCount)
{
    VOS_UCHAR  *pucDes = (VOS_UCHAR*)pvDes;
    VOS_UCHAR  *pucSrc = (VOS_UCHAR*)pvSrc;

    /* 相同则不需要复制 */
    if (pvSrc == pvDes)
    {
        return;
    }
    /* DES在SRC左边, 含DES叠加在SRC头部 */
    else if (pucSrc > pucDes)
    {
        for(; pucDes < ((VOS_UCHAR*)pvDes + uwCount); pucDes++)
        {
            *pucDes = *pucSrc;
            pucSrc++;
        }
    }
    /* DES在SRC右边, 含DES叠加在尾部 */
    else
    {
        pucSrc = pucSrc + (uwCount - 1);
        for(pucDes = pucDes + (uwCount - 1); pucDes >= (VOS_UCHAR*)pvDes; pucDes--)
        {
            *pucDes = *pucSrc;
            pucSrc--;
        }
    }
}
VOS_VOID UCOM_COMM_SetMem(
                VOS_VOID               *pvDes,
                VOS_UCHAR               ucData,
                VOS_UINT32              uwCount)
{
    VOS_UINT32              i;
    VOS_UCHAR              *pucDes  = (VOS_UCHAR*)pvDes;

    /*逐byte进行赋值*/
    for(i = 0; i < uwCount; i++)
    {
       *pucDes++  = ucData;
    }
}
VOS_VOID UCOM_COMM_GetChnBit16(
                VOS_VOID               *pvDes,
                VOS_UINT32              uwSize,
                VOS_UINT32              uwChnID,
                VOS_VOID               *pvSrc,
                VOS_UINT32              uwChnNum)
{
    VOS_INT16 *pshwSrc, *pshwDes;
    VOS_UINT32  i, uwLen;

    pshwSrc     = (VOS_INT16*)pvSrc;
    pshwDes     = (VOS_INT16*)pvDes;

    uwLen = (uwSize / sizeof(VOS_INT16));

    for(i=0; i< uwLen; i++)
    {
        pshwDes[i] = pshwSrc[(uwChnNum * i) + uwChnID];
    }

}


VOS_VOID UCOM_COMM_SetChnBit16(
                VOS_VOID               *pvDes,
                VOS_UINT32              uwChnNum,
                VOS_VOID               *pvSrc,
                VOS_UINT32              uwSize,
                VOS_UINT32              uwChnID
                )
{
    VOS_INT16 *pshwSrc, *pshwDes;
    VOS_UINT32  i, uwLen;

    pshwSrc     = (VOS_INT16*)pvSrc;
    pshwDes     = (VOS_INT16*)pvDes;

    uwLen = (uwSize / sizeof(VOS_INT16));

    for(i=0; i< uwLen; i++)
    {
        pshwDes[(uwChnNum * i) + uwChnID] = pshwSrc[i];
    }

}


VOS_UINT32 UCOM_COMM_RegBitRd(
                VOS_UINT32              uwAddr,
                VOS_UINT16              uhwStartBit,
                VOS_UINT16              uhwEndBit)
{
    VOS_UINT32                          uwContent;
    VOS_UINT32                          uwTmpMask;
    VOS_UINT32                          uwTmpBit;

    /*参数异常时则读取所有32bit内容*/
    if ((uhwEndBit < uhwStartBit)
        || (uhwStartBit > UCOM_BIT31)
        || (uhwEndBit > UCOM_BIT31))
    {
        uhwStartBit = UCOM_BIT0;
        uhwEndBit   = UCOM_BIT31;
    }

    /*根据起始和结束比特生成掩码*/
    uwTmpBit        = UCOM_BIT31 - uhwEndBit;
    uwTmpMask       = UCOM_COMM_UINT32_MAX << uwTmpBit;
    uwTmpMask       = uwTmpMask >> ( uhwStartBit + uwTmpBit );
    uwTmpMask       = uwTmpMask << uhwStartBit;

    uwContent       = UCOM_RegRd(uwAddr);
    uwContent      &= uwTmpMask;
    uwContent       = uwContent >> uhwStartBit;

    return uwContent;

}

VOS_VOID UCOM_COMM_RegBitWr(
                VOS_UINT32              uwAddr,
                VOS_UINT16              uhwStartBit,
                VOS_UINT16              uhwEndBit,
                VOS_UINT32              uwContent)
{
    VOS_UINT32                          uwSetValue;
    VOS_UINT32                          uwRegContent;
    VOS_UINT32                          uwTmpMask;
    VOS_UINT32                          uwTmpBit;

    /*其它参数异常时进行异常处理*/
    if ((uhwEndBit < uhwStartBit)
        || (uhwStartBit > UCOM_BIT31)
        || (uhwEndBit > UCOM_BIT31))
    {
        return;
    }

    /*根据待写入内容和起始比特信息，生成写寄存器的数据*/
    uwSetValue      = uwContent;
    uwSetValue      = uwSetValue << uhwStartBit;

    /*根据起始和结束比特生成掩码*/
    uwTmpBit        = UCOM_BIT31 - uhwEndBit;
    uwTmpMask       = UCOM_COMM_UINT32_MAX << uwTmpBit;
    uwTmpMask       = uwTmpMask >> ( uhwStartBit + uwTmpBit);
    uwTmpMask       = uwTmpMask << uhwStartBit;

    /*先读取WBBP寄存器地址中的内容，再写入指定bit数据*/
    uwRegContent    = UCOM_RegRd(uwAddr);
    uwRegContent   &= (~uwTmpMask);
    uwSetValue     &= uwTmpMask;
    UCOM_RegWr(uwAddr, (uwRegContent | uwSetValue));

    return;
}

/*****************************************************************************
 函 数 名  : UCOM_COMM_SendDSPMsg
 功能描述  : 发送消息
 输入参数  : uwSenderPid   - 发送消息的PID
             uwReceiverPid - 接收消息的PID
             uwChannel     - 接收消息的ChannelID
             pvMsg         - 消息首地址
             uwLen         - 长度(8bit)

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID UCOM_COMM_SendDspMsg(
            VOS_UINT32              uwSenderPid,
            VOS_UINT32              uwReceiverPid,
            VOS_VOID               *pvMsg,
            VOS_UINT32              uwLen,
            VOS_VOID               *pvTail,
            VOS_UINT32              uwTailLen)
{
    VOS_UINT32                          uwRet;
    MsgBlock                           *pstSndMsg = VOS_NULL;
    VOS_UINT32                          uwMsgLen  = (uwLen + uwTailLen);

    /* 分配消息内存 */
    pstSndMsg = VOS_AllocMsg(uwSenderPid, uwMsgLen);

    if (VOS_NULL == pstSndMsg)
    {
        OM_LogError(UCOM_COMM_SendMsg_NoMem);

        /* 复位 */
        OM_LogFatalAndReset(UCOM_COMM_SendMsg_NoMem);

        return;
    }

    /* 填充消息内容 */
    pstSndMsg->uwReceiverPid        = uwReceiverPid;
    pstSndMsg->uwLength             = uwMsgLen;

    /* 拷贝待发送消息内容 */
    UCOM_MemCpy(pstSndMsg->aucValue , pvMsg, uwLen);

    /* 拷贝cmdId */
    UCOM_MemCpy((pstSndMsg->aucValue + uwLen), pvTail, uwTailLen);

    /* 发送消息 */
    uwRet = VOS_SendMsg(uwSenderPid, pstSndMsg, DRV_MAILBOX_CHANNEL_DSP_CH);

    /* 若发送消息失败,记录异常,返回错误 */
    if (VOS_OK != uwRet)
    {
        OM_LogError2(UCOM_COMM_SendMsg_Err, uwRet, pstSndMsg);

        /* 复位 */
        OM_LogFatalAndReset(UCOM_COMM_SendMsg_Err);

        return;
    }

    return;
}

/*****************************************************************************
 函 数 名  : UCOM_COMM_SendMsg
 功能描述  : 发送消息
 输入参数  : uwSenderPid   - 发送消息的PID
             uwReceiverPid - 接收消息的PID
             uwChannel     - 接收消息的ChannelID
             pvMsg         - 消息首地址
             uwLen         - 长度(8bit)

 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID UCOM_COMM_SendMsg(
                VOS_UINT32              uwSenderPid,
                VOS_UINT32              uwReceiverPid,
                VOS_UINT32              uwChannel,
                VOS_VOID               *pvMsg,
                VOS_UINT32              uwLen)
{
    VOS_UINT32                          uwRet;
    MsgBlock                           *pstSndMsg = VOS_NULL;

    /* 分配消息内存 */
    pstSndMsg = VOS_AllocMsg(uwSenderPid, uwLen);
    if (VOS_NULL == pstSndMsg)
    {
        OM_LogError(UCOM_COMM_SendMsg_NoMem);

        /* 复位 */
        OM_LogFatalAndReset(UCOM_COMM_SendMsg_NoMem);

        return;
    }

    /* 填充消息内容 */
    pstSndMsg->uwReceiverPid        = uwReceiverPid;
    pstSndMsg->uwLength             = uwLen;

    /* 拷贝待发送消息内容 */
    UCOM_MemCpy(pstSndMsg->aucValue, pvMsg, uwLen);

    /* 发送消息 */
    uwRet = VOS_SendMsg(uwSenderPid, pstSndMsg, uwChannel);

    /* 若发送消息失败,记录异常,返回错误 */
    if (VOS_OK != uwRet)
    {
        OM_LogError2(UCOM_COMM_SendMsg_Err, uwRet, pstSndMsg);

        /* 复位 */
        OM_LogFatalAndReset(UCOM_COMM_SendMsg_Err);

        return;
    }

    return;
}

/*****************************************************************************
 函 数 名  : UCOM_COMM_IdleHook
 功能描述  : IDLE任务处理函数
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月31日
    作    者   : 陈超群
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID UCOM_COMM_IdleHook(VOS_UINT32 ulVosCtxSw)
{
    VOS_CPU_SR                          srSave;

    /* 如果watchdog使能，此处喂狗 */
    if(UCOM_GET_WATCHDOG_ENABLE())
    {
        /* 锁中断 */
        srSave = VOS_SplIMP();

        /* 调用DRV_WATCHDOG_Feed()喂狗 */
        DRV_WATCHDOG_Feed();

        /* 释放中断 */
        VOS_Splx(srSave);
    }

    //AUDIO_COMM_CheckContext();

    OM_CPUVIEW_RecordStackUsage();

    UCOM_LOW_PowerTask(ulVosCtxSw);

}


VOS_VOID UCOM_COMM_Sleep(VOS_UINT32 ulSleepTimeLen)
{
    VOS_UINT32 uwTimer;

    /* 起定时器 */
    DRV_TIMER_Start(&uwTimer,
                    ulSleepTimeLen,
                    DRV_TIMER_MODE_ONESHOT,
                    VOS_NULL_PTR,
                    0);
    /* 进入WFI */
    UCOM_SET_WFI(0);
}


VOS_VOID UCOM_COMM_TimeDelayFor( VOS_UINT32 uwTimeLen )
{
    volatile VOS_UINT32                 uwTmpI;
    VOS_UINT32                          uwDelayCycleNum;

    /*根据时钟频率计算出需延迟的时钟周期*/
    uwDelayCycleNum     = (VOS_UINT32)( uwTimeLen * ( VOS_UINT32 )( ( ( UCOM_HIFI_FREQ/1000 )/UCOM_COMM_LOOP_CLOCK_PERIOD )/12 ) );

    for ( uwTmpI = 0; uwTmpI < uwDelayCycleNum; uwTmpI++ )
    {
    }

    return;
}
VOS_UINT32 UCOM_GetUncachedAddr(VOS_UINT32 uwAddr)
{
    return ((VOS_UINT32)(uwAddr) - UCOM_MEMMAP_OFFSET);
}


VOS_UINT32 UCOM_GetUnmappedAddr(VOS_UINT32 uwAddr)
{
    return ((VOS_UINT32)(uwAddr) - UCOM_CACHED2PHY_OFFSET);
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ucom_comm.c */
