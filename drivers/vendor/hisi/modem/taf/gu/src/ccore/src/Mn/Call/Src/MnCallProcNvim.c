
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MnCallCtx.h"
#include "MnCallProcNvim.h"
#include "NVIM_Interface.h"
#include "MnCall.h"

#include "MnCallTimer.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"
#include "NasUsimmApi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_MN_CALL_PROC_NIVM_C

/*****************************************************************************
  2 全局变量
*****************************************************************************/
extern VOS_UINT8 gucTafCallStatusControl;
extern VOS_UINT8 gucTafMultiSimCallStatusControl;
extern MN_CALL_VP_NV_CFG_STATE_ENUM_U8 g_enVpNvCfgState;  /* NV项，用控制是否支持可视电话的特性 */

/*****************************************************************************
  3 宏定义
*****************************************************************************/

/*****************************************************************************
   4 函数实现
*****************************************************************************/


VOS_VOID  MN_CALL_ReadCustomizeServiceNvim( VOS_VOID )
{
    NAS_NVIM_CUSTOMIZE_SERVICE_STRU               stCustomServ;
    VOS_UINT32                                    ulResult;


    stCustomServ.ulStatus           = VOS_FALSE;
    stCustomServ.ulCustomizeService = VOS_FALSE;

    /* 读取对应的几个NV项，并保存到全局变量中 */
    ulResult = NV_Read(en_NV_Item_CustomizeService,
                       &stCustomServ,
                       sizeof(stCustomServ));

    /* PC工程,回放工程,CC默认打开 */
#if (defined(__PS_WIN32_RECUR__))
        stCustomServ.ulStatus = VOS_TRUE;
        stCustomServ.ulCustomizeService = VOS_TRUE;
#endif

    if((ulResult == NV_OK)
    &&(VOS_TRUE == stCustomServ.ulStatus)
    &&(VOS_TRUE == (stCustomServ.ulCustomizeService&0x01)))
    {
       gucTafCallStatusControl = VOS_TRUE;
    }
    else
    {
       gucTafCallStatusControl = VOS_FALSE;
    }

    return;
}


VOS_VOID  MN_CAL_ReadMultiSimCallConfNvim( VOS_VOID )
{
    MN_CALL_NV_ITEM_MULTISIM_CALLCON_STRU         stMultiSimCallCon;
    VOS_UINT32                                    ulResult;


    stMultiSimCallCon.usMultiSimCallConf = VOS_FALSE;

    ulResult = NV_Read(en_NV_Item_MultiSimCallConf,
                      &stMultiSimCallCon,
                      sizeof(stMultiSimCallCon));

    if((ulResult == NV_OK)
    &&(VOS_TRUE == stMultiSimCallCon.usMultiSimCallConf))
    {
       gucTafMultiSimCallStatusControl = VOS_TRUE;
    }
    else
    {
       gucTafMultiSimCallStatusControl = VOS_FALSE;
    }

    return;
}


VOS_VOID  MN_CAL_ReadVideoCallNvim( VOS_VOID )
{
    MN_CALL_NV_ITEM_VIDEO_CALL_STRU               stVideoCall;
    VOS_UINT32                                    ulResult;
    VOS_UINT32                                    ulLength;

    ulLength                 = 0;

    stVideoCall.ucStatus     = VOS_FALSE;
    stVideoCall.ucVpCfgState = MN_CALL_VP_BUTT;
    
    NV_GetLength(en_NV_Item_VIDEO_CALL, &ulLength);	

    ulResult = NV_Read(en_NV_Item_VIDEO_CALL,
                       &stVideoCall,
                       ulLength);

    /* PC工程,回放工程,CC默认打开 */
#if (defined(__PS_WIN32_RECUR__))
        stVideoCall.ucStatus = VOS_TRUE;
        stVideoCall.ucVpCfgState = MN_CALL_VP_MO_MT_BOTH;
#endif

     if((ulResult == NV_OK)
     &&(VOS_TRUE == stVideoCall.ucStatus))
     {
        g_enVpNvCfgState = stVideoCall.ucVpCfgState;
     }
     else
     {
        g_enVpNvCfgState = MN_CALL_VP_MO_MT_BOTH;
     }

    return;
}
VOS_VOID  MN_CAL_ReadCcbsNvim( VOS_VOID )
{
    NAS_NVIM_CCBS_SUPPORT_FLG_STRU                          stCcbsSupportFlg;
    MN_CALL_CUSTOM_CFG_INFO_STRU                           *pstCcbsSupportFlg = VOS_NULL_PTR;


    stCcbsSupportFlg.ucCcbsSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;

    pstCcbsSupportFlg = MN_CALL_GetCustomCfgInfo();

    if (NV_OK != NV_Read(en_NV_Item_NVIM_CCBS_SUPPORT_FLG,
                         &stCcbsSupportFlg, sizeof(NAS_NVIM_CCBS_SUPPORT_FLG_STRU)))
    {
        MN_WARN_LOG("MN_CAL_ReadCcbsNvim():WARNING: en_NV_Item_NVIM_CCBS_SUPPORT_FLG Error");

        return;
    }


    if ((MN_CALL_NV_ITEM_ACTIVE != stCcbsSupportFlg.ucCcbsSupportFlg)
     && (MN_CALL_NV_ITEM_DEACTIVE != stCcbsSupportFlg.ucCcbsSupportFlg))
    {
        pstCcbsSupportFlg->ucCcbsSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;
        MN_WARN_LOG("MN_CAL_ReadCcbsNvim():WARNING: NV parameter Error");
        return;
    }

    pstCcbsSupportFlg->ucCcbsSupportFlg = stCcbsSupportFlg.ucCcbsSupportFlg;

    return;
}




VOS_VOID  MN_CAL_ReadCallDeflectionNvim( VOS_VOID )
{
    MN_CALL_NVIM_CALL_DEFLECTION_SUPPORT_FLG_STRU           stCallDeflectionSupportFlg;
    MN_CALL_CUSTOM_CFG_INFO_STRU                           *pstCallDeflectionSupportFlg = VOS_NULL_PTR;


    stCallDeflectionSupportFlg.ucCallDeflectionSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;

    pstCallDeflectionSupportFlg = MN_CALL_GetCustomCfgInfo();

    if (NV_OK != NV_Read(en_NV_Item_NVIM_CALL_DEFLECTION_SUPPORT_FLG,
                         &stCallDeflectionSupportFlg, sizeof(MN_CALL_NVIM_CALL_DEFLECTION_SUPPORT_FLG_STRU)))
    {
        MN_WARN_LOG("MN_CAL_ReadCallDeflectionNvim():WARNING: en_NV_Item_NVIM_CALL_DEFLECTION_SUPPORT_FLG Error");

        return;
    }


    if ((MN_CALL_NV_ITEM_ACTIVE != stCallDeflectionSupportFlg.ucCallDeflectionSupportFlg)
     && (MN_CALL_NV_ITEM_DEACTIVE != stCallDeflectionSupportFlg.ucCallDeflectionSupportFlg))
    {
        pstCallDeflectionSupportFlg->ucCallDeflectionSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;
        MN_WARN_LOG("MN_CAL_ReadCallDeflectionNvim():WARNING: NV parameter Error");
        return;
    }

    pstCallDeflectionSupportFlg->ucCallDeflectionSupportFlg = stCallDeflectionSupportFlg.ucCallDeflectionSupportFlg;

    return;
}


VOS_VOID  MN_CAL_ReadAlsSupportNvim( VOS_VOID )
{
    MN_CALL_NVIM_ALS_SUPPORT_FLG_STRU                       stAlsSupportFlg;
    MN_CALL_CUSTOM_CFG_INFO_STRU                           *pstCustomCfg = VOS_NULL_PTR;
    VOS_UINT32                                              ulLength;

    ulLength                        = 0;

    stAlsSupportFlg.ucAlsSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;

    pstCustomCfg = MN_CALL_GetCustomCfgInfo();
    
    NV_GetLength(en_NV_Item_NVIM_ALS_SUPPORT_FLG, &ulLength);	
    if (ulLength > sizeof(MN_CALL_NVIM_ALS_SUPPORT_FLG_STRU))
    {
        return;
    }
    
    if (NV_OK != NV_Read(en_NV_Item_NVIM_ALS_SUPPORT_FLG,
                         &stAlsSupportFlg, ulLength))
    {
        MN_WARN_LOG("MN_CAL_ReadAlsSupportNvim():WARNING: en_NV_Item_NVIM_ALS_SUPPORT_FLG Error");

        return;
    }


    if ((MN_CALL_NV_ITEM_ACTIVE != stAlsSupportFlg.ucAlsSupportFlg)
     && (MN_CALL_NV_ITEM_DEACTIVE != stAlsSupportFlg.ucAlsSupportFlg))
    {
        pstCustomCfg->ucAlsSupportFlg = MN_CALL_NV_ITEM_DEACTIVE;
        MN_WARN_LOG("MN_CAL_ReadAlsSupportNvim():WARNING: NV parameter Error");
        return;
    }

    pstCustomCfg->ucAlsSupportFlg = stAlsSupportFlg.ucAlsSupportFlg;

    return;
}


VOS_VOID  MN_CALL_ReadReportEccNumSupportNvim( VOS_VOID )
{
    MN_CALL_NVIM_REPORT_ECC_NUM_SUPPORT_FLG_STRU            stCustomSetEcc;
    MN_CALL_CUSTOM_CFG_INFO_STRU                           *pstCustomCfg = VOS_NULL_PTR;
    VOS_UINT32                                              ulLength;


    ulLength = 0;
    PS_MEM_SET(&stCustomSetEcc, 0x00, sizeof(stCustomSetEcc));

    pstCustomCfg = MN_CALL_GetCustomCfgInfo();

    /* 先获取NV的长度 */
    NV_GetLength(en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG, &ulLength);

    if (ulLength > sizeof(MN_CALL_NVIM_REPORT_ECC_NUM_SUPPORT_FLG_STRU))
    {
        MN_WARN_LOG("MN_CALL_ReadReportEccNumSupportNvim():WARNING: en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG length Error");
        pstCustomCfg->ucReportEccNumFlg = VOS_FALSE;

        return;
    }

    NV_GetLength(en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG, &ulLength);	
    if (ulLength > sizeof(MN_CALL_NVIM_REPORT_ECC_NUM_SUPPORT_FLG_STRU))
    {
        return;
    }

    /* 如果NV读取失败，则设置为不激活 */
    if (NV_OK != NV_Read(en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG,
                         &stCustomSetEcc, ulLength))
    {
        MN_WARN_LOG("MN_CAL_ReadReportEccNumSupportNvim():WARNING: en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG Error");

        pstCustomCfg->ucReportEccNumFlg = VOS_FALSE;

        return;
    }

    /* NV激活则设置主动上报激活标志为VOS_TRUE */
    if (MN_CALL_NV_ITEM_ACTIVE == stCustomSetEcc.ucReportEccNumFlg)
    {
        pstCustomCfg->ucReportEccNumFlg = VOS_TRUE;
    }
    else
    {
        pstCustomCfg->ucReportEccNumFlg = VOS_FALSE;
    }

    return;
}
VOS_VOID  MN_CALL_ReadCallRedialCfgNvim( VOS_VOID )
{
    MN_CALL_REDIAL_CFG_STRU             stCallRedialCfg;
    MN_CALL_CUSTOM_CFG_INFO_STRU       *pstCustomCfg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;


    ulLength = 0;

    pstCustomCfg = MN_CALL_GetCustomCfgInfo();

    /* GCF测试,短信重发功能需要关闭 */
    if (VOS_TRUE == NAS_USIMMAPI_IsTestCard())
    {
        pstCustomCfg->stCallRedialCfg.ucIsCallRedialSupportFlg = VOS_FALSE;
        pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod       = 0;
        pstCustomCfg->stCallRedialCfg.ulCallRedialInterval     = 0;

        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD,
                                    pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod);

        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDAIL_INTERVAL,
                                    pstCustomCfg->stCallRedialCfg.ulCallRedialInterval);

        return;
    }

    /* 先获取NV的长度 */
    NV_GetLength(en_NV_Item_CS_Call_Redial_CFG, &ulLength);

    PS_MEM_SET(&stCallRedialCfg, 0x0, sizeof(MN_CALL_REDIAL_CFG_STRU));

    if (ulLength > sizeof(MN_CALL_REDIAL_CFG_STRU))
    {
        MN_WARN_LOG("MN_CALL_ReadCallRedialCfgNvim():WARNING: en_NV_Item_CS_Call_Redial_CFG length Error");
        pstCustomCfg->stCallRedialCfg.ucIsCallRedialSupportFlg = VOS_FALSE;
        pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod       = 0;
        pstCustomCfg->stCallRedialCfg.ulCallRedialInterval     = 0;
        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod);
        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDAIL_INTERVAL,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialInterval);
        return;
    }

    /* 如果NV读取失败，则设置为不激活 */
    if (NV_OK != NV_Read(en_NV_Item_CS_Call_Redial_CFG,
                         &stCallRedialCfg, sizeof(MN_CALL_REDIAL_CFG_STRU)))
    {
        MN_WARN_LOG("MN_CALL_ReadCallRedialCfgNvim():WARNING: en_NV_Item_CS_Call_Redial_CFG Error");
        pstCustomCfg->stCallRedialCfg.ucIsCallRedialSupportFlg = VOS_FALSE;
        pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod       = 0;
        pstCustomCfg->stCallRedialCfg.ulCallRedialInterval     = 0;
        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod);
        MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDAIL_INTERVAL,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialInterval);
        return;
    }

    if (MN_CALL_NV_ITEM_ACTIVE == stCallRedialCfg.ucIsCallRedialSupportFlg)
    {
        pstCustomCfg->stCallRedialCfg.ucIsCallRedialSupportFlg = VOS_TRUE;

        /* nv中存的时长为秒，需要转换为毫秒 */
        pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod
            = MN_CALL_ONE_THOUSAND_MILLISECOND * stCallRedialCfg.ulCallRedialPeriod;
        pstCustomCfg->stCallRedialCfg.ulCallRedialInterval
            = MN_CALL_ONE_THOUSAND_MILLISECOND * stCallRedialCfg.ulCallRedialInterval;
    }
    else
    {
        pstCustomCfg->stCallRedialCfg.ucIsCallRedialSupportFlg = VOS_FALSE;
        pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod       = 0;
        pstCustomCfg->stCallRedialCfg.ulCallRedialInterval     = 0;
    }

    MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDIAL_PERIOD,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialPeriod);
    MN_CALL_UpdateTimerPeriod(MN_CALL_TID_WAIT_CALL_REDAIL_INTERVAL,
                              pstCustomCfg->stCallRedialCfg.ulCallRedialInterval);

    return;
}




VOS_VOID TAF_CALL_ReadCallNotSupportedCause(VOS_VOID)
{
    MN_CALL_CUSTOM_CFG_INFO_STRU        *pstCustomCfg = VOS_NULL_PTR;
    TAF_CALL_NVIM_CALL_NOT_SUPPORTED_CAUSE_STRU             stVoiceCallNotSupportedCause;

    if (NV_OK != NV_Read(en_NV_Item_CALL_CallNotSupportedCause,
                         &stVoiceCallNotSupportedCause,
                         sizeof(stVoiceCallNotSupportedCause)))
    {
        stVoiceCallNotSupportedCause.ucVoiceCallNotSupportedCause = MN_CALL_USER_BUSY;
        stVoiceCallNotSupportedCause.ucVideoCallNotSupportedCause = MN_CALL_INCOMPATIBLE_DESTINATION;
    }

    pstCustomCfg = MN_CALL_GetCustomCfgInfo();
    
    pstCustomCfg->ucVoiceCallNotSupportedCause = 
                      stVoiceCallNotSupportedCause.ucVoiceCallNotSupportedCause;
    pstCustomCfg->ucVideoCallNotSupportedCause = 
                      stVoiceCallNotSupportedCause.ucVideoCallNotSupportedCause;

    return;
}


VOS_VOID MN_CALL_ReadNvimInfo(VOS_VOID)
{
    /* en_NV_Item_CustomizeService */
    MN_CALL_ReadCustomizeServiceNvim();

    /* en_NV_Item_MultiSimCallConf */
    MN_CAL_ReadMultiSimCallConfNvim();

    /* en_NV_Item_VIDEO_CALL */
    MN_CAL_ReadVideoCallNvim();

    /* en_NV_Item_NVIM_CCBS_SUPPORT_FLG */
    MN_CAL_ReadCcbsNvim();

    

    /* en_NV_Item_NVIM_CALL_DEFLECTION_SUPPORT_FLG */
    MN_CAL_ReadCallDeflectionNvim();

    /* en_NV_Item_NVIM_ALS_SUPPORT_FLG */
    MN_CAL_ReadAlsSupportNvim();

    /* en_NV_Item_REPORT_ECC_NUM_SUPPORT_FLAG */
    MN_CALL_ReadReportEccNumSupportNvim();

    MN_CALL_ReadCallRedialCfgNvim();

    TAF_CALL_ReadCallNotSupportedCause();

    return;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



