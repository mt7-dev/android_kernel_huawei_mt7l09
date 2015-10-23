
#ifndef  TAF_MMA_TIMER_MGMT_PROC_H
#define  TAF_MMA_TIMER_MGMT_PROC_H

#pragma pack(4)

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "MnComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TAF_MMA_MAX_TIMER_NUM                       (30)

/* 等待PB的文件刷新指示定时器 */
#define TI_TAF_MMA_WAIT_PIH_USIM_STATUS_IND_LEN                  (10*1000)

/* 等待IMSA的开机回复定时器 */
#define TI_TAF_MMA_WAIT_IMSA_START_CNF_LEN                       (3*1000)

/* 等待IMSA的关机回复定时器 */
#define TI_TAF_MMA_WAIT_IMSA_POWER_OFF_CNF_LEN                   (3*1000)

/* 等待MMC的开机回复定时器 */
#define TI_TAF_MMA_WAIT_MMC_START_CNF_LEN                        (70*1000)

/* 等待MMC的关机回复定时器 */
#define TI_TAF_MMA_WAIT_MMC_POWER_OFF_CNF_LEN                    (60*1000)

#define TI_TAF_MMA_WAIT_MMC_ACQ_CNF_LEN                          (180*1000)

/* 等待MMC的REG_CNF定时器 */
#define TI_TAF_MMA_WAIT_MMC_REG_CNF_LEN                          (1200*1000)

/* 等待MMC的POWER_SAVE_CNF定时器 */
#define TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF_LEN                   (10*1000)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum TAF_MMA_TIMER_ID_ENUM
{
    /* 等待PB的文件刷新指示定时器 */
    TI_TAF_MMA_WAIT_PIH_USIM_STATUS_IND                  = MN_TIMER_CLASS_MMA ,

    /* 等待IMSA的开机回复定时器 */
    TI_TAF_MMA_WAIT_IMSA_START_CNF                       ,

    /* 等待IMSA的关机回复定时器 */
    TI_TAF_MMA_WAIT_IMSA_POWER_OFF_CNF                   ,

    /* 等待MMC的开机回复定时器 */
    TI_TAF_MMA_WAIT_MMC_START_CNF                        ,

    /* 等待MMC的关机回复定时器 */
    TI_TAF_MMA_WAIT_MMC_POWER_OFF_CNF                    ,

    /* 等待IMSA的IMS VOICE CAP消息的保护定时器 */
    TI_TAF_MMA_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY,

    /* 等待MMC的MMC_ACQ_CNF定时器 */
    TI_TAF_MMA_WAIT_MMC_ACQ_CNF                         ,

    /* 等待MMC的REG_CNF定时器 */
    TI_TAF_MMA_WAIT_MMC_REG_CNF                         ,

    /* 等待MMC的POWER_SAVE_CNF定时器 */
    TI_TAF_MMA_WAIT_MMC_POWER_SAVE_CNF                  ,


    TI_TAF_MMA_TIMER_BUTT
};
typedef VOS_UINT32  TAF_MMA_TIMER_ID_ENUM_UINT32;
enum TAF_MMA_TIMER_STATUS_ENUM
{
    TAF_MMA_TIMER_STATUS_STOP,              /* 定时器停止状态 */
    TAF_MMA_TIMER_STATUS_RUNING,            /* 定时器运行状态 */
    TAF_MMA_TIMER_STATUS_BUTT
};
typedef VOS_UINT8 TAF_MMA_TIMER_STATUS_ENUM_UINT8;

/*****************************************************************************
  3 结构声明
*****************************************************************************/


typedef struct
{
    HTIMER                              hTimer;                                 /* 定时器的运行指针 */
    TAF_MMA_TIMER_ID_ENUM_UINT32        enTimerId;                              /* 定时器的ID */
    TAF_MMA_TIMER_STATUS_ENUM_UINT8     enTimerStatus;                          /* 定时器的运行状态,启动或停止 */
    VOS_UINT8                           aucReserve[3];
} TAF_MMA_TIMER_CTX_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头                                   */ /*_H2ASN_Skip*/
    TAF_MMA_TIMER_STATUS_ENUM_UINT8     enTimerStatus;
    VOS_UINT8                           aucReserve[3];
    VOS_UINT32                          ulLen;              /* 定时器时长 */
}TAF_MMA_TIMER_INFO_STRU;

/*****************************************************************************
  4 函数声明
*****************************************************************************/
VOS_VOID  TAF_MMA_StartTimer(
    TAF_MMA_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT32                          ulLen
);

VOS_VOID  TAF_MMA_StopTimer(
    TAF_MMA_TIMER_ID_ENUM_UINT32        enTimerId
);

TAF_MMA_TIMER_STATUS_ENUM_UINT8  TAF_MMA_GetTimerStatus(
    TAF_MMA_TIMER_ID_ENUM_UINT32        enTimerId
);

VOS_VOID  TAF_MMA_SndOmTimerStatus(
    TAF_MMA_TIMER_STATUS_ENUM_UINT8     enTimerStatus,
    TAF_MMA_TIMER_ID_ENUM_UINT32        enTimerId,
    VOS_UINT32                          ulLen
);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* TAF_MMA_TIMER_MGMT_H */


