



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RrmTimer.h"
#include "Rrm.h"
#include "RrmDebug.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RRM_TIMER_C


#if (FEATURE_ON == FEATURE_DSDS)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 RRM_StartTimer
(
    RRM_RFID_ENUM_UINT16                enRFIDTimerId
)
{
    RRM_RFID_TIMER_CTRL_STRU           *pstRFIDTimerAddr;
    VOS_UINT32                          ulRslt;


    /*获取该状态定时器控制块地址*/
    pstRFIDTimerAddr       = RRM_GetRFIDTimerAddr(enRFIDTimerId);

    /*无法获取该定时器控制块地址，说明定时器ID非法*/
    if( VOS_NULL_PTR == pstRFIDTimerAddr)
    {
        /* 打印错误信息 */
        RRM_WARNING_LOG1(UEPS_PID_RRM, "TimerId is Error!", enRFIDTimerId);
        return VOS_ERR;
    }

    /*检查该状态定时器是否在运行，如果正在运行，先停止该定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if(VOS_NULL_PTR != pstRFIDTimerAddr->pstStaTHandle)
    {
        VOS_StopRelTimer(&(pstRFIDTimerAddr->pstStaTHandle));
    }

    /* 定时器ID设置 */
    pstRFIDTimerAddr->enRFIDTimerId = enRFIDTimerId;

    /* 定时器无参数信息，启动定时器时将定时器参数设置为0
      非循环定时器，定时器模式为VOS_RELTIMER_NOLOOP*/
    ulRslt        = VOS_StartRelTimer( &(pstRFIDTimerAddr->pstStaTHandle),
                                        UEPS_PID_RRM,
                                        RRM_TMR_DEF_LEN,
                                        pstRFIDTimerAddr->enRFIDTimerId,
                                        0,
                                        VOS_RELTIMER_NOLOOP,
                                        VOS_TIMER_PRECISION_0);

    /*这里调用的是VOS的接口函数，因此需要使用VOS的返回值进行检查*/
    if( VOS_OK != ulRslt)
    {
        /* 启动失败，打印失败信息 */
        RRM_ERROR_LOG1(UEPS_PID_RRM, "Start TimerId Failure!", enRFIDTimerId);
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 RRM_StopTimer
(
    RRM_RFID_ENUM_UINT16                enRFIDTimerId
)
{
    RRM_RFID_TIMER_CTRL_STRU           *pstRFIDTimerAddr;


    /*获取该状态定时器控制块地址*/
    pstRFIDTimerAddr       = RRM_GetRFIDTimerAddr(enRFIDTimerId);

    /*无法获取该定时器控制块，说明定时器ID非法*/
    if(VOS_NULL_PTR == pstRFIDTimerAddr)
    {
        /* 打印错误信息 */
        RRM_WARNING_LOG1(UEPS_PID_RRM, "TimerId is Error!", enRFIDTimerId);
        return VOS_ERR;
    }

    /* 定时器的ID标识是否一致，如果不一致，直接返回 */
    if (enRFIDTimerId != pstRFIDTimerAddr->enRFIDTimerId)
    {
        /* 打印错误信息 */
        RRM_WARNING_LOG2(UEPS_PID_RRM,
                       "enRFIDTimerId is <1>, RFIDTimerId is <2>!",
                       enRFIDTimerId, pstRFIDTimerAddr->enRFIDTimerId);

        return VOS_ERR;
    }

    /*检查该状态定时器是否在运行，如果正在运行，停止该定时器。
      停止定时器时，VOS会直接将该定时器句柄清除为VOS_NULL_PTR*/
    if(VOS_NULL_PTR != pstRFIDTimerAddr->pstStaTHandle)
    {
        VOS_StopRelTimer(&(pstRFIDTimerAddr->pstStaTHandle));

        /* Timer ID清除 */
        pstRFIDTimerAddr->enRFIDTimerId = RRM_RFID_BUTT;
    }

    return VOS_OK;
}

#endif /* FEATURE_ON == FEATURE_DSDS */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

