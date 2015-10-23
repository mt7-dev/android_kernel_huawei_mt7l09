



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RrmInit.h"
#include "Rrm.h"
#include "RrmCtrl.h"
#include "RrmDebug.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RRM_INIT_C

#if (FEATURE_ON == FEATURE_MULTI_MODEM)

#if (FEATURE_ON == FEATURE_DSDS)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 RRM_PidInit(enum VOS_INIT_PHASE_DEFINE enPhase)
{
    switch (enPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            /* 初始化RRM信息 */
            RRM_Init();
            break;

        default:
            break;
    }

    return VOS_OK;
}
VOS_UINT32 RRM_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase)
{
    VOS_UINT32                          ulResult = VOS_ERR;


    switch (enPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            /* 创建可维可测信号量 */
            ulResult =  RRM_MNTN_CreateMutexSem(UEPS_PID_RRM);
            if (VOS_OK != ulResult)
            {
                RRM_ERROR_LOG(UEPS_PID_RRM, "RRM_FidInit, VOS_RegisterPIDInfo Fail.");
                return VOS_ERR;
            }

            /* LOG 初始化 */
            RRM_INIT_LOG_ENT();

            /* 注册PID */
            ulResult = VOS_RegisterPIDInfo(UEPS_PID_RRM,
                                           (Init_Fun_Type)RRM_PidInit,
                                           (Msg_Fun_Type)RRM_MsgProc);
            if (VOS_OK != ulResult)
            {
                RRM_ERROR_LOG(UEPS_PID_RRM, "RRM_FidInit, VOS_RegisterPIDInfo Fail.");
                return VOS_ERR;
            }

            /* 注册任务优先级 */
            ulResult = VOS_RegisterTaskPrio(UEPS_FID_RRM, RRM_TASK_PRIO);
            if (VOS_OK != ulResult)
            {
                RRM_ERROR_LOG(UEPS_PID_RRM, "RRM_FidInit, VOS_RegisterTaskPrio Fail.");
                return VOS_ERR;
            }

            break;

        default:
            break;
    }

    return VOS_OK;
}

#else

VOS_UINT32 RRM_FidInit(enum VOS_INIT_PHASE_DEFINE enPhase)
{
    switch (enPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            break;

        default:
            break;
    }

    return VOS_OK;
}

#endif /* FEATURE_ON == FEATURE_DSDS */

#endif /* FEATURE_ON == FEATURE_MULTI_MODEM */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

