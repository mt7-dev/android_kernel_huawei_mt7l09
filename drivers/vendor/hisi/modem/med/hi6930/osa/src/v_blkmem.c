/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_blkmen.c                                                      */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement memory allotee                                     */
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
#include "v_blkmem.h"
#include "v_lib.h"
#include "v_int.h"
#include "v_io.h"
#include "vos_id.h"

#ifdef VOS_VENUS_TEST_STUB
#include "stdio.h"
#endif

#define    THIS_FILE_ID        FILE_ID_V_BLKMEM_C

/* message control blocks */
VOS_MEM_CTRL_BLOCK     VOS_MemCtrlBlk[VOS_MEM_CTRL_BLOCK_NUMBER];

#define VOS_MEM_HEAD_BLOCK_SIZE             ( sizeof(VOS_MEM_HEAD_BLOCK) )

/* defination of message buffer */
extern VOS_MEM_BLOCK_INFO MsgBlockInfo[];

/* be used the head and tail of memory to protect overflow */
#define VOS_MEMORY_PROTECT_SIZE                                4

/* the number of words which on the head of the user's space */
#define VOS_MEMORY_RESERVED_WORD_HEAD                          2

/* the number of words which on the tail of the user's space */
#define VOS_MEMORY_RESERVED_WORD_TAIL                          1

/* the number of bytes which on the user's space */
#define VOS_MEMORY_RESERVED_BYTES \
    ((sizeof(VOS_UINT32) * VOS_MEMORY_RESERVED_WORD_HEAD) \
    + (sizeof(VOS_UINT32) * VOS_MEMORY_RESERVED_WORD_TAIL))

/* the begin address of user's space */
VOS_UINT32 gulVosSpaceStart;

/* the end address of user's space */
VOS_UINT32 gulVosSpaceEnd;

/* the begin address of user's space */
VOS_UINT32 gulVosSpaceAndProtectionStart;

/* the begin address of user's space */
VOS_UINT32 gulVosSpaceAndProtectionEnd;

/* msg space, keep lint happy */
VOS_UINT8  g_ulMsgPool[VOS_MSG_POOL_SIZE]={0};

/*****************************************************************************
 Function   : VOS_MemCtrlBlkInit_1
 Description: Init one memory control block
 Input      : VOS_MemCtrlBlock -- address of control block
            : ulBlockLength -- length
            : ulTotalBlockNumber -- number
            : ulAddress -- the start of address
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_MemCtrlBlkInit( VOS_MEM_CTRL_BLOCK *VOS_MemCtrlBlock,
                      VOS_INT ulBlockLength, VOS_INT ulTotalBlockNumber,
                      VOS_UINT32 *ulCtrlAddress, VOS_UINT32 *ulSpaceAddress )
{
    VOS_UINT32           ulLength;
    VOS_INT              i;
    VOS_MEM_HEAD_BLOCK   *Block;
    VOS_UINT32           ulTemp;
    VOS_UINT32           *pulTemp;

    VOS_MemCtrlBlock->BlockLength = ulBlockLength;
    VOS_MemCtrlBlock->TotalBlockNumber = ulTotalBlockNumber;
    VOS_MemCtrlBlock->IdleBlockNumber = ulTotalBlockNumber;

    ulLength = (VOS_UINT32)(VOS_MEMORY_RESERVED_BYTES + (VOS_UINT32)ulBlockLength);

    VOS_MemCtrlBlock->Buffer = *ulSpaceAddress;
    VOS_MemCtrlBlock->BufferEnd = *ulSpaceAddress
        + (ulLength * (VOS_UINT32)ulTotalBlockNumber);

    VOS_MemCtrlBlock->Blocks = (VOS_MEM_HEAD_BLOCK *)(*ulCtrlAddress);

    Block = VOS_MemCtrlBlock->Blocks;

    ulTemp = VOS_MemCtrlBlock->Buffer;

    for( i=1; i<ulTotalBlockNumber; i++)
    {
        Block->ulMemCtrlAddress = (VOS_UINT32)VOS_MemCtrlBlock;
        Block->ulMemAddress = ulTemp;
        Block->ulMemUsedFlag = CONTROL_BLOCK_IDLE;
        Block->pstNext = Block + 1;

        /* add protection on head */
        pulTemp = (VOS_UINT32 *)ulTemp;
        *pulTemp = (VOS_UINT32)Block;
        pulTemp++;
        *pulTemp = VOS_MEMORY_CRC;

        /* offset next */
        Block++;
        ulTemp += ulLength;

        /* add protection on tail */
        pulTemp = (VOS_UINT32 *)ulTemp;
        pulTemp--;
        *pulTemp = VOS_MEMORY_CRC;
    }
    Block->ulMemCtrlAddress = (VOS_UINT32)VOS_MemCtrlBlock;
    Block->ulMemAddress = ulTemp;
    Block->ulMemUsedFlag = CONTROL_BLOCK_IDLE;
    Block->pstNext = VOS_NULL_PTR;
     /* add protection on head */
    pulTemp = (VOS_UINT32 *)ulTemp;
    *pulTemp = (VOS_UINT32)Block;
    pulTemp++;
    *pulTemp = VOS_MEMORY_CRC;

    /* offset next */
    Block++;
    ulTemp += ulLength;

    /* add protection on tail */
    pulTemp = (VOS_UINT32 *)ulTemp;
    pulTemp--;
    *pulTemp = VOS_MEMORY_CRC;

    *ulCtrlAddress = (VOS_UINT32)Block;
    *ulSpaceAddress = ulTemp;

#if VOS_YES == VOS_MEMORY_CHECK
    VOS_MemCtrlBlock->BusyBlocks = VOS_NULL_PTR;
#endif

#if VOS_YES == VOS_MEMORY_COUNT
    VOS_MemCtrlBlock->MinIdleBlockNumber = ulTotalBlockNumber;
    VOS_MemCtrlBlock->MinSize = 0x0fffffff;
    VOS_MemCtrlBlock->MaxSize = 0;
    VOS_MemCtrlBlock->lRealNumber = 0;
    VOS_MemCtrlBlock->lMaxRealNumber = 0;
#endif
}

/*****************************************************************************
 Function   : VOS_MemCtrlBlkInit
 Description: Init all message control blocks
 Input      : VOS_VOID
            :
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_MemInit( VOS_VOID )
{
    VOS_INT    i;
    VOS_UINT32 lTotalSize = 0;
    VOS_UINT32 lTotalCtrlSize = 0;
    VOS_UINT32 ulSpaceStart;
    VOS_UINT32 ulSpaceEnd;
    VOS_UINT32 ulCtrlStart;

    /* calculate msg+mem's size */
    for ( i=0; i<VOS_MEM_CTRL_BLOCK_NUMBER; i++ )
    {
        lTotalSize += ( (VOS_UINT32)(MsgBlockInfo[i].size)
            + VOS_MEMORY_RESERVED_BYTES + VOS_MEM_HEAD_BLOCK_SIZE )
            * (VOS_UINT32)(MsgBlockInfo[i].number);
        lTotalCtrlSize +=
            VOS_MEM_HEAD_BLOCK_SIZE * (VOS_UINT32)(MsgBlockInfo[i].number);
    }

    /* add protected space */
    lTotalSize += 2 * VOS_MEMORY_PROTECT_SIZE;
    lTotalCtrlSize += VOS_MEMORY_PROTECT_SIZE;

    if ( VOS_MSG_POOL_SIZE < lTotalSize )
    {
        Print1("# OSA msg pool is too small.size %ld.\r\n",lTotalSize);

        return VOS_ERR;
    }

    ulCtrlStart = (VOS_UINT32)(&g_ulMsgPool[0]);

    ulSpaceStart = ulCtrlStart + lTotalCtrlSize;
    gulVosSpaceStart = ulSpaceStart;

    ulSpaceEnd = (ulCtrlStart + lTotalSize) - VOS_MEMORY_PROTECT_SIZE ;
    gulVosSpaceEnd = ulSpaceEnd;

    gulVosSpaceAndProtectionStart = gulVosSpaceStart - VOS_MEMORY_PROTECT_SIZE;

    gulVosSpaceAndProtectionEnd = gulVosSpaceEnd + VOS_MEMORY_PROTECT_SIZE;

    for ( i=0; i<VOS_MEM_CTRL_BLOCK_NUMBER; i++ )
    {
        VOS_MemCtrlBlkInit( &VOS_MemCtrlBlk[i],  MsgBlockInfo[i].size,
            MsgBlockInfo[i].number, &ulCtrlStart, &ulSpaceStart);
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_MemCtrlBlkMalloc
 Description: allocate a block
 Input      : VOS_MemCtrlBlock -- block address
            : ulLength -- length
            : alloc_info -- who allocate
 Return     : the address of the block
 Other      :
 *****************************************************************************/
VOS_VOID* VOS_MemCtrlBlkMalloc( VOS_MEM_CTRL_BLOCK *VOS_MemCtrlBlock,
                                VOS_INT ulLength )
{
    VOS_CPU_SR           CpuLockLevel;
    VOS_MEM_HEAD_BLOCK   *Block;

    CpuLockLevel = VOS_SplIMP();

    if( 0 >= VOS_MemCtrlBlock->IdleBlockNumber )
    {
        VOS_Splx(CpuLockLevel);
        return((VOS_VOID*)VOS_NULL_PTR);
    }

    VOS_MemCtrlBlock->IdleBlockNumber--;
    Block = VOS_MemCtrlBlock->Blocks;
    Block->ulMemUsedFlag = CONTROL_BLOCK_BUSY;
    VOS_MemCtrlBlock->Blocks = Block->pstNext;

#if VOS_YES == VOS_MEMORY_CHECK
    Block->pstPre = VOS_NULL_PTR;
    Block->pstNext = VOS_MemCtrlBlock->BusyBlocks;
    if ( VOS_NULL_PTR != VOS_MemCtrlBlock->BusyBlocks )
    {
        VOS_MemCtrlBlock->BusyBlocks->pstPre = Block;
    }
    VOS_MemCtrlBlock->BusyBlocks = Block;
#endif

    VOS_Splx(CpuLockLevel);

#if VOS_YES == VOS_MEMORY_COUNT
    /* record the usage of control block */
    if(VOS_MemCtrlBlock->IdleBlockNumber < VOS_MemCtrlBlock->MinIdleBlockNumber)
    {
        VOS_MemCtrlBlock->MinIdleBlockNumber
            = VOS_MemCtrlBlock->IdleBlockNumber;
    }
#endif

    return (VOS_VOID *)Block->ulMemAddress;
}

/*****************************************************************************
 Function   : VOS_MemCtrlBlkFree
 Description: free a block
 Input      :
            :
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_MemCtrlBlkFree( VOS_MEM_CTRL_BLOCK *VOS_MemCtrlBlock,
                               VOS_MEM_HEAD_BLOCK *Block,
                               VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_CPU_SR           CpuLockLevel;
#if VOS_YES == VOS_MEMORY_COUNT
    VOS_MEM_CTRL_BLOCK   *pstTemp;
#endif

    CpuLockLevel = VOS_SplIMP();

    if ( CONTROL_BLOCK_IDLE == Block->ulMemUsedFlag )
    {
        VOS_Splx(CpuLockLevel);

        VOS_SetErrorNo(VOS_ERRNO_MEMORY_FREE_REPECTION);
        LogPrint3("# Free Msg again.F %ld L %ld.Tick %ld.\r\n",
            ulFileID, usLineNo, VOS_GetTick() );

        return VOS_ERR;
    }

#if VOS_YES == VOS_MEMORY_CHECK
    if ( Block == VOS_MemCtrlBlock->BusyBlocks )
    {
        VOS_MemCtrlBlock->BusyBlocks = Block->pstNext;
        if ( VOS_NULL_PTR != VOS_MemCtrlBlock->BusyBlocks )
        {
            VOS_MemCtrlBlock->BusyBlocks->pstPre= VOS_NULL_PTR;
        }
    }
    else
    {
        Block->pstPre->pstNext = Block->pstNext;
        if ( VOS_NULL_PTR != Block->pstNext )
        {
            (Block->pstNext)->pstPre = Block->pstPre;
        }
    }
#endif

    Block->ulMemUsedFlag = CONTROL_BLOCK_IDLE;
    Block->pstNext = VOS_MemCtrlBlock->Blocks;
    VOS_MemCtrlBlock->Blocks = Block;

    VOS_MemCtrlBlock->IdleBlockNumber++;

#if VOS_YES == VOS_MEMORY_COUNT
    pstTemp = (VOS_MEM_CTRL_BLOCK *)(Block->ulRealCtrlAddr);
    pstTemp->lRealNumber--;
#endif

    VOS_Splx(CpuLockLevel);

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_MemBlkMalloc
 Description: allocate messagge memory
 Input      :
            :
 Return     : address
 Other      :
 *****************************************************************************/
VOS_VOID * VOS_MemBlkMalloc( VOS_PID PID, VOS_INT ulLength,
                            VOS_UINT32 ulFileID, VOS_INT32 usLineNo )
{
    VOS_INT              i;
    VOS_UINT32           *pulSpace;
#if VOS_YES == VOS_MEMORY_CHECK
    VOS_MEM_HEAD_BLOCK   *pstTemp;
#endif
#if VOS_YES == VOS_MEMORY_COUNT
    VOS_BOOL             ulFlag = TRUE;
    VOS_INT              lTempSuffix = 0;
#endif

    if( 0 == ulLength )
    {
        return( VOS_NULL_PTR );
    }

    /* No timer.Modified by regenmann at 2009-7-8 */
    /* for ( i=0; i<VOS_MEM_CTRL_BLOCK_NUMBER-1; i++ ) */
    for ( i=0; i<VOS_MEM_CTRL_BLOCK_NUMBER; i++ )
    {
        if( ulLength <= MsgBlockInfo[i].size )
        {
#if VOS_YES == VOS_MEMORY_COUNT
            if ( TRUE == ulFlag )
            {
                ulFlag = FALSE;
                lTempSuffix = i;
            }
#endif
            pulSpace = (VOS_UINT32 *)VOS_MemCtrlBlkMalloc( &VOS_MemCtrlBlk[i],
                ulLength );
            if( VOS_NULL_PTR != pulSpace )
            {
#if VOS_YES == VOS_MEMORY_CHECK
                pstTemp = (VOS_MEM_HEAD_BLOCK *)(*pulSpace);
                pstTemp->ulAllocSize = (VOS_UINT32)ulLength;
                pstTemp->ulcputickAlloc = VOS_GetTick();
                pstTemp->ulAllocPid = PID;
                pstTemp->aulMemRecord[0] = ulFileID;
                pstTemp->aulMemRecord[1] = (VOS_UINT32)usLineNo;
#endif

#if VOS_YES == VOS_MEMORY_COUNT
                pstTemp->ulRealCtrlAddr
                    = (VOS_UINT32)(&VOS_MemCtrlBlk[lTempSuffix]);
                VOS_MemCtrlBlk[lTempSuffix].lRealNumber++;

                if ( VOS_MemCtrlBlk[lTempSuffix].lRealNumber
                    > VOS_MemCtrlBlk[lTempSuffix].lMaxRealNumber )
                {
                    VOS_MemCtrlBlk[lTempSuffix].lMaxRealNumber
                        = VOS_MemCtrlBlk[lTempSuffix].lRealNumber;
                }

                if ( VOS_MemCtrlBlk[lTempSuffix].MaxSize < ulLength )
                {
                    VOS_MemCtrlBlk[lTempSuffix].MaxSize = ulLength;
                }

                if ( VOS_MemCtrlBlk[lTempSuffix].MinSize > ulLength)
                {
                    VOS_MemCtrlBlk[lTempSuffix].MinSize = ulLength;
                }
#endif

                /* offset space which be reserved of OSA */
                pulSpace += VOS_MEMORY_RESERVED_WORD_HEAD;
                return (VOS_VOID *)pulSpace;
            }
        }
    }

    VOS_SetErrorNo(VOS_ERRNO_MSG_MEMORY_FULL);

    LogPrint3("# alloc msg fail size %ld.F %ld L %ld.\r\n",
        (VOS_INT)ulLength, (VOS_INT)ulFileID, usLineNo);

    return( VOS_NULL_PTR );
}

/*****************************************************************************
 Function   : VOS_MemCheck
 Description: check memory is OSA's or not
 Input      :
            :
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_MemCheck( VOS_VOID *pAddr, VOS_UINT32 *pulBlock,
                         VOS_UINT32 *pulCtrl, VOS_UINT32 ulFileID,
                         VOS_INT32 usLineNo )
{
    VOS_UINT32          *pulTemp;
    VOS_UINT32          ulHeadValue;
    VOS_UINT32          ulTailValue;
    VOS_UINT32          ulBlockAddress;
    VOS_MEM_HEAD_BLOCK  *pstHeadBlock;
    VOS_MEM_CTRL_BLOCK  *pstMemCtrl;
    VOS_UINT32          ulTempAddress;

    ulTempAddress = (VOS_UINT32)pAddr;

    if ( (ulTempAddress < gulVosSpaceAndProtectionStart)
        || (ulTempAddress > gulVosSpaceAndProtectionEnd) )
    {
        VOS_SetErrorNo(VOS_ERRNO_MEMORY_NON_DOPRAMEM);

        LogPrint2("# NOT OSA MEM: F %ld L %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo);

        return(VOS_ERRNO_MEMORY_NON_DOPRAMEM);
    }

    if ( (ulTempAddress <= gulVosSpaceStart)
        || (ulTempAddress >= gulVosSpaceEnd) )
    {
        VOS_SetErrorNo(VOS_ERRNO_MEMORY_DOPRAMEM_OVERFLOW);

        LogPrint2("# OSA MEM OVERFLOW: F %ld L %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo);

        return(VOS_ERRNO_MEMORY_DOPRAMEM_OVERFLOW);
    }

    pulTemp = (VOS_UINT32 *)pAddr;

    pulTemp -= VOS_MEMORY_RESERVED_WORD_HEAD;

    ulBlockAddress = *pulTemp;
    pulTemp++;
    ulHeadValue = *pulTemp;
    pulTemp++;

    if ( VOS_MEMORY_CRC != ulHeadValue )
    {
        VOS_SetErrorNo(VOS_ERRNO_MEMORY_HEAD_COVER);

        LogPrint2("# MEM HEAD FAIL: F %ld L %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo);

        return(VOS_ERRNO_MEMORY_HEAD_COVER);
    }

    *pulBlock = ulBlockAddress;

    pstHeadBlock = (VOS_MEM_HEAD_BLOCK *)ulBlockAddress;
    ulTempAddress = pstHeadBlock->ulMemCtrlAddress;
    pstMemCtrl = (VOS_MEM_CTRL_BLOCK *)ulTempAddress;

    *pulCtrl = ulTempAddress;

    pulTemp = (VOS_UINT32 *)( (VOS_UINT32)pulTemp
        + (VOS_UINT32)(pstMemCtrl->BlockLength) );

    ulTailValue = *pulTemp;

    if ( VOS_MEMORY_CRC != ulTailValue )
    {
        VOS_SetErrorNo(VOS_ERRNO_MEMORY_TAIL_COVER);

        LogPrint2("# MEM TAIL FAIL: F %ld L %ld.\r\n",
            (VOS_INT)ulFileID, usLineNo);
        return(VOS_ERRNO_MEMORY_TAIL_COVER);
    }

    return VOS_OK;
}

/*****************************************************************************
 Function   : VOS_IsMemUsed
 Description: deliver the msg to a task.
 Input      : Pid  -- PID who send the message
              pMsg -- the pointer of message
              Pri -- the priority of msg
 Return     : VOS_OK on success and error code on failure
 Other      : After sending message, the status would be changed to ready.
 *****************************************************************************/
VOS_UINT32 VOS_IsMemUsed( VOS_UINT32 ulInterAddr )
{
    VOS_UINT32          ulBlockAdd;
    VOS_UINT32          ulCtrlkAdd;
    VOS_MEM_HEAD_BLOCK  *pstHead;

    if ( VOS_NULL == ulInterAddr )
    {
        return FALSE;
    }

    if ( VOS_OK != VOS_MemCheck( (VOS_VOID*)ulInterAddr, &ulBlockAdd, &ulCtrlkAdd,
        VOS_FILE_ID, __LINE__ ) )
    {
        return FALSE;
    }

    pstHead = (VOS_MEM_HEAD_BLOCK *)ulBlockAdd;

    if ( CONTROL_BLOCK_IDLE == pstHead->ulMemUsedFlag )
    {
        return FALSE;
    }

    return TRUE;
}

#if VOS_YES == VOS_MEMORY_COUNT
/*****************************************************************************
 Function   : VOS_show_memory_info
 Description: print memory use info
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      : only for designer
 *****************************************************************************/
VOS_VOID VOS_show_memory_info()
{
    VOS_UINT32 i;

    for ( i=0; i<VOS_MEM_CTRL_BLOCK_NUMBER; i++)
    {
        Print2("MSG %d Max is %ld.\r\n",i,VOS_MemCtrlBlk[i].MaxSize);
        Print2("MSG %d Min is %ld.\r\n",i,VOS_MemCtrlBlk[i].MinSize);
        Print2("MSG %d Real number is %ld.\r\n",i,VOS_MemCtrlBlk[i].lMaxRealNumber);
        Print2("MSG %d Max use is %ld.\r\n",i,
            VOS_MemCtrlBlk[i].TotalBlockNumber
            - VOS_MemCtrlBlk[i].MinIdleBlockNumber);
        Print("\r\n");
    }
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


