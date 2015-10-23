/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: win32_sem.c                                                     */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement win32 semaphore                                    */
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
extern "C"{
#endif /* __cplusplus */
#endif /* __cplusplus */

#include "vos_config.h"
#include "v_IO.h"
#include "v_task.h"
#include "v_int.h"
#include "v_lib.h"



#if (VOS_WIN32 == VOS_OS_VER)

#define    THIS_FILE_ID        FILE_ID_V_WIN32_SEM_C

#define VOS_MAX_SEM_NAME_LENGTH                             8

typedef struct SEM_CONTROL_STRU
{
    VOS_UINT32                Flag;/* control block's state */
    struct SEM_CONTROL_STRU   *SemId;/* the ID return to User */
    VOS_UINT32                SemFlags;
    VOS_UINT32                SemInitCount;
    VOS_CHAR                  Name[VOS_MAX_SEM_NAME_LENGTH];
    HANDLE                    Win32Handle;/* maped Win32 Handle */
} SEM_CONTROL_BLOCK;

/* the number of queue's control block */
VOS_UINT32              vos_SemCtrlBlkNumber;

/* the start address of queue's control block */
SEM_CONTROL_BLOCK       *vos_SemCtrlBlk;

/* the Max usage of queue */
VOS_UINT32              vos_SemMaxSemId;

/*****************************************************************************
 Function   : VOS_SemCtrlBlkInit
 Description: Init semaphore's control block
 Input      : ulSemCtrlBlkNumber -- number
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_SemCtrlBlkInit(VOS_UINT32 ulSemCtrlBlkNumber)
{
    VOS_UINT32 i;

    vos_SemCtrlBlkNumber = ulSemCtrlBlkNumber;

    vos_SemCtrlBlk = (SEM_CONTROL_BLOCK*)
                      malloc( ulSemCtrlBlkNumber
                            * sizeof(SEM_CONTROL_BLOCK) );
    if(VOS_NULL_PTR == vos_SemCtrlBlk)
    {
        Print("# VOS_SemCtrlBlkInit malloc Error.\r\n");
        return(VOS_ERR);
    }

    for(i=0; i<vos_SemCtrlBlkNumber; i++)
    {
        vos_SemCtrlBlk[i].Flag  = CONTROL_BLOCK_IDLE;
        vos_SemCtrlBlk[i].SemId = vos_SemCtrlBlk+i;
    }

    vos_SemMaxSemId = 0;

    return(VOS_OK);
}

/*****************************************************************************
 Function   : VOS_SemCtrlBlkGet
 Description: allocate a control block
 Input      : VOS_VOID
 Return     : address
 Other      :
 *****************************************************************************/
SEM_CONTROL_BLOCK *VOS_SemCtrlBlkGet(VOS_VOID)
{
    VOS_UINT32      i;
    VOS_CPU_SR      CpuLockLevel;

    CpuLockLevel = VOS_SplIMP();

    for(i=0; i<vos_SemCtrlBlkNumber; i++)
    {
        if(vos_SemCtrlBlk[i].Flag == CONTROL_BLOCK_IDLE)
        {
            vos_SemCtrlBlk[i].Flag = CONTROL_BLOCK_BUSY;
            break;
        }
    }

    VOS_Splx(CpuLockLevel);

    if( i < vos_SemCtrlBlkNumber)
    {
        /* record the max usage of SEM */
        if ( i > vos_SemMaxSemId )
        {
            vos_SemMaxSemId = i;
        }

        return vos_SemCtrlBlk+i;
    }
    else
    {
        LogPrint("# VOS_GetSemCtrlBlk no Idle.\r\n");

        VOS_SetErrorNo(VOS_ERRNO_SEMA4_FULL);

        return(VOS_NULL_PTR);
    }
}

/*****************************************************************************
 Function   : VOS_SemCtrlBlkFree
 Description: fress a block
 Input      : Sem_Address -- address
 Return     : VOS_OK on success or errno on failure.
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_SemCtrlBlkFree( SEM_CONTROL_BLOCK *Sem_Address )
{
    VOS_CPU_SR             CpuLockLevel;

    if( Sem_Address == Sem_Address->SemId )
    {
        if(Sem_Address->Flag == CONTROL_BLOCK_IDLE)
        {
            return(VOS_ERR);
        }
        else
        {
            CpuLockLevel = VOS_SplIMP();

            Sem_Address->Flag = CONTROL_BLOCK_IDLE;

            VOS_Splx(CpuLockLevel);
        }

        return(VOS_OK);
    }
    else
    {
        Print("# VOS_FreeSemCtrlBlk free NULL Sem.\r\n");

        return(VOS_ERR);
    }
}

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
    VOS_UINT32             i;
    SEM_CONTROL_BLOCK      *iSemId;
    HANDLE                 pulSemId;

    iSemId = VOS_SemCtrlBlkGet();

    if( iSemId == VOS_NULL_PTR)
    {
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_CCREATE_OBJTFULL);
        return(VOS_ERRNO_SEMA4_CCREATE_OBJTFULL);
    }

    if(ulSmInit == VOS_NULL_DWORD)
    {
        pulSemId = CreateSemaphore( VOS_NULL_PTR, (VOS_INT32)1, 1, VOS_NULL_PTR );
    }
    else
    {
        pulSemId = CreateSemaphore( VOS_NULL_PTR, (VOS_INT32)ulSmInit,\
                                    0x400000, VOS_NULL_PTR );
    }

    if(pulSemId == VOS_NULL_PTR)
    {
        VOS_SemCtrlBlkFree(iSemId);
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_CCREATE_OBJTFULL);
        return(VOS_ERRNO_SEMA4_CCREATE_OBJTFULL);
    }
    else
    {
        *pulSmID = (VOS_UINT32)iSemId;

        if ( VOS_NULL_PTR != acSmName )
        {
            for(i=0; i<VOS_MAX_SEM_NAME_LENGTH/2; i++)
            {
                iSemId->Name[i]  = acSmName[i];
            }
            iSemId->Name[VOS_MAX_SEM_NAME_LENGTH/2]  = '\0';
        }
        else
        {
            iSemId->Name[0] = '\0';
        }

        iSemId->SemFlags     = ulFlags;
        iSemId->SemInitCount = ulSmInit;
        iSemId->Win32Handle  = pulSemId;

        return(VOS_OK);
    }
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
    SEM_CONTROL_BLOCK  *temp_Ptr;
    VOS_UINT32         TimeOut;
    VOS_UINT32         ulerror;


    temp_Ptr = (SEM_CONTROL_BLOCK *)Sm_ID;

    if( temp_Ptr != temp_Ptr->SemId )
    {
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_P_IDERR);
        return (VOS_ERRNO_SEMA4_P_IDERR);
    }

    if (CONTROL_BLOCK_IDLE == temp_Ptr->Flag)
    {
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_P_NOTACTIVE);
        return(VOS_ERRNO_SEMA4_P_NOTACTIVE);
    }

    if ( ulTimeOutInMillSec == 0 )
    {
        TimeOut = INFINITE;
    }
    else
    {
        TimeOut = ulTimeOutInMillSec;
    }

    ulerror = WaitForSingleObject( temp_Ptr->Win32Handle, TimeOut );

    if ( WAIT_OBJECT_0 == ulerror )
    {
        return VOS_OK;
    }

    if ( WAIT_TIMEOUT == ulerror )
    {
        VOS_SetErrorNo (VOS_ERRNO_SEMA4_P_TIMEOUT);
        return VOS_ERRNO_SEMA4_P_TIMEOUT;
    }

    VOS_SetErrorNo (VOS_ERRNO_SEMA4_P_CANOTP);
    return VOS_ERRNO_SEMA4_P_CANOTP;

}

/*****************************************************************************
 Function   : VOS_SmV
 Description: Release/Unlock the sema4 that has locked a resource
 Input      : ulSmID -- id of semaphore
 Return     : VOS_OK on success and errno on failure
 *****************************************************************************/
VOS_UINT32 VOS_SmV( VOS_UINT32 Sm_ID )
{
    SEM_CONTROL_BLOCK  *temp_Ptr;
    VOS_UINT32         ulerror;

    temp_Ptr = (SEM_CONTROL_BLOCK *)Sm_ID;

    if( temp_Ptr == temp_Ptr->SemId )
    {
        if( temp_Ptr->Flag == CONTROL_BLOCK_IDLE)
        {
            VOS_SetErrorNo(VOS_ERRNO_SEMA4_V_NOTACTIVE);
            return(VOS_ERRNO_SEMA4_V_NOTACTIVE);
        }

        ulerror = (VOS_UINT32)ReleaseSemaphore( temp_Ptr->Win32Handle,\
                                                1, VOS_NULL_PTR );

        if( 0 == ulerror )
        {
            VOS_SetErrorNo(VOS_ERRNO_SEMA4_V_NOTACTIVE);
            return(VOS_ERRNO_SEMA4_V_NOTACTIVE);
        }
        else
        {
            return VOS_OK;
        }
    }
    else
    {
        VOS_SetErrorNo(VOS_ERRNO_SEMA4_V_IDERR);
        return(VOS_ERRNO_SEMA4_V_IDERR);
    }
}

/*****************************************************************************
 Function   : VOS_show_sem_info
 Description: print the usage info of Sem
 Input      : VOS_VOID
 Return     : VOS_VOID
 Other      :
 *****************************************************************************/
VOS_VOID VOS_show_sem_info( VOS_VOID )
{
    Print1("Max be used sem is %ld.\r\n",vos_SemMaxSemId);
}


VOS_VOID VOS_Win32ReleaseAllSem(VOS_VOID)
{
    VOS_UINT32 i;

    if(VOS_NULL_PTR == vos_SemCtrlBlk)
    {
        return;
    }

    for(i=0; i<vos_SemCtrlBlkNumber; i++)
    {
        if (vos_SemCtrlBlk[i].Win32Handle != VOS_NULL_PTR)
        {
            ReleaseSemaphore(vos_SemCtrlBlk[i].Win32Handle,
                             0,
                             vos_SemCtrlBlk[i].SemInitCount);
        }
    }

    free(vos_SemCtrlBlk);
    vos_SemCtrlBlk = VOS_NULL_PTR;
}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



