
#include <osm.h>
#include <gen_msg.h>
/*lint -save -e537*/
#include <DrvInterface.h>
/*lint -restore*/
#include <ftm.h>
#include <ftm_ct.h>
/*lint -save -e537*/
#include <msp_errno.h>
/*lint -restore*/
#include <msp_drx.h>
/*lint -save -e767*/
#define    THIS_FILE_ID        MSP_FILE_ID_FTM_MAILBOX_PROC_C
/*lint -restore*/
/* 邮箱数据可读通知信号量，邮箱回调设置 */
VOS_SEM g_mailbox_readsem = 0; 

/*****************************************************************************
 函 数 名  : ftm_mailbox_msgproc
 功能描述  : 邮箱数据读取
 输入参数  : enMbxType 邮箱数据类型
 输出参数  : 无
 返 回 值  : 0无数据，大于0为邮箱数据长度
*****************************************************************************/
VOS_UINT32 ftm_mailbox_msgproc(MAILBOX_SERVICE_TYPE_E enMbxType)
{
    VOS_UINT32 ulRet      = 0;
    VOS_UINT32 ulMsgSize  = 0;
    VOS_VOID* pMailBoxMsg = NULL;
    MsgBlock* pMsgBlock   = NULL;
    VOS_UINT32 ret = ERR_MSP_UNKNOWN;

    /* 读取消息长度 */
    HAL_SDMLOG("[%s] BEIGN TO READ DATA.\n", __FUNCTION__);
    ulMsgSize = BSP_MailBox_ComMsgSize(enMbxType);
    HAL_SDMLOG("[%s] BEIGN TO READ DATA.LEN = %d\n", __FUNCTION__,ulMsgSize);
    if(ulMsgSize == 0 )
    {
        return ulMsgSize;
    }

    pMailBoxMsg = VOS_MemAlloc(MSP_SYS_FTM_PID, ((DYNAMIC_MEM_PT)), ulMsgSize);
    if(NULL == pMailBoxMsg)
    {
        HAL_SDMLOG("[%s] VOS_MemAlloc failed.\n", __FUNCTION__);
        return 0;
    }

    /* 读取邮箱数据 */
    ulRet = BSP_MailBox_ComMsgRead(enMbxType, pMailBoxMsg, ulMsgSize, EN_MAILBOX_SLEEP_WAKEUP);
    if(ulRet != BSP_OK)
    {
        VOS_MemFree(MSP_SYS_FTM_PID, pMailBoxMsg);
        return 0;
    }

    /* 发送消息到MSP_SYS_FTM_PID任务 */
    pMsgBlock = (MsgBlock*)VOS_AllocMsg(MSP_SYS_FTM_PID, sizeof(OS_MSG_STRU));

    if (pMsgBlock)
    {
        OS_MSG_STRU* pMsg = (OS_MSG_STRU*)pMsgBlock->aucValue;

        pMsgBlock->ulReceiverPid = MSP_SYS_FTM_PID;
        pMsgBlock->ulSenderPid   = MSP_SYS_FTM_PID;
        pMsg->ulMsgId  = ID_MSG_L1A_CT_IND;
        pMsg->ulParam1 = pMailBoxMsg;
        pMsg->ulParam2 = ulMsgSize;

        ret = VOS_SendMsg(MSP_SYS_FTM_PID, pMsgBlock);
        if (ret != VOS_OK)
        {
            HAL_SDMLOG("[%s] send msg to MSP_SYS_FTM_PID FAILED \n",  __FUNCTION__);
        }
    }

    return ulMsgSize;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_taskproc
 功能描述  : 读邮箱数据自处理任务
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID ftm_mailbox_taskproc()
{
	/*lint -save -e716*/
	while(1)
	/*lint -restore*/
    {
        if(VOS_ERR == VOS_SmP(g_mailbox_readsem, 0))
        {
            HAL_SDMLOG("[%s] VOS_Smp(g_mailbox_readsem, 0) error\n",  __FUNCTION__);
        }

		/*vote for lock*/
		drx_msp_fid_vote_lock(DRX_FTM_MAILBOX_VOTE);
       /* LTE CT 原语 */
        if(ftm_mailbox_msgproc(EN_MAILBOX_SERVICE_LTE_CT) > 0)
        {
            //VOS_SmV(g_mailbox_readsem);
            continue;
        }

      /* LTE BT 原语 */
        if(ftm_mailbox_msgproc(EN_MAILBOX_SERVICE_LTE_BT) > 0)
        {
            //VOS_SmV(g_mailbox_readsem);
            continue;
        }

        /* TDS CT 原语 */
        if(ftm_mailbox_msgproc(EN_MAILBOX_SERVICE_TDS_CT) > 0)
        {
            //VOS_SmV(g_mailbox_readsem);
            continue;
        }

        /* TDS BT 原语 */
        if(ftm_mailbox_msgproc(EN_MAILBOX_SERVICE_TDS_BT) > 0)
        {
            //VOS_SmV(g_mailbox_readsem);
            continue;
        }
		/*vote for lock*/
		drx_msp_fid_vote_unlock(DRX_FTM_MAILBOX_VOTE);
        
        //VOS_SmV(g_mailbox_readsem);
    }

}

/*****************************************************************************
 函 数 名  : ftm_mailbox_ltect_cb
 功能描述  : LTE CT类型数据到底回调
 输入参数  : enIntType 邮箱中断类型
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID ftm_mailbox_ltect_cb(MAILBOX_INT_TYPE_E enIntType)
{
    VOS_SmV(g_mailbox_readsem);

    return;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_ltebt_cb
 功能描述  : LTE BT类型数据到底回调
 输入参数  : enIntType 邮箱中断类型
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID ftm_mailbox_ltebt_cb(MAILBOX_INT_TYPE_E enIntType)
{
    VOS_SmV(g_mailbox_readsem);

    return;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_tdsct_cb
 功能描述  : TDS CT类型数据到底回调
 输入参数  : enIntType 邮箱中断类型
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID ftm_mailbox_tdsct_cb(MAILBOX_INT_TYPE_E enIntType)
{
    VOS_SmV(g_mailbox_readsem);

    return;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_tdsbt_cb
 功能描述  : TDS BT类型数据到底回调
 输入参数  : enIntType 邮箱中断类型
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID ftm_mailbox_tdsbt_cb(MAILBOX_INT_TYPE_E enIntType)
{
    VOS_SmV(g_mailbox_readsem);

    return;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_ltect_write
 功能描述  : 写LTE CT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_ltect_write(VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    HAL_SDMLOG("[%s] : send lte ct to dsp.\n", __FUNCTION__);
    ulRet = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_LTE_CT, param, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ulRet != BSP_OK)
    {
        HAL_SDMLOG("[%s] : send lte ct to dsp fail!!!.\n", __FUNCTION__);
        ulRet = ERR_MSP_FAILURE;
    }
    FTM_MNTN_FTM2RTT_MSG(*((VOS_UINT*)param))

    return ulRet;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_ltebt_write
 功能描述  : 写LTE bT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_ltebt_write(VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    HAL_SDMLOG("[%s] : send lte ct to dsp.\n", __FUNCTION__);
    ulRet = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_LTE_BT, param, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ulRet != BSP_OK)
    {
        HAL_SDMLOG("[%s] : send lte ct to dsp fail !!!.\n", __FUNCTION__);
        ulRet = ERR_MSP_FAILURE;
    }
    FTM_MNTN_FTM2RTT_MSG(*((VOS_UINT*)param))

    return ulRet;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_tdsct_write
 功能描述  : 写TDS CT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_tdsct_write(VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    ulRet = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_TDS_CT, param, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ulRet != BSP_OK)
    {
        ulRet = ERR_MSP_FAILURE;
    }
    FTM_MNTN_FTM2RTT_MSG(*((VOS_UINT*)param))

    return ulRet;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_tdsbt_write
 功能描述  : 写LTE BT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_tdsbt_write(VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    ulRet = BSP_MailBox_ComMsgWrite(EN_MAILBOX_SERVICE_TDS_BT, param, ulLen, EN_MAILBOX_SLEEP_WAKEUP);
    if(ulRet != BSP_OK)
    {
        ulRet = ERR_MSP_FAILURE;
    }
    FTM_MNTN_FTM2RTT_MSG(*((VOS_UINT*)param))

    return ulRet;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_ltect_write
 功能描述  : 写LTE CT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_ct_write(VOS_UINT32 mode, VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    if(EN_FCHAN_MODE_TD_SCDMA == mode)
    {
        vos_printf("set tds dsp msg to dsp\n");
        ulRet = ftm_mailbox_tdsct_write(param, ulLen);
    }
    else
    {
        vos_printf("set lte dsp msg to dsp\n");
        ulRet = ftm_mailbox_ltect_write(param, ulLen);
    }

    return ulRet;
}

/*****************************************************************************
 函 数 名  : ftm_mailbox_bt_write
 功能描述  : 写LTE BT类型数据到邮箱
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_bt_write(VOS_UINT32 mode, VOS_VOID* param,VOS_UINT32 ulLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    if(EN_FCHAN_MODE_TD_SCDMA == mode)
    {
        ulRet = ftm_mailbox_tdsbt_write(param, ulLen);
    }
    else
    {
        ulRet = ftm_mailbox_ltebt_write(param, ulLen);
    }

    return ulRet;
}


/*****************************************************************************
 函 数 名  : ftm_mailbox_init
 功能描述  : FTM模块邮箱适配初始化
 输入参数  : param 原语内容
             ulLen 原语长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 ftm_mailbox_init()
{
    if(VOS_OK != VOS_SmCCreate( "ftm_mailbox_taskproc", 0, VOS_SEMA4_FIFO, &g_mailbox_readsem))
    {
        HAL_SDMLOG("[%s] : VOS_SmCCreate failed.\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    if(BSP_OK != BSP_MailBox_ComNotifyReg(EN_MAILBOX_SERVICE_LTE_CT, ftm_mailbox_ltect_cb))
    {
        HAL_SDMLOG("[%s] : BSP_MailBox_ComNotifyReg EN_MAILBOX_SERVICE_LTE_CT failed.\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    if(BSP_OK != BSP_MailBox_ComNotifyReg(EN_MAILBOX_SERVICE_LTE_BT, ftm_mailbox_ltebt_cb))
    {
        HAL_SDMLOG("[%s] : BSP_MailBox_ComNotifyReg EN_MAILBOX_SERVICE_LTE_BT failed.\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    if(BSP_OK != BSP_MailBox_ComNotifyReg(EN_MAILBOX_SERVICE_TDS_CT, ftm_mailbox_tdsct_cb))
    {
        HAL_SDMLOG("[%s] : BSP_MailBox_ComNotifyReg EN_MAILBOX_SERVICE_TDS_CT failed.\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    if(BSP_OK != BSP_MailBox_ComNotifyReg(EN_MAILBOX_SERVICE_TDS_BT, ftm_mailbox_tdsbt_cb))
    {
        HAL_SDMLOG("[%s] : BSP_MailBox_ComNotifyReg EN_MAILBOX_SERVICE_TDS_BT failed.\n", __FUNCTION__);
        return ERR_MSP_FAILURE;
    }

    return ERR_MSP_SUCCESS;
}

