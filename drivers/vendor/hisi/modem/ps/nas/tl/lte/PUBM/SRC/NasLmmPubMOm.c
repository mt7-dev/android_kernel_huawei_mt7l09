


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasLmmPubMInclude.h"
/*#include    "IpDhcpv4Server.h"*/
#include    "NasERabmIpFilter.h"
/* lihong00150010 ims begin */
#include    "NasEmmSecuOm.h"
#include    "ImsaNvInterface.h"
/* lihong00150010 ims end */
#include    "SysNvId.h"
#include    "omnvinterface.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASMMPUBMOM_C
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
NAS_EMM_DRX_CYCLE_LEN_CHANGE_ENUM_UINT32    g_ulDrxCycleLenChange
                                            = NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED;
/*
static APP_MM_REPORT_MODE_ENUM_UINT32      g_ulMmInfoRptFlag = APP_MM_RM_AUTO;
static APP_EMM_INFO_STRU                   g_stAppEmmInfo = {0};
*/


static NAS_EMM_PROTOCOL_STATE_MAP_STRU g_astProtocolStateMap[] =
{
    /* 空状态 */
    { EMM_MS_NULL               ,     MM_SS_BUTT,      \
      APP_EMM_MS_NULL           ,     APP_EMM_SS_NULL                       },

    /* 去注册状态 */
    { EMM_MS_DEREG              ,     EMM_SS_DEREG_NORMAL_SERVICE,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_NORMAL_SERVICE       },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_LIMITED_SERVICE,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_LIMITED_SERVICE      },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_ATTEMPT_TO_ATTACH    },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_PLMN_SEARCH,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_PLMN_SEARCH          },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_NO_IMSI,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_NO_IMSI              },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_ATTACH_NEEDED,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_ATTACH_NEEDED        },

    { EMM_MS_DEREG              ,     EMM_SS_DEREG_NO_CELL_AVAILABLE,\
      APP_EMM_MS_DEREG          ,     APP_EMM_SS_DEREG_NO_CELL_AVAILABLE    },



    /* 注册状态 */
    { EMM_MS_REG                ,     EMM_SS_REG_NORMAL_SERVICE,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_NORMAL_SERVICE         },

    { EMM_MS_REG                ,     EMM_SS_REG_ATTEMPTING_TO_UPDATE,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_ATTEMPTING_TO_UPDATE   },

    { EMM_MS_REG                ,     EMM_SS_REG_LIMITED_SERVICE,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_LIMITED_SERVICE        },

    { EMM_MS_REG                ,     EMM_SS_REG_PLMN_SEARCH,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_PLMN_SEARCH            },

    { EMM_MS_REG                ,     EMM_SS_REG_WAIT_ACCESS_GRANT_IND,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_WAIT_ACCESS_GRANT_IND},

    { EMM_MS_REG                ,     EMM_SS_REG_NO_CELL_AVAILABLE,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_NO_CELL_AVAILABLE      },

    { EMM_MS_REG                ,     EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_ATTEMPT_TO_UPDATE_MM      },

    { EMM_MS_REG                ,     EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF,\
      APP_EMM_MS_REG            ,     APP_EMM_SS_REG_IMSI_DETACH_WAIT_CN_DETACH_CNF      },



    /* 注册过程中 */
    { EMM_MS_REG_INIT           ,     MM_SS_BUTT,      \
      APP_EMM_MS_REG_INIT       ,     APP_EMM_SS_NULL                       },

    /* TAU过程中  */
    { EMM_MS_TAU_INIT           ,     MM_SS_BUTT,      \
      APP_EMM_MS_TAU_INIT       ,     APP_EMM_SS_NULL                       },

    /* SERVICE过程中 */
    { EMM_MS_SER_INIT           ,     MM_SS_BUTT,      \
      APP_EMM_MS_SER_INIT       ,     APP_EMM_SS_NULL                       },

    /* 去注册过程中 */
    { EMM_MS_DEREG_INIT         ,     MM_SS_BUTT,      \
      APP_EMM_MS_DEREG_INIT     ,     APP_EMM_SS_NULL                       }
};

static VOS_UINT32   g_ulProtocolStateMapNum
        = sizeof(g_astProtocolStateMap)/sizeof(NAS_EMM_PROTOCOL_STATE_MAP_STRU);

static  NAS_LMM_OM_ACT_STRU  g_astOmDataMap[] =
{
    /* APP_MM_MSG_TYPE_ENUM_UINT32      NAS_LMM_OM_ACTION_FUN */

    #if 0
    {ID_APP_MM_SET_PH_RA_MODE_REQ,      NAS_LMM_PubmDaSetRaMode},
    {ID_APP_MM_INQ_PH_RA_MODE_REQ,      NAS_LMM_PubmDaInqRaMode},
    #endif

    #if 0
    {ID_APP_MM_SET_MS_NET_CAP_REQ,      NAS_LMM_PubmDaSetMsNetCap},
    {ID_APP_MM_INQ_MS_NET_CAP_REQ,      NAS_LMM_PubmDaInqMsNetCap},
    #endif


    {ID_APP_MM_INQ_LTE_CS_REQ,         NAS_LMM_PubmDaInqLteCs},

    {ID_APP_MM_INQ_UE_MODE_REQ,         NAS_LMM_PubmDaInqUeMode},

    /* niuxiufan DT begin */
    {ID_APP_MM_INQ_LTE_GUTI_REQ,         NAS_LMM_PubmDaInqGuti},
    {ID_APP_MM_INQ_IMSI_REQ,             NAS_LMM_PubmDaInqImsi},
    {ID_APP_MM_INQ_EMM_STATE_REQ,        NAS_LMM_PubmDaInqEmmState},
    /* niuxiufan DT end */
};

static VOS_UINT32   g_astOmDataMapNum
        = sizeof(g_astOmDataMap)/sizeof(NAS_LMM_OM_ACT_STRU);


NAS_LMM_OM_MSG_ID_CHANGE_STRU g_stMmOmMsgIdStru[] =
{
    {0,NAS_EMM_CN_MT_ATTACH_ACP,    NAS_EMM_ATTACH_ACP,     EMM_OMT_KE_ATTACH_ACP},
    {0,NAS_EMM_CN_MT_ATTACH_REJ,    NAS_EMM_ATTACH_REJ,     EMM_OMT_KE_ATTACH_REJ},
    {0,NAS_EMM_CN_MT_DETACH_REQ_MT, NAS_EMM_DETACH_REQ_MT,  EMM_OMT_KE_DETACH_REQ_MT},
    {0,NAS_EMM_CN_MT_DETACH_ACP_MO, NAS_EMM_DETACH_ACP_MO,  EMM_OMT_KE_DETACH_ACP_MO},
    {0,NAS_EMM_CN_MT_TAU_ACP,       NAS_EMM_TAU_ACP,        EMM_OMT_KE_TAU_ACP},
    {0,NAS_EMM_CN_MT_TAU_REJ,       NAS_EMM_TAU_REJ,        EMM_OMT_KE_TAU_REJ},
    {0,NAS_EMM_CN_MT_SER_REJ,       NAS_EMM_SER_REJ,        EMM_OMT_KE_SER_REJ},
    {0,NAS_EMM_CN_MT_GUTI_CMD,      NAS_EMM_GUTI_CMD,       EMM_OMT_KE_GUTI_CMD},
    {0,NAS_EMM_CN_MT_AUTH_REQ,      NAS_EMM_AUTH_REQ,       EMM_OMT_KE_AUTH_REQ},
    {0,NAS_EMM_CN_MT_AUTH_RSP,      NAS_EMM_AUTH_RSP,       EMM_OMT_KE_AUTH_RSP},
    {0,NAS_EMM_CN_MT_AUTH_REJ,      NAS_EMM_AUTH_REJ,       EMM_OMT_KE_AUTH_REJ},
    {0,NAS_EMM_CN_MT_IDEN_REQ,      NAS_EMM_IDEN_REQ,       EMM_OMT_KE_IDEN_REQ},
    {0,NAS_EMM_CN_MT_SMC_CMD,       NAS_EMM_SECU_CMD,       EMM_OMT_KE_SECU_CMD},
    {0,NAS_EMM_CN_MT_SMC_CMP,       NAS_EMM_SECU_CMP,       EMM_OMT_KE_SECU_CMP},
    {0,NAS_EMM_CN_MT_EMM_STATUS,    NAS_EMM_STATUS,         EMM_OMT_KE_EMM_STATUS},
    {0,NAS_EMM_CN_MT_EMM_INFO,      NAS_EMM_INFO,           EMM_OMT_KE_EMM_INFO},
    {0,NAS_EMM_CN_MT_DOWNLINK_NAS_TRANSPORT, NAS_EMM_DOWNLINK_NAS_TRANSPORT, EMM_OMT_KE_EMM_DOWNLINK_NAS_TRANSPORT},
    {0,NAS_EMM_CN_CS_SER_NOTIFICATION, NAS_EMM_CS_SER_NOTIFICATION, EMM_OMT_KE_CS_SER_NOTIFICATION}
};
VOS_UINT32 g_ulSzieof_g_stMmOmMsgIdStru = sizeof(g_stMmOmMsgIdStru);

/*niuxiufan DT begin */
APP_MM_DT_REPORT_CTRL_STRU  gastNasDtRptCtrlTbl[NAS_DT_RPT_CTRL_MAX_ITEM_NUM] = {0};
VOS_UINT32                  g_ulNasEmmOmMsgHookFlag = 0;
/*niuxiufan DT end */

VOS_UINT32                  g_NasEmmOmInfoIndFlag = 0;

/* xiongxianghui00253310 modify for ftmerrlog begin */
EMM_FTM_INFO_MANAGE_STRU             g_astEmmFtmInfoManage;
EMM_DATABASE_INFO_STRU               g_astEmmInfo;
/* xiongxianghui00253310 modify for ftmerrlog end   */

/*lifuxin00253982 modify for error log start*/
EMM_ERRLOG_INFO_MANAGE_STRU          g_astEmmErrlogInfoManage;
NAS_LMM_CN_CAUSE_TRANS_STRU          g_astEmmErrlogErrNoMap[] =
{

    /*------------casue-----------------------------------error id---------------------------*/
    {NAS_LMM_CAUSE_ILLEGAL_UE,                  LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ILLEGAL_UE},
    {NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW,          LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW},
    {NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW, LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW},
    {NAS_LMM_CAUSE_PLMN_NOT_ALLOW,              LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_PLMN_NOT_ALLOW},
    {NAS_LMM_CAUSE_TA_NOT_ALLOW,                LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_TA_NOT_ALLOW},
    {NAS_LMM_CAUSE_ROAM_NOT_ALLOW,              LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ROAM_NOT_ALLOW},
    {NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN,  LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN},
    {NAS_LMM_CAUSE_NO_SUITABL_CELL,             LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_NO_SUITABL_CELL},
    {NAS_LMM_CAUSE_ESM_FAILURE,                 LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_ESM_FAILURE},
    {NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG, LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG},
    {NAS_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG,  LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_SEMANTICALLY_INCORRECT_MSG},
    {NAS_LMM_CAUSE_INVALID_MANDATORY_INF,       LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_INVALID_MANDATORY_INFORMATION},
    {NAS_LMM_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE,  LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_MSG_TYPE_NON_EXIST_OR_IMPLEMENT},
    {NAS_LMM_CAUSE_IE_NONEXIST_NOTIMPLEMENTED,  LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENT},
    {NAS_LMM_CAUSE_PROTOCOL_ERROR,              LNAS_OM_ERRLOG_ID_LMM_CN_CAUSE_PROTOCOL_ERROR},
};

static VOS_UINT32   g_astEmmErrlogErrNum
        = sizeof(g_astEmmErrlogErrNoMap)/sizeof(NAS_LMM_CN_CAUSE_TRANS_STRU);
/*lifuxin00253982 modify for error log end*/

APP_MM_DT_STRU                    g_stEmmDtInfo = {0};

/*****************************************************************************
  3 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
NAS_LMM_OM_ACTION_FUN  NAS_LMM_OmFindFun( VOS_UINT32          ulMsgId)
{
    VOS_UINT32                          i;
    NAS_LMM_OM_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;

    /* 在g_astNvDataMap中查找对应的NV TYPE*/
    for(i = 0; i < g_astOmDataMapNum; i++)
    {
        /* 若NV类型相同,则找到了,返回当前的NV动作函数，退出循环 */
        if(ulMsgId == g_astOmDataMap[i].ulMsgId)
        {
            pActFun            = g_astOmDataMap[i].pfFun;
            break;
        }
    }

    return  pActFun;
}


NAS_LMM_OM_ACTION_FUN  NAS_LMM_OmCommFindFun
(
    const NAS_LMM_OM_ACT_STRU           *aucActMap,
    VOS_UINT32                          ulMapElementNum,
    VOS_UINT32                          ulMsgId
)
{
    VOS_UINT32                          i;
    NAS_LMM_OM_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;

    /* 在aucActMap中查找对应的 ulMsgId */
    for(i = 0; i < ulMapElementNum; i++)
    {
        /* 若ulMsgId类型相同,则找到了,返回当前的ulMsgId动作函数，退出循环 */
        if(ulMsgId == aucActMap[i].ulMsgId)
        {
            pActFun = aucActMap[i].pfFun;
            break;
        }
    }

    return  pActFun;
}

/*****************************************************************************
 Function Name   : NAS_LMM_OmMaintain
 Description     : OM操作维护消息总入口
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.  qilili00145085  2008-10-30  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_OmMaintain( MsgBlock            *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulSrcMsgId;
    NAS_EMM_GENERAL_MSG_HEADER_STRU    *pstSrcMsg;
    NAS_LMM_OM_ACTION_FUN                pActFun = NAS_LMM_NULL_PTR;


    ulRet                               = NAS_LMM_MSG_DISCARD;
    pstSrcMsg                           = (NAS_EMM_GENERAL_MSG_HEADER_STRU*)pMsg;
    ulSrcMsgId                          = pstSrcMsg->ulMsgId;


    pActFun = NAS_LMM_OmFindFun(ulSrcMsgId);
    if (NAS_LMM_NULL_PTR != pActFun)
    {   /* 若这里找到了，就在这里执行APP的请求 */
        (*pActFun)(pMsg);
        ulRet = NAS_LMM_MSG_HANDLED;
    }
    else
    {
        /* 如果找不到处理函数，表示当前没有该处理函数 */
/*        NAS_LMM_PUBM_LOG_INFO("in NAS_LMM_OmFindFun, Action functin is NOT FOUND. ");*/

        #if 0
        /*如果是普通命令中的设置类命令*/
        if( (ulSrcMsgId > (ID_APP_MM_CMD_SET_START)) &&
            (ulSrcMsgId < (ID_APP_MM_CMD_SET_END  )))
        {
            ulRet = NAS_LMM_PubmDaSet(pMsg, ulSrcMsgId);
        }

         /*如果是普通命令中的查询类命令*/
        if( (ulSrcMsgId > (ID_APP_MM_CMD_INQ_START)) &&
            (ulSrcMsgId < (ID_APP_MM_CMD_INQ_END  )))
        {
            ulRet = NAS_LMM_PubmDaInq(pMsg, ulSrcMsgId);
        }
        #endif
    }

    return  ulRet;

}
VOS_UINT32 NAS_EMM_OmInfoIndMsgProc( MsgBlock    *pMsg )
{
    OM_EMM_INFO_REPORT_REQ_STRU           *pMmOmInfoReqMsg;
    OM_EMM_INFO_REPORT_CNF_STRU           *pMmOmInfoCnfMsg;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_OmInfoIndMsgProc: enter !!");

    pMmOmInfoReqMsg = (OM_EMM_INFO_REPORT_REQ_STRU *)pMsg;

    if (OM_EMM_INFO_REPORT_OPEN == pMmOmInfoReqMsg->enRptCtrl)
    {
        g_NasEmmOmInfoIndFlag = OM_EMM_REPORT_INFO_OPEN;
    }
    else
    {
        g_NasEmmOmInfoIndFlag = OM_EMM_REPORT_INFO_CLOSE;

    }
    /* 将保存上报信息的全局变量清除 */
    NAS_LMM_MEM_SET(&g_stEmmOmInfo, 0, sizeof(OM_EMM_STATE_INFO_STRU));

    /* 为消息分配地址 */
    pMmOmInfoCnfMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(OM_EMM_INFO_REPORT_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pMmOmInfoCnfMsg)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_OmInfoIndMsgProc: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return NAS_LMM_MSG_HANDLED;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pMmOmInfoCnfMsg, 0, sizeof(OM_EMM_INFO_REPORT_CNF_STRU));


    /* 打包消息头 */
    pMmOmInfoCnfMsg->ulSenderCpuId = pMmOmInfoReqMsg->ulReceiverCpuId;
    pMmOmInfoCnfMsg->ulSenderPid     = pMmOmInfoReqMsg->ulReceiverPid;
    pMmOmInfoCnfMsg->ulReceiverPid   = pMmOmInfoReqMsg->ulSenderPid;
    pMmOmInfoCnfMsg->ulReceiverCpuId  = pMmOmInfoReqMsg->ulSenderCpuId;
    pMmOmInfoCnfMsg->ulLength = EMM_MSG_LENTH_NO_HEADER(OM_EMM_INFO_REPORT_CNF_STRU);
    pMmOmInfoCnfMsg->usOriginalId    = pMmOmInfoReqMsg->usTerminalId;
    pMmOmInfoCnfMsg->usTerminalId    = pMmOmInfoReqMsg->usOriginalId;
    pMmOmInfoCnfMsg->ulTimeStamp     = pMmOmInfoReqMsg->ulTimeStamp;
    pMmOmInfoCnfMsg->ulSN            = pMmOmInfoReqMsg->ulSN;


    /*设置OPID*/
    pMmOmInfoCnfMsg->ulOpId      = pMmOmInfoReqMsg->ulOpId;

    /*设置消息类型*/
    pMmOmInfoCnfMsg->ulMsgId     = ID_OM_EMM_INFO_REPORT_CNF;

    pMmOmInfoCnfMsg->ulRslt      = APP_SUCCESS;

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pMmOmInfoCnfMsg);

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_LMM_CompareEmmInfo( VOS_VOID )
{
    VOS_UINT32                          ulRst = NAS_EMM_SUCC;
    EMM_MAIN_STATE_ENUM_UINT16      ulProtoclMainState;
    EMM_SUB_STATE_ENUM_UINT16       ulProtoclSubState;
    EMM_UPDATE_STATE_ENUM_UINT32    enEmmUpState;

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_CompareEmmInfo: enter !!");

    /* 由EMM当前状态获取协议状态 */
    NAS_LMM_PubmGetEmmState(&ulProtoclMainState,
                            &ulProtoclSubState,
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());
    /* 获取EU值 */
    NAS_LMM_PubmGetEmmUpdateState(&enEmmUpState);

    if ((ulProtoclMainState != g_stEmmOmInfo.enMainState)
        || (ulProtoclSubState != g_stEmmOmInfo.enSubState))
    {
        g_stEmmOmInfo.enMainState = ulProtoclMainState;
        g_stEmmOmInfo.enSubState = ulProtoclSubState;
        ulRst = NAS_EMM_FAIL;
    }



    if (enEmmUpState != g_stEmmOmInfo.enEmmUpState)
    {
        g_stEmmOmInfo.enEmmUpState = enEmmUpState;
        ulRst = NAS_EMM_FAIL;
    }

    /* 比较GUTI信息 */
    if (NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&g_stEmmOmInfo.stGuti,
                                                    NAS_LMM_GetEmmInfoUeidGutiAddr(),
                                                    sizeof(NAS_MM_GUTI_STRU)))
    {
        NAS_LMM_MEM_CPY(&g_stEmmOmInfo.stGuti,
                        NAS_LMM_GetEmmInfoUeidGutiAddr(),
                        sizeof(NAS_MM_GUTI_STRU));
        ulRst = NAS_EMM_FAIL;
    }

    /* 比较加密和完整性保护算法 */
    if ((NAS_EMM_GetSecuEca() != g_stEmmOmInfo.enAlgSecurity)
        || (NAS_EMM_GetSecuEia() != g_stEmmOmInfo.enAlgIntegrity))
    {
        g_stEmmOmInfo.enAlgSecurity = NAS_EMM_GetSecuEca();
        g_stEmmOmInfo.enAlgIntegrity = NAS_EMM_GetSecuEia();
        ulRst = NAS_EMM_FAIL;
    }

    /* 比较UE操作模式 */
    if (NAS_LMM_GetEmmInfoUeOperationMode() != g_stEmmOmInfo.enLteUeMode)
    {
        g_stEmmOmInfo.enLteUeMode = NAS_LMM_GetEmmInfoUeOperationMode();
        ulRst = NAS_EMM_FAIL;
    }

    return ulRst;
}
VOS_UINT32 NAS_LMM_DtCompareGuti(VOS_VOID)
{
    NAS_MM_GUTI_STRU                  *pstEmmCurGuti;
    VOS_UINT32                          ulRst = NAS_EMM_SUCC;
    pstEmmCurGuti                       = NAS_LMM_GetEmmInfoUeidGutiAddr();

    if(NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&g_stEmmDtInfo.stGuti,pstEmmCurGuti,
                                                 sizeof(NAS_MM_GUTI_STRU)))
    {
       ulRst = NAS_EMM_FAIL;
    }

    return ulRst;

}


VOS_UINT32 NAS_LMM_DtCompareImsi(VOS_VOID)
{
    VOS_UINT32                          ulRst = NAS_EMM_SUCC;
    NAS_OM_IMSI_INFO_STRU              stImsiInfo;
    VOS_UINT8                          ucIndex;

    /*如果当前无卡,向OM回复查询IMSI为空*/
    if (NAS_LMM_SIM_STATUS_AVAILABLE == NAS_LMM_GetSimState())
    {

        /* 获取IMSI */
        stImsiInfo.ucImsiLen = (VOS_UINT8)((g_stEmmInfo.stMmUeId.aucImsi[0])*2 - 1);

        /*最后一个字节最高4位为F,则表明IMSI为偶数个 */
        if (0xF0 == (g_stEmmInfo.stMmUeId.aucImsi[g_stEmmInfo.stMmUeId.aucImsi[0]] & 0xF0))
        {
            stImsiInfo.ucImsiLen -= 1;
        }

        for(ucIndex= 0; ucIndex< stImsiInfo.ucImsiLen; ucIndex++)
        {
            if (0 == (ucIndex+1)%2)
            {
                stImsiInfo.ucImsi[ucIndex] = 0x0F & (g_stEmmInfo.stMmUeId.aucImsi[(ucIndex+1)/2 + 1]);
            }
            else
            {
                stImsiInfo.ucImsi[ucIndex] = 0x0F & (g_stEmmInfo.stMmUeId.aucImsi[(ucIndex+1)/2 + 1] >> 4);
            }
        }
    }


    if(NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&g_stEmmDtInfo.stImsi,&stImsiInfo,
                                                 sizeof(NAS_OM_IMSI_INFO_STRU)))
    {
       ulRst = NAS_EMM_FAIL;
    }

    return ulRst;

}
VOS_UINT32 NAS_LMM_DtCompareEmmState(VOS_VOID)
{
    VOS_UINT32                          ulRst = NAS_EMM_SUCC;
    EMM_MAIN_STATE_ENUM_UINT16      ulProtoclMainState;
    EMM_SUB_STATE_ENUM_UINT16       ulProtoclSubState;
    VOS_UINT16                           usStaTId;           /* 定时器信息*/

    /* 由EMM当前状态获取协议状态 */
    NAS_LMM_PubmGetEmmState(&ulProtoclMainState,
                            &ulProtoclSubState,
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());
    /* 获取定时器信息 */
    usStaTId = (VOS_UINT16)NAS_LMM_GetEmmCurTimer();

    if ((ulProtoclMainState != g_stEmmDtInfo.stEmmState.enMainState)
        || (ulProtoclSubState != g_stEmmDtInfo.stEmmState.enSubState)
        || (usStaTId != g_stEmmDtInfo.stEmmState.usStaTId))
    {
        ulRst = NAS_EMM_FAIL;
    }

    return ulRst;

}
VOS_VOID NAS_LMM_DtJudgeifNeedRpt(VOS_VOID)
{

    /*判断是否需要上报GUTI*/
    if (NAS_DT_RPT_START== NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enRptStatus)
    {
        if(NAS_EMM_FAIL == NAS_LMM_DtCompareGuti())
        {
        #if (VOS_WIN32 != VOS_OS_VER)
           /*上报GUTI信息*/
           NAS_LMM_PubmIndGuti();
        #endif

        }
    }
     /*判断是否需要上报IMSI*/
    if (NAS_DT_RPT_START== NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enRptStatus)
    {
        if(NAS_EMM_FAIL == NAS_LMM_DtCompareImsi())
        {
        #if (VOS_WIN32 != VOS_OS_VER)
           /*上报Imsi信息*/
           NAS_LMM_PubmIndImsi();
        #endif

        }
    }

    /* 判断是否需要上报EMM状态*/
    if(NAS_DT_RPT_START== NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enRptStatus)
    {
        if(NAS_EMM_FAIL == NAS_LMM_DtCompareEmmState())
        {
        #if(VOS_WIN32 != VOS_OS_VER)
           /*上报EMM状态*/
           NAS_LMM_PubmIndEmmState();
        #endif
        }
    }
    return;
}



VOS_VOID  NAS_LMM_SetAppParaValue
(
    NAS_LMM_APP_PARA_SET_RESLT_STRU      *pstParaInfo,
    const NAS_LMM_APP_PARA_SET_HEAD_STRU       *pstAppMsg
)
{
    pstParaInfo->ulOpId                 = pstAppMsg->ulOpId;

    /*交叉MID参数*/
    pstParaInfo->usOriginalId = pstAppMsg->usOriginalId;
    pstParaInfo->usTerminalId = pstAppMsg->usTerminalId;
    pstParaInfo->ulTimeStamp  = pstAppMsg->ulTimeStamp;

    /*设置SN参数*/
    pstParaInfo->ulSN = pstAppMsg->ulSN;
    return;
}


VOS_VOID NAS_EMM_SendIntraTauReq
(
    APP_MM_MSG_TYPE_ENUM_UINT32         ulOmMmMsgType,
    NAS_LMM_INTRA_TAU_TYPE_ENUM_UINT32  enIntraTauType
)
{
    VOS_UINT32                          ulMsgLenthNoHeader;
    NAS_LMM_INTRA_TAU_REQ_STRU           *pEmmIntraTauReq;
/*    VOS_UINT32                          ulRst;*/

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_SendIntraTauReq is entered");

     /* 计算EMM要发给OM的消息长度, 不包含VOS头 */
    ulMsgLenthNoHeader = EMM_MSG_LENTH_NO_HEADER(NAS_LMM_INTRA_TAU_REQ_STRU);

    /* 申请DOPRA消息 */
    /*lint -e433 -e826*/
    pEmmIntraTauReq = (VOS_VOID *)NAS_LMM_GetSendIntraMsgBuffAddr(ulMsgLenthNoHeader);
    /*lint +e433 +e826*/

    if(NAS_LMM_NULL_PTR  == pEmmIntraTauReq)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_EMM_SendIntraTauReq: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }

    /* 打包消息头 */
    NAS_EMM_COMP_INTRA_MSG_HEADER(      pEmmIntraTauReq,
                                        ulMsgLenthNoHeader);


    /* 填写消息ID */
    pEmmIntraTauReq->ulMsgId            = ID_NAS_LMM_INTRA_TAU_REQ;

    pEmmIntraTauReq->ulOmMsgType        = ulOmMmMsgType;

    pEmmIntraTauReq->enIntraTauType     = enIntraTauType;

    /* 发送消息 */
    /*
    ulRst = NAS_LMM_SendIntraMsg(        pEmmIntraTauReq);
    if(NAS_EMM_SUCC != ulRst)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_SendIntraTauReq: SendIntraMsg ERR !!");
    }
    */
    NAS_EMM_SEND_INTRA_MSG(             pEmmIntraTauReq);

    return;

}

#if 0
/*****************************************************************************
 Function Name  : NAS_LMM_MtTpSetOptNetCap
 Discription    : 透明命令下发
 Input          : pMsg
 Output         : None
 Return         : None
 History:
      1.  qilili00145085  2008-10-30  Draft Enact
*****************************************************************************/
VOS_VOID    NAS_LMM_MtTpSetOptMsNetCap(    MsgBlock    *pMsg)
{
    (VOS_VOID)(pMsg);
#if 0
    OMT_EMM_TP_OPT_MS_NET_CAP_REQ_STRU    *pEmmTpOptNetCap;
    OMT_MM_TP_CMD_CNF_STRU    *pEmmTpOptNetCapCnf;

    VOS_UINT32                          ulMsgLenthNoHeader;

    pEmmTpOptNetCap                     = (OMT_EMM_TP_OPT_MS_NET_CAP_REQ_STRU *)pMsg;

    /* 计算EMM要发给OM的消息长度, 不包含VOS头 */
    ulMsgLenthNoHeader = EMM_MSG_LENTH_NO_HEADER(OMT_MM_TP_CMD_CNF_STRU);

    /* 申请DOPRA消息 */
    pEmmTpOptNetCapCnf = (OMT_MM_TP_CMD_CNF_STRU *)NAS_LMM_ALLOC_MSG(ulMsgLenthNoHeader);
    if(NAS_LMM_NULL_PTR  == pEmmTpOptNetCapCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_MtTpSetOptMsNetCap: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }

    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER(            pEmmTpOptNetCapCnf,
                                        ulMsgLenthNoHeader);

    /*打包APP消息头*/
    EMM_COMP_MM_APP_MSG_HEADER(     pEmmTpOptNetCapCnf,
                                        pEmmTpOptNetCap);

    /* 填写消息ID */
    EMM_COMP_OMT_TP_CNF_MSGID(          pEmmTpOptNetCapCnf,
                                        ID_APP_MM_TRANSPARENT_CMD_CNF);


    /* 填写透明命令的类型 */
    pEmmTpOptNetCapCnf->ucMmTpCmdType = NAS_EMM_TP_SET_MS_NET_CAP_REQ;

     /* 设置的参数与原参数相同 */
    if(0 == NAS_LMM_MEM_CMP(             &(pEmmTpOptNetCap->stMmNetCap),
                                        &(NAS_LMM_PUBM_GLOBAL.stMsNetCap),
                                        NAS_EMM_MS_NET_CAP_MAX_SIZE))
    {
        /* 透明命令执行成功 */
        pEmmTpOptNetCapCnf->ucMmTpCmdRst
                                        = NAS_EMM_OMT_MAINTAIN_RST_SUCC;
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_MtTpSetOptNetCap: Parameter, stMsNetCap, same as before !!");
    }
    else
    {
        NAS_LMM_MEM_CPY(                 &(NAS_LMM_PUBM_GLOBAL.stMsNetCap),
                                        &(pEmmTpOptNetCap->stMmNetCap),
                                        sizeof(NAS_EMM_MS_NET_CAP_STRU));

        /* 透明命令执行成功 */
        pEmmTpOptNetCapCnf->ucMmTpCmdRst
                                        = NAS_EMM_OMT_MAINTAIN_RST_SUCC;
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MtTpSetOptNetCap: pEmmTpOptNetCapCnf is set succ.");
    }

    /* 发送消息 */
    NAS_LMM_SendOuterMsg(pEmmTpOptNetCapCnf);
#endif
    return;
}
#endif

#if 0

VOS_UINT32  NAS_LMM_PubmSetEplmn(APP_MM_SET_PLMN_REQ_STRU *pstSetPlmnReq)
{
    LNAS_LMM_NV_EPLMN_LIST_STRU         stNvEplmnList;
    VOS_VOID                            *pData;
    VOS_UINT16                          usDataLen;
    VOS_UINT32                          ulRslt;
    NAS_LMM_NETWORK_INFO_STRU            *pstNetInfo;

    pstNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();

     /*填充 NV PLMN 结构*/
    NAS_LMM_MEM_SET(&stNvEplmnList, 0, sizeof(LNAS_LMM_NV_EPLMN_LIST_STRU));

    if (APP_MM_SET_TYPE_SETTING == pstSetPlmnReq->enSetType)
    {
        if (NAS_LMM_MAX_EQU_PLMN_NUM < pstSetPlmnReq->stPlmnList.ulPlmnNum)
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmSetEplmn: set plmn num is 16 ");
            pstSetPlmnReq->stPlmnList.ulPlmnNum = NAS_LMM_MAX_EQU_PLMN_NUM;
        }
        stNvEplmnList.bitOpNvPlmn       = NAS_EMM_BIT_SLCT;
        stNvEplmnList.stPlmnList.ulPlmnNum = pstSetPlmnReq->stPlmnList.ulPlmnNum;
        NAS_LMM_MEM_CPY(                 (stNvEplmnList.stPlmnList.astPlmnId),
                                        (pstSetPlmnReq->stPlmnList.astPlmnId),
                                        sizeof(APP_PLMN_ID_STRU) * pstSetPlmnReq->stPlmnList.ulPlmnNum);

        /*更新全局变量*/
        pstNetInfo->bitOpEPlmnList      = NAS_EMM_BIT_SLCT;
        pstNetInfo->stEPlmnList.ulPlmnNum = pstSetPlmnReq->stPlmnList.ulPlmnNum;
        NAS_LMM_MEM_CPY(                 (pstNetInfo->stEPlmnList.astPlmnId),
                                        (pstSetPlmnReq->stPlmnList.astPlmnId),
                                        sizeof(APP_PLMN_ID_STRU) * pstSetPlmnReq->stPlmnList.ulPlmnNum);

    }
    else if (APP_MM_SET_TYPE_DELETE == pstSetPlmnReq->enSetType)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmSetEplmn: Delete Eplmn ");

        /*设置类型为DELETE*/
        stNvEplmnList.bitOpNvPlmn       = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                 &stNvEplmnList.stPlmnList,
                                        0,
                                        sizeof(stNvEplmnList.stPlmnList));

        /*更新全局变量*/
        pstNetInfo->bitOpEPlmnList      = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                 &(pstNetInfo->stEPlmnList),
                                        0,
                                        sizeof(pstNetInfo->stEPlmnList));
    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PubmSetEplmn: set type is err !");
        ulRslt = NAS_LMM_NVIM_FAIL;
        return ulRslt;
    }

    /*写入 NVIM*/
    pData                           = &(stNvEplmnList);
    usDataLen                       = sizeof(LNAS_LMM_NV_EPLMN_LIST_STRU);

    ulRslt = NAS_LMM_NvimWrite(EN_NV_ID_EQUIVALENTPLMN, pData, usDataLen);

    return ulRslt;
}
#endif

#if 0
VOS_VOID    NAS_LMM_PubmDaSetRaMode(       MsgBlock    *pMsg )
{
    APP_MM_SET_PH_RA_MODE_REQ_STRU     *pstEmmSetRaModeReq;
    LNAS_LMM_NV_RAMOD_STRU              stNvRaMod;
    VOS_VOID                            *pData;
    VOS_UINT16                          usDataLen;
    VOS_UINT32                          ulRslt;
    NAS_LMM_APP_PARA_SET_RESLT_STRU      stAppParaInfo;

    /* 类型转换 */
    pstEmmSetRaModeReq                  =  (VOS_VOID *)pMsg;

    /*设置回复消息的部分内容*/
    stAppParaInfo.ulMsgId   = ID_APP_MM_SET_PH_RA_MODE_CNF;
    NAS_LMM_SetAppParaValue(&stAppParaInfo,(NAS_LMM_APP_PARA_SET_HEAD_STRU *)pstEmmSetRaModeReq);

    /*填充 NV RaMod 结构*/
    NAS_LMM_MEM_SET(&stNvRaMod, 0, sizeof(LNAS_LMM_NV_RAMOD_STRU));

    if (APP_MM_SET_TYPE_SETTING == pstEmmSetRaModeReq->enSetType)
    {
        if (APP_PH_RAT_LTE_TDD < pstEmmSetRaModeReq->enAccessMode)
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmDaSetRaMode:Input value is Invalid!");
            stAppParaInfo.ulRslt            = APP_FAILURE;

            /*回复CNF消息*/
            NAS_LMM_PubmDaSendSetCnf(&stAppParaInfo);
            return;
        }

        stNvRaMod.bitOpRaMod            = NAS_EMM_BIT_SLCT;
        stNvRaMod.enRaMod               = pstEmmSetRaModeReq->enAccessMode;

        /* 更新本地参数 */
        g_stEmmInfo.bitOpRaMod          = NAS_EMM_BIT_SLCT;
        g_stEmmInfo.enRaMod             = pstEmmSetRaModeReq->enAccessMode;
    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PubmDaSetRaMode:No suppor delete or  set type is err !");
        stAppParaInfo.ulRslt            = APP_FAILURE;

        /*回复CNF消息*/
        NAS_LMM_PubmDaSendSetCnf(&stAppParaInfo);
        return;
    }

    /*写入 NVIM*/
    pData                               = &(stNvRaMod);
    usDataLen                           = sizeof(LNAS_LMM_NV_RAMOD_STRU);
    ulRslt = NAS_LMM_NvimWrite(EN_NV_ID_UE_RAMOD, pData, usDataLen);

     /*回复 CNF 消息*/
    if (NAS_LMM_NVIM_OK == ulRslt)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaSetRaMode: SUCC");
        stAppParaInfo.ulRslt            = APP_SUCCESS;

    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PubmDaSetRaMode: FAIL");
        stAppParaInfo.ulRslt            = APP_FAILURE;
    }

    /*回复CNF消息*/
    NAS_LMM_PubmDaSendSetCnf(&stAppParaInfo);
    return;
}
#endif

#if 0
VOS_VOID NAS_LMM_PubmDaSetMsNetCap(    MsgBlock    *pMsg)
{
    APP_MM_SET_MS_NET_CAP_REQ_STRU     *pstEmmSetMsNetCapReq;
    LNAS_LMM_NV_MS_NET_CAP_STRU         stNvMsNetCap;
    VOS_VOID                            *pData;
    VOS_UINT16                          usDataLen;
    VOS_UINT32                          ulRslt;
    NAS_LMM_APP_PARA_SET_RESLT_STRU      stAppParaInfo;

    /* 类型转换 */
    pstEmmSetMsNetCapReq                =  (VOS_VOID *)pMsg;

    /*设置回复消息的部分内容*/
    stAppParaInfo.ulMsgId   = ID_APP_MM_SET_MS_NET_CAP_CNF;
    NAS_LMM_SetAppParaValue(&stAppParaInfo,(NAS_LMM_APP_PARA_SET_HEAD_STRU *)pstEmmSetMsNetCapReq);

    /*填充 NV HPLMN 结构*/
    NAS_LMM_MEM_SET(&stNvMsNetCap, 0, sizeof(LNAS_LMM_NV_MS_NET_CAP_STRU));

    if (APP_MM_SET_TYPE_SETTING == pstEmmSetMsNetCapReq->enSetType)
    {
        stNvMsNetCap.bitOpMsNetCap      = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                 &(stNvMsNetCap.stMsNetCap),
                                        &(pstEmmSetMsNetCapReq->stMsNetCap),
                                        sizeof(APP_MS_NET_CAP_STRU));
    }
    else if (APP_MM_SET_TYPE_DELETE == pstEmmSetMsNetCapReq->enSetType)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmDaSetMsNetCap:Delete Ms Net Cap");
        stNvMsNetCap.bitOpMsNetCap      = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_GetEmmInfoAddr()->bitOpMsNetCap       = NAS_EMM_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                 &(stNvMsNetCap.stMsNetCap),
                                        0,
                                        sizeof(APP_MS_NET_CAP_STRU));
    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PubmDaSetMsNetCap: set type is err !");
        stAppParaInfo.ulRslt            = APP_FAILURE;

        /*回复CNF消息*/
        NAS_LMM_PubmDaSendSetCnf(&stAppParaInfo);
        return;
    }

    /*写入 NVIM*/
    pData                               = (VOS_VOID *)&(stNvMsNetCap);
    usDataLen                           = sizeof(LNAS_LMM_NV_MS_NET_CAP_STRU);
    ulRslt = NAS_LMM_NvimWrite(EN_NV_ID_MS_NET_CAPABILITY, pData, usDataLen);

    /*回复 CNF 消息*/
    if (NAS_LMM_NVIM_OK == ulRslt)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaSetMsNetCap: SUCC");
        stAppParaInfo.ulRslt            = APP_SUCCESS;
    }
    else
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PubmDaSetMsNetCap: FAIL");
        stAppParaInfo.ulRslt            = APP_FAILURE;
    }

    /*回复CNF消息*/
    NAS_LMM_PubmDaSendSetCnf(&stAppParaInfo);

    return;
}
#endif

#if 0
VOS_VOID    NAS_LMM_PubmDaInqRaMode(     MsgBlock    *pMsg )
{
    APP_MM_INQ_PH_RA_MODE_REQ_STRU     *pstEmmInqRaModReq;
    APP_MM_INQ_PH_RA_MODE_CNF_STRU     *pstEmmInqRaModCnf;
    LNAS_LMM_NV_RAMOD_STRU              stNvRaMod;
    VOS_VOID                           *pData;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmDaInqRaMode enter !!");
    pstEmmInqRaModReq                  = (VOS_VOID *)pMsg;

    /* 申请DOPRA消息 */
    pstEmmInqRaModCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_PH_RA_MODE_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstEmmInqRaModCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqRaMode: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }

    NAS_LMM_MEM_SET(pstEmmInqRaModCnf, 0, sizeof(APP_MM_INQ_PH_RA_MODE_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER(pstEmmInqRaModCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_PH_RA_MODE_CNF_STRU),
                                pstEmmInqRaModReq);

    pstEmmInqRaModCnf->ulMsgId          = ID_APP_MM_INQ_PH_RA_MODE_CNF;
    pstEmmInqRaModCnf->ulOpId           = pstEmmInqRaModReq->ulOpId;

    /*read HPLMN from NVIM*/
    pData = (VOS_VOID *)&(stNvRaMod);
    usDataLen = sizeof(LNAS_LMM_NV_RAMOD_STRU);
    ulRslt = NAS_LMM_NvimRead(EN_NV_ID_UE_RAMOD, pData, &usDataLen);

    if((ulRslt == EN_NV_OK) && (NAS_EMM_BIT_SLCT == stNvRaMod.bitOpRaMod))
    {   /* 读取成功 */
        pstEmmInqRaModCnf->enAccessMode = stNvRaMod.enRaMod;
    }
    else
    {   /* 读取失败 */
        pstEmmInqRaModCnf->enAccessMode = APP_PH_RAT_TYPE_BUTT;
    }

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstEmmInqRaModCnf);

    return;
}
#endif
#if 0

VOS_VOID    NAS_LMM_PubmDaInqMsNetCap(     MsgBlock    *pMsg )
{
    APP_MM_INQ_MS_NET_CAP_REQ_STRU     *pstAppInqMsetCapReq;
    APP_MM_INQ_MS_NET_CAP_CNF_STRU     *pstAppInqMsNetCapCnf;
    LNAS_LMM_NV_MS_NET_CAP_STRU         stNvMsNetCap;
    VOS_VOID                           *pData;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usDataLen;


    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PubmDaInqMsNetCap: enter!!");
    pstAppInqMsetCapReq                = (VOS_VOID *)pMsg;

    /* 申请DOPRA消息 */
    pstAppInqMsNetCapCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_MS_NET_CAP_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqMsNetCapCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqMsNetCap: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }

    NAS_LMM_MEM_SET(pstAppInqMsNetCapCnf, 0, sizeof(APP_MM_INQ_MS_NET_CAP_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER(pstAppInqMsNetCapCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_MS_NET_CAP_CNF_STRU),
                                pstAppInqMsetCapReq);

    pstAppInqMsNetCapCnf->ulMsgId       = ID_APP_MM_INQ_MS_NET_CAP_CNF;
    pstAppInqMsNetCapCnf->ulOpId        = pstAppInqMsetCapReq->ulOpId;

    /*read UE NET CAP from NVIM*/
    pData = (VOS_VOID *)&(stNvMsNetCap);
    usDataLen = sizeof(LNAS_LMM_NV_MS_NET_CAP_STRU);

    ulRslt = NAS_LMM_NvimRead(EN_NV_ID_MS_NET_CAPABILITY, pData, &usDataLen);
    if((EN_NV_OK == ulRslt) && (NAS_EMM_BIT_SLCT == stNvMsNetCap.bitOpMsNetCap))
    {   /* 读取成功 */
        NAS_LMM_MEM_CPY(&(pstAppInqMsNetCapCnf->stMsNetCap),
                        &(stNvMsNetCap.stMsNetCap),
                        sizeof(APP_MS_NET_CAP_STRU));
    }
    else
    {   /* 读取失败 */
        NAS_LMM_MEM_SET(&(pstAppInqMsNetCapCnf->stMsNetCap),
                         0,
                         sizeof(APP_MS_NET_CAP_STRU));
    }

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqMsNetCapCnf);

    return;
}
#endif

/* lihong00150010 ims begin */

VOS_VOID  NAS_LMM_ReadImsaNvImsCapability( VOS_UINT8 *pucSmsOverIms, VOS_UINT8 *pucSrvcc)
{
    IMS_NV_IMS_CAP_STRU             stNvImsCapability;
    VOS_UINT32                      ulRslt;
    /*lint -e718*/
    /*lint -e732*/
    /*lint -e516*/
    ulRslt = LPs_NvimItem_Read(EN_NV_ID_IMS_CAPABILITY,&stNvImsCapability,\
                 sizeof(IMS_NV_IMS_CAP_STRU));
    /*lint +e516*/
    /*lint +e732*/
    /*lint +e718*/

    if(ulRslt == VOS_OK)
    {
        /*根据NV内容设置*/
        *pucSmsOverIms = stNvImsCapability.ucSmsOnImsSupportFlag;
        *pucSrvcc = stNvImsCapability.ucSrvccOnImsSupportFlag;

    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN("IMSA_ReadImsaNvImsCapability: read NV err!");

        /*设置初始值*/
        *pucSmsOverIms = VOS_TRUE;
        *pucSrvcc = VOS_TRUE;
    }

}
/* lihong00150010 ims end */

/*leili modify for isr begin*/
/*****************************************************************************
 Function Name   : NAS_LMM_PubmDaInqLteCs
 Description     :
 Input           : MsgBlock    *pMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY        2012-02-17    Draft Enact

*****************************************************************************/
VOS_VOID  NAS_LMM_PubmDaInqLteCs(MsgBlock *pMsg)
{
    APP_MM_INQ_LTE_CS_REQ_STRU      *pstAppInqReq;
    APP_MM_INQ_LTE_CS_CNF_STRU      *pstAppInqCnf;

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqLteCs: enter");

    pstAppInqReq                       = (VOS_VOID *)pMsg;

    /* 为消息分配地址 */
    pstAppInqCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_LTE_CS_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqLteCs: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppInqCnf, 0, sizeof(APP_MM_INQ_LTE_CS_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER(pstAppInqCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_LTE_CS_CNF_STRU),
                                pstAppInqReq);

    /*设置OPID*/
    pstAppInqCnf->ulOpId      = pstAppInqReq->ulOpId;

    /*设置消息类型*/
    pstAppInqCnf->ulMsgId     = ID_APP_MM_INQ_LTE_CS_CNF;

    pstAppInqCnf->ulRslt      = APP_SUCCESS;

    if(NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaInqLteCs: REG domain  NAS_LMM_REG_DOMAIN_CS_PS!!");
        pstAppInqCnf->stLtecsInfo.ucSmsOverSGs = NAS_LMM_LTE_CS_SERVICE_SUPPORT;

        if(NAS_LMM_ADDITIONAL_UPDATE_BUTT == NAS_EMM_GetAddUpdateRslt())
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaInqLteCs: Addition Update Result not exist!!");
            pstAppInqCnf->stLtecsInfo.ucCSFB = NAS_LMM_LTE_CS_SERVICE_SUPPORT;
        }
        else
        {
            NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaInqLteCs: Addition Update Resultsexist!!");
            /* modify by jiqiang 2014.03.20 pclint begin */
            /*lint -e960*/
            if((NAS_LMM_ADDITIONAL_UPDATE_CSFB_NOT_PREFERED == NAS_EMM_GetAddUpdateRslt()) || (NAS_LMM_ADDITIONAL_UPDATE_NO_INFO == NAS_EMM_GetAddUpdateRslt()))
            /*lint +e960*/
            /* modify by jiqiang 2014.03.20 pclint end */
            {
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PubmDaInqLteCs: Addition Update Result is NO_INFO or CSFB_NOT_PREFERED!!");
                pstAppInqCnf->stLtecsInfo.ucCSFB = NAS_LMM_LTE_CS_SERVICE_SUPPORT;
            }
        }
    }
	/* lihong00150010 ims begin */
    NAS_LMM_ReadImsaNvImsCapability(&pstAppInqCnf->stLtecsInfo.ucSmsOverIMS,&pstAppInqCnf->stLtecsInfo.ucSrVcc);
	/* lihong00150010 ims end */

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqCnf);

    return;
}
/*leili modify for isr end*/


/* niuxiufan DT begin */

VOS_VOID  NAS_LMM_PubmDaInqGuti(MsgBlock *pMsg)
{
    APP_MM_INQ_LTE_GUTI_REQ_STRU      *pstAppInqReq;
    APP_MM_INQ_LTE_GUTI_CNF_STRU      *pstAppInqCnf;

    pstAppInqReq                       = (VOS_VOID *)pMsg;


    /* 根据命令判断是否停止GUTI上报 */
    if (DT_CMD_STOP == pstAppInqReq->enCmd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqGuti: START");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enRptStatus = NAS_DT_RPT_STOP;
    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqGuti: STOP");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enDtRptItem = NAS_DT_REPORT_TYPE_GUTI;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enRptStatus = NAS_DT_RPT_START;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enRptType = APP_MM_RM_AUTO;
    }

    /* 为消息分配地址 */
    pstAppInqCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_LTE_GUTI_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqGuti: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppInqCnf, 0, sizeof(APP_MM_INQ_LTE_GUTI_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_CNF_MSG_VOS_HEADER(pstAppInqCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_LTE_GUTI_CNF_STRU),
                                pstAppInqReq);

    /*设置OPID*/
    pstAppInqCnf->ulOpId      = pstAppInqReq->ulOpId;

    /*设置消息类型*/
    pstAppInqCnf->ulMsgId     = ID_APP_MM_INQ_LTE_GUTI_CNF;

    pstAppInqCnf->ulRslt      = APP_SUCCESS;

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppInqCnf);
    }
#endif


    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqCnf);

    NAS_LMM_PubmIndGuti();

    return;
}
VOS_VOID  NAS_LMM_PubmDaInqImsi(MsgBlock *pMsg)
{
    APP_MM_INQ_IMSI_REQ_STRU      *pstAppInqReq;
    APP_MM_INQ_IMSI_CNF_STRU      *pstAppInqCnf;

     pstAppInqReq                       = (VOS_VOID *)pMsg;

    /* 根据命令判断是否停止IMSI上报 */
     if (DT_CMD_STOP == pstAppInqReq->enCmd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqImsi: STOP");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enRptStatus = NAS_DT_RPT_STOP;
    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqImsi: START");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enDtRptItem = NAS_DT_REPORT_TYPE_IMSI;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enRptStatus = NAS_DT_RPT_START;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enRptType = APP_MM_RM_AUTO;
    }

    /* 为消息分配地址 */
    pstAppInqCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_IMSI_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqImsi: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppInqCnf, 0, sizeof(APP_MM_INQ_IMSI_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_CNF_MSG_VOS_HEADER(pstAppInqCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_IMSI_CNF_STRU),
                                pstAppInqReq);

    /*设置OPID*/
    pstAppInqCnf->ulOpId      = pstAppInqReq->ulOpId;

    /*设置消息类型*/
    pstAppInqCnf->ulMsgId     = ID_APP_MM_INQ_IMSI_CNF;
    pstAppInqCnf->ulRslt      = APP_SUCCESS;

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppInqCnf);
    }
#endif
    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqCnf);

    NAS_LMM_PubmIndImsi();

    return;
}
VOS_VOID  NAS_LMM_PubmDaInqEmmState(MsgBlock *pMsg)
{
    APP_MM_INQ_EMM_STATE_REQ_STRU      *pstAppInqReq;
    APP_MM_INQ_EMM_STATE_CNF_STRU      *pstAppInqCnf;

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqEmmState: enter");

    pstAppInqReq                       = (VOS_VOID *)pMsg;

    /* 根据命令判断是否停止EMM State上报 */
    if (DT_CMD_STOP == pstAppInqReq->enCmd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqEmmState: STOP");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enRptStatus = NAS_DT_RPT_STOP;
    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqEmmState: START");
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enDtRptItem = NAS_DT_REPORT_TYPE_EMM_STATE;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enRptStatus = NAS_DT_RPT_START;
        NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enRptType = APP_MM_RM_AUTO;
    }

    /* 为消息分配地址 */
    pstAppInqCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_EMM_STATE_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqEmmState: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppInqCnf, 0, sizeof(APP_MM_INQ_EMM_STATE_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_CNF_MSG_VOS_HEADER(pstAppInqCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_EMM_STATE_CNF_STRU),
                                pstAppInqReq);

    /*设置OPID*/
    pstAppInqCnf->ulOpId      = pstAppInqReq->ulOpId;

    /*设置消息类型*/
    pstAppInqCnf->ulMsgId     = ID_APP_MM_INQ_EMM_STATE_CNF;

    pstAppInqCnf->ulRslt      = APP_SUCCESS;

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppInqCnf);
    }
#endif

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqCnf);

    NAS_LMM_PubmIndEmmState();

    return;
}
VOS_VOID  NAS_LMM_PubmIndGuti(VOS_VOID)
{
    APP_MM_INQ_LTE_GUTI_IND_STRU      *pstAppGutiInd;
    NAS_MM_GUTI_STRU                  *pstEmmCurGuti;

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndGuti: enter");

    /* 根据命令判断是否停止GUTI上报 */
    if (NAS_DT_RPT_STOP == NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_GUTI)->enRptStatus)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndGuti: STOP");
        return;
    }

    /* 为消息分配地址 */
    pstAppGutiInd = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_LTE_GUTI_IND_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppGutiInd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqGuti: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppGutiInd, 0, sizeof(APP_MM_INQ_LTE_GUTI_IND_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_IND_MSG_HEADER( pstAppGutiInd,
                                 EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_LTE_GUTI_IND_STRU));

    /*打包APP消息头*/
    EMM_COMP_OMT_APP_MSG_HEADER( pstAppGutiInd);

    /*设置消息类型*/
    pstAppGutiInd->ulMsgId     = ID_APP_MM_INQ_LTE_GUTI_IND;
    pstAppGutiInd->ulOpId      = DIAG_CMD_EMM_APP_GUTI_INFO_IND;


    /* 获取GUTI */
    pstEmmCurGuti                       = NAS_LMM_GetEmmInfoUeidGutiAddr();
    NAS_LMM_MEM_CPY( &pstAppGutiInd->stGuti,
                    pstEmmCurGuti,
                    sizeof(NAS_LMM_GUTI_STRU));

    /*保存GUTI信息至全局变量*/
    NAS_LMM_MEM_CPY(&g_stEmmDtInfo.stGuti,
                        pstEmmCurGuti,
                        sizeof(NAS_MM_GUTI_STRU));

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppGutiInd);
    }
#endif
    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppGutiInd);
    return;
}
VOS_VOID  NAS_LMM_PubmIndImsi(VOS_VOID)
{
    APP_MM_INQ_IMSI_IND_STRU      *pstAppImsiInd;
    NAS_OM_IMSI_INFO_STRU          stImsiInfo;
    VOS_UINT8                      i;

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndImsi: enter");


    /* 根据命令判断是否停止IMSI上报 */
    if (NAS_DT_RPT_STOP == NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_IMSI)->enRptStatus)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndImsi: STOP");
        return;
    }

    /* 为消息分配地址 */
    pstAppImsiInd = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_IMSI_IND_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppImsiInd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndImsi: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppImsiInd, 0, sizeof(APP_MM_INQ_IMSI_IND_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_IND_MSG_HEADER( pstAppImsiInd,
                                 EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_IMSI_IND_STRU));

    /*打包APP消息头*/
    EMM_COMP_OMT_APP_MSG_HEADER( pstAppImsiInd);

    /*设置消息类型*/
    pstAppImsiInd->ulMsgId     = ID_APP_MM_INQ_IMSI_IND;
    pstAppImsiInd->ulOpId      = DIAG_CMD_EMM_APP_IMSI_INFO_IND;


    NAS_LMM_MEM_SET(&stImsiInfo, 0, sizeof(NAS_OM_IMSI_INFO_STRU));

    /*如果当前无卡,向OM回复查询IMSI为空*/
    if (NAS_LMM_SIM_STATUS_AVAILABLE == NAS_LMM_GetSimState())
    {

        /* 获取IMSI */
        stImsiInfo.ucImsiLen = (VOS_UINT8)((g_stEmmInfo.stMmUeId.aucImsi[0])*2 - 1);

        /*最后一个字节最高4位为F,则表明IMSI为偶数个 */
        if (0xF0 == (g_stEmmInfo.stMmUeId.aucImsi[g_stEmmInfo.stMmUeId.aucImsi[0]] & 0xF0))
        {
            stImsiInfo.ucImsiLen -= 1;
        }

        for(i = 0; i < stImsiInfo.ucImsiLen; i++)
        {
            if (0 == (i+1)%2)
            {
                stImsiInfo.ucImsi[i] = 0x0F & (g_stEmmInfo.stMmUeId.aucImsi[(i+1)/2 + 1]);
            }
            else
            {
                stImsiInfo.ucImsi[i] = 0x0F & (g_stEmmInfo.stMmUeId.aucImsi[(i+1)/2 + 1] >> 4);
            }
        }
    }

    NAS_LMM_MEM_CPY( &pstAppImsiInd->stImsi,
                    &stImsiInfo,
                    sizeof(NAS_OM_IMSI_INFO_STRU));

    /*保存IMSI信息至全局变量*/
    NAS_LMM_MEM_CPY(&g_stEmmDtInfo.stImsi,
                        &stImsiInfo,
                        sizeof(NAS_OM_IMSI_INFO_STRU));

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppImsiInd);
    }
#endif
    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppImsiInd);

    return;
}
VOS_VOID  NAS_LMM_PubmIndEmmState(VOS_VOID)
{
    APP_MM_INQ_EMM_STATE_IND_STRU      *pstAppEmmStateInd;
    NAS_OM_EMM_STATE_STRU              stEmmPtlState;

    NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndEmmState: enter");


    /* 根据命令判断是否停止EMM State上报 */
    if (NAS_DT_RPT_STOP == NAS_EMM_GetNasDtRptCtrlItem(NAS_DT_REPORT_TYPE_EMM_STATE)->enRptStatus)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndEmmState: STOP");
        return;
    }


    /* 为消息分配地址 */
    pstAppEmmStateInd = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_EMM_STATE_IND_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppEmmStateInd)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmIndEmmState: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppEmmStateInd, 0, sizeof(APP_MM_INQ_EMM_STATE_IND_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_DT_IND_MSG_HEADER( pstAppEmmStateInd,
                                 EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_EMM_STATE_IND_STRU));

    /*打包APP消息头*/
    EMM_COMP_OMT_APP_MSG_HEADER( pstAppEmmStateInd);


    /*设置消息类型*/
    pstAppEmmStateInd->ulMsgId     = ID_APP_MM_INQ_EMM_STATE_IND;
    pstAppEmmStateInd->ulOpId      = DIAG_CMD_EMM_APP_EMM_STATE_IND;

    /* 获取协议状态信息 */
    NAS_LMM_PubmGetEmmState(&(stEmmPtlState.enMainState),
                           &(stEmmPtlState.enSubState),
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());

    stEmmPtlState.usStaTId = (VOS_UINT16)NAS_LMM_GetEmmCurTimer();

    NAS_LMM_MEM_CPY( &pstAppEmmStateInd->stEmmState,
                    &stEmmPtlState,
                    sizeof(NAS_OM_EMM_STATE_STRU));

    /*协议状态信息保存至全局变量*/
    NAS_LMM_MEM_CPY(&g_stEmmDtInfo.stEmmState,
                    &stEmmPtlState,
                    sizeof(NAS_OM_EMM_STATE_STRU));

#if (VOS_OS_VER != VOS_WIN32)
    if (1 == g_ulNasEmmOmMsgHookFlag)
    {
        (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstAppEmmStateInd);
    }
#endif
    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppEmmStateInd);
    return;
}

/* niuxiufan DT end */



/*****************************************************************************
 Function Name   : NAS_LMM_PubmDaInqUeMode
 Description     :
 Input           : MsgBlock    *pMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY        2012-02-17    Draft Enact

*****************************************************************************/
VOS_VOID  NAS_LMM_PubmDaInqUeMode(MsgBlock *pMsg)
{
    APP_MM_INQ_UE_MODE_REQ_STRU      *pstAppInqReq;
    APP_MM_INQ_UE_MODE_CNF_STRU      *pstAppInqCnf;

    pstAppInqReq                       = (VOS_VOID *)pMsg;

    /* 为消息分配地址 */
    pstAppInqCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(APP_MM_INQ_UE_MODE_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstAppInqCnf)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqUeMode: NAS_LMM_ALLOC_MSG alloc NULL_PTR !!");
        return;
    }


    /* 结构初始化 */
    NAS_LMM_MEM_SET(pstAppInqCnf, 0, sizeof(APP_MM_INQ_UE_MODE_CNF_STRU));

    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER(pstAppInqCnf,
                                EMM_MSG_LENTH_NO_HEADER(APP_MM_INQ_UE_MODE_CNF_STRU),
                                pstAppInqReq);

    /*设置消息体*/
    pstAppInqCnf->ulOpId      = pstAppInqReq->ulOpId;
    pstAppInqCnf->ulMsgId     = ID_APP_MM_INQ_UE_MODE_CNF;
    pstAppInqCnf->ulRslt      = APP_SUCCESS;
    pstAppInqCnf->enCurrentUeMode = NAS_LMM_GetEmmInfoUeOperationMode();
    /*当前UE支持CS\PS和PS模式，所以数量为2*/
    pstAppInqCnf->ulSupportModeCnt = 2;
    if(EMM_SETTING_DATA_CENTRIC == NAS_LMM_GetEmmInfoUeCenter())
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqUeMode: UE Center is DATA_CENTRIC!!");
        pstAppInqCnf->aenSupportModeList[0] = NAS_LMM_UE_PS_MODE_2;
        pstAppInqCnf->aenSupportModeList[1] = NAS_LMM_UE_CS_PS_MODE_2;

    }
    else
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_PubmDaInqUeMode: UE Center is VOICE_CENTRIC!!");
        pstAppInqCnf->aenSupportModeList[0] = NAS_LMM_UE_PS_MODE_1;
        pstAppInqCnf->aenSupportModeList[1] = NAS_LMM_UE_CS_PS_MODE_1;
    }

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstAppInqCnf);

    return;
}



VOS_VOID    NAS_LMM_PubmGetEmmState
(
    APP_EMM_MAIN_STATE_ENUM_UINT16      *pulProtoclMainState,
    APP_EMM_SUB_STATE_ENUM_UINT16       *pulProtoclSubState,
    NAS_LMM_MAIN_STATE_ENUM_UINT16      enMainState,
    NAS_LMM_SUB_STATE_ENUM_UINT16       enSubState
)
{
    APP_EMM_MAIN_STATE_ENUM_UINT16      ulProtoclMainState;
    APP_EMM_SUB_STATE_ENUM_UINT16       ulProtoclSubState;
    VOS_UINT32                          ulLoop;

    NAS_LMM_FSM_STATE_STACK_STRU         *pstFsmStack;        /* 状态机栈地址 */
    NAS_LMM_FSM_STATE_STRU               *pstFsmStackTop;     /* 指向栈顶状态 */


    /* 初始化协议状态 */
    ulProtoclMainState                  = APP_EMM_MS_NULL;
    ulProtoclSubState                   = APP_EMM_SS_NULL;


    /* 连接建立过程,释放过程,鉴权过程 需要从状态栈获取状态 */
    if ( (EMM_MS_RRC_CONN_EST_INIT == enMainState)
        ||(EMM_MS_RRC_CONN_REL_INIT == enMainState)
        ||(EMM_MS_AUTH_INIT == enMainState) )
    {
        /* 获取状态栈栈顶地址 */
        pstFsmStack
            =  NAS_LMM_GetFsmStackAddr(NAS_LMM_PARALLEL_FSM_EMM);
        pstFsmStackTop
            = &((pstFsmStack->astFsmStack)[pstFsmStack->ucStackDepth - 1]);

        /* 从状态栈获取状态 */
        enMainState                     = pstFsmStackTop->enMainState;
        enSubState                      = pstFsmStackTop->enSubState;
    }

    /*挂起过程中 */
    if (EMM_MS_SUSPEND == enMainState)
    {
        enMainState = NAS_EMM_GetMsBefSuspend();
        enSubState = NAS_EMM_GetSsBefSuspend();
    }

    if (EMM_MS_RESUME == enMainState)
    {
        if (EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND == enSubState)
        {
            enMainState = NAS_EMM_GetMsBefResume();
            enSubState = NAS_EMM_GetSsBefResume();
        }
        else
        {
            if ((NAS_LMM_SYS_CHNG_TYPE_REVERSE_RSL == NAS_EMM_GetResumeType())
                || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_HO == NAS_EMM_GetResumeType())
                || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_CCO == NAS_EMM_GetResumeType())
                || (NAS_LMM_SYS_CHNG_TYPE_REVERSE_REDIR == NAS_EMM_GetResumeType()))
            {
                enMainState = NAS_EMM_GetMsBefSuspend();
                enSubState = NAS_EMM_GetSsBefSuspend();
            }
            else
            {
                enMainState = NAS_EMM_GetMsBefResume();
                enSubState = NAS_EMM_GetSsBefResume();
            }
        }
    }

    /* 根据状态映射表读取协议状态 */
    for (ulLoop = 0; ulLoop< g_ulProtocolStateMapNum; ulLoop++)
    {
        if ( (enMainState == g_astProtocolStateMap[ulLoop].enMainState)
            &&((enSubState == g_astProtocolStateMap[ulLoop].enSubState)
              || (MM_SS_BUTT == g_astProtocolStateMap[ulLoop].enSubState)) )
        {
            ulProtoclMainState
                = g_astProtocolStateMap[ulLoop].ulProtoclMainState;
            ulProtoclSubState
                = g_astProtocolStateMap[ulLoop].ulProtoclSubState;
            break;
        }
    }

    /* 赋值输出 */
    *pulProtoclMainState                = ulProtoclMainState;
    *pulProtoclSubState                 = ulProtoclSubState;

    return;
}
VOS_VOID    NAS_LMM_PubmGetEmmUpdateState
(
    APP_EMM_UPDATE_STATE_ENUM_UINT32    *pulEmmUpState
)
{
    *pulEmmUpState                      =  NAS_EMM_UPDATE_STAE;

    return;
}



VOS_VOID    NAS_LMM_PubmGetAreaInfo
(
    APP_GUTI_STRU                       *pstGuti,
    APP_TA_LIST_STRU                    *pstTaiList,
    APP_TA_STRU                         *pstLastTa
)
{
    NAS_MM_GUTI_STRU                    *pstEmmCurGuti;
    NAS_MM_TA_LIST_STRU                 *pstEmmCurTaiList;
    NAS_MM_NETWORK_ID_STRU              *pstEmmCurNetId;

    /* 获取GUTI */
    pstEmmCurGuti                       = NAS_LMM_GetEmmInfoUeidGutiAddr();
    NAS_LMM_MEM_CPY( pstGuti,
                    pstEmmCurGuti,
                    sizeof(NAS_LMM_GUTI_STRU));

    /* 获取TAI列表 */
    pstEmmCurTaiList                    = NAS_LMM_GetEmmInfoNetInfoTaiListAddr();
    NAS_LMM_MEM_CPY( pstTaiList,
                    pstEmmCurTaiList,
                    sizeof(NAS_MM_TA_LIST_STRU));

    /* 获取TA */
    pstEmmCurNetId                      = NAS_LMM_GetEmmInfoNetInfoLastRegNetAddr();
    NAS_LMM_MEM_CPY( &pstLastTa->stPlmnId,
                    &pstEmmCurNetId->stPlmnId,
                    sizeof(NAS_MM_PLMN_ID_STRU));
    NAS_LMM_MEM_CPY( &pstLastTa->stTac,
                    &pstEmmCurNetId->stTac,
                    sizeof(NAS_MM_TAC_STRU));

    return;

}
VOS_VOID    NAS_LMM_PubmGetEmmInfo
(
    APP_EMM_INFO_STRU                   *pstEmmInfoCnf
)
{
    /* 获取协议状态信息 */
    NAS_LMM_PubmGetEmmState(&(pstEmmInfoCnf->ulMainState),
                           &(pstEmmInfoCnf->ulSubState),
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());
    /* 获取EMM update状态 */
    NAS_LMM_PubmGetEmmUpdateState(&(pstEmmInfoCnf->ulEmmUpState));

    /* 获取 GUTI,TaiList,Last Ta */
    NAS_LMM_PubmGetAreaInfo(&(pstEmmInfoCnf->stGuti),
                           &(pstEmmInfoCnf->stTaiList),
                           &(pstEmmInfoCnf->stLastTa));

}
VOS_VOID    NAS_LMM_SendOmtKeyEvent(     PS_EMM_OMT_KEY_EVENT_TYPE_ENUM_UINT8
                                        ucEmmOmtKeyEvent  )
{
#ifndef PS_ITT_PC_TEST
    /*lint -e778*/
    /*lint -e648*/
    if(ERR_MSP_SUCCESS != DIAG_ReportEventLog(MODID(UE_MODULE_MM_ID, LOG_TYPE_INFO),\
                    (VOS_UINT32)ucEmmOmtKeyEvent+OM_PS_KEY_EVENT_BASE_ID))
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_SendOmtKeyEvent: Send Msg Fail!");
        return;
    }
    /*lint +e648*/
    /*lint +e778*/
#endif

}



VOS_VOID    NAS_LMM_SendOmtAirMsg(       NAS_LMM_OM_AIR_MSG_UP_DOWN_ENUM_UINT8  ucDir,
                                        NAS_LMM_OM_AIR_MSG_ID_ENUM_UINT8       ucAirMsgId,
                                        NAS_MSG_STRU                       *pstNasMsg)
{
#ifndef PS_ITT_PC_TEST

    DIAG_AIR_MSG_LOG_STRU        stAirMsg;
    VOS_UINT8                   *pucTmpData = VOS_NULL_PTR;

    /*分配空口消息空间*/
    pucTmpData = (VOS_UINT8*)
                        NAS_LMM_MEM_ALLOC(pstNasMsg->ulNasMsgSize);

    if(VOS_NULL_PTR== pucTmpData)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_SendOmtAirMsg: MEM_ALLOC ERR!");
        return;
    }

    NAS_LMM_MEM_CPY( (pucTmpData),(pstNasMsg->aucNasMsg),(pstNasMsg->ulNasMsgSize));

    /*空口消息赋值*/
    if(NAS_EMM_OMT_AIR_MSG_UP == ucDir)
    {
        stAirMsg.ulId = DIAG_AIR_MSG_LOG_ID(PS_PID_MM, OS_MSG_UL);
        stAirMsg.ulSideId = DIAG_SIDE_NET;
    }
    else
    {
        stAirMsg.ulId = DIAG_AIR_MSG_LOG_ID(PS_PID_MM, OS_MSG_DL);
        stAirMsg.ulSideId = DIAG_SIDE_UE;
    }
    stAirMsg.ulMessageID = (VOS_UINT32)(ucAirMsgId+ PS_MSG_ID_AIR_BASE);
    stAirMsg.ulDestMod = 0;
    stAirMsg.ulDataSize = pstNasMsg->ulNasMsgSize;
    stAirMsg.pData = (void*)pucTmpData;

    /*上报空口消息*/
    if(ERR_MSP_SUCCESS != DIAG_ReportAirMessageLog(&stAirMsg))
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_SendOmtAirMsg: Send Msg Fail!");
    }

    /*释放内存空间*/
    NAS_LMM_MEM_FREE(pucTmpData);

#endif
    (VOS_VOID)(pstNasMsg);
}


VOS_VOID   NAS_LMM_FindAirMsgId( VOS_UINT8 ucCnMsgType,
                                VOS_UINT8 *pucAirMsg,
                                VOS_UINT8 *pucKeyEvent)
{
    VOS_UINT8 i;
    VOS_UINT8 ucLen;
    ucLen = sizeof(g_stMmOmMsgIdStru)/(sizeof(NAS_LMM_OM_MSG_ID_CHANGE_STRU));
    for(i = 0;i < ucLen;i++)
    {
        if (ucCnMsgType == g_stMmOmMsgIdStru[i].enCnMsgType)
        {
            *pucAirMsg   = g_stMmOmMsgIdStru[i].enAirMsg;
            *pucKeyEvent = g_stMmOmMsgIdStru[i].enKeyEvent;
            break;
        }
    }
    return;
}


VOS_VOID    NAS_LMM_PubmDaSendSetCnf( const NAS_LMM_APP_PARA_SET_RESLT_STRU *pstAppRslt )
{

    APP_MM_SET_CMD_CNF_STRU            *pstEmmSetCnf;

    NAS_LMM_PUBM_LOG3_NORM("NAS_LMM_PubmDaSetCnf:  ulMsgId =, ulRslt =, ulOpId = ",
                                        pstAppRslt->ulMsgId,
                                        pstAppRslt->ulRslt,
                                        pstAppRslt->ulOpId);

    /* 申请DOPRA消息 */
    pstEmmSetCnf = (VOS_VOID *)NAS_LMM_ALLOC_MSG(
                                        sizeof(APP_MM_SET_CMD_CNF_STRU));
    if(NAS_LMM_NULL_PTR  == pstEmmSetCnf)
    {
        NAS_LMM_PUBM_LOG1_WARN("NAS_LMM_PubmDaSetCnf: alloc NULL_PTR !! ulMsgId = ",
                                        pstAppRslt->ulMsgId);
        return;
    }

    NAS_LMM_MEM_SET(pstEmmSetCnf, 0, sizeof(APP_MM_SET_CMD_CNF_STRU));


    /* 打包消息头 */
    EMM_COMP_APP_MSG_VOS_HEADER( pstEmmSetCnf,
                                 EMM_MSG_LENTH_NO_HEADER(APP_MM_SET_CMD_CNF_STRU),
                                 pstAppRslt);

    /* 填写消息体 */
    pstEmmSetCnf->ulMsgId               = pstAppRslt->ulMsgId;
    pstEmmSetCnf->ulOpId                = pstAppRslt->ulOpId;
    pstEmmSetCnf->ulRslt                = pstAppRslt->ulRslt;

    /* 发送消息 */
    NAS_LMM_SEND_MSG(pstEmmSetCnf);

    return;

}


VOS_UINT32  NAS_LMM_CheckAppDrxParam( CONST APP_DRX_STRU *pstAppDrxParam )
{
    /*检查入参指针*/
    if (VOS_NULL_PTR == pstAppDrxParam)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CheckAppDrxParam: Input NULL PTR.");
        return NAS_EMM_PARA_INVALID;
    }

    /*SPLIT on CCCH有效性验证*/
    if (NAS_EMM_PARA_INVALID == NAS_LMM_IsDrxSplitOnCcchValid(pstAppDrxParam->ucSplitOnCcch))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CheckAppDrxParam: SplitOnCcch InValid.");
        return NAS_EMM_PARA_INVALID;
    }

    /*non_DRX Timer有效性验证*/
    if (NAS_EMM_PARA_INVALID == NAS_LMM_IsDrxNonDrxTimerValid(pstAppDrxParam->ucNonDrxTimer))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CheckAppDrxParam: NonDrxTimer InValid.");
        return NAS_EMM_PARA_INVALID;
    }

    /*SPLIT PG CYCLE CODE有效性验证*/
    if (NAS_EMM_PARA_INVALID == NAS_LMM_IsDrxPgCycleCodeValid(pstAppDrxParam->ucSplitPgCode))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CheckAppDrxParam: SplitPgCycleCode InValid.");
        return NAS_EMM_PARA_INVALID;
    }

    /* DRX Valude for S1 Mode有效性验证*/
    if (NAS_EMM_PARA_INVALID == NAS_LMM_IsUeDrxCycleValid(pstAppDrxParam->ucPsDrxLen))
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_CheckAppDrxParam: UeSpecDrxCycle InValid.");
        return NAS_EMM_PARA_INVALID;
    }

    return NAS_EMM_PARA_VALID;

}
VOS_UINT32  NAS_LMM_IsDrxSplitOnCcchValid( VOS_UINT8 ucSplitOnCcch )
{
    /*ucSplitOnCcch取值:0 or 1*/
    if ((NAS_LMM_SPLIT_ON_CCCH_NOT_SUPPORT != ucSplitOnCcch)
      &&(NAS_LMM_SPLIT_ON_CCCH_SUPPORT != ucSplitOnCcch))
    {
        return NAS_EMM_PARA_INVALID;
    }
    return NAS_EMM_PARA_VALID;
}
VOS_UINT32  NAS_LMM_IsDrxNonDrxTimerValid( VOS_UINT8 ucNonDrxTimer )
{
    /*ucNonDrxTimer取值范围:[0,7]*/
    if (NAS_LMM_MAX_NON_DRX_TIMER < ucNonDrxTimer)
    {
        return NAS_EMM_PARA_INVALID;
    }
    return NAS_EMM_PARA_VALID;
}
VOS_UINT32  NAS_LMM_IsDrxPgCycleCodeValid( VOS_UINT8 ucSplitPgCode )
{
    /*ucSplitPgCode取值范围:[0,98]*/
    if (NAS_LMM_MAX_SPLIT_PG_CYCLE_CODE < ucSplitPgCode)
    {
        return NAS_EMM_PARA_INVALID;
    }
    return NAS_EMM_PARA_VALID;
}
VOS_UINT32  NAS_LMM_IsUeDrxCycleValid( VOS_UINT8 ucPsDrxLen )
{
    /*Drx Value for S1 Mode: 24008-930 10.5.5.6*/
    CONST VOS_UINT32 aulUeSpecDrxValue[NAS_LMM_UE_SPEC_DRX_VALUE_NUM] = {0,6,7,8,9};
    VOS_UINT32                          i;

    for(i = 0; i < NAS_LMM_UE_SPEC_DRX_VALUE_NUM; i++)
    {
        if(aulUeSpecDrxValue[i] == ucPsDrxLen)
        {
           return NAS_EMM_PARA_VALID;
        }
    }

    return NAS_EMM_PARA_INVALID;
}

/*PC REPLAY MODIFY BY LEILI DELETE*/

/* xiongxianghui00253310 modify for ftmerrlog begin */

VOS_VOID NAS_LMM_OmInfoIndProc(VOS_VOID)
{
    VOS_UINT32 ulFtmRst = NAS_EMM_FAIL;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_OmInfoIndProc!");

    if(NAS_EMM_FTM_ACTION_FLAG_OPEN == NAS_EMM_GetFtmInfoActionFlag())
    {
        ulFtmRst = NAS_LMM_CompareEmmDatabaseInfo();
    }

    NAS_LMM_UpdateEmmDatabaseInfo();

    if(NAS_EMM_SUCC == ulFtmRst)
    {
        NAS_LMM_SendOmFtmMsg();
    }
}



VOS_VOID NAS_LMM_FtmInfoInit( VOS_VOID )
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_FtmInfoInit!");

    NAS_LMM_MEM_SET(NAS_EMM_GetFtmInfoManageAddr(), 0, sizeof(EMM_FTM_INFO_MANAGE_STRU));

    NAS_LMM_MEM_SET(NAS_EMM_GetEmmInfoAddr(), 0, sizeof(EMM_DATABASE_INFO_STRU));

    NAS_EMM_GetFtmInfoActionFlag() = NAS_EMM_FTM_ACTION_FLAG_CLOSE;

    return;
}



VOS_UINT32 NAS_LMM_CompareEmmDatabaseInfo( VOS_VOID )
{
    NAS_LMM_TIMER_CTRL_STRU             *pstPtlTimer;
    APP_EMM_MAIN_STATE_ENUM_UINT16      ulProtoclMainState;
    APP_EMM_SUB_STATE_ENUM_UINT16       ulProtoclSubState;


    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CompareEmmDatabaseInfo!");

    /* 由EMM当前状态获取协议状态 */
    NAS_LMM_PubmGetEmmState(&ulProtoclMainState,
                            &ulProtoclSubState,
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());

    if((ulProtoclMainState != NAS_EMM_GetEmmInfoAddr()->enMainState)
        || (ulProtoclSubState != NAS_EMM_GetEmmInfoAddr()->enSubState))
    {
        return NAS_EMM_SUCC;
    }

    if(NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&NAS_EMM_GetEmmInfoAddr()->stGuti,
                                                    NAS_LMM_GetEmmInfoUeidGutiAddr(),
                                                    sizeof(NAS_MM_GUTI_STRU)))
    {
        return NAS_EMM_SUCC;
    }

    if(NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&NAS_EMM_GetEmmInfoAddr()->stPlmnId,
                                                    NAS_LMM_GetEmmInfoLastRegPlmnAddr(),
                                                    sizeof(NAS_MM_PLMN_ID_STRU)))
    {
        return NAS_EMM_SUCC;
    }

    if((NAS_EMM_GetEmmInfoAddr()->enEmmCnRejCause) != NAS_EMM_GetFtmInfoCnRej())
    {
        return NAS_EMM_SUCC;
    }

    if(NAS_LMM_MEM_CMP_EQUAL != NAS_LMM_MEM_CMP(&NAS_EMM_GetEmmInfoAddr()->stTac,
                                                    NAS_LMM_GetEmmInfoLastRegTacAddr(),
                                                    sizeof(NAS_MM_TAC_STRU)))
    {
        return NAS_EMM_SUCC;
    }

    /*lint -e506 -e944*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(TI_NAS_EMM_PTL_T3412);
    /*lint +e506 +e944*/
    if(NAS_EMM_GetEmmInfoAddr()->ulTimerLen3412 != pstPtlTimer->ulTimerLen)
    {
        return NAS_EMM_SUCC;
    }

    return NAS_EMM_FAIL;
}
 VOS_VOID NAS_LMM_UpdateEmmDatabaseInfo(VOS_VOID)
 {
    NAS_LMM_TIMER_CTRL_STRU             *pstPtlTimer;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_UpdateEmmDatabaseInfo!");

    /* 更新EMM状态 */
    NAS_LMM_PubmGetEmmState(&NAS_EMM_GetEmmInfoAddr()->enMainState,
                            &NAS_EMM_GetEmmInfoAddr()->enSubState,
                            NAS_LMM_GetEmmCurFsmMS(),
                            NAS_LMM_GetEmmCurFsmSS());

    /* 更新GUTI信息 */
    NAS_LMM_MEM_CPY(&NAS_EMM_GetEmmInfoAddr()->stGuti,
                    NAS_LMM_GetEmmInfoUeidGutiAddr(),
                    sizeof(NAS_MM_GUTI_STRU));

    /* 更新PlmnId信息 */
    NAS_LMM_MEM_CPY(&NAS_EMM_GetEmmInfoAddr()->stPlmnId,
                    NAS_LMM_GetEmmInfoLastRegPlmnAddr(),
                    sizeof(NAS_MM_PLMN_ID_STRU));

    /* 更新TAC信息 */
    NAS_LMM_MEM_CPY(&NAS_EMM_GetEmmInfoAddr()->stTac,
                    NAS_LMM_GetEmmInfoLastRegTacAddr(),
                    sizeof(NAS_MM_TAC_STRU));


    /* 更新CN拒绝原因 */
    NAS_EMM_GetEmmInfoAddr()->enEmmCnRejCause = NAS_EMM_GetFtmInfoCnRej();

    /* 更新TimerLen3412 */
    /*lint -e506 -e944*/
    pstPtlTimer = NAS_LMM_GetPtlTimerAddr(TI_NAS_EMM_PTL_T3412);
    /*lint +e506 +e944*/
    NAS_EMM_GetEmmInfoAddr()->ulTimerLen3412 = pstPtlTimer->ulTimerLen;

    return;
 }


 VOS_UINT32   NAS_LMM_RevOmFtmCtrlMsg(MsgBlock   *pMsgStru)
 {
     OM_FTM_CTRL_IND_STRU  *pstOmFtmCtlInfo;

     NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmFtmCtrlMsg!");

     if (VOS_NULL_PTR == pMsgStru)
     {
         NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmFtmCtrlMsg, input point is null!");

         return NAS_LMM_MSG_DISCARD;
     }

     pstOmFtmCtlInfo = (OM_FTM_CTRL_IND_STRU *)(VOS_VOID *)pMsgStru;

     if(1 == pstOmFtmCtlInfo->ucActionFlag)
     {
        /*设置工程模式上报功能打开*/
         NAS_EMM_SetFtmOmManageFtmActionFlag(NAS_EMM_FTM_ACTION_FLAG_OPEN);

        /*上报当前信息 */
         NAS_LMM_SendOmFtmMsg();
     }
     else
     {
         /*关闭工程模式上报，*/
          NAS_EMM_SetFtmOmManageFtmActionFlag(NAS_EMM_FTM_ACTION_FLAG_CLOSE);
     }

     return NAS_LMM_MSG_HANDLED;

 }
 VOS_VOID     NAS_LMM_SendOmFtmMsg(VOS_VOID)
 {
     LNAS_OM_FTM_REPROT_IND_STRU *pFtmMsg;

     NAS_LMM_PUBM_LOG1_INFO("LMM_OM_SendOmFtmMsg: ActionFlag = ", NAS_EMM_GetFtmInfoActionFlag());

     /* 若工程模式关闭，则不发送 */
     if(NAS_EMM_FTM_ACTION_FLAG_OPEN != NAS_EMM_GetFtmInfoActionFlag())
     {
         return;
     }

     /*申请消息内存*/
     pFtmMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LNAS_OM_FTM_REPROT_IND_STRU));

     /*判断申请结果，若失败打印错误并退出*/
     if (VOS_NULL_PTR == pFtmMsg)
     {
         /*打印错误*/
         NAS_LMM_PUBM_LOG_ERR("LMM_OM_SendOmFtmMsg: MSG ALLOC ERR!");
         return;
     }

     /*构造ID_EMM_ESM_PDN_CON_IND消息*/
     /*填充消息头*/
     NAS_LMM_COMP_OM_MSG_HEADER(         pFtmMsg,
                                         (sizeof(LNAS_OM_FTM_REPROT_IND_STRU)-
                                         NAS_EMM_LEN_VOS_MSG_HEADER));

     /*填充消息ID*/
     pFtmMsg->ulMsgName                  = ID_OM_FTM_REPROT_IND;

     /*填充消息内容*/
     pFtmMsg->ulMsgType                  = OM_ERR_LOG_MSG_FTM_REPORT;
     pFtmMsg->ulMsgSN                    = NAS_EMM_GetFtmInfoMsgSN();
     NAS_EMM_GetFtmInfoMsgSN()++;
     pFtmMsg->ulRptlen                   = sizeof(LNAS_OM_FTM_REPROT_CONTENT_STRU);

     pFtmMsg->stLnasFtmContent.ulMsgModuleID  = OM_ERR_LOG_MOUDLE_ID_LMM;
     pFtmMsg->stLnasFtmContent.usModemId      = 0;
     pFtmMsg->stLnasFtmContent.usProjectID    = 0;
     pFtmMsg->stLnasFtmContent.ulLength       = sizeof(LNAS_LMM_FTM_INFO_STRU);

    /*填充拒绝原因*/
     pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.ucEmmCnRejCause =
         g_astEmmFtmInfoManage.enEmmCnRejCause;

    /*填充PlmnID*/
     pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stPlmnId.aucPlmnId[0] =
         g_astEmmInfo.stPlmnId.aucPlmnId[0];
     pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stPlmnId.aucPlmnId[1] =
         g_astEmmInfo.stPlmnId.aucPlmnId[1];
     pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stPlmnId.aucPlmnId[2] =
         g_astEmmInfo.stPlmnId.aucPlmnId[2];

    /*填充Guti*/
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMmeCode.ucMmeCode =
        g_astEmmInfo.stGuti.stMmeCode.ucMmeCode;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMmeGroupId.ucGroupId =
        g_astEmmInfo.stGuti.stMmeGroupId.ucGroupId;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMmeGroupId.ucGroupIdCnt =
        g_astEmmInfo.stGuti.stMmeGroupId.ucGroupIdCnt;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMTmsi.ucMTmsi =
        g_astEmmInfo.stGuti.stMTmsi.ucMTmsi;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMTmsi.ucMTmsiCnt1 =
        g_astEmmInfo.stGuti.stMTmsi.ucMTmsiCnt1;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMTmsi.ucMTmsiCnt2 =
        g_astEmmInfo.stGuti.stMTmsi.ucMTmsiCnt2;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stMTmsi.ucMTmsiCnt3 =
        g_astEmmInfo.stGuti.stMTmsi.ucMTmsiCnt3;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stPlmnId.aucPlmnId[0] =
        g_astEmmInfo.stGuti.stPlmnId.aucPlmnId[0];
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stPlmnId.aucPlmnId[1] =
        g_astEmmInfo.stGuti.stPlmnId.aucPlmnId[1];
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stGuti.stPlmnId.aucPlmnId[2] =
        g_astEmmInfo.stGuti.stPlmnId.aucPlmnId[2];

    /*填充Timer3412*/
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.ulTimerLen3412 =
        g_astEmmInfo.ulTimerLen3412;

    /*填充Tac*/
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stTac.ucTac =
        g_astEmmInfo.stTac.ucTac;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stTac.ucTacCnt =
        g_astEmmInfo.stTac.ucTacCnt;

    /*填充mainstate, substate*/
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stEmmState.ulLmmMainState =
        g_astEmmInfo.enMainState;
    pFtmMsg->stLnasFtmContent.u.stLmmFtmInfo.stEmmState.ulLmmSubState =
        g_astEmmInfo.enSubState;


     /*向OM模块发送状态变化消息*/
     NAS_LMM_SEND_MSG( pFtmMsg);

     return;

 }
VOS_VOID NAS_LMM_ErrlogInfoInit(VOS_VOID)
{
    VOS_UINT32                         ulRslt;
    NV_ID_ERR_LOG_CTRL_INFO_STRU       stNvErrlogCtrl;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ErrlogInfoInit!");

    NAS_LMM_MEM_SET(&stNvErrlogCtrl, 0, sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU));
    NAS_LMM_MEM_SET(NAS_EMM_GetErrlogManageAddr(), 0, sizeof(EMM_ERRLOG_INFO_MANAGE_STRU));
    /*lint -e516*/
    /*lint -e732*/
    ulRslt = LPs_NvimItem_Read(en_NV_Item_ErrLogCtrlInfo, &stNvErrlogCtrl, sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU));
    /*lint +e732*/
    /*lint +e516*/
    if(EN_NV_OK != ulRslt)
    {
        NAS_EMM_GetErrlogActionFlag() = NAS_EMM_ERRLOG_ACTION_FLAG_CLOSE;
        NAS_EMM_GetErrlogAlmLevel() = NAS_EMM_ERRLOG_LEVEL_CRITICAL;
        return;
    }

    if(1 == stNvErrlogCtrl.ucAlmStatus)
    {
        NAS_EMM_GetErrlogActionFlag() = NAS_EMM_ERRLOG_ACTION_FLAG_OPEN;
        NAS_EMM_GetErrlogAlmLevel() = stNvErrlogCtrl.ucAlmLevel;
    }
    else
    {
        NAS_EMM_GetErrlogActionFlag() = NAS_EMM_ERRLOG_ACTION_FLAG_CLOSE;
        NAS_EMM_GetErrlogAlmLevel() = NAS_EMM_ERRLOG_LEVEL_CRITICAL;
    }

    return;
}


VOS_UINT32 NAS_LMM_RevOmErrlogCtrlMsg(MsgBlock   *pMsgStru)
{
    OM_ERROR_LOG_CTRL_IND_STRU  *pstOmErrlogCtlInfo;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmErrlogCtrlMsg!");

    if (VOS_NULL_PTR == pMsgStru)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmErrlogCtrlMsg, input point is null!");

        return NAS_LMM_MSG_DISCARD;
    }

    pstOmErrlogCtlInfo = (OM_ERROR_LOG_CTRL_IND_STRU *)(VOS_VOID *)pMsgStru;

    if(1 == pstOmErrlogCtlInfo->ucAlmStatus)
    {
        /*设置商用Errlog功能打开*/
        NAS_EMM_SetErrlogActionFlag(NAS_EMM_ERRLOG_ACTION_FLAG_OPEN);
        NAS_EMM_SetErrlogAlmLevel(pstOmErrlogCtlInfo->ucAlmLevel);
    }
    else
    {
        /*设置商用Errlog功能关闭*/
        NAS_LMM_MEM_SET(NAS_EMM_GetErrlogManageAddr(), 0, sizeof(EMM_ERRLOG_INFO_MANAGE_STRU));
        NAS_EMM_SetErrlogActionFlag(NAS_EMM_ERRLOG_ACTION_FLAG_CLOSE);
    }

    return NAS_LMM_MSG_HANDLED;
}


 VOS_UINT32   NAS_LMM_RevOmReadErrlogReq(const MsgBlock   *pMsgStru)
 {
    VOS_UINT32  ulErrIndex = 0;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmReadErrlogReq!");

    if (VOS_NULL_PTR == pMsgStru)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevOmReadErrlogReq, input point is null!");

        return NAS_LMM_MSG_DISCARD;
    }

    /*发送error log信息到OM*/
    NAS_LMM_SendOmErrlogCnf();

    /*上报之后将error log buffer清空*/
    for(ulErrIndex = 0; ulErrIndex < NAS_EMM_ERRLOG_MAX_NUM; ulErrIndex++)
    {
        NAS_LMM_MEM_SET(&NAS_EMM_GetErrlogInfo(ulErrIndex), 0, sizeof(LMM_ERR_INFO_DETAIL_STRU));
    }

    NAS_EMM_GetErrlogAmount() = 0;
    NAS_EMM_GetErrlogNextNullPos() = 0;

    return NAS_LMM_MSG_HANDLED;
 }


VOS_VOID     NAS_LMM_SendOmErrlogCnf(VOS_VOID)
{
    LMM_OM_ERR_LOG_REPORT_CNF_STRU *pErrLogMsg;
    VOS_UINT32  ulErrIndex = 0;

    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SendOmErrlogCnf: ActionFlag = ", NAS_EMM_GetErrlogActionFlag());

    /*申请消息内存*/
    pErrLogMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LMM_OM_ERR_LOG_REPORT_CNF_STRU));

    /*判断申请结果，若失败打印错误并退出*/
    if (VOS_NULL_PTR == pErrLogMsg)
    {
        /*打印错误*/
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_SendOmErrlogCnf: MSG ALLOC ERR!");
        return;
    }

    NAS_LMM_MEM_SET(pErrLogMsg, 0, sizeof(LMM_OM_ERR_LOG_REPORT_CNF_STRU))

    /*构造ID_EMM_ESM_PDN_CON_IND消息*/
    /*填充消息头*/
    NAS_LMM_COMP_OM_MSG_HEADER(         pErrLogMsg,
                                         (sizeof(LMM_OM_ERR_LOG_REPORT_CNF_STRU)-
                                         NAS_EMM_LEN_VOS_MSG_HEADER));

    /*填充消息ID*/
    pErrLogMsg->ulMsgName                  = ID_OM_ERR_LOG_REPORT_CNF;

    /*填充消息内容*/
    pErrLogMsg->ulMsgType                  = OM_ERR_LOG_MSG_ERR_REPORT;
    pErrLogMsg->ulMsgSN                    = NAS_EMM_GetErrlogMsgSN();
    NAS_EMM_GetErrlogMsgSN()++;

    if((NAS_EMM_ERRLOG_ACTION_FLAG_OPEN != NAS_EMM_GetErrlogActionFlag())
        ||(0 == NAS_EMM_GetErrlogAmount()))
    {
        pErrLogMsg->ulRptlen = 0;
    }
    else
    {
        pErrLogMsg->ulRptlen = sizeof(LMM_OM_ERR_LOG_INFO_STRU);
    }

    pErrLogMsg->stLmmErrlogInfo.ulMsgModuleID = OM_ERR_LOG_MOUDLE_ID_LMM;
    pErrLogMsg->stLmmErrlogInfo.usModemId = 0;
    pErrLogMsg->stLmmErrlogInfo.usALMLevel = NAS_EMM_GetErrlogAlmLevel();
    pErrLogMsg->stLmmErrlogInfo.usALMType = NAS_EMM_GetErrlogAlmType();

    pErrLogMsg->stLmmErrlogInfo.ulAlmLowSlice = NAS_EMM_GetErrlogAlmLowSlice();
    pErrLogMsg->stLmmErrlogInfo.ulAlmHighSlice = NAS_EMM_GetErrlogAlmHighSlice();

    pErrLogMsg->stLmmErrlogInfo.ulAlmLength = sizeof(LMM_ALM_INFO_STRU);

    pErrLogMsg->stLmmErrlogInfo.stAlmInfo.ulErrlogNum = NAS_EMM_GetErrlogAmount();

    NAS_LMM_PUBM_LOG1_INFO("ulMsgModuleID = ", pErrLogMsg->stLmmErrlogInfo.ulMsgModuleID);
    NAS_LMM_PUBM_LOG1_INFO("usModemId = ", pErrLogMsg->stLmmErrlogInfo.usModemId);
    NAS_LMM_PUBM_LOG1_INFO("usALMLevel = ", pErrLogMsg->stLmmErrlogInfo.usALMLevel);
    NAS_LMM_PUBM_LOG1_INFO("usALMType = ", pErrLogMsg->stLmmErrlogInfo.usALMType);
    NAS_LMM_PUBM_LOG1_INFO("ulAlmLowSlice = ", pErrLogMsg->stLmmErrlogInfo.ulAlmLowSlice);
    NAS_LMM_PUBM_LOG1_INFO("ulAlmHighSlice = ", pErrLogMsg->stLmmErrlogInfo.ulAlmHighSlice);
    NAS_LMM_PUBM_LOG1_INFO("ulAlmLength = ", pErrLogMsg->stLmmErrlogInfo.ulAlmLength);
    NAS_LMM_PUBM_LOG1_INFO("ulErrlogNum = ", pErrLogMsg->stLmmErrlogInfo.stAlmInfo.ulErrlogNum);

    for(ulErrIndex = 0; ulErrIndex < NAS_EMM_GetErrlogAmount(); ulErrIndex++)
    {
        pErrLogMsg->stLmmErrlogInfo.stAlmInfo.stLmmErrInfoDetail[ulErrIndex].usErrLogID =
            NAS_EMM_GetErrlogInfo(ulErrIndex).usErrLogID;
        NAS_LMM_PUBM_LOG1_INFO("usErrLogID = ", NAS_EMM_GetErrlogInfo(ulErrIndex).usErrLogID);
    }

    /*向OM模块发送状态变化消息*/
    NAS_LMM_SEND_MSG( pErrLogMsg);

    return;
}


VOS_VOID NAS_LMM_ErrlogInfoProc(VOS_UINT8 ucCnCause)
{
    VOS_UINT64                          ulCurTime;
    LMM_ERR_INFO_DETAIL_STRU            stErrlogInfo;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ErrlogInfoProc!");

    if (NAS_EMM_ERRLOG_ACTION_FLAG_OPEN != NAS_EMM_GetErrlogActionFlag())
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_EMM_ERRLOG_ACTION_FLAG_OPEN != NAS_EMM_GetErrlogActionFlag()!");

        return;
    }
    (VOS_VOID)BSP_BBPGetCurTime(&ulCurTime);
    NAS_EMM_GetErrlogAlmLowSlice() = ulCurTime & 0xffffffff;
    NAS_EMM_GetErrlogAlmHighSlice() = 0;

    stErrlogInfo.usErrLogID = NAS_LMM_CnCauseProc(ucCnCause);

    NAS_LMM_MEM_CPY(&NAS_EMM_GetErrlogInfo(NAS_EMM_GetErrlogNextNullPos()),
                    &stErrlogInfo,
                    sizeof(LMM_ERR_INFO_DETAIL_STRU));

    if(NAS_EMM_GetErrlogAmount() < NAS_EMM_ERRLOG_MAX_NUM)
    {
        /* 存储的商用Errlog个数小于NAS_EMM_ERRLOG_MAX_NUM */

        NAS_EMM_GetErrlogNextNullPos()++;

        NAS_EMM_GetErrlogAmount()++;

        if(NAS_EMM_GetErrlogAmount() >= NAS_EMM_ERRLOG_MAX_NUM)
        {
            NAS_EMM_GetErrlogNextNullPos() = 0;
        }
    }
    else
    {
        /* 存储的商用Errlog个数等于NAS_EMM_ERRLOG_MAX_NUM */

        NAS_EMM_GetErrlogNextNullPos()++;

        if(NAS_EMM_GetErrlogNextNullPos() >= NAS_EMM_ERRLOG_MAX_NUM)
        {
            NAS_EMM_GetErrlogNextNullPos() = 0;
        }
    }

    return;
}



LNAS_OM_ERRLOG_ID_ENUM_UINT16  NAS_LMM_CnCauseProc(NAS_EMM_CN_CAUSE_ENUM_UINT8 ucCnCause)
{
    LNAS_OM_ERRLOG_ID_ENUM_UINT16   ulErrId = 0;
    VOS_UINT32 i;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_CnCauseProc!");

    for(i = 0; i < g_astEmmErrlogErrNum; i++)
    {
        if(ucCnCause == g_astEmmErrlogErrNoMap[i].ulCauseId)
        {
            ulErrId = g_astEmmErrlogErrNoMap[i].ulErrorlogID;
            break;
        }
    }
    if(g_astEmmErrlogErrNum == i)
    {
        ulErrId = LNAS_OM_ERRLOG_ID_CN_CAUSE_OTHERS;
    }

    return ulErrId;
}

/* xiongxianghui00253310 modify for ftmerrlog end  */

/*leixiantiao fix pclint error 826 begin*/
/*lint -e826*/
VOS_VOID NAS_LMM_SaveRevMsgInfo(MsgBlock *pMsg)
{
    NAS_LMM_MSG_SAVE_INFO_STRU    *pstRevMsgInfo = VOS_NULL_PTR;
    NAS_LMM_PID_MSG_STRU          *pMmPidMsg = VOS_NULL_PTR;
    pMmPidMsg                = (NAS_LMM_PID_MSG_STRU *)pMsg;


    /*获取地址*/
    pstRevMsgInfo = NAS_EMM_GetErrlogMsgQueueAddr();

    if(NAS_SAVE_RECEIVE_MSG_INFO_NUM <= pstRevMsgInfo->ulNextIndex)
    {
        pstRevMsgInfo->ulNextIndex = 0;
    }

    /*填写错误打印信息*/
    pstRevMsgInfo->astReciveMsgInfo[pstRevMsgInfo->ulNextIndex].ulTimeStamp = VOS_GetTick();
    pstRevMsgInfo->astReciveMsgInfo[pstRevMsgInfo->ulNextIndex].ulSendPid = pMmPidMsg->ulSenderPid;
    pstRevMsgInfo->astReciveMsgInfo[pstRevMsgInfo->ulNextIndex].ulMsgName = pMmPidMsg->ulMsgId;

    pstRevMsgInfo->ulNextIndex++;
}
/*lint +e826*/
/*leixiantiao fix pclint error 826 end*/


VOS_VOID NAS_LMM_ExportRevMsgQueque2ExcLog(VOS_UINT32* pulExcLogAddr, VOS_UINT32 ulSaveSize)
{
    NAS_LMM_MSG_SAVE_INFO_STRU    *pstRevMsgInfo = VOS_NULL_PTR;
    VOS_UINT32                     *pulSaveAddr = pulExcLogAddr;
    VOS_UINT32                      i = 0;
    VOS_UINT32                      ulEntryNum = 0;
    VOS_UINT32                      ulLeftSpace = ulSaveSize;

    /*获取地址*/
    pstRevMsgInfo = NAS_EMM_GetErrlogMsgQueueAddr();

    *pulSaveAddr = (VOS_UINT32)NAS_LMM_GetEmmCurFsmMS();
    *(pulSaveAddr+1) = (VOS_UINT32)NAS_LMM_GetEmmCurFsmSS();
    pulSaveAddr += 2;
    ulLeftSpace -= (2 * sizeof(VOS_UINT32));

    /*最新的消息排在最前面保存，依次排列 */
    for (i = pstRevMsgInfo->ulNextIndex; i > 0; i--)
    {
        *(pulSaveAddr + ulEntryNum*3) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulTimeStamp;
        *(pulSaveAddr + ulEntryNum*3 + 1) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulSendPid;
        *(pulSaveAddr + ulEntryNum*3 + 2) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulMsgName;
        ulEntryNum++;
        if (ulLeftSpace < ((ulEntryNum + 1) * sizeof(NAS_LMM_RECIVE_MSG_STRU)))
        {
            return;
        }
    }

    for (i = NAS_SAVE_RECEIVE_MSG_INFO_NUM; i > pstRevMsgInfo->ulNextIndex; i--)
    {
        *(pulSaveAddr + ulEntryNum*3) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulTimeStamp;
        *(pulSaveAddr + ulEntryNum*3 + 1) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulSendPid;
        *(pulSaveAddr + ulEntryNum*3 + 2) = pstRevMsgInfo->astReciveMsgInfo[i - 1].ulMsgName;
        ulEntryNum++;
        if (ulLeftSpace < ((ulEntryNum + 1) * sizeof(NAS_LMM_RECIVE_MSG_STRU)))
        {
            return;
        }
    }

}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

/* end of 子系统+模块+文件名.c */
