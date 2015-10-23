

#ifndef __TAFAPSCTX_H__
#define __TAFAPSCTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "TafFsm.h"
#include "TafApsTimerMgmt.h"
#include "TafApsDsFlowStats.h"
#include "AtMnInterface.h"
#include "SmEsmInterface.h"
#include "MmcApsInterface.h"
#include "Taf_Aps.h"
#include "NasNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TAF_APS_MAX_MSG_BUFFER_LEN                      (1500)                  /* 缓存的最大长度 */
#define TAF_APS_MAX_MSG_QUEUE_NUM                       (22)                    /* 最大缓存的个数 */

#define TAF_APS_RAB_ID_OFFSET                           (5)                     /* 当前到网络的定义是5到15，因此有一个5的偏移量 */

#define TAF_APS_RAB_ID_MIN                              (5)                     /* RAB ID的取值范围，最小是5 */
#define TAF_APS_RAB_ID_MAX                              (15)                    /* RAB ID的取值范围，最大是15 */

/* 最大RAB承载个数 */
#define TAF_APS_MAX_RAB_NUM                             (15)                    /* RAB ID的取值范围，最大是15 */


#define TAF_APS_MAX_CMD_BUFFER_QUEUE_SIZE               (11)                    /* 等待消息结果的队列长度，目前假定为10个 */

#define TAF_APS_INVALID_PDPID                           (0xFF)                  /* 无效的PDP ID */
#define TAF_APS_INVALID_CID                             (0xFF)                  /* 无效的CID */
#define TAF_APS_INVALID_CLIENTID                        (0xFFFE)                /* 无效的CLIENT ID */
#define TAF_APS_INVALID_OPID                            (0xFF)                  /* 无效的OPID ID */

#define TAF_APS_MAX_MSG_QUEUE_SIZE                      (11)                    /* 内部消息队列最大的长度，
                                                                                   当前状态机内部发出，应该最多十个PDP实体，目前使用的长度是12 */
#define TAF_APS_INVALID_INTERNAL_MSG_ID                 (0xFFFF)                /* 消息ID的无效值 */

#define TAF_APS_32BIT_DATA_LEN                          (32)                    /* 32位数据的bit长度 */
#define TAF_APS_DEFAULT_CID                             (0)                     /* 默认承载所使用的CID，CID0 */
#define TAF_APS_TI_MASK                                 (7)                     /* 去除Ti的flag值时使用 */
#define TAF_APS_OCTET_MOVE_THREE_BITS                   (0x03)                  /* 将一个字节移动3位 */

/* PDP激活被拒10次之内，不限制用户向网侧发起PDP激活请求 */
#define TAF_APS_PDP_ACT_LIMIT_NO_DELAY_COUNT_THRESHOLD   (10)

/* PDP激活被拒11-19次之内，按照10s步长递增启动定时器 */
#define TAF_APS_PDP_ACT_LIMIT_STEP_DELAY_COUNT_THRESHOLD (19)
#define TAF_APS_PDP_ACT_LIMIT_STEP_TIME                  (10)

/* PDP激活被拒19次(包括19次)以上，按照90s时长启动定时器 */
#define TAF_APS_PDP_ACT_LIMIT_AVERAGE_DELAY_TIME         (90)

/* PDP暂停激活定时器时长随机数的阈值 */
#define TAF_APS_PDP_ACT_LIMIT_RANDOM_TIME_THRESHOLD      (45)

/* 当失败次数是10的整数倍时，PDP暂停激活定时器使用随机时长 */
#define TAF_APS_PDP_ACT_LIMIT_RANDOM_TIME_FAILCOUNT      (10)

/* TI_TAF_APS_LIMIT_PDP_ACT定时器不和任何PDP ID邦定，所以定义一个无效值 */
#define TAF_APS_PDP_ACT_LIMIT_PDP_ID                    (0xFF)

#define TAF_APS_DSFLOW_AT_REPORT_PERIOD                 (2)

#define TAF_APS_GET_MULTI_DFS_SUPPORT_FLG()     (g_stTafApsCtx.stSwitchDdrRateInfo.ulMultiDfsFlg)

#define TAF_APS_GET_MULTI_DFS_PROFILE_NUM(dfsType)             (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[dfsType].ulProfileNum)
#define TAF_APS_GET_MULTI_DFS_PROFILE_DL_RATE(dfsType, i)      (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[dfsType].astProfile[i].ulDlRate)
#define TAF_APS_GET_MULTI_DFS_PROFILE_UL_RATE(dfsType, i)      (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[dfsType].astProfile[i].ulUlRate)
#define TAF_APS_GET_MULTI_DFS_PROFILE_DDR_BAND(dfsType, i)     (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[dfsType].astProfile[i].ulDdrBand)

#define TAF_APS_GET_DFS_PROFILE_NUM()           (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[0].ulProfileNum)
#define TAF_APS_GET_DFS_PROFILE_DL_RATE(i)      (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[0].astProfile[i].ulDlRate)
#define TAF_APS_GET_DFS_PROFILE_UL_RATE(i)      (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[0].astProfile[i].ulUlRate)
#define TAF_APS_GET_DFS_PROFILE_DDR_BAND(i)     (g_stTafApsCtx.stSwitchDdrRateInfo.astDfsConfig[0].astProfile[i].ulDdrBand)

#define TAF_APS_DFS_REQUEST_SUCC_CNT(i)         (g_stTafApsCtx.stSwitchDdrRateInfo.stDfsDebugInfo.ulRequestSucCnt  += i)
#define TAF_APS_DFS_REQUEST_FAIL_CNT(i)         (g_stTafApsCtx.stSwitchDdrRateInfo.stDfsDebugInfo.ulRequestFailCnt  += i)
#define TAF_APS_DFS_UPDATE_SUCC_CNT(i)          (g_stTafApsCtx.stSwitchDdrRateInfo.stDfsDebugInfo.ulUpdateSucCnt  += i)
#define TAF_APS_DFS_UPDATE_FAIL_CNT(i)          (g_stTafApsCtx.stSwitchDdrRateInfo.stDfsDebugInfo.ulUpdateFailCnt += i)

#define TAF_APS_GET_DSFLOW_RELEASE_DFS()        (g_stTafApsCtx.stSwitchDdrRateInfo.ulMinBand)
#define TAF_APS_GET_DSFLOW_MIN_DDR_BAND()       (g_stTafApsCtx.stSwitchDdrRateInfo.ulMinBand)
#define TAF_APS_GET_DSFLOW_MAX_DDR_BAND()       (g_stTafApsCtx.stSwitchDdrRateInfo.ulMaxBand)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum TAF_APS_FSM_ID_ENUM
{
    /* TAF APS MAIN 状态机，当前处于稳态 */
    TAF_APS_FSM_MAIN                                    =0X00,

    /* TAF APS MS ACTIVATING状态机，当前在UE发起激活操作过程中 */
    TAF_APS_FSM_MS_ACTIVATING                           =0X01,

    /* TAF APS NW ACTIVATING状态机，当前在NW发起激活操作过程中 */
    TAF_APS_FSM_NW_ACTIVATING                           =0X02,

    /* TAF APS MS DEACTIVATING状态机，当前在UE发起去激活操作过程中 */
    TAF_APS_FSM_MS_DEACTIVATING                         =0X03,

    /* TAF APS MS MODIFYING状态机，当前在UE发起MODIFY操作过程中 */
    TAF_APS_FSM_MS_MODIFYING                            =0X04,

    TAF_APS_FSM_BUTT

};
typedef VOS_UINT32 TAF_APS_FSM_ID_ENUM_UINT32;
enum TAF_APS_DFS_TYPE_ENUM
{
    TAF_APS_DFS_TYPE_BASE               = 0x00,                                 /*当前未在任何网络上驻留*/
    TAF_APS_DFS_TYPE_GSM                = 0x01,                                 /* GSM下DFS */
    TAF_APS_DFS_TYPE_WCDMA              = 0x02,                                 /* WCDMA下DFS */
    TAF_APS_DFS_TYPE_LTE                = 0x03,                                 /* LTE下DFS */
    TAF_APS_DFS_TYPE_TD_WCDMA           = 0x04,                                 /* TD-WCDMA下DFS */
    TAF_APS_DFS_TYPE_CDMA               = 0x05,                                 /* CDMA下DFS */
    TAF_APS_DFS_TYPE_BUTT                                                       /* 无效的接入技术 */
};
typedef VOS_UINT32 TAF_APS_DFS_TYPE_ENUM_UINT32;

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

typedef SM_ESM_EPS_BEARER_INFO_IND_STRU TAF_APS_EPS_BEARER_INFO_IND_STRU;




typedef struct
{
    VOS_UINT8                           ucRabActiveFlag;                        /*标识该RAB是否激活*/
    VOS_UINT8                           aucReserve[3];                          /*保留位*/

    VOS_UINT32                          ulStartLinkTime;                        /*当前连接开始时间*/
    TAF_DSFLOW_INFO_STRU                stCurrentFlowInfo;  /*当前连接流量信息，即最后一次PDP连接的流量信息 */
    TAF_DSFLOW_INFO_STRU                stTotalFlowInfo;    /*累积流量信息，该RAB历史所有连接累加的流量信息 */

}TAF_APS_DSFLOW_STATS_STRU;
typedef struct
{
    VOS_UINT32                          ulRequestSucCnt;
    VOS_UINT32                          ulRequestFailCnt;
    VOS_UINT32                          ulUpdateSucCnt;                         /* 投票成功次数 */
    VOS_UINT32                          ulUpdateFailCnt;                        /* 投票失败次数 */
}TAF_APS_DFS_DEBUG_INFO_STRU;


typedef struct
{
    VOS_INT32                           iDfsReqId;                              /* DDR调频投票请求ID */
    VOS_UINT32                          ulReqValue;                             /* DDR调频投票请求值 */
    VOS_UINT32                          ulSetMaxFlag;                           /* 设置最大频率的标识 */

    VOS_UINT32                          ulMultiDfsFlg;
    TAF_NV_DFS_DSFLOW_RATE_CONFIG_STRU  astDfsConfig[TAF_NVIM_DIFF_DFS_NUM];
    TAF_APS_DFS_DEBUG_INFO_STRU         stDfsDebugInfo;

    VOS_UINT32                          ulMinBand;                              /* 最小DDR频率 */
    VOS_UINT32                          ulMaxBand;                              /* 最大DDR频率 */
}TAF_APS_SWITCH_DDR_RATE_INFO_STRU;
typedef struct
{
    VOS_UINT8                           ucApsDsFlowSave2NvFlg;                  /* 标识是否需要保存历史流量信息到NV中 */

    VOS_UINT8                           ucDsFlowOMReportFlg;                    /* 标识是否进行OM流量上报 */
    VOS_UINT8                           ucDsFlowATRptPeriod;                    /* AT端口流量上报的周期单位s */
    VOS_UINT8                           ucDsFlowOMRptPeriod;                    /* OM流量上报的周期单位s */
    VOS_UINT8                           ucDsFlowATRptTmrExpCnt;                 /* AT端口流量上报时机，用于控制上报周期 */
    VOS_UINT8                           ucDsFlowOMRptTmrExpCnt;                 /* OM流量上报时机，用于控制上报周期 */

    VOS_UINT8                           ucApsDsFlowSavePeriod;                  /* 流量写NV的周期 */
    VOS_UINT8                           ucReserve;
    TAF_APS_DSFLOW_STATS_STRU           astApsDsFlowStats[TAF_APS_MAX_RAB_NUM]; /* 保留以RABID为单位的流量数据 */

}TAF_APS_DSFLOW_STATS_CTX_STRU;



typedef struct
{
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimerId;
    VOS_UINT32                         *pulMsgInfo;
    VOS_UINT32                          ulMsgInfoLen;
    VOS_UINT32                          ulPara;
}TAF_APS_CMD_BUFFER_STRU;


typedef struct
{
    VOS_UINT32                          ulEventType;
    VOS_UINT8                           aucEntryMsgBuffer[TAF_APS_MAX_MSG_BUFFER_LEN];
} TAF_APS_ENTRY_MSG_STRU;


typedef struct
{
    TAF_APS_ENTRY_MSG_STRU              stMsgEntry;                             /* 缓存的具体内容 */
}TAF_APS_CACHE_MSG_INFO_STRU;
typedef struct
{
    VOS_UINT32                          ucCurrIndex;                            /* 记录当前处理缓存的index */
    VOS_UINT8                           ucCacheMsgNum;                          /* 缓存消息的个数 */
    VOS_UINT8                           aucReserve[3];
    TAF_APS_CACHE_MSG_INFO_STRU         astMsgQueue[TAF_APS_MAX_MSG_QUEUE_NUM]; /* APS的消息队列数组，存储的是带通用消息头的消息 */
}TAF_APS_MSG_QUEUE_STRU;
typedef struct
{
    /* TAF APS消息类型*/
    AT_MN_MSGTYPE_ENUM_UINT16           enMsgType;
    VOS_UINT8                           ucPdpid;
    VOS_UINT8                           ucReserved;
} TAF_APS_PS_CALL_ANSWER_MSG_STRU;
typedef struct
{
    TAF_APS_PS_CALL_ANSWER_MSG_STRU     stAnswerMsg;

} TAF_APS_FSM_NW_ACTIVATING_CTX_STRU;


typedef union
{
    TAF_APS_FSM_NW_ACTIVATING_CTX_STRU  stNwActivatingCtx;

}TAF_APS_FSM_EXTRA_CTX_UNION;


typedef struct
{
    /* 状态机描述指针,指针为NULL表示当前状态机无效  */
    TAF_FSM_DESC_STRU                  *pstFsmDesc;

    /* 当前状态机标识 */
    TAF_APS_FSM_ID_ENUM_UINT32          enFsmId;

    /* 当前状态 */
    VOS_UINT32                          ulState;                            /*  */

    /* 子状态机入口消息MSGID和内容  */
    TAF_APS_ENTRY_MSG_STRU              stEntryMsg;

    /* 状态机数据 */
    TAF_APS_FSM_EXTRA_CTX_UNION         unFsmCtx;
}TAF_APS_SUB_FSM_CTX_STRU;


typedef struct
{
    /* 状态机描述指针,指针为NULL表示当前状态机无效  */
    TAF_FSM_DESC_STRU                  *pstFsmDesc;

    /* 当前状态机标识 */
    TAF_APS_FSM_ID_ENUM_UINT32          enFsmId;

    /* 当前状态 */
    VOS_UINT32                          ulState;                            /*  */
}TAF_APS_MAIN_FSM_CTX_STRU;


typedef struct
{
    TAF_APS_MAIN_FSM_CTX_STRU           stMainFsm;                              /* 主状态机 */
    TAF_APS_SUB_FSM_CTX_STRU            stSubFsm;                               /* 子状态机 */

    /**************************************************************************
                        TAF APS 当前的网络状态和PS域的卡状态，各个状态机实体里面独立维护
    **************************************************************************/
    MMC_APS_RAT_TYPE_ENUM_UINT32        enRatType;                              /* 查询返回的当前参数 */
    VOS_UINT32                          ulPsSimRegStatus;                       /* PS域SIM卡状态信息,VOS_TRUE:卡有效,VOS_FALSE:卡无效 */
    VOS_UINT8                           ucPdpId;                                /* 当前状态机所对应的PDP ID */
    VOS_UINT8                           aucReserve[3];
}TAF_APS_PDP_ENTITY_FSM_CTX_STRU;
typedef struct
{
    VOS_UINT32                          ulDefaultBearerInfoFlg;                 /* 当前默认承载信息是否使用的标记，VOS_TRUE:当前已经使用，
                                                                                   VOS_FALSE:当前还没有使用 */
    TAF_APS_EPS_BEARER_INFO_IND_STRU    stDefaultBearerInfo;                    /* 存储的默认承载信息 */
}TAF_APS_DEFAULT_BEARER_INFO_STRU;
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          usMsgID;                                /* 消息ID */
    VOS_UINT8                           aucReserve[2];

    VOS_UINT8                           aucBuf[TAF_APS_MAX_MSG_BUFFER_LEN];     /* 消息内容 */
}TAF_APS_INTERNAL_MSG_BUF_STRU;


typedef struct
{
    /* TAF APS内部消息队列 */
    TAF_APS_INTERNAL_MSG_BUF_STRU       astApsMsgQueue[TAF_APS_MAX_MSG_QUEUE_SIZE];

    /* 发送消息临时缓存buff,申请局部变量时直接使用该空间,为了避免发送内部消息时
    占用较大的栈空间 */
    TAF_APS_INTERNAL_MSG_BUF_STRU       stSendMsgBuf;

    /*内部消息缓冲队列的索引尾指针*/
    VOS_UINT8                           ucTail;

    /*内部消息缓冲队列的索引头指针*/
    VOS_UINT8                           ucHeader;

    VOS_UINT16                          usReserved;
} TAF_APS_INTERNAL_MSG_QUEUE_STRU;



typedef struct
{
    VOS_UINT8                           ucPdpActLimitFlg;                       /* 保存NV项打开关闭标志 */
    VOS_UINT8                           ucPdpActFailCount;                      /* 记录PDP激活失败次数 */
    VOS_UINT8                           aucReserved[2];                         /* 保留 四字节对齐 */
}TAF_APS_PDP_ACT_LIMIT_INFO_STRU;


typedef struct
{
    /**************************************************************************
                        存储L模的默认承载
    **************************************************************************/
    TAF_APS_DEFAULT_BEARER_INFO_STRU        stLteDefaultBearerInfo;             /* 存放默认承载信息 */

    /**************************************************************************
                        TAF APS状态机状态
    **************************************************************************/
    TAF_APS_PDP_ENTITY_FSM_CTX_STRU         astPdpEntityFsmCtx[TAF_APS_MAX_PDPID];   /* 当前最多11个PDP上下文，因此有11个独立的状态机 */
    TAF_APS_PDP_ENTITY_FSM_CTX_STRU        *pstCurrPdpEntityFsmCtx;                    /* 当前所使用的实体状态机 */

    TAF_APS_TIMER_CTX_STRU                  astApsTimerCtx[TAF_APS_MAX_TIMER_NUM];  /* APS当前正在运行的定时器资源 */
    TAF_APS_DSFLOW_STATS_CTX_STRU           stApsDsFlowCtx;                         /* APS流量统计上下文 */

    /**************************************************************************
                        TAF APS缓存消息队列
    **************************************************************************/
    TAF_APS_MSG_QUEUE_STRU                  stBufferEntryMsgQueue;              /* APS的缓存消息队列 */

    /**************************************************************************
                        TAF APS缓存命令队列
    **************************************************************************/
    TAF_APS_CMD_BUFFER_STRU                 astCmdBufferQueue[TAF_APS_MAX_CMD_BUFFER_QUEUE_SIZE]; /* 等待L4A设置查询结果的队列 */

    /**************************************************************************
                        APS 内部消息缓存队列
    **************************************************************************/
    TAF_APS_INTERNAL_MSG_QUEUE_STRU         stInternalMsgQueue;                     /* MM子层的内部消息队列 */

    TAF_APS_PDP_ACT_LIMIT_INFO_STRU         stPdpActLimitInfo;

    TAF_APS_SWITCH_DDR_RATE_INFO_STRU       stSwitchDdrRateInfo;                    /* DDR投票信息 */

    VOS_UINT32                              aulCallRemainTmrLen[TAF_MAX_CID + 1];
}TAF_APS_CONTEXT_STRU;

extern TAF_APS_CONTEXT_STRU             g_stTafApsCtx;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

TAF_APS_PDP_ENTITY_FSM_CTX_STRU* TAF_APS_GetPdpEntityFsm(
    VOS_UINT32                          ulMainFsmState,
    TAF_APS_FSM_ID_ENUM_UINT32          enSubFsmId,
    VOS_UINT32                          ulSubFsmState
);

VOS_VOID  TAF_APS_InitDsFlowCtx(
    TAF_APS_DSFLOW_STATS_CTX_STRU      *pstApsTimerCtx
);

TAF_APS_DSFLOW_STATS_CTX_STRU*  TAF_APS_GetDsFlowCtxAddr( VOS_VOID );

TAF_APS_TIMER_CTX_STRU*  TAF_APS_GetTimerCtxAddr( VOS_VOID );




TAF_APS_MSG_QUEUE_STRU* TAF_APS_GetCachMsgBufferAddr(VOS_VOID);


VOS_UINT8  TAF_APS_GetCacheNum( VOS_VOID );

VOS_VOID  TAF_APS_SaveCacheMsgInMsgQueue(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
);

VOS_UINT32  TAF_APS_SaveCacheMsg(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
);

VOS_UINT32  TAF_APS_ClearCacheMsg(
    VOS_UINT32                          ulEventType
);

VOS_UINT32  TAF_APS_GetNextCachedMsg(
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg
);

VOS_VOID  TAF_APS_SetCurrPdpEntityMainFsmState(
    VOS_UINT32                          ulState
);


VOS_VOID  TAF_APS_SetCurrPdpEntitySubFsmState(
    VOS_UINT32                          ulState
);


VOS_VOID  TAF_APS_DelWaitInfoInL4aMsgRsltQueue(
    AT_MN_MSGTYPE_ENUM_UINT16           enMsgType
);

VOS_UINT32  TAF_APS_SetPdpIdMainFsmState(
    VOS_UINT8                          ucPdpId,
    VOS_UINT32                         ulState
);

VOS_UINT32  TAF_APS_GetPdpIdMainFsmState(
    VOS_UINT8                          ucPdpId
);

VOS_UINT32  TAF_APS_CheckPdpServiceActivated(
    VOS_UINT8                          ucPdpId
);

VOS_UINT32  TAF_APS_SetPdpIdSubFsmState(
    VOS_UINT8                          ucPdpId,
    VOS_UINT32                         ulState
);

VOS_UINT32  TAF_APS_GetPdpIdSubFsmState(
    VOS_UINT8                          ucPdpId
);

VOS_VOID  TAF_APS_SaveWaitInfoInWaitL4aMsgRsltQueue(
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT16                          usClientId,
    AT_MN_MSGTYPE_ENUM_UINT16           enMsgType,
    VOS_UINT8                           ucOpId
);

TAF_APS_PDP_ENTITY_FSM_CTX_STRU* TAF_APS_GetCurrPdpEntityFsmAddr(VOS_VOID);


VOS_VOID  TAF_APS_InitCmdBufferQueue(
    TAF_APS_CMD_BUFFER_STRU            *pstCmdBufferQueue
);

TAF_APS_CMD_BUFFER_STRU  *TAF_APS_GetCmdBufferQueueAddr( VOS_VOID );

VOS_VOID  TAF_APS_DelItemInCmdBufferQueue(
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT32                          ulPara
);

VOS_VOID  TAF_APS_SaveItemInCmdBufferQueue(
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT32                         *pulMsgInfo,
    VOS_UINT32                          ulMsgInfoLen,
    VOS_UINT32                          ulPara
);

TAF_APS_CMD_BUFFER_STRU  *TAF_APS_GetItemFromCmdBufferQueue(
    TAF_APS_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT32                          ulPara
);


VOS_VOID TAF_APS_SetCurrPdpEntityFsmAddr(
    VOS_UINT8                           ucPdpId
);

VOS_VOID TAF_APS_SaveCurrSubFsmEntryMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

VOS_VOID  TAF_APS_InitMainFsmCtx(
    TAF_APS_MAIN_FSM_CTX_STRU                *pstCurrFsmCtx
);

VOS_VOID  TAF_APS_InitSubFsmCtx(
    TAF_APS_SUB_FSM_CTX_STRU                *pstCurrFsmCtx
);

VOS_VOID  TAF_APS_InitInternalBuffer(
    TAF_APS_MSG_QUEUE_STRU             *pstBufferEntryMsgQueue
);

VOS_VOID TAF_APS_QuitCurrSubFsm( VOS_VOID );

VOS_VOID TAF_APS_InitSubFsm(
    TAF_APS_FSM_ID_ENUM_UINT32          enFsmId,
    TAF_FSM_DESC_STRU                  *pstFsmDesc,
    VOS_UINT32                          ulState
);
VOS_VOID  TAF_APS_SetCurrFsmEntityPdpId(
    VOS_UINT8                           ucPdpId
);

VOS_UINT8  TAF_APS_GetCurrFsmEntityPdpId(VOS_VOID);

VOS_VOID  TAF_APS_SetCurrPdpEntitySimRegStatus(
    VOS_UINT32                          ulPsSimRegStatus
);

VOS_UINT32  TAF_APS_GetCurrPdpEntitySimRegStatus( VOS_VOID );

VOS_VOID  TAF_APS_SetCurrPdpEntityRatType(
    MMC_APS_RAT_TYPE_ENUM_UINT32                enRatType
);

MMC_APS_RAT_TYPE_ENUM_UINT32  TAF_APS_GetCurrPdpEntityRatType( VOS_VOID );

MMC_APS_RAT_TYPE_ENUM_UINT32 TAF_APS_GetPdpEntityRatType(
    VOS_UINT8                          ucPdpId
);

VOS_VOID  TAF_APS_SaveDefaultBearInfo(
    TAF_APS_EPS_BEARER_INFO_IND_STRU       *pstDefaultBearerInfo
);

TAF_APS_EPS_BEARER_INFO_IND_STRU*  TAF_APS_GetDefaultBearInfo( VOS_VOID );

VOS_VOID  TAF_APS_SetDefaultBearInfoStatus(
    VOS_UINT32                          ulDefaultBearerInfoFlg
);

TAF_APS_ENTRY_MSG_STRU* TAF_APS_GetCurrSubFsmMsgAddr(VOS_VOID);

VOS_VOID  TAF_APS_GetPsSimRegStatus(
    VOS_UINT32                          ulPsSimRegStatus
);

VOS_VOID  TAF_APS_SetPsSimRegStatus(
    VOS_UINT32                          ulPsSimRegStatus
);

TAF_APS_INTERNAL_MSG_BUF_STRU* TAF_APS_GetNextInternalMsg( VOS_VOID );
VOS_UINT32  TAF_APS_SndInternalMsg(
    VOS_VOID                           *pSndMsg
);
TAF_APS_INTERNAL_MSG_BUF_STRU *TAF_APS_GetIntMsgSendBuf(
    VOS_UINT32                          ulLen
);
VOS_VOID  TAF_APS_InitInternalMsgQueue(
    TAF_APS_INTERNAL_MSG_QUEUE_STRU     *pstInternalMsgQueue                   /* MM子层的内部消息队列 */
);
TAF_APS_CONTEXT_STRU*  TAF_APS_GetApsCtxAddr( VOS_VOID );

VOS_VOID  TAF_APS_SaveCachePsCallAnswerMsg(
    AT_MN_MSGTYPE_ENUM_UINT16           enMsgType,
    VOS_UINT8                           ucPdpid
);



TAF_APS_PDP_ACT_LIMIT_INFO_STRU* TAF_APS_GetPdpActLimitInfoAddr(VOS_VOID);
VOS_VOID TAF_APS_InitPdpActLimitInfo(VOS_VOID);



VOS_VOID TAF_APS_InitSwitchDdrInfo(VOS_VOID);
TAF_APS_SWITCH_DDR_RATE_INFO_STRU*  TAF_APS_GetSwitchDdrRateInfoAddr(VOS_VOID);

VOS_VOID TAF_APS_InitCallRemainTmrLen(VOS_VOID);
VOS_VOID TAF_APS_SetCallRemainTmrLen(
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulTmrLen
);
VOS_UINT32 TAF_APS_GetCallRemainTmrLen(VOS_UINT8 ucCid);

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
