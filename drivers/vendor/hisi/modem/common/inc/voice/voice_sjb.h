

#ifndef __VOICE_SJB_H__
#define __VOICE_SJB_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "ImsCodecInterface.h"
#include "voice_jb_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define VOICE_SJB_RX_DEPTH               (15)                         /* 下行JB的深度 */
#define VOICE_SJB_TX_DEPTH               (3)                          /* 上行JB的深度 */
#define VOICE_SJB_INIT_DEPTH             (4)                          /* 初始化的深度 */
#define VOICE_SJB_UINT32_MAX             (0xFFFFFFFF)                 /* UINT32的最大值 */
#define VOICE_SJB_20MSTS_WB              (320)                        /* 16K采样率20ms对应的TS */
#define VOICE_SJB_20MSTS_NB              (160)                        /* 8K采样率20ms对应的TS */
#define VOICE_SJB_MaxTSDiff              (48*20*5)                    /* 48K采样率，20ms,5帧的TS */
#define VOICE_SJB_GetRxSjbCtrl()         (&g_stRxSjbManager)          /* 下行JB控制块 */
#define VOICE_SJB_GetTxSjbCtrl()         (&g_stTxSjbManager)          /* 上行JB控制块 */

#define VOICE_SJB_TSErrCntMax            (5)                          /* 连续因TS过旧丢包数超过
                                                                        该值，就初始化JB */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum VOICE_SJB_ITEM_STATUS_ENUM
{
    VOICE_SJB_ITEM_STATUS_EMPTY            = 0,                       /* 数组中元素为空 */
    VOICE_SJB_ITEM_STATUS_NOTEMPTY,                                   /* 数组中元素不空 */
    VOICE_SJB_ITEM_STATUS_BUTT
};
typedef VOS_UINT32 VOICE_SJB_ITEM_STATUS_ENUM_UINT32;

/* JB是否已经初始化完全 */
enum VOICE_SJB_INIT_STATUS_ENUM
{
    VOICE_SJB_INIT_STATUS_NOT              = 0,                       /* 初始化未完成 */
    VOICE_SJB_INIT_STATUS_DONE,                                       /* 初始化完成 */
    VOICE_SJB_INIT_STATUS_BUTT
};
typedef VOS_UINT32 VOICE_SJB_INIT_STATUS_ENUM_UINT32;
/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  5 消息定义
*****************************************************************************/

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : VOICE_SJB_PKT_RX_STRU
 功能描述  : 下行数据包
*****************************************************************************/
typedef struct
{
    IMSA_VOICE_RX_DATA_IND_STRU       stPktData;                   /* 消息内容 */

    VOS_VOID                         *pstPrev;                     /* 上一帧地址 */
    VOS_VOID                         *pstNext;                     /* 下一帧地址 */
    VOICE_SJB_ITEM_STATUS_ENUM_UINT32  enIsNotEmpty;               /* 这个元素中没有数据 */
} VOICE_SJB_PKT_RX_STRU;

/*****************************************************************************
 实体名称  : VOICE_SJB_PKT_TX_STRU
 功能描述  : 上行数据包
*****************************************************************************/
typedef struct
{
    VOICE_IMSA_TX_DATA_IND_STRU       stPktData;                   /* 消息内容 */

    VOS_VOID                         *pstPrev;                     /* 上一帧地址 */
    VOS_VOID                         *pstNext;                     /* 下一帧地址 */
    VOICE_SJB_ITEM_STATUS_ENUM_UINT32  enIsNotEmpty;               /* 这个元素中没有数据 */
} VOICE_SJB_PKT_TX_STRU;

/*****************************************************************************
 实体名称  : VOICE_SJB_RX_BUF_CTRL_STRU
 功能描述  : 下行SJB的数组
*****************************************************************************/
typedef struct
{
    VOICE_SJB_PKT_RX_STRU              stPkts[VOICE_SJB_RX_DEPTH]; /* 数据包数组 */
    VOICE_SJB_PKT_RX_STRU             *pstHead;                    /* 队首地址 */
    VOICE_SJB_PKT_RX_STRU             *pstTail;                    /* 队尾地址 */
} VOICE_SJB_RX_BUF_CTRL_STRU;

/*****************************************************************************
 实体名称  : VOICE_SJB_TX_BUF_CTRL_STRU
 功能描述  : 上行SJB的数组
*****************************************************************************/
typedef struct
{
    VOICE_SJB_PKT_TX_STRU              stPkts[VOICE_SJB_TX_DEPTH]; /* 数据包数组 */
    VOICE_SJB_PKT_TX_STRU             *pstHead;                    /* 队首地址 */
    VOICE_SJB_PKT_TX_STRU             *pstTail;                    /* 队尾地址 */
} VOICE_SJB_TX_BUF_CTRL_STRU;

/*****************************************************************************
 实体名称  : VOICE_SJB_STATE
 功能描述  : SJB的状态量数组
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        uwTSof20ms;                  /* 20ms对应的TS值 */
    VOICE_SJB_INIT_STATUS_ENUM_UINT32  enAlreadyInit;               /* 初始化完成标志 */
} VOICE_SJB_STATE;

/*****************************************************************************
 实体名称  : VOICE_SJB_PARA
 功能描述  : SJB的统计量
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        uwLastTS;                    /* 最近的TS */
    VOS_UINT32                        uwCurPktNum;                 /* 当前数组中包数 */
    VOS_UINT32                        uwRcvPktNum;                 /* 收到的包数 */
    VOS_UINT32                        uwLostByStore;               /* 存包时发现的丢包数 */
    VOS_UINT32                        uwLostByGet;                 /* 取包时发现的丢包 */
    VOS_UINT32                        uwTSErrCnt;                  /* 因TS过旧而被连续丢的包数 */
    VOS_UINT32                        uwGetErrCnt;                 /* 连续取包失败次数 */
} VOICE_SJB_PARA;

/*****************************************************************************
 实体名称  : VOICE_SJB_RX_MANAGER_STRU
 功能描述  : 下行JitterBuffer控制块
*****************************************************************************/
typedef struct
{
    VOICE_SJB_RX_BUF_CTRL_STRU         stBuff;                      /* buff控制块 */
    VOICE_SJB_STATE                    stState;                     /* 状态量 */
    VOICE_SJB_PARA                     stPara;                      /* 统计量 */
} VOICE_SJB_RX_MANAGER_STRU;

/*****************************************************************************
 实体名称  : VOICE_SJB_TX_MANAGER_STRU
 功能描述  : 上行JitterBuffer控制块
*****************************************************************************/
typedef struct
{
    VOICE_SJB_TX_BUF_CTRL_STRU         stBuff;                       /* buff控制块 */
    VOICE_SJB_STATE                    stState;                      /* 状态量 */
    VOICE_SJB_PARA                     stPara;                       /* 统计量 */
    VOICE_JB_PKT_TIME_ENUM_UINT32      enPktTime;                    /* 发包时长 */
} VOICE_SJB_TX_MANAGER_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_SJB_RX_MANAGER_STRU       g_stRxSjbManager;
extern VOICE_SJB_TX_MANAGER_STRU       g_stTxSjbManager;


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID VOICE_SJB_InitTx( VOICE_SJB_TX_MANAGER_STRU *pstSjbManager);
extern VOS_UINT32 VOICE_SJB_StorePacketTx( VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt);
extern VOS_UINT32 VOICE_SJB_RemoveTailTx( VOICE_SJB_TX_MANAGER_STRU  *pstSjbManager);
extern VOS_UINT32 VOICE_SJB_InsertPktTx(
                VOICE_SJB_TX_MANAGER_STRU               *pstSjbManager,
                VOICE_SJB_PKT_TX_STRU                   *pstPktToStore,
                VOICE_SJB_PKT_TX_STRU                   *pstPktPrev,
                VOICE_SJB_PKT_TX_STRU                   *pstPktNext );
extern VOICE_SJB_PKT_TX_STRU* VOICE_SJB_FindEmptyPtrTx( VOICE_SJB_TX_MANAGER_STRU *pstSjbManager );
extern VOS_UINT32 VOICE_SJB_GetPacketTx( VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt);

extern VOS_VOID VOICE_SJB_InitRx( VOICE_SJB_RX_MANAGER_STRU *pstSjbManager);
extern VOS_UINT32 VOICE_SJB_StorePacketRx( VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt);
extern VOS_UINT32 VOICE_SJB_RemoveTailRx( VOICE_SJB_RX_MANAGER_STRU *pstSjbManager);
extern VOS_UINT32 VOICE_SJB_InsertPktRx(
                VOICE_SJB_RX_MANAGER_STRU               *pstSjbManager,
                VOICE_SJB_PKT_RX_STRU                   *pstPktToStore,
                VOICE_SJB_PKT_RX_STRU                   *pstPktPrev,
                VOICE_SJB_PKT_RX_STRU                   *pstPktNext );
extern VOICE_SJB_PKT_RX_STRU* VOICE_SJB_FindEmptyPtrRx( VOICE_SJB_RX_MANAGER_STRU *pstSjbManager );
extern VOS_UINT32 VOICE_SJB_GetPacketRx( VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt);
extern VOS_UINT32 VOICE_SJB_GetPktNumTx( VOICE_SJB_TX_MANAGER_STRU* pstSjbManager );
extern VOS_UINT32 VOICE_SJB_TSCompare( VOS_UINT32 uwSrc, VOS_UINT32 uwDest );
extern VOS_UINT32 VOICE_SJB_SetPktTimeTx(
                VOICE_SJB_TX_MANAGER_STRU               *pstSjbManager,
                VOICE_JB_PKT_TIME_ENUM_UINT32            enPktTime);


#ifdef __cplusplus
}
#endif

#endif


