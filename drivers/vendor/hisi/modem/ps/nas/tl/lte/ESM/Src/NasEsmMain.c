

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmMain.h"
#include    "NasEsmInclude.h"
#include    "NasEsmTest.h"
#include    "LPsOm.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMMAIN_C
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
NAS_ESM_ENTITY_STRU      g_stNasEsmEntity;                        /*ESM 实体定义*/
NAS_ESM_ENTITY_STRU      *pg_stNasEsmEntity = &g_stNasEsmEntity;

OM_ESM_STATE_INFO_STRU   g_stEsmOmInfo[APP_ESM_MAX_EPSB_NUM];

extern VOS_VOID  NAS_ERABM_TaskEntry( MsgBlock* pMsg );
extern VOS_UINT32 NAS_ERABM_PidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_UINT32 NAS_ETC_PidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_VOID  NAS_ETC_TaskEntry( MsgBlock* pMsg );
extern VOS_UINT32 IP_PidInit ( enum VOS_INIT_PHASE_DEFINE ip );
extern VOS_VOID  IP_TaskEntry
(
    MsgBlock                           *pstMsg
);
/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_VOID  NAS_ESM_TaskEntry(MsgBlock * pMsg)
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU          *pHeader = VOS_NULL_PTR;

    /* 判断入口参数是否合法*/
    if( VOS_NULL_PTR == pMsg)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_TaskEntry:ERROR: No Msg!");
        return;
    }

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pMsg;

    NAS_ESM_PrintEsmRevMsg(pHeader,NAS_COMM_GET_ESM_PRINT_BUF());

    switch(pHeader->ulSenderPid)
    {
        /*如果是EMM消息，调用EMM处理函数*/
        case PS_PID_MM:
            NAS_ESM_EmmMsgDistr(pMsg);
            break;

        /*如果是RABM消息，调用RABM处理函数*/
        case PS_PID_RABM:
            NAS_ESM_RabmMsgDistr(pMsg);
            break;

        /*如果是APP消息，调用APP处理函数*/
        case PS_PID_APP:
            NAS_ESM_AppMsgDistr(pMsg);
            break;
        case WUEPS_PID_TAF:
            NAS_ESM_SmMsgDistr(pMsg);
            break;

        case MSP_PID_DIAG_APP_AGENT:
            (VOS_VOID)LTE_MsgHook((VOS_VOID*)pMsg);
            NAS_ESM_OmMsgDistr(pMsg);
            break;

        /*如果是Timer消息，调用Timer溢出处理函数*/
        case VOS_PID_TIMER:
            NAS_ESM_TimerMsgDistr(pMsg);
            break;
        /* xiongxianghui00253310 add fot errlog 20131212 begin */
        /*如果是OM消息，调用OM处理函数*/
        #if (FEATURE_PTM == FEATURE_ON)
        case ACPU_PID_OM:
            NAS_ESM_OmMsgDistrForAcpuPidOm(pMsg);
            break;
        #endif
        /* xiongxianghui00253310 add fot errlog 20131212 end */
        /*发送对象错误，报警*/
        default:
            NAS_ESM_WARN_LOG("NAS_ESM_TaskEntry:NORM: Error SenderPid!");
            break;
    }
    if (OM_ESM_REPORT_INFO_OPEN == g_NasEsmOmInfoIndFlag)
    {
        if (NAS_ESM_FAILURE == NAS_ESM_CompareEsmInfo())
        {
            /* 通过DIAG上报 */
            #if (VOS_WIN32 != VOS_OS_VER)
            (VOS_VOID)DIAG_ReportCommand(DIAG_CMD_ESM_APP_ESM_INFO_IND, (sizeof(OM_ESM_STATE_INFO_STRU))*OM_ESM_MAX_EPSB_NUM, (VOS_VOID*)g_stEsmOmInfo);
            #endif

        }
    }

    NAS_ESM_DtJudgeifNeedRpt();

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_OmInfoIndProc();
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */
}

#if 0

VOS_VOID NAS_ESM_ReadInitInfoFromNv()
{
    VOS_UINT32                          ulCid               = NAS_ESM_NULL;

    NAS_ESM_INFO_LOG("NAS_ESM_ReadInitInfoFromNv is entered.");

    /* 从NV中读取数据流的静态信息 */
    for(ulCid = 0; ulCid < NAS_ESM_MAX_EPSB_NUM; ulCid++)
    {
        NAS_ESM_ReadSdfParaFromNV(ulCid);
    }

    /* 从NV中读取承载管理方式 */
    NAS_ESM_ReadPdpManInfoFromNv();
}
#endif


/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ESM_InitAttachBearerReestInfo()
{
    VOS_UINT32 ulResult;
    NAS_ESM_ENTITY_STRU *pEsmEntityInfo = NAS_ESM_Entity();
    LNAS_ESM_NV_ATTACH_BEARER_REEST_STRU stTmpAttachBearerNvData = {0};
    /*lint -e718*/
    /*lint -e732*/
    /*lint -e516*/
    ulResult = LPs_NvimItem_Read((EN_NV_ID_ATTACH_BEARER_RE_ESTABLISH),\
                            (VOS_VOID *)(&stTmpAttachBearerNvData),\
                            sizeof(LNAS_ESM_NV_ATTACH_BEARER_REEST_STRU));
    /*lint +e516*/
    /*lint +e732*/
    /*lint +e718*/
    /* 判断读取结果 */
    if (ulResult != ERR_MSP_SUCCESS)
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_AttachBearerReestInfoInit:ERROR: Cannot Read from NV .");

        stTmpAttachBearerNvData.bitOpAttachBearerReest = NAS_ESM_OP_FALSE;
        stTmpAttachBearerNvData.ulReestTimeLen = 0;
    }

    /*  判断读取参数的有效性 */
    if ((NAS_ESM_OP_TRUE == stTmpAttachBearerNvData.bitOpAttachBearerReest) &&
        (stTmpAttachBearerNvData.ulReestTimeLen > TI_NAS_ESM_ATTACH_BEARER_REEST_TIMER_LEN))
    {
        NAS_ESM_ERR_LOG("NAS_ESM_AttachBearerReestInfoInit:Timer is too long, use default value");
        stTmpAttachBearerNvData.ulReestTimeLen = TI_NAS_ESM_ATTACH_BEARER_REEST_TIMER_LEN;
    }

    /* 拷贝有效数据到ESM Entity */
    pEsmEntityInfo->stAttachBearerReestInfo.stNvData.bitOpAttachBearerReest= stTmpAttachBearerNvData.bitOpAttachBearerReest;
    pEsmEntityInfo->stAttachBearerReestInfo.stNvData.bitOpRsv = 0;
    pEsmEntityInfo->stAttachBearerReestInfo.stNvData.ulReestTimeLen =
                                stTmpAttachBearerNvData.ulReestTimeLen * 1000;

    pEsmEntityInfo->stAttachBearerReestInfo.ulEpsbId = NAS_ESM_UNASSIGNED_EPSB_ID;

    NAS_ESM_MEM_SET(&(pEsmEntityInfo->stAttachBearerReestInfo.stTimerInfo), 0, sizeof(NAS_ESM_TIMER_STRU));
    pEsmEntityInfo->stAttachBearerReestInfo.stTimerInfo.enPara = TI_NAS_ESM_ATTACH_BEARER_REEST;

    NAS_ESM_INFO_LOG2("NAS_ESM_AttachBearerReestInfoInit: Param ",
                      pEsmEntityInfo->stAttachBearerReestInfo.stNvData.bitOpAttachBearerReest,
                      pEsmEntityInfo->stAttachBearerReestInfo.stNvData.ulReestTimeLen);
}


VOS_VOID NAS_ESM_Init(VOS_VOID)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_SM                           START INIT...");

    /*初始化SM实体*/
    NAS_ESM_MEM_SET((VOS_VOID*)NAS_ESM_Entity(),NAS_ESM_NULL,sizeof(NAS_ESM_ENTITY_STRU));

    /*设置EMM的状态为未注册*/
    NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_DETACHED);

    /*设置L MODE的状态为SUSPENDED*/
    NAS_ESM_SetLModeStatus(NAS_ESM_L_MODE_STATUS_SUSPENDED);
    /*设置已激活的承载数目为零*/
    NAS_ESM_SetCurMaxOpIdValue(NAS_ESM_MIN_OPID_VALUE);

#ifdef PS_ITT_PC_TEST_NAS
    /*NAS ST场景下，由于现有用例在注册时使用了PTI值为2，所以PTI初始化为1，调用PTI分配函数，会从2开始使用*/
    NAS_ESM_SetCurMaxPTIValue(NAS_ESM_PTI_MIN_VALUE);
#else
    /*PTI初始化为0，注册时，调用PTI分配函数，会从1开始使用*/
    /*lint -e778*/
    NAS_ESM_SetCurMaxPTIValue(NAS_ESM_PTI_MIN_VALUE-1);
    /*lint +e778*/
#endif

    NAS_ESM_SetCurMaxGwAuthIdVaule(NAS_ESM_GWAUTHID_MIN_VALUE);

    /*初始化承载参数*/
    NAS_ESM_BearerCntxtParaInit();

    /*初始化承载管理方式*/
    NAS_ESM_BearerManageInfoInit(NAS_ESM_GetBearerManageInfoAddr());

    /*初始化状态表信息*/
    for( ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++ )
    {
        NAS_ESM_RelStateTblResource(ulCnt);
    }

    /*初始化承载信息*/
    for( ulCnt = NAS_ESM_MIN_EPSB_ID; ulCnt <= NAS_ESM_MAX_EPSB_ID; ulCnt++ )
    {
        NAS_ESM_InitEpsbCntxtInfo(ulCnt);

        NAS_ESM_MEM_SET(&(g_stEsmOmInfo[ulCnt - NAS_ESM_MIN_EPSB_ID]),
                        NAS_ESM_NULL,
                        sizeof(OM_ESM_STATE_INFO_STRU));
    }

    /* 初始化ESM缓存区 */
    NAS_COMM_InitBuff(NAS_COMM_BUFF_TYPE_ESM);

    /* 初始化注册承载相关内容 */
    NAS_ESM_InitAttachBearerReestInfo();

    /* ESM软调初始化 */
    NAS_ESM_DebugInit();

    /* 初始化延迟释放非紧急PDN连接定时器 */
    NAS_ESM_Entity()->stDeferRelNonEmcTimerInfo.enPara = TI_NAS_ESM_REL_NON_EMC_BEARER;

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_ESM_FtmInfoInit();
    NAS_ESM_ErrlogInfoInit();
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */
    
}
VOS_VOID NAS_ESM_ClearEsmResource( VOS_VOID )
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_ClearEsmResource is entered.");

    /*根据消息记录通知APP*/
    NAS_ESM_ClearStateTable(APP_ERR_SM_DETACHED, PS_FALSE);

    /* 设置EMM的状态为未注册 */
    NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_DETACHED);

    /* 如果支持双APN，则停止对应定时器，并设置注册承载无效 */
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        NAS_ESM_TimerStop(0, TI_NAS_ESM_ATTACH_BEARER_REEST);
        NAS_ESM_SetAttachBearerId(NAS_ESM_UNASSIGNED_EPSB_ID);
    }

    /* 设置已激活的承载数目为零 */
    NAS_ESM_SetCurMaxOpIdValue(NAS_ESM_MIN_OPID_VALUE);

#ifdef PS_ITT_PC_TEST_NAS
    /*NAS ST场景下，由于现有用例在注册时使用了PTI值为2，所以PTI初始化为1，调用PTI分配函数，会从2开始使用*/
    NAS_ESM_SetCurMaxPTIValue(NAS_ESM_PTI_MIN_VALUE);
#else
    /*PTI初始化为0，注册时，调用PTI分配函数，会从1开始使用*/
    /*lint -e778*/
    NAS_ESM_SetCurMaxPTIValue(NAS_ESM_PTI_MIN_VALUE-1);
    /*lint +e778*/

    /* CID0在PS融合后比较特殊，在ESM维护的内容与NV的内容会存在不一致, 需将CID0初始化为专有类型 */
    NAS_ESM_ClearCid0StaticInfo();
#endif

    NAS_ESM_SetCurMaxGwAuthIdVaule(NAS_ESM_GWAUTHID_MIN_VALUE);


    /* 初始化CID 21~31 关联静态信息 */
    NAS_ESM_ClearNwCtrlSdfPara();
    /* 通知APP和SM 激活承载被释放，初始化承载信息 */
    for( ulCnt = NAS_ESM_MIN_EPSB_ID; ulCnt <= NAS_ESM_MAX_EPSB_ID; ulCnt++ )
    {
        if (NAS_ESM_BEARER_STATE_ACTIVE != NAS_ESM_GetBearCntxtState(ulCnt))
        {
            /* 初始化承载信息 */
            NAS_ESM_InitEpsbCntxtInfo(ulCnt);
            continue;
        }

        #ifndef PS_ITT_PC_TEST_NAS
        /* 通知APP 激活承载被释放 */
        NAS_ESM_InformAppBearerRel(ulCnt);

        /* 通知SM 激活承载被释放 */
        NAS_ESM_SndSmEspBearerDeactIndMsg(ulCnt);
        #endif

        /* 初始化承载信息 */
        NAS_ESM_InitEpsbCntxtInfo(ulCnt);
    }

    /* 初始化业务流的动态信息 */
    for( ulCnt = 0; ulCnt < NAS_ESM_MAX_CID_NUM; ulCnt++ )
    {
        NAS_ESM_MEM_SET(NAS_ESM_GetSdfCntxtInfo(ulCnt),
                        NAS_ESM_NULL,
                        sizeof(NAS_ESM_SDF_CNTXT_INFO_STRU));
    }

    /* 初始化PDN连接的个数 */
    NAS_ESM_SetCurPdnNum(NAS_ESM_NULL);

    /* 初始化发送空口消息的结构体 */
    NAS_ESM_ClearEsmSndNwMsgAddr();

    /* 清除ESM缓存区 */
    NAS_ESM_ClearEsmBuff();

}


VOS_VOID NAS_ESM_InitEpsbCntxtInfo(VOS_UINT32 ulEpsbId)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbInfo         = VOS_NULL_PTR;

    pstEpsbInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    NAS_ESM_MEM_SET(pstEpsbInfo,
                    NAS_ESM_NULL,
                    sizeof(NAS_ESM_EPSB_CNTXT_INFO_STRU));

    pstEpsbInfo->bitOpCId             = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpApn             = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpPco             = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpPdnAddr         = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpEpsQos          = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpEsmCause        = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpApnAmbr         = NAS_ESM_OP_FALSE;
    pstEpsbInfo->bitOpGateWayAddrInfo = NAS_ESM_OP_FALSE;
    pstEpsbInfo->ulEpsbId             = ulEpsbId;
    pstEpsbInfo->enBearerCntxtType    = NAS_ESM_BEARER_TYPE_DEFAULT;
    pstEpsbInfo->enBearerCntxtState   = NAS_ESM_BEARER_STATE_INACTIVE;
    pstEpsbInfo->ulLinkedEpsbId       = NAS_ESM_UNASSIGNED_EPSB_ID;
}


VOS_UINT32 NAS_ESM_PidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                {
                    NAS_ESM_Init();
                }
                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }

    return VOS_OK;
}


/*added by hujianbo for PTT*/
VOS_UINT32 NAS_PTT_PidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                {
                    vos_printf("NAS_PTT_PidInit Pid:%d\n", PS_PID_PTT);
                }
                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }

    return VOS_OK;
}

VOS_VOID  NAS_PTT_TaskEntry( MsgBlock* pMsg )
{
    vos_printf("NAS_PTT_TaskEntry:PTT Rcv Error Msg %x!", pMsg);

    return;
}

VOS_UINT32 CM_FidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32                              ulRet;

    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                #if (VOS_OS_VER != VOS_WIN32)
                ulRet = VOS_RegisterPIDInfo(PS_PID_PTT,
                                            NAS_PTT_PidInit,
                                            NAS_PTT_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }
                #endif

                ulRet = VOS_RegisterPIDInfo(PS_PID_ESM,
                                            NAS_ESM_PidInit,
                                            NAS_ESM_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(PS_PID_RABM,
                                            NAS_ERABM_PidInit,
                                            NAS_ERABM_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(PS_PID_TC,
                                            NAS_ETC_PidInit,
                                            NAS_ETC_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(PS_PID_IP,
                                            IP_PidInit,
                                            IP_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterMsgTaskPrio(PS_FID_CM, VOS_PRIORITY_M5);
                if( PS_SUCC != ulRet )
                {
                    return ulRet;
                }

                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;
    }

    return VOS_OK;
}
VOS_VOID LNAS_LPSOM_GetIpAddrInfo(APP_LPS_CAMPED_CELL_INFO_STRU *pstCampedCellInfo)
{
    VOS_UINT32 i = 0;

    for (i = 0; i < NAS_ESM_MAX_EPSB_NUM; i++)
    {
        pstCampedCellInfo->astEpsbCntxtInfo[i].ulEpsbId = pg_stNasEsmEntity->astEpsbCntxtInfo[i].ulEpsbId;
        NAS_ESM_MEM_CPY(&(pstCampedCellInfo->astEpsbCntxtInfo[i].stPdnAddrInfo),
                            &(pg_stNasEsmEntity->astEpsbCntxtInfo[i].stPdnAddrInfo),
                            sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));
    }

    return;
}


VOS_VOID NAS_ESM_ClearEsmResoureWhenEstingEmcPdn(VOS_VOID)
{
    VOS_UINT32 ulCnt = 0;

    NAS_ESM_INFO_LOG("NAS_ESM_ClearEsmResoureWhenEstingEmcPdn: enter");


    /*设置ESM状态为正在注册*/
    NAS_ESM_SetEmmStatus(NAS_ESM_PS_REGISTER_STATUS_ATTACHING);

    /* 如果支持双APN，则停止对应定时器，并设置注册承载无效 */
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        NAS_ESM_TimerStop(0, TI_NAS_ESM_ATTACH_BEARER_REEST);
        NAS_ESM_SetAttachBearerId(NAS_ESM_UNASSIGNED_EPSB_ID);
    }

    /* 本地去激活除紧急之外的所有承载 */
    for( ulCnt = NAS_ESM_MIN_EPSB_ID; ulCnt <= NAS_ESM_MAX_EPSB_ID; ulCnt++ )
    {
        /*
         * 如果是激活的非紧急默认承载，则去激活；
         * 如果是非激活的默认承载或紧急的默认承载，不需要去激活，跳过
         * 如果是专有承载，则在NAS_ESM_DeactBearerAndInformApp中进行去激活
         */
        if ((NAS_ESM_BEARER_TYPE_DEFAULT == NAS_ESM_GetBearCntxtType(ulCnt)) &&
            (NAS_ESM_BEARER_STATE_ACTIVE == NAS_ESM_GetBearCntxtState(ulCnt)))
        {
            NAS_ESM_DeactBearerAndInformApp(ulCnt);
        }
    }

    /* 初始化PDN连接的个数 */
    NAS_ESM_SetCurPdnNum(NAS_ESM_NULL);

    /* 通知RABM清除资源 */
    NAS_ESM_SndEsmRabmRelIndMsg();

    /*清除动态表，紧急相关除外*/
    NAS_ESM_ClearStateTable(APP_ERR_SM_NON_EMERGENCY_NOT_ALLOWED, PS_TRUE);

    /* 初始化发送空口消息的结构体 */
    NAS_ESM_ClearEsmSndNwMsgAddr();

    /* 清除ESM缓存区 */
    NAS_ESM_ClearEsmBuff();
}
/*lint +e961*/
/*lint +e960*/




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of NasEsmMain.c */

