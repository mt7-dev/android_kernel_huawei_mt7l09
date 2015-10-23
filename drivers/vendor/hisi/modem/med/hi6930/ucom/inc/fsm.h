/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fsm.h
  版 本 号   : 初稿
  作    者   : 邵海刚 45755
  生成日期   : 2009年7月10日
  最近修改   :
  功能描述   : fsm.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2009年7月10日
    作    者   : 邵海刚 45755
    修改内容   : 创建文件

******************************************************************************/

#ifndef __FSM_H__
#define __FSM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 状态机定义宏 */
#define FSM_STA_TBL_ITEM(enMainState, enSubState, astActTbl)\
        {\
            ((((VOS_UINT32)(enMainState)) << 16) | ((VOS_UINT16)(enSubState))),\
            ((sizeof(astActTbl))/sizeof(ACT_STRU)),\
            (astActTbl)\
        }

#define FSM_ACT_TBL_ITEM(PID, enMsgType, pActFun)\
        {\
            ((((VOS_UINT32)(PID)) << 16) | ((VOS_UINT16)(enMsgType))),\
            ((pActFun))\
        }

#define DSP_MAX_STACK_DEPTH                             (8)                     /* 状态机栈的深度 */
/*FSM 结构体最大空间,包括STA_STRU/ACT_STRU*/
#define FSM_COMP_LENGTH_MAX                             (12)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/* 定义异常函数原型 */
typedef VOS_UINT32 ( * EXCEP_FUN )
(
    VOS_UINT32, /* EVENT ID,包括PID和MSG ID */
    VOS_VOID *  /* 消息指针 */
);

/* 动作处理函数的类型定义 */
typedef VOS_UINT32 ( * ACTION_FUN )
(
    VOS_VOID *  /* 消息指针 */
);

/* 动作表结构 */
typedef struct ACT
{
    VOS_UINT32      ulEventType;   /* 事件类型 */
    ACTION_FUN      pfActionFun;   /* _H2ASN_Replace VOS_UINT32 pfActionFun; */ /* 动作函数 */
}  ACT_STRU;


/* 状态转移表结构 */
typedef struct STA
{
    VOS_UINT32       ulState;       /* 状态                                */
    VOS_UINT32       ulSize;        /* 动作表的大小字节数除以ACT结构的大小 */
    ACT_STRU        *pActTable;     /* _H2ASN_Replace VOS_UINT32 pActTable; */ /* 动作表的基地址                      */
}  STA_STRU;

/* 有限状态机描述符结构 */
typedef struct FSM_DESC
{
    STA_STRU*       pStaTable;                /* 状态转移表的基地址     */
    VOS_UINT32      ulSize;                   /* 状态转移表的大小       */
    EXCEP_FUN       pfExceptHandle;           /* _H2ASN_Replace VOS_UINT32 pfExceptHandle; */ /* 用于处理异常的回调函数 */
}  FSM_DESC_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
typedef VOS_INT16 (* FSM_COMP_FUNC)(const VOS_VOID*, const VOS_VOID*);


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_VOID FSM_Swap(
                        VOS_INT16           *pshwTarget,
                        VOS_INT16           *pshwSource,
                        VOS_UINT32           uwLength );
extern VOS_VOID * FSM_Bsearch(
                                    VOS_INT16      *pKey,
                                    VOS_UINT8      *pBase,
                                    VOS_UINT32      uwNum,
                        VOS_UINT32 uwWidth,
                        FSM_COMP_FUNC fCompareFunc);

extern VOS_INT16 FSM_ActCompare( const VOS_VOID *arg1, const VOS_VOID *arg2 );
extern ACTION_FUN FSM_FindAct( FSM_DESC_STRU  *pFsmDesc,
                                    VOS_UINT32      uwState,
                                    VOS_UINT32      uwEventType);
extern VOS_UINT32 FSM_ProcessEvent(
                           FSM_DESC_STRU                   *pFsmDesc,
                           VOS_UINT32                       uwCurState,
                           VOS_UINT32                       uwEventType,
                           VOS_UINT16                       uhwMsgID,
                           VOS_VOID                        *pRcvMsg );
extern VOS_UINT32 FSM_RegisterFsm( FSM_DESC_STRU   *pstFsmDesc,
                                         VOS_UINT32       uwSize,
                                         STA_STRU        *pstStaTable,
                                         EXCEP_FUN        pfExceptHandle );
extern VOS_VOID FSM_Sort(
                             VOS_UINT8            *puhwTable,
                             VOS_UINT32            uwNum,
                             VOS_UINT32            uwWidth,
                             FSM_COMP_FUNC         fCompareFunc);
extern VOS_INT16 FSM_StaCompare( const VOS_VOID *arg1, const VOS_VOID *arg2 );
extern VOS_VOID FSM_StaSort(STA_STRU* pstTable, VOS_UINT32 uwSize);







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of fsm.h */
