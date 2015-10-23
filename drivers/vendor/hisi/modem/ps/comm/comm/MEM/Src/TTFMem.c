

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "stddef.h"
#include "om.h"
#include "TTFMem.h"
#include "TTFComm.h"
#include "pslog.h"
#include "TTFMemRB.h"
#include "NVIM_Interface.h"
#include "TtfErrlog.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif



/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
extern  VOS_VOID CDS_FreeIpfDlSrcMem(VOS_VOID);

/******************************************************************************
   3 私有定义
******************************************************************************/
/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID                    PS_FILE_ID_TTF_MEM_C
/*lint +e767*/

#define    TTF_MEM_ACPU_FREE_MEM_CNT_ADDR  (ECS_TTF_ACPU_FREE_MEM_CNT_ADDR)    /* 存放C核查询A核内存的预留数目的地址 */

/*ACPU上没有RLC PID*/
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#define    WUEPS_PID_RLC                   (112)
#endif

/******************************************************************************
   4 全局变量定义
******************************************************************************/
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
VOS_UINT32                                 g_ulTtfBlkMemCheck = 1;             /* 控制是否跟踪TTF BLK MEM */
#endif
VOS_UINT32                                 g_ulTtfBlkMemAlertEvtCnt    =0;
VOS_UINT32                                 g_ulTtfBlkMemAlocFailEvtCnt =0;


/* 零拷贝内存池初始化标志，防止重复调用初始化函数 */
VOS_UINT32                              g_ulTtfMemInitFlag      = VOS_FALSE;

/* 内存池管理控制全局变量 */
TTF_MEM_POOL_STRU                       g_astTtfMemPool[TTF_MEM_POOL_ID_BUTT];


/* 内存申请失败指针数据 */
unsigned short *const                   g_apusTtfMemAllocFailCnt[TTF_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_SHARE].usTtfMemAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_PDU].usTtfMemAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_DATA].usTtfMemAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_EXT].usTtfMemAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_IP_DATA].usTtfMemAllocFailCnt
                                        };

/* 内存连续申请失败指针数据 */
unsigned short *const                   g_apusTtfMemConAllocFailCnt[TTF_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_SHARE].usTtfMemConAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_PDU].usTtfMemConAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_DATA].usTtfMemConAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_EXT].usTtfMemConAllocFailCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_IP_DATA].usTtfMemConAllocFailCnt
                                        };

/* 内存申请失败超过门限次数 */
unsigned short *const                   g_apusTtfMemExcThresholdCnt[TTF_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_SHARE].usTtfMemExcThresholdCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_DL_PDU].usTtfMemExcThresholdCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_DATA].usTtfMemExcThresholdCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_EXT].usTtfMemExcThresholdCnt,
                                            &g_astTtfMemPool[TTF_MEM_POOL_ID_UL_IP_DATA].usTtfMemExcThresholdCnt
                                        };

/* 获取内存池管理控制结构宏，仅TTF_MEM内部使用 */
#define TTF_MEM_GET_POOL(ulPoolId)      (&g_astTtfMemPool[(ulPoolId)])

/* 保存内存释放空指针信息 */
#define TTF_MEM_FREE_SAVE_NULL_PTR_INFO(stNullPtrInfo,usFileId,usLineNum)   /*lint -e717*/\
            {\
                (stNullPtrInfo).ulTtfMemFreeNullPtrCnt++;\
                (stNullPtrInfo).usTtfMemFreeFileId  = (usFileId);\
                (stNullPtrInfo).usTtfMemFreeLineNum = (usLineNum);\
            }/*lint +e717*/

/* 保存内存释放错误内存信息 */
#define TTF_MEM_FREE_SAVE_INVALID_MEM_INFO(astTtfMemFreeInvalidMemInfo,pstTtfMem,ucType,usTypeValue,usFileId,usLineNum)  /*lint -e717*/\
            {\
                (astTtfMemFreeInvalidMemInfo).ulInvalidTtfMemCnt++;\
                (astTtfMemFreeInvalidMemInfo).ulInvalidTtfMemAddr   = (VOS_UINT32)pstTtfMem;\
                (astTtfMemFreeInvalidMemInfo).usTtfMemFreeFileId    = (usFileId);\
                (astTtfMemFreeInvalidMemInfo).usTtfMemFreeLineNum   = (usLineNum);\
                (astTtfMemFreeInvalidMemInfo).ucInvalidTtfMemType   = (ucType);\
                (astTtfMemFreeInvalidMemInfo).usInvalidTtfMemValue  = (usTypeValue);\
            } /*lint +e717*/


/* 保留MAC头预留长度 */
const VOS_UINT16 TTF_MEM_MAC_HEADER_RSV_LEN = 14;

/* 保留SKBUFF头预留长度 */
const VOS_UINT16 TTF_MEM_SKB_HEAD_PAD_LEN   = 64;

/* 保留SKBUFF尾部预留长度 */
const VOS_UINT16 TTF_MEM_SKB_TAIL_PAD_LEN   = 288;

/* 每次发生内存上报事件时，每次所能内存事件次数门限 */
VOS_UINT32                              g_ulTtfMemEventRptCntThresholdPerTime = 1;

/* 每次发生内存上报事件时，每次所能内存事件次数门限 */
VOS_UINT16                              g_usTtfMemAllocFailCntThreshold = 0x3FF;

/* 内存池连续申请内存失败复位门限 */
VOS_UINT16                              g_usTtfMemResetThreshold = 20;

/* 开启内存详细信息打印开关 */
VOS_UINT8                               g_ucTtfMemPrintDetail      = PS_TRUE;

/* TTF MEM FREE 可维可测实体 */
TTF_MEM_FREE_MNTN_INFO_STRU             g_stTtfMemFreeMntnEntity;

TTF_MEM_SOLUTION_CFG_NV_STRU            g_stTtfMemSolution;

TTF_NODE_MEM_POOL_STRU                  g_stTtfNodeMemPool = {0};
TTF_NODE_MEM_NODE_STRU                  g_stNodeMemSpace[TTF_NODE_MEM_POOL_CNT];

/* 内存泄露保护实体 */
TTF_MEM_RCOV_ENTITY_STRU                g_stTtfMemRcovEnt;

/******************************************************************************
   5 函数实现
******************************************************************************/


VOS_VOID TTF_NodeMemAbnmlEventRpt
(
    VOS_UINT16   usPid,
    VOS_UINT16   usFileId,
    VOS_UINT16   usLineNo,
    TTF_NODE_MEM_NODE_STRU  *pstNodeMem,
    TTF_NODE_MEM_TRIG_TYPE_ENUM_UINT8 enTrigType
)
{
    VOS_UINT8                          aucData[offsetof(PS_OM_EVENT_IND_STRU, aucData) + sizeof(TTF_OM_EVENT_NODE_MEM_ABNML_STRU)] = {0};
    PS_OM_EVENT_IND_STRU              *pstEventInd;
    TTF_OM_EVENT_NODE_MEM_ABNML_STRU  *pstNodeMemEvent;

    pstEventInd     = (PS_OM_EVENT_IND_STRU *)aucData;
    pstNodeMemEvent = (TTF_OM_EVENT_NODE_MEM_ABNML_STRU *)pstEventInd->aucData;

    pstEventInd->ulLength   = sizeof(aucData) - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
    pstEventInd->usEventId  = TTF_EVENT_NODE_MEM_ABNML_INFO;
    pstEventInd->ulModuleId = WUEPS_PID_RLC;

    pstNodeMemEvent->usPid          = usPid;
    pstNodeMemEvent->usCurFileId    = usFileId;
    pstNodeMemEvent->usCurLineNum   = usLineNo;
    pstNodeMemEvent->ucTrigType     = enTrigType;
    pstNodeMemEvent->pstNodeMemAddr = pstNodeMem;

    /* 地址不对的有可能是非法地址,不做拷贝 */
    if ((VOS_NULL_PTR != pstNodeMem) && (TTF_NODE_MEM_CHK_ADDR_FAIL != enTrigType))
    {
        PS_MEM_CPY(&(pstNodeMemEvent->stNodeMem), pstNodeMem, sizeof(TTF_NODE_MEM_NODE_STRU));
    }

    /*调用OM的事件上报接口*/
    if ( PS_SUCC != OM_Event(pstEventInd))
    {
        PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_NodeMemAbnmlEventRpt: OM Send Event Fail!");
    }

    return;
}


VOS_VOID TTF_NodeMemPoolInit(VOS_VOID)
{
    VOS_INT i = 0;

    /* 如果已经初始化就不再初始化 */
    if (TTF_NODE_MEM_POOL_STATE_ACTIVE == g_stTtfNodeMemPool.ucState)
    {
        return;
    }
    g_stTtfNodeMemPool.ucState      = TTF_NODE_MEM_POOL_STATE_ACTIVE;
    g_stTtfNodeMemPool.usFreeCnt    = TTF_NODE_MEM_POOL_CNT;
    g_stTtfNodeMemPool.usTotalCnt   = g_stTtfNodeMemPool.usFreeCnt;
    g_stTtfNodeMemPool.pBaseAddr    = TTF_NODE_MEM_POOL_BASE_ADDR;
    g_stTtfNodeMemPool.ulTotalSize  = TTF_NODE_MEM_POOL_CNT * sizeof(TTF_NODE_MEM_NODE_STRU);

    for (; i < g_stTtfNodeMemPool.usTotalCnt; i++)
    {
        g_stTtfNodeMemPool.pFreeNode[i] = &g_stNodeMemSpace[i];
#if(TTF_NODE_MEM_DBG == FEATURE_ON)
        g_stNodeMemSpace[i].ucState      = TTF_NODE_MEM_IDLE;
        g_stNodeMemSpace[i].ucMagicNo    = 0x5A;
        g_stNodeMemSpace[i].usFileId     = 0;
        g_stNodeMemSpace[i].usLineNo     = 0;
        g_stNodeMemSpace[i].usFreeFileId = 0;
        g_stNodeMemSpace[i].usFreeLineNo = 0;
#endif
    }
}
VOS_UINT8 *TTF_NodeMemAlloc_Debug
(
    VOS_UINT32  ulPid,
    VOS_UINT16  usLen,
    VOS_UINT16  usFileId,
    VOS_UINT16  usLineNo
)
{
    VOS_INT32                lLockKey;
    TTF_NODE_MEM_NODE_STRU  *pMem = VOS_NULL_PTR;

    if (usLen > TTF_NODE_MEM_POOL_SIZE)
    {
        TTF_LOG3(ulPid, PS_PRINT_ERROR,
            "TTF_NodeMemAlloc_Debug, too long uslen %d, usFileId %d, usLineNo %d! \n",
            (VOS_INT32)usLen, (VOS_INT32)usFileId, (VOS_INT32)usLineNo);

        return VOS_NULL_PTR;
    }

    lLockKey = VOS_SplIMP();
    if (g_stTtfNodeMemPool.usFreeCnt > 0)
    {
        g_stTtfNodeMemPool.usFreeCnt--;
        pMem = (TTF_NODE_MEM_NODE_STRU *)g_stTtfNodeMemPool.pFreeNode[g_stTtfNodeMemPool.usFreeCnt];
    }
    VOS_Splx(lLockKey);

    if (VOS_NULL_PTR != pMem)
    {
#if(TTF_NODE_MEM_DBG == FEATURE_ON)
        if (TTF_NODE_MEM_IDLE != pMem->ucState)
        {
            TTF_NodeMemAbnmlEventRpt((VOS_UINT16)ulPid, usFileId, usLineNo, pMem, TTF_NODE_MEM_DUP_ALLOC);

            vos_printf("TTF_NodeMemAlloc_Debug, dup alloc found usFreeFileId %d usFreeLinNo %d usCurFileId %d, usCurLineNo %d usAllocFileId %d, usAllocLineNo %d pMem 0x%x\n",
                (VOS_INT32)pMem->usFreeFileId,  (VOS_INT32)pMem->usFreeLineNo,
                (VOS_INT32)usFileId, (VOS_INT32)usLineNo,
                (VOS_INT32)pMem->usFileId,  (VOS_INT32)pMem->usLineNo, pMem);

            return VOS_NULL_PTR;
        }

        pMem->usPid     = (VOS_UINT16)ulPid;
        pMem->usFileId  = usFileId;
        pMem->usLineNo  = usLineNo;
        pMem->ucState   = TTF_NODE_MEM_USED;
#endif
        return pMem->aucData;
    }


    TTF_LOG3(ulPid, PS_PRINT_INFO,
        "TTF_NodeMemAlloc_Debug, Alloc Mem Fail, No free mem, usLen %d, usFileId %d, usLineNo %d! \n",
        (VOS_INT32)usLen, (VOS_INT32)usFileId, (VOS_INT32)usLineNo);

    return VOS_NULL_PTR;
}


VOS_UINT32 TTF_NodeMemCheck(VOS_VOID *pMem)
{
    if ( ((VOS_UINT32)pMem < (VOS_UINT32)g_stTtfNodeMemPool.pBaseAddr)
        || ((VOS_UINT32)pMem > (VOS_UINT32)((VOS_UINT8 *)g_stTtfNodeMemPool.pBaseAddr + g_stTtfNodeMemPool.ulTotalSize)))
    {
        return VOS_NO;
    }

    return VOS_YES;
}


VOS_VOID TTF_NodeMemFree_Debug
(
    VOS_UINT32   ulPid,
    VOS_UINT8   *pMem,
    VOS_UINT16   usFileId,
    VOS_UINT16   usLineNo
)
{
    VOS_INT32   lLockKey;
    TTF_NODE_MEM_NODE_STRU *pstTtfNodeMem;

    if (VOS_NULL_PTR == pMem)
    {
        TTF_LOG2(ulPid, PS_PRINT_ERROR,
            "TTF_NodeMemFree_Debug, pMem is NULL, usFileId %d, usLineNo %d \n ",
            (VOS_INT32)usFileId, (VOS_INT32)usLineNo);

        return;
    }

    pMem         -= TTF_NODE_MEM_RSV_LEN;
    pstTtfNodeMem = (TTF_NODE_MEM_NODE_STRU *)pMem;

    if (VOS_NO == TTF_NodeMemCheck(pstTtfNodeMem))
    {
        TTF_LOG4(ulPid, PS_PRINT_ERROR,
            "TTF_NodeMemFree_Debug, Not Ttf node mem free, pMem 0x%x, Node Mem 0x%x, usFileId %d, usLineNo %d \n",
            (VOS_INT32)pMem, (VOS_INT32)g_stTtfNodeMemPool.pBaseAddr, (VOS_INT32)usFileId, (VOS_INT32)usLineNo);

        TTF_NodeMemAbnmlEventRpt((VOS_UINT16)ulPid, usFileId, usLineNo, pstTtfNodeMem, TTF_NODE_MEM_CHK_ADDR_FAIL);

        return;
    }

#if(TTF_NODE_MEM_DBG == FEATURE_ON)
    if (TTF_NODE_MEM_USED != pstTtfNodeMem->ucState)
    {
        TTF_NodeMemAbnmlEventRpt((VOS_UINT16)ulPid, usFileId, usLineNo, pstTtfNodeMem, TTF_NODE_MEM_DUP_FREE);

        vos_printf("TTF_NodeMemFree_Debug, dup free found usMagicNo %d usFreeFileId %d usFreeLinNo %d usCurFileId %d, usCurLineNo %d, usAllocFileId %d, usAllocLineNo %d, pMem 0x%x\n",
            (VOS_INT32)pstTtfNodeMem->ucMagicNo, (VOS_INT32)pstTtfNodeMem->usFreeFileId,
            (VOS_INT32)pstTtfNodeMem->usFreeLineNo, (VOS_INT32)usFileId, (VOS_INT32)usLineNo,
            (VOS_INT32)pstTtfNodeMem->usFileId,  (VOS_INT32)pstTtfNodeMem->usLineNo, pstTtfNodeMem);

        return;
    }

    /* 检查内存是否有被踩 */
    if (0x5A != pstTtfNodeMem->ucMagicNo)
    {
        TTF_NodeMemAbnmlEventRpt((VOS_UINT16)ulPid, usFileId, usLineNo, pstTtfNodeMem, TTF_NODE_MEM_CHK_MEM_FAIL);

        vos_printf("TTF_NodeMemFree_Debug, MagicNo modified usMagicNo %d usFreeFileId %d usFreeLinNo %d usCurFileId %d, usCurLineNo %d, usAllocFileId %d, usAllocLineNo %d, pMem 0x%x\n",
            (VOS_INT32)pstTtfNodeMem->ucMagicNo, (VOS_INT32)pstTtfNodeMem->usFreeFileId,
            (VOS_INT32)pstTtfNodeMem->usFreeLineNo, (VOS_INT32)usFileId, (VOS_INT32)usLineNo,
            (VOS_INT32)pstTtfNodeMem->usFileId,  (VOS_INT32)pstTtfNodeMem->usLineNo, pstTtfNodeMem);
    }

    pstTtfNodeMem->usPid            = (VOS_UINT16)ulPid;
    pstTtfNodeMem->usFreeFileId     = usFileId;
    pstTtfNodeMem->usFreeLineNo     = usLineNo;
    pstTtfNodeMem->ucState          = TTF_NODE_MEM_IDLE;
#endif

    lLockKey = VOS_SplIMP();
    if (g_stTtfNodeMemPool.usFreeCnt >= g_stTtfNodeMemPool.usTotalCnt)
    {
        VOS_Splx(lLockKey);
        return;
    }

    g_stTtfNodeMemPool.pFreeNode[g_stTtfNodeMemPool.usFreeCnt] = (TTF_NODE_MEM_NODE_STRU *)pMem;
    g_stTtfNodeMemPool.usFreeCnt++;
    VOS_Splx(lLockKey);
}
VOS_VOID  TTF_NodeMemShow(VOS_UINT32 ulStartIndx, VOS_UINT32 ulEndIndx)
{
    VOS_UINT32 i = 0;

    vos_printf("Node mem base addr 0x%x\n", g_stTtfNodeMemPool.pBaseAddr);
    vos_printf("Current free cnt %d \n", g_stTtfNodeMemPool.usFreeCnt);
    vos_printf("Current total cnt %d \n", g_stTtfNodeMemPool.usTotalCnt);

    vos_printf("ALL Mem:\n\n\n");
    for(i = ulStartIndx; i < ulEndIndx; i++)
    {
        vos_printf("g_stTtfNodeMemPool.pFreeNode[%d] = 0x%x ", i, g_stTtfNodeMemPool.pFreeNode[i]);

#if(TTF_NODE_MEM_DBG == FEATURE_ON)
        vos_printf("ucMagicNo = 0x%x, usPid = %d, usFileId = %d, usLineNo = %d, ucState = %d usFreeFileId = %d, usFreeLineNo = %d\n",
        g_stTtfNodeMemPool.pFreeNode[i]->ucMagicNo, g_stTtfNodeMemPool.pFreeNode[i]->usPid, g_stTtfNodeMemPool.pFreeNode[i]->usFileId,
        g_stTtfNodeMemPool.pFreeNode[i]->usLineNo, g_stTtfNodeMemPool.pFreeNode[i]->ucState,
        g_stTtfNodeMemPool.pFreeNode[i]->usFreeFileId, g_stTtfNodeMemPool.pFreeNode[i]->usFreeLineNo);
#endif
    }

    vos_printf("\r\n");
}


VOS_UINT32  TTF_NodeMemLeakCheck(VOS_VOID)
{
    /* 排除G使用的19块静态内存 */
    #if (FEATURE_OFF == FEATURE_MULTI_MODEM)
    if ((g_stTtfNodeMemPool.usFreeCnt + TTF_NODE_MEM_G_STATIC_CNT) != g_stTtfNodeMemPool.usTotalCnt)
    #else
    if ((g_stTtfNodeMemPool.usFreeCnt + (TTF_NODE_MEM_G_STATIC_CNT * 2)) != g_stTtfNodeMemPool.usTotalCnt)
    #endif
    {
        return PS_TRUE;
    }

    return PS_FALSE;
}

VOS_UINT32 TTF_GetCipherMemSection(VOS_UINT32 *pulBaseAddr)
{
    BSP_DDR_SECT_QUERY   stQuery;
    BSP_DDR_SECT_INFO    stInfo;
    VOS_UINT32           ulBaseAddr;

    stQuery.enSectType = BSP_DDR_SECT_TYPE_TTF;
    DRV_GET_FIX_DDR_ADDR(&stQuery, &stInfo);

    ulBaseAddr = stInfo.ulSectPhysAddr;

    *pulBaseAddr = TTF_UL_CIPHER_PARA_NODE_ADDR(ulBaseAddr);

    return PS_SUCC;
}


VOS_UINT32 TTF_GetTTFMemSection(VOS_UINT32 *pulBaseAddr, VOS_UINT32 *pulLength)
{
    BSP_DDR_SECT_QUERY             stQuery;
    BSP_DDR_SECT_INFO              stInfo;
    VOS_UINT32                     ulBaseAddr;

    stQuery.enSectType = BSP_DDR_SECT_TYPE_TTF;
    DRV_GET_FIX_DDR_ADDR(&stQuery, &stInfo);

    ulBaseAddr   = stInfo.ulSectPhysAddr;
    *pulBaseAddr = TTF_MEM_POOL_BASE_ADDR(ulBaseAddr);
    *pulLength   = (stInfo.ulSectSize - TTF_MEMCTRL_GetOffsetBaseLen(TTF_MEMCTRL_CCORE_POOL_ADDR_TYPE));

    return PS_SUCC;
}



VOS_UINT32 TTF_MemCheckReportMemInfoStatus
(
    VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId,
    BLK_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType
)
{
    VOS_INT32                           lLockKey;
    TTF_MEM_POOL_STRU                  *pstPoolInfo;

    /*
    下面三种情况上报事件
    1、检查出内存泄露
    2、第一次发生内存申请失败
    3、第一次发生内存不足告警
    */
    lLockKey = VOS_SplIMP();
    if ( TRIG_TYPE_LEAK == enTrigType )
    {

    }
    else if ( (TRIG_TYPE_ALLOC_FAIL == enTrigType) && (0 == g_ulTtfBlkMemAlocFailEvtCnt) )
    {
        g_ulTtfBlkMemAlocFailEvtCnt++;
    }
    else if ( (TRIG_TYPE_ALERT == enTrigType) && (0 == g_ulTtfBlkMemAlertEvtCnt) )
    {
        g_ulTtfBlkMemAlertEvtCnt++;
    }
    else
    {
        VOS_Splx(lLockKey);
        return PS_FALSE;
    }
    VOS_Splx(lLockKey);

    pstPoolInfo     = TTF_MEM_GET_POOL(ulPoolId);

    if (PS_TRUE != pstPoolInfo->usUsedFlag)
    {
        TTF_LOG1(ulPid, PS_PRINT_ERROR, "TTF_MemCheckReportMemInfoStatus: Pool %d not used!\n",  (VOS_INT32)ulPoolId);
        return PS_FALSE;
    }

    return PS_TRUE;
}



VOS_VOID TTF_MemTraceInfoRptSaveMemData
(
    VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId,
    BLK_MEM_TRACE_INFO_STRU *pstBlkMemTraceInfo,
    TTF_MEM_ST *pMem
)
{

    if ( ulPoolId != pMem->ucPoolId )
    {
        TTF_LOG2(ulPid, PS_PRINT_ERROR, "TTF_MemTraceInfoRptSaveMemData, ulPoolId %d, pMem->ucPoolId %d Mismatch! \n", (VOS_INT32)ulPoolId, pMem->ucPoolId);
        return;
    }

    if ( TTF_MEM_POOL_ID_EXT > ulPoolId )
    {
        if ( VOS_NULL_PTR != pMem->pData )
        {
            pstBlkMemTraceInfo->aucMemData[0]   = pMem->pData[0];
            pstBlkMemTraceInfo->aucMemData[1]   = pMem->pData[1];
            pstBlkMemTraceInfo->aucMemData[2]   = pMem->pData[2];
            pstBlkMemTraceInfo->aucMemData[3]   = pMem->pData[3];
        }
        else
        {
            VOS_MemSet(pstBlkMemTraceInfo->aucMemData, 0, sizeof(pstBlkMemTraceInfo->aucMemData));
            TTF_LOG1(ulPid, PS_PRINT_ERROR, "TTF_MemTraceInfoRptSaveMemData, ulPoolId %d Mem Ptr is Null! \n", (VOS_INT32)ulPoolId);
        }

        return;
    }

    if ( TTF_MEM_POOL_ID_EXT == ulPoolId )
    {
        *((VOS_UINT32 *)pstBlkMemTraceInfo->aucMemData) = (VOS_UINT32)pMem->pExtBuffAddr;
    }

    return;
}/* TTF_MemTraceInfoRptSaveMemData */

VOS_VOID TTF_BlkMemUsedInfoEventRpt(VOS_UINT32 ulPid,
    VOS_UINT32  ulPoolId, BLK_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType )
{
    PS_OM_EVENT_IND_STRU                   *pstTtfOmEventInd = VOS_NULL_PTR;
    VOS_UINT32                              ulLength;
    VOS_UINT32                              ulDataLen;
    VOS_UINT32                              ulTotalCnt = 0;
    TTF_MEM_POOL_STRU                      *pstPoolInfo;
    TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU    *pstTtfOmEventBlkMemUsedInfo;

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    TTF_MEM_ST                             *pMem;
    VOS_UINT32                              ulBlkMemNum;
    VOS_UINT16                              usRptCnt = 0;
    VOS_UINT32                              ulOffset = 0;
    VOS_UINT8                               ucRptNo = 0;
    VOS_UINT16                              usRptEventCnt = 0;
    BLK_MEM_TRACE_INFO_STRU                *pstBlkMemTraceInfo;
#endif

    /* 检查上报状态是否正确 */
    if ( PS_TRUE != TTF_MemCheckReportMemInfoStatus(ulPid, ulPoolId, enTrigType) )
    {
        return;
    }

    pstPoolInfo  = TTF_MEM_GET_POOL(ulPoolId);

    ulTotalCnt   = pstPoolInfo->usTtfMemUsedCnt;

    /*========================================*/
    /*没有打开内存调试编译开关，或者没有打开内存调试使用开关，仅上报TTF Blk MEM 泄露总块数 */
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    if ( !TTF_BLK_MEM_DEBUG_SWITCH_ON )
#endif
    {
        ulDataLen           = offsetof(TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo);

        /*消息总长度*/
        ulLength            = offsetof(PS_OM_EVENT_IND_STRU, aucData) + ulDataLen;

        /*申请消息内存*/
        pstTtfOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(ulPid, ulLength);

        if (VOS_NULL_PTR == pstTtfOmEventInd )
        {
            PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_ShowUsedBlkMemInfo: MemAlloc Fail!");
            return;
        }

        /*填写消息内容*/
        pstTtfOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
        pstTtfOmEventInd->usEventId     = TTF_EVENT_BLK_MEM_USED_INFO;
        pstTtfOmEventInd->ulModuleId    = WUEPS_PID_RLC;/* 由于Event目前只能对应一个固定的PID，在此只能先暂定为RLC PID */

        pstTtfOmEventBlkMemUsedInfo     = (TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstTtfOmEventInd->aucData;
        pstTtfOmEventBlkMemUsedInfo->ucPoolId       = (VOS_UINT8)ulPoolId;
        pstTtfOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
        pstTtfOmEventBlkMemUsedInfo->ucNo           = 1;
        pstTtfOmEventBlkMemUsedInfo->usRptCnt       = 0;
        pstTtfOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

        /*调用OM的事件上报接口*/
        if ( PS_SUCC != OM_Event((PS_OM_EVENT_IND_STRU*)pstTtfOmEventInd))
        {
            PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_ShowUsedBlkMemInfo: OM Send Event Fail!");
        }
        /*释放申请的内存*/
        PS_MEM_FREE(ulPid, pstTtfOmEventInd);
        return;
    }

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    /*========================================*/
    /* 打开了内存调试编译和使用开关，上报内存跟踪信息 */
    ulDataLen           = offsetof(TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo) + ( EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG * sizeof(BLK_MEM_TRACE_INFO_STRU));

    /*消息总长度*/
    ulLength            = offsetof(PS_OM_EVENT_IND_STRU,aucData) + ulDataLen;

    /*申请消息内存*/
    pstTtfOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(ulPid, ulLength);

    if ( VOS_NULL_PTR == pstTtfOmEventInd )
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_ShowUsedBlkMemInfo: MemAlloc Fail!");
        return;
    }

    /* 遍历内存块 */
    for (ulBlkMemNum = 0; ulBlkMemNum < pstPoolInfo->usTtfMemTotalCnt; ulBlkMemNum ++)
    {
        pMem = pstPoolInfo->pstTtfMemStStartAddr + ulBlkMemNum;

        if (VOS_NULL_PTR == pMem->pstDbgInfo)
        {
            PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Blk Mem debug info is null!");
            continue;
        }

        if (TTF_BLK_MEM_STATE_FREE != pMem->pstDbgInfo->enMemStateFlag)
        {
            ulOffset    =  offsetof(TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo)  + ( usRptCnt * sizeof(BLK_MEM_TRACE_INFO_STRU) );

            pstBlkMemTraceInfo  = (BLK_MEM_TRACE_INFO_STRU *)(pstTtfOmEventInd->aucData + ulOffset );
            pstBlkMemTraceInfo->ulAllocTick         = pMem->pstDbgInfo->ulAllocTick;
            pstBlkMemTraceInfo->usAllocFileId       = pMem->pstDbgInfo->usAllocFileId;
            pstBlkMemTraceInfo->usAllocLineNum      = pMem->pstDbgInfo->usAllocLineNum;

            pstBlkMemTraceInfo->ulTraceTick         = pMem->pstDbgInfo->ulTraceTick;
            pstBlkMemTraceInfo->usTraceFileId       = (pMem->pstDbgInfo->usTraceFileId);
            pstBlkMemTraceInfo->usTraceLineNum      = (pMem->pstDbgInfo->usTraceLineNum);

            TTF_MemTraceInfoRptSaveMemData(ulPid, ulPoolId, pstBlkMemTraceInfo, pMem);

            usRptCnt++;

            /* 分批发送，每笔最多发送一千块 */
            if (  EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG == usRptCnt )
            {
                ucRptNo++;
                usRptCnt = 0;
                /*填写消息内容*/

                pstTtfOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
                pstTtfOmEventInd->usEventId     = TTF_EVENT_BLK_MEM_USED_INFO;
                pstTtfOmEventInd->ulModuleId    = WUEPS_PID_RLC;

                pstTtfOmEventBlkMemUsedInfo     = (TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstTtfOmEventInd->aucData;
                pstTtfOmEventBlkMemUsedInfo->ucPoolId       = (VOS_UINT8)ulPoolId;
                pstTtfOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
                pstTtfOmEventBlkMemUsedInfo->ucNo           = ucRptNo;
                pstTtfOmEventBlkMemUsedInfo->usRptCnt       = EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG;
                pstTtfOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

                /*调用OM的事件上报接口*/
                if ( PS_SUCC != OM_Event((PS_OM_EVENT_IND_STRU*)pstTtfOmEventInd))
                {
                    PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_ShowUsedBlkMemInfo: OM Send Event Fail!");
                }

                ++usRptEventCnt;
            }

            if ( g_ulTtfMemEventRptCntThresholdPerTime < usRptEventCnt )
            {
                /* 释放申请的内存 */
                PS_MEM_FREE(ulPid, pstTtfOmEventInd);
                return;
            }
        }
    }

    /* 最后一笔数据块数不足 EVENT_BLK_MEM_USED_INFO_CNT_PER_MSG */
    if ( 0 != usRptCnt )
    {
        ucRptNo++;

        ulDataLen   = ( usRptCnt * sizeof(BLK_MEM_TRACE_INFO_STRU))  + offsetof(TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo) ;

        ulLength    = offsetof(PS_OM_EVENT_IND_STRU,aucData) + ulDataLen;

        /*填写消息内容*/
        pstTtfOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
        pstTtfOmEventInd->usEventId     = TTF_EVENT_BLK_MEM_USED_INFO;
        pstTtfOmEventInd->ulModuleId    = WUEPS_PID_RLC;

        pstTtfOmEventBlkMemUsedInfo     = (TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstTtfOmEventInd->aucData;
        pstTtfOmEventBlkMemUsedInfo->ucPoolId       = (VOS_UINT8)ulPoolId;
        pstTtfOmEventBlkMemUsedInfo->enTrigType     = enTrigType;
        pstTtfOmEventBlkMemUsedInfo->ucNo           = ucRptNo;
        pstTtfOmEventBlkMemUsedInfo->usRptCnt       = usRptCnt;
        pstTtfOmEventBlkMemUsedInfo->ulTotalUsedCnt = ulTotalCnt;

        /*调用OM的事件上报接口*/
        if ( PS_SUCC != OM_Event((PS_OM_EVENT_IND_STRU*)pstTtfOmEventInd))
        {
            PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_ShowUsedBlkMemInfo: OM Send Event Fail!");
        }
    }

    /*释放申请的内存*/
    PS_MEM_FREE(ulPid, pstTtfOmEventInd);

    return ;
#endif
}



TTF_MEM_ST *TTF_MemBlkAllocPlus_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
     VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, VOS_UINT16 usLen)
{
    TTF_MEM_POOL_STRU              *pstTtfMemPool;
    TTF_MEM_ST                     *pMem;
    TTF_MEM_ST                     *pTailMem;
    VOS_UINT16                      usRemLen;


    if ( ulPoolId >= TTF_MEM_POOL_ID_BUTT )
    {
        PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Poolid <1> is invalid!",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /*================================*/    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Pool<1> is not used!",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /*================================*/    /* 申请长度是否在该内存内 */
    if (0 == usLen )
    {
        PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,usLen exceed the ranger!",
            usLen);
        return VOS_NULL_PTR;
    }
    pMem = VOS_NULL_PTR;

    if (usLen <= pstTtfMemPool->usMaxByteLen)
    {
        pMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usLen);
        return pMem;
    }

    /*malloc more than one block and link the block together*/
    usRemLen = usLen;

    while (usRemLen > pstTtfMemPool->usMaxByteLen)
    {
        pTailMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, pstTtfMemPool->usMaxByteLen);
        if (VOS_NULL_PTR == pTailMem)
        {
            PS_LOG(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAllocPlus_Debug,alloc ERR !");
            TTF_MemFree_Debug(usFileID, usLineNum, ulPid, &pTailMem);
            if (VOS_NULL_PTR != pMem)
            {
                TTF_MemFree_Debug(usFileID, usLineNum, ulPid, &pMem);
            }
            return VOS_NULL_PTR;
        }

        TTF_MemLink(ulPid, &pMem, pTailMem);

        usRemLen    -= pstTtfMemPool->usMaxByteLen;

    }

    if (usRemLen > 0)
    {
        pTailMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usRemLen);
        if (VOS_NULL_PTR == pTailMem)
        {
            PS_LOG(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAllocPlus_Debug,alloc ERR !");
            TTF_MemFree_Debug(usFileID, usLineNum, ulPid, &pMem);
            return VOS_NULL_PTR;
        }

        TTF_MemLink(ulPid, &pMem, pTailMem);
    }

    return pMem;
} /* TTF_MemBlkAllocPlus_Debug */


#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)

VOS_UINT32 TTF_MemFreeSaveDebugInfo(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,TTF_MEM_ST *pstTtfMem)
{

    if ( NULL != pstTtfMem->pstDbgInfo )
    {
        /*判断内存指针状态是否正确*/
        if ( TTF_BLK_MEM_STATE_ALLOC != pstTtfMem->pstDbgInfo->enMemStateFlag )
        {
            TTF_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeInvalidMemInfo,pstTtfMem,TTF_INVALID_MEM_TYPE_STUTAS,(VOS_UINT16)pstTtfMem->pstDbgInfo->enMemStateFlag,usFileID,usLineNum);
            TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR, "Error: TTF_MemFreeSaveDebugInfo, Mem blk status is Free! FileID %d, LineNum %d \n",
                        usFileID, usLineNum);

            return PS_FAIL;
        }

        pstTtfMem->pstDbgInfo->enMemStateFlag       = TTF_BLK_MEM_STATE_FREE;
        pstTtfMem->pstDbgInfo->usAllocFileId        = usFileID;
        pstTtfMem->pstDbgInfo->usAllocLineNum       = usLineNum;
        pstTtfMem->pstDbgInfo->ulAllocTick          = OM_GetSlice();
    }
    else
    {
        TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR, "TTF_MemFreeSaveDebugInfo, Blk Mem debug info is null! FileID %d, LineNum %d \n",
                        usFileID, usLineNum);

    }

    return PS_SUCC;

}/* TTF_MemFreeCtrlHead_Debug */


VOS_VOID TTF_MemRcovMem(VOS_UINT32 ulPoolId, VOS_UINT32 ulPid, VOS_UINT16 usFileId, VOS_UINT16 usLine)
{
    TTF_MEM_ST                     *pstTempMem;
    VOS_UINT16                      usBlkMemNum = 0;
    VOS_UINT16                      usMemRptCnt = 0;
    TTF_BLK_MEM_DEBUG_INFO_STRU    *pstBlkMemDebugInfo;
    TTF_BLK_MEM_DEBUG_INFO_STRU    *pstTempMemDebugInfo;
    TTF_MEM_POOL_STRU              *pstTtfMemPool;

    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    pstBlkMemDebugInfo = (TTF_BLK_MEM_DEBUG_INFO_STRU *)PS_MEM_ALLOC(ulPid, TTF_MEM_DEBUG_NUM * sizeof(TTF_BLK_MEM_DEBUG_INFO_STRU));

    if (VOS_NULL_PTR == pstBlkMemDebugInfo)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "TTF_MemBlkAlloc_Debug: MemAlloc Fail!");

        return;
    }

    pstTempMemDebugInfo = pstBlkMemDebugInfo;

    for (usBlkMemNum = 0; usBlkMemNum < pstTtfMemPool->usTtfMemTotalCnt; usBlkMemNum ++)
    {
        pstTempMem = pstTtfMemPool->pstTtfMemStStartAddr + usBlkMemNum;

        if ((VOS_NULL_PTR != pstTempMem->pstDbgInfo)
            &&(TTF_BLK_MEM_STATE_FREE != pstTempMem->pstDbgInfo->enMemStateFlag))
        {
            pstTempMemDebugInfo->ulAllocTick       = pstTempMem->pstDbgInfo->ulAllocTick;
            pstTempMemDebugInfo->usAllocFileId     = pstTempMem->pstDbgInfo->usAllocFileId;
            pstTempMemDebugInfo->usAllocLineNum    = pstTempMem->pstDbgInfo->usAllocLineNum;
            pstTempMemDebugInfo->ulTraceTick       = pstTempMem->pstDbgInfo->ulTraceTick;
            pstTempMemDebugInfo->usTraceFileId     = pstTempMem->pstDbgInfo->usTraceFileId;
            pstTempMemDebugInfo->usTraceLineNum    = pstTempMem->pstDbgInfo->usTraceLineNum;

            usMemRptCnt++;
            pstTempMemDebugInfo = pstBlkMemDebugInfo + usMemRptCnt;
        }

        if (usMemRptCnt >= TTF_MEM_DEBUG_NUM)
        {
            break;
        }
    }

    DRV_SYSTEM_ERROR(TTF_MEM_ALLOC_FAIL_SYSTEM_ERROR, usFileId, usLine,
                    (VOS_CHAR *)pstBlkMemDebugInfo, TTF_MEM_DEBUG_NUM * sizeof(TTF_BLK_MEM_DEBUG_INFO_STRU));

}

#endif



VOS_VOID TTF_PrintBlkMemPoolUsedInfo( VOS_UINT8 ucPoolId )
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPoolInfo;
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    VOS_UINT16                          ulTtfMemNum;
    TTF_MEM_ST                         *pMem;
    VOS_UINT16                          usAmdPduSn;
#endif


    pstTtfMemPoolInfo = TTF_MEM_GET_POOL(ucPoolId);

    if (PS_TRUE != pstTtfMemPoolInfo->usUsedFlag)
    {
        vos_printf("Pool %d not uesed ! \n", ucPoolId);

        return;
    }

    vos_printf("Pool %d Mem Blk used Cnt = %d \n", ucPoolId, pstTtfMemPoolInfo->usTtfMemUsedCnt );

    if( PS_FALSE == g_ucTtfMemPrintDetail)
    {
        return;
    }

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    for (ulTtfMemNum = 0;
              ulTtfMemNum < pstTtfMemPoolInfo->usTtfMemTotalCnt;
              ulTtfMemNum ++)
    {
        pMem = pstTtfMemPoolInfo->pstTtfMemStStartAddr + ulTtfMemNum;

        if (VOS_NULL_PTR == pMem->pstDbgInfo)
        {
            vos_printf("Pool ID %u Blk Mem %u debug info is null", pMem->ucPoolId, ulTtfMemNum);
            continue;
        }

        if (TTF_BLK_MEM_STATE_FREE != pMem->pstDbgInfo->enMemStateFlag)
        {
            vos_printf("Alloc: File %4d L %5d T %11u, Blk Mem Level: %4u, Status: %d, TraceFile:%4d, TraceLine:%5d, TraceTick:%11d, \n ",
                (VOS_INT32)pMem->pstDbgInfo->usAllocFileId,
                (VOS_INT32)pMem->pstDbgInfo->usAllocLineNum,
                pMem->pstDbgInfo->ulAllocTick,
                ulTtfMemNum,
                pMem->pstDbgInfo->enMemStateFlag,
                (VOS_INT32)pMem->pstDbgInfo->usTraceFileId,
                (VOS_INT32)pMem->pstDbgInfo->usTraceLineNum,
                pMem->pstDbgInfo->ulTraceTick);

            if ( TTF_MEM_POOL_ID_EXT == ucPoolId )
            {
              /* 打印指针地址 */
              vos_printf("ExtBuffAddr ptr: \n");
              vos_printf("%d \n", (VOS_UINT32)pMem->pExtBuffAddr);
            }
            else
            {
                /* 解析PDU数据中RLC SN,  */
                if ( 0x80 == (pMem->pData[0]&0x80) )
                {
                    usAmdPduSn = ((VOS_UINT16)(pMem->pData[0] & 0x7f) << 5) | (VOS_UINT16)((pMem->pData[1] & 0xf8) >> 3);
                    vos_printf("Sn: %4d; \n", usAmdPduSn);
                }
                else
                {
                    vos_printf("Sn: null; \n");
                }
            }


        }

    }
#endif

}/* TTF_PrintBlkMemPoolUsedInfo */


/*****************************************************************************
 函 数 名  : TTF_ShowUsedBlkMemInfo
 功能描述  : 显示TTF_MEM统计信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年8月2日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID TTF_ShowUsedBlkMemInfo(VOS_VOID)
{
    VOS_UINT32                           ulPoolId;


    vos_printf("TTF_ShowUsedBlkMemInfo:Begin.\r\n");

    for (ulPoolId = 0; ulPoolId < TTF_MEM_POOL_ID_BUTT; ulPoolId++)
    {

        vos_printf("TTF_ShowUsedBlkMemInfo print pool %d info begin: \n", ulPoolId );
        TTF_PrintBlkMemPoolUsedInfo((VOS_UINT8)ulPoolId);
        vos_printf("TTF_ShowUsedBlkMemInfo print pool %d info end! \n", ulPoolId);
    }

    return ;
}



VOS_VOID TTF_MemPrintDetail(VOS_UINT8 ucChoice)
{
    g_ucTtfMemPrintDetail = ucChoice;

    return;
}


VOS_VOID TTF_ShowTtfUncacheMemInfo(VOS_VOID)
{


    return ;
}




TTF_MEM_ST *TTF_MemBlkSingleFree_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,TTF_MEM_ST *pstTtfMem)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;
    TTF_MEM_CLUSTER_STRU               *pstTtfMemCluster;
    VOS_INT32                           lLockKey;
    TTF_MEM_ST                         *pRtnMem;
    VOS_VOID                           *pExtMem = VOS_NULL_PTR;

    pRtnMem = pstTtfMem->pNext;

    if ( TTF_MEM_POOL_ID_BUTT <= pstTtfMem->ucPoolId )
    {
        TTF_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeInvalidMemInfo,pstTtfMem,TTF_INVALID_MEM_TYPE_POOLID,pstTtfMem->ucPoolId,usFileID,usLineNum);
        TTF_LOG3(WUEPS_PID_RLC, PS_PRINT_ERROR, "ERROR: TTF_MemBlkSingleFree_Debug, Poolid %d is invalid! FileID %d, LineNum %d \n",
            (VOS_INT32)pstTtfMem->ucPoolId, usFileID, usLineNum);
        return VOS_NULL_PTR;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(pstTtfMem->ucPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        TTF_LOG3(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemBlkSingleFree_Debug, Poolid %d is not used! FileID %d, LineNum %d \n",
            (VOS_INT32)pstTtfMem->ucPoolId, usFileID, usLineNum);
        return pRtnMem;
    }

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)

    if ( PS_SUCC != TTF_MemFreeSaveDebugInfo(usFileID, usLineNum, pstTtfMem) )
    {
        TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemBlkSingleFree_Debug, TTF_MemFreeSaveDebugInfo fail! FileID %d, LineNum %d \n",
                    usFileID, usLineNum);
        return pRtnMem;
    };

#endif

    pstTtfMemCluster = &(pstTtfMemPool->astClusterTable[pstTtfMem->ucClusterId]);

    if ( TTF_MEM_POOL_ID_EXT == pstTtfMem->ucPoolId )
    {
        pExtMem = pstTtfMem->pExtBuffAddr;
        pstTtfMem->pExtBuffAddr = VOS_NULL_PTR;
        /* 为了后面正确维护pstTtfMemPool->ulTtfMemUsedByteSize的值 */
        pstTtfMem->usLen        = 0;
        pstTtfMem->pData        = pstTtfMem->pOrigData;
    }

    lLockKey = VOS_SplIMP();

    if (pstTtfMemCluster->usFreeCnt >= pstTtfMemCluster->usTotalCnt)
    {
        VOS_Splx(lLockKey);
        return pRtnMem;
    }

    pstTtfMemCluster->apstFreeStack[pstTtfMemCluster->usFreeCnt] = pstTtfMem;
    pstTtfMemPool->usTtfMemUsedCnt--;
    pstTtfMemCluster->usFreeCnt++;

    pstTtfMemPool->ulTtfMemUsedByteSize -= (pstTtfMem->usLen + TTF_MEM_GET_HDR_RSV_LEN(pstTtfMem));
    VOS_Splx(lLockKey);

    if (( VOS_NULL_PTR != pstTtfMemPool->pMemExtFreeFunc )&&( VOS_NULL_PTR != pExtMem ))
    {
        /* 释放外部内存 */
        pstTtfMemPool->pMemExtFreeFunc(pExtMem);
    }

    if (VOS_NULL_PTR != pstTtfMemPool->pMemFreeEvent)
    {
        pstTtfMemPool->pMemFreeEvent(pstTtfMemPool->usTtfMemUsedCnt, pstTtfMemPool->ulTtfMemUsedByteSize);
    }

    return pRtnMem;
} /* TTF_MemBlkSingleFree_Debug */

TTF_MEM_ST *TTF_MemBlkCopyAllocFromMem_Debug
(
    VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId, TTF_MEM_ST *pMemSrc, VOS_UINT16 usOffset,
    VOS_UINT16 usLen
)
{
    TTF_MEM_ST *pCurrMem        = pMemSrc;
    VOS_UINT16  usCurrOffset    = usOffset;
    VOS_UINT16  usCopyLen       = 0;
    VOS_UINT16  usWorkedLen     = 0;
    VOS_UINT16  usMemSrcLen;
    TTF_MEM_ST *pMem            = VOS_NULL_PTR;


    /*====================*/ /* 参数检查 */
    if (VOS_NULL_PTR == pMemSrc)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par pMemSrc is Null!");
        return VOS_NULL_PTR;
    }

    if ( 0 == usLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par usLen is 0!");
        return VOS_NULL_PTR;
    }

    /*====================*/ /*判断TTF内存块的长度是否符合要求*/
    usMemSrcLen = TTF_MemGetLen(ulPid, pMemSrc);

    if ( usMemSrcLen < (usOffset + usLen) )
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning:MemSrcLen <1> Less Than (Offset + Len) <2>!",
            usMemSrcLen, (usOffset + usLen));
        return VOS_NULL_PTR;
    }

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usUsed <= usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pCurrMem        = pCurrMem->pNext;
        }
        else
        {
            break;
        }
    }

    /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
    if ( VOS_NULL_PTR == pCurrMem )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING,
            "Warning: TTF_MemBlkCopyAllocFromMem, pCurrMem is VOS_NULL_PTR!");
        return VOS_NULL_PTR;
    }


    /*====================*/ /* 分配一块长度为usLen的TTF内存块 */
    pMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usLen);

    if (VOS_NULL_PTR == pMem)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Error: MemCopy, Alloc Mem is Fail!");
        return VOS_NULL_PTR;
    }


    /*================*/ /* 第一块要进行偏移，特殊处理 */
    usCopyLen   = PS_MIN(pCurrMem->usUsed - usCurrOffset, usLen);
    DRV_RT_MEMCPY(pMem->pData, pCurrMem->pData + usCurrOffset, usCopyLen);
    usWorkedLen = usCopyLen;

    /*================*/ /*  后续块可以从头开始处理，无需偏移 */
    pCurrMem        = pCurrMem->pNext;

    while (usWorkedLen < usLen)
    {
        /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
        usCopyLen   = PS_MIN(pCurrMem->usUsed, usLen - usWorkedLen);
        DRV_RT_MEMCPY(pMem->pData + usWorkedLen, pCurrMem->pData, usCopyLen);
        usWorkedLen    += usCopyLen;
        pCurrMem        = pCurrMem->pNext;
    }

    /*更新used长度*/
    pMem->usUsed = usLen;

    return pMem;
}   /* TTF_MemBlkCopyAllocFromMem_Debug */


/*****************************************************************************
 Prototype       : TTF_MemBlkCopyAlloc_Debug
 Description     : 分配一块自己模块管理的TTF内存块，并且填入已经填写完成的数据内容
 Input           : ulPid       -- 调用此函数的模块的Pid
                   pPdu        -- 已经完成填写的数据头指针
                   usLen       -- 要复制的数据块长度,单位为字节
 Output          : NONE
 Return Value    : TTF_MEM_ST* -- 指向分配好的块的指针
                                  或VOS_NULL_PTR表示失败

 History         :
    Date         : 2009-12-13
    Author       : dengqingshan
*****************************************************************************/
TTF_MEM_ST *TTF_MemBlkCopyAlloc_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
                    VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, VOS_UINT8 *pData, VOS_UINT16 usLen)
{
    TTF_MEM_ST                         *pMem = VOS_NULL_PTR;
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;


    /*====================*/ /* 参数检查 */
    if (VOS_NULL_PTR == pData)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:TTF_MemBlkCopyAlloc_Debug MemCopy pData is NULL!\n");
        return VOS_NULL_PTR;
    }

    if (0 == usLen)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:TTF_MemBlkCopyAlloc_Debug Alloc MemCopy Len is 0!\n");
        return VOS_NULL_PTR;
    }

    if( ulPoolId >= TTF_MEM_POOL_ID_BUTT )
    {
        PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Poolid <1> is invalid!",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /*================================*/    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Pool<1> is not used!",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /*====================*/ /* 分配一块长度为usLen的TTF内存块 */
    pMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usLen);

    if (VOS_NULL_PTR == pMem)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Error: MemCopy, Alloc Mem is Fail!\n");
        return VOS_NULL_PTR;
    }

    /*====================*/ /* 填入已经填写完成的数据内容 */
    DRV_RT_MEMCPY(pMem->pData, pData, usLen);/*COPY至内存数据部分*/

    /*更新used长度*/
    pMem->usUsed = usLen;

    return pMem;
} /* TTF_MemBlkCopyAlloc_Debug */


/*****************************************************************************
 Prototype       : TTF_MemBlkCopyAllocPacket_Debug
 Description     : 分配一块自己模块管理的TTF内存块，并且填入已经填写完成的数据内容
 Input           : ulPid       -- 调用此函数的模块的Pid
                   pPdu        -- 已经完成填写的数据头指针
                   usLen       -- 要复制的数据块长度,单位为字节
 Output          : NONE
 Return Value    : TTF_MEM_ST* -- 指向分配好的块的指针
                                  或VOS_NULL_PTR表示失败

 History         :
    Date         : 2009-12-13
    Author       : dengqingshan
*****************************************************************************/
TTF_MEM_ST *TTF_MemBlkCopyAllocPacket_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
                    VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, VOS_UINT8 *pData, VOS_UINT16 usLen)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;
    TTF_MEM_ST                         *pMem;
    TTF_MEM_ST                         *pTailMem;
    VOS_UINT16                          usCopyLen;
    VOS_UINT16                          usWorkedLen;


    /*====================*/ /* 参数检查 */
    if (VOS_NULL_PTR == pData)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MemBlkCopyAllocPacket_Debug, WARNING, SrcData is NULL! FileId <1>, LineNum <2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return VOS_NULL_PTR;
    }

    if (0 == usLen)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MemBlkCopyAllocPacket_Debug, WARNING, Len is 0! FileId <1>, LineNum <2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return VOS_NULL_PTR;
    }

    if ( ulPoolId >= TTF_MEM_POOL_ID_BUTT )
    {
        PS_LOG3(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MemBlkCopyAllocPacket_Debug, WARNING, Poolid <1> is invalid! FileId <2>, LineNum <3>",
            (VOS_INT32)ulPoolId, (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return VOS_NULL_PTR;
    }

    /*================================*/    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        PS_LOG3(ulPid, 0, PS_PRINT_WARNING,
            "TTF_MemBlkCopyAllocPacket_Debug, WARNING, Pool<1> is not used!, FileId <2>, LineNum <3>",
            (VOS_INT32)ulPoolId, (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return VOS_NULL_PTR;
    }

    /*================================*/    /* 申请内存，并拷贝 */
    usWorkedLen = 0;
    pMem        = VOS_NULL_PTR;

    while (usWorkedLen < usLen)
    {
        usCopyLen   = PS_MIN( (usLen - usWorkedLen), pstTtfMemPool->usMaxByteLen );

        pTailMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usCopyLen);

        if (VOS_NULL_PTR == pTailMem)
        {
            PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
                "TTF_MemBlkCopyAllocPacket_Debug, WARNING, AllocMem Fail, FileId <1>, LineNum <2>",
                (VOS_INT32)usFileID, (VOS_INT32)usLineNum);

            TTF_MemFree_Debug(usFileID, usLineNum, ulPid, &pMem);
            return VOS_NULL_PTR;
        }

        DRV_RT_MEMCPY(pTailMem->pData, pData + usWorkedLen, usCopyLen);
        pTailMem->usUsed    = usCopyLen;
        usWorkedLen        += usCopyLen;

        TTF_MemLink(ulPid, &pMem, pTailMem);
    }

    return pMem;
} /* TTF_MemBlkCopyAllocPacket_Debug */


/*****************************************************************************
 函 数 名  : TTF_MemBlkCheckPoolLeak
 功能描述  : 对外提供接口, 判断TTF BLK MEM是否有泄漏
 输入参数  : VOS_UINT32 ulPoolId 检查的内存池ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年7月22日
    作    者   : zengfei 57034
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  TTF_MemBlkCheckPoolLeak(VOS_UINT32 ulPoolId)
{
    TTF_MEM_POOL_STRU              *pstTtfMemPool;


    if ( ulPoolId >= TTF_MEM_POOL_ID_BUTT )
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemBlkCheckPoolLeak,Poolid %d is invalid!\n",
            (VOS_INT32)ulPoolId);
        return PS_FALSE;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemBlkCheckPoolLeak,Pool %d is not used!\n",
            (VOS_INT32)ulPoolId);
        return PS_FALSE;
    }

    if (0 == pstTtfMemPool->usTtfMemUsedCnt)
    {
        return PS_FALSE;
    }

    TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR, "TTF_MEM_BLK Is Leak,Pool %d,UsedCnt %d \n!",
        (VOS_INT32)ulPoolId, pstTtfMemPool->usTtfMemUsedCnt);

    TTF_BlkMemUsedInfoEventRpt(WUEPS_PID_RLC, ulPoolId, TRIG_TYPE_LEAK);

    return PS_TRUE;
} /* TTF_MemBlkCheckPoolLeak */

VOS_VOID TTF_MemSndNormalEvent(VOS_UINT8 ucPoolId)
{
    VOS_UINT32                              ulLength;
    VOS_UINT32                              ulDataLen;
    PS_OM_EVENT_IND_STRU                   *pstTtfOmEventInd = VOS_NULL_PTR;
    TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU    *pstTtfOmEventBlkMemUsedInfo;


    ulDataLen           = offsetof(TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU, astBlkMemTraceInfo);

    /*消息总长度*/
    ulLength            = offsetof(PS_OM_EVENT_IND_STRU, aucData) + ulDataLen;

    /*申请消息内存*/
    pstTtfOmEventInd    = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(WUEPS_PID_RLC, ulLength);

    if (VOS_NULL_PTR == pstTtfOmEventInd )
    {
        PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemBlkCheckPoolLeak: MemAlloc Fail!");
        return ;
    }

    /*填写消息内容*/
    pstTtfOmEventInd->ulLength      = ulLength - offsetof( PS_OM_EVENT_IND_STRU, usEventId);
    pstTtfOmEventInd->usEventId     = TTF_EVENT_BLK_MEM_USED_INFO;
    pstTtfOmEventInd->ulModuleId    = WUEPS_PID_RLC;/* 由于Event目前只能对应一个固定的PID，在此只能先暂定为RLC PID */

    pstTtfOmEventBlkMemUsedInfo     = (TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU *)pstTtfOmEventInd->aucData;
    pstTtfOmEventBlkMemUsedInfo->ucPoolId       = ucPoolId;
    pstTtfOmEventBlkMemUsedInfo->enTrigType     = TRIG_TYPE_NORMAL;
    pstTtfOmEventBlkMemUsedInfo->ucNo           = 1;
    pstTtfOmEventBlkMemUsedInfo->usRptCnt       = 0;
    pstTtfOmEventBlkMemUsedInfo->ulTotalUsedCnt = 0;

    /*调用OM的事件上报接口*/
    if ( PS_SUCC != OM_Event((PS_OM_EVENT_IND_STRU*)pstTtfOmEventInd))
    {
        PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemBlkCheckPoolLeak: OM Send Event Fail!");
    }
    /*释放申请的内存*/
    PS_MEM_FREE(WUEPS_PID_RLC, pstTtfOmEventInd);
    return;


}



VOS_UINT32 TTF_MemReportMemLeakMsg( VOS_UINT32 ulPid )
{
    TTF_MEM_LEAK_INFO_IND_MSG          *pstTtfMemLeakInfoInd;

    /*申请消息  */
    pstTtfMemLeakInfoInd = (TTF_MEM_LEAK_INFO_IND_MSG *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ulPid,
                                               sizeof(TTF_MEM_LEAK_INFO_IND_MSG));

    /* 内存申请失败，返回 */
    if( VOS_NULL_PTR == pstTtfMemLeakInfoInd )
    {
        return VOS_ERR;
    }

    /*填写消息内容*/
    pstTtfMemLeakInfoInd->ulReceiverPid     = ulPid;
    pstTtfMemLeakInfoInd->ulMsgId           = ID_TRACE_TTF_MEM_LEAK_IND;

    /* 发送消息 */
    TTF_TRACE_MSG(pstTtfMemLeakInfoInd);

    PS_FREE_MSG(ulPid, pstTtfMemLeakInfoInd);

    return VOS_OK;

}


/*****************************************************************************
 函 数 名  : TTF_MemBlkCheckLeak
 功能描述  : 对外提供接口, 判断TTF BLK MEM是否有泄漏
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年7月22日
    作    者   : zengfei 57034
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  TTF_MemBlkCheckLeak(VOS_VOID)
{
    VOS_UINT32                          ulPoolId = TTF_MEM_POOL_ID_UL_DATA;
    VOS_UINT32                          ulRet    = PS_FALSE;
    VOS_UINT32                          ulFinal = PS_FALSE;


    ulRet = TTF_MemBlkCheckPoolLeak(ulPoolId);

    if (ulRet == PS_TRUE)
    {
        ulFinal = PS_TRUE;
    }

    ulRet = TTF_MemBlkCheckPoolLeak(TTF_MEM_POOL_ID_EXT);

    if ( ulRet == PS_TRUE)
    {
        ulFinal = PS_TRUE;
    }

    return ulFinal;
} /* TTF_MemBlkCheckLeak */

VOS_VOID *TTF_MemAllocCtrlMem(VOS_UINT32 ulLength)
{
    return VOS_CacheMemAlloc(ulLength);
}


VOS_UINT32 TTF_MemPoolInit
(
    VOS_UINT8                            ucPoolId,
    VOS_UINT8                          **ppucDataMemAddr
)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;
    TTF_MEM_ST                        **ppst1stTtfMem;
    TTF_MEM_ST                         *pstTtfMem;
    TTF_MEM_CLUSTER_STRU               *pstTtfMemCluster;
    VOS_UINT32                          ulTtfMemCnt;
    VOS_UINT8                           ucClusterId;
    TTF_MEM_ST                        **ppTempMem;
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    TTF_BLK_MEM_DEBUG_ST               *pstMemDbInfo;
#endif


    pstTtfMemPool = TTF_MEM_GET_POOL(ucPoolId);

    if (PS_TRUE == pstTtfMemPool->usUsedFlag)
    {
        return PS_SUCC;
    }

    /*================================*/
    /* 申请对应内存池的可用内存索引表，每个POOL申请一次，后面分割给每个cluster使用 */
    ppst1stTtfMem = (TTF_MEM_ST **)TTF_MemAllocCtrlMem(sizeof(TTF_MEM_ST *) * pstTtfMemPool->usTtfMemTotalCnt);

    if (VOS_NULL_PTR == ppst1stTtfMem)
    {
        return PS_FAIL;
    }

    /*================================*/
    /* 申请对应内存池的TTF_MEM，每个POOL申请一次，后面分割给每个cluster使用 */
    pstTtfMem = (TTF_MEM_ST *)TTF_MemAllocCtrlMem(sizeof(TTF_MEM_ST) * pstTtfMemPool->usTtfMemTotalCnt);

    if (VOS_NULL_PTR == pstTtfMem)
    {
        return PS_FAIL;
    }


#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    /* 申请TTF_MEM对应的TTF_BLK_MEM_DEBUG_ST */
    pstMemDbInfo = (TTF_BLK_MEM_DEBUG_ST *)TTF_MemAllocCtrlMem(sizeof(TTF_BLK_MEM_DEBUG_ST) * pstTtfMemPool->usTtfMemTotalCnt);

    if (VOS_NULL_PTR == pstMemDbInfo)
    {
        return PS_FAIL;
    }
#endif

    memset(ppst1stTtfMem, 0, sizeof(TTF_MEM_ST *) * pstTtfMemPool->usTtfMemTotalCnt);
    memset(pstTtfMem, 0, sizeof(TTF_MEM_ST) * pstTtfMemPool->usTtfMemTotalCnt);

#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
    memset(pstMemDbInfo, 0, sizeof(TTF_BLK_MEM_DEBUG_ST) * pstTtfMemPool->usTtfMemTotalCnt);
#endif

    ppTempMem = ppst1stTtfMem;

    /*================================*/    /* 记录该内存池所有TTF_MEM指针，检查内存泄露使用 */
    pstTtfMemPool->pstTtfMemStStartAddr    = pstTtfMem;


    /*================================*/    /* 建立每个cluster的TTF_MEM */
    for (ucClusterId = 0; ucClusterId < pstTtfMemPool->ucClusterCnt; ucClusterId++)
    {
        pstTtfMemCluster    = &(pstTtfMemPool->astClusterTable[ucClusterId]);

        /* 建立级数索引表和可用内存索引表的关系 */
        pstTtfMemCluster->apstFreeStack = ppTempMem;

        for (ulTtfMemCnt = 0;
             ulTtfMemCnt < pstTtfMemCluster->usFreeCnt;
             ulTtfMemCnt ++)
        {
            TTF_MEM_INIT(pstTtfMem, TTF_MEM_TYPE_BLK, pstTtfMemCluster->usLen);

            pstTtfMem->ucPoolId     = ucPoolId;

            if ( TTF_MEM_POOL_ID_EXT == pstTtfMem->ucPoolId )
            {
                pstTtfMem->pData = VOS_NULL_PTR;
            }
            else
            {
                pstTtfMem->pData        = *ppucDataMemAddr; /* 建立TTF_MEM_ST和对应数据的关系 */
            }

            pstTtfMem->ucClusterId  = ucClusterId;
            pstTtfMem->pOrigData    = pstTtfMem->pData;
#if (FEATURE_ON == FEATURE_TTF_MEM_DEBUG)
            pstTtfMem->pstDbgInfo   = pstMemDbInfo;
            pstMemDbInfo++;
#endif
             /* 建立可用内存索引表和TTF_MEM_ST的关系 */

           *ppTempMem               = pstTtfMem;

            pstTtfMem++;
            ppTempMem++;

            *ppucDataMemAddr       += pstTtfMemCluster->usLen;
        }

    }

    return PS_SUCC;
} /* TTF_MemPoolInit */

VOS_UINT32 TTF_MemPoolCreate
(
    VOS_UINT8                     ucPoolId,
    TTF_MEM_POOL_CFG_NV_STRU     *pstPoolCfg,
    VOS_UINT8                   **ppucDataMemAddr,
    VOS_UINT32                   *pulLength
)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;
    TTF_MEM_CLUSTER_STRU               *pstMemCluster;
    VOS_UINT32                          usLastClusterMaxLen;
    VOS_UINT32                          usDataLen;
    VOS_UINT8                           ucClusterId;
    VOS_UINT8                          *pucDataMemAddr;
    VOS_UINT8                           ucClusterCnt = 0;
    VOS_UINT32                          ulMemSize;

    pucDataMemAddr  = (VOS_UINT8 *)(*ppucDataMemAddr);

    pstTtfMemPool   = TTF_MEM_GET_POOL(ucPoolId);

    /* 检查对应ID的内存池是否有被重复创建 */
    if (PS_TRUE == pstTtfMemPool->usUsedFlag)
    {
        PS_LOG1(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemPoolCreate Fail, ulPoolId %d is already used \n",
            ucPoolId);
        return PS_FAIL;
    }

    ucClusterCnt                    = pstPoolCfg->ucClusterCnt;

    /* 初始化内存池的通用变量 */
    pstTtfMemPool->usUsedFlag       = PS_FALSE;
    pstTtfMemPool->ucClusterCnt     = ucClusterCnt;
    pstTtfMemPool->usTtfMemTotalCnt = 0;

    pstTtfMemPool->usMaxByteLen     = pstPoolCfg->ausBlkSize[ucClusterCnt - 1];

    PS_MEM_SET(pstTtfMemPool->aucLenIndexTable, 0, TTF_MEM_BLK_MAX_BYTE_LEN);

    PS_MEM_SET(pstTtfMemPool->astClusterTable, 0,
        sizeof(TTF_MEM_CLUSTER_STRU) * TTF_MEM_MAX_CLUSTER_NUM);

    usLastClusterMaxLen   = 1;

    for (ucClusterId = 0; ucClusterId < ucClusterCnt; ucClusterId++)
    {
        pstMemCluster                   = &(pstTtfMemPool->astClusterTable[ucClusterId]);

        pstMemCluster->usTotalCnt       = pstPoolCfg->ausBlkCnt[ucClusterId];
        pstMemCluster->usFreeCnt        = pstPoolCfg->ausBlkCnt[ucClusterId];
        pstMemCluster->usLen            = pstPoolCfg->ausBlkSize[ucClusterId];

        ulMemSize = pstPoolCfg->ausBlkSize[ucClusterId] * pstPoolCfg->ausBlkCnt[ucClusterId] ;

        *pulLength       -= ulMemSize;
        *ppucDataMemAddr += ulMemSize;

        for(usDataLen = usLastClusterMaxLen;
            usDataLen <= pstPoolCfg->ausBlkSize[ucClusterId];
            usDataLen++)
        {
            pstTtfMemPool->aucLenIndexTable[usDataLen] = ucClusterId;
        }

        usLastClusterMaxLen                 = usDataLen;

        pstTtfMemPool->usTtfMemTotalCnt    += pstPoolCfg->ausBlkCnt[ucClusterId];
    }

    if(PS_SUCC != TTF_MemPoolInit(ucPoolId, &pucDataMemAddr))
    {
        return PS_FAIL;
    }

    pstTtfMemPool->usUsedFlag   = PS_TRUE;

    return PS_SUCC;
} /* TTF_MemPoolCreate */

VOS_UINT32 TTF_MemNvCfgTotalLength(TTF_MEM_SOLUTION_CFG_NV_STRU *pstTtfMemSolution)
{
    TTF_MEM_POOL_CFG_NV_STRU      *pstPoolCfg;
    VOS_UINT8                 ucLoop;
    VOS_UINT8                 ucClusterNum;
    VOS_UINT8                 ucClusterLoop;
    VOS_UINT32                ulTotalLength = 0;

    /*检查各内存池各cluster是否正确*/
    for (ucLoop = 0; ucLoop < pstTtfMemSolution->ucPoolCnt; ucLoop++)
    {
        pstPoolCfg   = &pstTtfMemSolution->astTtfMemPoolCfgInfo[ucLoop];
        ucClusterNum = pstPoolCfg->ucClusterCnt;

        for(ucClusterLoop = 0 ; ucClusterLoop < ucClusterNum ; ucClusterLoop++)
        {
            ulTotalLength += pstPoolCfg->ausBlkSize[ucClusterLoop] * pstPoolCfg->ausBlkCnt[ucClusterLoop];
        }
    }

    return ulTotalLength;
}
VOS_UINT32 TTF_MemReadNvCfg(TTF_MEM_SOLUTION_CFG_NV_STRU *pstTtfMemSolution)
{
    VOS_UINT32                     ulRet;
    VOS_UINT8                      ucLoop;
    VOS_UINT8                      ucClusterNum;
    VOS_UINT8                      ucClusterLoop;
    TTF_MEM_POOL_CFG_NV_STRU      *pstPoolCfg;

    /*读取TTF内存方案*/
    ulRet = NV_Read (en_NV_Item_TTF_MEM_SOLUTION_CFG, pstTtfMemSolution , sizeof(TTF_MEM_SOLUTION_CFG_NV_STRU));
    if (NV_OK != ulRet)
    {
        PS_LOG1(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemReadNvCfg Fail, Read NV FAIL, Error Code <1>\n", (VOS_INT32)ulRet);
        return PS_FAIL;
    }

    if (TTF_MEM_POOL_ID_BUTT != pstTtfMemSolution->ucPoolCnt)
    {
        PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemReadNvCfg Fail, Pool Num Error \n");
        return PS_FAIL;
    }

    /*检查各内存池各cluster是否正确*/
    for (ucLoop = 0; ucLoop < pstTtfMemSolution->ucPoolCnt; ucLoop++)
    {
        pstPoolCfg   = &pstTtfMemSolution->astTtfMemPoolCfgInfo[ucLoop];
        ucClusterNum = pstPoolCfg->ucClusterCnt;

        if (ucClusterNum > TTF_MEM_MAX_CLUSTER_NUM)
        {
            PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemReadNvCfg Fail, Cluster Num Error \n");
            return PS_FAIL;
        }

        for(ucClusterLoop = 0 ; ucClusterLoop < ucClusterNum ; ucClusterLoop++)
        {
            if (pstPoolCfg->ausBlkSize[ucClusterLoop] > TTF_MEM_BLK_MAX_BYTE_LEN)
            {
                PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemReadNvCfg Fail, Memory Size Error \n");
                return PS_FAIL;
            }
        }
    }


    return PS_SUCC;
}



VOS_VOID TTF_MemPrintSolutionCfg(VOS_VOID)
{

    TTF_MEM_SOLUTION_CFG_NV_STRU  *pstSolutionCfg = &g_stTtfMemSolution;
    TTF_MEM_POOL_CFG_NV_STRU      *pstPoolCfg;

    VOS_UINT8                     ucPoolCnt;
    VOS_UINT8                     ucPoolLoop;
    VOS_UINT8                     ucClusterCnt;
    VOS_UINT8                     ucClusterLoop;

    ucPoolCnt = pstSolutionCfg->ucPoolCnt;

    vos_printf("TTF Mem Pool Solution Print :\r\n");
    vos_printf("TTF Mem Pool Cnt %d \r\n", ucPoolCnt);

    for(ucPoolLoop = 0; ucPoolLoop < ucPoolCnt; ucPoolLoop++)
    {
        pstPoolCfg = &pstSolutionCfg->astTtfMemPoolCfgInfo[ucPoolLoop];

        ucClusterCnt = pstPoolCfg->ucClusterCnt;
        vos_printf("TTF Mem Pool [%d] Cluster Cnt %d \r\n", ucPoolLoop, ucClusterCnt);

        for (ucClusterLoop = 0; ucClusterLoop < ucClusterCnt; ucClusterLoop++)
        {
            vos_printf("TTF Mem Pool [%d] Cluster [%d] -- size [%d] cnt[%d]\r\n",
                ucPoolLoop, ucClusterLoop, pstPoolCfg->ausBlkSize[ucClusterLoop], pstPoolCfg->ausBlkCnt[ucClusterLoop]);
        }

    }

    vos_printf("TTF Mem Pool Solution Print END\r\n");
    return;
}


/*****************************************************************************
 函 数 名  : TTF_MemBlkIsAlert
 功能描述  : 对外提供接口, 判断TTF BLK MEM剩余数是否低于门限
 输入参数  : VOS_UINT32 ulPoolId    检查的内存池ID
 输出参数  : 无
 返 回 值  : PS_TRUE : TTF BLK MEM剩余数低于门限
             PS_FALSE: TTF BLK MEM剩余数高于门限
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年7月22日
    作    者   : zengfei 57034
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  TTF_MemBlkIsAlert(VOS_UINT32 ulPoolId)
{
    VOS_UINT16 usFreeCnt = 0xFFFF;

    usFreeCnt = g_astTtfMemPool[ulPoolId].usTtfMemTotalCnt
                    - g_astTtfMemPool[ulPoolId].usTtfMemUsedCnt;

    if (usFreeCnt < TTF_BLK_MEM_ALERT_THLD)
    {
        TTF_BlkMemUsedInfoEventRpt(WUEPS_PID_RLC, ulPoolId, TRIG_TYPE_ALERT);
        return PS_TRUE;
    }

    return PS_FALSE;
}


/*****************************************************************************
 函 数 名  : TTF_MemBlkAddHeadData_Debug
 功能描述  : 从TTF_MEM块头部填充指定字节数目的数据
 输入参数  : ulPid       -- 调用此函数的模块的Pid
             ppMemDest   -- 要填写的TTF内存块头指针
             pSrcData    -- 目的数据地址
             usAddLen    -- 填充数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : PS_SUCC -- 成功; PS_FAIL -- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰，蒋丽萍
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 TTF_MemBlkAddHeadData_Debug
(
    VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId, TTF_MEM_ST **ppMemDest, VOS_UINT8 *pSrcData,
    VOS_UINT16 usAddLen
)
{
    TTF_MEM_ST*         pDestMem;
    TTF_MEM_ST*         pHeaderMem;
    PS_BOOL_ENUM_UINT8  enAllocMem;     /*是否需要新申请内存的标志位。*/

    /*====================*/ /* 参数检查 */
    if (VOS_NULL_PTR == ppMemDest)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par ppMemDest is Null!\n");
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == pSrcData)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par pSrcData is Null!\n");
        return PS_FAIL;
    }

    if ( 0 == usAddLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par usAddLen is 0!\n");
        return PS_FAIL;
    }

    pDestMem    = *ppMemDest;

    /*====================*/    /* 判断是否需要申请新的TTF_MEM块保存头部 */
    if (VOS_NULL_PTR == pDestMem)
    {
        /*若当前TTF内存块不存在，则申请动态数据块*/
        enAllocMem    = PS_TRUE;
    }
    else
    {
        if (usAddLen >  (VOS_UINT16)(pDestMem->pData - pDestMem->pOrigData))
        {
            /*头部预留的长度不足以填下头部，则申请专用内存数据块*/
            enAllocMem    = PS_TRUE;
        }
        else
        {
            enAllocMem    = PS_FALSE;
        }
    }

    if ( PS_TRUE == enAllocMem )
    {
        pHeaderMem    = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usAddLen);
        if (VOS_NULL_PTR == pHeaderMem)
        {
            PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Error:Alloc pHeaderMem is Null!\n");
            return PS_FAIL;
        }

        DRV_RT_MEMCPY(pHeaderMem->pData, pSrcData, usAddLen);
        pHeaderMem->usUsed    = usAddLen;

        TTF_MemLink(ulPid, &pHeaderMem, pDestMem);
        *ppMemDest  = pHeaderMem;
    }
    else
    {
        PS_ASSERT_RTN((VOS_NULL_PTR != pDestMem), PS_FAIL);
        TTF_MEM_RESUME_TO_HDR(pDestMem, usAddLen);
        DRV_RT_MEMCPY(pDestMem->pData, pSrcData, usAddLen);
    }
    return PS_SUCC;
} /* TTF_MemBlkAddHeadData_Debug */


/*****************************************************************************
 函 数 名  : TTF_MemBlkAddTailData_Debug
 功能描述  : 从TTF_MEM块尾部填充指定字节数目的数据
 输入参数  : VOS_UINT32 ulFileID    文件名
             VOS_UINT32 ulFileID    行号
             VOS_UINT32 ulPid       调用此函数的模块的Pid
             TTF_MEM_ST **ppMemDest 要填写的TTF内存块头指针
             VOS_UINT8  *pSrcData   目的数据地址
             VOS_UINT16 usAddLen    填充数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : PS_SUCC -- 成功; PS_FAIL -- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰，蒋丽萍
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 TTF_MemBlkAddTailData_Debug
(
    VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId, TTF_MEM_ST **ppMemDest, VOS_UINT8  *pSrcData,
    VOS_UINT16  usAddLen
)
{
    TTF_MEM_ST         *pDestMem;
    TTF_MEM_ST         *pTailMem;
    PS_BOOL_ENUM_UINT8  enAllocMem;

    /*====================*/    /* 参数检查 */
    PS_ASSERT_RTN((VOS_NULL_PTR != ppMemDest), PS_FAIL);
    PS_ASSERT_RTN((VOS_NULL_PTR != pSrcData), PS_FAIL);
    PS_ASSERT_RTN((0 != usAddLen), PS_FAIL);

    pDestMem    = *ppMemDest;

    if (VOS_NULL_PTR == pDestMem)
    {
        enAllocMem      = PS_TRUE;
    }
    else
    {
        while (VOS_NULL_PTR != pDestMem->pNext)
        {
            pDestMem    = pDestMem->pNext;
        }

        if ( usAddLen >  (pDestMem->usLen - pDestMem->usUsed) )
        {
            enAllocMem    = PS_TRUE;
        }
        else
        {
            enAllocMem    = PS_FALSE;
        }
    }

    if ( PS_TRUE == enAllocMem )
    {
        pTailMem    = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId,usAddLen);
        if (VOS_NULL_PTR == pTailMem)
        {
            PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Alloc pTailMem is Null!\n");
            return PS_FAIL;
        }

        DRV_RT_MEMCPY(pTailMem->pData, pSrcData, usAddLen);
        pTailMem->usUsed    = usAddLen;

        TTF_MemLink(ulPid, &pDestMem, pTailMem);

        if (VOS_NULL_PTR == *ppMemDest)
        {
            *ppMemDest  = pDestMem;
        }

    }
    else
    {
        PS_ASSERT_RTN((VOS_NULL_PTR != pDestMem), PS_FAIL);
        DRV_RT_MEMCPY(&pDestMem->pData[pDestMem->usUsed], pSrcData, usAddLen);

        pDestMem->usUsed    += usAddLen;
    }

    return PS_SUCC;
} /* TTF_MemBlkAddTailData_Debug */

/*****************************************************************************
 函 数 名  : TTF_MemBlkAddTailDataWithLastMem_Debug
 功能描述  : 从TTF_MEM块尾部填充指定字节数目的数据
 输入参数  : ulFileID    -- File-ID
             ulLineNum   -- File Line
             ulPid       -- 调用此函数的模块的Pid
             ppMemDest   -- 存放要填写的TTF内存块头指针的地址
             pLastMem    -- 要填写的TTF内存块尾指针
             pSrcData    -- 目的数据地址
             usAddLen    -- 填充数据的长度，单位为字节
 输出参数  : ppMemDest   -- 存放要填写的TTF内存块头指针的地址
 返 回 值  : PS_SUCC -- 成功; PS_FAIL -- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月2日
    作    者   : liukai
    修改内容   : Created
*****************************************************************************/
VOS_UINT32 TTF_MemBlkAddTailDataWithLastMem_Debug
(
    VOS_UINT16 usFileID,VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    VOS_UINT32 ulPoodId,TTF_MEM_ST **ppMemDest, TTF_MEM_ST *pLastMem,
    VOS_UINT8  *pSrcData, VOS_UINT16  usAddLen
)
{
    TTF_MEM_ST *pDestMem;
    TTF_MEM_ST *pTailMem;
    PS_BOOL_ENUM_UINT8  enAllocMem;

    /*====================*/    /* 参数检查 */
    PS_ASSERT_RTN((VOS_NULL_PTR != ppMemDest), PS_FAIL);
    PS_ASSERT_RTN((VOS_NULL_PTR != pSrcData), PS_FAIL);

    pDestMem    = *ppMemDest;

    if (VOS_NULL_PTR == pDestMem)
    {
        enAllocMem    = PS_TRUE;
    }
    else
    {
        pDestMem = pLastMem;

        /* 判断是否需要申请新的TTF_MEM块保存头部 */
        if (( TTF_MEM_TYPE_BLK == pDestMem->usType ) || ( TTF_MEM_TYPE_DYN == pDestMem->usType ))
        {
            /*头部预留的长度不足以填下头部*/
            if ( usAddLen >  (pDestMem->usLen - pDestMem->usUsed) )
            {
                enAllocMem    = PS_TRUE;
            }
            else
            {
                enAllocMem    = PS_FALSE;
            }
        }
        else
        {
            PS_LOG1(ulPid, 0, PS_PRINT_WARNING, "Warning: TTFMem Type <1> is Wrong!\n",
                pDestMem->usType);
            return PS_FAIL;
        }
    }

    if ( PS_TRUE == enAllocMem )
    {
        pTailMem    = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoodId, usAddLen);

        if (VOS_NULL_PTR == pTailMem)
        {
            PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Alloc pTailMem is Null!\n");
            return PS_FAIL;
        }

        DRV_RT_MEMCPY(pTailMem->pData, pSrcData, usAddLen);
        pTailMem->usUsed    = usAddLen;

        if (VOS_NULL_PTR != pLastMem)
        {
            pLastMem->pNext = pTailMem;  /* 将pMemTail链接上去 */
        }
        else
        {
            *ppMemDest = pTailMem;
        }
    }
    else
    {
        PS_ASSERT_RTN((VOS_NULL_PTR != pDestMem), PS_FAIL);
        DRV_RT_MEMCPY(&pDestMem->pData[pDestMem->usUsed], pSrcData, usAddLen);

        pDestMem->usUsed    += usAddLen;
    }

    return PS_SUCC;
}    /* TTF_MemBlkAddTailDataWithLastMem_Debug */


/*****************************************************************************
 函 数 名  : TTF_MemSingleFree_Debug
 功能描述  :
 输入参数  : VOS_INT8 *cFileName
             VOS_UINT32 ulLineNum
             VOS_UINT32 ulPid
             TTF_MEM_ST *pMem
 输出参数  : 无
 返 回 值  : TTF_MEM_ST *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年3月24日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
TTF_MEM_ST *TTF_MemSingleFree_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
                VOS_UINT32 ulPid, TTF_MEM_ST *pMem)
{
    TTF_MEM_ST         *pRtnMem;

    /* 参数检查 */
    if ( VOS_NULL_PTR == pMem )
    {
        TTF_MEM_FREE_SAVE_NULL_PTR_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPtrInfo,usFileID,usLineNum);
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "TTF_MemSingleFree_Debug, pMem is NULL! FileID %d, LineNum %d \n", usFileID, usLineNum);
        return  VOS_NULL_PTR;
    }

    if ( TTF_MEM_TYPE_BLK != pMem->usType )
    {
        TTF_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeInvalidMemInfo,pMem,TTF_INVALID_MEM_TYPE_BLKTYPE,pMem->usType,usFileID,usLineNum);
        TTF_LOG3(ulPid, PS_PRINT_WARNING, "TTF_MemSingleFree_Debug, Mem Type %d is invalid! FileID %d, LineNum %d \n",
            (VOS_INT32)pMem->usType, usFileID, usLineNum);
        return  VOS_NULL_PTR;
    }

    /* 释放内存 */
    pRtnMem = TTF_MemBlkSingleFree_Debug(usFileID, usLineNum, pMem);

    return pRtnMem;
}    /* TTF_MemSingleFree_Debug */


/*****************************************************************************
 函 数 名  : TTF_MemFree_Debug
 功能描述  :
 输入参数  : VOS_INT8 *cFileName
             VOS_UINT32 ulLineNum
             VOS_UINT32 ulPid
             TTF_MEM_ST *pMem
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年3月24日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID TTF_MemFree_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
             VOS_UINT32 ulPid, TTF_MEM_ST **ppMem)
{
    TTF_MEM_ST     *pFree;

    /*====================*/ /* 参数检查 */
    if(VOS_NULL_PTR == ppMem)
    {
        TTF_MEM_FREE_SAVE_NULL_PTR_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPPtrInfo,usFileID,usLineNum);
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "Warning: TTF_MEMFREE ppMem is NullPtr!ulFileID %d,ulLineNum %d", (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return;
    }

    pFree   = *ppMem;
    if (VOS_NULL_PTR == *ppMem)
    {
        TTF_MEM_FREE_SAVE_NULL_PTR_INFO(g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPtrInfo,usFileID,usLineNum);
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "Warning: TTF_MEMFREE *ppMem is NullPtr!ulFileID %d,ulLineNum %d", (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return;
    }

    while (VOS_NULL_PTR != pFree)
    {
        pFree = TTF_MemSingleFree_Debug(usFileID, usLineNum, ulPid, pFree);
    }

    *ppMem = VOS_NULL_PTR;

}    /* TTF_MemFree_Debug */

/*****************************************************************************
 函 数 名  : TTF_MemBlkCutHeadData_Debug
 功能描述  : 从TTF_MEM块头部移去指定字节数目的数据
 输入参数  : ulPid    -- 调用此函数的模块的Pid
             pMemSrc  -- 要填写的TTF内存块头指针
             pDest    -- 目的数据地址
             usLen    -- 填写数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : 获取数据成功返回PS_SUCC；失败返回PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TTF_MemBlkCutHeadData_Debug
(
    VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    TTF_MEM_ST **ppMemSrc, VOS_UINT8 *pDest, VOS_UINT16 usLen
)
{
    TTF_MEM_ST *pCurrMem;
    VOS_UINT16  usWorkedLen     = 0;
    VOS_UINT16  usMemSrcLen;

    /*====================*/ /* 参数检查 */
    if (VOS_NULL_PTR == ppMemSrc)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par ppMemSrc is Null!\n");
        return PS_FAIL;
    }

    if ( (VOS_NULL_PTR == *ppMemSrc) || (VOS_NULL_PTR == pDest) )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par *ppMemSrc Or pDest is Null!\n");
        return PS_FAIL;
    }

    if ( 0 == usLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par usLen is 0!\n");
        return PS_FAIL;
    }

    /*====================*/ /*判断TTF内存块的长度是否符合要求*/
    pCurrMem        = *ppMemSrc;
    usMemSrcLen     = TTF_MemGetLen(ulPid, pCurrMem);

    if ( usMemSrcLen < usLen )
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning: MemSrcLen <1> Less Than usLen <2>!\n",
            usMemSrcLen, usLen);
        return PS_FAIL;
    }

    /*====================*/ /*获取所需长度的数据*/
    while (VOS_NULL_PTR != pCurrMem)
    {
        if (pCurrMem->usUsed <= (usLen - usWorkedLen))
        {
            /*截取整个TTFMem的数据*/
            DRV_RT_MEMCPY(pDest + usWorkedLen, pCurrMem->pData, pCurrMem->usUsed);

            usWorkedLen    += pCurrMem->usUsed;
            pCurrMem        = TTF_MemSingleFree_Debug(usFileID, usLineNum, ulPid, pCurrMem);

            if ( usLen == usWorkedLen )
            {
                *ppMemSrc   = pCurrMem;
                return PS_SUCC;
            }
        }
        else
        {
            /*截取当前TTFMem相应长度的数据*/
            DRV_RT_MEMCPY(pDest + usWorkedLen, pCurrMem->pData, (usLen - usWorkedLen));
            TTF_MEM_REMOVE_FROM_HDR(pCurrMem, (usLen - usWorkedLen));
            usWorkedLen     = usLen;
            *ppMemSrc   = pCurrMem;
            return PS_SUCC;
        }
    }

    PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: pCurrMem is Null!\n");
    return PS_FAIL;
} /* TTF_MemBlkCutHeadData_Debug */


/*****************************************************************************
 函 数 名  : TTF_MemBlkCutTailData_Debug
 功能描述  : 从TTF_MEM块尾部移去指定字节数目的数据
 输入参数  : ulPid    -- 调用此函数的模块的Pid
             ppMemSrc  -- 要填写的TTF内存块头指针
             pDest    -- 目的数据地址
             usLen    -- 填写数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : 获取数据成功返回PS_SUCC；失败返回PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT16 TTF_MemBlkCutTailData_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid, TTF_MEM_ST **ppMemSrc,
    VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem;
    TTF_MEM_ST *pDelMem = VOS_NULL_PTR;
    VOS_UINT16  usCurrLen;
    VOS_UINT16  usCurrOffset;
    VOS_UINT16  usWorkedLen;

    /*====================*/    /* 参数检查 */
    if (VOS_NULL_PTR == ppMemSrc)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "Warning: TTF_MemBlkCutTailData_Debug, ppMemSrc is NULL! File<1> ,Line<2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == *ppMemSrc)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "Warning: TTF_MemBlkCutTailData_Debug, *ppMemSrc is NULL! File<1> ,Line<2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == pDest)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "Warning: TTF_MemBlkCutTailData_Debug, pDest is NULL! File<1> ,Line<2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return PS_FAIL;
    }

    if (0 == usLen)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING,
            "Warning: TTF_MemBlkCutTailData_Debug, usLen = 0! File<1> ,Line<2>",
            (VOS_INT32)usFileID, (VOS_INT32)usLineNum);
        return PS_FAIL;
    }

    pCurrMem    = *ppMemSrc;
    pDelMem     = *ppMemSrc;    /*当非全部截取时，更新pDelMem*/

    /*====================*/    /* 寻找到需要的TTF_MEM_ST */
    usCurrLen   = TTF_MemGetLen(ulPid, pCurrMem);

    if (usCurrLen < usLen)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning: usCurrLen <1> Less Than usLen <2>!\n",
            usCurrLen, usLen);
        return PS_FAIL;
    }

    /* 为提高寻找效率，把对尾部的数据位置需求，转换成从头部开始找位置 */
    usCurrOffset    = usCurrLen - usLen;

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usUsed < usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pCurrMem        = pCurrMem->pNext;
        }

        /* 当前块长度等于偏移量，数据从下一个块第一个字节开始，之后都要拷贝出来，释放 */
        else if (pCurrMem->usUsed == usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pDelMem         = pCurrMem->pNext;
            pCurrMem->pNext = VOS_NULL_PTR;
            break;
        }
        /*当前块需要截取数据*/
        else
        {
            break;
        }
    }

    /* 经过前面的运算，pCurrMem肯定不为空 */
    PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

    usWorkedLen     = 0;
    /*================*/    /* 第一块需要进行偏移，特殊处理 */
    /*如果不是从第一个块的第一个数据开始拷贝，说明第一块不能释放 */
    if (0 != usCurrOffset)
    {
        usWorkedLen         = pCurrMem->usUsed - usCurrOffset;

        DRV_RT_MEMCPY(pDest, &pCurrMem->pData[usCurrOffset], usWorkedLen);

        pCurrMem->usUsed    = usCurrOffset;
        pDelMem             = pCurrMem->pNext;
        pCurrMem->pNext     = VOS_NULL_PTR;
    }

    /* 后面的块可以直接拷贝数据后，释放 */
    while (VOS_NULL_PTR != pDelMem)
    {
        DRV_RT_MEMCPY(pDest + usWorkedLen, pDelMem->pData, pDelMem->usUsed);
        usWorkedLen    += pDelMem->usUsed;
        pDelMem        = TTF_MemSingleFree(ulPid, pDelMem);
    }

    /*当全部截取时，源内存块返回空指针，其他情况不变*/
    if ( usCurrLen == usLen )
    {
        *ppMemSrc = VOS_NULL_PTR;
    }
    return PS_SUCC;
} /* TTF_MemBlkCutTailData_Debug */

/*****************************************************************************
 函 数 名  : TTF_MemBlkCutTailDataWithLastMem_Debug
 功能描述  : 从TTF_MEM块尾部移去指定字节数目的数据
 输入参数  : ulPid       -- 调用此函数的模块的Pid
             ppMemSrc    -- 存放要移去的TTF内存块头指针的地址
             pDest    -- 目的数据地址
             usLen    -- 填写数据的长度, 单位为字节
 输出参数  : ppMemSrc    -- 存放要移去的TTF内存块头指针的地址
             ppLastMem   -- 要移去的TTF内存块尾指针
 返 回 值  : PS_SUCC -- 成功; PS_FAIL -- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月3日
    作    者   : liukai
    修改内容   : Created
*****************************************************************************/
VOS_UINT16 TTF_MemBlkCutTailDataWithLastMem_Debug(VOS_UINT16 usFileID,
               VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
               TTF_MEM_ST **ppMemSrc, TTF_MEM_ST **ppLastMem,
               VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem;
    TTF_MEM_ST *pDelMem = VOS_NULL_PTR;
    VOS_UINT16  usCurrLen;
    VOS_UINT16  usCurrOffset;
    VOS_UINT16  usWorkedLen;

    /*====================*/    /* 参数检查 */
    PS_ASSERT_RTN((VOS_NULL_PTR != ppMemSrc), PS_FAIL);
    PS_ASSERT_RTN((VOS_NULL_PTR != *ppMemSrc), PS_FAIL);
    PS_ASSERT_RTN((VOS_NULL_PTR != pDest), PS_FAIL);

    pCurrMem    = *ppMemSrc;
    pDelMem     = *ppMemSrc;    /*当非全部截取时，更新pDelMem*/

    /*====================*/    /* 寻找到需要的TTF_MEM_ST */
    usCurrLen   = TTF_MemGetLen(ulPid, pCurrMem);

    if (usCurrLen < usLen)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning: usCurrLen <1> Less Than usLen <2>!\n",
            usCurrLen, usLen);
        return PS_FAIL;
    }

    /* 为提高寻找效率，把对尾部的数据位置需求，转换成从头部开始找位置 */
    usCurrOffset    = usCurrLen - usLen;

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usUsed < usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pCurrMem        = pCurrMem->pNext;
        }
        /*当前块长度等于偏移量，数据从下一个块第一个字节开始，之后都要拷贝出来，释放*/
        else if (pCurrMem->usUsed == usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pDelMem         = pCurrMem->pNext;
            pCurrMem->pNext = VOS_NULL_PTR;
            break;
        }
        /*当前块需要截取数据*/
        else
        {
            break;
        }
    }

    /* 经过前面的运算，pCurrMem肯定不为空 */
    PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

    usWorkedLen     = 0;
    /*================*/    /* 第一块需要进行偏移，特殊处理 */
    /*如果不是从第一个块的第一个数据开始拷贝，说明第一块不能释放 */
    if (0 != usCurrOffset)
    {
        usWorkedLen         = pCurrMem->usUsed - usCurrOffset;

        DRV_RT_MEMCPY(pDest, &pCurrMem->pData[usCurrOffset], usWorkedLen);

        pCurrMem->usUsed    = usCurrOffset;
        pDelMem             = pCurrMem->pNext;
        pCurrMem->pNext     = VOS_NULL_PTR;
    }

    *ppLastMem = pCurrMem;    /* update last TTF_MEM */

    /* 后面的块可以直接拷贝数据后，释放 */
    while (VOS_NULL_PTR != pDelMem)
    {
        if (VOS_NULL_PTR != pDest)
        {
            DRV_RT_MEMCPY(pDest + usWorkedLen, pDelMem->pData, pDelMem->usUsed);
        }
        usWorkedLen    += pDelMem->usUsed;
        pDelMem        = TTF_MemSingleFree_Debug(usFileID, usLineNum, ulPid, pDelMem);
    }

    /*当全部截取时，源内存块返回空指针，其他情况不变*/
    if ( usCurrLen == usLen )
    {
        *ppMemSrc = VOS_NULL_PTR;
        *ppLastMem = VOS_NULL_PTR;
    }
    return PS_SUCC;
} /* TTF_MemBlkCutTailDataWithLastMem_Debug */

TTF_MEM_ST *TTF_MemBlkRef_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
    VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, TTF_MEM_ST *pMemRef,
    VOS_UINT16 usOffset, VOS_UINT16  usLen)
{
    TTF_MEM_ST *pMem;
    VOS_UINT32  ulRtn;

    pMem    = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usLen);

    if (VOS_NULL_PTR == pMem)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Error: TTF_MemBlkAlloc_Debug Fail\n");
        return VOS_NULL_PTR;
    }

    ulRtn = TTF_MemGet(ulPid, pMemRef, usOffset, pMem->pData, usLen);

    if (PS_SUCC != ulRtn)
    {
        TTF_MemFree_Debug(usFileID, usLineNum, ulPid, &pMem);
        return VOS_NULL_PTR;
    }

    pMem->usUsed    = usLen;

    return pMem;
}/*TTF_MemBlkRef_Debug*/

TTF_MEM_ST *TTF_MemBlkAllocWithUsed_Debug
(
    VOS_UINT16 usFileID, VOS_UINT16 usLineNum, VOS_UINT32 ulPid,
    VOS_UINT32 ulPoolId, VOS_UINT16 usLen
)
{
    TTF_MEM_ST     *pMem;

    pMem    = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, ulPid, ulPoolId, usLen);

    if (VOS_NULL_PTR == pMem)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "Error: TTF_MemBlkAlloc_Debug Fail\n");
        return VOS_NULL_PTR;
    }

    pMem->usUsed    = usLen;

    return pMem;
} /* TTF_MemBlkAllocWithUsed_Debug */

VOS_UINT32 TTF_MemSet(VOS_UINT32 ulPid, TTF_MEM_ST *pMemDest, VOS_UINT16 usOffset,
    VOS_UINT8 *pSrc, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem        = pMemDest;
    VOS_UINT16  usCurrOffset    = usOffset;
    VOS_UINT16  usCopyLen       = 0;
    VOS_UINT16  usWorkedLen     = 0;
    VOS_UINT16  usMemDestLen    = 0;

    /*====================*/ /* 参数检查 */
    if ( (VOS_NULL_PTR == pMemDest)||(VOS_NULL_PTR == pSrc) )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par pMemDest Or pSrc is Null!\n");
        return PS_FAIL;
    }

    if ( 0 == usLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par usLen is 0!\n");
        return PS_FAIL;
    }

    /*====================*/ /*判断TTF内存块的长度是否符合要求*/
    while(VOS_NULL_PTR != pMemDest)
    {
        usMemDestLen   += pMemDest->usLen;
        pMemDest        = pMemDest->pNext;
    }

    if ( usMemDestLen < (usOffset + usLen) )
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning:MemDestLen <1> Less Than (Offset + Len) <2>!\n",
            usMemDestLen, (usOffset + usLen));
        return PS_FAIL;
    }

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usLen <= usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usLen;
            pCurrMem        = pCurrMem->pNext;
        }
        else
        {
            break;
        }
    }


    /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
    PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

    /*================*/ /* 第一块要进行偏移，特殊处理 */
    usCopyLen   = PS_MIN(pCurrMem->usLen - usCurrOffset, usLen);
    DRV_RT_MEMCPY(pCurrMem->pData + usCurrOffset, pSrc, usCopyLen);
    usWorkedLen = usCopyLen;


    /*================*/ /*  后续块可以从头开始处理，无需偏移 */
    pCurrMem        = pCurrMem->pNext;

    while (usWorkedLen < usLen)
    {
        /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
        PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

        usCopyLen   = PS_MIN(pCurrMem->usLen, (usLen - usWorkedLen));
        DRV_RT_MEMCPY(pCurrMem->pData, pSrc + usWorkedLen, usCopyLen);
        usWorkedLen    += usCopyLen;
        pCurrMem        = pCurrMem->pNext;
    }

    return PS_SUCC;
} /* TTF_MemSet */


/*****************************************************************************
 Prototype       : TTF_MemGet
 Description     : 从一TTF内存块获取一定长度的数据到指定目标地址
 Input           : ulPid    -- 调用此函数的模块的Pid
                   pMemSrc  -- 要获取数据的TTF内存块头指针
                   usOffset -- 内存块数据偏移
                   pDest    -- 存放获取数据的目标地址
                   usLen    -- 获取的数据长度，单位为字节
 Output          : NONE
 Return Value    : PS_SUCC   -- 成功
                   PS_FAIL  -- 失败
                   TTF_CAUSE_INPUT_PARAMS -- 输入参数错误

 History         :
    Date         : 2005-04-19
    Author       : w29695
    Modification:  2006-12-04,查鸣峰(44034)，实现该函数
*****************************************************************************/
VOS_UINT32 TTF_MemGet(VOS_UINT32 ulPid, TTF_MEM_ST *pMemSrc, VOS_UINT16 usOffset,
    VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem        = pMemSrc;
    VOS_UINT16  usCurrOffset    = usOffset;
    VOS_UINT16  usCopyLen       = 0;
    VOS_UINT16  usWorkedLen     = 0;
    VOS_UINT16  usMemSrcLen;

    /*====================*/ /* 参数检查 */
    if ( (VOS_NULL_PTR == pMemSrc)||(VOS_NULL_PTR == pDest) )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par pMemSrc Or pDest is Null!\n");
        return PS_FAIL;
    }

    if ( 0 == usLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par usLen is 0!\n");
        return PS_FAIL;
    }

    /*====================*/ /*判断TTF内存块的长度是否符合要求*/
    usMemSrcLen = TTF_MemGetLen(ulPid, pMemSrc);

    if ( usMemSrcLen < (usOffset + usLen) )
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning:MemSrcLen <1> Less Than (Offset + Len) <2>!\n",
            usMemSrcLen, (usOffset + usLen));
        return PS_FAIL;
    }

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usUsed <= usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pCurrMem        = pCurrMem->pNext;
        }
        else
        {
            break;
        }
    }

    /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
    PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

    /*================*/ /* 第一块要进行偏移，特殊处理 */
    usCopyLen   = PS_MIN(pCurrMem->usUsed - usCurrOffset, usLen);
    DRV_RT_MEMCPY(pDest, pCurrMem->pData + usCurrOffset, usCopyLen);
    usWorkedLen = usCopyLen;


    /*================*/ /*  后续块可以从头开始处理，无需偏移 */
    pCurrMem        = pCurrMem->pNext;

    while (usWorkedLen < usLen)
    {
        /*经过以上参数和内存块长度的检查，pCurrMem不应该为NULL*/
        PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

        usCopyLen   = PS_MIN(pCurrMem->usUsed, usLen - usWorkedLen);
        DRV_RT_MEMCPY(pDest + usWorkedLen, pCurrMem->pData, usCopyLen);
        usWorkedLen    += usCopyLen;
        pCurrMem        = pCurrMem->pNext;
    }

    return PS_SUCC;
} /* TTF_MemGet */


/*****************************************************************************
 Prototype       : TTF_MemLink
 Description     : 将一个TTF内存块添加到另一个TTF内存块后面, 被添加的内存块可以为空
                   注意:该函数实现功能同TTF_MemStick类似，主要用于Maps3000开发
 Input           : ulPid    -- 调用此函数的模块的Pid
                   pMemHead -- 要粘贴的TTF内存块头指针
                   pMemTail -- 粘贴到头部后的TTF内存块尾指针
 Output          : pMemHead -- 粘贴后的TTF内存块头指针
 Return Value    : PS_SUCC   -- 成功
                   PS_FAIL  -- 失败
                   TTF_CAUSE_INPUT_PARAMS -- 输入参数错误

 History         :
    Date         : 2006-12-04
    Author       : 查鸣峰(44034)
*****************************************************************************/
VOS_UINT32 TTF_MemLink(VOS_UINT32 ulPid, TTF_MEM_ST **ppMemHead, TTF_MEM_ST *pMemTail)
{
    TTF_MEM_ST *pTmp = VOS_NULL_PTR;

    /*====================*/ /* 参数检查 */
    if ((VOS_NULL_PTR == pMemTail)||(VOS_NULL_PTR == ppMemHead))
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning:Input Par pMemTail Or ppMemHead is Null!\n");
        return PS_FAIL;
    }


    if (VOS_NULL_PTR == *ppMemHead)
    {
        *ppMemHead = pMemTail;
        return PS_SUCC;
    }

    pTmp = *ppMemHead;

    /*====================*/ /* 找到此TTF内存块的最后一个节点 */
    while (VOS_NULL_PTR != pTmp->pNext)
    {
        pTmp = pTmp->pNext;
    }

    pTmp->pNext = pMemTail;  /* 将pMemTail链接上去 */

    return PS_SUCC;
} /* TTF_MemLink */


/*****************************************************************************
 函 数 名  : TTF_MemGetHeadData
 功能描述  : 从TTF_MEM块头部获取数据，但数据内容不从块中删除
 输入参数  : ulPid    -- 调用此函数的模块的Pid
             pMemSrc  -- 要填写的TTF内存块头指针
             pDest    -- 目的数据地址
             usLen    -- 获取数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : 获取数据成功返回PS_SUCC；失败返回PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TTF_MemGetHeadData( VOS_UINT32 ulPid, TTF_MEM_ST *pMemSrc,
    VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem        = pMemSrc;
    VOS_UINT16  usCopyLen       = 0;
    VOS_UINT16  usWorkedLen     = 0;
    VOS_UINT16  usMemSrcLen;

    /*====================*/ /* 参数检查 */
    if ( (VOS_NULL_PTR == pMemSrc) || (VOS_NULL_PTR == pDest) )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par pMemSrc Or pDest is Null!\n");
        return PS_FAIL;
    }

    if ( 0 == usLen )
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: Input Par usLen is 0!\n");
        return PS_FAIL;
    }

    /*====================*/ /*判断TTF内存块的长度是否符合要求*/
    usMemSrcLen = TTF_MemGetLen(ulPid, pMemSrc);

    if ( usMemSrcLen < usLen )
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning: MemSrcLen <1> Less Than usLen <2>!\n",
            usMemSrcLen, usLen);
        return PS_FAIL;
    }

    /*====================*/ /*获取所需长度的数据*/
    while (VOS_NULL_PTR != pCurrMem)
    {
        usCopyLen       = PS_MIN(pCurrMem->usUsed, (usLen - usWorkedLen));
        DRV_RT_MEMCPY(pDest+ usWorkedLen, pCurrMem->pData, usCopyLen);
        usWorkedLen    += usCopyLen;

        if ( usWorkedLen == usLen )
        {
            return PS_SUCC;
        }
        else
        {
            pCurrMem        = pCurrMem->pNext;
        }
    }

    PS_LOG(ulPid, 0, PS_PRINT_WARNING, "Warning: pCurrMem is Null!\n");
    return PS_FAIL;
} /* TTF_MemGetHeadData */

/*****************************************************************************
 函 数 名  : TTF_MemGetTailData
 功能描述  : 从TTF_MEM块尾部获取数据，但数据内容不从块中删除
 输入参数  : ulPid    -- 调用此函数的模块的Pid
             pMemSrc  -- 要填写的TTF内存块头指针
             pDest    -- 目的数据地址
             usLen    -- 获取数据的长度，单位为字节
 输出参数  : 无
 返 回 值  : 获取数据成功返回PS_SUCC；失败返回PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2006年12月8日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TTF_MemGetTailData( VOS_UINT32 ulPid, TTF_MEM_ST *pMemSrc,
    VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    TTF_MEM_ST *pCurrMem;
    VOS_UINT16  usCurrLen;
    VOS_UINT16  usCurrOffset;
    VOS_UINT16  usWorkedLen;


    /*====================*/    /* 参数检查 */
    PS_ASSERT_RTN((VOS_NULL_PTR != pMemSrc), PS_FAIL);
    PS_ASSERT_RTN((VOS_NULL_PTR != pDest), PS_FAIL);
    PS_ASSERT_RTN((0 != usLen), PS_FAIL);


    pCurrMem    = pMemSrc;

    /*====================*/    /* 寻找到需要的TTF_MEM_ST */
    usCurrLen   = TTF_MemGetLen(ulPid, pCurrMem);

    if (usCurrLen < usLen)
    {
        PS_LOG2(ulPid, 0, PS_PRINT_WARNING, "Warning: MemSrcLen <1> Less Than usLen <2>!\n",
            usCurrLen, usLen);
        return PS_FAIL;
    }


    /* 为提高寻找效率，把对尾部的数据位置需求，转换成从头部开始找位置 */
    usCurrOffset  = usCurrLen - usLen;

    /*================*/ /* 寻找到偏移后的第一个TTF_MEM */
    while (VOS_NULL_PTR != pCurrMem)
    {
        /*============*/ /* 要计算的数据不在该块内，到下一块进行计算 */
        if (pCurrMem->usUsed <= usCurrOffset)
        {
            usCurrOffset   -= pCurrMem->usUsed;
            pCurrMem        = pCurrMem->pNext;
        }
        else
        {
            break;
        }
    }


    /* 经过前面的运算，pCurrMem肯定不为空 */
    PS_ASSERT_RTN((VOS_NULL_PTR != pCurrMem), PS_FAIL);

    usWorkedLen     = 0;
    /*================*/ /* 第一块需要进行偏移，特殊处理 */
    if (0 != usCurrOffset)
    {
        usWorkedLen         = pCurrMem->usUsed - usCurrOffset;
        DRV_RT_MEMCPY(pDest, &pCurrMem->pData[usCurrOffset], usWorkedLen);
        pCurrMem            = pCurrMem->pNext;
    }

    /* 后面的块可以直接拷贝数据*/
    while (VOS_NULL_PTR != pCurrMem)
    {
        DRV_RT_MEMCPY(pDest + usWorkedLen, pCurrMem->pData, pCurrMem->usUsed);
        usWorkedLen    += pCurrMem->usUsed;
        pCurrMem        = pCurrMem->pNext;
    }

    return PS_SUCC;
} /* TTF_MemGetTailData */

/*****************************************************************************
 Prototype       : TTF_MemStick
 Description     : GPRS专用，W不使用,将一个TTF内存块添加到另一个TTF内存块后面
 Input           : ulPid    -- 调用此函数的模块的Pid
                   pMemHead -- 要粘贴的TTF内存块头指针
                   pMemTail -- 粘贴到头部后的TTF内存块尾指针
 Output          : pMemHead -- 粘贴后的TTF内存块头指针
 Return Value    : PS_SUCC   -- 成功
                   PS_FAIL  -- 失败
                   TTF_CAUSE_INPUT_PARAMS -- 输入参数错误

 History         :
    Date         : 2005-04-19
    Author       : 汪波(29695)
    Modification:  2006-12-04,查鸣峰(44034)，实现该函数
*****************************************************************************/
VOS_UINT32 TTF_MemStick(VOS_UINT32 ulPid, TTF_MEM_ST *pMemHead, TTF_MEM_ST *pMemTail)
{
    TTF_MEM_ST *pTmp = VOS_NULL_PTR;


    /*====================*/ /* 参数检查 */
    if ((VOS_NULL_PTR == pMemHead) || (VOS_NULL_PTR == pMemTail))
    {
        return PS_FAIL;
    }

    pTmp = pMemHead;

    /*====================*/ /* 找到此TTF内存块的最后一个节点 */
    while (VOS_NULL_PTR != pTmp->pNext)
    {
        pTmp = pTmp->pNext;
    }

    pTmp->pNext = pMemTail;  /* 将pMemTail链接上去 */

    return PS_SUCC;
} /* TTF_MemStick */


VOS_UINT32 TTF_MemInsertNext(VOS_UINT32 ulPid, TTF_MEM_ST *pMemCurr, TTF_MEM_ST *pMemNext)
{
    if (VOS_NULL_PTR == pMemCurr || VOS_NULL_PTR == pMemNext)
    {
        return PS_FAIL;
    }

    pMemNext->pNext = pMemCurr->pNext;
    pMemCurr->pNext = pMemNext;

    return PS_SUCC;
}


VOS_UINT32 TTF_MemIsCritical(VOS_VOID)
{
    /*暂时打桩该函数，始终返回VOS_NO*/

    return VOS_NO;
}
VOS_UINT32 TTF_MemRegEventCallBack
(
    TTF_MEM_POOL_ID_ENUM_UINT32         enPoolId,
    TTF_MEM_EVENT_CALLBACK              pMemAllocEvent,
    TTF_MEM_EVENT_CALLBACK              pMemFreeEvent
)
{
    TTF_MEM_POOL_STRU              *pstTtfMemPool;

    if (enPoolId >= TTF_MEM_POOL_ID_BUTT)
    {
        PS_LOG1(WUEPS_PID_RLC, 0, PS_PRINT_ERROR, "TTF_MemRegEventCallBack Fail, PoolId %d exceed the Ranger\r\n", (VOS_INT32)enPoolId);
        return  PS_FAIL;
    }

    pstTtfMemPool   = TTF_MEM_GET_POOL(enPoolId);

    pstTtfMemPool->pMemAllocEvent   = pMemAllocEvent;
    pstTtfMemPool->pMemFreeEvent    = pMemFreeEvent;

    return PS_SUCC;
} /* TTF_MemRegEventCallBack */

VOS_UINT32 TTF_MemRegExtFreeCallBack
(
    TTF_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc
)
{
    TTF_MEM_POOL_STRU                   *pstTtfMemPool;


    if ( VOS_NULL_PTR == pMemExtFreeFunc )
    {
        return PS_FAIL;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(TTF_MEM_POOL_ID_EXT);
    pstTtfMemPool->pMemExtFreeFunc = pMemExtFreeFunc;

    return PS_SUCC;
} /* TTF_MemRegExtFreeCallBack */


/*****************************************************************************
 函 数 名  : TTF_SetBlkMemFlag
 功能描述  :
 输入参数  : VOS_INT8 *cFileName
             VOS_UINT32 ulLineNum
             VOS_UINT32 ulPid
             VOS_UINT16 usLen
 输出参数  : 无
 返 回 值  : TTF_MEM_ST *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年3月24日
    作    者   : 查鸣峰
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID TTF_MemSetBlkFlag(TTF_MEM_ST *pstMem, VOS_UINT16 usFileID, VOS_UINT16 usLineNum)
{
    while (VOS_NULL_PTR != pstMem)
    {
        if (VOS_NULL_PTR != pstMem->pstDbgInfo)
        {
            pstMem->pstDbgInfo->usTraceFileId   = usFileID;
            pstMem->pstDbgInfo->usTraceLineNum  = usLineNum;
            pstMem->pstDbgInfo->ulTraceTick     = OM_GetSlice();

            pstMem  = pstMem->pNext;
        }
    }
} /* TTF_MemSetBlkFlag */


VOS_VOID TTF_MemFreeData(VOS_UINT32 ulPid, TTF_MEM_ST *pItem)
{
    if ( VOS_NULL_PTR != pItem )
    {
        TTF_MemFree(ulPid, pItem);
    }

    return;

}


/*lint -esym( 528, TTF_MemGetLen )*/
VOS_UINT16 TTF_MemGetLen(VOS_UINT32 ulPid, TTF_MEM_ST *pMem)
{
    VOS_UINT16      usLen   = 0;
    TTF_MEM_ST     *pTmp    = pMem;

    while(VOS_NULL_PTR != pTmp)
    {
        usLen   += pTmp->usUsed;
        pTmp     = pTmp->pNext;
    }

    return usLen;
} /*TTF_MemGetLen*/



VOS_VOID TTF_MemGetAllocFailCnt(VOS_VOID)
{
    VOS_UINT16                          usLoop;


    vos_printf("TTF_MemGetAllocFailCnt Start: \n");
    vos_printf("TTF Mem Alloc Fail Threshold: %d \n", g_usTtfMemAllocFailCntThreshold);

    for( usLoop = 0; usLoop < TTF_MEM_POOL_ID_BUTT; ++usLoop )
    {
        vos_printf("TTF Mem Pool ID: %d \n", usLoop);
        vos_printf("TTF Mem Alloc Fail Cnt: %d * %d + %d \n",
                    *g_apusTtfMemExcThresholdCnt[usLoop], g_usTtfMemAllocFailCntThreshold, *g_apusTtfMemAllocFailCnt[usLoop]);
    }

    vos_printf("TTF_MemGetAllocFailCnt End. \n");

    return;
}/* TTF_MemGetAllocFailCnt */

VOS_VOID TTF_MemSetAllocFailThreshold(VOS_UINT16 usThreshold)
{
    g_usTtfMemAllocFailCntThreshold = usThreshold;

    return;
}/* TTF_MemSetAllocFailThreshold */

VOS_VOID TTF_MemReSetAllocFailStatus(VOS_VOID)
{
    VOS_UINT16                          usLoop;


    for( usLoop = 0; usLoop < TTF_MEM_POOL_ID_BUTT; ++usLoop )
    {
        *g_apusTtfMemExcThresholdCnt[usLoop]  = 0;
        *g_apusTtfMemAllocFailCnt[usLoop]     = 0;
    }

    return;
}/* TTF_MemReSetAllocFailStatus */

VOS_VOID TTF_MemHelp( VOS_VOID )
{
    vos_printf("********************TTF_MEM软调信息************************\n");
    vos_printf("TTF_MemBlkCheckPoolLeak(ulPoolId):           检查TTF_MEM内存泄露，其中ulPoolId:\n");
    vos_printf("                                             0--DL_SHARE; 1--DL_PDU; 2--UL_DATA; 3--EXT; 4--UL_IP_DATA\n");
    vos_printf("TTF_ShowUsedBlkMemInfo(VOS_VOID)             查看内存的使用信息(g_ucTtfMemDebugPrint =1时开启信息打印)。\n");
    vos_printf("TTF_MemSetTraceFunc(ucChoice)                配置函数跟踪轨迹打印，ucChoice = 0 关闭轨迹打印。\n");
    vos_printf("TTF_MemGetAllocFailCnt(VOS_VOID)             获取内存申请失败次数信息。\n");
    vos_printf("TTF_MemSetAllocFailThreshold(usThreshold)    配置内存申请失败上报门限。\n");
    vos_printf("TTF_MemGetAllocFailCnt(VOS_VOID)             获取内存申请失败统计信息。\n");
    vos_printf("TTF_MemReSetAllocFailStatus(VOS_VOID)        重置内存申请失败统计信息。\n");
    vos_printf("TTF_MemShowMntnInfo( VOS_VOID )              获取C核内存回收释放可维可测信息。\n");
    vos_printf("TTF_RbMemPrintIsrCnt                         C核收到IPF中断次数.\n");
    vos_printf("TTF_MemRcovShow                              C核TTF Mem内存泄露保护状态.\n");

    return;
}/* TTF_MemHelp */

VOS_VOID TTF_MemRcovShow(VOS_VOID)
{
    VOS_UINT32 ulPoolId;

    vos_printf("当前保护内存池掩码:             0x%x\r\n", g_stTtfMemRcovEnt.ucRcovPoolMsk);
    vos_printf("当前系统时间(Slice):            %u\r\n", VOS_GetSlice());

    for (ulPoolId = 0; ulPoolId < TTF_MEM_POOL_ID_BUTT; ulPoolId++)
    {
        vos_printf("内存池:%d 的状态:                       %u\r\n", ulPoolId, g_stTtfMemRcovEnt.astPoolInfo[ulPoolId].ulState);
        vos_printf("内存池:%d 恢复时长(Slice):              %u\r\n", ulPoolId, g_stTtfMemRcovEnt.astPoolInfo[ulPoolId].ulRcovTimerLen);
        vos_printf("内存池:%d 进入恢复状态门限:             %u\r\n", ulPoolId, g_stTtfMemRcovEnt.astPoolInfo[ulPoolId].ulRcovThres);
        vos_printf("内存池:%d 进入恢复状态的时戳(Slice):    %u\r\n", ulPoolId, g_stTtfMemRcovEnt.astPoolInfo[ulPoolId].ulEnterRcovTs);
    }
}


VOS_VOID TTF_MemRcovEntInit(VOS_VOID)
{
    VOS_UINT32 ulPoolId;

    TTF_MEM_RST_RCOV_ENT();

    TTF_MEM_SET_RCOV_ENT_POOL_MSK(TTF_MEM_RCOV_DEF_POOL_MSK);

    for (ulPoolId = 0; ulPoolId < TTF_MEM_POOL_ID_BUTT; ulPoolId++)
    {
        TTF_MEM_SET_RCOV_ENT_THRES(ulPoolId, TTF_MEM_RCOV_THRES);
        TTF_MEM_SET_RCOV_ENT_TIMER_LEN(ulPoolId, TTF_MEM_RCOV_TIMER_LEN);
        TTF_MEM_SET_RCOV_ENT_STATE(ulPoolId,  TTF_MEM_STATE_NORMAL);
    }
}



VOS_VOID TTF_MemRcovAllocFail(VOS_UINT32 ulPoolId, VOS_UINT32 ulFailCnt, VOS_UINT32 ulPid, VOS_UINT16 usFileId, VOS_UINT16 usLine)
{
    VOS_UINT32  ulCurrTs;
    VOS_UINT32  ulTimerLen;
    VOS_UINT32  ulTimerStart;
    VOS_UINT32  ulPoolMsk;

    ulPoolMsk = TTF_MEM_MASK_POOL(ulPoolId);

    if (!TTF_MEM_CHK_NEED_RCOV_POOL(ulPoolMsk))
    {
        TTF_MEM_SET_RCOV_ENT_STATE(ulPoolId,  TTF_MEM_STATE_ALLOC_FAIL);

        return;
    }

    /* 已经在等待恢复状态，判断是否已经超时 */
    if (TTF_MEM_STATE_WAIT_RCOV == TTF_MEM_GET_RCOV_ENT_STATE(ulPoolId))
    {
        ulCurrTs        = VOS_GetSlice();
        ulTimerLen      = TTF_MEM_GET_RCOV_ENT_TIMER_LEN(ulPoolId);
        ulTimerStart    = TTF_MEM_GET_RCOV_ENT_TIMESTAMP(ulPoolId);

        /* 已经超时，复位单板 */
        if (TTF_MEM_RCOV_TIMER_EXPIRED(ulCurrTs, (ulTimerStart  + ulTimerLen)))
        {
            TTF_MEM_RCOV_MEM(ulPoolId, ulPid, usFileId, usLine);
        }
    }
    else
    {
        /* 超过门限，进入等待恢复状态 */
        if (ulFailCnt >= TTF_MEM_GET_RCOV_ENT_THRES(ulPoolId))
        {
            TTF_MEM_RCOV_ENTER_WAIT_RCOV_STATE(ulPoolId);
        }
        else
        {
            TTF_MEM_SET_RCOV_ENT_STATE(ulPoolId,  TTF_MEM_STATE_ALLOC_FAIL);
        }
    }
}

#if (FEATURE_ON == FEATURE_SKB_EXP) /* V3R3 版本 */

VOS_VOID TTF_MemSetDefaultNvCfg(TTF_MEM_SOLUTION_CFG_NV_STRU *pstTtfMemSolution)
{
    const   VOS_UINT8   ucPoolMask  = 0x0F;


    pstTtfMemSolution->ucPoolCnt    = TTF_MEM_MAX_POOL_NUM;
    pstTtfMemSolution->ucPoolMask   = ucPoolMask;

    /*DL SHARE*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ucClusterCnt  = 2;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 864;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 198;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 1952;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 158;

    /*DL PDU */
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ucClusterCnt  = 3;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 84;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 1024;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 700;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 950;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_2] = 1560;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_2]  = 585;

    /*UL DATA*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ucClusterCnt  = 4;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 84;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 5120;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 628;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 1700;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_2] = 1652;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_2]  = 300;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_3] = 3700;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_3]  = 10;

    /*UL EXT*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ucClusterCnt  = 1;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = TTF_MEM_CTRL_MEM_CNT;


    /* UL IP PACKET */
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ucClusterCnt  = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 0;

    return;
}
TTF_MEM_ST *TTF_MemBlkAlloc_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
     VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, VOS_UINT16 usLen)
{
    TTF_MEM_POOL_STRU              *pstTtfMemPool;
    TTF_MEM_CLUSTER_STRU           *pstTtfMemCluster;
    TTF_MEM_ST                     *pMem;
    VOS_INT32                       lLockKey;
    VOS_UINT8                       ucClusterId;
    VOS_UINT8                       ucMostFitLev;
    VOS_UINT16                      usDataLen;


    if ((ulPoolId >= TTF_MEM_POOL_ID_BUTT) || (TTF_MEM_POOL_ID_UL_IP_DATA == ulPoolId))
    {
        TTF_LOG1(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Poolid %d is invalid! \n",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if ( PS_TRUE != pstTtfMemPool->usUsedFlag )
    {
        TTF_LOG1(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Pool %d is not used! \n",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 申请长度是否在该内存内 */
    if ( TTF_MEM_POOL_ID_EXT == ulPoolId )
    {
        /* 如果是外部内存，则将长度置为零 */
        usLen = 0;
    }
    else
    {
        /* 非外部内存池，如果长度为零则打印告警 */
        if ( 0 == usLen )
        {
            TTF_LOG2(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug, Invalid usLen = %d for the Pool %d ! \n", usLen, (VOS_INT32)ulPoolId);
            return VOS_NULL_PTR;
        }
    }

    /* 如果为下行共享内存池，则做长度偏移 */
    if ( TTF_MEM_POOL_ID_DL_SHARE == ulPoolId )
    {
        /* 采用MBB的方式, 在申请内存时, 在头部和尾部保留pad */
        TTF_MEM_RSV_PAD_LEN(usDataLen, usLen);
    }

    /* 如果超过最大长度, 则打印告警 */
    if ( usLen > pstTtfMemPool->usMaxByteLen )
    {
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug, usLen = %d exceed the Pool %d ranger! \n", usLen, (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 从内存池的多个簇里寻找合适的TTF_MEM */
    ucMostFitLev = TTF_GET_LEN_INDEX(pstTtfMemPool, usLen);

    for(ucClusterId = ucMostFitLev;
        ucClusterId < pstTtfMemPool->ucClusterCnt;
        ucClusterId ++)
    {
        pstTtfMemCluster  = &(pstTtfMemPool->astClusterTable[ucClusterId]);

        lLockKey = VOS_SplIMP();

        if (0 != pstTtfMemCluster->usFreeCnt)
        {
            /* 获取一个没有使用的TTF_MEM_ST结点 */
            pMem    = pstTtfMemCluster->apstFreeStack[--pstTtfMemCluster->usFreeCnt];
            pstTtfMemPool->usTtfMemUsedCnt++;

            VOS_Splx(lLockKey);

            TTF_MEM_RESET(pMem);

            lLockKey = VOS_SplIMP();
            pstTtfMemPool->ulTtfMemUsedByteSize += pMem->usLen;
            VOS_Splx(lLockKey);

            if (VOS_NULL_PTR != pstTtfMemPool->pMemAllocEvent)
            {
                pstTtfMemPool->pMemAllocEvent(pstTtfMemPool->usTtfMemUsedCnt, pstTtfMemPool->ulTtfMemUsedByteSize);
            }

            TTF_MEM_DBG(pMem, usFileID, usLineNum);

            if (TTF_MEM_POOL_ID_DL_SHARE == ulPoolId)
            {
               pMem->pData  = pMem->pOrigData + TTF_MEM_SKB_HEAD_PAD_LEN + TTF_MEM_MAC_HEADER_RSV_LEN;
               pMem->usLen -= TTF_MEM_SKB_HEAD_PAD_LEN + TTF_MEM_MAC_HEADER_RSV_LEN;
            }

            (*g_apusTtfMemConAllocFailCnt[ulPoolId]) = 0;

            TTF_MEM_RCOV_ALLOC_SUCC(ulPoolId);

            return pMem;
        }

        VOS_Splx(lLockKey);
    }

    /* 第一次出现内存申请失败上报，后面每达到门限上报一次 */
    if ( 0 == ((*g_apusTtfMemAllocFailCnt[ulPoolId]) & g_usTtfMemAllocFailCntThreshold ))
    {
        (*g_apusTtfMemExcThresholdCnt[ulPoolId])++;

        /* 生成Errlog事件，failCnt是在后面维护的，所以这里要+1 */
        TTF_MNTN_ErrlogTtfMemAllocFail(ulPid, ulPoolId, usFileID, usLineNum, (*g_apusTtfMemAllocFailCnt[ulPoolId]) + 1);

        /* 上报内存池申请失败事件 */
        TTF_BlkMemUsedInfoEventRpt(ulPid, ulPoolId, TRIG_TYPE_ALLOC_FAIL);
    }

    (*g_apusTtfMemAllocFailCnt[ulPoolId])++;
    (*g_apusTtfMemConAllocFailCnt[ulPoolId])++;

    TTF_MEM_RCOV_ALLOC_FAIL(ulPoolId, (*g_apusTtfMemConAllocFailCnt[ulPoolId]), ulPid, usFileID, usLineNum);

    return VOS_NULL_PTR;
} /* TTF_MemBlkAlloc_Debug */

VOS_VOID TTF_ExtMemFree( void **ppstExtMem )
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;
    void                               *pstMem = VOS_NULL_PTR;

    TTF_DEBUG_TRACE_FUNC_ENTER();

    if ( VOS_NULL_PTR == ppstExtMem )
    {
        g_stTtfMemFreeMntnEntity.ulTtfMemExtFreeFailCnt++;
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_WARNING, "WARNING: TTF_RemoteFreeImmZc pstImmZc is NULL !\n");

        return;
    }

    if ( VOS_NULL_PTR == *ppstExtMem )
    {
        g_stTtfMemFreeMntnEntity.ulTtfMemExtFreeFailCnt++;
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_WARNING, "WARNING: TTF_RemoteFreeImmZc *pstImmZc is NULL !\n");

        return;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(TTF_MEM_POOL_ID_EXT);
    if ( VOS_NULL_PTR == pstTtfMemPool->pMemExtFreeFunc )
    {
        g_stTtfMemFreeMntnEntity.ulTtfMemExtFreeFailCnt++;
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_ERROR, "ERROR: TTF_RemoteFreeImmZc EXT free method is not registered !\n");

        return;
    }

    pstMem = *ppstExtMem;
    pstTtfMemPool->pMemExtFreeFunc(pstMem);
    (*ppstExtMem) = VOS_NULL_PTR;


    TTF_DEBUG_TRACE_FUNC_LEAVE();

    return;
}/* TTF_ExtMemFree */

TTF_MEM_ST * TTF_DataTransformTtfMem_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,VOS_UINT8* pucData,VOS_UINT16 usLen, VOS_VOID*  pstIMM)
{
    TTF_MEM_ST                         *pstMem = VOS_NULL_PTR;

    TTF_DEBUG_TRACE_FUNC_ENTER();

    if ( VOS_NULL_PTR == pstIMM )
    {
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_ERROR, "ERROR: TTF_DataTransformTtfMem_Debug Input Para pstIMMZc is NULL !\n");
        return VOS_NULL_PTR;
    }

    if ( VOS_NULL_PTR == pucData )
    {
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_ERROR, "ERROR: TTF_DataTransformTtfMem_Debug Input Para pucDatas is NULL !\n");
        TTF_RemoteFreeImmMem(pstIMM);
        return VOS_NULL_PTR;
    }

    pstMem = TTF_MemBlkAlloc_Debug(usFileID, usLineNum, WUEPS_PID_RLC, TTF_MEM_POOL_ID_EXT, 0);

    if ( VOS_NULL_PTR == pstMem )
    {
        TTF_LOG(WUEPS_PID_RLC, PS_PRINT_ERROR, "ERROR:TTF_DataTransformTtfMem_Debug TTF_MemBlkAlloc_Debug fail !\n");
        TTF_RemoteFreeImmMem(pstIMM);
        return VOS_NULL_PTR;
    }

    pstMem->pOrigData       = (VOS_UINT8*)(pucData - TTF_MEM_MAC_HEADER_RSV_LEN);
    pstMem->pData           = pucData;
    pstMem->usLen           = usLen + TTF_MEM_MAC_HEADER_RSV_LEN;
    pstMem->usUsed          = usLen;
    pstMem->pExtBuffAddr    = pstIMM;

    TTF_DEBUG_TRACE_FUNC_LEAVE();

    return pstMem;
} /* TTF_DataTransformTtfMem_Debug */

VOS_VOID  TTF_MemPoolInfoEventRpt(VOS_UINT8 ucPoolId)
{
    TTF_MEM_POOL_STRU                      *pstTtfMemPool;


    pstTtfMemPool = TTF_MEM_GET_POOL(ucPoolId);

    if ( PS_TRUE != pstTtfMemPool->usUsedFlag )
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemPoolInfoEventRpt, Pool %d is not used!\n", ucPoolId);
        return;
    }

    if ( pstTtfMemPool->usTtfMemUsedCnt> 0 )
    {
        TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR, "TTF_MEM_BLK Is Leak,Pool %d,UsedCnt %d \n!",
            ucPoolId, pstTtfMemPool->usTtfMemUsedCnt);

        TTF_BlkMemUsedInfoEventRpt(WUEPS_PID_RLC, ucPoolId, TRIG_TYPE_LEAK);
        TTF_MemReportMemLeakMsg(WUEPS_PID_RLC);
    }
    else
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_NORMAL, "NORMAL: TTF_MemPoolInfoEventRpt, Pool %d Normal\n!",ucPoolId);
        TTF_MemSndNormalEvent(ucPoolId) ;
    }

    return;
} /* TTF_MemPoolInfoEventRpt */

VOS_VOID TTF_MemCcpuCheckPoolLeak( VOS_VOID )
{
    VOS_UINT8                          ucPoolId;

    /* 释放核间队列的节点 */
    TTF_MemFreeFromQue();

    /* 释放CDS内存释放队列内存 */
    CDS_FreeIpfDlSrcMem();


    for(ucPoolId = TTF_MEM_POOL_ID_DL_SHARE; ucPoolId < TTF_MEM_POOL_ID_EXT; ++ucPoolId )
    {
        TTF_MemPoolInfoEventRpt(ucPoolId);
    }

    return;
}


VOS_UINT32 TTF_GetLocalFreeMemCnt(VOS_UINT8 ucPoolId)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;

    if ( ucPoolId >= TTF_MEM_POOL_ID_UL_IP_DATA )
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_ERROR, "TTF_GetLocalFreeMemCnt PoolId fail %d \n", ucPoolId);
        return PS_FAIL;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(ucPoolId);

    return pstTtfMemPool->astClusterTable[pstTtfMemPool->ucClusterCnt - 1].usFreeCnt;
} /* TTF_GetLocalFreeMemCnt */

VOS_UINT32 TTF_GetDlIpFreeMemCnt (VOS_VOID)
{
    return TTF_GetLocalFreeMemCnt(TTF_MEM_POOL_ID_DL_SHARE);
}



VOS_UINT32 TTF_GetACoreReservedCnt (VOS_VOID)
{
    /*lint --e(778)*/
    return (*(VOS_UINT32 *)(TTF_MEM_ACPU_FREE_MEM_CNT_ADDR));

} /* TTF_GetACoreReservedCnt */



VOS_UINT32 TTF_GetUlIpFreeMemCnt (VOS_VOID)
{
    return TTF_GetACoreReservedCnt();
}


VOS_VOID TTF_MemFreeShowMntnInfo( VOS_VOID )
{
    TTF_MEM_FREE_INVALIED_MEM_INFO_STRU    *pstInvalidMemInfo   = &g_stTtfMemFreeMntnEntity.astTtfMemFreeInvalidMemInfo;
    TTF_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPtrInfo      = &g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPtrInfo;
    TTF_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPPtrInfo     = &g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPPtrInfo;

    vos_printf("TTF MEM FREE 可维可测信息 :\n");
    vos_printf("===========================================\n");
    vos_printf("TTF MEM FREE 非法内存信息 :\n");
    vos_printf("TTF MEM FREE 收到的非法内存个数       : %d \n", (VOS_INT32)pstInvalidMemInfo->ulInvalidTtfMemCnt);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存地址     : 0x%u \n", (VOS_INT32)pstInvalidMemInfo->ulInvalidTtfMemAddr);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存文件ID   : %d \n", pstInvalidMemInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存行号信息 : %d \n", pstInvalidMemInfo->usTtfMemFreeLineNum);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存类型     : %d \n", pstInvalidMemInfo->ucInvalidTtfMemType);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存值       : %d \n", pstInvalidMemInfo->usInvalidTtfMemValue);

    vos_printf("===========================================\n");
    vos_printf("TTF MEM FREE 空指针信息 :\n");
    vos_printf("TTF MEM FREE 一级指针为空的次数       : %d \n", (VOS_INT32)pstNullPtrInfo->ulTtfMemFreeNullPtrCnt);
    vos_printf("TTF MEM FREE 输入指针文件ID           : %d \n", pstNullPtrInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 输入指针内存行号信息     : %d \n", pstNullPtrInfo->usTtfMemFreeLineNum);

    vos_printf("TTF MEM FREE 二级指针为空的次数       : %d \n", (VOS_INT32)pstNullPPtrInfo->ulTtfMemFreeNullPtrCnt);
    vos_printf("TTF MEM FREE 输入指针内存文件ID       : %d \n", pstNullPPtrInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 输入指针内存行号信息     : %d \n", pstNullPPtrInfo->usTtfMemFreeLineNum);

    vos_printf("TTF MEM Pool Status:\n");
    vos_printf("          TTF_MEM_POOL_ID_DL_SHARE    : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_DL_SHARE].usUsedFlag);
    vos_printf("          TTF_MEM_POOL_ID_DL_PDU      : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_DL_PDU].usUsedFlag);
    vos_printf("          TTF_MEM_POOL_ID_UL_DATA     : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_UL_DATA].usUsedFlag);
    vos_printf("          TTF_MEM_POOL_ID_EXT         : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_EXT].usUsedFlag);

    vos_printf("TTF_MEM_POOL_ID_EXT 释放函数          : 0x%u\n", (VOS_INT32)g_astTtfMemPool[TTF_MEM_POOL_ID_EXT].pMemExtFreeFunc);
    vos_printf("TTF Ext MEM FREE 失败次数             : %d \n", (VOS_INT32)g_stTtfMemFreeMntnEntity.ulTtfMemExtFreeFailCnt);

}


/******************************************************************************/
/*******************************V9R1版本***************************************/
/******************************************************************************/
#else
VOS_VOID TTF_MemSetDefaultNvCfg(TTF_MEM_SOLUTION_CFG_NV_STRU *pstTtfMemSolution)
{
    const   VOS_UINT8   ucPoolMask  = 0x16;

    pstTtfMemSolution->ucPoolCnt    = TTF_MEM_MAX_POOL_NUM;
    pstTtfMemSolution->ucPoolMask   = ucPoolMask;

    /*DL SHARE*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ucClusterCnt  = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_SHARE].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 0;

    /*DL PDU */
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ucClusterCnt  = 4;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 84;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 2304;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 700;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 824;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_2] = 1560;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_2]  = 1860;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_3] = 1628;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_DL_PDU].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_3]  = 20;

    /*UL DATA*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ucClusterCnt  = 4;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 164;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 5430;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 628;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 1748;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_2] = 1652;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_2]  = 300;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_3] = 3700;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_3]  = 10;

    /*UL EXT*/
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ucClusterCnt  = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 0;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_EXT].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 0;


    /* UL IP PACKET */
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ucClusterCnt  = 3;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_0] = 404;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_0]  = 1000;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_1] = 1560;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_1]  = 1000;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkSize[TTF_MEM_POOL_BLK_INDEX_2] = 1628;
    pstTtfMemSolution->astTtfMemPoolCfgInfo[TTF_MEM_POOL_ID_UL_IP_DATA].ausBlkCnt[TTF_MEM_POOL_BLK_INDEX_2]  = 20;


    return;
}
TTF_MEM_ST *TTF_MemBlkAlloc_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
     VOS_UINT32 ulPid, VOS_UINT32 ulPoolId, VOS_UINT16 usLen)
{
    TTF_MEM_POOL_STRU              *pstTtfMemPool;
    TTF_MEM_CLUSTER_STRU           *pstTtfMemCluster;
    TTF_MEM_ST                     *pMem;
    VOS_INT32                       lLockKey;
    VOS_UINT8                       ucClusterId;
    VOS_UINT8                       ucMostFitLev;


    /* V9R1版本上只有TTF_MEM_POOL_ID_DL_PDU，TTF_MEM_POOL_ID_UL_DATA ，
    TTF_MEM_POOL_ID_UL_IP_DATA三个内存池生效 */
    if ( (ulPoolId >= TTF_MEM_POOL_ID_BUTT)
        || (TTF_MEM_POOL_ID_EXT == ulPoolId)
        || (TTF_MEM_POOL_ID_DL_SHARE == ulPoolId) )
    {
        TTF_LOG1(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Poolid %d is invalid! \n",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if ( PS_TRUE != pstTtfMemPool->usUsedFlag )
    {
        TTF_LOG1(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug,Pool %d is not used! \n",
            (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 内存池，如果长度为零则打印告警 */
    if ( 0 == usLen )
    {
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug, Invalid usLen = %d for the Pool %d ! \n", usLen, (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 如果超过最大长度, 则打印告警 */
    if ( usLen > pstTtfMemPool->usMaxByteLen )
    {
        TTF_LOG2(ulPid, PS_PRINT_WARNING, "TTF_MemBlkAlloc_Debug, usLen = %d exceed the Pool %d ranger! \n", usLen, (VOS_INT32)ulPoolId);
        return VOS_NULL_PTR;
    }

    /* 从内存池的多个簇里寻找合适的TTF_MEM */
    ucMostFitLev = TTF_GET_LEN_INDEX(pstTtfMemPool, usLen);

    for(ucClusterId = ucMostFitLev;
        ucClusterId < pstTtfMemPool->ucClusterCnt;
        ucClusterId ++)
    {
        pstTtfMemCluster  = &(pstTtfMemPool->astClusterTable[ucClusterId]);

        lLockKey = VOS_SplIMP();

        if (0 != pstTtfMemCluster->usFreeCnt)
        {
            /* 获取一个没有使用的TTF_MEM_ST结点 */
            pMem    = pstTtfMemCluster->apstFreeStack[--pstTtfMemCluster->usFreeCnt];
            pstTtfMemPool->usTtfMemUsedCnt++;

            VOS_Splx(lLockKey);

            TTF_MEM_RESET(pMem);

            lLockKey = VOS_SplIMP();
            pstTtfMemPool->ulTtfMemUsedByteSize += pMem->usLen;
            VOS_Splx(lLockKey);


            if (VOS_NULL_PTR != pstTtfMemPool->pMemAllocEvent)
            {
                pstTtfMemPool->pMemAllocEvent(pstTtfMemPool->usTtfMemUsedCnt, pstTtfMemPool->ulTtfMemUsedByteSize);
            }

            TTF_MEM_DBG(pMem, usFileID, usLineNum);

           (*g_apusTtfMemConAllocFailCnt[ulPoolId]) = 0;

            TTF_MEM_RCOV_ALLOC_SUCC(ulPoolId);

           return pMem;
        }

        VOS_Splx(lLockKey);
    }

    /* 第一次出现内存申请失败上报，后面每达到门限上报一次 */
    if (0 == ((*g_apusTtfMemAllocFailCnt[ulPoolId]) & g_usTtfMemAllocFailCntThreshold ))
    {
        (*g_apusTtfMemExcThresholdCnt[ulPoolId])++;

        /* 生成Errlog事件，failCnt是在后面维护的，所以这里要+1 */
        TTF_MNTN_ErrlogTtfMemAllocFail(ulPid, (VOS_UINT8)ulPoolId, usFileID, usLineNum, (*g_apusTtfMemAllocFailCnt[ulPoolId]) + 1);

        /* 上报内存池申请失败事件 */
        TTF_BlkMemUsedInfoEventRpt(ulPid, ulPoolId, TRIG_TYPE_ALLOC_FAIL);
    }

    (*g_apusTtfMemAllocFailCnt[ulPoolId])++;
    (*g_apusTtfMemConAllocFailCnt[ulPoolId])++;

    TTF_MEM_RCOV_ALLOC_FAIL(ulPoolId, (*g_apusTtfMemConAllocFailCnt[ulPoolId]), ulPid, usFileID, usLineNum);

    return VOS_NULL_PTR;
} /* TTF_MemBlkAlloc_Debug */

VOS_VOID TTF_ExtMemFree( void **ppstExtMem )
{
    TTF_LOG(WUEPS_PID_RLC, PS_PRINT_ERROR, "call TTF_ExtMemFree is invalid! \n");


    return;

}/* TTF_ExtMemFree */

TTF_MEM_ST * TTF_DataTransformTtfMem_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,VOS_UINT8* pucData,VOS_UINT16 usLen, VOS_VOID*  pstIMM)
{
    TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR,
        "call TTF_DataTransformTtfMem_Debug is invalid, File:%d, Line:%d! \n", usFileID, usLineNum);


    return VOS_NULL_PTR;
} /* TTF_DataTransformTtfMem_Debug */

VOS_UINT32 TTF_GetLocalFreeMemCnt (VOS_UINT32 ulPoolId)
{
    TTF_MEM_POOL_STRU                  *pstTtfMemPool;


    if (ulPoolId >= TTF_MEM_POOL_ID_BUTT)
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_GetCCoreReservedCnt,Poolid %d is invalid! \n",
            (VOS_INT32)ulPoolId);
        return PS_FAIL;
    }

    if ((TTF_MEM_POOL_ID_DL_SHARE == ulPoolId) || (TTF_MEM_POOL_ID_EXT == ulPoolId))
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_GetCCoreReservedCnt,Poolid %d is invalid! \n",
            (VOS_INT32)ulPoolId);
        return PS_FAIL;
    }

    /* 获取内存池 */
    pstTtfMemPool = TTF_MEM_GET_POOL(ulPoolId);

    if ((TTF_MEM_POOL_ID_UL_IP_DATA == ulPoolId) || (TTF_MEM_POOL_ID_DL_PDU == ulPoolId))
    {
        /* V9R1版本上对于上行IP包数据包内存池或者C-CPU下行PDU内存池，返回该内存池第二个档位剩余内存块数 */
        return pstTtfMemPool->astClusterTable[pstTtfMemPool->ucClusterCnt - 2].usFreeCnt;
    }
    else
    {
        /* 内存池最大档位剩余内存块数 */
        return pstTtfMemPool->astClusterTable[pstTtfMemPool->ucClusterCnt - 1].usFreeCnt;
    }
} /* TTF_GetLocalFreeMemCnt */



VOS_VOID  TTF_MemPoolInfoEventRpt(VOS_UINT8 ucPoolId)
{
    TTF_MEM_POOL_STRU                      *pstTtfMemPool;
    VOS_UINT16                              usTtfPoolUsedCnt = 0;


    /* C-CPU上行IP包数据内存池需要给ADQ预留128块内存 */
    if (TTF_MEM_POOL_ID_UL_IP_DATA == ucPoolId)
    {
        usTtfPoolUsedCnt = TTF_MEM_UL_IP_DATA_POOL_RSV_CNT;
    }

    pstTtfMemPool = TTF_MEM_GET_POOL(ucPoolId);

    if (PS_TRUE != pstTtfMemPool->usUsedFlag)
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_WARNING, "TTF_MemPoolInfoEventRpt, Pool %d is not used!\n", ucPoolId);
        return;
    }

    if (pstTtfMemPool->usTtfMemUsedCnt> usTtfPoolUsedCnt)
    {
        TTF_LOG2(WUEPS_PID_RLC, PS_PRINT_ERROR, "TTF_MEM_BLK Is Leak,Pool %d,UsedCnt %d \n!",
            ucPoolId, pstTtfMemPool->usTtfMemUsedCnt);

        TTF_BlkMemUsedInfoEventRpt(WUEPS_PID_RLC, ucPoolId, TRIG_TYPE_LEAK);
        TTF_MemReportMemLeakMsg(WUEPS_PID_RLC);
    }
    else
    {
        TTF_LOG1(WUEPS_PID_RLC, PS_PRINT_NORMAL, "NORMAL: TTF_MemPoolInfoEventRpt, Pool %d Normal\n!",ucPoolId);
        TTF_MemSndNormalEvent(ucPoolId) ;
    }


    return;
} /* TTF_MemPoolInfoEventRpt */



VOS_VOID TTF_MemCcpuCheckPoolLeak( VOS_VOID )
{
    VOS_UINT8                          ucPoolId;

    /* v9r1 内存检查时也需要释放挂在cds缓存队列的数据 */
    CDS_FreeIpfDlSrcMem();

    /* V9R1版本上只有TTF_MEM_POOL_ID_DL_PDU，TTF_MEM_POOL_ID_UL_DATA ，
    TTF_MEM_POOL_ID_UL_IP_DATA三个内存池生效 */
    for (ucPoolId = TTF_MEM_POOL_ID_DL_SHARE; ucPoolId < TTF_MEM_POOL_ID_BUTT; ++ucPoolId)
    {
        if ((TTF_MEM_POOL_ID_DL_SHARE != ucPoolId) && (TTF_MEM_POOL_ID_EXT != ucPoolId))
        {
            TTF_MemPoolInfoEventRpt(ucPoolId);
        }
    }

    return;
}



VOS_UINT32 TTF_GetDlIpFreeMemCnt (VOS_VOID)
{
    return TTF_GetLocalFreeMemCnt(TTF_MEM_POOL_ID_DL_PDU);
}



VOS_UINT32 TTF_GetUlIpFreeMemCnt (VOS_VOID)
{
    return TTF_GetLocalFreeMemCnt(TTF_MEM_POOL_ID_UL_IP_DATA);
}



VOS_VOID TTF_MemFreeShowMntnInfo( VOS_VOID )
{
    TTF_MEM_FREE_INVALIED_MEM_INFO_STRU    *pstInvalidMemInfo   = &g_stTtfMemFreeMntnEntity.astTtfMemFreeInvalidMemInfo;
    TTF_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPtrInfo      = &g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPtrInfo;
    TTF_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPPtrInfo     = &g_stTtfMemFreeMntnEntity.astTtfMemFreeNullPPtrInfo;

    vos_printf("TTF MEM FREE 可维可测信息 :\n");
    vos_printf("===========================================\n");
    vos_printf("TTF MEM FREE 非法内存信息 :\n");
    vos_printf("TTF MEM FREE 收到的非法内存个数       : %d \n", (VOS_INT32)pstInvalidMemInfo->ulInvalidTtfMemCnt);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存地址     : 0x%u \n", (VOS_INT32)pstInvalidMemInfo->ulInvalidTtfMemAddr);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存文件ID   : %d \n", pstInvalidMemInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存行号信息 : %d \n", pstInvalidMemInfo->usTtfMemFreeLineNum);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存类型     : %d \n", pstInvalidMemInfo->ucInvalidTtfMemType);
    vos_printf("TTF MEM FREE 非法TTF MEM 内存值       : %d \n", pstInvalidMemInfo->usInvalidTtfMemValue);

    vos_printf("===========================================\n");
    vos_printf("TTF MEM FREE 空指针信息 :\n");
    vos_printf("TTF MEM FREE 一级指针为空的次数       : %d \n", (VOS_INT32)pstNullPtrInfo->ulTtfMemFreeNullPtrCnt);
    vos_printf("TTF MEM FREE 输入指针文件ID           : %d \n", pstNullPtrInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 输入指针内存行号信息     : %d \n", pstNullPtrInfo->usTtfMemFreeLineNum);

    vos_printf("TTF MEM FREE 二级指针为空的次数       : %d \n", (VOS_INT32)pstNullPPtrInfo->ulTtfMemFreeNullPtrCnt);
    vos_printf("TTF MEM FREE 输入指针内存文件ID       : %d \n", pstNullPPtrInfo->usTtfMemFreeFileId);
    vos_printf("TTF MEM FREE 输入指针内存行号信息     : %d \n", pstNullPPtrInfo->usTtfMemFreeLineNum);

    vos_printf("TTF MEM Pool Status:\n");
    vos_printf("          TTF_MEM_POOL_ID_DL_PDU      : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_DL_PDU].usUsedFlag);
    vos_printf("          TTF_MEM_POOL_ID_UL_DATA     : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_UL_DATA].usUsedFlag);
    vos_printf("          TTF_MEM_POOL_ID_UL_IP_DATA  : %d\n", g_astTtfMemPool[TTF_MEM_POOL_ID_UL_IP_DATA].usUsedFlag);

}


#endif



VOS_UINT32 TTF_MemBlkInit(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulBaseAddr;
    VOS_UINT8                          *pucBaseAddr;
    VOS_UINT32                          ulTtfMemPoolLength;
    VOS_UINT8                           ucLoop;
    VOS_UINT32                          ulTtfMemCfgLength;

    /*初始化函数只会被PID初始化函数顺序调用*/
    if ( VOS_TRUE == g_ulTtfMemInitFlag )
    {
        PS_LOG(WUEPS_PID_RLC, 0, PS_PRINT_WARNING,
               "TTF_MemBlkInit, Warning, Memory pools were already initialized!\n");

        return PS_SUCC;
    }

    /* 设置内存池信息 */
    PS_MEM_SET(g_astTtfMemPool, 0, sizeof(TTF_MEM_POOL_STRU)* TTF_MEM_POOL_ID_BUTT);

    /*获取TTF固定内存地址*/
    TTF_GetTTFMemSection(&ulBaseAddr, &ulTtfMemPoolLength);

    /*读取配置文件*/
    ulRet = TTF_MemReadNvCfg(&g_stTtfMemSolution);
    if (PS_SUCC ==ulRet)
    {
        ulTtfMemCfgLength = TTF_MemNvCfgTotalLength(&g_stTtfMemSolution);
        if ( ulTtfMemCfgLength > ulTtfMemPoolLength)
        {
            PS_LOG2(WUEPS_PID_RLC, 0, PS_PRINT_ERROR,
                "TTF_MemBlkInit Fail, TTF_MemBlkInit Total Length Invalid <1> <2> \n",
                (VOS_INT32)ulTtfMemCfgLength, (VOS_INT32)ulTtfMemPoolLength);
            ulRet = PS_FAIL;
        }
    }

    if (PS_FAIL == ulRet)
    {
        TTF_MemSetDefaultNvCfg(&g_stTtfMemSolution);
    }

    pucBaseAddr = (VOS_UINT8 *)ulBaseAddr;
    for (ucLoop = 0; ucLoop < g_stTtfMemSolution.ucPoolCnt; ucLoop++ )
    {
        /* C-CPU上各个内存池是否生效判断 */
        if ( TTF_MEM_MASK_POOL(ucLoop) == (TTF_MEM_MASK_POOL(ucLoop) & g_stTtfMemSolution.ucPoolMask) )
        {
           ulRet = TTF_MemPoolCreate(ucLoop, &g_stTtfMemSolution.astTtfMemPoolCfgInfo[ucLoop],
                                         &pucBaseAddr, &ulTtfMemPoolLength);
           if (PS_FAIL == ulRet)
           {
               return PS_FAIL;
           }
        }
    }

    /* 实始化内存保护实体 */
    TTF_MemRcovEntInit();

    /*置上已被初始化的标志*/
    g_ulTtfMemInitFlag = VOS_TRUE;

    return PS_SUCC;
} /* TTF_MemBlkInit */

#if (FEATURE_PTM == FEATURE_ON)


VOS_VOID TTF_MNTN_ErrlogTtfMemAllocFail(VOS_UINT32 ulPid, VOS_UINT8 ucPoolId, VOS_UINT16 usFileId, VOS_UINT16 usLine, VOS_UINT32 ulAllocFailCnt)
{
    TTF_MNTN_ERR_LOG_MEM_ALLOC_FAIL_STRU    stErrLogMemFail;
    TTF_MNTN_ERR_LOG_MEM_ALLOC_INFO_STRU   *pstMemAllocInfo;
    TTF_MEM_POOL_STRU                      *pstTtfMemPool;
    TTF_MEM_ST                             *pstMem;
    VOS_UINT32                              ulIndx;
    VOS_UINT8                               ucAlmLev;
    VOS_UINT16                              usMemRptCnt;
    MODEM_ID_ENUM_UINT16                    enModemId;



    ucAlmLev    = TTF_ERR_LOG_GET_ALM_LEV(TTF_ERR_LOG_ALM_ID_TTF_MEM_FAIL);
    enModemId   = VOS_GetModemIDFromPid(ulPid);

    if (!TTF_ERR_LOG_NEED_RPT_LEV(enModemId, ucAlmLev))
    {
        return;
    }

    usMemRptCnt = 0;

    TTF_ERR_LOG_FILL_HEADER(
        &(stErrLogMemFail.stHeader),
        enModemId,
        TTF_ERR_LOG_ALM_ID_TTF_MEM_FAIL,
        ucAlmLev,
        (sizeof(TTF_MNTN_ERR_LOG_MEM_ALLOC_FAIL_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stErrLogMemFail.ucPoolId            = ucPoolId;
    stErrLogMemFail.ulAllocFailCnt      = ulAllocFailCnt;
    stErrLogMemFail.usFileId            = usFileId;
    stErrLogMemFail.usLine              = usLine;

    PS_MEM_SET(stErrLogMemFail.aucRsv, 0, sizeof(stErrLogMemFail.aucRsv));

    pstTtfMemPool = TTF_MEM_GET_POOL(ucPoolId);

    for(ulIndx = 0; ulIndx < pstTtfMemPool->usTtfMemTotalCnt;  ulIndx++)
    {
        pstMem          = pstTtfMemPool->pstTtfMemStStartAddr + ulIndx;
        pstMemAllocInfo = &(stErrLogMemFail.astAllocInfo[usMemRptCnt]);

        TTF_ERR_LOG_RCD_DBG(pstMemAllocInfo, pstMem, usMemRptCnt);

        if (usMemRptCnt >= TTF_ERR_LOG_RCD_MEM_CNT)
        {
            break;
        }
    }

    TTF_MNTN_ErrlogBufPut(ulPid, (VOS_CHAR *)&stErrLogMemFail, sizeof(TTF_MNTN_ERR_LOG_MEM_ALLOC_FAIL_STRU));
}

#endif

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

