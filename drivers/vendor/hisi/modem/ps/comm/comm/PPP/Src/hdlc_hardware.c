
/******************************************************************************
   头文件包含
******************************************************************************/
#include "hdlc_hardware.h"
#include "TTFComm.h"
#include "soc_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "ppp_public.h"


#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif

#if(FEATURE_ON == FEATURE_PPP)
#ifdef FEATURE_HDLC_MASTER
/*****************************************************************************
   1 协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_HDLC_HARDWARE_C
/*lint +e767 */

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
#if (VOS_OS_VER == VOS_WIN32)
extern VOS_UINT32  VHW_HDLC_Task( VOS_VOID );
extern VOS_UINT8   g_ucScCtrlRegAddr[0xFFC];
#endif

extern PPP_DATA_Q_CTRL_ST     g_PppDataQCtrl;
/*****************************************************************************
   3 私有定义
*****************************************************************************/
VOS_UINT32 PPP_HDLC_HARD_DefIsr(unsigned int ulPara);

VOS_UINT32 PPP_HDLC_HARD_FrmIsr(unsigned int ulPara);


#define  PPP_HDLC_MNTN_TRACE_PARA       (1)
#define  PPP_HDLC_MNTN_TRACE_REG        (2)
#define  PPP_HDLC_MNTN_TRACE_DATA       (4)

/*****************************************************************************
   4 全局变量定义
*****************************************************************************/
/* 解封装输出的非完整帧信息 */
HDLC_DEF_UNCOMPLETED_INFO_STRU  g_stUncompletedInfo = {0};

/* 保存解封装使用的内存 */
HDLC_DEF_BUFF_INFO_STRU        *g_pstHdlcDefBufInfo = VOS_NULL_PTR;

/* 保存封装使用的内存 */
HDLC_FRM_BUFF_INFO_STRU        *g_pstHdlcFrmBufInfo = VOS_NULL_PTR;

/* HDLC配置相关信息 */
HDLC_CONFIG_INFO_STRU           g_stHdlcConfigInfo  =
{
    0,
    0,
    0,
    0,
    0,
    HDLC_DEF_INTERRUPT_LIMIT_DEFAULT,
    HDLC_FRM_INTERRUPT_LIMIT_DEFAULT,
    VOS_NULL_PTR,
    0
};

/* 统计信息 */
PPP_HDLC_HARD_DATA_PROC_STAT_ST g_PppHdlcHardStat   = {0};

/* 保留清原始中断时的RAW_INT和STATUS值 */
HDLC_REG_SAVE_INFO_STRU         g_stHdlcRegSaveInfo;

#define    HDLC_IP_BASE_ADDR    (g_stHdlcConfigInfo.ulHDLCIPBaseAddr)

/* 系统控制器基地址 */
VOS_UINT32  g_ulHdlcScCtrlBaseAddr  = 0;

VOS_UINT32  g_ulPppVirtAddr;
VOS_UINT32  g_ulPppPhyAddr;

/******************************************************************************
   5 函数实现
******************************************************************************/


VOS_VOID PPP_HDLC_HARD_PeriphClkOpen(VOS_VOID)
{
#if (SC_CTRL_MOD_6758_SFT == SC_CTRL_MOD)

    VOS_UINT32      ulValue = 0;

    ulValue |= (1 << HDLC_PERIPH_CLK_BITPOS);

    TTF_WRITE_32REG(SOC_SCTRL_SC_PERIPH_CLKEN0_ADDR(g_ulHdlcScCtrlBaseAddr), ulValue);

#elif (SC_CTRL_MOD_6930_SFT == SC_CTRL_MOD)

    VOS_UINT32      ulValue = 0;

    ulValue |= (1 << HDLC_CRG_CLK_BITPOS);

    TTF_WRITE_32REG(HDLC_CRG_CLKEN4_ADDR(g_ulHdlcScCtrlBaseAddr), ulValue);

#endif

    return;
}


VOS_VOID PPP_HDLC_HARD_PeriphClkClose(VOS_VOID)
{
#if (SC_CTRL_MOD_6758_SFT == SC_CTRL_MOD)

    VOS_UINT32      ulValue = 0;

    ulValue |= (1 << HDLC_PERIPH_CLK_BITPOS);

    TTF_WRITE_32REG(SOC_SCTRL_C_PERIPH_CLKDIS0_ADDR(g_ulHdlcScCtrlBaseAddr), ulValue);

#elif (SC_CTRL_MOD_6930_SFT == SC_CTRL_MOD)

    VOS_UINT32      ulValue = 0;

    ulValue |= (1 << HDLC_CRG_CLK_BITPOS);

    TTF_WRITE_32REG(HDLC_CRG_CLKENDIS4_ADDR(g_ulHdlcScCtrlBaseAddr), ulValue);

#endif

    return;
}


VOS_UINT32 PPP_HDLC_HARD_MntnSetConfig(VOS_UINT32 ulConfig)
{
    g_stHdlcConfigInfo.ulHdlcMntnTraceCfg = ulConfig;

    return g_stHdlcConfigInfo.ulHdlcMntnTraceCfg;
}


VOS_UINT32 PPP_HDLC_HARD_MntnSetDefIntLimit(VOS_UINT32 ulIntLimit)
{
    g_stHdlcConfigInfo.ulHdlcDefIntLimit = ulIntLimit;

    return g_stHdlcConfigInfo.ulHdlcDefIntLimit;
}


VOS_UINT32 PPP_HDLC_HARD_MntnSetFrmIntLimit(VOS_UINT32 ulIntLimit)
{
    g_stHdlcConfigInfo.ulHdlcFrmIntLimit = ulIntLimit;

    return g_stHdlcConfigInfo.ulHdlcFrmIntLimit;
}


VOS_VOID PPP_HDLC_HARD_MntnShowConfigInfo(VOS_VOID)
{
    /* 输出IP相关信息 */
    PPP_MNTN_LOG3(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                  "HDLC Hardware Info: g_ulHDLCIPBaseAddr 0x%x g_slHdlcISRDef %d g_slHdlcISRFrm %d\r\n",
                  HDLC_IP_BASE_ADDR, g_stHdlcConfigInfo.slHdlcISRDef, g_stHdlcConfigInfo.slHdlcISRFrm);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                  "HDLC Hardware Info: g_ulHdlcDefMasterSem 0x%x g_ulHdlcFrmMasterSem 0x%x\r\n",
                  g_stHdlcConfigInfo.ulHdlcDefMasterSem, g_stHdlcConfigInfo.ulHdlcFrmMasterSem);

    /* 输出内存相关信息 */
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                  "HDLC Memory Info: g_pstHdlcDefBufInfo 0x%x TTF_HDLC_MASTER_DEF_BUF_LEN %d\r\n",
                  g_pstHdlcDefBufInfo, TTF_HDLC_MASTER_DEF_BUF_LEN);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                  "HDLC Memory Info: g_pstHdlcFrmBufInfo 0x%x TTF_HDLC_MASTER_FRM_BUF_LEN %d\r\n",
                  g_pstHdlcFrmBufInfo, TTF_HDLC_MASTER_FRM_BUF_LEN);

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnShowStatInfo(VOS_VOID)
{
    vos_printf("\n================HDLC Hardware STAT INFO Begin==========================\n");

    vos_printf("解封装处理IP类型包个数            = %d\n", g_PppHdlcHardStat.ulDefIpDataProcCnt);
    vos_printf("解封装处理PPP类型包个数           = %d\n", g_PppHdlcHardStat.ulDefPppDataProcCnt);
    vos_printf("解封装输出非完整帧个数            = %d\n", g_PppHdlcHardStat.ulDefUncompleteCnt);
    vos_printf("解封装等待中断次数                = %d\n", g_PppHdlcHardStat.ulDefWaitIntCnt);
    vos_printf("解封装轮询次数                    = %d\n", g_PppHdlcHardStat.ulDefWaitQueryCnt);
    vos_printf("解封装中断次数                    = %d\n", g_PppHdlcHardStat.ulDefIsrCnt);
    vos_printf("解封装LCP帧暂停次数               = %d\n", g_PppHdlcHardStat.ulDefLcpPauseCnt);
    vos_printf("解封装空间满暂停次数              = %d\n", g_PppHdlcHardStat.ulDefFullPauseCnt);
    vos_printf("解封装丢弃错误数据包个数          = %d\n", g_PppHdlcHardStat.ulDefInputDiscardCnt);

    vos_printf("封装处理IP类型包个数              = %d\n", g_PppHdlcHardStat.ulFrmIpDataProcCnt);
    vos_printf("封装处理PPP类型包个数             = %d\n", g_PppHdlcHardStat.ulFrmPppDataProcCnt);
    vos_printf("封装等待中断次数                  = %d\n", g_PppHdlcHardStat.ulFrmWaitIntCnt);
    vos_printf("封装轮询次数                      = %d\n", g_PppHdlcHardStat.ulFrmWaitQueryCnt);
    vos_printf("封装中断次数                      = %d\n", g_PppHdlcHardStat.ulFrmIsrCnt);
    vos_printf("封装申请目的空间内存失败次数      = %d\n", g_PppHdlcHardStat.ulFrmAllocOutputMemFailCnt);
    vos_printf("封装申请第一个目的空间内存失败次数= %d\n", g_PppHdlcHardStat.ulFrmAllocFirstMemFailCnt);
    vos_printf("封装输出参数链表满次数            = %d\n", g_PppHdlcHardStat.ulFrmOutputLinkFullCnt);
    vos_printf("封装丢弃错误数据包个数            = %d\n", g_PppHdlcHardStat.ulFrmInputDiscardCnt);

    vos_printf("解封装输入链表最大节点数          = %d\n", g_PppHdlcHardStat.ulDefMaxInputCntOnce);
    vos_printf("解封装输入链表最大总长度          = %d\n", g_PppHdlcHardStat.ulDefMaxInputSizeOnce);
    vos_printf("解封装输出有效帧最大个数          = %d\n", g_PppHdlcHardStat.ulDefMaxValidCntOnce);
    vos_printf("解封装轮询最大次数                = %d\n", g_PppHdlcHardStat.ulDefMaxQueryCnt);

    vos_printf("封装输入链表最大节点数            = %d\n", g_PppHdlcHardStat.ulFrmMaxInputCntOnce);
    vos_printf("封装输入链表最大总长度            = %d\n", g_PppHdlcHardStat.ulFrmMaxInputSizeOnce);
    vos_printf("封装输出使用最大节点个数          = %d\n", g_PppHdlcHardStat.ulFrmMaxOutputCntOnce);
    vos_printf("封装输出使用最大节点总长度        = %d\n", g_PppHdlcHardStat.ulFrmMaxOutputCntOnce);
    vos_printf("封装轮询最大次数                  = %d\n", g_PppHdlcHardStat.ulFrmMaxQueryCnt);

    vos_printf("单次处理最大节点数                = %d\n", g_PppHdlcHardStat.ulMaxCntOnce);
    vos_printf("处理总节点数                      = %d\n", g_PppHdlcHardStat.ulHdlcHardProcCnt);
    vos_printf("continue次数                      = %d\n", g_PppHdlcHardStat.ulContinueCnt);
    vos_printf("usDefExpInfo标识                  = %d\n", g_PppHdlcHardStat.usDefExpInfo);
    vos_printf("usFrmExpInfo标识                  = %d\n", g_PppHdlcHardStat.usFrmExpInfo);

    vos_printf("规避HDLC BUG不拷贝数据次数        = %d\n", g_PppHdlcHardStat.ulForbidHdlcBugNoCpy);
    vos_printf("规避HDLC BUG拷贝数据次数          = %d\n", g_PppHdlcHardStat.ulForbidHdlcBugCpy);


    vos_printf("================HDLC Hardware STAT INFO End==========================\n");

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnTraceMsg
(
    HDLC_MNTN_TRACE_HEAD_STRU          *pstHead,
    HDLC_MNTN_EVENT_TYPE_ENUM_UINT32    ulMsgname,
    VOS_UINT32                          ulDataLen
)
{
    pstHead->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstHead->ulReceiverPid   = PS_PID_APP_PPP;
    pstHead->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstHead->ulSenderPid     = PS_PID_APP_PPP;
    pstHead->ulLength        = ulDataLen - VOS_MSG_HEAD_LENGTH;

    pstHead->ulMsgname       = ulMsgname;

    PPP_MNTN_TRACE_MSG(pstHead);

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnTraceSingleData
(
    VOS_UINT16                          usDataLen,
    VOS_UINT8                          *pucDataAddr,
    HDLC_MNTN_EVENT_TYPE_ENUM_UINT32    ulEventType,
    VOS_UINT32                          ulNodeIndex
)
{
    VOS_UINT32                          ulDataLen;
    HDLC_MNTN_NODE_DATA_STRU           *pstNodeData;
    VOS_UINT32                          ulAllocDataLen;


    ulAllocDataLen = TTF_MIN(usDataLen, HDLC_MNTN_ALLOC_MEM_MAX_SIZE);

    /* 消息长度等于消息结构大小加数据内容长度 */
    ulDataLen   = ulAllocDataLen + sizeof(HDLC_MNTN_NODE_DATA_STRU);

    pstNodeData = (HDLC_MNTN_NODE_DATA_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP, ulDataLen);

    if (VOS_NULL_PTR == pstNodeData)
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP_HDLC_HARD_MntnTraceSingleData, NORMAL, Alloc mem failed, ulEventType %d!\r\n",
                      ulEventType);
        return;
    }

    /* 用于标识这是一组输入链表中的第几个IP包 */
    pstNodeData->usNodeIndex = (VOS_UINT16)ulNodeIndex;
    pstNodeData->usLen       = usDataLen;

    PPP_MemSingleCopy((VOS_UINT8 *)(pstNodeData + 1), pucDataAddr, ulAllocDataLen);

    PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstNodeData,
                               ulEventType, ulDataLen);

    PS_MEM_FREE(PS_PID_APP_PPP, pstNodeData);

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnTraceInputParaLink
(
    HDLC_MNTN_EVENT_TYPE_ENUM_UINT32    ulEventType,
    VOS_UINT32                          ulLinkNodeCnt,
    VOS_UINT32                          ulLinkTotalSize,
    HDLC_PARA_LINK_NODE_STRU           *pastLinkNodeBuf
)
{
    HDLC_MNTN_INPUT_PARA_LINK_STRU      stInputPara;
    HDLC_MNTN_INPUT_PARA_LINK_STRU     *pstInputPara = &stInputPara;
    VOS_UINT32                          ulDataLen;


    ulDataLen = sizeof(HDLC_MNTN_INPUT_PARA_LINK_STRU);

    /* 记录并上报参数链表所有节点的信息*/
    pstInputPara->ulInputLinkNodeCnt   = ulLinkNodeCnt;
    pstInputPara->ulInputLinkTotalSize = ulLinkTotalSize;

    /* 参数链表每个节点的内容 */
    VOS_MemCpy((VOS_UINT8 *)(&pstInputPara->astInputParaLinkNodeBuf[0]),
               (VOS_UINT8 *)(pastLinkNodeBuf),
               ulLinkNodeCnt * sizeof(HDLC_PARA_LINK_NODE_STRU));

    PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstInputPara,
                               ulEventType, ulDataLen);

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnDefTraceInput
(
    HDLC_DEF_BUFF_INFO_STRU         *pstDefBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;


    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        PPP_HDLC_HARD_MntnTraceInputParaLink(ID_HDLC_MNTN_DEF_INPUT_PARA,
                                             pstBuildInfo->ulInputLinkNodeCnt,
                                             pstBuildInfo->ulInputLinkTotalSize,
                                             &(pstDefBuffInfo->astInputParaLinkNodeBuf[0]));
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        /* 记录并上报参数链表所有节点的数据内容，每个节点是一个IP包 */
        for ( ulNodeLoop = 0; ulNodeLoop < pstBuildInfo->ulInputLinkNodeCnt; ulNodeLoop++ )
        {
            pstParaNode = &(pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_DEF_INPUT_DATA, ulNodeLoop);

#if ((FEATURE_OFF == FEATURE_SKB_EXP) || (FEATURE_ON == FEATURE_TTFMEM_CACHE))
            /* 需要将数据写回DDR，HDLC从DDR中读数据 */
            PPP_HDLC_CACHE_FLUSH((VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)), pstParaNode->usDataLen);
#endif
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnDefTraceRegConfig
(
    VOS_UINT32      ulEnable,
    VOS_UINT32      ulValue,
    VOS_UINT32      ulEnableInterrupt
)
{
    HDLC_MNTN_DEF_REG_CONFIG_STRU           stRegConfig;
    HDLC_MNTN_DEF_REG_CONFIG_STRU          *pstRegConfig = &stRegConfig;
    VOS_UINT32                              ulDataLen;

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_REG) != 0)
    {
        ulDataLen    = sizeof(HDLC_MNTN_DEF_REG_CONFIG_STRU);

        /* 拷贝全部寄存器内容 */
        pstRegConfig->ulStateSwRst             = TTF_READ_32REG(SOC_ARM_HDLC_STATE_SW_RST_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulPriorTimeoutCtrl       = TTF_READ_32REG(SOC_ARM_HDLC_PRIROR_TIMEOUT_CTRL_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulRdErrCurrAddr          = TTF_READ_32REG(SOC_ARM_HDLC_RD_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulWrErrCurrAddr          = TTF_READ_32REG(SOC_ARM_HDLC_WR_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefEn              = TTF_READ_32REG(SOC_ARM_HDLC_DEF_EN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefRawInt          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefIntStatus       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INT_STATUS_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefIntClr          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefCfg             = TTF_READ_32REG(SOC_ARM_HDLC_DEF_CFG_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefUncompletedLen      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_LEN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefUncompletedPro      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_PRO_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefUncompletedAddr     = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefUncompleteStAgo     = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_AGO_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefGoOn            = TTF_READ_32REG(SOC_ARM_HDLC_DEF_GO_ON_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefStatus          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefUncompletStNow      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_NOW_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInLliAddr           = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_LLI_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInPktAddr           = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInPktLen            = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInPktLenMax         = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_MAX_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefOutSpcAddr          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_OUT_SPC_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefOutSpaceDep         = TTF_READ_32REG(SOC_ARM_HDLC_DEF_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefRptAddr             = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RPT_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefRptDep              = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefErrInfor0       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefErrInfor1       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefErrInfor2       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcDefErrInfor3       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_3_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInfoFr1CntAgo       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_AGO_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulDefInfoFr1CntNow       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_NOW_ADDR(HDLC_IP_BASE_ADDR));

        /* 使能前勾包，使能寄存器还没有配置，因为配置之后HDLC会开始工作，会改变其他寄存器的值 */
        if( VOS_FALSE == ulEnable)
        {
            pstRegConfig->ulHdlcDefEn   = ulValue;
            PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstRegConfig,
                                       ID_HDLC_MNTN_DEF_REG_BEFORE_EN, ulDataLen);
        }
        else
        {
            /* 使能后勾包时，如果采用中断方式，则RawInt和Status取g_stHdlcRegSaveInfo保存的值 */
            if( VOS_TRUE == ulEnableInterrupt )
            {
                pstRegConfig->ulHdlcDefRawInt   = g_stHdlcRegSaveInfo.ulHdlcDefRawInt;
                pstRegConfig->ulHdlcDefStatus   = g_stHdlcRegSaveInfo.ulHdlcDefStatus;
            }
            PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstRegConfig,
                                       ID_HDLC_MNTN_DEF_REG_AFTER_EN, ulDataLen);
        }
    }

    return;
}
VOS_VOID PPP_HDLC_HARD_MntnShowDefReg(VOS_VOID)
{
    HDLC_MNTN_DEF_REG_CONFIG_STRU           stRegConfig;
    HDLC_MNTN_DEF_REG_CONFIG_STRU          *pstRegConfig = &stRegConfig;


    /* 拷贝全部寄存器内容 */
    pstRegConfig->ulStateSwRst             = TTF_READ_32REG(SOC_ARM_HDLC_STATE_SW_RST_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulPriorTimeoutCtrl       = TTF_READ_32REG(SOC_ARM_HDLC_PRIROR_TIMEOUT_CTRL_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulRdErrCurrAddr          = TTF_READ_32REG(SOC_ARM_HDLC_RD_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulWrErrCurrAddr          = TTF_READ_32REG(SOC_ARM_HDLC_WR_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefEn              = TTF_READ_32REG(SOC_ARM_HDLC_DEF_EN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefRawInt          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefIntStatus       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INT_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefIntClr          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefCfg             = TTF_READ_32REG(SOC_ARM_HDLC_DEF_CFG_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefUncompletedLen      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_LEN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefUncompletedPro      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_PRO_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefUncompletedAddr     = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefUncompleteStAgo     = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_AGO_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefGoOn            = TTF_READ_32REG(SOC_ARM_HDLC_DEF_GO_ON_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefStatus          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefUncompletStNow      = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_NOW_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInLliAddr           = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_LLI_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInPktAddr           = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInPktLen            = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInPktLenMax         = TTF_READ_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_MAX_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefOutSpcAddr          = TTF_READ_32REG(SOC_ARM_HDLC_DEF_OUT_SPC_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefOutSpaceDep         = TTF_READ_32REG(SOC_ARM_HDLC_DEF_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefRptAddr             = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RPT_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefRptDep              = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefErrInfor0       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefErrInfor1       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefErrInfor2       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcDefErrInfor3       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_ERR_INFO_3_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInfoFr1CntAgo       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_AGO_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulDefInfoFr1CntNow       = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_NOW_ADDR(HDLC_IP_BASE_ADDR));

    vos_printf("\n================HDLC Hardware ShowDefReg Begin==========================\n");

    vos_printf("ulStateSwRst             = 0x%x\n", pstRegConfig->ulStateSwRst);
    vos_printf("ulPriorTimeoutCtrl       = 0x%x\n", pstRegConfig->ulPriorTimeoutCtrl);
    vos_printf("ulRdErrCurrAddr          = 0x%x\n", pstRegConfig->ulRdErrCurrAddr);
    vos_printf("ulWrErrCurrAddr          = 0x%x\n", pstRegConfig->ulWrErrCurrAddr);
    vos_printf("ulHdlcDefEn              = 0x%x\n", pstRegConfig->ulHdlcDefEn);
    vos_printf("ulHdlcDefRawInt          = 0x%x\n", pstRegConfig->ulHdlcDefRawInt);
    vos_printf("ulHdlcDefIntStatus       = 0x%x\n", pstRegConfig->ulHdlcDefIntStatus);
    vos_printf("ulHdlcDefIntClr          = 0x%x\n", pstRegConfig->ulHdlcDefIntClr);
    vos_printf("ulHdlcDefCfg             = 0x%x\n", pstRegConfig->ulHdlcDefCfg);
    vos_printf("ulDefUncompletedLen      = 0x%x\n", pstRegConfig->ulDefUncompletedLen);
    vos_printf("ulDefUncompletedPro      = 0x%x\n", pstRegConfig->ulDefUncompletedPro);
    vos_printf("ulDefUncompletedAddr     = 0x%x\n", pstRegConfig->ulDefUncompletedAddr);
    vos_printf("ulDefUncompleteStAgo     = 0x%x\n", pstRegConfig->ulDefUncompleteStAgo);
    vos_printf("ulHdlcDefGoOn            = 0x%x\n", pstRegConfig->ulHdlcDefGoOn);
    vos_printf("ulHdlcDefStatus          = 0x%x\n", pstRegConfig->ulHdlcDefStatus);
    vos_printf("ulDefUncompletStNow      = 0x%x\n", pstRegConfig->ulDefUncompletStNow);
    vos_printf("ulDefInLliAddr           = 0x%x\n", pstRegConfig->ulDefInLliAddr);
    vos_printf("ulDefInPktAddr           = 0x%x\n", pstRegConfig->ulDefInPktAddr);
    vos_printf("ulDefInPktLen            = 0x%x\n", pstRegConfig->ulDefInPktLen);
    vos_printf("ulDefInPktLenMax         = 0x%x\n", pstRegConfig->ulDefInPktLenMax);
    vos_printf("ulDefOutSpcAddr          = 0x%x\n", pstRegConfig->ulDefOutSpcAddr);
    vos_printf("ulDefOutSpaceDep         = 0x%x\n", pstRegConfig->ulDefOutSpaceDep);
    vos_printf("ulDefRptAddr             = 0x%x\n", pstRegConfig->ulDefRptAddr);
    vos_printf("ulDefRptDep              = 0x%x\n", pstRegConfig->ulDefRptDep);
    vos_printf("ulHdlcDefErrInfor0       = 0x%x\n", pstRegConfig->ulHdlcDefErrInfor0);
    vos_printf("ulHdlcDefErrInfor1       = 0x%x\n", pstRegConfig->ulHdlcDefErrInfor1);
    vos_printf("ulHdlcDefErrInfor2       = 0x%x\n", pstRegConfig->ulHdlcDefErrInfor2);
    vos_printf("ulHdlcDefErrInfor3       = 0x%x\n", pstRegConfig->ulHdlcDefErrInfor3);
    vos_printf("ulDefInfoFr1CntAgo       = 0x%x\n", pstRegConfig->ulDefInfoFr1CntAgo);
    vos_printf("ulDefInfoFr1CntNow       = 0x%x\n", pstRegConfig->ulDefInfoFr1CntNow);

    vos_printf("\n================HDLC Hardware ShowDefReg End==========================\n");

}
VOS_VOID PPP_HDLC_HARD_MntnDefTraceUncompleteInfo
(
    HDLC_DEF_UNCOMPLETED_INFO_STRU     *pstUncompletedInfo
)
{
    HDLC_MNTN_DEF_UNCOMPLETED_INFO_STRU stMntnUncompletedInfo;


    VOS_MemCpy(&stMntnUncompletedInfo.stUncompletedInfo,
               pstUncompletedInfo, sizeof(HDLC_DEF_UNCOMPLETED_INFO_STRU));

    PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)&stMntnUncompletedInfo,
                               ID_HDLC_MNTN_DEF_UNCOMPLETED_INFO,
                               sizeof(HDLC_MNTN_DEF_UNCOMPLETED_INFO_STRU));
    return;
}


VOS_VOID PPP_HDLC_HARD_MntnDefTraceOutput
(
    VOS_UINT16                          usValidFrameNum,
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo
)
{
    HDLC_MNTN_DEF_OUTPUT_PARA_STRU     *pstOutputPara;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulNodeLoop;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;
    VOS_UINT16                          usMaxFrameNum;


    /* 解封装上报空间可维可测 */
    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        usMaxFrameNum = (HDLC_MNTN_ALLOC_MEM_MAX_SIZE - sizeof(HDLC_MNTN_DEF_OUTPUT_PARA_STRU)) /
                         sizeof(HDLC_DEF_RPT_NODE_STRU);
        usMaxFrameNum = TTF_MIN(usMaxFrameNum, usValidFrameNum);

        ulDataLen     = sizeof(HDLC_MNTN_DEF_OUTPUT_PARA_STRU) + usMaxFrameNum * sizeof(HDLC_DEF_RPT_NODE_STRU);
        pstOutputPara = (HDLC_MNTN_DEF_OUTPUT_PARA_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP, ulDataLen);

        if (VOS_NULL_PTR == pstOutputPara)
        {
            PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                          "PPP_HDLC_HARD_MntnDefTraceOutput, NORMAL, Alloc mem failed ulDataLen %!\r\n",
                          ulDataLen);
            return;
        }

        pstOutputPara->usDefValidNum = usValidFrameNum;
        pstOutputPara->usTraceNum    = usMaxFrameNum;

        VOS_MemCpy((VOS_UINT8 *)(pstOutputPara + 1),
                   (VOS_UINT8 *)(&(pstDefBuffInfo->astRptNodeBuf[0])),
                   usMaxFrameNum * sizeof(HDLC_DEF_RPT_NODE_STRU));

        PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstOutputPara,
                                   ID_HDLC_MNTN_DEF_OUTPUT_PARA, ulDataLen);

        PS_MEM_FREE(PS_PID_APP_PPP, pstOutputPara);
    }

    /* 解封装目的空间中每个有效帧可维可测 */
    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        for ( ulNodeLoop = 0; ulNodeLoop < usValidFrameNum; ulNodeLoop++ )
        {
            pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstRptNode->usDefOutOneLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstRptNode->pucDefOutOneAddr)),
                                              ID_HDLC_MNTN_DEF_OUTPUT_DATA, ulNodeLoop);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnFrmTraceInput
(
    HDLC_FRM_BUFF_INFO_STRU         *pstFrmBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;


    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        PPP_HDLC_HARD_MntnTraceInputParaLink(ID_HDLC_MNTN_FRM_INPUT_PARA,
                                             pstBuildInfo->ulInputLinkNodeCnt,
                                             pstBuildInfo->ulInputLinkTotalSize,
                                             &(pstFrmBuffInfo->astInputParaLinkNodeBuf[0]));
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        /* 记录并上报参数链表所有节点的数据内容，每个节点是一个IP包 */
        for ( ulNodeLoop = 0; ulNodeLoop < pstBuildInfo->ulInputLinkNodeCnt; ulNodeLoop++ )
        {
            pstParaNode = &(pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_FRM_INPUT_DATA, ulNodeLoop);

#if ((FEATURE_OFF == FEATURE_SKB_EXP) || (FEATURE_ON == FEATURE_TTFMEM_CACHE))
            /* 需要将数据写回DDR，HDLC从DDR中读数据 */
            PPP_HDLC_CACHE_FLUSH((VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)), pstParaNode->usDataLen);
#endif
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnFrmTraceRegConfig
(
    VOS_UINT32      ulEnable,
    VOS_UINT32      ulValue,
    VOS_UINT32      ulEnableInterrupt
)
{
    HDLC_MNTN_FRM_REG_CONFIG_STRU           stRegConfig;
    HDLC_MNTN_FRM_REG_CONFIG_STRU          *pstRegConfig = &stRegConfig;
    VOS_UINT32                              ulDataLen;

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_REG) != 0)
    {
        ulDataLen    = sizeof(HDLC_MNTN_FRM_REG_CONFIG_STRU);

        /* 拷贝全部寄存器内容 */
        pstRegConfig->ulStateSwRst          = TTF_READ_32REG(SOC_ARM_HDLC_STATE_SW_RST_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulPriorTimeoutCtrl    = TTF_READ_32REG(SOC_ARM_HDLC_PRIROR_TIMEOUT_CTRL_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulRdErrCurrAddr       = TTF_READ_32REG(SOC_ARM_HDLC_RD_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulWrErrCurrAddr       = TTF_READ_32REG(SOC_ARM_HDLC_WR_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmEn           = TTF_READ_32REG(SOC_ARM_HDLC_FRM_EN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmRawInt       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmIntStatus    = TTF_READ_32REG(SOC_ARM_HDLC_FRM_INT_STATUS_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmIntClr       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_INT_CLR_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmCfg          = TTF_READ_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmAccm         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulHdlcFrmStatus       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmInLliAddr        = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_LLI_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmInSublliAddr     = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_SUBLLI_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmInPktLen         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_PKT_LEN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmInBlkAddr        = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_BLK_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmInBlkLen         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_BLK_LEN_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmOutLliAddr       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_LLI_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmOutSpaceAddr     = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_SPACE_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmOutSpaceDep      = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmRptAddr          = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RPT_ADDR(HDLC_IP_BASE_ADDR));
        pstRegConfig->ulFrmRptDep           = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR));

        /* 使能前勾包，使能寄存器还没有配置，因为配置之后HDLC会开始工作，会改变其他寄存器的值 */
        if( VOS_FALSE == ulEnable )
        {
            pstRegConfig->ulHdlcFrmEn    = ulValue;
            PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstRegConfig,
                                       ID_HDLC_MNTN_FRM_REG_BEFORE_EN, ulDataLen);
        }
        else
        {
            /* 使能后勾包时，如果采用中断方式，则RawInt和Status取g_stHdlcRegSaveInfo保存的值 */
            if( VOS_TRUE == ulEnableInterrupt )
            {
                pstRegConfig->ulHdlcFrmRawInt   = g_stHdlcRegSaveInfo.ulHdlcFrmRawInt;
                pstRegConfig->ulHdlcFrmStatus   = g_stHdlcRegSaveInfo.ulHdlcFrmStatus;
            }
            PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstRegConfig,
                                       ID_HDLC_MNTN_FRM_REG_AFTER_EN, ulDataLen);
        }
    }

    return;
}
VOS_VOID PPP_HDLC_HARD_MntnShowFrmReg(VOS_VOID)
{
    HDLC_MNTN_FRM_REG_CONFIG_STRU           stRegConfig;
    HDLC_MNTN_FRM_REG_CONFIG_STRU          *pstRegConfig = &stRegConfig;


    /* 拷贝全部寄存器内容 */
    pstRegConfig->ulStateSwRst          = TTF_READ_32REG(SOC_ARM_HDLC_STATE_SW_RST_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulPriorTimeoutCtrl    = TTF_READ_32REG(SOC_ARM_HDLC_PRIROR_TIMEOUT_CTRL_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulRdErrCurrAddr       = TTF_READ_32REG(SOC_ARM_HDLC_RD_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulWrErrCurrAddr       = TTF_READ_32REG(SOC_ARM_HDLC_WR_ERR_CURR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmEn           = TTF_READ_32REG(SOC_ARM_HDLC_FRM_EN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmRawInt       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmIntStatus    = TTF_READ_32REG(SOC_ARM_HDLC_FRM_INT_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmIntClr       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_INT_CLR_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmCfg          = TTF_READ_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmAccm         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulHdlcFrmStatus       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmInLliAddr        = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_LLI_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmInSublliAddr     = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_SUBLLI_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmInPktLen         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_PKT_LEN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmInBlkAddr        = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_BLK_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmInBlkLen         = TTF_READ_32REG(SOC_ARM_HDLC_FRM_IN_BLK_LEN_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmOutLliAddr       = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_LLI_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmOutSpaceAddr     = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_SPACE_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmOutSpaceDep      = TTF_READ_32REG(SOC_ARM_HDLC_FRM_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmRptAddr          = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RPT_ADDR(HDLC_IP_BASE_ADDR));
    pstRegConfig->ulFrmRptDep           = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR));

    vos_printf("\n================HDLC Hardware ShowFrmReg Begin==========================\n");

    vos_printf("ulStateSwRst                    = 0x%x\n", pstRegConfig->ulStateSwRst);
    vos_printf("ulPriorTimeoutCtrl              = 0x%x\n", pstRegConfig->ulPriorTimeoutCtrl);
    vos_printf("ulRdErrCurrAddr                 = 0x%x\n", pstRegConfig->ulRdErrCurrAddr);
    vos_printf("ulWrErrCurrAddr                 = 0x%x\n", pstRegConfig->ulWrErrCurrAddr);
    vos_printf("ulHdlcFrmEn                     = 0x%x\n", pstRegConfig->ulHdlcFrmEn);
    vos_printf("ulHdlcFrmRawInt                 = 0x%x\n", pstRegConfig->ulHdlcFrmRawInt);
    vos_printf("ulHdlcFrmIntStatus              = 0x%x\n", pstRegConfig->ulHdlcFrmIntStatus);
    vos_printf("ulHdlcFrmIntClr                 = 0x%x\n", pstRegConfig->ulHdlcFrmIntClr);
    vos_printf("ulHdlcFrmCfg                    = 0x%x\n", pstRegConfig->ulHdlcFrmCfg);
    vos_printf("ulHdlcFrmAccm                   = 0x%x\n", pstRegConfig->ulHdlcFrmAccm);
    vos_printf("ulHdlcFrmStatus                 = 0x%x\n", pstRegConfig->ulHdlcFrmStatus);
    vos_printf("ulFrmInLliAddr                  = 0x%x\n", pstRegConfig->ulFrmInLliAddr);
    vos_printf("ulFrmInSublliAddr               = 0x%x\n", pstRegConfig->ulFrmInSublliAddr);
    vos_printf("ulFrmInPktLen                   = 0x%x\n", pstRegConfig->ulFrmInPktLen);
    vos_printf("ulFrmInBlkAddr                  = 0x%x\n", pstRegConfig->ulFrmInBlkAddr);
    vos_printf("ulFrmInBlkLen                   = 0x%x\n", pstRegConfig->ulFrmInBlkLen);
    vos_printf("ulFrmOutLliAddr                 = 0x%x\n", pstRegConfig->ulFrmOutLliAddr);
    vos_printf("ulFrmOutSpaceAddr               = 0x%x\n", pstRegConfig->ulFrmOutSpaceAddr);
    vos_printf("ulFrmOutSpaceDep                = 0x%x\n", pstRegConfig->ulFrmOutSpaceDep);
    vos_printf("ulFrmRptAddr                    = 0x%x\n", pstRegConfig->ulFrmRptAddr);
    vos_printf("ulFrmRptDep                     = 0x%x\n", pstRegConfig->ulFrmRptDep);

    vos_printf("\n================HDLC Hardware ShowFrmReg End==========================\n");

    return;
}
VOS_VOID PPP_HDLC_HARD_MntnFrmTraceOutput
(
    VOS_UINT8                           ucFrmValidNum,
    VOS_UINT16                          usFrmOutSegNum,
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU     *pstBuildInfo
)
{
    HDLC_MNTN_FRM_OUTPUT_PARA_STRU      stOutputPara;
    HDLC_MNTN_FRM_OUTPUT_PARA_STRU     *pstOutputPara = &stOutputPara;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        ulDataLen     = sizeof(HDLC_MNTN_FRM_OUTPUT_PARA_STRU);

        pstOutputPara->ulOutputLinkNodeCnt   = pstBuildInfo->ulOutputLinkNodeCnt;
        pstOutputPara->ulOutputLinkTotalSize = pstBuildInfo->ulOutputLinkTotalSize;
        pstOutputPara->ucFrmValidNum         = ucFrmValidNum;
        pstOutputPara->usOutputNodeUsedCnt   = usFrmOutSegNum;

        VOS_MemCpy((VOS_UINT8 *)(&(pstOutputPara->astOutputParaLinkNodeBuf[0])),
                   (VOS_UINT8 *)(&(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0])),
                   TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU));

        VOS_MemCpy((VOS_UINT8 *)(&(pstOutputPara->astRptNodeBuf[0])),
                   (VOS_UINT8 *)(&(pstFrmBuffInfo->astRptNodeBuf[0])),
                   TTF_HDLC_FRM_RPT_MAX_NUM * sizeof(HDLC_FRM_RPT_NODE_STRU));

        PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstOutputPara,
                                        ID_HDLC_MNTN_FRM_OUTPUT_PARA, ulDataLen);
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        for ( ulNodeLoop = 0; ulNodeLoop < usFrmOutSegNum; ulNodeLoop++ )
        {
            pstParaNode = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_FRM_OUTPUT_DATA, ulNodeLoop);
        }
    }

    return;
}



VOS_UINT32 PPP_HDLC_HARD_InitBuf(VOS_VOID)
{
    BSP_DDR_SECT_QUERY   stQuery;
    BSP_DDR_SECT_INFO    stInfo;
    VOS_UINT32           ulBaseAddr;
    VOS_UINT32           ulHdlcMemBaseAddr;

    stQuery.enSectType = BSP_DDR_SECT_TYPE_TTF;
    DRV_GET_FIX_DDR_ADDR(&stQuery, &stInfo);

    /* A核使用虚拟地址 */
    ulBaseAddr = stInfo.ulSectVirtAddr;

    g_ulPppVirtAddr = stInfo.ulSectVirtAddr;
    g_ulPppPhyAddr  = stInfo.ulSectPhysAddr;

    /* HDLC内存还没有独立划分，目前在CIPHER内存后面 */
    ulHdlcMemBaseAddr = TTF_HDLC_MASTER_START_ADDR(ulBaseAddr);

    /* 根据TtfMemoryMap.h初始化HDLC所需内存 */
    g_pstHdlcDefBufInfo = (HDLC_DEF_BUFF_INFO_STRU *)ulHdlcMemBaseAddr;
    g_pstHdlcFrmBufInfo = (HDLC_FRM_BUFF_INFO_STRU *)(ulHdlcMemBaseAddr + sizeof(HDLC_DEF_BUFF_INFO_STRU));

    /*lint -e506 -e774*/
    /* TTF_HDLC_MASTER_DEF_BUF_LEN必须与结构HDLC_DEF_BUFF_INFO_STRU的大小一致 */
    if (TTF_HDLC_MASTER_DEF_BUF_LEN != sizeof(HDLC_DEF_BUFF_INFO_STRU))
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_InitBuf, ERROR, TTF_HDLC_MASTER_DEF_BUF_LEN %d sizeof(HDLC_DEF_BUFF_INFO_STRU) %d\r\n",
                      TTF_HDLC_MASTER_DEF_BUF_LEN, sizeof(HDLC_DEF_BUFF_INFO_STRU));
        return VOS_ERR;
    }

    /* TTF_HDLC_MASTER_FRM_BUF_LEN收必须与结构HDLC_FRM_BUFF_INFO_STRU的大小一致 */
    if (TTF_HDLC_MASTER_FRM_BUF_LEN != sizeof(HDLC_FRM_BUFF_INFO_STRU))
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_InitBuf, ERROR, TTF_HDLC_MASTER_FRM_BUF_LEN %d sizeof(HDLC_FRM_BUFF_INFO_STRU) %d\r\n",
                      TTF_HDLC_MASTER_FRM_BUF_LEN, sizeof(HDLC_FRM_BUFF_INFO_STRU));
        return VOS_ERR;
    }
    /*lint +e506 +e774*/

    return VOS_OK;
}
VOS_UINT32 PPP_HDLC_HARD_Init(VOS_VOID)
{
    VOS_UINT32                          ulBaseAddr;


    /* 获取HDLC基地址 */
    ulBaseAddr      = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_HDLC);

    HDLC_IP_BASE_ADDR = IO_ADDRESS(ulBaseAddr);

#if (VOS_OS_VER == VOS_WIN32)
    g_ulHdlcScCtrlBaseAddr  = g_ucScCtrlRegAddr;
#else
    ulBaseAddr              = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_SYSCTRL);
    g_ulHdlcScCtrlBaseAddr  = IO_ADDRESS(ulBaseAddr);
#endif

    /* 关闭HDLC时钟 */
    PPP_HDLC_HARD_PeriphClkClose();

    /*获取HDLC解封装中断号*/
    g_stHdlcConfigInfo.slHdlcISRDef   = DRV_GET_INT_NO(BSP_INT_TYPE_HDLC_DEF);

    /*获取HDLC封装中断号*/
    g_stHdlcConfigInfo.slHdlcISRFrm   = DRV_GET_INT_NO(BSP_INT_TYPE_HDLC_FRM);

    /* 初始化内存 */
    if (VOS_OK != PPP_HDLC_HARD_InitBuf())
    {
        return VOS_ERR;
    }

    if ( VOS_OK != VOS_SmBCreate("HdlcDefMasterSem", 0, VOS_SEMA4_FIFO, &g_stHdlcConfigInfo.ulHdlcDefMasterSem) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                     "PPP_HDLC_HARD_Init, ERROR, Create g_ulHdlcDefMasterSem failed!\r\n");
        return VOS_ERR;
    }

    if ( VOS_OK != VOS_SmBCreate("HdlcFrmMasterSem", 0, VOS_SEMA4_FIFO, &g_stHdlcConfigInfo.ulHdlcFrmMasterSem) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                     "PPP_HDLC_HARD_Init, ERROR, Create g_ulHdlcFrmMasterSem failed!\r\n");
        return VOS_ERR;
    }

    /* 中断挂接 */
    if (VOS_OK != DRV_VICINT_CONNECT((VOIDFUNCPTR *)g_stHdlcConfigInfo.slHdlcISRDef, (VOIDFUNCPTR)PPP_HDLC_HARD_DefIsr, 0))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_Init, ERROR, Connect slHdlcISRDef %d to PPP_HDLC_HARD_DefIsr failed!\r\n",
                      g_stHdlcConfigInfo.slHdlcISRDef);
        return VOS_ERR;
    }

    /* 中断使能 */
    if (VOS_OK != DRV_VICINT_ENABLE(g_stHdlcConfigInfo.slHdlcISRDef))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_Init, ERROR, Enable slHdlcISRDef %d failed!\r\n",
                      g_stHdlcConfigInfo.slHdlcISRDef);
        return VOS_ERR;
    }

    /* 中断挂接 */
    if (VOS_OK != DRV_VICINT_CONNECT((VOIDFUNCPTR *)g_stHdlcConfigInfo.slHdlcISRFrm, (VOIDFUNCPTR)PPP_HDLC_HARD_FrmIsr, 0))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_Init, ERROR, Connect slHdlcISRFrm %d to PPP_HDLC_HARD_FrmIsr failed!\r\n",
                      g_stHdlcConfigInfo.slHdlcISRFrm);
        return VOS_ERR;
    }

    if (VOS_OK != DRV_VICINT_ENABLE(g_stHdlcConfigInfo.slHdlcISRFrm))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_Init, ERROR, Enable slHdlcISRFrm %d failed!\r\n",
                      g_stHdlcConfigInfo.slHdlcISRFrm);
        return VOS_ERR;
    }

    return VOS_OK;
}    /* link_HDLCInit */

VOS_VOID PPP_HDLC_HARD_SetUp(PPP_ID usPppId)
{
    HDLC_DEF_UNCOMPLETED_INFO_STRU      *pstUncompletedInfo;


    pstUncompletedInfo = HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId);

    VOS_MemSet(pstUncompletedInfo, 0, sizeof(HDLC_DEF_UNCOMPLETED_INFO_STRU));

    return;
}


VOS_VOID PPP_HDLC_HARD_Disable(VOS_VOID)
{
    /* 暂无操作，因为HDLC优化后，一套链表封装或解封装完成时，由硬件自动对frm_en或def_en清零；
       封装或解封装过程出错时，硬件也会自动清零，使内部状态机返回IDLE状态；*/
}


VOS_VOID PPP_HDLC_HARD_CommCfgReg(VOS_VOID)
{
    /*
    prior_timeout_ctrl(0x04)
      31  24   23                16   15  9         8             7   2    1          0
    |--------|----------------------|-------|-------------------|-------|---------------|
    |   Rsv  | axireq_timeout_value |  Rsv  | axireq_timeout_en |  Rsv  |hdlc_prior_ctrl|

    Reserved             [31:24] 8'b0   h/s R/W  保留位。读时返回0。写时无影响。
    axireq_timeout_value [23:16] 8'b0   h/s R/W  软件配置AXI总线读写请求超时的判断值
    Reserved             [15:9]  2'b0   h/s R/W  保留位。读时返回0。写时无影响。
    axireq_timeout_en    [8]     1'b0   h/s R/W  是否允许硬件判断AXI总线读写请求超时，由软件配置：
                                                   0不允许
                                                   1允许
    Reserved             [7:2]   1'b0   h/s R/W  保留位。读时返回0。写时无影响。
    hdlc_prior_ctrl      [1:0]   1'b0   h/s R/W  HDLC封装、解封装优先级配置寄存器：
                                                    00：一个在工作中，另一个也被使能情况下，先将已处于工作中的那个处理完，
                                                        即谁先被使能则先将谁处理完；
                                                    01：下行封装优先级高；
                                                    10：上行解封装优先级高；
                                                    11：无效。
                                                    (HDLC内部控制被暂停者的继续开始工作：当解封装被暂停，封装整套链表被处理完后，
                                                    解封装就开始继续工作；当封装被暂停，解封装整个数据包被处理完后，封装就开始继续工作。)
    */

    VOS_UINT32                          ulValue = 0x0;


    /* 使能AXI请求超时判断，debug时使用，由于HDLC设置超时时间过短，故正常功能模式下不开启 */
/*    SET_BIT_TO_DWORD(ulValue, 8); */

    /* 设置AXI请求超时时长，该值由SoC提供，并且保证无平台差异 */
    ulValue |= (HDLC_AXI_REQ_TIMEOUT_VALUE << 16);

    TTF_WRITE_32REG(SOC_ARM_HDLC_PRIROR_TIMEOUT_CTRL_ADDR(HDLC_IP_BASE_ADDR), ulValue);

    return;
}
VOS_VOID PPP_HDLC_HARD_CommReleaseLink
(
    PPP_ZC_STRU     **ppstLinkNode,
    VOS_UINT32        ulRelCnt
)
{
    VOS_UINT32                          ulNodeLoop;


    for ( ulNodeLoop = 0; ulNodeLoop < ulRelCnt; ulNodeLoop++ )
    {
        PPP_MemFree(ppstLinkNode[ulNodeLoop]);

        ppstLinkNode[ulNodeLoop] = VOS_NULL_PTR;
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_CommWaitSem
(
    VOS_UINT32          ulHdlcMasterSem,
    VOS_UINT32          ulSemTimeoutLen
)
{
    VOS_UINT32                          ulResult;

    /* 等待封装或解封装完成 */
    ulResult = VOS_SmP(ulHdlcMasterSem, ulSemTimeoutLen);

    if (VOS_OK != ulResult)
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_CommWaitSem, WARNING, VOS_SmP ulHdlcMasterSem 0x%x failed! ErrorNo = 0x%x\r\n",
                      ulHdlcMasterSem, ulResult);

        g_PppHdlcHardStat.usDefExpInfo |=   (1 << HDLC_SEM_TIMEOUT_IND_BITPOS);
        g_PppHdlcHardStat.usFrmExpInfo |=   (1 << HDLC_SEM_TIMEOUT_IND_BITPOS);

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 PPP_HDLC_HARD_DefIsEnabled(VOS_VOID)
{
    VOS_UINT32                          ulValue;


    /* SoC会在处理完一套输入链表的时候自动将使能位清零 */
    ulValue = TTF_READ_32REG(SOC_ARM_HDLC_DEF_EN_ADDR(HDLC_IP_BASE_ADDR));

    if (0x01 == (ulValue & 0x01))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}
VOS_UINT32 PPP_HDLC_HARD_DefIsr(unsigned int ulPara)
{
    g_stHdlcRegSaveInfo.ulHdlcDefRawInt = TTF_READ_32REG(SOC_ARM_HDLC_DEF_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    g_stHdlcRegSaveInfo.ulHdlcDefStatus = TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));

    /* 收到一次中断后清除原始中断 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);

    /* 释放封装完成信号量 */
    VOS_SmV(g_stHdlcConfigInfo.ulHdlcDefMasterSem);

    g_PppHdlcHardStat.ulDefIsrCnt++;

    return IRQ_HANDLED;
}


VOS_UINT32 PPP_HDLC_HARD_DefWaitStatusChange()
{
    /*
    hdlc_def_status  (0x88)
      31 30   24 23   8 7 6  5   4   3   2  1  0
    |---|-------|------|---|---|---|---|---|----|
    |Rsv|  Type |  Num |Rsv|Idx|Ful|Ful|Now|Stat|
    Reserved             [31]    1'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_err_type         [30:24] 7'b0    h/s RO   有帧上报时，错误帧类型，对应的bit位为1即表明发生该类型错误：
                                                  bit 30：错误类型6，转义字符0x7D后紧接一个Flag域；
                                                  bit 29：错误类型5，当AC域无压缩时，Address域值非0xFF；
                                                  bit 28：错误类型4，当AC域无压缩时，Control域值非0x03；
                                                  bit 27：错误类型3，当P域需剥离时，收到非法的Protocol域值；
                                                  bit 26：错误类型2，解封装后帧字节数小于4bites；
                                                  bit 25：错误类型1，解封装后帧字节数大于1502bytes（PPP帧的Information域不超过1500Bytes，加上协议域不超过1502Bytes）；
                                                  bit 24：错误类型0， CRC校验错误。
    def_valid_num        [23:8]  16'b0   h/s RO   有帧上报时，有效帧数目；（不包括最后一个可能的非完整帧）
    Reserved             [7:6]   2'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_error_index      [5]     1'b0    h/s RO   解封装发生错误指示
    def_rpt_ful          [4]     1'b0    h/s RO   解封装外部正确帧信息上报空间存满暂停指示
    def_out_spc_ful      [3]     1'b0    h/s RO   解封装外部输出存储空间存满暂停指示
    def_uncompleted_now  [2]     1'b0    h/s RO   用于指示当前链表是否有解出非完整帧，为了支持多个PPP/IP拨号而增加的配置：0，没有；1，有
    def_all_pkt_pro_stat [1:0]   2'b0    h/s RO   一套输入链表处理状态：00：未完成一套输入链表处理；01：未完成一套输入链表处理，已解出LCP帧，硬件处于暂停状态；
                                                  10：完成一套输入链表处理，但无帧上报；11: 完成一套输入链表处理，且有帧上报；
    */
    VOS_UINT32              ulRsltWaitNum;           /* 防止硬件异常的保护变量 */
    volatile VOS_UINT32     ulStatus;                /* 解封装状态 */

   /* 查询hdlc_frm_status (0x28)的第[0]位和第[1]位，任何一个为1或者超时则返回 */

    ulRsltWaitNum = 0UL;

    while (ulRsltWaitNum < HDLC_DEF_MAX_WAIT_RESULT_NUM)
    {
        /* 查询状态寄存器hdlc_def_status (0x88)的0-1和3-5位，任何一位变为1表示解封装模块暂停或停止 */
        ulStatus  =   TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));

        if (HDLC_DEF_STATUS_DOING != (ulStatus & HDLC_DEF_STATUS_MASK))
        {
            break;
        }

        ulRsltWaitNum++;
    }

    if ( HDLC_DEF_MAX_WAIT_RESULT_NUM <= ulRsltWaitNum )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_DefWaitStatusChange, WARNING, wait hdlc_def_status timeout %d status 0x%x!\r\n",
                      ulRsltWaitNum, ulStatus);

        g_PppHdlcHardStat.usDefExpInfo |=   (1 << HDLC_WAIT_STATUS_TIMEOUT_IND_BITPOS);

        return VOS_ERR;
    }

    g_PppHdlcHardStat.ulFrmMaxQueryCnt = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxQueryCnt, ulRsltWaitNum);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_DefWaitResult
(
    VOS_UINT32          ulEnableInterrupt
)
{
    VOS_UINT32                          ulStatus;                /* 解封装状态 */
    VOS_UINT32                          ulResult;


    if (VOS_TRUE == ulEnableInterrupt)
    {
        /* 等待中断得到暂停或完成状态 */
        ulResult = PPP_HDLC_HARD_CommWaitSem(g_stHdlcConfigInfo.ulHdlcDefMasterSem, HDLC_DEF_MASTER_INT_TIMER_LEN);

        /* 由于在中断服务程序中进行了清中断操作，而Status指示是否出错的bit由原始
           中断寄存器决定，故此处取保存在g_stHdlcRegSaveInfo中的状态值 */
        ulStatus = g_stHdlcRegSaveInfo.ulHdlcDefStatus;

    }
    else
    {
        /* 轮询得到暂停或完成 */
        ulResult = PPP_HDLC_HARD_DefWaitStatusChange();

        /* 查询hdlc_def_status (0x88)获取解封装状态并将其返回 */
        ulStatus  =  TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    }

    /* 上报寄存器可维可测 */
    PPP_HDLC_HARD_MntnDefTraceRegConfig(VOS_TRUE, 0, ulEnableInterrupt);

    /* 等不到说明HDLC还在工作 */
    if (VOS_OK != ulResult)
    {
        return HDLC_DEF_STATUS_DOING;
    }

    ulStatus &=  HDLC_DEF_STATUS_MASK;

    return ulStatus;
}


PPP_ZC_STRU * PPP_HDLC_HARD_DefProcRptNode
(
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          usFistSegLen;
    VOS_UINT8                          *pucDefOutOneAddr;


    if ( (0 == pstRptNode->usDefOutOneLen) || (HDLC_DEF_OUT_PER_MAX_CNT < pstRptNode->usDefOutOneLen) )
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_DefProcValidFrames, WARNING, invalid usDefOutOneLen %d\r\n",
                      pstRptNode->usDefOutOneLen);

        return VOS_NULL_PTR;
    }

    pstMem = PPP_MemAlloc(pstRptNode->usDefOutOneLen, PPP_ZC_UL_RESERVE_LEN);

    if (VOS_NULL_PTR == pstMem)
    {
        return VOS_NULL_PTR;
    }

    pucDefOutOneAddr    = (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)pstRptNode->pucDefOutOneAddr);
    /* 判断该帧起始加长度是否超过输出空间尾部，超出后按绕回处理 */
    if ((pucDefOutOneAddr + pstRptNode->usDefOutOneLen) >
        HDLC_DEF_OUTPUT_BUF_END_ADDR)
    {
        if (pucDefOutOneAddr <= HDLC_DEF_OUTPUT_BUF_END_ADDR)
        {
            usFistSegLen = (VOS_UINT32)(HDLC_DEF_OUTPUT_BUF_END_ADDR - pucDefOutOneAddr);

            /* 拷贝从起始地址至输出空间尾部的数据 */
            PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), pucDefOutOneAddr,
                              usFistSegLen);

            /* 拷贝在输出空间首部的剩余数据 */
            PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem) + usFistSegLen, HDLC_DEF_OUTPUT_BUF_START_ADDR,
                              pstRptNode->usDefOutOneLen - usFistSegLen);
        }
        else
        {
            PPP_MemFree(pstMem);

            PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                     "\r\nPPP, PPP_HDLC_HARD_DefProcRptNode, ERROR, Error pucDefOutOneAddr.\r\n", pucDefOutOneAddr, HDLC_DEF_OUTPUT_BUF_END_ADDR);

            PPP_HDLC_HARD_MntnShowDefReg();

            return VOS_NULL_PTR;
        }
    }
    else
    {
        PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), pucDefOutOneAddr,
                          pstRptNode->usDefOutOneLen);
    }

    /* 设置零拷贝数据长度 */
    PPP_ZC_SET_DATA_LEN(pstMem, pstRptNode->usDefOutOneLen);

    return pstMem;
}


VOS_VOID PPP_HDLC_HARD_DefProcValidFrames
(
    VOS_UINT32          ulMode,
        PPP_ID          usPppId,
    struct link        *pstLink
)
{
    VOS_UINT16                          usValidFrameNum;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;
    VOS_UINT32                          ulFrameLoop;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;
    PPP_ZC_STRU                        *pstMem;


    pstDefBuffInfo  = HDLC_DEF_GET_BUF_INFO(usPppId);
    usValidFrameNum = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR), 8, 23);

    /* 上报解封装后数据可维可测:上报空间信息、输出内容 */
    PPP_HDLC_HARD_MntnDefTraceOutput(usValidFrameNum, pstDefBuffInfo);

    /* 有效帧数最大值检查 */
    if (TTF_HDLC_DEF_RPT_MAX_NUM < usValidFrameNum)
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_DefProcValidFrames, WARNING, usValidFrameNum = %d > TTF_HDLC_DEF_RPT_MAX_NUM = %d",
                      usValidFrameNum, TTF_HDLC_DEF_RPT_MAX_NUM);
        return;
    }

    g_PppHdlcHardStat.ulDefMaxValidCntOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxValidCntOnce, usValidFrameNum);

    /* 将目的空间的有效帧数据拷贝至零拷贝内存，根据拨号类型调用上行发数接口 */
    for ( ulFrameLoop = 0 ; ulFrameLoop < usValidFrameNum; ulFrameLoop++ )
    {
        pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulFrameLoop]);

        pstMem = PPP_HDLC_HARD_DefProcRptNode(pstRptNode);

        /* 申请不到内容或上报信息错误，丢弃该有效帧 */
        if (VOS_NULL_PTR == pstMem)
        {
            continue;
        }

        if (HDLC_IP_MODE == ulMode)
        {
            PPP_HDLC_ProcIpModeUlData(pstLink, pstMem, pstRptNode->usDefOutOnePro);
        }
        else
        {
            PPP_HDLC_ProcPppModeUlData(usPppId, pstMem);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_DefProcErrorFrames
(
    struct link        *pstLink
)
{
    VOS_UINT8                           ucErrType;
    VOS_UINT8                           ucMask;
    VOS_UINT32                          ulErrTypeLoop;
    VOS_UINT8                           ucResult;
    HDLC_DEF_ERR_FRAMES_CNT_STRU        stErrCnt;


    /* 查询状态寄存器hdlc_def_status (0x88)的第24:30对应比特位为1表示有某种错误帧输出，
       为0表示无帧输出 */
    ucErrType = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR) , 24, 30);

    if (0 == ucErrType)
    {
        return;
    }

    /*lint -e734*/
    /* get fcs error count */
    stErrCnt.usFCSErrCnt        = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get frame too long error count */
    stErrCnt.usLenLongCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get frame too short error count */
    stErrCnt.usLenShortCnt      = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get error protocol count */
    stErrCnt.usErrProtocolCnt   = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get error control count */
    stErrCnt.usErrCtrlCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get error address count */
    stErrCnt.usErrAddrCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get error flag position count */
    stErrCnt.usFlagPosErrCnt    = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_3_ADDR(HDLC_IP_BASE_ADDR), 0, 15);
    /*lint +e734*/
    for (ulErrTypeLoop = 0UL; ulErrTypeLoop < HDLC_DEF_MAX_TYPE_CNT; ulErrTypeLoop++)
    {
        ucMask   = SET_BITS_VALUE_TO_BYTE(0x01, ulErrTypeLoop);    /* 构造掩码 */
        ucResult = (VOS_UINT8)GET_BITS_FROM_BYTE(ucErrType, ucMask);

        if (0 != ucResult)      /* 存在此类错误 */
        {
            if (0UL == ulErrTypeLoop)   /* 错误类型0: CRC校验错误 */
            {
                pstLink->hdlc.stats.badfcs       += stErrCnt.usFCSErrCnt;
                pstLink->hdlc.lqm.SaveInErrors   += stErrCnt.usFCSErrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc fcs\r\n");
            }
            else if (1UL == ulErrTypeLoop)    /* 错误类型1: 解封装后帧字节数大于1502bytes */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usLenLongCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame length too long\r\n");
            }
            else if (2UL == ulErrTypeLoop)    /* 错误类型2: 解封装后帧字节数小于4bytes */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usLenShortCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame length too short\r\n");
            }
            else if (3UL == ulErrTypeLoop)    /* 错误类型3: 当P域需剥离时, 收到非法的Protocol域值(非*******0 *******1形式) */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usErrProtocolCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame protocol\r\n");
            }
            else if (4UL == ulErrTypeLoop)    /* 错误类型4: 当AC域无压缩时, Control域值非0x03 */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usErrCtrlCnt;
                pstLink->hdlc.stats.badcommand += stErrCnt.usErrCtrlCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame control\r\n");
            }
            else if (5UL == ulErrTypeLoop)    /* 错误类型5: 当AC域无压缩时, Address域值非0xFF */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usErrAddrCnt;
                pstLink->hdlc.stats.badaddr    += stErrCnt.usErrAddrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame address\r\n");
            }
            else if (6UL == ulErrTypeLoop)    /* 错误类型6: 转义字符0x7D后紧接一个Flag域 */
            {
                pstLink->hdlc.lqm.SaveInErrors += stErrCnt.usFlagPosErrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame flag position\r\n");
            }
            else
            {
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "link_HDLCDefDealErr err\r\n");
            }
        }
    }

    return;
}



VOS_VOID PPP_HDLC_HARD_DefCfgGoOnReg
(
    VOS_UINT32          ulDefStatus
)
{
    /*
    hdlc_def_go_on  (0x84)
     31                  17  16  15    9   8  7   1  0
    |----------------------|----|-------|----|-----|----|
    |         Rsv          |Goon|  Rsv  |Goon| Rsv |Goon|
    Reserved             [31:17] 15'b0   h/s R/W  保留位。读时返回0。写时无影响。
    def_rpt_ful_goon     [16]    1'b0    h/s WO   外部解封装有效帧信息上报空间存满暂停解除
    Reserved             [15:9]  7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_outspc_ful_goon  [8]     1'b0    h/s WO   外部解封装输出数据存储空间存满暂停状态清除
    Reserved             [7:1]   7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_lcp_goon         [0]     1'b0    h/s WO   解出一个合法LCP帧导致的硬件暂停状态清除。当解封装模块未处理完一组待解封装的数据（<=2KB(def_in_pkt_len_max)），解出一个合法LCP帧，则会暂停解帧，等待此软件向此寄存器写"1"，再继续处理剩余的数据。
    */

    /* GO_ON前清除上次解封装的原始中断 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);

    if (HDLC_DEF_STATUS_PAUSE_RPT_SPACE_FULL == ulDefStatus )
    {
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_GO_ON_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)0x10000);
    }
    else if (HDLC_DEF_STATUS_PAUSE_OUTPUT_SPACE_FULL == ulDefStatus )
    {
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_GO_ON_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)0x100);
    }
    else if (HDLC_DEF_STATUS_PAUSE_LCP == ulDefStatus )
    {
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_GO_ON_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)0x1);
    }
    else
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_DefCfgGoOnReg, ERROR, Wrong ulDefStatus %d!\r\n",
                      ulDefStatus);
    }

    /* PC上使用HDLC模拟器 */
    #if (VOS_OS_VER == VOS_WIN32)
    VHW_HDLC_Task();
    #endif

    return;
}


VOS_UINT32 PPP_HDLC_HARD_DefCfgReg
(
    VOS_UINT32                      ulMode,
    struct link                    *pstLink,
    HDLC_DEF_UNCOMPLETED_INFO_STRU *pstDefUncompletedInfo
)
{
    /*
        hdlc_def_cfg  (0x70)
         31                           4 3     2   1     0
        |-------------------------------|-------|-----|-----|
        |              Rsv              |  Pfc  | Acfc| ago |
        Reserved             [31:4]  28'b0   h/s R/W  保留位。读时返回0。写时无影响。
        def_pfc              [3:2]   2'b0    h/s R/W  P域压缩指示：00：P域无压缩，需剥离；01：P域压缩，需剥离；11：P域不剥离；其他：无效；
        def_acfc             [1]     1'b0    h/s R/W  AC域压缩指示：0：AC域无压缩；1：表示AC域压缩；
        def_uncompleted_ago  [0]     1'b0    h/s R/W  用于指示对应当前解封装输入链表，同一PPP/IP拨号的以前最近解封装输入链表是否有解出非完整帧，
                                                      为了支持多个PPP/IP拨号而增加的配置：0，没有；1，有
        */

    VOS_UINT32       ulDeframerCfg;
    VOS_UINT8        ucACComp;
    VOS_UINT8        ucPComp;
    VOS_UINT8        ucLowByte;
    VOS_UINT16       usLowWord;

    /* 1.根据ulMode、P域和AC域是否压缩配置hdlc_def_cfg (0x70) */

    /* 获取AC域压缩指示, P域压缩指示 */
    ucACComp = (1 == pstLink->lcp.want_acfcomp) ? 1 : 0;

    if (HDLC_IP_MODE == ulMode) /* IP模式: P域不合入 */
    {
        ucPComp = (1 == pstLink->lcp.want_protocomp)
                   ? HDLC_PROTOCOL_REMOVE_WITH_COMPRESS
                   : HDLC_PROTOCOL_REMOVE_WITHOUT_COMPRESS;
    }
    else                        /* PPP模式: P域合入 */
    {
        ucPComp = HDLC_PROTOCOL_NO_REMOVE;
    }

    /* 将ucPComp设置到一个字节的第2, 3位上 */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, HDLC_DEF_PFC_BITPOS);

    if (1 == ucACComp)  /* AC域压缩 */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_DEF_ACFC_BITPOS);
    }

    /* 2.设置非完整帧相关信息 */
    if ( (VOS_NULL_PTR != pstDefUncompletedInfo) &&
        (HDLC_DEF_UNCOMPLETED_EXIST == pstDefUncompletedInfo->ucExistFlag) )
    {
        /* def_uncompleted_ago置1表示有上次输出的非完整帧参与本次解封装 */
        SET_BIT_TO_BYTE(ucLowByte, HDLC_DEF_IS_UNCOMPLETED_AGO_BITPOS);

        /*
        def_uncompleted_len  (0x74)
         31                 16  15                  0
        |---------------------|----------------------|
        |         Rsv         |         Len          |
        Reserved             [31:16] 16'b0   h/s R/W  保留位。读时返回0。写时无影响。
        def_uncompleted_len  [15:0]  16'b0   h/s R/W  对应当前解封装输入链表，同一PPP/IP拨号的以前最近解封装输入链表解出非完整帧的长度，为了支持多个PPP/IP拨号而增加的配置
        */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_LEN_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->usDefOutOneLen & 0xFFFF);

        /*
        def_uncompleted_pro  (0x78)
         31                 16  15                  0
        |---------------------|----------------------|
        |         Rsv         |         Pro          |
        Reserved             [31:16] 16'b0   h/s R/W  保留位。读时返回0。写时无影响。
        def_uncompleted_pro  [15:0]  16'b0   h/s R/W  对应当前解封装输入链表，同一PPP/IP拨号的以前最近解封装输入链表解出非完整帧的
                                                      协议，为了支持多个PPP/IP拨号而增加的配置（可能其中的0Byte、1Byte或2Byte有效）
        */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_PRO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->usDefOutOnePro & 0xFFFF);

        /*
        def_uncompleted_addr  (0x7C)
         31                  0
        |----------------------|
        |         Addr         |
        def_uncompleted_addr [31:0]  32'b0   h/s R/W  对应当前解封装输入链表，同一PPP/IP拨号的以前最近解封装输入链表解出非完整帧的
                                                      外部存储起始地址，为了支持多个PPP/IP拨号而增加的配置（该地址可能是与原来上报不同的新地址）
        */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->pucDefOutOneAddr);

        /*
        def_uncomplet_st_ago  (0x80)
         31                  16 15             5 4     0
        |----------------------|----------------|-------|
        |         Ago          |       Rsv      |  Ago  |
        crc16_result_ago     [31:16] 16'b0   h/s R/W  与当前解封装输入链表同一PPP/IP拨号的以前最近那套解封装输入链表处理完解出非完整帧时的CRC校验值
        Reserved             [15:5]  11'b0   h/s R/W  保留位。读时返回0。写时无影响。
        def_data_st_curr_ago [4:0]   5'b0    h/s R/W  与当前解封装输入链表同一PPP/IP拨号的以前最近那套解封装输入链表处理完解出非完整帧时的数据状态机当前状态
        */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_AGO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->ulDefStAgo);

        /*
        def_info_frl_cnt_ago  (0xC0)
         31        27 26                 16 15   11 10              0
        |------------|---------------------|-------|-----------------|
        |    Rsv     |         Ago         |  Rsv  |       Ago       |
        Reserved             [31:27] 5'b0    h/s R/W  保留位。读时返回0。写时无影响。
        def_framel_cnt_ago   [26:16] 11'b0   h/s R/W  与当前解封装输入链表同一PPP/IP拨号的以前最近那套解封装输入链表处理完解出非完整帧时的帧长度
        Reserved             [15:11] 5'b0    h/s R/W  保留位。读时返回0。写时无影响。
        def_info_cnt_ago     [10:0]  11'b0   h/s R/W  与当前解封装输入链表同一PPP/IP拨号的以前最近那套解封装输入链表处理完解出非完整帧时的信息长度
        */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_AGO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->ulDefInfoFrlCntAgo);
    }

    usLowWord     = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulDeframerCfg = HDLC_MAKE_DWORD(0x00, usLowWord);

    /* 将配置结果写入寄存器 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_CFG_ADDR(HDLC_IP_BASE_ADDR),ulDeframerCfg);

    /* 设置输入数据最大单包长度,规避HDLC BUG,最大长度+1 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_MAX_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)HDLC_DEF_IN_PER_MAX_CNT);

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_DefSaveUncompletedInfo
(
    PPP_ID          usPppId
)
{
    HDLC_DEF_UNCOMPLETED_INFO_STRU     *pstUncompletedInfo;
    VOS_UINT32                          ulStatus;                /* 解封装状态 */
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;
    VOS_UINT32                          ulValidNum;
    VOS_UINT8                          *pucDefOutOneAddr;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;


    pstDefBuffInfo     = HDLC_DEF_GET_BUF_INFO(usPppId);
    pstUncompletedInfo = HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId);

    ulStatus  =   TTF_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));

    /* 查询状态寄存器hdlc_def_status (0x88)的第2位
       为1表示本次解封装有非完整帧输出，
       为0表示无非完整帧输出 */
    if (0 == (ulStatus & 0x4))
    {
        pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_NOT_EXIST;

        return;
    }

    g_PppHdlcHardStat.ulDefUncompleteCnt++;

    /* def_valid_num        [23:8]  16'b0   h/s RO   有帧上报时，有效帧数目；（不包括最后一个可能的非完整帧） */
    ulValidNum = (ulStatus & 0xFFFF00) >> 8;

    /* 非完整帧的上报信息在有效帧后面，但是不算在有效帧数目内 */
    if (TTF_HDLC_DEF_RPT_MAX_NUM <= ulValidNum)
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                     "PPP_HDLC_HARD_DefSaveUncompletedInfo, ERROR, ulValidNum %d >= TTF_HDLC_DEF_RPT_MAX_NUM %d!\r\n",
                     ulValidNum, TTF_HDLC_DEF_RPT_MAX_NUM);

        pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_NOT_EXIST;

        return;
    }

    /* 有非完整帧时需要读取并保存def_uncomplet_st_now(0x8C)、def_info_frl_cnt_now(0xC4)
       读取并保存上报空间有效帧之后的非完整帧长度、协议和数据地址 */
    pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_EXIST;

    pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulValidNum]);

    pucDefOutOneAddr = (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)pstRptNode->pucDefOutOneAddr);


    if (pucDefOutOneAddr != HDLC_DEF_OUTPUT_BUF_START_ADDR)
    {
        if ((pucDefOutOneAddr - HDLC_DEF_OUTPUT_BUF_START_ADDR) >= pstRptNode->usDefOutOneLen)
        {
            DRV_RT_MEMCPY(HDLC_DEF_OUTPUT_BUF_START_ADDR, pucDefOutOneAddr, pstRptNode->usDefOutOneLen);
        }
        else
        {
            PS_MEM_MOVE(HDLC_DEF_OUTPUT_BUF_START_ADDR, pucDefOutOneAddr, pstRptNode->usDefOutOneLen);
        }
#ifndef PPP_ST_TEST
        pstRptNode->pucDefOutOneAddr = (VOS_UINT8*)PPP_VIRT_TO_PHY((VOS_UINT32)HDLC_DEF_OUTPUT_BUF_START_ADDR);
#endif
    }


    /* 非完整帧的协议、长度和在存储空间的地址，软件只是暂存这些信息，等下解封装的时候再原样配给HDLC */
    pstUncompletedInfo->usDefOutOnePro   = pstRptNode->usDefOutOnePro;
    pstUncompletedInfo->usDefOutOneLen   = pstRptNode->usDefOutOneLen;
    pstUncompletedInfo->pucDefOutOneAddr = pstRptNode->pucDefOutOneAddr;

    /* 软件只是暂存这些信息，等下解封装的时候再原样配给HDLC */
    pstUncompletedInfo->ulDefStAgo         = TTF_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_NOW_ADDR(HDLC_IP_BASE_ADDR));
    pstUncompletedInfo->ulDefInfoFrlCntAgo = TTF_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_NOW_ADDR(HDLC_IP_BASE_ADDR));

    PPP_HDLC_HARD_MntnDefTraceUncompleteInfo(pstUncompletedInfo);

    return;
}


VOS_VOID PPP_HDLC_HARD_DefProcException
(
    VOS_UINT32          ulStatus,
    VOS_UINT32          ulEnableInterrupt
)
{
    VOS_UINT32                          ulRawInt;


    if( VOS_TRUE == ulEnableInterrupt )
    {
        /* 由于在中断服务程序中进行了清中断操作，故此处取保存在g_stHdlcRegSaveInfo中的原始中断寄存器值 */
        ulRawInt                        =   g_stHdlcRegSaveInfo.ulHdlcDefRawInt;
        g_PppHdlcHardStat.usDefExpInfo |=   (1 << HDLC_INTERRUPT_IND_BITPOS);
    }
    else
    {
        ulRawInt  =   TTF_READ_32REG(SOC_ARM_HDLC_DEF_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    }

    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                  "PPP_HDLC_HARD_DefProcException, ERROR, Exception ocurr status 0x%x RAW_INT 0x%x\r\n",
                  ulStatus, ulRawInt);
    PPP_HDLC_HARD_MntnShowFrmReg();
    PPP_HDLC_HARD_MntnShowDefReg();

    g_PppHdlcHardStat.usDefExpInfo |=   (1 << HDLC_EXCEPTION_IND_BITPOS);

    /* 如果HDLC出现异常，则单板异常重启 */
    DRV_SYSTEM_ERROR(HDLC_DEF_SYSTEM_ERROR_ID, __LINE__, (VOS_INT)g_PppHdlcHardStat.usDefExpInfo,
                         (VOS_CHAR *)&g_stHdlcRegSaveInfo,
                         sizeof(HDLC_REG_SAVE_INFO_STRU));

    return;
}


VOS_VOID PPP_HDLC_HARD_DefWaitAndProc
(
    VOS_UINT32          ulMode,
    VOS_UINT32          ulEnableInterrupt,
    PPP_ID              usPppId,
    struct link        *pstLink
)
{
    VOS_UINT32                          ulDefStatus;
    VOS_UINT32                          ulContinue;

    for (; ;)
    {
        /* 使能中断，则等待中断辅程序释放信号量；否则轮询解封装状态寄存器 */
        ulDefStatus = PPP_HDLC_HARD_DefWaitResult(ulEnableInterrupt);

        switch ( ulDefStatus )
        {
            case HDLC_DEF_STATUS_PAUSE_RPT_SPACE_FULL :
            case HDLC_DEF_STATUS_PAUSE_OUTPUT_SPACE_FULL :
                /* 处理有效帧，配置GO_ON寄存器 */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefCfgGoOnReg(ulDefStatus);

                ulContinue = VOS_TRUE;
                g_PppHdlcHardStat.ulDefFullPauseCnt++;
                break;
            case HDLC_DEF_STATUS_PAUSE_LCP :
                /* 处理有效帧和LCP帧，更新配置寄存器，配置GO_ON寄存器 */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefCfgReg(ulMode, pstLink, VOS_NULL_PTR);
                PPP_HDLC_HARD_DefCfgGoOnReg(ulDefStatus);

                ulContinue = VOS_TRUE;
                g_PppHdlcHardStat.ulDefLcpPauseCnt++;
                break;
            case HDLC_DEF_STATUS_DONE_WITHOUT_FRAMES :
                /* 根据非完整帧指示，保存非完整帧信息 */
                PPP_HDLC_HARD_DefSaveUncompletedInfo(usPppId);

                ulContinue = VOS_FALSE;
                break;
            case HDLC_DEF_STATUS_DONE_WITH_FRAMES :
                /* 处理有效帧、错误帧和LCP帧(可能是最后一帧)，根据非完整帧指示，保存非完整帧信息 */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefProcErrorFrames(pstLink);
                PPP_HDLC_HARD_DefSaveUncompletedInfo(usPppId);
                ulContinue = VOS_FALSE;
                break;
            case HDLC_DEF_STATUS_DOING :
            default:
                /* 打印异常日志，挂起PPP任务 */
                PPP_HDLC_HARD_DefProcException(ulDefStatus, ulEnableInterrupt);

                ulContinue = VOS_FALSE;
                break;
        }

        /* 暂停状态需要继续处理，其他状态解封装完成退出 */
        if (VOS_TRUE != ulContinue)
        {
            break;
        }
    }

    g_stHdlcRegSaveInfo.ulHdlcDefRawInt = 0xFFFFFFFF;
    g_stHdlcRegSaveInfo.ulHdlcDefStatus = 0xFFFFFFFF;

    return;
}
PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_DefCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;


    /* 获取待封装数据包类型 */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* 解封装最大输入长度是内存模块允许的最大长度，目前是1536B */
    /* 规避HDLC Bug内存可能扩大1 */
    if ( (0 == ulDataLen) || (HDLC_DEF_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP_HDLC_HARD_DefCheckPara, NORMAL, invalid data length %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}
VOS_VOID PPP_HDLC_HARD_ForbiddenHdlcBug(PPP_ZC_STRU **ppstMem)
{
    PPP_ZC_STRU *pstMem;
    PPP_ZC_STRU *pstTmpMem;
    VOS_UINT8   *pstData;
    VOS_UINT16   usLen;
    const VOS_UINT8    ucEndByte = 0x7e;

    pstMem  = *ppstMem;

    if (VOS_NULL_PTR == pstMem)
    {
        return;
    }

    usLen   = PPP_ZC_GET_DATA_LEN(pstMem);
    pstData = PPP_ZC_GET_DATA_PTR(pstMem);

    if (usLen <= 1)
    {
        return;
    }

    if (ucEndByte != pstData[usLen - 2])
    {
        /* 倒数第二个字节不为7e不用处理 */
        return;
    }
    else if ((usLen >= 3) && (ucEndByte == pstData[usLen - 3]))
    {
        /* 倒数第三个字节为7e不用处理 */
        return;
    }
    else
    {
        /*
            倒数第二个字节为7e,倒数第三个字节不为7e的场景
            将最后一个字节后向移一个字节,同时内存的长度+1

            skb申请的内存结构如下
           |--------data(uslen)--------|----32bytes align(0~31)-----|--skb_share_info(256)----|
        */
        if (PPP_ZC_GET_RESERVE_ROOM(pstMem) > 0)
        {
            INSERT_BYTE_LAST(pstMem, pstData, usLen, ucEndByte);
            g_PppHdlcHardStat.ulForbidHdlcBugNoCpy++;
        }
        else
        {
            /* 申请一个保留1字节的内存 */

            pstTmpMem = PPP_MemAlloc(usLen + 1, 0);
            if (VOS_NULL_PTR != pstTmpMem)
            {
                PPP_MemWriteData(pstTmpMem, pstData, usLen);

                pstData = PPP_ZC_GET_DATA_PTR(pstTmpMem);

                INSERT_BYTE_LAST(pstTmpMem, pstData, usLen, ucEndByte);
                g_PppHdlcHardStat.ulForbidHdlcBugCpy++;
            }

            /* 不管新申请的内存是否为空都传出去,在外面处理 */
            *ppstMem = pstTmpMem;

            /* 将原内存释放 */
            PPP_MemFree(pstMem);
        }
    }
}
VOS_UINT32 PPP_HDLC_HARD_DefBuildInputParaLink
(
    HDLC_PARA_LINK_BUILD_PARA_STRU  *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                              ulLinkLoop;
    PPP_ZC_STRU                            *pstMem;
    PPP_DATA_TYPE_ENUM_UINT8                ucDataType;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32  enCheckResult;
    VOS_UINT16                              usDataLen;
    HDLC_PARA_LINK_NODE_STRU               *pstParaNode;
    VOS_UINT32                              ulNodeIndex;
    HDLC_DEF_BUFF_INFO_STRU                *pstDefBuffInfo;
    HDLC_DEF_UNCOMPLETED_INFO_STRU         *pstUncompletedInfo;
    VOS_UINT32                              ulMaxDataLen1Time = TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE;


    /* 清空输出信息 */
    VOS_MemSet(pstBuildInfo, 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    pstUncompletedInfo = HDLC_DEF_GET_UNCOMPLETED_INFO(pstBuildPara->usPppId);

    if (HDLC_DEF_UNCOMPLETED_EXIST == pstUncompletedInfo->ucExistFlag)
    {
        ulMaxDataLen1Time = TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE - pstUncompletedInfo->usDefOutOneLen;
    }


    /* 根据usPppId找到对应的内存  */
    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(pstBuildPara->usPppId);
    ucDataType     = pstBuildPara->ucDataType;

    for ( ulLinkLoop = 0 ; ulLinkLoop < TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_NUM; ulLinkLoop++ )
    {
        pstMem     = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        /* 还没有到最大节点数，但是队列中已经没有数据 */
        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usDataLen     = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_DefCheckPara(pstMem, usDataLen, ucDataType);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            pstBuildInfo->ulDealCnt++;

            g_PppHdlcHardStat.ulDefInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }


        /* 从PPP队列取出头结点，并插入输入队列 pstBuildInfo->apstInputLinkNode */
        pstMem  = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        /* 规避HDLC BUG,在发现HDLC BUG场景下对原有数据进行替换,
           后面的pstMem为替换后的内存指针,原始内存指针可能已经被释放
         */
        PPP_HDLC_HARD_ForbiddenHdlcBug(&pstMem);

        /* 替换内存的时候申请内存失败,直接处理下一块数据  */
        if (VOS_NULL_PTR == pstMem)
        {
            continue;
        }

        /* 内存可能发生替换,重新取长度 */
        usDataLen = PPP_ZC_GET_DATA_LEN(pstMem);

        /* 一次配链表过程中待封装数据包总长不能超过15KB */
        /* 规避HDLC BUG,后面数据被修改过,可能扩大了1字节,需要放到后面判断 */
        if( pstBuildInfo->ulInputLinkTotalSize + usDataLen  > ulMaxDataLen1Time )
        {
            /* 内存放回队列,下次再处理 */
            PPP_ZC_ENQUEUE_HEAD(pstBuildPara->pstDataQ, pstMem);

            break;
        }

        /* 获取当前要添加节点的下标 */
        ulNodeIndex = pstBuildInfo->ulInputLinkNodeCnt;

        /* 本次要构造的参数节点 */
        pstParaNode = &(pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex]);

        /* 把节点组成链表 */
        if( 0 != ulNodeIndex )
        {
            pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU*)PPP_VIRT_TO_PHY((VOS_UINT32)pstParaNode);
        }

        /* 填写输入参数链表结点相关域 */
        pstParaNode->pucDataAddr = (VOS_UINT8*)PPP_VIRT_TO_PHY((VOS_UINT32)PPP_ZC_GET_DATA_PTR(pstMem));
        pstParaNode->usDataLen   = usDataLen;
        pstParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->apstInputLinkNode[ulNodeIndex] = pstMem;
        pstBuildInfo->ulInputLinkTotalSize          += (VOS_UINT32)usDataLen;
        pstBuildInfo->ulInputLinkNodeCnt++;
        pstBuildInfo->ulDealCnt++;

#if ((FEATURE_OFF == FEATURE_SKB_EXP) || (FEATURE_ON == FEATURE_TTFMEM_CACHE))
        /* 需要将数据写回DDR，HDLC从DDR中读数据 */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));
#endif
    }

    if (0 == pstBuildInfo->ulInputLinkNodeCnt)
    {
        return VOS_ERR;
    }

    /* 上报输入参数链表内容可维可测 */
    PPP_HDLC_HARD_MntnDefTraceInput(pstDefBuffInfo, pstBuildInfo);

    return VOS_OK;
}
VOS_VOID PPP_HDLC_HARD_DefCfgBufReg(HDLC_DEF_BUFF_INFO_STRU *pstDefBuffInfo)
{

    /* 将封装输入参数链表的起始地址配置给寄存器def_in_lli_addr(0x90) */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_IN_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->astInputParaLinkNodeBuf[0])));

    /* 将封装输出参数链表的起始地址配置给寄存器def_out_spc_addr(0xA0) */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_OUT_SPC_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->aucOutputDataBuf[0])));

    /* 将封装输出参数链表的起始地址配置给寄存器def_out_space_dep(0xA4)低16位 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_DEF_OUTPUT_DATA_BUF_LEN & 0xFFFF);

    /* 将封装有效帧结果信息上报空间起始地址配置给寄存器def_rpt_addr(0xA8) */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_RPT_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->astRptNodeBuf[0])));

    /* 将封装有效帧结果信息上报空间深度配置给寄存器def_rpt_dep (0xAC)低16位 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_DEF_RPT_BUF_LEN & 0xFFFF);

    return;
}
VOS_UINT32 PPP_HDLC_HARD_DefCfgEnReg
(
    VOS_UINT32                      ulTotalLen
)
{
    /*
    hdlc_def_en   (0x60)
      31   25 24  23 19 18  17  16  15  14  13  12  11  10   9   8   7   1  0
    |--------|---|-----|---|---|---|---|---|---|---|---|---|---|---|------|---|
    |   Rsv  |en | Rsv |en |en |en |en |en |en |en |en |en |en |en |  Rsv |en |

    Reserved            [31:25] 7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_over_int_en     [24]    1'b0    h/s R/W  一套链表解封装结束中断使能;0：中断禁止;1：中断使能;
    Reserved            [23:19] 5'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_rpt_ful_en      [18]    1'b0    h/s R/W  解封装外部正确帧信息上报空间存满暂停中断使能;0：中断禁止;1：中断使能;
    def_out_spc_ful_en  [17]    1'b0    h/s R/W  解封装外部输出存储空间存满暂停中断使能;0：中断禁止;1：中断使能
    def_lcp_int_en      [16]    1'b0    h/s R/W  解封装解出有效LCP帧暂停中断上报使能;0：中断禁止;1：中断使能
    def_rpt_prm_err_en  [15]    1'b0    h/s R/W  解封装上报空间相关参数错误中断使能;0：中断禁止;1：中断使能
    def_out_prm_err_en  [14]    1'b0    h/s R/W  解封装输出空间相关参数错误中断使能;0：中断禁止;1：中断使能
    def_in_prm_err_en   [13]    1'b0    h/s R/W  解封装输入链表相关参数错误中断使能;0：中断禁止;1：中断使能
    def_cfg_err_en      [12]    1'b0    h/s R/W  解封装协议压缩指示配置错误中断使能;0：中断禁止;1：中断使能
    def_wr_timeout_en   [11]    1'b0    h/s R/W  解封装时AXI总线写请求timeout中断使能;0：中断禁止;1：中断使能
    def_rd_timeout _en  [10]    1'b0    h/s R/W  解封装时AXI总线读请求timeout中断使能;0：中断禁止;1：中断使能
    def_wr_err_en       [9]     1'b0    h/s R/W  解封装时AXI总线写操作错误中断使能;0：中断禁止;1：中断使能
    def_rd_err_en       [8]     1'b0    h/s R/W  解封装时AXI总线读操作错误中断使能;0：中断禁止;1：中断使能
    Reserved            [7:1]   7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    def_en              [0]     1'b0    h/s R/W  一套输入链表解封装使能，软件向def_en写入1'b1启动解封装工作；一套输入链表解封装完成后，由硬件自动对def_en清零；
                                                 解封装过程出错时，硬件也会对def_en自动清零，使内部状态机返回IDLE状态；读该寄存器返回解封装处理状态。
                                                 写时设置一套输入链表解封装使能：0：不使能解封装处理；1：使能解封装处理；
                                                 读时返回一套输入链表解封装处理状态：0：没在进行解封装处理；1：正在进行解封装处理。
    */
    VOS_UINT32          ulEnableInterrupt;
    VOS_UINT32          ulValue;
    const VOS_UINT32    ulInterruptValue    = 0x0107FF01;   /* 使用中断方式时配置使能寄存器的值 */
    const VOS_UINT32    ulPollValue         = 0x01;         /* 使用轮询方式时配置使能寄存器的值 */


    if( ulTotalLen > HDLC_DEF_INTERRUPT_LIMIT )
    {
        /* 配置封装相关使能寄存器hdlc_def_en的[31:0]位为0x0107FF01 */
        ulValue             = ulInterruptValue;
        ulEnableInterrupt   = VOS_TRUE;

        g_PppHdlcHardStat.ulDefWaitIntCnt++;
    }
    else
    {
        /* 配置封装相关使能寄存器hdlc_frm_en的[31:0]位为0x01 */
        ulValue             = ulPollValue;
        ulEnableInterrupt   = VOS_FALSE;

        g_PppHdlcHardStat.ulDefWaitQueryCnt++;
    }

    /* 使能前清除上次封装、解封装的原始中断 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);

    /* 上报寄存器可维可测 */
    PPP_HDLC_HARD_MntnDefTraceRegConfig(VOS_FALSE, ulValue, ulEnableInterrupt);

    /* 使能硬件之前先强制ARM顺序执行结束前面的指针 */
    TTF_FORCE_ARM_INSTUCTION();

    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_EN_ADDR(HDLC_IP_BASE_ADDR), ulValue);

    /* PC上使用HDLC模拟器 */
    #if (VOS_OS_VER == VOS_WIN32)
    VHW_HDLC_Task();
    #endif

    return ulEnableInterrupt;
}


VOS_UINT32 PPP_HDLC_HARD_DefPacket
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;


    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(usPppId);

    stBuildPara.ucDataType = PPP_PULL_PACKET_TYPE;
    stBuildPara.usProtocol = PROTO_IP;
    stBuildPara.pstDataQ   = pstDataQ;

    /* 构造输入参数链表，并上报链表内存可维可测 */
    ulBuildResult = PPP_HDLC_HARD_DefBuildInputParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulDefMaxInputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulDefMaxInputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);

    /* 配置封装、解封装通用寄存器 */
    PPP_HDLC_HARD_CommCfgReg();

    /* 配置内存相关寄存器 */
    PPP_HDLC_HARD_DefCfgBufReg(pstDefBuffInfo);

    /* 配置压缩指示、非完整帧相关信息寄存器 */
    PPP_HDLC_HARD_DefCfgReg(HDLC_IP_MODE, pstLink, HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId));

#if (FEATURE_ON == FEATURE_TTFMEM_CACHE)
    PPP_HDLC_CACHE_FLUSH((VOS_UINT8 *)pstDefBuffInfo, sizeof(HDLC_DEF_BUFF_INFO_STRU));
    PPP_HDLC_CACHE_INVALID((VOS_UINT8 *)pstDefBuffInfo, sizeof(HDLC_DEF_BUFF_INFO_STRU));
#endif

    /* 配置使能寄存器，并上报使能前寄存器可维可测 */
    ulEnableInterrupt = PPP_HDLC_HARD_DefCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* 等待解封装暂停或完成，然后处理输出数据，可能会有多次停等的过程 */
    PPP_HDLC_HARD_DefWaitAndProc(HDLC_IP_MODE, ulEnableInterrupt, usPppId, pstLink);

    /* 释放已解封装完成的数据 */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}
VOS_UINT32 PPP_HDLC_HARD_DefRawData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;


    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(usPppId);

    VOS_MemSet(&stBuildInfo, 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    stBuildPara.ucDataType = PPP_PULL_RAW_DATA_TYPE;
    stBuildPara.usProtocol = PROTO_IP;
    stBuildPara.pstDataQ   = pstDataQ;

    /* 构造输入参数链表，并上报链表内存可维可测 */
    ulBuildResult = PPP_HDLC_HARD_DefBuildInputParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulDefMaxInputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulDefMaxInputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);

    /* 配置封装、解封装通用寄存器 */
    PPP_HDLC_HARD_CommCfgReg();

    /* 配置内存相关寄存器 */
    PPP_HDLC_HARD_DefCfgBufReg(pstDefBuffInfo);

    /* 配置压缩指示、非完整帧相关信息寄存器 */
    PPP_HDLC_HARD_DefCfgReg(HDLC_PPP_MODE, pstLink, HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId));

#if (FEATURE_ON == FEATURE_TTFMEM_CACHE)
    PPP_HDLC_CACHE_FLUSH((VOS_UINT8 *)pstDefBuffInfo, sizeof(HDLC_DEF_BUFF_INFO_STRU));
    PPP_HDLC_CACHE_INVALID((VOS_UINT8 *)pstDefBuffInfo, sizeof(HDLC_DEF_BUFF_INFO_STRU));
#endif

    /* 配置使能寄存器，并上报使能前寄存器可维可测 */
    ulEnableInterrupt = PPP_HDLC_HARD_DefCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* 等待解封装暂停或完成，然后处理输出数据，可能会有多次停等的过程 */
    PPP_HDLC_HARD_DefWaitAndProc(HDLC_PPP_MODE, ulEnableInterrupt, usPppId, pstLink);

    /* 释放已解封装完成的数据 */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}
VOS_UINT32 PPP_HDLC_HARD_FrmIsEnabled(VOS_VOID)
{
    VOS_UINT32                          ulValue;


    /* SoC会在处理完一套输入链表的时候自动将使能位清零 */
    ulValue = TTF_READ_32REG(SOC_ARM_HDLC_FRM_EN_ADDR(HDLC_IP_BASE_ADDR));

    if (0x01 == (ulValue & 0x01))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}
VOS_UINT32 PPP_HDLC_HARD_FrmIsr(unsigned int ulPara)
{
    g_stHdlcRegSaveInfo.ulHdlcFrmRawInt = TTF_READ_32REG(SOC_ARM_HDLC_FRM_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    g_stHdlcRegSaveInfo.ulHdlcFrmStatus = TTF_READ_32REG(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR));

    /* 收到一次中断后清除原始中断 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);

    /* 释放封装完成信号量 */
    VOS_SmV(g_stHdlcConfigInfo.ulHdlcFrmMasterSem);

    g_PppHdlcHardStat.ulFrmIsrCnt++;

    return IRQ_HANDLED;
}


VOS_UINT32 PPP_HDLC_HARD_FrmUpdateLink
(
    VOS_UINT32                      ulAllocMemCnt,
    VOS_UINT32 *                    aulAllocLen,
    PPP_ZC_STRU                   **ppstAllocedMem,
    HDLC_PARA_LINK_BUILD_PARA_STRU *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU *pstBuildInfo
)
{
    VOS_UINT32                          ulMemLoop;
    VOS_UINT32                          ulNodeIndex;
    HDLC_PARA_LINK_NODE_STRU           *pstOutputParaNode;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    /* 根据usPppId找到对应的内存  */
    pstFrmBuffInfo          = HDLC_FRM_GET_BUF_INFO(pstBuildPara->usPppId);

    if( (pstBuildInfo->ulOutputLinkNodeCnt + ulAllocMemCnt) > TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM )
    {
        g_PppHdlcHardStat.ulFrmOutputLinkFullCnt++;

        PPP_HDLC_HARD_CommReleaseLink(ppstAllocedMem, ulAllocMemCnt);

        return VOS_ERR;
    }

    /* 更新目的空间数组 apstOutputLinkNode，用于释放目的空间 */
    VOS_MemCpy(&(pstBuildInfo->apstOutputLinkNode[pstBuildInfo->ulOutputLinkNodeCnt]),
               &(ppstAllocedMem[0]),
               ulAllocMemCnt * sizeof(PPP_ZC_STRU *));

    /* 给封装输出参数链表结点的各个成员赋值 */
    for ( ulMemLoop = 0; ulMemLoop < ulAllocMemCnt; ulMemLoop++ )
    {
        ulNodeIndex = pstBuildInfo->ulOutputLinkNodeCnt;

        pstOutputParaNode = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeIndex]);

        /* 更新上个输入参数链表结点的pstNextNode域 */
        if (0 != ulNodeIndex)
        {
            pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU*)PPP_VIRT_TO_PHY((VOS_UINT32)pstOutputParaNode);
        }

        pstOutputParaNode->pucDataAddr = (VOS_UINT8*)PPP_VIRT_TO_PHY((VOS_UINT32)PPP_ZC_GET_DATA_PTR(ppstAllocedMem[ulMemLoop]));
        pstOutputParaNode->usDataLen   = (VOS_UINT16)aulAllocLen[ulMemLoop];
        pstOutputParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->ulOutputLinkNodeCnt++;
        pstBuildInfo->ulOutputLinkTotalSize += aulAllocLen[ulMemLoop];

#if ((FEATURE_OFF == FEATURE_SKB_EXP) || (FEATURE_ON == FEATURE_TTFMEM_CACHE))
        PPP_HDLC_CACHE_INVALID((VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstOutputParaNode->pucDataAddr)), pstOutputParaNode->usDataLen);
#endif
    }

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmOutputMemAlloc
(
    VOS_UINT16                           usLen,
    HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    VOS_UINT16                  usFrmedMaxLen;                                  /* 封装后的可能的最大长度 */
    VOS_UINT16                  usAllocLen;
    VOS_UINT32                  ulAllocLoop;
    VOS_UINT32                  ulLoopCnt;
    VOS_UINT32                  ulAllocMemCnt;                                  /* 本次申请的内存块数 */
    VOS_UINT32                  aulAllocLen[HDLC_OUTPUT_PARA_LINK_MAX_SIZE];    /* 记录本次申请的各个内存块长度 */
    PPP_ZC_STRU *               apstAllocedMem[HDLC_OUTPUT_PARA_LINK_MAX_SIZE]; /* 记录本次申请的各个内存块指针 */
    PPP_ZC_STRU                *pstMem;


    /* 封装后可能的最大数据长度(2*原始数据长度+13B) */
    usFrmedMaxLen = (VOS_UINT16)HDLC_FRM_GET_MAX_FRAMED_LEN(usLen);
    ulAllocMemCnt = 0;
    ulLoopCnt     = TTF_CEIL(usFrmedMaxLen, PPP_ZC_MAX_DATA_LEN);

    for ( ulAllocLoop = 0 ; ulAllocLoop < ulLoopCnt; ulAllocLoop++ )
    {
        if (ulAllocLoop != (ulLoopCnt - 1) )
        {
            usAllocLen = PPP_ZC_MAX_DATA_LEN;
        }
        else
        {
            usAllocLen = (VOS_UINT16)(usFrmedMaxLen - ulAllocLoop*PPP_ZC_MAX_DATA_LEN);
        }

        pstMem = PPP_MemAlloc(usAllocLen, PPP_ZC_DL_RESERVE_LEN);

        if (VOS_NULL_PTR == pstMem)
        {
            break;
        }

        ulAllocMemCnt++;
        aulAllocLen[ulAllocLoop]    = usAllocLen;
        apstAllocedMem[ulAllocLoop] = pstMem;
    }

    /* 如果有申请内存失败的情况，则释放本次已经申请的内存 */
    if (ulLoopCnt > ulAllocMemCnt)
    {
        PPP_HDLC_HARD_CommReleaseLink(&(apstAllocedMem[0]), ulAllocMemCnt);
        return VOS_ERR;
    }

    return PPP_HDLC_HARD_FrmUpdateLink(ulAllocMemCnt,
                                       &(aulAllocLen[0]), &(apstAllocedMem[0]),
                                       pstBuildPara, pstBuildInfo);
}



VOS_UINT32 PPP_HDLC_HARD_FrmGetProtocol
(
    PPP_ZC_STRU     *pstMem,
    VOS_UINT16      *pusProtocol
)
{
    VOS_UINT8           aucProto[2];
    VOS_UINT32          ulRtn;


    ulRtn = PPP_MemGet(pstMem, 0, aucProto, 2);

    if ( PS_FAIL == ulRtn )
    {
        return VOS_ERR;
    }

    if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[0], 0x01) )
    {
        *pusProtocol = HDLC_MAKE_WORD(0x00, aucProto[0]);
    }
    else
    {
        if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[1], 0x01) )
        {
            *pusProtocol = HDLC_MAKE_WORD(aucProto[0], aucProto[1]);
        }
        else
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                            "PPP_HDLC_HARD_FrmGetProtocol, Warning, PPP mode framer data protocol error\r\n");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_FrmCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType,
    VOS_UINT16                   usProtocol
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;
    VOS_UINT32                   ulRlst;
    VOS_UINT16                   usCurrProtocol;


    /* 获取待封装数据包类型 */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* 待封装数据包长度异常 */
    if ( (0 == ulDataLen) || (HDLC_FRM_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                        "PPP_HDLC_HARD_FrmCheckPara,IP mode framer data len wrong %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* 如果不是PPP模式 */
    if( PPP_PUSH_RAW_DATA_TYPE != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_PASS;
    }

    ulRlst  = PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usCurrProtocol);

    if( VOS_OK != ulRlst )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                        "PPP_HDLC_HARD_FrmCheckPara, Warning, PPP mode get framer data protocol error\r\n");
        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* 保证一次链表配置过程中封装协议值一致 */
    if( usProtocol != usCurrProtocol )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}


VOS_VOID PPP_HDLC_HARD_FrmStartTimer
(
    HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara
)
{
    if( VOS_NULL_PTR != g_stHdlcConfigInfo.stHdlcFrmTimerHandle )
    {
        return;
    }

    if (VOS_OK != VOS_StartRelTimer(&g_stHdlcConfigInfo.stHdlcFrmTimerHandle, PS_PID_APP_PPP,
                     HDLC_FRM_TIME_INTERVAL,
                     pstBuildPara->usPppId,
                     HDLC_FRM_OUTPUT_SPACE_ALLOC_FAIL,
                     VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_0))
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                        "PPP_HDLC_HARD_FrmStartTimer, Warning, start reltimer error\r\n");
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmBuildParaLink
(
 HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara,
 HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
 )
{
    PPP_ZC_STRU                             *pstMem;
    VOS_UINT32                               ulLoop;
    VOS_UINT16                               usUnFrmLen;
    VOS_UINT32                               ulAllocResult;
    HDLC_PARA_LINK_NODE_STRU                *pstParaNode;
    VOS_UINT32                               ulNodeIndex;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32   enCheckResult;
    VOS_UINT32                               ulAllocMemFail;
    HDLC_FRM_BUFF_INFO_STRU                 *pstFrmBuffInfo;


    VOS_MemSet(pstBuildInfo, 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    /* 根据usPppId找到对应的内存  */
    pstFrmBuffInfo = HDLC_FRM_GET_BUF_INFO(pstBuildPara->usPppId);
    ulAllocMemFail = VOS_FALSE;

    for(ulLoop = 0; ulLoop < TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM; ulLoop++)
    {
        pstMem = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usUnFrmLen    = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_FrmCheckPara(pstMem, usUnFrmLen,
                                                   pstBuildPara->ucDataType,
                                                   pstBuildPara->usProtocol);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            pstBuildInfo->ulDealCnt++;

            g_PppHdlcHardStat.ulFrmInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }

        /* 一次配链表过程中待封装数据包总长不能超过15KB */
        if( pstBuildInfo->ulInputLinkTotalSize + usUnFrmLen > TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_SIZE )
        {
            break;
        }

        /* 申请封装目的空间 */
        ulAllocResult = PPP_HDLC_HARD_FrmOutputMemAlloc(usUnFrmLen, pstBuildPara, pstBuildInfo);

        if( VOS_OK != ulAllocResult )
        {
            ulAllocMemFail = VOS_TRUE;

            g_PppHdlcHardStat.ulFrmAllocOutputMemFailCnt++;
            break;
        }

        /* 从PPP队列取出头结点，并插入输入队列 pstBuildInfo->apstInputLinkNode */
        pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        /* 获取当前要添加节点的下标 */
        ulNodeIndex = pstBuildInfo->ulInputLinkNodeCnt;

        /* 本次要构造的参数节点 */
        pstParaNode = &(pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex]);

        /* 填写输入参数链表结点相关域 */
        if( 0 != ulNodeIndex )
        {
            pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU*)PPP_VIRT_TO_PHY((VOS_UINT32)pstParaNode);
        }

        pstParaNode->pucDataAddr = (VOS_UINT8*)PPP_VIRT_TO_PHY((VOS_UINT32)PPP_ZC_GET_DATA_PTR(pstMem));
        pstParaNode->usDataLen   = usUnFrmLen;
        pstParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->apstInputLinkNode[ulNodeIndex] = pstMem;
        pstBuildInfo->ulInputLinkTotalSize          += (VOS_UINT32)usUnFrmLen;
        pstBuildInfo->ulInputLinkNodeCnt++;
        pstBuildInfo->ulDealCnt++;

#if ((FEATURE_OFF == FEATURE_SKB_EXP) || (FEATURE_ON == FEATURE_TTFMEM_CACHE))
        /* 需要将数据写回DDR，HDLC从DDR中读数据 */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));
#endif
    }

    if (0 == pstBuildInfo->ulInputLinkNodeCnt)
    {
        if (VOS_TRUE == ulAllocMemFail)
        {
            /* 等待一段时间后，重新尝试申请内存再封装 */
            PPP_HDLC_HARD_FrmStartTimer(pstBuildPara);

            g_PppHdlcHardStat.ulFrmAllocFirstMemFailCnt++;
        }

        return VOS_ERR;
    }

    /* 报输入输出参数链表内容可维可测 */
    PPP_HDLC_HARD_MntnFrmTraceInput(pstFrmBuffInfo, pstBuildInfo);

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_FrmCfgBufReg(HDLC_FRM_BUFF_INFO_STRU *pstFrmBuffInfo)
{

    /* 将封装输入参数链表的起始地址配置给寄存器frm_in_lli_addr */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_IN_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astInputParaLinkNodeBuf[0])));

    /* 将封装输出参数链表的起始地址配置给寄存器frm_out_lli_addr */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_OUT_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0])));

    /* 将封装有效帧结果信息上报空间起始地址配置给寄存器frm_rpt_addr */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_RPT_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astRptNodeBuf[0])));

    /* 将封装有效帧结果信息上报空间深度配置给寄存器frm_rpt_dep的[15:0]位 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_FRM_RPT_BUF_LEN & 0xFFFF);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmCfgReg
(
    struct link        *pstLink,
    VOS_UINT16          usProtocol
)
{
    /*
    hdlc_frm_cfg   (0x20)
      31              16 15           4 3      2   1      0
    |-------------------|--------------|--------|------|------|
    |      protocol     |      Rsv     |   Pfc  | Acfc |1dor2d|

    frm_protocol         [31:16] 16'b0   h/s R/W  封装协议值。有效协议值规定参见规格列表。
    Reserved             [15:4]  12'b0   h/s R/W  保留位。读时返回0。写时无影响。
    frm_pfc              [3:2]   2'b0    h/s R/W  P域压缩指示：00：硬件模块添加P域，P域无压缩;
                                                               01：硬件模块添加P域，P域压缩;
                                                               11：硬件模块不添加P域;
                                                               其他：无效;
    frm_acfc             [1]     1'b0    h/s R/W  AC域压缩指示：0：AC域无压缩;1：表示AC域压缩;
    frm_in_lli_1dor2d    [0]     1'b0    h/s R/W  封装输入一维或二维链表选择指示寄存器：
                                                                0为一维;1为二维;

    IP模式一定添加P域,PPP模式一定不添加P域
      LCP帧: AC域不压缩，P域不压缩
    */

    VOS_UINT32              ulFrmCfg;


    /* 将寄存器hdlc_frm_cfg的[0]位frm_in_lli_1dor2d配置为0 */
    ulFrmCfg = 0x0;

    /* 配置hdlc_frm_cfg的 P域和 AC域 */
    if (PROTO_LCP != usProtocol)
    {
        if ( 1 == pstLink->lcp.his_acfcomp )
        {
            ulFrmCfg |= (1 << HDLC_FRM_ACFC_BITPOS);
        }

        if ( 1 == pstLink->lcp.his_protocomp )
        {
            ulFrmCfg |= (1 << HDLC_FRM_PFC_BITPOS);
        }

        /* 配置hdlc_frm_accm */
        TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR),
                            pstLink->lcp.his_accmap);
    }
    else
    {
        TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);
    }

    /* 配置寄存器hdlc_frm_cfg的[31:16]位frm_protocol为usProtocol */
    ulFrmCfg |= ( ((VOS_UINT32)usProtocol) << 16 );

    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR), ulFrmCfg);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmRawCfgReg
(
    struct link        *pstLink,
    VOS_UINT16          usProtocol
)
{
    /*
    hdlc_frm_cfg   (0x20)
      31              16 15           4 3      2   1      0
    |-------------------|--------------|--------|------|------|
    |      protocol     |      Rsv     |   Pfc  | Acfc |1dor2d|

    frm_protocol         [31:16] 16'b0   h/s R/W  封装协议值。有效协议值规定参见规格列表。
    Reserved             [15:4]  12'b0   h/s R/W  保留位。读时返回0。写时无影响。
    frm_pfc              [3:2]   2'b0    h/s R/W  P域压缩指示：00：硬件模块添加P域，P域无压缩;
                                                               01：硬件模块添加P域，P域压缩;
                                                               11：硬件模块不添加P域;
                                                               其他：无效;
    frm_acfc             [1]     1'b0    h/s R/W  AC域压缩指示：0：AC域无压缩;1：表示AC域压缩;
    frm_in_lli_1dor2d    [0]     1'b0    h/s R/W  封装输入一维或二维链表选择指示寄存器：
                                                                0为一维;1为二维;

    IP模式一定添加P域,PPP模式一定不添加P域
      LCP帧: AC域不压缩，P域不压缩
    */

    VOS_UINT32              ulFrmCfg;
    /*PS_BOOL_ENUM_UINT8      enPComp;       是否压缩协议字段, 是 - PS_TRUE */
    PS_BOOL_ENUM_UINT8      enACComp;     /* 是否压缩地址和控制字段, 是 - PS_TRUE */
    VOS_UINT32              ulACCM;
    VOS_UINT8               ucACComp;
    VOS_UINT8               ucPComp;
    VOS_UINT8               ucLowByte;
    VOS_UINT16              usLowWord;


    /* 将寄存器hdlc_frm_cfg的[0]位frm_in_lli_1dor2d配置为0 */
    ulFrmCfg    = 0x0;

    if (PROTO_LCP == usProtocol)
    {
        /* LCP帧P不压缩, AC不压缩, ACCM为全转义 */
        /*enPComp     = PS_FALSE; */
        enACComp    = PS_FALSE;
        ulACCM      = 0xFFFFFFFF;
    }
    else
    {
        /* 使用默认值 */
        /*enPComp     = (1 == pstLink->lcp.his_protocomp) ? PS_TRUE : PS_FALSE; */
        enACComp    = (1 == pstLink->lcp.his_acfcomp) ? PS_TRUE : PS_FALSE;
        /* 由于PPP拨号不知道ACCM, 故设置为协议默认值 */
        ulACCM      = 0xFFFFFFFF;
    }

    /* 获取AC域压缩指示, P域压缩指示, 协议值, ACCM */
    ucACComp = (PS_TRUE == enACComp) ? 1 : 0;
    ucPComp = HDLC_PROTOCOL_NO_ADD;

    /* 设置accm */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR),ulACCM);

    /* 将ucPComp设置到一个字节的第2, 3位上 */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, HDLC_FRM_PFC_BITPOS);

    if (1 == ucACComp)  /* AC域压缩 */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_FRM_ACFC_BITPOS);
    }
    else
    {
        CLEAR_BIT_TO_BYTE(ucLowByte, HDLC_FRM_ACFC_BITPOS);
    }

    usLowWord   = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulFrmCfg    = HDLC_MAKE_DWORD(usProtocol, usLowWord);

    /* 设置AC域压缩指示, P域压缩指示, 协议值和使能封装标记, accm */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR),ulFrmCfg);

    return;
}
VOS_UINT32 PPP_HDLC_HARD_FrmCfgEnReg(VOS_UINT32   ulTotalLen)
{
    /*
    1.hdlc_frm_en   (0x10)
      31   25 24  23 18 17  16  15  14  13  12  11  10   9   8  7    1  0
    |--------|---|-----|---|---|---|---|---|---|---|---|---|---|------|---|
    |   Rsv  |en | Rsv |en |en |en |en |en |en |en |en |en |en |  Rsv |en |

    Reserved            [31:25] 7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    frm_over_int_en     [24]    1'b0    h/s R/W  一套链表封装结束中断使能;0：中断禁止;1：中断使能;
    Reserved            [23:18] 6'b0    h/s R/W  保留位。读时返回0。写时无影响。
    frm_rpt_dep_err_en  [17]    1'b0    h/s R/W  封装外部正确帧长度上报空间不足中断使能;0：中断禁止;1：中断使能;
    frm_out_spc_err_en  [16]    1'b0    h/s R/W  封装外部输出存储空间不足中断使能;0：中断禁止;1：中断使能
    frm_rpt_prm_err_en  [15]    1'b0    h/s R/W  封装上报空间相关参数错误中断使能;0：中断禁止;1：中断使能
    frm_out_prm_err_en  [14]    1'b0    h/s R/W  封装输出链表相关参数错误中断使能;0：中断禁止;1：中断使能
    frm_in_prm_err_en   [13]    1'b0    h/s R/W  封装输入链表相关参数错误中断使能;0：中断禁止;1：中断使能
    frm_cfg_err_en      [12]    1'b0    h/s R/W  封装协议及其压缩指示配置错误中断使能;0：中断禁止;1：中断使能
    frm_wr_timeout_en   [11]    1'b0    h/s R/W  封装时AXI总线写请求timeout中断使能;0：中断禁止;1：中断使能
    frm_rd_timeout_en   [10]    1'b0    h/s R/W  封装时AXI总线读请求timeout中断使能;0：中断禁止;1：中断使能
    frm_wr_err_en       [9]     1'b0    h/s R/W  封装时AXI总线写操作错误中断使能;0：中断禁止;1：中断使能
    frm_rd_err_en       [8]     1'b0    h/s R/W  封装时AXI总线读操作错误中断使能;0：中断禁止;1：中断使能
    Reserved            [7:1]   7'b0    h/s R/W  保留位。读时返回0。写时无影响。
    frm_en              [0]     1'b0    h/s R/W  一套链表封装使能，软件向frm_en写入1'b1启动封装工作;一套链表封装完成后，由硬件自动对frm_en清零；
                                                 封装过程出错时，硬件也会对frm_en自动清零，使内部状态机返回IDLE状态；
                                                 写时设置一套链表封装使能;0：不使能封装处理;1：使能封装处理;
                                                 读时返回一套链表封装处理状态;0：没在进行封装处理;1：正在进行封装处理。
    */

    VOS_UINT32          ulEnableInterrupt;
    VOS_UINT32          ulValue;
    const VOS_UINT32    ulInterruptValue    = 0x0103FF01;   /* 使用中断方式时配置使能寄存器的值 */
    const VOS_UINT32    ulPollValue         = 0x01;         /* 使用轮询方式时配置使能寄存器的值 */


    /* 判断待封装数据的总长度，若大于门限则使用中断方式，否则使用轮询方式 */
    if( ulTotalLen > HDLC_FRM_INTERRUPT_LIMIT )
    {
        /* 配置封装相关使能寄存器hdlc_frm_en的[31:0]位为0x0103FF01 */
        ulValue             = ulInterruptValue;
        ulEnableInterrupt   = VOS_TRUE;

        g_PppHdlcHardStat.ulFrmWaitIntCnt++;
    }
    else
    {
        /* 配置封装相关使能寄存器hdlc_frm_en的[31:0]位为0x01 */
        ulValue             = ulPollValue;
        ulEnableInterrupt   = VOS_FALSE;

        g_PppHdlcHardStat.ulFrmWaitQueryCnt++;
    }

    /* 使能前清除上次封装、解封装的原始中断 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEF_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_INT_CLR_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFF);

    /* 上报寄存器可维可测 */
    PPP_HDLC_HARD_MntnFrmTraceRegConfig(VOS_FALSE, ulValue, ulEnableInterrupt);

    /* 使能硬件之前先强制ARM顺序执行结束前面的指针 */
    TTF_FORCE_ARM_INSTUCTION();

    TTF_WRITE_32REG(SOC_ARM_HDLC_FRM_EN_ADDR(HDLC_IP_BASE_ADDR), ulValue);

    /* PC上使用HDLC模拟器 */
    #if (VOS_OS_VER == VOS_WIN32)
    VHW_HDLC_Task();
    #endif

    return ulEnableInterrupt;
}
VOS_UINT32 PPP_HDLC_HARD_FrmWaitStatusChange(VOS_VOID)
{
    VOS_UINT32              ulFrmRsltWaitNum;           /* 防止硬件异常的保护变量 */
    volatile VOS_UINT32     ulFrmStatus;                /* 封装状态 */


   /* 查询hdlc_frm_status (0x28)的第[0]位和第[1]位，任何一个为1或者超时则返回 */
    ulFrmRsltWaitNum = 0UL;

    while (ulFrmRsltWaitNum < HDLC_FRM_MAX_WAIT_RESULT_NUM)
    {
        /* 读取 hdlc_frm_status的[0][1]位 */
        ulFrmStatus  =   TTF_READ_32REG(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR));

        if (HDLC_FRM_ALL_PKT_DOING != (ulFrmStatus & HDLC_FRM_STATUS_MASK))
        {
            break;
        }

        ulFrmRsltWaitNum++;
    }

    if ( HDLC_FRM_MAX_WAIT_RESULT_NUM <= ulFrmRsltWaitNum )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_FrmWaitStatusChange, WARNING, wait hdlc_frm_status timeout %d status 0x%x!\r\n",
                      ulFrmRsltWaitNum, ulFrmStatus);

        g_PppHdlcHardStat.usFrmExpInfo |=   (1 << HDLC_WAIT_STATUS_TIMEOUT_IND_BITPOS);

        return VOS_ERR;
    }

    g_PppHdlcHardStat.ulFrmMaxQueryCnt = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxQueryCnt, ulFrmRsltWaitNum);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmWaitResult
(
    VOS_UINT32          ulEnableInterrupt
)
{
    VOS_UINT32              ulFrmStatus;        /* 封装状态 */
    VOS_UINT32              ulResult;


    if (VOS_TRUE == ulEnableInterrupt)
    {
        /* 等待中断得到出错或完成状态 */
        ulResult = PPP_HDLC_HARD_CommWaitSem(g_stHdlcConfigInfo.ulHdlcFrmMasterSem, HDLC_FRM_MASTER_INT_TIMER_LEN);

        /* 由于在中断服务程序中进行了清中断操作，而Status指示是否出错的bit由原始中断寄存器
           决定，故此处取保存在g_stHdlcRegSaveInfo中的状态值 */
        ulFrmStatus = g_stHdlcRegSaveInfo.ulHdlcFrmStatus;
    }
    else
    {
        /* 轮询得到出错或完成 */
        ulResult = PPP_HDLC_HARD_FrmWaitStatusChange();

        /* 查询hdlc_frm_status (0x28)获取封装状态并将其返回 */
        ulFrmStatus  =  TTF_READ_32REG(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR));
    }

    /* 上报寄存器可维可测 */
    PPP_HDLC_HARD_MntnFrmTraceRegConfig(VOS_TRUE, 0, ulEnableInterrupt);

    /* 等不到说明HDLC还在工作 */
    if (VOS_OK != ulResult)
    {
        return HDLC_FRM_ALL_PKT_DOING;
    }

    ulFrmStatus &=  HDLC_FRM_STATUS_MASK;

    return ulFrmStatus;
}


VOS_VOID PPP_HDLC_HARD_FrmProcValidFrames
(
    PPP_ID                               usPppId,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT16                          usFrmOutSegNum;
    VOS_UINT8                           ucFrmValidNum;
    VOS_UINT32                          ulFrmOutSpaceCnt;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;
    HDLC_PARA_LINK_NODE_STRU           *pstOutputParaLink;
    HDLC_FRM_RPT_NODE_STRU             *pstFrmRptNodeStru;
    PPP_ZC_STRU                       **ppstOutputLinkNode;
    VOS_UINT32                          ucRptSpaceIndex;
    VOS_UINT32                          ulOutputLinkIndex;
    VOS_UINT8                          *pucFrmOutAddr;
    VOS_UINT16                          usFrmOutLen;
    VOS_UINT16                          usDataLen;


    pstFrmBuffInfo     = HDLC_FRM_GET_BUF_INFO(usPppId);                   /* 根据usPppId找到对应的内存  */
    pstFrmRptNodeStru  = &(pstFrmBuffInfo->astRptNodeBuf[0]);              /* 封装上报空间首地址 */
    pstOutputParaLink  = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0]);   /* 封装输出链表首地址 */
    ppstOutputLinkNode = &(pstBuildInfo->apstOutputLinkNode[0]);           /* 封装输出链表节点对应零拷贝内存链表首地址 */
    ulFrmOutSpaceCnt   = pstBuildInfo->ulOutputLinkNodeCnt;                /* 封装输出链表节点的个数 */

    /* 有效帧存放占用片段个数usFrmOutSegNum= hdlc_frm_status寄存器[31:16]位的值 */
    usFrmOutSegNum = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* 有效帧个数usFrmValidNum= hdlc_frm_status寄存器[15:8]位的值 */
    ucFrmValidNum  = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR), 8, 15);

    /* 上报封装后数据可维可测:上报空间信息、输出链表内容 */
    PPP_HDLC_HARD_MntnFrmTraceOutput(ucFrmValidNum, usFrmOutSegNum, pstFrmBuffInfo, pstBuildInfo);

    /* ucFrmValidNum肯定要小于等于使用的内存块数usFrmOutSegNum */
    if( ucFrmValidNum > usFrmOutSegNum )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_valid_num = %d > usFrmOutSegNum = %d",
                      ucFrmValidNum, usFrmOutSegNum);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    /* ucFrmValidNum应该与输入数据相同ulInputLinkNodeCnt，如果解封装出错可能会比后者小 */
    if( ucFrmValidNum > pstBuildInfo->ulInputLinkNodeCnt )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_valid_num = %d > InputLinkNodeCnt = %d",
                      ucFrmValidNum, pstBuildInfo->ulInputLinkNodeCnt);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    /* usFrmOutSegNum使用的内存块数肯定小于等于ulOutputLinkNodeCnt */
    if( usFrmOutSegNum  > ulFrmOutSpaceCnt )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_out_seg_num = %d > OutputLinkNodeCnt = %d",
                      usFrmOutSegNum, ulFrmOutSpaceCnt);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    ucRptSpaceIndex   = 0;
    ulOutputLinkIndex = 0;

    while( ucRptSpaceIndex < ucFrmValidNum )
    {
        pucFrmOutAddr    = (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)pstFrmRptNodeStru[ucRptSpaceIndex].pucFrmOutOneAddr);
        usFrmOutLen      = pstFrmRptNodeStru[ucRptSpaceIndex].usFrmOutOneLen;

        if ( (0 == usFrmOutLen) || (HDLC_FRM_OUT_PER_MAX_CNT < usFrmOutLen) )
        {
            PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                          "PPP_HDLC_HARD_FrmProcValidFrames, ERROR, invalid usFrmOutOneLen %d\r\n",
                          usFrmOutLen);

            /* 释放申请的目的空间 */
            PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                          ulFrmOutSpaceCnt - ulOutputLinkIndex);
            return;
        }

        pstMem      = ppstOutputLinkNode[ulOutputLinkIndex];

        if( pucFrmOutAddr != PPP_ZC_GET_DATA_PTR(pstMem) )
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                              "PPP_HDLC_HARD_FrmProcValidFram, ERROR, SOC copy error!");

            /* 释放申请的目的空间 */
            PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                          ulFrmOutSpaceCnt - ulOutputLinkIndex);
            return;
        }

        while( usFrmOutLen > 0 )
        {
            /* 从pstBuildInfo->apstOutputLinkNode取出头结点 pstMem */
            pstMem    = ppstOutputLinkNode[ulOutputLinkIndex];

            /* 单个输出链表结点所指目的空间的大小 */
            usDataLen = pstOutputParaLink[ulOutputLinkIndex].usDataLen;

            if( usFrmOutLen > usDataLen )
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usDataLen);
                usFrmOutLen -= usDataLen;
            }
            else
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usFrmOutLen);
                usFrmOutLen  = 0;
            }

            PPP_HDLC_ProcDlData(usPppId, pstMem);

            g_PppDataQCtrl.stStat.ulDownlinkSndDataCnt++;

            /* 将发送完的数据包从apstOutputLinkNode中清空 */
            ppstOutputLinkNode[ulOutputLinkIndex] = VOS_NULL_PTR;

            ulOutputLinkIndex++;
        }

        ucRptSpaceIndex++;
    }

    /* 释放剩余未利用的目的空间 */
    PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                  ulFrmOutSpaceCnt - ulOutputLinkIndex);

    return;
}
VOS_VOID PPP_HDLC_HARD_FrmProcException
(
    VOS_UINT32          ulStatus,
    VOS_UINT32          ulEnableInterrupt
)
{
    VOS_UINT32                          ulRawInt;


    if( VOS_TRUE == ulEnableInterrupt )
    {
        /* 由于在中断服务程序中进行了清中断操作，故此处取保存在g_stHdlcRegSaveInfo中的原始中断寄存器值 */
        ulRawInt                        =   g_stHdlcRegSaveInfo.ulHdlcFrmRawInt;
        g_PppHdlcHardStat.usFrmExpInfo |=   (1 << HDLC_INTERRUPT_IND_BITPOS);
    }
    else
    {
        ulRawInt  =   TTF_READ_32REG(SOC_ARM_HDLC_FRM_RAW_INT_ADDR(HDLC_IP_BASE_ADDR));
    }

    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                 "PPP_HDLC_HARD_FrmProcException, ERROR, Exception ocurr status 0x%x RAW_INT 0x%x\r\n",
                 ulStatus, ulRawInt);
    PPP_HDLC_HARD_MntnShowFrmReg();
    PPP_HDLC_HARD_MntnShowDefReg();

    g_PppHdlcHardStat.usFrmExpInfo |=   (1 << HDLC_EXCEPTION_IND_BITPOS);

    /* 复位前先Delay 1s保证可维可测正常输出 */
    VOS_TaskDelay(1000);

    /* 如果HDLC出现异常，则单板异常重启 */
    DRV_SYSTEM_ERROR(HDLC_FRM_SYSTEM_ERROR_ID, __LINE__, (VOS_INT)g_PppHdlcHardStat.usFrmExpInfo,
                         (VOS_CHAR *)&g_stHdlcRegSaveInfo,
                         sizeof(HDLC_REG_SAVE_INFO_STRU));

    return;
}
VOS_VOID PPP_HDLC_HARD_FrmWaitAndProc
(
    VOS_UINT32                           ulEnableInterrupt,
    PPP_ID                               usPppId,
    struct link                         *pstLink,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    VOS_UINT32                          ulFrmStatus;


    /* 使能中断，则等待中断辅程序释放信号量；否则轮询解封装状态寄存器 */
    ulFrmStatus = PPP_HDLC_HARD_FrmWaitResult(ulEnableInterrupt);

    if ( HDLC_FRM_ALL_PKT_DONE == ulFrmStatus )
    {
        /* 处理有效帧，并释放剩余内存 */
        PPP_HDLC_HARD_FrmProcValidFrames(usPppId, pstBuildInfo);
    }
    else
    {
        /* 释放申请的目的空间 */
        PPP_HDLC_HARD_CommReleaseLink(pstBuildInfo->apstOutputLinkNode, pstBuildInfo->ulOutputLinkNodeCnt);

        /* 打印异常日志，挂起PPP任务 */
        PPP_HDLC_HARD_FrmProcException(ulFrmStatus, ulEnableInterrupt);
    }

    g_stHdlcRegSaveInfo.ulHdlcFrmRawInt = 0xFFFFFFFF;
    g_stHdlcRegSaveInfo.ulHdlcFrmStatus = 0xFFFFFFFF;

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmPacket
(
    PPP_ID              usPppId,
    VOS_UINT16          usProtocol,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCurCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    stBuildPara.ucDataType = PPP_PUSH_PACKET_TYPE;
    stBuildPara.usProtocol = usProtocol;
    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = pstDataQ;

    /* 根据usPppId找到对应的内存 */
    pstFrmBuffInfo = HDLC_FRM_GET_BUF_INFO(usPppId);

    /* 构造输入输出参数链表，并上报链表内存可维可测 */
    ulBuildResult  = PPP_HDLC_HARD_FrmBuildParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCurCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                    "PPP_HDLC_HARD_FrmPacket, WARNING, BuildParaLink Fail.\r\n");
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulFrmMaxInputCntOnce   = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxInputSizeOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);
    g_PppHdlcHardStat.ulFrmMaxOutputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputCntOnce, stBuildInfo.ulOutputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce, stBuildInfo.ulOutputLinkTotalSize);

    /* 配置封装、解封装通用寄存器 */
    PPP_HDLC_HARD_CommCfgReg();

    /* 配置内存相关寄存器 */
    PPP_HDLC_HARD_FrmCfgBufReg(pstFrmBuffInfo);

    /* 配置封装相关寄存器 */
    PPP_HDLC_HARD_FrmCfgReg(pstLink, usProtocol);

#if (FEATURE_ON == FEATURE_TTFMEM_CACHE)
    PPP_HDLC_CACHE_FLUSH((VOS_UINT8 *)pstFrmBuffInfo, sizeof(HDLC_FRM_BUFF_INFO_STRU));
    PPP_HDLC_CACHE_INVALID((VOS_UINT8 *)pstFrmBuffInfo, sizeof(HDLC_FRM_BUFF_INFO_STRU));
#endif

    /* 配置使能寄存器，并上报使能前寄存器可维可测 */
    ulEnableInterrupt = PPP_HDLC_HARD_FrmCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* 等待封装完成，然后处理输出数据,释放多余的目的空间内存 */
    PPP_HDLC_HARD_FrmWaitAndProc(ulEnableInterrupt, usPppId, pstLink, &stBuildInfo);

    /* 释放pstBuildInfo->apstInputLinkNode中的结点内存 */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmRawData
(
    PPP_ID              usPppId,
    VOS_UINT16          usProtocol,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCurCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    stBuildPara.ucDataType = PPP_PUSH_RAW_DATA_TYPE;
    stBuildPara.usProtocol = usProtocol;
    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = pstDataQ;

    /* 根据usPppId找到对应的内存  */
    pstFrmBuffInfo         = HDLC_FRM_GET_BUF_INFO(usPppId);

    /* 构造输入输出参数链表，并上报链表内存可维可测 */
    ulBuildResult  = PPP_HDLC_HARD_FrmBuildParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCurCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        return ulBuildResult;
    }


    g_PppHdlcHardStat.ulFrmMaxInputCntOnce   = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxInputSizeOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);
    g_PppHdlcHardStat.ulFrmMaxOutputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputCntOnce, stBuildInfo.ulOutputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce, stBuildInfo.ulOutputLinkTotalSize);

    /* 配置封装、解封装通用寄存器 */
    PPP_HDLC_HARD_CommCfgReg();

    /* 配置内存相关寄存器 */
    PPP_HDLC_HARD_FrmCfgBufReg(pstFrmBuffInfo);

    /* 配置封装相关寄存器 */
    PPP_HDLC_HARD_FrmRawCfgReg(pstLink, usProtocol);

#if (FEATURE_ON == FEATURE_TTFMEM_CACHE)
    PPP_HDLC_CACHE_FLUSH((VOS_UINT8 *)pstFrmBuffInfo, sizeof(HDLC_FRM_BUFF_INFO_STRU));
    PPP_HDLC_CACHE_INVALID((VOS_UINT8 *)pstFrmBuffInfo, sizeof(HDLC_FRM_BUFF_INFO_STRU));
#endif

    /* 配置使能寄存器，并上报使能前寄存器可维可测 */
    ulEnableInterrupt = PPP_HDLC_HARD_FrmCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* 等待封装完成，然后处理输出数据,释放多余的目的空间内存 */
    PPP_HDLC_HARD_FrmWaitAndProc(ulEnableInterrupt, usPppId, pstLink, &stBuildInfo);

    /* 释放pstBuildInfo->apstInputLinkNode中的结点内存 */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


PPP_HDLC_RESULT_TYPE_ENUM_UINT32 PPP_HDLC_HARD_ProcData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ulDealTotalCnt;
    VOS_UINT32                          ulDealCurCnt;
    VOS_UINT32                          ulDealResult;
    VOS_UINT16                          usProtocol;
    PPP_DATA_TYPE_ENUM_UINT8            ucDataType;


    g_PppHdlcHardStat.ulHdlcHardProcCnt++;

    ulDealTotalCnt  = 0;
    ulDealCurCnt    = 0;

    for (;;)
    {
        pstMem  = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstDataQ);

        /* 队列为空的时候返回空指针 */
        if ( VOS_NULL_PTR == pstMem )
        {
            return PPP_HDLC_RESULT_COMM_FINISH;
        }

        /*处理该结点(结点的释放动作已经在各处理函数内部完成，无需再释放结点)*/
        ucDataType   = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);
        ulDealResult = VOS_OK;
        ulDealCurCnt = 0;

        switch ( ucDataType )
        {
            case PPP_PULL_PACKET_TYPE:

                ulDealResult = PPP_HDLC_HARD_DefPacket(usPppId, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;
                break;
            case PPP_PUSH_PACKET_TYPE:

                ulDealResult = PPP_HDLC_HARD_FrmPacket(usPppId, PROTO_IP, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulFrmIpDataProcCnt += ulDealCurCnt;
                break;
            case PPP_PULL_RAW_DATA_TYPE:

                ulDealResult = PPP_HDLC_HARD_DefRawData(usPppId, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;
                break;
            case PPP_PUSH_RAW_DATA_TYPE:

                /* 获取待封装数据包的协议值usProtocol */
                if (VOS_OK == PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usProtocol))
                {
                    ulDealResult = PPP_HDLC_HARD_FrmRawData(usPppId, usProtocol, pstLink, pstDataQ, &ulDealCurCnt);

                    g_PppHdlcHardStat.ulFrmPppDataProcCnt += ulDealCurCnt;
                }
                else
                {
                    pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                    PPP_MemFree(pstMem);
                }
                break;
            default:
                pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                PPP_MemFree(pstMem);
                PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                              "PPP_HDLC_HARD_ProcData, WARNING, ucDataType %d is Abnormal!", ucDataType);
                break;
        }

        /*统计*/
        ulDealTotalCnt += ulDealCurCnt;

        if ( ulDealTotalCnt > g_PppHdlcHardStat.ulMaxCntOnce )
        {
            g_PppHdlcHardStat.ulMaxCntOnce = ulDealTotalCnt;
        }

        if (VOS_OK != ulDealResult)
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                         "PPP_HDLC_HARD_ProcData, WARNING, ulDealResult = ERROR!");
            break;
        }

        /*如果循环处理的结点个数超出了队列一次允许处理最大结点数，
          则退出循环并发送PPP_DATA_PROC_NOTIFY消息*/
        if ( (ulDealTotalCnt >= PPP_ONCE_DEAL_MAX_CNT) )
        {
            g_PppHdlcHardStat.ulContinueCnt++;
            return PPP_HDLC_RESULT_COMM_CONTINUE;
        }
    } /* for (;;) */

    return PPP_HDLC_RESULT_COMM_ERROR;
}
VOS_UINT32 PPP_HDLC_HARD_SendAsFrmPacketMsg
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU    *pstMsg;
    VOS_UINT32                               ulLength;


    ulLength    = sizeof(HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU *)PS_ALLOC_MSG(PS_PID_APP_PPP, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        /*打印出错信息---申请消息包失败:*/
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                        "PPP_HDLC_HARD_SendMsg, ERROR: PS_ALLOC_MSG Failed!\r\n");
        return VOS_ERR;
    }

    /*填写消息头:*/
    pstMsg->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid     = PS_PID_APP_PPP;
    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = PS_PID_APP_PPP;
    pstMsg->MsgHeader.ulLength        = ulLength;

    pstMsg->MsgHeader.ulMsgName       = PPP_HDLC_PROC_AS_FRM_PACKET_IND;
    /*填写消息体:*/
    pstMsg->usPppId                   = usPppId;
    pstMsg->usProtocol                = usProtocol;
    pstMsg->pstMem                    = pstMem;

    /*发送该消息*/
    if (VOS_OK != PS_SEND_MSG(PS_PID_APP_PPP, pstMsg))
    {
        /*打印警告信息---发送消息失败:*/
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                        "PPP_HDLC_HARD_SendAsFrmPacketMsg, ERROR : PS_SEND_MSG Failed!");
        PPP_MemFree(pstMem);
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID PPP_HDLC_HARD_ProcAsFrmPacket
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    VOS_UINT32              ulDealCurCnt;
    PPP_ZC_QUEUE_STRU       stDataQ;
    struct link            *pstLink;
    VOS_UINT32              ulDealResult;

    pstLink = PPP_LINK(usPppId);

    ulDealCurCnt = 0;

    /* 封装成下行IP类型数据包 */
    PPP_ZC_SET_DATA_APP(pstMem, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);

    /* 初始化队列stDataQ, 并将 pstMem 入队 */
    PPP_ZC_QUEUE_INIT(&stDataQ);
    PPP_ZC_ENQUEUE_TAIL(&stDataQ, pstMem);

    /* 直接将该数据包封装后发送给PC */
    ulDealResult = PPP_HDLC_HARD_FrmPacket(usPppId, usProtocol, pstLink, &stDataQ, &ulDealCurCnt);

    if ((VOS_OK != ulDealResult) || (1 != ulDealCurCnt))
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                      "PPP_HDLC_HARD_MakeFrmPacket, ERROR, ulDealResult %d ulDealCurCnt %d!\r\n",
                      ulDealResult, ulDealCurCnt);

        /* 说明因某种错误导致数据没有从队列中移出，需要释放 */
        if (0 < PPP_ZC_GET_QUEUE_LEN(&stDataQ))
        {
            PPP_MemFree(pstMem);
        }

        return;
    }

    g_PppHdlcHardStat.ulFrmIpDataProcCnt++;

    return;
}


VOS_VOID PPP_HDLC_HARD_ProcProtocolPacket
(
    struct link     *pstLink,
    struct ppp_mbuf *pstMbuf,
    VOS_INT32        ulPri,
    VOS_UINT16       usProtocol
)
{
    PPP_ZC_STRU            *pstMem;
    VOS_UINT16              usPppId;
    VOS_UINT32              ulFrmResult;
    VOS_UINT32              ulDefResult;


    usPppId = (VOS_UINT16)PPP_LINK_TO_ID(pstLink);

    pstMem  = ppp_m_alloc_ttfmem_from_mbuf(pstMbuf);

    ppp_m_freem(pstMbuf);
    pstMbuf  = VOS_NULL_PTR;

    if (VOS_NULL_PTR == pstMem)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP_HDLC_HARD_ProcProtocolPacket, WARNING, pstMem is NULL!\r\n");
        return;
    }

    /* PPP协议栈回复的协商包需要封装后发给PC，确认此时是否封装与解封装都已经完成 */
    ulFrmResult     = PPP_HDLC_HARD_FrmIsEnabled();
    ulDefResult     = PPP_HDLC_HARD_DefIsEnabled();

    /* 此函数在上行解出协商包时，PPP协议栈产生应答，这个时候封装不应该使能 */
    if( (VOS_TRUE == ulFrmResult) || (VOS_TRUE == ulDefResult) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_HDLC_HARD_ProcProtocolPacket, WARNING, ulFrmResult %d, ulDefResult %d!\r\n",
                      ulFrmResult, ulDefResult);
        PPP_HDLC_HARD_SendAsFrmPacketMsg(usPppId, usProtocol, pstMem);

        return;
    }

    /* 下次PPP任务调度的时候再封装此协商，那时封装与解封装都未使能 */
    PPP_HDLC_HARD_ProcAsFrmPacket(usPppId, usProtocol, pstMem);

    return;
}

VOID PPP_Help(VOID)
{
    vos_printf("********************PPP软调信息************************\n");
    vos_printf("PPP_HDLC_HARD_MntnShowStatInfo           打印统计信息\n");
    vos_printf("PPP_INPUT_ShowStatInfo                   打印 g_PppDataQCtrl信息\n");
    vos_printf("PPP_HDLC_HARD_MntnSetConfig              设置可维可测等级:\n");
    vos_printf("                                         1--参数；2--寄存器；4--数据\n");
    vos_printf("PPP_HDLC_HARD_MntnShowDefReg             打印解封装寄存器信息\n");
    vos_printf("PPP_HDLC_HARD_MntnShowFrmReg             打印封装寄存器信息\n");
    vos_printf("PPP_HDLC_HARD_MntnSetDefIntLimit         设置解封装中断水线\n");
    vos_printf("PPP_HDLC_HARD_MntnSetFrmIntLimit         设置封装中断水线\n");
    vos_printf("PPP_HDLC_HARD_MntnShowConfigInfo         打印配置信息\n");

    return;
}
VOS_UINT32 PPP_PHY_TO_VIRT(unsigned int ulPAddr)
{
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    if ((ulPAddr >= g_ulPppPhyAddr) && (ulPAddr <= (g_ulPppPhyAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN)))
    {
        return TTF_PHY_TO_VIRT(ulPAddr);
    }
    else
    {
        return phys_to_virt(ulPAddr);
    }
#else
    return TTF_PHY_TO_VIRT(ulPAddr);
#endif
}


VOS_UINT32 PPP_VIRT_TO_PHY(unsigned int ulVAddr)
{
#if(FEATURE_OFF == FEATURE_SKB_EXP)
    if ((ulVAddr >= g_ulPppVirtAddr) && (ulVAddr <= (g_ulPppVirtAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN)))
    {
        return TTF_VIRT_TO_PHY(ulVAddr);
    }
    else
    {
        return virt_to_phys(ulVAddr);
    }
#else
    return TTF_VIRT_TO_PHY(ulVAddr);
#endif
}



#else
/*****************************************************************************
   1 协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_HDLC_HARDWARE_C
/*lint +e767 */

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
#if (VOS_OS_VER == VOS_WIN32)
extern VOS_UINT32 VHW_HDLC_Task( VOS_VOID );
#endif

extern VOS_UINT32 PPP_SendPushedData(VOS_UINT16 usPppId, VOS_UINT8 *pucDataBuf, VOS_UINT16 usLen);
/*****************************************************************************
   3 私有定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulFramerCfg;
    VOS_UINT32 ulFramerAccm;
    VOS_UINT32 ulFramerCtrl;
    VOS_UINT32 ulFramerStatus;
} HDLC_RegFramer_STRU;

typedef struct
{
    VOS_UINT32 ulDeframerCfg;
    VOS_UINT32 ulDeframerCtrl;
    VOS_UINT32 ulDeframerGoOn;
    VOS_UINT32 ulDeframerStatus;
} HDLC_RegDeframer_STRU;

typedef struct
{
    VOS_UINT32 ulFramerCtrl;
    VOS_UINT32 ulFramerStatus;
    VOS_UINT8  ucFramerIn[4];
    VOS_UINT8  ucFramerOut[4];
} HDLC_RegFramerCntInOut_STRU;

typedef struct
{
    VOS_UINT32 ulFramerCfg;
    VOS_UINT32 ulFramerAccm;
    VOS_UINT32 ulFramerGroupCnt;
    HDLC_RegFramerCntInOut_STRU astRegFramerInOut[6];    /* 封装最多6组: 1502 / 256 + 1 */
} HDLC_RegFramerCnt_STRU;

typedef struct
{
    VOS_UINT32 ulDeframerCfg[52];    /* 一组最多解出52帧 */
    VOS_UINT32 ulDeframerCtrl;
    VOS_UINT32 ulDeframerStatus[52];
    VOS_UINT32 ulDeframerGoOnCnt;
} HDLC_RegDeframerCntInOut_STRU;

typedef struct

{
    HDLC_RegDeframerCntInOut_STRU astRegDeframerInOut[16];    /* 解封装最多16组: 4096 /256 */
    VOS_UINT32 ulDeframerGroupCnt;
} HDLC_RegDeframerCnt_STRU;

typedef struct
{
    VOS_UINT32                  ulUplinkIpDataProcCnt;             /* 上行IP数据包处理个数 */
    VOS_UINT32                  ulUplinkPppDataProcCnt;            /* 上行简单PPP帧处理个数 */

    VOS_UINT32                  ulDownlinkIpDataProcCnt;           /* 下行IP数据包处理个数 */
    VOS_UINT32                  ulDownlinkPppDataProcCnt;          /* 下行简单PPP帧处理个数 */

    VOS_UINT32                  ulMaxCntOnce;                      /* PPP一次最多处理的结点个数 */
} PPP_HDLC_HARD_DATA_PROC_STAT_ST;

/*****************************************************************************
   4 全局变量定义
*****************************************************************************/
extern PPP_DATA_Q_CTRL_ST       g_PppDataQCtrl;

VOS_UINT32                      g_ulHDLCIPBaseAddr = 0;

HDLC_RegFramer_STRU             stRegFramer;
HDLC_RegDeframer_STRU           stRegDeframer;
VOS_UINT32                      ulHDLCIsrStatus;
VOS_UINT32                      ulHDLCIsrRawData;

PPP_HDLC_HARD_DATA_PROC_STAT_ST g_PppHdlcHardStat = {0};

/* 申请封装目的空间失败定时器句柄 */
/* HDLC配置相关信息 */
HDLC_CONFIG_INFO_STRU           g_stHdlcConfigInfo  =
{
    0,
    0,
    0,
    0,
    0,
    1000,
    1000,
    VOS_NULL_PTR,
    0
};

VOS_UINT32  g_ulPppVirtAddr;
VOS_UINT32  g_ulPppPhyAddr;


/******************************************************************************
   5 函数实现
******************************************************************************/
VOS_VOID    PPP_PrintHardStatInfo(VOS_VOID)
{
    vos_printf("\n================HDLC Hardware STAT INFO Begin==========================\n");

    vos_printf("处理上行IP类型数据个数    = %d\n", g_PppHdlcHardStat.ulUplinkIpDataProcCnt);
    vos_printf("处理上行PPP类型数据个数   = %d\n", g_PppHdlcHardStat.ulUplinkPppDataProcCnt);
    vos_printf("处理下行IP类型数据个数    = %d\n", g_PppHdlcHardStat.ulDownlinkIpDataProcCnt);
    vos_printf("处理下行PPP类型数据个数   = %d\n", g_PppHdlcHardStat.ulDownlinkPppDataProcCnt);
    vos_printf("单次处理数据最大个数   = %d\n", g_PppHdlcHardStat.ulMaxCntOnce);

    vos_printf("================HDLC Hardware STAT INFO End==========================\n");
}

/*****************************************************************************
 函 数 名  : PPP_HDLC_HARD_Init
 功能描述  : 对HDLC进行初始化: HDLC配置发生异常时, 对异常中断上报的控制
 输入参数  : pstHDLCInterruptCfg - 异常中断上报控制结构
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 PPP_HDLC_HARD_Init(VOS_VOID)
{
    link_HDLCInit();

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_SetUp(PPP_ID usPppId)
{

    return;
}


VOS_VOID PPP_HDLC_HARD_Disable(VOS_VOID)
{
    link_HDLCDisable();
}

/*****************************************************************************
 函 数 名  : link_HDLCInit
 功能描述  : 对HDLC进行初始化: HDLC配置发生异常时, 对异常中断上报的控制
 输入参数  : pstHDLCInterruptCfg - 异常中断上报控制结构
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCInit(VOS_VOID)
{
    HDLC_INTERRUPT_CFG_STRU   stHDLCInterruptCfg;
    HDLC_INTERRUPT_CFG_STRU  *pstHDLCInterruptCfg = &stHDLCInterruptCfg;
    VOS_UINT32                ulInterruptEnable = 0UL;
    VOS_UINT32                ulBaseAddr;

    /*将HDLC使能*/
    SOC_AMR_HDLC_ENABLE();

    ulBaseAddr = DRV_GET_IP_BASE_ADDR(BSP_IP_TYPE_HDLC);

    g_ulHDLCIPBaseAddr = IO_ADDRESS(ulBaseAddr);

    stHDLCInterruptCfg.enFramerErrorEnable = PS_TRUE;
    stHDLCInterruptCfg.enFramerDataLessEnable = PS_TRUE;
    stHDLCInterruptCfg.enFramerDataMoreEnable = PS_TRUE;

    stHDLCInterruptCfg.enDeframerErrorEnable = PS_TRUE;
    stHDLCInterruptCfg.enDeframerDataLessEnable = PS_TRUE;
    stHDLCInterruptCfg.enDeframerDataMoreEnable = PS_TRUE;

    /* 调试时需要打开相应的寄存器 */
    if (PS_TRUE == pstHDLCInterruptCfg->enFramerDataLessEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_FRAMER_DATA_LEN_SHORT_ERR_EN_BITPOS);
    }

    if (PS_TRUE == pstHDLCInterruptCfg->enFramerDataMoreEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_FRAMER_DATA_LEN_LONG_ERR_EN_BITPOS);
    }

    if (PS_TRUE == pstHDLCInterruptCfg->enFramerErrorEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_FRAMER_PARA_ERR_EN_BITPOS);
    }

    if (PS_TRUE == pstHDLCInterruptCfg->enDeframerDataLessEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_DEFRAMER_DATA_LEN_SHORT_ERR_EN_BITPOS);
    }

    if (PS_TRUE == pstHDLCInterruptCfg->enDeframerDataMoreEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_DEFRAMER_DATA_LEN_LONG_ERR_EN_BITPOS);
    }

    if (PS_TRUE == pstHDLCInterruptCfg->enDeframerErrorEnable)
    {
        SET_BIT_TO_DWORD(ulInterruptEnable, HDLC_INTERRUPT_DEFRAMER_PARA_ERR_EN_BITPOS);
    }

    TTF_WRITE_32REG(SOC_ARM_HDLC_INTERRUPT_ENABLE_ADDR(g_ulHDLCIPBaseAddr),ulInterruptEnable);
}    /* link_HDLCInit */


/*****************************************************************************
 函 数 名  : link_ReadFramerLastRecord
 功能描述  : 当HDLC发生异常时, 通过此函数能够将最近一次
             对Framer, Deframer的配置显示出来
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月28日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_ReadFramerLastRecord()
{
    vos_printf("ulHDLCIsrStatus is 0x%x \r\n", (VOS_INT32)ulHDLCIsrStatus);
    vos_printf("ulHDLCIsrRawData is 0x%x \r\n", (VOS_INT32)ulHDLCIsrRawData);
    vos_printf("ulFramerCfg is 0x%x \r\n", (VOS_INT32)stRegFramer.ulFramerCfg);
    vos_printf("ulFramerAccm is 0x%x \r\n", (VOS_INT32)stRegFramer.ulFramerAccm);
    vos_printf("ulFramerCtrl is 0x%x \r\n", (VOS_INT32)stRegFramer.ulFramerCtrl);
    vos_printf("ulFramerStatus is 0x%x \r\n", (VOS_INT32)stRegFramer.ulFramerStatus);

    vos_printf("ulDeframerCfg is 0x%x\r\n", (VOS_INT32)stRegDeframer.ulDeframerCfg);
    vos_printf("ulDeframerCtrl is 0x%x\r\n", (VOS_INT32)stRegDeframer.ulDeframerCtrl);
    vos_printf("ulDeframerStatus is 0x%x\r\n", (VOS_INT32)stRegDeframer.ulDeframerStatus);
}    /* link_ReadFramerLastRecord */

/*****************************************************************************
 函 数 名  : link_HDLCFramerRegRead
 功能描述  : 当HDLC发生异常时, 通过此函数能够获得最近一次对Framer的配置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月28日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCFramerRegRead()
{
    stRegFramer.ulFramerCfg     = TTF_READ_32REG(SOC_ARM_HDLC_FRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr));

    stRegFramer.ulFramerAccm    = TTF_READ_32REG(SOC_ARM_HDLC_FRAMER_ACCM_ADDR(g_ulHDLCIPBaseAddr));

    stRegFramer.ulFramerCtrl    = TTF_READ_32REG(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr));

    stRegFramer.ulFramerStatus  = TTF_READ_32REG(SOC_ARM_HDLC_FRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr));
}    /* link_HDLCFramerRegRead */

/*****************************************************************************
 函 数 名  : link_HDLCDeframerRegRead
 功能描述  : 当HDLC发生异常时, 通过此函数能够获得最近一次对Deframer的配置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月28日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCDeframerRegRead()
{
    stRegDeframer.ulDeframerCfg     = TTF_READ_32REG(SOC_ARM_HDLC_DEFRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr));

    stRegDeframer.ulDeframerCtrl    = TTF_READ_32REG(SOC_ARM_HDLC_DEFRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr));

    stRegDeframer.ulDeframerStatus  = TTF_READ_32REG(SOC_ARM_HDLC_DEFRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr));
}    /* link_HDLCDeframerRegRead */

/*****************************************************************************
 函 数 名  : link_HDLCIsrRegRead
 功能描述  : 当HDLC配置发生异常时, 从HDLC源中断寄存器和中断上报寄存器中
             读取上报了那些类型的配置错误
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 说    明  : 这里的中断将上报配置HDLC时, 发生那些类型的配置错误, 它们被称为
             "源中断错误类型", 它们产生后, SoC会根据软件配置的那些类型错误
             需要上报的, 进行中断错误类型过滤, 过滤后的错误类型将上报到
             中断上报寄存器中
             在这个函数里, 我们没有从中断上报寄存器中读值, 而是直接从源中断
             寄存器中读值, 这是因为SoC担心软件对中断错误类型进行了过滤

 修改历史      :
  1.日    期   : 2008年3月28日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCIsrRegRead()
{
    ulHDLCIsrRawData    = TTF_READ_32REG(SOC_ARM_HDLC_INTERRUPT_FRAMER_RAW_INT_ADDR(g_ulHDLCIPBaseAddr));
    ulHDLCIsrStatus     = TTF_READ_32REG(SOC_ARM_HDLC_INTERRUPT_INT_STATUS_ADDR(g_ulHDLCIPBaseAddr));
}    /* link_HDLCIsrRegRead */

/*****************************************************************************
 函 数 名  : link_HDLCReset
 功能描述  : 对HDLC进行复位
 输入参数  : ulHDLCModule - HDLC module, Framer: HDLC_FRAMER_MODULE
                                         Deframer: HDLC_DEFRAMER_MODULE
             enResetReason - 复位原因
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCReset(VOS_UINT32 ulHDLCModule,
                        HDLC_RESET_REASON_ENUM_UINT8 enResetReason)
{
#if 0    /* for release */
    VOS_UINT32 ulCfg;
    volatile VOS_UINT32 *pulRttMem;
#endif
    VOS_UINT32   ulTaskID;

    if (HDLC_FRAMER_MODULE == ulHDLCModule)
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "Framer reset reason: %d\r\n", enResetReason);

#if 1    /* for debug */
        /* 获取HDLC寄存器的值 */
        link_HDLCIsrRegRead();

        /* 获取Framer最近配置 */
        link_HDLCFramerRegRead();

        link_ReadFramerLastRecord();
#else    /* for release */
        /* Disable Framer */
        ulCfg = 0UL;      /* frm_en is in bit0 */


        TTF_WRITE_32REG(SOC_ARM_HDLC_FRAMER_CFG_ADDR,ulCfg);

        /* Enable Framer */
        SET_BIT_TO_DWORD(ulCfg, HDLC_FRAMER_CFG_EN_BITPOS);
        TTF_WRITE_32REG(SOC_ARM_HDLC_FRAMER_CFG_ADDR,ulCfg);
#endif
    }

    if (HDLC_DEFRAMER_MODULE == ulHDLCModule)
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,"Deframer reset reason: %d\r\n", enResetReason);

#if 1    /* for debug */
        /* 获取HDLC寄存器的值 */
        link_HDLCIsrRegRead();

        /* 获取Deframer最近配置 */
        link_HDLCDeframerRegRead();

        link_ReadFramerLastRecord();
#else    /* for release */
        /* Disable Deframer */
        ulCfg = 0UL;      /* def_en is in bit0 */
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEFRAMER_CFG_ADDR,ulCfg);

        /* Enable Deframer */
        SET_BIT_TO_DWORD(ulCfg, HDLC_DEFRAMER_CFG_EN_BITPOS);
        TTF_WRITE_32REG(SOC_ARM_HDLC_DEFRAMER_CFG_ADDR,ulCfg);
#endif
    }

    ulTaskID = VOS_GetCurrentTaskID();

    if ( VOS_OK != VOS_SuspendTask( ulTaskID ) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,"# SUSPED PPP task error.\r\n");
    }
}    /* link_HDLCReset */


VOS_VOID link_HDLC_Enable(VOS_VOID)
{
    #if 0
    VOS_UINT32 ulReg;
    VOS_INT32  intLockLevel;

    intLockLevel = VOS_SplIMP();

    ulReg = TTF_READ_32REG(SOC_SC_PERIPH_CLKEN0_REG);

    ulReg |= 1 << SOC_SC_PERIPH_CLK_HDLC_BIT;

    TTF_WRITE_32REG(SOC_SC_PERIPH_CLKEN0_REG, ulReg);

    VOS_Splx(intLockLevel);
    #endif

    return;
}

VOS_VOID link_HDLC_Disable(VOS_VOID)
{
    #if 0
    VOS_UINT32 ulReg;
    VOS_INT32  intLockLevel;

    intLockLevel = VOS_SplIMP();

    ulReg = TTF_READ_32REG(SOC_SC_PERIPH_CLKDIS0_REG);

    ulReg |= 1 << SOC_SC_PERIPH_CLK_HDLC_BIT;

    TTF_WRITE_32REG(SOC_SC_PERIPH_CLKDIS0_REG, ulReg);

    VOS_Splx(intLockLevel);
    #endif

    return;
}

/*****************************************************************************
 函 数 名  : link_HDLCDisable
 功能描述  : 对HDLC进行去使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCDisable(VOS_VOID)
{
    /* 因为At_ModemRelInd需要调用link_HDLCDisable,
       而NAS组又要求不能加上PPP_HDLC_BY_HARDWARE的宏开关到AT代码中
       (包括PPP相关的头文件的包含),
       为了能够编译通过, 只好在函数link_HDLCDisable内部区分PPP_HDLC_BY_HARDWARE
       跟PPP_HDLC_BY_SOFTWARE */
    VOS_UINT32 ulCfg;

    /* Disable Framer */
    ulCfg = 0UL;      /* frm_en is in bit0 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr),ulCfg);

    /* Disable Deframer */
    ulCfg = 0UL;      /* def_en is in bit0 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEFRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr),ulCfg);
}    /* link_HDLCDisable */


/*****************************************************************************
 函 数 名  : link_HDLCFramer
 功能描述  : 封装下行收到的数据
 输入参数  : pstHdlcFramerPara - 封装参数
 输出参数  : pstHdlcFramerPara - 封装参数
 返 回 值  : 封装是否成功, 成功 - PS_TRUE, 否则 - PS_FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月6日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 link_HDLCFramer(HDLC_FRAMER_PPP_LINK_PARA_STRU *pstHdlcFramerPppPara,
                           HDLC_FRAMER_PARA_STRU *pstHdlcFramerPara)
{
    VOS_UINT32  ulDataLen;
    VOS_UINT8   ucFramerBlkCnt;         /* 软件下发数据分组总数 */
    VOS_UINT8   ucPComp;
    VOS_UINT8   ucACComp;
    VOS_UINT8   ucLowByte;
    VOS_UINT16  usLowWord;
    VOS_UINT16  usProtocol;
    VOS_UINT32  ulFramerCfg;            /* 封装配置 */
    VOS_UINT32  ulACCM;
    VOS_UINT32  ulDataLoop;             /* 软件当前下发数据的组号, 该数值从1开始计数 */
    VOS_UINT8  *pucCurrOutData;         /* 封装后的数据写入此地址 */
    VOS_UINT16  usCurrWrited;           /* 此组写入数据的字节数 */
    VOS_UINT32  ulFrameRsltWaitNum;     /* 防止硬件异常的保护变量 */
    volatile VOS_UINT32  ulFramerStatus;         /* 封装状态 */
    VOS_UINT16  usFrameBlkLen;          /* 软件下发一组数据被封装后的长度, 单位: 字节 */
    VOS_UINT16  usFrameByteCnt = 0;         /* 读取的数据总长 */
    PPP_ZC_STRU   **ppstMem;
    const VOS_UINT16 usByteAllowed = HDLC_FRAMER_IN_PER_MAX_CNT;    /* 软件与硬件约定一次下发不得超过256Bytes */

    /* 防止输入数据过长 */
    ulDataLen = pstHdlcFramerPara->stInData.ulDataLen;
    if (HDLC_FRAMER_IN_PER_FRAME_MAX_USED_BYTES < ulDataLen)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "Framer data too long\r\n");

        pstHdlcFramerPara->stOutData.usFrameByteCnt = 0;
        return VOS_ERR;
    }

    /* 计算当前帧会分割多少组进行封装 */
    ucFramerBlkCnt = (VOS_UINT8)(((ulDataLen + usByteAllowed) - 1) / usByteAllowed);

    /* 获取AC域压缩指示, P域压缩指示, 协议值, ACCM */
    ucACComp = (PS_TRUE == pstHdlcFramerPara->enACComp) ? 1 : 0;
    if (HDLC_IP_MODE == pstHdlcFramerPara->ucMode)  /* IP模式: P域不合入 */
    {
        ucPComp = (PS_TRUE == pstHdlcFramerPara->enPComp)
                   ? HDLC_PROTOCOL_ADD_WITH_COMPRESS
                   : HDLC_PROTOCOL_ADD_WITHOUT_COMPRESS;
    }
    else    /* PPP模式: P域合入 */
    {
        ucPComp = HDLC_PROTOCOL_NO_ADD;
    }

    usProtocol = pstHdlcFramerPara->usProtocol;
    ulACCM = pstHdlcFramerPara->ulACCM;

    /* 设置accm */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRAMER_ACCM_ADDR(g_ulHDLCIPBaseAddr),ulACCM);

    /* 将ucPComp设置到一个字节的第2, 3位上 */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, (VOS_UINT8)HDLC_FRAMER_CFG_PFC_MOVEBITS_LEN);

    if (1 == ucACComp)  /* AC域压缩 */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_FRAMER_CFG_ACFC_BITPOS);
    }
    else
    {
        CLEAR_BIT_TO_BYTE(ucLowByte, HDLC_FRAMER_CFG_ACFC_BITPOS);
    }
    SET_BIT_TO_BYTE(ucLowByte, HDLC_FRAMER_CFG_EN_BITPOS);

    usLowWord = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulFramerCfg = HDLC_MAKE_DWORD(usProtocol, usLowWord);

    /* 设置AC域压缩指示, P域压缩指示, 协议值和使能封装标记, accm */
    TTF_WRITE_32REG(SOC_ARM_HDLC_FRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr),ulFramerCfg);

    pucCurrOutData = pstHdlcFramerPara->stOutData.pucData;

    for (ulDataLoop = 1UL; ulDataLoop <= (VOS_UINT32)ucFramerBlkCnt; ulDataLoop ++)    /* 一帧是否仍有数据需要封装 */
    {
        /*
           计算一次下发数据的长度,
           软件下发一组数据的长度范围[1,256]Bytes, 用0x01表示长度为1Bytes, 0x100表示256Bytes
        */
        if (ulDataLoop != (VOS_UINT32)ucFramerBlkCnt)
        {
            /* 非最后一组, 一定是256字节 */
            usCurrWrited = usByteAllowed;
        }
        else
        {
            usCurrWrited = (VOS_UINT16)(ulDataLen - ((ulDataLoop-1) * usByteAllowed));
        }

        /*set new bitpos(bit0), blkCnt(8~10), blkIndex(12~14)*/
        /*
        0:frm_blk_new
        8-10:frm_blk_num
        12-14:frm_blk_index
        16-26:frm_blk_length
        */
        TTF_Write32RegByBit(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 8, 10, ucFramerBlkCnt);
        TTF_Write32RegByBit(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 12, 14, ulDataLoop);
        TTF_Write32RegByBit(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 16, 26, usCurrWrited);
        TTF_Write32RegByBit(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 0, 0, 1);

        /* 采用memcpy函数, 将分割后的帧数据, 向输入Buffer写入,
           为了加快后续处理速度, m_buf和TTF_MEM都采用减切方式将数据取走到HDLC */
        ppstMem = &(pstHdlcFramerPara->stInData.pstMem);

        PPP_MemCutHeadData( ppstMem, (VOS_UINT8 *)(SOC_ARM_HDLC_FRAMER_IN_DATA_ADDR(g_ulHDLCIPBaseAddr)),
                            usCurrWrited);

        /* 将frm_block_new置0, 指示硬件软件已经将当前组下发结束 */
        TTF_Write32RegByBit(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 0, 0, 0);

        #if (VOS_OS_VER == VOS_WIN32)
        VHW_HDLC_Task();
        #endif

        /* 判断当前组是否封装完成 */
        ulFrameRsltWaitNum = 0UL;
        while (ulFrameRsltWaitNum < HDLC_FRAMER_MAX_WAIT_RESULT_NUM)
        {
            ulFramerStatus = TTF_READ_32REG(SOC_ARM_HDLC_FRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr));    /* 获取当前处理一组待封装数据的状态 */

            if (HDLC_FRAMER_BLOCK_DONE
                == (VOS_UINT8)GET_BITS_FROM_DWORD(ulFramerStatus, HDLC_FRAMER_BLOCK_DONE_MASK))   /* a framer block done */
            {
                break ;
            }

            ulFrameRsltWaitNum ++;
        }

        if ( HDLC_FRAMER_MAX_WAIT_RESULT_NUM != ulFrameRsltWaitNum )
        {
            /*blklen: bit 16~27*/
            usFrameBlkLen = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_FRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr), 16, 27);

            if (HDLC_FRAMER_OUT_PER_MAX_CNT < usFrameBlkLen)
            {
                /* 单组数据超长, 视为异常 */
                link_HDLCReset(HDLC_FRAMER_MODULE, HDLC_FRAMER_OUT_PER_LONG_ERR);

                pstHdlcFramerPara->stOutData.usFrameByteCnt = 0;
                return VOS_ERR;
            }

            /* 计算此次拷贝后读取的数据总长, 单位: 字节 */
            usFrameByteCnt
                = (VOS_UINT16)((pucCurrOutData + usFrameBlkLen) - pstHdlcFramerPara->stOutData.pucData);

            if (HDLCSIZE < usFrameByteCnt)     /* to avoid array overflow */
            {
                /* 数据总长超长, 视为异常 */
                link_HDLCReset(HDLC_FRAMER_MODULE, HDLC_FRAMER_OUT_ALL_LONG_ERR);

                pstHdlcFramerPara->stOutData.usFrameByteCnt = 0;
                return VOS_ERR;
            }
            else
            {
                /* 从HDLC Framer读数 */

                pstHdlcFramerPara->stOutData.pucData = (VOS_UINT8 *)(SOC_ARM_HDLC_FRAMER_OUT_DATA_ADDR(g_ulHDLCIPBaseAddr));
                pucCurrOutData += usFrameBlkLen;
            }
        }
        else    /* HDLC Framer 工作时间过长, 视为异常 */
        {
            link_HDLCReset(HDLC_FRAMER_MODULE, HDLC_FRAMER_WORK_TIME_LONG_ERR);

            pstHdlcFramerPara->stOutData.usFrameByteCnt = 0;
            return VOS_ERR;
        }
    }   /* end of while loop */

    pstHdlcFramerPara->stOutData.usFrameByteCnt = usFrameByteCnt;

    return VOS_OK;
}   /* link_HDLCFramer */


/*****************************************************************************
 函 数 名  : link_HDLCPushPacket
 功能描述  : 在IP模式下封装下行收到的IP报文
 输入参数  : l         - PPP链接
             pstInData  - 待封装的数据
             pri        - PPP链接的优先级(保留与软件实现代码的形式, 不使用)
             usProtocol - 待封装的协议值
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月5日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCPushPacket
(
    struct link    *l,
    PPP_ZC_STRU    *pstInData,
    VOS_INT32       pri,
    VOS_UINT16      usProtocol
)
{
    VOS_UINT32              ulFramerCfg;
    VOS_UINT32              ulFramerCtrl;
    VOS_UINT8              *pucTmpSrcData;
    VOS_UINT32              ulFrameRsltWaitNum; /* 防止硬件异常的保护变量 */
    volatile VOS_UINT32     ulFramerStatus;     /* 封装状态 */
    VOS_UINT16              usFrameBlkLen;
    VOS_UINT16              usDataLen;


    /* 数据内容长度 */
    usDataLen = PPP_ZC_GET_DATA_LEN(pstInData);

    if ((0 == usDataLen) || (HDLC_FRAMER_IN_PER_FRAME_MAX_USED_BYTES < usDataLen))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "IP mode framer data len wrong %d\r\n", usDataLen);
        PPP_MemFree(pstInData);

        return ;
    }

    /*=========================================================================
      ulFramerCfg:  bit[0]          使能位       1为使能,0为不使能
                    bit[1]          AC域压缩位   1为AC域压缩,0为AC域不压缩
                    bit[2]-bit[3]   P域压缩指示  00,硬件模块添加P域,P域不压缩
                                                  01,硬件模块添加P域,P域压缩
                                                  11,硬件模块不添加P域
                    bit[16]-bit[31] 封装协议值

      IP模式一定添加P域,PPP模式一定不添加P域
      LCP帧: AC域不压缩，P域不压缩
    ==========================================================================*/
    ulFramerCfg = 0x01;
    if (PROTO_LCP != usProtocol)
    {
        if (1 == l->lcp.his_acfcomp)
        {
            ulFramerCfg |= (1 << 1);
        }

        if (1 == l->lcp.his_protocomp)
        {
            ulFramerCfg |= (1 << 2);
        }
    }

    ulFramerCfg |= ( ((VOS_UINT32)usProtocol) << 16 );

    *(VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr)) = ulFramerCfg;

    /* 设置accm */
    if (PROTO_LCP != usProtocol)
    {
        *(VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_ACCM_ADDR(g_ulHDLCIPBaseAddr)) = l->lcp.his_accmap;
    }
    else
    {
        *(VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_ACCM_ADDR(g_ulHDLCIPBaseAddr)) = 0xFFFFFFFF;
    }

    /*=========================================================================
    ulFramerCtrl:
        bit[0]          0 完成待封装数据输入,1 开始待封装数据输入
        bit[8]-bit[10]  待封装数据分组总数,HDLC增强后为1
        bit[12]-bit[14] 当前下发数据的组号,HDLC增强后为1
        bit[16]-bit[26] 下发一组待封装数据的长度
    ==========================================================================*/
    ulFramerCtrl = (0x1101 | (((VOS_UINT32)usDataLen) << 16));

    *(VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr)) = ulFramerCtrl;

    pucTmpSrcData   = (VOS_UINT8 *)(SOC_ARM_HDLC_FRAMER_IN_DATA_ADDR(g_ulHDLCIPBaseAddr));

    /* 零拷贝目前暂不支持链式结构 */
    /*
    while (VOS_NULL_PTR != pstMem)
    {
    */
        /*截取整个零拷贝结构的数据*/
        DRV_RT_MEMCPY(pucTmpSrcData, PPP_ZC_GET_DATA_PTR(pstInData), usDataLen);

        /*pucTmpSrcData += usDataLen;*/

        PPP_MemFree(pstInData);
    /*
    }
    */

    /* 将SOC_ARM_HDLC_FRAMER_CTRL_ADDR的bit 0置0, 指示硬件软件已经将当前组下发结束 */
    ulFramerCtrl &= 0xFFFFFFFE;
    *(VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr)) = ulFramerCtrl;

    #if (VOS_OS_VER == VOS_WIN32)
    VHW_HDLC_Task();
    #endif

    /* 判断当前组是否封装完成 */
    ulFrameRsltWaitNum = 0UL;
    while (ulFrameRsltWaitNum < HDLC_FRAMER_MAX_WAIT_RESULT_NUM)
    {
        /* Linux编译器优化，只做一次循环，加volatile防止编译器优化 */
        ulFramerStatus = *(volatile VOS_UINT32 *)(SOC_ARM_HDLC_FRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr));

        if (HDLC_FRAMER_BLOCK_DONE == (ulFramerStatus & 0x01))
        {
            break ;
        }
        ulFrameRsltWaitNum ++;
    }

    if ( HDLC_FRAMER_MAX_WAIT_RESULT_NUM != ulFrameRsltWaitNum )
    {
        /*blklen: bit 16~27*/
        usFrameBlkLen = (VOS_UINT16)((ulFramerStatus & 0x0fff0000) >> 16);

        if (HDLC_FRAMER_OUT_PER_MAX_CNT < usFrameBlkLen)
        {
            /* 单组数据超长, 视为异常 */
            link_HDLCReset(HDLC_FRAMER_MODULE, HDLC_FRAMER_OUT_PER_LONG_ERR);

            return;
        }
    }
    else    /* HDLC Framer 工作时间过长, 视为异常 */
    {
        link_HDLCReset(HDLC_FRAMER_MODULE, HDLC_FRAMER_WORK_TIME_LONG_ERR);

        return;
    }

    /* TODO:待替换为调用PPP_HDLC_ProcDlData */
    PPP_SendPushedData((VOS_UINT16)PPP_LINK_TO_ID(l),
                          (VOS_UINT8 *)(SOC_ARM_HDLC_FRAMER_OUT_DATA_ADDR(g_ulHDLCIPBaseAddr)),
                          usFrameBlkLen);

    return;
}    /* link_HDLCPushPacket */

/*****************************************************************************
 函 数 名  : link_HDLCSetDeframerCfg
 功能描述  : 设置解封装控制寄存器hdlc_def_ctrl
 输入参数  : pstHdlcDeframerPara - 解封装参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月10日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCSetDeframerCfg(HDLC_DEFRAMER_PARA_STRU *pstHdlcDeframerPara)
{
    VOS_UINT32       ulDeframerCfg;
    VOS_UINT8        ucACComp;
    VOS_UINT8        ucPComp;
    VOS_UINT8        ucLowByte;
    VOS_UINT16       usLowWord;

    /* 获取AC域压缩指示, P域压缩指示 */
    ucACComp = (PS_TRUE == pstHdlcDeframerPara->enACComp) ? 1 : 0;
    if (HDLC_IP_MODE == pstHdlcDeframerPara->ucMode)  /* IP模式: P域不合入 */
    {
        ucPComp = (PS_TRUE == pstHdlcDeframerPara->enPComp)
                   ? HDLC_PROTOCOL_REMOVE_WITH_COMPRESS
                   : HDLC_PROTOCOL_REMOVE_WITHOUT_COMPRESS;
    }
    else    /* PPP模式: P域合入 */
    {
        ucPComp = HDLC_PROTOCOL_NO_REMOVE;
    }

    /* 将ucPComp设置到一个字节的第2, 3位上 */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, HDLC_DEFRAMER_CFG_PFC_MOVEBITS_NUM);

    if (1 == ucACComp)  /* AC域压缩 */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_DEFRAMER_CFG_ACFC_BITPOS);
    }
    else
    {
        CLEAR_BIT_TO_BYTE(ucLowByte, HDLC_DEFRAMER_CFG_ACFC_BITPOS);
    }
    SET_BIT_TO_BYTE(ucLowByte, HDLC_DEFRAMER_CFG_EN_BITPOS);

    usLowWord = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulDeframerCfg = HDLC_MAKE_DWORD(0x00, usLowWord);

    /* 设置AC域压缩指示, P域压缩指示, 使能解封装标记 */
    TTF_WRITE_32REG(SOC_ARM_HDLC_DEFRAMER_CFG_ADDR(g_ulHDLCIPBaseAddr),ulDeframerCfg);
}    /* link_HDLCSetDeframerCfg */


/*****************************************************************************
 函 数 名  : link_HDLCDefDealErr
 功能描述  : 统计解封装出的错误帧
 输入参数  : l - PPP链接
             ucErrType - 寄存器上指示哪些类发生哪写错误帧
 输出参数  : l - PPP链接
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月8日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCDefDealErr(struct link *l, VOS_UINT8 ucErrType)
{
    VOS_UINT8   ucMask;
    VOS_UINT32  ulErrTypeLoop;
    VOS_UINT8   ucResult;
    HDLC_DEFRAMER_ERR_FRAMES_CNT_STRU   stErrCnt;

    /*lint -e734*/
    /* get fcs error count */
    stErrCnt.usFCSErrCnt        = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_0_ADDR(g_ulHDLCIPBaseAddr), 0, 15);

    /* get frame too long error count */
    stErrCnt.usLenLongCnt       = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_0_ADDR(g_ulHDLCIPBaseAddr), 16, 31);

    /* get frame too short error count */
    stErrCnt.usLenShortCnt      = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_1_ADDR(g_ulHDLCIPBaseAddr), 0, 15);

    /* get error protocol count */
    stErrCnt.usErrProtocolCnt   = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_1_ADDR(g_ulHDLCIPBaseAddr), 16, 31);

    /* get error control count */
    stErrCnt.usErrCtrlCnt       = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_2_ADDR(g_ulHDLCIPBaseAddr), 0, 15);

    /* get error address count */
    stErrCnt.usErrAddrCnt       = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_2_ADDR(g_ulHDLCIPBaseAddr), 16, 31);

    /* get error flag position count */
    stErrCnt.usFlagPosErrCnt    = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_ERROR_INFO_3_ADDR(g_ulHDLCIPBaseAddr), 0, 15);
    /*lint +e734*/
    for (ulErrTypeLoop = 0UL; ulErrTypeLoop < HDLC_DEFRAMER_MAX_TYPE_CNT; ulErrTypeLoop ++)
    {
        ucMask = SET_BITS_VALUE_TO_BYTE(0x01, ulErrTypeLoop);    /* 构造掩码 */
        ucResult = (VOS_UINT8)GET_BITS_FROM_BYTE(ucErrType, ucMask);

        if (0 != ucResult)      /* 存在此类错误 */
        {
            if (0UL == ulErrTypeLoop)   /* 错误类型0: CRC校验错误 */
            {
                l->hdlc.stats.badfcs       += stErrCnt.usFCSErrCnt;
                l->hdlc.lqm.SaveInErrors   += stErrCnt.usFCSErrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc fcs\r\n");
            }
            else if (1UL == ulErrTypeLoop)    /* 错误类型1: 解封装后帧字节数大于1502bytes */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usLenLongCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame length too long\r\n");
            }
            else if (2UL == ulErrTypeLoop)    /* 错误类型2: 解封装后帧字节数小于4bytes */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usLenShortCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame length too short\r\n");
            }
            else if (3UL == ulErrTypeLoop)    /* 错误类型3: 当P域需剥离时, 收到非法的Protocol域值(非*******0 *******1形式) */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usErrProtocolCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc frame protocol\r\n");
            }
            else if (4UL == ulErrTypeLoop)    /* 错误类型4: 当AC域无压缩时, Control域值非0x03 */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usErrCtrlCnt;
                l->hdlc.stats.badcommand += stErrCnt.usErrCtrlCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame control\r\n");
            }
            else if (5UL == ulErrTypeLoop)    /* 错误类型5: 当AC域无压缩时, Address域值非0xFF */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usErrAddrCnt;
                l->hdlc.stats.badaddr += stErrCnt.usErrAddrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame address\r\n");
            }
            else if (6UL == ulErrTypeLoop)    /* 错误类型6: 转义字符0x7D后紧接一个Flag域 */
            {
                l->hdlc.lqm.SaveInErrors += stErrCnt.usFlagPosErrCnt;
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "bad hdlc frame flag position\r\n");
            }
            else
            {
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "link_HDLCDefDealErr err\r\n");
            }
        }
    }
}    /* link_HDLCDefDealErr */

/*****************************************************************************
 函 数 名  : link_HDLCDefDealData
 功能描述  : 解封装上行收到的数据
 输入参数  : pstHdlcDeframerPppPara - 解封装时用到的PPP链接参数
             enDealWithErrFrame - 是否去读取错误帧信息, 是 - PS_TRUE, 否 - PS_FALSE
             ulDeframerStatusReg - 当前解出帧的状况
             pstHdlcDeframerPara - 解封装参数
 输出参数  : pstHdlcDeframerPppPara - 解封装时用到的PPP链接参数
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月7日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCDefDealData
(
    HDLC_DEFRAMER_PPP_LINK_PARA_STRU *pstHdlcDeframerPppPara,
    PS_BOOL_ENUM_UINT8                enDealWithErrFrame,
    VOS_UINT32                        ulDeframerStatusReg,
    HDLC_DEFRAMER_PARA_STRU          *pstHdlcDeframerPara
)
{
    PS_BOOL_ENUM_UINT8                enWithLCPFrame;    /* 此组数据中是否存在LCP帧, 是 - PS_TRUE, 否 - PS_FALSE */
    VOS_UINT32                        ulDeframerPerInfo;
    struct link                      *l;
    VOS_UINT32                        ulValidCnt;        /* 解出的合法帧的帧数 */
    VOS_UINT32                        ulValidLoop;
    VOS_UINT16                        usDataLen;         /* 帧的长度, 单位: 字节 */
    VOS_UINT8                        *pucDataReadAddr;   /* 解封装出的一帧的数据存放地址, 此地址由硬件保证地址一定是4字节的倍数 */
    VOS_UINT16                        usProtocol;
    VOS_UINT8                         ucErrType;
    VOS_UINT32                        ulDataAddrOffset;  /* 解封装出的一帧相对解封装数据输出地址偏移多少字节 */
    volatile VOS_UINT32              *pulPerInfoAddr;
    volatile VOS_UINT32              *pulDataRegAddr;    /* 此寄存器中所装内容指示解封装出的一帧相对解封装数据输出地址偏移多少字节 */
    PPP_ZC_STRU                      *pstMem;            /* 从硬件上取数到目的零拷贝 */


    ulValidCnt = TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr), 8, 17);

    if (HDLC_DEFRAMER_OUT_LIST_MAX_CNT < ulValidCnt)
    {
        /* 异常时复位Deframer */
        link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_OUT_LIST_LONG_ERR);

        return ;
    }

    enWithLCPFrame = PS_FALSE;  /* initial */
    l = pstHdlcDeframerPppPara->l;

    /* 对合法帧的处理 */
    for (ulValidLoop = 1UL; ulValidLoop <= ulValidCnt; ulValidLoop ++)
    {
        /* 上报列表中记录了不超过256字节数据所解出所有帧的相关信息,
           这里计算一帧的信息在解封装上报列表中的地址 */
        pulPerInfoAddr = (((volatile VOS_UINT32 *)(SOC_ARM_HDLC_DEFRAMER_OUT_LIST_ADDR(g_ulHDLCIPBaseAddr)))
                       + ((ulValidLoop-1) * HDLC_DEFRAMER_PER_FRAME_INFO_DWORD_USE));
        ulDeframerPerInfo = TTF_READ_32REG((VOS_UINT32)pulPerInfoAddr);  /* 从解封装上报列表中读出一帧的信息 */
        usDataLen = GET_LOW_WORD_FROM_DWORD(ulDeframerPerInfo);     /* 获取一帧的长度 */

        if (0 != usDataLen)     /* 如果一帧存在数据 */
        {
            /* 单帧长度超长, 视为硬件异常 */
            if (HDLC_DEFRAMER_OUT_PER_FRAME_MAX_USED_BYTES < usDataLen)
            {
                /* 异常时复位Deframer */
                link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_OUT_PER_LONG_ERR);

                return ;
            }

            pulDataRegAddr = (volatile VOS_UINT32 *)(pulPerInfoAddr + HDLC_DEFRAMER_PER_FRAME_ADDR_DWORD_OFFSET);
            ulDataAddrOffset = TTF_READ_32REG((VOS_UINT32)pulDataRegAddr);   /* 获得此帧相对解封装数据输出地址偏移多少字节 */

            /* 防止地址偏移超出解封装输出数据范围 */
            if ((HDLC_DEFRAMER_OUT_ALL_FRAMES_MAX_USED_BYTES) <= ulDataAddrOffset)
            {
                /* 异常时复位Deframer */
                link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_OUT_OFFSET_LONG_ERR);

                return ;
            }

            pucDataReadAddr = (VOS_UINT8 *)((SOC_ARM_HDLC_DEFRAMER_OUT_DATA_BASE_ADDR(g_ulHDLCIPBaseAddr)) + ulDataAddrOffset);

            /* 数据读取地址软硬件约定从4字节边界读起 */
            if (0 != (((VOS_UINT32)(pucDataReadAddr)) % sizeof(VOS_UINT32)))
            {
                /* 异常时复位Deframer */
                link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_OUT_ADDRESS_ERR);

                return ;
            }

            /* 分配一块长度为usDataLen的零拷贝内存块 */
            pstMem = PPP_MemAlloc(usDataLen, PPP_ZC_UL_RESERVE_LEN);

            if (VOS_NULL_PTR == pstMem)
            {
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                             "Error: PPP, link_HDLCDefDealData, WARING, Alloc Mem for ip type Fail\r\n");
                return;
            }

            /*COPY至零拷贝内存数据部分*/
            PPP_MemWriteData(pstMem, pucDataReadAddr, usDataLen);

            /*
            TTF_TraceData(PS_PID_APP_PPP, PS_PID_APP_PPP, 0x13, pstMem->pData, usDataLen,
                PPP_LINK_TO_ID(l) ,0, 0, 0);
            */

            if (HDLC_IP_MODE == pstHdlcDeframerPara->ucMode)    /* IP模式 */
            {
                usProtocol = GET_HIGH_WORD_FROM_DWORD(ulDeframerPerInfo);
                if (PROTO_LCP == usProtocol)
                {
                    enWithLCPFrame  = PS_TRUE;
                }

                PPP_HDLC_ProcIpModeUlData(l, pstMem, usProtocol);
            }
            else    /* PPP模式 */
            {
                PPP_HDLC_ProcPppModeUlData((VOS_UINT16)PPP_LINK_TO_ID(l), pstMem);
            }
        }   /* end of if */
    }   /* end of for */

    if (PS_TRUE == enDealWithErrFrame)    /* when HDLC pause, need not to record error frames counts */
    {
        ucErrType = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr), 24, 30);
        if (0 != ucErrType)     /* 有错误帧统计信息 */
        {
            link_HDLCDefDealErr(l, ucErrType);
        }
    }


    /* met LCP frames, need to update deframer parameters */
    if ( PS_TRUE == enWithLCPFrame )
    {
        pstHdlcDeframerPara->enACComp = (1 == l->lcp.want_acfcomp) ? PS_TRUE : PS_FALSE;
        pstHdlcDeframerPara->enPComp = (1 == l->lcp.want_protocomp) ? PS_TRUE : PS_FALSE;

        /* if met LCP frames, update deframer cfg para */
        link_HDLCSetDeframerCfg(pstHdlcDeframerPara);
    }
}    /* link_HDLCDefDealData */

/*****************************************************************************
 函 数 名  : link_HDLCDeframer
 功能描述  : 解封装上行收到的数据
 输入参数  : pstHdlcDeframerPppPara - 解封装时用到的PPP链接参数
             pstHdlcDeframerPara - 解封装参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月7日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCDeframer(HDLC_DEFRAMER_PPP_LINK_PARA_STRU *pstHdlcDeframerPppPara,
                           HDLC_DEFRAMER_PARA_STRU *pstHdlcDeframerPara)
{
    const VOS_UINT16                    usByteAllowed = HDLC_DEFRAMER_IN_PER_MAX_CNT;    /* 软件与硬件约定一次下发不得超过256Bytes */
    VOS_UINT8                           *pucInDataStart;    /* 指向当前要写入的待解封装数据的起始地址 */
    VOS_UINT32                          ulDataLen;     /* 待解封装的数据长度, 单位: 字节 */
    VOS_UINT8                           ucDeframerBlkCnt;  /* 待解封装的数据分几次下发 */
    VOS_UINT32                          ulDataLoop;    /* 指示第几组数据, 从1开始计数 */
    VOS_UINT16                          usCurrWrited;      /* 此组下发的数据长度, 单位: 字节 */
    VOS_UINT32                          ulDeframeRsltWaitNum;  /* 防止硬件工作异常的保护变量 */
    volatile VOS_UINT32                 ulDeframerStatus;
    volatile VOS_UINT8                  ucBlockStatus;
    PS_BOOL_ENUM_UINT8                  enMeetLcpPause;    /* 记录一组数据处理中是否遇到LCP帧暂停的情况, 是 - PS_TRUE, 否 - PS_FALSE */
    volatile VOS_UINT32                 *pulRttMem;

    /* 防止数据过长 */
    ulDataLen = pstHdlcDeframerPara->stDeframerInData.ulLen;
    if (HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES < ulDataLen)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "Deframer data too long\r\n");
        return ;
    }

    link_HDLCSetDeframerCfg(pstHdlcDeframerPara);

    /* initial */
    pucInDataStart = pstHdlcDeframerPara->stDeframerInData.pucData;
    ulDataLoop = 1UL;
    enMeetLcpPause = PS_FALSE;
    usCurrWrited = 0;

    /* 计算当前数据会分割多少组进行封装 */
    ucDeframerBlkCnt = (VOS_UINT8)(((ulDataLen + usByteAllowed) - 1) / usByteAllowed);

    while (ulDataLoop <= (VOS_UINT32)ucDeframerBlkCnt)    /* 一批数据是否仍有数据需要解封装 */
    {
        /* 如果没有发生LCP帧暂停, 需要写入新的一组数据, 否则不需要 */
        if (PS_FALSE == enMeetLcpPause)
        {
            /* 计算一次下发数据的长度 */
            if (ulDataLoop != (VOS_UINT32)ucDeframerBlkCnt)
            {
                /* 非最后一组, 一定是256字节 */
                usCurrWrited = usByteAllowed;
            }
            else
            {
                usCurrWrited = (VOS_UINT16)(ulDataLen - ((ulDataLoop-1)*usByteAllowed));
            }

            /*blklen: 16~27*/
            TTF_Write32RegByBit(SOC_ARM_HDLC_DEFRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 16, 27, usCurrWrited);
            TTF_Write32RegByBit(SOC_ARM_HDLC_DEFRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 0, 0, 1);

            /* 采用memcpy函数, 将分割后的帧数据, 向输入Buffer写入 */
            PPP_MemSingleCopy((VOS_UINT8 *)(SOC_ARM_HDLC_DEFRAMER_IN_DATA_ADDR(g_ulHDLCIPBaseAddr)), pucInDataStart, usCurrWrited);

            pucInDataStart += usCurrWrited;

            /* 将def_block_new置0, 指示硬件软件已经将当前组下发结束 */
            TTF_Write32RegByBit(SOC_ARM_HDLC_DEFRAMER_CTRL_ADDR(g_ulHDLCIPBaseAddr), 0, 0, 0);
        }   /* end of if */

        #if (VOS_OS_VER == VOS_WIN32)
        VHW_HDLC_Task();
        #endif

        /* 判断当前组是否解封装完成 */
        ulDeframeRsltWaitNum = 0UL;
        while (ulDeframeRsltWaitNum < HDLC_DEFRAMER_MAX_WAIT_RESULT_NUM)
        {
            ucBlockStatus = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_DEFRAMER_STATUS_ADDR(g_ulHDLCIPBaseAddr), 0, 1);

            if (HDLC_DEFRAMER_BLOCK_STATUS_DOING != ucBlockStatus)   /* a deframer block done */
            {
                break ;
            }

            ulDeframeRsltWaitNum ++;
        }

        ulDeframerStatus = 0;
        if ( HDLC_DEFRAMER_MAX_WAIT_RESULT_NUM != ulDeframeRsltWaitNum )
        {
            switch(ucBlockStatus)
            {
                case HDLC_DEFRAMER_BLOCK_STATUS_DONE_WITH_FRAMES:
                {
                    link_HDLCDefDealData(pstHdlcDeframerPppPara, PS_TRUE, ulDeframerStatus,
                        pstHdlcDeframerPara);
                    ulDataLoop ++;  /* 一组数据处理完毕, 循环变量增加1 */
                    enMeetLcpPause = PS_FALSE;
                    break ;
                }


                case HDLC_DEFRAMER_BLOCK_STATUS_PAUSE:
                {
                    link_HDLCDefDealData(pstHdlcDeframerPppPara, PS_FALSE, ulDeframerStatus,
                        pstHdlcDeframerPara);

                    /* 标记设置go on */
                    pulRttMem = (volatile VOS_UINT32 *)(SOC_ARM_HDLC_DEFRAMER_GO_ON_ADDR(g_ulHDLCIPBaseAddr));
                    SET_BIT_TO_DWORD(*pulRttMem, HDLC_DEFRAMER_GO_ON_BITPOS);

                    enMeetLcpPause = PS_TRUE;
                    /* ulDataLoop不需要增加1, 因为此组数据没有处理完毕 */
                    break ;
                }

                case HDLC_DEFRAMER_BLOCK_STATUS_DONE_WITHOUT_FRAMES:
                {
                    ulDataLoop ++;  /* 一组数据处理完毕, 循环变量增加1 */
                    enMeetLcpPause = PS_FALSE;
                    break ;
                }

                case HDLC_DEFRAMER_BLOCK_STATUS_DOING:
                default:
                {
                    /* 异常时复位Deframer */
                    link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_WORK_TIME_LONG_ERR);
                    return ;
                }
            }   /* end of switch */
        }
        else
        {
            /* 异常时复位Deframer */
            link_HDLCReset(HDLC_DEFRAMER_MODULE, HDLC_DEFRAMER_WORK_TIME_LONG_ERR);
            return ;
        }
    }   /* end of while */

}    /* link_HDLCDeframer */


/*****************************************************************************
 函 数 名  : link_HDLCPullData
 功能描述  : 在IP模式下解封装上行收到的数据
 输入参数  : l - PPP链接
             pucData - 待解封装的数据(不需要释放)
             ullen - 待解封装的数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月5日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCPullData(struct link *l, VOS_UINT8 *pucData, VOS_UINT32 ullen)
{
    HDLC_DEFRAMER_PARA_STRU             stHdlcDeframerPara;
    HDLC_DEFRAMER_PPP_LINK_PARA_STRU    stHdlcDeframerPppPara;

    if (!l)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "link_HDLCPullData:Can't Pull an async packet from a logical link\r\n");
        return;
    }

    if (0 == ullen)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "IP mode deframer data len is zero\r\n");
        return ;
    }

    stHdlcDeframerPara.ucMode = HDLC_IP_MODE;
    stHdlcDeframerPara.enPComp = (1 == l->lcp.want_protocomp) ? PS_TRUE : PS_FALSE;
    stHdlcDeframerPara.enACComp = (1 == l->lcp.want_acfcomp) ? PS_TRUE : PS_FALSE;

    stHdlcDeframerPara.stDeframerInData.pucData = (VOS_UINT8 *)pucData;
    stHdlcDeframerPara.stDeframerInData.ulLen = ullen;

    stHdlcDeframerPppPara.l = l;

    link_HDLCDeframer(&stHdlcDeframerPppPara, &stHdlcDeframerPara);
}    /* link_HDLCPullData */


/*****************************************************************************
 函 数 名  : link_HDLCPullPacket
 功能描述  : 在IP模式下解封装上行收到的数据
 输入参数  : l - PPP链接
             pucData - 待解封装的数据(不需要释放)
             ullen - 待解封装的数据长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月5日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID link_HDLCPullPacket(struct link *l, PPP_ZC_STRU *pstData)
{
    PPP_ZC_STRU            *pstCurrMem;
    VOS_UINT8              *pucCurrDataStart;    /* 当前的m_buf数据起始地址 */
    VOS_UINT32              ulCurrDataLen;    /* 此次pull数据的长度, 不超过4096字节 */
    VOS_UINT32              ulCurrPullCnt;    /* 当前的m_buf共pull的总次数 */
    VOS_UINT32              ulCurrPullLoop;
    VOS_UINT32              ulCurrPulledByteCnt;    /* 此次pull的字节数 */


    if (!l)
    {
        PPP_MemFree(pstData);
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "link_HDLCPullPacket:Can't Pull an async packet from a logical link\r\n");

        return;
    }

    pstCurrMem  = pstData;

    /* 当前零拷贝不支持链式 */
    /*
    while (VOS_NULL_PTR != pstCurrMem)
    {
    */
        pucCurrDataStart = PPP_ZC_GET_DATA_PTR(pstCurrMem);
        /*lint -e613*/
        ulCurrDataLen    = PPP_ZC_GET_DATA_LEN(pstCurrMem);
        /*lint +e613*/

        /* 计算当前m_buf数据会分割多少次4096字节进行解封装,
           HDLC要求一次数据最多4096字节 */
        ulCurrPullCnt
            = (((ulCurrDataLen + HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES) - 1) / HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES);
        for (ulCurrPullLoop = 0; ulCurrPullLoop < ulCurrPullCnt; ulCurrPullLoop ++)
        {
            if (ulCurrPullLoop != (ulCurrPullCnt-1))
            {
                /* 非最后一次pull, 一定是4096字节 */
                ulCurrPulledByteCnt = HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES;
            }
            else
            {
                ulCurrPulledByteCnt = (ulCurrDataLen - (ulCurrPullLoop*HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES));
            }
            link_HDLCPullData(l,
                                (pucCurrDataStart + (ulCurrPullLoop * HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES)),
                                ulCurrPulledByteCnt);
        }
    /*
        pstCurrMem  = pstCurrMem->pNext;
    }
    */

    PPP_MemFree(pstData);
}    /* link_HDLCPullPacket */


/*****************************************************************************
 函 数 名  : Ppp_HDLCFrameEncap
 功能描述  : PPP模式下封装下行收到的数据
 输入参数  : PppId - PPP-ID
             ucIndex - gastAtClientTab数组下标
             pucData - 待封装的数据
             usLen - 待封装的数据的长度
 输出参数  : 无
 返 回 值  : 封装结果, 成功 - VOS_OK, 否则 - VOS_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月7日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 Ppp_HDLCFrameEncap(VOS_UINT16 usPppId,
                              PPP_ZC_STRU *pstDataMem)
{
    VOS_UINT16 usProtocol;
    struct link *l;
    HDLC_FRAMER_PARA_STRU stHdlcFramerPara;
    VOS_UINT32 ulFrameResult;
    VOS_UINT16 usLen;
    VOS_UINT8           aucProto[2];
    VOS_UINT32          ulRtn;
    HDLC_FRAMER_PPP_LINK_PARA_STRU stHdlcFramerPppPara;

    l = PPP_LINK(usPppId);

    usLen = PPP_ZC_GET_DATA_LEN(pstDataMem);

    if (0 == usLen)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "PPP mode framer data len is zero\r\n");
        return VOS_ERR;
    }

    ulRtn = PPP_MemGet(pstDataMem, 0, aucProto, 2);
    if (PS_FAIL == ulRtn)
    {
        PPP_MemFree(pstDataMem);

        return VOS_ERR;
    }

    if (0x01 == GET_BITS_FROM_BYTE(aucProto[0], 0x01))
    {
        usProtocol = HDLC_MAKE_WORD(0x00, aucProto[0]);
    }
    else
    {
        if (0x01 == GET_BITS_FROM_BYTE(aucProto[1], 0x01))
        {
            usProtocol = HDLC_MAKE_WORD(aucProto[0], aucProto[1]);
        }
        else
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "PPP mode framer data protocol error\r\n");
            PPP_MemFree(pstDataMem);

            return VOS_ERR;
        }
    }

    stHdlcFramerPara.usProtocol = usProtocol;
    if (PROTO_LCP == usProtocol)
    {
        /* LCP帧P不压缩, AC不压缩, ACCM为全转义 */
        stHdlcFramerPara.enPComp = PS_FALSE;
        stHdlcFramerPara.enACComp = PS_FALSE;
        stHdlcFramerPara.ulACCM = 0xFFFFFFFF;
    }
    else
    {
        /* 使用默认值 */
        stHdlcFramerPara.enPComp = (1 == l->lcp.his_protocomp) ? PS_TRUE : PS_FALSE;
        stHdlcFramerPara.enACComp = (1 == l->lcp.his_acfcomp) ? PS_TRUE : PS_FALSE;
        /* stHdlcFramerPara.ulACCM = l->lcp.his_accmap; */
        /* 由于PPP拨号不知道ACCM, 故设置为协议默认值 */
        stHdlcFramerPara.ulACCM = 0xFFFFFFFF;
    }
    stHdlcFramerPara.ucMode = HDLC_PPP_MODE;

    stHdlcFramerPara.stInData.pstMem = pstDataMem;
    stHdlcFramerPara.stInData.ulDataLen = (VOS_UINT32)usLen;
    stHdlcFramerPara.stOutData.pucData  = (VOS_UINT8 *)(l->async.xbuff);
    stHdlcFramerPppPara.l = l;

    ulFrameResult = link_HDLCFramer(&stHdlcFramerPppPara, &stHdlcFramerPara);

    if (VOS_OK == ulFrameResult)
    {
        /* TODO:待替换为调用PPP_HDLC_ProcDlData */
        PPP_SendPushedData(usPppId,
                           (stHdlcFramerPara.stOutData.pucData),
                           (stHdlcFramerPara.stOutData.usFrameByteCnt));
    }
    else
    {
        if (VOS_NULL_PTR != pstDataMem)
        {
            PPP_MemFree(pstDataMem);
        }
    }

    /* 在link_HDLCFramer内部每组写入数据时, 同时释放内存, 以确保零拷贝内存已经完全释放 */
    return ulFrameResult;
}    /* Ppp_HDLCFrameEncap */

/*****************************************************************************
 函 数 名  : Ppp_HDLCDataDecap
 功能描述  : PPP模式下解封装上行收到的数据
 输入参数  : PppId - PPP-ID
             ucIndex - gastAtClientTab数组下标
             pucData - 待封装的数据(不需要释放)
             usLen - 待封装的数据的长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月8日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID Ppp_HDLCDataDecap(VOS_UINT16 PppId,
                            VOS_UINT8 *pucData,
                            VOS_UINT32 ulLen)
{
    HDLC_DEFRAMER_PARA_STRU stHdlcDeframerPara;
    HDLC_DEFRAMER_PPP_LINK_PARA_STRU stHdlcDeframerPppPara;
    struct link *l;

    if (0 == ulLen)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "PPP mode deframer data len is zero\r\n");
        return ;
    }

    l = PPP_LINK(PppId);

    stHdlcDeframerPara.ucMode = HDLC_PPP_MODE;
    stHdlcDeframerPara.enPComp = (1 == l->lcp.want_protocomp) ? PS_TRUE : PS_FALSE;
    stHdlcDeframerPara.enACComp = (1 == l->lcp.want_acfcomp) ? PS_TRUE : PS_FALSE;

    stHdlcDeframerPara.stDeframerInData.pucData = pucData;
    stHdlcDeframerPara.stDeframerInData.ulLen = ulLen;

    stHdlcDeframerPppPara.l = l;

    link_HDLCDeframer(&stHdlcDeframerPppPara, &stHdlcDeframerPara);

    return ;
}    /* Ppp_HDLCDataDecap */


/*****************************************************************************
 函 数 名  : Ppp_HDLCFrameDecap
 功能描述  : PPP模式下解封装上行收到的数据
 输入参数  : PppId - PPP-ID
             ucIndex - gastAtClientTab数组下标
             pucData - 待封装的数据(不需要释放)
             usLen - 待封装的数据的长度
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月8日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID Ppp_HDLCFrameDecap(VOS_UINT16 PppId, PPP_ZC_STRU *pstData)
{
    PPP_ZC_STRU            *pstCurrMem;    /* 当前的m_buf */
    VOS_UINT8              *pucCurrDataStart;    /* 当前的m_buf数据起始地址 */
    VOS_UINT32              ulCurrDataLen;    /* 此次pull数据的长度, 不超过4096字节 */
    VOS_UINT32              ulCurrPullCnt;    /* 当前的m_buf共pull的总次数 */
    VOS_UINT32              ulCurrPullLoop;
    VOS_UINT32              ulCurrPulledByteCnt;    /* 此次pull的字节数 */


    pstCurrMem  = pstData;

    /*
    while (VOS_NULL_PTR != pstCurrMem)
    {
    */
        pucCurrDataStart = PPP_ZC_GET_DATA_PTR(pstCurrMem);
        /*lint -e613*/
        ulCurrDataLen    = PPP_ZC_GET_DATA_LEN(pstCurrMem);
        /*lint +e613*/

        /* 计算当前m_buf数据会分割多少次4096字节进行解封装,
           HDLC要求一次数据最多4096字节 */
        ulCurrPullCnt
            = (((ulCurrDataLen + HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES) - 1) / HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES);
        for (ulCurrPullLoop = 0; ulCurrPullLoop < ulCurrPullCnt; ulCurrPullLoop ++)
        {
            if (ulCurrPullLoop != (ulCurrPullCnt-1))
            {
                /* 非最后一次pull, 一定是4096字节 */
                ulCurrPulledByteCnt = HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES;
            }
            else
            {
                ulCurrPulledByteCnt = (ulCurrDataLen - (ulCurrPullLoop*HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES));
            }
            Ppp_HDLCDataDecap(PppId,
                               (VOS_UINT8 *)(pucCurrDataStart + (ulCurrPullLoop * HDLC_DEFRAMER_IN_ALL_FRAMES_MAX_USED_BYTES)),
                               ulCurrPulledByteCnt);
        }
    /*
        pstCurrMem  = pstCurrMem->pNext;
    }
    */

    PPP_MemFree(pstData);
}    /* Ppp_HDLCFrameDecap */

PPP_HDLC_RESULT_TYPE_ENUM_UINT32 PPP_HDLC_HARD_ProcData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU   *pstDataQ
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ulDealCnt = 0;
    PPP_DATA_TYPE_ENUM_UINT8            ucDataType;

    for (;;)
    {
        pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(&g_PppDataQCtrl.stDataQ);

        /* 队列为空的时候返回空指针 */
        if ( VOS_NULL_PTR == pstMem )
        {
            return PPP_HDLC_RESULT_COMM_FINISH;
        }

        /*处理该结点(结点的释放动作已经在各处理函数内部完成，无需再释放结点)*/
        ucDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

        link_HDLC_Enable();

        switch ( ucDataType )
        {
            case PPP_PULL_PACKET_TYPE:
                g_PppHdlcHardStat.ulUplinkIpDataProcCnt++;

                link_HDLCPullPacket(pstLink, pstMem);
                break;
            case PPP_PUSH_PACKET_TYPE:
                g_PppHdlcHardStat.ulDownlinkIpDataProcCnt++;

                link_HDLCPushPacket(pstLink, pstMem, 0, PROTO_IP);
                break;
            case PPP_PULL_RAW_DATA_TYPE:
                g_PppHdlcHardStat.ulUplinkPppDataProcCnt++;

                Ppp_HDLCFrameDecap(usPppId, pstMem);
                break;
            case PPP_PUSH_RAW_DATA_TYPE:
                g_PppHdlcHardStat.ulDownlinkPppDataProcCnt++;

                Ppp_HDLCFrameEncap(usPppId, pstMem);
                break;
            default:
                PPP_MemFree(pstMem);
                PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                              "PPP_ProcDataNotify, WARNING, ucDataType %d is Abnormal!", ucDataType);
                break;
        }

        link_HDLC_Disable();

        /*统计*/
        ulDealCnt++;

        if ( ulDealCnt > g_PppHdlcHardStat.ulMaxCntOnce )
        {
            g_PppHdlcHardStat.ulMaxCntOnce = ulDealCnt;
        }

        /*如果循环处理的结点个数超出了队列一次允许处理最大结点数，
          则退出循环并发送PPP_DATA_PROC_NOTIFY消息*/
        if ( ulDealCnt >= PPP_ONCE_DEAL_MAX_CNT )
        {
            return PPP_HDLC_RESULT_COMM_CONTINUE;
        }
    } /* for (;;) */
}
VOS_VOID PPP_HDLC_HARD_ProcProtocolPacket
(
    struct link     *pstLink,
    struct ppp_mbuf *pstMbuf,
    VOS_INT32       ulPri,
    VOS_UINT16      ulProtocol
)
{
    PPP_ZC_STRU            *pstMem;


    pstMem  = ppp_m_alloc_ttfmem_from_mbuf(pstMbuf);

    ppp_m_freem(pstMbuf);
    pstMbuf  = VOS_NULL_PTR;

    if (VOS_NULL_PTR != pstMem)
    {
        link_HDLCPushPacket(pstLink, pstMem, ulPri, ulProtocol);
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_ProcAsFrmPacket
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    /* HDLC优化后需要，这里解决编译问题 */
}
VOS_UINT32 PPP_PHY_TO_VIRT(unsigned int ulPAddr)
{
    VOS_UINT32  ulUpEdge = (g_ulPppPhyAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN);

#if(FEATURE_OFF == FEATURE_SKB_EXP)
    if ((ulPAddr >= g_ulPppPhyAddr) && (ulPAddr <= (g_ulPppPhyAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN)))
    {
        return TTF_PHY_TO_VIRT(ulPAddr);
    }
    else
    {
        return phys_to_virt(ulPAddr);
    }
#else
    return TTF_PHY_TO_VIRT(ulPAddr);
#endif
}


VOS_UINT32 PPP_VIRT_TO_PHY(unsigned int ulVAddr)
{
    VOS_UINT32  ulUpEdge = (g_ulPppVirtAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN);

#if(FEATURE_OFF == FEATURE_SKB_EXP)
    if ((ulVAddr >= g_ulPppVirtAddr) && (ulVAddr <= (g_ulPppVirtAddr + TTF_HDLC_MASTER_LINK_TOTAL_LEN)))
    {
        return TTF_VIRT_TO_PHY(ulVAddr);
    }
    else
    {
        return virt_to_phys(ulVAddr);
    }
#else
    return TTF_VIRT_TO_PHY(ulVAddr);
#endif
}


#endif
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

