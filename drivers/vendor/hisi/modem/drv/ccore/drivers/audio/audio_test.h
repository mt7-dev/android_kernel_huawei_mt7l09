

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __AUDIO_TEST_H__
#define __AUDIO_TEST__

#include "bsp_memmap.h"
#include "drv_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_DMA_BASE_ADDR               (HI_EDMA_REGBASE_ADDR_VIRT)                           /* DMA在V7R1中的基地址 */
#define DRV_SC_DMA_SEL_CTRL_ADDR        (0x900004E0U)                           /* V7R1中DMA选择控制寄存器 */

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 2.1 外部直接引用定义 */
#define DRV_DMA_LLI_LINK(uwAddr)        (((uwAddr) & 0xffffffe0UL) | (0x2UL))   /* 链表链接LLI配置字 */
#define DRV_DMA_LLI_CHAN(uhwChanNo)     (((unsigned int)(uhwChanNo) << 8)|(0x1UL))/* 通道链接LLI配置字 */
#define DRV_DMA_LLI_DISABLE             (0x0UL)                                 /* 链接模式不使能配置字 */

#define DRV_DMA_MEM_SIO_CFG             (0x83311057)                            /* 典型的MEM->SIO的DMA配置，源地址递增，16bit位宽burst4 */
#define DRV_DMA_SIO_MEM_CFG             (0x43311047)                            /* 典型的SIO->MEM的DMA配置，目的地址递增，16bit位宽burst4 */
#define DRV_DMA_MEM_MEM_CFG             (0xfff33003)                            /* 典型的MEM->MEM的DMA配置，地址双向递增，64bit位宽burst1  */

/* 2.2 内部引用定义 */
#define DRV_DMA_MAX_CHANNEL_NUM         (16)                                    /* 最大通道数                              */

#define DRV_DMA_CPU_NO                  (0)//(2)                               /* EDMA上处理器编号                    */
#define DRV_DMA_INT_STAT_CPU(i)             (DRV_DMA_BASE_ADDR+(       (0x40*i)))  /* 处理器i的中断状态寄存器                 */
#define DRV_DMA_INT_TC1_CPU(i)              (DRV_DMA_BASE_ADDR+(0x0004+(0x40*i)))  /* 处理器i的通道传输完成中断状态寄存器     */
#define DRV_DMA_INT_TC2_CPU(i)              (DRV_DMA_BASE_ADDR+(0x0008+(0x40*i)))  /* 处理器i的链表节点传输完成中断状态寄存器 */
#define DRV_DMA_INT_ERR1_CPU(i)             (DRV_DMA_BASE_ADDR+(0x000c+(0x40*i)))  /* 处理器i的配置错误中断状态寄存器         */
#define DRV_DMA_INT_ERR2_CPU(i)             (DRV_DMA_BASE_ADDR+(0x0010+(0x40*i)))  /* 处理器i的数据传输错误中断状态寄存器     */
#define DRV_DMA_INT_ERR3_CPU(i)             (DRV_DMA_BASE_ADDR+(0x0014+(0x40*i)))  /* 处理器i的读链表错误中断状态寄存器       */
#define DRV_DMA_INT_TC1_MASK_CPU(i)         (DRV_DMA_BASE_ADDR+(0x0018+(0x40*i)))  /* 处理器i的通道传输完成中断屏蔽寄存器     */
#define DRV_DMA_INT_TC2_MASK_CPU(i)         (DRV_DMA_BASE_ADDR+(0x001c+(0x40*i)))  /* 处理器i的链表节点传输完成中断屏蔽寄存器 */
#define DRV_DMA_INT_ERR1_MASK_CPU(i)        (DRV_DMA_BASE_ADDR+(0x0020+(0x40*i)))  /* 处理器i的配置错误中断屏蔽寄存器         */
#define DRV_DMA_INT_ERR2_MASK_CPU(i)        (DRV_DMA_BASE_ADDR+(0x0024+(0x40*i)))  /* 处理器i的数据传输错误中断屏蔽寄存器     */
#define DRV_DMA_INT_ERR3_MASK_CPU(i)        (DRV_DMA_BASE_ADDR+(0x0028+(0x40*i)))  /* 处理器i的链表读取错误中断屏蔽寄存器     */
#define DRV_DMA_INT_TC1_RAW             (DRV_DMA_BASE_ADDR+(0x0600))            /* 原始通道传输完成中断状态寄存器          */
#define DRV_DMA_INT_TC2_RAW             (DRV_DMA_BASE_ADDR+(0x0608))            /* 原始链表节点传输完成中断状态寄存器      */
#define DRV_DMA_INT_ERR1_RAW            (DRV_DMA_BASE_ADDR+(0x0610))            /* 原始配置错误中断状态寄存器              */
#define DRV_DMA_INT_ERR2_RAW            (DRV_DMA_BASE_ADDR+(0x0618))            /* 原始数据传输错误中断状态寄存器          */
#define DRV_DMA_INT_ERR3_RAW            (DRV_DMA_BASE_ADDR+(0x0620))            /* 原始链表读取错误中断状态寄存器          */
#define DRV_DMA_SREQ                    (DRV_DMA_BASE_ADDR+(0x0660))            /* 单传输请求寄存器                        */
#define DRV_DMA_LSREQ                   (DRV_DMA_BASE_ADDR+(0x0664))            /* 末次单传输请求寄存器                    */
#define DRV_DMA_BREQ                    (DRV_DMA_BASE_ADDR+(0x0668))            /* 突发传输请求寄存器                      */
#define DRV_DMA_LBREQ                   (DRV_DMA_BASE_ADDR+(0x066C))            /* 末次突发传输请求寄存器                  */
#define DRV_DMA_FREQ                    (DRV_DMA_BASE_ADDR+(0x0670))            /* 批量传输请求寄存器                      */
#define DRV_DMA_LFREQ                   (DRV_DMA_BASE_ADDR+(0x0674))            /* 末次批量传输请求寄存器                  */
#define DRV_DMA_CH_PRI                  (DRV_DMA_BASE_ADDR+(0x0688))            /* 优先级控制寄存器                        */
#define DRV_DMA_CH_STAT                 (DRV_DMA_BASE_ADDR+(0x0690))            /* 全局DMA状态寄存器                       */
#define DRV_DMA_DMA_CTRL                (DRV_DMA_BASE_ADDR+(0x0698))            /* DMA全局控制寄存器                       */
#define DRV_DMA_CX_CURR_CNT1(j)         (DRV_DMA_BASE_ADDR+(0x0700+(0x10*j)))  /* 通道j的三维传输剩余size状态寄存器       */
#define DRV_DMA_CX_CURR_CNT0(j)         (DRV_DMA_BASE_ADDR+(0x0704+(0x10*j)))  /* 通道j的一、二维传输剩余size状态寄存器   */
#define DRV_DMA_CX_CURR_SRC_ADDR(j)     (DRV_DMA_BASE_ADDR+(0x0708+(0x10*j)))  /* 通道j的源地址寄存器                     */
#define DRV_DMA_CX_CURR_DES_ADDR(j)     (DRV_DMA_BASE_ADDR+(0x070C+(0x10*j)))  /* 通道j的目的地址寄存器                   */
#define DRV_DMA_CX_LLI(j)               (DRV_DMA_BASE_ADDR+(0x0800+(0x40*j)))  /* 通道j的链表地址寄存器                   */
#define DRV_DMA_CX_BINDX(j)             (DRV_DMA_BASE_ADDR+(0x0804+(0x40*j)))  /* 通道j的二维地址偏移量配置寄存器         */
#define DRV_DMA_CX_CINDX(j)             (DRV_DMA_BASE_ADDR+(0x0808+(0x40*j)))  /* 通道j的三维地址偏移量配置寄存器         */
#define DRV_DMA_CX_CNT1(j)              (DRV_DMA_BASE_ADDR+(0x080C+(0x40*j)))  /* 通道j的传输长度1配置寄存器              */
#define DRV_DMA_CX_CNT0(j)              (DRV_DMA_BASE_ADDR+(0x0810+(0x40*j)))  /* 通道j的传输长度配置寄存器               */
#define DRV_DMA_CX_SRC_ADDR(j)          (DRV_DMA_BASE_ADDR+(0x0814+(0x40*j)))  /* 通道j的源地址寄存器                     */
#define DRV_DMA_CX_DES_ADDR(j)          (DRV_DMA_BASE_ADDR+(0x0818+(0x40*j)))  /* 通道j的目的地址寄存器                   */
#define DRV_DMA_CX_CONFIG(j)            (DRV_DMA_BASE_ADDR+(0x081C+(0x40*j)))  /* 通道j的配置寄存器                       */
#define DRV_DMA_CX_AXI_CONF(j)          (DRV_DMA_BASE_ADDR+(0x0820+(0x40*j)))  /* 通道j的AXI特殊操作配置寄存器            */

#define DRV_DMA_INT_STAT           (DRV_DMA_INT_STAT_CPU(DRV_DMA_CPU_NO))        
#define DRV_DMA_INT_TC1            (DRV_DMA_INT_TC1_CPU(DRV_DMA_CPU_NO))          
#define DRV_DMA_INT_TC2            (DRV_DMA_INT_TC2_CPU(DRV_DMA_CPU_NO))          
#define DRV_DMA_INT_ERR1           (DRV_DMA_INT_ERR1_CPU(DRV_DMA_CPU_NO))        
#define DRV_DMA_INT_ERR2           (DRV_DMA_INT_ERR2_CPU(DRV_DMA_CPU_NO))         
#define DRV_DMA_INT_ERR3           (DRV_DMA_INT_ERR3_CPU(DRV_DMA_CPU_NO))        
#define DRV_DMA_INT_TC1_MASK       (DRV_DMA_INT_TC1_MASK_CPU(DRV_DMA_CPU_NO))     
#define DRV_DMA_INT_TC2_MASK       (DRV_DMA_INT_TC2_MASK_CPU(DRV_DMA_CPU_NO))    
#define DRV_DMA_INT_ERR1_MASK      (DRV_DMA_INT_ERR1_MASK_CPU(DRV_DMA_CPU_NO))    
#define DRV_DMA_INT_ERR2_MASK      (DRV_DMA_INT_ERR2_MASK_CPU(DRV_DMA_CPU_NO))   
#define DRV_DMA_INT_ERR3_MASK      (DRV_DMA_INT_ERR3_MASK_CPU(DRV_DMA_CPU_NO))   

#define DRV_DMA_GetCxIsrPtr()           (&g_astDrvDmaCxIntIsr[0])               /* 获取DMA中断处理函数全局变量首地址 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 函 数 名  : DRV_DMA_INT_TYPE_ENUM
 功能描述  : DMA通道中断类型
*****************************************************************************/
typedef enum DRV_DMA_INT_TYPE_ENUM
{
    DRV_DMA_INT_TYPE_TC1 = 0,           /* 通道完成     */
    DRV_DMA_INT_TYPE_TC2,               /* 链表结点完成(除最后一个结点) */
    DRV_DMA_INT_TYPE_ERR1,              /* 配置错误     */
    DRV_DMA_INT_TYPE_ERR2,              /* 数据传输错误 */
    DRV_DMA_INT_TYPE_ERR3,              /* 读取链表错误 */
    DRV_DMA_INT_TYPE_BUTT
}drv_dma_int_type;

/*转定义32比特读函数*/
#define DMA_REG_READ(uwAddr)                    (*((volatile unsigned int *)(uwAddr)))

/*转定义32比特写函数*/
#define DMA_REG_WRITE(uwAddr, uwValue)         (*((volatile unsigned int *)(uwAddr)) = uwValue)

/*转定义按比特读函数*/
#define DMA_REG_READBIT(uwAddr, uhwStartBit, uhwEndBit)               \
    BSP_REG_GETBITS(uwAddr, 0,  uhwStartBit, uhwEndBit - uhwStartBit)

/*转定义按比特写函数*/
#define DMA_REG_WRITEBIT(uwAddr, uhwStartBit, uhwEndBit, swContent)    \
    BSP_REG_SETBITS(uwAddr, 0, uhwStartBit, uhwEndBit - uhwStartBit, swContent)

typedef enum DMA_BIT_ENUM
{
    DMA_BIT0 = 0,
    DMA_BIT1 ,
    DMA_BIT2 ,
    DMA_BIT3 ,
    DMA_BIT4 ,
    DMA_BIT5 ,
    DMA_BIT6 ,
    DMA_BIT7 ,
    DMA_BIT8 ,
    DMA_BIT9 ,
    DMA_BIT10,
    DMA_BIT11,
    DMA_BIT12,
    DMA_BIT13,
    DMA_BIT14,
    DMA_BIT15,
    DMA_BIT16,
    DMA_BIT17,
    DMA_BIT18,
    DMA_BIT19,
    DMA_BIT20,
    DMA_BIT21,
    DMA_BIT22,
    DMA_BIT23,
    DMA_BIT24,
    DMA_BIT25,
    DMA_BIT26,
    DMA_BIT27,
    DMA_BIT28,
    DMA_BIT29,
    DMA_BIT30,
    DMA_BIT31,
    DMA_BIT_BUTT
}dma_bit;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : DRV_DMA_CXCFG_STRU
 功能描述  : EMAC通道配置实体

LLI寄存器含义:
   b31-5, 链表地址b31-5, 即32byte对齐
   b11-8, 链接通道号
   b1-0,  通道链接使能, 00-不使能;01-通道链接;10-链表链接;

 config寄存器含义:
   b31 b30 b29   b28  b27-24 b23-20 b18-16 b14-12 b9-4   b3-2      b1    b0
   si  di smode dmode   sl     dl    sw     dw    peri flow_ctrl itc_en ch_en
 其中,
   sl/dl, src/dest burst len, 取值0000-1111表示长度为1-16
   sw/dw, src/dest width, 取值000-101表示宽度为8/16/32/64/128/256bit
   flow_ctrl, 00-M2M;01-M2P;10-M2P外设流控
*****************************************************************************/
typedef struct
{
   unsigned int       uwLli;              /* 通道Linked List Item地址     */
   unsigned short       uhwDstBIndex;       /* 通道的二维目的地址偏移量配置，占低16bit */
   unsigned short       uhwSrcBIndex;       /* 通道的二维源地址偏移量配置，占高16bit */
   unsigned short       uhwDstCIndex;       /* 通道的三维目的地址偏移量配置，占低16bit */
   unsigned short       uhwSrcCIndex;       /* 通道的三维源地址偏移量配置，占高16bit */
   unsigned short       uhwCCount;          /* 三维传输长度配置,单位Frame，占低16bit   */
   unsigned short       uhwRsv;             /* 保留位，占高16bit                    */
   unsigned short       uhwACount;          /* 一维传输长度配置,单位Byte，占低16bit */
   unsigned short       uhwBCount;          /* 二维传输长度配置,单位Array，占高16bit  */
   unsigned int       uwSrcAddr;          /* 通道源地址                   */
   unsigned int       uwDstAddr;          /* 通道目的地址                 */
   unsigned int       uwConfig;           /* 通道配置寄存器               */

} DRV_DMA_CXCFG_STRU __attribute__((aligned(32)));

/*****************************************************************************
 函 数 名  : drv_dma_int_func
 功能描述  : DMA中断处理回调函数
 输入参数  : enIntType - 中断类型, 参见drv_dma_int_type
             uwPara    - 用户参数
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
typedef void (*drv_dma_int_func)(
                drv_dma_int_type enIntType,
                unsigned int                   uwPara);

/*****************************************************************************
 实体名称  : DRV_DMA_CXISR_STRU
 功能描述  : 定义DMA通道回调函数数据实体
*****************************************************************************/
typedef struct
{
    drv_dma_int_func    pfFunc;         /* 回调函数指针 */
    unsigned int          uwPara;         /* 回调参数     */

} DRV_DMA_CXISR_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

void bsp_dma_init( void );
void bsp_dma_isr(void);
unsigned int bsp_dma_startwithcfg(
                       unsigned short              uhwChannelNo,
                       DRV_DMA_CXCFG_STRU     *pstCfg,
                       drv_dma_int_func        pfIntHandleFunc,
                       unsigned int              uwPara);
void bsp_dma_stop(unsigned short uhwChannelNo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of drv_dma.h */
