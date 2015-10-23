

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_ipc.h"
#include "ucom_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*lint -e413*/

/* IPC互发中断响应程序的函数指针数组 */
DRV_IPC_INT_SRC_ISR_STRU    g_astDrvIpcIntSrcIsr[DRV_IPC_MAX_INT_NUM];

/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_VOID DRV_IPC_Init( VOS_VOID )
{
    DRV_IPC_INT_SRC_ISR_STRU           *pstIntIsr   = DRV_IPC_GetIntSrcIsrPtr();

    /*注册IPC ISR*/
    VOS_ConnectInterrupt(DRV_IPC_INT_NO_HIFI, DRV_IPC_Isr);

    /*写全0屏蔽HiFi所有互发源*/
    UCOM_RegWr(DRV_IPC_CPU_INT_ENABLE_HIFI, 0x0);

    /*初始化g_astDrvIpcIntSrcIsr为全NULL*/
    UCOM_MemSet(&pstIntIsr[0], 0, DRV_IPC_MAX_INT_NUM*sizeof(DRV_IPC_INT_SRC_ISR_STRU));

}
VOS_VOID DRV_IPC_Isr( VOS_VOID )
{
    VOS_UINT32                              i;
    VOS_UINT32                              uwIntState;
    DRV_IPC_INT_SRC_ISR_STRU               *pstIntIsr = DRV_IPC_GetIntSrcIsrPtr();

    /*读取DRV_IPC_CPU_INT_STAT_REG(IPC_TARGET_CPU_HIFI)*/
    uwIntState  = UCOM_RegRd(DRV_IPC_CPU_INT_STAT_HIFI);

    /* 无中断源待处理则退出 */
    if (0 == uwIntState)
    {
        return;
    }

    /*写uwState入DRV_IPC_CPU_INT_CLR_REG(IPC_TARGET_CPU_HIFI)清除已经处理中断*/
    UCOM_RegWr(DRV_IPC_CPU_INT_CLR_HIFI, uwIntState);

    /*遍历所有32个BIT位的IPC中断请求*/
    for (i = 0; i < DRV_IPC_MAX_INT_NUM; i++)
    {
        /*若该BIT位有中断请求*/
        if (uwIntState & (0x1L << i))
        {
            /*若对应中断处理函数非空*/
            if (VOS_NULL != pstIntIsr[i].pfFunc)
            {
                /*调用对应的中断处理函数*/
                pstIntIsr[i].pfFunc(pstIntIsr[i].uwPara);
            }
        }
    }

}
VOS_VOID DRV_IPC_RegIntSrc(
                IPC_INT_LEV_E                   enSrc,
                DRV_IPC_INT_FUNC                pfFunc,
                VOS_UINT32                      uwPara)
{
    DRV_IPC_INT_SRC_ISR_STRU               *pstIntIsr = DRV_IPC_GetIntSrcIsrPtr();

    /*清除对应中断源中断*/
    /* UCOM_RegWr(DRV_IPC_CPU_INT_CLR_HIFI, (0x1<<enSrc)); */

    /*保存注册的中断服务程序信息*/
    pstIntIsr[enSrc].pfFunc = pfFunc;
    pstIntIsr[enSrc].uwPara = uwPara;

    /*打开屏蔽位，使能该源中断*/
    UCOM_RegBitWr(DRV_IPC_CPU_INT_ENABLE_HIFI, enSrc, enSrc, 1);
}


VOS_VOID DRV_IPC_TrigInt(
                VOS_UINT16                  enTarget,
                VOS_UINT16                  enIntSrc)
{
    /*写对应CPU对应中断源比特，触发IPC中断*/
    UCOM_RegBitWr(DRV_IPC_CPU_RAW_INT((VOS_UINT32)enTarget), enIntSrc, enIntSrc, 1);
}

/*lint +e413*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

