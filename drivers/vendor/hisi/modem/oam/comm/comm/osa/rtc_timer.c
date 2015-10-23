/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_timer.c                                                       */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement timer                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/
/*lint -save -e537*/
#include "vos.h"
#include "v_IO.h"
#include "DrvInterface.h"
#include "product_config.h"

/* LINUX不支持 */
#if (VOS_VXWORKS == VOS_OS_VER)
#include "stdlib.h"
#endif
/*lint -restore*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_V_RTC_TIMER_C


/* the state of Timer */
#define RTC_TIMER_CTRL_BLK_RUNNIG                           0
#define RTC_TIMER_CTRL_BLK_PAUSE                            1
#define RTC_TIMER_CTRL_BLK_STOP                             2

/* the tag of  RTC_TimerCtrlBlkFree */
#define THE_FIRST_RTC_TIMER_TAG                             1
#define THE_SECOND_RTC_TIMER_TAG                            2

/* timer task's stack size */
#define RTC_TIMER_TASK_STACK_SIZE                           3072

/* 1s->1000ms */
#define RTC_ONE_SECOND                                      1000

/* 32.768K */
#define RTC_CYCLE_LENGTH                                    32.768


/* 32.768K */
#define RTC_PRECISION_CYCLE_LENGTH                          0.32768


#define RTC_TIMER_NORMAL_COUNT                              5


typedef struct RTC_TIMER_CONTROL_STRU
{
    VOS_UINT32      TimerId;/* timer ID */
    VOS_UINT32      ulUsedFlag;/* whether be used or not */
    VOS_PID         Pid;/* who allocate the timer */
    VOS_UINT32      Name;/* timer's name */
    VOS_UINT32      Para;/* timer's paremate */
    REL_TIMER_FUNC  CallBackFunc;/* timer's callback function */
    HTIMER          *phTm;/* user's pointer which point the real timer room */
    VOS_UINT32      TimeOutValueInMilliSeconds;
    VOS_UINT32      TimeOutValueInCycle;
    struct RTC_TIMER_CONTROL_STRU *next;
    struct RTC_TIMER_CONTROL_STRU *previous;
    VOS_UINT8       Mode;/* timer's mode */
    VOS_UINT8       State;/* timer's state */
    VOS_UINT8       Reserved[2];/* for 4 byte aligned */
    VOS_UINT32      ulPrecision;/* record only */
    VOS_UINT32      ulPrecisionInCycle;/* unit is 32K cycle */

#if VOS_YES == VOS_TIMER_CHECK
    VOS_UINT32      ulAllocTick;/* CPU tick of block allocation */
    VOS_UINT32      ulFileID;/* alloc file ID */
    VOS_UINT32      ulLineNo;/* alloc line no. */
#endif
    VOS_UINT32      ulBackUpTimerId;/* timer ID */
} RTC_TIMER_CONTROL_BLOCK;


typedef struct DRX_TIMER_CONTROL_STRU
{
    VOS_UINT32      ulUsedFlag;                     /* whether be used or not */
    VOS_PID         ulPid;                          /* who allocate the timer */
    VOS_UINT32      ulName;                         /* timer's name */
    VOS_UINT32      ulPara;                         /* timer's paremate */
    HTIMER         *phTm;                           /* user's pointer which point the real timer room */
    VOS_UINT32      ulTimeOutValueInMilliSeconds;   /* timer's length(ms) */
    VOS_UINT32      ulTimeOutValueSlice;            /* timer's length(32k) */
    VOS_UINT32      ulTimeEndSlice;                 /* the end slice time of timer */

#if VOS_YES == VOS_TIMER_CHECK
    VOS_UINT32      ulAllocTick;                    /* CPU tick of block allocation */
    VOS_UINT32      ulFileID;                       /* alloc file ID */
    VOS_UINT32      ulLineNo;                       /* alloc line no. */
#endif
} DRX_TIMER_CONTROL_BLOCK;

typedef struct
{
    VOS_UINT32      ulStartCount;                   /* 调底软接口起定时器次数 */
    VOS_UINT32      ulStopCount;                    /* 调底软接口停定时器次数 */
    VOS_UINT32      ulExpireCount;                  /* 收到定时器中断次数 */
    VOS_UINT32      ulStartErrCount;                /* 调底软起定时器接口返回错误次数 */
    VOS_UINT32      ulStopErrCount;                 /* 调底软停定时器接口返回错误次数 */
    VOS_UINT32      ulElapsedErrCount;              /* 调底软获取剩余时间接口次数 */
    VOS_UINT32      ulElapsedContentErrCount;       /* 调底软获取剩余时间接口返回错误次数 */
}RTC_TIMER_SOC_TIMER_INFO_STRU;

/* the number of task's control block */
VOS_UINT32                RTC_TimerCtrlBlkNumber;

/* the number of free task's control block */
VOS_UINT32                RTC_TimerIdleCtrlBlkNumber;

/* the start address of task's control block */
RTC_TIMER_CONTROL_BLOCK   *RTC_TimerCtrlBlk;

/* the start address of free task's control block list */
RTC_TIMER_CONTROL_BLOCK   *RTC_TimerIdleCtrlBlk;

/* the begin address of timer control block */
VOS_VOID                  *RTC_TimerCtrlBlkBegin;

/* the end address of timer control block */
VOS_VOID                  *RTC_TimerCtrlBlkEnd;

/* the head of the running timer list */
RTC_TIMER_CONTROL_BLOCK   *RTC_Timer_head_Ptr = VOS_NULL_PTR;

/* the task ID of timer's task */
VOS_UINT32                RTC_TimerTaskId;

/* the Min usage of timer */
VOS_UINT32                RTC_TimerMinTimerIdUsed;

/* the semaphore will be given when RTC's interrupt occures */
VOS_UINT32                RTC_SEM;

/*record start value */
VOS_UINT32                RTC_Start_Value = ELAPESD_TIME_INVAILD;

#define RTC_TIMER_CTRL_BUF_SIZE (sizeof(RTC_TIMER_CONTROL_BLOCK) * RTC_MAX_TIMER_NUMBER )

VOS_CHAR g_acRtcTimerCtrlBuf[RTC_TIMER_CTRL_BUF_SIZE];

/* 记录 RTC timer 可维可测信息 */
RTC_TIMER_SOC_TIMER_INFO_STRU g_stRtcSocTimerInfo;


/* the array of DRX timer's control block */
DRX_TIMER_CONTROL_BLOCK   g_astDRXTimerCtrlBlk[DRX_TIMER_MAX_NUMBER];

/* the semaphore will be given when DRX's interrupt occures */
VOS_UINT32                g_ulDRXSem;

/* the task ID of DRX timer's task */
VOS_UINT32                g_ulDRXTimerTaskId;


#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#define VOS_RTC_TIMER_ID  (TIMER_ACPU_OSA_ID)
#else
#define VOS_RTC_TIMER_ID  (TIMER_CCPU_OSA_ID)
#endif

#if VOS_YES == VOS_TIMER_CHECK

VOS_VOID VOS_ShowUsed32KTimerInfo( VOS_VOID );

#endif

extern VOS_VOID OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber);
extern VOS_VOID OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber, VOS_UINT32 ulSendPid, VOS_UINT32 ulRcvPid, VOS_UINT32 ulMsgName);

VOS_UINT32 g_ulstartslice = 0;      /* timer启动时的slice */
VOS_UINT32 g_ulExpectSlice = 0;     /* 预测的超时slice */
VOS_UINT32 g_ulCallbackSlice = 0;   /* 超时回调slice */

typedef struct
{
    VOS_UINT32 ulmax;                   /* 最大的偏差 */
    VOS_UINT32 timercallback;           /* timer超时的slice */
    VOS_UINT32 startslice;              /* 启动timer时的slice */
    VOS_UINT32 timerlen;                /* 启动timer时长 */
    VOS_UINT32 ulexpectslice;              /* 预测的callback slice */
}VOS_TIMER_CALLBACK_STRU;

typedef struct
{
    VOS_UINT32 ulmaxelapse;             /* 获取的耗时时长与实际的耗时时长的最大偏差 */
    VOS_UINT32 ulcurslice;              /* 当前的slice */
    VOS_UINT32 timerretelapse;          /* 从timer获取的elapse */
    VOS_UINT32 startslice;              /* 启动timer时的slice */
    VOS_UINT32 timerlen;                /* 启动timer时长 */
}VOS_TIMER_ELAPSE_STRU;

typedef struct
{
    VOS_UINT32 ulmax;                   /* 回调与任务得到执行的最大偏差 */
    VOS_UINT32 ulcurslice;              /* 当前的slice */
    VOS_UINT32 timercallback;           /* timer超时回调的slice */
}VOS_TIMER_TASK_STRU;


VOS_TIMER_CALLBACK_STRU g_stCallback;
VOS_TIMER_ELAPSE_STRU   g_stElapse;
VOS_TIMER_TASK_STRU     g_stTimerTask;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
VOS_UINT32 g_timerlpm = 0;

signed int VOS_TimerLpmCb(int x)
{
    g_timerlpm = 1;
	return 0;
}
#endif

#ifndef FLOAT_SUPPORT

/*****************************************************************************
 Function   : RTC_MUL_32_DOT_768
 Description: 乘以32.768
 Input      : ulValue -- timer's value.uint is 32K cycle.
              ulFileID -- 文件ID
              usLineNo -- 行号
 Return     : 与32.768做乘法的结果
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_MUL_32_DOT_768(VOS_UINT32 ulValue,VOS_UINT32 ulFileID,
                                 VOS_INT32 usLineNo)
{
    VOS_UINT32 ulProductHigh;
    VOS_UINT32 ulProductLow;
    VOS_UINT32 ulQuotientHigh;
    VOS_UINT32 ulQuotientLow;
    VOS_UINT32 ulRemainder;
    VOS_UINT32 ulReturn;

    ulReturn = VOS_64Multi32(0, ulValue, 32768, &ulProductHigh, &ulProductLow);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    ulReturn = VOS_64Div32(ulProductHigh, ulProductLow, 1000, &ulQuotientHigh, &ulQuotientLow, &ulRemainder);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    if ( VOS_NULL != ulQuotientHigh )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    return ulQuotientLow;
}

/*****************************************************************************
 Function   : RTC_DIV_32_DOT_768
 Description: 除以32.768
 Input      : ulValue -- timer's value.uint is 32K cycle.
              ulFileID -- 文件ID
              usLineNo -- 行号
 Return     : 与32.768做除法的结果
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_DIV_32_DOT_768(VOS_UINT32 ulValue,VOS_UINT32 ulFileID,
                                 VOS_INT32 usLineNo)
{

    VOS_UINT32 ulProductHigh;
    VOS_UINT32 ulProductLow;
    VOS_UINT32 ulQuotientHigh;
    VOS_UINT32 ulQuotientLow;
    VOS_UINT32 ulRemainder;
    VOS_UINT32 ulReturn;

    ulReturn = VOS_64Multi32(0, ulValue, 1000, &ulProductHigh, &ulProductLow);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_DIV_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    ulReturn = VOS_64Div32(ulProductHigh, ulProductLow, 32768, &ulQuotientHigh, &ulQuotientLow, &ulRemainder);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_DIV_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    if ( VOS_NULL != ulQuotientHigh )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_DIV_32_DOT_768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    return ulQuotientLow;

}

/*****************************************************************************
 Function   : RTC_MUL_DOT_32768
 Description: 乘以0.32768
 Input      : ulValue -- timer's value.uint is 32K cycle.
              ulFileID -- 文件ID
              usLineNo -- 行号
 Return     : 与0.32768做乘法的结果
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_MUL_DOT_32768(VOS_UINT32 ulValue,VOS_UINT32 ulFileID,
                                 VOS_INT32 usLineNo)
{
    VOS_UINT32 ulProductHigh;
    VOS_UINT32 ulProductLow;
    VOS_UINT32 ulQuotientHigh;
    VOS_UINT32 ulQuotientLow;
    VOS_UINT32 ulRemainder;
    VOS_UINT32 ulReturn;

    ulReturn = VOS_64Multi32(0, ulValue, 32768, &ulProductHigh, &ulProductLow);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_DOT_32768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    ulReturn = VOS_64Div32(ulProductHigh, ulProductLow, 100000, &ulQuotientHigh, &ulQuotientLow, &ulRemainder);
    if ( VOS_OK != ulReturn )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_DOT_32768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    if ( VOS_NULL != ulQuotientHigh )
    {
        DRV_SYSTEM_ERROR(RTC_FLOAT_MUL_DOT_32768, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)&ulValue, sizeof(ulValue));
        return VOS_ERR;
    }

    return ulQuotientLow;
}

#endif

/*****************************************************************************
 Function   : RTC_DualTimerIsr
 Description: ISR of DualTimer
 Input      :
 Return     :
 Other      :
 *****************************************************************************/
VOS_INT32 RTC_DualTimerIsr(VOS_INT lPara)
{
    VOS_UINT32 ulMax;
    
    g_stRtcSocTimerInfo.ulExpireCount++;

    if ( VOS_OK != DRV_TIMER_STOP((VOS_UINT32)lPara) )
    {
        g_stRtcSocTimerInfo.ulStopErrCount++;
    }

    g_ulCallbackSlice = VOS_GetSlice();

    ulMax = (g_ulCallbackSlice > g_ulExpectSlice) ?  (g_ulCallbackSlice - g_ulExpectSlice) : (g_ulExpectSlice - g_ulCallbackSlice);

    if(ulMax > g_stCallback.ulmax)
    {
        g_stCallback.ulmax          = ulMax;
        g_stCallback.timercallback  = g_ulCallbackSlice;
        g_stCallback.startslice     = g_ulstartslice;
        g_stCallback.timerlen       = RTC_Start_Value;
        g_stCallback.ulexpectslice  = g_ulExpectSlice;
    }

    VOS_SmV(RTC_SEM);

    return 0;
}

/*****************************************************************************
 Function   : StopHardTimer
 Description: stop hard timer
 Input      :
 Return     :
 Other      :
 *****************************************************************************/
VOS_VOID StopHardTimer(VOS_VOID)
{
    g_stRtcSocTimerInfo.ulStopCount++;

    if ( VOS_OK != DRV_TIMER_STOP(VOS_RTC_TIMER_ID) )
    {
        g_stRtcSocTimerInfo.ulStopErrCount++;
    }

    RTC_Start_Value = ELAPESD_TIME_INVAILD;

    return;
}

/*****************************************************************************
 Function   : StartHardTimer
 Description: start hard timer
 Input      : value -- timer's value.uint is 32K cycle.
 Return     :
 Other      :
 *****************************************************************************/
VOS_VOID StartHardTimer( VOS_UINT32 value )
{
    g_stRtcSocTimerInfo.ulStartCount++;

    StopHardTimer();

    RTC_Start_Value = value;

    if ( VOS_OK != DRV_TIMER_START(VOS_RTC_TIMER_ID, (FUNCPTR_1)RTC_DualTimerIsr, VOS_RTC_TIMER_ID, value, TIMER_ONCE_COUNT,TIMER_UNIT_NONE) )
    {
        g_stRtcSocTimerInfo.ulStartErrCount++;
    }

    g_ulstartslice = VOS_GetSlice();
    g_ulExpectSlice = g_ulstartslice + value;

    return;
}

/*****************************************************************************
 Function   : GetHardTimerElapsedTime
 Description: get the elapsed time from hard timer
 Input      :
 Return     :
 Other      :
 *****************************************************************************/
VOS_UINT32 GetHardTimerElapsedTime(VOS_VOID)
{
    VOS_UINT32 ulcurlice;
    VOS_UINT32 ultmplice;
    VOS_UINT32 ulMax;
    VOS_UINT32 ulTempValue = 0;

    if ( ELAPESD_TIME_INVAILD == RTC_Start_Value )
    {
        return 0;
    }

    if ( VOS_OK != DRV_TIMER_GET_REST_TIME(VOS_RTC_TIMER_ID, TIMER_UNIT_NONE, (VOS_UINT*)&ulTempValue) )
    {
        g_stRtcSocTimerInfo.ulElapsedErrCount++;
    }

    ulcurlice = VOS_GetSlice();
    ultmplice = RTC_Start_Value - (ulcurlice - g_ulstartslice);

    ulMax = (ultmplice > ulTempValue) ? (ultmplice - ulTempValue) : (ulTempValue - ultmplice);

    if(ulMax > g_stElapse.ulmaxelapse)
    {
        g_stElapse.ulmaxelapse      = ulMax;
        g_stElapse.ulcurslice       = ulcurlice;
        g_stElapse.timerretelapse   = ulTempValue;
        g_stElapse.startslice       = g_ulstartslice;
        g_stElapse.timerlen         = RTC_Start_Value;
    }
    if ( RTC_Start_Value < ulTempValue )
    {
        g_stRtcSocTimerInfo.ulElapsedContentErrCount++;

        return RTC_Start_Value;
    }

    return RTC_Start_Value - ulTempValue;
}

/*****************************************************************************
 Function   : RTC_TimerCtrlBlkInit
 Description: Init timer's control block
 Input      : ulTimerCtrlBlkNumber -- number
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_TimerCtrlBlkInit(VOS_VOID)
{
    VOS_UINT32                i;

    RTC_TimerCtrlBlkNumber  = RTC_MAX_TIMER_NUMBER;
    RTC_TimerIdleCtrlBlkNumber = RTC_MAX_TIMER_NUMBER;

    RTC_TimerCtrlBlk = (RTC_TIMER_CONTROL_BLOCK*)g_acRtcTimerCtrlBuf;

    RTC_TimerIdleCtrlBlk = RTC_TimerCtrlBlk;
    RTC_TimerCtrlBlkBegin = (VOS_VOID *)RTC_TimerCtrlBlk;
    RTC_TimerCtrlBlkEnd  = (VOS_VOID*)( (VOS_UINT32)(RTC_TimerCtrlBlk) +
        RTC_TimerCtrlBlkNumber * sizeof(RTC_TIMER_CONTROL_BLOCK) );

    for(i=0; i<RTC_TimerCtrlBlkNumber-1; i++)
    {
        RTC_TimerCtrlBlk[i].State        = RTC_TIMER_CTRL_BLK_STOP;
        RTC_TimerCtrlBlk[i].ulUsedFlag   = VOS_NOT_USED;
        RTC_TimerCtrlBlk[i].TimerId      = i;
        RTC_TimerCtrlBlk[i].ulBackUpTimerId = i;
        RTC_TimerCtrlBlk[i].phTm         = VOS_NULL_PTR;
        RTC_TimerCtrlBlk[i].CallBackFunc = VOS_NULL_PTR;
        RTC_TimerCtrlBlk[i].previous     = VOS_NULL_PTR;
        RTC_TimerCtrlBlk[i].next         = &(RTC_TimerCtrlBlk[i+1]);
    }

    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].State        = RTC_TIMER_CTRL_BLK_STOP;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].ulUsedFlag   = VOS_NOT_USED;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].TimerId      = RTC_TimerCtrlBlkNumber-1;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].ulBackUpTimerId = RTC_TimerCtrlBlkNumber-1;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].phTm         = VOS_NULL_PTR;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].CallBackFunc = VOS_NULL_PTR;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].previous     = VOS_NULL_PTR;
    RTC_TimerCtrlBlk[RTC_TimerCtrlBlkNumber-1].next         = VOS_NULL_PTR;

    RTC_TimerMinTimerIdUsed             = RTC_TimerCtrlBlkNumber;

    VOS_MemSet(&g_stRtcSocTimerInfo, 0x0, sizeof(RTC_TIMER_SOC_TIMER_INFO_STRU));

    if( VOS_OK != VOS_SmCreate("RTC", 0, VOS_SEMA4_FIFO, &RTC_SEM))
    {
        Print("# RTC timer creat semaphore error.\r\n");

        /*free(RTC_TimerCtrlBlk);*/

        return VOS_ERR;
    }

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    DRV_TIMER_DEBUG_REGISTER(VOS_RTC_TIMER_ID, VOS_TimerLpmCb, 0);
#endif

    return(VOS_OK);
}

/*****************************************************************************
 Function   : RTC_TimerCtrlBlkGet
 Description: allocte a block
 Input      : void
 Return     : address
 Other      :
 *****************************************************************************/
RTC_TIMER_CONTROL_BLOCK *RTC_TimerCtrlBlkGet(VOS_UINT32 ulFileID, VOS_INT32 usLineNo)
{
    /*int                      intLockLevel;*/
    RTC_TIMER_CONTROL_BLOCK  *temp_Timer_Ctrl_Ptr;

    VOS_VOID                 *pDumpBuffer;

    /*intLockLevel = VOS_SplIMP();*/

    if( 0 == RTC_TimerIdleCtrlBlkNumber )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_RTC_ERRNO_SYSTIMER_FULL);

        pDumpBuffer = (VOS_VOID*)DRV_EXCH_MEM_MALLOC(VOS_DUMP_MEM_TOTAL_SIZE);

        if (VOS_NULL_PTR == pDumpBuffer)
        {
            return((RTC_TIMER_CONTROL_BLOCK*)VOS_NULL_PTR);
        }

        VOS_MemSet(pDumpBuffer, 0, VOS_DUMP_MEM_TOTAL_SIZE);

        /* 防止拷贝内存越界，取最小值 */
        VOS_MemCpy(pDumpBuffer, (VOS_VOID *)g_acRtcTimerCtrlBuf,
                   ((VOS_DUMP_MEM_TOTAL_SIZE < RTC_TIMER_CTRL_BUF_SIZE) ? VOS_DUMP_MEM_TOTAL_SIZE : RTC_TIMER_CTRL_BUF_SIZE ));

        return((RTC_TIMER_CONTROL_BLOCK*)VOS_NULL_PTR);
    }
    else
    {
        RTC_TimerIdleCtrlBlkNumber--;

        temp_Timer_Ctrl_Ptr = RTC_TimerIdleCtrlBlk;
        temp_Timer_Ctrl_Ptr->ulUsedFlag = VOS_USED;
        RTC_TimerIdleCtrlBlk = RTC_TimerIdleCtrlBlk->next;
    }

    /*VOS_Splx(intLockLevel);*/

    /* record the usage of timer control block */
    if ( RTC_TimerIdleCtrlBlkNumber < RTC_TimerMinTimerIdUsed )
    {
        RTC_TimerMinTimerIdUsed = RTC_TimerIdleCtrlBlkNumber;
    }

    temp_Timer_Ctrl_Ptr->next = VOS_NULL_PTR;
    temp_Timer_Ctrl_Ptr->previous = VOS_NULL_PTR;

#if VOS_YES == VOS_TIMER_CHECK
        temp_Timer_Ctrl_Ptr->ulFileID = ulFileID;
        temp_Timer_Ctrl_Ptr->ulLineNo = (VOS_UINT32)usLineNo;
        temp_Timer_Ctrl_Ptr->ulAllocTick = VOS_GetSlice();
#endif

    return temp_Timer_Ctrl_Ptr;
}

/*****************************************************************************
 Function   : RTC_TimerCtrlBlkFree
 Description: free a block
 Input      : Ptr -- address
              ucTag -- where call this function.this should be deleted when release
 Return     : void
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_TimerCtrlBlkFree(RTC_TIMER_CONTROL_BLOCK *Ptr, VOS_UINT8 ucTag )
{
    /*int             intLockLevel;*/

    if ( (VOS_UINT32)Ptr < (VOS_UINT32)RTC_TimerCtrlBlkBegin
        || (VOS_UINT32)Ptr > (VOS_UINT32)RTC_TimerCtrlBlkEnd )
    {
        return VOS_ERR;
    }

    /*intLockLevel = VOS_SplIMP();*/

    if ( VOS_NOT_USED == Ptr->ulUsedFlag )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_FREE_RECEPTION);

        return VOS_RTC_ERRNO_RELTM_FREE_RECEPTION;
    }

    Ptr->ulUsedFlag = VOS_NOT_USED;
    Ptr->Reserved[0] = ucTag;
    Ptr->next = RTC_TimerIdleCtrlBlk;
    RTC_TimerIdleCtrlBlk = Ptr;

    RTC_TimerIdleCtrlBlkNumber++;

    /*VOS_Splx(intLockLevel);*/

    return VOS_OK;
}

/*****************************************************************************
 Function   : RTC_TimerTaskFunc
 Description: RTC timer task entry
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID RTC_TimerTaskFunc( VOS_UINT32 Para0, VOS_UINT32 Para1,
                            VOS_UINT32 Para2, VOS_UINT32 Para3 )
{
    RTC_TIMER_CONTROL_BLOCK     *head_Ptr;
    /* the timer control which expire */
    RTC_TIMER_CONTROL_BLOCK     *RTC_TimerCtrlBlkCurrent;
    /* the timer head control which expire */
    RTC_TIMER_CONTROL_BLOCK     *RTC_TimerCtrlBlkexpired = VOS_NULL_PTR;
    RTC_TIMER_CONTROL_BLOCK     *RTC_TimerCtrlBlkexpiredTail = VOS_NULL_PTR;
    VOS_UINT32                  ulLockLevel;
    VOS_UINT32                  ulTempCount;
    REL_TIMER_MSG               *pstExpireMsg;
    VOS_UINT32                  ulElapsedCycles;
    VOS_UINT32                  ulcurlice;

    for(;;)
    {
        /* SemTake SEM when releas*/
        /* coverity[lock] */
        (VOS_VOID)VOS_SmP(RTC_SEM, 0);

        ulcurlice = VOS_GetSlice();
        if((ulcurlice - g_ulCallbackSlice) > g_stTimerTask.ulmax)
        {
            g_stTimerTask.ulmax          = (ulcurlice - g_ulCallbackSlice);
            g_stTimerTask.ulcurslice     = ulcurlice;
            g_stTimerTask.timercallback  = g_ulCallbackSlice;
        }
        /*intLockLevel = VOS_SplIMP();*/
        VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

        ulElapsedCycles = RTC_Start_Value;
        RTC_Start_Value = ELAPESD_TIME_INVAILD;

        head_Ptr = RTC_Timer_head_Ptr;

        if ( head_Ptr != VOS_NULL_PTR)
        {
            /* sub timer value */
            head_Ptr->TimeOutValueInCycle -= ulElapsedCycles;

            ulTempCount = 0;

            /* check the left timer */
            while ( ( VOS_NULL_PTR != head_Ptr )
                && ( 0 == head_Ptr->TimeOutValueInCycle ) )
            {
                ulTempCount++;
                if ( RTC_TimerCtrlBlkNumber < ulTempCount )
                {
                    /*VOS_Splx(intLockLevel);*/
                    VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

                    LogPrint2("# RTC Timer list error.Max %d Count %d.\r\n",
                        (VOS_INT)RTC_TimerCtrlBlkNumber, (VOS_INT)ulTempCount);
                    return;
                }

                RTC_TimerCtrlBlkCurrent = head_Ptr;

                *(head_Ptr->phTm) = VOS_NULL_PTR;
                /*head_Ptr->State = RTC_TIMER_CTRL_BLK_STOP;*/

                head_Ptr = RTC_TimerCtrlBlkCurrent->next;

                RTC_TimerCtrlBlkCurrent->next = VOS_NULL_PTR;
                if ( VOS_NULL_PTR == RTC_TimerCtrlBlkexpired )
                {
                    RTC_TimerCtrlBlkexpired = RTC_TimerCtrlBlkCurrent;
                    RTC_TimerCtrlBlkexpiredTail = RTC_TimerCtrlBlkCurrent;
                }
                else
                {
                    RTC_TimerCtrlBlkexpiredTail->next = RTC_TimerCtrlBlkCurrent; /*lint !e613*/  /* [false alarm]: 屏蔽Fortify错误 */
                    RTC_TimerCtrlBlkexpiredTail = RTC_TimerCtrlBlkCurrent;
                }

                RTC_Timer_head_Ptr = head_Ptr;
            }

            if ( VOS_NULL_PTR != RTC_Timer_head_Ptr )
            {
                RTC_Timer_head_Ptr->previous = VOS_NULL_PTR;

                StartHardTimer(RTC_Timer_head_Ptr->TimeOutValueInCycle);
            }

            /*VOS_Splx(intLockLevel);*/
            VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

            while ( VOS_NULL_PTR != RTC_TimerCtrlBlkexpired )
            {
                head_Ptr = RTC_TimerCtrlBlkexpired;

                if( VOS_RELTIMER_LOOP == head_Ptr->Mode )
                {
                    if ( VOS_NULL_PTR == head_Ptr->CallBackFunc )
                    {
                       V_Start32KRelTimer( head_Ptr->phTm,
                                   head_Ptr->Pid,
                                   head_Ptr->TimeOutValueInMilliSeconds,
                                   head_Ptr->Name,
                                   head_Ptr->Para,
                                   VOS_RELTIMER_LOOP,
                                   head_Ptr->ulPrecision,
#if VOS_YES == VOS_TIMER_CHECK
                                   head_Ptr->ulFileID,
                                   (VOS_INT32)head_Ptr->ulLineNo);
#else
                                   VOS_FILE_ID,
                                   __LINE__);
#endif
                    }
                    else
                    {
                        V_Start32KCallBackRelTimer( head_Ptr->phTm,
                                   head_Ptr->Pid,
                                   head_Ptr->TimeOutValueInMilliSeconds,
                                   head_Ptr->Name,
                                   head_Ptr->Para,
                                   VOS_RELTIMER_LOOP,
                                   head_Ptr->CallBackFunc,
                                   head_Ptr->ulPrecision,
#if VOS_YES == VOS_TIMER_CHECK
                                   head_Ptr->ulFileID,
                                   (VOS_INT32)head_Ptr->ulLineNo);
#else
                                   VOS_FILE_ID,
                                   __LINE__);
#endif
                    }
                }
                
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
                if(g_timerlpm)
                {
                    g_timerlpm = 0;
                    LogPrint2("[C SR] rtc_timer pid %d, name 0x%x.\n", 
                        RTC_TimerCtrlBlkexpired->Pid, RTC_TimerCtrlBlkexpired->Name);
                }
#endif

                /* CallBackFunc需要用32位传入，所以和name互换位置保证数据不丢失 */
                OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_3, (VOS_UINT32)(RTC_TimerCtrlBlkexpired->Pid), RTC_TimerCtrlBlkexpired->Name, (VOS_UINT32)(RTC_TimerCtrlBlkexpired->CallBackFunc));

                if ( VOS_NULL_PTR == RTC_TimerCtrlBlkexpired->CallBackFunc )
                {
                    /* Alloc expires's Msg */
                    pstExpireMsg
                        = VOS_TimerPreAllocMsg(RTC_TimerCtrlBlkexpired->Pid);

                    if ( VOS_NULL_PTR != pstExpireMsg )
                    {
                        pstExpireMsg->ulName = RTC_TimerCtrlBlkexpired->Name;
                        pstExpireMsg->ulPara = RTC_TimerCtrlBlkexpired->Para;

#if (VOS_YES == VOS_TIMER_CHECK)
                        pstExpireMsg->pNext
                            = (struct REL_TIMER_MSG_STRU *)(RTC_TimerCtrlBlkexpired->ulAllocTick);
                        pstExpireMsg->pPrev
                            = (struct REL_TIMER_MSG_STRU *)VOS_GetSlice();
#endif
                        /* coverity[check_return] */
                        VOS_SendMsg(DOPRA_PID_TIMER, pstExpireMsg);

                    }
                }
                else
                {
                    RTC_TimerCtrlBlkexpired->CallBackFunc(
                        RTC_TimerCtrlBlkexpired->Para,
                        RTC_TimerCtrlBlkexpired->Name);
                }

                OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_3);

                RTC_TimerCtrlBlkexpired = RTC_TimerCtrlBlkexpired->next;

                VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

                RTC_TimerCtrlBlkFree(head_Ptr, THE_FIRST_RTC_TIMER_TAG);

                VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);
            }
        }
        else
        {
            /*VOS_Splx(intLockLevel);*/
            VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);
        }
    }
}

/*****************************************************************************
 Function   : RTC_TimerTaskCreat
 Description: create RTC timer task
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_TimerTaskCreat(VOS_VOID)
{
    VOS_UINT32 TimerArguments[4] = {0,0,0,0};

    return( VOS_CreateTask( "RTC_TIMER",
                            &RTC_TimerTaskId,
                            RTC_TimerTaskFunc,
                            COMM_RTC_TIMER_TASK_PRIO,
                            RTC_TIMER_TASK_STACK_SIZE,
                            TimerArguments) );
}

/*****************************************************************************
 Function   : RTC_Add_Timer_To_List
 Description: add a timer to list
 Input      : Timer -- the tiemr's adddress
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID RTC_Add_Timer_To_List( RTC_TIMER_CONTROL_BLOCK  *Timer)
{
    RTC_TIMER_CONTROL_BLOCK  *temp_Ptr;
    RTC_TIMER_CONTROL_BLOCK  *pre_temp_Ptr;
    /*int                      intLockLevel;*/
    VOS_UINT32               ElapsedCycles = 0;

    /* intLockLevel = VOS_SplIMP(); */

    if ( VOS_NULL_PTR == RTC_Timer_head_Ptr )
    {
        RTC_Timer_head_Ptr = Timer;
    }
    else
    {
        ElapsedCycles = GetHardTimerElapsedTime();

        Timer->TimeOutValueInCycle += ElapsedCycles;

        /*  find the location to insert */
        temp_Ptr = pre_temp_Ptr = RTC_Timer_head_Ptr;

        while ( temp_Ptr != VOS_NULL_PTR )
        {
            if (Timer->TimeOutValueInCycle >= temp_Ptr->TimeOutValueInCycle)
            {
                Timer->TimeOutValueInCycle -= temp_Ptr->TimeOutValueInCycle;

                if ( Timer->TimeOutValueInCycle <= Timer->ulPrecisionInCycle )
                {
                    /* forward adjust; do nothindg when TimeOutValueInCycle == 0 */
                    Timer->TimeOutValueInCycle = 0;

                    pre_temp_Ptr = temp_Ptr;
                    temp_Ptr = temp_Ptr->next;

                    while ( (temp_Ptr != VOS_NULL_PTR)
                        && (Timer->TimeOutValueInCycle == temp_Ptr->TimeOutValueInCycle) )
                    {
                        pre_temp_Ptr = temp_Ptr;
                        temp_Ptr = temp_Ptr->next;
                    }/* make sure the order of expiry */

                    break;
                }

                pre_temp_Ptr = temp_Ptr;
                temp_Ptr = temp_Ptr->next;
            }
            else
            {
                if ( temp_Ptr->TimeOutValueInCycle - Timer->TimeOutValueInCycle <= Timer->ulPrecisionInCycle )
                {
                    /* backward adjust */
                    Timer->TimeOutValueInCycle = 0;

                    pre_temp_Ptr = temp_Ptr;
                    temp_Ptr = temp_Ptr->next;

                    while ( (temp_Ptr != VOS_NULL_PTR)
                        && (Timer->TimeOutValueInCycle == temp_Ptr->TimeOutValueInCycle) )
                    {
                        pre_temp_Ptr = temp_Ptr;
                        temp_Ptr = temp_Ptr->next;
                    }/* make sure the order of expiry */

                    break;
                }

                /* can't adjust */
                break;
            }
        }

        /* insert timer < head timer*/
        if ( temp_Ptr == RTC_Timer_head_Ptr )
        {
            Timer->next = RTC_Timer_head_Ptr;
            RTC_Timer_head_Ptr = Timer;
        }
        else
        {
            Timer->next = temp_Ptr;
            pre_temp_Ptr->next = Timer;
            Timer->previous = pre_temp_Ptr;
        }

        if ( temp_Ptr != VOS_NULL_PTR )
        {
            temp_Ptr->TimeOutValueInCycle
                = temp_Ptr->TimeOutValueInCycle - Timer->TimeOutValueInCycle;
            temp_Ptr->previous = Timer;
        }
    }

    /* restart RTC timer */
    if ( RTC_Timer_head_Ptr == Timer)
    {
        /* judge timer value when the new timer at head */
        Timer->TimeOutValueInCycle -= ElapsedCycles;

        StartHardTimer(Timer->TimeOutValueInCycle);

    }

    /*VOS_Splx(intLockLevel);*/
}

/*****************************************************************************
 Function   : RTC_Del_Timer_From_List
 Description: del a timer from list
 Input      : Timer -- the timer's address
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID RTC_Del_Timer_From_List( RTC_TIMER_CONTROL_BLOCK  *Timer)
{
    /*int                      intLockLevel;*/
    VOS_BOOL                 bIsHead = VOS_FALSE;

    /*intLockLevel = VOS_SplIMP();*/

    /* deletet this timer from list */
    if ( Timer == RTC_Timer_head_Ptr )
    {
        bIsHead = VOS_TRUE;

        RTC_Timer_head_Ptr = Timer->next;
        if ( VOS_NULL_PTR != RTC_Timer_head_Ptr )
        {
            RTC_Timer_head_Ptr->previous = VOS_NULL_PTR;
        }
    }
    else
    {
        (Timer->previous)->next = Timer->next;
        if ( VOS_NULL_PTR != Timer->next )
        {
            (Timer->next)->previous = Timer->previous;
        }
    }

    /* adjust the time_val after this timer */
    if ( Timer->next != NULL )
    {
        Timer->next->TimeOutValueInCycle += Timer->TimeOutValueInCycle;

        if (VOS_TRUE == bIsHead)
        {
            Timer->next->TimeOutValueInCycle -= GetHardTimerElapsedTime();

            StartHardTimer(Timer->next->TimeOutValueInCycle);
        }
    }

    /* Stop timer3 if no timer */
    if ( VOS_NULL_PTR == RTC_Timer_head_Ptr )
    {
        StopHardTimer();
    }

    /*VOS_Splx(intLockLevel);*/
}

/*****************************************************************************
 Function   : R_Stop32KTimer
 Description: stop a 32K relative timer which was previously started.
 Input      : phTm -- where store the timer to be stopped
 Return     :  VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 R_Stop32KTimer( HTIMER *phTm, VOS_UINT32 ulFileID, VOS_INT32 usLineNo, VOS_TIMER_OM_EVENT_STRU *pstEvent )
{
    VOS_UINT32               TimerId;
    RTC_TIMER_CONTROL_BLOCK  *Timer;
    /*int                      intLockLevel;*/

    /*intLockLevel = VOS_SplIMP();*/

    if( VOS_NULL_PTR == *phTm )
    {
        /*VOS_Splx(intLockLevel);*/

        return VOS_OK;
    }

    if ( VOS_OK != RTC_CheckTimer(phTm, &TimerId, ulFileID, usLineNo ) )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_STOP_TIMERINVALID);
        return(VOS_RTC_ERRNO_RELTM_STOP_TIMERINVALID);
    }

    Timer = &RTC_TimerCtrlBlk[TimerId];

    /* del the timer from the running list */
    RTC_Del_Timer_From_List( Timer );

    *(Timer->phTm) = VOS_NULL_PTR;

    /* OM */
    if ( VOS_NULL_PTR != pstEvent )
    {
        pstEvent->ucMode      = Timer->Mode;
        pstEvent->Pid         = Timer->Pid;
        pstEvent->ulLength    = Timer->TimeOutValueInMilliSeconds;
        pstEvent->ulName      = Timer->Name;
        pstEvent->ulParam     = Timer->Para;
        pstEvent->enPrecision = (VOS_TIMER_PRECISION_ENUM_UINT32)Timer->ulPrecision;
    }

    /*VOS_Splx(intLockLevel);*/

    /*Timer->State = RTC_TIMER_CTRL_BLK_STOP;*/

    return RTC_TimerCtrlBlkFree(Timer, THE_SECOND_RTC_TIMER_TAG);
}

/*****************************************************************************
 Function     : R_Get32KRelTmRemainTime
 Description  : get left time
 Input Param  : phTm
 Output       : pulTime
 Return Value : VOS_OK on success or errno on failure
*****************************************************************************/
VOS_UINT32 R_Get32KRelTmRemainTime( HTIMER * phTm, VOS_UINT32 * pulTime,
                                 VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_UINT32                  TimerId;
    VOS_UINT32                  remain_value = 0;
    RTC_TIMER_CONTROL_BLOCK     *head_Ptr;
    RTC_TIMER_CONTROL_BLOCK     *temp_Ptr;
    /*int                         intLockLevel;*/
    VOS_UINT32                  ulTempValue;

    /*intLockLevel = VOS_SplIMP();*/

    if( VOS_NULL_PTR == *phTm )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_STOP_INPUTISNULL);
        return(VOS_RTC_ERRNO_RELTM_STOP_INPUTISNULL);
    }

    if ( VOS_OK != RTC_CheckTimer(phTm, &TimerId, ulFileID, usLineNo ) )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_STOP_TIMERINVALID);
        return(VOS_RTC_ERRNO_RELTM_STOP_TIMERINVALID);
    }

    head_Ptr = RTC_Timer_head_Ptr;

    while ( (VOS_NULL_PTR != head_Ptr) && (head_Ptr->TimerId != TimerId) )
    {
        remain_value += (head_Ptr->TimeOutValueInCycle);

        temp_Ptr = head_Ptr;
        head_Ptr = temp_Ptr->next;
    }

    if ( (VOS_NULL_PTR == head_Ptr) || ( head_Ptr->TimerId != TimerId) )
    {
        /*VOS_Splx(intLockLevel);*/

        return VOS_ERR;
    }
    else
    {
        remain_value += (head_Ptr->TimeOutValueInCycle);

        ulTempValue = GetHardTimerElapsedTime();

        /*VOS_Splx(intLockLevel);*/

#ifdef FLOAT_SUPPORT
        *pulTime
            = (VOS_UINT32)( (remain_value - ulTempValue) / RTC_CYCLE_LENGTH );
#else
        *pulTime
            = (VOS_UINT32)RTC_DIV_32_DOT_768((remain_value - ulTempValue), ulFileID, usLineNo);
#endif

        return(VOS_OK);
    }
}


VOS_UINT32 RTC_CheckTimer( HTIMER  *phTm, VOS_UINT32 *ulTimerID,
                           VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    RTC_TIMER_CONTROL_BLOCK  *Timer;

    if ( ((VOS_UINT_PTR)*phTm >= (VOS_UINT_PTR)RTC_TimerCtrlBlkBegin)
        && ((VOS_UINT_PTR)*phTm < (VOS_UINT_PTR)RTC_TimerCtrlBlkEnd) )
    {
        Timer = (RTC_TIMER_CONTROL_BLOCK *)(*phTm);

        if ( phTm == Timer->phTm )
        {
            *ulTimerID = Timer->ulBackUpTimerId;

            if ( Timer->ulBackUpTimerId != Timer->TimerId)
            {
                Timer->TimerId = Timer->ulBackUpTimerId;
            }

            return VOS_OK;
        }

        VOS_SIMPLE_FATAL_ERROR(RTC_CHECK_TIMER_ID);
    }
    else
    {
        DRV_SYSTEM_ERROR(RTC_CHECK_TIMER_RANG, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)phTm, 128);
    }

    return VOS_ERR;
}


/*lint -save -e958*/
/*****************************************************************************
 Function   : V_Start32KCallBackRelTimer
 Description: allocate and start a relative timer using callback function.
 Input      : Pid           -- process ID of application
              ulLength       -- expire time. unit is millsecond
              ulName         -- timer name to be pass to app as a parameter
              ulParam        -- additional parameter to be pass to app
              ucMode         -- timer work mode
                                VOS_RELTIMER_LOOP  -- start periodically
                                VOS_RELTIMER_NOLOO -- start once time
              TimeOutRoutine -- Callback function when time out
              ulPrecision    -- precision,unit is 0 - 100->0%- 100%
 Output     : phTm           -- timer pointer which system retuns to app
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 V_Start32KCallBackRelTimer( HTIMER *phTm, VOS_PID Pid, VOS_UINT32 ulLength,
    VOS_UINT32 ulName, VOS_UINT32 ulParam, VOS_UINT8 ucMode, REL_TIMER_FUNC TimeOutRoutine,
    VOS_UINT32 ulPrecision, VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    RTC_TIMER_CONTROL_BLOCK  *Timer;
    VOS_UINT32               TimerId;
    VOS_UINT32               ulLockLevel;

    /* stop the timer if exists */
    /*intLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

    if( VOS_NULL_PTR != *phTm )
    {
        if ( VOS_OK == RTC_CheckTimer(phTm, &TimerId, ulFileID, usLineNo) )
        {
            if ( VOS_OK != R_Stop32KTimer( phTm, ulFileID, usLineNo, VOS_NULL_PTR ) )
            {
                /*VOS_Splx(intLockLevel);*/
                VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

                VOS_SIMPLE_FATAL_ERROR(START_32K_CALLBACK_RELTIMER_FAIL_TO_STOP);

                return VOS_ERR;
            }
        }
    }

    Timer = RTC_TimerCtrlBlkGet(ulFileID, usLineNo);
    if(Timer == VOS_NULL_PTR)
    {
        /*VOS_Splx(intLockLevel);*/
        VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_START_MSGNOTINSTALL);

        DRV_SYSTEM_ERROR(START_32K_CALLBACK_RELTIMER_FAIL_TO_ALLOCATE, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)(&g_stRtcSocTimerInfo), sizeof(RTC_TIMER_SOC_TIMER_INFO_STRU));

        return(VOS_RTC_ERRNO_RELTM_START_MSGNOTINSTALL);
    }

    /* if(phTm != VOS_NULL_PTR)
    {
        *phTm = (HTIMER)(&(Timer->TimerId));
    }*/

    Timer->Pid                          = Pid;
    Timer->Name                         = ulName;
    Timer->Para                         = ulParam;
    Timer->Mode                         = ucMode;
    Timer->phTm                         = phTm;
    Timer->TimeOutValueInMilliSeconds   = ulLength;
    Timer->ulPrecision                  = ulPrecision;
#ifdef FLOAT_SUPPORT
    Timer->TimeOutValueInCycle = (VOS_UINT32)(RTC_CYCLE_LENGTH * ulLength);
    Timer->ulPrecisionInCycle = (VOS_UINT32)(RTC_PRECISION_CYCLE_LENGTH * ulLength * ulPrecision);
#else
    Timer->TimeOutValueInCycle = (VOS_UINT32)RTC_MUL_32_DOT_768(ulLength, ulFileID, usLineNo);
    Timer->ulPrecisionInCycle = (VOS_UINT32)RTC_MUL_DOT_32768((ulLength * ulPrecision), ulFileID, usLineNo);
#endif
    /*Timer->State                        = RTC_TIMER_CTRL_BLK_RUNNIG;*/
    Timer->CallBackFunc                 = TimeOutRoutine;
    /*Timer->next                         = VOS_NULL_PTR;*/

    *phTm = (HTIMER)(&(Timer->TimerId));

    RTC_Add_Timer_To_List( Timer );

    /*VOS_Splx(intLockLevel);*/
    VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

    return(VOS_OK);
}

/*****************************************************************************
 Function   : V_Start32KRelTimer
 Description: allocate and start a relative timer using callback function.
 Input      : Pid           -- process ID of application
              ulLength       -- expire time. unit is millsecond
              ulName         -- timer name to be pass to app as a parameter
              ulParam        -- additional parameter to be pass to app
              ucMode         -- timer work mode
                                VOS_RELTIMER_LOOP  -- start periodically
                                VOS_RELTIMER_NOLOO -- start once time
              ulPrecision    -- precision,unit is 0 - 100->0%- 100%
 Output     : phTm           -- timer pointer which system retuns to app
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 V_Start32KRelTimer( HTIMER *phTm, VOS_PID Pid, VOS_UINT32 ulLength,
    VOS_UINT32 ulName, VOS_UINT32 ulParam, VOS_UINT8 ucMode, VOS_UINT32 ulPrecision,
    VOS_UINT32 ulFileID, VOS_INT32 usLineNo)
{
    RTC_TIMER_CONTROL_BLOCK  *Timer;
    VOS_UINT32               TimerId;
    VOS_UINT32               ulLockLevel;

    /* stop the timer if exists */
    /*intLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

    if( VOS_NULL_PTR != *phTm )
    {
        if ( VOS_OK == RTC_CheckTimer( phTm, &TimerId, ulFileID, usLineNo ) )
        {
            if ( VOS_OK != R_Stop32KTimer( phTm, ulFileID, usLineNo, VOS_NULL_PTR ) )
            {
                /*VOS_Splx(intLockLevel);*/
                VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

                VOS_SIMPLE_FATAL_ERROR(START_32K_RELTIMER_FAIL_TO_STOP);

                return VOS_ERR;
            }
        }
    }

    Timer = RTC_TimerCtrlBlkGet(ulFileID, usLineNo);

    if( VOS_NULL_PTR == Timer )
    {
        /*VOS_Splx(intLockLevel);*/
        VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

        VOS_SetErrorNo(VOS_RTC_ERRNO_RELTM_START_MSGNOTINSTALL);

        DRV_SYSTEM_ERROR(START_32K_CALLBACK_RELTIMER_FAIL_TO_ALLOCATE, (VOS_INT)ulFileID, (VOS_INT)usLineNo, (VOS_CHAR *)(&g_stRtcSocTimerInfo), sizeof(RTC_TIMER_SOC_TIMER_INFO_STRU));

        return(VOS_RTC_ERRNO_RELTM_START_MSGNOTINSTALL);
    }

    /*if( VOS_NULL_PTR != phTm )
    {
        *phTm = (HTIMER)(&(Timer->TimerId));
    }*/

    Timer->Pid                          = Pid;
    Timer->Name                         = ulName;
    Timer->Para                         = ulParam;
    Timer->Mode                         = ucMode;
    Timer->phTm                         = phTm;
    Timer->TimeOutValueInMilliSeconds   = ulLength;
    Timer->ulPrecision                  = ulPrecision;
#ifdef FLOAT_SUPPORT
    Timer->TimeOutValueInCycle  = (VOS_UINT32)(RTC_CYCLE_LENGTH * ulLength);
    Timer->ulPrecisionInCycle = (VOS_UINT32)(RTC_PRECISION_CYCLE_LENGTH * ulLength * ulPrecision);
#else
    Timer->TimeOutValueInCycle = (VOS_UINT32)RTC_MUL_32_DOT_768(ulLength, ulFileID, usLineNo);
    Timer->ulPrecisionInCycle = (VOS_UINT32)RTC_MUL_DOT_32768((ulLength * ulPrecision), ulFileID, usLineNo);
#endif
    /*Timer->State                        = VOS_TIMER_CTRL_BLK_RUNNIG;*/
    Timer->CallBackFunc                 = VOS_NULL_PTR;
    /*Timer->next                         = VOS_NULL_PTR;*/

    *phTm = (HTIMER)(&(Timer->TimerId));

    /* add the timer to the running list */
    RTC_Add_Timer_To_List( Timer );

    /*VOS_Splx(intLockLevel);*/
    VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

    return(VOS_OK);
}
/*lint -restore*/

/*****************************************************************************
 Function   : R_Restart32KRelTimer
 Description: Restart a relative timer which was previously started
 Input      : phTm -- where store timer ID to be restarted
 Return     : VOS_OK on success or errno on failure.
 Other      : the properties of timer should not be changed,
              but timer ID could have been changed
 *****************************************************************************/
VOS_UINT32 R_Restart32KRelTimer( HTIMER *phTm, VOS_UINT32 ulFileID,
                                 VOS_INT32 usLineNo )
{
    VOS_UINT32               TimerId;
    RTC_TIMER_CONTROL_BLOCK  *Timer;
    /*int                      intLockLevel;*/

    /*intLockLevel = VOS_SplIMP();*/

    if( VOS_NULL_PTR == *phTm )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_ERRNO_RELTM_RESTART_TIMERINVALID);

        VOS_SIMPLE_FATAL_ERROR(RESTART_32K_RELTIMER_NULL);

        return(VOS_ERRNO_RELTM_RESTART_TIMERINVALID);
    }

    if ( VOS_OK != RTC_CheckTimer( phTm, &TimerId, ulFileID, usLineNo ) )
    {
        /*VOS_Splx(intLockLevel);*/

        VOS_SetErrorNo(VOS_ERRNO_RELTM_RESTART_TIMERINVALID);

        VOS_SIMPLE_FATAL_ERROR(RESTART_32K_RELTIMER_FAIL_TO_CHECK);

        return(VOS_ERRNO_RELTM_RESTART_TIMERINVALID);
    }

    Timer   = &RTC_TimerCtrlBlk[TimerId];

    /* Del the old timer but not free timer control block */
    RTC_Del_Timer_From_List( Timer );

    /* reset timer value */
#ifdef FLOAT_SUPPORT
    Timer->TimeOutValueInCycle
        = (VOS_UINT32)(RTC_CYCLE_LENGTH * Timer->TimeOutValueInMilliSeconds);
#else
    Timer->TimeOutValueInCycle = (VOS_UINT32)RTC_MUL_32_DOT_768(Timer->TimeOutValueInMilliSeconds, ulFileID, usLineNo);
#endif

    Timer->next = VOS_NULL_PTR;
    Timer->previous = VOS_NULL_PTR;

    /* add the new timer to list */
    RTC_Add_Timer_To_List( Timer );

    /*VOS_Splx(intLockLevel);*/

    return VOS_OK;
}

/*****************************************************************************
 Function   : RTC_timer_running
 Description: a APP RTC timer is running or not
 Input      : void
 Return     : true or false
 Other      :
 *****************************************************************************/
VOS_UINT32 RTC_timer_running( VOS_VOID )
{
    RTC_TIMER_CONTROL_BLOCK     *head_Ptr;
    VOS_UINT32                   ulLockLevel;

    /*intLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

    head_Ptr = RTC_Timer_head_Ptr;

    if ( head_Ptr != VOS_NULL_PTR)
    {
        while ( VOS_NULL_PTR != head_Ptr )
        {
            if ( APP_PID != head_Ptr->Pid )
            {
                head_Ptr = head_Ptr->next;
            }
            else
            {
                /*VOS_Splx(intLockLevel);*/
                VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

                return VOS_TRUE;
            }
        }

        /*VOS_Splx(intLockLevel);*/
        VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

        return VOS_FALSE;
    }
    else
    {
        /*VOS_Splx(intLockLevel);*/
        VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

        return VOS_FALSE;
    }
}

/*****************************************************************************
 Function   : VOS_GetFirst32KTimer
 Description: get left time of the first timer.Unit is ms
 Input      : void
 Return     :
 Other      :
 *****************************************************************************/
 #if 0
VOS_UINT32 VOS_GetFirst32KTimer( VOS_VOID )
{
    int         intLockLevel;
    VOS_UINT32  ulTempValue = 0;
    VOS_UINT32  ulElapsedValue = 0;

    intLockLevel = VOS_SplIMP();

    if ( VOS_NULL_PTR != RTC_Timer_head_Ptr )
    {
        ulTempValue = RTC_Timer_head_Ptr->TimeOutValueInCycle;

        ulElapsedValue = GetHardTimerElapsedTime();
    }

    VOS_Splx(intLockLevel);

    return (VOS_UINT32)( (ulTempValue - ulElapsedValue) / RTC_CYCLE_LENGTH );

}
 #endif

/*****************************************************************************
 Function   : VOS_GetSlice
 Description: get left time of the first timer.Unit is 30us
 Input      : void
 Return     :
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_GetSlice(VOS_VOID)
{
    /* Add for L Only version */
    if (BOARD_TYPE_LTE_ONLY == BSP_OM_GetBoardType())
    {
        return VOS_GetTick();
    }
    /* Add for L Only version */

#if (VOS_WIN32 != VOS_OS_VER)
    return DRV_GET_SLICE();
#else
    return VOS_GetTick();
#endif
}

/*****************************************************************************
 Function   : RTC_CalcTimerInfo
 Description: print the usage info of timer
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_BOOL RTC_CalcTimerInfo(VOS_VOID)
{
    if ( RTC_UPPER_TIMER_NUMBER > RTC_TimerMinTimerIdUsed )
    {
        RTC_TimerMinTimerIdUsed = RTC_TimerCtrlBlkNumber;
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
#if VOS_YES == VOS_TIMER_CHECK

/*****************************************************************************
 Function   : VOS_ShowUsed32KTimerInfo
 Description: print the usage info of 32K timer's control block
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID VOS_ShowUsed32KTimerInfo( VOS_VOID )
{
    VOS_UINT32                   ulLockLevel;
    RTC_TIMER_CONTROL_BLOCK     *pstTimer;

    LogPrint("# VOS_ShowUsed32KTimerInfo:");

    /*nIntLockLevel = VOS_SplIMP();*/
    VOS_SpinLockIntLock(&g_stVosTimerSpinLock, ulLockLevel);

    pstTimer = RTC_Timer_head_Ptr;
    while( VOS_NULL_PTR != pstTimer )
    {
        LogPrint6("# F %d L %d P %d N %d R %d T %d.\r\n",
               (VOS_INT)pstTimer->ulFileID,
               (VOS_INT)pstTimer->ulLineNo,
               (VOS_INT)pstTimer->Pid,
               (VOS_INT)pstTimer->Name,
               (VOS_INT)pstTimer->Para,
               (VOS_INT)pstTimer->ulAllocTick);

        pstTimer = pstTimer->next;
    }

    /*VOS_Splx(nIntLockLevel);*/
    VOS_SpinUnlockIntUnlock(&g_stVosTimerSpinLock, ulLockLevel);

    return;
}

#endif


#if (OSA_CPU_CCPU == VOS_OSA_CPU)

/*****************************************************************************
 Function   : VOS_DrxTimerIsr
 Description: ISR of DRX Timer.
 Input      :
 Return     :
 Other      :
 *****************************************************************************/
VOS_VOID VOS_DrxTimerIsr(VOS_VOID)
{
    g_stRtcSocTimerInfo.ulStopCount++;

    if ( VOS_OK != DRV_TIMER_STOP(TIMER_CCPU_DRX_TIMER_ID) )
    {
        g_stRtcSocTimerInfo.ulStopErrCount++;
    }

    VOS_SmV(g_ulDRXSem);

    return;
}

/*****************************************************************************
 Function   : VOS_StartDrxHardTimer
 Description: Start SOC hard timer by DRV interface.
 Input      : ulValue - 32k timer length.
 Return     :
 Other      :
 *****************************************************************************/
VOS_VOID VOS_StartDrxHardTimer(VOS_UINT32 ulValue)
{
    g_stRtcSocTimerInfo.ulStopCount++;
    if ( VOS_OK != DRV_TIMER_STOP(TIMER_CCPU_DRX_TIMER_ID) )
    {
        g_stRtcSocTimerInfo.ulStopErrCount++;
    }

    g_stRtcSocTimerInfo.ulStartCount++;
    if ( VOS_OK != DRV_TIMER_START(TIMER_CCPU_DRX_TIMER_ID, (FUNCPTR_1)VOS_DrxTimerIsr, TIMER_CCPU_DRX_TIMER_ID, ulValue, TIMER_ONCE_COUNT,TIMER_UNIT_NONE) )
    {
        g_stRtcSocTimerInfo.ulStartErrCount++;
    }

    return;
}

/*****************************************************************************
 Function   : VOS_DrxTimerCtrlBlkInit
 Description: Init timer's control block
 Input      : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_DrxTimerCtrlBlkInit(VOS_VOID)
{
    VOS_UINT32                i;

    for (i = 0; i < DRX_TIMER_MAX_NUMBER; i++)
    {
        g_astDRXTimerCtrlBlk[i].ulUsedFlag = DRX_TIMER_NOT_USED_FLAG;
    }

    if( VOS_OK != VOS_SmCreate("DRX", 0, VOS_SEMA4_FIFO, &g_ulDRXSem))
    {
        LogPrint("# VOS_DrxTimerCtrlBlkInit: create semaphore error.\r\n");

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_GetNextDrxTimer
 Description: Get the shortest timer near now.
 Input      : ulCurSlice - the current time.
 Return     : the length of the shortest timer, or VOS_NULL_DWORD indicates no timer.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_GetNextDrxTimer(VOS_UINT32 ulCurSlice)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulInterval;
    VOS_UINT32                          ulMinValue;

    ulMinValue = VOS_NULL_DWORD;

    for (i = 0; i < DRX_TIMER_MAX_NUMBER; i++)
    {
        if (DRX_TIMER_USED_FLAG == g_astDRXTimerCtrlBlk[i].ulUsedFlag)
        {
            ulInterval = g_astDRXTimerCtrlBlk[i].ulTimeEndSlice - ulCurSlice;

            /* The interval must be smaller than timer's length. */
            if (g_astDRXTimerCtrlBlk[i].ulTimeOutValueSlice >= ulInterval)
            {
                if (ulMinValue > ulInterval)
                {
                    ulMinValue = ulInterval;
                }
            }
        }
    }

    return ulMinValue;
}

/*****************************************************************************
 Function   : VOS_DrxTimerTaskFunc
 Description: DRX timer task entry
 Input      : Parameters are unuseful.
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID VOS_DrxTimerTaskFunc( VOS_UINT32 Para0, VOS_UINT32 Para1,
                            VOS_UINT32 Para2, VOS_UINT32 Para3 )
{
    VOS_INT                             lLockLevel;
    VOS_UINT32                          ulCurSlice;
    VOS_UINT32                          i;
    VOS_UINT32                          ulNextTimer;
    REL_TIMER_MSG                      *pstExpireMsg;

    /* coverity[no_escape] */
    for(;;)
    {
        /* SemTake SEM when release */
        (VOS_VOID)VOS_SmP(g_ulDRXSem, 0);

        lLockLevel = VOS_SplIMP();

        ulCurSlice = VOS_GetSlice();

        for (i = 0; i < DRX_TIMER_MAX_NUMBER; i++)
        {
            if (DRX_TIMER_USED_FLAG == g_astDRXTimerCtrlBlk[i].ulUsedFlag)
            {
                /* Check timer is timeout.*/
                if (DRX_TIMER_TIMEOUT_INTERVAL >= (ulCurSlice - g_astDRXTimerCtrlBlk[i].ulTimeEndSlice))
                {
                    /* timer is timeout then notify user by sending msg */
                    pstExpireMsg = VOS_TimerPreAllocMsg(g_astDRXTimerCtrlBlk[i].ulPid);

                    if ( VOS_NULL_PTR != pstExpireMsg )
                    {
                        pstExpireMsg->ulName = g_astDRXTimerCtrlBlk[i].ulName;
                        pstExpireMsg->ulPara = g_astDRXTimerCtrlBlk[i].ulPara;

#if (VOS_YES == VOS_TIMER_CHECK)
                        pstExpireMsg->pNext
                            = (struct REL_TIMER_MSG_STRU *)(g_astDRXTimerCtrlBlk[i].ulAllocTick);
                        pstExpireMsg->pPrev
                            = (struct REL_TIMER_MSG_STRU *)ulCurSlice;
#endif
                        (VOS_VOID)VOS_SendMsg(DOPRA_PID_TIMER, pstExpireMsg);
                    }

                    *(g_astDRXTimerCtrlBlk[i].phTm)    = VOS_NULL_PTR;
                    g_astDRXTimerCtrlBlk[i].ulUsedFlag = DRX_TIMER_NOT_USED_FLAG;
                }
            }
        }

        ulNextTimer = VOS_GetNextDrxTimer(ulCurSlice);

        if (VOS_NULL_DWORD != ulNextTimer)
        {
            VOS_StartDrxHardTimer(ulNextTimer);
        }

        VOS_Splx(lLockLevel);
    }

    return; /*lint !e527*/
}


/*****************************************************************************
 Function   : VOS_DrxTimerTaskCreat
 Description: create DRX timer task
 Input      : void
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_DrxTimerTaskCreat(VOS_VOID)
{
    VOS_UINT32 TimerArguments[4] = {0,0,0,0};

    return( VOS_CreateTask( "DRX_TIMER",
                            &g_ulDRXTimerTaskId,
                            VOS_DrxTimerTaskFunc,
                            COMM_RTC_TIMER_TASK_PRIO,
                            RTC_TIMER_TASK_STACK_SIZE,
                            TimerArguments) );
}

/*****************************************************************************
 Function   : V_StopDrxTimer
 Description: stop a DRX timer which was previously started.
 Input      : phTm -- where store the timer to be stopped
 Return     :  VOS_OK on success or errno on failure
 *****************************************************************************/
VOS_UINT32 V_StopDrxTimer( HTIMER *phTm, VOS_UINT32 ulFileID, VOS_INT32 lLineNo)
{
    VOS_INT                             lLockLevel;
    DRX_TIMER_CONTROL_BLOCK            *pstTimerCtrl;
    VOS_UINT32                          ulNextTime;

    if (VOS_NULL_PTR == phTm)
    {
        VOS_SetErrorNo(VOS_ERRNO_DRXTIME_STOP_INPUTISNULL);

        return(VOS_ERRNO_DRXTIME_STOP_INPUTISNULL);
    }

    lLockLevel = VOS_SplIMP();

    if (VOS_NULL_PTR == *phTm)
    {
        VOS_Splx(lLockLevel);

        return VOS_OK;
    }

    pstTimerCtrl = (DRX_TIMER_CONTROL_BLOCK*)(*phTm); /*lint !e740*/
    *phTm        = VOS_NULL_PTR;

    if (DRX_TIMER_USED_FLAG == pstTimerCtrl->ulUsedFlag)
    {
        pstTimerCtrl->ulUsedFlag = DRX_TIMER_NOT_USED_FLAG;

        ulNextTime = VOS_GetNextDrxTimer(VOS_GetSlice());

        if (VOS_NULL_DWORD == ulNextTime)
        {
            g_stRtcSocTimerInfo.ulStopCount++;
            if ( VOS_OK != DRV_TIMER_STOP(TIMER_CCPU_DRX_TIMER_ID) )
            {
                g_stRtcSocTimerInfo.ulStopErrCount++;
            }
        }
        else
        {
            VOS_StartDrxHardTimer(ulNextTime);
        }
    }

    VOS_Splx(lLockLevel);

    return VOS_OK;
}

/*****************************************************************************
 Function   : V_StartDrxTimer
 Description: allocate and start a DRX timer.
 Input      : Pid           -- process ID of application
              ulLength       -- expire time. unit is millsecond
              ulName         -- timer name to be pass to app as a parameter
              ulParam        -- additional parameter to be pass to app
 Output     : phTm           -- timer pointer which system retuns to app
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 V_StartDrxTimer( HTIMER *phTm, VOS_PID Pid, VOS_UINT32 ulLength,
    VOS_UINT32 ulName, VOS_UINT32 ulParam, VOS_UINT32 ulFileID, VOS_INT32 lLineNo)

{
    VOS_UINT32                          i;
    VOS_INT                             lLockLevel;
    VOS_UINT32                          ulCurSlice;
    VOS_UINT32                          ulNextTime;

    if (VOS_NULL_PTR == phTm)
    {
        VOS_SetErrorNo(VOS_ERRNO_DRXTIME_START_INPUTISNULL);
        return(VOS_ERRNO_DRXTIME_START_INPUTISNULL);
    }

    lLockLevel = VOS_SplIMP();

    /* stop the timer if exists */
    if (VOS_NULL_PTR != *phTm)
    {
        if (VOS_OK != V_StopDrxTimer(phTm, ulFileID, lLineNo))
        {
            VOS_Splx(lLockLevel);

            return VOS_ERRNO_DRXTIME_START_STOP_FAIL;
        }
    }

    for(i = 0; i < DRX_TIMER_MAX_NUMBER; i++)
    {
        if (DRX_TIMER_NOT_USED_FLAG == g_astDRXTimerCtrlBlk[i].ulUsedFlag)
        {
            break;
        }
    }

    /* All DRX timer are used. */
    if (DRX_TIMER_MAX_NUMBER == i)
    {
        VOS_Splx(lLockLevel);

        VOS_SetErrorNo(VOS_ERRNO_DRXTIME_START_MSGNOTINSTALL);

        VOS_ProtectionReboot(VOS_ERRNO_DRXTIME_START_MSGNOTINSTALL, (VOS_INT)Pid, (VOS_INT)ulName, (VOS_CHAR*)g_astDRXTimerCtrlBlk, sizeof(g_astDRXTimerCtrlBlk));

        return(VOS_ERRNO_DRXTIME_START_MSGNOTINSTALL);
    }

    g_astDRXTimerCtrlBlk[i].ulPid   = Pid;
    g_astDRXTimerCtrlBlk[i].ulName  = ulName;
    g_astDRXTimerCtrlBlk[i].ulPara  = ulParam;
    g_astDRXTimerCtrlBlk[i].phTm    = phTm;
    g_astDRXTimerCtrlBlk[i].ulTimeOutValueInMilliSeconds = ulLength;

#ifdef FLOAT_SUPPORT
    g_astDRXTimerCtrlBlk[i].ulTimeOutValueSlice = (VOS_UINT32)(RTC_CYCLE_LENGTH * ulLength);
#else
    g_astDRXTimerCtrlBlk[i].ulTimeOutValueSlice = (VOS_UINT32)RTC_MUL_32_DOT_768(ulLength, ulFileID, lLineNo);
#endif

    ulCurSlice = VOS_GetSlice();

    g_astDRXTimerCtrlBlk[i].ulTimeEndSlice = g_astDRXTimerCtrlBlk[i].ulTimeOutValueSlice + ulCurSlice;

#if VOS_YES == VOS_TIMER_CHECK
    g_astDRXTimerCtrlBlk[i].ulFileID    = ulFileID;
    g_astDRXTimerCtrlBlk[i].ulLineNo    = (VOS_UINT32)lLineNo;
    g_astDRXTimerCtrlBlk[i].ulAllocTick = VOS_GetSlice();
#endif

    g_astDRXTimerCtrlBlk[i].ulUsedFlag  = DRX_TIMER_USED_FLAG;

    *phTm = (HTIMER)(&g_astDRXTimerCtrlBlk[i]); /*lint !e740*/

    ulNextTime = VOS_GetNextDrxTimer(ulCurSlice);

    if (VOS_NULL_DWORD != ulNextTime)
    {
        VOS_StartDrxHardTimer(ulNextTime);
    }

    VOS_Splx(lLockLevel);

    return VOS_OK;
}

#endif

/*****************************************************************************
 Function   : ShowRtcTimerLog
 Description:
 Input      : VOID
 Return     : VOID
 Other      :
 *****************************************************************************/
VOS_VOID ShowRtcTimerLog( VOS_VOID )
{
    vos_printf("g_stRtcSocTimerInfo.ulStartCount = %d. (call DRV Start timer num)\r\n",
                g_stRtcSocTimerInfo.ulStartCount);
    vos_printf("g_stRtcSocTimerInfo.ulStopCount =  %d. (call DRV Stop timer num)\r\n",
                g_stRtcSocTimerInfo.ulStopCount);
    vos_printf("g_stRtcSocTimerInfo.ulExpireCount = %d. (receive DRV ISR of DualTimer num)\r\n",
                g_stRtcSocTimerInfo.ulExpireCount);
    vos_printf("g_stRtcSocTimerInfo.ulStartErrCount = %d. (call DRV Stop timer err num)\r\n",
                g_stRtcSocTimerInfo.ulStartErrCount);
    vos_printf("g_stRtcSocTimerInfo.ulStopErrCount = %d. (call DRV Start timer err num)\r\n",
                g_stRtcSocTimerInfo.ulStopErrCount);
    vos_printf("g_stRtcSocTimerInfo.ulElapsedErrCount = %d. (call DRV get rest timer num)\r\n",
                g_stRtcSocTimerInfo.ulElapsedErrCount);
    vos_printf("g_stRtcSocTimerInfo.ulElapsedContentErrCount = %d. (call DRV get rest timer err num)\r\n",
                g_stRtcSocTimerInfo.ulElapsedContentErrCount);
    vos_printf("elapse info : max 0x%x, cur slice 0x%x, ret slice 0x%x, start slice 0x%x, timer len 0x%x.\n",
        g_stElapse.ulmaxelapse, 
        g_stElapse.ulcurslice, 
        g_stElapse.timerretelapse, 
        g_stElapse.startslice, 
        g_stElapse.timerlen);
    
    vos_printf("callback info : max 0x%x, callback slice 0x%x, start slice 0x%x, timer len 0x%x, expect slice 0x%x.\n",
        g_stCallback.ulmax, 
        g_stCallback.timercallback, 
        g_stCallback.startslice, 
        g_stCallback.timerlen, 
        g_stCallback.ulexpectslice);

    vos_printf("taskproc info : max 0x%x, cur slice 0x%x, callback slice 0x%x.\n",
        g_stTimerTask.ulmax, 
        g_stTimerTask.ulcurslice, 
        g_stTimerTask.timercallback);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


