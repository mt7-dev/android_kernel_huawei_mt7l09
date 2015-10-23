
#ifndef _NAS_RABM_MAIN_H_
#define _NAS_RABM_MAIN_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "AtRabmInterface.h"
#include "NVIM_Interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define NAS_RABM_GetProFuncTblAddr(enSysMode)                   (g_pstNasRabmProcFuncTbl[enSysMode])
#define NAS_RABM_SetProFuncTblAddr(enSysMode, pstProcFucTbl)    (g_pstNasRabmProcFuncTbl[enSysMode] = (pstProcFucTbl))

#define NAS_RABM_GetCurrentSysMode()                (g_enNasRabmSysMode)
#define NAS_RABM_SetSysMode(enMode)                 (g_enNasRabmSysMode = (enMode))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum NAS_RABM_FASTDORM_STATUS_ENUM
{
    NAS_RABM_FASTDORM_STOP,                                                     /* 当前FAST DORMANCY在停止状态 */
    NAS_RABM_FASTDORM_INIT_DETECT,                                              /* 当前FAST DORMANCY在初始检测流量 */
    NAS_RABM_FASTDORM_WAIT_GMM_QRY_RESULT,                                      /* 当前FAST DORMANCY在等待GMM流程业务查询结果状态 */
    NAS_RABM_FASTDORM_RUNNING,                                                  /* 发送START DORMANCY后，当前FAST DORMANCY在运行状态 */
    NAS_RABM_FASTDORM_RETRY,                                                    /* 当前有业务和信令流程，当前FAST DORMANCY在RETRY状态 */
    NAS_RABM_FASTDORM_PAUSE,                                                    /* 发现有流量，当前FAST DORMANCY在暂停状态 */
    NAS_RABM_FASTDORM_SUSPEND,                                                  /* 启动FAST DORMANCY后，到2G下时，当前FAST DORMANCY在挂起状态 */
    NAS_RABM_FASTDORM_BUTT
};
typedef VOS_UINT32 NAS_RABM_FASTDORM_STATUS_ENUM_UINT32;
enum NAS_RABM_OM_MSG_ID_ENUM
{
    /* MMC发送给OM的消息 */
    NAS_RABM_OM_MSG_FASTDORM_STATUS = 0x1111,                                   /*_H2ASN_MsgChoice  NAS_MMC_LOG_FSM_INFO_STRU */
    NAS_RABM_OM_MSG_BUTT
};
typedef VOS_UINT32 NAS_RABM_OM_MSG_ID_ENUM_UINT32;


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

typedef VOS_VOID (*NAS_RABM_SYS_MODE_CHG_PROC_FUNC)(VOS_VOID);

typedef VOS_VOID (*NAS_RABM_SM_ACTIVATE_IND_PROC_FUNC)(VOS_VOID *pMsg);
typedef VOS_VOID (*NAS_RABM_SM_DEACTIVATE_IND_PROC_FUNC)(VOS_VOID *pMsg);
typedef VOS_VOID (*NAS_RABM_SM_MODIFY_IND_PROC_FUNC)(VOS_VOID *pMsg);

typedef VOS_VOID (*NAS_RABM_GMM_REESTABLISH_CNF_PROC_FUNC)(VOS_VOID *pMsg);
typedef VOS_VOID (*NAS_RABM_DATA_SUSPEND_IND_FUNC)(VOS_BOOL bRatChangeFlg);
typedef VOS_VOID (*NAS_RABM_DATA_RESUME_IND_FUNC)(VOS_UINT8 ucRebuildRabFlag);

typedef VOS_VOID (*NAS_RABM_SYS_MODE_CHG_IND_FUNC)(GMM_RABM_NET_RAT_ENUM_UINT32 enOldSysMode, GMM_RABM_NET_RAT_ENUM_UINT32 enNewSysMode);
typedef VOS_VOID (*NAS_RABM_TASK_ENTRY_FUNC)(struct MsgCB* pMsg);

/*****************************************************************************
 结构名    : NAS_RABM_PROC_FUNC_TBL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RABM消息处理函数表结构
*****************************************************************************/
typedef struct
{
    NAS_RABM_SM_ACTIVATE_IND_PROC_FUNC      pSmActivateIndProcFunc;
    NAS_RABM_SM_DEACTIVATE_IND_PROC_FUNC    pSmDeactivateIndProcFunc;
    NAS_RABM_SM_MODIFY_IND_PROC_FUNC        pSmModifyIndProcFunc;

    NAS_RABM_GMM_REESTABLISH_CNF_PROC_FUNC  pGmmReestablishCnfProcFunc;

    NAS_RABM_DATA_SUSPEND_IND_FUNC          pDataSuspendIndProcFunc;
    NAS_RABM_DATA_RESUME_IND_FUNC           pDataResumeIndProcFunc;
    NAS_RABM_SYS_MODE_CHG_IND_FUNC          pSysModeChgIndProcFunc;

    NAS_RABM_TASK_ENTRY_FUNC                pTaskEntry;

}NAS_RABM_PROC_FUNC_TBL_STRU;

/*****************************************************************************
 结构名    : NAS_RABM_SYS_CHG_TBL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :  模式改变处理函数表结构
*****************************************************************************/
typedef struct
{
    GMM_RABM_NET_RAT_ENUM_UINT32        enOldMode;
    GMM_RABM_NET_RAT_ENUM_UINT32        enNewMode;
    NAS_RABM_SYS_MODE_CHG_PROC_FUNC     pSysModeChgProcFunc;

} NAS_RABM_SYS_CHG_TBL_STRU;



typedef struct
{
    VOS_UINT32                                              ulCurrNoFluxCnt;            /* 无流量时长计数，以秒为单位 */
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32                  enFastDormOperationType;    /* 当前FAST DORMANCY的操作方式 */
    VOS_UINT32                                              ulUserDefNoFluxCnt;         /* 用户定义的无流量时长要求 */
    NAS_RABM_FASTDORM_STATUS_ENUM_UINT32                    enCurrFastDormStatus;       /* 存储当前的FAST DORMANCY运行状态 */

    VOS_UINT32                                              ulUlDataCnt;                /* 上行数据计数器 */
    VOS_UINT32                                              ulDlDataCnt;                /* 下行数据计数器 */

} NAS_RABM_FASTDORM_CTX_STRU;



typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头                   */ /*_H2ASN_Skip*/
    NAS_RABM_FASTDORM_CTX_STRU          stFastDormCtx;
}NAS_RABM_LOG_FASTDORM_INFO_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

/*****************************************************************************
 函 数 名  : NAS_RABM_RegProcFuncTbl
 功能描述  : 注册指定系统模式下的RABM函数处理表
 输入参数  : enSysMode      - 系统模式
             pstProcFucTbl  - 函数处理表
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RegProcFuncTbl(
    GMM_RABM_NET_RAT_ENUM_UINT32        enSysMode,
    NAS_RABM_PROC_FUNC_TBL_STRU        *pstProcFucTbl
);


/*****************************************************************************
 函 数 名  : NAS_RABM_CtxInit
 功能描述  : RABM初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_CtxInit(VOS_VOID);


/*****************************************************************************
 函 数 名  : NAS_RABM_MsgProc
 功能描述  : 处理RABM消息, 根据消息类型调用相应的处理函数
 输入参数  : struct MsgCB                       *pMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_MsgProc(
    struct MsgCB                       *pMsg
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmMsg
 功能描述  : 处理SM消息, 根据消息类型调用对应处理函数
 输入参数  : struct MsgCB                       *pMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmMsg(
    struct MsgCB                       *pMsg
);


/*****************************************************************************
 函 数 名  : NAS_RABM_ProcessGmmMsg
 功能描述  : 处理GMM消息, 根据消息类型调用对应处理函数
 输入参数  : struct MsgCB                       *pMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvGmmMsg(
    struct MsgCB                       *pMsg
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmActivateInd
 功能描述  : 处理ID_SM_RABM_ACTIVATE_IND原语
 输入参数  : pstSmActivateInd - ID_SM_RABM_ACTIVATE_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmActivateInd(
    RABMSM_ACTIVATE_IND_STRU           *pstSmActivateInd
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmDeactivateInd
 功能描述  : 处理RABMSM_DEACTIVATE_IND原语
 输入参数  : pstSmDeactivateInd - RABMSM_DEACTIVATE_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmDeactivateInd(
    RABMSM_DEACTIVATE_IND_STRU         *pstSmDeactivateInd
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmModifyInd
 功能描述  : 处理RABMSM_MODIFY_IND原语
 输入参数  : pstSmModifyInd - RABMSM_MODIFY_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmModifyInd(
    RABMSM_MODIFY_IND_STRU             *pstSmModifyInd
);

#if (FEATURE_ON == FEATURE_LTE)

/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmBearerActivateInd
 功能描述  : 处理ID_SM_RABM_BEARER_MODIFY_IND原语
 输入参数  : pstSmBearerActivateInd - ID_SM_RABM_BEARER_ACTIVATE_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmBearerActivateInd(
    SM_RABM_BEARER_ACTIVATE_IND_STRU   *pstSmBearerActivateInd
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmBearerModifyInd
 功能描述  : 处理ID_SM_RABM_BEARER_DEACTIVATE_IND原语
 输入参数  : pstSmBearerModifyInd - ID_SM_RABM_BEARER_MODIFY_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmBearerModifyInd(
    SM_RABM_BEARER_MODIFY_IND_STRU     *pstSmBearerModifyInd
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSmBearerDeactivateInd
 功能描述  : 处理RABMSM_PDP_DEACTIVATE_IND原语
 输入参数  : pstSmBearerDeactivateInd - ID_SM_RABM_BEARER_DEACTIVATE_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSmBearerDeactivateInd(
    SM_RABM_BEARER_DEACTIVATE_IND_STRU *pstSmBearerDeactivateInd
);

#endif

/*****************************************************************************
 函 数 名  : NAS_RABM_RcvGmmReestablishCnf
 功能描述  : 处理ID_GMM_RABM_REESTABLISH_CNF原语
 输入参数  : pstGmmReestablishCnf   - ID_GMM_RABM_REESTABLISH_CNF消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvGmmReestablishCnf(
    GMMRABM_REESTABLISH_CNF_STRU       *pstGmmReestablishCnf
);


/*****************************************************************************
 函 数 名  : NAS_RABM_ProcSysSrvChangeInd
 功能描述  : 处理系统状态改变指示消息, 包含系统模式和服务状态
 输入参数  : pstGmmSysSrvChgInd - ID_GMM_RABM_SYS_SRV_CHG_IND消息内容
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvGmmSysSrvChgInd(
    GMM_RABM_SYS_SRV_CHG_IND_STRU      *pstGmmSysSrvChgInd
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvDataSuspendInd
 功能描述  : 处理挂起指示
 输入参数  : bRatChangeFlg - 异系统切换标志
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvDataSuspendInd(
    VOS_BOOL                            bRatChangeFlg
);

/*****************************************************************************
 函 数 名  : NAS_RABM_RcvDataResumeInd
 功能描述  : 处理恢复指示
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
 修改历史  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvDataResumeInd(
    VOS_UINT8                           ucRebuildRabFlag
);


/*****************************************************************************
 函 数 名  : NAS_RABM_RcvSysModeChgInd
 功能描述  : 处理系统模式改变
 输入参数  : enNewSysMode - 新的系统模式
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvSysModeChgInd(
    GMM_RABM_NET_RAT_ENUM_UINT32        enNewSysMode
);


/*****************************************************************************
 函 数 名  : NAS_RABM_ProcessCommMsg
 功能描述  :
 输入参数  : struct MsgCB                       *pMsg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID NAS_RABM_RcvCommMsg(
    struct MsgCB                       *pMsg
);

extern VOS_UINT32    NAS_RABM_IsPdpActiveInWCDMA(VOS_VOID);
extern VOS_VOID NAS_RABM_ConvertAtFasTDormTypeToWrrFasTDormType(
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32                enAtFastDormType,
    RRC_FASTDORM_START_TYPE_ENUM_UINT32                 *penWrrFastDormType
);
extern VOS_VOID NAS_RABM_RcvSetFastDormParaReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
);
extern VOS_VOID NAS_RABM_RcvGetFastDormParaReq(
    AT_RABM_QRY_FASTDORM_PARA_REQ_STRU *pstFastDormPara
);

extern VOS_VOID NAS_RABM_RcvAtMsg(
    struct MsgCB                       *pMsg
);
extern VOS_VOID NAS_RABM_SuspendFastDorm(VOS_VOID);
extern VOS_VOID NAS_RABM_ResumeFastDorm(VOS_VOID);

extern VOS_VOID NAS_RABM_SysModeChgProcFastDorm(
    GMM_RABM_NET_RAT_ENUM_UINT32        enOldSysMode,
    GMM_RABM_NET_RAT_ENUM_UINT32        enNewSysMode
);
extern VOS_VOID NAS_RABM_RcvAtFastDormStartReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
);
extern VOS_VOID NAS_RABM_RcvAtFastDormStopReq(
    AT_RABM_SET_FASTDORM_PARA_REQ_STRU *pstFastDormPara
);
extern VOS_VOID NAS_RABM_SetFastDormUserDefNoFluxCntValue(
    VOS_UINT32                          ulNoFluxCnt
);
extern VOS_UINT32 NAS_RABM_GetFastDormUserDefNoFluxCntValue(VOS_VOID);
extern VOS_VOID NAS_RABM_SetFastDormCurrNoFluxCntValue(
    VOS_UINT32                          ulNoFluxCnt
);
extern VOS_UINT32 NAS_RABM_GetFastDormCurrNoFluxCntValue(VOS_VOID);
extern VOS_VOID NAS_RABM_SetFastDormOperationType(
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32 enFastDormOperationType
);
extern AT_RABM_FASTDORM_OPERATION_ENUM_UINT32 NAS_RABM_GetFastDormOperationType(VOS_VOID);
extern NAS_RABM_FASTDORM_STATUS_ENUM_UINT32 NAS_RABM_GetCurrFastDormStatus(VOS_VOID);
extern VOS_VOID NAS_RABM_RcvWasFastDormInfoInd(
    struct MsgCB                       *pMsg
);
extern VOS_VOID NAS_RABM_SetCurrFastDormStatus(
    NAS_RABM_FASTDORM_STATUS_ENUM_UINT32    enCurrFastDormStatus
);
extern VOS_VOID NAS_RABM_InitFastDormCtx(VOS_VOID);



VOS_VOID NAS_RABM_IncFastDormUlDataCnt(VOS_VOID);

VOS_VOID NAS_RABM_IncFastDormDlDataCnt(VOS_VOID);

VOS_UINT32 NAS_RABM_GetFastDormUlDataCnt(VOS_VOID);

VOS_UINT32 NAS_RABM_GetFastDormDlDataCnt(VOS_VOID);

VOS_VOID NAS_RABM_ClrFastDormUlDataCnt(VOS_VOID);

VOS_VOID NAS_RABM_ClrFastDormDlDataCnt(VOS_VOID);

VOS_VOID NAS_RABM_RcvCdsMsg(
    struct MsgCB                       *pMsg
);

VOS_VOID NAS_RABM_RcvCdsServiceInd(
    CDS_RABM_SERVICE_IND_STRU          *pstCdsServiceInd
);



VOS_UINT32 NAS_RABM_IsDataServiceRequestPending(VOS_VOID);
VOS_UINT32 NAS_RABM_IsRabReestablishPending(VOS_VOID);
VOS_VOID NAS_RABM_MNTN_TracePktLostEvt(
    NAS_RABM_PKT_FLOW_DIR_ENUM_UINT8    enPtkFlowDir,
    VOS_UINT8                           ucPoint
);

VOS_VOID NAS_RABM_RcvReleaseRrcReq(
    AT_RABM_RELEASE_RRC_REQ_STRU       *pstMsg
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

#endif

