


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasEmmcMain.h"
#include  "NasEmmcPublic.h"
#include  "NasEmmcRcvMsg.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMCMAIN_C
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



/*****************************************************************************
  3 Function
*****************************************************************************/
static NAS_EMMC_COMM_ACT_STRU               g_astEmmcRsvMsgProcMap[] =
{
    /* MM_MSG_TYPE_ENUM_UINT32          NAS_LMM_COMM_ACTION_FUN */

    /*========== MMC_MSG ==========*/
    {ID_MMC_LMM_PLMN_SRCH_REQ,          WUEPS_PID_MMC,          NAS_EMMC_RcvMmcPlmnSrchReq},
    {ID_MMC_LMM_EPLMN_NOTIFY_REQ,       WUEPS_PID_MMC,          NAS_EMMC_RcvMmcEplmnNotifyReq},
    {ID_MMC_LMM_STOP_PLMN_SRCH_REQ,     WUEPS_PID_MMC,          NAS_EMMC_RcvMmcStopSrchReq},
    {ID_MMC_LMM_USER_PLMN_END_NOTIFY,   WUEPS_PID_MMC,          NAS_EMMC_RcvMmcUserPlmnEndNotify},
    {ID_MMC_LMM_WCDMA_SYS_INFO_IND,     WUEPS_PID_MMC,          NAS_EMMC_RcvMmcGuSysInfoInd},
    {ID_MMC_LMM_GSM_SYS_INFO_IND,       WUEPS_PID_MMC,          NAS_EMMC_RcvMmcGuSysInfoInd},
    {ID_MMC_LMM_BG_PLMN_SEARCH_REQ,     WUEPS_PID_MMC,          NAS_EMMC_RcvMmcBgPlmnSearchReq},
    {ID_MMC_LMM_STOP_BG_PLMN_SEARCH_REQ,WUEPS_PID_MMC,          NAS_EMMC_RcvMmcStopBgPlmnSearchReq},
    {ID_MMC_LMM_CELL_SELECTION_CTRL_REQ,WUEPS_PID_MMC,          NAS_EMMC_RcvMmcCellSelectionReq},
    {ID_MMC_LMM_LTE_SYS_INFO_IND,       WUEPS_PID_MMC,          NAS_LMM_PreProcMmcLteSysInfoInd},

    /*========== EMM_MSG ==========*/
    {ID_EMMC_EMM_START_REQ,             PS_PID_MM,              NAS_EMMC_RcvEmmStartReq},
    {ID_EMMC_EMM_STOP_REQ,              PS_PID_MM,              NAS_EMMC_RcvEmmStopReq},

    /*========== LRRC_MSG ==========*/
    {ID_LRRC_LMM_PLMN_SEARCH_CNF,       PS_PID_ERRC,            NAS_EMMC_RcvRrcPlmnSrchCnf},
    {ID_LRRC_LMM_SYS_INFO_IND,          PS_PID_ERRC,            NAS_EMMC_RcvRrcSysInfoInd},
    {ID_LRRC_LMM_PLMN_SEARCH_STOP_CNF,  PS_PID_ERRC,            NAS_EMMC_RcvRrcPlmnSrchStopCnf},
    {ID_LRRC_LMM_AREA_LOST_IND,         PS_PID_ERRC,            NAS_EMMC_RcvRrcAreaLostInd},
    {ID_LRRC_LMM_BG_PLMN_SEARCH_CNF,    PS_PID_ERMM,            NAS_EMMC_RcvRrcBgPlmnSearchCnf},
    {ID_LRRC_LMM_BG_PLMN_SEARCH_STOP_CNF,PS_PID_ERMM,           NAS_EMMC_RcvRrcBgPlmnSearchStopCnf},
    {ID_LRRC_LMM_NOT_CAMP_ON_IND,        PS_PID_ERRC,           NAS_EMMC_RcvRrcNotCampOnInd},

    /*========== TIMER_MSG ==========*/
    {TI_NAS_EMMC_STATE_WAIT_PLMN_SRCH_CNF_TIMER,    VOS_PID_TIMER,  NAS_EMMC_ProcTimerMsgWaitPlmnSrchcnfTimerExp},

};



static VOS_UINT32   g_astEmmcRsvMsgProcMapNum
        = sizeof(g_astEmmcRsvMsgProcMap)/sizeof(NAS_LMM_COMM_ACT_STRU);
/*lint -e960*/
VOS_UINT32  NAS_EMMC_MsgDistr( MsgBlock *pMsg )
{

    VOS_UINT32                          ulRet       = NAS_LMM_MSG_DISCARD;
    NAS_EMMC_COMM_ACTION_FUN            pActFun     = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulMsgId     = NAS_EMM_NULL;
    PS_MSG_HEADER_STRU                 *pHeader     = VOS_NULL_PTR;

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pMsg;
    NAS_EMMC_GetNameFromMsg(ulMsgId, pMsg);

    pActFun = NAS_EMMC_CommFindFun(     (NAS_EMMC_COMM_ACT_STRU *)g_astEmmcRsvMsgProcMap,
                                        g_astEmmcRsvMsgProcMapNum,
                                        ulMsgId,
                                        pHeader->ulSenderPid);
    if (NAS_EMMC_NULL_PTR != pActFun)
    {
        /* 若这里找到了，就在这里执行 */
        ulRet = (*pActFun)(pMsg);
    }

    return ulRet;
}
VOS_UINT32 NAS_EMMC_ProcTimerMsgWaitPlmnSrchcnfTimerExp(MsgBlock *pMsg)
{
    /*打印进入该函数*/
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_ProcTimerMsgWaitPlmnSrchcnfTimerExp entered");

    (VOS_VOID)pMsg;

    NAS_EMMC_SetPlmnSrchFlag(NAS_EMM_PLMN_SRCH_FLAG_INVALID);

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMMC_Init(VOS_VOID )
{

    /*打印进入该函数*/
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC                          START INIT...");

    /*初始化EMMC全局变量*/
    /*NAS_LMM_MEM_SET(NAS_EMMC_GetEmmcInfoAddr(), (VOS_CHAR)0xff, sizeof(NAS_EMMC_GLOBAL_STRU));*/
    NAS_LMM_MEM_SET(NAS_EMMC_GetEmmcInfoAddr(), 0, sizeof(NAS_EMMC_GLOBAL_STRU));

    return;
}
/*lint +e960*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif






