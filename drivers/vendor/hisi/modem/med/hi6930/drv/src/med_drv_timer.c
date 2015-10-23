

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_timer_hifi.h"
#include "ucom_comm.h"
#if (VOS_CPU_TYPE == VOS_HIFI)
#include "hal.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 定时器驱动软件数据对象实体, 保存定时器状态、注册信息等 */
DRV_TIMER_CTRL_OBJ_STRU     g_stDrvTimerCtrlObj;


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID DRV_TIMER_Init(VOS_VOID)
{
    VOS_UINT32                          i;
    /* 对V7R2，HIFI使用的timer已切换为32k，不需要上层操作 */
    //VOS_UINT32                          uwScTimerEn1Addr;
    DRV_TIMER_CTRL_OBJ_STRU            *pstTimers  = DRV_TIMER_GetTimerCtrlObjPtr();

    /* 对V7R2，HIFI使用的timer已切换为32k，不需要上层操作 */
    /* 获取timer使能寄存器1的地址 */
    //uwScTimerEn1Addr = SOC_AO_SCTRL_SC_TIMER_EN1_ADDR(DRV_AO_SC_BASE_ADDR);

    /*初始化Timer控制全局结构全0*/
    UCOM_MemSet(pstTimers, 0, sizeof(DRV_TIMER_CTRL_OBJ_STRU));

    /*初始化Timer控制状态*/
    pstTimers->uwIdleNum                        = DRV_TIMER_LINKED_MAX_NUM;
    pstTimers->pstLinkedBlk                     = VOS_NULL_PTR;
    pstTimers->pstIdleBlk                       = pstTimers->astLinkedBlk;

    /*初始化链式Timer双向链表记录，使其前指针相连*/
    for(i = 0; i < (DRV_TIMER_LINKED_MAX_NUM - 1); i++)
    {
        pstTimers->astLinkedBlk[i].uhwUsedFlag  = VOS_NO;
        pstTimers->astLinkedBlk[i].pstNext      = &(pstTimers->astLinkedBlk[i+1]);
    }

    /*对链式Timer边界情况进行特殊处理*/
    pstTimers->astLinkedBlk[DRV_TIMER_LINKED_MAX_NUM - 1].uhwUsedFlag  = VOS_NO;
    pstTimers->astLinkedBlk[DRV_TIMER_LINKED_MAX_NUM - 1].pstNext      = VOS_NULL_PTR;

    /*初始化非链式定时器，目前系统仅支持1，如支持更多需在此添加初始化操作*/
    pstTimers->astUnlinkBlk[0].uwDevice         = DRV_TIMER_DEVICE_DUALTIMER5_1;
    pstTimers->astUnlinkBlk[0].uhwUsedFlag      = VOS_NO;

    /*注册定时器设备中的timer中断处理程序, Timer8,AP Dual_Timer5_1中断*/
    VOS_ConnectInterrupt(OS_INTR_CONNECT_30, DRV_TIMER_IsrUnLinkedTimer);

    /*使能定时器设备中的Timer11中断*/
    VOS_EnableInterrupt(OS_INTR_CONNECT_30);

    /* 注册Timer0组合中断回调函数 */
    VOS_ConnectInterrupt(OS_INTR_CONNECT_28, DRV_TIMER_IsrLinkedTimer);

    /*使能定时器设备中的Timer10中断*/
    VOS_EnableInterrupt(OS_INTR_CONNECT_28);

    /*禁止所有Hifi内部Timer中断，这些中断可以查看V7R2的中断向量表*/
    VOS_DisableInterrupt(OS_INTR_CONNECT_05);
    VOS_DisableInterrupt(OS_INTR_CONNECT_06);
    VOS_DisableInterrupt(OS_INTR_CONNECT_19);

    /* 对V7R2，HIFI使用的timer已切换为32k，不需要上层操作 */
}


VOS_UINT32 DRV_TIMER_Start(
                VOS_UINT32                                 *puwTimer,
                VOS_UINT32                                  uwTimeOutLen,
                DRV_TIMER_MODE_ENUM_UINT16                  enMode,
                DRV_TIMER_INT_FUNC                          pfFunc,
                VOS_UINT32                                  uwPara)
{
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTimer= VOS_NULL_PTR;
    DRV_TIMER_CTRL_OBJ_STRU                                *pstTimers;
    VOS_CPU_SR                                              uwCpuSr;
    VOS_UINT32                                              uwLinkedMaxTimeout;

    /*锁中断互斥*/
    uwCpuSr = VOS_SplIMP();

    /*获取timer控制块结构体指针*/
    pstTimers   = DRV_TIMER_GetTimerCtrlObjPtr();

    uwLinkedMaxTimeout = DRV_TIMER_LINKED_MAX_TIMEOUT;

    /*若已使用的链接定时器个数已达上限或定时时长超过约定值，直接返回错误*/
    if (uwTimeOutLen > uwLinkedMaxTimeout)
    {
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }

    /*申请新的定时块*/
    pstTimer    = DRV_TIMER_GetLinkedBlk(pstTimers);

    /*若无法申请到新的定时块*/
    if (VOS_NULL_PTR == pstTimer)
    {
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }
    else
    {
        /*接收定时相关参数*/
        pstTimer->uwLength  = uwTimeOutLen;
        pstTimer->enMode    = enMode;
        pstTimer->pfFunc    = pfFunc;
        pstTimer->uwPara    = uwPara;

        /* 定时器设备设置 */
        pstTimer->uwDevice  = DRV_TIMER_DEVICE_DUALTIMER5_0;

        /* 新增链接定时器 */
        DRV_TIMER_AddToLink(pstTimer, pstTimers);

        /* 将此定时器控制块地址保存 */
        *puwTimer           = (VOS_UINT32)pstTimer;
    }

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    return VOS_OK;
}


VOS_UINT32 DRV_TIMER_Stop(VOS_UINT32 *puwTimer)
{
    VOS_UINT32                          uwRet       = VOS_OK;
    DRV_TIMER_CTRL_OBJ_STRU            *pstTimers   = DRV_TIMER_GetTimerCtrlObjPtr();
    DRV_TIMER_CTRL_BLOCK_STRU          *pstTimer    = (DRV_TIMER_CTRL_BLOCK_STRU *)(*puwTimer);
    VOS_CPU_SR                          uwCpuSr;

    /*锁中断互斥*/
    uwCpuSr = VOS_SplIMP();

    /*判断puwTimer合法性*/
    uwRet   = DRV_TIMER_CheckLinkedBlk((DRV_TIMER_CTRL_BLOCK_STRU *)(*puwTimer),
                                        pstTimers->pstLinkedBlk);

    /*若非法，则返回错误*/
    if (VOS_ERR == uwRet)
    {
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }

    /* 将定时器置成无效模式,定时中断响应中将删除并释放这个定时器 */
    pstTimer->enMode = DRV_TIMER_MODE_ONESHOT;
    pstTimer->pfFunc = (DRV_TIMER_INT_FUNC)VOS_NULL_PTR;

    VOS_Splx(uwCpuSr);

    return uwRet;
}


VOS_UINT32 DRV_TIMER_StartPrecise(
                VOS_UINT32                                 *puwTimer,
                VOS_UINT32                                  uwTimeOutLen,
                DRV_TIMER_MODE_ENUM_UINT16                  enMode,
                DRV_TIMER_INT_FUNC                          pfFunc,
                VOS_UINT32                                  uwPara)
{
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTimer;
    DRV_TIMER_CTRL_OBJ_STRU                                *pstTimers;
    VOS_CPU_SR                                              uwCpuSr;

    /*锁中断互斥*/
    uwCpuSr = VOS_SplIMP();

    /*获取timer控制块结构体指针*/
    pstTimers   = DRV_TIMER_GetTimerCtrlObjPtr();

    /*若定时时长非法或当前独立定时器已经达到支持的上限，则直接返回错误*/
    if (uwTimeOutLen > DRV_TIMER_UNLINK_MAX_TIMEOUT)
    {
        /*释放互斥*/
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }

    /*申请新的定时块*/
    pstTimer = DRV_TIMER_GetUnlinkBlk(pstTimers);

    /*若无法申请到新的定时块*/
    if (VOS_NULL_PTR == pstTimer)
    {
        /*释放互斥*/
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }
    else
    {
        /*接收定时相关参数*/
        pstTimer->uwLength  = uwTimeOutLen;
        pstTimer->enMode    = enMode;
        pstTimer->pfFunc    = pfFunc;
        pstTimer->uwPara    = uwPara;

        /*根据定时器时钟计算LoadCnt值，并赋给pstTimer*/
        pstTimer->uwLoadCnt = (pstTimer->uwLength * (DRV_TIMER_UNLINK_TIMER_FREQ))/10000;

        /*启动非链接定时器*/
        if (DRV_TIMER_DEVICE_DUALTIMER5_1 == pstTimer->uwDevice)
        {
            /*配置并启动独立定时器*/
            DRV_TIMER_StartDwApbTimer(DRV_TIMER_UNLINK_TIMER_ADDR,
                                      DRV_TIMER_UNLINK_TIMER_IDX,
                                      pstTimer->uwLoadCnt,
                                      pstTimer->enMode);
        }
        else
        {
            /*若HIFI支持更多外部独立定时器，在此添加类似操作*/
        }

        /*将此定时器控制块地址保存，返回给调用者*/
        *puwTimer           = (VOS_UINT32)pstTimer;
    }

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    return VOS_OK;
}


VOS_UINT32 DRV_TIMER_StopPrecise(VOS_UINT32 *puwTimer)
{
    VOS_UINT32                                              uwTimerNum;
    DRV_TIMER_CTRL_OBJ_STRU                                *pstTimers;
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTimer= VOS_NULL_PTR;
    VOS_CPU_SR                                              uwCpuSr;

    /*锁中断互斥*/
    uwCpuSr = VOS_SplIMP();

    pstTimer    = (DRV_TIMER_CTRL_BLOCK_STRU *)(*puwTimer);
    /*获取timer控制块结构体指针*/
    pstTimers   = DRV_TIMER_GetTimerCtrlObjPtr();

    /*验证定时器编号是否在非链接定时器控制块队列内*/
    for (uwTimerNum = 0; uwTimerNum < DRV_TIMER_UNLINK_MAX_NUM; uwTimerNum++)
    {
        if (pstTimer == &(pstTimers->astUnlinkBlk[uwTimerNum]))
        {
            pstTimer->uhwUsedFlag = VOS_NO;

            /*若删除的节点为硬件定时器，则还需要停止硬件设备*/
            if (DRV_TIMER_DEVICE_DUALTIMER5_1 == pstTimer->uwDevice)
            {
                DRV_TIMER_StopDwApbTimer(DRV_TIMER_UNLINK_TIMER_ADDR,
                                         DRV_TIMER_UNLINK_TIMER_IDX);
                pstTimers->uhwUnlinkNum--;
            }
            else
            {
                /*若HIFI支持更多外部独立定时器，在此添加类似操作*/
            }

            /*释放互斥*/
            VOS_Splx(uwCpuSr);
            return VOS_OK;
        }
    }

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    /*若此定时器不在队列内，直接返回错误*/
    return VOS_ERR;
}


VOS_VOID DRV_TIMER_IsrUnLinkedTimer(VOS_VOID)
{
    VOS_UINT32                          uwTimerNum;
    DRV_TIMER_CTRL_BLOCK_STRU          *pstTimer;
    DRV_TIMER_CTRL_OBJ_STRU            *pstTimers;

    /*获取timer控制块结构体指针*/
    pstTimers       = DRV_TIMER_GetTimerCtrlObjPtr();

    /*V7R2中清中断为读清*/
    (VOS_VOID)UCOM_RegRd(DRV_TIMER_EOI(DRV_TIMER_UNLINK_TIMER_ADDR, DRV_TIMER_UNLINK_TIMER_IDX));

    for (uwTimerNum = 0; uwTimerNum < DRV_TIMER_UNLINK_MAX_NUM; uwTimerNum++)
    {
        pstTimer    = &(pstTimers->astUnlinkBlk[uwTimerNum]);

        if (DRV_TIMER_DEVICE_DUALTIMER5_1 ==  pstTimer->uwDevice)
        {
            /*调用对应超时回调函数*/
            pstTimer->pfFunc((VOS_UINT32)pstTimer, pstTimer->uwPara);
            return;
        }
    }
}
VOS_VOID DRV_TIMER_IsrLinkedTimer(VOS_VOID)
{
    VOS_UINT32                     uwLoadCnt       = 0;
    DRV_TIMER_CTRL_OBJ_STRU       *pstTimers;
    DRV_TIMER_CTRL_BLOCK_STRU     *pstTimer;
    DRV_TIMER_CTRL_BLOCK_STRU     *pstNextTimer;
    DRV_TIMER_CTRL_BLOCK_STRU     *pstExpiredTimers[DRV_TIMER_LINKED_MAX_NUM] = {VOS_NULL_PTR};
    VOS_UINT32                     uwExpiredCnt = 0;
    VOS_UINT32                     i;
    VOS_UINT32                     uwLinkedPrecisionNum;

    /*获取timer控制块结构体指针*/
    pstTimers                       = DRV_TIMER_GetTimerCtrlObjPtr();

    pstTimer                        = pstTimers->pstLinkedBlk;

    /*若链接定时头节点为空(即所有节点已释放)，关闭定时器*/
    if (VOS_NULL_PTR == pstTimer)
    {
        DRV_TIMER_StopLinkedTimer();
        return;
    }

    /* 头节点已到期, 将头节点LoadCnt置0 */
    pstTimer->uwLoadCnt  = 0;

    uwLinkedPrecisionNum = DRV_TIMER_LINKED_TIMER_FREQ / 10000;

    /*查找节点链表中的所有到期节点(精度有限为0.1ms，这个范围内可能有多个到期节点)*/
    while ( uwLoadCnt <= uwLinkedPrecisionNum )
    {
        /*保存该节点的Next指针*/
        pstNextTimer = pstTimer->pstNext;

        /*保存到期节点*/
        pstExpiredTimers[uwExpiredCnt] = pstTimer;
        uwExpiredCnt++;

        /*将此到期节点从链表中删除*/
        DRV_TIMER_DelFromLink(pstTimer, pstTimers);

        /*下一个节点*/
        pstTimer = pstNextTimer;

        /*累加到期的节点LoadCnt值*/
        if(VOS_NULL_PTR != pstTimer)
        {
            uwLoadCnt += pstTimer->uwLoadCnt;
        }
        else
        {
            break;
        }

    }

    /*对于第一个非到期节点，若非空，则删除其前节点关系，并重启定时器*/
    if (VOS_NULL_PTR != (pstTimers->pstLinkedBlk))
    {
        /*更新头指针信息*/
        (pstTimers->pstLinkedBlk)->pstPrevious  = VOS_NULL_PTR;

        /*重启定时器*/
        DRV_TIMER_StartLinkedTimer((pstTimers->pstLinkedBlk)->uwLoadCnt);
    }
    /*若头节点为空，停止定时器, 防止hifi定时器翻转匹配上目标值触发中断*/
    else
    {
        DRV_TIMER_StopLinkedTimer();
    }

    /* 处理到期节点 */
    for(i = 0; i < uwExpiredCnt; i++)
    {
        pstTimer = pstExpiredTimers[i];

        /*对于循环定时器，重新加回链表*/
        if (DRV_TIMER_MODE_PERIODIC == pstTimer->enMode)
        {
            DRV_TIMER_AddToLink(pstTimer, pstTimers);
        }
        /*对于非循环定时器，释放*/
        else
        {
            DRV_TIMER_FreeLinkedBlk(pstTimer, pstTimers);
        }
    }
    /* 调用回调函数,注意:这里的隐含条件是DRV_TIMER_FreeLinkedBlk没有清除定时器回调 */
    for(i = 0; i < uwExpiredCnt; i++)
    {
        pstTimer = pstExpiredTimers[i];

         /* 执行回调函数 */
        if( pstTimer->pfFunc != VOS_NULL_PTR )
        {
            pstTimer->pfFunc((VOS_UINT32)pstTimer, pstTimer->uwPara);
        }
    }

}


VOS_UINT32 DRV_TIMER_CheckLinkedBlk(
                DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimerLst)
{
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstListTimer;

    /*将队列头节点赋予临时变量*/
    pstListTimer = pstTimerLst;

    /*遍历对应队列,查找控制块相匹配的节点*/
    while (VOS_NULL_PTR != pstListTimer)
    {
        /*若检索到相匹配节点地址与该控制块地址不等，直接返回正常*/
        if (pstTimer == pstListTimer)
        {
            return VOS_OK;
        }
        pstListTimer = pstListTimer->pstNext;
    }

    /*若未检索到相匹配节点或相匹配的节点地址与该控制块地址不等，返回错误*/
    return VOS_ERR;
}


DRV_TIMER_CTRL_BLOCK_STRU *DRV_TIMER_GetLinkedBlk(DRV_TIMER_CTRL_OBJ_STRU *pstTimers)
{
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTempTimer;

    /*若无空闲定时器块可使用，则直接返回空指针*/
    if (0 == pstTimers->uwIdleNum)
    {
        return VOS_NULL_PTR;
    }
    else
    {
        pstTimers->uwIdleNum--;
    }


    /*取空闲timer控制块的头节点使用*/
    pstTempTimer                = pstTimers->pstIdleBlk;

    /*更新控制块头节点*/
    pstTimers->pstIdleBlk       = pstTimers->pstIdleBlk->pstNext;

    /*初始化该timer控制块节点信息*/
    pstTempTimer->uhwUsedFlag   = VOS_YES;
    pstTempTimer->pstNext       = VOS_NULL_PTR;
    pstTempTimer->pstPrevious   = VOS_NULL_PTR;

    /*返回该timer控制块*/
    return pstTempTimer;
}
DRV_TIMER_CTRL_BLOCK_STRU *DRV_TIMER_GetUnlinkBlk(DRV_TIMER_CTRL_OBJ_STRU *pstTimers)
{
    VOS_UINT32                          uwTimerNum;
    DRV_TIMER_CTRL_BLOCK_STRU          *pstTimer;

    /*若所有非链式控制块都已使用，则直接返回空指针*/
    if (DRV_TIMER_UNLINK_MAX_NUM == pstTimers->uhwUnlinkNum)
    {
        return VOS_NULL_PTR;
    }

    /*更新非链接定时器状态信息*/
    pstTimers->uhwUnlinkNum++;

    /*查找非链接定时器控制块组内第一个未被使用的定时器块*/
    for (uwTimerNum = 0; uwTimerNum < DRV_TIMER_UNLINK_MAX_NUM; uwTimerNum++)
    {
        /*获取遍历的非链接定时器块指针*/
        pstTimer    = &(pstTimers->astUnlinkBlk[uwTimerNum]);

        /*若当前非链接定时器块未被使用*/
        if (VOS_NO == pstTimer->uhwUsedFlag)
        {
            /*修改该timer控制块状态信息*/
            pstTimer->uhwUsedFlag   = VOS_YES;
            break;
        }
    }

    /*返回该timer控制块*/
    return pstTimer;
}
VOS_VOID DRV_TIMER_FreeLinkedBlk(
                DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers)
{
    /*将释放的timer控制块前插入空闲timer控制块队列*/
    pstTimer->uhwUsedFlag   = VOS_NO;
    pstTimer->pstNext       = pstTimers->pstIdleBlk;
    pstTimers->pstIdleBlk   = pstTimer;

    /*更新空闲timer控制块个数*/
    pstTimers->uwIdleNum++;
}


VOS_VOID DRV_TIMER_AddToLink(
                DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers)
{
    VOS_UINT32                                              uwCntTmp;
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTmpTimer;
    DRV_TIMER_CTRL_BLOCK_STRU                              *pstTmpPreTimer;

    /*更新节点状态信息*/
    pstTimers->uhwLinkedNum++;

    /*根据定时器时钟计算LoadCnt值，并赋给pstTimer*/
    pstTimer->uwLoadCnt = (pstTimer->uwLength * (DRV_TIMER_LINKED_TIMER_FREQ))/10000;

    /*初始化临时变量*/
    pstTmpTimer     = pstTimers->pstLinkedBlk;
    pstTmpPreTimer  = pstTimers->pstLinkedBlk;

    if (VOS_NULL_PTR != pstTmpTimer)
    {
        /*计算本节点需设置的LoadCnt值*/
        uwCntTmp        = pstTimer->uwLoadCnt
                        + (pstTmpTimer->uwLoadCnt - DRV_TIMER_ReadLinkedTimer());
    }
    else
    {
        /*当前为首节点*/
        uwCntTmp        = pstTimer->uwLoadCnt;
    }

    /*若当前头节点为空,则本节点为头节点*/
    if (VOS_NULL_PTR == pstTimers->pstLinkedBlk)
    {
        pstTimers->pstLinkedBlk                = pstTimer;
    }
    else
    {
        /*遍历查找当前链表中各节点时长,并找到合适的插入位置*/
        while (VOS_NULL_PTR != pstTmpTimer)
        {
            /*若需插入的节点时长比当前节点长，则需继续往后寻找插入位置*/
            if (uwCntTmp >= (pstTmpTimer->uwLoadCnt))
            {
                /*修正插入节点时长*/
                uwCntTmp -= pstTmpTimer->uwLoadCnt;
            }
            else
            {
                /*若需插入的节点时长比当前节点短，则插入位置在当前节点前，更新当前节点时长*/
                pstTmpTimer->uwLoadCnt -= uwCntTmp;
                break;
            }
            pstTmpPreTimer          = pstTmpTimer;
            pstTmpTimer             = pstTmpTimer->pstNext;
        }

        /*若需要插在头节点之前*/
        if ((pstTimers->pstLinkedBlk == pstTmpTimer)
            && (VOS_NULL_PTR != pstTmpTimer))
        {
            pstTimers->pstLinkedBlk = pstTimer;
            pstTimer->pstNext       = pstTmpTimer;
            pstTmpTimer->pstPrevious= pstTimer;
        }
        else
        {
            /*更新插入节点的前后关系*/
            pstTmpPreTimer->pstNext = pstTimer;
            pstTimer->pstPrevious   = pstTmpPreTimer;
            pstTimer->pstNext       = pstTmpTimer;
            if(pstTmpTimer != VOS_NULL_PTR)
            {
                pstTmpTimer->pstPrevious= pstTimer;
            }
        }
    }
    /* 若需插入的位置在头节点前,则将此节点更新为头节点并重新启动定时器*/
    if ( pstTimer == pstTimers->pstLinkedBlk )
    {
        /*配置重启定时器*/
        DRV_TIMER_StartLinkedTimer(pstTimer->uwLoadCnt);
    }
    /*否则，更新待插入节点的时长*/
    else
    {
        pstTimer->uwLoadCnt = uwCntTmp;
    }
}
VOS_VOID DRV_TIMER_DelFromLink(
                DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers)
{

    /*若需删除的对应节点为头节点*/
    if (pstTimer == pstTimers->pstLinkedBlk)
    {
        /*更新头节点信息*/
        pstTimers->pstLinkedBlk                    = pstTimer->pstNext;
        if (VOS_NULL_PTR != pstTimers->pstLinkedBlk)
        {
            pstTimers->pstLinkedBlk->pstPrevious   = VOS_NULL_PTR;
        }
    }
    else
    {
        /*若待删除的节点非头节点，则更其前后节点信息*/
        (pstTimer->pstPrevious)->pstNext            = pstTimer->pstNext;
        if ( VOS_NULL_PTR != pstTimer->pstNext )
        {
            (pstTimer->pstNext)->pstPrevious        = pstTimer->pstPrevious;
        }
    }

    pstTimers->uhwLinkedNum -- ;

    /*若删除的节点有后续节点,则更新该节点的计数值*/
    if (VOS_NULL_PTR != pstTimer->pstNext)
    {
        pstTimer->pstNext->uwLoadCnt += pstTimer->uwLoadCnt;
    }

    /*若当前已无节点，则停止定时器*/
    if (0 == pstTimers->uhwLinkedNum)
    {
        /*停止定时器*/
        DRV_TIMER_StopLinkedTimer();
    }

    /* 删除当前节点的前后关系 */
    pstTimer->pstNext       = VOS_NULL_PTR;
    pstTimer->pstPrevious   = VOS_NULL_PTR;
}
VOS_VOID DRV_TIMER_StartDwApbTimer(
                VOS_UINT32              uwBaseAddr,
                VOS_UINT32              uwTimerIndx,
                VOS_UINT32              uwLoadCnt,
                VOS_UINT16              uhwMode)
{
    VOS_UINT32                          uwTimerCtrlAddr;
    VOS_UINT32                          uwTimerCountAddr;
    VOS_UINT32                          uwTimerBitWr;
    /* 打开外设时钟 */
    UCOM_RegBitWr(DRV_TIMER_PERIPH_CLK_EN_ADDR,UCOM_BIT0,UCOM_BIT0,0x1);

    /*计算APB TIMER设备中索引号为uwTimerIndx的定时器的控制寄存器地址*/
    uwTimerCtrlAddr  = DRV_TIMER_CONTROL(uwBaseAddr, uwTimerIndx);

    /*计算APB TIMER设备中索引号为uwTimerIndx的定时器的初始值寄存器地址*/
    uwTimerCountAddr = DRV_TIMER_LOADCOUNT(uwBaseAddr, uwTimerIndx);

    /*对V7R2的TIMER，验证发现不能在BIT4为1时写其他位，否则会造成其他异常，
      如ctrl寄存器其他值被改或者loadcnt不使能。因此不能使用BIT位逐一写入
      根据寄存器手册，中断不屏蔽，BIT2写0，TIMER不使能，BIT0写0，BIT1由输入决定*/
    uwTimerBitWr     = 0;
    uwTimerBitWr     = uwTimerBitWr | (uhwMode << UCOM_BIT1);

    /*设定定时器: 定时器中断不屏蔽, 定时器模式由输入确定
      另外，V7R2默认为递减，DRV已配置好大小为32bit计数, 这两点不需要专门配置*/
    UCOM_RegWr(uwTimerCtrlAddr, uwTimerBitWr);

    /*设置定时器初值*/
    UCOM_RegWr(uwTimerCountAddr, uwLoadCnt);

    /*读清对应的中断寄存器,清除对应中断*/
    (VOS_VOID)UCOM_RegRd(DRV_TIMER_EOI(uwBaseAddr, uwTimerIndx));

    /*启动定时器*/
    uwTimerBitWr     = uwTimerBitWr | (1 << UCOM_BIT0);
    UCOM_RegWr(uwTimerCtrlAddr, uwTimerBitWr);

}
VOS_VOID DRV_TIMER_RestartDwApbTimer(
                VOS_UINT32              uwBaseAddr,
                VOS_UINT32              uwTimerIndx)
{
    VOS_UINT32                          uwTimerAddr;
    VOS_UINT32                          uwTimerWr;

    /*计算APB TIMER设备中索引号为uwTimerIndx的定时器的控制寄存器地址*/
    uwTimerAddr = DRV_TIMER_CONTROL(uwBaseAddr, uwTimerIndx);

    uwTimerWr   = UCOM_RegRd(uwTimerAddr);

    /*禁止定时器,即在第0位写0，第4位为timer标记位，需同时写0*/
    uwTimerWr   = uwTimerWr & (0xffffffee);
    UCOM_RegWr(uwTimerAddr, uwTimerWr);

    /*写清对应的中断寄存器,清除对应中断*/
    (VOS_VOID)UCOM_RegRd(DRV_TIMER_EOI(uwBaseAddr, uwTimerIndx));

    /*启动定时器, 即在第0位写1*/
    uwTimerWr   = uwTimerWr | (1 << UCOM_BIT0);
    UCOM_RegWr(uwTimerAddr, uwTimerWr);
}


VOS_UINT32 DRV_TIMER_ReadDwApbTimer(
                VOS_UINT32              uwBaseAddr,
                VOS_UINT32              uwTimerIndx,
                VOS_UINT32             *puwCntLow,
                VOS_UINT32             *puwCntHigh)
{
    VOS_UINT32                          uwCntLow;

    /*读取计数值低32bit*/
    uwCntLow = UCOM_RegRd(DRV_TIMER_CURRENTVALUE(uwBaseAddr, uwTimerIndx));

    /*若输入的保存计数值指针非空，则保存计数值低32bit*/
    if (VOS_NULL != puwCntLow)
    {
        *puwCntLow  = uwCntLow;
    }

    /*默认返回计数值低32bit*/
    return uwCntLow;
}
VOS_VOID DRV_TIMER_StopDwApbTimer(
                VOS_UINT32              uwBaseAddr,
                VOS_UINT32              uwTimerIndx)
{
    VOS_UINT32                          uwTimerAddr;
    VOS_UINT32                          uwTimerWr;

    /*计算APB TIMER设备中索引号为uwTimerIndx的定时器的控制寄存器地址*/
    uwTimerAddr = DRV_TIMER_CONTROL(uwBaseAddr, uwTimerIndx);

    uwTimerWr   = UCOM_RegRd(uwTimerAddr);

    /*禁止定时器,并屏蔽中断，即在第0位写0，第2位写1, 第4位为timer标记位，需同时写0 */
    uwTimerWr   = uwTimerWr & 0xffffffee;
    uwTimerWr   = uwTimerWr | (1 << UCOM_BIT2);

    /*屏蔽中断*/
    UCOM_RegWr(uwTimerAddr, uwTimerWr);
}
VOS_UINT32 DRV_TIMER_ReadSysTimeStamp(VOS_VOID)
{
    return UCOM_RegRd(DRV_TIMER_OM_ADDR);
}


VOS_UINT32 DRV_TIMER_GetOmFreq(VOS_VOID)
{
    return DRV_TIMER_OM_FREQ;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

