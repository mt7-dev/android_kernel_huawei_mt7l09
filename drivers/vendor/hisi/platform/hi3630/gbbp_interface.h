#ifndef __GBB_PINTERFACE_H__
#define __GBB_PINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "soc_interface.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define GBBP_ADDR_OFFSET                    (0x8000)

#ifndef BIT_X
#define BIT_X(num)                          (((unsigned long)0x01) << (num))
#endif

/*--------------------------------------------------------------------------------------------*
 *  BBP 寄存器地址
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * 1.1  前言
 *--------------------------------------------------------------------------------------------*/

/* 使用的bbp中断相关bit位 */
#define GBBP_FRM_INT_BIT                ( BIT_X(0) )                            /* 帧中断比特位 */
#define GBBP_SYNC_INT_BIT               ( BIT_X(2) )                            /* 同步搜索中断比特位 */
#define GBBP_DECODE_INT_BIT             ( BIT_X(3) )                            /* 译码数据上报中断比特位 */
#define GBBP_CFG_INT_BIT                ( BIT_X(8) )                            /* GSM配置中断比特位,4时隙配置中断 */
#define GBBP_MEAS_INT_BIT               ( BIT_X(28) )                           /* 测量中断比特位 */
#define GBBP_DEMODE_INT_BIT             ( BIT_X(29) )                           /* GSM解调中断比特位 */
#define GBBP_WCELL_CFG_INT_BIT          ( BIT_X(12) )                           /* 异系统配置中断 */

#define GBBP_WCELL_STOP_INT_BIT         ( BIT_X(13) )                           /* 异系统配置中断 */
#define GBBP_WCELL_DELAY_INT_BIT        ( BIT_X(14) )                           /* 异系统配置中断 */

#define GPHY_BG_W_CFG_INT_BIT           ( BIT_X(15) )                           /* 为W背景搜触发的自定义中断  */
#define GPHY_BG_RFF_OFF_INT_BIT         ( BIT_X(16) )                           /* 异常停止w任务时的处理 */

#define GBBP_GAUGE_INT_BIT              ( BIT_X(27) )                           /* 当校准32.768KHz时钟计数结束之后，产生校准中断 */

#define GBBP_LMEAS_INT0_INT_BIT         ( BIT_X(24) )                           /* LTE测量时相对于CTU的INT0中断(Occasion预处理中断) */
#define GBBP_LMEAS_INT1_INT_BIT         ( BIT_X(25) )                           /* LTE测量时相对于CTU的INT1中断(Occasion开始中断) */
#define GBBP_LMEAS_INT2_INT_BIT         ( BIT_X(26) )                           /* LTE测量时相对于CTU的INT2中断(Occasion结束中断) */







/*************************************** CPU配置 *********************************************/
#define HAL_GTC_RAM_ADDR                        ( 0x0000 )                      /* GTC指令RAM */

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_NOP_ADDR                        ( 31 )                          /* GTC NOP指令地址 */

/************************************* GTC指令定义 ******************************************/
#define HAL_GTC_START_INSTRUCTION_CODE          ( 0 )                           /* START指令码 */
#define HAL_GTC_FRM_OFFSET_INSTRUCTION_CODE     ( 1 )                           /* FRM_OFFSET指令码 */
#define HAL_GTC_FRM_IT_INSTRUCTION_CODE         ( 2 )                           /* FRM_IT指令码 */
#define HAL_GTC_HOLD_INSTRUCTION_CODE           ( 3 )                           /* HOLD指令码 */
#define HAL_GTC_STOP_INSTRUCTION_CODE           ( 4 )                           /* STOP指令码 */
#define HAL_GTC_CONFIG1_INSTRUCTION_CODE        ( 5 )                           /* CONFIG1指令码 */
#define HAL_GTC_CONFIG2_INSTRUCTION_CODE        ( 6 )                           /* CONFIG2指令码 */
#define HAL_GTC_CONFIG3_INSTRUCTION_CODE        ( 7 )                           /* CONFIG3指令码 */


/*--------------------------------------------------------------------------------------------*
 * 1.2  GBBP中断控制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_CPU_GRM_INT_ADDR                ( 28 )                          /* 表示8个CPU可编程中断 */
#define HAL_GTC_SLOTINT_OFFSET_L_ADDR           ( 30 )                          /* 8个时隙头中断偏移位置（一个时隙内）低8bit */
#define HAL_GTC_SLOTINT_OFFSET_H_ADDR           ( 31 )                          /* 8个时隙头中断偏移位置（一个时隙内）高8bit */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_AGC_DEC_INT_T3_FN_ADDR         ( 0x5214 )                      /* GBBP AGC测量,NB业务译码中断所在TDMA帧号模51计数值 */
#define HAL_GBBP_DEM_INT_FN_QB_ADDR             ( 0x5218 )                      /* GBBP 解调中断所在TDMA帧号模51计数值,QB计数值 */
#define HAL_GBBP_IMI_INT_QB_RPT_ADDR            ( 0x7008 )                      /* IMI时钟中断上报时GTC的TDMA QB计数，测试用 */
#define HAL_GBBP_DRX_SLEEP_INT_QB_RPT_ADDR      ( 0x700c )                      /* 32K时钟中断上报时GTC的TDMA QB计数，测试用 */


#define HAL_GBBP_ARM_INT_STA_ADDR               ( 0x1200 )                      /* GBBP 中断状态寄存器 ARM*/
#define HAL_GBBP_ARM_INT_MASK_ADDR              ( 0x1204 )                      /* GBBP 中断屏蔽位 ARM*/
#define HAL_GBBP_ARM_INT_CLR_ADDR               ( 0x1208 )                      /* GBBP 中断清零寄存器 ARM*/
#define HAL_GBBP_DSP_INT_STA_ADDR               ( 0x1210 )                      /* GBBP 中断状态寄存器 */
#define HAL_GBBP_DSP_INT_MASK_ADDR              ( 0x1214 )                      /* GBBP 中断屏蔽位 */
#define HAL_GBBP_DSP_INT_CLR_ADDR               ( 0x1218 )                      /* GBBP 中断清零寄存器 */
#define HAL_GBBP_IMI_INT_QB_RPT_POWERDOWN_ADDR  ( 0x1220 )                      /* GBBP 掉电区 DSP IMI时钟中断上报时GTC的TDMA QB计数 */


/*--------------------------------------------------------------------------------------------*
 * 1.3  射频控制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_SPI_RD_START_ADDR                   ( 0 )                           /* GTC SPI发送数据在SPI DATA RAM中的起始位置 */
#define HAL_GTC_SPI_TRANS_DATA_NUM_ADDR             ( 1 )                           /* 本次需要发送的SPI串口数据数 */
#define HAL_GTC_SPI_RD_EN_ADDR                      ( 2 )                           /* GTC启动发送SPI数据脉冲 */
#define HAL_GTC_GSP_LINE1_CTRL_BITWISE_CLR_ADDR     ( 25 )                          /* 第一组线控清零控制 */
#define HAL_GTC_GSP_LINE1_CTRL_BITWISE_SET_ADDR     ( 26 )                          /* 第一组线控置位控制 */
#define HAL_GTC_GSP_LINE2_CTRL_BITWISE_CLR_ADDR     ( 14 )                          /* 第二组线控清零控制 */
#define HAL_GTC_GSP_LINE2_CTRL_BITWISE_SET_ADDR     ( 15 )                          /* 第二组线控置位控制 */
#define HAL_GTC_GSP_LINE3_CTRL_BITWISE_CLR_ADDR     ( 18 )                          /* 第三组线控清零控制 */
#define HAL_GTC_GSP_LINE3_CTRL_BITWISE_SET_ADDR     ( 19 )                          /* 第三组线控置位控制 */
#define HAL_GTC_GSP_LINE4_CTRL_BITWISE_CLR_ADDR     ( 20 )                          /* 第四组线控清零控制 */
#define HAL_GTC_GSP_LINE4_CTRL_BITWISE_SET_ADDR     ( 21 )                          /* 第四组线控置位控制 */
#define HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_CLR_ADDR  ( 22 )                          /* 扩展线控清零控制 */
#define HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_SET_ADDR  ( 23 )                          /* 扩展线控置位控制 */
#define HAL_GTC_GSP_LINE01_SEL_CFG_ADDR             ( 27 )                          /* 跨通道测量配置选择信号 */
#define HAL_GTC_GSP_MIPI_START_INI_ADDR             ( 16 )
#define HAL_GTC_GSP_MIPI_MUM_ADDR                   ( 17 )

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_SPI_RF_CFG_ADDR                ( 0x1368 )                      /* SPI寄存器配置 */
#define HAL_GBBP_CPU_SPI_RF_RD_ADDR                 ( 0x136C )                      /* CPU启动SPI数据发送脉冲 */
#define HAL_GBBP_CPU_LINE_CTRL_ADDR                 ( 0x1364 )                      /* RF线控信号CPU配置地址 */
#define HAL_GBBP_CPU_EXT_LINE_CTRL_ADDR             ( 0x5E04 )
#define HAL_GBBP_GTC_GSP_EXT_LINE_STATE_RPT_ADDR    ( 0x5E08 )
#define HAL_GBBP_CPU_LINE01_SEL_CFG_ADDR            ( 0x5e0c )
#define HAL_GBBP_CPU_LINE01_SEL_STATE_RPT_ADDR      ( 0x5e10 )

/*--------------------------------------------------------------------------------------------*
 * 1.4  GRIF模块
 *--------------------------------------------------------------------------------------------*/
/*************************************** GTC配置 *********************************************/
#define HAL_GTC_GSP_RX_LOW_LEN_ADDR                 ( 0 )
#define HAL_GTC_GSP_RX_HIGH_LEN_ADDR                ( 1 )
#define HAL_GTC_GSP_GRIF_CTRL_ADDR                  ( 2 )

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_1REG_ADDR        ( 0xF260 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_2REG_ADDR        ( 0xF264 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_3REG_ADDR        ( 0xF268 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_4REG_ADDR        ( 0xF26C )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_5REG_ADDR        ( 0xF270 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_6REG_ADDR        ( 0xF274 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_7REG_ADDR        ( 0xF278 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_8REG_ADDR        ( 0xF27C )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_9REG_ADDR        ( 0xF280 )
#define HAL_GBBP_ADDR_RF_UNIFY_SPI_10REG_ADDR       ( 0xF284 )
#define HAL_GBBP_ABB_CONFIG_SEL_ADDR                ( 0xF288 )
#define HAL_GBBP_ADDR_G_TX_DC_OFFSET_ADDR           ( 0xF290 )
#define HAL_GBBP_ADDR_G_CH_PARA_1REG_ADDR           ( 0xF294 )
#define HAL_GBBP_ADDR_G_CH_PARA_2REG_ADDR           ( 0xF298 )
#define HAL_GBBP_ADDR_G_CH_PARA_3REG_ADDR           ( 0xF29C )
#define HAL_GBBP_ADDR_G_CH_PARA_4REG_ADDR           ( 0xF2A0 )
#define HAL_GBBP_ADDR_G_CH_PARA_5REG_ADDR           ( 0xF2A4 )
#define HAL_GBBP_ADDR_G_CH_PARA_6REG_ADDR           ( 0xF2A8 )
#define HAL_GBBP_ADDR_G_CH_PARA_7REG_ADDR           ( 0xF2AC )
#define HAL_GBBP_ADDR_G_CH_PARA_8REG_ADDR           ( 0xF2B0 )
#define HAL_GBBP_ADDR_G_CH_PARA_9REG_ADDR           ( 0xF2B8 )
#define HAL_GBBP_ADDR_G_CH_PARA_10REG_ADDR          ( 0xF2C4 )
#define HAL_GBBP_ADDR_G_CH_PARA_11REG_ADDR          ( 0xF2C8 )
#define HAL_GBBP_ADDR_G_CH_PARA_12REG_ADDR          ( 0xF2CC )
#define HAL_GBBP_ADDR_G_CH_PARA_13REG_ADDR          ( 0xF2D0 )
#define HAL_GBBP_ADDR_G_CH_PARA_14REG_ADDR          ( 0xF2D4 )
#define HAL_GBBP_ADDR_G_CH_PARA_15REG_ADDR          ( 0xF2D8 )
#define HAL_GBBP_ADDR_G_CH_PARA_16REG_ADDR          ( 0xF2DC )
#define HAL_GBBP_ADDR_G_CH_PARA_17REG_ADDR          ( 0xF2E0 )
#define HAL_GBBP_ADDR_G_CH_DC_OFFSET_ADDR           ( 0xF2B4 )
#define HAL_GBBP_ADDR_RF_UNIFY_TEST_PIN_SEL_ADDR    ( 0xF2BC )
#define HAL_GBBP_ADDR_RF_UNIFY_TEST_PIN_TRIG_ADDR   ( 0xF2C0 )
#define HAL_GBBP_RX_RSHIFT_NUM_SEL_ADDR             ( 0xF2E4 )
#define HAL_GBBP_CLIP_MODE_SEL_ADDR                 ( 0xF2E8 )
#define HAL_GBBP_CPU_GRIF_FIFO_RD_ADDR              ( 0xf2ec )
#define HAL_GBBP_CPU_GRIF_CONFIG_ADDR               ( 0xf2f0 )
#define HAL_GBBP_ADDR_G_32CH_PARA_1REG_ADDR         ( 0xF31C )
#define HAL_GBBP_ADDR_G_32CH_PARA_2REG_ADDR         ( 0xF320 )
#define HAL_GBBP_ADDR_G_32CH_PARA_3REG_ADDR         ( 0xF324 )
#define HAL_GBBP_ADDR_G_32CH_PARA_4REG_ADDR         ( 0xF328 )
#define HAL_GBBP_ADDR_G_32CH_PARA_5REG_ADDR         ( 0xF32C )
#define HAL_GBBP_ADDR_G_32CH_PARA_6REG_ADDR         ( 0xF330 )
#define HAL_GBBP_ADDR_G_32CH_PARA_7REG_ADDR         ( 0xF334 )
#define HAL_GBBP_ADDR_G_32CH_PARA_8REG_ADDR         ( 0xF338 )
#define HAL_GBBP_ADDR_G_32CH_PARA_9REG_ADDR         ( 0xF33C )
/*--------------------------------------------------------------------------------------------*
 *1.5   自动增益控制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_GSP_DAGC_CTRL_ADDR                  ( 3 )                       /* GSP DAGC线控配置 */
#define HAL_GTC_GSP_AGC_CTRL1_ADDR                  ( 20 )                      /* AGC模块工作配置 1 */
#define HAL_GTC_GSP_AGC_CTRL2_ADDR                  ( 21 )                      /* AGC模块工作配置 2 */
#define HAL_GTC_GSP_AGC_START_ADDR                  ( 22 )                      /* AGC启动配置 */
#define HAL_GTC_GSP_RSSI_FRQ_LOW_ADDR               ( 3 )                       /* 当前测量的RSSI对应的频点号低8bit */
#define HAL_GTC_GSP_RSSI_FRQ_HIGH_ADDR              ( 4 )                       /* 当前测量的RSSI对应的频点号高4bit */
#define HAL_GTC_GSP_INIT_GAIN_RFIC_ADDR             ( 7 )                       /* RF每时隙处理的初始档位 */
#define HAL_GTC_GSP_AGC_TRUNC_WIDTH_ADDR            ( 13 )                      /* AGC测量数字细调环截位位宽 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_R2_RSSI1234_THRESHOLD_ADDR         ( 0x5204 )                  /* 2轮测量门限 */
#define HAL_GBBP_R2_RSSI4567_THRESHOLD_ADDR         ( 0x5208 )                  /* 2轮测量门限 */
#define HAL_GBBP_R3_RSSI1234_THRESHOLD_ADDR         ( 0x5210 )
#define HAL_GBBP_R3_RSSI4567_THRESHOLD_ADDR         ( 0x521c )
#define HAL_GBBP_AGC_WAIT_LENGTH_ADDR               ( 0x520c )                  /* 测量时间和测量模式配置 */
#define HAL_GBBP_DC_IQ_REMOVE_RPT_ADDR              ( 0x5220 )                  /* 多路数字细调环偏置输出 */
#define HAL_GBBP_DC_IQ_BURST_REMOVE_RPT_ADDR        ( 0x5224 )                  /* 时隙前处理I路, Q路直流偏置上报 */
#define HAL_GBBP_AAGC_NEXT_GAIN_ADDR                ( 0x5250 )                  /* AGC期望控制增益 */
#define HAL_GBBP_AAGC_GAIN1_CPU_ADDR                ( 0x1460 )                  /* 最近一个时隙AGC期望控制增益 */
#define HAL_GBBP_AAGC_GAIN2_CPU_ADDR                ( 0x1464 )                  /* 次近一个时隙AGC期望控制增益 */
#define HAL_GBBP_AAGC_GAIN3_CPU_ADDR                ( 0x1468 )                  /* 次次近一个时隙AGC期望控制增益 */
#define HAL_GBBP_AAGC_GAIN4_CPU_ADDR                ( 0x146c )                  /* 最远一个时隙AGC期望控制增益 */
#define HAL_GBBP_RSSI_QB_FRQ_ADDR                   ( 0x5254 )                  /* 测量中断时刻TDMA帧QB值 */
#define HAL_GBBP_DRSSI_ENERGY_RPT_ADDR              ( 0x5258 )                  /* DRSSI求对数运算前数值上报 */
#define HAL_GBBP_AGC_FAST1_2_RSSI_CPU_ADDR          ( 0x525c )                  /* 快速测量第一轮以及第二轮RSSI上报。 */
#define HAL_GBBP_AAGC_GAIN_STATE_REG_ADDR           ( 0x5260 )                  /* 自研RF最小以及最小增益控制档位 */
#define HAL_GBBP_AAGC_GAIN_THRD_REG_ADDR            ( 0x5264 )                  /* AGC目标能量与当前能量之差的控制阈值 */
#define HAL_GBBP_CPU_OFC_RXCM_REG_ADDR              ( 0x5360 )                  /* OFC相关配置,agc 通道增益乒乓切换使能 */
#define HAL_GBBP_CPU_RX_ADDR                        ( 0x5368 )                  /* GSM增益控制寄存器地址 */
#define HAL_GBBP_CPU_SAMPLE_REG_ADDR                ( 0x536C )                  /* 采样点选择与直流偏置旁路 */
#define HAL_GBBP_CPU_AGC_RAM_SWITCH_IND             ( 0x5370 )                  /* AGC直流偏置RAM和通道增益RAM手动切换指示信号，该信号为脉冲信号，自清零 */
#define HAL_GBBP_DAGC_MULFACTOR_FIX                 ( 0x537c )                  /* DRSSI上报，其对应为ABB输出信号的能量 */
#define HAL_GBBP_AGC_IP2_I_RPT_ADDR                 ( 0x5278 )                  /* I路4个样点数据平方累加上报 */
#define HAL_GBBP_AGC_IP2_Q_RPT_ADDR                 ( 0x527C )                  /* Q路4个样点数据平方累加上报 */
#define HAL_GBBP_CPU_SPI_WORD_SEL_ADDR              ( 0x5580 )                  /* 下行接受增益SPI控制字格式选择 */
#define HAL_GBBP_CPU_GAIN1_SPI_DATA_ADDR            ( 0x5584 )                  /* 控制增益模式下第1档SPI控制字 */
#define HAL_GBBP_CPU_GAIN2_SPI_DATA_ADDR            ( 0x5588 )                  /* 控制增益模式下第2档SPI控制字 */
#define HAL_GBBP_CPU_GAIN3_SPI_DATA_ADDR            ( 0x558c )                  /* 控制增益模式下第3档SPI控制字 */
#define HAL_GBBP_CPU_GAIN4_SPI_DATA_ADDR            ( 0x5590 )                  /* 控制增益模式下第4档SPI控制字 */
#define HAL_GBBP_CPU_GAIN5_SPI_DATA_ADDR            ( 0x5594 )                  /* 控制增益模式下第5档SPI控制字 */
#define HAL_GBBP_CPU_GAIN6_SPI_DATA_ADDR            ( 0x5598 )                  /* 控制增益模式下第6档SPI控制字 */
#define HAL_GBBP_CPU_GAIN7_SPI_DATA_ADDR            ( 0x559c )                  /* 控制增益模式下第7档SPI控制字 */
#define HAL_GBBP_CPU_GAIN8_SPI_DATA_ADDR            ( 0x55a0 )                  /* 控制增益模式下第8档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN1_SPI_DATA_ADDR        ( 0x55a4 )                  /* 控制增益模式下第1档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN2_SPI_DATA_ADDR        ( 0x55a8 )                  /* 控制增益模式下第2档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN3_SPI_DATA_ADDR        ( 0x55ac )                  /* 控制增益模式下第3档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN4_SPI_DATA_ADDR        ( 0x55b0 )                  /* 控制增益模式下第4档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN5_SPI_DATA_ADDR        ( 0x55b4 )                  /* 控制增益模式下第5档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN6_SPI_DATA_ADDR        ( 0x55b8 )                  /* 控制增益模式下第6档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN7_SPI_DATA_ADDR        ( 0x55bc )                  /* 控制增益模式下第7档SPI控制字 */
#define HAL_GBBP_CPU_DIV_GAIN8_SPI_DATA_ADDR        ( 0x55c0 )                  /* 控制增益模式下第8档SPI控制字 */
#define HAL_GBBP_DC_OFFSET_ADDR                     ( 0x6000 )                  /* DC Offset */
#define HAL_GBBP_CHANNEL_GAIN_ADDR                  ( 0x6400 )                  /* Channel Gain */
#define HAL_GBBP_SLOT_DCR_RSSI_THRESHOLD_ADDR       ( 0x5378 )
#define HAL_GBBP_GACI_THERSHOLD_ADDR                ( 0x5884 )                  /* 邻频检测门限参数 */
/*--------------------------------------------------------------------------------------------*
 * 1.7  复位模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_RESET_ADDR                     ( 0x1350 )                  /* BBP复位控制信号 */

/*--------------------------------------------------------------------------------------------*
 * 1.8  子模块时钟控制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_SOFT_CLK_CTRL_ADDR             ( 0x1354 )                  /* 各个子模块的时钟关闭和打通控制 */
#define HAL_GBBP_CPU_AUTO_CLK_BYPASS_ADDR           ( 0x135c )                  /* GBBP自动门控 */

/*--------------------------------------------------------------------------------------------*
 * 1.9  GTC模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_GTC_CFG_ADDR                   ( 0x1360 )                  /* CPU控制GTC模块 */
#define HAL_GBBP_GTC_STATE_RPT_ADDR                 ( 0x1374 )                  /* GTC状态 */
#define HAL_GBBP_GTC_FRM_OFFSET_RPT_ADDR            ( 0x137c )                  /* TDMA定时与TimeBase时间偏差 */
#define HAL_GBBP_GTC_FRM_IT_OFFSET_RPT_ADDR         ( 0x1380 )                  /* 帧中断与TimeBase时间偏差 */
#define HAL_GBBP_GTC_GSP_LINE_STATE_RPT_ADDR        ( 0x1384 )                  /* GSP实际线控状态 */
#define HAL_GBBP_CPU_GTC_T1_T2_T3_CNG_ADDR          ( 0x1388 )                  /* 设置GTC的T1T2T3值 */
#define HAL_GBBP_GTC_T1_T2_T3_RPT_CPU_ADDR          ( 0x138c )                  /* GTC内部维护的计数器T1T2T3 */
#define HAL_GBBP_CPU_GTC_FRM_OFFSET_CFG_ADDR        ( 0x1390 )                  /* cpu配置帧头调整 */
#define HAL_GBBP_CPU_GTC_FN_CNG_ADDR                ( 0x1394 )                  /* 帧号配置到bbp中 */
#define HAL_GBBP_GTC_FN_RPT_ADDR                    ( 0x1398 )                  /* 当前帧号 */
#define HAL_GBBP_GTC_FN_HIGH_RPT_ADDR               ( 0x139A )                  /* 当前帧号 */
#define HAL_GBBP_GTC_QB_RPT_ADDR                    ( 0x139c )                  /* 当前时刻TDMA的qb计数值 */
#define HAL_GBBP_TIMEBASE_RPT_ADDR                  ( 0x13A8 )                  /* 绝对时标上报 */
#define HAL_GBBP_CPU_TIMING_GET_TRIGGER_ADDR        ( 0x5a20 )                  /* FN和Qb获取锁定 */
#define HAL_GBBP_TIMING_GET_FN_RPT_ADDR             ( 0x5a24 )                  /* 锁定的FN */
#define HAL_GBBP_TIMING_GET_TIMEBASE_RPT_ADDR       ( 0x5a28 )                  /* 锁定的Qb */


/*--------------------------------------------------------------------------------------------*
 * 1.10 同步搜索模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_SRCH_CFG_ADDR                       ( 3 )                       /* SYN同步控制，包括同步使能、搜索模式 */
#define HAL_GTC_SRCH_START_ADDR                     ( 4 )                       /* SYN同步启动 */
#define HAL_GTC_NCO_INIT_VALUE_HI_ADDR              ( 14 )                      /* 邻区频偏初值的高8bit */
#define HAL_GTC_NCO_INIT_VALUE_LOW_ADDR             ( 13 )                      /* 邻区频偏初值的低8bit */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_SOFT_AFC_VALUE_ADDR            ( 0x1000 )                  /* 软件配置AFC软值 */
#define HAL_GBBP_CPU_SOFT_AFC_RENEW_ADDR            ( 0x1004 )                  /* 物理层软件启动更新AFC值，脉冲信号 */
#define HAL_GBBP_CPU_CI_THRES_CFG_ADDR              ( 0x1008 )                  /* gmsk/8psk门限设置 */
#define HAL_GBBP_CPU_NB_AFC_NUM_ADDR                ( 0x100c )                  /* 用于计算NB-AFC的NB个数选择信号 */
#define HAL_GBBP_CPU_AFC_LOOP1_CFG_ADDR             ( 0x1080 )                  /* 上电后NB-AFC环路滤波累加器一的配置值 */
#define HAL_GBBP_CPU_AFC_LOOP2_CFG_ADDR             ( 0x5E20 )
#define HAL_GBBP_NB_AFC_LOOP_SWITCH_ADDR            ( 0x5340 )
#define HAL_GBBP_ADDR_NB_AFC_LOOP_SLOW_ADDR         ( 0x5E14 )
#define HAL_GBBP_ADDR_NB_AFC_LOOP_FAST_ADDR         ( 0x5E18 )
#define HAL_GBBP_AFC_LOOP2_RPT_ADDR                 ( 0x5E1C )
#define HAL_GBBP_CPU_AFC_CFG_ADDR                   ( 0x1010 )                  /* AFC工作模式选择，AFC值清零/写入 */
#define HAL_GBBP_AFC_VAULE_I_CPU_ADDR               ( 0x1014 )                  /* 上报的afc实部相关累加值 */
#define HAL_GBBP_AFC_VAULE_Q_CPU_ADDR               ( 0x1018 )                  /* 上报的afc虚部相关累加值 */
#define HAL_GBBP_CPU_AFC_PWM_WIDTH_SEL_ADDR         ( 0x101c )                  /* PDM转换模块选择 */
#define HAL_GBBP_CPU_NB_AFC_NUM_CLR_ADDR            ( 0x1020 )                  /* 表示重新开始统计NB-AFC的NB-BURST个数 */
#define HAL_GBBP_CPU_FB_KP_KINT_ADDR                ( 0x1024 )                  /* FB-AFC收敛参数 */
#define HAL_GBBP_FB_TIMEBASE_RPT_ADDR               ( 0x1028 )                  /* 滞后FB 1215QB处timebase的QB，FN计数 */
#define HAL_GBBP_FB_FN_RPT_ADDR                     ( 0x102c )                  /* 同步搜索时间门限值（以帧为单位） */
#define HAL_GBBP_CPU_FB_TIMES_THRES_ADDR            ( 0x1030 )                  /* FB搜索门限配置 */
#define HAL_GBBP_CPU_FB_ENERG_THRES_ADDR            ( 0x1034 )                  /* FB搜索得到能量门限 */
#define HAL_GBBP_AFC_RPT_ADDR                       ( 0x1038 )                  /* AFC值上报 */
#define HAL_GBBP_AFC_LOOP1_RPT_ADDR                 ( 0x1078 )                  /* 当前NB-AFC环路滤波累加器一的上报值 */
#define HAL_GBBP_AFC_PHASE_THRESHOLD_CFG_ADDR       ( 0x108c )                  /* NB-AFC算法相位估计值参考上下限 */
#define HAL_GBBP_AFC_FCUT_THRESHOLD_CFG_ADDR        ( 0x1090 )                  /* NB-AFC算法系数fcut的最小值以及最大值 */
#define HAL_GBBP_AFC_ALPHA_SEL_ADDR                 ( 0x1094 )                  /* NB-AFC算法α滤波系数选择 */
#define HAL_GBBP_FB_AFC_PHASE_THRESHOLD_CFG_ADDR    ( 0x1230 )                  /* FB-AFC算法相位估计值参考上下限 */
#define HAL_GBBP_FB_AFC_FCUT_THRESHOLD_CFG_ADDR     ( 0x1234 )                  /* FB-AFC算法系数fcut的最小,最大值 */
#define HAL_GBBP_FB_AFC_ALPHA_SEL_ADDR              ( 0x1238 )                  /* NB-AFC算法α滤波系数选择 */
#define HAL_GBBP_FB_RPT1_ADDR                       ( 0x103c )                  /* FB搜索得到的FB时隙头位置，相对于gtc_timebase位置 */
#define HAL_GBBP_FB_RPT2_ADDR                       ( 0x1040 )                  /* 本轮FB搜索的最大能量值 */
#define HAL_GBBP_SB_POS_RPT1_ADDR                   ( 0x1044 )                  /* SB搜索成功得到的帧头与TIMEBASE的差 */
#define HAL_GBBP_SB_SQUARE_RPT1_ADDR                ( 0x1048 )                  /* SB搜索能量值的最大值 */
#define HAL_GBBP_SB_POS_RPT2_ADDR                   ( 0x104c )                  /* SB搜索次大值相对于timebase对应的位置 */
#define HAL_GBBP_SB_SQUARE_RPT2_ADDR                ( 0x1050 )                  /* SB搜索次大值 */
#define HAL_GBBP_SB_POS_RPT3_ADDR                   ( 0x1054 )                  /* SB搜索能量的第三大值相对于timebase的位置 */
#define HAL_GBBP_SB_SQUARE_RPT3_ADDR                ( 0x1058 )                  /* SB搜索能量的第三大值 */
#define HAL_GBBP_STATE_RPT_ADDR                     ( 0x105C )                  /* 搜索中断状态 */
#define HAL_GBBP_SRCH_QB_RPT_ADDR                   ( 0x1064 )                  /* 搜索中断所在gtc_timebase上报 */
#define HAL_GBBP_SRCH_FN_RPT_ADDR                   ( 0x106c )                  /* 搜索中断所在位置 */
#define HAL_GBBP_SB_ADVANCE_LEN_CFG_ADDR            ( 0x1070 )                  /* 001搜索模式下，SB提前接收的符号个数 */
#define HAL_GBBP_SRCH_OPT_CFG_ADDR                  ( 0x1480 )                  /* 搜索优化模式配置 */
#define HAL_GBBP_CPU_SRCH_EN_CLR_ADDR               ( 0x1124 )                  /* 清搜索使能，清解调使能 */
#define HAL_GBBP_NCELL_NCO_RPT                      ( 0x1084 )
#define HAL_GBBP_AFC_K_VALUE_ADDR                   ( 0x5e00 )                  /* AFC K值调整，默认值为10'd128，TCXO方案下保持默认值，DCXO方案下根据实际系统进行调整。 */

/*--------------------------------------------------------------------------------------------*
 * 1.11 下行解调模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_DEMOD_PARA1_ADDR                    ( 16 )                      /* 解调参数1 */
#define HAL_GTC_DEMOD_PARA2_ADDR                    ( 17 )                      /* 解调参数2 */
#define HAL_GTC_DEMOD_PARA3_ADDR                    ( 18 )                      /* 解调参数3 */
#define HAL_GTC_TOA_PARA_ADDR                       ( 29 )                      /* TOA参数 */
#define HAL_GTC_DEMOD_START_ADDR                    ( 19 )                      /* 下行NB解调启动信号 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_NB_POS_RPT_ADDR                    ( 0x1060 )                  /* NB位置，类型上报 */
#define HAL_GBBP_CPU_8PSK_PSP_ALPHA_PARA_ADDR       ( 0x1068 )                  /* 8psk均衡解调中求PSP_EN的参数alpha */
#define HAL_GBBP_CPU_TOA_ALPHA_PARA_ADDR            ( 0x1074 )                  /* alpha滤波系数选择 */
#define HAL_GBBP_CPU_CH_ECT_OPT_ADDR                ( 0x1088 )                  /* 信道估计优化 */
#define HAL_GBBP_NB_MOD_TYPE_RPT_ADDR               ( 0x1098 )                  /* 四个BURST调制盲检测标志上报 */
#define HAL_GBBP_GROSS_CARRIER_ENERGY1_ADDR         ( 0x109C )                  /* 第一近的一个BURST解调前粗测C值上报 */
#define HAL_GBBP_GROSS_INTERFERE_ENERGY1_ADDR       ( 0x10A0 )                  /* 第一近的一个BURST解调前粗测I值上报 */
#define HAL_GBBP_GROSS_CARRIER_ENERGY2_ADDR         ( 0x10A4 )                  /* 第二近的一个BURST解调前粗测C值上报 */
#define HAL_GBBP_GROSS_INTERFERE_ENERGY2_ADDR       ( 0x10A8 )                  /* 第二近的一个BURST解调前粗测I值上报 */
#define HAL_GBBP_GROSS_CARRIER_ENERGY3_ADDR         ( 0x10AC )                  /* 第三近的一个BURST解调前粗测C值上报 */
#define HAL_GBBP_GROSS_INTERFERE_ENERGY3_ADDR       ( 0x10B0 )                  /* 第三近的一个BURST解调前粗测I值上报 */
#define HAL_GBBP_GROSS_CARRIER_ENERGY4_ADDR         ( 0x10B4 )                  /* 第四近的一个BURST解调前粗测C值上报 */
#define HAL_GBBP_GROSS_INTERFERE_ENERGY4_ADDR       ( 0x10B8 )                  /* 第四近BURST解调前粗测I值上报 */
#define HAL_GBBP_MQ1_FENZI1_ADDR                    ( 0x10C0 )                  /* 第一近BURST解调前半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ1_FENMU1_ADDR                    ( 0x10C4 )                  /* 第一近BURST解调前半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ2_FENZI1_ADDR                    ( 0x10C8 )                  /* 第一近BURST解调后半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ2_FENMU1_ADDR                    ( 0x10CC )                  /* 第一近BURST解调后半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ1_FENZI2_ADDR                    ( 0x10D0 )                  /* 第二近BURST解调前半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ1_FENMU2_ADDR                    ( 0x10D4 )                  /* 第二近BURST解调前半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ2_FENZI2_ADDR                    ( 0x10D8 )                  /* 第二近BURST解调后半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ2_FENMU2_ADDR                    ( 0x10DC )                  /* 第二近BURST解调后半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ1_FENZI3_ADDR                    ( 0x10E0 )                  /* 第三近BURST解调前半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ1_FENMU3_ADDR                    ( 0x10E4 )                  /* 第三近BURST解调前半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ2_FENZI3_ADDR                    ( 0x10E8 )                  /* 第三近BURST解调后半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ2_FENMU3_ADDR                    ( 0x10EC )                  /* 第三近BURST解调后半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ1_FENZI4_ADDR                    ( 0x10F0 )                  /* 第四近BURST解调前半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ1_FENMU4_ADDR                    ( 0x10F4 )                  /* 第四近BURST解调前半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ2_FENZI4_ADDR                    ( 0x10F8 )                  /* 第四近BURST解调后半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ2_FENMU4_ADDR                    ( 0x10FC )                  /* 第四近BURST解调后半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ1_FENZI5_ADDR                    ( 0x530c )                  /* 第五近BURST解调前半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ1_FENMU5_ADDR                    ( 0x5310 )                  /* 第五近BURST解调前半个BURST的MQ平方的分母值 */
#define HAL_GBBP_MQ2_FENZI5_ADDR                    ( 0x5314 )                  /* 第五近BURST解调后半个BURST的MQ平方的分子值 */
#define HAL_GBBP_MQ2_FENMU5_ADDR                    ( 0x5318 )                  /* 第五近BURST解调后半个BURST的MQ平方的分母值 */
#define HAL_GBBP_VALPP_CARRIER_ENERGY0_ADDR         ( 0x1100 )                  /* 最近burst对应的载波能量值 */
#define HAL_GBBP_VALPP_INTERFERE_ENERGY0_ADDR       ( 0x1150 )                  /* 最近burst对应的干扰能量值 */
#define HAL_GBBP_VALPP_DEM_TYPE_TOA0                ( 0x1104 )                  /* 最近burst VALPP解调TOA，type信息 */
#define HAL_GBBP_VALPP_CARRIER_ENERGY1_ADDR         ( 0x1108 )                  /* 第二近burst对应的载波能量值 */
#define HAL_GBBP_VALPP_INTERFERE_ENERGY1_ADDR       ( 0x1154 )                  /* 第二近burst对应的干扰能量值 */
#define HAL_GBBP_VALPP_DEM_TYPE_TOA1_ADDR           ( 0x110c )                  /* 第二近burst VALPP解调TOA，type信息 */
#define HAL_GBBP_VALPP_CARRIER_ENERGY2_ADDR         ( 0x1110 )                  /* 第三近burst对应的载波能量值 */
#define HAL_GBBP_VALPP_INTERFERE_ENERGY2_ADDR       ( 0x1158 )                  /* 第三近burst对应的干扰能量值 */
#define HAL_GBBP_VALPP_DEM_TYPE_TOA2_ADDR           ( 0x1114 )                  /* 第三近burst VALPP解调TOA，type信息 */
#define HAL_GBBP_VALPP_CARRIER_ENERGY3_ADDR         ( 0x1118 )                  /* 第四近burst对应的载波能量值 */
#define HAL_GBBP_VALPP_INTERFERE_ENERGY3_ADDR       ( 0x115c )                  /* 第四近burst对应的干扰能量值 */
#define HAL_GBBP_VALPP_DEM_TYPE_TOA3_ADDR           ( 0x111c )                  /* 第四近burst VALPP解调TOA，type信息 */
#define HAL_GBBP_VALPP_CARRIER_ENERGY4_ADDR         ( 0x532C )                  /* 第五近burst对应的载波能量值 */
#define HAL_GBBP_VALPP_INTERFERE_ENERGY4_ADDR       ( 0x5330 )                  /* 第五近burst对应的干扰能量值 */
#define HAL_GBBP_VALPP_DEM_TYPE_TOA4_ADDR           ( 0x5298 )                  /* 第五近burst VALPP解调TOA，type信息 */
#define HAL_GBBP_CPU_DEM_WIDTH_CFG_ADDR             ( 0x1120 )                  /* 解调位宽配置 */
#define HAL_GBBP_CPU_DB_TYPE_CFG_ADDR               ( 0x1128 )                  /* DB解调门限 */
#define HAL_GBBP_CPU_DEM_BEP_MODE_EN_ADDR           ( 0x112c )                  /* 解调BEP功能 */
#define HAL_GBBP_CPU_VALPP_CARRY_CTRL_ADDR          ( 0x1130 )                  /* 数据搬移配置 */
#define HAL_GBBP_CPU_VALPP_CARRY_START_ADDR         ( 0x1134 )                  /* 数据搬移使能 */
#define HAL_GBBP_CPU_SAIC_CTRL_ADDR                 ( 0x1138 )                  /* SAIC解调控制 */
#define HAL_GBBP_MAX_DB_DATA_ENERGY_RPT_ADDR        ( 0x113c )                  /* DummuBurst检测 DATA最大能量上报 */
#define HAL_GBBP_MAX_DB_ENERGY_RPT_ADDR             ( 0x1140 )                  /* DummuBurst检测 DB最大能量上报 */
#define HAL_GBBP_ENG_WEIGHT_ADDR                    ( 0x1144 )                  /* 解调径加权参数配置 */
#define HAL_GBBP_LS_DC_CPU_ADDR                     ( 0x1160 )                  /* 当前burst直流I/Q路上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT1_ADDR          ( 0x5730 )                  /* 8PSK径1能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT2_ADDR          ( 0x5734 )                  /* 8PSK径2能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT3_ADDR          ( 0x5738 )                  /* 8PSK径3能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT4_ADDR          ( 0x573c )                  /* 8PSK径4能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT5_ADDR          ( 0x5740 )                  /* 8PSK径5能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT6_ADDR          ( 0x5744 )                  /* 8PSK径6能量值上报 */
#define HAL_GBBP_PATH_ENERGY_CPU_RPT7_ADDR          ( 0x5748 )                  /* 8PSK径7能量值上报 */
#define HAL_GBBP_CARRY_RAM_ADDR                     ( 0x5400 )                  /* 0x05400~0x0555b:SB,NB解调软值与CPU接口的搬移ram，4个6bit解调软值，依次占用imi_rddata的5～0，13～8，21～16，29～24bit位 */
#define HAL_GBBP_CPU_JITTER_CORR_CFG_ADDR           ( 0x53b4 )                  /* JITTER消除算法配置 */
#define HAL_GBBP_CPU_TSC_SWITCH_EN_ADDR             ( 0x53c4 )                  /* 训练序列数据上报控制 */
#define HAL_GBBP_TSC_RAM_ADDR                       ( 0x2800 )                  /* 训练序列数据上报RAM */
#define HAL_GBBP_GACI_INDEX_CPU_ADDR                ( 0x58a0 )                  /* 输出邻频检测结果，上报CPU四种可能的干扰状态 */

/*--------------------------------------------------------------------------------------------*
 * 1.12 下行信道译码模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_DL_CB_INDEX_ADDR                    ( 7 )                       /* 下行业务序号 */
#define HAL_GTC_DEC_BURST_POS_ADDR                  ( 8 )                       /* 译码码块在软值存储RAM中的位置 */
#define HAL_GTC_DEC_TYPE1_ADDR                      ( 9 )                       /* 译码参数1 */
#define HAL_GTC_DEC_TYPE2_ADDR                      ( 10 )                      /* 译码参数2 */
#define HAL_GTC_HARQ_POS1_ADDR                      ( 11 )                      /* 在HARQ译码中做HARQ合并的第一码块的起始位置 */
#define HAL_GTC_HARQ_POS2_ADDR                      ( 12 )                      /* 在HARQ译码中做HARQ合并的第二码块的起始位置 */
#define HAL_GTC_HARQ1_CB_INDEX_ADDR                 ( 15 )                      /* 参与HARQ合并译码的码块的CB_INDEX */
#define HAL_GTC_DL_BURST_CYCLE_ADDR                 ( 18 )                      /* 下行译码存放BURST循环周期，质量测量使能信号 */
#define HAL_GTC_GSP_DEC_TEST_MODE_EN_ADDR           ( 0 )                       /* 测试模式使能，精简寻呼模式配置 */
#define HAL_GTC_GSP_DEC_TEST_AMR_FRM_ADDR           ( 1 )                       /* 在译码测试模式下配置的AMR帧类型 */
#define HAL_GTC_GSP_DEC_TEST_PS_FRM_ADDR            ( 2 )                       /* 在译码测试模式下配置的PS帧类型 */
#define HAL_GTC_DEC_START_ADDR                      ( 19 )                      /* 译码启动信号，译码时隙，质量上报清零 */
#define HAL_GTC_DEC_BURST_MAP_INFO_ADDR             ( 8 )                       /* 下行译码8个BURST是否需要解调软值在译码过程清零使用配置 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_BLER_BER_CLR_ADDR              ( 0x1300 )                  /* BLER,BER清零，统计模式 */
#define HAL_GBBP_CPU_AMR_CB_INDEX_ADDR              ( 0x1304 )                  /* AMR语音激活集合内对应的AMR语音帧cb_index */
#define HAL_GBBP_CPU_AMR_ACTIVE_SET_NUM_ADDR        ( 0x1308 )                  /* AMR语音激活集内有效语音帧类型个数 */
#define HAL_GBBP_CPU_AMR_CRC_PRO_NUM_CFG_ADDR       ( 0x53cc )                  /* AMR带内bit优化算法中对SPEECH CRC的保护次数 */
#define HAL_GBBP_CPU_P0_PRIOR_REG_CFG_ADDR          ( 0x53d0 )                  /* AMR带内bit优化算法中00对应的先验概率 */
#define HAL_GBBP_CPU_P1_PRIOR_REG_CFG_ADDR          ( 0x53d4 )                  /* AMR带内bit优化算法中01对应的先验概率 */
#define HAL_GBBP_CPU_P2_PRIOR_REG_CFG_ADDR          ( 0x53d8 )                  /* AMR带内bit优化算法中10对应的先验概率 */
#define HAL_GBBP_CPU_P3_PRIOR_REG_CFG_ADDR          ( 0x53dc )                  /* AMR带内bit优化算法中11对应的先验概率 */
#define HAL_GBBP_CMI_P01_PRIOR_PRE_RPT_ADDR         ( 0x5380 )                  /* CMI带内bit0，bit1的先验概率上报 */
#define HAL_GBBP_CMI_P23_PRIOR_PRE_RPT_ADDR         ( 0x5384 )                  /* CMI带内bit2，bit3的先验概率上报 */
#define HAL_GBBP_CMC_P01_PRIOR_PRE_ADDR             ( 0x5388 )                  /* CMC带内bit0，bit1的先验概率上报 */
#define HAL_GBBP_CMC_P23_PRIOR_PRE_ADDR             ( 0x538c )                  /* CMC带内bit2，bit3的先验概率上报 */
#define HAL_GBBP_CPU_P01_MAPPRE_CFG_ADDR            ( 0x5390 )                  /* CMC或者CMI 带内bit0，bit1对应的历史先验概率值 */
#define HAL_GBBP_CPU_P23_MAPPRE_CFG_ADDR            ( 0x5394 )                  /* CMC或者CMI 带内bit2，bit3对应的历史先验概率值 */
#define HAL_GBBP_CPU_CMC_CMI_LOAD_EN_ADDR           ( 0x5398 )                  /* CMC或者CMI相关信息  */
#define HAL_GBBP_AMR_INFO1_RPT_ADDR                 ( 0x53f8 )                  /* AMR帧类型上报 */
#define HAL_GBBP_AMR_INFO2_RPT_ADDR                 ( 0x53fc )                  /* AMR判决上报 */
#define HAL_GBBP_CPU_AMR_CORR_COEFF_CFG_ADDR        ( 0x53e0 )                  /* AFS/AHS相关门限参数 */
#define HAL_GBBP_BLER_ERRR_RPT_ADDR                 ( 0x130c )                  /* 累计BLER错误数 */
#define HAL_GBBP_BLER_TOTAL_RPT_ADDR                ( 0x1310 )                  /* 累计BLER总数 */
#define HAL_GBBP_BER_ERRR_RPT_ADDR                  ( 0x1314 )                  /* 累计BER错误数 */
#define HAL_GBBP_BER_TOTAL_RPT_ADDR                 ( 0x1318 )                  /* 累计BER总数 */
#define HAL_GBBP_CODEC_PARA_REG1_ADDR               ( 0x1320 )                  /* 编码参数1 */
#define HAL_GBBP_CODEC_PARA_REG2_ADDR               ( 0x1324 )                  /* 编码参数2 */
#define HAL_GBBP_CODEC_PARA_REG3_ADDR               ( 0x1328 )                  /* 编码参数3 */
#define HAL_GBBP_CODEC_PARA_REG4_ADDR               ( 0x132c )                  /* 编码参数4 */
#define HAL_GBBP_CODEC_PARA_REG5_ADDR               ( 0x1330 )                  /* 编码参数5 */
#define HAL_GBBP_CODEC_PARA_REG6_ADDR               ( 0x1334 )                  /* 编码参数6 */
#define HAL_GBBP_DEC_STATE_RPT1_ADDR                ( 0x1338 )                  /* 译码状态机状态1 */
#define HAL_GBBP_DEC_STATE_RPT2_ADDR                ( 0x133c )                  /* 译码状态机状态2 */
#define HAL_GBBP_Q_BIT_RPT1_ADDR                    ( 0x1340 )                  /* Q bit或者偷帧bit软值1 */
#define HAL_GBBP_Q_BIT_RPT2_ADDR                    ( 0x1344 )                  /* Q bit或者偷帧bit软值2 */
#define HAL_GBBP_DEC_INT_TOTAL_ADDR                 ( 0x1348 )                  /* 译码中断次数上报 */
#define HAL_GBBP_USF_CORR_SOFTDATA1_RPT_ADDR        ( 0x1500 )                  /* USF软值上报1 */
#define HAL_GBBP_USF_CORR_SOFTDATA2_RPT_ADDR        ( 0x1504 )                  /* USF软值上报2 */
#define HAL_GBBP_USF_CORR_SOFTDATA3_RPT_ADDR        ( 0x1508 )                  /* USF软值上报3 */
#define HAL_GBBP_USF_CORR_SOFTDATA4_RPT_ADDR        ( 0x150c )                  /* USF软值上报4 */
#define HAL_GBBP_USF_CORR_SOFTDATA5_RPT_ADDR        ( 0x1510 )                  /* USF软值上报5 */
#define HAL_GBBP_USF_CORR_SOFTDATA6_RPT_ADDR        ( 0x1514 )                  /* USF软值上报6 */
#define HAL_GBBP_USF_CORR_SOFTDATA7_RPT_ADDR        ( 0x1518 )                  /* USF软值上报7 */
#define HAL_GBBP_USF_CORR_SOFTDATA8_RPT_ADDR        ( 0x151c )                  /* USF软值上报8 */
#define HAL_GBBP_USF_CORR_SOFTDATA9_RPT_ADDR        ( 0x1520 )                  /* USF软值上报9 */
#define HAL_GBBP_BFI_RSQ_METRIC_RPT_ADDR            ( 0x134c )                  /* 语音RSQ_METRIC，RSQ_METRIC，BFI上报 */
#define HAL_GBBP_DEC_RPT_RAM_ADDR                   ( 0x2000 )                  /* 译码RAM */
#define HAL_GBBP_NB_DEC_CTRL_RPT_ADDR               ( 0x2004 )                  /* bbp译码控制信息上报起始RAM位置 */
#define HAL_GBBP_SB_DEC_RPT_ADDR                    ( 0x2398 )                  /* SB 译码上报缓存地址 */
#define HAL_GBBP_CODEC_WR_CONFLICT_CNT_ADDR         ( 0x5718 )                  /* VALPP RAM单口访问冲突计数器 */
#define HAL_GBBP_HARQ_MODE_ADDR                     ( 0x5a00 )                  /*harq 外移模式配置*/
#define HAL_GBBP_HARQ_INIT_EN_ADDR                  ( 0x5a04 )                  /*harq 外移模式异常复位信号*/
#define HAL_GBBP_HARQ_WR_BASE_ADDR                  ( 0x5a08 )                  /*harq 外移模式配置harq DDR 写操作*/
#define HAL_HARQ_RD_BASE_ADDR                       ( 0x5a0c )                  /*harq 外移模式配置harq DDR 读操作*/
#define HAL_DDR_HARQ_CFG_ADDR                       ( 0x5a10 )                  /*harq 外移模式配置harq DDR FIFO操作*/

/*--------------------------------------------------------------------------------------------*
 * 1.13 上行信道编码模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_UL_CB1_INDEX_ADDR                   ( 20 )                      /* 上行码块业务索引号 */
#define HAL_GTC_UL_CB_NUM_ADDR                      ( 23 )                      /* 编码块起始标志,FACCH偷帧标志 */
#define HAL_GTC_IC_PUNC_INF_ADDR                    ( 24 )                      /* 带内bit0，bit1信息，打孔信息 */
#define HAL_GTC_RACH_BSIC_ADDR                      ( 25 )                      /* RACH业务中BS（基站）的BSIC比特 */
#define HAL_GTC_COD_BURST_POS_ADDR                  ( 26 )                      /* 当前编码数据第一个BURST在编码存储RAM */
#define HAL_GTC_GSP_UL_BURST_CYCLE_ADDR             ( 27 )                      /* 上行编码存放BURST循环周期，编码使能信号 */
#define HAL_GTC_GSP_UL_COD_START_ADDR               ( 28 )                      /* 上行编码启动信号 */
#define HAL_GTC_UL_MAP_PARA_ADDR                    ( 29 )                      /* 上行编码信息 */
#define HAL_GTC_MAP_BURST_POS_ADDR                  ( 30 )                      /* MAP位置 */
#define HAL_GTC_MAP_START_ADDR                      ( 31 )                      /* MAP BURST启动信号 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_UL_CODE_STATE_RPT_ADDR             ( 0x13B0 )                  /* 编码状态 */
#define HAL_GBBP_UL_CODE_RAM_RST_ADDR               ( 0x13B4 )                  /* 编码交织后存储RAM清零信号 */
#define HAL_GBBP_LOOPC_MODE_CFG_ADDR                ( 0x53f0 )                  /* LoopC环回模式 */
#define HAL_GBBP_COD_CBINDEX_RPT_ADDR               ( 0x53f4 )                  /* 上行编码信息 */
#define HAL_GBBP_UL_CODE_CFG_RAM_ADDR               ( 0x3000 )                  /* CPU配置的编码数据 */
#define HAL_GBBP_UL_MAP_CFG_RAM_ADDR                ( 0x5600 )                  /* CPU配置的调制数据 */

/*--------------------------------------------------------------------------------------------*
 * 1.14 上行调制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_MOD_START_POS_ADDR                  ( 12 )                      /* 调制启动位置 */
#define HAL_GTC_MOD_SYMBOL_LENGTH_ADDR              ( 13 )                      /* 调制长度 */
#define HAL_GTC_GSP_MOD_TYPE_ADDR                   ( 14 )                      /* 调制类型 */
#define HAL_GTC_GSP_MOD_CTRL_ADDR                   ( 15 )                      /* 调制控制信号 */
#define HAL_GTC_GSP_ULMOD_TEST_MOD_LOW_ADDR         ( 5 )                       /* 调制测试模式 */
#define HAL_GTC_GSP_ULMOD_TEST_MOD_HIGH_ADDR        ( 6 )                       /* 预调制长度 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_IQ_MOD_DTA_ADDR                ( 0x53c8 )                  /* 上行发射校准时强制I/Q路调制 */
#define HAL_GBBP_TX_DATA_MULFACTOR_ADDR             ( 0x5374 )                  /* 缩放因子 */
#define HAL_GBBP_MOD_BIT0_RPT_ADDR                  ( 0x53a0 )                  /* GMSK待调制数据0 */
#define HAL_GBBP_MOD_BIT1_RPT_ADDR                  ( 0x53a4 )                  /* GMSK待调制数据1 */
#define HAL_GBBP_MOD_BIT2_RPT_ADDR                  ( 0x53a8 )                  /* GMSK待调制数据2 */
#define HAL_GBBP_MOD_BIT3_RPT_ADDR                  ( 0x53ac )                  /* GMSK待调制数据3 */
#define HAL_GBBP_MOD_BIT4_RPT_ADDR                  ( 0x53b0 )                  /* GMSK待调制数据4 */
#define HAL_GBBP_TX_IQ_MISMATCH_COMP_ADDR           ( 0x58BC )                  /* TX iq MISMATCH */
#define HAL_GBBP_TX_IQ_DC_OFFSET_COMP_ADDR          ( 0x58C0 )                  /* TX DCOC */
#define HAL_GBBP_TX_DIG_RMAP_MODE_SEL_ADDR          ( 0x58C4 )                  /* 数字爬坡选择 */

/*--------------------------------------------------------------------------------------------*
 * 1.15 DRX模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_WAKE_TEST_QB_FRAC_RPT_ADDR         ( 0x1224 )                  /* DRX唤醒切换时GTC计数 */
#define HAL_GBBP_WAKE_TEST_T1_T2_T3_RPT_ADDR        ( 0x1228 )                  /* DRX唤醒切换时GTC此时T1，T2，T3计数 */
#define HAL_GBBP_WAKE_GTC_FN_RPT_ADDR               ( 0x122c )                  /* DRX唤醒时的GTC的FN计数 */

#define HAL_GBBP_GAUGE_EN_CNF_ADDR                  ( 0x7000 )                  /* 时钟校准长度，使能信号 */
#define HAL_GBBP_GAUGE_RESULT_RPT_ADDR              ( 0x7004 )                  /* 时钟校准结果 */


/* 物理层使用的DRX部分地址,OAM不使用 */
#define HAL_GBBP_DRX_DSP_INT_STA_ADDR               (( GDRX_BASE_ADDR+0x10 )&0xFFFF)        /* GBBP DRX 中断状态寄存器 */
#define HAL_GBBP_DRX_DSP_INT_MASK_ADDR              (( GDRX_BASE_ADDR+0x14 )&0xFFFF)        /* GBBP DRX 中断屏蔽位  */
#define HAL_GBBP_DRX_DSP_INT_CLR_ADDR               (( GDRX_BASE_ADDR+0x18 )&0xFFFF)        /* GBBP DRX 中断清零寄存器 */
#define HAL_GBBP_DRX_INT_DELAY_CLK_ADDR             (( GDRX_BASE_ADDR+0x1c )&0xFFFF)        /* GBBP DRX 中断延迟上报间隔时间参数，单位为imi_clk */
#define HAL_GBBP_DRX_PHY_INT_DELAY_REG_PD_ADDR      (( GDRX_BASE_ADDR+0x28 )&0xFFFF)        /* 非掉电区ARM 104M中断延迟上报间隔时间参数，单位为imi_clk */

#define HAL_GBBP_DRX_SLEEP_EN_ADDR                  (( GDRX_BASE_ADDR+0x30 )&0xFFFF)        /* 睡眠启动， */
#define HAL_GBBP_DRX_WAKE_CNF_ADDR                  (( GDRX_BASE_ADDR+0x34 )&0xFFFF)        /* 时钟切换时间 */
#define HAL_GBBP_DRX_WAKEUP_LEN_ADDR                (( GDRX_BASE_ADDR+0x38 )&0xFFFF)        /* 睡眠到上报唤醒时间 */
#define HAL_GBBP_DRX_MSR_CNF_ADDR                   (( GDRX_BASE_ADDR+0x3c )&0xFFFF)        /* 睡眠时间 */
#define HAL_GBBP_DRX_CPU_DRX_DEPTH_CFG_ADDR         (( GDRX_BASE_ADDR+0x40 )&0xFFFF)        /* 睡眠时间深度的门限值 */
#define HAL_GBBP_DRX_WAKE_GTC_QB_FRAC_RPT_ADDR      (( GDRX_BASE_ADDR+0x50 )&0xFFFF)        /* DRX唤醒切换时加载到GTC计数 */
#define HAL_GBBP_DRX_WAKE_GTC_T1_T2_T3_RPT_ADDR     (( GDRX_BASE_ADDR+0x54 )&0xFFFF)        /* DRX唤醒切换时加载到GTC的T1，T2，T3计数 */
#define HAL_GBBP_DRX_WAKE_GTC_FN_RPT_ADDR           (( GDRX_BASE_ADDR+0x58 )&0xFFFF)        /* DRX唤醒时的GTC的FN计数 */
#define HAL_GBBP_DRX_INT_MASK_STATE_RPT_ADDR        (( GDRX_BASE_ADDR+0x5c )&0xFFFF)        /* DRX中断状态上报 */


/************************************ OAM使用DRX地址 *****************************************/

/* OAM使用的DRX部分地址,物理层不使用 */
#define HAL_GBBP_DRX_ARM_INT_STA_ADDR               ( 0x00 )                    /* GBBP 中断状态寄存器 ARM */
#define HAL_GBBP_DRX_ARM_INT_MASK_ADDR              ( 0x04 )                    /* GBBP 中断屏蔽位 ARM */
#define HAL_GBBP_DRX_ARM_INT_CLR_ADDR               ( 0x08 )                    /* GBBP 中断清零寄存器 ARM */
#define HAL_GBBP_DRX_ARM_WAKE_EN_ADDR               ( 0x30 )                    /* 睡眠唤醒 */



/*--------------------------------------------------------------------------------------------*
 * 1.16 加解密模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_A5_DECIPH_CFG_ADDR                  ( 5 )                       /* 下行解密配置 */
#define HAL_GTC_A5_CIPH_CFG_ADDR                    ( 6 )                       /* 上行加密配置 */

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_A5KC_LOW_ADDR                  ( 0x1450 )                  /* A51/A52所需的Kc低32bit */
#define HAL_GBBP_CPU_A5KC_HIGH_ADDR                 ( 0x1454 )                  /* A51/A52所需的Kc低32bit */
#define HAL_GBBP_CPU_A5_LSB_SEQ_SEL_ADDR            ( 0x1458 )                  /* A53加解密算法bit输出顺序配置 */
#define HAL_GBBP_DECIPH_CFG_INFO1_CPU_ADDR          ( 0x1470 )                  /* 解密时配置内容1 */
#define HAL_GBBP_DECIPH_CFG_INFO2_CPU_ADDR          ( 0x1474 )                  /* 解密时配置内容2 */
#define HAL_GBBP_CIPH_CFG_INFO1_CPU_ADDR            ( 0x1478 )                  /* 加密时配置内容1 */
#define HAL_GBBP_CIPH_CFG_INFO2_CPU_ADDR            ( 0x147C )                  /* 加密时配置内容2 */

/*--------------------------------------------------------------------------------------------*
 * 1.17 GAPC模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_PA_CTRL_CFG_ADDR                    ( 4 )                       /* APC控制信号 */
#define HAL_GTC_PA_LEN_LOW_CFG_ADDR                 ( 5 )                       /* PA爬坡长度Low Byte */
#define HAL_GTC_PA_LEN_HIGH_CFG_ADDR                ( 6 )                       /* PA爬坡长度High Byte */
#define HAL_GTC_PA_TARGET_LOW_CFG_ADDR              ( 7 )                       /* PA爬坡目标电压Low Byte */
#define HAL_GTC_PA_TARGET_HIGH_CFG_ADDR             ( 8 )                       /* PA爬坡目标电压High Byte */
#define HAL_GTC_PA_INIT_LOW_CFG_ADDR                ( 9 )                       /* PA爬坡初始电压Low Byte */
#define HAL_GTC_PA_INIT_HIGH_CFG_ADDR               ( 10 )                      /* PA爬坡初始电压High Byte */


/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_CPU_GAPC_CONFIG_RAM_ADDR           ( 0x2500 )                  /* APC爬坡系数配置RAM */
#define HAL_GBBP_CPU_APC_SWITCH_EN_ADDR             ( 0x53c0 )                  /* APC爬坡系数pp访问配置 */

/*--------------------------------------------------------------------------------------------*
 * 1.18 双模切换
 *--------------------------------------------------------------------------------------------*/

/*************************************** GTC配置 *********************************************/
#define HAL_GTC_G2W_CTRL_ADDR                       ( 23 )                      /* G测量W配置 */
#define HAL_GTC_W2G_CTRL_ADDR                       ( 24 )                      /* W测量G天线配置 */

/*************************************** CPU配置 *********************************************/
/* 2G下进行4G测量期间，配置Occasion长度的地址 */
#define GPHY_DRV_CPU_2G4_GAP_LENGTH_ADDR            ( 0x1248 )

/* 2G下进行4G测量期间，GDSP提前停止occasion的信号，脉冲信号。GBBP收到该信号后，立即停止gap_cnt计数，并上报int2（出沟中断），并拉高通道控制指示信号wg_intrasys_valid_delay。自清零 */
#define GPHY_DRV_CPU_2G4_GAP_STOP_ADDR              ( 0x124c )

/* 2G测4G时, INT0与INT1间隔配置的地址 */
#define GPHY_DRV_CPU_2G4_DIST_ADDR                  ( 0x1250 )

/* 2G测4G时, INT0启动配置的地址 */
#define GPHY_DRV_CPU_2G4_INT0_GEN_QB_ADDR           ( 0x1254 )

/*--------------------------------------------------------------------------------------------*
 * 1.19 环回测试模式
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * 1.20 测试管脚控制模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_TEST_PIN_SEL_LOW_ADDR              ( 0x5700 )                  /* 测试引脚选择信号 */
#define HAL_GBBP_TEST_PIN_SEL_HIGH_ADDR             ( 0x5704 )                  /* 测试引脚选择信号 */
#define HAL_GBBP_TEST_MODULE_SEL_ADDR               ( 0x5710 )                  /* 模块测试引脚 */
#define HAL_GBBP_DRX_TEST_PIN_SEL_ADDR              ( 0x5858 )                  /* GDRX测试引脚 */
#define HAL_GBBP_GSP_START_TDMA_QB_RPT_ADDR         ( 0x5714 )                  /* 采样值 */


/*--------------------------------------------------------------------------------------------*
 * 1.21 测试寄存器模块
 *--------------------------------------------------------------------------------------------*/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_DSP_DEDICATE_CTRL_REG_ADDR         ( 0x5720 )                  /* DSP专用寄存器写使能控制字 */
#define HAL_GBBP_DSP_DEDICATE_REG8_ADDR             ( 0x574c )                  /* DSP专用寄存器8 */

/*--------------------------------------------------------------------------------------------*
 * 1.22 GBBP版本寄存器
 *--------------------------------------------------------------------------------------------*/

/*************************************** CPU配置 *********************************************/
#define HAL_GBBP_VERSION_ADDR                       ( 0x5760 )                  /* GBBP版本 */
#define HAL_GBBP_ERR_TRIG_ADDR                      ( 0x5764 )                  /* 逻辑分析仪Trigger信号 */
#define HAL_GBBP_EDGE_VERSION_ADDR                  ( 0x5768 )                  /* 版本号 */



#define HAL_GBBP_CPU_GDUMP_FIFO_RD_ADDR             ( 0xF000 )                  /* CPU读取GRIF FIFO数据偏移地址 */
#define HAL_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR          ( 0xF004 )                  /* FIFO水线深度选择,采样数据源选择,采数模块配置类型选择,采数模块总使能移地址 */
#define HAL_GBBP_CPU_GDUMP_LEN_CONFIG_ADDR          ( 0xF008 )                  /* CPU配置采样数据个数移地址 */
#define HAL_GBBP_CPU_GDUMP_SAM_LOW_TIMING_ADDR      ( 0xF00C )                  /* 可配启动时刻采数模式下移地址 */
#define HAL_GBBP_CPU_GDUMP_SAM_HIGH_TIMING_ADDR     ( 0xF010 )                  /* 可配启动时刻采数模式下，采数起始位置移地址 */
#define HAL_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR         ( 0xF014 )                  /* 可配启动时刻采数模式下，采数起始位置移地址 */




/*--------------------------------------------------------------------------------------------*
 * 1.23 OAM组件访问GBBP寄存器
 *--------------------------------------------------------------------------------------------*/

/************************************ OAM使用GBBP地址 ****************************************/

#define OAM_GBBP_FRAME_INT_BIT                      BIT_N(0)
#define OAM_GBBP_WAKE_UP_INT_BIT                    BIT_N(24)

#define OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT         BIT_N(25)
#define OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT         BIT_N(27)


/* GBBP 的寄存器定义,如果OAM代码双实例,则只需要使用一份地址 */
#define OAM_GBBP_ARM_INT_STA_ADDR                   (GBBP_BASE_ADDR+HAL_GBBP_ARM_INT_STA_ADDR)
#define OAM_GBBP_ARM_INT_MASK_ADDR                  (GBBP_BASE_ADDR+HAL_GBBP_ARM_INT_MASK_ADDR)
#define OAM_GBBP_ARM_INT_CLR_ADDR                   (GBBP_BASE_ADDR+HAL_GBBP_ARM_INT_CLR_ADDR)

#define OAM_GBBP_BLER_ERRR_RPT_ADDR                 (GBBP_BASE_ADDR+HAL_GBBP_BLER_ERRR_RPT_ADDR)
#define OAM_GBBP_BLER_TOTAL_RPT_ADDR                (GBBP_BASE_ADDR+HAL_GBBP_BLER_TOTAL_RPT_ADDR)

#define OAM_GBBP_GTC_FN_RPT_ADDR                    (GBBP_BASE_ADDR+HAL_GBBP_GTC_FN_RPT_ADDR)
#define OAM_GBBP_GTC_FN_HIGH_RPT_ADDR               (GBBP_BASE_ADDR+HAL_GBBP_GTC_FN_HIGH_RPT_ADDR)

#define OAM_GBBP_DRX_ARM_INT_STA_ADDR               (GDRX_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_STA_ADDR)
#define OAM_GBBP_DRX_ARM_INT_MASK_ADDR              (GDRX_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_MASK_ADDR)
#define OAM_GBBP_DRX_ARM_INT_CLR_ADDR               (GDRX_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_CLR_ADDR)
#define OAM_GBBP_DRX_ARM_WAKE_EN_ADDR               (GDRX_BASE_ADDR+HAL_GBBP_DRX_ARM_WAKE_EN_ADDR)


/* GBBP 的寄存器定义,如果OAM代码不是双实例,则OAM在V9副卡显示使用以下地址,V3不会定义以下地址 */
#define OAM_GBBP1_ARM_INT_STA_ADDR                  (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_ARM_INT_STA_ADDR)
#define OAM_GBBP1_ARM_INT_MASK_ADDR                 (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_ARM_INT_MASK_ADDR)
#define OAM_GBBP1_ARM_INT_CLR_ADDR                  (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_ARM_INT_CLR_ADDR)

#define OAM_GBBP1_BLER_ERRR_RPT_ADDR                (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_BLER_ERRR_RPT_ADDR)
#define OAM_GBBP1_BLER_TOTAL_RPT_ADDR               (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_BLER_TOTAL_RPT_ADDR)

#define OAM_GBBP1_GTC_FN_RPT_ADDR                   (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_GTC_FN_RPT_ADDR)
#define OAM_GBBP1_GTC_FN_HIGH_RPT_ADDR              (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_GTC_FN_HIGH_RPT_ADDR)

#define OAM_GBBP1_DRX_ARM_INT_STA_ADDR              (SOC_BBP_GSM1_ON_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_STA_ADDR)
#define OAM_GBBP1_DRX_ARM_INT_MASK_ADDR             (SOC_BBP_GSM1_ON_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_MASK_ADDR)
#define OAM_GBBP1_DRX_ARM_INT_CLR_ADDR              (SOC_BBP_GSM1_ON_BASE_ADDR+HAL_GBBP_DRX_ARM_INT_CLR_ADDR)
#define OAM_GBBP1_DRX_ARM_WAKE_EN_ADDR              (SOC_BBP_GSM1_ON_BASE_ADDR+HAL_GBBP_DRX_ARM_WAKE_EN_ADDR)


/* 采数功能 */
#define OAM_GBBP_CPU_GDUMP_FIFO_RD_ADDR             (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_FIFO_RD_ADDR)         /* CPU读取GRIF FIFO数据 */
#define OAM_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR          (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_GEN_CONFIG_ADDR)      /* FIFO水线深度选择,采样数据源选择,采数模块配置类型选择,采数模块总使能 */
#define OAM_GBBP_CPU_GDUMP_LEN_CONFIG_ADDR          (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_LEN_CONFIG_ADDR)      /* CPU配置采样数据个数，全0表示采样数据个数无限长 */
#define OAM_GBBP_CPU_GDUMP_SAM_LOW_TIMING_ADDR      (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_SAM_LOW_TIMING_ADDR)  /* 可配启动时刻采数模式下，采数起始位置，35'h7ffffffff表示立即启动，该配置为该35bit中[12:0]。默认值为立即启动模式 */
#define OAM_GBBP_CPU_GDUMP_SAM_HIGH_TIMING_ADDR     (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_SAM_HIGH_TIMING_ADDR) /* 可配启动时刻采数模式下，采数起始位置 */
#define OAM_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR         (GBBP_BASE_ADDR+HAL_GBBP_CPU_GDUMP_RECV_CONFIG_ADDR)     /* CPU配置采样数据启动信号 */

#define OAM_GBBP0_GAUGE_RESULT_RPT_ADDR             (GBBP_BASE_ADDR+HAL_GBBP_GAUGE_RESULT_RPT_ADDR)
#define OAM_GBBP1_GAUGE_RESULT_RPT_ADDR             (SOC_BBP_GSM1_BASE_ADDR+HAL_GBBP_GAUGE_RESULT_RPT_ADDR)




/*****************************************************************************
  3 枚举定义
******************************************************************************/

/*****************************************************************************
 枚举名    :
 协议表格  :
 ASN.1描述 :
 枚举说明  :
*****************************************************************************/


/*****************************************************************************
 枚举名    :
 协议表格  :
 ASN.1描述 :
 枚举说明  :
*****************************************************************************/










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
 结构名    :
 协议表格  :
 结构说明  :
*****************************************************************************/
/*****************************************************************************
 结构名    :
 协议表格  :
 结构说明  :
*****************************************************************************/

/*****************************************************************************
  7 UNION定义
*****************************************************************************/







/*--------------------------------------------------------------------------------------------*
 * GBBP寄存器定义
 *--------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------*
 * 1.1  前言
 *--------------------------------------------------------------------------------------------*/
/*****************************************************************************
 联合名    : HAL_GTC_DATA_TRANSFER_INSTRUCTION_UNION
 联合说明  : GTC数据指令格式,数据处理指令
*****************************************************************************/

typedef union
{
    unsigned short                          uhwInstructionValue;                    /* 指令字 */
    struct
    {
        unsigned short                      uhwDestAddress      : 5;                /* 指令地址 */
        unsigned short                      uhwDataOperand      : 8;                /* 指令数据 */
        unsigned short                      uhwCode             : 3;                /* 指令码 */
    }stReg;
}HAL_GTC_DATA_TRANSFER_INSTRUCTION_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_TIME_SCHEDULE_INSTRUCTION_UNION
 联合说明  : GTC时间指令格式,时间调度指令
*****************************************************************************/
typedef union
{
    unsigned short                          uhwInstructionValue;                    /* 指令字 */
    struct
    {
        unsigned short                      uhwAbsoluteTime     : 13;               /* 绝对时间，单位为QB */
        unsigned short                      uhwCode             : 3;                /* 指令码 */
    }stReg;
}HAL_GTC_TIME_SCHEDULE_INSTRUCTION_UNION;





/*--------------------------------------------------------------------------------------------*
 * 1.2  GBBP中断控制模块
 *--------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------*
 * GTC部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_CPU_GRM_INT_UNION
 联合说明  : CPU_GRM_INT寄存器
*****************************************************************************/
typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gsp_cpu_grm_it      : 8;                /* bit7~bit0：表示8个CPU可编程中断 */
        unsigned short                      reserved            : 8;                /* reserve */
    }stReg;
}HAL_GTC_CPU_GRM_INT_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_SLOTINT_OFFSET_L_UNION
 联合说明  : SLOTINT_OFFSET_L寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gsp_slot_int_offset_high    : 8;        /* bit7~0：8个时隙头中断偏移位置（一个时隙内）低8bit，固定正向偏移，单位为qb（1/4 symbol） */
        unsigned short                      reserved                    : 8;        /* reserve */
    }stReg;
}HAL_GTC_SLOTINT_OFFSET_L_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_SLOTINT_OFFSET_H_UNION
 联合说明  : SLOTINT_OFFSET_H寄存器定义
*****************************************************************************/

typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gsp_slot_int_offset_low     : 2;        /* bit1~0：8个时隙头中断偏移位置（一个时隙内）高2bit，固定正向偏移，单位为qb（1/4 symbol） */
        unsigned short                      reserved                    : 14;       /* reserve */
    }stReg;
}HAL_GTC_SLOTINT_OFFSET_H_UNION;



/*--------------------------------------------------------------------------------------------*
 * CPU部分
 *--------------------------------------------------------------------------------------------*/
/*****************************************************************************
 联合名    : HAL_GBBP_DSP_INT_CLR_UNION
 联合说明  : DSP_INT_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long                          uwValue;
    struct
    {
        unsigned long                      dsp_int00_clr       : 1;                /* TDMA帧中断清零信号，1表示中断请求清除，0表示该中断不清除 */
        unsigned long                      dsp_int01_clr       : 1;                /* GTC PAGE中断清零寄存器 */
        unsigned long                      dsp_int02_clr       : 1;                /* 同步搜索中断清零信号 */
        unsigned long                      dsp_int03_clr       : 1;                /* 译码数据上报中断清零信号 */
        unsigned long                      dsp_int04_clr       : 1;                /* 时隙0中断清零信号，1有效 */
        unsigned long                      dsp_int05_clr       : 1;                /* 时隙1中断清零信号 */
        unsigned long                      dsp_int06_clr       : 1;                /* 时隙2中断清零信号 */
        unsigned long                      dsp_int07_clr       : 1;                /* 时隙3中断清零信号 */
        unsigned long                      dsp_int08_clr       : 1;                /* 时隙4中断清零信号 */
        unsigned long                      dsp_int09_clr       : 1;                /* 时隙5中断清零信号 */
        unsigned long                      dsp_int10_clr       : 1;                /* 时隙6中断清零信号 */
        unsigned long                      dsp_int11_clr       : 1;                /* 时隙7中断清零信号 */
        unsigned long                      dsp_int12_clr       : 1;                /* 可编程中断0清零信号 */
        unsigned long                      dsp_int13_clr       : 1;                /* 可编程中断1清零信号 */
        unsigned long                      dsp_int14_clr       : 1;                /* 可编程中断2清零信号 */
        unsigned long                      dsp_int15_clr       : 1;                /* 可编程中断3清零信号 */
        unsigned long                      dsp_int16_clr       : 1;                /* 可编程中断4清零信号 */
        unsigned long                      dsp_int17_clr       : 1;                /* 可编程中断5清零信号 */
        unsigned long                      dsp_int18_clr       : 1;                /* 可编程中断6清零信号 */
        unsigned long                      dsp_int19_clr       : 1;                /* 可编程中断7清零信号 */
        unsigned long                      dsp_int20_clr       : 1;                /* RFIC模拟DCR中断清零信号 */
        unsigned long                      reserved            : 7;                /* reserved */
        unsigned long                      dsp_int28_clr       : 1;                /* 电平测量结束中断清零信号 */
        unsigned long                      dsp_int29_clr       : 1;                /* 解调结束中断清零信号 */
        unsigned long                      dsp_int30_clr       : 1;                /* ABB对GRIF FIFO的写满溢出操作中断清零信号 */
        unsigned long                      dsp_int31_clr       : 1;                /* CPU对GRIF FIFO的读空操作中断清零信号 */
    }stReg;
}HAL_GBBP_DSP_INT_CLR_UNION;



/*--------------------------------------------------------------------------------------------*
 * 1.3  射频控制模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_SPI_RD_START_ADDR_UNION
 联合说明  : SPI_RD_START_ADDR寄存器定义
*****************************************************************************/

typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gtc_spi_rd_start_addr   : 8;            /* bit7~0：GTC SPI串口待发送数据在SPI DATA RAM中的起始位置. */
        unsigned short                      reserved                : 8;            /* reserve */
    }stReg;
}HAL_GTC_SPI_RD_START_ADDR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_SPI_TRANS_DATA_NUM_UNION
 联合说明  : SPI_TRANS_DATA_NUM 寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gtc_spi_trans_data_num  : 5;            /* bit4~bit0：本次需要发送的SPI串口数据数 */
        unsigned short                      reserved                : 11;           /* reserve */
    }stReg;
}HAL_GTC_SPI_TRANS_DATA_NUM_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_SPI_RD_EN_UNION
 联合说明  : SPI_RD_EN 寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short                          uhwValue;
    struct
    {
        unsigned short                      gtc_spi_rd_en       : 1;                /* GTC启动发送SPI数据脉冲 */
        unsigned short                      reserved            : 15;               /* reserve */
    }stReg;
}HAL_GTC_SPI_RD_EN_UNION;



/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE1_CTRL_BITWISE_CLR_UNION
 联合说明  : GSP_LINE1_CTRL_BITWISE_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line1_ctrl_bitwise_clr              : 8;      /* 第一组线控清零控制。对比特位写1，才生效清零使能。写0无意义。若要置位，请看GTC寄存器地址26； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE1_CTRL_BITWISE_CLR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE1_CTRL_BITWISE_SET_UNION
 联合说明  : GSP_LINE1_CTRL_BITWISE_SET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line1_ctrl_bitwise_set              : 8;      /* 第一组线控置位控制。对比特位写1，才生效设置使能。写0无意义。若要清零，请看GTC寄存器地址25； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE1_CTRL_BITWISE_SET_UNION;



/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE2_CTRL_BITWISE_CLR_UNION
 联合说明  : GSP_LINE2_CTRL_BITWISE_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line2_ctrl_bitwise_clr              : 8;      /* 第一组线控清零控制。对比特位写1，才生效清零使能。写0无意义。若要置位，请看GTC寄存器地址26； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE2_CTRL_BITWISE_CLR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE2_CTRL_BITWISE_SET_UNION
 联合说明  : GSP_LINE2_CTRL_BITWISE_SET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line2_ctrl_bitwise_set              : 8;      /* 第一组线控置位控制。对比特位写1，才生效设置使能。写0无意义。若要清零，请看GTC寄存器地址25； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE2_CTRL_BITWISE_SET_UNION;




/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE3_CTRL_BITWISE_CLR_UNION
 联合说明  : GSP_LINE3_CTRL_BITWISE_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line3_ctrl_bitwise_clr              : 8;      /* 第一组线控清零控制。对比特位写1，才生效清零使能。写0无意义。若要置位，请看GTC寄存器地址26； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE3_CTRL_BITWISE_CLR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE3_CTRL_BITWISE_SET_UNION
 联合说明  : GSP_LINE3_CTRL_BITWISE_SET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line3_ctrl_bitwise_set              : 8;      /* 第一组线控置位控制。对比特位写1，才生效设置使能。写0无意义。若要清零，请看GTC寄存器地址25； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE3_CTRL_BITWISE_SET_UNION;



/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE4_CTRL_BITWISE_CLR_UNION
 联合说明  : GSP_LINE4_CTRL_BITWISE_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line4_ctrl_bitwise_clr              : 4;      /* 第一组线控清零控制。对比特位写1，才生效清零使能。写0无意义。若要置位，请看GTC寄存器地址26； */
        unsigned short                                         : 12;     /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE4_CTRL_BITWISE_CLR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE4_CTRL_BITWISE_SET_UNION
 联合说明  : GSP_LINE4_CTRL_BITWISE_SET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line4_ctrl_bitwise_set              : 4;      /* 第一组线控置位控制。对比特位写1，才生效设置使能。写0无意义。若要清零，请看GTC寄存器地址25； */
        unsigned short                                         : 12;     /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE4_CTRL_BITWISE_SET_UNION;




/*****************************************************************************
 联合名    : HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_CLR_UNION
 联合说明  : GSP_EXT_LINE_CTRL_BITWISE_CLR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ext_line_ctrl_bitwise_clr           : 8;      /* 第一组线控清零控制。对比特位写1，才生效清零使能。写0无意义。若要置位，请看GTC寄存器地址26； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_CLR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_SET_UNION
 联合说明  : GSP_EXT_LINE_CTRL_BITWISE_SET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ext_line_ctrl_bitwise_set           : 8;      /* 第一组线控置位控制。对比特位写1，才生效设置使能。写0无意义。若要清零，请看GTC寄存器地址25； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_EXT_LINE_CTRL_BITWISE_SET_UNION;



/*****************************************************************************
 联合名    : HAL_GTC_GSP_MIPI_START_INI_UNION
 联合说明  : GSP_MIPI_START_INI寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mipi_start                          : 1;      /* GSM配置MIPI总线写操作启动信号 */
        unsigned short gsp_mipi_init_addr                      : 6;      /* GSM配置MIPI总线写操作时，对应的配置寄存器组起始地址 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_GSP_MIPI_START_INI_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_GSP_MIPI_MUM_UNION
 联合说明  : GSP_MIPI_MUM寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mipi_num                            : 6;      /* GSM配置MIPI总线写操作时，本次操作对应的配置个数 */
        unsigned short                                         : 10;     /* reserve */
    }stReg;
}HAL_GTC_GSP_MIPI_MUM_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE2_CTRL_UNION
 联合说明  : GSP_LINE2_CTRL寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short ant_value1                              : 4;      /* 第二组线控信号。 */
        unsigned short saw_sel                                 : 1;
        unsigned short pa_band                                 : 1;
        unsigned short                                         : 10;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE2_CTRL_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_GSP_LINE3_CTRL_UNION
 联合说明  : GSP_LINE2_CTRL寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_line3_ctrl_part1                    : 6;      /* 对应于第三组线控信号gtc_gsp_line_ctrl[23:16] bit5~bit0：对应于天线开关的高6bit*/
        unsigned short gsp_line3_ctrl_part2                    : 2;
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_LINE3_CTRL_UNION;



/*--------------------------------------------------------------------------------------------*
 * CPU部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_LINE_CTRL_ADDR_UNION
 联合说明  : HAL_GBBP_CPU_LINE_CTRL_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_gtc_line_ctrl                      : 28;    /* bit0：ABB上行功率爬坡置位使能，默认值0 */
        unsigned long                                         : 3;     /* */
        unsigned long cpu_gtc_line_ctrl_wr_en                 : 1;     /* 16bit线控信号写使能指示信号 */
    }stReg;
}HAL_GBBP_CPU_LINE_CTRL_ADDR_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_GTC_GSP_LINE_STATE_RPT_ADDR_UNION
 联合说明  : HAL_GBBP_GTC_GSP_LINE_STATE_RPT_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gtc_gsp_line_ctrl                       : 28;    /* bit0：ABB上行功率爬坡置位使能，默认值0 */
        unsigned long                                         : 4;     /* */
    }stReg;
}HAL_GBBP_GTC_GSP_LINE_STATE_RPT_ADDR_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_EXT_LINE_CTRL_ADDR_UNION
 联合说明  : HAL_GBBP_CPU_EXT_LINE_CTRL_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_rf_trcv_on                          : 1;     /* bit0：bit0对应rf_trcv_on，即射频芯片工作使能信号 */
        unsigned long cpu_rf_tx_en                            : 1;     /* bit1：bit1对应rf_tx_en，即射频上行发送工作使能信号 */
        unsigned long cpu_rf_rst_en                           : 1;     /* bit2：bit2对应rf_rst_n，即射频复位控制信号 */
        unsigned long cpu_abb_rx_a_en                         : 1;     /* bit3：bit3对应abb_rxa_en。ABB RX A通道使能信号 */
        unsigned long cpu_abb_rx_b_en                         : 1;     /* bit4: bit4对应abb_rxb_en，RX B通道使能信号 */
        unsigned long cpu_abb_tx_en                           : 1;     /* bit5：bit5对应abb_tx_en，ABB TX通道使能信号 */
        unsigned long cpu_gapc_en                             : 1;     /* bit6：bit6对应gapc_en，Auxdac通道使能信号 */
        unsigned long                                         : 1;     /* bit7： */
        unsigned long                                         : 23;    /* Rsv */
        unsigned long cpu_ext_line_ctrl_wr_en                 : 1;     /* 8bit定向线控信号CPU写使能指示信号 */
    }stReg;
}HAL_GBBP_CPU_EXT_LINE_CTRL_ADDR_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_GTC_GSP_EXT_LINE_STATE_RPT_ADDR_UNION
 联合说明  : HAL_GBBP_GTC_GSP_EXT_LINE_STATE_RPT_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gtc_gsp_ext_line_ctrl                   : 8;     /* bit5：RF下行接收置位使能信号（SiGe）或GDSP软件控制rf_tcvr_on线控（适用于RFCMOS）。*/
        unsigned long                                         : 24;    /* bit4：PA工作模式选择，当置位使能有效后，PA工作模式为8PSK，默认值0 */
    }stReg;
}HAL_GBBP_GTC_GSP_EXT_LINE_STATE_RPT_ADDR_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_LINE01_SEL_CFG_ADDR_UNION
 联合说明  : HAL_GBBP_CPU_LINE01_SEL_CFG_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_Rfic_ch_sel                         : 1;     /* bit0：bit0对应RFIC0与RFIC1通道控制选择 */
        unsigned long cpu_abb_ch_sel                          : 1;     /* bit1：bit1对应ABB CH0与ABB CH1通道控制选择 */
        unsigned long cpu_auxdac_ch_sel                       : 1;     /* bit2：bit2对应AUXDAC0和AUXDAC1控制选择 */
        unsigned long cpu_mipi_ch_sel                         : 1;     /* bit3：bit3对应MIPI0和MIPI1通道控制选择 */
        unsigned long                                         : 27;    /* Rsv */
        unsigned long cpu_line01_sel_ctrl_wr_en               : 1;     /* 4bit跨通道测量配置选择信号CPU写使能指示信号 */
    }stReg;
}HAL_GBBP_CPU_LINE01_SEL_CFG_ADDR_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_LINE01_SEL_STATE_RPT_ADDR_UNION
 联合说明  : HAL_GBBP_CPU_LINE01_SEL_STATE_RPT_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gtc_line01_sel_ctrl                     : 4;     /* bit5：RF下行接收置位使能信号（SiGe）或GDSP软件控制rf_tcvr_on线控（适用于RFCMOS）。*/
        unsigned long                                         : 28;    /* bit4：PA工作模式选择，当置位使能有效后，PA工作模式为8PSK，默认值0 */
    }stReg;
}HAL_GBBP_CPU_LINE01_SEL_STATE_RPT_ADDR_UNION;




/*****************************************************************************
 联合名    : HAL_GBBP_CPU_SPI_RF_CFG_UNION
 联合说明  : CPU_SPI_RF_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_spi_rd_start_addr                   : 8;      /* bit7~0：CPU接口配置SPI串口待发送数据在SPI DATA RAM中的起始位置. */
        unsigned long cpu_spi_trans_data_num                  : 5;      /* bit12~bit8：CPU接口配置本次需要发送的SPI串口数据数 */
        unsigned long                                         : 18;     /* reserve */
        unsigned long spi_gtc_sel                             : 1;      /* bit[31]: 1表示SPI射频控制参数由GTC模块输出，0表示SPI射频控制参数由CPU接口模块输出，默认值为1;该信号只是用于软件规避TA较大时配置RF射频延时问题；正常情况下该信号都应该配置为1 */
    }stReg;
}HAL_GBBP_CPU_SPI_RF_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_1REG_UNION
 联合说明  : ADDR_G_CH_PARA_1REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h0_ext                             : 12;     /* GSM下行信道滤波器配置系数0，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h1_ext                             : 12;     /* GSM下行信道滤波器配置系数1，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_1REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_2REG_UNION
 联合说明  : ADDR_G_CH_PARA_2REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h2_ext                             : 12;     /* GSM下行信道滤波器配置系数2，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h3_ext                             : 12;     /* GSM下行信道滤波器配置系数3，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_2REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_3REG_UNION
 联合说明  : ADDR_G_CH_PARA_3REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h4_ext                             : 12;     /* GSM下行信道滤波器配置系数4，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h5_ext                             : 12;     /* GSM下行信道滤波器配置系数5，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_3REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_4REG_UNION
 联合说明  : ADDR_G_CH_PARA_4REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h6_ext                             : 12;     /* GSM下行信道滤波器配置系数6，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h7_ext                             : 12;     /* GSM下行信道滤波器配置系数7，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_4REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_5REG_UNION
 联合说明  : ADDR_G_CH_PARA_5REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h8_ext                             : 12;     /* GSM下行信道滤波器配置系数8，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h9_ext                             : 12;     /* GSM下行信道滤波器配置系数9，12bit有符号数。 初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_5REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_6REG_UNION
 联合说明  : ADDR_G_CH_PARA_6REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h10_ext                            : 12;     /* GSM下行信道滤波器配置系数10，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h11_ext                            : 12;     /* GSM下行信道滤波器配置系数11，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_6REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_7REG_UNION
 联合说明  : ADDR_G_CH_PARA_7REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h12_ext                            : 12;     /* GSM下行信道滤波器配置系数12，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h13_ext                            : 12;     /* GSM下行信道滤波器配置系数13，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_7REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_8REG_UNION
 联合说明  : ADDR_G_CH_PARA_8REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h14_ext                            : 12;     /* GSM下行信道滤波器配置系数14，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h15_ext                            : 12;     /* GSM下行信道滤波器配置系数15，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_8REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_9REG_UNION
 联合说明  : ADDR_G_CH_PARA_9REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h16_ext                            : 12;     /* GSM下行信道滤波器配置系数16，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h17_ext                            : 12;     /* GSM下行信道滤波器配置系数17，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_9REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_10REG_UNION
 联合说明  : ADDR_G_CH_PARA_10REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h18_ext                            : 12;     /* GSM下行信道滤波器配置系数18，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h19_ext                            : 12;     /* GSM下行信道滤波器配置系数19，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_10REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_11REG_UNION
 联合说明  : ADDR_G_CH_PARA_11REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h20_ext                            : 12;     /* GSM下行信道滤波器配置系数20，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h21_ext                            : 12;     /* GSM下行信道滤波器配置系数21，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_11REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_12REG_UNION
 联合说明  : ADDR_G_CH_PARA_12REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h22_ext                            : 12;     /* GSM下行信道滤波器配置系数22，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h23_ext                            : 12;     /* GSM下行信道滤波器配置系数23，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_12REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_13REG_UNION
 联合说明  : ADDR_G_CH_PARA_13REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h24_ext                            : 12;     /* GSM下行信道滤波器配置系数24，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h25_ext                            : 12;     /* GSM下行信道滤波器配置系数25，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_13REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_14REG_UNION
 联合说明  : ADDR_G_CH_PARA_14REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h26_ext                            : 12;     /* GSM下行信道滤波器配置系数26，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h27_ext                            : 12;     /* GSM下行信道滤波器配置系数27，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_14REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_15REG_UNION
 联合说明  : ADDR_G_CH_PARA_15REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h28_ext                            : 12;     /* GSM下行信道滤波器配置系数28，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h29_ext                            : 12;     /* GSM下行信道滤波器配置系数29，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_15REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_1REG_UNION
 联合说明  : ADDR_G_CH_PARA_16REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h30_ext                            : 12;     /* GSM下行信道滤波器配置系数30，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long g_ch_h31_ext                            : 12;     /* GSM下行信道滤波器配置系数31，12bit有符号数。    初始化 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_16REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_CH_PARA_17REG_UNION
 联合说明  : ADDR_G_CH_PARA_17REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long g_ch_h32_ext                            : 12;     /* GSM下行信道滤波器配置系数32，12bit有符号数。    初始化 */
        unsigned long                                         : 20;     /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_CH_PARA_17REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_1REG_UNION
 联合说明  : ADDR_G_32CH_PARA_1REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h0_ext                            : 12;     /* GSM下行信道滤波器配置系数0，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
        unsigned long g_32ch_h1_ext                            : 12;     /* GSM下行信道滤波器配置系数1，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_1REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_2REG_UNION
 联合说明  : ADDR_G_32CH_PARA_2REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h2_ext                            : 12;     /* GSM下行信道滤波器配置系数2，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
        unsigned long g_32ch_h3_ext                            : 12;     /* GSM下行信道滤波器配置系数3，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_2REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_3REG_UNION
 联合说明  : ADDR_G_32CH_PARA_3REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h4_ext                            : 12;     /* GSM下行信道滤波器配置系数4，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
        unsigned long g_32ch_h5_ext                            : 12;     /* GSM下行信道滤波器配置系数5，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_3REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_4REG_UNION
 联合说明  : ADDR_G_32CH_PARA_4REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h6_ext                            : 12;     /* GSM下行信道滤波器配置系数6，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
        unsigned long g_32ch_h7_ext                            : 12;     /* GSM下行信道滤波器配置系数7，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_4REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_5REG_UNION
 联合说明  : ADDR_G_32CH_PARA_5REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h8_ext                            : 12;     /* GSM下行信道滤波器配置系数8，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
        unsigned long g_32ch_h9_ext                            : 12;     /* GSM下行信道滤波器配置系数9，12bit有符号数。    初始化 */
        unsigned long                                          : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_5REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_6REG_UNION
 联合说明  : ADDR_G_32CH_PARA_6REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h10_ext                            : 12;     /* GSM下行信道滤波器配置系数10，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
        unsigned long g_32ch_h11_ext                            : 12;     /* GSM下行信道滤波器配置系数11，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_6REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_7REG_UNION
 联合说明  : ADDR_G_32CH_PARA_7REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h12_ext                            : 12;     /* GSM下行信道滤波器配置系数12，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
        unsigned long g_32ch_h13_ext                            : 12;     /* GSM下行信道滤波器配置系数13，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_7REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_8REG_UNION
 联合说明  : ADDR_G_32CH_PARA_8REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h14_ext                            : 12;     /* GSM下行信道滤波器配置系数14，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
        unsigned long g_32ch_h15_ext                            : 12;     /* GSM下行信道滤波器配置系数15，12bit有符号数。    初始化 */
        unsigned long                                           : 4;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_8REG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_ADDR_G_32CH_PARA_2REG_UNION
 联合说明  : ADDR_G_32CH_PARA_2REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long g_32ch_h16_ext                            : 12;     /* GSM下行信道滤波器配置系数16，12bit有符号数。    初始化 */
        unsigned long                                           : 20;      /* reserve */
    }stReg;
}HAL_GBBP_ADDR_G_32CH_PARA_9REG_UNION;




/*--------------------------------------------------------------------------------------------*
 * 1.4  GRIF模块
 *--------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------*
 *1.5   自动增益控制模块
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * GTC部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_GSP_INIT_GAIN_RFIC_ADDR_REG
 联合说明  : GSP_INIT_GAIN_RFIC_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_init_gain_rfic                      : 3;     /* bit2~0: RF每时隙处理的初始档位。自研RF档位与增益控制值对应关系参见表19。 */
        unsigned short                                         : 3;
        unsigned short gsp_dcr_burst_mode                      : 1;     /* 时隙去直流模式 */
        unsigned short gsp_rf_gain_word_sel                    : 1;     /* 8档射频控制字选择信号 */
        unsigned short                                         : 8;     /* reserve */
    }stReg;
}HAL_GTC_GSP_INIT_GAIN_RFIC_ADDR_REG;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_DAGC_CTRL_UNION
 联合说明  : GSP_DAGC_CTRL寄存器定义
*****************************************************************************/

typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dagc_enDEL                          : 1;      /* bit0:DAGC模块工作使能信号。 */
        unsigned short gsp_dagc_process_sel                    : 1;      /* DAGC处理模式选择，1为搜索模式，0为NB解调模式。 */
        unsigned short reserve                                 : 14;     /* reserve */
    }stReg;
}HAL_GTC_GSP_DAGC_CTRL_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_AGC_CTRL1_UNION
 联合说明  : GSP_AGC_CTRL1寄存器定义
*****************************************************************************/

typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_agc_en                              : 1;      /* AGC模块工作使能信号。 */
        unsigned short gsp_frq_sel                             : 3;      /* 直流偏置和前向通道增益调整RAM地址区域选择信号。 */
        unsigned short                                         : 2;      /* reserve */
        unsigned short gsp_fast_agc_mode                       : 1;      /* fast 测量模式 */
        unsigned short gsp_gain_sel                            : 1;      /* AGC档位配置方式选择。0，由agc_init_gain配置，1，由agc_next_gain配置。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_AGC_CTRL1_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_GSP_AGC_CTRL2_UNION
 联合说明  : GSP_AGC_CTRL2寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_agc_average_time_round_one          : 2;      /* AGC三轮能量累加处理长度配置。 */
        unsigned short gsp_agc_average_time_round_two          : 2;      /* AGC三轮能量累加处理长度配置。 */
        unsigned short gsp_agc_average_time_round_three        : 2;      /* AGC三轮能量累加处理长度配置。 */
        unsigned short gsp_dcr_nv_sel                          : 1;      /* 粗调直流选择。 */
        unsigned short gsp_slot_dcr_mode                       : 1;      /* 时隙前计算直流模式开启。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_AGC_CTRL2_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_AGC_START_UNION
 联合说明  : GSP_AGC_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_register_clr                        : 1;      /* 多路数字细调环偏置寄存器清零，1个系统周期的脉冲信号。 */
        unsigned short gsp_init_gain_en                        : 1;      /* AGC工作时隙起始档位配置有效指示，1个系统周期的脉冲信号。需提前数据接收时刻100us启动该信号，需与gsp_init_gain和gsp_gain_sel信号配合使用。 */
        unsigned short gsp_burst_register_clr                  : 1;      /* 时隙前直流累加细调环清零信号。 */
        unsigned short gsp_dc_cal_start                        : 1;      /* 时隙前直流计算启动信号。 */
        unsigned short gsp_dcr_recv_active                     : 1;      /* 解调与搜索通道直流偏置数字细调环激活，高电平有效 */
        unsigned short                                         : 2;      /* reserve */
        unsigned short gsp_agc_fast_start                      : 1;      /* AGC模块快速测量开始指示信号，1个系统周期的脉冲信号。在工作开始指示信号发出之前需要配置好AGC的本时隙的初始档位。快速测量不进行解调与搜索。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_AGC_START_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_RSSI_FRQ_ADDR_UNION
 联合说明  : GSP_RSSI_FRQ_HIGH_ADDR寄存器定义
*****************************************************************************/

typedef union
{
    unsigned short uhwValue;

    struct
    {
        unsigned short low_byte                                : 8;
        unsigned short high_byte                               : 8;
    }stWord;

    struct
    {
        unsigned short gsp_rssi_frq_low                        : 8;      /* 当前测量的RSSI对应的频点号低8bit */
        unsigned short gsp_rssi_frq_high                       : 4;      /* 当前测量的RSSI对应的频点号高4bit，GSP_RSSI_FRQ_LOW_ADDR，GSP_RSSI_FRQ_HIGH_ADDR输出gsp_rssi_frq[11:0] */
        unsigned short gsp_rrc_filter_para_sel                 : 3;     /* reserve */
        unsigned short                                         : 1;
    }stReg;
}HAL_GTC_GSP_RSSI_FRQ_ADDR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_AGC_TRUNC_WIDTH_UNION
 联合说明  : GSP_AGC_TRUNC_WIDTH寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_agc_trunc_width                     : 3;      /* AGC测量数字细调环截位位宽，默认值3'd3。                                        3'd0：8；                                  3'd1：10；                                  3'd2：12；                                  3'd3：14；                                  3'd4：4；                                  3'd5：6 */
        unsigned short gsp_dagc_trunc_width                    : 3;      /* DAGC测量数字细调环截位位宽，默认值3'd3。                                        3'd0：8；                                  3'd1：10；                                  3'd2：12；                                  3'd3：14；                                  3'd4：4；                                  3'd5：6。 */
        unsigned short                                         : 2;
        unsigned short reserved                                : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_AGC_TRUNC_WIDTH_UNION;

/*--------------------------------------------------------------------------------------------*
 * GPU部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_SAMPLE_REG_UNION
 联合说明  : CPU_SAMPLE_REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long                                         : 8;      /* reserve */
        unsigned long cpu_sample_sel_group                    : 1;      /* 采样点组选择配置，该信号需要用帧头锁存。 */
        unsigned long                                         : 3;      /* reserve */
        unsigned long cpu_dcr_bypass                          : 1;      /* 测量通道直流偏置数字细调环旁路功能，高电平有效。 */
        unsigned long                                         : 19;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_SAMPLE_REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_AGC_WAIT_LENGTH_UNION
 联合说明  : AGC_WAIT_LENGTH寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_agc_wait                            : 5;                 /* 快速AGC能量计算每轮之间的等待时间，单位symbol，默认8。 */
        unsigned long                                         : 3;                 /* reserve */
        unsigned long cpu_dcr_recv_bypass                     : 1;                 /* 解调与搜索通道直流偏置数字细调环旁路功能，高电平有效。该信号有效时，相当于数字细调环不对接收数据进行反馈处理 */
        unsigned long                                         : 3;
        unsigned long cpu_rssi_hypersis                       : 4;                 /* RSSI磁滞范围，单位dBm */
        unsigned long                                         : 8;
        unsigned long cpu_dcr_burst_mode                      : 1;                 /* 解调通道直流偏置数字细调环旁路功能，高电平有效。 */
        unsigned long                                         : 7;
    }stReg;
}HAL_GBBP_AGC_WAIT_LENGTH_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_R2_RSSI1234_THRESHOLD_UNION
 联合说明  : R2_RSSI1234_THRESHOLD寄存器定义
*****************************************************************************/

typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_r2_rssi1_threshold                     : 8;      /* RSSI门限1，单位dBm */
        unsigned long cpu_r2_rssi2_threshold                     : 8;      /* RSSI门限2，单位dBm */
        unsigned long cpu_r2_rssi3_threshold                     : 8;      /* RSSI门限3，单位dBm */
        unsigned long                                            : 8;      /* reserve */
    }stReg;
}HAL_GBBP_R2_RSSI1234_THRESHOLD_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_R2_RSSI4567_THRESHOLD_UNION
 联合说明  : R2_RSSI4567_THRESHOLD寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_r2_rssi4_threshold                     : 8;      /* RSSI门限4，单位dBm */
        unsigned long cpu_r2_rssi5_threshold                     : 8;      /* RSSI门限5，单位dBm */
        unsigned long cpu_r2_rssi6_threshold                     : 8;      /* RSSI门限6，单位dBm */
        unsigned long cpu_r2_rssi7_threshold                     : 8;      /* RSSI门限7，单位dBm */
    }stReg;
}HAL_GBBP_R2_RSSI4567_THRESHOLD_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_R3_RSSI1234_THRESHOLD_UNION
 联合说明  : R3_RSSI1234_THRESHOLD寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_r3_rssi1_threshold                     : 8;      /* RSSI门限1，单位dBm */
        unsigned long cpu_r3_rssi2_threshold                     : 8;      /* RSSI门限2，单位dBm */
        unsigned long cpu_r3_rssi3_threshold                     : 8;      /* RSSI门限3，单位dBm */
        unsigned long                                            : 8;      /* reserve */
    }stReg;
}HAL_GBBP_R3_RSSI1234_THRESHOLD_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_R3_RSSI4567_THRESHOLD_UNION
 联合说明  : R3_RSSI4567_THRESHOLD寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_r3_rssi4_threshold                     : 8;      /* RSSI门限4，单位dBm */
        unsigned long cpu_r3_rssi5_threshold                     : 8;      /* RSSI门限5，单位dBm */
        unsigned long cpu_r3_rssi6_threshold                     : 8;      /* RSSI门限6，单位dBm */
        unsigned long cpu_r3_rssi7_threshold                     : 8;      /* RSSI门限7，单位dBm */
    }stReg;
}HAL_GBBP_R3_RSSI4567_THRESHOLD_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_OFC_RXCM_REG_UNION
 联合说明  : CPU_OFC_RXCM_REG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long                                         : 31;      /* reserve */
        unsigned long cpu_agc_page_switch_en                  : 1;      /* AGC模块配置前向通道增益调整值和RXCORR参数存储RAM 和DC_OFFSET 存储RAM  */
    }stReg;
}HAL_GBBP_CPU_OFC_RXCM_REG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_DC_OFFSET_UNION
 联合说明  : DC_OFFSET寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long i_dc_offset                      : 14;      /* I路DC */
        unsigned long                                  : 2;
        unsigned long q_dc_offset                      : 14;      /* Q路DC */
        unsigned long                                  : 2;
    }stReg;
    struct
    {
        unsigned long i_dc_offset                      : 12;      /* I路DC */
        unsigned long                                  : 4;
        unsigned long q_dc_offset                      : 12;      /* Q路DC */
        unsigned long                                  : 4;
    }stRegNonPilot;
}HAL_GBBP_DC_OFFSET_UNION;



/*--------------------------------------------------------------------------------------------*
 * 1.7  复位模块
 *--------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------*
 * 1.8  子模块时钟控制模块
 *--------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------*
 * 1.9  GTC模块
 *--------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------*
 * GTC部分
 *--------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------*
 * CPU部分
 *--------------------------------------------------------------------------------------------*/
/*****************************************************************************
 联合名    : HAL_GBBP_CPU_GTC_T1_T2_T3_CNG_UNION
 联合说明  : CPU_GTC_T1_T2_T3_CNG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_gtc_t1_cnt                          : 11;     /* 精简帧号维护机制中的T1初始值，该参数值与cpu_gtc_t2_cnt，cpu_gtc_t3_cnt， */
        unsigned long                                         : 5;      /* reserve */
        unsigned long cpu_gtc_t2_cnt                          : 5;      /* 精简帧号维护机制中的T2初始值，该参数值与cpu_gtc_t1_cnt，在GTC的TDMA帧边界处生效,所以应配置为下一帧的帧号 */
        unsigned long                                         : 3;      /* reserve */
        unsigned long cpu_gtc_t3_cnt                          : 6;      /* 精简帧号维护机制中的T3初始值，该参数值与cpu_gtc_t1_cnt， */
        unsigned long                                         : 2;      /* reserve */
    }stReg;
}HAL_GBBP_CPU_GTC_T1_T2_T3_CNG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_GTC_FRM_OFFSET_CFG_UNION
 联合说明  : CPU_GTC_FRM_OFFSET_CFG寄存器定义
*****************************************************************************/

typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_gtc_frm_offset                      : 13;     /* bit[12:0] CPU配置的gtc_frm_offset参数 */
        unsigned long                                         : 3;      /* reserve */
        unsigned long cpu_gtc_frm_it_offset                   : 13;     /* bit[28:16] CPU配置的gtc_frm_it_offset参数 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long cpu_gtc_frm_offset_wr_en                : 1;      /* CPU配置的cpu_gtc_frm_offset，cpu_gtc_frm_it_offset立即生效指示信号，逻辑自清零。 */
    }stReg;
}HAL_GBBP_CPU_GTC_FRM_OFFSET_CFG_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_GTC_FN_CNG_UNION
 联合说明  : CPU_GTC_FN_CNG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_gtc_fn_low                          : 11;     /* bit[10:0] CPU配置的TDMA帧号低位0~26x51-1循环计数, CPU配置后，在GTC的TDMA帧边界处生效,所以应配置为下一帧的帧号 */
        unsigned long                                         : 5;      /* reserve */
        unsigned long cpu_gtc_fn_high                         : 11;     /* bit[26:16] CPU配置TDMA帧号高位0~2047循环计数，低位计满后，高位才加1；CPU配置后，在GTC的TDMA帧边界处生效,所以应配置为下一帧的帧号 */
        unsigned long                                         : 4;      /* reserve */
        unsigned long cpu_gtc_fn_wr_en                        : 1;      /* 帧号初始化信号，1有效，逻辑自清零，只有当需要T1,T2,T3，fn_low，fn_high都写入后方可对该bit置1 */
    }stReg;
}HAL_GBBP_CPU_GTC_FN_CNG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_GTC_FN_RPT_UNION
 联合说明  : GTC_FN_RPT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uhwValue;
    struct
    {
        unsigned long gtc_fn_low                              : 11;     /* GTC此时实际帧号低位 */
        unsigned long                                         : 5;      /* reserve */
        unsigned long gtc_fn_high                             : 11;     /* bit[26:16] GTC此时实际帧号高位 */
        unsigned long                                         : 5;      /* reserve */
    }stReg;
}HAL_GBBP_GTC_FN_RPT_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_GTC_CFG_UNION
 联合说明  : CPU_GTC_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uhwValue;
    struct
    {
        unsigned long gtc_page_switch_en                      : 1;      /* 0：不切换指令RAM页，1：乒乓切换指令RAM页。复位后第一次配置时应切换指令页。 */
        unsigned long cpu_gtc_start                           : 1;      /* 写1启动指令页执行，内部产生一个52M脉冲， */
        unsigned long gtc_auto_run                            : 1;      /* 1：GTC指令在TDMA帧头处自动执行使能，默认为0； */
        unsigned long cpu_gtc_hold_page                       : 1;      /* 1表示允许修改正在执行的GTC指令页，需要修改时需要对其置1，修改完成后将其置0  */
        unsigned long gtc_pro_en                              : 1;      /* GTC模块工作使能信号，首先配置该信号为高，然后再启动GTC */
        unsigned long                                         : 3;      /* reserve */
        unsigned long cpu_drx_test_mode_en                    : 1;      /* 1表示DRX测试模式使能，此时GTC时钟应不关闭，用于DRX唤醒后的定时维护情况的比对。正常使用下该信号应该被置0，测试用 */
        unsigned long                                         : 23;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_GTC_CFG_UNION;

/*--------------------------------------------------------------------------------------------*
 * 1.10 同步搜索模块
 *--------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------*
 * GTC部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_SRCH_CFG_UNION
 联合说明  : GTC_SRCH_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_srch_en                             : 1;      /* CPU搜索使能信号，在整个搜索期间都保持为高电平。 */
        unsigned short gsp_srch_mod                            : 3;      /* 搜索模式配置： */
        unsigned short gsp_fb_pos_adj                          : 3;      /* CPU对FB搜索的位置的调整，主要为规避设计风险而设计的。 */
        unsigned short gsp_soft_agc_drssi_25db_en              : 1;      /* 软件强制配置DRSSI的使能开关 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_SRCH_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_SRCH_START_UNION
 联合说明  : GTC_SRCH_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_srch_start                          : 1;      /* cpu对搜索的启动信号，脉冲信号。每次启动搜索时需要置一次，1有效（下同）,GTC模块内部已做处理，使其在需要的qb内的第一个clk_sys处生效,GTC逻辑自清零。当gsp_srch_mod=001时，gsp_srch_start必须提前当前默认的SB位置34个符号产生。 */
        unsigned short                                         : 5;      /* reserve */
        unsigned short gsp_nco_en                              : 1;      /* reserve */
        unsigned short gsp_ncell_srch_flag                     : 1;      /* 1标识当前是邻区搜索，0标识当前是服务区搜索 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_SRCH_START_UNION;

/*--------------------------------------------------------------------------------------------*
 * CPU部分
 *--------------------------------------------------------------------------------------------*/
/*****************************************************************************
 联合名    : HAL_GBBP_CPU_AFC_CFG_UNION
 联合说明  : CPU_AFC_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_afc_init                            : 14;     /* CPU配置的AFC的初始值，cpu_afc_init需要在cpu_afc_init_en前准备好，在开机后的初始搜索，需要配置此值   mpu_int0 */
        unsigned long reserved0                               : 1;      /* reserve */
        unsigned long cpu_afc_init_en                         : 1;      /* AFC初始值写入使能，CPU写1，逻辑自清零，请先配置CPU_AFC_LOOP1_CFG，再配置CPU_AFC_CFG mpu_int0 */
        unsigned long reserved1                               : 16;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_AFC_CFG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_AFC_RPT_UNION
 联合说明  : AFC_RPT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long afc_vctxo_cpu                           : 14;     /* AFC输出的VCTXO的控制电压信号    mpu_int_2 */
        unsigned long                                         : 18;     /* reserve */
    }stReg;
}HAL_GBBP_AFC_RPT_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_AFC_LOOP1_RPT_UNION
 联合说明  : AFC_LOOP1_RPT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long nbafc_1loop_cpu                         : 28;     /* 当前NB-AFC环路滤波累加器一的上报值 */
        unsigned long                                         : 4;      /* reserve */
    }stReg;
}HAL_GBBP_AFC_LOOP1_RPT_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_FB_TIMES_THRES_UNION
 联合说明  : CPU_FB_TIMES_THRES寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_fb_failed_thres                     : 2;      /* FB同步失败次数门限。 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long cpu_sb_failed_thres                     : 2;      /* SB译码失败次数门限。 */
        unsigned long                                         : 6;      /* reserve */
        unsigned long cpu_fb_peak_window                      : 17;     /* FB搜索窗的符号个数门限值。缺省值为13906（对应11帧＋1时隙）。当搜索的时间长度大于该值时，FB搜索结束。 */
        unsigned long                                         : 3;      /* reserve */
    }stReg;
}HAL_GBBP_CPU_FB_TIMES_THRES_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_FB_KP_KINT_UNION
 联合说明  : CPU_FB_KP_KINT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long                                         : 16;     /* reserve */
        unsigned long cpu_afc_maxfbnum                        : 5;      /* 用于FB－AFC判决的总FB个数。 */
        unsigned long                                         : 11;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_FB_KP_KINT_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_SB_ADVANCE_LEN_CFG_UNION
 联合说明  : SB_ADVANCE_LEN_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_sb_advance_len                      : 6;      /* 001搜索模式下，SB提前接收的符号个数。即在SB之前，多接收cpu_sb_advance_len个符号，范围是0～34 */
        unsigned long reserve                                 : 26;     /* reserve */
    }stReg;
}HAL_GBBP_SB_ADVANCE_LEN_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_SRCH_QB_RPT_UNION
 联合说明  : SRCH_QB_RPT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long srch_timebase_qb                        : 13;     /* 搜索中断所在gtc_timebase上报 */
        unsigned long                                         : 3;      /* reserve */
        unsigned long srch_int_mod                            : 3;      /* 搜索中断时gsp_srch_mod上报 */
        unsigned long                                         : 9;      /* reserve */
        unsigned long srch_start_mod                          : 3;      /* 搜索启动时gsp_srch_mod上报 */
        unsigned long ncell_srch_flag                         : 1;      /* 当前搜索小区类型上报，1表示邻区，0表示服务区 */
    }stReg;
}HAL_GBBP_SRCH_QB_RPT_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_NB_AFC_LOOP_SWITCH_UNION
 联合说明  : NB_AFC_LOOP_SWITCH寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_nb_afc_switch                       : 1;      /* 一阶环与二阶环选择控制信号。0:表示采用1阶环路；1:表示采用2阶环路。 */
        unsigned long cpu_nb_afc_slow_fast_switch             : 1;      /* 二阶环快慢速阶段控制。0:表示采用慢速跟踪；1:表示采用快速跟踪。*/
        unsigned long                                         : 29;
        unsigned long cpu_nb_afc_sel                          : 1;      /* NB-AFC控制方式选择。0，GTC控制；1，CPU控制。默认CPU控制。 */
    }
    stReg;
}HAL_GBBP_NB_AFC_LOOP_SWITCH_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.11 下行解调模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_DEMOD_PARA1_UNION
 联合说明  : GTC_DEMOD_PARA1寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_nb_demod_en                         : 1;      /* NB解调使能信号，高电平有效，在NB解调过程中一直为高，在启动译码前也需要将该信号置1 */
        unsigned short gsp_dl_tsc_group_num                    : 3;      /* 训练序列组号 */
        unsigned short gsp_demod_slot                          : 3;      /* 启动解调时的时隙号，用于TOA值随路上报 */
        unsigned short gsp_nserv_cell_ind                      : 1;      /* 1表示非服务小区 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEMOD_PARA1_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEMOD_PARA2_UNION
 联合说明  : GTC_DEMOD_PARA2寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_demod_type                          : 2;      /* 下行调制方式指示 00 GMSK调制 01 8PSK调制,10表示下行调制方式未知，11 无意义； */
        unsigned short gsp_ls_dcr_en                           : 1;      /* reserve */
        unsigned short gsp_demod_flag_sav_pos                  : 5;      /* PS域DATA业务下 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEMOD_PARA2_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEMOD_PARA3_UNION
 联合说明  : GTC_DEMOD_PARA3寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dem_burst_pos                       : 7;      /* 解调BURST数据存储起始位置指示 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_DEMOD_PARA3_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_TOA_PARA_UNION
 联合说明  : GTC_TOA_PARA寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_nb_pos                              : 4;      /* 软件规避模式下由软件计算的当前NB位置的调整值。正常模式下该信号可以配置为0 */
        unsigned short gsp_nb_toa_alpha_sel                    : 1;      /* 选择是否在本时隙进行能量Alpha滤波，电平信号。 */
        unsigned short gsp_nb_pos_sel                          : 1;      /* 软件计算当前NB位置的选择信号。 */
        unsigned short gsp_nb_nco_en                           : 1;      /* 解邻区NB时，纠偏使能信号 */
        unsigned short gsp_nb_afc_en                           : 1;      /* NB-AFC使能信号，1有效。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_TOA_PARA_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEMOD_START_UNION
 联合说明  : GTC_DEMOD_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_demod_start                         : 1;      /* 下行NB解调启动信号，脉冲信号， */
        unsigned short gsp_gaci_en                             : 1;      /* 邻频检测模块使能信号。邻频抑制功能仅针对于NB情况下使用，搜索模式下（FB、SB）不进行邻频检测。 */
        unsigned short gsp_gaci_index                          : 2;      /* 滤波器系数选择位 */
        unsigned short                                         : 12;     /* reserve */
    }stReg;
}HAL_GTC_DEMOD_START_UNION;





/*--------------------------------------------------------------------------------------------*
 * CPU模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_SAIC_CTRL_UNION
 联合说明  : CPU_SAIC_CTRL寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_saic_shift_pre                      : 2;      /* 判决反馈前，SAIC滤波输出值的移位值，11～13比特，默认值2'b01。该值用来调整性能偏差。 */
        unsigned long                                         : 6;      /* reserve */
        unsigned long cpu_saic_shift_aft                      : 2;      /* 判决反馈后，SAIC滤波输出值的移位值，10～12比特，默认值是2'b01。该值用来调整性能偏差。 */
        unsigned long                                         : 6;      /* reserve */
        unsigned long cpu_renew_toa_pos                       : 2;      /* DSP在帧头位置配置下帧NB位置的调整量。 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long cpu_gmsk_ci_beta                        : 2;      /* GMSK CI测量的调整系数，默认值是15(配置为2'b00)，可选值1和8 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long cpu_cross_delta                         : 3;      /* case(cpu_cross_delta) */
        unsigned long                                         : 4;      /* reserve */
        unsigned long cpu_nb_info_rpt_mode                    : 1;      /* 1 表示固定选择偶路，0表示选择最佳路 默认值为0。                     此信号用于选择上报的nb_pos_cpu, nb_pos_adj_cpu和C/I等。 */
    }stReg;
}HAL_GBBP_CPU_SAIC_CTRL_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_ENG_WEIGHT_UNION
 联合说明  : ENG_WEIGHT寄存器定义
*****************************************************************************/

typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_mid_finger_weight                   : 3;      /* NB位置计算使用的中心径能量加权值，默认值0 */
        unsigned long                                         : 1;      /* reserve */
        unsigned long cpu_rptpos_use_weight                   : 1;      /* 上报的nb_pos和nb_pos_adj使用中心径能量加权算法，1为使用，默认值0 */
        unsigned long                                         : 3;      /* reserve */
        unsigned long cpu_dempos_use_weight                   : 1;      /* 解调位置继承使用中心径能量加权算法，1为使用，默认值0 */
        unsigned long                                         : 7;      /* reserve */
        unsigned long cpu_sb_finger_weight                    : 3;      /* SB位置计算使用的中心径能量加权值，默认值0 */
        unsigned long                                         : 13;     /* reserve */
    }stReg;
}HAL_GBBP_ENG_WEIGHT_UNION;



/*****************************************************************************
 联合名    : HAL_GBBP_CPU_DEM_WIDTH_CFG_UNION
 联合说明  : CPU_DEM_WIDTH_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_demod_width_adjust                  : 2;      /* 解调动态位宽配置 */
        unsigned long reserved0                               : 14;     /* reserve */
        unsigned long cpu_harq_width_mode                     : 1;      /* HARQ译码前数据位宽恢复模式配置 */
        unsigned long reserved1                               : 15;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_DEM_WIDTH_CFG_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_TOA_ALPHA_PARA_UNION
 联合说明  : CPU_TOA_ALPHA_PARA寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_alpha_parameter                     : 3;      /* alpha滤波系数选择： */
        unsigned long                                         : 29;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_TOA_ALPHA_PARA_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_CI_THRES_CFG_UNION
 联合说明  : CPU_CI_THRES_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_gmsk_ci_thres                       : 3;                 /* GMSK调制方式下，CI判决门限选择信号，用于筛选nb-afc的输出 */
        unsigned long                                         : 13;                /* reserve */
        unsigned long cpu_psk_ci_thres                        : 3;                 /* 8PSK调制方式下，CI判决门限选择信号，用于筛选nb-afc的输出 */
        unsigned long                                         : 13;                /* reserve */
    }stReg;
}HAL_GBBP_CPU_CI_THRES_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_LS_DC_CPU_UNION
 联合说明  : LS_DC_CPU寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long ls_dc_q_cpu                           : 8;                /* Q路DC */
        unsigned long                                       : 8;
        unsigned long ls_dc_i_cpu                           : 8;                /* I路DC */
        unsigned long                                       : 8;
    }stReg;
}HAL_GBBP_LS_DC_CPU_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.12 下行信道译码模块
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * GTC模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_DL_CB_INDEX_UNION
 联合说明  : GTC_DL_CB_INDEX寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dl_cb_index                         : 7;      /* 一般情况下表示下行业务序号 */
        unsigned short gsp_cycle_rpt_en                        : 1;      /* 1表示循环上报模式使能，此时单码块完成译码就上报中断 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DL_CB_INDEX_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEC_BURST_POS_UNION
 联合说明  : GTC_DEC_BURST_POS寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dec_burst_pos                       : 7;      /* 表示待译码码块的第一个BURST在VALPP软值存储RAM中的位置编号 */
        unsigned short gsp_amr_state_flag                      : 1;      /* 1表示处于AMR 的SPEECH态 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEC_BURST_POS_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEC_TYPE1_UNION
 联合说明  : GTC_DEC_TYPE1寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_enhanced_dec_mode                   : 2;      /* gsp_enhanced_dec_mode[1]＝1表示增强型译码模式使能；0表示采用传统VTB译码模式 */
        unsigned short                                         : 2;      /* reserve */
        unsigned short gsp_dl_amr_cmc_frame                    : 1;      /* 表示当前帧号的CMI和CMC指示，0表示是CMI帧，1表示是CMC帧。复位默认值为0 */
        unsigned short gsp_dl_amr_cmi_value                    : 2;      /* 软件配置的历史CMI值 */
        unsigned short gsp_facch_dec_mode                      : 1;      /* FACCH译码模式选择，AFS，FS，EFS下1 表示相关译码结果判断是否为FACCH偷帧，如果是,则逻辑启动FACCH译码,如果不是,则逻辑启动其他码块译码；0 表示直接启动FACCH译码根据译码的CRC校验结果判断是否为FACCH偷帧。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEC_TYPE1_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DEC_TYPE2_UNION
 联合说明  : GTC_DEC_TYPE2寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_cs4_judge_mode                      : 2;      /* 00 表示在GPRS网络，根据网络分配信息，将CS4和MCS1234区分为CS4 */
        unsigned short                                         : 2;      /* reserve */
        unsigned short gsp_edge_usf_rxqual_en                  : 1;      /* 1表示 EDGE业务下USF参与BER和BEP统计，默认值为0 */
        unsigned short gsp_bep_en                              : 1;      /* 1表示打开bep统计功能，0表示关闭bep统计功能，默认值为0 */
        unsigned short gsp_ib_im_rxqual_en                     : 1;      /* 1表示带内bit ib和ID_MARKER bit参与质量测量统计 */
        unsigned short gsp_harq_func_en                        : 1;      /* 1表示软件配置HARQ功能使能，0表示该功能屏蔽 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEC_TYPE2_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_HARQ_POS1_UNION
 联合说明  : GTC_HARQ_POS1寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_harq1_pos                           : 7;      /* 在HARQ译码过程中表示需要做harq合并的第一个码块的读起始位置；在HARQ软值保存过程中为第一个码块的写起始位置 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_HARQ_POS1_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_HARQ_POS2_UNION
 联合说明  : GTC_HARQ_POS2寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_harq2_pos                           : 7;      /* 在HARQ译码过程中表示需要做harq合并的第二个码块的读起始位置；在HARQ软值保存过程中为第二个码块的写起始位置 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_HARQ_POS2_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_HARQ1_CB_INDEX_UNION
 联合说明  : GTC_HARQ1_CB_INDEX寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_harq1_cb_index                      : 7;      /* bit6～bit0：参与HARQ合并译码的任意一个码块（应该为DATA块）的CB_INDEX，复位默认值为7'd52，如果是MCS6，9、MCS5，7合并，那么可以选择任意一个码块（应该为DATA块）的cb_index进行配置 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_HARQ1_CB_INDEX_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_HARQ2_CB_INDEX_UNION
 联合说明  : GTC_HARQ2_CB_INDEX寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_harq2_cb_index                      : 7;      /* bit6～bit0：参与HARQ合并译码的第二个码块的CB_INDEX */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_HARQ2_CB_INDEX_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_DL_BURST_CYCLE_UNION
 联合说明  : GTC_DL_BURST_CYCLE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dl_burst_sav_cycle                  : 6;      /* 下行译码存放BURST循环周期（该参数只对CS域业务有效），复位默认值为24，表示周期为24；在22BURST交织的业务下需要根据软件需求配置。 */
        unsigned short gsp_bler_en                             : 1;      /* 质量测量BLER模块使能信号，高电平有效。（必要时关断，以节省功耗） */
        unsigned short gsp_ber_en                              : 1;      /* 质量测量BER模块使能信号，高电平有效。（必要时关断，以节省功耗） */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DL_BURST_CYCLE_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_DEC_TEST_MODE_EN_UNION
 联合说明  : GSP_DEC_TEST_MODE_EN寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dec_test_mode_en                    : 1;      /* 1表示译码模式是测试模式，此时译码模块不做帧类型判决 */
        unsigned short gsp_enhanced_ut_en                      : 1;      /* 1表示是增强型译码的UT验证模式，测试用 */
        unsigned short                                         : 2;      /* reserve */
        unsigned short gsp_reduce_page_mode                    : 4;      /* bit3~bit0，分别表示当前精简寻呼译码时有效的BURST是否存在，如0011标识第一个，第二个BURST存在，第三个第四个BURST译码数据补零。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_DEC_TEST_MODE_EN_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_DEC_TEST_AMR_FRM_UNION
 联合说明  : GSP_DEC_TEST_AMR_FRM寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dec_test_amr_frm                    : 6;      /* 在译码测试模式下配置的AMR帧类型，此时仍然需要配置gsp_dl_cb_index为AFS或者AHS业务 */
        unsigned short                                         : 10;     /* reserve */
    }stReg;
}HAL_GTC_GSP_DEC_TEST_AMR_FRM_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_DEC_TEST_PS_FRM_UNION
 联合说明  : GSP_DEC_TEST_PS_FRM寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dec_test_ps_frm                     : 3;      /* 在译码测试模式下配置的PS业务帧类型 */
        unsigned short gsp_dec_test_ps_cps                     : 5;      /* 在译码测试模式下配置的PS业务对应具体业务的CPSbit信息，该信息在配置的PS业务帧类型 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_DEC_TEST_PS_FRM_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_DEC_START_UNION
 联合说明  : GTC_DEC_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dec_start                           : 1;      /* NB承载业务的译码启动信号,GTC逻辑自清零 */
        unsigned short gsp_last_dec_flag                       : 1;      /* 1表示本次译码需要上报中断，该译码中断模式只在gsp_cycle_rpt_en为0才生效 */
        unsigned short gsp_dec_slot                            : 3;      /* 本次启动译码时对应的时隙号 */
        unsigned short                                         : 1;      /* reserve */
        unsigned short gsp_bler_clr                            : 1;      /* 接收质量上报BLER清零信号，脉冲信号,GTC逻辑自清零 */
        unsigned short gsp_ber_clr                             : 1;      /* 接收质量上报BER清零信号，脉冲信号,GTC逻辑自清零 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_DEC_START_UNION;


/*--------------------------------------------------------------------------------------------*
 * CPU模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_CPU_AMR_CB_INDEX_UNION
 联合说明  : CPU_AMR_CB_INDEX寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_amr0_cb_index                       : 7;      /* AMR语音激活集合内codec_mode=00对应的AMR语音帧cb_index */
        unsigned long                                         : 1;      /* reserve */
        unsigned long cpu_amr1_cb_index                       : 7;      /* AMR语音激活集合内codec_mode=01对应的AMR语音帧cb_index */
        unsigned long                                         : 1;      /* reserve */
        unsigned long cpu_amr2_cb_index                       : 7;      /* AMR语音激活集合内codec_mode=10对应的AMR语音帧cb_index */
        unsigned long                                         : 1;      /* reserve */
        unsigned long cpu_amr3_cb_index                       : 7;      /* AMR语音激活集合内codec_mode=11对应的AMR语音帧cb_index */
        unsigned long                                         : 1;      /* reserve */
    }stReg;
}HAL_GBBP_CPU_AMR_CB_INDEX_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_AMR_ACTIVE_SET_NUM_UNION
 联合说明  : CPU_AMR_ACTIVE_SET_NUM寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_amr_active_set_num                  : 2;      /* AMR语音激活集内有效语音帧类型个数 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long cpu_speech_bfi_thres                    : 4;      /* 语音帧BFI相关译码判决门限，配置范围0～8 */
        unsigned long                                         : 8;      /* reserve */
        unsigned long cpu_amr_active_set_num_upflag           : 1;      /* 当AMR激活集个数发生变化时，需要配置该信号为1 */
        unsigned long                                         : 15;     /* reserve */
    }stReg;
}HAL_GBBP_CPU_AMR_ACTIVE_SET_NUM_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CMI_P01_PRIOR_PRE_RPT_UNION
 联合说明  : CMI_P01_PRIOR_PRE_RPT寄存器定义
*****************************************************************************/

typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long mappre_0_cmi                            : 16;     /* CMI带内bit 0对应的先验概率上报 */
        unsigned long mappre_1_cmi                            : 16;     /* CMI带内bit 1对应的先验概率上报 */
    }stReg;
}HAL_GBBP_CMI_P01_PRIOR_PRE_RPT_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CMI_P23_PRIOR_PRE_RPT_UNION
 联合说明  : CMI_P23_PRIOR_PRE_RPT寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long mappre_2_cmi                            : 16;     /* CMI带内bit 2对应的先验概率上报 */
        unsigned long mappre_3_cmi                            : 16;     /* CMI带内bit 3对应的先验概率上报 */
    }stReg;
}HAL_GBBP_CMI_P23_PRIOR_PRE_RPT_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_CMC_P01_PRIOR_PRE_UNION
 联合说明  : CMC_P01_PRIOR_PRE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long mappre_0_cmc                            : 16;     /* CMC带内bit 0对应的先验概率上报 */
        unsigned long mappre_1_cmc                            : 16;     /* CMC带内bit 1对应的先验概率上报 */
    }stReg;
}HAL_GBBP_CMC_P01_PRIOR_PRE_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CMC_P23_PRIOR_PRE_UNION
 联合说明  : CMC_P23_PRIOR_PRE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long mappre_2_cmc                            : 16;     /* CMC带内bit 2对应的先验概率上报 */
        unsigned long mappre_3_cmc                            : 16;     /* CMC带内bit 3对应的先验概率上报 */
    }stReg;
}HAL_GBBP_CMC_P23_PRIOR_PRE_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CMC_P23_PRIOR_PRE_UNION
 联合说明  : CMC_P23_PRIOR_PRE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_cmi_cmc_mappre_load_en              : 1;      /* 1标识需要做加载CMI或者CMC的历史先验概率，该信号配置之前需要软件先配置CPU_P01_MAPPRE_CFG，CPU_P23_MAPPRE_CFG寄存器 */
        unsigned long cpu_cmi_cmc_mappre_load_mode            : 1;      /* 0标识加载CMI的历史先验概率，1标识加载CMC的历史先验概率 */
        unsigned long cpu_nodata_judge_mode                   : 1;      /* 1标识采用C02通过状态来判决NODATA的模式 */
        unsigned long                                         : 5;      /* reserve */
        unsigned long dec_cmc_cmi_rpt                         : 1;      /* 1标识当前译码中断所在周期是CMC周期 */
        unsigned long                                         : 7;      /* reserve */
        unsigned long cpu_amr_corr_thres                      : 14;     /* AMR判决算法中绝对门限，AFS推荐值为233，AHS推荐值为230 */
        unsigned long                                         : 2;      /* reserve */
    }stReg;
}HAL_GBBP_CPU_CMC_CMI_LOAD_EN_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_P01_MAPPRE_CFG_UNION
 联合说明  : CPU_P01_MAPPRE_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_p0_mappre                           : 16;     /* CMC */
        unsigned long cpu_p1_mappre                           : 16;     /* CMC */
    }stReg;
}HAL_GBBP_CPU_P01_MAPPRE_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_CPU_P23_MAPPRE_CFG_UNION
 联合说明  : CPU_P23_MAPPRE_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;
    struct
    {
        unsigned long cpu_p2_mappre                           : 16;     /* CMC */
        unsigned long cpu_p3_mappre                           : 16;     /* CMC */
    }stReg;
}HAL_GBBP_CPU_P23_MAPPRE_CFG_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.13 上行信道编码模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_UL_CB1_INDEX_UNION
 联合说明  : UL_CB1_INDEX寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_cb1_index                        : 7;      /* 上行码块业务索引号 */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_UL_CB1_INDEX_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_UL_CB_NUM_UNION
 联合说明  : GTC_UL_CB_NUM寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short                                         : 4;      /* reserve */
        unsigned short gsp_ul_cb_start_pos                     : 2;      /* 上行待编码码块起始位置指示标志 */
        unsigned short gsp_facch_stolen_flag                   : 2;      /* FACCH偷帧标志 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_UL_CB_NUM_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_IC_PUNC_INF_UNION
 联合说明  : GTC_IC_PUNC_INF寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_in_band_data_id0                 : 2;      /* [1:0]：表示带内比特id0信息 */
        unsigned short gsp_ul_in_band_data_id1                 : 2;      /* [1:0]：表示带内比特id0信息 */
        unsigned short gsp_punc_index_data1                    : 2;      /* [5:4]表示DATA1（DATA）的打孔方式； */
        unsigned short gsp_punc_index_data2                    : 2;      /* [5:4]表示DATA1（DATA）的打孔方式； */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_IC_PUNC_INF_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_RACH_BSIC_UNION
 联合说明  : GTC_RACH_BSIC寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_rach_bsic_data                      : 6;      /* RACH业务中BS（基站）的BSIC比特 */
        unsigned short                                         : 10;     /* reserve */
    }stReg;
}HAL_GTC_RACH_BSIC_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_COD_BURST_POS_UNION
 联合说明  : COD_BURST_POS寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_cod_burst_pos                       : 7;      /* 表示当前编码数据第一个BURST在编码存储RAM（ram611x16s）中的位置编号: */
        unsigned short                                         : 9;      /* reserve */
    }stReg;
}HAL_GTC_COD_BURST_POS_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_UL_BURST_CYCLE_UNION
 联合说明  : GSP_UL_BURST_CYCLE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_burst_sav_cycle                  : 6;      /* 上行编码存放BURST循环周期（该参数只对CS域业务有效，即是CS域业务映射的复帧周期）. */
        unsigned short                                         : 1;      /* reserve */
        unsigned short gsp_ul_cod_en                           : 1;      /* 上行编码过程的使能信号 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_UL_BURST_CYCLE_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_UL_COD_START_UNION
 联合说明  : GSP_UL_COD_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_cod_start                        : 1;      /* 上行编码启动信号，脉冲信号,GTC逻辑自清零 */
        unsigned short                                         : 15;     /* reserve */
    }stReg;
}HAL_GTC_GSP_UL_COD_START_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_UL_MAP_PARA_UNION
 联合说明  : UL_MAP_PARA寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_mod_type                         : 1;      /* 调制类型标志： */
        unsigned short gsp_ul_burst_type                       : 1;      /* BURST类型标志： */
        unsigned short gsp_ul_tsc_group_num                    : 4;      /* 上行NB的训练序列组号 */
        unsigned short gsp_demod_loop_mode_en                  : 1;      /* 1表示是下行解调环回测试模式，0表示是正常的上行调制和下行解调工作模式 */
        unsigned short gsp_cpu_wrte_mod                        : 1;      /* 1 表示由CPU来控制调制前RAM数据的写入 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_UL_MAP_PARA_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_MAP_BURST_POS_UNION
 联合说明  : MAP_BURST_POS寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_map_burst_pos                       : 7;      /* 需要送去成BURST的数据存储位置指示（可选集为编码RAM中每个BURST存储的起始地址，参考gsp_ul_cod_burst_pos[6:0]的位置编号方式，所不同的是：该信号下发的是当前准备调制的BURST位置编号，而不是整个编码码块起始BURST的位置编号。） */
        unsigned short gsp_map_ram_switch_en                   : 1;      /* 1表示使能MAP后RAM(调制模块数据输入RAM)乒乓切换使能 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_MAP_BURST_POS_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_MAP_START_UNION
 联合说明  : MAP_START寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_map_start                        : 1;      /* 成BURST映射启动信号，高电平脉冲信号（提前调制启动信号6.25个Symble）,GTC逻辑自清零 */
        unsigned short                                         : 15;     /* reserve */
    }stReg;
}HAL_GTC_MAP_START_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.14 上行调制模块
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * GTC部分
 *--------------------------------------------------------------------------------------------*/
/*****************************************************************************
 联合名    : HAL_GTC_MOD_START_POS_UNION
 联合说明  : GTC_MOD_START_POS寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mod_start_pos                       : 7;      /* bit6~0: 该信号表示gsp_mod_start需要在QB内产生位置信息(即是在QB内48拍哪一拍的位置产生) */
        unsigned short gsp_mod_symbol_h_length                 : 1;      /* 与gsp_mod_symbol_l_length[7:0]组成9bit的调制长度，该比特为最高位，gsp_mod_symbol_l_length[7:0]为低8bit */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_MOD_START_POS_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_MOD_SYMBOL_LENGTH_UNION
 联合说明  : GTC_MOD_SYMBOL_LENGTH寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mod_symbol_l_length                 : 8;      /* 调制符号个数(在1个burst调制期间是常数，范围是0～148*2)，当调制时，符号计数大于gsp_mod_symbol_length时，调制模块停止输出 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_MOD_SYMBOL_LENGTH_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_GSP_MOD_TYPE_UNION
 联合说明  : GSP_MOD_TYPE寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mod_type                            : 1;      /* 调制类型： */
        unsigned short                                         : 15;     /* reserve */
    }stReg;
}HAL_GTC_GSP_MOD_TYPE_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_MOD_CTRL_UNION
 联合说明  : GTC_GSP_MOD_CTRL寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_mod_start                           : 1;      /* bit0: CPU配置GTC启动调制，一个脉冲，连续多时隙时，CPU仍需要每个时隙都启动一次，调制时，逻辑不关心是否是多时隙. */
        unsigned short gsp_mod_stop                            : 1;      /* bit1: CPU 配置GTC停止调制，一个脉冲。 */
        unsigned short                                         : 14;     /* reserve */
    }stReg;
}HAL_GTC_GSP_MOD_CTRL_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_ULMOD_TEST_MOD_LOW_UNION
 联合说明  : GSP_ULMOD_TEST_MOD_LOW寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ulmod_test_mod_low                  : 8;      /* bit[0]:测试使能。0为原始设计模式，1为测试模式 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_GSP_ULMOD_TEST_MOD_LOW_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_GSP_ULMOD_TEST_MOD_HIGH_UNION
 联合说明  : GSP_ULMOD_TEST_MOD_HIGH寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ulmod_test_mod_high                 : 4;      /* bit[3:0]:预调制长度，0表示不进行预调制，默认值为4，调制过程中，该参数应保持 */
        unsigned short                                         : 12;     /* reserve */
    }stReg;
}HAL_GTC_GSP_ULMOD_TEST_MOD_HIGH_UNION;



/*--------------------------------------------------------------------------------------------*
 * CPU部分
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_TX_IQ_MISMATCH_COMP_UNION
 联合说明  : G_TX_IQ_MISMATCH_COMP寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long dsp_g_tx_amp_comp        : 7;                                /* G TX IQmismatch幅度校正因子，有符号数。 */
        unsigned long reserved0                : 9;
        unsigned long dsp_g_tx_phase_comp      : 7;                                /* G TX IQmismatch相位校正因子，有符号数。 */
        unsigned long reserved1                : 9;
    }stReg;
}HAL_GBBP_TX_IQ_MISMATCH_COMP_UNION;

/*****************************************************************************
 联合名    : HAL_GBBP_TX_IQ_DC_OFFSET_COMP_UNION
 联合说明  : G_TX_IQ_DC_OFFSET_COMP1寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long dsp_g_tx_dc_offset_i1    : 12;                                /* G TX IQmismatch I路直流偏置补偿值，有符号数 */
        unsigned long reserved0                : 4;
        unsigned long dsp_g_tx_dc_offset_q1    : 12;                                /* G TX IQmismatch Q路直流偏置补偿值，有符号数 */
        unsigned long reserved1                : 4;
    }stReg;
}HAL_GBBP_TX_IQ_DC_OFFSET_COMP_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_TX_DIG_RMAP_MODE_SEL_UNION
 联合说明  : TX_DIG_RMAP_MODE_SEL寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_gmsk_digramp_en      : 1;                                /* Gmsk数字爬坡 */
        unsigned long                          : 7;
        unsigned long cpu_8psk_digramp_en      : 1;                                /* G TX IQmismatch Q路直流偏置补偿值，有符号数 */
        unsigned long                          : 7;
        unsigned long cpu_ramp_default_value   : 10;                               /* 数字爬坡时，下坡结束后的apc_ramp默认值 */
        unsigned long                          : 6;
    }stReg;
}HAL_GBBP_TX_DIG_RMAP_MODE_SEL_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_TX_DATA_MULFACTOR_UNION
 联合说明  : TX_DATA_MULFACTOR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long cpu_gmsk_tx_data_mulfactor : 10;                             /* GMSK上行调制数据缩放因子 */
        unsigned long                            : 6;
        unsigned long cpu_psk_tx_data_mulfactor  : 10;                             /* 8PSK上行调制数据缩放因子 */
        unsigned long                            : 6;
    }stReg;
}HAL_GBBP_TX_DATA_MULFACTOR_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.15 DRX模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GBBP_MSR_CNF_UNION
 联合说明  : MSR_CNF寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gdrx_msr_frac                           : 11;     /* 睡眠计数小数部分，该计数器的进位表示1个104M时钟。 */
        unsigned long                                         : 5;      /* reserve */
        unsigned long gdrx_msr_integer                        : 7;      /* 睡眠计数整数部分，少于96的104MHz时钟个数。 */
        unsigned long                                         : 1;      /* reserve */
        unsigned long gdrx_msr_timebase                       : 6;      /* bit[29:24]睡眠计数96倍部分（指示1个32K有多少个96个104M钟的倍数）。 */
        unsigned long                                         : 2;      /* reserve */
    }stReg;
}HAL_GBBP_MSR_CNF_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_GAUGE_RESULT_RPT_UNION
 联合说明  : GAUGE_RESULT_RPT寄存器定义
*****************************************************************************/

typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gdrx_gauge_result                       : 28;     /* bit[27:0] 时钟校准结果，测量周期内104钟个数 */
        unsigned long                                         : 2;      /* reserve */
        unsigned long gdrx_gauge_state                        : 1;      /* bit[30] 测量状态查询。1，正在测量。 */
        unsigned long gdrx_sleep_state                        : 1;      /* bit[31] 睡眠状态查询。1，正在睡眠。 */
    }stReg;
}HAL_GBBP_GAUGE_RESULT_RPT_UNION;


/*****************************************************************************
 联合名    : HAL_GBBP_GAUGE_EN_CNF_UNION
 联合说明  : GAUGE_EN_CNF寄存器定义
*****************************************************************************/
typedef union
{
    unsigned long uwValue;

    struct
    {
        unsigned long gdrx_gauge_len                          : 17;     /* 时钟校准32KHz时钟个数 */
        unsigned long                                         : 13;     /* reserve */
        unsigned long gdrx_gauge_en                           : 1;      /* bit[0] 时钟校准使能，写1开启，1个52M脉冲。自清零，写0无意义。 */
        unsigned long gdrx_gauge_stop                         : 1;      /* bit[1] GDRX停止脉冲，写1停止，1个52M脉冲。自清零，写0无意义。 */
    }stReg;
}HAL_GBBP_GAUGE_EN_CNF_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.16 加解密模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_A5_DECIPH_CFG_UNION
 联合说明  : A5_DECIPH_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_dl_deciph_en                        : 1;      /* 下行解密模块工作模式选择 */
        unsigned short gsp_dl_gsm_alg_sel                      : 3;      /* 下行A51或者A52,A53算法选择 */
        unsigned short gsp_dl_fn_adj_flag                      : 2;      /* 下行帧号调整标志 */
        unsigned short                                         : 10;     /* reserve */
    }stReg;
}HAL_GTC_A5_DECIPH_CFG_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_A5_CIPH_CFG_UNION
 联合说明  : A5_CIPH_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_ul_ciph_en                          : 1;      /* 上行解密模块工作模式选择 */
        unsigned short gsp_ul_gsm_alg_sel                      : 3;      /* 上行行A51或者A52,A53算法选择 */
        unsigned short gsp_ul_fn_adj_flag                      : 2;      /* 上行帧号调整标志 */
        unsigned short                                         : 1;      /* reserve */
        unsigned short gsp_ul_a5_ciph_start                    : 1;      /* 1表示启动上行A5加密。每次有加密操作该信号必须配置。 */
        unsigned short                                         : 8;      /* reserve */
    }stReg;
}HAL_GTC_A5_CIPH_CFG_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.17 GAPC模块
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_PA_CTRL_CFG_UNION
 联合说明  : PA_CTRL_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gsp_pa_start                            : 1;      /* GSP启动APC驱动，1个52M高脉冲（必需在frac_cnt=46处发出（GTC内部处理））可用该启动信号来配置APC输出RAMP DAC所需的控制字 */
        unsigned short gsp_pa_stop                             : 1;      /* GSP停止APC爬坡驱动的信号。写1停止APC爬坡驱动，内部产生一个52M脉冲。可用该启动信号来配置APC输出RAMP DAC所需的控制字 */
        unsigned short gsp_pa_single_start                     : 1;      /* 单次APC驱动启动信号。写1启动目标功率电压值为gsp_pa_init的单次APC驱动，内部产生一个52M脉冲。（逻辑内部一次达到该目标功率电压值），可用该启动信号来配置APC输出TXAGC DAC所需的控制字 */
        unsigned short gsp_pa_change                           : 1;      /* target重配置APC爬坡驱动启动信号。写1启动一次target重配置的APC爬坡驱动（应用于多时隙功率变化），内部产生一个52M脉冲（必需在frac_cnt=46处发出（GTC内部处理））。 */
        unsigned short gsp_pa_dac_sel                          : 1;      /* 上行ABB内部GAPC DAC控制选择信号。 */
        unsigned short                                         : 11;     /* reserve */
    }stReg;
}HAL_GTC_PA_CTRL_CFG_UNION;

/*****************************************************************************
 联合名    : HAL_GTC_PA_LEN_CFG_UNION
 联合说明  : PA_LEN_HIGH_CFG&PA_LEN_LOW_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;

    struct
    {
        unsigned short low_byte                                : 8;
        unsigned short high_byte                               : 8;
    }stWord;

    struct
    {
        unsigned short gsp_pa_len                              : 13;      /* APC爬坡驱动持续时间长度，具体指首次上爬坡到目标值gsp_pa_target时开始，到开始下爬坡到初始值的时刻这段时间（具体参见下面时序图该信号的含义）。单位Qb，默认值为4个burst长度。 */
        unsigned short                                         : 3;     /* reserve */
    }stReg;
}HAL_GTC_PA_LEN_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_PA_TARGET_CFG_UNION
 联合说明  : PA_TARGET_HIGH_CFG&PA_TARGET_LOW_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;

    struct
    {
        unsigned short low_byte                                : 8;
        unsigned short high_byte                               : 8;
    }stWord;

    struct
    {
        unsigned short gsp_pa_target                           : 10;      /* GSP配置APC目标电压值。 */
        unsigned short                                         : 3;      /* reserve */
        unsigned short gsp_pa_coeff_index                      : 2;      /* 指示本次爬坡的爬坡系数索引号，确保在gsp_pa_start或者gsp_pa_change有效时,此值已经配置。本索引号取值范围是0～3 */
        unsigned short                                         : 1;      /* reserve */
    }stReg;
}HAL_GTC_PA_TARGET_CFG_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_PA_INIT_CFG_UNION
 联合说明  : PA_INIT_LOW_CFG&PA_INIT_HIGH_CFG寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;

    struct
    {
        unsigned short low_byte                                : 8;
        unsigned short high_byte                               : 8;
    }stWord;


    struct
    {
        unsigned short gsp_pa_init                             : 10;      /* APC爬坡初始功率电压值。也可作为单次APC驱动的目标功率电压值。 */
        unsigned short gsp_ramp_point                          : 5;      /* 指示当前爬坡的点数，上坡和下坡点数相同。 */
        unsigned short                                         : 1;      /* reserve */
    }stReg;
}HAL_GTC_PA_INIT_CFG_UNION;




/*--------------------------------------------------------------------------------------------*
 * 1.18 双模切换
 *--------------------------------------------------------------------------------------------*/

/*****************************************************************************
 联合名    : HAL_GTC_G2W_CTRL_ADDR_UNION
 联合说明  : G2W_CTRL_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gtc_want_switch                         : 1;      /* 切换W天线开关，52M脉冲信号 */
        unsigned short gtc_wrf_on                              : 1;      /* 打开W RF下行、ABB，52M脉冲信号 */
        unsigned short gtc_wpro_on                             : 1;      /* 指示W射频已稳定，可处理小区搜索或能量测量，52M脉冲信号 */
        unsigned short gtc_wrf_spi                             : 1;      /* 启动W SPI写spi_data1使能，52M脉冲信号 */
        unsigned short gtc_woff                                : 1;      /* 关闭W ABB、RF，切换天线开关，结束W测量，52M脉冲信号 */
        unsigned short reserve                                 : 11;     /* reserve */
    }stReg;
}HAL_GTC_G2W_CTRL_ADDR_UNION;


/*****************************************************************************
 联合名    : HAL_GTC_W2G_CTRL_ADDR_UNION
 联合说明  : W2G_CTRL_ADDR寄存器定义
*****************************************************************************/
typedef union
{
    unsigned short uhwValue;
    struct
    {
        unsigned short gtc_g2g_switch                          : 1;      /* 切换到G天线，切换到GTC内部寄存器27下的选择5，2M脉冲信号 */
        unsigned short reserved                                : 15;     /* reserve */
    }stReg;
}HAL_GTC_W2G_CTRL_ADDR_UNION;


/*--------------------------------------------------------------------------------------------*
 * 1.19 环回测试模式
 *--------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------*
 * 1.20 测试管脚控制模块
 *--------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------*
 * 1.21 测试寄存器模块
 *--------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------*
 * 1.22 GBBP版本寄存器
 *--------------------------------------------------------------------------------------------*/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/




/*****************************************************************************
  10 函数声明
*****************************************************************************/











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif /*  */


