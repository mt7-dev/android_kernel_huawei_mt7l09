



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RrmLog.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RRM_LOG_C

#if (FEATURE_ON == FEATURE_DSDS)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
RRM_LOG_ENT_STRU                        g_stRrmLogEnt;     /* RRM打印实体 */


/* 互斥信号量 */
VOS_UINT32                            g_ulRrmInitLogMutexSem;
VOS_UINT32                            g_ulRrmLogSaveMutexSem;


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID RRM_MNTN_InitLogEnt(RRM_LOG_ENT_STRU *pstLogEnt)
{
    if (VOS_OK != VOS_SmP(g_ulRrmInitLogMutexSem, 0))
    {
        return;
    }

    pstLogEnt->ulCnt = 0;

    VOS_SmV(g_ulRrmInitLogMutexSem);

    return;
}


VOS_VOID RRM_MNTN_LogSave
(
    RRM_LOG_ENT_STRU               *pstLogEnt,
    VOS_UINT32                      ulPid,
    LOG_LEVEL_EN                    enLevel,
    PS_FILE_ID_DEFINE_ENUM_UINT32   enFile,
    VOS_UINT32                      ulLine,
    VOS_INT32                       lpara1,
    VOS_INT32                       lpara2,
    VOS_INT32                       lpara3,
    VOS_INT32                       lpara4
)
{
    VOS_UINT32      ulCnt;
    VOS_UINT32      ulCurrentSlice;
    VOS_UINT32      ulTimeStamp;
    VOS_UINT32      ulTimeStampTmp;

    if (VOS_OK != VOS_SmP(g_ulRrmLogSaveMutexSem, 0))
    {
        return;
    }

    ulCnt           = pstLogEnt->ulCnt;

    /* 为了与SDT里面的TimeStamp一致，根据SDT的换算方法:(0xFFFFFFFF - OM_GetSlice())%32768*100，对Slice进行换算 */
    ulCurrentSlice  = OM_GetSlice();
    /* Slice为递减时，ulCurrentSlice  = 0xFFFFFFFF - ulCurrentSlice，递增时，不用减 */

    ulTimeStampTmp  = ulCurrentSlice&0x7FFF;
    ulTimeStampTmp  = ulTimeStampTmp *100;
    ulTimeStampTmp  = ulTimeStampTmp>>15;

    ulTimeStamp     = ulCurrentSlice>>15;
    ulTimeStamp     = ulTimeStamp*100;
    ulTimeStamp     = ulTimeStamp + ulTimeStampTmp;
    if( ulCnt < RRM_LOG_RECORD_MAX_NUM )
    {
        pstLogEnt->astData[ulCnt].ulTick        = ulTimeStamp;
        pstLogEnt->astData[ulCnt].enFile        = enFile;
        pstLogEnt->astData[ulCnt].ulLine        = ulLine;
        pstLogEnt->astData[ulCnt].enLevel       = enLevel;
        pstLogEnt->astData[ulCnt].alPara[0]     = lpara1;
        pstLogEnt->astData[ulCnt].alPara[1]     = lpara2;
        pstLogEnt->astData[ulCnt].alPara[2]     = lpara3;
        pstLogEnt->astData[ulCnt].alPara[3]     = lpara4;
        ulCnt++;
        pstLogEnt->ulCnt                        = ulCnt;
    }

    VOS_SmV(g_ulRrmLogSaveMutexSem);

    /* 记录满了，自动发送 */
    if (ulCnt >= RRM_LOG_RECORD_MAX_NUM)
    {
        RRM_MNTN_LogOutput(pstLogEnt, ulPid);
    }

    return;
}
VOS_VOID RRM_MNTN_LogOutput(RRM_LOG_ENT_STRU *pstLogEnt, VOS_UINT32 ulPid)
{
    RRM_TRACE_LOG_MSG_STRU          stRrmTraceLogMsg;


    /*  避免递归死循环 */
    if ((pstLogEnt->ulCnt > RRM_LOG_RECORD_MAX_NUM) || (0 == pstLogEnt->ulCnt))
    {
        RRM_MNTN_InitLogEnt(pstLogEnt);
        return ;
    }

    stRrmTraceLogMsg.ulSenderCpuId      = VOS_LOCAL_CPUID;
    stRrmTraceLogMsg.ulSenderPid        = ulPid;
    stRrmTraceLogMsg.ulReceiverCpuId    = VOS_LOCAL_CPUID;
    stRrmTraceLogMsg.ulReceiverPid      = ulPid;
    stRrmTraceLogMsg.ulLength           = (sizeof(RRM_TRACE_LOG_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    stRrmTraceLogMsg.usMsgType          = ID_RRM_TRACE_LOG_MSG;
    stRrmTraceLogMsg.usTransId          = 0;

    DRV_RT_MEMCPY( &stRrmTraceLogMsg.stLogMsgCont, pstLogEnt, sizeof(RRM_LOG_ENT_STRU) );

    OM_TraceMsgHook(&stRrmTraceLogMsg);

    RRM_MNTN_InitLogEnt(pstLogEnt);

    return ;
}
VOS_UINT32 RRM_MNTN_CreateMutexSem(VOS_UINT32 ulPid)
{
    VOS_UINT32      ulResult;

    ulResult    = VOS_SmMCreate("INIT", VOS_SEMA4_FIFO,
                    (VOS_UINT32 *)(&g_ulRrmInitLogMutexSem));
    if (VOS_OK != ulResult)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "Create INIT g_ulRrmLogSaveMutexSem fail!");
        return VOS_ERR;
    }

    ulResult    = VOS_SmMCreate("SAVE", VOS_SEMA4_FIFO,
                    (VOS_UINT32 *)(&g_ulRrmLogSaveMutexSem));
    if (VOS_OK != ulResult)
    {
        RRM_WARNING_LOG(UEPS_PID_RRM, "Create SAVE g_ulRrmLogSaveMutexSem fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif /* FEATURE_ON == FEATURE_DSDS */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

