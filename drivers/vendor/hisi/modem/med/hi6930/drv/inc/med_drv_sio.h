
#ifndef __MED_DRV_SIO_H__
#define __MED_DRV_SIO_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "soc_baseaddr_interface.h"
#include "soc_sio_interface.h"
#include "CodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#if (VOS_CPU_TYPE == VOS_HIFI)
#define DRV_SC_CTRL_ADDR_AUDIO     (SOC_SCTRL_SC_CLKCFGALL0_ADDR(SOC_SC_OFF_BASE_ADDR))    /* 根据V9R1设计，VOICE的时钟控制寄存器使用SIO1 */
#define DRV_SC_CTRL_ADDR_VOICE     (SOC_SCTRL_SC_CLKCFGALL1_ADDR(SOC_SC_OFF_BASE_ADDR))    /* 系统控制寄存器 */
#define DRV_SC_CTRL_ADDR_SMART_PA  (SOC_SCTRL_SC_CLKCFGALL2_ADDR(SOC_SC_OFF_BASE_ADDR))    /* 系统控制寄存器 */

#define DRV_SC_CLK_SEL1_ADDR       (SOC_SCTRL_SC_CLK_SEL1_ADDR(SOC_SC_OFF_BASE_ADDR))
#define DRV_SC_CLK_ENABLE_ADDR     (SOC_SCTRL_SC_PERIPH_CLKEN3_ADDR(SOC_SC_OFF_BASE_ADDR))  /* V9R1中外设时钟使能寄存器 */
#define DRV_SC_CLK_DISABLE_ADDR    (SOC_SCTRL_SC_PERIPH_CLKDIS3_ADDR(SOC_SC_OFF_BASE_ADDR)) /* V9R1中外设时钟关闭寄存器 */

#define DRV_SIO_BASE_ADDR_AUDIO    VOS_NULL                                     /* SIO基地址(虚地址) */
#define DRV_SIO_BASE_ADDR_VOICE    (0x900A7000U)                                /* SIO基地址(虚地址) */
#define DRV_SIO_BASE_ADDR_SMART_PA VOS_NULL                                     /* SIO基地址(虚地址) */

#else
#define DRV_SC_CTRL_ADDR_AUDIO     ((VOS_UINT32)SOC_SCTRL_SC_CLKCFGALL0_ADDR(&g_auwRegStub[0]))
#define DRV_SC_CTRL_ADDR_VOICE     ((VOS_UINT32)SOC_SCTRL_SC_CLKCFGALL1_ADDR(&g_auwRegStub[0]))
#define DRV_SC_CTRL_ADDR_SMART_PA  ((VOS_UINT32)SOC_SCTRL_SC_CLKCFGALL2_ADDR(&g_auwRegStub[0]))

#define DRV_SC_CLK_SEL1_ADDR       ((VOS_UINT32)SOC_SCTRL_SC_CLK_SEL1_ADDR(&g_auwRegStub[0]))
#define DRV_SC_CLK_ENABLE_ADDR     ((VOS_UINT32)SOC_SCTRL_SC_PERIPH_CLKEN3_ADDR(&g_auwRegStub[0]))
#define DRV_SC_CLK_DISABLE_ADDR    ((VOS_UINT32)SOC_SCTRL_SC_PERIPH_CLKDIS3_ADDR(&g_auwRegStub[0]))

#define DRV_SIO_BASE_ADDR_AUDIO    ((VOS_UINT32)&g_auwRegStub[0x0c00])
#define DRV_SIO_BASE_ADDR_VOICE    ((VOS_UINT32)&g_auwRegStub[0x0d00])
#define DRV_SIO_BASE_ADDR_SMART_PA ((VOS_UINT32)&g_auwRegStub[0x0e00])
#define DRV_SC_CLK_RST_DIS_ADDR    ((VOS_UINT32)&g_auwRegStub[0x0f00])
#define DRV_SIO_MODE_SUBSYS_CTRL   ((VOS_UINT32)&g_auwRegStub[0x0f70])
#endif

#define DRV_SIO_VOICE_MIC_CHANNEL_NUM   (1)                                     /* MicIn  SIO通道数 */
#define DRV_SIO_VOICE_SPK_CHANNEL_NUM   (1)
#define DRV_SIO_CHANNEL_NUM_PCM         (1)
#define DRV_SIO_CHANNEL_NUM_I2S         (2)


/* V7R2 SIO CTRL3配置地址 */
#define DRV_SC_CTRL3_ADDR             ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x040c))

/* V7R2 SIO主从配置bit位 */
#define DRV_SC_SIO_MASTER_MODE_BIT    (UCOM_BIT3)

/* V7R2 SIO时钟分频控制寄存器配置地址 */
#define DRV_CRG_CLKDIV1_ADDR          ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x0100))

/* V7R2 SIO时钟xfs分频bit位最低位 */
#define DRV_CRG_PCM_SYNC_DIV_BIT_LO   (UCOM_BIT16)

/* V7R2 SIO时钟xfs分频bit位最高位 */
#define DRV_CRG_PCM_SYNC_DIV_BIT_HI   (UCOM_BIT27)

/* V7R2 SIO时钟CLK分频bit位最低位 */
#define DRV_CRG_PCM_BCLK_DIV_BIT_LO   (UCOM_BIT0)

/* V7R2 SIO时钟CLK分频bit位最高位 */
#define DRV_CRG_PCM_BCLK_DIV_BIT_HI   (UCOM_BIT15)

/* V7R2 SIO总线时钟使能配置地址 */
#define DRV_CRG_CLKEN3_ADDR           ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x0018))

/* V7R2 SIO使能配置bit位 */
#define DRV_SC_LTE_SIO_CLK_EN_BIT     (UCOM_BIT29)

/* V7R2 SIO总线时钟使能配置地址 */
#define DRV_CRG_CLKDIS3_ADDR          ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x001c))

/* V7R2 SIO不使能配置bit位 */
#define DRV_SC_LTE_SIO_CLK_DIS_BIT    (UCOM_BIT29)

/* V7R2 SIO总线时钟使能状态查询地址 */
#define DRV_CRG_CLKSTAT3_ADDR         ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x0020))

/* V7R2 SIO使能状态bit位 */
#define DRV_SC_LTE_SIO_CLK_STATUS_BIT (UCOM_BIT29)

/* V7R2 SIO DMA控制寄存器 */
#define DRV_SC_CTRL54_ADDR            ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x04d8))

/* V7R2 SIO DMA通道选择bit位*/
#define DRV_SC_SIO_DMA_SEL_BIT        (UCOM_BIT2)

/* V7R2 SIO SLAVE时，时钟同向/反向配置寄存器 */
#define DRV_SC_SIO_CLK_POSITIVE       ((VOS_UINT32)(DRV_SC_CRG_BASE_ADDR + 0x0140))
#define DRV_SC_SIO_CLK_POST_SEL_BIT   (UCOM_BIT6)

/*****************************************************************************
 实体名称  : DRV_SIO_I2S_DUAL_RX_CHN_ADDR
 功能描述  : I2S读取接收数据寄存器。
*****************************************************************************/
#define DRV_SIO_I2S_DUAL_RX_CHN_ADDR(uwAddr)    SOC_SIO_I2S_DUAL_RX_CHN_ADDR(uwAddr)

/*****************************************************************************
 实体名称  : DRV_SIO_I2S_DUAL_TX_CHN_ADDR
 功能描述  : I2S写发送数据寄存器。
*****************************************************************************/
#define DRV_SIO_I2S_DUAL_TX_CHN_ADDR(uwAddr)    SOC_SIO_I2S_DUAL_TX_CHN_ADDR(uwAddr)

/*****************************************************************************
 实体名称  : DRV_SIO_PCM_RD_ADDR
 功能描述  : PCM读取接收数据寄存器。
*****************************************************************************/
#define DRV_SIO_PCM_RD_ADDR(uwAddr)             SOC_SIO_PCM_RD_ADDR(uwAddr)

/*****************************************************************************
 实体名称  : DRV_SIO_PCM_XD_ADDR
 功能描述  : PCM写发送数据寄存器。
*****************************************************************************/
#define DRV_SIO_PCM_XD_ADDR(uwAddr)             SOC_SIO_PCM_XD_ADDR(uwAddr)

/*****************************************************************************
 实体名称  : DRV_SIO_GetCfgPtr
 功能描述  : 获取SIO状态寄存器指针
*****************************************************************************/
#define DRV_SIO_GetAudioCfgPtr()                VOS_NULL//(&g_stDrvSioAudioCfg)无相关SIO
#define DRV_SIO_GetVoiceCfgPtr()                (&g_stDrvSioVoiceCfg)
#define DRV_SIO_GetSmartPaCfgPtr()              VOS_NULL//(&g_stDrvSioSmartPaCfg)无相关SIO

/*****************************************************************************
 实体名称  : DRV_SIO_SetSyncMode
 功能描述  : 设置同步模式master / slave,
             mode 数据类型使用DRV_SIO_SYNC_MODE_ENUM_UINT32
*****************************************************************************/
//#define DRV_SIO_SetSyncMode(mode)               (g_stDrvSioVoiceCfg.enSyncMode=(mode))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : DRV_SIO_INT_MASK_ENUM
 功能描述  : SIO各类中断掩码
*****************************************************************************/
enum DRV_SIO_INT_MASK_ENUM
{
    DRV_SIO_INT_MASK_RX_INTR                              = 0x1,                /* 接收FIFO高于阈值中断                 */
    DRV_SIO_INT_MASK_TX_INTR                              = 0x2,                /* 发送FIFO低于阈值中断                 */
    DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER                   = 0x4,                /* I2S模式下为右声道接收FIFO上溢出中断  */
                                                                                /* PCM模式下为PCM接收FIFO上溢出中断     */
    DRV_SIO_INT_MASK_RX_LEFT_FIFO_OVER                    = 0x8,                /* 左声道接收FIFO上溢出,只在I2S模式有效 */
    DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER                  = 0x10,               /* I2S模式下为右声道发送FIFO下溢出中断  */
                                                                                /* PCM模式下为PCM发送FIFO下溢出中断     */
    DRV_SIO_INT_MASK_TX_LEFT_FIFO_UNDER                   = 0x20,               /* 左声道接收FIFO下溢出,只在I2S模式有效 */
    DRV_SIO_INT_MASK_BUTT                                 = 0x40
};
typedef VOS_UINT32 DRV_SIO_INT_MASK_ENUM_UINT32;

/*****************************************************************************
 实体名称  : DRV_SIO_CLK_STATUS_ENUM
 功能描述  : SIO时钟工作状态
*****************************************************************************/
enum DRV_SIO_CLK_STATUS_ENUM
{
    DRV_SIO_CLK_STATUS_CLOSE                                = 0,                /* SIO时钟关闭 */
    DRV_SIO_CLK_STATUS_OPEN,                                                    /* SIO时钟打开 */
    DRV_SIO_CLK_STATUS_BUTT
};
typedef VOS_UINT32 DRV_SIO_CLK_STATUS_ENUM_UINT32;

/*****************************************************************************
 实体名称  : DRV_SIO_MODE_ENUM
 功能描述  : SIO工作模式
*****************************************************************************/
enum DRV_SIO_MODE_ENUM
{
    DRV_SIO_MODE_I2S                                        = 0,                /* SIO I2S工作模式 */
    DRV_SIO_MODE_PCM,                                                           /* SIO PCM工作模式 */
    DRV_SIO_MODE_BUTT
};
typedef VOS_UINT32 DRV_SIO_MODE_ENUM_UINT32;

/*****************************************************************************
 实体名称  : DRV_SIO_SAMPLING_ENUM
 功能描述  : SIO采样率
*****************************************************************************/
enum DRV_SIO_SAMPLEING_ENUM
{
    DRV_SIO_SAMPLING_8K                                   = 8000,               /* 采样率为8k */
    DRV_SIO_SAMPLING_16K                                  = 16000,              /* 采样率为16k */
    DRV_SIO_SAMPLING_48K                                  = 48000,              /* 采样率为48k */
    DRV_SIO_SAMPLING_BUTT
};
typedef VOS_UINT32 DRV_SIO_SAMPLING_ENUM_UINT32;

/*****************************************************************************
 实体名称  : DRV_SIO_SYNC_MODE_ENUM
 功能描述  : SIO同步模式配置
*****************************************************************************/
enum DRV_SIO_SYNC_MODE_ENUM
{
    DRV_SIO_SYNC_MODE_SLAVE                                 = 0,                /* 从模式, 需要模拟CODEC输入时钟信号 */
    DRV_SIO_SYNC_MODE_MASTER,                                                   /* 主模式, 需要输出时钟信号到模拟CODEC */
    DRV_SIO_SYNC_MODE_BUTT
};
typedef VOS_UINT32 DRV_SIO_SYNC_MODE_ENUM_UINT32;


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
 函 数 名  : DRV_SIO_INT_FUNC
 功能描述  : SIO中断处理回调函数
 输入参数  : enIntType - 中断类型, 参见DRV_SIO_INT_TYPE_ENUM_UINT16
             uwPara    - 用户参数
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
typedef VOS_VOID (*DRV_SIO_INT_FUNC)(
                     DRV_SIO_INT_MASK_ENUM_UINT32 enIntType,
                     VOS_UINT32                   uwPara);


/*****************************************************************************
 实体名称  : DRV_SIO_INT_ISR_STRU
 功能描述  : 定义SIO中断回调函数数据实体
*****************************************************************************/
typedef struct
{
    DRV_SIO_INT_MASK_ENUM_UINT32        enIntMask;                              /* SIO中断掩码 */
    DRV_SIO_INT_FUNC                    pfFunc;                                 /* 回调函数指针 */
    VOS_UINT32                          uwPara;                                 /* 回调参数     */

} DRV_SIO_INT_ISR_STRU;

/*****************************************************************************
 实体名称  : DRV_SIO_CFG_STRU
 功能描述  : 定义SIO配置实体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwSioBaseAddr;                          /* SIO基地址寄存器 */
    DRV_SIO_CLK_STATUS_ENUM_UINT32      enClkStatus;                            /* 保存SIO时钟控制 */
    DRV_SIO_MODE_ENUM_UINT32            enWorkMode;                             /* I2S / PCM模式配置 */
    DRV_SIO_SYNC_MODE_ENUM_UINT32       enSyncMode;                             /* slave / master */
    DRV_SIO_INT_ISR_STRU                stIsr;                                  /* 保存SIO中断回调 */
} DRV_SIO_CFG_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern DRV_SIO_CFG_STRU                 g_stDrvSioVoiceCfg;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID DRV_SIO_Audio_Close(VOS_VOID);
extern VOS_VOID DRV_SIO_Audio_CloseClk(VOS_VOID);
extern VOS_UINT32 DRV_SIO_Audio_Loop(DRV_SIO_SAMPLING_ENUM_UINT32  enSamplingType);
extern VOS_UINT32 DRV_SIO_Audio_Open(
                       DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                       DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                       VOS_UINT16                      uhwIsModeMaster,
                       DRV_SIO_INT_FUNC                pfIntHandleFunc,
                       VOS_UINT32                      uwPara);
extern VOS_VOID DRV_SIO_Audio_OpenClk(VOS_VOID);
extern VOS_VOID DRV_SIO_Close(DRV_SIO_CFG_STRU *pstCfg);
extern VOS_UINT32 DRV_SIO_GetRxRegAddr( DRV_SIO_CFG_STRU   *pstCfg );
extern VOS_UINT32 DRV_SIO_GetSioChannelNum( DRV_SIO_CFG_STRU   *pstCfg );
extern VOS_UINT32 DRV_SIO_GetTxRegAddr(DRV_SIO_CFG_STRU   *pstCfg);
extern VOS_UINT32 DRV_SIO_GetVoiceMicChn(VOS_VOID);
extern VOS_UINT32 DRV_SIO_GetVoiceSpkChn(VOS_VOID);
extern VOS_VOID DRV_SIO_Init(VOS_VOID);
extern VOS_UINT32 DRV_SIO_Open(
                       DRV_SIO_CFG_STRU               *pstCfg,
                       DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                       DRV_SIO_INT_FUNC                pfIntHandleFunc,
                       VOS_UINT32                      uwPara);
extern VOS_VOID DRV_SIO_Reset(DRV_SIO_CFG_STRU   *pstCfg);
extern VOS_VOID DRV_SIO_SetMaster(
                       DRV_SIO_CFG_STRU             *pstCfg,
                       DRV_SIO_SAMPLING_ENUM_UINT32  enSamplingRate);
extern VOS_VOID DRV_SIO_SetSlave(DRV_SIO_CFG_STRU   *pstCfg);
extern VOS_VOID DRV_SIO_SmartPa_Close(VOS_VOID);
extern VOS_VOID DRV_SIO_SmartPa_CloseClk(VOS_VOID);
extern VOS_UINT32 DRV_SIO_SmartPa_Open(
                       DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                       DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                       VOS_UINT16                      uhwIsModeMaster,
                       DRV_SIO_INT_FUNC                pfIntHandleFunc,
                       VOS_UINT32                      uwPara);
extern VOS_VOID DRV_SIO_SmartPa_OpenClk(VOS_VOID);
extern VOS_VOID DRV_SIO_Voice_Close(VOS_VOID);
extern VOS_VOID DRV_SIO_Voice_CloseClk(VOS_VOID);
extern VOS_UINT32 DRV_SIO_Voice_GetWorkMode(VOS_UINT32 uwVoiceChanNum);
extern VOS_UINT32 DRV_SIO_Voice_Open(
                       DRV_SIO_INT_MASK_ENUM_UINT32    enIntMask,
                       DRV_SIO_SAMPLING_ENUM_UINT32    enSamplingType,
                       VOS_UINT16                      uhwIsModeMaster,
                       DRV_SIO_INT_FUNC                pfIntHandleFunc,
                       VOS_UINT32                      uwPara);
extern VOS_VOID DRV_SIO_Voice_OpenClk(VOS_VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_drv_sio.h */

