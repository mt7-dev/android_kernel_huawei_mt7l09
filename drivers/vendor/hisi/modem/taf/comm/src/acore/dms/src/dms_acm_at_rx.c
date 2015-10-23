

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
#include <DrvInterface.h>

pComRecv pfnAcmReadData = VOS_NULL;

VOS_UINT32          dms_debug_flag = VOS_FALSE;
VOS_UINT32          dms_saveRdDtaCallbackAddr = 0xfffffff;
VOS_UINT32          dms_saveAcmReadDataAddr = 0xfffffff;
VOS_UINT32                              g_ulPcuiReadUdiCnt = 0;
VOS_UINT32                              g_ulCtrlReadUdiCnt = 0;

extern VOS_UINT32                       g_ulPcuiRxSem;
extern VOS_UINT32                       g_ulCtrlRxSem;
VOS_VOID dms_setdebugflag(VOS_UINT32 ulFlag)
{
    dms_debug_flag = ulFlag;
    return;
}

VOS_VOID dms_printDebugInfo(VOS_VOID)
{
    DMS_MAIN_INFO * pstMainInfo = dmsGetMainInfo();

    vos_printf("dms_printDebugInfo: dms_saveRdDtaCallbackAddr = %d,\r\n", dms_saveRdDtaCallbackAddr);
    vos_printf("dms_printDebugInfo: dms_saveAcmReadDataAddr = %d\r\n", dms_saveAcmReadDataAddr);

    vos_printf("dms_printDebugInfo: pfnAcmReadData = %d,\r\n", (VOS_UINT32)pfnAcmReadData);
    vos_printf("dms_printDebugInfo: pstMainInfo->pfnRdDtaCallback = %d\r\n", (VOS_UINT32)(pstMainInfo->pfnRdDtaCallback));

    return;
}

/*****************************************************************************
 函 数 名  : Dms_AtReadData
 功能描述  : AT读数据总入口
 输入参数  : enClientId: AT 客户端
              pDataBuf: 数据指针
              ulLen:数据长度
 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_INVALID_PARAMETER
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 Dms_AtReadData(DMS_AT_CLIENT_ENUM enClientId, VOS_UINT8 * pDataBuf, VOS_UINT32 ulLen)
{
    VOS_UINT8 ucPortNo = 0;
    VOS_UINT32 ulRet = 0;

    if((VOS_NULL == pfnAcmReadData) || (VOS_NULL == pDataBuf))
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if(EN_DMS_AT_CLIENT_ACM == enClientId)
    {
        ucPortNo = EN_DMS_BEARER_USB_COM4_AT;
    }

    else if(EN_DMS_AT_CLIENT_CTRL == enClientId)
    {
        ucPortNo = EN_DMS_BEARER_USB_COM_CTRL;
    }

    else
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if (VOS_TRUE == dms_debug_flag)
    {
        vos_printf("Dms_AtReadData: PortNo = %d, len = %d, buf = %s\r\n", ucPortNo, ulLen, pDataBuf);
    }

   ulRet = (VOS_UINT32)pfnAcmReadData(ucPortNo, pDataBuf, (VOS_UINT16)ulLen);

    return ulRet;
}

/*****************************************************************************
 函 数 名  : DMS_ACMRecvFuncReg
 功能描述  : AT数据接收函数注册接口
 输入参数  : pCallback: 回调函数指针

 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_INVALID_PARAMETER
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_INT32 DMS_ACMRecvFuncReg(pComRecv pCallback)
{
    DMS_MAIN_INFO * pstMainInfo = dmsGetMainInfo();

    if (VOS_NULL == pCallback)
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    pstMainInfo->pfnRdDtaCallback = Dms_AtReadData;
    pfnAcmReadData = pCallback;

    dms_saveRdDtaCallbackAddr = (VOS_UINT32)Dms_AtReadData;
    dms_saveAcmReadDataAddr = (VOS_UINT32)pCallback;

    return ERR_MSP_SUCCESS;
}




VOS_VOID ctrlAtReadProc(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    static VOS_UINT8 aucStaticDtaStore[DMS_RD_BUF_LEN + 32];
    VOS_UINT32 ulReadLen               = 0;
    DMS_READ_DATA_PFN pstGetRdFun = NULL;
/*    DMS_LOGIC_CHAN_ENUM enLogic = EN_DMS_CHANNEL_LAST; */
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();
    VOS_UINT8 *pucCtrlAtStore = NULL;

    /*取32字节对齐的地址*/
    pucCtrlAtStore =&(aucStaticDtaStore[32 -(VOS_UINT32)(aucStaticDtaStore)%32]);
    g_ulCtrlReadUdiCnt++;
    if (dms_GetPortHandle(EN_DMS_BEARER_USB_COM_CTRL ) == FALSE)
    {
        VOS_TaskDelay(DMS_RD_SLEEP_TIME);
    }
    else
    {
/*        enLogic = aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].aenLogicChan; */

        ret =dms_UdiRead(EN_DMS_BEARER_USB_COM_CTRL,pucCtrlAtStore, DMS_RD_BUF_LEN, &ulReadLen);

        if((ret == ERR_MSP_SUCCESS)&&(ulReadLen != 0))
        {
            dmsAtCtrlTaskSetSleepFlag(FALSE);

            /*获取连接通知回调*/
            aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].ucChanStat = ACM_EVT_DEV_READY;

            /*获取读回调函数*/
            pstGetRdFun = dmsGetReadFun();

            if (NULL != pstGetRdFun)
            {
                (VOS_VOID)pstGetRdFun(EN_DMS_AT_CLIENT_CTRL, pucCtrlAtStore, ulReadLen);
            }

            dmsAtCtrlTaskSetSleepFlag(TRUE);

        }
        else
        {
            VOS_TaskDelay(100);
        }
    }
}

VOS_VOID vcomAtReadProc(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    static VOS_UINT8 aucStaticDtaStore[DMS_RD_BUF_LEN + 32];
    VOS_UINT32 ulReadLen               = 0;
    DMS_READ_DATA_PFN pstGetRdFun = NULL;
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();
    VOS_UINT8 *pucVcomAtStore = NULL;

    /*取32字节对齐的地址*/
    pucVcomAtStore =&(aucStaticDtaStore[32 - (VOS_UINT32)(aucStaticDtaStore)%32]);

    g_ulPcuiReadUdiCnt++;

    if (dms_GetPortHandle(EN_DMS_BEARER_USB_COM4_AT ) == FALSE)
    {
        VOS_TaskDelay(DMS_RD_SLEEP_TIME);
    }
    else
    {
        ret = dms_UdiRead(EN_DMS_BEARER_USB_COM4_AT,pucVcomAtStore, DMS_RD_BUF_LEN, &ulReadLen);

        if (VOS_TRUE == dms_debug_flag)
        {
            vos_printf("vcomAtReadProc: dms_UdiRead ret = %d, len = %d\r\n", ret, ulReadLen);
        }

        if ((ret == ERR_MSP_SUCCESS)&&(ulReadLen != 0))
        {
            dmsAtPcuiTaskSetSleepFlag(FALSE);

            aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].ucChanStat = ACM_EVT_DEV_READY;

            /*获取读回调函数*/
            pstGetRdFun = dmsGetReadFun();

            if (NULL != pstGetRdFun)
            {
                if (VOS_TRUE == dms_debug_flag)
                {
                    vos_printf("vcomAtReadProc: call AT reg func\r\n");
                }

                (VOS_VOID)pstGetRdFun(EN_DMS_AT_CLIENT_ACM, pucVcomAtStore, ulReadLen);
            }

            dmsAtPcuiTaskSetSleepFlag(TRUE);
        }
        else
        {
            VOS_TaskDelay(100);
        }
    }

}




VOS_VOID dms_VcomCtrlAtTask(VOS_VOID)
{
#if (VOS_OS_VER == VOS_WIN32)
    VOS_UINT32 i;
    for(i = 0; i<1; i++)
#else
    for(;;)
#endif
    {
        if (FALSE == dms_GetPortHandle(EN_DMS_BEARER_USB_COM_CTRL))
        {
            if (VOS_OK != VOS_SmP(g_ulCtrlRxSem, 0))
            {
                vos_printf("dms_AtPcuiTask: VOS_SmP fail!\n");
            }
        }
        ctrlAtReadProc();
    }
}



VOS_VOID dms_AtPcuiTask(VOS_VOID)
{
#if (VOS_OS_VER == VOS_WIN32)
    VOS_UINT32 i;
    for(i = 0; i<1; i++)
#else
    for(;;)
#endif
    {
        if (FALSE == dms_GetPortHandle(EN_DMS_BEARER_USB_COM4_AT))
        {
            if (VOS_OK != VOS_SmP(g_ulPcuiRxSem, 0))
            {
                vos_printf("dms_AtPcuiTask: VOS_SmP fail!\n");
            }
        }
        vcomAtReadProc();
    }
}

/*****************************************************************************
 函 数 名  : dms_VcomAtPcuiEvtCB
 功能描述  : AT PCUI口事件处理回调函数
 输入参数  : ulEvt: 事件类型

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID dms_VcomAtPcuiEvtCB(ACM_EVT_E  ulEvt)
{
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();
/*    DMS_LOGIC_CHAN_ENUM enLogic = EN_DMS_CHANNEL_LAST; */

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_EVT, ulEvt, 0, 0);

    if(ACM_EVT_DEV_READY==ulEvt)
    {
        aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].ucChanStat = ACM_EVT_DEV_READY;
        return ;
    }
    else
    {
/*        enLogic = aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].aenLogicChan; */

        aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].ucChanStat =ACM_EVT_DEV_SUSPEND;

        /*恢复默认配置*/
        aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].aenLogicChan = EN_DMS_CHANNEL_AT;
    }
    return;
}

/*****************************************************************************
 函 数 名  : dms_VcomCtrlEvtCB
 功能描述  : AT CTRL口事件处理回调函数
 输入参数  : ulEvt: 事件类型

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID dms_VcomCtrlEvtCB(ACM_EVT_E  ulEvt)
{
    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();
/*    DMS_LOGIC_CHAN_ENUM enLogic = EN_DMS_CHANNEL_LAST; */

    if(ACM_EVT_DEV_READY==ulEvt)
    {
        aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].ucChanStat = ACM_EVT_DEV_READY;
        return ;
    }
    else
    {
/*        enLogic = aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].aenLogicChan; */

        aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].ucChanStat =ACM_EVT_DEV_SUSPEND;

        /*恢复默认配置*/
        aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].aenLogicChan = EN_DMS_CHANNEL_AT;
    }
    return;
}

/*****************************************************************************
 函 数 名  : dms_VcomAtPcuiWrtCB
 功能描述  : AT PCUI口写完成回调函数
 输入参数  : pDoneBuff: 写指针
             s32DoneSize: 完成写入的长度
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID dms_VcomAtPcuiWrtCB (char* pDoneBuff, int s32DoneSize)
{
    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_WRT_CB,(VOS_UINT32)pDoneBuff , (VOS_UINT32)s32DoneSize, 0);

    if(s32DoneSize < 0)
    {
       DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_WRT_CB_ERR,(VOS_UINT32)pDoneBuff , (VOS_UINT32)s32DoneSize, 0);
    }

    if(Dms_IsStaticBuf ((VOS_UINT8*)pDoneBuff))
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

/*****************************************************************************
 函 数 名  : dms_VcomCtrlWrtCB
 功能描述  : AT CTRL口写完成回调函数
 输入参数  : pDoneBuff: 写指针
             s32DoneSize: 写完成长度
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID dms_VcomCtrlWrtCB (char* pDoneBuff, int s32DoneSize)
{
    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_WRT_CB,(VOS_UINT32)pDoneBuff , (VOS_UINT32)s32DoneSize, 0);

    if(s32DoneSize <= 0)
    {
       DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_WRT_CB_ERR,(VOS_UINT32)pDoneBuff , (VOS_UINT32)s32DoneSize, 0);
    }

    if(Dms_IsStaticBuf ((VOS_UINT8*)pDoneBuff))
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


/*****************************************************************************
 函 数 名  : dmsVcomAtPcuiOpen
 功能描述  : 打开PCUI口
 输入参数  :

 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_FAILURE
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dmsVcomAtPcuiOpen(VOS_VOID)
{
    VOS_INT32 slUartHd = 0;
    VOS_UINT32 ret      = ERR_MSP_SUCCESS;
    DMS_PHY_BEAR_PROPERTY_STRU *pstVcomCfg;
    UDI_OPEN_PARAM  stParam ;

    pstVcomCfg =  dmsgetPhyBearProperty();

    if(pstVcomCfg[EN_DMS_BEARER_USB_COM4_AT].slPortHandle !=UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_OPEN, 0, 0, 0);

    stParam.devid =(UDI_DEVICE_ID)UDI_USB_ACM_AT;

    slUartHd = udi_open(&stParam);

    if (UDI_INVALID_HANDLE != slUartHd)
    {
 #if (VOS_LINUX== VOS_OS_VER)
        /*注册USB事件回调*/
        if( 0 > udi_ioctl((int)slUartHd, (int)ACM_IOCTL_SET_EVT_CB, dms_VcomAtPcuiEvtCB))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 0);
        }

        /*注册异步写完成回调*/
        if( 0 >udi_ioctl((int)slUartHd, (int)UDI_IOCTL_SET_WRITE_CB, dms_VcomAtPcuiWrtCB))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 1);
        }

        /*设置该通道发送buf为非拷贝方式*/
        if( 0 >udi_ioctl((int)slUartHd, (int)ACM_IOCTL_WRITE_DO_COPY, 0))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 2);
        }
#endif
        pstVcomCfg[EN_DMS_BEARER_USB_COM4_AT].slPortHandle = slUartHd;
        VOS_SmV(g_ulPcuiRxSem);
        return ret;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 3);

    return ERR_MSP_FAILURE;

}

/*****************************************************************************
 函 数 名  : dmsVcomCtrolOpen
 功能描述  : 打开AT CTRL 口
 输入参数  :

 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_FAILURE
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dmsVcomCtrolOpen(VOS_VOID)
{
    VOS_INT32 slUartHd = UDI_INVALID_HANDLE;
    VOS_UINT32 ret      = ERR_MSP_SUCCESS;
    DMS_PHY_BEAR_PROPERTY_STRU *pstVcomCfg;

    UDI_OPEN_PARAM  stParam ;

    pstVcomCfg =  dmsgetPhyBearProperty();

    if(pstVcomCfg[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle !=UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_OPEN, 0, 0, 0);

    stParam.devid =(UDI_DEVICE_ID)UDI_USB_ACM_CTRL;

    slUartHd = udi_open(&stParam);

    if (UDI_INVALID_HANDLE != slUartHd)
    {

#if (VOS_LINUX== VOS_OS_VER)

         /*注册USB事件回调*/
        if( 0 > udi_ioctl((int)slUartHd, (int)ACM_IOCTL_SET_EVT_CB, dms_VcomCtrlEvtCB))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 0);
        }

        /*注册异步写完成回调*/
        if( 0 >udi_ioctl((int)slUartHd, (int)UDI_IOCTL_SET_WRITE_CB, dms_VcomCtrlWrtCB))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 1);
        }

        /*设置该通道发送buf为非拷贝方式*/
        if( 0 >udi_ioctl((int)slUartHd, (int)ACM_IOCTL_WRITE_DO_COPY, 0))
        {
            DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 2);
        }
#endif
        pstVcomCfg[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle = slUartHd;
        VOS_SmV(g_ulCtrlRxSem);
        return ret;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_OPEN_ERR, (VOS_UINT32)slUartHd, 0, 3);

    return ERR_MSP_FAILURE;

}

/*****************************************************************************
 函 数 名  : dmsVcomAtPcuiClose
 功能描述  : 关闭PCUI通道
 输入参数  :

 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_FAILURE
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dmsVcomAtPcuiClose(VOS_VOID)
{
    VOS_INT32 slVcomRet = ERR_MSP_SUCCESS;

    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();

    if(aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].slPortHandle ==UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_CLOSE, 0, 0, 0);

    slVcomRet = udi_close((int)aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].slPortHandle);

    if (slVcomRet == ERR_MSP_SUCCESS)
    {
        aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].slPortHandle = UDI_INVALID_HANDLE;

        aenPhyBear[EN_DMS_BEARER_USB_COM4_AT].ucChanStat =ACM_EVT_DEV_SUSPEND;

        return (VOS_UINT32)slVcomRet;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_AT_CLOSE_ERR, (VOS_UINT32)slVcomRet, 0, 0);

    return ERR_MSP_FAILURE;

}

/*****************************************************************************
 函 数 名  : dmsVcomCtrlClose
 功能描述  : 关闭 AT CTRL通道
 输入参数  :

 输出参数  :
 返 回 值  : ERR_MSP_SUCCESS/ERR_MSP_FAILURE
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月27日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 dmsVcomCtrlClose(VOS_VOID)
{
    VOS_INT32 slVcomRet = ERR_MSP_SUCCESS;

    DMS_PHY_BEAR_PROPERTY_STRU* aenPhyBear = dmsgetPhyBearProperty();

    if(aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle ==UDI_INVALID_HANDLE)
    {
        return ERR_MSP_SUCCESS;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_CLOSE, 0, 0, 0);

    slVcomRet = udi_close((int)aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle);

    if (slVcomRet == ERR_MSP_SUCCESS)
    {
        aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].slPortHandle = UDI_INVALID_HANDLE;
        aenPhyBear[EN_DMS_BEARER_USB_COM_CTRL].ucChanStat =ACM_EVT_DEV_SUSPEND;

        return (VOS_UINT32)slVcomRet;
    }

    DMS_DEBUG_SDM_FUN(EN_SDM_DMS_VCOM_CTRL_CLOSE_ERR, (VOS_UINT32)slVcomRet, 0, 0);

    return ERR_MSP_FAILURE;

}

VOS_UINT32 DMS_OpenHsicPort()
{
    return 0;
}

VOS_VOID DMS_ShowReadCnt(VOS_VOID)
{
    vos_printf("=======DMS Read Count\n");
    vos_printf("pcui        : %u \n", g_ulPcuiReadUdiCnt);
    vos_printf("ctrl        : %u \n", g_ulCtrlReadUdiCnt);
}
