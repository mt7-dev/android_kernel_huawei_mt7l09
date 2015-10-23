

#ifndef __TAFAPS_COMFUNC_H__
#define __TAFAPS_COMFUNC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "NasStkInterface.h"

#include "TafApsGetPdpIdList.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define TAF_APS_GET_SM_ERR_CODE_MAP_TBL_PTR()   (g_astTafApsSmErrCodeMapTbl)
#define TAF_APS_GET_SM_ERR_CODE_MAP_TBL_SIZE()  (sizeof(g_astTafApsSmErrCodeMapTbl)/sizeof(TAF_APS_SM_ERR_CODE_MAP_STRU))

#define TAF_APS_GET_L4A_ERR_CODE_MAP_TBL_PTR()  (g_astTafApsL4aErrCodeMapTbl)
#define TAF_APS_GET_L4A_ERR_CODE_MAP_TBL_SIZE() (sizeof(g_astTafApsL4aErrCodeMapTbl)/sizeof(TAF_APS_L4A_ERR_CODE_MAP_STRU))


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT8 NAS_MML_GetPsAttachAllowFlg(VOS_VOID);

VOS_UINT32   TAF_APS_AllocPdpId(
    TAF_CTRL_STRU                      *pstCtrl,
    VOS_UINT8                           ucCid,
    VOS_UINT8                          *pucPdpId
);
VOS_UINT32   TAF_APS_IsPdpIdValid(
    VOS_UINT8                           ucPdpId
);

VOS_UINT8 TAF_APS_GetCidValue(
    VOS_UINT32                          ulBearerActivateFlg,
    VOS_UINT8                          *pucCidList
);
VOS_UINT32 TAF_APS_CheckCidDefinedCtx(
    VOS_UINT8                           ucCid
);
VOS_UINT8   TAF_APS_GetPdpIdByNsapi(
    VOS_UINT8                           ucNsapi
);
VOS_UINT8 TAF_APS_GetPdpIdByLlcSapi(
    VOS_UINT8                           ucLlcSapi
);

VOS_UINT32 TAF_APS_CheckBearerPdpTypeSuitable(
    VOS_UINT8                           ucDefaultPdpType,
    TAF_PDP_TYPE_ENUM_UINT8             enUserPdpType
);

VOS_UINT32 TAF_APS_String2Hex(
    VOS_UINT8                          *pucStr,
    VOS_UINT16                          usStrLen,
    VOS_UINT32                         *pulResult
);
VOS_UINT8   TAF_APS_GetPdpIdByCid(
    VOS_UINT8                           ucCid
);

VOS_UINT8   TAF_APS_GetPdpEntOpId(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

VOS_UINT32   TAF_APS_GetPdpEntModuleId(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

VOS_UINT8  TAF_APS_GetPdpIdByDefaultCid (VOS_VOID);

VOS_VOID TAF_APS_SetPdpEntClientId (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    VOS_UINT16                          usClientId
);
VOS_VOID TAF_APS_SetPdpEntOpId (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    VOS_UINT8                           ucOpId
);
VOS_UINT32  TAF_APS_GetTiFromPdpId(
    VOS_UINT8                           ucPdpId,
    SM_TAF_TRANS_ID_STRU               *pstTi
);
VOS_UINT32  TAF_APS_GetPdpIdFromTi(
    SM_TAF_TRANS_ID_STRU               *pstTi,
    VOS_UINT8                          *pucPdpId
);
VOS_VOID  TAF_APS_SetPdpEntityTi(
    VOS_UINT8                           ucPdpId,
    SM_TAF_TRANS_ID_STRU               *pstTis
);
VOS_UINT32 TAF_APS_GetCidFromLteBitCid(
    VOS_UINT32                          ulBitCid
);

VOS_UINT32  TAF_APS_GetActivedCid(
    VOS_UINT8                           *pucActivedCid
);

VOS_UINT32 TAF_APS_Ipv4AddrAtoi(
    VOS_CHAR                           *pcString,
    VOS_UINT8                          *pucNumber
);

VOS_UINT32 TAF_APS_ValidatePdpType(
    TAF_PDP_TYPE_ENUM_UINT8             enPdpTypeReq,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpTypeCnf
);

VOS_UINT32 TAF_APS_Ipv4AddrItoa(
    VOS_CHAR                           *pcString,
    VOS_UINT8                          *pucNumber
);

VOS_UINT8 TAF_APS_ConverUserDefPdpTypeToApsEntityPdpType (
    TAF_PDP_TYPE_ENUM_UINT8             enUserPdpType,
    VOS_UINT8                           ucDefaultPdpType
);

VOS_VOID TAF_APS_GetSpecPdpLinkedPdpInfo (
    TAF_APS_PDPID_LIST_STRU            *pstPdpIdList,
    VOS_UINT8                           ucNsapi
);

extern TAF_APS_USER_CONN_STATUS_ENUM_UINT8 TAF_APS_GetUserConnStatus(VOS_VOID);

/*****************************************************************************
 函 数 名  : TAF_APS_MapSmCause
 功能描述  : 将SM错误码映射成PS域呼叫错误码
 输入参数  : enSmCause - SM错误码
 输出参数  : 无
 返 回 值  : TAF_PS_CAUSE_ENUM_UINT32
 调用函数  :
 被调函数  :
*****************************************************************************/
TAF_PS_CAUSE_ENUM_UINT32 TAF_APS_MapSmCause(
    SM_TAF_CAUSE_ENUM_UINT16            enSmCause
);


#if (FEATURE_ON == FEATURE_LTE)
/*****************************************************************************
 函 数 名  : TAF_APS_MapL4aCause
 功能描述  : 将L4A错误码映射成PS域呼叫错误码
 输入参数  : enL4aCause - L4A错误码
 输出参数  : 无
 返 回 值  : TAF_PS_CAUSE_ENUM_UINT32
 调用函数  :
 被调函数  :
*****************************************************************************/
TAF_PS_CAUSE_ENUM_UINT32 TAF_APS_MapL4aCause(
    VOS_UINT32                          ulL4aCause
);
#endif

/*****************************************************************************
 函 数 名  : TAF_APS_ConvertPdpType
 功能描述  : 转换PDP类型(APS--->TAF)
 输入参数  : ucPdpTypeNum - 内部PDP类型
 输出参数  : 无
 返 回 值  : TAF_PDP_TYPE_ENUM_UINT8
 调用函数  :
 被调函数  :
*****************************************************************************/
TAF_PDP_TYPE_ENUM_UINT8 TAF_APS_ConvertPdpType(VOS_UINT8 ucPdpTypeNum);

/*****************************************************************************
 函 数 名  : TAF_APS_GetCtrlHdrFromCmdBufferQueue
 功能描述  : 从缓存消息队列中获取消息头
 输入参数  : AF_APS_TIMER_ID_ENUM_UINT32        enTimer
 输出参数  : TAF_CTRL_STRU                      *pstCtrl
 返 回 值  : VOS_UINT32

*****************************************************************************/
VOS_UINT32 TAF_APS_GetCtrlHdrFromCmdBufferQueue(
    TAF_CTRL_STRU                      *pstCtrl,
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimer,
    VOS_UINT32                          ulPara
);

#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
 函 数 名  : TAF_APS_IsImsBearer
 功能描述  : 判断是否为IMS专有承载
 输入参数  : ulLinkedEpsbId - 关联的IMS专有承载ID
 输出参数  : 无
 返 回 值  : VOS_VOID

*****************************************************************************/
VOS_UINT32  TAF_APS_IsImsBearer(
    VOS_UINT8                           ucLinkedCid,
    VOS_UINT32                          ulLinkedEpsbId
);
#endif

#if (FEATURE_ON == FEATURE_PTM)
VOS_VOID TAF_APS_PsCallFailErrRecord(TAF_PS_CAUSE_ENUM_UINT32 enCause);
#endif


/*****************************************************************************
 函 数 名  : TAF_APS_SetPdpEntModuleId
 功能描述  : 设置PDP ID实体所对应的ulModuleId值
 输入参数  : ucPdpId        :PDP ID的值
             ulModuleId     :ulModuleId的值
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID TAF_APS_SetPdpEntModuleId (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulModuleId
);

VOS_VOID TAF_APS_SetPdpEntCidValue (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

VOS_VOID TAF_APS_RmvPdpEntClientInfo (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    TAF_APS_BITCID_INFO_STRU           *pstBitCid
);

VOS_VOID   TAF_APS_SetPdpEntClientInfo(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);

VOS_VOID   TAF_APS_AddPdpEntClientInfo(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    CONST TAF_CTRL_STRU                *pstCtrl
);

VOS_VOID TAF_APS_SetPdpEntBitCidMask (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);
VOS_VOID TAF_APS_RmvPdpEntBitCidMask (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

VOS_UINT32 TAF_APS_IsPdpEntBitCidMaskExit (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

#if (FEATURE_ON == FEATURE_IMS)
VOS_UINT32 TAF_APS_CheckImsBearerByCid (
    VOS_UINT8                           ucCid
);

VOS_UINT32 TAF_APS_CheckImsBearerByRabId (
    VOS_UINT32                          ulRabId
);

VOS_VOID TAF_APS_GetImsBearerCid (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                          *pucCid
);

#endif

VOS_VOID TAF_APS_GetPdpEntBitCidInfo(
    VOS_UINT8                           ucPdpId,
    TAF_APS_BITCID_INFO_STRU                    *pstCid
);

VOS_VOID TAF_APS_SetPdpEntAttachBearerFlag (
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucFlag
);

VOS_UINT8 TAF_APS_GetPdpEntAttachBearerFlag (
    VOS_UINT8                           ucPdpId
);

VOS_VOID   TAF_APS_SetPsCallEvtCtrl(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    TAF_CTRL_STRU                      *pstCtrl
);

VOS_INT32 TAF_APS_CompareApn(
    const VOS_UINT8                 aucBearerApn[],
    VOS_UINT8                       ucBearerApnLen,
    const VOS_UINT8                 aucUserApn[],
    VOS_UINT8                       ucUserApnLen
);

VOS_VOID TAF_APS_SetPdpEntDialPdpType(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_UINT8 TAF_APS_GetPdpEntDialPdpType(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

/*****************************************************************************
 函 数 名  : TAF_APS_MatchCallWithAllBearer
 功能描述  : 检查当前是否有承载匹配用户拨号参数的承载
 输入参数  : pstCtrl      --- 控制信息
             pstDialParam --- 拨号参数
 输出参数  : pucPdpId     --- 实体索引
 返 回 值  : VOS_TRUE/VOS_FALSE
*****************************************************************************/
VOS_UINT32 TAF_APS_MatchCallWithAllBearer(
    CONST TAF_CTRL_STRU                *pstCtrl,
    CONST TAF_APS_MATCH_PARAM_STRU     *pstMatchParam,
    VOS_UINT8                          *pucPdpId
);

VOS_UINT16   TAF_APS_GetPdpEntClientId(
    VOS_UINT8                           ucPdpId,
    VOS_UINT8                           ucCid
);

VOS_VOID TAF_APS_SetPdpEntModDialInfo (
    VOS_UINT8                           ucPdpId,
    TAF_APS_PDP_MOD_DIAL_STRU          *pstModDial
);

VOS_VOID TAF_APS_GetPdpEntModDialInfo (
    VOS_UINT8                           ucPdpId,
    TAF_APS_PDP_MOD_DIAL_STRU          *pstModDial
);


/*****************************************************************************
 函 数 名  : TAF_APS_SetPdpEntAddrType
 功能描述  : 设置实体上下文的地址类型
 输入参数  : ucPdpId   --- 实体索引(有效性由调用者保证)
             enPdpType --- PDP类型
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID TAF_APS_SetPdpEntAddrType(
    VOS_UINT8                           ucPdpId,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

#if (FEATURE_ON == FEATURE_LTE)
/*****************************************************************************
 函 数 名  : TAF_APS_IsAttachBearerExist
 功能描述  : 检查注册承载是否存在
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_TRUE/VOS_FALSE
*****************************************************************************/
VOS_UINT32 TAF_APS_IsAttachBearerExist(VOS_VOID);
#endif

/*****************************************************************************
 函 数 名  : TAF_APS_IsAnyBearerExist
 功能描述  : 检查是否有承载存在
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_TRUE/VOS_FALSE
*****************************************************************************/
VOS_UINT32 TAF_APS_IsAnyBearerExist(VOS_VOID);

/*****************************************************************************
 函 数 名  : TAF_APS_GetAttachAllowFlg
 功能描述  : 获取注册允许标识
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_TRUE/VOS_FALSE
*****************************************************************************/
VOS_UINT8 TAF_APS_GetAttachAllowFlg(VOS_VOID);

/*****************************************************************************
 函 数 名  : TAF_APS_GenMatchParamByCidInfo
 功能描述  : 生成承载匹配参数
 输入参数  : pstPdpTblInfo --- CID表信息
 输出参数  : pstMatchParam --- 匹配参数
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID TAF_APS_GenMatchParamByCidInfo(
    CONST TAF_PDP_TABLE_STRU           *pstPdpTblInfo,
    TAF_APS_MATCH_PARAM_STRU           *pstMatchParam
);

/*****************************************************************************
 函 数 名  : TAF_APS_GenMatchParamByDailInfo
 功能描述  : 生成承载匹配参数
 输入参数  : pstDialParaInfo --- 拨号参数
 输出参数  : pstMatchParam   --- 匹配参数
 返 回 值  : VOS_VOID
*****************************************************************************/
VOS_VOID TAF_APS_GenMatchParamByDailInfo(
    CONST TAF_PS_DIAL_PARA_STRU        *pstDialParaInfo,
    TAF_APS_MATCH_PARAM_STRU           *pstMatchParam
);



#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafApsCtx.h */
