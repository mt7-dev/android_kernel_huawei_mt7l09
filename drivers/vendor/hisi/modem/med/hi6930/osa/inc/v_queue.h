/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_queue.h                                                       */
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

#ifndef _VOS_QUEUE_H
#define _VOS_QUEUE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "v_msg.h"
#include "vos_config.h"


VOS_UINT32 VOS_QueueCtrlBlkInit(VOS_UINT32 ulQueueCtrlBlcokNumber);

VOS_UINT32 VOS_FixedQueueCreate(VOS_UINT32 ulLength, VOS_UINT32 *pulQueueID);

VOS_UINT32 VOS_FixedQueueWrite(VOS_UINT32 ulQueueID, VOS_VOID * pBufferAddr,
                               VOS_UINT32 Pri);

VOS_UINT32 VOS_FixedQueueRead(VOS_UINT32 ulQueueID, VOS_UINT32 *pBufferAddr);

VOS_VOID* VOS_OutMsg( VOS_UINT32 ulQueueID );

VOS_UINT32 VOS_FixedQueueClean( VOS_UINT32 ulQueueID, VOS_UINT32 *pBufferAddr );

VOS_UINT16 VOS_PeekQueue( VOS_VOID );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_QUEUE_H */
