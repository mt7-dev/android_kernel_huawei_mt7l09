/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_queue.c                                                       */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement queue                                              */
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

#include "vos_config.h"
#include "v_typdef.h"
#include "v_queue.h"
#include "v_msg.h"
#include "v_lib.h"
#include "v_io.h"
#include "v_task.h"
#include "vos_id.h"
#include "v_int.h"
#if (VOS_ZOS == VOS_OS_VER)
#include "kernel.h"
#endif



#define    THIS_FILE_ID        FILE_ID_V_QUEUE_C

typedef struct
{
    VOS_UINT32          *QStart;
    VOS_UINT32          *QEnd;
    VOS_UINT32          *QIn;
    VOS_UINT32          *QOut;
    VOS_UINT32          QSize;
    VOS_UINT32          QEntries;
}VOS_Q_STRU;

typedef struct
{
    VOS_UINT32          Flag;/* control block's state */
    VOS_UINT32          Qid; /* queue ID */
    VOS_UINT32          ulCurrentAddr;
#if (VOS_ZOS == VOS_OS_VER)
    VOS_EVENT           *OsSemId;/*pend this Sem to get queue */
#endif
#if (VOS_WIN32 == VOS_OS_VER)
    VOS_UINT32          Sem_ID;/*pend this Sem to get queue */
#endif
    VOS_Q_STRU          Q;
} VOS_QUEUE_CONTROL_BLOCK;

/* the number of queue's control block */
VOS_UINT32               vos_QueueCtrlBlcokNumber;

/* the start address of queue's control block */
VOS_QUEUE_CONTROL_BLOCK  vos_QueueCtrlBlcok[VOS_MAX_QUEUE_NUMBER];

/* the space of queue, keep lint happy */
VOS_UINT8                g_ulQueueSpace[VOS_QUEUE_POOL_SIZE]={0};

/* the current value of queue's space  */
VOS_UINT32              g_ulCurrentQueueAddress;

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 VOS_SmCCreate( VOS_CHAR   acSmName[4],
                          VOS_UINT32 ulSmInit,
                          VOS_UINT32 ulFlags,
                          VOS_UINT32 *pulSmID );

extern VOS_UINT32 VOS_SmP( VOS_UINT32 Sm_ID, VOS_UINT32 ulTimeOutInMillSec );

extern VOS_UINT32 VOS_SmV( VOS_UINT32 Sm_ID );
#endif

/*****************************************************************************
 Function   : VOS_QueueCtrlBlkInit
 Description: Init queue's control block
 Input      : ulQueueCtrlBlcokNumber -- number of queue
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_QueueCtrlBlkInit(VOS_UINT32 ulQueueCtrlBlcokNumber)
{
    VOS_UINT32  i;

    memset(g_ulQueueSpace, 0, VOS_QUEUE_POOL_SIZE*sizeof(VOS_UINT8));

    vos_QueueCtrlBlcokNumber = ulQueueCtrlBlcokNumber;

    for(i=0; i<vos_QueueCtrlBlcokNumber; i++)
    {
        vos_QueueCtrlBlcok[i].Flag          = CONTROL_BLOCK_IDLE;
        vos_QueueCtrlBlcok[i].Qid           = i;
        vos_QueueCtrlBlcok[i].ulCurrentAddr = VOS_NULL;
    }

    g_ulCurrentQueueAddress = (VOS_UINT32)(&g_ulQueueSpace[0]);

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_QueueCtrlBlkGet
 Description: allocte a queue control block
 Input      : VOS_VOID
            : VOS_VOID
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_QueueCtrlBlkGet(VOS_VOID)
{
    VOS_UINT32      i;
    VOS_CPU_SR      CpuLockLevel;

    CpuLockLevel = VOS_SplIMP();

    for(i=0; i<vos_QueueCtrlBlcokNumber; i++)
    {
        if(vos_QueueCtrlBlcok[i].Flag == CONTROL_BLOCK_IDLE)
        {
            vos_QueueCtrlBlcok[i].Flag = CONTROL_BLOCK_BUSY;

            break;
        }
    }

    VOS_Splx(CpuLockLevel);

    if( i < vos_QueueCtrlBlcokNumber)
    {
        return i;
    }

    LogPrint("# VOS_QueueCtrlBlkGet no Idle.\r\n");

    VOS_SetErrorNo(VOS_ERRNO_QUEUE_FULL);

    return( VOS_NULL_DWORD );
}

#if 0
/*****************************************************************************
 Function   : VOS_QueueCtrlBlkFree
 Description: free a queue control block
 Input      : Qid -- queue ID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_QueueCtrlBlkFree(VOS_UINT32 Qid)
{
    VOS_CPU_SR             CpuLockLevel;

    if( Qid < vos_QueueCtrlBlcokNumber )
    {
        if(vos_QueueCtrlBlcok[Qid].Flag == CONTROL_BLOCK_IDLE)
        {
            Print("# VOS_QueueCtrlBlkFree free Idle Queue.\r\n");
        }
        else
        {
            CpuLockLevel = VOS_SplIMP();

            vos_QueueCtrlBlcok[Qid].Flag = CONTROL_BLOCK_IDLE;

            VOS_Splx(CpuLockLevel);
        }

        return VOS_OK;
    }
    else
    {
        Print("# VOS_QueueGetCtrlBlkInit Error.\r\n");

        return VOS_ERR;
    }
}
#endif

/*****************************************************************************
 Function   : VOS_FixedQueueCreate
 Description: Create queue
 Input      : pchName      -- Name(16 chars) of the queue
              ulLength     -- The length of the queue, 0 means default size
                              which definded by VOS_DEFAULT_QUEUE_SIZE
              pulQueueID   -- The address of the queue ID
              ulQueueMode  -- Queue block mode:
                              VOS_MSG_Q_FIFO or VOS_MSG_Q_PRIORITY
              ulMaxMsgSize -- The Max message size that the queue can contain.
                              Now must be 4
              ulQueueNum   -- The Max number of Queue size
 Output     : pulQueueID   -- The address of the queue ID
 Return     : VOS_OK on success or errno on failure
 Other      : The queue name can be same to others
 *****************************************************************************/
VOS_UINT32 VOS_FixedQueueCreate(VOS_UINT32 ulLength, VOS_UINT32 *pulQueueID)
{
    VOS_UINT32      ulBeginAddress;
    VOS_UINT32      iQid;
    VOS_UINT32      AllocSize;
    VOS_CPU_SR      CpuLockLevel;

    AllocSize = ulLength*sizeof(VOS_UINT32);

    /*if( pulQueueID == VOS_NULL_PTR )
    {
        VOS_SetErrorNo(VOS_ERRNO_QUEUE_CREATE_OUTPUTISNULL);
        return( VOS_ERRNO_QUEUE_CREATE_OUTPUTISNULL );
    }*/ /* economize text */

    /* create queue */
    iQid = VOS_QueueCtrlBlkGet();
    if( VOS_NULL_DWORD == iQid )
    {
        VOS_SetErrorNo(VOS_ERRNO_QUEUE_CREATE_NOFREECB);
        return( VOS_ERRNO_QUEUE_CREATE_NOFREECB );
    }

    /* Allocate space. I can not consider freeing space.
        Nothing be done for managing memory */
    CpuLockLevel = VOS_SplIMP();

    ulBeginAddress = g_ulCurrentQueueAddress;

    g_ulCurrentQueueAddress += AllocSize;

    VOS_Splx(CpuLockLevel);

    vos_QueueCtrlBlcok[iQid].Q.QStart = (VOS_UINT32 *)ulBeginAddress;

    vos_QueueCtrlBlcok[iQid].Q.QEnd
        = (VOS_UINT32 *)((VOS_UINT32)vos_QueueCtrlBlcok[iQid].Q.QStart + AllocSize);
    vos_QueueCtrlBlcok[iQid].Q.QIn = vos_QueueCtrlBlcok[iQid].Q.QStart;
    vos_QueueCtrlBlcok[iQid].Q.QOut = vos_QueueCtrlBlcok[iQid].Q.QStart;
    vos_QueueCtrlBlcok[iQid].Q.QSize = ulLength;
    vos_QueueCtrlBlcok[iQid].Q.QEntries = 0;

#if (VOS_WIN32 == VOS_OS_VER)
    if( VOS_OK
        != VOS_SmCCreate("tmp", 0, 0, &vos_QueueCtrlBlcok[iQid].Sem_ID))
    {
        /* I can not consider freeing space.
            Nothing be done for managing memory */
        /* VOS_QueueCtrlBlkFree( iQid ); */ /* economize text */
        VOS_SetErrorNo(VOS_ERRNO_QUEUE_CREATE_OSALFAIL);
        return( VOS_ERRNO_QUEUE_CREATE_OSALFAIL );
    }
#endif

#if (VOS_ZOS == VOS_OS_VER)
    vos_QueueCtrlBlcok[iQid].OsSemId = VOSSemCreate(0);

    if( VOS_NULL_PTR == vos_QueueCtrlBlcok[iQid].OsSemId )
    {
        /* I can not consider freeing space.
            Nothing be done for managing memory */
        /* VOS_QueueCtrlBlkFree( iQid ); *//* economize text */
        VOS_SetErrorNo(VOS_ERRNO_QUEUE_CREATE_OSALFAIL);
        return( VOS_ERRNO_QUEUE_CREATE_OSALFAIL );
    }
#endif

    *pulQueueID = iQid;

    return( VOS_OK );
}

/*****************************************************************************
 Function   : VOS_AddQueue
 Description: Write a message to a specific queue's list
 Input      : ulQueueID    -- Queue ID
              suffix       -- queue's suffix
              AddressValue -- the message's address
 Output     : None
 Return     : VOS_OK on success or errno on failure
 Other      : If the buffer size to write is larger than max message length,
              the writing would be failed.
 *****************************************************************************/
VOS_UINT32 VOS_AddQueue(VOS_UINT32 ulQueueID, VOS_UINT32 AddressValue,
                        VOS_UINT32 Pri)
{
    VOS_CPU_SR                 CpuLockLevel;
    VOS_Q_STRU                *pstQueue = &(vos_QueueCtrlBlcok[ulQueueID].Q);
    VOS_UINT32                *puwCur;
    VOS_UINT32                *puwPre;
    VOS_UINT32                 uwCnt;

    CpuLockLevel = VOS_SplIMP();

    if ( pstQueue->QEntries >= pstQueue->QSize )
    {
         VOS_Splx(CpuLockLevel);
         return VOS_ERR;
    }

    if ( VOS_EMERGENT_PRIORITY_MSG == Pri )
    {
        puwCur  = pstQueue->QOut;

        if (pstQueue->QOut == pstQueue->QStart)
        {
            pstQueue->QOut = pstQueue->QEnd - 1;
        }
        else
        {
            pstQueue->QOut--;
        }

        puwPre  = pstQueue->QOut;

        /* 保证所有紧急消息按FIFO顺序出队 */
        for (uwCnt = 0; uwCnt < pstQueue->QEntries; uwCnt++)
        {
            if (!VOS_IsMsgEmergent(*puwCur))
            {
                break;
            }

            *puwPre = *puwCur;
            puwPre  = puwCur++;

            if (puwCur == pstQueue->QEnd)
            {
                puwCur = pstQueue->QStart;
            }
        }
        *puwPre = (VOS_UINT32)AddressValue;
        pstQueue->QEntries++;

    }
    else
    {
        *(pstQueue->QIn)++ = (VOS_UINT32)AddressValue;
        pstQueue->QEntries++;
        if (pstQueue->QIn == pstQueue->QEnd )
        {
            pstQueue->QIn = pstQueue->QStart;
        }
    }

    VOS_Splx(CpuLockLevel);

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_FixedQueueWrite
 Description: Write a message to a specific queue synchronuslly
 Input      : ulQueueID    -- Queue ID
              pBufferAddr  -- Message buffer
              ulBufferSize -- The size of Message buffer
              ulTimeOut    -- Time-out interval, in milliseconds.
                              0 means infinite
 Output     : None
 Return     : VOS_OK on success or errno on failure
 Other      : If the buffer size to write is larger than max message length,
              the writing would be failed.
 *****************************************************************************/
VOS_UINT32 VOS_FixedQueueWrite(VOS_UINT32 ulQueueID, VOS_VOID * pBufferAddr,
                               VOS_UINT32 Pri)
{
    VOS_UINT32          ulReturn;

    ulReturn = VOS_AddQueue(ulQueueID, (VOS_UINT32)pBufferAddr, Pri);

    if (VOS_OK != ulReturn )
    {
        LogPrint1("# Queue ID %ld is full.\r\n",(VOS_INT)ulQueueID);

        return VOS_ERR;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    if ( VOS_OK != VOS_SmV( vos_QueueCtrlBlcok[ulQueueID].Sem_ID) )
    {
        LogPrint("# VOS_SmV error.\r\n");

        return VOS_ERR;
    }
#endif

#if (VOS_ZOS == VOS_OS_VER)
    if ( VOS_OK != VOSSemPost( vos_QueueCtrlBlcok[ulQueueID].OsSemId ) )
    {
        LogPrint("# VOS_SmV error.\r\n");

        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_FixedQueueRead
 Description: Reading message from a queue synchronouslly
 Input      : ulQueueID    -- Queue ID to read
              ulTimeOut    -- Time-out interval,  in milliseconds.
                              0 means infinite
              pBufferAddr  -- Buffer to retrieve msg
              ulBufferSize -- size of buffer
 Output     : None
 Return     : the length of the message readed from specific queue;
              VOS_NULL_LONG indicates reading failure.
 Other      : If the reading buffer size is less than message length, the msg
              would be truncated.
 *****************************************************************************/
VOS_UINT32 VOS_FixedQueueRead(VOS_UINT32 ulQueueID, VOS_UINT32 * pBufferAddr)
{
    VOS_CPU_SR          CpuLockLevel;
    VOS_UINT32          TempValue;
#if (VOS_ZOS == VOS_OS_VER)
    VOS_UINT8           ucErrNo;
#endif

#if (VOS_WIN32 == VOS_OS_VER)
    if( VOS_OK != VOS_SmP( vos_QueueCtrlBlcok[ulQueueID].Sem_ID, 0 ) )
    {
        Print("# msgQReceive error.\r\n");
        return VOS_ERR;
    }
#endif

#if (VOS_ZOS == VOS_OS_VER)
    VOSSemPend(vos_QueueCtrlBlcok[ulQueueID].OsSemId, 0, &ucErrNo);

    if ( VOS_OK != ucErrNo )
    {
        Print("# msgQReceive error.\r\n");
        return VOS_ERR;
    }
#endif

    CpuLockLevel = VOS_SplIMP();

    if (vos_QueueCtrlBlcok[ulQueueID].Q.QEntries > 0 )
    {
        TempValue = *vos_QueueCtrlBlcok[ulQueueID].Q.QOut++;
        vos_QueueCtrlBlcok[ulQueueID].Q.QEntries--;
        if ( vos_QueueCtrlBlcok[ulQueueID].Q.QOut
            == vos_QueueCtrlBlcok[ulQueueID].Q.QEnd )
        {
            vos_QueueCtrlBlcok[ulQueueID].Q.QOut
                = vos_QueueCtrlBlcok[ulQueueID].Q.QStart;
        }

        vos_QueueCtrlBlcok[ulQueueID].ulCurrentAddr = TempValue;

        VOS_Splx(CpuLockLevel);

        *pBufferAddr = TempValue;

        return  VOS_OK;
    }

    VOS_Splx(CpuLockLevel);

    return  VOS_ERR;
}

/*****************************************************************************
 Function   : VOS_OutMsg
 Description: get a ready message from the message queue.
 Input      : ulTaskID --- Task ID to get its message
 Return     : message address and VOS_NULL_PTR on failure
 Other      : After getting the message, the status would be changed to ACTIVE
 *****************************************************************************/
VOS_VOID* VOS_OutMsg( VOS_UINT32 ulQueueID )
{
    VOS_CPU_SR          CpuLockLevel;
    VOS_UINT32          TempValue;

    CpuLockLevel = VOS_SplIMP();

    if (vos_QueueCtrlBlcok[ulQueueID].Q.QEntries > 0 )
    {
        TempValue = *vos_QueueCtrlBlcok[ulQueueID].Q.QOut++;
        vos_QueueCtrlBlcok[ulQueueID].Q.QEntries--;
        if ( vos_QueueCtrlBlcok[ulQueueID].Q.QOut
            == vos_QueueCtrlBlcok[ulQueueID].Q.QEnd )
        {
            vos_QueueCtrlBlcok[ulQueueID].Q.QOut
                = vos_QueueCtrlBlcok[ulQueueID].Q.QStart;
        }
        VOS_Splx(CpuLockLevel);

        TempValue += VOS_MSG_BLK_HEAD_LEN;

        return  (VOS_VOID *)TempValue;
    }


    VOS_Splx(CpuLockLevel);

    return  VOS_NULL_PTR;
}

/*****************************************************************************
 Function   : VOS_FixedQueueClean
 Description: clean up a queue.
 Input      : ulQueueID --- Task ID to get its message
 Return     : message address and VOS_NULL_PTR on failure
 Other      : After getting the message, the status would be changed to ACTIVE
 *****************************************************************************/
VOS_UINT32 VOS_FixedQueueClean( VOS_UINT32 ulQueueID, VOS_UINT32 *pBufferAddr )
{
    VOS_VOID *pvMsg;

    if ( ulQueueID >= vos_QueueCtrlBlcokNumber )
    {
        return VOS_ERR;
    }

    pvMsg = VOS_OutMsg(ulQueueID);

    while ( VOS_NULL_PTR != pvMsg )
    {
        VOS_FreeMsg(VOS_PID_DEFAULT, pvMsg);

        pvMsg = VOS_OutMsg(ulQueueID);
    }

#if (VOS_ZOS == VOS_OS_VER)
    VOSSemReset(vos_QueueCtrlBlcok[ulQueueID].OsSemId);
#endif

    *pBufferAddr = vos_QueueCtrlBlcok[ulQueueID].ulCurrentAddr;

    return VOS_OK;
}


/*****************************************************************************
 Function   : VOS_PeekQueue
 Description: check queue is empty.
 Input      : NULL
 Return     : 1: if queue is not empty , 0:if queue is empty
 Other      : used by DRX
 *****************************************************************************/
VOS_UINT16 VOS_PeekQueue( VOS_VOID )
{
    VOS_UINT16      uhwModuleNum;
    VOS_FID         ulFid;
    VOS_UINT32      ulQueueID;
    VOS_UINT16      ulRet = 0;
    VOS_CPU_SR      CpuLockLevel;

    CpuLockLevel    = VOS_SplIMP();

    for ( uhwModuleNum = VOS_FID_RESERVED + 1; uhwModuleNum < VOS_FID_BUTT; uhwModuleNum++ )
    {
        ulFid       = uhwModuleNum;
        ulQueueID   = vos_FidCtrlBlk[ulFid].Qid;

        if(vos_QueueCtrlBlcok[ulQueueID].Q.QEntries > 0)
        {
            ulRet   = 1;
            break;
        }
    }

    VOS_Splx(CpuLockLevel);

    return ulRet;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

