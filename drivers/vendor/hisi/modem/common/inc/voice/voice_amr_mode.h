/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_amr_mode.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2010年4月3日
  最近修改   :
  功能描述   : VOICE_amr_mode.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2010年4月3日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/
#ifndef __VOICE_AMR_MODE_H__
#define __VOICE_AMR_MODE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "WttfCodecInterface.h"
#include "CodecInterface.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 全局变量宏封装 */
#define VOICE_AMR_MODE_CTRL_GetObjPtr()        (&g_stVoiceAmrModeCtrlObj)
#define VOICE_AMR_MODE_CTRL_SetAcsSet(enAcs)   (g_stVoiceAmrModeCtrlObj.enAcsSet = enAcs)
#define VOICE_AMR_MODE_CTRL_GetAcsSet()        (g_stVoiceAmrModeCtrlObj.enAcsSet)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum VOICE_AMR_DTX_ENUM
{
    VOICE_AMR_DTX_DISABLE                 = 0,                                    /* DTX OFF */
    VOICE_AMR_DTX_ENABLE                  = 1,                                    /* DTX ON  */
    VOICE_AMR_DTX_BUTT
};
typedef VOS_UINT16 VOICE_AMR_DTX_ENUM_UINT16;


enum VOICE_AMR_ACS_SET_ENUM
{
    VOICE_AMR_ACS_SET_NO                  = 0,                                    /* ACS未设置 */
    VOICE_AMR_ACS_SET_YES                 = 1,                                    /* ACS已设置 */
    VOICE_AMR_ACS_SET_BUTT
};
typedef VOS_UINT16 VOICE_AMR_ACS_SET_ENUM_UINT16;
/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
typedef struct
{
    WTTFVOICE_AMR_TYPE_ENUM_UINT16                enAmrType;                    /* AMR类型: UMTS-AMR|UMTS-AMR2|UMTS-AMRWB */
    VOS_UINT16                                    enCurrentMode;                /* 当前AMR速率模式 */
    VOS_UINT16                                    enTargetMode;                 /* 目标AMR速率模式 */
    VOS_UINT16                                    uhwNumInAcs;                  /* ACS中速率模式数目 */
    VOS_UINT16                                    aenAcs[WTTFVOICE_AMR_CODEC_MODE_NUM];
                                                                                /* AMR ACS */
    VOICE_AMR_DTX_ENUM_UINT16                     enDtxEnable;                  /* DTX 标志 */
    VOS_UINT16                                    uhwFrmCnt;                    /* 语音帧计数 */
    VOICE_AMR_ACS_SET_ENUM_UINT16                 enAcsSet;                     /* 标志ACS是否已设置 */
    VOS_UINT16                                    uhwReserve;
} VOICE_AMR_MODE_CTRL_OBJ;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_AMR_MODE_CTRL_OBJ                   g_stVoiceAmrModeCtrlObj;                  /* AMR模式控制数据实体, 详细参见VOICE_AMR_MODE_CTRL_OBJ定义 */


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 VOICE_MsgUmtsMacModeSetIndIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgUmtsMacModeChangeIndR(VOS_VOID *pstOsaMsg );
extern VOICE_AMR_DTX_ENUM_UINT16 VOICE_AmrModeCtrlGetDtx(VOS_VOID);
extern VOS_UINT16 VOICE_AmrModeCtrlChangeMode(VOS_VOID);
extern VOS_VOID VOICE_AmrModeCtrlInit(VOS_VOID);
extern VOS_VOID VOICE_UmtsAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType,VOS_UINT32 uwReceiverPid);
extern VOS_VOID VOICE_TDAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType);
extern VOS_UINT32 VOICE_MsgTDmacModeSetIndIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgTDMacModeChangeIndR(VOS_VOID *pstOsaMsg );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_vs_amr_mode_ctrl.h */
