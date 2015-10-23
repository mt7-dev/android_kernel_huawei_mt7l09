


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MnCallCtx.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e(767)*/
#define    THIS_FILE_ID        PS_FILE_ID_MN_CALL_CTX_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* MMC CTX,用于保存MMC状态机,描述表 */
MN_CALL_CONTEXT_STRU                    g_stMnCallCtx;


/*****************************************************************************
   3 函数实现
*****************************************************************************/


MN_CALL_CONTEXT_STRU*  MN_CALL_GetCtx( VOS_VOID )
{
    return &(g_stMnCallCtx);
}


MN_CALL_CUSTOM_CFG_INFO_STRU* MN_CALL_GetCustomCfgInfo( VOS_VOID )
{
    return &(MN_CALL_GetCtx()->stMsCfgInfo.stCustomCfg);
}


VOS_UINT32 MN_CALL_GetCallRedialSupportFlg(VOS_VOID)
{
    return (MN_CALL_GetCtx()->stMsCfgInfo.stCustomCfg.stCallRedialCfg.ucIsCallRedialSupportFlg);
}


MN_CALL_MSG_BUFF_STRU*  MN_CALL_GetBufferedMsg( VOS_VOID )
{
    return &(MN_CALL_GetCtx()->stMnCallBufferedMsg);
}


VOS_VOID MN_CALL_ClearBufferedMsg(VOS_VOID)
{
    MN_CALL_MSG_BUFF_STRU              *pstMsgBuff = VOS_NULL_PTR;

    pstMsgBuff = MN_CALL_GetBufferedMsg();

    PS_MEM_SET(pstMsgBuff, 0x0, sizeof(MN_CALL_MSG_BUFF_STRU));
    return;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID TAF_CALL_SetSrvccState(MN_CALL_SRVCC_STATE_ENUM_UINT8 enSrvccState)
{
    MN_CALL_GetCtx()->enSrvccState = enSrvccState;
}


MN_CALL_SRVCC_STATE_ENUM_UINT8 TAF_CALL_GetSrvccState(VOS_VOID)
{
    return (MN_CALL_GetCtx()->enSrvccState);
}
#endif


VOS_VOID MN_CALL_InitCtx(VOS_VOID)
{
    PS_MEM_SET(MN_CALL_GetCtx(), 0x0, sizeof(g_stMnCallCtx));

    #if (FEATURE_ON == FEATURE_IMS)    
    TAF_CALL_SetSrvccState(MN_CALL_SRVCC_STATE_BUTT);
    #endif
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



