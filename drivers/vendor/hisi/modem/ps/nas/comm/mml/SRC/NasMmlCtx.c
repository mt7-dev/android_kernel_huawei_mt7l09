/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasMmlCtx.c
  版 本 号   : 初稿
  作    者   : zhoujun 40661
  生成日期   : 2011年6月29日
  最近修改   : 2011年6月29日
  功能描述   : MML_CTX的全局变量管理以及对外提供API接口
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月29日
    作    者   : zhoujun 40661
    修改内容   : 创建文件

****************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "PsLib.h"
#include "pslog.h"
#include "PsCommonDef.h"
#include "NasMmlCtx.h"
#include "NasComm.h"
#include "NasMmlLib.h"
#include "NasMmcSndLmm.h"

#include "NasErrorLog.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_NASMML_CTX_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/
/* 保存三个模块共用的CTX */
NAS_MML_CTX_STRU                        g_stNasMmlCtx;

extern NAS_MNTN_MM_INFO_STRU            g_stNasMntnErrorLogMm;

/*****************************************************************************
  6 函数定义
*****************************************************************************/

/*****************************************************************************
 函 数 名  : NAS_MML_GetMmlCtx
 功能描述  : 获取当前MML的CTX
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 当前MML的CTX
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月9日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_CTX_STRU*  NAS_MML_GetMmlCtx( VOS_VOID )
{
    return &(g_stNasMmlCtx);
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimStatusInfoCtx
 功能描述  : 初始化MML_CTX中SIM 状态相关信息
 输入参数  : 无
 输出参数  : pstSimStatusInfo:SIM卡状态相关信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimStatusInfoCtx(
    NAS_MML_SIM_STATUS_STRU             *pstSimStatusInfo
)
{
    pstSimStatusInfo->ucSimPresentStatus  = VOS_FALSE;
    pstSimStatusInfo->enSimType           = NAS_MML_SIM_TYPE_BUTT;
    pstSimStatusInfo->ucSimCsRegStatus    = VOS_FALSE;
    pstSimStatusInfo->ucSimPsRegStatus    = VOS_FALSE;
    pstSimStatusInfo->enCsUpdateStatus    = NAS_MML_LOCATION_UPDATE_STATUS_BUTT;
    pstSimStatusInfo->enPsUpdateStatus    = NAS_MML_ROUTING_UPDATE_STATUS_BUTT;
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimMsIdentityInfoCtx
 功能描述  : 初始化MML_CTX中SIM卡MS相关信息
 输入参数  : 无
 输出参数  : pstMsIdentity:SIM卡MS相关信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimMsIdentityInfoCtx(
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity
)
{
    VOS_UINT32                          i;

    /* IMSI内容 */
    for ( i = 0 ; i < NAS_MML_MAX_IMSI_LEN ; i++ )
    {
        pstMsIdentity->aucImsi[i]           = NAS_MML_IMSI_INVALID;
    }

    /* PTMSI内容 */
    for ( i = 0 ; i < NAS_MML_MAX_PTMSI_LEN ; i++ )
    {
        pstMsIdentity->aucPtmsi[i]          = NAS_MML_PTMSI_INVALID;
    }

    /* PTMSI Signature内容 */
    for ( i = 0 ; i < NAS_MML_MAX_PTMSI_SIGNATURE_LEN ; i++ )
    {
        pstMsIdentity->aucPtmsiSignature[i] = NAS_MML_PTMSI_SIGNATURE_INVALID;
    }

    /* Tmsi内容 */
    for ( i = 0 ; i < NAS_MML_MAX_TMSI_LEN ; i++ )
    {
        pstMsIdentity->aucTmsi[i] = NAS_MML_TMSI_INVALID;
    }

    pstMsIdentity->enUeOperMode     = NAS_MML_SIM_UE_OPER_MODE_BUTT;
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitUeIdPtmisInvalid
 功能描述  : 初始化MML_CTX中PTMSI为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitUeIdPtmisInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* PTMSI内容 */
    for ( i = 0 ; i < NAS_MML_MAX_PTMSI_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsi[i] = NAS_MML_PTMSI_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitUeIdPtmsiSignatureInvalid
 功能描述  : 初始化MML_CTX中PTMSI SIGNATURE为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitUeIdPtmsiSignatureInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* PTMSI SIGNATURE内容 */
    for ( i = 0 ; i < NAS_MML_MAX_PTMSI_SIGNATURE_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsiSignature[i] = NAS_MML_PTMSI_SIGNATURE_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitUeIdTmsiInvalid
 功能描述  : 初始化MML_CTX中TMSI为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitUeIdTmsiInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* PTMSI SIGNATURE内容 */
    for ( i = 0 ; i < NAS_MML_MAX_TMSI_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucTmsi[i] = NAS_MML_TMSI_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimPsSecurityCtx
 功能描述  : 初始化MML_CTX中SIM卡PS域的安全参数相关信息
 输入参数  : 无
 输出参数  : pstPsSecurity:SIM卡PS域的安全参数相关信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimPsSecurityCtx(
    NAS_MML_SIM_PS_SECURITY_INFO_STRU   *pstPsSecurity
)
{
    VOS_UINT32                          i;

    /* CKSN */
    pstPsSecurity->ucCKSN               = NAS_MML_CKSN_INVALID;

    /* GSM KC */
    for ( i = 0 ; i < NAS_MML_GSM_KC_LEN ; i++ )
    {
        pstPsSecurity->aucGsmKc[i]      = NAS_MML_GSMKC_INVALID;
    }

    /* UMTS CK */
    for ( i = 0 ; i < NAS_MML_UMTS_CK_LEN ; i++ )
    {
        pstPsSecurity->aucUmtsCk[i]      = NAS_MML_CK_INVALID;
    }

    /* UMTS IK */
    for ( i = 0 ; i < NAS_MML_UMTS_IK_LEN ; i++ )
    {
        pstPsSecurity->aucUmtsIk[i]      = NAS_MML_IK_INVALID;
    }
}


/*****************************************************************************
 函 数 名  : NAS_MML_InitPsSecurityGsmKcInvalid
 功能描述  : 初始化MML_CTX中PS域的安全参数中的GSM KC为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitPsSecurityGsmKcInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* GSM KC内容无效 */
    for ( i = 0 ; i < NAS_MML_GSM_KC_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucGsmKc[i] = NAS_MML_GSMKC_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitPsSecurityUmtsCkInvalid
 功能描述  : 初始化MML_CTX中PS域的安全参数中的UMTS CK为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitPsSecurityUmtsCkInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* UMTS CK内容无效 */
    for ( i = 0 ; i < NAS_MML_UMTS_CK_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsCk[i] = NAS_MML_CK_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitPsSecurityUmtsIkInvalid
 功能描述  : 初始化MML_CTX中PS域的安全参数中的UMTS IK为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitPsSecurityUmtsIkInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* UMTS CK内容无效 */
    for ( i = 0 ; i < NAS_MML_UMTS_IK_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsIk[i] = NAS_MML_IK_INVALID;
    }

}


/*****************************************************************************
 函 数 名  : NAS_MML_InitCsSecurityGsmKcInvalid
 功能描述  : 初始化MML_CTX中CS域的安全参数中的GSM KC为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitCsSecurityGsmKcInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* GSM KC内容无效 */
    for ( i = 0 ; i < NAS_MML_GSM_KC_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucGsmKc[i] = NAS_MML_GSMKC_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitCsSecurityUmtsCkInvalid
 功能描述  : 初始化MML_CTX中CS域的安全参数中的UMTS CK为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitCsSecurityUmtsCkInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* UMTS CK内容无效 */
    for ( i = 0 ; i < NAS_MML_UMTS_CK_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsCk[i] = NAS_MML_CK_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitCsSecurityUmtsIkInvalid
 功能描述  : 初始化MML_CTX中PS域的安全参数中的UMTS IK为无效值
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitCsSecurityUmtsIkInvalid(VOS_VOID)
{
    VOS_UINT32                          i;

    /* UMTS CK内容无效 */
    for ( i = 0 ; i < NAS_MML_UMTS_IK_LEN ; i++ )
    {
        NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsIk[i] = NAS_MML_IK_INVALID;
    }

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimCsSecurityCtx
 功能描述  : 初始化MML_CTX中SIM卡CS域的安全参数相关信息
 输入参数  : 无
 输出参数  : pstCsSecurity:SIM卡CS域的安全参数相关信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimCsSecurityCtx(
    NAS_MML_SIM_CS_SECURITY_INFO_STRU   *pstCsSecurity
)
{
    VOS_UINT32                          i;

    /* CKSN */
    pstCsSecurity->ucCKSN               = NAS_MML_CKSN_INVALID;

    /* GSM KC */
    for ( i = 0 ; i < NAS_MML_GSM_KC_LEN ; i++ )
    {
        pstCsSecurity->aucGsmKc[i]      = NAS_MML_GSMKC_INVALID;
    }

    /* UMTS CK */
    for ( i = 0 ; i < NAS_MML_UMTS_CK_LEN ; i++ )
    {
        pstCsSecurity->aucUmtsCk[i]      = NAS_MML_CK_INVALID;
    }

    /* UMTS IK */
    for ( i = 0 ; i < NAS_MML_UMTS_IK_LEN ; i++ )
    {
        pstCsSecurity->aucUmtsIk[i]      = NAS_MML_IK_INVALID;
    }
}


VOS_VOID  NAS_MML_InitSimEhPlmnInfo(
    NAS_MML_SIM_EHPLMN_INFO_STRU        *pstEhPlmnInfo
)
{
    VOS_UINT32                          i;

    pstEhPlmnInfo->ucEhPlmnNum          = 0;
    pstEhPlmnInfo->enPLMNSelInd         = NAS_MML_LAST_RPLMN_SEL_IND_RPLMN;
    pstEhPlmnInfo->enEHplmnPresentInd   = NAS_MML_EHPLMN_PRESENT_IND_NO_PREF;

    pstEhPlmnInfo->enHplmnType          = NAS_MML_HPLMN_TYPE_BUTT;

    for ( i = 0 ; i < NAS_MML_MAX_EHPLMN_NUM ; i++ )
    {
        pstEhPlmnInfo->astEhPlmnInfo[i].stPlmnId.ulMcc  = NAS_MML_INVALID_MCC;
        pstEhPlmnInfo->astEhPlmnInfo[i].stPlmnId.ulMnc  = NAS_MML_INVALID_MNC;
#if (FEATURE_ON == FEATURE_LTE)
        pstEhPlmnInfo->astEhPlmnInfo[i].usSimRat        = NAS_MML_SIM_ALL_RAT_SUPPORT_LTE;
#else
        pstEhPlmnInfo->astEhPlmnInfo[i].usSimRat        = NAS_MML_SIM_ALL_RAT;
#endif

        PS_MEM_SET(pstEhPlmnInfo->astEhPlmnInfo[i].aucReserve,
                   0X00,
                   sizeof(pstEhPlmnInfo->astEhPlmnInfo[i].aucReserve));

    }
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimHPlmnWithRatInfo
 功能描述  : 初始化MML_CTX中SIM卡HPLMNWITHRAT信息
 输入参数  : 无
 输出参数  : pstHplmnWithRatInfo:HPLMNWithRat信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimHPlmnWithRatInfo(
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU *pstHplmnWithRatInfo
)
{
    VOS_UINT32                          i;

    pstHplmnWithRatInfo->ucHPlmnWithRatNum  = 0;

    for ( i = 0 ; i < NAS_MML_MAX_HPLMN_WITH_RAT_NUM ; i++ )
    {
        pstHplmnWithRatInfo->astHPlmnWithRatInfo[i].stPlmnId.ulMcc   = NAS_MML_INVALID_MCC;
        pstHplmnWithRatInfo->astHPlmnWithRatInfo[i].stPlmnId.ulMnc   = NAS_MML_INVALID_MNC;
        pstHplmnWithRatInfo->astHPlmnWithRatInfo[i].usSimRat         = NAS_MML_INVALID_SIM_RAT;
    }
}
/*****************************************************************************
 函 数 名  : NAS_MML_InitSimUserPlmnInfo
 功能描述  : 初始化MML_CTX中SIM卡USERPLMN信息
 输入参数  : 无
 输出参数  : pstUserPlmnInfo:USERPLMN信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimUserPlmnInfo(
    NAS_MML_SIM_USERPLMN_INFO_STRU      *pstUserPlmnInfo
)
{
    VOS_UINT32                          i;

    pstUserPlmnInfo->ucUserPlmnNum      = 0;

    for ( i = 0 ; i < NAS_MML_MAX_USERPLMN_NUM ; i++ )
    {
        pstUserPlmnInfo->astUserPlmnInfo[i].stPlmnId.ulMcc   = NAS_MML_INVALID_MCC;
        pstUserPlmnInfo->astUserPlmnInfo[i].stPlmnId.ulMnc   = NAS_MML_INVALID_MNC;
        pstUserPlmnInfo->astUserPlmnInfo[i].usSimRat         = NAS_MML_INVALID_SIM_RAT;
    }
}


VOS_VOID  NAS_MML_InitSimOperPlmnInfo(
    NAS_MML_SIM_OPERPLMN_INFO_STRU      *pstOperPlmnInfo
)
{
    VOS_UINT32                          i;

    pstOperPlmnInfo->usOperPlmnNum = 0;

    for ( i = 0 ; i < NAS_MML_MAX_OPERPLMN_NUM ; i++ )
    {
        pstOperPlmnInfo->astOperPlmnInfo[i].stPlmnId.ulMcc   = NAS_MML_INVALID_MCC;
        pstOperPlmnInfo->astOperPlmnInfo[i].stPlmnId.ulMnc   = NAS_MML_INVALID_MNC;
        pstOperPlmnInfo->astOperPlmnInfo[i].usSimRat         = NAS_MML_INVALID_SIM_RAT;
    }
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimSelPlmnInfo
 功能描述  : 初始化MML_CTX中SIM卡SELPLMN信息
 输入参数  : 无
 输出参数  : pstSelPlmnInfo:SELPLMN信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimSelPlmnInfo(
    NAS_MML_SIM_SELPLMN_INFO_STRU        *pstSelPlmnInfo
)
{
    VOS_UINT32                          i;

    pstSelPlmnInfo->ucSelPlmnNum        = 0;

    for ( i = 0 ; i < NAS_MML_MAX_SELPLMN_NUM ; i++ )
    {
        pstSelPlmnInfo->astPlmnId[i].ulMcc   = NAS_MML_INVALID_MCC;
        pstSelPlmnInfo->astPlmnId[i].ulMnc   = NAS_MML_INVALID_MNC;
    }
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimForbidPlmnInfo
 功能描述  : 初始化MML_CTX中SIM卡FORBIDPLMN信息
 输入参数  : 无
 输出参数  : pstForbidPlmnInfo:FORBIDPLMN信息
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月11日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimForbidPlmnInfo(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU   *pstForbidPlmnInfo
)
{
    VOS_UINT32                          i;

    pstForbidPlmnInfo->ucForbGprsPlmnNum    = 0;
    pstForbidPlmnInfo->ucForbRegLaNum       = 0;
    pstForbidPlmnInfo->ucForbRoamLaNum      = 0;

    for ( i = 0 ; i < NAS_MML_MAX_FORBGPRSPLMN_NUM ; i++ )
    {
        pstForbidPlmnInfo->astForbGprsPlmnList[i].ulMcc = NAS_MML_INVALID_MCC;
        pstForbidPlmnInfo->astForbGprsPlmnList[i].ulMnc = NAS_MML_INVALID_MNC;
    }

    for ( i = 0 ; i < NAS_MML_MAX_FORBLA_FOR_ROAM_NUM ; i++ )
    {
        pstForbidPlmnInfo->astForbRomLaList[i].stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
        pstForbidPlmnInfo->astForbRomLaList[i].stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
        pstForbidPlmnInfo->astForbRomLaList[i].aucLac[0]      = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstForbidPlmnInfo->astForbRomLaList[i].aucLac[1]      = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    for ( i = 0 ; i < NAS_MML_MAX_FORBLA_FOR_REG_NUM ; i++ )
    {
        pstForbidPlmnInfo->astForbRegLaList[i].stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
        pstForbidPlmnInfo->astForbRegLaList[i].stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
        pstForbidPlmnInfo->astForbRegLaList[i].aucLac[0]      = NAS_MML_LAC_LOW_BYTE_INVALID;
        pstForbidPlmnInfo->astForbRegLaList[i].aucLac[1]      = NAS_MML_LAC_HIGH_BYTE_INVALID;
    }

    /* 关机情况下不能清理forbid plmn信息 */
    if ( NAS_MML_INIT_CTX_POWEROFF == enInitType )
    {
        return;
    }

    pstForbidPlmnInfo->ucUsimForbPlmnNum    = 0;
    pstForbidPlmnInfo->ucForbPlmnNum        = 0;
    for ( i = 0 ; i < NAS_MML_MAX_FORBPLMN_NUM ; i++ )
    {
        pstForbidPlmnInfo->astForbPlmnIdList[i].ulMcc   = NAS_MML_INVALID_MCC;
        pstForbidPlmnInfo->astForbPlmnIdList[i].ulMnc   = NAS_MML_INVALID_MNC;
    }


}
VOS_VOID  NAS_MML_InitSimPlmnInfoCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_SIM_PLMN_INFO_STRU         *pstSimStatusInfo
)
{
    /* 关机情况下不需要清空内存中SIM相关变量 */
    if (NAS_MML_INIT_CTX_STARTUP  == enInitType )
    {
        NAS_MML_InitSimEhPlmnInfo(&(pstSimStatusInfo->stEhPlmnInfo));

        NAS_MML_InitSimHPlmnWithRatInfo(&(pstSimStatusInfo->stHplmnWithRatInfo));

        NAS_MML_InitSimUserPlmnInfo(&(pstSimStatusInfo->stUserPlmnInfo));

        NAS_MML_InitSimOperPlmnInfo(&(pstSimStatusInfo->stOperPlmnInfo));

        NAS_MML_InitSimSelPlmnInfo(&(pstSimStatusInfo->stSelPlmnInfo));

        NAS_MML_SetUsimMncLen(NAS_MML_MNC_LENGTH_THREE_BYTES_IN_IMSI);

    }

    NAS_MML_InitSimForbidPlmnInfo(enInitType, &(pstSimStatusInfo->stForbidPlmnInfo));

}

/*****************************************************************************
 函 数 名  : NAS_MML_InitSimInfoCtx
 功能描述  : 初始化MML_CTX中SIM相关信息
 输入参数  : enInitType:初始化类型
             pstSimInfo:SIM卡信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年6月30日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitSimInfoCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_SIM_INFO_STRU              *pstSimInfo
)
{
    NAS_MML_InitSimStatusInfoCtx(&(pstSimInfo->stSimStatus));

    NAS_MML_InitSimMsIdentityInfoCtx(&(pstSimInfo->stMsIdentity));

    NAS_MML_InitSimPsSecurityCtx(&(pstSimInfo->stPsSecurity));

    NAS_MML_InitSimCsSecurityCtx(&(pstSimInfo->stCsSecurity));

    NAS_MML_InitSimPlmnInfoCtx(enInitType, &(pstSimInfo->stSimPlmnInfo));
}


VOS_VOID  NAS_MML_InitMs3GppRel(
    NAS_MML_MS_3GPP_REL_STRU               *pstMs3GppRel
)
{
    pstMs3GppRel->enMsGsmRel    =   NAS_MML_3GPP_REL_R4;
    pstMs3GppRel->enMsWcdmaRel  =   NAS_MML_3GPP_REL_R6;
    pstMs3GppRel->enMsMscRel    =   NAS_MML_MSC_REL_R99;
    pstMs3GppRel->enMsSgsnRel   =   NAS_MML_SGSN_REL_R99;
    pstMs3GppRel->enLteNasRelease = NAS_MML_3GPP_REL_R9;
}


VOS_VOID  NAS_MML_InitPlatformBandCap(
    NAS_MML_PLATFORM_BAND_CAP_STRU     *pstPlatformBandCapability
)
{
    PS_MEM_SET(pstPlatformBandCapability, 0, sizeof(NAS_MML_PLATFORM_BAND_CAP_STRU));
}


VOS_VOID  NAS_MML_InitMsCapability(
    NAS_MML_MS_CAPACILITY_INFO_STRU    *pstMsCapability
)
{
    VOS_UINT8                          i;

    pstMsCapability->ucClassmark1               =   NAS_MML_DEFAULT_CLASSMARK1_VALUE;

    pstMsCapability->aucClassmark2[0]           =   NAS_MML_DEFAULT_CLASSMARK2_FIRST_VALUE;
    pstMsCapability->aucClassmark2[1]           =   NAS_MML_DEFAULT_CLASSMARK2_SECOND_VALUE;
    pstMsCapability->aucClassmark2[2]           =   NAS_MML_DEFAULT_CLASSMARK2_THIRD_VALUE;
    pstMsCapability->aucClassmark2[3]           =   NAS_MML_DEFAULT_CLASSMARK2_FOURTH_VALUE;

    PS_MEM_SET(pstMsCapability->aucFddClassmark3, 0x0, NAS_MML_CLASSMARK3_LEN);   
    PS_MEM_SET(pstMsCapability->aucTddClassmark3, 0x0, NAS_MML_CLASSMARK3_LEN);    

    pstMsCapability->stMsNetworkCapability.ucNetworkCapabilityLen     =   NAS_MML_DEFAULT_NETWORKCAPABILITY_LEN;

    PS_MEM_SET(pstMsCapability->stMsNetworkCapability.aucNetworkCapability, 0, NAS_MML_MAX_NETWORK_CAPABILITY_LEN);

    pstMsCapability->stMsNetworkCapability.aucNetworkCapability[0]    =   NAS_MML_DEFAULT_NETWORKCAPABILITY_FIRST_VALUE;
    pstMsCapability->stMsNetworkCapability.aucNetworkCapability[1]    =   NAS_MML_DEFAULT_NETWORKCAPABILITY_SECOND_VALUE;

    for ( i = 0 ; i < NAS_MML_MAX_IMEISV_LEN ; i++ )
    {
        pstMsCapability->aucImeisv[i]           =   i;

        /* 后面的字段填写默认值 */
        if ( i >= 10 )
        {
            pstMsCapability->aucImeisv[i]       = NAS_MML_DEFAULT_IMEI_VALUE;
        }
    }

#if (FEATURE_ON == FEATURE_LTE)
    PS_MEM_SET(pstMsCapability->stUeNetworkCapbility.aucUeNetCap, 0, NAS_MML_MAX_UE_NETWORK_CAPABILITY_LEN);
    pstMsCapability->stUeNetworkCapbility.ucUeNetCapLen  = NAS_MML_DEFAULT_UE_NETWORK_CAPABILITY_LEN;
    pstMsCapability->stUeNetworkCapbility.aucUeNetCap[0] = NAS_MML_DEFAULT_UE_NET_CAP_FIRST_VALUE;
    pstMsCapability->stUeNetworkCapbility.aucUeNetCap[1] = NAS_MML_DEFAULT_UE_NET_CAP_FIRST_VALUE;
    pstMsCapability->stUeNetworkCapbility.aucUeNetCap[2] = NAS_MML_DEFAULT_UE_NET_CAP_THIRD_VALUE;
    pstMsCapability->stUeNetworkCapbility.aucUeNetCap[3] = NAS_MML_DEFAULT_UE_NET_CAP_THIRD_VALUE;
#endif

    /* 初始化为GSM only */
    for (i = 0; i < NAS_MML_MAX_PLATFORM_RAT_NUM; i++)
    {
        pstMsCapability->stPlatformRatCap.aenRatPrio[i] = NAS_MML_PLATFORM_RAT_TYPE_BUTT;
    }
    pstMsCapability->stPlatformRatCap.ucRatNum      = NAS_MML_MIN_PLATFORM_RAT_NUM;
    pstMsCapability->stPlatformRatCap.aenRatPrio[0] = NAS_MML_PLATFORM_RAT_TYPE_GSM;

    
    NAS_MML_InitPlatformBandCap(&(pstMsCapability->stPlatformBandCap));
}


VOS_VOID  NAS_MML_InitMsSysCfgInfo(
    NAS_MML_MS_SYS_CFG_INFO_STRU        *pstMsSysCfgInfo
)
{
    pstMsSysCfgInfo->enMsMode                   = NAS_MML_MS_MODE_PS_CS;
    pstMsSysCfgInfo->ucPsAutoAttachFlg          = VOS_TRUE;
#if   (FEATURE_ON == FEATURE_LTE)
    pstMsSysCfgInfo->stPrioRatList.ucRatNum     = NAS_MML_MAX_RAT_NUM;
    pstMsSysCfgInfo->stPrioRatList.aucRatPrio[0]= NAS_MML_NET_RAT_TYPE_LTE;
    pstMsSysCfgInfo->stPrioRatList.aucRatPrio[1]= NAS_MML_NET_RAT_TYPE_WCDMA;
    pstMsSysCfgInfo->stPrioRatList.aucRatPrio[2]= NAS_MML_NET_RAT_TYPE_GSM;
#else
    pstMsSysCfgInfo->stPrioRatList.ucRatNum     = NAS_MML_MAX_RAT_NUM;
    pstMsSysCfgInfo->stPrioRatList.aucRatPrio[0]= NAS_MML_NET_RAT_TYPE_WCDMA;
    pstMsSysCfgInfo->stPrioRatList.aucRatPrio[1]= NAS_MML_NET_RAT_TYPE_GSM;
#endif

    pstMsSysCfgInfo->st3Gpp2RatList.ucRatNum      = 0x0;
    pstMsSysCfgInfo->st3Gpp2RatList.aucRatPrio[0] = NAS_MML_3GPP2_RAT_TYPE_BUTT;
    pstMsSysCfgInfo->st3Gpp2RatList.aucRatPrio[1] = NAS_MML_3GPP2_RAT_TYPE_BUTT;

    /* GSM的支持的Band */
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm1800= VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm1900= VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm450 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm480 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm700 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsm850 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsmE900= VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsmP900= VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unGsmBand.stBitBand.BandGsmR900= VOS_FALSE;

    /* WCDMA的支持的Band */
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_III_1800 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_II_1900  = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_IV_1700  = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_IX_J1700 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_I_2100   = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_VIII_900 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_VII_2600 = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_VI_800   = VOS_TRUE;
    pstMsSysCfgInfo->stMsBand.unWcdmaBand.stBitBand.BandWCDMA_V_850    = VOS_TRUE;

    /* LTE的支持的Band */
    pstMsSysCfgInfo->stMsBand.stLteBand.aulLteBand[0] = NAS_MML_LTE_ALL_BAND_SUPPORTED;
    pstMsSysCfgInfo->stMsBand.stLteBand.aulLteBand[1] = NAS_MML_LTE_ALL_BAND_SUPPORTED;

#if   (FEATURE_ON == FEATURE_LTE)
    /* 设置LTE的能力默认为ENABLE状态 */
    NAS_MML_SetLteCapabilityStatus(NAS_MML_LTE_CAPABILITY_STATUS_BUTT);

    /* 设置LTE的CS业务配置能力默认为不激活 */
    NAS_MML_SetLteCsServiceCfg(NAS_MML_LTE_SUPPORT_BUTT);

    /* 初始化UE USAGE为无效值   */
    NAS_MML_SetLteUeUsageSetting(NAS_MML_LTE_UE_USAGE_SETTIN_BUTT);

    /* 初始化VOICE DOMAIN PREFERENCE为无效值 */
     NAS_MML_SetVoiceDomainPreference(NAS_MML_VOICE_DOMAIN_PREFER_BUTT);

    NAS_MML_SetDisableLteRoamFlg(VOS_FALSE);

#endif

    NAS_MML_SetDelayedCsfbLauFlg(VOS_FALSE);
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitRplmnCfgInfo
 功能描述  : 初始化RPLMN的定制特性
 输入参数  : 无
 输出参数  : pstRplmnCfg:RPLMN的定制特性
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月14日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitRplmnCfgInfo(
    NAS_MML_RPLMN_CFG_INFO_STRU         *pstRplmnCfg
)
{
    VOS_UINT32                          i;

    pstRplmnCfg->enTinType              = NAS_MML_TIN_TYPE_INVALID;
    pstRplmnCfg->ucMultiRATRplmnFlg     = VOS_FALSE;
    pstRplmnCfg->ucLastRplmnRatFlg      = VOS_FALSE;
    pstRplmnCfg->enLastRplmnRat         = NAS_MML_NET_RAT_TYPE_BUTT;
    pstRplmnCfg->stGRplmnInNV.ulMcc     = NAS_MML_INVALID_MCC;
    pstRplmnCfg->stGRplmnInNV.ulMnc     = NAS_MML_INVALID_MNC;
    pstRplmnCfg->stWRplmnInNV.ulMcc     = NAS_MML_INVALID_MCC;
    pstRplmnCfg->stWRplmnInNV.ulMnc     = NAS_MML_INVALID_MNC;

    for ( i = 0 ; i < NAS_MML_MAX_IMSI_LEN ; i++ )
    {
        pstRplmnCfg->aucLastImsi[i]    = NAS_MML_IMSI_INVALID;
    }
}



VOS_VOID  NAS_MML_InitHplmnCfgInfo(
    NAS_MML_HPLMN_CFG_INFO_STRU         *pstHplmnCfg
)
{
    pstHplmnCfg->enAutoSearchHplmnFlg   = NAS_MML_AUTO_SRCH_FLG_TYPE_HPLMN_FIRST;
    pstHplmnCfg->ucEhplmnSupportFlg     = VOS_TRUE;
    pstHplmnCfg->ucHplmnSearchPowerOn   = VOS_FALSE;
    pstHplmnCfg->ucManualSearchHplmnFlg = VOS_TRUE;

    pstHplmnCfg->stPrioHplmnActCfg.ucActiveFlg    = VOS_FALSE;
    pstHplmnCfg->stPrioHplmnActCfg.usPrioHplmnAct = NAS_MML_SIM_NO_RAT;

    pstHplmnCfg->ucActingHPlmnSupportFlg = VOS_FALSE;

    NAS_MML_InitUserCfgEHplmnInfo( &(pstHplmnCfg->stUserCfgEhplmnInfo) );
}


VOS_VOID  NAS_MML_InitAisRoamingCfg(
    NAS_MML_AIS_ROAMING_CFG_STRU        *pstAisRoamingCfg
)
{
    pstAisRoamingCfg->ucEnableFlg               = VOS_FALSE;
    pstAisRoamingCfg->enHighPrioRatType         = NAS_MML_NET_RAT_TYPE_BUTT;
    pstAisRoamingCfg->stHighPrioPlmnId.ulMcc    = NAS_MML_INVALID_MCC;
    pstAisRoamingCfg->stHighPrioPlmnId.ulMnc    = NAS_MML_INVALID_MNC;
    pstAisRoamingCfg->stSimHPlmnId.ulMcc        = NAS_MML_INVALID_MCC;
    pstAisRoamingCfg->stSimHPlmnId.ulMnc        = NAS_MML_INVALID_MNC;
}


VOS_VOID  NAS_MML_InitRoamCfgInfo(
    NAS_MML_ROAM_CFG_INFO_STRU          *pstRoamCfg
)
{
    pstRoamCfg->enRoamCapability            = NAS_MML_ROAM_NATION_OFF_INTERNATION_OFF;
    pstRoamCfg->ucRoamFeatureFlg            = VOS_FALSE;
    pstRoamCfg->ucNationalRoamNum           = NAS_MML_DFAULT_NATIONAL_ROAM_PLMN_NUM;
    pstRoamCfg->ucNotRoamNum                = NAS_MML_DFAULT_NOT_ROAM_PLMN_NUM;
    pstRoamCfg->astRoamPlmnIdList[0].ulMcc  = NAS_MML_DFAULT_NATIONAL_ROAM_MCC;
    pstRoamCfg->astRoamPlmnIdList[0].ulMnc  = NAS_MML_DFAULT_NATIONAL_ROAM_MNC;


    NAS_MML_InitAisRoamingCfg(&(pstRoamCfg->stAisRoamingCfg));
}


VOS_VOID  NAS_MML_InitBgSearchCfgInfo(
    NAS_MML_BG_SEARCH_CFG_INFO_STRU     *pstBgSearchCfg
)
{
    pstBgSearchCfg->ucEnhancedHplmnSrchFlg                             = VOS_FALSE;
    pstBgSearchCfg->ucSpecialRoamFlg                                   = VOS_FALSE;
    pstBgSearchCfg->ulFirstStartHplmnTimerLen                          = TI_NAS_MMC_HPLMN_TIMER_FIRST_LEN;                                /* 默认为2分钟 */
    pstBgSearchCfg->stDefaultMaxHplmnTimerPeriod.ucNvimActiveFlg       = VOS_FALSE;
    pstBgSearchCfg->stDefaultMaxHplmnTimerPeriod.ulDefaultMaxHplmnLen  = TI_NAS_MMC_DEFAULT_MAX_SEARCH_HPLMN_LEN;
    pstBgSearchCfg->stNonFirstHplmnTimerPeriod.ucNvimActiveFlg         = VOS_FALSE;
    pstBgSearchCfg->stNonFirstHplmnTimerPeriod.ulNonFirstHplmnTimerLen = TI_NAS_MMC_HPLMN_TIMER_FIRST_LEN;
    pstBgSearchCfg->ucScanCtrlFlg                                      = VOS_FALSE;

    pstBgSearchCfg->stBgSearchRegardlessMcc.ucCustomMccNum             = 0;

    NAS_MML_InitHighPrioRatHplmnTimerCfgInfo(&(pstBgSearchCfg->stHighRatHplmnTimerCfg));
}
VOS_VOID  NAS_MML_InitHplmnRejCauseChangedCounter(
    NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU           *pstChangeCounInfo
)
{
    pstChangeCounInfo->aucReserve[0]                    = 0;
    pstChangeCounInfo->aucReserve[1]                    = 0;
    pstChangeCounInfo->ucGmmHplmnRejCauseChangedCounter = 0;
    pstChangeCounInfo->ucMmHplmnRejCauseChangedCounter  = 0;

    return;
}



VOS_VOID  NAS_MML_InitPlmnLockCfgInfo(
    NAS_MML_PLMN_LOCK_CFG_INFO_STRU     *pstPlmnLockCfg
)
{
    VOS_UINT32                          i;

    /* 初始化锁网黑名单信息 */
    pstPlmnLockCfg->ucBlackPlmnLockNum  = 0;
    for ( i = 0 ; i < NAS_MML_MAX_BLACK_LOCK_PLMN_NUM; i++ )
    {
        pstPlmnLockCfg->astBlackPlmnId[i].ulMcc = NAS_MML_INVALID_MCC;
        pstPlmnLockCfg->astBlackPlmnId[i].ulMnc = NAS_MML_INVALID_MNC;
    }

    /* 初始化锁网白名单信息 */
    pstPlmnLockCfg->ucWhitePlmnLockNum  = 0;
    for ( i = 0 ; i < NAS_MML_MAX_WHITE_LOCK_PLMN_NUM; i++ )
    {
        pstPlmnLockCfg->astWhitePlmnId[i].ulMcc = NAS_MML_INVALID_MCC;
        pstPlmnLockCfg->astWhitePlmnId[i].ulMnc = NAS_MML_INVALID_MNC;
    }

    NAS_MML_InitDisabledRatPlmnCfgInfo(&(pstPlmnLockCfg->stDisabledRatPlmnCfg));
}


VOS_VOID  NAS_MML_InitRatForbiddenList(
    NAS_MML_RAT_FORBIDDEN_LIST_STRU    *pstRatBlackList
)
{
    VOS_UINT32                          i;

    pstRatBlackList->enSwitchFlag       = NAS_MML_RAT_FORBIDDEN_LIST_SWITCH_INACTIVE;
    pstRatBlackList->ucImsiListNum      = 0;
    pstRatBlackList->ucForbidRatNum     = 0;

    for (i = 0; i < NAS_MML_MAX_IMSI_FORBIDDEN_LIST_NUM; i++)
    {
        pstRatBlackList->astImsiList[i].ulMcc = NAS_MML_INVALID_MCC;
        pstRatBlackList->astImsiList[i].ulMnc = NAS_MML_INVALID_MNC;
    }

    for (i = 0; i < NAS_MML_MAX_RAT_FORBIDDEN_LIST_NUM; i++)
    {
        pstRatBlackList->aenForbidRatList[i] = NAS_MML_NET_RAT_TYPE_BUTT;
    }
    
}


VOS_VOID  NAS_MML_InitRatForbiddenStatusCfg(
    NAS_MML_RAT_FORBIDDEN_STATUS_STRU  *pstRatCapaStatus
)
{
   pstRatCapaStatus->ucIsImsiInForbiddenListFlg = VOS_FALSE;
   pstRatCapaStatus->ucGsmCapaStatus            = NAS_MML_RAT_CAPABILITY_STATUS_BUTT;
   pstRatCapaStatus->ucLteCapaStatus            = NAS_MML_RAT_CAPABILITY_STATUS_BUTT;
   pstRatCapaStatus->ucUtranCapaStatus          = NAS_MML_RAT_CAPABILITY_STATUS_BUTT;
}



VOS_VOID  NAS_MML_InitDisabledRatPlmnCfgInfo(
    NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU                *pstDisabledRatPlmnCfg
)
{
    /* 初始化带禁止接入技术的PLMN信息 */
    VOS_UINT32                          i;

    /* 初始化禁止接入技术的PLMN信息 */
    pstDisabledRatPlmnCfg->ulDisabledRatPlmnNum = 0;

    for ( i = 0 ; i < NAS_MML_MAX_DISABLED_RAT_PLMN_NUM; i++ )
    {
        pstDisabledRatPlmnCfg->astDisabledRatPlmnId[i].stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
        pstDisabledRatPlmnCfg->astDisabledRatPlmnId[i].stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
        pstDisabledRatPlmnCfg->astDisabledRatPlmnId[i].enRat = NAS_MML_NET_RAT_TYPE_BUTT;
    }
}



VOS_VOID  NAS_MML_InitAvailTimerCfgInfo(
    NAS_MML_AVAIL_TIMER_CFG_STRU       *pstAvailTimerCfg
)
{
    pstAvailTimerCfg->ulDeepSearchTimeCount  = 0;
    pstAvailTimerCfg->ulDeepSearchTimeLen    = 0;
    pstAvailTimerCfg->ulFirstSearchTimeCount = 0;
    pstAvailTimerCfg->ulFirstSearchTimeLen   = 0;
}



VOS_VOID  NAS_MML_InitUserCfgEHplmnInfo(
    NAS_MML_USER_CFG_EHPLMN_INFO_STRU  *pstUserCfgEhplmnInfo
)
{
    /* 初始化IMSI的PLMN LIST信息个数为0  */
    pstUserCfgEhplmnInfo->ucImsiPlmnListNum = 0;
    PS_MEM_SET( pstUserCfgEhplmnInfo->astImsiPlmnList,
                (VOS_UINT8)0X00,
                sizeof(pstUserCfgEhplmnInfo->astImsiPlmnList));

    /* 初始化EHPLMN信息个数为0  */
    pstUserCfgEhplmnInfo->ucEhplmnListNum = 0;
    PS_MEM_SET( pstUserCfgEhplmnInfo->astEhPlmnList,
                (VOS_UINT8)0X00,
                sizeof(pstUserCfgEhplmnInfo->astEhPlmnList));

    /* 初始化保留字段为0 */
    PS_MEM_SET( pstUserCfgEhplmnInfo->aucResv,
                (VOS_UINT8)0X00,
                sizeof(pstUserCfgEhplmnInfo->aucResv));


    return;

}



VOS_VOID  NAS_MML_InitMiscellaneousCfgInfo(
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU *pstMiscellaneousCfg
)
{
    pstMiscellaneousCfg->ucCsRejSearchSupportFlg            = VOS_FALSE;
    pstMiscellaneousCfg->ucStkSteeringOfRoamingSupportFlg   = VOS_TRUE;
    pstMiscellaneousCfg->ucMaxForbRoamLaFlg                 = VOS_FALSE;
    pstMiscellaneousCfg->ucMaxForbRoamLaNum                 = NAS_MML_DEFAULT_MAX_FORB_ROAM_LA_NUM;
    pstMiscellaneousCfg->ucNvGsmForbidFlg                   = VOS_FALSE;
    pstMiscellaneousCfg->ucRoamBrokerActiveFlag             = VOS_FALSE;
    pstMiscellaneousCfg->ucRoamBrokerRegisterFailCnt        = NAS_MML_MAX_CS_REG_FAIL_CNT;
    pstMiscellaneousCfg->ucSingleDomainFailPlmnSrchFlag     = VOS_FALSE;

    pstMiscellaneousCfg->stSingleDomainFailActionCtx.ucActiveFlag  = VOS_FALSE;
    pstMiscellaneousCfg->stSingleDomainFailActionCtx.ucCount       = 0;

    pstMiscellaneousCfg->ucRegFailNetWorkFailureCustomFlg          = VOS_FALSE;

    pstMiscellaneousCfg->ucCsOnlyDataServiceSupportFlg        = VOS_TRUE;

    NAS_MML_SetLteDisabledRauUseLteInfoFlag(VOS_FALSE);

    NAS_MML_InitSorAdditionalLauCtx();
}


VOS_VOID  NAS_MML_InitCustomCfgInfo(
    NAS_MML_CUSTOM_CFG_INFO_STRU        *pstCustomCfg
)
{
    NAS_MML_InitRplmnCfgInfo(&(pstCustomCfg->stRplmnCfg));

    NAS_MML_InitHplmnCfgInfo(&(pstCustomCfg->stHplmnCfg));

    NAS_MML_InitRoamCfgInfo(&(pstCustomCfg->stRoamCfg));

    NAS_MML_InitBgSearchCfgInfo(&(pstCustomCfg->stBgSearchCfg));

    NAS_MML_InitPlmnLockCfgInfo(&(pstCustomCfg->stPlmnLockCfg));

    NAS_MML_InitAvailTimerCfgInfo(&(pstCustomCfg->stAvailTimerCfg));

    NAS_MML_SetHoWaitSysinfoTimerLen(NAS_MML_HO_WAIT_SYS_INFO_DEFAULT_LEN_VALUE);

    NAS_MML_InitMiscellaneousCfgInfo(&(pstCustomCfg->stMiscellaneousCfg));

#if (FEATURE_ON == FEATURE_LTE)
    /* 默认允许LTE国际漫游 */
    NAS_MML_SetLteRoamAllowedFlg(VOS_TRUE);
#endif

    NAS_MML_SetDailRejectCfg(VOS_FALSE);


    NAS_MML_SetChangeRegRejCauFlg(NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_INACTIVE);
    NAS_MML_SetPreferredRegRejCause_HPLMN_EHPLMN(NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE);
    NAS_MML_SetPreferredRegRejCause_NOT_HPLMN_EHPLMN(NAS_MML_REG_FAIL_CAUSE_ROAM_NOT_ALLOW);

    NAS_MML_InitHplmnRejCauseChangedCounter(&(pstCustomCfg->stChangeRegRejCauInfo.stHplmnRejCauseChangedCounter));
    NAS_MML_SetUserAutoReselActiveFlg(VOS_FALSE);

    NAS_MML_InitHplmnAuthRejCounter(&(pstCustomCfg->stIgnoreAuthRejInfo));


    NAS_MML_SetCsfbEmgCallLaiChgLauFirstFlg(VOS_FALSE);

    NAS_MML_SetPlmnExactlyComparaFlg(VOS_FALSE);

    /* 默认支持cs语音业务 */
    NAS_MML_SetSupportCsServiceFLg(VOS_TRUE);

    NAS_MML_SetHplmnRegisterCtrlFlg(VOS_FALSE);

    /* 默认支持H3G定制特性 */
    NAS_MML_SetH3gCtrlFlg(VOS_TRUE);

#if (FEATURE_ON == FEATURE_LTE)
    /* 默认enable lte定时器时长为0分钟 */
    NAS_MML_SetCsfbEmgCallEnableLteTimerLen(0);
    NAS_MML_SetCsPsMode1EnableLteTimerLen(0);

    /* 默认支持ISR */
    NAS_MML_SetIsrSupportFlg(VOS_TRUE);
    NAS_MML_SetIsRauNeedFollowOnCsfbMtFlg(VOS_FALSE);

    NAS_MML_SetIsRauNeedFollowOnCsfbMoFlg(VOS_FALSE);

    NAS_MML_SetIsDelFddBandSwitchOnFlg(VOS_FALSE);
#endif

    /* 默认不支持SVLTE特性 */
    NAS_MML_SetSvlteSupportFlag(VOS_FALSE);

    NAS_MML_SetDsdsRfShareFlg(VOS_FALSE);

    /* 默认不支持接入禁止发起PLMN搜网特性 */
    NAS_MML_SetSupportAccBarPlmnSearchFlg(VOS_FALSE);

    NAS_MML_InitUserCfgOPlmnInfo(&(pstCustomCfg->stUserCfgOPlmnInfo));

    NAS_MML_InitRatForbiddenList(&(pstCustomCfg->stRatForbiddenListInfo));
    NAS_MML_InitRatForbiddenStatusCfg(&(pstCustomCfg->stRatFirbiddenStatusCfg));
    NAS_MML_SetImsVoiceInterSysLauEnableFlg(VOS_FALSE);
    NAS_MML_SetImsVoiceMMEnableFlg(VOS_FALSE);
	
    NAS_MML_SetLcEnableFlg(VOS_FALSE);

    NAS_MML_SetUltraFlashCsfbSupportFlg(VOS_FALSE);

    NAS_MML_Set3GPP2UplmnNotPrefFlg(VOS_TRUE);

    return;
}


/*****************************************************************************
 函 数 名  : NAS_MML_InitMsCfgCtx
 功能描述  : 初始化MML_CTX中手机配置相关信息
 输入参数  : pstMsCfgInfo:手机配置信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年6月30日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitMsCfgCtx(
    NAS_MML_MS_CFG_INFO_STRU           *pstMsCfgInfo
)
{
    NAS_MML_InitMs3GppRel(&(pstMsCfgInfo->stMs3GppRel));

    NAS_MML_InitMsCapability(&(pstMsCfgInfo->stMsCapability));

    NAS_MML_InitMsSysCfgInfo(&(pstMsCfgInfo->stMsSysCfgInfo));

    NAS_MML_InitCustomCfgInfo(&(pstMsCfgInfo->stCustomCfg));
}

VOS_VOID NAS_MML_InitCampRai(
    NAS_MML_CAMP_PLMN_INFO_STRU         *pstCampPlmnInfo
)
{
    /* 将当前驻留PLMN，LAI,RAC更新为无效值 */
    pstCampPlmnInfo->stLai.stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
    pstCampPlmnInfo->stLai.stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
    pstCampPlmnInfo->stLai.aucLac[0]      = NAS_MML_LAC_LOW_BYTE_INVALID;
    pstCampPlmnInfo->stLai.aucLac[1]      = NAS_MML_LAC_HIGH_BYTE_INVALID;
    pstCampPlmnInfo->ucRac                = NAS_MML_RAC_INVALID;

    return;
}
VOS_VOID NAS_MML_InitRssiValue(
    NAS_MML_CAMP_CELL_INFO_STRU        *pstCampCellInfo
)
{
    VOS_UINT8                           i;
    pstCampCellInfo->ucRssiNum = 1;

    for ( i = 0; i < RRC_NAS_AT_CELL_MAX_NUM; i++ )
    {
        pstCampCellInfo->astRssiInfo[i].sRscpValue      = NAS_MML_UTRA_RSCP_UNVALID;
        pstCampCellInfo->astRssiInfo[i].sRssiValue      = NAS_MML_RSSI_UNVALID;

        pstCampCellInfo->astRssiInfo[i].ucChannalQual   = NAS_MML_UTRA_BLER_INVALID;
        pstCampCellInfo->astRssiInfo[i].ucRssiLevel     = NAS_MML_LOWER_RSSI_LEVEL;
    }

    pstCampCellInfo->ucCellNum    = 0;
    pstCampCellInfo->sUeRfPower   = NAS_MML_UE_RFPOWER_INVALID;
    pstCampCellInfo->usCellDlFreq = NAS_MML_UE_FREQ_INVALID;
    pstCampCellInfo->usCellUlFreq = NAS_MML_UE_FREQ_INVALID;

    for ( i = 0; i < RRC_NAS_AT_CELL_MAX_NUM; i++ )
    {
        pstCampCellInfo->astCellInfo[i].sCellRSCP = NAS_MML_UTRA_RSCP_UNVALID;

        pstCampCellInfo->astCellInfo[i].sCellRssi = NAS_MML_RSSI_UNVALID;

        pstCampCellInfo->astCellInfo[i].ulCellId  = NAS_MML_CELLID_INVALID;
    }

    pstCampCellInfo->usArfcn                      = 0x0;
}
VOS_VOID  NAS_MML_InitRrcNcellInfo(
    NAS_MML_RRC_NCELL_INFO_STRU         *pstRrcNcellInfo
)
{
    PS_MEM_SET(pstRrcNcellInfo,0x00,sizeof(NAS_MML_RRC_NCELL_INFO_STRU));
    pstRrcNcellInfo->ucLteNcellExist     = VOS_FALSE;
    pstRrcNcellInfo->ucUtranNcellExist   = VOS_FALSE;
}



VOS_VOID  NAS_MML_InitCampPlmnInfo(
    NAS_MML_CAMP_PLMN_INFO_STRU         *pstCampPlmnInfo
)
{
    pstCampPlmnInfo->enNetRatType                              = NAS_MML_NET_RAT_TYPE_BUTT;
    pstCampPlmnInfo->enNetworkMode                             = NAS_MML_NET_MODE_BUTT;
    pstCampPlmnInfo->enSysSubMode                              = RRC_NAS_SYS_SUBMODE_BUTT;

    NAS_MML_InitCampRai(pstCampPlmnInfo);

    pstCampPlmnInfo->stOperatorNameInfo.stOperatorPlmnId.ulMcc = NAS_MML_INVALID_MCC;
    pstCampPlmnInfo->stOperatorNameInfo.stOperatorPlmnId.ulMnc = NAS_MML_INVALID_MNC;

    PS_MEM_SET(pstCampPlmnInfo->stOperatorNameInfo.aucOperatorNameLong, 0, NAS_MML_MAX_OPER_LONG_NAME_LEN);
    PS_MEM_SET(pstCampPlmnInfo->stOperatorNameInfo.aucOperatorNameShort, 0, NAS_MML_MAX_OPER_SHORT_NAME_LEN);

    NAS_MML_InitRrcNcellInfo(&(pstCampPlmnInfo->stRrcNcellInfo));

    NAS_MML_InitRssiValue(&(pstCampPlmnInfo->stCampCellInfo));
}



VOS_VOID  NAS_MML_InitCsDomainInfo(
    NAS_MML_CS_DOMAIN_INFO_STRU         *pstCsDomainInfo
)
{
    pstCsDomainInfo->ucAttFlg                               = VOS_TRUE;
    pstCsDomainInfo->ucCsAttachAllow                        = VOS_TRUE;

    pstCsDomainInfo->enCsRegStatus                          = NAS_MML_REG_STATUS_BUTT;

    pstCsDomainInfo->stCsAcRestriction.ucRestrictPagingRsp  = VOS_FALSE;
    pstCsDomainInfo->stCsAcRestriction.ucRestrictRegister   = VOS_FALSE;
    pstCsDomainInfo->stCsAcRestriction.ucRestrictNormalService      = VOS_FALSE;
    pstCsDomainInfo->stCsAcRestriction.ucRestrictEmergencyService   = VOS_FALSE;

    pstCsDomainInfo->ucCsSupportFlg                         = VOS_FALSE;
    pstCsDomainInfo->ulCsDrxLen                             = NAS_MML_CS_INVALID_DRX_LEN;
    pstCsDomainInfo->ulCsRegisterBarToUnBarFlag             = VOS_FALSE;
    pstCsDomainInfo->ulT3212Len                     = NAS_MML_T3212_INFINITE_TIMEOUT_VALUE;
    pstCsDomainInfo->stLastSuccLai.stPlmnId.ulMcc   = NAS_MML_INVALID_MCC;
    pstCsDomainInfo->stLastSuccLai.stPlmnId.ulMnc   = NAS_MML_INVALID_MNC;
    pstCsDomainInfo->stLastSuccLai.aucLac[0]        = NAS_MML_LAC_LOW_BYTE_INVALID;
    pstCsDomainInfo->stLastSuccLai.aucLac[1]        = NAS_MML_LAC_HIGH_BYTE_INVALID;
    pstCsDomainInfo->stLastSuccLai.ucRac = NAS_MML_RAC_INVALID;
}



VOS_VOID  NAS_MML_InitPsDomainInfo(
    NAS_MML_PS_DOMAIN_INFO_STRU         *pstPsDomainInfo
)
{
    pstPsDomainInfo->ucPsAttachAllow                    = VOS_FALSE;

    pstPsDomainInfo->enPsRegStatus                      = NAS_MML_REG_STATUS_BUTT;

    pstPsDomainInfo->stPsAcRestriction.ucRestrictPagingRsp  = VOS_FALSE;
    pstPsDomainInfo->stPsAcRestriction.ucRestrictRegister   = VOS_FALSE;
    pstPsDomainInfo->stPsAcRestriction.ucRestrictNormalService      = VOS_FALSE;
    pstPsDomainInfo->stPsAcRestriction.ucRestrictEmergencyService   = VOS_FALSE;
    pstPsDomainInfo->ucPsSupportFlg                     = VOS_FALSE;
    pstPsDomainInfo->ulPsRegisterBarToUnBarFlag         = VOS_FALSE;
    pstPsDomainInfo->stLastSuccRai.stLai.stPlmnId.ulMcc = NAS_MML_INVALID_MCC;
    pstPsDomainInfo->stLastSuccRai.stLai.stPlmnId.ulMnc = NAS_MML_INVALID_MNC;
    pstPsDomainInfo->stLastSuccRai.stLai.aucLac[0]      = NAS_MML_LAC_LOW_BYTE_INVALID;
    pstPsDomainInfo->stLastSuccRai.stLai.aucLac[1]      = NAS_MML_LAC_HIGH_BYTE_INVALID;

    pstPsDomainInfo->stPsDomainDrxPara.enPsRegisterContainDrx = NAS_MML_PS_REG_CONTAIN_BUTT;
    pstPsDomainInfo->stPsDomainDrxPara.ucNonDrxTimer          = NAS_MML_PS_DEFAULT_NON_DRX_TIMER;
    pstPsDomainInfo->stPsDomainDrxPara.ucUeUtranPsDrxLen      = NAS_MML_PS_UE_UTRAN_DEFAULT_DRX_LEN;
    pstPsDomainInfo->stPsDomainDrxPara.ucWSysInfoDrxLen       = NAS_MML_PS_UE_UTRAN_DEFAULT_DRX_LEN;
    pstPsDomainInfo->stPsDomainDrxPara.ucUeEutranPsDrxLen     = NAS_MML_PS_UE_LTE_DEFAULT_DRX_LEN;
    pstPsDomainInfo->stPsDomainDrxPara.ucLSysInfoDrxLen       = NAS_MML_PS_UE_LTE_DEFAULT_DRX_LEN;
    pstPsDomainInfo->stPsDomainDrxPara.ucSplitOnCcch          = NAS_MML_PS_DEFAULT_DRX_SPLIT_ON_CCCH;
    pstPsDomainInfo->stPsDomainDrxPara.ucSplitPgCycleCode     = NAS_MML_PS_DEFAULT_DRX_SPLIT_PG_CYCLE_CODE;

    NAS_MML_SetPsLocalDetachFlag(VOS_FALSE);
#if (FEATURE_ON == FEATURE_IMS)
    NAS_MML_SetGUNwImsVoiceSupportFlg(NAS_MML_NW_IMS_VOICE_NOT_SUPPORTED);
#endif

}

/*****************************************************************************
函 数 名  : NAS_MML_InitMsCurrBandInfoInfo
功能描述  : 初始化MS当前驻留的频段信息
输入参数  : 无
输出参数  : pstMsBandInfo:初始化MS当前频段相关信息
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期  : 2011年7月14日
 作    者   : zhoujun 40661
 修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitMsCurrBandInfoInfo(
    NAS_MML_MS_BAND_INFO_STRU         *pstMsBandInfo
)
{
    /* GSM的当前驻留的Band */
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm1800          = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm1900          = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm450           = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm480           = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm700           = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsm850           = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsmE900          = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsmP900          = VOS_FALSE;
    pstMsBandInfo->unGsmBand.stBitBand.BandGsmR900          = VOS_FALSE;

    /* WCDMA的当前驻留的Band */
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_III_1800 = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_II_1900  = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_IV_1700  = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_IX_J1700 = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_I_2100   = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_VIII_900 = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_VII_2600 = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_VI_800   = VOS_FALSE;
    pstMsBandInfo->unWcdmaBand.stBitBand.BandWCDMA_V_850    = VOS_FALSE;

    /* LTE的当前驻留的的Band */
    pstMsBandInfo->stLteBand.aulLteBand[0]                  = NAS_MML_LTE_ALL_BAND_INVALID;
    pstMsBandInfo->stLteBand.aulLteBand[1]                  = NAS_MML_LTE_ALL_BAND_INVALID;
}

/*****************************************************************************
函 数 名  : NAS_MML_InitNetwork3GppRel
功能描述  : 初始化MS当前驻留网络的协议版本
输入参数  : 无
输出参数  : pstMsBandInfo:初始化MS当前驻留网络的协议版本
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期  : 2011年7月14日
 作    者   : zhoujun 40661
 修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitNetwork3GppRel(
    NAS_MML_NETWORK_3GPP_REL_STRU      *pstNetwork3GppRel
)
{
    pstNetwork3GppRel->enNetMscRel      = NAS_MML_NETWORK_MSG_REL_VER_BUTT;
    pstNetwork3GppRel->enNetSgsnRel     = NAS_MML_NETWORK_SGSN_REL_VER_BUTT;
}



VOS_VOID  NAS_MML_InitEquPlmnInfo(
    NAS_MML_EQUPLMN_INFO_STRU          *pstEquPlmnInfo
)
{
    VOS_UINT32                          i;

    pstEquPlmnInfo->ucEquPlmnNum    = 0;
    for ( i = 0 ; i < NAS_MML_MAX_EQUPLMN_NUM ; i++ )
    {
        pstEquPlmnInfo->astEquPlmnAddr[i].ulMcc = NAS_MML_INVALID_MCC;
        pstEquPlmnInfo->astEquPlmnAddr[i].ulMnc = NAS_MML_INVALID_MNC;
    }

    pstEquPlmnInfo->ucValidFlg = VOS_FALSE;
}


VOS_VOID  NAS_MML_InitConnStatusInfo(
    NAS_MML_CONN_STATUS_INFO_STRU       *pstConnStatus
)
{
    pstConnStatus->ucCsSigConnStatusFlg         =   VOS_FALSE;
    pstConnStatus->ucPsSigConnStatusFlg         =   VOS_FALSE;
    pstConnStatus->ucPsTbfStatusFlg             =   VOS_FALSE;
    pstConnStatus->ucRrcStatusFlg               =   VOS_FALSE;
    pstConnStatus->ucCsServiceConnStatusFlg     =   VOS_FALSE;
    pstConnStatus->ucCsServiceBufferFlg         =   VOS_FALSE;
    pstConnStatus->ucPsServiceBufferFlg         =   VOS_FALSE;
    pstConnStatus->ucPdpStatusFlg               =   VOS_FALSE;
    pstConnStatus->ucEpsSigConnStatusFlg        =   VOS_FALSE;
    pstConnStatus->ucEpsServiceConnStatusFlg    =   VOS_FALSE;
    pstConnStatus->ucEmergencyServiceFlg        =   VOS_FALSE;
    pstConnStatus->ucPsTcServiceFlg             =   VOS_FALSE;
    pstConnStatus->ucEmcPdpStatusFlg            =   VOS_FALSE;

    pstConnStatus->enCsfbServiceStatus          =   NAS_MML_CSFB_SERVICE_STATUS_BUTT;
    PS_MEM_SET(pstConnStatus->aucReserved, 0x0, sizeof(pstConnStatus->aucReserved));
}


VOS_VOID NAS_MML_InitEpsDomainInfo(
    NAS_MML_EPS_DOMAIN_INFO_STRU       *pstEpsDomainInfo
)
{
   pstEpsDomainInfo->enT3412Status            = NAS_MML_TIMER_STOP;
   pstEpsDomainInfo->enT3423Status            = NAS_MML_TIMER_STOP;
   pstEpsDomainInfo->enAdditionUpdateRsltInfo = NAS_MML_ADDITION_UPDATE_RSLT_BUTT;
   pstEpsDomainInfo->enEpsRegStatus           = NAS_MML_REG_NOT_REGISTERED_NOT_SEARCH;
   pstEpsDomainInfo->enNwImsVoCap             = NAS_MML_NW_IMS_VOICE_NOT_SUPPORTED;  

   return;
}


VOS_VOID NAS_MML_InitImsDomainInfo(
    NAS_MML_IMS_DOMAIN_INFO_STRU        *pstImsDomainInfo
)
{
    pstImsDomainInfo->ucImsVoiceAvail   = VOS_FALSE;
    pstImsDomainInfo->enImsNormalRegSta = NAS_MML_IMS_NORMAL_REG_STATUS_DEREG; 
}


VOS_VOID NAS_MML_InitPsBearerContext(
    NAS_MML_PS_BEARER_CONTEXT_STRU     *pstPsBearerCtx
)
{
    VOS_UINT8                           i;

    for(i = 0; i < NAS_MML_MAX_PS_BEARER_NUM; i++)
    {
        pstPsBearerCtx[i].enPsBearerIsrFlg = NAS_MML_PS_BEARER_EXIST_BEFORE_ISR_ACT;
        pstPsBearerCtx[i].enPsBearerState  = NAS_MML_PS_BEARER_STATE_INACTIVE;
        pstPsBearerCtx[i].ucPsActPending   = VOS_FALSE;
        pstPsBearerCtx[i].aucReserved[0]   = 0;
    }

    return;
}


VOS_VOID NAS_MML_InitEmergencyNumList(
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstEmergencyNumList
)
{
    pstEmergencyNumList->ucEmergencyNumber = 0;
    PS_MEM_SET(pstEmergencyNumList->aucEmergencyList, (VOS_CHAR)0xFF,
               sizeof(pstEmergencyNumList->aucEmergencyList));
    return;
}


VOS_VOID  NAS_MML_InitNetworkCtx(
    NAS_MML_NETWORK_INFO_STRU           *pstNetworkInfo
)
{
    NAS_MML_InitCampPlmnInfo(&(pstNetworkInfo->stCampPlmnInfo));

    NAS_MML_InitEquPlmnInfo(&(pstNetworkInfo->stEquPlmnInfo));

    NAS_MML_InitNetwork3GppRel(&(pstNetworkInfo->stNetwork3GppRel));

    NAS_MML_InitMsCurrBandInfoInfo(&(pstNetworkInfo->stBandInfo));

    NAS_MML_InitPsDomainInfo(&(pstNetworkInfo->stPsDomainInfo));

    NAS_MML_InitCsDomainInfo(&(pstNetworkInfo->stCsDomainInfo));

    NAS_MML_InitEpsDomainInfo(&(pstNetworkInfo->stEpsDomainInfo));

    NAS_MML_InitImsDomainInfo(&(pstNetworkInfo->stImsDomainInfo));

    NAS_MML_InitConnStatusInfo(&(pstNetworkInfo->stConnStatus));
    NAS_MML_InitPsBearerContext(pstNetworkInfo->astPsBearerContext);
    NAS_MML_InitEmergencyNumList(&pstNetworkInfo->stEmergencyNumList);
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitInternalMsgQueue
 功能描述  : 初始化MML_CTX中内部消息队列
 输入参数  : pstInternalMsgQueue:内部消息队列
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年6月30日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitInternalMsgQueue(
    NAS_MML_INTERNAL_MSG_QUEUE_STRU     *pstInternalMsgQueue                   /* MM子层的内部消息队列 */
)
{
    VOS_UINT32                          i;

    pstInternalMsgQueue->ucHeader   = 0;
    pstInternalMsgQueue->ucTail     = 0;

    for ( i = 0 ; i < NAS_MML_MAX_MSG_QUEUE_SIZE; i++ )
    {
        pstInternalMsgQueue->astNasMmMsgQueue[i].usMsgID    = NAS_MML_INVALID_INTERNAL_MSG_ID;
        PS_MEM_SET(pstInternalMsgQueue->astNasMmMsgQueue[i].aucBuf,
                   0x00,
                   NAS_MML_MAX_INTERNAL_MSG_LEN);
    }

}
VOS_VOID  NAS_MML_InitMaintainInfo(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8    enInitType,
    NAS_MML_MAINTAIN_CTX_STRU          *pstMaintainInfo
)
{
    PS_MEM_SET(&g_stNasMntnErrorLogMm, 0, sizeof(g_stNasMntnErrorLogMm));

    /* 上电开机时才需要初始化如下变量 */
    if (NAS_MML_INIT_CTX_STARTUP != enInitType)
    {
        return;
    }

    /* 初始默认不发送PC回放消息 */
    pstMaintainInfo->stUeMaintainInfo.ucUeSndPcRecurFlg    = VOS_FALSE;

    /* 初始默认PC工具未连接 */
    pstMaintainInfo->stOmMaintainInfo.ucOmConnectFlg       = VOS_FALSE;

    /* 初始默认PC工具未使能NAS的PC回放消息发送 */
    pstMaintainInfo->stOmMaintainInfo.ucOmPcRecurEnableFlg = VOS_FALSE;

#if (FEATURE_ON == FEATURE_PTM)
    NAS_MML_InitErrLogMntnInfo(&pstMaintainInfo->stErrLogMntnInfo);

    pstMaintainInfo->stFtmMntnInfo.ucFtmCtrlFlag           = VOS_FALSE;
#endif
}

/*****************************************************************************
 函 数 名  : NAS_MML_InitCtx
 功能描述  : 开机初始化MML_CTX
 输入参数  : pstMmlCtx:MML上下文信息
             enInitType:INIT类型
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年6月30日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_InitCtx(
    NAS_MML_INIT_CTX_TYPE_ENUM_UINT8     enInitType,
    NAS_MML_CTX_STRU                    *pstMmlCtx
)
{
    /* 初始化SIM卡信息 */
    NAS_MML_InitSimInfoCtx(enInitType, &(pstMmlCtx->stSimInfo));

    /* 初始化MS配置信息 */
    NAS_MML_InitMsCfgCtx(&(pstMmlCtx->stMsCfgInfo));

    /* 初始化当前网络信息 */
    NAS_MML_InitNetworkCtx(&(pstMmlCtx->stNetworkInfo));

    /* 初始化内部消息队列 */
    NAS_MML_InitInternalMsgQueue(&(pstMmlCtx->stInternalMsgQueue));

    /* 初始化可谓可测信息 */
    NAS_MML_InitMaintainInfo(enInitType, &(pstMmlCtx->stMaintainInfo));

}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimInfo
 功能描述  : 获取MML中保存的SIM信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_INFO_STRU*  NAS_MML_GetSimInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetMsCfgInfo
 功能描述  : 获取MML中保存的MS的配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的MS的配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_MS_CFG_INFO_STRU* NAS_MML_GetMsCfgInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetNetworkInfo
 功能描述  : 获取MML中保存的当前网络的信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前网络的信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_NETWORK_INFO_STRU* NAS_MML_GetNetworkInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimStatus
 功能描述  : 获取MML中保存的SIM的状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM的状态
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_STATUS_STRU* NAS_MML_GetSimStatus(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimPresentStatus
 功能描述  : 获取MML中保存的SIM的是否在位的信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM的是否在位的信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetSimPresentStatus(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimPresentStatus);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimPresentStatus
 功能描述  : 获取MML中保存的SIM的类型,USIM或SIM卡
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡类型
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_TYPE_ENUM_UINT8 NAS_MML_GetSimType(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enSimType);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimType
 功能描述  : 设置MML中保存的SIM的类型,USIM或SIM卡
 输入参数  : enSimType - sim卡类型
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡类型
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimType(
    NAS_MML_SIM_TYPE_ENUM_UINT8                 enSimType
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enSimType = enSimType;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimCsRegStatus
 功能描述  : 获取MML中保存的CS域的SIM卡注册信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的CS域的SIM卡注册信息,有效或无效
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetSimCsRegStatus(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimCsRegStatus);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimPsRegStatus
 功能描述  : 获取MML中保存的PS域的SIM卡注册信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的PS域的SIM卡注册信息,有效或无效
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetSimPsRegStatus(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimPsRegStatus);
}


VOS_VOID NAS_MML_SetSimPresentStatus(
    VOS_UINT8                           ucSimPresentStatus
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimPresentStatus = ucSimPresentStatus;
}


VOS_VOID NAS_MML_SetSimCsRegStatus(
    VOS_UINT8                           ucSimCsRegStatus
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimCsRegStatus = ucSimCsRegStatus;
}


VOS_VOID NAS_MML_SetSimPsRegStatus(
    VOS_UINT8                           ucSimPsRegStatus
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.ucSimPsRegStatus = ucSimPsRegStatus;
}


VOS_VOID NAS_MML_SetPsUpdateStatus(
    NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8    enPsUpdateStatus
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enPsUpdateStatus = enPsUpdateStatus;
}


NAS_MML_ROUTING_UPDATE_STATUS_ENUM_UINT8 NAS_MML_GetPsUpdateStatus(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enPsUpdateStatus;
}


VOS_VOID NAS_MML_SetCsUpdateStatus(
    NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8   enCsUpdateStatus
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enCsUpdateStatus = enCsUpdateStatus;
}


NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8 NAS_MML_GetCsUpdateStatus(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stSimStatus.enCsUpdateStatus;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimMsIdentity
 功能描述  : 获取MML中保存的SIM卡中MS的IDENTITY
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中MS的IDENTITY
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_MS_IDENTITY_STRU* NAS_MML_GetSimMsIdentity(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity);
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetSimMsIdentity
 功能描述  : 更新MML中保存的SIM卡中MS的IDENTITY
 输入参数  : pstSimMsIdentity:需要更新的SIM卡中MS的IDENTITY
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimMsIdentity(
    NAS_MML_SIM_MS_IDENTITY_STRU        *pstSimMsIdentity
)
{
    PS_MEM_CPY(&((NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity)),
               pstSimMsIdentity,
               sizeof(NAS_MML_SIM_MS_IDENTITY_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimImsi
 功能描述  : 获取MML中保存的SIM卡中IMSI
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中IMSI
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetSimImsi(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucImsi;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetUeIdPtmsi
 功能描述  : 获取MML中保存的Ptmsi
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的Ptmsi
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetUeIdPtmsi(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsi;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetUeIdPtmsi
 功能描述  : 更新MML中保存Ptmsi
 输入参数  : pucPtmsi:需要更新PTMSI
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetUeIdPtmsi(
    VOS_UINT8                          *pucPtmsi
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsi,
               pucPtmsi,
               (sizeof(VOS_UINT8) * NAS_MML_MAX_PTMSI_LEN));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetUeIdPtmsiSignature
 功能描述  : 获取MML中保存的Ptmsi Signature
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的Ptmsi
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetUeIdPtmsiSignature(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsiSignature;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetUeIdPtmsiSignature
 功能描述  : 更新MML中保存Ptmsi
 输入参数  : pucPtmsi:需要更新PTMSI SIGNATURE
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetUeIdPtmsiSignature(
    VOS_UINT8                          *pucPtmsiSignature
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucPtmsiSignature,
               pucPtmsiSignature,
               (sizeof(VOS_UINT8) * NAS_MML_MAX_PTMSI_SIGNATURE_LEN));
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetUeIdTmsi
 功能描述  : 获取MML中保存的Tmsi
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的Tmsi
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetUeIdTmsi(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucTmsi;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetUeIdTmsi
 功能描述  : 更新MML中保存Tmsi
 输入参数  : pucTmsi:需要更新Tmsi
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetUeIdTmsi(
    VOS_UINT8                          *pucTmsi
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.aucTmsi,
               pucTmsi,
               NAS_MML_MAX_TMSI_LEN);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetUeOperMode
 功能描述  : 更新MML中保存UeOperMode
 输入参数  : enUeOperMode:UE模式
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月27日
   作    者   : huwen 44270
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetUeOperMode(
    NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8 enUeOperMode
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.enUeOperMode = enUeOperMode;
}

NAS_MML_SIM_UE_OPER_MODE_ENUM_UINT8 NAS_MML_GetUeOperMode(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stMsIdentity.enUeOperMode;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimPsSecurity
 功能描述  : 获取MML中保存的SIM卡中PS域的安全参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中PS域的安全参数
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_PS_SECURITY_INFO_STRU* NAS_MML_GetSimPsSecurity(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity);
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetSimPsSecurity
 功能描述  : 更新MML中保存的SIM卡中PS域的安全参数
 输入参数  : pstSimPsSecruity:需要更新的SIM卡中PS域的安全参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimPsSecurity(
    NAS_MML_SIM_PS_SECURITY_INFO_STRU   *pstSimPsSecruity

)
{
    PS_MEM_CPY(&((NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity)),
               pstSimPsSecruity,
               sizeof(NAS_MML_SIM_PS_SECURITY_INFO_STRU));
}



VOS_UINT8 NAS_MML_GetSimPsSecurityCksn(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.ucCKSN;
}


VOS_VOID NAS_MML_SetSimPsSecurityCksn(
    VOS_UINT8                           ucCksn
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.ucCKSN = ucCksn;
}


VOS_UINT8* NAS_MML_GetSimPsSecurityUmtsCk(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsCk;
}


VOS_VOID NAS_MML_SetSimPsSecurityUmtsCk(
    VOS_UINT8                           *pucUmtsCk
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsCk,
               pucUmtsCk,
               (sizeof(VOS_UINT8) * NAS_MML_UMTS_CK_LEN));
}



VOS_UINT8* NAS_MML_GetSimPsSecurityUmtsIk(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsIk;
}


VOS_VOID NAS_MML_SetSimPsSecurityUmtsIk(
    VOS_UINT8                           *pucUmtsIk
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucUmtsIk,
               pucUmtsIk,
               (sizeof(VOS_UINT8) * NAS_MML_UMTS_IK_LEN));
}


VOS_UINT8* NAS_MML_GetSimPsSecurityGsmKc(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucGsmKc;
}


VOS_VOID NAS_MML_SetSimPsSecurityGsmKc(
    VOS_UINT8                           *pucGsmKc
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stPsSecurity.aucGsmKc,
               pucGsmKc,
               (sizeof(VOS_UINT8) * NAS_MML_GSM_KC_LEN));
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimCsSecurity
 功能描述  : 获取MML中保存的SIM卡中CS域的安全参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中CS域的安全参数
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_CS_SECURITY_INFO_STRU* NAS_MML_GetSimCsSecurity(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity);
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetSimCsSecurity
 功能描述  : 更新MML中保存的SIM卡中CS域的安全参数
 输入参数  : pstSimCsSecruity:需要更新的SIM卡中CS域的安全参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimCsSecurity(
    NAS_MML_SIM_CS_SECURITY_INFO_STRU  *pstSimCsSecruity
)
{
    PS_MEM_CPY(&((NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity)),
               pstSimCsSecruity,
               sizeof(NAS_MML_SIM_CS_SECURITY_INFO_STRU));
}



VOS_UINT8 NAS_MML_GetSimCsSecurityCksn(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.ucCKSN;
}


VOS_VOID NAS_MML_SetSimCsSecurityCksn(
    VOS_UINT8                           ucCksn
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.ucCKSN = ucCksn;
}


VOS_UINT8* NAS_MML_GetSimCsSecurityUmtsCk(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsCk;
}


VOS_VOID NAS_MML_SetSimCsSecurityUmtsCk(
    VOS_UINT8                           *pucUmtsCk
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsCk,
               pucUmtsCk,
               (sizeof(VOS_UINT8) * NAS_MML_UMTS_CK_LEN));
}



VOS_UINT8* NAS_MML_GetSimCsSecurityUmtsIk(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsIk;
}


VOS_VOID NAS_MML_SetSimCsSecurityUmtsIk(
    VOS_UINT8                           *pucUmtsIk
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucUmtsIk,
               pucUmtsIk,
               (sizeof(VOS_UINT8) * NAS_MML_UMTS_IK_LEN));
}


VOS_UINT8* NAS_MML_GetSimCsSecurityGsmKc(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucGsmKc;
}


VOS_VOID NAS_MML_SetSimCsSecurityGsmKc(
    VOS_UINT8                           *pucGsmKc
)
{
    PS_MEM_CPY(NAS_MML_GetMmlCtx()->stSimInfo.stCsSecurity.aucGsmKc,
               pucGsmKc,
               (sizeof(VOS_UINT8) * NAS_MML_GSM_KC_LEN));
}



/*****************************************************************************
 函 数 名  : NAS_MML_GetSimEhplmnList
 功能描述  : 获取MML中保存的SIM卡中EHPLMN的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中EHPLMN的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_EHPLMN_INFO_STRU* NAS_MML_GetSimEhplmnList(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stEhPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimEhplmnList
 功能描述  : 更新MML中保存的SIM卡中EHPLMN的内容
 输入参数  : pstEhplmnList:需更新的EHPLMN的内容
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimEhplmnList(
    NAS_MML_SIM_EHPLMN_INFO_STRU        *pstEhplmnList
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stEhPlmnInfo),
               pstEhplmnList,
               sizeof(NAS_MML_SIM_EHPLMN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimHplmnWithRatList
 功能描述  : 获取MML中保存的SIM卡中HPLMNWITHRAT的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中HPLMNWITHRAT的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU* NAS_MML_GetSimHplmnWithRatList(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stHplmnWithRatInfo);
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetSimHplmnWithRatList
 功能描述  : 更新MML中保存的SIM卡中HPLMNWITHRAT的内容
 输入参数  : pstHplmnWithRatList:需要更新的HPLMNWITHRAT的内容
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimHplmnWithRatList(
    NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU    *pstHplmnWithRatList
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stHplmnWithRatInfo),
               pstHplmnWithRatList,
               sizeof(NAS_MML_SIM_HPLMN_WITH_RAT_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimUserPlmnList
 功能描述  : 获取MML中保存的SIM卡中USERPLMN的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中USERPLMN的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_USERPLMN_INFO_STRU* NAS_MML_GetSimUserPlmnList(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stUserPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimUserPlmnList
 功能描述  : 更新MML中保存的SIM卡中USERPLMN的内容
 输入参数  : pstUserPlmnList:需更新USERPLMN的内容
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimUserPlmnList(
    NAS_MML_SIM_USERPLMN_INFO_STRU      *pstUserPlmnList
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stUserPlmnInfo),
               pstUserPlmnList,
               sizeof(NAS_MML_SIM_USERPLMN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimOperPlmnList
 功能描述  : 获取MML中保存的SIM卡中OPERPLMN的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中OPERPLMN的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_OPERPLMN_INFO_STRU* NAS_MML_GetSimOperPlmnList(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stOperPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimOperPlmnList
 功能描述  : 更新MML中保存的SIM卡中OPERPLMN的内容
 输入参数  : pstOperPlmnList:需更新OPERPLMN的内容
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimOperPlmnList(
    NAS_MML_SIM_OPERPLMN_INFO_STRU     *pstOperPlmnList
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stOperPlmnInfo),
               pstOperPlmnList,
               sizeof(NAS_MML_SIM_OPERPLMN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimSelPlmnList
 功能描述  : 获取MML中保存的SIM卡中SELPLMN的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中SELPLMN的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_SELPLMN_INFO_STRU* NAS_MML_GetSimSelPlmnList(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stSelPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimSelPlmnList
 功能描述  : 更新MML中保存的SIM卡中SELPLMN的内容
 输入参数  : 无
 输出参数  : pstSelPlmnList:需要更新的SELPLMN的内容
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimSelPlmnList(
    NAS_MML_SIM_SELPLMN_INFO_STRU       *pstSelPlmnList
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stSelPlmnInfo),
               pstSelPlmnList,
               sizeof(NAS_MML_SIM_SELPLMN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimForbidPlmnList
 功能描述  : 获取MML中保存的SIM卡中FORBID网络信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的SIM卡中FORBID网络信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_SIM_FORBIDPLMN_INFO_STRU* NAS_MML_GetForbidPlmnInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stForbidPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimForbidPlmnList
 功能描述  : 更新MML中保存的SIM卡中FORBID网络信息
 输入参数  : pstForbidPlmnInfo:需更新的FORBID网络信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimForbidPlmnList(
    NAS_MML_SIM_FORBIDPLMN_INFO_STRU    *pstForbidPlmnInfo
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stForbidPlmnInfo),
               pstForbidPlmnInfo,
               sizeof(NAS_MML_SIM_FORBIDPLMN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetSimHplmnTimerLen
 功能描述  : 更新MML中保存SIM卡中HPLMN定时器的时长
 输入参数  : ulSimHplmnTimerLen:SIM卡中HPLMN定时器的时长
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetSimHplmnTimerLen(
    VOS_UINT32                          ulSimHplmnTimerLen
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.ulSimHplmnTimerLen = ulSimHplmnTimerLen;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimHplmnTimerLen
 功能描述  : 获取MML中保存SIM卡中HPLMN定时器的时长
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存SIM卡中HPLMN定时器的时长
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_MML_GetSimHplmnTimerLen(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.ulSimHplmnTimerLen;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSelplmnList
 功能描述  : 获取MML中保存当前MS支持的协议版本
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存当前MS支持的协议版本
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_MS_3GPP_REL_STRU* NAS_MML_GetMs3GppRel(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMs3GppRel);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetMs3GppRel
 功能描述  : 更新MML中保存当前MS支持的协议版本
 输入参数  : pstMs3GppRel:需要更新的MS的协议版本
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetMs3GppRel(
    NAS_MML_MS_3GPP_REL_STRU           *pstMs3GppRel
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMs3GppRel),
               pstMs3GppRel,
               sizeof(NAS_MML_MS_3GPP_REL_STRU));
}


/*****************************************************************************
 函 数 名  : NAS_MMC_GetMsCapability
 功能描述  : 获取MML中保存当前MS支持的手机能力
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存当前MS支持的手机能力
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_MS_CAPACILITY_INFO_STRU* NAS_MML_GetMsCapability(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetImeisv
 功能描述  : 获取MML中保存当前MS的IMEISV的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存当前MS的IMEISV的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8* NAS_MML_GetImeisv( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.aucImeisv);
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetMsMode
 功能描述  : 获取当前手机模式
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 获取当前手机模式
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月9日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_MS_MODE_ENUM_UINT8 NAS_MML_GetMsMode( VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enMsMode);
}


NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8 NAS_MML_GetLteCsServiceCfg( VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteCsServiceCfg;
}






NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32 NAS_MML_GetLteCapabilityStatus( VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteCapabilityStatus);
}


MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32 NAS_MML_GetDisableLteReason( VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enDisableLteReason);
}


VOS_UINT8 NAS_MML_GetLteDisabledRauUseLteInfoFlag(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucLteDisabledRauUseLteInfoFlag);
}


VOS_VOID NAS_MML_SetLteDisabledRauUseLteInfoFlag(VOS_UINT8 ucLteDisabledRauUseLteInfoFlag)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucLteDisabledRauUseLteInfoFlag = ucLteDisabledRauUseLteInfoFlag;
    return;
}



VOS_UINT8   NAS_MML_GetLteRoamAllowedFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.stLteRoamCfg.ucLteRoamAllowedFlg);
}


VOS_UINT32 NAS_MML_GetDisableLteRoamFlg( VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ulDisableLteRoamFlg);
}


VOS_VOID NAS_MML_GetRoamEnabledMccList(VOS_UINT32 *pulMcc)
{
    VOS_UINT32      i;

    for (i=0; i<NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER; i++)
    {
        pulMcc[i] = NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.stLteRoamCfg.aulRoamEnabledMccList[i];
    }
}



NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetEpsRegStatus( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enEpsRegStatus);
}



VOS_UINT8 NAS_MML_GetDailRejectCfg( VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stDailRejectCfg.ucEnableFlg;
}



NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8 NAS_MML_GetCsfbServiceStatus( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.enCsfbServiceStatus;
}


NAS_MML_PS_BEARER_CONTEXT_STRU* NAS_MML_GetPsBearerCtx( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.astPsBearerContext;
}


NAS_MML_TIMER_INFO_ENUM_UINT8 NAS_MML_GetT3412Status(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enT3412Status;
}


NAS_MML_TIMER_INFO_ENUM_UINT8 NAS_MML_GetT3423Status(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enT3423Status;
}


NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8 NAS_MML_GetAdditionUpdateRslt(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enAdditionUpdateRsltInfo;
}


NAS_MML_EMERGENCY_NUM_LIST_STRU* NAS_MML_GetEmergencyNumList (VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stEmergencyNumList);
}



NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8 NAS_MML_GetLteUeUsageSetting( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteUeUsageSetting;
}


NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8 NAS_MML_GetVoiceDomainPreference( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enVoiceDomainPreference;
}



VOS_UINT32 NAS_MML_GetHoWaitSysinfoTimerLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulHoWaitSysinfoTimeLen);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetPsAutoAttachFlg
 功能描述  : 获取当前PS域是否自动Attach标识
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 获取当前PS域是否自动Attach标识
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月9日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetPsAutoAttachFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ucPsAutoAttachFlg);
}


NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8 NAS_MML_GetChangeRegRejCauFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.enChangeRegRejCauseFlg);
}


VOS_UINT8 NAS_MML_GetPreferredRegRejCause_HPLMN_EHPLMN(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.ucPreferredRegRejCau_HPLMN_EHPLMN);
}


VOS_UINT8 NAS_MML_GetPreferredRegRejCause_NOT_HPLMN_EHPLMN(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.ucPreferredRegRejCau_NOT_HPLMN_EHPLMN);
}



NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU* NAS_MML_GetHplmnRejCauseChangedCounter(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.stHplmnRejCauseChangedCounter);
}


VOS_UINT8 NAS_MML_GetUserAutoReselActiveFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucUserAutoReselActiveFlg);
}

VOS_VOID NAS_MML_SetUserAutoReselActiveFlg(VOS_UINT8 ucActiveFlag)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucUserAutoReselActiveFlg = ucActiveFlag;
}



VOS_VOID NAS_MML_SetMsMode(
    NAS_MML_MS_MODE_ENUM_UINT8          enMsMode
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enMsMode = enMsMode;
}


VOS_VOID NAS_MML_SetLteCsServiceCfg(
    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8                   enLteCsServiceCfg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteCsServiceCfg
                               = enLteCsServiceCfg;
}



VOS_VOID NAS_MML_SetLteCapabilityStatus(
    NAS_MML_LTE_CAPABILITY_STATUS_ENUM_UINT32               enLCapabilityStatus
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteCapabilityStatus = enLCapabilityStatus;
}


VOS_VOID NAS_MML_SetDisableLteReason(
    MMC_LMM_DISABLE_LTE_REASON_ENUM_UINT32                  enDisableLteReason
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enDisableLteReason = enDisableLteReason;
}


VOS_VOID NAS_MML_SetDisableLteRoamFlg(
    VOS_UINT32                          ulDisableFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ulDisableLteRoamFlg
                               = ulDisableFlg;
}


VOS_VOID NAS_MML_SetRoamEnabledMccList(VOS_UINT32 *pulMcc)
{
    VOS_UINT32      i;

    for (i=0; i<NAS_MML_ROAM_ENABLED_MAX_MCC_NUMBER; i++)
    {
        NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.stLteRoamCfg.aulRoamEnabledMccList[i] = pulMcc[i];
    }
}


VOS_VOID NAS_MML_SetRoamRplmnSearchFlg(VOS_UINT8 ucRoamSearchRPLMNFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.ucRoamSearchRPLMNFlg = ucRoamSearchRPLMNFlg;
}


VOS_UINT8 NAS_MML_GetRoamRplmnSearchFlg()
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.ucRoamSearchRPLMNFlg;
}


VOS_UINT32 *NAS_MML_GetNationalMCCList()
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.aucNationalMccList;
}


VOS_VOID NAS_MML_SetNationalMCCList(VOS_UINT32 *pulMcc)
{
    VOS_UINT32      i;

    for (i=0; i<NAS_MML_MAX_NATIONAL_MCC_NUM; i++)
    {
        NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.aucNationalMccList[i] = pulMcc[i];
    }
}



VOS_VOID NAS_MML_SetDisableLTEOnBandFlg(VOS_UINT8 ucDisableLTEOnBandFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucDisableLTEOnBandFlg = ucDisableLTEOnBandFlg;
}


VOS_UINT8 NAS_MML_GetDisableLTEOnBandFlg()
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucDisableLTEOnBandFlg;
}




VOS_VOID NAS_MML_SetUltraFlashCsfbSupportFlg(
    VOS_UINT8                           ucUltraFlashCsfbSupportFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucUltraFlashCsfbSupportFlg = ucUltraFlashCsfbSupportFlg;
}


VOS_UINT8 NAS_MML_GetUltraFlashCsfbSupportFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucUltraFlashCsfbSupportFlg;
}



VOS_VOID NAS_MML_SetLteRoamAllowedFlg(
    VOS_UINT8                           ucAllowedFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.stLteRoamCfg.ucLteRoamAllowedFlg
                               = ucAllowedFlg;
}



VOS_VOID NAS_MML_SetEpsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enEpsRegStatus
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enEpsRegStatus = enEpsRegStatus;
}





VOS_VOID NAS_MML_SetRelCauseCsfbHighPrioFlg(
    VOS_UINT8       ucFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.ucIsRelCauseCsfbHighPrio = ucFlg;
}


VOS_UINT32  NAS_MML_GetRelCauseCsfbHighPrioFlg()
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.ucIsRelCauseCsfbHighPrio);
}


VOS_VOID NAS_MML_SetChangeRegRejCauFlg(
    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8            enEnableFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.enChangeRegRejCauseFlg
                               = enEnableFlg;
}


VOS_VOID NAS_MML_SetPreferredRegRejCause_HPLMN_EHPLMN(
    VOS_UINT8                           ucRejectCause
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.ucPreferredRegRejCau_HPLMN_EHPLMN
                               = ucRejectCause;
}


VOS_VOID NAS_MML_SetPreferredRegRejCause_NOT_HPLMN_EHPLMN(
    VOS_UINT8                           ucRejectCause
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stChangeRegRejCauInfo.ucPreferredRegRejCau_NOT_HPLMN_EHPLMN
                               = ucRejectCause;
}


VOS_VOID NAS_MML_SetDailRejectCfg(
    VOS_UINT8                           ucEnableFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stDailRejectCfg.ucEnableFlg
                               = ucEnableFlg;
}



VOS_VOID NAS_MML_SetCsfbServiceStatus(
    NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8                     enCsfbServiceStatus
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.enCsfbServiceStatus = enCsfbServiceStatus;
}


VOS_VOID NAS_MML_SetVoiceDomainPreference(
    NAS_MML_VOICE_DOMAIN_PREFERENCE_ENUM_UINT8              enVoiceDomainPrefer
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enVoiceDomainPreference = enVoiceDomainPrefer;
}


VOS_VOID NAS_MML_SetLteUeUsageSetting(
    NAS_MML_LTE_UE_USAGE_SETTING_ENUM_UINT8                 enUeUsageSetting
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.enLteUeUsageSetting = enUeUsageSetting;
}

VOS_VOID NAS_MML_SetT3412Status(NAS_MML_TIMER_INFO_ENUM_UINT8 enTimerStatus)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enT3412Status = enTimerStatus;
}


VOS_VOID NAS_MML_SetT3423Status(NAS_MML_TIMER_INFO_ENUM_UINT8 enTimerStatus)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enT3423Status = enTimerStatus;
}


VOS_VOID NAS_MML_SetAdditionUpdateRslt(
    NAS_MML_ADDITION_UPDATE_RSLT_INFO_ENUM_UINT8            enAdditionUpdRslt
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enAdditionUpdateRsltInfo = enAdditionUpdRslt;
}


VOS_VOID NAS_MML_SetEmergencyNumList (
    NAS_MML_EMERGENCY_NUM_LIST_STRU    *pstEmcNumList
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEmergencyNumList = *pstEmcNumList;
}


VOS_VOID NAS_MML_SetTinType(
    NAS_MML_TIN_TYPE_ENUM_UINT8         enTinType
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRplmnCfg.enTinType = enTinType;
}


VOS_VOID NAS_MML_SetPsBearerCtx(
    VOS_UINT8                           ucRabId,
    NAS_MML_PS_BEARER_CONTEXT_STRU     *pstPsBearerCtx
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.astPsBearerContext[ucRabId - NAS_MML_NSAPI_OFFSET] = *pstPsBearerCtx;
}


VOS_VOID NAS_MML_SetHoWaitSysinfoTimerLen(
    VOS_UINT32                          ulHoWaitSysinfoTimeLen
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulHoWaitSysinfoTimeLen = ulHoWaitSysinfoTimeLen;
}


VOS_VOID NAS_MML_SetCsfbEmgCallEnableLteTimerLen(
    VOS_UINT32                          ulEnalbeLteTimeLen
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulCsfbEmgCallEnableLteTimerLen = ulEnalbeLteTimeLen;
}


VOS_VOID NAS_MML_SetCsPsMode1EnableLteTimerLen(
    VOS_UINT32                          ulEnalbeLteTimeLen
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulCsPsMode1EnableLteTimerLen = ulEnalbeLteTimeLen;
}


VOS_VOID NAS_MML_SetSvlteSupportFlag(
    VOS_UINT8                           ucSvlteSupportFlag
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSvlteSupportFlag = ucSvlteSupportFlag;
}


VOS_UINT8 NAS_MML_GetSvlteSupportFlag(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSvlteSupportFlag);
}



VOS_UINT32 NAS_MML_GetCsfbEmgCallEnableLteTimerLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulCsfbEmgCallEnableLteTimerLen);
}


VOS_UINT32 NAS_MML_GetCsPsMode1EnableLteTimerLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ulCsPsMode1EnableLteTimerLen);
}



VOS_VOID NAS_MML_SetCsfbEmgCallLaiChgLauFirstFlg(
    VOS_UINT8                           ucCsfbEmgCallLaiChgLauFirstFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucCsfbEmgCallLaiChgLauFirstFlg
                         = ucCsfbEmgCallLaiChgLauFirstFlg;
}


VOS_UINT8 NAS_MML_GetCsfbEmgCallLaiChgLauFirstFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucCsfbEmgCallLaiChgLauFirstFlg);
}



VOS_VOID NAS_MML_SetPsAutoAttachFlg(
    VOS_UINT8                           usPsAutoAttachFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ucPsAutoAttachFlg = usPsAutoAttachFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetMsSupportBand
 功能描述  : 获取MML中保存当前MS支持的手机Band
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存当前MS支持的手机Band
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_MS_BAND_INFO_STRU* NAS_MML_GetMsSupportBand( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.stMsBand);
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetMsPrioRatList
 功能描述  : 获取MML中保存当前MS支持的接入模式和接入优先级
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存当前MS支持的接入模式和接入优先级
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_PLMN_RAT_PRIO_STRU* NAS_MML_GetMsPrioRatList( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.stPrioRatList);
}


NAS_MML_3GPP2_RAT_PRIO_STRU* NAS_MML_GetMs3Gpp2PrioRatList( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.st3Gpp2RatList);
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetRplmnCfg
 功能描述  : 获取MML中保存RPLMN的配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存RPLMN的配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_RPLMN_CFG_INFO_STRU* NAS_MML_GetRplmnCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRplmnCfg);
}


NAS_MML_NET_RAT_TYPE_ENUM_UINT8 NAS_MML_GetLastRplmnRat( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRplmnCfg.enLastRplmnRat);
}


VOS_VOID NAS_MML_SetLastRplmnRat(NAS_MML_NET_RAT_TYPE_ENUM_UINT8 enRat)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRplmnCfg.enLastRplmnRat = enRat;
    return;
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetHplmnCfg
 功能描述  : 获取MML中保存HPLMN的配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存HPLMN的配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_HPLMN_CFG_INFO_STRU* NAS_MML_GetHplmnCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stHplmnCfg);
}


/*****************************************************************************
 函 数 名  : NAS_MMC_GetRoamCfg
 功能描述  : 获取MML中保存漫游配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存漫游配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_ROAM_CFG_INFO_STRU* NAS_MML_GetRoamCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg);
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetAisRoamingCfg
 功能描述  : 获取MML中保存泰国漫游配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存泰国漫游配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2012年5月17日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_AIS_ROAMING_CFG_STRU* NAS_MML_GetAisRoamingCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.stAisRoamingCfg);
}




NAS_MML_PRIO_HPLMNACT_CFG_STRU* NAS_MML_GetPrioHplmnActCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stHplmnCfg.stPrioHplmnActCfg);
}


/*****************************************************************************
 函 数 名  : NAS_MMC_GetBgSearchCfg
 功能描述  : 获取MML中保存BG搜索信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存BG搜索信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_BG_SEARCH_CFG_INFO_STRU* NAS_MML_GetBgSearchCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetPlmnLockCfg
 功能描述  : 获取MML中保存运营商锁网信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存运营商锁网信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_PLMN_LOCK_CFG_INFO_STRU* NAS_MML_GetPlmnLockCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stPlmnLockCfg);
}



NAS_MML_DISABLED_RAT_PLMN_CFG_INFO_STRU* NAS_MML_GetDisabledRatPlmnCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stPlmnLockCfg.stDisabledRatPlmnCfg);
}



NAS_MML_RAT_FORBIDDEN_LIST_STRU* NAS_MML_GetRatForbiddenListCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatForbiddenListInfo);
}



NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetGsmForbiddenStatusFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucGsmCapaStatus);
}



VOS_VOID NAS_MML_SetGsmForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enGsmCapaStatus )
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucGsmCapaStatus = enGsmCapaStatus;
}



NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetUtranForbiddenStatusFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucUtranCapaStatus);
}



VOS_VOID NAS_MML_SetUtranForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enUtranCapaStatus )
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucUtranCapaStatus = enUtranCapaStatus;
}



NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 NAS_MML_GetLteForbiddenStatusFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucLteCapaStatus);
}



VOS_VOID NAS_MML_SetLteForbiddenStatusFlg( NAS_MML_RAT_CAPABILITY_STATUS_ENUM_UINT8 enLteCapaStatus )
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucLteCapaStatus = enLteCapaStatus;
}



VOS_UINT8 NAS_MML_GetImsiInForbiddenListFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucIsImsiInForbiddenListFlg);
}



VOS_VOID NAS_MML_SetImsiInForbiddenListFlg( VOS_UINT8 ucIsImsiInForbiddenList )
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRatFirbiddenStatusCfg.ucIsImsiInForbiddenListFlg = ucIsImsiInForbiddenList;
}



NAS_MML_MISCELLANEOUS_CFG_INFO_STRU* NAS_MML_GetMiscellaneousCfgInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetStkSteeringOfRoamingSupportFlg
 功能描述  : 获取MML中是否支持stk streer of roaming这个特性
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_FALSE:不支持,VOS_TRUE:支持
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetStkSteeringOfRoamingSupportFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucStkSteeringOfRoamingSupportFlg);
}


VOS_VOID NAS_MML_SetStkSteeringOfRoamingSupportFlg(
    VOS_UINT8                           ucStkSteeringOfRoamingSupportFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucStkSteeringOfRoamingSupportFlg
                                             = ucStkSteeringOfRoamingSupportFlg;
}



NAS_MML_AVAIL_TIMER_CFG_STRU* NAS_MML_GetAvailTimerCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stAvailTimerCfg);
}


NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU* NAS_MML_GetHighPrioRatHplmnTimerCfg( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.stHighRatHplmnTimerCfg);
}



VOS_UINT8  NAS_MML_GetHighPrioRatHplmnTimerActiveFlg( VOS_VOID )
{
    NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU             *pstHighRatHplmnTimerCfg = VOS_NULL_PTR;

    pstHighRatHplmnTimerCfg = NAS_MML_GetHighPrioRatHplmnTimerCfg();

    return pstHighRatHplmnTimerCfg->ucActiveFLg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetStkSteeringOfRoamingSupportFlg
 功能描述  : 获取MML中vodafone的搜网定制需求,CS域失败四次后下发搜网请求
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_FALSE:不支持,VOS_TRUE:支持
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8 NAS_MML_GetCsRejSearchSupportFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucCsRejSearchSupportFlg);
}


VOS_UINT8 NAS_MML_GetRoamBrokerActiveFlag( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucRoamBrokerActiveFlag);
}


VOS_UINT8 NAS_MML_GetRoamBrokerRegisterFailCnt( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucRoamBrokerRegisterFailCnt);
}


VOS_VOID NAS_MML_SetRoamBrokerRegisterFailCnt(
    VOS_UINT8                           ucRoamBrokerRegisterFailCnt
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucRoamBrokerRegisterFailCnt
                       = ucRoamBrokerRegisterFailCnt;
}

VOS_UINT8 NAS_MML_GetDtSingleDomainPlmnSearchFlag( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucSingleDomainFailPlmnSrchFlag);
}


VOS_VOID NAS_MML_SetDtSingleDomainPlmnSearchFlag(
    VOS_UINT8                           ucPlmnSrchFlag
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucSingleDomainFailPlmnSrchFlag
                       = ucPlmnSrchFlag;
}


VOS_VOID NAS_MML_SetCsRejSearchSupportFlg(
    VOS_UINT8                           ucCsRejSearchSupportFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucCsRejSearchSupportFlg
                                        = ucCsRejSearchSupportFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCampPlmnInfo
 功能描述  : 获取MML中保存的当前驻留PLMN信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前驻留PLMN信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_CAMP_PLMN_INFO_STRU* NAS_MML_GetCurrCampPlmnInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo) ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCampCellInfo
 功能描述  : 获取MML中保存的当前驻留小区信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前驻留小区信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_CAMP_CELL_INFO_STRU* NAS_MML_GetCampCellInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stCampCellInfo) ;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetCurrNetType
 功能描述  : 获取当前的网络接入模式
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 当前的网络接入模式
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月16日
    作    者   : zhoujun 40661
    修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_NET_RAT_TYPE_ENUM_UINT8  NAS_MML_GetCurrNetRatType( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.enNetRatType) ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetCurrNetType
 功能描述  : 更新当前的网络接入模式
 输入参数  : enNetRatType:当前网络接入模式
 输出参数  : 无
 返 回 值  : 当前的网络接入模式
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月16日
    作    者   : zhoujun 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_SetCurrNetRatType(
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enNetRatType
)
{
    (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.enNetRatType) = enNetRatType ;
}

VOS_VOID  NAS_MML_SetCurrSysSubMode(
    NAS_MML_RRC_SYS_SUBMODE_ENUM_UINT8  enSysSubMode
)
{
    (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.enSysSubMode) = enSysSubMode ;
}
/*****************************************************************************
 函 数 名  : NAS_MML_GetCurrCampPlmnId
 功能描述  : 获取当前驻留网络的PLMNID
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月16日
    作    者   : zhoujun 40661
    修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_PLMN_ID_STRU*  NAS_MML_GetCurrCampPlmnId( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stLai.stPlmnId) ;
}


NAS_MML_LAI_STRU*  NAS_MML_GetCurrCampLai( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stLai) ;
}



VOS_UINT8 NAS_MML_GetCurrCampRac(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.ucRac;
}



VOS_UINT16 NAS_MML_GetCurrCampArfcn(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stCampCellInfo.usArfcn;
}



VOS_VOID NAS_MML_SetCurrCampArfcn(
    VOS_UINT16                          usArfcn
)
{
    (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stCampCellInfo.usArfcn) = usArfcn;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCurrBandInfo
 功能描述  : 获取MML中保存的当前驻留频段信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前驻留PLMN信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_MS_BAND_INFO_STRU* NAS_MML_GetCurrBandInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stBandInfo) ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCsDomainInfo
 功能描述  : 获取MML中保存的CS域信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的CS域信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_CS_DOMAIN_INFO_STRU* NAS_MML_GetCsDomainInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetCsRegStatus
 功能描述  : 更新CS域的注册结果
 输入参数  : enCsRegStatus:CS域需要更新的注册结果
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月23日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetCsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enCsRegStatus
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.enCsRegStatus = enCsRegStatus;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCsRegStatus
 功能描述  : 获取CS域的注册结果
 输入参数  : 无
 输出参数  : 无
 返 回 值  : CS域的注册结果
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月23日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetCsRegStatus( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.enCsRegStatus);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetCsLastSuccLai
 功能描述  : 获取MML中保存的CS域最后一次注册成功的LAI信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的CS域最后一次注册成功的LAI信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_LAI_STRU* NAS_MML_GetCsLastSuccLai( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stLastSuccLai)   ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetPsLastSuccRai
 功能描述  : 获取MML中保存的PS域最后一次注册成功的RAI信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的PS域最后一次注册成功的RAI信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_RAI_STRU* NAS_MML_GetPsLastSuccRai( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stLastSuccRai)   ;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetPsDomainInfo
 功能描述  : 获取MML中保存的PS域信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的PS域信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_PS_DOMAIN_INFO_STRU* NAS_MML_GetPsDomainInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetPsRegStatus
 功能描述  : 获取PS域的注册结果
 输入参数  : 无
 输出参数  : 无
 返 回 值  : PS域的注册结果
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月23日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_REG_STATUS_ENUM_UINT8  NAS_MML_GetPsRegStatus( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.enPsRegStatus);
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetPsRegStatus
 功能描述  : 设置PS域的注册结果
 输入参数  : enPsRegStatus:PS域的注册结果
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月23日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetPsRegStatus(
    NAS_MML_REG_STATUS_ENUM_UINT8       enPsRegStatus
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.enPsRegStatus = enPsRegStatus;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetNetwork3GppRel
 功能描述  : 获取MML中保存的当前网络的协议版本
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前网络的协议版本
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_NETWORK_3GPP_REL_STRU* NAS_MML_GetNetwork3GppRel( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stNetwork3GppRel)  ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetEquPlmnList
 功能描述  : 获取MML中保存的当前EQU PLMN信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前EQU PLMN信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_EQUPLMN_INFO_STRU* NAS_MML_GetEquPlmnList( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stEquPlmnInfo);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetConnStatus
 功能描述  : 获取MML中保存的当前连接状态和业务状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的当前连接状态和业务状态
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
NAS_MML_CONN_STATUS_INFO_STRU* NAS_MML_GetConnStatus(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus)  ;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetRrcConnStatusFlg
 功能描述  : 设置RRC的连接状态
 输入参数  : ucRrcStatusFlg:RRC链接存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetRrcConnStatusFlg(
    VOS_UINT8                           ucRrcStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucRrcStatusFlg
        = ucRrcStatusFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetCsSigConnStatusFlg
 功能描述  : 设置CS域的信令连接状态
 输入参数  : ucCsSigConnStatusFlg:CS域的信令链接存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetCsSigConnStatusFlg(
    VOS_UINT8                           ucCsSigConnStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsSigConnStatusFlg
        = ucCsSigConnStatusFlg;
}


VOS_UINT8 NAS_MML_GetCsSigConnStatusFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsSigConnStatusFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetPsSigConnStatusFlg
 功能描述  : 设置PS域的信令连接状态
 输入参数  : ucPsSigConnStatusFlg:PS域的信令链接存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetPsSigConnStatusFlg(
    VOS_UINT8                           ucPsSigConnStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucPsSigConnStatusFlg
        = ucPsSigConnStatusFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetPsSigConnStatusFlg
 功能描述  : 设置PS域的TBF存在状态
 输入参数  : ucPsTbfStatusFlg:PS域的TBF链接存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetPsTbfStatusFlg(
    VOS_UINT8                           ucPsTbfStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucPsTbfStatusFlg
        = ucPsTbfStatusFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MMG_GetPsTbfStatusFlg
 功能描述  : 获取PS域的TBF存在状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 返回PS域的TBF存在状态
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8 NAS_MML_GetPsTbfStatusFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucPsTbfStatusFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetEpsSigConnStatusFlg
 功能描述  : 设置EPS域的信令连接状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetEpsSigConnStatusFlg(
    VOS_UINT8                           ucEpsSigConnStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucEpsSigConnStatusFlg
        = ucEpsSigConnStatusFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetEpsServiceConnStatusFlg
 功能描述  : 设置EPS域的数据连接状态
 输入参数  : ucEpsServiceConnStatusFlg:EPS的链接存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetEpsServiceConnStatusFlg(
    VOS_UINT8                           ucEpsServiceConnStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucEpsServiceConnStatusFlg
        = ucEpsServiceConnStatusFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_SetCsServiceConnStatusFlg
 功能描述  : 设置CS域的业务存在状态
 输入参数  : ucCsServiceConnStatusFlg:CS域的业务存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetCsServiceConnStatusFlg(
    VOS_UINT8                           ucCsServiceConnStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsServiceConnStatusFlg
        = ucCsServiceConnStatusFlg;
}


VOS_UINT8 NAS_MML_GetCsServiceConnStatusFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsServiceConnStatusFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetCsServiceBufferStatusFlg
 功能描述  : 设置CS域的缓存业务存在状态
 输入参数  : ucCsServiceConnStatusFlg:CS域的缓存业务存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetCsServiceBufferStatusFlg(
    VOS_UINT8                           ucCsServiceBufferStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsServiceBufferFlg
        = ucCsServiceBufferStatusFlg;
}



VOS_UINT8 NAS_MML_GetCsServiceBufferStatusFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucCsServiceBufferFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetPsServiceBufferStatusFlg
 功能描述  : 设置PS域的缓存业务存在状态
 输入参数  : ucPsServiceConnStatusFlg:CS域的缓存业务存在状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetPsServiceBufferStatusFlg(
    VOS_UINT8                           ucPsServiceBufferStatusFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucPsServiceBufferFlg
        = ucPsServiceBufferStatusFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_SetCsEmergencyServiceFlg
 功能描述  : 设置CS域的业务是否为紧急呼叫业务
 输入参数  : ucEmergencyServiceFlg:CS域的业务是否为紧急呼叫业务
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_MML_SetCsEmergencyServiceFlg(
    VOS_UINT8                           ucEmergencyServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucEmergencyServiceFlg
        = ucEmergencyServiceFlg;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetCsEmergencyServiceFlg
 功能描述  : 判断CS域的业务
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 是否为紧急呼叫业务
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetCsEmergencyServiceFlg(VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucEmergencyServiceFlg);
}


VOS_VOID NAS_MML_SetPsTcServiceFlg(
    VOS_UINT8                           ucPsTcServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucPsTcServiceFlg
        = ucPsTcServiceFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MMC_GetEHplmnSupportFlg
 功能描述  : 获取MML中保存EHPLMN的是否支持的配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存EHPLMN的是否支持的配置信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8 NAS_MML_GetEHplmnSupportFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stHplmnCfg.ucEhplmnSupportFlg);
}



VOS_UINT8 NAS_MML_GetActingHplmnSupportFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stHplmnCfg.ucActingHPlmnSupportFlg);
}


VOS_VOID NAS_MML_SetActingHplmnSupportFlg(VOS_UINT8 ucActingHPlmnSupportFlg)
{
   NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stHplmnCfg.ucActingHPlmnSupportFlg  = ucActingHPlmnSupportFlg;
}



/*****************************************************************************
 函 数 名  : NAS_MML_GetCsAttachAllowFlg
 功能描述  : 获取MML中保存的CS域是否允许ATTACH的标志
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的CS域是否允许ATTACH的标志
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetCsAttachAllowFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.ucCsAttachAllow);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetPsAttachAllowFlg
 功能描述  : 获取MML中保存的PS域是否允许ATTACH的标志
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML中保存的PS域是否允许ATTACH的标志
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 NAS_MML_GetPsAttachAllowFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ucPsAttachAllow);
}

/*****************************************************************************
 函 数 名  : NAS_MMC_SetCsAttachAllowFlg
 功能描述  : 更新MML中保存的CS域是否允许ATTACH的标志
 输入参数  : ucCsAttachAllowFlg:CS域是否允许ATTACH的标志
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_VOID NAS_MML_SetCsAttachAllowFlg(
    VOS_UINT8                           ucCsAttachAllowFlg
)
{
    (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.ucCsAttachAllow) = ucCsAttachAllowFlg;
}


VOS_UINT8 NAS_MML_GetPsLocalDetachFlag( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ucPsLocalDetachFlag);
}


VOS_VOID NAS_MML_SetPsLocalDetachFlag(VOS_UINT8 ucPsLocalDetachFlag )
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ucPsLocalDetachFlag = ucPsLocalDetachFlag;
}


/*****************************************************************************
 函 数 名  : NAS_MMC_SetPsAttachAllowFlg
 功能描述  : 更新MML中保存的PS域是否允许ATTACH的标志
 输入参数  : ucPsAttachAllowFlg:PS域是否允许ATTACH的标志
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_VOID NAS_MML_SetPsAttachAllowFlg(
    VOS_UINT8                           ucPsAttachAllowFlg
)
{
    (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ucPsAttachAllow) = ucPsAttachAllowFlg;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetOperatorNameLong
 功能描述  : 获取保存的运营商的长名称
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 保存的运营商的长名称
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8 *NAS_MML_GetOperatorNameLong(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stOperatorNameInfo.aucOperatorNameLong);
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetOperatorNameShort
 功能描述  : 获取保存的运营商的短名称
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 保存的运营商的短名称
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT8 *NAS_MML_GetOperatorNameShort(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stOperatorNameInfo.aucOperatorNameShort);
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetOperatorNameInfo
 功能描述  : 获取保存的运营商的相关信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 保存的运营商的相关信息
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年7月8日
   作    者   : zhoujun 40661
   修改内容   : 新生成函数

*****************************************************************************/

NAS_MML_OPERATOR_NAME_INFO_STRU* NAS_MML_GetOperatorNameInfo(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stOperatorNameInfo);
}


VOS_UINT8 NAS_MML_GetPsRestrictRegisterFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictRegister);
}


VOS_UINT8 NAS_MML_GetPsRestrictPagingFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictPagingRsp);
}


VOS_UINT8 NAS_MML_GetPsRestrictNormalServiceFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictNormalService);
}



VOS_UINT8 NAS_MML_GetPsRestrictEmergencyServiceFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictEmergencyService);
}



VOS_UINT8 NAS_MML_GetCsRestrictRegisterFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictRegister);
}


VOS_UINT8 NAS_MML_GetCsRestrictPagingFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictPagingRsp);
}


NAS_MML_ACCESS_RESTRICTION_STRU* NAS_MML_GetCsAcRestrictionInfo(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction);
}


NAS_MML_ACCESS_RESTRICTION_STRU* NAS_MML_GetPsAcRestrictionInfo(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction);
}



VOS_UINT8 NAS_MML_GetCsRestrictNormalServiceFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictNormalService);
}


VOS_UINT8 NAS_MML_GetCsRestrictEmergencyServiceFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictEmergencyService);
}




VOS_VOID NAS_MML_SetCsRestrictRegisterFlg(
    VOS_UINT8                           ucCsRestrictionRegisterFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictRegister = ucCsRestrictionRegisterFlg;
}


VOS_VOID NAS_MML_SetCsRestrictPagingFlg(
    VOS_UINT8                           ucCsRestrictionPagingFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictPagingRsp = ucCsRestrictionPagingFlg;
}


VOS_VOID NAS_MML_SetCsRestrictNormalServiceFlg(
    VOS_UINT8                           ucCsRestrictiNormalServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictNormalService = ucCsRestrictiNormalServiceFlg;
}


VOS_VOID NAS_MML_SetCsRestrictEmergencyServiceFlg(
    VOS_UINT8                           ucCsRestrictiEmergencyServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.stCsAcRestriction.ucRestrictEmergencyService = ucCsRestrictiEmergencyServiceFlg;
}




VOS_VOID NAS_MML_SetPsRestrictRegisterFlg(
    VOS_UINT8                           ucPsRestrictionRegisterFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictRegister = ucPsRestrictionRegisterFlg;
}

VOS_VOID NAS_MML_SetPsRestrictPagingFlg(
    VOS_UINT8                           ucPsRestrictionPagingFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictPagingRsp = ucPsRestrictionPagingFlg;
}


VOS_VOID NAS_MML_SetPsRestrictNormalServiceFlg(
    VOS_UINT8                           ucPsRestrictNormalServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictNormalService = ucPsRestrictNormalServiceFlg;
}



VOS_VOID NAS_MML_SetPsRestrictEmergencyServiceFlg(
    VOS_UINT8                           ucPsRestrictiEmergencyServiceFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsAcRestriction.ucRestrictEmergencyService = ucPsRestrictiEmergencyServiceFlg;
}



VOS_VOID NAS_MML_ClearCsPsRestrictionAll()
{
    /* 清除CS接入受限信息 */
    NAS_MML_SetCsRestrictRegisterFlg(VOS_FALSE);
    NAS_MML_SetCsRestrictPagingFlg(VOS_FALSE);
    NAS_MML_SetCsRestrictNormalServiceFlg(VOS_FALSE);
    NAS_MML_SetCsRestrictEmergencyServiceFlg(VOS_FALSE);

    /* 清除PS接入受限信息 */
    NAS_MML_SetPsRestrictRegisterFlg(VOS_FALSE);
    NAS_MML_SetPsRestrictPagingFlg(VOS_FALSE);
    NAS_MML_SetPsRestrictNormalServiceFlg(VOS_FALSE);
    NAS_MML_SetPsRestrictEmergencyServiceFlg(VOS_FALSE);
}


VOS_UINT32 NAS_MML_GetPsRegisterBarToUnBarFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ulPsRegisterBarToUnBarFlag);
}



VOS_VOID NAS_MML_SetPsRegisterBarToUnBarFlg(
    VOS_UINT32                           ulPsUnBarFlag
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.ulPsRegisterBarToUnBarFlag = ulPsUnBarFlag;
}


VOS_UINT32 NAS_MML_GetCsRegisterBarToUnBarFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.ulCsRegisterBarToUnBarFlag);
}


VOS_VOID NAS_MML_SetCsRegisterBarToUnBarFlg(
    VOS_UINT32                           ulCsUnBarFlag
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCsDomainInfo.ulCsRegisterBarToUnBarFlag = ulCsUnBarFlag;
}


NAS_MML_TIN_TYPE_ENUM_UINT8 NAS_MML_GetTinType( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRplmnCfg.enTinType;
}


NAS_MML_UE_NETWORK_CAPABILITY_STRU* NAS_MML_GetUeNetWorkCapability(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.stUeNetworkCapbility);
}


VOS_VOID NAS_MML_SetUeNetWorkCapability(NAS_MML_UE_NETWORK_CAPABILITY_STRU *pstUeNetworkCapbility)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.stUeNetworkCapbility = *pstUeNetworkCapbility;
}


NAS_MML_PLATFORM_RAT_CAP_STRU* NAS_MML_GetPlatformRatCap(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.stPlatformRatCap);
}



VOS_VOID NAS_MML_SetRoamCapability(
    NAS_MML_ROAM_CAPABILITY_ENUM_UINT8  enRoamCapability
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stRoamCfg.enRoamCapability
                                        = enRoamCapability;
}





VOS_VOID NAS_MML_SetMsSysCfgBand(
    NAS_MML_MS_BAND_INFO_STRU          *pstMsBand
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.stMsBand = *pstMsBand;
}



VOS_VOID NAS_MML_SetMsSysCfgPrioRatList(
    NAS_MML_PLMN_RAT_PRIO_STRU         *pstPrioRatList
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.stPrioRatList = *pstPrioRatList;
}




VOS_VOID NAS_MML_SaveEquPlmnList (
    VOS_UINT32                          ulPlmnNum,
    NAS_MML_PLMN_ID_STRU               *pstNewEquPlmnList
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulEquPlmnNum;
    NAS_MML_EQUPLMN_INFO_STRU          *pstEplmnList;
    NAS_MML_PLMN_ID_STRU               *pstPlmnId;

    /* 先添加当前驻留的plmn信息 */
    pstPlmnId    = NAS_MML_GetCurrCampPlmnId();
    pstEplmnList = NAS_MML_GetEquPlmnList();
    pstEplmnList->astEquPlmnAddr[0].ulMcc = pstPlmnId->ulMcc;
    pstEplmnList->astEquPlmnAddr[0].ulMnc = pstPlmnId->ulMnc;


    /* 超过当前最大Eplmn容量时，使用最大容量,需要去除RPLMN个数 */
    if (ulPlmnNum > NAS_MML_MAX_EQUPLMN_NUM - 1)
    {
        ulEquPlmnNum = NAS_MML_MAX_EQUPLMN_NUM - 1;
    }
    else
    {
        ulEquPlmnNum = ulPlmnNum;
    }


    /* EPLMN信息从第一个位置开始记录 */
    for ( i = 0; i < ulEquPlmnNum; i++ )
    {
        pstEplmnList->astEquPlmnAddr[1 + i].ulMcc = pstNewEquPlmnList[i].ulMcc;
        pstEplmnList->astEquPlmnAddr[1 + i].ulMnc = pstNewEquPlmnList[i].ulMnc;
    }

    ulEquPlmnNum ++;

    /* 1.紧急PDN连接存在时，不删除EPLMN列表中的forbidden plmn列表，待释放时删除：
         if there is no PDN connection for emergency bearer services established, the UE shall remove from
         the list any PLMN code that is already in the list of "forbidden PLMNs" or in the list of "forbidden
         PLMNs for GPRS service". If there is a PDN connection for emergency bearer services established, the
         UE shall remove from the list of equivalent PLMNs any PLMN code present in the list of forbidden PLMNs
         or in the list of "forbidden PLMNs for GPRS service" when the PDN connection for emergency bearer services
         is released.
       2.紧急注册时，不能删除EPLMN列表中的forbidden plmn列表(目前不支持)：
         and if the attach procedure is not for emergency bearer services, the UE shall remove from the list any
         PLMN code that is already in the list of "forbidden PLMNs" or in the list of "forbidden PLMNs for GPRS 
         service". */       
    if (VOS_FALSE == NAS_MML_GetEmcPdpStatusFlg())
    {
        /*从EPLMN列表中删除无效 、禁止和不允许漫游的网络*/
        ulEquPlmnNum = NAS_MML_DelInvalidPlmnFromList(ulEquPlmnNum, pstEplmnList->astEquPlmnAddr);
        ulEquPlmnNum = NAS_MML_DelForbPlmnInList(ulEquPlmnNum, pstEplmnList->astEquPlmnAddr);
    }
    
    pstEplmnList->ucEquPlmnNum = (VOS_UINT8)ulEquPlmnNum;

    NAS_MML_SetEplmnValidFlg(VOS_TRUE);

    return;
}
VOS_VOID NAS_MML_UpdateGURplmn (
    NAS_MML_PLMN_ID_STRU               *pstRPlmnId,
    NAS_MML_NET_RAT_TYPE_ENUM_UINT8     enCurrRat
)
{
    NAS_MML_RPLMN_CFG_INFO_STRU        *pstRplmnCfgInfo = VOS_NULL_PTR;

    /* 用于获取RPLMN的定制需求信息 */
    pstRplmnCfgInfo = NAS_MML_GetRplmnCfg();

    /* 更新全局变量中的LastRPLMN的接入技术 */
    if (enCurrRat != NAS_MML_GetLastRplmnRat())
    {
        NAS_MML_SetLastRplmnRat(enCurrRat);
    }

    /* 更新全局变量中的RPLMN */
    if (VOS_FALSE == pstRplmnCfgInfo->ucMultiRATRplmnFlg)
    {
        /* 不支持双RPLMN，则W和G的RPLMN都更新 */
        pstRplmnCfgInfo->stGRplmnInNV = *pstRPlmnId;
        pstRplmnCfgInfo->stWRplmnInNV = *pstRPlmnId;

    }
    else
    {
        /* 支持双RPLMN, 则只更新对应接入技术的RPLMN */
        if (NAS_MML_NET_RAT_TYPE_GSM == enCurrRat)
        {
            pstRplmnCfgInfo->stGRplmnInNV = *pstRPlmnId;
        }
        else if (NAS_MML_NET_RAT_TYPE_WCDMA == enCurrRat)
        {
            pstRplmnCfgInfo->stWRplmnInNV = *pstRPlmnId;
        }
        else
        {
            ;
        }
    }

    return;
}


/*****************************************************************************
 函 数 名  : NAS_MML_GetSimCsLociFileContent
 功能描述  : 获取MML内存中USIM模块EFLOCI文件的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML内存中USIM模块EFLOCI文件的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月1日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetSimCsLociFileContent( VOS_VOID )
{
    NAS_MML_SIM_INFO_STRU               *pstSimInfo;

    pstSimInfo  = NAS_MML_GetSimInfo();

    return pstSimInfo->stSimNativeContent.aucCsLociInfoFile;
}

/*****************************************************************************
 函 数 名  : NAS_MML_GetSimPsLociFileContent
 功能描述  : 获取MML内存中USIM模块EFPSLOCI文件的内容
 输入参数  : 无
 输出参数  : 无
 返 回 值  : MML内存中USIM模块EFPSLOCI文件的内容
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月1日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8* NAS_MML_GetSimPsLociFileContent( VOS_VOID )
{
    NAS_MML_SIM_INFO_STRU               *pstSimInfo;

    pstSimInfo  = NAS_MML_GetSimInfo();

    return pstSimInfo->stSimNativeContent.aucPsLociInfoFile;
}

/*****************************************************************************
 函 数 名  : NAS_MML_UpdateSimCsLociFileContent
 功能描述  : 更新MML内存中USIM模块EFLOCI文件的内容
 输入参数  : pucCsLociFileContent:待更新的EFCSLOCI文件的内容
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月1日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_UpdateSimCsLociFileContent(
    VOS_UINT8                          *pucCsLociFileContent
)
{
    NAS_MML_SIM_INFO_STRU               *pstSimInfo;

    pstSimInfo  = NAS_MML_GetSimInfo();

    PS_MEM_CPY(pstSimInfo->stSimNativeContent.aucCsLociInfoFile,
               pucCsLociFileContent,
               NAS_MML_CS_LOCI_SIM_FILE_LEN);
}

/*****************************************************************************
 函 数 名  : NAS_MML_UpdateSimPsLociFileContent
 功能描述  : 更新MML内存中USIM模块EFPSLOCI文件的内容
 输入参数  : pucPsLociFileContent:待更新的EFPSLOCI文件的内容
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月1日
   作    者   : zhoujun /40661
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_MML_UpdateSimPsLociFileContent(
    VOS_UINT8                          *pucPsLociFileContent
)
{
    NAS_MML_SIM_INFO_STRU               *pstSimInfo;

    pstSimInfo  = NAS_MML_GetSimInfo();

    PS_MEM_CPY(pstSimInfo->stSimNativeContent.aucPsLociInfoFile,
               pucPsLociFileContent,
               NAS_MML_PS_LOCI_SIM_FILE_LEN);
}





VOS_UINT8 NAS_MML_GetUePcRecurFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMaintainInfo.stUeMaintainInfo.ucUeSndPcRecurFlg);
}


VOS_VOID NAS_MML_SetUePcRecurFlg(
    VOS_UINT8                           ucUePcRecurFlg
)
{
    NAS_MML_GetMmlCtx()->stMaintainInfo.stUeMaintainInfo.ucUeSndPcRecurFlg = ucUePcRecurFlg;
}


VOS_UINT8 NAS_MML_GetOmConnectFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMaintainInfo.stOmMaintainInfo.ucOmConnectFlg);
}


VOS_VOID NAS_MML_SetOmConnectFlg(
    VOS_UINT8                           ucOmConnectFlg
)
{
    NAS_MML_GetMmlCtx()->stMaintainInfo.stOmMaintainInfo.ucOmConnectFlg = ucOmConnectFlg;
}


VOS_UINT8 NAS_MML_GetOmPcRecurEnableFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMaintainInfo.stOmMaintainInfo.ucOmPcRecurEnableFlg);
}


VOS_VOID NAS_MML_SetOmPcRecurEnableFlg(
    VOS_UINT8                           ucOmPcRecurEnableFlg
)
{
    NAS_MML_GetMmlCtx()->stMaintainInfo.stOmMaintainInfo.ucOmPcRecurEnableFlg = ucOmPcRecurEnableFlg;
}



VOS_UINT8 NAS_MML_GetScanCtrlEnableFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.ucScanCtrlFlg);
}


VOS_VOID NAS_MML_SetScanCtrlEnableFlg(VOS_UINT8 ucScanCtrlEnableFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.ucScanCtrlFlg = ucScanCtrlEnableFlg;
}


VOS_VOID NAS_MML_SetHPlmnType(
    NAS_MML_HPLMN_TYPE_ENUM_UINT8       enHPlmnType
)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stEhPlmnInfo.enHplmnType = enHPlmnType;
}


NAS_MML_HPLMN_TYPE_ENUM_UINT8 NAS_MML_GetHPlmnType(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stEhPlmnInfo.enHplmnType);
}


VOS_UINT8  NAS_MML_GetSingleDomainRegFailActionSupportFlag(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.stSingleDomainFailActionCtx.ucActiveFlag);
}


NAS_MML_SINGLE_DOMAIN_FAIL_ACTION_LIST_CTX_STRU*  NAS_MML_GetSingleDomainFailActionCtx(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.stSingleDomainFailActionCtx);
}


NAS_MML_SOR_ADDITIONAL_LAU_STRU*  NAS_MML_GetSorAdditionalLauCtx(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.stAdditionalLau);
}


VOS_VOID NAS_MML_InitSorAdditionalLauCtx(VOS_VOID)
{
    NAS_MML_SOR_ADDITIONAL_LAU_STRU    *pstAdditionalLau    = VOS_NULL_PTR;

    pstAdditionalLau = NAS_MML_GetSorAdditionalLauCtx();

    pstAdditionalLau->ucAdditionalLauFlag       = VOS_FALSE;
    pstAdditionalLau->stLai.stPlmnId.ulMcc      = NAS_MML_INVALID_MCC;
    pstAdditionalLau->stLai.stPlmnId.ulMnc      = NAS_MML_INVALID_MNC;
    pstAdditionalLau->stLai.aucLac[0]           = NAS_MML_LAC_LOW_BYTE_INVALID;
    pstAdditionalLau->stLai.aucLac[1]           = NAS_MML_LAC_HIGH_BYTE_INVALID;
    pstAdditionalLau->stLai.enCampPlmnNetRat    = NAS_MML_NET_RAT_TYPE_BUTT;
    pstAdditionalLau->stLai.ucRac               = NAS_MML_RAC_INVALID;

    PS_MEM_SET(pstAdditionalLau->auReserv, 0X00, sizeof(pstAdditionalLau->auReserv)); 
    
    return ;
}


VOS_UINT8 NAS_MML_GetSorAdditionalLauFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.stAdditionalLau.ucAdditionalLauFlag);
}


VOS_VOID NAS_MML_SetBgSearchRegardlessMccList(
    VOS_UINT32                         *pulMcc,
    VOS_UINT8                           ucMccNum
)
{
    VOS_UINT32                          i;

    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.stBgSearchRegardlessMcc.ucCustomMccNum = ucMccNum;

    for (i = 0; i < ucMccNum; i++)
    {
        NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.stBgSearchRegardlessMcc.aulCostumMccList[i] = pulMcc[i];
    }
}


NAS_MMC_HPLMN_SEARCH_REGARDLESS_MCC_CTX_STRU*  NAS_MML_GetBgSearchRegardlessMccCtx(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stBgSearchCfg.stBgSearchRegardlessMcc);
}



VOS_UINT8  NAS_MML_GetRegFailNetWorkFailureCustomFlag(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucRegFailNetWorkFailureCustomFlg);
}


VOS_VOID  NAS_MML_SetRegFailNetWorkFailureCustomFlag(VOS_UINT8 ucRegFailNetWorkFailureCustomFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stMiscellaneousCfg.ucRegFailNetWorkFailureCustomFlg = ucRegFailNetWorkFailureCustomFlg;
}



NAS_MML_PLMN_ID_STRU  NAS_MML_GetImsiHomePlmn(
    VOS_UINT8                          *pucImsi
)
{
    NAS_MML_PLMN_ID_STRU                stHplmn;

    VOS_UINT8                           ucUsimMncLenth;
    VOS_UINT8                           ucExactlyComparaFlag;

    ucUsimMncLenth       = NAS_MML_GetUsimMncLen();
    ucExactlyComparaFlag = NAS_MML_GetPlmnExactlyComparaFlg();

    PS_MEM_SET(&stHplmn, 0, sizeof(NAS_MML_PLMN_ID_STRU));

    stHplmn.ulMcc |= (pucImsi[1] & NAS_MML_OCTET_HIGH_FOUR_BITS) >> NAS_MML_OCTET_MOVE_FOUR_BITS;
    stHplmn.ulMcc |= (pucImsi[2] & NAS_MML_OCTET_LOW_FOUR_BITS) << NAS_MML_OCTET_MOVE_EIGHT_BITS;
    stHplmn.ulMcc |= (pucImsi[2] & NAS_MML_OCTET_HIGH_FOUR_BITS) << NAS_MML_OCTET_MOVE_TWELVE_BITS;
    stHplmn.ulMnc |= (pucImsi[3] & NAS_MML_OCTET_LOW_FOUR_BITS);
    stHplmn.ulMnc |= (pucImsi[3] & NAS_MML_OCTET_HIGH_FOUR_BITS) << NAS_MML_OCTET_MOVE_FOUR_BITS;

    if ((NAS_MML_MNC_LENGTH_TWO_BYTES_IN_IMSI == ucUsimMncLenth)
     && (VOS_TRUE == ucExactlyComparaFlag))
    {
        stHplmn.ulMnc |= (NAS_MML_LOW_BYTE_INVALID) << NAS_MML_OCTET_MOVE_SIXTEEN_BITS;
    }
    else
    {
        stHplmn.ulMnc |= (pucImsi[4] & NAS_MML_OCTET_LOW_FOUR_BITS) << NAS_MML_OCTET_MOVE_SIXTEEN_BITS;
    }

    /* 对于PLMN ID相同的比较，末位为0或者F的情况，已经在新的比较函数中
      处理:NAS_MMC_CompareBcchPlmnwithSimPlmn不需要添加两次PLMN ID，对于
      中移动卡的特殊处理需要保留,添加2位MNC的HPLMN到Hplmn列表中,
      移动MCC=460,MNC=02和07中国移动卡特殊处理
    */
    if (( 0x000604 == stHplmn.ulMcc )
     && ( (0x200   == (stHplmn.ulMnc & 0xFFFF ) )
       || (0x700   == (stHplmn.ulMnc & 0xFFFF))))
    {
        stHplmn.ulMnc &= 0xFF0000;
    }

    return stHplmn;
}




NAS_MML_EHPLMN_PRESENT_IND_ENUM_UINT8  NAS_MML_GetEHplmnPresentInd( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.stEhPlmnInfo.enEHplmnPresentInd;
}



VOS_VOID  NAS_MML_SetPsRegContainDrx(
    NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8 enPsRegContainDrx)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.enPsRegisterContainDrx = enPsRegContainDrx;
}


NAS_MML_PS_REG_CONTAIN_DRX_PARA_ENUM_UINT8 NAS_MML_GetPsRegContainDrx(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.enPsRegisterContainDrx;
}


VOS_VOID  NAS_MML_SetNonDrxTimer(VOS_UINT8  ucPsNonDrxTimerLength)
{
    /* 取值范围0-7，如果大于7则认为无效，取默认值0 */
    if (NAS_MML_NON_DRX_TIMER_MAX < ucPsNonDrxTimerLength)
    {
        ucPsNonDrxTimerLength = NAS_MML_PS_DEFAULT_NON_DRX_TIMER;
    }

    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucNonDrxTimer = ucPsNonDrxTimerLength;
}
VOS_VOID NAS_MML_SetSplitPgCycleCode(
    VOS_UINT8                           ucSplitPgCycleCode
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucSplitPgCycleCode = ucSplitPgCycleCode;
}


VOS_UINT8 NAS_MML_GetNonDrxTimer(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucNonDrxTimer);
}


VOS_VOID  NAS_MML_SetUeUtranPsDrxLen(VOS_UINT8  ucUeUtranPsDrxLen)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucUeUtranPsDrxLen = ucUeUtranPsDrxLen;
}


VOS_UINT8 NAS_MML_GetUeUtranPsDrxLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucUeUtranPsDrxLen);
}


VOS_VOID  NAS_MML_SetUeEutranPsDrxLen(VOS_UINT8  ucUeEutranPsDrxLen)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucUeEutranPsDrxLen = ucUeEutranPsDrxLen;
}


VOS_UINT8 NAS_MML_GetUeEutranPsDrxLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucUeEutranPsDrxLen);
}


VOS_VOID  NAS_MML_SetWSysInfoDrxLen(VOS_UINT8  ucWSysInfoDrxLength)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucWSysInfoDrxLen = ucWSysInfoDrxLength;
}


VOS_UINT8 NAS_MML_GetWSysInfoDrxLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucWSysInfoDrxLen);
}


VOS_VOID  NAS_MML_SetLSysInfoDrxLen(VOS_UINT8  ucLSysInfoDrxLength)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucLSysInfoDrxLen = ucLSysInfoDrxLength;
}


VOS_UINT8 NAS_MML_GetLSysInfoDrxLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucLSysInfoDrxLen);
}

VOS_UINT8 NAS_MML_GetSplitPgCycleCode(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucSplitPgCycleCode);
}


VOS_UINT8 NAS_MML_GetSplitOnCcch(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucSplitOnCcch);
}


VOS_VOID NAS_MML_SetSplitOnCcch(VOS_UINT8 ucSplitOnCcch)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.stPsDomainDrxPara.ucSplitOnCcch = ucSplitOnCcch;
}



VOS_VOID  NAS_MML_SetPlmnExactlyComparaFlg(VOS_UINT8  ucPlmnExactlyComparaFlag)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucPlmnExactlyCompareFlg = ucPlmnExactlyComparaFlag;
}


VOS_VOID  NAS_MML_SetHplmnRegisterCtrlFlg(VOS_UINT8  ucHplmnRegisterFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucHplmnRegisterCtrlFlg = ucHplmnRegisterFlg;
}



VOS_UINT8 NAS_MML_GetHplmnRegisterCtrlFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucHplmnRegisterCtrlFlg);
}



VOS_VOID  NAS_MML_SetH3gCtrlFlg(VOS_UINT8  ucH3gFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucH3gCtrlFlg = ucH3gFlg;
}



VOS_UINT8 NAS_MML_GetH3gCtrlFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucH3gCtrlFlg);
}



VOS_UINT8 NAS_MML_GetPlmnExactlyComparaFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucPlmnExactlyCompareFlg);
}



VOS_VOID  NAS_MML_SetUsimMncLen(VOS_UINT8  ucUsimMncLength)
{
    NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.ucUsimMncLen = ucUsimMncLength;
}


VOS_UINT8 NAS_MML_GetUsimMncLen(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stSimInfo.stSimPlmnInfo.ucUsimMncLen);
}


VOS_VOID  NAS_MML_SetSupportCsServiceFLg(VOS_UINT8  ucCsServiceFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSupportCsServiceFlg = ucCsServiceFlg;
}


VOS_UINT8  NAS_MML_GetSupportCsServiceFLg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSupportCsServiceFlg;
}



NAS_MML_USER_CFG_EHPLMN_INFO_STRU* NAS_MML_GetUserCfgEhplmnInfo( VOS_VOID )
{
    NAS_MML_HPLMN_CFG_INFO_STRU         *pstHplmnCfg;

    pstHplmnCfg = NAS_MML_GetHplmnCfg();

    return &(pstHplmnCfg->stUserCfgEhplmnInfo);
}





VOS_VOID NAS_MML_SetEplmnValidFlg(
    VOS_UINT8                           ucEplmnValidFlg
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEquPlmnInfo.ucValidFlg = ucEplmnValidFlg;
}


VOS_UINT8 NAS_MML_GetEplmnValidFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stNetworkInfo.stEquPlmnInfo.ucValidFlg);
}



VOS_UINT8 NAS_MML_GetIsrSupportFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsrSupportFlg);
}


VOS_VOID NAS_MML_SetIsrSupportFlg(
    VOS_UINT8                           ucIsrSupport
)
{
    (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsrSupportFlg) = ucIsrSupport;
}




#if (FEATURE_ON == FEATURE_PTM)

OM_RING_ID NAS_MML_GetErrLogRingBufAddr(VOS_VOID)
{
    return g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.pstRingBuffer;
}


VOS_VOID NAS_MML_SetErrLogRingBufAddr(OM_RING_ID pRingBuffer)
{
    g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.pstRingBuffer = pRingBuffer;

    return;
}



VOS_UINT8 NAS_MML_GetErrlogCtrlFlag(VOS_VOID)
{
    return g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.ucErrLogCtrlFlag;
}


VOS_VOID NAS_MML_SetErrlogCtrlFlag(VOS_UINT8 ucFlag)
{
    g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.ucErrLogCtrlFlag = ucFlag;

    return;
}


VOS_UINT16 NAS_MML_GetErrlogAlmLevel(VOS_VOID)
{
    return g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.usAlmLevel;
}


VOS_VOID NAS_MML_SetErrlogAlmLevel(VOS_UINT16 usAlmLevel)
{
    g_stNasMmlCtx.stMaintainInfo.stErrLogMntnInfo.usAlmLevel = usAlmLevel;

    return;
}


VOS_UINT8 NAS_MML_GetFtmCtrlFlag(VOS_VOID)
{
    return g_stNasMmlCtx.stMaintainInfo.stFtmMntnInfo.ucFtmCtrlFlag;
}



VOS_VOID NAS_MML_SetFtmCtrlFlag(VOS_UINT8 ucFlag)
{
    g_stNasMmlCtx.stMaintainInfo.stFtmMntnInfo.ucFtmCtrlFlag = ucFlag;

    return;
}


VOS_VOID NAS_MML_InitErrLogMntnInfo(
    NAS_MML_ERRLOG_MNTN_INFO_STRU          *pstErrLogInfo
)
{
    VOS_CHAR                               *pbuffer;
    OM_RING_ID                              pRingbuffer;

    pstErrLogInfo->ucErrLogCtrlFlag         = VOS_FALSE;
    pstErrLogInfo->usAlmLevel               = NAS_ERR_LOG_CTRL_LEVEL_CRITICAL;

    /* 申请cache的动态内存 , 长度加1是因为读和写指针之间在写满时会相差一个字节 */
    pbuffer = (char *)PS_MEM_ALLOC(WUEPS_PID_MMC ,NAS_MML_RING_BUFFER_SIZE + 1);
    if (VOS_NULL_PTR == pbuffer)
    {
        pstErrLogInfo->pstRingBuffer = VOS_NULL_PTR;
        return;
    }

    /* 调用OM的接口，将申请的动态内存创建为RING BUFFER */
    pRingbuffer = OM_RingBufferCreateEx(pbuffer, NAS_MML_RING_BUFFER_SIZE + 1);
    if (VOS_NULL_PTR == pRingbuffer)
    {
        PS_MEM_FREE(WUEPS_PID_MMC, pbuffer);
    }

    /* 保存ringbuffer指针 */
    pstErrLogInfo->pstRingBuffer = pRingbuffer;

    return;

}
VOS_UINT32 NAS_MML_PutErrLogRingBuf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
)
{
    VOS_UINT32                          ulFreeSize;
    OM_RING_ID                          pTafRingBuffer;

    pTafRingBuffer = NAS_MML_GetErrLogRingBufAddr();
    if (VOS_NULL_PTR == pTafRingBuffer)
    {
        return 0;
    }

    /* 如果写入比RING BUFFER还大则不写入 */
    if (ulbytes > NAS_MML_RING_BUFFER_SIZE)
    {
        return 0;
    }

    /* 获取RING BUFFER剩余空间大小 */
    ulFreeSize = (VOS_UINT32)OM_RingBufferFreeBytes(pTafRingBuffer);

    /* 如果剩余空间不足写入的大小，则清空RING BUFFER */
    if (ulFreeSize < ulbytes)
    {
        OM_RingBufferFlush(pTafRingBuffer);
    }

    /* 写入RING BUFFER */
    return (VOS_UINT32)OM_RingBufferPut(pTafRingBuffer, pbuffer, (VOS_INT)ulbytes);
}


VOS_UINT32 NAS_MML_GetErrLogRingBufContent(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulbytes
)
{
    return (VOS_UINT32)OM_RingBufferGet(NAS_MML_GetErrLogRingBufAddr(), pbuffer, (VOS_INT)ulbytes);
}


VOS_UINT32 NAS_MML_GetErrLogRingBufUseBytes(VOS_VOID)
{
    return (VOS_UINT32)OM_RingBufferNBytes(NAS_MML_GetErrLogRingBufAddr());
}


VOS_VOID NAS_MML_CleanErrLogRingBuf(VOS_VOID)
{
    OM_RingBufferFlush(NAS_MML_GetErrLogRingBufAddr());

    return;
}


VOS_UINT32 NAS_MML_RegFailCauseNeedRecord(NAS_MML_REG_FAIL_CAUSE_ENUM_UINT16 enRegFailCause)
{
    /*以下原因值不需要记录，其余原因需要记录 */
    if ((enRegFailCause == NAS_MML_REG_FAIL_CAUSE_NULL)
     || (enRegFailCause == NAS_MML_REG_FAIL_CAUSE_LOCAL_NULL))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

#endif
VOS_UINT8 NAS_MML_GetDelayedCsfbLauFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ucDelayedCsfbLauFlg);
}


VOS_VOID NAS_MML_SetDelayedCsfbLauFlg(VOS_UINT8 ucDelayedCsfbLauFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsSysCfgInfo.ucDelayedCsfbLauFlg = ucDelayedCsfbLauFlg;
}


VOS_UINT8 NAS_MML_GetIsRauNeedFollowOnCsfbMtFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsRauNeedFollowOnCsfbMtFlg);
}


VOS_VOID NAS_MML_SetIsRauNeedFollowOnCsfbMtFlg(VOS_UINT8 ucIsRauNeedFollowOnCsfbMtFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsRauNeedFollowOnCsfbMtFlg = ucIsRauNeedFollowOnCsfbMtFlg;
}



VOS_UINT8 NAS_MML_GetIsRauNeedFollowOnCsfbMoFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsRauNeedFollowOnCsfbMoFlg);
}


VOS_VOID NAS_MML_SetIsRauNeedFollowOnCsfbMoFlg(VOS_UINT8 ucIsRauNeedFollowOnCsfbMoFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsRauNeedFollowOnCsfbMoFlg = ucIsRauNeedFollowOnCsfbMoFlg;
}



VOS_UINT8 NAS_MML_GetIsDelFddBandSwitchOnFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsDelFddBandSwitchOnFlg);
}


VOS_VOID NAS_MML_SetIsDelFddBandSwitchOnFlg(VOS_UINT8 ucIsDelFddBandSwitchOnFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stLteCustomCfgInfo.ucIsDelFddBandSwitchOnFlg = ucIsDelFddBandSwitchOnFlg;
}




VOS_VOID NAS_MML_SetSupportAccBarPlmnSearchFlg(
    VOS_UINT8                           ucAccBarPlmnSearchFlg
)
{
    (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSupportAccBarPlmnSearchFlg) = ucAccBarPlmnSearchFlg;
}


VOS_UINT8 NAS_MML_GetSupportAccBarPlmnSearchFlg( VOS_VOID )
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucSupportAccBarPlmnSearchFlg);
}


NAS_MML_USER_CFG_OPLMN_INFO_STRU* NAS_MML_GetUserCfgOPlmnInfo( VOS_VOID )
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stUserCfgOPlmnInfo);
}


VOS_VOID NAS_MML_SetUserCfgOPlmnInfo(
    NAS_MML_USER_CFG_OPLMN_INFO_STRU       *pstUserCfgOPlmnInfo
)
{
    PS_MEM_CPY(&(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stUserCfgOPlmnInfo),
               pstUserCfgOPlmnInfo,
               sizeof(NAS_MML_USER_CFG_OPLMN_INFO_STRU));
}


VOS_VOID  NAS_MML_InitUserCfgOPlmnInfo(
    NAS_MML_USER_CFG_OPLMN_INFO_STRU       *pstUserCfgOPlmnInfo
)
{
    /* 初始化特性使能开关为关闭  */
    pstUserCfgOPlmnInfo->ucActiveFlg    = VOS_FALSE;
    pstUserCfgOPlmnInfo->ucImsiCheckFlg = VOS_TRUE;

    /* 初始化OPLMN版本号信息 */
    PS_MEM_SET( pstUserCfgOPlmnInfo->aucVersion,
                (VOS_UINT8)0X00,
                sizeof(pstUserCfgOPlmnInfo->aucVersion));

    /* 初始化OPLMN信息个数为0 */
    pstUserCfgOPlmnInfo->usOplmnListNum    = 0;

    /* 初始化使能IMSI信息个数为0 */
    pstUserCfgOPlmnInfo->ucImsiPlmnListNum= 0;

    /* 初始化IMSI列表 */
    PS_MEM_SET( pstUserCfgOPlmnInfo->astImsiPlmnList,
                (VOS_UINT8)0X00,
                sizeof(pstUserCfgOPlmnInfo->astImsiPlmnList));

    return;
}
VOS_UINT8  NAS_MML_GetRrcUtranNcellExistFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stRrcNcellInfo.ucUtranNcellExist;
}



VOS_VOID  NAS_MML_SetRrcUtranNcellExistFlg( 
    VOS_UINT8                           ucUtranNcellExist
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stRrcNcellInfo.ucUtranNcellExist = ucUtranNcellExist;
}


VOS_UINT8  NAS_MML_GetRrcLteNcellExistFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stRrcNcellInfo.ucLteNcellExist;
}



VOS_VOID  NAS_MML_SetRrcLteNcellExistFlg( 
    VOS_UINT8                           ucLteNcellExist
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.stRrcNcellInfo.ucLteNcellExist = ucLteNcellExist;
}




VOS_VOID  NAS_MML_SetDsdsRfShareFlg(
    VOS_UINT16                          usDsdsRfShareFlg    
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.usDsdsRfShareSupportFlg = usDsdsRfShareFlg;
    
    return;
}


VOS_UINT16 NAS_MML_GetDsdsRfShareFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.usDsdsRfShareSupportFlg;   
}

VOS_VOID    NAS_MML_SetImsNormalRegStatus(
    NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8                enRegStatus
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stImsDomainInfo.enImsNormalRegSta = enRegStatus;

    return;
}


NAS_MML_IMS_NORMAL_REG_STATUS_ENUM_UINT8   NAS_MML_GetImsNormalRegStatus(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stImsDomainInfo.enImsNormalRegSta;
}


VOS_VOID    NAS_MML_SetImsVoiceAvailFlg(
    VOS_UINT8                                               ucAvail
)
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stImsDomainInfo.ucImsVoiceAvail = ucAvail;

    return;
}


VOS_UINT8   NAS_MML_GetImsVoiceAvailFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stImsDomainInfo.ucImsVoiceAvail;
}

VOS_VOID NAS_MML_SetGsmBandCapability(
    VOS_UINT32                          ulBand
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.stPlatformBandCap.ulGsmCapability = ulBand;
}


VOS_UINT32 NAS_MML_GetGsmBandCapability(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stMsCapability.stPlatformBandCap.ulGsmCapability;
}



VOS_VOID NAS_MML_SetEmcPdpStatusFlg( 
    VOS_UINT8                           ucEmcPdpStatusFlg
)
{
    NAS_MML_CONN_STATUS_INFO_STRU      *pstConnStatus = VOS_NULL_PTR;

    pstConnStatus   = NAS_MML_GetConnStatus();
    
    pstConnStatus->ucEmcPdpStatusFlg    = ucEmcPdpStatusFlg;
}



VOS_UINT8 NAS_MML_GetEmcPdpStatusFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stConnStatus.ucEmcPdpStatusFlg;
}


VOS_UINT8 NAS_MML_GetImsVoiceInterSysLauEnableFlg (VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stImsVoiceMM.ucImsVoiceInterSysLauEnable;
}


VOS_VOID NAS_MML_SetImsVoiceInterSysLauEnableFlg( VOS_UINT8 ucImsVoiceInterSysLauEnable )
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stImsVoiceMM.ucImsVoiceInterSysLauEnable = ucImsVoiceInterSysLauEnable;
}


VOS_UINT8 NAS_MML_GetImsVoiceMMEnableFlg(VOS_VOID)
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stImsVoiceMM.ucImsVoiceMMEnable;
}


VOS_VOID NAS_MML_SetImsVoiceMMEnableFlg( VOS_UINT8 ucImsVoiceMMEnable)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stImsVoiceMM.ucImsVoiceMMEnable = ucImsVoiceMMEnable;
}


NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 NAS_MML_GetGUNwImsVoiceSupportFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.enNwImsVoCap;
}


VOS_VOID NAS_MML_SetGUNwImsVoiceSupportFlg( NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 enNwImsVoCap )
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stPsDomainInfo.enNwImsVoCap = enNwImsVoCap;
}


NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 NAS_MML_GetLteNwImsVoiceSupportFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enNwImsVoCap;
}


VOS_VOID NAS_MML_SetLteNwImsVoiceSupportFlg( NAS_MML_NW_IMS_VOICE_CAP_ENUM_UINT8 enNwImsVoCap )
{
    NAS_MML_GetMmlCtx()->stNetworkInfo.stEpsDomainInfo.enNwImsVoCap = enNwImsVoCap;
}

VOS_UINT8 NAS_MML_GetPsSupportFLg(VOS_VOID)
{
    /* 由于LTE网络系统消息中无PS域支持项，默认填写支持 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetMmlCtx()->stNetworkInfo.stCampPlmnInfo.enNetRatType)
    {
        return VOS_TRUE;
    }

    return NAS_MML_GetNetworkInfo()->stPsDomainInfo.ucPsSupportFlg;
}
VOS_VOID NAS_MML_SetLcEnableFlg(VOS_UINT8 ucLcEnableFLg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucLcEnableFLg = ucLcEnableFLg;

    return;
}


VOS_UINT8 NAS_MML_GetLcEnableFlg(VOS_VOID)
{
    return (NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.ucLcEnableFLg);
}



VOS_VOID NAS_MML_SetIgnoreAuthRejFlg(
    VOS_UINT8                           ucIgnoreAuthRejFlg
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stIgnoreAuthRejInfo.ucIgnoreAuthRejFlg
                               = ucIgnoreAuthRejFlg;
}



VOS_VOID NAS_MML_SetMaxAuthRejNo(
    VOS_UINT8                           ucMaxAuthRejNo
)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stIgnoreAuthRejInfo.ucMaxAuthRejNo
                               = ucMaxAuthRejNo;
}



NAS_MML_IGNORE_AUTH_REJ_INFO_STRU * NAS_MML_GetAuthRejInfo(VOS_VOID)
{
    return &(NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.stIgnoreAuthRejInfo);
}




VOS_VOID NAS_MML_InitHplmnAuthRejCounter(
    NAS_MML_IGNORE_AUTH_REJ_INFO_STRU                      *pstAuthRejInfo
)
{
    pstAuthRejInfo->ucMaxAuthRejNo = 0;
    pstAuthRejInfo->ucHplmnCsAuthRejCounter = 0;
    pstAuthRejInfo->ucHplmnPsAuthRejCounter = 0;
    pstAuthRejInfo->ucIgnoreAuthRejFlg = VOS_FALSE;

    return;
}


VOS_VOID  NAS_MML_InitHighPrioRatHplmnTimerCfgInfo(
    NAS_MML_HIGH_PRIO_RAT_HPLMN_TIMER_CFG_STRU             *pstHighHplmnTimerCfg
)
{
    pstHighHplmnTimerCfg->ulNonFirstSearchTimeLen   = TI_NAS_MMC_HIGH_PRIO_RAT_HPLMN_TIMER_NON_FIRST_LEN;
    pstHighHplmnTimerCfg->ulFirstSearchTimeLen      = TI_NAS_MMC_HIGH_RPIO_RAT_HPLMN_TIMER_FIRST_LEN;
    pstHighHplmnTimerCfg->ulFirstSearchTimeCount    = TI_NAS_MMC_HIGH_RPIO_RAT_HPLMN_TIMER_FIRST_SEARCH_COUNT;    
    pstHighHplmnTimerCfg->ulRetrySearchTimeLen      = TI_NAS_MMC_HIGH_RPIO_RAT_HPLMN_TIMER_FIRST_LEN;
    pstHighHplmnTimerCfg->ucActiveFLg               = VOS_FALSE;
}



VOS_UINT8 NAS_MML_Get3GPP2UplmnNotPrefFlg( VOS_VOID )
{
    return NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.uc3GPPUplmnNotPrefFlg;
}


VOS_VOID NAS_MML_Set3GPP2UplmnNotPrefFlg(VOS_UINT8 uc3GPPUplmnNotPrefFlg)
{
    NAS_MML_GetMmlCtx()->stMsCfgInfo.stCustomCfg.uc3GPPUplmnNotPrefFlg = uc3GPPUplmnNotPrefFlg;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

