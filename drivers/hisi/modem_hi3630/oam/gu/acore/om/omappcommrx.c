/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : omappcommrx.c
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2011年3月11日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月11日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "OmApp.h"
#include "omrl.h"
#include "Omappagent.h"
#include "cpm.h"
#include "OmAppRl.h"
#include "omprivate.h"
#include "SCMProc.h"
#include "DrvInterface.h"
#include "FileSysInterface.h"
#include "AtAppVcomInterface.h"
#include "dms.h"

#if (FEATURE_ON == FEATURE_MERGE_OM_CHAN)
#include "OmCommonPpm.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define    THIS_FILE_ID        PS_FILE_ID_OM_COMMRX_C

#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)

VOS_UINT32                             g_ulAcpuFTMFlag = VOS_FALSE;

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 用于ACPU上USB设备的UDI句柄 */
UDI_HANDLE                              g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];

/* 用于ACPU上ICC通道的控制数据列表 */
OM_ICC_CHANNEL_CTRL_STRU                g_astOMACPUIccCtrlTable[OM_ICC_CHANNEL_BUTT];

/* 用于ACPU上SOCP接收 通道的统计信息 */
OM_ACPU_DEBUG_INFO                      g_stAcpuDebugInfo;

/* 用于记录 VCOM30 通道发送的统计信息 */
OM_VCOM_DEBUG_INFO                      g_stVComDebugInfo;

#if (FEATURE_ON == FEATURE_CBT_LOG)
/* 用于记录 VCOM27 通道发送的统计信息 */
OM_VCOM_CBT_LOG_DEBUG_INFO           g_stVComCBTDebugInfo;
#endif

VOS_BOOL                                g_GUOmOnCtrlPort    = VOS_FALSE;

VOS_UINT32                              g_ulUSBSendErrCnt   = 0;

VOS_SPINLOCK                            g_stVosErrLogSendSpinLock;  /* 自旋锁，用来作Err Log上报状态机的临界资源保护 */

#if (VOS_LINUX == VOS_OS_VER)
struct semaphore                        g_stVCOMRxBuffSem;
#endif
/*****************************************************************************
  3 外部引用声明
*****************************************************************************/

extern UDI_HANDLE                       g_OSAIccUDIHandle;

extern OM_RECORD_BUF_STRU               g_astAcpuRecordInfo[VOS_EXC_DUMP_MEM_NUM_BUTT];

extern OM_APP_MSG_RECORD_STRU           g_stOmAppMsgRecord;

extern OM_NV_MNTN_RECORD_STRU                  *g_pstRecordOmToApp;

extern OM_NV_MNTN_RECORD_STRU                  *g_pstRecordAppToOm;

#if(FEATURE_ON == FEATURE_PTM)
extern struct semaphore                        g_stOmRxErrorLogBuffSem;
#endif

#if (FEATURE_ON == FEATURE_CBT_LOG)
extern VOS_UINT32                     g_ulCBTLogEnable;
#endif

extern VOS_UINT V_ICC_OSAMsg_CB(VOS_UINT ulChannelID,VOS_INT lLen);

VOS_VOID SCM_LogToFile(FILE *fp);
VOS_VOID SOCP_LogToFile(FILE *fp);

VOS_INT OM_AcpuReadVComData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pucData, VOS_UINT32 uslength);

VOS_UINT32 OMRL_AcpuHdlcData(VOS_UINT8 *pucData, VOS_UINT32 ulLen);
VOS_VOID OM_SendAcpuSocpVote(SOCP_VOTE_TYPE_ENUM_U32 enVote);

/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 GU_OamAppSendDataSync(VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_INT32   lRetLen;

    /* 返回写入数据长度代表写操作成功 */
    lRetLen = DRV_UDI_WRITE(g_astOMPortUDIHandle[OM_APP_PORT_HANDLE], pucData, ulDataLen);

    if (VOS_OK != lRetLen)
    {
        /*打印信息，调用UDI接口*/
        LogPrint1("\r\nGU_OamAppSendDataSync: DRV_UDI_WRITE Send Data Error is %d", lRetLen);

        return VOS_ERR;
    }

    return VOS_OK;

}


VOS_UINT32 GU_OamPortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    VOS_INT32           lRet;
    ACM_WR_ASYNC_INFO   stVcom;
    VOS_UINT32          ulInSlice;
    VOS_UINT32          ulOutSlice;
    VOS_UINT32          ulWriteSlice;
    VOS_CHAR            aucUsbLog[100];
#ifndef FEATURE_USB_ZERO_COPY
    stVcom.pBuffer = (VOS_CHAR*)pucData;
    stVcom.u32Size = ulDataLen;
    stVcom.pDrvPriv= VOS_NULL_PTR;
#else
    stVcom.pVirAddr= pucData;
    stVcom.pPhyAddr= VOS_NULL_PTR;
    stVcom.u32Size = ulDataLen;
    stVcom.pDrvPriv= VOS_NULL_PTR;
#endif
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1++;

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\nGU_OamPortSend: USB HANDLE  is err. \n");

        return CPM_SEND_ERR;
    }

    ulInSlice = OM_GetSlice();

    /* 返回写入数据长度代表写操作成功 */
    lRet = DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2++;

    ulOutSlice = OM_GetSlice();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = ulInSlice - ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime = ulWriteSlice;
    }

    if (BSP_OK == lRet)     /*当前发送成功*/
    {
        return CPM_SEND_OK;
    }
    else if(BSP_OK > lRet)    /*临时错误*/
    {
        /*打印信息，调用UDI接口的错误信息*/
        LogPrint1("\r\nGU_OamPortSend: DRV_UDI_IOCTL Send Data return Error %d\n", lRet);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue  = (VOS_UINT32)lRet;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime   = OM_GetSlice();

        if (VOS_OK != OM_AcpuGetLogPath(aucUsbLog, OM_USB_LOG_FILE, OM_USB_UNITARY_LOG_FILE))
        {
            return CPM_SEND_FUNC_NULL;
        }

        if( 0 == (g_ulUSBSendErrCnt%USB_SEND_DATA_ERROR_MAX)) /*累计丢包超过限制需要记录到log文件中*/
        {
            OM_Acpu_WriteLogFile(aucUsbLog, (VOS_CHAR *)&g_stAcpuDebugInfo, sizeof(OM_ACPU_DEBUG_INFO));
        }

        g_ulUSBSendErrCnt++;

        return CPM_SEND_FUNC_NULL; /*对于临时错误，需要返回NULL丢弃数据*/
    }
    else    /*其他错误需要复位单板*/
    {
        /*打印信息，调用UDI接口*/
        LogPrint1("\r\nGU_OamPortSend: DRV_UDI_IOCTL Send Data return Error %d\n", lRet);

        DRV_SYSTEM_ERROR(OAM_USB_SEND_ERROR, lRet, (VOS_INT)enHandle,
                        (VOS_CHAR *)&g_stAcpuDebugInfo, sizeof(OM_ACPU_DEBUG_INFO));

        return CPM_SEND_ERR;
    }
}


VOS_UINT32 GU_OamAppSendData(VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    return GU_OamPortSend(OM_APP_PORT_HANDLE, pucData, ulDataLen);
}


VOS_UINT32 GU_OamCtrlSendData(VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    return GU_OamPortSend(OM_CTRL_PORT_HANDLE, pucData, ulDataLen);
}

/*****************************************************************************
 函 数 名  : GU_OamHsicSendData
 功能描述  : 将输入的数据通过Hsic 发送给PC侧
 输入参数  : pucData:   传递的数据
                            ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : VOS_ERROR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 GU_OamHsicSendData(VOS_UINT8 *pucData, VOS_UINT32 ulDataLen)
{
    return GU_OamPortSend(OM_HSIC_PORT_HANDLE, pucData, ulDataLen);
}

/*****************************************************************************
 函 数 名  : GU_OamPortWriteAsyCB
 功能描述  : 用于处理OM的异步发送数据的回调
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamPortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8* pucData, VOS_INT lLen)
{
    VOS_UINT32 ulResult;

    if(lLen < 0)        /*USB返回的错误值，保证可以打开SOCP的中断屏蔽*/
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCBErrNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCBErrTime = OM_GetSlice();

        SCM_RlsDestBuf(SOCP_CODER_DST_GU_OM, 0);
    }
    else
    {
        ulResult = SCM_RlsDestBuf(SOCP_CODER_DST_GU_OM, (VOS_UINT32)lLen);

        if(VOS_OK != ulResult)
        {
            LogPrint2("\r\nGU_OamUSBWriteDataCB: SCM_RlsDestBuf return Error 0x%x, Data Len is %d", (VOS_INT)ulResult, lLen);
            g_stAcpuDebugInfo.ulSocpReadDoneErrLen += lLen;
            g_stAcpuDebugInfo.ulSocpReadDoneErrNum++;
        }
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum++;

    return;
}
#ifdef FEATURE_USB_ZERO_COPY
VOS_VOID GU_OamAppWriteDataCB(VOS_UINT8* pucData, VOS_UINT8 *pucPhyAddr, VOS_INT lLen)
#else
VOS_VOID GU_OamAppWriteDataCB(VOS_UINT8* pucData, VOS_INT lLen)
#endif
{
    GU_OamPortWriteAsyCB(OM_APP_PORT_HANDLE, pucData, lLen);

    return;
}


#ifdef FEATURE_USB_ZERO_COPY
VOS_VOID GU_OamCtrlWriteDataCB(VOS_UINT8* pucData, VOS_UINT8 *pucPhyAddr, VOS_INT lLen)
#else
VOS_VOID GU_OamCtrlWriteDataCB(VOS_UINT8* pucData, VOS_INT lLen)
#endif
{
    /*当前只承载OM数据*/
    GU_OamPortWriteAsyCB(OM_CTRL_PORT_HANDLE, pucData, lLen);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamHsicWriteDataCB
 功能描述  : 用于处理Hsic 口的异步发送数据的回调
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
#ifdef FEATURE_USB_ZERO_COPY
VOS_VOID GU_OamHsicWriteDataCB(VOS_UINT8* pucData, VOS_UINT8 *pucPhyAddr, VOS_INT lLen)
#else
VOS_VOID GU_OamHsicWriteDataCB(VOS_UINT8* pucData, VOS_INT lLen)
#endif
{
    /*当前只承载OM数据*/
    GU_OamPortWriteAsyCB(OM_HSIC_PORT_HANDLE, pucData, lLen);

    return;
}
VOS_VOID GU_OamDisconnectPorts(CPM_PHY_PORT_ENUM_UINT32  enPhyPort)
{
    VOS_UINT8               aucBytes[] = {0x0a,0x00,0x08,0x00,0xb3,0x80,0x00,0x00,0x01,0x00,0x00,0x00};

    /* 断开连接 */
    CPM_DisconnectPorts(enPhyPort, CPM_OM_COMM);

#if (FEATURE_ON == FEATURE_CBT_LOG)
    if (OM_CBT_LOG_ENABLE_FLAG == g_ulCBTLogEnable)
    {
        return;
    }
#endif

    /* 关闭ACPU OM发送 */
    g_ulAcpuOMSwitchOnOff = OM_STATE_IDLE;

    OM_SendAcpuSocpVote(SOCP_VOTE_FOR_SLEEP);

    (VOS_VOID)GU_OamSndPcMsgToCcpu(aucBytes, sizeof(aucBytes));

    OM_NotifyOtherCpuReleaseLink();
}

/*****************************************************************************
 函 数 名  : GU_OamPortStatus
 功能描述  : 用于ACPU上面处理物理端口断开后的OM链接断开
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamPortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState)
{
    if(ACM_EVT_DEV_SUSPEND == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime = OM_GetSlice();

        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_COMM))
        {
            GU_OamDisconnectPorts(enPhyPort);

            g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsDrvSuspend++;
            g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();
        }
    }
    else if(ACM_EVT_DEV_READY == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime = OM_GetSlice();
    }
    else
    {
        LogPrint2("GU_OamUSBStatusCB: The USB Port %d State %d is Unknow", (VOS_INT)enPhyPort, (VOS_INT)enPortState);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime = OM_GetSlice();
    }

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamPortStatusCB
 功能描述  : 用于ACPU上面处理物理端口断开后的OM链接断开
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamAppStatusCB(ACM_EVT_E enPortState)
{
    if (CPM_APP_PORT == CPM_QueryPhyPort(CPM_PCVOICE_COMM))/*当前运行的是Pcvoice业务，不处理*/
    {
        LogPrint1("\r\n Info: GU_OamAppStatusCB: Pcvoice Rec The USB Port State %d", enPortState);

        return;
    }

    GU_OamPortStatus(OM_APP_PORT_HANDLE, CPM_APP_PORT, enPortState);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamCtrlStatusCB
 功能描述  : 用于ACPU上面处理物理端口断开后的OM链接断开
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamCtrlStatusCB(ACM_EVT_E enPortState)
{
    GU_OamPortStatus(OM_CTRL_PORT_HANDLE, CPM_CTRL_PORT, enPortState);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamHsicStatusCB
 功能描述  : 用于ACPU上面处理物理端口断开后的OM链接断开
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamHsicStatusCB(ACM_EVT_E enPortState)
{
    GU_OamPortStatus(OM_HSIC_PORT_HANDLE, CPM_HSIC_PORT, enPortState);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUSBReadData
 功能描述  : 用于ACPU上面底软把数据通过ICC发送给OM模块
 输入参数  :  enPhyPort: 物理端口
                            UdiHandle:设备句柄
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT32 GU_OamReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle)
{
    ACM_WR_ASYNC_INFO                   stInfo;
    CPM_PHY_PORT_ENUM_UINT32            enPcvoiceRet;
    CPM_PHY_PORT_ENUM_UINT32            enDiagRet;
    VOS_CHAR* pdata = NULL;

    if (VOS_ERROR == UdiHandle)
    {
        LogPrint("\r\nGU_OamReadPortData: Input HANDLE  is err. \n");

        return VOS_ERR;
    }

    VOS_MemSet(&stInfo, 0, sizeof(stInfo));

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(UdiHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\nGU_OamReadPortData:Call DRV_UDI_IOCTL is Failed\n");

        return VOS_ERR;
    }
#ifdef FEATURE_USB_ZERO_COPY
    pdata = stInfo.pVirAddr;
#else
    pdata = stInfo.pBuffer;
#endif

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pdata, stInfo.u32Size, OM_ACPU_USB_CB);
    /*lint +e40*/

    /*判断当前是否承载的是Pcvoice*/
    enPcvoiceRet = CPM_QueryPhyPort(CPM_PCVOICE_COMM);
    enDiagRet = CPM_QueryPhyPort(CPM_DIAG_COMM);
    if ((enPhyPort != enPcvoiceRet)&&(enPhyPort != enDiagRet))
    {
        CPM_PnpAppPortCB(enPhyPort, VOS_TRUE);  /*需要重新链接物理和逻辑端口关系*/
    }

    if(VOS_OK != CPM_ComRcv(enPhyPort, (VOS_UINT8*)pdata, stInfo.u32Size))
    {
        LogPrint1("\r\n Info: GU_OamReadPortData:Call CPM_ComRcv is NULL, PhyPort is %d\n", (VOS_INT)enPhyPort);
    }

    if(VOS_OK != DRV_UDI_IOCTL(UdiHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\nGU_OamReadPortData:Call DRV_UDI_IOCTL UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : GU_OamAppReadDataCB
 功能描述  : 用于ACPU上面底软把APP 口数据通过ICC发送给OM模块
 输入参数  :  enPhyPort: 物理端口
                            UdiHandle:设备句柄
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT32 GU_OamAppReadDataCB(VOS_VOID)
{
    return GU_OamReadPortData(CPM_APP_PORT, g_astOMPortUDIHandle[OM_APP_PORT_HANDLE]);
}

/*****************************************************************************
 函 数 名  : GU_OamCtrlReadDataCB
 功能描述  : 用于ACPU上面底软把Ctrl 口数据通过ICC发送给OM模块
 输入参数  :  enPhyPort: 物理端口
                            UdiHandle:设备句柄
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT32 GU_OamCtrlReadDataCB(VOS_VOID)
{
    return GU_OamReadPortData(CPM_CTRL_PORT, g_astOMPortUDIHandle[OM_CTRL_PORT_HANDLE]);
}

/*****************************************************************************
 函 数 名  : GU_OamHsicReadDataCB
 功能描述  : 用于ACPU上面底软把HSIC 口数据通过ICC发送给OM模块
 输入参数  :  enPhyPort: 物理端口
                            UdiHandle:设备句柄
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT32 GU_OamHsicReadDataCB(VOS_VOID)
{
    if (OM_HSIC_PORT_STATUS_OFF == OM_GetHsicPortStatus())
    {
        return VOS_OK;
    }

    return GU_OamReadPortData(CPM_HSIC_PORT, g_astOMPortUDIHandle[OM_HSIC_PORT_HANDLE]);
}


VOS_UINT32 GU_OamSndPcMsgToCcpu(VOS_UINT8 *pucData, VOS_UINT32 ulSize)
{
    VOS_INT32  lResult = VOS_ERROR;

    lResult = DRV_UDI_WRITE(g_astOMACPUIccCtrlTable[OM_OM_ICC_CHANNEL].UDIHdle, pucData, ulSize);

    /* 由于C核复位，写ICC通道失败会返回一个特殊值，不能复位单板 */
    if (BSP_ERR_ICC_CCORE_RESETTING == lResult )
    {
        LogPrint1("\n# GU_OamSndPcMsgToCcpu Error,Ccore Reset,ulSize %d .\n",(VOS_INT)ulSize);

        return VOS_ERR;
    }

    /* 当前写操作失败 */
    if(ulSize != lResult)
    {
        LogPrint2("GU_OamSndPcMsgToCcpu: The ICC UDI Write is Error.Size:%d,lResult:%d\n",(VOS_INT)ulSize,lResult);

        g_stAcpuPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrNum++;
        g_stAcpuPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrLen += ulSize;
        g_stAcpuPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrSlice= VOS_GetSlice();

        return VOS_ERR;
    }
    g_stAcpuPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendMsgNum++;
    g_stAcpuPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendLen += ulSize;
    g_stAcpuPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendSlice= VOS_GetSlice();

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : GU_OamReadPortDataInit
 功能描述  : 用于初始化OM使用的设备
 输入参数  : enPhyPort: 物理端口号
                           enHandle: 端口的句柄
                           pReadCB: 该端口上面的读取回调函数
                           pWriteCB: 该端口上面的异步写回调函数
                           pStateCB: 该端口上面的状态回调函数
 输出参数  : pUdiHandle: 端口的句柄
 返 回 值  : VOS_OK/VOS_ERR
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    VOS_VOID                            *pReadCB,
                                    VOS_VOID                            *pWriteCB,
                                    VOS_VOID                            *pStateCB)
{
    UDI_OPEN_PARAM      stUdiPara;
    ACM_READ_BUFF_INFO  stReadBuffInfo;

    /*初始化当前使用的USB通道*/
    if(CPM_APP_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_OM_ID;
    }
    else if(CPM_CTRL_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_CTRL_ID;
    }
    else if(CPM_HSIC_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_HSIC_ACM6_ID;
    }
    else
    {
        LogPrint1("\r\nGU_OamReadPortDataInit: Open Wrong Port %d!", (VOS_INT)enPhyPort);

        return;
    }

    stReadBuffInfo.u32BuffSize = OM_ICC_BUFFER_SIZE;
    stReadBuffInfo.u32BuffNum  = OM_DRV_MAX_IO_COUNT;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = OM_GetSlice();

    if (VOS_ERROR != g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\nGU_OamReadPortDataInit: The UDI Handle is not Null !");

        return;
    }

    /* 打开OM使用的USB通道 */
    g_astOMPortUDIHandle[enHandle] = DRV_UDI_OPEN(&stUdiPara);

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\nGU_OamReadPortDataInit: Open UDI ACM failed!");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = OM_GetSlice();

    /* 配置OM使用的USB通道缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");

        return;
    }

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SEND_BUFF_CAN_DMA, &stReadBuffInfo))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SEND_BUFF_CAN_DMA Failed\r\n");

        return;
    }

    /* 注册OM使用的USB读数据回调函数 */
    if (VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");

        return;
    }

    if(VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if(VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    /* 设置通道 0 copy */
    if(VOS_OK != DRV_UDI_IOCTL(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_DO_COPY, VOS_NULL_PTR))
    {
        LogPrint("\r\nGU_OamReadPortDataInit, DRV_UDI_IOCTL ACM_IOCTL_WRITE_DO_COPY Failed\r\n");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = OM_GetSlice();

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamAppPortOpen
 功能描述  : 用于初始化OM使用的App端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID GU_OamAppPortOpen(VOS_VOID)
{
    GU_OamReadPortDataInit(CPM_APP_PORT,
                            OM_APP_PORT_HANDLE,
                            GU_OamAppReadDataCB,
                            GU_OamAppWriteDataCB,
                            GU_OamAppStatusCB);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamCtrlPortOpen
 功能描述  : 用于初始化OM使用的Ctrl 端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID GU_OamCtrlPortOpen(VOS_VOID)
{
    GU_OamReadPortDataInit(CPM_CTRL_PORT,
                            OM_CTRL_PORT_HANDLE,
                            GU_OamCtrlReadDataCB,
                            GU_OamCtrlWriteDataCB,
                            GU_OamCtrlStatusCB);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamHsicPortOpen
 功能描述  : 用于初始化OM使用的Hsic端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/

VOS_VOID GU_OamHsicPortOpen(VOS_VOID)
{

    GU_OamReadPortDataInit(CPM_HSIC_PORT,
                           OM_HSIC_PORT_HANDLE,
                           GU_OamHsicReadDataCB,
                           GU_OamHsicWriteDataCB,
                           VOS_NULL_PTR);

    return;
}



VOS_VOID GU_OamPortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort)
{
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice = OM_GetSlice();

    if (enPhyPort == CPM_QueryPhyPort(CPM_OM_COMM))
    {
        GU_OamDisconnectPorts(enPhyPort);

        g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsPortClose++;
        g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();
    }

    if (VOS_ERROR != g_astOMPortUDIHandle[enHandle])
    {
        DRV_UDI_CLOSE(g_astOMPortUDIHandle[enHandle]);

        g_astOMPortUDIHandle[enHandle] = VOS_ERROR;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice = OM_GetSlice();
    }

    return;
}


VOS_VOID GU_OamAppPortClose(VOS_VOID)
{
    GU_OamPortCloseProc(OM_APP_PORT_HANDLE, CPM_APP_PORT);

    return;
}


VOS_VOID GU_OamCtrlPortClose(VOS_VOID)
{
    GU_OamPortCloseProc(OM_CTRL_PORT_HANDLE, CPM_CTRL_PORT);

    return;
}


VOS_VOID GU_OamHsicPortClose(VOS_VOID)
{
    GU_OamPortCloseProc(OM_HSIC_PORT_HANDLE, CPM_HSIC_PORT);

    return;
}


VOS_VOID GU_OamAppPortInit(VOS_VOID)
{
    DRV_USB_REGUDI_ENABLECB((USB_UDI_ENABLE_CB_T)GU_OamAppPortOpen);

    DRV_USB_REGUDI_DISABLECB((USB_UDI_DISABLE_CB_T)GU_OamAppPortClose);

    CPM_PhySendReg(CPM_APP_PORT,  GU_OamAppSendData);

    return;
}


VOS_VOID GU_OamCtrlPortInit(VOS_VOID)
{
    DRV_USB_REGUDI_ENABLECB((USB_UDI_ENABLE_CB_T)GU_OamCtrlPortOpen);

    DRV_USB_REGUDI_DISABLECB((USB_UDI_DISABLE_CB_T)GU_OamCtrlPortClose);

    GU_OamCtrlPortOpen();   /*当前切换的时候USB一定是连接的*/

    CPM_PhySendReg(CPM_CTRL_PORT,  GU_OamCtrlSendData);

    g_GUOmOnCtrlPort = VOS_TRUE;

    return;
}
VOS_VOID GU_OamHsicPortInit(VOS_VOID)
{
    /* 产品不支持HSIC特性，直接返回 */
    if (BSP_MODULE_SUPPORT != DRV_GET_HSIC_SUPPORT())
    {
        return;
    }

    /* 如果HSIC通道已经枚举成功，则由协议栈执行初始化操作；否则将初始化函数注册至底软，
        由底软在HSIC枚举成功后调用以进行初始化*/
    if (VOS_TRUE == DRV_GET_HSIC_ENUM_STATUS())
    {
        GU_OamHsicPortOpen();
    }
    else
    {
        DRV_HSIC_REGUDI_ENABLECB((HSIC_UDI_ENABLE_CB_T)GU_OamHsicPortOpen);
    }

    DRV_HSIC_REGUDI_DISABLECB((HSIC_UDI_DISABLE_CB_T)GU_OamHsicPortClose);

    CPM_PhySendReg(CPM_HSIC_PORT,  GU_OamHsicSendData);

    return;
}


VOS_UINT32 GU_OamVComReadData(VOS_UINT32 ulChan, VOS_UINT8 *pData, VOS_UINT32 uslength)
{
    if(ulChan != DMS_VCOM_OM_CHAN_GU_DATA)
    {
        vos_printf("\r\n GU_OamVComReadData:PhyPort port is error: %d\n", ulChan);

        return VOS_ERR;
    }

#if (VOS_LINUX == VOS_OS_VER)
    down(&g_stVCOMRxBuffSem);
#endif

    g_stVComDebugInfo.ulVCOMRcvNum++;
    g_stVComDebugInfo.ulVCOMRcvLen += uslength;

    if ((VOS_NULL_PTR == pData) || (0 == uslength))
    {

#if (VOS_LINUX == VOS_OS_VER)
        up(&g_stVCOMRxBuffSem);
#endif
        vos_printf("\r\n GU_OamVComReadData:Send data is NULL\n");

        return VOS_ERR;
    }

    if (CPM_PORT_BUTT == CPM_QueryPhyPort(CPM_OM_COMM))
    {
        CPM_ConnectPorts(CPM_VCOM_PORT, CPM_OM_COMM);
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pData, uslength, OM_ACPU_USB_CB);
    /*lint +e40*/

    if(VOS_OK != CPM_ComRcv(CPM_VCOM_PORT, pData, uslength))
    {
        g_stVComDebugInfo.ulVCOMRcvErrNum++;
        g_stVComDebugInfo.ulVCOMRcvErrLen += uslength;

        vos_printf("\r\n Info: GU_OamVComReadData:Call CPM_ComRcv is fail, PhyPort is CPM_VCOM_PORT \n");
    }

#if (VOS_LINUX == VOS_OS_VER)
    up(&g_stVCOMRxBuffSem);
#endif

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : GU_OamVComStatus
 功能描述  : 用于ACPU上面处理VCom端口断开后的OM链接断开
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年2月2日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID GU_OamVComStatus(VOS_UINT32 ulChan, VOS_UINT32 enPortState)
{
    if(DMS_CHAN_EVT_CLOSE == enPortState)
    {
        g_stVComDebugInfo.ulVCOMOutNum++;

        g_stVComDebugInfo.ulVCOMOutTime = OM_GetSlice();

        if (CPM_VCOM_PORT == CPM_QueryPhyPort(CPM_OM_COMM))
        {
            GU_OamDisconnectPorts(CPM_VCOM_PORT);

            g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsDrvSuspend++;

            g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();
        }
    }
    else if(DMS_CHAN_EVT_OPEN == enPortState)
    {
        g_stVComDebugInfo.ulVCOMInNum++;

        g_stVComDebugInfo.ulVCOMInTime = OM_GetSlice();
    }
    else
    {
        LogPrint1("GU_OamVComStatus: The VCOM Port State %d is Unknow", (VOS_INT)enPortState);
    }

    return;
}


VOS_UINT32 GU_OamVComSendData(VOS_UINT8 *pData, VOS_UINT32 uslength)
{
    g_stVComDebugInfo.ulVCOMSendNum++;
    g_stVComDebugInfo.ulVCOMSendLen += uslength;

    if(VOS_OK != DMS_WriteOmData(DMS_VCOM_OM_CHAN_GU_DATA, pData, uslength))
    {
        g_stVComDebugInfo.ulVCOMSendErrNum++;
        g_stVComDebugInfo.ulVCOMSendErrLen += uslength;

        return CPM_SEND_ERR;
    }

    return CPM_SEND_OK;
}


VOS_UINT32 GU_OamQueryLogPort(VOS_UINT32  *pulLogPort)
{
    if (VOS_NULL_PTR == pulLogPort)
    {
        vos_printf("\r\n GU_OamQueryLogPort: para is NULL !!\n");
        return VOS_ERR;
    }

    *pulLogPort = CPM_QueryPhyPort(CPM_OM_COMM);

    if ((CPM_APP_PORT != *pulLogPort) && (CPM_VCOM_PORT != *pulLogPort))
    {
        vos_printf("\r\n GU_OamQueryLogPort: log prot is not support. port:%d!!\n", *pulLogPort);
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 GU_OamLogPortSwitch(VOS_UINT32  ulPhyPort)
{
    CPM_PHY_PORT_ENUM_UINT32            enPhyPort;

    if ((CPM_VCOM_PORT != ulPhyPort) && (CPM_APP_PORT != ulPhyPort))
    {
        vos_printf("\r\n GU_OamPortSwitch: enPhyPort is error.port %d\n", ulPhyPort);
        return VOS_ERR;
    }

    enPhyPort = CPM_QueryPhyPort(CPM_OM_COMM);

    if (ulPhyPort != enPhyPort)
    {
        /* 断开连接 */
        GU_OamDisconnectPorts(enPhyPort);

        g_stAcpuPcToUeSucRecord.stRlsInfo.ulRlsPortSwitch++;
        g_stAcpuPcToUeSucRecord.stRlsInfo.ulSlice = OM_GetSlice();

        CPM_ConnectPorts(ulPhyPort, CPM_OM_COMM);
    }

    return VOS_OK;
}
VOS_VOID GU_OamVComPortInit(VOS_VOID)
{
    VOS_MemSet(&g_stVComDebugInfo, 0, sizeof(g_stVComDebugInfo));

#if (VOS_LINUX == VOS_OS_VER)
    sema_init(&g_stVCOMRxBuffSem, 1);
#endif

    DMS_RegOmChanDataReadCB(DMS_VCOM_OM_CHAN_GU_DATA, GU_OamVComReadData);

    DMS_RegOmChanEventCB(DMS_VCOM_OM_CHAN_GU_DATA, GU_OamVComStatus);

    CPM_PhySendReg(CPM_VCOM_PORT, GU_OamVComSendData);

#if (FEATURE_ON == FEATURE_CBT_LOG)
    APP_VCOM_RegDataCallback(APP_VCOM_DEV_INDEX_CBT, OMRL_AcpuRcvCBTData);
    VOS_MemSet(&g_stVComCBTDebugInfo, 0, sizeof(g_stVComCBTDebugInfo));
#endif

    return;
}


VOS_VOID GU_OamErrLogVComPortInit(VOS_VOID)
{
#if(FEATURE_ON == FEATURE_PTM)
    VOS_SpinLockInit(&g_stVosErrLogSendSpinLock);

    /* 商用ERR LOG上报全局变量初始化 */
    g_stOmAppMsgRecord.ulErrLogReportSend = 0;/* 记录Err Log需要上报组件 */
    g_stOmAppMsgRecord.ulFTMReportSend    = 0;/* 记录FTM需要上报组件 */
    g_stOmAppMsgRecord.pulErrorLogModule  = VOS_NULL_PTR;
    g_stOmAppMsgRecord.pulFTMModule       = VOS_NULL_PTR;
    g_stOmAppMsgRecord.usModemId          = MODEM_ID_BUTT;

#if (VOS_LINUX == VOS_OS_VER)
    sema_init(&g_stOmRxErrorLogBuffSem, 1);
#endif

    /* 注册收Vcom Error log函数给NAS */
    APP_VCOM_RegDataCallback(APP_VCOM_DEV_INDEX_ERRLOG, OM_AcpuReadVComData);
#endif
    return;
}


VOS_VOID GU_OamPortInit(VOS_VOID)
{
    VOS_MemSet(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    VOS_MemSet(g_astOMPortUDIHandle, VOS_ERROR, sizeof(g_astOMPortUDIHandle));

    GU_OamAppPortInit();    /* App 端口的初始化 */

    GU_OamHsicPortInit();   /* Hsic 端口的初始化 */

    /*Ctrl端口的初始化是在端口切换实现*/

    GU_OamVComPortInit();   /* Vcom 端口的初始化 */

    GU_OamErrLogVComPortInit(); /* Error log Vcom 端口的初始化 */

    return;
}

VOS_UINT GU_OamAcpu_ICCError_CB(VOS_UINT ulChanID, VOS_UINT ulEvent, VOS_VOID* pParam)
{
    VOS_UINT32 ulChannelID;

    if(g_astOMACPUIccCtrlTable[OM_OM_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        ulChannelID = OM_OM_ICC_CHANNEL;
    }
    else if(g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        ulChannelID = OM_OSA_MSG_ICC_CHANNEL;
    }
    else
    {
        LogPrint("OM_ComRx_ICCError_CB: The Channel ID is not used by OAM\n");

        return VOS_ERR;
    }

    LogPrint2("GU_OAMAcpu_ICCError_CB: The Error Channel ID is %d,and Event is %d\n",
                (VOS_INT32)ulChannelID, (VOS_INT32)ulEvent);

    return VOS_OK;
}


VOS_UINT32 OM_ComRx_ICC_Init(VOS_VOID)
{
    VOS_INT32       i;

    /*初始化 ICC通道*/
    VOS_MemSet(g_astOMACPUIccCtrlTable, 0, sizeof(g_astOMACPUIccCtrlTable));

    for(i=0; i<OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr
                                    = (ICC_CHAN_ATTR_S*)VOS_MemAlloc(ACPU_PID_OM,
                                                                     STATIC_MEM_PT,
                                                                     sizeof(ICC_CHAN_ATTR_S));

        if(VOS_NULL_PTR == g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr)
        {
            LogPrint1("OM_ComRx_ICC_Init: VOS_MemAlloc Failed, Index is %d.\n", i);

            return VOS_ERR;             /*分配内存失败单板会重启，因此不需要释放之前已经申请的内存*/
        }
    }

    g_astOMACPUIccCtrlTable[OM_OM_ICC_CHANNEL].stUDICtrl.enICCId                    = UDI_ICC_GUOM0_ID;

    g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.enICCId               = UDI_ICC_GUOM4_ID;
    g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.pstICCAttr->read_cb   = V_ICC_OSAMsg_CB;

    for(i=0; i<OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32Priority    = OM_ICC_CHANNEL_PRIORITY;
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32TimeOut     = OM_ICC_HANDSHAKE_TIME_MAX;

        if( i == OM_OSA_MSG_ICC_CHANNEL)
        {
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OSA_ICC_BUFFER_SIZE;
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OSA_ICC_BUFFER_SIZE;
        }
        else
        {
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OM_ICC_BUFFER_SIZE;
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OM_ICC_BUFFER_SIZE;
        }

        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->enChanMode     = ICC_CHAN_MODE_PACKET;
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->event_cb       = GU_OamAcpu_ICCError_CB;

        g_astOMACPUIccCtrlTable[i].UDIHdle = DRV_UDI_OPEN((UDI_OPEN_PARAM *)&(g_astOMACPUIccCtrlTable[i].stUDICtrl));

        if(VOS_ERROR == g_astOMACPUIccCtrlTable[i].UDIHdle)
        {
            /* 打开失败时记录当前ICC通道信息 */
            DRV_SYSTEM_ERROR(OM_APP_ICC_INIT_ERROR, THIS_FILE_ID, __LINE__,
                            (VOS_CHAR*)&i, sizeof(VOS_INT32));

            return VOS_ERR;
        }
    }

    g_OSAIccUDIHandle = g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle;

    return VOS_OK;
}
VOS_VOID GU_OamGetSendDataLen(VOS_UINT32 ulDataLen, VOS_UINT32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyport;

    enPhyport = CPM_QueryPhyPort(CPM_OM_COMM);

    /*当发送是通过USB并且发送长度大于60k的时候，需要限制发送长度*/
    if (((CPM_APP_PORT == enPhyport)||(CPM_CTRL_PORT == enPhyport))
        &&(ulDataLen > USB_MAX_DATA_LEN))
    {
        LogPrint1("GU_OamUSBWriteDataCB: Change the send Data Len %d", (VOS_INT)ulDataLen);

        *pulSendDataLen = USB_MAX_DATA_LEN;
    }
    else
    {
        *pulSendDataLen = ulDataLen;  /*其他情况下不需要调整当前的大小，包括sd、wifi*/
    }

    *penPhyport = enPhyport;
    return;
}


VOS_VOID GU_OamSocpSendDataToUDI(SOCP_CODER_DST_ENUM_U32 enChanID, VOS_UINT8 *pucData, VOS_UINT8 *pucPHYData, VOS_UINT32 ulDataLen)
{
    VOS_UINT32                  ulResult;
    VOS_UINT32                  ulRet = VOS_ERR;
    CPM_PHY_PORT_ENUM_UINT32    enPhyport;
    VOS_UINT32                  ulSendDataLen;
    VOS_BOOL                    bReadDoneFlag = VOS_FALSE;
    VOS_BOOL                    bUsbSendSucFlag = VOS_FALSE;

    /*参数检查*/
    SOCP_SEND_DATA_PARA_CHECK(enChanID, ulDataLen, pucData);

    GU_OamGetSendDataLen(ulDataLen, &ulSendDataLen, &enPhyport);

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE(pucData, ulSendDataLen, OM_ACPU_SEND_USB);
    /*lint +e40*/

    ulResult = CPM_ComSend(CPM_OM_COMM, pucData, ulSendDataLen);

    OMRL_AcpuHdlcData(pucData, ulSendDataLen);

    if(CPM_SEND_ERR == ulResult)  /*当前通道已经发送失败，调用SOCP通道无数据搬运*/
    {
        g_stAcpuDebugInfo.ulUSBSendErrNum++;
        g_stAcpuDebugInfo.ulUSBSendErrLen += ulSendDataLen;

        bReadDoneFlag = VOS_TRUE;
    }
    else if(CPM_SEND_FUNC_NULL == ulResult)   /*当前通道异常，扔掉所有数据*/
    {
        bReadDoneFlag = VOS_TRUE;

        g_stAcpuDebugInfo.ulOmDiscardNum++;
        g_stAcpuDebugInfo.ulOmDiscardLen += ulDataLen;
    }
    else if(CPM_SEND_OK == ulResult)
    {
        if ((CPM_APP_PORT != enPhyport)&&(CPM_CTRL_PORT != enPhyport)&&(CPM_HSIC_PORT != enPhyport)) /*对于当前是同步步发送的端口需要马上释放SOCP*/
        {
            bReadDoneFlag = VOS_TRUE;
        }
        else
        {
            ulRet = VOS_OK;
        }

        bUsbSendSucFlag = VOS_TRUE;
    }
    else
    {
        LogPrint1("GU_OamUSBWriteDataCB: CPM_ComSend return Error %d", (VOS_INT)ulResult);
    }

    if (VOS_TRUE == bReadDoneFlag)
    {
        ulRet = SCM_RlsDestBuf(SOCP_CODER_DST_GU_OM, ulSendDataLen);

        if(VOS_OK != ulRet)
        {
            g_stAcpuDebugInfo.ulSocpReadDoneErrNum++;
            g_stAcpuDebugInfo.ulSocpReadDoneErrLen += ulSendDataLen;
            LogPrint1("GU_OamSocpSendDataToUDI: SCM_RlsDestBuf return Error %d", (VOS_INT)ulRet);
        }
    }

    if ((VOS_OK == ulRet) && (VOS_TRUE == bUsbSendSucFlag))
    {
        g_stAcpuDebugInfo.ulUSBSendNum++;
        g_stAcpuDebugInfo.ulUSBSendLen += ulSendDataLen;
        if(g_stAcpuDebugInfo.ulUSBSendLen > OM_DATA_MAX_LENGTH)
        {
            g_stAcpuDebugInfo.ulUSBSendRealNum++;
            g_stAcpuDebugInfo.ulUSBSendLen -= OM_DATA_MAX_LENGTH;
        }
    }

    return;
}

/*****************************************************************************
 Prototype       : GU_OmAcpuSwitchOnOff
 Description     : ON or Off Acpu Switch
 Input           : VOS_UINT32
 Output          : None
 Return Value    : None

 History         : ---
    Date         :
    Author       :
    Modification : Created function
 *****************************************************************************/
VOS_VOID GU_OmAcpuSwitchOnOff(VOS_UINT32 ulFlag)
{
    g_ulAcpuOMSwitchOnOff = ulFlag;
}


VOS_VOID GU_OmPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle)
{
    vos_printf("\r\nThe Port Write num 1 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1);
    vos_printf("\r\nThe Port Write num 2 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2);
    vos_printf("\r\nThe Port Write Max Time is       0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime);

    vos_printf("\r\nThe Port Write CB Num is         %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum);

    vos_printf("\r\nThe Port Write Err Time is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum);
    vos_printf("\r\nThe Port Write Err Value is      0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue);
    vos_printf("\r\nThe Port Write Err Len is        0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen);

    vos_printf("\r\nThe Port In CB Num is            %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum);
    vos_printf("\r\nThe Port In CB Time is           0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime);
    vos_printf("\r\nThe Port Out CB Num is           %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum);
    vos_printf("\r\nThe Port Out CB Time is          0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime);
    vos_printf("\r\nThe Port State CB Err Num is     %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum);
    vos_printf("\r\nThe Port State CB Err Time is    0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime);

    vos_printf("\r\nThe Port Out CB Num is           %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCBErrNum);
    vos_printf("\r\nThe Port State CB Err Num is     0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCBErrTime);

    vos_printf("\r\nThe Port Open num is            %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum);
    vos_printf("\r\nThe Port Open slice is          0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice);

    vos_printf("\r\nThe Port Open OK num is         %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum);
    vos_printf("\r\nThe Port Open OK slice is       0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice);

    vos_printf("\r\nThe Port Open OK2 num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num);
    vos_printf("\r\nThe Port Open OK2 slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice);

    vos_printf("\r\nThe Port Close num is           %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum);
    vos_printf("\r\nThe Port Close slice is         0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice);

    vos_printf("\r\nThe Port Close OK num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum);
    vos_printf("\r\nThe Port Close OK slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice);

    return;
}


VOS_VOID GU_OmVComInfoShow(VOS_VOID)
{
    vos_printf("\r\nVCom Send num is           %d", g_stVComDebugInfo.ulVCOMSendNum);
    vos_printf("\r\nVCom Send Len is           %d", g_stVComDebugInfo.ulVCOMSendLen);

    vos_printf("\r\nVCom Send Error num is         %d", g_stVComDebugInfo.ulVCOMSendErrNum);
    vos_printf("\r\nVCom Send Error Len is         %d\r\n", g_stVComDebugInfo.ulVCOMSendErrLen);

    vos_printf("\r\nVCom receive num is           %d", g_stVComDebugInfo.ulVCOMRcvNum);
    vos_printf("\r\nVCom receive Len is           %d", g_stVComDebugInfo.ulVCOMRcvLen);

    vos_printf("\r\nVCom receive Error num is         %d", g_stVComDebugInfo.ulVCOMRcvErrNum);
    vos_printf("\r\nVCom receive Error Len is         %d\r\n", g_stVComDebugInfo.ulVCOMRcvErrLen);

#if (FEATURE_ON == FEATURE_CBT_LOG)
    vos_printf("\r\nVCom CBT LOG Hdlc num is           %d", g_stVComCBTDebugInfo.ulVCOMHdlcEnNum);
    vos_printf("\r\nVCom CBT LOG Hdlc Fail num is      %d", g_stVComCBTDebugInfo.ulVCOMHdlcEnFailNum);

    vos_printf("\r\nVCom CBT LOG Send num is           %d", g_stVComCBTDebugInfo.ulVCOMSendNum);
    vos_printf("\r\nVCom CBT LOG Send Len is           %d", g_stVComCBTDebugInfo.ulVCOMSendLen);

    vos_printf("\r\nVCom CBT LOG Send Error num is         %d", g_stVComCBTDebugInfo.ulVCOMSendErrNum);
    vos_printf("\r\nVCom CBT LOG Send Error Len is         %d\r\n", g_stVComCBTDebugInfo.ulVCOMSendErrLen);
#endif

    return;
}


VOS_VOID OM_AcpuReadNVLog(VOS_VOID)
{
    FILE                               *fp;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  acLogPath[] = ".\\yaffs0\\A_NV_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  acLogPath[] = "/modem_log/A_NV_OmLog.bin";

#else
    VOS_CHAR                  acLogPath[] = "/yaffs0/A_NV_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    if ((VOS_NULL_PTR == g_pstRecordAppToOm)||(VOS_NULL_PTR == g_pstRecordOmToApp))
    {
        vos_printf("g_pstRecordAppToOm is null\n");
        DRV_FILE_CLOSE(fp);

        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)g_pstRecordAppToOm, sizeof(VOS_CHAR), SAVE_MAX_SEND_INFO*sizeof(OM_NV_MNTN_RECORD_STRU), fp);
    DRV_FILE_WRITE((VOS_VOID*)g_pstRecordOmToApp, sizeof(VOS_CHAR), SAVE_MAX_SEND_INFO*sizeof(OM_NV_MNTN_RECORD_STRU), fp);

    DRV_FILE_CLOSE(fp);

}


VOS_VOID OM_AcpuLogShowToFile(VOS_BOOL bIsSendMsg)
{
    FILE                               *fp;
    OM_AUTOCONFIG_CNF_STRU             *pstSendCnf;
    VOS_UINT32                          ulTemp = 0x5a5a5a5a;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  g_acLogPath[] = ".\\yaffs0\\A_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  g_acLogPath[] = "/modem_log/A_OmLog.bin";

#else
    VOS_CHAR                  g_acLogPath[] = "/yaffs0/A_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(g_acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuPcToUeSucRecord, sizeof(VOS_CHAR), sizeof(g_stAcpuPcToUeSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuPcToUeErrRecord, sizeof(VOS_CHAR), sizeof(g_stAcpuPcToUeErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuUeToPcSucRecord, sizeof(VOS_CHAR), sizeof(g_stAcpuUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuUeToPcErrRecord, sizeof(VOS_CHAR), sizeof(g_stAcpuUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuDebugInfo , sizeof(VOS_CHAR), sizeof(g_stAcpuDebugInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stVComDebugInfo , sizeof(VOS_CHAR), sizeof(g_stVComDebugInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)g_astAcpuRecordInfo , sizeof(VOS_CHAR), sizeof(g_astAcpuRecordInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_ulAcpuOMSwitchOnOff , sizeof(VOS_CHAR), sizeof(g_ulAcpuOMSwitchOnOff), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SCM 相关*/
    SCM_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SOCP 相关*/
    SOCP_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_CLOSE(fp);

    OM_AcpuReadNVLog();

    /* 发送消息給 Acpu */
    if(VOS_FALSE == bIsSendMsg)
    {
        return;
    }

    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(ACPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return;
    }

    pstSendCnf->ulReceiverPid  = CCPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_RECORD_DBU_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(ACPU_PID_OMAGENT, pstSendCnf);

    return;
}

#else

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 用于ACPU上ICC通道的控制数据列表 */
OM_ICC_CHANNEL_CTRL_STRU                g_astOMACPUIccCtrlTable[OM_ICC_CHANNEL_BUTT];

/* 用于ACPU上SOCP接收 通道的统计信息 */
OM_ACPU_DEBUG_INFO                      g_stAcpuDebugInfo={0};

VOS_SPINLOCK                            g_stVosErrLogSendSpinLock;  /* 自旋锁，用来作Err Log上报状态机的临界资源保护 */

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/

extern UDI_HANDLE                       g_OSAIccUDIHandle;

extern OM_RECORD_BUF_STRU               g_astAcpuRecordInfo[VOS_EXC_DUMP_MEM_NUM_BUTT];

extern OM_APP_MSG_RECORD_STRU           g_stOmAppMsgRecord;

extern OM_VCOM_DEBUG_INFO               g_stVComDebugInfo[3];

#if(FEATURE_ON == FEATURE_PTM)
extern struct semaphore                        g_stOmRxErrorLogBuffSem;
#endif

extern VOS_UINT V_ICC_OSAMsg_CB(VOS_UINT ulChannelID,VOS_INT lLen);

VOS_VOID SCM_LogToFile(FILE *fp);
VOS_VOID SOCP_LogToFile(FILE *fp);

VOS_INT OM_AcpuReadVComData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pucData, VOS_UINT32 uslength);

VOS_VOID OM_SendAcpuSocpVote(SOCP_VOTE_TYPE_ENUM_U32 enVote);

/*****************************************************************************
  4 函数实现
*****************************************************************************/


VOS_UINT32 GU_OamSndPcMsgToCcpu(OMRL_RCV_CHAN_CTRL_INFO_STRU *pstCtrlInfo, VOS_UINT8 *pucData, VOS_UINT32 ulSize)
{
    VOS_INT32  lResult = VOS_ERROR;

    lResult = BSP_ICC_Write(UDI_ICC_GUOM0, pucData, (VOS_INT32)ulSize);

    /* 由于C核复位，写ICC通道失败会返回一个特殊值，不能复位单板 */
    if (BSP_ERR_ICC_CCORE_RESETTING == lResult )
    {
        LogPrint1("\n# GU_OamSndPcMsgToCcpu Error,Ccore Reset,ulSize %d .\n",(VOS_INT)ulSize);

        return VOS_ERR;
    }

    /* 当前写操作失败 */
    if(ulSize != (VOS_UINT32)lResult)
    {
        LogPrint2("GU_OamSndPcMsgToCcpu: The ICC UDI Write is Error.Size:%d,lResult:%d\n",(VOS_INT)ulSize,lResult);

        pstCtrlInfo->stPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrNum++;
        pstCtrlInfo->stPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrLen += ulSize;
        pstCtrlInfo->stPcToUeErrRecord.stICCSendFailInfo.ulICCOmSendErrSlice= VOS_GetSlice();

        return VOS_ERR;
    }
    pstCtrlInfo->stPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendMsgNum++;
    pstCtrlInfo->stPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendLen += ulSize;
    pstCtrlInfo->stPcToUeSucRecord.stICCSendSUCInfo.ulICCOmSendSlice= VOS_GetSlice();

    return VOS_OK;
}


VOS_VOID GU_OamErrLogVComPortInit(VOS_VOID)
{
#if(FEATURE_ON == FEATURE_PTM)
    VOS_SpinLockInit(&g_stVosErrLogSendSpinLock);

    /* 商用ERR LOG上报全局变量初始化 */
    g_stOmAppMsgRecord.ulErrLogReportSend = 0;/* 记录Err Log需要上报组件 */
    g_stOmAppMsgRecord.ulFTMReportSend    = 0;/* 记录FTM需要上报组件 */
    g_stOmAppMsgRecord.pulErrorLogModule  = VOS_NULL_PTR;
    g_stOmAppMsgRecord.pulFTMModule       = VOS_NULL_PTR;
    g_stOmAppMsgRecord.usModemId          = MODEM_ID_BUTT;

#if (VOS_LINUX == VOS_OS_VER)
    sema_init(&g_stOmRxErrorLogBuffSem, 1);
#endif

    /* 注册收Vcom Error log函数给NAS */
    APP_VCOM_RegDataCallback(APP_VCOM_DEV_INDEX_ERRLOG, OM_AcpuReadVComData);
#endif
    return;
}





VOS_UINT GU_OamAcpu_ICCError_CB(VOS_UINT ulChanID, VOS_UINT ulEvent, VOS_VOID* pParam)
{
    VOS_UINT32 ulChannelID;

    if(g_astOMACPUIccCtrlTable[OM_OM_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        ulChannelID = OM_OM_ICC_CHANNEL;
    }
    else if(g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        ulChannelID = OM_OSA_MSG_ICC_CHANNEL;
    }
    else
    {
        LogPrint("GU_OamAcpu_ICCError_CB: The Channel ID is not used by OAM\n");

        return VOS_ERR;
    }

    LogPrint2("GU_OAMAcpu_ICCError_CB: The Error Channel ID is %d,and Event is %d\n",
                (VOS_INT32)ulChannelID, (VOS_INT32)ulEvent);

    return VOS_OK;
}


VOS_UINT32 OM_ComRx_ICC_Init(VOS_VOID)
{
    VOS_INT32       i;

    /*初始化 ICC通道*/
    VOS_MemSet(g_astOMACPUIccCtrlTable, 0, sizeof(g_astOMACPUIccCtrlTable));

    for(i=0; i<OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr
                                    = (ICC_CHAN_ATTR_S*)VOS_MemAlloc(ACPU_PID_OM,
                                                                     STATIC_MEM_PT,
                                                                     sizeof(ICC_CHAN_ATTR_S));

        if(VOS_NULL_PTR == g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr)
        {
            LogPrint1("OM_ComRx_ICC_Init: VOS_MemAlloc Failed, Index is %d.\n", i);

            return VOS_ERR;             /*分配内存失败单板会重启，因此不需要释放之前已经申请的内存*/
        }
    }

    g_astOMACPUIccCtrlTable[OM_OM_ICC_CHANNEL].stUDICtrl.enICCId                    = UDI_ICC_GUOM0_ID;

    g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.enICCId               = UDI_ICC_GUOM4_ID;
    g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.pstICCAttr->read_cb   = V_ICC_OSAMsg_CB;

    for(i=0; i<OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32Priority    = OM_ICC_CHANNEL_PRIORITY;
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32TimeOut     = OM_ICC_HANDSHAKE_TIME_MAX;

        if( i == OM_OSA_MSG_ICC_CHANNEL)
        {
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OSA_ICC_BUFFER_SIZE;
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OSA_ICC_BUFFER_SIZE;
        }
        else
        {
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OM_ICC_BUFFER_SIZE;
            g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OM_ICC_BUFFER_SIZE;
        }

        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->enChanMode     = ICC_CHAN_MODE_PACKET;
        g_astOMACPUIccCtrlTable[i].stUDICtrl.pstICCAttr->event_cb       = GU_OamAcpu_ICCError_CB;

        g_astOMACPUIccCtrlTable[i].UDIHdle = DRV_UDI_OPEN((UDI_OPEN_PARAM *)&(g_astOMACPUIccCtrlTable[i].stUDICtrl));

        if(VOS_ERROR == g_astOMACPUIccCtrlTable[i].UDIHdle)
        {
            /* 打开失败时记录当前ICC通道信息 */
            DRV_SYSTEM_ERROR(OM_APP_ICC_INIT_ERROR, THIS_FILE_ID, __LINE__,
                            (VOS_CHAR*)&i, sizeof(VOS_INT32));

            return VOS_ERR;
        }
    }

    g_OSAIccUDIHandle = g_astOMACPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle;

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : GU_OmAcpuSwitchOnOff
 Description     : ON or Off Acpu Switch
 Input           : VOS_UINT32
 Output          : None
 Return Value    : None

 History         : ---
    Date         :
    Author       :
    Modification : Created function
 *****************************************************************************/
VOS_VOID GU_OmAcpuSwitchOnOff(VOS_UINT32 ulFlag)
{
    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = ulFlag;
}



VOS_VOID OM_AcpuLogShowToFile(VOS_BOOL bIsSendMsg)
{
    FILE                               *fp;
    OM_AUTOCONFIG_CNF_STRU             *pstSendCnf;
    VOS_UINT32                          ulTemp = 0x5a5a5a5a;

#if(VOS_WIN32 == VOS_OS_VER)
    VOS_CHAR                  g_acLogPath[] = ".\\yaffs0\\A_OmLog.bin";

#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
    VOS_CHAR                  g_acLogPath[] = "/modem_log/A_OmLog.bin";

#else
    VOS_CHAR                  g_acLogPath[] = "/yaffs0/A_OmLog.bin";
#endif
#endif

    fp = DRV_FILE_OPEN(g_acLogPath, "w+");

    if (VOS_NULL_PTR == fp)
    {
        return;
    }

    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuCnfCtrlInfo.stPcToUeSucRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuCnfCtrlInfo.stPcToUeSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuCbtCtrlInfo.stPcToUeSucRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuCbtCtrlInfo.stPcToUeSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuCnfCtrlInfo.stPcToUeErrRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuCnfCtrlInfo.stPcToUeErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuCbtCtrlInfo.stPcToUeErrRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuCbtCtrlInfo.stPcToUeErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuTxCnfCtrlInfo.stUeToPcSucRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuTxCnfCtrlInfo.stUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuTxIndCtrlInfo.stUeToPcSucRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuTxIndCtrlInfo.stUeToPcSucRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuTxCnfCtrlInfo.stUeToPcErrRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuTxCnfCtrlInfo.stUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&(g_stAcpuTxIndCtrlInfo.stUeToPcErrRecord), sizeof(VOS_CHAR), sizeof(g_stAcpuTxIndCtrlInfo.stUeToPcErrRecord), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuDebugInfo , sizeof(VOS_CHAR), sizeof(g_stAcpuDebugInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)&g_stVComDebugInfo[0], sizeof(VOS_CHAR), sizeof(g_stVComDebugInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_WRITE((VOS_VOID*)g_astAcpuRecordInfo , sizeof(VOS_CHAR), sizeof(g_astAcpuRecordInfo), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    DRV_FILE_WRITE((VOS_VOID*)&g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff , sizeof(VOS_CHAR), sizeof(g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff), fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SCM 相关*/
    SCM_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);

    /* SOCP 相关*/
    SOCP_LogToFile(fp);
    DRV_FILE_WRITE((VOS_VOID*)&ulTemp, sizeof(VOS_CHAR), sizeof(VOS_UINT32), fp);
    DRV_FILE_CLOSE(fp);

    /* 发送消息給 Acpu */
    if(VOS_FALSE == bIsSendMsg)
    {
        return;
    }

    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(ACPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return;
    }

    pstSendCnf->ulReceiverPid  = CCPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_RECORD_DBU_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(ACPU_PID_OMAGENT, pstSendCnf);

    return;
}


VOS_VOID OmAcpuPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle)
{
    vos_printf("\r\nThe Port Write num 1 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1);
    vos_printf("\r\nThe Port Write num 2 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2);
    vos_printf("\r\nThe Port Write Max Time is       0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime);

    vos_printf("\r\nThe Port Write CB Num is         %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum);

    vos_printf("\r\nThe Port Write Err Time is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum);
    vos_printf("\r\nThe Port Write Err Value is      0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue);
    vos_printf("\r\nThe Port Write Err Len is        0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen);

    vos_printf("\r\nThe Port In CB Num is            %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum);
    vos_printf("\r\nThe Port In CB Time is           0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime);
    vos_printf("\r\nThe Port Out CB Num is           %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum);
    vos_printf("\r\nThe Port Out CB Time is          0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime);
    vos_printf("\r\nThe Port State CB Err Num is     %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum);
    vos_printf("\r\nThe Port State CB Err Time is    0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime);

    vos_printf("\r\nThe Port Open num is            %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum);
    vos_printf("\r\nThe Port Open slice is          0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice);

    vos_printf("\r\nThe Port Open OK num is         %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum);
    vos_printf("\r\nThe Port Open OK slice is       0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice);

    vos_printf("\r\nThe Port Open OK2 num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num);
    vos_printf("\r\nThe Port Open OK2 slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice);

    vos_printf("\r\nThe Port Close num is           %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum);
    vos_printf("\r\nThe Port Close slice is         0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice);

    vos_printf("\r\nThe Port Close OK num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum);
    vos_printf("\r\nThe Port Close OK slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice);
}

#endif //(FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif



