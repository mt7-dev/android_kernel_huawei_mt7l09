/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmOperator.c
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2009年10月27日
  最近修改      :
  功能描述      : 该C文件给出了OmOperator模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2009年10月27日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "omoperator.h"
#include "NVIM_Interface.h"
#include "UsimPsInterface.h"
#include "apminterface.h"
#include "spysystem.h"
#include "TafOamInterface.h"
#include "hpamailboxdef.h"
#include "omprintf.h"
#include "omappoperator.h"
#include "DspInterface.h"
#include "omnvinterface.h"
#include "NasNvInterface.h"
#include "Omappagent.h"


#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
#include "OmLittleImageLog.h"
#endif

#if (RAT_MODE != RAT_GU)
#include "product_config.h"
#endif

#if(FEATURE_ON == FEATURE_SOCP_ON_DEMAND)
#include "msp_diag.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人:甘兰 47350;原因:Log打印 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_OPERATOR_C
/*lint +e767 修改人:甘兰 47350;*/


/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/*记录注册给NAS的Client Id*/
VOS_UINT16                      g_usClientId;
/*需要进行鉴权的NV项*/
VOS_UINT16                      g_ausOMNvAuthIdList[] =
{
    en_NV_Item_IMEI,
    en_NV_Item_USB_Enum_Status,
    en_NV_Item_CustomizeSimLockPlmnInfo,
    en_NV_Item_CardlockStatus,
    en_NV_Item_CustomizeSimLockMaxTimes,
    en_NV_Item_CustomizeService,
    en_NV_Item_PRODUCT_ID,
    en_NV_Item_PREVENT_TEST_IMSI_REG,
    en_NV_Item_AT_SHELL_OPEN_FLAG,
};

/*记录当前用户的用户权限*/
static VOS_UINT32               g_ulOMPrivilegeLevel = LEVEL_NORMAL;

extern OM_TRACE_EVENT_CONFIG_PS_STRU g_stTraceEventConfig;
/* Debug */
VOS_UINT32                      g_ulOmVersionDbg = 0;

/* 模式查询上报标识 */
VOS_BOOL                        g_bDSPModeFlag   = VOS_FALSE;

OM_NV_OPERATE_RECORD_STRU       g_astRecordAcpuToCcpu[SAVE_MAX_SEND_INFO];

VOS_UINT32                      g_ulNvAcpuToCcpuCount = 0;

VOS_UINT32                      g_ulFTMFlag = VOS_FALSE;

extern VOS_UINT32               g_ulOmNosigEnable;

#if(FEATURE_ON == FEATURE_SOCP_ON_DEMAND)
OM_SOCPVOTE_INFO_STRU           g_astOmCcpuSocpVoteInfo[OM_SOCPVOTE_INFO_BUTT]= {0};
#endif

/*****************************************************************************
  3 函数申明
*****************************************************************************/
extern VOS_VOID Om_EndBbpDbg(VOS_VOID);

extern VOS_VOID OM_NoSigCtrlInit(VOS_VOID);


/*lint -e416 -e661 -e662 -e717 修改人:甘兰*/

VOS_VOID OM_MmaMsgProc(MsgBlock *pstMsg)
{
    VOS_UINT32                          ulPrimId;
    MN_APP_PHONE_EVENT_INFO_STRU        *pstEvent;
    MN_APP_PHONE_SET_CNF_STRU           *pstLoadDefault;
    VOS_UINT8                           ucModemID;
    VOS_UINT8                           ucFuncType;

    ucModemID = (VOS_UINT8)VOS_GetModemIDFromPid(pstMsg->ulSenderPid);

    ulPrimId = *(VOS_UINT32*)(pstMsg->aucValue);

    ucFuncType = OM_ADD_FUNCID_MODEMINFO(OM_QUERY_FUNC,ucModemID);

    if (TAF_OAM_PHONE_EVENT_IND == ulPrimId)
    {
        pstEvent = (MN_APP_PHONE_EVENT_INFO_STRU*)pstMsg;

        vos_printf("\r\n OM_MmaMsgProc: pstEvent->stPhoneEvent.PhoneError %d\n",
                    pstEvent->stPhoneEvent.PhoneError);

        OM_SendResult(ucFuncType, pstEvent->stPhoneEvent.PhoneError, OM_APP_SET_FTM_CNF);
    }
    else if (TAF_OAM_PHONE_SET_CNF == ulPrimId)
    {
        pstLoadDefault = (MN_APP_PHONE_SET_CNF_STRU *)pstMsg;

        OM_SendResult(ucFuncType, pstLoadDefault->ucResult, OM_APP_LOAD_DEFAULT_CNF);
    }
    else
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                           "OM_MmaMsgProc: ulPrimId ",(VOS_INT32)ulPrimId);
    }

    return;
}


VOS_UINT32 OM_BackupNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32 ulResult;

    ulResult = NV_Backup();
    if (NV_OK != ulResult)
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
         "OM_BackupNv:NV_Backup, errno:", (VOS_INT32)ulResult);
    }
    OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
    return VOS_OK;
}

#ifdef DMT

VOS_UINT32 OM_RestoreNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32 ulType;
    VOS_UINT32 ulResult = VOS_ERR;
    VOS_BOOL   bIsCreatefile;

    ulType = *((VOS_UINT32*)pstAppToOmMsg->aucPara);
    /*恢复全部NV项*/
    if (OM_RESTORE_ALL == ulType)
    {
        ulResult = NV_RestoreProc(VOS_FALSE, VOS_FALSE, &bIsCreatefile);
    }
    /*只恢复生成NV项*/
    else if (OM_RESTORE_MANUFACTURE == ulType)
    {
        ulResult = NV_RestoreProc(VOS_FALSE, VOS_TRUE, &bIsCreatefile);
    }
    else
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                "OM_RestoreNv:Type is not correct:", (VOS_INT32)ulType);
    }

    if (NV_OK != ulResult)
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                "OM_RestoreNv: Restore Failed, errno:", (VOS_INT32)ulResult);
    }
    OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
    return VOS_OK;
}
#endif


VOS_VOID OM_RecordNvInfo(VOS_UINT32 ulMode, VOS_UINT32 ulCount, VOS_UINT16 *pausNvId)
{
    VOS_UINT32                          ulIndex;
    OM_SW_VER_FLAG_STRU                 stCustomVersion;

    if ( VOS_OK != NV_Read(en_NV_Item_SW_VERSION_FLAG, &stCustomVersion, sizeof(stCustomVersion)) )
    {
        stCustomVersion.ulNvSwVerFlag = VOS_TRUE;
    }

    /* 如果ulCustomVersion == VOS_TRUE表示不是烧片版本，则直接返回 */
    if ( (VOS_TRUE == stCustomVersion.ulNvSwVerFlag))
    {
        return;
    }

    /* 烧片版本且FTM模式才记录 */
    if(VOS_TRUE != g_ulFTMFlag)
    {
        return;
    }

    g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].ulFlag = ulMode;
    g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].ulNVNum = ulCount;
    g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].ulSlice = VOS_GetSlice();

    for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        if(SAVE_MAX_NV_ID == ulIndex)
        {
            break;
        }
        g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].usNVID[ulIndex] = pausNvId[ulIndex];
    }

    return;
}


VOS_VOID OM_RecordNvErrInfo(VOS_UINT16 usErrCode)
{
    OM_SW_VER_FLAG_STRU                 stCustomVersion;

    if ( VOS_OK != NV_Read(en_NV_Item_SW_VERSION_FLAG, &stCustomVersion, sizeof(stCustomVersion)) )
    {
        stCustomVersion.ulNvSwVerFlag = VOS_TRUE;
    }

    /* 如果ulCustomVersion == VOS_TRUE表示不是烧片版本，则直接返回 */
    if ( (VOS_TRUE == stCustomVersion.ulNvSwVerFlag))
    {
        return;
    }

    /* 烧片版本且FTM模式才记录 */
    if(VOS_TRUE != g_ulFTMFlag)
    {
        return;
    }

    g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].ulFlag = (g_astRecordAcpuToCcpu[g_ulNvAcpuToCcpuCount].ulFlag | usErrCode);

    g_ulNvAcpuToCcpuCount++;

    if(SAVE_MAX_SEND_INFO == g_ulNvAcpuToCcpuCount)
    {
         g_ulNvAcpuToCcpuCount = 0;
    }

    return;
}


VOS_UINT32 OM_ReadNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU      *pstOmToAppMsg;
    APP_OM_READ_NV_STRU     *pstAppOmReadNv;
    OM_APP_READ_NV_STRU     *pstOmAppReadNv;
    VOS_UINT16              *pusOmToAppPara;
    VOS_UINT32               ulCount;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulTotalSize = 0;
    VOS_UINT32               ulResult;
    VOS_UINT16               usNvId;
    VOS_UINT32               ulNvLen;

    pstAppOmReadNv = (APP_OM_READ_NV_STRU*)(pstAppToOmMsg->aucPara);
    ulCount = pstAppOmReadNv->ulCount;

    OM_RecordNvInfo(0, ulCount, pstAppOmReadNv->ausNvItemId);

    /*Get the total length of all NV items.*/
    for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        usNvId = pstAppOmReadNv->ausNvItemId[ulIndex];
        ulResult = NV_GetLength(usNvId, &ulNvLen);
        if (VOS_OK != ulResult)
        {
            NV_GET_RETURN(ulResult, usNvId);
            OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                        "OM_ReadNv:NV_GetLength, NV id: ",(VOS_INT32)usNvId);

            OM_RecordNvErrInfo(0x30);

            return ulResult;
        }
        ulTotalSize += ulNvLen;
    }

    /*Allocate the memory space.*/
    ulTotalSize += OM_APP_MSG_EX_LEN + OM_READ_NV_HEAD_SIZE + (ulCount*OM_NV_ITEM_SIZE);
    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_AssistantMemAlloc(WUEPS_PID_OM,
                                                   DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, NV_ALLOC_BUFFER_FAIL, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ReadNv:VOS_MemAlloc.\n");
        OM_RecordNvErrInfo(0x40);
        return VOS_ERR;

    }
    /*Assign the return value and the count to struct's fields.*/
    pstOmAppReadNv = (OM_APP_READ_NV_STRU*)(pstOmToAppMsg->aucPara);
    pstOmAppReadNv->ulErrorCode = VOS_OK;
    pstOmAppReadNv->ulCount = ulCount;

    pusOmToAppPara = (VOS_UINT16*)(pstOmAppReadNv->ausNVItemData);
    /*Read the NV content by the NV Id.*/
    for(ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        usNvId = pstAppOmReadNv->ausNvItemId[ulIndex];
        NV_GetLength(usNvId, &ulNvLen);

        *pusOmToAppPara = usNvId;
        pusOmToAppPara++;
        *pusOmToAppPara = (VOS_UINT16)ulNvLen;
        pusOmToAppPara++;

        ulResult = NV_ReadEx(OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType),
                            usNvId, pusOmToAppPara, ulNvLen);
        if (NV_OK != ulResult)
        {
            NV_GET_RETURN(ulResult, usNvId);
            OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
            VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                    "OM_ReadNv:NV_Read, NV id:", (VOS_INT32)usNvId);
            OM_RecordNvErrInfo(0x50);
            return ulResult;
        }
        pusOmToAppPara += ulNvLen/sizeof(VOS_UINT16);
    }

    OM_RecordNvErrInfo(VOS_OK);

    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return VOS_OK;
}

VOS_UINT32 OM_IsAuthNv(VOS_UINT16 usNvId)
{
    VOS_UINT32 ulIndex;

    for (ulIndex = 0; ulIndex
            < (sizeof(g_ausOMNvAuthIdList)/sizeof(VOS_UINT16)); ulIndex++)
    {
        if (usNvId == g_ausOMNvAuthIdList[ulIndex])
        {
            if (LEVEL_ADVANCED == g_ulOMPrivilegeLevel)
            {
                return VOS_YES;
            }
            return VOS_NO;
        }
    }

    return VOS_YES;
}

VOS_UINT32 OM_WriteNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    APP_OM_WRITE_NV_STRU   *pstAppOmWriteNv;
    VOS_UINT16             *pusAppToOmPara;
    VOS_UINT32              ulCount;
    VOS_UINT32              ulIndex;
    VOS_UINT16              usNvId;
    VOS_UINT16              usNvLen;
    VOS_UINT32              ulResult;

    pstAppOmWriteNv = (APP_OM_WRITE_NV_STRU*)(pstAppToOmMsg->aucPara);
    /*Get the number of all NV Id.*/
    ulCount = pstAppOmWriteNv->ulCount;

    OM_RecordNvInfo(1, ulCount, pstAppOmWriteNv->ausNvItemData);

    pusAppToOmPara = (VOS_UINT16*)(pstAppOmWriteNv->ausNvItemData);
    /*Write the NV content by NV Id.*/
    for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        usNvId  = *pusAppToOmPara;
        pusAppToOmPara++;
        usNvLen = *pusAppToOmPara;
        pusAppToOmPara++;

        /*判断此NV项是否需要进行鉴权*/
        if (VOS_YES != OM_IsAuthNv(usNvId))
        {
            OM_SendResult(pstAppToOmMsg->ucFuncType, OM_NEED_AUTH, usReturnPrimId);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_WARNING,
                    "OM_WriteNv:OM_IsAuthNv, NV id:", (VOS_INT32)usNvId);
            OM_RecordNvErrInfo(0x70);

            return VOS_ERR;
        }

        ulResult = NV_WriteEx(OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType),
                                usNvId, pusAppToOmPara, (VOS_UINT32)usNvLen);
        if(NV_OK != ulResult)
        {
            NV_GET_RETURN(ulResult, usNvId);
            OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                        "OM_WriteNv:NV_Write, NV id:", (VOS_INT32)usNvId);
            OM_RecordNvErrInfo(0x80);

            return VOS_ERR;
        }
        /*由于返回的usNvLen以byte为单位，所以需要除以指针指向类型的大小*/
        pusAppToOmPara += (usNvLen/sizeof(VOS_UINT16));
    }

    OM_RecordNvErrInfo(VOS_OK);

    OM_SendResult(pstAppToOmMsg->ucFuncType, NV_OK, usReturnPrimId);

    return VOS_OK;
}


VOS_UINT32 OM_GetNvIdList(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                          VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32              ulNvNum;
    VOS_UINT32              ulTotalLen;
    OM_APP_MSG_EX_STRU      *pstOmToAppMsg;
    VOS_UINT32              ulResult;
    OM_APP_GET_NV_LIST_STRU *pstOmGetNvList;

    ulNvNum = NV_GetNVIdListNum();
    /*No NV exist*/
    if (0 == ulNvNum)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetNvIdList:No NV exist.\n");
        return VOS_ERR;
    }

    /*APP_HEAD + Result + NV nums + NV ID/LEN lists*/
    ulTotalLen = sizeof(OM_APP_MSG_EX_STRU) + sizeof(VOS_UINT32)
                        + (ulNvNum*sizeof(NV_LIST_INFO_STRU));

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(
                                   WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalLen);

    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetNvIdList:VOS_MemAlloc Fail.");
        return VOS_ERR;
    }

    pstOmGetNvList = (OM_APP_GET_NV_LIST_STRU*)(pstOmToAppMsg->aucPara);

    /*获取每个NV项的ID和长度*/
    ulResult = NV_GetNVIdList(pstOmGetNvList->astNvInfo);
    if (NV_OK != ulResult)
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetNvIdList:VOS_MemAlloc Fail.");
        return VOS_ERR;
    }

    /*填写执行结果和NV项数目*/
    pstOmGetNvList->ulErrorCode     = VOS_OK;
    pstOmGetNvList->usCount         = (VOS_UINT16)ulNvNum;
    pstOmGetNvList->ucMsgIndex      = 1;    /*分包索引，先默认为1*/
    pstOmGetNvList->ucTotalMsgCnt   = 1;    /*分包总数，先默认为1*/

    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalLen - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}


VOS_UINT32 OM_GetAperiodicStatus(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                                   VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU           *pstOmToAppMsg;
    APP_OM_APERIODIC_STATUS_STRU *pstAppOmStatus;
    OM_APP_APERIODIC_STATUS_STRU *pstOmAppStatus = VOS_NULL_PTR;
    VOS_UINT32                    ulTotalNum;
    VOS_UINT32                    ulTotalSize;
    VOS_UINT32                    ulHWId;
    VOS_UINT32                    ulHWStatus;
    VOS_UINT8                     ucCardStatus;
    VOS_UINT8                     ucCardType;
    VOS_UINT32                    ulIndex;
    VOS_UINT32                   *pulHWId;
    OM_HW_STATUS_STRU            *pstHwStatus;

    pstAppOmStatus = (APP_OM_APERIODIC_STATUS_STRU*)(pstAppToOmMsg->aucPara);
    ulTotalNum = pstAppOmStatus->ulNum;

    ulTotalSize = OM_APP_MSG_EX_LEN + sizeof(pstOmAppStatus->ulNum) +
                                        (ulTotalNum*sizeof(OM_HW_STATUS_STRU));
    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(
                                   WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetAperiodicStatus:VOS_MemAlloc.\n");
        return VOS_ERR;
    }

    pstOmAppStatus = (OM_APP_APERIODIC_STATUS_STRU*)(pstOmToAppMsg->aucPara);
    pstOmAppStatus->ulNum = ulTotalNum;

    pstHwStatus = pstOmAppStatus->astHWStatus;
    pulHWId = pstAppOmStatus->aulHWId;

    /*Get the status of hardware according the hardware Id.*/
    for(ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        ulHWId = pulHWId[ulIndex];
        pstHwStatus[ulIndex].ulHWId = ulHWId;

        switch(ulHWId)
        {
            case ID_OM_PMU_STATUS:
                ulHWStatus = (VOS_UINT32)DRV_PMU_SELFCHECK();
                break;

            case ID_OM_ABB_STATUS:
                ulHWStatus = (VOS_UINT32)DRV_ABB_SELFCHECK();
                break;

            case ID_OM_SPI_STATUS:
                ulHWStatus = (VOS_UINT32)DRV_SPI_SELFCHECK();
                break;

            case ID_OM_I2C_STATUS:
                ulHWStatus = (VOS_UINT32)DRV_I2C_SELFCHECK();
                break;

            case ID_OM_SIC_STATUS:
                if(VOS_OK != USIMM_GetCardType(&ucCardStatus, &ucCardType))
                {
                    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
                    PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetAperiodicStatus:USIMM_GetCardType.\n");
                    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
                    return VOS_ERR;
                }
                ulHWStatus = ((((VOS_INT32)ucCardType) << 16)
                                            |((VOS_INT32)ucCardStatus));    /*bit 16~31:usCardType;bit 0~15:usCardStatus*/
                break;

             default:
                OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
                PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetAperiodicStatus:Unknown hardware Id.\n");
                VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
                return VOS_ERR;
        }
        
        pstHwStatus[ulIndex].ulHWStatus = ulHWStatus;
    }
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}

VOS_UINT32 OM_QueryMemory(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                          VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU      *pstOmToAppMsg;
    OM_QUERY_MEMORY_STRU    *pstMemReq;
    OM_QUERY_MEMORY_STRU    *pstMemInd = VOS_NULL_PTR;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulCount;
    VOS_UINT32               ulTotalSize;
    VOS_UINT32               ulMemData = 0;
    VOS_UINT32               ulResult = 0;

    pstMemReq = (OM_QUERY_MEMORY_STRU*)(pstAppToOmMsg->aucPara);
    ulCount = pstMemReq->ulCnt;

    /*sizeof(VOS_UINT32)代表OM_QUERY_MEMORY_STRU中每个aulAddr单元的长度*/
    ulTotalSize = OM_APP_MSG_EX_LEN + sizeof(pstMemInd->ulCnt) + (ulCount*sizeof(VOS_UINT32));
    /*Allocate the memory space.*/
    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                       ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        /*0 indicates the count of the memory contents is zero, failed.*/
        OM_SendResult(pstAppToOmMsg->ucFuncType, 0, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_QueryMemory:VOS_MemAlloc.\n");
        return VOS_ERR;
    }
    pstMemInd = (OM_QUERY_MEMORY_STRU*)(pstOmToAppMsg->aucPara);
    /*Get the memory value by calling driver interface.*/
    for (ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        ulResult = (VOS_UINT32)DRV_MEM_READ(pstMemReq->aulAddr[ulIndex], (VOS_UINT*)(&ulMemData));
        /*It indicates the called function is failed.*/
        if (VOS_OK != ulResult)
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_QueryMemory:memReadApi\n");
            ulCount = 0;
            break;
        }
        pstMemInd->aulAddr[ulIndex] = ulMemData;
    }
    pstMemInd->ulCnt = ulCount;
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
VOS_UINT32 OM_SetBootFlagConfig(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                                    VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32 *pulTmpCmdPtr;

    pulTmpCmdPtr = (VOS_UINT32*)(pstAppToOmMsg->aucPara);
    if(VOS_OK!= DRV_BOOTFLAG_CFG(*pulTmpCmdPtr))
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetBootFlagConfig:BootFlagConfig_API\n");
        return VOS_ERR;
    }
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    return VOS_OK;
}


VOS_UINT32  OM_QueryVersionInfo(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                                     VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU               *pstOmToAppMsg;
    OM_QUERY_VERSION_INFO_STRU       *pstVersionInfo = VOS_NULL_PTR;
    VOS_UINT32                       ulTotalSize;
    VERSIONINFO_I                    *pstTmpData = VOS_NULL_PTR;
    VOS_UINT32                       ulDataLen;
    VOS_UINT32                       ulTempLen;
    VOS_UINT32                       temp;
    VOS_UINT32                       i;
    VOS_UINT8                        *pucData;
    VOS_UINT32                       ulPlatformInfo;

    ulTotalSize = OM_APP_MSG_EX_LEN + sizeof(OM_QUERY_VERSION_INFO_STRU);

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                                    DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryVersionInfo: VOS_MemAlloc.\n");
        return VOS_ERR;
    }

    pstVersionInfo = (OM_QUERY_VERSION_INFO_STRU*)(pstOmToAppMsg->aucPara);

    /*工具侧通过此标识判断是否为ASIC版本，用来计算TimeStamp的间隔时间*/
#if (VOS_WIN32 == VOS_OS_VER)
    pstVersionInfo->ulPlatformType = OM_WIN32_PLATFORM;
#else
    DRV_GET_PLATFORM_INFO(&ulPlatformInfo);

    if(VER_ASIC == ulPlatformInfo)
    {
        pstVersionInfo->ulPlatformType = OM_ASIC_PLATFORM;
    }
    else/* VER_SOC */
    {
        pstVersionInfo->ulPlatformType = OM_FPGA_PLATFORM;
    }
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

    /*Call the driver interface.*/
    if (VOS_OK != DRV_VER_QUERY((VOS_VOID**)(&pstTmpData), (VOS_UINT*)(&ulDataLen)))
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryVersionInfo: versionQueryApi.\n!");
        return VOS_ERR;
    }

    ulTempLen = sizeof(pstVersionInfo->staVersion);
    vos_printf("\nOM_QueryVersionInfo: ulDataLen=%d ulTempLen=%d \n",ulDataLen,ulTempLen);

    if ( ulDataLen !=  ulTempLen )
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryVersionInfo: Exec fail.\n!");
        return VOS_ERR;
    }

    /*Version comes from driver assign to version struct.*/
    VOS_MemCpy(pstVersionInfo->staVersion, pstTmpData, ulTempLen);

    /*Assign the length field.*/
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    /* Debug */
    temp = (VOS_UINT16)(pstOmToAppMsg->usLength + VOS_OM_HEADER_LEN);
    pucData = (VOS_UINT8*)pstOmToAppMsg;
    if (1 == g_ulOmVersionDbg)
    {
        vos_printf("\nOM_QueryVersionInfo Data:len: = %d\n", temp);
        for ( i = 0 ; i < temp; i++ )
        {
            vos_printf(" %x ", pucData[i]);
        }
        vos_printf("\r\n");
    }

    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return VOS_OK;
}
VOS_UINT32  OM_SysparaGetUsimFileReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                                  VOS_UINT16 usReturnPrimId)
{
    OM_QUERY_USIMM_STRU *pstGetUSIMReq;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    pstGetUSIMReq = (OM_QUERY_USIMM_STRU*)(pstAppToOmMsg->aucPara);

    stGetFileInfo.enAppType     = USIMM_UNLIMIT_APP;
    stGetFileInfo.ucRecordNum   = pstGetUSIMReq->ucRecordNum;
    stGetFileInfo.usEfId        = pstGetUSIMReq->usEfId;

    /*Call USIM inferface function.*/
    if ( VOS_OK != USIMM_GetFileReq(WUEPS_PID_OM, 0, &stGetFileInfo) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 OM_SysparaSetUsimFileReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                                      VOS_UINT16 usReturnPrimId)
{
    OM_CONFIG_USIMM_STRU        *pstUpdateUSIMReq;
    USIMM_SET_FILE_INFO_STRU    stUpdateReq;

    pstUpdateUSIMReq = (OM_CONFIG_USIMM_STRU*)(pstAppToOmMsg->aucPara);

    stUpdateReq.enAppType       = USIMM_UNLIMIT_APP;
    stUpdateReq.pucEfContent    = pstUpdateUSIMReq->aucEf;
    stUpdateReq.ucRecordNum     = pstUpdateUSIMReq->ucRecordNum;
    stUpdateReq.ulEfLen         = pstUpdateUSIMReq->ucEfLen;
    stUpdateReq.usEfId          = pstUpdateUSIMReq->usEfId;

    /*Call the USIM interface function.*/
    if ( VOS_OK != USIMM_SetFileReq(WUEPS_PID_OM, 0, &stUpdateReq) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 OM_LmtMsgProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_LMT_MSG_STRU         *pstLmtMsg;
    VOS_BOOL                bRet;
    OM_SW_VER_FLAG_STRU     stCustomVersion;
    VOS_UINT32              ulResult;
    MODEM_ID_ENUM_UINT16    enModemID;
    VOS_RATMODE_ENUM_UINT32 enMode;

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    if(MODEM_ID_BUTT <= enModemID)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetFTMReq:Modem ID Error!");
        return VOS_ERR;
    }

    pstLmtMsg = (OM_LMT_MSG_STRU*)(pstAppToOmMsg->aucPara);

    if (VOS_YES == pstLmtMsg->ucNvEnable)
    {
        /*指示物理层需要更新NV项*/
        if(MODEM_ID_1 == enModemID)
        {
            ulResult = APM_ConfigGDsp1Nv(VOS_TRUE);
        }
        else
        {
            ulResult = APM_ConfigDspNv(VOS_TRUE);   /*需要重新配置NV数据*/
        }

        if ( VOS_OK !=  ulResult)
        {
            OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetFTMReq:refresh NV fail!");
            return VOS_ERR;
        }
    }

    /* 当usSysMode为0xffff代表不加载DSP */
    if (VOS_NULL_WORD != pstLmtMsg->usSysMode)
    {
        if(LMT_RAT_WCDMA == pstLmtMsg->usSysMode)
        {
            enMode = VOS_RATMODE_WCDMA;
        }
        else if(LMT_RAT_GSM == pstLmtMsg->usSysMode)
        {
            enMode = VOS_RATMODE_GSM;
        }
        else
        {
            OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetFTMReq:TOOL RAT Modem Error!");
            return VOS_ERR;
        }

        if (VOS_FALSE == pstLmtMsg->ucFtmMode)
        {
            g_ulOmNosigEnable = VOS_FALSE;
            /*加载物理层*/
            bRet = SHPA_LoadPhy(enMode, enModemID, UPHY_OAM_BUSINESS_TYPE_CT);
        }
        else
        {
            /* 判断是否为烧片版本 */
            if ( VOS_OK != NV_Read(en_NV_Item_SW_VERSION_FLAG, &stCustomVersion, sizeof(stCustomVersion)) )
            {
                stCustomVersion.ulNvSwVerFlag = VOS_TRUE;
            }

            /* 如果ulCustomVersion == VOS_TRUE表示不是烧片版本，则直接返回，告诉不支持非信令 */
            if ( VOS_TRUE == stCustomVersion.ulNvSwVerFlag)
            {
                OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
                PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetFTMReq:Don't support nonsig bt!");
                return VOS_ERR;
            }

            /* 初始化非信令控制结构 */
            OM_NoSigCtrlInit();

            g_ulOmNosigEnable = MAIL_BOX_PROTECTWORD_SND;

            bRet = SHPA_LoadPhy(enMode, enModemID, UPHY_OAM_BUSINESS_TYPE_NO_SIG_BT);
        }

        if (VOS_FALSE == bRet)
        {
            OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SetFTMReq:SHPA_LoadCal fail!");
            return VOS_ERR;
        }
    }

    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);

    return VOS_OK;
}
VOS_UINT32 OM_ActivePhyProc(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_ACTIVE_PHY_STRU                 *pstActiveMsg;
    VOS_RATMODE_ENUM_UINT32             enRatMode;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulResult;

    enModemId = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    pstActiveMsg = (OM_ACTIVE_PHY_STRU*)pstAppToOmMsg->aucPara;

    /* 工具侧定义与单板侧不一致 */
    if(LMT_RAT_WCDMA == pstActiveMsg->usSysMode)
    {
        enRatMode = VOS_RATMODE_WCDMA;
    }
    else if(LMT_RAT_GSM == pstActiveMsg->usSysMode)
    {
        enRatMode = VOS_RATMODE_GSM;
    }
    else
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ActivePhyProc:TOOL RAT Modem Error!");

        return VOS_ERR;
    }

    ulResult = SHPA_ActiveDSP(enRatMode, enModemId);

    OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

    return ulResult;
}
VOS_UINT32 OM_QueryDspType(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                        VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32 ulDspType;

    ulDspType = SHPA_GetRateType(OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType));

    OM_SendResult(pstAppToOmMsg->ucFuncType, ulDspType, usReturnPrimId);

    g_bDSPModeFlag = VOS_TRUE;   /* 查询过一次就一直上报， 如不需上报必须下电 */

    return VOS_OK;
}
VOS_VOID OM_DspTypeInd(VOS_UINT8 ucModemId,VOS_UINT32 ulPhyMode, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT8       ucFuncType;

    if (VOS_FALSE == g_bDSPModeFlag)
    {
        return;
    }

    ucFuncType = OM_ADD_FUNCID_MODEMINFO(OM_QUERY_FUNC,ucModemId);

    OM_SendResult(ucFuncType, ulPhyMode, usReturnPrimId);

    return ;
}


VOS_UINT32 OM_QueryPA(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU              *pstOmToAppMsg;
    VOS_UINT32                       ulTotalSize;
    VOS_UINT16                      *pusData;
    OM_APP_PA_ATTRIBUTE_STRU        *pstPaAttribute;
    VOS_UINT16                       usQueryType = 0;
    VOS_UINT32                       ulIndex;
    VOS_UINT32                       ulNum;
    VOS_INT16                        sTempValue = 0;
    VOS_UINT16                       usVoltValue = 0;
    VOS_INT                          lStatus = 0;
    VOS_INT32                        lResult = VOS_OK;
    VOS_UINT32                       ulResult;
    MODEM_ID_ENUM_UINT16             enModemID;
    HKADC_TEMP_PROTECT_E             enTempType;

    /*指向查询类型数组的指针*/
    pusData = (VOS_UINT16*)(pstAppToOmMsg->aucPara);
    /*得到所有查询类型所占的字节数*/
    ulNum = pstAppToOmMsg->usLength - (OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN);
    /*获取查询类型的个数*/
    ulNum = ulNum/sizeof(usQueryType);
    /*返回数据包的总长度，参见OM_APP_PA_ATTRIBUTE_STRU*/
    ulTotalSize = (OM_APP_MSG_EX_LEN + sizeof(VOS_UINT32))
                              + (ulNum*sizeof(OM_PA_ITEM_STRU));
    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                                          DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryPA: VOS_MemAlloc.\n");
        return VOS_ERR;
    }

    enModemID = OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType);

    pstPaAttribute = (OM_APP_PA_ATTRIBUTE_STRU*)(pstOmToAppMsg->aucPara);
    usQueryType = *pusData;

    /*调用底软接口，获取PA属性*/
    for (ulIndex = 0; ulIndex < ulNum; ulIndex++)
    {
        usQueryType = *pusData;
        switch (usQueryType)
        {
        case OM_W_PA_TEMP:
        case OM_G_PA_TEMP:
            if(MODEM_ID_0 == enModemID)
            {
                enTempType = HKADC_TEMP_PA0;
            }
            else
            {
                enTempType = HKADC_TEMP_PA1;
            }

            lResult = DRV_HKADC_GET_TEMP(enTempType, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
            lStatus = sTempValue;
            break;

        case OM_G_PLL_LOCK:
            lResult = DRV_GET_RF_GLOCKSTATE((VOS_UINT*)(&lStatus));
            break;

        case OM_W_HKADC:
        case OM_G_HKADC:
            if(MODEM_ID_0 == enModemID)
            {
                enTempType = HKADC_TEMP_PA0;
            }
            else
            {
                enTempType = HKADC_TEMP_PA1;
            }

            lResult = DRV_HKADC_GET_TEMP(enTempType, &usVoltValue, VOS_NULL_PTR, HKADC_CONV_DELAY);
            lStatus = (VOS_INT)usVoltValue;
            break;

        case OM_W_BBP_PLL_LOCK:
            lStatus = (VOS_INT)DRV_GET_WCDMA_PLL_STATUS();
            break;
        case OM_G_BBP_PLL_LOCK:
            lStatus = (VOS_INT)DRV_GET_GSM_PLL_STATUS();
            break;
        case OM_DSP_PLL_LOCK:
            lStatus = (VOS_INT)DRV_GET_DSP_PLL_STATUS();
            break;
        case OM_ARM_PLL_LOCK:
            lStatus = (VOS_INT)DRV_GET_ARM_PLL_STATUS();
            break;
        case OM_SIM_TEMP:
            lResult = DRV_HKADC_GET_TEMP(HKADC_TEMP_SIM_CARD, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
            lStatus = sTempValue;
            break;
        case OM_SDMMC_STATUS:
            lStatus = (VOS_INT)DRV_SDMMC_USB_STATUS();
            break;
        case OM_BATTER_VOLT:
            if(BSP_MODULE_SUPPORT == DRV_GET_BATTERY_SUPPORT()) /*调用底软接口判断电池是否支持*/
            {
                lResult = DRV_HKADC_BAT_VOLT_GET((VOS_INT*)&lStatus);
            }
            else
            {
                lResult = VOS_OK;
            }
            break;
        case OM_BATTER_TEMP:
            if(BSP_MODULE_SUPPORT == DRV_GET_BATTERY_SUPPORT()) /*调用底软接口判断电池是否支持*/
            {
                lResult = DRV_HKADC_GET_TEMP(HKADC_TEMP_BATTERY, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
                lStatus = sTempValue;
            }
            else
            {
                lResult = VOS_OK;
            }
            break;
        case OM_OLED_TEMP:
            if(BSP_MODULE_SUPPORT == DRV_GET_OLED_SUPPORT())
            {
                lResult = DRV_HKADC_GET_TEMP(HKADC_TEMP_LCD, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
                lStatus = sTempValue;
            }
            else
            {
                lResult = VOS_OK;
            }

            break;
        case OM_DCXO_TEMP:
            if(MODEM_ID_0 == enModemID)
            {
                enTempType = HKADC_TEMP_DCXO0;
            }
            else
            {
                enTempType = HKADC_TEMP_DCXO1;
            }

            lResult = DRV_HKADC_GET_TEMP(enTempType, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
            lStatus = sTempValue;
            break;
        case OM_DCXO_TEMP_LT:
            if(MODEM_ID_0 == enModemID)
            {
                enTempType = HKADC_TEMP_DCXO0_LOW;
            }
            else
            {
                enTempType = HKADC_TEMP_DCXO1_LOW;
            }

            lResult = DRV_HKADC_GET_TEMP(enTempType, VOS_NULL_PTR, &sTempValue, HKADC_CONV_DELAY);
            lStatus = sTempValue;
            break;
        default:
            lResult = VOS_ERR;
            break;
        }
        /*查询失败*/
        if (VOS_OK != lResult)
        {
            /*Just for PcLint*/
            ulResult = (VOS_UINT32)lResult;
            ulResult <<= 16;
            ulResult |= usQueryType;
            VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
            OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                      "OM_QueryPA:Failed type.\n", (VOS_INT32)ulResult);
            return VOS_ERR;
        }
        /*填写查询的结果*/
        pstPaAttribute->aPaItem[ulIndex].usQueryType  = usQueryType;
        pstPaAttribute->aPaItem[ulIndex].sQueryResult = (VOS_INT16)lStatus;
        pusData++;
    }
    /*Assign the length field.*/
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    pstPaAttribute->ulResult = VOS_OK;

    if (( BSP_MODULE_UNSUPPORT == DRV_GET_BATTERY_SUPPORT() )
        &&( OM_BATTER_TEMP == usQueryType ))
    {
        pstPaAttribute->ulResult = VOS_ERR; /* STICK上不支持的查询项返回VOS_ERR */
    }

    if ((BSP_MODULE_UNSUPPORT == DRV_GET_OLED_SUPPORT())
        &&( OM_OLED_TEMP== usQueryType ))
    {
        pstPaAttribute->ulResult = VOS_ERR; /* STICK上不支持的查询项返回VOS_ERR */
    }

    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
VOS_UINT32 OM_RebootUE(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    /*延迟500毫秒，以保证上面的确认消息被PC侧接收*/
    VOS_TaskDelay(500);

    VOS_FlowReboot();

    return VOS_OK;
}
VOS_UINT32 OM_GetVersion(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32              ulVersionType;
    OM_FACTORY_VER_STRU     stFactoryVersion;
    VOS_INT32               lResult;

    ulVersionType = *((VOS_UINT32*)pstAppToOmMsg->aucPara);

    VOS_MemSet(&stFactoryVersion, 0, sizeof(OM_FACTORY_VER_STRU));

    /*获取软件版本信息*/
    lResult = DRV_MEM_VERCTRL((VOS_INT8*)(stFactoryVersion.aucSoftwareVersion),
                    OM_FACTORY_VER_LEN - 1, VER_SOFTWARE, VERIONREADMODE);

    /*对外部版本进行特殊处理*/
    if (OM_OUTSIDE_VER == ulVersionType)
    {
        if (VOS_OK == lResult)
        {
            VOS_INT8* pcVersion = (VOS_INT8*)stFactoryVersion.aucSoftwareVersion;
            VOS_INT8* pcTemp;
            pcTemp = (VOS_INT8*)VOS_StrRChr((VOS_CHAR*)pcVersion, 'S');
            if (VOS_NULL_PTR != pcTemp)
            {
                VOS_MemSet(pcTemp, 0, OM_FACTORY_VER_LEN - (VOS_UINT32)(pcTemp - pcVersion));
            }
        }

        /*硬件版本*/
        DRV_MEM_VERCTRL((VOS_INT8*)(stFactoryVersion.aucHardwareVersion),
                OM_FACTORY_VER_LEN - 1, VER_HARDWARE, VERIONREADMODE);

        /*获取产品信息*/
        DRV_MEM_VERCTRL((VOS_INT8*)(stFactoryVersion.aucProductVersion),
                    OM_FACTORY_VER_LEN - 1, VER_PRODUCT_ID, VERIONREADMODE);
    }
    /*对内部版本进行特殊处理*/
    else if (OM_INSIDE_VER == ulVersionType)
    {
        /*硬件版本*/
        DRV_GET_FULL_HW_VER((VOS_CHAR*)stFactoryVersion.aucHardwareVersion,
                        OM_FACTORY_VER_LEN - 1);

        /*获取产品信息*/
        DRV_GET_PRODUCTID_INTER_VER(
            (VOS_CHAR*)(stFactoryVersion.aucProductVersion), OM_FACTORY_VER_LEN - 1);
    }
    else
    {
        /*Make PcLint happy.*/
    }

    /*获取后台工具版本*/
    DRV_GET_CDROM_VERSION((VOS_CHAR*)stFactoryVersion.aucCdRomVersion, OM_CDROM_VER_LEN);

    /*发送给PC侧工具*/
    stFactoryVersion.ulResult = VOS_OK;
    stFactoryVersion.usLength = sizeof(OM_FACTORY_VER_STRU) - VOS_OM_HEADER_LEN;
    OM_SendContent(pstAppToOmMsg->ucFuncType, (OM_APP_MSG_EX_STRU*)(&stFactoryVersion), usReturnPrimId);
    return VOS_OK;
}


VOS_UINT32 OM_SysCtrlCmd(VOS_UINT16 usClientId, VOS_UINT32 ulMode, MODEM_ID_ENUM_UINT16 enModemID)
{
    MN_APP_PHONE_MODE_SET_REQ_STRU *pstMsg;

    if(MODEM_ID_BUTT <= enModemID)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SysCtrlCmd: Modem ID Error!");

        return VOS_ERR;
    }

    pstMsg = (MN_APP_PHONE_MODE_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_OM,
            sizeof(MN_APP_PHONE_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_SysCtrlCmd: VOS_AllocMsg failed!");

        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemID)
    {
        pstMsg->ulReceiverPid      = I1_WUEPS_PID_MMA;
    }
    else
    {
        pstMsg->ulReceiverPid      = I0_WUEPS_PID_MMA;
    }

    pstMsg->ulMsgId            = OAM_MMA_PHONE_MODE_SET_REQ;

    /* 发送消息给NAS进行系统控制 */
    pstMsg->usClientId         = usClientId;
    pstMsg->opID               = 0;
    pstMsg->stPhOpMode.CmdType = TAF_PH_CMD_SET;
    pstMsg->stPhOpMode.PhMode  = (VOS_UINT8)ulMode;
    pstMsg->stPhOpMode.PhReset = TAF_PH_OP_MODE_UNRESET;

    return VOS_SendMsg(WUEPS_PID_OM, pstMsg);
}
VOS_UINT32 OM_SetFTMReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_AUTOCONFIG_CNF_STRU          *pstSendCnf;

    if (VOS_OK != OM_SysCtrlCmd(OAM_CLIENT_ID_OM,
                                *((VOS_UINT32*)pstAppToOmMsg->aucPara),
                                OM_GET_FUNCID_MODEMINFO(pstAppToOmMsg->ucFuncType)))
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);

        return VOS_ERR;
    }

    g_ulFTMFlag = VOS_TRUE;


    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return VOS_ERR;
    }

    pstSendCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_SET_FTM_MODE_REQ;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstSendCnf);

    return VOS_OK;
}
VOS_UINT32 OM_LoadDefCmd(VOS_UINT16 usClientId)
{
    MN_APP_PHONE_LOADDEFAULT_REQ_STRU *pstMsg;

    pstMsg = (MN_APP_PHONE_LOADDEFAULT_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_OM,
            sizeof(MN_APP_PHONE_LOADDEFAULT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_LoadDefCmd: VOS_AllocMsg failed!");

        return VOS_ERR;
    }

    pstMsg->ulReceiverPid      = WUEPS_PID_MMA;
    pstMsg->ulMsgId            = OAM_MMA_PHONE_LOADDEFAULT_REQ;
    pstMsg->usClientId         = usClientId;
    pstMsg->opID               = 0;

    return VOS_SendMsg(WUEPS_PID_OM, pstMsg);
}


VOS_UINT32 OM_LoadDefault(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    if (VOS_OK != OM_LoadDefCmd(OAM_CLIENT_ID_OM))
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID OM_Num2BCD(const VOS_UINT8 *pucSrc, VOS_UINT8 *pucDest, VOS_UINT32 ulLength)
{
    VOS_UINT8   ucNumIndex;

    for (ucNumIndex = 0; ucNumIndex < ulLength; ucNumIndex++)
    {
        if (0 == (ucNumIndex % 2))
        {
            pucDest[ucNumIndex / 2] = pucSrc[ucNumIndex] & 0xF;
        }
        else
        {
            pucDest[ucNumIndex / 2] |= ((pucSrc[ucNumIndex] & 0xF) << 4);
        }
    }

    if (1 == (ulLength % 2))
    {
        pucDest[ulLength / 2] |= 0xF0; /* 高位 */
    }
}
VOS_UINT32 OM_QueryUELabel(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                  ulResult;
    OM_FACTORY_LABEL_STRU       stOmImei;
    IMEI_STRU                   stIMEI;
    VOS_UINT8                   ucCheckData = 0;
    VOS_UINT8                   i;

    VOS_MemSet(&stIMEI, 0, sizeof(stIMEI));
    VOS_MemSet(&stOmImei, 0, sizeof(stOmImei));

    ulResult = NV_Read(en_NV_Item_IMEI, (VOS_VOID*)&stIMEI, sizeof(IMEI_STRU));
    if (NV_OK != ulResult)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);
        return VOS_ERR;
    }

    /*计算校验值*/
    for (i = 0; i < (OM_IMEI_NV_LEN - 2); i += 2)
    {
        ucCheckData += stIMEI.aucImei[i]
                     +((stIMEI.aucImei[i+1] + stIMEI.aucImei[i+1])/10)
                     +((stIMEI.aucImei[i+1] + stIMEI.aucImei[i+1])%10);
    }
    ucCheckData = (10 - (ucCheckData%10))%10;
    stIMEI.aucImei[14]  = ucCheckData;

    OM_Num2BCD(stIMEI.aucImei, stOmImei.aucIMEI + 1, OM_IMEI_NV_LEN);

    stOmImei.aucIMEI[0] = OM_IMEI_LEN - 1;
    stOmImei.ulResult = VOS_OK;
    stOmImei.usLength = sizeof(OM_FACTORY_LABEL_STRU) - VOS_OM_HEADER_LEN;

    OM_SendContent(pstAppToOmMsg->ucFuncType, (OM_APP_MSG_EX_STRU*)&stOmImei,
                        usReturnPrimId);
    return VOS_OK;
}
VOS_UINT32 OM_QueryMemInfo(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU              *pstOmToAppMsg;
    OM_MEMORY_INFO_STRU             *pstMemInfo;
    VOS_MEM_OCCUPYING_INFO_ST       *pstMemOccupyInfo;
    VOS_UINT32                       ulTotalSize;
    VOS_UINT32                       ulPidNum;

    /*获取当前系统PID的个数*/
    ulPidNum = VOS_PID_BUTT;
    ulPidNum = ulPidNum - VOS_PID_DOPRAEND;

    /*由于PID的内存信息分为普通内存和消息内存两种，所以分配空间要乘2*/
    ulTotalSize = (OM_APP_MSG_EX_LEN + sizeof(OM_MEMORY_INFO_STRU))
                    + (((2*ulPidNum) - 1)*sizeof(VOS_MEM_OCCUPYING_INFO_ST));

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                                    DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryMemInfo: VOS_MemAlloc.\n");
        return VOS_ERR;
    }

    pstMemInfo = (OM_MEMORY_INFO_STRU*)(pstOmToAppMsg->aucPara);

    /*调用底软接口获取当前的堆状态*/
    if (VOS_OK != DRV_GET_HEAPINFO(&(pstMemInfo->ulAllocMemSize),
                                   &(pstMemInfo->ulTotalMemSize)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryMemInfo: DRV_GET_HEAPINFO.\n");
        return VOS_ERR;
    }

    pstMemOccupyInfo = (VOS_MEM_OCCUPYING_INFO_ST*)(pstMemInfo->astMemOccupyInfo);

    /*获取每个PID所使用的内存信息*/
    if (VOS_OK != VOS_GetMemOccupyingInfo(pstMemOccupyInfo,
                           ulPidNum*sizeof(VOS_MEM_OCCUPYING_INFO_ST)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryMemInfo: VOS_GetMemOccupyingInfo.\n");
        return VOS_ERR;
    }

    pstMemOccupyInfo += ulPidNum;

    /*获取每个PID所使用的消息信息*/
    if (VOS_OK != VOS_GetMsgOccupyingInfo(pstMemOccupyInfo,
                            ulPidNum*sizeof(VOS_MEM_OCCUPYING_INFO_ST)))
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_QueryMemInfo: VOS_GetMsgOccupyingInfo.\n");
        return VOS_ERR;
    }

    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);

    pstMemInfo->ulResult = VOS_OK;
    pstMemInfo->ulMinPid = VOS_PID_DOPRAEND;
    pstMemInfo->ulPidNum = ulPidNum;

    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
VOS_UINT32 OM_EstablishReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    /* 链路断开 */
    g_ulOMSwitchOnOff = OM_STATE_IDLE;

    OM_SendCcpuSocpVote(SOCP_VOTE_FOR_WAKE);

    VOS_MemSet(&g_stTraceEventConfig, 0, sizeof(OM_TRACE_EVENT_CONFIG_PS_STRU));
    VOS_MemSet(g_aulLogPrintLevPsTable, 0, LOG_PS_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));
    VOS_MemSet(g_aulLogPrintLevDrvTable, 0, LOG_DRV_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));

    /* 链路打开 */
    g_ulOMSwitchOnOff = OM_STATE_ACTIVE;

    return VOS_OK;
}


VOS_UINT32 OM_ReleaseReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId)
{
    g_ulOMSwitchOnOff           = OM_STATE_IDLE;

    /* 清空配置参数 */
    VOS_MemSet(&g_stTraceEventConfig, 0, sizeof(OM_TRACE_EVENT_CONFIG_PS_STRU));
    VOS_MemSet(g_aulLogPrintLevPsTable, 0, LOG_PS_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));
    VOS_MemSet(g_aulLogPrintLevDrvTable, 0, LOG_DRV_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));

    APM_OamClearSdtLinkStatus();
    Om_EndBbpDbg();

    OM_SendCcpuSocpVote(SOCP_VOTE_FOR_SLEEP);

    return VOS_OK;
}
VOS_VOID OM_InitAuthVariable(VOS_VOID)
{
    IMEI_STRU                   stIMEI;
    VOS_UINT8                   aucDefaultIMEI[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    /*假如IMEI为默认值，则不需要鉴权*/
    if (NV_OK == NV_Read(en_NV_Item_IMEI, (VOS_VOID*)&stIMEI, sizeof(stIMEI)))
    {
        if (0 == VOS_MemCmp((VOS_CHAR*)aucDefaultIMEI, &stIMEI, sizeof(stIMEI)))
        {
            g_ulOMPrivilegeLevel = LEVEL_ADVANCED;
        }
    }

    return;
}
VOS_UINT32 OM_Authorize(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_PASSWORD_STRU    *pstPassword;

    g_ulOMPrivilegeLevel = LEVEL_NORMAL;

    pstPassword = (OM_PASSWORD_STRU*)(pstAppToOmMsg->aucPara);

    /*获取当前的解锁码*/
    if (VOS_OK != MMA_VerifyOperatorLockPwd((VOS_UINT8*)(pstPassword->aucPassword)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
           "OM_Authorize:MMA_VerifyOperatorLockPwd failed");
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);

        return VOS_ERR;
    }

    g_ulOMPrivilegeLevel = LEVEL_ADVANCED;
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    return VOS_OK;
}
/*lint +e416 +e661 +e662 +e717*/


VOS_UINT32 OM_GetFilterListReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                     ulNvNum;
    VOS_UINT32                     ulTotalLen;
    OM_APP_MSG_EX_STRU             *pstOmToAppMsg;
    VOS_CHAR                       *pcSrc;
    VOS_UINT32                      ulFilterIdLen;
    OM_APP_GET_NV_FILTER_LIST_STRU *pstOmGetFilterNvList;

    ulNvNum = NV_GetResumeNvIdNum(NV_MANUFACTURE_ITEM) + NV_GetResumeNvIdNum(NV_USER_ITEM);

    /*APP_HEAD + OM_APP_GET_NV_FILTER_LIST_STRU + NV Filter ID lists*/
    ulTotalLen = sizeof(OM_APP_MSG_EX_STRU)
                 + sizeof(OM_APP_GET_NV_FILTER_LIST_STRU)
                 + (ulNvNum*sizeof(VOS_UINT16));

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(
                                   WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalLen);

    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetFilterListReq:VOS_MemAlloc Fail.");

        return VOS_ERR;
    }

    pstOmGetFilterNvList = (OM_APP_GET_NV_FILTER_LIST_STRU*)(pstOmToAppMsg->aucPara);

    /* 获取 Filter User NV ID */
    pcSrc = (VOS_CHAR *)(pstOmGetFilterNvList->ausNvFilterList);
    ulFilterIdLen = NV_GetResumeNvIdNum(NV_USER_ITEM);

    NV_GetResumeNvIdList(NV_USER_ITEM, (VOS_UINT16*)pcSrc, ulFilterIdLen);

    ulFilterIdLen *= sizeof(VOS_UINT16);

    /* 获取 Filter Manufacture NV ID */
    pcSrc += ulFilterIdLen;
    ulFilterIdLen = NV_GetResumeNvIdNum(NV_MANUFACTURE_ITEM);

    NV_GetResumeNvIdList(NV_MANUFACTURE_ITEM, (VOS_UINT16*)pcSrc, ulFilterIdLen);

    /*获取产品信息*/
    pstOmGetFilterNvList->ulProductId = (VOS_UINT32)DRV_GET_HW_VERSION_INDEX();

    /* 填写执行结果和NV Filter项数目 */
    pstOmGetFilterNvList->ulErrorCode = VOS_OK;
    pstOmGetFilterNvList->ulCount     = ulNvNum;

    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalLen - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return VOS_OK;
}
VOS_UINT32 OM_CtrlAptReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32  ulAptSwitch;
    VOS_INT     lResult = VOS_ERR;

    ulAptSwitch = *((VOS_UINT32*)pstAppToOmMsg->aucPara);

    if (VOS_TRUE == ulAptSwitch)
    {
        lResult = DRV_PMU_APT_ENABLE();
    }
    else if (VOS_FALSE == ulAptSwitch)
    {
        lResult = DRV_PMU_APT_DISABLE();
    }
    else
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                "OM_CtrlAptReq:Para error.", (VOS_INT32)ulAptSwitch);
    }

    OM_SendResult(pstAppToOmMsg->ucFuncType, (VOS_UINT32)lResult, usReturnPrimId);

    return VOS_OK;
}


VOS_UINT32 OM_QueryModemNumReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                          ulModemNum;

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
    ulModemNum = (VOS_UINT32)MODEM_ID_BUTT;
#else
    ulModemNum = (VOS_UINT32)(MODEM_ID_0 + 1);
#endif

    OM_SendResult(pstAppToOmMsg->ucFuncType, (VOS_UINT32)ulModemNum, usReturnPrimId);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_BROWSER_NV_FILE_IMEI_PROTECT)

VOS_UINT32 OM_QueryPrivilegeLevel(VOS_VOID)
{
    return g_ulOMPrivilegeLevel;
}

#endif


VOS_UINT32 OM_WriteFileReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    OM_AUTOCONFIG_CNF_STRU          *pstSendCnf;

    OM_LogShowToFile(VOS_FALSE);

    pstSendCnf = (OM_AUTOCONFIG_CNF_STRU*)VOS_AllocMsg(CCPU_PID_OMAGENT,
                            sizeof(OM_AUTOCONFIG_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 分配消息失败 */
    if (VOS_NULL_PTR == pstSendCnf)
    {
        return VOS_ERR;
    }

    pstSendCnf->ulReceiverPid  = ACPU_PID_OMAGENT;
    pstSendCnf->usPrimId       = OM_RECORD_DBU_INFO_REQ;
    pstSendCnf->aucData[0]     = pstAppToOmMsg->ucFuncType;

    (VOS_VOID)VOS_SendMsg(CCPU_PID_OMAGENT, pstSendCnf);

    return VOS_OK;
}
VOS_UINT32 OM_QuerySliceReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU             *pstOmToAppMsg;
    OM_QUERY_SLICE_CNF_STRU        *pstSendCnf;
    VOS_UINT32                     ulResult;
    VOS_UINT32                     ulTotalSize;

    /* 减去OM_APP_MSG_EX_STRU结构中内容字段4字节，再加上内容字段OM_QUERY_SLICE_CNF_STRU结构长度 */
    ulTotalSize = sizeof(OM_APP_MSG_EX_STRU) - 4 + sizeof(OM_QUERY_SLICE_CNF_STRU);

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                       ulTotalSize);

    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        /*0 indicates the count of the memory contents is zero, failed.*/
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_ERR, usReturnPrimId);
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_QuerySliceReq:VOS_MemAlloc.\n");
        return VOS_ERR;
    }

    pstSendCnf = (OM_QUERY_SLICE_CNF_STRU*)(pstOmToAppMsg->aucPara);

#if (RAT_GU != RAT_MODE)
    ulResult = BSP_BBPGetCurTime(&pstSendCnf->ulSlice);
#else
    ulResult = VOS_ERR;
#endif

    /*lint -e774 GU下pclint认为if语句总成立 */
    if(BSP_OK != ulResult)
    {
       pstSendCnf->ulSlice = 0;
    }
    /*lint +e774 GU下pclint认为if语句总成立 */

    pstSendCnf->ulResult = ulResult;

    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);

    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);

    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);

    return VOS_OK;
}

/*****************************************************************************
  4 消息处理函数映射表
*****************************************************************************/
/*Global map table used to find the function according the PrimId.*/
OM_MSG_FUN_STRU g_astOmMsgFunTbl[] =
{
    {APP_OM_READ_NV_REQ,                OM_ReadNv,                    OM_APP_READ_NV_IND},
    {APP_OM_WRITE_NV_REQ,               OM_WriteNv,                   OM_APP_WRITE_NV_CNF},
    {APP_OM_NV_BACKUP_REQ,              OM_BackupNv,                  OM_APP_NV_BACKUP_CNF},
    {APP_OM_NV_ID_LIST_REQ,             OM_GetNvIdList,               OM_APP_NV_ID_LIST_CNF},
    {APP_OM_APER_STASTUS_REQ,           OM_GetAperiodicStatus,        OM_APP_APER_STASTUS_IND},
    {APP_OM_QUERY_MM_REQ,               OM_QueryMemory,               OM_APP_QUERY_MM_IND},
    {APP_OM_SET_BOOT_REQ,               OM_SetBootFlagConfig,         OM_APP_SET_BOOT_CNF},
    {APP_OM_QUERY_VER_REQ,              OM_QueryVersionInfo,          OM_APP_QUERY_VER_IND},
    {APP_OM_GET_USIM_REQ,               OM_SysparaGetUsimFileReq,     OM_APP_GET_USIM_CNF},
    {APP_OM_SET_USIM_REQ,               OM_SysparaSetUsimFileReq,     OM_APP_SET_USIM_CNF},
    {APP_OM_HANDLE_LMT_REQ,             OM_LmtMsgProc,                OM_APP_HANDLE_LMT_CNF},
    {APP_OM_ACTIVE_PHY_REQ,             OM_ActivePhyProc,             OM_APP_ACTIVE_PHY_CNF},
    {APP_OM_QUERY_DSP_REQ,              OM_QueryDspType,              OM_APP_QUERY_DSP_CNF},
    {APP_OM_PA_ATTRIBUTE_REQ,           OM_QueryPA,                   OM_APP_PA_ATTRIBUTE_IND},
    {APP_OM_REBOOT_REQ,                 OM_RebootUE,                  OM_APP_REBOOT_CNF},
    {APP_OM_VERSION_REQ,                OM_GetVersion,                OM_APP_VERSION_IND},
    {APP_OM_SET_FTM_REQ,                OM_SetFTMReq,                 OM_APP_SET_FTM_CNF},
    {APP_OM_LOAD_DEFAULT_REQ,           OM_LoadDefault,               OM_APP_LOAD_DEFAULT_CNF},
    {APP_OM_QUERY_LABEL_REQ,            OM_QueryUELabel,              OM_APP_QUERY_LABEL_IND},
    {APP_OM_QUERY_MEM_INFO_REQ,         OM_QueryMemInfo,              OM_APP_QUERY_MEM_INFO_IND},
    {APP_OM_ESTABLISH_REQ,              OM_EstablishReq,              OM_APP_ESTABLISH_CNF},
    {APP_OM_RELEASE_REQ,                OM_ReleaseReq,                OM_APP_RELEASE_CNF},
    {APP_OM_AUTHORIZE_REQ,              OM_Authorize,                 OM_APP_AUTHORIZE_CNF},
    {APP_OM_FILTER_LIST_REQ,            OM_GetFilterListReq,          OM_APP_FILTER_LIST_CNF},
#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
    {APP_OM_START_UP_LOG_REQ,           OM_StartUpLogSaveReq,         OM_APP_START_UP_LOG_CNF},
    {APP_OM_STOP_LOG_REQ,               OM_StopLogSaveReq,            OM_APP_STOP_LOG_CNF},
#endif
    {APP_OM_CTRL_APT_REQ,               OM_CtrlAptReq,                OM_APP_CTRL_APT_CNF},
    {APP_OM_QUERY_MODEM_NUM_REQ,        OM_QueryModemNumReq,          OM_APP_QUERY_MODEM_NUM_CNF},
    {APP_OM_WRITE_NV_LOG_FILE_REQ,      OM_WriteFileReq,              OM_APP_WRITE_NV_LOG_FILE_CNF},
    {APP_OM_QUERY_SLICE_REQ,            OM_QuerySliceReq,             OM_APP_QUERY_SLICE_CNF},
};
VOS_VOID OM_QueryMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU      *pstAppToOmMsg;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulTotalNum;
    VOS_UINT16               usPrimId;
    VOS_UINT16               usReturnPrimId;
    VOS_UINT32               ulResult = VOS_ERR;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pRspPacket;
    ulTotalNum = sizeof(g_astOmMsgFunTbl)/sizeof(OM_MSG_FUN_STRU);
    usPrimId = pstAppToOmMsg->usPrimId;
    /*Search the corresponding function and return PrimId*/
    for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        if (usPrimId == (VOS_UINT16)(g_astOmMsgFunTbl[ulIndex].ulPrimId))
        {
            usReturnPrimId = (VOS_UINT16)(g_astOmMsgFunTbl[ulIndex].ulReturnPrimId);
            ulResult = g_astOmMsgFunTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);
            break;
        }
    }

    /*Can't find the function handles the usPrimId.*/
    if (ulIndex == ulTotalNum)
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usPrimId);
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_INFO, "OM_QueryMsgProc: PrimId can't be handled, : %d\n", (VOS_INT32)usPrimId);
    }
    return;
}

/*****************************************************************************
 Prototype       : OM_CcpuSocpVoteInfoAdd
 Description     : Ccpu OM Record the information of om vote. Only for K3V3
 Input           : enVote  - The information type value.
 Output          : None.
 Return Value    : None.

 History         : ---
    Date         : 2014-02-20
    Author       : zhuli
    Modification : Created function
 *****************************************************************************/

VOS_VOID OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTEINFO_ENUM_UINT32 enInfoType)
{
#if(FEATURE_ON == FEATURE_SOCP_ON_DEMAND)
    if (enInfoType >= OM_SOCPVOTE_INFO_BUTT)
    {
        return;
    }

    g_astOmCcpuSocpVoteInfo[enInfoType].ulCounter++;

    g_astOmCcpuSocpVoteInfo[enInfoType].ulSlice = OM_GetSlice();
#endif

    return;
}


/*****************************************************************************
 Prototype       : OM_SendCcpuSocpVote
 Description     : Ccpu OM send the Socp volte massage.
 Input           : enVote  - The vote value.
 Output          : None.
 Return Value    : None.

 History         : ---
    Date         : 2014-02-20
    Author       : zhuli
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_SendCcpuSocpVote(SOCP_VOTE_TYPE_ENUM_U32 enVote)
{
#if(FEATURE_ON == FEATURE_SOCP_ON_DEMAND)
    DIAG_MSG_SOCP_VOTE_REQ_STRU *pstMsg;

    if (enVote >= SOCP_VOTE_TYPE_BUTT)
    {
        OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTE_WARNNING);

        return;
    }

    pstMsg = (DIAG_MSG_SOCP_VOTE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_OM, sizeof(DIAG_MSG_SOCP_VOTE_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg) /*Alloc msg fail could reboot*/
    {
        OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTE_FATAL);

        return;
    }

    pstMsg->ulReceiverPid   = MSP_PID_DIAG_APP_AGENT;
    pstMsg->ulVoteId        = SOCP_VOTE_GU_OM_COMM;
    pstMsg->ulVoteType      = enVote;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_OM, pstMsg))
    {
        OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTE_ERROR);

        return;
    }

    if (SOCP_VOTE_FOR_SLEEP == enVote)
    {
        OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTE_SENDSLEEPMSG);
    }
    else
    {
        OM_CcpuSocpVoteInfoAdd(OM_SOCPVOTE_SENDWAKEMSG);
    }
#endif

    return;
}

/*****************************************************************************
 Prototype       : OM_CSocpVoteInfoShow
 Description     : Show the mnte info of OM Socp vote. Only for K3V3
 Input           : None.
 Output          : None.
 Return Value    : None.

 History         : ---
    Date         : 2014-02-20
    Author       : zhuli
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_CSocpVoteInfoShow(VOS_VOID)
{
#if(FEATURE_ON == FEATURE_SOCP_ON_DEMAND)
    VOS_UINT32  i;
    VOS_CHAR    *acInfoTital[OM_SOCPVOTE_INFO_BUTT] = { "OM_SOCPVOTE_CALLBEGIN",
                                                        "OM_SOCPVOTE_CALLEND",
                                                        "OM_SOCPVOTE_SENDSLEEPMSG",
                                                        "OM_SOCPVOTE_SENDWAKEMSG",
                                                        "OM_SOCPVOTE_WARNNING",
                                                        "OM_SOCPVOTE_ERROR",
                                                        "OM_SOCPVOTE_FATAL"};

    for(i=0; i<OM_SOCPVOTE_INFO_BUTT; i++)
    {
        vos_printf("\r\n%s: Counter is %d, Slice is 0x%x",  acInfoTital[i], 
                                                            g_astOmCcpuSocpVoteInfo[i].ulCounter,
                                                            g_astOmCcpuSocpVoteInfo[i].ulSlice);
    }
#endif

    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
