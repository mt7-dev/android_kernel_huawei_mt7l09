

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MnMsgInclude.h"
#include "TafCbaProcUsim.h"

#include "Taf_Common.h"

#include "NasUsimmApi.h"

#include "MnMsgSmCommProc.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


#define    THIS_FILE_ID        PS_FILE_ID_MNMSG_USIM_PROC_C

/*****************************************************************************
  2 常量定义
*****************************************************************************/
enum MN_MSG_STUB_CFG_VALUE_ENUM
{
    MN_MSG_STUB_CFG_VALUE_RESET = 0,
    MN_MSG_STUB_CFG_VALUE_SET   = 1
};
typedef VOS_UINT8 MN_MSG_STUB_CFG_VALUE_ENUM_U8;


/*****************************************************************************
  3 类型定义
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucListSetRec;                           /*列表时,USIM返回的记录个数*/
    VOS_UINT8                           ucListSuccRec;                          /*列表时,USIM返回写入成功的记录个数*/
    VOS_UINT8                           ucDeleteSetRec;                         /*删除短信时,USIM返回的记录个数*/
    VOS_UINT8                           ucDeleteSuccRec;                        /*删除短信时,USIM返回写入成功的记录个数*/
    VOS_UINT8                           ucDeleteStaRptSetRec;                   /*删除短信状态报告时,USIM返回的记录个数*/
    VOS_UINT8                           ucDeleteStaRptSuccRec;                  /*删除短信状态报告时,USIM返回写入成功的记录个数*/
    VOS_UINT8                           ucDeleteSrvParmSetRec;                  /*删除短信参数文件EFSMSP,USIM返回的记录个数*/
    VOS_UINT8                           ucDeleteSrvParmSuccRec;                 /*删除短信参数文件EFSMSP,USIM返回写入成功的记录个数*/
}MN_MSG_USIM_RETURN_REC_STRU;

/*****************************************************************************
  4 变量定义
*****************************************************************************/
LOCAL MN_MSG_USIM_RETURN_REC_STRU      f_stMsgUsimReturnRec;
LOCAL VOS_UINT8                        f_ucMsgCurrSmRec;
VOS_UINT8                              g_ucSetEfSmsFail = 0;
LOCAL TAF_MSG_FDN_INFO_STRU            f_stMsgFdnInfo;
#if ( VOS_WIN32 == VOS_OS_VER )
VOS_UINT8                              g_ucSetEfSmsCapacity = 0;
VOS_UINT8                              g_ucSetEfSmsNoResponse = 0;
#endif
/*****************************************************************************
  5 函数实现
*****************************************************************************/

VOS_VOID MSG_PrintUsimReturnRecStru(
    MN_MSG_USIM_RETURN_REC_STRU        *pstUsimReturnRec
)
{
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucListSetRec ", (VOS_INT32)pstUsimReturnRec->ucListSetRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucListSuccRec ", (VOS_INT32)pstUsimReturnRec->ucListSuccRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteSetRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteSetRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteSuccRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteSuccRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteStaRptSetRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteStaRptSetRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteStaRptSuccRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteStaRptSuccRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteSrvParmSetRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteSrvParmSetRec);
    MN_INFO_LOG1("MSG_PrintUsimReturnRecStru: ucDeleteSrvParmSuccRec ", (VOS_INT32)pstUsimReturnRec->ucDeleteSrvParmSuccRec);
}


VOS_VOID MN_MSG_Usim_Stub(
    VOS_UINT32                          ulDestTskId,
    VOS_UINT16                          usEfId,
    VOS_UINT32                          ulResult,
    VOS_UINT8                           ucRecordNum
)
{
    PS_USIM_SET_FILE_CNF_STRU *pUsimSetFileRsp;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*)PS_ALLOC_MSG(WUEPS_PID_USIM, sizeof(PS_USIM_SET_FILE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if(VOS_NULL_PTR == pUsimSetFileRsp)
    {
        return;
    }

    pUsimSetFileRsp->ulMsgName = PS_USIM_SET_FILE_RSP;
    pUsimSetFileRsp->ulReceiverPid = ulDestTskId;
    pUsimSetFileRsp->ulReceiverCpuId = VOS_LOCAL_CPUID;       /* 设置消息组名                             */
    pUsimSetFileRsp->ulMsgName = PS_USIM_SET_FILE_RSP;        /* 设置消息名称                             */

    pUsimSetFileRsp->ulResult      = ulResult;                                             /* 读取文件的结果                           */
    pUsimSetFileRsp->ulErrorCode   = USIMM_SW_MEMORY_ERROR;
    pUsimSetFileRsp->usEfId        = usEfId;                                               /* 文件的ID                                 */
    pUsimSetFileRsp->ucRecordNum   = ucRecordNum;                                          /* linear fixed或cycle fixed类型EF记录号    */

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_USIM, pUsimSetFileRsp))
    {
        NAS_WARNING_LOG(WUEPS_PID_USIM, "MN_MSG_Usim_Stub():WARNING:SEND MSG FIAL");
    }
}
VOS_VOID MSG_SmssFileContentToMem(
    VOS_UINT8                           *pucEf,
    VOS_UINT16                          usEfLen
)
{
    MN_MSG_SMSS_INFO_STRU               stSmssInfo;

    stSmssInfo.ucLastUsedTpMr = *(pucEf);
    if (MN_MSG_SIM_MEM_FULL_SET == *(pucEf + 1))
    {
        stSmssInfo.enMemCapExcNotFlag = MN_MSG_MEM_FULL_SET;
    }
    else
    {
        stSmssInfo.enMemCapExcNotFlag = MN_MSG_MEM_FULL_UNSET;
    }
    MN_MSG_UpdateSmssInfo(VOS_TRUE, &stSmssInfo, usEfLen);

    return;
}

/*****************************************************************************
 函 数 名  : MSG_UpdateMemSmStatus
 功能描述  : 写USIM卡失败后,需要将内存中的短信状态恢复为原来的状态,该函数主要用于
 Read,List,Modify Sm等操作
 输入参数  : ucRecIndex   -  需要更新的索引位置
             ucStatus     - 需要更新内存中的状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL VOS_VOID MSG_UpdateMemSmStatus(
    VOS_UINT8                           ucRecIndex,
    VOS_UINT8                           ucStatus
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];

    PS_MEM_SET(aucSmContent,(VOS_CHAR)0XFF,MN_MSG_EFSMS_LEN);
    ulRet = MN_MSG_ReadSmsInfo(MN_MSG_MEM_STORE_SIM,ucRecIndex,aucSmContent);
    if (MN_ERR_NO_ERROR == ulRet)
    {
        aucSmContent[0] = ucStatus;
        MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
    }
}


LOCAL VOS_VOID MSG_ProcEfSmssGetFileRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_GET_FILE_CNF_STRU           *pUsimGetFileRsp;
    MN_MSG_USIM_POWER_STATE_ENUM_U8     enPowerState;
    MN_MSG_SMSS_INFO_STRU               stSmssInfo;

    pUsimGetFileRsp = (PS_USIM_GET_FILE_CNF_STRU *) pMsg;
    PS_MEM_SET(&stSmssInfo,0X00,sizeof(stSmssInfo));
    MN_MSG_GetUsimPowerState(&enPowerState);

    MN_INFO_LOG1("MSG_ProcEfSmssGetFileRsp: enPowerState is ", enPowerState);
    if (MN_MSG_USIM_POWER_WAIT_EFSMSS == enPowerState)
    {
        MN_MSG_StopTimer(MN_MSG_TID_WAIT_GET_EF_RSP);
        if (VOS_OK == pUsimGetFileRsp->ulResult)
        {
            MSG_SmssFileContentToMem(pUsimGetFileRsp->aucEf, pUsimGetFileRsp->usEfLen);
        }
        else
        {
            MN_WARN_LOG("MSG_ProcEfSmssGetFileRsp:the Rsp of Usim is Failed");
        }

        /*获取EFSMSP文件的内容*/
        MN_MSG_GetUsimParmReq(EF_SMS_SMSP_ID);
    }
    else
    {
        MN_WARN_LOG("MSG_ProcEfSmssGetFileRsp:Invalid State");
    }
}
LOCAL VOS_VOID MSG_ProcEfSmspGetFileRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_GET_FILE_CNF_STRU           *pUsimGetFileRsp;
    MN_MSG_USIM_POWER_STATE_ENUM_U8     enPowerState;

    pUsimGetFileRsp = (PS_USIM_GET_FILE_CNF_STRU*) pMsg;
    MN_MSG_GetUsimPowerState(&enPowerState);
    MN_INFO_LOG1("MSG_ProcEfSmspGetFileRsp: enPowerState is ", enPowerState);
    MN_INFO_LOG1("MSG_ProcEfSmspGetFileRsp: pUsimGetFileRsp->ulResult is ", (long)pUsimGetFileRsp->ulResult);
    if (MN_MSG_USIM_POWER_WAIT_EFSMSP == enPowerState)
    {
        MN_MSG_StopTimer(MN_MSG_TID_WAIT_GET_EF_RSP);
        if (VOS_OK == pUsimGetFileRsp->ulResult)
        {
            MN_MSG_UpdateTotalSmspInfo(VOS_TRUE,
                                       pUsimGetFileRsp->ucTotalNum,               /*文件中存储记录的个数，从 1开始为记录的内容 */
                                       (VOS_UINT8)pUsimGetFileRsp->usEfLen,
                                       pUsimGetFileRsp->aucEf);
        }
        else
        {
            MN_WARN_LOG("MSG_ProcEfSmssGetFileRsp:the Rsp of Usim is Failed");
        }
        MN_MSG_SmspInitFinish(MN_CLIENT_ALL);
        MN_MSG_GetUsimParmReq(EF_SMS_SMS_ID);
    }
    else
    {
        MN_WARN_LOG("MSG_ProcEfSmspGetFileRsp:Invalid State");
    }
    MN_INFO_LOG("MSG_ProcEfSmspGetFileRsp: step out of function.");
}
LOCAL VOS_VOID MSG_ProcEfSmsGetFileRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_GET_FILE_CNF_STRU           *pUsimGetFileRsp;
    MN_MSG_USIM_POWER_STATE_ENUM_U8     enPowerState;
    MN_MSG_USIM_EFUST_INFO_STRU         stUstInfo;
    VOS_UINT32                          ulRet;

    pUsimGetFileRsp = (PS_USIM_GET_FILE_CNF_STRU *) pMsg;
    PS_MEM_SET(&stUstInfo,0X00,sizeof(stUstInfo));
    MN_MSG_GetUsimPowerState(&enPowerState);
    MN_INFO_LOG1("MSG_ProcEfSmsGetFileRsp: enPowerState is ", enPowerState);
    if (MN_MSG_USIM_POWER_WAIT_EFSMS == enPowerState)
    {
        MN_MSG_StopTimer(MN_MSG_TID_WAIT_GET_EF_RSP);
        if (VOS_OK == pUsimGetFileRsp->ulResult)
        {
            MN_MSG_UpdateEachMemSmInfo(f_ucMsgCurrSmRec - 1,
                                       pUsimGetFileRsp->aucEf,
                                       MN_MSG_EFSMS_LEN);
        }
        else
        {
            MN_WARN_LOG("MSG_ProcEfSmsGetFileRsp:the Rsp of Usim is Failed");
        }
/* 规避USIM桩文件问题: 输出的EFSMS文件记录数大于实际输出的记录数据 */
#ifndef __PS_WIN32_RECUR__
        /*如果还有短信未读取完,获取下一条记录,否则读取文件EFSMSR的内容*/
        if (f_ucMsgCurrSmRec < MN_MSG_GetSmCapacity(MN_MSG_MEM_STORE_SIM))
        {
            f_ucMsgCurrSmRec++;
            ulRet = MN_MSG_SndUsimGetFileReq(EF_SMS_SMS_ID,f_ucMsgCurrSmRec);
            if (USIMM_API_SUCCESS == ulRet)
            {
                return;
            }
        }
        else
#endif
        {
            f_ucMsgCurrSmRec = 1;
        }
        MN_MSG_GetUsimParmReq(EF_SMS_SMSR_ID);
    }
    else
    {
        MN_WARN_LOG("MSG_ProcEfSmsGetFileRsp:Invalid State");
    }
}


LOCAL VOS_VOID MSG_ProcEfSmsrGetFileRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_GET_FILE_CNF_STRU           *pUsimGetFileRsp;
    MN_MSG_USIM_POWER_STATE_ENUM_U8     enPowerState;

    pUsimGetFileRsp = (PS_USIM_GET_FILE_CNF_STRU*) pMsg;
    MN_MSG_GetUsimPowerState(&enPowerState);
    MN_INFO_LOG1("MSG_ProcEfSmsrGetFileRsp: enPowerState is ", enPowerState);
    if (MN_MSG_USIM_POWER_WAIT_EFSMSR == enPowerState)
    {
        MN_MSG_StopTimer(MN_MSG_TID_WAIT_GET_EF_RSP);
        if (VOS_OK == pUsimGetFileRsp->ulResult)
        {
            MN_MSG_UpdateTotalSmsrInfo(VOS_TRUE,
                                       pUsimGetFileRsp->ucTotalNum,               /*文件中存储记录的个数，从 1开始为记录的内容 */
                                       pUsimGetFileRsp->usEfLen,
                                       pUsimGetFileRsp->aucEf);
        }
        else
        {
            MN_WARN_LOG("MSG_ProcEfSmsrGetFileRsp:the Rsp of Usim is Failed");
        }
        MN_MSG_GetUsimParmReq(EF_SMS_INVALID_ID);
    }
    else
    {
        MN_WARN_LOG("MSG_ProcEfSmsrGetFileRsp:Invalid State");
    }
}
LOCAL VOS_VOID  MSG_ReadSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    MN_CLIENT_ID_T                      clientId
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    MN_OPERATION_ID_T                   bcOpId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        /*广播上报USIM中短信已发生了改变*/
        bcOpId = MN_MSG_GetBCopId();
        MN_MSG_ReportSmChanged(clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
        /*将修改的已读短信恢复成未读短信的状态*/
        MSG_UpdateMemSmStatus(ucRecIndex,EF_SMS_STATUS_MT_TO_BE_READ);
        MN_WARN_LOG("MSG_ReadSaveUsimMsgRsp:Usim Returns Error");
    }
}


LOCAL VOS_VOID MSG_ListSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    MN_CLIENT_ID_T                      clientId
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    VOS_UINT8                           ucSetUsimRec;
    MN_OPERATION_ID_T                   bcOpId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    f_stMsgUsimReturnRec.ucListSetRec++;
    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        f_stMsgUsimReturnRec.ucListSuccRec++;
    }
    else
    {
        ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
        /*将修改的已读短信恢复成未读短信的状态*/
        MSG_UpdateMemSmStatus(ucRecIndex,EF_SMS_STATUS_MT_TO_BE_READ);
        MN_WARN_LOG("MSG_ListSaveUsimMsgRsp:USIM Returns Error");
    }

    ucSetUsimRec = MN_MSG_GetUsimRec(MN_MSG_SET_USIM_LIST);
    /*LIST时,所有设置USIM的记录数都返回*/
    if (f_stMsgUsimReturnRec.ucListSetRec == ucSetUsimRec)
    {
        if (f_stMsgUsimReturnRec.ucListSuccRec > 0)
        {
            /*广播上报USIM中短信已发生了改变*/
            bcOpId = MN_MSG_GetBCopId();
            MN_MSG_ReportSmChanged(clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
        }
        f_stMsgUsimReturnRec.ucListSuccRec = 0;
        f_stMsgUsimReturnRec.ucListSetRec = 0;
        MN_MSG_SetUsimRec(MN_MSG_SET_USIM_LIST,0);
    }
}
LOCAL VOS_VOID MSG_WriteSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    MN_MSG_WRITE_EVT_INFO_STRU          stWriteEvt;
    VOS_UINT8                           ucStatus;
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];
    MN_OPERATION_ID_T                   bcOpId;
    VOS_UINT32                          ulRslt;

    ucStatus = 0;


    PS_MEM_SET(&stWriteEvt,0X00,sizeof(stWriteEvt));
    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    stWriteEvt.enMemStroe = MN_MSG_MEM_STORE_SIM;
    stWriteEvt.ulIndex = ucRecIndex;
    ulRslt = MN_MSG_GetStatus(MN_MSG_MEM_STORE_SIM,ucRecIndex,&ucStatus);
    if (MN_ERR_NO_ERROR != ulRslt)
    {
        MN_WARN_LOG("MSG_WriteSaveUsimMsgRsp:MN_MSG_GetStatus Returns Error");
    }
    MN_MSG_GetAppStatus(ucStatus, &stWriteEvt.enStatus);
    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        /*上报写文件成功*/
        stWriteEvt.bSuccess = VOS_TRUE;
        stWriteEvt.ulFailCause = MN_ERR_NO_ERROR;
        MN_MSG_ReportWriteEvent(pstSmaUsimInfo->clientId,pstSmaUsimInfo->opId,&stWriteEvt,MN_MSG_EVT_WRITE);
        /*广播上报USIM中的短信发生了变化*/
        bcOpId = MN_MSG_GetBCopId();
        MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        MN_MSG_RecoverOrgSm(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmContent);
        MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
        /*上报写文件失败*/
        stWriteEvt.bSuccess = VOS_FALSE;
        if (USIMM_SW_MEMORY_ERROR == pUsimSetFileRsp->ulErrorCode)
        {
            stWriteEvt.ulFailCause = MN_ERR_CLASS_SMS_USIM_MEM_ERROR;
        }
        else
        {
            stWriteEvt.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
        }
        MN_MSG_ReportWriteEvent(pstSmaUsimInfo->clientId,pstSmaUsimInfo->opId,&stWriteEvt,MN_MSG_EVT_WRITE);
        MN_WARN_LOG("MSG_WriteSaveUsimMsgRsp:Usim Returns Error");
    }
}
LOCAL VOS_VOID MSG_DeleteSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];
    VOS_UINT8                           ucSetUsimRec;
    VOS_BOOL                            bReportFlag;
    VOS_BOOL                            bDeleteAll;
    VOS_UINT32                          ulDeleteCount;
    VOS_UINT32                          ulRet;
    MN_MSG_DELETE_EVT_INFO_STRU         stDeleteInfo;
    MN_OPERATION_ID_T                   bcOpId;


    ulDeleteCount = 0;


    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    PS_MEM_SET(&stDeleteInfo,0X00,sizeof(stDeleteInfo));

    stDeleteInfo.enDeleteType = pstSmaUsimInfo->enDeleteType;
    stDeleteInfo.enMemStore = MN_MSG_MEM_STORE_SIM;
    stDeleteInfo.ulIndex = ucRecIndex;
    if (MN_MSG_DELETE_SINGLE == pstSmaUsimInfo->enDeleteType)
    {
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            stDeleteInfo.bSuccess = VOS_TRUE;
            stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                      pstSmaUsimInfo->opId,
                                      &stDeleteInfo,
                                      MN_MSG_EVT_DELETE);
            /*广播上报USIM中短信发生了改变*/
            bcOpId = MN_MSG_GetBCopId();
            MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
            /*查看是否需要通知网侧可用内存*/
            MN_MSG_StartMemNotification();
        }
        else
        {
            MN_MSG_RecoverOrgSm(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmContent);
            MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
            stDeleteInfo.bSuccess = VOS_FALSE;
            stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                     pstSmaUsimInfo->opId,
                                     &stDeleteInfo,
                                     MN_MSG_EVT_DELETE);
        }
    }
    else
    {
        f_stMsgUsimReturnRec.ucDeleteSetRec++;
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            f_stMsgUsimReturnRec.ucDeleteSuccRec++;
            /*查看是否需要通知网侧可用内存*/
            MN_MSG_StartMemNotification();
        }
        else
        {
            /*还原原来的短信内容*/
            MN_MSG_RecoverOrgSm(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmContent);
            MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
        }
        ucSetUsimRec = MN_MSG_GetUsimRec(MN_MSG_SET_USIM_DELETE);
        if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteSetRec)
        {
            MN_MSG_SetUsimRec(MN_MSG_SET_USIM_DELETE,0);
            /*获取当前是否已上报删除短信的结果*/
            bReportFlag = MN_MSG_GetDeleteReportFlag(MN_MSG_SET_USIM_DELETE);
            if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteSuccRec)
            {
                /*删除结果已经上报,需上报USIM中短信发生了改变*/
                if (VOS_TRUE == bReportFlag)
                {
                    /*广播上报USIM中短信发生了改变*/
                    bcOpId = MN_MSG_GetBCopId();
                    MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
                }
                else
                {
                    /*短信未能全部删除完成,还需要删除*/
                    ulRet = MN_MSG_DeleteMultiSm(pstSmaUsimInfo->clientId,
                                                 pstSmaUsimInfo->opId,
                                                 MN_MSG_MEM_STORE_SIM,
                                                 pstSmaUsimInfo->enDeleteType,
                                                 &bDeleteAll,
                                                 &ulDeleteCount);
                    if (MN_ERR_NO_ERROR != ulRet)
                    {
                        /*删除失败*/
                        stDeleteInfo.bSuccess = VOS_FALSE;
                        stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
                        MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                                 pstSmaUsimInfo->opId,
                                                 &stDeleteInfo,
                                                 MN_MSG_EVT_DELETE);

                        /*本次无新的删除*/
                        if (0 == ulDeleteCount)
                        {
                            /*广播上报USIM中短信发生了改变*/
                            bcOpId = MN_MSG_GetBCopId();
                            MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
                        }
                    }
                    else if (0 == ulDeleteCount) /*表明已经完全删除*/
                    {
                       /*短信全部删除完成,上报删除成功*/
                       stDeleteInfo.bSuccess = VOS_TRUE;
                       stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
                       MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                                pstSmaUsimInfo->opId,
                                                &stDeleteInfo,
                                                MN_MSG_EVT_DELETE);
                       /*广播上报USIM中短信发生了改变*/
                       bcOpId = MN_MSG_GetBCopId();
                       MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
                    }
                    else
                    {
                    }
                }
            }
            else /*有短信删除失败,上报删除短信失败*/
            {
                if (VOS_FALSE == bReportFlag)
                {
                    stDeleteInfo.bSuccess = VOS_FALSE;
                    stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
                    MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                             pstSmaUsimInfo->opId,
                                             &stDeleteInfo,
                                             MN_MSG_EVT_DELETE);
                    MN_MSG_SetUsimRec(MN_MSG_SET_USIM_DELETE_BEGIN,0);
                    MN_MSG_SetDeleteReportFlag(MN_MSG_SET_USIM_DELETE,VOS_TRUE);
                }
                if (f_stMsgUsimReturnRec.ucDeleteSuccRec > 0)
                {
                    /*广播上报USIM中短信发生了改变*/
                    bcOpId = MN_MSG_GetBCopId();
                    MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
                }
            }
            f_stMsgUsimReturnRec.ucDeleteSetRec = 0;
            f_stMsgUsimReturnRec.ucDeleteSuccRec = 0;
        }
    }
}


LOCAL VOS_VOID MSG_ModifySaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];
    VOS_UINT8                           ucRecIndex;
    MN_MSG_MODIFY_STATUS_EVT_INFO_STRU  stModifyStatusEvt;
    MN_MSG_STATUS_TYPE_ENUM_U8          enStatus;
    MN_OPERATION_ID_T                   bcOpId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    PS_MEM_SET(aucSmContent,(VOS_CHAR)0XFF,MN_MSG_EFSMS_LEN);
    PS_MEM_SET(&stModifyStatusEvt,0X00,sizeof(stModifyStatusEvt));
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    ulRet = MN_MSG_ReadSmsInfo(MN_MSG_MEM_STORE_SIM,ucRecIndex,aucSmContent);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return;
    }
    MN_MSG_GetAppStatus(aucSmContent[0],&enStatus);
    stModifyStatusEvt.enMemStore = MN_MSG_MEM_STORE_SIM;
    stModifyStatusEvt.ulIndex = pstSmaUsimInfo->ucRecIndex;
    stModifyStatusEvt.enStatus = enStatus;
    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        stModifyStatusEvt.bSuccess = VOS_TRUE;
        stModifyStatusEvt.ulFailCause = MN_ERR_NO_ERROR;
         /*上报修改成功*/
        MN_MSG_ReportModifyEvent(pstSmaUsimInfo->clientId,
                                 pstSmaUsimInfo->opId,
                                 &stModifyStatusEvt,
                                 MN_MSG_EVT_MODIFY_STATUS);
        /*广播上报USIM中短信已发生了改变*/
        bcOpId = MN_MSG_GetBCopId();
        MN_MSG_ReportSmChanged(pstSmaUsimInfo->clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        MN_MSG_RecoverOrgSm(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmContent);
        MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
        /*上报修改失败*/
        stModifyStatusEvt.bSuccess = VOS_FALSE;
        if (USIMM_SW_MEMORY_ERROR == pUsimSetFileRsp->ulErrorCode)
        {
            stModifyStatusEvt.ulFailCause = MN_ERR_CLASS_SMS_USIM_MEM_ERROR;
        }
        else
        {
            stModifyStatusEvt.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
        }
        MN_SendClientEvent(pstSmaUsimInfo->clientId,
                           MN_CALLBACK_MSG,
                           MN_MSG_EVT_MODIFY_STATUS,
                           (VOS_VOID *)&stModifyStatusEvt);
        MN_WARN_LOG("MSG_ModifySaveUsimMsgRsp:Usim Returns Error");
    }
}
LOCAL VOS_VOID MSG_RcvSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucRecIndex;
    MN_MSG_BCD_ADDR_STRU                stScAddr;
    MN_MSG_RAW_TS_DATA_STRU             stRawData;
    VOS_UINT8                           ucStatus;
    MN_OPERATION_ID_T                   bcopId;
    MN_MSG_DELIVER_EVT_INFO_STRU        stDeliverEvt;
    MN_MSG_CONFIG_PARM_STRU             stCfgParm;
    MN_CLIENT_ID_T                      clientId;
    MN_MSG_WRITE_EFSMS_CAUSE_ENUM_U32   enEfsmsCause;

    clientId = pstSmaUsimInfo->clientId;
    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex   = pUsimSetFileRsp->ucRecordNum - 1;
    PS_MEM_SET(aucSmContent,(VOS_CHAR)0xFF,MN_MSG_EFSMS_LEN);
    PS_MEM_SET(&stCfgParm,0X00,sizeof(stCfgParm));

    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        MN_MSG_Internal_SendRpt(VOS_TRUE,0,0);
        ulRet = MN_MSG_ReadSmsInfo(MN_MSG_MEM_STORE_SIM,ucRecIndex,aucSmContent);

        if ((MN_ERR_NO_ERROR == ulRet)
         && (EF_SMS_STATUS_MT_TO_BE_READ == (aucSmContent[0] & 0x03)))
        {
            ulRet = MN_MSG_ParseEFSmContent(aucSmContent,&stScAddr,&stRawData,&ucStatus);
            if (MN_ERR_NO_ERROR != ulRet)
            {
                MN_WARN_LOG("MSG_RcvSaveUsimMsgRsp: MN_MSG_ParseEFSmContent ErrCode ");
            }
            /*广播上报收到一条新短信*/
            bcopId = MN_MSG_GetBCopId();
            stCfgParm.enRcvSmAct = MN_MSG_RCVMSG_ACT_STORE;
            stCfgParm.enSmMemStore = MN_MSG_MEM_STORE_SIM;
            MSG_BuildDeliverEvtParm(&stCfgParm,&stScAddr,&stRawData,&stDeliverEvt);
            stDeliverEvt.ulInex = pstSmaUsimInfo->ucRecIndex;
            MN_MSG_ReportRcvMsgEvent(bcopId,&stDeliverEvt);
            bcopId = MN_MSG_GetBCopId();
            MN_MSG_ReportSmChanged(clientId,bcopId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
        }
        else
        {
            MN_WARN_LOG("MSG_RcvSaveUsimMsgRsp,Not Find Proper Sm");
        }
    }
    else
    {
        MSG_UpdateMemSmStatus(ucRecIndex,EF_SMS_STATUS_FREE_SPACE);
        MN_WARN_LOG("MSG_RcvSaveUsimMsgRsp,Usim Returns Error");

        if (USIMM_SW_MEMORY_ERROR == pUsimSetFileRsp->ulErrorCode)
        {
            enEfsmsCause = MN_MSG_WRITE_EFSMS_CAUSE_MEM_PROBLEM;
            MN_MSG_UpdateSmsInfo(VOS_TRUE, 0);
        }
        else
        {
            enEfsmsCause = MN_MSG_WRITE_EFSMS_CAUSE_UNSEPCIFIC;
        }

        MN_MSG_FailToWriteEfsms(MN_MSG_MEM_STORE_SIM, enEfsmsCause);
    }
}


LOCAL VOS_VOID  MSG_SendSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    MN_CLIENT_ID_T                      clientId
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    MN_OPERATION_ID_T                   bcOpId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex   = pUsimSetFileRsp->ucRecordNum - 1;

    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        /*广播上报USIM中短信发生了改变*/
        bcOpId = MN_MSG_GetBCopId();
        MN_MSG_ReportSmChanged(clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        /*更新内存中该条短信的状态,置为空短信*/
        MSG_UpdateMemSmStatus(ucRecIndex,EF_SMS_STATUS_FREE_SPACE);
        /*更新该短信的存储区域为不存储*/
        MN_MSG_UpdateMoSaveInfo(MN_MSG_MEM_STORE_NONE,0);
        MN_WARN_LOG("MSG_RcvSaveUsimMsgRsp,Usim Returns Error");
    }
}
LOCAL VOS_VOID MSG_RcvMoRptSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    MN_CLIENT_ID_T                      clientId
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    VOS_UINT8                           aucSmContent[MN_MSG_EFSMS_LEN];
    MN_OPERATION_ID_T                   bcOpId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    PS_MEM_SET(aucSmContent,(VOS_CHAR)0XFF,MN_MSG_EFSMS_LEN);

    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        /*广播上报USIM中短信已发生了改变*/
        bcOpId = MN_MSG_GetBCopId();
        MN_MSG_ReportSmChanged(clientId,bcOpId,VOS_TRUE,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        ucRecIndex   = pUsimSetFileRsp->ucRecordNum - 1;
        /*还原内存中该索引的短信*/
        MN_MSG_RecoverOrgSm(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmContent);
        MN_MSG_UpdateEachMemSmInfo(ucRecIndex,aucSmContent,MN_MSG_EFSMS_LEN);
        MN_WARN_LOG("MSG_RcvMoRptSaveUsimMsgRsp:Usim returns Error");
    }
}


LOCAL VOS_VOID MSG_WriteEfSmspUsimRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    MN_MSG_SRV_PARM_EVT_INFO_STRU       stSrvParmEvt;
    VOS_UINT32                          ulRet;
    MN_MSG_SRV_PARAM_STRU                stSrvParm;
    MN_MSG_USIM_EFSMSP_DTL_INFO_STRU    stOrgSrvParm;
    MN_OPERATION_ID_T                   bcopId;

    PS_MEM_SET(&stSrvParmEvt,0X00,sizeof(stSrvParmEvt));
    PS_MEM_SET(&stSrvParm,0X00,sizeof(stSrvParm));
    PS_MEM_SET(&stOrgSrvParm,0X00,sizeof(stOrgSrvParm));
    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    ulRet = MN_MSG_ReadSmspInfo(MN_MSG_MEM_STORE_SIM,ucRecIndex,&stSrvParm);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return;
    }
    stSrvParmEvt.operType = MN_MSG_OPER_SET;
    stSrvParmEvt.ulIndex = ucRecIndex;
    PS_MEM_CPY(&stSrvParmEvt.stSrvParm,&stSrvParm,sizeof(stSrvParmEvt.stSrvParm));
    bcopId = MN_MSG_GetBCopId();

    if (VOS_OK == pUsimSetFileRsp->ulResult)
    {
        /*上报设置成功*/
        stSrvParmEvt.bSuccess = VOS_TRUE;
        stSrvParmEvt.ulFailCause = MN_ERR_NO_ERROR;
        MN_MSG_ReportSrvParmEvent(pstSmaUsimInfo->clientId,
                                   pstSmaUsimInfo->opId,
                                   &stSrvParmEvt,
                                   MN_MSG_EVT_WRITE_SRV_PARM);

        /*广播上报USIM中的短信参数已发生了改变*/
        MN_MSG_ReportSrvParmChanged(pstSmaUsimInfo->clientId,bcopId,MN_MSG_MEM_STORE_SIM);
    }
    else
    {
        /*恢复内存中原来的数据*/
        MN_MSG_RecoverOrgSrvParm(ucRecIndex,&stOrgSrvParm);
        MN_MSG_UpdateEachMemSmspInfo(ucRecIndex,stOrgSrvParm.bUsed,&(stOrgSrvParm.stParm));
        /*上报设置失败*/
        stSrvParmEvt.bSuccess = VOS_FALSE;
        stSrvParmEvt.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
        MN_MSG_ReportSrvParmEvent(pstSmaUsimInfo->clientId,
                                   pstSmaUsimInfo->opId,
                                   &stSrvParmEvt,
                                   MN_MSG_EVT_WRITE_SRV_PARM);
        MN_WARN_LOG("MSG_WriteEfSmspUsimRsp:Usim Returns Error" );
    }
}


LOCAL VOS_VOID MSG_DeleteEfSmspUsimRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    MN_MSG_USIM_EFSMSP_DTL_INFO_STRU    stOrgSrvParm;
    VOS_UINT8                           ucSetUsimRec;
    MN_MSG_DELETE_EVT_INFO_STRU         stDeleteInfo;
    VOS_BOOL                            bReportFlag;
    MN_OPERATION_ID_T                   bcopId;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    PS_MEM_SET(&stOrgSrvParm,0X00,sizeof(stOrgSrvParm));
    PS_MEM_SET(&stDeleteInfo,0X00,sizeof(stDeleteInfo));

    stDeleteInfo.enDeleteType = pstSmaUsimInfo->enDeleteType;
    stDeleteInfo.enMemStore = MN_MSG_MEM_STORE_SIM;
    stDeleteInfo.ulIndex = ucRecIndex;
    bcopId = MN_MSG_GetBCopId();
    if (MN_MSG_DELETE_SINGLE == pstSmaUsimInfo->enDeleteType)
    {
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            /*上报删除成功*/
            stDeleteInfo.bSuccess = VOS_TRUE;
            stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                     pstSmaUsimInfo->opId,
                                     &stDeleteInfo,
                                      MN_MSG_EVT_DELETE_SRV_PARM);
            /*上报USIM的短信参数发生了变化*/
            MN_MSG_ReportSrvParmChanged(pstSmaUsimInfo->clientId,bcopId,MN_MSG_MEM_STORE_SIM);
        }
        else
        {
            /*恢复内存中原来的数据*/
            MN_MSG_RecoverOrgSrvParm(ucRecIndex,&stOrgSrvParm);
            MN_MSG_UpdateEachMemSmspInfo(ucRecIndex,
                                         stOrgSrvParm.bUsed,
                                         &(stOrgSrvParm.stParm));

            /*上报删除失败*/
            stDeleteInfo.bSuccess = VOS_FALSE;
            stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                     pstSmaUsimInfo->opId,
                                     &stDeleteInfo,
                                     MN_MSG_EVT_DELETE_SRV_PARM);
        }
    }
    else
    {
        f_stMsgUsimReturnRec.ucDeleteSrvParmSetRec++;
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            f_stMsgUsimReturnRec.ucDeleteSrvParmSuccRec++;
        }
        else
        {
            /*恢复内存中原来的数据*/
            MN_MSG_RecoverOrgSrvParm(ucRecIndex,&stOrgSrvParm);
            MN_MSG_UpdateEachMemSmspInfo(ucRecIndex,
                                         stOrgSrvParm.bUsed,
                                         &(stOrgSrvParm.stParm));
        }

        ucSetUsimRec = MN_MSG_GetUsimRec(MN_MSG_SET_USIM_DELETE_EFSMSP);
        if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteSrvParmSetRec)
        {
            MN_MSG_SetUsimRec(MN_MSG_SET_USIM_DELETE_EFSMSP,0);
            bReportFlag = MN_MSG_GetDeleteReportFlag(MN_MSG_SET_USIM_DELETE_EFSMSP);
            if (VOS_FALSE == bReportFlag)
            {
                if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteSrvParmSuccRec)
                {
                    /*上报删除成功*/
                    stDeleteInfo.bSuccess = VOS_TRUE;
                    stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
                }
                else
                {
                    /*上报删除失败*/
                    stDeleteInfo.bSuccess = VOS_FALSE;
                    stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
                }
                MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                         pstSmaUsimInfo->opId,
                                         &stDeleteInfo,
                                         MN_MSG_EVT_DELETE_SRV_PARM);
            }
            /*上报USIM的短信参数发生了变化*/
            if (f_stMsgUsimReturnRec.ucDeleteSrvParmSuccRec > 0)
            {
                MN_MSG_ReportSrvParmChanged(pstSmaUsimInfo->clientId,bcopId,MN_MSG_MEM_STORE_SIM);
            }
            f_stMsgUsimReturnRec.ucDeleteSrvParmSetRec = 0;
            f_stMsgUsimReturnRec.ucDeleteSrvParmSuccRec = 0;
        }
    }
}


LOCAL VOS_VOID  MSG_DeleteStaRptSaveUsimMsgRsp(
    VOS_VOID                            *pMsg,
    const MN_MSG_USIM_INFO_STRU         *pstSmaUsimInfo
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;
    VOS_UINT8                           aucSmsrContent[MN_MSG_EFSMSR_LEN];
    VOS_UINT8                           ucSetUsimRec;
    VOS_BOOL                            bDeleteAll;
    VOS_UINT8                           ucDeleteCount;
    VOS_UINT32                          ulRet;
    VOS_BOOL                            bReportFlag;
    MN_MSG_DELETE_EVT_INFO_STRU         stDeleteInfo;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;
    ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    PS_MEM_SET(&stDeleteInfo,0X00,sizeof(stDeleteInfo));

    stDeleteInfo.enDeleteType = pstSmaUsimInfo->enDeleteType;
    stDeleteInfo.enMemStore = MN_MSG_MEM_STORE_SIM;
    stDeleteInfo.ulIndex = ucRecIndex;
    if (MN_MSG_DELETE_SINGLE == pstSmaUsimInfo->enDeleteType)
    {
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            stDeleteInfo.bSuccess = VOS_TRUE;
            stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                     pstSmaUsimInfo->opId,
                                     &stDeleteInfo,
                                     MN_MSG_EVT_DELETE_STARPT);
        }
        else
        {
            MN_MSG_RecoverOrgSmsr(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmsrContent);
            MN_MSG_UpdateEachMemSmsrInfo(ucRecIndex,aucSmsrContent,MN_MSG_EFSMSR_LEN);
            stDeleteInfo.bSuccess = VOS_FALSE;
            stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                     pstSmaUsimInfo->opId,
                                     &stDeleteInfo,
                                     MN_MSG_EVT_DELETE_STARPT);
        }
    }
    else
    {
        f_stMsgUsimReturnRec.ucDeleteStaRptSetRec++;
        if (VOS_OK == pUsimSetFileRsp->ulResult)
        {
            f_stMsgUsimReturnRec.ucDeleteStaRptSuccRec++;
        }
        else
        {
            /*还原原来的短信状态报告内容*/
            MN_MSG_RecoverOrgSmsr(ucRecIndex%MN_MSG_MAX_ORG_REC_NUM,aucSmsrContent);
            MN_MSG_UpdateEachMemSmsrInfo(ucRecIndex,aucSmsrContent,MN_MSG_EFSMSR_LEN);
        }
        ucSetUsimRec = MN_MSG_GetUsimRec(MN_MSG_SET_USIM_DELETE_STARPT);
        if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteStaRptSetRec)
        {
            MN_MSG_SetUsimRec(MN_MSG_SET_USIM_DELETE_STARPT,0);
            bReportFlag = MN_MSG_GetDeleteReportFlag(MN_MSG_SET_USIM_DELETE_STARPT);
            if (ucSetUsimRec == f_stMsgUsimReturnRec.ucDeleteStaRptSuccRec)
            {
                /*删除结果未能上报*/
                if (VOS_FALSE == bReportFlag)
                {
                    /*短信状态报告未能全部删除完成,还需要删除*/
                    ulRet = MN_MSG_DeleteMultiStaRpt(pstSmaUsimInfo->clientId,
                                                     pstSmaUsimInfo->opId,
                                                     MN_MSG_MEM_STORE_SIM,
                                                     &bDeleteAll,
                                                     &ucDeleteCount);
                    if (MN_ERR_NO_ERROR != ulRet)
                    {
                        /*删除失败*/
                        stDeleteInfo.bSuccess = VOS_FALSE;
                        stDeleteInfo.ulFailCause = ulRet;
                        MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                                 pstSmaUsimInfo->opId,
                                                 &stDeleteInfo,
                                                 MN_MSG_EVT_DELETE_STARPT);
                    }
                    else
                    {
                        if (0 == ucDeleteCount) /*表明已经完全删除*/
                        {
                           /*短信全部删除完成,上报删除成功*/
                            stDeleteInfo.bSuccess = VOS_TRUE;
                            stDeleteInfo.ulFailCause = MN_ERR_NO_ERROR;
                            MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                                     pstSmaUsimInfo->opId,
                                                     &stDeleteInfo,
                                                     MN_MSG_EVT_DELETE_STARPT);
                        }
                    }
                }
            }
            else /*有短信删除失败,上报删除短信失败*/
            {
                if (VOS_FALSE == bReportFlag)
                {
                    stDeleteInfo.bSuccess = VOS_FALSE;
                    stDeleteInfo.ulFailCause = MN_ERR_CLASS_SMS_UPDATE_USIM;
                    MN_MSG_ReportDeleteEvent(pstSmaUsimInfo->clientId,
                                             pstSmaUsimInfo->opId,
                                             &stDeleteInfo,
                                             MN_MSG_EVT_DELETE_STARPT);
                    MN_MSG_SetDeleteReportFlag(MN_MSG_SET_USIM_DELETE_STARPT,VOS_TRUE);
                    MN_MSG_SetUsimRec(MN_MSG_SET_USIM_DELETE_STARPT_BEGIN,0);
                }
            }
            f_stMsgUsimReturnRec.ucDeleteStaRptSetRec = 0;
            f_stMsgUsimReturnRec.ucDeleteStaRptSuccRec = 0;
        }
    }
}
LOCAL VOS_VOID MSG_SetEfSmssUsimRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU *)pMsg;
    if (VOS_OK != pUsimSetFileRsp->ulResult)
    {
        MN_WARN_LOG1("MSG_SetEfSmssUsimRsp:Usim Returns Err, ErrCode ",(long)pUsimSetFileRsp->ulResult);
    }
}

/*****************************************************************************
 函 数 名  : MN_MSG_InitParm
 功能描述  : 开机时需要初始化当前设置USIM的记录数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_InitParm(VOS_VOID)
{
    /*初始化设置USIM的记录数*/
    PS_MEM_SET(&f_stMsgUsimReturnRec,0X00,sizeof(f_stMsgUsimReturnRec));
}

/*****************************************************************************
 函 数 名  : MN_MSG_GetSmCurrRecFromUsim
 功能描述  : 获取当前向USIM取短信的记录数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8  MN_MSG_GetSmCurrRecFromUsim(VOS_VOID)
{
    MN_INFO_LOG1("MN_MSG_GetSmCurrRecFromUsim: f_ucMsgCurrSmRec", (VOS_INT32)f_ucMsgCurrSmRec);
    return f_ucMsgCurrSmRec;
}

/*****************************************************************************
 函 数 名  : MN_MSG_SetSmCurrRecFromUsim
 功能描述  : 设置当前向USIM取短信的记录数
 输入参数  : ucCurrRec  - 需要设置的记录数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  MN_MSG_SetSmCurrRecFromUsim(
    VOS_UINT8                           ucCurrRec
)
{
    f_ucMsgCurrSmRec = ucCurrRec;
}


VOS_UINT32  MN_MSG_SndUsimGetFileReq(
    VOS_UINT16                          usEfId,
    VOS_UINT8                           ucRecNum
)
{
    VOS_UINT32                          ulRet;
    USIMM_GET_FILE_INFO_STRU stGetFileInfo;

    /*调用USIM的函数获取指定文件和指定记录号的内容*/
    TAF_COMM_BUILD_USIM_GET_FILE_INFO(&stGetFileInfo, USIMM_UNLIMIT_APP, usEfId, ucRecNum);

    ulRet = NAS_USIMMAPI_GetFileReq(WUEPS_PID_TAF, 0, &stGetFileInfo);


    if (USIMM_API_SUCCESS != ulRet)
    {
        return ulRet;
    }

    /*调用USIM的接口返回成功,设置相应的状态*/
    switch (usEfId)
    {
        case EF_SMS_SMSS_ID:
            MN_MSG_UpdateUsimStateInfo(MN_MSG_USIM_POWER_WAIT_EFSMSS);
            break;
        case EF_SMS_SMSP_ID:
            MN_MSG_UpdateUsimStateInfo(MN_MSG_USIM_POWER_WAIT_EFSMSP);
            break;
        case EF_SMS_SMS_ID:
            MN_MSG_UpdateUsimStateInfo(MN_MSG_USIM_POWER_WAIT_EFSMS);
            break;
        case EF_SMS_SMSR_ID:
            MN_MSG_UpdateUsimStateInfo(MN_MSG_USIM_POWER_WAIT_EFSMSR);
            break;
        default:
            MN_WARN_LOG1("MN_MSG_SndUsimGetFileReq:Invalid EFID ",usEfId);
            return USIMM_API_WRONG_PARA;
    }

    /*返回成功,启动定时器*/
    MN_MSG_StartTimer(MN_MSG_TID_WAIT_GET_EF_RSP,0);

    return ulRet;
}
VOS_UINT32  MN_MSG_SndUsimGetMaxRecReq(
    VOS_UINT16                          usEfId
)
{
    VOS_UINT32                          ulRet;

    /*调用USIM的函数获取文件EFSMS的记录数*/
    ulRet = NAS_USIMMAPI_MaxEFRecordNumReq(WUEPS_PID_TAF, USIMM_UNLIMIT_APP, usEfId);

    /*返回成功,启动定时器并更新状态*/
    if (USIMM_API_SUCCESS == ulRet)
    {
        MN_MSG_UpdateUsimStateInfo(MN_MSG_USIM_POWER_WAIT_EFSMSREC);
        MN_MSG_StartTimer(MN_MSG_TID_WAIT_GET_EF_RSP,0);
    }
    return ulRet;
}


VOS_VOID  MN_MSG_RcvUsimGetMaxRecRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_EFMAX_CNF_STRU              *pUsimGetMaxNumMsg;
    MN_MSG_USIM_POWER_STATE_ENUM_U8     enPowerState;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulTotalRec = 0;

    pUsimGetMaxNumMsg = (PS_USIM_EFMAX_CNF_STRU*) pMsg;
    MN_MSG_GetUsimPowerState(&enPowerState);
    if (MN_MSG_USIM_POWER_WAIT_EFSMSREC == enPowerState)
    {
        MN_MSG_StopTimer(MN_MSG_TID_WAIT_GET_EF_RSP);
        if ((VOS_OK == pUsimGetMaxNumMsg->ulResult)
         && (EF_SMS_SMS_ID == pUsimGetMaxNumMsg->usEfId))
        {
#if ( VOS_WIN32 == VOS_OS_VER )
            if (0 != g_ucSetEfSmsCapacity)
            {
                ulTotalRec = g_ucSetEfSmsCapacity;
            }
            else
            {
                ulTotalRec = pUsimGetMaxNumMsg->ucRecordNum;
            }
#else
            ulTotalRec = pUsimGetMaxNumMsg->ucRecordNum;
#endif
            if (ulTotalRec > 0)
            {
                MN_MSG_UpdateSmsInfo(VOS_TRUE, (VOS_UINT8)ulTotalRec);
            }
        }
        else
        {
            MN_WARN_LOG("MN_MSG_RcvSmsGetRec:the Rsp of Usim is Failed");
        }

        if (ulTotalRec > 0)
        {
            /*短信一条一条的读取,从第一条开始读起,读取成功则等等USIM的回复*/
            f_ucMsgCurrSmRec = 1;
            ulRet = MN_MSG_SndUsimGetFileReq(EF_SMS_SMS_ID,f_ucMsgCurrSmRec);
            if (USIMM_API_SUCCESS == ulRet)
            {
                return;
            }
        }
        MN_MSG_GetUsimParmReq(EF_SMS_SMSR_ID);
    }
    else
    {
        MN_WARN_LOG("MN_MSG_RcvSmsGetRec:Invalid State");
    }
}
VOS_VOID  MN_MSG_RcvUsimGetFileRsp(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_GET_FILE_CNF_STRU           *pstUsimGetFileRsp;

    pstUsimGetFileRsp = (PS_USIM_GET_FILE_CNF_STRU*) pMsg;
    switch (pstUsimGetFileRsp->usEfId)
    {
        case EF_SMS_SMSS_ID:
            MSG_ProcEfSmssGetFileRsp(pMsg);
            break;
        case EF_SMS_SMSP_ID:
            MSG_ProcEfSmspGetFileRsp(pMsg);
            break;
        case EF_SMS_SMS_ID:
            MSG_ProcEfSmsGetFileRsp(pMsg);
            break;
        case EF_SMS_SMSR_ID:
            MSG_ProcEfSmsrGetFileRsp(pMsg);
            break;
#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

        case TAF_CBA_EF_CBMI_ID:
        case TAF_CBA_EF_CBMIR_ID:
        case TAF_CBA_EF_CBMID_ID:
            TAF_CBA_MsgProc((struct MsgCB*)pMsg);
            break;

#endif
        default:
            MN_INFO_LOG("MN_MSG_RcvUsimGetFileRsp:Rcv Invalid EFID");
            break;

    }
}


VOS_UINT32 MN_MSG_SndUsimSetFileReq(
    VOS_UINT16                          usEfId,
    VOS_UINT8                           *pucEf,
    VOS_UINT8                           ucEfLen,
    VOS_UINT8                           ucRecNum
)
{
    USIMM_SET_FILE_INFO_STRU            stSetFileInfo;

    if (VOS_FALSE == MN_MSG_IsUsimFileExist(usEfId))
    {
        MN_WARN_LOG("MN_MSG_SndUsimSetFileReq: File is inavailable.");
        return USIMM_API_FAILED;
    }

    if (MN_MSG_STUB_CFG_VALUE_SET == g_ucSetEfSmsFail)
    {
        MN_MSG_Usim_Stub(WUEPS_PID_TAF, usEfId, VOS_ERR, ucRecNum);
        return USIMM_API_SUCCESS;
    }
    else
    {

        #ifdef __PS_WIN32_RECUR__
        return USIMM_API_SUCCESS;
        #else
        TAF_COMM_BUILD_USIM_SET_FILE_INFO(&stSetFileInfo,
                                        USIMM_UNLIMIT_APP,
                                        usEfId,
                                        ucRecNum,
                                        ucEfLen,
                                        pucEf);

        return NAS_USIMMAPI_SetFileReq(WUEPS_PID_TAF, 0, &stSetFileInfo);
        #endif
    }
}


VOS_UINT32 MN_MSG_EnvelopRspRpAckWithTpdu(
    PS_USIM_ENVELOPE_CNF_STRU           *pstEnvelope
)
{
    MN_MSG_RP_ACK_STRU                  stRpAck;
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo;
    MN_MSG_DELIVER_RPT_ACK_STRU         *pstDeliverRptAck;
    MN_MSG_TP_PID_TYPE_ENUM_U8          enPid;
    MN_MSG_DCS_CODE_STRU                stDcs;
    MN_MSG_RAW_TS_DATA_STRU             *pstRawData;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucSendLen;
    VOS_UINT8                           *pucSendData;
    TAF_MSG_SIGNALLING_TYPE_ENUM_UINT32  enMsgSignallingType;

    /*1. RP-ACK消息中TPDU部分原始数据结构内存申请*/
    pstTsDataInfo = (MN_MSG_TS_DATA_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_TS_DATA_INFO_STRU));
    if (VOS_NULL_PTR == pstTsDataInfo)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }

    /*2. RP-ACK消息中TPDU部分原始数据结构填写*/
    PS_MEM_SET(pstTsDataInfo, 0x00, sizeof(MN_MSG_TS_DATA_INFO_STRU));
    pstTsDataInfo->enTpduType = MN_MSG_TPDU_DELIVER_RPT_ACK;
    pstDeliverRptAck = &pstTsDataInfo->u.stDeliverRptAck;
    pstDeliverRptAck->ucParaInd = MN_MSG_TP_PID_MASK | MN_MSG_TP_DCS_MASK | MN_MSG_TP_UDL_MASK;
    MN_MSG_GetMtTpPidAndDcs(&enPid, &stDcs);
    pstDeliverRptAck->enPid = enPid;
    PS_MEM_CPY(&pstDeliverRptAck->stDcs, &stDcs, sizeof(stDcs));
    pstDeliverRptAck->bUserDataHeaderInd = VOS_FALSE;
    pstDeliverRptAck->stUserData.ulLen = pstEnvelope->ucDataLen;
    PS_MEM_CPY(pstDeliverRptAck->stUserData.aucOrgData,
               pstEnvelope->aucData,
               pstEnvelope->ucDataLen);

    /*3. RP-ACK消息中TPDU编码内存申请*/
    pstRawData = (MN_MSG_RAW_TS_DATA_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_RAW_TS_DATA_STRU));
    if (VOS_NULL_PTR == pstRawData)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstTsDataInfo);
        MN_WARN_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }

    /*4. RP-ACK消息中TPDU编码，编码函数调用后TPDU原始数据结构已经不需要所以此处释放该内存*/
    ulRet = MN_MSG_Encode(pstTsDataInfo, pstRawData);
    PS_MEM_FREE(WUEPS_PID_TAF, pstTsDataInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
        MN_NORM_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: Fail to MN_MSG_Encode.");
        return MN_ERR_CLASS_SMS_INVALID_TPDU;
    }

    /*5. RP-ACK消息中TPDU编码长度异常退出*/
    if (pstRawData->ulLen > MN_MSG_MAX_TPDU_LEN)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
        MN_NORM_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: TPDU length is overflow.");
        return MN_ERR_CLASS_SMS_INVALID_TPDU;
    }

    /*6. RP-ACK消息中RP-ACK原始数据填写，TPDU相关消息填写完成后，TPDU编码数据结构已经不需要所以此处释放该内存*/
    PS_MEM_SET(&stRpAck,0x00,sizeof(stRpAck));
    stRpAck.ucRpUserDataLen = (VOS_UINT8)pstRawData->ulLen;
    PS_MEM_CPY(stRpAck.aucRpUserData, pstRawData->aucData, pstRawData->ulLen);
    PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
    stRpAck.ucRpMsgType = MN_MSG_RP_MTI_UP_RP_ACK;
    stRpAck.bRpUserDataExist = VOS_TRUE;
    stRpAck.ucRpUserDataIei = MN_MSG_RP_USER_DATA_IEI;

    /*7. RP-ACK消息中RP-ACK数据编码内存申请及编码*/
    pucSendData = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_TAF, MN_MSG_MAX_LEN);
    if (VOS_NULL_PTR == pucSendData)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }
    MN_MSG_EncodeRpAck(&stRpAck, pucSendData,&ucSendLen);

    enMsgSignallingType = MN_MSG_GetMtRouteStackType();
    
    /*8. RP-ACK消息中RP-ACK消息发送*/
    ulRet = MN_MSG_SendSmsRpReportReq(pucSendData,ucSendLen, enMsgSignallingType);    
    if (VOS_OK != ulRet)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpAckWithTpdu: Send Msg Failed");
    }

    PS_MEM_FREE(WUEPS_PID_TAF, pucSendData);
    return MN_ERR_NO_ERROR;
}


VOS_UINT32 MN_MSG_EnvelopRspRpErrorWithTpdu(
    PS_USIM_ENVELOPE_CNF_STRU           *pstEnvelope
)
{
    MN_MSG_RP_ERR_STRU                  stRpErr;
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo;
    MN_MSG_DELIVER_RPT_ERR_STRU         *pstDeliverRptErr;
    MN_MSG_TP_PID_TYPE_ENUM_U8          enPid;
    MN_MSG_DCS_CODE_STRU                stDcs;
    MN_MSG_RAW_TS_DATA_STRU             *pstRawData;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucSendLen;
    VOS_UINT8                           *pucSendData;
    TAF_MSG_SIGNALLING_TYPE_ENUM_UINT32 enMsgSignallingType;

    /*1. RP-ERROR消息中TPDU部分原始数据结构内存申请*/
    pstTsDataInfo = (MN_MSG_TS_DATA_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_TS_DATA_INFO_STRU));
    if (VOS_NULL_PTR == pstTsDataInfo)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }

    /*2. RP-ERROR消息中TPDU部分原始数据结构填写*/
    PS_MEM_SET(pstTsDataInfo, 0x00, sizeof(MN_MSG_TS_DATA_INFO_STRU));
    pstTsDataInfo->enTpduType       = MN_MSG_TPDU_DELIVER_RPT_ERR;
    pstDeliverRptErr                = &pstTsDataInfo->u.stDeliverRptErr;
    pstDeliverRptErr->enFailCause   = MN_MSG_TP_CAUSE_SIM_DATA_DOWNLOAD_ERR;
    pstDeliverRptErr->ucParaInd     = MN_MSG_TP_PID_MASK | MN_MSG_TP_DCS_MASK | MN_MSG_TP_UDL_MASK;
    MN_MSG_GetMtTpPidAndDcs(&enPid, &stDcs);
    pstDeliverRptErr->enPid = enPid;
    PS_MEM_CPY(&pstDeliverRptErr->stDcs, &stDcs, sizeof(stDcs));
    pstDeliverRptErr->bUserDataHeaderInd    = VOS_FALSE;
    pstDeliverRptErr->stUserData.ulLen      = pstEnvelope->ucDataLen;
    PS_MEM_CPY(pstDeliverRptErr->stUserData.aucOrgData,
               pstEnvelope->aucData,
               pstEnvelope->ucDataLen);

    /*3. RP-ERROR消息中TPDU编码内存申请*/
    pstRawData = (MN_MSG_RAW_TS_DATA_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(MN_MSG_RAW_TS_DATA_STRU));
    if (VOS_NULL_PTR == pstRawData)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstTsDataInfo);
        MN_WARN_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }

    /*4. RP-ERROR消息中TPDU编码，编码函数调用后TPDU原始数据结构已经不需要所以此处释放该内存*/
    ulRet = MN_MSG_Encode(pstTsDataInfo, pstRawData);
    PS_MEM_FREE(WUEPS_PID_TAF, pstTsDataInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
        MN_NORM_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: Fail to MN_MSG_Encode.");
        return MN_ERR_CLASS_SMS_INVALID_TPDU;
    }

    /*5. RP-ERROR消息中TPDU编码长度异常退出*/
    if (pstRawData->ulLen > MN_MSG_MAX_TPDU_LEN)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
        MN_NORM_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: TPDU length is overflow.");
        return MN_ERR_CLASS_SMS_INVALID_TPDU;
    }

    /*6. RP-ERROR消息中RP-ERROR原始数据填写，TPDU相关消息填写完成后，TPDU编码数据结构已经不需要所以此处释放该内存*/
    PS_MEM_SET(&stRpErr,0x00,sizeof(stRpErr));
    stRpErr.ucRpUserDataLen  = (VOS_UINT8)pstRawData->ulLen;
    PS_MEM_CPY(stRpErr.aucRpUserData, pstRawData->aucData, pstRawData->ulLen);
    PS_MEM_FREE(WUEPS_PID_TAF, pstRawData);
    stRpErr.ucRpMsgType      = MN_MSG_RP_MTI_UP_RP_ERR;
    stRpErr.bRpUserDataExist = VOS_TRUE;
    stRpErr.ucRpUserDataIei  = MN_MSG_RP_USER_DATA_IEI;
    stRpErr.ucRpCauseLen = 1;
    stRpErr.stRpCause.bDiagnosticsExist = VOS_FALSE;
    stRpErr.stRpCause.enRpCause = MN_MSG_RP_CAUSE_PROTOCOL_ERR_UNSPECIFIED;

    /*7. RP-ERROR消息中RP-ACK数据编码内存申请及编码*/
    pucSendData = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_TAF, MN_MSG_MAX_LEN);
    if (VOS_NULL_PTR == pucSendData)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: Fail to alloc memory.");
        return MN_ERR_NOMEM;
    }
    MN_MSG_EncodeRpErr(&stRpErr, pucSendData,&ucSendLen);

    enMsgSignallingType = MN_MSG_GetMtRouteStackType();
    
    /*8. RP-ERROR消息中RP-ERROR消息发送*/
    ulRet = MN_MSG_SendSmsRpReportReq(pucSendData,ucSendLen, enMsgSignallingType);

    if (VOS_OK != ulRet)
    {
        MN_WARN_LOG("MN_MSG_EnvelopRspRpErrorWithTpdu: Send Msg Failed");
    }

    PS_MEM_FREE(WUEPS_PID_TAF, pucSendData);
    return MN_ERR_NO_ERROR;
}


VOS_VOID  MN_MSG_RcvUsimEnvelopRsp(
    PS_USIM_ENVELOPE_CNF_STRU           *pstEnvelope
)
{
    VOS_UINT32                          ulRet;

    if (VOS_OK == pstEnvelope->ulResult)
    {
        /*(U)SIM卡返回的TP-UDL为0时，回复网络的RP-ACK消息不带TPDU*/
        if (0 == pstEnvelope->ucDataLen)
        {
            MN_MSG_Internal_SendRpt(VOS_TRUE, 0, 0);
            return;
        }

        /*(U)SIM卡返回的TP-UDL不为0时，回复网络的RP-ACK消息带TPDU*/
        ulRet = MN_MSG_EnvelopRspRpAckWithTpdu(pstEnvelope);
    }
    else
    {
        /*(U)SIM卡返回的错误原因值为卡忙时，回复网络的RP-ERROR消息不带TPDU，TP-FCS为0xd4*/
        if (USIMM_SW_CARD_BUSY == pstEnvelope->ulErrorCode)
        {
            MN_MSG_Internal_SendRpt(VOS_FALSE,
                                    MN_MSG_RP_CAUSE_PROTOCOL_ERR_UNSPECIFIED,
                                    MN_MSG_TP_CAUSE_SAT_BUSY);
            return;
        }

        /*(U)SIM卡返回的TP-UDL为0时，回复网络的RP-ERROR消息不带TPDU，TP-FCS为0xd5*/
        if (0 == pstEnvelope->ucDataLen)
        {
            MN_MSG_Internal_SendRpt(VOS_FALSE,
                                    MN_MSG_RP_CAUSE_PROTOCOL_ERR_UNSPECIFIED,
                                    MN_MSG_TP_CAUSE_SIM_DATA_DOWNLOAD_ERR);
            return;
        }

        /*(U)SIM卡返回的TP-UDL不为0时，回复网络的RP-ERROR消息带TPDU，TP-FCS为0xd5*/
        ulRet = MN_MSG_EnvelopRspRpErrorWithTpdu(pstEnvelope);

    }

    if (MN_ERR_NO_ERROR != ulRet)
    {
        MN_MSG_Internal_SendRpt(VOS_FALSE,
                                MN_MSG_RP_CAUSE_PROTOCOL_ERR_UNSPECIFIED,
                                MN_MSG_TP_CAUSE_ERR_IN_MS);
    }

}
VOS_VOID  MN_MSG_RcvUsimRefreshInd(
    VOS_VOID                            *pMsg
)
{
    PS_USIM_REFRESH_IND_STRU           *pstRefreshInd;

    USIMM_APP_TYPE_ENUM_UINT32          enAppType;
    VOS_UINT32                          ulRequireRefresh;

    pstRefreshInd = (PS_USIM_REFRESH_IND_STRU *)pMsg;

    if ((USIMM_REFRESH_FILE_LIST != pstRefreshInd->enRefreshType)
     && (USIMM_REFRESH_3G_SESSION_RESET != pstRefreshInd->enRefreshType)
     && (USIMM_REFRESH_ALL_FILE != pstRefreshInd->enRefreshType))
    {
        return;
    }

    /* 3G SESSION RESET和所有文件更新类型的REFRESH事件要求重读所有USIM文件 */
    if ((USIMM_REFRESH_3G_SESSION_RESET == pstRefreshInd->enRefreshType )
     || (USIMM_REFRESH_ALL_FILE == pstRefreshInd->enRefreshType))
    {
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSS_ID, VOS_TRUE);
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMS_ID, VOS_TRUE);
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSP_ID, VOS_TRUE);
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSR_ID, VOS_TRUE);

        MN_MSG_CfgDataInit(MNPH_USIM_STATUS_AVILABLE);

        return;
    }

    /* 指定文件列表类型的REFRESH要求重读列表中存在的USIM文件 */
    ulRequireRefresh = TAF_GetSpecificFileListRefreshFileInfo(EF_SMS_SMSS_ID, pstRefreshInd, &enAppType);
    if (VOS_TRUE == ulRequireRefresh)
    {
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSS_ID, VOS_TRUE);
    }

    ulRequireRefresh = TAF_GetSpecificFileListRefreshFileInfo(EF_SMS_SMS_ID, pstRefreshInd, &enAppType);
    if (VOS_TRUE == ulRequireRefresh)
    {
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMS_ID, VOS_TRUE);
    }

    ulRequireRefresh = TAF_GetSpecificFileListRefreshFileInfo(EF_SMS_SMSP_ID, pstRefreshInd, &enAppType);
    if (VOS_TRUE == ulRequireRefresh)
    {
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSP_ID, VOS_TRUE);
    }

    ulRequireRefresh = TAF_GetSpecificFileListRefreshFileInfo(EF_SMS_SMSR_ID, pstRefreshInd, &enAppType);
    if (VOS_TRUE == ulRequireRefresh)
    {
        MN_MSG_SetSmsFileRefreshFlag(EF_SMS_SMSR_ID, VOS_TRUE);
    }

    MN_MSG_CfgDataInit(MNPH_USIM_STATUS_AVILABLE);

    return;
}
VOS_VOID  MN_MSG_RcvUsimSetFileRsp(
    VOS_VOID                            *pMsg
)
{
    MN_MSG_USIM_INFO_STRU               SmaUsimInfo;
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRet;
    PS_USIM_SET_FILE_CNF_STRU           *pUsimSetFileRsp;
    VOS_UINT8                           ucRecIndex;

    pUsimSetFileRsp = (PS_USIM_SET_FILE_CNF_STRU*) pMsg;

    PS_MEM_SET(&SmaUsimInfo, 0, sizeof(SmaUsimInfo));

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

    ulRet = TAF_CBA_ProcUsimSetFileRsp(pUsimSetFileRsp);
    if ( VOS_OK == ulRet )
    {
        MN_INFO_LOG1("MN_MSG_RcvUsimSetFileRsp,CB Files ",(VOS_INT32)(pUsimSetFileRsp->usEfId));
        return;
    }

#endif

    ucRecIndex = pUsimSetFileRsp->ucRecordNum;
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: pUsimSetFileRsp->usEfId ", pUsimSetFileRsp->usEfId);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: pUsimSetFileRsp->ulResult ", (long)pUsimSetFileRsp->ulResult);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: pUsimSetFileRsp->ucRecordNum ", pUsimSetFileRsp->ucRecordNum);
    MSG_PrintUsimReturnRecStru(&f_stMsgUsimReturnRec);
    if (MN_MSG_USIM_DEF_REC != pUsimSetFileRsp->ucRecordNum)
    {
        ucRecIndex = pUsimSetFileRsp->ucRecordNum - 1;
    }

    ulRet = MN_MSG_FindNodeByUsimResult(ucRecIndex,
                                        pUsimSetFileRsp->usEfId,
                                        &SmaUsimInfo,
                                        &ulIndex);

    if (VOS_ERR == ulRet)
    {
        MN_WARN_LOG1("MN_MSG_RcvUsimSetFileRsp,Not matched USIM Node, EFID ",pUsimSetFileRsp->usEfId);
        return;
    }
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.clientId ", SmaUsimInfo.clientId);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.opId ", SmaUsimInfo.opId);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.ucRecIndex ", SmaUsimInfo.ucRecIndex);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.usEfId ", SmaUsimInfo.usEfId);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.enDeleteType ", SmaUsimInfo.enDeleteType);
    MN_NORM_LOG1("MN_MSG_RcvUsimSetFileRsp: SmaUsimInfo.enSmaUsimAction ", SmaUsimInfo.enSmaUsimAction);
#if ( VOS_WIN32 == VOS_OS_VER )
    if (0 != g_ucSetEfSmsNoResponse)
    {
        if ((MN_MSG_USIM_WRITE == SmaUsimInfo.enSmaUsimAction)
        || (MN_MSG_USIM_RCVNEWMSG == SmaUsimInfo.enSmaUsimAction))
       {
           MN_WARN_LOG1("MN_MSG_RcvUsimSetFileRsp,Not matched USIM Node, EFID ",pUsimSetFileRsp->usEfId);
           MN_MSG_DestroyUsimNode(ulIndex);
           return;
       }
    }
#endif

    switch (SmaUsimInfo.enSmaUsimAction)
    {
        case MN_MSG_USIM_READ:
            MSG_ReadSaveUsimMsgRsp(pMsg,SmaUsimInfo.clientId);
            break;
        case MN_MSG_USIM_LIST:
            MSG_ListSaveUsimMsgRsp(pMsg,SmaUsimInfo.clientId);
            break;
        case MN_MSG_USIM_WRITE:
            MSG_WriteSaveUsimMsgRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_DELETE:
            MSG_DeleteSaveUsimMsgRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_MODIFYSTATUS:
            MSG_ModifySaveUsimMsgRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_RCVNEWMSG:
        case MN_MSG_USIM_RCVSTARPT:
            MSG_RcvSaveUsimMsgRsp(pMsg, &SmaUsimInfo);
            break;
        case MN_MSG_USIM_SEND:
            MSG_SendSaveUsimMsgRsp(pMsg,SmaUsimInfo.clientId);
            break;
        case MN_MSG_USIM_RCVMORPT:
            MSG_RcvMoRptSaveUsimMsgRsp(pMsg,SmaUsimInfo.clientId);
            break;
        case MN_MSG_USIM_WRITE_EFSMSP:
            MSG_WriteEfSmspUsimRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_DELETE_EFSMSP:
            MSG_DeleteEfSmspUsimRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_DELETE_STARPT:
            MSG_DeleteStaRptSaveUsimMsgRsp(pMsg,&SmaUsimInfo);
            break;
        case MN_MSG_USIM_SET_EFSMSS:
            MSG_SetEfSmssUsimRsp(pMsg);
            break;
        default:
            MN_WARN_LOG1("MN_MSG_RcvUsimSetFileRsp,Invalid Action, Action ",SmaUsimInfo.enSmaUsimAction);
            break;

    }

    MN_MSG_DestroyUsimNode(ulIndex);
}


VOS_VOID MN_MSG_RcvUsimMoControlRsp(
    PS_USIM_ENVELOPE_CNF_STRU           *pstEnvelope
)
{
    MN_MSG_MO_ENTITY_STRU               stMoEntity;
    MN_MSG_RETRY_INFO_STRU              stRetryInfo;
    MN_MSG_SENDMSG_EVT_INFO_STRU        stSendMsgEvt;
    VOS_UINT32                          ulRet;
    VOS_BOOL                            bBufferEntity;
    VOS_UINT32                          ulIndex;

    MN_MSG_StopTimer(MN_MGS_ID_WAIT_MO_SMS_CTRL_CHECK);

    PS_MEM_SET(&stMoEntity, 0, sizeof(stMoEntity));
    PS_MEM_SET(&stRetryInfo, 0, sizeof(stRetryInfo));

    ulRet = MN_MSG_GetSpecificStatusMoEntity(MN_MSG_MO_STATE_WAIT_SMS_CTRL_RSP,
                                             &bBufferEntity,
                                             &ulIndex,
                                             &stMoEntity);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        MN_WARN_LOG("MN_MSG_RcvUsimMoControlRsp: invalid status");
        return;
    }

/*
    获取返回结果USIM的操作类型
    if 操作类型是SMS-PP data download
        if UICC返回结果是"90 00"(对于USIM返回不同的结果值)
            根据UICC返回的结果构造Rp-ACK的TP-UD,同时DCS和PID应该保持不变
            调用函数MN_MSG_Internal_SendRpt发送Rp-ACK消息到网络
        else if UICC返回结果是"93 00"
            调用函数MN_MSG_Internal_SendRpt发送Rp-Error消息到网络,同时TP-FCS的值应该为"SIM Application Toolkit Busy"
        else if UICC返回结果是"6F XX"
            TP-FCS的值应该为"UICC data download error",同时DCS和PID应该保持不变
            调用函数MN_MSG_Internal_SendRpt发送Rp-Error消息到网络
        else if UICC返回的结果是"62 XX"或"63 XX"
            根据UICC返回的结果构造Rp-ERROR的TP-UD,同时DCS和PID应该保持不变,TP-FCS的值应该为"SIM data download error"
            调用函数MN_MSG_Internal_SendRpt发送Rp-Error消息到网络
        else
            调用函数MN_MSG_Internal_SendRpt发送Rp-Error消息到网络

*/
    ulRet = MN_MSG_UpdateMoEntityAccordingToMoSmsCtrl(pstEnvelope, &stMoEntity);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        MN_WARN_LOG1("MN_MSG_RcvUsimMoControlRsp : MN_MSG_UpdateMoEntityAccordingToMoSmsCtrl ", (VOS_INT32)ulRet);
        MN_SendClientResponse(stMoEntity.clientId,
                              stMoEntity.opId,
                              ulRet);
        MN_MSG_DestroyMoInfo();
    }
    else
    {
        MN_SendClientResponse(stMoEntity.clientId, stMoEntity.opId, MN_ERR_NO_ERROR);

        MN_MSG_UpdateSmaMoState(MN_MSG_MO_STATE_WAIT_REPORT_IND);

        MN_MSG_GetRetryInfo(&stRetryInfo);

        /* 短信重发周期>0 才需要启动短信重发周期*/
        if (stRetryInfo.ulRetryPeriod > 0)
        {
            MN_MSG_StartTimer(MN_MSG_ID_WAIT_RETRY_PERIOD,0);
        }

        /*广播上报短信已被发送*/
        PS_MEM_SET(&stSendMsgEvt,0X00,sizeof(stSendMsgEvt));
        stSendMsgEvt.enTpduType = stMoEntity.enTpduType;
        MN_MSG_ReportSentEvent(stMoEntity.clientId, stMoEntity.opId,&stSendMsgEvt,MN_MSG_EVT_MSG_SENT);
    }

    return ;
}

/* MN_MSG_ProcUsimFdnInd */


VOS_VOID MN_MSG_ProcFdnCnf(struct MsgCB * pstMsg)
{
    MN_MSG_MO_ENTITY_STRU               stMoEntity;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulIndex;
    VOS_BOOL                            bCheckMoCtrl;
    VOS_BOOL                            bBufferEntity;
    PB_FDN_CHECK_CNF_STRU              *pstCheckCnf;

    PS_MEM_SET(&stMoEntity, 0, sizeof(stMoEntity));

    /* 获取等待FDN检查结果的MO实体或缓存 */
    ulRet = MN_MSG_GetSpecificStatusMoEntity(MN_MSG_MO_STATE_WAIT_FDN_CHECK_RSP,
                                             &bBufferEntity,
                                             &ulIndex,
                                             &stMoEntity);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        MN_WARN_LOG("MN_MSG_ProcFdnCnf: discard FDN RESPONSE.");
        return;
    }

    pstCheckCnf = (PB_FDN_CHECK_CNF_STRU *)pstMsg;

    /* 目前FDN的回复消息同时进CALL和MSG模块,需要丢弃不同client ID的消息 */
    if (stMoEntity.clientId != pstCheckCnf->ulSendPara)
    {
        MN_INFO_LOG("MN_MSG_ProcFdnCnf: not same client ID,discard FDN RESPONSE.");
        return;
    }

    MN_MSG_StopTimer(MN_MGS_ID_WAIT_FDN_CHECK);

    /* FDN检查结果为不通过，拒绝短信发送请求，同时释放短信发送实体 */
    if (PB_FDN_CHECK_SUCC != pstCheckCnf->enResult)
    {
        MN_WARN_LOG("MN_MSG_ProcFdnCnf: Fail to pass FDN check.");
        if (PB_FDN_CHECK_NUM1_FAIL == pstCheckCnf->enResult)
        {
            MN_SendClientResponse(stMoEntity.clientId,
                                  stMoEntity.opId,
                                  MN_ERR_CLASS_FDN_CHECK_SC_FAILURE);
        }
        else
        {
            MN_SendClientResponse(stMoEntity.clientId,
                                  stMoEntity.opId,
                                  MN_ERR_CLASS_FDN_CHECK_DN_FAILURE);
        }

        /* 销毁等待FDN检查结果的MO实体或缓存 */
        MN_MSG_DestroySpecificMoEntity(bBufferEntity, ulIndex);
        return;
    }

    /* MO SMS CONTROL 处理:  */
    ulRet = MN_MSG_CheckMoSmsCtrl(&stMoEntity, &bCheckMoCtrl);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        MN_WARN_LOG("MN_MSG_ProcFdnCnf: Fail to MSG_CheckMoSmsCtrl.");
        MN_SendClientResponse(stMoEntity.clientId,
                              stMoEntity.opId,
                              ulRet);

        /* 销毁等待FDN检查结果的MO实体或缓存 */
        MN_MSG_DestroySpecificMoEntity(bBufferEntity, ulIndex);
        return;
    }

    /* 要求检查MO SMS CONTROL: 启动定时器等待USIM的检查结果 */
    if (VOS_TRUE == bCheckMoCtrl)
    {
        ulRet = MN_MSG_ProcMoCtrl(stMoEntity.clientId,
                                  stMoEntity.opId,
                                  stMoEntity.aucRpDataInfo);
        if (MN_ERR_NO_ERROR != ulRet)
        {
            MN_WARN_LOG("MN_MSG_ProcFdnCnf: Fail to MN_MSG_ProcMoCtrl.");
            MN_SendClientResponse(stMoEntity.clientId,
                                  stMoEntity.opId,
                                  ulRet);

            /* 销毁等待FDN检查结果的MO实体或缓存 */
            MN_MSG_DestroySpecificMoEntity(bBufferEntity, ulIndex);
            return;
        }

        /* 保存待发送短信到发送实体或缓存，并更新状态到MN_MSG_MO_STATE_WAIT_FDN_CHECK_RSP
           启动定时器等待USIM的检查结果 */
        MN_MSG_StartTimer(MN_MGS_ID_WAIT_MO_SMS_CTRL_CHECK, 0);
        MN_MSG_UpdateSpecificMoEntityStatus(bBufferEntity, ulIndex, MN_MSG_MO_STATE_WAIT_SMS_CTRL_RSP);

        return;
    }

    /* 主呼叫实体或当前主呼叫实体进入空闲态，则发送短息到SMS */
    /* FDN和CALL CONTROL检查结果为通过，发送呼叫请求到CC */
    if (VOS_OK == MN_MSG_SendMsgAvailable(bBufferEntity,
                                          ulIndex,
                                          &stMoEntity))
    {
        MN_MSG_SendMsgToSms(&stMoEntity);
    }

    return;

}


VOS_VOID MN_MSG_GetFdnMeCfg(VOS_VOID)
{
    if (NV_OK != NV_Read(en_NV_Item_FDN_Info,
                         &f_stMsgFdnInfo.stFdnConfig,
                         sizeof(TAF_FDN_CONFIG_STRU)))
    {
        f_stMsgFdnInfo.stFdnConfig.enMeStatus        = TAF_FDN_CHECK_FEATURE_OFF;
    }

    return;
}


VOS_UINT32 MN_MSG_CheckFdn(
    MN_MSG_MO_ENTITY_STRU              *pstMoEntity,
    VOS_BOOL                           *pbCheckFdn
)
{
    VOS_BOOL                            bBufferEntity;
    VOS_UINT32                          ulIndex;
    MN_MSG_MO_ENTITY_STRU               stMoEntity;
    VOS_UINT32                          ulRet;

    VOS_UINT32                          ulFdnStatus;

    *pbCheckFdn = VOS_FALSE;

    /* 已经完成FDN检查，直接返回 */
    if (pstMoEntity->enSmaMoState > MN_MSG_MO_STATE_WAIT_FDN_CHECK_RSP)
    {
        return MN_ERR_NO_ERROR;
    }

    /* STK用户发起的业务不做FDN业务检查 */
    if (OAM_CLIENT_ID_STK == pstMoEntity->clientId)
    {
        return MN_ERR_NO_ERROR;
    }

    /* ME没有启用FDN检查功能，直接返回业务检查通过 */
    if (TAF_FDN_CHECK_FEATURE_ON != f_stMsgFdnInfo.stFdnConfig.enMeStatus)
    {
        return MN_ERR_NO_ERROR;
    }

    /*
    根据(U)SIM卡的FDN状态确定是否发起FDN检查
       0 去使能: 不做FDN检查
       1 使能:   做FDN检查
    */
    NAS_USIMMAPI_FdnQuery(&ulFdnStatus);

    if (USIMM_FDNSTATUS_ON == ulFdnStatus)
    {
        *pbCheckFdn = VOS_TRUE;
    }

    /* 如果当前已经有短信在MN_MSG_MO_STATE_WAIT_FDN_CHECK_RSP 状态则拒绝新短信 */
    ulRet = MN_MSG_GetSpecificStatusMoEntity(MN_MSG_MO_STATE_WAIT_FDN_CHECK_RSP,
                                             &bBufferEntity,
                                             &ulIndex,
                                             &stMoEntity);
    if (MN_ERR_NO_ERROR == ulRet)
    {
        return MN_ERR_CLASS_SMS_MOSTATE;
    }

    return MN_ERR_NO_ERROR;
}
VOS_VOID MN_MSG_DispatchUsimMsg(struct MsgCB * pstMsg)
{
    switch (((MSG_HEADER_STRU *)pstMsg)->ulMsgName)
    {
        case PS_USIM_GET_FILE_RSP:
            MN_MSG_RcvUsimGetFileRsp((VOS_VOID*)pstMsg);
            break;
        case PS_USIM_SET_FILE_RSP:
            MN_MSG_RcvUsimSetFileRsp((VOS_VOID*)pstMsg);
            break;
        case PS_USIM_EF_MAX_RECORD_NUM_CNF:
            MN_MSG_RcvUsimGetMaxRecRsp((VOS_VOID*)pstMsg);
            break;
        /* Deleted PS_USIM_ENVELOPE_CNF */
        /* Deleted PS_USIM_REFRESH_IND */

        case PS_USIM_FDN_CHECK_CNF:
            /* FDN检查结果处理 */
            MN_MSG_ProcFdnCnf(pstMsg);
            break;

        default:
            MN_WARN_LOG("MN_ProcUsimMsg:Rcv Invalid Msg Type");
            break;
    }

    return;
}
VOS_UINT32 MN_MSG_SendMsgAvailable(
    VOS_BOOL                                bBufferEntity,
    VOS_UINT32                              ulIndex,
    MN_MSG_MO_ENTITY_STRU                  *pstMoEntity
)
{
    MN_MSG_MO_ENTITY_STRU               stMoEntity;

    if (VOS_TRUE != bBufferEntity)
    {
        return VOS_OK;
    }

    PS_MEM_SET(&stMoEntity, 0, sizeof(stMoEntity));

    MN_MSG_GetMoEntity(&stMoEntity);
    if (MN_MSG_MO_STATE_NULL == stMoEntity.enSmaMoState)
    {
        MN_MSG_CreateMoInfo(pstMoEntity);
        return VOS_OK;
    }

    return VOS_ERR;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

