

/*******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
*******************************************************************************/

#include "msp_errno.h"
#include <dms.h>
#include "dms_core.h"
#include "vos.h"


VOS_UINT32              g_ulNdisCfgFlag     = 0xffffffff;
USBNdisStusChgFunc      g_atConnectBrk      = NULL;
USBNdisAtRecvFunc       g_atCmdRcv          = NULL;

/*****************************************************************************
 函 数 名  : dms_NcmCfg
 功能描述  : 打开NDIS CTRL通道
 输入参数  :

 输出参数  :
 返 回 值  : ERR_MSP_FAILURE/ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dms_NcmCfg(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    BSP_S32 slNcmHandle ;
    DMS_PHY_BEAR_PROPERTY_STRU  *pstComCfg =NULL;
    UDI_OPEN_PARAM stParam ;

    pstComCfg = dmsgetPhyBearProperty();

    if(pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle != UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    stParam.devid = UDI_NCM_CTRL_ID;

    slNcmHandle = udi_open(&stParam);

    if(slNcmHandle ==UDI_INVALID_HANDLE)
    {
        ret =  ERR_MSP_FAILURE;
    }
    else
    {
        pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle = slNcmHandle;

        ret = (VOS_UINT32)udi_ioctl (slNcmHandle , NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC, dms_AtNdisWrtCB);
        ret = ERR_MSP_SUCCESS;
    }

    /*配置 NDIS CTRL 通道*/
    Dms_NcmProcCbReg(g_atConnectBrk,(USB_NAS_AT_CMD_RECV)g_atCmdRcv);

    return ret;

}

/*****************************************************************************
 函 数 名  : dms_NcmClose
 功能描述  : NDIS CTRL通道关闭
 输入参数  :

 输出参数  :
 返 回 值  : -1/ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dms_NcmClose(VOS_VOID)
{
    VOS_INT32 ret = ERR_MSP_SUCCESS;
    DMS_PHY_BEAR_PROPERTY_STRU  *pstComCfg =NULL;

    pstComCfg = dmsgetPhyBearProperty();

    ret = udi_close(pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle);

    pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle = UDI_INVALID_HANDLE;

    return ( VOS_UINT32)ret;

}

/*****************************************************************************
 函 数 名  : dms_NcmSendData
 功能描述  : NDIS CTRL通道数据发送接口
 输入参数  : pData: 发送buf
             ulLen: 发送长度
 输出参数  :
 返 回 值  : ERR_MSP_INVALID_PARAMETER/ERR_MSP_FAILURE/ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dms_NcmSendData(VOS_UINT8 *pData, VOS_UINT32 ulLen)
{

    VOS_INT32 ret = ERR_MSP_SUCCESS;
    NCM_AT_RSP_S stATResponse = {0};
    DMS_PHY_BEAR_PROPERTY_STRU  *pstComCfg =NULL;

    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();

    /*检查NDIS通道状态*/
    if(aenPhyBear[EN_DMS_BEARER_USB_NCM].ucChanStat == ACM_EVT_DEV_SUSPEND)
    {
        return ERR_MSP_FAILURE;
    }

    if((pData==NULL)||(ulLen==0)||(ulLen > 2048))
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    pstComCfg = dmsgetPhyBearProperty();

    stATResponse.pu8AtAnswer = pData;
    stATResponse.u32Length = ulLen;

    if(UDI_INVALID_HANDLE ==pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle)
    {
        return ERR_MSP_FAILURE;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_NCM_SEND_TO_DRV, (VOS_UINT32)pData, ulLen, 0);

    ret=udi_ioctl(pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle, NCM_IOCTL_AT_RESPONSE, &stATResponse);


    if(ret !=ERR_MSP_SUCCESS)
    {
        return ERR_MSP_FAILURE;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_NCM_SEND_TO_DRV_SUCC, 0, 0, 0);

    return ERR_MSP_SUCCESS;

}

/*****************************************************************************
 函 数 名  : DMS_NCMStatusChangeReg
 功能描述  : ndis通道速率配置函数
 输入参数  : pPdpStru: 配置数据

 输出参数  :
 返 回 值  : ERR_MSP_INVALID_PARAMETER/-1/0
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_INT32 DMS_NCMStatusChangeReg(NAS_PRO_STRU * pPdpStru)
{
    VOS_INT32 slRet = -1;

    DMS_PHY_BEAR_PROPERTY_STRU  *pstComCfg = NULL;

    pstComCfg = dmsgetPhyBearProperty();

    if(NULL == pPdpStru)
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if(UDI_INVALID_HANDLE == pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle)
    {
        return slRet;
    }

    slRet = udi_ioctl (pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle , NCM_IOCTL_NETWORK_CONNECTION_NOTIF, (VOS_VOID*)(&pPdpStru->enActiveSatus));

    if(0 != slRet)
    {
        return slRet;
    }

    return slRet;
}


/*****************************************************************************
 函 数 名  : DMS_NCMExtFuncReg
 功能描述  : ndis通道回调函数注册函数封装
 输入参数  : connectBrk: 连接状态处理函数
             atCmdRcv: 数据接收回调函数
 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

int DMS_NCMExtFuncReg(USB_NAS_BRK connectBrk,USB_NAS_AT_CMD_RECV atCmdRcv)
{

    g_atConnectBrk = (USBNdisStusChgFunc )connectBrk;
    g_atCmdRcv     = (USBNdisAtRecvFunc )atCmdRcv;

    Dms_NcmProcCbReg((USBNdisStusChgFunc )connectBrk,(USB_NAS_AT_CMD_RECV )atCmdRcv);

    return ERR_MSP_SUCCESS;
}


/*****************************************************************************
 函 数 名  : DMS_NCMExtFuncReg
 功能描述  : ndis通道回调函数注册函数
 输入参数  : connectBrk: 连接状态处理函数
             atCmdRcv: 数据接收回调函数
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID Dms_NcmProcCbReg(USBNdisStusChgFunc connectBrk,USB_NAS_AT_CMD_RECV atCmdRcv)
{
    VOS_INT32 slRet = -1;

    DMS_PHY_BEAR_PROPERTY_STRU  *pstComCfg =NULL;

    if((connectBrk == NULL)||(atCmdRcv  ==NULL))
    {
            return ;
    }

    /*NDIS 通道没有打开或者 通道已经被配置，则直接返回*/
    if((g_ulNdisCfgFlag == 0xffffffff)||(g_ulNdisCfgFlag == 0))
    {
        return ;
    }

    pstComCfg = dmsgetPhyBearProperty();

    if(UDI_INVALID_HANDLE == pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle)
    {
        return ;
    }

    slRet = udi_ioctl (pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle , NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC, (USBNdisStusChgFunc)(connectBrk));

    if(ERR_MSP_SUCCESS != slRet)
    {
        return ;
    }

    slRet = udi_ioctl (pstComCfg[EN_DMS_BEARER_USB_NCM].slPortHandle , NCM_IOCTL_REG_AT_PROCESS_FUNC, (USBNdisAtRecvFunc)(atCmdRcv));

    if(ERR_MSP_SUCCESS != slRet)
    {
        return ;
    }

    g_ulNdisCfgFlag = 0;

    return ;
}
VOS_VOID Dms_SetNdisChanStatus(ACM_EVT_E enStatus )
{
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();

    aenPhyBear[EN_DMS_BEARER_USB_NCM].ucChanStat = (VOS_UINT8)enStatus;
}



VOS_VOID dms_AtNdisWrtCB (char* pDoneBuff, int status)
{

    if (Dms_IsStaticBuf ((VOS_UINT8*)pDoneBuff))
    {
        Dms_FreeStaticBuf((VOS_UINT8*)pDoneBuff);
    }
    else
    {
        if(pDoneBuff != NULL)
        {
#if (VOS_LINUX== VOS_OS_VER)
            kfree(pDoneBuff);
#endif
        }
    }

    return;
}



