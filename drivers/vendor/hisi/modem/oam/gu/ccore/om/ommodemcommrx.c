/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ommodemcommrx.c
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2011年3月11日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2011年3月11日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "errorlog.h"
#include "omrl.h"
#include "omprivate.h"
#include "DrvInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_OM_COMMRX_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*用于CCPU上ICC通道的数据处理回调函数列表*/
OM_PORT_READ_DATA_CB                    g_astOMComRxCBTable[OM_ICC_CHANNEL_BUTT] = {0};

/*用于CCPU上ICC通道的控制数据列表*/
OM_ICC_CHANNEL_CTRL_STRU                g_astOMCCPUIccCtrlTable[OM_ICC_CHANNEL_BUTT];

/* debug */
VOS_UINT32                              g_ulOmCcpuDbg = 0;

/*****************************************************************************
  3 外部函数声明
*****************************************************************************/
extern UDI_HANDLE                       g_OSAIccUDIHandle;

extern VOS_UINT V_ICC_OSAMsg_CB(VOS_UINT ulChannelID,VOS_INT lLen);

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 OM_ComRx_ICCDataRec(OM_ICC_CHANNEL_ENUM_UINT32 enChannelId, VOS_INT32 lLen)
{
    VOS_UINT8 *pucData;
    VOS_INT32 lResult;

    /*申请内存用于保存ICC传递的数据*/
    pucData = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, (VOS_UINT32)lLen);

    if (VOS_NULL_PTR == pucData)
    {
        LogPrint1("OM_ComRx_ICCDataRec: VOS_MemAlloc is Failed, Data Len is %d.\n", lLen);

        return VOS_ERR;
    }

    /*读取ICC传递的数据*/
    lResult = DRV_UDI_READ(g_astOMCCPUIccCtrlTable[enChannelId].UDIHdle, pucData, (VOS_UINT32)lLen);
    if (lLen != lResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pucData);
        LogPrint("OM_ComRx_ICCDataRec: DRV_UDI_READ is Failed.\n");

        return VOS_ERR;
    }

    /*ICC数据处理函数返回错误*/
    if (VOS_OK != g_astOMComRxCBTable[enChannelId](pucData, (VOS_UINT16)lLen))
    {
        LogPrint("OM_ComRx_ICCDataRec: The CB function is Failed.\n");
    }

    VOS_MemFree(WUEPS_PID_OM, pucData);

    return VOS_OK;
}

VOS_UINT OM_ComRx_ICC_OM_CB(VOS_UINT ulChannelID,VOS_INT lLen)
{
    VOS_UINT32 ulResult;
    VOS_UINT8 *pucData;

    if (lLen <= 0)
    {
        LogPrint("OM_ComRx_ICC_OM_CB: The Data Len is 0.\n");

        g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmParaErrNum++;

        return VOS_ERR;
    }

    if (VOS_NULL_PTR == g_astOMComRxCBTable[OM_OM_ICC_CHANNEL])  /*当前回调函数为空*/
    {
        LogPrint("OM_ComRx_ICC_OM_CB The CB Function is Empty.\n");

        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecLen += lLen;
        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecMsgNum++;
        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecSlice = OM_GetSlice();

        pucData = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, (VOS_UINT32)lLen);

        if(VOS_NULL_PTR != pucData)
        {
            DRV_UDI_READ(g_astOMCCPUIccCtrlTable[OM_OM_ICC_CHANNEL].UDIHdle, pucData, (VOS_UINT32)lLen);
            VOS_MemFree(WUEPS_PID_OM, pucData);
        }

        return VOS_OK;
    }

    ulResult = OM_ComRx_ICCDataRec(OM_OM_ICC_CHANNEL, lLen);

    if (VOS_ERR == ulResult)
    {
        LogPrint("OM_ComRx_ICC_OM_CB The Send Data is Failed.\n");

        g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrLen += lLen;
        g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrMsgNum++;
        g_stPcToUeErrRecord.stICCRecFailInfo.ulICCOmRecErrSlice = OM_GetSlice();
    }
    else
    {
        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecLen += lLen;
        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecMsgNum++;
        g_stPcToUeSucRecord.stICCRecSUCInfo.ulICCOmRecSlice = OM_GetSlice();
    }

    return ulResult;
}

VOS_UINT OM_ComRx_ICCError_CB(VOS_UINT ulChanID, VOS_UINT ulEvent, VOS_VOID* pParam)
{
    OAM_MNTN_ICC_ERROR_EVENT_STRU stIccErrorInfo;

    stIccErrorInfo.ulEvent = ulEvent;

    if (g_astOMCCPUIccCtrlTable[OM_OM_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        stIccErrorInfo.ulChannelID = OM_OM_ICC_CHANNEL;
    }
    else if(g_astOMCCPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle == ulChanID)
    {
        stIccErrorInfo.ulChannelID = OM_OSA_MSG_ICC_CHANNEL;
    }
    else
    {
        LogPrint("OM_ComRx_ICCError_CB: The Channel ID is not used by OAM\n");

        return VOS_ERR;
    }

    LogPrint2("OM_ComRx_ICCError_CB: The Error ICC Channel is %d, and Error Event is %d",
                    (VOS_INT32)stIccErrorInfo.ulChannelID, (VOS_INT32)ulEvent);

    /*通道ID和错误原因写入ErrorLog*/
    MNTN_RecordErrorLog(MNTN_OAM_ICC_ERROR_EVENT, &stIccErrorInfo, sizeof(stIccErrorInfo));

    return VOS_OK;
}
VOS_VOID OM_ComRx_RegisterCB(OM_ICC_CHANNEL_ENUM_UINT32    enPortNum,
                                        OM_PORT_READ_DATA_CB          pfDataFun)
{
    if (enPortNum >= OM_ICC_CHANNEL_BUTT)    /* 输入端口号不在合理范围 */
    {
        LogPrint("OM_ComRx_RegisterCB The enPortNum is Error.\n");

        return ;
    }

    g_astOMComRxCBTable[enPortNum]   = pfDataFun;

    return ;
}
VOS_VOID OM_ComRx_DeRegisterCB(OM_ICC_CHANNEL_ENUM_UINT32    enPortNum)
{
    if (enPortNum >= OM_ICC_CHANNEL_BUTT)    /*输入端口号不在合理范围*/
    {
        LogPrint("OM_ComRx_DeRegisterCB The enPortNum is Error.\n");

        return ;
    }

    g_astOMComRxCBTable[enPortNum] = VOS_NULL_PTR;

    return ;
}
VOS_UINT32 OM_ComRx_ICC_Init(VOS_VOID)
{
    VOS_INT32  i;

    VOS_MemSet(g_astOMComRxCBTable, 0 ,sizeof(g_astOMComRxCBTable));
    VOS_MemSet(g_astOMCCPUIccCtrlTable, 0, sizeof(g_astOMCCPUIccCtrlTable));

    for (i = 0; i < OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr = (ICC_CHAN_ATTR_S*)VOS_MemAlloc(WUEPS_PID_OM,
                                                                                         STATIC_MEM_PT,
                                                                                         sizeof(ICC_CHAN_ATTR_S));
        if(VOS_NULL_PTR == g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr)
        {
            LogPrint1("OM_ComRx_ICC_Init: VOS_MemAlloc Failed, Index is %d.\n", i);

            return VOS_ERR;
        }
    }

    g_astOMCCPUIccCtrlTable[OM_OM_ICC_CHANNEL].stUDICtrl.enICCId                    = UDI_ICC_GUOM0_ID;
    g_astOMCCPUIccCtrlTable[OM_OM_ICC_CHANNEL].stUDICtrl.pstICCAttr->read_cb        = OM_ComRx_ICC_OM_CB;

    g_astOMCCPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.enICCId               = UDI_ICC_GUOM4_ID;
    g_astOMCCPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].stUDICtrl.pstICCAttr->read_cb   = V_ICC_OSAMsg_CB;

    for(i=0; i<OM_ICC_CHANNEL_BUTT; i++)
    {
        g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32Priority    = OM_ICC_CHANNEL_PRIORITY;  /* 统一使用最高优先级 */
        g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32TimeOut     = OM_ICC_HANDSHAKE_TIME_MAX;

        if( i == OM_OSA_MSG_ICC_CHANNEL)
        {
            g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OSA_ICC_BUFFER_SIZE;
            g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OSA_ICC_BUFFER_SIZE;
        }
        else
        {
            g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOInSize  = OM_ICC_BUFFER_SIZE;
            g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->u32FIFOOutSize = OM_ICC_BUFFER_SIZE;
        }

        g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->enChanMode     = ICC_CHAN_MODE_PACKET;
        g_astOMCCPUIccCtrlTable[i].stUDICtrl.pstICCAttr->event_cb       = OM_ComRx_ICCError_CB;

        g_astOMCCPUIccCtrlTable[i].UDIHdle = DRV_UDI_OPEN((UDI_OPEN_PARAM *)&(g_astOMCCPUIccCtrlTable[i].stUDICtrl));

        if (VOS_ERROR == g_astOMCCPUIccCtrlTable[i].UDIHdle)
        {
            /* 打开失败时记录当前ICC通道信息 */
            DRV_SYSTEM_ERROR(OM_APP_ICC_INIT_ERROR, THIS_FILE_ID, __LINE__,
                            (VOS_CHAR*)&i, sizeof(VOS_INT32));

            return VOS_ERR;
        }
    }

    g_OSAIccUDIHandle = g_astOMCCPUIccCtrlTable[OM_OSA_MSG_ICC_CHANNEL].UDIHdle;

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif



