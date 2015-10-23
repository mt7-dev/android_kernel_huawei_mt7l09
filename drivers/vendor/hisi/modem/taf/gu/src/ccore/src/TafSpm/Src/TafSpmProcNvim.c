

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "pslog.h"
#include "om.h"
#include "PsTypeDef.h"
#include "TafSpmCtx.h"
#include "TafLog.h"
#include "PsNvId.h"
#include "NVIM_Interface.h"
#include "TafNvInterface.h"
#include "MnComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_TAF_SPM_PROC_NVIM_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/


/*****************************************************************************
  6 函数定义
*****************************************************************************/

VOS_VOID TAF_SPM_ReadFdnInfoNvim(VOS_VOID)
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU *pstServiceCtrlCfgInfo = VOS_NULL_PTR;
    TAF_FDN_NVIM_CONFIG_STRU            stFdnConfig;

    pstServiceCtrlCfgInfo               = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    if (NV_OK != NV_Read(en_NV_Item_FDN_Info,
                         &stFdnConfig,
                         sizeof(TAF_FDN_NVIM_CONFIG_STRU)))
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_SPM_ReadNvimInfo():WARNING: en_NV_Item_FDN_Info Error");
        pstServiceCtrlCfgInfo->ulMeStatus = TAF_FDN_CHECK_FEATURE_OFF;

        return;
    }

    pstServiceCtrlCfgInfo->ulMeStatus   = stFdnConfig.ulMeStatus;

    return;
}


VOS_VOID TAF_SPM_ReadSimCallCtrlNvim(VOS_VOID)
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU                     *pstServiceCtrlCfgInfo = VOS_NULL_PTR;
    MN_CALL_NVIM_SIM_CALL_CONTROL_FLG_STRU                  stSimCallCtrlFlg;

    pstServiceCtrlCfgInfo                           = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    if (NV_OK != NV_Read(en_NV_Item_NVIM_SIM_CALL_CONTROL_SUPPORT_FLG,
                        &stSimCallCtrlFlg,
                        sizeof(MN_CALL_NVIM_SIM_CALL_CONTROL_FLG_STRU)))
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_SPM_ReadNvimInfo():WARNING: en_NV_Item_NVIM_SIM_CALL_CONTROL_SUPPORT_FLG Error");
        pstServiceCtrlCfgInfo->ucSimCallCtrlSupportFlg  = VOS_FALSE;

        return;
    }

    pstServiceCtrlCfgInfo->ucSimCallCtrlSupportFlg = stSimCallCtrlFlg.ucSimCallCtrlSupportFlg;

    return;
}


VOS_VOID TAF_SPM_ReadVideoCallNvim(VOS_VOID)
{
    TAF_SPM_SERVICE_CTRL_CFG_INFO_STRU                     *pstServiceCtrlCfgInfo = VOS_NULL_PTR;
    MN_CALL_NV_ITEM_VIDEO_CALL_STRU                         stVideoCall;

    pstServiceCtrlCfgInfo                           = TAF_SPM_GetServiceCtrlCfgInfoAddr();

    if (NV_OK != NV_Read(en_NV_Item_VIDEO_CALL,
                        &stVideoCall,
                        sizeof(MN_CALL_NV_ITEM_VIDEO_CALL_STRU)))
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_SPM_ReadVideoCallNvim():WARNING: en_NV_Item_VIDEO_CALL Error");

        return;
    }

    if (VOS_TRUE == stVideoCall.ucStatus)
    {
        pstServiceCtrlCfgInfo->enVpCfgState = stVideoCall.ucVpCfgState;        
    }

    return;
}



VOS_VOID TAF_SPM_ReadNvimInfo(VOS_VOID)
{
    TAF_SPM_ReadFdnInfoNvim();

    TAF_SPM_ReadSimCallCtrlNvim();

    TAF_SPM_ReadVideoCallNvim();

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

