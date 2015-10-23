
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CmmcaMntn.h"
#include "CmmcaCtx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_CMMCA_MNTN_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_UINT8                               g_ucCmmcaParseDbg = VOS_FALSE;

/*****************************************************************************
  3 函数定义
*****************************************************************************/

VOS_VOID CMMCA_Debug_SetParseDbg(VOS_UINT8 ucParseDbg)
{
    g_ucCmmcaParseDbg = ucParseDbg;
}


VOS_VOID CMMCA_Debug_PrintPdnId(VOS_UINT8 ucPdnId)
{
    if (g_ucCmmcaParseDbg)
    {
        vos_printf("CMMCA_Debug_PrintPdnId: ucPdnId is %d\r\n", ucPdnId);
    }
}


VOS_VOID CMMCA_Debug_PrintPdnType(VOS_UINT8 ucPdnType)
{
    if (g_ucCmmcaParseDbg)
    {
        vos_printf("CMMCA_Debug_PrintPdnType: ucPdnType is %d\r\n", ucPdnType);
    }
}


VOS_VOID CMMCA_Debug_PrintApn(
    VOS_UINT8                          *pucApn,
    VOS_UINT16                          usApnLen
)
{
    if (g_ucCmmcaParseDbg)
    {
        vos_printf("CMMCA_Debug_PrintApn: usApnLen is %d\r\n, apn is %s\r\n", usApnLen, pucApn);
    }
}


VOS_VOID CMMCA_Debug_PrintRatId(
    CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16    enCmdId,
    CMMCA_MMC_RAT_ID_ENUM_UINT8         enRatId
)
{
    if (g_ucCmmcaParseDbg)
    {
        vos_printf("CMMCA_Debug_PrintRatId: enCmdId is %d\r\n, usDataLen is %d\r\n", enCmdId, enRatId);
    }
}


VOS_VOID CMMCA_Debug_PrintDataIndLenErr(
    CMMCA_MMC_RAT_CMD_ID_ENUM_UINT16    enCmdId,
    VOS_UINT32                          ulDataLen
)
{
    if (g_ucCmmcaParseDbg)
    {
        vos_printf("CMMCA_Debug_PrintCmdDataLenErr: enCmdId is %d\r\n, usDataLen is %d\r\n", enCmdId, ulDataLen);
    }
}


VOS_VOID  CMMCA_LogBuffFullInd(VOS_VOID)
{
    CMMCA_MNTN_BUFF_FULL_IND_STRU      *pstMsg = VOS_NULL_PTR;

    pstMsg = (CMMCA_MNTN_BUFF_FULL_IND_STRU*)PS_MEM_ALLOC(WUEPS_PID_CMMCA,
                              sizeof(CMMCA_MNTN_BUFF_FULL_IND_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        CMMCA_ERROR_LOG("CMMCA_LogBuffFullInd:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_CMMCA;
    pstMsg->ulReceiverPid   = WUEPS_PID_CMMCA;
    pstMsg->ulLength        = sizeof(CMMCA_MNTN_BUFF_FULL_IND_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->enMsgId         = CMMCA_MNTN_BUFF_FULL_IND;

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_CMMCA, pstMsg);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





