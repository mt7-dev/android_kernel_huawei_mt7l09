

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_mem_dyn.h"
#include "ucom_comm.h"
#include "om_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 内存控制对象 */
UCOM_MEM_DYN_CTRL_STRU                  g_stUcomMemDynCtrl;

/* DDR内存池 */
VOS_UINT32                              g_auwUcomMemDynPoolDDR[UCOM_MEM_DYN_POOL_SIZE_DDR/sizeof(VOS_UINT32)];

/* TCM内存池 */
UCOM_SEC_TCMBSS
VOS_UINT32                              g_auwUcomMemDynPoolTCM[UCOM_MEM_DYN_POOL_SIZE_TCM/sizeof(VOS_UINT32)];

/* 内存配置表 */
VOS_UINT32                              g_auwUcomMemDynPoolCfgTbl[UCOM_MEM_DYN_TYPE_BUTT][UCOM_MEM_DYN_POOL_CFG_ITEM_BUTT] = \
{
    /* 内存首地址           |       内存大小 */
    {(VOS_UINT32)g_auwUcomMemDynPoolDDR,    UCOM_MEM_DYN_POOL_SIZE_DDR},    /* DDR内存池配置 */
    {(VOS_UINT32)g_auwUcomMemDynPoolTCM,    UCOM_MEM_DYN_POOL_SIZE_TCM},    /* TCM内存池配置 */
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID UCOM_MEM_DYN_Init( VOS_VOID )
{
    UCOM_MEM_DYN_CTRL_STRU             *pstMemCtrlBlk;
    UCOM_MEM_DYN_LIST_STRU             *pstIdleList;
    UCOM_MEM_DYN_LIST_STRU             *pstFreeList;
    UCOM_MEM_DYN_LIST_STRU             *pstUnFreeList;
    UCOM_MEM_DYN_NODE_STRU             *pstNode;
    VOS_UINT32                         (*puwPoolCfgTbl)[UCOM_MEM_DYN_POOL_CFG_ITEM_BUTT];
    VOS_UINT32                          uwCnt;
    UCOM_MEM_DYN_TYPE_ENUM_UINT32       enType;
    VOS_UINT32                          uwMask;

    /* 根据最新编程规范,变量定义时不可初始化 */
    pstMemCtrlBlk = UCOM_MEM_DYN_GetMemCtrl();
    pstIdleList   = UCOM_MEM_DYN_GetIdleList(pstMemCtrlBlk);
    uwCnt         = 0;

    /* 初始化内存控制对象 */
    UCOM_MemSet(pstMemCtrlBlk,
                0,
                sizeof(UCOM_MEM_DYN_CTRL_STRU));

    /* 初始化未使用的内存链表 */
    UCOM_MEM_DYN_InitList(pstIdleList);

    /* 将节点依次加入IdleList */
    for (uwCnt = 0; uwCnt < UCOM_MEM_DYN_NODE_MAX_NUM; uwCnt++)
    {
        UCOM_MEM_DYN_SetIdleNode(&(pstMemCtrlBlk->astNodes[uwCnt]));
    }

    puwPoolCfgTbl = UCOM_MEM_DYN_GetPoolCfgTbl();

    /* 用于字节对齐 */
    uwMask = ~(UCOM_MEM_DYN_MALLOC_ALIGNMENT - 1);

    /* 初始化待分配与已分配内存控制链表 */
    for (enType= 0; enType < UCOM_MEM_DYN_TYPE_BUTT; enType++)
    {
        pstFreeList   = UCOM_MEM_DYN_GetFreeList(pstMemCtrlBlk, enType);
        pstUnFreeList = UCOM_MEM_DYN_GetUnFreeList(pstMemCtrlBlk, enType);

        /* 初始化待分配节点链表 */
         UCOM_MEM_DYN_InitList(pstFreeList);

        /* 初始化已分配节点链表 */
        UCOM_MEM_DYN_InitList(pstUnFreeList);

        /* 获取一个未使用节点 */
        pstNode = UCOM_MEM_DYN_GetIdleNode();

        if (VOS_NULL != pstNode)
        {
            /* 将DDR内存池作为待分配节点加入待分配节点链表 */
            pstNode->stBlk.uwAddr   = (puwPoolCfgTbl[enType][UCOM_MEM_DYN_POOL_CFG_ITEM_ADDR] + (UCOM_MEM_DYN_MALLOC_ALIGNMENT - 1)) & uwMask;
            pstNode->stBlk.uwSize   = puwPoolCfgTbl[enType][UCOM_MEM_DYN_POOL_CFG_ITEM_SIZE] - UCOM_MEM_DYN_MALLOC_ALIGNMENT;

            UCOM_MEM_DYN_InsertNode(pstNode, pstFreeList);
        }
    }

    return;
}


VOS_VOID* UCOM_MEM_DYN_Malloc(
                VOS_UINT32                          uwSize,
                UCOM_MEM_DYN_REQUEST_ENUM_UINT32    enReq)
{
    VOS_UINT32                          uwAddr;

    uwAddr          = VOS_NULL;

    /* 实际分配时, 按照分配内存对齐的整数倍进行 */
    uwSize  = ((uwSize + UCOM_MEM_DYN_MALLOC_ALIGNMENT) - 1) & (~(UCOM_MEM_DYN_MALLOC_ALIGNMENT - 1));

    /*锁任务*/
    VOS_TaskLock();

    /* 分配TCM内存 */
    if(enReq & UCOM_MEM_DYN_REQUEST_TCM)
    {
        uwAddr = UCOM_MEM_DYN_AllocByType(uwSize, UCOM_MEM_DYN_TYPE_TCM);
    }

    /* 分配DDR内存 */
    if( (VOS_NULL == uwAddr) && (enReq & UCOM_MEM_DYN_REQUEST_DDR))
    {
        uwAddr = UCOM_MEM_DYN_AllocByType(uwSize, UCOM_MEM_DYN_TYPE_DDR);
    }

    /* 解锁任务 */
    VOS_TaskUnlock();

    if(VOS_NULL == uwAddr)
    {
        OM_LogFatalAndReset(UCOM_DYN_MEM_Overflow);
    }

    return (VOS_VOID*)uwAddr;

}
VOS_UINT32 UCOM_MEM_DYN_Free(VOS_VOID *pMem)
{
    VOS_UINT32                    (*puwPoolCfgTbl)[UCOM_MEM_DYN_POOL_CFG_ITEM_BUTT];
    UCOM_MEM_DYN_TYPE_ENUM_UINT32   enType;
    VOS_UINT32                      uwAddrStart;
    VOS_UINT32                      uwAddrEnd;
    VOS_UINT32                      uwAddr;
    VOS_UINT32                      uwRet;

    puwPoolCfgTbl   = UCOM_MEM_DYN_GetPoolCfgTbl();
    uwAddr          = (VOS_UINT32)pMem;
    uwRet           = VOS_ERR;

    /*锁任务*/
    VOS_TaskLock();

    /* 查找内存类型 */
    for (enType= 0; enType < UCOM_MEM_DYN_TYPE_BUTT; enType++)
    {
        uwAddrStart = puwPoolCfgTbl[enType][UCOM_MEM_DYN_POOL_CFG_ITEM_ADDR];
        uwAddrEnd   = uwAddrStart + puwPoolCfgTbl[enType][UCOM_MEM_DYN_POOL_CFG_ITEM_SIZE];

        if((uwAddr >= uwAddrStart) && (uwAddr < uwAddrEnd))
        {
            uwRet = UCOM_MEM_DYN_FreeByType(uwAddr, enType);
            break;
        }
    }

    /* 解锁任务 */
    VOS_TaskUnlock();

    return uwRet;

}
VOS_UINT32  UCOM_MEM_DYN_AllocByType(
                VOS_UINT32                          uwSize,
                UCOM_MEM_DYN_TYPE_ENUM_UINT32       enType)
{
    UCOM_MEM_DYN_CTRL_STRU             *pstMemCtrlBlk;
    UCOM_MEM_DYN_LIST_STRU             *pstFreeList;
    UCOM_MEM_DYN_LIST_STRU             *pstUnFreeList;
    UCOM_MEM_DYN_NODE_STRU             *pstNode;
    UCOM_MEM_DYN_NODE_STRU             *pstSeek;
    VOS_UINT32                          uwAddr;
    VOS_UINT32                          uwBestFit;

    pstMemCtrlBlk   = UCOM_MEM_DYN_GetMemCtrl();
    uwAddr          = VOS_NULL;

    /* 获取待分配链表 */
    pstFreeList = UCOM_MEM_DYN_GetFreeList(pstMemCtrlBlk, enType);

    /* 查找是否存在符合条件的待分配节点 */
    pstSeek = UCOM_MEM_DYN_SeekNodeBySize(uwSize, pstFreeList, &uwBestFit);

    /* 可进行分配  */
    if(pstSeek != VOS_NULL)
    {
        /* 分配的内存地址 */
        uwAddr = pstSeek->stBlk.uwAddr;

        /* 获取已分配链表 */
        pstUnFreeList = UCOM_MEM_DYN_GetUnFreeList(pstMemCtrlBlk, enType);

        /* 若待分配节点的大小与需求相同, 则直接分配 */
        if(VOS_OK == uwBestFit)
        {
            UCOM_MEM_DYN_DelNode(pstSeek, pstFreeList);
            UCOM_MEM_DYN_InsertNode(pstSeek, pstUnFreeList);
        }
        else
        {
            /* 获取一个未使用节点 */
            pstNode = UCOM_MEM_DYN_GetIdleNode();

            if(VOS_NULL == pstNode)
            {
                return VOS_NULL;
            }

            /* 做为已分配节点加入链表 */
            pstNode->stBlk.uwAddr = pstSeek->stBlk.uwAddr;
            pstNode->stBlk.uwSize = uwSize;

            UCOM_MEM_DYN_InsertNode(pstNode, pstUnFreeList);

            /* 更新待分配节点信息 */
            pstSeek->stBlk.uwAddr += uwSize;
            pstSeek->stBlk.uwSize -= uwSize;
        }
    }

    return uwAddr;
}
VOS_UINT32  UCOM_MEM_DYN_FreeByType(
                VOS_UINT32                          uwAddr,
                UCOM_MEM_DYN_TYPE_ENUM_UINT32       enType)
{
    UCOM_MEM_DYN_CTRL_STRU             *pstMemCtrlBlk;
    UCOM_MEM_DYN_LIST_STRU             *pstFreeList;
    UCOM_MEM_DYN_LIST_STRU             *pstUnFreeList;
    UCOM_MEM_DYN_NODE_STRU             *pstSeek;
    UCOM_MEM_DYN_NODE_STRU             *pstNext;
    UCOM_MEM_DYN_NODE_STRU             *pstPrev;

    pstMemCtrlBlk   = UCOM_MEM_DYN_GetMemCtrl();

    /* 获取已分配链表 */
    pstUnFreeList = UCOM_MEM_DYN_GetUnFreeList(pstMemCtrlBlk, enType);

    /* 查找是否存在符合条件的已分配节点 */
    pstSeek = UCOM_MEM_DYN_SeekNodeByAddr(uwAddr, pstUnFreeList);

    /* 节点不存在  */
    if(VOS_NULL == pstSeek)
    {
        return VOS_ERR;
    }

    /* 将节点从已分配链表中删除 */
    UCOM_MEM_DYN_DelNode(pstSeek, pstUnFreeList);

    /* 获取待分配链表 */
    pstFreeList = UCOM_MEM_DYN_GetFreeList(pstMemCtrlBlk, enType);

    /* 将节点插回到待分配链表中 */
    UCOM_MEM_DYN_InsertNode(pstSeek, pstFreeList);

    /* 获取前驱和后继节点 */
    pstPrev = pstSeek->pstPrev;
    pstNext = pstSeek->pstNext;

    /* 可以与后继节点合并 */
    if((pstSeek->stBlk.uwAddr + pstSeek->stBlk.uwSize) == pstNext->stBlk.uwAddr)
    {
        /* 合并后继节点空间 */
        pstSeek->stBlk.uwSize += pstNext->stBlk.uwSize;

        /* 删除后继节点 */
        UCOM_MEM_DYN_DelNode(pstNext, pstFreeList);

        /* 将节点放在未使用链表 */
        UCOM_MEM_DYN_SetIdleNode(pstNext);
    }

    /* 可以与前驱节点合并 */
    if((pstPrev->stBlk.uwAddr + pstPrev->stBlk.uwSize) == pstSeek->stBlk.uwAddr)
    {
        /* 前驱合并当前节点空间 */
        pstPrev->stBlk.uwSize += pstSeek->stBlk.uwSize;

        /* 删除当前节点 */
        UCOM_MEM_DYN_DelNode(pstSeek, pstFreeList);

        /* 将节点放在未使用链表 */
        UCOM_MEM_DYN_SetIdleNode(pstSeek);
    }

    return VOS_OK;
}
UCOM_MEM_DYN_NODE_STRU*  UCOM_MEM_DYN_SeekNodeBySize(
                VOS_UINT32                          uwSize,
                UCOM_MEM_DYN_LIST_STRU             *pstList,
                VOS_UINT32                         *puwBestFit)
{
    UCOM_MEM_DYN_NODE_STRU             *pstNode;
    UCOM_MEM_DYN_NODE_STRU             *pstSeek;
    VOS_UINT32                          uwMaxSize;

    if(0 == pstList->uwLen)
    {
        return VOS_NULL;
    }

    pstSeek     = VOS_NULL;
    uwMaxSize   = 0xFFFFFFFF;   /* 无符号整型最大值 */
    *puwBestFit = VOS_ERR;      /* 初始化为非最佳匹配 */

    /* 取第1个节点 */
    pstNode = pstList->stHead.pstNext;

    while(pstNode != NULL)
    {
        /* 有大小相等的则结束搜索 */
        if(pstNode->stBlk.uwSize == uwSize)
        {
            pstSeek     = pstNode;
            *puwBestFit = VOS_OK;
            break;
        }

        /* 搜索大小最接近的 */
        if( (pstNode->stBlk.uwSize > uwSize)
          &&(pstNode->stBlk.uwSize < uwMaxSize))
        {
            pstSeek     = pstNode;
            uwMaxSize   = pstNode->stBlk.uwSize;
        }


        pstNode = pstNode->pstNext;
    }

    /* 不能返回尾节点 */
    if(pstSeek == &pstList->stTail)
    {
        return VOS_NULL;
    }
    else
    {
        return pstSeek;
    }

}


UCOM_MEM_DYN_NODE_STRU*  UCOM_MEM_DYN_SeekNodeByAddr(
                VOS_UINT32                          uwAddr,
                UCOM_MEM_DYN_LIST_STRU             *pstList )
{
    UCOM_MEM_DYN_NODE_STRU             *pstNode;
    UCOM_MEM_DYN_NODE_STRU             *pstSeek;

    if(0 == pstList->uwLen)
    {
        return VOS_NULL;
    }

    pstSeek     = VOS_NULL;

    /* 取第1个节点 */
    pstNode = pstList->stHead.pstNext;

    while(pstNode != NULL)
    {
        /* 有地址相等的则结束搜索 */
        if(pstNode->stBlk.uwAddr == uwAddr)
        {
            pstSeek  = pstNode;
            break;
        }

        pstNode = pstNode->pstNext;
    }

    /* 不能返回尾节点 */
    if(pstSeek == &pstList->stTail)
    {
        return VOS_NULL;
    }
    else
    {
        return pstSeek;
    }

}
VOS_VOID UCOM_MEM_DYN_SetIdleNode(UCOM_MEM_DYN_NODE_STRU *pstNode)
{
    UCOM_MEM_DYN_CTRL_STRU             *pstMemCtrlBlk;
    UCOM_MEM_DYN_LIST_STRU             *pstIdleList;

    pstMemCtrlBlk = UCOM_MEM_DYN_GetMemCtrl();
    pstIdleList   = UCOM_MEM_DYN_GetIdleList(pstMemCtrlBlk);

    UCOM_MEM_DYN_AddNode(pstNode, pstIdleList);
}


UCOM_MEM_DYN_NODE_STRU* UCOM_MEM_DYN_GetIdleNode( VOS_VOID )
{
    UCOM_MEM_DYN_CTRL_STRU             *pstMemCtrlBlk;
    UCOM_MEM_DYN_LIST_STRU             *pstIdleList;
    UCOM_MEM_DYN_NODE_STRU             *pstNode;


    pstMemCtrlBlk = UCOM_MEM_DYN_GetMemCtrl();
    pstIdleList   = UCOM_MEM_DYN_GetIdleList(pstMemCtrlBlk);

    if(0 == pstIdleList->uwLen)
    {
        return VOS_NULL;
    }

    /* 取第一个节点 */
    pstNode = pstIdleList->stHead.pstNext;

    /* 在Idle链表中删除这个节点 */
    UCOM_MEM_DYN_DelNode(pstNode, pstIdleList);

    return pstNode;
}



 VOS_VOID UCOM_MEM_DYN_InitList(UCOM_MEM_DYN_LIST_STRU *pstList)
{
    UCOM_MEM_DYN_NODE_STRU      *pstNode;

    /* 设置头结点 */
    pstNode = &pstList->stHead;
    /* 置尾节点地址为32bit最小地址，方便节点插入 */
    pstNode->stBlk.uwAddr = 0;
    pstNode->stBlk.uwSize = 0;
    pstNode->pstPrev = VOS_NULL;
    pstNode->pstNext = &pstList->stTail;

    /* 设置尾节点 */
    pstNode = &pstList->stTail;
    /* 置尾节点地址为32bit最大地址，方便节点插入 */
    pstNode->stBlk.uwAddr = 0xFFFFFFFF;
    pstNode->stBlk.uwSize = 0;
    pstNode->pstPrev = &pstList->stHead;
    pstNode->pstNext = VOS_NULL;

    /* 设置链表长度 */
    pstList->uwLen = 0;
}
VOS_VOID UCOM_MEM_DYN_AddNode(
                UCOM_MEM_DYN_NODE_STRU      *pstNode,
                UCOM_MEM_DYN_LIST_STRU      *pstList )
{
    UCOM_MEM_DYN_NODE_STRU          *pstCurrNode;

    /* 取第一个节点 */
    pstCurrNode = pstList->stHead.pstNext;

    /* 插入节点 */
    pstCurrNode->pstPrev->pstNext   = pstNode;
    pstNode->pstPrev                = pstCurrNode->pstPrev;
    pstNode->pstNext                = pstCurrNode;
    pstCurrNode->pstPrev            = pstNode;

    /* 更新链表长度 */
    pstList->uwLen++;

}
VOS_VOID UCOM_MEM_DYN_InsertNode(
                UCOM_MEM_DYN_NODE_STRU      *pstNode,
                UCOM_MEM_DYN_LIST_STRU      *pstList )
{
    UCOM_MEM_DYN_NODE_STRU          *pstCurrNode;

    /* 取第一个节点 */
    pstCurrNode = pstList->stHead.pstNext;

    /* 遍历链表插入 */
    while(pstCurrNode != VOS_NULL)
    {
        if(pstCurrNode->stBlk.uwAddr >= pstNode->stBlk.uwAddr)
        {
            /* 插入节点, 保证地址由小到大排列 */
            pstCurrNode->pstPrev->pstNext   = pstNode;
            pstNode->pstPrev                = pstCurrNode->pstPrev;
            pstNode->pstNext                = pstCurrNode;
            pstCurrNode->pstPrev            = pstNode;

            /* 更新链表长度 */
            pstList->uwLen++;

            break;
        }

        /* 下一个节点 */
        pstCurrNode = pstCurrNode->pstNext;
    }
}
VOS_VOID UCOM_MEM_DYN_DelNode(
                UCOM_MEM_DYN_NODE_STRU      *pstNode,
                UCOM_MEM_DYN_LIST_STRU      *pstList )
{
   UCOM_MEM_DYN_NODE_STRU                   *pstPrevNode;
   UCOM_MEM_DYN_NODE_STRU                   *pstNextNode;

   /* 获取节点的前后节点 */
   pstPrevNode = pstNode->pstPrev;
   pstNextNode = pstNode->pstNext;

   /* 删除节点 */
   pstPrevNode->pstNext = pstNextNode;
   pstNextNode->pstPrev = pstPrevNode;

   pstList->uwLen--;
}






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

