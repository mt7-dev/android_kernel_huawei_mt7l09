

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "v_fileid.h"
#include "stdio.h"
#include "venus_stub.h"
#include <xtensa/tie/xt_hifi2.h>
#include "drv_mailbox.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#ifdef VOS_VENUS_TEST_STUB
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_UINT32 g_auwVosTestMessageBuf[256]={0};
VOS_UINT32 g_uwTestSem;
ae_p24x2s  g_aeVar0;
ae_p24x2s  g_aeVar1;
VOS_UINT16 g_uhwStartIrq=0;
VOS_UINT32 g_uwTestPos=0;
VOS_UINT32 g_ulTestResetCount = 0;
VOS_UINT8 g_ucVosTestMsgBuf[4] = {1,2,3,4};
VOS_UINT8 g_ucVosTestMsgByte = 0;
VENUS_TEST_SEQ_STRU g_stTestSeq;
VENUS_MSG_FILE_STRU g_stMsgFile;
VOS_UINT32 g_uwPidTbl[] = {DSP_PID_VOICE_RT,DSP_PID_VOICE,DSP_PID_AUDIO_RT,DSP_PID_AUDIO,DSP_PID_HIFI_OM};
VOS_UINT32 g_uwNmiFlag = 0;
VOS_UINT32 g_uwVosLongTimeTestFlag = 0;

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#define THIS_FILE_ID        9999 //FILE_ID_TEST_C
VOS_VOID VOS_IdleProcess( VOS_VOID);

#define _XTSTR(x) #x
#define XTSTR(x) _XTSTR(x)

static __inline__ VOS_UINT32 read_ccount()
{
    VOS_UINT32 ccount;

    __asm__ __volatile__ (
        "rsr     %0, "XTSTR(CCOUNT)
        : "=a" (ccount)
        );
    return ccount;
}

VOS_UINT32 VOS_TestSendMsg(VOS_UINT32 ulDstPid, VOS_UINT32 ulLength, VOS_UINT32 ulSrcPid);
VOS_VOID VOS_RandMsg();


VOS_UINT32 Calculate(VOS_UINT32 ulStart,VOS_UINT32 ulEnd)
 {
     VOS_UINT32 i;
     VOS_UINT32 ulSum=0;
     VOS_UINT32 uhwTmp[37];

     memset(uhwTmp,0x55,sizeof(uhwTmp));

     for (i =  ulStart;i < ulEnd;i++)
     {
         ulSum += i * i;
         uhwTmp[0] = ulSum;
     }
     VOS_DEBUG_LOG_CPU_INFO(ulSum);

     return ulSum;
 }


#if VENUS_ST_OS

VOS_VOID VOS_IntHandlers();
VOS_VOID VOS_UserexcHandler( VOS_UINT32 uwExceptionNo);


VOS_VOID VOS_TaskSwHook( VOS_VOID *tcb_old, VOS_VOID *tcb_new)
{
    VOS_UINT32 uwLogInfo[2];

    uwLogInfo[0] = g_pstVosTCBCur->OSTCBPrio;
    uwLogInfo[1] = g_pstVosTCBHighRdy->OSTCBPrio;
    VOS_VenusLogInfo(uwLogInfo,2);

}

VOS_VOID VOS_MsgGetHook( VOS_VOID *pMsg)
{
    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_MSG_GET_HOOK);

}

VOS_VOID VOS_EntIntHook( VOS_UINT32 uwIntNo )
{

    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_MSG_ENTER_INT_HOOK + uwIntNo);
}

VOS_VOID VOS_ExitIntHook( VOS_UINT32 uwIntNo )
{
    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_MSG_EXIT_INT_HOOK + uwIntNo);
}



VOS_UINT32 VOS_TestSendEmergentMsg(VOS_UINT32 ulDstPid, VOS_UINT32 ulLength, VOS_UINT32 ulSrcPid)
{
    MSG_CB          *pMsg;
    VOS_UINT32      ulReturnValue;

    VOS_UINT32 i;

    pMsg = (MSG_CB*)VOS_AllocMsg(ulSrcPid, ulLength);
    if (pMsg == VOS_NULL_PTR)
    {
        Print("in VOS_TestSendMsg, Malloc Error ===");
        return VOS_ERR;
    }

    pMsg->uwSenderCpuId   = VOS_LOCAL_CPUID;
    pMsg->uwSenderPid     = ulSrcPid;
    pMsg->uwReceiverCpuId = VOS_LOCAL_CPUID;
    pMsg->uwReceiverPid   = ulDstPid;
    pMsg->uwLength        = ulLength;
    for (i=0; i<pMsg->uwLength; i++)
    {
        pMsg->aucValue[i] = g_ucVosTestMsgByte++;
    }

    ulReturnValue = VOS_SendUrgentMsg( ulDstPid, pMsg );
    if (ulReturnValue != VOS_OK)
    {
        Print("in VOS_TestSendMsg, Send Error ===");
    }

    return ulReturnValue;
}




VOS_UINT32 HIGHPidInit(enum  VOS_INIT_PHASE_DEFINE ip)
{

    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:
               break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
               break;
        case   VOS_IP_RESTART:
           break;
        case   VOS_IP_BUTT:
            break;
    }

    return VOS_OK;
}


VOS_VOID HIGHMsgProc (struct MsgCB* pMsg)
{
    static VOS_UINT16 x = 1;
    MSG_CB          *pTstMsg;

    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_ENTER_RT_MSGPROC);

    if (VOS_NULL_PTR == pMsg)
    {
        Print("HIGH receive message error.\r\n");

        return;
    }
    VOS_DEBUG_LOG_CPU_INFO((*((VOS_UINT16 *)(&pMsg->aucValue[0]))));


    switch ((*((VOS_UINT16 *)(&pMsg->aucValue[0]))) )
    {
        case OS_TEST_MSGID_RT_SEM_P:
            VOS_SmP(g_uwTestSem,0);
            break;

        case OS_TEST_MSGID_RT_CO_PROC:
            g_aeVar1 = AE_ABSP24(g_aeVar0);
            break;

        case OS_TEST_MSGID_RT_DIV_BY_ZERO:
            x = x / 0;
            break;

        case OS_TEST_MSGID_RT_CHECK_NMI:
            VOS_DEBUG_LOG_CPU_INFO(g_uwNmiFlag);
            break;

        case OS_TEST_MSGID_RT_SEND_TO_LOW:
            VOS_TestSendMsg(DSP_PID_VOICE,4,DSP_PID_VOICE_RT);
            break;

        case OS_TEST_MSGID_RT_SEM_V:
            VOS_SmV(g_uwTestSem);
            break;

        case OS_TEST_MSGID_SEM_RESET:
            VOS_DEBUG_LOG_CPU_INFO(((VOS_EVENT *)g_uwTestSem)->OSEventCnt);
            VOS_SmReset(g_uwTestSem, 1);
            VOS_DEBUG_LOG_CPU_INFO(((VOS_EVENT *)g_uwTestSem)->OSEventCnt);
            break;

        case OS_TEST_MSGID_MSG_HOOK:
            VOS_RegisterMsgGetHook(VOS_MsgGetHook);
            VOS_TestSendMsg(DSP_PID_VOICE,4,DSP_PID_VOICE_RT);
            VOS_TestSendMsg(DSP_PID_APM,4,DSP_PID_VOICE_RT);
            break;

        case OS_TEST_MSGID_ENTER_INT_HOOK:
            VOS_RegisterEnterIntrHook(VOS_EntIntHook);
            VOS_RegisterExitIntrHook(VOS_ExitIntHook);
            break;

        case OS_TEST_MSGID_STACK_SIZE:
            VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(IDLE_STK_ID));
            VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(DSP_FID_RT));
            VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(DSP_FID_NORMAL));
            break;

        case OS_TEST_MSGID_LONG_TIME:
            g_uwVosLongTimeTestFlag = 1;
            break;

        case OS_TEST_MSGID_RT_ALLOC:


            VOS_TaskLock();
            pTstMsg = (MSG_CB*)VOS_AllocMsg(DSP_PID_VOICE_RT, 512-24);
            VOS_DEBUG_LOG_CPU_INFO((VOS_UINT32)pTstMsg);

            pTstMsg = (MSG_CB*)VOS_AllocMsg(DSP_PID_VOICE_RT, 512-23);
            VOS_DEBUG_LOG_CPU_INFO((VOS_UINT32)pTstMsg);

            pTstMsg = (MSG_CB*)VOS_AllocMsg(DSP_PID_VOICE_RT, 64);
            VOS_DEBUG_LOG_CPU_INFO((VOS_UINT32)pTstMsg);

            pTstMsg = (MSG_CB*)VOS_AllocMsg(DSP_PID_VOICE_RT, 128);
            VOS_DEBUG_LOG_CPU_INFO((VOS_UINT32)pTstMsg);

            pTstMsg = (MSG_CB*)VOS_AllocMsg(DSP_PID_VOICE_RT, 128-24);
            VOS_DEBUG_LOG_CPU_INFO((VOS_UINT32)pTstMsg);

            VOS_TaskUnlock();
            break;

        case OS_TEST_MSGID_INT_STATISTIC:
            VOS_DEBUG_LOG_CLK();
            VOS_DisableInterrupt(OS_INTR_CONNECT_07);
            break;

        default:
            break;
    }

    if(g_uwVosLongTimeTestFlag == 1)
    {
        Calculate(0,100000);
    }

    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_EXIT_RT_MSGPROC);
}



VOS_UINT32 UCOM_COMM_RtFidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32 ulReturnCode;

    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:
            ulReturnCode = VOS_RegisterPIDInfo(DSP_PID_VOICE_RT,
                                  (Init_Fun_Type) HIGHPidInit,
                                      (Msg_Fun_Type) HIGHMsgProc  );
            if ( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterMsgTaskPrio(DSP_FID_RT, VOS_PRIORITY_P6 );
            if ( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }
            xthal_set_intset(0x80000002);
            xthal_set_intclear(0x80000002);
            xthal_set_intenable(0x00000000);

            VOS_RegTaskSwitchHook(VOS_TaskSwHook);
            VOS_RegIdleHook(VOS_IdleProcess);
            VOS_ConnectInterrupt(OS_INTR_CONNECT_01,VOS_IntHandlers);
            VOS_ConnectInterrupt(OS_INTR_CONNECT_02,VOS_IntHandlers_02);
            VOS_ConnectInterrupt(OS_INTR_CONNECT_07,VOS_IntHandlers_07);
            VOS_ConnectInterrupt(OS_INTR_CONNECT_31,VOS_IntHandlers);
            VOS_EnableInterrupt(OS_INTR_CONNECT_01);
            VOS_EnableInterrupt(OS_INTR_CONNECT_02);
            VOS_EnableInterrupt(OS_INTR_CONNECT_07);
            VOS_EnableInterrupt(OS_INTR_CONNECT_31);
            VOS_RegisterExceptionHandler(VOS_UserexcHandler);
            VOS_InitTest();
            break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
            break;
    }
    return VOS_OK;

}





VOS_UINT32 LOWPidInit( enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:
               break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
               break;
        case   VOS_IP_RESTART:
           break;
        case   VOS_IP_BUTT:
            break;
    }

    return VOS_OK;
}



VOS_VOID LOWMsgProc(struct MsgCB* pMsg)
{
    VOS_CPU_SR cr;
    VOS_UINT16 i;
    VOS_UINT32 uwRet;
    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_ENTER_MED_MSGPROC);

    if (VOS_NULL_PTR == pMsg)
    {
        Print("LOW receive message error.\r\n");

        return;
    }

    VOS_DEBUG_LOG_CPU_INFO((*((VOS_UINT16 *)(&pMsg->aucValue[0]))));

    switch ((*((VOS_UINT16 *)(&pMsg->aucValue[0]))))
    {
        case OS_TEST_MSGID_SEND_TO_RT:
            VOS_TestSendMsg(DSP_PID_VOICE_RT, 4, DSP_PID_MED_OM);
            break;

        case OS_TEST_MSGID_SEND_TO_RT_A:
            VOS_TaskLock();
            for ( i = 0; i <= VOS_TSK_RT_QUEUE_NUMBER; i++)
            {
                uwRet = VOS_TestSendMsg(DSP_PID_VOICE_RT, 4, DSP_PID_MED_OM);
                VOS_DEBUG_LOG_CPU_INFO(uwRet);
            }
            VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_FILL_MSGQ);
            VOS_TaskUnlock();
            break;

        case OS_TEST_MSGID_SEM_V:
            VOS_SmV(g_uwTestSem);
            break;

        case OS_TEST_MSGID_INT_SEM_V:
            g_uhwStartIrq = 1;
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_INT_LEVEL1:
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_INT_LEVEL3:
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_INT_LEVEL2:
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_INTER_CORE:
            VOS_TestSendMsg(DSP_PID_APM, 4, DSP_PID_MED_OM);
            break;

        case OS_TEST_MSGID_SEND_TO_RT_A_INT:
            g_uhwStartIrq = 2;
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_INTER_CORE_INT:
            g_uhwStartIrq = 3;
            Calculate(0,300000);
            break;

        case OS_TEST_MSGID_EMERGENT_MSG:

            VOS_TaskLock();
            uwRet = VOS_TestSendMsg(DSP_PID_VOICE_RT, 4, DSP_PID_MED_OM);
            VOS_DEBUG_LOG_CPU_INFO(uwRet);
            for ( i = 1; i <= VOS_TSK_RT_QUEUE_NUMBER; i++)
            {
                uwRet = VOS_TestSendEmergentMsg(DSP_PID_VOICE_RT,4,DSP_PID_MED_OM);
                VOS_DEBUG_LOG_CPU_INFO(uwRet);
            }
            VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_FILL_MSGQ);
            VOS_TaskUnlock();
            break;

        default:
            break;
    }

    if (g_uwVosLongTimeTestFlag == 1)
    {
        Calculate(0,5000);
    }

    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_EXIT_MED_MSGPROC);

}

VOS_UINT32 UCOM_COMM_NormalFidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32 ulReturnCode;
    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:
            ulReturnCode = VOS_RegisterPIDInfo(DSP_PID_VOICE,
                                  (Init_Fun_Type) LOWPidInit,
                                  (Msg_Fun_Type) LOWMsgProc);
            if( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(DSP_PID_MED_OM,
                                  (Init_Fun_Type) LOWPidInit,
                                  (Msg_Fun_Type) LOWMsgProc);
            if( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterMsgTaskPrio(DSP_FID_NORMAL,
                                                   VOS_PRIORITY_P5 );
            if( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }
            break;
        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
            break;
    }
    return VOS_OK;
}



VOS_VOID VOS_UserexcHandler( VOS_UINT32 uwExceptionNo)
{
    VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_USER_EXEC);
}


VOS_VOID VOS_IntHandlers()
{
    VOS_UINT32 i;
    VOS_UINT32 uwRet;

    if (g_uhwStartIrq == 1)
    {
        VOS_SmV(g_uwTestSem);
        VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_INT_REL_SEM);
        g_uhwStartIrq = 0;
        return;
    }
    else if (g_uhwStartIrq == 2)
    {
        for ( i = 0; i <= VOS_TSK_RT_QUEUE_NUMBER; i++)
        {
            uwRet = VOS_TestSendMsg(DSP_PID_VOICE_RT, 4, DSP_PID_MED_OM);
            VOS_DEBUG_LOG_CPU_INFO(uwRet);
        }
        VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_FILL_MSGQ);
        g_uhwStartIrq = 0;
        return;
    }
    else if (g_uhwStartIrq == 3)
    {
        for ( i = 0; i <= VOS_TSK_RT_QUEUE_NUMBER; i++)
        {
            uwRet = VOS_TestSendMsg(DSP_PID_APM, 4, DSP_PID_MED_OM);
            VOS_DEBUG_LOG_CPU_INFO(uwRet);
        }
        VOS_DEBUG_LOG_CPU_INFO(OS_TEST_FLAG_FILL_MSGQ);
        g_uhwStartIrq = 0;
        return;
    }
    else
    {
        return;
    }

    #if 0
    printf("\nclock=%d\nidle stack=[%08x,%08x],High stack=[%08x,%08x],Low  stack=[%08x,%08x]\r\n",
        read_ccount(),
        sizeof(g_stVosTaskIdleStk),
        GetMaxStackUsed(IDLE_STK_ID),
        GetFidTaskSize(DSP_FID_RT),
        GetMaxStackUsed(DSP_FID_RT),
        GetFidTaskSize(DSP_FID_NORMAL),
        GetMaxStackUsed(DSP_FID_NORMAL)
        );

    #endif
}


VOS_VOID VOS_IdleProcess( VOS_UINT32 ulVosCtxSw)
{


}
#endif


VOS_VOID VOS_IntHandlers_02()
{
    VOS_UINT32 uwTick;

    uwTick = VOS_DEBUG_GET_TICK();
    VOS_DEBUG_SIM_CLEAR_INTERRUPT(OS_INTR_CONNECT_02);
}


VOS_VOID VOS_IntHandlers_07()
{

    VOS_UINT32 uwTick;

    uwTick = VOS_DEBUG_GET_TICK();
    VOS_DEBUG_SIM_CLEAR_INTERRUPT(OS_INTR_CONNECT_07);
    VOS_TrigSendmsg(uwTick);

    if(g_uwVosLongTimeTestFlag == 1)
    {
        VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(IDLE_STK_ID));
        VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(DSP_FID_RT));
        VOS_DEBUG_LOG_CPU_INFO(VOS_GetMaxStackUsed(DSP_FID_NORMAL));
        VOS_RandMsg();
        VOS_RandMsg();
        VOS_RandMsg();
   }
}


VOS_UINT32 VOS_TestSendMsg(VOS_UINT32 ulDstPid, VOS_UINT32 ulLength, VOS_UINT32 ulSrcPid)
{
    MSG_CB          *pMsg;
    VOS_UINT32      ulReturnValue;

    VOS_UINT32 i;

    pMsg = (MSG_CB*)VOS_AllocMsg(ulSrcPid, ulLength);
    if (pMsg == VOS_NULL_PTR)
    {
        Print("in VOS_TestSendMsg, Malloc Error ===");
        return VOS_ERR;
    }

    pMsg->uwSenderCpuId   = VOS_LOCAL_CPUID;
    pMsg->uwSenderPid     = ulSrcPid;
    pMsg->uwReceiverCpuId = VOS_LOCAL_CPUID;
    pMsg->uwReceiverPid   = ulDstPid;
    pMsg->uwLength        = ulLength;
    for (i=0; i<pMsg->uwLength; i++)
    {
        pMsg->aucValue[i] = g_ucVosTestMsgByte++;
    }

    ulReturnValue = VOS_SendMsg( ulDstPid, pMsg );
    if (ulReturnValue != VOS_OK)
    {
        Print("in VOS_TestSendMsg, Send Error ===");
    }

    return ulReturnValue;
}


VOS_VOID VOS_RandMsg()
{
        VOS_UINT32 uwPidx1,uwPidx2;

        uwPidx1 = rand()%3;
        uwPidx2 = rand()%3;
        VOS_TestSendMsg(g_uwPidTbl[uwPidx1], 4, g_uwPidTbl[uwPidx2]);
}




VOS_VOID GetMsgFromFile()
{
    FILE *fp = fopen(".\\testmsg.bin","rb");
    if (fp == NULL)
    {
        LogPrint("open file 'testmsg.bin' error!");
        return;
    }
    fread(&g_stMsgFile,1,sizeof(VENUS_MSG_FILE_STRU),fp);
    fclose(fp);
}


VOS_VOID GetTestSeqFromFile()
{
    FILE *fp = fopen(".\\testseq.bin","rb");
    if (fp == NULL)
    {
        LogPrint("open file 'testseq.bin' error!");
        return;
    }
    fread(&g_stTestSeq,1,sizeof(VENUS_TEST_SEQ_STRU),fp);
    fclose(fp);
}


VOS_VOID VOS_TrigSendmsg(VOS_UINT32 uwTick)
{
    VOS_UINT32                          uwResult;
    VOS_VENUS_MSG_STRU                 *pstMsg;
    VOS_UINT16                          uhwOffset;
    MSG_CB                             *pstStubMsg;

    if ( (uwTick >= g_stTestSeq.stSeq[g_uwTestPos].uwTick) && ( g_uwTestPos < g_stTestSeq.uwCount))
    {
        if(g_stTestSeq.stSeq[g_uwTestPos].uwMsgIndex > 100)
        {
            g_uwTestPos ++;
            return;
        }

        uhwOffset = g_stMsgFile.uwIndex[g_stTestSeq.stSeq[g_uwTestPos].uwMsgIndex]/2;
        pstStubMsg = (MSG_CB *)&g_stMsgFile.uhwMsgBytes[uhwOffset];

        pstMsg    = ( VOS_VENUS_MSG_STRU * )( VOS_AllocMsg( DSP_PID_VOICE, pstStubMsg->uwLength ) );

        /* 如果可以分配空间 */
        if ( VOS_NULL_PTR != pstMsg )
        {

            UCOM_MemCpy(pstMsg,pstStubMsg,pstStubMsg->uwLength + VOS_MSG_HEAD_LENGTH);

            /* 发送消息,并且得到返回结果 */
            uwResult                        = VOS_SendMsg( pstMsg->uwSenderPid, pstMsg );

            /* 如果发送失败,报告错误 */
            if ( VOS_OK != uwResult )
            {

            }
            else
            {
                g_uwTestPos++;
            }
       }
   }
}
VOS_VOID VOS_InitTest()
{
    *((VOS_UINT32 *)(0x3f000000)) = 0;
    GetMsgFromFile();
    GetTestSeqFromFile();
    VOS_SmCCreate(NULL,0,0,&g_uwTestSem);
}




VOS_VOID VOS_VenusLogUint32(VOS_UINT32 uwValue)
{
    VOS_CPU_SR cr;

    cr = VOS_SplIMP();
    g_auwVosTestMessageBuf[2] = 2; //count of value
    g_auwVosTestMessageBuf[3] = read_ccount();
    g_auwVosTestMessageBuf[4] = uwValue;
    sim_call();
    VOS_Splx(cr);
}


VOS_VOID VOS_VenusLogInfo(VOS_UINT32 *puwValue, VOS_UINT32  uwDataLen)
{
    VOS_CPU_SR cr;
    VOS_UINT32 i;

    cr = VOS_SplIMP();

    g_auwVosTestMessageBuf[2] = uwDataLen + 1; //count of value,in VOS_UINT32
    g_auwVosTestMessageBuf[3] = read_ccount();
    for (i = 0; i < uwDataLen; i++)
    {
        g_auwVosTestMessageBuf[4 + i] = puwValue[i];
    }
    sim_call();

    VOS_Splx(cr);
}


#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

