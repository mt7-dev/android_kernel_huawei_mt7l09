/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_ctu.h */
/* Version       : 2.0 */
/* Created       : 2013-09-23*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_ctu*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_CTU_H__
#define __HI_BBP_CTU_H__

/*
 * Project: hi
 * Module : bbp_ctu
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_ctu 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_CTU_C0_PRIMARY_MODE_IND_OFFSET             (0x0) /* 通道0对应主模（主模0）的模式指示寄存器 */
#define    HI_BBP_CTU_C0_SECOND_MODE_IND_OFFSET              (0x4) /* 主模0对应的从模（从模0）的模式指示寄存器 */
#define    HI_BBP_CTU_AFC0_VALUE_INTER_RAT_OFFSET            (0xC) /* 寄存器 */
#define    HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_OFFSET          (0x10) /* 寄存器 */
#define    HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_OFFSET    (0x14) /* 寄存器 */
#define    HI_BBP_CTU_C0_RFIC_SELECT_MASK_OFFSET             (0x18) /* 寄存器 */
#define    HI_BBP_CTU_C0_MIPI_SELECT_MASK_OFFSET             (0x1C) /* 寄存器 */
#define    HI_BBP_CTU_C0_ABB_SELECT_MASK_OFFSET              (0x20) /* 寄存器 */
#define    HI_BBP_CTU_C0_PMU_SELECT_MASK_OFFSET              (0x24) /* 寄存器 */
#define    HI_BBP_CTU_DSP_C0_SEL_MASK_OFFSET                 (0x28) /* 寄存器 */
#define    HI_BBP_CTU_DSP_RFIC0_RSTN_OFFSET                  (0x2C) /* 寄存器 */
#define    HI_BBP_CTU_C0_SECOND_AFC_MODE_OFFSET              (0x30) /* 寄存器 */
#define    HI_BBP_CTU_BBP_SYS_0CONTROL_OFFSET                (0x34) /* 寄存器 */
#define    HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_OFFSET      (0x38) /* 寄存器 */
#define    HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_OFFSET       (0x3C) /* 寄存器 */
#define    HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_OFFSET           (0x40) /* 寄存器 */
#define    HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_OFFSET            (0x44) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_1_OFFSET               (0x48) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_2_OFFSET               (0x4C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_3_OFFSET               (0x50) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_4_OFFSET               (0x54) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_5_OFFSET               (0x58) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_6_OFFSET               (0x5C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_7_OFFSET               (0x60) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_8_OFFSET               (0x64) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_9_OFFSET               (0x68) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_10_OFFSET              (0x6C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_11_OFFSET              (0x70) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_12_OFFSET              (0x74) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_13_OFFSET              (0x78) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_14_OFFSET              (0x7C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_15_OFFSET              (0x80) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC0_16_OFFSET              (0x84) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_OFFSET            (0x88) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_OFFSET        (0x8C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_1_OFFSET               (0x90) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_2_OFFSET               (0x94) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_3_OFFSET               (0x98) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_4_OFFSET               (0x9C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_5_OFFSET               (0xA0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_6_OFFSET               (0xA4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_7_OFFSET               (0xA8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_8_OFFSET               (0xAC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_9_OFFSET               (0xB0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_10_OFFSET              (0xB4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_11_OFFSET              (0xB8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_12_OFFSET              (0xBC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_13_OFFSET              (0xC0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_14_OFFSET              (0xC4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_15_OFFSET              (0xC8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI0_16_OFFSET              (0xCC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_OFFSET            (0xD0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_OFFSET        (0xD4) /* 寄存器 */
#define    HI_BBP_CTU_MIPI0_RD_DATA_LOW_SOFT_OFFSET          (0xD8) /* 寄存器 */
#define    HI_BBP_CTU_MIPI0_RD_DATA_HIGH_SOFT_OFFSET         (0xDC) /* 寄存器 */
#define    HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_OFFSET          (0xE0) /* 寄存器 */
#define    HI_BBP_CTU_MIPI0_GRANT_DSP_OFFSET                 (0xE4) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI0_WDATA_LOW_OFFSET             (0xE8) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI0_WDATA_HIGH_OFFSET            (0xEC) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI0_EN_IMI_OFFSET                (0xF0) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_OFFSET           (0xF4) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI0_RD_CLR_OFFSET                (0xF8) /* 寄存器 */
#define    HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET           (0xFC) /* 寄存器 */
#define    HI_BBP_CTU_CH0_BBP_SEL_OFFSET                     (0x100)
#define    HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_OFFSET              (0x104)
#define    HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_OFFSET             (0x108)
#define    HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_OFFSET             (0x10C)
#define    HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_OFFSET            (0x110)
#define    HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_OFFSET           (0x114)
#define    HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_OFFSET          (0x118)
#define    HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_OFFSET           (0x11C)
#define    HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET          (0x120)
#define    HI_BBP_CTU_C1_PRIMARY_MODE_IND_OFFSET             (0x300) /* 通道1对应主模（主模1）的模式指示寄存器 */
#define    HI_BBP_CTU_C1_SECOND_MODE_IND_OFFSET              (0x304) /* 主模1对应的从模（从模1）的模式指示寄存器 */
#define    HI_BBP_CTU_C1_CH_NUM_IND_OFFSET                   (0x308) /* 从模1对应通道的指示寄存器 */
#define    HI_BBP_CTU_AFC1_VALUE_INTER_RAT_OFFSET            (0x30C) /* 寄存器 */
#define    HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_OFFSET          (0x310) /* 寄存器 */
#define    HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_OFFSET    (0x314) /* 寄存器 */
#define    HI_BBP_CTU_C1_RFIC_SELECT_MASK_OFFSET             (0x318) /* 寄存器 */
#define    HI_BBP_CTU_C1_MIPI_SELECT_MASK_OFFSET             (0x31C) /* 寄存器 */
#define    HI_BBP_CTU_C1_ABB_SELECT_MASK_OFFSET              (0x320) /* 寄存器 */
#define    HI_BBP_CTU_C1_PMU_SELECT_MASK_OFFSET              (0x324) /* 寄存器 */
#define    HI_BBP_CTU_DSP_C1_SEL_MASK_OFFSET                 (0x328) /* 寄存器 */
#define    HI_BBP_CTU_DSP_RFIC1_RSTN_OFFSET                  (0x32C) /* 寄存器 */
#define    HI_BBP_CTU_C1_SECOND_AFC_MODE_OFFSET              (0x330) /* 寄存器 */
#define    HI_BBP_CTU_BBP_SYS_1CONTROL_OFFSET                (0x334) /* 寄存器 */
#define    HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_OFFSET      (0x338) /* 寄存器 */
#define    HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_OFFSET       (0x33C) /* 寄存器 */
#define    HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_OFFSET           (0x340) /* 寄存器 */
#define    HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_OFFSET            (0x344) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_1_OFFSET               (0x348) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_2_OFFSET               (0x34C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_3_OFFSET               (0x350) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_4_OFFSET               (0x354) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_5_OFFSET               (0x358) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_6_OFFSET               (0x35C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_7_OFFSET               (0x360) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_8_OFFSET               (0x364) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_9_OFFSET               (0x368) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_10_OFFSET              (0x36C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_11_OFFSET              (0x370) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_12_OFFSET              (0x374) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_13_OFFSET              (0x378) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_14_OFFSET              (0x37C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_15_OFFSET              (0x380) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_RFIC1_16_OFFSET              (0x384) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_OFFSET            (0x388) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_OFFSET        (0x38C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_1_OFFSET               (0x390) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_2_OFFSET               (0x394) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_3_OFFSET               (0x398) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_4_OFFSET               (0x39C) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_5_OFFSET               (0x3A0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_6_OFFSET               (0x3A4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_7_OFFSET               (0x3A8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_8_OFFSET               (0x3AC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_9_OFFSET               (0x3B0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_10_OFFSET              (0x3B4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_11_OFFSET              (0x3B8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_12_OFFSET              (0x3BC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_13_OFFSET              (0x3C0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_14_OFFSET              (0x3C4) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_15_OFFSET              (0x3C8) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_MIPI1_16_OFFSET              (0x3CC) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_OFFSET            (0x3D0) /* 寄存器 */
#define    HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_OFFSET        (0x3D4) /* 寄存器 */
#define    HI_BBP_CTU_MIPI1_RD_DATA_LOW_SOFT_OFFSET          (0x3D8) /* 寄存器 */
#define    HI_BBP_CTU_MIPI1_RD_DATA_HIGH_SOFT_OFFSET         (0x3DC) /* 寄存器 */
#define    HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_OFFSET          (0x3E0) /* 寄存器 */
#define    HI_BBP_CTU_MIPI1_GRANT_DSP_OFFSET                 (0x3E4) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI1_WDATA_LOW_OFFSET             (0x3E8) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI1_WDATA_HIGH_OFFSET            (0x3EC) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI1_EN_IMI_OFFSET                (0x3F0) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_OFFSET           (0x3F4) /* 寄存器 */
#define    HI_BBP_CTU_DSP_MIPI1_RD_CLR_OFFSET                (0x3F8) /* 寄存器 */
#define    HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET           (0x3FC) /* 寄存器 */
#define    HI_BBP_CTU_CH1_BBP_SEL_OFFSET                     (0x400)
#define    HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_OFFSET              (0x404)
#define    HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_OFFSET             (0x408)
#define    HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_OFFSET             (0x40C)
#define    HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_OFFSET            (0x410)
#define    HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_OFFSET           (0x414)
#define    HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_OFFSET          (0x418)
#define    HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_OFFSET           (0x41C)
#define    HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET          (0x420)
#define    HI_BBP_CTU_DELAY_CNT_OFFSET                       (0x618) /* 定时锁存延迟寄存器 */
#define    HI_BBP_CTU_INT_WAIT_CNT_OFFSET                    (0x61C) /* 中断延迟寄存器 */
#define    HI_BBP_CTU_LTEBBP_INPUT_MASK_OFFSET               (0x620) /* 寄存器 */
#define    HI_BBP_CTU_WBBP_INPUT_MASK_OFFSET                 (0x624) /* 寄存器 */
#define    HI_BBP_CTU_GMBBP_INPUT_MASK_OFFSET                (0x628) /* 寄存器 */
#define    HI_BBP_CTU_GSBBP_INPUT_MASK_OFFSET                (0x62C) /* 寄存器 */
#define    HI_BBP_CTU_TDSBBP_INPUT_MASK_OFFSET               (0x630) /* 寄存器 */
#define    HI_BBP_CTU_LTE_MUX_FRAME_NUM_OFFSET               (0x634) /* 寄存器 */
#define    HI_BBP_CTU_TDS_MUX_FRAME_NUM_OFFSET               (0x638) /* 寄存器 */
#define    HI_BBP_CTU_W_MUX_FRAME_NUM_OFFSET                 (0x63C) /* 寄存器 */
#define    HI_BBP_CTU_MEAS_FLAG_L_RD_OFFSET                  (0x640) /* 寄存器 */
#define    HI_BBP_CTU_MEAS_FLAG_W_RD_OFFSET                  (0x644) /* 寄存器 */
#define    HI_BBP_CTU_MEAS_FLAG_T_RD_OFFSET                  (0x648) /* 寄存器 */
#define    HI_BBP_CTU_MEAS_FLAG_GM_RD_OFFSET                 (0x64C) /* 寄存器 */
#define    HI_BBP_CTU_MEAS_FLAG_GS_RD_OFFSET                 (0x650) /* 寄存器 */
#define    HI_BBP_CTU_WAKE_FLAG_L_WR_OFFSET                  (0x654) /* 寄存器 */
#define    HI_BBP_CTU_WAKE_FLAG_W_WR_OFFSET                  (0x658) /* 寄存器 */
#define    HI_BBP_CTU_WAKE_FLAG_T_WR_OFFSET                  (0x65C) /* 寄存器 */
#define    HI_BBP_CTU_WAKE_FLAG_GM_WR_OFFSET                 (0x660) /* 寄存器 */
#define    HI_BBP_CTU_WAKE_FLAG_GS_WR_OFFSET                 (0x664) /* 寄存器 */
#define    HI_BBP_CTU_TIMING_GET_IND_OFFSET                  (0x610) /* 定时锁存使能指示寄存器 */
#define    HI_BBP_CTU_TIMING_CLEAR_OFFSET                    (0x614) /* 定时标志清除寄存器 */
#define    HI_BBP_CTU_TIMING_VALID_OFFSET                    (0x668) /* 寄存器 */
#define    HI_BBP_CTU_W_TIMING_DSP_OFFSET                    (0x66C) /* 寄存器 */
#define    HI_BBP_CTU_GM_FN_DSP_OFFSET                       (0x670) /* 寄存器 */
#define    HI_BBP_CTU_GM_QB_DSP_OFFSET                       (0x674) /* 寄存器 */
#define    HI_BBP_CTU_GS_FN_DSP_OFFSET                       (0x678) /* 寄存器 */
#define    HI_BBP_CTU_GS_QB_DSP_OFFSET                       (0x67C) /* 寄存器 */
#define    HI_BBP_CTU_LTE_TIMING_DSP_OFFSET                  (0x680) /* 寄存器 */
#define    HI_BBP_CTU_TDS_TIMING1_DSP_OFFSET                 (0x684) /* 寄存器 */
#define    HI_BBP_CTU_TDS_TIMING2_DSP_OFFSET                 (0x688) /* 寄存器 */
#define    HI_BBP_CTU_CTU_TIMING_DSP_OFFSET                  (0x68C) /* 寄存器 */
#define    HI_BBP_CTU_CTU_BASECNT1_DSP_OFFSET                (0x690) /* 寄存器 */
#define    HI_BBP_CTU_CTU_BASECNT2_DSP_OFFSET                (0x694) /* 寄存器 */
#define    HI_BBP_CTU_CTU_BASECNT_DSP_OFFSET                 (0x6B8) /* 寄存器 */
#define    HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_OFFSET          (0x6BC) /* 寄存器 */
#define    HI_BBP_CTU_CTU_TIMING_VALID_DSP_OFFSET            (0x6C0) /* 寄存器 */
#define    HI_BBP_CTU_DSP_CTU_TIMING_CLR_OFFSET              (0x6C4) /* 寄存器 */
#define    HI_BBP_CTU_W_INTRASYS_VALID_OFFSET                (0x698) /* 寄存器 */
#define    HI_BBP_CTU_GM_INTRASYS_VALID_OFFSET               (0x69C) /* 寄存器 */
#define    HI_BBP_CTU_GS_INTRASYS_VALID_OFFSET               (0x6A0) /* 寄存器 */
#define    HI_BBP_CTU_LTE_INTRASYS_VALID_OFFSET              (0x6A4) /* 寄存器 */
#define    HI_BBP_CTU_TDS_INTRASYS_VALID_OFFSET              (0x6A8) /* 寄存器 */
#define    HI_BBP_CTU_RFIC_CK_CFG_OFFSET                     (0x6AC) /* 寄存器 */
#define    HI_BBP_CTU_DSP_WAITTIME_OFFSET                    (0x6B4) /* 寄存器 */
#define    HI_BBP_CTU_S_W_CH_NUM_IND_OFFSET                  (0xA40) /* W为从模时对应通道的指示寄存器 */
#define    HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_OFFSET         (0x6C8) /* 寄存器 */
#define    HI_BBP_CTU_W_MEASURE_REPORT_VALID_OFFSET          (0x6CC) /* 寄存器 */
#define    HI_BBP_CTU_W_PREINT_OFFSET_TIME_OFFSET            (0x6D0) /* 寄存器 */
#define    HI_BBP_CTU_W_INTERSYS_MEASURE_TIME_OFFSET         (0x6D4) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_OFFSET             (0x6D8) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_OFFSET             (0x6DC) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_RCV_GM_OFFSET              (0x6E0) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_RCV_GS_OFFSET              (0x6E4) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_RCV_W_OFFSET               (0x6E8) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_W_INT012_OFFSET              (0x6EC) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_RCV_LTE_OFFSET              (0x6F0) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_RCV_TDS_OFFSET              (0x6F4) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_RCV_GM_OFFSET               (0x6F8) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_RCV_GS_OFFSET               (0x6FC) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_RCV_W_OFFSET                (0x700) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_W_INT012_OFFSET               (0x704) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_RCV_LTE_OFFSET              (0x708) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_RCV_TDS_OFFSET              (0x70C) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_RCV_GM_OFFSET               (0x710) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_RCV_GS_OFFSET               (0x714) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_RCV_W_OFFSET                (0x718) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_W_INT012_OFFSET               (0x71C) /* 寄存器 */
#define    HI_BBP_CTU_W_SND_LTE_INT_13_OFFSET                (0x720) /* 寄存器 */
#define    HI_BBP_CTU_W_SND_TDS_INT_13_OFFSET                (0x724) /* 寄存器 */
#define    HI_BBP_CTU_W_SND_GM_INT_13_OFFSET                 (0x728) /* 寄存器 */
#define    HI_BBP_CTU_W_SND_GS_INT_13_OFFSET                 (0x72C) /* 寄存器 */
#define    HI_BBP_CTU_W_SND_W_INT_13_OFFSET                  (0x730) /* 寄存器 */
#define    HI_BBP_CTU_S_LTE_CH_NUM_IND_OFFSET                (0xA44) /* LTE为从模时对应通道的指示寄存器 */
#define    HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_OFFSET       (0x734) /* 寄存器 */
#define    HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_OFFSET        (0x738) /* 寄存器 */
#define    HI_BBP_CTU_LTE_PREINT_OFFSET_TIME_OFFSET          (0x73C) /* 寄存器 */
#define    HI_BBP_CTU_LTE_INTERSYS_MEASURE_TIME_OFFSET       (0x740) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_OFFSET           (0x744) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_OFFSET           (0x748) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_OFFSET            (0x74C) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_OFFSET            (0x750) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_OFFSET             (0x754) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_LTE_INT012_OFFSET            (0x758) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_OFFSET            (0x75C) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_OFFSET            (0x760) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_RCV_GM_OFFSET             (0x764) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_RCV_GS_OFFSET             (0x768) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_RCV_W_OFFSET              (0x76C) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_LTE_INT012_OFFSET             (0x770) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_RCV_W_OFFSET              (0x774) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_OFFSET            (0x778) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_OFFSET             (0x77C) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_OFFSET             (0x780) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_OFFSET            (0x784) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_LTE_INT012_OFFSET             (0x788) /* 寄存器 */
#define    HI_BBP_CTU_LTE_SND_LTE_INT_13_OFFSET              (0x78C) /* 寄存器 */
#define    HI_BBP_CTU_LTE_SND_TDS_INT_13_OFFSET              (0x790) /* 寄存器 */
#define    HI_BBP_CTU_LTE_SND_GM_INT_13_OFFSET               (0x794) /* 寄存器 */
#define    HI_BBP_CTU_LTE_SND_GS_INT_13_OFFSET               (0x798) /* 寄存器 */
#define    HI_BBP_CTU_LTE_SND_W_INT_13_OFFSET                (0x79C) /* 寄存器 */
#define    HI_BBP_CTU_S_TDS_CH_NUM_IND_OFFSET                (0xA48) /* TDS为从模时对应通道的指示寄存器 */
#define    HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_OFFSET       (0x7A0) /* 寄存器 */
#define    HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_OFFSET        (0x7A4) /* 寄存器 */
#define    HI_BBP_CTU_TDS_PREINT_OFFSET_TIME_OFFSET          (0x7A8) /* 寄存器 */
#define    HI_BBP_CTU_TDS_INTERSYS_MEASURE_TIME_OFFSET       (0x7AC) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_OFFSET           (0x7B0) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_OFFSET           (0x7B4) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_OFFSET            (0x7B8) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_OFFSET            (0x7BC) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_OFFSET             (0x7C0) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_TDS_INT012_OFFSET            (0x7C4) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_OFFSET            (0x7C8) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_OFFSET            (0x7CC) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_RCV_GM_OFFSET             (0x7D0) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_RCV_GS_OFFSET             (0x7D4) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_RCV_W_OFFSET              (0x7D8) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_TDS_INT012_OFFSET             (0x7DC) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_OFFSET            (0x7E0) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_RCV_W_OFFSET              (0x7E4) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_OFFSET             (0x7E8) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_OFFSET             (0x7EC) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_OFFSET            (0x7F0) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_TDS_INT012_OFFSET             (0x7F4) /* 寄存器 */
#define    HI_BBP_CTU_TDS_SND_LTE_INT_13_OFFSET              (0x7F8) /* 寄存器 */
#define    HI_BBP_CTU_TDS_SND_TDS_INT_13_OFFSET              (0x7FC) /* 寄存器 */
#define    HI_BBP_CTU_TDS_SND_GM_INT_13_OFFSET               (0x800) /* 寄存器 */
#define    HI_BBP_CTU_TDS_SND_GS_INT_13_OFFSET               (0x804) /* 寄存器 */
#define    HI_BBP_CTU_TDS_SND_W_INT_13_OFFSET                (0x808) /* 寄存器 */
#define    HI_BBP_CTU_S_GM_CH_NUM_IND_OFFSET                 (0xA4C) /* GM为从模时对应通道的指示寄存器 */
#define    HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_OFFSET        (0x80C) /* 寄存器 */
#define    HI_BBP_CTU_GM_MEASURE_REPORT_VALID_OFFSET         (0x810) /* 寄存器 */
#define    HI_BBP_CTU_GM_PREINT_OFFSET_TIME_OFFSET           (0x814) /* 寄存器 */
#define    HI_BBP_CTU_GM_INTERSYS_MEASURE_TIME_OFFSET        (0x818) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_OFFSET            (0x81C) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_OFFSET            (0x820) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_OFFSET             (0x824) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_OFFSET             (0x828) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_RCV_W_OFFSET              (0x82C) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GM_INT012_OFFSET             (0x830) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_RCV_LTE_OFFSET             (0x834) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_RCV_TDS_OFFSET             (0x838) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_RCV_GM_OFFSET              (0x83C) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_RCV_GS_OFFSET              (0x840) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_RCV_W_OFFSET               (0x844) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GM_INT012_OFFSET              (0x848) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_OFFSET             (0x84C) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_OFFSET             (0x850) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_RCV_W_OFFSET               (0x854) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_RCV_GS_OFFSET              (0x858) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_RCV_GM_OFFSET              (0x85C) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GM_INT012_OFFSET              (0x860) /* 寄存器 */
#define    HI_BBP_CTU_GM_SND_LTE_INT_13_OFFSET               (0x864) /* 寄存器 */
#define    HI_BBP_CTU_GM_SND_TDS_INT_13_OFFSET               (0x868) /* 寄存器 */
#define    HI_BBP_CTU_GM_SND_GM_INT_13_OFFSET                (0x86C) /* 寄存器 */
#define    HI_BBP_CTU_GM_SND_GS_INT_13_OFFSET                (0x870) /* 寄存器 */
#define    HI_BBP_CTU_GM_SND_W_INT_13_OFFSET                 (0x874) /* 寄存器 */
#define    HI_BBP_CTU_S_GS_CH_NUM_IND_OFFSET                 (0xA50) /* GS为从模时对应通道的指示寄存器 */
#define    HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_OFFSET        (0x878) /* 寄存器 */
#define    HI_BBP_CTU_GS_MEASURE_REPORT_VALID_OFFSET         (0x87C) /* 寄存器 */
#define    HI_BBP_CTU_GS_PREINT_OFFSET_TIME_OFFSET           (0x880) /* 寄存器 */
#define    HI_BBP_CTU_GS_INTERSYS_MEASURE_TIME_OFFSET        (0x884) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_OFFSET            (0x888) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_OFFSET            (0x88C) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_OFFSET             (0x890) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_OFFSET             (0x894) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_RCV_W_OFFSET              (0x898) /* 寄存器 */
#define    HI_BBP_CTU_INT_CLEAR_GS_INT012_OFFSET             (0x89C) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_RCV_LTE_OFFSET             (0x8A0) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_RCV_TDS_OFFSET             (0x8A4) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_RCV_GM_OFFSET              (0x8A8) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_RCV_GS_OFFSET              (0x8AC) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_RCV_W_OFFSET               (0x8B0) /* 寄存器 */
#define    HI_BBP_CTU_INT_MASK_GS_INT012_OFFSET              (0x8B4) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_OFFSET             (0x8B8) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_OFFSET             (0x8BC) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_RCV_GM_OFFSET              (0x8C0) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_RCV_GS_OFFSET              (0x8C4) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_RCV_W_OFFSET               (0x8C8) /* 寄存器 */
#define    HI_BBP_CTU_INT_TYPE_GS_INT012_OFFSET              (0x8CC) /* 寄存器 */
#define    HI_BBP_CTU_GS_SND_LTE_INT_13_OFFSET               (0x8D0) /* 寄存器 */
#define    HI_BBP_CTU_GS_SND_TDS_INT_13_OFFSET               (0x8D4) /* 寄存器 */
#define    HI_BBP_CTU_GS_SND_GM_INT_13_OFFSET                (0x8D8) /* 寄存器 */
#define    HI_BBP_CTU_GS_SND_GS_INT_13_OFFSET                (0x8DC) /* 寄存器 */
#define    HI_BBP_CTU_GS_SND_W_INT_13_OFFSET                 (0x8E0) /* 寄存器 */
#define    HI_BBP_CTU_GBBP1_19M_SEL_OFFSET                   (0x8E4)
#define    HI_BBP_CTU_GBBP2_19M_SEL_OFFSET                   (0x8E8)
#define    HI_BBP_CTU_WBBP_19M_SEL_OFFSET                    (0x8EC)
#define    HI_BBP_CTU_LBBP_19M_SEL_OFFSET                    (0x8F0)
#define    HI_BBP_CTU_TBBP1_19M_SEL_OFFSET                   (0x8F4)
#define    HI_BBP_CTU_GBBP1_CLK_SEL_OFFSET                   (0x8F8)
#define    HI_BBP_CTU_GBBP2_CLK_SEL_OFFSET                   (0x8FC)
#define    HI_BBP_CTU_WBBP_CLK_SEL_OFFSET                    (0x900)
#define    HI_BBP_CTU_LBBP_CLK_SEL_OFFSET                    (0x904)
#define    HI_BBP_CTU_TBBP_CLK_SEL_OFFSET                    (0x908)
#define    HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET                   (0x90C) /* 寄存器 */
#define    HI_BBP_CTU_W_ABBIF_FMT_OFFSET                     (0x910) /* 寄存器 */
#define    HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET                   (0x914) /* 寄存器 */
#define    HI_BBP_CTU_GM_ABBIF_FMT_OFFSET                    (0x918) /* 寄存器 */
#define    HI_BBP_CTU_GS_ABBIF_FMT_OFFSET                    (0x91C) /* 寄存器 */
#define    HI_BBP_CTU_REG0_FOR_USE_OFFSET                    (0x920) /* 寄存器 */
#define    HI_BBP_CTU_REG1_FOR_USE_OFFSET                    (0x924) /* 寄存器 */
#define    HI_BBP_CTU_REG2_FOR_USE_OFFSET                    (0x928) /* 寄存器 */
#define    HI_BBP_CTU_REG3_FOR_USE_OFFSET                    (0x92C) /* 寄存器 */
#define    HI_BBP_CTU_REG4_FOR_USE_OFFSET                    (0x930) /* 寄存器 */
#define    HI_BBP_CTU_REG5_FOR_USE_OFFSET                    (0x934) /* 寄存器 */
#define    HI_BBP_CTU_REG6_FOR_USE_OFFSET                    (0x938) /* 寄存器 */
#define    HI_BBP_CTU_REG7_FOR_USE_OFFSET                    (0x93C) /* 寄存器 */
#define    HI_BBP_CTU_REG8_FOR_USE_OFFSET                    (0x940) /* 寄存器 */
#define    HI_BBP_CTU_REG9_FOR_USE_OFFSET                    (0x944) /* 寄存器 */
#define    HI_BBP_CTU_REG10_FOR_USE_OFFSET                   (0x948) /* 寄存器 */
#define    HI_BBP_CTU_REG11_FOR_USE_OFFSET                   (0x94C) /* 寄存器 */
#define    HI_BBP_CTU_REG12_FOR_USE_OFFSET                   (0x950) /* 寄存器 */
#define    HI_BBP_CTU_REG13_FOR_USE_OFFSET                   (0x954) /* 寄存器 */
#define    HI_BBP_CTU_REG14_FOR_USE_OFFSET                   (0x958) /* 寄存器 */
#define    HI_BBP_CTU_REG15_FOR_USE_OFFSET                   (0x95C) /* 寄存器 */
#define    HI_BBP_CTU_REG16_FOR_USE_OFFSET                   (0x960) /* 寄存器 */
#define    HI_BBP_CTU_REG17_FOR_USE_OFFSET                   (0x964) /* 寄存器 */
#define    HI_BBP_CTU_REG18_FOR_USE_OFFSET                   (0x968) /* 寄存器 */
#define    HI_BBP_CTU_REG19_FOR_USE_OFFSET                   (0x96C) /* 寄存器 */
#define    HI_BBP_CTU_REG20_FOR_USE_OFFSET                   (0x970) /* 寄存器 */
#define    HI_BBP_CTU_REG21_FOR_USE_OFFSET                   (0x974) /* 寄存器 */
#define    HI_BBP_CTU_REG22_FOR_USE_OFFSET                   (0x978) /* 寄存器 */
#define    HI_BBP_CTU_REG23_FOR_USE_OFFSET                   (0x97C) /* 寄存器 */
#define    HI_BBP_CTU_REG24_FOR_USE_OFFSET                   (0x980) /* 寄存器 */
#define    HI_BBP_CTU_REG25_FOR_USE_OFFSET                   (0x984) /* 寄存器 */
#define    HI_BBP_CTU_REG26_FOR_USE_OFFSET                   (0x988) /* 寄存器 */
#define    HI_BBP_CTU_REG27_FOR_USE_OFFSET                   (0x98C) /* 寄存器 */
#define    HI_BBP_CTU_REG28_FOR_USE_OFFSET                   (0x990) /* 寄存器 */
#define    HI_BBP_CTU_REG29_FOR_USE_OFFSET                   (0x994) /* 寄存器 */
#define    HI_BBP_CTU_REG30_FOR_USE_OFFSET                   (0x998) /* 寄存器 */
#define    HI_BBP_CTU_REG31_FOR_USE_OFFSET                   (0x99C) /* 寄存器 */
#define    HI_BBP_CTU_REG32_FOR_USE_OFFSET                   (0x9A0) /* 寄存器 */
#define    HI_BBP_CTU_REG33_FOR_USE_OFFSET                   (0x9A4) /* 寄存器 */
#define    HI_BBP_CTU_REG34_FOR_USE_OFFSET                   (0x9A8) /* 寄存器 */
#define    HI_BBP_CTU_REG35_FOR_USE_OFFSET                   (0x9AC) /* 寄存器 */
#define    HI_BBP_CTU_REG36_FOR_USE_OFFSET                   (0x9B0) /* 寄存器 */
#define    HI_BBP_CTU_REG37_FOR_USE_OFFSET                   (0x9B4) /* 寄存器 */
#define    HI_BBP_CTU_REG38_FOR_USE_OFFSET                   (0x9B8) /* 寄存器 */
#define    HI_BBP_CTU_REG39_FOR_USE_OFFSET                   (0x9BC) /* 寄存器 */
#define    HI_BBP_CTU_REG40_FOR_USE_OFFSET                   (0x9C0) /* 寄存器 */
#define    HI_BBP_CTU_REG41_FOR_USE_OFFSET                   (0x9C4) /* 寄存器 */
#define    HI_BBP_CTU_REG42_FOR_USE_OFFSET                   (0x9C8) /* 寄存器 */
#define    HI_BBP_CTU_REG43_FOR_USE_OFFSET                   (0x9CC) /* 寄存器 */
#define    HI_BBP_CTU_REG44_FOR_USE_OFFSET                   (0x9D0) /* 寄存器 */
#define    HI_BBP_CTU_REG45_FOR_USE_OFFSET                   (0x9D4) /* 寄存器 */
#define    HI_BBP_CTU_REG46_FOR_USE_OFFSET                   (0x9D8) /* 寄存器 */
#define    HI_BBP_CTU_REG47_FOR_USE_OFFSET                   (0x9DC) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG0_CPU_OFFSET                    (0x9E0) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG1_CPU_OFFSET                    (0x9E4) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG2_CPU_OFFSET                    (0x9E8) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG3_CPU_OFFSET                    (0x9EC) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG4_CPU_OFFSET                    (0x9F0) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG5_CPU_OFFSET                    (0x9F4) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG6_CPU_OFFSET                    (0x9F8) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG7_CPU_OFFSET                    (0x9FC) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG8_CPU_OFFSET                    (0xA00) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG9_CPU_OFFSET                    (0xA04) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG10_CPU_OFFSET                   (0xA08) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG11_CPU_OFFSET                   (0xA0C) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG12_CPU_OFFSET                   (0xA10) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG13_CPU_OFFSET                   (0xA14) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG14_CPU_OFFSET                   (0xA18) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG15_CPU_OFFSET                   (0xA1C) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG16_CPU_OFFSET                   (0xA20) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG17_CPU_OFFSET                   (0xA24) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG18_CPU_OFFSET                   (0xA28) /* 寄存器 */
#define    HI_BBP_CTU_DBG_REG19_CPU_OFFSET                   (0xA2C) /* 寄存器 */
#define    HI_BBP_CTU_DBG_CLK_GATE_EN_OFFSET                 (0xA30) /* 门控时钟配置寄存器 */
#define    HI_BBP_CTU_DBG_REG20_CPU_OFFSET                   (0xA34) /* 寄存器 */
#define    HI_BBP_CTU_DBG_COMM_EN_OFFSET                     (0xA38) /* 寄存器 */
#define    HI_BBP_CTU_L_CH_SW_OFFSET                         (0xB00)
#define    HI_BBP_CTU_W_CH_SW_OFFSET                         (0xB04)
#define    HI_BBP_CTU_T_CH_SW_OFFSET                         (0xB08)
#define    HI_BBP_CTU_GM_CH_SW_OFFSET                        (0xB0C)
#define    HI_BBP_CTU_GS_CH_SW_OFFSET                        (0xB10)
#define    HI_BBP_CTU_L_TCVR_SEL0_OFFSET                     (0xB20)
#define    HI_BBP_CTU_L_TCVR_SEL1_OFFSET                     (0xB24)
#define    HI_BBP_CTU_T_TCVR_SEL0_OFFSET                     (0xB28)
#define    HI_BBP_CTU_T_TCVR_SEL1_OFFSET                     (0xB2C)
#define    HI_BBP_CTU_W_TCVR_SEL0_OFFSET                     (0xB30)
#define    HI_BBP_CTU_W_TCVR_SEL1_OFFSET                     (0xB34)
#define    HI_BBP_CTU_GM_TCVR_SEL0_OFFSET                    (0xB38)
#define    HI_BBP_CTU_GM_TCVR_SEL1_OFFSET                    (0xB3C)
#define    HI_BBP_CTU_GS_TCVR_SEL0_OFFSET                    (0xB40)
#define    HI_BBP_CTU_GS_TCVR_SEL1_OFFSET                    (0xB44)
#define    HI_BBP_CTU_L_TCVR_VALUE0_OFFSET                   (0xB48)
#define    HI_BBP_CTU_L_TCVR_VALUE1_OFFSET                   (0xB4C)
#define    HI_BBP_CTU_T_TCVR_VALUE0_OFFSET                   (0xB50)
#define    HI_BBP_CTU_T_TCVR_VALUE1_OFFSET                   (0xB54)
#define    HI_BBP_CTU_W_TCVR_VALUE0_OFFSET                   (0xB58)
#define    HI_BBP_CTU_W_TCVR_VALUE1_OFFSET                   (0xB5C)
#define    HI_BBP_CTU_GM_TCVR_VALUE0_OFFSET                  (0xB60)
#define    HI_BBP_CTU_GM_TCVR_VALUE1_OFFSET                  (0xB64)
#define    HI_BBP_CTU_GS_TCVR_VALUE0_OFFSET                  (0xB68)
#define    HI_BBP_CTU_GS_TCVR_VALUE1_OFFSET                  (0xB6C)
#define    HI_BBP_CTU_RF_TCVR_STATE_OFFSET                   (0xB70)
#define    HI_BBP_CTU_MIPI_UNBIND_EN_OFFSET                  (0xB90)
#define    HI_BBP_CTU_MIPI_CH_SEL0_OFFSET                    (0xB94)
#define    HI_BBP_CTU_MIPI_CH_SEL1_OFFSET                    (0xB98)
#define    HI_BBP_CTU_APT_CH_SEL0_OFFSET                     (0xB9C)
#define    HI_BBP_CTU_APT_CH_SEL1_OFFSET                     (0xBA0)
#define    HI_BBP_CTU_AFC_CH_SEL0_OFFSET                     (0xBA4)
#define    HI_BBP_CTU_AFC_CH_SEL1_OFFSET                     (0xBA8)
#define    HI_BBP_CTU_ABB_TX_CH_SEL0_OFFSET                  (0xBAC)
#define    HI_BBP_CTU_ABB_TX_CH_SEL1_OFFSET                  (0xBB0)
#define    HI_BBP_CTU_PMU_CH_SEL0_OFFSET                     (0xBB4)
#define    HI_BBP_CTU_PMU_CH_SEL1_OFFSET                     (0xBB8)
#define    HI_BBP_CTU_VALID_NUM_OFFSET                       (0xBC0)
#define    HI_BBP_CTU_VALID_NUM1_OFFSET                      (0xBC4)
#define    HI_BBP_CTU_DBG_EN_OFFSET                          (0xF80)
#define    HI_BBP_CTU_DBG_ID_OFFSET                          (0xF84)
#define    HI_BBP_CTU_DBG_FLT_OFFSET                         (0xF88)
#define    HI_BBP_CTU_BBC_REV0_OFFSET                        (0xFE0)
#define    HI_BBP_CTU_BBC_REV1_OFFSET                        (0xFE4)
#define    HI_BBP_CTU_BBC_REV2_OFFSET                        (0xFE8)
#define    HI_BBP_CTU_BBC_REV3_OFFSET                        (0xFEC)
#define    HI_BBP_CTU_BBC_REV4_OFFSET                        (0xFF0)

/********************************************************************************/
/*    bbp_ctu 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    c0_primary_mode_ind        : 3; /* [2..0] 通道0对应的主模（定义为主模0）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模0。000：LTE 主模。001：W主模。010：TDS主模。011：Gm主模；100：Gs主模；101~111：无效。配置时刻：各模根据通信场景配置 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_PRIMARY_MODE_IND_T;    /* 通道0对应主模（主模0）的模式指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_second_mode_ind         : 3; /* [2..0] 主模0对应的从模（定义为从模0）的模式指示，表示当前LTE/W/Gm/Gs/TDS谁是从模0。000：LTE从模。001：W 从模。010：TDS从模。011：Gm从模；100：Gs从模；101~111：无效。配置时刻：各模根据通信场景配置注：约定主模0固定走通道0，主模1固定走通道1。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_SECOND_MODE_IND_T;    /* 主模0对应的从模（从模0）的模式指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    afc0_value_inter_rat       : 32; /* [31..0] AFC0值传递[15:0]:AFC积分器值[16]:AFC锁定指示配置时刻：DSP从主模切换为从模时配置；DSP从从模切换为主模时读取并清除锁定指示生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_AFC0_VALUE_INTER_RAT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_afc_pdm_select_mask     : 3; /* [2..0] 通道0 c0_afc_pdm_select控制信号。3’b000: c0_afc_pdm_select自由输出。3’b001:强制c0_afc_pdm_select输出为3’b000。3’b010:强制c0_afc_pdm_select输出为3’b001。3’b011: 强制c0_afc_pdm_select输出为3’b010。3’b100:强制c0_afc_pdm_select输出为3’b011。3’b101: 强制c0_afc_pdm_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_ant_switch_pa_select_mask : 3; /* [2..0] 通道0 c0_ant_switch_pa_select控制信号。3’b000: c0_ant_switch_pa_select自由输出。3’b001:强制c0_ant_switch_pa_select输出为3’b000。3’b010:强制c0_ant_switch_pa_select输出为3’b001。3’b011: 强制c0_ant_switch_pa_select输出为3’b010。3’b100:强制c0_ant_switch_pa_select输出为3’b011。3’b101: 强制c0_ant_switch_pa_select输出为3’b100。3'b110~3'b111:无效。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_rfic_select_mask        : 3; /* [2..0] 通道0 c0_rfic_select控制信号。3’b000: c0_rfic_select自由输出。3’b001:强制c0_rfic_select输出为3’b000。3’b010:强制c0_rfic_select输出为3’b001。3’b011: 强制c0_rfic_select输出为3’b010。3’b100:强制c0_rfic_select输出为3’b011。3’b101: 强制c0_rfic_select输出为3’b100。3'b110~3'b111:无效。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_RFIC_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_mipi_select_mask        : 3; /* [2..0] 通道0 c0_mipi_select控制信号。3’b000: c0_mipi_select自由输出。3’b001:强制c0_mipi_select输出为3’b000。3’b010:强制c0_mipi_select输出为3’b001。3’b011: 强制c0_mipi_select输出为3’b010。3’b100:强制c0_mipi_select输出为3’b011。3’b101: 强制c0_mipi_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_MIPI_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_abb_select_mask         : 3; /* [2..0] 通道0 c0_abb_select控制信号。3’b000: c0_abb_select自由输出。3’b001:强制c0_abb_select输出为3’b000。3’b010:强制c0_abb_select输出为3’b001。3’b011: 强制c0_abb_select输出为3’b010。3’b100:强制c0_abb_select输出为3’b011。3’b101: 强制c0_abb_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_ABB_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_pmu_select_mask         : 3; /* [2..0] 通道0 c0_pmu_select控制信号。3’b000: c0_pmu_select自由输出。3’b001:强制c0_pmu_select输出为3’b000。3’b010:强制c0_pmu_select输出为3’b001。3’b011: 强制c0_pmu_select输出为3’b010。3’b100:强制c0_pmu_select输出为3’b011。3’b101: 强制c0_pmu_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留，调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_PMU_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_c0_sel_mask            : 3; /* [2..0] 当前通道0（交叉之前）输出的强配模式。注：其适用于各个外围器件（ABB、RF、PA等）公共的交叉强配。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留，调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_C0_SEL_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_rfic0_rstn             : 1; /* [0..0] 软件配置的RFIC0复位信号。 低有效。0：复位，1：解复位。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_RFIC0_RSTN_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c0_second_afc_mode         : 1; /* [0..0] 0:通道0从模使用通道0主模afc控制字1:通道0从模使用通道0从模afc控制字 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C0_SECOND_AFC_MODE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    bbp_sys_0control           : 32; /* [31..0] 备份bbp_sys_0control。GU不涉及。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBP_SYS_0CONTROL_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_rd_overtime_flag_dsp : 1; /* [0..0] DSP等待回读结果超时标志位1:超时 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ssi0_rd_overtime_flag_dsp  : 1; /* [0..0] DSP等待回读结果超时标志位1:超时 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_rd_overtime_clr_imi  : 1; /* [0..0] DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ssi0_rd_overtime_clr_imi   : 1; /* [0..0] DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_1          : 32; /* [31..0] xbbp向SSI0 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_2          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_2_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_3          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_3_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_4          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_4_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_5          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_5_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_6          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_6_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_7          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_7_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_8          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_8_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_9          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_9_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_10         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_10_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_11         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_11_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_12         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_12_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_13         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_14         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_14_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_15         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_15_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic0_16         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC0_16_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_rfic0       : 1; /* [0..0] 所有回读指令执行完毕,可以取走回读数据1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_rfic0_clr   : 1; /* [0..0] 1:DSP取走所有回读数据后,写入clr信号清标志位 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_1          : 32; /* [31..0] xbbp向MIPI0 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_2          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_2_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_3          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_3_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_4          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_4_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_5          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_5_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_6          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_6_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_7          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_7_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_8          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_8_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_9          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_9_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_10         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_10_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_11         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_11_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_12         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_12_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_13         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_14         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_14_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_15         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_15_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi0_16         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI0_16_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_mipi0       : 1; /* [0..0] 所有回读指令执行完毕,可以取走回读数据1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_mipi0_clr   : 1; /* [0..0] 1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_rd_data_low_soft     : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放低32bit. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI0_RD_DATA_LOW_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_rd_data_high_soft    : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放高32bit. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI0_RD_DATA_HIGH_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_end_flag_mipi0_soft     : 1; /* [0..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读数据有效指示 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_grant_dsp            : 1; /* [0..0] 由软件直接控制MIPI0 master1:软件可以发送CFG_IND启动mipi00:软件等待mipi0授权使用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI0_GRANT_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_wdata_low        : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令低32bit */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI0_WDATA_LOW_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_wdata_high       : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令高32bit */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI0_WDATA_HIGH_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_en_imi           : 1; /* [0..0] 由软件直接控制MIPI0 master,需首先拉高此指示,屏蔽xbbp对MIPI0的控制 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI0_EN_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_cfg_ind_imi      : 1; /* [0..0] 由软件直接控制MIPI0 master,拉高此dsp_mipi0_en后,产生软件启动脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_rd_clr           : 1; /* [0..0] 由软件直接控制MIPI0 master,取走回读数据后,清标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI0_RD_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    abb0_line_control_cmd_sel  : 1; /* [0..0] ABB0线控强配模式：1：表示软件强配，此时上述各个强配才生效；0：表示由CTU根据各BBP产生。默认值为0。测试用。 */
        unsigned int    abb0_mode_sel_cmd          : 3; /* [3..1] 软件强配模式abb0_mode_sel. */
        unsigned int    abb0_tx_en_cmd             : 1; /* [4..4] 软件强配模式abb0_tx_en. */
        unsigned int    abb0_rxa_en_cmd            : 1; /* [5..5] 软件强配模式abb0_rxa_en. */
        unsigned int    abb0_rxb_en_cmd            : 1; /* [6..6] 软件强配模式abb0_rxb_en. */
        unsigned int    abb0_blka_en_cmd           : 1; /* [7..7] 软件强配模式abb0_rxa_blk_en. */
        unsigned int    abb0_blkb_en_cmd           : 1; /* [8..8] 软件强配模式abb0_rxb_blk_en. */
        unsigned int    reserved                   : 23; /* [31..9] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ch0_bbp_sel                : 3; /* [2..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CH0_BBP_SEL_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_func_sel         : 1; /* [0..0] cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_test_func        : 1; /* [0..0] cpu配置mipi功能,测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_sclk_test        : 1; /* [0..0] cpu配置mipi功能, 测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_sdata_test       : 1; /* [0..0] cpu配置mipi功能, 测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_clr_ind_imi      : 1; /* [0..0] cpu配置mipi功能,mipi0master清异常信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi0_fifo_clr_imi     : 1; /* [0..0] mipi0存储待发送数据同步FIFO复位信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_ssi0_fifo_clr_imi      : 1; /* [0..0] ssi0存储待发送数据同步FIFO复位信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_rx0_lte_ckg_bypass     : 1; /* [0..0]  */
        unsigned int    cpu_rx0_tds_ckg_bypass     : 1; /* [1..1]  */
        unsigned int    cpu_rx0_w_ckg_bypass       : 1; /* [2..2]  */
        unsigned int    cpu_rx0_g1_ckg_bypass      : 1; /* [3..3]  */
        unsigned int    cpu_rx0_g2_ckg_bypass      : 1; /* [4..4]  */
        unsigned int    cpu_tx0_lte_ckg_bypass     : 1; /* [5..5]  */
        unsigned int    cpu_tx0_tds_ckg_bypass     : 1; /* [6..6]  */
        unsigned int    cpu_tx0_w_ckg_bypass       : 1; /* [7..7]  */
        unsigned int    cpu_tx0_g1_ckg_bypass      : 1; /* [8..8]  */
        unsigned int    cpu_tx0_g2_ckg_bypass      : 1; /* [9..9]  */
        unsigned int    reserved                   : 22; /* [31..10] 通道0上下行时钟门控bypass,仅供测试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T;

typedef union
{
    struct
    {
        unsigned int    c1_primary_mode_ind        : 3; /* [2..0] 通道1对应的主模（定义为主模1）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模1。000：LTE 主模。001：W主模。010：TDS主模。011：Gm主模；100：Gs主模；101~111：无效。配置时刻：各模根据通信场景配置 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_PRIMARY_MODE_IND_T;    /* 通道1对应主模（主模1）的模式指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_second_mode_ind         : 3; /* [2..0] 主模1对应的从模（定义为从模1）的模式指示，表示LTE/W/Gm/Gs/TDS谁是从模1。000：LTE从模。001：W 从模。010：TDS从模。011：Gm从模；100：Gs从模；101~111：无效。配置时刻：各模根据通信场景配置.注：约定主模0固定走通道0，主模1固定走通道1。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_SECOND_MODE_IND_T;    /* 主模1对应的从模（从模1）的模式指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_ch_num_ind              : 1; /* [0..0] 从模1对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    c1_ch_num_ind_sel          : 1; /* [1..1] 从模1对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_CH_NUM_IND_T;    /* 从模1对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    afc1_value_inter_rat       : 32; /* [31..0] AFC1值传递[15:0]:AFC积分器值[16]:AFC锁定指示配置时刻：DSP从主模切换为从模时配置；DSP从从模切换为主模时读取并清除锁定指示生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_AFC1_VALUE_INTER_RAT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_afc_pdm_select_mask     : 3; /* [2..0] 通道1 c1_afc_pdm_select控制信号。3’b000: c1_afc_pdm_select自由输出。3’b001:强制c1_afc_pdm_select输出为3’b000。3’b010:强制c1_afc_pdm_select输出为3’b001。3’b011: 强制c1_afc_pdm_select输出为3’b010。3’b100:强制c1_afc_pdm_select输出为3’b011。3’b101: 强制c1_afc_pdm_select输出为3’b100。3'b110~3'b111:无效。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_ant_switch_pa_select_mask : 3; /* [2..0] 通道1 c1_ant_switch_pa_select控制信号。3’b000: c1_ant_switch_pa_select自由输出。3’b001:强制c1_ant_switch_pa_select输出为3’b000。3’b010:强制c1_ant_switch_pa_select输出为3’b001。3’b011: 强制c1_ant_switch_pa_select输出为3’b010。3’b100:强制c1_ant_switch_pa_select输出为3’b011。3’b101: 强制c1_ant_switch_pa_select输出为3’b100。3'b110~3'b111:无效。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_rfic_select_mask        : 3; /* [2..0] 通道1 c1_rfic_select控制信号。3’b000: c1_rfic_select自由输出。3’b001:强制c1_rfic_select输出为3’b000。3’b010:强制c1_rfic_select输出为3’b001。3’b011: 强制c1_rfic_select输出为3’b010。3’b100:强制c1_rfic_select输出为3’b011。3’b101: 强制c1_rfic_select输出为3’b100。3'b110~3'b111:无效。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_RFIC_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_mipi_select_mask        : 3; /* [2..0] 通道0 c1_mipi_select控制信号。3’b000: c1_mipi_select自由输出。3’b001:强制c1_mipi_select输出为3’b000。3’b010:强制c1_mipi_select输出为3’b001。3’b011: 强制c1_mipi_select输出为3’b010。3’b100:强制c1_mipi_select输出为3’b011。3’b101: 强制c1_mipi_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_MIPI_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_abb_select_mask         : 3; /* [2..0] 通道0 c1_abb_select控制信号。3’b000: c1_abb_select自由输出。3’b001:强制c1_abb_select输出为3’b000。3’b010:强制c1_abb_select输出为3’b001。3’b011: 强制c1_abb_select输出为3’b010。3’b100:强制c1_abb_select输出为3’b011。3’b101: 强制c1_abb_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_ABB_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_pmu_select_mask         : 3; /* [2..0] 通道0 c1_pmu_select控制信号。3’b000: c1_pmu_select自由输出。3’b001:强制c1_pmu_select输出为3’b000。3’b010:强制c1_pmu_select输出为3’b001。3’b011: 强制c1_pmu_select输出为3’b010。3’b100:强制c1_pmu_select输出为3’b011。3’b101: 强制c1_pmu_select输出为3’b100。3'b111:无效;其它为保留。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留，调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_PMU_SELECT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_c1_sel_mask            : 3; /* [2..0] 当前通道1（交叉之前） 输出的强配模式。注：其适用于各个外围器件（ABB、RF、PA等）公共的交叉强配。调试用。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留，调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_C1_SEL_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_rfic1_rstn             : 1; /* [0..0] 软件配置的RFIC1复位信号。 低有效。 0：复位，1：解复位。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_RFIC1_RSTN_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    c1_second_afc_mode         : 1; /* [0..0] 0:通道1从模使用通道1主模afc控制字1:通道1从模使用通道1从模afc控制字 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_C1_SECOND_AFC_MODE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    bbp_sys_1control           : 32; /* [31..0] 备份bbp_sys_1controlGU不涉及。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBP_SYS_1CONTROL_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi1_rd_overtime_flag_dsp : 1; /* [0..0] DSP等待回读结果超时标志位1:超时 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ssi1_rd_overtime_flag_dsp  : 1; /* [0..0] DSP等待回读结果超时标志位1:超时 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi1_rd_overtime_clr_imi  : 1; /* [0..0] DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ssi1_rd_overtime_clr_imi   : 1; /* [0..0] DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_1          : 32; /* [31..0] xbbp向SSI1 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_2          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_2_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_3          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_3_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_4          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_4_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_5          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_5_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_6          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_6_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_7          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_7_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_8          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_8_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_9          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_9_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_10         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_10_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_11         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_11_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_12         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_12_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_13         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_14         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_14_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_15         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_15_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_rfic1_16         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_RFIC1_16_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_rfic1       : 1; /* [0..0] 所有回读指令执行完毕,可以取走回读数据1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_rfic1_clr   : 1; /* [0..0] 1:DSP取走所有回读数据后,写入clr信号清标志位 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_1          : 32; /* [31..0] xbbp向MIPI1 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_2          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_2_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_3          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_3_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_4          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_4_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_5          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_5_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_6          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_6_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_7          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_7_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_8          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_8_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_9          : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_9_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_10         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_10_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_11         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_11_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_12         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_12_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_13         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_14         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_14_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_15         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_15_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_mipi1_16         : 32; /* [31..0] 同上 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_MIPI1_16_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_mipi1       : 1; /* [0..0] 所有回读指令执行完毕,可以取走回读数据1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_result_flag_mipi1_clr   : 1; /* [0..0] 1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi1_rd_data_low_soft     : 32; /* [31..0] 由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读指令存放低32bit. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI1_RD_DATA_LOW_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi1_rd_data_high_soft    : 32; /* [31..0] 由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读指令存放高32bit. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI1_RD_DATA_HIGH_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_end_flag_mipi1_soft     : 1; /* [0..0] 由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读数据有效指示 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi1_grant_dsp            : 1; /* [0..0] 由软件直接控制MIPI1 master1:软件可以发送CFG_IND启动mipi10:软件等待mipi1授权使用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI1_GRANT_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi1_wdata_low        : 32; /* [31..0] 由软件直接控制MIPI1 master,不借助xbbp发送写指令时,写指令低32bit */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI1_WDATA_LOW_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi1_wdata_high       : 32; /* [31..0] 由软件直接控制MIPI1 master,不借助xbbp发送写指令时,写指令高32bit */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI1_WDATA_HIGH_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi1_en_imi           : 1; /* [0..0] 由软件直接控制MIPI1 master,需首先拉高此指示,屏蔽xbbp对MIPI0的控制 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI1_EN_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi1_cfg_ind_imi      : 1; /* [0..0] 由软件直接控制MIPI1 master,拉高此dsp_mipi1_en后,产生软件启动脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi1_rd_clr           : 1; /* [0..0] 由软件直接控制MIPI1 master,取走回读数据后,清标志位脉冲 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_MIPI1_RD_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    abb1_line_control_cmd_sel  : 1; /* [0..0] ABB1线控强配模式：1：表示软件强配，此时上述各个强配才生效；0：表示由CTU根据各BBP产生。默认值为0。测试用。 */
        unsigned int    abb1_mode_sel_cmd          : 3; /* [3..1] 软件强配模式abb1_mode_sel. */
        unsigned int    abb1_tx_en_cmd             : 1; /* [4..4] 软件强配模式abb1_tx_en. */
        unsigned int    abb1_rxa_en_cmd            : 1; /* [5..5] 软件强配模式abb1_rxa_en. */
        unsigned int    abb1_rxb_en_cmd            : 1; /* [6..6] 软件强配模式abb1_rxb_en. */
        unsigned int    abb1_blka_en_cmd           : 1; /* [7..7] 软件强配模式abb1_rxa_blk_en. */
        unsigned int    abb1_blkb_en_cmd           : 1; /* [8..8] 软件强配模式abb1_rxb_blk_en. */
        unsigned int    reserved                   : 23; /* [31..9] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ch1_bbp_sel                : 3; /* [2..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CH1_BBP_SEL_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_func_sel         : 1; /* [0..0] cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_test_func        : 1; /* [0..0] cpu配置mipi功能,测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_sclk_test        : 1; /* [0..0] cpu配置mipi功能,测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_sdata_test       : 1; /* [0..0] cpu配置mipi功能,测试用 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_clr_ind_imi      : 1; /* [0..0] cpu配置mipi功能,mipi1master清异常信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_mipi1_fifo_clr_imi     : 1; /* [0..0] mipi1存储待发送数据同步FIFO复位信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_ssi1_fifo_clr_imi      : 1; /* [0..0] ssi1存储待发送数据同步FIFO复位信号 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_T;

typedef union
{
    struct
    {
        unsigned int    cpu_rx1_lte_ckg_bypass     : 1; /* [0..0]  */
        unsigned int    cpu_rx1_tds_ckg_bypass     : 1; /* [1..1]  */
        unsigned int    cpu_rx1_w_ckg_bypass       : 1; /* [2..2]  */
        unsigned int    cpu_rx1_g1_ckg_bypass      : 1; /* [3..3]  */
        unsigned int    cpu_rx1_g2_ckg_bypass      : 1; /* [4..4]  */
        unsigned int    cpu_tx1_lte_ckg_bypass     : 1; /* [5..5]  */
        unsigned int    cpu_tx1_tds_ckg_bypass     : 1; /* [6..6]  */
        unsigned int    cpu_tx1_w_ckg_bypass       : 1; /* [7..7]  */
        unsigned int    cpu_tx1_g1_ckg_bypass      : 1; /* [8..8]  */
        unsigned int    cpu_tx1_g2_ckg_bypass      : 1; /* [9..9]  */
        unsigned int    reserved                   : 22; /* [31..10] 通道1上下行时钟门控bypass,仅供测试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T;

typedef union
{
    struct
    {
        unsigned int    delay_cnt                  : 5; /* [4..0] CTU检测到timing_get_ind后固定延迟delay_cnt个CTU时钟周期后拉高timing_valid信号。配置时刻：上电初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 27; /* [31..5] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DELAY_CNT_T;    /* 定时锁存延迟寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_wait_cnt               : 16; /* [15..0] CTU中断上报等待时间，单位为一个CLK周期。DSP清除中断标志寄存器的某种类型bit后，CTU上报中断电平需要拉低，如果此时中断标志寄存器还有其它中断类型有效，则需等待int_wait_cnt个时钟周期后再拉高CTU上报中断。配置时刻：上电初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_WAIT_CNT_T;    /* 中断延迟寄存器 */

typedef union
{
    struct
    {
        unsigned int    ltebbp_input_mask          : 2; /* [1..0] 电平信号，低有效。当LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。bit1:1’b0: lte_intrasys_valid输入无效，被强制为1。bit0:1’b0:lte_intersys_ingap_pre_ind输入无效，被强制为0。bit[1:0] = 11b: 让lte_intrasys_valid和lte_intersys_ingap_pre_ind输入有效。配置时刻：LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTEBBP_INPUT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wbbp_input_mask            : 2; /* [1..0] 电平信号，低有效。当W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。bit1:1’b0: 让w_intrasys_valid输入无效，被强制为1。bit0:1’b0:w_intersys_ingap_pre_ind输入无效，被强制为0。bit[1:0] = 11b: 让w_intrasys_valid和w_intersys_ingap_pre_ind输入有效。配置时刻：W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WBBP_INPUT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gmbbp_input_mask           : 2; /* [1..0] 电平信号，低有效。当Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。bit1:1’b0: 让gm_intrasys_valid输入无效，被强制为1。bit0:1’b0:gm_intersys_ingap_pre_ind输入无效，被强制为0。bit[1:0] = 11b: 让gm_intrasys_valid和gm_intersys_ingap_pre_ind输入有效。配置时刻：Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GMBBP_INPUT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gsbbp_input_mask           : 2; /* [1..0] 电平信号，低有效。当Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。bit1:1’b0: 让gs_intrasys_valid输入无效，被强制为1。bit0:1’b0:gs_intersys_ingap_pre_ind输入无效，被强制为0。bit[1:0] = 11b: 让gs_intrasys_valid和gs_intersys_ingap_pre_ind输入有效。配置时刻：Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GSBBP_INPUT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    tdsbbp_input_mask          : 2; /* [1..0] 电平信号，低有效。当TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。bit1:1’b0: 让tds_intrasys_valid输入无效，被强制为1。bit0:1’b0:tds_intersys_ingap_pre_ind输入无效，被强制为0。bit[1:0] = 11b: 让tds_intrasys_valid和tds_intersys_ingap_pre_ind输入有效。配置时刻：TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDSBBP_INPUT_MASK_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_mux_frame_num          : 32; /* [31..0] 用于提供LTE的复帧号，由LDSP更新。取值范围是0～1223。配置时刻：生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_MUX_FRAME_NUM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_mux_frame_num          : 32; /* [31..0] 用于提供TDS的复帧号，由TDSP更新。取值范围是0～1223。配置时刻：生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_MUX_FRAME_NUM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_mux_frame_num            : 32; /* [31..0] 用于提供w的复帧号，由wDSP更新。配置时刻：生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_MUX_FRAME_NUM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    meas_flag_l_rd             : 32; /* [31..0] W/TD/GM/GS为主模时写此寄存器，L为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MEAS_FLAG_L_RD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    meas_flag_w_rd             : 32; /* [31..0] L/TD/GM/GS为主模时写此寄存器，W为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MEAS_FLAG_W_RD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    meas_flag_t_rd             : 32; /* [31..0] W/L/GM/GS为主模时写此寄存器，TD为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MEAS_FLAG_T_RD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    meas_flag_gm_rd            : 32; /* [31..0] W/TD/L/GS为主模时写此寄存器，GM为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MEAS_FLAG_GM_RD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    meas_flag_gs_rd            : 32; /* [31..0] W/TD/GM/L为主模时写此寄存器，GS为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MEAS_FLAG_GS_RD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wake_flag_l_wr             : 32; /* [31..0] L为从模时写此寄存器，W/TD/GM/GS主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WAKE_FLAG_L_WR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wake_flag_w_wr             : 32; /* [31..0] W为从模时写此寄存器，L/TD/GM/GS主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WAKE_FLAG_W_WR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wake_flag_t_wr             : 32; /* [31..0] TD为从模时写此寄存器，W/L/GM/GS主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WAKE_FLAG_T_WR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wake_flag_gm_wr            : 32; /* [31..0] GM为从模时写此寄存器，W/TD/L/GS主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WAKE_FLAG_GM_WR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wake_flag_gs_wr            : 32; /* [31..0] GS为从模时写此寄存器，W/TD/GM/L主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WAKE_FLAG_GS_WR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    timing_get_ind             : 1; /* [0..0] 逻辑自清零。DSP需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS的系统定时，CTU的内部定时供DSP查询。配置时刻：软件需要锁存定时生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TIMING_GET_IND_T;    /* 定时锁存使能指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    timing_clear               : 1; /* [0..0] 逻辑自清零。定时标志清除，用于清除timing_valid标志。配置时刻：DSP读取定时完毕后配置生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TIMING_CLEAR_T;    /* 定时标志清除寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_timing_valid             : 1; /* [0..0] 定时上报有效标志，高有效。DSP配置timing_get_ind后，查询此标志，如果为高，则可以获取LTE，W，Gm，Gs, TDS的系统定时。DSP读取定时完毕后配置timing_clear寄存器，timing_valid拉低为无效。CTU检测到timing_get_ind后固定延迟delay_cnt后拉高timing_valid。配置时刻：生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TIMING_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_clk_cnt_dsp              : 4; /* [3..0] [3:0]为chip内时钟计数，范围0~15 */
        unsigned int    w_chip_cnt_dsp             : 12; /* [15..4] [15:4]为时隙内chip计数，范围0~2559 */
        unsigned int    w_slot_cnt_dsp             : 4; /* [19..16] [19:16]为时隙计数，范围0~14 */
        unsigned int    w_sfn_dsp                  : 12; /* [31..20] W的系统定时信息，由DSP 配置timing_get_ind触发上报。上报时刻：timing_valid为高时上报有效.[31:20]表示W的SFN计数（源自w_timing_b[11:0]），范围0~4095。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_TIMING_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_gm_fn_dsp                : 22; /* [21..0] GM的FN计数（源自gm_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 10; /* [31..22] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_FN_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_gm_qb_dsp                : 13; /* [12..0] GM的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。 [12:0]表示 G系统定时帧内Qb计数（源自gm_timing_b[12:0]），范围0~4999。上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 19; /* [31..13] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_QB_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gs_fn_dsp                  : 22; /* [21..0] GS的FN计数（源自gs_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 10; /* [31..22] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_FN_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_gs_qb_dsp                : 13; /* [12..0] GS的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。 [12:0]表示 GS系统定时帧内Qb计数（源自gs_timing_b[12:0]），范围0~4999。上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 19; /* [31..13] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_QB_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_lte_sys_ts_cnt_dsp       : 15; /* [14..0] [14:0]是子帧内计数。 */
        unsigned int    reserved_1                 : 1; /* [15..15] 保留 */
        unsigned int    o_lte_sys_subfrm_cnt_dsp   : 4; /* [19..16] [19:16]是子帧计数 */
        unsigned int    o_lte_sfn_dsp              : 10; /* [29..20] LTE的系统定时。由DSP 配置timing_get_ind触发上报。上报时刻：timing_valid为高时上报有效。其中:[29:20]表示SFN计数，范围0~1023 */
        unsigned int    reserved_0                 : 2; /* [31..30] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_TIMING_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_tds_timing1_dsp          : 16; /* [15..0] TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是TDS的tcfsr系统子帧号0~65535循环计数，TDS实际有效子帧范围为0~8191上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_TIMING1_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_tds_timing2_dsp          : 18; /* [17..0] TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是是TDS的framc帧计数上报时刻：timing_valid为高时上报有效 */
        unsigned int    reserved                   : 14; /* [31..18] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_TIMING2_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_ctu_timing_dsp           : 32; /* [31..0] CTU内部定时，由DSP 配置timing_get_ind触发上报。计数器使用总线钟上报时刻：timing_valid为高时上报有效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CTU_TIMING_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_ctu_basecnt1_dsp         : 32; /* [31..0] CTU内部定时计数器，有时钟就计数，计满从头开始。计数器使用总线钟上报时刻：随时读取。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CTU_BASECNT1_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_ctu_basecnt2_dsp         : 32; /* [31..0] CTU内部定时。使用clk_19m_bbp_soc时钟计数。各主模在int0、int1、int2的时刻触发上报。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CTU_BASECNT2_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ctu_basecnt_dsp            : 32; /* [31..0] CTU内部19.2M钟驱动定时计数器上报结果 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CTU_BASECNT_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_ctu_timing_get_ind     : 1; /* [0..0] CTU内部19.2M钟驱动定时计数器结果读取使能 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    ctu_timing_valid_dsp       : 1; /* [0..0] CTU内部19.2M钟驱动定时计数器结果读取有效指示 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_CTU_TIMING_VALID_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_ctu_timing_clr         : 1; /* [0..0] CTU内部19.2M钟驱动定时计数器结果读取后清零使能 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_CTU_TIMING_CLR_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_w_intrasys_valid         : 1; /* [0..0] w主模下控制RF指示，电平信号。1’b0: W放弃RF控制权。1’b1: W享有控制RF权（但实际配置RF需w_intrasys_valid_delay信号为1）。 */
        unsigned int    o_w_intrasys_valid_delay   : 1; /* [1..1] w主模下实际控制RF指示，电平信号。为1代表W控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。w_intrasys_valid_delay的上升沿比w_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_INTRASYS_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_gm_intrasys_valid        : 1; /* [0..0] gm主模下控制RF指示，电平信号。1’b0: gm放弃RF控制权。1’b1: gm享有控制RF权（但实际配置RF需gm_intrasys_valid_delay信号为1）。 */
        unsigned int    o_gm_intrasys_valid_delay  : 1; /* [1..1] gm主模下实际控制RF指示，电平信号。为1代表gm控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。gm_intrasys_valid_delay的上升沿比gm_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_INTRASYS_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_gs_intrasys_valid        : 1; /* [0..0] gs主模下控制RF指示，电平信号。1’b0: gs放弃RF控制权。1’b1: gs享有控制RF权（但实际配置RF需gs_intrasys_valid_delay信号为1）。 */
        unsigned int    o_gs_intrasys_valid_delay  : 1; /* [1..1] gs主模下实际控制RF指示，电平信号。为1代表gs控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。gs_intrasys_valid_delay的上升沿比gs_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_INTRASYS_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_lte_intrasys_valid       : 1; /* [0..0] Lte主模下控制RF指示，电平信号。1’b0: LTE放弃RF控制权。1’b1: LTE享有控制RF权（但实际配置RF需lte_intrasys_valid_delay信号为1）。 */
        unsigned int    o_lte_intrasys_valid_delay : 1; /* [1..1] LTE主模下实际控制RF指示，电平信号。为1代表LTE控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。lte_intrasys_valid_delay的上升沿比lte_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_INTRASYS_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_tds_intrasys_valid       : 1; /* [0..0] TDS主模下控制RF指示，电平信号。1’b0: TDS放弃RF控制权。1’b1: TDS享有控制RF权（但实际配置RF需tds_intrasys_valid_delay信号为1）。 */
        unsigned int    o_tds_intrasys_valid_delay : 1; /* [1..1] TDS主模下实际控制RF指示，电平信号。为1代表TDS控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。tds_intrasys_valid_delay的上升沿比tds_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_INTRASYS_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    cpu_rfic0_ck_cfg           : 3; /* [2..0] SSI1master等待时间 */
        unsigned int    cpu_rfic1_ck_cfg           : 3; /* [5..3] SSI0master等待时间 */
        unsigned int    reserved                   : 26; /* [31..6]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RFIC_CK_CFG_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi_wait_time         : 8; /* [7..0] 通过xbbp向MIPI master发出回读类指令时,DSP等待回读结果长度配置 */
        unsigned int    dsp_ssi_wait_time          : 8; /* [15..8] 通过xbbp向SSI master发出回读类指令时,DSP等待回读结果长度配置 */
        unsigned int    reserved                   : 16; /* [31..16]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DSP_WAITTIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    s_w_ch_num_ind             : 1; /* [0..0] W为从模时对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    s_w_ch_num_ind_sel         : 1; /* [1..1] W为从模时对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_S_W_CH_NUM_IND_T;    /* W为从模时对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_intersys_measure_type    : 4; /* [3..0] 异系统测量类型。4’b0：测量3G。4’b1: 3G背景搜；4’d2: 3G ANR。其它预留。生效时刻：立即生效 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_measure_report_valid     : 1; /* [0..0] 单频点测量周期到指示，L或者TDS/Gm/Gs作为主模，通知W DSP W测量的单频点周期到达。只在连接态下需要配置。电平信号，高有效。生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_MEASURE_REPORT_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_preint_offset_time       : 32; /* [31..0] 预处理中断int0到int1之间的offset信息，单位us（向下取整）。。配置时刻：主模启动W从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_PREINT_OFFSET_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    w_intersys_measure_time    : 32; /* [31..0] W作为从模时，主模分配给W用于异系统测量时间长度，告知WDSP。单位us，向下取整到us。配置时刻：主模启动W从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_INTERSYS_MEASURE_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_rcv_lte        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_LTE[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_rcv_tds        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_TDS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_rcv_gm         : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GM[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_rcv_gs         : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_rcv_w          : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_W[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_w_int012         : 3; /* [2..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_W_INT012[2:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_W_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_rcv_lte         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_LTE[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_rcv_tds         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_TDS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_rcv_gm          : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GM[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_rcv_gs          : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_rcv_w           : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_W[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_w_int012          : 3; /* [2..0] 中断屏蔽标志。用于屏蔽INT_TYPE_W_INT012[2:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_W_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_rcv_lte       : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。 LTE（主模）提前通知W异系统测量启动中断。[1]: 中断类型1。 LTE（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 LTE（主模）出GAP中断。[3]: 中断类型3。LTE（从模）测量结束中断。[4]: 中断类型4。LTE（主模）强制W退出异系统测量中断。[5]: 中断类型5。LTE（主模）通知W上报本次测量结果。[6]: 保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留（W收到LTE的相关中断） */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_rcv_tds       : 16; /* [15..0] 中断状态标志0，1，2只有当TDS处于主模时才能产生。[0]: 中断类型0。 TDS（主模）提前通知W异系统测量启动中断。[1]: 中断类型1。 TDS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模）测量结束中断。[4]: 中断类型4。TDS（主模）强制W退出异系统测量中断。[5]: 中断类型5。TDS（主模）通知W上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_rcv_gm        : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。GM（主模）提前通知W异系统测量启动中断。[1]: 中断类型1。GM（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GM（主模）出GAP中断。[3]: 中断类型3。GM（从模）测量结束中断。[4]: 中断类型4。GM（主模）强制W退出异系统测量中断。[5]: 中断类型5。GM（主模）通知W上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当GM处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_rcv_gs        : 16; /* [15..0] [0]: 中断类型0。GS（主模）提前通知W异系统测量启动中断。[1]: 中断类型1。GS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GS（主模）出GAP中断。[3]: 中断类型3。GS（从模）测量结束中断。[4]: 中断类型4。GS（主模）强制W退出异系统测量中断。[5]: 中断类型5。GS（主模）通知W上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_rcv_w         : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。W（主模）提前通知W异频测量启动中断。[1]: 中断类型1。W（主模）进GAP中断，通知W进行异频测量射频通道配置，天线切换等操作。[2]: 中断类型2。W（主模）出GAP中断。[3]: 中断类型3。W（从模，即异频）测量结束中断。[4]: 中断类型4。W（主模）强制W退出异频测量中断。[5]: 中断类型5。W（主模）通知W上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当W处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_w_int012        : 3; /* [2..0] 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。[0]: W主模时，CTU告诉通知WDSP产生int0中断。[1]: W主模时，CTU告诉通知WDSP产生int1中断。[2]: W主模时，CTU告诉通知WDSP产生int2中断。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_W_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    w_snd_lte_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应w_snd_lte_int_15 ~ 7。W DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_w[15:7]）给LDSP。生效时刻：立即生效bit6：对应w_snd_lte_int_6。WDSP通过此寄存器触发类型6中断（int_type_lte_rcv_w[6]）给LDSP。生效时刻：立即生效bit5：对应w_snd_lte_int_5。W主模时通知LDSP L测量周期到达，高有效。W DSP通过此寄存器触发类型5中断（int_type_lte_rcv_w[5]）给LDSP。生效时刻：立即生效bit4：对应w_snd_lte_int_4。W主模时强制退出异系统测量指示。高有效。W DSP通过此寄存器触发类型4中断（int_type_lte_rcv_w[4]）给LDSP。生效时刻：立即生效bit3：对应w_snd_lte_int_3。W测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_w[3]）给LDSP。L主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_SND_LTE_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    w_snd_tds_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应w_snd_tds_int_15 ~ 7。W DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_w[15:7]）给TDSP。生效时刻：立即生效bit6：对应w_snd_tds_int_6。WDSP通过此寄存器触发类型6中断（int_type_tds_rcv_w[6]）给TDSP。生效时刻：立即生效bit5：对应w_snd_tds_int_5。W主模时通知TDSP T测量周期到达，高有效。W DSP通过此寄存器触发类型5中断（int_type_tds_rcv_w[5]）给TDSP。生效时刻：立即生效bit4：对应w_snd_tds_int_4。W主模时强制退出异系统测量指示。高有效。W DSP通过此寄存器触发类型4中断（int_type_tds_rcv_w[4]）给TDSP。生效时刻：立即生效bit3：对应w_snd_tds_int_3。W测量结束指示，高有效。用于触发中断类型3 （int_type_tds_rcv_w[3]）给TDSP。T主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_SND_TDS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    w_snd_gm_int_13            : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应w_snd_gm_int_15 ~ 7。W DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_w[15:7]）给GMDSP。生效时刻：立即生效bit6：对应w_snd_gm_int_6。WDSP通过此寄存器触发类型6中断（int_type_gm_rcv_w[6]）给GMDSP。生效时刻：立即生效bit5：对应w_snd_gm_int_5。W主模时通知GMDSP GM测量周期到达，高有效。W DSP通过此寄存器触发类型5中断（int_type_gm_rcv_w[5]）给GMDSP。生效时刻：立即生效bit4：对应w_snd_gm_int_4。W主模时强制退出异系统测量指示。高有效。W DSP通过此寄存器触发类型4中断（int_type_gm_rcv_w[4]）给GMDSP。生效时刻：立即生效bit3：对应w_snd_gm_int_3。W测量结束指示，高有效。用于触发中断类型3 （int_type_gm_rcv_w[3]）给GMDSP。GM主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_SND_GM_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    w_snd_gs_int_13            : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应w_snd_gs_int_15 ~ 7。W DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_w[15:7]）给GSDSP。生效时刻：立即生效bit6：对应w_snd_gs_int_6。WDSP通过此寄存器触发类型6中断（int_type_gs_rcv_w[6]）给GSDSP。生效时刻：立即生效bit5：对应w_snd_gs_int_5。W主模时通知GSDSP GS测量周期到达，高有效。W DSP通过此寄存器触发类型5中断（int_type_gs_rcv_w[5]）给GSDSP。生效时刻：立即生效bit4：对应w_snd_gs_int_4。W主模时强制退出异系统测量指示。高有效。W DSP通过此寄存器触发类型4中断（int_type_gs_rcv_w[4]）给GSDSP。生效时刻：立即生效bit3：对应w_snd_gs_int_3。W测量结束指示，高有效。用于触发中断类型3 （int_type_gs_rcv_w[3]）给GSDSP。GS主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_SND_GS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    w_snd_w_int_13             : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应w_snd_w_int_15 ~ 7。W DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_w[15:7]）给WDSP。生效时刻：立即生效bit6：对应w_snd_w_int_6。WDSP通过此寄存器触发类型6中断（int_type_w_rcv_w[6]）给WDSP。生效时刻：立即生效bit5：对应w_snd_w_int_5。W主模时通知WDSP W测量周期到达，高有效。W DSP通过此寄存器触发类型5中断（int_type_w_rcv_w[5]）给WDSP。生效时刻：立即生效bit4：对应w_snd_w_int_4。W主模时强制退出异系统测量指示。高有效。W DSP通过此寄存器触发类型4中断（int_type_w_rcv_w[4]）给WDSP。生效时刻：立即生效bit3：对应w_snd_w_int_3。W测量结束指示，高有效。用于触发中断类型3 （int_type_w_rcv_w[3]）给WDSP。W主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_SND_W_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    s_lte_ch_num_ind           : 1; /* [0..0] LTE为从模时对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    s_lte_ch_num_ind_sel       : 1; /* [1..1] LTE为从模时对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_S_LTE_CH_NUM_IND_T;    /* LTE为从模时对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_intersys_measure_type  : 4; /* [3..0] 异系统测量类型。4’b0：测量LTE。4’b1：背景搜LTE。其它预留生效时刻：立即生效 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_measure_report_valid   : 1; /* [0..0] 单频点测量周期到指示，W/Gm/Gs或者TDS作为主模，通知L DSP 测量的单频点周期到达。只在连接态下需要配置，且Gm测L时不需要配置；电平信号，高有效。生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_preint_offset_time     : 32; /* [31..0] 预处理中断int0到int1之间的offset信息，单位us（向下取整）。配置时刻：每次启动LTE从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_PREINT_OFFSET_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_intersys_measure_time  : 32; /* [31..0] LTE作为从模时，主模主模分配给LTE用于异系统测量时间长度,告知LTE DSP。单位us，向下取整配置时刻：每次启动LTE从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_INTERSYS_MEASURE_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_rcv_lte      : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_LTE[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_rcv_tds      : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_TDS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_rcv_gm       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GM[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_rcv_gs       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_rcv_w        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_W[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_lte_int012       : 3; /* [2..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_LTE_INT012[2:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_LTE_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_rcv_lte       : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_LTE[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_rcv_tds       : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_TDS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_rcv_gm        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GM[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_rcv_gs        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_rcv_w         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_W[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_lte_int012        : 3; /* [2..0] 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_INT012[2:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_LTE_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_rcv_w       : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。 W（主模）提前通知LTE异系统测量启动中断。[1]: 中断类型1。 W（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 W（主模）出GAP中断。[3]: 中断类型3。W（从模）测量结束中断。[4]: 中断类型4。W（主模）强制LTE退出异系统测量中断。[5]: 中断类型5。W（主模）通知LTE上报本次测量结果。[6]: 保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_rcv_tds     : 16; /* [15..0] 中断状态标志0，1，2只有当TDS处于主模时才能产生。[0]: 中断类型0。 TDS（主模）提前通知LTE异系统测量启动中断。[1]: 中断类型1。 TDS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模）测量结束中断。[4]: 中断类型4。TDS（主模）强制LTE退出异系统测量中断。[5]: 中断类型5。TDS（主模）通知LTE上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_rcv_gm      : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。GM（主模）提前通知LTE异系统测量启动中断。[1]: 中断类型1。GM（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GM（主模）出GAP中断。[3]: 中断类型3。GM（从模）测量结束中断。[4]: 中断类型4。GM（主模）强制LTE退出异系统测量中断。[5]: 中断类型5。GM（主模）通知LTE上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当GM处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_rcv_gs      : 16; /* [15..0] [0]: 中断类型0。GS（主模）提前通知LTE异系统测量启动中断。[1]: 中断类型1。GS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GS（主模）出GAP中断。[3]: 中断类型3。GS（从模）测量结束中断。[4]: 中断类型4。GS（主模）强制LTE退出异系统测量中断。[5]: 中断类型5。GS（主模）通知LTE上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_rcv_lte     : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。LTE（主模）提前通知LTE异频测量启动中断。[1]: 中断类型1。LTE（主模）进GAP中断，通知LTE进行异频测量射频通道配置，天线切换等操作。[2]: 中断类型2。LTE（主模）出GAP中断。[3]: 中断类型3。LTE（从模，即异频）测量结束中断。[4]: 中断类型4。LTE（主模）强制LTE退出异频测量中断。[5]: 中断类型5。LTE（主模）通知LTE上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当LTE处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_lte_int012      : 3; /* [2..0] 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。[0]: L主模时，CTU告诉通知LDSP产生int0中断。[1]: L主模时，CTU告诉通知LDSP产生int1中断。[2]: L主模时，CTU告诉通知LDSP产生int2中断。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_LTE_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    lte_snd_lte_int_13         : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应lte_snd_lte_int_15 ~ 7。L DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_lte[15:7]）给LDSP。生效时刻：立即生效bit6：对应lte_snd_lte_int_6。LDSP通过此寄存器触发类型6中断（int_type_lte_rcv_lte[6]）给LDSP。生效时刻：立即生效bit5：对应lte_snd_lte_int_5。L主模时通知LDSP L测量周期到达，高有效。L DSP通过此寄存器触发类型5中断（int_type_lte_rcv_lte[5]）给LDSP。生效时刻：立即生效bit4：对应lte_snd_lte_int_4。L主模时强制退出异系统测量指示。高有效。L DSP通过此寄存器触发类型4中断（int_type_lte_rcv_lte[4]）给LDSP。生效时刻：立即生效bit3：对应lte_snd_lte_int_3。L测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_lte[3]）给LDSP。L主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_SND_LTE_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    lte_snd_tds_int_13         : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应lte_snd_tds_int_15 ~ 7。L DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_lte[15:7]）给TDSDSP。生效时刻：立即生效bit6：对应lte_snd_tds_int_6。LDSP通过此寄存器触发类型6中断（int_type_tds_rcv_lte[6]）给TDSDSP。生效时刻：立即生效bit5：对应lte_snd_tds_int_5。L主模时通知TDSDSP TDS测量周期到达，高有效。L DSP通过此寄存器触发类型5中断（int_type_tds_rcv_lte[5]）给TDSDSP。生效时刻：立即生效bit4：对应lte_snd_tds_int_4。L主模时强制退出异系统测量指示。高有效。L DSP通过此寄存器触发类型4中断（int_type_tds_rcv_lte[4]）给TDSDSP。生效时刻：立即生效bit3：对应lte_snd_tds_int_3。L测量结束指示，高有效。用于触发中断类型3 （int_type_tds_rcv_lte[3]）给TDSDSP。TDS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_SND_TDS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    lte_snd_gm_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应lte_snd_gm_int_15 ~ 7。L DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_lte[15:7]）给GMDSP。生效时刻：立即生效bit6：对应lte_snd_gm_int_6。LDSP通过此寄存器触发类型6中断（iint_type_gm_rcv_lte[6]）给GMDSP。生效时刻：立即生效bit5：对应lte_snd_gm_int_5。L主模时通知GMDSP GM测量周期到达，高有效。L DSP通过此寄存器触发类型5中断（int_type_gm_rcv_lte[5]）给GMDSP。生效时刻：立即生效bit4：对应lte_snd_gm_int_4。L主模时强制退出异系统测量指示。高有效。L DSP通过此寄存器触发类型4中断（int_type_gm_rcv_lte[4]）给GMDSP。生效时刻：立即生效bit3：对应lte_snd_gm_int_3。L测量结束指示，高有效。用于触发中断类型3 （int_type_gm_rcv_lte[3]）给GMDSP。GM主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_SND_GM_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    lte_snd_gs_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应lte_snd_gs_int_15 ~ 7。L DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_lte[15:7]）给GSDSP。生效时刻：立即生效bit6：对应lte_snd_gs_int_6。LDSP通过此寄存器触发类型6中断（int_type_gs_rcv_lte[6]）给GSDSP。生效时刻：立即生效bit5：对应lte_snd_gs_int_5。L主模时通知GSDSP GS测量周期到达，高有效。L DSP通过此寄存器触发类型5中断（int_type_gs_rcv_lte[5]）给GSDSP。生效时刻：立即生效bit4：对应lte_snd_gs_int_4。L主模时强制退出异系统测量指示。高有效。L DSP通过此寄存器触发类型4中断（int_type_gs_rcv_lte[4]）给GSDSP。生效时刻：立即生效bit3：对应lte_snd_gs_int_3。L测量结束指示，高有效。用于触发中断类型3 （int_type_gs_rcv_lte[3]）给GSDSP。GS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_SND_GS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    lte_snd_w_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应lte_snd_w_int_15 ~ 7。L DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_lte[15:7]）给WDSP。生效时刻：立即生效bit6：对应lte_snd_w_int_6。LDSP通过此寄存器触发类型6中断（int_type_w_rcv_lte[6]）给WDSP。生效时刻：立即生效bit5：对应lte_snd_w_int_5。L主模时通知WDSP W测量周期到达，高有效。L DSP通过此寄存器触发类型5中断（int_type_w_rcv_lte[5]）给WDSP。生效时刻：立即生效bit4：对应lte_snd_w_int_4。L主模时强制退出异系统测量指示。高有效。L DSP通过此寄存器触发类型4中断（int_type_w_rcv_lte[4]）给WDSP。生效时刻：立即生效bit3：对应lte_snd_w_int_3。L测量结束指示，高有效。用于触发中断类型3 （int_type_w_rcv_lte[3]）给WDSP。W主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_SND_W_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    s_tds_ch_num_ind           : 1; /* [0..0] TDS为从模时对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    s_tds_ch_num_ind_sel       : 1; /* [1..1] TDS为从模时对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_S_TDS_CH_NUM_IND_T;    /* TDS为从模时对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_intersys_measure_type  : 4; /* [3..0] 异系统测量类型 。4’b0： 测量TDS4’b1： 背景搜TDS其它预留生效时刻：立即生效 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_measure_report_valid   : 1; /* [0..0] 单频点测量周期到指示，U/Gm/Gs或者L作为主模，通知T DSP W测量的单频点周期到达。只在连接态下需要配置，且G测TDS时不需要配置；电平信号，高有效。生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_preint_offset_time     : 32; /* [31..0] 预处理中断int0到int1之间的offset信息，单位us（向下取整）。配置时刻：主模启动TDS从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_PREINT_OFFSET_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_intersys_measure_time  : 32; /* [31..0] 测量时间长度，告知TDS DSP。单位us，向下取整到us。配置时刻：主模启动TDS从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_INTERSYS_MEASURE_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_rcv_lte      : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_LTE[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_rcv_tds      : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_TDS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_rcv_gm       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GM[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_rcv_gs       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_rcv_w        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_W[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_tds_int012       : 3; /* [2..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_TDS_INT012[2:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_TDS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_rcv_lte       : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_LTE[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_rcv_tds       : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_TDS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_rcv_gm        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GM[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_rcv_gs        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_rcv_w         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_W[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_tds_int012        : 3; /* [2..0] 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_INT012[2:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_TDS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_rcv_lte     : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。 LTE（主模）提前通知TDS异系统测量启动中断。[1]: 中断类型1。 LTE（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 LTE（主模）出GAP中断。[3]: 中断类型3。LTE（从模）测量结束中断。[4]: 中断类型4。LTE（主模）强制TDS退出异系统测量中断。[5]: 中断类型5。LTE（主模）通知TDS上报本次测量结果。[6]: 保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_rcv_w       : 16; /* [15..0] 中断状态标志0，1，2只有当TDS处于主模时才能产生。[0]: 中断类型0。 TDS（主模）提前通知TDS异系统测量启动中断。[1]: 中断类型1。 TDS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模）测量结束中断。[4]: 中断类型4。TDS（主模）强制TDS退出异系统测量中断。[5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_rcv_gm      : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。GM（主模）提前通知TDS异系统测量启动中断。[1]: 中断类型1。GM（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GM（主模）出GAP中断。[3]: 中断类型3。GM（从模）测量结束中断。[4]: 中断类型4。GM（主模）强制TDS退出异系统测量中断。[5]: 中断类型5。GM（主模）通知TDS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当GM处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_rcv_gs      : 16; /* [15..0] [0]: 中断类型0。GS（主模）提前通知TDS异系统测量启动中断。[1]: 中断类型1。GS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GS（主模）出GAP中断。[3]: 中断类型3。GS（从模）测量结束中断。[4]: 中断类型4。GS（主模）强制TDS退出异系统测量中断。[5]: 中断类型5。GS（主模）通知TDS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_rcv_tds     : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。TDS（主模）提前通知TDS异频测量启动中断。[1]: 中断类型1。TDS（主模）进GAP中断，通知TDS进行异频测量射频通道配置，天线切换等操作。[2]: 中断类型2。TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模，即异频）测量结束中断。[4]: 中断类型4。TDS（主模）强制TDS退出异频测量中断。[5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_tds_int012      : 3; /* [2..0] 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。[0]: T主模时，CTU告诉通知TDSP产生int0中断。[1]: T主模时，CTU告诉通知TDSP产生int1中断。[2]: T主模时，CTU告诉通知TDSP产生int2中断。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_TDS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    tds_snd_lte_int_13         : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应tds_snd_lte_int_15 ~ 7。TDS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_tds[15:7]）给LDSP。生效时刻：立即生效bit6：对应tds_snd_lte_int_6。TDSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_tds[6]）给LDSP。生效时刻：立即生效bit5：对应tds_snd_lte_int_5。TDS主模时通知LDSP L测量周期到达，高有效。TDS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_tds[5]）给LDSP。生效时刻：立即生效bit4：对应tds_snd_lte_int_4。TDS主模时强制退出异系统测量指示。高有效。TDS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_tds[4]）给LDSP。生效时刻：立即生效bit3：对应tds_snd_lte_int_3。TDS测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_tds[3]）给LDSP。L主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_SND_LTE_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    tds_snd_tds_int_13         : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应tds_snd_tds_int_15 ~ 7。TDS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_tds[15:7]）给TDSP。生效时刻：立即生效bit6：对应tds_snd_tds_int_6。TDSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_tds[6]）给TDSP。生效时刻：立即生效bit5：对应tds_snd_tds_int_5。TDS主模时通知TDSP T测量周期到达，高有效。TDS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_tds[5]）给TDSP。生效时刻：立即生效bit4：对应tds_snd_tds_int_4。TDS主模时强制退出异系统测量指示。高有效。TDS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_tds[4]）给TDSP。生效时刻：立即生效bit3：对应tds_snd_tds_int_3。TDS测量结束指示，高有效。用于触发中断类型3 （int_type_tds_rcv_tds[3]）给TDSP。T主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_SND_TDS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    tds_snd_gm_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应tds_snd_gm_int_15 ~ 7。TDS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_tds[15:7]）给GMDSP。生效时刻：立即生效bit6：对应tds_snd_gm_int_6。TDSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_tds[6]）给GMDSP。生效时刻：立即生效bit5：对应tds_snd_gm_int_5。TDS主模时通知GMDSP GM测量周期到达，高有效。TDS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_tds[5]）给GMDSP。生效时刻：立即生效bit4：对应tds_snd_gm_int_4。TDS主模时强制退出异系统测量指示。高有效。TDS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_tds[4]）给GMDSP。生效时刻：立即生效bit3：对应tds_snd_gm_int_3。TDS测量结束指示，高有效。用于触发中断类型3 （int_type_gm_rcv_tds[3]）给GMDSP。GM主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_SND_GM_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    tds_snd_gs_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应tds_snd_gs_int_15 ~ 7。TDS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_tds[15:7]）给GSDSP。生效时刻：立即生效bit6：对应tds_snd_gs_int_6。TDSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_tds[6]）给GSDSP。生效时刻：立即生效bit5：对应tds_snd_gs_int_5。TDS主模时通知GSDSP GS测量周期到达，高有效。TDS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_tds[5]）给GSDSP。生效时刻：立即生效bit4：对应tds_snd_gs_int_4。TDS主模时强制退出异系统测量指示。高有效。TDS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_tds[4]）给GSDSP。生效时刻：立即生效bit3：对应tds_snd_gs_int_3。TDS测量结束指示，高有效。用于触发中断类型3 （int_type_gs_rcv_tds[3]）给GSDSP。GS主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_SND_GS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    tds_snd_w_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应tds_snd_w_int_15 ~ 7。TDS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_tds[15:7]）给WDSP。生效时刻：立即生效bit6：对应tds_snd_w_int_6。TDSDSP通过此寄存器触发类型6中断（int_type_w_rcv_tds[6]）给WDSP。生效时刻：立即生效bit5：对应tds_snd_w_int_5。TDS主模时通知WDSP W测量周期到达，高有效。TDS DSP通过此寄存器触发类型5中断（int_type_w_rcv_tds[5]）给WDSP。生效时刻：立即生效bit4：对应tds_snd_w_int_4。TDS主模时强制退出异系统测量指示。高有效。TDS DSP通过此寄存器触发类型4中断（int_type_w_rcv_tds[4]）给WDSP。生效时刻：立即生效bit3：对应tds_snd_w_int_3。TDS测量结束指示，高有效。用于触发中断类型3 （int_type_w_rcv_tds[3]）给WDSP。W主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_SND_W_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    s_gm_ch_num_ind            : 1; /* [0..0] GM为从模时对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    s_gm_ch_num_ind_sel        : 1; /* [1..1] GM为从模时对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_S_GM_CH_NUM_IND_T;    /* GM为从模时对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    gm_intersys_measure_type   : 4; /* [3..0] 异系统测量类型。4’d0：测量GM RSSI。4’d1: 测量GM bsic verification。4’d2: 测量GM bsic confirm。4’d3: 背景搜GM；4’d4: 2G ANR。其它预留生效时刻：立即生效 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gm_measure_report_valid    : 1; /* [0..0] 单频点测量周期到指示，L或者TDS/W/Gs作为主模，通知GM DSP GM测量的单频点周期到达。只在连接态下需要配置。电平信号，高有效。生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_MEASURE_REPORT_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gm_preint_offset_time      : 32; /* [31..0] 预处理中断int0到int1之间的offset信息，单位us（向下取整）。。配置时刻：主模启动GM从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_PREINT_OFFSET_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gm_intersys_measure_time   : 32; /* [31..0] GM作为从模时，主模分配给GM用于异系统测量时间长度，告知GMDSP。单位us，向下取整到us。配置时刻：主模启动GM从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_INTERSYS_MEASURE_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_rcv_lte       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_LTE[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_rcv_tds       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_TDS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_rcv_gm        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GM[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_rcv_gs        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_rcv_w         : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_W[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gm_int012        : 3; /* [2..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GM_INT012[2:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GM_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_rcv_lte        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_LTE[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_rcv_tds        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_TDS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_rcv_gm         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GM[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_rcv_gs         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_rcv_w          : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_W[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gm_int012         : 3; /* [2..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GM_INT012[2:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GM_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_rcv_lte      : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。 LTE（主模）提前通知GM异系统测量启动中断。[1]: 中断类型1。 LTE（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 LTE（主模）出GAP中断。[3]: 中断类型3。LTE（从模）测量结束中断。[4]: 中断类型4。LTE（主模）强制GM退出异系统测量中断。[5]: 中断类型5。LTE（主模）通知GM上报本次测量结果。[6]: 保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_rcv_tds      : 16; /* [15..0] 中断状态标志0，1，2只有当TDS处于主模时才能产生。[0]: 中断类型0。 TDS（主模）提前通知GM异系统测量启动中断。[1]: 中断类型1。 TDS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模）测量结束中断。[4]: 中断类型4。TDS（主模）强制GM退出异系统测量中断。[5]: 中断类型5。TDS（主模）通知GM上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_rcv_w        : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。W（主模）提前通知GM异系统测量启动中断。[1]: 中断类型1。W（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。W（主模）出GAP中断。[3]: 中断类型3。W（从模）测量结束中断。[4]: 中断类型4。W（主模）强制GM退出异系统测量中断。[5]: 中断类型5。W（主模）通知GM上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当W处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_rcv_gs       : 16; /* [15..0] [0]: 中断类型0。GS（主模）提前通知GM异系统测量启动中断。[1]: 中断类型1。GS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GS（主模）出GAP中断。[3]: 中断类型3。GS（从模）测量结束中断。[4]: 中断类型4。GS（主模）强制GM退出异系统测量中断。[5]: 中断类型5。GS（主模）通知GM上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_rcv_gm       : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。GM（主模）提前通知GM异频测量启动中断。[1]: 中断类型1。GM（主模）进GAP中断，通知GM进行异频测量射频通道配置，天线切换等操作。[2]: 中断类型2。GM（主模）出GAP中断。[3]: 中断类型3。GM（从模，即异频）测量结束中断。[4]: 中断类型4。GM（主模）强制GM退出异频测量中断。[5]: 中断类型5。GM（主模）通知GM上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当GM处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gm_int012       : 3; /* [2..0] 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。[0]: GM主模时，CGMU告诉通知GMDSP产生int0中断。[1]: GM主模时，CGMU告诉通知GMDSP产生int1中断。[2]: GM主模时，CGMU告诉通知GMDSP产生int2中断。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GM_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    gm_snd_lte_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gm_snd_lte_int_15 ~ 7。GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。生效时刻：立即生效bit6：对应gm_snd_lte_int_6。GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。生效时刻：立即生效bit5：对应gm_snd_lte_int_5。GM主模时通知LTEDSP LTE测量周期到达，高有效。GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。生效时刻：立即生效bit4：对应gm_snd_lte_int_4。GM主模时强制退出异系统测量指示。高有效。GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。生效时刻：立即生效bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_SND_LTE_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    gm_snd_tds_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gm_snd_tds_int_15 ~ 7。GM DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gm[15:7]）给TDSDSP。生效时刻：立即生效bit6：对应gm_snd_tds_int_6。GMDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gm[6]）给TDSDSP。生效时刻：立即生效bit5：对应gm_snd_tds_int_5。GM主模时通知TDSDSP tds测量周期到达，高有效。GM DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gm[5]）给TDSDSP。生效时刻：立即生效bit4：对应gm_snd_tds_int_4。GM主模时强制退出异系统测量指示。高有效。GM DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gm[4]）给TDSDSP。生效时刻：立即生效bit3：对应gm_snd_tds_int_3。GM测量结束指示，高有效。用于触发中断类型3 （int_type_tds_rcv_gm[3]）给TDSDSP。tds主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_SND_TDS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gm_snd_gm_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gm_snd_gm_int_15 ~ 7。GM DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gm[15:7]）给GMDSP。生效时刻：立即生效bit6：对应gm_snd_gm_int_6。GMDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gm[6]）给GMDSP。生效时刻：立即生效bit5：对应gm_snd_gm_int_5。GM主模时通知GMDSP GM测量周期到达，高有效。GM DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gm[5]）给GMDSP。生效时刻：立即生效bit4：对应gm_snd_gm_int_4。GM主模时强制退出异系统测量指示。高有效。GM DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gm[4]）给GMDSP。生效时刻：立即生效bit3：对应gm_snd_gm_int_3。GM测量结束指示，高有效。用于触发中断类型3 （int_type_gm_rcv_gm[3]）给GMDSP。GM主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_SND_GM_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gm_snd_gs_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gm_snd_gs_int_15 ~ 7。GM DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gm[15:7]）给GSDSP。生效时刻：立即生效bit6：对应gm_snd_gs_int_6。GMDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gm[6]）给GSDSP。生效时刻：立即生效bit5：对应gm_snd_gs_int_5。GM主模时通知GSDSP GS测量周期到达，高有效。GM DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gm[5]）给GSDSP。生效时刻：立即生效bit4：对应gm_snd_gs_int_4。GM主模时强制退出异系统测量指示。高有效。GM DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gm[4]）给GSDSP。生效时刻：立即生效bit3：对应gm_snd_gs_int_3。GM测量结束指示，高有效。用于触发中断类型3 （int_type_gs_rcv_gm[3]）给GSDSP。GS主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_SND_GS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gm_snd_w_int_13            : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gm_snd_lte_int_15 ~ 7。GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。生效时刻：立即生效bit6：对应gm_snd_lte_int_6。GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。生效时刻：立即生效bit5：对应gm_snd_lte_int_5。GM主模时通知LTEDSP LTE测量周期到达，高有效。GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。生效时刻：立即生效bit4：对应gm_snd_lte_int_4。GM主模时强制退出异系统测量指示。高有效。GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。生效时刻：立即生效bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_SND_W_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    s_gs_ch_num_ind            : 1; /* [0..0] GS为从模时对应的通道选择信号。0：选择通道0；1：选择通道1。配置时刻：各模根据通信场景配置 */
        unsigned int    s_gs_ch_num_ind_sel        : 1; /* [1..1] GS为从模时对应的通道选择信号生效方式。0：在int1生效；1：立即生效。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_S_GS_CH_NUM_IND_T;    /* GS为从模时对应通道的指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    gs_intersys_measure_type   : 4; /* [3..0] 异系统测量类型。4’d0：测量GS RSSI。4’d1: 测量GS bsic verification。4’d2: 测量GS bsic confirm。4’d3: 背景搜GS4’d4: 2G ANR。其它预留生效时刻：立即生效 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gs_measure_report_valid    : 1; /* [0..0] 单频点测量周期到指示，L或者TDS/W/Gm作为主模，通知GS DSP GS测量的单频点周期到达。只在连接态下需要配置。电平信号，高有效。生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_MEASURE_REPORT_VALID_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gs_preint_offset_time      : 32; /* [31..0] 预处理中断int0到int1之间的offset信息，单位us（向下取整）。。配置时刻：主模启动GS从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_PREINT_OFFSET_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gs_intersys_measure_time   : 32; /* [31..0] GS作为从模时，主模分配给GS用于异系统测量时间长度，告知GSDSP。单位us，向下取整到us。配置时刻：主模启动GS从模异系统测量时配置生效时刻：立即生效 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_INTERSYS_MEASURE_TIME_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_rcv_lte       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_LTE[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_rcv_tds       : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_TDS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_rcv_gm        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GM[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_rcv_gs        : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GS[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_rcv_w         : 16; /* [15..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_W[15:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_clear_gs_int012        : 3; /* [2..0] 逻辑自清零。中断标志清除指示，高有效。用于清除INT_TYPE_GS_INT012[2:0]对应bit位生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_CLEAR_GS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_rcv_lte        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_LTE[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_rcv_tds        : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_TDS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_rcv_gm         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GM[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_rcv_gs         : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GS[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_rcv_w          : 16; /* [15..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_W[15:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    int_mask_gs_int012         : 3; /* [2..0] 中断屏蔽标志。用于屏蔽INT_TYPE_GS_INT012[2:0]对应bit中断。0：屏蔽中断上报；1：使能中断上报。配置时刻：初始化生效时刻：立即生效 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_MASK_GS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_rcv_lte      : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。 LTE（主模）提前通知GS异系统测量启动中断。[1]: 中断类型1。 LTE（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 LTE（主模）出GAP中断。[3]: 中断类型3。LTE（从模）测量结束中断。[4]: 中断类型4。LTE（主模）强制GS退出异系统测量中断。[5]: 中断类型5。LTE（主模）通知GS上报本次测量结果。[6]: 保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_rcv_tds      : 16; /* [15..0] 中断状态标志0，1，2只有当TDS处于主模时才能产生。[0]: 中断类型0。 TDS（主模）提前通知GS异系统测量启动中断。[1]: 中断类型1。 TDS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。 TDS（主模）出GAP中断。[3]: 中断类型3。TDS（从模）测量结束中断。[4]: 中断类型4。TDS（主模）强制GS退出异系统测量中断。[5]: 中断类型5。TDS（主模）通知GS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_rcv_gm       : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。GM（主模）提前通知GS异系统测量启动中断。[1]: 中断类型1。GM（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GM（主模）出GAP中断。[3]: 中断类型3。GM（从模）测量结束中断。[4]: 中断类型4。GM（主模）强制GS退出异系统测量中断。[5]: 中断类型5。GM（主模）通知GS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当GM处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_RCV_GM_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_rcv_gs       : 16; /* [15..0] [0]: 中断类型0。GS（主模）提前通知GS异系统测量启动中断。[1]: 中断类型1。GS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。[2]: 中断类型2。GS（主模）出GAP中断。[3]: 中断类型3。GS（从模）测量结束中断。[4]: 中断类型4。GS（主模）强制GS退出异系统测量中断。[5]: 中断类型5。GS（主模）通知GS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当TDS处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_RCV_GS_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_rcv_w        : 16; /* [15..0] 中断状态标志。各bit单独对应一种中断类型，高有效。[0]: 中断类型0。W（主模）提前通知GS异频测量启动中断。[1]: 中断类型1。W（主模）进GAP中断，通知GS进行异频测量射频通道配置，天线切换等操作。[2]: 中断类型2。W（主模）出GAP中断。[3]: 中断类型3。W（从模，即异频）测量结束中断。[4]: 中断类型4。W（主模）强制GS退出异频测量中断。[5]: 中断类型5。W（主模）通知GS上报本次测量结果。[6]: 中断类型6。保留。[15:7]: 对应中断类型[15:7]，预留。中断状态标志0，1，2只有当W处于主模时才能产生。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_RCV_W_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    o_int_type_gs_int012       : 3; /* [2..0] 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。[0]: GS主模时，CTU告诉通知GSDSP产生int0中断。[1]: GS主模时，CTU告诉通知GSDSP产生int1中断。[2]: GS主模时，CTU告诉通知GSDSP产生int2中断。 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_INT_TYPE_GS_INT012_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 GM */
        unsigned int    gs_snd_lte_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gs_snd_lte_int_15 ~ 7。GS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gs[15:7]）给LTEDSP。生效时刻：立即生效bit6：对应gs_snd_lte_int_6。GSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gs[6]）给LTEDSP。生效时刻：立即生效bit5：对应gs_snd_lte_int_5。GS主模时通知LTEDSP LTE测量周期到达，高有效。GS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gs[5]）给LTEDSP。生效时刻：立即生效bit4：对应gs_snd_lte_int_4。GS主模时强制退出异系统测量指示。高有效。GS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gs[4]）给LTEDSP。生效时刻：立即生效bit3：对应gs_snd_lte_int_3。GS测量结束指示，高有效。用于触发中断类型3 （int_type_lte_rcv_gs[3]）给LTEDSP。LTE主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_SND_LTE_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0] 保留 */
        unsigned int    gs_snd_tds_int_13          : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gs_snd_tds_int_15 ~ 7。GS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gs[15:7]）给TDSDSP。生效时刻：立即生效bit6：对应gs_snd_tds_int_6。GSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gs[6]）给TDSDSP。生效时刻：立即生效bit5：对应gs_snd_tds_int_5。GS主模时通知TDSDSP TDS测量周期到达，高有效。GS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gs[5]）给TDSDSP。生效时刻：立即生效bit4：对应gs_snd_tds_int_4。GS主模时强制退出异系统测量指示。高有效。GS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gs[4]）给TDSDSP。生效时刻：立即生效bit3：对应gs_snd_tds_int_3。GS测量结束指示，高有效。用于触发中断类型3 （int_type_tds_rcv_gs[3]）给TDSDSP。TDS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_SND_TDS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gs_snd_gm_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gs_snd_gm_int_15 ~ 7。GS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gs[15:7]）给GMDSP。生效时刻：立即生效bit6：对应gs_snd_gm_int_6。GSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gs[6]）给GMDSP。生效时刻：立即生效bit5：对应gs_snd_gm_int_5。GS主模时通知GMDSP GM测量周期到达，高有效。GS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gs[5]）给GMDSP。生效时刻：立即生效bit4：对应gs_snd_gm_int_4。GS主模时强制退出异系统测量指示。高有效。GS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gs[4]）给GMDSP。生效时刻：立即生效bit3：对应gs_snd_gm_int_3。GS测量结束指示，高有效。用于触发中断类型3 （int_type_gm_rcv_gs[3]）给GMDSP。GM主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_SND_GM_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gs_snd_gs_int_13           : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gs_snd_gs_int_15 ~ 7。GS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gs[15:7]）给GSDSP。生效时刻：立即生效bit6：对应gs_snd_gs_int_6。GSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gs[6]）给GSDSP。生效时刻：立即生效bit5：对应gs_snd_gs_int_5。GS主模时通知GSDSP GS测量周期到达，高有效。GS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gs[5]）给GSDSP。生效时刻：立即生效bit4：对应gs_snd_gs_int_4。GS主模时强制退出异系统测量指示。高有效。GS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gs[4]）给GSDSP。生效时刻：立即生效bit3：对应gs_snd_gs_int_3。GS测量结束指示，高有效。用于触发中断类型3 （int_type_gs_rcv_gs[3]）给GSDSP。GS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_SND_GS_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 3; /* [2..0]  */
        unsigned int    gs_snd_w_int_13            : 13; /* [15..3] 逻辑自清零。高有效。bit15~7：对应gs_snd_w_int_15 ~ 7。GS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_gs[15:7]）给WDSP。生效时刻：立即生效bit6：对应gs_snd_w_int_6。GSDSP通过此寄存器触发类型6中断（int_type_w_rcv_gs[6]）给WDSP。生效时刻：立即生效bit5：对应gs_snd_w_int_5。GS主模时通知WDSP W测量周期到达，高有效。GS DSP通过此寄存器触发类型5中断（int_type_w_rcv_gs[5]）给WDSP。生效时刻：立即生效bit4：对应gs_snd_w_int_4。GS主模时强制退出异系统测量指示。高有效。GS DSP通过此寄存器触发类型4中断（int_type_w_rcv_gs[4]）给WDSP。生效时刻：立即生效bit3：对应gs_snd_w_int_3。GS测量结束指示，高有效。用于触发中断类型3 （int_type_w_rcv_gs[3]）给WDSP。W主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 生效时刻：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_SND_W_INT_13_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    gbbp1_19m_sel              : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GBBP1_19M_SEL_T;

typedef union
{
    struct
    {
        unsigned int    gbbp2_19m_sel              : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GBBP2_19M_SEL_T;

typedef union
{
    struct
    {
        unsigned int    wbbp_19m_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WBBP_19M_SEL_T;

typedef union
{
    struct
    {
        unsigned int    lbbp_19m_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LBBP_19M_SEL_T;

typedef union
{
    struct
    {
        unsigned int    tbbp_19m_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TBBP1_19M_SEL_T;

typedef union
{
    struct
    {
        unsigned int    gbbp1_clk_sel              : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GBBP1_CLK_SEL_T;

typedef union
{
    struct
    {
        unsigned int    gbbp2_clk_sel              : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GBBP2_CLK_SEL_T;

typedef union
{
    struct
    {
        unsigned int    wbbp_clk_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_WBBP_CLK_SEL_T;

typedef union
{
    struct
    {
        unsigned int    lbbp_clk_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LBBP_CLK_SEL_T;

typedef union
{
    struct
    {
        unsigned int    tbbp_clk_sel               : 2; /* [1..0] V9R1版本，软件不配置。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TBBP_CLK_SEL_T;

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    lte_rxb_iq_exchange        : 1; /* [1..1] 1'b1：LTE的RXB通路IQ路数据交换1'b0：LTE的RXB通路IQ路数据不交换 */
        unsigned int    lte_rxb_q_inv              : 1; /* [2..2] 1'b1：LTE的RXB通路Q路数据取反1'b0：LTE的RXB通路Q路数据不取反 */
        unsigned int    lte_rxb_i_inv              : 1; /* [3..3] 1'b1：LTE的RXB通路I路数据取反1'b0：LTE的RXB通路I路数据不取反 */
        unsigned int    lte_rxa_iq_exchange        : 1; /* [4..4] 1'b1：LTE的RXA通路IQ路数据交换1'b0：LTE的RXA通路IQ路数据不交换 */
        unsigned int    lte_rxa_q_inv              : 1; /* [5..5] 1'b1：LTE的RXA通路Q路数据取反1'b0：LTE的RXA通路Q路数据不取反 */
        unsigned int    lte_rxa_i_inv              : 1; /* [6..6] 1'b1：LTE的RXA通路I路数据取反1'b0：LTE的RXA通路I路数据不取反 */
        unsigned int    lte_tx_iq_exchange         : 1; /* [7..7] 1'b1：LTE的TX通路IQ路数据交换1'b0：LTE的TX通路IQ路数据不交换 */
        unsigned int    lte_tx_q_inv               : 1; /* [8..8] 1'b1：LTE的TX通路Q路数据取反1'b0：LTE的TX通路Q路数据不取反 */
        unsigned int    lte_tx_i_inv               : 1; /* [9..9] 1'b1：LTE的TX通路I路数据取反1'b0：LTE的TX通路I路数据不取反 */
        unsigned int    reserved_0                 : 22; /* [31..10] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_LTE_ABBIF_FMT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    w_rxb_iq_exchange          : 1; /* [1..1] 1'b1：W的RXB通路IQ路数据交换1'b0：W的RXB通路IQ路数据不交换 */
        unsigned int    w_rxb_q_inv                : 1; /* [2..2] 1'b1：W的RXB通路Q路数据取反1'b0：W的RXB通路Q路数据不取反 */
        unsigned int    w_rxb_i_inv                : 1; /* [3..3] 1'b1：W的RXB通路I路数据取反1'b0：W的RXB通路I路数据不取反 */
        unsigned int    w_rxa_iq_exchange          : 1; /* [4..4] 1'b1：W的RXA通路IQ路数据交换1'b0：W的RXA通路IQ路数据不交换 */
        unsigned int    w_rxa_q_inv                : 1; /* [5..5] 1'b1：W的RXA通路Q路数据取反1'b0：W的RXA通路Q路数据不取反 */
        unsigned int    w_rxa_i_inv                : 1; /* [6..6] 1'b1：W的RXA通路I路数据取反1'b0：W的RXA通路I路数据不取反 */
        unsigned int    w_tx_iq_exchange           : 1; /* [7..7] 1'b1：W的TX通路IQ路数据交换1'b0：W的TX通路IQ路数据不交换 */
        unsigned int    w_tx_q_inv                 : 1; /* [8..8] 1'b1：W的TX通路Q路数据取反1'b0：W的TX通路Q路数据不取反 */
        unsigned int    w_tx_i_inv                 : 1; /* [9..9] 1'b1：W的TX通路I路数据取反1'b0：W的TX通路I路数据不取反。 */
        unsigned int    reserved_0                 : 22; /* [31..10] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_ABBIF_FMT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    tds_rxb_iq_exchange        : 1; /* [1..1] 1'b1：TDS的RXB通路IQ路数据交换1'b0：TDS的RXB通路IQ路数据不交换 */
        unsigned int    tds_rxb_q_inv              : 1; /* [2..2] 1'b1：TDS的RXB通路Q路数据取反1'b0：TDS的RXB通路Q路数据不取反 */
        unsigned int    tds_rxb_i_inv              : 1; /* [3..3] 1'b1：TDS的RXB通路I路数据取反1'b0：TDS的RXB通路I路数据不取反 */
        unsigned int    tds_rxa_iq_exchange        : 1; /* [4..4] 1'b1：TDS的RXA通路IQ路数据交换1'b0：TDS的RXA通路IQ路数据不交换 */
        unsigned int    tds_rxa_q_inv              : 1; /* [5..5] 1'b1：TDS的RXA通路Q路数据取反1'b0：TDS的RXA通路Q路数据不取反 */
        unsigned int    tds_rxa_i_inv              : 1; /* [6..6] 1'b1：TDS的RXA通路I路数据取反1'b0：TDS的RXA通路I路数据不取反 */
        unsigned int    tds_tx_iq_exchange         : 1; /* [7..7] 1'b1：TDS的TX通路IQ路数据交换1'b0：TDS的TX通路IQ路数据不交换 */
        unsigned int    tds_tx_q_inv               : 1; /* [8..8] 1'b1：TDS的TX通路Q路数据取反1'b0：TDS的TX通路Q路数据不取反 */
        unsigned int    tds_tx_i_inv               : 1; /* [9..9] 1'b1：TDS的TX通路I路数据取反1'b0：TDS的TX通路I路数据不取反 */
        unsigned int    reserved_0                 : 22; /* [31..10] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_TDS_ABBIF_FMT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    gm_rxb_iq_exchange         : 1; /* [1..1] 1'b1：GM的RXB通路IQ路数据交换1'b0：GM的RXB通路IQ路数据不交换 */
        unsigned int    gm_rxb_q_inv               : 1; /* [2..2] 1'b1：GM的RXB通路Q路数据取反1'b0：GM的RXB通路Q路数据不取反 */
        unsigned int    gm_rxb_i_inv               : 1; /* [3..3] 1'b1：GM的RXB通路I路数据取反1'b0：GM的RXB通路I路数据不取反 */
        unsigned int    gm_rxa_iq_exchange         : 1; /* [4..4] 1'b1：GM的RXA通路IQ路数据交换1'b0：GM的RXA通路IQ路数据不交换 */
        unsigned int    gm_rxa_q_inv               : 1; /* [5..5] 1'b1：GM的RXA通路Q路数据取反1'b0：GM的RXA通路Q路数据不取反 */
        unsigned int    gm_rxa_i_inv               : 1; /* [6..6] 1'b1：GM的RXA通路I路数据取反1'b0：GM的RXA通路I路数据不取反 */
        unsigned int    gm_tx_iq_exchange          : 1; /* [7..7] 1'b1：GM的TX通路IQ路数据交换1'b0：GM的TX通路IQ路数据不交换 */
        unsigned int    gm_tx_q_inv                : 1; /* [8..8] 1'b1：GM的TX通路Q路数据取反1'b0：GM的TX通路Q路数据不取反 */
        unsigned int    gm_tx_i_inv                : 1; /* [9..9] 1'b1：GM的TX通路I路数据取反1'b0：GM的TX通路I路数据不取反 */
        unsigned int    reserved_0                 : 22; /* [31..10] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_ABBIF_FMT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    gs_rxb_iq_exchange         : 1; /* [1..1] 1'b1：GS的RXB通路IQ路数据交换1'b0：GS的RXB通路IQ路数据不交换 */
        unsigned int    gs_rxb_q_inv               : 1; /* [2..2] 1'b1：GS的RXB通路Q路数据取反1'b0：GS的RXB通路Q路数据不取反 */
        unsigned int    gs_rxb_i_inv               : 1; /* [3..3] 1'b1：GS的RXB通路I路数据取反1'b0：GS的RXB通路I路数据不取反 */
        unsigned int    gs_rxa_iq_exchange         : 1; /* [4..4] 1'b1：GS的RXA通路IQ路数据交换1'b0：GS的RXA通路IQ路数据不交换 */
        unsigned int    gs_rxa_q_inv               : 1; /* [5..5] 1'b1：GS的RXA通路Q路数据取反1'b0：GS的RXA通路Q路数据不取反 */
        unsigned int    gs_rxa_i_inv               : 1; /* [6..6] 1'b1：GS的RXA通路I路数据取反1'b0：GS的RXA通路I路数据不取反 */
        unsigned int    gs_tx_iq_exchange          : 1; /* [7..7] 1'b1：GS的TX通路IQ路数据交换1'b0：GS的TX通路IQ路数据不交换 */
        unsigned int    gs_tx_q_inv                : 1; /* [8..8] 1'b1：GS的TX通路Q路数据取反1'b0：GS的TX通路Q路数据不取反 */
        unsigned int    gs_tx_i_inv                : 1; /* [9..9] 1'b1：GS的TX通路I路数据取反1'b0：GS的TX通路I路数据不取反 */
        unsigned int    reserved_0                 : 22; /* [31..10] 保留。下列配置为调试用，默认值为0. */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_ABBIF_FMT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg0_for_use               : 32; /* [31..0] 寄存器3，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG0_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg1_for_use               : 32; /* [31..0] 寄存器4，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG1_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg2_for_use               : 32; /* [31..0] 寄存器4，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG2_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg3_for_use               : 32; /* [31..0] 寄存器6，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG3_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg4_for_use               : 32; /* [31..0] 寄存器7，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG4_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg5_for_use               : 32; /* [31..0] 寄存器8，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG5_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg6_for_use               : 32; /* [31..0] 寄存器9，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG6_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg7_for_use               : 32; /* [31..0] 寄存器7，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG7_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg8_for_use               : 32; /* [31..0] 寄存器8，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG8_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg9_for_use               : 32; /* [31..0] 寄存器9，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG9_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg10_for_use              : 32; /* [31..0] 寄存器10，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG10_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg11_for_use              : 32; /* [31..0] 寄存器11，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG11_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg12_for_use              : 32; /* [31..0] 寄存器12，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG12_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg13_for_use              : 32; /* [31..0] 寄存器13，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG13_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg14_for_use              : 32; /* [31..0] 寄存器14，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG14_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg15_for_use              : 32; /* [31..0] 寄存器15，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG15_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg16_for_use              : 32; /* [31..0] 寄存器16，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG16_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg17_for_use              : 32; /* [31..0] 寄存器17，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG17_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg18_for_use              : 32; /* [31..0] 寄存器18，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG18_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg19_for_use              : 32; /* [31..0] 寄存器19，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG19_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg20_for_use              : 32; /* [31..0] 寄存器20，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG20_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg21_for_use              : 32; /* [31..0] 寄存器21，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG21_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg22_for_use              : 32; /* [31..0] 寄存器22，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG22_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg23_for_use              : 32; /* [31..0] 寄存器23，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG23_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg24_for_use              : 32; /* [31..0] 寄存器24，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG24_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg25_for_use              : 32; /* [31..0] 寄存器25，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG25_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg26_for_use              : 32; /* [31..0] 寄存器26，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG26_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg27_for_use              : 32; /* [31..0] 寄存器27，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG27_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg28_for_use              : 32; /* [31..0] 寄存器28，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG28_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg29_for_use              : 32; /* [31..0] 寄存器27，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG29_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg30_for_use              : 32; /* [31..0] 寄存器28，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG30_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg31_for_use              : 32; /* [31..0] 寄存器28，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG31_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg32_for_use              : 32; /* [31..0] 寄存器32，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG32_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg33_for_use              : 32; /* [31..0] 寄存器33，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG33_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg34_for_use              : 32; /* [31..0] 寄存器34，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG34_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg35_for_use              : 32; /* [31..0] 寄存器35，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG35_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg36_for_use              : 32; /* [31..0] 寄存器36，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG36_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg37_for_use              : 32; /* [31..0] 寄存器37，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG37_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg38_for_use              : 32; /* [31..0] 寄存器38，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG38_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg39_for_use              : 32; /* [31..0] 寄存器39，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG39_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg40_for_use              : 32; /* [31..0] 寄存器40，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG40_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg41_for_use              : 32; /* [31..0] 寄存器41，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG41_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg42_for_use              : 32; /* [31..0] 寄存器42，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG42_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg43_for_use              : 32; /* [31..0] 寄存器43，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG43_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg44_for_use              : 32; /* [31..0] 寄存器44，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG44_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg45_for_use              : 32; /* [31..0] 寄存器45，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG45_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg46_for_use              : 32; /* [31..0] 寄存器46，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG46_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    reg47_for_use              : 32; /* [31..0] 寄存器47，预留。供DSP之间传递信息。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_REG47_FOR_USE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg0_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG0_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg1_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG1_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg2_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG2_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg3_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG3_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg4_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG4_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg5_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG5_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg6_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG6_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg7_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG7_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg8_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG8_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg9_cpu               : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG9_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg10_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG10_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg11_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG11_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg12_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG12_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg13_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG13_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg14_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG14_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg15_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG15_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg16_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG16_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg17_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG17_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg18_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG18_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_reg19_cpu              : 32; /* [31..0]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG19_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_dbg_clk_gate_en        : 1; /* [0..0] bbp_comm_dbg模块门控时钟使能信号：1：直接使用输入时钟，未进行门控；0：输入时钟无效，此时进行低功耗状态。 */
        unsigned int    dsp_dbg_clk_bypass         : 1; /* [1..1] bbp_comm_dbg模块时钟bypass配置信号：1：直接使用输入时钟，不进行门控处理，此时dsp_dbg_clk_gate_en配置无效；0：表示根据dsp_dbg_clk_gate_en进行门控处理。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_CLK_GATE_EN_T;    /* 门控时钟配置寄存器 */

typedef union
{
    struct
    {
        unsigned int    com2tds_busy_rf_ssi1       : 1; /* [0..0] TDS SSI1忙标志。 */
        unsigned int    com2tds_busy_rf_ssi0       : 1; /* [1..1] TDS SSI0忙标志。 */
        unsigned int    com2w_busy_rf_ssi1         : 1; /* [2..2] W SSI1忙标志。 */
        unsigned int    com2w_busy_rf_ssi0         : 1; /* [3..3] W SSI0忙标志。 */
        unsigned int    com2lte_busy_rf_ssi1       : 1; /* [4..4] LTE SSI1忙标志。 */
        unsigned int    com2lte_busy_rf_ssi0       : 1; /* [5..5] LTE SSI0忙标志。 */
        unsigned int    com2g2_busy_rf_ssi1        : 1; /* [6..6] G2 SSI1忙标志。 */
        unsigned int    com2g2_busy_rf_ssi0        : 1; /* [7..7] G2 SSI0忙标志。 */
        unsigned int    com2g1_busy_rf_ssi1        : 1; /* [8..8] G1 SSI1忙标志。 */
        unsigned int    com2g1_busy_rf_ssi0        : 1; /* [9..9] G1 SSI0忙标志。 */
        unsigned int    com2tds_busy_mipi1         : 1; /* [10..10] TDS MIPI1忙标志。 */
        unsigned int    com2tds_busy_mipi0         : 1; /* [11..11] TDS MIPI0忙标志。 */
        unsigned int    com2w_busy_mipi1           : 1; /* [12..12] W MIPI1忙标志。 */
        unsigned int    com2w_busy_mipi0           : 1; /* [13..13] W MIPI0忙标志。 */
        unsigned int    com2lte_busy_mipi1         : 1; /* [14..14] LTE MIPI1忙标志。 */
        unsigned int    com2lte_busy_mipi0         : 1; /* [15..15] LTE MIPI0忙标志。 */
        unsigned int    com2g2_busy_mipi1          : 1; /* [16..16] G2 MIPI1忙标志。 */
        unsigned int    com2g2_busy_mipi0          : 1; /* [17..17] G2 MIPI0忙标志。 */
        unsigned int    com2g1_busy_mipi1          : 1; /* [18..18] G1 MIPI1忙标志。 */
        unsigned int    com2g1_busy_mipi0          : 1; /* [19..19] G1 MIPI0忙标志。 */
        unsigned int    reserved                   : 12; /* [31..20]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_REG20_CPU_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_comm_en                : 1; /* [0..0] bbp_comm_dbg使能信号：1：使能；0：不使能。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_COMM_EN_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    l_ch_sw_rf                 : 1; /* [0..0] rf相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    l_ch_sw_mipi               : 1; /* [1..1] mipi相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    l_ch_sw_abb_tx             : 1; /* [2..2] abb tx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    l_ch_sw_abb_rx             : 1; /* [3..3] abb rx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    l_ch_sw_pmu                : 1; /* [4..4] pmu相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    l_ch_sw_apt                : 1; /* [5..5] apt相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_L_CH_SW_T;

typedef union
{
    struct
    {
        unsigned int    w_ch_sw_rf                 : 1; /* [0..0] rf相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    w_ch_sw_mipi               : 1; /* [1..1] mipi相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    w_ch_sw_abb_tx             : 1; /* [2..2] abb tx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    w_ch_sw_abb_rx             : 1; /* [3..3] abb rx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    w_ch_sw_pmu                : 1; /* [4..4] pmu相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    w_ch_sw_apt                : 1; /* [5..5] apt相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_CH_SW_T;

typedef union
{
    struct
    {
        unsigned int    t_ch_sw_rf                 : 1; /* [0..0] rf相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    t_ch_sw_mipi               : 1; /* [1..1] mipi相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    t_ch_sw_abb_tx             : 1; /* [2..2] abb tx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    t_ch_sw_abb_rx             : 1; /* [3..3] abb rx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    t_ch_sw_pmu                : 1; /* [4..4] pmu相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    t_ch_sw_apt                : 1; /* [5..5] apt相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_T_CH_SW_T;

typedef union
{
    struct
    {
        unsigned int    gm_ch_sw_rf                : 1; /* [0..0] rf相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gm_ch_sw_mipi              : 1; /* [1..1] mipi相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gm_ch_sw_abb_tx            : 1; /* [2..2] abb tx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gm_ch_sw_abb_rx            : 1; /* [3..3] abb rx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gm_ch_sw_pmu               : 1; /* [4..4] pmu相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gm_ch_sw_apt               : 1; /* [5..5] apt相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_CH_SW_T;

typedef union
{
    struct
    {
        unsigned int    gs_ch_sw_rf                : 1; /* [0..0] rf相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gs_ch_sw_mipi              : 1; /* [1..1] mipi相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gs_ch_sw_abb_tx            : 1; /* [2..2] abb tx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gs_ch_sw_abb_rx            : 1; /* [3..3] abb rx相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gs_ch_sw_pmu               : 1; /* [4..4] pmu相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    gs_ch_sw_apt               : 1; /* [5..5] apt相关信号是否交换：1'b0：0/1通道不交换1'b1：0/1通道交换 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_CH_SW_T;

typedef union
{
    struct
    {
        unsigned int    l_tcvr_sel0                : 1; /* [0..0] lte模输出的通道0的tcvr_on信号来源：1'b0：来自于lte bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_L_TCVR_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    l_tcvr_sel1                : 1; /* [0..0] lte模输出的通道1的tcvr_on信号来源：1'b0：来自于lte bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_L_TCVR_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    t_tcvr_sel0                : 1; /* [0..0] tds模输出的通道0的tcvr_on信号来源：1'b0：来自于tds bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_T_TCVR_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    t_tcvr_sel1                : 1; /* [0..0] tds模输出的通道1的tcvr_on信号来源：1'b0：来自于tds bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_T_TCVR_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    w_tcvr_sel0                : 1; /* [0..0] wcdma模输出的通道0的tcvr_on信号来源：1'b0：来自于wcdma bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_TCVR_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    w_tcvr_sel1                : 1; /* [0..0] wcdma模输出的通道1的tcvr_on信号来源：：1'b0：来自于wcdma bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_TCVR_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    gm_tcvr_sel0               : 1; /* [0..0] gsm（m）输出的通道0的tcvr_on信号来源：1'b0：来自于gsm（m） bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_TCVR_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    gm_tcvr_sel1               : 1; /* [0..0] gsm（m）输出的通道1的tcvr_on信号来源：1'b0：来自于gsm（m） bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_TCVR_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    gs_tcvr_sel0               : 1; /* [0..0] gsm（s）输出的通道0的tcvr_on信号来源：1'b0：来自于gsm（s） bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_TCVR_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    gs_tcvr_sel1               : 1; /* [0..0] gsm（s）输出的通道1的tcvr_on信号来源：1'b0：来自于gsm（s） bbp1'b1：来自于dsp配置值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_TCVR_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    l_tcvr_value0              : 1; /* [0..0] dsp配置的lte模送给通道0 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_L_TCVR_VALUE0_T;

typedef union
{
    struct
    {
        unsigned int    l_tcvr_value1              : 1; /* [0..0] dsp配置的lte模送给通道1 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_L_TCVR_VALUE1_T;

typedef union
{
    struct
    {
        unsigned int    t_tcvr_value0              : 1; /* [0..0] dsp配置的tds模送给通道0 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_T_TCVR_VALUE0_T;

typedef union
{
    struct
    {
        unsigned int    t_tcvr_value1              : 1; /* [0..0] dsp配置的tds模送给通道1 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_T_TCVR_VALUE1_T;

typedef union
{
    struct
    {
        unsigned int    w_tcvr_value0              : 1; /* [0..0] dsp配置的wcdma模送给通道0 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_TCVR_VALUE0_T;

typedef union
{
    struct
    {
        unsigned int    w_tcvr_value1              : 1; /* [0..0] dsp配置的wcdma模送给通道1 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_W_TCVR_VALUE1_T;

typedef union
{
    struct
    {
        unsigned int    gm_tcvr_value0             : 1; /* [0..0] dsp配置的gsm（m）模送给通道0 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_TCVR_VALUE0_T;

typedef union
{
    struct
    {
        unsigned int    gm_tcvr_value1             : 1; /* [0..0] dsp配置的gsm（m）模送给通道1的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GM_TCVR_VALUE1_T;

typedef union
{
    struct
    {
        unsigned int    gs_tcvr_value0             : 1; /* [0..0] dsp配置的gsm（s）模送给通道0 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_TCVR_VALUE0_T;

typedef union
{
    struct
    {
        unsigned int    gs_tcvr_value1             : 1; /* [0..0] dsp配置的gsm（s）模送给通道1 的tcvr_on值 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_GS_TCVR_VALUE1_T;

typedef union
{
    struct
    {
        unsigned int    l_tcvr_state0              : 1; /* [0..0] lte bbp输入的通道0 tcvr_on信号状态 */
        unsigned int    l_tcvr_state1              : 1; /* [1..1] lte bbp输入的通道1 tcvr_on信号状态 */
        unsigned int    t_tcvr_state0              : 1; /* [2..2] tds bbp输入的通道0 tcvr_on信号状态 */
        unsigned int    reserved_4                 : 1; /* [3..3] 保留 */
        unsigned int    w_tcvr_state0              : 1; /* [4..4] wcdma bbp输入的通道0 tcvr_on信号状态 */
        unsigned int    reserved_3                 : 1; /* [5..5] 保留 */
        unsigned int    gm_tcvr_state0             : 1; /* [6..6] gsm（m） bbp输入的通道0 tcvr_on信号状态 */
        unsigned int    reserved_2                 : 1; /* [7..7] 保留 */
        unsigned int    gs_tcvr_state0             : 1; /* [8..8] gsm（s） bbp输入的通道0 tcvr_on信号状态 */
        unsigned int    reserved_1                 : 7; /* [15..9] 保留 */
        unsigned int    ch_tcvr_state0             : 1; /* [16..16] 通道0 最终输出的tcvr_on信号状态 */
        unsigned int    ch_tcvr_state1             : 1; /* [17..17] 通道1 最终输出的tcvr_on信号状态 */
        unsigned int    reserved_0                 : 14; /* [31..18]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_RF_TCVR_STATE_T;

typedef union
{
    struct
    {
        unsigned int    mipi_unbind_en             : 1; /* [0..0] mipi通道不绑定使能：1'b0：通道绑定1'b1：通道不绑定 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI_UNBIND_EN_T;

typedef union
{
    struct
    {
        unsigned int    mipi_ch_sel0               : 1; /* [0..0] 通道0映射mipi器件关系：1'b0：通道0映射到mipi01'b1：通道0映射到mipi1 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI_CH_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    mipi_ch_sel1               : 1; /* [0..0] 通道1映射mipi器件关系：1'b0：通道1映射到mipi01'b1：通道1映射到mipi1 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_MIPI_CH_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    apt_ch_sel0                : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_APT_CH_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    apt_ch_sel1                : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_APT_CH_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    afc_ch_sel0                : 1; /* [0..0] afc pdm0信号来源：1'b0：来自于通道01'b1：来自于通道1 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_AFC_CH_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    afc_ch_sel1                : 1; /* [0..0] afc pdm1信号来源：1'b0：来自于通道01'b1：来自于通道1 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_AFC_CH_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    abb_tx_ch_sel0             : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_ABB_TX_CH_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    abb_tx_ch_sel1             : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_ABB_TX_CH_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    pmu_ch_sel0                : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_PMU_CH_SEL0_T;

typedef union
{
    struct
    {
        unsigned int    pmu_ch_sel1                : 1; /* [0..0] 保留 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_PMU_CH_SEL1_T;

typedef union
{
    struct
    {
        unsigned int    valid_num                  : 16; /* [15..0] 保留 */
        unsigned int    reserved                   : 16; /* [31..16]  */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_VALID_NUM_T;

typedef union
{
    struct
    {
        unsigned int    valid_num1                 : 16; /* [15..0] 保留 */
        unsigned int    valid_num2                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_VALID_NUM1_T;

typedef union
{
    struct
    {
        unsigned int    dbg_en                     : 1; /* [0..0] 可维可测使能信号，当dbg_rpt_mode为0时逻辑自清，否则需要写清。1'b0：可维可测功能不使能                             1'b1：可维可测功能使能 */
        unsigned int    dbg_rpt_mode               : 1; /* [1..1] 可维可测数据上报模式：'b0：触发后继续采样dbg_pkg_num个数据包后自动结束；'b1：只要dbg_en为1就上报，没有上报个数限制。 */
        unsigned int    reserved                   : 14; /* [15..2] 保留。 */
        unsigned int    dbg_pkg_num                : 16; /* [31..16] 可维可测的包个数 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_EN_T;

typedef union
{
    struct
    {
        unsigned int    dbg_id                     : 32; /* [31..0] 可维可测上报数据包ID号基址 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_ID_T;

typedef union
{
    struct
    {
        unsigned int    dbg_flt                    : 16; /* [15..0] 可维可测内部分组指示：1：上报ssi通道0数据2：上报ssi通道1数据3：上报mipi通道0数据4：上报mipi通道1数据其它：保留 */
        unsigned int    reserved                   : 16; /* [31..16] 保留。 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_DBG_FLT_T;

typedef union
{
    struct
    {
        unsigned int    bbc_rev0                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBC_REV0_T;

typedef union
{
    struct
    {
        unsigned int    bbc_rev1                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBC_REV1_T;

typedef union
{
    struct
    {
        unsigned int    bbc_rev2                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBC_REV2_T;

typedef union
{
    struct
    {
        unsigned int    bbc_rev3                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBC_REV3_T;

typedef union
{
    struct
    {
        unsigned int    bbc_rev4                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_CTU_BBC_REV4_T;


/********************************************************************************/
/*    bbp_ctu 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_ctu_c0_primary_mode_ind_c0_primary_mode_ind,c0_primary_mode_ind,HI_BBP_CTU_C0_PRIMARY_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_PRIMARY_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_primary_mode_ind_reserved,reserved,HI_BBP_CTU_C0_PRIMARY_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_PRIMARY_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_second_mode_ind_c0_second_mode_ind,c0_second_mode_ind,HI_BBP_CTU_C0_SECOND_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_SECOND_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_second_mode_ind_reserved,reserved,HI_BBP_CTU_C0_SECOND_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_SECOND_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc0_value_inter_rat_afc0_value_inter_rat,afc0_value_inter_rat,HI_BBP_CTU_AFC0_VALUE_INTER_RAT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC0_VALUE_INTER_RAT_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_afc_pdm_select_mask_c0_afc_pdm_select_mask,c0_afc_pdm_select_mask,HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_afc_pdm_select_mask_reserved,reserved,HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_AFC_PDM_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_ant_switch_pa_select_mask_c0_ant_switch_pa_select_mask,c0_ant_switch_pa_select_mask,HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_ant_switch_pa_select_mask_reserved,reserved,HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_ANT_SWITCH_PA_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_rfic_select_mask_c0_rfic_select_mask,c0_rfic_select_mask,HI_BBP_CTU_C0_RFIC_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_RFIC_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_rfic_select_mask_reserved,reserved,HI_BBP_CTU_C0_RFIC_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_RFIC_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_mipi_select_mask_c0_mipi_select_mask,c0_mipi_select_mask,HI_BBP_CTU_C0_MIPI_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_MIPI_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_mipi_select_mask_reserved,reserved,HI_BBP_CTU_C0_MIPI_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_MIPI_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_abb_select_mask_c0_abb_select_mask,c0_abb_select_mask,HI_BBP_CTU_C0_ABB_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_ABB_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_abb_select_mask_reserved,reserved,HI_BBP_CTU_C0_ABB_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_ABB_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_pmu_select_mask_c0_pmu_select_mask,c0_pmu_select_mask,HI_BBP_CTU_C0_PMU_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_PMU_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_pmu_select_mask_reserved,reserved,HI_BBP_CTU_C0_PMU_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_PMU_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_c0_sel_mask_dsp_c0_sel_mask,dsp_c0_sel_mask,HI_BBP_CTU_DSP_C0_SEL_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_C0_SEL_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_c0_sel_mask_reserved,reserved,HI_BBP_CTU_DSP_C0_SEL_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_C0_SEL_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_rfic0_rstn_dsp_rfic0_rstn,dsp_rfic0_rstn,HI_BBP_CTU_DSP_RFIC0_RSTN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_RFIC0_RSTN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_rfic0_rstn_reserved,reserved,HI_BBP_CTU_DSP_RFIC0_RSTN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_RFIC0_RSTN_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_second_afc_mode_c0_second_afc_mode,c0_second_afc_mode,HI_BBP_CTU_C0_SECOND_AFC_MODE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_SECOND_AFC_MODE_OFFSET)
HI_SET_GET(hi_bbp_ctu_c0_second_afc_mode_reserved,reserved,HI_BBP_CTU_C0_SECOND_AFC_MODE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C0_SECOND_AFC_MODE_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbp_sys_0control_bbp_sys_0control,bbp_sys_0control,HI_BBP_CTU_BBP_SYS_0CONTROL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBP_SYS_0CONTROL_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_overtime_flag_dsp_mipi0_rd_overtime_flag_dsp,mipi0_rd_overtime_flag_dsp,HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_overtime_flag_dsp_reserved,reserved,HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi0_rd_overtime_flag_dsp_ssi0_rd_overtime_flag_dsp,ssi0_rd_overtime_flag_dsp,HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi0_rd_overtime_flag_dsp_reserved,reserved,HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI0_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_overtime_clr_mipi0_rd_overtime_clr_imi,mipi0_rd_overtime_clr_imi,HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_overtime_clr_reserved,reserved,HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi0_rd_overtime_clr_ssi0_rd_overtime_clr_imi,ssi0_rd_overtime_clr_imi,HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi0_rd_overtime_clr_reserved,reserved,HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI0_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_1_rd_result_rfic0_1,rd_result_rfic0_1,HI_BBP_CTU_RD_RESULT_RFIC0_1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_2_rd_result_rfic0_2,rd_result_rfic0_2,HI_BBP_CTU_RD_RESULT_RFIC0_2_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_2_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_3_rd_result_rfic0_3,rd_result_rfic0_3,HI_BBP_CTU_RD_RESULT_RFIC0_3_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_3_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_4_rd_result_rfic0_4,rd_result_rfic0_4,HI_BBP_CTU_RD_RESULT_RFIC0_4_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_4_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_5_rd_result_rfic0_5,rd_result_rfic0_5,HI_BBP_CTU_RD_RESULT_RFIC0_5_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_5_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_6_rd_result_rfic0_6,rd_result_rfic0_6,HI_BBP_CTU_RD_RESULT_RFIC0_6_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_6_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_7_rd_result_rfic0_7,rd_result_rfic0_7,HI_BBP_CTU_RD_RESULT_RFIC0_7_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_7_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_8_rd_result_rfic0_8,rd_result_rfic0_8,HI_BBP_CTU_RD_RESULT_RFIC0_8_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_8_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_9_rd_result_rfic0_9,rd_result_rfic0_9,HI_BBP_CTU_RD_RESULT_RFIC0_9_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_9_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_10_rd_result_rfic0_10,rd_result_rfic0_10,HI_BBP_CTU_RD_RESULT_RFIC0_10_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_10_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_11_rd_result_rfic0_11,rd_result_rfic0_11,HI_BBP_CTU_RD_RESULT_RFIC0_11_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_11_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_12_rd_result_rfic0_12,rd_result_rfic0_12,HI_BBP_CTU_RD_RESULT_RFIC0_12_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_12_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_13_rd_result_rfic0_13,rd_result_rfic0_13,HI_BBP_CTU_RD_RESULT_RFIC0_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_14_rd_result_rfic0_14,rd_result_rfic0_14,HI_BBP_CTU_RD_RESULT_RFIC0_14_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_14_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_15_rd_result_rfic0_15,rd_result_rfic0_15,HI_BBP_CTU_RD_RESULT_RFIC0_15_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_15_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic0_16_rd_result_rfic0_16,rd_result_rfic0_16,HI_BBP_CTU_RD_RESULT_RFIC0_16_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC0_16_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic0_rd_result_flag_rfic0,rd_result_flag_rfic0,HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic0_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic0_clr_rd_result_flag_rfic0_clr,rd_result_flag_rfic0_clr,HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic0_clr_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC0_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_1_rd_result_mipi0_1,rd_result_mipi0_1,HI_BBP_CTU_RD_RESULT_MIPI0_1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_2_rd_result_mipi0_2,rd_result_mipi0_2,HI_BBP_CTU_RD_RESULT_MIPI0_2_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_2_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_3_rd_result_mipi0_3,rd_result_mipi0_3,HI_BBP_CTU_RD_RESULT_MIPI0_3_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_3_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_4_rd_result_mipi0_4,rd_result_mipi0_4,HI_BBP_CTU_RD_RESULT_MIPI0_4_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_4_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_5_rd_result_mipi0_5,rd_result_mipi0_5,HI_BBP_CTU_RD_RESULT_MIPI0_5_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_5_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_6_rd_result_mipi0_6,rd_result_mipi0_6,HI_BBP_CTU_RD_RESULT_MIPI0_6_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_6_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_7_rd_result_mipi0_7,rd_result_mipi0_7,HI_BBP_CTU_RD_RESULT_MIPI0_7_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_7_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_8_rd_result_mipi0_8,rd_result_mipi0_8,HI_BBP_CTU_RD_RESULT_MIPI0_8_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_8_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_9_rd_result_mipi0_9,rd_result_mipi0_9,HI_BBP_CTU_RD_RESULT_MIPI0_9_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_9_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_10_rd_result_mipi0_10,rd_result_mipi0_10,HI_BBP_CTU_RD_RESULT_MIPI0_10_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_10_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_11_rd_result_mipi0_11,rd_result_mipi0_11,HI_BBP_CTU_RD_RESULT_MIPI0_11_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_11_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_12_rd_result_mipi0_12,rd_result_mipi0_12,HI_BBP_CTU_RD_RESULT_MIPI0_12_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_12_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_13_rd_result_mipi0_13,rd_result_mipi0_13,HI_BBP_CTU_RD_RESULT_MIPI0_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_14_rd_result_mipi0_14,rd_result_mipi0_14,HI_BBP_CTU_RD_RESULT_MIPI0_14_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_14_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_15_rd_result_mipi0_15,rd_result_mipi0_15,HI_BBP_CTU_RD_RESULT_MIPI0_15_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_15_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi0_16_rd_result_mipi0_16,rd_result_mipi0_16,HI_BBP_CTU_RD_RESULT_MIPI0_16_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI0_16_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi0_rd_result_flag_mipi0,rd_result_flag_mipi0,HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi0_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi0_clr_rd_result_flag_mipi0_clr,rd_result_flag_mipi0_clr,HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi0_clr_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI0_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_data_low_soft_mipi0_rd_data_low_soft,mipi0_rd_data_low_soft,HI_BBP_CTU_MIPI0_RD_DATA_LOW_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_DATA_LOW_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_rd_data_high_soft_mipi0_rd_data_high_soft,mipi0_rd_data_high_soft,HI_BBP_CTU_MIPI0_RD_DATA_HIGH_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_RD_DATA_HIGH_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_end_flag_mipi0_soft_rd_end_flag_mipi0_soft,rd_end_flag_mipi0_soft,HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_end_flag_mipi0_soft_reserved,reserved,HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_END_FLAG_MIPI0_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_grant_dsp_mipi0_grant_dsp,mipi0_grant_dsp,HI_BBP_CTU_MIPI0_GRANT_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_GRANT_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi0_grant_dsp_reserved,reserved,HI_BBP_CTU_MIPI0_GRANT_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI0_GRANT_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_wdata_low_dsp_mipi0_wdata_low,dsp_mipi0_wdata_low,HI_BBP_CTU_DSP_MIPI0_WDATA_LOW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_WDATA_LOW_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_wdata_high_dsp_mipi0_wdata_high,dsp_mipi0_wdata_high,HI_BBP_CTU_DSP_MIPI0_WDATA_HIGH_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_WDATA_HIGH_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_en_imi_dsp_mipi0_en_imi,dsp_mipi0_en_imi,HI_BBP_CTU_DSP_MIPI0_EN_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_EN_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_en_imi_reserved,reserved,HI_BBP_CTU_DSP_MIPI0_EN_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_EN_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_cfg_ind_imi_dsp_mipi0_cfg_ind_imi,dsp_mipi0_cfg_ind_imi,HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_cfg_ind_imi_reserved,reserved,HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_CFG_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_rd_clr_dsp_mipi0_rd_clr,dsp_mipi0_rd_clr,HI_BBP_CTU_DSP_MIPI0_RD_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_RD_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi0_rd_clr_reserved,reserved,HI_BBP_CTU_DSP_MIPI0_RD_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI0_RD_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_line_control_cmd_sel,abb0_line_control_cmd_sel,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_mode_sel_cmd,abb0_mode_sel_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_tx_en_cmd,abb0_tx_en_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_rxa_en_cmd,abb0_rxa_en_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_rxb_en_cmd,abb0_rxb_en_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_blka_en_cmd,abb0_blka_en_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_abb0_blkb_en_cmd,abb0_blkb_en_cmd,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb0_line_control_cmd_reserved,reserved,HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB0_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_ch0_bbp_sel_ch0_bbp_sel,ch0_bbp_sel,HI_BBP_CTU_CH0_BBP_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CH0_BBP_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_ch0_bbp_sel_reserved,reserved,HI_BBP_CTU_CH0_BBP_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CH0_BBP_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_func_sel_cpu_mipi0_func_sel,cpu_mipi0_func_sel,HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_func_sel_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_FUNC_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_test_func_cpu_mipi0_test_func,cpu_mipi0_test_func,HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_test_func_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_TEST_FUNC_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_sclk_test_cpu_mipi0_sclk_test,cpu_mipi0_sclk_test,HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_sclk_test_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_SCLK_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_sdata_test_cpu_mipi0_sdata_test,cpu_mipi0_sdata_test,HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_sdata_test_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_SDATA_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_clr_ind_imi_cpu_mipi0_clr_ind_imi,cpu_mipi0_clr_ind_imi,HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_clr_ind_imi_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_CLR_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_fifo_clr_imi_cpu_mipi0_fifo_clr_imi,cpu_mipi0_fifo_clr_imi,HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi0_fifo_clr_imi_reserved,reserved,HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI0_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_ssi0_fifo_clr_imi_cpu_ssi0_fifo_clr_imi,cpu_ssi0_fifo_clr_imi,HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_ssi0_fifo_clr_imi_reserved,reserved,HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_SSI0_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_rx0_lte_ckg_bypass,cpu_rx0_lte_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_rx0_tds_ckg_bypass,cpu_rx0_tds_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_rx0_w_ckg_bypass,cpu_rx0_w_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_rx0_g1_ckg_bypass,cpu_rx0_g1_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_rx0_g2_ckg_bypass,cpu_rx0_g2_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_tx0_lte_ckg_bypass,cpu_tx0_lte_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_tx0_tds_ckg_bypass,cpu_tx0_tds_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_tx0_w_ckg_bypass,cpu_tx0_w_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_tx0_g1_ckg_bypass,cpu_tx0_g1_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_cpu_tx0_g2_ckg_bypass,cpu_tx0_g2_ckg_bypass,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx0_tx0_ckg_bypass_reserved,reserved,HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX0_TX0_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_primary_mode_ind_c1_primary_mode_ind,c1_primary_mode_ind,HI_BBP_CTU_C1_PRIMARY_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_PRIMARY_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_primary_mode_ind_reserved,reserved,HI_BBP_CTU_C1_PRIMARY_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_PRIMARY_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_second_mode_ind_c1_second_mode_ind,c1_second_mode_ind,HI_BBP_CTU_C1_SECOND_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_SECOND_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_second_mode_ind_reserved,reserved,HI_BBP_CTU_C1_SECOND_MODE_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_SECOND_MODE_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_ch_num_ind_c1_ch_num_ind,c1_ch_num_ind,HI_BBP_CTU_C1_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_ch_num_ind_c1_ch_num_ind_sel,c1_ch_num_ind_sel,HI_BBP_CTU_C1_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_ch_num_ind_reserved,reserved,HI_BBP_CTU_C1_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc1_value_inter_rat_afc1_value_inter_rat,afc1_value_inter_rat,HI_BBP_CTU_AFC1_VALUE_INTER_RAT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC1_VALUE_INTER_RAT_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_afc_pdm_select_mask_c1_afc_pdm_select_mask,c1_afc_pdm_select_mask,HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_afc_pdm_select_mask_reserved,reserved,HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_AFC_PDM_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_ant_switch_pa_select_mask_c1_ant_switch_pa_select_mask,c1_ant_switch_pa_select_mask,HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_ant_switch_pa_select_mask_reserved,reserved,HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_ANT_SWITCH_PA_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_rfic_select_mask_c1_rfic_select_mask,c1_rfic_select_mask,HI_BBP_CTU_C1_RFIC_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_RFIC_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_rfic_select_mask_reserved,reserved,HI_BBP_CTU_C1_RFIC_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_RFIC_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_mipi_select_mask_c1_mipi_select_mask,c1_mipi_select_mask,HI_BBP_CTU_C1_MIPI_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_MIPI_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_mipi_select_mask_reserved,reserved,HI_BBP_CTU_C1_MIPI_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_MIPI_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_abb_select_mask_c1_abb_select_mask,c1_abb_select_mask,HI_BBP_CTU_C1_ABB_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_ABB_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_abb_select_mask_reserved,reserved,HI_BBP_CTU_C1_ABB_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_ABB_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_pmu_select_mask_c1_pmu_select_mask,c1_pmu_select_mask,HI_BBP_CTU_C1_PMU_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_PMU_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_pmu_select_mask_reserved,reserved,HI_BBP_CTU_C1_PMU_SELECT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_PMU_SELECT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_c1_sel_mask_dsp_c1_sel_mask,dsp_c1_sel_mask,HI_BBP_CTU_DSP_C1_SEL_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_C1_SEL_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_c1_sel_mask_reserved,reserved,HI_BBP_CTU_DSP_C1_SEL_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_C1_SEL_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_rfic1_rstn_dsp_rfic1_rstn,dsp_rfic1_rstn,HI_BBP_CTU_DSP_RFIC1_RSTN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_RFIC1_RSTN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_rfic1_rstn_reserved,reserved,HI_BBP_CTU_DSP_RFIC1_RSTN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_RFIC1_RSTN_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_second_afc_mode_c1_second_afc_mode,c1_second_afc_mode,HI_BBP_CTU_C1_SECOND_AFC_MODE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_SECOND_AFC_MODE_OFFSET)
HI_SET_GET(hi_bbp_ctu_c1_second_afc_mode_reserved,reserved,HI_BBP_CTU_C1_SECOND_AFC_MODE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_C1_SECOND_AFC_MODE_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbp_sys_1control_bbp_sys_1control,bbp_sys_1control,HI_BBP_CTU_BBP_SYS_1CONTROL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBP_SYS_1CONTROL_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_overtime_flag_dsp_mipi1_rd_overtime_flag_dsp,mipi1_rd_overtime_flag_dsp,HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_overtime_flag_dsp_reserved,reserved,HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi1_rd_overtime_flag_dsp_ssi1_rd_overtime_flag_dsp,ssi1_rd_overtime_flag_dsp,HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi1_rd_overtime_flag_dsp_reserved,reserved,HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI1_RD_OVERTIME_FLAG_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_overtime_clr_mipi1_rd_overtime_clr_imi,mipi1_rd_overtime_clr_imi,HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_overtime_clr_reserved,reserved,HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi1_rd_overtime_clr_ssi1_rd_overtime_clr_imi,ssi1_rd_overtime_clr_imi,HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_ssi1_rd_overtime_clr_reserved,reserved,HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_SSI1_RD_OVERTIME_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_1_rd_result_rfic1_1,rd_result_rfic1_1,HI_BBP_CTU_RD_RESULT_RFIC1_1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_2_rd_result_rfic1_2,rd_result_rfic1_2,HI_BBP_CTU_RD_RESULT_RFIC1_2_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_2_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_3_rd_result_rfic1_3,rd_result_rfic1_3,HI_BBP_CTU_RD_RESULT_RFIC1_3_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_3_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_4_rd_result_rfic1_4,rd_result_rfic1_4,HI_BBP_CTU_RD_RESULT_RFIC1_4_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_4_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_5_rd_result_rfic1_5,rd_result_rfic1_5,HI_BBP_CTU_RD_RESULT_RFIC1_5_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_5_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_6_rd_result_rfic1_6,rd_result_rfic1_6,HI_BBP_CTU_RD_RESULT_RFIC1_6_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_6_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_7_rd_result_rfic1_7,rd_result_rfic1_7,HI_BBP_CTU_RD_RESULT_RFIC1_7_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_7_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_8_rd_result_rfic1_8,rd_result_rfic1_8,HI_BBP_CTU_RD_RESULT_RFIC1_8_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_8_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_9_rd_result_rfic1_9,rd_result_rfic1_9,HI_BBP_CTU_RD_RESULT_RFIC1_9_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_9_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_10_rd_result_rfic1_10,rd_result_rfic1_10,HI_BBP_CTU_RD_RESULT_RFIC1_10_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_10_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_11_rd_result_rfic1_11,rd_result_rfic1_11,HI_BBP_CTU_RD_RESULT_RFIC1_11_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_11_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_12_rd_result_rfic1_12,rd_result_rfic1_12,HI_BBP_CTU_RD_RESULT_RFIC1_12_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_12_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_13_rd_result_rfic1_13,rd_result_rfic1_13,HI_BBP_CTU_RD_RESULT_RFIC1_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_14_rd_result_rfic1_14,rd_result_rfic1_14,HI_BBP_CTU_RD_RESULT_RFIC1_14_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_14_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_15_rd_result_rfic1_15,rd_result_rfic1_15,HI_BBP_CTU_RD_RESULT_RFIC1_15_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_15_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_rfic1_16_rd_result_rfic1_16,rd_result_rfic1_16,HI_BBP_CTU_RD_RESULT_RFIC1_16_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_RFIC1_16_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic1_rd_result_flag_rfic1,rd_result_flag_rfic1,HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic1_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic1_clr_rd_result_flag_rfic1_clr,rd_result_flag_rfic1_clr,HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_rfic1_clr_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_RFIC1_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_1_rd_result_mipi1_1,rd_result_mipi1_1,HI_BBP_CTU_RD_RESULT_MIPI1_1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_2_rd_result_mipi1_2,rd_result_mipi1_2,HI_BBP_CTU_RD_RESULT_MIPI1_2_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_2_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_3_rd_result_mipi1_3,rd_result_mipi1_3,HI_BBP_CTU_RD_RESULT_MIPI1_3_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_3_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_4_rd_result_mipi1_4,rd_result_mipi1_4,HI_BBP_CTU_RD_RESULT_MIPI1_4_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_4_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_5_rd_result_mipi1_5,rd_result_mipi1_5,HI_BBP_CTU_RD_RESULT_MIPI1_5_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_5_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_6_rd_result_mipi1_6,rd_result_mipi1_6,HI_BBP_CTU_RD_RESULT_MIPI1_6_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_6_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_7_rd_result_mipi1_7,rd_result_mipi1_7,HI_BBP_CTU_RD_RESULT_MIPI1_7_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_7_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_8_rd_result_mipi1_8,rd_result_mipi1_8,HI_BBP_CTU_RD_RESULT_MIPI1_8_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_8_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_9_rd_result_mipi1_9,rd_result_mipi1_9,HI_BBP_CTU_RD_RESULT_MIPI1_9_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_9_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_10_rd_result_mipi1_10,rd_result_mipi1_10,HI_BBP_CTU_RD_RESULT_MIPI1_10_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_10_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_11_rd_result_mipi1_11,rd_result_mipi1_11,HI_BBP_CTU_RD_RESULT_MIPI1_11_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_11_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_12_rd_result_mipi1_12,rd_result_mipi1_12,HI_BBP_CTU_RD_RESULT_MIPI1_12_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_12_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_13_rd_result_mipi1_13,rd_result_mipi1_13,HI_BBP_CTU_RD_RESULT_MIPI1_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_14_rd_result_mipi1_14,rd_result_mipi1_14,HI_BBP_CTU_RD_RESULT_MIPI1_14_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_14_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_15_rd_result_mipi1_15,rd_result_mipi1_15,HI_BBP_CTU_RD_RESULT_MIPI1_15_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_15_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_mipi1_16_rd_result_mipi1_16,rd_result_mipi1_16,HI_BBP_CTU_RD_RESULT_MIPI1_16_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_MIPI1_16_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi1_rd_result_flag_mipi1,rd_result_flag_mipi1,HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi1_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi1_clr_rd_result_flag_mipi1_clr,rd_result_flag_mipi1_clr,HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_result_flag_mipi1_clr_reserved,reserved,HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_RESULT_FLAG_MIPI1_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_data_low_soft_mipi1_rd_data_low_soft,mipi1_rd_data_low_soft,HI_BBP_CTU_MIPI1_RD_DATA_LOW_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_DATA_LOW_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_rd_data_high_soft_mipi1_rd_data_high_soft,mipi1_rd_data_high_soft,HI_BBP_CTU_MIPI1_RD_DATA_HIGH_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_RD_DATA_HIGH_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_end_flag_mipi1_soft_rd_end_flag_mipi1_soft,rd_end_flag_mipi1_soft,HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_rd_end_flag_mipi1_soft_reserved,reserved,HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RD_END_FLAG_MIPI1_SOFT_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_grant_dsp_mipi1_grant_dsp,mipi1_grant_dsp,HI_BBP_CTU_MIPI1_GRANT_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_GRANT_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi1_grant_dsp_reserved,reserved,HI_BBP_CTU_MIPI1_GRANT_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI1_GRANT_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_wdata_low_dsp_mipi1_wdata_low,dsp_mipi1_wdata_low,HI_BBP_CTU_DSP_MIPI1_WDATA_LOW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_WDATA_LOW_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_wdata_high_dsp_mipi1_wdata_high,dsp_mipi1_wdata_high,HI_BBP_CTU_DSP_MIPI1_WDATA_HIGH_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_WDATA_HIGH_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_en_imi_dsp_mipi1_en_imi,dsp_mipi1_en_imi,HI_BBP_CTU_DSP_MIPI1_EN_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_EN_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_en_imi_reserved,reserved,HI_BBP_CTU_DSP_MIPI1_EN_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_EN_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_cfg_ind_imi_dsp_mipi1_cfg_ind_imi,dsp_mipi1_cfg_ind_imi,HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_cfg_ind_imi_reserved,reserved,HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_CFG_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_rd_clr_dsp_mipi1_rd_clr,dsp_mipi1_rd_clr,HI_BBP_CTU_DSP_MIPI1_RD_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_RD_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_mipi1_rd_clr_reserved,reserved,HI_BBP_CTU_DSP_MIPI1_RD_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_MIPI1_RD_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_line_control_cmd_sel,abb1_line_control_cmd_sel,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_mode_sel_cmd,abb1_mode_sel_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_tx_en_cmd,abb1_tx_en_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_rxa_en_cmd,abb1_rxa_en_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_rxb_en_cmd,abb1_rxb_en_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_blka_en_cmd,abb1_blka_en_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_abb1_blkb_en_cmd,abb1_blkb_en_cmd,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb1_line_control_cmd_reserved,reserved,HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB1_LINE_CONTROL_CMD_OFFSET)
HI_SET_GET(hi_bbp_ctu_ch1_bbp_sel_ch1_bbp_sel,ch1_bbp_sel,HI_BBP_CTU_CH1_BBP_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CH1_BBP_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_ch1_bbp_sel_reserved,reserved,HI_BBP_CTU_CH1_BBP_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CH1_BBP_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_func_sel_cpu_mipi1_func_sel,cpu_mipi1_func_sel,HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_func_sel_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_FUNC_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_test_func_cpu_mipi1_test_func,cpu_mipi1_test_func,HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_test_func_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_TEST_FUNC_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_sclk_test_cpu_mipi1_sclk_test,cpu_mipi1_sclk_test,HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_sclk_test_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_SCLK_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_sdata_test_cpu_mipi1_sdata_test,cpu_mipi1_sdata_test,HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_sdata_test_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_SDATA_TEST_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_clr_ind_imi_cpu_mipi1_clr_ind_imi,cpu_mipi1_clr_ind_imi,HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_clr_ind_imi_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_CLR_IND_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_fifo_clr_imi_cpu_mipi1_fifo_clr_imi,cpu_mipi1_fifo_clr_imi,HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_mipi1_fifo_clr_imi_reserved,reserved,HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_MIPI1_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_ssi1_fifo_clr_imi_cpu_ssi1_fifo_clr_imi,cpu_ssi1_fifo_clr_imi,HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_ssi1_fifo_clr_imi_reserved,reserved,HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_SSI1_FIFO_CLR_IMI_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_rx1_lte_ckg_bypass,cpu_rx1_lte_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_rx1_tds_ckg_bypass,cpu_rx1_tds_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_rx1_w_ckg_bypass,cpu_rx1_w_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_rx1_g1_ckg_bypass,cpu_rx1_g1_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_rx1_g2_ckg_bypass,cpu_rx1_g2_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_tx1_lte_ckg_bypass,cpu_tx1_lte_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_tx1_tds_ckg_bypass,cpu_tx1_tds_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_tx1_w_ckg_bypass,cpu_tx1_w_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_tx1_g1_ckg_bypass,cpu_tx1_g1_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_cpu_tx1_g2_ckg_bypass,cpu_tx1_g2_ckg_bypass,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_cpu_rx1_tx1_ckg_bypass_reserved,reserved,HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET)
HI_SET_GET(hi_bbp_ctu_delay_cnt_delay_cnt,delay_cnt,HI_BBP_CTU_DELAY_CNT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DELAY_CNT_OFFSET)
HI_SET_GET(hi_bbp_ctu_delay_cnt_reserved,reserved,HI_BBP_CTU_DELAY_CNT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DELAY_CNT_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_wait_cnt_int_wait_cnt,int_wait_cnt,HI_BBP_CTU_INT_WAIT_CNT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_WAIT_CNT_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_wait_cnt_reserved,reserved,HI_BBP_CTU_INT_WAIT_CNT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_WAIT_CNT_OFFSET)
HI_SET_GET(hi_bbp_ctu_ltebbp_input_mask_ltebbp_input_mask,ltebbp_input_mask,HI_BBP_CTU_LTEBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTEBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_ltebbp_input_mask_reserved,reserved,HI_BBP_CTU_LTEBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTEBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_input_mask_wbbp_input_mask,wbbp_input_mask,HI_BBP_CTU_WBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_input_mask_reserved,reserved,HI_BBP_CTU_WBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_gmbbp_input_mask_gmbbp_input_mask,gmbbp_input_mask,HI_BBP_CTU_GMBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GMBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_gmbbp_input_mask_reserved,reserved,HI_BBP_CTU_GMBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GMBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_gsbbp_input_mask_gsbbp_input_mask,gsbbp_input_mask,HI_BBP_CTU_GSBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GSBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_gsbbp_input_mask_reserved,reserved,HI_BBP_CTU_GSBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GSBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_tdsbbp_input_mask_tdsbbp_input_mask,tdsbbp_input_mask,HI_BBP_CTU_TDSBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDSBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_tdsbbp_input_mask_reserved,reserved,HI_BBP_CTU_TDSBBP_INPUT_MASK_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDSBBP_INPUT_MASK_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_mux_frame_num_lte_mux_frame_num,lte_mux_frame_num,HI_BBP_CTU_LTE_MUX_FRAME_NUM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_MUX_FRAME_NUM_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_mux_frame_num_tds_mux_frame_num,tds_mux_frame_num,HI_BBP_CTU_TDS_MUX_FRAME_NUM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_MUX_FRAME_NUM_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_mux_frame_num_w_mux_frame_num,w_mux_frame_num,HI_BBP_CTU_W_MUX_FRAME_NUM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_MUX_FRAME_NUM_OFFSET)
HI_SET_GET(hi_bbp_ctu_meas_flag_l_rd_meas_flag_l_rd,meas_flag_l_rd,HI_BBP_CTU_MEAS_FLAG_L_RD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MEAS_FLAG_L_RD_OFFSET)
HI_SET_GET(hi_bbp_ctu_meas_flag_w_rd_meas_flag_w_rd,meas_flag_w_rd,HI_BBP_CTU_MEAS_FLAG_W_RD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MEAS_FLAG_W_RD_OFFSET)
HI_SET_GET(hi_bbp_ctu_meas_flag_t_rd_meas_flag_t_rd,meas_flag_t_rd,HI_BBP_CTU_MEAS_FLAG_T_RD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MEAS_FLAG_T_RD_OFFSET)
HI_SET_GET(hi_bbp_ctu_meas_flag_gm_rd_meas_flag_gm_rd,meas_flag_gm_rd,HI_BBP_CTU_MEAS_FLAG_GM_RD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MEAS_FLAG_GM_RD_OFFSET)
HI_SET_GET(hi_bbp_ctu_meas_flag_gs_rd_meas_flag_gs_rd,meas_flag_gs_rd,HI_BBP_CTU_MEAS_FLAG_GS_RD_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MEAS_FLAG_GS_RD_OFFSET)
HI_SET_GET(hi_bbp_ctu_wake_flag_l_wr_wake_flag_l_wr,wake_flag_l_wr,HI_BBP_CTU_WAKE_FLAG_L_WR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WAKE_FLAG_L_WR_OFFSET)
HI_SET_GET(hi_bbp_ctu_wake_flag_w_wr_wake_flag_w_wr,wake_flag_w_wr,HI_BBP_CTU_WAKE_FLAG_W_WR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WAKE_FLAG_W_WR_OFFSET)
HI_SET_GET(hi_bbp_ctu_wake_flag_t_wr_wake_flag_t_wr,wake_flag_t_wr,HI_BBP_CTU_WAKE_FLAG_T_WR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WAKE_FLAG_T_WR_OFFSET)
HI_SET_GET(hi_bbp_ctu_wake_flag_gm_wr_wake_flag_gm_wr,wake_flag_gm_wr,HI_BBP_CTU_WAKE_FLAG_GM_WR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WAKE_FLAG_GM_WR_OFFSET)
HI_SET_GET(hi_bbp_ctu_wake_flag_gs_wr_wake_flag_gs_wr,wake_flag_gs_wr,HI_BBP_CTU_WAKE_FLAG_GS_WR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WAKE_FLAG_GS_WR_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_get_ind_timing_get_ind,timing_get_ind,HI_BBP_CTU_TIMING_GET_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_GET_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_get_ind_reserved,reserved,HI_BBP_CTU_TIMING_GET_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_GET_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_clear_timing_clear,timing_clear,HI_BBP_CTU_TIMING_CLEAR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_clear_reserved,reserved,HI_BBP_CTU_TIMING_CLEAR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_valid_o_timing_valid,o_timing_valid,HI_BBP_CTU_TIMING_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_timing_valid_reserved,reserved,HI_BBP_CTU_TIMING_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TIMING_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_timing_dsp_w_clk_cnt_dsp,w_clk_cnt_dsp,HI_BBP_CTU_W_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_timing_dsp_w_chip_cnt_dsp,w_chip_cnt_dsp,HI_BBP_CTU_W_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_timing_dsp_w_slot_cnt_dsp,w_slot_cnt_dsp,HI_BBP_CTU_W_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_timing_dsp_w_sfn_dsp,w_sfn_dsp,HI_BBP_CTU_W_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_fn_dsp_o_gm_fn_dsp,o_gm_fn_dsp,HI_BBP_CTU_GM_FN_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_FN_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_fn_dsp_reserved,reserved,HI_BBP_CTU_GM_FN_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_FN_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_qb_dsp_o_gm_qb_dsp,o_gm_qb_dsp,HI_BBP_CTU_GM_QB_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_QB_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_qb_dsp_reserved,reserved,HI_BBP_CTU_GM_QB_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_QB_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_fn_dsp_gs_fn_dsp,gs_fn_dsp,HI_BBP_CTU_GS_FN_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_FN_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_fn_dsp_reserved,reserved,HI_BBP_CTU_GS_FN_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_FN_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_qb_dsp_o_gs_qb_dsp,o_gs_qb_dsp,HI_BBP_CTU_GS_QB_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_QB_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_qb_dsp_reserved,reserved,HI_BBP_CTU_GS_QB_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_QB_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_timing_dsp_o_lte_sys_ts_cnt_dsp,o_lte_sys_ts_cnt_dsp,HI_BBP_CTU_LTE_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_timing_dsp_reserved_1,reserved_1,HI_BBP_CTU_LTE_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_timing_dsp_o_lte_sys_subfrm_cnt_dsp,o_lte_sys_subfrm_cnt_dsp,HI_BBP_CTU_LTE_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_timing_dsp_o_lte_sfn_dsp,o_lte_sfn_dsp,HI_BBP_CTU_LTE_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_timing_dsp_reserved_0,reserved_0,HI_BBP_CTU_LTE_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_timing1_dsp_o_tds_timing1_dsp,o_tds_timing1_dsp,HI_BBP_CTU_TDS_TIMING1_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_TIMING1_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_timing1_dsp_reserved,reserved,HI_BBP_CTU_TDS_TIMING1_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_TIMING1_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_timing2_dsp_o_tds_timing2_dsp,o_tds_timing2_dsp,HI_BBP_CTU_TDS_TIMING2_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_TIMING2_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_timing2_dsp_reserved,reserved,HI_BBP_CTU_TDS_TIMING2_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_TIMING2_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_timing_dsp_o_ctu_timing_dsp,o_ctu_timing_dsp,HI_BBP_CTU_CTU_TIMING_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_TIMING_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_basecnt1_dsp_o_ctu_basecnt1_dsp,o_ctu_basecnt1_dsp,HI_BBP_CTU_CTU_BASECNT1_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_BASECNT1_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_basecnt2_dsp_o_ctu_basecnt2_dsp,o_ctu_basecnt2_dsp,HI_BBP_CTU_CTU_BASECNT2_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_BASECNT2_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_basecnt_dsp_ctu_basecnt_dsp,ctu_basecnt_dsp,HI_BBP_CTU_CTU_BASECNT_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_BASECNT_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_ctu_timing_get_ind_dsp_ctu_timing_get_ind,dsp_ctu_timing_get_ind,HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_ctu_timing_get_ind_reserved,reserved,HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_CTU_TIMING_GET_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_timing_valid_dsp_ctu_timing_valid_dsp,ctu_timing_valid_dsp,HI_BBP_CTU_CTU_TIMING_VALID_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_TIMING_VALID_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_ctu_timing_valid_dsp_reserved,reserved,HI_BBP_CTU_CTU_TIMING_VALID_DSP_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_CTU_TIMING_VALID_DSP_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_ctu_timing_clr_dsp_ctu_timing_clr,dsp_ctu_timing_clr,HI_BBP_CTU_DSP_CTU_TIMING_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_CTU_TIMING_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_ctu_timing_clr_reserved,reserved,HI_BBP_CTU_DSP_CTU_TIMING_CLR_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_CTU_TIMING_CLR_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intrasys_valid_o_w_intrasys_valid,o_w_intrasys_valid,HI_BBP_CTU_W_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intrasys_valid_o_w_intrasys_valid_delay,o_w_intrasys_valid_delay,HI_BBP_CTU_W_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intrasys_valid_reserved,reserved,HI_BBP_CTU_W_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intrasys_valid_o_gm_intrasys_valid,o_gm_intrasys_valid,HI_BBP_CTU_GM_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intrasys_valid_o_gm_intrasys_valid_delay,o_gm_intrasys_valid_delay,HI_BBP_CTU_GM_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intrasys_valid_reserved,reserved,HI_BBP_CTU_GM_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intrasys_valid_o_gs_intrasys_valid,o_gs_intrasys_valid,HI_BBP_CTU_GS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intrasys_valid_o_gs_intrasys_valid_delay,o_gs_intrasys_valid_delay,HI_BBP_CTU_GS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intrasys_valid_reserved,reserved,HI_BBP_CTU_GS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intrasys_valid_o_lte_intrasys_valid,o_lte_intrasys_valid,HI_BBP_CTU_LTE_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intrasys_valid_o_lte_intrasys_valid_delay,o_lte_intrasys_valid_delay,HI_BBP_CTU_LTE_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intrasys_valid_reserved,reserved,HI_BBP_CTU_LTE_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intrasys_valid_o_tds_intrasys_valid,o_tds_intrasys_valid,HI_BBP_CTU_TDS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intrasys_valid_o_tds_intrasys_valid_delay,o_tds_intrasys_valid_delay,HI_BBP_CTU_TDS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intrasys_valid_reserved,reserved,HI_BBP_CTU_TDS_INTRASYS_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTRASYS_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_rfic_ck_cfg_cpu_rfic0_ck_cfg,cpu_rfic0_ck_cfg,HI_BBP_CTU_RFIC_CK_CFG_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RFIC_CK_CFG_OFFSET)
HI_SET_GET(hi_bbp_ctu_rfic_ck_cfg_cpu_rfic1_ck_cfg,cpu_rfic1_ck_cfg,HI_BBP_CTU_RFIC_CK_CFG_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RFIC_CK_CFG_OFFSET)
HI_SET_GET(hi_bbp_ctu_rfic_ck_cfg_reserved,reserved,HI_BBP_CTU_RFIC_CK_CFG_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RFIC_CK_CFG_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_waittime_dsp_mipi_wait_time,dsp_mipi_wait_time,HI_BBP_CTU_DSP_WAITTIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_WAITTIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_waittime_dsp_ssi_wait_time,dsp_ssi_wait_time,HI_BBP_CTU_DSP_WAITTIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_WAITTIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_dsp_waittime_reserved,reserved,HI_BBP_CTU_DSP_WAITTIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DSP_WAITTIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_w_ch_num_ind_s_w_ch_num_ind,s_w_ch_num_ind,HI_BBP_CTU_S_W_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_W_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_w_ch_num_ind_s_w_ch_num_ind_sel,s_w_ch_num_ind_sel,HI_BBP_CTU_S_W_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_W_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_w_ch_num_ind_reserved,reserved,HI_BBP_CTU_S_W_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_W_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intersys_measure_type_w_intersys_measure_type,w_intersys_measure_type,HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intersys_measure_type_reserved,reserved,HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_measure_report_valid_w_measure_report_valid,w_measure_report_valid,HI_BBP_CTU_W_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_measure_report_valid_reserved,reserved,HI_BBP_CTU_W_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_preint_offset_time_w_preint_offset_time,w_preint_offset_time,HI_BBP_CTU_W_PREINT_OFFSET_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_PREINT_OFFSET_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_intersys_measure_time_w_intersys_measure_time,w_intersys_measure_time,HI_BBP_CTU_W_INTERSYS_MEASURE_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_INTERSYS_MEASURE_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_lte_int_clear_w_rcv_lte,int_clear_w_rcv_lte,HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_tds_int_clear_w_rcv_tds,int_clear_w_rcv_tds,HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_gm_int_clear_w_rcv_gm,int_clear_w_rcv_gm,HI_BBP_CTU_INT_CLEAR_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_gs_int_clear_w_rcv_gs,int_clear_w_rcv_gs,HI_BBP_CTU_INT_CLEAR_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_w_int_clear_w_rcv_w,int_clear_w_rcv_w,HI_BBP_CTU_INT_CLEAR_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_rcv_w_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_int012_int_clear_w_int012,int_clear_w_int012,HI_BBP_CTU_INT_CLEAR_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_w_int012_reserved,reserved,HI_BBP_CTU_INT_CLEAR_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_lte_int_mask_w_rcv_lte,int_mask_w_rcv_lte,HI_BBP_CTU_INT_MASK_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_MASK_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_tds_int_mask_w_rcv_tds,int_mask_w_rcv_tds,HI_BBP_CTU_INT_MASK_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_MASK_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_gm_int_mask_w_rcv_gm,int_mask_w_rcv_gm,HI_BBP_CTU_INT_MASK_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_MASK_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_gs_int_mask_w_rcv_gs,int_mask_w_rcv_gs,HI_BBP_CTU_INT_MASK_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_MASK_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_w_int_mask_w_rcv_w,int_mask_w_rcv_w,HI_BBP_CTU_INT_MASK_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_rcv_w_reserved,reserved,HI_BBP_CTU_INT_MASK_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_int012_int_mask_w_int012,int_mask_w_int012,HI_BBP_CTU_INT_MASK_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_w_int012_reserved,reserved,HI_BBP_CTU_INT_MASK_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_lte_o_int_type_w_rcv_lte,o_int_type_w_rcv_lte,HI_BBP_CTU_INT_TYPE_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_tds_o_int_type_w_rcv_tds,o_int_type_w_rcv_tds,HI_BBP_CTU_INT_TYPE_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_gm_o_int_type_w_rcv_gm,o_int_type_w_rcv_gm,HI_BBP_CTU_INT_TYPE_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_gs_o_int_type_w_rcv_gs,o_int_type_w_rcv_gs,HI_BBP_CTU_INT_TYPE_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_w_o_int_type_w_rcv_w,o_int_type_w_rcv_w,HI_BBP_CTU_INT_TYPE_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_rcv_w_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_int012_o_int_type_w_int012,o_int_type_w_int012,HI_BBP_CTU_INT_TYPE_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_w_int012_reserved,reserved,HI_BBP_CTU_INT_TYPE_W_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_W_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_lte_int_13_reserved_1,reserved_1,HI_BBP_CTU_W_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_lte_int_13_w_snd_lte_int_13,w_snd_lte_int_13,HI_BBP_CTU_W_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_lte_int_13_reserved_0,reserved_0,HI_BBP_CTU_W_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_tds_int_13_reserved_1,reserved_1,HI_BBP_CTU_W_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_tds_int_13_w_snd_tds_int_13,w_snd_tds_int_13,HI_BBP_CTU_W_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_tds_int_13_reserved_0,reserved_0,HI_BBP_CTU_W_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gm_int_13_reserved_1,reserved_1,HI_BBP_CTU_W_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gm_int_13_w_snd_gm_int_13,w_snd_gm_int_13,HI_BBP_CTU_W_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gm_int_13_reserved_0,reserved_0,HI_BBP_CTU_W_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gs_int_13_reserved_1,reserved_1,HI_BBP_CTU_W_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gs_int_13_w_snd_gs_int_13,w_snd_gs_int_13,HI_BBP_CTU_W_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_gs_int_13_reserved_0,reserved_0,HI_BBP_CTU_W_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_w_int_13_reserved_1,reserved_1,HI_BBP_CTU_W_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_w_int_13_w_snd_w_int_13,w_snd_w_int_13,HI_BBP_CTU_W_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_snd_w_int_13_reserved_0,reserved_0,HI_BBP_CTU_W_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_lte_ch_num_ind_s_lte_ch_num_ind,s_lte_ch_num_ind,HI_BBP_CTU_S_LTE_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_LTE_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_lte_ch_num_ind_s_lte_ch_num_ind_sel,s_lte_ch_num_ind_sel,HI_BBP_CTU_S_LTE_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_LTE_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_lte_ch_num_ind_reserved,reserved,HI_BBP_CTU_S_LTE_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_LTE_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intersys_measure_type_lte_intersys_measure_type,lte_intersys_measure_type,HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intersys_measure_type_reserved,reserved,HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_measure_report_valid_lte_measure_report_valid,lte_measure_report_valid,HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_measure_report_valid_reserved,reserved,HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_preint_offset_time_lte_preint_offset_time,lte_preint_offset_time,HI_BBP_CTU_LTE_PREINT_OFFSET_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_PREINT_OFFSET_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_intersys_measure_time_lte_intersys_measure_time,lte_intersys_measure_time,HI_BBP_CTU_LTE_INTERSYS_MEASURE_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_INTERSYS_MEASURE_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_lte_int_clear_lte_rcv_lte,int_clear_lte_rcv_lte,HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_tds_int_clear_lte_rcv_tds,int_clear_lte_rcv_tds,HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_gm_int_clear_lte_rcv_gm,int_clear_lte_rcv_gm,HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_gs_int_clear_lte_rcv_gs,int_clear_lte_rcv_gs,HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_w_int_clear_lte_rcv_w,int_clear_lte_rcv_w,HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_rcv_w_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_int012_int_clear_lte_int012,int_clear_lte_int012,HI_BBP_CTU_INT_CLEAR_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_lte_int012_reserved,reserved,HI_BBP_CTU_INT_CLEAR_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_lte_int_mask_lte_rcv_lte,int_mask_lte_rcv_lte,HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_tds_int_mask_lte_rcv_tds,int_mask_lte_rcv_tds,HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_gm_int_mask_lte_rcv_gm,int_mask_lte_rcv_gm,HI_BBP_CTU_INT_MASK_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_gs_int_mask_lte_rcv_gs,int_mask_lte_rcv_gs,HI_BBP_CTU_INT_MASK_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_w_int_mask_lte_rcv_w,int_mask_lte_rcv_w,HI_BBP_CTU_INT_MASK_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_rcv_w_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_int012_int_mask_lte_int012,int_mask_lte_int012,HI_BBP_CTU_INT_MASK_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_lte_int012_reserved,reserved,HI_BBP_CTU_INT_MASK_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_w_o_int_type_lte_rcv_w,o_int_type_lte_rcv_w,HI_BBP_CTU_INT_TYPE_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_w_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_tds_o_int_type_lte_rcv_tds,o_int_type_lte_rcv_tds,HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_gm_o_int_type_lte_rcv_gm,o_int_type_lte_rcv_gm,HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_gs_o_int_type_lte_rcv_gs,o_int_type_lte_rcv_gs,HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_lte_o_int_type_lte_rcv_lte,o_int_type_lte_rcv_lte,HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_int012_o_int_type_lte_int012,o_int_type_lte_int012,HI_BBP_CTU_INT_TYPE_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_lte_int012_reserved,reserved,HI_BBP_CTU_INT_TYPE_LTE_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_LTE_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_lte_int_13_reserved_1,reserved_1,HI_BBP_CTU_LTE_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_lte_int_13_lte_snd_lte_int_13,lte_snd_lte_int_13,HI_BBP_CTU_LTE_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_lte_int_13_reserved_0,reserved_0,HI_BBP_CTU_LTE_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_tds_int_13_reserved_1,reserved_1,HI_BBP_CTU_LTE_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_tds_int_13_lte_snd_tds_int_13,lte_snd_tds_int_13,HI_BBP_CTU_LTE_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_tds_int_13_reserved_0,reserved_0,HI_BBP_CTU_LTE_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gm_int_13_reserved_1,reserved_1,HI_BBP_CTU_LTE_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gm_int_13_lte_snd_gm_int_13,lte_snd_gm_int_13,HI_BBP_CTU_LTE_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gm_int_13_reserved_0,reserved_0,HI_BBP_CTU_LTE_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gs_int_13_reserved_1,reserved_1,HI_BBP_CTU_LTE_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gs_int_13_lte_snd_gs_int_13,lte_snd_gs_int_13,HI_BBP_CTU_LTE_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_gs_int_13_reserved_0,reserved_0,HI_BBP_CTU_LTE_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_w_int_13_reserved_1,reserved_1,HI_BBP_CTU_LTE_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_w_int_13_lte_snd_w_int_13,lte_snd_w_int_13,HI_BBP_CTU_LTE_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_snd_w_int_13_reserved_0,reserved_0,HI_BBP_CTU_LTE_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_tds_ch_num_ind_s_tds_ch_num_ind,s_tds_ch_num_ind,HI_BBP_CTU_S_TDS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_TDS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_tds_ch_num_ind_s_tds_ch_num_ind_sel,s_tds_ch_num_ind_sel,HI_BBP_CTU_S_TDS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_TDS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_tds_ch_num_ind_reserved,reserved,HI_BBP_CTU_S_TDS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_TDS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intersys_measure_type_tds_intersys_measure_type,tds_intersys_measure_type,HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intersys_measure_type_reserved,reserved,HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_measure_report_valid_tds_measure_report_valid,tds_measure_report_valid,HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_measure_report_valid_reserved,reserved,HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_preint_offset_time_tds_preint_offset_time,tds_preint_offset_time,HI_BBP_CTU_TDS_PREINT_OFFSET_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_PREINT_OFFSET_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_intersys_measure_time_tds_intersys_measure_time,tds_intersys_measure_time,HI_BBP_CTU_TDS_INTERSYS_MEASURE_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_INTERSYS_MEASURE_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_lte_int_clear_tds_rcv_lte,int_clear_tds_rcv_lte,HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_tds_int_clear_tds_rcv_tds,int_clear_tds_rcv_tds,HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_gm_int_clear_tds_rcv_gm,int_clear_tds_rcv_gm,HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_gs_int_clear_tds_rcv_gs,int_clear_tds_rcv_gs,HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_w_int_clear_tds_rcv_w,int_clear_tds_rcv_w,HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_rcv_w_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_int012_int_clear_tds_int012,int_clear_tds_int012,HI_BBP_CTU_INT_CLEAR_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_tds_int012_reserved,reserved,HI_BBP_CTU_INT_CLEAR_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_lte_int_mask_tds_rcv_lte,int_mask_tds_rcv_lte,HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_tds_int_mask_tds_rcv_tds,int_mask_tds_rcv_tds,HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_gm_int_mask_tds_rcv_gm,int_mask_tds_rcv_gm,HI_BBP_CTU_INT_MASK_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_gs_int_mask_tds_rcv_gs,int_mask_tds_rcv_gs,HI_BBP_CTU_INT_MASK_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_w_int_mask_tds_rcv_w,int_mask_tds_rcv_w,HI_BBP_CTU_INT_MASK_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_rcv_w_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_int012_int_mask_tds_int012,int_mask_tds_int012,HI_BBP_CTU_INT_MASK_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_tds_int012_reserved,reserved,HI_BBP_CTU_INT_MASK_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_lte_o_int_type_tds_rcv_lte,o_int_type_tds_rcv_lte,HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_w_o_int_type_tds_rcv_w,o_int_type_tds_rcv_w,HI_BBP_CTU_INT_TYPE_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_w_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_gm_o_int_type_tds_rcv_gm,o_int_type_tds_rcv_gm,HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_gs_o_int_type_tds_rcv_gs,o_int_type_tds_rcv_gs,HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_tds_o_int_type_tds_rcv_tds,o_int_type_tds_rcv_tds,HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_int012_o_int_type_tds_int012,o_int_type_tds_int012,HI_BBP_CTU_INT_TYPE_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_tds_int012_reserved,reserved,HI_BBP_CTU_INT_TYPE_TDS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_TDS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_lte_int_13_reserved_1,reserved_1,HI_BBP_CTU_TDS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_lte_int_13_tds_snd_lte_int_13,tds_snd_lte_int_13,HI_BBP_CTU_TDS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_lte_int_13_reserved_0,reserved_0,HI_BBP_CTU_TDS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_tds_int_13_reserved_1,reserved_1,HI_BBP_CTU_TDS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_tds_int_13_tds_snd_tds_int_13,tds_snd_tds_int_13,HI_BBP_CTU_TDS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_tds_int_13_reserved_0,reserved_0,HI_BBP_CTU_TDS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gm_int_13_reserved_1,reserved_1,HI_BBP_CTU_TDS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gm_int_13_tds_snd_gm_int_13,tds_snd_gm_int_13,HI_BBP_CTU_TDS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gm_int_13_reserved_0,reserved_0,HI_BBP_CTU_TDS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gs_int_13_reserved_1,reserved_1,HI_BBP_CTU_TDS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gs_int_13_tds_snd_gs_int_13,tds_snd_gs_int_13,HI_BBP_CTU_TDS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_gs_int_13_reserved_0,reserved_0,HI_BBP_CTU_TDS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_w_int_13_reserved_1,reserved_1,HI_BBP_CTU_TDS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_w_int_13_tds_snd_w_int_13,tds_snd_w_int_13,HI_BBP_CTU_TDS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_snd_w_int_13_reserved_0,reserved_0,HI_BBP_CTU_TDS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gm_ch_num_ind_s_gm_ch_num_ind,s_gm_ch_num_ind,HI_BBP_CTU_S_GM_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GM_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gm_ch_num_ind_s_gm_ch_num_ind_sel,s_gm_ch_num_ind_sel,HI_BBP_CTU_S_GM_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GM_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gm_ch_num_ind_reserved,reserved,HI_BBP_CTU_S_GM_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GM_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intersys_measure_type_gm_intersys_measure_type,gm_intersys_measure_type,HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intersys_measure_type_reserved,reserved,HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_measure_report_valid_gm_measure_report_valid,gm_measure_report_valid,HI_BBP_CTU_GM_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_measure_report_valid_reserved,reserved,HI_BBP_CTU_GM_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_preint_offset_time_gm_preint_offset_time,gm_preint_offset_time,HI_BBP_CTU_GM_PREINT_OFFSET_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_PREINT_OFFSET_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_intersys_measure_time_gm_intersys_measure_time,gm_intersys_measure_time,HI_BBP_CTU_GM_INTERSYS_MEASURE_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_INTERSYS_MEASURE_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_lte_int_clear_gm_rcv_lte,int_clear_gm_rcv_lte,HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_tds_int_clear_gm_rcv_tds,int_clear_gm_rcv_tds,HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_gm_int_clear_gm_rcv_gm,int_clear_gm_rcv_gm,HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_gs_int_clear_gm_rcv_gs,int_clear_gm_rcv_gs,HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_w_int_clear_gm_rcv_w,int_clear_gm_rcv_w,HI_BBP_CTU_INT_CLEAR_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_rcv_w_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_int012_int_clear_gm_int012,int_clear_gm_int012,HI_BBP_CTU_INT_CLEAR_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gm_int012_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_lte_int_mask_gm_rcv_lte,int_mask_gm_rcv_lte,HI_BBP_CTU_INT_MASK_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_tds_int_mask_gm_rcv_tds,int_mask_gm_rcv_tds,HI_BBP_CTU_INT_MASK_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_gm_int_mask_gm_rcv_gm,int_mask_gm_rcv_gm,HI_BBP_CTU_INT_MASK_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_gs_int_mask_gm_rcv_gs,int_mask_gm_rcv_gs,HI_BBP_CTU_INT_MASK_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_w_int_mask_gm_rcv_w,int_mask_gm_rcv_w,HI_BBP_CTU_INT_MASK_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_rcv_w_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_int012_int_mask_gm_int012,int_mask_gm_int012,HI_BBP_CTU_INT_MASK_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gm_int012_reserved,reserved,HI_BBP_CTU_INT_MASK_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_lte_o_int_type_gm_rcv_lte,o_int_type_gm_rcv_lte,HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_tds_o_int_type_gm_rcv_tds,o_int_type_gm_rcv_tds,HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_w_o_int_type_gm_rcv_w,o_int_type_gm_rcv_w,HI_BBP_CTU_INT_TYPE_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_w_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_gs_o_int_type_gm_rcv_gs,o_int_type_gm_rcv_gs,HI_BBP_CTU_INT_TYPE_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_gm_o_int_type_gm_rcv_gm,o_int_type_gm_rcv_gm,HI_BBP_CTU_INT_TYPE_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_int012_o_int_type_gm_int012,o_int_type_gm_int012,HI_BBP_CTU_INT_TYPE_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gm_int012_reserved,reserved,HI_BBP_CTU_INT_TYPE_GM_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GM_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_lte_int_13_reserved_1,reserved_1,HI_BBP_CTU_GM_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_lte_int_13_gm_snd_lte_int_13,gm_snd_lte_int_13,HI_BBP_CTU_GM_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_lte_int_13_reserved_0,reserved_0,HI_BBP_CTU_GM_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_tds_int_13_reserved_1,reserved_1,HI_BBP_CTU_GM_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_tds_int_13_gm_snd_tds_int_13,gm_snd_tds_int_13,HI_BBP_CTU_GM_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_tds_int_13_reserved_0,reserved_0,HI_BBP_CTU_GM_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gm_int_13_reserved_1,reserved_1,HI_BBP_CTU_GM_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gm_int_13_gm_snd_gm_int_13,gm_snd_gm_int_13,HI_BBP_CTU_GM_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gm_int_13_reserved_0,reserved_0,HI_BBP_CTU_GM_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gs_int_13_reserved_1,reserved_1,HI_BBP_CTU_GM_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gs_int_13_gm_snd_gs_int_13,gm_snd_gs_int_13,HI_BBP_CTU_GM_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_gs_int_13_reserved_0,reserved_0,HI_BBP_CTU_GM_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_w_int_13_reserved_1,reserved_1,HI_BBP_CTU_GM_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_w_int_13_gm_snd_w_int_13,gm_snd_w_int_13,HI_BBP_CTU_GM_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_snd_w_int_13_reserved_0,reserved_0,HI_BBP_CTU_GM_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gs_ch_num_ind_s_gs_ch_num_ind,s_gs_ch_num_ind,HI_BBP_CTU_S_GS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gs_ch_num_ind_s_gs_ch_num_ind_sel,s_gs_ch_num_ind_sel,HI_BBP_CTU_S_GS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_s_gs_ch_num_ind_reserved,reserved,HI_BBP_CTU_S_GS_CH_NUM_IND_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_S_GS_CH_NUM_IND_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intersys_measure_type_gs_intersys_measure_type,gs_intersys_measure_type,HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intersys_measure_type_reserved,reserved,HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTERSYS_MEASURE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_measure_report_valid_gs_measure_report_valid,gs_measure_report_valid,HI_BBP_CTU_GS_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_measure_report_valid_reserved,reserved,HI_BBP_CTU_GS_MEASURE_REPORT_VALID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_MEASURE_REPORT_VALID_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_preint_offset_time_gs_preint_offset_time,gs_preint_offset_time,HI_BBP_CTU_GS_PREINT_OFFSET_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_PREINT_OFFSET_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_intersys_measure_time_gs_intersys_measure_time,gs_intersys_measure_time,HI_BBP_CTU_GS_INTERSYS_MEASURE_TIME_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_INTERSYS_MEASURE_TIME_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_lte_int_clear_gs_rcv_lte,int_clear_gs_rcv_lte,HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_tds_int_clear_gs_rcv_tds,int_clear_gs_rcv_tds,HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_gm_int_clear_gs_rcv_gm,int_clear_gs_rcv_gm,HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_gs_int_clear_gs_rcv_gs,int_clear_gs_rcv_gs,HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_w_int_clear_gs_rcv_w,int_clear_gs_rcv_w,HI_BBP_CTU_INT_CLEAR_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_rcv_w_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_int012_int_clear_gs_int012,int_clear_gs_int012,HI_BBP_CTU_INT_CLEAR_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_clear_gs_int012_reserved,reserved,HI_BBP_CTU_INT_CLEAR_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_CLEAR_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_lte_int_mask_gs_rcv_lte,int_mask_gs_rcv_lte,HI_BBP_CTU_INT_MASK_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_tds_int_mask_gs_rcv_tds,int_mask_gs_rcv_tds,HI_BBP_CTU_INT_MASK_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_gm_int_mask_gs_rcv_gm,int_mask_gs_rcv_gm,HI_BBP_CTU_INT_MASK_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_gs_int_mask_gs_rcv_gs,int_mask_gs_rcv_gs,HI_BBP_CTU_INT_MASK_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_w_int_mask_gs_rcv_w,int_mask_gs_rcv_w,HI_BBP_CTU_INT_MASK_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_rcv_w_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_int012_int_mask_gs_int012,int_mask_gs_int012,HI_BBP_CTU_INT_MASK_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_mask_gs_int012_reserved,reserved,HI_BBP_CTU_INT_MASK_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_MASK_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_lte_o_int_type_gs_rcv_lte,o_int_type_gs_rcv_lte,HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_lte_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_LTE_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_tds_o_int_type_gs_rcv_tds,o_int_type_gs_rcv_tds,HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_tds_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_TDS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_gm_o_int_type_gs_rcv_gm,o_int_type_gs_rcv_gm,HI_BBP_CTU_INT_TYPE_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_gm_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_RCV_GM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_GM_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_gs_o_int_type_gs_rcv_gs,o_int_type_gs_rcv_gs,HI_BBP_CTU_INT_TYPE_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_gs_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_RCV_GS_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_GS_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_w_o_int_type_gs_rcv_w,o_int_type_gs_rcv_w,HI_BBP_CTU_INT_TYPE_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_rcv_w_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_RCV_W_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_RCV_W_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_int012_o_int_type_gs_int012,o_int_type_gs_int012,HI_BBP_CTU_INT_TYPE_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_int_type_gs_int012_reserved,reserved,HI_BBP_CTU_INT_TYPE_GS_INT012_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_INT_TYPE_GS_INT012_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_lte_int_13_reserved_1,reserved_1,HI_BBP_CTU_GS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_lte_int_13_gs_snd_lte_int_13,gs_snd_lte_int_13,HI_BBP_CTU_GS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_lte_int_13_reserved_0,reserved_0,HI_BBP_CTU_GS_SND_LTE_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_LTE_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_tds_int_13_reserved_1,reserved_1,HI_BBP_CTU_GS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_tds_int_13_gs_snd_tds_int_13,gs_snd_tds_int_13,HI_BBP_CTU_GS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_tds_int_13_reserved_0,reserved_0,HI_BBP_CTU_GS_SND_TDS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_TDS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gm_int_13_reserved_1,reserved_1,HI_BBP_CTU_GS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gm_int_13_gs_snd_gm_int_13,gs_snd_gm_int_13,HI_BBP_CTU_GS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gm_int_13_reserved_0,reserved_0,HI_BBP_CTU_GS_SND_GM_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GM_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gs_int_13_reserved_1,reserved_1,HI_BBP_CTU_GS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gs_int_13_gs_snd_gs_int_13,gs_snd_gs_int_13,HI_BBP_CTU_GS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_gs_int_13_reserved_0,reserved_0,HI_BBP_CTU_GS_SND_GS_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_GS_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_w_int_13_reserved_1,reserved_1,HI_BBP_CTU_GS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_w_int_13_gs_snd_w_int_13,gs_snd_w_int_13,HI_BBP_CTU_GS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_snd_w_int_13_reserved_0,reserved_0,HI_BBP_CTU_GS_SND_W_INT_13_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_SND_W_INT_13_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp1_19m_sel_gbbp1_19m_sel,gbbp1_19m_sel,HI_BBP_CTU_GBBP1_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP1_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp1_19m_sel_reserved,reserved,HI_BBP_CTU_GBBP1_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP1_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp2_19m_sel_gbbp2_19m_sel,gbbp2_19m_sel,HI_BBP_CTU_GBBP2_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP2_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp2_19m_sel_reserved,reserved,HI_BBP_CTU_GBBP2_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP2_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_19m_sel_wbbp_19m_sel,wbbp_19m_sel,HI_BBP_CTU_WBBP_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_19m_sel_reserved,reserved,HI_BBP_CTU_WBBP_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_lbbp_19m_sel_lbbp_19m_sel,lbbp_19m_sel,HI_BBP_CTU_LBBP_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LBBP_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_lbbp_19m_sel_reserved,reserved,HI_BBP_CTU_LBBP_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LBBP_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_tbbp1_19m_sel_tbbp_19m_sel,tbbp_19m_sel,HI_BBP_CTU_TBBP1_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TBBP1_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_tbbp1_19m_sel_reserved,reserved,HI_BBP_CTU_TBBP1_19M_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TBBP1_19M_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp1_clk_sel_gbbp1_clk_sel,gbbp1_clk_sel,HI_BBP_CTU_GBBP1_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP1_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp1_clk_sel_reserved,reserved,HI_BBP_CTU_GBBP1_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP1_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp2_clk_sel_gbbp2_clk_sel,gbbp2_clk_sel,HI_BBP_CTU_GBBP2_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP2_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_gbbp2_clk_sel_reserved,reserved,HI_BBP_CTU_GBBP2_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GBBP2_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_clk_sel_wbbp_clk_sel,wbbp_clk_sel,HI_BBP_CTU_WBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_wbbp_clk_sel_reserved,reserved,HI_BBP_CTU_WBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_WBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_lbbp_clk_sel_lbbp_clk_sel,lbbp_clk_sel,HI_BBP_CTU_LBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_lbbp_clk_sel_reserved,reserved,HI_BBP_CTU_LBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_tbbp_clk_sel_tbbp_clk_sel,tbbp_clk_sel,HI_BBP_CTU_TBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_tbbp_clk_sel_reserved,reserved,HI_BBP_CTU_TBBP_CLK_SEL_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TBBP_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_reserved_1,reserved_1,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxb_iq_exchange,lte_rxb_iq_exchange,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxb_q_inv,lte_rxb_q_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxb_i_inv,lte_rxb_i_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxa_iq_exchange,lte_rxa_iq_exchange,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxa_q_inv,lte_rxa_q_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_rxa_i_inv,lte_rxa_i_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_tx_iq_exchange,lte_tx_iq_exchange,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_tx_q_inv,lte_tx_q_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_lte_tx_i_inv,lte_tx_i_inv,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_lte_abbif_fmt_reserved_0,reserved_0,HI_BBP_CTU_LTE_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_LTE_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_reserved_1,reserved_1,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxb_iq_exchange,w_rxb_iq_exchange,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxb_q_inv,w_rxb_q_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxb_i_inv,w_rxb_i_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxa_iq_exchange,w_rxa_iq_exchange,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxa_q_inv,w_rxa_q_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_rxa_i_inv,w_rxa_i_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_tx_iq_exchange,w_tx_iq_exchange,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_tx_q_inv,w_tx_q_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_w_tx_i_inv,w_tx_i_inv,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_abbif_fmt_reserved_0,reserved_0,HI_BBP_CTU_W_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_reserved_1,reserved_1,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxb_iq_exchange,tds_rxb_iq_exchange,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxb_q_inv,tds_rxb_q_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxb_i_inv,tds_rxb_i_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxa_iq_exchange,tds_rxa_iq_exchange,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxa_q_inv,tds_rxa_q_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_rxa_i_inv,tds_rxa_i_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_tx_iq_exchange,tds_tx_iq_exchange,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_tx_q_inv,tds_tx_q_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_tds_tx_i_inv,tds_tx_i_inv,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_tds_abbif_fmt_reserved_0,reserved_0,HI_BBP_CTU_TDS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_TDS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_reserved_1,reserved_1,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxb_iq_exchange,gm_rxb_iq_exchange,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxb_q_inv,gm_rxb_q_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxb_i_inv,gm_rxb_i_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxa_iq_exchange,gm_rxa_iq_exchange,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxa_q_inv,gm_rxa_q_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_rxa_i_inv,gm_rxa_i_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_tx_iq_exchange,gm_tx_iq_exchange,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_tx_q_inv,gm_tx_q_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_gm_tx_i_inv,gm_tx_i_inv,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_abbif_fmt_reserved_0,reserved_0,HI_BBP_CTU_GM_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_reserved_1,reserved_1,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxb_iq_exchange,gs_rxb_iq_exchange,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxb_q_inv,gs_rxb_q_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxb_i_inv,gs_rxb_i_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxa_iq_exchange,gs_rxa_iq_exchange,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxa_q_inv,gs_rxa_q_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_rxa_i_inv,gs_rxa_i_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_tx_iq_exchange,gs_tx_iq_exchange,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_tx_q_inv,gs_tx_q_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_gs_tx_i_inv,gs_tx_i_inv,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_abbif_fmt_reserved_0,reserved_0,HI_BBP_CTU_GS_ABBIF_FMT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_ABBIF_FMT_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg0_for_use_reg0_for_use,reg0_for_use,HI_BBP_CTU_REG0_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG0_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg1_for_use_reg1_for_use,reg1_for_use,HI_BBP_CTU_REG1_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG1_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg2_for_use_reg2_for_use,reg2_for_use,HI_BBP_CTU_REG2_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG2_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg3_for_use_reg3_for_use,reg3_for_use,HI_BBP_CTU_REG3_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG3_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg4_for_use_reg4_for_use,reg4_for_use,HI_BBP_CTU_REG4_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG4_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg5_for_use_reg5_for_use,reg5_for_use,HI_BBP_CTU_REG5_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG5_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg6_for_use_reg6_for_use,reg6_for_use,HI_BBP_CTU_REG6_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG6_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg7_for_use_reg7_for_use,reg7_for_use,HI_BBP_CTU_REG7_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG7_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg8_for_use_reg8_for_use,reg8_for_use,HI_BBP_CTU_REG8_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG8_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg9_for_use_reg9_for_use,reg9_for_use,HI_BBP_CTU_REG9_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG9_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg10_for_use_reg10_for_use,reg10_for_use,HI_BBP_CTU_REG10_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG10_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg11_for_use_reg11_for_use,reg11_for_use,HI_BBP_CTU_REG11_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG11_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg12_for_use_reg12_for_use,reg12_for_use,HI_BBP_CTU_REG12_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG12_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg13_for_use_reg13_for_use,reg13_for_use,HI_BBP_CTU_REG13_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG13_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg14_for_use_reg14_for_use,reg14_for_use,HI_BBP_CTU_REG14_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG14_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg15_for_use_reg15_for_use,reg15_for_use,HI_BBP_CTU_REG15_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG15_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg16_for_use_reg16_for_use,reg16_for_use,HI_BBP_CTU_REG16_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG16_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg17_for_use_reg17_for_use,reg17_for_use,HI_BBP_CTU_REG17_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG17_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg18_for_use_reg18_for_use,reg18_for_use,HI_BBP_CTU_REG18_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG18_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg19_for_use_reg19_for_use,reg19_for_use,HI_BBP_CTU_REG19_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG19_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg20_for_use_reg20_for_use,reg20_for_use,HI_BBP_CTU_REG20_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG20_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg21_for_use_reg21_for_use,reg21_for_use,HI_BBP_CTU_REG21_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG21_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg22_for_use_reg22_for_use,reg22_for_use,HI_BBP_CTU_REG22_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG22_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg23_for_use_reg23_for_use,reg23_for_use,HI_BBP_CTU_REG23_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG23_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg24_for_use_reg24_for_use,reg24_for_use,HI_BBP_CTU_REG24_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG24_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg25_for_use_reg25_for_use,reg25_for_use,HI_BBP_CTU_REG25_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG25_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg26_for_use_reg26_for_use,reg26_for_use,HI_BBP_CTU_REG26_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG26_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg27_for_use_reg27_for_use,reg27_for_use,HI_BBP_CTU_REG27_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG27_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg28_for_use_reg28_for_use,reg28_for_use,HI_BBP_CTU_REG28_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG28_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg29_for_use_reg29_for_use,reg29_for_use,HI_BBP_CTU_REG29_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG29_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg30_for_use_reg30_for_use,reg30_for_use,HI_BBP_CTU_REG30_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG30_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg31_for_use_reg31_for_use,reg31_for_use,HI_BBP_CTU_REG31_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG31_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg32_for_use_reg32_for_use,reg32_for_use,HI_BBP_CTU_REG32_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG32_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg33_for_use_reg33_for_use,reg33_for_use,HI_BBP_CTU_REG33_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG33_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg34_for_use_reg34_for_use,reg34_for_use,HI_BBP_CTU_REG34_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG34_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg35_for_use_reg35_for_use,reg35_for_use,HI_BBP_CTU_REG35_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG35_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg36_for_use_reg36_for_use,reg36_for_use,HI_BBP_CTU_REG36_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG36_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg37_for_use_reg37_for_use,reg37_for_use,HI_BBP_CTU_REG37_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG37_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg38_for_use_reg38_for_use,reg38_for_use,HI_BBP_CTU_REG38_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG38_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg39_for_use_reg39_for_use,reg39_for_use,HI_BBP_CTU_REG39_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG39_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg40_for_use_reg40_for_use,reg40_for_use,HI_BBP_CTU_REG40_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG40_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg41_for_use_reg41_for_use,reg41_for_use,HI_BBP_CTU_REG41_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG41_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg42_for_use_reg42_for_use,reg42_for_use,HI_BBP_CTU_REG42_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG42_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg43_for_use_reg43_for_use,reg43_for_use,HI_BBP_CTU_REG43_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG43_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg44_for_use_reg44_for_use,reg44_for_use,HI_BBP_CTU_REG44_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG44_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg45_for_use_reg45_for_use,reg45_for_use,HI_BBP_CTU_REG45_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG45_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg46_for_use_reg46_for_use,reg46_for_use,HI_BBP_CTU_REG46_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG46_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_reg47_for_use_reg47_for_use,reg47_for_use,HI_BBP_CTU_REG47_FOR_USE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_REG47_FOR_USE_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg0_cpu_dbg_reg0_cpu,dbg_reg0_cpu,HI_BBP_CTU_DBG_REG0_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG0_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg1_cpu_dbg_reg1_cpu,dbg_reg1_cpu,HI_BBP_CTU_DBG_REG1_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG1_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg2_cpu_dbg_reg2_cpu,dbg_reg2_cpu,HI_BBP_CTU_DBG_REG2_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG2_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg3_cpu_dbg_reg3_cpu,dbg_reg3_cpu,HI_BBP_CTU_DBG_REG3_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG3_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg4_cpu_dbg_reg4_cpu,dbg_reg4_cpu,HI_BBP_CTU_DBG_REG4_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG4_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg5_cpu_dbg_reg5_cpu,dbg_reg5_cpu,HI_BBP_CTU_DBG_REG5_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG5_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg6_cpu_dbg_reg6_cpu,dbg_reg6_cpu,HI_BBP_CTU_DBG_REG6_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG6_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg7_cpu_dbg_reg7_cpu,dbg_reg7_cpu,HI_BBP_CTU_DBG_REG7_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG7_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg8_cpu_dbg_reg8_cpu,dbg_reg8_cpu,HI_BBP_CTU_DBG_REG8_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG8_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg9_cpu_dbg_reg9_cpu,dbg_reg9_cpu,HI_BBP_CTU_DBG_REG9_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG9_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg10_cpu_dbg_reg10_cpu,dbg_reg10_cpu,HI_BBP_CTU_DBG_REG10_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG10_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg11_cpu_dbg_reg11_cpu,dbg_reg11_cpu,HI_BBP_CTU_DBG_REG11_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG11_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg12_cpu_dbg_reg12_cpu,dbg_reg12_cpu,HI_BBP_CTU_DBG_REG12_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG12_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg13_cpu_dbg_reg13_cpu,dbg_reg13_cpu,HI_BBP_CTU_DBG_REG13_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG13_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg14_cpu_dbg_reg14_cpu,dbg_reg14_cpu,HI_BBP_CTU_DBG_REG14_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG14_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg15_cpu_dbg_reg15_cpu,dbg_reg15_cpu,HI_BBP_CTU_DBG_REG15_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG15_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg16_cpu_dbg_reg16_cpu,dbg_reg16_cpu,HI_BBP_CTU_DBG_REG16_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG16_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg17_cpu_dbg_reg17_cpu,dbg_reg17_cpu,HI_BBP_CTU_DBG_REG17_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG17_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg18_cpu_dbg_reg18_cpu,dbg_reg18_cpu,HI_BBP_CTU_DBG_REG18_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG18_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg19_cpu_dbg_reg19_cpu,dbg_reg19_cpu,HI_BBP_CTU_DBG_REG19_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG19_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_clk_gate_en_dsp_dbg_clk_gate_en,dsp_dbg_clk_gate_en,HI_BBP_CTU_DBG_CLK_GATE_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_CLK_GATE_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_clk_gate_en_dsp_dbg_clk_bypass,dsp_dbg_clk_bypass,HI_BBP_CTU_DBG_CLK_GATE_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_CLK_GATE_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_clk_gate_en_reserved,reserved,HI_BBP_CTU_DBG_CLK_GATE_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_CLK_GATE_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2tds_busy_rf_ssi1,com2tds_busy_rf_ssi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2tds_busy_rf_ssi0,com2tds_busy_rf_ssi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2w_busy_rf_ssi1,com2w_busy_rf_ssi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2w_busy_rf_ssi0,com2w_busy_rf_ssi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2lte_busy_rf_ssi1,com2lte_busy_rf_ssi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2lte_busy_rf_ssi0,com2lte_busy_rf_ssi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g2_busy_rf_ssi1,com2g2_busy_rf_ssi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g2_busy_rf_ssi0,com2g2_busy_rf_ssi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g1_busy_rf_ssi1,com2g1_busy_rf_ssi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g1_busy_rf_ssi0,com2g1_busy_rf_ssi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2tds_busy_mipi1,com2tds_busy_mipi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2tds_busy_mipi0,com2tds_busy_mipi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2w_busy_mipi1,com2w_busy_mipi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2w_busy_mipi0,com2w_busy_mipi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2lte_busy_mipi1,com2lte_busy_mipi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2lte_busy_mipi0,com2lte_busy_mipi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g2_busy_mipi1,com2g2_busy_mipi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g2_busy_mipi0,com2g2_busy_mipi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g1_busy_mipi1,com2g1_busy_mipi1,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_com2g1_busy_mipi0,com2g1_busy_mipi0,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_reg20_cpu_reserved,reserved,HI_BBP_CTU_DBG_REG20_CPU_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_REG20_CPU_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_comm_en_dbg_comm_en,dbg_comm_en,HI_BBP_CTU_DBG_COMM_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_COMM_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_comm_en_reserved,reserved,HI_BBP_CTU_DBG_COMM_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_COMM_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_rf,l_ch_sw_rf,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_mipi,l_ch_sw_mipi,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_abb_tx,l_ch_sw_abb_tx,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_abb_rx,l_ch_sw_abb_rx,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_pmu,l_ch_sw_pmu,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_l_ch_sw_apt,l_ch_sw_apt,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_ch_sw_reserved,reserved,HI_BBP_CTU_L_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_rf,w_ch_sw_rf,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_mipi,w_ch_sw_mipi,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_abb_tx,w_ch_sw_abb_tx,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_abb_rx,w_ch_sw_abb_rx,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_pmu,w_ch_sw_pmu,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_w_ch_sw_apt,w_ch_sw_apt,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_ch_sw_reserved,reserved,HI_BBP_CTU_W_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_rf,t_ch_sw_rf,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_mipi,t_ch_sw_mipi,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_abb_tx,t_ch_sw_abb_tx,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_abb_rx,t_ch_sw_abb_rx,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_pmu,t_ch_sw_pmu,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_t_ch_sw_apt,t_ch_sw_apt,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_ch_sw_reserved,reserved,HI_BBP_CTU_T_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_rf,gm_ch_sw_rf,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_mipi,gm_ch_sw_mipi,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_abb_tx,gm_ch_sw_abb_tx,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_abb_rx,gm_ch_sw_abb_rx,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_pmu,gm_ch_sw_pmu,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_gm_ch_sw_apt,gm_ch_sw_apt,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_ch_sw_reserved,reserved,HI_BBP_CTU_GM_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_rf,gs_ch_sw_rf,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_mipi,gs_ch_sw_mipi,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_abb_tx,gs_ch_sw_abb_tx,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_abb_rx,gs_ch_sw_abb_rx,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_pmu,gs_ch_sw_pmu,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_gs_ch_sw_apt,gs_ch_sw_apt,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_ch_sw_reserved,reserved,HI_BBP_CTU_GS_CH_SW_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_CH_SW_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_sel0_l_tcvr_sel0,l_tcvr_sel0,HI_BBP_CTU_L_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_sel0_reserved,reserved,HI_BBP_CTU_L_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_sel1_l_tcvr_sel1,l_tcvr_sel1,HI_BBP_CTU_L_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_sel1_reserved,reserved,HI_BBP_CTU_L_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_sel0_t_tcvr_sel0,t_tcvr_sel0,HI_BBP_CTU_T_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_sel0_reserved,reserved,HI_BBP_CTU_T_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_sel1_t_tcvr_sel1,t_tcvr_sel1,HI_BBP_CTU_T_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_sel1_reserved,reserved,HI_BBP_CTU_T_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_sel0_w_tcvr_sel0,w_tcvr_sel0,HI_BBP_CTU_W_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_sel0_reserved,reserved,HI_BBP_CTU_W_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_sel1_w_tcvr_sel1,w_tcvr_sel1,HI_BBP_CTU_W_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_sel1_reserved,reserved,HI_BBP_CTU_W_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_sel0_gm_tcvr_sel0,gm_tcvr_sel0,HI_BBP_CTU_GM_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_sel0_reserved,reserved,HI_BBP_CTU_GM_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_sel1_gm_tcvr_sel1,gm_tcvr_sel1,HI_BBP_CTU_GM_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_sel1_reserved,reserved,HI_BBP_CTU_GM_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_sel0_gs_tcvr_sel0,gs_tcvr_sel0,HI_BBP_CTU_GS_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_sel0_reserved,reserved,HI_BBP_CTU_GS_TCVR_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_sel1_gs_tcvr_sel1,gs_tcvr_sel1,HI_BBP_CTU_GS_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_sel1_reserved,reserved,HI_BBP_CTU_GS_TCVR_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_value0_l_tcvr_value0,l_tcvr_value0,HI_BBP_CTU_L_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_value0_reserved,reserved,HI_BBP_CTU_L_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_value1_l_tcvr_value1,l_tcvr_value1,HI_BBP_CTU_L_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_l_tcvr_value1_reserved,reserved,HI_BBP_CTU_L_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_L_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_value0_t_tcvr_value0,t_tcvr_value0,HI_BBP_CTU_T_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_value0_reserved,reserved,HI_BBP_CTU_T_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_value1_t_tcvr_value1,t_tcvr_value1,HI_BBP_CTU_T_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_t_tcvr_value1_reserved,reserved,HI_BBP_CTU_T_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_T_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_value0_w_tcvr_value0,w_tcvr_value0,HI_BBP_CTU_W_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_value0_reserved,reserved,HI_BBP_CTU_W_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_value1_w_tcvr_value1,w_tcvr_value1,HI_BBP_CTU_W_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_w_tcvr_value1_reserved,reserved,HI_BBP_CTU_W_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_W_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_value0_gm_tcvr_value0,gm_tcvr_value0,HI_BBP_CTU_GM_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_value0_reserved,reserved,HI_BBP_CTU_GM_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_value1_gm_tcvr_value1,gm_tcvr_value1,HI_BBP_CTU_GM_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gm_tcvr_value1_reserved,reserved,HI_BBP_CTU_GM_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GM_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_value0_gs_tcvr_value0,gs_tcvr_value0,HI_BBP_CTU_GS_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_value0_reserved,reserved,HI_BBP_CTU_GS_TCVR_VALUE0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_VALUE0_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_value1_gs_tcvr_value1,gs_tcvr_value1,HI_BBP_CTU_GS_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_gs_tcvr_value1_reserved,reserved,HI_BBP_CTU_GS_TCVR_VALUE1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_GS_TCVR_VALUE1_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_l_tcvr_state0,l_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_l_tcvr_state1,l_tcvr_state1,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_t_tcvr_state0,t_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_reserved_4,reserved_4,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_w_tcvr_state0,w_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_reserved_3,reserved_3,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_gm_tcvr_state0,gm_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_reserved_2,reserved_2,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_gs_tcvr_state0,gs_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_reserved_1,reserved_1,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_ch_tcvr_state0,ch_tcvr_state0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_ch_tcvr_state1,ch_tcvr_state1,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_rf_tcvr_state_reserved_0,reserved_0,HI_BBP_CTU_RF_TCVR_STATE_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_RF_TCVR_STATE_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_unbind_en_mipi_unbind_en,mipi_unbind_en,HI_BBP_CTU_MIPI_UNBIND_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_UNBIND_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_unbind_en_reserved,reserved,HI_BBP_CTU_MIPI_UNBIND_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_UNBIND_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_ch_sel0_mipi_ch_sel0,mipi_ch_sel0,HI_BBP_CTU_MIPI_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_ch_sel0_reserved,reserved,HI_BBP_CTU_MIPI_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_ch_sel1_mipi_ch_sel1,mipi_ch_sel1,HI_BBP_CTU_MIPI_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_mipi_ch_sel1_reserved,reserved,HI_BBP_CTU_MIPI_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_MIPI_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_apt_ch_sel0_apt_ch_sel0,apt_ch_sel0,HI_BBP_CTU_APT_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_APT_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_apt_ch_sel0_reserved,reserved,HI_BBP_CTU_APT_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_APT_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_apt_ch_sel1_apt_ch_sel1,apt_ch_sel1,HI_BBP_CTU_APT_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_APT_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_apt_ch_sel1_reserved,reserved,HI_BBP_CTU_APT_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_APT_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc_ch_sel0_afc_ch_sel0,afc_ch_sel0,HI_BBP_CTU_AFC_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc_ch_sel0_reserved,reserved,HI_BBP_CTU_AFC_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc_ch_sel1_afc_ch_sel1,afc_ch_sel1,HI_BBP_CTU_AFC_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_afc_ch_sel1_reserved,reserved,HI_BBP_CTU_AFC_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_AFC_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb_tx_ch_sel0_abb_tx_ch_sel0,abb_tx_ch_sel0,HI_BBP_CTU_ABB_TX_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB_TX_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb_tx_ch_sel0_reserved,reserved,HI_BBP_CTU_ABB_TX_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB_TX_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb_tx_ch_sel1_abb_tx_ch_sel1,abb_tx_ch_sel1,HI_BBP_CTU_ABB_TX_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB_TX_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_abb_tx_ch_sel1_reserved,reserved,HI_BBP_CTU_ABB_TX_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_ABB_TX_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_pmu_ch_sel0_pmu_ch_sel0,pmu_ch_sel0,HI_BBP_CTU_PMU_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_PMU_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_pmu_ch_sel0_reserved,reserved,HI_BBP_CTU_PMU_CH_SEL0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_PMU_CH_SEL0_OFFSET)
HI_SET_GET(hi_bbp_ctu_pmu_ch_sel1_pmu_ch_sel1,pmu_ch_sel1,HI_BBP_CTU_PMU_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_PMU_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_pmu_ch_sel1_reserved,reserved,HI_BBP_CTU_PMU_CH_SEL1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_PMU_CH_SEL1_OFFSET)
HI_SET_GET(hi_bbp_ctu_valid_num_valid_num,valid_num,HI_BBP_CTU_VALID_NUM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_VALID_NUM_OFFSET)
HI_SET_GET(hi_bbp_ctu_valid_num_reserved,reserved,HI_BBP_CTU_VALID_NUM_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_VALID_NUM_OFFSET)
HI_SET_GET(hi_bbp_ctu_valid_num1_valid_num1,valid_num1,HI_BBP_CTU_VALID_NUM1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_VALID_NUM1_OFFSET)
HI_SET_GET(hi_bbp_ctu_valid_num1_valid_num2,valid_num2,HI_BBP_CTU_VALID_NUM1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_VALID_NUM1_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_en_dbg_en,dbg_en,HI_BBP_CTU_DBG_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_en_dbg_rpt_mode,dbg_rpt_mode,HI_BBP_CTU_DBG_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_en_reserved,reserved,HI_BBP_CTU_DBG_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_en_dbg_pkg_num,dbg_pkg_num,HI_BBP_CTU_DBG_EN_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_EN_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_id_dbg_id,dbg_id,HI_BBP_CTU_DBG_ID_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_ID_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_flt_dbg_flt,dbg_flt,HI_BBP_CTU_DBG_FLT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_FLT_OFFSET)
HI_SET_GET(hi_bbp_ctu_dbg_flt_reserved,reserved,HI_BBP_CTU_DBG_FLT_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_DBG_FLT_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbc_rev0_bbc_rev0,bbc_rev0,HI_BBP_CTU_BBC_REV0_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBC_REV0_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbc_rev1_bbc_rev1,bbc_rev1,HI_BBP_CTU_BBC_REV1_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBC_REV1_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbc_rev2_bbc_rev2,bbc_rev2,HI_BBP_CTU_BBC_REV2_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBC_REV2_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbc_rev3_bbc_rev3,bbc_rev3,HI_BBP_CTU_BBC_REV3_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBC_REV3_OFFSET)
HI_SET_GET(hi_bbp_ctu_bbc_rev4_bbc_rev4,bbc_rev4,HI_BBP_CTU_BBC_REV4_T,HI_BBP_CTU_BASE_ADDR, HI_BBP_CTU_BBC_REV4_OFFSET)

#endif // __HI_BBP_CTU_H__

