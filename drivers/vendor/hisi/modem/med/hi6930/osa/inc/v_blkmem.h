/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_blkmen.h                                                      */
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

#ifndef _VOS_BLOCK_MEM_H
#define _VOS_BLOCK_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#include "v_typdef.h"

typedef struct MEM_HEAD_BLOCK
{
    VOS_UINT32        ulMemCtrlAddress;/* the address of  VOS_MEM_CTRL_BLOCK */
    VOS_UINT32        ulMemAddress;/* the address of User's */
    VOS_UINT32        ulMemUsedFlag;/* whether be used or not */
    struct MEM_HEAD_BLOCK  *pstNext;/*  next block whether allocated or not */

#if VOS_YES == VOS_MEMORY_CHECK
    struct MEM_HEAD_BLOCK  *pstPre;/* the allocated block's previous */
    VOS_UINT32        ulAllocSize;/* the actual allocated size */
    VOS_UINT32        ulcputickAlloc;/* CPU tick of message allocation */
    VOS_UINT32        ulAllocPid;/* Pid who alloc the memory */
    VOS_UINT32        aulMemRecord[8];/* record something of user */
#endif

#if VOS_YES == VOS_MEMORY_COUNT
    VOS_UINT32        ulRealCtrlAddr;/* which VOS_MEM_CTRL_BLOCK should be allocated */
#endif
} VOS_MEM_HEAD_BLOCK;

typedef struct
{
    VOS_INT             BlockLength;/* block size */
    VOS_INT             TotalBlockNumber;/* block number */
    VOS_UINT32          Buffer;/*start address of block */
    VOS_UINT32          BufferEnd;/*end address of block*/
    VOS_INT             IdleBlockNumber;/* the number of free block*/
    VOS_MEM_HEAD_BLOCK  *Blocks;/*list of free block*/

#if VOS_YES == VOS_MEMORY_CHECK
    VOS_MEM_HEAD_BLOCK  *BusyBlocks;/*list of busy block*/
#endif

#if VOS_YES == VOS_MEMORY_COUNT
    VOS_INT             MinIdleBlockNumber;/*the MIN value of free block*/
    VOS_INT             MinSize;/* the Min allocated size */
    VOS_INT             MaxSize;/* the Max allocated size */
    VOS_INT             lRealNumber;
    VOS_INT             lMaxRealNumber;
#endif
} VOS_MEM_CTRL_BLOCK;

VOS_UINT32 VOS_MemInit( VOS_VOID );

VOS_VOID * VOS_MemBlkMalloc( VOS_PID PID, VOS_INT ulLength,
                            VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

VOS_UINT32 VOS_MemCheck( VOS_VOID *pAddr, VOS_UINT32 *pulBlock,
                         VOS_UINT32 *pulCtrl, VOS_UINT32 ulFileID,
                         VOS_INT32 usLineNo );

VOS_UINT32 VOS_MemCtrlBlkFree( VOS_MEM_CTRL_BLOCK *VOS_MemCtrlBlock,
                               VOS_MEM_HEAD_BLOCK *Block,
                               VOS_UINT32 ulFileID, VOS_INT32 usLineNo );

VOS_UINT32 VOS_IsMemUsed( VOS_UINT32 ulInterAddr );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_BLOCK_MEM_H */

