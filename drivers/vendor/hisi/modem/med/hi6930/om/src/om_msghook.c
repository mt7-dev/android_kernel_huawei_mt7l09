/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_msghook.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年6月16日
  最近修改   :
  功能描述   :
  函数列表   :
              OM_MSGHOOK_Init
              OM_MSGHOOK_MsgCfgMsgHookReq
              OM_MSGHOOK_VosMsgHook
  修改历史   :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_OM_MSGHOOK_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 可维可测模块层间消息钩取功能全局变量 */
OM_MSGHOOK_STRU                         g_stOmMsghook;

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : OM_MSGHOOK_Init
 功能描述  : 可维可测消息钩取功能初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月31日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_MSGHOOK_Init(VOS_VOID)
{
    /* 默认关闭消息钩取 */
   OM_MSGHOOK_SetMsgHookEnable(OM_SWITCH_OFF);
}
VOS_UINT32 OM_MSGHOOK_DefaultCfg(CODEC_MSG_HOOK_CONFIG_STRU *pstMsgHookCfg)
{
    /* 判断NV项设置的值的合法性 */
    if (pstMsgHookCfg->uhwEnable >= OM_SWITCH_BUTT)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 若参数正常，则访问全局变量，设置层间消息钩取功能 */
    OM_MSGHOOK_SetMsgHookEnable(pstMsgHookCfg->uhwEnable);

    return UCOM_RET_SUCC;
}


VOS_VOID OM_MSGHOOK_SetMsgHookEnable(VOS_UINT16 uhwEnable)
{
    g_stOmMsghook.uhwVosMsgHookEnable = uhwEnable;

    /* 注册系统消息钩子函数 */
    if (OM_SWITCH_ON == uhwEnable)
    {
        VOS_RegisterMsgGetHook(OM_MSGHOOK_VosMsgHook);
    }
    else
    {
        VOS_RegisterMsgGetHook(VOS_NULL);
    }
}

/*****************************************************************************
 函 数 名  : OM_MSGHOOK_MsgCfgMsgHookReq
 功能描述  : 处理ID_OM_CODEC_MSG_HOOK_REQ消息，配置是否钩取HiFi上交互的VOS消息
 输入参数  : VOS_VOID *pstOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 OM_MSGHOOK_MsgCfgMsgHookReq(VOS_VOID *pvOsaMsg)
{
    OM_CODEC_MSG_HOOK_REQ_STRU           *pstMsgHook;
    CODEC_OM_MSG_HOOK_CNF_STRU            stMsgCnf;
    VOS_UINT32                          uwRet   = UCOM_RET_SUCC;

    UCOM_MemSet(&stMsgCnf, 0, sizeof(stMsgCnf));

    /* 按照OM_CODEC_MSG_HOOK_REQ_STRU结构进行消息解析 */
    pstMsgHook = (OM_CODEC_MSG_HOOK_REQ_STRU*)pvOsaMsg;

    /* 参数检查，若消息内容中usHookEnable范围超过OM_SWITCH_ENUM枚举范围 */
    if (pstMsgHook->enHookEnable >= OM_SWITCH_BUTT)
    {
        uwRet = UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 若参数正常，则访问全局变量，更新层间消息钩取功能 */
        OM_MSGHOOK_SetMsgHookEnable(pstMsgHook->enHookEnable);
        uwRet = UCOM_RET_SUCC;
    }

    /* 填充回复消息并通过透明消息返回 */
    stMsgCnf.uhwMsgId   = ID_CODEC_OM_MSG_HOOK_CNF;
    stMsgCnf.enReturn   = (VOS_UINT16)uwRet;

    OM_COMM_SendTranMsg(&stMsgCnf, sizeof(stMsgCnf));

    return uwRet;
}

/*****************************************************************************
 函 数 名  : OM_MSGHOOK_VosMsgHook
 功能描述  : 消息钩取钩子函数，将VOS消息内容通过OM上报上去
 输入参数  : VOS_VOID *pvOsaMsg - 待上报的消息
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月18日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID OM_MSGHOOK_VosMsgHook(VOS_VOID *pvOsaMsg)
{
    MsgBlock    *pstMsg = (MsgBlock *)pvOsaMsg;

    /* 若消息钩取打开 */
    if (OM_SWITCH_ON == OM_MSGHOOK_GetMsgHookEnable())
    {
        OM_COMM_SendTrace((VOS_UCHAR *)pvOsaMsg, pstMsg->uwLength + VOS_MSG_HEAD_LENGTH);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

