

#include        "MM_Inc.h"
#include        "MmCcInterface.h"
#include        "GmmMmInterface.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_MM_RCV_C


#define NAS_MM_SYNC_INFO_CODEC_AMR      (0x05)
#define NAS_MM_SYNC_INFO_CODEC_AMR2     (0x06)
#define NAS_MM_SYNC_INFO_CODEC_AMRWB    (0x0a)



/***********************************************************************
 *  MODULE   : Mm_RcvMmcStartReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      2003.12.09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvMmcStartReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    Mm_TimerStop(MM_TIMER_STOP_ALL);
    g_MmGlobalInfo.ucRcvAgentFlg &= ~MM_RCV_AUTH_CNF_FLG;
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvMmcPlmnSrchIni(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvMmcPlmnUserSelReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            ucRet    消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvMmcPlmnUserSelReq(
                            VOS_VOID            *pRcvMsg                        /* 收到的原语头指针                         */
                            )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvMmcSysInfoInd(
                        VOS_VOID            *pRcvMsg                            /* 收到的原语头指针                         */
                        )
{
    VOS_UINT32                              ulT3212Value;
    VOS_UINT8                               ucRet = MM_TRUE;                               /* 返回值                                   */
    MMCMM_SYS_INFO_IND_STRU                *pSysInfoInd = VOS_NULL_PTR;                    /* 定义接收到的消息类型指针                 */

    VOS_UINT8   ucRacChgFlg;
    VOS_UINT32  ulNewCellId;

    VOS_UINT8                           ucSimPsRegStatus;
    VOS_UINT8                           ucPsAttachAllow;

    ucSimPsRegStatus    = NAS_MML_GetSimPsRegStatus();
    ucPsAttachAllow     = NAS_MML_GetPsAttachAllowFlg();

    pSysInfoInd = (MMCMM_SYS_INFO_IND_STRU *)pRcvMsg;                                                      /* 消息转换                                 */


    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        /* 判断是否为 GSM 的系统信息指示消息 */
        if (MMCMM_GSM_SYS_INFO_IND != pSysInfoInd->MsgHeader.ulMsgName)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvMmcSysInfoInd: WARNING: Receive WAS sys info in GSM.");

            return MM_FALSE;
        }
        ucRet = MM_RcvMmcGsmSysInfoInd(pRcvMsg);
        return ucRet;
    }
    else
    {
        /* 判断是否为 WCDMA 的系统信息指示消息 */
        if (MMCMM_SYS_INFO_IND != pSysInfoInd->MsgHeader.ulMsgName)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvMmcSysInfoInd: WARNING: Receive GAS sys info in WCDMA!.");

            return MM_FALSE;
        }
    }

    if ((MM_CONST_NUM_8 < pSysInfoInd->ucCnCommonSize)
        || (MM_CONST_NUM_8 < pSysInfoInd->ucCsInfoSize))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvMmcSysInfoInd:WARNING: MMCMM_SYS_INFO_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        if ((0 != pSysInfoInd->ucPsInfoSize)
         && (RRC_NAS_SYS_MSK_CSPS_INFO ==
            (RRC_NAS_SYS_MSK_CSPS_INFO & pSysInfoInd->ulMask)))
        {
            ucRacChgFlg = MM_Com_RacChgChk((MMCMM_SYS_INFO_IND_STRU*)pRcvMsg);

            /* BEGIN: Modified by liurui id:40632, 2007/3/5   PN:A32D09094 for 12.4.1.4c2*/
            if (MM_TRUE == ucRacChgFlg)
            {
                if (MM_TRUE == g_MmGlobalInfo.ucLikeNetmode2)
                {
                    g_MmGlobalInfo.ucLikeNetmode2 = MM_FALSE;
                    Mm_TimerStop( MM_TIMER_T3211 );
                    Mm_TimerStop( MM_TIMER_T3212 );
                }
                g_MmGlobalInfo.ucLikeB = MM_FALSE;
            }
            /* END:   Modified by liurui id:40632, 2007/3/5   PN:A32D09094 */
        }

        /* 处理紧急呼列表 */
        NAS_MM_ProcEccNumList_RcvSysinfo(pRcvMsg);

        g_MmGlobalInfo.ucSysInfoFlg = MM_TRUE;
        g_MmGlobalInfo.MsCsInfo.ulCurFobidnFlg = pSysInfoInd->ulForbiddenFlg;

        g_MmGlobalInfo.MsCsInfo.TempLai.PlmnId.ulMcc
                = g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMcc;
        g_MmGlobalInfo.MsCsInfo.TempLai.PlmnId.ulMnc
                = g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMnc;
        g_MmGlobalInfo.MsCsInfo.TempLai.ulLac =
            g_MmGlobalInfo.MsCsInfo.CurLai.ulLac;
        if (RRC_NAS_SYS_MSK_PLMN ==
            (RRC_NAS_SYS_MSK_PLMN & pSysInfoInd->ulMask))
        {
            g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMcc
                = pSysInfoInd->PlmnId.ulMcc;
            g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMnc
                = pSysInfoInd->PlmnId.ulMnc;
        }
        if (0 != pSysInfoInd->ucCnCommonSize)
        {
            g_MmGlobalInfo.MsCsInfo.CurLai.ulLac =
                  (VOS_UINT32)pSysInfoInd->usLac;
        }

        if (RRC_NAS_SYS_MSK_CSPS_INFO ==
            (RRC_NAS_SYS_MSK_CSPS_INFO & pSysInfoInd->ulMask))
        {
            if (0 != pSysInfoInd->ucCsInfoSize)
            {
                ulT3212Value =
                    pSysInfoInd->ucLRTimerT3212 * MM_T3212_UNIT_TRANSFORM;

                if (ulT3212Value > MM_TIMER_T3212_MAX_VALUE)
                {
                    ulT3212Value = MM_TIMER_T3212_MAX_VALUE;
                }

                if ((MM_CONST_NUM_0 == ulT3212Value)
                    || (MM_CONST_NUM_0 == gstMmTimer[MM_TIMER_T3212].ulTimerLen))
                {
                    gstMmTimer[MM_TIMER_T3212].ulTimerLen = ulT3212Value;                   /* 设置T3212的时长为无效值                  */
                    Mm_TimerStop(MM_TIMER_T3212);
                }
                else
                {
                    if (gstMmTimer[MM_TIMER_T3212].ulTimerLen != ulT3212Value)
                    {
                        Mm_ComT3212(ulT3212Value);
                    }
                }

                if (MM_CONST_NUM_0 == pSysInfoInd->ucAttachDetach)
                {
                    g_MmGlobalInfo.ucAttFlg = MM_ATT_NOT_NEED;
                }
                else
                {
                    g_MmGlobalInfo.ucAttFlg = MM_ATT_NEED;
                }
            }

            if (0 != pSysInfoInd->ucPsInfoSize)
            {
                g_MmGlobalInfo.usRac      = (VOS_UINT16)pSysInfoInd->ucRac;

                g_MmGlobalInfo.ucNewNtMod = pSysInfoInd->ucNetworkOperationMode;
                /* 如果当前的网络被bar，那么直接刷新一下网络模式*/
                if (VOS_TRUE == NAS_MML_GetCsRestrictRegisterFlg())
                {
                     g_MmGlobalInfo.ucNtMod = g_MmGlobalInfo.ucNewNtMod;
                }

                if (0 == g_MmGlobalInfo.ucNtMod)
                {
                    g_MmGlobalInfo.ucNtMod = g_MmGlobalInfo.ucNewNtMod;
                }
            }
            else
            {
                g_MmGlobalInfo.usRac      = MM_UNSUPPORT_GPRS;
                g_MmGlobalInfo.ucNtMod    = MM_NET_MODE_INVALID;
                g_MmGlobalInfo.ucNewNtMod = MM_NET_MODE_INVALID;
            }
        }

        if (0 == pSysInfoInd->ucCsInfoSize)
        {
            /* 系统消息中指示CS不支持，MM在idle态下状态迁移到No cell available */
            if (MM_IDLE_NO_CELL_AVAILABLE == g_MmGlobalInfo.ucState)
            {
                ucRet = MM_FALSE;
            }

            if ((MM_IDLE_NORMAL_SERVICE == g_MmGlobalInfo.ucState)
             || (MM_IDLE_ATTEMPTING_TO_UPDATE == g_MmGlobalInfo.ucState)
             || (MM_IDLE_LIMITED_SERVICE == g_MmGlobalInfo.ucState)
             || (MM_IDLE_PLMN_SEARCH == g_MmGlobalInfo.ucState)
             || (MM_IDLE_LOCATION_UPDATE_NEEDED == g_MmGlobalInfo.ucState))
            {
                g_MmGlobalInfo.ucMmServiceState = MM_NO_SERVICE;
                Mm_ComSetMmState(MM_IDLE_NO_CELL_AVAILABLE);
                PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_NORMAL, "Mm_RcvMmcSysInfoInd:NORMAL: STATUS is MM_IDLE_NO_CELL_AVAILABLE");
                ucRet = MM_FALSE;
            }


            /* 回复MMCMM_ATTACH_CNF的操作,清除等待ATTACH结果标志位 */
            if (MM_WAIT_CS_ATTACH == (g_MmGlobalInfo.stAttachInfo.enAttachType
                                    & MM_WAIT_CS_ATTACH))
            {
                NAS_MM_SndMmcAttachCnf();
            }
            g_MmGlobalInfo.stAttachInfo.enAttachType = MM_WAIT_NULL_ATTACH;

            NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE,
                                        MM_MMC_LU_RESULT_FAILURE,
                                        NAS_MML_REG_FAIL_CAUSE_SERVING_CELL_DOMAIN_NOT_SUPPORT);
            NAS_MM_SndGmmLuResultInd(MMGMM_LU_FAILURE,
                                     NAS_MML_REG_FAIL_CAUSE_OTHER_CAUSE);
        }

        if (RRC_NAS_SYS_MSK_CS_DRX ==
            (RRC_NAS_SYS_MSK_CS_DRX & pSysInfoInd->ulMask))
        {
            g_MmGlobalInfo.MsCsInfo.ucCsDrxLen
                = (VOS_UINT8)pSysInfoInd->ulCsDrxLength;
        }

        if (MM_NET_MODE_I == g_MmGlobalInfo.ucNewNtMod)
        {
            if ((VOS_TRUE != ucSimPsRegStatus)
                || (VOS_FALSE == ucPsAttachAllow))
            {
                g_MmGlobalInfo.ucLikeB = MM_TRUE;
                PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_INFO, "Mm_RcvMmcSysInfoInd:INFO: MmLikeB is MM_TRUE");
            }
            else
            {
            }
        }
        ulNewCellId = pSysInfoInd->ulCellId;
        if (ulNewCellId != g_MmGlobalInfo.MsCsInfo.ulCellId)
        {
            g_MmGlobalInfo.MsCsInfo.ulCellId = ulNewCellId;
            g_MmGlobalInfo.MsCsInfo.ucCellChgFlg = MM_TRUE;
        }
        else
        {
            g_MmGlobalInfo.MsCsInfo.ucCellChgFlg = MM_FALSE;
        }

    }

    return ucRet;
}

/***********************************************************************
 *  MODULE   : Mm_RcvGmmCombinedAttachIni
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmCombinedAttachIni(
                                VOS_VOID            *pRcvMsg                    /* 收到的原语头指针                         */
                                )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmCombinedAttachAccept
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmCombinedAttachAccept(
                                    VOS_VOID            *pRcvMsg                /* 收到的原语头指针                         */
                                    )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_COMBINED_ATTACH_ACCEPTED_STRU     *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_COMBINED_ATTACH_ACCEPTED_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MM_CONST_NUM_1 < pstMsg->enAttachResult)
        || ((GMMMM_ATTACH_RESLUT_PS_ONLY == pstMsg->enAttachResult)
            && ((NAS_MML_REG_FAIL_CAUSE_NULL != pstMsg->enCause)
            && (NAS_MML_REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_HLR != pstMsg->enCause)
            && (NAS_MML_REG_FAIL_CAUSE_MSC_UNREACHABLE != pstMsg->enCause)
            && (NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE != pstMsg->enCause)
            && (NAS_MML_REG_FAIL_CAUSE_PROCEDURE_CONGESTION != pstMsg->enCause)))
			|| (MM_CONST_NUM_5 < pstMsg->ulAttachAttCounter))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvGmmCombinedAttachAccept:WARNING: MMCMM_COMBINED_ATTACH_ACCEPTED CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        if (GMMMM_ATTACH_RESLUT_PS_ONLY == pstMsg->enAttachResult)
        {
            g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pstMsg->enCause;
        }
        else if (GMMMM_ATTACH_RESLUT_COMBINED == pstMsg->enAttachResult)
        {
            g_MmGlobalInfo.ucRejectCause = NAS_MML_REG_FAIL_CAUSE_NULL;
        }
        else
        {

        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : NAS_MM_RcvGmmCombinedAttachRej
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 NAS_MM_RcvGmmCombinedAttachRej(
                                VOS_VOID            *pRcvMsg                    /* 收到的原语头指针                         */
                                )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_COMBINED_ATTACH_REJECTED_STRU     *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (GMMMM_COMBINED_ATTACH_REJECTED_STRU *)pRcvMsg;                      /* 消息转换                                 */

    if ((NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW != pMsg->enCause)
        && (NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN != pMsg->enCause))
    {
        g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pMsg->enCause;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}



/***********************************************************************
 *  MODULE   : Mm_RcvGmmImsiDetechIni
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmImsiDetechIni(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmImsiDetechComplete
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmImsiDetechComplete(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : NAS_MM_RcvGmmNetworkDetechInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 NAS_MM_RcvGmmNetworkDetechInd(
    VOS_VOID                           *pRcvMsg
)
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_NETWORK_DETACH_IND_STRU           *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_NETWORK_DETACH_IND_STRU *)pRcvMsg;                            /* 消息转换                                 */

    /*如果是网侧发起的IMSI detach，MM上报NAS_OM_EVENT_IMSI_DETACH_IND事件*/
    if (GMMMM_NET_DETACH_IMSI == pstMsg->enDetachType)
    {
        NAS_EventReport(WUEPS_PID_MM, NAS_OM_EVENT_IMSI_DETACH_IND,
                            VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
    }

    if ((GMMMM_NET_DETACH_IMSI != pstMsg->enDetachType)
        && (GMMMM_NET_DETACH_NOATTACH != pstMsg->enDetachType)
        && (GMMMM_NET_DETACH_REATTACH != pstMsg->enDetachType))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "NAS_MM_RcvGmmNetworkDetechInd:WARNING: MMCMM_NETWORK_DETACH_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        ucRet = MM_TRUE;
        if ((NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW != pstMsg->ulDetachCause)
            && (NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN != pstMsg->ulDetachCause))
        {
            g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pstMsg->ulDetachCause;
        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmGmmActionResultInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmGmmActionResultInd(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_GMM_ACTION_RESULT_IND_STRU        *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_GMM_ACTION_RESULT_IND_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MM_CONST_NUM_3 < pstMsg->enGmmActionType))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvGmmGmmActionResultInd:WARNING: MMCMM_GMM_ACTION_RESULT_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        ucRet = MM_TRUE;
        if ((NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW != pstMsg->enCause)
            && (NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN != pstMsg->enCause))
        {
            g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pstMsg->enCause;
        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmAuthFailInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmAuthFailInd(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmCombinedRauInitiation
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmCombinedRauInitiation(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                             /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmCombinedRauAccepted
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmCombinedRauAccepted(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_COMBINED_RAU_ACCEPTED_STRU        *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_COMBINED_RAU_ACCEPTED_STRU *)pRcvMsg;                         /* 消息转换                                 */

    if (GMMMM_RAU_RESULT_PS_ONLY == pstMsg->enRauResult)
    {
        g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pstMsg->enCause;
    }
    else if (GMMMM_RAU_RESULT_COMBINED == pstMsg->enRauResult)
    {
        g_MmGlobalInfo.ucRejectCause = NAS_MML_REG_FAIL_CAUSE_NULL;
    }
    else
    {

    }
    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvGmmCombinedRauRejected
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmCombinedRauRejected(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_COMBINED_RAU_REJECTED_STRU        *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_COMBINED_RAU_REJECTED_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW != pstMsg->enCause)
        && (NAS_MML_REG_FAIL_CAUSE_GPRS_SERV_NOT_ALLOW_IN_PLMN != pstMsg->enCause))
    {
        g_MmGlobalInfo.ucRejectCause = (VOS_UINT8)pstMsg->enCause;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}



/***********************************************************************
 *  MODULE   : Mm_RcvMmcCoverageLostInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvMmcCoverageLostInd(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    g_MmGlobalInfo.ucSysInfoFlg = MM_FALSE;

    return MM_TRUE;                                                             /* 返回检查结果                             */
}



VOS_UINT8 Mm_RcvCcEstReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet;                             /* 返回值                                   */
    MMCC_EST_REQ_STRU                       *pMsg;                              /* 定义接收到的消息类型指针                 */
    pMsg = (MMCC_EST_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    ucRet = MM_TRUE;

    if ( ( MM_CONST_NUM_0 == pMsg->ulCallType )
      || ( MM_CONST_NUM_2 < pMsg->ulCallType )
      || ( MM_CONST_NUM_7 < pMsg->ulCallPri )
      || ( MMCC_CALL_MODE_OTHER < pMsg->ulCallMode ) )
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvCcEstReq:WARNING: MMCC_EST_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmCcEstReq.ulCallPri       = pMsg->ulCallPri;
        g_MmCcEstReq.ulCallType      = pMsg->ulCallType;
        g_MmCcEstReq.ulTransactionId = pMsg->ulTransactionId;
        g_MmCcEstReq.ulCallMode = pMsg->ulCallMode;
        if (MMCC_EMERGENCY_CALL == g_MmCcEstReq.ulCallType)
        {                                                                       /* 建立紧急呼叫                             */
            g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucEstingCallTypeFlg =
                MM_TRUE;          /* 设置是紧急呼叫的标志                     */

            NAS_MML_SetCsEmergencyServiceFlg( VOS_TRUE );
        }
        else
        {                                                                       /* 建立正常呼叫                             */
            g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucEstingCallTypeFlg =
                MM_FALSE;         /* 设置是正常呼叫的标志                     */

            NAS_MML_SetCsEmergencyServiceFlg( VOS_FALSE );
        }

        /* 复位呼叫重建次数 */
        g_MmGlobalInfo.ucCallRetryCount = 0;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvCcRelReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      03-12-09  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvCcRelReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMCC_REL_REQ_STRU                       *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMCC_REL_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (((MMCC_RELEASE_ALL != pMsg->ulReleaseType)
        && (MMCC_RELEASE_SPECIFIC != pMsg->ulReleaseType))
        || ((MMCC_RELEASE_SPECIFIC == pMsg->ulReleaseType)
            && (MM_CONST_NUM_14 < pMsg->ulTransactionId)))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvCcRelReq:WARNING: MMCC_REL_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmCcRelReq.ulReleaseType   = pMsg->ulReleaseType;
        g_MmCcRelReq.ulTransactionId = pMsg->ulTransactionId;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvCcAbortReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.11    新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvCcAbortReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                     ucRet = MM_TRUE;              /* 返回值                                   */
    MMCC_ABORT_REQ_STRU                          *pMsg  = VOS_NULL_PTR;         /* 定义接收到的消息类型指针                 */

    pMsg = (MMCC_ABORT_REQ_STRU *)pRcvMsg;

    if ((pMsg->ulTransactionId > 14) || (pMsg->ulTransactionId < 8))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvCcAbortReq:WARNING: MMCC_ABORT_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmCcAbortReq.ulTransactionId = pMsg->ulTransactionId;

        /* CC中止呼叫，设置不需要进行重试 */
        g_MmGlobalInfo.ucCallRetryCount = MM_CALL_RETRY_MAX_TIMES;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvCcDataReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.11  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvCcDataReq(
                   VOS_VOID            *pRcvMsg                                 /* 收到的原语头指针                         */
                   )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMCC_DATA_REQ_STRU                      *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMCC_DATA_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (MM_CONST_NUM_15 < pMsg->ulTransactionId)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvCcDataReq:WARNING: MMCC_DATA_REQ CHECK PRIMITIVE ERROR!");
    }
    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvCcReestReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.11  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvCcReestReq(
                        VOS_VOID             *pRcvMsg                           /* 收到的原语头指针                         */
                        )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    MMCC_REEST_REQ_STRU                      *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    pMsg = (MMCC_REEST_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MM_CONST_NUM_14 < pMsg->ulTransactionId)
        || (MM_CONST_NUM_7 == pMsg->ulTransactionId))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvCcReestReq:WARNING: MMCC_REEST_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmCcReestReq.ulTransactionId  = pMsg->ulTransactionId;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvRrcPagingInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.11    新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvRrcPagingInd(
                        VOS_VOID             *pRcvMsg                           /* 收到的原语头指针                         */
                        )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    RRMM_PAGING_IND_STRU                     *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    MSG_HEADER_STRU                          *pMsgHeader;

    pMsgHeader = (MSG_HEADER_STRU*)pRcvMsg;

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        /* 判断是否为 GSM 的寻呼消息 */
        if (UEPS_PID_GAS != pMsgHeader->ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcPagingInd: WARNING: Receive WAS Page Ind in GSM!");

            return MM_FALSE;
        }

        /* 调用函数 MM_RcvGsmRrcPagingInd 处理 */
        ucRet = MM_RcvGsmRrcPagingInd(pRcvMsg);

        return ucRet;
    }
    else
    {
        /* 判断是否为 WCDMA 的寻呼消息 */
        if (WUEPS_PID_WRR != pMsgHeader->ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcPagingInd: WARNING: Receive GSM Page Ind in WCDMA!");

            return MM_FALSE;
        }
    }

    pMsg = (RRMM_PAGING_IND_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((RRC_NAS_CS_DOMAIN != pMsg->ulCnDomainId)
        || (MM_CONST_NUM_1 < pMsg->ulPagingType)
        || (MM_CONST_NUM_6 < pMsg->ulPagingCause)
        || (MM_CONST_NUM_4 < pMsg->ulPagingUeId))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvRrcPagingInd:WARNING: RRMM_PAGING_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmGlobalInfo.ucPagingCause = (VOS_UINT8)pMsg->ulPagingCause;          /* 设置寻呼原因值                           */

        if ((RRC_P_TMSI_GSM_MAP == pMsg->ulPagingUeId)
            || (RRC_TMSI_GSM_MAP_P_TMSI == pMsg->ulPagingUeId))
        {
            g_MmGlobalInfo.ucPagingRecordTypeId = MM_TMSI_PAGING;
        }
        else if (RRC_IMSI_GSM_MAP == pMsg->ulPagingUeId)
        {
            g_MmGlobalInfo.ucPagingRecordTypeId = MM_IMSI_PAGING;
        }
        else
        {
            ucRet = MM_FALSE;
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvRrcPagingInd:WARNING: RRMM_PAGING_IND CHECK PRIMITIVE ERROR!");
        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvRrcEstCnf(
                        VOS_VOID            *pRcvMsg                            /* 收到的原语头指针                         */
                        )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    RRMM_EST_CNF_STRU                       *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */
    NAS_MML_CONN_STATUS_INFO_STRU           *pstConnStatus;

    pstConnStatus       = NAS_MML_GetConnStatus();

    pMsg = (RRMM_EST_CNF_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {                                                                           /* 当前在 GSM 网中                          */
        if (UEPS_PID_GAS != pMsg->MsgHeader.ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcEstCnf: WARNING: Receive WAS message in GSM.");
            return MM_FALSE;
        }
    }
    else
    {
        if (WUEPS_PID_WRR != pMsg->MsgHeader.ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcEstCnf: WARNING: Receive GAS message in WCDMA.");
            return MM_FALSE;
        }
    }

    if (RRC_NAS_CS_DOMAIN != pMsg->ulCnDomainId)
    {                                                                           /* 不是CS域消息                             */
        NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                "Mm_RcvRrcEstCnf: WARNING: Not receive CS domain message.");
        return MM_FALSE;
    }

    if (g_MmGlobalInfo.ucRrEstReqOpid != pMsg->ulOpId)
    {                                                                           /* OP ID不相等                              */
        NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                "Mm_RcvRrcEstCnf: WARNING: OPID is wrong.");
        return MM_FALSE;
    }

    if (   (RRC_EST_SUCCESS == pMsg->ulResult)
        || (RRC_EST_PRESENT == pMsg->ulResult))
    {
        NAS_MM_SndGmmCsConnectInd(MMGMM_CS_CONNECT_EXIST);

        /* 24.008, 4.4.4.9, case f1, CR1099 update:
          更新RRC链路状态 */
        pstConnStatus->ucCsSigConnStatusFlg     = VOS_TRUE;
        pstConnStatus->ucRrcStatusFlg           = VOS_TRUE;
    }
    else
    {
        /* 建链失败，清除MM 链路控制变量 */
        NAS_MM_ClearMmLinkCtrlStru();

        NAS_MML_SetCsSigConnStatusFlg(VOS_FALSE);

        /* 在LU过程中,random access rej时,需要再发起一次LU建链过程,在第二次建链失败时,清除CSFB标志
           其它原因值直接清除CSFB标志*/
#if (FEATURE_ON == FEATURE_LTE)
        if (( (NAS_MML_NET_RAT_TYPE_GSM     == NAS_MML_GetCurrNetRatType())
          && (RRC_EST_RANDOM_ACCESS_REJECT == pMsg->ulResult)
          && (g_MmGlobalInfo.LuInfo.ucT3213AttmptCnt >= MM_CONST_NUM_1)
          && (WAIT_FOR_RR_CONNECTION_LOCATION_UPDATING == g_MmGlobalInfo.ucState))
         || (RRC_EST_RANDOM_ACCESS_REJECT != pMsg->ulResult))
        {
            NAS_MML_SetCsfbServiceStatus(NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);

            NAS_MML_SetRelCauseCsfbHighPrioFlg(VOS_FALSE);   
        }
#endif

    }

    return ucRet;                                                               /* 返回检查结果                             */
}
VOS_UINT8 Mm_RcvRrcRelInd(
                        VOS_VOID            *pRcvMsg                            /* 收到的原语头指针                         */
                        )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    RRMM_REL_IND_STRU                        *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */
    NAS_MML_CONN_STATUS_INFO_STRU       *pstConnStatus;

    pstConnStatus   = NAS_MML_GetConnStatus();

    pMsg = (RRMM_REL_IND_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((RRC_NAS_CS_DOMAIN != pMsg->ulCnDomainId)
        || (MM_CONST_NUM_1 < pMsg->ulRrcConnStatus)
        || (RRC_REL_CAUSE_BUTT <= pMsg->ulRelCause))
    {                                                                           /* 消息中的取值不合法                       */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvRrcRelInd:WARNING: RRMM_REL_IND CHECK PRIMITIVE ERROR!");
        return MM_FALSE;
    }
    
    /* 链路释放，清除MM 链路控制变量，并更新CS 信令连接状态 */
    pstConnStatus->ucCsSigConnStatusFlg = VOS_FALSE;
    if (RRC_RRC_CONN_STATUS_ABSENT == pMsg->ulRrcConnStatus)
    {
        pstConnStatus->ucRrcStatusFlg   = VOS_FALSE;
    }
    
#if (FEATURE_ON == FEATURE_LTE)

    /* 若当前存在CSFB标志，则清除CSFB标志 */
    if (VOS_TRUE == NAS_MML_IsCsfbServiceStatusExist())
    {        
        NAS_MML_SetCsfbServiceStatus(NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);
        
        NAS_MML_SetRelCauseCsfbHighPrioFlg(VOS_FALSE);   
    }
#endif

    /* 链路释放后，原有链路上GAS上报的Gas_RR_CHAN_IND消息失效 */
    g_MmGlobalInfo.ucGasRrChanIndMsgValidFlg = VOS_FALSE;

    /* 以下状态在判断是否需要重试流程后再清除MM链路控制变量 */
    if (   (WAIT_FOR_OUTGOING_MM_CONNECTION != g_MmGlobalInfo.ucState)
        && (WAIT_FOR_RR_CONNECTION_LOCATION_UPDATING != g_MmGlobalInfo.ucState)
        && (LOCATION_UPDATING_INITIATED != g_MmGlobalInfo.ucState))
    {
        NAS_MM_ClearMmLinkCtrlStru();
    }

    g_MmSubLyrShare.MmShare.ucCsIntegrityProtect  = NAS_MML_RRC_INTEGRITY_PROTECT_DEACTIVE;

    g_MmGlobalInfo.ucCsSigConnFlg = MM_CS_SIG_CONN_ABSENT;
    PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_INFO, "Mm_RcvRrcRelInd:INFO: MmCsSigConnFlg is MM_CS_SIG_CONN_ABSENT");

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        MM_SndMmcRrRelInd(pMsg->ulRelCause);
    }
    return ucRet;                                                               /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvRrcSecurityInd(
                           VOS_VOID            *pRcvMsg                         /* 收到的原语头指针                         */
                           )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    RRMM_SECURITY_IND_STRU                   *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    pMsg = (RRMM_SECURITY_IND_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        /* 判断是否为 GSM 的同步指示消息 */
        if (UEPS_PID_GAS!= pMsg->MsgHeader.ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcSecurityInd: WARNING: Receive WAS Security Ind in GSM!");

            return MM_FALSE;
        }

        ucRet = MM_RcvGsmRrcSyncInd(pRcvMsg);

        return ucRet;

    }
    else
    {
        /* 判断是否为 WCDMA 的同步指示消息 */
        if (WUEPS_PID_WRR != pMsg->MsgHeader.ulSenderPid)
        {
            NAS_LOG(WUEPS_PID_MM, MM_GSMDIFMSG, PS_PRINT_WARNING,
                    "Mm_RcvRrcSecurityInd: WARNING: Receive GAS Sync in WCDMA!");

            return MM_FALSE;
        }
    }

    if (RRC_SECURITY_MOBI_MSG < pMsg->ulRptMode)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvRrcSecurityInd:WARNING: RRMM_SECURITY_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */

        g_MmGlobalInfo.stMmLinkCtrl.ucSmcOrFirstNasMsgRcvdFlg = VOS_TRUE;       /* 当前CS信令链路已收到SMC消息 */

        g_MmGlobalInfo.CsSecutityInfo.ucSecurityType = (VOS_UINT8)pMsg->ulRptMode;

        if ((RRC_SECURITY_DEL_CS_KEY == pMsg->ulRptMode)
            || (RRC_SECURITY_DEL_CS_PS_KEY == pMsg->ulRptMode))
        {
            NAS_MML_InitCsSecurityUmtsCkInvalid();
            NAS_MML_InitCsSecurityUmtsIkInvalid();

            NAS_MML_SetSimCsSecurityCksn(MM_CKSN_INVALID);
        }
        else
        {
            if ( RRC_SECURITY_CS_SMC_RCV == pMsg->ulRptMode )
            {
                g_MmSubLyrShare.MmShare.ucCsIntegrityProtect = NAS_MML_RRC_INTEGRITY_PROTECT_ACTIVE;
                Mm_TimerStop(MM_TIMER_T3218);                                   /* 停T3218                                  */
                Mm_ComDelRandRes();                                             /* 删除RAND和RES                            */
            }
            #if 0
            else if(RRC_SECURITY_CS_SMC_CMPL == pMsg->ulRptMode)
            {
                g_MmGlobalInfo.CsSecutityInfo.ucSecutityFlg =
                    (VOS_UINT8)pMsg->ulCsCipherStatus;                              /* 设置加密是否启动                         */
              /*  if ( RRC_INTEG_PROTECT_NOCHG != pMsg->ulIntegCmd ) */
                if ( RRC_INTEG_PROTECT_CS_START == pMsg->ulIntegCmd )
                {                                                               /* 完整性保护状态改变                       */
                    g_MmGlobalInfo.CsSecutityInfo.ucIntegFlg =
                                                    MM_INTEG_PROTECT_START;     /* 完整性保护启动                           */
                }
            }
            #endif
        }
        #if 0
        if (RRC_CIPHER_START == g_MmGlobalInfo.CsSecutityInfo.ucSecutityFlg)
        {
            if ( RRC_NAS_CIPHER_ALGO_UEA0 == pMsg->ulCsCipherAlgor )
            {
                g_MmGlobalInfo.ucCsCipherAlgor = NAS_MM_CIPHER_ALGO_UEA0;
            }
            else if ( RRC_NAS_CIPHER_ALGO_UEA1 == pMsg->ulCsCipherAlgor )
            {
                g_MmGlobalInfo.ucCsCipherAlgor = NAS_MM_CIPHER_ALGO_UEA1;
            }
            else
            {

            }
        }
        #endif
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvRrcSyncInd(
                       VOS_VOID            *pRcvMsg
                       )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    VOS_UINT8                                 i;
    RRMM_SYNC_IND_STRU                       *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */
    pMsg = (RRMM_SYNC_IND_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if  (RRC_NAS_MAX_RAB_SETUP < pMsg->ulRabCnt)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvRrcSyncInd:WARNING: RRMM_SYNC_IND CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        PS_MEM_SET(&g_RrMmSyncInd,0,sizeof(g_RrMmSyncInd));

        g_RrMmSyncInd.ulRabCnt = pMsg->ulRabCnt;

        for (i=0; i < pMsg->ulRabCnt; i++)
        {
            g_RrMmSyncInd.NasSyncInfo[i].ulRabId       = pMsg->NasSyncInfo[i].ulRabId;
            g_RrMmSyncInd.NasSyncInfo[i].ulCnDomainId  = pMsg->NasSyncInfo[i].ulCnDomainId;
            g_RrMmSyncInd.NasSyncInfo[i].ulRabSyncInfo = (VOS_UINT32)(pMsg->NasSyncInfo[i].ucRabSyncInfo);
        }

        /* 此处Amr no change发送到CC时会转换成release，CC新开发代码中需要修改 */
        g_RrMmSyncInd.ulReason      = pMsg->enAmrModeCfgType + 4;

        if ( g_RrMmSyncInd.ulRabCnt > 0 )
        {
            if (NAS_MM_SYNC_INFO_CODEC_AMR == (pMsg->NasSyncInfo[0].ucRabSyncInfo))
            {
                g_RrMmSyncInd.ulChannelMode = MMCC_CHAN_MOD_AMR;
            }
            else if ( NAS_MM_SYNC_INFO_CODEC_AMR2 == (pMsg->NasSyncInfo[0].ucRabSyncInfo) )
            {
                g_RrMmSyncInd.ulChannelMode = MMCC_CHAN_MOD_AMR2;
            }
            else if ( NAS_MM_SYNC_INFO_CODEC_AMRWB == (pMsg->NasSyncInfo[0].ucRabSyncInfo) )
            {
                g_RrMmSyncInd.ulChannelMode = MMCC_CHAN_MOD_AMRWB;
            }
            else
            {
                g_RrMmSyncInd.ulChannelMode = MMCC_CHAN_MOD_AMR2;
            }

        }
        else
        {
            g_RrMmSyncInd.ulChannelMode = MMCC_CHAN_MOD_AMR2;
        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvRrcDataInd
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.12  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvRrcDataInd(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                               ucRet = MM_TRUE;                    /* 返回值                                   */

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : NAS_MM_RcvGmmGprsDetechIni
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.12  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 NAS_MM_RcvGmmGprsDetechIni(
                             VOS_VOID            *pRcvMsg                       /* 收到的原语头指针                         */
                            )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    GMMMM_GPRS_DETACH_INITIATION_STRU        *pstMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_GPRS_DETACH_INITIATION_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((GMMMM_COMBINED_DETACH_NORMAL != pstMsg->enDetachType)
        && (GMMMM_GPRS_DETACH_NORMAL != pstMsg->enDetachType))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "NAS_MM_RcvGmmGprsDetechIni:WARNING: MMCMM_GPRS_DETACH_INITIATION CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        ucRet = MM_TRUE;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvGmmGprsDetechCmpl
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇    2003.12.12  新版作成
 ************************************************************************/
VOS_UINT8 Mm_RcvGmmGprsDetechCmpl(
                          VOS_VOID            *pRcvMsg                          /* 收到的原语头指针                         */
                          )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */

    return ucRet;                                                               /* 返回检查结果                             */
}
VOS_UINT8   Mm_RcvAgntUsimAuthenticationCnf(
                                         VOS_VOID     *pMsg                     /* 当前处理的消息                           */
                                        )
{
    VOS_UINT8                               ucRet = MM_TRUE;                    /* 返回结果初始化为成功                     */

    if (MM_RCV_AUTH_CNF_FLG
        != (MM_RCV_AUTH_CNF_FLG & g_MmGlobalInfo.ucRcvAgentFlg))
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvAgntUsimAuthenticationCnf:WARNING: g_MmGlobalInfo.ucRcvAgentFlg Abnormal");
        return MM_FALSE;
    }
    else
    {
        g_MmGlobalInfo.ucRcvAgentFlg &= ~MM_RCV_AUTH_CNF_FLG;
    }

    Mm_Com_UsimAuthenticationCnfChgFormat((USIMM_AUTH_CNF_STRU *)pMsg);

    if (AGENT_AUTH_RST_SUCCESS == g_AgentUsimAuthCnf.ucCheckRst)
    {
        NAS_MML_SetSimCsSecurityUmtsCk(g_AgentUsimAuthCnf.aucCipheringKey);
        NAS_MML_SetSimCsSecurityUmtsIk(g_AgentUsimAuthCnf.aucIntegrityKey);

        PS_MEM_CPY(g_MmGlobalInfo.AuthenCtrlInfo.aucRes,
                    g_AgentUsimAuthCnf.aucResponse, 4);

        /* 设置 RES 存在标志 */
        g_MmGlobalInfo.AuthenCtrlInfo.ucResFlg = MM_AUTHEN_RES_PRESENT;

        if (MM_CONST_NUM_0 == g_AgentUsimAuthCnf.ucKcLength)                    /* g_AgentUsimAuthCnf.ucKcLength 为零 */
        {
            /* 调用函数 NAS_MML_SecContext3GTO2G 求的 KC */
            NAS_MML_SecContext3GTO2G(NAS_MML_GetSimCsSecurityUmtsIk(),
                                     NAS_MML_GetSimCsSecurityUmtsCk(),
                                     NAS_MML_GetSimCsSecurityGsmKc());

        }
        else
        {
            /* 将 KC 保存到全局变量 */
            NAS_MML_SetSimCsSecurityGsmKc(g_AgentUsimAuthCnf.aucKc);

        }

        if (0 != g_AgentUsimAuthCnf.ucExtRspLength)
        {
            g_MmGlobalInfo.AuthenCtrlInfo.ucExtRspLength =
                               g_AgentUsimAuthCnf.ucExtRspLength;
            if (MM_MAX_RES_EXT_LEN <
                g_MmGlobalInfo.AuthenCtrlInfo.ucExtRspLength)
            {
                 g_MmGlobalInfo.AuthenCtrlInfo.ucExtRspLength = MM_MAX_RES_EXT_LEN;
            }
            PS_MEM_CPY(g_MmGlobalInfo.AuthenCtrlInfo.aucExtRes,
                        g_AgentUsimAuthCnf.aucExtResponse,
                        g_MmGlobalInfo.AuthenCtrlInfo.ucExtRspLength);          /* 保存Extern RES                           */
        }
        else
        {
            g_MmGlobalInfo.AuthenCtrlInfo.ucExtRspLength = 0;
        }
    }
    else
    {
        if (AGENT_AUTH_FAIL_SQN == g_AgentUsimAuthCnf.ucFailureCause)
        {
            g_MmGlobalInfo.AuthenCtrlInfo.ucFailParaLength =
                g_AgentUsimAuthCnf.ucFailParaLength;
            if (0 != g_MmGlobalInfo.AuthenCtrlInfo.ucFailParaLength)
            {
                if (14 < g_MmGlobalInfo.AuthenCtrlInfo.ucFailParaLength)
                {
                    g_MmGlobalInfo.AuthenCtrlInfo.ucFailParaLength = 14;
                }
                PS_MEM_CPY(g_MmGlobalInfo.AuthenCtrlInfo.aucFailPara,
                            g_AgentUsimAuthCnf.aucFailPara,
                            g_MmGlobalInfo.AuthenCtrlInfo.ucFailParaLength);
            }
        }
    }

    return ucRet;
}


VOS_UINT8 Mm_RcvMmcDetachReq(
                         VOS_VOID            *pRcvMsg                           /* 收到的原语头指针                         */
                         )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    MMCMM_DETACH_REQ_STRU                    *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    pMsg = (MMCMM_DETACH_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MMC_MM_PS_DETACH != pMsg->enDetachCause)
        && (MMC_MM_CS_DETACH != pMsg->enDetachCause)
        && (MMC_MM_PS_CS_DETACH != pMsg->enDetachCause))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvMmcDetachReq:WARNING: MMCMM_DETACH_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        ucRet = MM_TRUE;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvMmcPowerOffReq(
                         VOS_VOID            *pRcvMsg                           /* 收到的原语头指针                         */
                         )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    VOS_UINT8                                i;
    NAS_MML_EMERGENCY_NUM_LIST_STRU         *pstEmergencyNumList;
    pstEmergencyNumList     = NAS_MML_GetEmergencyNumList();

    Mm_TimerStop(MM_TIMER_STOP_ALL);

    pstEmergencyNumList->ulMcc = 0xFFFFFFFF;

    for (i = 0; i < pstEmergencyNumList->ucEmergencyNumber; i++)
    {
        PS_MEM_SET(&pstEmergencyNumList->aucEmergencyList[i],
                    0xFF,
                    sizeof(NAS_MML_EMERGENCY_NUM_STRU));
    }

    pstEmergencyNumList->ucEmergencyNumber = 0;


    return ucRet;                                                               /* 返回检查结果                             */
}
/***********************************************************************
 *  MODULE   : Mm_RcvSsEstReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSsEstReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSS_EST_REQ_STRU                       *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */
    pMsg = (MMSS_EST_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (MM_CONST_NUM_15 < pMsg->ulTi)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSsEstReq:WARNING: MMSS_EST_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSsEstReq.ulTi = pMsg->ulTi;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvSsRelReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSsRelReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSS_REL_REQ_STRU                       *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMSS_REL_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MM_CONST_NUM_14 < pMsg->ulTi)
        || (MM_CONST_NUM_7 == pMsg->ulTi))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSsRelReq:WARNING: MMSS_REL_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSsRelReq.ulTi = pMsg->ulTi;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvSsDataReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSsDataReq(
                   VOS_VOID            *pRcvMsg                                 /* 收到的原语头指针                         */
                   )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSS_DATA_REQ_STRU                      *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMSS_DATA_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (MM_CONST_NUM_15 < pMsg->ulTi)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSsDataReq:WARNING: MMSS_DATA_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */

    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvSmsEstReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSmsEstReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSMS_EST_REQ_STRU                      *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMSMS_EST_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (MM_CONST_NUM_15 < pMsg->ulTi)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSmsEstReq:WARNING: MMSMS_EST_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSmsEstReq.ulEstCause = pMsg->ulEstCause;
        g_MmSmsEstReq.ulTi       = pMsg->ulTi;
    }

    g_MmGlobalInfo.ucPagingType = MM_MO_PAGING;

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvSmsRelReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSmsRelReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSMS_REL_REQ_STRU                      *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMSMS_REL_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if ((MM_CONST_NUM_14 < pMsg->ulTi)
        || (MM_CONST_NUM_7 == pMsg->ulTi))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSmsRelReq:WARNING: MMSMS_REL_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSmsRelReq.ulTi = pMsg->ulTi;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


/***********************************************************************
 *  MODULE   : Mm_RcvSmsDataReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-09  新版作成
 *     2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSmsDataReq(
                   VOS_VOID            *pRcvMsg                                 /* 收到的原语头指针                         */
                   )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSMS_DATA_REQ_STRU                     *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMSMS_DATA_REQ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    if (MM_CONST_NUM_15 < pMsg->ulTi)
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSmsDataReq:WARNING: MMSMS_DATA_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */

    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvCcPromptRej
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-10  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvCcPromptRej(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMCC_PROMPT_REJ_STRU                    *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pMsg = (MMCC_PROMPT_REJ_STRU *)pRcvMsg;                                                             /* 消息转换                                 */

    g_MmCcPromptRej.ulRefuseCause = pMsg->ulRefuseCause;
    
    return ucRet;                                                               /* 返回检查结果                             */
}
VOS_UINT8 Mm_RcvMmcAttachReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                           ucRet = MM_TRUE;                   /* 返回值                                   */
    MMCMM_ATTACH_REQ_STRU              *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    VOS_UINT8                           ucSimPsRegStatus;
    VOS_UINT8                           ucPsAttachAllow;

    ucSimPsRegStatus = NAS_MML_GetSimPsRegStatus();
    ucPsAttachAllow  = NAS_MML_GetPsAttachAllowFlg();

    pMsg = (MMCMM_ATTACH_REQ_STRU*)pRcvMsg;                                     /* 消息转换                                 */

    if ((MMC_MM_ATTACH_TYPE_GPRS != pMsg->enAttachType)
        && (MMC_MM_ATTACH_TYPE_IMSI != pMsg->enAttachType)
        && (MMC_MM_ATTACH_TYPE_GPRS_IMSI != pMsg->enAttachType))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvMmcAttachReq:WARNING: MMCMM_ATTACH_REQ CHECK PRIMITIVE ERROR!");
    }

    if (MM_TRUE == g_MmGlobalInfo.ucSysInfoFlg)
    {
        if ((VOS_TRUE == NAS_MML_GetPsAttachAllowFlg())
         && (MM_NET_MODE_I == g_MmGlobalInfo.ucNtMod))
        {
            if ((VOS_TRUE == ucSimPsRegStatus)
                && (MM_UNSUPPORT_GPRS != g_MmGlobalInfo.usRac))
            {
                if((VOS_TRUE == ucPsAttachAllow)
                || (MMC_MM_ATTACH_TYPE_GPRS_IMSI == pMsg->enAttachType))
                {
                    g_MmGlobalInfo.ucLikeB = MM_FALSE;
                    PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_INFO, "Mm_RcvMmcAttachReq:INFO: MmLikeB is MM_FALSE");
                }
            }
        }
    }

    return ucRet;                                                               /* 返回检查结果                             */
}




VOS_UINT8 Mm_RcvMmcRelReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    return MM_TRUE;                                                                   /* 返回检查结果                             */
}
/***********************************************************************
 *  MODULE   : Mm_RcvMmcModeChngReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇      04-03-10  新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvMmcModeChngReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMCMM_MODE_CHANGE_REQ_STRU              *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */
    pMsg = (MMCMM_MODE_CHANGE_REQ_STRU*)pRcvMsg;                                /* 消息转换                                 */

    if ( (NAS_MML_MS_MODE_NULL != pMsg->enMsMode)
      && (NAS_MML_MS_MODE_PS_CS != pMsg->enMsMode)
      && (NAS_MML_MS_MODE_PS_ONLY != pMsg->enMsMode)
      && (NAS_MML_MS_MODE_CS_ONLY != pMsg->enMsMode) )
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvMmcModeChngReq:WARNING: MMCMM_MODE_CHANGE_REQ CHECK PRIMITIVE ERROR!");
    }
    return ucRet;                                                               /* 返回检查结果                             */
}
VOS_UINT8 Mm_RcvGmmLuIni(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    GMMMM_LU_INITIATION_STRU                *pstMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */

    pstMsg = (GMMMM_LU_INITIATION_STRU*)pRcvMsg;                                  /* 消息转换                                 */

#if (FEATURE_ON == FEATURE_LTE)
    if ( NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        return MM_FALSE;
    }
#endif

    if ((GMMMM_NO_TIMER_EXPIRED != pstMsg->enTimerExpiredFlg)
     && (GMMMM_T3312_EXPIRED != pstMsg->enTimerExpiredFlg)
     && (GMMMM_T3311_EXPIRED != pstMsg->enTimerExpiredFlg)
     && (GMMMM_T3302_EXPIRED != pstMsg->enTimerExpiredFlg)
     && (GMMMM_T3330_EXPIRED != pstMsg->enTimerExpiredFlg)
     && (GMMMM_T3412OrT3423_EXPIRED != pstMsg->enTimerExpiredFlg))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvGmmLuIni:WARNING: MMCMM_LU_INITIATION CHECK PRIMITIVE ERROR!");
    }
    return ucRet;                                                               /* 返回检查结果                             */
}


VOS_UINT8 Mm_RcvRrcAcInfoChgInd(
                            VOS_VOID            *pRcvMsg                        /* 收到的原语头指针                         */
                            )
{
    /* USIM卡禁止GSM接入时，不允许注册和寻呼业务,此处直接返回MM_FALSE，
       防止Mm_Cell_S6_E74中再设置成注册和寻呼不受限 */
    if (VOS_TRUE == NAS_MML_GetGsmForbidFlg())
    {
        return MM_FALSE;
    }
    return MM_TRUE;                                                             /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvMmAgentInquire
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇     2005.01.28  Agent对应
 ************************************************************************/
VOS_UINT8 Mm_RcvMmAgentInquire(
                            VOS_VOID            *pRcvMsg                        /* 收到的原语头指针                         */
                            )
{
    /*Mm_SndAgentInfo();*/
    return MM_TRUE;                                                             /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvSsAbortReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇   2005.02.01    新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSsAbortReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                ucRet = MM_TRUE;                   /* 返回值                                   */
    MMSS_ABORT_REQ_STRU                     *pMsg  = VOS_NULL_PTR;              /* 定义接收到的消息类型指针                 */
    pMsg = (MMSS_ABORT_REQ_STRU *)pRcvMsg;                                      /* 消息转换                                 */

    if ((pMsg->ulTransactionId > 14) || (pMsg->ulTransactionId < 8))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSsAbortReq:WARNING: MMSS_ABORT_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSsAbortReq.ulTransactionId = pMsg->ulTransactionId;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}

/***********************************************************************
 *  MODULE   : Mm_RcvSmsAbortReq
 *  FUNCTION : 消息检查
 *  INPUT    : VOS_VOID            *pRcvMsg  收到的原语头指针
 *  OUTPUT   : VOS_VOID
 *  RETURN   : VOS_UINT8            消息检查结果
 *  NOTE     :
 *  HISTORY  :
 *     1.  张志勇   2005.02.01    新版作成
       2.日    期  : 2006年12月4日
         作    者  : luojian id:60022475
         修改内容  : 问题单号:A32D07777
 ************************************************************************/
VOS_UINT8 Mm_RcvSmsAbortReq(
                       VOS_VOID            *pRcvMsg                             /* 收到的原语头指针                         */
                       )
{
    VOS_UINT8                                 ucRet = MM_TRUE;                  /* 返回值                                   */
    MMSMS_ABORT_REQ_STRU                     *pMsg  = VOS_NULL_PTR;             /* 定义接收到的消息类型指针                 */

    pMsg = (MMSMS_ABORT_REQ_STRU *)pRcvMsg;                                     /* 消息转换                                 */

    if ((pMsg->ulTransactionId > 14) || (pMsg->ulTransactionId < 8))
    {                                                                           /* 消息中的取值不合法                       */
        ucRet = MM_FALSE;                                                       /* 返回消息检查失败                         */
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_RcvSmsAbortReq:WARNING: MMSMS_ABORT_REQ CHECK PRIMITIVE ERROR!");
    }
    else
    {                                                                           /* 消息中的取值合法，存储消息               */
        g_MmSmsAbortReq.ulTransactionId = pMsg->ulTransactionId;
    }

    return ucRet;                                                               /* 返回检查结果                             */
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

