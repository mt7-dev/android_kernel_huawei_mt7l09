/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: kernel.c                                                        */
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

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "kernel.h"
#include "v_int.h"

#define    THIS_FILE_ID        FILE_ID_KERNEL_C
#if VOS_STK_CHK_EN
#include "vos_id.h"
#endif

#include "ucom_comm.h"
#include "cpu_c.h"
#include "venus_stub.h"

#define  VOS_TASK_SW()  VOSCtxSw()



/* Counter of number of context switches */
VOS_UINT32  g_ulVosCtxSwCtr;

/* Pointer to list of free EVENT control blocks */
VOS_EVENT *g_pstVosEventFreeList;

/* Table of EVENT control blocks */
VOS_EVENT g_astVosEventTbl[VOS_MAX_EVENTS];

/* Interrupt nesting level */
VOS_UINT8 g_ucVosIntNesting;

VOS_UINT8 g_ucVosIntExitY;

/* Multitasking lock nesting level */
VOS_UINT8 g_ucVosLockNesting;

/* Priority of current task */
VOS_UINT8 g_ucVosPrioCur;

/* Priority of highest priority task */
VOS_UINT8 g_ucVosPrioHighRdy;

/* Ready list group */
VOS_UINT8 g_ucVosRdyGrp;

/* Table of tasks which are ready to run */
VOS_UINT8 g_aucVosRdyTbl[VOS_RDY_TBL_SIZE];

/* Flag indicating that kernel is running */
VOS_BOOL g_bVosRunning;

/* Number of tasks created */
VOS_UINT8 g_ucVosTaskCtr;

/* Idle counter */
volatile VOS_UINT32  g_ulVosIdleCtr;

/* Idle task stack,the top is 0x55aa55aa */
UCOM_SECTION(".os.stack.bss")
VOS_UINT8 g_stVosTaskIdleStk[VOS_TASK_IDLE_STK_SIZE]={0};

/* Pointer to currently running TCB*/
VOS_TCB *g_pstVosTCBCur;

/* Pointer to list of free TCBs */
VOS_TCB *g_pstVosTCBFreeList;

/* Pointer to highest priority TCB R-to-R */
VOS_TCB  *g_pstVosTCBHighRdy;

/* Pointer to doubly linked list of TCBs */
VOS_TCB  *g_pstVosTCBList;

/* Table of pointers to created TCBs  */
VOS_TCB *g_pastVosTCBPrioTbl[VOS_LOWEST_PRIO + 1];

/* Table of TCBs */
VOS_TCB g_stVosTCBTbl[VOS_MAX_TASKS];

/* Current value of system time (in ticks) */
volatile VOS_UINT32 g_ulVosTicks;

/*
******************************************************************************
*                              MAPPING TABLE TO MAP BIT POSITION TO BIT MASK
*
* Note: Index into table is desired bit position, 0..7
*       Indexed value corresponds to bit mask
******************************************************************************
*/
VOS_UINT8 const g_ucVosMapTbl[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
******************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
******************************************************************************
*/
VOS_UINT8 const g_ucVosUnMapTbl[] =
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x00 to 0x0F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x10 to 0x1F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x20 to 0x2F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x30 to 0x3F */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x40 to 0x4F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x50 to 0x5F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x60 to 0x6F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x70 to 0x7F */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x80 to 0x8F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x90 to 0x9F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xA0 to 0xAF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xB0 to 0xBF */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xC0 to 0xCF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xD0 to 0xDF */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xE0 to 0xEF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0        /* 0xF0 to 0xFF */
};

/*
******************************************************************************
*                                       FUNCTION PROTOTYPES
******************************************************************************
*/
VOS_VOID  VOS_InitEventList(VOS_VOID);
VOS_VOID  VOS_InitMisc(VOS_VOID);
VOS_VOID  VOS_InitRdyList(VOS_VOID);
VOS_VOID  VOS_InitTaskIdle(VOS_VOID);
VOS_VOID  VOS_InitTCBList(VOS_VOID);

/*****************************************************************************
 Function   : VOSInit
 Description: This function is used to initialize the internals of os and
              MUST be called prior to calling OSStart().
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOSInit(VOS_VOID)
{
    /* Initialize miscellaneous variables */
    VOS_InitMisc();

    /* Initialize the Ready List */
    VOS_InitRdyList();

    /* Initialize the free list of OS_TCBs */
    VOS_InitTCBList();

    /* Initialize the free list of OS_EVENTs */
    VOS_InitEventList();

    /* Create the Idle Task */
    VOS_InitTaskIdle();
}

/*****************************************************************************
 Function   : VOSIntEnter
 Description: enter interrupt
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOSIntEnter(VOS_VOID)
{
    if (g_bVosRunning == TRUE)
    {
        if (g_ucVosIntNesting < 255)
        {
            /* Increment ISR nesting level  */
            g_ucVosIntNesting++;
        }
    }
}

/*****************************************************************************
 Function   : VOSIntExit
 Description: leave interrupt
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOSIntExit(VOS_VOID)
{
    VOS_CPU_SR  cpu_sr;

    if (g_bVosRunning == TRUE)
    {
        cpu_sr = VOS_SplIMP();

        /* Prevent OSIntNesting from wrapping */
        if (g_ucVosIntNesting > 0)
        {
            g_ucVosIntNesting--;
        }

        /* Reschedule only if all ISRs complete ... */
        if ((g_ucVosIntNesting == 0) && (g_ucVosLockNesting == 0))
        {
            g_ucVosIntExitY = g_ucVosUnMapTbl[g_ucVosRdyGrp];
            g_ucVosPrioHighRdy
                = (VOS_UINT8)((g_ucVosIntExitY << 3)\
                + g_ucVosUnMapTbl[g_aucVosRdyTbl[g_ucVosIntExitY]]);

            /* No Ctx Sw if current task is highest rdy */
            if (g_ucVosPrioHighRdy != g_ucVosPrioCur)
            {
                g_pstVosTCBHighRdy  = g_pastVosTCBPrioTbl[g_ucVosPrioHighRdy];
#if (VOS_ARM == VOS_CPU_TYPE)
                g_ucVosPrioCur = g_ucVosPrioHighRdy;/* added by regenmann */
#endif
                /* Keep track of the number of ctx switches */
                g_ulVosCtxSwCtr++;

                /* Perform interrupt level ctx switch */
                VOSIntCtxSw();
            }
        }

        VOS_Splx(cpu_sr);
    }
}

/*****************************************************************************
 Function   : VOSStart
 Description: start kernel
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOSStart(VOS_VOID)
{
    VOS_UINT8 y;
    VOS_UINT8 x;

    if (g_bVosRunning == FALSE)
    {
        /* Find highest priority's task priority number */
        y = g_ucVosUnMapTbl[g_ucVosRdyGrp];
        x = g_ucVosUnMapTbl[g_aucVosRdyTbl[y]];
        g_ucVosPrioHighRdy = (VOS_UINT8)((y << 3) + x);
        g_ucVosPrioCur = g_ucVosPrioHighRdy;

        /* Point to highest priority task ready to run */
        g_pstVosTCBHighRdy = g_pastVosTCBPrioTbl[g_ucVosPrioHighRdy];
        g_pstVosTCBCur = g_pstVosTCBHighRdy;

        /* Execute target specific code to start task */
        VOSStartHighRdy();
    }
}

/*****************************************************************************
 Function   : VOSTimeTick
 Description: This function is used to signal to kernel the occurrence of
              a 'system tick' (also known as a 'clock tick').
              This function should be called by the ticker ISR but,
              can also be called by a high priority task.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOSTimeTick(VOS_VOID)
{
    VOS_CPU_SR  cpu_sr;
    VOS_TCB     *ptcb;

    cpu_sr = VOS_SplIMP();
    /* Update the 32-bit tick counter */
    g_ulVosTicks++;
    VOS_Splx(cpu_sr);

    if (g_bVosRunning == TRUE)
    {
        /* Point at first TCB in TCB list */
        ptcb = g_pstVosTCBList;

        /* Go through all TCBs in TCB list */
        while (ptcb->OSTCBPrio != VOS_IDLE_PRIO)
        {
            cpu_sr = VOS_SplIMP();

            /* Delayed or waiting for event with TO */
            if (ptcb->OSTCBDly != 0)
            {
                /* Decrement nbr of ticks to end of delay */
                if (--ptcb->OSTCBDly == 0)
                {
                    /* Is task suspended? */
                    if ((ptcb->OSTCBStat & VOS_STAT_SUSPEND) == VOS_STAT_RDY)
                    {
                        /* No,  Make task R-to-R (timed out)*/
                        g_ucVosRdyGrp |= ptcb->OSTCBBitY;
                        g_aucVosRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
                    else
                    {
                        /* ... loosing the task when the ...  */
                        ptcb->OSTCBDly = 1;
                    }
                }
            }

            /* Point at next TCB in TCB list  */
            ptcb = ptcb->OSTCBNext;

            VOS_Splx(cpu_sr);
        }
    }
}

/*****************************************************************************
 Function   : VOS_EventTaskRdy
 Description: This function is called by other kernel services and is used
              to ready a task that was waiting for an event to occur.
 Input      : pevent is a pointer to the event control block corresponding
              to the event.
            : msg is a pointer to a message. This pointer is used by message
              oriented services such as MAILBOXEs and QUEUEs.
              The pointer is not used when called by other service functions.

            : msk is a mask that is used to clear the status byte of the TCB.
              For example, OSSemPost() will pass OS_STAT_SEM,
              OSMboxPost() will pass OS_STAT_MBOX etc.
 Return     : pri
 Other      :
 *****************************************************************************/
VOS_UINT8 VOS_EventTaskRdy(VOS_EVENT *pevent, VOS_VOID *msg, VOS_UINT8 msk)
{
    VOS_TCB     *ptcb;
    VOS_UINT8   x;
    VOS_UINT8   y;
    VOS_UINT8   bitx;
    VOS_UINT8   bity;
    VOS_UINT8   prio;

    /* Find highest prio. task waiting for message  */
    y = g_ucVosUnMapTbl[pevent->OSEventGrp];
    bity = g_ucVosMapTbl[y];
    x = g_ucVosUnMapTbl[pevent->OSEventTbl[y]];
    bitx = g_ucVosMapTbl[x];

    /* Find priority of task getting the msg */
    prio = (VOS_UINT8)((y << 3) + x);

    /* Remove this task from the waiting list */
    if ((pevent->OSEventTbl[y] &= ~bitx) == 0x00)
    {
        /* Clr group bit if this was only task pending */
        pevent->OSEventGrp &= ~bity;
    }

    /* Point to this task's OS_TCB */
    ptcb =  g_pastVosTCBPrioTbl[prio];

    /* Prevent OSTimeTick() from readying task  */
    ptcb->OSTCBDly =  0;

    /* Unlink ECB from this task */
    ptcb->OSTCBEventPtr = (VOS_EVENT *)0;

    /* Prevent compiler warning if not used  */
    msg = msg;

    /* Clear bit associated with event type */
    ptcb->OSTCBStat &= ~msk;

    /* See if task is ready (could be susp'd) */
    if (ptcb->OSTCBStat == VOS_STAT_RDY)
    {
        /* Put task in the ready to run list */
        g_ucVosRdyGrp |=  bity;
        g_aucVosRdyTbl[y] |=  bitx;
    }

    return (prio);
}

/*****************************************************************************
 Function   : VOS_EventTaskWait
 Description: This function is called by other kernel services to suspend
              a task because an event has not occurred.
 Input      : pevent is a pointer to the event control block corresponding
              to the event.
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_EventTaskWait(VOS_EVENT *pevent)
{
    /* Store pointer to event control block in TCB        */
    g_pstVosTCBCur->OSTCBEventPtr = pevent;

    /* Task no longer ready      */
    g_aucVosRdyTbl[g_pstVosTCBCur->OSTCBY] &= ~g_pstVosTCBCur->OSTCBBitX;

    if ( g_aucVosRdyTbl[g_pstVosTCBCur->OSTCBY] == 0x00 )
    {
        /* Clear event grp bit if this was only task pending  */
        g_ucVosRdyGrp &= ~g_pstVosTCBCur->OSTCBBitY;
    }

    /* Put task in waiting list  */
    pevent->OSEventTbl[g_pstVosTCBCur->OSTCBY] |= g_pstVosTCBCur->OSTCBBitX;
    pevent->OSEventGrp |= g_pstVosTCBCur->OSTCBBitY;
}

/*****************************************************************************
 Function   : VOS_EventTO
 Description: This function is called by other kernel services to make a
              task ready to run because a timeout occurred.
 Input      : pevent is a pointer to the event control block corresponding
              to the event.
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_EventTO(VOS_EVENT *pevent)
{
    pevent->OSEventTbl[g_pstVosTCBCur->OSTCBY] &= ~g_pstVosTCBCur->OSTCBBitX;

    if ( pevent->OSEventTbl[g_pstVosTCBCur->OSTCBY] == 0x00)
    {
        pevent->OSEventGrp &= ~g_pstVosTCBCur->OSTCBBitY;
    }

    /* Set status to ready */
    g_pstVosTCBCur->OSTCBStat = VOS_STAT_RDY;

    /* No longer waiting for event */
    g_pstVosTCBCur->OSTCBEventPtr = (VOS_EVENT *)0;
}

/*****************************************************************************
 Function   : VOS_EventWaitListInit
 Description: This function is called by other kernel services to initialize
              the event wait list.
 Input      : pevent is a pointer to the event control block corresponding
              to the event.
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_EventWaitListInit(VOS_EVENT *pevent)
{
    VOS_UINT8  *ptbl;

    /* No task waiting on event  */
    pevent->OSEventGrp = 0x00;
    ptbl = &pevent->OSEventTbl[0];

#if VOS_EVENT_TBL_SIZE > 0
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 1
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 2
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 3
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 4
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 5
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 6
    *ptbl++ = 0x00;
#endif

#if VOS_EVENT_TBL_SIZE > 7
    *ptbl = 0x00;
#endif
}

/*****************************************************************************
 Function   : VOS_InitEventList
 Description: This function is called by OSInit() to initialize the free list
              of event control blocks.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_InitEventList(VOS_VOID)
{
#if (VOS_MAX_EVENTS > 1)
    VOS_UINT16      i;
    VOS_EVENT       *pevent1;
    VOS_EVENT       *pevent2;

    pevent1 = &g_astVosEventTbl[0];
    pevent2 = &g_astVosEventTbl[1];

    /* Init. list of free EVENT control blocks  */
    for (i = 0; i < (VOS_MAX_EVENTS - 1); i++)
    {
        pevent1->OSEventType = VOS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr = pevent2;
        pevent1++;
        pevent2++;
    }

    pevent1->OSEventType = VOS_EVENT_TYPE_UNUSED;
    pevent1->OSEventPtr = (VOS_EVENT *)0;
    g_pstVosEventFreeList      = &g_astVosEventTbl[0];
#else
    /* Only have ONE event control block */
    g_pstVosEventFreeList = &g_astVosEventTbl[0];
    g_pstVosEventFreeList->OSEventType = VOS_EVENT_TYPE_UNUSED;
    g_pstVosEventFreeList->OSEventPtr = (VOS_EVENT *)0;
#endif
}

/*****************************************************************************
 Function   : VOS_InitMisc
 Description: This function is called by OSInit() to initialize
              miscellaneous variables.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_InitMisc(VOS_VOID)
{
    /* Clear the 32-bit system clock */
    g_ulVosTicks = 0L;

    /* Clear the interrupt nesting counter */
    g_ucVosIntNesting = 0;

    /* Clear the scheduling lock counter */
    g_ucVosLockNesting = 0;

    /* Clear the number of tasks */
    g_ucVosTaskCtr = 0;

    /* Indicate that multitasking not started */
    g_bVosRunning = FALSE;

    /* Clear the context switch counter  */
    g_ulVosCtxSwCtr = 0;

    /* Clear the 32-bit idle counter */
    g_ulVosIdleCtr = 0L;

}

/*****************************************************************************
 Function   : VOS_InitRdyList
 Description: This function is called by OSInit() to initialize the Ready List.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_InitRdyList(VOS_VOID)
{
    VOS_UINT16   i;
    VOS_UINT8    *prdytbl;

    /* Clear the ready list */
    g_ucVosRdyGrp      = 0x00;
    prdytbl       = &g_aucVosRdyTbl[0];
    for (i = 0; i < VOS_RDY_TBL_SIZE; i++)
    {
        *prdytbl++ = 0x00;
    }

    g_ucVosPrioCur = 0;
    g_ucVosPrioHighRdy = 0;

    g_pstVosTCBHighRdy = (VOS_TCB *)0;
    g_pstVosTCBCur = (VOS_TCB *)0;
}

/*****************************************************************************
 Function   : VOS_InitTaskIdle
 Description: This function creates the Idle Task.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_InitTaskIdle(VOS_VOID)
{
    #if VOS_STK_CHK_EN
    /* 0x55 match with VOS_STK_FLG */
    memset(((VOS_UINT8 *)&g_stVosTaskIdleStk) + sizeof(VOS_UINT32),0x55,sizeof(g_stVosTaskIdleStk) - sizeof(VOS_UINT32) * 1);
    #endif
    (VOS_VOID)VOSTaskCreate(VOS_TaskIdle,
                       0,
                       (VOS_STK *)&g_stVosTaskIdleStk[VOS_TASK_IDLE_STK_SIZE - sizeof(VOS_UINT32)],
                       VOS_IDLE_PRIO);
}

/*****************************************************************************
 Function   : VOS_InitTCBList
 Description: This function is called by OSInit() to initialize
              the free list of OS_TCBs.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_InitTCBList(VOS_VOID)
{
    VOS_UINT8     i;
    VOS_TCB       *ptcb1;
    VOS_TCB       *ptcb2;

    /* TCB Initialization */
    g_pstVosTCBList     = (VOS_TCB *)0;

    /* Clear the priority table */
    for (i = 0; i < (VOS_LOWEST_PRIO + 1); i++)
    {
        g_pastVosTCBPrioTbl[i] = (VOS_TCB *)0;
    }

    ptcb1 = &g_stVosTCBTbl[0];
    ptcb2 = &g_stVosTCBTbl[1];

    /* Init. list of free TCBs */
    for (i = 0; i < (VOS_MAX_TASKS - 1); i++)
    {
        ptcb1->OSTCBNext = ptcb2;
        ptcb1++;
        ptcb2++;
    }

    /* Last OS_TCB */
    ptcb1->OSTCBNext = (VOS_TCB *)0;
    g_pstVosTCBFreeList = &g_stVosTCBTbl[0];
}

/*****************************************************************************
 Function   : VOS_Sched
 Description: schedule
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_Sched(VOS_VOID)
{
    VOS_CPU_SR     cpu_sr;
    VOS_UINT8      y;

    cpu_sr = VOS_SplIMP();

    /* Sched. only if all ISRs done & not locked */
    if ((g_ucVosIntNesting == 0) && (g_ucVosLockNesting == 0))
    {

        /* Get pointer to HPT ready to run */
        y = g_ucVosUnMapTbl[g_ucVosRdyGrp];
        g_ucVosPrioHighRdy
            = (VOS_UINT8)((y << 3) + g_ucVosUnMapTbl[g_aucVosRdyTbl[y]]);

        /* No Ctx Sw if current task is highest rdy */
        if (g_ucVosPrioHighRdy != g_ucVosPrioCur)
        {
            g_pstVosTCBHighRdy = g_pastVosTCBPrioTbl[g_ucVosPrioHighRdy];
#if (VOS_ARM == VOS_CPU_TYPE)
            g_ucVosPrioCur = g_ucVosPrioHighRdy;/* added by regenmann */
#endif
            /* Increment context switch counter */
            g_ulVosCtxSwCtr++;

            /* Perform a context switch */
            VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_TASK_SWITCH_START);
            VOS_TASK_SW();
        }
    }

    VOS_Splx(cpu_sr);
}

/*****************************************************************************
 Function   : VOS_TaskIdle
 Description: This task is internal to kernel and executes whenever no other
              higher priority tasksexecutes because they are ALL waiting for
              event(s) to occur.
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_TaskIdle(VOS_UINT32 data)
{
    VOS_CPU_SR  cpu_sr;
    VOS_IDLE_HOOK_FUN_TYPE pfIdleHook = VOS_NULL;
    /* Prevent compiler warning for not using 'pdata' */
    data = data;

    for (;;)
    {
        cpu_sr = VOS_SplIMP();
        g_ulVosIdleCtr++;
        VOS_Splx(cpu_sr);

        pfIdleHook = g_pfVosHookFuncTable[OS_HOOK_TYPE_IDLE];
        if(VOS_NULL!=pfIdleHook)
        {
            pfIdleHook(g_ulVosCtxSwCtr);
        }


    }
}


/*****************************************************************************
 Function   : VOS_TCBInit
 Description: This function is internal to kernel and is used to initialize
              a Task Control Block when a task is created (see OSTaskCreate()
              and OSTaskCreateExt()).
 Input      : prio is the priority of the task being created

              ptos is a pointer to the task's top-of-stack assuming that the CPU
              registers have been placed on the stack.
              Note that the top-of-stack corresponds to a 'high' memory location
              is OS_STK_GROWTH is set to 1 and a 'low' memory location if
              OS_STK_GROWTH is set to 0.
              Note that stack growth is CPU specific.

            : pbos is a pointer to the bottom of stack.
              A NULL pointer is passed if called by 'OSTaskCreate()'.

            : id is the task's ID (0..65535)

            : stk_size is the size of the stack (in 'stack units').
              If the stack units are INT8Us then, 'stk_size' contains the number
              of bytes for the stack.  If the stack units are INT32Us then,
              the stack contains '4 * stk_size' bytes.
              The stack units are established by the #define constant VOS_STK
              which is CPU specific.'stk_size' is 0 if called by 'OSTaskCreate()'

            : pext is a pointer to a user supplied memory area that is used to
              extend the task control block.
              This allows you to store the contents of floating-point registers,
              MMU registers or anything else you could find useful during a
              context switch.  You can even assign a name to each task and store
              this name in this TCB extension.
              A NULL pointer is passed if called by OSTaskCreate().

            : opt options as passed to 'OSTaskCreateExt()' or,
              0 if called from 'OSTaskCreate()'.

 Returns    : VOS_OK    if the call was successful
            : OS_NO_MORE_TCB if there are no more free TCBs to be allocated
              and thus, the task cannot be created.
Other      :
*****************************************************************************/
VOS_UINT8 VOS_TCBInit(VOS_UINT8 prio, VOS_STK *ptos, VOS_STK *pbos, VOS_UINT16 id,
                   VOS_UINT32 stk_size, VOS_VOID *pext, VOS_UINT16 opt)
{
    VOS_CPU_SR  cpu_sr;
    VOS_TCB     *ptcb;

    cpu_sr = VOS_SplIMP();

    /* Get a free TCB from the free TCB list */
    ptcb = g_pstVosTCBFreeList;
    if (ptcb != (VOS_TCB *)0)
    {
        /* Update pointer to free TCB list  */
        g_pstVosTCBFreeList = ptcb->OSTCBNext;
        VOS_Splx(cpu_sr);

        /* Load Stack pointer in TCB  */
        ptcb->OSTCBStkPtr = ptos;
        ptcb->StackAddress = (VOS_UINT32)pbos;
        /* Load task priority into TCB  */
        ptcb->OSTCBPrio = (VOS_UINT8)prio;
        /* Task is ready to run  */
        ptcb->OSTCBStat = VOS_STAT_RDY;
        /* Task is not delayed  */
        ptcb->OSTCBDly = 0;

        /* Prevent compiler warning if not used */
        pext = pext;
        stk_size = stk_size;
        pbos = pbos;
        opt = opt;
        id = id;

        /* Pre-compute X, Y, BitX and BitY */
        ptcb->OSTCBY = prio >> 3;
        ptcb->OSTCBBitY = g_ucVosMapTbl[ptcb->OSTCBY];
        ptcb->OSTCBX = prio & 0x07;
        ptcb->OSTCBBitX = g_ucVosMapTbl[ptcb->OSTCBX];

        /* Task is not pending on an event */
        ptcb->OSTCBEventPtr = (VOS_EVENT *)0;
        cpu_sr = VOS_SplIMP();
        g_pastVosTCBPrioTbl[prio] = ptcb;

        /* Link into TCB chain */
        ptcb->OSTCBNext = g_pstVosTCBList;
        ptcb->OSTCBPrev = (VOS_TCB *)0;

        if (g_pstVosTCBList != (VOS_TCB *)0)
        {
            g_pstVosTCBList->OSTCBPrev = ptcb;
        }
        g_pstVosTCBList = ptcb;

        /* Make task ready to run                   */
        g_ucVosRdyGrp |= ptcb->OSTCBBitY;
        g_aucVosRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        VOS_Splx(cpu_sr);
        return (VOS_OK);
    }

    VOS_Splx(cpu_sr);

    return (VOS_ERRNO_NO_MORE_TCB);
}

/*****************************************************************************
 Function   : VOS_TCBReset
 Description: reset tcb
 Input      : same as 'OS_TCBInit'
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_UINT8 VOS_TCBReset(VOS_UINT8 prio, VOS_STK *ptos, VOS_STK *pbos, VOS_UINT16 id,
                    VOS_UINT32 stk_size, VOS_VOID *pext, VOS_UINT16 opt)
{
    VOS_CPU_SR   cpu_sr;
    VOS_TCB      *ptcb;

    cpu_sr = VOS_SplIMP();

    /* Get curent TCB */
    ptcb = g_pastVosTCBPrioTbl[prio];
    if (ptcb != (VOS_TCB *)0)
    {
        VOS_Splx(cpu_sr);

        /* Load Stack pointer in TCB*/
        ptcb->OSTCBStkPtr = ptos;

        /* Task is ready to run */
        ptcb->OSTCBStat = VOS_STAT_RDY;

        /* Task is not delayed */
        ptcb->OSTCBDly = 0;
        /* Task is not pending on an event */
        ptcb->OSTCBEventPtr = (VOS_EVENT *)0;

        cpu_sr = VOS_SplIMP();

        /* Make task ready to run */
        g_ucVosRdyGrp |= ptcb->OSTCBBitY;
        g_aucVosRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;

        VOS_Splx(cpu_sr);

        return (VOS_OK);
    }

    VOS_Splx(cpu_sr);

    return (VOS_ERRNO_TASK_NOT_EXIST);
}

/*****************************************************************************
 Function   : VOSSemCreate
 Description: This function creates a semaphore.
 Input      : cnt           is the initial value for the semaphore.
 Output     : VOS_VOID
 Return     : != (VOS_VOID *)0  is a pointer to the event control clock (OS_EVENT)
              associated with the created semaphore
              == (VOS_VOID *)0  if no event control blocks were available
 *****************************************************************************/
VOS_EVENT *VOSSemCreate(VOS_UINT16 cnt)
{
    VOS_CPU_SR   cpu_sr;
    VOS_EVENT    *pevent;

    /* See if called from ISR ... */
    if (g_ucVosIntNesting > 0)
    {
        /* ... can't CREATE from an ISR */
        return ((VOS_EVENT *)0);
    }

    cpu_sr = VOS_SplIMP();

    /* Get next free event control block */
    pevent = g_pstVosEventFreeList;

    /* See if pool of free ECB pool was empty */
    if (g_pstVosEventFreeList != (VOS_EVENT *)0)
    {
        g_pstVosEventFreeList = (VOS_EVENT *)g_pstVosEventFreeList->OSEventPtr;
    }

    VOS_Splx(cpu_sr);

    /* Get an event control block */
    if (pevent != (VOS_EVENT *)0)
    {
        pevent->OSEventType = VOS_EVENT_TYPE_SEM;

        /* Set semaphore value */
        pevent->OSEventCnt = cnt;

        /* Unlink from ECB free list */
        pevent->OSEventPtr = (VOS_VOID *)0;

        /* Initialize to 'nobody waiting' on sem. */
        VOS_EventWaitListInit(pevent);
    }

    return (pevent);
}

/*****************************************************************************
 Function   : VOSSemPend
 Description: This function waits for a semaphore.
 Input      : pevent is a pointer to the event control block associated with the
              desired semaphore.
            : timeout is an optional timeout period (in clock ticks).
              If non-zero, your task will  wait for the resource up to the amount
              of time specified by this argument. If you specify 0, however,
              your task will wait forever at the specified semaphore or,
              until the resource becomes available (or the event occurs).
Output      : err is a pointer to where an error message will be deposited.
              Possible error messages are:
Returns     : none
*****************************************************************************/
VOS_VOID VOSSemPend(VOS_EVENT *pevent, VOS_UINT16 timeout, VOS_UINT8 *err)
{
    VOS_CPU_SR  cpu_sr;

    /* See if called from ISR ... */
    if (g_ucVosIntNesting > 0)
    {
        /* ... can't PEND from an ISR  */
        *err = VOS_ERRNO_PEND_ISR;
        return;
    }

#if VOS_ARG_CHK_EN > 0
    /* Validate 'pevent' */
    if (pevent == (VOS_EVENT *)0)
    {
        *err = VOS_ERRNO_PEVENT_NULL;
        return;
    }

    /* Validate event block type  */
    if (pevent->OSEventType != VOS_EVENT_TYPE_SEM)
    {
        *err = VOS_ERRNO_EVENT_TYPE;
        return;
    }
#endif

    cpu_sr = VOS_SplIMP();

    /* If sem. is positive, resource available ...*/
    if (pevent->OSEventCnt > 0)
    {
        /* ... decrement semaphore only if positive. */
        pevent->OSEventCnt--;
        VOS_Splx(cpu_sr);
        *err = VOS_OK;
        return;
    }

    /* Resource not available, pend on semaphore  */
    g_pstVosTCBCur->OSTCBStat |= VOS_STAT_SEM;

    /* Store pend timeout in TCB */
    g_pstVosTCBCur->OSTCBDly = timeout;

    /* Suspend task until event or timeout occurs  */
    VOS_EventTaskWait(pevent);

    VOS_Splx(cpu_sr);

    /* Find next highest priority task ready  */
    VOS_Sched();

    cpu_sr = VOS_SplIMP();

    /* Must have timed out if still waiting for event */
    if (g_pstVosTCBCur->OSTCBStat & VOS_STAT_SEM)
    {
        VOS_EventTO(pevent);

        VOS_Splx(cpu_sr);

        /* Indicate that didn't get event within TO */
        *err = VOS_ERRNO_TIMEOUT;
        return;
    }
    g_pstVosTCBCur->OSTCBEventPtr = (VOS_EVENT *)0;

    VOS_Splx(cpu_sr);

    *err = VOS_OK;
}

/*****************************************************************************
 Function   : VOSSemPost
 Description: This function signals a semaphore
 Input      : pevent is a pointer to the event control block associated with the
              desired semaphore.
Output      : VOS_VOID
Returns     : VOS_OK or error code
*****************************************************************************/
VOS_UINT8 VOSSemPost(VOS_EVENT *pevent)
{
    VOS_CPU_SR  cpu_sr;

#if VOS_ARG_CHK_EN > 0
    /* Validate 'pevent'  */
    if (pevent == (VOS_EVENT *)0)
    {
        return (VOS_ERRNO_PEVENT_NULL);
    }

    /* Validate event block type */
    if (pevent->OSEventType != VOS_EVENT_TYPE_SEM)
    {
        return (VOS_ERRNO_EVENT_TYPE);
    }
#endif

    cpu_sr = VOS_SplIMP();

    /* See if any task waiting for semaphore    */
    if (pevent->OSEventGrp != 0x00)
    {
        /* Ready highest prio task waiting on event */
        VOS_EventTaskRdy(pevent, (VOS_VOID *)0, VOS_STAT_SEM);

        VOS_Splx(cpu_sr);

        /* Find highest priority task ready to run  */
        VOS_Sched();

        return (VOS_OK);
    }

    /* Make sure semaphore will not overflow */
    if (pevent->OSEventCnt < 65535)
    {
        /* Increment semaphore count to register event   */
        pevent->OSEventCnt++;

        VOS_Splx(cpu_sr);

        return (VOS_OK);
    }

    VOS_Splx(cpu_sr);

    /* Semaphore value has reached its maximum */
    return (VOS_ERRNO_SEM_OVF);
}

/*****************************************************************************
 Function   : VOSSemReset
 Description: This function reset a semaphore
 Input      : pevent is a pointer to the event control block associated with the
              desired semaphore.
Output      : VOS_VOID
Returns     : VOS_OK or error code
*****************************************************************************/
VOS_VOID VOSSemReset(VOS_EVENT *pevent)
{
    VOS_CPU_SR  cpu_sr;

    cpu_sr = VOS_SplIMP();

    /* Get an event control block  */
    if (pevent != (VOS_EVENT *)0)
    {
        /* Set semaphore value  */
        pevent->OSEventCnt  = 0;

        /* Initialize to 'nobody waiting' on sem.   */
        VOS_EventWaitListInit(pevent);
    }

    VOS_Splx(cpu_sr);

    return;
}

#if (VOS_ZOS == VOS_OS_VER)
/*****************************************************************************
 Function   : VOS_SmCCreate
 Description: To create a counting semaphore;
 Input      : acSmName -- the semaphore name, can be null
              ulSmInit -- The count number of the semaphore that create;
              ulFlags  -- FIFO or priority;
 Output     : pulSmID  -- the ID of the create semaphore;
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SmCCreate( VOS_CHAR   acSmName[4],
                          VOS_UINT32 ulSmInit,
                          VOS_UINT32 ulFlags,
                          VOS_UINT32 *pulSmID )
{
    VOS_EVENT              *pstSemEvent;

    pstSemEvent = VOSSemCreate((VOS_UINT16)ulSmInit);
    if( VOS_NULL_PTR == pstSemEvent )
    {
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_CCREATE_OSALCREATEERR);
        return( VOS_ERRNO_SEMA4_CCREATE_OSALCREATEERR );
    }

    *pulSmID=(VOS_UINT32)pstSemEvent;
    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_SmP
 Description: Lock the resource for synchronization, if the resource is none
              then block, otherwise the number of the resource --
 Input      : ulSmID             -- the ID of the resource to lock
              ulTimeOutInMillSec -- the time to wait(0 for ever)
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SmP( VOS_UINT32 Sm_ID, VOS_UINT32 ulTimeOutInMillSec )
{
    VOS_UINT8          ucErrNo;
    VOS_EVENT          *pstSemEvent;

    pstSemEvent = (VOS_EVENT *)Sm_ID;
    VOSSemPend(pstSemEvent, 0, &ucErrNo);

    return ucErrNo;

}

/*****************************************************************************
 Function   : VOS_SmV
 Description: Release/Unlock the sema4 that has locked a resource
 Input      : ulSmID -- id of semaphore
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SmV( VOS_UINT32 Sm_ID )
{
    VOS_EVENT          *pstSemEvent;

    pstSemEvent = (VOS_EVENT *)Sm_ID;
    return VOSSemPost(pstSemEvent);
}

VOS_VOID VOS_SmReset(VOS_UINT32 Sm_ID, VOS_UINT32 ulSmInit)
{
    VOS_EVENT          *pstSemEvent;

    pstSemEvent = (VOS_EVENT *)Sm_ID;
    VOSSemReset(pstSemEvent);

    pstSemEvent->OSEventCnt = (VOS_UINT16)ulSmInit;
}
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

