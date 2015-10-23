

#include <msp_errno.h>
#include <osm.h>
#include <gen_msg.h>
#include <ftm.h>
#include "LMspLPhyInterface.h"
#include "ftm_ct.h"
/*lint -save -e537*/
#include <AppRrcInterface.h>
#include "TafAppMma.h"
/*lint -restore*/
#include "Tds_ps_at.h"
#include "TPsTMspInterface.h" 

/*lint -save -e767*/
#define    THIS_FILE_ID        MSP_FILE_ID_FTM_COMM_CMD_C
/*lint -restore*/

/*****************************************************************************
 函 数 名  : at_ftm_ltcommcmd_set
 功能描述  : ID_MSG_FTM_SET_LTCOMMCMD_REQ 处理函数
             进行LTE频段切换
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ltcommcmd_set(VOS_VOID* pParam)
{
	/*lint -save -e813*/
    FTM_SET_LTCOMMCMD_CNF_STRU  stCnf = { 0 };
    FTM_SET_LTCOMMCMD_REQ_STRU* pstFtmReq   = (FTM_SET_LTCOMMCMD_REQ_STRU*)pParam;
    OM_PHY_COMM_CMD_SET_REQ_STRU stReqToDsp = { 0 };
	/*lint -restore*/
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    VOS_UINT32 i     = 0;

    VOS_MemSet(&ftm_GetMainInfo()->stRdLtCommCmdCnf, 0, sizeof(FTM_RD_LTCOMMCMD_CNF_STRU));
    ftm_GetMainInfo()->stRdLtCommCmdCnf.ulErrCode = ERR_MSP_SUCCESS;

    stReqToDsp.ulMsgId   = OM_PHY_COMM_CMD_SET_REQ;
    stReqToDsp.ulDataLen = pstFtmReq->ulDataLen;
    VOS_MemCpy(stReqToDsp.cData, pstFtmReq->cData, stReqToDsp.ulDataLen);  

    vos_printf("at_ftm_ltcommcmd_set: 1\n");
    for(i=0; i<pstFtmReq->ulDataLen; i++)
    {
       if(i%10 == 0)
       {
          vos_printf("\n");
       }
       vos_printf("%02X", stReqToDsp.cData[i]);     
    }
    vos_printf("\n");

    switch(pstFtmReq->ulCmdDest)
    {
        case EN_LTCOMMCMD_DEST_LDSP_CT:
            ulRet = ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_COMM_CMD_SET_REQ_STRU));
            break;

        case EN_LTCOMMCMD_DEST_LDSP_BT:
            ulRet = ftm_mailbox_ltebt_write(&stReqToDsp, sizeof(OM_PHY_COMM_CMD_SET_REQ_STRU));
            break;

        case EN_LTCOMMCMD_DEST_TDSP_CT:
            ulRet = ftm_mailbox_tdsct_write(&stReqToDsp, sizeof(OM_PHY_COMM_CMD_SET_REQ_STRU));
            break;

        case EN_LTCOMMCMD_DEST_TDSP_BT:
            ulRet = ftm_mailbox_tdsbt_write(&stReqToDsp, sizeof(OM_PHY_COMM_CMD_SET_REQ_STRU));
            break;

        default:
            ulRet = ERR_MSP_FAILURE;
    }

    if (ERR_MSP_SUCCESS != ulRet)
    {
        stCnf.ulErrCode = ERR_MSP_UNKNOWN;
        stCnf.ulDataLen = 0;
        
        return ftm_comm_send(ID_MSG_FTM_SET_LTCOMMCMD_CNF,
            (VOS_UINT32)&stCnf, sizeof(FTM_SET_LTCOMMCMD_CNF_STRU));
    }  
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_ltcommcmd_read
 功能描述  : ID_MSG_FTM_RD_LTCOMMCMD_REQ 处理函数
 输入参数  : 无
 输出参数  : 无s
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ltcommcmd_read(VOS_VOID* pParam)
{
    FTM_RD_LTCOMMCMD_CNF_STRU* pstFtmCnf = (FTM_RD_LTCOMMCMD_CNF_STRU *)&ftm_GetMainInfo()->stRdLtCommCmdCnf;

    return ftm_comm_send(ID_MSG_FTM_RD_LTCOMMCMD_CNF, (VOS_UINT32)pstFtmCnf, sizeof(FTM_RD_LTCOMMCMD_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_ltcommcmd_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ltcommcmd_timeout()
{
	/*lint -save -e813*/
	FTM_SET_LTCOMMCMD_CNF_STRU  stCnf = { 0 };
	/*lint -restore*/
    stCnf.ulErrCode = ERR_MSP_TIME_OUT;
    stCnf.ulDataLen = 0;
        
   return ftm_comm_send(ID_MSG_FTM_SET_LTCOMMCMD_CNF,
            (VOS_UINT32)&stCnf, sizeof(FTM_SET_LTCOMMCMD_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_ltcommcmd_dspcnf
 功能描述  : DSP原语PHY_OM_COMM_CMD_SET_CNF处理函数
 输入参数  : pParam PHY_OM_COMM_CMD_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ltcommcmd_dspcnf(VOS_VOID* pParam)
{
    PHY_OM_COMM_CMD_SET_CNF_STRU* pDspCnf = (PHY_OM_COMM_CMD_SET_CNF_STRU*)pParam;
	/*lint -save -e813*/
    FTM_SET_LTCOMMCMD_CNF_STRU    stSetLtCommCmdCnf  = { 0 };
	/*lint -restore*/
    stSetLtCommCmdCnf.ulErrCode = ERR_MSP_SUCCESS;
    stSetLtCommCmdCnf.ulDataLen = pDspCnf->ulDataLen;
    VOS_MemCpy(stSetLtCommCmdCnf.cData, pDspCnf->cData, stSetLtCommCmdCnf.ulDataLen);    

    return ftm_comm_send(ID_MSG_FTM_SET_LTCOMMCMD_CNF,
        (VOS_UINT32)&stSetLtCommCmdCnf, sizeof(FTM_SET_LTCOMMCMD_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_ltcommcmd_dspind
 功能描述  : DSP原语PHY_OM_COMM_CMD_IND处理函数
 输入参数  : pParam PHY_OM_COMM_CMD_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ltcommcmd_dspind(VOS_VOID* pParam)
{
    PHY_OM_COMM_CMD_IND_STRU* pDspInd    = (PHY_OM_COMM_CMD_IND_STRU*)pParam;
    FTM_RD_LTCOMMCMD_CNF_STRU* pstFtmCnf = (FTM_RD_LTCOMMCMD_CNF_STRU *)&ftm_GetMainInfo()->stRdLtCommCmdCnf;

    VOS_MemSet(pstFtmCnf, 0, sizeof(FTM_RD_LTCOMMCMD_CNF_STRU));
    pstFtmCnf->ulErrCode = ERR_MSP_SUCCESS;
    pstFtmCnf->ulDataLen = pDspInd->ulDataLen;
    VOS_MemCpy(pstFtmCnf->cData, pDspInd->cData, pstFtmCnf->ulDataLen);

    return ERR_MSP_SUCCESS;
}

