

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasLmmPubMInclude.h"
#include    "UsimPsInterface.h"
/* lihong00150010 ims begin */
#include    "SysNvId.h"
/* lihong00150010 ims begin */


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASMMPUBMNVIM_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

VOS_UINT32                              g_aucNvimBuff[NAS_NVIM_MAX_BUFFER];

static NAS_LMM_NV_ACT_STRU               g_astNvDataMap[] =
{
    /*NVIM数据*/
    {EN_NV_ID_UE_NET_CAPABILITY,    NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    /*{EN_NV_ID_MS_NET_CAPABILITY,    NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},*/
    {EN_NV_ID_IMSI,                 NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_EPS_LOC,              NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_SEC_CONTEXT,          NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_UE_CENTER,            NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
	/* lihong00150010 ims begin */
    #if 0
    {EN_NV_ID_UE_CS_SERVICE,        NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    #endif
    {en_NV_Item_UMTS_CODEC_TYPE,    NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {en_NV_Item_MED_CODEC_TYPE,    NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
	/* lihong00150010 ims end */
    {EN_NV_ID_UE_VOICE_DOMAIN,      NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_NAS_RELEASE,          NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_CONFIG_NWCAUSE,       NAS_USIM_FILE_ID_INVALID,     NAS_LMM_DecodeOneNvItem,   NAS_LMM_EncodeOneNvItem},

    /*SIM数据*/
    {EN_NV_ID_EPS_LOC_FILE,         NAS_USIM_FILE_ID_EPS_LOC_INFO, NAS_LMM_DecodeSimPsLoc,     NAS_LMM_EncodeSimPsLoc},
    {EN_NV_ID_IMSI_FILE,            NAS_USIM_FILE_ID_IMSI,        NAS_LMM_DecodeSimImsi,       NAS_LMM_EncodeOneNvItem},
    {EN_NV_ID_SEC_CONTEXT_FILE,     NAS_USIM_FILE_ID_SEC_CONTEXT, NAS_LMM_DecodeSimSecContext, NAS_LMM_EncodeSimSecContext},
    {EN_NV_ID_ACC_CLASSMASK_FILE,   NAS_USIM_FILE_ID_AC_CLASS,    NAS_LMM_DecodeNvACC,         NAS_LMM_EncodeOneNvItem},

    /* 软USIM的处理 */
    {EN_NV_ID_USIM_END,             NAS_USIM_FILE_ID_BUTT,        NAS_LMM_DecodeSoftUsimCnf,    NAS_LMM_EncodeOneNvItem},
    /*OM数据*/
};

VOS_UINT32 g_ulNvDataMapNum
            = sizeof(g_astNvDataMap)/sizeof(NAS_LMM_NV_ACT_STRU);


/* 将从USIM读取的文件ID */
VOS_UINT32 g_aulMmUsimEf[] = {  0x6F38, 0x6FAD, 0x6F7B, 0x6F31, 0x6F7E,
                                0x6F73, 0x6F07, 0x6F08, 0x6F09, 0x6FE4,
                                0x6F78};
VOS_UINT32 g_ulUsimSecuFileLen;


VOS_UINT8  g_aucUsimSecuContext[NAS_NVIM_SECU_CONTEXT_MAX_LEN] = {0}; /* 保存上次写卡的安全上下文码流 */


/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name  : NAS_LMM_NvimWrite
 Discription    : 其他模块调用本函数写一个指定的NV_Item
 Input          : ulNVItemType: NV_Item类型
*pData：NV的数据指针
                  usDataLen：要写入数据长度
 Output         : None
 Return         : 0：数据成功写入，函数正常返回
                  2：要写的NV类型超过最大值出错
                  4：系统异常，数据无法写入
 History:
     1.sunbing   49683      2010-7-22  modify
*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_UINT32  NAS_LMM_NvimWrite(   NAS_LMM_ITEM_TYPE_ENUM_UINT32    ulNVItemType,
                                VOS_VOID                        *pData,
                                VOS_UINT16                      usDataLen )
{
    VOS_UINT32                          ulRslt = NAS_LMM_NVIM_FAIL;
    NAS_NVIM_CTRL_TBL                   stNvCtrlTbl;
    VOS_UINT32                          i;
    NAS_LMM_NV_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;
    VOS_UINT16                          usEfId = 0;
    USIMM_SET_FILE_INFO_STRU            stSetFileInfo ={0};

    /*check input params*/
    if((NAS_NVIM_NULL_PTR == pData) ||
        (usDataLen == VOS_NULL) ||
        (ulNVItemType > EN_NV_ID_PS_END))
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimWrite:input para ERR");
        return NAS_LMM_NVIM_FAIL;
    }

    /*Initalize NVIM ctrl tbl*/
    stNvCtrlTbl.pData = pData;
    stNvCtrlTbl.pNvData = g_aucNvimBuff;
    stNvCtrlTbl.usDataLen = usDataLen;
    stNvCtrlTbl.usNvDataLen = 0;

    /* 在g_astNvDataMap中查找对应的NV TYPE*/
    for(i = 0; i < g_ulNvDataMapNum; i++)
    {
        /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
        if(ulNVItemType == g_astNvDataMap[i].ulEventType)
        {
            pActFun             = g_astNvDataMap[i].pfEncodeActionFun;
            usEfId              = g_astNvDataMap[i].usUsimFileId;
            break;
        }
    }

    if (NAS_LMM_NULL_PTR == pActFun)
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimWrite:Action functin is ERR");

        return NAS_LMM_NVIM_FAIL;
    }

    ulRslt = (*pActFun) (&stNvCtrlTbl);

    if ( NAS_LMM_NVIM_WRITTEN_UNNEEDED == ulRslt)
    {
        NAS_LMM_NVIM_LOG_INFO("NAS_LMM_NvimWrite: Don't need to write usim.");
        return NAS_LMM_NVIM_WRITTEN_UNNEEDED;
    }

    if(ulRslt != NAS_LMM_NVIM_OK)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimWrite:Encode is ERR");

        return NAS_LMM_NVIM_FAIL;
    }

    /*硬USIM下，并且NV ITEM属于USIM的范围，则向USIM中写文件*/
    if((EN_NV_ID_USIM_BEGIN          <  ulNVItemType)
        &&(EN_NV_ID_USIM_END            >  ulNVItemType))
    {
        NAS_LMM_NVIM_LOG1_INFO("NAS_LMM_NvimWrite:USIMM_SetFileReq, ulNVItemType = ", ulNVItemType);

        /*ucRecordNum默认为1，NAS目前涉及的文件是线形文件，且只有一个记录*/
        stSetFileInfo.enAppType    = USIMM_UMTS_APP;
        stSetFileInfo.ucRecordNum  = 1;
        stSetFileInfo.usEfId       = usEfId;
        stSetFileInfo.ulEfLen      = stNvCtrlTbl.usNvDataLen;
        stSetFileInfo.pucEfContent = stNvCtrlTbl.pNvData;

        /*
        ulRslt = USIMM_SetFileReq(      PS_USIM_CLIENT_LMM,
                                        usEfId,
                                       (VOS_UINT8*)stNvCtrlTbl.pNvData,
                                       (VOS_UINT8 )stNvCtrlTbl.usNvDataLen,
                                        ucRecordNum );*/

        ulRslt = USIMM_SetFileReq(      PS_PID_MM,
                                        0,
                                        &stSetFileInfo );

        NAS_LMM_NVIM_LOG3_INFO("usNvDataLen = , len = , usEfId = ",
                                        stNvCtrlTbl.usNvDataLen,
                                        stNvCtrlTbl.usNvDataLen,
                                        usEfId);
        NAS_COMM_PrintArray(NAS_COMM_GET_MM_PRINT_BUF(), (VOS_UINT8*)stNvCtrlTbl.pNvData,stNvCtrlTbl.usNvDataLen);

        return ulRslt;
    }

    /*其他情况向NV中写信息*/
    /*lint -e718*/
    /*lint -e516*/
    /*lint -e732*/
    ulRslt = LPs_NvimItem_Write(     ulNVItemType,
                                    stNvCtrlTbl.pNvData,
                                    stNvCtrlTbl.usNvDataLen);
    /*lint +e732*/
    /*lint +e516*/
    /*lint +e718*/
    return ulRslt;

}

/*****************************************************************************
 Function Name  : NAS_LMM_NvimRead
 Discription    : 其他模块调用本函数读一个指定的NV_Item
 Input          : ulNVItemType: NV_Item类型
                  *pData ：数据缓存指针
                  *pusDataLen：返回数据长度
 Output         : pusDataLen ：返回数据长度
 Return         : 0：数据成功读取，函数正常返回
                  1：未找到要读取的NV项
                  3：传入的缓存不够出错
                  4：系统异常，读取出错
 History:
      1.sunbing   49683      2010-7-22  modify
*****************************************************************************/
NAS_LMM_USIM_NV_RESULT  NAS_LMM_NvimRead(    NAS_LMM_ITEM_TYPE_ENUM_UINT32   ulNVItemType,
                                VOS_VOID                       *pData,
                                const VOS_UINT16                     *pusDataLen )
{
    VOS_UINT32                          ulRslt = NAS_LMM_NVIM_FAIL;
    NAS_NVIM_CTRL_TBL                   stNvCtrlTbl;
    VOS_UINT32                          i;
    NAS_LMM_NV_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;
    USIMM_GET_FILE_INFO_STRU            stGetFileInfo = {0};

    /*check input params*/
    if((NAS_NVIM_NULL_PTR == pData) ||
        (NAS_NVIM_NULL_PTR == pusDataLen) ||
        (ulNVItemType > EN_NV_ID_PS_END))
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimRead :Input Para is error");
        return NAS_LMM_NVIM_FAIL;
    }

    NAS_LMM_MEM_SET(g_aucNvimBuff, 0, sizeof(g_aucNvimBuff));

    /*Initalize NVIM ctrl tbl*/
    stNvCtrlTbl.pData                   = pData;
    stNvCtrlTbl.pNvData                 = g_aucNvimBuff;
    stNvCtrlTbl.usDataLen               = *pusDataLen;
    stNvCtrlTbl.usNvDataLen             = *pusDataLen;

    /*硬USIM下，并且NV ITEM属于USIM的范围，则从USIM中读取文件*/
    if((EN_NV_ID_USIM_BEGIN          <  ulNVItemType)
        &&(EN_NV_ID_USIM_END            >  ulNVItemType))
    {
        for(i = 0; i < g_ulNvDataMapNum; i++)
        {
            /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
            if(ulNVItemType == g_astNvDataMap[i].ulEventType)
            {
                stGetFileInfo.usEfId      = g_astNvDataMap[i].usUsimFileId;
                break;
            }
        }

        if(NAS_USIM_FILE_ID_INVALID == stGetFileInfo.usEfId)
        {
            NAS_LMM_NVIM_LOG_INFO("NAS_LMM_NvimRead :Donot find Usim file ID.");

            return NAS_LMM_NVIM_FAIL;
        }

        /*ucRecordNum默认为1，NAS目前涉及的文件是线形文件，且只有一个记录*/
        stGetFileInfo.ucRecordNum = 1;

        stGetFileInfo.enAppType = USIMM_UMTS_APP;
        /*
        ulRslt = Api_UsimGetFileReq(PS_USIM_CLIENT_MM,
                                    usEfId,
                                    ucRecordNum);
        */


        ulRslt = USIMM_GetFileReq(  PS_PID_MM,
                                    0,
                                    &stGetFileInfo);

        NAS_EMM_GET_SEND_COUNTER()++;

        /*需要返回失败，这样，调用处发现返回失败后，就把全局变量内存清空，
        而不会向里面赋值，真正的赋值要等待收到USIM的READ FILE消息后，才做。*/
        return NAS_LMM_NVIM_HARD_USIM;

    }

    /*其他情况从NV中读取信息*/
    /*lint -e718*/
    /*lint -e516*/
    /*lint -e732*/
    ulRslt = LPs_NvimItem_Read(      ulNVItemType,
                                    stNvCtrlTbl.pNvData,
                                    stNvCtrlTbl.usNvDataLen);
    /*lint +e732*/
    /*lint +e516*/
    /*lint +e718*/
    if(ulRslt != EN_NV_OK)
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimRead:PS_NVIM_Read is fail");
        return NAS_LMM_NVIM_FAIL;
    }

    /* 在g_astNvDataMap中查找对应的NV TYPE*/
    for(i = 0; i < g_ulNvDataMapNum; i++)
    {
        /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
        if(ulNVItemType == g_astNvDataMap[i].ulEventType)
        {
           pActFun            = g_astNvDataMap[i].pfDecodeActionFun;
           break;
        }
    }

    if (NAS_LMM_NULL_PTR != pActFun)
    {
        ulRslt = (*pActFun) (&stNvCtrlTbl);
        if(NAS_LMM_NVIM_OK == ulRslt)
        {
            return NAS_LMM_NVIM_OK;
        }
        else
        {
            return NAS_LMM_NVIM_FAIL;
        }
    }
    else /* 如果找不到处理函数，表示当前没有该处理函数 */
    {
        NAS_LMM_NVIM_LOG_ERR("NAS_LMM_NvimRead:Action functin is ERR");
        return NAS_LMM_NVIM_FAIL;
    }

}


VOS_UINT32  NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf( MsgBlock  * pMsg)
{
    PS_USIM_GET_FILE_CNF_STRU          *pstUsimCnf = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulRst;

    NAS_LMM_PUBM_LOG2_INFO("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf: g_ulSendMsgCounter = ,NAS_EMM_GET_USIM_FILE_TYPE=",
                                        NAS_EMM_GET_SEND_COUNTER(),
                                        NAS_EMM_GET_USIM_FILE_TYPE());

    /* 状态匹配检查,若不匹配,退出 */
    if (NAS_EMM_PLMN_CHK_STAT_INVALID(EMM_MS_NULL, EMM_SS_NULL_WAIT_READING_USIM))
    {
        NAS_LMM_EMM_PLMN_LOG_WARN("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf: STATE ERR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*收到后计数*//*对全局变量进行保护，以防出现负数*/
    if (NAS_NVIM_SEND_COUNTER_NONE == NAS_EMM_GET_SEND_COUNTER())
    {
        NAS_LMM_EMM_PLMN_LOG_WARN("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf: Receive USIM cnf when Counter is 0!!!");
        return NAS_LMM_MSG_DISCARD;
    }
    else
    {
        NAS_EMM_GET_SEND_COUNTER() --;
    }

    pstUsimCnf = (PS_USIM_GET_FILE_CNF_STRU*)pMsg;


    NAS_LMM_PUBM_LOG1_WARN("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf:===>",pstUsimCnf->ulResult);

    /*读卡失败*/
    if (VOS_OK != pstUsimCnf->ulResult)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf: Read file err !");


        if( NAS_NVIM_SEND_COUNTER_NONE == NAS_EMM_GET_SEND_COUNTER() )
        {/*可选最后一个文件读取失败，进行读NV、转状态的动作*/

            NAS_LMM_DoAfterReadOptFileOver();
        }

        return NAS_LMM_MSG_HANDLED;
    }

    /* 对文件进行处理 */
    ulRst = NAS_LMM_ProcessHardUsimCnf(pstUsimCnf);
    if(NAS_LMM_NVIM_OK != ulRst)
    {
        NAS_LMM_EMM_PLMN_LOG1_ERR("NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf : Decode USIM MSG err: ",
                                ulRst);
    }

    if( NAS_NVIM_SEND_COUNTER_NONE == NAS_EMM_GET_SEND_COUNTER())
    {   /* 读完必读或可选文件 */

        NAS_LMM_DoAfterReadOptFileOver();
    }

    /* 没有读完，则继续等待读卡响应消息 */

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_LMM_DoAfterReadOptFileOver( VOS_VOID )
{
    NAS_LMM_FSM_STATE_STRU               EmmState;
    NAS_LMM_UE_OPERATION_MODE_ENUM_UINT32   ulLteUeMode;

    NAS_LMM_UEID_STRU                    *pstUeId;


    /* 停止TI_NAS_LMM_TIMER_WAIT_USIM_CNF */
    NAS_LMM_StopStateTimer(TI_NAS_LMM_TIMER_WAIT_USIM_CNF);
    /*如果读取到的GUTI无效但是安全上下文有效，则删除本地和NV中的上下文*/
    pstUeId                             = NAS_LMM_GetEmmInfoUeidAddr();

    if((NAS_EMM_BIT_NO_SLCT == pstUeId->bitOpGuti)
        &&(EMM_CUR_SECU_NOT_EXIST!= NAS_LMM_GetSecuCurFsmCS()))
    {
        NAS_EMM_ClearCurSecuCntxt();
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);
    }

    /* Read NV Files*/
    NAS_LMM_ProcessNvData();

    /* 状态转成读必选文件状态 */
    /* V7不再区分必读可选 */
    /*NAS_EMM_GET_USIM_FILE_TYPE() = NAS_LMM_USIM_MANDATORY_FILE;*/
    /*PC REPLAY MODIFY BY LEILI END*/
    ulLteUeMode = NAS_LMM_ComputeLteUeMode( NAS_EMM_GetMsMode(),
                                          NAS_LMM_GetEmmInfoUeCenter());
    NAS_LMM_SetEmmInfoUeOperationMode(ulLteUeMode);
    /*PC REPLAY MODIFY BY LEILI END*/
    /* 启动TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER */
    NAS_LMM_StartStateTimer(TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER);

    /* 转状态 */
    EmmState.enFsmId             = NAS_LMM_PARALLEL_FSM_EMM;
    EmmState.enMainState         = EMM_MS_NULL;
    EmmState.enSubState          = EMM_SS_NULL_WAIT_MMC_START_CNF;
    EmmState.enStaTId            = TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER;
    NAS_LMM_StaTransProc(EmmState);

    /* 向MMC发送MMC_EMM_START_REQ(正常启动)消息 */
    (VOS_VOID)NAS_EMM_PLMN_SendMmcStartReq(EMMC_EMM_START_REQ_CAUSE_NORMAL_START);

    return;
}
VOS_VOID  NAS_LMM_ProcessNvData( VOS_VOID )
{
    /* 打印进入该函数， INFO_LEVEL */
    NAS_LMM_NVIM_LOG_INFO("NAS_LMM_ProcessNvData is entered.");

    /*初始化EMM全局变量ReadNV*/
    NAS_EMM_ReadEmmNvim();

    /*NAS_LMM_UpdateNvImsi(NAS_LMM_GetEmmInfoUeidImsiAddr());*/
    /*读取MMC相关NV*/
    /*NAS_EMMC_ReadMmcNvim();*/

    return;
}
VOS_UINT32  NAS_LMM_ProcessHardUsimCnf( PS_USIM_GET_FILE_CNF_STRU* pstUsimFileCnf)
{
    PS_USIM_GET_FILE_CNF_STRU           *pstUsimCnf = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulRslt = NAS_LMM_MSG_HANDLED;
    NAS_NVIM_CTRL_TBL                   stNvCtrlTbl;
    VOS_UINT32                          i;
    NAS_LMM_NV_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;

    pstUsimCnf = pstUsimFileCnf;

    /*Initalize NVIM ctrl tbl*/
    stNvCtrlTbl.pNvData                 = pstUsimCnf->aucEf;
    stNvCtrlTbl.usNvDataLen             = pstUsimCnf->usEfLen;

    NAS_LMM_PUBM_LOG2_INFO("NAS_LMM_ProcessHardUsimCnf:pstUsimCnf->usEfId=,pstUsimCnf->ucEfLen",
                                        pstUsimCnf->usEfId,
                                        pstUsimCnf->usEfLen);

    /* 在g_astNvDataMap中查找对应的NV TYPE*/
    for(i = 0; i < g_ulNvDataMapNum; i++)
    {
        /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
        if(pstUsimCnf->usEfId == g_astNvDataMap[i].usUsimFileId)
        {
           pActFun            = g_astNvDataMap[i].pfDecodeActionFun;
           break;
        }
    }

    if (NAS_LMM_NULL_PTR != pActFun)
    {
        ulRslt = (*pActFun) (&stNvCtrlTbl);
        return  ulRslt;
    }
    else
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
        NAS_LMM_PUBM_LOG1_ERR("NAS_LMM_NvimRead: no fun deal with UsimFileId =  ",
                                        pstUsimCnf->usEfId);
        return  NAS_LMM_NVIM_FAIL;
    }

}


/*****************************************************************************
 Function Name   : NAS_LMM_ReadMmcUsimFile
 Description     : 部分USIM文件是MMC读取的，LMM从MMC那里读到LMM中来
                   和V1R1比较，这些文件在V7中不再读:
                        6F62    MMC_READ_HPLMN_SEL_FILE_ID
                        6F7B    MMC_READ_FORBIDDEN_PLMN_LIST_FILE_ID
                        6F60    MMC_READ_UPLMN_SEL_FILE_ID
                        6F61    MMC_READ_OPLMN_SEL_FILE_ID
                        6F31    MMC_READ_HPLMN_PERI_FILE_ID
                        6FAD    MMC_READ_MNC_LENGTH_FILE_ID
                        6FDC    Last RPLMN Selection Indication

 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-2-23  Draft Enact

*****************************************************************************/
NAS_LMM_USIM_READ_RST_ENUM_UINT32  NAS_LMM_ReadMmcUsimFile(VOS_VOID)
{


    VOS_UINT32                          ulRst;
    PS_USIM_GET_FILE_CNF_STRU          *pstUsimCnfMsg;

    pstUsimCnfMsg = (PS_USIM_GET_FILE_CNF_STRU *)(g_aucNvimBuff);

    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* 6F07    MMC_READ_IMSI_FILE_ID 读USIM卡提供的接口函数 */
    ulRst = NAS_EMM_GetCardIMSI((VOS_UINT8 *)(pstUsimCnfMsg->aucEf));
    /*PC REPLAY MODIFY BY LEILI END*/
    if(USIMM_API_SUCCESS != ulRst)
    {
        NAS_LMM_EMM_PLMN_LOG1_ERR("NAS_LMM_ReadMmcUsimFile : read IMSI err: ",
                                  ulRst);
        return  NAS_LMM_USIM_READ_HARD_IMSI_ERR;
    }
    else
    {
        /* 打印从USIM读出的原始码流 */
        NAS_LMM_PUBM_LOG_NORM("\n");
        NAS_LMM_PUBM_LOG_NORM("====== USIMM_GetCardIMSI: IMSI:  ======");
        NAS_COMM_PrintArray(             NAS_COMM_GET_MM_PRINT_BUF(),
                                         pstUsimCnfMsg->aucEf,
                                         NAS_LMM_NVIM_IMSI_FILE_LEN);
        NAS_LMM_PUBM_LOG_NORM("\n");

        /* 准备一下 NAS_LMM_ProcessHardUsimCnf 的入口参数 */
        pstUsimCnfMsg->usEfId   = NAS_USIM_FILE_ID_IMSI;
        pstUsimCnfMsg->usEfLen  = NAS_LMM_NVIM_IMSI_FILE_LEN;
        pstUsimCnfMsg->ucRecordNum = 0;
        pstUsimCnfMsg->ucTotalNum  = 0;

        /* 解码 */
        ulRst = NAS_LMM_ProcessHardUsimCnf(pstUsimCnfMsg);
        if(NAS_LMM_NVIM_OK != ulRst)
        {
            NAS_LMM_EMM_PLMN_LOG1_ERR("NAS_LMM_ReadMmcUsimFile : Decode IMSI err: ", ulRst);
            return  NAS_LMM_USIM_READ_HARD_IMSI_ERR;
        }
    }

    return  NAS_LMM_USIM_READ_SUCC;


    /* 不再读取 :   6F38    MMC_READ_UST_FILE_ID */
}


/*****************************************************************************
 Function Name   : NAS_LMM_ReadLteUsimFile
 Description     : 读取LTE自己的个文件:
                    6FE3    NAS_USIM_FILE_ID_EPS_LOC_INFO
                    6FE4    NAS_USIM_FILE_ID_SEC_CONTEXT
                    安全上下文文件和GUTI应该是同时存在的，有GUTI就应该有安全，有
                    安全，就应该有GUTI;

 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HanLufeng 41410      2011-2-23  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_LMM_ReadLteUsimFile( VOS_VOID )
{

    VOS_UINT32 ulRslt;
    VOS_UINT32 ulReadHardUsimFileNum = NAS_LMM_SERTAB_SUPPORT_NONE;

    /*6F78    NAS_USIM_FILE_ID_AC_CLASS */
    ulRslt = NAS_EMM_ReadAccClassMask();
    if (NAS_LMM_ERR_CODE_HARD_USIM == ulRslt)
    {   /* 返回值  意味着读硬SUIM卡 */

        ulReadHardUsimFileNum++;
    }


    /*6FE3 安全上下文和 PS_LOC*/
    ulRslt = NAS_LMM_ReadUsimEmmInfoFile();
    if(NAS_LMM_ERR_CODE_HARD_USIM == ulRslt)
    {   /* 返回值 NAS_LMM_SIM_SRV_AVAILABLE 意味着读硬SUIM卡 */

        ulReadHardUsimFileNum++;
        ulReadHardUsimFileNum++;
    }

    
    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_ReadUsimOptFile: ulReadHardUsimFileNum = ",
                                        ulReadHardUsimFileNum);
    

    return;

}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif


