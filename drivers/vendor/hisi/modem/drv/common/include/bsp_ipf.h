
#ifndef    _BSP_IPF_H_
#define    _BSP_IPF_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************
  头文件包含
**************************************************************************/
#include <osl_common.h>
#include <osl_spinlock.h>
#include <hi_base.h>
#include <hi_ipf.h>
#include <soc_interrupts.h>
#include <bsp_sram.h>
#include <bsp_shared_ddr.h>
#include <drv_ipf.h>
#include <bsp_om.h>
#include <bsp_memmap.h>
#ifdef CONFIG_BALONG_MODEM_RESET
#include <bsp_reset.h>
#endif
#ifdef CONFIG_MODULE_VIC
#ifndef __CMSIS_RTOS
#include <bsp_vic.h>
#endif
#endif

/**************************************************************************
  宏定义
**************************************************************************/
/* #define __BSP_IPF_DEBUG__*/

#ifdef __VXWORKS__
#define INT_LVL_IPF INT_LVL_IPF0
#endif

#ifdef __KERNEL__
#define INT_LVL_IPF INT_LVL_IPF1
#endif

/*IPF描述符地址定义*/

#define IPF_DLBD_MEM_ADDR              SHM_MEM_IPF_ADDR
#define IPF_DLBD_MEM_SIZE               (IPF_DLBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_DLRD_MEM_ADDR              (IPF_DLBD_MEM_ADDR + IPF_DLBD_MEM_SIZE)
#define IPF_DLRD_MEM_SIZE               (IPF_DLRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_DLAD0_MEM_ADDR              (IPF_DLRD_MEM_ADDR + IPF_DLRD_MEM_SIZE)
#define IPF_DLAD0_MEM_SIZE               (IPF_DLAD0_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_DLAD1_MEM_ADDR              (IPF_DLAD0_MEM_ADDR + IPF_DLAD0_MEM_SIZE)
#define IPF_DLAD1_MEM_SIZE               (IPF_DLAD1_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_DLCD_MEM_ADDR              (IPF_DLAD1_MEM_ADDR  + IPF_DLAD1_MEM_SIZE)
#define IPF_DLCD_MEM_SIZE               (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

#define IPF_ULBD_MEM_ADDR              (IPF_DLCD_MEM_ADDR  + IPF_DLCD_MEM_SIZE)
#define IPF_ULBD_MEM_SIZE               (IPF_ULBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_ULRD_MEM_ADDR              (IPF_ULBD_MEM_ADDR + IPF_ULBD_MEM_SIZE)
#define IPF_ULRD_MEM_SIZE               (IPF_ULRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_ULAD0_MEM_ADDR              (IPF_ULRD_MEM_ADDR + IPF_ULRD_MEM_SIZE)
#define IPF_ULAD0_MEM_SIZE               (IPF_ULAD0_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_ULAD1_MEM_ADDR              (IPF_ULAD0_MEM_ADDR + IPF_ULAD0_MEM_SIZE)
#define IPF_ULAD1_MEM_SIZE               (IPF_ULAD1_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_PWRCTL_BASIC_FILTER_ADDR		(IPF_ULAD1_MEM_ADDR + IPF_ULAD1_MEM_SIZE)
#define IPF_PWRCTL_BASIC_FILTER_SIZE 		(IPF_BF_NUM * sizeof(IPF_MATCH_INFO_S))

#define IPF_EXT_FILTER_ADDR             (IPF_PWRCTL_BASIC_FILTER_ADDR + IPF_PWRCTL_BASIC_FILTER_SIZE)
#define IPF_EXT_FILTER_SIZE 		     (EXFLITER_NUM * sizeof(IPF_MATCH_INFO_S))

#define IPF_PWRCTL_INFO_ADDR		(IPF_EXT_FILTER_ADDR + IPF_EXT_FILTER_SIZE)
#define IPF_PWRCTL_INFO_SIZE           128

#define IPF_PWRCTL_ADQ_WPTR_INFO_ADDR	(IPF_PWRCTL_INFO_ADDR + IPF_PWRCTL_INFO_SIZE)
#define IPF_PWRCTL_ADQ_WPTR_INFO_SIZE       32

#define IPF_PWRCTL_ADQ_RPTR_CHCTRL_ADDR	(IPF_PWRCTL_ADQ_WPTR_INFO_ADDR + IPF_PWRCTL_ADQ_WPTR_INFO_SIZE)
#define IPF_PWRCTL_ADQ_RPTR_CHCTRL_SIZE       64

#define IPF_INIT_ADDR                        (IPF_PWRCTL_ADQ_RPTR_CHCTRL_ADDR + IPF_PWRCTL_ADQ_RPTR_CHCTRL_SIZE)
#define IPF_INIT_SIZE                          16

#define IPF_ULBD_IDLENUM_ADDR       (IPF_INIT_ADDR + IPF_INIT_SIZE)
#define IPF_ULBD_IDLENUM_SIZE          16

#define IPF_DLCDRPTR_MEM_ADDR                (IPF_ULBD_IDLENUM_ADDR + IPF_ULBD_IDLENUM_SIZE)
#define IPF_DLCDRPTR_MEM_SIZE                sizeof(BSP_U32)

#define IPF_DEBUG_INFO_ADDR           (IPF_DLCDRPTR_MEM_ADDR + IPF_DLCDRPTR_MEM_SIZE)
#define IPF_DEBUG_INFO_SIZE            sizeof(IPF_DEBUG_INFO_S)

#define IPF_DEBUG_DLCD_ADDR           (IPF_DEBUG_INFO_ADDR + IPF_DEBUG_INFO_SIZE)
#define IPF_DEBUG_DLCD_SIZE            (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

#define IPF_DEBUG_INFO_END_ADDR    (IPF_DEBUG_DLCD_ADDR + IPF_DEBUG_DLCD_SIZE)

#define IPF_DLDESC_SIZE   (IPF_DLBD_MEM_SIZE + IPF_DLRD_MEM_SIZE + IPF_DLAD0_MEM_SIZE + IPF_DLAD1_MEM_SIZE + IPF_DLCD_MEM_SIZE)

#define IPF_BF_NUM             		       64  /* 基本过滤器数目 */
#define IPF_TOTAL_FILTER_NUM            256  /* 过滤器总数目先使用一半 */
#define EXFLITER_NUM                         (IPF_TOTAL_FILTER_NUM-IPF_BF_NUM) /* 扩展过滤器数目 */
#define IPF_TAIL_INDEX                       511  /* filter标识最后一个filter */
#define IPF_DISABLE_FILTER                0  /* filter disable*/
#define IPF_FILTERHEAD                      0  /* filter链表头的数据*/
#define IPF_FILTERTYPE_MASK             0xFFFF/*filter类型标示判断位*/
#define IPF_SET_EXT_FILTER             0x10000/*filter类型标示判断位*/
#define IPF_DQ_DEPTH_MASK              0x1FF/*BD、RD队列深度寄存器MASK*/
#define IPF_DQ_PTR_MASK                  0xFF/*BD、RD队列指针MASK*/

#define TIME_OUT_ENABLE                   0x10000 /* 超时计时器使能*/
#define TIME_OUT_CFG                        65           /* 默认超时时间,时钟频率166MHZ下为0.1ms */
#define GATE_CFG                         		0x307           /*设置时钟门控为自动*/
#define IPF_BDWPTR_MUX                    0x100  /*BD写指针互斥量*/
#define IPF_INT_OPEN0                        0x0C03        /*上行通道中断屏蔽*/
#define IPF_INT_OPEN1                        0x0C030000 /*下行通道中断屏蔽*/

#define IPF_CHANNEL_STATE_IDLE       0x14 /*通道状态IDLE*/
#define IPF_ADQ_BUF_EPT_MASK            0x00000004
#define IPF_ADQ_BUF_FULL_MASK           0x00000008
#define IPF_ADQ_BUF_MASK                0xFFFFFFF3
#define IPF_ADQ_THR_MASK                0x0000FFFF
#define IPF_ADQ_EN_MASK                 0xFFFFFFFC
#define IPF_ADQ_DEFAULT_SEETING     (IPF_ULADQ_PLEN_TH<<16)|(IPF_ADSIZE_128<<4)|(IPF_ADSIZE_128<<2)|(IPF_BOTH_ADQ_EN)
#define IPF_ADQ_OFF                          (IPF_ULADQ_PLEN_TH<<16)|(IPF_ADSIZE_128<<4)|(IPF_ADSIZE_128<<2)|(IPF_NO_ADQ)

#define IPF_UL_RPT_INT0                     0x1            /*上行结果上报中断0*/
#define IPF_UL_TIMEOUT_INT0             0x2            /*上行结果上报超时中断0*/
#define IPF_UL_ADQ0_EPTY_INT0         0x400         /*上行ADQ0空中断0*/
#define IPF_UL_ADQ1_EPTY_INT0         0x800         /*上行ADQ1空中断0*/

#define IPF_DL_RPT_INT1                     0x10000       /*下行结果上报中断0*/
#define IPF_DL_TIMEOUT_INT1             0x20000       /*下行结果上报超时中断0*/
#define IPF_DL_ADQ0_EPTY_INT1         0x4000000    /*下行ADQ0空中断0*/
#define IPF_DL_ADQ1_EPTY_INT1         0x8000000    /*下行ADQ1空中断0*/

#define SCH_BD_ONLY    0
#define SCH_ALL_DESC   1

#ifdef __VXWORKS__
#define IPF_PRINT                         printf
#else
#define IPF_PRINT                         printk
#endif

#define IPF_IO_ADDRESS(x)  (((x) - DDR_SHARED_MEM_ADDR) + DDR_SHARED_MEM_VIRT_ADDR)
#define IPF_IO_ADDRESS_PHY(x)  (((x) - DDR_SHARED_MEM_VIRT_ADDR) + DDR_SHARED_MEM_ADDR)


/**************************************************************************
  枚举定义
**************************************************************************/


/**************************************************************************
  STRUCT定义
**************************************************************************/
#if 0
typedef unsigned int TTF_BLK_MEM_STATE_ENUM_UINT32;

typedef struct
{
    TTF_BLK_MEM_STATE_ENUM_UINT32   enMemStateFlag;
    unsigned int                    ulAllocTick;        /* CPU tick when alloc or free */
    unsigned short                  usAllocFileId;      /* File ID when alloc or free */
    unsigned short                  usAllocLineNum;     /* File Line when alloc or free */
    unsigned short                  usTraceFileId;      /* File ID when traced */
    unsigned short                  usTraceLineNum;     /* File Line when traced */
    unsigned int                    ulTraceTick;        /* CPU tick when traced */
} TTF_BLK_MEM_DEBUG_ST;

typedef struct _TTF_NODE_ST
{
    struct _TTF_NODE_ST    *pNext;
    struct _TTF_NODE_ST    *pPrev;
} TTF_NODE_ST;

typedef struct _TTF_MEM_ST
{
    TTF_NODE_ST                     stNode;
    unsigned char                   ucPoolId;       /*本内存属于哪一个内存池 */
    unsigned char                   ucClusterId;    /*本内存是属于哪一个级别*/
    unsigned char                   ucReserve[2];
    struct _TTF_MEM_ST             *pNext;          /* 该数据的下一段 */
    unsigned short                  usType;         /* 内存类型，DYN、BLK、EXT类型 */
    unsigned short                  usLen;          /* 申请数据的总长度，但不一定全部被使用 */
    unsigned short                  usUsed;         /* 已经使用的数据长度   */
    unsigned short                  usApp;          /* 使用者可以使用的区域 */

    TTF_BLK_MEM_DEBUG_ST           *pstDbgInfo;

    unsigned char                  *pOrigData;      /* 记录数据的原始指针 */
    unsigned char                  *pData;          /* 存放数据的指针，物理上指向结构体的连续内存 */
    void                           *pExtBuffAddr;   /* 保存extern类型的内存地址用于释放 */
    unsigned long                   ulForCds[1];    /*预留给CDS使用，GU模不用初始化*/

    #if !defined (VERSION_V3R2) && !defined (VERSION_V3R2_C00)
    unsigned long                   ulForLte[6];    /*预留给LTE使用，GU模不用初始化*/
    #endif
}TTF_MEM_ST;
#endif

typedef struct tagIPF_REG_INFO_S
{
	BSP_U32 u32StartAddr; /* 需要备份的寄存器地址 */
	BSP_U32 u32Length;		/* 需要备份的长度 */
}IPF_REG_INFO_S;

typedef struct tagIPF_UL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_AD_DESC_S* pstIpfADQ0;
    IPF_AD_DESC_S* pstIpfADQ1;
    BSP_U32* pu32IdleBd; /* 记录上一次获取的空闲BD 数 */
#ifdef __VXWORKS__
    BSP_IPF_WakeupUlCb pFnUlIntCb; /* 中断中唤醒的PS任务 */
    BSP_IPF_AdqEmptyUlCb pAdqEmptyUlCb;/*上行AD空回调C核*/
#endif
}IPF_UL_S;

typedef struct tagIPF_DL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_AD_DESC_S* pstIpfADQ0;
    IPF_AD_DESC_S* pstIpfADQ1;

    IPF_CD_DESC_S* pstIpfCDQ;
    IPF_CD_DESC_S* pstIpfDebugCDQ;

#ifdef __KERNEL__
    BSP_IPF_WakeupDlCb pFnDlIntCb; /* 中断中唤醒的PS任务 */
    BSP_IPF_AdqEmptyDlCb pAdqEmptyDlCb;/*下行AD空回调，A核心*/

/*ACore硬件使用的物理地址*/
    IPF_BD_DESC_S* pstIpfPhyBDQ;
    IPF_RD_DESC_S* pstIpfPhyRDQ;
    IPF_AD_DESC_S* pstIpfPhyADQ0;
    IPF_AD_DESC_S* pstIpfPhyADQ1;

#endif
    BSP_U32 *u32IpfCdRptr; /* 当前可以读出的位置 */
    BSP_U32 u32IpfCdWptr;
    BSP_U32 u32IdleBd; /* 记录上一次获取的空闲BD 数 */
}IPF_DL_S;

typedef struct tagIPF_ID_S
{
    BSP_U32 u32PsID;
    BSP_U32 u32FilterID;
    struct tagIPF_ID_S* pstNext;
}IPF_ID_S;

typedef struct tagIPF_FILTER_INFO_S
{
    IPF_ID_S* pstUseList;
    BSP_U32 u32FilterNum;
}IPF_FILTER_INFO_S;

typedef struct tagIPF_DEBUG_INFO_S
{
#ifdef __BSP_IPF_DEBUG__
    BSP_U32 u32IpfDebug;
#endif
    BSP_U32 u32UlBdqOverflow; /* 中断上报上行BD队列溢出次数 */
    BSP_U32 u32DlBdqOverflow; /* 中断上报下行BD队列溢出次数 */

    BSP_U32 ipf_ccore_not_init_count;    /*初始化失败或者ccore单独复位期间试图数传*/
    BSP_U32 ipf_acore_not_init_count;    /*初始化失败或者ccore单独复位期间试图数传*/

    BSP_U32 u32UlBdNotEnough;       /* 上行配置BD,BD不够用次数 */
    BSP_U32 u32UlAd0NotEnough;       /* 上行配置BD,BD不够用次数 */
    BSP_U32 u32UlAd1NotEnough;       /* 上行配置BD,BD不够用次数 */
    BSP_U32 u32UlRdNotEnough;       /* 上行配置BD,BD不够用次数 */
    BSP_U32 u32DlBdNotEnough;       /* 下行配置BD,BD不够用次数 */
    BSP_U32 u32DlAd0NotEnough;       /* 下行配置BD,BD不够用次数 */
    BSP_U32 u32DlAd1NotEnough;       /* 下行配置BD,BD不够用次数 */
    BSP_U32 u32DlRdNotEnough;       /* 下行配置BD,BD不够用次数 */

    BSP_U32 u32UlAdq0Overflow; /* 中断上报下行BD队列溢出次数 */
    BSP_U32 u32UlAdq1Overflow; /* 中断上报下行BD队列溢出次数 */
    BSP_U32 u32DlAdq0Overflow; /* 中断上报下行BD队列溢出次数 */
    BSP_U32 u32DlAdq1Overflow; /* 中断上报下行BD队列溢出次数 */
    BSP_U32 u32DlCdNotEnough;       /* 下行配置CD,CD不够用次数 */
    BSP_U32 u32UlIPFBusyNum;       /* 上行IPF忙次数 */
    BSP_U32 u32DlIPFBusyNum;       /* 下行IPF忙次数 */
    BSP_U32 u32UlIPFOccupyNum;       /* 上行IPF核间互斥次数 */
    BSP_U32 u32DlIPFOccupyNum;       /* 下行IPF核间互斥次数 */

    BSP_U32 ipf_ulbd_len_zero_count;
    BSP_U32 ipf_ulad0_error_count;
    BSP_U32 ipf_ulad1_error_count;
    BSP_U32 ipf_dlbd_len_zero_count;
    BSP_U32 ipf_dlad0_error_count;
    BSP_U32 ipf_dlad1_error_count;

    BSP_U32 ipf_timestamp_ul_en;
    BSP_U32 ipf_cfg_ulbd_count;
    BSP_U32 ipf_ulbd_done_count;
    BSP_U32 ipf_get_ulrd_count;
    BSP_U32 ipf_cfg_ulad0_count;
    BSP_U32 ipf_cfg_ulad1_count;
    BSP_U32 ipf_ccore_suspend_count;
    BSP_U32 ipf_ccore_resume_count;

    BSP_U32 ipf_timestamp_dl_en;
    BSP_U32 ipf_cfg_dlbd_count;
    BSP_U32 ipf_dlbd_done_count;
    BSP_U32 ipf_get_dlrd_count;
    BSP_U32 ipf_cfg_dlad0_count;
    BSP_U32 ipf_cfg_dlad1_count;
    BSP_U32 ipf_acore_suspend_count;
    BSP_U32 ipf_acore_resume_count;

}IPF_DEBUG_INFO_S;

#ifdef __VXWORKS__

typedef struct tagIPF_PWRCTL_FILTER_INFO_S
{
	BSP_U32 u32Flag;
	IPF_CHANNEL_TYPE_E eChnType;
	BSP_U32 u32FilterNum;
	IPF_FILTER_CONFIG_S *pstFilterInfo;
}IPF_PWRCTL_FILTER_INFO_S;
#endif

/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/



/**************************************************************************
  函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : ipf_init
*
* 功能描述  : IPF初始化
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    初始化成功
*             IPF_ERROR      初始化失败
*
* 修改记录  :2011年1月21日   鲁婷  创建
*****************************************************************************/
BSP_S32 ipf_init(BSP_VOID);

/*****************************************************************************
* 函 数 名     : IPF_Int_Connect
*
* 功能描述  : 挂IPF中断处理函数(两核都提供)
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值      : 无
*
* 修改记录  :2011年12月2日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_Int_Connect(BSP_VOID);

void ipf_write_basic_filter(u32 filter_hw_id, IPF_MATCH_INFO_S* match_infos);

/*****************************************************************************
* 函 数 名  : IPF_IntHandler
*
* 功能描述  : IPF中断处理函数
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月24日   鲁婷  创建
*****************************************************************************/
#ifdef __KERNEL__
irqreturn_t  IPF_IntHandler (int irq, void* dev);
#endif

#ifdef __VXWORKS__
BSP_VOID IPF_IntHandler(BSP_VOID);
#endif

#ifdef __VXWORKS__
/*****************************************************************************
* 函 数 名  : IPF_FilterList_Init
*
* 功能描述  : IPF过滤器链表初始化     内部使用，不作为接口函数
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    过滤器链表初始化成功
*             IPF_ERROR      过滤器链表初始化失败
*
* 修改记录  :2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FilterList_Init(BSP_VOID);

/*****************************************************************************
* 函 数 名  : IPF_DeleteAll
*
* 功能描述  : 删除链表中的所有结点
*
* 输入参数  : IPF_FILTER_CHAIN_TYPE_E eChnType  过滤器链类型
*
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  :2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_DeleteAll(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead);

/*****************************************************************************
* 函 数 名  : IPF_MallocOneFilter
*
* 功能描述  : 从freelist中分配一个结点
*
* 输入参数  : 无
*
* 输出参数  : 无

* 返 回 值  : 结点指针
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*
* 说明      : 由调用函数来保证一定能分配到结点
*****************************************************************************/
IPF_ID_S* IPF_MallocOneFilter(IPF_ID_S* g_stIPFFreeList);

/*****************************************************************************
* 函 数 名  : IPF_AddTailUsedFilter
*
* 功能描述  : 将结点加到uselist的结尾
*
* 输入参数  : IPF_FILTER_CHAIN_TYPE_E eFilterChainhead 过滤器链类型
*             IPF_ID_S* stNode             待插入的结点指针
*
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddTailUsedFilter(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, IPF_ID_S* stNode);

/*****************************************************************************
* 函 数 名  : IPF_AddTailFilterChain
*
* 功能描述  : 将新的过滤器配置配置到寄存器，并修改上一个过滤器的nextindex
*
* 输入参数  : BSP_U32 u32LastFilterID  上一个过滤器的ID
*             BSP_U32 u32FilterID  当前需配置的过滤器ID
*             IPF_MATCH_INFO_S* pstMatchInfo  当前需配置的过滤器参数结构体指针
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年5月12日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddTailFilterChain(BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo);

/*****************************************************************************
* 函 数 名     : IPF_FindFilterID
*
* 功能描述  : 在上下行链表中寻找与PS ID 匹配的Filter ID
*
* 输入参数  : IPF_FILTER_CHAIN_TYPE_E eFilterChainhead  过滤器链类型
*                           BSP_U32 u32PsID                PS ID
*
* 输出参数  : BSP_U32* u32FilterID   查询到的Filter ID

* 返 回 值     : IPF_SUCCESS                查询成功
*                          IPF_ERROR                  查询失败
*
* 修改记录  :2011年1月11日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FindFilterID(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, BSP_U32 u32PsID, BSP_U32* u32FilterID);

/*****************************************************************************
* 函 数 名      : IPF_ConfigCD
*
* 功能描述  : 配置CD
*
* 输入参数  : IPF_TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr
* 输出参数  : 无
* 返 回 值      :  无
*
* 修改记录  :2011年1月24日   鲁婷  创建
*****************************************************************************/
//BSP_S32 IPF_ConfigCD(TTF_MEM_ST* pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr);
#endif

#ifdef __CMSIS_RTOS
/*****************************************************************************
* 函 数 名       : ipf_drx_bak_reg
*
* 功能描述  : 提供的低功耗接口，保存IPF相关信息
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值     : IPF_SUCCESS     IPF可以下电，相关信息已经保存
*                           IPF_ERROR        IPF不可以下电
*
* 修改记录  : 2011年2月14日   鲁婷  创建
*****************************************************************************/
s32 ipf_drx_bak_reg(void);

/*****************************************************************************
* 函 数 名       : ipf_drx_restore_reg
*
* 功能描述  : 提供的低功耗接口，用于IPF上电恢复
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值     : 无
*
* 注意:该函数需要在锁中断的情况下调用
* 修改记录  : 2011年2月14日   鲁婷  创建
*****************************************************************************/
void ipf_drx_restore_reg(void);

#ifdef CONFIG_BALONG_MODEM_RESET

int bsp_ipf_reset_ccore_lpm3_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);
#endif

#endif

#if (defined(BSP_CONFIG_HI3630) && defined(__KERNEL__))
static __inline__ unsigned ipf_readl(u32 addr)
{
    return readl((const volatile void *)(addr));
}
static __inline__ void ipf_writel(u32 val, u32 addr)
{
    writel(val, (volatile void *)addr);
}
#else
static __inline__ unsigned ipf_readl(u32 addr)
{
    return readl(addr);
}
static __inline__ void ipf_writel(u32 val, u32 addr)
{
    writel(val, addr);
}
#endif


/*******************可维可测函数****************************************/
BSP_VOID BSP_IPF_Help(BSP_VOID);
BSP_S32 BSP_IPF_Dump_BDInfo(IPF_CHANNEL_TYPE_E eChnType);
BSP_S32 BSP_IPF_Dump_RDInfo(IPF_CHANNEL_TYPE_E eChnType);
BSP_S32 BSP_IPF_Dump_ADInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32AdType);
BSP_S32 BSP_IPF_Info(IPF_CHANNEL_TYPE_E eChnType);
BSP_VOID BSP_IPF_MEM(BSP_VOID);
/*******************可维可测函数****************************************/

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_DMAC_H_ */

