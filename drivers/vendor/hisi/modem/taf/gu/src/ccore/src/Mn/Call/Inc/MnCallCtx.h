
#ifndef _MN_CALL_CTX_H_
#define _MN_CALL_CTX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include  "MnCallApi.h"
#include  "NasCcAirMsg.h"

#include "NasMncc.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  1 全局变量定义
*****************************************************************************/

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define  MN_CALL_MAX_SS_CMD_KEY_EVT_NUM    (10)                                  /* 补充业务操作管理结构 */

#define  MN_CALL_ENTITY_MAX_NUM            (1)                                  /*CALL模块支持的实体数，目前只有一个*/
#define  MN_CALL_INVALID_CATEGORY          (128)                                /* 用户定制的紧急呼号码最大条数 */

#define MN_CALL_ONE_THOUSAND_MILLISECOND                    (1000)              /* 1000MS */



/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 补充业务后续操作 */
typedef enum
{
    MN_CALL_SS_SUBSEQ_OP_RPT_SUCC,                                              /* 向应用层上报操作成功 */
    MN_CALL_SS_SUBSEQ_OP_RPT_FAIL,                                              /* 向应用层上报操作失败 */
    MN_CALL_SS_SUBSEQ_OP_ACCPT_OTH                                              /* 接听另一个呼叫 */
} MN_CALL_SS_SUBSEQ_OP_ENUM;

/* 补充业务后续操作 */
typedef enum
{
    MN_CALL_CTRL_NULL,                                                          /* 向应用层上报操作成功 */
    MN_CALL_CTRL_PROCEEDING,                                                    /* 向应用层上报操作失败 */
    MN_CALL_CTRL_END,                                                           /* 接听另一个呼叫 */
    MN_CALL_CTRL_MAX
} MN_CALL_CTRL_STATE_ENUM;
typedef TAF_UINT32    MN_CALL_CTRL_STATE_ENUM_U32;

/* 补充业务进展事件 */
typedef enum
{
    MN_CALL_SS_PROG_EVT_REL,                                                    /* 有呼叫被挂断 */
    MN_CALL_SS_PROG_EVT_SETUP_COMPL,                                            /* 来电接听完成 */
    MN_CALL_SS_PROG_EVT_HOLD_CNF,                                               /* 呼叫保持成功 */
    MN_CALL_SS_PROG_EVT_HOLD_REJ,                                               /* 呼叫保持被拒绝 */
    MN_CALL_SS_PROG_EVT_RETRIEVE_CNF,                                           /* 呼叫恢复成功 */
    MN_CALL_SS_PROG_EVT_RETRIEVE_REJ,                                           /* 呼叫恢复被拒绝 */
    MN_CALL_SS_PROG_EVT_BUILD_MPTY_CNF,                                         /* 建立MPTY成功 */
    MN_CALL_SS_PROG_EVT_BUILD_MPTY_REJ,                                         /* 建立MPTY被拒绝 */
    MN_CALL_SS_PROG_EVT_SPLIT_MPTY_CNF,                                         /* 分离MPTY成功 */
    MN_CALL_SS_PROG_EVT_SPLIT_MPTY_REJ,                                         /* 分离MPTY被拒绝 */
    MN_CALL_SS_PROG_EVT_DEFLECT_CNF,                                            /* 呼叫偏转成功 */
    MN_CALL_SS_PROG_EVT_DEFLECT_REJ,                                            /* 呼叫偏转被拒绝 */
    MN_CALL_SS_PROG_EVT_ACT_CCBS_CNF,                                           /* 激活CCBS成功 */
    MN_CALL_SS_PROG_EVT_ACT_CCBS_REJ,                                           /* 激活CCBS被拒绝 */
    MN_CALL_SS_PROG_EVT_ECT_CNF,                                                /* ECT操作成功 */
    MN_CALL_SS_PROG_EVT_ECT_REJ,                                                /* ECT操作被拒绝 */
    MN_CALL_SS_PROG_EVT_MAX
} MN_CALL_SS_PROGRESS_EVT_ENUM;
enum MN_CALL_SRVCC_STATE_ENUM
{
    MN_CALL_SRVCC_STATE_START,
    MN_CALL_SRVCC_STATE_SUCCESS,
    MN_CALL_SRVCC_STATE_FAIL,
    
    MN_CALL_SRVCC_STATE_BUTT
};
typedef VOS_UINT8 MN_CALL_SRVCC_STATE_ENUM_UINT8;

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



typedef struct
{
    VOS_UINT8                           ucCcbsSupportFlg;                       /*CCBS(遇忙呼叫完成)业务*/
    
    VOS_UINT8                           ucCallDeflectionSupportFlg;             /*支持呼叫偏转业务 */
    VOS_UINT8                           ucAlsSupportFlg;                        /*支持线路切换业务*/

    VOS_UINT8                           ucReportEccNumFlg;

    VOS_UINT8                           ucVoiceCallNotSupportedCause;
    VOS_UINT8                           ucVideoCallNotSupportedCause;
    VOS_UINT8                           aucReserve[2];

    MN_CALL_REDIAL_CFG_STRU             stCallRedialCfg;
}MN_CALL_CUSTOM_CFG_INFO_STRU;
typedef struct
{
    MN_CALL_CUSTOM_CFG_INFO_STRU        stCustomCfg;                            /* NVIM中的定制信息 */
}MN_CALL_MS_CFG_INFO_STRU;
typedef struct
{
    VOS_BOOL                            bUsed;                                  /* 是否已被使用 */
    VOS_UINT32                          ulTi;                                   /* 该呼叫对应的CC实体的Ti */
    MN_CALL_CLIR_CFG_ENUM_U8            enClirCfg;                              /* 主叫号码限制的配置 */
    VOS_UINT8                           aucReserved[3];
    MN_CALL_CUG_CFG_STRU                stCugCfg;                               /* CUG的配置 */
    VOS_BOOL                            bRbSet;
    VOS_UINT32                          ulRbId;
    VOS_BOOL                            bVpChannelOpenFlg;                      /*记录呼叫类型为可视电话情况下业务信道是否已打开*/
    MN_CALL_INFO_STRU                   stCallInfo;                             /* 呼叫信息 */
} MN_CALL_MGMT_STRU;


typedef struct
{
    MN_CALL_ID_T                        callId;                                 /* 产生事件的呼叫ID */
    VOS_UINT8                           aucReserved[3];
    MN_CALL_SS_PROGRESS_EVT_ENUM        enEvt;                                  /* 事件的类型 */
    MN_CALL_SS_SUBSEQ_OP_ENUM           enSubseqOp;                             /* 收到该事件的后续操作 */
} MN_CALL_SUPS_CMD_ENUM_U8_KEY_EVT_STRU;
typedef struct
{
    VOS_UINT8                               bInProgress;                        /* 是否有操作正在进行 */
    MN_CALL_SUPS_CMD_ENUM_U8                enCallSupsCmd;                      /* 当前的操作 */
    MN_CLIENT_ID_T                          clientId;                           /* 发起操作的Client的ID */
    MN_OPERATION_ID_T                       opId;
    VOS_UINT8                               ucNumOfKeyEvt;                      /* 关注的事件个数 */
    VOS_UINT8                               ucInvokeId;
    VOS_UINT8                               aucReserved[1];
    MN_CALL_SUPS_CMD_ENUM_U8_KEY_EVT_STRU   astKeyEvts[MN_CALL_MAX_SS_CMD_KEY_EVT_NUM];  /* 关注的事件 */
} MN_CALL_SUPS_CMD_ENUM_U8_MGMT_STRU;
typedef struct
{
    VOS_UINT32                          bitOpBufferedSetupMsg : 1;
    VOS_UINT32                          bitOpBufferedRejIndMsg : 1;
    VOS_UINT32                          bitSpare : 30;
    MNCC_REQ_PRIM_MSG_STRU              stBufferedSetupMsg;
    MNCC_IND_PRIM_MSG_STRU              stBufferedRejIndMsg;
} MN_CALL_MSG_BUFF_STRU;




typedef struct
{
    MN_CALL_MGMT_STRU                         stCallMgmtInfo;                   /* 呼叫管理信息 */
    MN_CALL_SUPS_CMD_ENUM_U8_KEY_EVT_STRU     stSimInfo;                        /*被关注的补充业务操作事件*/
    MN_CALL_MS_CFG_INFO_STRU                  stMsCfgInfo;                      /* MS的配置信息 */
    MN_CALL_SUPS_CMD_ENUM_U8_MGMT_STRU        stNetworkInfo;                    /* 补充业务管理结构*/

    MN_CALL_MSG_BUFF_STRU                     stMnCallBufferedMsg;              /* 呼叫重建功能需要缓冲普通呼叫或紧急呼叫set up消息 */

    MN_CALL_SRVCC_STATE_ENUM_UINT8            enSrvccState;
}MN_CALL_CONTEXT_STRU;

/* 任意点回放导出全局变量使用*/
typedef struct
{
    MN_CALL_MGMT_STRU                   pc_g_f_astCallMgmtEntities[MN_CALL_MAX_NUM];
    MN_CALL_SUPS_CMD_ENUM_U8_MGMT_STRU  pc_g_f_stCallSupsCmdMgmt;

    /*CCBS功能打开时,保留的Setup消息 */
    NAS_CC_MSG_SETUP_MO_STRU            pc_g_f_stCcbsSetup;

    MN_CALL_ALS_LINE_NO_ENUM_U8         pc_g_f_enAlsLineNo;
    VOS_UINT8                           pc_g_gucTafCallStatusControl;
    VOS_UINT8                           pc_g_gucTafMultiSimCallStatusControl;
    VOS_UINT8                           pc_g_g_enVpNvCfgState;
} NAS_CALL_OUTSIDE_RUNNING_CONTEXT_ST;

typedef struct
{
    MN_CALL_UUS1_MSG_TYPE_ENUM_U32      enCallMsgType;
    NAS_CC_MSG_TYPE_ENUM_U8             enCcMsgType;
}MN_CALL_MATCH_MSG_TYPE_STRU;


/*****************************************************************************
 Structure      : NAS_CC_SDT_MSG_ST
 Description    : PC回放工程，CC相关的全局变量通过以下消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                        MsgHeader;                                          /* 消息头                                   */ /*_H2ASN_Skip*/
    NAS_CALL_OUTSIDE_RUNNING_CONTEXT_ST    stOutsideCtx;
}NAS_CALL_SDT_MSG_ST;




/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/


MN_CALL_CUSTOM_CFG_INFO_STRU* MN_CALL_GetCustomCfgInfo( VOS_VOID );

VOS_UINT32 MN_CALL_GetCallRedialSupportFlg(VOS_VOID);
MN_CALL_MSG_BUFF_STRU*  MN_CALL_GetBufferedMsg( VOS_VOID );
VOS_VOID MN_CALL_ClearBufferedMsg(VOS_VOID);
VOS_VOID MN_CALL_InitCtx(VOS_VOID);


#if (FEATURE_ON == FEATURE_IMS)
VOS_VOID TAF_CALL_SetSrvccState(MN_CALL_SRVCC_STATE_ENUM_UINT8 enSrvccState);

MN_CALL_SRVCC_STATE_ENUM_UINT8 TAF_CALL_GetSrvccState(VOS_VOID);
#endif

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

#endif /* end of MnCallCtx.h */

