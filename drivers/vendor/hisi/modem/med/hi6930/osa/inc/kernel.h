/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: kernel.h                                                        */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: kernel                                                       */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef  _KERNEL_H
#define  _KERNEL_H

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "v_lib.h"

/*
******************************************************************************
*                                             MISCELLANEOUS
******************************************************************************
*/

/* IDLE task priority */
#define  VOS_IDLE_PRIO       (VOS_LOWEST_PRIO)

/* Size of event table  */
#define  VOS_EVENT_TBL_SIZE (((VOS_LOWEST_PRIO) / 8) + 1)

/* Size of ready table  */
#define  VOS_RDY_TBL_SIZE   (((VOS_LOWEST_PRIO) / 8) + 1)


/*
******************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
******************************************************************************
*/
#define  VOS_STAT_RDY            0x00        /* Ready to run */
#define  VOS_STAT_SEM            0x01        /* Pending on semaphore */
#define  VOS_STAT_SUSPEND        0x08        /* Task is suspended */

/*
*******************************************************************************
*                                        OS_EVENT types
*******************************************************************************
*/
#define  VOS_EVENT_TYPE_UNUSED     0
#define  VOS_EVENT_TYPE_SEM        3

/*
******************************************************************************
*                                          type
******************************************************************************
*/
typedef struct {
    VOS_VOID   *OSEventPtr; /* Pointer to message or queue structure   */
    VOS_UINT16 OSEventCnt; /* Semaphore Count (not used if other EVENT type) */
    VOS_UINT8  OSEventType;/* Type of event control block (see OS_EVENT_TYPE_???)*/
    VOS_UINT8  OSEventGrp; /* Group corresponding to tasks waiting for event to occur*/
    /* List of tasks waiting for event to occur */
    VOS_UINT8  OSEventTbl[VOS_EVENT_TBL_SIZE];
} VOS_EVENT;

typedef struct os_tcb {
    VOS_STK       *OSTCBStkPtr; /* Pointer to current top of stack  */
    struct os_tcb *OSTCBNext;/* Pointer to next     TCB in the TCB list  */
    struct os_tcb *OSTCBPrev;/* Pointer to previous TCB in the TCB list  */
    VOS_EVENT     *OSTCBEventPtr; /* Pointer to event control block  */
    VOS_UINT32    StackAddress;
    /*VOS_UINT32    StackSize;*/ /* economize text*/
    VOS_TASK_ENTRY_TYPE Function;/* the entry of task */
    VOS_UINT32    Args[VOS_TARG_NUM];/* the argument of the entry */
    VOS_UINT16    OSTCBDly;/* ticks to delay task or, timeout waiting for event*/
    VOS_UINT8     OSTCBStat; /* Task status  */
    VOS_UINT8     OSTCBPrio; /* Task priority (0 == highest, 63 == lowest) */
    VOS_UINT8     OSTCBX;/* Bit position in group  corresponding to task priority*/
    VOS_UINT8     OSTCBY;/* Index into ready table corresponding to task priority */
    VOS_UINT8     OSTCBBitX;/* Bit mask to access bit position in ready table */
    VOS_UINT8     OSTCBBitY; /* Bit mask to access bit position in ready group */
    VOS_UINT16    Reserved;/* aligned */
} VOS_TCB;


typedef struct os_taskSwitch
{
    VOS_UINT16    usPrioCru;                       /* 当前任务的优先级 */
    VOS_UINT16    usPrioNext;                      /* 切换后任务的优先级 */
    VOS_UINT32    ulSwitchSlice;                   /* 切换时的slice */
} VOS_TaskSwitch;

/*
******************************************************************************
*                                            GLOBAL VARIABLES
******************************************************************************
*/
extern VOS_UINT32 g_ulVosCtxSwCtr;

extern VOS_EVENT *g_pstVosEventFreeList;
extern VOS_EVENT g_astVosEventTbl[VOS_MAX_EVENTS];


extern VOS_UINT8 g_ucVosIntNesting;
extern VOS_UINT8 g_ucVosIntExitY;

extern VOS_UINT8 g_ucVosLockNesting;

extern VOS_UINT8 g_ucVosPrioCur;
extern VOS_UINT8 g_ucVosPrioHighRdy;

extern VOS_UINT8 g_ucVosRdyGrp;
extern VOS_UINT8 g_aucVosRdyTbl[VOS_RDY_TBL_SIZE];

extern VOS_BOOL g_bVosRunning;

extern VOS_UINT8 g_ucVosTaskCtr;

extern volatile VOS_UINT32  g_ulVosIdleCtr;

extern VOS_UINT8 g_stVosTaskIdleStk[VOS_TASK_IDLE_STK_SIZE];


extern VOS_TCB *g_pstVosTCBCur;
extern VOS_TCB *g_pstVosTCBFreeList;
extern VOS_TCB *g_pstVosTCBHighRdy;
extern VOS_TCB *g_pstVosTCBList;
extern VOS_TCB *g_pastVosTCBPrioTbl[VOS_LOWEST_PRIO + 1];
extern VOS_TCB g_stVosTCBTbl[VOS_MAX_TASKS];

extern volatile VOS_UINT32  g_ulVosTicks;

extern VOS_UINT8  const g_ucVosMapTbl[];
extern VOS_UINT8  const g_ucVosUnMapTbl[];


/*
******************************************************************************
*                       FUNCTION PROTOTYPES
******************************************************************************
*/
VOS_EVENT *VOSSemCreate(VOS_UINT16 cnt);

VOS_VOID VOSSemPend(VOS_EVENT *pevent, VOS_UINT16 timeout, VOS_UINT8 *err);

VOS_UINT8 VOSSemPost(VOS_EVENT *pevent);

VOS_VOID VOSSemReset(VOS_EVENT *pevent);

VOS_UINT8 VOSTaskCreate(VOS_VOID (*task)(VOS_UINT32 d), VOS_UINT32 data,
                        VOS_STK *ptos, VOS_UINT8 prio);

VOS_UINT8 VOSTaskReset(VOS_VOID (*task)(VOS_UINT32 d), VOS_UINT32 data,
                        VOS_STK *ptos, VOS_UINT8 prio);

VOS_VOID VOSTimeTick(VOS_VOID);

VOS_VOID VOSIntEnter(VOS_VOID);

VOS_VOID VOSIntExit(VOS_VOID);

VOS_UINT8 VOS_EventTaskRdy(VOS_EVENT *pevent, VOS_VOID *msg, VOS_UINT8 msk);

VOS_VOID VOS_EventTaskWait(VOS_EVENT *pevent);

VOS_VOID VOS_EventTO(VOS_EVENT *pevent);

VOS_VOID VOS_EventWaitListInit(VOS_EVENT *pevent);

VOS_VOID VOS_Sched(VOS_VOID);

VOS_VOID VOS_TaskIdle(VOS_UINT32 data);

VOS_UINT8 VOS_TCBInit(VOS_UINT8 prio, VOS_STK *ptos, VOS_STK *pbos, VOS_UINT16 id,
                 VOS_UINT32 stk_size, VOS_VOID *pext, VOS_UINT16 opt);

VOS_UINT8 VOS_TCBReset(VOS_UINT8 prio, VOS_STK *ptos, VOS_STK *pbos, VOS_UINT16 id,
                 VOS_UINT32 stk_size, VOS_VOID *pext, VOS_UINT16 opt);

VOS_VOID VOSIntCtxSw(VOS_VOID);

VOS_VOID VOSStartHighRdy(VOS_VOID);

VOS_STK *VOS_TaskStkInit(VOS_VOID (*task)(VOS_UINT32 d), VOS_UINT32 data,
                       VOS_STK *ptos, VOS_UINT16 opt);

VOS_VOID VOSCtxSw(VOS_VOID);

VOS_VOID VOSTickISR(VOS_VOID);

VOS_VOID VOS_StartTick(VOS_VOID);

VOS_UINT32 VOS_SmCCreate( VOS_CHAR   acSmName[4],
                          VOS_UINT32 ulSmInit,
                          VOS_UINT32 ulFlags,
                          VOS_UINT32 *pulSmID );
VOS_UINT32 VOS_SmP( VOS_UINT32 Sm_ID, VOS_UINT32 ulTimeOutInMillSec );
VOS_UINT32 VOS_SmV( VOS_UINT32 Sm_ID );
VOS_VOID VOS_SmReset(VOS_UINT32 Sm_ID, VOS_UINT32 ulSmInit);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _KERNEL_H */

