/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: LightLED.c                                                      */
/*                                                                           */
/* Author: Xu cheng                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-06                                                             */
/*                                                                           */
/* Description: Turn on LED according to some Events which be supported by PS*/
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-06                                                          */
/*    Author: Xu cheng                                                       */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "lightled.h"
#include "DrvInterface.h"
#include "UsimPsInterface.h"
#include "apminterface.h"
#if (RAT_MODE != RAT_GU)
#include "TafOamInterface.h"
#endif

VOS_INT g_lOldLedState = LED_LIGHT_POWER_ON;


/*****************************************************************************
 Function   : OM_GetLedState
 Description: Get led state from the PS state.
 Input      : event
 Return     : void
 Other      :
 *****************************************************************************/
VOS_INT OM_GetLedState(USIMM_CARD_SERVIC_ENUM_UINT32 ucCardStatus,
                            WAS_MNTN_OM_OUT_OF_SERVICE_ENUM_UINT8 enIsOutService,
                            NAS_OM_REGISTER_STATE_ENUM_UINT8 enRegState,
                            NAS_OM_SERVICE_TYPE_ENUM_UINT8 enServiceType)
{
#if (RAT_MODE != RAT_GU)
    TAF_APS_USER_CONN_STATUS_ENUM_UINT8 ucConnStatus;
#endif

#if (RAT_MODE == RAT_GU)
    VOS_UINT32    ulRatType;

    /*无卡 */
    if (USIMM_CARD_SERVIC_ABSENT == ucCardStatus)
    {
        return LED_LIGHT_SIM_ABSENT;
    }
#endif

    /*未启动协议栈*/
    if (NAS_OM_REGISTER_STATE_POWEROFF == enRegState)
    {
        return LED_LIGHT_OFFLINE;
    }

#if (RAT_MODE == RAT_GU)
    /*出服务区 + 无网或者正在搜索网络*/
    if ((WAS_MNTN_OM_OUT_SERVICE == enIsOutService)
        || (NAS_OM_REGISTER_STATE_NOT_REGISTER == enRegState))
    {
        ulRatType = SHPA_GetRateType(MODEM_ID_0);

        if (VOS_RATMODE_GSM == ulRatType)
        {
            return LED_LIGHT_G_PLMN_SEARCH;
        }

        if (VOS_RATMODE_WCDMA == ulRatType)
        {
            return LED_LIGHT_W_PLMN_SEARCH;
        }

        return LED_LIGHT_OFFLINE;
    }
#endif

#if (RAT_MODE != RAT_GU)
    if ((WAS_MNTN_OM_OUT_SERVICE == enIsOutService)
        || (NAS_OM_REGISTER_STATE_NOT_REGISTER == enRegState))
    {
        return LED_LIGHT_OFFLINE;
    }
	
    ucConnStatus = TAF_APS_GetUserConnStatus();
#endif


    /*有网络，无业务*/
    if (NAS_OM_SERVICE_TYPE_NO_SERVICE == enServiceType)
    {
        /*GSM 绿灯单闪*/
        if (NAS_OM_REGISTER_STATE_REGISTER_2G == enRegState)
        {
            return LED_LIGHT_G_REGISTERED;
        }

        /*WCDMA 蓝灯单闪*/
        if (NAS_OM_REGISTER_STATE_REGISTER_3G == enRegState)
        {
            return LED_LIGHT_W_REGISTERED;
        }

#if (RAT_MODE != RAT_GU)
        /*LTE 青灯单闪*/
        if (NAS_OM_REGISTER_STATE_REGISTER_4G == enRegState)
        {
            /* 拨号后处于IDLE态认为处于LTE业务状态 */
            if(TAF_APS_USER_CONN_EXIST == ucConnStatus)
            {
                return LED_LIGHT_H_CONNNECTED;
            }
            else
            {
                return LED_LIGHT_L_REGISTERED;
            }
        }
#endif
    }

    /*有网路，有GSM业务*/
    if (NAS_OM_SERVICE_TYPE_GSM == enServiceType)
    {
#if (RAT_MODE != RAT_GU)
        return (TAF_APS_USER_CONN_EXIST == ucConnStatus) ? LED_LIGHT_G_CONNECTED : LED_LIGHT_G_REGISTERED;
#elif (RAT_MODE == RAT_GU)
        return LED_LIGHT_G_CONNECTED;
#endif
    }

    /*有网路，有WCDMA业务*/
    if (NAS_OM_SERVICE_TYPE_WCDMA == enServiceType)
    {
#if (RAT_MODE != RAT_GU)
        return (TAF_APS_USER_CONN_EXIST == ucConnStatus) ? LED_LIGHT_W_CONNNECTED : LED_LIGHT_W_REGISTERED;
#elif (RAT_MODE == RAT_GU)
        return LED_LIGHT_W_CONNNECTED;
#endif
    }

    /*有网路，有HSPA业务*/
    if (NAS_OM_SERVICE_TYPE_HSPA == enServiceType)
    {
#if (RAT_MODE != RAT_GU)
        return (TAF_APS_USER_CONN_EXIST == ucConnStatus) ? LED_LIGHT_H_CONNNECTED : LED_LIGHT_W_REGISTERED;
#elif (RAT_MODE == RAT_GU)
        return LED_LIGHT_H_CONNNECTED;
#endif
    }

#if (RAT_MODE != RAT_GU)
    /*有LTE业务 或 拨号后处于IDLE态都认为处于LTE业务状态*/
    if ((NAS_OM_SERVICE_TYPE_LTE == enServiceType) || (NAS_OM_REGISTER_STATE_REGISTER_4G == enRegState))
    {
        return (TAF_APS_USER_CONN_EXIST == ucConnStatus) ? LED_LIGHT_H_CONNNECTED : LED_LIGHT_L_REGISTERED;
    }
#endif

    return LED_LIGHT_OFFLINE;
}


/*****************************************************************************
 Function   : OM_RuningPsStateCheckCb
 Description: Get the current PS state called by driver.
 Input      : VOS_VOID
 Return     : void
 Other      :
 *****************************************************************************/
VOS_INT OM_RuningPsStateCheckCb(VOS_VOID)
{
    WAS_MNTN_OM_OUT_OF_SERVICE_ENUM_UINT8   enIsOutService;
    NAS_OM_REGISTER_STATE_ENUM_UINT8        enRegState;
    NAS_OM_SERVICE_TYPE_ENUM_UINT8          enServiceType;
    VOS_INT                                 lCurLedState;
    LED_STATE_STRUCT                        stMsg;
    VOS_UINT8                               ucCardStatus;

    enIsOutService  = WAS_MNTN_GetOmOutOfService();
    enRegState      = NAS_GetRegisterState();
    enServiceType   = NAS_GetServiceType();

    USIMM_GetCardType(&ucCardStatus, VOS_NULL_PTR);

    lCurLedState    = OM_GetLedState((USIMM_CARD_SERVIC_ENUM_UINT32)ucCardStatus,
                                enIsOutService, enRegState, enServiceType);

    /*假如有LED灯有变化，则上报给SDT工具显示*/
    if (g_lOldLedState != lCurLedState)
    {
        stMsg.ulSenderCpuId     = VOS_LOCAL_CPUID;
        stMsg.ulSenderPid       = PC_PID_TOOL;
        stMsg.ulReceiverCpuId   = VOS_LOCAL_CPUID;
        stMsg.ulReceiverPid     = PC_PID_TOOL;
        stMsg.ulLength          = sizeof(LED_STATE_STRUCT) - VOS_MSG_HEAD_LENGTH;
        stMsg.ulMsgName         = OAM_LED_STATE_CNF;

        stMsg.lOldLedState      = g_lOldLedState;
        stMsg.lNewLedState      = lCurLedState;
        stMsg.enIsOutService    = enIsOutService;
        stMsg.enRegState        = enRegState;
        stMsg.enServiceType     = enServiceType;
        stMsg.ucCardStatus      = ucCardStatus;
        stMsg.ulRatType         = SHPA_GetRateType(MODEM_ID_0);

        OM_TraceMsgHook(&stMsg);

        g_lOldLedState = lCurLedState;
    }

    return lCurLedState;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




