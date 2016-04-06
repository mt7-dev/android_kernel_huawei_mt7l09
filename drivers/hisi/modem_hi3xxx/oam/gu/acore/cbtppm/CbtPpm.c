

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
//#include "AtAppVcomInterface.h"
#include "CbtPpm.h"
#include "CbtCpm.h"
#include "OmCommonPpm.h"
#include "OmAppRl.h"
#include "OmApp.h"
#include "SCMSoftDecode.h"
#include "omprivate.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_CBT_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
VOS_SEM                            g_ulCbtUsbPseudoSyncSemId;   /* 伪造为同步接口需使用的信号量 */

/* 用于ACPU上CBT端口的UDI句柄 */
UDI_HANDLE                         g_ulCbtPortUDIHandle    = VOS_ERROR;

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
extern  CBTCPM_RCV_FUNC                 g_pCbtRcvFunc;
extern  CBTCPM_SEND_FUNC                g_pCbtSndFunc;
extern  OM_VCOM_DEBUG_INFO         g_stVComDebugInfo[3];
extern  OMRL_CBT_HDLC_ENCODE_MEM_CTRL           g_stCbtHdlcEncBuf;

extern VOS_UINT32 CBTSCM_SoftDecodeDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen);

/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 CBTPPM_OamCbtPortDataSnd(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    CBTCPM_SEND_FUNC                    pFunc;

    pFunc = CBTCPM_GetSndFunc();

    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_ERR;
    }

    return pFunc(pucVirAddr, pucPhyAddr, ulDataLen);
}


VOS_UINT32 CBTPPM_OamUsbCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    VOS_INT32                           lRet;
    ACM_WR_ASYNC_INFO                   stVcom;
    VOS_UINT32                          ulInSlice;
    VOS_UINT32                          ulOutSlice;
    VOS_UINT32                          ulWriteSlice;

    if ((VOS_NULL_PTR == pucVirAddr) || (VOS_NULL_PTR == pucPhyAddr))
    {
        /* 打印错误 */
        LogPrint("\r\n CBTPPM_OamUsbCbtSendData: Vir or Phy Addr is Null \n");

        return CPM_SEND_PARA_ERR;
    }

    stVcom.pVirAddr = (VOS_CHAR*)pucVirAddr;
    stVcom.pPhyAddr = (VOS_CHAR*)pucPhyAddr;
    stVcom.u32Size  = ulDataLen;
    stVcom.pDrvPriv = VOS_NULL_PTR;

    g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteNum1++;

    ulInSlice = OM_GetSlice();

    lRet = DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteNum2++;

    ulOutSlice = OM_GetSlice();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = ulInSlice - ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteMaxTime = ulWriteSlice;
    }

    if (BSP_OK == lRet)     /*当前发送成功*/
    {
        if (VOS_OK != VOS_SmP(g_ulCbtUsbPseudoSyncSemId, 0))
        {
            LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: DRV_UDI_IOCTL Send Data return Error %d\n", lRet);
            return VOS_ERR;
        }

        return VOS_OK;
    }
    else if (BSP_OK > lRet)    /*临时错误*/
    {
        /*打印信息，调用UDI接口的错误信息*/
        LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: DRV_UDI_IOCTL Send Data return Error %d\n", lRet);

        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrValue  = (VOS_UINT32)lRet;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrTime   = OM_GetSlice();

        return VOS_ERR; /*对于临时错误，需要返回NULL丢弃数据*/
    }
    else    /*其他错误需要复位单板*/
    {
        /*打印信息，调用UDI接口*/
        LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: DRV_UDI_IOCTL Send Data return Error %d\n", lRet);

        DRV_SYSTEM_ERROR(OAM_USB_SEND_ERROR, (VOS_INT)THIS_FILE_ID, (VOS_INT)__LINE__,
                        (VOS_CHAR *)&lRet, sizeof(VOS_INT32));

        return VOS_ERR;
    }

}
VOS_VOID CBTPPM_OamUsbCbtPortClose(VOS_VOID)
{
    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        return;
    }

    DRV_UDI_CLOSE(g_ulCbtPortUDIHandle);

    g_ulCbtPortUDIHandle = VOS_ERROR;

    /* CBT端口从USB或VCOM切换到UART时接收函数指针为空，收到USB状态变更时不做断开处理 */
    if (VOS_NULL_PTR == CBTCPM_GetRcvFunc())
    {
        return;
    }

    PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CBT);

    return;
}
VOS_VOID CBTPPM_OamUsbCbtWriteDataCB(VOS_UINT8* pucVirData, VOS_UINT8* pucPhyData, VOS_INT lLen)
{
    if (lLen < 0)
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtWriteDataCB: lLen < 0. \n");
    }

    /* 伪同步接口，释放信号量 */
    VOS_SmV(g_ulCbtUsbPseudoSyncSemId);

    return;
}
VOS_VOID CBTPPM_OamUsbCbtStatusCB(ACM_EVT_E enPortState)
{
    /* CBT端口从USB或VCOM切换到UART时接收函数指针为空，收到USB状态变更时不做断开处理 */
    if (VOS_NULL_PTR == CBTCPM_GetRcvFunc())
    {
        return;
    }

    /* CBT端口只处理GU的端口断开 */
    PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CBT);

    return;
}


VOS_VOID CBTPPM_OamUsbCbtPortOpen(VOS_VOID)
{
    CBTPPM_OamCbtPortDataInit(OM_USB_CBT_PORT_HANDLE,
                              CBTPPM_OamUsbCbtReadDataCB,
                              CBTPPM_OamUsbCbtWriteDataCB,
                              CBTPPM_OamUsbCbtStatusCB);

    return;
}


VOS_INT32 CBTPPM_OamUsbCbtReadDataCB(VOS_VOID)
{
    ACM_WR_ASYNC_INFO                   stInfo;
    CBTCPM_RCV_FUNC                     pFunc;

    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB: Input HANDLE  is err. \n");

        return VOS_ERR;
    }

    VOS_MemSet(&stInfo, 0, sizeof(stInfo));

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB:Call DRV_UDI_IOCTL is Failed\n");

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size, OM_ACPU_USB_CB);
    /*lint +e40*/

    /* 数据接收函数 */
    pFunc = CBTCPM_GetRcvFunc();

    if (VOS_NULL_PTR != pFunc)
    {
        if (VOS_OK != pFunc((VOS_UINT8 *)stInfo.pVirAddr, stInfo.u32Size))
        {
            /* 增加可维可测计数 */
        }
    }

    if(VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB:Call DRV_UDI_IOCTL UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID CBTPPM_OamCbtPortDataInit(OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                        VOS_VOID                            *pReadCB,
                                        VOS_VOID                            *pWriteCB,
                                        VOS_VOID                            *pStateCB)
{
    UDI_OPEN_PARAM                      stUdiPara;
    ACM_READ_BUFF_INFO                  stReadBuffInfo;


    stUdiPara.devid            = UDI_ACM_OM_ID;

    stReadBuffInfo.u32BuffSize = OM_ICC_BUFFER_SIZE;
    stReadBuffInfo.u32BuffNum  = OM_DRV_MAX_IO_COUNT;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = OM_GetSlice();

    if (VOS_ERROR != g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit: The UDI Handle is not Null !");

        return;
    }

    /* 打开CBT使用的USB通道 */
    g_ulCbtPortUDIHandle = DRV_UDI_OPEN(&stUdiPara);

    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit: Open UDI ACM failed!");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = OM_GetSlice();

    /* 配置CBT使用的USB通道缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");

        return;
    }

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_SEND_BUFF_CAN_DMA, &stReadBuffInfo))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SEND_BUFF_CAN_DMA Failed\r\n");

        return;
    }

    /* 注册OM使用的USB读数据回调函数 */
    if (VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");

        return;
    }

    if(VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if(VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SET_EVT_CB Failed\r\n");

        return;
    }

    if (VOS_OK != DRV_UDI_IOCTL(g_ulCbtPortUDIHandle, ACM_IOCTL_WRITE_DO_COPY, VOS_NULL_PTR))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_WRITE_DO_COPY Failed\r\n");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = OM_GetSlice();

    return;
}


VOS_UINT32 CBTPPM_OamUsbCbtPortInit(VOS_VOID)
{
    /* CBT端口与PCVOICE复用，动态注册数据接收函数 */
    CBTCPM_PortRcvReg(CBTSCM_SoftDecodeDataRcv);

    /* 动态注册数据发送函数 */
    CBTCPM_PortSndReg(CBTPPM_OamUsbCbtSendData);

    /* USB承载的CBT端口，调用底软的异步接口发送数据，现在需要伪造成同步接口，申请信号量 */
    if(VOS_OK != VOS_SmCCreate("UCBT", 0, VOS_SEMA4_FIFO, &g_ulCbtUsbPseudoSyncSemId))
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtPortInit: create g_ulCbtUsbPseudoSyncSemId failedd\r\n");

        return VOS_ERR;
    }

    /* USB承载的CBT端口初始化UDI设备句柄 */
    g_ulCbtPortUDIHandle = VOS_ERROR;

    /* CBT端口通过CBT的CPM管理，不注册物理发送函数 */
    DRV_USB_REGUDI_ENABLECB((USB_UDI_ENABLE_CB_T)CBTPPM_OamUsbCbtPortOpen);

    DRV_USB_REGUDI_DISABLECB((USB_UDI_DISABLE_CB_T)CBTPPM_OamUsbCbtPortClose);

    return VOS_OK;
}
VOS_INT CBTPPM_OamVComCbtReadData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 ullength)
{
    CBTCPM_RCV_FUNC                     pFunc;

    if (ucDevIndex != APP_VCOM_DEV_INDEX_LOG)
    {
        vos_printf("\r\n CBTPPM_OamVComCbtReadData:PhyPort port is error: %d\n", ucDevIndex);

        return VOS_ERR;
    }

    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvNum++;
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvLen += ullength;

    if ((VOS_NULL_PTR == pData) || (0 == ullength))
    {
        vos_printf("\r\n CBTPPM_OamVComCbtReadData:Send data is NULL\n");

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pData, ullength, OM_ACPU_VCOM_CB);
    /*lint +e40*/

    pFunc = CBTCPM_GetRcvFunc();

    /* 数据接收函数 */
    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_ERR;
    }

    if (VOS_OK != pFunc((VOS_UINT8*)pData, ullength))
    {
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvErrNum++;
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvErrLen += ullength;

        vos_printf("\r\n Info: CBTPPM_OamVComCbtReadData:Call CBT Data Rcv Func fail\n");

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 CBTPPM_OamVComCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendNum++;
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendLen += ulDataLen;

    if (VOS_OK != APP_VCOM_Send(APP_VCOM_DEV_INDEX_LOG, pucVirAddr, ulDataLen))
    {
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendErrNum++;
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendErrLen += ulDataLen;

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID CBTPPM_OamVComCbtPortInit(VOS_VOID)
{
    /* CBT端口与PCVOICE复用，动态注册数据接收函数 */
    CBTCPM_PortRcvReg(CBTSCM_SoftDecodeDataRcv);

    /* 校准走VCOM28，会有数据下发，数据的回复不走CPM，直接发送 */
    APP_VCOM_RegDataCallback(APP_VCOM_DEV_INDEX_LOG, CBTPPM_OamVComCbtReadData);

    CBTCPM_PortSndReg(CBTPPM_OamVComCbtSendData);

    return;
}


VOS_VOID CBTPPM_OamCbtPortInit(VOS_VOID)
{
    OM_CHANNLE_PORT_CFG_STRU            stPortCfg;

    /* 读取OM的物理输出通道 */
    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        stPortCfg.enCbtPortNum = CPM_CBT_PORT_VCOM;
    }

    if (CPM_CBT_PORT_USB == stPortCfg.enCbtPortNum)
    {
        /* USB 承载的CBT端口的初始化 */
        CBTPPM_OamUsbCbtPortInit();
    }
    /* 默认都走VCOM做CBT */
    else
    {
        /* Vcom 口CBT通道的初始化 */
        CBTPPM_OamVComCbtPortInit();
    }

    return;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




