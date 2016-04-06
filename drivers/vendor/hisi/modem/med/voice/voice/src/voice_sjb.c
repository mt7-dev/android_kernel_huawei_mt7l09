
#include "voice_sjb.h"
#include "om_log.h"
#include "voice_debug.h"

/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_SJB_C


/*****************************************************************************
   2 全局变量定义
******************************************************************************/
/* JitterBuffer上下行控制块及存储数组 */
VOICE_SJB_RX_MANAGER_STRU                g_stRxSjbManager;
VOICE_SJB_TX_MANAGER_STRU                g_stTxSjbManager;
VOS_VOID VOICE_SJB_InitTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager)
{
    VOICE_JB_PKT_TIME_ENUM_UINT32 enPktTime;

    enPktTime = pstSjbManager->enPktTime;

    /* JB控制块初始化 */
    UCOM_MemSet(pstSjbManager, 0, sizeof(VOICE_SJB_TX_MANAGER_STRU));

    /* 如果Init之前的PktTime是有效值，则保留。否则将其置为20ms */
    if((enPktTime < VOICE_JB_PKT_TIME_BUTT)
      && ((enPktTime % VOICE_JB_PKT_TIME_20MS) == 0) && (enPktTime > 0))
    {
        pstSjbManager->enPktTime = enPktTime;
    }
    else
    {
        pstSjbManager->enPktTime = VOICE_JB_PKT_TIME_20MS;
    }

    /* 初始化完成 */
    OM_LogInfo(VOICE_SJB_InitTxSucc);

    return;
}
VOS_UINT32 VOICE_SJB_SetPktTimeTx(
                VOICE_SJB_TX_MANAGER_STRU *pstSjbManager,
                VOICE_JB_PKT_TIME_ENUM_UINT32 enPktTime)
{
    /* 如果待设置的PktTime是有效的，则设置。否则保持不变并返回失败。 */
    if((enPktTime < VOICE_JB_PKT_TIME_BUTT) && ((enPktTime % VOICE_JB_PKT_TIME_20MS) == 0))
    {
        pstSjbManager->enPktTime = enPktTime;
        OM_LogInfo1(VOICE_SJB_SetPktTimeTxSucc, enPktTime);
        return UCOM_RET_SUCC;
    }
    else
    {
        OM_LogWarning1(VOICE_SJB_SetPktTimeTxFail, enPktTime);
        return UCOM_RET_FAIL;
    }
}
VOS_UINT32 VOICE_SJB_StorePacketTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt)
{
    VOICE_SJB_PKT_TX_STRU *pstPktTemp;
    VOS_UINT32 uwRet;

    /* 如果队列已满，删掉尾巴 */
    if(pstSjbManager->stPara.uwCurPktNum >= VOICE_SJB_TX_DEPTH)
    {
        VOICE_SJB_RemoveTailTx( pstSjbManager );
        pstSjbManager->stPara.uwLostByStore++;
    }

    /* 找到空位，写入该帧，因为上行不会有乱序，所以每次加入的帧都是队首 */
    pstPktTemp = VOICE_SJB_FindEmptyPtrTx( pstSjbManager );

    if(VOS_NULL == pstPktTemp)
    {
        pstSjbManager->stPara.uwLostByStore++;
        OM_LogError(VOICE_SJB_StorePacketTxFail);
        return UCOM_RET_FAIL;
    }
    UCOM_MemCpy(pstPktTemp, pstPkt, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

     /* 插入后当前帧就是首帧了 */
    uwRet = VOICE_SJB_InsertPktTx(pstSjbManager, pstPktTemp, VOS_NULL, pstSjbManager->stBuff.pstHead);

    return uwRet;
}
VOS_UINT32 VOICE_SJB_RemoveTailTx(VOICE_SJB_TX_MANAGER_STRU  *pstSjbManager)
{
    /* 尾巴不为空 */
    if(VOS_NULL != pstSjbManager->stBuff.pstTail)
    {
        pstSjbManager->stBuff.pstTail->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_EMPTY;
        pstSjbManager->stBuff.pstTail = pstSjbManager->stBuff.pstTail->pstPrev;

        /* 尾巴的上一帧不为空，则将其Next指针置为空 */
        if(VOS_NULL != pstSjbManager->stBuff.pstTail)
        {
            pstSjbManager->stBuff.pstTail->pstNext= VOS_NULL;
        }

        /* 尾巴的上一帧是空的，则此时链表的头也要置为空*/
        else
        {
            pstSjbManager->stBuff.pstHead = VOS_NULL;
        }
        pstSjbManager->stPara.uwCurPktNum--;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_SJB_InsertPktTx(
                VOICE_SJB_TX_MANAGER_STRU               *pstSjbManager,
                VOICE_SJB_PKT_TX_STRU                   *pstPktToStore,
                VOICE_SJB_PKT_TX_STRU                   *pstPktPrev,
                VOICE_SJB_PKT_TX_STRU                   *pstPktNext)
{
    pstPktToStore->pstNext      = pstPktNext;
    pstPktToStore->pstPrev      = pstPktPrev;
    pstPktToStore->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_NOTEMPTY;

    /* 插到队首 */
    if(VOS_NULL == pstPktPrev)
    {
        pstSjbManager->stBuff.pstHead = pstPktToStore;
    }
    else
    {
        pstPktPrev->pstNext = pstPktToStore;
    }

    /* 插到队尾 */
    if(VOS_NULL == pstPktNext)
    {
        pstSjbManager->stBuff.pstTail = pstPktToStore;
    }
    else
    {
        pstPktNext->pstPrev = pstPktToStore;
    }
    pstSjbManager->stPara.uwRcvPktNum++;
    pstSjbManager->stPara.uwCurPktNum++;

    /* 上报当前深度和总帧数 */
    OM_LogInfo2(VOICE_SJB_InsertPktTxSucc,
                pstSjbManager->stPara.uwCurPktNum,
                pstSjbManager->stPara.uwRcvPktNum);

    return UCOM_RET_SUCC;
}

VOICE_SJB_PKT_TX_STRU* VOICE_SJB_FindEmptyPtrTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager)
{
    VOS_UINT32 uhwCycle;

    VOICE_SJB_PKT_TX_STRU* pstPktTemp = &pstSjbManager->stBuff.stPkts[0];

    for(uhwCycle = 0; uhwCycle < VOICE_SJB_TX_DEPTH; uhwCycle++)
    {
        if(VOICE_SJB_ITEM_STATUS_EMPTY == pstPktTemp->enIsNotEmpty)
        {
            return pstPktTemp;
        }
        else
        {
            pstPktTemp++;
        }
    }

    /* 如果数组中所有位置全都不空，则返回空指针 */
    return VOS_NULL;
}
VOS_UINT32 VOICE_SJB_GetPacketTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32 uwRet;

    /* 数组里是空的，取不到包，但要更新TS */
    if (0 == pstSjbManager->stPara.uwCurPktNum)
    {
        OM_LogWarning(VOICE_SJB_GetPacketTxFail);
        return UCOM_RET_FAIL;
    }
    /* 数组不空 */
    else
    {
        /* 取数据 */
        UCOM_MemCpy(pstPkt, pstSjbManager->stBuff.pstTail, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

        /* 删掉尾巴 */
        uwRet = VOICE_SJB_RemoveTailTx(pstSjbManager);
        return uwRet;
    }
}


VOS_VOID VOICE_SJB_InitRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    /* JB控制块初始化 */
    UCOM_MemSet(pstSjbManager, 0, sizeof(VOICE_SJB_RX_MANAGER_STRU));

    /* 20ms对应的TS设置为320 */
    pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_WB;

    /* 初始化完成 */
    OM_LogInfo(VOICE_SJB_InitRxSucc);

    return;
}
VOS_UINT32 VOICE_SJB_StorePacketRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32 uwCurrentTS = pstPkt->ulTS;
    VOS_UINT32 uwCycle;
    VOS_UINT32 uwRet;
    VOS_UINT32 uwCpuSr;
    VOICE_SJB_PKT_RX_STRU *pstPktTemp;
    VOICE_SJB_PKT_RX_STRU *pstPktItr;

    /* 锁中断互斥 */
    uwCpuSr = VOS_SplIMP();

    /* 时戳小于LastTS，说明该包太旧，丢掉。此处需要考虑时戳翻转 */
    if(VOICE_SJB_TSCompare(uwCurrentTS, pstSjbManager->stPara.uwLastTS))
    {
        pstSjbManager->stPara.uwLostByStore++;
        pstSjbManager->stPara.uwTSErrCnt++;
        OM_LogWarning2(VOICE_SJB_StorePacketRx_TsTooOld, uwCurrentTS, pstSjbManager->stPara.uwLastTS);

        /*  因时间戳过旧连续丢包过多，就认为是异常，初始化JB  */
        if(pstSjbManager->stPara.uwTSErrCnt > VOICE_SJB_TSErrCntMax)
        {
            VOICE_SJB_InitRx(pstSjbManager);
            OM_LogWarning(VOICE_SJB_StorePacketRx_InitJB);
        }

        /* 释放互斥 */
        VOS_Splx(uwCpuSr);
        return UCOM_RET_FAIL;
    }
    else
    {
        /* 丢包不连续，将连续丢包数置0 */
        pstSjbManager->stPara.uwTSErrCnt = 0;
    }

    /* 如果队列已满，丢掉一包 */
    if(pstSjbManager->stPara.uwCurPktNum >= VOICE_SJB_RX_DEPTH)
    {
        /* 队尾更老，丢掉队尾 */
        if(VOICE_SJB_TSCompare(pstSjbManager->stBuff.pstTail->stPktData.ulTS, uwCurrentTS))
        {
            VOICE_SJB_RemoveTailRx(pstSjbManager);
            pstSjbManager->stPara.uwLostByStore++;
        }
        else
        {
            /* 该包更老，丢掉返回 */
            pstSjbManager->stPara.uwLostByStore++;
            OM_LogInfo1(VOICE_SJB_StorePacketRx_TsTooOld, pstSjbManager->stPara.uwLostByStore);

            /* 释放互斥 */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }
    }

    /* 从数组中找到一个空位，然后将该帧写入 */
    pstPktTemp = VOICE_SJB_FindEmptyPtrRx(pstSjbManager);
    if(VOS_NULL == pstPktTemp)
    {
        pstSjbManager->stPara.uwLostByStore++;
        OM_LogInfo1(VOICE_SJB_StorePacketRx_NoSpace, pstSjbManager->stPara.uwLostByStore);

        /* 如果链表维护正常，不会走进该分支，进了该分支，说明JB链表已经乱了，重新初始化 */
        VOICE_SJB_InitRx(pstSjbManager);

        /* 释放互斥 */
        VOS_Splx(uwCpuSr);
        return UCOM_RET_FAIL;
    }
    UCOM_MemCpy(pstPktTemp, pstPkt, sizeof( IMSA_VOICE_RX_DATA_IND_STRU));

    /* 更新该帧前后的prev和next指针，更新总帧数 */
    pstPktItr = pstSjbManager->stBuff.pstHead;
    for(uwCycle = 0; uwCycle < pstSjbManager->stPara.uwCurPktNum; uwCycle++)
    {
        /* 如果时标重复，说明是重复的包，要丢掉 */
        if(pstPktItr->stPktData.ulTS == uwCurrentTS)
        {
            pstSjbManager->stPara.uwLostByStore++;
            OM_LogInfo1(VOICE_SJB_StorePacketRx_SamePkt, pstSjbManager->stPara.uwLostByStore);

            /* 释放互斥 */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }

        /* 找到第一个TS比当前帧小的，便是当前帧的Next */
        if(VOICE_SJB_TSCompare(pstPktItr->stPktData.ulTS, uwCurrentTS))
        {
             uwRet = VOICE_SJB_InsertPktRx(pstSjbManager, pstPktTemp,  pstPktItr->pstPrev, pstPktItr);

             /* 释放互斥 */
             VOS_Splx(uwCpuSr);
             return uwRet;
        }
        pstPktItr = pstPktItr->pstNext;
    }

    /* 数组中现有包都比现在的要大，则要插到队尾后面。如果当前总帧数为0，则pstTail应为NULL */
    uwRet = VOICE_SJB_InsertPktRx(pstSjbManager, pstPktTemp, pstSjbManager->stBuff.pstTail, VOS_NULL);

    /* 释放互斥 */
    VOS_Splx(uwCpuSr);
    return uwRet;

}
VOS_UINT32 VOICE_SJB_RemoveTailRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    /* 数组里面还有尾巴 */
    if(VOS_NULL != pstSjbManager->stBuff.pstTail)
    {
        pstSjbManager->stBuff.pstTail->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_EMPTY;
        pstSjbManager->stBuff.pstTail = pstSjbManager->stBuff.pstTail->pstPrev;

        /* 尾巴的上一帧不为空,则将其上一帧的Next指针置空 */
        if(VOS_NULL != pstSjbManager->stBuff.pstTail)
        {
            pstSjbManager->stBuff.pstTail->pstNext= VOS_NULL;
        }

        /* 尾巴的上一帧是空的，则将链表的头也置空 */
        else
        {
            pstSjbManager->stBuff.pstHead = VOS_NULL;
        }
        pstSjbManager->stPara.uwCurPktNum--;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_SJB_InsertPktRx(
                VOICE_SJB_RX_MANAGER_STRU                   *pstSjbManager,
                VOICE_SJB_PKT_RX_STRU                       *pstPktToStore,
                VOICE_SJB_PKT_RX_STRU                       *pstPktPrev,
                VOICE_SJB_PKT_RX_STRU                       *pstPktNext)
{
    pstPktToStore->pstNext      = pstPktNext;
    pstPktToStore->pstPrev      = pstPktPrev;
    pstPktToStore->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_NOTEMPTY;

    /* 插到队首 */
    if(VOS_NULL == pstPktPrev)
    {
        pstSjbManager->stBuff.pstHead = pstPktToStore;
    }
    else
    {
        pstPktPrev->pstNext = pstPktToStore;
    }

    /* 插到队尾 */
    if(VOS_NULL == pstPktNext)
    {
        pstSjbManager->stBuff.pstTail = pstPktToStore;
    }
    else
    {
        pstPktNext->pstPrev = pstPktToStore;
    }
    pstSjbManager->stPara.uwRcvPktNum++;
    pstSjbManager->stPara.uwCurPktNum++;

    /* 根据数据包的CodecType设置uwTSof20ms的值，WB和G711是320，其余是160 */
    if((pstPktToStore->stPktData.usCodecType == CODEC_AMRWB)
        || (pstPktToStore->stPktData.usCodecType == CODEC_G711))
    {
        pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_WB;
    }
    else
    {
        pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_NB;
    }

    /* 上报当前深度和总帧数 */
    OM_LogInfo2(VOICE_SJB_InsertPktRxSucc,
                pstSjbManager->stPara.uwCurPktNum,
                pstSjbManager->stPara.uwRcvPktNum);

    return UCOM_RET_SUCC;
}



VOICE_SJB_PKT_RX_STRU* VOICE_SJB_FindEmptyPtrRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    VOS_UINT32 uwCycle;
    VOICE_SJB_PKT_RX_STRU *pstPktTemp = &pstSjbManager->stBuff.stPkts[0];

    for(uwCycle = 0; uwCycle < VOICE_SJB_RX_DEPTH; uwCycle++)
    {
        if(VOICE_SJB_ITEM_STATUS_EMPTY == pstPktTemp->enIsNotEmpty)
        {
            return pstPktTemp;
        }
        else
        {
            pstPktTemp++;
        }
    }

    return VOS_NULL;
}



VOS_UINT32 VOICE_SJB_GetPacketRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32                   uwRet;
    VOS_UINT32                   uwTSDiff;
    VOS_UINT32                   uwCpuSr;
    VOICE_JB_HOOK_STRU           stJBHook;

    /* 锁中断互斥 */
    uwCpuSr = VOS_SplIMP();

    /* 初始化标志为false，先判断有没有完成 */
    if(VOICE_SJB_INIT_STATUS_NOT == pstSjbManager->stState.enAlreadyInit)
    {
        if(pstSjbManager->stPara.uwCurPktNum < VOICE_SJB_INIT_DEPTH)
        {
            OM_LogInfo(VOICE_SJB_GetPacketRx_UnInit);

            /* 释放互斥 */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }
        else
        {
            /* 将初始化完全标志置为true，将LastTS置为比尾帧TS少20ms，以保证第一帧成功取走 */
            pstSjbManager->stState.enAlreadyInit = VOICE_SJB_INIT_STATUS_DONE;
            pstSjbManager->stPara.uwLastTS       = pstSjbManager->stBuff.pstTail->stPktData.ulTS - pstSjbManager->stState.uwTSof20ms;
        }
    }

    /* 数组里是空的，取不到包,要更新LastTS */
    if(0 == pstSjbManager->stPara.uwCurPktNum)
    {
        pstSjbManager->stPara.uwLostByGet++;
        pstSjbManager->stPara.uwLastTS += pstSjbManager->stState.uwTSof20ms;
        OM_LogWarning1(VOICE_SJB_GetPacketRx_NoPkt, pstSjbManager->stPara.uwLostByGet);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        uwTSDiff = VOICE_SJB_TSCompare( pstSjbManager->stPara.uwLastTS, pstSjbManager->stBuff.pstTail->stPktData.ulTS );

        /* 如果尾帧时戳和LastTS相差大于40毫秒，则不是要取的帧，返回失败,更新LastTs */
        if(uwTSDiff > (2 * (pstSjbManager->stState.uwTSof20ms)))
        {
            pstSjbManager->stPara.uwLostByGet++;
            pstSjbManager->stPara.uwGetErrCnt++;
            pstSjbManager->stPara.uwLastTS += pstSjbManager->stState.uwTSof20ms;

            OM_LogInfo3(VOICE_SJB_GetPacketRx_NoWantedPkt,
                        pstSjbManager->stPara.uwLostByGet,
                        pstSjbManager->stPara.uwLastTS,
                        pstSjbManager->stBuff.pstTail->stPktData.ulTS);

            /* 连续取包失败过多，就认为是异常，初始化JB */
            if(pstSjbManager->stPara.uwGetErrCnt > VOICE_SJB_TSErrCntMax)
            {
                VOICE_SJB_InitRx(pstSjbManager);
                OM_LogWarning(VOICE_SJB_GetPacketRx_InitJB);

                /* 释放互斥 */
                VOS_Splx(uwCpuSr);
                return UCOM_RET_FAIL;
            }

            uwRet = UCOM_RET_FAIL;
        }
        /* 成功取到尾帧，更新LastTS */
        else
        {
            pstSjbManager->stPara.uwGetErrCnt = 0;
            UCOM_MemCpy(pstPkt, pstSjbManager->stBuff.pstTail, sizeof(IMSA_VOICE_RX_DATA_IND_STRU));
            pstSjbManager->stPara.uwLastTS = pstSjbManager->stBuff.pstTail->stPktData.ulTS;
            uwRet = VOICE_SJB_RemoveTailRx(pstSjbManager);
        }
    }

    /*填写上报的JB参数*/
    UCOM_MemSet(&stJBHook.stData, 0, sizeof(HME_JB_HIFI_STATICS));
    stJBHook.stData.uiBufLen       = pstSjbManager->stPara.uwCurPktNum * VOICE_JB_PKT_TIME_20MS;
    stJBHook.stData.uiBufMaxSize   = VOICE_SJB_RX_DEPTH;
    stJBHook.stData.uiSeq          = pstPkt->usSN;
    stJBHook.stData.uiTimeStamp    = pstPkt->ulTS;
    stJBHook.stData.uiSSRC         = pstPkt->ulSSRC;
    stJBHook.stData.uiPacketRecv   = pstSjbManager->stPara.uwRcvPktNum;
    stJBHook.stData.uiPacketLoss   = pstSjbManager->stPara.uwLostByGet;
    stJBHook.stData.uiJBPacketLoss = pstSjbManager->stPara.uwLostByStore;

    /* 将JB的参数钩取上报给OM */
    VOICE_DbgSendHook(VOICE_HOOK_JB_PARA,
                      &stJBHook,
                      sizeof(HME_JB_HIFI_STATICS));

    /* 释放互斥 */
    VOS_Splx(uwCpuSr);
    return uwRet;
}


VOS_UINT32 VOICE_SJB_GetPktNumTx(VOICE_SJB_TX_MANAGER_STRU* pstSjbManager)
{
    return pstSjbManager->stPara.uwRcvPktNum;
}


VOS_UINT32 VOICE_SJB_TSCompare(VOS_UINT32 uwSrc, VOS_UINT32 uwDest)
{
    VOS_UINT32 uwRet;

    if(uwDest > uwSrc)
    {
        uwRet = uwDest - uwSrc;
        return uwRet;
    }
    else
    {
        /* 此处是时标翻转的情况 */
        if((uwDest + (VOICE_SJB_UINT32_MAX - uwSrc)) < VOICE_SJB_MaxTSDiff)
        {
            uwRet = uwDest + (VOICE_SJB_UINT32_MAX - uwSrc) + 1;
            return uwRet;
        }
        else
        {
            return 0;
        }
    }
}

