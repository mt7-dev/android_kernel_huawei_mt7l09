


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasCcCtx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e(767)*/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_CC_CTX_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* CC CTX,用于保存CC上下文 */
NAS_CC_CONTEXT_STRU                    g_stNasCcCtx;


/*****************************************************************************
   3 函数实现
*****************************************************************************/


NAS_CC_CONTEXT_STRU*  NAS_CC_GetCcCtx( VOS_VOID )
{
    return &(g_stNasCcCtx);
}


NAS_CC_CUSTOM_CFG_INFO_STRU* NAS_CC_GetCustomCfgInfo( VOS_VOID )
{
    return &(NAS_CC_GetCcCtx()->stMsCfgInfo.stCustomCfg);
}


NAS_CC_SS_SWITCH_STATE_ENUM_UINT8 NAS_CC_GetSsSwitchHoldState(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldState;
}


NAS_CC_SS_SWITCH_STATE_ENUM_UINT8 NAS_CC_GetSsSwitchRetrieveState(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveState;
}


NAS_CC_ENTITY_ID_T NAS_CC_GetSsSwitchHoldEntityID(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.ulHoldEntityID;
}


NAS_CC_ENTITY_ID_T NAS_CC_GetSsSwitchRetrieveEntityID(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.ulRetrieveEntityID;
}


NAS_CC_CAUSE_VALUE_ENUM_U8 NAS_CC_GetSsSwitchHoldCause(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldCause;
}


NAS_CC_CAUSE_VALUE_ENUM_U8 NAS_CC_GetSsSwitchRetrieveCause(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveCause;
}


VOS_VOID NAS_CC_SetSsSwitchHoldInfo(
    NAS_CC_ENTITY_ID_T                  ulEntityID,
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enState,
    NAS_CC_CAUSE_VALUE_ENUM_U8          enCause
)
{
    g_stNasCcCtx.stSsControl.stSwitchInfo.ulHoldEntityID    = ulEntityID;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldState       = enState;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldCause       = enCause;

    return;
}


VOS_VOID NAS_CC_SetSsSwitchRetrieveInfo(
    NAS_CC_ENTITY_ID_T                  ulEntityID,
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enState,
    NAS_CC_CAUSE_VALUE_ENUM_U8          enCause
)
{
    g_stNasCcCtx.stSsControl.stSwitchInfo.ulRetrieveEntityID  = ulEntityID;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveState     = enState;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveCause     = enCause;

    return;
}


VOS_VOID NAS_CC_SetSsSwitchHoldEntityID(NAS_CC_ENTITY_ID_T ulEntityID)
{
    g_stNasCcCtx.stSsControl.stSwitchInfo.ulHoldEntityID    = ulEntityID;

    return;
}


VOS_VOID NAS_CC_SetSsSwitchRetrieveEntityID(NAS_CC_ENTITY_ID_T ulEntityID)
{
    g_stNasCcCtx.stSsControl.stSwitchInfo.ulRetrieveEntityID  = ulEntityID;

    return;
}


VOS_UINT8 NAS_CC_GetSsSwitchOpFacility(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpFacility;
}


VOS_VOID NAS_CC_SetSsSwitchOpFacility(
    VOS_UINT32                          ulOpFlg,
    NAS_CC_MSG_FACILITY_MT_STRU        *pstFacilityMsg
)
{
    if (VOS_FALSE == ulOpFlg)
    {
        PS_MEM_SET(&(g_stNasCcCtx.stSsControl.stSwitchInfo.stFacility),
                   0,
                   sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

        g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpFacility = VOS_FALSE;
    }
    else
    {
        PS_MEM_CPY(&(g_stNasCcCtx.stSsControl.stSwitchInfo.stFacility),
                   pstFacilityMsg,
                   sizeof(g_stNasCcCtx.stSsControl.stSwitchInfo.stFacility));

        g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpFacility = VOS_TRUE;
    }

    return;
}


NAS_CC_MSG_FACILITY_MT_STRU* NAS_CC_GetSsSwitchFacility(VOS_VOID)
{
    return &(g_stNasCcCtx.stSsControl.stSwitchInfo.stFacility);
}


VOS_VOID NAS_CC_SetSsSwitchInvokeId(
    VOS_UINT8                           ucFlg,
    VOS_UINT8                           ucInvokeId
)
{
    if (VOS_FALSE == ucFlg)
    {
        g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpInvokeId  = VOS_FALSE;
        g_stNasCcCtx.stSsControl.stSwitchInfo.ucInvokeId    = NAS_CC_INVALID_INVOKE_ID;
    }
    else
    {
        g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpInvokeId  = VOS_TRUE;
        g_stNasCcCtx.stSsControl.stSwitchInfo.ucInvokeId    = ucInvokeId;
    }

    return;
}


VOS_UINT8 NAS_CC_GetSsSwitchOpInvokeId(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpInvokeId;
}


VOS_UINT8 NAS_CC_GetSsSwitchInvokeId(VOS_VOID)
{
    return g_stNasCcCtx.stSsControl.stSwitchInfo.ucInvokeId;
}


VOS_VOID NAS_CC_InitSsSwitchInfo(VOS_VOID)
{
    PS_MEM_SET(&g_stNasCcCtx.stSsControl.stSwitchInfo, 0, sizeof(NAS_CC_SS_SWITCH_INFO_STRU));

    g_stNasCcCtx.stSsControl.stSwitchInfo.ulHoldEntityID        = NAS_CC_INVALID_ENTITY_ID;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldState           = NAS_CC_SS_SWITCH_IDLE;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enHoldCause           = NAS_CC_CAUSE_NULL;
    g_stNasCcCtx.stSsControl.stSwitchInfo.ulRetrieveEntityID    = NAS_CC_INVALID_ENTITY_ID;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveState       = NAS_CC_SS_SWITCH_IDLE;
    g_stNasCcCtx.stSsControl.stSwitchInfo.enRetrieveCause       = 0;
    g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpFacility          = VOS_FALSE;
    g_stNasCcCtx.stSsControl.stSwitchInfo.ucOpInvokeId          = VOS_FALSE;
    g_stNasCcCtx.stSsControl.stSwitchInfo.ucInvokeId            = NAS_CC_INVALID_INVOKE_ID;

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



