

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


#define THIS_FILE_ID LMSP_FILE_ID_DMS_ACM_AT_TX_C

DMS_STATIC_BUF_STRU stDmsStaticBufInfo ;

#if (VOS_WIN32== VOS_OS_VER)

VOS_UINT8 aucStaticBuf[DMS_LOG_STATIC_ONE_BUF_SIZE*DMS_LOG_STATIC_BUF_NUM + 32] = {0};
#endif
VOS_UINT8 *g_aucStaticBuf = NULL;

extern VOS_UINT32 dms_debug_flag;

/*****************************************************************************
函 数 名  : writeAtData
功能描述  : AT写USB数据总入口
输入参数  : ulClientId: 客户端ID
          aucDataBuf: 发送数据指针
          ulLen:发送长度
输出参数  :
返 回 值  : ERR_MSP_FAILURE/ERR_MSP_SUCCESS
调用函数  :
被调函数  : Dms_WriteAtData
修改历史  :
1.日    期  : 2012年8月27日
  作    者  : heliping
  修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 writeAtData(VOS_UINT32 ulClientId, VOS_UINT8 *aucDataBuf, VOS_UINT32 ulLen)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT8 *pSenBuf  = VOS_NULL;

    if (VOS_NULL == aucDataBuf || 0 == ulLen)
    {
        return ERR_MSP_FAILURE;
    }

    /*发送 PCUI 通道的数据 */
    if (ulClientId == EN_DMS_AT_CLIENT_CTRL)
    {
        pSenBuf = Dms_GetStaticBuf(ulLen);

        if(pSenBuf ==VOS_NULL)
        {
            return ERR_MSP_FAILURE;
        }

        VOS_MemCpy(pSenBuf, aucDataBuf, ulLen);

        ret = dms_VcomWriteAsync(EN_DMS_BEARER_USB_COM_CTRL,pSenBuf,(VOS_UINT32)ulLen);

        if( ret != ERR_MSP_SUCCESS)
        {
            Dms_FreeStaticBuf( pSenBuf );
        }

    }
    /*发送 CTRL 通道的数据 */
    else if (ulClientId == EN_DMS_AT_CLIENT_ACM)
    {

        pSenBuf = Dms_GetStaticBuf(ulLen);

        if(pSenBuf ==VOS_NULL)
        {
            return ERR_MSP_FAILURE;
        }

        VOS_MemCpy(pSenBuf, aucDataBuf, ulLen);

        ret = dms_VcomWriteAsync(EN_DMS_BEARER_USB_COM4_AT,pSenBuf,(VOS_UINT32)ulLen);

        if( ret != ERR_MSP_SUCCESS)
        {
            Dms_FreeStaticBuf( pSenBuf );
        }

    }
    /*发送 NDIS CTRL 通道的数据 */
    else if(ulClientId == EN_DMS_AT_CLIENT_NCM)
    {
        pSenBuf = Dms_GetStaticBuf(ulLen);

        if(pSenBuf ==VOS_NULL)
        {
            return ERR_MSP_FAILURE;
        }

        VOS_MemCpy(pSenBuf, aucDataBuf, ulLen);

        ret = dms_NcmSendData(pSenBuf,(VOS_UINT32)ulLen);

        if( ret != ERR_MSP_SUCCESS)
        {
            Dms_FreeStaticBuf( pSenBuf );
        }
    }
    else
    {
        ret = ERR_MSP_INVALID_PARAMETER;
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : Dms_WriteAtData
 功能描述  : AT写数据总入口
 输入参数  : ucPortNo: 通道ID
             pData: 发送数据指针
             uslength:发送长度
 输出参数  :
 返 回 值  : ERR_MSP_FAILURE/ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_INT32 Dms_WriteAtData(VOS_UINT8 ucPortNo, VOS_UINT8* pData, VOS_UINT16 uslength)
{
    VOS_INT32 slRet = -1;
    VOS_UINT32 ulClientId = 0;

    if (VOS_TRUE == dms_debug_flag)
    {
        vos_printf("Dms_WriteAtData: PortNo = %d, len = %d, buf = %s\r\n", ucPortNo, uslength, pData);
    }

    if(EN_DMS_BEARER_USB_COM4_AT == ucPortNo)
    {
        ulClientId = EN_DMS_AT_CLIENT_ACM;
    }
    else if(EN_DMS_BEARER_USB_NCM == ucPortNo)
    {
        ulClientId =  EN_DMS_AT_CLIENT_NCM;
    }

    else if(EN_DMS_BEARER_USB_COM_CTRL == ucPortNo)
    {
        ulClientId = EN_DMS_AT_CLIENT_CTRL;
    }
    else
    {
        return slRet;
    }

    slRet = (VOS_INT32)writeAtData(ulClientId, pData, (VOS_UINT32)uslength);

    return slRet;
}

/*****************************************************************************
 函 数 名  : Dms_StaticBufInit
 功能描述  : AT发送静态buf初始化
 输入参数  :

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping上
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID Dms_StaticBufInit(VOS_VOID)
{
    VOS_UINT32 i = 0;
    VOS_UINT8 * pTemp = NULL;

#if (VOS_LINUX== VOS_OS_VER)
    VOS_UINT32 ulBufSize;

    ulBufSize = (DMS_LOG_STATIC_ONE_BUF_SIZE*DMS_LOG_STATIC_BUF_NUM + 32);
    g_aucStaticBuf = kmalloc(ulBufSize, GFP_KERNEL|__GFP_DMA);

    if(g_aucStaticBuf == VOS_NULL)
    {
        return ;
    }
#endif

#if (VOS_WIN32== VOS_OS_VER)
    g_aucStaticBuf = aucStaticBuf;
#endif

    /*取32字节对齐的地址*/
    pTemp = g_aucStaticBuf + (32 - ((VOS_UINT32 )g_aucStaticBuf%32));

    stDmsStaticBufInfo.enBufType      = EN_DMS_DYMIC_BUF_TYP;

    /* 初始化缓冲信息*/
    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++)
    {
        stDmsStaticBufInfo.stBufSta[i].buf    = (VOS_UINT8 *)(i * DMS_LOG_STATIC_ONE_BUF_SIZE + pTemp);
        stDmsStaticBufInfo.stBufSta[i].enBusy = EN_STATIC_BUF_STA_IDLE;
    }

    return ;

}
/*****************************************************************************
函 数 名  : Dms_GetStaticBuf
功能描述  : AT发送获取静态buf
输入参数  : ulLen :需要buf的长度

输出参数  :
返 回 值  : VOS_NULL/addr
调用函数  :
被调函数  :
修改历史  :
 1.日    期  : 2012年8月27日
   作    者  : heliping
   修改内容  : Creat Function
*****************************************************************************/

VOS_UINT8* Dms_GetStaticBuf(VOS_UINT32 ulLen)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 *buf = NULL;

    /*数据长度超过静态buf最大size，动态申请buf*/
    if(ulLen >DMS_LOG_STATIC_ONE_BUF_SIZE )
    {

#if (VOS_LINUX== VOS_OS_VER)
        buf = kmalloc(ulLen, GFP_KERNEL|__GFP_DMA);
#else
        buf = VOS_NULL;
#endif
        return (VOS_UINT8* )buf;
    }

    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++)
    {
        if (stDmsStaticBufInfo.stBufSta[i].enBusy == EN_STATIC_BUF_STA_IDLE)
        {
            stDmsStaticBufInfo.stBufSta[i].enBusy = EN_STATIC_BUF_STA_BUSY;

            return  stDmsStaticBufInfo.stBufSta[i].buf;
        }
    }

    /*极限场景下 如果静态buf用完，申请动态内存使用*/
#if (VOS_LINUX== VOS_OS_VER)
    buf = kmalloc(ulLen, GFP_KERNEL|__GFP_DMA);
#else
    buf = VOS_NULL;
#endif

    return (VOS_UINT8* )buf;

}

/*****************************************************************************
函 数 名  : Dms_IsStaticBuf
功能描述  : 判断是否是静态地址
输入参数  : buf: buf地址

输出参数  :
返 回 值  : TRUE/FALSE
调用函数  :
被调函数  :
修改历史  :
1.日    期  : 2012年8月27日
  作    者  : heliping
  修改内容  : Creat Function
*****************************************************************************/

 VOS_BOOL Dms_IsStaticBuf(VOS_UINT8 *buf)
 {

    if(( buf >= g_aucStaticBuf )
        &&(buf < g_aucStaticBuf +DMS_LOG_STATIC_ONE_BUF_SIZE * DMS_LOG_STATIC_BUF_NUM  +32))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/*****************************************************************************
 函 数 名  : Dms_FreeStaticBuf
 功能描述  : 释放静态空间
 输入参数  : buf: 被释放的buf地址

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID Dms_FreeStaticBuf( VOS_UINT8 * buf)
{
    VOS_UINT32 i = 0;

    if (NULL == buf)
    {
        return ;
    }

    /*静态buf释放*/
    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++)
    {
        if (stDmsStaticBufInfo.stBufSta[i].buf == buf)
        {
            stDmsStaticBufInfo.stBufSta[i].enBusy = EN_STATIC_BUF_STA_IDLE;
            return ;
        }
    }

    /*动态buf释放*/
    if(i == DMS_LOG_STATIC_BUF_NUM)
    {
#if (VOS_LINUX== VOS_OS_VER)
        kfree(buf );
#endif
    }


    return ;
}
/*****************************************************************************
 函 数 名  : dms_VcomWriteAsync
 功能描述  : USB异步写接口
 输入参数  : VcomId: 通道ID
             pucDataBuf: 发送指针
             slLen:发送长度
 输出参数  :
 返 回 值  : ERR_MSP_FAILURE/ERR_MSP_SUCCESS
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dms_VcomWriteAsync(VOS_UINT32 VcomId,VOS_UINT8 *pucDataBuf,VOS_UINT32 ulLen)
{
    VOS_INT32 ret = ERR_MSP_SUCCESS;
    ACM_WR_ASYNC_INFO  stVcom  = {0};
    VOS_INT32 slHandle = 0;
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();

    stVcom.pBuffer = (VOS_CHAR*)pucDataBuf;
    stVcom.u32Size = ulLen;

    /*LTE DIAG CTRL*/
    if (VcomId == EN_DMS_BEARER_USB_COM1_DIAG_CTRL)
    {
        slHandle = aenPhyBear[EN_DMS_BEARER_USB_COM1_DIAG_CTRL].slPortHandle;

        if(slHandle == UDI_INVALID_HANDLE)
        {
            return ERR_MSP_FAILURE;
        }

        DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM1_WRT, ulLen, 0, 0);

        ret = udi_ioctl((int)slHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

        if(ret == ERR_MSP_SUCCESS)
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM1_WRT_SUCC, ulLen, 0, 0);
        }

    }
    /*LTE DIAG DATA*/
    else if(VcomId == EN_DMS_BEARER_USB_COM2_DIAG_APP)
    {
        slHandle = aenPhyBear[EN_DMS_BEARER_USB_COM2_DIAG_APP].slPortHandle;

        if(slHandle == UDI_INVALID_HANDLE)
        {
            return ERR_MSP_FAILURE;
        }

        DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM2_WRT, ulLen, 0, 0);
        ret = udi_ioctl((int)slHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

        if(ret == ERR_MSP_SUCCESS)
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM2_WRT_SUCC, ulLen, 0, 0);
        }

    }
    /*AT PCUI*/
    else if(VcomId == EN_DMS_BEARER_USB_COM4_AT)
    {
        slHandle = aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].slPortHandle;

        if(slHandle == UDI_INVALID_HANDLE)
        {
            if (VOS_TRUE == dms_debug_flag)
            {
                vos_printf("dms_VcomWriteAsync: INVALID HANDLE\r\n");
            }

            return ERR_MSP_FAILURE;
        }

        if(aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].ucChanStat ==ACM_EVT_DEV_SUSPEND)
        {
            if (VOS_TRUE == dms_debug_flag)
            {
                vos_printf("dms_VcomWriteAsync: DEV SUSPEND\r\n");
            }

            return ERR_MSP_FAILURE;
        }

        DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_WRT, ulLen, 0, 0);

        ret = udi_ioctl((int)slHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

        if(ret == ERR_MSP_SUCCESS)
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_WRT_SUSS, ulLen, 0, 0);
        }

    }
    /*AT CTRL*/
    else if(VcomId == EN_DMS_BEARER_USB_COM_CTRL)
    {
        slHandle = aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle;

        if(slHandle == UDI_INVALID_HANDLE)
        {
            return ERR_MSP_FAILURE;
        }

        DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_WRT, ulLen, 0, 0);

        ret = udi_ioctl((int)slHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

        if(ret == ERR_MSP_SUCCESS)
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_WRT_SUSS, ulLen, 0, 0);
        }

    }
    else
    {
        ret = ERR_MSP_INVALID_PARAMETER;
    }

    return (VOS_UINT32)ret;
}



