


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "diag_common.h"
#include  "msp_errno.h"
#include  "diag_debug.h"
#include  "DrvInterface.h"
/*lint -restore*/
/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_COMMON_C
/*lint -restore +e767*/

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
DIAG_LOG_PKT_NUM_ACC_STRU g_DiagLogPktNum ={0};

/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_UINT32 diag_GetAgentPid(VOS_VOID)
{
    /*lint -save -e40*/
#if(VOS_OS_VER == VOS_LINUX)
    return MSP_PID_DIAG_APP_AGENT;
#else
    return MSP_PID_DIAG_AGENT;
#endif
    /*lint -restore  +e40*/
}

/*yuyangyang NV 获取imei号接口 begin*/

VOS_UINT32 diag_GetImei(VOS_CHAR szimei [ 16 ])
{
    VOS_UINT32  ret;
    VOS_UINT32  uslen = 0;
    VOS_UINT32  subscript = 0;
    VOS_CHAR   checkdata = 0;
    VOS_CHAR   auctemp[DIAG_NV_IMEI_LEN+1] = {0};

    uslen = DIAG_NV_IMEI_LEN+1;

    ret = NVM_Read(0, auctemp, uslen);

    if(ret != 0)
    {
        return ret;
    }
    else
    {
        for (subscript = 0; subscript < (DIAG_NV_IMEI_LEN - 1); subscript += 2)
        {
            checkdata += (VOS_CHAR)(((auctemp[subscript])
                           +((auctemp[subscript + 1] * 2) / 10))
                           +((auctemp[subscript + 1] * 2) % 10));
        }
        checkdata = (10 - (checkdata%10)) % 10;

        for (subscript = 0; subscript < uslen; subscript++)
        {
            *(szimei + subscript) = *(auctemp + subscript) + 0x30; /*字符转换*/
        }

        szimei[DIAG_NV_IMEI_LEN - 1] = checkdata + 0x30;
        szimei[DIAG_NV_IMEI_LEN] = 0;
    }

    return 0;
}
/*yuyangyang NV 获取imei号接口 end*/


VOS_UINT64 diag_GetFrameTime(VOS_VOID)
{
    VOS_UINT64 ulFrameCount = 0;
    VOS_UINT32 ret;

    ret = BSP_BBPGetCurTime(&ulFrameCount);
    if(ERR_MSP_SUCCESS != ret)
    {
       ulFrameCount = 0;
    }
    return ulFrameCount;
}
#if ((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))

VOS_INT32 diag_RegRead(VOS_UINT32 u32RegAddr,ENADDRTYPE enAddrType, VOS_UINT32 *pu32Value)
{
    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *pu32Value = *((volatile VOS_UINT8 *)u32RegAddr);
            return OK;

        case ADDRTYPE16BIT:
            *pu32Value = *((volatile VOS_UINT16 *)u32RegAddr);
            return OK;

        case ADDRTYPE32BIT:
            *pu32Value = *((volatile VOS_UINT32 *)u32RegAddr);
            return OK;

        default:
            return ERROR;
    }
}


VOS_INT32 diag_RegWrite(VOS_UINT32 u32RegAddr,ENADDRTYPE enAddrType, VOS_UINT32 u32Value)
{

    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *((volatile VOS_UINT8 *)u32RegAddr) = (VOS_UINT8)u32Value;
            return OK;

        case ADDRTYPE16BIT:
            *((volatile VOS_UINT16 *)u32RegAddr) = (VOS_UINT16)u32Value;
            return OK;

        case ADDRTYPE32BIT:
            *((volatile VOS_UINT32 *)u32RegAddr) = u32Value;
            return OK;

        default:
            return ERROR;
    }
}
#endif


VOS_UINT32 diag_SendMsg(VOS_UINT32 ulSenderId, VOS_UINT32 ulRecverId, VOS_UINT32 ulMsgId, VOS_UINT8* pDta, VOS_UINT32 dtaSize)
{
    VOS_UINT32 ret = ERR_MSP_UNKNOWN;

    DIAG_DATA_MSG_STRU* pDataMsg = NULL;
    /*lint -save -e740*/
    pDataMsg = (DIAG_DATA_MSG_STRU*)VOS_AllocMsg(ulSenderId,(sizeof(DIAG_DATA_MSG_STRU)+ dtaSize - 20));
    /*lint -restore*/

    if (pDataMsg != NULL)
    {
        pDataMsg->ulReceiverPid = ulRecverId;
        pDataMsg->ulSenderPid   = ulSenderId;
        pDataMsg->ulLength      = sizeof(DIAG_DATA_MSG_STRU)+ dtaSize - 20;

        pDataMsg->ulMsgId    = ulMsgId;
        pDataMsg->ulLen      = dtaSize;
        VOS_MemCpy(pDataMsg->pContext,pDta,dtaSize);

        ret = VOS_SendMsg(ulSenderId, pDataMsg);
        if (ret != VOS_OK)
        {
			diag_printf("diag_SendMsg ulSenderId=%d,ulRecverId=%d,ulMsgId=0x%x,pDta=%p,dtaSize=%d!\n",ulSenderId,ulRecverId,ulMsgId,pDta,dtaSize);
        }
        else
        {
            ret = ERR_MSP_SUCCESS;
        }
    }
    return ret;
}

VOS_UINT32 diag_SendPsTransMsg(VOS_UINT32 ulRecverId, VOS_UINT8* pDta, VOS_UINT32 dtaSize)
{
    VOS_UINT32 ret = ERR_MSP_UNKNOWN;

    DIAG_PS_MSG_STRU* pDataMsg = NULL;

	/*传进来的dtaSize已经包含了VOS HEADER，因此申请时需要-20*/
    /*lint -save -e740*/
    pDataMsg = (DIAG_PS_MSG_STRU*)VOS_AllocMsg(diag_GetAgentPid(),dtaSize -VOS_MSG_HEAD_LENGTH);
    /*lint -restore*/
    if (pDataMsg)
    {

        VOS_MemCpy(pDataMsg,pDta,dtaSize);
        pDataMsg->ulReceiverPid = ulRecverId;
        pDataMsg->ulSenderPid   = diag_GetAgentPid();
        /* coverity[overflow_const] */
        pDataMsg->ulLength      = dtaSize - VOS_MSG_HEAD_LENGTH;

        ret = VOS_SendMsg(diag_GetAgentPid(), pDataMsg);
        if (ret != VOS_OK)
        {
			diag_printf(" ulRecverId=%d,pDta=%p,dtaSize=%d!\n",ulRecverId,pDta,dtaSize);
        }
        else
        {
            ret = ERR_MSP_SUCCESS;
        }
    }
    return ret;
}


VOS_UINT32 diag_AgentCnfFun(VOS_UINT8* pstCmdCnf,VOS_UINT32 ulCmdId,VOS_UINT32 ulDataSize )
{
    DIAG_CMD_AGENT_TO_FW_CNF_STRU* pstCnf = NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulSendSize =0;

    ulSendSize = sizeof(DIAG_CMD_AGENT_TO_FW_CNF_STRU)+ ulDataSize;

    pstCnf = VOS_MemAlloc(diag_GetAgentPid(),DYNAMIC_MEM_PT,ulSendSize);

    if(NULL != pstCnf)
    {
        pstCnf->ulCmdId = ulCmdId;
        VOS_MemCpy(pstCnf->aucData, pstCmdCnf, ulDataSize);
        /*lint -save -e40*/
#if(VOS_OS_VER == VOS_LINUX)
        ret = diag_SendMsg(diag_GetAgentPid(),MSP_PID_DIAG_FW,ID_MSG_DIAG_CMD_CNF_APP_AGENT_TO_FW,\
                  (VOS_UINT8*)pstCnf,ulSendSize);
#else
        ret = diag_SendMsg(diag_GetAgentPid(),MSP_PID_DIAG_FW,ID_MSG_DIAG_CMD_CNF_AGENT_TO_FW,\
                  (VOS_UINT8*)pstCnf,ulSendSize);
#endif
        /*lint -restore  +e40*/
        VOS_MemFree(diag_GetAgentPid(), pstCnf);
        return ret;
	}
    else
    {
    	 diag_printf("[%s]: malloc fail!\n",__FUNCTION__);
         return ERR_MSP_MALLOC_FAILUE;
    }
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



