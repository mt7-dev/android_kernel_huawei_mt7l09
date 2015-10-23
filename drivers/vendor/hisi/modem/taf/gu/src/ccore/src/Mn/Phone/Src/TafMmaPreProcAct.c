

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "PsTypeDef.h"
#include "PsCommonDef.h"
#include "NasComm.h"
#include "NasCommDef.h"
#include "TafMmaCtx.h"
#include "TafSdcCtx.h"
#include "MmaAppLocal.h"
#include "Taf_Status.h"
#include "TafMmaSndImsa.h"
#include "NasUsimmApi.h"
#include "msp_nvim.h"
#include "TafApsProcNvim.h"
#include "TafMmaFsmPhoneModeTbl.h"
#include "TafMmaPreProcAct.h"
#include "TafMmaSndImsa.h"
#include "TafLog.h"
#include "TafSdcLib.h"
#include "TafMmaSndInternalMsg.h"
#include "TafMmaSndTaf.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "ImsaMmaInterface.h"
#endif

#include "TafMmaSndApp.h"
#include "TafMmaProcNvim.h"
#include "TafMmaMntn.h"
#include "TafMmaFsmPhoneMode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_PREPROC_ACT_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
extern MMA_ME_PERSONALISATION_STATUS_STRU       g_stMmaMePersonalisationStatus;
extern VOS_UINT8                                g_ucMmaOmConnectFlg ;
extern MMA_TIMER_ST                             g_stPhPhResetTimer;
extern STATUS_CONTEXT_STRU                      g_StatusContext;
extern VOS_UINT32                               g_ucUsimHotOutFlag;
extern NAS_MMA_NEW_ADD_GLABAL_VARIABLE_FOR_PC_REPLAY_ST g_MmaNewValue;
extern VOS_UINT8                               g_aucMmaImei[TAF_PH_IMEI_LEN-1];
extern TAF_MMA_GLOBAL_VALUE_ST                 gstMmaValue;
extern MMA_TI_TABLE                            gastMmaTiTab[MMA_MAX_TI];
extern MMA_INTERNAL_TIMER_ST                   gstMmaInternalTimer;
extern MMA_TIMER_ST                            g_MmaSimTimer[TAF_SIM_TIMER_NUM];
extern MMA_UE_BAND_CAPA_ST                     gstMmaBandCapa;
extern MMA_TIMER_ST                                     g_stNetScanProtectTimer;
extern MMA_TIMER_ST                                     g_stAbortNetScanProtectTimer;

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#ifdef __PS_WIN32_RECUR__

VOS_UINT32 NAS_MMA_RestoreContextData(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMA_SDT_MSG_ST                      *pRcvMsgCB;
    NAS_MMA_OUTSIDE_RUNNING_CONTEXT_ST      *pstOutsideCtx;

    pRcvMsgCB     = (NAS_MMA_SDT_MSG_ST *)pstMsg;

    if (EVT_NAS_MMA_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY == pRcvMsgCB->usMsgID)
    {
        pstOutsideCtx = &pRcvMsgCB->stOutsideCtx;


        g_MmaNewValue.ucSimType = pstOutsideCtx->ucSimType;

        TAF_SDC_SetSimImsi(pstOutsideCtx->pc_g_aucMmaImsi);

        PS_MEM_CPY(g_aucMmaImei, pstOutsideCtx->pc_g_aucMmaImei, TAF_PH_IMEI_LEN-1);


        PS_MEM_CPY(&g_StatusContext, &pstOutsideCtx->pc_g_StatusContext, sizeof(STATUS_CONTEXT_STRU));
        PS_MEM_CPY(&gstMmaValue, &pstOutsideCtx->pc_gstMmaValue, sizeof(TAF_MMA_GLOBAL_VALUE_ST));
        gstMmaValue.pg_StatusContext = &g_StatusContext;

        PS_MEM_CPY(&g_stMmaMePersonalisationStatus, &pstOutsideCtx->pc_g_stMmaMePersonalisationStatus, sizeof(MMA_ME_PERSONALISATION_STATUS_STRU));
        PS_MEM_CPY(gastMmaTiTab, pstOutsideCtx->pc_gastMmaTiTab, MMA_MAX_TI*sizeof(MMA_TI_TABLE));

        PS_MEM_CPY(&gstMmaInternalTimer, &pstOutsideCtx->pc_gstMmaInternalTimer, sizeof(MMA_INTERNAL_TIMER_ST));

        PS_MEM_CPY(g_MmaSimTimer, pstOutsideCtx->pc_g_MmaSimTimer, TAF_SIM_TIMER_NUM*sizeof(MMA_TIMER_ST));

        MMA_INFOLOG("MMA: NAS_MMA_RestoreContextData - data is restored.");
    }
    return VOS_TRUE;
}


VOS_UINT32 NAS_MMA_RestoreFixedContextData(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMA_FIXED_CONTEXT_MSG_ST       *pRcvMsgCB;
    NAS_MMA_FIXED_PART_CONTEXT_ST      *pstOutsideCtx;
    VOS_UINT32                          ulVersion;
    NAS_NVIM_ROAM_CFG_INFO_STRU         stRoamFeature;
    MN_MMA_LAST_SETTED_SYSCFG_SET_STRU *pstLastSyscfgSet = VOS_NULL_PTR;

    NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU               stQuickStartFlg;

    stQuickStartFlg.ulQuickStartSta = MMA_QUICK_START_DISABLE;

    pstLastSyscfgSet = MN_MMA_GetLastSyscfgSetAddr();

    pRcvMsgCB        = (NAS_MMA_FIXED_CONTEXT_MSG_ST *)pstMsg;

    if (EVT_NAS_MMA_FIXED_PART_CONTEXT == pRcvMsgCB->usMsgID)
    {
        pstOutsideCtx = &pRcvMsgCB->stFixedOutsideCtx;

        ulVersion = pstOutsideCtx->ulVersion;
        switch (ulVersion)
        {
            case 1:      /* restore new para here, no break. */
                gstMmaBandCapa = pstOutsideCtx->gstMmaBandCapa;
            case 0:
                /* 全局变量恢复 */

                TAF_SDC_SetSimImsi(pstOutsideCtx->aucMmaImsi);

                g_MmaNewValue.ucSimType = pstOutsideCtx->aucSimType;

                pstLastSyscfgSet->stUserSetBand.uUserSetGuBand.ulPrefBand = pstOutsideCtx->ulPrefBand;
                pstLastSyscfgSet->stUserSetBand.uUserSetUeFormatGuBand = pstOutsideCtx->Band;
                gstMmaValue.ulQuickStartFlg = pstOutsideCtx->ulQuickStartFlg;

                gstMmaValue.stEfustServiceCfg = pstOutsideCtx->stEfustServiceCfg;

                PS_MEM_CPY(g_MmaNewValue.aucImeiBcd, pstOutsideCtx->aucImeiBcd, TAF_PH_IMEI_LEN+1);

                gstMmaValue.stSetMsClass.MsClassType = pstOutsideCtx->MsClassType;
                pstLastSyscfgSet->ucRoam = pstOutsideCtx->ucRoam;
                gstMmaValue.ucRoamFeatureStatus = pstOutsideCtx->ucRoamFeatureStatus;

                gstMmaValue.stMeInfo = pstOutsideCtx->stMeInfo;

                g_MmaNewValue.stSimLockPlmnInfo = pstOutsideCtx->stSimLockPlmnInfo;
                g_MmaNewValue.stCardLockStatus = pstOutsideCtx->stCardLockStatus;
                g_MmaNewValue.stSimLockMaxTimes = pstOutsideCtx->stSimLockMaxTimes;

                PS_MEM_CPY(g_MmaNewValue.aucPwd, pstOutsideCtx->aucPwd, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX+1);
                g_MmaNewValue.usSimMePersonalisationActFlag = pstOutsideCtx->usSimMePersonalisationActFlag;
                PS_MEM_CPY(g_MmaNewValue.aucSimPersonalisationStr, pstOutsideCtx->aucSimPersonalisationStr, TAF_MAX_IMSI_LEN+1);


                /* NV项的恢复 */
                MMA_WriteValue2Nvim(en_NV_Item_User_Set_Freqbands, &pstOutsideCtx->ulPrefBand, sizeof(VOS_UINT32));
                MMA_WriteValue2Nvim(en_NV_Item_Support_Freqbands, &pstOutsideCtx->Band, sizeof(MMA_UE_SUPPORT_FREQ_BAND_STRU));

                stQuickStartFlg.ulQuickStartSta = pstOutsideCtx->ulQuickStartFlg;
                MMA_WriteValue2Nvim(en_NV_Item_FollowOn_OpenSpeed_Flag, &(stQuickStartFlg.ulQuickStartSta), sizeof(NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU));

                MMA_WriteValue2Nvim(en_NV_Item_RF_Auto_Test_Flag, &pstOutsideCtx->usRfAutoTestFlg, sizeof(VOS_UINT16));
                MMA_WriteValue2Nvim(en_NV_Item_EFust_Service_Cfg, &pstOutsideCtx->stEfustServiceCfg, sizeof(MMA_EFUST_SERVICE_CFG_STRU));

                MMA_WriteValue2Nvim(en_NV_Item_IMEI, pstOutsideCtx->aucImeiBcd, TAF_PH_IMEI_LEN);


                MMA_WriteValue2Nvim(en_NV_Item_MMA_MsClass, &pstOutsideCtx->MsClassType, sizeof(TAF_PH_MS_CLASS_TYPE));
                stRoamFeature.ucRoamFeatureFlg = pstOutsideCtx->ucRoamFeatureStatus;
                stRoamFeature.ucRoamCapability = pstOutsideCtx->ucRoam;
                MMA_WriteValue2Nvim(en_NV_Item_Roam_Capa, &stRoamFeature, 2);

                MMA_WriteValue2Nvim(en_NV_Item_FMRID, &pstOutsideCtx->stMeInfo.FmrId, TAF_MAX_MFR_ID_LEN);
                MMA_WriteValue2Nvim(en_NV_Item_ProductName, &pstOutsideCtx->stMeInfo.ProductName, TAF_PH_PRODUCT_NAME_LEN);
                /* MMA_WriteValue2Nvim(en_NV_Item_ModelId, &pstOutsideCtx->stMeInfo.ModelId, TAF_MAX_MODEL_ID_LEN); */

                MMA_WriteValue2Nvim(en_NV_Item_CustomizeSimLockPlmnInfo, &pstOutsideCtx->stSimLockPlmnInfo, sizeof(TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU));
                MMA_WriteValue2Nvim(en_NV_Item_CardlockStatus, &pstOutsideCtx->stCardLockStatus, sizeof(TAF_CUSTOM_CARDLOCK_STATUS_STRU));
                MMA_WriteValue2Nvim(en_NV_Item_CustomizeSimLockMaxTimes, &pstOutsideCtx->stSimLockMaxTimes, sizeof(TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU));

                MMA_WriteValue2Nvim(en_NV_Item_Sim_Personalisation_Pwd, pstOutsideCtx->aucPwd, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);
                MMA_WriteValue2Nvim(en_NV_Item_Me_Personalisation_Active_Flag, &pstOutsideCtx->usSimMePersonalisationActFlag, 2);
                MMA_WriteValue2Nvim(en_NV_Item_Sim_Personalisation_Imsi_Str, pstOutsideCtx->aucSimPersonalisationStr, pstOutsideCtx->aucSimPersonalisationStr[0]+1);

                break;
            default:
                break;
        }

        MMA_INFOLOG("MMA: NAS_MMA_RestoreFixedContextData - data is restored.");
    }
    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_MMA_SetMiniMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 当前不为FULL模式,都可以直接回复成功 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
#if (FEATURE_ON == FEATURE_LTE)
        NVM_Flush();
#endif
        NV_Flush();

        TAF_SDC_SetCurPhoneMode(ucSetMode);

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_SetRfOffMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 当前模式不为FULL模式，直接上报成功 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SDC_SetCurPhoneMode(ucSetMode);

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_MMA_SetFtMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 当前模式不为FULL模式，直接上报成功 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SDC_SetCurPhoneMode(ucSetMode);

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_MMA_SetVdfMiniMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 当前模式不为FULL模式，直接上报成功 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SDC_SetCurPhoneMode(ucSetMode);

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_MMA_SetLowPowerMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 当前模式不为FULL模式，直接上报成功 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        TAF_SDC_SetCurPhoneMode(ucSetMode);

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_MMA_SetResetMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;
    VOS_UINT32                          ulRet;

    stPhMode.CmdType    = TAF_PH_CMD_SET;
    stPhMode.PhMode     = ucSetMode;

    /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
    MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

    /* 启动延迟定时器待AT恢复OK后重启单板*/
    g_stPhPhResetTimer.ulTimerId = TI_MN_PH_RESET;

    ulRet = NAS_StartRelTimer( &g_stPhPhResetTimer.MmaHTimer,
                               WUEPS_PID_MMA,
                               TI_MN_PH_RESET_LENGTH,
                               TAF_MMA,
                               TI_MN_PH_RESET,
                               VOS_RELTIMER_NOLOOP );

    if (VOS_OK !=  ulRet)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SetRestMode:start TI_MN_PH_RESET failed!");
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_SetFullMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;

    /* 当前已经处于在线模式，直接上报成功 */
    if (ucSetMode == TAF_SDC_GetCurPhoneMode())
    {
        stPhMode.CmdType    = TAF_PH_CMD_SET;
        stPhMode.PhMode     = ucSetMode;

        /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

#if (VOS_WIN32 == VOS_OS_VER)

    if (VOS_TRUE == g_ucMmaOmConnectFlg)
    {
        NAS_MMA_SndNVData();
    }
#endif

    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_SetPowerOffMode_PreProc(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucSetMode
)
{
    TAF_PH_OP_MODE_CNF_STRU                                 stPhMode;
    VOS_UINT16                                              usAppCfgSupportType;
#if (FEATURE_ON == FEATURE_POWER_ON_OFF)
    VOS_UINT32                          ulRslt;
#endif

    usAppCfgSupportType     = TAF_SDC_GetAppCfgSupportType();
    stPhMode.CmdType        = TAF_PH_CMD_SET;
    stPhMode.PhMode         = ucSetMode;

    TAF_APS_SaveDsFlowInfoToNv();

    if (SYSTEM_APP_WEBUI == usAppCfgSupportType)
    {
#if (FEATURE_ON == FEATURE_POWER_ON_OFF)
        if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
        {
            /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
            MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);





            /* 将TASKDELAY修改为启动定时器进行保护 */
            ulRslt = NAS_StartRelTimer(&g_stPowerDownDelayTimer.MmaHTimer,
                                  WUEPS_PID_MMA,
                                  TI_TAF_MMA_DELAY_POWER_DOWN_TIMER_LEN,
                                  TAF_MMA,
                                  TI_TAF_MMA_DELAY_POWER_DOWN,
                                  VOS_RELTIMER_NOLOOP );

            if( VOS_OK !=  ulRslt)
            {
                MMA_WARNINGLOG("MN_PH_Switch2OffFunc():ERROR:Start TI_TAF_MMA_DELAY_POWER_DOWN Timer Error!");
                DRV_POWER_OFF();
            }



            return VOS_TRUE;
        }

        return VOS_FALSE;
#endif
    }

    if (SYSTEM_APP_ANDROID == usAppCfgSupportType)
    {
        if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
        {
            /* 在上报事件前先调用COMM接口Flush NV */
#if (FEATURE_ON == FEATURE_LTE)
            NVM_Flush();
#endif
            NV_Flush();

            /* 其他模式下使用AT+CFUN=8也需要去激活USIM卡 */
            NAS_USIMMAPI_DeactiveCardReq(WUEPS_PID_MMA);

            MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

            return VOS_TRUE;
        }

        return VOS_FALSE;
    }

    /* 目前除了WEBUI和ANDROID，其他后台不会下发AT+CFUN=8 */
    if (TAF_PH_MODE_FULL != TAF_SDC_GetCurPhoneMode())
    {
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_NO_ERROR);

        return VOS_TRUE;
    }

    return VOS_FALSE;

}

VOS_UINT32 TAF_MMA_IsPhoneModeSetAllowed_PreProc(
    VOS_UINT16                          usClientId,
    TAF_PH_MODE                         ucPhMode
)
{
    TAF_SDC_RAT_PRIO_STRU              *pstRatPrio      = VOS_NULL_PTR;
    VOS_UINT8                           ucCurPhmode;

    /* 模式设置是否允许的表 VOS_TRUE :代表允许, VOS_FALSE:代表不允许 */
    VOS_UINT32                          aulOperModeSetTable[TAF_PH_MODE_NUM_MAX][TAF_PH_MODE_NUM_MAX] =
    {
        /* operate mode: MINI--0,FULL--1,TXOFF--2,RXOFF--3,RFOFF--4,FT--5,RESET--6,LOW--7
                         POWEROFF--8,LOWPOWER--9 */
        /*->0*/     /*->1*/     /*->2*/     /*->3*/     /*->4*/     /*->5*/     /*->6*/     /*->7*/     /* ->8 */   /* ->9 */
/*0->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE},
/*1->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE},
/*2->*/ {VOS_FALSE, VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE},
/*3->*/ {VOS_FALSE, VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE},
/*4->*/ {VOS_FALSE, VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_FALSE},
/*5->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE},
/*6->*/ {VOS_FALSE, VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE,  VOS_FALSE},
/*7->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE},
/*8->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE},
/*9->*/ {VOS_TRUE,  VOS_TRUE,   VOS_FALSE,  VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_FALSE,  VOS_TRUE,   VOS_TRUE,   VOS_TRUE}
    };

    pstRatPrio                          = TAF_SDC_GetMsPrioRatList();
    ucCurPhmode                         = TAF_SDC_GetCurPhoneMode();

    /* 表中不准许设置，则直接回复失败 */
    if (VOS_FALSE                       == aulOperModeSetTable[ucCurPhmode][ucPhMode] )
    {
        return VOS_FALSE;
    }

    /* 异常保护:如果CL模式，且当前是AT设置模式，则上报设置失败 */
    if ((VOS_TRUE                       == TAF_SDC_IsConfigCLInterWork())
     && (usClientId                     < AT_CLIENT_ID_BUTT))
    {
        return VOS_FALSE;
    }

    /* 异常保护:如果不是CL模式，且当前是CMMCA设置模式，则上报CMMCA失败 */
    if ((VOS_FALSE                      == TAF_SDC_IsConfigCLInterWork())
     && (CMMCA_CLIENT_ID                == usClientId))
    {
        return VOS_FALSE;
    }

    /* 在CMMCA进行开机时候，如果当前不是L ONLY MODE,则直接回复CMMCA失败 */
    if ( (VOS_TRUE                      != TAF_SDC_IsLteOnlyMode(pstRatPrio))
      && (CMMCA_CLIENT_ID               == usClientId) )
    {
        return VOS_FALSE;
    }

    /* 可以进行模式设置 */
    return VOS_TRUE;

}


VOS_UINT32 TAF_MMA_ProcOmPhoneModeSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucOldMode;
    MN_APP_REQ_MSG_STRU                *pstRcvMsg       = VOS_NULL_PTR;
    TAF_PH_OP_MODE_STRU                *pstPhModeSet    = VOS_NULL_PTR;

    pstRcvMsg       = (MN_APP_REQ_MSG_STRU *)pstMsg;
    pstPhModeSet    = (TAF_PH_OP_MODE_STRU*)pstRcvMsg->aucContent;

    usClientId      = pstRcvMsg->clientId;
    ucOpId          = pstRcvMsg->opId;
    ulRet           = VOS_TRUE;
    ucOldMode       = TAF_SDC_GetCurPhoneMode();

    /* 模式设置状态机在运行则直接进入后处理缓存 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        return VOS_FALSE;
    }


    /* 判断当前是否允许进行模式设置 */
    if (VOS_FALSE == TAF_MMA_IsPhoneModeSetAllowed_PreProc(usClientId, pstPhModeSet->PhMode))
    {
        stPhMode.CmdType    = TAF_PH_CMD_SET;
        stPhMode.PhMode     = TAF_SDC_GetCurPhoneMode();

        MMA_PhModeReport(usClientId,  ucOpId, stPhMode, TAF_ERR_ERROR);

        return VOS_TRUE;
    }


    /* 根据不同的模式进行设置处理 */
    ulRet =TAF_MMA_ProcPhoneModeSet_PreProc(pstPhModeSet->PhMode, usClientId, ucOpId);

    if (ucOldMode != pstPhModeSet->PhMode)
    {
        /* 离开FT模式需要允许睡眠 */
        if (TAF_PH_MODE_FT == ucOldMode)
        {
            TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_FTM);
            if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
            {
                vos_printf("\n*TAF_MMA_ProcOmPhoneModeSetReq_PreProc:PWRCTRL_SLEEP_FTM,TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK\n");
            }
        }

        /* 进入FT模式需要禁止睡眠 */
        if (TAF_PH_MODE_FT == pstPhModeSet->PhMode)
        {
            TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_FTM);
            if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
            {
                vos_printf("\n*TAF_MMA_ProcOmPhoneModeSetReq_PreProc:PWRCTRL_SLEEP_FTM,TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK\n");
            }
        }
    }

    return ulRet;
}
VOS_UINT32 TAF_MMA_ProcPhoneModeSet_PreProc(
    TAF_PH_MODE                         ucCurrPhMode,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;
    VOS_UINT32                          ulRet;

    ulRet = VOS_TRUE;

    /* 根据不同的模式进行设置处理 */
    switch (ucCurrPhMode)
    {
        case TAF_PH_MODE_MINI:

            ulRet = TAF_MMA_SetMiniMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_RFOFF:

            ulRet = TAF_MMA_SetRfOffMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_FT:

            ulRet = TAF_MMA_SetFtMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_VDFMINI:

            ulRet = TAF_MMA_SetVdfMiniMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_LOWPOWER:

            ulRet = TAF_MMA_SetLowPowerMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_FULL:

            ulRet = TAF_MMA_SetFullMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_RESET:

            ulRet = TAF_MMA_SetResetMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        case TAF_PH_MODE_POWEROFF:

            ulRet = TAF_MMA_SetPowerOffMode_PreProc(usClientId, ucOpId, ucCurrPhMode);
            break;

        default:

            /* warning 打印 */
            TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_ProcPhoneModeSet_PreProc:invalid mode!");

            stPhMode.CmdType = TAF_PH_CMD_SET;
            stPhMode.PhMode = TAF_SDC_GetCurPhoneMode();
            MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_ERROR);

            break;
    }

    return ulRet;
}



VOS_UINT32 TAF_MMA_ProcTafPhoneModeSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    TAF_PH_OP_MODE_CNF_STRU             stPhMode;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucOldMode;
    TAF_PH_MODE                         ucCurrPhMode;
    TAF_MMA_PHONE_MODE_SET_REQ_STRU    *pstRcvMsg       = VOS_NULL_PTR;

    pstRcvMsg       = (TAF_MMA_PHONE_MODE_SET_REQ_STRU *)pstMsg;

    usClientId      = pstRcvMsg->stCtrl.usClientId;
    ucOpId          = pstRcvMsg->stCtrl.ucOpId;
    ulRet           = VOS_TRUE;
    ucOldMode       = TAF_SDC_GetCurPhoneMode();
    ucCurrPhMode    = pstRcvMsg->stPhoneModePara.PhMode;

    /* 开机读取NV */
    if (TAF_PH_MODE_FULL == pstRcvMsg->stPhoneModePara.PhMode)
    {
        TAF_MMA_ReadLcCtrlNvim();

        if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
        {
            TAF_MMA_ShowCLConfigInfo();
        }
    }

    /* 模式设置状态机在运行则直接进入后处理缓存 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        return VOS_FALSE;
    }

    /* 判断当前是否允许进行模式设置 */
    if (VOS_FALSE == TAF_MMA_IsPhoneModeSetAllowed_PreProc(usClientId, ucCurrPhMode))
    {
        stPhMode.CmdType = TAF_PH_CMD_SET;
        stPhMode.PhMode = TAF_SDC_GetCurPhoneMode();
        MMA_PhModeReport(usClientId, ucOpId, stPhMode, TAF_ERR_ERROR);

        return VOS_TRUE;
    }

    /* 在CMMCA进行开机时候，如果当前是手动模式，则修改为自动搜网模式 */
    if ( (MMA_MMC_PLMN_SEL_MODE_AUTO    != TAF_MMA_GetPlmnSelectionMode())
      && (CMMCA_CLIENT_ID               == usClientId) )
    {
        /* CL模式时候，如果手动模式，则修正为自动搜网模式，进行异常保护，防止APP没有设置当前为自动搜网模式 */
        TAF_MMA_WritePlmnSelectionModeNvim(MMA_MMC_PLMN_SEL_MODE_AUTO);
        TAF_MMA_SetPlmnSelectionMode(TAF_PH_PLMN_SEL_MODE_AUTO);
    }

    /* 根据不同的模式进行设置处理 */
    ulRet = TAF_MMA_ProcPhoneModeSet_PreProc(ucCurrPhMode, usClientId, ucOpId);

    if (ucOldMode != ucCurrPhMode)
    {
        /* 离开FT模式需要允许睡眠 */
        if (TAF_PH_MODE_FT == ucOldMode)
        {

            if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
            {
                vos_printf("\n*TAF_MMA_ProcTafPhoneModeSetReq_PreProc:PWRCTRL_SLEEP_FTM,TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK\n");
            }

            TAF_DRVAPI_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_FTM);
        }

        /* 进入FT模式需要禁止睡眠 */
        if (TAF_PH_MODE_FT == ucCurrPhMode)
        {
            TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_FTM);
            if (VOS_TRUE == TAF_MMA_GetMmaLogInfoFlag())
            {
                vos_printf("\n*TAF_MMA_ProcTafPhoneModeSetReq_PreProc:PWRCTRL_SLEEP_FTM,TAF_DRVAPI_PWRCTRL_SLEEPVOTE_LOCK\n");
            }
        }
    }

    return ulRet;
}




VOS_UINT32 TAF_MMA_RcvOmPhoneModeSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg       = VOS_NULL_PTR;
    TAF_PH_OP_MODE_STRU                *pstPhModeSet    = VOS_NULL_PTR;
    TAF_PH_OP_MODE_CNF_STRU             stPhModeCnf;
    VOS_UINT32                          ulRet;

    pstRcvMsg       = (MN_APP_REQ_MSG_STRU *)pstMsg;
    pstPhModeSet    = (TAF_PH_OP_MODE_STRU *)pstRcvMsg->aucContent;

    /* 根据命令类型分发处理 */
    switch (pstPhModeSet->CmdType)
    {
        case TAF_PH_CMD_SET:            /* 设置类分发处理 */

            ulRet = TAF_MMA_ProcOmPhoneModeSetReq_PreProc(ulEventType, pstMsg);

            break;

        case TAF_PH_CMD_QUERY:          /* 查询类分发处理 */

            /* 上报事件命令类型填写 */
            stPhModeCnf.CmdType = TAF_PH_CMD_QUERY;

            /* 上报事件命令当前模式类型填写 */
            stPhModeCnf.PhMode = TAF_SDC_GetCurPhoneMode();

            /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
            MMA_PhModeReport(pstRcvMsg->clientId, pstRcvMsg->opId, stPhModeCnf, TAF_ERR_NO_ERROR);

            ulRet = VOS_TRUE;
            break;

        default:

            /* 其他类分发处理 */
            TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvOmPhoneModeSetReq_PreProc:invalid para!");

            stPhModeCnf.CmdType    = pstPhModeSet->CmdType;
            stPhModeCnf.PhMode     = TAF_SDC_GetCurPhoneMode();
            MMA_PhModeReport(pstRcvMsg->clientId, pstRcvMsg->opId, stPhModeCnf, TAF_ERR_PARA_ERROR);
            ulRet = VOS_TRUE;
            break;
    }

    return ulRet;
}
VOS_UINT32 TAF_MMA_RcvPhoneModeQuery_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg       = VOS_NULL_PTR;
    TAF_PH_OP_MODE_CNF_STRU             stPhModeCnf;

    pstRcvMsg       = (MN_APP_REQ_MSG_STRU *)pstMsg;

    /* 上报事件命令类型填写 */
    stPhModeCnf.CmdType = TAF_PH_CMD_QUERY;

    /* 上报事件命令当前模式类型填写 */
    stPhModeCnf.PhMode = TAF_SDC_GetCurPhoneMode();

    /* 上报TAF_PH_EVT_OPER_MODE_CNF事件 */
    MMA_PhModeReport(pstRcvMsg->clientId, pstRcvMsg->opId, stPhModeCnf, TAF_ERR_NO_ERROR);

    return VOS_TRUE;
}
VOS_VOID TAF_MMA_SaveInterUsimStatusChangeIndMsgInCacheQueue_PreProc(VOS_VOID)
{
    TAF_MMA_INTER_USIM_STATUS_CHANGE_IND_STRU               stInternalMsg;
    VOS_UINT32                                              ulEventType;

    PS_MEM_SET(&stInternalMsg, 0, sizeof(TAF_MMA_INTER_USIM_STATUS_CHANGE_IND_STRU));

    stInternalMsg.MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    stInternalMsg.MsgHeader.ulSenderPid     = WUEPS_PID_MMA;
    stInternalMsg.MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    stInternalMsg.MsgHeader.ulReceiverPid   = WUEPS_PID_MMA;
    stInternalMsg.MsgHeader.ulMsgName       = MMA_MMA_INTER_USIM_STATUS_CHANGE_IND;
    stInternalMsg.MsgHeader.ulLength        = sizeof(TAF_MMA_INTER_USIM_STATUS_CHANGE_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 缓存内部卡状态指示消息 */
    ulEventType = TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND);
    TAF_MMA_SaveCacheMsgInMsgQueue(ulEventType, &stInternalMsg);
}
VOS_UINT32 TAF_MMA_RcvPihUsimStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    PS_USIM_STATUS_IND_STRU            *pstUsimMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulUsimStaChg;
    VOS_UINT32                          ulImsiChg;
    TAF_PH_FPLMN_OPERATE_STRU           stFplmnOperate;
    VOS_UINT8                           ucMmaLogInfoFlag;

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    TAF_MMA_USIMM_CARD_TYPE_ENUM_UINT32           enCardType;                   /* 卡类型:SIM、USIM、ROM-SIM  */
    TAF_MMA_CARD_STATUS_ENUM_UINT8                enCardStatus;
#endif

    pstUsimMsg      = (PS_USIM_STATUS_IND_STRU*)pstMsg;

    ucMmaLogInfoFlag = TAF_MMA_GetMmaLogInfoFlag();

    if (VOS_TRUE == ucMmaLogInfoFlag)
    {
        vos_printf("\n*TAF_MMA_RcvPihUsimStatusInd_PrePro:CurFsm State = %d\n", g_StatusContext.ulFsmState);
        vos_printf("\n*TAF_MMA_RcvPihUsimStatusInd_PrePro:enCardStatus = %d\n", (pstUsimMsg->enCardStatus));
        vos_printf("\n*TAF_MMA_RcvPihUsimStatusInd_PrePro:enCardType = %d\n", (pstUsimMsg->enCardType));
    }

    /* 发送消息 AT_MMA_USIM_STATUS_IND 给AT */
    MMA_PhUsimStatusInd(pstUsimMsg);

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    TAF_MMA_ConvertCardType(pstUsimMsg->enCardType, &enCardType);
    TAF_MMA_ConvertCardStatus(pstUsimMsg->enCardStatus, &enCardStatus);
    TAF_MMA_SndSimStatusInd(enCardType, enCardStatus);
#endif

    /* 更新卡类型 */
    TAF_MMA_ChangeUsimType(pstUsimMsg->enCardType);

    /* 在收到SIM上报的卡状态时，通知SMS模块当前的卡状态 */
    MN_PH_SndMsgUsimStatus(pstUsimMsg->enCardStatus);

    ulUsimStaChg    = TAF_MMA_IsUsimStatusChanged(pstUsimMsg->enCardStatus);
    ulImsiChg       = TAF_MMA_IsImsiChanged(pstUsimMsg);

    if (VOS_TRUE == ulImsiChg)
    {
        TAF_SDC_SetSimImsi(pstUsimMsg->aucIMSI);

        TAF_SDC_SetLastSimImsi(pstUsimMsg->aucIMSI);
    }

    if (USIMM_CARD_SERVIC_AVAILABLE == pstUsimMsg->enCardStatus)
    {
#if (VOS_WIN32 == VOS_OS_VER)
        /* PC工程持续集成适配代码，卡状态改变重新进行锁网锁卡校验 */
        g_stMmaMePersonalisationStatus.SimLockStatus = MMA_SIM_IS_UNLOCK;
#endif
        /* 突尼斯问题修改:在SIM卡状态发生改变,或者IMSI发生改变时,需要重新校验锁卡状态.
           因为校验函数只有在MMA_SIM_IS_UNLOCK状态才会重新校验,所以重新初始化此变量 */
        if ((VOS_TRUE == ulUsimStaChg)
         || (VOS_TRUE == ulImsiChg))
        {
            g_stMmaMePersonalisationStatus.SimLockStatus = MMA_SIM_IS_UNLOCK;
        }

        /* 检查是否锁卡 */
        MMA_CheckMePersonalisationStatus();
    }

    MMA_ChangeUsimStatus(pstUsimMsg->enCardStatus);

    /* 更新卡的IMSI和SDC中的IMSI状态以及完成锁卡校验后,再上报卡状态指示给AT */
    TAF_MMA_SndAtIccStatusInd_PreProc(ulUsimStaChg);

    /* 开机过程中收到卡状态指示的处理 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        if (VOS_TRUE == ucMmaLogInfoFlag)
        {
            vos_printf("*TAF_MMA_RcvPihUsimStatusInd_PrePro:Cur Phone Mode Fsm is running\n");
        }
        return TAF_MMA_ProcUsimStatusIndInPhoneModeProc_PreProc(ulUsimStaChg, ulImsiChg);
    }

    /* 卡状态以及IMSI都未改变则返回 */
    if ((VOS_FALSE == ulUsimStaChg)
     && (VOS_FALSE == ulImsiChg))
    {
        if (VOS_TRUE == ucMmaLogInfoFlag)
        {
            vos_printf("*TAF_MMA_RcvPihUsimStatusInd_PrePro:UsimStaChg = VOS_FALSE, ulImsiChg = VOS_FALSE\n");
        }
        return VOS_TRUE;
    }

    if (VOS_TRUE == MN_MMA_GetRoamingBrokerFlg())
    {
        stFplmnOperate.ulCmdType = TAF_PH_FPLMN_DEL_ALL;
        Api_MmaFplmnHandleFromMmc(0xffff, 0, stFplmnOperate);
    }

    /* 如果有卡则发送读文件请求 */
    if (TAF_SDC_USIM_STATUS_VALID == TAF_SDC_GetSimStatus())
    {
        TAF_MMA_ReadUsimInfo_PreProc();
    }

    /* 非开机过程中收到卡状态不触发关机流程 */
    if (STA_FSM_NULL == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    /* 已经开机则进入main触发关机流程 */
    return VOS_FALSE;
}
VOS_UINT32 TAF_MMA_RcvMmaInterUsimStatusChangeInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果当前为NULL状态则不需要处理 */
    if (STA_FSM_NULL == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    /* 启动phone mode状态机 */
    return VOS_FALSE;
}


VOS_UINT32 TAF_MMA_RcvMmaSimLockStatusChangeInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果当前非available状态则丢弃该消息 */
    if (TAF_SDC_USIM_STATUS_VALID != TAF_SDC_GetSimStatus())
    {
        return VOS_TRUE;
    }

    /* 上报一次卡状态 */
    TAF_MMA_SndAtIccStatusInd_PreProc(VOS_TRUE);

    /* 开机过程中,构造内部卡状态改变指示消息缓存 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        TAF_MMA_SaveInterUsimStatusChangeIndMsgInCacheQueue_PreProc();

        return VOS_TRUE;
    }

    /* 如果当前为NULL状态则不需要处理 */
    if (STA_FSM_NULL == g_StatusContext.ulFsmState)
    {
        return VOS_TRUE;
    }

    /* 启动phone mode状态机 */
    return VOS_FALSE;
}

VOS_UINT32 TAF_MMA_RcvAtSyscfgHandle_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg   = (MN_APP_REQ_MSG_STRU *)pstMsg;

    /*查询命令处理*/
    MMA_PhSysCfgQuery(pstRcvMsg->clientId, pstRcvMsg->opId);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvAtParaReadReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MN_APP_REQ_MSG_STRU                *pstRcvMsg = VOS_NULL_PTR;
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    TAF_PARA_TYPE                       ucParaType;

    pstRcvMsg = (MN_APP_REQ_MSG_STRU *)pstMsg;

    ucParaType  = pstRcvMsg->aucContent[0];
    usClientId  = pstRcvMsg->clientId;
    ucOpId      = pstRcvMsg->opId;

    /* MMA参数查询调用 */
    if ((ucParaType >= TAF_PH_MFR_ID_PARA )
     && (ucParaType <= (TAF_PH_PRODUCT_NAME_PARA + 1)))
    {
        MMA_QueryProc(usClientId, ucOpId, &ucParaType);

        return VOS_TRUE;
    }

    switch (ucParaType)
    {
        case TAF_PH_ACCESS_MODE_PARA:
        case TAF_PH_MS_CLASS_PARA:
        case TAF_PH_REG_PLMN_PARA:

            MMA_DbQuery(usClientId, ucOpId, ucParaType);
            return VOS_TRUE;

        default:

            TAF_WARNING_LOG(WUEPS_PID_MMA, "TAF_MMA_RcvAtParaReadReq_PreProc:invalid query para!");
            break;
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmcNetworkCapabilityInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU               *pstRcvMsg       = VOS_NULL_PTR;
    TAF_SDC_NETWORK_CAP_INFO_STRU                           stNewNwCapInfo;
    VOS_UINT32                                              ulNwCapInfoChanged;

    pstRcvMsg = (MMC_MMA_NETWORK_CAPABILITY_INFO_IND_STRU *)pstMsg;

    stNewNwCapInfo.enNwEmcBsCap = (TAF_SDC_NW_EMC_BS_CAP_ENUM_UINT8)pstRcvMsg->enNwEmcBsCap;
    stNewNwCapInfo.enNwImsVoCap = (TAF_SDC_NW_IMS_VOICE_CAP_ENUM_UINT8)pstRcvMsg->enNwImsVoCap;

    switch (pstRcvMsg->enLteCsCap)
    {
        case MMA_MMC_LTE_CS_CAPBILITY_NO_ADDITION_INFO:
            stNewNwCapInfo.enLteCsCap = TAF_SDC_LTE_CS_CAPBILITY_NO_ADDITION_INFO;
            break;

        case MMA_MMC_LTE_CS_CAPBILITY_CSFB_NOT_PREFER:
            stNewNwCapInfo.enLteCsCap = TAF_SDC_LTE_CS_CAPBILITY_CSFB_NOT_PREFER;
            break;

        case MMA_MMC_LTE_CS_CAPBILITY_SMS_ONLY:
            stNewNwCapInfo.enLteCsCap = TAF_SDC_LTE_CS_CAPBILITY_SMS_ONLY;
            break;

        case MMA_MMC_LTE_CS_CAPBILITY_NOT_SUPPORTED:
            stNewNwCapInfo.enLteCsCap = TAF_SDC_LTE_CS_CAPBILITY_NOT_SUPPORTED;
            break;

        default:
            stNewNwCapInfo.enLteCsCap = TAF_SDC_LTE_CS_CAPBILITY_BUTT;
            break;
    }

    /* 判断当前的网络能力与GU和L的能力相比是否发生了改变 */
    ulNwCapInfoChanged = TAF_MMA_IsNetworkCapInfoChanged(&stNewNwCapInfo);

    if (VOS_TRUE == ulNwCapInfoChanged)
    {
        /* update infomaton to the corresponding global variable according to RAT */
        TAF_MMA_SetNetworkCapabilityInfo(TAF_SDC_GetSysMode(), &stNewNwCapInfo);

#if (FEATURE_IMS == FEATURE_ON)
        if (VOS_TRUE == TAF_SDC_GetImsSupportFlag())
        {
            /* 通知SPM网络能力变更，触发SPM处理缓存的消息，消息中不带当前的网络能力，由SPM取SDC全局变量中的 */
            TAF_MMA_SndSpmNetworkCapabilityChangeNotify();

            /* send service infomation change indcation */
            TAF_MMA_SndImsaSrvInfoNotify((MMA_MMC_SERVICE_STATUS_ENUM_UINT32)TAF_SDC_GetPsServiceStatus());
        }
#endif
    }


    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvPihUsimRefreshIndMsgProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    PS_USIM_REFRESH_IND_STRU           *pUsimRefreshIndMsg = VOS_NULL_PTR;

    pUsimRefreshIndMsg = (PS_USIM_REFRESH_IND_STRU*)pstMsg;

    /* 目前PIH上报文件刷新指示消息后, USIM继续上报卡状态指示给MMA
       后续此处逻辑被USIM删除后,MMA需要重新读取IMSI比较是否发生改变
       ,并进行关机和开机处理 */
    if (USIMM_REFRESH_ALL_FILE == pUsimRefreshIndMsg->enRefreshType)
    {
        /* refresh消息指示所有文件都发生变更 */
        TAF_MMA_UsimRefreshIndAllFileChangedMsgProc(pUsimRefreshIndMsg);

        return VOS_TRUE;
    }

    if (USIMM_REFRESH_FILE_LIST == pUsimRefreshIndMsg->enRefreshType)
    {
        /* refresh 消息指示特定文件发生变更 */
        TAF_MMA_UsimRefreshIndFileListChangedMsgProc(pUsimRefreshIndMsg);

        return VOS_TRUE;
    }

    if (USIMM_REFRESH_3G_SESSION_RESET == pUsimRefreshIndMsg->enRefreshType)
    {
       return TAF_MMA_ProcPihUsimRefreshInd3gSessionReset_PreProc();
    }

    return VOS_TRUE;
}




VOS_UINT32 TAF_MMA_ProcPihUsimRefreshInd3gSessionReset_PreProc(VOS_VOID)
{
    TAF_MMA_ReadUsimInfo_PreProc();

    /* 开机过程中,构造内部卡状态改变指示消息缓存 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        TAF_MMA_SaveInterUsimStatusChangeIndMsgInCacheQueue_PreProc();

        return VOS_TRUE;
    }

    /* 非开机过程中收到卡状态不触发关机流程 */
    if (STA_FSM_NULL == g_StatusContext.ulFsmState)
    {
       return VOS_TRUE;
    }

    /* 已经开机则进入main触发关机流程 */
    return VOS_FALSE;
}

VOS_VOID TAF_MMA_ReadUsimInfo_PreProc(VOS_VOID)
{
    /* 读取SPN文件 */
    TAF_MMA_ReadSpnFile();

    MMA_ReadSimCPHSOperNameStr();

    /* 网络选择菜单控制使能，开机需要读6F15文件 */
    TAF_MMA_ReadCustomerServiceProfileFile_SwitchOn();

    /* 接入技术平衡控制使能，开机读4F36文件 */
    TAF_MMA_ReadRatModeFile();
}

VOS_VOID TAF_MMA_SndAtIccStatusInd_PreProc(
    VOS_UINT32                          ulUsimStaChg
)
{
    VOS_UINT8                           ucSimStatus;
    VOS_UINT8                           ucSimLockStatus;

    ucSimStatus                         = MMA_GetUsimStatus(ulUsimStaChg);

    /* 获取锁卡状态 */
    if ( MMA_SIM_IS_LOCK == MMA_GetMeLockStatus() )
    {
        ucSimLockStatus                 = VOS_TRUE;
    }
    else
    {
        ucSimLockStatus                 = VOS_FALSE;
    }

    /* 热插拔期间不上报SIMST给AP */
   if ((TAF_PH_MODE_FULL == TAF_SDC_GetCurPhoneMode())
    && (VOS_TRUE         == g_ucUsimHotOutFlag))
   {
       g_ucUsimHotOutFlag = VOS_FALSE;

       return;
   }

   /* 适配RIL逻辑,上电第一次首先收到的是协议栈启动完成的命令(PSINIT)
      上电时上报的卡状态延迟到协议栈初始化完成之后上报上去 */
   if (TAF_MMA_PHONE_MODE_STA_WAIT_PIH_USIM_STATUS_IND == TAF_MMA_FSM_GetFsmTopState())
   {
       return;
   }

   TAF_MMA_SndAtIccStatusInd(ucSimStatus, ucSimLockStatus);

   return;
}




VOS_UINT32 TAF_MMA_ProcUsimStatusIndInPhoneModeProc_PreProc(
    VOS_UINT32                          ulUsimStaChg,
    VOS_UINT32                          ulImsiChg
)
{
    /* 开机过程中,等待卡状态指示状态时直接进状态机处理该消息 */
    if (TAF_MMA_PHONE_MODE_STA_WAIT_PIH_USIM_STATUS_IND == TAF_MMA_FSM_GetFsmTopState())
    {
        /* 如果有卡则发送读文件请求 */
        if (TAF_SDC_USIM_STATUS_VALID == TAF_SDC_GetSimStatus())
        {
            TAF_MMA_ReadUsimInfo_PreProc();
        }

        return VOS_FALSE;
    }

    /* 卡状态或IMSI发生改变则重新读取卡文件并缓存卡状态改变消息 */
    if ((VOS_TRUE == ulUsimStaChg)
     || (VOS_TRUE == ulImsiChg))
    {
        if (TAF_SDC_USIM_STATUS_VALID == TAF_SDC_GetSimStatus())
        {
            TAF_MMA_ReadUsimInfo_PreProc();
        }

        TAF_MMA_SaveInterUsimStatusChangeIndMsgInCacheQueue_PreProc();
    }

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmcCampOnInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_CAMP_ON_IND_STRU_STRU      *pstCampOnInd = VOS_NULL_PTR;

    pstCampOnInd = (MMC_MMA_CAMP_ON_IND_STRU_STRU*)pstMsg;

    if (VOS_TRUE == pstCampOnInd->ucCampOnFlg)
    {
        TAF_SDC_SetCampOnFlag(VOS_TRUE);
    }
    else
    {
        TAF_SDC_SetCampOnFlag(VOS_FALSE);
    }

    return VOS_TRUE;
}



TAF_PH_RAT_TYPE_ENUM_UINT8 TAF_MMA_ConvertSdcRatToAppRatType(
                        TAF_SDC_SYS_MODE_ENUM_UINT8 enSdcRatType
                        )
{
    switch (enSdcRatType)
    {
        case TAF_SDC_SYS_MODE_GSM:
             return TAF_PH_RAT_GSM;

        case TAF_SDC_SYS_MODE_WCDMA:
             return TAF_PH_RAT_WCDMA;

        case TAF_SDC_SYS_MODE_LTE:
             return TAF_PH_RAT_LTE;

        default:
            MMA_WARNINGLOG("TAF_MMA_ConvertSdcRatToAppRatType():WORNING:The input is invalid.");
            return TAF_PH_RAT_BUTT;
    }
}


TAF_PH_SERVICE_DOMAIN TAF_MMA_ConvertMmcDomainToMmaDomainType(
                        MMA_MMC_SRVDOMAIN_ENUM_UINT32 enMmcDomainType
                        )
{
    switch (enMmcDomainType)
    {
        case MMA_MMC_SRVDOMAIN_NO_DOMAIN:
        case MMA_MMC_SRVDOMAIN_NOT_REG_PLMN_SEARCHING:
             return TAF_PH_SERVICE_NULL;

        case MMA_MMC_SRVDOMAIN_CS:
             return TAF_PH_SERVICE_CS;

        case MMA_MMC_SRVDOMAIN_PS:
             return TAF_PH_SERVICE_PS;

        case MMA_MMC_SRVDOMAIN_CS_PS:
             return TAF_PH_SERVICE_CS_PS;

        case MMA_MMC_SRVDOMAIN_NO_CHANGE:
             return TAF_PH_SERVICE_NOCHANGE;

        default:
            MMA_WARNINGLOG("TAF_MMA_ConvertMmcDomainToMmaDomainType():WORNING:The input is invalid.");
            return TAF_PH_SERVICE_NULL;
    }
}

VOS_VOID TAF_MMA_SndRegRejInfoInd(
    MMC_MMA_REG_RESULT_IND_STRU        *pstRejInfoInd
)
{
    /* 将MMC的数据类型转换为TAF的数据类型，进行上报 */
    TAF_PH_REG_REJ_INFO_STRU            stTafRejInfo;
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo = VOS_NULL_PTR;

    /* 填充上报事件 */
    pstCampPlmnInfo           = TAF_SDC_GetCampPlmnInfo();
    stTafRejInfo.enRat        = TAF_MMA_ConvertSdcRatToAppRatType(pstCampPlmnInfo->enSysMode);
    stTafRejInfo.stPlmnId.Mcc = pstCampPlmnInfo->stPlmnId.ulMcc;
    stTafRejInfo.stPlmnId.Mnc = pstCampPlmnInfo->stPlmnId.ulMnc;
    stTafRejInfo.ucSrvDomain  = TAF_MMA_ConvertMmcDomainToMmaDomainType(pstRejInfoInd->enCnDomainId);
    stTafRejInfo.ulRejCause   = pstRejInfoInd->enRejCause;

    /* 拒绝类型为注册被拒 */
    stTafRejInfo.ucRejType    = MMA_MMC_REJ_TYPE_REG;

    /* 将PLMN ID转为BCD格式 */
    MMA_PlmnId2Bcd(&stTafRejInfo.stPlmnId);

    /* 填充消息头 */
    stTafRejInfo.PhoneEvent = TAF_MMA_EVT_REG_REJ_INFO_IND;
    stTafRejInfo.OpId       = MMA_OP_ID_INTERNAL;
    stTafRejInfo.ClientId   = MMA_CLIENTID_BROADCAST;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(stTafRejInfo.ClientId,(VOS_UINT8*)&stTafRejInfo,sizeof(TAF_PH_REG_REJ_INFO_STRU));

    return;
}


VOS_UINT32 TAF_MMA_RcvMmcRegResultInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_REG_RESULT_IND_STRU        *pstRegResultInd   = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_IMS)
    VOS_UINT32                          ulUeSupportIms;
#endif
    pstRegResultInd     = (MMC_MMA_REG_RESULT_IND_STRU *)pstMsg;

#if (FEATURE_ON == FEATURE_IMS)
    /* IMS支持时当PS注册完成后通知一次IMSA,保证异系统场景下,RAT变更时,IMSA感知 */
    if ((VOS_TRUE                 == TAF_SDC_GetImsSupportFlag())
     && ((MMA_MMC_SRVDOMAIN_PS    == pstRegResultInd->enCnDomainId)
      || (MMA_MMC_SRVDOMAIN_CS_PS == pstRegResultInd->enCnDomainId)))
    {
        TAF_MMA_SndImsaSrvInfoNotify((MMA_MMC_SERVICE_STATUS_ENUM_UINT32)TAF_SDC_GetPsServiceStatus());
    }
#endif

    /* 注册失败时且需要上报拒绝原因值 */
    if ((VOS_FALSE == pstRegResultInd->ucRegRslt)
     && (VOS_TRUE  == pstRegResultInd->ucReportCauseCtrl))
    {
        TAF_MMA_SndRegRejInfoInd(pstRegResultInd);
    }

#if (FEATURE_ON == FEATURE_IMS)

    if ((MMA_MMC_SRVDOMAIN_PS    == pstRegResultInd->enCnDomainId)
     || (MMA_MMC_SRVDOMAIN_CS_PS == pstRegResultInd->enCnDomainId))
    {
        /* PS注册失败直接通知IMS不可用 */
        if (VOS_FALSE == pstRegResultInd->ucRegRslt)
        {
            TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);

            /* 停止等待IMSA的IMS voice是否可用消息定时器 */
            TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY);

            /* 通知MMC当前IMS voice是否可用 */
            TAF_MMA_SndMmcImsVoiceCapInd(VOS_FALSE);

            return VOS_TRUE;
        }

        ulUeSupportIms = TAF_SDC_IsImsVoiceSupportedInUE();

        /* 当前IMS的RAT NV不支持或语音优选域为CS ONLY或网络不支持IMS时 认为IMS VOICE不可用*/
        if ((VOS_FALSE == TAF_SDC_IsImsVoiceSupportedInNW())
         || (VOS_FALSE == ulUeSupportIms))
        {
            TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);

            /* 停止等待IMSA的IMS voice是否可用消息定时器 */
            TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY);

            /* 通知MMC当前IMS voice是否可用 */
            TAF_MMA_SndMmcImsVoiceCapInd(VOS_FALSE);

            return VOS_TRUE;
        }

        /* 在PS注册成功时启动保护定时器等待IMS注册完成后上报IMS voice是否可用 */
        TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY, TAF_SDC_GetWaitImsVoiceAvailTimerLen());
    }
#else

    /* 通知MMC当前IMS voice是否可用 */
    TAF_MMA_SndMmcImsVoiceCapInd(VOS_FALSE);
#endif

    return VOS_TRUE;
}


#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 TAF_MMA_RcvImsaImsVoiceCapNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_MMA_IMS_VOICE_CAP_NOTIFY_STRU *pstImsVoiceCap = VOS_NULL_PTR;

    pstImsVoiceCap = (IMSA_MMA_IMS_VOICE_CAP_NOTIFY_STRU *)pstMsg;

    /* 当前如果等待IMS是否可用定时器在运行则处理该消息;否则直接忽略该消息 */
    if (TAF_MMA_TIMER_STATUS_RUNING != TAF_MMA_GetTimerStatus(TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY))
    {
        return VOS_TRUE;
    }

    /* 停止等待IMSA的IMS voice是否可用消息定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY);

    /* 更新IMS voice是否可用信息到SDC中 */
    if (MMA_IMSA_IMS_VOICE_CAP_AVAILABLE == pstImsVoiceCap->enImsVoiceCap)
    {
        TAF_SDC_SetImsVoiceAvailFlg(VOS_TRUE);
    }
    else
    {
        TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);
    }

    /* 通知MMC当前IMS voice是否可用 */
    TAF_MMA_SndMmcImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    /* 通知SPM当前IMS VOICE是否可用 */
    TAF_MMA_SndSpmImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvTiWaitImsaImsVoiceCapNtfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 更新IMS voice不可用信息到SDC中 */
    TAF_SDC_SetImsVoiceAvailFlg(VOS_FALSE);

    /* 通知MMC当前IMS voice不可用 */
    TAF_MMA_SndMmcImsVoiceCapInd(VOS_FALSE);

    /* 通知SPM当前IMS VOICE是否可用 */
    TAF_MMA_SndSpmImsVoiceCapInd(TAF_SDC_GetImsVoiceAvailFlg());

    return VOS_TRUE;
}

#endif


VOS_UINT32 TAF_MMA_RcvMmcUsimAuthFailInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 将MMC的数据类型转换为TAF的数据类型，进行上报 */
    TAF_PH_REG_REJ_INFO_STRU            stTafRejInfo;
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo = VOS_NULL_PTR;
    MMC_MMA_USIM_AUTH_FAIL_IND_STRU    *pstAuthFailInd  = VOS_NULL_PTR;

    pstAuthFailInd            = (MMC_MMA_USIM_AUTH_FAIL_IND_STRU *)pstMsg;

    /* 填充上报事件 */
    pstCampPlmnInfo           = TAF_SDC_GetCampPlmnInfo();
    stTafRejInfo.enRat        = TAF_MMA_ConvertSdcRatToAppRatType(pstCampPlmnInfo->enSysMode);
    stTafRejInfo.stPlmnId.Mcc = pstCampPlmnInfo->stPlmnId.ulMcc;
    stTafRejInfo.stPlmnId.Mnc = pstCampPlmnInfo->stPlmnId.ulMnc;
    stTafRejInfo.ucSrvDomain  = TAF_MMA_ConvertMmcDomainToMmaDomainType(pstAuthFailInd->enCnDomainId);

    /* 为了和网络拒绝原因值区分，usim鉴权失败原因值加上一个偏移位 */
    stTafRejInfo.ulRejCause   = pstAuthFailInd->enRejCause + (1 << 16);

    stTafRejInfo.ucReserved  = 0;
    stTafRejInfo.ucRejType   = MMA_MMC_REJ_TYPE_USIM_AUTH;

    /* 将PLMN ID转为BCD格式 */
    MMA_PlmnId2Bcd(&stTafRejInfo.stPlmnId);

    /* 填充消息头 */
    stTafRejInfo.PhoneEvent = TAF_MMA_EVT_REG_REJ_INFO_IND;
    stTafRejInfo.OpId       = MMA_OP_ID_INTERNAL;
    stTafRejInfo.ClientId   = MMA_CLIENTID_BROADCAST;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(stTafRejInfo.ClientId,(VOS_UINT8*)&stTafRejInfo,sizeof(TAF_PH_REG_REJ_INFO_STRU));

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvMmcCsServiceConnStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU                *pstCsServiceConnStatusInd = VOS_NULL_PTR;

    pstCsServiceConnStatusInd = (MMC_MMA_CS_SERVICE_CONN_STATUS_IND_STRU *)pstMsg;

    TAF_SDC_SetCsServiceConnStatusFlag(pstCsServiceConnStatusInd->ucCsServiceConnStatusFlag);

    return VOS_TRUE;
}



VOS_UINT32 TAF_MMA_RcvMmcServRejInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_SERV_REJ_IND_STRU          *pstServRejInd = VOS_NULL_PTR;
    TAF_PH_REG_REJ_INFO_STRU            stTafServRejInfo;
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo = VOS_NULL_PTR;

    pstServRejInd = (MMC_MMA_SERV_REJ_IND_STRU *) pstMsg;
    PS_MEM_SET(&stTafServRejInfo, 0, sizeof(stTafServRejInfo));

    /* 填充上报事件 */
    pstCampPlmnInfo               = TAF_SDC_GetCampPlmnInfo();
    stTafServRejInfo.enRat        = TAF_MMA_ConvertSdcRatToAppRatType(pstCampPlmnInfo->enSysMode);
    stTafServRejInfo.stPlmnId.Mcc = pstCampPlmnInfo->stPlmnId.ulMcc;
    stTafServRejInfo.stPlmnId.Mnc = pstCampPlmnInfo->stPlmnId.ulMnc;
    stTafServRejInfo.ucSrvDomain  = TAF_MMA_ConvertMmcDomainToMmaDomainType(pstServRejInd->enCnDomainId);

    stTafServRejInfo.ulRejCause   = pstServRejInd->usRejCause;
    stTafServRejInfo.ucRejType    = MMA_MMC_REJ_TYPE_SERV_REQ;

    /* 将PLMN ID转为BCD格式 */
    MMA_PlmnId2Bcd(&stTafServRejInfo.stPlmnId);

    /* 填充消息头 */
    stTafServRejInfo.PhoneEvent = TAF_MMA_EVT_REG_REJ_INFO_IND;
    stTafServRejInfo.OpId       = MMA_OP_ID_INTERNAL;
    stTafServRejInfo.ClientId   = MMA_CLIENTID_BROADCAST;

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg(stTafServRejInfo.ClientId,(VOS_UINT8*)&stTafServRejInfo,sizeof(TAF_PH_REG_REJ_INFO_STRU));
    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvMmcDetachInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_DETACH_IND_STRU            *pstDetachInd = VOS_NULL_PTR;
    TAF_PH_REG_REJ_INFO_STRU            stTafServRejInfo;
    TAF_SDC_CAMP_PLMN_INFO_STRU        *pstCampPlmnInfo = VOS_NULL_PTR;

    pstDetachInd = (MMC_MMA_DETACH_IND_STRU *) pstMsg;
    PS_MEM_SET(&stTafServRejInfo, 0, sizeof(stTafServRejInfo));

    if (MMA_MMC_NETWORK_DETACH_TYPE_NOATTACH == pstDetachInd->enNetworkDetachType)
    {
        /* 填充上报事件 */
        pstCampPlmnInfo               = TAF_SDC_GetCampPlmnInfo();
        stTafServRejInfo.enRat        = TAF_MMA_ConvertSdcRatToAppRatType(pstCampPlmnInfo->enSysMode);
        stTafServRejInfo.stPlmnId.Mcc = pstCampPlmnInfo->stPlmnId.ulMcc;
        stTafServRejInfo.stPlmnId.Mnc = pstCampPlmnInfo->stPlmnId.ulMnc;
        stTafServRejInfo.ucSrvDomain  = TAF_MMA_ConvertMmcDomainToMmaDomainType(pstDetachInd->enCnDomainId);

        stTafServRejInfo.ulRejCause   = pstDetachInd->ulDetachCause;
        stTafServRejInfo.ucRejType    = MMA_MMC_REJ_TYPE_NETWORK_DETACH_IND;

        /* 将PLMN ID转为BCD格式 */
        MMA_PlmnId2Bcd(&stTafServRejInfo.stPlmnId);

        /* 填充消息头 */
        stTafServRejInfo.PhoneEvent = TAF_MMA_EVT_REG_REJ_INFO_IND;
        stTafServRejInfo.OpId       = MMA_OP_ID_INTERNAL;
        stTafServRejInfo.ClientId   = MMA_CLIENTID_BROADCAST;

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg(stTafServRejInfo.ClientId, (VOS_UINT8*)&stTafServRejInfo, sizeof(TAF_PH_REG_REJ_INFO_STRU));
    }

    /* 进状态机处理 */
    return VOS_FALSE;
}



VOS_VOID TAF_MMA_ConvertMmcCauseToTafFormat(
    MMC_MMA_NET_SCAN_CAUSE_ENUM_UINT8   enMmcCause,
    TAF_MMA_NET_SCAN_CAUSE_ENUM_UINT8  *pstMmaCause
)
{
    switch ( enMmcCause )
    {
        case MMC_MMA_NET_SCAN_CAUSE_SIGNAL_EXIST :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_SIGNAL_EXIST;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_STATE_NOT_ALLOWED :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_STATE_NOT_ALLOWED;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_FREQ_LOCK :
           *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_FREQ_LOCK;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_PARA_ERROR :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_PARA_ERROR;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_CONFLICT :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_CONFLICT;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_SERVICE_EXIST :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_SERVICE_EXIST;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_NOT_CAMPED :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_NOT_CAMPED;
            break;

        case MMC_MMA_NET_SCAN_CAUSE_RAT_TYPE_ERROR :
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_RAT_TYPE_ERROR;
            break;

        default:
            *pstMmaCause = TAF_MMA_NET_SCAN_CAUSE_BUTT;
            break;
    }

    return;
}


VOS_VOID TAF_MMA_ConvertMmcBandToTafFormat(
    NAS_MML_MS_BAND_INFO_STRU          *pstMmcBand,
    TAF_USER_SET_PREF_BAND64           *pstUserBand
)
{
    MMA_USER_PREF_BAND_STRU            *pstUserBitBand      = VOS_NULL_PTR;
    GSM_BAND_STRU                      *pstGsmBitBand       = VOS_NULL_PTR;
    WCDMA_BAND_STRU                    *pstWcdmaBitBand     = VOS_NULL_PTR;
    MMA_USER_BAND_SET_UN                uUserSetBand;

    PS_MEM_SET(&uUserSetBand, 0x00, sizeof(MMA_USER_BAND_SET_UN));

    /* uUserSetBand将收到的MMC格式频段保存成32位 */
    pstUserBitBand                        = &(uUserSetBand.BitBand);
    pstGsmBitBand                         = &(pstMmcBand->unGsmBand.stBitBand);
    pstWcdmaBitBand                       = &(pstMmcBand->unWcdmaBand.stBitBand);

    pstUserBitBand->BandGsm850            = pstGsmBitBand->BandGsm850;
    pstUserBitBand->BandGsmDcs1800        = pstGsmBitBand->BandGsm1800;
    pstUserBitBand->BandGsmEgsm900        = pstGsmBitBand->BandGsmE900;
    pstUserBitBand->BandGsmPcs1900        = pstGsmBitBand->BandGsm1900;
    pstUserBitBand->BandGsmPgsm900        = pstGsmBitBand->BandGsmP900;
    pstUserBitBand->BandGsmRgsm900        = pstGsmBitBand->BandGsmR900;

    pstUserBitBand->BandWcdma_I_Imt2k1    = pstWcdmaBitBand->BandWCDMA_I_2100;
    pstUserBitBand->BandWcdma_II_PCS_1900 = pstWcdmaBitBand->BandWCDMA_II_1900;
    pstUserBitBand->BandWcdma_III_1800    = pstWcdmaBitBand->BandWCDMA_III_1800;
    pstUserBitBand->BandWcdma_IV_1700     = pstWcdmaBitBand->BandWCDMA_IV_1700;
    pstUserBitBand->BandWcdma_V_850       = pstWcdmaBitBand->BandWCDMA_V_850;
    pstUserBitBand->BandWcdma_VI_800      = pstWcdmaBitBand->BandWCDMA_VI_800;
    pstUserBitBand->BandWcdma_VII_2600    = pstWcdmaBitBand->BandWCDMA_VII_2600;
    pstUserBitBand->BandWcdma_VIII_900    = pstWcdmaBitBand->BandWCDMA_VIII_900;
    pstUserBitBand->BandWcdma_IX_1700     = pstWcdmaBitBand->BandWCDMA_IX_J1700;
    pstUserBitBand->BandWCDMA_XIX_850     = pstWcdmaBitBand->BandWCDMA_XIX_850;
    pstUserBitBand->BandWcdma_XI_1500     = pstWcdmaBitBand->BandWCDMA_XI_1500;

    /* 将uUserSetBand转换成低32位和高32位结构 */
    pstUserBand->ulBandHigh = uUserSetBand.ulPrefBand & TAF_PH_BAND_NEED_CHANGE_TO_64BIT;
    pstUserBand->ulBandLow  = uUserSetBand.ulPrefBand & (~TAF_PH_BAND_NEED_CHANGE_TO_64BIT);

    return;
}




VOS_VOID TAF_MMA_ConvertMmcNetScanCnfToMmaFormat(
    MMC_MMA_NET_SCAN_CNF_STRU          *pstMsg,
    TAF_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf
)
{
    VOS_UINT32                          i;

    /* 填充消息头 */
    pstNetScanCnf->PhoneEvent  = TAF_MMA_EVT_NET_SCAN_CNF;
    pstNetScanCnf->usClientId  = gstMmaValue.stNetScan.usClientId;
    pstNetScanCnf->ucOpId      = gstMmaValue.stNetScan.ucOpId;

    pstNetScanCnf->enResult    = pstMsg->enResult;
    pstNetScanCnf->ucFreqNum   = pstMsg->ucFreqNum;

    TAF_MMA_ConvertMmcCauseToTafFormat(pstMsg->enCause, &pstNetScanCnf->enCause);

    if ( TAF_MMA_NET_SCAN_MAX_FREQ_NUM < pstNetScanCnf->ucFreqNum )
    {
        pstNetScanCnf->ucFreqNum = TAF_MMA_NET_SCAN_MAX_FREQ_NUM;
    }

    for ( i = 0; i < pstNetScanCnf->ucFreqNum; i++ )
    {
        pstNetScanCnf->astNetScanInfo[i].usArfcn        = pstMsg->astNetScanInfo[i].usArfcn;
        pstNetScanCnf->astNetScanInfo[i].usC1           = pstMsg->astNetScanInfo[i].usC1;
        pstNetScanCnf->astNetScanInfo[i].usC2           = pstMsg->astNetScanInfo[i].usC2;
        pstNetScanCnf->astNetScanInfo[i].usLac          = pstMsg->astNetScanInfo[i].usLac;
        pstNetScanCnf->astNetScanInfo[i].ulMcc          = pstMsg->astNetScanInfo[i].ulMcc;
        pstNetScanCnf->astNetScanInfo[i].ulMnc          = pstMsg->astNetScanInfo[i].ulMnc;
        pstNetScanCnf->astNetScanInfo[i].usBsic         = pstMsg->astNetScanInfo[i].usBsic;
        pstNetScanCnf->astNetScanInfo[i].sRxlev         = pstMsg->astNetScanInfo[i].sRxlev;
        pstNetScanCnf->astNetScanInfo[i].sRssi          = pstMsg->astNetScanInfo[i].sRssi;
        pstNetScanCnf->astNetScanInfo[i].ulCellId       = pstMsg->astNetScanInfo[i].ulCellId;

        TAF_MMA_ConvertMmcBandToTafFormat(&pstMsg->astNetScanInfo[i].stBand,
                                          &pstNetScanCnf->astNetScanInfo[i].stBand);
    }


    return;
}
VOS_UINT32 TAF_MMA_RcvMmcNetScanCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_NET_SCAN_CNF_STRU           stNetScanCnf;
    MMC_MMA_NET_SCAN_CNF_STRU          *pstMmcNetScanCnf = VOS_NULL_PTR;

    pstMmcNetScanCnf = (MMC_MMA_NET_SCAN_CNF_STRU *)pstMsg;

    PS_MEM_SET(&stNetScanCnf, 0x00, sizeof(TAF_MMA_NET_SCAN_CNF_STRU));

    TAF_MMA_ConvertMmcNetScanCnfToMmaFormat(pstMmcNetScanCnf, &stNetScanCnf);

    if ( MMA_TIMER_RUN == g_stNetScanProtectTimer.ucTimerStatus )
    {
        TAF_MMA_StopNetScanProtectTimer();

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg( stNetScanCnf.usClientId,
                       (VOS_UINT8*)&stNetScanCnf,
                       sizeof(TAF_MMA_NET_SCAN_CNF_STRU) );
    }
    else if ( MMA_TIMER_RUN == g_stAbortNetScanProtectTimer.ucTimerStatus )
    {
        TAF_MMA_StopAbortNetScanProtectTimer();

        /* 调用电话管理上报函数 */
        MN_PH_SendMsg( stNetScanCnf.usClientId,
                       (VOS_UINT8*)&stNetScanCnf,
                       sizeof(TAF_MMA_NET_SCAN_CNF_STRU) );
    }
    else
    {
        /* NETSCAN的保护定时器不在运行，则认为是异常回复 */
        MMA_WARNINGLOG("TAF_MMA_RcvMmcNetScanCnf: RCV UNEXPECTED NETSCAN CNF MSG!\r\n");
    }

    return VOS_TRUE;
}
VOS_VOID TAF_MMA_ConvertMmcAbortNetScanCnfToMmaFormat(
    MMC_MMA_ABORT_NET_SCAN_CNF_STRU    *pstMsg,
    TAF_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf
)
{
    VOS_UINT32                          i;

    pstNetScanCnf->PhoneEvent           = TAF_MMA_EVT_NET_SCAN_CNF;
    pstNetScanCnf->ucOpId               = gstMmaValue.stNetScan.ucOpId;
    pstNetScanCnf->usClientId           = gstMmaValue.stNetScan.usClientId;

    pstNetScanCnf->enResult             = TAF_MMA_NET_SCAN_RESULT_SUCCESS;
    pstNetScanCnf->enCause              = TAF_MMA_NET_SCAN_CAUSE_BUTT;
    pstNetScanCnf->ucFreqNum            = pstMsg->ucFreqNum;

    if ( TAF_MMA_NET_SCAN_MAX_FREQ_NUM < pstNetScanCnf->ucFreqNum )
    {
        pstNetScanCnf->ucFreqNum = TAF_MMA_NET_SCAN_MAX_FREQ_NUM;
    }

    for ( i = 0; i < pstNetScanCnf->ucFreqNum; i++ )
    {
        pstNetScanCnf->astNetScanInfo[i].usArfcn        = pstMsg->astNetScanInfo[i].usArfcn;
        pstNetScanCnf->astNetScanInfo[i].usC1           = pstMsg->astNetScanInfo[i].usC1;
        pstNetScanCnf->astNetScanInfo[i].usC2           = pstMsg->astNetScanInfo[i].usC2;
        pstNetScanCnf->astNetScanInfo[i].usLac          = pstMsg->astNetScanInfo[i].usLac;
        pstNetScanCnf->astNetScanInfo[i].ulMcc          = pstMsg->astNetScanInfo[i].ulMcc;
        pstNetScanCnf->astNetScanInfo[i].ulMnc          = pstMsg->astNetScanInfo[i].ulMnc;
        pstNetScanCnf->astNetScanInfo[i].usBsic         = pstMsg->astNetScanInfo[i].usBsic;
        pstNetScanCnf->astNetScanInfo[i].sRxlev         = pstMsg->astNetScanInfo[i].sRxlev;
        pstNetScanCnf->astNetScanInfo[i].sRssi          = pstMsg->astNetScanInfo[i].sRssi;
        pstNetScanCnf->astNetScanInfo[i].ulCellId       = pstMsg->astNetScanInfo[i].ulCellId;

        TAF_MMA_ConvertMmcBandToTafFormat(&pstMsg->astNetScanInfo[i].stBand,
                                          &pstNetScanCnf->astNetScanInfo[i].stBand);
    }

    return;
}



VOS_UINT32 TAF_MMA_RcvMmcAbortNetScanCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_NET_SCAN_CNF_STRU           stNetScanCnf;
    MMC_MMA_ABORT_NET_SCAN_CNF_STRU    *pstMmcAbortNetScanCnf = VOS_NULL_PTR;

    pstMmcAbortNetScanCnf = (MMC_MMA_ABORT_NET_SCAN_CNF_STRU *)pstMsg;

    if ( MMA_TIMER_RUN != g_stAbortNetScanProtectTimer.ucTimerStatus )
    {
        MMA_WARNINGLOG("TAF_MMA_RcvMmcAbortNetScanCnf:abort timer not run!!!\r\n");

        return VOS_TRUE;
    }

    PS_MEM_SET(&stNetScanCnf, 0x00, sizeof(stNetScanCnf));

    /* 组装给AT模块回复的CNF结构 */
    TAF_MMA_ConvertMmcAbortNetScanCnfToMmaFormat(pstMmcAbortNetScanCnf, &stNetScanCnf);

    /* 停止NETSCAN的保护定时器 */
    TAF_MMA_StopAbortNetScanProtectTimer();

    /* 调用电话管理上报函数 */
    MN_PH_SendMsg( stNetScanCnf.usClientId,
                   (VOS_UINT8*)&stNetScanCnf,
                   sizeof(TAF_MMA_NET_SCAN_CNF_STRU) );

    return VOS_TRUE;
}


#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_UINT32 TAF_MMA_RcvMtcNcellInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 转发给mmc */
    TAF_MMA_SndMmcNcellInfoInd(pstMsg);
    return VOS_TRUE;
}




VOS_UINT32 TAF_MMA_RcvMtcPsTransferInd_PreProc(
     VOS_UINT32                         ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 转发给mmc */
    TAF_MMA_SndMmcPsTransferInd(pstMsg);
    return VOS_TRUE;
}





VOS_UINT32 TAF_MMA_RcvMtcOtherModemInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 转发给mmc */
    TAF_MMA_SndMmcOtherModemInfoNotify(pstMsg);

    return VOS_TRUE;
}
#endif
VOS_UINT32 TAF_MMA_RcvMmcEplmnInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    MMC_MMA_EPLMN_INFO_IND_STRU        *pstEplmnInfoIndMsg  = VOS_NULL_PTR;

    pstEplmnInfoIndMsg   = (MMC_MMA_EPLMN_INFO_IND_STRU *)pstMsg;

    TAF_MMA_SndMtcEplmnInfoInd(pstEplmnInfoIndMsg);

#endif
    return VOS_TRUE;
}





VOS_UINT32 TAF_MMA_RcvMmaAcqReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_ACQ_REQ_STRU               *pstMmaAcqReqMsg    = VOS_NULL_PTR;
    MMC_MMA_ACQ_CNF_STRU                stSndMsg;
    VOS_UINT8                           ucCtxIndex;
    VOS_UINT32                          ulRet;

    pstMmaAcqReqMsg = (TAF_MMA_ACQ_REQ_STRU *)pstMsg;

    PS_MEM_SET(&stSndMsg, 0, sizeof(MMC_MMA_ACQ_CNF_STRU));

    /* 如果是关机状态，或者是正在开关机，则回复拒绝 */
    if ((TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
     || (STA_FSM_NULL           == g_StatusContext.ulFsmState)
     || (STA_FSM_STOP           == g_StatusContext.ulFsmState))
    {
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaAcqReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_REJECT,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果不是CL模式，且是CMMCA消息，则上报CMMCA拒绝 */
    if ((VOS_FALSE              == TAF_SDC_IsConfigCLInterWork())
     && (CMMCA_CLIENT_ID        == pstMmaAcqReqMsg->stCtrl.usClientId))
    {
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaAcqReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_REJECT,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果卡无效，直接回失败 */
    if (VOS_FALSE == TAF_SDC_IsUsimStausValid())
    {
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaAcqReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果g_stTafMmaCtx.astMmaOperCtx里TAF_MMA_OPER_ACQ_REQ类型结构正在被使用 */
    if (VOS_TRUE == TAF_MMA_IsOperTypeUsed(TAF_MMA_OPER_ACQ_REQ))
    {
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaAcqReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 找不到未被使用的，回复失败 */
    if (VOS_FALSE == TAF_MMA_GetUnusedOperCtxIndex(&ucCtxIndex))
    {
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaAcqReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 分配一个g_stTafMmaCtx.astMmaOperCtx到对应操作 */
    TAF_MMA_SetOperCtx(pstMmaAcqReqMsg->stCtrl,
                       TAF_MMA_OPER_ACQ_REQ, ucCtxIndex);

    /* 给MMC发acq req消息 */
    ulRet = TAF_MMA_SndMmcAcqReq(&(pstMmaAcqReqMsg->stAcqPara));
    if (VOS_OK != ulRet)
    {
        return VOS_TRUE;
    }

    /* 启动保护定时器TI_TAF_MMA_WAIT_MMC_ACQ_CNF */
    TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_MMC_ACQ_CNF, TI_TAF_MMA_WAIT_MMC_ACQ_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_ConvertAttachDomainToAttachType(
    TAF_PH_SERVICE_DOMAIN               ucAttachDomain
)
{
    VOS_UINT32                          ulAttachType;

    switch ( ucAttachDomain )
    {
        case TAF_PH_SERVICE_CS:
            ulAttachType = STA_ATTACH_TYPE_IMSI;
            break;

        case TAF_PH_SERVICE_PS:
            ulAttachType = STA_ATTACH_TYPE_GPRS;
            break;

        case TAF_PH_SERVICE_CS_PS:
        case TAF_PH_SERVICE_ANY:
            ulAttachType = STA_ATTACH_TYPE_GPRS_IMSI;
            break;

        case TAF_PH_SERVICE_NULL:
            ulAttachType = STA_ATTACH_TYPE_NULL;
            break;

        case TAF_PH_SERVICE_NOCHANGE:
        default:
            ulAttachType = STA_ATTACH_TYPE_GPRS;
            break;

    }

    return ulAttachType;

}


VOS_UINT32 TAF_MMA_RcvMmaRegReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_REG_REQ_STRU               *pstMmaRegReqMsg = VOS_NULL_PTR;
    MMC_MMA_REG_CNF_STRU                stSndMsg;
    VOS_UINT8                           ucCtxIndex;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulAttachType;

    pstMmaRegReqMsg = (TAF_MMA_REG_REQ_STRU*)pstMsg;
    PS_MEM_SET(&stSndMsg, 0, sizeof(MMC_MMA_REG_CNF_STRU));

    /* 如果是关机状态，或者是正在开关机，则回复拒绝 */
    if ((TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
     || (STA_FSM_NULL           == g_StatusContext.ulFsmState)
     || (STA_FSM_STOP           == g_StatusContext.ulFsmState))
    {
        TAF_MMA_SndRegCnf(&(pstMmaRegReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_REJECT,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果不是CL模式，且是CMMCA消息，则上报CMMCA拒绝 */
    if ((VOS_FALSE              == TAF_SDC_IsConfigCLInterWork())
     && (CMMCA_CLIENT_ID        == pstMmaRegReqMsg->stCtrl.usClientId))
    {
        TAF_MMA_SndRegCnf(&(pstMmaRegReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_REJECT,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果卡无效，直接回失败 */
    if (VOS_FALSE == TAF_SDC_IsUsimStausValid())
    {
        TAF_MMA_SndRegCnf(&(pstMmaRegReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 如果g_stTafMmaCtx.astMmaOperCtx里TAF_MMA_OPER_REG_REQ类型结构正在被使用 */
    if (VOS_TRUE == TAF_MMA_IsOperTypeUsed(TAF_MMA_OPER_REG_REQ))
    {
        TAF_MMA_SndRegCnf(&(pstMmaRegReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 找不到未被使用的，回复失败  */
    if (VOS_FALSE == TAF_MMA_GetUnusedOperCtxIndex(&ucCtxIndex))
    {
        TAF_MMA_SndRegCnf(&(pstMmaRegReqMsg->stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);
        return VOS_TRUE;
    }

    /* 分配一个g_stTafMmaCtx.astMmaOperCtx到对应操作 */
    TAF_MMA_SetOperCtx(pstMmaRegReqMsg->stCtrl,
                       TAF_MMA_OPER_REG_REQ, ucCtxIndex);

    /* 给MMC发reg req消息 */
    ulRet = TAF_MMA_SndMmcRegReq(&(pstMmaRegReqMsg->stRegPara));
    if (VOS_OK != ulRet)
    {
        TAF_MMA_ClearOperCtx(ucCtxIndex);
        return VOS_TRUE;
    }

    ulAttachType = TAF_MMA_ConvertAttachDomainToAttachType(pstMmaRegReqMsg->stRegPara.ucAttachDomain);

    /* 发送attach */
    ulRet = Sta_Attach(STA_OPID_ATTATCH,
                       ulAttachType,
                       pstMmaRegReqMsg->stRegPara.enEpsAttachReason);

    if (STA_ERROR == ulRet)
    {
        MMA_WARNINGLOG("TAF_MMA_RcvMmaRegReq_PreProc:Sta Attach fail!");
    }

    /* 启动保护定时器TI_TAF_MMA_WAIT_MMC_REG_CNF */
    TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_MMC_REG_CNF, TI_TAF_MMA_WAIT_MMC_REG_CNF_LEN);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmaPowerSaveReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_POWER_SAVE_REQ_STRU        *pstMmaPowerSaveReqMsg  = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;
    VOS_UINT32                          ulRet;

    pstMmaPowerSaveReqMsg  = (TAF_MMA_POWER_SAVE_REQ_STRU*)pstMsg;

    /* 如果是开关机过程中，直接回复拒绝 */
    if (TAF_MMA_FSM_PHONE_MODE == TAF_MMA_GetCurrFsmId())
    {
        TAF_MMA_SndPowerSaveCnf(&(pstMmaPowerSaveReqMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_REJECT);
        return VOS_TRUE;
    }

    /* 如果不是CL模式，且是CMMCA消息，则上报CMMCA拒绝 */
    if ((VOS_FALSE             == TAF_SDC_IsConfigCLInterWork())
     && (CMMCA_CLIENT_ID       == pstMmaPowerSaveReqMsg->stCtrl.usClientId))
    {
        TAF_MMA_SndPowerSaveCnf(&(pstMmaPowerSaveReqMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_REJECT);
        return VOS_TRUE;
    }

    /* 如果已经关机，直接回复成功 */
    if ((STA_FSM_NULL    == g_StatusContext.ulFsmState)
     || (STA_FSM_STOP    == g_StatusContext.ulFsmState))
    {
        TAF_MMA_SndPowerSaveCnf(&(pstMmaPowerSaveReqMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_SUCCESS);
        return VOS_TRUE;
    }

    /* 如果g_stTafMmaCtx.astMmaOperCtx里TAF_MMA_OPER_POWER_SAVE_REQ类型结构正在被使用 */
    if (VOS_TRUE == TAF_MMA_IsOperTypeUsed(TAF_MMA_OPER_POWER_SAVE_REQ))
    {
        TAF_MMA_SndPowerSaveCnf(&(pstMmaPowerSaveReqMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_FAILURE);
        return VOS_TRUE;
    }

    /* 找不到未被使用的，回复失败  */
    if (VOS_FALSE == TAF_MMA_GetUnusedOperCtxIndex(&ucCtxIndex))
    {
        TAF_MMA_SndPowerSaveCnf(&(pstMmaPowerSaveReqMsg->stCtrl),
                                TAF_MMA_APP_OPER_RESULT_FAILURE);
        return VOS_TRUE;
    }

    /* 分配一个g_stTafMmaCtx.astMmaOperCtx到对应操作 */
    TAF_MMA_SetOperCtx(pstMmaPowerSaveReqMsg->stCtrl,
                       TAF_MMA_OPER_POWER_SAVE_REQ, ucCtxIndex);

    /* 给MMC发power save req消息 */
    ulRet = TAF_MMA_SndMmcPowerSaveReq(&(pstMmaPowerSaveReqMsg->stPowerSavePara));
    if (VOS_OK != ulRet)
    {
        return VOS_TRUE;
    }

    /* 启动保护定时器TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF */
    TAF_MMA_StartTimer(TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF,
                       TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_RcvMmcAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_ACQ_CNF_STRU               *pstMmcMmaAcqCnf = VOS_NULL_PTR;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt;

    pstMmcMmaAcqCnf = (MMC_MMA_ACQ_CNF_STRU *)pstMsg;
    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();
    enRslt          = TAF_MMA_APP_OPER_RESULT_SUCCESS;

    if (MMC_MMA_ACQ_RESULT_SUCCESS != pstMmcMmaAcqCnf->enAcqRslt)
    {
        enRslt = TAF_MMA_APP_OPER_RESULT_FAILURE;
    }

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_ACQ_REQ对应的记录项 */
    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_ACQ_REQ, &ucCtxIndex))
    {
        /*  回复MMA_ACQ_CNF */
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                          enRslt,
                          pstMmcMmaAcqCnf);

        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }

    /* 停止保护定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_MMC_ACQ_CNF);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
VOS_UINT32 TAF_MMA_RcvMmcAcqInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_ACQ_IND_STRU               *pstMmcMmaAcqInd = VOS_NULL_PTR;
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt;

    pstMmcMmaAcqInd = (MMC_MMA_ACQ_IND_STRU *)pstMsg;
    enRslt          = TAF_MMA_APP_OPER_RESULT_SUCCESS;

    if (MMC_MMA_ACQ_RESULT_SUCCESS != pstMmcMmaAcqInd->enAcqRslt)
    {
        enRslt = TAF_MMA_APP_OPER_RESULT_FAILURE;
    }

    /* 回复MMA_ACQ_IND */
    TAF_MMA_SndAcqInd(enRslt, pstMmcMmaAcqInd);

    return VOS_TRUE;
}
#endif
VOS_UINT32 TAF_MMA_RcvMmcRegCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MMA_REG_CNF_STRU               *pstMmcMmaRegCnf = VOS_NULL_PTR;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;
    TAF_MMA_APP_OPER_RESULT_ENUM_UINT32 enRslt;

    pstMmcMmaRegCnf = (MMC_MMA_REG_CNF_STRU *)pstMsg;
    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    switch (pstMmcMmaRegCnf->enRegRslt)
    {
        case MMC_MMA_REG_RESULT_SUCCESS:
            enRslt = TAF_MMA_APP_OPER_RESULT_SUCCESS;
            break;

        case MMC_MMA_REG_RESULT_ACQ_FAILURE:
            enRslt = TAF_MMA_APP_OPER_RESULT_ACQUIRED_FAIL;
            break;

        case MMC_MMA_REG_RESULT_REG_FAILURE:
            enRslt = TAF_MMA_APP_OPER_RESULT_FAILURE;
            break;

        default:
            enRslt = TAF_MMA_APP_OPER_RESULT_ACQUIRED_FAIL;
            break;
    }

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_REG_REQ对应的记录项 */
    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_REG_REQ, &ucCtxIndex))
    {

        /*  回复MMA_REG_CNF */
        TAF_MMA_SndRegCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                          enRslt,
                          pstMmcMmaRegCnf);

        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }

    /* 停止保护定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_MMC_REG_CNF);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvMmcPowerSaveCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    TAF_MMA_CTRL_STRU                   stCtrlInfo;
    VOS_UINT8                           ucCtxIndex;

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_ACQ_REQ对应的记录项 */
    if (VOS_TRUE != TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_POWER_SAVE_REQ, &ucCtxIndex))
    {
        MMA_WARNINGLOG("TAF_MMA_RcvMmcPowerSaveCnf_PreProc:NOT FIND TAF_MMA_OPER_POWER_SAVE_REQ!");

        TAF_MMA_BuildMmaCtrlInfo(WUEPS_PID_CMMCA, CMMCA_CLIENT_ID, 0, &stCtrlInfo);
        TAF_MMA_SndPowerSaveCnf(&stCtrlInfo,
                                TAF_MMA_APP_OPER_RESULT_SUCCESS);

        return VOS_TRUE;
    }

    /* 回复MMA_POWER_SAVE_CNF */
    TAF_MMA_SndPowerSaveCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                            TAF_MMA_APP_OPER_RESULT_SUCCESS);

    TAF_MMA_ClearOperCtx(ucCtxIndex);

    /* 停止保护定时器 */
    TAF_MMA_StopTimer(TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF);

    return VOS_TRUE;
}
VOS_UINT32 TAF_MMA_RcvTiWaitMmcAcqCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    MMC_MMA_ACQ_CNF_STRU                stSndMsg;
    VOS_UINT8                           ucCtxIndex;

    PS_MEM_SET(&stSndMsg, 0, sizeof(MMC_MMA_ACQ_CNF_STRU));
    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    stSndMsg.stPlmnId.ulMcc = MMA_INVALID_MCC;
    stSndMsg.stPlmnId.ulMnc = MMA_INVALID_MNC;

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_ACQ_REQ对应的记录项 */
    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_ACQ_REQ, &ucCtxIndex))
    {
        /*  回复MMA_ACQ_CNF */
        TAF_MMA_SndAcqBestNetworkCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE,
                          &stSndMsg);

        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_MMA_RcvTiWaitMmcRegCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    MMC_MMA_REG_CNF_STRU                stSndMsg;
    VOS_UINT8                           ucCtxIndex;

    PS_MEM_SET(&stSndMsg, 0, sizeof(MMC_MMA_REG_CNF_STRU));
    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    stSndMsg.stPlmnId.ulMcc = MMA_INVALID_MCC;
    stSndMsg.stPlmnId.ulMnc = MMA_INVALID_MNC;

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_REG_REQ对应的记录项 */
    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_REG_REQ, &ucCtxIndex))
    {
        /*  回复MMA_REG_CNF */
        TAF_MMA_SndRegCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                          TAF_MMA_APP_OPER_RESULT_ACQUIRED_FAIL,
                          &stSndMsg);

        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_MMA_RcvTiWaitMmcPowerSaveExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx   = VOS_NULL_PTR;
    VOS_UINT8                           ucCtxIndex;

    pstMmaOperCtx   = TAF_MMA_GetOperCtxAddr();

    /* 从全局变量g_stTafMmaCtx.astMmaOperCtx查找TAF_MMA_OPER_POWER_SAVE_REQ对应的记录项 */
    if (VOS_TRUE == TAF_MMA_GetSpecOperTypeIndex(TAF_MMA_OPER_POWER_SAVE_REQ, &ucCtxIndex))
    {
        /*  回复MMA_POWER_SAVE_CNF */
        TAF_MMA_SndPowerSaveCnf(&(pstMmaOperCtx[ucCtxIndex].stCtrl),
                          TAF_MMA_APP_OPER_RESULT_FAILURE);

        TAF_MMA_ClearOperCtx(ucCtxIndex);
    }

    return VOS_TRUE;

}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


