

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omprivate.h"
#include "OamSpecTaskDef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_OM_APP_OUTSIDE_C


/*****************************************************************************
  2 外部引用变量定义
*****************************************************************************/



VOS_VOID OM_AcpuCallBackMsgProc(MsgBlock* pMsg)
{
    OM_REQ_PACKET_STRU    *pstAppMsg;
    OM_REQUEST_PROCEDURE  *pOmFuncProc;
    VOS_UINT32            ulFuncID;

    pstAppMsg = (OM_REQ_PACKET_STRU*)pMsg->aucValue;
    ulFuncID  = (VOS_UINT32)pstAppMsg->ucModuleId;

    /*参数检测*/
    if ((OM_FUNCID_ACPU_PART_NUM >= ulFuncID) || (OM_FUNCID_MAX_NUM <= ulFuncID))
    {
        PS_LOG1(ACPU_PID_OM_CALLBACK, 0, PS_PRINT_WARNING,
                        "OM_CallBackMsgProc:ucModuleId is overflow :", (VOS_INT32)ulFuncID);

        VOS_UnreserveMsg(ACPU_PID_OM, pMsg);

        return;
    }

    if (VOS_NULL_PTR == g_astAcpuOmFuncIdProcTbl[ulFuncID].pOmFuncProc)
    {
        PS_LOG1(ACPU_PID_OM_CALLBACK, 0, PS_PRINT_WARNING,
            "OM_CallBackMsgProc:The Process Function is NULL, ucModuleId is :", (VOS_INT32)ulFuncID);

        VOS_UnreserveMsg(ACPU_PID_OM, pMsg);

        return;
    }

    pOmFuncProc = g_astAcpuOmFuncIdProcTbl[ulFuncID].pOmFuncProc;
    pOmFuncProc(pstAppMsg, OM_AcpuSendData);

    VOS_UnreserveMsg(ACPU_PID_OM, pMsg);

    return;
}
VOS_UINT32 OM_AcpuCallBackFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_OM_CALLBACK,
                        VOS_NULL_PTR,
                        (Msg_Fun_Type)OM_AcpuCallBackMsgProc);

            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            if(VOS_OK != VOS_RegisterMsgTaskPrio(ACPU_FID_OM_CALLBACK, VOS_PRIORITY_P6))
            {
                PS_LOG(ACPU_PID_OM_CALLBACK, 0, PS_PRINT_ERROR,
                    "OMCallBackFidInit:VOS_RegisterMsgTaskPrio failure !\n");

                return VOS_ERR;
            }
/*lint -e960 修改人:甘兰 */
            if((BSP_MODULE_SUPPORT == DRV_GET_WIFI_SUPPORT())
                && (VOS_TRUE == Sock_IsEnable()))
            {
                /* 接收SOCKET数据的自处理任务 */
                ulRslt = VOS_RegisterSelfTaskPrio(ACPU_FID_OM_CALLBACK,
                                 (VOS_TASK_ENTRY_TYPE)Sock_ServerTask,
                                 COMM_SOCK_SELFTASK_PRIO, RL_SOCK_TASK_STACK_SIZE);
                if ( VOS_NULL_BYTE == ulRslt )
                {
                    return VOS_ERR;
                }
            }
/*lint +e960 修改人:甘兰 */
            break;
        }

        default:
            break;
    }
    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

