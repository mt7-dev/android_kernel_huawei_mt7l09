/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fsm.c
  版 本 号   : 初稿
  作    者   : 邵海刚 45755
  生成日期   : 2009年07月10日
  最近修改   :
  功能描述   : 状态机运转基本函数
  函数列表   :
  修改历史   :
  1.日    期   : 2009年07月10日
    作    者   : 邵海刚 45755
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "vos.h"
#include "fsm.h"
#include "ucom_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : FSM_Swap
 功能描述  : 值互换函数
 输入参数  : VOS_INT16 *pshwTarget
             VOS_INT16 *pshwSource
             VOS_UINT32     uwLength，单位:sizeof
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年7月24日
    作    者   : chenchaoqun
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID FSM_Swap(
                        VOS_INT16        *pshwTarget,
                        VOS_INT16        *pshwSource,
                        VOS_UINT32        uwLength )
{

    /*  内容交换函数，限定交换的内容长度不超过FSM_COMP_LENGTH_MAX，单位:word */
    VOS_INT16                           auwTemp[FSM_COMP_LENGTH_MAX];

    UCOM_MemCpy(&auwTemp[0], pshwTarget, uwLength);
    UCOM_MemCpy(pshwTarget, pshwSource, uwLength);
    UCOM_MemCpy(pshwSource, &auwTemp[0], uwLength);
}
/*****************************************************************************
 函 数 名  : FSM_Sort
 功能描述  : 冒泡排序函数
 输入参数  : VOS_INT16 * pTable
             VOS_UINT32 uwNum
             VOS_UINT32 uwWidth,pTable所指向结构体长度,单位sizeof
             FSM_COMP_FUNC fCompareFunc
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年7月24日
    作    者   : chenchaoqun
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID FSM_Sort(
                      VOS_UINT8            *puhwTable,
                      VOS_UINT32            uwNum,
                      VOS_UINT32            uwWidth,
                      FSM_COMP_FUNC         fCompareFunc)
{
    VOS_INT16                       shwRet;
    VOS_UINT32                      uwLoop;
    VOS_UINT32                      uwSubLoop;


    for ( uwLoop = 0 ; uwLoop < uwNum ; uwLoop++ )
    {

        /*对下标为0->i之间内容进行排序*/
        for ( uwSubLoop = 0 ; uwSubLoop < ((uwNum - uwLoop) - 1 ); uwSubLoop++ )
        {
             shwRet = fCompareFunc( puhwTable + ( uwSubLoop * uwWidth ),
                                    puhwTable + ( (uwSubLoop + 1) * uwWidth ) );

             /* 若前者大于后者，两者交换，将前者后移*/
             if ( 1 == shwRet )
             {
                 FSM_Swap((VOS_INT16*)(puhwTable + (uwSubLoop*uwWidth)),(VOS_INT16*)(puhwTable+((uwSubLoop+1)*uwWidth)),uwWidth);
             }

        }
    }
}

/*****************************************************************************
 函 数 名  : FSM_Bsearch
 功能描述  : 折半查找函数,内容有序
 输入参数  : VOS_INT16 * pKey
             VOS_INT16 * pBase
             VOS_UINT32 uwNum
             VOS_UINT32 uwWidth
             FSM_COMP_FUNC fCompareFunc
 输出参数  : 无
 返 回 值  : VOS_VOID *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年7月24日
    作    者   : chenchaoqun
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID * FSM_Bsearch(
                             VOS_INT16      *pKey,
                             VOS_UINT8      *pBase,
                             VOS_UINT32      uwNum,
                             VOS_UINT32      uwWidth,
                             FSM_COMP_FUNC   fCompareFunc)
{
    VOS_INT32                           swLow;
    VOS_INT32                           swMid;
    VOS_INT32                           swHigh;
    VOS_VOID                           *pRet = VOS_NULL_PTR;
    VOS_INT16                           shwRet;

    swLow       = 0;
    swHigh      = (VOS_INT32)uwNum - 1;

    while(swLow <= swHigh)
    {

        swMid   = (swLow + swHigh) >> 1;
        shwRet  = fCompareFunc(pKey, pBase + ((VOS_UINT32)swMid * uwWidth));

        /* 若要查找的值大于该值，则以该值的下一个值为起始点进一步查找 */
        if (1 == shwRet)
        {
            swLow = swMid + 1;
        }
        /* 若要查找的值小于该值，则以该值的上一个值为截止点进一步查找 */
        else if ( -1 == shwRet )
        {
            swHigh = swMid - 1;
        }
        else
        {
            /* 若查找到则返回该位置 */
            pRet = pBase + ((VOS_UINT32)swMid * uwWidth);
            break;
        }
    }
    return pRet;
}

/*****************************************************************************
 Function   : FSM_StaCompare
 Description: 状态比较函数，在对状态转移表进行快速排序时用到
 Calls      :
 Called By  :
 Input      : 状态1 状态2
 Output     :
 Return     : 0 -- 状态1和状态2相等
             -1 -- 状态1小于状态2
              1 -- 状态1大于状态2
 Other      : 内部函数
 *****************************************************************************/
VOS_INT16 FSM_StaCompare( const VOS_VOID *arg1, const VOS_VOID *arg2 )
{
    STA_STRU                    *pstSta1;
    STA_STRU                    *pstSta2;

    pstSta1 = (STA_STRU*) arg1;
    pstSta2 = (STA_STRU*) arg2;

    /*  如果状态1小于状态2,返回-1 */
    if (pstSta1->ulState < pstSta2->ulState)
    {
        return -1;
    }

    /*  如果状态1等于状态2,返回0 */
    if (pstSta1->ulState == pstSta2->ulState)
    {
        return 0;
    }

    /*  如果状态1大于状态2,返回1 */
    return 1;
}


/*****************************************************************************
 Function   : FSM_ActCompare
 Description: 事件比较函数，在对事件处理表进行快速排序时用到
 Calls      :
 Called By  :
 Input      : 事件1 事件2
 Output     :
 Return     : 0 -- 事件1和事件2相等
             -1 -- 事件1小于事件2
              1 -- 事件1大于事件2
 Other      : 内部函数
 *****************************************************************************/
VOS_INT16 FSM_ActCompare( const VOS_VOID *arg1, const VOS_VOID *arg2 )
{
    ACT_STRU                    *pstAct1;
    ACT_STRU                    *pstAct2;

    pstAct1 = (ACT_STRU*) arg1;
    pstAct2 = (ACT_STRU*) arg2;

    /*  如果事件1小于事件2,返回-1 */
    if (pstAct1->ulEventType < pstAct2->ulEventType)
    {
        return -1;
    }

    /*  如果事件1等于事件2,返回0 */
    if (pstAct1->ulEventType == pstAct2->ulEventType)
    {
        return 0;
    }

    /*  如果事件1大于事件2,返回0 */
    return 1;
}


/*****************************************************************************
 Function   : FSM_FindAct
 Description: 根据状态和事件类型在排序后的状态转移表中快速查找事件处理函数
 Calls      :
 Called By  :
 Input      : 状态机描述符 状态 事件类型
 Output     :
 Return     : 事件处理函数
 Other      : 内部函数
 *****************************************************************************/
ACTION_FUN FSM_FindAct( FSM_DESC_STRU  *pFsmDesc,
                             VOS_UINT32      uwState,
                             VOS_UINT32      uwEventType)
{
    STA_STRU             stKeySta;      /* 要查找的状态 */
    STA_STRU            *pstRetSta;     /* 返回的状态   */
    ACT_STRU             stKeyAct;      /* 要查找的事件 */
    ACT_STRU            *pstRetAct;     /* 返回的事件   */

    pstRetSta           = VOS_NULL_PTR;
    pstRetAct           = VOS_NULL_PTR;

    stKeySta.ulState    = uwState;

    /* 调用二分查找函数bsearch在状态转移表中查找相应的事件处理表 */
    pstRetSta           = (STA_STRU*) FSM_Bsearch( (VOS_INT16 *)(&stKeySta),
                                                   (VOS_UINT8 *)pFsmDesc->pStaTable,
                                                   pFsmDesc->ulSize,
                                                   sizeof(STA_STRU)/sizeof(VOS_INT8),
                                                  (FSM_COMP_FUNC)FSM_StaCompare );

    /* 如果找不到相应的事件处理表,设置错误码,返回VOS_NULL_PTR */
    if (VOS_NULL_PTR == pstRetSta)
    {
        /* 此处与原函数不同，不再向Dopra返回错误码，
           而是直接返回空指针，表示该消息没有被该状态机处理。 */
        return (ACTION_FUN) VOS_NULL_PTR;
    }

    stKeyAct.ulEventType = uwEventType;

    /* 调用二分查找函数bsearch在事件处理表中查找相应的事件处理函数 */
    pstRetAct = (ACT_STRU*) FSM_Bsearch ( (VOS_INT16 *)(&stKeyAct),
                                          (VOS_UINT8 *)pstRetSta->pActTable,
                                          pstRetSta->ulSize,
                                          sizeof(ACT_STRU)/sizeof(VOS_INT8),
                                          (FSM_COMP_FUNC)FSM_ActCompare );


    /* 如果找不到相应的事件处理函数,设置错误码,返回VOS_NULL_PTR */
    if (VOS_NULL_PTR == pstRetAct)
    {
        /* 此处与原函数不同，不再向Dopra返回错误码，
           而是直接返回空指针，表示该消息没有被该状态机处理。 */
        return (ACTION_FUN) VOS_NULL_PTR;
    }

    return (pstRetAct->pfActionFun);

}

/*****************************************************************************
 Function   : FSM_StaSort
 Description: 对状态转移表和和里面的每个事件处理函数表进行快速排序
 Calls      :
 Called By  :
 Input      : 状态转移表 状态转移表的大小
 Output     : 排序后的状态转移表
 Return     :
 Other      : 内部函数
 *****************************************************************************/
VOS_VOID FSM_StaSort(STA_STRU* pstTable, VOS_UINT32 uwSize)
{
    VOS_UINT32    uwIndex;

    /* 调用冒泡排序对pTable进行排序 */
    FSM_Sort( (VOS_UINT8 *)pstTable,
               uwSize,
               sizeof(STA_STRU),
              (FSM_COMP_FUNC)FSM_StaCompare);

    /* 对每一个事件处理函数表调用冒泡排序进行排序 */
    for (uwIndex = 0; uwIndex < uwSize; uwIndex++)
    {
        FSM_Sort( (VOS_UINT8 *)pstTable->pActTable,
                   pstTable->ulSize,
                   sizeof(ACT_STRU),
                  (FSM_COMP_FUNC)FSM_ActCompare );

        /* 对数组中的下一个STA_STRU元素中的动作表进行排序 */
        pstTable++;
    }
}

/*****************************************************************************
 Function   : FSM_RegisterFsm
 Description: 登记状态机表
 Calls      :
 Called By  :
 Input      : 状态机名字 状态转移表 状态转移表大小 异常处理函数
 Output     : 状态机描述符
 Return     : SUCC          -- success
              other err code    -- failure
 Other      : 外部函数
 *****************************************************************************/
VOS_UINT32 FSM_RegisterFsm( FSM_DESC_STRU   *pstFsmDesc,
                                  VOS_UINT32       uwSize,
                                  STA_STRU        *pstStaTable,
                                  EXCEP_FUN        pfExceptHandle )
{

    /* 检查入口参数 */
    if ( ( VOS_NULL_PTR == pstFsmDesc ) || ( VOS_NULL_PTR == pstStaTable ) )
    {
        return VOS_ERR;
    }

    /* 对状态机描述符结构进行赋值 */
    pstFsmDesc->ulSize         = uwSize;
    pstFsmDesc->pStaTable      = pstStaTable;
    pstFsmDesc->pfExceptHandle = pfExceptHandle;

    /* 对状态转移表进行排序 */
    FSM_StaSort(pstStaTable, uwSize);

    return VOS_OK;

}


/*****************************************************************************
 Function   : FSM_ProcessEvent
 Description: 状态机的事件处理函数
 Calls      :
 Called By  :
 Input      : 状态机描述符 当前状态 处理事件 消息ID 消息指针
 Output     :
 Return     :
 Other      : 外部函数
 *****************************************************************************/
VOS_UINT32 FSM_ProcessEvent(
                    FSM_DESC_STRU                   *pFsmDesc,
                    VOS_UINT32                       uwCurState,
                    VOS_UINT32                       uwEventType,
                    VOS_UINT16                       uhwMsgID,
                    VOS_VOID                        *pRcvMsg )
{
    ACTION_FUN                          pActFun;

    /* 查找状态转移表中的相应的动作处理函数 */
    pActFun = FSM_FindAct(pFsmDesc, uwCurState, uwEventType);

    if (pActFun != VOS_NULL_PTR)
    {
        /* 如果返回的事件处理函数不为空,调用它进行事件处理 */
        return (*pActFun) ( pRcvMsg );
    }
    /* 如果找不到处理函数，并不表示异常，仅仅表示该消息没有被当前状态机处理，
        在这种情况下，需要调用该状态机的异常处理函数，看该消息能够在异常处理函数中处理。 */
    else
    {
        /* 异常函数处理， */
        if (VOS_NULL_PTR != pFsmDesc->pfExceptHandle)
        {
            return (*pFsmDesc->pfExceptHandle) (uwEventType, pRcvMsg );
        }
        /* 如果没有异常处理函数，表示该消息没有被该状态机处理 */
        else
        {
            return VOS_ERR;
        }
    }
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

